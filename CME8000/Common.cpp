// Common.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "CME8000.h"
#include "Common.h"

#include "LogFile.h"
#include "AJinAXL.h"
#include "DataManager.h"

#include "ErrorDlg.h"
#include "AlarmDlg.h"
#include "MsgBoxDlg.h"
#include "NumPadDlg.h"
#include "KeyPadDlg.h"
#include "PasswordDlg.h"
#include "WorkDlg.h"

#include "SequenceMain.h"

#include <iostream>
#include <string>
#include <fstream>
#include "Wininet.h"

#include <wbemidl.h>
#include <comdef.h>

#pragma comment(lib, "wbemuuid.lib")

// CCommon
CCommon g_objCommon;

IMPLEMENT_DYNAMIC(CCommon, CWnd)

CCriticalSection g_csShowError;
CCriticalSection g_csShowAlarm;
CCriticalSection g_csIndexVacuum;

CCommon::CCommon()
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	m_nFreq = freq.QuadPart;
}

CCommon::~CCommon()
{
}

BEGIN_MESSAGE_MAP(CCommon, CWnd)
END_MESSAGE_MAP()

// CCommon 메시지 처리기입니다.

void CCommon::Save_MotionPos()
{
	int nCount = 0;
	for(int i=0; i<AXIS_COUNT; i++) {
		if (i == 3 || i == 11) continue;	// No Axis, Index R Axis
		if(g_objAJinAXL.Is_Done(i)) { gAlm.dMotionPos[i] = g_objAJinAXL.Get_Position(i); }
		else						{ gAlm.dMotionPos[i] = -100.0; nCount++; }
	}
	if (nCount == 0) return;

	theApp.uSleep(1000);
	nCount = 0;
	for(int i=0; i<AXIS_COUNT; i++) {
		if (i == 3 || i == 11) continue;	// No Axis, Index R Axis
		if (gAlm.dMotionPos[i] < -10.0) {
			if(g_objAJinAXL.Is_Done(i)) gAlm.dMotionPos[i] = g_objAJinAXL.Get_Position(i);
			else nCount++;
		}
	}
	if (nCount == 0) return;

	theApp.uSleep(3000);
	for(int i=0; i<AXIS_COUNT; i++) {
		if (i == 3 || i == 11) continue;	// No Axis, Index R Axis
		if (gAlm.dMotionPos[i] < -10.0) {
			if(g_objAJinAXL.Is_Done(i)) gAlm.dMotionPos[i] = g_objAJinAXL.Get_Position(i);
		}
	}
}

int CCommon::Check_MotionPos()
{
	int		nMotionNo = 99;
	double	dCurrentPos, dCheckPos;

	if (gAlm.dMotionChkPos < 0.05) return nMotionNo;

	for(int i=0; i<AXIS_COUNT; i++) {
		if (i == 3 || i == 11) continue;	// No Axis, Index R Axis
		if (gAlm.dMotionPos[i] < -10.0) continue;
		if (!g_objAJinAXL.Is_Done(i)) continue;

		dCheckPos   = gAlm.dMotionPos[i];
		dCurrentPos = g_objAJinAXL.Get_Position(i);
		if (fabs(dCurrentPos - dCheckPos) > gAlm.dMotionChkPos) return i;
	}
	return nMotionNo;
}

BOOL CCommon::Check_Position(int nAxis, int nMoveIdx, double dRange)
{
	double dCurrentPos = g_objAJinAXL.Get_Position(nAxis);
	MOVE_DATA *pMoveData = g_objDataManager.Get_pMoveData();

	double dCheckPos = -1.0;

	if (g_objAJinAXL.Is_Done(nAxis)) {
		if (nAxis == AX_TRAY_PICKER_X)		dCheckPos = pMoveData->dTrayPickerX[nMoveIdx];		// AXIS 00
		if (nAxis == AX_TRAY_PICKER_Z)		dCheckPos = pMoveData->dTrayPickerZ[nMoveIdx];		// AXIS 01
		if (nAxis == AX_TRAY_PICKER_R)		dCheckPos = pMoveData->dTrayPickerR[nMoveIdx];		// AXIS 02
		if (nAxis == AX_LOAD_STAGE1_X)		dCheckPos = pMoveData->dLoadStage1X[nMoveIdx];		// AXIS 04
		if (nAxis == AX_LOAD_STAGE1_Z)		dCheckPos = pMoveData->dLoadStage1Z[nMoveIdx];		// AXIS 05
		if (nAxis == AX_LOAD_STAGE2_X)		dCheckPos = pMoveData->dLoadStage2X[nMoveIdx];		// AXIS 06
		if (nAxis == AX_LOAD_STAGE2_Z)		dCheckPos = pMoveData->dLoadStage2Z[nMoveIdx];		// AXIS 07
		if (nAxis == AX_LOAD_PICKER_Y)		dCheckPos = pMoveData->dLoadPickerY[nMoveIdx];		// AXIS 08
		if (nAxis == AX_LOAD_PICKER_Z)		dCheckPos = pMoveData->dLoadPickerZ[nMoveIdx];		// AXIS 09
		if (nAxis == AX_LOAD_PICKER_P)		dCheckPos = pMoveData->dLoadPickerP[nMoveIdx];		// AXIS 00
		if (nAxis == AX_MAIN_INDEX_R)		dCheckPos = pMoveData->dMainIndexR[nMoveIdx];		// AXIS 11
		if (nAxis == AX_VISION_CM_X)		dCheckPos = pMoveData->dVisionCmX[nMoveIdx];		// AXIS 12
		if (nAxis == AX_CAP_STAGE1_X)		dCheckPos = pMoveData->dCapStage1X[nMoveIdx];		// AXIS 13
		if (nAxis == AX_CAP_STAGE1_Z)		dCheckPos = pMoveData->dCapStage1Z[nMoveIdx];		// AXIS 14
		if (nAxis == AX_CAP_STAGE2_X)		dCheckPos = pMoveData->dCapStage2X[nMoveIdx];		// AXIS 15
		if (nAxis == AX_CAP_STAGE2_Z)		dCheckPos = pMoveData->dCapStage2Z[nMoveIdx];		// AXIS 16
		if (nAxis == AX_CAP_PICKER_Y)		dCheckPos = pMoveData->dCapPickerY[nMoveIdx];		// AXIS 17
		if (nAxis == AX_CAP_PICKER_Z)		dCheckPos = pMoveData->dCapPickerZ[nMoveIdx];		// AXIS 18
		if (nAxis == AX_CAP_PICKER_P)		dCheckPos = pMoveData->dCapPickerP[nMoveIdx];		// AXIS 19
		if (nAxis == AX_CAP_BUFFER_Y)		dCheckPos = pMoveData->dCapBufferY[nMoveIdx];		// AXIS 10
		if (nAxis == AX_VISION_CAP_Y)		dCheckPos = pMoveData->dVisionCapY[nMoveIdx];		// AXIS 21
		if (nAxis == AX_ASSY_PICKER_X)		dCheckPos = pMoveData->dAssyPickerX[nMoveIdx];		// AXIS 22
		if (nAxis == AX_ASSY_PICKER_Y)		dCheckPos = pMoveData->dAssyPickerY[nMoveIdx];		// AXIS 23
		if (nAxis == AX_ASSY_PICKER_Z)		dCheckPos = pMoveData->dAssyPickerZ[nMoveIdx];		// AXIS 24
		if (nAxis == AX_TRANS_STAGE_X)		dCheckPos = pMoveData->dTransStageX[nMoveIdx];		// AXIS 25
		if (nAxis == AX_TRANS_STAGE_Z)		dCheckPos = pMoveData->dTransStageZ[nMoveIdx];		// AXIS 26
		if (nAxis == AX_TRANS_STAGE_T)		dCheckPos = pMoveData->dTransStageT[nMoveIdx];		// AXIS 27
		if (nAxis == AX_UNLOAD_PICKER_X)	dCheckPos = pMoveData->dUnloadPickerX[nMoveIdx];	// AXIS 28
		if (nAxis == AX_UNLOAD_PICKER_Z)	dCheckPos = pMoveData->dUnloadPickerZ[nMoveIdx];	// AXIS 29
		if (nAxis == AX_UNLOAD_PICKER_P)	dCheckPos = pMoveData->dUnloadPickerP[nMoveIdx];	// AXIS 20
		if (nAxis == AX_UNLOAD_STAGE1_Y)	dCheckPos = pMoveData->dUnloadStage1Y[nMoveIdx];	// AXIS 31
		if (nAxis == AX_UNLOAD_STAGE1_Z)	dCheckPos = pMoveData->dUnloadStage1Z[nMoveIdx];	// AXIS 32
		if (nAxis == AX_UNLOAD_STAGE2_Y)	dCheckPos = pMoveData->dUnloadStage2Y[nMoveIdx];	// AXIS 33
		if (nAxis == AX_UNLOAD_STAGE2_Z)	dCheckPos = pMoveData->dUnloadStage2Z[nMoveIdx];	// AXIS 34

		if (fabs(dCurrentPos - dCheckPos) < dRange) return TRUE;

	} else return FALSE;

	return FALSE;
}

void CCommon::Move_Position(int nAxis, int nMoveIdx)
{
	MOVE_DATA *pMoveData = g_objDataManager.Get_pMoveData();

	double dMovePos = -999.0;

	if (nAxis == AX_TRAY_PICKER_X)			dMovePos = pMoveData->dTrayPickerX[nMoveIdx];		// AXIS 00
	if (nAxis == AX_TRAY_PICKER_Z)			dMovePos = pMoveData->dTrayPickerZ[nMoveIdx];		// AXIS 01
	if (nAxis == AX_TRAY_PICKER_R)			dMovePos = pMoveData->dTrayPickerR[nMoveIdx];		// AXIS 02
	if (nAxis == AX_LOAD_STAGE1_X)			dMovePos = pMoveData->dLoadStage1X[nMoveIdx];		// AXIS 04
	if (nAxis == AX_LOAD_STAGE1_Z)			dMovePos = pMoveData->dLoadStage1Z[nMoveIdx];		// AXIS 05
	if (nAxis == AX_LOAD_STAGE2_X)			dMovePos = pMoveData->dLoadStage2X[nMoveIdx];		// AXIS 06
	if (nAxis == AX_LOAD_STAGE2_Z)			dMovePos = pMoveData->dLoadStage2Z[nMoveIdx];		// AXIS 07
	if (nAxis == AX_LOAD_PICKER_Y)			dMovePos = pMoveData->dLoadPickerY[nMoveIdx];		// AXIS 08
	if (nAxis == AX_LOAD_PICKER_Z)			dMovePos = pMoveData->dLoadPickerZ[nMoveIdx];		// AXIS 09
	if (nAxis == AX_LOAD_PICKER_P)			dMovePos = pMoveData->dLoadPickerP[nMoveIdx];		// AXIS 00
	if (nAxis == AX_MAIN_INDEX_R)			dMovePos = pMoveData->dMainIndexR[nMoveIdx];		// AXIS 11
	if (nAxis == AX_VISION_CM_X)			dMovePos = pMoveData->dVisionCmX[nMoveIdx];			// AXIS 12
	if (nAxis == AX_CAP_STAGE1_X)			dMovePos = pMoveData->dCapStage1X[nMoveIdx];		// AXIS 13
	if (nAxis == AX_CAP_STAGE1_Z)			dMovePos = pMoveData->dCapStage1Z[nMoveIdx];		// AXIS 14
	if (nAxis == AX_CAP_STAGE2_X)			dMovePos = pMoveData->dCapStage2X[nMoveIdx];		// AXIS 15
	if (nAxis == AX_CAP_STAGE2_Z)			dMovePos = pMoveData->dCapStage2Z[nMoveIdx];		// AXIS 16
	if (nAxis == AX_CAP_PICKER_Y)			dMovePos = pMoveData->dCapPickerY[nMoveIdx];		// AXIS 17
	if (nAxis == AX_CAP_PICKER_Z)			dMovePos = pMoveData->dCapPickerZ[nMoveIdx];		// AXIS 18
	if (nAxis == AX_CAP_PICKER_P)			dMovePos = pMoveData->dCapPickerP[nMoveIdx];		// AXIS 19
	if (nAxis == AX_CAP_BUFFER_Y)			dMovePos = pMoveData->dCapBufferY[nMoveIdx];		// AXIS 10
	if (nAxis == AX_VISION_CAP_Y)			dMovePos = pMoveData->dVisionCapY[nMoveIdx];		// AXIS 21
	if (nAxis == AX_ASSY_PICKER_X)			dMovePos = pMoveData->dAssyPickerX[nMoveIdx];		// AXIS 22
	if (nAxis == AX_ASSY_PICKER_Y)			dMovePos = pMoveData->dAssyPickerY[nMoveIdx];		// AXIS 23
	if (nAxis == AX_ASSY_PICKER_Z)			dMovePos = pMoveData->dAssyPickerZ[nMoveIdx];		// AXIS 24
	if (nAxis == AX_TRANS_STAGE_X)			dMovePos = pMoveData->dTransStageX[nMoveIdx];		// AXIS 25
	if (nAxis == AX_TRANS_STAGE_Z)			dMovePos = pMoveData->dTransStageZ[nMoveIdx];		// AXIS 26
	if (nAxis == AX_TRANS_STAGE_T)			dMovePos = pMoveData->dTransStageT[nMoveIdx];		// AXIS 27
	if (nAxis == AX_UNLOAD_PICKER_X)		dMovePos = pMoveData->dUnloadPickerX[nMoveIdx];		// AXIS 28
	if (nAxis == AX_UNLOAD_PICKER_Z)		dMovePos = pMoveData->dUnloadPickerZ[nMoveIdx];		// AXIS 29
	if (nAxis == AX_UNLOAD_PICKER_P)		dMovePos = pMoveData->dUnloadPickerP[nMoveIdx];		// AXIS 20
	if (nAxis == AX_UNLOAD_STAGE1_Y)		dMovePos = pMoveData->dUnloadStage1Y[nMoveIdx];		// AXIS 31
	if (nAxis == AX_UNLOAD_STAGE1_Z)		dMovePos = pMoveData->dUnloadStage1Z[nMoveIdx];		// AXIS 32
	if (nAxis == AX_UNLOAD_STAGE2_Y)		dMovePos = pMoveData->dUnloadStage2Y[nMoveIdx];		// AXIS 33
	if (nAxis == AX_UNLOAD_STAGE2_Z)		dMovePos = pMoveData->dUnloadStage2Z[nMoveIdx];		// AXIS 34

	if (dMovePos == -999) return;
	g_objAJinAXL.Move_Absolute(nAxis, dMovePos);
}

void CCommon::Show_Error(int nErrNo)
{
	g_csShowError.Lock();
	if (!g_dlgError.IsWindowVisible() && !g_dlgAlarm.IsWindowVisible()) {
		g_dlgError.Set_ErrorNumber(nErrNo);
		g_dlgError.ShowWindow(SW_SHOW);
	}
	g_csShowError.Unlock();
}

void CCommon::Set_ErrorSubMessage(CString sErrSubMsg)
{
	g_dlgError.Set_ErrorSubMessage(sErrSubMsg);
}

void CCommon::Show_Alarm(CString sMsg, int nState, BOOL bOn)
{
	g_csShowAlarm.Lock();
	if (bOn) {
		if (!g_dlgError.IsWindowVisible() && !g_dlgAlarm.IsWindowVisible()) {
			g_dlgWork.MachineStopLog("ALARM", sMsg);
			g_dlgAlarm.Set_AlarmMessage(sMsg);
			g_dlgAlarm.Set_AlarmState(nState);
			g_dlgAlarm.ShowWindow(SW_SHOW);
		}
	} else {
		if (!g_dlgError.IsWindowVisible() && g_dlgAlarm.IsWindowVisible()) {
			g_dlgAlarm.ShowWindow(SW_HIDE);
		}
	}
	g_csShowAlarm.Unlock();
}

int CCommon::Show_MsgBox(int nBtnCnt, CString sMsg)
{
	CMsgBoxDlg dlgMsgBox;
	dlgMsgBox.Set_Message(nBtnCnt, sMsg);
	return dlgMsgBox.DoModal();
}

int CCommon::Show_NumPad(CString sOld, CString &sNew, int nAxis)
{
	CNumPadDlg dlgNumPad;
	dlgNumPad.Set_OldValue(sOld, nAxis);
	if (dlgNumPad.DoModal() != IDOK) return IDCANCEL;

	sNew = g_strReturnNumPad;
	return IDOK;
}

int CCommon::Show_KeyPad(CString &sKey)
{
	CKeyPadDlg dlgKeyPad;
	if (dlgKeyPad.DoModal() != IDOK) return IDCANCEL;

	sKey = g_strReturnKeyPad;
	return IDOK;
}

int CCommon::Show_Password(int &nUser)
{
	CPasswordDlg dlgPassword;
	if (dlgPassword.DoModal() != IDOK) return IDCANCEL;

	nUser = g_nLoginUser;
	return IDOK;
}

BOOL CCommon::Check_MainEmgAir()
{
#ifdef AJIN_BOARD_USE
	DX_DATA_12 *pDX12 = g_objAJinAXL.Get_pDX12();
	if (pDX12->iEmgSw1)		{ Show_Error(1); return FALSE; }
	if (pDX12->iEmgSw2)		{ Show_Error(2); return FALSE; }
	if (pDX12->iEmgSw3)		{ Show_Error(3); return FALSE; }
	if (!pDX12->iMainAir1)	{ Show_Error(7); return FALSE; }
	if (!pDX12->iMainAir2)	{ Show_Error(8); return FALSE; }
#endif
	return TRUE;
}

BOOL CCommon::Check_MainDoor(BOOL bAuto)
{
#ifdef DRY_RUN_TEST
	return TRUE;
#endif
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

#ifdef AJIN_BOARD_USE
	DX_DATA_13 *pDX13 = g_objAJinAXL.Get_pDX13();
	BOOL bLoad1 = g_objSequenceMain.Get_LotLoadEnable(0);
	BOOL bLoad2 = g_objSequenceMain.Get_LotLoadEnable(1);

// 	if ((!bAuto || !bLoad) && pDX13->iDoor04Unlock) { Show_Alarm("Main 04번 Door(X1019) Unlocked."); return FALSE; }

	if (pEquipData->bUseInlineMode && !pEquipData->bUseDoorLock) {
		// Inline 모드는 도어락 사용 안해도 무조건 Load Port쪽 확인한다.
		if (pDX13->iDoor01Open) { Show_Alarm("Main 01번 Door(X1300) Opened."); return FALSE; }
		if (pDX13->iDoor02Open) { Show_Alarm("Main 02번 Door(X1301) Opened."); return FALSE; }

	} else {
		if (!pEquipData->bUseDoorLock) return TRUE;

		// temp
		if (pDX13->iDoor01Open && !bLoad1)					{ Show_Alarm("Main 01번 Door(X1300) Opened."); return FALSE; }
		if (pDX13->iDoor02Open && !bLoad2)					{ Show_Alarm("Main 02번 Door(X1301) Opened."); return FALSE; }
		if (pDX13->iDoor03Open)								{ Show_Alarm("Main 03번 Door(X1302) Opened."); return FALSE; }
		if (pDX13->iDoor04Open && !gData.bLoadPort3Wait)	{ Show_Alarm("Main 04번 Door(X1303) Opened."); return FALSE; }
		if (pDX13->iDoor05Open && !gData.bUnloadPort2Wait)	{ Show_Alarm("Main 05번 Door(X1304) Opened."); return FALSE; }
		if (pDX13->iDoor06Open && !gData.bUnloadPort2Wait)	{ Show_Alarm("Main 06번 Door(X1305) Opened."); return FALSE; }
		if (pDX13->iDoor07Open)								{ Show_Alarm("Main 07번 Door(X1306) Opened."); return FALSE; }
		if (pDX13->iDoor08Open)								{ Show_Alarm("Main 08번 Door(X1307) Opened."); return FALSE; }
		if (pDX13->iDoor09Open)								{ Show_Alarm("Main 09번 Door(X1308) Opened."); return FALSE; }
		if (pDX13->iDoor10Open && !gData.bUnloadPort1Wait)	{ Show_Alarm("Main 10번 Door(X1309) Opened."); return FALSE; }
		if (pDX13->iDoor11Open && !gData.bCapPort1Wait)		{ Show_Alarm("Main 11번 Door(X1310) Opened."); return FALSE; }
		if (pDX13->iDoor12Open && !gData.bCapPort2Wait)		{ Show_Alarm("Main 12번 Door(X1311) Opened."); return FALSE; }
		if (pDX13->iDoor13Open)								{ Show_Alarm("Main 13번 Door(X1312) Opened."); return FALSE; }
	}
#endif
	return TRUE;
}

BOOL CCommon::Check_PortArea(BOOL bAuto)
{
	// 슬라이드를 사용하지 않고 손을 장비안으로 넣어 트레이 적재 및 배출을 하기 때문에 사용하지 않는다.
	return TRUE;
#ifdef DRY_RUN_TEST
	return TRUE;
#endif
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();
	if (!bAuto)	return TRUE;

#ifdef AJIN_BOARD_USE
	DX_DATA_00 *pDX00 = g_objAJinAXL.Get_pDX00();
	DX_DATA_01 *pDX01 = g_objAJinAXL.Get_pDX01();
	DX_DATA_02 *pDX02 = g_objAJinAXL.Get_pDX02();
	DX_DATA_03 *pDX03 = g_objAJinAXL.Get_pDX03();
	BOOL bLoad1 = g_objSequenceMain.Get_LotLoadEnable(0);
	BOOL bLoad2 = g_objSequenceMain.Get_LotLoadEnable(1);

	if (pEquipData->bUseInlineMode && !pEquipData->bUseDoorLock) {
		// Inline 모드는 도어락 사용 안해도 무조건 Load Port쪽 확인한다.
		if (!pDX01->iLoadPortAreaCheck) { Show_Alarm("Load Port Area(X0115) 감지 되었습니다."); return FALSE; }

	} else {
		if (!pEquipData->bUseDoorLock) return TRUE;

		if (!pDX01->iLoadPortAreaCheck && (pDX00->iLoadPort1SlideOpen || pDX00->iLoadPort1SlideClose)) {
			Show_Alarm("Load Port Area(X0115) 감지 되었습니다."); return FALSE;
		}

		if (!pDX01->iLoadPortAreaCheck && (pDX01->iLoadPort2SlideOpen || pDX01->iLoadPort2SlideClose)) {
			Show_Alarm("Load Port Area(X0115) 감지 되었습니다."); return FALSE;
		}

		if (!pDX01->iLoadPort3AreaCheck && !gData.bLoadPort3Wait) {
			Show_Alarm("Load Port Area(X0131) 감지 되었습니다."); return FALSE;
		}

		if (!pDX02->iCapPortAreaCheck	&& (pDX02->iCapPort1SlideOpen || pDX02->iCapPort1SlideClose)) {
			Show_Alarm("Cap Port Area(X0231) 감지 되었습니다."); return FALSE;
		}

		if (!pDX02->iCapPortAreaCheck	&& (pDX02->iCapPort2SlideOpen || pDX02->iCapPort2SlideClose)) {
			Show_Alarm("Cap Port Area(X0231) 감지 되었습니다."); return FALSE;
		}

		if (!pDX03->iUnloadPort1AreaCheck && (pDX03->iUnloadPort1SlideOpen || pDX03->iUnloadPort1SlideClose)) {
			Show_Alarm("Unload Port1 Area(X0315) 감지 되었습니다."); return FALSE;
		}

		if (!pDX03->iUnloadPort2AreaCheck && (pDX03->iUnloadPort2SlideOpen || pDX03->iUnloadPort2SlideClose)) {
			Show_Alarm("Unload Port2 Area(X0331) 감지 되었습니다."); return FALSE;
		}
	}
#endif
	return TRUE;
}

BOOL CCommon::Check_TrayFull()
{
#ifdef AJIN_BOARD_USE
	DX_DATA_01 *pDX01 = g_objAJinAXL.Get_pDX01();
	DX_DATA_02 *pDX02 = g_objAJinAXL.Get_pDX02();
	DX_DATA_03 *pDX03 = g_objAJinAXL.Get_pDX03();

	if (pDX01->iLoadPort3HighCheck)		
	{ 
		Show_MsgBox("Load Port3 Full Sensor Checked.");
		//Show_Alarm("Load Port3 Full Sensor Checked.");
		return FALSE; 
	}
	if (pDX02->iCapPort1HighCheck)		
	{ 
		Show_MsgBox("Cap Port1 Full Sensor Checked.");
		//Show_Alarm("Cap Port1 Full Sensor Checked.", STATE_CAPTRAY);
		return FALSE;
	}
	if (pDX02->iCapPort2HighCheck)		
	{ 
		Show_MsgBox("Cap Port2 Full Sensor Checked.");
		//Show_Alarm("Cap Port1 Full Sensor Checked.", STATE_CAPTRAY);
		return FALSE;
	}
	if (pDX03->iUnloadPort1HighCheck)	
	{ 
		Show_MsgBox("Unload Port1 Full Sensor Checked.");
		//Show_Alarm("Unload Port2 Full Sensor Checked.", STATE_SHIPTRAY); 
		return FALSE;
	}
	if (pDX03->iUnloadPort2HighCheck)	
	{ 
		Show_MsgBox("Unload Port2 Full Sensor Checked.");
		//Show_Alarm("Unload Port2 Full Sensor Checked.", STATE_SHIPTRAY); 
		return FALSE;
	}
#endif
	return TRUE;
}

BOOL CCommon::Check_UnloadTrayAlways1()
{
	DX_DATA_05 *pDX05 = g_objAJinAXL.Get_pDX05();

	if(!pDX05->iUnloadStage1Exist) return FALSE;
	else  return TRUE;
}

BOOL CCommon::Check_UnloadTrayAlways2()
{
	DX_DATA_05 *pDX05 = g_objAJinAXL.Get_pDX05();

	if(!pDX05->iUnloadStage1Exist) return FALSE;
	else  return TRUE;
}


// Servo On Error : Error (100 - 145)
BOOL CCommon::Check_ServoOn()
{
#ifdef AJIN_BOARD_USE
	for (int i = 0; i < AXIS_COUNT; i++) {
		if (i == 3) continue;
		if (!g_objAJinAXL.Get_ServoOn(i)) { Show_Error(i + 100); return FALSE; }
	}
#endif
	return TRUE;
}

// Drive Alarm : Error(200 - 245)
BOOL CCommon::Check_DirveAlarm()
{
#ifdef AJIN_BOARD_USE
	for (int i = 0; i < AXIS_COUNT; i++) {
		if (i == 3) continue;
		if (g_objAJinAXL.Get_AmpAlarm(i)) { Show_Error(i + 200); return FALSE; }
	}
#endif
	return TRUE;
}

// Limit Check : Error ELN(300 - 345), ELP(400 - 445)
BOOL CCommon::Check_EndLimit()
{
#ifdef AJIN_BOARD_USE
	for (int i = 0; i < AXIS_COUNT; i++) {
		if (g_objAJinAXL.Use_OrgAxis(i)) continue;
		if (g_objAJinAXL.Get_EndLimitPos(i)) { Show_Error(i + 300); return FALSE; }
		if (g_objAJinAXL.Get_EndLimitNeg(i)) { Show_Error(i + 400); return FALSE; }
	}
#endif
	return TRUE;
}

// Home Check : Error (500 - 545)
BOOL CCommon::Check_HomeDone()
{
#ifdef AJIN_BOARD_USE
	for (int i = 0; i < AXIS_COUNT; i++) {
		if (i == AX_NO_AXIS) continue;
 		if (i == AX_MAIN_INDEX_R && g_objSequenceMain.Get_MainRunCase(AUTO_MAIN_INDEX) > 20) continue;
		if (!g_objAJinAXL.Get_HomeDone(i)) { Show_Error(i + 500); return FALSE; }
	}
#endif
	return TRUE;
}

void CCommon::Locking_MainDoor(BOOL bLock, BOOL bAuto)
{
	DY_DATA_13 *pDY13 = g_objAJinAXL.Get_pDY13();

	// 0000 0000 0000 0000 0001 1111 1111 1111
	// 0    0    0    0    1    F    F    F
	if (bLock) {
		BOOL bLoad1 = g_objSequenceMain.Get_LotLoadEnable(0);
		BOOL bLoad2 = g_objSequenceMain.Get_LotLoadEnable(1);
		pDY13->nValue &= ~0x1FFF;	// All Lock
		if (bAuto && bLoad1)				pDY13->nValue |= 0x1;	// Except 1
		if (bAuto && bLoad2)				pDY13->nValue |= 0x2;	// Except 2
		if (bAuto && gData.bLoadPort3Wait)	pDY13->nValue |= 0x8;	// Except 4
		if (bAuto && gData.bCapPort1Wait)	pDY13->nValue |= 0x400;	// Except 11
		if (bAuto && gData.bCapPort2Wait)	pDY13->nValue |= 0x800;	// Except 12
		if (bAuto && gData.bUnloadPort1Wait)	pDY13->nValue |= 0x200;	// Except 10
		if (bAuto && gData.bUnloadPort2Wait)	pDY13->nValue |= 0x30;	// Except 5, 6
	} else {
		DWORD dwStart = GetTickCount();
		while (TRUE) {
			BOOL bMove = FALSE;
			for (int i = 0; i < AXIS_COUNT; i++) {
				if (i == 2 || i == 3) continue;
				if (!g_objAJinAXL.Is_Done(i)) { bMove = TRUE; break; }
			}
			if (!bMove) break;
			if (GetTickCount() - dwStart > 5000) break;	// 5초
			theApp.DoEvents();
		}
		pDY13->nValue |= 0x1FFF;
	}
	g_objAJinAXL.Write_Output(13);
}

void CCommon::Locking_PortSlide(BOOL bLock, int nPart)
{
	if (nPart == 0 || nPart == 1) {
		DY_DATA_00 *pDY00 = g_objAJinAXL.Get_pDY00();
		pDY00->oLoadPort1SlideLock = bLock; pDY00->oLoadPort1SlideUnlock = !bLock;
		g_objAJinAXL.Write_Output(0);
	}
	if (nPart == 0 || nPart == 2) {
		DY_DATA_01 *pDY01 = g_objAJinAXL.Get_pDY01();
		pDY01->oLoadPort2SlideLock = bLock; pDY01->oLoadPort2SlideUnlock = !bLock;
		g_objAJinAXL.Write_Output(1);
	}
	if (nPart == 0 || nPart == 3) {
		DY_DATA_01 *pDY01 = g_objAJinAXL.Get_pDY01();
		pDY01->oLoadPort3SlideLock = bLock; pDY01->oLoadPort3SlideUnlock = !bLock;
		g_objAJinAXL.Write_Output(1);
	}
	if (nPart == 0 || nPart == 4) {
		DY_DATA_02 *pDY02 = g_objAJinAXL.Get_pDY02();
		pDY02->oCapPort1SlideLock = bLock; pDY02->oCapPort1SlideUnlock = !bLock;
		g_objAJinAXL.Write_Output(2);
	}
	if (nPart == 0 || nPart == 5) {
		DY_DATA_02 *pDY02 = g_objAJinAXL.Get_pDY02();
		pDY02->oCapPort2SlideLock = bLock; pDY02->oCapPort2SlideUnlock = !bLock;
		g_objAJinAXL.Write_Output(2);
	}
	if (nPart == 0 || nPart == 6) {
		DY_DATA_03 *pDY03 = g_objAJinAXL.Get_pDY03();
		pDY03->oUnloadPort1SlideLock = bLock; pDY03->oUnloadPort1SlideUnlock = !bLock;
		g_objAJinAXL.Write_Output(3);
	}
	if (nPart == 0 || nPart == 7) {
		DY_DATA_03 *pDY03 = g_objAJinAXL.Get_pDY03();
		pDY03->oUnloadPort2SlideLock = bLock; pDY03->oUnloadPort2SlideUnlock = !bLock;
		g_objAJinAXL.Write_Output(3);
	}
}

/////////////////////////////////////////////////////////////////////////////

int CCommon::Get_Random(int nStart, int nEnd)
{
	static BOOL bSeed = FALSE;
	if (nStart >= nEnd) return 0;
	if (!bSeed) { srand((unsigned)time(NULL)); bSeed = TRUE; }
	return (rand() % (nEnd - nStart + 1) + nStart);
}

void CCommon::Backup_File(CString strPath, CString strName)
{
	g_objLogFile.Create_Folder(strPath + "\\Backup");

	CString strOriginFile, strTargetFile, strDateTime;

	SYSTEMTIME time;
	GetLocalTime(&time);
	strDateTime.Format("%04d%02d%02d_%02d%02d%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

	strOriginFile.Format("%s\\%s.ini", strPath, strName);
	strTargetFile.Format("%s\\Backup\\%s_%s.ini", strPath, strName, strDateTime);

	if (!CopyFile(strOriginFile, strTargetFile, FALSE)) return;
}

///////////////////////////////////////////////////////////////////////////////
//	1. Tray Picker I/O 함수
void CCommon::Set_TrayPickerGripClose()
{
	DY_DATA_00 *pDY00 = g_objAJinAXL.Get_pDY00();
	pDY00->oTrayPickerGrip12Open = FALSE; pDY00->oTrayPickerGrip12Close = TRUE;
	pDY00->oTrayPickerGrip34Open = FALSE; pDY00->oTrayPickerGrip34Close = TRUE;
	g_objAJinAXL.Write_Output(0);
}

BOOL CCommon::Get_TrayPickerGripClose()
{
	DX_DATA_00 *pDX00 = g_objAJinAXL.Get_pDX00();
	if (!pDX00->iTrayPickerGrip1Open && pDX00->iTrayPickerGrip1Close &&
		!pDX00->iTrayPickerGrip2Open && pDX00->iTrayPickerGrip2Close &&
		!pDX00->iTrayPickerGrip3Open && pDX00->iTrayPickerGrip3Close &&
		!pDX00->iTrayPickerGrip4Open && pDX00->iTrayPickerGrip4Close) return TRUE;

	return FALSE;
}

void CCommon::Set_TrayPickerGripOpen()
{
	DY_DATA_00 *pDY00 = g_objAJinAXL.Get_pDY00();
	pDY00->oTrayPickerGrip12Open = TRUE; pDY00->oTrayPickerGrip12Close = FALSE;
	pDY00->oTrayPickerGrip34Open = TRUE; pDY00->oTrayPickerGrip34Close = FALSE;
	g_objAJinAXL.Write_Output(0);
}

BOOL CCommon::Get_TrayPickerGripOpen()
{
	DX_DATA_00 *pDX00 = g_objAJinAXL.Get_pDX00();
	if (pDX00->iTrayPickerGrip1Open && !pDX00->iTrayPickerGrip1Close &&
		pDX00->iTrayPickerGrip2Open && !pDX00->iTrayPickerGrip2Close &&
		pDX00->iTrayPickerGrip3Open && !pDX00->iTrayPickerGrip3Close &&
		pDX00->iTrayPickerGrip4Open && !pDX00->iTrayPickerGrip4Close) return TRUE;

	return FALSE;
}

// 	2. Load Port I/O 함수
void CCommon::Set_LoadPortSupportIn(int nNo)
{
	DY_DATA_00 *pDY00 = g_objAJinAXL.Get_pDY00();
	DY_DATA_01 *pDY01 = g_objAJinAXL.Get_pDY01();
	if (nNo == 1) { pDY00->oLoadPort1SupportIn = TRUE; pDY00->oLoadPort1SupportOut = FALSE; }
	if (nNo == 2) { pDY01->oLoadPort2SupportIn = TRUE; pDY01->oLoadPort2SupportOut = FALSE; }
	g_objAJinAXL.Write_Output(0);
	g_objAJinAXL.Write_Output(1);
}

BOOL CCommon::Get_LoadPortSupportIn(int nNo)
{
	DX_DATA_00 *pDX00 = g_objAJinAXL.Get_pDX00();
	DX_DATA_01 *pDX01 = g_objAJinAXL.Get_pDX01();
	if (nNo == 1) {
		if (!pDX00->iLoadPort1Support1In || pDX00->iLoadPort1Support1Out) return FALSE;
		if (!pDX00->iLoadPort1Support2In || pDX00->iLoadPort1Support2Out) return FALSE;
	} else if (nNo == 2) {
		if (!pDX01->iLoadPort2Support1In || pDX01->iLoadPort2Support1Out) return FALSE;
		if (!pDX01->iLoadPort2Support2In || pDX01->iLoadPort2Support2Out) return FALSE;
	} 
	return TRUE;
}

void CCommon::Set_LoadPortSupportOut(int nNo)
{
	DY_DATA_00 *pDY00 = g_objAJinAXL.Get_pDY00();
	DY_DATA_01 *pDY01 = g_objAJinAXL.Get_pDY01();
	if (nNo == 1) pDY00->oLoadPort1SupportIn = FALSE; pDY00->oLoadPort1SupportOut = TRUE;
	if (nNo == 2) pDY01->oLoadPort2SupportIn = FALSE; pDY01->oLoadPort2SupportOut = TRUE;
	g_objAJinAXL.Write_Output(0);
	g_objAJinAXL.Write_Output(1);
}

BOOL CCommon::Get_LoadPortSupportOut(int nNo)
{
	DX_DATA_00 *pDX00 = g_objAJinAXL.Get_pDX00();
	DX_DATA_01 *pDX01 = g_objAJinAXL.Get_pDX01();
	if (nNo == 1) {
		if (pDX00->iLoadPort1Support1In || !pDX00->iLoadPort1Support1Out) return FALSE;
		if (pDX00->iLoadPort1Support2In || !pDX00->iLoadPort1Support2Out) return FALSE;
	} else if (nNo == 2) {
		if (pDX01->iLoadPort2Support1In || !pDX01->iLoadPort2Support1Out) return FALSE;
		if (pDX01->iLoadPort2Support2In || !pDX01->iLoadPort2Support2Out) return FALSE;
	} 
	return TRUE;
}

// 	3. Load Picker I/O 함수
void CCommon::Set_LoadPickerUp()
{
	DY_DATA_06 *pDY06 = g_objAJinAXL.Get_pDY06();
	pDY06->oLoadPickerDown01 = FALSE;
	pDY06->oLoadPickerDown02 = FALSE;
	pDY06->oLoadPickerDown03 = FALSE;
	pDY06->oLoadPickerDown04 = FALSE;
	g_objAJinAXL.Write_Output(6);
}

BOOL CCommon::Get_LoadPickerUp()
{
	DX_DATA_06 *pDX06 = g_objAJinAXL.Get_pDX06();

	if (pDX06->iLoadPickerUp01 && !pDX06->iLoadPickerDown01 &&
		pDX06->iLoadPickerUp02 && !pDX06->iLoadPickerDown02 &&
		pDX06->iLoadPickerUp03 && !pDX06->iLoadPickerDown03 &&
		pDX06->iLoadPickerUp04 && !pDX06->iLoadPickerDown04) return TRUE;

	return FALSE;
}

void CCommon::Set_LoadPickerDown(int nNo)
{
	DY_DATA_06 *pDY06 = g_objAJinAXL.Get_pDY06();
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	if (nNo == 0 || nNo == 1) pDY06->oLoadPickerDown01 = TRUE;
	if (nNo == 0 || nNo == 2) pDY06->oLoadPickerDown02 = TRUE;
	if (nNo == 0 || nNo == 3) pDY06->oLoadPickerDown03 = TRUE;
	if (nNo == 0 || nNo == 4) pDY06->oLoadPickerDown04 = TRUE;
	g_objAJinAXL.Write_Output(6);
}

BOOL CCommon::Get_LoadPickerDown(int nNo)
{
	DX_DATA_06 *pDX06 = g_objAJinAXL.Get_pDX06();
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	if (nNo == 0) {
		if (pDX06->iLoadPickerUp01 || !pDX06->iLoadPickerDown01) return FALSE;
		if (pDX06->iLoadPickerUp02 || !pDX06->iLoadPickerDown02) return FALSE;
		if (pDX06->iLoadPickerUp03 || !pDX06->iLoadPickerDown03) return FALSE;
		if (pDX06->iLoadPickerUp04 || !pDX06->iLoadPickerDown04) return FALSE;
		return TRUE;
	}
	else if (nNo ==  1 && !pDX06->iLoadPickerUp01 && pDX06->iLoadPickerDown01) return TRUE;
	else if (nNo ==  2 && !pDX06->iLoadPickerUp02 && pDX06->iLoadPickerDown02) return TRUE;
	else if (nNo ==  3 && !pDX06->iLoadPickerUp03 && pDX06->iLoadPickerDown03) return TRUE;
	else if (nNo ==  4 && !pDX06->iLoadPickerUp04 && pDX06->iLoadPickerDown04) return TRUE;
	return FALSE;
}

void CCommon::Set_LoadPickerOpen(int nNo)
{
	DY_DATA_06 *pDY06 = g_objAJinAXL.Get_pDY06();
	if(nNo == 0)
	{
		pDY06->oLoadPickerGrip01 = FALSE;
		pDY06->oLoadPickerGrip02 = FALSE;
		pDY06->oLoadPickerGrip03 = FALSE;
		if (PICK > 3 ) pDY06->oLoadPickerGrip04 = FALSE;
		//if (PICK > 4 ) pDY06->oLoadPickerGrip05 = FALSE;
		//if (PICK > 5 ) pDY06->oLoadPickerGrip06 = FALSE;
	}

	if(nNo == 1) pDY06->oLoadPickerGrip01 = FALSE;
	if(nNo == 2) pDY06->oLoadPickerGrip02 = FALSE;
	if(nNo == 3) pDY06->oLoadPickerGrip03 = FALSE;
	if(nNo == 4) if (PICK > 3 ) pDY06->oLoadPickerGrip04 = FALSE;
	//if(nNo == 5) if (PICK > 4 ) pDY06->oLoadPickerGrip05 = FALSE;
	//if(nNo == 6) if (PICK > 5 ) pDY06->oLoadPickerGrip06 = FALSE;

	g_objAJinAXL.Write_Output(6);
}

BOOL CCommon::Get_LoadPickerOpen(int nNo)
{

#ifndef AJIN_BOARD_USE
	return TRUE;
#endif

	DX_DATA_06 *pDX06 = g_objAJinAXL.Get_pDX06();
	if(nNo == 0)
	{
		if (!pDX06->iLoadPickerOpen01) { return FALSE; }
		if (!pDX06->iLoadPickerOpen02) { return FALSE; }
		if (!pDX06->iLoadPickerOpen03) {  return FALSE; }
		if (PICK > 3 && !pDX06->iLoadPickerOpen04) { return FALSE; }
		//if (PICK > 4 && !pDX06->iLoadPickerOpen05) {  return FALSE; }
		//if (PICK > 5 && !pDX06->iLoadPickerOpen06) {  return FALSE; }
	}

	if(nNo == 1) if (!pDX06->iLoadPickerOpen01) { return FALSE; }
	if(nNo == 2) if (!pDX06->iLoadPickerOpen02) { return FALSE; }
	if(nNo == 3) if (!pDX06->iLoadPickerOpen03) { return FALSE; }
	if(nNo == 4) if (PICK > 3 && !pDX06->iLoadPickerOpen04) { return FALSE; }
	//if(nNo == 5) if (PICK > 4 && !pDX06->iLoadPickerOpen05) {  return FALSE; }
	//if(nNo == 6) if (PICK > 5 && !pDX06->iLoadPickerOpen06) {  return FALSE; }
	return TRUE;
}

void CCommon::Set_LoadPickerClose(int nNo)
{
	DY_DATA_06 *pDY06 = g_objAJinAXL.Get_pDY06();

	if(nNo == 0)
	{
		pDY06->oLoadPickerGrip01 = TRUE;
		pDY06->oLoadPickerGrip02 = TRUE;
		pDY06->oLoadPickerGrip03 = TRUE;
		if (PICK > 3 ) pDY06->oLoadPickerGrip04 = TRUE;
		//if (PICK > 4 ) pDY06->oLoadPickerGrip05 = TRUE;
		//if (PICK > 5 ) pDY06->oLoadPickerGrip06 = TRUE;
	}

	if(nNo == 1) pDY06->oLoadPickerGrip01 = TRUE;
	if(nNo == 2) pDY06->oLoadPickerGrip02 = TRUE;
	if(nNo == 3) pDY06->oLoadPickerGrip03 = TRUE;
	if(nNo == 4) if (PICK > 3 ) pDY06->oLoadPickerGrip04 = TRUE;
	//if(nNo == 5) if (PICK > 4 ) pDY06->oLoadPickerGrip05 = TRUE;
	//if(nNo == 6) if (PICK > 5 ) pDY06->oLoadPickerGrip06 = TRUE;
	g_objAJinAXL.Write_Output(6);
}

BOOL CCommon::Get_LoadPickerClose(int nNo)
{

#ifndef AJIN_BOARD_USE
	return TRUE;
#endif

	DX_DATA_06 *pDX06 = g_objAJinAXL.Get_pDX06();
	if(nNo == 0)
	{
		if (!pDX06->iLoadPickerCMCheck01) {  return FALSE; }
		if (!pDX06->iLoadPickerCMCheck02) {  return FALSE; }
		if (!pDX06->iLoadPickerCMCheck03) {  return FALSE; }
		if (PICK > 3 && !pDX06->iLoadPickerCMCheck04) { return FALSE; }
		//if (PICK > 4 && !pDX06->iLoadPickerCMCheck05) { return FALSE; }
		//if (PICK > 5 && !pDX06->iLoadPickerCMCheck06) { return FALSE; }
	}

	if(nNo == 1) if (!pDX06->iLoadPickerCMCheck01) { return FALSE; }
	if(nNo == 2) if (!pDX06->iLoadPickerCMCheck02) { return FALSE; }
	if(nNo == 3) if (!pDX06->iLoadPickerCMCheck03) { return FALSE; }
	if(nNo == 4) if (PICK > 3 && !pDX06->iLoadPickerCMCheck04) { return FALSE; }
	//if(nNo == 5) if (PICK > 4 && !pDX06->iLoadPickerCMCheck05) { return FALSE; }
	//if(nNo == 6) if (PICK > 5 && !pDX06->iLoadPickerCMCheck06) { return FALSE; }

	return TRUE;
}


BOOL CCommon::Get_LoadPickerCmCheckOff()
{
	DX_DATA_06 *pDX06 = g_objAJinAXL.Get_pDX06();

#ifdef AJIN_BOARD_USE
	if (!pDX06->iLoadPickerCMCheck01 && !pDX06->iLoadPickerCMCheck02 &&
		!pDX06->iLoadPickerCMCheck03 &&	!pDX06->iLoadPickerCMCheck04) return TRUE;
#else
	return TRUE;
#endif
	return FALSE;
}

void CCommon::Set_LoadPickerDownMulti(int nNo, int nCnt)
{
	DY_DATA_06 *pDY06 = g_objAJinAXL.Get_pDY06();

	if (nNo == 1) {
		if (nCnt > 0) pDY06->oLoadPickerDown01 = TRUE;
		if (nCnt > 1) pDY06->oLoadPickerDown02 = TRUE;
		if (nCnt > 2) pDY06->oLoadPickerDown03 = TRUE;
		if (nCnt > 3) pDY06->oLoadPickerDown04 = TRUE;
	}
	else if (nNo == 2) {
		if (nCnt > 0) pDY06->oLoadPickerDown02 = TRUE;
		if (nCnt > 1) pDY06->oLoadPickerDown03 = TRUE;
		if (nCnt > 2) pDY06->oLoadPickerDown04 = TRUE;
	}
	else if (nNo == 3) {
		if (nCnt > 0) pDY06->oLoadPickerDown03 = TRUE;
		if (nCnt > 1) pDY06->oLoadPickerDown04 = TRUE;
	}
	else if (nNo == 4) {
		if (nCnt > 0) pDY06->oLoadPickerDown04 = TRUE;
	}

	g_objAJinAXL.Write_Output(6);
}

BOOL CCommon::Get_LoadPickerDownMulti(int nNo, int nCnt)
{
	DX_DATA_06 *pDX06 = g_objAJinAXL.Get_pDX06();

	if (nNo == 1) {
		if (nCnt > 0 && (pDX06->iLoadPickerUp01 || !pDX06->iLoadPickerDown01)) return FALSE;
		if (nCnt > 1 && (pDX06->iLoadPickerUp02 || !pDX06->iLoadPickerDown02)) return FALSE;
		if (nCnt > 2 && (pDX06->iLoadPickerUp03 || !pDX06->iLoadPickerDown03)) return FALSE;
		if (nCnt > 3 && (pDX06->iLoadPickerUp04 || !pDX06->iLoadPickerDown04)) return FALSE;
	}
	else if (nNo == 2) {
		if (nCnt > 0 && (pDX06->iLoadPickerUp02 || !pDX06->iLoadPickerDown02)) return FALSE;
		if (nCnt > 1 && (pDX06->iLoadPickerUp03 || !pDX06->iLoadPickerDown03)) return FALSE;
		if (nCnt > 2 && (pDX06->iLoadPickerUp04 || !pDX06->iLoadPickerDown04)) return FALSE;
	}
	else if (nNo == 3) {
		if (nCnt > 0 && (pDX06->iLoadPickerUp03 || !pDX06->iLoadPickerDown03)) return FALSE;
		if (nCnt > 1 && (pDX06->iLoadPickerUp04 || !pDX06->iLoadPickerDown04)) return FALSE;
	}
	else if (nNo == 4) {
		if (nCnt > 0 && (pDX06->iLoadPickerUp04 || !pDX06->iLoadPickerDown04)) return FALSE;
	}

	return TRUE;
}

// 4. Index Table I/O 함수
BOOL CCommon::Get_IndexLoadVacuumOff(int nNo)
{
	DX_DATA_11 *pDX11 = g_objAJinAXL.Get_pDX11();

	if(nNo == 0){
		if (!pDX11->iIndexLoadVac01 && !pDX11->iIndexLoadVac02 &&
			!pDX11->iIndexLoadVac03 && !pDX11->iIndexLoadVac04) return TRUE;
	}
	else if (nNo == 1) { if (!pDX11->iIndexLoadVac01) return TRUE; }
	else if (nNo == 2) { if (!pDX11->iIndexLoadVac02) return TRUE; }
	else if (nNo == 3) { if (!pDX11->iIndexLoadVac03) return TRUE; }
	else if (nNo == 4) { if (!pDX11->iIndexLoadVac04) return TRUE; }

	return FALSE;
}

void CCommon::Set_IndexLoadVacuumOff(int nNo)
{
	DY_DATA_11 *pDY11 = g_objAJinAXL.Get_pDY11();

	if (nNo == 0 || nNo == 1) pDY11->oIndexLoadVac01 = FALSE;
	if (nNo == 0 || nNo == 2) pDY11->oIndexLoadVac02 = FALSE;
	if (nNo == 0 || nNo == 3) pDY11->oIndexLoadVac03 = FALSE;
	if (nNo == 0 || nNo == 4) pDY11->oIndexLoadVac04 = FALSE;

	g_objAJinAXL.Write_Output(11);
}

BOOL CCommon::Get_IndexAssyVacuumOff(int nNo)
{
	DX_DATA_11 *pDX11 = g_objAJinAXL.Get_pDX11();

	if(nNo == 0){
		if (!pDX11->iIndexAssyVac01 && !pDX11->iIndexAssyVac02 &&
			!pDX11->iIndexAssyVac03 && !pDX11->iIndexAssyVac04) return TRUE;
	}
	else if (nNo == 1) { if (!pDX11->iIndexAssyVac01) return TRUE; }
	else if (nNo == 2) { if (!pDX11->iIndexAssyVac02) return TRUE; }
	else if (nNo == 3) { if (!pDX11->iIndexAssyVac03) return TRUE; }
	else if (nNo == 4) { if (!pDX11->iIndexAssyVac04) return TRUE; }

	return FALSE;
}

void CCommon::Set_IndexAssyVacuumOff(int nNo)
{
	DY_DATA_11 *pDY11 = g_objAJinAXL.Get_pDY11();

	if (nNo == 0 || nNo == 1) pDY11->oIndexAssyVac01 = FALSE;
	if (nNo == 0 || nNo == 2) pDY11->oIndexAssyVac02 = FALSE;
	if (nNo == 0 || nNo == 3) pDY11->oIndexAssyVac03 = FALSE;
	if (nNo == 0 || nNo == 4) pDY11->oIndexAssyVac04 = FALSE;

	g_objAJinAXL.Write_Output(11);
}

// 	5. Cap Port I/O 함수
void CCommon::Set_CapPortSupportIn(int nNo)
{
	DY_DATA_02 *pDY02 = g_objAJinAXL.Get_pDY02();
	if (nNo == 1) { pDY02->oCapPort1SupportIn = TRUE; pDY02->oCapPort1SupportOut = FALSE; }
	if (nNo == 2) { pDY02->oCapPort2SupportIn = TRUE; pDY02->oCapPort2SupportOut = FALSE; }
	g_objAJinAXL.Write_Output(2);
}

BOOL CCommon::Get_CapPortSupportIn(int nNo)
{
	DX_DATA_02 *pDX02 = g_objAJinAXL.Get_pDX02();
	if (nNo == 1) {
		if (!pDX02->iCapPort1Support1In || pDX02->iCapPort1Support1Out) return FALSE;
		if (!pDX02->iCapPort1Support2In || pDX02->iCapPort1Support2Out) return FALSE;
	} else if (nNo == 2) {
		if (!pDX02->iCapPort2Support1In || pDX02->iCapPort2Support1Out) return FALSE;
		if (!pDX02->iCapPort2Support2In || pDX02->iCapPort2Support2Out) return FALSE;
	} 
	return TRUE;
}

void CCommon::Set_CapPortSupportOut(int nNo)
{
	DY_DATA_02 *pDY02 = g_objAJinAXL.Get_pDY02();
	if (nNo == 1) pDY02->oCapPort1SupportIn = FALSE; pDY02->oCapPort1SupportOut = TRUE;
	if (nNo == 2) pDY02->oCapPort2SupportIn = FALSE; pDY02->oCapPort2SupportOut = TRUE;
	g_objAJinAXL.Write_Output(2);
}

BOOL CCommon::Get_CapPortSupportOut(int nNo)
{
	DX_DATA_02 *pDX02 = g_objAJinAXL.Get_pDX02();
	if (nNo == 1) {
		if (pDX02->iCapPort1Support1In || !pDX02->iCapPort1Support1Out) return FALSE;
		if (pDX02->iCapPort1Support2In || !pDX02->iCapPort1Support2Out) return FALSE;
	} else if (nNo == 2) {
		if (pDX02->iCapPort2Support1In || !pDX02->iCapPort2Support1Out) return FALSE;
		if (pDX02->iCapPort2Support2In || !pDX02->iCapPort2Support2Out) return FALSE;
	} 
	return TRUE;
}

// 	6. Cap Picker I/O 함수
void CCommon::Set_CapPickerUp(int nNo)
{
	DY_DATA_07 *pDY07 = g_objAJinAXL.Get_pDY07();
	if (nNo == 0 || nNo == 1) pDY07->oCapPickerDown01 = FALSE;
	if (nNo == 0 || nNo == 2) pDY07->oCapPickerDown02 = FALSE;
	if (nNo == 0 || nNo == 3) pDY07->oCapPickerDown03 = FALSE;
	if (nNo == 0 || nNo == 4) pDY07->oCapPickerDown04 = FALSE;
	g_objAJinAXL.Write_Output(7);
}

BOOL CCommon::Get_CapPickerUp(int nNo)
{
	DX_DATA_07 *pDX07 = g_objAJinAXL.Get_pDX07();
	if (nNo == 0) {
		if (pDX07->iCapPickerUp01 && !pDX07->iCapPickerDown01 &&
			pDX07->iCapPickerUp02 && !pDX07->iCapPickerDown02 &&
			pDX07->iCapPickerUp03 && !pDX07->iCapPickerDown03 &&
			pDX07->iCapPickerUp04 && !pDX07->iCapPickerDown04) return TRUE;
	}
	else if (nNo ==  1 && pDX07->iCapPickerUp01 && !pDX07->iCapPickerDown01) return TRUE;
	else if (nNo ==  2 && pDX07->iCapPickerUp02 && !pDX07->iCapPickerDown02) return TRUE;
	else if (nNo ==  3 && pDX07->iCapPickerUp03 && !pDX07->iCapPickerDown03) return TRUE;
	else if (nNo ==  4 && pDX07->iCapPickerUp04 && !pDX07->iCapPickerDown04) return TRUE;
	return FALSE;
}

void CCommon::Set_CapPickerNormal(int nNo)
{
	DY_DATA_07 *pDY07 = g_objAJinAXL.Get_pDY07();
	if (nNo == 0 || nNo == 1) pDY07->oCapPickerTurn01 = FALSE;
	if (nNo == 0 || nNo == 2) pDY07->oCapPickerTurn02 = FALSE;
	if (nNo == 0 || nNo == 3) pDY07->oCapPickerTurn03 = FALSE;
	if (nNo == 0 || nNo == 4) pDY07->oCapPickerTurn04 = FALSE;
	g_objAJinAXL.Write_Output(7);
}

BOOL CCommon::Get_CapPickerNormal(int nNo)
{
	DX_DATA_07 *pDX07 = g_objAJinAXL.Get_pDX07();
	if (nNo == 0) {
		if (pDX07->iCapPickerTurn01) return FALSE;
		if (pDX07->iCapPickerTurn02) return FALSE;
		if (pDX07->iCapPickerTurn03) return FALSE;
		if (pDX07->iCapPickerTurn04) return FALSE;

		if (!pDX07->iCapPickerTurn01 && !pDX07->iCapPickerTurn02 && !pDX07->iCapPickerTurn03 && !pDX07->iCapPickerTurn04)
		{
			return TRUE;
		}
	}
	else if (nNo ==  1 && !pDX07->iCapPickerTurn01) return TRUE;
	else if (nNo ==  2 && !pDX07->iCapPickerTurn02) return TRUE;
	else if (nNo ==  3 && !pDX07->iCapPickerTurn03) return TRUE;
	else if (nNo ==  4 && !pDX07->iCapPickerTurn04) return TRUE;
	return FALSE;
}

void CCommon::Set_CapPickerTurn(int nNo)
{
	DY_DATA_07 *pDY07 = g_objAJinAXL.Get_pDY07();
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	if (nNo == 0 || nNo == 1) pDY07->oCapPickerTurn01 = TRUE;
	if (nNo == 0 || nNo == 2) pDY07->oCapPickerTurn02 = TRUE;
	if (nNo == 0 || nNo == 3) pDY07->oCapPickerTurn03 = TRUE;
	if (nNo == 0 || nNo == 4) pDY07->oCapPickerTurn04 = TRUE;
	g_objAJinAXL.Write_Output(7);
}

BOOL CCommon::Get_CapPickerTurn(int nNo)
{
	DX_DATA_07 *pDX07 = g_objAJinAXL.Get_pDX07();
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();

	if (nNo == 0) {
		if (!pDX07->iCapPickerTurn01) return FALSE;
		if (!pDX07->iCapPickerTurn02) return FALSE;
		if (!pDX07->iCapPickerTurn03) return FALSE;
		if (!pDX07->iCapPickerTurn04) return FALSE;
		return TRUE;
	}
	else if (nNo ==  1 && pDX07->iCapPickerTurn01) return TRUE;
	else if (nNo ==  2 && pDX07->iCapPickerTurn02) return TRUE;
	else if (nNo ==  3 && pDX07->iCapPickerTurn03) return TRUE;
	else if (nNo ==  4 && pDX07->iCapPickerTurn04) return TRUE;
	return FALSE;
}

void CCommon::Set_CapPickerVacOff(int nNo)
{
	DY_DATA_07 *pDY07 = g_objAJinAXL.Get_pDY07();
	if (nNo == 0 || nNo == 1) { pDY07->oCapPickerVac01 = FALSE; pDY07->oCapPickerAir01 = TRUE; }
	if (nNo == 0 || nNo == 2) { pDY07->oCapPickerVac02 = FALSE; pDY07->oCapPickerAir02 = TRUE; }
	if (nNo == 0 || nNo == 3) { pDY07->oCapPickerVac03 = FALSE; pDY07->oCapPickerAir03 = TRUE; }
	if (nNo == 0 || nNo == 4) { pDY07->oCapPickerVac04 = FALSE; pDY07->oCapPickerAir04 = TRUE; }
	g_objAJinAXL.Write_Output(7);
}

BOOL CCommon::Get_CapPickerVacOff(int nNo)
{
	DX_DATA_07 *pDX07 = g_objAJinAXL.Get_pDX07();
	if (nNo == 0) {
		if (!pDX07->iCapPickerVac01 && !pDX07->iCapPickerVac02 &&
			!pDX07->iCapPickerVac03 && !pDX07->iCapPickerVac04) return TRUE;
	}
	else if (nNo ==  1 && !pDX07->iCapPickerVac01) return TRUE;
	else if (nNo ==  2 && !pDX07->iCapPickerVac02) return TRUE;
	else if (nNo ==  3 && !pDX07->iCapPickerVac03) return TRUE;
	else if (nNo ==  4 && !pDX07->iCapPickerVac04) return TRUE;
	return FALSE;
}

void CCommon::Set_CapPickerAirOff(int nNo)
{
	DY_DATA_07 *pDY07 = g_objAJinAXL.Get_pDY07();
	if (nNo == 0 || nNo == 1) pDY07->oCapPickerAir01 = FALSE;
	if (nNo == 0 || nNo == 2) pDY07->oCapPickerAir02 = FALSE;
	if (nNo == 0 || nNo == 3) pDY07->oCapPickerAir03 = FALSE;
	if (nNo == 0 || nNo == 4) pDY07->oCapPickerAir04 = FALSE;
	g_objAJinAXL.Write_Output(7);
}

void CCommon::Set_CapPickerDownMulti(int nNo, int nCnt)
{
	DY_DATA_07 *pDY07 = g_objAJinAXL.Get_pDY07();

	if (nNo == 1) {
		if (nCnt > 0) pDY07->oCapPickerDown01 = TRUE;
		if (nCnt > 1) pDY07->oCapPickerDown02 = TRUE;
		if (nCnt > 2) pDY07->oCapPickerDown03 = TRUE;
		if (nCnt > 3) pDY07->oCapPickerDown04 = TRUE;
	}
	else if (nNo == 2) {
		if (nCnt > 0) pDY07->oCapPickerDown02 = TRUE;
		if (nCnt > 1) pDY07->oCapPickerDown03 = TRUE;
		if (nCnt > 2) pDY07->oCapPickerDown04 = TRUE;
	}
	else if (nNo == 3) {
		if (nCnt > 0) pDY07->oCapPickerDown03 = TRUE;
		if (nCnt > 1) pDY07->oCapPickerDown04 = TRUE;
	}
	else if (nNo == 4) {
		if (nCnt > 0) pDY07->oCapPickerDown04 = TRUE;
	}

	g_objAJinAXL.Write_Output(7);
}

BOOL CCommon::Get_CapPickerDownMulti(int nNo, int nCnt)
{
	DX_DATA_07 *pDX07 = g_objAJinAXL.Get_pDX07();

	if (nNo == 1) {
		if (nCnt > 0 && (pDX07->iCapPickerUp01 || !pDX07->iCapPickerDown01)) return FALSE;
		if (nCnt > 1 && (pDX07->iCapPickerUp02 || !pDX07->iCapPickerDown02)) return FALSE;
		if (nCnt > 2 && (pDX07->iCapPickerUp03 || !pDX07->iCapPickerDown03)) return FALSE;
		if (nCnt > 3 && (pDX07->iCapPickerUp04 || !pDX07->iCapPickerDown04)) return FALSE;
	}
	else if (nNo == 2) {
		if (nCnt > 0 && (pDX07->iCapPickerUp02 || !pDX07->iCapPickerDown02)) return FALSE;
		if (nCnt > 1 && (pDX07->iCapPickerUp03 || !pDX07->iCapPickerDown03)) return FALSE;
		if (nCnt > 2 && (pDX07->iCapPickerUp04 || !pDX07->iCapPickerDown04)) return FALSE;
	}
	else if (nNo == 3) {
		if (nCnt > 0 && (pDX07->iCapPickerUp03 || !pDX07->iCapPickerDown03)) return FALSE;
		if (nCnt > 1 && (pDX07->iCapPickerUp04 || !pDX07->iCapPickerDown04)) return FALSE;
	}
	else if (nNo == 4) {
		if (nCnt > 0 && (pDX07->iCapPickerUp04 || !pDX07->iCapPickerDown04)) return FALSE;
	}

	return TRUE;
}

void CCommon::Set_CapPickerVacOnMulti(int nNo, int nCnt)
{
#ifdef DRY_RUN_TEST
	return;
#endif
	DY_DATA_07 *pDY07 = g_objAJinAXL.Get_pDY07();

	if (nNo == 1) {
		if (nCnt > 0) { pDY07->oCapPickerVac01 = TRUE; }
		if (nCnt > 1) { pDY07->oCapPickerVac02 = TRUE; }
		if (nCnt > 2) { pDY07->oCapPickerVac03 = TRUE; }
		if (nCnt > 3) { pDY07->oCapPickerVac04 = TRUE; }
	}
	else if (nNo == 2) {
		if (nCnt > 0) { pDY07->oCapPickerVac02 = TRUE; }
		if (nCnt > 1) { pDY07->oCapPickerVac03 = TRUE; }
		if (nCnt > 2) { pDY07->oCapPickerVac04 = TRUE; }
	}
	else if (nNo == 3) {
		if (nCnt > 0) { pDY07->oCapPickerVac03 = TRUE; }
		if (nCnt > 1) { pDY07->oCapPickerVac04 = TRUE; }
	}
	else if (nNo == 4) {
		if (nCnt > 0) { pDY07->oCapPickerVac04 = TRUE; }
	}

	g_objAJinAXL.Write_Output(7);
}

BOOL CCommon::Get_CapPickerVacOnMulti(int nNo, int nCnt)
{
#ifdef DRY_RUN_TEST
	return TRUE;
#endif
	DX_DATA_07 *pDX07 = g_objAJinAXL.Get_pDX07();

	if (nNo == 1) {
		if (nCnt > 0 && !pDX07->iCapPickerVac01) return FALSE;
		if (nCnt > 1 && !pDX07->iCapPickerVac02) return FALSE;
		if (nCnt > 2 && !pDX07->iCapPickerVac03) return FALSE;
		if (nCnt > 3 && !pDX07->iCapPickerVac04) return FALSE;
	}
	else if (nNo == 2) {
		if (nCnt > 0 && !pDX07->iCapPickerVac02) return FALSE;
		if (nCnt > 1 && !pDX07->iCapPickerVac03) return FALSE;
		if (nCnt > 2 && !pDX07->iCapPickerVac04) return FALSE;
	}
	else if (nNo == 3) {
		if (nCnt > 0 && !pDX07->iCapPickerVac03) return FALSE;
		if (nCnt > 1 && !pDX07->iCapPickerVac04) return FALSE;
	}
	else if (nNo == 4) {
		if (nCnt > 0 && !pDX07->iCapPickerVac04) return FALSE;
	}

	return TRUE;
}

void CCommon::Set_CapPickerVacOffMulti(int nNo, int nCnt)
{
	DY_DATA_07 *pDY07 = g_objAJinAXL.Get_pDY07();

	if (nNo == 1) {
		if (nCnt > 0) { pDY07->oCapPickerVac01 = FALSE; pDY07->oCapPickerAir01 = TRUE; }
		if (nCnt > 1) { pDY07->oCapPickerVac02 = FALSE; pDY07->oCapPickerAir02 = TRUE; }
		if (nCnt > 2) { pDY07->oCapPickerVac03 = FALSE; pDY07->oCapPickerAir03 = TRUE; }
		if (nCnt > 3) { pDY07->oCapPickerVac04 = FALSE; pDY07->oCapPickerAir04 = TRUE; }
	}
	else if (nNo == 2) {
		if (nCnt > 0) { pDY07->oCapPickerVac02 = FALSE; pDY07->oCapPickerAir02 = TRUE; }
		if (nCnt > 1) { pDY07->oCapPickerVac03 = FALSE; pDY07->oCapPickerAir03 = TRUE; }
		if (nCnt > 2) { pDY07->oCapPickerVac04 = FALSE; pDY07->oCapPickerAir04 = TRUE; }
	}
	else if (nNo == 3) {
		if (nCnt > 0) { pDY07->oCapPickerVac03 = FALSE; pDY07->oCapPickerAir03 = TRUE; }
		if (nCnt > 1) { pDY07->oCapPickerVac04 = FALSE; pDY07->oCapPickerAir04 = TRUE; }
	}
	else if (nNo == 4) {
		if (nCnt > 0) { pDY07->oCapPickerVac04 = FALSE; pDY07->oCapPickerAir04 = TRUE; }
	}

	g_objAJinAXL.Write_Output(8);
}

BOOL CCommon::Get_CapPickerVacOffMulti(int nNo, int nCnt)
{
	DX_DATA_07 *pDX07 = g_objAJinAXL.Get_pDX07();

	if (nNo == 1) {
		if (nCnt > 0 && pDX07->iCapPickerVac01) return FALSE;
		if (nCnt > 1 && pDX07->iCapPickerVac02) return FALSE;
		if (nCnt > 2 && pDX07->iCapPickerVac03) return FALSE;
		if (nCnt > 3 && pDX07->iCapPickerVac04) return FALSE;
	}
	else if (nNo == 2) {
		if (nCnt > 0 && pDX07->iCapPickerVac02) return FALSE;
		if (nCnt > 1 && pDX07->iCapPickerVac03) return FALSE;
		if (nCnt > 2 && pDX07->iCapPickerVac04) return FALSE;
	}
	else if (nNo == 3) {
		if (nCnt > 0 && pDX07->iCapPickerVac03) return FALSE;
		if (nCnt > 1 && pDX07->iCapPickerVac04) return FALSE;
	}
	else if (nNo == 4) {
		if (nCnt > 0 && pDX07->iCapPickerVac04) return FALSE;
	}

	return TRUE;
}

void CCommon::Set_CapPickerAirOffMulti(int nNo, int nCnt)
{
	DY_DATA_07 *pDY07 = g_objAJinAXL.Get_pDY07();

	if (nNo == 1) {
		if (nCnt > 0) pDY07->oCapPickerAir01 = FALSE;
		if (nCnt > 1) pDY07->oCapPickerAir02 = FALSE;
		if (nCnt > 2) pDY07->oCapPickerAir03 = FALSE;
		if (nCnt > 3) pDY07->oCapPickerAir04 = FALSE;
	}
	else if (nNo == 2) {
		if (nCnt > 0) pDY07->oCapPickerAir02 = FALSE;
		if (nCnt > 1) pDY07->oCapPickerAir03 = FALSE;
		if (nCnt > 2) pDY07->oCapPickerAir04 = FALSE;
	}
	else if (nNo == 3) {
		if (nCnt > 0) pDY07->oCapPickerAir03 = FALSE;
		if (nCnt > 1) pDY07->oCapPickerAir04 = FALSE;
	}
	else if (nNo == 4) {
		if (nCnt > 0) pDY07->oCapPickerAir04 = FALSE;
	}

	g_objAJinAXL.Write_Output(7);
}

//	7. Cap Buffer I/O 함수
void CCommon::Set_CapBufferAlign(BOOL bOn)
{
	DY_DATA_09 *pDY09 = g_objAJinAXL.Get_pDY09();
	pDY09->oCapBufferAlignIn = bOn;

	g_objAJinAXL.Write_Output(9);
}

BOOL CCommon::Get_CapBufferAlign(BOOL bOn)
{
	DX_DATA_09 *pDX09 = g_objAJinAXL.Get_pDX09();
	
	if (bOn && pDX09->iCapBufferStageAlignIn && !pDX09->iCapBufferStageAlignOut) return TRUE;
	if (!bOn && !pDX09->iCapBufferStageAlignIn && pDX09->iCapBufferStageAlignOut) return TRUE;

	return FALSE;	
}

void CCommon::Set_CapCleanAirOn()
{
	DY_DATA_09 *pDY09 = g_objAJinAXL.Get_pDY09();
	pDY09->oCapCleanAir01 = TRUE;
	pDY09->oCapCleanAir02 = TRUE;
	pDY09->oCapCleanAir03 = TRUE;
	pDY09->oCapCleanAir04 = TRUE;
	pDY09->oCapCleanSuction = TRUE;

	g_objAJinAXL.Write_Output(9);
}

void CCommon::Set_CapCleanAirOff()
{
	DY_DATA_09 *pDY09 = g_objAJinAXL.Get_pDY09();
	pDY09->oCapCleanAir01 = FALSE;
	pDY09->oCapCleanAir02 = FALSE;
	pDY09->oCapCleanAir03 = FALSE;
	pDY09->oCapCleanAir04 = FALSE;
	pDY09->oCapCleanSuction = FALSE;

	g_objAJinAXL.Write_Output(9);
}

// 	8. Assembly Picker I/O 함수
void CCommon::Set_AssyPickerUp(int nNo)
{
	DY_DATA_08 *pDY08 = g_objAJinAXL.Get_pDY08();
	if (nNo == 0 || nNo == 1) pDY08->oAssyPickerDown01 = FALSE;
	if (nNo == 0 || nNo == 2) pDY08->oAssyPickerDown02 = FALSE;
	if (nNo == 0 || nNo == 3) pDY08->oAssyPickerDown03 = FALSE;
	if (nNo == 0 || nNo == 4) pDY08->oAssyPickerDown04 = FALSE;
	g_objAJinAXL.Write_Output(8);
}

BOOL CCommon::Get_AssyPickerUp(int nNo)
{
	DX_DATA_08 *pDX08 = g_objAJinAXL.Get_pDX08();
	if (nNo == 0) {
		if (pDX08->iAssyPickerUp01 && !pDX08->iAssyPickerDown01 &&
			pDX08->iAssyPickerUp02 && !pDX08->iAssyPickerDown02 &&
			pDX08->iAssyPickerUp03 && !pDX08->iAssyPickerDown03 &&
			pDX08->iAssyPickerUp04 && !pDX08->iAssyPickerDown04) return TRUE;
	}
	else if (nNo ==  1 && pDX08->iAssyPickerUp01 && !pDX08->iAssyPickerDown01) return TRUE;
	else if (nNo ==  2 && pDX08->iAssyPickerUp02 && !pDX08->iAssyPickerDown02) return TRUE;
	else if (nNo ==  3 && pDX08->iAssyPickerUp03 && !pDX08->iAssyPickerDown03) return TRUE;
	else if (nNo ==  4 && pDX08->iAssyPickerUp04 && !pDX08->iAssyPickerDown04) return TRUE;
	return FALSE;
}

void CCommon::Set_AssyPickerDown(int nNo)
{
	DY_DATA_08 *pDY08 = g_objAJinAXL.Get_pDY08();
	if (nNo == 0 || nNo == 1) pDY08->oAssyPickerDown01 = TRUE;
	if (nNo == 0 || nNo == 2) pDY08->oAssyPickerDown02 = TRUE;
	if (nNo == 0 || nNo == 3) pDY08->oAssyPickerDown03 = TRUE;
	if (nNo == 0 || nNo == 4) pDY08->oAssyPickerDown04 = TRUE;
	g_objAJinAXL.Write_Output(8);
}

BOOL CCommon::Get_AssyPickerDown(int nNo)
{
	DX_DATA_08 *pDX08 = g_objAJinAXL.Get_pDX08();
	if (nNo == 0) {
		if (!pDX08->iAssyPickerUp01 && pDX08->iAssyPickerDown01 &&
			!pDX08->iAssyPickerUp02 && pDX08->iAssyPickerDown02 &&
			!pDX08->iAssyPickerUp03 && pDX08->iAssyPickerDown03 &&
			!pDX08->iAssyPickerUp04 && pDX08->iAssyPickerDown04) return TRUE;
	}
	else if (nNo ==  1 && !pDX08->iAssyPickerUp01 && pDX08->iAssyPickerDown01) return TRUE;
	else if (nNo ==  2 && !pDX08->iAssyPickerUp02 && pDX08->iAssyPickerDown02) return TRUE;
	else if (nNo ==  3 && !pDX08->iAssyPickerUp03 && pDX08->iAssyPickerDown03) return TRUE;
	else if (nNo ==  4 && !pDX08->iAssyPickerUp04 && pDX08->iAssyPickerDown04) return TRUE;
	return FALSE;
}

void CCommon::Set_AssyPickerVacOff(int nNo)
{
	DY_DATA_08 *pDY08 = g_objAJinAXL.Get_pDY08();
	if (nNo == 0 || nNo == 1) { pDY08->oAssyPickerVac01 = FALSE; pDY08->oAssyPickerAir01 = TRUE; }
	if (nNo == 0 || nNo == 2) { pDY08->oAssyPickerVac02 = FALSE; pDY08->oAssyPickerAir02 = TRUE; }
	if (nNo == 0 || nNo == 3) { pDY08->oAssyPickerVac03 = FALSE; pDY08->oAssyPickerAir03 = TRUE; }
	if (nNo == 0 || nNo == 4) { pDY08->oAssyPickerVac04 = FALSE; pDY08->oAssyPickerAir04 = TRUE; }
	g_objAJinAXL.Write_Output(8);
}

BOOL CCommon::Get_AssyPickerVacOff(int nNo)
{
	DX_DATA_08 *pDX08 = g_objAJinAXL.Get_pDX08();
	if (nNo == 0) {
		if (!pDX08->iAssyPickerVac01 && !pDX08->iAssyPickerVac02 &&
			!pDX08->iAssyPickerVac03 && !pDX08->iAssyPickerVac04) return TRUE;
	}
	else if (nNo ==  1 && !pDX08->iAssyPickerVac01) return TRUE;
	else if (nNo ==  2 && !pDX08->iAssyPickerVac02) return TRUE;
	else if (nNo ==  3 && !pDX08->iAssyPickerVac03) return TRUE;
	else if (nNo ==  4 && !pDX08->iAssyPickerVac04) return TRUE;
	return FALSE;
}

void CCommon::Set_AssyPickerAirOff(int nNo)
{
	DY_DATA_08 *pDY08 = g_objAJinAXL.Get_pDY08();
	if (nNo == 0 || nNo == 1) pDY08->oAssyPickerAir01 = FALSE;
	if (nNo == 0 || nNo == 2) pDY08->oAssyPickerAir02 = FALSE;
	if (nNo == 0 || nNo == 3) pDY08->oAssyPickerAir03 = FALSE;
	if (nNo == 0 || nNo == 4) pDY08->oAssyPickerAir04 = FALSE;
	g_objAJinAXL.Write_Output(8);
}

BOOL CCommon::Get_AssyPickerTilt(int nNo)
{
	DX_DATA_09 *pDX09 = g_objAJinAXL.Get_pDX09();
	if (nNo == 0) {
		if (!pDX09->iAssyPickerTilt01 && !pDX09->iAssyPickerTilt02 &&
			!pDX09->iAssyPickerTilt03 && !pDX09->iAssyPickerTilt04) return TRUE;
	}
	else if (nNo ==  1 && !pDX09->iAssyPickerTilt01) return TRUE;
	else if (nNo ==  2 && !pDX09->iAssyPickerTilt02) return TRUE;
	else if (nNo ==  3 && !pDX09->iAssyPickerTilt03) return TRUE;
	else if (nNo ==  4 && !pDX09->iAssyPickerTilt04) return TRUE;
	return FALSE;
}

BOOL CCommon::Get_InfoAssyPickerTilt(int nNo)
{
	DX_DATA_09 *pDX09 = g_objAJinAXL.Get_pDX09();
	
	if(pDX09->iAssyPickerTilt01 && gData.InfoIndex[1][0] > 0 && gData.InfoAssyPick[0] == 9) return FALSE;
	if(pDX09->iAssyPickerTilt02 && gData.InfoIndex[1][1] > 0 && gData.InfoAssyPick[1] == 9) return FALSE;
	if(pDX09->iAssyPickerTilt03 && gData.InfoIndex[1][2] > 0 && gData.InfoAssyPick[2] == 9) return FALSE;
	if(pDX09->iAssyPickerTilt04 && gData.InfoIndex[1][3] > 0 && gData.InfoAssyPick[3] == 9) return FALSE;

	return TRUE;
}

// 	9. Trans Stage I/O 함수
void CCommon::Set_TransStageVacOff(int nNo)
{
	DY_DATA_05 *pDY05 = g_objAJinAXL.Get_pDY05();
	if (nNo == 0 || nNo == 1) { pDY05->oTransStageVac01 = FALSE; pDY05->oTransStageAir01 = TRUE; }
	if (nNo == 0 || nNo == 2) { pDY05->oTransStageVac02 = FALSE; pDY05->oTransStageAir02 = TRUE; }
	if (nNo == 0 || nNo == 3) { pDY05->oTransStageVac03 = FALSE; pDY05->oTransStageAir03 = TRUE; }
	if (nNo == 0 || nNo == 4) { pDY05->oTransStageVac04 = FALSE; pDY05->oTransStageAir04 = TRUE; }
	g_objAJinAXL.Write_Output(5);
}

BOOL CCommon::Get_TransStageVacOff(int nNo)
{
	DX_DATA_05 *pDX05 = g_objAJinAXL.Get_pDX05();
	if (nNo == 0) {
		if (!pDX05->iTransStageVac01 && !pDX05->iTransStageVac02 &&
			!pDX05->iTransStageVac03 && !pDX05->iTransStageVac04) return TRUE;
	}
	else if (nNo ==  1 && !pDX05->iTransStageVac01) return TRUE;
	else if (nNo ==  2 && !pDX05->iTransStageVac02) return TRUE;
	else if (nNo ==  3 && !pDX05->iTransStageVac03) return TRUE;
	else if (nNo ==  4 && !pDX05->iTransStageVac04) return TRUE;
	return FALSE;
}

void CCommon::Set_TransStageAirOff(int nNo)
{
	DY_DATA_05 *pDY05 = g_objAJinAXL.Get_pDY05();
	if (nNo == 0 || nNo == 1) pDY05->oTransStageAir01 = FALSE;
	if (nNo == 0 || nNo == 2) pDY05->oTransStageAir02 = FALSE;
	if (nNo == 0 || nNo == 3) pDY05->oTransStageAir03 = FALSE;
	if (nNo == 0 || nNo == 4) pDY05->oTransStageAir04 = FALSE;
	g_objAJinAXL.Write_Output(5);
}

void CCommon::Set_TransStageClampOn()
{
	DY_DATA_05 *pDY05 = g_objAJinAXL.Get_pDY05();
	pDY05->oTransStageClamp1On = TRUE; pDY05->oTransStageClamp1Off = FALSE;
	pDY05->oTransStageClamp2On = TRUE; pDY05->oTransStageClamp2Off = FALSE;	
	g_objAJinAXL.Write_Output(5);
}

BOOL CCommon::Get_TransStageClampOn()
{
	DX_DATA_05 *pDX05 = g_objAJinAXL.Get_pDX05();
	if (pDX05->iTransStageClamp1On && !pDX05->iTransStageClamp1Off &&
		pDX05->iTransStageClamp2On && !pDX05->iTransStageClamp2Off) return TRUE;
	return FALSE;
}

void CCommon::Set_TransStageClampOff()
{
	DY_DATA_05 *pDY05 = g_objAJinAXL.Get_pDY05();
	pDY05->oTransStageClamp1On = FALSE; pDY05->oTransStageClamp1Off = TRUE;
	pDY05->oTransStageClamp2On = FALSE; pDY05->oTransStageClamp2Off = TRUE;	
	g_objAJinAXL.Write_Output(5);
}

BOOL CCommon::Get_TransStageClampOff()
{
	DX_DATA_05 *pDX05 = g_objAJinAXL.Get_pDX05();
	if (!pDX05->iTransStageClamp1On && pDX05->iTransStageClamp1Off &&
		!pDX05->iTransStageClamp2On && pDX05->iTransStageClamp2Off) return TRUE;
	return FALSE;
}

//	10. Unload Picker I/O 함수
void CCommon::Set_UnloadPickerUp(int nNo)
{
	DY_DATA_10 *pDY10 = g_objAJinAXL.Get_pDY10();
	if (nNo == 0 || nNo == 1) pDY10->oUnloadPickerDown01 = FALSE;
	if (nNo == 0 || nNo == 2) pDY10->oUnloadPickerDown02 = FALSE;
	if (nNo == 0 || nNo == 3) pDY10->oUnloadPickerDown03 = FALSE;
	if (nNo == 0 || nNo == 4) pDY10->oUnloadPickerDown04 = FALSE;
	g_objAJinAXL.Write_Output(10);
}

BOOL CCommon::Get_UnloadPickerUp(int nNo)
{
	DX_DATA_10 *pDX10 = g_objAJinAXL.Get_pDX10();
	if (nNo == 0) {
		if (pDX10->iUnloadPickerUp01 && !pDX10->iUnloadPickerDown01 &&
			pDX10->iUnloadPickerUp02 && !pDX10->iUnloadPickerDown02 &&
			pDX10->iUnloadPickerUp03 && !pDX10->iUnloadPickerDown03 &&
			pDX10->iUnloadPickerUp04 && !pDX10->iUnloadPickerDown04) return TRUE;
	}
	else if (nNo ==  1 && pDX10->iUnloadPickerUp01 && !pDX10->iUnloadPickerDown01) return TRUE;
	else if (nNo ==  2 && pDX10->iUnloadPickerUp02 && !pDX10->iUnloadPickerDown02) return TRUE;
	else if (nNo ==  3 && pDX10->iUnloadPickerUp03 && !pDX10->iUnloadPickerDown03) return TRUE;
	else if (nNo ==  4 && pDX10->iUnloadPickerUp04 && !pDX10->iUnloadPickerDown04) return TRUE;
	return FALSE;
}

void CCommon::Set_UnloadPickerVacOff(int nNo)
{
#ifdef DRY_RUN_TEST
	return;
#endif
	DY_DATA_10 *pDY10 = g_objAJinAXL.Get_pDY10();
	if (nNo == 0 || nNo == 1) { pDY10->oUnloadPickerVac01 = FALSE; pDY10->oUnloadPickerAir01 = TRUE; }
	if (nNo == 0 || nNo == 2) { pDY10->oUnloadPickerVac02 = FALSE; pDY10->oUnloadPickerAir02 = TRUE; }
	if (nNo == 0 || nNo == 3) { pDY10->oUnloadPickerVac03 = FALSE; pDY10->oUnloadPickerAir03 = TRUE; }
	if (nNo == 0 || nNo == 4) { pDY10->oUnloadPickerVac04 = FALSE; pDY10->oUnloadPickerAir04 = TRUE; }
	g_objAJinAXL.Write_Output(10);
}

BOOL CCommon::Get_UnloadPickerVacOff(int nNo)
{
	DX_DATA_10 *pDX10 = g_objAJinAXL.Get_pDX10();
	if (nNo == 0) {
		if (!pDX10->iUnloadPickerVac01 && !pDX10->iUnloadPickerVac02 &&
			!pDX10->iUnloadPickerVac03 && !pDX10->iUnloadPickerVac04) return TRUE;
	}
	else if (nNo ==  1 && !pDX10->iUnloadPickerVac01) return TRUE;
	else if (nNo ==  2 && !pDX10->iUnloadPickerVac02) return TRUE;
	else if (nNo ==  3 && !pDX10->iUnloadPickerVac03) return TRUE;
	else if (nNo ==  4 && !pDX10->iUnloadPickerVac04) return TRUE;
	return FALSE;
}

void CCommon::Set_UnloadPickerAirOff(int nNo)
{
	DY_DATA_10 *pDY10 = g_objAJinAXL.Get_pDY10();
	if (nNo == 0 || nNo == 1) pDY10->oUnloadPickerAir01 = FALSE;
	if (nNo == 0 || nNo == 2) pDY10->oUnloadPickerAir02 = FALSE;
	if (nNo == 0 || nNo == 3) pDY10->oUnloadPickerAir03 = FALSE;
	if (nNo == 0 || nNo == 4) pDY10->oUnloadPickerAir04 = FALSE;
	g_objAJinAXL.Write_Output(10);
}

void CCommon::Set_UnloadPickerDownMulti(int nNo, int nCnt)
{
	DY_DATA_10 *pDY10 = g_objAJinAXL.Get_pDY10();

	if (nNo == 1) {
		if (nCnt > 0) pDY10->oUnloadPickerDown01 = TRUE;
		if (nCnt > 1) pDY10->oUnloadPickerDown02 = TRUE;
		if (nCnt > 2) pDY10->oUnloadPickerDown03 = TRUE;
		if (nCnt > 3) pDY10->oUnloadPickerDown04 = TRUE;
	}
	else if (nNo == 2) {
		if (nCnt > 0) pDY10->oUnloadPickerDown02 = TRUE;
		if (nCnt > 1) pDY10->oUnloadPickerDown03 = TRUE;
		if (nCnt > 2) pDY10->oUnloadPickerDown04 = TRUE;
	}
	else if (nNo == 3) {
		if (nCnt > 0) pDY10->oUnloadPickerDown03 = TRUE;
		if (nCnt > 1) pDY10->oUnloadPickerDown04 = TRUE;
	}
	else if (nNo == 4) {
		if (nCnt > 0) pDY10->oUnloadPickerDown04 = TRUE;
	}

	g_objAJinAXL.Write_Output(10);
}

BOOL CCommon::Get_UnloadPickerDownMulti(int nNo, int nCnt)
{
	DX_DATA_10 *pDX10 = g_objAJinAXL.Get_pDX10();

	if (nNo == 1) {
		if (nCnt > 0 && (pDX10->iUnloadPickerUp01 || !pDX10->iUnloadPickerDown01)) return FALSE;
		if (nCnt > 1 && (pDX10->iUnloadPickerUp02 || !pDX10->iUnloadPickerDown02)) return FALSE;
		if (nCnt > 2 && (pDX10->iUnloadPickerUp03 || !pDX10->iUnloadPickerDown03)) return FALSE;
		if (nCnt > 3 && (pDX10->iUnloadPickerUp04 || !pDX10->iUnloadPickerDown04)) return FALSE;
	}
	else if (nNo == 2) {
		if (nCnt > 0 && (pDX10->iUnloadPickerUp02 || !pDX10->iUnloadPickerDown02)) return FALSE;
		if (nCnt > 1 && (pDX10->iUnloadPickerUp03 || !pDX10->iUnloadPickerDown03)) return FALSE;
		if (nCnt > 2 && (pDX10->iUnloadPickerUp04 || !pDX10->iUnloadPickerDown04)) return FALSE;
	}
	else if (nNo == 3) {
		if (nCnt > 0 && (pDX10->iUnloadPickerUp03 || !pDX10->iUnloadPickerDown03)) return FALSE;
		if (nCnt > 1 && (pDX10->iUnloadPickerUp04 || !pDX10->iUnloadPickerDown04)) return FALSE;
	}
	else if (nNo == 4) {
		if (nCnt > 0 && (pDX10->iUnloadPickerUp04 || !pDX10->iUnloadPickerDown04)) return FALSE;
	}

	return TRUE;
}

void CCommon::Set_UnloadPickerVacOffMulti(int nNo, int nCnt)
{
#ifdef DRY_RUN_TEST
	return;
#endif
	DY_DATA_10 *pDY10 = g_objAJinAXL.Get_pDY10();

	if (nNo == 1) {
		if (nCnt > 0) { pDY10->oUnloadPickerVac01 = FALSE; pDY10->oUnloadPickerAir01 = TRUE; }
		if (nCnt > 1) { pDY10->oUnloadPickerVac02 = FALSE; pDY10->oUnloadPickerAir02 = TRUE; }
		if (nCnt > 2) { pDY10->oUnloadPickerVac03 = FALSE; pDY10->oUnloadPickerAir03 = TRUE; }
		if (nCnt > 3) { pDY10->oUnloadPickerVac04 = FALSE; pDY10->oUnloadPickerAir04 = TRUE; }
	}
	else if (nNo == 2) {
		if (nCnt > 0) { pDY10->oUnloadPickerVac02 = FALSE; pDY10->oUnloadPickerAir02 = TRUE; }
		if (nCnt > 1) { pDY10->oUnloadPickerVac03 = FALSE; pDY10->oUnloadPickerAir03 = TRUE; }
		if (nCnt > 2) { pDY10->oUnloadPickerVac04 = FALSE; pDY10->oUnloadPickerAir04 = TRUE; }
	}
	else if (nNo == 3) {
		if (nCnt > 0) { pDY10->oUnloadPickerVac03 = FALSE; pDY10->oUnloadPickerAir03 = TRUE; }
		if (nCnt > 1) { pDY10->oUnloadPickerVac04 = FALSE; pDY10->oUnloadPickerAir04 = TRUE; }
	}
	else if (nNo == 4) {
		if (nCnt > 0) { pDY10->oUnloadPickerVac04 = FALSE; pDY10->oUnloadPickerAir04 = TRUE; }
	}

	g_objAJinAXL.Write_Output(10);
}

BOOL CCommon::Get_UnloadPickerVacOffMulti(int nNo, int nCnt)
{
	DX_DATA_10 *pDX10 = g_objAJinAXL.Get_pDX10();

	if (nNo == 1) {
		if (nCnt > 0 && pDX10->iUnloadPickerVac01) return FALSE;
		if (nCnt > 1 && pDX10->iUnloadPickerVac02) return FALSE;
		if (nCnt > 2 && pDX10->iUnloadPickerVac03) return FALSE;
		if (nCnt > 3 && pDX10->iUnloadPickerVac04) return FALSE;
	}
	else if (nNo == 2) {
		if (nCnt > 0 && pDX10->iUnloadPickerVac02) return FALSE;
		if (nCnt > 1 && pDX10->iUnloadPickerVac03) return FALSE;
		if (nCnt > 2 && pDX10->iUnloadPickerVac04) return FALSE;
	}
	else if (nNo == 3) {
		if (nCnt > 0 && pDX10->iUnloadPickerVac03) return FALSE;
		if (nCnt > 1 && pDX10->iUnloadPickerVac04) return FALSE;
	}
	else if (nNo == 4) {
		if (nCnt > 0 && pDX10->iUnloadPickerVac04) return FALSE;
	}

	return TRUE;
}

void CCommon::Set_UnloadPickerAirOffMulti(int nNo, int nCnt)
{
	DY_DATA_10 *pDY10 = g_objAJinAXL.Get_pDY10();

	if (nNo == 1) {
		if (nCnt > 0) pDY10->oUnloadPickerAir01 = FALSE;
		if (nCnt > 1) pDY10->oUnloadPickerAir02 = FALSE;
		if (nCnt > 2) pDY10->oUnloadPickerAir03 = FALSE;
		if (nCnt > 3) pDY10->oUnloadPickerAir04 = FALSE;
	}
	else if (nNo == 2) {
		if (nCnt > 0) pDY10->oUnloadPickerAir02 = FALSE;
		if (nCnt > 1) pDY10->oUnloadPickerAir03 = FALSE;
		if (nCnt > 2) pDY10->oUnloadPickerAir04 = FALSE;
	}
	else if (nNo == 3) {
		if (nCnt > 0) pDY10->oUnloadPickerAir03 = FALSE;
		if (nCnt > 1) pDY10->oUnloadPickerAir04 = FALSE;
	}
	else if (nNo == 4) {
		if (nCnt > 0) pDY10->oUnloadPickerAir04 = FALSE;
	}

	g_objAJinAXL.Write_Output(10);
}

// 	11. Unload Port I/O 함수
void CCommon::Set_UnloadPortSupportIn(int nNo)
{
	DY_DATA_03 *pDY03 = g_objAJinAXL.Get_pDY03();
	if (nNo == 1) { pDY03->oUnloadPort1SupportIn = TRUE; pDY03->oUnloadPort1SupportOut = FALSE; }
	if (nNo == 2) { pDY03->oUnloadPort2SupportIn = TRUE; pDY03->oUnloadPort2SupportOut = FALSE; }
	g_objAJinAXL.Write_Output(3);
}

BOOL CCommon::Get_UnloadPortSupportIn(int nNo)
{
	DX_DATA_03 *pDX03 = g_objAJinAXL.Get_pDX03();
	if (nNo == 1) {
		if (!pDX03->iUnloadPort1Support1In || pDX03->iUnloadPort1Support1Out) return FALSE;
		if (!pDX03->iUnloadPort1Support2In || pDX03->iUnloadPort1Support2Out) return FALSE;
	} else if (nNo == 2) {
		if (!pDX03->iUnloadPort2Support1In || pDX03->iUnloadPort2Support1Out) return FALSE;
		if (!pDX03->iUnloadPort2Support2In || pDX03->iUnloadPort2Support2Out) return FALSE;
	} 
	return TRUE;
}

void CCommon::Set_UnloadPortSupportOut(int nNo)
{
	DY_DATA_03 *pDY03 = g_objAJinAXL.Get_pDY03();
	if (nNo == 1) { pDY03->oUnloadPort1SupportIn = FALSE; pDY03->oUnloadPort1SupportOut = TRUE; }
	if (nNo == 2) { pDY03->oUnloadPort2SupportIn = FALSE; pDY03->oUnloadPort2SupportOut = TRUE; }
	g_objAJinAXL.Write_Output(3);
}

BOOL CCommon::Get_UnloadPortSupportOut(int nNo)
{
	DX_DATA_03 *pDX03 = g_objAJinAXL.Get_pDX03();
	if (nNo == 1) {
		if (pDX03->iUnloadPort1Support1In || !pDX03->iUnloadPort1Support1Out) return FALSE;
		if (pDX03->iUnloadPort1Support2In || !pDX03->iUnloadPort1Support2Out) return FALSE;
	} else if (nNo == 2) {
		if (pDX03->iUnloadPort2Support1In || !pDX03->iUnloadPort2Support1Out) return FALSE;
		if (pDX03->iUnloadPort2Support2In || !pDX03->iUnloadPort2Support2Out) return FALSE;
	} 
	return TRUE;
}

//  12. Tray I/O 함수
BOOL CCommon::Get_LoadStageMasterSlaveIn(int nNo)
{
	DX_DATA_04 *pDX04 = g_objAJinAXL.Get_pDX04();

	if (nNo == 1) {
		if (!pDX04->iLoadStage1MasterIn || pDX04->iLoadStage1MasterOut) return FALSE;
		if (!pDX04->iLoadStage1SlaveIn  || pDX04->iLoadStage1SlaveOut) return FALSE;
	} else if (nNo == 2) {
		if (!pDX04->iLoadStage2MasterIn || pDX04->iLoadStage2MasterOut) return FALSE;
		if (!pDX04->iLoadStage2SlaveIn  || pDX04->iLoadStage2SlaveOut) return FALSE;
	}
	return TRUE;
}

BOOL CCommon::Get_LoadStageMasterSlaveOut(int nNo)
{
	DX_DATA_04 *pDX04 = g_objAJinAXL.Get_pDX04();

	if (nNo == 1) {
		if (pDX04->iLoadStage1MasterIn || !pDX04->iLoadStage1MasterOut) return FALSE;
		if (pDX04->iLoadStage1SlaveIn  || !pDX04->iLoadStage1SlaveOut) return FALSE;
	} else if (nNo == 2) {
		if (pDX04->iLoadStage2MasterIn || !pDX04->iLoadStage2MasterOut) return FALSE;
		if (pDX04->iLoadStage2SlaveIn  || !pDX04->iLoadStage2SlaveOut) return FALSE;
	}
	return TRUE;
}

BOOL CCommon::Get_CapStageMasterSlaveIn(int nNo)
{
	DX_DATA_04 *pDX04 = g_objAJinAXL.Get_pDX04();

	if (nNo == 1) {
		if (!pDX04->iCapStage1MasterIn || pDX04->iCapStage1MasterOut) return FALSE;
		if (!pDX04->iCapStage1SlaveIn  || pDX04->iCapStage1SlaveOut) return FALSE;
	} else if (nNo == 2) {
		if (!pDX04->iCapStage2MasterIn || pDX04->iCapStage2MasterOut) return FALSE;
		if (!pDX04->iCapStage2SlaveIn  || pDX04->iCapStage2SlaveOut) return FALSE;
	}
	return TRUE;
}

BOOL CCommon::Get_CapStageMasterSlaveOut(int nNo)
{
	DX_DATA_04 *pDX04 = g_objAJinAXL.Get_pDX04();

	if (nNo == 1) {
		if (pDX04->iCapStage1MasterIn || !pDX04->iCapStage1MasterOut) return FALSE;
		if (pDX04->iCapStage1SlaveIn  || !pDX04->iCapStage1SlaveOut) return FALSE;
	} else if (nNo == 2) {
		if (pDX04->iCapStage2MasterIn || !pDX04->iCapStage2MasterOut) return FALSE;
		if (pDX04->iCapStage2SlaveIn  || !pDX04->iCapStage2SlaveOut) return FALSE;
	}
	return TRUE;
}

BOOL CCommon::Get_UnloadStageMasterSlaveIn(int nNo)
{
	DX_DATA_05 *pDX05 = g_objAJinAXL.Get_pDX05();

	if (nNo == 1) {
		if (!pDX05->iUnloadStage1MasterIn || pDX05->iUnloadStage1MasterOut) return FALSE;
		if (!pDX05->iUnloadStage1SlaveIn  || pDX05->iUnloadStage1SlaveOut) return FALSE;
	} else if (nNo == 2) {
		if (!pDX05->iUnloadStage2MasterIn || pDX05->iUnloadStage2MasterOut) return FALSE;
		if (!pDX05->iUnloadStage2SlaveIn  || pDX05->iUnloadStage2SlaveOut) return FALSE;
	}
	return TRUE;
}

BOOL CCommon::Get_UnloadStageMasterSlaveOut(int nNo)
{
	DX_DATA_05 *pDX05 = g_objAJinAXL.Get_pDX05();

	if (nNo == 1) {
		if (pDX05->iUnloadStage1MasterIn || !pDX05->iUnloadStage1MasterOut) return FALSE;
		if (pDX05->iUnloadStage1SlaveIn  || !pDX05->iUnloadStage1SlaveOut) return FALSE;
	} else if (nNo == 2) {
		if (pDX05->iUnloadStage2MasterIn || !pDX05->iUnloadStage2MasterOut) return FALSE;
		if (pDX05->iUnloadStage2SlaveIn  || !pDX05->iUnloadStage2SlaveOut) return FALSE;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
// 1. Info Load Picker I/O 함수
void CCommon::Set_InfoLoadPickerDown()
{
	DY_DATA_06 *pDY06 = g_objAJinAXL.Get_pDY06();
	int *pInfo = gData.InfoLoadPick;

	if (pInfo[0] > 0) pDY06->oLoadPickerDown01 = TRUE;
	if (pInfo[1] > 0) pDY06->oLoadPickerDown02 = TRUE;
	if (pInfo[2] > 0) pDY06->oLoadPickerDown03 = TRUE;
	if (pInfo[3] > 0) pDY06->oLoadPickerDown04 = TRUE;

	g_objAJinAXL.Write_Output(6);
}

BOOL CCommon::Get_InfoLoadPickerDown()
{
	DX_DATA_06 *pDX06 = g_objAJinAXL.Get_pDX06();
	int *pInfo = gData.InfoLoadPick;

	if (((!pDX06->iLoadPickerUp01 && pDX06->iLoadPickerDown01 && pInfo[0] > 0 ) || pInfo[0] == 0) &&
		((!pDX06->iLoadPickerUp02 && pDX06->iLoadPickerDown02 && pInfo[1] > 0 ) || pInfo[1] == 0) &&
		((!pDX06->iLoadPickerUp03 && pDX06->iLoadPickerDown03 && pInfo[2] > 0 ) || pInfo[2] == 0) &&
		((!pDX06->iLoadPickerUp04 && pDX06->iLoadPickerDown04 && pInfo[3] > 0 ) || pInfo[3] == 0)) return TRUE;

	return FALSE;
}

void CCommon::Set_InfoLoadPickerGrip()
{
	DY_DATA_06 *pDY06 = g_objAJinAXL.Get_pDY06();
	DX_DATA_06 *pDX06 = g_objAJinAXL.Get_pDX06();

	int *pInfo = gData.InfoLoadPick;

	if (pInfo[0] > 0) pDY06->oLoadPickerGrip01 = TRUE;
	if (pInfo[1] > 0) pDY06->oLoadPickerGrip02 = TRUE;
	if (pInfo[2] > 0) pDY06->oLoadPickerGrip03 = TRUE;
	if (pInfo[3] > 0) pDY06->oLoadPickerGrip04 = TRUE;

#ifndef AJIN_BOARD_USE
	if (pInfo[0] > 0) pDX06->iLoadPickerCMCheck01 = TRUE;
	if (pInfo[1] > 0) pDX06->iLoadPickerCMCheck02 = TRUE;
	if (pInfo[2] > 0) pDX06->iLoadPickerCMCheck03 = TRUE;
	if (pInfo[3] > 0) pDX06->iLoadPickerCMCheck04 = TRUE;
#endif 
	
	g_objAJinAXL.Write_Output(6);
}

BOOL CCommon::Get_InfoLoadPickerGrip()
{
	DX_DATA_06 *pDX06 = g_objAJinAXL.Get_pDX06();
	int *pInfo = gData.InfoLoadPick;

	if (((pInfo[0] > 0 && !pDX06->iLoadPickerOpen01) || pInfo[0] == 0) &&
		((pInfo[1] > 0 && !pDX06->iLoadPickerOpen02) || pInfo[1] == 0) &&
		((pInfo[2] > 0 && !pDX06->iLoadPickerOpen03) || pInfo[2] == 0) &&
		((pInfo[3] > 0 && !pDX06->iLoadPickerOpen04) || pInfo[3] == 0)) return TRUE;

	return FALSE;
}

BOOL CCommon::Get_InfoLoadPickerGripOpen()
{
	DX_DATA_06 *pDX06 = g_objAJinAXL.Get_pDX06();
	int *pInfo = gData.InfoLoadPick;

	if (((pInfo[0] == 0 && pDX06->iLoadPickerOpen01) || pInfo[0] > 0) &&
		((pInfo[1] == 0 && pDX06->iLoadPickerOpen02) || pInfo[1] > 0) &&
		((pInfo[2] == 0 && pDX06->iLoadPickerOpen03) || pInfo[2] > 0) &&
		((pInfo[3] == 0 && pDX06->iLoadPickerOpen04) || pInfo[3] > 0)) return TRUE;

	return FALSE;
}

BOOL CCommon::Get_InfoLoadPickerCmCheck()
{
#ifdef DRY_RUN_TEST
	return TRUE;
#endif
	DX_DATA_06 *pDX06 = g_objAJinAXL.Get_pDX06();
	int *pInfo = gData.InfoLoadPick;

	if (((pInfo[0] > 0 && pDX06->iLoadPickerCMCheck01) || pInfo[0] == 0) &&
		((pInfo[1] > 0 && pDX06->iLoadPickerCMCheck02) || pInfo[1] == 0) &&
		((pInfo[2] > 0 && pDX06->iLoadPickerCMCheck03) || pInfo[2] == 0) &&
		((pInfo[3] > 0 && pDX06->iLoadPickerCMCheck04) || pInfo[3] == 0)) return TRUE;

	return FALSE;
}

int CCommon::Get_InfoLoadPickerDownCnt(int nTrayX, int nTrayY, int nCnt)
{
	int nCount = 0;
	int *pInfo = Get_LoadTrayInfo(nTrayY);

	for (int i = nTrayX; i < nTrayX + nCnt; i++) { if (pInfo[i] > 0) nCount++; }

	return nCount;
}

// 2. Info Main Index I/O 함수
BOOL CCommon::Get_InfoIndexLoadVacuumOn()
{
#ifdef DRY_RUN_TEST
	return TRUE;	// 테스트
#endif
	DX_DATA_11 *pDX11 = g_objAJinAXL.Get_pDX11();
	int *pInfo = gData.InfoIndex[0];

	if (((pDX11->iIndexLoadVac01 && pInfo[0] > 0) || pInfo[0] == 0) &&
		((pDX11->iIndexLoadVac02 && pInfo[1] > 0) || pInfo[1] == 0) &&
		((pDX11->iIndexLoadVac03 && pInfo[2] > 0) || pInfo[2] == 0) &&
		((pDX11->iIndexLoadVac04 && pInfo[3] > 0) || pInfo[3] == 0)) return TRUE;

	return FALSE;
}

void CCommon::Set_InfoIndexLoadVacuumOn()
{
#ifdef DRY_RUN_TEST
	return;		// 테스트
#endif
	DY_DATA_11 *pDY11 = g_objAJinAXL.Get_pDY11();
	int *pInfo = gData.InfoIndex[0];

	if (pInfo[0] > 0) pDY11->oIndexLoadVac01 = TRUE;
	if (pInfo[1] > 0) pDY11->oIndexLoadVac02 = TRUE;
	if (pInfo[2] > 0) pDY11->oIndexLoadVac03 = TRUE;
	if (pInfo[3] > 0) pDY11->oIndexLoadVac04 = TRUE;

	g_objAJinAXL.Write_Output(11);
}

BOOL CCommon::Get_InfoIndexAssyVacuumOn()
{
#ifdef DRY_RUN_TEST
	return TRUE;	// 테스트
#endif
	DX_DATA_11 *pDX11 = g_objAJinAXL.Get_pDX11();
	int *pInfo = gData.InfoIndex[1];

	if (((pDX11->iIndexAssyVac01 && pInfo[0] > 0) || pInfo[0] == 0) &&
		((pDX11->iIndexAssyVac02 && pInfo[1] > 0) || pInfo[1] == 0) &&
		((pDX11->iIndexAssyVac03 && pInfo[2] > 0) || pInfo[2] == 0) &&
		((pDX11->iIndexAssyVac04 && pInfo[3] > 0) || pInfo[3] == 0)) return TRUE;

	return FALSE;
}

void CCommon::Set_InfoIndexAssyVacuumOn()
{
#ifdef DRY_RUN_TEST
	return;		// 테스트
#endif
	DY_DATA_11 *pDY11 = g_objAJinAXL.Get_pDY11();
	int *pInfo = gData.InfoIndex[1];

	if (pInfo[0] > 0) pDY11->oIndexAssyVac01 = TRUE;
	if (pInfo[1] > 0) pDY11->oIndexAssyVac02 = TRUE;
	if (pInfo[2] > 0) pDY11->oIndexAssyVac03 = TRUE;
	if (pInfo[3] > 0) pDY11->oIndexAssyVac04 = TRUE;

	g_objAJinAXL.Write_Output(11);
}

int CCommon::Get_MainIndexPos(int nPos)
{
	DX_DATA_11 *pDX11 = g_objAJinAXL.Get_pDX11();
	int nIndexPos = pDX11->iIndexPosition0 + (pDX11->iIndexPosition1 << 1);
	int nNo = 0;

	// Load
	if (nPos == 1) 
	{	
		if (nIndexPos == 0) nNo = 0;
		if (nIndexPos == 1) nNo = 1;
		if (nIndexPos == 2) nNo = 2;
		if (nIndexPos == 3) nNo = 3;

	} 
	else if (nPos == 2) // Assy
	{	
		if (nIndexPos == 0) nNo = 3;
		if (nIndexPos == 1) nNo = 0;
		if (nIndexPos == 2) nNo = 1;
		if (nIndexPos == 3) nNo = 2;

	}
	else if (nPos == 3) // Trans 
	{	
		if (nIndexPos == 0) nNo = 2;
		if (nIndexPos == 1) nNo = 3;
		if (nIndexPos == 2) nNo = 0;
		if (nIndexPos == 3) nNo = 1;
	}

	return nNo;
}

// 3. Info Cap Picker I/O 함수
void CCommon::Set_InfoCapPickerDown()
{
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();
	DY_DATA_07 *pDY07 = g_objAJinAXL.Get_pDY07();
	int *pInfo = gData.InfoCapBuffer;

	if (pInfo[0] == 0) pDY07->oCapPickerDown01 = TRUE;
	if (pInfo[1] == 0) pDY07->oCapPickerDown02 = TRUE;
	if (pInfo[2] == 0) pDY07->oCapPickerDown03 = TRUE;
	if (pInfo[3] == 0) pDY07->oCapPickerDown04 = TRUE;

	g_objAJinAXL.Write_Output(7);
}

BOOL CCommon::Get_InfoCapPickerDown()
{
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();
	DX_DATA_07 *pDX07 = g_objAJinAXL.Get_pDX07();
	int *pInfo = gData.InfoCapBuffer;

	if (((pDX07->iCapPickerUp01 || !pDX07->iCapPickerDown01) && pInfo[0] == 0 ) || ((!pDX07->iCapPickerUp01 || pDX07->iCapPickerDown01) && pInfo[0] > 0)) return FALSE;
	if (((pDX07->iCapPickerUp02 || !pDX07->iCapPickerDown02) && pInfo[1] == 0 ) || ((!pDX07->iCapPickerUp02 || pDX07->iCapPickerDown02) && pInfo[1] > 0)) return FALSE;
	if (((pDX07->iCapPickerUp03 || !pDX07->iCapPickerDown03) && pInfo[2] == 0 ) || ((!pDX07->iCapPickerUp03 || pDX07->iCapPickerDown03) && pInfo[2] > 0)) return FALSE;
	if (((pDX07->iCapPickerUp04 || !pDX07->iCapPickerDown04) && pInfo[3] == 0 ) || ((!pDX07->iCapPickerUp04 || pDX07->iCapPickerDown04) && pInfo[3] > 0)) return FALSE;

	return TRUE;
}

BOOL CCommon::Get_InfoCapPickerVacOn()
{
#ifdef DRY_RUN_TEST
	return TRUE;
#endif
	DX_DATA_07 *pDX07 = g_objAJinAXL.Get_pDX07();
	int *pInfo = gData.InfoCapPick;

	if (((pDX07->iCapPickerVac01 && pInfo[0] > 0 ) || pInfo[0] == 0) &&
		((pDX07->iCapPickerVac02 && pInfo[1] > 0 ) || pInfo[1] == 0) &&
		((pDX07->iCapPickerVac03 && pInfo[2] > 0 ) || pInfo[2] == 0) &&
		((pDX07->iCapPickerVac04 && pInfo[3] > 0 ) || pInfo[3] == 0)) return TRUE;

	return FALSE;
}

void CCommon::Set_InfoCapPickerVacOff()
{
#ifdef DRY_RUN_TEST
	return;		// 테스트
#endif
	DY_DATA_07 *pDY07 = g_objAJinAXL.Get_pDY07();
	int *pInfo = gData.InfoCapPick;

	if (pInfo[0] == 0) { pDY07->oCapPickerVac01 = FALSE; pDY07->oCapPickerAir01 = TRUE; }
	if (pInfo[1] == 0) { pDY07->oCapPickerVac02 = FALSE; pDY07->oCapPickerAir02 = TRUE; }
	if (pInfo[2] == 0) { pDY07->oCapPickerVac03 = FALSE; pDY07->oCapPickerAir03 = TRUE; }
	if (pInfo[3] == 0) { pDY07->oCapPickerVac04 = FALSE; pDY07->oCapPickerAir04 = TRUE; }

	g_objAJinAXL.Write_Output(7);
}

BOOL CCommon::Get_InfoCapPickerVacOff()
{
#ifdef DRY_RUN_TEST
	return TRUE;
#endif
	DX_DATA_07 *pDX07 = g_objAJinAXL.Get_pDX07();
	int *pInfo = gData.InfoCapPick;

	if (((!pDX07->iCapPickerVac01 && pInfo[0] == 0 ) || pInfo[0] > 0) &&
		((!pDX07->iCapPickerVac02 && pInfo[1] == 0 ) || pInfo[1] > 0) &&
		((!pDX07->iCapPickerVac03 && pInfo[2] == 0 ) || pInfo[2] > 0) &&
		((!pDX07->iCapPickerVac04 && pInfo[3] == 0 ) || pInfo[3] > 0)) return TRUE;

	return FALSE;
}

void CCommon::Set_InfoCapPickerAirOff()
{
	DY_DATA_07 *pDY07 = g_objAJinAXL.Get_pDY07();
	int *pInfo = gData.InfoCapPick;

	if (pInfo[0] == 0) { pDY07->oCapPickerAir01 = FALSE; }
	if (pInfo[1] == 0) { pDY07->oCapPickerAir02 = FALSE; }
	if (pInfo[2] == 0) { pDY07->oCapPickerAir03 = FALSE; }
	if (pInfo[3] == 0) { pDY07->oCapPickerAir04 = FALSE; }

	g_objAJinAXL.Write_Output(7);
}

void CCommon::Set_InfoCapPickerNormal()
{
	DY_DATA_07 *pDY07 = g_objAJinAXL.Get_pDY07();
	int *pInfo = gData.InfoCapPick;

	if (pInfo[0] == 0) pDY07->oCapPickerTurn01 = FALSE;
	if (pInfo[1] == 0) pDY07->oCapPickerTurn02 = FALSE;
	if (pInfo[2] == 0) pDY07->oCapPickerTurn03 = FALSE;
	if (pInfo[3] == 0) pDY07->oCapPickerTurn04 = FALSE;
	g_objAJinAXL.Write_Output(7);
}

BOOL CCommon::Get_InfoCapPickerNormal()
{
	DX_DATA_07 *pDX07 = g_objAJinAXL.Get_pDX07();
	int *pInfo = gData.InfoCapPick;

	if (((!pDX07->iCapPickerTurn01 && pInfo[0] == 0 ) || pInfo[0] > 0) &&
		((!pDX07->iCapPickerTurn02 && pInfo[1] == 0 ) || pInfo[1] > 0) &&
		((!pDX07->iCapPickerTurn03 && pInfo[2] == 0 ) || pInfo[2] > 0) &&
		((!pDX07->iCapPickerTurn04 && pInfo[3] == 0 ) || pInfo[3] > 0)) return TRUE;

	return FALSE;
}

// 4. Info Assy Picker I/O 함수
void CCommon::Set_InfoAssyPickerDown(int nPos)
{
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();
	DY_DATA_08 *pDY08 = g_objAJinAXL.Get_pDY08();
	int *pInfo = NULL;

	if (nPos == 0) {
		pInfo = gData.InfoAssyPick;

		if (pInfo[0] == 0) pDY08->oAssyPickerDown01 = TRUE;
		if (pInfo[1] == 0) pDY08->oAssyPickerDown02 = TRUE;
		if (pInfo[2] == 0) pDY08->oAssyPickerDown03 = TRUE;
		if (pInfo[3] == 0) pDY08->oAssyPickerDown04 = TRUE;
	}
	if (nPos == 1) {
		pInfo = gData.InfoIndex[1];

		if (pInfo[0] > 0) pDY08->oAssyPickerDown01 = TRUE;
		if (pInfo[1] > 0) pDY08->oAssyPickerDown02 = TRUE;
		if (pInfo[2] > 0) pDY08->oAssyPickerDown03 = TRUE;
		if (pInfo[3] > 0) pDY08->oAssyPickerDown04 = TRUE;
	}
	g_objAJinAXL.Write_Output(8);
}

BOOL CCommon::Get_InfoAssyPickerDown(int nPos)
{
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();
	DX_DATA_08 *pDX08 = g_objAJinAXL.Get_pDX08();
	int *pInfo = NULL;

	if (nPos == 0) {
		pInfo = gData.InfoAssyPick;
		
		if (((pDX08->iAssyPickerUp01 || !pDX08->iAssyPickerDown01) && pInfo[0] == 0 ) || ((!pDX08->iAssyPickerUp01 || pDX08->iAssyPickerDown01) && pInfo[0] > 0)) return FALSE;
		if (((pDX08->iAssyPickerUp02 || !pDX08->iAssyPickerDown02) && pInfo[1] == 0 ) || ((!pDX08->iAssyPickerUp02 || pDX08->iAssyPickerDown02) && pInfo[1] > 0)) return FALSE;
		if (((pDX08->iAssyPickerUp03 || !pDX08->iAssyPickerDown03) && pInfo[2] == 0 ) || ((!pDX08->iAssyPickerUp03 || pDX08->iAssyPickerDown03) && pInfo[2] > 0)) return FALSE;
		if (((pDX08->iAssyPickerUp04 || !pDX08->iAssyPickerDown04) && pInfo[3] == 0 ) || ((!pDX08->iAssyPickerUp04 || pDX08->iAssyPickerDown04) && pInfo[3] > 0)) return FALSE;
		return TRUE;
	}
	if (nPos == 1) {
		pInfo = gData.InfoIndex[1];

		if (((!pDX08->iAssyPickerUp01 && pDX08->iAssyPickerDown01 && pInfo[0] > 0 ) || pInfo[0] == 0) &&
			((!pDX08->iAssyPickerUp02 && pDX08->iAssyPickerDown02 && pInfo[1] > 0 ) || pInfo[1] == 0) &&
			((!pDX08->iAssyPickerUp03 && pDX08->iAssyPickerDown03 && pInfo[2] > 0 ) || pInfo[2] == 0) &&
			((!pDX08->iAssyPickerUp04 && pDX08->iAssyPickerDown04 && pInfo[3] > 0 ) || pInfo[3] == 0)) return TRUE;
	}

	return FALSE;
}

void CCommon::Set_InfoAssyPickerGoodDown()
{
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();
	DY_DATA_08 *pDY08 = g_objAJinAXL.Get_pDY08();

	if (gData.InfoIndex[1][0] > 0 && gData.InfoAssyPick[0] == 9) pDY08->oAssyPickerDown01 = TRUE;
	if (gData.InfoIndex[1][1] > 0 && gData.InfoAssyPick[1] == 9) pDY08->oAssyPickerDown02 = TRUE;
	if (gData.InfoIndex[1][2] > 0 && gData.InfoAssyPick[2] == 9) pDY08->oAssyPickerDown03 = TRUE;
	if (gData.InfoIndex[1][3] > 0 && gData.InfoAssyPick[3] == 9) pDY08->oAssyPickerDown04 = TRUE;

	g_objAJinAXL.Write_Output(8);
}

BOOL CCommon::Get_InfoAssyPickerGoodDown()
{
	EQUIP_DATA *pEquipData = g_objDataManager.Get_pEquipData();
	DX_DATA_08 *pDX08 = g_objAJinAXL.Get_pDX08();

	if (((gData.InfoIndex[1][0] > 0 && gData.InfoAssyPick[0] == 9 && !pDX08->iAssyPickerUp01 && pDX08->iAssyPickerDown01) || (gData.InfoIndex[1][0] == 0 || gData.InfoAssyPick[0] == 2)) &&
		((gData.InfoIndex[1][1] > 0 && gData.InfoAssyPick[1] == 9 && !pDX08->iAssyPickerUp02 && pDX08->iAssyPickerDown02) || (gData.InfoIndex[1][1] == 0 || gData.InfoAssyPick[1] == 2)) &&
		((gData.InfoIndex[1][2] > 0 && gData.InfoAssyPick[2] == 9 && !pDX08->iAssyPickerUp03 && pDX08->iAssyPickerDown03) || (gData.InfoIndex[1][2] == 0 || gData.InfoAssyPick[2] == 2)) &&
		((gData.InfoIndex[1][3] > 0 && gData.InfoAssyPick[3] == 9 && !pDX08->iAssyPickerUp04 && pDX08->iAssyPickerDown04) || (gData.InfoIndex[1][3] == 0 || gData.InfoAssyPick[3] == 2))) return TRUE;

	return FALSE;
}

void CCommon::Set_InfoAssyPickerVacOn()
{
#ifdef DRY_RUN_TEST
	return;		// 테스트
#endif
	DY_DATA_08 *pDY08 = g_objAJinAXL.Get_pDY08();
	int *pInfo = gData.InfoAssyPick;

	if (pInfo[0] == 0) { pDY08->oAssyPickerVac01 = TRUE; }
	if (pInfo[1] == 0) { pDY08->oAssyPickerVac02 = TRUE; }
	if (pInfo[2] == 0) { pDY08->oAssyPickerVac03 = TRUE; }
	if (pInfo[3] == 0) { pDY08->oAssyPickerVac04 = TRUE; }

	g_objAJinAXL.Write_Output(8);
}

BOOL CCommon::Get_InfoAssyPickerVacOn()
{
#ifdef DRY_RUN_TEST
	return TRUE;
#endif
	DX_DATA_08 *pDX08 = g_objAJinAXL.Get_pDX08();
	int *pInfo = gData.InfoAssyPick;

	if (((pDX08->iAssyPickerVac01 && pInfo[0] > 0 ) || pInfo[0] == 0) &&
		((pDX08->iAssyPickerVac02 && pInfo[1] > 0 ) || pInfo[1] == 0) &&
		((pDX08->iAssyPickerVac03 && pInfo[2] > 0 ) || pInfo[2] == 0) &&
		((pDX08->iAssyPickerVac04 && pInfo[3] > 0 ) || pInfo[3] == 0)) return TRUE;

	return FALSE;
}

void CCommon::Set_InfoAssyPickerVacOff()
{
	DY_DATA_08 *pDY08 = g_objAJinAXL.Get_pDY08();
	int *pInfo = gData.InfoIndex[1];

	if (pInfo[0] > 0) { pDY08->oAssyPickerVac01 = FALSE; pDY08->oAssyPickerAir01 = TRUE; }
	if (pInfo[1] > 0) { pDY08->oAssyPickerVac02 = FALSE; pDY08->oAssyPickerAir02 = TRUE; }
	if (pInfo[2] > 0) { pDY08->oAssyPickerVac03 = FALSE; pDY08->oAssyPickerAir03 = TRUE; }
	if (pInfo[3] > 0) { pDY08->oAssyPickerVac04 = FALSE; pDY08->oAssyPickerAir04 = TRUE; }

	g_objAJinAXL.Write_Output(8);
}

BOOL CCommon::Get_InfoAssyPickerVacOff()
{
	DX_DATA_08 *pDX08 = g_objAJinAXL.Get_pDX08();
	int *pInfo = gData.InfoIndex[1];

	if (((!pDX08->iAssyPickerVac01 && pInfo[0] > 0 ) || pInfo[0] == 0) &&
		((!pDX08->iAssyPickerVac02 && pInfo[1] > 0 ) || pInfo[1] == 0) &&
		((!pDX08->iAssyPickerVac03 && pInfo[2] > 0 ) || pInfo[2] == 0) &&
		((!pDX08->iAssyPickerVac04 && pInfo[3] > 0 ) || pInfo[3] == 0)) return TRUE;

	return FALSE;
}

void CCommon::Set_InfoAssyPickerAirOff()
{
	DY_DATA_08 *pDY08 = g_objAJinAXL.Get_pDY08();
	int *pInfo = gData.InfoIndex[1];

	if (pInfo[0] > 0) { pDY08->oAssyPickerAir01 = FALSE; }
	if (pInfo[1] > 0) { pDY08->oAssyPickerAir02 = FALSE; }
	if (pInfo[2] > 0) { pDY08->oAssyPickerAir03 = FALSE; }
	if (pInfo[3] > 0) { pDY08->oAssyPickerAir04 = FALSE; }

	g_objAJinAXL.Write_Output(8);
}

void CCommon::Set_InfoAssyPickerGoodVacOff()
{
	DY_DATA_08 *pDY08 = g_objAJinAXL.Get_pDY08();

	if (gData.InfoIndex[1][0] > 0 && gData.InfoAssyPick[0] == 9) { pDY08->oAssyPickerVac01 = FALSE; pDY08->oAssyPickerAir01 = TRUE; }
	if (gData.InfoIndex[1][1] > 0 && gData.InfoAssyPick[1] == 9) { pDY08->oAssyPickerVac02 = FALSE; pDY08->oAssyPickerAir02 = TRUE; }
	if (gData.InfoIndex[1][2] > 0 && gData.InfoAssyPick[2] == 9) { pDY08->oAssyPickerVac03 = FALSE; pDY08->oAssyPickerAir03 = TRUE; }
	if (gData.InfoIndex[1][3] > 0 && gData.InfoAssyPick[3] == 9) { pDY08->oAssyPickerVac04 = FALSE; pDY08->oAssyPickerAir04 = TRUE; }

	g_objAJinAXL.Write_Output(8);
}

BOOL CCommon::Get_InfoAssyPickerGoodVacOff()
{
	DX_DATA_08 *pDX08 = g_objAJinAXL.Get_pDX08();

	if (((!pDX08->iAssyPickerVac01 && gData.InfoIndex[1][0] > 0 && gData.InfoAssyPick[0] == 9 ) || (gData.InfoIndex[1][0] == 0 && gData.InfoAssyPick[0] != 9)) &&
		((!pDX08->iAssyPickerVac02 && gData.InfoIndex[1][1] > 0 && gData.InfoAssyPick[1] == 9 ) || (gData.InfoIndex[1][1] == 0 && gData.InfoAssyPick[1] != 9)) &&
		((!pDX08->iAssyPickerVac03 && gData.InfoIndex[1][2] > 0 && gData.InfoAssyPick[2] == 9 ) || (gData.InfoIndex[1][2] == 0 && gData.InfoAssyPick[2] != 9)) &&
		((!pDX08->iAssyPickerVac04 && gData.InfoIndex[1][3] > 0 && gData.InfoAssyPick[3] == 9 ) || (gData.InfoIndex[1][3] == 0 && gData.InfoAssyPick[3] != 9))) return TRUE;

	return FALSE;
}

void CCommon::Set_InfoAssyPickerGoodAirOff()
{
	DY_DATA_08 *pDY08 = g_objAJinAXL.Get_pDY08();

	if (gData.InfoIndex[1][0] > 0 && gData.InfoAssyPick[0] == 9) { pDY08->oAssyPickerAir01 = FALSE; }
	if (gData.InfoIndex[1][1] > 0 && gData.InfoAssyPick[1] == 9) { pDY08->oAssyPickerAir02 = FALSE; }
	if (gData.InfoIndex[1][2] > 0 && gData.InfoAssyPick[2] == 9) { pDY08->oAssyPickerAir03 = FALSE; }
	if (gData.InfoIndex[1][3] > 0 && gData.InfoAssyPick[3] == 9) { pDY08->oAssyPickerAir04 = FALSE; }

	g_objAJinAXL.Write_Output(8);
}

// 5. Info Trans Stage I/O 함수
void CCommon::Set_InfoTransStageVacOn()
{
#ifdef DRY_RUN_TEST
	return;
#endif
	DY_DATA_05 *pDY05 = g_objAJinAXL.Get_pDY05();
	int *pInfo = gData.InfoIndex[2];

	if (pInfo[0] > 0) { pDY05->oTransStageVac01 = TRUE; }
	if (pInfo[1] > 0) { pDY05->oTransStageVac02 = TRUE; }
	if (pInfo[2] > 0) { pDY05->oTransStageVac03 = TRUE; }
	if (pInfo[3] > 0) { pDY05->oTransStageVac04 = TRUE; }

	g_objAJinAXL.Write_Output(5);
}

BOOL CCommon::Get_InfoTransStageVacOn()
{
#ifdef DRY_RUN_TEST
	return TRUE;
#endif
	DX_DATA_05 *pDX05 = g_objAJinAXL.Get_pDX05();
	int *pInfo = gData.InfoTransStage;

	if (((pDX05->iTransStageVac01 && pInfo[0] > 0) || pInfo[0] == 0) &&
		((pDX05->iTransStageVac02 && pInfo[1] > 0) || pInfo[1] == 0) &&
		((pDX05->iTransStageVac03 && pInfo[2] > 0) || pInfo[2] == 0) &&
		((pDX05->iTransStageVac04 && pInfo[3] > 0) || pInfo[3] == 0)) return TRUE;

	return FALSE;
}

// 6. Info Unload Picker I/O 함수
void CCommon::Set_InfoUnloadPickerDown()
{
	DY_DATA_10  *pDY10 = g_objAJinAXL.Get_pDY10();
	int *pInfo = gData.InfoTransStage;

	if (pInfo[0] > 0) pDY10->oUnloadPickerDown01 = TRUE;
	if (pInfo[1] > 0) pDY10->oUnloadPickerDown02 = TRUE;
	if (pInfo[2] > 0) pDY10->oUnloadPickerDown03 = TRUE;
	if (pInfo[3] > 0) pDY10->oUnloadPickerDown04 = TRUE;

	g_objAJinAXL.Write_Output(10);
}

BOOL CCommon::Get_InfoUnloadPickerDown()
{
	DX_DATA_10 *pDX10 = g_objAJinAXL.Get_pDX10();
	int *pInfo = gData.InfoTransStage;

	if (((!pDX10->iUnloadPickerUp01 && pDX10->iUnloadPickerDown01 && pInfo[0] > 0 ) || pInfo[0] == 0) &&
		((!pDX10->iUnloadPickerUp02 && pDX10->iUnloadPickerDown02 && pInfo[1] > 0 ) || pInfo[1] == 0) &&
		((!pDX10->iUnloadPickerUp03 && pDX10->iUnloadPickerDown03 && pInfo[2] > 0 ) || pInfo[2] == 0) &&
		((!pDX10->iUnloadPickerUp04 && pDX10->iUnloadPickerDown04 && pInfo[3] > 0 ) || pInfo[3] == 0)) return TRUE;

	return FALSE;
}

void CCommon::Set_InfoUnloadPickerVacOn()
{
#ifdef DRY_RUN_TEST
	return;		// 테스트
#endif
	DY_DATA_10 *pDY10 = g_objAJinAXL.Get_pDY10();
	int *pInfo = gData.InfoUnloadPick;

	if (pInfo[0] > 0) { pDY10->oUnloadPickerVac01 = TRUE; }
	if (pInfo[1] > 0) { pDY10->oUnloadPickerVac02 = TRUE; }
	if (pInfo[2] > 0) { pDY10->oUnloadPickerVac03 = TRUE; }
	if (pInfo[3] > 0) { pDY10->oUnloadPickerVac04 = TRUE; }

	g_objAJinAXL.Write_Output(10);
}

BOOL CCommon::Get_InfoUnloadPickerVacOn()
{
#ifdef DRY_RUN_TEST
	return TRUE;
#endif
	DX_DATA_10 *pDX10 = g_objAJinAXL.Get_pDX10();
	int *pInfo = gData.InfoUnloadPick;

	if (((pDX10->iUnloadPickerVac01 && pInfo[0] > 0 ) || pInfo[0] == 0) &&
		((pDX10->iUnloadPickerVac02 && pInfo[1] > 0 ) || pInfo[1] == 0) &&
		((pDX10->iUnloadPickerVac03 && pInfo[2] > 0 ) || pInfo[2] == 0) &&
		((pDX10->iUnloadPickerVac04 && pInfo[3] > 0 ) || pInfo[3] == 0)) return TRUE;

	return FALSE;
}

BOOL CCommon::Get_InfoUnloadPickerVacOff()
{
#ifdef DRY_RUN_TEST
	return TRUE;
#endif
	DX_DATA_10 *pDX10 = g_objAJinAXL.Get_pDX10();
	int *pInfo = gData.InfoUnloadPick;

	if (((!pDX10->iUnloadPickerVac01 && pInfo[0] == 0 ) || pInfo[0] > 0) &&
		((!pDX10->iUnloadPickerVac02 && pInfo[1] == 0 ) || pInfo[1] > 0) &&
		((!pDX10->iUnloadPickerVac03 && pInfo[2] == 0 ) || pInfo[2] > 0) &&
		((!pDX10->iUnloadPickerVac04 && pInfo[3] == 0 ) || pInfo[3] > 0)) return TRUE;

	return FALSE;
}

BOOL CCommon::Get_InfoUnloadPickerCapCheck()
{
#ifdef DRY_RUN_TEST
	return TRUE;
#endif

#ifndef AJIN_BOARD_USE
	return TRUE;
#endif 
	DX_DATA_10 *pDX10 = g_objAJinAXL.Get_pDX10();
	int *pInfo = gData.InfoUnloadPick;

	if (((pDX10->iUnloadPickerCapChk1 && pInfo[0] > 0 ) || pInfo[0] == 0) &&
		((pDX10->iUnloadPickerCapChk2 && pInfo[1] > 0 ) || pInfo[1] == 0) &&
		((pDX10->iUnloadPickerCapChk3 && pInfo[2] > 0 ) || pInfo[2] == 0) &&
		((pDX10->iUnloadPickerCapChk4 && pInfo[3] > 0 ) || pInfo[3] == 0)) return TRUE;

	return FALSE;
}

//7. Info Cap Clean I/O
void CCommon::Set_InfoCapCleanAirOn()
{
#ifdef DRY_RUN_TEST
	return;		// 테스트
#endif
	DY_DATA_09 *pDY09 = g_objAJinAXL.Get_pDY09();
	DX_DATA_07 *pDX07 = g_objAJinAXL.Get_pDX07();

	if (pDX07->iCapPickerDown01) pDY09->oCapCleanAir01 = TRUE;
	if (pDX07->iCapPickerDown02) pDY09->oCapCleanAir02 = TRUE;
	if (pDX07->iCapPickerDown03) pDY09->oCapCleanAir03 = TRUE;
	if (pDX07->iCapPickerDown04) pDY09->oCapCleanAir04 = TRUE;
	pDY09->oCapCleanSuction = TRUE;

	g_objAJinAXL.Write_Output(9);
}

///////////////////////////////////////////////////////////////////////////////
// Load Tray Info
int *CCommon::Get_LoadTrayInfo(int nLine)
{
	static int nInfo[LT_X] = { 0 };

	for (int i = 0; i < LT_X; i++) {
		nInfo[i] = gData.InfoLoadTray[nLine][i];
	}
	return nInfo;
}

///////////////////////////////////////////////////////////////////////////////
CString CCommon::Get_AssyPickerTiltErr()
{
	DX_DATA_09 *pDX09 = g_objAJinAXL.Get_pDX09();
	int *pInfo = gData.InfoIndex[1];
	int nPNo = gData.nPNoIndex[1] - 1;

	CString strErr, strSNo;
	strErr = "";
	if (pDX09->iAssyPickerTilt01 && pInfo[0] > 0) { 
		if (strErr == "") strErr = " #==> ";

		if (gData.bCapAlignNg[0]) strErr += "1(Align NG). ";
		else					  strErr += "1(Align Good). ";

		gLot.nCapFailCount[nPNo]++;
	}
	if (pDX09->iAssyPickerTilt02 && pInfo[1] > 0) {
		if (strErr == "") strErr = " #==> ";

		if (gData.bCapAlignNg[1]) strErr += "2(Align NG). ";
		else					  strErr += "2(Align Good). ";

		gLot.nCapFailCount[nPNo]++;
	}
	if (pDX09->iAssyPickerTilt03 && pInfo[2] > 0) {
		if (strErr == "") strErr = " #==> ";
		
		if (gData.bCapAlignNg[2]) strErr += "3(Align NG). ";
		else					  strErr += "3(Align Good). ";

		gLot.nCapFailCount[nPNo]++;
	}
	if (pDX09->iAssyPickerTilt04 && pInfo[3] > 0) {
		if (strErr == "") strErr = " #==> ";

		if (gData.bCapAlignNg[3]) strErr += "4(Align NG). ";
		else					  strErr += "4(Align Good). ";

		gLot.nCapFailCount[nPNo]++;
	}

	if (strErr != "") {
		strErr += "번 Picker Tilt를 확인 하여 주십시오.";
	}
	return strErr;
}

CString CCommon::Get_AssyPickerForceErr()
{
	DX_DATA_08 *pDX08 = g_objAJinAXL.Get_pDX08();
	int *pInfo = gData.InfoIndex[1];
	int nPNo = gData.nPNoIndex[1] - 1;

	CString strErr, strData;
	strErr = "";
	if (pDX08->iAssyPickerDown01) { 
		if (strErr == "") strErr = " #==> ";
		strData.Format("1번 Picker : %0.3lf kg", gData.dLoadCell[0]);
		strErr += strData;
	
	} else if (pDX08->iAssyPickerDown02) { 
		if (strErr == "") strErr = " #==> ";
		strData.Format("2번 Picker : %0.3lf kg", gData.dLoadCell[0]);
		strErr += strData;

	} else if (pDX08->iAssyPickerDown03) { 
		if (strErr == "") strErr = " #==> ";
		strData.Format("3번 Picker : %0.3lf kg", gData.dLoadCell[0]);
		strErr += strData;

	} else if (pDX08->iAssyPickerDown04) { 
		if (strErr == "") strErr = " #==> ";
		strData.Format("4번 Picker : %0.3lf kg", gData.dLoadCell[0]);
		strErr += strData;
	}	

	if (strErr != "") {
		strErr += "Load Cell 측정값 확인 하여 주십시오.";
	}
	return strErr;
}

CString CCommon::Get_LoadPickerCheckErr()
{
	DX_DATA_06 *pDX06 = g_objAJinAXL.Get_pDX06();
	int *pInfo = gData.InfoLoadPick;

	CString strErr, strSNo;
	strErr = "";
	if (!pDX06->iLoadPickerCMCheck01 && pInfo[0] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "1. "; }
	if (!pDX06->iLoadPickerCMCheck02 && pInfo[1] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "2. "; }
	if (!pDX06->iLoadPickerCMCheck03 && pInfo[2] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "3. "; }
	if (!pDX06->iLoadPickerCMCheck04 && pInfo[3] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "4. "; }
	
	if (strErr != "") {
		strErr += "번 Load Picker CM Check 센서를 확인 하여 주십시오.";
	}
	return strErr;
}


CString CCommon::Get_LoadIndexVacOnCheckErr()
{
	DX_DATA_11 *pDX11 = g_objAJinAXL.Get_pDX11();
	int *pInfo = gData.InfoIndex[0];

	CString strErr, strSNo;
	strErr = "";
	if (!pDX11->iIndexLoadVac01 && pInfo[0] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "1. "; }
	if (!pDX11->iIndexLoadVac02 && pInfo[1] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "2. "; }
	if (!pDX11->iIndexLoadVac03 && pInfo[2] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "3. "; }
	if (!pDX11->iIndexLoadVac04 && pInfo[3] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "4. "; }

	if (strErr != "") {
		strErr += "번 Load Vac 센서를 확인 하여 주십시오.";
	}
	return strErr;
}

CString CCommon::Get_LoadIndexVacOffCheckErr()
{
	DX_DATA_11 *pDX11 = g_objAJinAXL.Get_pDX11();
	int *pInfo = gData.InfoIndex[0];

	CString strErr, strSNo;
	strErr = "";
	if (pDX11->iIndexLoadVac01 && pInfo[0] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "1. "; }
	if (pDX11->iIndexLoadVac02 && pInfo[1] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "2. "; }
	if (pDX11->iIndexLoadVac03 && pInfo[2] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "3. "; }
	if (pDX11->iIndexLoadVac04 && pInfo[3] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "4. "; }

	if (strErr != "") {
		strErr += "번 Load Vac 센서를 확인 하여 주십시오.";
	}
	return strErr;
}

CString CCommon::Get_CapPickerVacOnCheckErr()
{

	DX_DATA_07 *pDX07 = g_objAJinAXL.Get_pDX07();
	int *pInfo = gData.InfoCapPick;
	
	CString strErr, strSNo;
	strErr = "";
	if (!pDX07->iCapPickerVac01 && pInfo[0] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "1. "; }
	if (!pDX07->iCapPickerVac02 && pInfo[1] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "2. "; }
	if (!pDX07->iCapPickerVac03 && pInfo[2] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "3. "; }
	if (!pDX07->iCapPickerVac04 && pInfo[3] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "4. "; }

	if (strErr != "") {
		strErr += "번 Cap Picker Vac 센서를 확인 하여 주십시오.";
	}
	return strErr;
}

CString CCommon::Get_CapPickerUpCheckErr()
{
	DX_DATA_07 *pDX07 = g_objAJinAXL.Get_pDX07();
	
	CString strErr, strSNo;
	strErr = "";
	if ((!pDX07->iCapPickerUp01 || pDX07->iCapPickerDown01) ) { if (strErr == "") { strErr = "# ==> "; } strErr += "1. "; }
	if ((!pDX07->iCapPickerUp02 || pDX07->iCapPickerDown02) ) { if (strErr == "") { strErr = "# ==> "; } strErr += "2. "; }
	if ((!pDX07->iCapPickerUp03 || pDX07->iCapPickerDown03) ) { if (strErr == "") { strErr = "# ==> "; } strErr += "3. "; }
	if ((!pDX07->iCapPickerUp04 || pDX07->iCapPickerDown04) ) { if (strErr == "") { strErr = "# ==> "; } strErr += "4. "; }

	if (strErr != "") {
		strErr += "번 Cap Picker Up or Down 센서를 확인 하여 주십시오.";
	}
	return strErr;
}


CString CCommon::Get_CapPickerVacOffCheckErr()
{

	DX_DATA_07 *pDX07 = g_objAJinAXL.Get_pDX07();
	int *pInfo = gData.InfoCapPick;

	CString strErr, strSNo;
	strErr = "";
	if (pDX07->iCapPickerVac01 && pInfo[0] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "1. "; }
	if (pDX07->iCapPickerVac02 && pInfo[1] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "2. "; }
	if (pDX07->iCapPickerVac03 && pInfo[2] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "3. "; }
	if (pDX07->iCapPickerVac04 && pInfo[3] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "4. "; }

	if (strErr != "") {
		strErr += "번 Cap Picker Vac 센서를 확인 하여 주십시오.";
	}
	return strErr;
}

CString CCommon::Get_AssyPickerUpCheckErr()
{
	DX_DATA_08 *pDX08 = g_objAJinAXL.Get_pDX08();

	CString strErr, strSNo;
	strErr = "";
	if ((!pDX08->iAssyPickerUp01 || pDX08->iAssyPickerDown01) ) { if (strErr == "") { strErr = "# ==> "; } strErr += "1. "; }
	if ((!pDX08->iAssyPickerUp02 || pDX08->iAssyPickerDown02) ) { if (strErr == "") { strErr = "# ==> "; } strErr += "2. "; }
	if ((!pDX08->iAssyPickerUp03 || pDX08->iAssyPickerDown03) ) { if (strErr == "") { strErr = "# ==> "; } strErr += "3. "; }
	if ((!pDX08->iAssyPickerUp04 || pDX08->iAssyPickerDown04) ) { if (strErr == "") { strErr = "# ==> "; } strErr += "4. "; }

	if (strErr != "") {
		strErr += "번 Assy Picker Up or Down 센서를 확인 하여 주십시오.";
	}
	return strErr;
}


CString CCommon::Get_AssyPickerDownCheckErr()
{
	DX_DATA_08 *pDX08 = g_objAJinAXL.Get_pDX08();
	
	CString strErr, strSNo;
	strErr = "";
	if ((pDX08->iAssyPickerUp01 || !pDX08->iAssyPickerDown01) ) { if (strErr == "") { strErr = "# ==> "; } strErr += "1. "; }
	if ((pDX08->iAssyPickerUp02 || !pDX08->iAssyPickerDown02) ) { if (strErr == "") { strErr = "# ==> "; } strErr += "2. "; }
	if ((pDX08->iAssyPickerUp03 || !pDX08->iAssyPickerDown03) ) { if (strErr == "") { strErr = "# ==> "; } strErr += "3. "; }
	if ((pDX08->iAssyPickerUp04 || !pDX08->iAssyPickerDown04) ) { if (strErr == "") { strErr = "# ==> "; } strErr += "4. "; }

	if (strErr != "") {
		strErr += "번 Assy Picker Up or Down 센서를 확인하여 주십시오.";
	}
	return strErr;
}



CString CCommon::Get_AssyPickerDownCheckErr(int nPos)
{
	DX_DATA_08 *pDX08 = g_objAJinAXL.Get_pDX08();
	int *pInfo = NULL;
	if (nPos == 0) {
		pInfo = gData.InfoAssyPick;
	}
	if (nPos == 1) {
		pInfo = gData.InfoIndex[1];
	}
	
	CString strErr, strSNo;
	strErr = "";
	if ((pDX08->iAssyPickerUp01 || !pDX08->iAssyPickerDown01) && pInfo[0] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "1. "; }
	if ((pDX08->iAssyPickerUp02 || !pDX08->iAssyPickerDown02) && pInfo[1] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "2. "; }
	if ((pDX08->iAssyPickerUp03 || !pDX08->iAssyPickerDown03) && pInfo[2] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "3. "; }
	if ((pDX08->iAssyPickerUp04 || !pDX08->iAssyPickerDown04) && pInfo[3] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "4. "; }

	if (strErr != "") {
		strErr += "번 Assy Picker Up or Down 센서를 확인하여 주십시오.";
	}
	return strErr;
}


CString CCommon::Get_AssyPickerVacOnCheckErr()
{
	DX_DATA_08 *pDX08 = g_objAJinAXL.Get_pDX08();
	int *pInfo = gData.InfoAssyPick;

	CString strErr, strSNo;
	strErr = "";
	
	if ((!pDX08->iAssyPickerVac01 ) && pInfo[0] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "1. "; }
	if ((!pDX08->iAssyPickerVac02 ) && pInfo[1] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "2. "; }
	if ((!pDX08->iAssyPickerVac03 ) && pInfo[2] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "3. "; }
	if ((!pDX08->iAssyPickerVac04 ) && pInfo[3] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "4. "; }


	if (strErr != "") {
		strErr += "번 Assy Picker Vac On 센서를 확인 하여 주십시오.";
	}
	return strErr;

}

CString CCommon::Get_AssyPickerVacOffCheckErr()
{
	DX_DATA_08 *pDX08 = g_objAJinAXL.Get_pDX08();
	int *pInfo = gData.InfoIndex[1];

	CString strErr, strSNo;
	strErr = "";

	if ((pDX08->iAssyPickerVac01 ) && pInfo[0] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "1. "; }
	if ((pDX08->iAssyPickerVac02 ) && pInfo[1] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "2. "; }
	if ((pDX08->iAssyPickerVac03 ) && pInfo[2] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "3. "; }
	if ((pDX08->iAssyPickerVac04 ) && pInfo[3] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "4. "; }


	if (strErr != "") {
		strErr += "번 Assy Picker Vac On/Off 센서를 확인 하여 주십시오.";
	}
	return strErr;

}


CString CCommon::Get_AssyPickerGoodDownCheckErr()
{
	DX_DATA_08 *pDX08 = g_objAJinAXL.Get_pDX08();
	int *pInfo = gData.InfoIndex[1];

	CString strErr, strSNo;
	strErr = "";

	if ((pDX08->iAssyPickerUp01 || !pDX08->iAssyPickerDown01 ) && gData.InfoIndex[1][0] > 0 && gData.InfoAssyPick[0] == 9 ) { if (strErr == "") { strErr = "# ==> "; } strErr += "1. "; }
	if ((pDX08->iAssyPickerUp02 || !pDX08->iAssyPickerDown02 ) && gData.InfoIndex[1][1] > 0 && gData.InfoAssyPick[1] == 9) { if (strErr == "") { strErr = " #==> "; } strErr += "2. "; }
	if ((pDX08->iAssyPickerUp03 || !pDX08->iAssyPickerDown03 ) && gData.InfoIndex[1][2] > 0 && gData.InfoAssyPick[2] == 9) { if (strErr == "") { strErr = " #==> "; } strErr += "3. "; }
	if ((pDX08->iAssyPickerUp04 || !pDX08->iAssyPickerDown04 ) && gData.InfoIndex[1][3] > 0 && gData.InfoAssyPick[3] == 9) { if (strErr == "") { strErr = " #==> "; } strErr += "4. "; }


	if (strErr != "") {
		strErr += "번 Assy Picker Down Check 센서를 확인 하여 주십시오.";
	}
	return strErr;
	
}

CString CCommon::Get_UnloadPickerDownCheckErr()
{
	DX_DATA_10 *pDX10 = g_objAJinAXL.Get_pDX10();
	int *pInfo = gData.InfoTransStage;

	CString strErr, strSNo;
	strErr = "";

	if ((pDX10->iUnloadPickerUp01 || !pDX10->iUnloadPickerDown01 ) && pInfo[0] > 0 ) { if (strErr == "") { strErr = "# ==> "; } strErr += "1. "; }
	if ((pDX10->iUnloadPickerUp02 || !pDX10->iUnloadPickerDown02 ) && pInfo[1] > 0 ) { if (strErr == "") { strErr = "# ==> "; } strErr += "2. "; }
	if ((pDX10->iUnloadPickerUp03 || !pDX10->iUnloadPickerDown03 ) && pInfo[2] > 0 ) { if (strErr == "") { strErr = "# ==> "; } strErr += "3. "; }
	if ((pDX10->iUnloadPickerUp04 || !pDX10->iUnloadPickerDown04 ) && pInfo[3] > 0 ) { if (strErr == "") { strErr = "# ==> "; } strErr += "4. "; }


	if (strErr != "") {
		strErr += "번 Unload Picker Up/Down 센서를 확인 하여 주십시오.";
	}
	return strErr;

}


CString CCommon::Get_UnloadPickerUpCheckErr()
{
	DX_DATA_10 *pDX10 = g_objAJinAXL.Get_pDX10();
	

	CString strErr, strSNo;
	strErr = "";

	if ((!pDX10->iUnloadPickerUp01 || pDX10->iUnloadPickerDown01 ) ) { if (strErr == "") { strErr = "# ==> "; } strErr += "1. "; }
	if ((!pDX10->iUnloadPickerUp02 || pDX10->iUnloadPickerDown02 ) ) { if (strErr == "") { strErr = "# ==> "; } strErr += "2. "; }
	if ((!pDX10->iUnloadPickerUp03 || pDX10->iUnloadPickerDown03 )) { if (strErr == "") { strErr = "# ==> "; } strErr += "3. "; }
	if ((!pDX10->iUnloadPickerUp04 || pDX10->iUnloadPickerDown04 ) ) { if (strErr == "") { strErr = "# ==> "; } strErr += "4. "; }


	if (strErr != "") {
		strErr += "번 Unload Picker Up/Down 센서를 확인 하여 주십시오.";
	}
	return strErr;

}


CString CCommon::Get_UnloadPickerVacOnCheckErr()
{
	DX_DATA_10 *pDX10 = g_objAJinAXL.Get_pDX10();
	int *pInfo = gData.InfoUnloadPick;

	CString strErr, strSNo;
	strErr = "";

	if ((!pDX10->iUnloadPickerVac01 ) && pInfo[0] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "1. "; }
	if ((!pDX10->iUnloadPickerVac02 ) && pInfo[1] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "2. "; }
	if ((!pDX10->iUnloadPickerVac03 ) && pInfo[2] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "3. "; }
	if ((!pDX10->iUnloadPickerVac04 ) && pInfo[3] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "4. "; }


	if (strErr != "") {
		strErr += "번 Unload Picker Vac 센서를 확인 하여 주십시오.";
	}
	return strErr;

}

CString CCommon::Get_UnloadPickerVacOffCheckErr()
{
	DX_DATA_10 *pDX10 = g_objAJinAXL.Get_pDX10();
	int *pInfo = gData.InfoUnloadPick;

	CString strErr, strSNo;
	strErr = "";

	if ((pDX10->iUnloadPickerVac01 ) && pInfo[0] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "1. "; }
	if ((pDX10->iUnloadPickerVac02 ) && pInfo[1] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "2. "; }
	if ((pDX10->iUnloadPickerVac03 ) && pInfo[2] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "3. "; }
	if ((pDX10->iUnloadPickerVac04 ) && pInfo[3] > 0) { if (strErr == "") { strErr = "# ==> "; } strErr += "4. "; }


	if (strErr != "") {
		strErr += "번 Unload Picker Vac 센서를 확인 하여 주십시오.";
	}
	return strErr;

}



void CCommon::Get_HardInfo(CString &strHardDisk)
{
	ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;

	// GetDiskFreeSpaceEx 호출
	if (GetDiskFreeSpaceEx("C:\\", &freeBytesAvailable, &totalBytes, &totalFreeBytes)) {
		// GB로 변환하여 출력
		double totalGB = static_cast<double>(totalBytes.QuadPart) / (1024 * 1024 * 1024);
		double freeGB = static_cast<double>(totalFreeBytes.QuadPart) / (1024 * 1024 * 1024);
		double usedGB = totalGB - freeGB;

		strHardDisk.Format("%lf", freeGB);
	}
}

void CCommon::Get_RAMSize(CString &strRAM)
{
	ULONGLONG Size;
	GetPhysicallyInstalledSystemMemory(&Size);
	strRAM.Format(_T("%llu"), ((Size/1024)/1024));
}

void CCommon::Get_CPUInfo(CString& strCPU)
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	HKEY hKey;	
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		char buffer[256];
		DWORD bufferSize = sizeof(buffer);
		if (RegQueryValueEx(hKey, _T("ProcessorNameString"), NULL, NULL, (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS)
		{
			strCPU = (CString)buffer;
		}
		RegCloseKey(hKey);
	}

	switch (sysInfo.wProcessorArchitecture) {
	case PROCESSOR_ARCHITECTURE_AMD64: strCPU += " x64";  break;
	case PROCESSOR_ARCHITECTURE_INTEL: strCPU += " x86"; break;
	default:  strCPU += " Unknown"; break;
	}   
}


void CCommon::Save_CapShiipData(int nType)	//1:Cap, 2:Ship
{
	CIniFileCS INI(gsCurrentDir + "\\System\\CapShipData.ini");

	if (nType == 1) {
		INI.Set_String("CAP_DATA", "LOTID", gData.sCapLotID);
		/*INI.Set_Integer("CAP_DATA", "TRAYCOUNT", gData.nCapTrayUseCount);
		INI.Set_Integer("CAP_DATA", "CMCOUNT", gData.nCapUseCount);*/
		INI.Set_Integer("CAP_DATA", "USETYCOUNT", gData.nCapTrayCount);
	}
	if (nType == 2) {
		INI.Set_String("SHIP_DATA", "LOTID", gData.sShipLotID);
		/*INI.Set_Integer("SHIP_DATA", "TRAYCOUNT", gData.nShipTrayUseCount);
		INI.Set_Integer("SHIP_DATA", "CMCOUNT", gData.nShipUseCount);
		INI.Set_Integer("SHIP_DATA", "USETYCOUNT", gData.nShipTrayCount);*/
	}
}


void CCommon::Read_CapShipPartNo()
{
	CIniFileCS INI(gsCurrentDir + "\\System\\CapShipPartNo.ini");
	if (!INI.Check_File()) {
		AfxMessageBox("CapShipPartNo.ini File Not Found!!!");
		gPart.nCapCount = gPart.nShipCount = 0;
		return;
	}

	CString sName;
	gPart.nCapCount = INI.Get_Integer("CAP_PARTNO", "COUNT", 0);
	for(int i=0; i<gPart.nCapCount; i++) {
		sName.Format("%02d", i+1);
		gPart.sCapPart[i] = INI.Get_String("CAP_PARTNO", sName, "");
		if (gPart.sCapPart[i].GetLength() < 1) gPart.sCapPart[i] = "????";
	}
	gPart.nShipCount = INI.Get_Integer("SHIP_PARTNO", "COUNT", 0);
	for(int i=0; i<gPart.nShipCount; i++) {
		sName.Format("%02d", i+1);
		gPart.sShipPart[i] = INI.Get_String("SHIP_PARTNO", sName, "");
		if (gPart.sShipPart[i].GetLength() < 1) gPart.sShipPart[i] = "????";
	}
}

BOOL CCommon::Check_CapShipPartNo(int nType, CString sPartNo)	//1:Cap, 2:Ship
{
	if (nType != 1 && nType != 2) return FALSE;
	if (sPartNo.GetLength() < 4) return FALSE;

	int nSize;
	if (nType == 1) {
		for(int i=0; i<gPart.nCapCount; i++) {
			nSize = gPart.sCapPart[i].GetLength();
			if (sPartNo.Left(nSize) == gPart.sCapPart[i]) return TRUE;
		}
	}

	if (nType == 2) {
		for(int i=0; i<gPart.nShipCount; i++) {
			nSize = gPart.sShipPart[i].GetLength();
			if (sPartNo.Left(nSize) == gPart.sShipPart[i]) return TRUE;
		}
	}
	return FALSE;
}