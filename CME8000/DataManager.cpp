// DataManager.cpp: 구현 파일
//
#include "stdafx.h"
#include "CME8000.h"
#include "DataManager.h"

#include "Common.h"
#include "CME8000Dlg.h"
#include "SequenceMain.h"
#include "LogFile.h"

// CDataManager
CDataManager g_objDataManager;

CDataManager::CDataManager()
{
	Reset_EquipData();
	Reset_MoveData();
}

CDataManager::~CDataManager()
{
}

void CDataManager::Reset_EquipData()
{
	m_EquipData.sEquipName = "";
	m_EquipData.sModel = "";
	m_EquipData.nLotBarcodePort = 0;
	m_EquipData.nAssyLoadCellPort = 0;
	m_EquipData.nUnloadLoadCellPort = 0;
	m_EquipData.bUseDoorLock = FALSE;
	m_EquipData.nVendorSelection = 0;


	m_EquipData.bUseMES = FALSE;
	m_EquipData.bUseInlineMode = FALSE;
	m_EquipData.bUseVisionCapDir = FALSE;
	m_EquipData.bUseVisionCmAlign = FALSE;
	m_EquipData.bUseVisionAlignAlarm = FALSE;
	m_EquipData.bUseVisionAlignOffset = FALSE;

	m_EquipData.bUseTrayPickerTurn = FALSE;
	m_EquipData.bUseCapPickerTurn = FALSE;
	m_EquipData.bChkAssyPickerTilt = FALSE;
	m_EquipData.bCapPickUpMulti = FALSE;
	m_EquipData.bUseMesCapReg = FALSE;
	m_EquipData.bUseMesShipReg = FALSE;
	m_EquipData.bUseIndexAssyVac = FALSE;

	m_EquipData.dLoadTrayPitchX = 0.0;
	m_EquipData.dLoadTrayPitchY = 0.0;
	m_EquipData.dCapTrayPitchX = 0.0;
	m_EquipData.dCapTrayPitchY = 0.0;
	m_EquipData.dShipTrayPitchX = 0.0;
	m_EquipData.dShipTrayPitchY = 0.0;
	m_EquipData.dIndexPitch = 0.0;

	for (int i = 0; i < 3; i++) m_EquipData.nVacOffDelay[i] = 0;
	for (int i = 0; i < 6; i++) m_EquipData.nDelayAdd[i] = 0;

	m_EquipData.sAviIp = "0.0.0.0";
	m_EquipData.dAlignOffset = 0.0;
	for (int i = 0; i < 2; i++) m_EquipData.dLoadCellRange[i] = 0.0;	// 0:Min, 1:Max 

	for (int i = 0; i < 6; i++) for (int j = 0; j < 4; j++) m_EquipData.bTower[i][j] = FALSE;
	for (int i = 0; i < 5; i++) for (int j = 0; j < 6; j++) m_EquipData.bBuzzer[i][j] = FALSE;

	m_EquipData.sPasswordMt = "";
	m_EquipData.sPasswordSi = "";

	m_EquipData.nCappingCnt = 0;
	m_EquipData.nLoadCellChkCnt = 0;

	m_EquipData.bResultTestUse = FALSE;
	m_EquipData.nResultTestNg = 0;


	//m_EquipData.sVendor[0] = "DH";
	//m_EquipData.sVendor[1] = "HS";
}

void CDataManager::Reset_MoveData()
{
	for (int i = 0; i < 3; i++) m_MoveData.dTrayPickerX[i];		// AXIS 00
	for (int i = 0; i < 4; i++) m_MoveData.dTrayPickerZ[i];		// AXIS 01
	for (int i = 0; i < 2; i++) m_MoveData.dTrayPickerR[i];		// AXIS 02
	for (int i = 0; i < 2; i++) m_MoveData.dLoadStage1X[i];		// AXIS 04
	for (int i = 0; i < 2; i++) m_MoveData.dLoadStage1Z[i];		// AXIS 05
	for (int i = 0; i < 2; i++) m_MoveData.dLoadStage2X[i];		// AXIS 06
	for (int i = 0; i < 1; i++) m_MoveData.dLoadStage2Z[i];		// AXIS 07
	for (int i = 0; i < 3; i++) m_MoveData.dLoadPickerY[i];		// AXIS 08
	for (int i = 0; i < 5; i++) m_MoveData.dLoadPickerZ[i];		// AXIS 09
	for (int i = 0; i < 6; i++) m_MoveData.dLoadPickerP[i];		// AXIS 00
	for (int i = 0; i < 5; i++) m_MoveData.dMainIndexR[i];		// AXIS 11
	for (int i = 0; i < 6; i++) m_MoveData.dVisionCmX[i];		// AXIS 12
	for (int i = 0; i < 6; i++) m_MoveData.dCapStage1X[i];		// AXIS 13
	for (int i = 0; i < 4; i++) m_MoveData.dCapStage1Z[i];		// AXIS 14
	for (int i = 0; i < 2; i++) m_MoveData.dCapStage1X[i];		// AXIS 15
	for (int i = 0; i < 2; i++) m_MoveData.dCapStage1Z[i];		// AXIS 16
	for (int i = 0; i < 13; i++) m_MoveData.dCapPickerY[i];		// AXIS 17
	for (int i = 0; i < 6; i++) m_MoveData.dCapPickerZ[i];		// AXIS 18
	for (int i = 0; i < 6; i++) m_MoveData.dCapPickerP[i];		// AXIS 19
	for (int i = 0; i < 6; i++) m_MoveData.dCapBufferY[i];		// AXIS 10
	for (int i = 0; i < 3; i++) m_MoveData.dVisionCapY[i];		// AXIS 21
	for (int i = 0; i < 7; i++) m_MoveData.dAssyPickerX[i];		// AXIS 22
	for (int i = 0; i < 7; i++) m_MoveData.dAssyPickerY[i];		// AXIS 23
	for (int i = 0; i < 7; i++) m_MoveData.dAssyPickerZ[i];		// AXIS 24
	for (int i = 0; i < 2; i++) m_MoveData.dTransStageX[i];		// AXIS 25
	for (int i = 0; i < 1; i++) m_MoveData.dTransStageZ[i];		// AXIS 26
	for (int i = 0; i < 6; i++) m_MoveData.dTransStageT[i];		// AXIS 27
	for (int i = 0; i < 4; i++) m_MoveData.dUnloadPickerX[i];	// AXIS 28
	for (int i = 0; i < 2; i++) m_MoveData.dUnloadPickerZ[i];	// AXIS 29
	for (int i = 0; i < 2; i++) m_MoveData.dUnloadPickerP[i];	// AXIS 20
	for (int i = 0; i < 6; i++) m_MoveData.dUnloadStage1Y[i];	// AXIS 31
	for (int i = 0; i < 6; i++) m_MoveData.dUnloadStage1Z[i];	// AXIS 32
	for (int i = 0; i < 7; i++) m_MoveData.dUnloadStage2Y[i];	// AXIS 33
	for (int i = 0; i < 5; i++) m_MoveData.dUnloadStage2Z[i];	// AXIS 34

}

BOOL CDataManager::Read_EquipData()
{
 	CIniFileCS INI(gsCurrentDir + "\\System\\EquipData.ini");
	if (!INI.Check_File()) { AfxMessageBox("EquipData.ini File Not Found!!!"); return FALSE; }

	CString strKey;
	m_EquipData.sEquipName = INI.Get_String("EQUIPMENT", "NAME", "");
	m_EquipData.sModel = INI.Get_String("EQUIPMENT", "MODEL", "");
	gData.sRecipe = (m_EquipData.sModel == "" ? "R53B" : m_EquipData.sModel);	// Default(R53B)

	m_EquipData.nLotBarcodePort = INI.Get_Integer("EQUIPMENT", "LOT_BARCODE", 1);
	m_EquipData.nAssyLoadCellPort = INI.Get_Integer("EQUIPMENT", "ASSY_LOAD_CELL", 8);
	m_EquipData.nUnloadLoadCellPort = INI.Get_Integer("EQUIPMENT", "UNLOAD_LOAD_CELL", 9);
	m_EquipData.bUseDoorLock = INI.Get_Bool("EQUIPMENT", "DOOR_LOCK", FALSE);
	gData.nDoorLockTime = INI.Get_Integer("EQUIPMENT", "DOOR_LOCK_TIME", 0);
	gAlm.dMotionChkPos		= INI.Get_Double("EQUIPMENT", "MOTION_CHECK", 0.0);

	m_EquipData.bUseMES = INI.Get_Bool("OPTION", "MES_USE", FALSE);
	m_EquipData.bUseInlineMode = INI.Get_Bool("OPTION", "INLINE_MODE", FALSE);

	m_EquipData.nVendorSelection = INI.Get_Integer("EQUIPMENT", "VENDOR_SELECTION", 0);
	m_EquipData.sVendor[0]=INI.Get_String("VENDOR","0", "");
	m_EquipData.sVendor[1]=INI.Get_String("VENDOR","1", "");
	gData.sVendor = m_EquipData.sVendor[m_EquipData.nVendorSelection];

#ifdef DRY_RUN_TEST
	m_EquipData.bUseVisionCapDir = FALSE;
	m_EquipData.bUseVisionCmAlign = FALSE;
#else
	m_EquipData.bUseVisionCapDir = INI.Get_Bool("OPTION", "VISION_CAP_DIR", FALSE);
	m_EquipData.bUseVisionCmAlign = INI.Get_Bool("OPTION", "VISION_CM_ALIGN", FALSE);
#endif
	m_EquipData.bUseVisionAlignAlarm = INI.Get_Bool("OPTION", "VISION_ALIGN_ALARM", FALSE);
	m_EquipData.bUseVisionAlignOffset = INI.Get_Bool("OPTION", "VISION_ALIGN_OFFSET", FALSE);
		
	m_EquipData.nInspectCmScanTimes = (ST_Y * ST_X) / 4;//INI.Get_Integer("OPTION", "SCAN_TIMES", 10000);	
	m_EquipData.nInspectCmLotTimes = INI.Get_Integer("OPTION", "LOT_TIMES", 10000);
	m_EquipData.nInspectCmMinutes = INI.Get_Integer("OPTION", "MINUTES", 10000);
		
	m_EquipData.bUseTrayPickerTurn = INI.Get_Bool("OPTION", "TRAY_PICKER_TURN", FALSE);
	m_EquipData.bUseCapPickerTurn = INI.Get_Bool("OPTION", "CAP_PICKER_TURN", FALSE);
	m_EquipData.bChkAssyPickerTilt = INI.Get_Bool("OPTION", "ASSY_PICKER_TILT", FALSE);
	m_EquipData.bCapPickUpMulti = INI.Get_Bool("OPTION", "CAP_PICKUP_MULTI", TRUE);

	m_EquipData.bUseMesCapReg = INI.Get_Bool("OPTION", "MES_CAP_REG", FALSE);
	m_EquipData.bUseMesShipReg = INI.Get_Bool("OPTION", "MES_SHIP_REG", FALSE);
	m_EquipData.bUseIndexAssyVac = INI.Get_Bool("OPTION", "INDEX_ASSY_VAC", FALSE);

	m_EquipData.dIndexPitch = INI.Get_Double ("INDEX", "PITCH", 0.0);
	m_EquipData.dLoadTrayPitchX = INI.Get_Double("LOAD_TRAY", "PITCH_X", 0.0);
	m_EquipData.dLoadTrayPitchY = INI.Get_Double("LOAD_TRAY", "PITCH_Y", 0.0);
	m_EquipData.dCapTrayPitchX = INI.Get_Double("CAP_TRAY", "PITCH_X", 0.0);
	m_EquipData.dCapTrayPitchY = INI.Get_Double("CAP_TRAY", "PITCH_Y", 0.0);
	m_EquipData.dShipTrayPitchX = INI.Get_Double("SHIP_TRAY", "PITCH_X_" + gData.sRecipe, 0.0);
	m_EquipData.dShipTrayPitchY = INI.Get_Double("SHIP_TRAY", "PITCH_Y_" + gData.sRecipe, 0.0);

	for (int i = 0; i < 3; i++) { strKey.Format("%d", i); m_EquipData.nVacOffDelay[i] = INI.Get_Integer("VAC_OFF_DELAY", strKey, 30); }
	for (int i = 0; i < 6; i++) { strKey.Format("%d", i); m_EquipData.nDelayAdd[i] = INI.Get_Integer("DELAY_ADD", strKey, 100); }

	m_EquipData.sAviIp = INI.Get_String("AVI", "AVI_IP", "");
	m_EquipData.dAlignOffset = INI.Get_Double ("VISION_ALIGN", "OFFSET", 0.0);
	m_EquipData.dLoadCellRange[0] = INI.Get_Double ("LOAD_CELL", "MIN", 0.0);
	m_EquipData.dLoadCellRange[1] = INI.Get_Double ("LOAD_CELL", "MAX", 0.0);
	
	for (int i = 0; i < 6; i++) for (int j = 0; j < 4; j++) { strKey.Format("%d%d", i, j); m_EquipData.bTower[i][j] = INI.Get_Bool("TOWER", strKey, FALSE); }
	for (int i = 0; i < 5; i++) for (int j = 0; j < 6; j++) { strKey.Format("%d%d", i, j); m_EquipData.bBuzzer[i][j] = INI.Get_Bool("BUZZER", strKey, FALSE); }

	m_EquipData.sPasswordMt = INI.Get_String("HIDDEN", "PASSWORD_MT", "");
	m_EquipData.sPasswordSi = INI.Get_String("HIDDEN", "PASSWORD_SI", "");

	m_EquipData.nLoadCellChkCnt = INI.Get_Integer("LOAD_CELL", "CHECK_COUNT", 0);
	for (int i = 0; i < PICK; i++) { strKey.Format("%d", i); gData.dAssyLoadCell[i] = INI.Get_Double("ASSY_LOAD_CELL", strKey, 0.0); }

	// Gloval Data
	gData.nCmMaxCount = LT_Y * LT_X;	// Tray 배열 (3X4)
	gData.nCapMaxCount = CT_Y * CT_X;	// Tray 배열 (4x7)
	gData.STY =  ST_Y;
	gData.nShipMaxCount = gData.STY * ST_X;	// Tray 배열 (3x7 or 3x6)
	
	m_EquipData.nResultTestNg = INI.Get_Integer("RESULT_TEST", "RESULT_NG", 0);

	//doorinterlock log

	if (gDoorLock.nOpenStart == 0 && m_EquipData.bUseDoorLock == FALSE)
		g_objLogFile.Save_Interlock(2);
	if (gDoorLock.nOpenStart == 1 && m_EquipData.bUseDoorLock == TRUE) 
		g_objLogFile.Save_Interlock(3);



	CCME8000Dlg *pMainDlg = (CCME8000Dlg*)AfxGetApp()->GetMainWnd();
	pMainDlg->Display_EquipName();

	return TRUE;
}

BOOL CDataManager::Read_MoveData()
{
	CIniFileCS INI(gsCurrentDir + "\\System\\MoveData_" + gData.sVendor + ".ini");
	if (!INI.Check_File()) { AfxMessageBox("MoveData.ini File Not Found!!!"); return FALSE; }

	CString strKey;

	for (int i = 0; i < 3; i++) { strKey.Format("%02d", i); m_MoveData.dTrayPickerX[i]		= INI.Get_Double("00_TRAY_PICKER_X", strKey, 0.0); }
	for (int i = 0; i < 3; i++) { strKey.Format("%02d", i); m_MoveData.dTrayPickerZ[i]		= INI.Get_Double("01_TRAY_PICKER_Z", strKey, 0.0); }
	for (int i = 0; i < 2; i++) { strKey.Format("%02d", i); m_MoveData.dTrayPickerR[i]		= INI.Get_Double("02_TRAY_PICKER_R", strKey, 0.0); }
	for (int i = 0; i < 4; i++) { strKey.Format("%02d", i); m_MoveData.dLoadStage1X[i]		= INI.Get_Double("04_LOAD_STAGE1_X", strKey, 0.0); }
	for (int i = 0; i < 5; i++) { strKey.Format("%02d", i); m_MoveData.dLoadStage1Z[i]		= INI.Get_Double("05_LOAD_STAGE1_Z", strKey, 0.0); }
	for (int i = 0; i < 4; i++) { strKey.Format("%02d", i); m_MoveData.dLoadStage2X[i]		= INI.Get_Double("06_LOAD_STAGE2_X", strKey, 0.0); }
	for (int i = 0; i < 5; i++) { strKey.Format("%02d", i); m_MoveData.dLoadStage2Z[i]		= INI.Get_Double("07_LOAD_STAGE2_Z", strKey, 0.0); }
	for (int i = 0; i < 3; i++) { strKey.Format("%02d", i); m_MoveData.dLoadPickerY[i]		= INI.Get_Double("08_LOAD_PICKER_Y", strKey, 0.0); }
	for (int i = 0; i < 4; i++) { strKey.Format("%02d", i); m_MoveData.dLoadPickerZ[i]		= INI.Get_Double("09_LOAD_PICKER_Z", strKey, 0.0); }
	for (int i = 0; i < 2; i++) { strKey.Format("%02d", i); m_MoveData.dLoadPickerP[i]		= INI.Get_Double("10_LOAD_PICKER_P", strKey, 0.0); }
	for (int i = 0; i < 1; i++) { strKey.Format("%02d", i); m_MoveData.dMainIndexR[i]		= INI.Get_Double("11_INDEX_R", strKey, 0.0); }
	for (int i = 0; i < 2; i++) { strKey.Format("%02d", i); m_MoveData.dVisionCmX[i]		= INI.Get_Double("12_VISION_CM_ALIGN_X", strKey, 0.0); }
	for (int i = 0; i < 3; i++) { strKey.Format("%02d", i); m_MoveData.dCapStage1X[i]		= INI.Get_Double("13_CAP_STAGE1_X", strKey, 0.0); }
	for (int i = 0; i < 4; i++) { strKey.Format("%02d", i); m_MoveData.dCapStage1Z[i]		= INI.Get_Double("14_CAP_STAGE1_Z", strKey, 0.0); }
	for (int i = 0; i < 3; i++) { strKey.Format("%02d", i); m_MoveData.dCapStage2X[i]		= INI.Get_Double("15_CAP_STAGE2_X", strKey, 0.0); }
	for (int i = 0; i < 4; i++) { strKey.Format("%02d", i); m_MoveData.dCapStage2Z[i]		= INI.Get_Double("16_CAP_STAGE2_Z", strKey, 0.0); }
	for (int i = 0; i < 13; i++) { strKey.Format("%02d", i); m_MoveData.dCapPickerY[i]		= INI.Get_Double("17_CAP_PICKER_Y", strKey, 0.0); }
	for (int i = 0; i < 4; i++) { strKey.Format("%02d", i); m_MoveData.dCapPickerZ[i]		= INI.Get_Double("18_CAP_PICKER_Z", strKey, 0.0); }
	for (int i = 0; i < 2; i++) { strKey.Format("%02d", i); m_MoveData.dCapPickerP[i]		= INI.Get_Double("19_CAP_PICKER_P", strKey, 0.0); }
	for (int i = 0; i < 4; i++) { strKey.Format("%02d", i); m_MoveData.dCapBufferY[i]		= INI.Get_Double("20_CAP_BUFFER_STAGE_Y", strKey, 0.0); }
	for (int i = 0; i < 1; i++) { strKey.Format("%02d", i); m_MoveData.dVisionCapY[i]		= INI.Get_Double("21_VISION_CAP_ALIGN_Y", strKey, 0.0); }
	for (int i = 0; i < 7; i++) { strKey.Format("%02d", i); m_MoveData.dAssyPickerX[i]		= INI.Get_Double("22_ASSEMBLY_PICKER_X", strKey, 0.0); }
	for (int i = 0; i < 7; i++) { strKey.Format("%02d", i); m_MoveData.dAssyPickerY[i]		= INI.Get_Double("23_ASSEMBLY_PICKER_Y", strKey, 0.0); }
	for (int i = 0; i < 7; i++) { strKey.Format("%02d", i); m_MoveData.dAssyPickerZ[i]		= INI.Get_Double("24_ASSEMBLY_PICKER_Z", strKey, 0.0); }
	for (int i = 0; i < 2; i++) { strKey.Format("%02d", i); m_MoveData.dTransStageX[i]		= INI.Get_Double("25_TRANS_STAGE_X", strKey, 0.0); }
	for (int i = 0; i < 3; i++) { strKey.Format("%02d", i); m_MoveData.dTransStageZ[i]		= INI.Get_Double("26_TRANS_STAGE_Z", strKey, 0.0); }
	for (int i = 0; i < 2; i++) { strKey.Format("%02d", i); m_MoveData.dTransStageT[i]		= INI.Get_Double("27_TRANS_STAGE_T", strKey, 0.0); }
	for (int i = 0; i < 4; i++) { strKey.Format("%02d", i); m_MoveData.dUnloadPickerX[i]	= INI.Get_Double("28_UNLOAD_PICKER_X", strKey, 0.0); }
	for (int i = 0; i < 4; i++) { strKey.Format("%02d", i); m_MoveData.dUnloadPickerZ[i]	= INI.Get_Double("29_UNLOAD_PICKER_Z", strKey, 0.0); }
	for (int i = 0; i < 2; i++) { strKey.Format("%02d", i); m_MoveData.dUnloadPickerP[i]	= INI.Get_Double("30_UNLOAD_PICKER_P", strKey, 0.0); }
	for (int i = 0; i < 3; i++) { strKey.Format("%02d", i); m_MoveData.dUnloadStage1Y[i]	= INI.Get_Double("31_UNLOAD_STAGE1_Y", strKey, 0.0); }
	for (int i = 0; i < 6; i++) { strKey.Format("%02d", i); m_MoveData.dUnloadStage1Z[i]	= INI.Get_Double("32_UNLOAD_STAGE1_Z", strKey, 0.0); }
	for (int i = 0; i < 3; i++) { strKey.Format("%02d", i); m_MoveData.dUnloadStage2Y[i]	= INI.Get_Double("33_UNLOAD_STAGE2_Y", strKey, 0.0); }
	for (int i = 0; i < 6; i++) { strKey.Format("%02d", i); m_MoveData.dUnloadStage2Z[i]	= INI.Get_Double("34_UNLOAD_STAGE2_Z", strKey, 0.0); }

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
