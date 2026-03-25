// WorkDlg.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "CME8000.h"
#include "WorkDlg.h"
#include "afxdialogex.h"

#include "LogFile.h"
#include "Common.h"

#include "Inspector.h"
#include "AviHandler.h"
#include "BarcodeLot.h"
#include "LoadCell.h"
#include "MESInterface.h"
#include "SequenceInit.h"
#include "SequenceMain.h"

#include "OperatorDlg.h"
#include "CME8000Dlg.h"

// CWorkDlg 대화 상자입니다.
CWorkDlg g_dlgWork;

IMPLEMENT_DYNAMIC(CWorkDlg, CDialogEx)

CWorkDlg::CWorkDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWorkDlg::IDD, pParent)
{
}

CWorkDlg::~CWorkDlg()
{
}

void CWorkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	for (int i = 0; i < 10; i++) DDX_Control(pDX, IDC_GROUP_0 + i, m_Group[i]);
	for (int i = 0; i < 4; i++) DDX_Control(pDX, IDC_LABEL_0 + i, m_Label[i]);
	for (int i = 0; i < 2; i++) DDX_Control(pDX, IDC_GRP_LOT_0 + i, m_grpLot[i]);
	for (int i = 0; i < 6; i++) DDX_Control(pDX, IDC_LBL_LOT_0 + i, m_lblLot[i]);
	for (int i = 0; i < 2; i++) DDX_Control(pDX, IDC_STC_LOT_ID_0 + i, m_stcLotId[i]);
	for (int i = 0; i < 2; i++) DDX_Control(pDX, IDC_STC_TRAY_COUNT_0 + i, m_stcTrayCount[i]);
	for (int i = 0; i < 2; i++) DDX_Control(pDX, IDC_STC_CM_COUNT_0 + i, m_stcCmCount[i]);

	for (int i = 0; i < 3; i++) DDX_Control(pDX, IDC_LBL_CAP_LOT_0 + i, m_lblCapLot[i]);
	DDX_Control(pDX, IDC_STC_CAP_LOT_ID_R, m_stcCapLotIdR);
	DDX_Control(pDX, IDC_STC_CAP_TRAY_COUNT_R, m_stcCapTrayCountR);
	DDX_Control(pDX, IDC_STC_CAP_CM_COUNT_R, m_stcCapCmCountR);
	DDX_Control(pDX, IDC_STC_CAP_LOT_ID_S, m_stcCapLotIdS);
	DDX_Control(pDX, IDC_STC_CAP_TRAY_COUNT_S, m_stcCapTrayCountS);
	DDX_Control(pDX, IDC_STC_CAP_CM_COUNT_S, m_stcCapCmCountS);
	DDX_Control(pDX, IDC_BTN_CAP_CLEAR, m_btnCapClear);

	for (int i = 0; i < 3; i++) DDX_Control(pDX, IDC_LBL_SHIP_LOT_0 + i, m_lblShipLot[i]);
	DDX_Control(pDX, IDC_STC_SHIP_LOT_ID_R, m_stcShipLotIdR);
	DDX_Control(pDX, IDC_STC_SHIP_TRAY_COUNT_R, m_stcShipTrayCountR);
	DDX_Control(pDX, IDC_STC_SHIP_CM_COUNT_R, m_stcShipCmCountR);
	DDX_Control(pDX, IDC_STC_SHIP_LOT_ID_S, m_stcShipLotIdS);
	DDX_Control(pDX, IDC_STC_SHIP_TRAY_COUNT_S, m_stcShipTrayCountS);
	DDX_Control(pDX, IDC_STC_SHIP_CM_COUNT_S, m_stcShipCmCountS);
	DDX_Control(pDX, IDC_BTN_SHIP_CLEAR, m_btnShipClear);

	DDX_Control(pDX, IDC_BTN_MES_CANCEL, m_btnMesCancel);
	DDX_Control(pDX, IDC_IMG_EQUIPMENT, m_imgEquipment);
	DDX_Control(pDX, IDC_PIC_UPH_BACK, m_picUphBack);

	DDX_Control(pDX, IDC_STC_LOAD_CELL_COUNT, m_stcLoadCellCnt);
	DDX_Control(pDX, IDC_BTN_LOAD_CELL_EDIT, m_btnLoadCellChkEdit);

	for (int i = 0; i < 7; i++) DDX_Control(pDX, IDC_LED_EQUIP_OPTION_0 + i, m_ledEquipOption[i]);

	for (int i = 0; i < 6; i++) DDX_Control(pDX, IDC_STC_LOAD_NO_0 + i, m_stcLoadNo[i]);
	for (int i = 0; i < 6; i++) DDX_Control(pDX, IDC_STC_ASSY_NO_0 + i, m_stcAssyNo[i]);
	for (int i = 0; i < 6; i++) DDX_Control(pDX, IDC_STC_TRANS_NO_0 + i, m_stcTransNo[i]);
	for (int i = 0; i < 6; i++) DDX_Control(pDX, IDC_STC_TSTAGE_NO_0 + i, m_stcTStageNo[i]);
	for (int i = 0; i < 6; i++) DDX_Control(pDX, IDC_STC_UNLOAD_NO_0 + i, m_stcUnloadNo[i]);

	for (int i = 0; i < 3; i++) DDX_Control(pDX, IDC_LED_INDEX_DONE_0 + i, m_ledIndexDone[i]);
	DDX_Control(pDX, IDC_STC_MAIN_INDEX_POS, m_stcIndexPos);

	DDX_Control(pDX, IDC_STC_CYCLE_TIME, m_stcCycleTime);

	DDX_Control(pDX, IDC_RDO_WORK_START, m_rdoWorkStart);
	DDX_Control(pDX, IDC_RDO_WORK_STOP, m_rdoWorkStop);
	DDX_Control(pDX, IDC_LED_INIT_COMPLETE, m_ledInitComplete);
	DDX_Control(pDX, IDC_LED_LOT_LOAD_ENABLE, m_ledLotLoadEnable);
	DDX_Control(pDX, IDC_CHK_CYCLE_STOP, m_chkCycleStop);
	DDX_Control(pDX, IDC_CHK_MES_USE, m_chkMesUse);
	for (int i = 0; i < 4; i++) DDX_Control(pDX, IDC_PIC_TRAY_BACK_0 + i, m_picTrayBack[i]);

	DDX_Control(pDX, IDC_STC_LOAD_TRAY_COUNT, m_stcLoadTrayCount);
	DDX_Control(pDX, IDC_STC_CAP_TRAY_COUNT, m_stcCapTrayCount);
	DDX_Control(pDX, IDC_STC_SHIP_TRAY_COUNT, m_stcShipTrayCount);
	DDX_Control(pDX, IDC_STC_SHIP_TRAY_COUNT_NG, m_stcNGTrayCount);

	DDX_Control(pDX, IDC_GRD_LOAD_TRAY, m_grdLoadTray);
	DDX_Control(pDX, IDC_GRD_CAP_TRAY, m_grdCapTray);
	DDX_Control(pDX, IDC_GRD_SHIP_TRAY , m_grdShipTray);
	DDX_Control(pDX, IDC_GRD_SHIP_TRAY_NG , m_grdNGTray);

	for (int i = 0; i < 16; i++) DDX_Control(pDX, IDC_STC_WORK_CASE_0 + i, m_stcWorkCase[i]);
	for (int i = 0; i < 12; i++) DDX_Control(pDX, IDC_STC_PORT_NO_0 + i, m_stcPortNo[i]);
	for (int i = 0; i < 4; i++) DDX_Control(pDX, IDC_STC_TAKT_0 + i, m_stcTakt[i]);
	for (int i = 0; i < 4; i++) DDX_Control(pDX, IDC_STC_UPH_0 + i, m_stcUph[i]);
	for (int i = 0; i < 2; i++) DDX_Control(pDX, IDC_STC_DAY_0 + i, m_stcDay[i]);
	for (int i = 0; i < 2; i++) DDX_Control(pDX, IDC_LED_VISION_STATUS_0 + i, m_ledVisionStatus[i]);
	for (int i = 0; i < 4; i++) DDX_Control(pDX, IDC_STC_VISION_RESULT_0 + i, m_stcVisionResult[i]);
	DDX_Control(pDX, IDC_BTN_CAPSW1, m_BtnCapSW1);
	DDX_Control(pDX, IDC_BTN_CAPSW2, m_BtnCapSW2);
	DDX_Control(pDX, IDC_BTN_CAPSW3, m_BtnCapSW3);
	DDX_Control(pDX, IDC_BTN_CAPF1, m_BtnCapSW1F);
	DDX_Control(pDX, IDC_BTN_CAPF2, m_BtnCapSWF2);
	DDX_Control(pDX, IDC_BTN_UNLOAD1, m_BtnUnloadSW1);
	DDX_Control(pDX, IDC_BTN_UNLOAD2, m_BtnUnloadSW2);
	DDX_Control(pDX, IDC_BTN_UNLOADF1, m_BtnUnloadSWF1);
	DDX_Control(pDX, IDC_BTN_UNLOADF2, m_BtnUnloadSWF2);
}

BEGIN_MESSAGE_MAP(CWorkDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_STN_CLICKED(IDC_LBL_LOT_0, &CWorkDlg::OnStnClickedLblLot0)
	ON_STN_CLICKED(IDC_LBL_LOT_3, &CWorkDlg::OnStnClickedLblLot3)
	ON_CONTROL_RANGE(STN_CLICKED, IDC_STC_LOT_ID_0, IDC_STC_LOT_ID_1, OnStcLotIdClick)
	ON_CONTROL_RANGE(STN_CLICKED, IDC_STC_CM_COUNT_0, IDC_STC_CM_COUNT_1, OnStcCmCountClick)
	ON_STN_CLICKED(IDC_STC_CAP_LOT_ID_S, &CWorkDlg::OnStnClickedCapLotIdS)
	ON_STN_CLICKED(IDC_STC_CAP_TRAY_COUNT_S, &CWorkDlg::OnStnClickedCapTrayCountS)
	ON_BN_CLICKED(IDC_BTN_CAP_CLEAR, &CWorkDlg::OnBnClickedCapClear)
	ON_STN_CLICKED(IDC_STC_SHIP_LOT_ID_S, &CWorkDlg::OnStnClickedShipLotIdS)
	ON_STN_CLICKED(IDC_STC_SHIP_TRAY_COUNT_S, &CWorkDlg::OnStnClickedShipTrayCountS)
	ON_BN_CLICKED(IDC_BTN_SHIP_CLEAR, &CWorkDlg::OnBnClickedShipClear)
	ON_BN_CLICKED(IDC_RDO_WORK_START, &CWorkDlg::OnBnClickedRdoWorkStart)
	ON_BN_CLICKED(IDC_RDO_WORK_STOP, &CWorkDlg::OnBnClickedRdoWorkStop)
	ON_BN_CLICKED(IDC_CHK_CYCLE_STOP, &CWorkDlg::OnBnClickedChkCycleStop)
	ON_BN_CLICKED(IDC_CHK_MES_USE, &CWorkDlg::OnBnClickedChkMesUse)
	ON_MESSAGE(UM_UPDATE_MODEL, &CWorkDlg::OnUpdateModel)
	ON_MESSAGE(UM_UPDATE_TRAY_INFO, &CWorkDlg::OnUpdateTrayInfo)
	ON_MESSAGE(UM_UPDATE_BARCODE, &CWorkDlg::OnUpdateBarcode)
	ON_MESSAGE(UM_UPDATE_LOADCELL, &CWorkDlg::OnUpdateLoadCell)
	ON_MESSAGE(UM_RESET_CYCLE_STOP, &CWorkDlg::OnResetCycleStop)
	ON_MESSAGE(UM_LOT_START_END, &CWorkDlg::OnLotStartEnd)
	ON_MESSAGE(UM_UPDATE_UPH, &CWorkDlg::OnUpdateUph)
	ON_MESSAGE(UM_VISION_RESULT, &CWorkDlg::OnVisionResult)
	ON_MESSAGE(UM_INDEX_TACK, &CWorkDlg::OnIndexTack)
	ON_MESSAGE(UM_SHOW_MSG, &CWorkDlg::OnShowMsg)
	ON_BN_CLICKED(IDC_BUTTON1, &CWorkDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CWorkDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BTN_MES_CANCEL, &CWorkDlg::OnBnClickedMesCancel)
	ON_STN_CLICKED(IDC_STC_LOAD_CELL_COUNT, &CWorkDlg::OnStnClickedLoadCellCnt)
	ON_BN_CLICKED(IDC_BTN_LOAD_CELL_EDIT, &CWorkDlg::OnBnClickedLoadCellEdit)
	ON_STN_CLICKED(IDC_LBL_CAP_LOT_0, &CWorkDlg::OnStnClickedLblCapLot0)
	ON_STN_CLICKED(IDC_LBL_SHIP_LOT_0, &CWorkDlg::OnStnClickedLblShipLot0)
	ON_BN_CLICKED(IDC_BTN_LOTSTART1, &CWorkDlg::OnBnClickedBtnLotstart1)
	ON_BN_CLICKED(IDC_BTN_LOTSTART2, &CWorkDlg::OnBnClickedBtnLotstart2)
	ON_BN_CLICKED(IDC_BTN_CAPSW1, &CWorkDlg::OnBnClickedBtnCapsw1)
	ON_BN_CLICKED(IDC_BTN_CAPSW2, &CWorkDlg::OnBnClickedBtnCapsw2)
	ON_BN_CLICKED(IDC_BTN_CAPSW3, &CWorkDlg::OnBnClickedBtnCapsw3)
	ON_BN_CLICKED(IDC_BTN_CAPF1, &CWorkDlg::OnBnClickedBtnCapf1)
	ON_BN_CLICKED(IDC_BTN_CAPF2, &CWorkDlg::OnBnClickedBtnCapf2)
	ON_BN_CLICKED(IDC_BTN_UNLOAD1, &CWorkDlg::OnBnClickedBtnUnload1)
	ON_BN_CLICKED(IDC_BTN_UNLOAD2, &CWorkDlg::OnBnClickedBtnUnload2)
	ON_BN_CLICKED(IDC_BTN_UNLOADF1, &CWorkDlg::OnBnClickedBtnUnloadf1)
END_MESSAGE_MAP()

// CWorkDlg 메시지 처리기입니다.

void CWorkDlg::Initial_Controls() 
{
	CString strText;
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	for (int i = 0; i < 10; i++) m_Group[i].Init_Ctrl("바탕", 12, TRUE, COLOR_DEFAULT, COLOR_DEFAULT);
	for (int i = 0; i < 4; i++) m_Label[i].Init_Ctrl("바탕", 11, FALSE, RGB(0xFF, 0xFF, 0xFF), RGB(0x80, 0x80, 0x80));

	for (int i = 0; i < 2; i++) m_grpLot[i].Init_Ctrl("바탕", 12, TRUE, COLOR_DEFAULT, COLOR_DEFAULT);
	for (int i = 0; i < 3; i++) m_lblLot[i].Init_Ctrl("바탕", 11, FALSE, RGB(0xFF, 0xFF, 0xFF), RGB(0x20, 0x20, 0x80));
	for (int i = 3; i < 6; i++) m_lblLot[i].Init_Ctrl("바탕", 11, FALSE, RGB(0xFF, 0xFF, 0xFF), RGB(0x40, 0x00, 0x80));

	for (int i = 0; i < 2; i++) m_stcLotId[i].Init_Ctrl("Arial", 10, TRUE, COLOR_DEFAULT, RGB(0x80, 0xF0, 0xF0));
	for (int i = 0; i < 2; i++) m_stcTrayCount[i].Init_Ctrl("바탕", 12, TRUE, COLOR_DEFAULT, RGB(0xD0, 0xD0, 0xD0));
	for (int i = 0; i < 2; i++) m_stcCmCount[i].Init_Ctrl("바탕", 12, TRUE, COLOR_DEFAULT, RGB(0x80, 0xF0, 0xF0));

	for (int i = 0; i < 3; i++) m_lblCapLot[i].Init_Ctrl("바탕", 11, FALSE, RGB(0xFF, 0xFF, 0xFF), RGB(0x20, 0x20, 0x80));
	m_stcCapLotIdR.Init_Ctrl("Arial", 10, TRUE, COLOR_DEFAULT, RGB(0xD0, 0xD0, 0xD0));
	m_stcCapTrayCountR.Init_Ctrl("바탕", 12, TRUE, COLOR_DEFAULT, RGB(0xD0, 0xD0, 0xD0));
	m_stcCapCmCountR.Init_Ctrl("바탕", 12, TRUE, COLOR_DEFAULT, RGB(0xD0, 0xD0, 0xD0));
	m_stcCapLotIdS.Init_Ctrl("Arial", 10, TRUE, COLOR_DEFAULT, RGB(0x80, 0xF0, 0xF0));
	m_stcCapTrayCountS.Init_Ctrl("바탕", 12, TRUE, COLOR_DEFAULT, RGB(0x80, 0xF0, 0xF0));
	m_stcCapCmCountS.Init_Ctrl("바탕", 12, TRUE, COLOR_DEFAULT, RGB(0xD0, 0xD0, 0xD0));

	for (int i = 0; i < 3; i++) m_lblShipLot[i].Init_Ctrl("바탕", 11, FALSE, RGB(0xFF, 0xFF, 0xFF), RGB(0x20, 0x20, 0x80));
	m_stcShipLotIdR.Init_Ctrl("Arial", 10, TRUE, COLOR_DEFAULT, RGB(0xD0, 0xD0, 0xD0));
	m_stcShipTrayCountR.Init_Ctrl("바탕", 12, TRUE, COLOR_DEFAULT, RGB(0xD0, 0xD0, 0xD0));
	m_stcShipCmCountR.Init_Ctrl("바탕", 12, TRUE, COLOR_DEFAULT, RGB(0xD0, 0xD0, 0xD0));
	m_stcShipLotIdS.Init_Ctrl("Arial", 10, TRUE, COLOR_DEFAULT, RGB(0x80, 0xF0, 0xF0));
	m_stcShipTrayCountS.Init_Ctrl("바탕", 12, TRUE, COLOR_DEFAULT, RGB(0x80, 0xF0, 0xF0));
	m_stcShipCmCountS.Init_Ctrl("바탕", 12, TRUE, COLOR_DEFAULT, RGB(0xD0, 0xD0, 0xD0));

	m_bmpEquipment.LoadBitmap(IDB_EQUIP_WORK);
	m_imgEquipment.SetBitmap(m_bmpEquipment);
	m_picUphBack.Init_Ctrl(COLOR_DEFAULT, COLOR_DEFAULT);
	for (int i = 0; i < 7; i++) m_ledEquipOption[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, CLedCS::emBlue, CLedCS::em16);
	for (int i = 0; i < 6; i++) m_stcLoadNo[i].Init_Ctrl("바탕", 12, TRUE, RGB(0x00, 0xFF, 0x00), RGB(0x00, 0x00, 0x00));
	for (int i = 0; i < 6; i++) m_stcAssyNo[i].Init_Ctrl("바탕", 12, TRUE, RGB(0x00, 0xFF, 0x00), RGB(0x00, 0x00, 0x00));
	for (int i = 0; i < 6; i++) m_stcTransNo[i].Init_Ctrl("바탕", 12, TRUE, RGB(0x00, 0xFF, 0x00), RGB(0x00, 0x00, 0x00));
	for (int i = 0; i < 6; i++) m_stcTStageNo[i].Init_Ctrl("바탕", 12, TRUE, RGB(0x00, 0xFF, 0x00), RGB(0x00, 0x00, 0x00));
	for (int i = 0; i < 6; i++) m_stcUnloadNo[i].Init_Ctrl("바탕", 12, TRUE, RGB(0x00, 0xFF, 0x00), RGB(0x00, 0x00, 0x00));

	for (int i = 0; i < 3; i++) m_ledIndexDone[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, CLedCS::emGreen, CLedCS::em24);
	m_stcIndexPos.Init_Ctrl("바탕", 12, TRUE, RGB(0x00, 0xFF, 0x00), RGB(0x00, 0x00, 0x00));

	m_stcCycleTime.Init_Ctrl("바탕", 12, TRUE, RGB(0x00, 0xFF, 0x00), RGB(0x00, 0x00, 0x00));
	m_rdoWorkStart.Init_Ctrl("바탕", 20, TRUE, COLOR_DEFAULT, COLOR_DEFAULT, 0, 0);
	m_rdoWorkStop.Init_Ctrl("바탕", 20, TRUE, COLOR_DEFAULT, COLOR_DEFAULT, 0, 0);
	m_ledInitComplete.Init_Ctrl("바탕", 12, TRUE, COLOR_DEFAULT, COLOR_DEFAULT, CLedCS::emGreen, CLedCS::em24);
	m_ledLotLoadEnable.Init_Ctrl("바탕", 10, TRUE, COLOR_DEFAULT, COLOR_DEFAULT, CLedCS::emGreen, CLedCS::em16);
	m_chkCycleStop.Init_Ctrl("바탕", 11, TRUE, COLOR_DEFAULT, COLOR_DEFAULT, 0, 0);
	m_chkMesUse.Init_Ctrl("Arial", 12, TRUE, RGB(0xFF, 0xFF, 0x00), RGB(0xC0, 0x10, 0x30), CCheckCS::emRed, CCheckCS::emRight);
	for (int i = 0; i < 4; i++) m_picTrayBack[i].Set_Color(COLOR_DEFAULT, RGB(0xF0, 0xF0, 0x80));
	
	m_stcLoadTrayCount.Init_Ctrl("바탕", 11, TRUE, RGB(0xFF, 0xFF, 0xFF), RGB(0x00, 0x00, 0x00));
	m_stcCapTrayCount.Init_Ctrl("바탕", 11, TRUE, RGB(0xFF, 0xFF, 0xFF), RGB(0x00, 0x00, 0x00));
	m_stcShipTrayCount.Init_Ctrl("바탕", 11, TRUE, RGB(0xFF, 0xFF, 0xFF), RGB(0x00, 0x00, 0x00));
	m_stcNGTrayCount.Init_Ctrl("바탕", 11, TRUE, RGB(0xFF, 0xFF, 0xFF), RGB(0x00, 0x00, 0x00));

	Initial_Grid(&m_grdLoadTray, LT_Y, LT_X);
	Initial_Grid(&m_grdCapTray, CT_Y, CT_X);
	Initial_Grid(&m_grdShipTray, ST_Y, ST_X);
	Initial_Grid(&m_grdNGTray, ST_Y, ST_X);
	for (int i = 0; i < 16; i++) m_stcWorkCase[i].Init_Ctrl("바탕", 11, TRUE, RGB(0xFF, 0xFF, 0xFF), RGB(0x40, 0x40, 0x40));
	for (int i = 0; i < 12; i++) m_stcPortNo[i].Init_Ctrl("바탕", 11, TRUE, RGB(0xFF, 0xFF, 0xFF), RGB(0x40, 0x40, 0x40));
	for (int i = 0; i < 4; i++) m_stcTakt[i].Init_Ctrl("Arial", 9, FALSE, RGB(0xFF, 0xFF, 0xFF), RGB(0x40, 0x40, 0x40));
	for (int i = 0; i < 4; i++) m_stcUph[i].Init_Ctrl("Arial", 9, FALSE, RGB(0xFF, 0xFF, 0xFF), RGB(0x40, 0x40, 0x40));
	for (int i = 0; i < 2; i++) m_stcDay[i].Init_Ctrl("Arial", 9, FALSE, RGB(0xFF, 0xFF, 0xFF), RGB(0x40, 0x40, 0x40));
	for (int i = 0; i < 2; i++) m_ledVisionStatus[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, CLedCS::emGreen, CLedCS::em16);
	for (int i = 0; i < 4; i++) m_stcVisionResult[i].Init_Ctrl("바탕", 11, TRUE, RGB(0xFF, 0xFF, 0xFF), RGB(0x00, 0x00, 0x00));
	m_stcLoadCellCnt.Init_Ctrl("바탕", 12, TRUE, COLOR_DEFAULT, RGB(0x80, 0xF0, 0xF0));
}

BOOL CWorkDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetWindowPos(this, 0, 75, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	Initial_Controls();

	m_pWorkInfoDlg = new CWorkInfoDlg(this);
	m_pWorkInfoDlg->Create(IDD_WORK_INFO_DLG, this);

	m_bAutoRunning = FALSE;
	m_nGroupNo = 0;

	gData.bLotEndBeep = FALSE;
	gData.bFirstLotStart = FALSE;
	gData.nCapTrayCount = 0;

	m_rdoWorkStop.SetCheck(TRUE);
	m_rdoWorkStop.Set_Color(RGB(0xFF, 0x00, 0x00), COLOR_DEFAULT);

	m_bCapLotIdInput = FALSE;
	m_bShipLotIdInput = FALSE;

#ifndef AJIN_BOARD_USE
	m_stcShipTrayCountR.SetWindowText("99");
	m_stcShipLotIdR.SetWindowText("5PKQFFFGGGGRT/5555/DDDDGGGGHHHH");
	m_stcShipCmCountR.SetWindowText("5555");

	m_stcCapTrayCountR.SetWindowText("99");
	m_stcCapLotIdR.SetWindowText("3CPPGGGGFFFF/5555/DDDDGGGGHHHH");
	m_stcCapCmCountR.SetWindowText("5555");

	m_stcShipTrayCountS.SetWindowText("99");
	m_stcShipLotIdS.SetWindowText("5PKQOOOOKKKK/5555/DDDDGGGGHHHH");
	m_stcShipCmCountS.SetWindowText("5555");

	m_stcCapTrayCountS.SetWindowText("99");
	m_stcCapLotIdS.SetWindowText("3CPPGGGGDDDD/5555/DDDDGGGGHHHH");
	m_stcCapCmCountS.SetWindowText("5555");

#else
	m_BtnCapSW1.ShowWindow(FALSE);
	m_BtnCapSW2.ShowWindow(FALSE);
	m_BtnCapSW3.ShowWindow(FALSE);
	m_BtnCapSW1F.ShowWindow(FALSE);
	m_BtnCapSWF2.ShowWindow(FALSE);
	m_BtnUnloadSW1.ShowWindow(FALSE);
	m_BtnUnloadSW2.ShowWindow(FALSE);
	m_BtnUnloadSWF1.ShowWindow(FALSE);
	m_BtnUnloadSWF2.ShowWindow(FALSE);

#endif
	


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CWorkDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CWorkDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	
	KillTimer(0);

	m_pWorkInfoDlg->DestroyWindow();
	delete m_pWorkInfoDlg;
	m_pWorkInfoDlg = NULL;
}

void CWorkDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow) 
	{
		Change_Model();	

		g_objAviHandler.Set_ConnectRequest();

		EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();
		CString strText;
		
		m_pWorkInfoDlg->ShowWindow(SW_SHOW);

#ifndef DRY_RUN_TEST
		m_bmpEquipment.DeleteObject();
		if (pEquipData->bUseDoorLock) m_bmpEquipment.LoadBitmap(IDB_EQUIP_WORK);
		else m_bmpEquipment.LoadBitmap(IDB_EQUIP_DOOR);
		m_imgEquipment.SetBitmap(m_bmpEquipment);
#endif

		m_stcLotId[0].GetWindowText(strText);
		if (strText.GetLength() < 1) {
			if (gData.sLotID[0].GetLength() < 1) gData.sLotID[0] = "CLOT_ID";
			m_stcLotId[0].SetWindowText(gData.sLotID[0]);
		}

		strText.Format("%d", gData.nTrayUseCount[0]);
		m_stcTrayCount[0].SetWindowText(strText);

		strText.Format("%d", gData.nCmUseCount[0]);
		m_stcCmCount[0].SetWindowText(strText);

		m_stcLotId[1].GetWindowText(strText);
		if (strText.GetLength() < 1) {
			if (gData.sLotID[1].GetLength() < 1) gData.sLotID[1] = "CLOT_ID";
			m_stcLotId[1].SetWindowText(gData.sLotID[1]);
		}

		strText.Format("%d", gData.nTrayUseCount[1]);
		m_stcTrayCount[1].SetWindowText(strText);

		strText.Format("%d", gData.nCmUseCount[1]);
		m_stcCmCount[1].SetWindowText(strText);

		strText.Format("%d", pEquipData->nLoadCellChkCnt);
		m_stcLoadCellCnt.SetWindowText(strText);
		m_stcLoadCellCnt.EnableWindow(FALSE);

		m_chkMesUse.SetCheck(pEquipData->bUseMES);

		m_ledEquipOption[0].Set_On(pEquipData->bUseInlineMode);
		m_ledEquipOption[1].Set_On(pEquipData->bUseVisionCapDir);
		m_ledEquipOption[2].Set_On(pEquipData->bUseVisionCmAlign);
		m_ledEquipOption[3].Set_On(pEquipData->bUseTrayPickerTurn);
		m_ledEquipOption[4].Set_On(pEquipData->bUseCapPickerTurn);
		m_ledEquipOption[5].Set_On(pEquipData->bUseMesCapReg);
		m_ledEquipOption[6].Set_On(pEquipData->bUseMesShipReg);

		m_grpLot[0].SetWindowText("Port 1 Lot Info");
		m_stcLotId[0].Set_Color(COLOR_DEFAULT, RGB(0x80, 0xF0, 0xF0));
		m_stcCmCount[0].Set_Color(COLOR_DEFAULT, RGB(0x80, 0xF0, 0xF0));

		m_grpLot[1].SetWindowText("Port 2 Lot Info");
		m_stcLotId[1].Set_Color(COLOR_DEFAULT, RGB(0x80, 0xF0, 0xF0));
		m_stcCmCount[1].Set_Color(COLOR_DEFAULT, RGB(0x80, 0xF0, 0xF0));

		g_objCommon.Locking_MainDoor(FALSE);

		SetTimer(0, 100, NULL);
		SetTimer(1, 5000, NULL);

	} else {
		KillTimer(0);
		m_pWorkInfoDlg->ShowWindow(SW_HIDE);
	}
}

void CWorkDlg::OnTimer(UINT_PTR nIDEvent)
{
	KillTimer(0);
	KillTimer(1);
	
	CCME8000Dlg *pMainDlg = (CCME8000Dlg*)AfxGetApp()->GetMainWnd();
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	if( nIDEvent == 1)
	{
		g_objAviHandler.Set_ConnectRequest();
		if (pEquipData->bUseVisionCmAlign) g_objInspector.Set_StatusRequest();
	}
	DX_DATA_12 *pDX12 = g_objAJinAXL.Get_pDX12();
	
	if (pDX12->iStartSw && !m_rdoWorkStart.GetCheck()) 
	{
		g_objLogFile.Save_HandlerLog("[Work Mode] START S/W push");
		m_rdoWorkStart.SetCheck(TRUE);
		pMainDlg->Set_LotErrorLog("START", 903, "Start");
		
	} 
	else if (pDX12->iStopSw && !m_rdoWorkStop.GetCheck()) 
	{
		g_objLogFile.Save_HandlerLog("[Work Mode] STOP S/W push");
		MachineStopLog("STOP_BUTTON_PUSH");
		m_rdoWorkStop.SetCheck(TRUE);
		pMainDlg->Set_LotErrorLog("STOP", 904, "Stop");		
	}

	if (pDX12->iResetSw) g_objCommon.Show_Alarm("", STATE_ALARM, FALSE);	// Alarm Off

	Check_Lamp();	// Load1/2, NG, Good Port Lamp Switch
	Display_Status();

	if (m_rdoWorkStart.GetCheck()) {
		if (!m_bAutoRunning) {	// First AutoRun
			if (!g_objSequenceInit.Get_InitComplete()) { g_objCommon.Show_Error(40); SetTimer(0, 100, NULL); return; }

			if (!Work_Start()) { m_rdoWorkStop.SetCheck(TRUE); SetTimer(0, 100, NULL); return; }

			m_bAutoRunning = TRUE;

			BOOL bLoad1 = g_objSequenceMain.Get_LotLoadEnable(0);
			BOOL bLoad2 = g_objSequenceMain.Get_LotLoadEnable(1);
			if (!bLoad1)					g_objCommon.Locking_PortSlide(TRUE, 1);
			if (!bLoad2)					g_objCommon.Locking_PortSlide(TRUE, 2);
			if (!gData.bLoadPort3Wait)		g_objCommon.Locking_PortSlide(TRUE, 3);
			if (!gData.bCapPort1Wait)		g_objCommon.Locking_PortSlide(TRUE, 4);
			if (!gData.bCapPort2Wait)		g_objCommon.Locking_PortSlide(TRUE, 5);
			if (!gData.bUnloadPort1Wait)	g_objCommon.Locking_PortSlide(TRUE, 6);
			if (!gData.bUnloadPort2Wait)	g_objCommon.Locking_PortSlide(TRUE, 7);
					
			g_objCommon.Locking_MainDoor(TRUE, TRUE);
			pMainDlg->Enable_ModeButton(FALSE);
			if (gAlm.bBegin) Reset_AlarmLog();
			pMainDlg->Set_CurrentState(STATE_RUN);

			g_objSequenceMain.Begin_MainRunThread();
			pMainDlg->Set_EquipRunStart();
			MachineStopLog("RUN_START");

		} else {				// Auto Running
			if (!g_objSequenceMain.Is_MainThreadRun()) {
				g_objLogFile.Save_HandlerLog("[Work Mode] Auto STOP");
				pMainDlg->Set_CurrentState(STATE_STOP);
			}
		}

	} else if (m_rdoWorkStop.GetCheck()) {
		if (m_bAutoRunning) {	// First AutoStop
			m_bAutoRunning = FALSE;

			g_objSequenceMain.End_MainRunThread();

			int nState = theApp.Get_MainState();
			if (nState != STATE_ALARM && nState != STATE_ERROR) pMainDlg->Set_CurrentState(STATE_STOP);

			m_rdoWorkStart.Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
			m_rdoWorkStop.Set_Color(RGB(0xFF, 0x00, 0x00), COLOR_DEFAULT);

			pMainDlg->Enable_ModeButton(TRUE);
			g_objCommon.Locking_MainDoor(FALSE);
			g_objCommon.Locking_PortSlide(FALSE);

			pMainDlg->Save_EquipRunTime();
			g_objCommon.Save_MotionPos();

		} else {				// Stop
			int nState = theApp.Get_MainState();
			if (nState != STATE_ERROR) g_objCommon.Check_MainEmgAir();
		}
	}

	SetTimer(0, 100, NULL);
	SetTimer(1, 5000, NULL);

	//int nMode = theApp.Get_MainMode();
	//if (nMode == MODE_OPERATOR || nMode == MODE_WORK)
	//{	
	//	SetTimer(0, 100, NULL);
	//	SetTimer(1, 5000, NULL);
	//}
	//else
	//{
	//	KillTimer(0);
	//	//KillTimer(1);
	//}
	CDialogEx::OnTimer(nIDEvent);
}

void CWorkDlg::OnStnClickedLblLot0()
{
	m_nGroupNo = 0;

	m_Group[0].Set_Color(RGB(0xFF, 0x00, 0x00), COLOR_DEFAULT);
	m_Group[1].Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
	m_Group[6].Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
	m_Group[9].Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
	m_bCapLotIdInput = FALSE;
	m_bShipLotIdInput = FALSE;
}

void CWorkDlg::OnStnClickedLblLot3()
{
	m_nGroupNo = 1;

	m_Group[0].Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
	m_Group[1].Set_Color(RGB(0xFF, 0x00, 0x00), COLOR_DEFAULT);
	m_Group[6].Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
	m_Group[9].Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
	m_bCapLotIdInput = FALSE;
	m_bShipLotIdInput = FALSE;
}

void CWorkDlg::OnStnClickedLblCapLot0()
{
	m_Group[0].Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
	m_Group[1].Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
	m_Group[6].Set_Color(RGB(0xFF, 0x00, 0x00), COLOR_DEFAULT);
	m_Group[9].Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
	m_bCapLotIdInput = TRUE;
	m_bShipLotIdInput = FALSE;
}

void CWorkDlg::OnStnClickedLblShipLot0()
{
	m_Group[0].Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
	m_Group[1].Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
	m_Group[6].Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
	m_Group[9].Set_Color(RGB(0xFF, 0x00, 0x00), COLOR_DEFAULT);
	m_bCapLotIdInput = FALSE;
	m_bShipLotIdInput = TRUE;
}

void CWorkDlg::OnStcLotIdClick(UINT nID)
{
	int ID = nID - IDC_STC_LOT_ID_0;

	CString strKey;
	if (g_objCommon.Show_KeyPad(strKey) != IDOK) return;

	m_stcLotId[ID].SetWindowText(strKey);
//	g_dlgOperator.m_stcOperLotId.SetWindowText(strKey);
}

void CWorkDlg::OnStcCmCountClick(UINT nID)
{
	int ID = nID - IDC_STC_CM_COUNT_0;

	CString strOld, strNew, strValue;

	m_stcCmCount[ID].GetWindowText(strOld);
	if (g_objCommon.Show_NumPad(strOld, strNew) != IDOK) return;

	int nCmCnt = atoi(strNew);

	//simul 
#ifndef AJIN_BOARD_USE
	if(nCmCnt > 0 && ID == 0) {
		DX_DATA_00 *pDX00 = g_objAJinAXL.Get_pDX00();
		pDX00->iLoadPort1LowCheck = TRUE;
	}

#endif

	int nTrayUseCount = nCmCnt / gData.nCmMaxCount;
	if (nCmCnt % gData.nCmMaxCount) nTrayUseCount++;
	strValue.Format("%d", nTrayUseCount);
	m_stcTrayCount[ID].SetWindowText(strValue);
// 	g_dlgOperator.m_stcOperTrayCount.SetWindowText(strValue);



	strValue.Format("%d", nCmCnt);
	m_stcCmCount[ID].SetWindowText(strValue);
// 	g_dlgOperator.m_stcOperCmCount.SetWindowText(strValue);
}

void CWorkDlg::OnStnClickedCapLotIdS()
{
	CString strKey;
	if (g_objCommon.Show_KeyPad(strKey) != IDOK) return;

	int nCapCnt = Check_CapShipLotID(1, strKey);
	if (nCapCnt < 1) return;

	m_stcCapLotIdS.SetWindowText(strKey);
}

void CWorkDlg::OnStnClickedCapTrayCountS()
{
	CString strOld, strNew, strValue;

	m_stcCapTrayCountS.GetWindowText(strOld);
	if (g_objCommon.Show_NumPad(strOld, strNew) != IDOK) return;

	if (atoi(strNew) > 100) return;
	gData.nCapTrayMax = atoi(strNew);

#ifndef AJIN_BOARD_USE
	if(gData.nCapTrayMax > 0)
	{
		DX_DATA_02 *pDX02 = g_objAJinAXL.Get_pDX02();
		pDX02->iCapPort1LowCheck = TRUE;

	}
#endif

	strValue.Format("%d", gData.nCapTrayMax);
	m_stcCapTrayCountS.SetWindowText(strValue);

	strValue.Format("%d", gData.nCapTrayMax * gData.nCapMaxCount);
	m_stcCapCmCountS.SetWindowText(strValue);
}

void CWorkDlg::OnBnClickedCapClear()
{
	if (g_objCommon.Show_MsgBox(2, "Cap Lot 정보를 초기화 하시겠습니까?") != IDOK) return;

	m_stcCapLotIdR.SetWindowText("");
	m_stcCapCmCountR.SetWindowText("");
	m_stcCapTrayCountR.SetWindowText("");

	m_stcCapLotIdS.SetWindowText("");
	m_stcCapCmCountS.SetWindowText("");
	m_stcCapTrayCountS.SetWindowText("");

	gData.sCapLotID.Empty();
	gData.nCapTrayLoad = 0;
	gData.nCapTrayMax = 0;
}

void CWorkDlg::OnStnClickedShipLotIdS()
{
	CString strKey;
	if (g_objCommon.Show_KeyPad(strKey) != IDOK) return;

	int nCapCnt = Check_CapShipLotID(2, strKey);
	if (nCapCnt < 1) return;

	m_stcShipLotIdS.SetWindowText(strKey);
}

void CWorkDlg::OnStnClickedShipTrayCountS()
{
	CString strOld, strNew, strValue;

	m_stcShipTrayCountS.GetWindowText(strOld);
	if (g_objCommon.Show_NumPad(strOld, strNew) != IDOK) return;

	if (atoi(strNew) > 100) return;
	gData.nShipTrayMax = atoi(strNew);

#ifndef AJIN_BOARD_USE
	if(gData.nShipTrayMax > 0)
	{
		DX_DATA_03 *pDX03 = g_objAJinAXL.Get_pDX03();
		pDX03->iUnlaodPort1LowCheck = TRUE;
	}
#endif

	strValue.Format("%d", gData.nShipTrayMax);
	m_stcShipTrayCountS.SetWindowText(strValue);

	strValue.Format("%d", gData.nShipTrayMax * gData.nShipMaxCount);
	m_stcShipCmCountS.SetWindowText(strValue);
}

void CWorkDlg::OnBnClickedShipClear()
{
	if (g_objCommon.Show_MsgBox(2, "Ship Lot 정보를 초기화 하시겠습니까?") != IDOK) return;

	m_stcShipLotIdR.SetWindowText("");
	m_stcShipCmCountR.SetWindowText("");
	m_stcShipTrayCountR.SetWindowText("");

	m_stcShipLotIdS.SetWindowText("");
	m_stcShipCmCountS.SetWindowText("");
	m_stcShipTrayCountS.SetWindowText("");

	gData.sShipLotID.Empty();
	gData.nShipTrayLoad = 0;
	gData.nShipTrayMax = 0;
}


int CWorkDlg::Check_CapShipLotID(int nType, CString sBarID)
{
	char chSep = '/';
	CString sPart, sLotId, sCnt, sTemp;

	AfxExtractSubString(sPart, sBarID, 0, chSep);	sPart.Trim();
	AfxExtractSubString(sCnt, sBarID, 1, chSep);	sCnt.Trim();
	AfxExtractSubString(sLotId, sBarID, 2, chSep);	sLotId.Trim();
	int nCount = atoi(sCnt);

	//'/' 구분 바코드 자릿수 체크 
	if(sPart.GetLength() < 10) { // 바코드 part number 자릿수 10 이하 체크 
		sTemp.Format("Part number를 체크해 주십시오\nPart number(%s)/수량/LotID", sPart);
		g_objCommon.Show_MsgBox(1, sTemp);
		return 0;
	}
	if (g_objCommon.Check_CapShipPartNo(nType, sPart) == FALSE) {
		if (nType == 1) sTemp.Format("Cap Parts가 아닙니다. 확인해 주세요.\nPart number(%s)", sPart);
		if (nType == 2) sTemp.Format("Ship Parts가 아닙니다. 확인해 주세요.\nPart number(%s)", sPart);
		g_objCommon.Show_MsgBox(1, sTemp);
		return 0;
	}

	if(sCnt.GetLength() < 2) { // 모듈 수량 자릿수 2 이하 체크 
		sTemp.Format("수량을 체크해 주십시오\nPart number/수량(%s)/LotID", sCnt);
		g_objCommon.Show_MsgBox(1, sTemp);
		return 0;
	}
	if(nCount < 1) {
		sTemp.Format("수량을 체크해 주십시오\nPart number/수량(%d)/LotID", nCount);
		g_objCommon.Show_MsgBox(1, sTemp);
		return 0;
	}
	if(sLotId.GetLength() < 11) { // 모듈 수량 자릿수 12 이하 체크 
		sTemp.Format("Lot ID를 체크해 주십시오\nPart number/수량/LotID(%s)", sLotId);
		g_objCommon.Show_MsgBox(1, sTemp);
		return 0;
	}

	if (nType == 1) {	//Cap만 중복체크
		BOOL ret = g_objLogFile.Check_BarcodeLog(sBarID);
		if(!ret){
			sTemp.Format("2주 내에 같은 바코드를 사용하였습니다.체크 바랍니다.\n(%s)", sBarID);
			g_objCommon.Show_MsgBox(1, sTemp);
			return 0;
		}
	}

	return nCount;
}

void CWorkDlg::OnBnClickedMesCancel()
{
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	if (m_rdoWorkStart.GetCheck()) {
		g_objCommon.Show_MsgBox(1, "Auto Run 중엔 취소할 수 없습니다........");
		return;
	}
	g_objMES.m_nMESCapSequence = 0;
	g_objMES.m_nMESShipSequence = 0;
}

void CWorkDlg::OnBnClickedRdoWorkStart()
{
	g_objLogFile.Save_HandlerLog("[Work Mode] START button push");
	CCME8000Dlg *pMainDlg = (CCME8000Dlg*)AfxGetApp()->GetMainWnd();

	pMainDlg->Set_LotErrorLog("START", 903, "Start");
}

void CWorkDlg::OnBnClickedRdoWorkStop()
{
	g_objLogFile.Save_HandlerLog("[Work Mode] STOP button push");
	MachineStopLog("STOP_BUTTON_PUSH");
	CCME8000Dlg *pMainDlg = (CCME8000Dlg*)AfxGetApp()->GetMainWnd();

	pMainDlg->Set_LotErrorLog("STOP", 904, "Stop");
}

void CWorkDlg::OnBnClickedChkCycleStop()
{
	if (m_chkCycleStop.GetCheck()) {
		m_chkCycleStop.Set_Color(RGB(0xFF, 0xFF, 0xFF), RGB(0x00, 0x00, 0xFF));
		if (g_objCommon.Show_MsgBox(2, "Are you sure you want to Cycle Stop?") == IDOK) {
			gData.bCycleStop = TRUE;
		} else {
			m_chkCycleStop.Set_Color(RGB(0x00, 0x00, 0x00), RGB(0xF0, 0xF0, 0xF0));
			m_chkCycleStop.SetCheck(FALSE);
		}
	} else {
		m_chkCycleStop.Set_Color(RGB(0x00, 0x00, 0x00), RGB(0xF0, 0xF0, 0xF0));
		gData.bCycleStop = FALSE;
	}
}

void CWorkDlg::OnBnClickedChkMesUse()
{
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	if (m_rdoWorkStart.GetCheck()) {
		g_objCommon.Show_MsgBox(1, "Can't change in Auto Run........");

		m_chkMesUse.SetCheck(pEquipData->bUseMES);
		return;
	}

	CIniFileCS INI("System/EquipData.ini");
	if (!INI.Check_File()) {
		AfxMessageBox("EquipData.ini File Not Found!!!");
		return;
	}
	INI.Set_Bool("OPTION", "MES_USE", m_chkMesUse.GetCheck());
	g_objDataManager.Read_EquipData();

	g_objMES.Set_MESUse(pEquipData->bUseMES);
}

void CWorkDlg::OnStnClickedLoadCellCnt()
{
	CString strOld, strNew;
	m_stcLoadCellCnt.GetWindowText(strOld);
	if (g_objCommon.Show_NumPad(strOld, strNew) != IDOK) return;

	m_stcLoadCellCnt.SetWindowText(strNew);

	CIniFileCS INI(gsCurrentDir + "\\System\\EquipData.ini");
	if (!INI.Check_File()) { AfxMessageBox("EquipData.ini File Not Found!!!"); return; }
	
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();
	int	nCnt;

	nCnt = atoi(strNew); INI.Set_Integer("LOAD_CELL", "CHECK_COUNT", nCnt);
	pEquipData->nLoadCellChkCnt = nCnt;

	m_stcLoadCellCnt.EnableWindow(FALSE);
}

void CWorkDlg::OnBnClickedLoadCellEdit()
{
	if (m_stcLoadCellCnt.IsWindowEnabled())
		m_stcLoadCellCnt.EnableWindow(FALSE);
	else
		m_stcLoadCellCnt.EnableWindow(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
// User Functions

BOOL CWorkDlg::Work_Start()
{
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	CString strTemp, strTemp2;

	int nState = theApp.Get_MainState();
	if (nState == STATE_ERROR || nState == STATE_ALARM) return FALSE;

	if (!g_objCommon.Check_MainDoor()) return FALSE;

	if (gData.sOperID == "") { g_objCommon.Show_MsgBox(1, "작업자 ID가 입력되지 않았습니다. 확인후 진행하여 주십시오."); return FALSE; }

	DX_DATA_00 *pDX00 = g_objAJinAXL.Get_pDX00();
	DX_DATA_01 *pDX01 = g_objAJinAXL.Get_pDX01();
	DX_DATA_02 *pDX02 = g_objAJinAXL.Get_pDX02();
	DX_DATA_03 *pDX03 = g_objAJinAXL.Get_pDX03();

	if (!pDX00->iLoadPort1SlideClose)	{ g_objCommon.Show_MsgBox(1, "Load Port 1번 Slide Close 센서가 감지 되지 않습니다. Slide를 끝까지 밀어주십시오."); return FALSE; }
	if (!pDX01->iLoadPort2SlideClose)	{ g_objCommon.Show_MsgBox(1, "Load Port 2번 Slide Close 센서가 감지 되지 않습니다. Slide를 끝까지 밀어주십시오."); return FALSE; }
	if (!pDX01->iLoadPort3SlideClose)	{ g_objCommon.Show_MsgBox(1, "Load Port 3번 Slide Close 센서가 감지 되지 않습니다. Slide를 끝까지 밀어주십시오."); return FALSE; }
	if (!pDX02->iCapPort1SlideClose)	{ g_objCommon.Show_MsgBox(1, "Cap Port1 Slide Close 센서가 감지 되지 않습니다. Slide를 끝까지 밀어주십시오."); return FALSE; }
	if (!pDX02->iCapPort2SlideClose)	{ g_objCommon.Show_MsgBox(1, "Cap Port2 Slide Close 센서가 감지 되지 않습니다. Slide를 끝까지 밀어주십시오."); return FALSE; }
	if (!pDX03->iUnloadPort1SlideClose)	{ g_objCommon.Show_MsgBox(1, "Ship Port1 Slide Close 센서가 감지 되지 않습니다. Slide를 끝까지 밀어주십시오."); return FALSE; }
	if (!pDX03->iUnloadPort2SlideClose)	{ g_objCommon.Show_MsgBox(1, "Ship Port2 Slide Close 센서가 감지 되지 않습니다. Slide를 끝까지 밀어주십시오."); return FALSE; }

	int nMotionNo = g_objCommon.Check_MotionPos();
	if (nMotionNo < 99) {
		double dCurrentPos = g_objAJinAXL.Get_Position(nMotionNo);
		CString strName = g_objAJinAXL.Get_AxisName(nMotionNo);
		strTemp.Format("Motion(%s) 위치를 Check 하세요.\n이전위치(%0.3lf) != 현재위치(%0.3lf)", strName, gAlm.dMotionPos[nMotionNo], dCurrentPos);
		g_objLogFile.Save_HandlerLog(strTemp);

		g_objCommon.Show_MsgBox(1, strTemp);
		m_rdoWorkStop.SetCheck(TRUE);
		return FALSE;
	}

	if (g_objSequenceMain.Get_IsAutoRun()) return TRUE;	// Auto Run이면 스킵


	if(pEquipData->bUseInlineMode && !g_objAviHandler.Is_Connected())
	{
		g_objCommon.Show_MsgBox(1, "CAP과 AVI 통신 연결되어 있지 않습니다."); return FALSE;
	}


	if (pEquipData->bUseInlineMode) {
		//g_objSequenceMain.Set_ClearRunData(1);	// 시점 변경 (LotEnd->LotStart)
		gData.bFirstLotStart = TRUE;

		g_dlgWork.Enable_UserInput(1, FALSE);
		g_dlgWork.Enable_UserInput(2, FALSE);

	} else {
		int nLoadTray1 = g_objSequenceMain.Get_MainRunCase(AUTO_LOAD_STAGE1);
		int nLoadTray2 = g_objSequenceMain.Get_MainRunCase(AUTO_LOAD_STAGE2);
		if ((nLoadTray1 == 0 || nLoadTray2 == 0) && !pDX00->iLoadPort1LowCheck && gData.nLoadTrayCount[0] == 0) {
			g_objCommon.Show_MsgBox(1, "Please Check the Load Port."); return FALSE;
		}

		//g_objSequenceMain.Set_ClearRunData(1);	// 시점 변경 (LotEnd->LotStart)
		gData.bFirstLotStart = TRUE;

		m_stcLotId[0].GetWindowText(strTemp);		// Lot ID
		gData.sLotID[0] = strTemp;
		if (strTemp.GetLength() < 2) { g_objCommon.Show_MsgBox(1, "Port1번에 LOT ID를 입력하여 주십시오."); return FALSE; }

		m_stcCmCount[0].GetWindowText(strTemp);	// CM 수량
		int nTempCnt = atoi(strTemp);
		gData.nCmUseCount[0] = nTempCnt;
		if (nTempCnt < 1 || nTempCnt > 2400) {	g_objCommon.Show_MsgBox(1, "Port1번의 CM 수량을 확인하여 주십시오. (1 ~ 2400)"); return FALSE; }

		m_stcTrayCount[0].GetWindowText(strTemp);	// Tray Count
		gData.nTrayUseCount[0] = atoi(strTemp);

		m_stcLotId[1].GetWindowText(strTemp2);		// Lot ID
	
		gData.sLotID[1] = strTemp2;

		m_stcCmCount[1].GetWindowText(strTemp2);	// CM 수량
		int nTempCnt2 = atoi(strTemp2);
		gData.nCmUseCount[1] = nTempCnt2;

		m_stcTrayCount[1].GetWindowText(strTemp2);	// Tray Count
		gData.nTrayUseCount[1] = atoi(strTemp2);

		if (strTemp2.GetLength() > 0 && nTempCnt2 > 0) { Enable_UserInput(2, TRUE); }
		else { gData.bLoadLampOn[1] = TRUE; g_objSequenceMain.Set_LotLoadEnable(TRUE, 1); }
	}

	return TRUE;
}

void CWorkDlg::Initial_Grid(CGridCS *pGrid, int nRows, int nCols, int nNoDir)
{
	pGrid->Set_RowCount(nRows);
	pGrid->Set_ColCount(nCols);
	pGrid->Set_FixRowCount(0);
	pGrid->Set_FixColCount(0);

	long lTotalW = pGrid->Get_GridWidth();
	long lTotalH = pGrid->Get_GridHeight();
	long lCellW = lTotalW / nCols;
	long lCellH = lTotalH / nRows;

	CString str;
	for (int i = 0; i < nRows; i++) {
		pGrid->Set_RowHeight(i, lCellH);

		for (int j = 0; j < nCols; j++) {
			if (i == 0) pGrid->Set_ColWidth(j, lCellW);

			pGrid->Set_CellAlign(i, j, CELL_ALIGN_CENTER);
			if (nNoDir == 1) { str.Format(_T("%d"), (nCols-1-j) * nRows + i + 1); }
			else			 { str.Format(_T("%d"), i * nCols + j + 1); }
			
			pGrid->Set_CellText(i, j, str);
		}
	}
}

void CWorkDlg::Check_Lamp()
{
	CCME8000Dlg *pMainDlg = (CCME8000Dlg*)AfxGetApp()->GetMainWnd();
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();
	DX_DATA_00 *pDX00 = g_objAJinAXL.Get_pDX00(); DY_DATA_00 *pDY00 = g_objAJinAXL.Get_pDY00();
	DX_DATA_01 *pDX01 = g_objAJinAXL.Get_pDX01(); DY_DATA_01 *pDY01 = g_objAJinAXL.Get_pDY01();
	DX_DATA_02 *pDX02 = g_objAJinAXL.Get_pDX02(); DY_DATA_02 *pDY02 = g_objAJinAXL.Get_pDY02();
	DX_DATA_03 *pDX03 = g_objAJinAXL.Get_pDX03(); DY_DATA_03 *pDY03 = g_objAJinAXL.Get_pDY03();
	DX_DATA_07 *pDX07 = g_objAJinAXL.Get_pDX07(); DY_DATA_07 *pDY07 = g_objAJinAXL.Get_pDY07();
	DX_DATA_09 *pDX09 = g_objAJinAXL.Get_pDX09(); DY_DATA_09 *pDY09 = g_objAJinAXL.Get_pDY09();
	DX_DATA_12 *pDX12 = g_objAJinAXL.Get_pDX12(); DY_DATA_12 *pDY12 = g_objAJinAXL.Get_pDY12();
	DX_DATA_13 *pDX13 = g_objAJinAXL.Get_pDX13(); DY_DATA_13 *pDY13 = g_objAJinAXL.Get_pDY13();

	BOOL bLoad1 = g_objSequenceMain.Get_LotLoadEnable(0);
	BOOL bLoad2 = g_objSequenceMain.Get_LotLoadEnable(1);

	if (g_objSequenceMain.Get_IsAutoRun()) {	// m_rdoWorkStart.GetCheck()
		// Load Port1
		if (pDX12->iLoad1Sw && bLoad1) {
			if (pDX00->iLoadPort1SlideClose && (!pEquipData->bUseDoorLock || !pDX13->iDoor01Open)) {	// 안전 확인.
				
				CString sTemp;
				m_stcCmCount[0].GetWindowText(sTemp);	// CM 수량
				int nCnt = atoi(sTemp);
				m_stcLotId[0].GetWindowText(sTemp);		// Lot ID				
				// Port1 CM 수량 확인 및 LotID 확인
				if (nCnt > 0 && gLot.sLotID[0] != sTemp && gLot.sLotID[1] != sTemp) {
					gData.nCmUseCount[0] = nCnt;
					gData.sLotID[0] = sTemp;
					m_stcTrayCount[0].GetWindowText(sTemp);	// Tray Count
					gData.nTrayUseCount[0] = atoi(sTemp);

					gData.bLoadLampOn[0] = FALSE;
					pMainDlg->Set_LampFlicker_Load1(FALSE);
					g_objSequenceMain.Set_LotLoadEnable(FALSE, 0);
					pDY00->oLoadPort1SlideLock = TRUE; pDY00->oLoadPort1SlideUnlock = FALSE;
					g_objAJinAXL.Write_Output(0);
					if (pEquipData->bUseDoorLock) pDY13->oDoor01Unlock = FALSE; 
					g_objAJinAXL.Write_Output(13);
				} else {
					g_objCommon.Show_MsgBox(1, "Port1 Lot 정보를 확인해 주십시오.");
				}
			}
		}
		// Load Port2
		if (pDX12->iLoad2Sw && bLoad2) {
			if (pDX01->iLoadPort2SlideClose && (!pEquipData->bUseDoorLock || !pDX13->iDoor02Open)) {	// 안전 확인.
				CString sTemp;
				m_stcCmCount[1].GetWindowText(sTemp);	// CM 수량
				int nCnt = atoi(sTemp);
				m_stcLotId[1].GetWindowText(sTemp);		// Lot ID
				// Port1 CM 수량 확인 및 LotID 확인
				if (nCnt > 0 && gLot.sLotID[0] != sTemp && gLot.sLotID[1] != sTemp) {
					gData.nCmUseCount[1] = nCnt;					
					gData.sLotID[1] = sTemp;					
					m_stcTrayCount[1].GetWindowText(sTemp);	// Tray Count
					gData.nTrayUseCount[1] = atoi(sTemp);
					
					gData.bLoadLampOn[1] = FALSE;
					pMainDlg->Set_LampFlicker_Load2(FALSE);
					g_objSequenceMain.Set_LotLoadEnable(FALSE, 1);
					pDY01->oLoadPort2SlideLock = TRUE; pDY01->oLoadPort2SlideUnlock = FALSE;
					g_objAJinAXL.Write_Output(0);
					if (pEquipData->bUseDoorLock) pDY13->oDoor02Unlock = FALSE; 
					g_objAJinAXL.Write_Output(13);
				} else {
					g_objCommon.Show_MsgBox(1, "Port2 Lot 정보를 확인해 주십시오.");
				}
			}
		}
		// Load Port3
		if (pDX12->iLoad3Sw) {
			if (pDX01->iLoadPort3SlideClose && (!pEquipData->bUseDoorLock || !pDX13->iDoor04Open)) {	// 안전 확인.
				if (!gData.bLoadPort3Wait && !m_bLoadSw3On) {
					pMainDlg->Set_LampFlicker_Load3(TRUE);
					pDY01->oLoadPort3SlideLock = FALSE; pDY01->oLoadPort3SlideUnlock = TRUE;
					g_objAJinAXL.Write_Output(1);
					if (pEquipData->bUseDoorLock) pDY13->oDoor04Unlock = TRUE; 
					g_objAJinAXL.Write_Output(13);

					gData.bLoadPort3Wait = TRUE;
					m_bLoadSw3On = TRUE;

				} else if (gData.bLoadPort3Wait && !m_bLoadSw3On) {
					pMainDlg->Set_LampFlicker_Load3(FALSE);
					pDY01->oLoadPort3SlideLock = TRUE; pDY01->oLoadPort3SlideUnlock = FALSE;
					g_objAJinAXL.Write_Output(1);
					if (pEquipData->bUseDoorLock) pDY13->oDoor04Unlock = FALSE; 
					g_objAJinAXL.Write_Output(13);

					gData.bLoadPort3Wait = FALSE;
					m_bLoadSw3On = TRUE;
				}
			}
		} else {
			m_bLoadSw3On = FALSE;
		}
		// Cap Port
		if (pDX12->iCap1Sw) {
			if (pDX02->iCapPort1SlideClose && (!pEquipData->bUseDoorLock || !pDX13->iDoor11Open)) {	// 안전 확인.
				if (!gData.bCapPort1Wait && !m_bCapSw1On) {
					pMainDlg->Set_LampFlicker_Cap1(TRUE);
					pDY02->oCapPort1SlideLock = FALSE; pDY02->oCapPort1SlideUnlock = TRUE;
					g_objAJinAXL.Write_Output(2);
					if (pEquipData->bUseDoorLock) pDY13->oDoor11Unlock = TRUE; 
					g_objAJinAXL.Write_Output(13);

					gData.bCapPort1Wait = TRUE;
					m_bCapSw1On = TRUE;

				} else if (gData.bCapPort1Wait && !m_bCapSw1On) {
					pMainDlg->Set_LampFlicker_Cap1(FALSE);
					pDY02->oCapPort1SlideLock = TRUE; pDY02->oCapPort1SlideUnlock = FALSE;
					g_objAJinAXL.Write_Output(2);
					if (pEquipData->bUseDoorLock) pDY13->oDoor11Unlock = FALSE; 
					g_objAJinAXL.Write_Output(13);

					gData.bCapPort1Wait = FALSE;
					m_bCapSw1On = TRUE;
				}
			}
		} else {
			m_bCapSw1On = FALSE;
		}

		if (pDX12->iCap2Sw) {
			if (pDX02->iCapPort2SlideClose && (!pEquipData->bUseDoorLock || !pDX13->iDoor12Open)) {	// 안전 확인.
				if (!gData.bCapPort2Wait && !m_bCapSw2On) {
					pMainDlg->Set_LampFlicker_Cap2(TRUE);
					pDY02->oCapPort2SlideLock = FALSE; pDY02->oCapPort2SlideUnlock = TRUE;
					g_objAJinAXL.Write_Output(2);
					if (pEquipData->bUseDoorLock) pDY13->oDoor12Unlock = TRUE; 
					g_objAJinAXL.Write_Output(13);

					gData.bCapPort2Wait = TRUE;
					m_bCapSw2On = TRUE;
				}
				else if (gData.bCapPort2Wait && !m_bCapSw2On) {
					pMainDlg->Set_LampFlicker_Cap2(FALSE);
					pDY02->oCapPort2SlideLock = TRUE; pDY02->oCapPort2SlideUnlock = FALSE;
					g_objAJinAXL.Write_Output(2);
					if (pEquipData->bUseDoorLock) pDY13->oDoor12Unlock = FALSE; 
					g_objAJinAXL.Write_Output(13);

					gData.bCapPort2Wait = FALSE;
					m_bCapSw2On = TRUE;
				}
			}
		} else {
			m_bCapSw2On = FALSE;
		}
		// Unload Tray
		if (pDX12->iUnload1Sw) {
			if (pDX03->iUnloadPort1SlideClose && (!pEquipData->bUseDoorLock || !pDX13->iDoor10Open)) {
				if (!gData.bUnloadPort1Wait && !m_bUnloadSw1On) {
					pMainDlg->Set_LampFlicker_Unload1(TRUE);
					pDY03->oUnloadPort1SlideLock = FALSE; pDY03->oUnloadPort1SlideUnlock = TRUE;
					g_objAJinAXL.Write_Output(3);
					if (pEquipData->bUseDoorLock) pDY13->oDoor10Unlock = TRUE; 
					g_objAJinAXL.Write_Output(13);

					gData.bUnloadPort1Wait = TRUE;
					m_bUnloadSw1On = TRUE;
				}
				else if (gData.bUnloadPort1Wait && !m_bUnloadSw1On) {
					pMainDlg->Set_LampFlicker_Unload1(FALSE);
					pDY03->oUnloadPort1SlideLock = TRUE; pDY03->oUnloadPort1SlideUnlock = FALSE;
					g_objAJinAXL.Write_Output(3);
					if (pEquipData->bUseDoorLock) pDY13->oDoor10Unlock = FALSE; 
					g_objAJinAXL.Write_Output(13);

					gData.bUnloadPort1Wait = FALSE;
					m_bUnloadSw1On = TRUE;
				}
			}
		} else {
			m_bUnloadSw1On = FALSE;
		}

		if (pDX12->iUnload2Sw && gData.bUnloadPort2Wait) {
			if (pDX03->iUnloadPort2SlideClose && (!pEquipData->bUseDoorLock || (!pDX13->iDoor05Open && !pDX13->iDoor06Open))) {	// 안전 확인.
				pMainDlg->Set_LampFlicker_Unload2(FALSE);
				pDY03->oUnloadPort2SlideLock = TRUE; pDY03->oUnloadPort2SlideUnlock = FALSE;
				g_objAJinAXL.Write_Output(3);
				if (pEquipData->bUseDoorLock) {
					pDY13->oDoor05Unlock = FALSE; pDY13->oDoor06Unlock = FALSE;
					g_objAJinAXL.Write_Output(13);
				}
				gData.nPNoUnloadPort = 0;
				gData.bUnloadPort2Wait = FALSE;
				gData.bUnloadTrayLotEnd[0] = FALSE;
				gData.bUnloadTrayLotEnd[1] = FALSE;
				//gData.nGoodTrayCount = 0;
			}
		}
	}
	//Cap Port1, 2, Unload Port1 버튼 누르면 도어락 풀리고 작업중 Flag Set
	//Flag Set 상태에서 버튼 누르면 도어락 걸리고 Flag Clear
	//Cap Port1, Unload Port1 트레이 없으면 알람
	//Cap Port2, Full Sensor 감지되면 알람
	//Load Port는 연속랏 테스트할때 정리하자....
}

void CWorkDlg::Display_Status()
{
	CString strTemp, strText;
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

// 	if (g_objMesAgent.Is_Connected()) { m_stcMesConnect.Set_Text("Connected"); m_stcMesConnect.Set_Color(RGB(0x00, 0x00, 0x00), RGB(0x00, 0xFF, 0x00)); }
// 	else { m_stcMesConnect.Set_Text("Disconnected"); m_stcMesConnect.Set_Color(RGB(0xFF, 0xFF, 0xFF), RGB(0x00, 0x00, 0x00)); }
// 
// 	if (g_objMesAgent.Is_HostOnline()) { m_stcMesOnline.Set_Text("Online"); m_stcMesOnline.Set_Color(RGB(0x00, 0x00, 0x00), RGB(0x00, 0xFF, 0x00)); }
// 	else { m_stcMesOnline.Set_Text("Offline"); m_stcMesOnline.Set_Color(RGB(0xFF, 0xFF, 0xFF), RGB(0x00, 0x00, 0x00)); }

	for (int i = 0; i < 3; i++) m_ledIndexDone[i].Set_On(gData.IndexDone[i]);

	DX_DATA_11 *pDX11 = g_objAJinAXL.Get_pDX11();
	int nIndexPos = pDX11->iIndexPosition0 + (pDX11->iIndexPosition1 << 1);
	strText.Format("%d", nIndexPos + 1);
	m_stcIndexPos.SetWindowText(strText);

	BOOL bInitComplete = g_objSequenceInit.Get_InitComplete();
	m_ledInitComplete.Set_On(bInitComplete);
	
	BOOL bLotLoadEnable = g_objSequenceMain.Get_LotLoadEnable(0);
	m_ledLotLoadEnable.Set_On(bLotLoadEnable);

	if (m_nGroupNo == 1) {
		m_grpLot[0].Init_Ctrl("바탕", 12, TRUE, RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
		m_grpLot[1].Init_Ctrl("바탕", 12, TRUE, RGB(0xFF, 0x00, 0x00), COLOR_DEFAULT);

	} else {
		m_grpLot[0].Init_Ctrl("바탕", 12, TRUE, RGB(0xFF, 0x00, 0x00), COLOR_DEFAULT);
		m_grpLot[1].Init_Ctrl("바탕", 12, TRUE, RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
	}

	for (int i = 0; i < PICK; i++) { strText.Format("%d-%d", gData.nTNoIndex[0][i], gData.nCNoIndex[0][i]); m_stcLoadNo[i].Set_Text(strText); }
	for (int i = 0; i < PICK; i++) { strText.Format("%d-%d", gData.nTNoIndex[1][i], gData.nCNoIndex[1][i]); m_stcAssyNo[i].Set_Text(strText); }
	for (int i = 0; i < PICK; i++) { strText.Format("%d-%d", gData.nTNoIndex[2][i], gData.nCNoIndex[2][i]); m_stcTransNo[i].Set_Text(strText); }
	for (int i = 0; i < PICK; i++) { strText.Format("%d-%d", gData.nTNoTransStage[i], gData.nCNoTransStage[i]); m_stcTStageNo[i].Set_Text(strText); }
	for (int i = 0; i < PICK; i++) { strText.Format("%d-%d", gData.nTNoUnloadPick[i], gData.nCNoUnloadPick[i]); m_stcUnloadNo[i].Set_Text(strText); }
	m_ledVisionStatus[0].Set_On(g_objInspector.Get_VisionStatus());
	m_ledVisionStatus[1].Set_On(pEquipData->bUseInlineMode && g_objAviHandler.Is_Connected());
	//m_ledVisionStatus[1].Set_On(pEquipData->bUseInlineMode && g_objAviHandler.Is_Opened());

	int *pCase = g_objSequenceMain.Get_pMainRunCase();
	for (int i = 0; i < AUTO_COUNT; i++) { strText.Format("%02d", *(pCase + i)); m_stcWorkCase[i].Set_Text(strText); }
	
	// Debug
	strText.Format("%d-%d", gData.nPNoAviPort, gData.nTNoAviPort);			m_stcPortNo[ 0].Set_Text(strText);
	strText.Format("%d-%d", gData.nPNoTrayPick, gData.nTNoTrayPick);		m_stcPortNo[ 1].Set_Text(strText);
	strText.Format("%d-%d", gData.nPNoLoadPort, gData.nTNoLoadPort);		m_stcPortNo[ 2].Set_Text(strText);
	strText.Format("%d-%d", gData.nPNoLoadTray[0], gData.nTNoLoadTray[0]);	m_stcPortNo[ 3].Set_Text(strText);
	strText.Format("%d-%d", gData.nPNoLoadTray[1], gData.nTNoLoadTray[1]);	m_stcPortNo[ 4].Set_Text(strText);
	strText.Format("%d-%d", gData.nPNoLoadPick, gData.nTNoLoadPick[0]);		m_stcPortNo[ 5].Set_Text(strText);
	strText.Format("%d-%d", gData.nPNoIndex[0], gData.nTNoIndex[0][0]);		m_stcPortNo[ 6].Set_Text(strText);
	strText.Format("%d-%d", gData.nPNoIndex[1], gData.nTNoIndex[1][0]);		m_stcPortNo[ 7].Set_Text(strText);
	strText.Format("%d-%d", gData.nPNoIndex[2], gData.nTNoIndex[2][0]);		m_stcPortNo[ 8].Set_Text(strText);
	strText.Format("%d-%d", gData.nPNoTransStage, gData.nTNoTransStage[0]);	m_stcPortNo[ 9].Set_Text(strText);
	strText.Format("%d-%d", gData.nPNoUnloadPick, gData.nTNoUnloadPick[0]);	m_stcPortNo[10].Set_Text(strText);
	strText.Format("%d", gData.nPNoUnloadTray);								m_stcPortNo[11].Set_Text(strText);
}

void CWorkDlg::Reset_AlarmLog()
{
	CString strLog, strErrNo;
	SYSTEMTIME time;

	gAlm.bBegin = FALSE;
	GetLocalTime(&time);

	gAlm.dwEndTime = GetTickCount();
	gAlm.sEndTime.Format("%04d%02d%02d_%02d%02d%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	gAlm.dwProcTime = gAlm.dwEndTime - gAlm.dwStartTime;
	
	gLot.dwErrorTime += gAlm.dwProcTime; gLot.nErrorCount++;

	strLog.Format("%s,%04d,%s,%s,%s,%d", gAlm.sLotID, gAlm.nAlmNo, gAlm.sAlmMsg, gAlm.sStartTime, gAlm.sEndTime, gAlm.dwProcTime);
	g_objLogFile.Save_AlarmResetLog(strLog);	// Alarm Reset

	strErrNo.Format("%04d", gAlm.nAlmNo);
//	g_objMesAgent.Set_ErrorUpdate(0, strErrNo);

	g_objLogFile.Save_ECMLog(1, strLog);
}

void CWorkDlg::MachineStopLog(CString sType, CString sMsg)
{
	CString strLog, strErrNo;
	SYSTEMTIME time;

	GetLocalTime(&time);

	if (sType == "STOP_BUTTON_PUSH" || sType == "ALARM" || sType == "PROGRAM_BEGIN") {
		if (gData.dwMStopStartTime > 1) {
			gData.dwMStopEndTime = GetTickCount();
			gData.dwMStopProcTime = gData.dwMStopEndTime - gData.dwMStopStartTime;

			gData.dwMStopStartTime = GetTickCount();
			if (sType == "ALARM")	strLog.Format("%s: %s,%d", sType, sMsg, gData.dwMStopProcTime);
			else					strLog.Format("%s,%d", sType, gData.dwMStopProcTime);

		} else {
			gData.dwMStopStartTime = GetTickCount();
			if (sType == "ALARM")	strLog.Format("%s: %s", sType, sMsg);
			else					strLog.Format("%s", sType);
		}
	} else if (sType == "RUN_START" || sType == "PROGRAM_EXIT") {
		gData.dwMStopEndTime = GetTickCount();
		if (gData.dwMStopStartTime < 1) gData.dwMStopProcTime = 0;
		else							gData.dwMStopProcTime = gData.dwMStopEndTime - gData.dwMStopStartTime;

		strLog.Format("%s,%d", sType, gData.dwMStopProcTime);

		gData.dwMStopStartTime = 0;
	}

	g_objLogFile.Save_MachineStopLog(strLog);
}

void CWorkDlg::Set_AutoRun(BOOL bAutoRun)
{
	if (bAutoRun) m_rdoWorkStart.SetCheck(TRUE);
	else m_rdoWorkStop.SetCheck(TRUE);
}

void CWorkDlg::Set_State(int nState)
{
	switch (nState) {
	case STATE_NONE:
	case STATE_INIT:
	case STATE_STOP:	// Stop
		m_rdoWorkStart.Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
		m_rdoWorkStop.Set_Color(RGB(0xFF, 0x00, 0x00), COLOR_DEFAULT);
		m_rdoWorkStop.SetCheck(TRUE);
		m_chkCycleStop.EnableWindow(TRUE);
		break;
	case STATE_RUN:		// Start
		m_rdoWorkStart.Set_Color(RGB(0xFF, 0x00, 0x00), COLOR_DEFAULT);
		m_rdoWorkStop.Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
		m_chkCycleStop.EnableWindow(FALSE);
		break;
	case STATE_ALARM:
	case STATE_ERROR:	// Error
		m_rdoWorkStart.Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
		m_rdoWorkStop.Set_Color(RGB(0xFF, 0x00, 0x00), COLOR_DEFAULT);
		m_rdoWorkStop.SetCheck(TRUE);
		break;
	}
}

void CWorkDlg::Enable_UserInput(int nNo, BOOL bEnable)
{
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	if (nNo == 1) {
		m_stcLotId[0].EnableWindow(bEnable);
		m_stcTrayCount[0].EnableWindow(bEnable);
		m_stcCmCount[0].EnableWindow(bEnable);
	} 
	if (nNo == 2) {
		m_stcLotId[1].EnableWindow(bEnable);
		m_stcTrayCount[1].EnableWindow(bEnable);
		m_stcCmCount[1].EnableWindow(bEnable);
	}
}

void CWorkDlg::Get_LotInfo(int nPx)
{
	if (nPx < 0 || nPx > 1) return;

	CString strTemp;
	m_stcLotId[nPx].GetWindowText(strTemp);		gData.sLotID[nPx] = strTemp;				// Lot ID
	m_stcCmCount[nPx].GetWindowText(strTemp);	gData.nCmUseCount[nPx] = atoi(strTemp);		// CM 수량
	m_stcTrayCount[nPx].GetWindowText(strTemp);	gData.nTrayUseCount[nPx] = atoi(strTemp);	// Tray Count
}

void CWorkDlg::Set_LotInfo(int nPx)
{
	if (nPx < 0 || nPx > 1) return;

	int nCmCount = gData.nCmUseCount[nPx];
	int nTrayCount = nCmCount / gData.nCmMaxCount;
	if (nCmCount % gData.nCmMaxCount) nTrayCount++;

	CString strTemp;
	m_stcLotId[nPx].SetWindowText(gData.sLotID[nPx]);
	strTemp.Format("%d", nCmCount); m_stcCmCount[nPx].SetWindowText(strTemp);
	strTemp.Format("%d", nTrayCount); m_stcTrayCount[nPx].SetWindowText(strTemp);
}

///////////////////////////////////////////////////////////////////////////////

void CWorkDlg::Get_CapLotId()
{
	CString strTemp;
	m_stcCapLotIdS.GetWindowText(strTemp);
	gData.sCapLotID = strTemp;
}

void CWorkDlg::Get_ShipLotId()
{
	CString strTemp;
	m_stcShipLotIdS.GetWindowText(strTemp);
	gData.sShipLotID = strTemp;
}

void CWorkDlg::Change_CapLotId()
{
	CString strTemp;

	m_stcCapLotIdS.GetWindowText(strTemp);
	m_stcCapLotIdR.SetWindowText(strTemp);
	m_stcCapLotIdS.SetWindowText("");
	gData.sCapLotID = strTemp;

	m_stcCapCmCountS.GetWindowText(strTemp);
	m_stcCapCmCountR.SetWindowText(strTemp);
	m_stcCapCmCountS.SetWindowText("0");

	m_stcCapTrayCountS.GetWindowText(strTemp);
	m_stcCapTrayCountR.SetWindowText(strTemp);
	m_stcCapTrayCountS.SetWindowText("0");

	gData.nCapTrayLoad = 0;
	gData.nCapTrayMax = atoi(strTemp);
}

void CWorkDlg::Change_ShipLotId()
{
	CString strTemp;

	m_stcShipLotIdS.GetWindowText(strTemp);
	m_stcShipLotIdR.SetWindowText(strTemp);
	m_stcShipLotIdS.SetWindowText("");
	gData.sShipLotID = strTemp;

	m_stcShipCmCountS.GetWindowText(strTemp);
	m_stcShipCmCountR.SetWindowText(strTemp);
	m_stcShipCmCountS.SetWindowText("0");

	m_stcShipTrayCountS.GetWindowText(strTemp);
	m_stcShipTrayCountR.SetWindowText(strTemp);
	m_stcShipTrayCountS.SetWindowText("0");

	gData.nShipTrayLoad = 0;
	gData.nShipTrayMax = atoi(strTemp);
}

BOOL CWorkDlg::Check_CapLotId()
{
	// 자동입력 취소 (2024.01.10)
// 	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();
// 	CString strCnt;
// 
// 	strCnt.Format("%d", 100 * gData.nCapMaxCount);
// 	if (!pEquipData->bUseMES || !pEquipData->bUseMesCapReg) m_stcCapLotIdS.SetWindowText("CAP-LOT-ID");
// 	if (!pEquipData->bUseMES || !pEquipData->bUseMesCapReg) m_stcCapTrayCountS.SetWindowText("100");
// 	if (!pEquipData->bUseMES || !pEquipData->bUseMesCapReg) m_stcCapCmCountS.SetWindowText(strCnt);

	CString strRead;

	m_stcCapLotIdS.GetWindowText(strRead);		// Lot ID
	if (strRead.GetLength() < 2) return FALSE;

	m_stcCapTrayCountS.GetWindowText(strRead);	// Tray 수량
	if (strRead.GetLength() < 1 || atoi(strRead) < 1) return FALSE;

	return TRUE;
}

BOOL CWorkDlg::Check_ShipLotId()
{
	// 자동입력 취소 (2024.01.10)
// 	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();
// 	CString strCnt;
// 
// 	strCnt.Format("%d", 100 * gData.nShipMaxCount);
// 	if (!pEquipData->bUseMES || !pEquipData->bUseMesShipReg) m_stcShipLotIdS.SetWindowText("SHIP-LOT-ID");
// 	if (!pEquipData->bUseMES || !pEquipData->bUseMesShipReg) m_stcShipTrayCountS.SetWindowText("100");
// 	if (!pEquipData->bUseMES || !pEquipData->bUseMesShipReg) m_stcShipCmCountS.SetWindowText(strCnt);

	CString strRead;

	m_stcShipLotIdS.GetWindowText(strRead);
	if (strRead.GetLength() < 2) return FALSE;

	m_stcShipTrayCountS.GetWindowText(strRead);
	if (strRead.GetLength() < 1 || atoi(strRead) < 1) return FALSE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// Message

LRESULT CWorkDlg::OnUpdateModel(WPARAM wParam, LPARAM lParam)
{
	CString strText;

	strText.Format("%d", gData.nTrayUseCount[0]);
	m_stcTrayCount[0].SetWindowText(strText);

	strText.Format("%d", gData.nCmUseCount[0]);
	m_stcCmCount[0].SetWindowText(strText);

	strText.Format("%d", gData.nTrayUseCount[1]);
	m_stcTrayCount[1].SetWindowText(strText);

	strText.Format("%d", gData.nCmUseCount[1]);
	m_stcCmCount[1].SetWindowText(strText);

	return 0;
}

LRESULT CWorkDlg::OnUpdateTrayInfo(WPARAM nTray, LPARAM lParam)
{
	CString strText;
	int nNo = (int)lParam;

	if (nTray == 0 || nTray == 1) {		// Load Tray
		strText.Format("%d", gData.nTNoLoadTray[nNo]);
		m_stcLoadTrayCount.SetWindowText(strText);

		for (int i = 0; i < LT_Y; i++) {
			for (int j = 0; j < LT_X; j++) {
				if		(gData.InfoLoadTray[i][j] == 9) m_grdLoadTray.Set_CellBackClr(i, j, RGB(0xFF, 0xFF, 0x00));	// Reserve
				else if (gData.InfoLoadTray[i][j] == 0) m_grdLoadTray.Set_CellBackClr(i, j, RGB(0xFF, 0xFF, 0xFF));	// Empty
				else									m_grdLoadTray.Set_CellBackClr(i, j, RGB(0x80, 0x80, 0x80));	// Error
			}
		}
		g_dlgOperator.Update_TrayInfo(nTray);
	}
	if (nTray == 0 || nTray == 2) {		// Cap Tray
		strText.Format("%d", gData.nCapTrayCount);
		m_stcCapTrayCount.SetWindowText(strText);

		for (int i = 0; i < CT_Y; i++) {
			for (int j = 0; j < CT_X; j++) {
				if		(gData.InfoCapTray[i][j] == 9) m_grdCapTray.Set_CellBackClr(i, j, RGB(0x00, 0xFF, 0xFF));	// Reserve
				else if (gData.InfoCapTray[i][j] == 0) m_grdCapTray.Set_CellBackClr(i, j, RGB(0xFF, 0xFF, 0xFF));	// Empty
				else								   m_grdCapTray.Set_CellBackClr(i, j, RGB(0x80, 0x80, 0x80));	// Error
			}
		}
		g_dlgOperator.Update_TrayInfo(nTray, nNo);
	}

	

	if (nTray == 0 || nTray == 3) {		// NG Tray
		strText.Format("%d", gData.nTNoUnloadTray);
		m_stcShipTrayCount.SetWindowText(strText);

		for (int i = 0; i < gData.STY; i++) {
			for (int j = 0; j < ST_X; j++) {
				if		(gData.InfoShipTray[i][j] >  1) m_grdShipTray.Set_CellBackClr(i, j, RGB(0x70, 0x70, 0x70));	// Not Use
				else if	(gData.InfoShipTray[i][j] == 1) m_grdShipTray.Set_CellBackClr(i, j, RGB(0x00, 0xFF, 0x00));	// Good
				else if (gData.InfoShipTray[i][j] == 0) m_grdShipTray.Set_CellBackClr(i, j, RGB(0xFF, 0xFF, 0xFF));	// Empty
				else									m_grdShipTray.Set_CellBackClr(i, j, RGB(0x80, 0x80, 0x80));	// Error
			}
		}
		g_dlgOperator.Update_TrayInfo(nTray, nNo);
	}


	if (nTray == 0 || nTray == 4)  // Unload Tray
	{		
		strText.Format("%d", gData.nTNoUnloadTray);
		m_stcNGTrayCount.SetWindowText(strText);

		for (int i = 0; i < gData.STY; i++) 
		{
			for (int j = 0; j < ST_X; j++)
			{
				if		(gData.InfoNgTray[i][j] >  1) m_grdNGTray.Set_CellBackClr(i, j, RGB(0x70, 0x70, 0x70));	// Not Use
				else if	(gData.InfoNgTray[i][j] == 1) m_grdNGTray.Set_CellBackClr(i, j, RGB(0x00, 0xFF, 0x00));	// Good
				else if (gData.InfoNgTray[i][j] == 0) m_grdNGTray.Set_CellBackClr(i, j, RGB(0xFF, 0xFF, 0xFF));	// Empty
				else									m_grdNGTray.Set_CellBackClr(i, j, RGB(0x80, 0x80, 0x80));	// Error
			}
		}
		g_dlgOperator.Update_TrayInfo(nTray, nNo);
	}

	return 0;
}

LRESULT CWorkDlg::OnUpdateBarcode(WPARAM wParam, LPARAM lParam)
{
	CString sBarcode = g_objBarcodeLot.Get_BarcodeLot();
	sBarcode.Replace("\r","");
	sBarcode.Replace("\n","");
	CString strTemp, strText;

	if (sBarcode.GetLength() < 2) return 0;

	int nType = 0;
	if		(m_bCapLotIdInput)  nType = 1;
	else if (m_bShipLotIdInput) nType = 2;
	else						return 0;
	int nCapCnt = Check_CapShipLotID(nType, sBarcode);
	if (nCapCnt < 1) return 0;

	if (m_bCapLotIdInput) {
		m_stcCapLotIdS.GetWindowText(strTemp);
		if (strTemp == sBarcode) return 0;

		char chSep = '/';
		CString strPart, strCount, strLotId;
		AfxExtractSubString(strPart, sBarcode, 0, chSep);  strPart.Trim();
		AfxExtractSubString(strCount, sBarcode, 1, chSep); strCount.Trim();
		AfxExtractSubString(strLotId, sBarcode, 2, chSep); strLotId.Trim();

		gData.sCapLotID = sBarcode;
		m_stcCapLotIdS.SetWindowText(sBarcode);

		int nCmCnt = atoi(strCount);
		strTemp.Format("%d", nCmCnt); m_stcCapCmCountS.SetWindowText(strTemp);

		gData.nCapTrayMax = nCmCnt / gData.nCapMaxCount;
		if (nCmCnt % gData.nCapMaxCount) gData.nCapTrayMax++;
		strTemp.Format("%d", gData.nCapTrayMax); m_stcCapTrayCountS.SetWindowText(strTemp);

	} else if (m_bShipLotIdInput) {
		m_stcShipLotIdS.GetWindowText(strTemp);
		if (strTemp == sBarcode) return 0;

		char chSep = '/';
		CString strPart, strCount, strLotId;
		AfxExtractSubString(strPart, sBarcode, 0, chSep);  strPart.Trim();
		AfxExtractSubString(strCount, sBarcode, 1, chSep); strCount.Trim();
		AfxExtractSubString(strLotId, sBarcode, 2, chSep); strLotId.Trim();

		gData.sShipLotID = sBarcode;
		m_stcShipLotIdS.SetWindowText(sBarcode);

		gData.nShipTrayMax = atoi(strCount);
		strTemp.Format("%d", gData.nShipTrayMax); m_stcShipTrayCountS.SetWindowText(strTemp);

		int nCmCnt = gData.nShipTrayMax * gData.nShipMaxCount;
		strTemp.Format("%d", nCmCnt); m_stcShipCmCountS.SetWindowText(strTemp);
	}

	return 0;
}

LRESULT CWorkDlg::OnUpdateLoadCell(WPARAM wParam, LPARAM lParam)
{
	CString strTemp;
	int nNo = (int)wParam;
	double dData;
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	if (nNo == pEquipData->nAssyLoadCellPort) dData = g_objLoadCell.Get_AssyLoadCell();
	if (nNo == pEquipData->nUnloadLoadCellPort) dData = g_objLoadCell.Get_UnloadLoadCell();

	if (dData < 0) return 0;

	if (nNo == pEquipData->nAssyLoadCellPort) {
		gData.dLoadCell[0] = dData;
	} else {
		gData.dLoadCell[1] = dData;
	}
	gData.bLoadCellComplete = TRUE;
	return 0;
}

LRESULT CWorkDlg::OnResetCycleStop(WPARAM wParam, LPARAM lParam)
{
	gData.bCycleStop = FALSE;
	m_chkCycleStop.SetCheck(FALSE);
	m_chkCycleStop.Set_Color(RGB(0x00, 0x00, 0x00), RGB(0xF0, 0xF0, 0xF0));
	//if (wParam == 1) g_objSequenceInit.Set_InitComplete(FALSE);	// 초기화 필요

	return 0;
}

LRESULT CWorkDlg::OnUpdateUph(WPARAM wParam, LPARAM lParam)
{
	SYSTEMTIME time;
	GetLocalTime(&time);

	int nHour = time.wHour;
	double	dDispTakt[4] = { 0.0 };
	CString strText;

	dDispTakt[0] = gUph.dTaktTime;		// 현재 Lot

	int nCount = gUph.nLotCount[nHour];
	for (int i = 0; i < nCount; i++) dDispTakt[1] += gUph.dTakt[nHour][i];
	dDispTakt[1] /= nCount;				// 1시간

	int nTotalCount = 0;
	if (nHour >= 7 && nHour < 19) {
		for (int i = 7; i < 19; i++) {
			nCount = gUph.nLotCount[i];
			for (int j = 0; j < nCount; j++) dDispTakt[2] += gUph.dTakt[i][j];
			nTotalCount += nCount;
		}
	} else {
		for (int i = 0; i < 7; i++) {
			nCount = gUph.nLotCount[i];
			for (int j = 0; j < nCount; j++) dDispTakt[2] += gUph.dTakt[i][j];
			nTotalCount += nCount;
		}

		for (int i = 19; i < 24; i++) {
			nCount = gUph.nLotCount[i];
			for (int j = 0; j < nCount; j++) dDispTakt[2] += gUph.dTakt[i][j];
			nTotalCount += nCount;
		}
	}
	dDispTakt[2] /= nTotalCount;		// 12시간

	nTotalCount = 0;
	for (int i = 0; i < 24; i++) {
		nCount = gUph.nLotCount[i];
		for (int j = 0; j < nCount; j++) dDispTakt[3] += gUph.dTakt[i][j];
		nTotalCount += nCount;
	}
	dDispTakt[3] /= nTotalCount;		// 1일

	// Takt & UPH
	for (int i = 0; i < 4; i++) {
		if (dDispTakt[i] == 0) strText = "";
		else strText.Format("%0.5f", dDispTakt[i]);
		m_stcTakt[i].SetWindowText(strText);

		if (dDispTakt[i] == 0) strText = "";
		else strText.Format("%d", int(3600 / dDispTakt[i]));
		m_stcUph[i].SetWindowText(strText);
	}

	//생산량
	for (int i = 0; i < 2; i++) {
		if (gUph.nCmCount[i] == 0) strText = "";
		else strText.Format("%d", gUph.nCmCount[i]);
		m_stcDay[i].SetWindowText(strText);
	}

	return 0;
}

LRESULT CWorkDlg::OnVisionResult(WPARAM wParam, LPARAM lParam)
{
	CString strText;
	int nTotal = gLot.nCmCount[0];

	if (wParam == 1 || wParam == 0) {
		strText.Format("%d", nTotal);	m_stcVisionResult[0].SetWindowText(strText);

		m_stcVisionResult[2].Init_Ctrl("바탕", 11, TRUE, RGB(0x00, 0xFF, 0x00), RGB(0x00, 0x00, 0x00));
		m_stcVisionResult[3].Init_Ctrl("바탕", 11, TRUE, RGB(0xFF, 0xFF, 0xFF), RGB(0x00, 0x00, 0x00));
		m_stcVisionResult[2].SetWindowText(gLot.sLotID[0]);
		m_stcVisionResult[3].SetWindowText(gLot.sLotID[1]);	
	}
	if (wParam == 2 || wParam == 0) {
		nTotal = gLot.nCmCount[1];
		strText.Format("%d", nTotal);	m_stcVisionResult[1].SetWindowText(strText);

		m_stcVisionResult[2].Init_Ctrl("바탕", 11, TRUE, RGB(0xFF, 0xFF, 0xFF), RGB(0x00, 0x00, 0x00));
		m_stcVisionResult[3].Init_Ctrl("바탕", 11, TRUE, RGB(0x00, 0xFF, 0x00), RGB(0x00, 0x00, 0x00));
		m_stcVisionResult[2].SetWindowText(gLot.sLotID[0]);
		m_stcVisionResult[3].SetWindowText(gLot.sLotID[1]);
	}
	return 0;
}

LRESULT CWorkDlg::OnLotStartEnd(WPARAM wParam, LPARAM lParam)
{
	CCME8000Dlg *pMainDlg = (CCME8000Dlg*)AfxGetApp()->GetMainWnd();
	int nNo = lParam;
	if (wParam == 1) {
		pMainDlg->Set_LotErrorLog("LOT START", 901, "Lot Start", nNo);
	} else if (wParam == 2) {
		pMainDlg->Set_LotErrorLog("LOT END", 902, "Lot End", nNo);
	}
	return 0;
}

LRESULT CWorkDlg::OnIndexTack(WPARAM wParam, LPARAM lParam)
{
	CString strTemp;
	strTemp.Format("Index Tack : %0.3lf", (double)(wParam / 1000.0));
	m_stcCycleTime.SetWindowText(strTemp);
	return 0;
}

LRESULT CWorkDlg::OnShowMsg(WPARAM wParam, LPARAM lParam)
{
	//g_dlgWork.PostMessage(UM_LOT_END_MSG, NULL, NULL);
	if(wParam == 1) g_objCommon.Show_MsgBox(1, "NG Tray Full 상태입니다.\nNG Tray 교체 해주십시오.");
	else if(wParam == 2) g_objCommon.Show_MsgBox(1, "현재 Lot이 종료 되었습니다. Tray 배출 해주십시오.");
	else if(wParam == 3) g_objCommon.Show_Alarm("Cap Tray 정보 입력 해주십시오.", STATE_CAPTRAY);
	else if(wParam == 4) g_objCommon.Show_Alarm("Cap Tray 투입 해주십시오.", STATE_CAPTRAY);
	else if(wParam == 5) g_objCommon.Show_Alarm("Ship Tray 투입 해주십시오.", STATE_SHIPTRAY);
	else 	g_objCommon.Show_MsgBox(1, "Lot의 마지막 트레이 작업 중입니다.\n배출 준비 해주십시오.");

	return 0;
}

void CWorkDlg::Change_Model()
{
	Initial_Grid(&m_grdShipTray, gData.STY, ST_X);
	g_dlgOperator.Change_Model();
}

///////////////////////////////////////////////////////////////////////////////

void CWorkDlg::OnBnClickedButton1()
{
}

void CWorkDlg::OnBnClickedButton2()
{
}


void CWorkDlg::OnBnClickedBtnLotstart1()
{
	DX_DATA_02 *pDX02 = g_objAJinAXL.Get_pDX02();

	pDX02->iCapPort2HighCheck = TRUE;
	//g_objInspector.Set_LotStart(gData.sLotID[0], 1);
	
}


void CWorkDlg::OnBnClickedBtnLotstart2()
{
	g_objInspector.Set_LotStart(gData.sLotID[1], 2);
}


void CWorkDlg::OnBnClickedBtnCapsw1()
{
	gData.bCapPort1Wait = TRUE;
	m_bCapSw1On = TRUE;
}


void CWorkDlg::OnBnClickedBtnCapsw2()
{
	gData.bCapPort2Wait = TRUE;
	m_bCapSw2On = TRUE;
}


void CWorkDlg::OnBnClickedBtnCapsw3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CWorkDlg::OnBnClickedBtnCapf1()
{
	m_bCapSw1On = FALSE;
}


void CWorkDlg::OnBnClickedBtnCapf2()
{
	m_bCapSw2On = FALSE;
}


void CWorkDlg::OnBnClickedBtnUnload1()
{
	gData.bUnloadPort1Wait = TRUE;
	m_bUnloadSw1On = TRUE;
}


void CWorkDlg::OnBnClickedBtnUnload2()
{
	gData.nPNoUnloadPort = 0;
	gData.bUnloadPort2Wait = FALSE;
	gData.bUnloadTrayLotEnd[0] = FALSE;
	gData.bUnloadTrayLotEnd[1] = FALSE;
}


void CWorkDlg::OnBnClickedBtnUnloadf1()
{
	m_bUnloadSw1On = FALSE;
}
