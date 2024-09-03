// Copyright (c) Nouman Tajik [github.com/tajiknomi]
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE. 


#include "stringUtil.h"
#include <codecvt>

bool StringUtils::endsWith(const std::wstring_view &str, const std::wstring_view &suffix) {
	return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

bool StringUtils::startsWith(const std::wstring_view &str, const std::wstring_view &prefix) {
	return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}

std::string StringUtils::ws2s(const std::wstring& wstr) {

	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	try { return converterX.to_bytes(wstr); }
	catch (const std::range_error& e) { return e.what(); }
}

std::wstring StringUtils::s2ws(const std::string& str) {

	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	try { return converterX.from_bytes(str); }
	catch (const std::range_error& e) { return converterX.from_bytes(e.what()); }
}

std::string StringUtils::convertWStringToUTF8(const std::wstring& wstr) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> utf8_converter;
	return utf8_converter.to_bytes(wstr);
}

std::vector<std::string> StringUtils::extract_items_from_str(const std::string& input_str, const std::string& delimiter) {

	std::string str(input_str);
	// Extract strings using a delimiter
	size_t pos = 0;
	std::string token;
	std::vector<std::string> items;

	while ((pos = str.find(delimiter)) != std::string::npos) {
		items.push_back(str.substr(0, pos));
		str.erase(0, pos + delimiter.length());
	}
	items.push_back(str.data());

	return items;
}
