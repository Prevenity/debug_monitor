// debug_monitor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>

struct db_buffer
{
	DWORD   dwProcessId;
	char    data[4096 - sizeof(DWORD)];
};

static HANDLE	hMutex = NULL;

HANDLE hEventDataReady;
HANDLE hDBWINBuffer;
HANDLE hEventBufferReady;

struct db_buffer *pDBBuffer;



int _tmain(int argc, _TCHAR* argv[])
{

	DWORD errorCode = 0;


	//OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"DBWinMutex");

	/*
	SECURITY_DESCRIPTOR     sdopen;

	InitializeSecurityDescriptor(&sdopen,SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sdopen, TRUE, NULL, FALSE);

	SECURITY_ATTRIBUTES     sa;
	ZeroMemory(&sa, sizeof sa);

	sa.nLength = sizeof sa;
	sa.lpSecurityDescriptor = &sdopen;
	sa.bInheritHandle = FALSE;

	if ((hMutex = CreateMutex(&sa, FALSE, L"DBWinMutex")) == 0)
	{

	errorCode = GetLastError();
	printf("createmutex error %d", errorCode);
	return errorCode;
	}
	*/


	hEventBufferReady = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"DBWIN_BUFFER_READY");
	if (hEventBufferReady == NULL) {
		hEventBufferReady = CreateEvent(NULL, FALSE, TRUE, L"DBWIN_BUFFER_READY");

		if (hEventBufferReady == NULL) {
			errorCode = GetLastError();
			printf("buffer ready error %d", errorCode);
			return errorCode;
		}
	}


	hEventDataReady = OpenEvent(SYNCHRONIZE, FALSE, L"DBWIN_DATA_READY");
	if (hEventDataReady == NULL) {
		hEventDataReady = CreateEvent(NULL, FALSE, FALSE, L"DBWIN_DATA_READY");

		if (hEventDataReady == NULL) {
			errorCode = GetLastError();
			printf("ready data error %d", errorCode);
			return errorCode;
		}
	}


	hDBWINBuffer = OpenFileMapping(FILE_MAP_READ, FALSE, L"DBWIN_BUFFER");


	if (hDBWINBuffer == NULL) {
		hDBWINBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(struct db_buffer), L"DBWIN_BUFFER");

		if (hDBWINBuffer == NULL) {
			errorCode = GetLastError();
			printf("create file mapping error %d", errorCode);
			return errorCode;
		}
	}

	pDBBuffer = (struct db_buffer *)MapViewOfFile(hDBWINBuffer, SECTION_MAP_READ, 0, 0, 0);

	if (pDBBuffer == NULL) {
		errorCode = GetLastError();
		printf("map view of file error %d", errorCode);
		return errorCode;
	}

	bool isRunning = true;
	while (isRunning)
	{

		DWORD mb = WaitForSingleObject(hEventDataReady, INFINITE);

		if (mb == WAIT_OBJECT_0) {

			printf("%s", pDBBuffer->data);
			SetEvent(hEventBufferReady);
		}

	}

	UnmapViewOfFile(pDBBuffer);
	CloseHandle(hDBWINBuffer);
	CloseHandle(hEventBufferReady);
	CloseHandle(hEventDataReady);
	//CloseHandle(hMutex);
	//hMutex = 0;
	return 0;
}