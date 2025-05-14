// MESInterface.h : 헤더 파일
//
#pragma once

class CMESInterface
{
public:
	CMESInterface();
	virtual ~CMESInterface();

private:
	int		m_nReadCnt;

	void Create_Folder(CString sPath);
	void MakeFolder(CString sPath);
	char* UTF8ToANSI(const char *pszCode);

protected:
	CWinThread *m_pThreadMESCap;
	static UINT	Thread_MESCap(LPVOID lpVoid);

	CWinThread *m_pThreadMESShip;
	static UINT	Thread_MESShip(LPVOID lpVoid);

public:
	CString	m_sOperLotID;
	int		m_nOperCount;
	CString	m_sOperPartNo;
	CString	m_sOperOpID;

	BOOL	m_bMESUse, m_bThreadMESCap, m_bThreadMESShip;
	CString	m_sMESResult, m_sMESLotID, m_sReasonCode, m_sReasonText;	//m_MESResult:0[OK], 1[NG]
	int		m_nMESCount;
	int		m_nMESCapSequence;	// 1: Start, 2: Ok
	int		m_nMESShipSequence;	// 1: Start, 2: Ok

	void Initialize(BOOL bMESUse);
	void Set_MESUse(BOOL bMESUse);
	void Set_Status(int	nStste);	// 1:Run, 2:Stop, 3:Idle

	void Set_CapLotChangeRequest(CString sLotID, CString sOperID);
	void Set_ShipLotChangeRequest(CString sLotID, CString sOperID);
	void Save_AviRmsData(CString sKey, CString sValue);

	void Read_Result(int nType);
	void Clear_Result();

	void Clear_APDResult();
	BOOL Read_APDResult();
};

extern CMESInterface g_objMES;

///////////////////////////////////////////////////////////////////////////////
