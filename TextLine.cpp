#include "TextLine.h"
#include <algorithm>
#include <cwctype>
#include <clocale>
#include <regex>
#include <ranges>

void TextLine::push_back(const wchar_t j)
{
	line.push_back(j);
}

void TextLine::sort_string_in_ascending_order()
{
	vector <wstring> collect_substr = make_vector_of_substring();

	sort(collect_substr.begin(), collect_substr.end(), [](const wstring& first, const wstring& second) {return first.size() < second.size(); });

	get_back_to_string(collect_substr);
}

void TextLine::sort_string_in_descending_order()
{
	vector <wstring> collect_substr = make_vector_of_substring();

	sort(collect_substr.begin(), collect_substr.end(), [](const wstring& first, const wstring& second) {return first.size() > second.size(); });

	get_back_to_string(collect_substr);
}

vector <wstring> TextLine::make_vector_of_substring() const
{
	vector <wstring> collect_substr;
	std::wregex word_regex(L"\\w+");

	for (std::wsregex_iterator it(line.begin(), line.end(), word_regex); it != std::wsregex_iterator(); ++it)
		collect_substr.push_back(it->str());

	return collect_substr;
}

vector<wchar_t> TextLine::make_vector_of_separator_symbols() const
{
	vector <wchar_t> sep;
	std::wregex punct_regex(L"[[:punct:][:space:]]");

	for (const wchar_t symbol : line)
		if (std::regex_match(wstring(1, symbol), punct_regex))
			sep.push_back(symbol);

	return sep;
}

void TextLine::clear()
{
	line.clear();
}

void TextLine::get_back_to_string(const vector <wstring>& collect_substr)
{
	vector <wchar_t> separator = make_vector_of_separator_symbols();
	separator.push_back('\0');

	clear();

	for (const auto& [str, sep] : std::ranges::views::zip(collect_substr, separator))
		line += str + sep;
}

void TextLine::sort_string_alphabetically()
{
	vector <wstring> collect_substr = make_vector_of_substring();

	sort(collect_substr.begin(), collect_substr.end());

	get_back_to_string(collect_substr);
}

void TextLine::sort_string_non_alphabetically()
{
	vector <wstring> collect_substr = make_vector_of_substring();

	sort(collect_substr.rbegin(), collect_substr.rend());

	get_back_to_string(collect_substr);
}

void TextLine::delete_repeat_symbols()
{
	wstring tmp_str;

	for (const wchar_t symbol : line)
		if (find(tmp_str.begin(), tmp_str.end(), symbol) == tmp_str.end())
			tmp_str.push_back(symbol);

	line = move(tmp_str);
}

void TextLine::big_letters()
{
	setlocale(LC_ALL, ".UTF8");

	for (wchar_t& symbol : line)
		symbol = towupper(symbol);
}

void TextLine::small_letters()
{
	setlocale(LC_ALL, ".UTF8");

	for (wchar_t& symbol : line)
		symbol = towlower(symbol);
}

wchar_t& TextLine::operator[](const size_t index)
{
	return line[index];
}

void TextLine::depending_on_the_letter()
{
	setlocale(LC_ALL, ".UTF8");

	for (wchar_t& symbol : line)
		if (iswlower(symbol))
			symbol = towupper(symbol);
		else if (iswupper(symbol))
			symbol = towlower(symbol);
}

bool TextLine::operator==(const TextLine& Text_line) const
{
	return line == Text_line.line;
}

TextLine::TextLine()
{
	line = L"";
}

TextLine::TextLine(const TextLine& Text_line)
{
	line = Text_line.line;
}

TextLine::TextLine(TextLine&& Text_line) noexcept
{
	line = move(Text_line.line);
}

TextLine::TextLine(wstring&& basic_line) noexcept
{
	line = move(basic_line);
}

TextLine::TextLine(const wstring& basic_line)
{
	line = basic_line;
}

TextLine::~TextLine()
{
	line.~basic_string();
}

TextLine& TextLine::operator=(const TextLine& Text_line)
{
	line = Text_line.line;

	return *this;
}

void TextLine::delete_repeat_words_in_string()
{
	vector <wstring> collect_substr = make_vector_of_substring();

	vector <wstring> tmp_vec;

	for (wstring& tmp_line : collect_substr)
		if (find(tmp_vec.begin(), tmp_vec.end(), tmp_line) == tmp_vec.end())
			tmp_vec.push_back(move(tmp_line));

	collect_substr = move(tmp_vec);

	get_back_to_string(collect_substr);
}

void TextLine::setter(const wstring& basic_line)
{
	line = basic_line;
}

const wstring& TextLine::getter() const
{
	return line;
}

void TextLine::erase(const size_t index)
{
	line.erase(line.begin() + index);
}

size_t TextLine::size() const
{
	return line.size();
}