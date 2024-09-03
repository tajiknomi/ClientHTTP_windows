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


#include "json.h"
#include <codecvt>
#include <iostream>

/* ================================ PRIVATE FUNCTIONS ================================ */

std::string JsonUtil::escape_json(const std::wstring& wstr) {
	std::string str(wstr.begin(), wstr.end());
	std::string result;
	for (char ch : str) {
		switch (ch) {
		case '\"': result += "\\\""; break;
		case '\\': result += "\\\\"; break;
		case '\b': result += "\\b";  break;
		case '\f': result += "\\f";  break;
		case '\n': result += "\\n";  break;
		case '\r': result += "\\r";  break;
		case '\t': result += "\\t";  break;
		default:   result += ch;     break;
		}
	}
	return result;
}

// Helper function to trim whitespace from both ends of a string
std::string JsonUtil::trim(const std::string& str) {
	size_t first = str.find_first_not_of(" \t\n\r");
	if (first == std::string::npos) return "";
	size_t last = str.find_last_not_of(" \t\n\r");
	return str.substr(first, (last - first + 1));
}

// Helper function to remove quotes from a string
std::string JsonUtil::remove_quotes(const std::string& str) {
	if (str.size() >= 2 && str[0] == '"' && str.back() == '"') {
		return str.substr(1, str.size() - 2);
	}
	return str;
}

// Helper function to parse JSON string and extract key-value pairs
std::vector<std::wstring> JsonUtil::parse_json(const std::string& json) {
	std::vector<std::wstring> parsedData;
	size_t pos = 0;
	std::string key, value;
	bool inString = false;
	bool isKey = true;

	while (pos < json.size()) {
		char ch = json[pos];

		if (ch == '"') {  // Toggle inString flag
			inString = !inString;
		}
		else if (inString) {  // Inside a string, capture characters
			if (isKey) {
				key += ch;
			}
			else {
				value += ch;
			}
		}
		else if (ch == ':') {  // End of a key, start of a value
			isKey = false;
		}
		else if (ch == ',' || ch == '}') {  // End of a key-value pair
			key = remove_quotes(key);
			value = remove_quotes(value);
			parsedData.push_back(std::wstring(key.begin(), key.end()));
			parsedData.push_back(std::wstring(value.begin(), value.end()));

			// Reset for next key-value pair
			key.clear();
			value.clear();
			isKey = true;
		}

		++pos;
	}

	return parsedData;
}

// Helper function to find a value for a given key in a JSON object string
std::string JsonUtil::find_json_value(const std::string& json, const std::string& key) {
	size_t keyPos = json.find("\"" + key + "\"");
	if (keyPos == std::string::npos) {
		return "";  // Key not found
	}

	size_t colonPos = json.find(":", keyPos);
	if (colonPos == std::string::npos) {
		return "";  // Invalid JSON format
	}

	size_t valueStart = json.find_first_not_of(" \t\n\r", colonPos + 1);
	if (valueStart == std::string::npos) {
		return "";  // No value found
	}

	// Handle string values
	if (json[valueStart] == '"') {
		size_t valueEnd = json.find('"', valueStart + 1);
		if (valueEnd == std::string::npos) {
			return "";  // Invalid JSON format
		}
		return json.substr(valueStart + 1, valueEnd - valueStart - 1);
	}

	// Handle other types of values (e.g., numbers, booleans)
	size_t valueEnd = json.find_first_of(",}", valueStart);
	return trim(json.substr(valueStart, valueEnd - valueStart));
}

// Helper function to escape double quotes and backslashes in a string
std::string JsonUtil::escape_json_string(const std::string& input) {
	std::string escaped;
	for (char c : input) {
		if (c == '"' || c == '\\') {
			escaped += '\\';  // Add escape character before quotes and backslashes
		}
		escaped += c;
	}
	return escaped;
}

// Helper function to add a new key-value pair to a JSON object string
std::string JsonUtil::append_json_key_value(const std::string& json, const std::string& key, const std::string& value) {
	std::string result = trim(json);

	// Remove trailing whitespace and the closing brace '}'
	if (result.back() == '}') {
		result.pop_back();
		result = trim(result); // Trim any whitespace again after popping back
	}
	else {
		return json;  // Invalid JSON format, return original data
	}

	// If the JSON is not empty (contains other key-value pairs), add a comma and newline
	if (result.size() > 1) {
		result += ",\r\n";
	}
	else {
		result += "\r\n";  // For an empty JSON object, just add a newline
	}

	// Escape the value string to make it JSON-compatible
	std::string escapedValue = escape_json_string(value);

	// Append the new key-value pair
	result += "  \"" + key + "\": \"" + escapedValue + "\"\r\n";

	// Close the JSON object with a closing brace and newline
	result += "}";

	return result;
}



/* ================================ PUBLIC APIs ================================ */


std::wstring JsonUtil::to_json(const std::vector<std::wstring>& data) {
	if (data.size() % 2 != 0) {
		return L"{}"; // Return empty JSON object if the vector size is not even
	}

	std::string json = "{";
	for (size_t i = 0; i < data.size(); i += 2) {
		std::wstring key = data[i];
		std::wstring value = data[i + 1];

		json += "\"" + escape_json(key) + "\":\"" + escape_json(value) + "\"";

		if (i + 2 < data.size()) { // If not the last pair, add a comma
			json += ",";
		}
	}
	json += "}";

	// Convert UTF-8 encoded JSON string to std::wstring
	return std::wstring(json.begin(), json.end());
}

std::vector<std::wstring> JsonUtil::from_json(const std::wstring& jsonData) {

	// Convert the input jsonData (std::wstring) to UTF-8 encoded std::string
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::string utf8jsonData = converter.to_bytes(jsonData);

	// Trim whitespace and check if it is a valid JSON object
	utf8jsonData = trim(utf8jsonData);
	if (utf8jsonData.front() != '{' || utf8jsonData.back() != '}') {
		return {};  // Return empty vector if not a valid JSON object
	}

	// Parse JSON manually
	return parse_json(utf8jsonData);
}

std::wstring JsonUtil::extractValue(const std::wstring& jsonData, const std::wstring& key) {

	// Convert the input jsonData and key to UTF-8 encoded std::string
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::string utf8jsonData = converter.to_bytes(jsonData);
	std::string utf8key = converter.to_bytes(key);

	// Trim whitespace and check if it is a valid JSON object
	utf8jsonData = trim(utf8jsonData);
	if (utf8jsonData.front() != '{' || utf8jsonData.back() != '}') {
		return L"";  // Return empty string if not a valid JSON object
	}

	// Find the value for the given key
	std::string utf8Value = find_json_value(utf8jsonData, utf8key);

	// Convert the value from UTF-8 to std::wstring
	return converter.from_bytes(utf8Value);
}

std::wstring JsonUtil::appendKeyValue(const std::wstring& jsonData, const std::wstring& key, const std::wstring& value) {

	// Convert the input jsonData, key, and value to UTF-8 encoded std::string
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::string utf8jsonData = converter.to_bytes(jsonData);
	std::string utf8key = converter.to_bytes(key);
	std::string utf8value = converter.to_bytes(value);

	// Trim whitespace and check if it is a valid JSON object
	utf8jsonData = trim(utf8jsonData);
	if (utf8jsonData.front() != '{' || utf8jsonData.back() != '}') {
		return jsonData;  // Return original data if not a valid JSON object
	}

	// Append the new key-value pair to the JSON string
	std::string utf8Result = append_json_key_value(utf8jsonData, utf8key, utf8value);

	// Convert the UTF-8 encoded JSON string to std::wstring
	return converter.from_bytes(utf8Result);
}


