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

#include "filemanager.h"
#include <codecvt>
#include "json.h"

/* ================================ PRIVATE FUNCTIONS ================================*/

// std::string to std::wstring
std::wstring s2ws(const std::string& str) {

	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	try { return converterX.from_bytes(str); }
	catch (const std::range_error& e) { return converterX.from_bytes(e.what()); }
}

// Extract the filename from the input path
std::wstring extractFilename(const std::wstring& filePath) {
	const size_t lastSlash = filePath.find_last_of(L"/\\"); // Find the last slash or backslash
	if (lastSlash != std::wstring::npos) {
		return filePath.substr(lastSlash + 1); // Extract the filename part
	}
	return filePath; // If no slashes or backslashes found, return the original path as the filename
}


/* ================================ PUBLIC APIs ================================*/

std::wstring filemanager(const std::wstring &dirToList) {

	std::wstring dirInfo{ L"{\"files\":[" };
	std::vector<std::wstring> fileList_json;
	std::wstring drive{ L"" };                // Add a drive full path here i.e. /, C:/, D:/, F:/
	std::error_code ec;

	auto it = fs::directory_iterator(dirToList, fs::directory_options::skip_permission_denied, ec);
	for (auto i = fs::begin(it); i != fs::end(it); i.increment(ec)) {
		auto entry = *i;
		if (fs::is_symlink(entry, ec)) {
			continue;
		}
		fileList_json.push_back(L"name");
		std::wstring path{ entry.path() };
		std::wstring filename{ extractFilename(path) };
		std::string sizeInBytes;
		if (fs::is_directory(path, ec)) {
			fileList_json.push_back(filename + L"/");
			//         size_t directorySize = calculateDirectorySize(path);          // THIS CONSUMES TOO MUCH TIME, NOT EFFICIENT !!!
			//         size_t directorySize = 0;
			//         if (directorySize == static_cast<size_t>(-1)) { sizeInBytes = "N/A"; }
			//         else { sizeInBytes = std::to_string(directorySize); }
			sizeInBytes = "N/A";
		}
		else {
			fileList_json.push_back(filename);
			sizeInBytes = std::to_string(fs::file_size(path, ec));
		}
		fileList_json.push_back(L"size");
		fileList_json.push_back(s2ws(sizeInBytes));
		dirInfo.append(JsonUtil::to_json(fileList_json));
		dirInfo.append(L",");
		fileList_json.clear();
	}
		dirInfo.pop_back();
		dirInfo.append(L"],\"dirToList\":[\"");
		dirInfo.append(dirToList);
		dirInfo.append(L"\"],");
		dirInfo.append(L"\"drive\":[\"");
		dirInfo.append(drive);
		dirInfo.append(L"\"]}");

		return dirInfo;
}
