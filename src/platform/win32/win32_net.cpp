#define MAX_PACKET_SIZE 1480

struct fn_net_socket
{
	int32 Handle;
	uint16 Port;
};

struct fn_net_address
{
	uint32 Address;
	uint16 Port;
};

global_variable volatile bool GlobalNetServerRunning;
global_variable volatile bool GlobalNetClientRunning;

internal fn_net_address fn_net_address_create(uint32 a, uint32 b, uint32 c, uint32 d, uint16 port)
{
	fn_net_address address = {};

	address.Address = (a << 24) | (b << 16) | (c << 8) | d;
	address.Port = port;

	return address;
}

internal DWORD win32_net_server_loop(LPVOID lpParam)
{
	fn_net_socket* sock = (fn_net_socket*)lpParam;

	while (GlobalNetServerRunning)
	{
		uint8 receiveBuffer[MAX_PACKET_SIZE];
		
		sockaddr_in from;
		socklen_t fromLen = sizeof(from);

		int receivedBytes = recvfrom(sock->Handle, (char*)receiveBuffer, MAX_PACKET_SIZE, 0, (SOCKADDR*)&from, &fromLen);

		uint32 fromAddress = ntohl(from.sin_addr.s_addr);
		uint32 fromPort = ntohs(from.sin_port);


	}
}

internal DWORD win32_net_client_loop(LPVOID lpParam)
{
	fn_net_socket* sock = (fn_net_socket*)lpParam;

	while (GlobalNetClientRunning)
	{
		char receiveBuffer[MAX_PACKET_SIZE];
		int receivedBytes = recv(sock->Handle, receiveBuffer, MAX_PACKET_SIZE, 0);

	}
}

internal bool win32_net_socket_init()
{
	WSADATA wsaData;
	return WSAStartup(MAKEWORD(2, 2), &wsaData) == NO_ERROR;
}

internal void win32_net_socket_shutdown()
{
	WSACleanup();
}



internal fn_net_socket win32_net_socket_open(uint16 port, bool blocking)
{
	fn_net_socket result = {};
	SOCKET socketHandle = INVALID_SOCKET;
	socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (socketHandle == INVALID_SOCKET)
	{
		// @TODO(Anders): handle socket failure
		OutputDebugStringA("Failed to open socket \n");
	}

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (bind(socketHandle, (const SOCKADDR*)&address, sizeof(sockaddr_in)) < 0)
	{
		OutputDebugStringA("Failed to bind socket \n");
	}

	if (!blocking)
	{
		DWORD nonBlocking = 1;
		if (ioctlsocket(socketHandle, FIONBIO, &nonBlocking) != 0)
		{
			OutputDebugStringA("Failed to set socket to non-blocking \n");
		}
	}

	result.Handle = (int)socketHandle;
	result.Port = port;

	return result;
}

internal void win32_net_socket_close(fn_net_socket* sock)
{
	closesocket(sock->Handle);
}

internal bool win32_net_socket_send(const fn_net_socket* sock, const fn_net_address* destAddress, void* data, int size)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(destAddress->Address);
	addr.sin_port = htons(destAddress->Port);

	int sentBytes = sendto(sock->Handle, (const char*) data, size, 0, (SOCKADDR*)&addr, sizeof(sockaddr_in));
	
	if (sentBytes != size)
	{
		OutputDebugStringA("Failed to send packet \n");
		return false;
	}

	return true;
}
