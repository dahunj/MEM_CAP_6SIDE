// SequenceMain.cpp : 구현 파일
//
#include "stdafx.h"
#include "CME8000.h"
#include "CME8000Dlg.h"
#include "SequenceMain.h"
#include <math.h>

#include "LogFile.h"
#include "Common.h"
#include "Inspector.h"
#include "MESInterface.h"
#include "AviHandler.h"
#include "WorkDlg.h"
#include "LoadCell.h"

CSequenceMain g_objSequenceMain;

///////////////////////////////////////////////////////////////////////////////

CSequenceMain::CSequenceMain()
{
	for (int i = 0; i < AXIS_COUNT; i++) m_pStatus[i] = g_objAJinAXL.Get_pStatus(i);
	for (int i = 0; i < AXIS_COUNT; i++) m_pParam[i] = g_objAJinAXL.Get_pParam(i);

	m_pDX00 = g_objAJinAXL.Get_pDX00();
	m_pDX01 = g_objAJinAXL.Get_pDX01();
	m_pDX02 = g_objAJinAXL.Get_pDX02();
	m_pDX03 = g_objAJinAXL.Get_pDX03();
	m_pDX04 = g_objAJinAXL.Get_pDX04();
	m_pDX05 = g_objAJinAXL.Get_pDX05();
	m_pDX06 = g_objAJinAXL.Get_pDX06();
	m_pDX07 = g_objAJinAXL.Get_pDX07();
	m_pDX08 = g_objAJinAXL.Get_pDX08();
	m_pDX09 = g_objAJinAXL.Get_pDX09();
	m_pDX10 = g_objAJinAXL.Get_pDX10();
	m_pDX11 = g_objAJinAXL.Get_pDX11();
	m_pDX12 = g_objAJinAXL.Get_pDX12();
	m_pDX13 = g_objAJinAXL.Get_pDX13();

	m_pDY00 = g_objAJinAXL.Get_pDY00();
	m_pDY01 = g_objAJinAXL.Get_pDY01();
	m_pDY02 = g_objAJinAXL.Get_pDY02();
	m_pDY03 = g_objAJinAXL.Get_pDY03();
	m_pDY04 = g_objAJinAXL.Get_pDY04();
	m_pDY05 = g_objAJinAXL.Get_pDY05();
	m_pDY06 = g_objAJinAXL.Get_pDY06();
	m_pDY07 = g_objAJinAXL.Get_pDY07();
	m_pDY08 = g_objAJinAXL.Get_pDY08();
	m_pDY09 = g_objAJinAXL.Get_pDY09();
	m_pDY10 = g_objAJinAXL.Get_pDY10();
	m_pDY11 = g_objAJinAXL.Get_pDY11();
	m_pDY12 = g_objAJinAXL.Get_pDY12();
	m_pDY13 = g_objAJinAXL.Get_pDY13();

	m_pEquipData = g_objDataManager.Get_pEquipData();
	m_pMoveData = g_objDataManager.Get_pMoveData();

	m_strLog = "";

	m_bThreadMainRun = FALSE;
	m_pThreadMainRun = NULL;

#ifndef AJIN_BOARD_USE
	m_pDX00->iLoadPort1LowCheck = FALSE;
	m_bLoadPortTrayExist = FALSE;
#endif
	 gData.bInspectCmThisLotVSkip = FALSE;
	 gData.nInspectCmCheckTime = 0;

	 gLot.nAlmCnt[0] = gLot.nAlmCnt[1] = 0;
	 gLot.dwStopTime[0] = gLot.dwStopTime[1] = 0;
	 gLot.dwFirstUnload[0] = gLot.dwFirstUnload[1] = 0;

	Reset_MainRunCase();
}

CSequenceMain::~CSequenceMain()
{
	if (m_pThreadMainRun) {
		m_bThreadMainRun = FALSE;
		WaitForSingleObject(m_pThreadMainRun->m_hThread, INFINITE);
	}
}

void CSequenceMain::Reset_MainRunCase()
{
	m_nTrayPickCase		= 0;		//  1. (Error : 3100)
	m_nLoadStage1Case	= 0;		//  2. (Error : 3200)
	m_nLoadStage2Case	= 50;		//  3. (Error : 3300)
	m_nLoadPickCase		= 0;		//  4. (Error : 3400)
	m_nMainIndexCase	= 0;		//  5. (Error : 3500)
	m_nVisionCmCase		= 0;		//  6. (Error : 3600)
	m_nVisionCapCase	= 0;		//  7. (Error : 3700)
	m_nCapStage1Case	= 0;		//  8. (Error : 3800)
	m_nCapStage2Case	= 50;		//  9. (Error : 3900)
	m_nCapPickCase		= 0;		// 10. (Error : 4000)
	m_nCapBufferCase	= 0;		// 11. (Error : 4100)
	m_nAssyPickCase		= 0;		// 12. (Error : 4200)
	m_nTransStageCase	= 0;		// 13. (Error : 4300)
	m_nUnloadPickCase	= 0;		// 14. (Error : 4400)
	m_nUnloadStage1Case	= 0;		// 15. (Error : 4500)	
	m_nUnloadStage2Case	= 50;		// 16. (Error : 4600) 
}

int *CSequenceMain::Get_pMainRunCase()
{
	static int nCase[16] = { 0 };

	nCase[0]  = m_nTrayPickCase;		//  1. (Error : 3100)
	nCase[1]  = m_nLoadStage1Case;		//  2. (Error : 3200)
	nCase[2]  = m_nLoadStage2Case;		//  3. (Error : 3300)
	nCase[3]  = m_nLoadPickCase;		//  4. (Error : 3400)
	nCase[4]  = m_nMainIndexCase;		//  5. (Error : 3500)
	nCase[5]  = m_nVisionCmCase;		//  6. (Error : 3600)
	nCase[6]  = m_nVisionCapCase;		//  7. (Error : 3700)
	nCase[7]  = m_nCapStage1Case;		//  8. (Error : 3800)
	nCase[8]  = m_nCapStage2Case;		//  9. (Error : 3900)
	nCase[9]  = m_nCapPickCase;			// 10. (Error : 4000)
	nCase[10] = m_nCapBufferCase;		// 11. (Error : 4100)
	nCase[11] = m_nAssyPickCase;		// 12. (Error : 4200)
	nCase[12] = m_nTransStageCase;		// 13. (Error : 4300)
	nCase[13] = m_nUnloadPickCase;		// 14. (Error : 4400)
	nCase[14] = m_nUnloadStage1Case;	// 15. (Error : 4500)
	nCase[15] = m_nUnloadStage2Case;	// 16. (Error : 4600)

	return nCase;
}

int CSequenceMain::Get_MainRunCase(int nRun)
{
	if		(nRun == AUTO_TRAY_PICKER)		return m_nTrayPickCase;		//  1. (Error : 3100)
	else if (nRun == AUTO_LOAD_STAGE1)		return m_nLoadStage1Case;	//  2. (Error : 3200)
	else if (nRun == AUTO_LOAD_STAGE2)		return m_nLoadStage2Case;	//  3. (Error : 3300)
	else if (nRun == AUTO_LOAD_PICKER)		return m_nLoadPickCase;		//  4. (Error : 3400)
	else if (nRun == AUTO_MAIN_INDEX)		return m_nMainIndexCase;	//  5. (Error : 3500)
	else if (nRun == AUTO_VISION_CM)		return m_nVisionCmCase;		//  6. (Error : 3600)
	else if (nRun == AUTO_VISION_CAP)		return m_nVisionCapCase;	//  7. (Error : 3700)
	else if (nRun == AUTO_CAP_STAGE1)		return m_nCapStage1Case;	//  8. (Error : 3800)
	else if (nRun == AUTO_CAP_STAGE2)		return m_nCapStage2Case;	//  9. (Error : 3900)
	else if (nRun == AUTO_CAP_PICKER)		return m_nCapPickCase;		// 10. (Error : 4000)
	else if (nRun == AUTO_CAP_BUFFER)		return m_nCapBufferCase;	// 11. (Error : 4100)
	else if (nRun == AUTO_ASSY_PICKER)		return m_nAssyPickCase;		// 12. (Error : 4200)
	else if (nRun == AUTO_TRANS_STAGE)		return m_nTransStageCase;	// 13. (Error : 4300)
	else if (nRun == AUTO_UNLOAD_PICKER)	return m_nUnloadPickCase;	// 14. (Error : 4400)
	else if (nRun == AUTO_UNLOAD_STAGE1)	return m_nUnloadStage1Case;	// 15. (Error : 4500)
	else if (nRun == AUTO_UNLOAD_STAGE2)	return m_nUnloadStage2Case;	// 16. (Error : 4600)

	return 0;
}

void CSequenceMain::Set_MainRunCase(int nRun, int nCase)
{
	if		(nRun == AUTO_TRAY_PICKER)		m_nTrayPickCase		= nCase;	//  1. (Error : 3100)
	else if (nRun == AUTO_LOAD_STAGE1)		m_nLoadStage1Case	= nCase;	//  2. (Error : 3200)
	else if (nRun == AUTO_LOAD_STAGE2)		m_nLoadStage2Case	= nCase;	//  3. (Error : 3300)
	else if (nRun == AUTO_LOAD_PICKER)		m_nLoadPickCase		= nCase;	//  4. (Error : 3400)
	else if (nRun == AUTO_MAIN_INDEX)		m_nMainIndexCase	= nCase;	//  5. (Error : 3500)
	else if (nRun == AUTO_VISION_CM)		m_nVisionCmCase		= nCase;	//  6. (Error : 3600)
	else if (nRun == AUTO_VISION_CAP)		m_nVisionCapCase	= nCase;	//  7. (Error : 3700)
	else if (nRun == AUTO_CAP_STAGE1)		m_nCapStage1Case	= nCase;	//  8. (Error : 3800)
	else if (nRun == AUTO_CAP_STAGE2)		m_nCapStage2Case	= nCase;	//  9. (Error : 3900)
	else if (nRun == AUTO_CAP_PICKER)		m_nCapPickCase		= nCase;	// 10. (Error : 4000)
	else if (nRun == AUTO_CAP_BUFFER)		m_nCapBufferCase	= nCase;	// 11. (Error : 4100)
	else if (nRun == AUTO_ASSY_PICKER)		m_nAssyPickCase		= nCase;	// 12. (Error : 4200)
	else if (nRun == AUTO_TRANS_STAGE)		m_nTransStageCase	= nCase;	// 13. (Error : 4300)
	else if (nRun == AUTO_UNLOAD_PICKER)	m_nUnloadPickCase	= nCase;	// 14. (Error : 4400)
	else if (nRun == AUTO_UNLOAD_STAGE1)	m_nUnloadStage1Case	= nCase;	// 15. (Error : 4500)
	else if (nRun == AUTO_UNLOAD_STAGE2)	m_nUnloadStage2Case	= nCase;	// 16. (Error : 4600)
}

void CSequenceMain::Set_MainRunLoop(int nRun, int nLoop)
{
	if		(nRun == AUTO_TRAY_PICKER)		m_tTrayPickLoop.Set_LoopTime(nLoop);		//  1. (Error : 3100)
	else if (nRun == AUTO_LOAD_STAGE1)		m_tLoadStage1Loop.Set_LoopTime(nLoop);		//  2. (Error : 3200)
	else if (nRun == AUTO_LOAD_STAGE2)		m_tLoadStage2Loop.Set_LoopTime(nLoop);		//  3. (Error : 3300)
	else if (nRun == AUTO_LOAD_PICKER)		m_tLoadPickLoop.Set_LoopTime(nLoop);		//  4. (Error : 3400)
	else if (nRun == AUTO_MAIN_INDEX)		m_tMainIndexLoop.Set_LoopTime(nLoop);		//  5. (Error : 3500)
	else if (nRun == AUTO_VISION_CM)		m_tVisionCmLoop.Set_LoopTime(nLoop);		//  6. (Error : 3600)
	else if (nRun == AUTO_VISION_CAP)		m_tVisionCapLoop.Set_LoopTime(nLoop);		//  7. (Error : 3700)
	else if (nRun == AUTO_CAP_STAGE1)		m_tCapStage1Loop.Set_LoopTime(nLoop);		//  8. (Error : 3800)
	else if (nRun == AUTO_CAP_STAGE2)		m_tCapStage2Loop.Set_LoopTime(nLoop);		//  9. (Error : 3900)
	else if (nRun == AUTO_CAP_PICKER)		m_tCapPickLoop.Set_LoopTime(nLoop);			// 10. (Error : 4000)
	else if (nRun == AUTO_CAP_BUFFER)		m_tCapBufferLoop.Set_LoopTime(nLoop);		// 11. (Error : 4100)
	else if (nRun == AUTO_ASSY_PICKER)		m_tAssyPickLoop.Set_LoopTime(nLoop);		// 12. (Error : 4200)
	else if (nRun == AUTO_TRANS_STAGE)		m_tTransStageLoop.Set_LoopTime(nLoop);		// 13. (Error : 4300)
	else if (nRun == AUTO_UNLOAD_PICKER)	m_tUnloadPickLoop.Set_LoopTime(nLoop);		// 14. (Error : 4400)
	else if (nRun == AUTO_UNLOAD_STAGE1)	m_tUnloadStage1Loop.Set_LoopTime(nLoop);	// 15. (Error : 4500)
	else if (nRun == AUTO_UNLOAD_STAGE2)	m_tUnloadStage2Loop.Set_LoopTime(nLoop);	// 16. (Error : 4600)
}

BOOL CSequenceMain::Get_IsAutoRun()
{
	int *pCase = Get_pMainRunCase();
	for (int i = 0; i < AUTO_COUNT; i++) {
		if (i == AUTO_TRAY_PICKER && m_nTrayPickCase == 1) continue;	// temp
		if (i == AUTO_LOAD_STAGE1 && m_nLoadStage1Case == 50) continue;
		if (i == AUTO_LOAD_STAGE2 && m_nLoadStage2Case == 50) continue;
		if (i == AUTO_CAP_STAGE1 && (m_nCapStage1Case == 15 || m_nCapStage1Case == 20  || m_nCapStage1Case == 50)) continue;
		if (i == AUTO_CAP_STAGE2 && (m_nCapStage2Case == 15 || m_nCapStage2Case == 20  || m_nCapStage2Case == 50)) continue;
		if (i == AUTO_CAP_PICKER && m_nCapPickCase == 10) continue;
		if (i == AUTO_CAP_BUFFER && m_nCapBufferCase == 10) continue;
		if (i == AUTO_ASSY_PICKER && m_nAssyPickCase == 1) continue;
		if (i == AUTO_UNLOAD_STAGE1 && (m_nUnloadStage1Case == 20 || m_nUnloadStage1Case == 50)) continue;
		if (i == AUTO_UNLOAD_STAGE2 && (m_nUnloadStage2Case == 20 || m_nUnloadStage2Case == 50)) continue;
		if (*(pCase + i) > 0) return TRUE;
	}

	// Temp
	if (!Check_LoadPickerEmpty()) return TRUE;
	if (!Check_IndexEmpty(-1)) return TRUE;
	if (!Check_TransStageEmpty()) return TRUE;
	if (!Check_UnloadPickerEmpty()) return TRUE;

	return FALSE;
}

void CSequenceMain::Begin_MainRunThread()
{
	Run_Replay();

	if (m_nTrayPickCase == 0)	m_nTrayPickCase = 1;

	if (m_nLoadStage1Case == 0)	m_nLoadStage1Case = 1;
	if (m_nLoadStage2Case == 0)	m_nLoadStage2Case = 1;

	if (m_nCapStage1Case == 0)	m_nCapStage1Case = 1;
	if (m_nCapStage2Case == 0)	m_nCapStage2Case = 1;

	if (m_nUnloadStage1Case == 0)	m_nUnloadStage1Case = 1;
	if (m_nUnloadStage2Case == 0)	m_nUnloadStage2Case = 1;

	g_objInspector.Set_StatusUpdate(1);
	g_objAviHandler.Set_StatusUpdate(1);
	
	if (m_pThreadMainRun) End_MainRunThread();
	m_bThreadMainRun = TRUE;
	m_pThreadMainRun = AfxBeginThread(Thread_MainRun, NULL);
}

void CSequenceMain::End_MainRunThread()
{
	if (m_pThreadMainRun) {
		m_bThreadMainRun = FALSE;
		WaitForSingleObject(m_pThreadMainRun->m_hThread, INFINITE);
	}
}

void CSequenceMain::Run_Replay()
{
	for (int i = 0; i < AUTO_COUNT; i++) Set_MainRunLoop(i, 5000);

	if (m_nLoadStage1Case == 17) m_tLoadStage1Loop.Set_LoopTime(30000);
	if (m_nLoadStage2Case == 17) m_tLoadStage2Loop.Set_LoopTime(30000);
	if (m_nVisionCmCase  == 10)  m_tVisionCmLoop.Set_LoopTime(30000);
	if (m_nVisionCapCase == 10)  m_tVisionCapLoop.Set_LoopTime(30000);

	// Cap 방향검사 Retry (2024.02.26)
	if (m_nAssyPickCase == 10 && m_nVisionCapCase == 5 && gData.bCapDirRetry) m_nVisionCapCase = 0;
}

BOOL CSequenceMain::Check_Mode()
{
	if(theApp.Get_MainMode() == MODE_MANUAL
		|| theApp.Get_MainMode() == MODE_SETUP
		|| theApp.Get_MainMode() == MODE_PARAM
		|| theApp.Get_MainMode() == MODE_PROHIBIT
		|| theApp.Get_MainMode() == MODE_INITIAL) return FALSE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Main Thread Function 
UINT CSequenceMain::Thread_MainRun(LPVOID lpVoid)
{
	while (g_objSequenceMain.m_bThreadMainRun) {
		if (!g_objSequenceMain.Check_Mode()) break;
		if (!g_objCommon.Check_MainEmgAir()) break;
		if (!g_objCommon.Check_MainDoor(TRUE)) break;
		//if (!g_objCommon.Check_PortArea(TRUE)) break;
		if (!g_objCommon.Check_TrayFull()) break;
		if (!g_objCommon.Check_ServoOn()) break;
		if (!g_objCommon.Check_DirveAlarm()) break;
		if (!g_objCommon.Check_EndLimit()) break;
		if (!g_objCommon.Check_HomeDone()) break;

		if(!g_objSequenceMain.Run_Simulation()) break;
		
		if (!g_objSequenceMain.TrayPicker_Run()) break;		//  1. (Error : 3100)
		if (!g_objSequenceMain.LoadStage1_Run()) break;		//  2. (Error : 3200)
		if (!g_objSequenceMain.LoadStage2_Run()) break;		//  3. (Error : 3300)
		if (!g_objSequenceMain.LoadPicker_Run()) break;		//  4. (Error : 3400)
		if (!g_objSequenceMain.MainIndex_Run()) break;		//  5. (Error : 3500)
		if (!g_objSequenceMain.VisionCm_Run()) break;		//  6. (Error : 3600)
		if (!g_objSequenceMain.VisionCap_Run()) break;		//  7. (Error : 3700)
		if (!g_objSequenceMain.CapStage1_Run()) break;		//  8. (Error : 3800)
		if (!g_objSequenceMain.CapStage2_Run()) break;		//  9. (Error : 3900)
		if (!g_objSequenceMain.CapPicker_Run()) break;		// 10. (Error : 4000)
		if (!g_objSequenceMain.CapBuffer_Run()) break;		// 11. (Error : 4100)
		if (!g_objSequenceMain.AssyPicker_Run()) break;		// 12. (Error : 4200)
		if (!g_objSequenceMain.TransStage_Run()) break;		// 13. (Error : 4300)
		if (!g_objSequenceMain.UnloadPicker_Run()) break;	// 14. (Error : 4400)
		if (!g_objSequenceMain.UnloadStage1_Run()) break;	// 15. (Error : 4500)
		if (!g_objSequenceMain.UnloadStage2_Run()) break;	// 16. (Error : 4600)

		

		if (g_objSequenceMain.LotEnd_Run()) break;

		if (gData.bLotEndBeep) {
			if (g_objSequenceMain.m_pThreadBeep == NULL) {
				g_objSequenceMain.m_pThreadBeep = AfxBeginThread(Thread_Beep, (LPVOID)(2000));
				gData.bLotEndBeep = FALSE;
			}
		}
		
		Sleep(5);
	}

	g_objInspector.Set_StatusUpdate(0);
	g_objAviHandler.Set_StatusUpdate(0);
	
	g_objSequenceMain.m_bThreadMainRun = FALSE;
	g_objSequenceMain.m_pThreadMainRun = NULL;

	return 0;
}

UINT CSequenceMain::Thread_Beep(LPVOID lpVoid)
{
	g_objSequenceMain.Beep_Post(0, (int)lpVoid);
	g_objSequenceMain.m_pThreadBeep = NULL;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CSequenceMain::LotEnd_Run()
{
//	if (!m_bLoadLotEnd) return FALSE;
	if (!m_bUnloadLotEnd) return FALSE;

	if (Get_IsAutoRun()) return FALSE;

	int nPNo = gData.nULPNo - 1;	// 맨마지막 공정인 Good Tray Port No 활용.
	if (nPNo < 0) nPNo = 0;
//	Job_LotEnd(gData.nULPNo);
// 	Beep_Post(1000);
	if (gData.bCycleStop) g_dlgWork.PostMessage(UM_RESET_CYCLE_STOP, NULL, NULL);

	gMes.nLotStatus = 0;
	gData.bFirstLotStart = FALSE;

	gData.bLoadPort3Wait = FALSE;
	gData.bCapPort1Wait = FALSE;
	gData.bCapPort2Wait = FALSE;
	gData.bUnloadPort1Wait = FALSE;
	gData.bUnloadPort2Wait = FALSE;
	gData.bLoadLampOn[0] = FALSE;
	gData.bLoadLampOn[1] = FALSE;

	CCME8000Dlg *pMainDlg = (CCME8000Dlg*)AfxGetApp()->GetMainWnd();
	pMainDlg->Set_LampFlicker_Load1(FALSE);
	pMainDlg->Set_LampFlicker_Load2(FALSE);
	pMainDlg->Set_LampFlicker_Load3(FALSE);
	pMainDlg->Set_LampFlicker_Cap1(FALSE);
	pMainDlg->Set_LampFlicker_Cap2(FALSE);
	pMainDlg->Set_LampFlicker_Unload1(FALSE);
	pMainDlg->Set_LampFlicker_Unload2(FALSE);

	CString strMsg;

// 	int nSum = gLot.nGoodCount[nPNo] + gLot.nNgCount[nPNo];
// 	double dRate = (nSum == 0 ? 0.0 : gLot.nGoodCount[nPNo] * 100.0 / nSum);
	strMsg.Format("Lot End ==> ID(%s)", gLot.sLotID[nPNo]);
	g_objLogFile.Save_HandlerLog(strMsg);

	strMsg.Format("Lot End.\n\n");

	gData.nPNoUnloadPort = 0;
	if (m_nAssyPickCase == 1) m_nAssyPickCase = 0;
	Set_ClearRunData(1);

	g_dlgWork.m_stcLotId[0].SetWindowText("");
	g_dlgWork.m_stcCmCount[0].SetWindowText("0");
	g_dlgWork.m_stcTrayCount[0].SetWindowText("0");
	
	g_dlgWork.m_stcLotId[1].SetWindowText("");
	g_dlgWork.m_stcCmCount[1].SetWindowText("0");
	g_dlgWork.m_stcTrayCount[1].SetWindowText("0");
	
	g_objCommon.Show_Alarm(strMsg, STATE_LOTEND);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//Type 0 : 랏 엔드 되도 초기화 되면 안되는 정보들 
//Type 1 : 랏 엔드시 초기화 되야 되는 정보들 
void CSequenceMain::Set_ClearRunData(int nType)
{
	memset(gData.bScanDone, 0x00, sizeof(BOOL) * 2);
	memset(gData.nCmInspectInfo, 0x00, sizeof(int) * 2 * 50 * 12);
	memset(gData.nCapInspectInfo, 0x00, sizeof(int) * 2 * 50 * 28);

	memset(gData.IndexDone, 0x00, sizeof(int) * 3);
	memset(gData.InfoLoadTray, 0x00, sizeof(int) * LT_Y * LT_X);
	memset(gData.InfoShipTray, 0x00, sizeof(int) * ST_Y * ST_X);

	if (nType == 0) memset(gData.InfoCapTray, 0x00, sizeof(int) * CT_Y * CT_X);
	g_dlgWork.PostMessage(UM_UPDATE_TRAY_INFO, 0, NULL);

	memset(gData.InfoIndex, 0x00, sizeof(int) * 3 * PICK);
	if (nType == 0) memset(gData.InfoCapBuffer, 0x00, sizeof(int) * PICK);
	memset(gData.InfoTransStage, 0x00, sizeof(int) * PICK);
	
	memset(gData.InfoLoadPick, 0x00, sizeof(int) * PICK);
	if (nType == 0) memset(gData.InfoCapPick, 0x00, sizeof(int) * PICK);
	if (nType == 0) memset(gData.InfoAssyPick, 0x00, sizeof(int) * PICK);
	memset(gData.InfoUnloadPick, 0x00, sizeof(int) * PICK);

	if (nType == 0) memset(gData.nCapTNoCapPicker, 0x00, sizeof(int) * PICK);
	if (nType == 0) memset(gData.nCapTNoCapBuffer, 0x00, sizeof(int) * PICK);
	if (nType == 0) memset(gData.nCapTNoAssyPicker, 0x00, sizeof(int) * PICK);

	if (nType == 0) memset(gData.nCapNoCapPicker, 0x00, sizeof(int) * PICK);
	if (nType == 0) memset(gData.nCapNoCapBuffer, 0x00, sizeof(int) * PICK);
	if (nType == 0) memset(gData.nCapNoAssyPicker, 0x00, sizeof(int) * PICK);

	if (nType == 0) gData.nInsCmScanLineCnt = 0;
	if (nType == 0) gData.nInsCmScanCntVolatile = m_pEquipData->nInspectCmScanTimes;
	if (nType == 0) gData.nInspectCmLotCount = 0;
	if (nType == 0) gData.dwRunTimeNow = 0;
	if (nType == 0) gData.dwRunTimeAccumulated = 0;	
	if (nType == 0) gData.bReload[0] = FALSE;

	gData.nPNoLoadPick = gData.nPNoUnloadPick = 0;
	gData.nPNoTransStage = gData.nPNoUnloadTray = 0;
	gData.nPNoUnloadPort = 0;
	gData.nLPNo = gData.nULPNo = 0;
	memset(gData.nPNoLoadTray, 0x00, sizeof(int) * 2);
	memset(gData.nPNoIndex, 0x00, sizeof(int) * 3);

	gData.nTNoTrayPick = gData.nTNoLoadPort = gData.nTNoUnloadTray = 0;
	memset(gData.nTNoLoadTray, 0x00, sizeof(int) * 2);
	memset(gData.nTNoLoadPick, 0x00, sizeof(int) * PICK);
	memset(gData.nTNoIndex, 0x00, sizeof(int) * 3 * PICK);
	memset(gData.nTNoTransStage, 0x00, sizeof(int) * PICK);
	memset(gData.nTNoUnloadPick, 0x00, sizeof(int) * PICK);

	memset(gData.nCNoLoadPick, 0x00, sizeof(int) * PICK);
	memset(gData.nCNoIndex, 0x00, sizeof(int) * 3 * PICK);
	memset(gData.nCNoTransStage, 0x00, sizeof(int) * PICK);
	memset(gData.nCNoUnloadPick, 0x00, sizeof(int) * PICK);

	memset(gLot.dAverageCycle, 0x00, sizeof(double) * 15 * 2 * 20);

	// Process 변수 초기화
	m_bLoadLotEnd = FALSE;
	m_bUnloadLotEnd = FALSE;

	//AHN
	m_dwFirstLoad = 0;
	m_dwLastUnLoad = 0;

	gLot.nErrorCount = 0;
	gLot.dwRunTime = gLot.dwErrorTime =0;
	gLot.dwStopTime[0] = gLot.dwStopTime[1] = 0;
	gLot.bLotEndComplete[0] = FALSE;
	gLot.bLotEndComplete[1] = FALSE;
	gLot.nCapFailCount[0] = gLot.nCapFailCount[1] = 0;
	gData.bFirstLotStart = FALSE;

	gData.nLoadTrayCount[0] = 0; gData.nLoadTrayCount[1] = 0;

	gData.nTrayUseCount[0] = gData.nTrayUseCount[1] = 0;
	gData.nCmUseCount[0] = gData.nCmUseCount[1] = 0;

	gData.bAviTrayLoad = FALSE;
	gData.nLastTrayNo[0] = gData.nLastTrayNo[1] = 0;

	gData.bLoadPort3Wait = FALSE;
	gData.bCapPort1Wait = FALSE;
	gData.bCapPort2Wait = FALSE;
	gData.bUnloadPort1Wait = FALSE;
	gData.bUnloadPort2Wait = FALSE;
	gData.bUnloadTrayLotEnd[0] = FALSE;
	gData.bUnloadTrayLotEnd[1] = FALSE;

	gData.bLoadLampOn[0] = FALSE;
	gData.bLoadLampOn[1] = FALSE;

	m_bLoadPortTrayExist = FALSE;
	
	gLot.nGoodCount[0] = gLot.nGoodCount[1] = gLot.nNgCount[0] = gLot.nNgCount[1] = 0;
	g_dlgWork.PostMessage(UM_VISION_RESULT, NULL, NULL);

	for (int p = 0; p <  2; p++) for (int i = 0; i < 50; i++) for (int j = 0; j < 12; j++) gMes.sBarID[p][i][j] = gMes.sJudge[p][i][j] = gMes.sNGCode[p][i][j] = "";

	for (int i = 0; i < 4; i++) { 
		for (int j = 0; j < PICK; j++) {
			gData.dIndexOffsetX[i][j] = gData.dIndexOffsetY[i][j] = 0.0;
			gData.dIndexCmSizeX[i][j] = gData.dIndexCmSizeY[i][j] = 0.0;
		}
	}
	for (int i = 0; i < PICK; i++) { 
		gData.dAssyPickOffsetX[i] = gData.dAssyPickOffsetY[i] = 0.0;
		gData.dAssyPickCapSizeX[i] = gData.dAssyPickCapSizeY[i] = 0.0;
		gData.dAlignDiffX[i] = gData.dAlignDiffY[i] = 0.0;
	}

	m_bLotLoadEnable[0] = FALSE;
	m_bLotLoadEnable[1] = FALSE;

	g_dlgWork.Enable_UserInput(1, TRUE);
	g_dlgWork.Enable_UserInput(2, TRUE);

	CCME8000Dlg *pMainDlg = (CCME8000Dlg*)AfxGetApp()->GetMainWnd();
	pMainDlg->Set_LampFlicker_Load1(FALSE);
	pMainDlg->Set_LampFlicker_Load2(FALSE);
	pMainDlg->Set_LampFlicker_Load3(FALSE);
	pMainDlg->Set_LampFlicker_Cap1(FALSE);
	pMainDlg->Set_LampFlicker_Cap2(FALSE);
	pMainDlg->Set_LampFlicker_Unload1(FALSE);
	pMainDlg->Set_LampFlicker_Unload2(FALSE);

	g_dlgWork.PostMessage(UM_UPDATE_MODEL, NULL, NULL);
}

///////////////////////////////////////////////////////////////////////////////
void CSequenceMain::Init_LoadTray(int nPNo, int nSNo)
{
	memset(gData.InfoLoadTray, 0x00, sizeof(int) * LT_Y * LT_X);	// Empty

	int nPx = (nPNo < 1 ? 0 : nPNo - 1);
	int nCmCnt = gData.nCmCntLoadStage[nSNo-1];

	if (!m_pEquipData->bUseInlineMode) {
		int nUseTray = gData.nTrayUseCount[nPx];
		int nUseCm   = gData.nCmUseCount[nPx];
		int nTrayCnt = gData.nLoadTrayCount[nPx];
		int nCmMax   = gData.nCmMaxCount;

		nCmCnt = (nTrayCnt < nUseTray ? nCmMax : (nUseCm - (nTrayCnt - 1) * nCmMax));
	}

 	for (int i = 0; i < nCmCnt; i++) gData.InfoLoadTray[i / LT_X][i % LT_X] = 9;	// 가로
//	for (int i = 0; i < nCmCnt; i++) gData.InfoLoadTray[i % LT_Y][i / LT_Y] = 9;	// 세로

	g_dlgWork.PostMessage(UM_UPDATE_TRAY_INFO, 1, nSNo-1);
}

void CSequenceMain::Init_CapTray()
{
	for (int y = 0; y < CT_Y; y++) {
		for (int x = 0; x < CT_X; x++) {
			gData.InfoCapTray[y][x] = 9;		
		}
	}

	g_dlgWork.PostMessage(UM_UPDATE_TRAY_INFO, 2, NULL);
}

void CSequenceMain::Init_UnloadTray()
{
	memset(gData.InfoShipTray, 0x00, sizeof(int) * ST_Y * ST_X);

	g_dlgWork.PostMessage(UM_UPDATE_TRAY_INFO, 3, NULL);
}

///////////////////////////////////////////////////////////////////////////////

BOOL CSequenceMain::Check_LoadTrayEmpty(int nNo)
{
	for (int y = 0; y < LT_Y; y++) {
		for (int x = 0; x < LT_X; x++) {
			if (gData.InfoLoadTray[y][x] > 0) return FALSE;
		}
	}
	return TRUE;
}

BOOL CSequenceMain::Check_LoadTrayLineEmpty(int nLine)
{
	for (int x = 0; x < LT_X; x++) {
		if (gData.InfoLoadTray[nLine-1][x] > 0) return FALSE;
	}
	return TRUE;
}

BOOL CSequenceMain::Check_CapTrayEmpty()
{
	for (int y = 0; y < CT_Y; y++) {
		for (int x = 0; x < CT_X; x++) {
			if (gData.InfoCapTray[y][x] > 0) return FALSE;
		}
	}
	return TRUE;
}

BOOL CSequenceMain::Check_CapTrayLineEmpty(int nLine)
{
	for (int x = 0; x < CT_X; x++) {
		if (gData.InfoCapTray[nLine-1][x] > 0) return FALSE;
	}
	return TRUE;
}

BOOL CSequenceMain::Check_UnloadTrayEmpty()
{
	for (int y = 0; y < gData.STY; y++) {
		for (int x = 0; x < ST_X; x++) {
			if (gData.InfoShipTray[y][x] > 0) return FALSE;
		}
	}
	return TRUE;
}

BOOL CSequenceMain::Check_LoadPickerEmpty()
{
	for (int i = 0; i < PICK; i++) { 
		if (gData.InfoLoadPick[i] > 0) return FALSE; 
	}
	return TRUE;
}

BOOL CSequenceMain::Check_CapPickerEmpty()
{
	for (int i = 0; i < PICK; i++) { 
		if (gData.InfoCapPick[i] > 0) return FALSE; 
	}
	return TRUE;
}

BOOL CSequenceMain::Check_AssyPickerEmpty()
{
	for (int i = 0; i < PICK; i++) { 
		if (gData.InfoAssyPick[i] > 0) return FALSE; 
	}
	return TRUE;
}

BOOL CSequenceMain::Check_UnloadPickerEmpty()
{
	for (int i = 0; i < PICK; i++) { 
		if (gData.InfoUnloadPick[i] > 0) return FALSE; 
	}
	return TRUE;
}

BOOL CSequenceMain::Check_CapBufferEmpty()
{
	for (int i = 0; i < PICK; i++) {
		if (gData.InfoCapBuffer[i] > 0) return FALSE;
	}
	return TRUE;
}

BOOL CSequenceMain::Check_TransStageEmpty()
{
	for (int i = 0; i < PICK; i++) {
		if (gData.InfoTransStage[i] > 0) return FALSE;
	}
	return TRUE;
}

BOOL CSequenceMain::Check_LoadPickerFull()
{
	for (int i = 0; i < PICK; i++) { 
		if (gData.InfoLoadPick[i] == 0) return FALSE; 
	}
	return TRUE;
}

BOOL CSequenceMain::Check_CapPickerFull()
{
	for (int i = 0; i < PICK; i++) { 
		if (gData.InfoCapPick[i] == 0) return FALSE; 
	}
	return TRUE;
}

BOOL CSequenceMain::Check_CapBufferFull()
{
	if (gData.InfoCapBuffer[PICK-1] > 0) return TRUE;
	return FALSE;
}

BOOL CSequenceMain::Check_UnloadTrayFull()
{
//	if (gData.InfoShipTray[gData.STY-1][ST_X-1] > 0) return TRUE;
	for(int y = 0; y < gData.STY; y++) {
		for (int x = 0; x < ST_X; x++) {
			if (gData.InfoShipTray[y][x] == 0) return FALSE;
		}
	}
	return TRUE;
}

BOOL CSequenceMain::Check_IndexEmpty(int nPos)
{
	// nPos (0:Load, 1:Assembly, 2:Unload, -1:All)
	int nS = (nPos == -1 ? 0 : nPos);
	int nE = (nPos == -1 ? 2 : nPos);

	for (int i = nS; i < nE + 1; i++) 
	{
		for (int j = 0; j < PICK; j++) 
		{
			if (gData.InfoIndex[i][j] > 0) return FALSE;
		}
	}
	return TRUE;
}

BOOL CSequenceMain::Check_IndexModule()
{
	for (int j = 0; j < PICK; j++) {
		if (gData.InfoIndex[0][j] > 0) return TRUE;	// Use : 9
		if (gData.InfoIndex[1][j] > 1) return TRUE; // 조립전 : 9, 조립후 : 1
	}
	return FALSE;
}

BOOL CSequenceMain::Check_CapInspAllGood()
{
	// CM Vision, Align Offset 중 하나라도 꺼져있으면 Offset 기능 사용하면 안된다.
	if (!m_pEquipData->bUseVisionAlignOffset || !m_pEquipData->bUseVisionCmAlign) {
		for (int i = 0; i < PICK; i++) { 
			gData.dAlignDiffX[i] = gData.dAlignDiffY[i] = 0.0;
			gData.dAssyPickOffsetX[i] = gData.dAssyPickOffsetY[i] = 0.0;
			gData.bCapAlignNg[i] = FALSE;
		}
		for (int i = 0; i < 4; i++) { for (int j = 0; j < PICK; j++) { gData.dIndexOffsetX[i][j] = gData.dIndexOffsetY[i][j] = 0.0; } }
		return TRUE;
	}

	// 모듈 이미지를 180도 회전해야 모듈이 제대로 보이고 인덱스 회전까지 합하면 총 270도
	// 캡 이미지를 -90도 회전해야 캡이 제대로 보인다.
	int nIndexNo = g_objCommon.Get_MainIndexPos(2);
	double dDiff_X = 0.0;
	double dDiff_Y = 0.0;
	double dTolerance = m_pEquipData->dAlignOffset;
	for (int i = 0; i < PICK; i++) { 
		dDiff_Y = gData.dIndexOffsetX[nIndexNo][i] - gData.dAssyPickOffsetX[i];
		dDiff_X = gData.dIndexOffsetY[nIndexNo][i] + gData.dAssyPickOffsetY[i];
		if ((gData.InfoIndex[1][i] > 0) && ((fabs(dDiff_X) > dTolerance) || (fabs(dDiff_Y) > dTolerance)))
		{
			if (fabs(dDiff_X) > dTolerance) gData.dAlignDiffX[i] = dDiff_X;
			else							gData.dAlignDiffX[i] = 0.0;
			if (fabs(dDiff_Y) > dTolerance) gData.dAlignDiffY[i] = dDiff_Y;
			else							gData.dAlignDiffY[i] = 0.0;
			gData.InfoAssyPick[i] = 2;
			gData.bCapAlignNg[i] = TRUE;
		} else {
			gData.dAlignDiffX[i] = 0.0;
			gData.dAlignDiffY[i] = 0.0;
			gData.bCapAlignNg[i] = FALSE;
		}
	}

	for (int i = 0; i < PICK; i++) { 
		if (gData.InfoAssyPick[i] == 2) return FALSE; 
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
int CSequenceMain::Get_CmScanCnt()
{
	int nCnt = 0;
	for (int i = 0; i < (PICK/2); i++) { if (gData.InfoIndex[0][i] > 0) nCnt++; }
	int nScanCnt = (nCnt > 2 ? 2 : (nCnt > 1 ? 1 : (nCnt > 0 ? 0 : -1)));
	return nScanCnt;
}

int CSequenceMain::Get_CapScanCnt()
{
	int nCnt = 0;
	for (int i = 0; i < PICK; i++) { if (gData.InfoAssyPick[i] > 0) nCnt++; }
	int nScanCnt = (nCnt > 2 ? 2 : (nCnt > 1 ? 1 : (nCnt > 0 ? 0 : -1)));	// 1/4, 2/5, 3/6 위치에 카메라
	return nScanCnt;
}
 
BOOL CSequenceMain::Select_LoadTrayPos(int &nTrayPosX, int &nTrayPosY)
{
	nTrayPosX = nTrayPosY = -1;
	
	if (Check_LoadTrayEmpty()) return FALSE;
	
	for (int y = 0; y < LT_Y; y++) {
		for (int x = 0; x < LT_X; x++) {
			if (gData.InfoLoadTray[y][x] > 0) { nTrayPosX = x; nTrayPosY = y; return TRUE; }
		}
	}
	if (nTrayPosX == -1 || nTrayPosY == -1) return FALSE;
	return TRUE;
}

BOOL CSequenceMain::Select_CapTrayPos(int &nTrayPosX, int &nTrayPosY)
{
	nTrayPosX = nTrayPosY = -1;
	if (Check_CapTrayEmpty()) return FALSE;

	for (int y = 0; y < CT_Y; y++) {
		for (int x = 0; x < CT_X; x++) {
			if (gData.InfoCapTray[y][x] == 9) { nTrayPosX = x; nTrayPosY = y; return TRUE; }
		}
	}
	return FALSE;
}

BOOL CSequenceMain::Select_UnloadTrayPos(int &nTrayPosX, int &nTrayPosY)
{
	nTrayPosX = nTrayPosY = -1;
	if (Check_UnloadTrayFull()) return FALSE;

	for (int y = 0; y < gData.STY; y++) {
		for (int x = 0; x < ST_X; x++) {
			if (gData.InfoShipTray[y][x] == 0) { nTrayPosX = x; nTrayPosY = y; return TRUE; }
		}
	}
	return FALSE;
}

BOOL CSequenceMain::Select_LoadPickerPos(int &nPos, int &nCnt)
{
	nPos = -1; nCnt = 0;
	for (int i = 0; i < PICK; i++) { if (gData.InfoLoadPick[i] == 0) { nPos = i; break; } }
	for (int j = nPos; j < PICK; j++) { if (gData.InfoLoadPick[j] > 0) break; nCnt++; }
	if (nPos == -1 || nCnt == 0) return FALSE;
	return TRUE;
}

BOOL CSequenceMain::Select_CapPickerPos(int &nPos, int &nCnt)
{
	nPos = -1; nCnt = 0;
	for (int i = 0; i < PICK; i++) { if (gData.InfoCapPick[i] == 0) { nPos = i; break; } }
	for (int i = nPos; i < PICK; i++) { if (gData.InfoCapPick[i] > 0) break; nCnt++; }
	if (nPos == -1 || nCnt == 0) return FALSE;
	if (!m_pEquipData->bCapPickUpMulti) nCnt = 1;
	return TRUE;
}

BOOL CSequenceMain::Select_AssyPickerPos(int &nPos, int &nCnt)
{
	nPos = -1;  nCnt = 0;
	for (int i = 0; i < PICK; i++) { if (gData.InfoAssyPick[i] == 0) { nPos = i; break; } }
	for (int j = nPos; j < PICK; j++) { if (gData.InfoAssyPick[j] > 0) break; nCnt++; }
	if (nPos == -1 || nCnt == 0) return FALSE;
	return TRUE;
}

BOOL CSequenceMain::Select_AssyPickerNgPos(int &nPos)
{
	nPos = -1;
	for (int i = 0; i < PICK; i++) { if (gData.InfoAssyPick[i] == 2) { nPos = i; break; } }
	if (nPos == -1) return FALSE;
	return TRUE;
}

BOOL CSequenceMain::Select_UnloadPickerPos(int &nPos, int &nCnt)
{
	nPos = -1; nCnt = 0;
	for (int i = 0; i < PICK; i++) { if (gData.InfoUnloadPick[i] > 0) { nPos = i; break; } }
	for (int j = nPos; j < PICK; j++) { if (gData.InfoUnloadPick[j] == 0) break; nCnt++; }
	if (nPos == -1 || nCnt == 0) return FALSE;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CSequenceMain::Check_CmAlignDone()
{
	if (!m_pEquipData->bUseVisionCmAlign) return TRUE;
	if (gData.bScanDone[0]) return TRUE;
	return TRUE;	// All Inspect Done
}

BOOL CSequenceMain::Check_CapAlignDone()
{
	if (!m_pEquipData->bUseVisionCapDir) return TRUE;
	if (gData.bScanDone[1]) return TRUE;
	return TRUE;	// All Inspect Done
}

// 한번에 2Lot 이상 들어가면 안된다.
BOOL CSequenceMain::Check_LoadTrayLoading(int nPNo)
{
	if (gData.nPNoLoadPick == nPNo) return FALSE;
	if (gData.nPNoIndex[0] == nPNo) return FALSE;
	if (gData.nPNoIndex[1] == nPNo) return FALSE;
	if (gData.nPNoIndex[2] == nPNo) return FALSE;
	if (gData.nPNoTransStage == nPNo) return FALSE;
 	if (gData.nPNoUnloadPick == nPNo) return FALSE;
// 	if (gData.nPNoUnloadTray == nPNo) return FALSE;

	return TRUE;
}

BOOL CSequenceMain::Check_LoadLotEnd(int nPNo, int nMode)
{
	// 로드가 다끝나고 앞공정에 모듈이 없으면 TRUE
	if (!gData.bCycleStop && (gData.nLoadTrayCount[nPNo-1] < gData.nTrayUseCount[nPNo-1])) return FALSE;
	if (gData.nPNoLoadTray[0] == nPNo || gData.nPNoLoadTray[1] == nPNo) return FALSE;
// 	if (!Check_AngleTrayEmpty()) return FALSE;
	if (nMode == 0 && !Check_LoadPickerEmpty()) return FALSE;

	return TRUE;
}

BOOL CSequenceMain::Check_UnloadLotEnd(int nMode)
{
//  if (!m_bLoadLotEnd) return FALSE;
	if (m_nLoadStage1Case != 60 && m_nLoadStage1Case != 50) return FALSE;
	if (m_nLoadStage2Case != 60 && m_nLoadStage2Case != 50) return FALSE;
	if (!Check_IndexEmpty(-1)) return FALSE;
	if (!Check_LoadPickerEmpty()) return FALSE;
	if (!Check_TransStageEmpty()) return FALSE;
	if (!Check_UnloadPickerEmpty()) return FALSE;

	return TRUE;
}

BOOL CSequenceMain::Get_VisionInspectUse()
{
	BOOL bUse = (m_pEquipData->bUseVisionCapDir || m_pEquipData->bUseVisionCmAlign);
	return bUse;
}

void CSequenceMain::Set_TaktLog(int nNo)
{
	// 0:Load, 1:Assembly, 2:Unload
	if (!gData.IndexDone[nNo]) return;
	gLot.dwTaktData[nNo] = GetTickCount() - gLot.dwTaktData[nNo];
	gLot.bTaktDone[nNo] = TRUE;
}

void CSequenceMain::Set_IndexEnd()
{
	int i = 0;
	int j = 0;	
	CString ErrMsg;
// 	for (i = 0; i < 2; i++) {
// 		ErrMsg.Format("1.InfoIndex%d ==> InfoIndex[0]:%d InfoIndex[1]:%d InfoIndex[2]:%d", i, gData.InfoIndex[i][0], gData.InfoIndex[i][1], gData.InfoIndex[i][2]);
// 		g_objLogFile.Save_HandlerLog(ErrMsg);
// 	}
	memmove(gData.InfoIndex[1], gData.InfoIndex[0], sizeof(int) * 2 * PICK);
	memmove(gData.nPNoIndex + 1, gData.nPNoIndex, sizeof(int) * 2);
	memmove(gData.nTNoIndex + 1, gData.nTNoIndex, sizeof(int) * 2 * PICK);
	memmove(gData.nCNoIndex[1], gData.nCNoIndex[0], sizeof(int) * 2 * PICK);

	memset(gData.InfoIndex[0], 0x00, sizeof(int) * PICK);
	gData.nPNoIndex[0] = 0;
	memset(gData.nTNoIndex[0], 0x00, sizeof(int) * PICK);
	memset(gData.nCNoIndex[0], 0x00, sizeof(int) * PICK);
	memset(gData.IndexDone, 0x00, sizeof(BOOL) * 3);

	for (i = 1; i >= 0; i--) {
		for (j = 0; j < PICK; j++) gData.sCIDIndex[i+1][j] = gData.sCIDIndex[i][j];
	}
	for (i = 0; i < PICK; i++) gData.sCIDIndex[0][i].Empty();

	if (gData.nCNoIndex[2][0] > 0) {
		int nLineNo = gData.nCNoIndex[2][0] / PICK;
		int nTNo = gData.nTNoIndex[2][0] - 1;
		gData.dwCmTactStart[nTNo][nLineNo] = GetTickCount();
	}

// 	for (i = 0; i < 2; i++) {
// 		ErrMsg.Format("1.InfoIndex%d ==> InfoIndex[0]:%d InfoIndex[1]:%d InfoIndex[2]:%d", i, gData.InfoIndex[i][0], gData.InfoIndex[i][1], gData.InfoIndex[i][2]);
// 		g_objLogFile.Save_HandlerLog(ErrMsg);
// 	}

	// Index Takt Log /////////////////////////////////////////////////////////
	gLot.dwTaktData[3] = GetTickCount() - gLot.dwTaktData[3];	// Index Takt

	CString	sLog;
	sLog.Format("[Index Table] Index[%d] LP[%d] Assy[%d] TP[%d]",
		gLot.dwTaktData[3], gLot.dwTaktData[0], gLot.dwTaktData[1], gLot.dwTaktData[2]);
	g_objLogFile.Save_HandlerLog(sLog);	// MCC로그 따로 저장해준다.

	g_dlgWork.PostMessage(UM_INDEX_TACK, (WPARAM)gLot.dwTaktData[3], NULL);

	memset(gLot.bTaktDone, 0x00, sizeof(BOOL) * 3);
	for (int i = 0; i < 4; i++) gLot.dwTaktData[i] = GetTickCount();
	///////////////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////////////////

void CSequenceMain::Job_LotStart(int nPortNo)
{
	int nLPNo = nPortNo - 1;
	if (Get_VisionInspectUse()) g_objInspector.Set_LotStart(gData.sLotID[nLPNo], nPortNo);
	gLot.bLotEndComplete[nLPNo] = FALSE;

	SYSTEMTIME time;
	GetLocalTime(&time);

	gLot.sLotID[nLPNo] = gData.sLotID[nLPNo];
	gLot.sStartTime[nLPNo].Format("%04d%02d%02d_%02d%02d%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	m_dwFirstLoad = gLot.dwLotStart[nLPNo] = GetTickCount();
	gLot.nTrayCount[nLPNo] = gData.nTrayUseCount[nLPNo];
	gLot.nCmCount[nLPNo] = gData.nCmUseCount[nLPNo];

	gLot.nCapCount[nLPNo] = 0;
 	gLot.nErrorCount = 0;
 	if (gData.bFirstLotStart == TRUE) {
		gData.bFirstLotStart = FALSE;
		gLot.dwRunTime = gLot.dwErrorTime = 0;
	}

	gLot.dwStopTime[0] = 0;
	gLot.dwStopTime[1] = 0;

	g_dlgWork.PostMessage(UM_LOT_START_END, (WPARAM)1, nPortNo);	// LotStart
}

void CSequenceMain::Job_LotEnd(int nPortNo)
{
	int nLPNo = nPortNo - 1;
	if (gLot.nCmCount[nLPNo] < 1) return;	// Error
	if (gLot.bLotEndComplete[nLPNo] == TRUE) return;

	//2주내 바코드 중복 체크 
	g_objLogFile.Save_BarcodeChkLog(gData.sCapLotID);
	g_objLogFile.Save_BarcodeChkLog(gData.sShipLotID);

	gLot.bLotEndComplete[nLPNo] = TRUE;		

	SYSTEMTIME time;
	GetLocalTime(&time);
	gLot.dwLotEnd[nLPNo] = GetTickCount();
	gLot.sEndTime[nLPNo].Format("%04d%02d%02d_%02d%02d%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

	if (Get_VisionInspectUse()) g_objInspector.Set_LotEnd(gData.sLotID[nLPNo], nPortNo);

	int nTrayCnt = gData.nLastTrayNo[nLPNo];
	int nCmCnt = gLot.nCapCount[nLPNo];


	//Start to End
	DWORD dwTime_StoE = gLot.dwLotEnd[nLPNo] - gLot.dwLotStart[nLPNo];
	gLot.dTactTime_StoE = dwTime_StoE / 1000.0 / nCmCnt;	//gLot.nCmCount[nLPNo];	// Floating-point inexact result
	m_dwLastUnLoad = gLot.dwLotEnd[nLPNo] - m_dwLastUnLoad;

	//Runtime
	DWORD dwTime_RunTime = gLot.dwLotEnd[nLPNo] - gLot.dwLotStart[nLPNo] - gLot.dwStopTime[nLPNo];
	gLot.dTactTime_RunTime = dwTime_RunTime / 1000.0 / nCmCnt;	//gLot.nCmCount[nLPNo];	// Floating-point inexact result
		
	//Unload
	DWORD dwTime_Unload = gLot.dwLotEnd[nLPNo] - gLot.dwLotStart[nLPNo] - gLot.dwStopTime[nLPNo];
	gLot.dTactTime_Unload = dwTime_Unload / 1000.0 / nCmCnt;	//gLot.nCmCount[nLPNo];	// Floating-point inexact result

	double dEff_RunTime = (double)dwTime_RunTime / (double)dwTime_StoE;
	double dEff_UnloadTime = (double)dwTime_Unload / (double)dwTime_StoE;

	m_strLog.Format("LotID,%s,Start_Time,%s,End_Time,%s,Time,%d,Tray_Count,%02d,CM_Count,%04d,Tack,%0.7lf",
		gLot.sLotID[nLPNo], gLot.sStartTime[nLPNo], gLot.sEndTime[nLPNo], dwTime_StoE, nTrayCnt/*gLot.nTrayCount[nLPNo]*/, nCmCnt/*gLot.nCmCount[nLPNo]*/, gLot.dTactTime_StoE);
	g_objLogFile.Save_HandlerLog(m_strLog);

// 	double dOne = (double)(dwTime - m_dwFirstLoad - m_dwLastUnLoad) / (gLot.nCmCount[nLPNo] - 1);
// 	m_strLog.Format("LotID(%s), Load(%0.3lf), Unload(%0.3lf), Takt as 600 EA Run(%0.6lf)", gLot.sLotID[nLPNo], (double)m_dwFirstLoad / 1000, (double)m_dwLastUnLoad / 1000, (dOne * 599 + m_dwFirstLoad + m_dwLastUnLoad) / 600000.0);
// 	g_objLogFile.Save_HandlerLog(m_strLog);
	
	m_strLog.Format("%s,%s,%s,%d,%02d,%04d,%0.7lf,%d",
		gLot.sLotID[nLPNo], gLot.sStartTime[nLPNo], gLot.sEndTime[nLPNo], dwTime_RunTime, dwTime_Unload, gLot.dTactTime_StoE, gLot.dTactTime_RunTime, gLot.dTactTime_Unload,gLot.nAlmCnt[nLPNo], gLot.dwStopTime[nLPNo], dEff_RunTime, dEff_UnloadTime, nTrayCnt/*gLot.nTrayCount[nLPNo]*/, nCmCnt/*gLot.nCmCount[nLPNo]*/, gLot.nCapFailCount[nLPNo]);
	g_objLogFile.Save_JobListLog(m_strLog, TRUE);

	g_dlgWork.PostMessage(UM_LOT_START_END, (WPARAM)2, nPortNo);	// LotEnd

	// UPH & 생산량
	//if (gLot.sLotID[nLPNo].GetLength() < 12) return;		// Lot ID

	gUph.dTaktTime = gLot.dTactTime_RunTime;

	if (time.wHour >= 7 && time.wHour < 19) gUph.nCmCount[0] += nCmCnt;	//gLot.nCmCount[nLPNo];
	else gUph.nCmCount[1] += nCmCnt;	//gLot.nCmCount[nLPNo];

	int i = (int)time.wHour;
	int j = gUph.nLotCount[i];
	if (j > 49) return;		// 1시간에 LOT 수량 MAX 50개

	gUph.dTakt[i][j] = gLot.dTactTime_RunTime;
	gUph.nLotCount[i] = j + 1;

	int nSum = nCmCnt;	//gLot.nCmCount[nLPNo]; //gLot.nGoodCount[nLPNo] + gLot.nNgCount[nLPNo];
	double dRate = 0.0;	//(nSum == 0 ? 0.0 : gLot.nGoodCount[nLPNo] * 100.0 / nSum);
	// Lot Result [날짜,Lot Start,Lot End,Term,호기,Lot ID,투입수];
	CString strMsg, strStart, strEnd, strTerm, strEquip;

	strStart.Format("%s:%s:%s", gLot.sStartTime[nLPNo].Mid(9, 2), gLot.sStartTime[nLPNo].Mid(11, 2), gLot.sStartTime[nLPNo].Mid(13, 2));
	strEnd.Format("%s:%s:%s", gLot.sEndTime[nLPNo].Mid(9, 2), gLot.sEndTime[nLPNo].Mid(11, 2), gLot.sEndTime[nLPNo].Mid(13, 2));
	int nSec = (int)(gLot.dwLotEnd[nLPNo] - gLot.dwLotStart[nLPNo]) / 1000;
	int nH = nSec / 60 / 60;
	int nM = (nSec - nH * 60 * 60) / 60;
	int nS = (nSec - nH * 60 * 60) % 60;
	strTerm.Format("%02d:%02d:%02d", nH, nM, nS);
	strEquip = m_pEquipData->sEquipName;

	strMsg.Format("%s,%s,%s,%s,%s,%d", strStart, strEnd, strTerm, strEquip, gLot.sLotID[nLPNo], nSum);
	g_objLogFile.Save_LotResult(nLPNo, strMsg);

	CCME8000Dlg *pMainDlg = (CCME8000Dlg*)AfxGetApp()->GetMainWnd();
	pMainDlg->Set_LotStateTime();
	strMsg.Format("%s,%s,%s,%0.3lf,%d,%0.3lf,%0.3lf,%0.3lf,%d,%0.2lf,%0.3lf,%0.3lf,%0.2lf",
		gLot.sLotID[nLPNo], strStart, strEnd, gLot.dTactTime_RunTime, nSum, (double)(gLot.dwRunTime) / 1000,
		(double)(gLot.dwStopTime[nLPNo]) / 1000, (double)(gLot.dwErrorTime) / 1000, gLot.nErrorCount, dRate, 3600 / gLot.dTactTime_RunTime ,(double)(gLot.dwRunTime + gLot.dwErrorTime + gLot.dwStopTime[nLPNo]) / gLot.nErrorCount);
	g_objLogFile.Save_OperatingRatio(strMsg);

	gData.dwRunTimeAccumulated += gLot.dwRunTime;
	gLot.dwRunTime = gLot.dwErrorTime = gLot.dwStopTime[nLPNo] = 0;

	g_objLogFile.Save_AverageCycle(nLPNo);
	g_objLogFile.Save_LotLog(nPortNo);	//gjcs

	// 해당포트 판정 완료, 결과 초기화
	gLot.nCapFailCount[nLPNo] = 0;

	memset(gData.nCmInspectInfo, 0x00, sizeof(int) * 2 * 50 * 12);
	memset(gData.nCapInspectInfo, 0x00, sizeof(int) * 2 * 50 * 28);
	memset(gData.nCmJigNo[nLPNo], 0x00, sizeof(int) * 50 * 12 * 5);	// Module Tracking 초기화
	gData.nLoadTrayCount[nLPNo] = 0;

	if (!m_pEquipData->bUseInlineMode) {
		gData.sLotID[nLPNo] = "";
		gData.nCmUseCount[nLPNo] = 0;
		gData.nTrayUseCount[nLPNo] = 0;	
	}

	gData.bLoadLampOn[nLPNo] = TRUE;

	if (!m_pEquipData->bUseInlineMode) {
		if (nLPNo == 0) {
			g_dlgWork.m_stcLotId[0].SetWindowText("");
			g_dlgWork.m_stcCmCount[0].SetWindowText("0");
			g_dlgWork.m_stcTrayCount[0].SetWindowText("0");
		} else {
			g_dlgWork.m_stcLotId[1].SetWindowText("");
			g_dlgWork.m_stcCmCount[1].SetWindowText("0");
			g_dlgWork.m_stcTrayCount[1].SetWindowText("0");
		}
	}
	
	if (!m_pEquipData->bUseInlineMode)	g_dlgWork.Enable_UserInput(nPortNo, TRUE);

	g_dlgWork.PostMessage(UM_UPDATE_UPH, NULL, NULL);

//	Beep_Post(1000);
}

void CSequenceMain::Beep_Post(int nState, int nTime)
{
	if (nTime < 500) nTime = 500;

	if (nState == 0) {	// Lot End (Alarm state)
		m_pDY12->oBuzzerBit0 = m_pEquipData->bBuzzer[0][0];
		m_pDY12->oBuzzerBit1 = m_pEquipData->bBuzzer[0][1];
		m_pDY12->oBuzzerBit2 = m_pEquipData->bBuzzer[0][2];
		m_pDY12->oBuzzerBit3 = m_pEquipData->bBuzzer[0][3];
		m_pDY12->oBuzzerBit4 = m_pEquipData->bBuzzer[0][4];

	} else if (nState == 1) {	// NG Full
		m_pDY12->oBuzzerBit0 = TRUE;
		m_pDY12->oBuzzerBit1 = FALSE;
		m_pDY12->oBuzzerBit2 = FALSE;
		m_pDY12->oBuzzerBit3 = FALSE;
		m_pDY12->oBuzzerBit4 = FALSE;
	}
	g_objAJinAXL.Write_Output(12);
	theApp.uSleep(nTime);
	m_pDY12->oBuzzerBit0 = FALSE;
	m_pDY12->oBuzzerBit1 = FALSE;
	m_pDY12->oBuzzerBit2 = FALSE;
	m_pDY12->oBuzzerBit3 = FALSE;
	m_pDY12->oBuzzerBit4 = FALSE;
	g_objAJinAXL.Write_Output(12);
}

BOOL CSequenceMain::Check_MainIndexCylinder()
{
	if (m_pDX11->iIndexLoadAlignIn && !m_pDX11->iIndexLoadAlignOut &&
		m_pDX11->iIndexAssyAlignIn && !m_pDX11->iIndexAssyAlignOut &&
		m_pDX11->iIndexTransAlignIn && !m_pDX11->iIndexTransAlignOut &&
		m_pDX11->iIndexLoadVacDown && !m_pDX11->iIndexLoadVacUp &&
		m_pDX11->iIndexAssyVacDown && !m_pDX11->iIndexAssyVacUp) {
		return TRUE;
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// 1. (Error : 3100)
BOOL CSequenceMain::TrayPicker_Run()
{
	switch (m_nTrayPickCase) {
	case 0:		// Start 시 1로 진행
		return TRUE;

	case 1:		// Inline Mode Check.
		if (m_bUnloadLotEnd || gData.bCycleStop || !m_pEquipData->bUseInlineMode) {
			m_nTrayPickCase = 50; m_tTrayPickLoop.Set_LoopTime(5000);	// 종료.
		} else {
			m_dwTrayPick = GetTickCount();
			m_nTrayPickCase++; m_tTrayPickLoop.Set_LoopTime(5000);
		}
		return TRUE;
	case 2:		// Picker X Move to AVI Position
		if (g_objCommon.Check_Position(AX_TRAY_PICKER_Z, 0) && g_objCommon.Check_Position(AX_TRAY_PICKER_R, 0)) {
			g_objCommon.Move_Position(AX_TRAY_PICKER_X, 0);		// AVI Position
			m_nTrayPickCase++; m_tTrayPickLoop.Set_LoopTime(5000);
		}		
		break;
	case 3:		// Check Picker X AVI Position
		if (g_objCommon.Check_Position(AX_TRAY_PICKER_X, 0) && g_objCommon.Check_Position(AX_TRAY_PICKER_R, 0)) {
			m_nTrayPickCase = 5; m_tTrayPickLoop.Set_LoopTime(5000);
		}
		break;

	case 5:		// Wait for AVI Tray Load
		if (gData.bAviTrayLoad) { m_nTrayPickCase++; m_tTrayPickLoop.Set_LoopTime(5000); }
		return TRUE;

	case 6:		// Picker Z Move to AVI Down Position
		if (g_objCommon.Check_Position(AX_TRAY_PICKER_X, 0) && g_objCommon.Check_Position(AX_TRAY_PICKER_R, 0)) {
			if (!g_objCommon.Get_TrayPickerGripOpen()) break;	// 인터락
			m_tTrayPickLoop.Takt_Start();

			g_objCommon.Move_Position(AX_TRAY_PICKER_Z, 1);
			m_nTrayPickCase++; m_tTrayPickLoop.Set_LoopTime(5000);
		}		
		break;
	case 7:		// 정보전달 & Picker Grip Close
		if (g_objCommon.Check_Position(AX_TRAY_PICKER_Z, 1)) {
			m_tTrayPickLoop.Takt_Save(1, 1);
			m_tTrayPickLoop.Takt_Start();

			gData.nPNoTrayPick = gData.nPNoAviPort;
			gData.nTNoTrayPick = gData.nTNoAviPort;
			gData.nCmCntTrayPick = gData.nCmCntAviPort;

			g_objCommon.Set_TrayPickerGripClose();
			m_nTrayPickCase++; m_tTrayPickLoop.Set_LoopTime(5000);
		}		
		break;
	case 8:		// Picker Z Move to Ready Up
		if (g_objCommon.Get_TrayPickerGripClose() && m_pDX00->iTrayPickerExist) {
			m_tTrayPickLoop.Takt_Save(1, 2);
			m_tTrayPickLoop.Takt_Start();

			g_objCommon.Move_Position(AX_TRAY_PICKER_Z, 0);
			m_nTrayPickCase++; m_tTrayPickLoop.Set_LoopTime(10000);
		}
		break;
	case 9:		// Send TrayUnload
		if (g_objCommon.Check_Position(AX_TRAY_PICKER_Z, 0) && m_pDX00->iTrayPickerExist) {
			m_tTrayPickLoop.Takt_Save(1, 3);
			m_tTrayPickLoop.Takt_Start();

			g_objAviHandler.Set_TrayUnload();
			m_nTrayPickCase = (m_pEquipData->bUseTrayPickerTurn ? 11 : 14); m_tTrayPickLoop.Set_LoopTime(5000);
		}
		break;

	case 11:	// Picker X Move to Rotate Position
		if (g_objCommon.Check_Position(AX_TRAY_PICKER_Z, 0) && m_pDX00->iTrayPickerExist) {
			g_objCommon.Move_Position(AX_TRAY_PICKER_X, 2);	// Rotate Position
			m_nTrayPickCase++; m_tTrayPickLoop.Set_LoopTime(10000);
		}		
		break;
	case 12:	// Picker R Movet to Rotate Position
		if (g_objCommon.Check_Position(AX_TRAY_PICKER_X, 2) && g_objCommon.Check_Position(AX_TRAY_PICKER_Z, 0)) {
			g_objCommon.Move_Position(AX_TRAY_PICKER_R, 1);	// Rotate
			m_nTrayPickCase++; m_tTrayPickLoop.Set_LoopTime(10000);
		}
		break;
	case 13:	// Picker X Move to Load Port1 Position
		if (m_pEquipData->bUseTrayPickerTurn && g_objCommon.Check_Position(AX_TRAY_PICKER_R, 1) && g_objCommon.Check_Position(AX_TRAY_PICKER_Z, 0)) {
			g_objCommon.Move_Position(AX_TRAY_PICKER_X, 1);	// Load1 Pos
			m_nTrayPickCase = 20; m_tTrayPickLoop.Set_LoopTime(10000);
		}
		break;
	case 14:	// Picker X Move to Load Port1 Position
		if (!m_pEquipData->bUseTrayPickerTurn && g_objCommon.Check_Position(AX_TRAY_PICKER_R, 0) && g_objCommon.Check_Position(AX_TRAY_PICKER_Z, 0)) {
			g_objCommon.Move_Position(AX_TRAY_PICKER_X, 1);	// Load1 Pos
			m_nTrayPickCase = 20; m_tTrayPickLoop.Set_LoopTime(10000);
		}
		break;

	case 20:	// Check Position
		if (g_objCommon.Check_Position(AX_TRAY_PICKER_X, 1)) {
			m_tTrayPickLoop.Takt_Save(1, 4);
			m_nTrayPickCase++; m_tTrayPickLoop.Set_LoopTime(5000);
		}
		break;
	case 21:	// Inter Lock - Load Port1 Check, Tray가 있으면 대기한다.
		if (!m_pDX00->iLoadPort1LowCheck && !m_bLoadPortTrayExist &&
			(m_nLoadStage1Case < 3 || m_nLoadStage1Case > 9) && (m_nLoadStage2Case < 3 || m_nLoadStage2Case > 9)) {
			m_nTrayPickCase++; m_tTrayPickLoop.Set_LoopTime(5000);
		}
		return TRUE;
	case 22:	// Picker Z Move to Load1 Down
		if (g_objCommon.Check_Position(AX_TRAY_PICKER_X, 1) &&
			((!m_pEquipData->bUseTrayPickerTurn && g_objCommon.Check_Position(AX_TRAY_PICKER_R, 0)) ||
			 ( m_pEquipData->bUseTrayPickerTurn && g_objCommon.Check_Position(AX_TRAY_PICKER_R, 1))))
		{
			m_tTrayPickLoop.Takt_Start();
			g_objCommon.Move_Position(AX_TRAY_PICKER_Z, 2);
			m_nTrayPickCase++; m_tTrayPickLoop.Set_LoopTime(10000);
		}
		break;
	case 23:	// Grip Open
		if (g_objCommon.Check_Position(AX_TRAY_PICKER_Z, 2)) {
			m_tTrayPickLoop.Takt_Save(1, 5);
			m_tTrayPickLoop.Takt_Start();
			g_objCommon.Set_TrayPickerGripOpen();
			m_nTrayPickCase++; m_tTrayPickLoop.Set_LoopTime(5000);
		}
		break;
	case 24:	// 정보전달, Z Move to Ready up
		if (g_objCommon.Get_TrayPickerGripOpen()) {
			m_tTrayPickLoop.Takt_Save(1, 6);
			m_tTrayPickLoop.Takt_Start();

			m_bLoadPortTrayExist = TRUE;
			gData.nPNoLoadPort = gData.nPNoTrayPick;
			gData.nTNoLoadPort = gData.nTNoTrayPick;
			gData.nCmCntLoadPort = gData.nCmCntTrayPick;
			
			g_objCommon.Move_Position(AX_TRAY_PICKER_Z, 0);
			m_nTrayPickCase++; m_tTrayPickLoop.Set_LoopTime(5000);
		}
		break;
	case 25:	// X Move to AVI Position
		if (g_objCommon.Check_Position(AX_TRAY_PICKER_Z, 0) && !m_pDX00->iTrayPickerExist) {
			m_tTrayPickLoop.Takt_Save(1, 7);
			m_tTrayPickLoop.Takt_Start();

			if (m_pEquipData->bUseTrayPickerTurn)	{
				g_objCommon.Move_Position(AX_TRAY_PICKER_X, 2);
				m_nTrayPickCase++; m_tTrayPickLoop.Set_LoopTime(10000);

			} else {
				g_objCommon.Move_Position(AX_TRAY_PICKER_X, 0);
				m_nTrayPickCase = 30; m_tTrayPickLoop.Set_LoopTime(10000);
			}			
		}
		break;
	case 26:
		if (g_objCommon.Check_Position(AX_TRAY_PICKER_X, 2)) {
			g_objCommon.Move_Position(AX_TRAY_PICKER_R, 0);	// Rotate
			m_nTrayPickCase++; m_tTrayPickLoop.Set_LoopTime(10000);
		}
		break;
	case 27:		// Rotation Check
		if (g_objCommon.Check_Position(AX_TRAY_PICKER_R, 0)) {
			g_objCommon.Move_Position(AX_TRAY_PICKER_X, 0);	// AVI Position
			m_nTrayPickCase = 30; m_tTrayPickLoop.Set_LoopTime(10000);
		}
		break;

	case 30:	// Position Check
		if (g_objCommon.Check_Position(AX_TRAY_PICKER_X, 0) && g_objCommon.Check_Position(AX_TRAY_PICKER_R, 0)) {
			m_tTrayPickLoop.Takt_Save(1, 8);
			m_nTrayPickCase = 1; m_tTrayPickLoop.Set_LoopTime(5000);

			m_strLog.Format("TrayPicker, %d", GetTickCount() - m_dwTrayPick);
			g_objLogFile.Save_TestLog(m_strLog);			
		}
		break;

	//종료
	case 50:
		if (g_objCommon.Check_Position(AX_TRAY_PICKER_Z, 0) && g_objCommon.Check_Position(AX_TRAY_PICKER_R, 0)) {
			g_objCommon.Move_Position(AX_TRAY_PICKER_X, 2);
			m_nTrayPickCase++; m_tTrayPickLoop.Set_LoopTime(5000);
		}
		break;
	case 51:
		if (g_objCommon.Check_Position(AX_TRAY_PICKER_X, 2) && g_objCommon.Get_TrayPickerGripOpen()) {
			m_nTrayPickCase = 0; m_tTrayPickLoop.Set_LoopTime(5000);
		}
		break;
	}

	// 1. (Error : 3100)
	if (m_tTrayPickLoop.Over_LoopTime()) {
		g_objCommon.Show_Error(3100 + m_nTrayPickCase);
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 2. (Error : 3200)
BOOL CSequenceMain::LoadStage1_Run()
{
	static int nLs1WorkPort = 0;		// 작업중인 Port (1 ~ 2)
	static int nLs1AviPort = 0;			// Inline Mode 일때 Avi Port 번호 받는다.

	if (gData.bLoadPort3Wait && m_nLoadStage1Case > 22 && m_nLoadStage1Case < 50) return TRUE;

	if (!m_pEquipData->bUseInlineMode) {
		// LJH 1번 투입시 2번에 Tray 있으면 가져가야한다...
		// Load Tray Lot 투입시 도어락, 버튼 확인하여 Tray Loading 작업 못하게 막아준다.
		// 단독 모드일때 투입시 LoadStage가 움직이면 위험하니까 Door가 열리면 LoadStage는 임시 정지 하도록 함. 
		if (m_bLotLoadEnable[0] && (m_pEquipData->bUseDoorLock && m_pDX13->iDoor01Open) && (nLs1WorkPort == 1 && m_nLoadStage1Case > 3 && m_nLoadStage1Case < 10)) return TRUE;
		if (m_bLotLoadEnable[1] && (m_pEquipData->bUseDoorLock && m_pDX13->iDoor02Open) && (nLs1WorkPort == 2 && m_nLoadStage1Case > 3 && m_nLoadStage1Case < 10)) return TRUE;
	}

	switch (m_nLoadStage1Case) {
	case 0:		// Start 시 1로 진행
		nLs1WorkPort = 0;
		nLs1AviPort = 0;
		return TRUE;

	case 1:		// Load Tray, Port 확인.
		if (g_objCommon.Check_Position(AX_LOAD_STAGE1_Z, 1)) {	// 안전 확인
			if (m_bUnloadLotEnd || gData.bCycleStop) { nLs1WorkPort = 0; m_nLoadStage1Case = 0; break; }	// 사이클스탑이면 종료.

			//Inlinde 모드에서는 Port 1번만 사용한다. 
			if (m_pEquipData->bUseInlineMode) {
				if (m_pDX00->iLoadPort1LowCheck && m_bLoadPortTrayExist && (m_nTrayPickCase < 12 || m_nTrayPickCase > 15)) {
					nLs1WorkPort = 1;//Inlinde 모드에서는 Port 1번만 사용한다. 
					nLs1AviPort = gData.nPNoLoadPort;
					m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(5000);
				}
				return TRUE;
			} else {
				// 단독 모드
				if (m_bLoadLotEnd) {
					m_nLoadStage1Case = 0;

				} else if (m_pDX00->iLoadPort1LowCheck || m_pDX01->iLoadPort2LowCheck) {
					// 맨처음 시작할때 2개 모두 적재 되어있으면 1번부터 가져간다.
					// 작업할 포트에 트레이 유무와 입력수량확인.
					if ((nLs1WorkPort == 0 || nLs1WorkPort == 2) && m_pDX00->iLoadPort1LowCheck && (gData.nCmUseCount[0] > 0)) { nLs1WorkPort = 1; nLs1AviPort = 1; }
					else if ((nLs1WorkPort == 0 || nLs1WorkPort == 1) && m_pDX01->iLoadPort2LowCheck && (gData.nCmUseCount[1] > 0)) { nLs1WorkPort = 2; nLs1AviPort = 2; }

					//if (!Check_LoadTrayLoading(nLs1WorkPort)) return TRUE;	// 같은 Port Lot을 동시에 검사하지 않는다.
					m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(5000);
				}
			}
		}
		break;
	case 2:		// Tray X Move to Load Position
		if (gData.bCycleStop) {
			nLs1WorkPort = 0;
			nLs1AviPort = 0;
			m_nLoadStage1Case = 0;	// 사이클스탑이면 종료.
		}
		if (gData.nLoadTrayCount[nLs1AviPort-1] <= gData.nTrayUseCount[nLs1AviPort-1] && !gData.bCycleStop) {
			if ((nLs1WorkPort == 1 && !m_pDX00->iLoadPort1LowCheck) || (nLs1WorkPort == 2 && !m_pDX01->iLoadPort2LowCheck)) break;

			if (nLs1WorkPort == 1 && !g_objCommon.Check_Position(AX_LOAD_STAGE1_X, 0)) g_objCommon.Move_Position(AX_LOAD_STAGE1_X, 0);	// Port1
			if (nLs1WorkPort == 2 && !g_objCommon.Check_Position(AX_LOAD_STAGE1_X, 1)) g_objCommon.Move_Position(AX_LOAD_STAGE1_X, 1);	// Port2
			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(5000);
		} 
		break;
	case 3:		// Check Position, Lot Start
		if ((nLs1WorkPort == 1 && g_objCommon.Check_Position(AX_LOAD_STAGE1_X, 0)) || 
			(nLs1WorkPort == 2 && g_objCommon.Check_Position(AX_LOAD_STAGE1_X, 1))) {
			m_dwLoadStage1 = GetTickCount();
			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(30000);
		} 
		break;
	case 4:		// Tray Z Move to Support Up Position (Load)
		if (g_objCommon.Get_LoadStageMasterSlaveOut(1) && !m_pDX04->iLoadStage1Exist && m_pDX00->iLoadPort1SlideClose && m_pDX01->iLoadPort2SlideClose) {
			m_tLoadStage1Loop.Takt_Start();
			gData.nLPNo = nLs1WorkPort;
			g_dlgWork.Enable_UserInput(nLs1WorkPort, FALSE);
			g_objCommon.Move_Position(AX_LOAD_STAGE1_Z, 2);	// Support Up
			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(10000);
		}
		break;
	case 5:		// Port Support Out
		if (g_objCommon.Check_Position(AX_LOAD_STAGE1_Z, 2)) {
			m_tLoadStage1Loop.Takt_Save(2, 1);
			m_tLoadStage1Loop.Takt_Start();
			g_objCommon.Set_LoadPortSupportOut(nLs1WorkPort);
			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 6:		// Tray Z Move to Support Down Position
		if (g_objCommon.Get_LoadPortSupportOut(nLs1WorkPort)) {
			m_tLoadStage1Loop.Takt_Save(2, 2);
			m_tLoadStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_LOAD_STAGE1_Z, 3);	// Support Down
			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(10000);
		}
		break;
	case 7:		// Port Support In
		if (g_objCommon.Check_Position(AX_LOAD_STAGE1_Z, 3)) {
			m_tLoadStage1Loop.Takt_Save(2, 3);
			m_tLoadStage1Loop.Takt_Start();
			//Inlinde 모드 일때는 CM 개수, Tray 번호등 정보가 존재하지만 단독모드에서는 없다.
			if (m_pEquipData->bUseInlineMode) {
				gData.nPNoLoadTray[0] = gData.nPNoLoadPort;
				gData.nTNoLoadTray[0] = gData.nTNoLoadPort;
				gData.nLoadTrayCount[nLs1AviPort-1] = gData.nTNoLoadPort;
				gData.nCmCntLoadStage[0] = gData.nCmCntLoadPort;

				//PC Information Log 파일 생성
				CString strLog, strCPU, strRAM, strHardDisk;
				g_objCommon.Get_CPUInfo(strCPU);
				g_objCommon.Get_RAMSize(strRAM);
				g_objCommon.Get_HardInfo(strHardDisk);

				strLog.Format("%s,%s,%s,%s,%s",gData.sLotID[gData.nPNoLoadTray[0]-1],gData.sRecipe, strCPU,strRAM,strHardDisk);
				g_objLogFile.Save_PCLog(gData.nPNoLoadTray[0], strLog);


			} else {
				gData.nPNoLoadTray[0] = nLs1WorkPort;
			}
			g_objCommon.Set_LoadPortSupportIn(nLs1WorkPort);
			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 8:		// Tray Z Move to Move Up Position
		if (g_objCommon.Get_LoadPortSupportIn(nLs1WorkPort) && m_pDX04->iLoadStage1Exist) {
			m_tLoadStage1Loop.Takt_Save(2, 4);
			m_tLoadStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_LOAD_STAGE1_Z, 1);	// Move Up (Work)
			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(10000);
		}
		break;
	case 9:		// Tray Master In
		if (g_objCommon.Check_Position(AX_LOAD_STAGE1_Z, 1) && m_pDX04->iLoadStage1Exist) {
			m_tLoadStage1Loop.Takt_Save(2, 5);
			m_tLoadStage1Loop.Takt_Start();

			m_bLoadPortTrayExist = FALSE;	// Port에 Tray가 없다고 알려준다.

			m_pDY04->oLoadStage1MasterIn = TRUE;
			g_objAJinAXL.Write_Output(4);
			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 10:	// Tray Slave In
		if (m_pDX04->iLoadStage1MasterIn && !m_pDX04->iLoadStage1MasterOut) {
			m_pDY04->oLoadStage1SlaveIn = TRUE;
			g_objAJinAXL.Write_Output(4);
			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 11:	// Check Tray Master/Slave In
		if (g_objCommon.Get_LoadStageMasterSlaveIn(1) && m_pDX04->iLoadStage1Exist) {
			m_tLoadStage1Loop.Takt_Save(2, 6);
			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(5000);
		}
		break;

	case 12:	// 안전확인
		if (m_nLoadStage2Case > 22) {
			m_nLoadStage1Case = 15; m_tLoadStage1Loop.Set_LoopTime(5000);
		}
		return TRUE;

	case 13:	// X Move to Load2 Position
		if (g_objCommon.Check_Position(AX_LOAD_STAGE1_Z, 1)) {
			g_objCommon.Move_Position(AX_LOAD_STAGE1_X, 1);	// Port2
			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 14:	// Position Check
		if (g_objCommon.Check_Position(AX_LOAD_STAGE1_X, 1)) {
			if (m_nLoadStage2Case > 22) {
				m_nLoadStage1Case = 15; m_tLoadStage1Loop.Set_LoopTime(5000);
			}
			return TRUE;			
		}
		break;

	case 15:	// Tray X Move to Work Position, Tray Init
		if (g_objCommon.Check_Position(AX_LOAD_STAGE1_Z, 1)) {
			m_tLoadStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_LOAD_STAGE1_X, 2);	// Work
			if (!m_pEquipData->bUseInlineMode) {
				gData.nLoadTrayCount[nLs1WorkPort-1]++;
				gData.nTNoLoadTray[0] = gData.nLoadTrayCount[nLs1WorkPort-1];
			}
			Init_LoadTray(nLs1AviPort, 1);
			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(20000);
		}
		break;
	case 16:	// Position Check
		if (g_objCommon.Check_Position(AX_LOAD_STAGE1_X, 2)) 
		{			
			if(gData.nLoadTrayCount[nLs1AviPort-1] == 1) 
			{
				//if((m_nLoadPickCase > 6 || m_nLoadPickCase < 4) && !Check_IndexEmpty(-1)) return TRUE;

				g_dlgWork.Enable_UserInput(nLs1WorkPort, FALSE);
				
				Job_LotStart(nLs1AviPort);
				m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(30000);

			} else {
				m_nLoadStage1Case = 20; m_tLoadStage1Loop.Set_LoopTime(10000);
			}
			m_tLoadStage1Loop.Takt_Save(2, 7);
		}
		break;
	case 17:	// Check Lot Ready
		if (g_objInspector.Check_LotReady()) 
		{			
			gData.sInspectCmLotIDLater = gData.sLotID[nLs1AviPort-1];
			if (gData.nInspectCmLotCount < m_pEquipData->nInspectCmLotTimes ) 
			{					
				gData.nInspectCmCheckTime = 0;
				gData.bInspectCmThisLotVSkip = FALSE;
			}
			m_nLoadStage1Case = 20; m_tLoadStage1Loop.Set_LoopTime(5000);
		}
		break;

	case 20:	
		// Working, 20에 있는 동안 Load Picker에서 Load Stage를 제어 한다. 
		// Load Picker 6번 케이스에서 Tray가 모두 비워졋는지 체크하고 LoadStage 21번 케이스로 변경한다.
		return TRUE;


	case 21:	// 안전 확인 
		if (m_nLoadStage2Case > 51 || m_nLoadStage2Case <= 20) {
			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(5000);
		}
		return TRUE;
	case 22:	// X Move to Unload Port Position
		if (g_objCommon.Check_Position(AX_LOAD_STAGE1_Z, 1)) {
			m_tLoadStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_LOAD_STAGE1_X, 3);
			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(10000);
		}
		break;
	case 23:	// Z Move to Unload Up Position
		if (g_objCommon.Check_Position(AX_LOAD_STAGE1_X, 3) && m_pDX01->iLoadPort3SlideClose) {
			m_tLoadStage1Loop.Takt_Save(2, 8);
			m_tLoadStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_LOAD_STAGE1_Z, 4);	// Unload Up
			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(10000);
		}
		break;
	case 24:	// Slave Out
		if (g_objCommon.Check_Position(AX_LOAD_STAGE1_Z, 4)) {
			m_tLoadStage1Loop.Takt_Save(2, 9);
			m_tLoadStage1Loop.Takt_Start();
			m_pDY04->oLoadStage1SlaveIn = FALSE;
			g_objAJinAXL.Write_Output(4);
			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 25:	// Master Out
		if (!m_pDX04->iLoadStage1SlaveIn && m_pDX04->iLoadStage1SlaveOut) {
			m_pDY04->oLoadStage1MasterIn = FALSE;
			g_objAJinAXL.Write_Output(4);
			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 26:	// Z Axis Move to Move Down Position
		if (g_objCommon.Get_LoadStageMasterSlaveOut(1)) {
			m_tLoadStage1Loop.Takt_Save(2, 10);
			m_tLoadStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_LOAD_STAGE1_Z, 0);	// Move Down
			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(10000);
		}
		break;
	case 27:	// Position Check
		if (g_objCommon.Check_Position(AX_LOAD_STAGE1_Z, 0)) {
			m_tLoadStage1Loop.Takt_Save(2, 11);
			m_nLoadStage1Case = 50; m_tLoadStage1Loop.Set_LoopTime(5000);

			m_strLog.Format("LoadTray1, %d", GetTickCount() - m_dwLoadStage1);
			g_objLogFile.Save_TestLog(m_strLog);
		}
		break;


	case 50:	//	Interlock
		if (m_nLoadStage2Case > 11 && m_nLoadStage2Case < 50) {
			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(5000);
		}
		return TRUE;

	case 51:	// Tray X Move to Load1 Position
		if (g_objCommon.Check_Position(AX_LOAD_STAGE1_Z, 0)) {
			if (g_objCommon.Check_Position(AX_LOAD_STAGE2_Z, 0)) break;	// 인터락
			m_tLoadStage1Loop.Takt_Start();
			if (m_pEquipData->bUseInlineMode) {
				nLs1WorkPort = 1;
			} else {	//앞에 작업 중인 Port 번호를 받는다.
				if (nLs1WorkPort == 0) nLs1WorkPort = gData.nPNoLoadTray[1];
				if (nLs1AviPort == 0) nLs1AviPort = gData.nPNoLoadTray[1];
			}

			if (nLs1WorkPort == 1) { g_objCommon.Move_Position(AX_LOAD_STAGE1_X, 0); }	// Port1
			if (nLs1WorkPort == 2) { g_objCommon.Move_Position(AX_LOAD_STAGE1_X, 1); }	// Port2

			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(10000);
		}
		break;
	case 52:	// Check End or Go
		if ((nLs1WorkPort == 1 && g_objCommon.Check_Position(AX_LOAD_STAGE1_X, 0)) ||
			(nLs1WorkPort == 2 && g_objCommon.Check_Position(AX_LOAD_STAGE1_X, 1)))
		{
			m_tLoadStage1Loop.Takt_Save(2, 12);
			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(10000);			
		}
		break;
	case 53:	// 안전확인
		if (m_nLoadStage2Case >= 20) {
			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(10000);
		}
		return TRUE;
	case 54:	// Z Move to Move Up Position 
		if ((nLs1WorkPort == 1 && g_objCommon.Check_Position(AX_LOAD_STAGE1_X, 0)) ||
			(nLs1WorkPort == 2 && g_objCommon.Check_Position(AX_LOAD_STAGE1_X, 1)))
		{
			g_objCommon.Move_Position(AX_LOAD_STAGE1_Z, 1);	// Move Up
			m_nLoadStage1Case++; m_tLoadStage1Loop.Set_LoopTime(10000);
		}
		break;
	case 55:	//
		if (g_objCommon.Check_Position(AX_LOAD_STAGE1_Z, 1)) {
			if (m_pEquipData->bUseInlineMode) {	// 인라인 모드는 무조건 Port 1번으로 가고 case도 1번에서 새로 시작한다.
				m_nLoadStage1Case = 1; m_tLoadStage1Loop.Set_LoopTime(10000);
			} else {							// 단독 모드는 연속랏이 되어야하기 때문에 Port확인 해준다.
				if (gData.nLoadTrayCount[nLs1WorkPort-1] >= gData.nTrayUseCount[nLs1WorkPort-1]) {
					m_bLotLoadEnable[nLs1WorkPort-1] = TRUE;	// Port에서 트레이 로드 완료
// 					m_bLoadLotEnd = TRUE;	//temp 연속랏은 좀더 생각해보자.
					if (gData.nTNoLoadTray[0] >= gData.nTrayUseCount[nLs1WorkPort-1]) gData.nLoadTrayCount[nLs1WorkPort-1] = 0;

					if (nLs1WorkPort == 1) {
						m_pDY00->oLoadPort1SlideLock = FALSE; m_pDY00->oLoadPort1SlideUnlock = TRUE; 
						//m_pDY13->oDoor02Unlock = TRUE; 
						nLs1WorkPort = 2;
						nLs1AviPort = 2;

					} else if (nLs1WorkPort == 2) {
						m_pDY01->oLoadPort2SlideLock = FALSE; m_pDY01->oLoadPort2SlideUnlock = TRUE; 
						//m_pDY13->oDoor03Unlock = TRUE; 
						nLs1WorkPort = 1;
						nLs1AviPort = 1;
					}
					g_objAJinAXL.Write_Output(0);
					g_objAJinAXL.Write_Output(1);

					m_nLoadStage1Case = 60; m_tLoadStage1Loop.Set_LoopTime(10000);
// 					m_nLoadStage1Case = 0; m_tLoadStage1Loop.Set_LoopTime(10000);
				} else {
					m_nLoadStage1Case = 2; m_tLoadStage1Loop.Set_LoopTime(10000);
				}
			}
		}
		break;
	case 60:	// Wait, Port Select
		if (gData.nLoadTrayCount[nLs1WorkPort-1] == 0 && !Check_LoadTrayLoading(nLs1WorkPort)) return TRUE;	// 같은 Port에서 연속으로 2Lot은 검사할 수 없다.

		if ((nLs1WorkPort == 1 && m_pDX00->iLoadPort1LowCheck && m_bLotLoadEnable[0] == FALSE) ||
			(nLs1WorkPort == 2 && m_pDX01->iLoadPort2LowCheck && m_bLotLoadEnable[1] == FALSE)) {
			g_dlgWork.Get_LotInfo(nLs1WorkPort-1);
			m_nLoadStage1Case = 2; m_tLoadStage1Loop.Set_LoopTime(10000);

		} else {
			if (m_bUnloadLotEnd) {
				m_nLoadStage1Case = 0; m_tLoadStage1Loop.Set_LoopTime(10000);	// 작업 종료
			}
		}		
		return TRUE;
	}

	// 2. (Error : 3200)
	if (m_tLoadStage1Loop.Over_LoopTime()) {
		g_objCommon.Show_Error(3200 + m_nLoadStage1Case);
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 3. (Error : 3300)
BOOL CSequenceMain::LoadStage2_Run()
{
	static int nLs2WorkPort = 0;		// 작업중인 Port (1 ~ 2)
	static int nLs2AviPort = 0;			// Inline Mode 일때 Avi Port 번호 받는다.

	if (gData.bLoadPort3Wait && m_nLoadStage2Case > 22 && m_nLoadStage2Case < 50) return TRUE;

	if (!m_pEquipData->bUseInlineMode) {
		// LJH 1번 투입시 2번에 Tray 있으면 가져가야한다...
		// Load Tray Lot 투입시 도어락, 버튼 확인하여 Tray Loading 작업 못하게 막아준다.
		if (m_bLotLoadEnable[0] && (m_pEquipData->bUseDoorLock && m_pDX13->iDoor01Open) && (nLs2WorkPort == 1 && m_nLoadStage2Case > 3 && m_nLoadStage2Case < 10)) return TRUE;
		if (m_bLotLoadEnable[1] && (m_pEquipData->bUseDoorLock && m_pDX13->iDoor02Open) && (nLs2WorkPort == 2 && m_nLoadStage2Case > 3 && m_nLoadStage2Case < 10)) return TRUE;
	}

	switch (m_nLoadStage2Case) {
	case 0:		// Start 시 1로 진행
		nLs2WorkPort = 0;
		nLs2AviPort = 0;
		return TRUE;

	case 1:		// Load Tray, Port 확인.
		if (g_objCommon.Check_Position(AX_LOAD_STAGE2_Z, 1)) {	// 안전 확인
			if (m_bUnloadLotEnd || gData.bCycleStop) { nLs2WorkPort = 0; m_nLoadStage2Case = 0; break; }	// 사이클스탑이면 종료.

			if (m_pEquipData->bUseInlineMode) {
				if (m_pDX00->iLoadPort1LowCheck && m_bLoadPortTrayExist && (m_nTrayPickCase < 12 || m_nTrayPickCase > 15)) {
					nLs2WorkPort = 1;
					nLs2AviPort = gData.nPNoLoadPort;
					m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(5000);
				}
				return TRUE;
			} else {
				// 단독 모드
				if (m_bLoadLotEnd) {
					m_nLoadStage2Case = 0;

				} else if (m_pDX00->iLoadPort1LowCheck || m_pDX01->iLoadPort2LowCheck) {
					// 맨처음 시작할때 2개 모두 적재 되어있으면 1번부터 가져간다.
					// 작업할 포트에 트레이 유무와 입력수량확인.
					if ((nLs2WorkPort == 0 || nLs2WorkPort == 2) && m_pDX00->iLoadPort1LowCheck && (gData.nCmUseCount[0] > 0)) { nLs2WorkPort = 1; nLs2AviPort = 1; }
					else if ((nLs2WorkPort == 0 || nLs2WorkPort == 1) && m_pDX01->iLoadPort2LowCheck && (gData.nCmUseCount[1] > 0)) { nLs2WorkPort = 2; nLs2AviPort = 2; }

					if (!Check_LoadTrayLoading(nLs2WorkPort)) return TRUE;	// 같은 Port Lot을 동시에 검사하지 않는다.
					m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(5000);
				}
			}
		}
		break;
	case 2:		// Tray X Move to Load Position
		if (gData.bCycleStop) {
			nLs2WorkPort = 0;
			nLs2AviPort = 0;
			m_nLoadStage2Case = 0;	// 사이클스탑이면 종료.
		}
		if (gData.nLoadTrayCount[nLs2AviPort-1] <= gData.nTrayUseCount[nLs2AviPort-1] && !gData.bCycleStop) {
			if ((nLs2WorkPort == 1 && !m_pDX00->iLoadPort1LowCheck) || (nLs2WorkPort == 2 && !m_pDX01->iLoadPort2LowCheck)) break;

			if (nLs2WorkPort == 1 && !g_objCommon.Check_Position(AX_LOAD_STAGE2_X, 0)) g_objCommon.Move_Position(AX_LOAD_STAGE2_X, 0);	// Port1
			if (nLs2WorkPort == 2 && !g_objCommon.Check_Position(AX_LOAD_STAGE2_X, 1)) g_objCommon.Move_Position(AX_LOAD_STAGE2_X, 1);	// Port2
			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(5000);
		} 
		break;
	case 3:		// Check Position, Lot Start
		if ((nLs2WorkPort == 1 && g_objCommon.Check_Position(AX_LOAD_STAGE2_X, 0)) || 
			(nLs2WorkPort == 2 && g_objCommon.Check_Position(AX_LOAD_STAGE2_X, 1))) {
			m_dwLoadStage2 = GetTickCount();
			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(30000);
		} 
		break;
	case 4:		// Tray Z Move to Support Up Position (Load)
		if (g_objCommon.Get_LoadStageMasterSlaveOut(2) && !m_pDX04->iLoadStage2Exist && m_pDX00->iLoadPort1SlideClose && m_pDX01->iLoadPort2SlideClose) {
			m_tLoadStage2Loop.Takt_Start();
			gData.nLPNo = nLs2WorkPort;
			g_dlgWork.Enable_UserInput(nLs2WorkPort, FALSE);
			g_objCommon.Move_Position(AX_LOAD_STAGE2_Z, 2);	// Support Up
			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(10000);
		}
		break;
	case 5:		// Port Support Out
		if (g_objCommon.Check_Position(AX_LOAD_STAGE2_Z, 2)) {
			m_tLoadStage2Loop.Takt_Save(3, 1);
			m_tLoadStage2Loop.Takt_Start();
			g_objCommon.Set_LoadPortSupportOut(nLs2WorkPort);
			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 6:		// Tray Z Move to Support Down Position
		if (g_objCommon.Get_LoadPortSupportOut(nLs2WorkPort)) {
			m_tLoadStage2Loop.Takt_Save(3, 2);
			m_tLoadStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_LOAD_STAGE2_Z, 3);	// Support Down
			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(10000);
		}
		break;
	case 7:		// 정보전달, Port Support In
		if (g_objCommon.Check_Position(AX_LOAD_STAGE2_Z, 3)) {
			m_tLoadStage2Loop.Takt_Save(3, 3);
			m_tLoadStage2Loop.Takt_Start();
			if (m_pEquipData->bUseInlineMode) {
				gData.nPNoLoadTray[1] = gData.nPNoLoadPort;
				gData.nTNoLoadTray[1] = gData.nTNoLoadPort;
				gData.nLoadTrayCount[nLs2AviPort-1] = gData.nTNoLoadPort;
				gData.nCmCntLoadStage[1] = gData.nCmCntLoadPort;

				//PC Information Log 파일 생성
				CString strLog, strCPU, strRAM, strHardDisk;
				g_objCommon.Get_CPUInfo(strCPU);
				g_objCommon.Get_RAMSize(strRAM);
				g_objCommon.Get_HardInfo(strHardDisk);

				strLog.Format("%s,%s,%s,%s,%s",gData.sLotID[gData.nPNoLoadTray[1]-1],gData.sRecipe, strCPU,strRAM,strHardDisk);
				g_objLogFile.Save_PCLog(gData.nPNoLoadTray[1], strLog);

			} else {
				gData.nPNoLoadTray[1] = nLs2WorkPort;
			}
			g_objCommon.Set_LoadPortSupportIn(nLs2WorkPort);
			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 8:		// Tray Z Move to Move Up Position
		if (g_objCommon.Get_LoadPortSupportIn(nLs2WorkPort) && m_pDX04->iLoadStage2Exist) {
			m_tLoadStage2Loop.Takt_Save(3, 4);
			m_tLoadStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_LOAD_STAGE2_Z, 1);	// Move Up (Work)
			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(10000);
		}
		break;
	case 9:		// Tray Master In
		if (g_objCommon.Check_Position(AX_LOAD_STAGE2_Z, 1) && m_pDX04->iLoadStage2Exist) {
			m_tLoadStage2Loop.Takt_Save(3, 5);
			m_tLoadStage2Loop.Takt_Start();

			m_bLoadPortTrayExist = FALSE;	// Port에 Tray가 없다고 알려준다.

			m_pDY04->oLoadStage2MasterIn = TRUE;
			g_objAJinAXL.Write_Output(4);
			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 10:	// Tray Slave In
		if (m_pDX04->iLoadStage2MasterIn && !m_pDX04->iLoadStage2MasterOut) {
			m_pDY04->oLoadStage2SlaveIn = TRUE;
			g_objAJinAXL.Write_Output(4);
			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 11:	// Check Tray Master/Slave In
		if (g_objCommon.Get_LoadStageMasterSlaveIn(2) && m_pDX04->iLoadStage2Exist) {
			m_tLoadStage2Loop.Takt_Save(3, 6);
			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(5000);
		}
		break;

	case 12:	// 안전확인
		if (m_nLoadStage1Case > 22) {
			m_nLoadStage2Case = 15; m_tLoadStage2Loop.Set_LoopTime(5000);
		}
		return TRUE;

	case 13:	// X Move to Load2 Position
		if (g_objCommon.Check_Position(AX_LOAD_STAGE2_Z, 1)) {
			g_objCommon.Move_Position(AX_LOAD_STAGE2_X, 1);	// Port2
			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 14:	// Position Check
		if (g_objCommon.Check_Position(AX_LOAD_STAGE2_X, 1)) {
			if (m_nLoadStage1Case > 22) {
				m_nLoadStage2Case = 15; m_tLoadStage2Loop.Set_LoopTime(5000);
			}
			return TRUE;			
		}
		break;

	case 15:	// Tray X Move to Work Position, Tray Init
		if (g_objCommon.Check_Position(AX_LOAD_STAGE2_Z, 1)) {
			m_tLoadStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_LOAD_STAGE2_X, 2);	// Work
			if (!m_pEquipData->bUseInlineMode) {
				gData.nLoadTrayCount[nLs2WorkPort-1]++;
				gData.nTNoLoadTray[1] = gData.nLoadTrayCount[nLs2WorkPort-1];
			}
			Init_LoadTray(nLs2AviPort, 2);
			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(20000);
		}
		break;
	case 16:	// Position Check
		if (g_objCommon.Check_Position(AX_LOAD_STAGE2_X, 2)) {
			
			if(gData.nLoadTrayCount[nLs2AviPort-1] == 1) 
			{
				g_dlgWork.Enable_UserInput(nLs2WorkPort, FALSE);				
				Job_LotStart(nLs2AviPort);
				m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(30000);
			} 
			else
			{
				m_nLoadStage2Case = 20; m_tLoadStage2Loop.Set_LoopTime(10000);
			}
			m_tLoadStage2Loop.Takt_Save(3, 7);
		}
		break;
	case 17:	// Check Lot Ready
		if (g_objInspector.Check_LotReady()) 
		{
			gData.sInspectCmLotIDLater = gData.sLotID[nLs2AviPort-1];
			if (gData.nInspectCmLotCount < m_pEquipData->nInspectCmLotTimes) 
			{				
				gData.nInspectCmCheckTime = 0;
				gData.bInspectCmThisLotVSkip = FALSE;
			}
			m_nLoadStage2Case = 20; m_tLoadStage2Loop.Set_LoopTime(5000);
		}
		break;

	case 20:	// Working
		return TRUE;


	case 21:	// 안전 확인 
		if (m_nLoadStage1Case > 51 || m_nLoadStage1Case <= 20) 
		{
			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(5000);
		}
		return TRUE;
	case 22:	// X Move to Unload Port Position
		if (g_objCommon.Check_Position(AX_LOAD_STAGE2_Z, 1)) {
			m_tLoadStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_LOAD_STAGE2_X, 3);
			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(10000);
		}
		break;
	case 23:	// Z Move to Unload Up Position
		if (g_objCommon.Check_Position(AX_LOAD_STAGE2_X, 3) && m_pDX01->iLoadPort3SlideClose) {
			m_tLoadStage2Loop.Takt_Save(3, 8);
			m_tLoadStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_LOAD_STAGE2_Z, 4);	// Unload Up
			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(10000);
		}
		break;
	case 24:	// Slave Out
		if (g_objCommon.Check_Position(AX_LOAD_STAGE2_Z, 4)) {
			m_tLoadStage2Loop.Takt_Save(3, 9);
			m_tLoadStage2Loop.Takt_Start();
			m_pDY04->oLoadStage2SlaveIn = FALSE;
			g_objAJinAXL.Write_Output(4);
			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 25:	// Master Out
		if (!m_pDX04->iLoadStage2SlaveIn && m_pDX04->iLoadStage2SlaveOut) {
			m_pDY04->oLoadStage2MasterIn = FALSE;
			g_objAJinAXL.Write_Output(4);
			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 26:	// Z Axis Move to Move Down Position
		if (g_objCommon.Get_LoadStageMasterSlaveOut(2)) {
			m_tLoadStage2Loop.Takt_Save(3, 10);
			m_tLoadStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_LOAD_STAGE2_Z, 0);	// Move Down
			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(10000);
		}
		break;
	case 27:	// Position Check
		if (g_objCommon.Check_Position(AX_LOAD_STAGE2_Z, 0)) {
			m_tLoadStage2Loop.Takt_Save(3, 11);
			m_nLoadStage2Case = 50; m_tLoadStage2Loop.Set_LoopTime(5000);

			m_strLog.Format("LoadTray2, %d", GetTickCount() - m_dwLoadStage2);
			g_objLogFile.Save_TestLog(m_strLog);
		}
		break;


	case 50:	//	Interlock
		if (m_nLoadStage1Case > 11 && m_nLoadStage1Case < 50) {
			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(5000);
		}
		return TRUE;

	case 51:	// Tray X Move to Load1 Position
		if (g_objCommon.Check_Position(AX_LOAD_STAGE2_Z, 0)) {
			if (g_objCommon.Check_Position(AX_LOAD_STAGE1_Z, 0)) break;	// 인터락
			m_tLoadStage2Loop.Takt_Start();
			if (m_pEquipData->bUseInlineMode) {
				nLs2WorkPort = 1;
			} else {	//앞에 작업 중인 Port 번호를 받는다.
				if (nLs2WorkPort == 0) nLs2WorkPort = gData.nPNoLoadTray[0];
				if (nLs2AviPort == 0) nLs2AviPort = gData.nPNoLoadTray[0];
			}

			if (nLs2WorkPort == 1) { g_objCommon.Move_Position(AX_LOAD_STAGE2_X, 0); }	// Port1
			if (nLs2WorkPort == 2) { g_objCommon.Move_Position(AX_LOAD_STAGE2_X, 1); }	// Port2

			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(10000);
		}
		break;
	case 52:	// Check End or Go
		if ((nLs2WorkPort == 1 && g_objCommon.Check_Position(AX_LOAD_STAGE2_X, 0)) ||
			(nLs2WorkPort == 2 && g_objCommon.Check_Position(AX_LOAD_STAGE2_X, 1)))
		{
			m_tLoadStage2Loop.Takt_Save(3, 12);
			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(10000);			
		}
		break;
	case 53:	// 안전확인
		if (m_nLoadStage1Case >= 20) {
			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(10000);
		}
		return TRUE;
	case 54:	// Z Move to Move Up Position 
		if ((nLs2WorkPort == 1 && g_objCommon.Check_Position(AX_LOAD_STAGE2_X, 0)) ||
			(nLs2WorkPort == 2 && g_objCommon.Check_Position(AX_LOAD_STAGE2_X, 1)))
		{
			g_objCommon.Move_Position(AX_LOAD_STAGE2_Z, 1);	// Move Up
			m_nLoadStage2Case++; m_tLoadStage2Loop.Set_LoopTime(10000);
		}
		break;
	case 55:	//
		if (g_objCommon.Check_Position(AX_LOAD_STAGE2_Z, 1)) {
			if (m_pEquipData->bUseInlineMode) {	// 인라인 모드는 무조건 Port 1번으로 가고 case도 1번에서 새로 시작한다.
				m_nLoadStage2Case = 1; m_tLoadStage2Loop.Set_LoopTime(10000);
			} else {							// 단독 모드는 연속랏이 되어야하기 때문에 Port확인 해준다.
				if (gData.nLoadTrayCount[nLs2WorkPort-1] >= gData.nTrayUseCount[nLs2WorkPort-1]) {
					m_bLotLoadEnable[nLs2WorkPort-1] = TRUE;	// Port에서 트레이 로드 완료
// 					m_bLoadLotEnd = TRUE;	//temp 연속랏은 좀더 생각해보자.
					if (gData.nTNoLoadTray[1] >= gData.nTrayUseCount[nLs2WorkPort-1]) gData.nLoadTrayCount[nLs2WorkPort-1] = 0;

					if (nLs2WorkPort == 1) {
						m_pDY00->oLoadPort1SlideLock = FALSE; m_pDY00->oLoadPort1SlideUnlock = TRUE; 
						//m_pDY13->oDoor02Unlock = TRUE; 
						nLs2WorkPort = 2;
						nLs2AviPort = 2;

					} else if (nLs2WorkPort == 2) {
						m_pDY01->oLoadPort2SlideLock = FALSE; m_pDY01->oLoadPort2SlideUnlock = TRUE; 
						//m_pDY13->oDoor03Unlock = TRUE; 
						nLs2WorkPort = 1;
						nLs2AviPort = 1;
					}
					g_objAJinAXL.Write_Output(0);
					g_objAJinAXL.Write_Output(1);

					m_nLoadStage2Case = 60; m_tLoadStage2Loop.Set_LoopTime(10000);
// 					m_nLoadStage2Case = 0; m_tLoadStage2Loop.Set_LoopTime(10000);
				} else {
					m_nLoadStage2Case = 2; m_tLoadStage2Loop.Set_LoopTime(10000);
				}
			}
		}
		break;
	case 60:	// Wait, Port Select
		if (gData.nLoadTrayCount[nLs2WorkPort-1] == 0 && !Check_LoadTrayLoading(nLs2WorkPort)) return TRUE;	// 같은 Port에서 연속으로 2Lot은 검사할 수 없다.

		if ((nLs2WorkPort == 1 && m_pDX00->iLoadPort1LowCheck && m_bLotLoadEnable[0] == FALSE) ||
			(nLs2WorkPort == 2 && m_pDX01->iLoadPort2LowCheck && m_bLotLoadEnable[1] == FALSE)) {
			g_dlgWork.Get_LotInfo(nLs2WorkPort-1);
			m_nLoadStage2Case = 2; m_tLoadStage2Loop.Set_LoopTime(10000);

		} else {
			if (m_bUnloadLotEnd) {
				m_nLoadStage2Case = 0; m_tLoadStage2Loop.Set_LoopTime(10000);	// 작업 종료
			}
		}		
		return TRUE;
	}

	// 3. (Error : 3300)
	if (m_tLoadStage2Loop.Over_LoopTime()) {
		g_objCommon.Show_Error(3300 + m_nLoadStage2Case);
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 4. (Error : 3400)
BOOL CSequenceMain::LoadPicker_Run()
{
	static int nLpWorkTray = 0;
	static int nLpTrayPosY = 0;
	static int nLpTrayPosX = 0;

	static double dLpY = 0.0;
	static double dLpX = 0.0;

	static int nLpStart = 0;		// Picker Start No
	static int nLpMultCnt = 0;		// 최종적으로 Picker가 Down되는 수

	static BOOL bLpTrayEmpty = FALSE;
	static int nPNoTemp = 1;

	switch (m_nLoadPickCase) {
	case 0:		// Wait
		if (m_nMainIndexCase <= 10 && !gData.IndexDone[0]) { m_nLoadPickCase++; m_tLoadPickLoop.Set_LoopTime(5000); }
		return TRUE;

	case 1:		// Tray가 Work 위치에 오면 시작
		if (m_nLoadStage1Case == 20 || m_nLoadStage2Case == 20) { m_nLoadPickCase++; m_tLoadPickLoop.Set_LoopTime(5000); }

		if (m_pEquipData->bUseInlineMode) {	// Index에 모듈이 있을때 Tray가 안넘어오면 Index는 돌아가게 해준다.
			if (((m_nLoadStage1Case == 0 || m_nLoadStage1Case == 1) && m_nLoadStage2Case == 50) ||
				((m_nLoadStage2Case == 0 || m_nLoadStage2Case == 1) && m_nLoadStage1Case == 50)) 
			{
				if (!Check_IndexEmpty(-1) && m_nVisionCmCase == 0) 
				{
					gData.IndexDone[0] = TRUE;
					m_nLoadPickCase = 0;
				}
			}
		} else {
			if (((m_nLoadStage1Case == 0 || m_nLoadStage1Case == 60) && m_nLoadStage2Case == 50) ||
				((m_nLoadStage2Case == 0 || m_nLoadStage2Case == 60) && m_nLoadStage1Case == 50)) {
				if (!gData.IndexDone[0] && m_nVisionCmCase == 0) {
					gData.IndexDone[0] = TRUE;
					m_nLoadPickCase = 0;
				}
			}
		}
		return TRUE;

	case 2:		// Picker Y and P Move to Tray Position
		if (g_objCommon.Get_LoadPickerUp() && g_objCommon.Check_Position(AX_LOAD_PICKER_Z, 0)) {
			if (m_nLoadStage1Case != 20 && m_nLoadStage2Case != 20) return TRUE;

			if ((m_nLoadStage1Case == 20 && (g_objAJinAXL.Is_MoveDone(AX_LOAD_STAGE1_X, dLpX) || g_objCommon.Check_Position(AX_LOAD_STAGE1_X, 2))) ||
				(m_nLoadStage2Case == 20 && (g_objAJinAXL.Is_MoveDone(AX_LOAD_STAGE2_X, dLpX) || g_objCommon.Check_Position(AX_LOAD_STAGE2_X, 2)))) {
				if (m_nLoadStage1Case == 20) nLpWorkTray = 1;
				if (m_nLoadStage2Case == 20) nLpWorkTray = 2;

				int nPickCnt = 0, nTrayCnt = 0;
				if (Select_LoadPickerPos(nLpStart, nPickCnt) && Select_LoadTrayPos(nLpTrayPosX, nLpTrayPosY)) {
					m_dwLoadPick = GetTickCount();
					m_tLoadPickLoop.Takt_Start();
					nTrayCnt = LT_X - nLpTrayPosX;
					int nCount = (nPickCnt < nTrayCnt ? nPickCnt : nTrayCnt);
					nLpMultCnt = g_objCommon.Get_InfoLoadPickerDownCnt(nLpTrayPosX, nLpTrayPosY, nCount);

					if (nLpWorkTray == 1) dLpX = m_pMoveData->dLoadStage1X[2] - (nLpTrayPosX - nLpStart) * m_pEquipData->dLoadTrayPitchX;
					if (nLpWorkTray == 2) dLpX = m_pMoveData->dLoadStage2X[2] - (nLpTrayPosX - nLpStart) * m_pEquipData->dLoadTrayPitchX;
					dLpY = m_pMoveData->dLoadPickerY[nLpWorkTray-1] - (m_pEquipData->dLoadTrayPitchY * nLpTrayPosY);

					if (nLpWorkTray == 1) g_objAJinAXL.Move_Absolute(AX_LOAD_STAGE1_X, dLpX);
					if (nLpWorkTray == 2) g_objAJinAXL.Move_Absolute(AX_LOAD_STAGE2_X, dLpX);
					g_objAJinAXL.Move_Absolute(AX_LOAD_PICKER_Y, dLpY);
					g_objCommon.Move_Position(AX_LOAD_PICKER_P, 0);
					m_nLoadPickCase++; m_tLoadPickLoop.Set_LoopTime(10000);
				}
			}
		}
		break;
	case 3:		// Z Axis Move to Tray Down
		if (g_objAJinAXL.Is_MoveDone(AX_LOAD_PICKER_Y, dLpY) && g_objCommon.Check_Position(AX_LOAD_PICKER_P, 0)) 
		{
			if ((nLpWorkTray == 1 && g_objAJinAXL.Is_MoveDone(AX_LOAD_STAGE1_X, dLpX)) ||
				(nLpWorkTray == 2 && g_objAJinAXL.Is_MoveDone(AX_LOAD_STAGE2_X, dLpX))) {
				if (g_objCommon.Get_InfoLoadPickerGripOpen()) 
				{
					m_tLoadPickLoop.Takt_Save(4, 1);
					m_tLoadPickLoop.Takt_Start();

					g_objCommon.Set_LoadPickerDownMulti(nLpStart+1, nLpMultCnt);
					g_objCommon.Move_Position(AX_LOAD_PICKER_Z, nLpWorkTray);
					m_nLoadPickCase++; m_tLoadPickLoop.Set_LoopTime(10000);
				}
			}
		}
		break;
	case 4:		// 정보전달 Picker Grip
		if (g_objCommon.Check_Position(AX_LOAD_PICKER_Z, nLpWorkTray) && g_objCommon.Get_LoadPickerDownMulti(nLpStart+1, nLpMultCnt)) {
			if (!m_tLoadPickLoop.Waiting_Time(m_pEquipData->nDelayAdd[0])) break;

			m_tLoadPickLoop.Takt_Save(4, 2);
			m_tLoadPickLoop.Takt_Start();
			
			for (int i = 0; i < nLpMultCnt; i++) {
				gData.InfoLoadPick[nLpStart+i] = gData.InfoLoadTray[nLpTrayPosY][nLpTrayPosX+i]; gData.InfoLoadTray[nLpTrayPosY][nLpTrayPosX+i] = 0;
				if (gData.InfoLoadPick[nLpStart+i] > 0) {
					gData.nCNoLoadPick[nLpStart+i] = nLpTrayPosY * LT_X + nLpTrayPosX + i + 1; 
					gData.nTNoLoadPick[nLpStart+i] = gData.nTNoLoadTray[nLpWorkTray-1];

					//모듈트랙킹
					int nPNo = gData.nPNoLoadTray[nLpWorkTray-1] - 1;
					int nTNo = gData.nTNoLoadTray[nLpWorkTray-1] - 1;
					int nCNo = gData.nCNoLoadPick[i] - 1;
					gData.nCmJigNo[nPNo][nTNo][nCNo][LOAD_STAGE] = nLpWorkTray;		 // Stage No.
					gData.nCmJigNo[nPNo][nTNo][nCNo][LOAD_PICK]  = nLpStart + i + 1; // Btm1 Jig No.
				}
			}
			gData.nPNoLoadPick = gData.nPNoLoadTray[nLpWorkTray-1];	
			nPNoTemp = gData.nPNoLoadPick;
			g_dlgWork.PostMessage(UM_UPDATE_TRAY_INFO, 1, nLpWorkTray-1);

			g_objCommon.Set_InfoLoadPickerGrip();
			m_nLoadPickCase++; m_tLoadPickLoop.Set_LoopTime(5000);			
		}
		break;
	case 5:		// Picker Up
		if (g_objCommon.Get_InfoLoadPickerGrip()) {
			if (!m_tLoadPickLoop.Waiting_Time(m_pEquipData->nDelayAdd[0])) break;

			m_tLoadPickLoop.Takt_Save(4, 3);
			m_tLoadPickLoop.Takt_Start();
			g_objCommon.Set_LoadPickerUp();
			g_objCommon.Move_Position(AX_LOAD_PICKER_Z, 0);
			m_nLoadPickCase++; m_tLoadPickLoop.Set_LoopTime(5000);
		}
		break;
	case 6:		// CM Check & Tray Empty Check
		if (g_objCommon.Check_Position(AX_LOAD_PICKER_Z, 0) && g_objCommon.Get_LoadPickerUp() && g_objCommon.Get_InfoLoadPickerCmCheck()) {
			m_tLoadPickLoop.Takt_Save(4, 4);
			bLpTrayEmpty = Check_LoadTrayEmpty();

			if (Check_LoadPickerFull() || bLpTrayEmpty) {
				if (Check_LoadPickerFull()) {
					m_nLoadPickCase = 10; m_tLoadPickLoop.Set_LoopTime(5000);
				} else {
					int nPNo = gData.nPNoLoadTray[nLpWorkTray-1];
					if (m_pEquipData->bUseInlineMode) {	// Inline Mode : 트레이가 비어있고 마지막 트레이면 Index로 넘어간다.
						if (bLpTrayEmpty && (gData.nTNoLoadTray[nLpWorkTray-1] == gData.nLastTrayNo[nPNo-1])) {
							m_nLoadPickCase = 10; m_tLoadPickLoop.Set_LoopTime(5000);
						} else {
							m_nLoadPickCase = 2; m_tLoadPickLoop.Set_LoopTime(5000);
						}
					} else {	// 단독 모드 : 트레이가 비어있고 마지막 트레이면 Index로 넘어간다.
						if (bLpTrayEmpty && (gData.nLoadTrayCount[nPNo-1] >= gData.nTrayUseCount[nPNo-1])) {
							m_nLoadPickCase = 10; m_tLoadPickLoop.Set_LoopTime(5000);
						} else {
							m_nLoadPickCase = 2; m_tLoadPickLoop.Set_LoopTime(5000);
						}
					}
				}
				if (bLpTrayEmpty) {
					if (nLpWorkTray == 1) m_nLoadStage1Case = 21;	// Unloading
					if (nLpWorkTray == 2) m_nLoadStage2Case = 21;	// Unloading
					int nPNo = gData.nPNoLoadTray[nLpWorkTray-1];

					gData.nPNoLoadTray[nLpWorkTray-1] = 0;
				}
			} else {
				m_nLoadPickCase = 2; m_tLoadPickLoop.Set_LoopTime(5000);
			}
		}
		break;

	case 10:	// Interlock Vision CM
		if (m_nVisionCmCase == 0 && g_objCommon.Check_Position(AX_VISION_CM_X, 0)) {
			m_nLoadPickCase++; m_tLoadPickLoop.Set_LoopTime(5000);
		}
		return TRUE;

	case 11:	// Y/P Move to Index Position
		if (g_objCommon.Check_Position(AX_LOAD_PICKER_Z, 0) && g_objCommon.Get_InfoLoadPickerCmCheck()) {
			m_tLoadPickLoop.Takt_Start();
			g_objCommon.Move_Position(AX_LOAD_PICKER_Y, 2);	// Index Position
			g_objCommon.Move_Position(AX_LOAD_PICKER_P, 1);	// Index Pitch
			m_nLoadPickCase++; m_tLoadPickLoop.Set_LoopTime(5000);
		}
	case 12:	// Position Check
		if (g_objCommon.Get_InfoLoadPickerCmCheck() && g_objCommon.Check_Position(AX_LOAD_PICKER_Y, 2) && g_objCommon.Check_Position(AX_LOAD_PICKER_P, 1)) {
			m_tLoadPickLoop.Takt_Save(4, 5);
			m_nLoadPickCase = 15; m_tLoadPickLoop.Set_LoopTime(5000);
		}
		break;

	case 15:	// index Check		
		if ((m_nMainIndexCase == 2 || m_nMainIndexCase == 5) && !gData.IndexDone[0]) {
			m_nLoadPickCase++; m_tLoadPickLoop.Set_LoopTime(5000);
		}
		return TRUE;

	case 16:	// Index Down
		if (g_objCommon.Check_Position(AX_LOAD_PICKER_Y, 2) && g_objCommon.Check_Position(AX_LOAD_PICKER_P, 1) &&
			g_objCommon.Get_InfoLoadPickerCmCheck() && !m_pDX11->iIndexLoadAlignIn && m_pDX11->iIndexLoadAlignOut) {
			m_tLoadPickLoop.Takt_Start();
			g_objCommon.Set_InfoLoadPickerDown();			
			g_objCommon.Move_Position(AX_LOAD_PICKER_Z, 3);	// Index Down
			m_nLoadPickCase++; m_tLoadPickLoop.Set_LoopTime(10000);
		}
		break;
	case 17:	// 정보전달, Grip Open
		if (g_objCommon.Check_Position(AX_LOAD_PICKER_Z, 3) && g_objCommon.Get_InfoLoadPickerDown() ) {
			//if (!m_tLoadPickLoop.Waiting_Time(100)) break;
			m_tLoadPickLoop.Takt_Save(4, 6);
			m_tLoadPickLoop.Takt_Start();

			for (int i = 0; i < PICK; i++) {
				gData.InfoIndex[0][i] = gData.InfoLoadPick[i]; gData.InfoLoadPick[i] = 0;
				gData.nCNoIndex[0][i] = gData.nCNoLoadPick[i]; gData.nCNoLoadPick[i] = 0;
				gData.nTNoIndex[0][i] = gData.nTNoLoadPick[i]; gData.nTNoLoadPick[i] = 0;

				//모듈트랙킹
				int nPNo = gData.nPNoLoadPick - 1;
				int nTNo = gData.nTNoIndex[0][i] - 1;
				int nCNo = gData.nCNoIndex[0][i] - 1;

				int nIdxNo = g_objCommon.Get_MainIndexPos(1);	// Load
				gData.nCmJigNo[nPNo][nTNo][nCNo][INDEX_LOAD_NO]	 = nIdxNo;	// Index No
				gData.nCmJigNo[nPNo][nTNo][nCNo][INDEX_LOAD_JIG] = i + 1;	// Index Jig
			}
			gData.nPNoIndex[0] = gData.nPNoLoadPick; gData.nPNoLoadPick = 0;
			
			g_objCommon.Set_LoadPickerOpen(0);
			g_objCommon.Set_InfoIndexLoadVacuumOn();
			
			m_nLoadPickCase++; m_tLoadPickLoop.Set_LoopTime(5000);
		}
		break;
	case 18:	// Picker Up
		if (g_objCommon.Get_LoadPickerOpen(0) && g_objCommon.Get_InfoIndexLoadVacuumOn() ) {
			if (!m_tLoadPickLoop.Waiting_Time(m_pEquipData->nDelayAdd[0])) break;
			m_tLoadPickLoop.Takt_Save(4, 7);
			m_tLoadPickLoop.Takt_Start();
			g_objCommon.Set_LoadPickerUp();
			g_objCommon.Set_IndexLoadVacuumOff(0);
			g_objCommon.Move_Position(AX_LOAD_PICKER_Z, 0);	// Ready Up
			
			m_nLoadPickCase++; m_tLoadPickLoop.Set_LoopTime(5000);
		}
		break;
	case 19:	// Align Out
		if (g_objCommon.Check_Position(AX_LOAD_PICKER_Z, 0) && g_objCommon.Get_LoadPickerUp() 
			&& g_objCommon.Get_LoadPickerCmCheckOff() && g_objCommon.Get_IndexLoadVacuumOff(0))
		{			
			m_pDY11->oIndexLoadAlignOut = FALSE;
			g_objAJinAXL.Write_Output(11);
			if (m_nVisionCmCase == 0) { m_nVisionCmCase = 1; m_tVisionCmLoop.Set_LoopTime(5000); }

			
			m_nLoadPickCase++; m_tLoadPickLoop.Set_LoopTime(5000);
		}
		break;
	case 20:	// return
		if (g_objCommon.Check_Position(AX_LOAD_PICKER_Z, 0) && g_objCommon.Get_LoadPickerUp() && m_pDX11->iIndexLoadAlignIn && !m_pDX11->iIndexLoadAlignOut) 
		{
			m_tLoadPickLoop.Takt_Save(4, 8);
			//g_objCommon.Set_InfoIndexLoadVacuumOn();
			
			if (bLpTrayEmpty) 
			{
				if ((m_nLoadStage1Case == 20 && m_nLoadStage2Case >= 50) || (m_nLoadStage2Case == 20 && m_nLoadStage1Case >= 50)) 
				{
					m_nLoadPickCase = 1; m_tLoadPickLoop.Set_LoopTime(5000);	// Picker와 Tray를 동시에 이동하기 위해...
				}
				else
				{
					m_tLoadPickLoop.Takt_Start();
					nLpWorkTray = (nLpWorkTray == 1 ? 2 : 1);	// 교체
					dLpY = m_pMoveData->dLoadPickerY[nLpWorkTray-1];
					g_objAJinAXL.Move_Absolute(AX_LOAD_PICKER_Y, dLpY);	// Stage Position
					g_objCommon.Move_Position(AX_LOAD_PICKER_P, 0);		// Stage Pitch
					m_nLoadPickCase++; m_tLoadPickLoop.Set_LoopTime(10000);
				}
			} 
			else
			{
				m_nLoadPickCase = 2; m_tLoadPickLoop.Set_LoopTime(5000);
			}
		}
		break;
	case 21:	//Position Check & Vision Start
		if (g_objAJinAXL.Is_MoveDone(AX_LOAD_PICKER_Y, dLpY) && g_objCommon.Check_Position(AX_LOAD_PICKER_P, 0)) {
				
			m_tLoadPickLoop.Takt_Save(4, 9);

			m_strLog.Format("LoadPicker, %d", GetTickCount() - m_dwLoadPick);
			g_objLogFile.Save_TestLog(m_strLog);

			m_nLoadPickCase = 1; m_tLoadPickLoop.Set_LoopTime(5000);
		}
		break;
	}

	// 4. (Error : 3400)
	if (m_tLoadPickLoop.Over_LoopTime()) {
		g_objCommon.Show_Error(3400 + m_nLoadPickCase);
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 5. (Error : 3500)
BOOL CSequenceMain::MainIndex_Run()
{
	static int nIndexVacuumRetry = 0;

	for (int i = 0; i < 3; i++) if (!gLot.bTaktDone[i]) Set_TaktLog(i);

	switch (m_nMainIndexCase) {
	case 0:
		if (Check_IndexEmpty(-1) && Check_LoadPickerEmpty()) return TRUE;

		if (m_nLoadPickCase == 0 &&
			(((m_nLoadStage1Case == 0 || m_nLoadStage1Case == 1 || m_nLoadStage1Case == 60) && m_nLoadStage2Case == 50) ||
			 ((m_nLoadStage2Case == 0 || m_nLoadStage2Case == 1 || m_nLoadStage2Case == 60) && m_nLoadStage1Case == 50)))
		{
			m_nMainIndexCase = 10; m_tMainIndexLoop.Set_LoopTime(5000);
		} else if (m_nLoadPickCase > 4 && m_nLoadPickCase <= 15) {	// Loading
			m_nMainIndexCase++; m_tMainIndexLoop.Set_LoopTime(5000);
		}
		return TRUE;

	case 1:	// Index Align1 Out & Vacuum Pad Up
		m_pDY11->oIndexLoadAlignOut = TRUE;
		m_pDY11->oIndexLoadVacUp = TRUE;
		g_objAJinAXL.Write_Output(11);
		m_nMainIndexCase = 2; m_tMainIndexLoop.Set_LoopTime(5000);
		break;

	case 2:	// Check Index Align1 Out & Vacuum Pad Up
		if (!m_pDX11->iIndexLoadAlignIn && m_pDX11->iIndexLoadAlignOut && m_pDX11->iIndexLoadVacUp && !m_pDX11->iIndexLoadVacDown) {
			m_nMainIndexCase = 5; m_tMainIndexLoop.Set_LoopTime(5000);
		}
		break;

	case 5:	// Wait for Load Picker Done
		if (m_nLoadPickCase > 19) 
		{	// LoadPicker Up 동작 후
			if (!m_tMainIndexLoop.Delay_LoopTime(100)) break;
			nIndexVacuumRetry = 0;	// Clear
			//g_objCommon.Set_InfoIndexLoadVacuumOn();
			m_nMainIndexCase = 6; m_tMainIndexLoop.Set_LoopTime(5000);
		}
		return TRUE;

	case 6:	// Retry (Align Out) or Vacuum Off
		if (m_pDX11->iIndexLoadAlignIn && !m_pDX11->iIndexLoadAlignOut) {
			if(1)//if (g_objCommon.Get_InfoIndexLoadVacuumOn()) 
			{
				g_objCommon.Set_IndexLoadVacuumOff(0);
				m_nMainIndexCase = 8; m_tMainIndexLoop.Set_LoopTime(5000);
			} 
			else
			{
				if (nIndexVacuumRetry < 1) {	// Retry 1회
					if (!m_tMainIndexLoop.Delay_LoopTime(1000)) break;	// 1초 기다리고 Retry (Align1 Out)
					m_pDY11->oIndexLoadAlignOut = TRUE;
					g_objAJinAXL.Write_Output(11);
					m_nMainIndexCase = 7; m_tMainIndexLoop.Set_LoopTime(5000);
				}
			}
		}
		break;
	case 7:	// Align1 In (Retry)
		if (!m_pDX11->iIndexLoadAlignIn && m_pDX11->iIndexLoadAlignOut) {
			nIndexVacuumRetry++;
			m_pDY11->oIndexLoadAlignOut = FALSE;
			g_objAJinAXL.Write_Output(11);
			m_nMainIndexCase = 6; m_tMainIndexLoop.Set_LoopTime(5000);
		}
		break;
	case 8:	// Vacuum Pad Down
		if (g_objCommon.Get_IndexLoadVacuumOff(0)) {
			m_pDY11->oIndexLoadVacUp = FALSE;
			g_objAJinAXL.Write_Output(11);
			m_nMainIndexCase = 9; m_tMainIndexLoop.Set_LoopTime(5000);
		}
		break;
	case 9:	// Check Vacuum Pad Down
		if (!m_pDX11->iIndexLoadVacUp && m_pDX11->iIndexLoadVacDown) {
			m_nMainIndexCase = 10; m_tMainIndexLoop.Set_LoopTime(30000);
		}
		break;

	case 10:	// Index R Move 90 Degree		
		if (gData.IndexDone[0] && gData.IndexDone[1] && gData.IndexDone[2] ) 
		{			
			if (m_pDX11->iIndexLoadAlignIn && !m_pDX11->iIndexLoadAlignOut && m_pDX11->iIndexAssyAlignIn && !m_pDX11->iIndexAssyAlignOut &&
				m_pDX11->iIndexTransAlignIn && !m_pDX11->iIndexTransAlignOut && !m_pDX11->iIndexLoadVacUp && m_pDX11->iIndexLoadVacDown &&
				!m_pDX11->iIndexAssyVacUp && m_pDX11->iIndexAssyVacDown) {

				if (Check_IndexEmpty(-1)) 
				{ 
					g_objLogFile.Save_TestLog("gData.IndexDone[0] = FALSE 1");
					gData.IndexDone[0] = FALSE;
					m_nMainIndexCase = 0; 
					return TRUE;
				}

				if ((g_objAJinAXL.Get_Position(AX_LOAD_PICKER_Y) <= 200.0 || g_objCommon.Get_LoadPickerUp()) &&
					(g_objAJinAXL.Get_Position(AX_ASSY_PICKER_X) <= 200.0 || g_objCommon.Get_AssyPickerUp(0)) &&
					(g_objAJinAXL.Get_Position(AX_TRANS_STAGE_X) >= 200.0 || g_objCommon.Check_Position(AX_TRANS_STAGE_Z, 0))) 
				{
					g_objAJinAXL.Move_Relative(AX_MAIN_INDEX_R, m_pMoveData->dMainIndexR[0]);
					m_nMainIndexCase = 30; m_tMainIndexLoop.Set_LoopTime(10000);
				}
			}
		}
		return TRUE;
	case 30:
		if( m_nVisionCmCase ==0 && Check_CmAlignDone())
		{
			m_nMainIndexCase = 11; m_tMainIndexLoop.Set_LoopTime(10000);
		}
		break;
	case 11:	// Check R Movd Done and Index End
		if (g_objAJinAXL.Is_MoveDone(AX_MAIN_INDEX_R, m_pMoveData->dMainIndexR[0])) {
			g_objLogFile.Save_TestLog("Set_IndexEnd");
			Set_IndexEnd();
			m_nMainIndexCase = 0; m_tMainIndexLoop.Set_LoopTime(5000);
		}
		break;

	// Lot End Home
	case 21:		// Index R Home
		if (m_pDX11->iIndexLoadAlignIn && !m_pDX11->iIndexLoadAlignOut && m_pDX11->iIndexAssyAlignIn && !m_pDX11->iIndexAssyAlignOut &&
			m_pDX11->iIndexTransAlignIn && !m_pDX11->iIndexTransAlignOut && !m_pDX11->iIndexLoadVacUp && m_pDX11->iIndexLoadVacDown &&
			!m_pDX11->iIndexAssyVacUp && m_pDX11->iIndexAssyVacDown) {
			if ((g_objAJinAXL.Get_Position(AX_LOAD_PICKER_Y) <= 200.0 || g_objCommon.Get_LoadPickerUp()) &&
				(g_objAJinAXL.Get_Position(AX_ASSY_PICKER_X) <= 200.0 || g_objCommon.Get_AssyPickerUp(0)) &&
				(g_objAJinAXL.Get_Position(AX_TRANS_STAGE_X) >= 200.0 || g_objCommon.Check_Position(AX_TRANS_STAGE_Z, 0))) {
				g_objAJinAXL.Home_Search(AX_MAIN_INDEX_R);
				m_nMainIndexCase = 22; m_tMainIndexLoop.Set_LoopTime(10000);
			}
		}
		break;
	case 22:		// Check R Home Done
		if (g_objAJinAXL.Is_Home(AX_MAIN_INDEX_R)) {
			m_nMainIndexCase = 0;
		}
		break;
	}

	// 5. (Error : 3500)
	if (m_tMainIndexLoop.Over_LoopTime()) {
		g_objCommon.Show_Error(3500 + m_nMainIndexCase);
		return FALSE;
	} else {
		return TRUE;
	}
}

///////////////////////////////////////////////////////////////////////////////
// 6. (Error : 3600)
BOOL CSequenceMain::VisionCm_Run()
{
	static int nCmScanCnt = 0;	// 총 Scan 횟수 
	static int nCmScanNo = 0;	// 현재 Scan 번호

	static double dCmX = 0.0;

	switch (m_nVisionCmCase) {
	case 0:		// Load Picker에서 Start
		return TRUE;

	case 1:		// X Move Inspect Position
		if (g_objCommon.Check_Position(AX_VISION_CM_X, 0)) 
		{
			if ((m_pEquipData->bUseVisionCmAlign && (gData.nInsCmScanCntVolatile == 0 || m_pEquipData->nInspectCmScanTimes == 0)) 
				|| (m_pEquipData->bUseVisionCmAlign && CheckInspectCmGoOrNot(gData.nPNoIndex[0]) && !gData.bInspectCmThisLotVSkip) )
				
			{	
				double dPickPosY = g_objAJinAXL.Get_Position(AX_LOAD_PICKER_Y);
				double dStagePos = max(m_pMoveData->dLoadPickerY[0], m_pMoveData->dLoadPickerY[1]);
				if (dPickPosY > dStagePos + 1.0) return TRUE;	// 충돌 방지
					
				
				if(gData.nInsCmScanCntVolatile == 0 || m_pEquipData->nInspectCmScanTimes == 0)
				{
					//pass
				}
				else if((gData.nCmUseCount[gData.nPNoIndex[0]-1]/4) < m_pEquipData->nInspectCmScanTimes)
				{
					m_pEquipData->nInspectCmScanTimes = (gData.nCmUseCount[gData.nPNoIndex[0]-1]/4);
				}
				else
				{
					m_pEquipData->nInspectCmScanTimes = gData.nInsCmScanCntVolatile;
				}
				
						
				m_dwVisionCm = GetTickCount();
				m_tVisionCmLoop.Takt_Start();
				nCmScanCnt = 0;
				nCmScanNo = 0;
				dCmX = m_pMoveData->dVisionCmX[1];	// Inspect Position
				g_objAJinAXL.Move_Absolute(AX_VISION_CM_X, dCmX);
				m_nVisionCmCase++; m_tVisionCmLoop.Set_LoopTime(5000);

			} 
			else
			{	// Align 검사를 안하면 종료한다.
				gData.IndexDone[0] = TRUE;
				m_nVisionCmCase = 0; m_tVisionCmLoop.Set_LoopTime(5000);
			}
		}
		break;
	case 2:		// Load Complete Send
		if (g_objAJinAXL.Is_MoveDone(AX_VISION_CM_X, dCmX)) 
		{
			nCmScanCnt = Get_CmScanCnt();
			if (gData.bCmAlignSkip) 
			{
				gData.bCmAlignSkip = FALSE;
				nCmScanNo++;
				m_nVisionCmCase = 10; m_tVisionCmLoop.Set_LoopTime(30000);
				break;
			}

			m_tVisionCmLoop.Takt_Save(5, 1);
			if (nCmScanNo <= nCmScanCnt) 
			{
				m_tVisionCmLoop.Takt_Start();
				int nPNo = gData.nPNoIndex[0] - 1;
				int nTNo1 = gData.nTNoIndex[0][nCmScanNo+0];
				int nTNo2 = gData.nTNoIndex[0][nCmScanNo+2];
				int nCNo1 = gData.nCNoIndex[0][nCmScanNo+0];
				int nCNo2 = gData.nCNoIndex[0][nCmScanNo+2];
				gData.nCmInspPickNo1 = nCmScanNo + 1;
				gData.nCmInspPickNo2 = nCmScanNo + 3;

				gData.bCmAlignSkip = FALSE;
				if (m_pEquipData->bUseInlineMode) 
				{
					g_objInspector.Set_LoadComplete("T12", gLot.sLotID[nPNo], gData.nPNoIndex[0], nTNo1, nTNo2, nCNo1, nCNo2, gData.nCmInspPickNo1, gData.nCmInspPickNo2);
				} 
				else
				{
					g_objInspector.Set_LoadComplete("T12", gLot.sLotID[nPNo], gData.nPNoIndex[0], nTNo1, nTNo2, nCNo1, nCNo2, gData.nCmInspPickNo1, gData.nCmInspPickNo2);
				}				

				nCmScanNo++;
				m_nVisionCmCase = 5; m_tVisionCmLoop.Set_LoopTime(30000);

			} else {
				m_nVisionCmCase = 15; m_tVisionCmLoop.Set_LoopTime(5000);
			}
		}
		break;
	case 3:
		{
			if (gData.bReload[0]) {
				gData.bReload[0] = FALSE;
			}
			g_objCommon.Move_Position(AX_VISION_CM_X, 0);
			m_nVisionCmCase = 1; m_tVisionCmLoop.Set_LoopTime(5000);
		}
		
		break;

	case 5:	// Wait for CM inspection(30sec)
		if (!m_pEquipData->bUseVisionCmAlign) {
			m_nVisionCmCase = 10; m_tVisionCmLoop.Set_LoopTime(5000);
		}
		break;

	case 6:	// Error
		nCmScanCnt = 0;
		nCmScanNo = 0;
		dCmX = m_pMoveData->dVisionCmX[1];	// Inspect Position
		g_objAJinAXL.Move_Absolute(AX_VISION_CM_X, dCmX);
		m_nVisionCmCase = 2; m_tVisionCmLoop.Set_LoopTime(5000);
		g_objAviHandler.Set_NotifyCmAlignAlarm();
		g_objCommon.Show_Error(3606);
		return FALSE;

	case 10:	// 다음 검사 위치 피치 이동
		if (!m_pEquipData->bUseVisionCmAlign) { if(!m_tVisionCmLoop.Waiting_Time(300)) break; }	// Delay

		if (g_objAJinAXL.Is_Done(AX_VISION_CM_X)) {
			m_tVisionCmLoop.Takt_Save(5, 2);
			if (nCmScanNo <= nCmScanCnt) {				
				m_tVisionCmLoop.Takt_Start();
				dCmX = m_pMoveData->dVisionCmX[1] + (m_pEquipData->dIndexPitch * nCmScanNo);
				g_objAJinAXL.Move_Absolute(AX_VISION_CM_X, dCmX);
				m_nVisionCmCase = 2; m_tVisionCmLoop.Set_LoopTime(10000);

			} else {
				m_nVisionCmCase = 15; m_tVisionCmLoop.Set_LoopTime(10000);
			}
		}
		break;
	case 15:	// X Move to Ready Position
		if (g_objAJinAXL.Is_Done(AX_VISION_CM_X)) {
			m_tVisionCmLoop.Takt_Start();
			
			g_objCommon.Move_Position(AX_VISION_CM_X, 0);
			m_nVisionCmCase++; m_tVisionCmLoop.Set_LoopTime(10000);
		}
		break;
	case 16:	// Check Position
		if (g_objCommon.Check_Position(AX_VISION_CM_X, 0)) 
		{
			if (Check_CmAlignDone()) 
			{
				gData.nInsCmScanLineCnt++;
				if(gData.nInsCmScanLineCnt >= m_pEquipData->nInspectCmScanTimes)
				{	
					gData.nInspectCmLotCount++;
					gData.nInsCmScanLineCnt = 0;					
				}		

				if( gData.nInspectCmLotCount >= m_pEquipData->nInspectCmLotTimes)
				{					
					gData.bInspectCmThisLotVSkip = TRUE;
				}
				m_tVisionCmLoop.Takt_Save(5,3);
				gData.IndexDone[0] = TRUE;
				g_objLogFile.Save_TestLog("gData.IndexDone[0] = TRUE  3");
// 				if (gData.IndexDone[0] && Check_CmAlignDone())	{ m_nVisionCmCase = 0; m_tVisionCmLoop.Set_LoopTime(30000); }
// 				else											{ m_nVisionCmCase++; m_tVisionCmLoop.Set_LoopTime(30000); }

				m_nVisionCmCase = 0; m_tVisionCmLoop.Set_LoopTime(30000);
				m_strLog.Format("VisionCmAlign, %d", GetTickCount() - m_dwVisionCm);
				g_objLogFile.Save_TestLog(m_strLog);
			}
		}
		break;
	case 17:	// 검사 완료 확인.
		if (!gData.IndexDone[0] && Check_CmAlignDone()) {
			gData.IndexDone[0] = TRUE;
			g_objLogFile.Save_TestLog("gData.IndexDone[0] = TRUE  4");
			m_nVisionCmCase = 0; m_tVisionCmLoop.Set_LoopTime(30000);
		}
		break;
	}

	// 6. (Error : 3600)
	if (m_tVisionCmLoop.Over_LoopTime()) {
		g_objCommon.Show_Error(3600 + m_nVisionCmCase);
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 7. (Error : 3700)
BOOL CSequenceMain::VisionCap_Run()
{
	static int nCapScanCnt = 0;	// 총 Scan 횟수
	static int nCapScanNo = 0;	// 현재 Scan 번호
	static double dCapY = 0.0;

	switch (m_nVisionCapCase) {
	case 0:		// Assembly Picker가 검사 위치에 오면 시각
		if (m_nAssyPickCase == 10) {
			m_nVisionCapCase = 1;
			 m_tVisionCapLoop.Set_LoopTime(5000);
		}
		return TRUE;

	case 1:		// X Move Inspect Position
		if (g_objAJinAXL.Is_Done(AX_VISION_CAP_Y)) {
			if (g_objCommon.Check_Position(AX_ASSY_PICKER_X, 1) && g_objCommon.Check_Position(AX_ASSY_PICKER_Y, 1) && g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 2)) {
				if (m_pEquipData->bUseVisionCapDir) {
					if (!g_objInspector.Check_LotReady()) return TRUE;
					m_dwVisionCap = GetTickCount();
					m_tVisionCapLoop.Takt_Start();
					nCapScanCnt = 0;
					nCapScanNo = 0;
					dCapY = m_pMoveData->dVisionCapY[0];	// Inspect Position
					g_objAJinAXL.Move_Absolute(AX_VISION_CAP_Y, dCapY);
					m_nVisionCapCase++; m_tVisionCapLoop.Set_LoopTime(5000);
				} else {	// Cap Dir 검사를 안하면 종료한다.
					if (m_nAssyPickCase == 10) m_nAssyPickCase = 11;
					m_nVisionCapCase = 0; m_tVisionCapLoop.Set_LoopTime(5000);
				}
			}
		}
		break;
	case 2:		// Load Complete Send
		if (g_objAJinAXL.Is_MoveDone(AX_VISION_CAP_Y, dCapY)) {
			nCapScanCnt = Get_CapScanCnt();
			m_tVisionCapLoop.Takt_Save(6, 1);
			if (nCapScanNo <= nCapScanCnt) {
				m_tVisionCapLoop.Takt_Start();
				int nCapTNo1 = (gData.InfoAssyPick[nCapScanNo+0] > 0 ? gData.nCapTNoAssyPicker[nCapScanNo+0] : 0);
				int nCapTNo2 = (gData.InfoAssyPick[nCapScanNo+3] > 0 ? gData.nCapTNoAssyPicker[nCapScanNo+3] : 0);
				int nCapNo1 = (gData.InfoAssyPick[nCapScanNo+0] > 0 ? gData.nCapNoAssyPicker[nCapScanNo+0] : 0);
				int nCapNo2 = (gData.InfoAssyPick[nCapScanNo+3] > 0 ? gData.nCapNoAssyPicker[nCapScanNo+3] : 0);
				gData.nCapInspPickNo1 = nCapScanNo + 1;
				gData.nCapInspPickNo2 = nCapScanNo + 4;
				gData.bCapDirSkip = gData.bCapDirRetry = FALSE;

				// Cap 검사는 Tray 번호와 CM 번호가 없다.
				CString strCapLotID;
				strCapLotID = gData.sCIDAssyPicker[nCapScanNo];
				strCapLotID.Replace("/", "_");
				g_objInspector.Set_LoadComplete("B12", strCapLotID, 1, nCapTNo1, nCapTNo2, nCapNo1, nCapNo2, gData.nCapInspPickNo1, gData.nCapInspPickNo2);

				nCapScanNo++;
				m_nVisionCapCase = 5; m_tVisionCapLoop.Set_LoopTime(30000);

			} else {
				gData.bCapDirSkip = gData.bCapDirRetry = FALSE;
				m_nVisionCapCase = 15; m_tVisionCapLoop.Set_LoopTime(5000);
			}
		}
		break;

	case 5:	// Wait for Cap inspection(30sec)
		if (!m_pEquipData->bUseVisionCapDir || gData.bCapDirSkip) {
			m_nVisionCapCase = 10; m_tVisionCapLoop.Set_LoopTime(5000);
		}
		break;
// 	case 6:	// Error
// 		nCapScanNo--;
// 		m_nVisionCapCase = 2; m_tVisionCapLoop.Set_LoopTime(5000);
// 		g_objCommon.Show_Error(3706);
// 		return FALSE;

	case 10:	// 다음 검사 위치 피치 이동
		if (!m_pEquipData->bUseVisionCapDir) { if (!m_tVisionCapLoop.Waiting_Time(300)) break; }	// Delay

		if (g_objAJinAXL.Is_Done(AX_VISION_CAP_Y)) {
			m_tVisionCapLoop.Takt_Save(6, 2);
			if (nCapScanNo <= nCapScanCnt) {
				m_tVisionCapLoop.Takt_Start();
				dCapY = m_pMoveData->dVisionCapY[0] + (m_pEquipData->dIndexPitch * nCapScanNo);
				g_objAJinAXL.Move_Absolute(AX_VISION_CAP_Y, dCapY);
				m_nVisionCapCase = 2; m_tVisionCapLoop.Set_LoopTime(10000);
			} else {
				m_nVisionCapCase = 15; m_tVisionCapLoop.Set_LoopTime(10000);
			}
		}
		break;
	case 15:	// X Move to Ready Position
		if (g_objAJinAXL.Is_Done(AX_VISION_CAP_Y)) {
			if (m_nAssyPickCase == 10) m_nAssyPickCase = 11;
			g_objCommon.Move_Position(AX_VISION_CAP_Y, 0);
			m_nVisionCapCase++; m_tVisionCapLoop.Set_LoopTime(10000);
		}
		break;
	case 16:	// Check Position
		if (g_objCommon.Check_Position(AX_VISION_CAP_Y, 0)) {
			m_tVisionCapLoop.Takt_Save(6, 3);
			m_nVisionCapCase = 0; m_tVisionCapLoop.Set_LoopTime(10000);

			m_strLog.Format("VisionCapAlign, %d", GetTickCount() - m_dwVisionCap);
			g_objLogFile.Save_TestLog(m_strLog);
		}
		break;
	}

	// 7. (Error : 3700)
	if (m_tVisionCapLoop.Over_LoopTime()) {
		g_objCommon.Show_Error(3700 + m_nVisionCapCase);
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 8. (Error : 3800)
BOOL CSequenceMain::CapStage1_Run()
{
	if (gData.bCapPort1Wait && m_nCapStage1Case > 0 && m_nCapStage1Case < 10) return TRUE;
	if (gData.bCapPort2Wait && m_nCapStage1Case > 22 && m_nCapStage1Case < 50) return TRUE;

	switch (m_nCapStage1Case) {
	case 0:		// Start시 1로 바꿈
		return TRUE;

	case 1:		// 안전확인, Z Move to Move Up
		if (m_bUnloadLotEnd) { m_nCapStage1Case = 0; m_tCapStage1Loop.Set_LoopTime(5000); break; }

		if (m_pDX02->iCapPort1LowCheck && !m_pDX04->iCapStage1Exist && g_objCommon.Check_Position(AX_CAP_STAGE1_X, 0)) {
			g_objCommon.Move_Position(AX_CAP_STAGE1_Z, 1);	// Move Up
			m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(5000);
		} else {
			if (!m_pDX02->iCapPort1LowCheck) { g_dlgWork.PostMessage(UM_SHOW_MSG, 4, NULL); return FALSE; }
		}
		break;
	case 2:		// Cap Count 확인
		if (g_objCommon.Check_Position(AX_CAP_STAGE1_X, 0) && g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 1) && g_objCommon.Get_CapStageMasterSlaveOut(1)) {
			if (!m_pDX02->iCapPort1SlideClose || !m_pDX02->iCapPort1LowCheck || m_pDX04->iCapStage1Exist) break;	// 인터락

			if (gData.nCapTrayLoad == 0 || gData.nCapTrayLoad >= gData.nCapTrayMax) {
				gData.nCapTrayCount = 0;
				m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(5000);
			} else {
				m_dwCapStage1 = GetTickCount();
				m_tCapStage1Loop.Takt_Start();
				g_objCommon.Move_Position(AX_CAP_STAGE1_Z, 2);	// Support Up
				m_nCapStage1Case = 6; m_tCapStage1Loop.Set_LoopTime(5000);
			}
		} else {
			if (!m_pDX02->iCapPort1LowCheck) { g_dlgWork.PostMessage(UM_SHOW_MSG, 4, NULL); return FALSE;}
		}
		break;
	case 3:		// Cap 정보 확인 및 MES 자재 등록.
		if (g_dlgWork.Check_CapLotId()) {
			if (m_pEquipData->bUseMES && m_pEquipData->bUseMesCapReg) {	// MES Cap 자재등록 사용
				g_objMES.m_nMESCapSequence = 0;
				g_dlgWork.Get_CapLotId();
				g_objMES.Set_CapLotChangeRequest(gData.sCapLotID, gData.sOperID);	// 자재 등록 요청
			}
			m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(30000);
		} 
		else
		{
			g_objCommon.Show_Alarm("Cap Tray 자재 정보를 입력해 주십시오.");
			return FALSE;
		}
		break;
	case 4:		// MES 결과 확인.
		if (g_objMES.m_nMESCapSequence == 2 || !m_pEquipData->bUseMES || !m_pEquipData->bUseMesCapReg) {
			g_dlgWork.Change_CapLotId();	// 정보 이동.
			m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 5:		// Z Move to Support Up
		if (g_objCommon.Check_Position(AX_CAP_STAGE1_X, 0) && g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 1) && g_objCommon.Get_CapStageMasterSlaveOut(1)) {
			if (!m_pDX02->iCapPort1SlideClose || !m_pDX02->iCapPort1LowCheck || m_pDX04->iCapStage1Exist) break;	// 인터락

			m_dwCapStage1 = GetTickCount();
			m_tCapStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_CAP_STAGE1_Z, 2);
			m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(5000);
		} else {
			if (!m_pDX02->iCapPort1LowCheck) { g_dlgWork.PostMessage(UM_SHOW_MSG, 4, NULL); return FALSE;}
		}
		break;
	case 6:		// Support Out
		if (g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 2)) {
			m_tCapStage1Loop.Takt_Save(7, 1);
			m_tCapStage1Loop.Takt_Start();
			g_objCommon.Set_CapPortSupportOut(1);
			m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 7:		// Z1 Move to Support Down Position
		if (g_objCommon.Get_CapPortSupportOut(1)) {
			m_tCapStage1Loop.Takt_Save(7, 2);
			m_tCapStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_CAP_STAGE1_Z, 3);	// Support Down Pos
			m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(10000);
		}
		break;
	case 8:		// Support In
		if (g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 3)) {
			m_tCapStage1Loop.Takt_Save(7, 3);
			m_tCapStage1Loop.Takt_Start();
			g_objCommon.Set_CapPortSupportIn(1);
			m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 9:		// Z1 Move to Moving Down Position
		if (g_objCommon.Get_CapPortSupportIn(1)) {
			m_tCapStage1Loop.Takt_Save(7, 4);
			m_tCapStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_CAP_STAGE1_Z, 0);	// Move Down
			m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 10:	// Z1 Master In
		if (g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 0)) {
			m_tCapStage1Loop.Takt_Save(7, 5);
			m_tCapStage1Loop.Takt_Start();
			m_pDY04->oCapStage1MasterIn = TRUE;
			g_objAJinAXL.Write_Output(4);
			m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 11:	// Slave In
		if (m_pDX04->iCapStage1MasterIn && !m_pDX04->iCapStage1MasterOut) {
			if (!m_tCapStage1Loop.Waiting_Time(300)) break;
			m_pDY04->oCapStage1SlaveIn = TRUE;
			g_objAJinAXL.Write_Output(4);
			m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 12:	// Cap Tray Count 증가
		if (g_objCommon.Get_CapStageMasterSlaveIn(1)) {
			if (m_pDX04->iCapStage1Exist) {
				m_tCapStage1Loop.Takt_Save(7, 6);
				gData.nCapTrayLoad++;
				m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(5000);
			}			
		}
		break;
	case 13:	// X1 Move to Start Position
		if (m_pDX04->iCapStage1Exist && g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 0)) {
			if (m_nCapStage2Case < 20) return TRUE;
			if (g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 0)) break;	// 인터락

			m_tCapStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_CAP_STAGE1_X, 1);	// Work Pos
			m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(10000);
		}
		break;
	case 14:	// Check X1 Start Position
		if (m_pDX04->iCapStage1Exist && g_objCommon.Check_Position(AX_CAP_STAGE1_X, 1)) {
			m_tCapStage1Loop.Takt_Save(7, 7);
			m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(5000);
		}
		break;

	case 15:	// Wait
		if (m_nCapStage2Case < 13 || m_nCapStage2Case > 23) { m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(5000); }
		return TRUE;

	case 16:	// Move Up
		if (m_pDX04->iCapStage1Exist && g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 0)) {
			m_tCapStage1Loop.Takt_Start();
			gData.nCapTrayCount++;
			gData.sCIDCapStage[0] = gData.sCapLotID;
			Init_CapTray();
			g_objCommon.Move_Position(AX_CAP_STAGE1_Z, 1);
			m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(10000);
		}
		break;
	case 17:	// Position Check
		if (m_pDX04->iCapStage1Exist && g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 1)) {
			m_tCapStage1Loop.Takt_Save(7, 8);
			m_nCapStage1Case = 20; m_tCapStage1Loop.Set_LoopTime(10000);
		}
		break;

	case 20:	// Work
		return TRUE;

	case 21:	// 안전 확인
		if (m_nCapStage2Case > 51 || m_nCapStage2Case <= 15) { m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(10000); }
		return TRUE;

	case 22:	// X Move to Unload Position
		if (g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 1)) {
			m_tCapStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_CAP_STAGE1_X, 2);	// Unload Pos
			m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(10000);
		}
		break;
	case 23:	// Z Move to Support Down
		if (m_pDX02->iCapPort2SlideClose && g_objCommon.Check_Position(AX_CAP_STAGE1_X, 2)) {
			m_tCapStage1Loop.Takt_Save(7, 9);
			m_tCapStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_CAP_STAGE1_Z, 3);	// Support Down
			m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(10000);
		}
		break;
	case 24:	// Port Support Out
		if (g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 3)) {
			m_tCapStage1Loop.Takt_Save(7, 10);
			m_tCapStage1Loop.Takt_Start();
			g_objCommon.Set_CapPortSupportOut(2);
			m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(10000);
		}
		break;
	case 25:	// Z Move to Support Up
		if (g_objCommon.Get_CapPortSupportOut(2)) {
			m_tCapStage1Loop.Takt_Save(7, 11);
			m_tCapStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_CAP_STAGE1_Z, 2);	// Support Up
			m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(10000);
		}
		break;
	case 26:	// Port Support In
		if (g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 2)) {
			m_tCapStage1Loop.Takt_Save(7, 12);
			m_tCapStage1Loop.Takt_Start();
			g_objCommon.Set_CapPortSupportIn(2);
			m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(10000);
		}
		break;
	case 27:	// Master/Slave Out
		if (g_objCommon.Get_CapPortSupportIn(2)) {
			m_tCapStage1Loop.Takt_Save(7, 13);
			m_tCapStage1Loop.Takt_Start();
			m_pDY04->oCapStage1MasterIn = FALSE;
			m_pDY04->oCapStage1SlaveIn = FALSE;
			g_objAJinAXL.Write_Output(4);
			m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(10000);
		}
		break;
	case 28:	// Z Move to Move Up Position
		if (g_objCommon.Get_CapStageMasterSlaveOut(1)) {
			m_tCapStage1Loop.Takt_Save(7, 14);
			m_tCapStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_CAP_STAGE1_Z, 1);	// Move Up
			m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(10000);
		}
		break;
	case 29:	// Position Check
		if (g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 1)) {
			m_tCapStage1Loop.Takt_Save(7, 15);
			m_nCapStage1Case = 50; m_tCapStage1Loop.Set_LoopTime(10000);

			m_strLog.Format("CapStage1, %d", GetTickCount() - m_dwCapStage1);
			g_objLogFile.Save_TestLog(m_strLog);
		}
		break;

	case 50:	// Stage2 Move Down 이후
		if (m_nCapStage2Case > 13 && m_nCapStage2Case < 50) { m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(10000); }
		return TRUE;

	case 51:	// Move to Load Position
		if (!m_pDX04->iCapStage1Exist && g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 1)) {
			m_tCapStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_CAP_STAGE1_X, 0);
			m_nCapStage1Case++; m_tCapStage1Loop.Set_LoopTime(10000);
		}
		break;
	case 52:
		if (!m_pDX04->iCapStage1Exist && g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 1) && g_objCommon.Check_Position(AX_CAP_STAGE1_X, 0)) {
			m_tCapStage1Loop.Takt_Save(7, 16);
			m_nCapStage1Case = 2; m_tCapStage1Loop.Set_LoopTime(10000);
		}
		break;
	}

	// 8. (Error : 3800)
	if (m_tCapStage1Loop.Over_LoopTime()) {
		g_objCommon.Show_Error(3800 + m_nCapStage1Case);
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 9. (Error : 3900)
BOOL CSequenceMain::CapStage2_Run()
{
	if (gData.bCapPort1Wait && m_nCapStage2Case > 0 && m_nCapStage2Case < 10) return TRUE;
	if (gData.bCapPort2Wait && m_nCapStage2Case > 22 && m_nCapStage2Case < 50) return TRUE;

	switch (m_nCapStage2Case) {
	case 0:		// Start시 1로 바꿈
		return TRUE;

	case 1:		// 안전확인, Z Move to Move Up
		if (m_bUnloadLotEnd) { m_nCapStage2Case = 0; m_tCapStage2Loop.Set_LoopTime(5000); break; }

		if (m_pDX02->iCapPort1LowCheck && !m_pDX04->iCapStage2Exist && g_objCommon.Check_Position(AX_CAP_STAGE2_X, 0)) {
			g_objCommon.Move_Position(AX_CAP_STAGE2_Z, 1);	// Move Up
			m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(5000);
		} else {
			if (!m_pDX02->iCapPort1LowCheck) { g_dlgWork.PostMessage(UM_SHOW_MSG, 4, NULL); return FALSE; }
		}
		break;
	case 2:		// Cap Count 확인
		if (g_objCommon.Check_Position(AX_CAP_STAGE2_X, 0) && g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 1) && g_objCommon.Get_CapStageMasterSlaveOut(2)) {
			if (!m_pDX02->iCapPort1SlideClose || !m_pDX02->iCapPort1LowCheck || m_pDX04->iCapStage2Exist) break;	// 인터락

			if (gData.nCapTrayLoad == 0 || gData.nCapTrayLoad >= gData.nCapTrayMax) {
				gData.nCapTrayCount = 0;
				m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(5000);
			} else {
				m_dwCapStage2 = GetTickCount();
				m_tCapStage2Loop.Takt_Start();
				g_objCommon.Move_Position(AX_CAP_STAGE2_Z, 2);	// Support Up
				m_nCapStage2Case = 6; m_tCapStage2Loop.Set_LoopTime(5000);
			}
		} else {
			if (!m_pDX02->iCapPort1LowCheck) { g_dlgWork.PostMessage(UM_SHOW_MSG, 4, NULL); return FALSE;}
		}
		break;
	case 3:		// Cap 정보 확인 및 MES 자재 등록.
		if (g_dlgWork.Check_CapLotId()) {
			if (m_pEquipData->bUseMES && m_pEquipData->bUseMesCapReg) {	// MES Cap 자재등록 사용
				g_objMES.m_nMESCapSequence = 0;
				g_dlgWork.Get_CapLotId();
				g_objMES.Set_CapLotChangeRequest(gData.sCapLotID, gData.sOperID);	// 자재 등록 요청
			}
			m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(30000);
		} else {
			g_objCommon.Show_Alarm("Cap Tray 자재 정보를 입력해 주십시오.");
			return FALSE;
		}
		break;
	case 4:		// MES 결과 확인.
		if (g_objMES.m_nMESCapSequence == 2 || !m_pEquipData->bUseMES || !m_pEquipData->bUseMesCapReg) {
			g_dlgWork.Change_CapLotId();	// 정보 이동.
			m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 5:		// Z Move to Support Up
		if (g_objCommon.Check_Position(AX_CAP_STAGE2_X, 0) && g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 1) && g_objCommon.Get_CapStageMasterSlaveOut(2)) {
			if (!m_pDX02->iCapPort1SlideClose || !m_pDX02->iCapPort1LowCheck || m_pDX04->iCapStage2Exist) break;	// 인터락

			m_dwCapStage2 = GetTickCount();
			m_tCapStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_CAP_STAGE2_Z, 2);
			m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(5000);
		} else {
			if (!m_pDX02->iCapPort1LowCheck) { g_dlgWork.PostMessage(UM_SHOW_MSG, 4, NULL); return FALSE;}
		}
		break;
	case 6:		// Support Out
		if (g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 2)) {
			m_tCapStage2Loop.Takt_Save(8, 1);
			m_tCapStage2Loop.Takt_Start();
			g_objCommon.Set_CapPortSupportOut(1);
			m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 7:		// Z1 Move to Support Down Position
		if (g_objCommon.Get_CapPortSupportOut(1)) {
			m_tCapStage2Loop.Takt_Save(8, 2);
			m_tCapStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_CAP_STAGE2_Z, 3);	// Support Down Pos
			m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(10000);
		}
		break;
	case 8:		// Support In
		if (g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 3)) {
			m_tCapStage2Loop.Takt_Save(8, 3);
			m_tCapStage2Loop.Takt_Start();
			g_objCommon.Set_CapPortSupportIn(1);
			m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 9:		// Z1 Move to Moving Down Position
		if (g_objCommon.Get_CapPortSupportIn(1)) {
			m_tCapStage2Loop.Takt_Save(8, 4);
			m_tCapStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_CAP_STAGE2_Z, 0);	// Move Down
			m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 10:	// Z1 Master In
		if (g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 0)) {
			m_tCapStage2Loop.Takt_Save(8, 5);
			m_tCapStage2Loop.Takt_Start();
			m_pDY04->oCapStage2MasterIn = TRUE;
			g_objAJinAXL.Write_Output(4);
			m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 11:	// Slave In
		if (m_pDX04->iCapStage2MasterIn && !m_pDX04->iCapStage2MasterOut) {
			if (!m_tCapStage2Loop.Waiting_Time(300)) break;
			m_pDY04->oCapStage2SlaveIn = TRUE;
			g_objAJinAXL.Write_Output(4);
			m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 12:	// Cap Tray Count 증가
		if (g_objCommon.Get_CapStageMasterSlaveIn(2)) {
			if (m_pDX04->iCapStage2Exist) {
				m_tCapStage2Loop.Takt_Save(8, 6);
				gData.nCapTrayLoad++;
				m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(5000);
			}			
		}
		break;
	case 13:	// X1 Move to Start Position
		if (m_pDX04->iCapStage2Exist && g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 0)) {
			if (m_nCapStage1Case < 20) return TRUE;
			if (g_objCommon.Check_Position(AX_CAP_STAGE1_Z, 0)) break;	// 인터락

			m_tCapStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_CAP_STAGE2_X, 1);	// Work Pos
			m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(10000);
		}
		break;
	case 14:	// Check X2 Start Position
		if (m_pDX04->iCapStage2Exist && g_objCommon.Check_Position(AX_CAP_STAGE2_X, 1)) {
			m_tCapStage2Loop.Takt_Save(8, 7);
			m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(10000);
		}
		break;

	case 15:	// Wait
		if (m_nCapStage1Case < 13 || m_nCapStage1Case > 23) { m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(5000); }
		return TRUE;

	case 16:	// Move Up
		if (m_pDX04->iCapStage2Exist && g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 0)) {
			m_tCapStage2Loop.Takt_Start();
			gData.nCapTrayCount++;
			gData.sCIDCapStage[1] = gData.sCapLotID;
			Init_CapTray();
			g_objCommon.Move_Position(AX_CAP_STAGE2_Z, 1);
			m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(10000);
		}
		break;
	case 17:	// Position Check
		if (m_pDX04->iCapStage2Exist && g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 1)) {
			m_tCapStage2Loop.Takt_Save(8, 8);
			m_nCapStage2Case = 20; m_tCapStage2Loop.Set_LoopTime(10000);
		}
		break;

	case 20:	// Work
		return TRUE;

	case 21:	// 안전 확인
		if (m_nCapStage1Case > 51 || m_nCapStage1Case <= 15) { m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(10000); }
		return TRUE;

	case 22:	// X Move to Unload Position
		if (g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 1)) {
			m_tCapStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_CAP_STAGE2_X, 2);	// Unload Pos
			m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(10000);
		}
		break;
	case 23:	// Z Move to Support Down
		if (m_pDX02->iCapPort2SlideClose && g_objCommon.Check_Position(AX_CAP_STAGE2_X, 2)) {
			m_tCapStage2Loop.Takt_Save(8, 9);
			m_tCapStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_CAP_STAGE2_Z, 3);	// Support Down
			m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(10000);
		}
		break;
	case 24:	// Port Support Out
		if (g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 3)) {
			m_tCapStage2Loop.Takt_Save(8, 10);
			m_tCapStage2Loop.Takt_Start();
			g_objCommon.Set_CapPortSupportOut(2);
			m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(10000);
		}
		break;
	case 25:	// Z Move to Support Up
		if (g_objCommon.Get_CapPortSupportOut(2)) {
			m_tCapStage2Loop.Takt_Save(8, 11);
			m_tCapStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_CAP_STAGE2_Z, 2);	// Support Up
			m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(10000);
		}
		break;
	case 26:	// Port Support In
		if (g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 2)) {
			m_tCapStage2Loop.Takt_Save(8, 12);
			m_tCapStage2Loop.Takt_Start();
			g_objCommon.Set_CapPortSupportIn(2);
			m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(10000);
		}
		break;
	case 27:	// Master/Slave Out
		if (g_objCommon.Get_CapPortSupportIn(2)) {
			m_tCapStage2Loop.Takt_Save(8, 13);
			m_tCapStage2Loop.Takt_Start();
			m_pDY04->oCapStage2MasterIn = FALSE;
			m_pDY04->oCapStage2SlaveIn = FALSE;
			g_objAJinAXL.Write_Output(4);
			m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(10000);
		}
		break;
	case 28:	// Z Move to Move Up Position
		if (g_objCommon.Get_CapStageMasterSlaveOut(2)) {
			m_tCapStage2Loop.Takt_Save(8, 14);
			m_tCapStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_CAP_STAGE2_Z, 1);	// Move Up
			m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(10000);
		}
		break;
	case 29:	// Position Check
		if (g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 1)) {
			m_tCapStage2Loop.Takt_Save(8, 15);
			m_nCapStage2Case = 50; m_tCapStage2Loop.Set_LoopTime(10000);

			m_strLog.Format("CapStage2, %d", GetTickCount() - m_dwCapStage2);
			g_objLogFile.Save_TestLog(m_strLog);
		}
		break;

	case 50:	// Stage2 Move Down 이후
		if (m_nCapStage1Case > 14 && m_nCapStage1Case < 50) { m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(10000); }
		return TRUE;

	case 51:	// Move to Load Position
		if (!m_pDX04->iCapStage2Exist && g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 1)) {
			m_tCapStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_CAP_STAGE2_X, 0);
			m_nCapStage2Case++; m_tCapStage2Loop.Set_LoopTime(10000);
		}
		break;
	case 52:
		if (!m_pDX04->iCapStage2Exist && g_objCommon.Check_Position(AX_CAP_STAGE2_Z, 1) && g_objCommon.Check_Position(AX_CAP_STAGE2_X, 0)) {
			m_tCapStage2Loop.Takt_Save(8, 16);
			m_nCapStage2Case = 2; m_tCapStage2Loop.Set_LoopTime(10000);
		}
		break;
	}

	// 9. (Error : 3900)
	if (m_tCapStage2Loop.Over_LoopTime()) {
		g_objCommon.Show_Error(3900 + m_nCapStage2Case);
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 10. (Error : 4000)
BOOL CSequenceMain::CapPicker_Run()
{
	static int nCpWorkTray = 0;		// 현재 작업중인 트레이 (1~2)

	static int nCpTrayPosX = 0;		// Tray X (0~)
	static int nCpTrayPosY = 0;		// Tray Y (0~)
	static int nCpPickPos = 0;		// Picker Start (0~)
	static int nCpMultiCnt = 0;		// Picker가 Down되는 수

	static double dCpPickY = 0.0;	// Picker Y
	static double dCpTrayX = 0.0;	// Tray X

	static BOOL bCpTrayEmpty = FALSE;

	switch (m_nCapPickCase) {
	case 0:		// Wait
		if (m_nCapStage1Case == 20 || m_nCapStage2Case == 20) m_nCapPickCase++;
		return TRUE;

	case 1:		// 작업 시작
		if (g_objCommon.Get_CapPickerUp(0) && g_objCommon.Check_Position(AX_CAP_PICKER_Z, 0)) {
			if (m_nCapStage1Case != 20 && m_nCapStage2Case != 20) return TRUE;	// 인터락
			m_dwCapPick = GetTickCount();

			if (m_nCapStage1Case == 20) nCpWorkTray = 1;
			if (m_nCapStage2Case == 20) nCpWorkTray = 2;
			m_nCapPickCase++; m_tCapPickLoop.Set_LoopTime(5000);
		}
		break;
	case 2:		// Stage X Picker Y Move to Cap Pickup Position
		if (g_objCommon.Get_CapPickerUp(0) && g_objCommon.Check_Position(AX_CAP_PICKER_Z, 0) && g_objAJinAXL.Is_Done(AX_CAP_PICKER_Y)) {
			if (m_nCapStage1Case != 20 && m_nCapStage2Case != 20) return TRUE;	// 인터락

			int nPickCnt = 0, nMoveIdx = 0;
			if (Select_CapPickerPos(nCpPickPos, nPickCnt) && Select_CapTrayPos(nCpTrayPosX, nCpTrayPosY)) 
			{
				m_tCapPickLoop.Takt_Start();
				if (m_pEquipData->bCapPickUpMulti) 
				{
					int nTrayCnt = CT_X - nCpTrayPosX;
					nCpMultiCnt = (nPickCnt < nTrayCnt ? nPickCnt : nTrayCnt);
					nMoveIdx = (nCpWorkTray-1) * 6;		// Cap Picker Y축 MoveData Index, Stage1 Picker1 = 0, Stage2 Picker1 = 6
				} 
				else 
				{
					nCpMultiCnt = 1;
					nMoveIdx = nCpPickPos + (nCpWorkTray-1) * 6;	// Cap Picker Y축 MoveData Index
				}

				if (nCpWorkTray == 1) dCpTrayX = m_pMoveData->dCapStage1X[1] - nCpTrayPosY * m_pEquipData->dCapTrayPitchY;
				if (nCpWorkTray == 2) dCpTrayX = m_pMoveData->dCapStage2X[1] - nCpTrayPosY * m_pEquipData->dCapTrayPitchY;
				if (nCpWorkTray == 1) g_objAJinAXL.Move_Absolute(AX_CAP_STAGE1_X, dCpTrayX);
				if (nCpWorkTray == 2) g_objAJinAXL.Move_Absolute(AX_CAP_STAGE2_X, dCpTrayX);

				dCpPickY = m_pMoveData->dCapPickerY[nMoveIdx] + (nCpTrayPosX - nCpPickPos) * m_pEquipData->dCapTrayPitchX;
				g_objAJinAXL.Move_Absolute(AX_CAP_PICKER_Y, dCpPickY);
								
				g_objCommon.Set_CapPickerNormal(0);
				m_nCapPickCase= 9; m_tCapPickLoop.Set_LoopTime(10000);
			}
		}
		break;
	case 9:
		if(g_objCommon.Get_CapPickerNormal(0))
		{
			if (!m_tCapPickLoop.Waiting_Time(10)) break;
			g_objCommon.Move_Position(AX_CAP_PICKER_P, 0);
			m_nCapPickCase = 3; m_tCapPickLoop.Set_LoopTime(10000);
		}
		break;
	case 3:		// Z Move to Tray Down
		if ((nCpWorkTray == 1 && g_objAJinAXL.Is_MoveDone(AX_CAP_STAGE1_X, dCpTrayX)) || (nCpWorkTray == 2 && g_objAJinAXL.Is_MoveDone(AX_CAP_STAGE2_X, dCpTrayX))) {
			if (g_objAJinAXL.Is_MoveDone(AX_CAP_PICKER_Y, dCpPickY) && g_objCommon.Check_Position(AX_CAP_PICKER_P, 0) ) {
				m_tCapPickLoop.Takt_Save(9, 1);
				m_tCapPickLoop.Takt_Start();

				g_objCommon.Move_Position(AX_CAP_PICKER_Z, nCpWorkTray);	//Cap Tray Down
				g_objCommon.Set_CapPickerDownMulti(nCpPickPos+1, nCpMultiCnt);
				m_nCapPickCase++; m_tCapPickLoop.Set_LoopTime(10000);
			}
		}
		break;
	case 4:		// 정보전달, Picker Vacuum On
		if (g_objCommon.Check_Position(AX_CAP_PICKER_Z, nCpWorkTray) && g_objCommon.Get_CapPickerDownMulti(nCpPickPos+1, nCpMultiCnt)) {
			for (int i = 0; i < nCpMultiCnt; i++) {
				gData.InfoCapPick[nCpPickPos+i] = gData.InfoCapTray[nCpTrayPosY][nCpTrayPosX+i]; gData.InfoCapTray[nCpTrayPosY][nCpTrayPosX+i] = 0;
				gData.nCapTNoCapPicker[nCpPickPos+i] = gData.nCapTrayCount;
				gData.nCapNoCapPicker[nCpPickPos+i] = nCpTrayPosY * CT_X + nCpTrayPosX + i + 1;
				gData.sCIDCapPicker[nCpPickPos+i] = gData.sCIDCapStage[nCpWorkTray-1];
			}
			g_dlgWork.PostMessage(UM_UPDATE_TRAY_INFO, 2, 0);

			g_objCommon.Set_CapPickerVacOnMulti(nCpPickPos+1, nCpMultiCnt);
			m_nCapPickCase++; m_tCapPickLoop.Set_LoopTime(10000);
		}
		break;
	case 5:		// Z Move to Ready Up
		if (/*g_objCommon.Get_InfoCapPickerVacOn()*/TRUE) {
			if (!m_tCapPickLoop.Waiting_Time(m_pEquipData->nDelayAdd[1])) break;

			g_objCommon.Move_Position(AX_CAP_PICKER_Z, 0);	//Ready Up
			g_objCommon.Set_CapPickerUp(0);	// All Up
			m_nCapPickCase++; m_tCapPickLoop.Set_LoopTime(10000);
		}
		break;
	case 6:		// Move to Cap Buffer Pos or Repeat Pick Up
		if (g_objCommon.Check_Position(AX_CAP_PICKER_Z, 0) && g_objCommon.Get_CapPickerUp(0) && g_objCommon.Get_InfoCapPickerVacOn()) {
			m_tCapPickLoop.Takt_Save(9, 2);

			bCpTrayEmpty = Check_CapTrayEmpty();

			if (bCpTrayEmpty && m_nCapStage1Case == 20) m_nCapStage1Case = 21;
			if (bCpTrayEmpty && m_nCapStage2Case == 20) m_nCapStage2Case = 21;

			if (Check_CapPickerFull()) {
				m_tCapPickLoop.Takt_Start();
				g_objCommon.Move_Position(AX_CAP_PICKER_Y, 12);	// Cap Buffer
				g_objCommon.Move_Position(AX_CAP_PICKER_P, 1);	// Cap Buffer
				
				m_nCapPickCase++; m_tCapPickLoop.Set_LoopTime(10000);

			} else {
				if (bCpTrayEmpty) {
					nCpWorkTray = (nCpWorkTray == 1 ? 2 : 1);	// 교체
					int nPickCnt = 0, nMoveIdx = 0;
					if (Select_CapPickerPos(nCpPickPos, nPickCnt)) {
						if (m_pEquipData->bCapPickUpMulti) 
						{
							nMoveIdx = (nCpWorkTray-1) * 6;		// Cap Picker Y축 MoveData Index, Stage1 Picker1 = 0, Stage2 Picker1 = 6
						} else {
							nMoveIdx = nCpPickPos + (nCpWorkTray-1) * 6;	// Cap Picker Y축 MoveData Index
						}
						dCpPickY = m_pMoveData->dCapPickerY[nMoveIdx] - nCpPickPos * m_pEquipData->dCapTrayPitchX;
						g_objAJinAXL.Move_Absolute(AX_CAP_PICKER_Y, dCpPickY);
						m_nCapPickCase = 1; m_tCapPickLoop.Set_LoopTime(10000);
					}
				} 
				else
				 {
					m_nCapPickCase = 2; m_tCapPickLoop.Set_LoopTime(10000);
				}
			}
		}
		break;
	case 7:		// Check Cap Buffer Pos
		if (g_objCommon.Check_Position(AX_CAP_PICKER_Y, 12) && g_objCommon.Check_Position(AX_CAP_PICKER_P, 1)) 
		{
			if (!m_tCapPickLoop.Waiting_Time(20)) break;
			if (m_pEquipData->bUseCapPickerTurn) g_objCommon.Set_CapPickerTurn(0);
			m_nCapPickCase = 8; m_tCapPickLoop.Set_LoopTime(10000);
			
		}
		break;
	case 8:
		if (!m_pEquipData->bUseCapPickerTurn || g_objCommon.Get_CapPickerTurn(0)) 
		{
			if (!m_tCapPickLoop.Waiting_Time(20)) break;
			m_tCapPickLoop.Takt_Save(9, 3);
			m_nCapPickCase = 10; m_tCapPickLoop.Set_LoopTime(10000);
		}
		break;


	case 10:	// Wait Cap Buffer
		if (m_nCapBufferCase == 0) { m_nCapPickCase++; m_tCapPickLoop.Set_LoopTime(10000); }
		return TRUE;

	case 11:	// Buffer Align Out
		if (g_objCommon.Check_Position(AX_CAP_BUFFER_Y, 0)) {
			m_tCapPickLoop.Takt_Start();
			g_objCommon.Set_CapBufferAlign(FALSE);
			m_nCapPickCase++; m_tCapPickLoop.Set_LoopTime(10000);
		}
		break;
	case 12:	// Picker Z Move to Buffer Down & Cleaner Up
		if (g_objCommon.Check_Position(AX_CAP_BUFFER_Y, 0) && g_objCommon.Get_CapBufferAlign(FALSE) &&
			g_objCommon.Check_Position(AX_CAP_PICKER_Y, 12) && g_objCommon.Check_Position(AX_CAP_PICKER_P, 1)) {
			m_tCapPickLoop.Takt_Save(9, 4);
			m_tCapPickLoop.Takt_Start();

			if (!m_tCapPickLoop.Waiting_Time(50)) break;

			g_objCommon.Set_InfoCapPickerDown();
			g_objCommon.Move_Position(AX_CAP_PICKER_Z, 3);

			m_pDY09->oCapCleanerUp = TRUE;
			g_objAJinAXL.Write_Output(9);
			m_nCapPickCase++; m_tCapPickLoop.Set_LoopTime(10000);
		}
		break;
	case 13:	// 정보전달, Picker Vacuum Off
		if (g_objCommon.Get_InfoCapPickerDown() && g_objCommon.Check_Position(AX_CAP_PICKER_Z, 3) && !m_pDX09->iCapCleanerDown && m_pDX09->iCapCleanerUp) {
			if (!m_tCapPickLoop.Waiting_Time(50)) break;

			for (int i = 0; i < PICK; i++) {
				if (gData.InfoCapBuffer[i] != 0) continue;
				gData.InfoCapBuffer[i] = gData.InfoCapPick[i]; gData.InfoCapPick[i] = 0;
				gData.nCapTNoCapBuffer[i] = gData.nCapTNoCapPicker[i]; gData.nCapTNoCapPicker[i] = 0;
				gData.nCapNoCapBuffer[i]  = gData.nCapNoCapPicker[i];  gData.nCapNoCapPicker[i]  = 0;
				gData.sCIDCapBuffer[i] = gData.sCIDCapPicker[i];
			}
			g_objCommon.Set_InfoCapPickerVacOff();
			g_objCommon.Set_CapBufferAlign(TRUE);
			m_nCapPickCase++; m_tCapPickLoop.Set_LoopTime(10000);
		}
		break;
	case 14:	// Clean Air On
		if (g_objCommon.Get_CapBufferAlign(TRUE) && g_objCommon.Check_Position(AX_CAP_PICKER_Z, 3)) {
			if (!m_tCapPickLoop.Waiting_Time(50)) break;

			g_objCommon.Set_InfoCapCleanAirOn();
			g_objCommon.Set_InfoCapPickerAirOff();
			m_nCapPickCase++; m_tCapPickLoop.Set_LoopTime(10000);
		}
		break;
	case 15:	// Clean Air Off
		if (g_objCommon.Get_CapBufferAlign(TRUE) && g_objCommon.Get_InfoCapPickerVacOff()) {
			if (!m_tCapPickLoop.Waiting_Time(m_pEquipData->nVacOffDelay[0])) break;

			g_objCommon.Set_CapCleanAirOff();	// Clean Air All Off
			m_nCapPickCase++; m_tCapPickLoop.Set_LoopTime(10000);
		}
		break;
	case 16:	// Picker Up & Cleaner Down
		if (g_objCommon.Get_CapBufferAlign(TRUE) && g_objCommon.Get_InfoCapPickerVacOff()) {
			g_objCommon.Set_CapPickerUp(0);
			g_objCommon.Move_Position(AX_CAP_PICKER_Z, 0);
			m_pDY09->oCapCleanerUp = FALSE;
			g_objAJinAXL.Write_Output(9);
			m_nCapPickCase++; m_tCapPickLoop.Set_LoopTime(10000);
		}
		break;
	case 17:	// Buffer Go
		if (g_objCommon.Check_Position(AX_CAP_PICKER_Z, 0) && g_objCommon.Get_CapPickerUp(0) && m_pDX09->iCapCleanerDown && !m_pDX09->iCapCleanerUp) {
			if (m_nCapBufferCase == 0) { m_nCapBufferCase = 1; m_tCapBufferLoop.Set_LoopTime(10000); }
			g_objCommon.Set_CapPickerNormal(0);
			//g_objCommon.Set_InfoCapPickerNormal();			// Turn Normal
			m_nCapPickCase = 18; m_tCapPickLoop.Set_LoopTime(10000);
			
		}
		break;
	case 18:
		if(g_objCommon.Get_CapPickerNormal(0))//( g_objCommon.Get_InfoCapPickerNormal())
		{
			if (!m_tCapPickLoop.Waiting_Time(10)) break;
			m_nCapPickCase = 20; m_tCapPickLoop.Set_LoopTime(10000);
		}
		break;

	case 20:	// return
		if (g_objCommon.Check_Position(AX_CAP_PICKER_Z, 0) && g_objCommon.Get_CapPickerUp(0)) {
			m_tCapPickLoop.Takt_Save(9, 5);
			m_tCapPickLoop.Takt_Start();
			
			g_objCommon.Move_Position(AX_CAP_PICKER_P, 0);	// Tray Pitch
			
			if (bCpTrayEmpty) 
			{
				nCpWorkTray = (nCpWorkTray == 1 ? 2 : 1);	// 교체
				int nPickCnt = 0, nMoveIdx = 0;
				if (Select_CapPickerPos(nCpPickPos, nPickCnt)) 
				{	
					if (m_pEquipData->bCapPickUpMulti) 
					{
						nMoveIdx = (nCpWorkTray-1) * 6;		// Cap Picker Y축 MoveData Index, Stage1 Picker1 = 0, Stage2 Picker1 = 6
					}
					else 
					{
						nMoveIdx = nCpPickPos + (nCpWorkTray-1) * 6;	// Cap Picker Y축 MoveData Index
					}
					dCpPickY = m_pMoveData->dCapPickerY[nMoveIdx] - nCpPickPos * m_pEquipData->dCapTrayPitchX;
					g_objAJinAXL.Move_Absolute(AX_CAP_PICKER_Y, dCpPickY);
					m_nCapPickCase++; m_tCapPickLoop.Set_LoopTime(10000);
				}
			} else {
				m_nCapPickCase = 2; m_tCapPickLoop.Set_LoopTime(10000);
			}
		}
		break;
	case 21:	// Check
		if (g_objAJinAXL.Is_MoveDone(AX_CAP_PICKER_Y, dCpPickY) && g_objCommon.Check_Position(AX_CAP_PICKER_P, 0) ) {
			m_tCapPickLoop.Takt_Save(9, 6);
			m_nCapPickCase = 1; m_tCapPickLoop.Set_LoopTime(10000);
			m_strLog.Format("Cap Picker, %d", GetTickCount() - m_dwCapPick);
			g_objLogFile.Save_TestLog(m_strLog);
		}
		break;
	}

	// 10. (Error : 4000)
	if (m_tCapPickLoop.Over_LoopTime()) {
		g_objCommon.Show_Error(4000 + m_nCapPickCase);
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 11. (Error : 4100)
BOOL CSequenceMain::CapBuffer_Run()
{
	switch (m_nCapBufferCase) {
	case 0:		// Cap Picker에서 Start
		return TRUE;

	case 1:		// 안전확인, Y Move to Clean Position
		if (g_objCommon.Check_Position(AX_CAP_BUFFER_Y, 0) && m_pDX09->iCapCleanerDown && !m_pDX09->iCapCleanerUp) {
			if (!g_objCommon.Get_CapBufferAlign(TRUE)) break;
			m_dwCapBuffer = GetTickCount();
			m_nCapBufferCase++; m_tCapBufferLoop.Set_LoopTime(5000);
		}
		break;
	case 2:		// Y Move to Unload Position
		if (g_objCommon.Check_Position(AX_CAP_BUFFER_Y, 0) && m_pDX09->iCapCleanerDown && !m_pDX09->iCapCleanerUp) {
			m_tCapBufferLoop.Takt_Start();
			g_objCommon.Move_Position(AX_CAP_BUFFER_Y, 2);
			m_nCapBufferCase++; m_tCapBufferLoop.Set_LoopTime(10000);
		}
		break;
	case 3:		// Position Check
		if (g_objCommon.Check_Position(AX_CAP_BUFFER_Y, 2)) {
			m_tCapBufferLoop.Takt_Save(10, 5);
			m_nCapBufferCase = 10; m_tCapBufferLoop.Set_LoopTime(5000);
		}
		break;

	case 10:	// Unloading
		return TRUE;

	case 11:	// Cap Align In
		if (g_objCommon.Check_Position(AX_CAP_BUFFER_Y, 2)) {
			
			m_nCapBufferCase++; m_tCapBufferLoop.Set_LoopTime(5000);
		}
		break;
	case 12:	// 안전확인, return
		if (m_pDX09->iCapCleanerDown && !m_pDX09->iCapCleanerUp) {
			m_tCapBufferLoop.Takt_Start();
			g_objCommon.Move_Position(AX_CAP_BUFFER_Y, 0);
			m_nCapBufferCase++; m_tCapBufferLoop.Set_LoopTime(10000);
		}
		break;
	case 13:	// Position Check
		if (g_objCommon.Check_Position(AX_CAP_BUFFER_Y, 0)) {
			m_tCapBufferLoop.Takt_Save(10, 6);
			m_nCapBufferCase = 0; m_tCapBufferLoop.Set_LoopTime(5000);
			m_strLog.Format("CapBuffer, %d", GetTickCount() - m_dwCapBuffer);
			g_objLogFile.Save_TestLog(m_strLog);
		}
		break;
	}

	// 11. (Error : 4100)
	if (m_tCapBufferLoop.Over_LoopTime()) {
		g_objCommon.Show_Error(4100 + m_nCapBufferCase);
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 12. (Error : 4200)
BOOL CSequenceMain::AssyPicker_Run()
{
	static int nApIdxNo = 0;
	static int nApJobNo = 0;

	static double dApX = 0.0;
	static double dApY = 0.0;
	static double dApZ = 0.0;

	switch (m_nAssyPickCase) {
	case 0:		// Cap Buffer Check
		if (Check_IndexEmpty(1) && !gData.IndexDone[1]) gData.IndexDone[1] = TRUE;
		else if (m_nCapBufferCase == 10) m_nAssyPickCase++;
		return TRUE;

	case 1:		// 안전확인 및 인덱스에 모듈이 있는지 확인
		if (g_objCommon.Check_Position(AX_ASSY_PICKER_X, 0) && g_objCommon.Check_Position(AX_ASSY_PICKER_Y, 0) && g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 0) &&
			g_objCommon.Get_AssyPickerUp(0) && m_pDX08->iAssyPickerTiltUp && !m_pDX08->iAssyPickerTiltDown) 
		{
			if (Check_IndexModule()) 
			{
				m_dwAssyPick = GetTickCount();
				m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(5000);
			}
			else 
			{
				if (Check_IndexEmpty(1) && !gData.IndexDone[1]) gData.IndexDone[1] = TRUE;
				return TRUE;
			}
		}
		break;
	case 2:		// Z Move to Buffer Down, Picker Vac On
		if (g_objCommon.Check_Position(AX_ASSY_PICKER_X, 0) && g_objCommon.Check_Position(AX_ASSY_PICKER_Y, 0) && g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 0) &&
			g_objCommon.Get_AssyPickerUp(0) && m_pDX08->iAssyPickerTiltUp && !m_pDX08->iAssyPickerTiltDown) {
			m_tAssyPickLoop.Takt_Start();
			g_objCommon.Move_Position(AX_ASSY_PICKER_Z, 1);
			g_objCommon.Set_InfoAssyPickerDown(0);
			g_objCommon.Set_InfoAssyPickerVacOn();
			m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(5000);
		}
		break;
	case 3:		// Buffer Align Out
		if (g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 1) && g_objCommon.Get_InfoAssyPickerDown(0)) {
			if (!m_tAssyPickLoop.Waiting_Time(m_pEquipData->nDelayAdd[2])) break;
			g_objCommon.Set_CapBufferAlign(FALSE);
			m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(5000);
		}
		break;
	case 4:		// 정보전달, Vac On Delay, Z Move to Ready Up 
		if (g_objCommon.Get_CapBufferAlign(FALSE)) 
		{
			for (int i = 0; i < PICK; i++) 
			{
				if (gData.InfoAssyPick[i] > 0) { gData.InfoAssyPick[i] = 9; continue; }
				gData.InfoAssyPick[i] = gData.InfoCapBuffer[i]; gData.InfoCapBuffer[i] = 0;
				gData.nCapTNoAssyPicker[i] = gData.nCapTNoCapBuffer[i]; gData.nCapTNoCapBuffer[i] = 0;
				gData.nCapNoAssyPicker[i] = gData.nCapNoCapBuffer[i]; gData.nCapNoCapBuffer[i]  = 0;
				gData.sCIDAssyPicker[i] = gData.sCIDCapBuffer[i];
			}
			g_objCommon.Set_AssyPickerUp(0);
			g_objCommon.Move_Position(AX_ASSY_PICKER_Z, 0);
			m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(10000);
		}
		break;
	case 5:		// X/Y/Z Move to Inspection Position
		if (g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 0) && g_objCommon.Get_AssyPickerUp(0) && g_objCommon.Get_InfoAssyPickerVacOn()) {
			m_tAssyPickLoop.Takt_Save(11, 1);
			m_tAssyPickLoop.Takt_Start();

			//Loadcell 측정 해야하는지 확인.
			if (m_pEquipData->nLoadCellChkCnt != 0 && m_pEquipData->nCappingCnt >= m_pEquipData->nLoadCellChkCnt) {
				nApJobNo = 0;	// 측정하는 피커 시작 번호
				dApY = m_pMoveData->dCapBufferY[3];
				g_objAJinAXL.Move_Absolute(AX_CAP_BUFFER_Y, dApY);	// Load Cell Position	
				m_nAssyPickCase = 70; m_tAssyPickLoop.Set_LoopTime(10000);
			} else {
				
				g_objCommon.Move_Position(AX_ASSY_PICKER_X, 1);
				g_objCommon.Move_Position(AX_ASSY_PICKER_Y, 1);
				g_objCommon.Move_Position(AX_ASSY_PICKER_Z, 2);
				m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(10000);
			}
		}
		break;
	case 6:		// Position Check
		if (g_objCommon.Check_Position(AX_ASSY_PICKER_X, 1) && g_objCommon.Check_Position(AX_ASSY_PICKER_Y, 1) && g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 2)) {
				if(m_nCapBufferCase != 10) break;
				if (m_nCapBufferCase == 10) m_nCapBufferCase = 11;
			m_tAssyPickLoop.Takt_Save(11, 2);
			m_nAssyPickCase = 10; m_tAssyPickLoop.Set_LoopTime(5000);
		}
		break;

	case 10:	// Waiting
		return TRUE;

	case 11:	// X/Y Move to Index Position
		if (g_objCommon.Check_Position(AX_ASSY_PICKER_X, 1) && g_objCommon.Check_Position(AX_ASSY_PICKER_Y, 1) && g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 2)) {
			m_tAssyPickLoop.Takt_Start();
			nApIdxNo = g_objCommon.Get_MainIndexPos(2);
			g_objCommon.Move_Position(AX_ASSY_PICKER_X, 2+nApIdxNo);
			g_objCommon.Move_Position(AX_ASSY_PICKER_Y, 2+nApIdxNo);
			g_objCommon.Move_Position(AX_ASSY_PICKER_Z, 0);
			m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(30000);
		}
		break;
	case 12:	// Position Check
		if (g_objCommon.Check_Position(AX_ASSY_PICKER_X, 2+nApIdxNo) && g_objCommon.Check_Position(AX_ASSY_PICKER_Y, 2+nApIdxNo) && g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 0)) {
			m_nAssyPickCase = 15; m_tAssyPickLoop.Set_LoopTime(5000);
		}
		break;
	case 15:	// Index Check
		if (m_nMainIndexCase <= 10 && !gData.IndexDone[1]) {
			if (Check_IndexEmpty(1)) {
				gData.IndexDone[1] = TRUE;
			} else {
				m_nAssyPickCase = 16; m_tTransStageLoop.Set_LoopTime(5000);
			}
		}
		return TRUE;

	// Cap Index Down
	case 16:	// Index Assy Vac Up
		m_tAssyPickLoop.Takt_Save(11, 3);
		m_tAssyPickLoop.Takt_Start();

		nApIdxNo = g_objCommon.Get_MainIndexPos(2);
		g_objCommon.Move_Position(AX_ASSY_PICKER_X, 2+nApIdxNo);
		g_objCommon.Move_Position(AX_ASSY_PICKER_Y, 2+nApIdxNo);

		m_pDY11->oIndexAssyVacUp = TRUE;
		g_objAJinAXL.Write_Output(11);
		m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(5000);
		break;
	case 17:	// Index Assy Vac On
		if (m_pDX11->iIndexAssyVacUp && !m_pDX11->iIndexAssyVacDown) {
			m_tAssyPickLoop.Takt_Save(11, 4);
			m_tAssyPickLoop.Takt_Start();
			g_objCommon.Set_InfoIndexAssyVacuumOn();
			m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(5000);
		}
		break;
	case 18:	// Index Assy Align Out
		m_tAssyPickLoop.Takt_Save(11, 5);
		if (Check_CapInspAllGood()) {
			m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(5000);
		} else {
			m_nAssyPickCase = 50; m_tAssyPickLoop.Set_LoopTime(5000);
		}
		break;
	case 19:	// Z Move to Index Down
		if (g_objCommon.Check_Position(AX_ASSY_PICKER_X, 2+nApIdxNo) && g_objCommon.Check_Position(AX_ASSY_PICKER_Y, 2+nApIdxNo) &&
			m_pDX11->iIndexAssyAlignIn && !m_pDX11->iIndexAssyAlignOut) {
			SYSTEMTIME time;
			GetLocalTime(&time);
			for (int i = 0; i < PICK; i++) {
				if (gData.InfoIndex[1][i] == 0) continue;
				int nTNo = gData.nTNoIndex[1][i] - 1;
				int nCNo = gData.nCNoIndex[1][i] - 1;
				gData.sCapAttachStart[nTNo][nCNo].Format("%04d-%02d-%02d %02d:%02d:%02d:%03d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
			}

			m_tAssyPickLoop.Takt_Start();
			g_objCommon.Set_InfoAssyPickerDown(1);
			g_objCommon.Move_Position(AX_ASSY_PICKER_Z, 3);	// Assembly Down Position
 			if (!m_pEquipData->bUseIndexAssyVac) g_objCommon.Set_IndexAssyVacuumOff(0);
			m_nAssyPickCase = 25; m_tAssyPickLoop.Set_LoopTime(5000);
		}
		break;

	// Cap Press & Tilt Check
	case 25:	// Z Move to Cap Press
		if (g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 3) && g_objCommon.Get_InfoAssyPickerDown(1)) { 
			if (!m_tAssyPickLoop.Waiting_Time(m_pEquipData->nDelayAdd[5])) break;	// Assy Picker Cap Press Delay
			dApZ = m_pMoveData->dAssyPickerZ[4];	// Cap Press
			g_objAJinAXL.Move_AbsSlow(AX_ASSY_PICKER_Z, dApZ, 0.5);	// Slow
			m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(5000);
		}
		break;
	case 26:	// Assy Picker Vac Off
		if (g_objAJinAXL.Is_MoveDone(AX_ASSY_PICKER_Z, dApZ) && g_objCommon.Get_InfoAssyPickerDown(1)) {
			if(!m_tAssyPickLoop.Waiting_Time(100)) break;	//2021.06.23c
			g_objCommon.Set_InfoAssyPickerVacOff();
			m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(5000);
		}
		break;
	case 27:	// Z Move to Tilt Check
		if (g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 4)) {
			if (!m_tAssyPickLoop.Waiting_Time(m_pEquipData->nVacOffDelay[1])) break;
			if (m_pEquipData->bChkAssyPickerTilt) {
				g_objCommon.Set_AssyPickerUp(0);
				g_objCommon.Move_Position(AX_ASSY_PICKER_Z, 5);	// Tilt Position
				m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(5000);
			} else {
				g_objCommon.Set_AssyPickerUp(0);
				g_objCommon.Move_Position(AX_ASSY_PICKER_Z, 0);	// Ready Position
				g_objCommon.Set_InfoAssyPickerAirOff();
				m_nAssyPickCase = 30; m_tAssyPickLoop.Set_LoopTime(5000);
			}			
		}
		break;
	case 28:	// Vac Off Check & Air Off, X/Y Axis Move to Tilt Check Position
		if (g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 5) && g_objCommon.Get_AssyPickerUp(0) && g_objCommon.Get_InfoAssyPickerVacOff()) {
			m_tAssyPickLoop.Takt_Save(11, 6);
			m_tAssyPickLoop.Takt_Start();
			g_objCommon.Set_InfoAssyPickerAirOff();
			g_objCommon.Move_Position(AX_ASSY_PICKER_X, 6);	// Tilt Position
			g_objCommon.Move_Position(AX_ASSY_PICKER_Y, 6);	// Tilt Position
			m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(5000);
		} else {
			if (!m_pEquipData->bChkAssyPickerTilt) {
				g_objCommon.Set_AssyPickerUp(0);
				g_objCommon.Move_Position(AX_ASSY_PICKER_Z, 0);	// Ready Position
				g_objCommon.Set_InfoAssyPickerAirOff();
				m_nAssyPickCase = 30; m_tAssyPickLoop.Set_LoopTime(5000);
			}
		}
		break;
	case 29:	// Tilt Check Cylinder Down
		if (g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 5) && g_objCommon.Check_Position(AX_ASSY_PICKER_X, 6) && g_objCommon.Check_Position(AX_ASSY_PICKER_Y, 6) &&
			g_objCommon.Get_AssyPickerUp(0) && g_objCommon.Get_InfoAssyPickerVacOff()) {
			m_pDY08->oAssyPickerTiltDown = TRUE;
			g_objAJinAXL.Write_Output(8);
			m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(5000);
		} else {
			if (!m_pEquipData->bChkAssyPickerTilt) {
				g_objCommon.Set_AssyPickerUp(0);
				g_objCommon.Move_Position(AX_ASSY_PICKER_Z, 0);	// Ready Position
				g_objCommon.Set_InfoAssyPickerAirOff();
				m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(5000);
			}
		}
		break;
	case 30:	// Tilt Check
		if (!m_pEquipData->bChkAssyPickerTilt || (m_pDX08->iAssyPickerTiltDown && !m_pDX08->iAssyPickerTiltUp && g_objCommon.Get_InfoAssyPickerTilt(0))) {
			if(!m_tAssyPickLoop.Waiting_Time(150)) break;
			m_tAssyPickLoop.Takt_Save(11, 7);
			CString strLog;
			SYSTEMTIME time;
			GetLocalTime(&time);

			for (int i = 0; i < PICK; i++) {
				if (gData.InfoIndex[1][i] == 0) continue;
				int nPNo = gData.nPNoIndex[1];
				int nTNo = gData.nTNoIndex[1][i];
				int nCNo = gData.nCNoIndex[1][i];
				strLog.Format("%d,%d,-,%d,%d,%d,%s,%0.3lf,%0.3lf,%0.3lf,%0.3lf,%0.3lf,%0.3lf,%0.3lf,%0.3lf,%0.3lf,%0.3lf",
					gData.nCapTNoAssyPicker[i], gData.nCapNoAssyPicker[i], nPNo, nTNo, nCNo, gMes.sBarID[nPNo-1][nTNo-1][nCNo-1],
					gData.dAssyPickOffsetX[i], gData.dAssyPickOffsetY[i], gData.dIndexOffsetX[nApIdxNo][i], gData.dIndexOffsetY[nApIdxNo][i],
					gData.dAlignDiffX[i], gData.dAlignDiffY[i],
					gData.dAssyPickCapSizeX[i], gData.dAssyPickCapSizeY[i], gData.dIndexCmSizeX[nApIdxNo][i], gData.dIndexCmSizeY[nApIdxNo][i]);
				g_objLogFile.Save_CapAssembly(gData.nPNoIndex[1], strLog);

				if (m_pEquipData->nLoadCellChkCnt != 0) m_pEquipData->nCappingCnt++;
					
				gLot.nCapCount[nPNo-1]++;

				if (gData.InfoIndex[1][i] == 9) gData.InfoIndex[1][i] = 1;
				gData.InfoAssyPick[i] = 0;
				gData.nCapTNoAssyPicker[i] = 0;
				gData.nCapNoAssyPicker[i]  = 0;

				gData.sCapAttachEnd[nTNo-1][nCNo-1].Format("%04d-%02d-%02d %02d:%02d:%02d:%03d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
				gData.sCIDIndex[1][i] = gData.sCIDAssyPicker[i];

				// CM/Cap SizeX, Y 초기화
				gData.dAssyPickCapSizeX[i] = gData.dAssyPickCapSizeY[i] = 0.0;
				gData.dIndexCmSizeX[nApIdxNo][i] = gData.dIndexCmSizeY[nApIdxNo][i] = 0.0;
			}
			g_objCommon.Set_IndexAssyVacuumOff(0);
			m_nAssyPickCase = 35; m_tAssyPickLoop.Set_LoopTime(5000);
		}
		break;

	// Picker return
	case 35:	// Picker Up
		if (((m_pEquipData->bChkAssyPickerTilt && m_pDX08->iAssyPickerTiltDown && !m_pDX08->iAssyPickerTiltUp && g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 5)) || 
			(!m_pEquipData->bChkAssyPickerTilt && !m_pDX08->iAssyPickerTiltDown && m_pDX08->iAssyPickerTiltUp && g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 0))) &&
			g_objCommon.Get_AssyPickerUp(0)) {
			m_tAssyPickLoop.Takt_Start();
			m_pDY08->oAssyPickerTiltDown = FALSE;
			m_pDY11->oIndexAssyVacUp = FALSE;
			m_pDY11->oIndexAssyAlignOut = FALSE;
			g_objAJinAXL.Write_Output(8);
			g_objAJinAXL.Write_Output(11);
			g_objCommon.Move_Position(AX_ASSY_PICKER_Z, 0);
			m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(5000);
		}
		break;
	case 36:	// return
		if (!m_pDX08->iAssyPickerTiltDown && m_pDX08->iAssyPickerTiltUp && !m_pDX11->iIndexAssyVacUp && m_pDX11->iIndexAssyVacDown &&
			!m_pDX11->iIndexAssyAlignOut  && m_pDX11->iIndexAssyAlignIn && g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 0) && g_objCommon.Get_AssyPickerUp(0)) {
			m_tAssyPickLoop.Takt_Save(11, 8);
			m_tAssyPickLoop.Takt_Start();

			if (!gData.IndexDone[1]) gData.IndexDone[1] = TRUE;

			g_objCommon.Move_Position(AX_ASSY_PICKER_X, 0);
			g_objCommon.Move_Position(AX_ASSY_PICKER_Y, 0);
			m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(5000);
		}
		break;
	case 37:	// Position Check
		if (g_objCommon.Check_Position(AX_ASSY_PICKER_X, 0) && g_objCommon.Check_Position(AX_ASSY_PICKER_Y, 0) && g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 0) &&
			g_objCommon.Get_AssyPickerUp(0) && !m_pDX08->iAssyPickerTiltDown && m_pDX08->iAssyPickerTiltUp) {
			m_tAssyPickLoop.Takt_Save(11, 9);
			m_nAssyPickCase = 0; m_tAssyPickLoop.Set_LoopTime(5000);

			m_strLog.Format("AssyPicker, %d", GetTickCount() - m_dwAssyPick);
			g_objLogFile.Save_TestLog(m_strLog);
		}
		break;

	case 50:	// Z Move to Index Down, Good Cap Down
		if (m_pDX11->iIndexAssyAlignIn && !m_pDX11->iIndexAssyAlignOut && g_objCommon.Check_Position(AX_ASSY_PICKER_X, 2+nApIdxNo) &&
			g_objCommon.Check_Position(AX_ASSY_PICKER_Y, 2+nApIdxNo)) {
			SYSTEMTIME time;
			GetLocalTime(&time);
			for (int i = 0; i < PICK; i++) {
				if (gData.InfoIndex[1][i] == 0 || gData.InfoAssyPick[i] != 9) continue;
				int nTNo = gData.nTNoIndex[1][i] - 1;
				int nCNo = gData.nCNoIndex[1][i] - 1;
				gData.sCapAttachStart[nTNo][nCNo].Format("%04d-%02d-%02d %02d:%02d:%02d:%03d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
			}

			m_tAssyPickLoop.Takt_Start();
			g_objCommon.Move_Position(AX_ASSY_PICKER_Z, 4);	//2021.06.23c
			g_objCommon.Set_InfoAssyPickerGoodDown();	// Good Down

			if (!m_pEquipData->bUseIndexAssyVac) g_objCommon.Set_IndexAssyVacuumOff(0);
			m_nAssyPickCase = 56; m_tAssyPickLoop.Set_LoopTime(5000);	//2021.06.23c
		}
		break;
	case 51:	// Position Check
		if (g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 3) && g_objCommon.Get_InfoAssyPickerGoodDown()) {
			if(!m_tAssyPickLoop.Waiting_Time(100)) break;
			m_nAssyPickCase = 55; m_tAssyPickLoop.Set_LoopTime(5000);
		}
		break;

	// Cap Press
	case 55:	// Z Move to Cap Press
		if (g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 3) && g_objCommon.Get_InfoAssyPickerGoodDown()) {
			dApZ = m_pMoveData->dAssyPickerZ[4];	// Cap Press
			g_objAJinAXL.Move_AbsSlow(AX_ASSY_PICKER_Z, dApZ, 0.5);	// Slow
			m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(5000);
		}
		break;
	case 56:	// Assy Picker Vac Off
		if (g_objAJinAXL.Is_MoveDone(AX_ASSY_PICKER_Z, dApZ) && g_objCommon.Get_InfoAssyPickerGoodDown()) {
			if(!m_tAssyPickLoop.Waiting_Time(100)) break;	//2021.06.23c
			g_objCommon.Set_InfoAssyPickerGoodVacOff();	// Good Vac Off
			m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(5000);
		}
		break;
	case 57:	// Z Move to Index Down, Picker Up
		if (g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 4)) {
			if (!m_tAssyPickLoop.Waiting_Time(m_pEquipData->nVacOffDelay[1])) break;

			if (gData.InfoIndex[1][0] > 0 && gData.InfoAssyPick[0] == 1) { gData.InfoAssyPick[0] = 9; }
			if (gData.InfoIndex[1][1] > 0 && gData.InfoAssyPick[1] == 1) { gData.InfoAssyPick[1] = 9; }
			if (gData.InfoIndex[1][2] > 0 && gData.InfoAssyPick[2] == 1) { gData.InfoAssyPick[2] = 9; }

			g_objCommon.Set_AssyPickerUp(0);
			g_objCommon.Move_Position(AX_ASSY_PICKER_Z, 3);	// Index Down
			g_objCommon.Set_InfoAssyPickerGoodAirOff();	// Good Air Off
			m_nAssyPickCase = 60; m_tAssyPickLoop.Set_LoopTime(5000);			
		}
		break;

	// Tilt Check
	case 60:	// NG Check, X, Y Move
		// Align Offset X, Y 방향을 알아야한다. 카메라 기준이라면 X, Y 옵셋 값이 반대일수 있다.
		if (Select_AssyPickerNgPos(nApJobNo)) {
			nApIdxNo = g_objCommon.Get_MainIndexPos(2);
			dApX = m_pMoveData->dAssyPickerX[2+nApIdxNo] + gData.dAlignDiffX[nApJobNo];
			dApY = m_pMoveData->dAssyPickerY[2+nApIdxNo] + gData.dAlignDiffY[nApJobNo];
			g_objAJinAXL.Move_Absolute(AX_ASSY_PICKER_X, dApX);
			g_objAJinAXL.Move_Absolute(AX_ASSY_PICKER_Y, dApY);
			m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(5000);
		} else {
			if (g_objAJinAXL.Is_Done(AX_ASSY_PICKER_Z)) {
				if (m_pEquipData->bChkAssyPickerTilt) {
					g_objCommon.Set_AssyPickerUp(0);
					g_objCommon.Move_Position(AX_ASSY_PICKER_Z, 5);	// Tilt Position
					m_nAssyPickCase = 28; m_tAssyPickLoop.Set_LoopTime(5000);
				} else {
					g_objCommon.Set_AssyPickerUp(0);
					g_objCommon.Move_Position(AX_ASSY_PICKER_Z, 0);	// Ready Position
					g_objCommon.Set_InfoAssyPickerAirOff();
					m_nAssyPickCase = 30; m_tAssyPickLoop.Set_LoopTime(5000);
				}
			}
		}
		break;
	case 61:	// Picker Down
		if (m_pDX11->iIndexAssyAlignIn && !m_pDX11->iIndexAssyAlignOut && g_objAJinAXL.Is_Done(AX_ASSY_PICKER_Z) &&
			g_objAJinAXL.Is_MoveDone(AX_ASSY_PICKER_X, dApX) && g_objAJinAXL.Is_MoveDone(AX_ASSY_PICKER_Y, dApY)) {
			SYSTEMTIME time;
			GetLocalTime(&time);
			//for (int i = 0; i < PICK; i++) {
				if (gData.InfoAssyPick[nApJobNo] == 2) {
					int nTNo = gData.nTNoIndex[1][nApJobNo] - 1;
					int nCNo = gData.nCNoIndex[1][nApJobNo] - 1;
					gData.sCapAttachStart[nTNo][nCNo].Format("%04d-%02d-%02d %02d:%02d:%02d:%03d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
				}
			//}

			m_tAssyPickLoop.Takt_Start();
			g_objCommon.Move_Position(AX_ASSY_PICKER_Z, 4);	//2021.06.23c
			g_objCommon.Set_AssyPickerDown(nApJobNo+1);

			if (!m_pEquipData->bUseIndexAssyVac) g_objCommon.Set_IndexAssyVacuumOff(0);
			m_nAssyPickCase = 66; m_tAssyPickLoop.Set_LoopTime(5000);
		}
		break;
	case 62:	// Position Check
		if (g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 3) && g_objCommon.Get_AssyPickerDown(nApJobNo+1)) {
			if(!m_tAssyPickLoop.Waiting_Time(100)) break;
			m_nAssyPickCase = 65; m_tAssyPickLoop.Set_LoopTime(5000);
		}
		break;

	// Cap Press & Tilt Check
	case 65:	// Z Move to Cap Press
		if (g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 3) && g_objCommon.Get_AssyPickerDown(nApJobNo+1)) {
			dApZ = m_pMoveData->dAssyPickerZ[4];	// Cap Press
			g_objAJinAXL.Move_AbsSlow(AX_ASSY_PICKER_Z, dApZ, 0.5);	// Slow
			m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(5000);
		}
		break;
	case 66:	// Assy Picker Vac Off
		if (g_objAJinAXL.Is_MoveDone(AX_ASSY_PICKER_Z, dApZ) && g_objCommon.Get_AssyPickerDown(nApJobNo+1)) {
			if(!m_tAssyPickLoop.Waiting_Time(100)) break;	//2021.06.23c
			g_objCommon.Set_AssyPickerVacOff(nApJobNo+1);
			m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(5000);
		}
		break;
	case 67:	// Z Move to Index Down, Picker Up
		if (g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 4)) {
			if (!m_tAssyPickLoop.Waiting_Time(m_pEquipData->nVacOffDelay[1])) break;

			if (gData.InfoIndex[1][nApJobNo] > 0 && gData.InfoAssyPick[nApJobNo] == 2) { gData.InfoAssyPick[nApJobNo] = 9; }
			g_objCommon.Set_AssyPickerUp(0);
			g_objCommon.Move_Position(AX_ASSY_PICKER_Z, 3);	// Index Down
			m_nAssyPickCase = 60; m_tAssyPickLoop.Set_LoopTime(5000);			
		}
		break;

	// Load Cell 측정.
	case 70:	// Assy Picker Cylinder Down
		if (g_objCommon.Get_AssyPickerUp(0) && g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 0) && g_objAJinAXL.Is_MoveDone(AX_CAP_BUFFER_Y, dApY)) {
			nApJobNo++;
			if (nApJobNo > PICK) {
				nApJobNo = 0;
				m_pEquipData->nCappingCnt = 0;

				CCME8000Dlg *pMainDlg = (CCME8000Dlg*)AfxGetApp()->m_pMainWnd;
				pMainDlg->Save_EquipCappingCnt();

				g_objCommon.Set_AssyPickerUp(0);
				g_objCommon.Move_Position(AX_ASSY_PICKER_Z, 0);	// Ready Up

				m_nAssyPickCase = 5; m_tAssyPickLoop.Set_LoopTime(10000);
			} else {
				SYSTEMTIME time;
				GetLocalTime(&time);
				m_strLoadCellStart.Format("%04d-%02d-%02d %02d:%02d:%02d:%03d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

				g_objCommon.Set_AssyPickerDown(nApJobNo);
				m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(10000);
			}
		}
		break;
	case 71:	// Assy Picker Z Move to Load Cell Position
		if (g_objCommon.Get_AssyPickerDown(nApJobNo) && g_objAJinAXL.Is_MoveDone(AX_CAP_BUFFER_Y, dApY)) {
			dApZ = m_pMoveData->dAssyPickerZ[6];	// Load Cell Down
			g_objAJinAXL.Move_AbsSlow(AX_ASSY_PICKER_Z, dApZ, 0.5);	// Load Cell Down Position
			m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(10000);
		}
		break;
	case 72:	// 일정시간 Dealy 후 Load Cell Check
		if (g_objCommon.Get_AssyPickerDown(nApJobNo) && g_objAJinAXL.Is_MoveDone(AX_ASSY_PICKER_Z, dApZ)) {
			if (!m_tAssyPickLoop.Waiting_Time(m_pEquipData->nDelayAdd[4])) break;	// 측정 전 대기시간
			gData.dLoadCell[0] = 0.0;
			g_objLoadCell.Get_Weight(ASSY_LOAD_CELL);
			m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(10000);
		}
		break;
	case 73:	// 측정값 확인 및 Picker Up
		if (gData.bLoadCellComplete) {
			if (gData.dLoadCell[0] >= m_pEquipData->dLoadCellRange[0] && gData.dLoadCell[0] <= m_pEquipData->dLoadCellRange[1]) {
				gData.dAssyLoadCell[nApJobNo-1] = gData.dLoadCell[0];

				SYSTEMTIME time;
				GetLocalTime(&time);
				m_strLoadCellEnd.Format("%04d-%02d-%02d %02d:%02d:%02d:%03d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

				CString strLog;
				strLog.Format("%s,%s,%d,%0.3lf", m_strLoadCellStart, m_strLoadCellEnd, nApJobNo, gData.dLoadCell[0]);
				g_objLogFile.Save_LoadCellLog(gData.nPNoIndex[1], strLog);

				g_objCommon.Set_AssyPickerUp(0);
				g_objCommon.Move_Position(AX_ASSY_PICKER_Z, 0);
				m_nAssyPickCase++; m_tAssyPickLoop.Set_LoopTime(10000);
			}
		}
		break;
	case 74:	// Buffer Stage Y Pitch Move
		if (g_objCommon.Get_AssyPickerUp(0) && g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 0)) {
			if (nApJobNo < PICK) {
				dApY = m_pMoveData->dCapBufferY[3] + nApJobNo * m_pEquipData->dIndexPitch;
				g_objAJinAXL.Move_Absolute(AX_CAP_BUFFER_Y, dApY);
			}
			m_nAssyPickCase = 70; m_tAssyPickLoop.Set_LoopTime(10000);
		}
		break;

	case 75:	// Error 후 처리
		if (g_objCommon.Get_AssyPickerUp(0) && g_objCommon.Check_Position(AX_ASSY_PICKER_Z, 0)) {
			if (m_pEquipData->nLoadCellChkCnt == 0) {	// Load Cell 측정 사용 안함. (
				m_pEquipData->nCappingCnt = 0;
				CCME8000Dlg *pMainDlg = (CCME8000Dlg*)AfxGetApp()->m_pMainWnd;
				pMainDlg->Save_EquipCappingCnt();
				m_nAssyPickCase = 5; m_tAssyPickLoop.Set_LoopTime(10000);
			} else {
				nApJobNo = 0;
				dApY = m_pMoveData->dCapBufferY[3];
				g_objAJinAXL.Move_Absolute(AX_CAP_BUFFER_Y, dApY);	// Load Cell Position	
				m_nAssyPickCase = 70; m_tAssyPickLoop.Set_LoopTime(10000);
			}
		}
		break;
	}

	// 12. (Error : 4200)
	if (m_tAssyPickLoop.Over_LoopTime()) {
		g_objCommon.Show_Error(4200 + m_nAssyPickCase);
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 13. (Error : 4300)
BOOL CSequenceMain::TransStage_Run()
{
	switch (m_nTransStageCase) {
	case 0:		// Index Check
		if (m_nMainIndexCase <= 10 && !gData.IndexDone[2]) {
			m_nTransStageCase = 1; m_tTransStageLoop.Set_LoopTime(5000);
		}
		return TRUE;

	case 1:		// 안전확인
		// Index가 비어 있으면 True로 바꿔준다.
		if (Check_IndexEmpty(2) && !gData.IndexDone[2]) {
			gData.IndexDone[2] = TRUE;
			m_nTransStageCase = 0; m_tTransStageLoop.Set_LoopTime(5000);

		} else if (g_objCommon.Check_Position(AX_TRANS_STAGE_X, 0) &&
			g_objCommon.Check_Position(AX_TRANS_STAGE_Z, 0) &&
			g_objCommon.Check_Position(AX_TRANS_STAGE_T, 0))
		{
			m_dwTransStage = GetTickCount();
			m_nTransStageCase++; m_tTransStageLoop.Set_LoopTime(5000);
		}
		break;
	case 2:		// Clamp Off Check & Z Move to Index Down, Vac On
		if (g_objCommon.Get_TransStageClampOff()) {
			m_tTransStageLoop.Takt_Start();

			g_objCommon.Move_Position(AX_TRANS_STAGE_Z, 1);	// Index Down
			m_nTransStageCase++; m_tTransStageLoop.Set_LoopTime(5000);
		}
		break;
	case 3:		// Index Align Out
		if (g_objCommon.Check_Position(AX_TRANS_STAGE_Z, 1)) {
			m_tTransStageLoop.Takt_Save(12, 1);
			m_tTransStageLoop.Takt_Start();
			g_objCommon.Set_InfoTransStageVacOn();	// Vac 사용안함
			//g_objCommon.Set_TransStageVacOff(0);	// Blower 사용 할꺼면 주석 삭제 
			m_pDY11->oIndexTransAlignOut = TRUE;
			g_objAJinAXL.Write_Output(11);
			m_nTransStageCase++; m_tTransStageLoop.Set_LoopTime(5000);
		}
		break;
	case 4:		// 정보전달, Clamp On
		if (m_pDX11->iIndexTransAlignOut && !m_pDX11->iIndexTransAlignIn) {
			m_tTransStageLoop.Takt_Save(12, 2);
			m_tTransStageLoop.Takt_Start();
			for (int i = 0; i < PICK; i++) {
				gData.InfoTransStage[i] = gData.InfoIndex[2][i]; gData.InfoIndex[2][i] = 0;
				gData.nTNoTransStage[i] = gData.nTNoIndex[2][i]; gData.nTNoIndex[2][i] = 0;
				gData.nCNoTransStage[i] = gData.nCNoIndex[2][i]; gData.nCNoIndex[2][i] = 0;

				gData.sCIDTransStage[i] = gData.sCIDIndex[2][i];
			}
			gData.nPNoTransStage = gData.nPNoIndex[2];
			gData.nPNoIndex[2] = 0;
			gData.nINoTransStage = g_objCommon.Get_MainIndexPos(3);

			g_objCommon.Set_TransStageClampOn();
			m_nTransStageCase++; m_tTransStageLoop.Set_LoopTime(5000);
		}
		break;
	case 5:		// Z Move to Ready Up
		if (g_objCommon.Get_TransStageClampOn()) {
			if (!m_tTransStageLoop.Waiting_Time(300)) break;
			m_tTransStageLoop.Takt_Save(12, 3);
			m_tTransStageLoop.Takt_Start();
			g_objCommon.Move_Position(AX_TRANS_STAGE_Z, 0);	//Ready Up
			m_nTransStageCase++; m_tTransStageLoop.Set_LoopTime(5000);
		}
		break;
	case 6:		// Index Align In & X, T Move to Unload Position
		if (/*g_objCommon.Get_InfoTransStageVacOn() && */g_objCommon.Check_Position(AX_TRANS_STAGE_Z, 0)) {
			m_tTransStageLoop.Takt_Save(12, 4);
			m_tTransStageLoop.Takt_Start();
			g_objCommon.Move_Position(AX_TRANS_STAGE_X, 1);	// Unload
			g_objCommon.Move_Position(AX_TRANS_STAGE_T, 1);	// Unload

			m_pDY11->oIndexTransAlignOut = FALSE;
			g_objAJinAXL.Write_Output(11);
			m_nTransStageCase++; m_tTransStageLoop.Set_LoopTime(20000);
		}
		break;
	case 7:		// Align In Check & Index Done
		if (!m_pDX11->iIndexTransAlignOut && m_pDX11->iIndexTransAlignIn) {
			m_tTransStageLoop.Takt_Save(12, 5);
			gData.IndexDone[2] = TRUE;
			m_nTransStageCase++; m_tTransStageLoop.Set_LoopTime(5000);
		}
		break;
	case 8:		// X, T Position Check & Z Move to Unload Up
		if (g_objCommon.Check_Position(AX_TRANS_STAGE_X, 1) && g_objCommon.Check_Position(AX_TRANS_STAGE_T, 1)) {
			m_tTransStageLoop.Takt_Save(12, 6);
			m_tTransStageLoop.Takt_Start();
			//g_objCommon.Set_TransStageVacOff(0);
			g_objCommon.Move_Position(AX_TRANS_STAGE_Z, 2);
			m_nTransStageCase++; m_tTransStageLoop.Set_LoopTime(5000);
		}
		break;
	case 9:		// Position Check
		if (g_objCommon.Check_Position(AX_TRANS_STAGE_Z, 2)) {
			m_tTransStageLoop.Takt_Save(12, 7);
			m_nTransStageCase++; m_tTransStageLoop.Set_LoopTime(5000);
		}
		break;
	case 10:	// Unloading
		return TRUE;

	case 11:	// Z Move to Ready Up
		if (g_objCommon.Check_Position(AX_TRANS_STAGE_Z, 2) && g_objCommon.Get_TransStageClampOff()) {
			m_tTransStageLoop.Takt_Start();
			g_objCommon.Move_Position(AX_TRANS_STAGE_Z, 0);
			m_nTransStageCase++; m_tTransStageLoop.Set_LoopTime(5000);
		}
		break;
	case 12:	// return
		if (g_objCommon.Check_Position(AX_TRANS_STAGE_Z, 0)) {
			m_tTransStageLoop.Takt_Save(12, 8);
			m_tTransStageLoop.Takt_Start();
			g_objCommon.Move_Position(AX_TRANS_STAGE_X, 0);	// Load
			g_objCommon.Move_Position(AX_TRANS_STAGE_T, 0);	// Load
			m_nTransStageCase++; m_tTransStageLoop.Set_LoopTime(5000);
		}
		break;
	case 13:	// Position Check
		if (g_objCommon.Check_Position(AX_TRANS_STAGE_X, 0) && g_objCommon.Check_Position(AX_TRANS_STAGE_T, 0)) {
			m_tTransStageLoop.Takt_Save(12, 9);
			m_nTransStageCase = 0; m_tTransStageLoop.Set_LoopTime(5000);

			m_strLog.Format("TransStage, %d", GetTickCount() - m_dwTransStage);
			g_objLogFile.Save_TestLog(m_strLog);
		}
		break;
	}

	// 13. (Error : 4300)
	if (m_tTransStageLoop.Over_LoopTime()) {
		g_objCommon.Show_Error(4300 + m_nTransStageCase);
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 14. (Error : 4400)
BOOL CSequenceMain::UnloadPicker_Run()
{
	static int	nUpWorkTray= 0;
	
	static int nUpPosX = 0;			// Tray X
	static int nUpPosY = 0;			// Tray Y
	static int nUpStart = 0;		// Picker Start No
	static int nUpPickCnt = 0;		// Picker가 잡을수 있는 수
	static int nUpDownSu = 0;		// 최종적으로 Picker가 Down되는 수

	static int nUpTrayLine = 0;		// Tray Line
	static int nUpTrayCnt = 0;		// Tray Cnt

	static double dUpY = 0.0;		// Picker Y
	static double dUpX = 0.0;		// Tray X

	switch (m_nUnloadPickCase) {
	case 0:		// Trans Stage Check
		if (m_nTransStageCase == 10) { m_nUnloadPickCase++; m_tUnloadPickLoop.Set_LoopTime(5000); }

		if (m_pEquipData->bUseInlineMode) {
			// Avi Lotend가 늦게 들어오거나 타이밍이 안맞아 Unload Picker가 지나쳤을 경우 대기하는데에서 확인해준다.
			if (gData.nTNoUnloadTray != 0 &&
				gData.nTNoUnloadTray == gData.nLastTrayNo[gData.nPNoUnloadTray-1] &&
				gData.nLastTrayNo[gData.nPNoUnloadTray-1] > 0 &&
				Check_LoadTrayLoading(gData.nPNoUnloadTray))
			{
				Job_LotEnd(gData.nPNoUnloadTray);
				gData.nLastTrayNo[gData.nPNoUnloadTray-1] = 0;
				if (m_pThreadBeep == NULL) {
					m_pThreadBeep = AfxBeginThread(Thread_Beep, (LPVOID)(2000));
				}
				gData.nTNoUnloadTray = 0;
				g_dlgWork.PostMessage(UM_SHOW_MSG, 2, NULL);		// 2020.09.14 khs
				if (m_nUnloadStage1Case == 20) { gData.bUnloadTrayLotEnd[0] = TRUE; m_nUnloadStage1Case = 21; }
				if (m_nUnloadStage2Case == 20) { gData.bUnloadTrayLotEnd[1] = TRUE; m_nUnloadStage2Case = 21; }

				// 배출중일때 초기화 할수있도록 gData.bUnloadTrayLotEnd = TRUE
				if (gData.bUnloadTrayLotEnd[0] == FALSE && m_nUnloadStage1Case > 20 && m_nUnloadStage1Case < 30) { gData.bUnloadTrayLotEnd[0] = TRUE; } 
				if (gData.bUnloadTrayLotEnd[1] == FALSE && m_nUnloadStage2Case > 20 && m_nUnloadStage2Case < 30) { gData.bUnloadTrayLotEnd[1] = TRUE; } 

				// 마지막 Tray 배출 이후 다른 스테이지 Empty Tray가 대기위치에 없을때는 여기에서 초기화 해준다.
				if ((m_nUnloadStage1Case >= 30 && m_nUnloadStage2Case < 20) ||
					(m_nUnloadStage2Case >= 30 && m_nUnloadStage1Case < 20) ) {

					m_pDY03->oUnloadPort2SlideLock = FALSE; m_pDY03->oUnloadPort2SlideUnlock = TRUE;
					g_objAJinAXL.Write_Output(3);
					gData.bUnloadPort2Wait = TRUE;
					gData.nPNoUnloadTray = 0;
				}
			}
		}
		return TRUE;

	case 1:		// 안전확인 & Move to Trans Down
		if (g_objCommon.Check_Position(AX_UNLOAD_PICKER_X, 0) && g_objCommon.Check_Position(AX_UNLOAD_PICKER_Z, 0) &&
			g_objCommon.Check_Position(AX_UNLOAD_PICKER_P, 0) && g_objCommon.Get_UnloadPickerUp(0) && g_objCommon.Check_Position(AX_TRANS_STAGE_Z, 2)) {
			m_dwUnloadPick = GetTickCount();
			m_tUnloadPickLoop.Takt_Start();

			g_objCommon.Move_Position(AX_UNLOAD_PICKER_Z, 1);
			g_objCommon.Set_InfoUnloadPickerDown();
			m_nUnloadPickCase++; m_tUnloadPickLoop.Set_LoopTime(5000);
		}		
		break;
	case 2:		// Trans Stage Clamp Off
		if (g_objCommon.Check_Position(AX_UNLOAD_PICKER_Z, 1) && g_objCommon.Get_InfoUnloadPickerDown()) {
			m_tUnloadPickLoop.Takt_Save(13, 1);
			m_tUnloadPickLoop.Takt_Start();
			g_objCommon.Set_TransStageClampOff();
			g_objCommon.Set_TransStageVacOff(0);
			m_nUnloadPickCase++; m_tUnloadPickLoop.Set_LoopTime(5000);
		}
		break;
	case 3:		// 정보전달, Picker Vac On
		if (g_objCommon.Get_TransStageClampOff()) 
		{
// 			if (!m_tUnloadPickLoop.Waiting_Time(m_pEquipData->nDelayAdd[3])) break;
			m_tUnloadPickLoop.Takt_Save(13, 2);
			m_tUnloadPickLoop.Takt_Start();

			for (int i = 0; i < PICK; i++) 
			{
				gData.InfoUnloadPick[i] = gData.InfoTransStage[i]; gData.InfoTransStage[i] = 0;
				gData.nTNoUnloadPick[i] = gData.nTNoTransStage[i]; gData.nTNoTransStage[i] = 0;
				gData.nCNoUnloadPick[i] = gData.nCNoTransStage[i]; gData.nCNoTransStage[i] = 0;

				gData.sCIDUnloadPicker[i] = gData.sCIDTransStage[i];

				if(gData.InfoUnloadPick[i] > 0 && gData.nTNoUnloadPick[i] == 1 && gData.nCNoUnloadPick[i] == 1)
				{
					gLot.dwFirstUnload[gData.nPNoTransStage-1] = GetTickCount();
				}
			}
			gData.nPNoUnloadPick = gData.nPNoTransStage; gData.nPNoTransStage = 0;
			gData.nINoUnloadPicker = gData.nINoTransStage;
			gData.nLNoUnloadPicker = gData.nCNoUnloadPick[0] / PICK;

			g_objCommon.Set_InfoUnloadPickerVacOn();
			m_nUnloadPickCase++; m_tUnloadPickLoop.Set_LoopTime(5000);
		}
		break;
	case 4:		// Vac On Delay, Picker Up
		if (g_objCommon.Get_TransStageClampOff()) {
			if (!m_tUnloadPickLoop.Waiting_Time(m_pEquipData->nDelayAdd[3])) break;

			m_tUnloadPickLoop.Takt_Save(13, 3);
// 			if (m_nTransStageCase == 10) m_nTransStageCase = 11;

			g_objCommon.Set_UnloadPickerUp(0);
			g_objCommon.Move_Position(AX_UNLOAD_PICKER_Z, 0);
			g_objCommon.Set_TransStageAirOff(0);
			
			m_nUnloadPickCase++; m_tUnloadPickLoop.Set_LoopTime(5000);		
		}
		break;
	case 5:		// Check Picker Up, Move to Unload Stage1 Position
		if (g_objCommon.Get_InfoUnloadPickerVacOn() && g_objCommon.Get_UnloadPickerUp(0) && g_objCommon.Check_Position(AX_UNLOAD_PICKER_Z, 0)) {
			m_tUnloadPickLoop.Takt_Start();

			if (m_nTransStageCase == 10) m_nTransStageCase = 11;	// Trans Stage Loading 위치로 이동.

			if (m_nUnloadStage1Case == 20 || m_nUnloadStage2Case == 20) {
				m_nUnloadPickCase = 10; m_tUnloadPickLoop.Set_LoopTime(5000);
			} else {	// Unload Stage 대기위치에 없으면 언로드 대기 위치로 이동.
				g_objCommon.Move_Position(AX_UNLOAD_PICKER_X, 1);
				g_objCommon.Move_Position(AX_UNLOAD_PICKER_P, 1);
				m_nUnloadPickCase++; m_tUnloadPickLoop.Set_LoopTime(5000);
			}	
		}
		break;

	case 6:		// wait
		if (m_nUnloadStage1Case != 20 && m_nUnloadStage2Case != 20) return TRUE;
		if (g_objCommon.Get_InfoUnloadPickerVacOn() && g_objCommon.Get_InfoUnloadPickerCapCheck() &&
			g_objCommon.Check_Position(AX_UNLOAD_PICKER_X, 1) && g_objCommon.Check_Position(AX_UNLOAD_PICKER_P, 1)) 
		{
			m_tUnloadPickLoop.Takt_Save(13, 4);
			m_nUnloadPickCase = 10; m_tUnloadPickLoop.Set_LoopTime(5000);
		}
		break;

	case 10:		
		if (g_objCommon.Get_InfoUnloadPickerVacOn() && g_objCommon.Get_UnloadPickerUp(0) &&
			(g_objCommon.Check_Position(AX_UNLOAD_PICKER_Z, 0) || g_objCommon.Check_Position(AX_UNLOAD_PICKER_Z, nUpWorkTray+1))) 
		{
			 
			if (gData.nPNoUnloadPick != gData.nPNoUnloadTray && gData.nPNoUnloadTray > 0)
			{
				if (m_pEquipData->bUseInlineMode) 
				{
					// Avi Lotend가 늦게 들어오거나 타이밍이 안맞아 Unload Picker가 지나쳤을 경우 대기하는데에서 확인해준다.
					if (gData.nTNoUnloadTray != 0 && gData.nTNoUnloadTray == gData.nLastTrayNo[gData.nPNoUnloadTray-1] &&
						gData.nLastTrayNo[gData.nPNoUnloadTray-1] > 0 && Check_LoadTrayLoading(gData.nPNoUnloadTray))
					{
						Job_LotEnd(gData.nPNoUnloadTray);
						gData.nLastTrayNo[gData.nPNoUnloadTray-1] = 0;
						if (m_pThreadBeep == NULL) {
							m_pThreadBeep = AfxBeginThread(Thread_Beep, (LPVOID)(2000));
						}
						gData.nTNoUnloadTray = 0;
						g_dlgWork.PostMessage(UM_SHOW_MSG, 2, NULL);		// 2020.09.14 khs
						if (m_nUnloadStage1Case == 20) { gData.bUnloadTrayLotEnd[0] = TRUE; m_nUnloadStage1Case = 21; }
						if (m_nUnloadStage2Case == 20) { gData.bUnloadTrayLotEnd[1] = TRUE; m_nUnloadStage2Case = 21; }

						// 배출중일때 초기화 할수있도록 gData.bUnloadTrayLotEnd = TRUE
						if (gData.bUnloadTrayLotEnd[0] == FALSE && m_nUnloadStage1Case > 20 && m_nUnloadStage1Case < 30) { gData.bUnloadTrayLotEnd[0] = TRUE; } 
						if (gData.bUnloadTrayLotEnd[1] == FALSE && m_nUnloadStage2Case > 20 && m_nUnloadStage2Case < 30) { gData.bUnloadTrayLotEnd[1] = TRUE; } 

						// 마지막 Tray 배출 이후 다른 스테이지 Empty Tray가 대기위치에 없을때는 여기에서 초기화 해준다.
						if ((m_nUnloadStage1Case >= 30 && m_nUnloadStage2Case < 20) || (m_nUnloadStage2Case >= 30 && m_nUnloadStage1Case < 20)) 
						{
							m_pDY03->oUnloadPort2SlideLock = FALSE; m_pDY03->oUnloadPort2SlideUnlock = TRUE;
							g_objAJinAXL.Write_Output(3);
							gData.bUnloadPort2Wait = TRUE;
							gData.nPNoUnloadTray = 0;
						}
					}
				}
				return TRUE;
			}
			if (m_nUnloadStage1Case != 20 && m_nUnloadStage2Case != 20) return TRUE;

			if (m_nUnloadStage1Case == 20) { nUpWorkTray = 1; dUpY = g_objAJinAXL.Get_Position(AX_UNLOAD_STAGE1_Y); }
			if (m_nUnloadStage2Case == 20) { nUpWorkTray = 2; dUpY = g_objAJinAXL.Get_Position(AX_UNLOAD_STAGE2_Y); }
			dUpX = g_objAJinAXL.Get_Position(AX_UNLOAD_PICKER_X);

			m_nUnloadPickCase++; m_tUnloadPickLoop.Set_LoopTime(5000);
		}
		break;
	case 11:	// Move to Unload Tray, Tray Position Check
		if (((nUpWorkTray == 1 && g_objAJinAXL.Is_MoveDone(AX_UNLOAD_STAGE1_Y, dUpY)) ||
			 (nUpWorkTray == 2 && g_objAJinAXL.Is_MoveDone(AX_UNLOAD_STAGE2_Y, dUpY))) &&
			g_objAJinAXL.Is_MoveDone(AX_UNLOAD_PICKER_X, dUpX))
		{
			if (m_nUnloadStage1Case != 20 && m_nUnloadStage2Case != 20) break;

			if (Select_UnloadPickerPos(nUpStart, nUpPickCnt)) {
				if (Select_UnloadTrayPos(nUpPosX, nUpPosY)){
					m_tUnloadPickLoop.Takt_Start();

					nUpTrayCnt = ST_X - nUpPosX;
					nUpDownSu = ((nUpPickCnt < nUpTrayCnt) ? nUpPickCnt : nUpTrayCnt);

					dUpX = m_pMoveData->dUnloadPickerX[nUpWorkTray] - (nUpPosX - nUpStart) * m_pEquipData->dShipTrayPitchX;
					if (nUpWorkTray == 1) dUpY = m_pMoveData->dUnloadStage1Y[1] + nUpPosY * m_pEquipData->dShipTrayPitchY;
					if (nUpWorkTray == 2) dUpY = m_pMoveData->dUnloadStage2Y[1] + nUpPosY * m_pEquipData->dShipTrayPitchY;					

					g_objAJinAXL.Move_Absolute(AX_UNLOAD_PICKER_X, dUpX);		
					if (nUpWorkTray == 1) g_objAJinAXL.Move_Absolute(AX_UNLOAD_STAGE1_Y, dUpY);
					if (nUpWorkTray == 2) g_objAJinAXL.Move_Absolute(AX_UNLOAD_STAGE2_Y, dUpY);

					g_objCommon.Move_Position(AX_UNLOAD_PICKER_P, 1);

					m_nUnloadPickCase++; m_tUnloadPickLoop.Set_LoopTime(10000);
				}
			}
		}
		break;
	case 12:	// Picker Down
		if (((nUpWorkTray == 1 && g_objAJinAXL.Is_MoveDone(AX_UNLOAD_STAGE1_Y, dUpY)) ||
			(nUpWorkTray == 2 && g_objAJinAXL.Is_MoveDone(AX_UNLOAD_STAGE2_Y, dUpY))) &&
			g_objAJinAXL.Is_MoveDone(AX_UNLOAD_PICKER_X, dUpX) && g_objCommon.Check_Position(AX_UNLOAD_PICKER_P, 1) &&
			g_objCommon.Get_InfoUnloadPickerCapCheck()) {
			m_tUnloadPickLoop.Takt_Save(13, 5);
			m_tUnloadPickLoop.Takt_Start();

			g_objCommon.Move_Position(AX_UNLOAD_PICKER_Z, nUpWorkTray+1);
			g_objCommon.Set_UnloadPickerDownMulti(nUpStart+1, nUpDownSu);
			m_nUnloadPickCase++; m_tUnloadPickLoop.Set_LoopTime(10000);
		}
		break;
	case 13:	// Vac Off
		if (g_objCommon.Check_Position(AX_UNLOAD_PICKER_Z, nUpWorkTray+1) && g_objCommon.Get_UnloadPickerDownMulti(nUpStart+1, nUpDownSu)) {
			g_objCommon.Set_UnloadPickerVacOffMulti(nUpStart+1, nUpDownSu);
			m_nUnloadPickCase++; m_tUnloadPickLoop.Set_LoopTime(10000);
		}
		break;
	case 14:	// 정보전달, Picker Up
		if (g_objCommon.Check_Position(AX_UNLOAD_PICKER_Z, nUpWorkTray+1) && g_objCommon.Get_UnloadPickerDownMulti(nUpStart+1, nUpDownSu)) {
			if (!m_tUnloadPickLoop.Waiting_Time(m_pEquipData->nVacOffDelay[2])) break;

			gData.nULPNo = gData.nPNoUnloadPick;
			for (int i = 0; i < nUpDownSu; i++) {
				//모듈트랙킹
				int nPNo = gData.nPNoUnloadPick - 1;
				int nTNo = gData.nTNoUnloadPick[nUpStart+i] - 1;
				int nCNo = gData.nCNoUnloadPick[nUpStart+i] - 1;
				gData.nCmJigNo[nPNo][nTNo][nCNo][UNLOAD_PICK] = nUpStart + i + 1;	// Unload Pick
				g_objLogFile.Save_CmTrackingLog("GOOD", nUpWorkTray, nUpPosX + i + 1, nUpPosY + 1, gData.nPNoUnloadPick, gData.nTNoUnloadPick[nUpStart+i], gData.nCNoUnloadPick[nUpStart+i]);
				g_objLogFile.Save_CapLasLog(gData.sShipLotID, gData.sCIDUnloadPicker[i], gData.nPNoUnloadPick, gData.nTNoUnloadPick[nUpStart+i], gData.nCNoUnloadPick[nUpStart+i], nUpStart+i+1);

				gData.InfoShipTray[nUpPosY][nUpPosX+i] = gData.InfoUnloadPick[nUpStart+i]; gData.InfoUnloadPick[nUpStart+i] = 0;
				gData.nTNoUnloadTray = gData.nTNoUnloadPick[nUpStart+i];
				gData.nTNoUnloadPick[nUpStart+i] = gData.nCNoUnloadPick[nUpStart+i] = 0;
			}
			gData.nPNoUnloadTray = gData.nPNoUnloadPick;

			g_dlgWork.PostMessage(UM_UPDATE_TRAY_INFO, 3, gData.nPNoUnloadTray-1);
			g_dlgWork.PostMessage(UM_VISION_RESULT, gData.nPNoUnloadTray, NULL);
			if (Check_UnloadPickerEmpty()) { gData.nPNoUnloadPick = 0; }

			g_objCommon.Set_UnloadPickerAirOffMulti(nUpStart+1, nUpDownSu);
			g_objCommon.Set_UnloadPickerUp(0);
			m_nUnloadPickCase++; m_tUnloadPickLoop.Set_LoopTime(10000);
		}
		break;
	case 15:	// Position Check, Unload Tray Full Check
		if (g_objCommon.Get_UnloadPickerUp(0) && g_objCommon.Get_InfoUnloadPickerVacOff()) {
			m_tUnloadPickLoop.Takt_Save(13, 6);
			m_tUnloadPickLoop.Takt_Start();
			if (Check_UnloadPickerEmpty() || Check_UnloadTrayFull()) {
				if (m_pEquipData->bUseInlineMode) {
					if ((gData.nTNoUnloadTray == gData.nLastTrayNo[gData.nPNoUnloadTray-1]) &&
						gData.nLastTrayNo[gData.nPNoUnloadTray-1] > 0 &&
						Check_LoadTrayLoading(gData.nPNoUnloadTray))
					{
						Job_LotEnd(gData.nPNoUnloadTray);
						gData.nLastTrayNo[gData.nPNoUnloadTray-1] = 0;
						if (m_pThreadBeep == NULL) {
							m_pThreadBeep = AfxBeginThread(Thread_Beep, (LPVOID)(2000));
						}
						gData.nTNoUnloadTray = 0;
						g_dlgWork.PostMessage(UM_SHOW_MSG, 2, NULL);		// 2020.09.14 khs
						if (m_nUnloadStage1Case == 20) { gData.bUnloadTrayLotEnd[0] = TRUE; m_nUnloadStage1Case = 21; }
						if (m_nUnloadStage2Case == 20) { gData.bUnloadTrayLotEnd[1] = TRUE; m_nUnloadStage2Case = 21; }
					}
				} else {
					if (Check_LoadTrayLoading(gData.nPNoUnloadTray)) {
						Job_LotEnd(gData.nPNoUnloadTray);
						if (m_pThreadBeep == NULL) {
							m_pThreadBeep = AfxBeginThread(Thread_Beep, (LPVOID)(2000));
						}
						gData.nTNoUnloadTray = 0;
						g_dlgWork.PostMessage(UM_SHOW_MSG, 2, NULL);		// 2020.09.14 khs
						if (m_nUnloadStage1Case == 20) { gData.bUnloadTrayLotEnd[0] = TRUE; m_nUnloadStage1Case = 21; }
						if (m_nUnloadStage2Case == 20) { gData.bUnloadTrayLotEnd[1] = TRUE; m_nUnloadStage2Case = 21; }
					}
					if (Check_UnloadLotEnd() && !m_bUnloadLotEnd) { m_bUnloadLotEnd = TRUE; }
				}

				if (Check_UnloadTrayFull()) {
					if (m_nUnloadStage1Case == 20) m_nUnloadStage1Case = 21;
					if (m_nUnloadStage2Case == 20) m_nUnloadStage2Case = 21;
				}

				if (Check_UnloadPickerEmpty()) {
					g_objCommon.Move_Position(AX_UNLOAD_PICKER_Z, 0);	// Ready Up
					m_nUnloadPickCase++; m_tUnloadPickLoop.Set_LoopTime(10000);
				} else {
					m_nUnloadPickCase = 10; m_tUnloadPickLoop.Set_LoopTime(30000);
				}
			} else {
				m_nUnloadPickCase = 10; m_tUnloadPickLoop.Set_LoopTime(30000);
			}
		}
		break;
	case 16:	// return
		if (g_objCommon.Get_UnloadPickerUp(0) && g_objCommon.Check_Position(AX_UNLOAD_PICKER_Z, 0) && g_objCommon.Get_InfoUnloadPickerVacOff()) {
			m_tUnloadPickLoop.Takt_Save(13, 7);
			m_tUnloadPickLoop.Takt_Start();
			g_objCommon.Move_Position(AX_UNLOAD_PICKER_X, 0);
			g_objCommon.Move_Position(AX_UNLOAD_PICKER_P, 0);
			m_nUnloadPickCase++; m_tUnloadPickLoop.Set_LoopTime(10000);
		}
		break;
	case 17:	// Position Check
		if (g_objCommon.Check_Position(AX_UNLOAD_PICKER_X, 0) && g_objCommon.Check_Position(AX_UNLOAD_PICKER_P, 0)) {
			m_tUnloadPickLoop.Takt_Save(13, 8);
			m_nUnloadPickCase = 0; m_tUnloadPickLoop.Set_LoopTime(10000);

			m_strLog.Format("UnloadPicker, %d", GetTickCount() - m_dwUnloadPick);
			g_objLogFile.Save_TestLog(m_strLog);
		}
		break;
	}

	// 14. (Error : 4400)
	if (m_tUnloadPickLoop.Over_LoopTime()) {
		g_objCommon.Show_Error(4400 + m_nUnloadPickCase);
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 15. (Error : 4500)
BOOL CSequenceMain::UnloadStage1_Run()
{
	static int threshold;
		
	if (gData.bUnloadPort1Wait && m_nUnloadStage1Case >  1 && m_nUnloadStage1Case < 10) return TRUE;
	if (gData.bUnloadPort2Wait && m_nUnloadStage1Case > 22 && m_nUnloadStage1Case < 50) return TRUE;
	
	if (gData.bUnloadPort1Wait && (m_nUnloadStage1Case == -1 || m_nUnloadStage1Case == -2 || m_nUnloadStage1Case == -3 )) return TRUE;



	switch (m_nUnloadStage1Case) 
	{
	case 0:		// Start시 1로 바꿈
		return TRUE;

	case 1:		// 안전확인
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Y, 0) && g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Z, 1) &&
			g_objCommon.Get_UnloadStageMasterSlaveOut(1) && !m_pDX05->iUnloadStage1Exist 
			&& m_pDX03->iUnloadPort1SlideClose && m_pDX03->iUnlaodPort1LowCheck)
		{
			m_dwUnloadStage1 = GetTickCount();
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		else
		{
			if (!m_pDX03->iUnlaodPort1LowCheck) { g_dlgWork.PostMessage(UM_SHOW_MSG, 5, NULL); return FALSE;}
		}
		break;
	case 2:		// Ship Tray Check, Z Move to Support Up
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Y, 0) && g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Z, 1) &&
			g_objCommon.Get_UnloadStageMasterSlaveOut(1) && !m_pDX05->iUnloadStage1Exist 
			&& m_pDX03->iUnloadPort1SlideClose && m_pDX03->iUnlaodPort1LowCheck)
		{
			if (gData.nShipTrayLoad == 0 || gData.nShipTrayLoad >= gData.nShipTrayMax) {	// Ship Count 확인
				m_nUnloadStage1Case = 15; m_tUnloadStage1Loop.Set_LoopTime(5000);
			} else {
				m_tUnloadStage1Loop.Takt_Start();
				g_objCommon.Move_Position(AX_UNLOAD_STAGE1_Z, 2);
				m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
			}
		} else {
			if (!m_pDX03->iUnlaodPort1LowCheck) { g_dlgWork.PostMessage(UM_SHOW_MSG, 5, NULL); return FALSE;}
		}
		break;
	case 3:		// Stage Master In
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Z, 2)) {
			m_tUnloadStage1Loop.Takt_Save(14, 1);
			m_tUnloadStage1Loop.Takt_Start();
			m_pDY05->oUnloadStage1MasterIn = TRUE;
			g_objAJinAXL.Write_Output(5);
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 4:		// Stage Slave In
		if (m_pDX05->iUnloadStage1MasterIn && !m_pDX05->iUnloadStage1MasterOut) {
			m_pDY05->oUnloadStage1SlaveIn = TRUE;
			g_objAJinAXL.Write_Output(5);
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 5:		// Master/Slave Check
		if (g_objCommon.Get_UnloadStageMasterSlaveIn(1)) {
			m_tUnloadStage1Loop.Takt_Save(14, 6);	// Master/slave In
			m_tUnloadStage1Loop.Takt_Start();
			g_objCommon.Set_UnloadPortSupportOut(1);
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 6:		// Z Move to Support Down
		if (g_objCommon.Get_UnloadPortSupportOut(1)) {
			m_tUnloadStage1Loop.Takt_Save(14, 2);	// Support Out
			m_tUnloadStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_UNLOAD_STAGE1_Z, 3);				
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 7:		// Support In
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Z, 3)) {
			if (!m_tUnloadStage1Loop.Waiting_Time(500)) break;
			m_tUnloadStage1Loop.Takt_Save(14, 3);	// Support Down
			m_tUnloadStage1Loop.Takt_Start();
			g_objCommon.Set_UnloadPortSupportIn(1);
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 8:		// Tray Check, Stage Master In
		if (g_objCommon.Get_UnloadPortSupportIn(1)) {
			if (!m_tUnloadStage1Loop.Waiting_Time(500)) break;
			m_tUnloadStage1Loop.Takt_Save(14, 4);	// Support In
			m_tUnloadStage1Loop.Takt_Start();
			// Port와 Tray가 아슬아슬하여 부딪힐때가 있어 MoveDown에서 이동하도록 한다.
			g_objCommon.Move_Position(AX_UNLOAD_STAGE1_Z, 0);	// Move Down
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 9:		// Z Move to Move Up
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Z, 0) && m_pDX05->iUnloadStage1Exist) {

			m_pDY05->oUnloadStage1MasterIn = FALSE;
			m_pDY05->oUnloadStage1SlaveIn = FALSE;
			g_objAJinAXL.Write_Output(5);
		
			m_tUnloadStage1Loop.Takt_Save(14, 14);
			m_nUnloadStage1Case = -3; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case -3:
		if(g_objCommon.Get_UnloadStageMasterSlaveOut(1))
		{
			m_pDY05->oUnloadStage1MasterIn = TRUE;			
			g_objAJinAXL.Write_Output(5);
			m_nUnloadStage1Case = -2; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case -2:
		if (m_pDX05->iUnloadStage1MasterIn && !m_pDX05->iUnloadStage1MasterOut && !m_pDX05->iUnloadStage1SlaveIn && m_pDX05->iUnloadStage1SlaveOut)
		{
			m_pDY05->oUnloadStage1SlaveIn = TRUE;
			g_objAJinAXL.Write_Output(5);
			m_nUnloadStage1Case = -1; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case -1:
		if(g_objCommon.Get_UnloadStageMasterSlaveIn(1))
		{
			m_nUnloadStage1Case = 10; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 10:	// 안전 확인 
		if (m_nUnloadStage2Case > 22 ) 
		{ 			
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000); 
		}
		return TRUE;

	case 11:	// Y Move to Work Position
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Z, 0) && m_pDX05->iUnloadStage1Exist ) {
			m_tUnloadStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_UNLOAD_STAGE1_Y, 1);	// Work Position
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 12:	// Position check
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Y, 1)) {
			m_tUnloadStage1Loop.Takt_Save(14, 7);
			m_tUnloadStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_UNLOAD_STAGE1_Z, 1);	// Move Up
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 13:
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Z, 1) && m_pDX05->iUnloadStage1Exist ) {
			m_tUnloadStage1Loop.Takt_Save(14, 5);
			Init_UnloadTray();
			gData.nShipTrayLoad++;
			threshold = 0;
			m_nUnloadStage1Case = 14; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 14:
		if(m_pDX05->iUnloadStage1Exist)
		{
			threshold++;
			if(threshold > 10) 
			{
				m_nUnloadStage1Case = 20; m_tUnloadStage1Loop.Set_LoopTime(5000);
			}
		}
		else
		{
			threshold = 0;
		}		
		break;
	case 15:	// Ship 정보 확인 및 MES 자재 등록.
		if (g_dlgWork.Check_ShipLotId()) {
			if (m_pEquipData->bUseMES && m_pEquipData->bUseMesShipReg) {	// MES Ship 자재등록 사용
				g_objMES.m_nMESShipSequence = 0;
				g_dlgWork.Get_ShipLotId();
				g_objMES.Set_ShipLotChangeRequest(gData.sShipLotID, gData.sOperID);	// 자재 등록 요청
			}
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(30000);
		} else {
			g_objCommon.Show_Alarm("Ship Tray 자재 정보를 입력해 주십시오.");
			return FALSE;
		}
		break;
	case 16:	// MES 결과 확인.
		if (g_objMES.m_nMESShipSequence == 2 || !m_pEquipData->bUseMES || !m_pEquipData->bUseMesShipReg) {
			g_dlgWork.Change_ShipLotId();	// 정보 이동.
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 17:
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Y, 0) && g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Z, 1) &&
			g_objCommon.Get_UnloadStageMasterSlaveOut(1) && !m_pDX05->iUnloadStage1Exist &&
			m_pDX03->iUnloadPort1SlideClose && m_pDX03->iUnlaodPort1LowCheck)
		{
			m_tUnloadStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_UNLOAD_STAGE1_Z, 2);				
			m_nUnloadStage1Case = 3; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;

	case 20:	// Working
		
		return TRUE;

	case 21:	// 안전 확인
		if (m_nUnloadStage2Case <= 10 || m_nUnloadStage2Case > 52) {
			if (g_objAJinAXL.Is_Done(AX_UNLOAD_STAGE1_Y)) {
				m_tUnloadStage1Loop.Takt_Start();
				// Port와 Tray가 아슬아슬하여 부딪힐때가 있어 MoveDown에서 이동하도록 한다.
				g_objCommon.Move_Position(AX_UNLOAD_STAGE1_Z, 0);	// Move Down
				m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
			}
		}
		return TRUE;
	case 22:	// Y Move to Unload Port Position
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Z, 0)) {
			m_tUnloadStage1Loop.Takt_Save(14, 14);
			m_tUnloadStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_UNLOAD_STAGE1_Y, 2);
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 23:	// Z Move to Support Down
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Y, 2) && m_pDX05->iUnloadStage1Exist
			 && m_pDX03->iUnloadPort2SlideClose) {
			if ((gData.nPNoUnloadTray != gData.nPNoUnloadPort) && gData.nPNoUnloadPort != 0) return TRUE;

			m_tUnloadStage1Loop.Takt_Save(14, 8);
			m_tUnloadStage1Loop.Takt_Start();
			gData.nPNoUnloadPort = gData.nPNoUnloadTray;
			g_objCommon.Move_Position(AX_UNLOAD_STAGE1_Z, 5);	// Support Down
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 24:	// Support Out
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Z, 5)) {
			m_tUnloadStage1Loop.Takt_Save(14, 9);
			m_tUnloadStage1Loop.Takt_Start();
			g_objCommon.Set_UnloadPortSupportOut(2);
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 25:	// Z Move to Support Up
		if (g_objCommon.Get_UnloadPortSupportOut(2)) {
			m_tUnloadStage1Loop.Takt_Save(14, 10);
			m_tUnloadStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_UNLOAD_STAGE1_Z, 4);	// Support Up				
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 26:	// Support In
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Z, 4)) {
			m_tUnloadStage1Loop.Takt_Save(14, 11);
			m_tUnloadStage1Loop.Takt_Start();
			g_objCommon.Set_UnloadPortSupportIn(2);
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 27:	// Slave Out
		if (g_objCommon.Get_UnloadPortSupportIn(2) && g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Z, 4)) {
			m_tUnloadStage1Loop.Takt_Save(14, 12);
			m_tUnloadStage1Loop.Takt_Start();
			m_pDY05->oUnloadStage1SlaveIn = FALSE;
			g_objAJinAXL.Write_Output(5);
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 28:	// Master Out
		if (!m_pDX05->iUnloadStage1SlaveIn && m_pDX05->iUnloadStage1SlaveOut) {
			m_pDY05->oUnloadStage1MasterIn = FALSE;
			g_objAJinAXL.Write_Output(5);
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 29:	// Stage2 Check, Z Move to Move Down
		if (g_objCommon.Get_UnloadStageMasterSlaveOut(1)) {
			m_tUnloadStage1Loop.Takt_Save(14, 13);
			m_tUnloadStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_UNLOAD_STAGE1_Z, 0);	// Move Down				
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 30:	// Position Check
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Z, 0) && !m_pDX05->iUnloadStage1Exist ) {
			m_tUnloadStage1Loop.Takt_Save(14, 14);
			if (gData.bUnloadTrayLotEnd[0] && m_bUnloadLotEnd) {	// 도어락 오픈 후처리 확인.
				m_pDY03->oUnloadPort2SlideLock = FALSE; m_pDY03->oUnloadPort2SlideUnlock = TRUE;
				//m_pDY13->oDoor05Unlock = TRUE;
				//m_pDY13->oDoor06Unlock = TRUE;
				g_objAJinAXL.Write_Output(3);
				g_objAJinAXL.Write_Output(13);
				gData.bUnloadPort2Wait = TRUE;
				gData.nPNoUnloadTray = 0;
			} else if (gData.bUnloadTrayLotEnd[0] && !m_bUnloadLotEnd) {	// 연속랏
				m_pDY03->oUnloadPort2SlideLock = FALSE; m_pDY03->oUnloadPort2SlideUnlock = TRUE;
				//m_pDY13->oDoor05Unlock = TRUE;
				//m_pDY13->oDoor06Unlock = TRUE;
				g_objAJinAXL.Write_Output(3);
				g_objAJinAXL.Write_Output(13);
				gData.bUnloadPort2Wait = TRUE;
				gData.nPNoUnloadTray = 0;
			}
			m_nUnloadStage1Case = 50; m_tUnloadStage1Loop.Set_LoopTime(5000);

			m_strLog.Format("UnloadStage1, %d", GetTickCount() - m_dwUnloadStage1);
			g_objLogFile.Save_TestLog(m_strLog);
		}
		break;

	case 50:	// Stage2 Check
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Z, 1) &&
			((!m_bUnloadLotEnd && m_nUnloadStage2Case >= 20) || (m_bUnloadLotEnd && m_nUnloadStage2Case > 20)))
		{
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);	// Lot 종료 시점이더라도 잔량있는 트레이 배출할 수 있게 해준다.
		}
		return TRUE;

	case 51:	// 안전 확인, Y Move to Load Port Position
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Z, 0) && !m_pDX05->iUnloadStage1Exist ) {
			if (g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Z, 0)) break;	// 인터락
			m_tUnloadStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_UNLOAD_STAGE1_Y, 0);
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 52:	// stage2 Check
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Y, 0)) {
			if (m_nUnloadStage2Case >= 20) {
				m_tUnloadStage1Loop.Takt_Save(14, 15);
				m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
			}
			return TRUE;
		}
		break;
	case 53:	// Z Move to Move Up
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Y, 0)) {
			m_tUnloadStage1Loop.Takt_Start();
			g_objCommon.Move_Position(AX_UNLOAD_STAGE1_Z, 1);
			m_nUnloadStage1Case++; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	case 54:	// Position Check
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Z, 1)) {
			m_tUnloadStage1Loop.Takt_Save(14, 16);
			m_nUnloadStage1Case = 1; m_tUnloadStage1Loop.Set_LoopTime(5000);
		}
		break;
	}

	// 15. (Error : 4500)
	if (m_tUnloadStage1Loop.Over_LoopTime()) {
		g_objCommon.Show_Error(4500 + m_nUnloadStage1Case);
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 16. (Error : 4600)
BOOL CSequenceMain::UnloadStage2_Run()
{
	static int threshold;

	if (gData.bUnloadPort1Wait && m_nUnloadStage2Case >  1 && m_nUnloadStage2Case < 10) return TRUE;
	if (gData.bUnloadPort2Wait && m_nUnloadStage2Case > 22 && m_nUnloadStage2Case < 50) return TRUE;
	
	if (gData.bUnloadPort1Wait && (m_nUnloadStage2Case == -1 || m_nUnloadStage2Case == -2 || m_nUnloadStage2Case == -3 )) return TRUE;
	

	switch (m_nUnloadStage2Case) {
	case 0:		// Start시 1로 바꿈
		return TRUE;

	case 1:		// 안전확인
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Y, 0) && g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Z, 1) &&
			g_objCommon.Get_UnloadStageMasterSlaveOut(2) && !m_pDX05->iUnloadStage2Exist  &&
			m_pDX03->iUnloadPort1SlideClose && m_pDX03->iUnlaodPort1LowCheck)
		{
			m_dwUnloadStage2 = GetTickCount();
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
		} else {
			if (!m_pDX03->iUnlaodPort1LowCheck) { g_dlgWork.PostMessage(UM_SHOW_MSG, 5, NULL); return FALSE;}
		}
		break;
	case 2:		// Ship Tray Check, Z Move to Support Up
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Y, 0) && g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Z, 1) &&
			g_objCommon.Get_UnloadStageMasterSlaveOut(2) && !m_pDX05->iUnloadStage2Exist  &&
			m_pDX03->iUnloadPort1SlideClose && m_pDX03->iUnlaodPort1LowCheck)
		{
			if (gData.nShipTrayLoad == 0 || gData.nShipTrayLoad >= gData.nShipTrayMax) {	// Ship Count 확인
				m_nUnloadStage2Case = 15; m_tUnloadStage2Loop.Set_LoopTime(5000);
			} else {
				m_tUnloadStage2Loop.Takt_Start();
				g_objCommon.Move_Position(AX_UNLOAD_STAGE2_Z, 2);
				m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
			}
		} else {
			if (!m_pDX03->iUnlaodPort1LowCheck) { g_dlgWork.PostMessage(UM_SHOW_MSG, 5, NULL); return FALSE;}
		}
		break;
	case 3:		// Stage Master In
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Z, 2)) {
			m_tUnloadStage2Loop.Takt_Save(15, 1);
			m_tUnloadStage2Loop.Takt_Start();
			m_pDY05->oUnloadStage2MasterIn = TRUE;
			g_objAJinAXL.Write_Output(5);
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 4:		// Stage Slave In
		if (m_pDX05->iUnloadStage2MasterIn && !m_pDX05->iUnloadStage2MasterOut) {
			m_pDY05->oUnloadStage2SlaveIn = TRUE;
			g_objAJinAXL.Write_Output(5);
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 5:		// Master/Slave Check
		if (g_objCommon.Get_UnloadStageMasterSlaveIn(2)) {
			m_tUnloadStage2Loop.Takt_Save(15, 6);	// Master/Slave In
			m_tUnloadStage2Loop.Takt_Start();
			g_objCommon.Set_UnloadPortSupportOut(1);
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 6:		// Z Move to Support Down
		if (g_objCommon.Get_UnloadPortSupportOut(1)) {
			m_tUnloadStage2Loop.Takt_Save(15, 2);	// Support Out
			m_tUnloadStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_UNLOAD_STAGE2_Z, 3);				
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 7:		// Support In
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Z, 3)) {
			if (!m_tUnloadStage2Loop.Waiting_Time(500)) break;
			m_tUnloadStage2Loop.Takt_Save(15, 3);	// Support Down
			m_tUnloadStage2Loop.Takt_Start();
			g_objCommon.Set_UnloadPortSupportIn(1);
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 8:		// Tray Check, Stage Master In
		if (g_objCommon.Get_UnloadPortSupportIn(1)) {
			if (!m_tUnloadStage2Loop.Waiting_Time(500)) break;
			m_tUnloadStage2Loop.Takt_Save(15, 4);	// Support In
			m_tUnloadStage2Loop.Takt_Start();
			// Port와 Tray가 아슬아슬하여 부딪힐때가 있어 MoveDown에서 이동하도록 한다.
			g_objCommon.Move_Position(AX_UNLOAD_STAGE2_Z, 0);	// Move Down
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 9:		// Z Move to Move Up
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Z, 0) && m_pDX05->iUnloadStage2Exist ) 
		{
			m_pDY05->oUnloadStage2MasterIn = FALSE;
			m_pDY05->oUnloadStage2SlaveIn = FALSE;
			g_objAJinAXL.Write_Output(5);
			m_tUnloadStage2Loop.Takt_Save(15, 14);
			m_nUnloadStage2Case = -3; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case -3:
		if(g_objCommon.Get_UnloadStageMasterSlaveOut(2))
		{
			m_pDY05->oUnloadStage2MasterIn = TRUE;			
			g_objAJinAXL.Write_Output(5);
			m_nUnloadStage2Case = -2; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case -2:
		if (m_pDX05->iUnloadStage2MasterIn && !m_pDX05->iUnloadStage2MasterOut)
		{
			m_pDY05->oUnloadStage2SlaveIn = TRUE;
			g_objAJinAXL.Write_Output(5);
			m_nUnloadStage2Case = -1; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case -1:
		if (g_objCommon.Get_UnloadStageMasterSlaveIn(2) ) 
		{ 				
			m_nUnloadStage2Case = 10; m_tUnloadStage2Loop.Set_LoopTime(5000); 
		}
		break;
	case 10:	// 안전 확인 
		if (m_nUnloadStage1Case > 22 ) 
		{ 				
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000); 
		}
		return TRUE;

	case 11:	// Y Move to Work Position
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Z, 0) && m_pDX05->iUnloadStage2Exist) {
			m_tUnloadStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_UNLOAD_STAGE2_Y, 1);	// Work Position
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 12:	// Position check
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Y, 1)) {
			m_tUnloadStage2Loop.Takt_Save(15, 7);
			m_tUnloadStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_UNLOAD_STAGE2_Z, 1);	// Move Up
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 13:
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Z, 1) && m_pDX05->iUnloadStage2Exist ) {
			m_tUnloadStage2Loop.Takt_Save(15, 5);
			Init_UnloadTray();
			gData.nShipTrayLoad++;
			threshold = 0;
			m_nUnloadStage2Case = 14; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 14:
		if(m_pDX05->iUnloadStage2Exist)
		{
			threshold++;
			if(threshold > 20) 
			{
				m_nUnloadStage2Case = 20; m_tUnloadStage2Loop.Set_LoopTime(5000);
			}
		}
		else
		{
			threshold = 0;
		}		
		break;
	case 15:	// Ship 정보 확인 및 MES 자재 등록.
		if (g_dlgWork.Check_ShipLotId()) {
			if (m_pEquipData->bUseMES && m_pEquipData->bUseMesShipReg) {	// MES Ship 자재등록 사용
				g_objMES.m_nMESShipSequence = 0;
				g_dlgWork.Get_ShipLotId();
				g_objMES.Set_ShipLotChangeRequest(gData.sShipLotID, gData.sOperID);	// 자재 등록 요청
			}
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(30000);
		} else {
			g_objCommon.Show_Alarm("Ship Tray 자재 정보를 입력해 주십시오.");
			return FALSE;
		}
		break;
	case 16:	// MES 결과 확인.
		if (g_objMES.m_nMESShipSequence == 2 || !m_pEquipData->bUseMES || !m_pEquipData->bUseMesShipReg) {
			g_dlgWork.Change_ShipLotId();	// 정보 이동.
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 17:
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Y, 0) && g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Z, 1) &&
			g_objCommon.Get_UnloadStageMasterSlaveOut(2) && !m_pDX05->iUnloadStage2Exist  &&
			m_pDX03->iUnloadPort1SlideClose && m_pDX03->iUnlaodPort1LowCheck)
		{
			m_tUnloadStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_UNLOAD_STAGE2_Z, 2);				
			m_nUnloadStage2Case = 3; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;

	case 20:	// Working
		return TRUE;

	case 21:	// 안전 확인
		if (m_nUnloadStage1Case <= 10 || m_nUnloadStage1Case > 52) {
			if (g_objAJinAXL.Is_Done(AX_UNLOAD_STAGE2_Y)) {
				m_tUnloadStage2Loop.Takt_Start();
				// Port와 Tray가 아슬아슬하여 부딪힐때가 있어 MoveDown에서 이동하도록 한다.
				g_objCommon.Move_Position(AX_UNLOAD_STAGE2_Z, 0);	// Move Down
				m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
			}
		}
		return TRUE;
	case 22:	// Y Move to Unload Port Position
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Z, 0)) {
			m_tUnloadStage2Loop.Takt_Save(15, 14);
			m_tUnloadStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_UNLOAD_STAGE2_Y, 2);
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 23:	// Z Move to Support Down
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Y, 2) && m_pDX05->iUnloadStage2Exist  && m_pDX03->iUnloadPort2SlideClose) {
			if ((gData.nPNoUnloadTray != gData.nPNoUnloadPort) && gData.nPNoUnloadPort != 0) return TRUE;

			m_tUnloadStage2Loop.Takt_Save(15, 8);
			m_tUnloadStage2Loop.Takt_Start();
			gData.nPNoUnloadPort = gData.nPNoUnloadTray;
			g_objCommon.Move_Position(AX_UNLOAD_STAGE2_Z, 5);	// Support Down
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 24:	// Support Out
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Z, 5)) {
			m_tUnloadStage2Loop.Takt_Save(15, 9);
			m_tUnloadStage2Loop.Takt_Start();
			g_objCommon.Set_UnloadPortSupportOut(2);
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 25:	// Z Move to Support Up
		if (g_objCommon.Get_UnloadPortSupportOut(2)) {
			m_tUnloadStage2Loop.Takt_Save(15, 10);
			m_tUnloadStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_UNLOAD_STAGE2_Z, 4);	// Support Up				
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 26:	// Support In
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Z, 4)) {
			m_tUnloadStage2Loop.Takt_Save(15, 11);
			m_tUnloadStage2Loop.Takt_Start();
			g_objCommon.Set_UnloadPortSupportIn(2);
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 27:	// Slave Out
		if (g_objCommon.Get_UnloadPortSupportIn(2) && g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Z, 4)) {
			m_tUnloadStage2Loop.Takt_Save(15, 12);
			m_tUnloadStage2Loop.Takt_Start();
			m_pDY05->oUnloadStage2SlaveIn = FALSE;
			g_objAJinAXL.Write_Output(5);
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 28:	// Master Out
		if (!m_pDX05->iUnloadStage2SlaveIn && m_pDX05->iUnloadStage2SlaveOut) {
			m_pDY05->oUnloadStage2MasterIn = FALSE;
			g_objAJinAXL.Write_Output(5);
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 29:	// Stage2 Check, Z Move to Move Down
		if (g_objCommon.Get_UnloadStageMasterSlaveOut(2)) {
			m_tUnloadStage2Loop.Takt_Save(15, 13);
			m_tUnloadStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_UNLOAD_STAGE2_Z, 0);	// Move Down				
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 30:	// Position Check
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Z, 0) && !m_pDX05->iUnloadStage2Exist) {
			m_tUnloadStage2Loop.Takt_Save(15, 14);
			if (gData.bUnloadTrayLotEnd[1] && m_bUnloadLotEnd) {	// 도어락 오픈 후처리 확인.
				m_pDY03->oUnloadPort2SlideLock = FALSE; m_pDY03->oUnloadPort2SlideUnlock = TRUE;
				//m_pDY13->oDoor05Unlock = TRUE;
				//m_pDY13->oDoor06Unlock = TRUE;
				g_objAJinAXL.Write_Output(3);
				g_objAJinAXL.Write_Output(13);
				gData.bUnloadPort2Wait = TRUE;
				gData.nPNoUnloadTray = 0;
			} else if (gData.bUnloadTrayLotEnd[1] && !m_bUnloadLotEnd) {	// 연속랏
				m_pDY03->oUnloadPort2SlideLock = FALSE; m_pDY03->oUnloadPort2SlideUnlock = TRUE;
				//m_pDY13->oDoor05Unlock = TRUE;
				//m_pDY13->oDoor06Unlock = TRUE;
				g_objAJinAXL.Write_Output(3);
				g_objAJinAXL.Write_Output(13);
				gData.bUnloadPort2Wait = TRUE;
				gData.nPNoUnloadTray = 0;
			}
			m_nUnloadStage2Case = 50; m_tUnloadStage2Loop.Set_LoopTime(5000);

			m_strLog.Format("UnloadStage2, %d", GetTickCount() - m_dwUnloadStage2);
			g_objLogFile.Save_TestLog(m_strLog);
		}
		break;

	case 50:	// Stage2 Check
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Z, 1) &&
			((!m_bUnloadLotEnd && m_nUnloadStage1Case >= 20) || (m_bUnloadLotEnd && m_nUnloadStage1Case > 20)))
		{
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);	// Lot 종료 시점이더라도 잔량있는 트레이 배출할 수 있게 해준다.
		}
		return TRUE;

	case 51:	// 안전 확인, Y Move to Load Port Position
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Z, 0) && !m_pDX05->iUnloadStage2Exist ) {
			if (g_objCommon.Check_Position(AX_UNLOAD_STAGE1_Z, 0)) break;	// 인터락
			m_tUnloadStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_UNLOAD_STAGE2_Y, 0);
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 52:	// stage2 Check
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Y, 0)) {
			if (m_nUnloadStage1Case >= 20) {
				m_tUnloadStage2Loop.Takt_Save(15, 15);
				m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
			}
			return TRUE;
		}
		break;
	case 53:	// Z Move to Move Up
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Y, 0)) {
			m_tUnloadStage2Loop.Takt_Start();
			g_objCommon.Move_Position(AX_UNLOAD_STAGE2_Z, 1);
			m_nUnloadStage2Case++; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	case 54:	// Position Check
		if (g_objCommon.Check_Position(AX_UNLOAD_STAGE2_Z, 1)) {
			m_tUnloadStage2Loop.Takt_Save(15, 16);
			m_nUnloadStage2Case = 1; m_tUnloadStage2Loop.Set_LoopTime(5000);
		}
		break;
	}

	// 16. (Error : 4600)
	if (m_tUnloadStage2Loop.Over_LoopTime()) {
		g_objCommon.Show_Error(4600 + m_nUnloadStage2Case);
		return FALSE;
	}
	return TRUE;
}


BOOL CSequenceMain::CheckInspectCmGoOrNot(int nPortNo)
{
	for(int i = 0;  i < 4; i++)
	{
		if(gData.nTNoIndex[0][i] == 1)  gData.sInspectCmLotIDPrevious = gData.sLotID[nPortNo-1];
	}
	
	/*if(gData.sInspectCmLotIDLater != gData.sInspectCmLotIDPrevious) 
	{
		gData.bInspectCmThisLotVSkip = FALSE;
	}*/

	gData.dwRunTimeNow = GetTickCount() - gLot.dwLotStart[nPortNo - 1];
	double dTimeLimit = m_pEquipData->nInspectCmMinutes*60*1000; //분단위로 변경 
		
	//조건이 안맞아서 비전 촬영을 하지 않는 중에도 시간이 지나면 초기화 
	if(gData.dwRunTimeNow + gData.dwRunTimeAccumulated > dTimeLimit 
		&& gData.nInspectCmLotCount >= m_pEquipData->nInspectCmLotTimes)
	{
		gData.nInspectCmLotCount = 0;	
		gData.bInspectCmThisLotVSkip = TRUE;
	}	

	if(gData.nInspectCmCheckTime >= 0) gData.nInspectCmCheckTime = (gData.dwRunTimeNow + gData.dwRunTimeAccumulated);
	
	//비전 촬영 중에 시간이 지나면 초기화, 찍던거는 마저 다 찍고 "-1"로 만든다 
	if(gData.nInspectCmCheckTime > dTimeLimit && gData.nInspectCmCheckTime >= 0 && gData.nInspectCmLotCount >= m_pEquipData->nInspectCmLotTimes )
	{
		gData.nInspectCmCheckTime = -1;

		gData.dwRunTimeNow = 0;
		gData.dwRunTimeAccumulated = 0;		
	}
		
	if (gData.nInspectCmLotCount < m_pEquipData->nInspectCmLotTimes 
		&& gData.nInsCmScanLineCnt < m_pEquipData->nInspectCmScanTimes)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CSequenceMain::Run_Simulation()
{
#ifdef AJIN_BOARD_USE
	return TRUE;
#endif
	 m_pDX02->iCapPort1LowCheck = TRUE;

	if(m_nTrayPickCase == 8)
	{
		Sleep(SIM_WAITTIMES);
		m_pDX00->iTrayPickerExist = TRUE;
		
	}

	if(m_nTrayPickCase == 24)
	{
		Sleep(SIM_WAITTIMES);
		m_pDX00->iLoadPort1LowCheck = TRUE;
		m_pDX00->iTrayPickerExist = FALSE;
	}
	
	if(m_nLoadStage1Case == 8)
	{
		Sleep(SIM_WAITTIMES);
		 m_pDX04->iLoadStage1Exist = TRUE;
		 m_pDX00->iLoadPort1LowCheck = FALSE;
	}

	if(m_nLoadStage2Case == 8)
	{
		Sleep(SIM_WAITTIMES);
		m_pDX04->iLoadStage2Exist = TRUE;
		m_pDX00->iLoadPort1LowCheck = FALSE;
	}

	if(m_nLoadStage1Case == 4)
	{
		Sleep(SIM_WAITTIMES);
		m_pDX04->iLoadStage1Exist = FALSE;
	}

	if(m_nLoadStage2Case == 4)
	{
		Sleep(SIM_WAITTIMES);
		m_pDX04->iLoadStage2Exist = FALSE;
	}

	if(m_nCapStage1Case == 12)
	{
		Sleep(SIM_WAITTIMES);
		m_pDX04->iCapStage1Exist =TRUE;
	}

	if(m_nCapStage2Case == 12)
	{
		Sleep(SIM_WAITTIMES);
		m_pDX04->iCapStage2Exist =TRUE;
	}

	if(m_nCapStage1Case == 51)
	{
		Sleep(SIM_WAITTIMES);
		m_pDX04->iCapStage1Exist =FALSE;
	}

	if(m_nCapStage2Case == 51)
	{
		Sleep(SIM_WAITTIMES);
		m_pDX04->iCapStage2Exist =FALSE;
	}

	if(m_nUnloadStage1Case == 1)
	{
		Sleep(SIM_WAITTIMES);
		m_pDX05->iUnloadStage1Exist = FALSE;
		m_pDX05->iUnloadStage1Exist2 = FALSE;
	}

	if(m_nUnloadStage2Case == 1)
	{
		Sleep(SIM_WAITTIMES);
		m_pDX05->iUnloadStage2Exist = FALSE;
		m_pDX05->iUnloadStage2Exist2 = FALSE;
	}

	if(m_nUnloadStage1Case == 9 || m_nUnloadStage2Case == 23)
	{
		Sleep(SIM_WAITTIMES);
		m_pDX05->iUnloadStage1Exist = TRUE;
		m_pDX05->iUnloadStage1Exist2 = TRUE;
	}


	if(m_nUnloadStage2Case == 9 || m_nUnloadStage2Case == 23)
	{
		Sleep(SIM_WAITTIMES);
		m_pDX05->iUnloadStage2Exist = TRUE;
		m_pDX05->iUnloadStage2Exist2 = TRUE;
	}

	if(m_nUnloadStage1Case == 30)
	{
		Sleep(SIM_WAITTIMES);
		m_pDX05->iUnloadStage1Exist = FALSE;
		m_pDX05->iUnloadStage1Exist2 = FALSE;
	}


	if(m_nUnloadStage2Case == 30)
	{
		Sleep(SIM_WAITTIMES);
		m_pDX05->iUnloadStage2Exist = FALSE;
		m_pDX05->iUnloadStage2Exist2 = FALSE;
	}

	if (m_nLoadStage1Case == 2) { Sleep(SIM_WAITTIMES); m_pDX00->iLoadPort1LowCheck = TRUE; m_pDX01->iLoadPort2LowCheck = TRUE; }
	if (m_nLoadStage2Case == 2) { Sleep(SIM_WAITTIMES); m_pDX00->iLoadPort1LowCheck = TRUE; m_pDX01->iLoadPort2LowCheck = TRUE; }

	if (m_nCapStage1Case == 1) { Sleep(SIM_WAITTIMES); m_pDX02->iCapPort1LowCheck = TRUE; }
	if (m_nCapStage2Case == 1) { Sleep(SIM_WAITTIMES); m_pDX02->iCapPort1LowCheck = TRUE; }

	

	if (m_nUnloadStage1Case == 1) { Sleep(SIM_WAITTIMES); m_pDX03->iUnlaodPort1LowCheck = TRUE; }
	if (m_nUnloadStage2Case == 1) { Sleep(SIM_WAITTIMES); m_pDX03->iUnlaodPort1LowCheck = TRUE; }



	return TRUE;
}
