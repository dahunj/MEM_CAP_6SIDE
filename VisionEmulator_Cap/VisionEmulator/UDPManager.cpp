// UDPManager.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VisionEmulator.h"
#include "UDPManager.h"

CCriticalSection g_csInspector;	// Send_Command 문제 해결하기 위함

CUDPManager g_objUDPManager;

// CUDPManager

IMPLEMENT_DYNAMIC(CUDPManager, CWnd)

CUDPManager::CUDPManager()
{

}

CUDPManager::~CUDPManager()
{
}


BEGIN_MESSAGE_MAP(CUDPManager, CWnd)
END_MESSAGE_MAP()



// CUDPManager 메시지 처리기입니다.


void CUDPManager::DoEvents(int nSleep)
{
	MSG msg;
	if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (nSleep > 0) Sleep(nSleep);
}

void CUDPManager::Get_ConnectRequest(int nInspector)
{
	m_bConnectPC = TRUE;

	//Set_ConnectReply(nInspector);
}

void CUDPManager::Get_ConnectReply(int nInspector)
{
	m_bConnectPC = TRUE;

}

void CUDPManager::Get_ConnectEnd(int nInspector)
{
	m_bConnectPC = FALSE;

}


void CUDPManager::Get_StatusRequest(int nInspector)
{
	//BOOL bStatus = g_objSequenceMain.Is_MainThreadRun();
	//if (bStatus) Set_StatusReply(nInspector, 1);
	//else Set_StatusReply(nInspector, 0);
}

void CUDPManager::Get_StatusReply(int nInspector, CString sStatus)
{
	m_nStatusPC = atoi(sStatus);

}

void CUDPManager::Get_StatusUpdate(int nInspector, CString sStatus)
{
	KillTimer(nInspector);
	m_nStatusPC = atoi(sStatus);

	SetTimer(nInspector, 15000, NULL);
}


void CUDPManager::Send_Command(CString strSend)
{
	// Inspector Log //////////////////////////////////////
	CString strLog;
	strLog.Format("[H->V] : %s", strSend);
	//g_objLogFile.Save_InspectorLog(strLog);
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

void CUDPManager::Initialize()
{
	BOOL bOpenedPC1 = m_UdpVisionPC.Open_Socket(8001, 8000, "127.0.0.1", this);
	if(bOpenedPC1) Set_ConnectRequest();

}

void CUDPManager::Terminate()
{
	m_UdpVisionPC.Close_Socket();

}


LRESULT CUDPManager::OnUdpReceive(WPARAM wLocalPort, LPARAM lParam)
{
	UINT nPort = (UINT)wLocalPort;
	int nInspector = 0, nLen = 0;
	BYTE byRecv[1024] = { 0 };
	CString strLog;

	if (nPort == 8000) { nInspector = INSPECTOR_PC1; nLen = m_UdpVisionPC.Read_Socket(byRecv); }

	if (nInspector == 0 || nLen < 1) {
		strLog.Format("[H<-V%d] : Local Port (%d) Mismatch or Receive Data Zero (%d)", nInspector, nPort, nLen);
		//g_objLogFile.Save_InspectorLog(strLog);
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
			strLog.Format("[H<-V%d] : <<Error>> %s : Start(%d), End(%d)", nInspector, m_strRecvCmd, nStart, nEnd);
			//g_objLogFile.Save_InspectorLog(strLog);
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
			strLog.Format("[H<-V%d] : %s", nInspector, strRecv);
			//g_objLogFile.Save_InspectorLog(strLog);
		}
		/////////////////////////////////////////////////////////

		CString strArg[7];
		for (int i = 0; i < 7; i++) AfxExtractSubString(strArg[i], strRecv, i + 2, chSep);

		if (strCmd == "CONNECT") {
			if (strOp == "REQUEST")	Get_ConnectRequest(nInspector);
			else if (strOp == "REPLY") Get_ConnectReply(nInspector);
			else if (strOp == "END") Get_ConnectEnd(nInspector);

		} else if (strCmd == "STATUS") {
			if (strOp == "REQUEST")	Get_StatusRequest(nInspector);
			else if (strOp == "REPLY") Get_StatusReply(nInspector, strArg[0]);
			else if (strOp == "UPDATE") Get_StatusUpdate(nInspector, strArg[0]);

		} 
	}
	return 1;
}


/////////////////////////////////////////////////////////////////////////////
// Set Command

void CUDPManager::Set_ConnectRequest()
{
	CString	strSendCmd;
	strSendCmd.Format("CONNECT,REQUEST");
	Send_Command(strSendCmd);
}

void CUDPManager::Set_ConnectReply()
{
	CString	strSendCmd;
	strSendCmd.Format("CONNECT,REPLY");
	Send_Command(strSendCmd);
}

void CUDPManager::Set_ConnectEnd()
{
	CString	strSendCmd;
	strSendCmd.Format("CONNECT,END");
	Send_Command(strSendCmd);
}

void CUDPManager::Set_StatusRequest()
{
	CString	strSendCmd;
	strSendCmd.Format("STATUS,REQUEST");
	Send_Command(strSendCmd);
}

void CUDPManager::Set_StatusReply(int nStatus)
{
	CString	strSendCmd;
	strSendCmd.Format("STATUS,REPLY,%d", nStatus);
	Send_Command(strSendCmd);
}

void CUDPManager::Set_StatusUpdate(int nStatus)
{
	CString	strSendCmd;
	strSendCmd.Format("STATUS,UPDATE,%d", nStatus);
	Send_Command(strSendCmd);
}


void CUDPManager::OnTimer(UINT_PTR nIDEvent)
{
	KillTimer(nIDEvent);
	switch (nIDEvent) {
	case 1:	m_nStatusPC = 0; break;

	}
	CWnd::OnTimer(nIDEvent);
}