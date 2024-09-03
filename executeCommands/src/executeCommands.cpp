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


#include "executeCommands.h"
#include <Windows.h>
#include <vector>
#include <iostream>

// EXECUTE_COMMAND_API std::wstring executeCommand(const std::wstring& command, const std::wstring& args) {
std::wstring executeCommand(const std::wstring &command, const std::wstring &args) {
    
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    HANDLE hChildStdoutRd, hChildStdoutWr;
    if (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)) {
        return L"Pipe creation failed";
    }

    HANDLE hProcess = NULL;
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;

    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));

    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdOutput = hChildStdoutWr;
    siStartInfo.hStdError = hChildStdoutWr;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    // Create the command line
    std::wstring fullCommand = command + L" " + args;
    LPWSTR cmdLine = const_cast<wchar_t*>(fullCommand.c_str());
    BOOL bSuccess = CreateProcess(NULL, cmdLine, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &siStartInfo, &piProcInfo);

    if (!bSuccess) {
        CloseHandle(hChildStdoutRd);
        CloseHandle(hChildStdoutWr);
        return L"Process creation failed";
    }

    CloseHandle(hChildStdoutWr);

    // Determine the code page of the console
    UINT codePage = GetConsoleCP();         // Depends on the system's locale and regional settings
   
    // Read the output from the child process
    DWORD dwRead;
    CHAR chBuf[4096];
    std::wstring output;

    while (ReadFile(hChildStdoutRd, chBuf, sizeof(chBuf), &dwRead, NULL) && dwRead > 0) {
        int numWChars = MultiByteToWideChar(codePage, 0, chBuf, dwRead, NULL, 0);
        if (numWChars > 0) {
            std::vector<wchar_t> wBuffer(numWChars);
            MultiByteToWideChar(codePage, 0, chBuf, dwRead, wBuffer.data(), numWChars);
            output.append(wBuffer.data(), numWChars);
        }
    }

    CloseHandle(hChildStdoutRd);

    // Wait for the child process to exit
    WaitForSingleObject(piProcInfo.hProcess, INFINITE);

    DWORD exitCode;
    GetExitCodeProcess(piProcInfo.hProcess, &exitCode);

    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

    output += L" | Child process exited with status: " + std::to_wstring(exitCode);

    return output;
}