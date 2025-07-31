#pragma once

const int INSPECTOR_ALL		= 0;	// PC1, PC2, PC3, PC4
const int INSPECTOR_PC1		= 1;	// Angle, Align, Btm1
const int INSPECTOR_PC2		= 2;	// Top1
const int INSPECTOR_PC3		= 3;	// Top2(Top2, Top3)
const int INSPECTOR_PC4		= 4;	// Btm2(Btm2, Btm3)


// CUDPManager

class CUDPManager : public CWnd
{
	DECLARE_DYNAMIC(CUDPManager)

public:
	CUDPManager();
	virtual ~CUDPManager();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnUdpReceive(WPARAM wLocalPort, LPARAM lParam);

private:
	CUdpSocketCS	m_UdpVisionPC;
	
	CString m_strRecvCmd;

	BOOL	m_bConnectPC;
	
	int		m_nStatusPC;		// Vision PC1 ป๓ลย (0:Not Ready, 1:Ready)
	
	BOOL	m_bLotReady;

	void DoEvents(int nSleep = 0);


	void Get_ConnectRequest(int nInspector);
	void Get_ConnectReply(int nInspector);
	void Get_ConnectEnd(int nInspector);

	void Get_StatusRequest(int nInspector);
	void Get_StatusReply(int nInspector, CString sStatus);
	void Get_StatusUpdate(int nInspector, CString sStatus);

	void Send_Command(CString strSend);
	void Exception_Log(CString sFunc, CString sGbn, int nCase);	// Recevie Exception Log

public:
	void Initialize();
	void Terminate();

	void Set_ConnectRequest();
	void Set_ConnectReply();
	void Set_ConnectEnd();
	void Set_StatusRequest();
	void Set_StatusReply(int nStatus);
	void Set_StatusUpdate(int nStatus); 

};
extern CUDPManager g_objUDPManager;

