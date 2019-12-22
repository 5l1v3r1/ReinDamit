#pragma once
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <TlHelp32.h>

using f_LoadLibraryA = HINSTANCE(WINAPI*)(const char * lpLibFilename);
using f_GetProcAddress = UINT_PTR(WINAPI*)(HINSTANCE hModule, const char * lpProcName);
using f_DLL_ENTRY_POINT = BOOL(WINAPI*)(void * hDll, DWORD dwReason, void * pReserved);

struct REIN_MANUELL_DATA
{
	f_LoadLibraryA ploadLibraryA;
	f_GetProcAddress pGetProcAddress; 
	HINSTANCE hMod; 
};

bool ReinManuell(HANDLE hProc, const char * szDllFile);


