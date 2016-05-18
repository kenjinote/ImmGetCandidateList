#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "imm32")

#include <windows.h>

TCHAR szClassName[] = TEXT("Window");
HWND hList;
WNDPROC DefaultEditWndProc;

LRESULT CALLBACK MyEditProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_IME_SETCONTEXT:
		if ((lParam & ISC_SHOWUICANDIDATEWINDOW) > 0)
		{
			lParam &= ~ISC_SHOWUICANDIDATEWINDOW;
		}
		break;
	case WM_APP:
		{
			SendMessage(hList, LB_RESETCONTENT, 0, 0);
			HIMC hImc = ImmGetContext(hWnd);
			if (hImc)
			{
				DWORD dwSize = ImmGetCandidateList(hImc, 0, NULL, 0);
				if (dwSize)
				{
					HGLOBAL hMem = GlobalAlloc(GHND, dwSize);
					if (hMem)
					{
						LPCANDIDATELIST lpCandidateList = (LPCANDIDATELIST)GlobalLock(hMem);
						ImmGetCandidateListW(hImc, 0, lpCandidateList, dwSize);
						for (DWORD i = 0; i < lpCandidateList->dwCount; ++i)
						{
							SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)((LPBYTE)lpCandidateList + lpCandidateList->dwOffset[i]));
						}
						GlobalFree(hMem);
					}
				}
				ImmReleaseContext(hWnd, hImc);
			}
		}
		break;
	case WM_IME_NOTIFY:
		switch (wParam)
		{
		case IMN_OPENCANDIDATE:
		case IMN_CHANGECANDIDATE:
			SendMessage(hWnd, WM_APP, 0, 0);
			break;
		case IMN_CLOSESTATUSWINDOW:
		case IMN_CLOSECANDIDATE:
			SendMessage(hList, LB_RESETCONTENT, 0, 0);
			break;
		}
		break;
	default:
		break;
	}
	return CallWindowProc(DefaultEditWndProc, hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hEdit;
	switch (msg)
	{
	case WM_CREATE:
		hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hList = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), 0, WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOINTEGRALHEIGHT, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		DefaultEditWndProc = (WNDPROC)SetWindowLongPtr(hEdit, GWLP_WNDPROC, (LONG_PTR)MyEditProc);
		break;
	case WM_SIZE:
		MoveWindow(hEdit, 10, 10, 256, 32, TRUE);
		MoveWindow(hList, 10, 50, LOWORD(lParam) - 20, HIWORD(lParam) - 60, TRUE);
		break;
	case WM_DESTROY:
		SetWindowLongPtr(hEdit, GWLP_WNDPROC, (LONG_PTR)DefaultEditWndProc);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("IME‚Ì•ÏŠ·Œó•âƒŠƒXƒg‚ðŽæ“¾"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
