#pragma once


// CMesAgent

class CMesAgent : public CWnd
{
	DECLARE_DYNAMIC(CMesAgent)

public:
	CMesAgent();
	virtual ~CMesAgent();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnClientConnect(WPARAM wConnect, LPARAM lParam);
	afx_msg LRESULT OnClientReceive(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClientClose(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

private:
	CClientSocketCS m_Client;

	BOOL	m_bConnected;
	BOOL	m_bHostOnline;

	CString m_strRecvCmd;


private:
	
	void Get_ControlState(CString sFlag);	// 1:Online, 2:Offline
	void Get_ErrorReply();

	
	void Get_TerminalDisplay(CString sDisplay);
	void Get_TimeSync();
		

	void Get_CapIdSucess(CString sCapId);
	void Get_CapIdFail(CString sCapId, CString sCode, CString sText);
	void Get_ShipIdSucess(CString sShipId);
	void Get_ShipIdFail(CString sShipId, CString sCode, CString sText);
	
	void Send_Command(CString sSend);
public:
	int		m_nMesCapStatus;	// 0:None, 1:Send, 2:Receive
	int		m_nMesShipStatus;	// 0:None, 1:Send, 2:Receive


public:
	void Initialize();
	void Terminate();

	BOOL Is_Connected() { return m_bConnected; }
	BOOL Is_HostOnline() { return m_bHostOnline; }
	

	void Set_OperUpdate(CString sOperId);				// Operator ID 변경시 보고
	void Set_ControlState(int nFlag, CString sOperId);	// 1:Onine, 2:Offline
	void Set_EquipState(int nFlag);						// 
	void Set_ErrorUpdate(int nFlag, int nErrNo, int nCategory);	// nFlag(0:해제, 1:발생)
		
	void Set_LotAbort(CString sLotId, CString sRecipe);
	void Set_IdleSet(CString sOperId, CString sCode);	// 비가동 집계 Set
	void Set_IdleReset(CString sOperId, CString sCode);	// 비가동 집계 Reset
	void Set_IdleReport(CString sOperId, CString sCode, CString sText, CString sSTime, CString sETime);

	void Set_LotEnd(CString sLotId, CString sRecipe, int nCount, int nOk, int nNg, int nBNg);
	void Set_CmEnd(CString sOut, int nTrayCnt, int nPosX, int nPosY, int nLotNo, int nTrayNo, int nCmNo);
	
	void Set_CapChangeRequest(CString sBarcode);
	void Set_ShipChangeRequest(CString sBarcode);
	void Set_CapChangeComplete(CString sBarcode);
	void Set_ShipChangeComplete(CString sBarcode);

	void Set_AlarmLog(int nErrNo, CString sErrMsg, int nCategory);
	void Reset_AlarmLog();
;
			
};


extern CMesAgent g_objMesAgent;

