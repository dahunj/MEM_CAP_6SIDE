// AviHandler.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "CME8000.h"
#include "AviHandler.h"

#include "LogFile.h"
#include "Common.h"
#include "SequenceMain.h"
#include "WorkDlg.h"

#define UDP_AVI_LPORT	8050
#define UDP_AVI_HPORT	8050

IMPLEMENT_DYNAMIC(CAviHandler, CWnd)

CAviHandler g_objAviHandler;

CCriticalSection g_csAviHandler;	// Send_Command 문제 해결하기 위함

// CAviHandler

CAviHandler::CAviHandler()
{
	m_strRecvCmd = "";
	m_bOpened = FALSE;
	m_bConnected = FALSE;
	m_nStatusAvi = 0;	// 상태 (0:Not Ready, 1:Ready)
}

CAviHandler::~CAviHandler()
{
}

BEGIN_MESSAGE_MAP(CAviHandler, CWnd)
	ON_MESSAGE(UM_UDP_RECEIVE, OnUdpReceive)
END_MESSAGE_MAP()

// CAviHandlerer 메시지 처리기입니다.

BOOL CAviHandler::Initialize()
{
	if (m_bOpened) return TRUE;

	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

#ifdef AJIN_BOARD_USE
	m_bOpened = m_UdpAvi.Open_Socket(UDP_AVI_LPORT, UDP_AVI_HPORT, pEquipData->sAviIp, this);
	if (!m_bOpened) return FALSE;
#else
	m_bOpened = m_UdpAvi.Open_Socket(20001, 20000, "127.0.0.1", this);
	if (!m_bOpened) return FALSE;
#endif

	CString strLog;
	strLog.Format("Initialize : IP(%s)", pEquipData->sAviIp);
	g_objLogFile.Save_AviHandlerLog(strLog);

	Set_ConnectRequest();

	return TRUE;
}

void CAviHandler::Terminate()
{
	Set_ConnectEnd();
	m_bOpened = FALSE;
	m_UdpAvi.Close_Socket();
	g_objLogFile.Save_AviHandlerLog("Terminate");
}

///////////////////////////////////////////////////////////////////////////////
// UDP Socket Message

LRESULT CAviHandler::OnUdpReceive(WPARAM wLocalPort, LPARAM lParam)
{
	UINT nPort = (UINT)wLocalPort;
	BYTE byRecv[1024] = { 0 };
	CString strLog;

	int nLen = m_UdpAvi.Read_Socket(byRecv);

	if (nLen < 1) {
		strLog.Format("[C<-H] : Local Port (%d) Mismatch or Receive Data Zero (%d)", nPort, nLen);
		g_objLogFile.Save_AviHandlerLog(strLog);
		return 0;
	}

	CString strRecvSocket;
	strRecvSocket.Format("%s", byRecv);
	m_strRecvCmd += strRecvSocket;

	while (!m_strRecvCmd.IsEmpty()) {
		int nStart = m_strRecvCmd.Find("@");
		int nEnd = m_strRecvCmd.Find("\n");

		if (nEnd < 0) break;	// 버퍼에 들어오는 중...

		if (nStart < 0 || nStart > nEnd) {
			strLog.Format("[C<-H] : <<Error>> %s : Start(%d), End(%d)", m_strRecvCmd, nStart, nEnd);
			g_objLogFile.Save_AviHandlerLog(strLog);
			m_strRecvCmd.Delete(0, nEnd + 1);	// 쓰레기값이 채워져 있어서...
			continue;
		}

		CString strRecv = m_strRecvCmd.Mid(nStart + 1, nEnd - nStart - 1);
		m_strRecvCmd.Delete(0, nEnd + 1);

		

		char chSep = ',';
		CString strCmd, strOp;

		AfxExtractSubString(strCmd, strRecv, 0, chSep);
		AfxExtractSubString(strOp, strRecv, 1, chSep);

		EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();
		if (!pEquipData->bUseInlineMode && strCmd !="ALARM") return 0;

		// AviHandler Log /////////////////////////////////////////////////////
		strLog.Format("[C<-H] : %s", strRecv);
		g_objLogFile.Save_AviHandlerLog(strLog);
		///////////////////////////////////////////////////////////////////////

		CString strArg[5];
		for (int i = 0; i < 5; i++) AfxExtractSubString(strArg[i], strRecv, i + 2, chSep);

		if (strCmd == "CONNECT") {
			if (strOp == "REQUEST")	Get_ConnectRequest();
			if (strOp == "REPLY")	Get_ConnectReply();
			if (strOp == "END")		Get_ConnectEnd();

		} else if (strCmd == "STATUS") {
			if (strOp == "REQUEST")	Get_StatusRequest();
			if (strOp == "REPLY")	Get_StatusReply(strArg[0]);
			if (strOp == "UPDATE")	Get_StatusUpdate(strArg[0]);

		} else if (strCmd == "TIME") {
			if (strOp == "UPDATE")	Get_TimeUpdate(strArg[0]);

		} else if (strCmd == "LOT") {
			if (strOp == "START")	Get_LotStart(strArg[0], strArg[1], strArg[2], strArg[3], strArg[4]);
			if (strOp == "END")		Get_LotEnd(strArg[0], strArg[1], strArg[2], strArg[3], strArg[4], strArg[5]);

		} else if (strCmd == "TRAY") {
			if (strOp == "LOAD")	Get_TrayLoad(strArg[0], strArg[1], strArg[2], strArg[3]);
			if (strOp == "END")		Get_TrayEnd(strArg[0], strArg[1]);

		} else if (strCmd == "BARCODE") {
			if (strOp == "UPDATE")	Get_BarcodeUpdate(strArg[0], strArg[1], strArg[2], strArg[3]);

		} 
		else if (strCmd == "APD") 
		{
			if (strOp == "REQUEST")	Get_ApdRequest();
		} 
		else if (strCmd == "ALARM") 
		{
			if (strOp == "ON")	Get_VisionAlarmOn();
			if (strOp == "OFF")	Get_VIsionAlarmOff();
		} 
	}

	return 1;
}

///////////////////////////////////////////////////////////////////////////////
// Get Command

void CAviHandler::Get_ConnectRequest()
{
	m_bConnected = TRUE;
	Set_ConnectReply();
}

void CAviHandler::Get_ConnectReply()
{
	m_bConnected = TRUE;
}

void CAviHandler::Get_ConnectEnd()
{
	m_bConnected = FALSE;
}

void CAviHandler::Get_StatusRequest()
{
	BOOL bStatus = g_objSequenceMain.Is_MainThreadRun();
	if (bStatus) Set_StatusReply(1);
	else Set_StatusReply(0);
}

void CAviHandler::Get_StatusReply(CString sStatus)
{
	m_nStatusAvi = atoi(sStatus);
}

void CAviHandler::Get_StatusUpdate(CString sStatus)
{
	m_nStatusAvi = atoi(sStatus);
}

void CAviHandler::Get_LotStart(CString sLotID, CString sPortNo, CString sTrayCnt, CString sCmCnt, CString sModel)
{
	int nPx = atoi(sPortNo) - 1;	// Port Index

	gData.sLotID[nPx] = sLotID;
	gData.nTrayUseCount[nPx] = atoi(sTrayCnt);
	gData.nCmUseCount[nPx] = atoi(sCmCnt);	

	if (sModel != gData.sRecipe) 
	{
		if (sModel != "R63B" && sModel != "R64B") { AfxMessageBox("Invalid Model Name!!!"); return; }

		CIniFileCS INI(gsCurrentDir + "\\System\\EquipData.ini");
		if (!INI.Check_File()) { AfxMessageBox("EquipData.ini File Not Found!!!"); return; }

		INI.Set_String("EQUIPMENT", "MODEL", sModel);

		g_objDataManager.Read_EquipData();
		g_objDataManager.Read_MoveData();

		if (g_dlgWork.IsWindowVisible()) g_dlgWork.Change_Model();
	}
	
	g_dlgWork.Set_LotInfo(nPx);
}

void CAviHandler::Get_LotEnd(CString sLotId, CString sPortNo, CString sTotal, CString sOkCnt, CString sNgCnt, CString sBNgCnt)
{
	int nPNo = atoi(sPortNo);

	if (nPNo < 1 || nPNo > 5) { g_objCommon.Show_Error(7002); return; }
	if (sLotId != gData.sLotID[nPNo-1]) { g_objCommon.Show_Error(7003); return; }

	gData.nAviTotal[nPNo-1] = atoi(sTotal);
	gData.nAviOkCnt[nPNo-1] = atoi(sOkCnt);
	gData.nAviNgCnt[nPNo-1] = atoi(sNgCnt);
	gData.nAviBNgCnt[nPNo-1] = atoi(sBNgCnt);

	gData.bAviLotEnd[nPNo-1] = TRUE;
}

void CAviHandler::Get_TrayLoad(CString sLotID, CString sTrayNo, CString sCmCnt, CString sPortNo)
{
	int nPx = atoi(sPortNo) - 1;

	gData.sLotID[nPx] = sLotID;
	gData.nPNoAviPort = nPx + 1;
	gData.nTNoAviPort = atoi(sTrayNo);
	gData.nCmCntAviPort = atoi(sCmCnt);

	

	gData.bAviTrayLoad = TRUE;
}

void CAviHandler::Get_TrayEnd(CString sLastTrayNo, CString sPortNo)
{
	int nPx = atoi(sPortNo) - 1;
	gData.nLastTrayNo[nPx] = atoi(sLastTrayNo);
}

void CAviHandler::Get_BarcodeUpdate(CString sPortNo, CString sTrayNo, CString sCmNo, CString sBarcode)
{
	int nPx = atoi(sPortNo) - 1;
	int	nTx = atoi(sTrayNo) - 1;
	int	nCx = atoi(sCmNo) - 1;
	if (nTx < 0 || nTx > 99 || nCx < 0 || nCx > 99) { g_objCommon.Show_Error(6101); return; }

	gMes.sBarID[nPx][nTx][nCx] = sBarcode;

	if (sBarcode != "NOREAD") return;

	CString strLog;
	strLog.Format("[Barcode NoRead] LotId(%s), PortNo(%d), TrayNo(%d), CmNo(%d)", gLot.sLotID[nPx], nPx+1, nTx+1, nCx+1);
	g_objLogFile.Save_HandlerLog(strLog);
}

void CAviHandler::Get_ApdRequest()
{
	Set_ApdReply();
}

void CAviHandler::Get_VisionAlarmOn()
{
	CIniFileCS INI(gsCurrentDir + "\\System\\EquipData.ini");
	if (!INI.Check_File()) { AfxMessageBox("EquipData.ini File Not Found!!!"); return ; }
	
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	pEquipData->bUseInlineMode = TRUE;
	INI.Set_Bool("OPTION", "INLINE_MODE", pEquipData->bUseInlineMode);

	pEquipData->bUseVisionCmAlign = TRUE;
	INI.Set_Bool("OPTION", "VISION_CM_ALIGN", pEquipData->bUseVisionCmAlign);

	pEquipData->bUseVisionAlignAlarm = TRUE;
	INI.Set_Bool("OPTION","VISION_ALIGN_ALARM", pEquipData->bUseVisionAlignAlarm);

	g_objDataManager.Read_EquipData();
	g_objDataManager.Read_MoveData();
}

void CAviHandler::Get_VIsionAlarmOff()
{
	CIniFileCS INI(gsCurrentDir + "\\System\\EquipData.ini");
	if (!INI.Check_File()) { AfxMessageBox("EquipData.ini File Not Found!!!"); return ; }

	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	pEquipData->bUseInlineMode = TRUE;
	INI.Set_Bool("OPTION", "INLINE_MODE", pEquipData->bUseInlineMode);

	pEquipData->bUseVisionCmAlign = TRUE;
	INI.Set_Bool("OPTION", "VISION_CM_ALIGN", pEquipData->bUseVisionCmAlign);

	pEquipData->bUseVisionAlignAlarm = FALSE;
	INI.Set_Bool("OPTION","VISION_ALIGN_ALARM", pEquipData->bUseVisionAlignAlarm);
	g_objDataManager.Read_EquipData();
	g_objDataManager.Read_MoveData();
}

void CAviHandler::Get_TimeUpdate(CString sTime)
{
	// sTime Format : "2000-01-01 12:30:30 000"
	CString strTemp;
	SYSTEMTIME time;

	strTemp = sTime.Mid(0, 4);  time.wYear = atoi(strTemp);			// Year
	strTemp = sTime.Mid(5, 2);  time.wMonth = atoi(strTemp);		// Month
	strTemp = sTime.Mid(8, 2);  time.wDay = atoi(strTemp);			// Day
	strTemp = sTime.Mid(11, 2); time.wHour = atoi(strTemp);			// Hour
	strTemp = sTime.Mid(14, 2); time.wMinute = atoi(strTemp);		// Minite
	strTemp = sTime.Mid(17, 2); time.wSecond = atoi(strTemp);		// Second
	strTemp = sTime.Mid(20, 3); time.wMilliseconds = atoi(strTemp);	// MilliSecond

	SetLocalTime(&time);
}

/////////////////////////////////////////////////////////////////////////////
// Set Command

void CAviHandler::Set_ConnectRequest()
{
	CString	strSendCmd;
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	m_bConnected = FALSE;
	strSendCmd.Format("CONNECT,REQUEST,%s,%d", pEquipData->sAviIp, UDP_AVI_LPORT);
	Send_Command(strSendCmd);
}

void CAviHandler::Set_ConnectReply()
{
	CString	strSendCmd;
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	strSendCmd.Format("CONNECT,REPLY,%s,%d", pEquipData->sAviIp, UDP_AVI_LPORT);
	Send_Command(strSendCmd);
}

void CAviHandler::Set_ConnectEnd()
{
	CString	strSendCmd;
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	strSendCmd.Format("CONNECT,END,%s,%d", pEquipData->sAviIp, UDP_AVI_LPORT);
	Send_Command(strSendCmd);
	m_bConnected = FALSE;
}

void CAviHandler::Set_StatusRequest()
{
	m_nStatusAvi = 0;
	CString	strSendCmd;
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	strSendCmd.Format("STATUS,REQUEST,%s,%d", pEquipData->sAviIp, UDP_AVI_LPORT);
	Send_Command(strSendCmd);
}

void CAviHandler::Set_StatusReply(int nStatus)
{
	CString	strSendCmd;
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	strSendCmd.Format("STATUS,REPLY,%d,%s,%d", nStatus, pEquipData->sAviIp, UDP_AVI_LPORT);
	Send_Command(strSendCmd);
}

void CAviHandler::Set_StatusUpdate(int nStatus)
{
	CString	strSendCmd;
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	strSendCmd.Format("STATUS,UPDATE,%d,%s,%d", nStatus, pEquipData->sAviIp, UDP_AVI_LPORT);
	Send_Command(strSendCmd);
}

void CAviHandler::Set_TrayUnload()
{
	CString	strSendCmd;
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	strSendCmd.Format("TRAY,UNLOAD,%s,%d", pEquipData->sAviIp, UDP_AVI_LPORT);
	Send_Command(strSendCmd);

	gData.bAviTrayLoad = FALSE;
}

void CAviHandler::Set_ApdReply()
{
	CString	strSendCmd;
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	double dCapForceTotal = 0.0, dCapForceAvg = 0.0;
	for (int i = 0; i < PICK; i++) dCapForceTotal += gData.dAssyLoadCell[i];
	dCapForceAvg = dCapForceTotal / PICK;

	strSendCmd.Format("APD,REPLY,%0.3lf,%s,%d", dCapForceAvg, pEquipData->sAviIp, UDP_AVI_LPORT);
	Send_Command(strSendCmd);
}

/////////////////////////////////////////////////////////////////////////////
// UDP Socket Send Message

void CAviHandler::Send_Command(CString strSend)
{
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();
	if (!pEquipData->bUseInlineMode) return;

	CString strLog;
	strLog.Format("[C->H] : %s", strSend);
	g_objLogFile.Save_AviHandlerLog(strLog);

	g_csAviHandler.Lock();	// Critical Section

	CString strSendSocket;
	strSendSocket.Format("@%s\n", strSend);

	char chSend[1024] = { 0 };
	int nLength = strSendSocket.GetLength();
	memcpy(chSend, (LPSTR)(LPCSTR)strSendSocket, nLength);

	m_UdpAvi.Write_Socket((BYTE*)chSend, nLength);

	g_csAviHandler.Unlock();	// Critical Section
}

void CAviHandler::Set_NotifyCmAlignAlarm()
{
	CString	strSendCmd;
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	strSendCmd.Format("NOTIFY,ALARM,%s,%d", pEquipData->sAviIp, UDP_AVI_LPORT);
	Send_Command(strSendCmd);
}


/////////////////////////////////////////////////////////////////////////////
