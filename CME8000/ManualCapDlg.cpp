// ManualCapDlg.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "CME8000.h"
#include "ManualCapDlg.h"
#include "afxdialogex.h"
#include "math.h"

#include "LogFile.h"
#include "AJinAXL.h"
#include "DataManager.h"
#include "Common.h"
#include "SequenceMain.h"

// CManualCapDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CManualCapDlg, CDialogEx)

CManualCapDlg::CManualCapDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CManualCapDlg::IDD, pParent)
{
}

CManualCapDlg::~CManualCapDlg()
{
}

void CManualCapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	for (int i = 0; i < 14; i++) DDX_Control(pDX, IDC_GROUP_0 + i, m_Group[i]);
	for (int i = 0; i <  8; i++) DDX_Control(pDX, IDC_LABEL_0 + i, m_Label[i]);
	for (int i = 0; i <  8; i++) DDX_Control(pDX, IDC_STC_AXIS_POS_0 + i, m_stcAxisPos[i]);
	for (int i = 0; i <  4; i++) DDX_Control(pDX, IDC_BTN_CAP_PORT1_IO_0	+ i, m_btnCapPort1Io[i]);
	for (int i = 0; i < 10; i++) DDX_Control(pDX, IDC_LED_CAP_PORT1_IO_0	+ i, m_ledCapPort1Io[i]);
	for (int i = 0; i <  4; i++) DDX_Control(pDX, IDC_BTN_CAP_PORT2_IO_0	+ i, m_btnCapPort2Io[i]);
	for (int i = 0; i < 10; i++) DDX_Control(pDX, IDC_LED_CAP_PORT2_IO_0	+ i, m_ledCapPort2Io[i]);
	for (int i = 0; i <  5; i++) DDX_Control(pDX, IDC_BTN_CAP_STAGE1_X_0	+ i, m_btnCapStage1X[i]);
	for (int i = 0; i <  4; i++) DDX_Control(pDX, IDC_BTN_CAP_STAGE1_Z_0	+ i, m_btnCapStage1Z[i]);
	for (int i = 0; i <  4; i++) DDX_Control(pDX, IDC_BTN_CAP_STAGE1_IO_0	+ i, m_btnCapStage1Io[i]);
	for (int i = 0; i <  5; i++) DDX_Control(pDX, IDC_LED_CAP_STAGE1_IO_0	+ i, m_ledCapStage1Io[i]);
	for (int i = 0; i <  5; i++) DDX_Control(pDX, IDC_BTN_CAP_STAGE2_X_0	+ i, m_btnCapStage2X[i]);
	for (int i = 0; i <  4; i++) DDX_Control(pDX, IDC_BTN_CAP_STAGE2_Z_0	+ i, m_btnCapStage2Z[i]);
	for (int i = 0; i <  4; i++) DDX_Control(pDX, IDC_BTN_CAP_STAGE2_IO_0	+ i, m_btnCapStage2Io[i]);
	for (int i = 0; i <  5; i++) DDX_Control(pDX, IDC_LED_CAP_STAGE2_IO_0	+ i, m_ledCapStage2Io[i]);
	for (int i = 0; i < 15; i++) DDX_Control(pDX, IDC_BTN_CAP_PICKER_Y_0	+ i, m_btnCapPickerY[i]);
	for (int i = 0; i <  4; i++) DDX_Control(pDX, IDC_BTN_CAP_PICKER_Z_0	+ i, m_btnCapPickerZ[i]);
	for (int i = 0; i <  2; i++) DDX_Control(pDX, IDC_BTN_CAP_PICKER_P_0	+ i, m_btnCapPickerP[i]);
	for (int i = 0; i < 42; i++) DDX_Control(pDX, IDC_BTN_CAP_PICKER_IO_0	+ i, m_btnCapPickerIo[i]);
	for (int i = 0; i < 24; i++) DDX_Control(pDX, IDC_LED_CAP_PICKER_IO_0	+ i, m_ledCapPickerIo[i]);
	for (int i = 0; i <  6; i++) DDX_Control(pDX, IDC_BTN_CAP_BUFFER_Y_0	+ i, m_btnCapBufferY[i]);
	for (int i = 0; i < 18; i++) DDX_Control(pDX, IDC_BTN_CAP_BUFFER_IO_0	+ i, m_btnCapBufferIo[i]);
	for (int i = 0; i <  4; i++) DDX_Control(pDX, IDC_LED_CAP_BUFFER_IO_0	+ i, m_ledCapBufferIo[i]);
}

BEGIN_MESSAGE_MAP(CManualCapDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BTN_CAP_PORT1_IO_0		, IDC_BTN_CAP_PORT1_IO_3	, OnBtnCapPort1IoClick)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BTN_CAP_PORT2_IO_0		, IDC_BTN_CAP_PORT2_IO_3	, OnBtnCapPort2IoClick)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BTN_CAP_STAGE1_X_0		, IDC_BTN_CAP_STAGE1_X_4	, OnBtnCapStage1XClick)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BTN_CAP_STAGE1_Z_0		, IDC_BTN_CAP_STAGE1_Z_3	, OnBtnCapStage1ZClick)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BTN_CAP_STAGE1_IO_0	, IDC_BTN_CAP_STAGE1_IO_3	, OnBtnCapStage1IoClick)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BTN_CAP_STAGE2_X_0		, IDC_BTN_CAP_STAGE2_X_4	, OnBtnCapStage2XClick)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BTN_CAP_STAGE2_Z_0		, IDC_BTN_CAP_STAGE2_Z_3	, OnBtnCapStage2ZClick)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BTN_CAP_STAGE2_IO_0	, IDC_BTN_CAP_STAGE2_IO_3	, OnBtnCapStage2IoClick)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BTN_CAP_PICKER_Y_0		, IDC_BTN_CAP_PICKER_Y_14	, OnBtnCapPickerYClick)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BTN_CAP_PICKER_Z_0		, IDC_BTN_CAP_PICKER_Z_3	, OnBtnCapPickerZClick)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BTN_CAP_PICKER_P_0		, IDC_BTN_CAP_PICKER_P_1	, OnBtnCapPickerPClick)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BTN_CAP_PICKER_IO_0	, IDC_BTN_CAP_PICKER_IO_41	, OnBtnCapPickerIoClick)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BTN_CAP_BUFFER_Y_0		, IDC_BTN_CAP_BUFFER_Y_5	, OnBtnCapBufferYClick)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BTN_CAP_BUFFER_IO_0	, IDC_BTN_CAP_BUFFER_IO_17	, OnBtnCapBufferIoClick)
	END_MESSAGE_MAP()

// CManualCapDlg 메시지 처리기입니다.

void CManualCapDlg::Initial_Controls() 
{
	for (int i =  0; i <  2; i++) m_Group[i].Init_Ctrl("Arial", 11, TRUE, RGB(0x00, 0x00, 0xFF), COLOR_DEFAULT);
	for (int i =  2; i <  5; i++) m_Group[i].Init_Ctrl("Arial", 11, TRUE, RGB(0xD0, 0x10, 0x10), COLOR_DEFAULT);
	for (int i =  5; i <  8; i++) m_Group[i].Init_Ctrl("Arial", 11, TRUE, RGB(0xD0, 0x10, 0xD0), COLOR_DEFAULT);
	for (int i =  8; i < 12; i++) m_Group[i].Init_Ctrl("Arial", 11, TRUE, RGB(0xFF, 0x90, 0x00), COLOR_DEFAULT);
	for (int i = 12; i < 14; i++) m_Group[i].Init_Ctrl("Arial", 11, TRUE, RGB(0x30, 0x80, 0x00), COLOR_DEFAULT);
	for (int i =  0; i <  8; i++) m_Label[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, RGB(0xB0, 0xB0, 0xB0));
	for (int i =  0; i <  8; i++) m_stcAxisPos[i].Init_Ctrl("Arial", 10, TRUE, RGB(0xFF, 0xFF, 0xFF), RGB(0x10, 0x10, 0x60));
	for (int i =  0; i <  4; i++) m_btnCapPort1Io[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, 0, 0);
	for (int i =  0; i < 10; i++) m_ledCapPort1Io[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, CLedCS::emGreen, CLedCS::em16);
	for (int i =  0; i <  4; i++) m_btnCapPort2Io[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, 0, 0);
	for (int i =  0; i < 10; i++) m_ledCapPort2Io[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, CLedCS::emGreen, CLedCS::em16);
	for (int i =  0; i <  5; i++) m_btnCapStage1X[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, 0, 0);
	for (int i =  0; i <  4; i++) m_btnCapStage1Z[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, 0, 0);
	for (int i =  0; i <  4; i++) m_btnCapStage1Io[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, 0, 0);
	for (int i =  0; i <  5; i++) m_ledCapStage1Io[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, CLedCS::emGreen, CLedCS::em16);
	for (int i =  0; i <  5; i++) m_btnCapStage2X[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, 0, 0);
	for (int i =  0; i <  4; i++) m_btnCapStage2Z[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, 0, 0);
	for (int i =  0; i <  4; i++) m_btnCapStage2Io[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, 0, 0);
	for (int i =  0; i <  5; i++) m_ledCapStage2Io[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, CLedCS::emGreen, CLedCS::em16);
	for (int i =  0; i < 15; i++) m_btnCapPickerY[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, 0, 0);
	for (int i =  0; i <  4; i++) m_btnCapPickerZ[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, 0, 0);
	for (int i =  0; i <  2; i++) m_btnCapPickerP[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, 0, 0);
	for (int i =  0; i < 42; i++) m_btnCapPickerIo[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, 0, 0);
	for (int i =  0; i < 24; i++) m_ledCapPickerIo[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, CLedCS::emGreen, CLedCS::em16);
	for (int i =  0; i <  6; i++) m_btnCapBufferY[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, 0, 0);
	for (int i =  0; i < 18; i++) m_btnCapBufferIo[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, 0, 0);
	for (int i =  0; i <  4; i++) m_ledCapBufferIo[i].Init_Ctrl("Arial", 10, FALSE, COLOR_DEFAULT, COLOR_DEFAULT, CLedCS::emGreen, CLedCS::em16);
}

BOOL CManualCapDlg::OnInitDialog() 
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetWindowPos(this, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	Initial_Controls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CManualCapDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CManualCapDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (!bShow) return;

	Display_Status();

	m_btnCapPickerIo[ 5].EnableWindow(PICK > 5);	// Picker 6 Up
	m_btnCapPickerIo[11].EnableWindow(PICK > 5);	// Picker 6 Down
	m_btnCapPickerIo[17].EnableWindow(PICK > 5);	// Picker 6 Vac On
	m_btnCapPickerIo[23].EnableWindow(PICK > 5);	// Picker 6 Vac Off
	m_btnCapPickerIo[29].EnableWindow(PICK > 5);	// Picker 6 Normal
	m_btnCapPickerIo[35].EnableWindow(PICK > 5);	// Picker 6 Turn
	m_btnCapBufferIo[ 9].EnableWindow(PICK > 5);	// Buffer 6 Air On
	m_btnCapBufferIo[15].EnableWindow(PICK > 5);	// Buffer 6 Air Off

	m_btnCapPickerIo[ 4].EnableWindow(PICK > 4);	// Picker 5 Up
	m_btnCapPickerIo[10].EnableWindow(PICK > 4);	// Picker 5 Down
	m_btnCapPickerIo[16].EnableWindow(PICK > 4);	// Picker 5 Vac On
	m_btnCapPickerIo[22].EnableWindow(PICK > 4);	// Picker 5 Vac Off
	m_btnCapPickerIo[28].EnableWindow(PICK > 4);	// Picker 5 Normal
	m_btnCapPickerIo[34].EnableWindow(PICK > 4);	// Picker 5 Turn
	m_btnCapBufferIo[ 8].EnableWindow(PICK > 4);	// Buffer 5 Air On
	m_btnCapBufferIo[14].EnableWindow(PICK > 4);	// Buffer 5 Air Off

	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();
	for (int i = 1; i < 6; i++) {
		BOOL bEnable = (!pEquipData->bCapPickUpMulti && i < PICK);
		m_btnCapPickerY[i+0].EnableWindow(bEnable);	// Stage1 Picker 2~6 Position
		m_btnCapPickerY[i+6].EnableWindow(bEnable);	// Stage2 Picker 2~6 Position
	}
	
	m_strLog.Format("[Manual Cap] Show Window");
	g_objLogFile.Save_HandlerLog(m_strLog);
}

void CManualCapDlg::Display_Status()
{
	CString strPos;
	for (int i = 0; i < 8; i++) {
		double dPos = g_objAJinAXL.Get_Position(AX_CAP_STAGE1_X + i);
		strPos.Format("%0.3lf", dPos);
		m_stcAxisPos[i].SetWindowText(strPos);
	}

	DX_DATA_02 *pDX02 = g_objAJinAXL.Get_pDX02();
	DX_DATA_04 *pDX04 = g_objAJinAXL.Get_pDX04();
	DX_DATA_07 *pDX07 = g_objAJinAXL.Get_pDX07();
	DX_DATA_09 *pDX09 = g_objAJinAXL.Get_pDX09();

	m_ledCapPort1Io[0].Set_On(pDX02->iCapPort1LowCheck);
	m_ledCapPort1Io[1].Set_On(pDX02->iCapPort1HighCheck);
	m_ledCapPort1Io[2].Set_On(pDX02->iCapPort1SlideOpen);
	m_ledCapPort1Io[3].Set_On(pDX02->iCapPort1SlideClose);
	m_ledCapPort1Io[4].Set_On(pDX02->iCapPort1SlideLock);
	m_ledCapPort1Io[5].Set_On(pDX02->iCapPort1SlideUnlock);
	m_ledCapPort1Io[6].Set_On(pDX02->iCapPort1Support1In);
	m_ledCapPort1Io[7].Set_On(pDX02->iCapPort1Support2In);
	m_ledCapPort1Io[8].Set_On(pDX02->iCapPort1Support1Out);
	m_ledCapPort1Io[9].Set_On(pDX02->iCapPort1Support2Out);

	m_ledCapPort2Io[0].Set_On(pDX02->iCapPort2LowCheck);
	m_ledCapPort2Io[1].Set_On(pDX02->iCapPort2SlideOpen);
	m_ledCapPort2Io[2].Set_On(pDX02->iCapPort2SlideClose);
	m_ledCapPort2Io[3].Set_On(!pDX02->iCapPortAreaCheck);
	m_ledCapPort2Io[4].Set_On(pDX02->iCapPort2SlideLock);
	m_ledCapPort2Io[5].Set_On(pDX02->iCapPort2SlideUnlock);
	m_ledCapPort2Io[6].Set_On(pDX02->iCapPort2Support1In);
	m_ledCapPort2Io[7].Set_On(pDX02->iCapPort2Support2In);
	m_ledCapPort2Io[8].Set_On(pDX02->iCapPort2Support1Out);
	m_ledCapPort2Io[9].Set_On(pDX02->iCapPort2Support2Out);

	m_ledCapStage1Io[0].Set_On(pDX04->iCapStage1MasterIn);
	m_ledCapStage1Io[1].Set_On(pDX04->iCapStage1MasterOut);
	m_ledCapStage1Io[2].Set_On(pDX04->iCapStage1SlaveIn);
	m_ledCapStage1Io[3].Set_On(pDX04->iCapStage1SlaveOut);
	m_ledCapStage1Io[4].Set_On(pDX04->iCapStage1Exist);

	m_ledCapStage2Io[0].Set_On(pDX04->iCapStage2MasterIn);
	m_ledCapStage2Io[1].Set_On(pDX04->iCapStage2MasterOut);
	m_ledCapStage2Io[2].Set_On(pDX04->iCapStage2SlaveIn);
	m_ledCapStage2Io[3].Set_On(pDX04->iCapStage2SlaveOut);
	m_ledCapStage2Io[4].Set_On(pDX04->iCapStage2Exist);

	m_ledCapPickerIo[ 0].Set_On(pDX07->iCapPickerUp01);
	m_ledCapPickerIo[ 1].Set_On(pDX07->iCapPickerUp02);
	m_ledCapPickerIo[ 2].Set_On(pDX07->iCapPickerUp03);
	m_ledCapPickerIo[ 3].Set_On(pDX07->iCapPickerUp04);

	m_ledCapPickerIo[ 6].Set_On(pDX07->iCapPickerDown01);
	m_ledCapPickerIo[ 7].Set_On(pDX07->iCapPickerDown02);
	m_ledCapPickerIo[ 8].Set_On(pDX07->iCapPickerDown03);
	m_ledCapPickerIo[ 9].Set_On(pDX07->iCapPickerDown04);

	m_ledCapPickerIo[12].Set_On(pDX07->iCapPickerVac01);
	m_ledCapPickerIo[13].Set_On(pDX07->iCapPickerVac02);
	m_ledCapPickerIo[14].Set_On(pDX07->iCapPickerVac03);
	m_ledCapPickerIo[15].Set_On(pDX07->iCapPickerVac04);

	m_ledCapPickerIo[18].Set_On(pDX07->iCapPickerTurn01);
	m_ledCapPickerIo[19].Set_On(pDX07->iCapPickerTurn02);
	m_ledCapPickerIo[20].Set_On(pDX07->iCapPickerTurn03);
	m_ledCapPickerIo[21].Set_On(pDX07->iCapPickerTurn04);

	m_ledCapBufferIo[0].Set_On(pDX09->iCapBufferStageAlignIn);
	m_ledCapBufferIo[1].Set_On(pDX09->iCapBufferStageAlignOut);
	m_ledCapBufferIo[2].Set_On(pDX09->iCapCleanerUp);
	m_ledCapBufferIo[3].Set_On(pDX09->iCapCleanerDown);
}

///////////////////////////////////////////////////////////////////////////////

void CManualCapDlg::OnBtnCapPort1IoClick(UINT nID)
{
	if (!g_objCommon.Check_MainDoor()) return;
	int nIndex = nID - IDC_BTN_CAP_PORT1_IO_0;

	DY_DATA_02 *pDY02 = g_objAJinAXL.Get_pDY02();

	if (nIndex == 0) { pDY02->oCapPort1SlideLock   = TRUE; pDY02->oCapPort1SlideUnlock = FALSE; }
	if (nIndex == 1) { pDY02->oCapPort1SlideUnlock = TRUE; pDY02->oCapPort1SlideLock   = FALSE; }
	if (nIndex == 2) { pDY02->oCapPort1SupportIn   = TRUE; pDY02->oCapPort1SupportOut  = FALSE; }
	if (nIndex == 3) { pDY02->oCapPort1SupportOut  = TRUE; pDY02->oCapPort1SupportIn   = FALSE; }

	g_objAJinAXL.Write_Output(2);

	m_strLog.Format("[Manual Cap] Cap Port1 IO (%d) Click", nIndex);
	g_objLogFile.Save_HandlerLog(m_strLog);
}

void CManualCapDlg::OnBtnCapPort2IoClick(UINT nID)
{
	if (!g_objCommon.Check_MainDoor()) return;
	int nIndex = nID - IDC_BTN_CAP_PORT2_IO_0;

	DY_DATA_02 *pDY02 = g_objAJinAXL.Get_pDY02();

	if (nIndex == 0) { pDY02->oCapPort2SlideLock   = TRUE; pDY02->oCapPort2SlideUnlock = FALSE; }
	if (nIndex == 1) { pDY02->oCapPort2SlideUnlock = TRUE; pDY02->oCapPort2SlideLock   = FALSE; }
	if (nIndex == 2) { pDY02->oCapPort2SupportIn   = TRUE; pDY02->oCapPort2SupportOut  = FALSE; }
	if (nIndex == 3) { pDY02->oCapPort2SupportOut  = TRUE; pDY02->oCapPort2SupportIn   = FALSE; }

	g_objAJinAXL.Write_Output(2);

	m_strLog.Format("[Manual Cap] Cap Port2 IO (%d) Click", nIndex);
	g_objLogFile.Save_HandlerLog(m_strLog);
}

void CManualCapDlg::OnBtnCapStage1XClick(UINT nID)
{
	if (!g_objAJinAXL.Is_Home(AX_CAP_STAGE1_X)) return;
	if (!g_objAJinAXL.Is_Home(AX_CAP_STAGE1_Z)) return;
	if (!g_objAJinAXL.Is_Home(AX_CAP_STAGE2_Z)) return;

	if (!g_objCommon.Check_MainDoor()) return;
	int nIndex = nID - IDC_BTN_CAP_STAGE1_X_0;

	if (!g_objCommon.Check_Position(AX_CAP_PICKER_Y, 12) && !g_objCommon.Check_Position(AX_CAP_PICKER_Z, 0)) {
		AfxMessageBox("Cap Picker Ready Up 위치가 아닙니다. 확인 후 진행하세요."); return;
	}
	if ((g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 0) && g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 0)) ||
		(g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 1) && g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 1))) {
			AfxMessageBox("Cap Stage1, 2 Z축 높이가 같습니다. 확인 후 진행하세요."); return;
	}

	if ((g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 0) && g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 1)) ||
		(g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 1) && g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 0))) {

		if (nIndex == 3 || nIndex == 4) {
			EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();
			int nDir = (nIndex == 3 ? 1 : -1);
			double dMove = pEquipData->dCapTrayPitchY * nDir;	// temp
			g_objAJinAXL.Move_Relative(AX_CAP_STAGE1_X, dMove);

		} else {
			g_objCommon.Move_Position(AX_CAP_STAGE1_X, nIndex);
		}

	} else {
		AfxMessageBox("Cap Stage1, 2 Z축 높이 확인 후 진행하세요."); return;
	}

	m_strLog.Format("[Manual Cap] Cap Stage1 X (%d) Click", nIndex);
	g_objLogFile.Save_HandlerLog(m_strLog);
}

void CManualCapDlg::OnBtnCapStage1ZClick(UINT nID)
{
	if (!g_objAJinAXL.Is_Home(AX_CAP_STAGE1_X)) return;
	if (!g_objAJinAXL.Is_Home(AX_CAP_STAGE1_Z)) return;
	if (!g_objAJinAXL.Is_Home(AX_CAP_STAGE2_X)) return;
	if (!g_objCommon.Check_MainDoor()) return;
	int nIndex = nID - IDC_BTN_CAP_STAGE1_Z_0;

	double dStage1 = g_objAJinAXL.Get_Position(AX_CAP_STAGE1_X);
	double dStage2 = g_objAJinAXL.Get_Position(AX_CAP_STAGE2_X);
	double dDiff = fabs(dStage1 - dStage2);
	if (dDiff < 300.0) { AfxMessageBox("Cap Stage1, 2 충돌 위험!!! X축 위치 확인 후 진행하세요."); return; }

	if (nIndex == 2 || nIndex == 3) {
		if (!g_objCommon.Check_Position(AX_CAP_STAGE1_X, 0) && !g_objCommon.Check_Position(AX_CAP_STAGE1_X, 2)) {
			AfxMessageBox("Cap Port1 위치가 아닙니다. X축 확인 후 진행하세요."); return;
		}
	}

	g_objCommon.Move_Position(AX_CAP_STAGE1_Z, nIndex);

	m_strLog.Format("[Manual Cap] Cap Stage1 Z (%d) Click", nIndex);
	g_objLogFile.Save_HandlerLog(m_strLog);
}

void CManualCapDlg::OnBtnCapStage1IoClick(UINT nID)
{
	if (!g_objCommon.Check_MainDoor()) return;
	int nIndex = nID - IDC_BTN_CAP_STAGE1_IO_0;

	DY_DATA_04 *pDY04 = g_objAJinAXL.Get_pDY04();

	if (nIndex == 0) { pDY04->oCapStage1MasterIn = TRUE; }
	if (nIndex == 1) { pDY04->oCapStage1SlaveIn  = TRUE; }
	if (nIndex == 2) { pDY04->oCapStage1MasterIn = FALSE; }
	if (nIndex == 3) { pDY04->oCapStage1SlaveIn  = FALSE; }

	g_objAJinAXL.Write_Output(4);

	m_strLog.Format("[Manual Cap] Cap Stage1 IO (%d) Click", nIndex);
	g_objLogFile.Save_HandlerLog(m_strLog);
}

void CManualCapDlg::OnBtnCapStage2XClick(UINT nID)
{
	if (!g_objAJinAXL.Is_Home(AX_CAP_STAGE2_X)) return;
	if (!g_objAJinAXL.Is_Home(AX_CAP_STAGE2_Z)) return;
	if (!g_objAJinAXL.Is_Home(AX_CAP_STAGE1_Z)) return;

	if (!g_objCommon.Check_MainDoor()) return;
	int nIndex = nID - IDC_BTN_CAP_STAGE2_X_0;

	if (!g_objCommon.Check_Position(AX_CAP_PICKER_Y, 12) && !g_objCommon.Check_Position(AX_CAP_PICKER_Z, 0)) {
		AfxMessageBox("Cap Picker Ready Up 위치가 아닙니다. 확인 후 진행하세요."); return;
	}
	if ((g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 0) && g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 0)) ||
		(g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 1) && g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 1))) {
			AfxMessageBox("Cap Stage1, 2 Z축 높이가 같습니다. 확인 후 진행하세요."); return;
	}

	if ((g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 0) && g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 1)) ||
		(g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 1) && g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 0))) {

			if (nIndex == 3 || nIndex == 4) {
				EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();
				int nDir = (nIndex == 3 ? 1 : -1);
				double dMove = pEquipData->dCapTrayPitchY * nDir;	// temp
				g_objAJinAXL.Move_Relative(AX_CAP_STAGE2_X, dMove);

			} else {
				g_objCommon.Move_Position(AX_CAP_STAGE2_X, nIndex);
			}

	} else {
		AfxMessageBox("Cap Stage1, 2 Z축 높이 확인 후 진행하세요."); return;
	}

	m_strLog.Format("[Manual Cap] Cap Stage2 X (%d) Click", nIndex);
	g_objLogFile.Save_HandlerLog(m_strLog);
}

void CManualCapDlg::OnBtnCapStage2ZClick(UINT nID)
{
	if (!g_objAJinAXL.Is_Home(AX_CAP_STAGE2_X)) return;
	if (!g_objAJinAXL.Is_Home(AX_CAP_STAGE2_Z)) return;
	if (!g_objAJinAXL.Is_Home(AX_CAP_STAGE1_X)) return;

	if (!g_objCommon.Check_MainDoor()) return;
	int nIndex = nID - IDC_BTN_CAP_STAGE2_Z_0;

	double dStage1 = g_objAJinAXL.Get_Position(AX_CAP_STAGE1_X);
	double dStage2 = g_objAJinAXL.Get_Position(AX_CAP_STAGE2_X);
	double dDiff = fabs(dStage1 - dStage2);
	if (dDiff < 300.0) { AfxMessageBox("Cap Stage1, 2 충돌 위험!!! X축 위치 확인 후 진행하세요."); return; }

	if (nIndex == 2 || nIndex == 3) {
		if (!g_objCommon.Check_Position(AX_CAP_STAGE2_X, 0) && !g_objCommon.Check_Position(AX_CAP_STAGE2_X, 2)) {
			AfxMessageBox("Cap Port1 위치가 아닙니다. X축 확인 후 진행하세요."); return;
		}
	}

	g_objCommon.Move_Position(AX_CAP_STAGE2_Z, nIndex);

	m_strLog.Format("[Manual Cap] Cap Stage2 Z (%d) Click", nIndex);
	g_objLogFile.Save_HandlerLog(m_strLog);
}

void CManualCapDlg::OnBtnCapStage2IoClick(UINT nID)
{
	if (!g_objCommon.Check_MainDoor()) return;
	int nIndex = nID - IDC_BTN_CAP_STAGE2_IO_0;

	DY_DATA_04 *pDY04 = g_objAJinAXL.Get_pDY04();

	if (nIndex == 0) { pDY04->oCapStage2MasterIn = TRUE; }
	if (nIndex == 1) { pDY04->oCapStage2SlaveIn  = TRUE; }
	if (nIndex == 2) { pDY04->oCapStage2MasterIn = FALSE; }
	if (nIndex == 3) { pDY04->oCapStage2SlaveIn  = FALSE; }

	g_objAJinAXL.Write_Output(4);

	m_strLog.Format("[Manual Cap] Cap Stage2 IO (%d) Click", nIndex);
	g_objLogFile.Save_HandlerLog(m_strLog);
}

void CManualCapDlg::OnBtnCapPickerYClick(UINT nID)
{
	if (!g_objAJinAXL.Is_Home(AX_CAP_PICKER_Z)) return;
	if (!g_objAJinAXL.Is_Home(AX_CAP_PICKER_Y)) return;

	if (!g_objCommon.Check_MainDoor()) return;
	int nIndex = nID - IDC_BTN_CAP_PICKER_Y_0;
	DX_DATA_07 *pDX07 = g_objAJinAXL.Get_pDX07();

	if (!g_objCommon.Check_Position(AX_CAP_PICKER_Z, 0)) {
		AfxMessageBox("Cap Picker Ready Up 위치가 아닙니다. 확인 후 진행하세요."); return;
	}
	if (!pDX07->iCapPickerUp01 || !pDX07->iCapPickerUp02 || !pDX07->iCapPickerUp03 || !pDX07->iCapPickerUp04) {
		AfxMessageBox("Cap Picker Up 상태가 아닙니다. 확인 후 진행하세요."); return;
	}

	if (nIndex == 13 || nIndex == 14) {
		EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();
		int nDir = (nIndex == 14 ? 1 : -1);
		double dMove = pEquipData->dCapTrayPitchX * nDir;	// temp
		g_objAJinAXL.Move_Relative(AX_CAP_PICKER_Y, dMove);

	} else {
		g_objCommon.Move_Position(AX_CAP_PICKER_Y, nIndex);
	}

	m_strLog.Format("[Manual Cap] Cap Picker Y (%d) Click", nIndex);
	g_objLogFile.Save_HandlerLog(m_strLog);
}

void CManualCapDlg::OnBtnCapPickerZClick(UINT nID)
{
	if (!g_objAJinAXL.Is_Home(AX_CAP_PICKER_Z)) return;

	if (!g_objCommon.Check_MainDoor()) return;
	int nIndex = nID - IDC_BTN_CAP_PICKER_Z_0;

	MOVE_DATA *pMoveData = g_objDataManager.Get_pMoveData();
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	if (nIndex == 1 && !g_objCommon.Check_Position(AX_CAP_PICKER_Y, 0)) {
		double dCur = g_objAJinAXL.Get_Position(AX_CAP_PICKER_Y);
		double dPos1, dPos2;

		if (pEquipData->bCapPickUpMulti) {
			dPos1 = pMoveData->dCapPickerY[0] + pEquipData->dCapTrayPitchX * (CT_X-1);
			dPos2 = pMoveData->dCapPickerY[0] - pEquipData->dCapTrayPitchX * (CT_X-1);
		} else {
			dPos1 = pMoveData->dCapPickerY[5];
			dPos2 = pMoveData->dCapPickerY[0] + pEquipData->dCapTrayPitchX * (CT_X-1);
		}

		if (dPos1 < dCur || dPos2 > dCur) {
			AfxMessageBox("Y축이 Cap Stage1 위치가 아닙니다. 확인 후 진행하세요."); return;
		}
	}

	if (nIndex == 2 && !g_objCommon.Check_Position(AX_CAP_PICKER_Y, 6)) {
		double dCur = g_objAJinAXL.Get_Position(AX_CAP_PICKER_Y);
		double dPos1, dPos2;

		if (pEquipData->bCapPickUpMulti) {
			dPos1 = pMoveData->dCapPickerY[6] + pEquipData->dCapTrayPitchX * (CT_X-1);
			dPos2 = pMoveData->dCapPickerY[6] - pEquipData->dCapTrayPitchX * (CT_X-1);
		} else {
			dPos1 = pMoveData->dCapPickerY[11];
			dPos2 = pMoveData->dCapPickerY[6] + pEquipData->dCapTrayPitchX * (CT_X-1);
		}

		if (dPos1 < dCur || dPos2 > dCur) {
			AfxMessageBox("Y축이 Cap Stage2 위치가 아닙니다. 확인 후 진행하세요."); return;
		}
	}

	if (nIndex == 3 && !g_objCommon.Check_Position(AX_CAP_PICKER_Y, 12)) {
		AfxMessageBox("Y축이 Cap buffer 위치가 아닙니다. 확인 후 진행하세요."); return;
	}

	g_objCommon.Move_Position(AX_CAP_PICKER_Z, nIndex);

	m_strLog.Format("[Manual Cap] Cap Picker Z (%d) Click", nIndex);
	g_objLogFile.Save_HandlerLog(m_strLog);
}

void CManualCapDlg::OnBtnCapPickerPClick(UINT nID)
{
	if (!g_objAJinAXL.Is_Home(AX_CAP_PICKER_Z)) return;
	if (!g_objAJinAXL.Is_Home(AX_CAP_PICKER_P)) return;
	if (!g_objCommon.Check_MainDoor()) return;
	int nIndex = nID - IDC_BTN_CAP_PICKER_P_0;

	if (!g_objCommon.Check_Position(AX_CAP_PICKER_Z, 0)) {
		AfxMessageBox("Z축이 Ready Up 위치가 아닙니다. Z축 확인 후 진행하세요."); return;
	}

	g_objCommon.Move_Position(AX_CAP_PICKER_P, nIndex);

	m_strLog.Format("[Manual Cap] Cap Picker P (%d) Click", nIndex);
	g_objLogFile.Save_HandlerLog(m_strLog);
}

void CManualCapDlg::OnBtnCapPickerIoClick(UINT nID)
{
// 	if (!g_objAJinAXL.Is_Home(AX_CAP_PICKER_Z)) return;
	if (!g_objCommon.Check_MainDoor()) return;
	int nIndex = nID - IDC_BTN_CAP_PICKER_IO_0;

	DY_DATA_07 *pDY07 = g_objAJinAXL.Get_pDY07();
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	if (nIndex == 0 || nIndex == 36) pDY07->oCapPickerDown01 = FALSE;
	if (nIndex == 1 || nIndex == 36) pDY07->oCapPickerDown02 = FALSE;
	if (nIndex == 2 || nIndex == 36) pDY07->oCapPickerDown03 = FALSE;
	if (nIndex == 3 || nIndex == 36) pDY07->oCapPickerDown04 = FALSE;
	
	if (nIndex ==  6 || nIndex == 37) pDY07->oCapPickerDown01 = TRUE;
	if (nIndex ==  7 || nIndex == 37) pDY07->oCapPickerDown02 = TRUE;
	if (nIndex ==  8 || nIndex == 37) pDY07->oCapPickerDown03 = TRUE;
	if (nIndex ==  9 || nIndex == 37) pDY07->oCapPickerDown04 = TRUE;

	if (nIndex == 12 || nIndex == 38) { pDY07->oCapPickerVac01 = TRUE; pDY07->oCapPickerAir01 = FALSE; }
	if (nIndex == 13 || nIndex == 38) { pDY07->oCapPickerVac02 = TRUE; pDY07->oCapPickerAir02 = FALSE; }
	if (nIndex == 14 || nIndex == 38) { pDY07->oCapPickerVac03 = TRUE; pDY07->oCapPickerAir03 = FALSE; }
	if (nIndex == 15 || nIndex == 38) { pDY07->oCapPickerVac04 = TRUE; pDY07->oCapPickerAir04 = FALSE; }

	if (nIndex == 18 || nIndex == 39) { pDY07->oCapPickerVac01 = FALSE; pDY07->oCapPickerAir01 = TRUE; }
	if (nIndex == 19 || nIndex == 39) { pDY07->oCapPickerVac02 = FALSE; pDY07->oCapPickerAir02 = TRUE; }
	if (nIndex == 20 || nIndex == 39) { pDY07->oCapPickerVac03 = FALSE; pDY07->oCapPickerAir03 = TRUE; }
	if (nIndex == 21 || nIndex == 39) { pDY07->oCapPickerVac04 = FALSE; pDY07->oCapPickerAir04 = TRUE; }

	if ((nIndex > 17 && nIndex < 24) || nIndex == 39) { g_objAJinAXL.Write_Output(7); Sleep(100); }

	if (nIndex == 18 || nIndex == 39) { pDY07->oCapPickerAir01 = FALSE; }
	if (nIndex == 19 || nIndex == 39) { pDY07->oCapPickerAir02 = FALSE; }
	if (nIndex == 20 || nIndex == 39) { pDY07->oCapPickerAir03 = FALSE; }
	if (nIndex == 21 || nIndex == 39) { pDY07->oCapPickerAir04 = FALSE; }

	if (nIndex == 24 || nIndex == 40) { pDY07->oCapPickerTurn01 = FALSE; }
	if (nIndex == 25 || nIndex == 40) { pDY07->oCapPickerTurn02 = FALSE; }
	if (nIndex == 26 || nIndex == 40) { pDY07->oCapPickerTurn03 = FALSE; }
	if (nIndex == 27 || nIndex == 40) { pDY07->oCapPickerTurn04 = FALSE; }

	if (nIndex == 30 || nIndex == 41) { pDY07->oCapPickerTurn01 = TRUE; }
	if (nIndex == 31 || nIndex == 41) { pDY07->oCapPickerTurn02 = TRUE; }
	if (nIndex == 32 || nIndex == 41) { pDY07->oCapPickerTurn03 = TRUE; }
	if (nIndex == 33 || nIndex == 41) { pDY07->oCapPickerTurn04 = TRUE; }

	g_objAJinAXL.Write_Output(7);

	m_strLog.Format("[Manual Cap] Cap Picker IO (%d) Click", nIndex);
	g_objLogFile.Save_HandlerLog(m_strLog);
}

void CManualCapDlg::OnBtnCapBufferYClick(UINT nID)
{
	if (!g_objAJinAXL.Is_Home(AX_CAP_BUFFER_Y)) return;
	if (!g_objCommon.Check_MainDoor()) return;
	int nIndex = nID - IDC_BTN_CAP_BUFFER_Y_0;
	DX_DATA_09 *pDX09 = g_objAJinAXL.Get_pDX09();

	if (pDX09->iCapCleanerUp) {
		AfxMessageBox("Cap Clean 노즐이 Up 되어 있습니다. 확인 후 진행하세요."); return;
	}
	if (!g_objCommon.Check_Position(AX_CAP_PICKER_Y, 0) && !g_objCommon.Check_Position(AX_CAP_PICKER_Y, 6) && !g_objCommon.Check_Position(AX_CAP_PICKER_Z, 0)) {
		AfxMessageBox("Cap Picker Ready Up 위치가 아닙니다. 확인 후 진행하세요."); return;
	}
	if (!g_objCommon.Check_Position(AX_ASSY_PICKER_X, 1) && !g_objCommon.Check_Position(AX_ASSY_PICKER_X, 2) && !g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 0)) {
		AfxMessageBox("Assembly Picker Ready Up 위치가 아닙니다. 확인 후 진행하세요."); return;
	}

	if(!g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 0))
	{
		AfxMessageBox("Assembly Picker Z Ready Up 위치가 아닙니다. 확인 후 진행하세요."); return;
	}

	if(!g_objCommon.Check_Position(AX_CAP_PICKER_Z, 0))
	{
		AfxMessageBox("Cap Picker Z Ready Up 위치가 아닙니다. 확인 후 진행하세요."); return;
	}

	if (nIndex == 4 || nIndex == 5) {
		EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();
		int nDir = (nIndex == 5 ? 1 : -1);
		double dMove = pEquipData->dIndexPitch * nDir;	// temp
		g_objAJinAXL.Move_Relative(AX_CAP_BUFFER_Y, dMove);

	} else {
		g_objCommon.Move_Position(AX_CAP_BUFFER_Y, nIndex);
	}

	m_strLog.Format("[Manual Cap] Cap Buffer Y (%d) Click", nIndex);
	g_objLogFile.Save_HandlerLog(m_strLog);
}

void CManualCapDlg::OnBtnCapBufferIoClick(UINT nID)
{
	if (!g_objCommon.Check_MainDoor()) return;
	int nIndex = nID - IDC_BTN_CAP_BUFFER_IO_0;

	DY_DATA_09 *pDY09 = g_objAJinAXL.Get_pDY09();

	// Buffer Align In/Out
	if (nIndex == 0) pDY09->oCapBufferAlignIn = TRUE;
	if (nIndex == 1) pDY09->oCapBufferAlignIn = FALSE;

	if (nIndex == 2 || nIndex == 3) {
		if(!g_objAJinAXL.Is_Home(AX_CAP_BUFFER_Y)) {
			AfxMessageBox("Cap Buffer Y 초기화 완료 상태가 아닙니다."); return;
		}
		if (!g_objCommon.Check_Position(AX_CAP_BUFFER_Y, 1) && !g_objCommon.Check_Position(AX_CAP_BUFFER_Y, 2)) {
			AfxMessageBox("Cap Buffer Y가 Cleaning 또는 Unload  위치가 아닙니다. 확인 후 진행하세요."); return;
		}

		//Cleaner Up/Down
		if (nIndex == 2) pDY09->oCapCleanerUp = TRUE;
		if (nIndex == 3) pDY09->oCapCleanerUp = FALSE;
	}

	// Clean Air On/Off
	if (nIndex == 4 || nIndex == 16) { pDY09->oCapCleanAir01 = TRUE; pDY09->oCapCleanSuction = TRUE; }
	if (nIndex == 5 || nIndex == 16) { pDY09->oCapCleanAir02 = TRUE; pDY09->oCapCleanSuction = TRUE; }
	if (nIndex == 6 || nIndex == 16) { pDY09->oCapCleanAir03 = TRUE; pDY09->oCapCleanSuction = TRUE; }
	if (nIndex == 7 || nIndex == 16) { pDY09->oCapCleanAir04 = TRUE; pDY09->oCapCleanSuction = TRUE; }

	if (nIndex == 10 || nIndex == 17) { pDY09->oCapCleanAir01 = FALSE; pDY09->oCapCleanSuction = FALSE; }
	if (nIndex == 11 || nIndex == 17) { pDY09->oCapCleanAir02 = FALSE; pDY09->oCapCleanSuction = FALSE; }
	if (nIndex == 12 || nIndex == 17) { pDY09->oCapCleanAir03 = FALSE; pDY09->oCapCleanSuction = FALSE; }
	if (nIndex == 13 || nIndex == 17) { pDY09->oCapCleanAir04 = FALSE; pDY09->oCapCleanSuction = FALSE; }

	g_objAJinAXL.Write_Output(9);

	m_strLog.Format("[Manual Cap] Cap Buffer IO (%d) Click", nIndex);
	g_objLogFile.Save_HandlerLog(m_strLog);
}

///////////////////////////////////////////////////////////////////////////////
