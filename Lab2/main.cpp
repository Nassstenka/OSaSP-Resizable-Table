#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
TCHAR WinName[] = _T("MainFrame");

int APIENTRY _tWinMain(HINSTANCE This, HINSTANCE Prev, LPTSTR cmd, int mode) {
	HWND hWnd;
	MSG msg;
	WNDCLASS wc;
	wc.hInstance = This;
	wc.lpszClassName = WinName;
	wc.lpfnWndProc = WndProc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	if (!RegisterClass(&wc)) return 0;
	hWnd = CreateWindow(WinName, _T("Лабораторная работа 2"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		1000, 500, HWND_DESKTOP, NULL, This, NULL);
	ShowWindow(hWnd, mode);
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

std::vector<std::string> readTextFromFile() {
	std::ifstream ifs("test.txt");
	std::string v;
	std::vector<std::string> lines;
	while (std::getline(ifs, v))
	{
		lines.push_back(v);
	}
	return lines;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc;
	RECT clientRect;
	const int columnNumber = 5;
	static HFONT hfont;
	static std::vector<std::string> stringArr;
	static int clientHeight, clientWidth;
	static int sx, sy;
	static int x, y;

	switch (message) {
	case WM_DESTROY: {
		PostQuitMessage(0);
		DeleteObject(hfont);
		break;
	}
	case WM_CREATE: {
		stringArr = readTextFromFile();
		hfont = CreateFont(0, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, RUSSIAN_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, NULL);
		break;
	}
	case WM_SIZE: {
		GetClientRect(hWnd, &clientRect);
		clientHeight = clientRect.bottom;
		clientWidth = clientRect.right;
		sx = clientWidth / columnNumber;
		break;
	}
	case WM_ERASEBKGND: {
		return (LRESULT)1;
	}
	case WM_PAINT: {
		HDC memDc;
		HBITMAP memBmp;
		HBRUSH hbrBkgnd;
		GetClientRect(hWnd, &clientRect);
		clientHeight = clientRect.bottom;
		clientWidth = clientRect.right;
		hdc = BeginPaint(hWnd, &ps);
		memDc = CreateCompatibleDC(hdc);
		memBmp = CreateCompatibleBitmap(hdc, clientWidth, clientHeight);
		HBITMAP oldBmp = (HBITMAP)SelectObject(memDc, memBmp);
		hbrBkgnd = CreateSolidBrush(RGB(255, 255, 255));
		FillRect(memDc, &clientRect, hbrBkgnd);
		DeleteObject(hbrBkgnd);
		SetTextColor(memDc, RGB(0, 0, 0));
		SetBkMode(memDc, TRANSPARENT);
		SelectObject(memDc, hfont);
		int startX = 0, startY = 0;;
		int colCounter = 0;
		int i = 0;
		int maxLineHeight = 0;
		RECT textRect;
		for (int j = 0; j <= columnNumber; j++)
		{
			MoveToEx(memDc, startX, 0, NULL);
			LineTo(memDc, startX, clientHeight);
			startX += sx;
		}
		startX = 0;
		while (i < stringArr.size())
		{
			if (colCounter == columnNumber)
			{ 
				startY += maxLineHeight + 20;
				MoveToEx(memDc, 0, startY, NULL);
				LineTo(memDc, clientWidth, startY);
				if (i != stringArr.size() - 1)
				{
					colCounter = 0;
					maxLineHeight = 0;
					startX = 0;
				}
			}
			const CHAR* str = stringArr[i].c_str();
			SetRect(&textRect, startX, startY, startX + sx, NULL);
			int lineHeight = DrawText(memDc, (LPCSTR)str, strlen(str), &textRect, DT_CENTER | DT_EDITCONTROL | DT_WORDBREAK | DT_NOCLIP);
			if (lineHeight > maxLineHeight)
			{
				maxLineHeight = lineHeight;
			}
			colCounter++;
			startX += sx;
			i++;
		}
		BitBlt(hdc, 0, 0, clientWidth, clientHeight, memDc, 0, 0, SRCCOPY);        
		SelectObject(hdc, oldBmp);
		DeleteObject(memBmp);
		DeleteDC(memDc);
		EndPaint(hWnd, &ps);
		break;
	}
	default: return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}