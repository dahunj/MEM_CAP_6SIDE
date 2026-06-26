// Host.h : 헤더 파일
//
#pragma once

#include "./CXml/Xml.h"

using namespace JWXml;

// CHost

class CHost : public CWnd
{
	DECLARE_DYNAMIC(CHost)

public:
	CHost();
	virtual ~CHost();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnServerAccept(WPARAM wLocalPort, LPARAM lClientIdx);
	afx_msg LRESULT OnServerReceive(WPARAM wLocalPort, LPARAM lClientIdx);
	afx_msg LRESULT OnServerRemove(WPARAM wLocalPort, LPARAM lClientIdx);

private:
	CServerSocketCS	m_Server;
	UINT			m_nLPort;

	BOOL	m_bConnected;
	BOOL	m_bHostOnline;
	CString	m_strRecvCmd;

	CXml	m_xml;

	int		m_nRecvCmdCount;	// 4Byte
	int		m_nSendCmdCount;

	CString m_strStFn;	// StreamFunction (S1F1, S2F3, S2F31, S2F49, S6F12)
	CString m_strRcmd;	// RCMD Command (START, CANCEL, DATA, PERMIT)

	DWORD	m_dwLastTime;	// 마지막 통신 시간
	CString m_strSetTime;	// Host 설정 시간

	CString m_strDisplay;	// Teminal Display Message

private:
	BOOL Extract_Xml(CString sXmlData);

	void Get_S1F1_Ready();				// Are You There Request
	void Get_S1F3_State();				// Equip Status Request
	void Get_S2F3_Link();				// Link Test Request
	void Get_S2F31_Time();				// Date and Time Set Request
	void Get_S2F49_LotStart();			// Enhanced Remote Command
	void Get_S2F49_LotIdFail();			// Enhanced Remote Command
	void Get_S2F49_RetestLotData();		// Enhanced Remote Command
	void Get_S2F49_MaterialConfirm();	// Enhanced Remote Command
	void Get_S2F49_MaterialFail();		// Enhanced Remote Command
	void Get_S2F49_PPSelect();			// Enhanced Remote Command
	void Get_S5F2_AlarmAck();			// Alarm Report Acknowledge
	void Get_S10F3_Display();			// Terminal Display, Single

	void Reply_HeartBeat();				// Heart Beat

	void Send_Command(CString sSend, BOOL bReply, CString sStFn, CString sRcmd="");	// XML

public:
	void Initialize();
	void Terminate();

	BOOL Is_Connected() { return m_bConnected; }
	BOOL Is_HostOnline() { return m_bHostOnline; }
	DWORD Get_LastTime() { return m_dwLastTime; }

	void Set_S1F1_Ready();		// Are You There Request
	void Set_S5F1_AlarmReport(int nFlag, CString sErrNo, CString sErrMsg);	// nFlag(1:Alarm, 0:해제) Alarm Report Send
	void Set_S6F11_ControlState(int nState);	// 1:Online, 2:Offline
	void Set_S6F11_EquipState(int nState, CString sErrNo, CString sCategory, CString sErrMsg);	// 2:Idle, 5:Run, 6:Down

	void Set_S6F11_LotReady(CString sLotId);	// Lot Ready
	void Set_S6F11_LotStarted(CString sLotId, int nCount);	// Lot Started Report
	void Set_S6F11_LotEnd(CString sLotId, CString sRecipe, int nCount, int nOk, int nNg, int nBNg);	// Lot Complete Report
	void Set_S6F11_LotAbort(CString sLotId, CString sRecipe);	// Lot Suspended Report
	void Set_S6F11_IdleSet();
	void Set_S6F11_IdleReset();
	void Set_S6F11_IdleReport();
	void Set_S6F11_CmEnd(CString sLotId, int nTray, int nPocket, CString sResult, CString sNgCode, CString sCmId);
	
	void Set_S6F11_MaterialReport(int nType, CString sId);
	void Set_S6F11_MaterialComplete(int nType, CString sId);
	void Set_S6F11_PPSelected(CString sLotId, CString sRecipeId);
	void Set_S9F13_Timeout();	// Conversation Timeout

	void Test_Command();
};

extern CHost g_objHost;

///////////////////////////////////////////////////////////////////////////////
