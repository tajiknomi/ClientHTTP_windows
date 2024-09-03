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
#include <string>
#include <codecvt>



std::wstring s2ws(const std::string& str) {

	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	try { return converterX.from_bytes(str); }
	catch (const std::range_error& e) { return converterX.from_bytes(e.what()); }
}

std::string ws2s(const std::wstring& wstr) {

	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	try { return converterX.to_bytes(wstr); }
	catch (const std::range_error& e) { return e.what(); }
}


namespace mysocket {
	/* Private functions */
	bool WinSocket::isConnected(void) {	
		return http_socket != INVALID_SOCKET;
	}
	/* Error Handlers for winsocket API's */
	void WinSocket::Err_handle_WSAStartup(const int &err_code) {
		if (err_code != 0) {
			switch (err_code)
			{
			case WSASYSNOTREADY:		std::cerr << "WSAStartup() : The underlying network subsystem is not ready for network communication" << std::endl; exit(EXIT_FAILURE);
			case WSAVERNOTSUPPORTED:	std::cerr << "WSAStartup() : The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation" << std::endl; exit(EXIT_FAILURE);
			case WSAEINPROGRESS:		std::cerr << "WSAStartup() : A blocking Windows Sockets 1.1 operation is in progress" << std::endl; exit(EXIT_FAILURE);
			case WSAEPROCLIM:			std::cerr << "WSAStartup() : A limit on the number of tasks supported by the Windows Sockets implementation has been reached" << std::endl; exit(EXIT_FAILURE);
			case WSAEFAULT:				std::cerr << "WSAStartup() : WSAStartup error : The lpWSAData parameter is not a valid pointer" << std::endl; exit(EXIT_FAILURE);
			default:					std::cerr << "WSAStartup() : This error code doesn't exist" << std::endl; exit(EXIT_FAILURE);
			}
		}
	}
	void WinSocket::Err_handle_getaddrinfo(const int &err_code) {
		if (err_code != 0) {
			switch (err_code)
			{
			case EAI_AGAIN:		std::cerr << "getaddrinfo() : A temporary failure in name resolution occurred" << std::endl; exit(EXIT_FAILURE);
			case EAI_BADFLAGS:	std::cerr << "getaddrinfo() : An invalid value was provided for the ai_flags member of the pHints parameter" << std::endl; exit(EXIT_FAILURE);
			case EAI_FAIL:		std::cerr << "getaddrinfo() : A nonrecoverable failure in name resolution occurred" << std::endl; exit(EXIT_FAILURE);
			case EAI_FAMILY:	std::cerr << "getaddrinfo() : The ai_family member of the pHints parameter is not supported" << std::endl; exit(EXIT_FAILURE);
			case EAI_MEMORY:	std::cerr << "getaddrinfo() : A memory allocation failure occurred" << std::endl; exit(EXIT_FAILURE);
			case EAI_NONAME:	std::cerr << "getaddrinfo() : The name does not resolve for the supplied parameters or the pNodeName and pServiceName parameters were not provided" << std::endl; exit(EXIT_FAILURE);
			case EAI_SERVICE:	std::cerr << "getaddrinfo() : The pServiceName parameter is not supported for the specified ai_socktype member of the pHints parameter" << std::endl; exit(EXIT_FAILURE);
			case EAI_SOCKTYPE:	std::cerr << "getaddrinfo() : The ai_socktype member of the pHints parameter is not supported" << std::endl; exit(EXIT_FAILURE);
			default:			std::cerr << "getaddrinfo() : This error code doesn't exist" << std::endl; exit(EXIT_FAILURE);
			}
		}
	}
	void WinSocket::Err_handle_socket(const SOCKET &ret_val) {
		if (ret_val == INVALID_SOCKET)
		{
			const int &err_code = WSAGetLastError();
			switch (err_code)
			{
			case WSANOTINITIALISED:		std::cerr << "socket() : A 0ful WSAStartup call must occur before using this function" << std::endl; exit(EXIT_FAILURE);
			case WSAENETDOWN:			std::cerr << "socket() : The network subsystem or the associated service provider has failed" << std::endl; exit(EXIT_FAILURE);
			case WSAEAFNOSUPPORT:		std::cerr << "socket() : The specified address family is not supported. For example, an application tried to create a socket for the AF_IRDA address family but an infrared adapter and device driver is not installed on the local computer" << std::endl; exit(EXIT_FAILURE);
			case WSAEINPROGRESS:		std::cerr << "socket() : A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function" << std::endl; exit(EXIT_FAILURE);
			case WSAEMFILE:				std::cerr << "socket() : No more socket descriptors are available" << std::endl; exit(EXIT_FAILURE);
			case WSAEINVAL:				std::cerr << "socket() : An invalid argument was supplied. This error is returned if the af parameter is set to AF_UNSPEC and the type and protocol parameter are unspecified" << std::endl; exit(EXIT_FAILURE);
			case WSAEINVALIDPROVIDER:	std::cerr << "socket() : The service provider returned a version other than 2.2" << std::endl; exit(EXIT_FAILURE);
			case WSAEINVALIDPROCTABLE:	std::cerr << "socket() : The service provider returned an invalid or incomplete procedure table to the WSPStartup" << std::endl; exit(EXIT_FAILURE);
			case WSAENOBUFS:			std::cerr << "socket() : No buffer space is available. The socket cannot be created" << std::endl; exit(EXIT_FAILURE);
			case WSAEPROTONOSUPPORT:	std::cerr << "socket() : The specified protocol is not supported" << std::endl; exit(EXIT_FAILURE);
			case WSAEPROTOTYPE:			std::cerr << "socket() : The specified protocol is the wrong type for this socket" << std::endl; exit(EXIT_FAILURE);
			case WSAEPROVIDERFAILEDINIT:std::cerr << "socket() : The service provider failed to initialize. This error is returned if a layered service provider (LSP) or namespace provider was improperly installed or the provider fails to operate correctly" << std::endl; exit(EXIT_FAILURE);
			case WSAESOCKTNOSUPPORT:	std::cerr << "socket() : The specified socket type is not supported in this address family" << std::endl; exit(EXIT_FAILURE);
			default:					std::cerr << "socket() : This error code doesn't exist" << std::endl; exit(EXIT_FAILURE);
			}
		}
	}
	void WinSocket::Err_handle_WSASocket(const SOCKET &ret_val) {
		if (ret_val == INVALID_SOCKET) {

			const int &err_code = WSAGetLastError();
			switch (err_code)
			{
			case WSANOTINITIALISED:			std::cerr << "WSASocket() returns error code : WSANOTINITIALISED" << std::endl;	exit(EXIT_FAILURE);
			case WSAENETDOWN:				std::cerr << "WSASocket() returns error code : WSAENETDOWN" << std::endl;			exit(EXIT_FAILURE);
			case WSAEAFNOSUPPORT:			std::cerr << "WSASocket() returns error code : WSAEAFNOSUPPORT" << std::endl;		exit(EXIT_FAILURE);
			case WSAEFAULT:					std::cerr << "WSASocket() returns error code : WSAEFAULT" << std::endl;			exit(EXIT_FAILURE);
			case WSAEINPROGRESS:			std::cerr << "WSASocket() returns error code : WSAEINPROGRESS" << std::endl;		exit(EXIT_FAILURE);
			case WSAEINVAL:					std::cerr << "WSASocket() returns error code : WSAEINVAL" << std::endl;			exit(EXIT_FAILURE);
			case WSAEINVALIDPROVIDER:		std::cerr << "WSASocket() returns error code : WSAEINVALIDPROVIDER" << std::endl;  exit(EXIT_FAILURE);
			case WSAEINVALIDPROCTABLE:		std::cerr << "WSASocket() returns error code : WSAEINVALIDPROCTABLE" << std::endl; exit(EXIT_FAILURE);
			case WSAEMFILE:					std::cerr << "WSASocket() returns error code : WSAEMFILE" << std::endl;			exit(EXIT_FAILURE);
			case WSAENOBUFS:				std::cerr << "WSASocket() returns error code : WSAENOBUFS" << std::endl;			exit(EXIT_FAILURE);
			case WSAEPROTONOSUPPORT:		std::cerr << "WSASocket() returns error code : WSAEPROTONOSUPPORT" << std::endl;	exit(EXIT_FAILURE);
			case WSAEPROTOTYPE:				std::cerr << "WSASocket() returns error code : WSAEPROTOTYPE" << std::endl;		exit(EXIT_FAILURE);
			case WSAEPROVIDERFAILEDINIT:	std::cerr << "WSASocket() returns error code : WSAEPROVIDERFAILEDINIT" << std::endl; exit(EXIT_FAILURE);
			case WSAESOCKTNOSUPPORT:		std::cerr << "WSASocket() returns error code : WSAESOCKTNOSUPPORT" << std::endl;	 exit(EXIT_FAILURE);
			}
		}
	}
	void WinSocket::Err_handle_send(const int &ret_val) {
		if (ret_val == SOCKET_ERROR)
		{
			const int &err_code = WSAGetLastError();
			switch (err_code)
			{
			case WSANOTINITIALISED:	std::cerr << "send() : WSAStartup call must occur before using this function" << std::endl; exit(EXIT_FAILURE);
			case WSAENETDOWN:		std::cerr << "send() : The network subsystem has failed" << std::endl; exit(EXIT_FAILURE);
			case WSAEACCES:			std::cerr << "send() : The requested address is a broadcast address, but the appropriate flag was not set. Call setsockopt with the SO_BROADCAST socket option to enable use of the broadcast address" << std::endl; exit(EXIT_FAILURE);
			case WSAEINTR:			std::cerr << "send() : A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall" << std::endl; exit(EXIT_FAILURE);
			case WSAEINPROGRESS:	std::cerr << "send() : A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function" << std::endl; exit(EXIT_FAILURE);
			case WSAEFAULT:			std::cerr << "send() : The buf parameter is not completely contained in a valid part of the user address space" << std::endl; exit(EXIT_FAILURE);
			case WSAENETRESET:		std::cerr << "send() : The connection has been broken due to the keep-alive activity detecting a failure while the operation was in progress" << std::endl; exit(EXIT_FAILURE);
			case WSAENOBUFS:		std::cerr << "send() : No buffer space is available" << std::endl; exit(EXIT_FAILURE);
			case WSAENOTCONN:		std::cerr << "send() : The socket is not connected" << std::endl; exit(EXIT_FAILURE);
			case WSAENOTSOCK:		std::cerr << "send() : The descriptor is not a socket" << std::endl; exit(EXIT_FAILURE);
			case WSAEOPNOTSUPP:		std::cerr << "send() : MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only receive operations" << std::endl; exit(EXIT_FAILURE);
			case WSAESHUTDOWN:		std::cerr << "send() : The socket has been shut down; it is not possible to send on a socket after shutdown has been invoked with how set to SD_SEND or SD_BOTH" << std::endl; exit(EXIT_FAILURE);
			case WSAEWOULDBLOCK:	std::cerr << "send() : The socket is marked as nonblocking and the requested operation would block" << std::endl; exit(EXIT_FAILURE);
			case WSAEMSGSIZE:		std::cerr << "send() : The socket is message oriented, and the message is larger than the maximum supported by the underlying transport" << std::endl; exit(EXIT_FAILURE);
			case WSAEHOSTUNREACH:	std::cerr << "send() : The remote host cannot be reached from this host at this time" << std::endl; exit(EXIT_FAILURE);
			case WSAEINVAL:			std::cerr << "send() : The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled" << std::endl; exit(EXIT_FAILURE);
			case WSAECONNABORTED:	std::cerr << "send() : The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable" << std::endl; exit(EXIT_FAILURE);
			case WSAECONNRESET:		std::cerr << "send() : The virtual circuit was reset by the remote side executing a hard or abortive close. For UDP sockets, the remote host was unable to deliver a previously sent UDP datagram and responded with a \"Port Unreachable\" ICMP packet. The application should close the socket as it is no longer usable" << std::endl; exit(EXIT_FAILURE);
			case WSAETIMEDOUT:		std::cerr << "send() : The connection has been dropped, because of a network failure or because the system on the other end went down without notice" << std::endl; exit(EXIT_FAILURE);
			default:				std::cerr << "send() : This error code doesn't exist" << std::endl; exit(EXIT_FAILURE);
			}

		}
	}
	void WinSocket::Err_handle_shutdown(const int &ret_val) {
		if (ret_val == SOCKET_ERROR)
		{
			const int &err_code = WSAGetLastError();
			switch (err_code) {
			case WSAECONNABORTED:	std::cerr << "shutdown() : The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable" << std::endl; exit(EXIT_FAILURE);
			case WSAECONNRESET:		std::cerr << "shutdown() : The virtual circuit was reset by the remote side executing a hard or abortive close. The application should close the socket as it is no longer usable" << std::endl; exit(EXIT_FAILURE);
			case WSAEINPROGRESS:	std::cerr << "shutdown() : A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function" << std::endl; exit(EXIT_FAILURE);
			case WSAEINVAL:			std::cerr << "shutdown() : The how parameter is not valid, or is not consistent with the socket type. For example, SD_SEND is used with a UNI_RECV socket type" << std::endl; exit(EXIT_FAILURE);
			case WSAENETDOWN:		std::cerr << "shutdown() : The network subsystem has failed" << std::endl; exit(EXIT_FAILURE);
			case WSAENOTCONN:		std::cerr << "shutdown() : The socket is not connected. This error applies only to a connection-oriented socket" << std::endl; exit(EXIT_FAILURE);
			case WSAENOTSOCK:		std::cerr << "shutdown() : The descriptor is not a socket" << std::endl; exit(EXIT_FAILURE);
			case WSANOTINITIALISED:	std::cerr << "shutdown() : A 0ful WSAStartup call must occur before using this function" << std::endl; exit(EXIT_FAILURE);
			default:				std::cerr << "shutdown() : This error code doesn't exist" << std::endl; exit(EXIT_FAILURE);
			}
		}
	}
	void WinSocket::Err_handle_recv(const int &ret_val) {
		if (ret_val == SOCKET_ERROR) {
			const int &err_code = WSAGetLastError();
			switch (err_code) {
			case WSANOTINITIALISED:	std::cerr << "recv() : A 0ful WSAStartup call must occur before using this function" << std::endl; exit(EXIT_FAILURE);
			case WSAENETDOWN:		std::cerr << "recv() : The network subsystem has failed" << std::endl; exit(EXIT_FAILURE);
			case WSAEFAULT:			std::cerr << "recv() : The buf parameter is not completely contained in a valid part of the user address space" << std::endl; exit(EXIT_FAILURE);
			case WSAENOTCONN:		std::cerr << "recv() : The socket is not connected" << std::endl; exit(EXIT_FAILURE);
			case WSAEINTR:			std::cerr << "recv() : The (blocking) call was canceled through WSACancelBlockingCall" << std::endl; exit(EXIT_FAILURE);
			case WSAEINPROGRESS:	std::cerr << "recv() : A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function" << std::endl; exit(EXIT_FAILURE);
			case WSAENETRESET:		std::cerr << "recv() : For a connection-oriented socket, this error indicates that the connection has been broken due to keep-alive activity that detected a failure while the operation was in progress. For a datagram socket, this error indicates that the time to live has expired" << std::endl; exit(EXIT_FAILURE);
			case WSAENOTSOCK:		std::cerr << "recv() : The descriptor is not a socket" << std::endl; exit(EXIT_FAILURE);
			case WSAEOPNOTSUPP:		std::cerr << "recv() : MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only send operations" << std::endl; exit(EXIT_FAILURE);
			case WSAESHUTDOWN:		std::cerr << "recv() : The socket has been shut down; it is not possible to receive on a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH" << std::endl; exit(EXIT_FAILURE);
			case WSAEWOULDBLOCK:	std::cerr << "recv() : The socket is marked as nonblocking and the receive operation would block" << std::endl; exit(EXIT_FAILURE);
			case WSAEMSGSIZE:		std::cerr << "recv() : The message was too large to fit into the specified buffer and was truncated" << std::endl; exit(EXIT_FAILURE);
			case WSAEINVAL:			std::cerr << "recv() : The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled or (for byte stream sockets only) len was zero or negative" << std::endl; exit(EXIT_FAILURE);
			case WSAECONNABORTED:	std::cerr << "recv() : The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable" << std::endl; exit(EXIT_FAILURE);
			case WSAETIMEDOUT:		std::cerr << "recv() : The connection has been dropped because of a network failure or because the peer system failed to respond" << std::endl; exit(EXIT_FAILURE);
			case WSAECONNRESET:		std::cerr << "recv() : The virtual circuit was reset by the remote side executing a hard or abortive close. The application should close the socket as it is no longer usable. On a UDP-datagram socket, this error would indicate that a previous send operation resulted in an ICMP \"Port Unreachable\" message" << std::endl;
			default:				std::cerr << "recv() : This error code doesn't exist" << std::endl; exit(EXIT_FAILURE);
			}
		}
	}
	void WinSocket::Err_handle_bind(const int &ret_val) {
		if (ret_val == SOCKET_ERROR) {
			const int &err_code = WSAGetLastError();
			switch (err_code) {
			case WSANOTINITIALISED:	std::cerr << "bind() returns error code : WSANOTINITIALISED" << std::endl; exit(EXIT_FAILURE);
			case WSAENETDOWN:		std::cerr << "bind() returns error code : WSAENETDOWN" << std::endl;		exit(EXIT_FAILURE);
			case WSAEACCES:			std::cerr << "bind() returns error code : WSAEACCES" << std::endl;			exit(EXIT_FAILURE);
			case WSAEADDRINUSE:		std::cerr << "bind() returns error code : WSAEADDRINUSE" << std::endl;		exit(EXIT_FAILURE);
			case WSAEADDRNOTAVAIL:	std::cerr << "bind() returns error code : WSAEADDRNOTAVAIL" << std::endl;	exit(EXIT_FAILURE);
			case WSAEFAULT:			std::cerr << "bind() returns error code : WSAEFAULT" << std::endl;			exit(EXIT_FAILURE);
			case WSAEINPROGRESS:	std::cerr << "bind() returns error code : WSAEINPROGRESS" << std::endl;	exit(EXIT_FAILURE);
			case WSAEINVAL:			std::cerr << "bind() returns error code : WSAEINVAL" << std::endl;			exit(EXIT_FAILURE);
			case WSAENOBUFS:		std::cerr << "bind() returns error code : WSAENOBUFS" << std::endl;		exit(EXIT_FAILURE);
			case WSAENOTSOCK:		std::cerr << "bind() returns error code : WSAENOTSOCK" << std::endl;		exit(EXIT_FAILURE);
			}
		}
	}
	void WinSocket::Err_handle_listen(const int &ret_val) {
		if (ret_val == SOCKET_ERROR) {
			const int &err_code = WSAGetLastError();
			switch (err_code) {
			case WSANOTINITIALISED:	std::cerr << "listen() returns error code : WSANOTINITIALISED" << std::endl;	exit(EXIT_FAILURE);
			case WSAENETDOWN:		std::cerr << "listen() returns error code : WSAENETDOWN" << std::endl;			exit(EXIT_FAILURE);
			case WSAEADDRINUSE:		std::cerr << "listen() returns error code : WSAEADDRINUSE" << std::endl;		exit(EXIT_FAILURE);
			case WSAEINPROGRESS:	std::cerr << "listen() returns error code : WSAEINPROGRESS" << std::endl;		exit(EXIT_FAILURE);
			case WSAEINVAL:			std::cerr << "listen() returns error code : WSAEINVAL" << std::endl;			exit(EXIT_FAILURE);
			case WSAEISCONN:		std::cerr << "listen() returns error code : WSAEISCONN" << std::endl;			exit(EXIT_FAILURE);
			case WSAEMFILE:			std::cerr << "listen() returns error code : WSAEMFILE" << std::endl;			exit(EXIT_FAILURE);
			case WSAENOBUFS:		std::cerr << "listen() returns error code : WSAENOBUFS" << std::endl;			exit(EXIT_FAILURE);
			case WSAENOTSOCK:		std::cerr << "listen() returns error code : WSAENOTSOCK" << std::endl;			exit(EXIT_FAILURE);
			case WSAEOPNOTSUPP:		std::cerr << "listen() returns error code : WSAEOPNOTSUPP" << std::endl;		exit(EXIT_FAILURE);
			}
		}
	}
	void WinSocket::Err_handle_inet_pton(const int &ret_val){
		if (ret_val != 1){
			int val = WSAGetLastError();
			if (val == WSAEAFNOSUPPORT){
				std::cout << "inet_pton() return error code : WSAEAFNOSUPPORT\n" << std::endl;
			}
			else{
				std::cout << "inet_pton() return error code : WSAEFAULT\n" << std::endl;
			}
			exit(EXIT_FAILURE);
		}
	}
	void WinSocket::Err_handle_accept(const int &ret_val) {
		if (ret_val == SOCKET_ERROR) {
			const int &err_code = WSAGetLastError();
			switch (err_code){
			case WSANOTINITIALISED:	std::cerr << "accept() returns error code : WSANOTINITIALISED" << std::endl;	exit(EXIT_FAILURE);
			case WSAECONNRESET:		std::cerr << "accept() returns error code : WSAECONNRESET" << std::endl;		exit(EXIT_FAILURE);
			case WSAEFAULT:			std::cerr << "accept() returns error code : WSAEFAULT" << std::endl;			exit(EXIT_FAILURE);
			case WSAEINTR:			std::cerr << "accept() returns error code : WSAEINTR" << std::endl;			exit(EXIT_FAILURE);
			case WSAEINVAL:			std::cerr << "accept() returns error code : WSAEINVAL" << std::endl;			exit(EXIT_FAILURE);
			case WSAEINPROGRESS:	std::cerr << "accept() returns error code : WSAEINPROGRESS" << std::endl;		exit(EXIT_FAILURE);
			case WSAEMFILE:			std::cerr << "accept() returns error code : WSAEMFILE" << std::endl;			exit(EXIT_FAILURE);
			case WSAENETDOWN:		std::cerr << "accept() returns error code : WSAENETDOWN" << std::endl;			exit(EXIT_FAILURE);
			case WSAENOBUFS:		std::cerr << "accept() returns error code : WSAENOBUFS" << std::endl;			exit(EXIT_FAILURE);
			case WSAENOTSOCK:		std::cerr << "accept() returns error code : WSAENOTSOCK" << std::endl;			exit(EXIT_FAILURE);
			case WSAEOPNOTSUPP:		std::cerr << "accept() returns error code : WSAEOPNOTSUPP" << std::endl;		exit(EXIT_FAILURE);
			case WSAEWOULDBLOCK:	std::cerr << "accept() returns error code : WSAEWOULDBLOCK" << std::endl;		exit(EXIT_FAILURE);
			}
		}
	}


	/* API's */
	void WinSocket::socket_init(void) {
		Err_handle_WSAStartup((WSAStartup(MAKEWORD(2, 2), &wsaData) != 0));
		sockaddr_in.sin_family = AF_INET;
	}
	void WinSocket::connect(const std::wstring &url, const std::wstring &port) {
		Err_handle_socket(http_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
		sockaddr_in.sin_port = htons(std::stoi(ws2s(port)));
		InetPtonW(AF_INET, url.c_str(), &sockaddr_in.sin_addr);
		if (::connect(http_socket, (struct sockaddr*)&sockaddr_in, sizeof(sockaddr_in)) == SOCKET_ERROR) {
			socket_close();
		}
	}
	void WinSocket::transmit(const std::wstring &data, const int& data_size_in_bytes) {
		if (!isConnected()) {
			return;
		}
		size_t nbytes_total = 0;
		while (nbytes_total < data.length()) {            // send data to server
			std::wstring wideChunk = data.substr(nbytes_total);
			std::string narrowChunk = ws2s(wideChunk);
			size_t narrowBytesToSend = narrowChunk.length();
			int nbytes_last = send(http_socket, narrowChunk.c_str(), static_cast<int>(narrowBytesToSend), 0);
			nbytes_total += nbytes_last; // Increment by the number of bytes sent
		}		
	}
	void WinSocket::receiveResponse(std::wstring &recievedData, const struct timeval &timeout) {
		if (!isConnected()) {
			return;
		}
		const int readBuff_size{ 1024 };
		char readBuff[readBuff_size] = {};
		int bytesRead = 0;
		fd_set read_fds;
		FD_ZERO(&read_fds);
		FD_SET(http_socket, &read_fds);

		std::string tmpDataRead;
		int select_result = select(0, &read_fds, NULL, NULL, &timeout);
		
		if (select_result == SOCKET_ERROR) {
		//	std::cerr << "select() failed with error: " << WSAGetLastError() << std::endl;
			socket_close();
		}
		else if (select_result == 0) {
			// Timeout occurred, no data received within timeout_sec seconds
			socket_close();
			return;
		}
		if (__WSAFDIsSet(http_socket, &read_fds)) {    // Recieve data from server
			while ((bytesRead = recv(http_socket, readBuff, (readBuff_size - 1), 0)) > 0) {
				readBuff[bytesRead] = 0x00;
				tmpDataRead += readBuff;        // Append incomming data to string
			}
		}
		if (bytesRead == SOCKET_ERROR) {
		//	std::cerr << "recv() failed with error: " << WSAGetLastError() << std::endl;
			socket_close();
		}
		recievedData = s2ws(tmpDataRead);
	}
	void WinSocket::socket_close() {
		closesocket(http_socket);
		http_socket = INVALID_SOCKET;
	}
	void WinSocket::socket_cleanup() {
		closesocket(http_socket);
		http_socket = INVALID_SOCKET;
		WSACleanup();
	}

}