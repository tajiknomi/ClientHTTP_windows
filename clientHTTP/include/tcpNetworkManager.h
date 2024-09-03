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
// SOFTWARE. 1 

#pragma once
//#pragma comment(lib, "Ws2_32.lib") /* Linking against the Ws2_32.lib library */

#include <ws2tcpip.h>
#include <iostream>
#include <ctime>


enum class ConnectionBehaviour : unsigned int {
	DropConnection,
	KeepAlive,
	WaitForPeerToDropConnection
};

namespace mysocket {

	class WinSocket {

	private:	/*	Attributes	*/
		WSADATA wsaData;
		int socket_type;
		int protocol;
		struct sockaddr_in sockaddr_in;
		
	private:	/*	Functions	*/
		bool isConnected(void);

	private:	/* Error handlers for windows socket API's */		
		void Err_handle_WSAStartup(const int& err_code);
		void Err_handle_getaddrinfo(const int& err_code);
		void Err_handle_socket(const SOCKET& ret_val);
		void Err_handle_WSASocket(const SOCKET& ret_val);
		void Err_handle_send(const int& ret_val);
		void Err_handle_shutdown(const int& ret_val);
		void Err_handle_recv(const int& ret_val);
		void Err_handle_bind(const int& ret_val);
		void Err_handle_inet_pton(const int& ret_val);
		void Err_handle_listen(const int& ret_val);
		void Err_handle_accept(const int& ret_val);

	public:
		/* Constructor */
		WinSocket() : http_socket(INVALID_SOCKET),
			socket_type(SOCK_STREAM) {
			wsaData = { NULL };
		};

	public:	/*	Attributes	*/
		SOCKET http_socket;
			
	public:	/* Public API's */	
		void socket_init(void);
		void connect(const std::wstring &url, const std::wstring &port_num);
		void transmit(const std::wstring &data, const int& data_size_in_bytes);
		void receiveResponse(std::wstring &recievedData, const struct timeval &timeout);
		void socket_close();			/* This closes the socket but DOES NOT release the internal resources */
		void socket_cleanup();			/* Close the socket and release the resources */
		
	public:		/*	Destructor	*/ 
		virtual ~WinSocket() { socket_cleanup(); }		/* In case if user forget to cleanup or exception caught */
	};
}