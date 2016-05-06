// enumWindows.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "enumWindows.h"
#include <stdio.h>

#define MAX_LOADSTRING 100

typedef struct ctrl_btn{
	HANDLE hButton;
	wchar_t buff[64];
}CTRLBTN;

CTRLBTN ctrlBtnStru[3] = { 0 };
static int loop = 0;

BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam);

char* THCAR2char(TCHAR* tchStr)
{
	int iLen = 2 * wcslen(tchStr);//CString,TCHAR汉字算一个字符，因此不用普通计算长度 
	char* chRtn = new char[iLen + 1];
	wcstombs(chRtn, tchStr, iLen + 1);
	return chRtn;
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{

	HWND hWnd = ::FindWindow(_T("#32770"), _T("Windows 安全"));
	::EnumChildWindows(hWnd, EnumChildProc, 0);
	//SetFocus(hWnd);
	//SetForegroundWindow(hWnd);
	HWND hBwnd = ::FindWindowEx(hWnd, NULL, _T("DirectUIHWND"), NULL);
	//SetForegroundWindow(hBwnd);
	//SetFocus(hWnd);
	//DWORD e;
	//LPARAM lparam = MAKELPARAM(60, 140);
	//LRESULT result;
	//= ::SendMessage(hBwnd, WM_SETCURSOR, 0, lparam);
	//::PostMessage(hBwnd, WM_MOUSEMOVE, 0, lparam);
	//::SendMessage(hBwnd, WM_ACTIVATE, HTCLIENT, WM_LBUTTONDOWN);
	//::SendMessage(hBwnd, WM_SETCURSOR, HTCLIENT, WM_LBUTTONDOWN);
	//ChangeWndMessageFilter(WM_LBUTTONDOWN, TRUE);
	//ChangeWndMessageFilter(WM_LBUTTONUP, TRUE);
	//result = ::PostMessage(hBwnd, WM_LBUTTONDOWN, MK_LBUTTON, lparam);
	//result = ::PostMessage(hBwnd, WM_LBUTTONUP, 0, lparam);
	//e = GetLastError();
	//result = ::PostMessage(hBwnd, WM_LBUTTONDOWN, MK_LBUTTON, lparam);
	//result = ::PostMessage(hBwnd, WM_LBUTTONUP, 0, lparam);
	TCHAR szFilePath[MAX_PATH + 1];
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	(_tcsrchr(szFilePath, _T('\\')))[1] = 0;
	//FILE *file;
	//file = fopen();
	TCHAR filename[32] = _T("trace.log");
	_tcscat_s(szFilePath, MAX_PATH+1, filename);

	//FILE* file;
	//file = fopen(THCAR2char(szFilePath), "w+");

	char* p;
	for (int i = 0; i < 3; i++) {
		if (wcscmp(ctrlBtnStru[i].buff, _T("安装(&I)")) == 0) {
		::SendMessage((HWND)ctrlBtnStru[i].hButton, BM_CLICK, 0, 0);
		p = "using first\n";
		//fwrite(p, 1, strlen(p), file);
		}
		if (wcscmp(ctrlBtnStru[i].buff, _T("始终安装此驱动程序软件(&I)")) == 0) {
			::SendMessage((HWND)ctrlBtnStru[i].hButton, BM_CLICK, 0, 0);
			p = "using second\n";
			//fwrite(p, 1, strlen(p), file);
		}
	}
	//fclose(file);
	return 0;
}

BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam)
{
	wchar_t buff[64] = { 0 };
	GetClassName(hWnd, buff, 63);

	if (wcscmp(_T("Button"), buff) == 0) {
		ctrlBtnStru[loop].hButton = hWnd;
		GetWindowText(hWnd, buff, 63);
		memcpy(ctrlBtnStru[loop].buff, buff, 64);
		loop += 1;
	}

	return true;
}