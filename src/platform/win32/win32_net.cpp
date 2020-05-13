#include <WinSock2.h>
#include <stdlib.h>

#define MAX_PACKET_SIZE 1488

struct win32_socket
{
	SOCKET Socket;
};



internal win32_socket win32_net_socket_open()
{
	win32_socket result = {};
	SOCKET socket = INVALID_SOCKET;
	socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socket == INVALID_SOCKET)
	{
		// @TODO(Anders): handle socket failure
	}


	result.Socket = socket;
	return result;
}

internal void win32_net_socket_close(win32_socket* socket)
{
	closesocket(socket->Socket);
}

internal void win32_net_connect(win32_socket* socket, const char* serverIP, int serverPort)
{
	sockaddr_in serverAddress = {};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(serverIP);
	serverAddress.sin_port = htons(serverPort);

	int result = connect(socket->Socket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));


}

internal void win32_net_socket_listen(win32_socket* socket)
{
	char buffer[MAX_PACKET_SIZE];

	int receivedBytes = recv(socket->Socket, buffer, MAX_PACKET_SIZE, 0);
}
