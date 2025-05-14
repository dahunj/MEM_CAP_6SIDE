// Inspector.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "CME8000.h"
#include "Inspector.h"

#include "LogFile.h"
#include "Common.h"
#include "DataManager.h"

#include "SequenceMain.h"
#include "MESInterface.h"
#include "AviHandler.h"

#define UDP_VPC_HOST_IP	"127.0.0.1"

#define UDP_VPC_LPORT	8000
#define UDP_VPC_HPORT	8001

IMPLEMENT_DYNAMIC(CInspector, CWnd)

CInspector g_objInspector;

CCriticalSection g_csInspector;	// Send_Command 문제 해결하기 위함

// CInspector

CInspector::CInspector()
{
	m_strRecvCmd = "";
	m_nStatusVPC = 0;		// Vision 상태 (0:Not Ready, 1:Ready)
	m_bLotReady = FALSE;
}

CInspector::~CInspector()
{
}

BEGIN_MESSAGE_MAP(CInspector, CWnd)
	ON_WM_TIMER()
	ON_MESSAGE(UM_UDP_RECEIVE, OnUdpReceive)
END_MESSAGE_MAP()

// CInspector 메시지 처리기입니다.

void CInspector::Initialize()
{
	BOOL bOpened = m_UdpVisionPC.Open_Socket(UDP_VPC_LPORT, UDP_VPC_HPORT, UDP_VPC_HOST_IP, this);
	if (bOpened) Set_ConnectRequest();
}

void CInspector::Terminate()
{
	Set_ConnectEnd();
	m_UdpVisionPC.Close_Socket();
}

///////////////////////////////////////////////////////////////////////////////
// UDP Socket Message

void CInspector::OnTimer(UINT_PTR nIDEvent)
{
	KillTimer(nIDEvent);
	m_nStatusVPC = 0;

	CWnd::OnTimer(nIDEvent);
}

LRESULT CInspector::OnUdpReceive(WPARAM wLocalPort, LPARAM lParam)
{
	UINT nPort = (UINT)wLocalPort;
	BYTE byRecv[1024] = { 0 };
	CString strLog;

	int nLen = m_UdpVisionPC.Read_Socket(byRecv);

	if (nLen < 1) {
		strLog.Format("[H<-V] : Local Port (%d) Mismatch or Receive Data Zero (%d)", nPort, nLen);
		g_objLogFile.Save_InspectorLog(strLog);
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
			strLog.Format("[H<-V] : <<Error>> %s : Start(%d), End(%d)", m_strRecvCmd, nStart, nEnd);
			g_objLogFile.Save_InspectorLog(strLog);
			m_strRecvCmd.Delete(0, nEnd + 1);	// 쓰레기값이 채워져 있어서...
			continue;
		}

		CString strRecv = m_strRecvCmd.Mid(nStart + 1, nEnd - nStart - 1);
		m_strRecvCmd.Delete(0, nEnd + 1);

		char chSep = ',';
		CString strCmd, strOp;

		AfxExtractSubString(strCmd, strRecv, 0, chSep);
		AfxExtractSubString(strOp, strRecv, 1, chSep);

		// Inspector Log ////////////////////////////////////////
		if (strCmd != "HEART" && strOp != "BEAT") {
			strLog.Format("[H<-V] : %s", strRecv);
			g_objLogFile.Save_InspectorLog(strLog);
		}
		/////////////////////////////////////////////////////////

		EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();
 		if (!pEquipData->bUseVisionCapDir && !pEquipData->bUseVisionCmAlign) return 0;

		CString strArg[11];
		for (int i = 0; i < 11; i++) AfxExtractSubString(strArg[i], strRecv, i + 2, chSep);

		if (strCmd == "CONNECT") {
			if (strOp == "REQUEST")	Get_ConnectRequest();
			else if (strOp == "REPLY") Get_ConnectReply();
			else if (strOp == "END") Get_ConnectEnd();

		} else if (strCmd == "STATUS") {
			if (strOp == "REQUEST")	Get_StatusRequest();
			else if (strOp == "REPLY") Get_StatusReply(strArg[0]);
			else if (strOp == "UPDATE") Get_StatusUpdate(strArg[0]);

		} else if (strCmd == "LOT") {
			if (strOp == "READY") Get_LotReady(strArg[0], strArg[1]);

		} else if (strCmd == "INSPECT") {
			if (strOp == "COMPLETE") Get_InspectComplete(strArg[0], strArg[1], strArg[2], strArg[3], strArg[4], strArg[5], strArg[6], strArg[7], strArg[8], strArg[9], strArg[10]);

		} else if (strCmd == "ERROR") {
			if (strOp == "REQUEST") Get_ErrorRequest(strArg[0], strArg[1], strArg[2], strArg[3], strArg[4], strArg[5]);

		} else if (strCmd == "HEART") {
			if (strOp == "BEAT") Get_HeartBeat();
		}
	}

	return 1;
}

///////////////////////////////////////////////////////////////////////////////
// Get Command

void CInspector::Get_ConnectRequest()
{
	Set_ConnectReply();
}

void CInspector::Get_ConnectReply()
{
}

void CInspector::Get_ConnectEnd()
{
}

void CInspector::Get_StatusRequest()
{
	int nStatus = (int)g_objSequenceMain.Is_MainThreadRun();
	Set_StatusReply(nStatus);
}

void CInspector::Get_StatusReply(CString sStatus)
{
	m_nStatusVPC = atoi(sStatus);
}

void CInspector::Get_StatusUpdate(CString sStatus)
{
	KillTimer(0);
	m_nStatusVPC = atoi(sStatus);
	SetTimer(0, 15000, NULL);
}

void CInspector::Get_LotReady(CString sLotId, CString sPortNo)
{
	int nPortNo = atoi(sPortNo);
	if (sLotId != gData.sLotID[nPortNo-1]) return;
	m_bLotReady = TRUE;
}

void CInspector::Get_InspectComplete(CString sGbn, CString sLotId, CString sPortNo, CString sTrayNo, CString sCmNo, CString sJudge, CString sNgCode, CString sOffsetX, CString sOffsetY, CString sSizeX, CString sSizeY)
{
	int nPx = atoi(sPortNo) - 1;
	int	nTx = atoi(sTrayNo) - 1;
	int	nCx = atoi(sCmNo) - 1;
	if (nTx < 0 || nTx > 99 || nCx < 0 || nCx > 200) { g_objCommon.Show_Error(6101); return; }

	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();
	int nIndexNo = g_objCommon.Get_MainIndexPos(1);

	int nV = ((sGbn == "T1" || sGbn == "T2") ? 0 : ((sGbn == "B1"|| sGbn == "B2") ? 1 : -1));
	if (nV == -1) { g_objCommon.Show_Error(6102); return; }

	if (sJudge != "G" && sNgCode.GetLength() < 2) sNgCode = "NON";	// Good 일때 NG Code는 Space(" ")

	if (sGbn == "T1" || sGbn == "T2") {
		int nMode = theApp.Get_MainMode();
		int nPreInfo = gData.nCmInspectInfo[nPx][nTx][nCx];
		int nNo1 = gData.nInspCmNo[0][0] - 1;
		int nNo2 = gData.nInspCmNo[0][1] - 1;

		int nPickNo = (nCx == nNo2 ? gData.nCmInspPickNo2 - 1 : (nCx == nNo1 ? gData.nCmInspPickNo1 - 1 : -1));
		if (nPickNo < 0) return;

		gData.nCmInspectInfo[nPx][nTx][nCx] = (sJudge == "G" ? 1 : 2);

		gData.dIndexOffsetX[nIndexNo][nPickNo] = atof(sOffsetX);	// CM Align Offset X
		gData.dIndexOffsetY[nIndexNo][nPickNo] = atof(sOffsetY);	// CM Align Offset Y
		gData.dIndexCmSizeX[nIndexNo][nPickNo] = atof(sSizeX);		// CM Size X
		gData.dIndexCmSizeY[nIndexNo][nPickNo] = atof(sSizeY);		// CM Size Y

		int nCase = g_objSequenceMain.Get_MainRunCase(AUTO_VISION_CM);
		if (nCase != 5) { Exception_Log("Inspect Complete", sGbn, nCase); return; }
		if (m_nT12ScanCnt > m_nT12ScanReq) return;

		int nCmInfo1 = (nNo1 >= 0 ? gData.nCmInspectInfo[nPx][nTx][nNo1] : 1);
		int nCmInfo2 = (nNo2 >= 0 ? gData.nCmInspectInfo[nPx][nTx][nNo2] : 1);

		//Temp
		if (!pEquipData->bUseVisionAlignAlarm && !pEquipData->bUseVisionAlignOffset){
			m_nT12ScanCnt++;
			if (m_nT12ScanCnt == m_nT12ScanReq) {
				gData.bScanDone[0] = TRUE;
				g_objSequenceMain.Set_MainRunCase(AUTO_VISION_CM, 10);
			}
		} else {
			if (pEquipData->bUseVisionAlignOffset) {
				m_nT12ScanCnt++;
				if (m_nT12ScanCnt == m_nT12ScanReq) {
					gData.bScanDone[0] = TRUE;
					g_objSequenceMain.Set_MainRunCase(AUTO_VISION_CM, 10);
				}
			} else {
				m_nT12ScanCnt++;
				if (m_nT12ScanCnt == m_nT12ScanReq &&
					(gData.InfoIndex[0][m_nT12ScanCnt+0] == 0 || (gData.InfoIndex[0][m_nT12ScanCnt+0] > 0 && nCmInfo1 == 1)) &&
					(gData.InfoIndex[0][m_nT12ScanCnt+3] == 0 || (gData.InfoIndex[0][m_nT12ScanCnt+3] > 0 && nCmInfo2 == 1))) {
					m_nT12ScanCnt = 0;
					gData.bScanDone[0] = TRUE;
					g_objSequenceMain.Set_MainRunCase(AUTO_VISION_CM, 10);

				} else {
					if (m_nT12ScanCnt == m_nT12ScanReq) {
						m_nT12ScanCnt = 0;
						g_objSequenceMain.Set_MainRunCase(AUTO_VISION_CM, 6);
					}
				}
			}
		}

	} else if (sGbn == "B1" || sGbn == "B2") {
		int nNo1 = gData.nInspCmNo[1][0] - 1;
		int nNo2 = gData.nInspCmNo[1][1] - 1;

		int nPickNo = (nCx == nNo2 ? gData.nCapInspPickNo2 - 1 : (nCx == nNo1 ? gData.nCapInspPickNo1 - 1 : -1));
		if (nPickNo < 0) return;

		gData.nCapInspectInfo[nPx][nTx][nCx] = (sJudge == "G" ? 1 : (sJudge == "N" ? 2 : 0));	// G,N,E

		if (gData.nCapInspectInfo[nPx][nTx][nCx] == 2) {
			CString strErrSub;
			strErrSub.Format(" ==> Picker:%d, Tray:%d, CM:%d", nPickNo+1, nTx+1, nCx+1);
			g_objCommon.Set_ErrorSubMessage(strErrSub);
			g_objCommon.Show_Error(6220);	// 3706
			return;
		}

		int nCase = g_objSequenceMain.Get_MainRunCase(AUTO_VISION_CAP);
		if (nCase != 5) { Exception_Log("Inspect Complete", sGbn, nCase); return; }
		if (m_nB12ScanCnt > m_nB12ScanReq) return;

		m_nB12ScanCnt++;
		if (m_nB12ScanCnt == m_nB12ScanReq) {
			gData.bScanDone[1] = TRUE;
			g_objSequenceMain.Set_MainRunCase(AUTO_VISION_CAP, 10);
		}
	}
}

void CInspector::Get_ErrorRequest(CString sGbn, CString sLotId, CString sPortNo, CString sTrayNo, CString sCmNo, CString sErrNo)
{
	int	nTNo = atoi(sTrayNo) - 1;
	int	nCNo = atoi(sCmNo) - 1;
	int nPNo = atoi(sPortNo) - 1;
	if (nTNo < 0 || nTNo > 99 || nCNo < 0 || nCNo > 200 || nPNo < 0 || nPNo > 2) { g_objCommon.Show_Error(6101); return; }

	int nErrNo = atoi(sErrNo);	// 6120 : Mirror Z Axis Error, 6130: Btm2 Picker Module Error

	CString strErrMsg;
	strErrMsg.Format(" ==> Vision:%s. LotID:%s. PortNo:%d. Tray:%d. CM:%d", sGbn, sLotId, nPNo+1, nTNo+1, nCNo+1);

	g_objCommon.Set_ErrorSubMessage(strErrMsg);

	int nMode = theApp.Get_MainMode();
	if (nErrNo != 6120 && (nMode == MODE_WORK || nMode == MODE_OPERATOR)) g_objCommon.Show_Error(nErrNo);
}

void CInspector::Get_HeartBeat()
{
	KillTimer(0);
	m_nStatusVPC = 1;
	SetTimer(0, 15000, NULL);
}

void CInspector::Exception_Log(CString sFunc, CString sGbn, int nCase)
{
	CString strLog;
	strLog.Format("%s Exception ==> Vision:%s, Case:%d", sFunc, sGbn, nCase);
	g_objLogFile.Save_InspectorLog(strLog);
}

/////////////////////////////////////////////////////////////////////////////
// Set Command

void CInspector::Set_ConnectRequest()
{
	CString	strSendCmd;
	strSendCmd.Format("CONNECT,REQUEST");
	Send_Command(strSendCmd);
}

void CInspector::Set_ConnectReply()
{
	CString	strSendCmd;
	strSendCmd.Format("CONNECT,REPLY");
	Send_Command(strSendCmd);
}

void CInspector::Set_ConnectEnd()
{
	CString	strSendCmd;
	strSendCmd.Format("CONNECT,END");
	Send_Command(strSendCmd);
}

void CInspector::Set_StatusRequest()
{
	CString	strSendCmd;
	strSendCmd.Format("STATUS,REQUEST");
	Send_Command(strSendCmd);
}

void CInspector::Set_StatusReply(int nStatus)
{
	CString	strSendCmd;
	strSendCmd.Format("STATUS,REPLY,%d", nStatus);
	Send_Command(strSendCmd);
}

void CInspector::Set_StatusUpdate(int nStatus)
{
	CString	strSendCmd;
	strSendCmd.Format("STATUS,UPDATE,%d", nStatus);
	Send_Command(strSendCmd);
}

void CInspector::Set_LotStart(CString sLotId, int nPortNo)
{
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	m_bLotReady = FALSE;
	CString	strSendCmd;
	strSendCmd.Format("LOT,START,%s,%d,%d,%d,%s", sLotId, nPortNo, gData.nTrayUseCount[nPortNo-1], gData.nCmUseCount[nPortNo-1], gData.sRecipe);
	Send_Command(strSendCmd);
}

void CInspector::Set_LotEnd(CString sLotId, int nPortNo)
{
	CString	strSendCmd;
	strSendCmd.Format("LOT,END,%s,%d", sLotId, nPortNo);
	Send_Command(strSendCmd);
}

void CInspector::Set_LoadComplete(CString sGbn, CString sLotId, int nPortNo, int nTNo1, int nTNo2, int nCNo1, int nCNo2, int nPickNo1, int nPickNo2)
{
	CString	strSendCmd, strTemp;
	CString strBar1, strBar2;
	int nIndexNo = g_objCommon.Get_MainIndexPos(1) + 1;
	int nINo1 = -1;
	int nINo2 = -1;

	if (sGbn == "T12") {
		m_nT12ScanReq = m_nT12ScanCnt = 0;
		if (nCNo1 > 0) { strBar1 = gMes.sBarID[nPortNo-1][nTNo1-1][nCNo1-1]; nINo1 = nIndexNo; m_nT12ScanReq++; }
		if (nCNo2 > 0) { strBar2 = gMes.sBarID[nPortNo-1][nTNo2-1][nCNo2-1]; nINo2 = nIndexNo; m_nT12ScanReq++; }

		if (nCNo1 == 0) { nCNo1 = -1; nTNo1 = -1; nPickNo1 = -1; strBar1 = ""; }
		if (nCNo2 == 0) { nCNo2 = -1; nTNo2 = -1; nPickNo2 = -1; strBar2 = ""; }

		if (nTNo1 == 0) nTNo1 = -1;
		if (nTNo2 == 0) nTNo2 = -1;

		if (strBar1 == "") strBar1.Format("NO_INFO1");
		if (strBar2 == "") strBar2.Format("NO_INFO2");

		gData.nInspCmNo[0][0] = nCNo1;
		gData.nInspCmNo[0][1] = nCNo2;
		gData.bScanDone[0] = FALSE;

	} else if (sGbn == "B12") {
		m_nB12ScanReq = m_nB12ScanCnt = 0;
		if (nCNo1 > 0) m_nB12ScanReq++;
		if (nCNo2 > 0) m_nB12ScanReq++;

		gData.nInspCmNo[1][0] = nCNo1;
		gData.nInspCmNo[1][1] = nCNo2;
		gData.bScanDone[1] = FALSE;
		
		strBar1.Format("NO_INFO1");
		strBar2.Format("NO_INFO2");
	}

	strSendCmd.Format("LOAD,COMPLETE,%s,%s,%d,%d,%d,%d,%d,%s,%s,%d,%d,%d,%d", sGbn, sLotId, nPortNo, nTNo1, nTNo2, nCNo1, nCNo2, strBar1, strBar2, nPickNo1, nPickNo2, nINo1, nINo2);
	Send_Command(strSendCmd);
}

void CInspector::Set_InitialRequest()
{
	CString	strSendCmd;
	strSendCmd.Format("INITIAL,REQUEST");
	Send_Command(strSendCmd);
}

void CInspector::Set_LightOff()
{
	CString strSendCmd;
	strSendCmd.Format("LIGHT,OFF");
	Send_Command(strSendCmd);
}

///////////////////////////////////////////////////////////////////////////////

int CInspector::Get_VisionStatus()
{
	return m_nStatusVPC;
}

BOOL CInspector::Check_LotReady()
{
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();
#ifdef DRY_RUN_TEST
	m_bLotReady = TRUE;
#else
	if (!pEquipData->bUseVisionCapDir && !pEquipData->bUseVisionCmAlign) m_bLotReady = TRUE;
#endif
	return m_bLotReady;
}

/////////////////////////////////////////////////////////////////////////////
// UDP Socket Send Message

void CInspector::Send_Command(CString strSend)
{
	// Inspector Log //////////////////////////////////////
	CString strLog;
	strLog.Format("[H->V] : %s", strSend);
	g_objLogFile.Save_InspectorLog(strLog);
	///////////////////////////////////////////////////////

	g_csInspector.Lock();	// Critical Section

	CString strSendSocket;
	strSendSocket.Format("@%s\n", strSend);

	char chSend[1024] = { 0 };
	int nLength = strSendSocket.GetLength();
	memcpy(chSend, (LPSTR)(LPCSTR)strSendSocket, nLength);

	m_UdpVisionPC.Write_Socket((BYTE*)chSend, nLength);

	g_csInspector.Unlock();	// Critical Section
}

/////////////////////////////////////////////////////////////////////////////

void CInspector::Test_Command()
{
}
