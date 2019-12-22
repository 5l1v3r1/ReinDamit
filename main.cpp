#include "rein.h"
#include <fstream>
#include <iostream>

int inject(HWND parent, char* file[], char* proc[]);

char szDllFile[300];
char szProc[100];


LRESULT CALLBACK MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int cmdShow)
{
	std::ifstream config;
	config.open("rein.conf");

	if (config.is_open()){
		if (!config.eof) {
			config >> szDllFile;
		}
		if (!config.eof) {
			config >> szProc;
		}
	}
	config.close;

	WNDCLASS wc;
	MSG msg;
	HWND hWnd;

	CHAR className[] = "text";
	wc = {};
	wc.lpfnWndProc = MessageHandler;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hInstance = hInstance;
	wc.lpszClassName = className;

	hWnd = CreateWindow(className, className, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, 0, 0, hInstance, 0);
	
	ShowWindow(hWnd, cmdShow);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);
	UpdateWindow(hWnd);

	while (true) {
		BOOL res = GetMessage(&msg, 0, 0, 0);
		if (res > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			return res;
		}
	}
}

int inject(HWND parent, char* file[], char* proc[]){
	PROCESSENTRY32 PE32{ 0 };
	PE32.dwSize = sizeof(PE32);

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnap == INVALID_HANDLE_VALUE) {
		DWORD Error = GetLastError();
		MessageBox(parent, "Error: Invalid Handle", "There was a Problem loading the process list", MB_OK | MB_ICONERROR);
		return 1;
	}
	else {
		BOOL bRet = Process32First(hSnap, &PE32);
		DWORD PID = 0; 
		while (bRet) {
			if (!strcmp(*proc, PE32.szExeFile)) {
				PID = PE32.th32ProcessID;
				break;
			}
			bRet = Process32Next(hSnap, &PE32);
		}
		CloseHandle(hSnap);
		HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
		if (hProc) {
			if (ReinManuell(hProc, *file)) {
				MessageBox(parent, "SUCCESS!", "DLL was injected", MB_OK | MB_ICONHAND);
			}
			else {
				DWORD Error = GetLastError();
				MessageBox(parent, "Error:", "Was not able to inject :'(", MB_OK | MB_ICONERROR);
			}
		}else {
			DWORD Error = GetLastError();
			MessageBox(parent, "Error:", "was not able to open the process", MB_OK | MB_ICONERROR);
		}
		CloseHandle(hProc);
	}
	return 0;
}