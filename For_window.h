#pragma once

#include <Windows.h>

WNDCLASS create_win(HBRUSH BGcolor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC procedure);
LRESULT CALLBACK MainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);