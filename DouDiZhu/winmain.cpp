/**************************************************************\
模块：
	“斗地主”的主窗体及按钮子窗体消息处理函数
文件：
	winmain.cpp
功能：
	负责整个程序所有窗体消息处理。
作者：
	宋保明
修改历史：
	修改人	修改时间	修改内容
	-------	-----------	-------------------------------
	宋保明	2014.12.8	创建
\**************************************************************/
#include <Windows.h>
#include "winmain.h"
#include "game.h"
#include "scene.h"


PTSTR szAppName = TEXT("斗地主");
PTSTR szDataFile = TEXT("data");
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ButtonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR szCmd, int nShow)
{
	HWND hwnd;
	MSG msg;
	WNDCLASSEX wcls;
	HBITMAP hbitmap = LoadBitmap(GetModuleHandle(NULL), TEXT("background"));
	HBRUSH hbrush = CreatePatternBrush(hbitmap);
	DeleteObject(hbitmap);

	wcls.cbSize = sizeof(wcls);
	wcls.style = CS_HREDRAW | CS_VREDRAW;
	wcls.lpfnWndProc = WndProc;
	wcls.hInstance = hInst;
	wcls.cbWndExtra = 0;
	wcls.cbClsExtra = 0;
	wcls.hbrBackground = hbrush/*(HBRUSH)GetStockObject(NULL_BRUSH)*/;
	wcls.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcls.hIconSm = NULL;
	wcls.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcls.lpszClassName = szAppName;
	wcls.lpszMenuName = NULL;

	if (!RegisterClassEx(&wcls)){
		MessageBox(NULL, TEXT("注册类出错！"), TEXT("错误"), MB_ICONERROR);
		return 0;
	}

	wcls.cbWndExtra = sizeof(BOOL);
	wcls.lpfnWndProc = ButtonProc;
	wcls.lpszClassName = TEXT("mybutton");
	wcls.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);


	if (!RegisterClassEx(&wcls)){
		MessageBox(NULL, TEXT("注册类出错！"), TEXT("错误"), MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName,
		szAppName,
		WS_OVERLAPPED | WS_CAPTION | \
		WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInst, NULL);

	ShowWindow(hwnd, nShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DeleteObject(hbrush);
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	SIZE size;
	POINT point;
	PAINTSTRUCT ps;
	int caption, border;
	static Game game(hwnd);
	static Scene scene(&game);

	switch (message){
	case WM_CREATE:
		size = scene.GetSize();
		caption = GetSystemMetrics(SM_CYCAPTION);
		border = GetSystemMetrics(SM_CXBORDER);
		SetWindowPos(hwnd, NULL, 0, 0,
			size.cx + border * 10, 
			size.cy + caption + border * 10,
			SWP_NOMOVE | SWP_NOZORDER);

		game.LoadPlayerScore();
		scene.InitScene(hwnd);
		game.GameStart();
		scene.DrawBackground();
		return 0;
	case WM_TIMER:
		KillTimer(hwnd, 1);
		switch (game.GetStatus()){
		case NOTSTART:
			game.GameStart();
			break;
		case GETLANDLORD:
			game.GetLandlord();
			break;
		case SENDLANDLORDCARD:
			game.SendLandlordCard();
			break;
		case DISCARD:
			game.Discard();
			break;
		case GAMEOVER:
			game.GameOver();
			break;
		}
		return 0;
	case WM_MYBUTTON:
		switch ((Command)wParam){
		case No:
		case Score1:
		case Score2:
		case Score3:
			scene.HideQuestionBtn();
			game.SendScore(wParam - 1);
			break;
		case Discard:
			//scene.HideDiscardBtn();
			game.Discard();
			break;
		case Pass:
			scene.HideDiscardBtn();
			game.Pass();
			break;
		case Hint:
			game.Hint();
			break;
		}
		return 0;
	case WM_MOUSEMOVE:
		if (!game.IsHumanTurn() || game.GetStatus() != DISCARD)
			return 0;

		point.x = LOWORD(lParam);
		point.y = HIWORD(lParam);
		if (wParam & MK_LBUTTON)
			scene.SelectCard(point);
		else if (wParam & MK_RBUTTON)
			scene.DeleteCard(point);
		return 0;
	case WM_RBUTTONDOWN:
		if (!game.IsHumanTurn() || game.GetStatus() != DISCARD)
			return 0;
		point.x = LOWORD(lParam);
		point.y = HIWORD(lParam);
		scene.DeleteCard(point);
		return 0;
	case WM_LBUTTONDOWN:
		if (!game.IsHumanTurn() || game.GetStatus() != DISCARD)
			return 0;
		point.x = LOWORD(lParam);
		point.y = HIWORD(lParam);
		scene.SelectCard(point);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		scene.ShowScene(hdc);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		game.StorePlayerScore();
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
//显示按钮图片
void ShowImage(HWND hwnd,HDC hdc, int number)
{
	HDC hdcmem;
	TCHAR name[20];
	HBITMAP hbitmap;

	GetWindowText(hwnd, name, sizeof(name));
	hbitmap = LoadBitmap(GetModuleHandle(NULL), name);

	hdcmem = CreateCompatibleDC(NULL);
	SelectObject(hdcmem, hbitmap);

	TransparentBlt(hdc, -7, -2, 82, 40, hdcmem, 82 * number, 0, 82, 40, RGB(0, 0, 0));

	DeleteObject(hbitmap);
	DeleteDC(hdcmem);
}
//按钮窗口处理函数
LRESULT CALLBACK ButtonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	BOOL enable ;
	TRACKMOUSEEVENT tme;

	enable = GetWindowLong(hwnd, 0);

	switch (message){
	case WM_CREATE:
		SetWindowLong(hwnd, 0, TRUE);
		return 0;
	case WM_MYBUTTON:
		enable = (BOOL)wParam;
		SetWindowLong(hwnd, 0, enable);
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;
	case WM_LBUTTONDOWN:
		if (!enable)
			return 0;
		hdc = GetDC(hwnd);
		ShowImage(hwnd, hdc, 2);
		ReleaseDC(hwnd, hdc);
		return 0;
	case WM_LBUTTONUP:
		if (!enable)
			return 0;
		PostMessage(GetParent(hwnd), WM_MYBUTTON, GetWindowLong(hwnd, GWL_ID), (LPARAM)hwnd);

		hdc = GetDC(hwnd);
		ShowImage(hwnd, hdc, 1);
		ReleaseDC(hwnd, hdc);
		return 0;
	case WM_MOUSEHOVER:
	case WM_MOUSEMOVE:
		if (!enable)
			return 0;
		
		hdc = GetDC(hwnd);
		if (wParam & MK_LBUTTON)
			ShowImage(hwnd, hdc, 2);
		else
			ShowImage(hwnd, hdc, 1);
		ReleaseDC(hwnd, hdc);

		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.hwndTrack = hwnd;
		tme.dwHoverTime = 1;
		TrackMouseEvent(&tme);

		return 0;
	case WM_MOUSELEAVE:
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		if (enable)
			ShowImage(hwnd, hdc, 0);
		else
			ShowImage(hwnd, hdc, 3);

		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}