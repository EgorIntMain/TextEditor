#pragma once
#include "TextLine.h"

class Text {
	vector <TextLine> text;
public:
	Text();
	Text(const wstring& data);
	~Text();
	void sort_strings_in_ascending_order();
	void setter(const wstring& data);
	wstring getter() const;
	void clear();
	size_t size() const;
	inline TextLine& operator[](const size_t index);
	void big_letters();
	void small_letters();
	void depending_on_the_letter();
	void delete_repeat_symbols();
	void sort_strings_alphabetically();
	void sort_strings_non_alphabetically();
	void sort_strings_in_descending_order();
	void delete_repeat_strings();
	void delete_repeat_words();
	void merge_strings();
	constexpr vector<TextLine>::iterator begin() noexcept { return text.begin(); };
	constexpr vector<TextLine>::iterator end() noexcept { return text.end(); };
};