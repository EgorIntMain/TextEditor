#pragma once

#include <Windows.h> 
#include <commdlg.h>
#include <string>
#include <stack>
#include <richedit.h>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
#define CHANGE_FILE_EXTENSION (WM_USER + 7)
#define FILE_PATH (WM_USER + 8)
#define USER_CLOSE (WM_USER + 9)
#define USER_FILE_OPEN (WM_USER + 10)
#define USER_FILE_SAVE_AS (WM_USER + 11)
#define USER_INFO_ABOUT_PROGRAM (WM_USER + 12)
#define USER_INFO_ABOUT_PROGRAM_CREATOR (WM_USER + 13)
#define DELETE_STRING (WM_USER + 14)
#define USER_FILE_SAVE (WM_USER + 15)
#define CHANGE_SYMBOLS (WM_USER + 16)
#define CHANGE_SYMBOLS_B (WM_USER + 17)
#define CHANGE_SYMBOLS_A (WM_USER + 18)
#define DELETE_SYMBOLS (WM_USER + 19)
#define SORT_STRING (WM_USER + 20)
#define SORT_WORD (WM_USER + 21)
#define SORT_WORD_B (WM_USER + 22)
#define DELETE_STRINGS (WM_USER + 23)
#define SORT_STRING_B (WM_USER + 24)
#define MERGE_STRING (WM_USER + 25)
#define CLEAR_EDIT (WM_USER + 26)
#define RETURN_TEXT (WM_USER + 27)
#define PRINT (WM_USER + 28)
#define CHANGE_MY (WM_USER + 29)
#define CHANGE_YOU (WM_USER + 30)
#define CREATE_EDIT_WINDOW (WM_USER + 31)
#define DISCONNECT (WM_USER + 32)

using std::wstring;

int get_screen_size(const int axis, const wstring& reg_way);

class Info_keep {
protected:
	HWND edit;
	HWND main_window;
	HINSTANCE hInstance;
	int screen_w;
	int screen_h;
	size_t text_size;
	wstring text;
	const wstring reg_way;
	bool file_saved;
	bool activated;
	wstring file_path;
public:
	Info_keep();
	virtual ~Info_keep();
	virtual void activator() = 0;
	HWND get_hwnd_main() const;
};

class Edit_Manager: virtual public Info_keep {
protected:
	virtual void activator() override = 0;
public:
	Edit_Manager();
	virtual ~Edit_Manager();
	void set_edit_text(const wstring& data) const;
	const wstring& get_text() const;
	void get_edit_text();
	HWND get_hwnd_edit() const;
	void resize_edit(WPARAM wp, LPARAM lp, HWND hWnd);
};

class Menu_Manager : virtual public Info_keep {
protected:
	virtual void activator() override = 0;
public:
	Menu_Manager();
	virtual ~Menu_Manager();
};

class Printer_Manager : virtual public Info_keep {
	PRINTDLG print_info;
	DOCINFOW docinfo;
protected:
	virtual void activator() override = 0;
public:
	Printer_Manager();
	virtual ~Printer_Manager();
	void print();
};

class File_Manager : virtual public Info_keep {
	OPENFILENAMEW ofn;
	bool open;
	void init_info();
	void init_params();
protected:
	virtual void activator() override = 0;
public:
	File_Manager();
	virtual ~File_Manager();
	void save_file_as();
	void save_file();
	void open_file();
	void file_save();
};

class Stack_Manager : virtual public Info_keep {
	std::stack<wstring> texts;
public:
	Stack_Manager();
	virtual ~Stack_Manager();
	wstring pop();
	bool empty() const;
	void push(const wstring& data);
	const wstring& top() const;
};

class Edit_Window : virtual public Info_keep {
	void make_cont_comboboxs();
	HWND edit_window;
	HWND edit_screen;
	HWND edit_ip;
	HWND apply_butt;
	WNDCLASS edit_win;
	MSG msg;
	wstring friend_ip_addres;
	int selectedIndex;
	void get_friend_ip_adr_from_reg();
protected:
	virtual void activator() override = 0;
public:
	Edit_Window();
	virtual ~Edit_Window();
	void apply_changes();
	int create_edit_window();
	wstring get_ip_address() const;
};

class Edit_Friend_Text_Window : virtual public Info_keep {
	HWND friend_window;
	HWND friend_edit;
	WNDCLASS friend_win;
	MSG msg_friend;
protected:
	virtual void activator() override = 0;
public:
	Edit_Friend_Text_Window();
	virtual ~Edit_Friend_Text_Window();
	int create_friend_window();
	HWND get_hwnd_friend_win() const;
	const wstring get_friend_text() const;
	HWND get_hwnd_friend_edit() const;
};

class Pop_Up_Menu : virtual public Info_keep {
	int y_pos;
	int x_pos;
	HMENU hPopupMenu;
protected:
	virtual void activator() override = 0;
public:
	Pop_Up_Menu();
	virtual ~Pop_Up_Menu();
	int create_popup_menu(LPARAM lp);
};

class Status_Bar : virtual public Info_keep {
	HWND hStatus;
protected:
	virtual void activator() override = 0;
public:
	Status_Bar();
	virtual ~Status_Bar();
	int create_status_bar(LPARAM lp);
	void bottom_status_bar(WPARAM wp, LPARAM lp) const; 
	HWND get_hwnd_status_bar() const;
};

class Program_Manager : public Edit_Manager, public Menu_Manager, public Printer_Manager, public File_Manager, public Stack_Manager, public Edit_Window, public Edit_Friend_Text_Window, public Pop_Up_Menu, public Status_Bar {
public:
	Program_Manager(HWND window);
	virtual ~Program_Manager();
	virtual void activator() override;
};