#pragma once
#include <string>
#include <vector>

using std::wstring;
using std::vector;

class TextLine {
	wstring line;
	vector<wchar_t> make_vector_of_separator_symbols() const;
	void get_back_to_string(const vector <wstring>& collect_substr);
	vector <wstring> make_vector_of_substring() const;
public:
	const wstring& getter() const;
	size_t size() const;
	void clear();
	void erase(const size_t index);
	void push_back(const wchar_t j);
	void setter(const wstring& line);
	wchar_t& operator[](const size_t index);
	bool operator==(const TextLine& Text_line) const;
	explicit TextLine(const TextLine& Text_line);
	TextLine();
	~TextLine();
	explicit TextLine(TextLine&& Text_line) noexcept;
	explicit TextLine(wstring&& basic_line) noexcept;
	explicit TextLine(const wstring& basic_line);
	TextLine& operator=(const TextLine& Text_line);
	void sort_string_in_ascending_order();
	void sort_string_in_descending_order();
	void sort_string_alphabetically();
	void sort_string_non_alphabetically();
	void delete_repeat_words_in_string();
	void delete_repeat_symbols();
	void big_letters();
	void small_letters();
	void depending_on_the_letter();
	constexpr wchar_t* data() noexcept { return line.data(); };
	constexpr const wchar_t* data() const noexcept { return line.data(); };
	constexpr wstring::iterator begin() noexcept { return line.begin(); };
	constexpr wstring::iterator end() noexcept { return line.end(); };
};