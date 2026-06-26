// Handler.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "MesAgent.h"
#include "Handler.h"

#include "Common.h"
#include "LogFile.h"
#include "MesAgentDlg.h"
#include "Host.h"

IMPLEMENT_DYNAMIC(CHandler, CWnd)

CHandler g_objHandler;

// CHandler

CHandler::CHandler()
{
	m_bConnected = FALSE;
	m_strRecvCmd = "";
}

CHandler::~CHandler()
{
}

BEGIN_MESSAGE_MAP(CHandler, CWnd)
	ON_MESSAGE(UM_SERVER_ACCEPT, &CHandler::OnServerAccept)
	ON_MESSAGE(UM_SERVER_RECEIVE, &CHandler::OnServerReceive)
	ON_MESSAGE(UM_SERVER_REMOVE, &CHandler::OnServerRemove)
END_MESSAGE_MAP()

// CHandler 메시지 처리기입니다.

void CHandler::Initialize()
{
	m_nLPort = HANDLER_PORT;
	m_Server.Listen_Socket(m_nLPort, this);
}

void CHandler::Terminate()
{
	m_Server.Close_Socket();

	CString strLog = "Handler Disconnected.";
	g_objLogFile.Save_HandlerLog(strLog);

	CMesAgentDlg *pMainDlg = (CMesAgentDlg*)AfxGetMainWnd();
	pMainDlg->Set_HandlerConnect(FALSE);

	if (g_objHost.Is_Connected()) g_objHost.Set_S6F11_ControlState(2);	//1:Online, 2:Offline
}

/////////////////////////////////////////////////////////////////////////////

LRESULT CHandler::OnServerAccept(WPARAM wLocalPort, LPARAM lClientIdx)
{
	UINT nPort = (UINT)wLocalPort;
	int nClient = (int)lClientIdx;

	if (nPort != m_nLPort) { g_objLogFile.Save_HandlerLog("OnServerAccept - Local Port Mismatch"); return 0; }

	if (lClientIdx > 0) { for (int i = 0; i < m_Server.Get_ClientCount()-1; i++) m_Server.Close_Client(i); }
	m_bConnected = TRUE;

	CString strLog = "Handler Connected.";
	g_objLogFile.Save_HandlerLog(strLog);

	CMesAgentDlg *pMainDlg = (CMesAgentDlg*)AfxGetMainWnd();
	pMainDlg->Set_HandlerConnect(TRUE);

	if (g_objHost.Is_Connected()) g_objHost.Set_S6F11_ControlState(1);	//1:Online, 2:Offline

	return 0;
}

LRESULT CHandler::OnServerReceive(WPARAM wLocalPort, LPARAM lClientIdx)
{
	UINT nPort = (UINT)wLocalPort;
	int nClient = (int)lClientIdx;

	if (nPort != m_nLPort) { g_objLogFile.Save_HandlerLog("OnServerReceive - Local Port Mismatch"); return 0; }

	BYTE byRecv[8193] = { 0 };	// 마지막 0x00
	int nLen = m_Server.Read_Socket(0, byRecv);
	if (nLen < 1) { g_objLogFile.Save_HandlerLog("OnServerReceive - Data Zero"); return 0; }

	CString strRecvSocket, strLog;
	strRecvSocket.Format("%s", byRecv);
	m_strRecvCmd += strRecvSocket;

	CMesAgentDlg *pMainDlg = (CMesAgentDlg*)AfxGetMainWnd();
	while (!m_strRecvCmd.IsEmpty()) {
		int nStart = m_strRecvCmd.Find("@");
		int nEnd = m_strRecvCmd.Find("\n");

		if (nEnd < 0) break;	// 버퍼에 들어오는 중...

		if (nStart < 0 || nStart > nEnd) {
			strLog.Format("[OnServerReceive] <<Error>> - Start(%d), End(%d).\n%s", nStart, nEnd, m_strRecvCmd);
			g_objLogFile.Save_HandlerLog(strLog);
			m_strRecvCmd.Delete(0, nEnd + 1);	// 쓰레기값이 채워져 있어서...
			continue;
		}

		CString strRecv = m_strRecvCmd.Mid(nStart + 1, nEnd - nStart - 1);
		m_strRecvCmd.Delete(0, nEnd + 1);

		// Handler Log //////////////////////////////////////////////////////////////
		strLog.Format("[<-] %s", strRecv);
		g_objLogFile.Save_HandlerLog(strLog);

		pMainDlg->Set_HandlerMsg(strLog);

		char chSep = ',';
		CString strCmd, strOp;

		AfxExtractSubString(strCmd, strRecv, 0, chSep);
		AfxExtractSubString(strOp, strRecv, 1, chSep);

		CString strA[7];
		for (int i = 0; i < 7; i++) AfxExtractSubString(strA[i], strRecv, i + 2, chSep);

		if (strCmd == "OPER") {
			if (strOp == "UPDATE") Get_OperUpdate(strA[0]);

		} else if (strCmd == "CONTROL") {
			if (strOp == "STATE") Get_ControlState(strA[0], strA[1]);

		} else if (strCmd == "EQUIP") {
			if (strOp == "STATE") Get_EquipState(strA[0]);

		} else if (strCmd == "ERROR") {
			if (strOp == "UPDATE") Get_ErrorUpdate(strA[0], strA[1], strA[2]);

		} else if (strCmd == "LOT") {
			if (strOp == "READY")   Get_LotReady(strA[0]);
			if (strOp == "STARTED") Get_LotStarted(strA[0], strA[1]);
			if (strOp == "END")     Get_LotEnd(strA[0], strA[1], strA[2], strA[3], strA[4], strA[5]);
			if (strOp == "ABORT")   Get_LotAbort(strA[0], strA[1]);

		} else if (strCmd == "IDLE") {
			if (strOp == "SET")    Get_IdleSet(strA[0], strA[1]);
			if (strOp == "RESET")  Get_IdleReset(strA[0], strA[1]);
			if (strOp == "REPORT") Get_IdleReport(strA[0], strA[1], strA[2], strA[3], strA[4]);

		} else if (strCmd == "CM") {
			if (strOp == "END") Get_CmEnd(strA[0], strA[1], strA[2], strA[3], strA[4], strA[5]);

		} else if (strCmd == "LOTID") {	// Retest Lot-ID
			if (strOp == "REQUEST") {
				int nTotal = atoi(strA[4]);
				int nCount = atoi(strA[5]);
				if (nTotal < 1 || nTotal > 100 || nCount < 1 || nCount > 100) return 0;	// Error
				for (int i = 0; i < nCount; i++) AfxExtractSubString(gData.sReCmId[i], strRecv, i + 8, chSep);
				Get_LotIdRequest(strA[0], strA[1], strA[2], strA[3], nTotal, nCount);
			}

		} else if (strCmd == "CAPID") {		// Cap-ID
			if (strOp == "REQUEST")  Get_CapIdRequest(strA[0]);
			if (strOp == "COMPLETE") Get_CapIdComplete(strA[0]);

		} else if (strCmd == "SHIPID") {	// Ship-ID
			if (strOp == "REQUEST")  Get_ShipIdRequest(strA[0]);
			if (strOp == "COMPLETE") Get_ShipIdComplete(strA[0]);

		} else if ("RECIPE") {
			if (strOp == "SELECTED") Get_RecipeSelected(strA[0], strA[1]);
		}
	}

	return 0;
}

LRESULT CHandler::OnServerRemove(WPARAM wLocalPort, LPARAM lClientIdx)
{
	UINT nLocalPort = (UINT)wLocalPort;
	int nClient = (int)lClientIdx;

	if (nLocalPort != m_nLPort) return 0;

	m_bConnected = FALSE;

	CString strLog = "Handler Disconnected.";
	g_objLogFile.Save_HandlerLog(strLog);

	CMesAgentDlg *pMainDlg = (CMesAgentDlg*)AfxGetMainWnd();
	pMainDlg->Set_HandlerConnect(FALSE);

	if (g_objHost.Is_Connected()) g_objHost.Set_S6F11_ControlState(2);	//1:Online, 2:Offline

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Get Command

void CHandler::Get_OperUpdate(CString sOperId)
{
	gData.sOperId = sOperId;
}

void CHandler::Get_ControlState(CString sFlag, CString sOperId)
{
	int nState = atoi(sFlag);	// 1:Online, 2:Offline
	gData.sOperId = sOperId;
	if (!g_objHost.Is_Connected()) return;

	g_objHost.Set_S6F11_ControlState(nState);
}

void CHandler::Get_EquipState(CString sState)
{
	int nState = atoi(sState);	// 1:Run, 4:Idle, 5:Down
	g_objHost.Set_S6F11_EquipState(nState, "0", "0", "");
}

void CHandler::Get_ErrorUpdate(CString sFlag, CString sErrNo, CString sCategory)
{
	int nSet = atoi(sFlag);

	CString strErrFile, strErrMsg;
	strErrFile.Format("%s\\%s", gsCurrentDir, gData.sErrFile);
	CIniFileCS INI(strErrFile);
	if (!INI.Check_File()) { AfxMessageBox(strErrFile + " File Not Found!!!"); return; }

	gAlarm.nAlmSet = nSet;
	gAlarm.sAlmNo = sErrNo;
	gAlarm.sAlmCat = sCategory;
	gAlarm.sAlmMsg = INI.Get_String("ERROR", sErrNo, "");

	g_objHost.Set_S5F1_AlarmReport(nSet, sErrNo, gAlarm.sAlmMsg);
}

void CHandler::Get_LotReady(CString sLotId)
{
	gMes.sHostLotId = gMes.sHostRecipe = "";
	gMes.nHostCmCount = 0;
	g_objHost.Set_S6F11_LotReady(sLotId);
}

void CHandler::Get_LotStarted(CString sLotId, CString sCmCnt)
{
	int nCount = atoi(sCmCnt);
	g_objHost.Set_S6F11_LotStarted(sLotId, nCount);
}

void CHandler::Get_LotEnd(CString sLotId, CString sRecipe, CString sCount, CString sOk, CString sNg, CString sBNg)
{
	int nCnt = atoi(sCount);
	int nOk = atoi(sOk);
	int nNg = atoi(sNg);
	int nBNg = atoi(sBNg);
	g_objHost.Set_S6F11_LotEnd(sLotId, sRecipe, nCnt, nOk, nNg, nBNg);
}

void CHandler::Get_LotAbort(CString sLotId, CString sRecipe)
{
	g_objHost.Set_S6F11_LotAbort(sLotId, sRecipe);
}

void CHandler::Get_IdleSet(CString sOperId, CString sCode)
{
	gData.sOperId = sOperId;
	gIdle.sIdleCode = sCode;
	g_objHost.Set_S6F11_IdleSet();
}

void CHandler::Get_IdleReset(CString sOperId, CString sCode)
{
	gData.sOperId = sOperId;
	gIdle.sIdleCode = sCode;
	g_objHost.Set_S6F11_IdleReset();
}

void CHandler::Get_IdleReport(CString sOperId, CString sCode, CString sText, CString sSTime, CString sETime)
{
	gData.sOperId = sOperId;
	gIdle.sIdleCode = sCode;
	gIdle.sIdleText = sText;
	gIdle.sIdleSTime = sSTime;
	gIdle.sIdleETime = sETime;
	g_objHost.Set_S6F11_IdleReport();
}

void CHandler::Get_CmEnd(CString sLotId, CString sTray, CString sPocket, CString sResult, CString sNgCode, CString sCmId)
{
	int nTray = atoi(sTray);
	int nPocket = atoi(sPocket);
	g_objHost.Set_S6F11_CmEnd(sLotId, nTray, nPocket, sResult, sNgCode, sCmId);
}

void CHandler::Get_LotIdRequest(CString sSite, CString sEqNo, CString sLabel, CString sRtstId, int nTotal, int nCount)
{
	
}

void CHandler::Get_CapIdRequest(CString sCapId)
{
	g_objHost.Set_S6F11_MaterialReport(1, sCapId);
}

void CHandler::Get_ShipIdRequest(CString sShipId)
{
	g_objHost.Set_S6F11_MaterialReport(2, sShipId);
}

void CHandler::Get_CapIdComplete(CString sCapId)
{
	g_objHost.Set_S6F11_MaterialComplete(1, sCapId);
}

void CHandler::Get_ShipIdComplete(CString sShipId)
{
	g_objHost.Set_S6F11_MaterialComplete(2, sShipId);
}

void CHandler::Get_RecipeSelected(CString sLotId, CString sRecipe)
{
	g_objHost.Set_S6F11_PPSelected(sLotId, sRecipe);
}

///////////////////////////////////////////////////////////////////////////////
// Set Command

void CHandler::Set_ControlState(int nFlag)
{
	CString strSend;
	strSend.Format("CONTROL,STATE,%d", nFlag);	// 1:Online, 2:Offline
	Send_Command(strSend);
}

void CHandler::Set_ErrorReply()
{
	CString strSend;
	strSend.Format("ERROR,REPLY");
	Send_Command(strSend);
}

void CHandler::Set_LotStart(CString sLotId, CString sRecipe, int nCmCnt)
{
	CString strSend;
	strSend.Format("LOT,START,%s,%s,%d", sLotId, sRecipe, nCmCnt);
	Send_Command(strSend);
}

void CHandler::Set_LotIdFail(CString sLotId, CString sRtstId, CString sLabel, CString sCode, CString sText)
{
	CString strSend;
	strSend.Format("LOTID,FAIL,%s,%s,%s,%s,%s", sLotId, sRtstId, sLabel, sCode, sText);
	Send_Command(strSend);
}

void CHandler::Set_LotIdSucess(CString sLotId, CString sRecipe, int nCmCount, CString sRtstId, CString sLabel)
{
	CString strSend;
	strSend.Format("LOTID,SUCESS,%s,%s,%d,%s,%s", sLotId, sRecipe, nCmCount, sRtstId, sLabel);
	Send_Command(strSend);
}

void CHandler::Set_CapIdSucess(CString sLotId)
{
	CString strSend;
	strSend.Format("CAPID,SUCESS,%s", sLotId);
	Send_Command(strSend);
}

void CHandler::Set_ShipIdSucess(CString sLotId)
{
	CString strSend;
	strSend.Format("SHIPID,SUCESS,%s", sLotId);
	Send_Command(strSend);
}

void CHandler::Set_CapIdFail(CString sLotId, CString sCode, CString sText)
{
	CString strSend;
	strSend.Format("CAPID,FAIL,%s,%s,%s", sLotId, sCode, sText);
	Send_Command(strSend);
}

void CHandler::Set_ShipIdFail(CString sLotId, CString sCode, CString sText)
{
	CString strSend;
	strSend.Format("SHIPID,FAIL,%s,%s,%s", sLotId, sCode, sText);
	Send_Command(strSend);
}

void CHandler::Set_RecipeSelect(CString sLotId, CString sRecipe)
{
	CString strSend;
	strSend.Format("RECIPE,SELECT,%s,%s", sLotId, sRecipe);
	Send_Command(strSend);
}

void CHandler::Set_TerminalDisplay(CString sDisplay)
{
	CString strSend;
	strSend.Format("TERMINAL,DISPLAY,%s", sDisplay);
	Send_Command(strSend);
}

void CHandler::Set_TimeSync()
{
	CString strSend;
	strSend.Format("TIME,UPDATE");
	Send_Command(strSend);
}

///////////////////////////////////////////////////////////////////////////////

void CHandler::Send_Command(CString sSend)
{
	CString strLog, strMsg, strSendSocket;

	strSendSocket.Format("@%s\n", sSend);

	char chSend[8192] = { 0 };
	int nLength = strSendSocket.GetLength();
	memcpy(chSend, (LPSTR)(LPCSTR)strSendSocket, nLength);

	if (!m_Server.Write_Socket(0, (BYTE*)chSend, nLength)) return;

	// Handler Log ////////////////////////////////////////////////////////////
	strLog.Format("[->] %s", sSend);
	g_objLogFile.Save_HandlerLog(strLog);

	strMsg.Format("[->] %s", sSend);
	CMesAgentDlg *pMainDlg = (CMesAgentDlg*)AfxGetMainWnd();
	pMainDlg->Set_HandlerMsg(strMsg);
}

///////////////////////////////////////////////////////////////////////////////
