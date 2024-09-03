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


#pragma once
#include <vector>
#include <string>

class JsonUtil {

private:
	static std::string escape_json(const std::wstring& wstr);
	static std::vector<std::wstring> parse_json(const std::string& json);
	static std::string remove_quotes(const std::string& str);
	static std::string trim(const std::string& str);
	static std::string find_json_value(const std::string& json, const std::string& key);
	static std::string append_json_key_value(const std::string& json, const std::string& key, const std::string& value);
	static std::string escape_json_string(const std::string& input);

public:
	// data -----> json
	static std::wstring to_json(const std::vector<std::wstring>& data);

	// json -----> data
	static std::vector<std::wstring> from_json(const std::wstring& jsonData);

	// Extract <value> from json using <key>
	static std::wstring extractValue(const std::wstring& jsonData, const std::wstring& key);

	// Insert a key-value pair into an existing JSON string
	static std::wstring appendKeyValue(const std::wstring& jsonData, const std::wstring& key, const std::wstring& value);
};
