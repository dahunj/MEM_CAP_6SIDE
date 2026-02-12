// CME8000.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.



#define	MODE_OPERATOR		0
#define MODE_INITIAL		1
#define MODE_WORK			2
#define MODE_MANUAL			3
#define MODE_SETUP			4
#define MODE_PROHIBIT		5
#define MODE_PARAM			6
#define MODE_ALARM			7

#define STATE_NONE			0
#define STATE_INIT			1
#define STATE_STOP			2
#define STATE_RUN			3
#define STATE_ALARM			4
#define STATE_ERROR			5
#define STATE_LOTEND		6
#define STATE_CAPTRAY		7
#define STATE_SHIPTRAY		8

// CCME8000App:
// 이 클래스의 구현에 대해서는 CME8000.cpp을 참조하십시오.
//
class CCME8000App : public CWinApp
{
public:
	CCME8000App();

// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.
	DECLARE_MESSAGE_MAP()

private:
	int m_nMainMode;
	int m_nMainState;

public:
	void Set_MainMode(int nMode) { m_nMainMode = nMode; }
	int  Get_MainMode() { return m_nMainMode; }

	void Set_MainState(int nState) { m_nMainState = nState; }
	int  Get_MainState() { return m_nMainState; }

	BOOL bParamMode;
	BOOL bIoMode;
	BOOL bAlarmMode;

	void DoEvents();
	void uSleep(int msec);

	void InstallCrashHandler();
};


extern CCME8000App theApp;

static void Create_Folder(CString sPath)
{
	if (sPath == _T("")) return;
	if (sPath.Right(1) == _T("\\")) sPath = sPath.Left(sPath.GetLength() - 1);
	if (GetFileAttributes(sPath) != -1) return;	// Directory Exist!!!

	int nFound = sPath.ReverseFind('\\');
	Create_Folder(sPath.Left(nFound));

	CreateDirectory(sPath, NULL);
}


static void MakeDumpPathA(char* outPath, size_t outSize)
{
	// exe 폴더에 dump 저장
	char exePath[MAX_PATH] = {0};
	GetModuleFileNameA(NULL, exePath, MAX_PATH);

	// 폴더만 추출
	char* p = strrchr(exePath, '\\');
	if (p) *p = 0;

	// 시간 문자열
	time_t t = time(NULL);
	struct tm tmv;
	localtime_s(&tmv, &t);

	DWORD pid = GetCurrentProcessId();

	Create_Folder("D:\\dahunj\\Temp");

	// 예: C:\...\Crash_2026-02-12_101530_PID1234.dmp
	_snprintf_s(outPath, outSize, _TRUNCATE,
		"D:\\dahunj\\Temp\\Crash_%04d-%02d-%02d_%02d%02d%02d_PID%lu.dmp",
		tmv.tm_year + 1900, tmv.tm_mon + 1, tmv.tm_mday,
		tmv.tm_hour, tmv.tm_min, tmv.tm_sec,
		(unsigned long)pid);
}

static void AppendCrashLogA(const char* msg)
{
	// 너무 복잡하게 하지 말고, 간단히 append만
	char exePath[MAX_PATH] = {0};
	GetModuleFileNameA(NULL, exePath, MAX_PATH);
	char* p = strrchr(exePath, '\\');
	if (p) *p = 0;

	// 시간 문자열
	time_t t = time(NULL);
	struct tm tmv;
	localtime_s(&tmv, &t);

	DWORD pid = GetCurrentProcessId();

	Create_Folder("D:\\dahunj\\Temp");


	char logPath[MAX_PATH] = {0};
	//_snprintf_s(logPath, MAX_PATH, _TRUNCATE, "%s\\CrashLog.txt", exePath);
	_snprintf_s(logPath, MAX_PATH, _TRUNCATE, "D:\\dahunj\\Temp\\CrashLog_%04d-%02d-%02d_%02d%02d%02d_PID%lu.txt", tmv.tm_year + 1900, tmv.tm_mon + 1, tmv.tm_mday,
		tmv.tm_hour, tmv.tm_min, tmv.tm_sec,
		(unsigned long)pid);

	FILE* fp = NULL;
	fopen_s(&fp, logPath, "a+");
	if (!fp) return;

	fprintf(fp, "%s\n", msg);
	fclose(fp);
}

static LONG WINAPI MyUnhandledExceptionFilter(EXCEPTION_POINTERS* pep)
{
	// 1) 간단 로그 먼저 (덤프가 실패해도 최소 정보 남기기)
	char buf[512] = {0};
	DWORD code = pep && pep->ExceptionRecord ? pep->ExceptionRecord->ExceptionCode : 0;
	void* addr = pep && pep->ExceptionRecord ? pep->ExceptionRecord->ExceptionAddress : 0;

	_snprintf_s(buf, sizeof(buf), _TRUNCATE,
		"[CRASH] code=0x%08lX addr=%p pid=%lu tid=%lu",
		(unsigned long)code, addr,
		(unsigned long)GetCurrentProcessId(),
		(unsigned long)GetCurrentThreadId());

	AppendCrashLogA(buf);

	// 2) 덤프 생성
	char dumpPath[MAX_PATH] = {0};
	MakeDumpPathA(dumpPath, sizeof(dumpPath));

	HANDLE hFile = CreateFileA(
		dumpPath,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION mdei;
		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = pep;
		mdei.ClientPointers = FALSE;

		// 용량이 너무 커지면 FullMemory 대신 MiniDumpWithDataSegs 등으로 낮춰도 됨
		MiniDumpWriteDump(
			GetCurrentProcess(),
			GetCurrentProcessId(),
			hFile,
			MiniDumpWithFullMemory,
			&mdei,
			NULL,
			NULL);

		CloseHandle(hFile);
	}
	else
	{
		AppendCrashLogA("[CRASH] CreateFileA failed for dump.");
	}

	// 프로세스 종료로 이어지게
	return EXCEPTION_EXECUTE_HANDLER;
}



