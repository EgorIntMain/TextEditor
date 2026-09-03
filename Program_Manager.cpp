#include "Program_Manager.h"
#include <fstream>
#include "Text.h"
#include "For_window.h"
#include <filesystem>
#include <windowsx.h>
#include <regex>

Edit_Manager::Edit_Manager()
{
	
}

Edit_Manager::~Edit_Manager()
{
	
}

void File_Manager::init_info()
{
	const size_t file_name_size = 3000;

	ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

	file_path.resize(file_name_size);

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hInstance = (HINSTANCE)main_window;
	ofn.hwndOwner = main_window;
	ofn.lpstrFile = file_path.data();
	ofn.nMaxFile = static_cast<DWORD>(file_path.size());
	ofn.lpstrFilter = L"Всі файли (*.*)\0*.*\0";
	ofn.lpstrInitialDir = L".\\";
	ofn.nMaxFile = file_name_size;
	ofn.lpstrDefExt = L"txt";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
}

void File_Manager::init_params()
{
	file_path = L"";
	open = false;
	file_saved = false;
}

File_Manager::File_Manager()
{
	DWORD file_pathSize = 0;

	if (RegGetValueW(HKEY_CURRENT_USER, reg_way.data(), L"file_path", RRF_RT_REG_SZ, NULL, NULL, &file_pathSize) != ERROR_SUCCESS)
	{
		init_params();
		init_info();
		return;
	}

	DWORD bufferSize = sizeof(file_path);
	file_path.resize(file_pathSize / sizeof(wchar_t), L'\0');

	switch (RegGetValueW(HKEY_CURRENT_USER, reg_way.data(), L"file_path", RRF_RT_REG_SZ, NULL, file_path.data(), &file_pathSize))
	{
	case ERROR_FILE_NOT_FOUND:
		init_params();
		break;
	case ERROR_SUCCESS:
		open = (file_path[0] != L'0');
		file_saved = false;
		break;
	default:
		init_params();
		break;
	}

	init_info();
}

File_Manager::~File_Manager()
{
	file_path.~basic_string();
}

Menu_Manager::~Menu_Manager()
{

}

Menu_Manager::Menu_Manager()
{
	
}

Pop_Up_Menu::Pop_Up_Menu()
{

}

Pop_Up_Menu::~Pop_Up_Menu()
{

}

void Pop_Up_Menu::activator()
{
	
}

static std::pair<int, int> get_selection_id(HWND edit)
{
	CHARRANGE cr = { 0, -1 };

	SendMessageW(edit, EM_EXGETSEL, 0, (LPARAM)&cr);

	return { cr.cpMin, cr.cpMax };
}

static void text_manipulation(void(Text::* fun)(), HWND edit, HWND main_window)
{
	std::pair<int, int> selection = get_selection_id(edit);

	if (selection.first == selection.second)
		return void(MessageBoxW(main_window, L"Будь ласка, виділіть текст для зміни!", L"Інформація", MB_OK));

	std::wstring selected_text(selection.second - selection.first + 1, L'\0');

	TEXTRANGEW tr;
	tr.chrg.cpMin = selection.first;
	tr.chrg.cpMax = selection.second;
	tr.lpstrText = selected_text.data();
	SendMessageW(edit, EM_GETTEXTRANGE, 0, (LPARAM)&tr);

	Text text(selected_text);

	(text.*fun)();

	SendMessageW(edit, EM_REPLACESEL, TRUE, (LPARAM)text.getter().c_str());
}

int Pop_Up_Menu::create_popup_menu(LPARAM lp)
{
	x_pos = GET_X_LPARAM(lp);
	y_pos = GET_Y_LPARAM(lp);

	hPopupMenu = CreatePopupMenu();
	HMENU Register_Menu = CreateMenu();
	HMENU Delete_Menu = CreateMenu();
	HMENU Sort_Menu = CreateMenu();
	AppendMenuW(Register_Menu, MF_STRING, 1006, L"Змінити літеру/літери на маленьку/маленькі");
	AppendMenuW(Register_Menu, MF_STRING, 1007, L"Змінити літеру/літери на велику/великі");
	AppendMenuW(Register_Menu, MF_STRING, 1008, L"Змінити літеру/літери в залежності від неї/них");
	AppendMenuW(Delete_Menu, MF_STRING, 1009, L"Видалити повторювальні рядки");
	AppendMenuW(Delete_Menu, MF_STRING, 1010, L"Видалити повторювальні символи у кожному рядку");
	AppendMenuW(Delete_Menu, MF_STRING, 1011, L"Видалити повторювальні слова у кожному рядку");
	AppendMenuW(Sort_Menu, MF_STRING, 1012, L"Відсортувати слова в рядках в алфавітному порядку");
	AppendMenuW(Sort_Menu, MF_STRING, 1013, L"Відсортувати слова в рядках в порядку протилежному до алфавітного");
	AppendMenuW(Sort_Menu, MF_STRING, 1014, L"Відсортувати слова в рядках в порядку зростання");
	AppendMenuW(Sort_Menu, MF_STRING, 1015, L"Відсортувати слова в рядках в порядку спадання");

	AppendMenuW(hPopupMenu, MF_STRING, 1001, L"Копіювати\tCtrl+C");
	AppendMenuW(hPopupMenu, MF_STRING, 1002, L"Вставити\tCtrl+V");
	AppendMenuW(hPopupMenu, MF_STRING, 1003, L"Вирізати\tCtrl+X");
	AppendMenuW(hPopupMenu, MF_SEPARATOR, 0, NULL);
	AppendMenuW(hPopupMenu, MF_STRING, 1004, L"Виділити все\tCtrl+A");
	AppendMenuW(hPopupMenu, MF_STRING, 1005, L"Очистити");
	AppendMenuW(hPopupMenu, MF_SEPARATOR, 0, NULL);
	AppendMenuW(hPopupMenu, MF_POPUP, (UINT_PTR)Delete_Menu, L"Видалити");
	AppendMenuW(hPopupMenu, MF_POPUP, (UINT_PTR)Register_Menu, L"Зміна регістру букв");
	AppendMenuW(hPopupMenu, MF_POPUP, (UINT_PTR)Sort_Menu, L"Сортування");
	AppendMenuW(hPopupMenu, MF_STRING, 1016, L"Об'єднати виділені рядки");

	int selectedId = TrackPopupMenu(hPopupMenu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON, x_pos, y_pos, 0, main_window, NULL);

	switch (selectedId)
	{
	case 1001: 
		SendMessageW(edit, WM_COPY, 0, 0);
		break;
	case 1002: 
		SendMessageW(edit, WM_PASTE, 0, 0);
		break;
	case 1003: 
		SendMessageW(edit, WM_CUT, 0, 0);
		break;
	case 1004: 
	{
		CHARRANGE cr = { 0, -1 }; 
		SendMessageW(edit, EM_EXSETSEL, 0, (LPARAM)&cr);
		break;
	}
	case 1005: 
		SetWindowTextW(edit, L"");
		break;
	case 1006: 
		text_manipulation(&Text::small_letters, edit, main_window);
		break;
	case 1007: 
		text_manipulation(&Text::big_letters, edit, main_window);
		break;
	case 1008: 
		text_manipulation(&Text::depending_on_the_letter, edit, main_window);
		break;
	case 1009:
		text_manipulation(&Text::delete_repeat_strings, edit, main_window);
		break;
	case 1010:
		text_manipulation(&Text::delete_repeat_symbols, edit, main_window);
		break;
	case 1011:
		text_manipulation(&Text::delete_repeat_words, edit, main_window);
		break;
	case 1012:
		text_manipulation(&Text::sort_strings_alphabetically, edit, main_window);
		break;
	case 1013:
		text_manipulation(&Text::sort_strings_non_alphabetically, edit, main_window);
		break;
	case 1014:
		text_manipulation(&Text::sort_strings_in_ascending_order, edit, main_window);
		break;
	case 1015:
		text_manipulation(&Text::sort_strings_in_descending_order, edit, main_window);
		break;
	case 1016:
		text_manipulation(&Text::merge_strings, edit, main_window);
		break;
	default:
		break;
	}

	DestroyMenu(hPopupMenu);

	return 0;
}

void Menu_Manager::activator()
{
	HMENU Menu = CreateMenu();
	HMENU File_Menu = CreateMenu();
	HMENU About_Menu = CreateMenu();
	HMENU Edit_Menu = CreateMenu();
	HMENU Register_Menu = CreateMenu();
	HMENU Delete_Menu = CreateMenu();
	HMENU Sort_Menu = CreateMenu();
	HMENU Text_Menu = CreateMenu();
	HMENU Together_Menu = CreateMenu();

	AppendMenuW(File_Menu, MF_STRING, USER_FILE_OPEN, L"Відкрити\tF1");
	AppendMenuW(File_Menu, MF_STRING, USER_FILE_SAVE, L"Зберегти\tF2");
	AppendMenuW(File_Menu, MF_STRING, USER_FILE_SAVE_AS, L"Зберегти як\tF3");
	AppendMenuW(File_Menu, MF_SEPARATOR, NULL, NULL);
	AppendMenuW(File_Menu, MF_STRING, USER_CLOSE, L"Закрити\tEscape");
	AppendMenuW(File_Menu, MF_STRING, PRINT, L"Друк\tF4");
	AppendMenuW(About_Menu, MF_STRING, USER_INFO_ABOUT_PROGRAM, L"Інформація про програму");
	AppendMenuW(About_Menu, MF_STRING, USER_INFO_ABOUT_PROGRAM_CREATOR, L"Інформація про розробника");
	AppendMenuW(Delete_Menu, MF_STRING, DELETE_STRING, L"Видалити повторювальні рядки");
	AppendMenuW(Delete_Menu, MF_STRING, DELETE_SYMBOLS, L"Видалити повторювальні символи у кожному рядку");
	AppendMenuW(Delete_Menu, MF_STRING, DELETE_STRINGS, L"Видалити повторювальні слова у кожному рядку");
	AppendMenuW(Register_Menu, MF_STRING, CHANGE_SYMBOLS, L"Змінити усі літери на маленькі");
	AppendMenuW(Register_Menu, MF_STRING, CHANGE_SYMBOLS_B, L"Змінити усі літери на великі");
	AppendMenuW(Register_Menu, MF_STRING, CHANGE_SYMBOLS_A, L"Змінити усі літери в залежності від них");
	AppendMenuW(Sort_Menu, MF_STRING, SORT_STRING, L"Відсортувати слова в рядках в алфавітному порядку");
	AppendMenuW(Sort_Menu, MF_STRING, SORT_STRING_B, L"Відсортувати слова в рядках в порядку протилежному до алфавітного");
	AppendMenuW(Sort_Menu, MF_STRING, SORT_WORD, L"Відсортувати слова в рядках в порядку зростання");
	AppendMenuW(Sort_Menu, MF_STRING, SORT_WORD_B, L"Відсортувати слова в рядках в порядку спадання");

	AppendMenuW(Edit_Menu, MF_POPUP, (UINT_PTR)Delete_Menu, L"Видалити");
	AppendMenuW(Edit_Menu, MF_POPUP, (UINT_PTR)Register_Menu, L"Зміна регістру букв");
	AppendMenuW(Edit_Menu, MF_POPUP, (UINT_PTR)Sort_Menu, L"Сортування");
	AppendMenuW(Edit_Menu, MF_STRING, MERGE_STRING, L"Об'єднати усі рядки");
	AppendMenuW(Text_Menu, MF_STRING, CLEAR_EDIT, L"Очистити поле вводу");
	AppendMenuW(Text_Menu, MF_STRING, RETURN_TEXT, L"Повернути попередній текст");

	AppendMenuW(Together_Menu, MF_STRING, CHANGE_MY, L"Редагувати моє текстове поле");
	AppendMenuW(Together_Menu, MF_STRING, CHANGE_YOU, L"Редагувати текстове поле друга");
	AppendMenuW(Together_Menu, MF_STRING, DISCONNECT, L"Відключитися");

	AppendMenuW(Menu, MF_POPUP, (UINT_PTR)File_Menu, L"Файл");
	AppendMenuW(Menu, MF_POPUP, (UINT_PTR)Edit_Menu, L"Операції над полем вводу");
	AppendMenuW(Menu, MF_POPUP, (UINT_PTR)About_Menu, L"Про");
	AppendMenuW(Menu, MF_POPUP, (UINT_PTR)Text_Menu, L"Текстові операції");
	AppendMenuW(Menu, MF_POPUP, (UINT_PTR)Together_Menu, L"Cпільне редагування файлу");
	AppendMenuW(Menu, MF_STRING, CREATE_EDIT_WINDOW, L"Вікно налаштування");

	SetMenu(main_window, Menu);
}

void Edit_Window::get_friend_ip_adr_from_reg()
{
	wchar_t raw_buffer[256] = { 0 };
	DWORD bufferSize = sizeof(raw_buffer);

	switch (RegGetValueW(HKEY_CURRENT_USER, reg_way.c_str(), L"friend_ip_addres", RRF_RT_REG_SZ, NULL, raw_buffer, &bufferSize))
	{
	case ERROR_FILE_NOT_FOUND:
		friend_ip_addres = L"";
		break;
	case ERROR_SUCCESS:
		friend_ip_addres = raw_buffer;
		break;
	default:
		friend_ip_addres = L"";
		break;
	}
}

Edit_Window::Edit_Window()
{
	msg = { 0 };
	selectedIndex = 0;

	get_friend_ip_adr_from_reg();
}

Edit_Window::~Edit_Window() 
{

}

Edit_Friend_Text_Window::Edit_Friend_Text_Window()
{
	msg_friend = { 0 };
}

Edit_Friend_Text_Window::~Edit_Friend_Text_Window()
{

}

void Edit_Friend_Text_Window::activator()
{
	friend_win = create_win((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_HELP), hInstance, LoadIcon(hInstance, MAKEINTRESOURCE(IDI_QUESTION)), L"Egor’s Text Friend Window", MainProcedure);

	if (!RegisterClassW(&friend_win))
		return;
}

int Edit_Friend_Text_Window::create_friend_window()
{
	if (friend_window && IsWindow(friend_window))
	{
		SetForegroundWindow(friend_window);
		return 0;
	}

	friend_window = CreateWindowW(L"Egor’s Text Friend Window", L"Вікно Вашого друга", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, screen_w, screen_h, main_window, NULL, hInstance, NULL);

	LoadLibrary(TEXT("Msftedit.dll"));
	friend_edit = CreateWindowEx(0, MSFTEDIT_CLASS, TEXT(""), WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE, 0, 0, screen_w, screen_h - 60, friend_window, (HMENU)100000, hInstance, NULL);
	SendMessageW(friend_edit, EM_SETEVENTMASK, 0, ENM_CHANGE);

	ShowWindow(friend_window, SW_SHOW);
	UpdateWindow(friend_window);

	while (GetMessage(&msg_friend, NULL, NULL, NULL))
	{
		TranslateMessage(&msg_friend);
		DispatchMessage(&msg_friend);
	}

	return 0;
}

HWND Edit_Friend_Text_Window::get_hwnd_friend_win() const
{
	return friend_window;
}

const wstring Edit_Friend_Text_Window::get_friend_text() const
{
	int text_size = GetWindowTextLengthW(friend_edit) + 1;
	wstring text = wstring(text_size, L'\0');

	GetWindowTextW(friend_edit, text.data(), text_size);

	return text;
}

HWND Edit_Friend_Text_Window::get_hwnd_friend_edit() const
{
	return friend_edit;
}

void Edit_Window::activator()
{
	edit_win = create_win((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_HELP), hInstance, LoadIcon(hInstance, MAKEINTRESOURCE(IDI_QUESTION)), L"Egor’s Text Edit Window", MainProcedure);

	if (!RegisterClassW(&edit_win))
		return;
}

bool isValidIPAddress(const wstring& friend_ip_addres) 
{
	return std::regex_match(friend_ip_addres.c_str(), std::wregex(L"(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:))$"));
}

void Edit_Window::apply_changes()
{
	selectedIndex = SendMessageW(edit_screen, CB_GETCURSEL, 0, 0);

	if (selectedIndex == CB_ERR) 
	{
		MessageBoxW(main_window, L"Ви не обрали розширення!", L"Інформація", MB_OK);
		return;
	}

	size_t friend_ip_len = GetWindowTextLengthW(edit_ip) + 1;
	friend_ip_addres = wstring(friend_ip_len, L'\0');
	GetWindowTextW(edit_ip, friend_ip_addres.data(), friend_ip_len);

	if (!isValidIPAddress(friend_ip_addres))
	{
		MessageBoxW(main_window, L"IPv6-адреса невірна!", L"Інформація", MB_OK);
		return;
	}

	wstring resolution(SendMessageW(edit_screen, CB_GETLBTEXTLEN, selectedIndex, 0), '\0');
	SendMessageW(edit_screen, CB_GETLBTEXT, selectedIndex, (LPARAM)resolution.data());

	size_t x_pos = resolution.find('x');
	wstring screen_h = resolution.substr(x_pos + 1);
	wstring screen_w = resolution.substr(0, x_pos);

	RegSetKeyValueW(HKEY_CURRENT_USER, reg_way.data(), L"screen_h", REG_SZ, (const BYTE*)screen_h.data(), (DWORD)((wcslen(screen_h.data()) + 1) * sizeof(wchar_t)));
	RegSetKeyValueW(HKEY_CURRENT_USER, reg_way.data(), L"screen_w", REG_SZ, (const BYTE*)screen_w.data(), (DWORD)((wcslen(screen_w.data()) + 1) * sizeof(wchar_t)));
	RegSetKeyValueW(HKEY_CURRENT_USER, reg_way.data(), L"friend_ip_addres", REG_SZ, (const BYTE*)friend_ip_addres.data(), (DWORD)((wcslen(friend_ip_addres.data()) + 1) * sizeof(wchar_t)));

	Info_keep::screen_h = stoi(screen_h);
	Info_keep::screen_w = stoi(screen_w);

	SetWindowPos(main_window, NULL, 0, 0, Info_keep::screen_w, Info_keep::screen_h, SWP_NOMOVE);
	SetWindowPos(edit, NULL, 0, 0, Info_keep::screen_w, Info_keep::screen_h - 60, SWP_NOMOVE);
}

void Edit_Window::make_cont_comboboxs()
{
	SendMessageW(edit_screen, CB_ADDSTRING, 0, (LPARAM)L"1280x720");
	SendMessageW(edit_screen, CB_ADDSTRING, 0, (LPARAM)L"1366x768");
	SendMessageW(edit_screen, CB_ADDSTRING, 0, (LPARAM)L"1920x1080");
	SendMessageW(edit_screen, CB_ADDSTRING, 0, (LPARAM)L"2560x1440");
	SendMessageW(edit_screen, CB_ADDSTRING, 0, (LPARAM)L"3840x2160");

	switch (screen_w)
	{
	case 1280:
		selectedIndex = 0;
		break;
	case 1366:
		selectedIndex = 1;
		break;
	case 1920:
		selectedIndex = 2;
		break;
	case 2560:
		selectedIndex = 3;
		break;
	case 3840:
		selectedIndex = 4;
		break;
	default:
		break;
	}

	SendMessageW(edit_screen, CB_SETCURSEL, selectedIndex, 0);
}

int Edit_Window::create_edit_window()
{
	if (edit_window && IsWindow(edit_window)) 
	{
		SetForegroundWindow(edit_window);
		return 0;
	}

	get_friend_ip_adr_from_reg();

	edit_window = CreateWindowW(L"Egor’s Text Edit Window", L"Вікно налаштування", WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 690, 300, main_window, NULL, hInstance, NULL);

	CreateWindowW(L"STATIC", L"Оберіть бажану роздільну здатність", WS_CHILD | WS_VISIBLE, 50, 20, 250, 20, edit_window, NULL, NULL, NULL);
	edit_screen = CreateWindowW(L"COMBOBOX", NULL, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 50, 50, 250, 150, edit_window, NULL, NULL, NULL);

	CreateWindowW(L"STATIC", L"IPv6-адреса Вашого друга", WS_CHILD | WS_VISIBLE, 350, 20, 250, 20, edit_window, NULL, NULL, NULL);
	edit_ip = CreateWindowW(L"edit", L"", CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | ES_MULTILINE, 350, 50, 300, 20, edit_window, NULL, NULL, NULL);

	SetWindowTextW(edit_ip, friend_ip_addres.data());

	apply_butt = CreateWindowW(L"BUTTON", L"Зберегти зміни", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 260, 100, 120, 50, edit_window, (HMENU)1, hInstance, NULL);

	make_cont_comboboxs();

	ShowWindow(edit_window, SW_SHOW);
	UpdateWindow(edit_window);

	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

void Edit_Manager::activator()
{
	LoadLibrary(TEXT("Msftedit.dll"));
	edit = CreateWindowEx(0, MSFTEDIT_CLASS, TEXT(""), WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE, 0, 0, screen_w, screen_h - 60, main_window, (HMENU)10000, hInstance, NULL);
	SendMessageW(edit, EM_SETEVENTMASK, 0, ENM_CHANGE);
}

HWND Info_keep::get_hwnd_main() const
{
	return main_window;
}

void File_Manager::activator()
{
	if (open)
		open_file();
}

void Printer_Manager::activator()
{
	
}

Program_Manager::Program_Manager(HWND window)
{
	main_window = window;
}

void Program_Manager::activator()
{
	hInstance = GetModuleHandle(NULL);
	Edit_Manager::activator();
	File_Manager::activator();
	Printer_Manager::activator();
	Menu_Manager::activator();
	Edit_Window::activator();
	Edit_Friend_Text_Window::activator();
	Status_Bar::activator();
	activated = true;
}

int get_screen_size(const int axis, const wstring& reg_way)
{
    wstring valueName = (axis ? L"screen_h" : L"screen_w"), buffer = L"\0";
	DWORD bufferSize = sizeof(buffer);

	int screen_size = 0;

	switch (RegGetValueW(HKEY_CURRENT_USER, reg_way.data(), valueName.data(), RRF_RT_REG_SZ, NULL, buffer.data(), &bufferSize))
	{
	case ERROR_FILE_NOT_FOUND:
		return GetSystemMetrics(axis);
	case ERROR_SUCCESS:
		screen_size = (buffer.size() != 0 ? stoi(buffer) : GetSystemMetrics(axis));
		break;
	default:
		return GetSystemMetrics(axis);
	}

	return screen_size;
}

Info_keep::Info_keep(): reg_way(L"Software\\TextEditor")
{
	screen_w = get_screen_size(SM_CXSCREEN, reg_way);
	screen_h = get_screen_size(SM_CYSCREEN, reg_way);
	activated = false;
}

Info_keep::~Info_keep()
{
	
}

void File_Manager::save_file_as()
{
	GetSaveFileNameW(&ofn) ? open = true, save_file() : void(MessageBoxW(main_window, L"Щось пішло не так!", L"Інформація", MB_OK));
}

void File_Manager::file_save()
{
	if (file_saved)
		PostMessageW(main_window, WM_DESTROY, 0, 0);

	switch (MessageBoxW(main_window, L"Зберегти файл?", L"Зберегти", MB_YESNOCANCEL | MB_ICONQUESTION))
	{
	case IDNO:
	{
		RegSetKeyValueW(HKEY_CURRENT_USER, reg_way.c_str(), L"file_path", REG_SZ, (const BYTE*)L"0", (DWORD)((wcslen(L"0") + 1) * sizeof(wchar_t)));
		PostMessageW(main_window, WM_DESTROY, 0, 0);
		break;
	}
	case IDCANCEL:
		return;
	default:
		break;
	}

	open ? save_file() : save_file_as();

	RegSetKeyValueW(HKEY_CURRENT_USER, reg_way.c_str(), L"file_path", REG_SZ, (const BYTE*)L"0", (DWORD)((wcslen(L"0") + 1) * sizeof(wchar_t)));

	PostMessageW(main_window, WM_DESTROY, 0, 0);
}

Program_Manager::~Program_Manager()
{
	text.~basic_string();
}

void Printer_Manager::print()
{
	if (!PrintDlg(&print_info))
	{
		MessageBoxW(main_window, L"Ви не обрали принтер, а тому нічого не буде надруковано!", L"Інформація", MB_OK);
		return;
	}

	text_size = GetWindowTextLengthW(edit) + 1;
	text = wstring(text_size, L'\0');
	GetWindowTextW(edit, text.data(), text_size);

	DEVNAMES* pdn = (DEVNAMES*)GlobalLock(print_info.hDevNames);

	if (!pdn)
	{
		GlobalUnlock(print_info.hDevNames);

		DeleteDC(print_info.hDC);

		MessageBoxW(main_window, L"Не було знайдено принтер!", L"Інформація", MB_OK);

		return;
	}

	Text print_text(text);

	LPWSTR print_name = (LPWSTR)pdn + pdn->wDeviceOffset;

	GlobalUnlock(print_info.hDevNames);

	HDC printer_info = CreateDCW(L"WINSPOOL", print_name, NULL, NULL);

	StartDocW(printer_info, &docinfo);

	int page_width = GetDeviceCaps(printer_info, HORZRES);
	int page_height = GetDeviceCaps(printer_info, VERTRES);

	int margin_x = page_width / 20;
	int margin_y = page_height / 20;

	int usable_width = page_width - (margin_x * 2);
	int max_y = page_height - margin_y;

	StartPage(printer_info);

	int current_y = margin_y;

	for (const TextLine& line : print_text)
	{
		RECT rect = { margin_x, current_y, margin_x + usable_width, current_y };

		DrawTextW(printer_info, line.data(), line.size(), &rect, DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX);

		int text_height = rect.bottom - rect.top;

		if (current_y + text_height > max_y)
		{
			EndPage(printer_info);
			StartPage(printer_info);

			current_y = margin_y;

			rect.top = current_y;
			rect.bottom = current_y + text_height;
		}

		DrawTextW(printer_info, line.data(), line.size(), &rect, DT_WORDBREAK | DT_NOPREFIX);

		current_y += text_height;
	}

	EndPage(printer_info);
	EndDoc(printer_info);

	DeleteDC(printer_info);

	if (print_info.hDC && print_info.hDC != printer_info)
		DeleteDC(print_info.hDC);
}

Printer_Manager::Printer_Manager()
{
	ZeroMemory(&print_info, sizeof(PRINTDLG));

	print_info.lStructSize = sizeof(PRINTDLG);
	print_info.hwndOwner = main_window;
	print_info.Flags = PD_RETURNDC | PD_HIDEPRINTTOFILE | PD_DISABLEPRINTTOFILE;
	print_info.hDevMode = NULL;
	print_info.hDevNames = NULL;
	print_info.nCopies = 1;
	print_info.nFromPage = 1;
	print_info.nToPage = 2;
	print_info.nMinPage = 1;
	print_info.nMaxPage = 2;

	docinfo.cbSize = sizeof(docinfo);
	docinfo.lpszDocName = L"Document";
	docinfo.lpszOutput = NULL;
	docinfo.lpszDatatype = NULL;
	docinfo.fwType = 0;
}

Printer_Manager::~Printer_Manager()
{

}

wstring Stack_Manager::pop()
{
	texts.pop();
	wstring top = this->top();
	file_saved = false;
	return top;
}

bool Stack_Manager::empty() const
{
	return texts.empty();
}

Stack_Manager::Stack_Manager()
{

}

Stack_Manager::~Stack_Manager()
{
	texts.~stack();
}

void Stack_Manager::push(const wstring& data)
{
	texts.push(data);
	file_saved = false;
}

const wstring& Stack_Manager::top() const
{
	if (empty())
		return L"";

	return texts.top();
}

const wstring& Edit_Manager::get_text() const
{
	return text;
}

void Edit_Manager::set_edit_text(const wstring& data) const
{
	SETTEXTEX st = { ST_DEFAULT, 1200 };
	SendMessageW(edit, EM_SETTEXTEX, (WPARAM)&st, (LPARAM)data.c_str());
}

void Edit_Manager::get_edit_text()
{
	text_size = GetWindowTextLengthW(edit) + 1;
	text = wstring(text_size, L'\0');

	GetWindowTextW(edit, text.data(), text_size);
}

HWND Edit_Manager::get_hwnd_edit() const
{
	return edit;
}

void Edit_Manager::resize_edit(WPARAM wp, LPARAM lp, HWND hWnd)
{
	int width = LOWORD(lp);
	int height = HIWORD(lp);

	RECT rcStatus;
	GetWindowRect(hWnd, &rcStatus);
	int statusHeight = rcStatus.bottom - rcStatus.top;

	if (edit)
		MoveWindow(edit, 0, 0, width, height - statusHeight, TRUE);
}

static DWORD CALLBACK CppStreamOutCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG* pcb)
{
	std::wofstream* ofs = reinterpret_cast<std::wofstream*>(dwCookie);

	ofs->write(reinterpret_cast<wchar_t*>(pbBuff), cb / sizeof(wchar_t));

	if (ofs->bad())
		return 1;

	*pcb = (cb / sizeof(wchar_t)) * sizeof(wchar_t);

	return 0;
}

void File_Manager::save_file()
{
	if (!open)
		GetSaveFileNameW(&ofn);

	std::wofstream ofs(file_path, std::wofstream::binary);
	ofs.imbue(std::locale(".UTF8"));

	if (!ofs.is_open())
		return void(MessageBoxW(main_window, L"Не можу зберегти файл!", L"Інформація", MB_OK));

	EDITSTREAM es = { 0 }; 
	es.dwCookie = reinterpret_cast<DWORD_PTR>(&ofs); 
	es.pfnCallback = CppStreamOutCallback;

	SendMessageW(edit, EM_STREAMOUT, SF_TEXT | SF_UNICODE, reinterpret_cast<LPARAM>(&es));

	ofs.close();

	file_saved = true;

	RegSetKeyValueW(HKEY_CURRENT_USER, reg_way.c_str(), L"file_path", REG_SZ, (const BYTE*)file_path.data(), (DWORD)((wcslen(file_path.data()) + 1) * sizeof(wchar_t)));
	PostMessageW(main_window, CHANGE_FILE_EXTENSION, (WPARAM)file_path.data(), 0);
}

static DWORD CALLBACK CppStreamCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG* pcb)
{
	std::wifstream* ifs = reinterpret_cast<std::wifstream*>(dwCookie);

	if (!ifs || !ifs->is_open()) 
		return 1;

	ifs->read(reinterpret_cast<wchar_t*>(pbBuff), cb / sizeof(wchar_t));
	*pcb = static_cast<LONG>(ifs->gcount() * sizeof(wchar_t));

	return 0;
}

void File_Manager::open_file()
{
	if (activated && GetOpenFileNameW(&ofn))
		file_path.resize(wcslen(file_path.c_str()));

	std::wifstream ifs(file_path, std::wifstream::binary);
	ifs.imbue(std::locale(".UTF8"));

	if (!ifs.is_open() && activated)
		return void(MessageBoxW(main_window, L"Не можу відкрити файл!", L"Інформація", MB_OK));
	else
	{
		RegSetKeyValueW(HKEY_CURRENT_USER, reg_way.c_str(), L"file_path", REG_SZ, (const BYTE*)L"0", (DWORD)((wcslen(L"0") + 1) * sizeof(wchar_t)));
		return;
	}

	EDITSTREAM es = { 0 };
	es.dwCookie = reinterpret_cast<DWORD_PTR>(&ifs);
	es.pfnCallback = CppStreamCallback;

	SendMessageW(edit, EM_STREAMIN, SF_TEXT | SF_UNICODE, reinterpret_cast<LPARAM>(&es));

	ifs.close();
	open = true;
	PostMessageW(main_window, CHANGE_FILE_EXTENSION, (WPARAM)file_path.data(), 0);
	PostMessageW(main_window, FILE_PATH, (WPARAM)file_path.data(), 0);
}

wstring Edit_Window::get_ip_address() const
{
	return friend_ip_addres;
}

Status_Bar::Status_Bar()
{

}

Status_Bar::~Status_Bar()
{

}

void Status_Bar::activator()
{
	InitCommonControls();
}

int Status_Bar::create_status_bar(LPARAM lp)
{
	hStatus = CreateWindowExW(0, STATUSCLASSNAMEW, NULL, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0, main_window, (HMENU)2000, ((LPCREATESTRUCT)lp)->hInstance, NULL);

	int statwidths[] = { 400, 800, 1200, 1600, 2000, 2400 };

	SendMessageW(hStatus, SB_SETPARTS, sizeof(statwidths) / sizeof(int), (LPARAM)statwidths);

	SendMessageW(hStatus, SB_SETTEXTW, 0, (LPARAM)L" Файл типу ");
	SendMessageW(hStatus, SB_SETTEXTW, 1, (LPARAM)L" Рядків: 1, символів: 0");
	SendMessageW(hStatus, SB_SETTEXTW, 2, (LPARAM)L" Мережа: Відключено");
	SendMessageW(hStatus, SB_SETTEXTW, 3, (LPARAM)L" Windows (CR LF)");
	SendMessageW(hStatus, SB_SETTEXTW, 4, (LPARAM)L" UTF-8");
	SendMessageW(hStatus, SB_SETTEXTW, 5, (LPARAM)L"");

	return 0;
}

void Status_Bar::bottom_status_bar(WPARAM wp, LPARAM lp) const
{
	int clientWidth = LOWORD(lp);
	int clientHeight = HIWORD(lp);

	int step = clientWidth / 6; 
	int statwidths[6];
	statwidths[0] = step;
	statwidths[1] = step * 2;
	statwidths[2] = step * 3;
	statwidths[3] = step * 4;
	statwidths[4] = step * 5;
	statwidths[5] = -1;         

	SendMessageW(hStatus, SB_SETPARTS, 6, (LPARAM)statwidths);
	SendMessageW(hStatus, WM_SIZE, wp, lp);

	RECT rcStatus;
	int statusHeight = 0;
	GetWindowRect(hStatus, &rcStatus);
	statusHeight = rcStatus.bottom - rcStatus.top;

	MoveWindow(edit, 0, 0, clientWidth, clientHeight - statusHeight, TRUE);
}

HWND Status_Bar::get_hwnd_status_bar() const
{
	return hStatus;
}