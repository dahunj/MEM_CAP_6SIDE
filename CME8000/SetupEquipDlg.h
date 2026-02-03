// SetupEquipDlg.h : 헤더 파일
//
#pragma once

// CSetupEquipDlg 대화 상자입니다.

class CSetupEquipDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetupEquipDlg)

public:
	CSetupEquipDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSetupEquipDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SETUP_EQUIP_DLG };
	CGroupCS	m_Group[12];
	CLabelCS	m_Label[7];

	CStaticCS	m_stcEquipName;
	CRadioCS	m_rdoModel[2];	// 0:R53B, 1:R54B
	CComboCS	m_cboLotBarcodePort;
	CComboCS	m_cboAssyLoadCellPort;
	CComboCS	m_cboUnloadLoadCellPort;
	CStaticCS	m_stcMotionCheck;
	CLabelCS	m_lblDoorLock;
	CRadioCS	m_rdoDoorLock[2];
	CLabelCS	m_lblDoorLock2;
	CStaticCS	m_stcDoorLockTime;

	CComboCS	m_cboMoveDataSelection;

	CCheckCS	m_chkUseInlineMode;
	CCheckCS	m_chkUseVisionCapDir;
	CCheckCS	m_chkUseVisionCmAlign;
	CCheckCS	m_chkUseVisionAlignAlarm;
	CCheckCS	m_chkUseVisionAlignOffset;

	CCheckCS	m_chkUseTrayPickerTurn;
	CCheckCS	m_chkUseCapPickerTurn;
	CCheckCS	m_chkAssyPickerTilt;

	CCheckCS	m_chkCapPickUpMulti;
	CCheckCS	m_chkUseMesCapReg;
	CCheckCS	m_chkUseMesShipReg;
	CCheckCS	m_chkUseIndexAssyVac;

	CLabelCS	m_lblModel[13];
	CStaticCS	m_stcIndexData[1];
	CStaticCS	m_stcLoadTrayData[4];
	CStaticCS	m_stcCapTrayData[4];
	CStaticCS	m_stcShipTrayData[4];

	CLabelCS	m_lblPickerVacOff[3];
	CStaticCS	m_stcVacOffDelay[3];

	CLabelCS	m_lblDelayAdd[6];
	CStaticCS	m_stcDelayAdd[6];

	CLabelCS	m_lblAviIp;
	CIPAddressCtrl	m_ipaAviIp;

	CLabelCS	m_lblAlignOffset;
	CStaticCS	m_stcAlignOffset;

	CStaticCS	m_stcLoadCellRange[2];

	CGroupCS	m_grpTower;
	CLabelCS	m_lblTower[10];
	CCheckCS	m_chkTower[6][4];
	CGroupCS	m_grpBuzzer;
	CLabelCS	m_lblBuzzer[11];
	CCheckCS	m_chkBuzzer[5][6];

	CGroupCS	m_grpHidden;
	CLabelCS	m_lblPasswordMt;
	CStaticCS	m_stcPasswordMt;
	CLabelCS	m_lblPasswordSi;
	CEditCS		m_edtPasswordSi;

	CLabelCS	m_lblCMVision[3];
	CStaticCS	m_stcCMVision[3];

	CGroupCS	m_grpResultTest;
	CLabelCS	m_lblResultTest;
	CCheckCS	m_chkResultTestUse;
	CEditCS		m_edtResultTest;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnStnClickedStcEquipName();
	afx_msg void OnRdoModelClick(UINT nID);
	afx_msg void OnStnClickedStcMotionCheck();

	afx_msg void OnStcIndexDataClick(UINT nID);
	afx_msg void OnStcLoadTrayDataClick(UINT nID);
	afx_msg void OnStcCapTrayDataClick(UINT nID);
	afx_msg void OnStcShipTrayDataClick(UINT nID);

	afx_msg void OnStcVacOffDelayClick(UINT nID);
	afx_msg void OnStcDelayAddClick(UINT nID);

	afx_msg void OnStcAlignOffsetClick();
	afx_msg void OnStcLoadCellRangeClick(UINT nID);

	afx_msg void OnStnClickedStcShowHidden();
	afx_msg void OnStnClickedStcPasswordMt();

private:
	CString m_strLog;

	void Initial_Controls();
	void Display_EquipData();

public:
	void Save_EquipData();
	void Cancel_EquipData();
	afx_msg void OnBnClickedBtnAviConnect();
	afx_msg void OnBnClickedBtnAviDisconnect();
	afx_msg void OnStnClickedStcCmVision0();
	afx_msg void OnStnClickedStcCmVision1();
	afx_msg void OnStnClickedStcCmVision2();
	afx_msg void OnStnClickedStcDoorlockTime();
};

///////////////////////////////////////////////////////////////////////////////
