#pragma once

#include <Windows.h>
#include <string>

using std::wstring;

WNDCLASS create_win(HBRUSH BGcolor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC procedure);
LRESULT CALLBACK MainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
int get_screen_size(const int axis, const wstring& reg_way);