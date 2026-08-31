#include "Text.h"
#include <algorithm>
#include <functional>

using std::move;
using std::mem_fn;

void Text::clear()
{
	text.clear();
}

void Text::merge_strings()
{
	wstring str;

	for (const TextLine& line: text)
		str += line.getter() + L' ';

	str.pop_back();

	clear();

	text.push_back(TextLine(str));
}

void Text::sort_strings_in_descending_order()
{
	for_each(text.begin(), text.end(), mem_fn(&TextLine::sort_string_in_descending_order));
}

void Text::sort_strings_in_ascending_order()
{
	for_each(text.begin(), text.end(), mem_fn(&TextLine::sort_string_in_ascending_order));
}

void Text::sort_strings_alphabetically()
{
	for_each(text.begin(), text.end(), mem_fn(&TextLine::sort_string_alphabetically));
}

void Text::sort_strings_non_alphabetically()
{
	for_each(text.begin(), text.end(), mem_fn(&TextLine::sort_string_non_alphabetically));
}

void Text::delete_repeat_symbols()
{
	for_each(text.begin(), text.end(), mem_fn(&TextLine::delete_repeat_symbols));
}

void Text::big_letters()
{
	for_each(text.begin(), text.end(), mem_fn(&TextLine::big_letters));
}

void Text::small_letters()
{
	for_each(text.begin(), text.end(), mem_fn(&TextLine::small_letters));
}

void Text::depending_on_the_letter()
{
	for_each(text.begin(), text.end(), mem_fn(&TextLine::depending_on_the_letter));
}

void Text::delete_repeat_strings()
{
	vector <TextLine> tmp_vec;

	for (TextLine& line : text)
		if (find(tmp_vec.begin(), tmp_vec.end(), line) == tmp_vec.end())
			tmp_vec.push_back(move(line));

	text = move(tmp_vec);
}

void Text::delete_repeat_words()
{
	for_each(text.begin(), text.end(), mem_fn(&TextLine::delete_repeat_words_in_string));
}

Text::Text(const wstring& data)
{
	setter(data);
}

void Text::setter(const wstring& data)
{
	if (text.begin() != text.end())
		clear();

	TextLine line;

	for (const wchar_t& symbol : data)
	{
		if (symbol != L'\0' && symbol != L'\r' && symbol != L'\n')
			line.push_back(symbol);
		else
			text.push_back(move(line));
	}
}

Text::~Text()
{
	text.~vector();
}

inline TextLine& Text::operator[](const size_t index)
{
	return text[index];
}

Text::Text()
{
	text = vector<TextLine>();
}

wstring Text::getter() const
{
	wstring data;

	for (const TextLine& line : text)
		data += line.getter() + L"\r\n";

	data.pop_back();
	data.pop_back();

	return data;
}

size_t Text::size() const
{
	return text.size();
}