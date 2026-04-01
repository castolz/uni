#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h> // for more recent InetPton
#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_BUFLEN 512
using namespace std;
int main() {
	// 1) Startup Winsock Application WSA
	WORD wVersionRequested;
	WSADATA wsaData; // WSADATA data structure
	wVersionRequested = MAKEWORD(2, 2);
	int err = WSAStartup(wVersionRequested, &wsaData);
	// 2) Initialize Socket
	SOCKET ConnectSocket = INVALID_SOCKET;
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// 3) Specify Connection
	struct sockaddr_in clientService;
	inet_pton(AF_INET, "172.217.18.14", &clientService.sin_addr);
	clientService.sin_family = AF_INET;
	clientService.sin_port = htons(80);
	// 4) Connect to server.
	int iResult;
	iResult = connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService));
	// 5) Send
	const char* rq_all = "GET / HTTP/1.1\r\nHost: www.google.de\r\nConnection: close\r\n\r\n";
	iResult = send(ConnectSocket, rq_all, (int)strlen(rq_all), 0);
	// 6) Receive
	int recvbuflen = DEFAULT_BUFLEN;
	char recvbuf[DEFAULT_BUFLEN] = "";
	do { // Receive until the peer closes the connection
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			wprintf(L"Bytes received: %d\n", iResult);
			for (int jj = 0; jj < recvbuflen; jj++) {
				cout << recvbuf[jj];
			}
		}
	} while (iResult > 0);
	// 7) close the socket
	iResult = closesocket(ConnectSocket);
	WSACleanup();
	return 0;
}
