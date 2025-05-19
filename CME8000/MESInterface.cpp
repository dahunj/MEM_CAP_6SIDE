// MESInterface.cpp : 구현 파일
//
#include "stdafx.h"
#include "CME8000.h"
#include "MESInterface.h"
#include <vector>
#include "io.h"

#include "LogFile.h"
#include "Common.h"
#include "WorkDlg.h"
//#include "DataManager.h"
//#include "CME8000Dlg.h"

#define MES_FOLDER_VALIDATION	"D:\\MES\\VALIDATION\\"
#define MES_FOLDER_LOG			"D:\\MES\\LOG\\"
#define MES_FOLDER_STATUS		"D:\\MES\\STATUS\\"

#define MES_FOLDER_RMS			"Z:\\MES\\Recipe\\"	// 네트워크 드라이브(Z:), Handler PC에 있는 파일에 저장한다.
#define MES_TIME_OUT			200	//200*50=20000(20초)

CMESInterface	 g_objMES;

CCriticalSection g_csMesThread;
CCriticalSection g_csMesLog;
CCriticalSection g_csMesUse;

CMESInterface::CMESInterface(void)
{
	m_pThreadMESCap = NULL;
	m_pThreadMESShip = NULL;

	m_bThreadMESCap = FALSE;
	m_bThreadMESShip = FALSE;
}

CMESInterface::~CMESInterface(void)
{
}

void CMESInterface::Initialize(BOOL bMESUse)
{
	MakeFolder(MES_FOLDER_VALIDATION);
	MakeFolder(MES_FOLDER_LOG);
	MakeFolder(MES_FOLDER_STATUS);

	g_objMES.m_nMESCapSequence = 0;
	g_objMES.m_nMESShipSequence = 0;

	Set_MESUse(bMESUse);	// On-Off Line Set
	//Set_Status(3);			// Idle Set
	Clear_Result();			// Validateion 파일 삭제
}

void CMESInterface::Create_Folder(CString sPath)
{
	if (sPath == _T("")) return;
	if (sPath.Right(1) == _T("\\")) sPath = sPath.Left(sPath.GetLength() - 1);
	if (GetFileAttributes(sPath) != -1) return;

	int nFound = sPath.ReverseFind('\\');
	Create_Folder(sPath.Left(nFound));

	CreateDirectory(sPath, NULL);
}

void CMESInterface::MakeFolder(CString sPath)	//"D:\\Run\\Log\\LotData\\YYYY\\MM\\DD\\";
{
	CString sTemp;
	int nIdx = 0;

	while (TRUE) {
		nIdx = sPath.Find("\\");
		sTemp += sPath.Mid(0, nIdx) + "\\";
		CreateDirectory(sTemp,FALSE);
		sPath = sPath.Mid(nIdx+1, sPath.GetLength());
		if (nIdx < 0) break;
	}
}

///////////////////////////////////////////////////////////////////////////////

void CMESInterface::Clear_Result()
{
	g_objLogFile.Save_HandlerLog("[MES] Clear_Result()");

	m_sMESResult = m_sMESLotID = m_sReasonCode = m_sReasonText = "";
	m_nMESCount = m_nReadCnt = 0;

	CString strPath;
	strPath.Format("%s*.*", MES_FOLDER_VALIDATION);

	CFileFind finder;
	BOOL bRes = finder.FindFile(strPath);

	while (bRes) {
		bRes = finder.FindNextFile();
		if (finder.IsDirectory()) continue;

		CString strFile = finder.GetFilePath();

// 		// Backup Test ////////////////////////////////////////////////////////
// 		CString strTime, strName;
// 		SYSTEMTIME time;
// 		GetLocalTime(&time);
// 		strTime.Format("%02d%02d%02d%03d", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
// 
// 		strName.Format("%s_%s", strTime, finder.GetFileName());
// 		CopyFile(strFile, MES_FOLDER_TEMP + strName, FALSE);
// 
// 		CString strLog;
// 		strLog.Format("[MES] Backup - %s", strFile);
// 		g_objLogFile.Save_HandlerLog(strLog);
		///////////////////////////////////////////////////////////////////////

		DeleteFile(strFile);
	}
}

void CMESInterface::Read_Result(int nType)
{
	CString strLog, strFile, strPath, strData, sDataA, sResult[10];

	strLog.Format("[MES] Read_Result(%d)", nType);
	g_objLogFile.Save_HandlerLog(strLog);

	strPath.Format("%s*.*", MES_FOLDER_VALIDATION);

	CFileFind finder;
	BOOL bRes = finder.FindFile(strPath);

	while (bRes) {
		bRes = finder.FindNextFile();
		if (finder.IsDirectory()) continue;

		strFile = finder.GetFilePath();
		strLog.Format("[MES] Read - %s", strFile);
		g_objLogFile.Save_HandlerLog(strLog);
		break;
	}

	int nlen = strFile.GetLength();
	if (strFile.GetLength() < 4) return;

	CFile file;
	if (!file.Open(strFile, CFile::modeRead)) return;

	int nSize = (int)file.GetLength();
	char *pBuff = new char[nSize + 1];
	pBuff[nSize] = '\0';

	file.Read(pBuff, nSize);
	strData.Format("%s", pBuff);
	strData.Replace("\r\n", ",");

	file.Close();
	delete pBuff;

	char chSepA = '=';
	char chSepB = ',';
	for (int i = 0; i < 8; i++) {
		AfxExtractSubString(sDataA, strData, i, chSepA);
		if (i == 0) continue;
		AfxExtractSubString(sResult[i-1], sDataA, 0, chSepB);
	}

	m_sMESResult = sResult[1];
	m_sReasonCode = sResult[2];
	m_sReasonText.Empty();
	m_sReasonText.Insert(0, UTF8ToANSI(sResult[3]));
	m_sMESLotID = sResult[4];
	m_nMESCount = atoi(sResult[6]);

	if (nType == 1)	g_objMES.m_bThreadMESCap = FALSE;
	if (nType == 2)	g_objMES.m_bThreadMESShip = FALSE;
}

char* CMESInterface::UTF8ToANSI(const char *pszCode)
{
	int nLength = MultiByteToWideChar(CP_UTF8, 0, pszCode, lstrlen(pszCode) + 1, NULL, NULL);
	BSTR bstrWide = SysAllocStringLen(NULL, nLength);

	MultiByteToWideChar(CP_UTF8, 0, pszCode, lstrlen(pszCode) + 1, bstrWide, nLength);

	nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
	char* pszAnsi = new char[nLength];

	WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);
	SysFreeString(bstrWide);

	return pszAnsi;
}

///////////////////////////////////////////////////////////////////////////////

void CMESInterface::Set_MESUse(BOOL bMESUse)
{
	g_csMesUse.Lock();

	m_bMESUse = bMESUse;

	SYSTEMTIME time;
	GetLocalTime(&time);

	CString strPath, strFile, strSave;
	strPath.Format("%s%04d%02d%02d", MES_FOLDER_STATUS, time.wYear, time.wMonth, time.wDay);
	strFile.Format("%s\\ValidationSet_%04d%02d%02d.txt", strPath, time.wYear, time.wMonth, time.wDay);
	Create_Folder(strPath);

	CFile file;
	if (file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite)) {
		try {
			file.SeekToEnd();

			int nState = (bMESUse ? 1 : 0);
			strSave.Format("[%04d/%02d/%02d %02d:%02d:%02d],CONTROL_STATE=%d\r\n",
				time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, nState);

			file.Write(strSave, strSave.GetLength());
			file.Close();

		} catch (CFileException *pEx) {
			pEx->Delete();
		}
	}

	g_csMesUse.Unlock();
}

void CMESInterface::Set_Status(int nStste)	//1:Run, 2:Stop, 3:Idle
{
	if (!g_objMES.m_bMESUse) return;

	g_csMesLog.Lock();

	SYSTEMTIME time;
	GetLocalTime(&time);

	CString strPath, strFile, strSave;
	strPath.Format("%s%04d%02d%02d", MES_FOLDER_LOG, time.wYear, time.wMonth, time.wDay);
	strFile.Format("%s\\%04d%02d%02d.txt", strPath, time.wYear, time.wMonth, time.wDay);
	Create_Folder(strPath);

	CFile file;
	if (file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite)) {
		try {
			file.SeekToEnd();

			CString strState = (nStste == 1 ? "RUN" : (nStste == 2 ? "STOP" : "IDLE"));
			strSave.Format("[%04d/%02d/%02d %02d:%02d:%02d],StateChange,UNITID=1000,WorkMode=%s\r\n",
				time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, strState);
			file.Write(strSave, strSave.GetLength());

			file.Close();

		} catch (CFileException *pEx) {
			pEx->Delete();
		}
	}

	g_csMesLog.Unlock();
}

void CMESInterface::Save_AviRmsData(CString sKey, CString sValue)
{
	CString strFile = (CString)MES_FOLDER_RMS + "Current_Recipe.txt";

	CIniFileCS INI(strFile);
	if (!INI.Check_File()) return;

	INI.Set_String("Current_Recipe", "Recipe_Name_Handler", gData.sRecipe);	// Model
	INI.Set_String("Current_Recipe", sKey, sValue);	// Change Data
}

///////////////////////////////////////////////////////////////////////////////

void CMESInterface::Set_CapLotChangeRequest(CString sLotID, CString sOperID)
{
	char chSep = '/';
	CString strPart, strCapCnt, strLotId;

	AfxExtractSubString(strPart, sLotID, 0, chSep);	  strPart.Trim();
	AfxExtractSubString(strCapCnt, sLotID, 1, chSep); strCapCnt.Trim();
	AfxExtractSubString(strLotId, sLotID, 2, chSep);  strLotId.Trim();

	m_sOperPartNo = strPart; m_sOperLotID = strLotId; m_nOperCount = atoi(strCapCnt); m_sOperOpID = sOperID;

	if (!m_pThreadMESCap) {
		m_bThreadMESCap = TRUE;
		m_pThreadMESCap = AfxBeginThread(Thread_MESCap, NULL);
	}

	g_csMesLog.Lock();

	SYSTEMTIME time;
	GetLocalTime(&time);

	CString strPath, strFile, strSave, sState;
	strPath.Format("%s%04d%02d%02d", MES_FOLDER_LOG, time.wYear, time.wMonth, time.wDay);
	strFile.Format("%s\\%04d%02d%02d.txt", strPath, time.wYear, time.wMonth, time.wDay);

	CString strPath2, strFile2;
	strPath2= "D:\\EVMS\\TP\\Backup";
	strFile2.Format("%s\\%04d%02d%02d.txt", strPath2, time.wYear, time.wMonth, time.wDay);


	Create_Folder(strPath);
	Create_Folder(strPath2);

	CFile file;
	if (file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite)) {
		try {
			file.SeekToEnd();

			// MTRLTYPE=KMODC01: Cap, ChangeCode=1: 재료소진
			sState = "MaterialExchangeConfirmRequest";
			strSave.Format("%04d-%02d-%02d %02d:%02d:%02d,LOTSTATUS=%s,MTRLTYPE=KMODC01,PARTNO=%s,MLOTID=%s,SLOT=1,QUANTITY=%d,CHANGECODE=#0001,OPID=%s,APPLY_AMOUNT=0\r\n",
				time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond,
				sState, m_sOperPartNo, m_sOperLotID, m_nOperCount, m_sOperOpID);
			file.Write(strSave, strSave.GetLength());

			file.Close();

			CopyFile(strFile, strFile2, FALSE);

		} catch (CFileException *pEx) {
			pEx->Delete();
		}
	}
	g_objMES.m_nMESCapSequence = 1;	// Start

	g_csMesLog.Unlock();
}

// Cap Thread Function
UINT CMESInterface::Thread_MESCap(LPVOID lpVoid)
{
	g_csMesThread.Lock();

	g_objLogFile.Save_HandlerLog("[MES] Thread_MESCap()");

	g_objMES.m_nReadCnt = 0;

	while (g_objMES.m_bThreadMESCap) {
		Sleep(100);
		g_objMES.Read_Result(1);
		g_objMES.m_nReadCnt++;
		if (g_objMES.m_nReadCnt >= MES_TIME_OUT) break;
	}

	if (g_objMES.m_nReadCnt >= MES_TIME_OUT) {	// Time Over
		g_objMES.Clear_Result();				// 수신 Folder All File 삭제
		g_objMES.m_nMESCapSequence = 0;
		g_objMES.m_pThreadMESCap = NULL;
		g_objCommon.Show_Error(9011);
		g_csMesThread.Unlock();
		return 0;
	}

	if (g_objMES.m_sMESResult == "1") {		// Lot 취소
		g_objMES.Clear_Result();			// 수신 Folder All File 삭제
		g_objMES.m_nMESCapSequence = 0;
		g_objMES.m_pThreadMESCap = NULL;
		g_objCommon.Show_Error(9012);
		g_csMesThread.Unlock();
		return 0;
	}

	g_objMES.m_nMESCapSequence = 2;			// OK
	g_objMES.m_bThreadMESCap = FALSE;
	g_objMES.m_pThreadMESCap = NULL;

	g_objMES.Clear_Result();				// 수신 Folder All File 삭제

	g_csMesThread.Unlock();

	return 0;
}

///////////////////////////////////////////////////////////////////////////////

void CMESInterface::Set_ShipLotChangeRequest(CString sLotID, CString sOperID)
{
	char chSep = '/';
	CString strPart, strShipCnt, strLotId;

	AfxExtractSubString(strPart, sLotID, 0, chSep);	   strPart.Trim();
	AfxExtractSubString(strShipCnt, sLotID, 1, chSep); strShipCnt.Trim();
	AfxExtractSubString(strLotId, sLotID, 2, chSep);   strLotId.Trim();

	m_sOperPartNo = strPart; m_sOperLotID = strLotId; m_nOperCount = atoi(strShipCnt); m_sOperOpID = sOperID;

	if (!m_pThreadMESShip) {
		m_bThreadMESShip = TRUE;
		m_pThreadMESShip = AfxBeginThread(Thread_MESShip, NULL);
	}

	g_csMesLog.Lock();

	SYSTEMTIME time;
	GetLocalTime(&time);

	CString strPath, strFile, strSave, sState;
	strPath.Format("%s%04d%02d%02d", MES_FOLDER_LOG, time.wYear, time.wMonth, time.wDay);
	strFile.Format("%s\\%04d%02d%02d.txt", strPath, time.wYear, time.wMonth, time.wDay);

	CString strPath2, strFile2;
	strPath2 ="D:\\EVMS\\TP\\Backup";
	strFile2.Format("%s\\%04d%02d%02d.txt", strPath2, time.wYear, time.wMonth, time.wDay);


	Create_Folder(strPath);

	CFile file;
	if (file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite)) {
		try {
			file.SeekToEnd();

			// MTRLTYPE=KSHTR01: Cap, ChangeCode=1: 재료소진
			sState = "MaterialExchangeConfirmRequest";
			strSave.Format("%04d-%02d-%02d %02d:%02d:%02d,LOTSTATUS=%s,MTRLTYPE=KSHTR01,PARTNO=%s,MLOTID=%s,SLOT=1,QUANTITY=%d,CHANGECODE=#0001,OPID=%s,APPLY_AMOUNT=0\r\n",
				time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond,
				sState, m_sOperPartNo, m_sOperLotID, m_nOperCount, m_sOperOpID);
			file.Write(strSave, strSave.GetLength());

			file.Close();

			CopyFile(strFile,strFile2, FALSE);

		} catch (CFileException *pEx) {
			pEx->Delete();
		}
	}
	g_objMES.m_nMESShipSequence = 1;	// Start

	g_csMesLog.Unlock();
}

// Ship Thread Function
UINT CMESInterface::Thread_MESShip(LPVOID lpVoid)
{
	g_csMesThread.Lock();

	g_objLogFile.Save_HandlerLog("[MES] Thread_MESShip()");

	g_objMES.m_nReadCnt = 0;

	while (g_objMES.m_bThreadMESShip) {
		Sleep(100);
		g_objMES.Read_Result(2);
		g_objMES.m_nReadCnt++;
		if (g_objMES.m_nReadCnt >= MES_TIME_OUT) break;
	}

	if (g_objMES.m_nReadCnt >= MES_TIME_OUT) {	// Time Over
		g_objMES.Clear_Result();				// 수신 Folder All File 삭제
		g_objMES.m_nMESShipSequence = 0;
		g_objMES.m_pThreadMESShip = NULL;
		g_objCommon.Show_Error(9021);
		g_csMesThread.Unlock();
		return 0;
	}

	if (g_objMES.m_sMESResult == "1") {		// Lot 취소
		g_objMES.Clear_Result();			// 수신 Folder All File 삭제
		g_objMES.m_nMESShipSequence = 0;
		g_objMES.m_pThreadMESShip = NULL;
		g_objCommon.Show_Error(9022);
		g_csMesThread.Unlock();
		return 0;
	}

	g_objMES.m_nMESShipSequence = 2;			// OK
	g_objMES.m_bThreadMESShip = FALSE;
	g_objMES.m_pThreadMESShip = NULL;

	g_objMES.Clear_Result();				// 수신 Folder All File 삭제

	g_csMesThread.Unlock();

	return 0;
}
