#include "resource.h"
#include "For_window.h"
#include "Program_Manager.h"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
	WNDCLASS win = create_win((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_HELP), hInst, LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1)), L"Egor’s Text Editor", MainProcedure);

	if (!RegisterClassW(&win))
		return -1;

	MSG msg = { 0 };

	CreateWindowW(L"Egor’s Text Editor", L"Текстовий редактор", WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_MAXIMIZE, 0, 0, get_screen_size(SM_CXSCREEN, L"Software\\TextEditor"), get_screen_size(SM_CYSCREEN, L"Software\\TextEditor"), NULL, NULL, NULL, NULL);

	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}