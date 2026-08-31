#include "P2PNetwork.h"      
#include "For_window.h"      
#include "Text.h"
#include "Program_Manager.h"
#define WM_NEW_NETWORK_TEXT (WM_USER + 1)
#define WM_FRIEND_CONECTED (WM_USER + 2)
#define WM_FRIEND_DISCONECTED (WM_USER + 3)
#define WM_NEW_NETWORK_TEXT_FRIEND (WM_USER + 4)
#define WM_FRIEND_CONECTED_FRIEND (WM_USER + 5)
#define WM_FRIEND_DISCONECTED_FRIEND (WM_USER + 6)

WNDCLASS create_win(HBRUSH BGcolor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC procedure)
{
	WNDCLASS WND = { 0 };

	WND.hIcon = Icon;
	WND.hCursor = Cursor;
	WND.hInstance = hInst;
	WND.lpszClassName = Name;
	WND.hbrBackground = BGcolor;
	WND.lpfnWndProc = procedure;

	return WND;
}

static void text_manipulation(Program_Manager& program, void(Text::*fun)())
{
	program.get_edit_text();

	Text text(program.get_text());

	(text.*fun)();

	program.set_edit_text(text.getter());
}

LRESULT CALLBACK MainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static Program_Manager program(hWnd);
	static P2PNetwork net;

	switch (msg)
	{
	case WM_CONTEXTMENU:
	{
		if ((HWND)wp != program.get_hwnd_edit())
			break;

		program.create_popup_menu(lp);

		break;
	}
	case WM_COMMAND:
		if (HIWORD(wp) == EN_CHANGE || HIWORD(wp) == EN_UPDATE)
		{
			if (HWND(lp) == program.get_hwnd_edit())
			{
				program.get_edit_text();

				SendMessageW(program.get_hwnd_status_bar(), SB_SETTEXTW, 1, (LPARAM)(wstring(L" Рядків: ") + wstring(std::to_wstring(SendMessageW(program.get_hwnd_edit(), EM_GETLINECOUNT, 0, 0))) + wstring(L" символів: ") + wstring(std::to_wstring(GetWindowTextLengthW(program.get_hwnd_edit())))).c_str());

				if (program.get_text() != program.top())
					program.push(program.get_text());

				if (net.isConnected() && HIWORD(wp) == EN_CHANGE)
				{
					wstring text = program.get_text();
					net.sendText(text);
			    }
			}
			else if (HWND(lp) == program.get_hwnd_friend_edit() && net.isConnected() && HIWORD(wp) == EN_CHANGE)
			{
				wstring text = program.get_friend_text();
				net.sendText(text);
			}
		}
		if (LOWORD(wp) == 1)
		{
			program.apply_changes();
			return 0;
		}
		switch (wp)
		{
		case USER_CLOSE:
			program.file_save();
			break;
		case USER_FILE_OPEN:
			program.open_file();
			break;
		case USER_FILE_SAVE_AS:
			program.save_file_as();
			break;
		case USER_INFO_ABOUT_PROGRAM:
			MessageBoxW(hWnd, L"Текстовий редактор", L"Інформація", MB_OK);
			break;
		case USER_INFO_ABOUT_PROGRAM_CREATOR:
			MessageBoxW(hWnd, L"Розробник: Винник Єгор!", L"Інформація", MB_OK);
			break;
		case DELETE_STRING:
			text_manipulation(program, &Text::delete_repeat_strings);
			break;
		case USER_FILE_SAVE:
			program.save_file();
			break;
		case CHANGE_SYMBOLS:
			text_manipulation(program, &Text::small_letters);
			break;
		case CHANGE_SYMBOLS_B:
			text_manipulation(program, &Text::big_letters);
			break;
		case CHANGE_SYMBOLS_A:
			text_manipulation(program, &Text::depending_on_the_letter);
			break;
		case DELETE_SYMBOLS:
			text_manipulation(program, &Text::delete_repeat_symbols);
			break;
		case SORT_STRING:
			text_manipulation(program, &Text::sort_strings_alphabetically);
			break;
		case SORT_WORD:
			text_manipulation(program, &Text::sort_strings_in_ascending_order);
			break;
		case SORT_WORD_B:
			text_manipulation(program, &Text::sort_strings_in_descending_order);
			break;
		case DELETE_STRINGS:
			text_manipulation(program, &Text::delete_repeat_words);
			break;
		case SORT_STRING_B:
			text_manipulation(program, &Text::sort_strings_non_alphabetically);
			break;
		case MERGE_STRING:
			text_manipulation(program, &Text::merge_strings);
			break;
		case CLEAR_EDIT:
			program.set_edit_text(wstring());
			break;
		case RETURN_TEXT:
			program.empty() ? void(MessageBoxW(hWnd, L"Ви вже повернули початковий текст!", L"Інформація", MB_OK)) : program.set_edit_text(program.pop()), SendMessageW(program.get_hwnd_edit(), EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
			break;
		case PRINT:
			program.print();
			break;
		case CHANGE_MY:
		{
			net.setOnMessageReceived([&](const wstring& text) {
				wstring* safe_text = new wstring(text);
				PostMessageW(program.get_hwnd_main(), WM_NEW_NETWORK_TEXT, (WPARAM)safe_text, 0);
				});

			net.setOnConnected([&]() {
				PostMessageW(program.get_hwnd_main(), WM_FRIEND_CONECTED, 0, 0);
				});

			net.setOnDisconnected([&]() {
				PostMessageW(program.get_hwnd_main(), WM_FRIEND_DISCONECTED, 0, 0);
				});

			net.startHost(5555);
			SendMessageW(program.get_hwnd_status_bar(), SB_SETTEXTW, 2, (LPARAM)L" Мережа: Очікування підключення");
			break;
		}
		case CHANGE_YOU:
		{
			net.setOnConnected([&]() {
				PostMessageW(program.get_hwnd_main(), WM_FRIEND_CONECTED_FRIEND, 0, 0);
				});

			net.setOnDisconnected([&]() {
				PostMessageW(program.get_hwnd_main(), WM_FRIEND_DISCONECTED_FRIEND, 0, 0);
				});

			net.setOnMessageReceived([&](const wstring& text) {
				wstring* safe_text = new wstring(text);
				PostMessageW(program.get_hwnd_main(), WM_NEW_NETWORK_TEXT_FRIEND, (WPARAM)safe_text, 0);
				});

			SendMessageW(program.get_hwnd_status_bar(), SB_SETTEXTW, 2, (LPARAM)L" Мережа: Очікування підключення");
			net.connectToHost(program.get_ip_address(), 5555);
			break;
		}
		case CREATE_EDIT_WINDOW:
			program.create_edit_window();
			break;
		case DISCONNECT:
			net.disconnect();
			break;
		default:
			break;
		}
		break;
	case WM_CREATE:
		if (hWnd == program.get_hwnd_main())
		{
			program.activator();
			program.create_status_bar(lp);
		}
		break;
	case WM_CTLCOLOREDIT:
		SetTextColor((HDC)wp, RGB(10, 25, 20));
		return (DWORD)GetSysColorBrush(COLOR_WINDOW);
	case WM_KEYDOWN:
		switch (wp)
		{
		case VK_ESCAPE:
			program.file_save();
			break;
		case VK_F1:
			program.open_file();
			break;
		case VK_F2:
			program.save_file();
			break;
		case VK_F3:
			program.save_file_as();
			break;
		case VK_F4:
			program.print();
			break;
		default:
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		if (hWnd == program.get_hwnd_main())
		{
			program.bottom_status_bar(wp, lp);
			program.resize_edit(wp, lp, program.get_hwnd_status_bar());
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_NEW_NETWORK_TEXT:
	{
		SendMessageW(program.get_hwnd_edit(), EM_SETEVENTMASK, 0, 0);
		wstring* received_text = (wstring*)wp;
		program.set_edit_text(*received_text);
		delete received_text;
		SendMessageW(program.get_hwnd_edit(), EM_SETEVENTMASK, 0, ENM_CHANGE);
		break;
	}
	case WM_FRIEND_CONECTED:
		SendMessageW(program.get_hwnd_status_bar(), SB_SETTEXTW, 2, (LPARAM)L" Мережа: Підключено до друга");
		break;
	case WM_FRIEND_DISCONECTED:
		SendMessageW(program.get_hwnd_status_bar(), SB_SETTEXTW, 2, (LPARAM)L" Мережа: Відключено");
		break;
	case WM_FRIEND_DISCONECTED_FRIEND:
		PostMessageW(program.get_hwnd_friend_win(), WM_CLOSE, 0, 0);
		SendMessageW(program.get_hwnd_status_bar(), SB_SETTEXTW, 2, (LPARAM)L" Мережа: Відключено");
		break;
	case WM_FRIEND_CONECTED_FRIEND:
		SendMessageW(program.get_hwnd_status_bar(), SB_SETTEXTW, 2, (LPARAM)L" Мережа: Підключено до друга");
		program.create_friend_window();
		break;
	case WM_NEW_NETWORK_TEXT_FRIEND:
	{
		SendMessageW(program.get_hwnd_friend_edit(), EM_SETEVENTMASK, 0, 0);
		wstring* received_text = (wstring*)wp;
		SendMessageW(program.get_hwnd_friend_edit(), EM_SETTEXTEX, 0, (LPARAM)(*received_text).c_str());
		delete received_text;
		SendMessageW(program.get_hwnd_friend_edit(), EM_SETEVENTMASK, 0, ENM_CHANGE);
		break;
	}
	case CHANGE_FILE_EXTENSION:
	{
		const wchar_t* raw_text = (const wchar_t*)wp;
		wstring received_text(raw_text);

		size_t dot_pos = received_text.find(L".");

		wstring extension = (dot_pos != wstring::npos) ? received_text.substr(dot_pos + 1) : L"";

		wstring final_message = L" Файл типу " + extension;
		SendMessageW(program.get_hwnd_status_bar(), SB_SETTEXTW, 0, (LPARAM)final_message.c_str());
		break;
	}
	case FILE_PATH:
	{
		const wchar_t* raw_text = (const wchar_t*)wp;
		wstring received_text(raw_text);

		wstring file_path = (received_text != L"0") ? received_text : L"";
		SendMessageW(program.get_hwnd_status_bar(), SB_SETTEXTW, 5, (LPARAM)file_path.c_str());
		break;
	}
	default:
		return DefWindowProcW(hWnd, msg, wp, lp);
	}

	return 0;
}