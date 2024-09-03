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

#include "fileTransferService.h"
#include <fstream>


std::wstring curlFileTransfer::extractFilename(const std::wstring& filePath) {
	const size_t lastSlash = filePath.find_last_of(L"/\\"); // Find the last slash or backslash
	if (lastSlash != std::wstring::npos) {
		return filePath.substr(lastSlash + 1); // Extract the filename part
	}
	return filePath; // If no slashes or backslashes found, return the original path as the filename
}

size_t curlFileTransfer::WriteData(void* buffer, size_t size, size_t nmemb, void* userp) {
	std::ofstream* file = static_cast<std::ofstream*>(userp);
	file->write(static_cast<const char*>(buffer), size * nmemb);
	return size * nmemb;
}

size_t curlFileTransfer::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	return size * nmemb;
}

bool curlFileTransfer::isDataServerAvailable(const std::string& url) {

	CURL* curl = curl_easy_init();
	if (!curl) {
		//    std::cerr << "Curl initialization failed." << std::endl;
		return false;
	}

	//    std::string url = "http://" + serverIP + ":" + std::to_string(port);
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	int res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	if (res == CURLE_OK) {
		long responseCode;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
		if (responseCode == 200) {
			curl_easy_cleanup(curl);
			return true;  // Port is open and responded with a 200 OK.
		}
	}
	curl_easy_cleanup(curl);
	return false;  // Port is either closed or didn't respond as expected.
}


bool curlFileTransfer::DownloadFileFromURL(const std::wstring &url, const std::wstring &destDirPath, std::wstring &errorMsg) {

	CURL* curl = curl_easy_init();
	if (!curl) {
		errorMsg = L"Failed to initialize libcurl";
		return false;
	}

	std::wstring outputFilePath = destDirPath + L"/" + url.substr(url.find_last_of('/') + 1);
	std::ofstream outputFile(outputFilePath, std::ios::binary);
	if (!outputFile) {
		errorMsg = L"Failed to open output file: " + outputFilePath;
		curl_easy_cleanup(curl);
		return false;
	}

	curl_easy_setopt(curl, CURLOPT_URL, StringUtils::ws2s(url).c_str());
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outputFile);

	CURLcode res = curl_easy_perform(curl);
	if (res != 0) {
		errorMsg = L"Failed to download file. curlError: " + StringUtils::s2ws(curl_easy_strerror(res));
		curl_easy_cleanup(curl);
		outputFile.close();
		return false;
	}
	curl_easy_cleanup(curl);
	outputFile.close();
	return true;
}

bool curlFileTransfer::UploadFileToURL(const std::wstring &url, const std::wstring &filePath, std::wstring &errorMsg) {

	// Open the file for reading
	std::ifstream fileStream(filePath, std::ios::binary);
	if (!fileStream.is_open()) {
		// Handle file opening failure
		errorMsg = filePath + L": file opening failure";
		return false;
	}
	// Get the file size
	fileStream.seekg(0, std::ios::end);
	if (static_cast<int>(fileStream.tellg()) == -1) { return false; } // Stream does not support the operation, or if it fails, the function returns -1

	curl_off_t fileSize = fileStream.tellg(); if (fileSize == 0) { return false; } // Don't proceed if size is ZERO
	fileStream.seekg(0, std::ios::beg);
	std::wstring filename = extractFilename(filePath);
	std::string filenameUtf8 = StringUtils::convertWStringToUTF8(filename);

	CURL* curl = curl_easy_init();
	if (!curl) {
		errorMsg = L"Failed to initialize libcurl";
		return false;
	}
	struct curl_httppost *formPost = nullptr;
	struct curl_httppost *lastPtr = nullptr;

	CURLFORMcode formAddResult = curl_formadd(&formPost, &lastPtr,
		CURLFORM_COPYNAME, "file",
		CURLFORM_FILE, StringUtils::ws2s(filePath).c_str(),
		CURLFORM_END);

	if (formAddResult != CURL_FORMADD_OK) {
		errorMsg = L"Failed to add form data: " + std::to_wstring(formAddResult);
		curl_easy_cleanup(curl);
		return false;
	}

	// Set options for the POST request
	curl_easy_setopt(curl, CURLOPT_URL, StringUtils::ws2s(url).c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPPOST, formPost);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "clienthttp (Windows NT; x86)");

	// Enable verbose mode to get detailed debug output
	// curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);		// DELETE THIS

	int retValue = curl_easy_perform(curl);
	if (retValue != CURLE_OK) {
		if (retValue == CURLE_COULDNT_CONNECT) {
			errorMsg = L"Couldn't connect to Data Server i.e.  " + url;
		}
		errorMsg = L"Failed to upload file. curlErrorCode " + std::to_wstring(retValue);
	}
	curl_easy_cleanup(curl);
	curl_formfree(formPost);
	return (retValue == CURLE_OK);
}

bool curlFileTransfer::UploadDirectoryToURL(const std::wstring &url, const std::wstring &dirPath, std::wstring &errorMsg, const std::wstring &extensions) {

	std::vector<std::wstring> filesToUpload;
	std::vector<std::string> extensions_vec = StringUtils::extract_items_from_str(StringUtils::ws2s(extensions), ",");
	std::error_code ec;

	for (const auto& entry : fs::recursive_directory_iterator(dirPath, fs::directory_options::skip_permission_denied, ec)) {
		std::wstring filePath;
		auto fileType = fs::status(entry, ec).type();    // To get error_code status
		if (!ec) {
			if (fileType == fs::file_type::regular) {
				filePath = entry.path().wstring();
				if (extensions.empty()) {
					filesToUpload.push_back(filePath);
				}
				else {
					std::string extension = entry.path().extension().string();
					for (auto& ext : extensions_vec) {
						if (ext == extension) {
							filesToUpload.push_back(filePath);
							break;
						}
					}
				}
			}
		}
		else {
			continue;   // Skip the file which causes error
		}
	}
	if (filesToUpload.empty()) {
		return false;
	}

	for (const auto& filePath : filesToUpload) {
		if (!UploadFileToURL(url, filePath, errorMsg)) {      // Upload a file            
			errorMsg = L"Couldn't upload: " + filePath;
		}
	}
	return true;
}
