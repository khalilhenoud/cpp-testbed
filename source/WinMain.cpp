#include <windows.h>
#include <gdiplus.h>
#include "application.h"

using namespace Gdiplus;

HINSTANCE	hInst;
HWND		hWnd;
HDC			hWindowDC;

int screenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
int screenHeight = GetSystemMetrics(SM_CYFULLSCREEN);
int client_area_width = 1280;
int client_area_height = 720;

void ComputeWindowPosition(int &x, int &y, int width, int height)
{
	x = y = 0;
	if (screenWidth <= width || screenWidth <= height)
        return;

	x = (screenWidth - width)/2;
	y = (screenHeight - height)/2;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
    case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	hInst = hInstance;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;

	// Initialize GDI plus.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	WNDCLASSEX wcex = {0};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= 0;
	wcex.hCursor		= LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= "OGL";
	wcex.hIconSm		= 0;
	RegisterClassEx(&wcex);

   	RECT r = {0, 0, client_area_width, client_area_height};
	int x, y;
	ComputeWindowPosition(x, y, client_area_width, client_area_height);
	AdjustWindowRect(&r, WS_CAPTION,FALSE);
    hWnd = CreateWindow("OGL", "", WS_CAPTION, x, y, r.right - r.left, r.bottom - r.top , 0, 0, hInstance, 0);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

    hWindowDC = GetDC(hWnd);

	core::Application::GetCurrentApplication()->Initialize();

	MSG msg;
    while (true) {
		while (PeekMessage(&msg, 0, 0, 0,PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				goto End;
			else
				DispatchMessage(&msg);
		}

		core::Application::GetCurrentApplication()->Update();
    }

End:
	core::Application::GetCurrentApplication()->Cleanup();

	// Shutdown GDI plus.
	GdiplusShutdown(gdiplusToken);

	return (int) msg.wParam;
}
