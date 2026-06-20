// AJinAXL.cpp : 구현 파일
//
#include "stdafx.h"
#include "AJinAXL.h"
#include "CME8000.h"
#include <math.h>
#include "Common.h"
#include "FifoTaskProcessor.h"

// AJin Board Library
#include "AXL.h"
#include "AXM.h"
#include "AXD.h"
#include "AXDev.h"
#pragma comment (lib, "AXL.lib")

CAJinAXL g_objAJinAXL;

CAJinAXL::CAJinAXL(void)
{
	for (int i = 0; i < DIO_MODULE_COUNT; i++) { DXY_DATA *pDX = Get_pDX(i); pDX->nValue = 0; }
	for (int i = 0; i < DIO_MODULE_COUNT; i++) { DXY_DATA *pDY = Get_pDY(i); pDY->nValue = 0; }

	for (int i = 0; i < AXIS_COUNT; i++) m_strAxisName[i] = _T("");

	m_pThreadAJin = NULL;
	m_bThreadAJin = FALSE;

	m_bReadVelocity = FALSE;
	m_runningCount = 0;
}

CAJinAXL::~CAJinAXL(void)
{
}

BOOL CAJinAXL::Initialize()
{
#ifdef AJIN_BOARD_USE
	DWORD dwReturn;

	dwReturn = AxlOpenNoReset(7);
	if (dwReturn != AXT_RT_SUCCESS) return FALSE;

	// DIO
	long lDIOCount;
	dwReturn = AxdInfoGetModuleCount(&lDIOCount);
	if (dwReturn != AXT_RT_SUCCESS) return FALSE;
	if (lDIOCount < DIO_MODULE_COUNT) return FALSE;

	Read_Input();
	Read_Output();

	// Door Interlock
	m_DY13.oModeSelect = TRUE; Write_Output(13); theApp.uSleep(500);
	m_DY13.oSafetyReset = TRUE;  Write_Output(13); theApp.uSleep(500);
	m_DY13.oSafetyReset = FALSE; Write_Output(13); theApp.uSleep(500);
	m_DY13.oSafetyReset = TRUE;  Write_Output(13); theApp.uSleep(500);
	m_DY13.oSafetyReset = FALSE; Write_Output(13); theApp.uSleep(500);

	Alarm_Reset(-1);	// All Reset

	// Motion
	long lAxisCount;
	dwReturn = AxmInfoGetAxisCount(&lAxisCount);
	if (dwReturn != AXT_RT_SUCCESS) return FALSE;
	if (lAxisCount < AXIS_COUNT) return FALSE;

	CString strMotFile = gsCurrentDir + "\\System\\AJinSetting.mot";
	dwReturn = AxmMotLoadParaAll((LPSTR)(LPCSTR)strMotFile);
	if (dwReturn != AXT_RT_SUCCESS) return FALSE;
#endif

	Read_AxisList();	// Axis Name & Param

	for (long i = 0; i < AXIS_COUNT; i++) Set_ServoOn(i);

	m_bThreadAJin = TRUE;
	m_pThreadAJin = AfxBeginThread(Thread_AJin, NULL);

#ifndef AJIN_BOARD_USE
	if (!proc.Start())
	{		
		return FALSE;
	}
#endif


	return TRUE;
}

void CAJinAXL::Terminate()
{
	if (m_pThreadAJin) {
		m_bThreadAJin = FALSE;
		WaitForSingleObject(m_pThreadAJin->m_hThread, INFINITE);
	}

	for (long i = 0; i < AXIS_COUNT; i++) Set_ServoOff(i);

#ifdef AJIN_BOARD_USE
	if (AxlIsOpened()) AxlClose();
#endif
}

void CAJinAXL::Read_Input()
{
#ifdef AJIN_BOARD_USE
	AxdiReadInportDword( 0, 0, &m_DX00.nValue);
	AxdiReadInportDword( 1, 0, &m_DX01.nValue);
	AxdiReadInportDword( 2, 0, &m_DX02.nValue);
	AxdiReadInportDword( 3, 0, &m_DX03.nValue);
	AxdiReadInportDword( 4, 0, &m_DX04.nValue);
	AxdiReadInportDword( 5, 0, &m_DX05.nValue);
	AxdiReadInportDword( 6, 0, &m_DX06.nValue);
	AxdiReadInportDword( 7, 0, &m_DX07.nValue);
	AxdiReadInportDword( 8, 0, &m_DX08.nValue);
	AxdiReadInportDword( 9, 0, &m_DX09.nValue);
	AxdiReadInportDword(10, 0, &m_DX10.nValue);
	AxdiReadInportDword(11, 0, &m_DX11.nValue);
	AxdiReadInportDword(12, 0, &m_DX12.nValue);
	AxdiReadInportDword(13, 0, &m_DX13.nValue);
#endif
}

void CAJinAXL::Read_Output()
{
#ifdef AJIN_BOARD_USE
	AxdoReadOutportDword(14, 0, &m_DY00.nValue);
	AxdoReadOutportDword(15, 0, &m_DY01.nValue);
	AxdoReadOutportDword(16, 0, &m_DY02.nValue);
	AxdoReadOutportDword(17, 0, &m_DY03.nValue);
	AxdoReadOutportDword(18, 0, &m_DY04.nValue);
	AxdoReadOutportDword(19, 0, &m_DY05.nValue);
	AxdoReadOutportDword(20, 0, &m_DY06.nValue);
	AxdoReadOutportDword(21, 0, &m_DY07.nValue);
	AxdoReadOutportDword(22, 0, &m_DY08.nValue);
	AxdoReadOutportDword(23, 0, &m_DY09.nValue);
	AxdoReadOutportDword(24, 0, &m_DY10.nValue);
	AxdoReadOutportDword(25, 0, &m_DY11.nValue);
	AxdoReadOutportDword(26, 0, &m_DY12.nValue);
	AxdoReadOutportDword(27, 0, &m_DY13.nValue);
#endif
}

void CAJinAXL::Write_Output(int nModule)
{
#ifdef AJIN_BOARD_USE
	if (nModule ==  0) AxdoWriteOutportDword(14, 0, m_DY00.nValue);
	if (nModule ==  1) AxdoWriteOutportDword(15, 0, m_DY01.nValue);
	if (nModule ==  2) AxdoWriteOutportDword(16, 0, m_DY02.nValue);
	if (nModule ==  3) AxdoWriteOutportDword(17, 0, m_DY03.nValue);
	if (nModule ==  4) AxdoWriteOutportDword(18, 0, m_DY04.nValue);
	if (nModule ==  5) AxdoWriteOutportDword(19, 0, m_DY05.nValue);
	if (nModule ==  6) AxdoWriteOutportDword(20, 0, m_DY06.nValue);
	if (nModule ==  7) AxdoWriteOutportDword(21, 0, m_DY07.nValue);
	if (nModule ==  8) AxdoWriteOutportDword(22, 0, m_DY08.nValue);
	if (nModule ==  9) AxdoWriteOutportDword(23, 0, m_DY09.nValue);
	if (nModule == 10) AxdoWriteOutportDword(24, 0, m_DY10.nValue);
	if (nModule == 11) AxdoWriteOutportDword(25, 0, m_DY11.nValue);
	if (nModule == 12) AxdoWriteOutportDword(26, 0, m_DY12.nValue);
	if (nModule == 13) AxdoWriteOutportDword(27, 0, m_DY13.nValue);
#else
	Sim_SetOutToIn(nModule);
#endif
}

void CAJinAXL::Read_MotionStatus()
{
#ifdef AJIN_BOARD_USE
	DWORD dwStatus;
	for (int i = 0; i < AXIS_COUNT; i++) {
		//AxmStatusGetCmdPos(i, &m_Status[i].dPos);		// Position Reading
		AxmStatusGetActPos(i, &m_Status[i].dPos);		// Position Reading

		if (m_bReadVelocity) AxmStatusReadVel(i, &m_Status[i].dVel);	// Velocity Reading

		AxmSignalIsServoOn(i, &dwStatus);								// Servo-On Reading
		m_Status[i].bSOn = (BOOL)dwStatus;

		AxmSignalReadInputBit(i, UIO_INP0, &dwStatus);	// Origin Limit (In0 : Home)
		m_Status[i].bOrg = (BOOL)dwStatus;

		AxmStatusReadMechanical(i, &dwStatus);	// Mechanical Input
		m_Status[i].bELP = (BOOL)((dwStatus >> 0) & 1);	// Positive Limit (bit0)
		m_Status[i].bELN = (BOOL)((dwStatus >> 1) & 1);	// Negative Limit (bit1)
		m_Status[i].bALM = (BOOL)((dwStatus >> 4) & 1);	// Alarm (bit4)
		m_Status[i].bInP = (BOOL)((dwStatus >> 5) & 1);	// In-Position (bit5)

		AxmStatusReadInMotion(i, &dwStatus);			// Motion Running
		m_Status[i].bRun = (BOOL)dwStatus;

		AxmHomeGetResult(i, &dwStatus);					// Home Done
		m_Status[i].bHom = (dwStatus == HOME_SUCCESS ? TRUE : FALSE);
	}
#endif
}

void CAJinAXL::Set_ServoOn(int nAxis)
{
#ifdef AJIN_BOARD_USE
	AxmMoveEStop(nAxis);
	AxmSignalServoOn(nAxis, TRUE);
#endif
}

void CAJinAXL::Set_ServoOff(int nAxis)
{
#ifdef AJIN_BOARD_USE
	AxmMoveEStop(nAxis);
	AxmSignalServoOn(nAxis, FALSE);
#endif
}

void CAJinAXL::Set_EncoderType(int nAxis, int nType)
{
#ifdef AJIN_BOARD_USE
	if (!Is_AbsoluteType(nAxis)) return;
	if (nType == 0) return;		// RTEX일 경우 Incremental 모드 설정을 하지 않음
	AxmSignalSetEncoderType(nAxis, nType);	// ENCODER_TYPE_INCREMENTAL(0), ENCODER_TYPE_ABSOLUTE(1)
	Sleep(10);		// 추가 2017.07.28
#endif
}

void CAJinAXL::Home_Search(int nAxis)
{
#ifdef AJIN_BOARD_USE
	AxmHomeSetStart(nAxis);
	m_Status[nAxis].bHom = FALSE; m_Status[nAxis].bRun = TRUE;
#endif
}

void CAJinAXL::Set_Home(int nAxis)
{
#ifdef AJIN_BOARD_USE
	AxmStatusSetActPos(nAxis, 0.0);
	AxmStatusSetCmdPos(nAxis, 0.0);
	AxmHomeSetResult(nAxis, HOME_SUCCESS);
#endif
}

void CAJinAXL::Move_Absolute(int nAxis, double dPos)
{
#ifdef AJIN_BOARD_USE
	AxmMotSetAbsRelMode(nAxis, POS_ABS_MODE);
	double	dVel = m_Param[nAxis].dSpeedM;
	double	dAcc = m_Param[nAxis].dAccel;
	AxmMoveStartPos(nAxis, dPos, dVel, dAcc, dAcc);
	m_Status[nAxis].bInP = FALSE; m_Status[nAxis].bRun = TRUE;
#else
	Sim_SetMotion(1, nAxis, dPos);
#endif
}

void CAJinAXL::Move_AbsSlow(int nAxis, double dPos, double dRatio)
{
#ifdef AJIN_BOARD_USE
	AxmMotSetAbsRelMode(nAxis, POS_ABS_MODE);
	double	dVel = m_Param[nAxis].dSpeedM * dRatio;
	double	dAcc = m_Param[nAxis].dAccel * dRatio;
	AxmMoveStartPos(nAxis, dPos, dVel, dAcc, dAcc);
	m_Status[nAxis].bInP = FALSE; m_Status[nAxis].bRun = TRUE;
#else
	Sim_SetMotion(1,nAxis, dPos);
#endif
}

void CAJinAXL::Move_Relative(int nAxis, double dPos)
{
#ifdef AJIN_BOARD_USE
	if (nAxis == AX_MAIN_INDEX_R) {
		AxmStatusSetActPos(nAxis, 0.0);
		AxmStatusSetCmdPos(nAxis, 0.0);
	}
	AxmMotSetAbsRelMode(nAxis, POS_REL_MODE);
	double	dVel = m_Param[nAxis].dSpeedM;
	double	dAcc = m_Param[nAxis].dAccel;
	AxmMoveStartPos(nAxis, dPos, dVel, dAcc, dAcc);
	m_Status[nAxis].bInP = FALSE; m_Status[nAxis].bRun = TRUE;
#else
	Sim_SetMotion(2,nAxis, dPos);
#endif
}

void CAJinAXL::Jog_Positive(int nAxis)
{
#ifdef AJIN_BOARD_USE
	double	dVel = m_Param[nAxis].dSpeedJ;
	double	dAcc = m_Param[nAxis].dSpeedJ * 4.0;
	AxmMoveVel(nAxis, dVel, dAcc, dAcc);
#endif
}

void CAJinAXL::Jog_Negative(int nAxis)
{
#ifdef AJIN_BOARD_USE
	double	dVel = m_Param[nAxis].dSpeedJ * -1.0;
	double	dAcc = m_Param[nAxis].dSpeedJ * 4.0;
	AxmMoveVel(nAxis, dVel, dAcc, dAcc);
#endif
}

void CAJinAXL::Stop_Motion(int nAxis)
{
#ifdef AJIN_BOARD_USE
	AxmMoveSStop(nAxis);
#endif
}

void CAJinAXL::EStop_Motion(int nAxis)
{
#ifdef AJIN_BOARD_USE
	AxmMoveEStop(nAxis);
#endif
}

void CAJinAXL::Alarm_Reset(int nAxis)
{
#ifdef AJIN_BOARD_USE
	int nAxStart = (nAxis == -1 ? 0 : nAxis);
	int nAxEnd = (nAxis == -1 ? AXIS_COUNT : nAxis + 1);
	for (int i = nAxStart; i < nAxEnd; i++) AxmMoveEStop(i);
	Sleep(200);
	for (int i = nAxStart; i < nAxEnd; i++) AxmSignalServoAlarmReset(i, TRUE);
	Sleep(200);
	for (int i = nAxStart; i < nAxEnd; i++) AxmSignalServoAlarmReset(i, FALSE);
#endif
}

void CAJinAXL::Move_Abs_Override(int nAxis, double dPos, double dAt)
{
#ifdef AJIN_BOARD_USE
	AxmMotSetAbsRelMode(nAxis, POS_ABS_MODE);
	double	dVel = m_Param[nAxis].dSpeedM;
	double	dAcc = m_Param[nAxis].dSpeedM * 4.0;
	double dMaxVel;
	AxmMotGetMaxVel(nAxis, &dMaxVel);

	AxmOverrideSetMaxVel(nAxis, dMaxVel);	// 오버라이드 최대값 설정
	AxmOverrideVelAtPos (nAxis, dPos, dVel, dAcc, dAcc, dAt, dVel/4.0, COMMAND);
	m_Status[nAxis].bInP = FALSE; m_Status[nAxis].bRun = TRUE;
#else
	Sim_SetMotion(3, nAxis, dPos);
#endif
}

void CAJinAXL::Move_Rel_Override(int nAxis, double dPos, double dAt)
{
#ifdef AJIN_BOARD_USE
	AxmMotSetAbsRelMode(nAxis, POS_REL_MODE);
	double	dVel = m_Param[nAxis].dSpeedM;
	double	dAcc = m_Param[nAxis].dSpeedM * 4.0;
	double dMaxVel;
	AxmMotGetMaxVel(nAxis, &dMaxVel);
	
	AxmOverrideSetMaxVel(nAxis, dMaxVel);	// 오버라이드 최대값 설정
	AxmOverrideVelAtPos (nAxis, dPos, dVel, dAcc, dAcc, dAt, dVel/4.0, COMMAND);
	m_Status[nAxis].bInP = FALSE; m_Status[nAxis].bRun = TRUE;
#endif
}

BOOL CAJinAXL::Is_Done(int nAxis)
{
#ifdef AJIN_BOARD_USE
	if (m_Status[nAxis].bRun) return FALSE;
	if (!m_Status[nAxis].bInP) return FALSE;
#endif
	return TRUE;
}

BOOL CAJinAXL::Is_Home(int nAxis)
{
#ifdef AJIN_BOARD_USE
	if (!m_Status[nAxis].bHom) return FALSE;
	if (m_Status[nAxis].bRun) return FALSE;
#endif
	return TRUE;
}

BOOL CAJinAXL::Is_MoveDone(int nAxis, double dPos, double dRange)
{
#ifdef AJIN_BOARD_USE
	if (!Is_Done(nAxis)) return FALSE;
	if (fabs(m_Status[nAxis].dPos - dPos) > dRange) return FALSE;
#endif
	return TRUE;
}

void CAJinAXL::Start_Trigger(int nAxis, double dStartPos, double dEndPos, double dPeriod, double dWidth)
{
#ifdef AJIN_BOARD_USE
	AxmTriggerSetReset(nAxis);

	double dTrigTime = dWidth / m_Param[nAxis].dSpeedM * 1000000.0;	// mm->usec

	// 1. Command Position ****************************************************
// 	AxmTriggerSetTimeLevel(nAxis, dTrigTime, HIGH, COMMAND, DISABLE);
	// 2. Actual Position *****************************************************
	AxmTriggerSetTimeLevel(nAxis, dTrigTime, HIGH, ACTUAL, DISABLE);
	//*************************************************************************
	
	DWORD dwCode = AxmTriggerSetBlock(nAxis, dStartPos, dEndPos, dPeriod);
	if (dwCode != AXT_RT_SUCCESS) AfxMessageBox("Trigger Setting Error");
#endif
}

void CAJinAXL::Stop_Trigger(int nAxis)
{
#ifdef AJIN_BOARD_USE
	AxmTriggerSetReset(nAxis);
#endif
}

void CAJinAXL::Start_Scan(int nAxis, double dPos, double dVel, double dTrigStart, double dTrigEnd, double dPeriod, double dWidth)
{
#ifdef AJIN_BOARD_USE
	// Trigger Setting
	AxmTriggerSetReset(nAxis);
	
	dPeriod /= 1000;
	dWidth /= 1000;

	double dTrigTime = dWidth / dVel * 1000000.0;	// mm->usec

	//dTrigTime = 20.0;	// mm->usec

	// 1. Command Position ****************************************************
// 	AxmTriggerSetTimeLevel(nAxis, dTrigTime, HIGH, COMMAND, DISABLE);
	// 2. Actual Position *****************************************************
	AxmTriggerSetTimeLevel(nAxis, dTrigTime, HIGH, ACTUAL, DISABLE);
	//*************************************************************************
	
	DWORD dwCode = AxmTriggerSetBlock(nAxis, dTrigStart, dTrigEnd, dPeriod);
	if (dwCode != AXT_RT_SUCCESS) AfxMessageBox("Trigger Setting Error");

	// Scan Move
	AxmMotSetAbsRelMode(nAxis, POS_ABS_MODE);
	double	dAcc = dVel * 10.0;
	AxmMoveStartPos(nAxis, dPos, dVel, dAcc, dAcc);
	m_Status[nAxis].bInP = FALSE; m_Status[nAxis].bRun = TRUE;
#endif
}

void CAJinAXL::Stop_Scan(int nAxis)
{
#ifdef AJIN_BOARD_USE
	AxmTriggerSetReset(nAxis);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Motion Param Read / Write Function

void CAJinAXL::Read_AxisList()
{
	CIniFileCS INI(gsCurrentDir + "\\System\\AxisList.ini");
	if (!INI.Check_File()) {
		AfxMessageBox("AxisList.ini File Not Found!!!");
		return;
	}

	CString strSection, strName;
	for (int i = 0; i < AXIS_COUNT; i++) {
		strSection.Format("AXIS_%02d", i);

		strName = INI.Get_String(strSection, "NAME", "");
		m_strAxisName[i].Format("[%02d] %s", i, strName);
		m_Param[i].dSpeedM = INI.Get_Double(strSection, "MOVE", 0.0);
		m_Param[i].dSpeedJ = INI.Get_Double(strSection, "JOG", 0.0);
		m_Param[i].dAccel = INI.Get_Double(strSection, "ACC", 0.0);
	}
}

void CAJinAXL::Read_MotionParam(int nAxis)
{
	CIniFileCS INI(gsCurrentDir + "\\System\\AxisList.ini");
	if (!INI.Check_File()) {
		AfxMessageBox("AxisList.ini File Not Found!!!");
		return;
	}
	
	CString strSection;
	strSection.Format("AXIS_%02d", nAxis);
	m_Param[nAxis].dSpeedM = INI.Get_Double(strSection, "MOVE", 0.0);
	m_Param[nAxis].dSpeedJ = INI.Get_Double(strSection, "JOG", 0.0);
	m_Param[nAxis].dAccel = INI.Get_Double(strSection, "ACC", 0.0);
}

void CAJinAXL::Save_MotionParam(int nAxis, double dSpeedM, double dSpeedJ, double dAccel)
{
	CIniFileCS INI(gsCurrentDir + "\\System\\AxisList.ini");
	if (!INI.Check_File()) {
		AfxMessageBox("AxisList.ini File Not Found!!!");
		return;
	}
	
	CString strSection;
	strSection.Format("AXIS_%02d", nAxis);
	INI.Set_Double(strSection, "MOVE", dSpeedM, "%0.3lf");
	INI.Set_Double(strSection, "JOG", dSpeedJ, "%0.3lf");
	INI.Set_Double(strSection, "ACC", dAccel, "%0.3lf");

	// RMS 항목
	Save_RmsMotionSpeed(nAxis, dSpeedM, dAccel);
}

void CAJinAXL::Save_RmsMotionSpeed(int nAxis, double dSpeedM, double dAccel)
{	
	CString strAxis, strSpeed, strAccel;
	strSpeed.Format("%0.3lf", dSpeedM);
	strAccel.Format("%0.3lf", dAccel);
	
	
}

/////////////////////////////////////////////////////////////////////////////
// Thread Function 
UINT CAJinAXL::Thread_AJin(LPVOID lpVoid)
{
	while (g_objAJinAXL.m_bThreadAJin) {
		g_objAJinAXL.Read_Input();
		g_objAJinAXL.Read_MotionStatus();
		Sleep(5);
	}
	g_objAJinAXL.m_bThreadAJin = FALSE;
	g_objAJinAXL.m_pThreadAJin = NULL;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CAJinAXL::Is_AbsoluteType(int nAxis)
{
	switch (nAxis) {
	case AX_LOAD_STAGE1_Z:
	case AX_LOAD_STAGE2_Z:
	case AX_CAP_STAGE1_Z:
	case AX_CAP_STAGE2_Z:
	case AX_UNLOAD_STAGE1_Z:
	case AX_UNLOAD_STAGE2_Z:
		return TRUE;
	default:
		return FALSE;
	}
}

BOOL CAJinAXL::Use_OrgAxis(int nAxis)
{
	switch (nAxis) {
	case AX_MAIN_INDEX_R:
	case AX_TRANS_STAGE_T:
		return TRUE;
	default:
		return FALSE;
	}
}

BOOL CAJinAXL::Use_ElpAxis(int nAxis)
{
	if (Use_OrgAxis(nAxis)) return FALSE;
	else return TRUE;
}

BOOL CAJinAXL::Use_ElnAxis(int nAxis)
{
	if (Use_OrgAxis(nAxis)) return FALSE;
	else return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

DXY_DATA *CAJinAXL::Get_pDX(int nIndex)
{
	if (nIndex ==  0) return (DXY_DATA*)&m_DX00;
	if (nIndex ==  1) return (DXY_DATA*)&m_DX01;
	if (nIndex ==  2) return (DXY_DATA*)&m_DX02;
	if (nIndex ==  3) return (DXY_DATA*)&m_DX03;
	if (nIndex ==  4) return (DXY_DATA*)&m_DX04;
	if (nIndex ==  5) return (DXY_DATA*)&m_DX05;
	if (nIndex ==  6) return (DXY_DATA*)&m_DX06;
	if (nIndex ==  7) return (DXY_DATA*)&m_DX07;
	if (nIndex ==  8) return (DXY_DATA*)&m_DX08;
	if (nIndex ==  9) return (DXY_DATA*)&m_DX09;
	if (nIndex == 10) return (DXY_DATA*)&m_DX10;
	if (nIndex == 11) return (DXY_DATA*)&m_DX11;
	if (nIndex == 12) return (DXY_DATA*)&m_DX12;
	if (nIndex == 13) return (DXY_DATA*)&m_DX13;
	return NULL;
}

DXY_DATA *CAJinAXL::Get_pDY(int nIndex)
{
	if (nIndex ==  0) return (DXY_DATA*)&m_DY00;
	if (nIndex ==  1) return (DXY_DATA*)&m_DY01;
	if (nIndex ==  2) return (DXY_DATA*)&m_DY02;
	if (nIndex ==  3) return (DXY_DATA*)&m_DY03;
	if (nIndex ==  4) return (DXY_DATA*)&m_DY04;
	if (nIndex ==  5) return (DXY_DATA*)&m_DY05;
	if (nIndex ==  6) return (DXY_DATA*)&m_DY06;
	if (nIndex ==  7) return (DXY_DATA*)&m_DY07;
	if (nIndex ==  8) return (DXY_DATA*)&m_DY08;
	if (nIndex ==  9) return (DXY_DATA*)&m_DY09;
	if (nIndex == 10) return (DXY_DATA*)&m_DY10;
	if (nIndex == 11) return (DXY_DATA*)&m_DY11;
	if (nIndex == 12) return (DXY_DATA*)&m_DY12;
	if (nIndex == 13) return (DXY_DATA*)&m_DY13;
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void CAJinAXL::Save_AxisList()
{
	CIniFileCS INI(gsCurrentDir + "\\System\\AxisList.ini");
	if (!INI.Check_File()) { AfxMessageBox("AxisList.ini File Not Found!!!"); return; }

	CString strAxis[AXIS_COUNT];

	strAxis[0 ] = "TRAY PICKER X";
	strAxis[1 ] = "TRAY PICKER Z";
	strAxis[2 ] = "TRAY PICKER R";
	strAxis[3 ] = "NO AXIS";
	strAxis[4 ] = "LOAD STAGE1 X";
	strAxis[5 ] = "LOAD STAGE1 Z";
	strAxis[6 ] = "LOAD STAGE2 X";
	strAxis[7 ] = "LOAD STAGE2 Z";
	strAxis[8 ] = "LOAD PICKER Y";
	strAxis[9 ] = "LOAD PICKER Z";
	strAxis[10] = "LOAD PICKER P";
	strAxis[11] = "MAIN INDEX R";
	strAxis[12] = "VISION CM X";
	strAxis[13] = "CAP STAGE1 X";
	strAxis[14] = "CAP STAGE1 Z";
	strAxis[15] = "CAP STAGE2 X";
	strAxis[16] = "CAP STAGE2 Z";
	strAxis[17] = "CAP PICKER Y";
	strAxis[18] = "CAP PICKER Z";
	strAxis[19] = "CAP PICKER P";
	strAxis[20] = "CAP BUFFER Y";
	strAxis[21] = "VISION CAP Y";
	strAxis[22] = "ASSY PICKER X";
	strAxis[23] = "ASSY PICKER Y";
	strAxis[24] = "ASSY PICKER Z";
	strAxis[25] = "TRANS STAGE X";
	strAxis[26] = "TRANS STAGE Z";
	strAxis[27] = "TRANS STAGE T";
	strAxis[28] = "UNLOAD PICKER X";
	strAxis[29] = "UNLOAD PICKER Z";
	strAxis[30] = "UNLOAD PICKER P";
	strAxis[31] = "UNLOAD STAGE1 Y";
	strAxis[32] = "UNLOAD STAGE1 Z";
	strAxis[33] = "UNLOAD STAGE2 Y";
	strAxis[34] = "UNLOAD STAGE2 Z";

	CString strSection, strName;
	for (int i = 0; i < AXIS_COUNT; i++) {
		strSection.Format("AXIS_%02d", i);
		INI.Set_String(strSection, "NAME", strAxis[i]);
	}

	Read_AxisList();
}

///////////////////////////////////////////////////////////////////////////////



void CAJinAXL::Sim_SetMotion(int nNo, int nAxis, double dPos)
{	
	Task t;
	t.nType = nNo;
	t.nAxis = nAxis;
	t.dPos = dPos;
	t.id = 0;

	proc.Enqueue(t);
	::Sleep(SIM_WAITTIMEM);		
}





ThreadIdManager::ThreadIdManager()
	: m_nextId(1)
{
}

int ThreadIdManager::AllocateId()
{
	CSingleLock lock(&m_cs, TRUE);
	if (!m_freeIds.empty())
	{
		std::set<int>::iterator it = m_freeIds.begin();
		int id = *it;
		m_freeIds.erase(it);
		return id;
	}
	return m_nextId++;
}

void ThreadIdManager::ReleaseId(int id)
{
	CSingleLock lock(&m_cs, TRUE);
	m_freeIds.insert(id);
}


int CAJinAXL::StartThread(int nType, int nAxis, double dPos)
{
	// 1) ID 배정
	int id = m_idMgr.AllocateId();
	if (id <= 0) return 0;

	// 2) 인자 준비
	ThreadArgs* pArgs	= new ThreadArgs;
	pArgs->pRunner		= this;
	pArgs->id			= id;
	pArgs->nAxisNo		= nAxis;
	pArgs->type			= nType;
	pArgs->dPosTarget	= dPos;

	// 3) 스레드 시작
	CWinThread* pThread = AfxBeginThread(&CAJinAXL::WorkerProc, pArgs);
	if (!pThread)
	{
		m_idMgr.ReleaseId(id);
		delete pArgs;
		return 0;
	}

	// 실행 중 개수 증가
	{
		CSingleLock lock(&m_csRunning, TRUE);
		++m_runningCount;
	}

	return id;
}


// 정적 워커 스레드 함수
UINT __cdecl CAJinAXL::WorkerProc(LPVOID pParam)
{
	ThreadArgs* pArgs = reinterpret_cast<ThreadArgs*>(pParam);
	if (!pArgs) return 0;

	CAJinAXL* pRunner = pArgs->pRunner;
	const int id = pArgs->id;

	const int nType = pArgs->type;
	const int nAxis = pArgs->nAxisNo;
	double dSpeed = g_objAJinAXL.Get_pParam(nAxis)->dSpeedM;

	double dPos = pArgs->dPosTarget;
	double dPosCur = g_objAJinAXL.m_Status[nAxis].dPos;


	g_objAJinAXL.Get_pStatus(nAxis)->bRun = TRUE;

	if(nType == 1 || nType == 3)
	{	
		while(TRUE)
		{	
			::Sleep(SIM_WAITTIMEM);

			if(dPos >= dPosCur)
			{
				g_objAJinAXL.m_Status[nAxis].dPos += (dSpeed/1000)*VELOCITY_WEIGHT;
				if(g_objAJinAXL.m_Status[nAxis].dPos > dPos) break;
			}
			else if(dPos < dPosCur)
			{
				g_objAJinAXL.m_Status[nAxis].dPos -= (dSpeed/1000)*VELOCITY_WEIGHT;
				if(g_objAJinAXL.m_Status[nAxis].dPos < dPos) break;
			}

		}

		g_objAJinAXL.Get_pStatus(nAxis)->bRun = FALSE;
		g_objAJinAXL.m_Status[nAxis].dPos = dPos;				
	}
	else if(nType == 2)
	{
		double target = g_objAJinAXL.m_Status[nAxis].dPos + dPos;

		while(TRUE)
		{
			::Sleep(SIM_WAITTIMEM);

			if(dPos >= dPosCur)
			{
				g_objAJinAXL.m_Status[nAxis].dPos += (dSpeed/1000)*VELOCITY_WEIGHT;
				if(g_objAJinAXL.m_Status[nAxis].dPos > target) break;
			}
			else if(dPos < dPosCur)
			{
				g_objAJinAXL.m_Status[nAxis].dPos -= (dSpeed/1000)*VELOCITY_WEIGHT;
				if(g_objAJinAXL.m_Status[nAxis].dPos < target) break;
			}			

		}

		g_objAJinAXL.Get_pStatus(nAxis)->bRun = FALSE;
		g_objAJinAXL.m_Status[nAxis].dPos = target;	

	}


	// 종료 통지
	if (pRunner)
		pRunner->NotifyDone(id);

	// 인자 해제
	delete pArgs;
	return 0;
}




void CAJinAXL::GetCompletedIds(std::vector<int>& outCompleted)
{
	outCompleted.clear();
	CSingleLock lock(&m_csCompleted, TRUE);
	if (!m_completedIds.empty())
	{
		outCompleted.swap(m_completedIds); // 한번에 비우기
	}
}

int CAJinAXL::RunningCount() const
{
	CSingleLock lock(const_cast<CCriticalSection*>(&m_csRunning), TRUE);
	return m_runningCount;
}


void CAJinAXL::NotifyDone(int id)
{
	// 1) ID 반납
	m_idMgr.ReleaseId(id);

	// 2) 완료 큐 적재
	{
		CSingleLock lock(&m_csCompleted, TRUE);
		m_completedIds.push_back(id);
	}

	// 3) 실행 중 개수 감소
	{
		CSingleLock lock(&m_csRunning, TRUE);
		if (m_runningCount > 0) --m_runningCount;
	}
}

void CAJinAXL::Sim_SetOutToIn(int nNo)
{
	Sleep(SIM_WAITTIMES);
	BOOL bT = TRUE;
	BOOL bF = FALSE;

	if (nNo==0) {
		m_DX00.iTrayPickerGrip1Close = m_DY00.oTrayPickerGrip12Close;
		m_DX00.iTrayPickerGrip2Close = m_DY00.oTrayPickerGrip12Close;
		
		m_DX00.iTrayPickerGrip1Open = m_DY00.oTrayPickerGrip12Open;
		m_DX00.iTrayPickerGrip2Open = m_DY00.oTrayPickerGrip12Open;

		m_DX00.iTrayPickerGrip3Close = m_DY00.oTrayPickerGrip34Close;
		m_DX00.iTrayPickerGrip4Close = m_DY00.oTrayPickerGrip34Close;

		m_DX00.iTrayPickerGrip3Open = m_DY00.oTrayPickerGrip34Open;
		m_DX00.iTrayPickerGrip4Open = m_DY00.oTrayPickerGrip34Open;


		m_DX00.iLoadPort1SlideLock = m_DY00.oLoadPort1SlideLock;
		m_DX00.iLoadPort1SlideUnlock = m_DY00.oLoadPort1SlideUnlock;

		m_DX00.iLoadPort1Support1In = m_DY00.oLoadPort1SupportIn;
		m_DX00.iLoadPort1Support1Out = m_DY00.oLoadPort1SupportOut;

		m_DX00.iLoadPort1Support2In = m_DY00.oLoadPort1SupportIn;
		m_DX00.iLoadPort1Support2Out = m_DY00.oLoadPort1SupportOut;

	}

	if (nNo==1) {
		
		m_DX01.iLoadPort2SlideLock = m_DY01.oLoadPort2SlideLock;
		m_DX01.iLoadPort2SlideUnlock = m_DY01.oLoadPort2SlideUnlock;

		m_DX01.iLoadPort2Support1In = m_DY01.oLoadPort2SupportIn;
		m_DX01.iLoadPort2Support1Out = m_DY01.oLoadPort2SupportOut;
		m_DX01.iLoadPort2Support2In = m_DY01.oLoadPort2SupportIn;
		m_DX01.iLoadPort2Support2Out = m_DY01.oLoadPort2SupportOut;

		//m_DX01.iLoadPortAreaCheck =
		m_DX01.iLoadPort3SlideLock = m_DY01.oLoadPort3SlideLock;
		m_DX01.iLoadPort3SlideUnlock = m_DY01.oLoadPort3SlideUnlock;
	}

	if (nNo==2) {
		m_DX02.iCapPort1SlideLock = m_DY02.oCapPort1SlideLock;
		m_DX02.iCapPort1SlideUnlock = m_DY02.oCapPort1SlideUnlock;
		
		m_DX02.iCapPort1Support1In = m_DY02.oCapPort1SupportIn;
		m_DX02.iCapPort1Support1Out = m_DY02.oCapPort1SupportOut;

		m_DX02.iCapPort1Support2In = m_DY02.oCapPort1SupportIn;
		m_DX02.iCapPort1Support2Out = m_DY02.oCapPort1SupportOut;

		m_DX02.iCapPort2Support1In = m_DY02.oCapPort2SupportIn;
		m_DX02.iCapPort2Support1Out = m_DY02.oCapPort2SupportOut;

		m_DX02.iCapPort2Support2In = m_DY02.oCapPort2SupportIn;
		m_DX02.iCapPort2Support2Out = m_DY02.oCapPort2SupportOut;
	}

	if (nNo==3) {
		m_DX03.iUnloadPort1SlideLock = m_DY03.oUnloadPort1SlideLock;
		m_DX03.iUnloadPort1SlideUnlock = m_DY03.oUnloadPort1SlideUnlock;

		m_DX03.iUnloadPort1Support1In = m_DY03.oUnloadPort1SupportIn;
		m_DX03.iUnloadPort1Support1Out = m_DY03.oUnloadPort1SupportOut;

		m_DX03.iUnloadPort1Support2In = m_DY03.oUnloadPort1SupportIn;
		m_DX03.iUnloadPort1Support2Out = m_DY03.oUnloadPort1SupportOut;


		m_DX03.iUnloadPort2Support1In = m_DY03.oUnloadPort2SupportIn;
		m_DX03.iUnloadPort2Support1Out = m_DY03.oUnloadPort2SupportOut;

		m_DX03.iUnloadPort2Support2In = m_DY03.oUnloadPort2SupportIn;
		m_DX03.iUnloadPort2Support2Out = m_DY03.oUnloadPort2SupportOut;


	}

	if (nNo==4) {
		if(m_DY04.oLoadStage1MasterIn == TRUE){
			m_DX04.iLoadStage1MasterIn = TRUE;
			m_DX04.iLoadStage1MasterOut = FALSE;
		}
		else{
			m_DX04.iLoadStage1MasterIn = FALSE;
			m_DX04.iLoadStage1MasterOut = TRUE;
		}

		if(m_DY04.oLoadStage1SlaveIn == TRUE){
			m_DX04.iLoadStage1SlaveIn = TRUE;
			m_DX04.iLoadStage1SlaveOut = FALSE;
		}
		else{
			m_DX04.iLoadStage1SlaveIn = FALSE;
			m_DX04.iLoadStage1SlaveOut = TRUE;
		}


		if(m_DY04.oLoadStage2MasterIn == TRUE){
			m_DX04.iLoadStage2MasterIn = TRUE;
			m_DX04.iLoadStage2MasterOut = FALSE;
		}
		else{
			m_DX04.iLoadStage2MasterIn = FALSE;
			m_DX04.iLoadStage2MasterOut = TRUE;
		}

		if(m_DY04.oLoadStage2SlaveIn == TRUE){
			m_DX04.iLoadStage2SlaveIn = TRUE;
			m_DX04.iLoadStage2SlaveOut = FALSE;
		}
		else{
			m_DX04.iLoadStage2SlaveIn = FALSE;
			m_DX04.iLoadStage2SlaveOut = TRUE;
		}


		if(m_DY04.oCapStage1MasterIn == TRUE){
			m_DX04.iCapStage1MasterIn = TRUE;
			m_DX04.iCapStage1MasterOut = FALSE;
		}
		else{
			m_DX04.iCapStage1MasterIn = FALSE;
			m_DX04.iCapStage1MasterOut = TRUE;
		}

		if(m_DY04.oCapStage1SlaveIn == TRUE){
			m_DX04.iCapStage1SlaveIn = TRUE;
			m_DX04.iCapStage1SlaveOut = FALSE;
		}
		else{
			m_DX04.iCapStage1SlaveIn = FALSE;
			m_DX04.iCapStage1SlaveOut = TRUE;
		}

		if(m_DY04.oCapStage2MasterIn == TRUE){
			m_DX04.iCapStage2MasterIn = TRUE;
			m_DX04.iCapStage2MasterOut = FALSE;
		}
		else{
			m_DX04.iCapStage2MasterIn = FALSE;
			m_DX04.iCapStage2MasterOut = TRUE;
		}

		if(m_DY04.oCapStage2SlaveIn == TRUE){
			m_DX04.iCapStage2SlaveIn = TRUE;
			m_DX04.iCapStage2SlaveOut = FALSE;
		}
		else{
			m_DX04.iCapStage2SlaveIn = FALSE;
			m_DX04.iCapStage2SlaveOut = TRUE;
		}

		
	}

	if (nNo==5) {
		if(m_DY05.oUnloadStage1MasterIn == TRUE){
			m_DX05.iUnloadStage1MasterIn = TRUE;
			m_DX05.iUnloadStage1MasterOut = FALSE;
		}
		else{
			m_DX05.iUnloadStage1MasterIn = FALSE;
			m_DX05.iUnloadStage1MasterOut = TRUE;
		}

		if(m_DY05.oUnloadStage1SlaveIn == TRUE){
			m_DX05.iUnloadStage1SlaveIn = TRUE;
			m_DX05.iUnloadStage1SlaveOut = FALSE;
		}
		else{
			m_DX05.iUnloadStage1SlaveIn = FALSE;
			m_DX05.iUnloadStage1SlaveOut = TRUE;
		}


		if(m_DY05.oUnloadStage2MasterIn == TRUE){
			m_DX05.iUnloadStage2MasterIn = TRUE;
			m_DX05.iUnloadStage2MasterOut = FALSE;
		}
		else{
			m_DX05.iUnloadStage2MasterIn = FALSE;
			m_DX05.iUnloadStage2MasterOut = TRUE;
		}

		if(m_DY05.oUnloadStage2SlaveIn == TRUE){
			m_DX05.iUnloadStage2SlaveIn = TRUE;
			m_DX05.iUnloadStage2SlaveOut = FALSE;
		}
		else{
			m_DX05.iUnloadStage2SlaveIn = FALSE;
			m_DX05.iUnloadStage2SlaveOut = TRUE;
		}

		m_DX05.iTransStageClamp1On = m_DY05.oTransStageClamp1On;
		m_DX05.iTransStageClamp1Off = m_DY05.oTransStageClamp1Off;
		m_DX05.iTransStageClamp2On = m_DY05.oTransStageClamp2On;
		m_DX05.iTransStageClamp2Off = m_DY05.oTransStageClamp2Off;

		m_DX05.iTransStageVac01 = m_DY05.oTransStageVac01;
		m_DX05.iTransStageVac02 = m_DY05.oTransStageVac02;
		m_DX05.iTransStageVac03 = m_DY05.oTransStageVac03;
		m_DX05.iTransStageVac04 = m_DY05.oTransStageVac04;
		
	}

	if (nNo==6) {
		if(m_DY06.oLoadPickerDown01 == TRUE){
			m_DX06.iLoadPickerUp01 = FALSE;
			m_DX06.iLoadPickerDown01 = TRUE;
		}
		else{
			m_DX06.iLoadPickerUp01 = TRUE;
			m_DX06.iLoadPickerDown01 = FALSE;
		}

		if(m_DY06.oLoadPickerDown02 == TRUE){
			m_DX06.iLoadPickerUp02 = FALSE;
			m_DX06.iLoadPickerDown02 = TRUE;
		}
		else{
			m_DX06.iLoadPickerUp02 = TRUE;
			m_DX06.iLoadPickerDown02 = FALSE;
		}

		if(m_DY06.oLoadPickerDown03 == TRUE){
			m_DX06.iLoadPickerUp03 = FALSE;
			m_DX06.iLoadPickerDown03 = TRUE;
		}
		else{
			m_DX06.iLoadPickerUp03 = TRUE;
			m_DX06.iLoadPickerDown03 = FALSE;
		}

		if(m_DY06.oLoadPickerDown04 == TRUE){
			m_DX06.iLoadPickerUp04 = FALSE;
			m_DX06.iLoadPickerDown04 = TRUE;
		}
		else{
			m_DX06.iLoadPickerUp04 = TRUE;
			m_DX06.iLoadPickerDown04 = FALSE;
		}

		m_DX06.iLoadPickerOpen01 = !m_DY06.oLoadPickerGrip01;
		m_DX06.iLoadPickerOpen02 = !m_DY06.oLoadPickerGrip02;
		m_DX06.iLoadPickerOpen03 = !m_DY06.oLoadPickerGrip03;
		m_DX06.iLoadPickerOpen04 = !m_DY06.oLoadPickerGrip04;
		
	}

	if (nNo==7) {
		if(m_DY07.oCapPickerDown01 == TRUE){
			m_DX07.iCapPickerUp01 = FALSE;
			m_DX07.iCapPickerDown01 = TRUE;
		}
		else{
			m_DX07.iCapPickerUp01 = TRUE;
			m_DX07.iCapPickerDown01 = FALSE;
		}
		m_DX07.iCapPickerTurn01 = m_DY07.oCapPickerTurn01;


		if(m_DY07.oCapPickerDown02 == TRUE){
			m_DX07.iCapPickerUp02 = FALSE;
			m_DX07.iCapPickerDown02 = TRUE;
		}
		else{
			m_DX07.iCapPickerUp02 = TRUE;
			m_DX07.iCapPickerDown02 = FALSE;
		}
		m_DX07.iCapPickerTurn02 = m_DY07.oCapPickerTurn02;

		if(m_DY07.oCapPickerDown03 == TRUE){
			m_DX07.iCapPickerUp03 = FALSE;
			m_DX07.iCapPickerDown03 = TRUE;
		}
		else{
			m_DX07.iCapPickerUp03 = TRUE;
			m_DX07.iCapPickerDown03 = FALSE;
		}
		m_DX07.iCapPickerTurn03 = m_DY07.oCapPickerTurn03;

		if(m_DY07.oCapPickerDown04 == TRUE){
			m_DX07.iCapPickerUp04 = FALSE;
			m_DX07.iCapPickerDown04 = TRUE;
		}
		else{
			m_DX07.iCapPickerUp04 = TRUE;
			m_DX07.iCapPickerDown04 = FALSE;
		}
		m_DX07.iCapPickerTurn04 = m_DY07.oCapPickerTurn04;

		m_DX07.iCapPickerVac01 = m_DY07.oCapPickerVac01;
		m_DX07.iCapPickerVac02 = m_DY07.oCapPickerVac02;
		m_DX07.iCapPickerVac03 = m_DY07.oCapPickerVac03;
		m_DX07.iCapPickerVac04 = m_DY07.oCapPickerVac04;

	}

	if (nNo==8) {
		if(m_DY08.oAssyPickerDown01 == TRUE){
			m_DX08.iAssyPickerUp01 = FALSE;
			m_DX08.iAssyPickerDown01 = TRUE;
		}
		else{
			m_DX08.iAssyPickerUp01 = TRUE;
			m_DX08.iAssyPickerDown01 = FALSE;
		}

		if(m_DY08.oAssyPickerDown02 == TRUE){
			m_DX08.iAssyPickerUp02 = FALSE;
			m_DX08.iAssyPickerDown02 = TRUE;
		}
		else{
			m_DX08.iAssyPickerUp02 = TRUE;
			m_DX08.iAssyPickerDown02 = FALSE;
		}

		if(m_DY08.oAssyPickerDown03 == TRUE){
			m_DX08.iAssyPickerUp03 = FALSE;
			m_DX08.iAssyPickerDown03 = TRUE;
		}
		else{
			m_DX08.iAssyPickerUp03 = TRUE;
			m_DX08.iAssyPickerDown03 = FALSE;
		}

		if(m_DY08.oAssyPickerDown04 == TRUE){
			m_DX08.iAssyPickerUp04 = FALSE;
			m_DX08.iAssyPickerDown04 = TRUE;
		}
		else{
			m_DX08.iAssyPickerUp04 = TRUE;
			m_DX08.iAssyPickerDown04 = FALSE;
		}


		if(m_DY08.oAssyPickerTiltDown == TRUE){
			m_DX08.iAssyPickerTiltUp = FALSE;
			m_DX08.iAssyPickerTiltDown = TRUE;
		}
		else{
			m_DX08.iAssyPickerTiltUp = TRUE;
			m_DX08.iAssyPickerTiltDown = FALSE;
		}
		
		m_DX08.iAssyPickerVac01 = m_DY08.oAssyPickerVac01;
		m_DX08.iAssyPickerVac02 = m_DY08.oAssyPickerVac02;
		m_DX08.iAssyPickerVac03 = m_DY08.oAssyPickerVac03;
		m_DX08.iAssyPickerVac04 = m_DY08.oAssyPickerVac04;


	}

	if (nNo==9) {
		if(m_DY09.oCapBufferAlignIn == TRUE){
			m_DX09.iCapBufferStageAlignIn = TRUE;
			m_DX09.iCapBufferStageAlignOut = FALSE;
		}
		else{
			m_DX09.iCapBufferStageAlignIn = FALSE;
			m_DX09.iCapBufferStageAlignOut = TRUE;
		}

		if(m_DY09.oCapCleanerUp == TRUE){
			m_DX09.iCapCleanerDown = FALSE;
			m_DX09.iCapCleanerUp = TRUE;
		}
		else{
			m_DX09.iCapCleanerDown = TRUE;
			m_DX09.iCapCleanerUp = FALSE;
		}

	}

	if (nNo==10) {
		if(m_DY10.oUnloadPickerDown01 == TRUE){
			m_DX10.iUnloadPickerUp01 = FALSE;
			m_DX10.iUnloadPickerDown01 = TRUE;
		}
		else{
			m_DX10.iUnloadPickerUp01 = TRUE;
			m_DX10.iUnloadPickerDown01 = FALSE;
		}

		if(m_DY10.oUnloadPickerDown02 == TRUE){
			m_DX10.iUnloadPickerUp02 = FALSE;
			m_DX10.iUnloadPickerDown02 = TRUE;
		}
		else{
			m_DX10.iUnloadPickerUp02 = TRUE;
			m_DX10.iUnloadPickerDown02 = FALSE;
		}

		if(m_DY10.oUnloadPickerDown03 == TRUE){
			m_DX10.iUnloadPickerUp03 = FALSE;
			m_DX10.iUnloadPickerDown03 = TRUE;
		}
		else{
			m_DX10.iUnloadPickerUp03 = TRUE;
			m_DX10.iUnloadPickerDown03 = FALSE;
		}

		if(m_DY10.oUnloadPickerDown04 == TRUE){
			m_DX10.iUnloadPickerUp04 = FALSE;
			m_DX10.iUnloadPickerDown04 = TRUE;
		}
		else{
			m_DX10.iUnloadPickerUp04 = TRUE;
			m_DX10.iUnloadPickerDown04 = FALSE;
		}

		m_DX10.iUnloadPickerVac01 = m_DY10.oUnloadPickerVac01;
		m_DX10.iUnloadPickerVac02 = m_DY10.oUnloadPickerVac02;
		m_DX10.iUnloadPickerVac03 = m_DY10.oUnloadPickerVac03;
		m_DX10.iUnloadPickerVac04 = m_DY10.oUnloadPickerVac04;
	}

	if (nNo==11) {
		///////////////
		if(m_DY11.oIndexLoadAlignOut == TRUE){
			m_DX11.iIndexLoadAlignOut = TRUE;
			m_DX11.iIndexLoadAlignIn = FALSE;
		}
		else{
			m_DX11.iIndexLoadAlignOut = FALSE;
			m_DX11.iIndexLoadAlignIn = TRUE;
		}
		//////////////
		if(m_DY11.oIndexLoadVacUp == TRUE){
			m_DX11.iIndexLoadVacUp = TRUE;
			m_DX11.iIndexLoadVacDown = FALSE;
		}
		else{
			m_DX11.iIndexLoadVacUp = FALSE;
			m_DX11.iIndexLoadVacDown = TRUE;
		}
		/////////////
		if(m_DY11.oIndexAssyAlignOut == TRUE){
			m_DX11.iIndexAssyAlignOut = TRUE;
			m_DX11.iIndexAssyAlignIn = FALSE;
		}
		else{
			m_DX11.iIndexAssyAlignOut = FALSE;
			m_DX11.iIndexAssyAlignIn = TRUE;
		}

		/////////////
		if(m_DY11.oIndexAssyVacUp == TRUE){
			m_DX11.iIndexAssyVacUp = TRUE;
			m_DX11.iIndexAssyVacDown = FALSE;
		}
		else{
			m_DX11.iIndexAssyVacUp = FALSE;
			m_DX11.iIndexAssyVacDown = TRUE;
		}

		/////////////
		if(m_DY11.oIndexTransAlignOut == TRUE){
			m_DX11.iIndexTransAlignIn = FALSE;
			m_DX11.iIndexTransAlignOut = TRUE;
		}
		else{
			m_DX11.iIndexTransAlignIn = TRUE;
			m_DX11.iIndexTransAlignOut = FALSE;
		}

		m_DX11.iIndexLoadVac01 = m_DY11.oIndexLoadVac01;
		m_DX11.iIndexLoadVac02 = m_DY11.oIndexLoadVac02;
		m_DX11.iIndexLoadVac03 = m_DY11.oIndexLoadVac03;
		m_DX11.iIndexLoadVac04 = m_DY11.oIndexLoadVac04;

	}

	if (nNo==12) {
		m_DX12.iStartSw		 = bF;
		m_DX12.iStopSw		 = bF;
		m_DX12.iResetSw		 = bF;

	}

	if (nNo==13) {
		m_DX13.iDoor01Open = m_DY13.oDoor01Unlock;
		m_DX13.iDoor02Open = m_DY13.oDoor02Unlock;
		m_DX13.iDoor03Open = m_DY13.oDoor03Unlock;
		m_DX13.iDoor04Open = m_DY13.oDoor04Unlock;
		m_DX13.iDoor05Open = m_DY13.oDoor05Unlock;
		m_DX13.iDoor06Open = m_DY13.oDoor06Unlock;
		m_DX13.iDoor07Open = m_DY13.oDoor07Unlock;
		m_DX13.iDoor08Open = m_DY13.oDoor08Unlock;
		m_DX13.iDoor09Open = m_DY13.oDoor09Unlock;
		m_DX13.iDoor10Open = m_DY13.oDoor10Unlock;
		m_DX13.iDoor11Open = m_DY13.oDoor11Unlock;
		m_DX13.iDoor12Open = m_DY13.oDoor12Unlock;
		m_DX13.iDoor13Open = m_DY13.oDoor13Unlock;
	}

	
}
