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

#include <string>
#include <Windows.h>
#include "curl/curl.h"
#include "stringUtil.h"
#include <filesystem>

namespace fs = std::filesystem;

class curlFileTransfer {

private:
	static std::wstring extractFilename(const std::wstring& filePath);
	static size_t WriteData(void* buffer, size_t size, size_t nmemb, void* userp);
	static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
	static size_t readCallback(char* buffer, size_t size, size_t nitems, void* stream);
	static bool isDataServerAvailable(const std::string& url);

public:		/* Public API */
	static bool DownloadFileFromURL(const std::wstring& url, const std::wstring& destDirPath, std::wstring& errorMsg);
	static bool UploadFileToURL(const std::wstring& url, const std::wstring& filePath, std::wstring& errorMsg);
	static bool UploadDirectoryToURL(const std::wstring& url, const std::wstring& dirPath, std::wstring& errorMsg, const std::wstring& extensions = L"");
};