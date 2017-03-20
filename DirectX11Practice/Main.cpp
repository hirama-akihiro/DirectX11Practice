//#include <d3d11.h>

#include <Windows.h>
#include <tchar.h>

#pragma once
#pragma comment(lib, "d3d11.lib")

// ウィンドウのコールバック関数
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
		PostMessage(hWnd, WM_DESTROY, 0, 0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return (DefWindowProc(hWnd, message, wParam, lParam));
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int nCmdShow)
{
	TCHAR szWindowsClass[] = "3DDISPPG";
	WNDCLASS wcex;
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowsClass;
	RegisterClass(&wcex);

	// ウィンドウをクリエイト
	HWND hWnd;
	hWnd = CreateWindowA(szWindowsClass,
		"3D Disp Pg",
		WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | TLS_MINIMUM_AVAILABLE,
		0,
		0,
		1280,
		720,
		NULL,
		NULL,
		hInst,
		NULL);

	ShowWindow(hWnd, nCmdShow);

	// メインループ
	MSG hMsg;
	while (true) {
		while (PeekMessageW(&hMsg, NULL, 0, 0, PM_REMOVE)) {
			if (hMsg.message == WM_QUIT) {
				goto END;
			}
			TranslateMessage(&hMsg);
			DispatchMessage(&hMsg);
		}
	}
END:
	return 0;
}

/*
#include <d3d11.h>

#include <windows.h>
#include <tchar.h>


//まずはウィンドウのコールバック関数を記述
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_CLOSE:
		PostMessage(hWnd, WM_DESTROY, 0, 0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(hWnd, message, wParam, lParam));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	//ウィンドウクラスを登録して
	TCHAR szWindowClass[] = "3DDISPPG";
	WNDCLASS wcex;
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	RegisterClass(&wcex);

	//ウィンドウをクリエイト
	HWND hWnd;
	hWnd = CreateWindow(szWindowClass,
		"3D Disp Pg",
		WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0,
		0,
		1020,
		1080,
		NULL,
		NULL,
		hInstance,
		NULL);

	//後で消すが，とりあえずウィンドウを出さないとわかり辛いので
	ShowWindow(hWnd, nCmdShow);

	//メインループ
	MSG hMsg;
	while (true) {
		while (PeekMessageW(&hMsg, NULL, 0, 0, PM_REMOVE)) {
			if (hMsg.message == WM_QUIT) {
				goto End;
			}
			TranslateMessage(&hMsg);
			DispatchMessage(&hMsg);
		}
	}

End:
	return 0;
}
*/