// Handler.h : 헤더 파일
//
#pragma once

// CHandler

#define HANDLER_PORT	10000	// Local Port for Handler

class CHandler : public CWnd
{
	DECLARE_DYNAMIC(CHandler)

public:
	CHandler();
	virtual ~CHandler();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnServerAccept(WPARAM wLocalPort, LPARAM lClientIdx);
	afx_msg LRESULT OnServerReceive(WPARAM wLocalPort, LPARAM lClientIdx);
	afx_msg LRESULT OnServerRemove(WPARAM wLocalPort, LPARAM lClientIdx);

private:
	CServerSocketCS	m_Server;
	UINT			m_nLPort;

	BOOL	m_bConnected;
	CString	m_strRecvCmd;

private:
	void Get_OperUpdate(CString sOperId);
	void Get_ControlState(CString sFlag, CString sOperId);	// 1:Online, 2:Offline
	void Get_EquipState(CString sState);					// 1:Run, 4:Idle, 5:Down
	void Get_ErrorUpdate(CString sFlag, CString sErrNo, CString sCategory);

	void Get_LotReady(CString sLotId);
	void Get_LotStarted(CString sLotId, CString nCmCnt);
	void Get_LotEnd(CString sLotId, CString sRecipe, CString sCount, CString sOk, CString sNg, CString sBNg, CString sFlag);
	void Get_LotAbort(CString sLotId, CString sRecipe);
	void Get_IdleSet(CString sOperId, CString sCode);
	void Get_IdleReset(CString sOperId, CString sCode);
	void Get_IdleReport(CString sOperId, CString sCode, CString sText, CString sSTime, CString sETime);
	void Get_CmEnd(CString sLotId, CString sTray, CString sPocket, CString sResult, CString sNgCode, CString sCmId);
	void Get_LotIdRequest(CString sSite, CString sEqNo, CString sLabel, CString sRtstId, int nTotal, int nCount);	// Retest
	void Get_CapIdRequest(CString sCapId);
	void Get_ShipIdRequest(CString sShipId);
	void Get_CapIdComplete(CString sCapId);
	void Get_ShipIdComplete(CString sShipId);
	void Get_RecipeSelected(CString sLotId, CString sRecipe);

	void Send_Command(CString sSend);

public:
	void Initialize();
	void Terminate();

	BOOL Is_Connected() { return m_bConnected; }

	void Set_ControlState(int nFlag);	// 1:Online, 2:Offline
	void Set_ErrorReply();	// Error Update 응답

	void Set_LotStart(CString sLotId, CString sRecipe, int nCmCnt);
	void Set_LotIdFail(CString sLotId, CString sRtstId, CString sLabel, CString sCode, CString sText);		// Retest
	void Set_LotIdSucess(CString sLotId, CString sRecipe, int nCmCount, CString sRtstId, CString sLabel);	// Retest

	void Set_CapIdSucess(CString sLotId);
	void Set_ShipIdSucess(CString sLotId);
	void Set_CapIdFail(CString sLotId, CString sCode, CString sText);
	void Set_ShipIdFail(CString sLotId, CString sCode, CString sText);
	void Set_RecipeSelect(CString sLotId, CString sRecipe);

	void Set_TerminalDisplay(CString sDisplay);
	void Set_TimeSync();
};

extern CHandler g_objHandler;

///////////////////////////////////////////////////////////////////////////////
