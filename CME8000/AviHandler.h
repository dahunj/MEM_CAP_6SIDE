// AviHandler.h : 헤더 파일
//
#pragma once

// CAviHandler

class CAviHandler : public CWnd
{
	DECLARE_DYNAMIC(CAviHandler)

public:
	CAviHandler();
	virtual ~CAviHandler();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnUdpReceive(WPARAM wLocalPort, LPARAM lParam);

private:
	CUdpSocketCS	m_UdpAvi;

	CString		m_strRecvCmd;

	BOOL	m_bOpened;
	BOOL	m_bConnected;
	int		m_nStatusAvi;

	void Get_ConnectRequest();
	void Get_ConnectReply();
	void Get_ConnectEnd();
	void Get_StatusRequest();
	void Get_StatusReply(CString sStatus);
	void Get_StatusUpdate(CString sStatus);
	void Get_TimeUpdate(CString sTime);
	void Get_LotStart(CString sLotID, CString sPortNo, CString sTrayCnt, CString sCmCnt, CString sModel);
	void Get_LotEnd(CString sLotID, CString sPortNo);
	void Get_TrayLoad(CString sLotID, CString sTrayNo, CString sCmCnt, CString sPortNo);
	void Get_TrayEnd(CString sLastTrayNo, CString sPortNo);
	void Get_BarcodeUpdate(CString sPortNo, CString sTrayNo, CString sCmNo, CString sBarcode);
	void Get_ApdRequest();

	void Send_Command(CString strSend);

public:
	BOOL Initialize();
	void Terminate();

	void Set_ConnectRequest();
	void Set_ConnectReply();
	void Set_ConnectEnd();
	void Set_StatusRequest();
	void Set_StatusReply(int nStatus);
	void Set_StatusUpdate(int nStatus);
	void Set_TrayUnload();
	void Set_ApdReply();


	void Set_ConnectStatus(BOOL isConnected) { m_bConnected = isConnected; }
	void Set_NotifyCmAlignAlarm();

	BOOL Is_Opened() { return m_bOpened; }

	BOOL Is_Connected() { return m_bConnected; } 

	
};

extern CAviHandler g_objAviHandler;

///////////////////////////////////////////////////////////////////////////////
