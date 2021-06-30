#include "MServer.h"
#include <ws2tcpip.h>

void MServer::init(int port)
{
	WSAData data;
	auto version = MAKEWORD(2, 2);
	WSAStartup(version, &data);

	SOCKET listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in listenAddr;
	listenAddr.sin_addr.s_addr = INADDR_ANY;
	listenAddr.sin_port = htons(port);
	listenAddr.sin_family = AF_INET;

	bind(listenfd, (sockaddr*)&listenAddr, sizeof(listenAddr));
	listen(listenfd, 5);
	_listenSocket = listenfd;
	FD_ZERO(&_sockets);
	FD_SET(_listenSocket, &_sockets);
}

SOCKET MServer::createClientSocket(int type /*= AF_INET*/)
{
	sockaddr_in clientAddr;
	int addrLength = sizeof(clientAddr);

	auto r = accept(_listenSocket, (sockaddr*)&clientAddr, &addrLength);
	if (r == -1)
		return -1;

	std::cout << std::endl << "【有新连接】"<< std::endl;
	FD_SET(r, &_sockets);
	return r;
}

int MServer::getCanReadClientSocketFdSet(fd_set& set)
{
	set = _sockets;
	timeval t;
	t.tv_sec = 0;
	t.tv_usec = 128;
	return select(set.fd_count + 1, &set, nullptr, nullptr, &t);
}

SOCKET MServer::getClientSocket(int index)
{
	if (index > 0 && _sockets.fd_count > index)
		return _sockets.fd_array[index];
	return 0;
}

SOCKET MServer::getListenSocket()
{
	return _listenSocket;
}

void MServer::showAllClientSocketLists()
{
	for (int i = 1; i < _sockets.fd_count; ++i)
	{
		sockaddr_in addr;
		int len = sizeof(addr);
		getpeername(_sockets.fd_array[i], (sockaddr*)&addr, &len);
		char ip[16] = { 0 };
		inet_ntop(AF_INET, &addr, ip, 16);
		std::cout << i << " : " << ip << std::endl;
	}
}

std::string MServer::readFromClient(SOCKET clientSocket)
{
	std::string ret;
	if (clientSocket <= 0)
		return ret;

	char cache[4];
	auto errCode = recv(clientSocket, cache, 4, 0);

	sockaddr_in addr;
	int len = sizeof(addr);
	getpeername(clientSocket, (sockaddr*)&addr, &len);
	char ip[16] = { 0 };
	inet_ntop(AF_INET, &addr, ip, 16);


	if (errCode > 0)
	{
		int remain = 0;
		memcpy(&remain, cache, sizeof(int));

		if (remain > 65536)
		{
			std::cout << std::endl << "数据量太大了，暂不做处理:" << ip << std::endl;
		}
		else
		{
			std::unique_ptr<char> pStr(new char[remain]);
			int recvedLength = 0;
			while (recvedLength < remain && errCode > 0)
			{
				int length = remain > 1500 ? 1500 : remain;
				errCode = recv(clientSocket, pStr.get() + recvedLength, length, 0);
				recvedLength += length;
			}
			ret = pStr.get();
		}
	}
	if(errCode <= 0)
	{
		closesocket(clientSocket);
		FD_CLR(clientSocket, &_sockets);
		std::cout << std::endl << "客户端已退出:" << ip << std::endl;

	}

	return ret;
}

void MServer::writeToClient(std::string data, int index)
{
	if (index <= 0)
		return ;

	auto clientSocket = _sockets.fd_array[index];

	char* cache = (char*)data.c_str();
	if (data.length() > 65536)
	{
		sockaddr_in addr;
		int len = sizeof(addr);
		getpeername(clientSocket, (sockaddr*)&addr, &len);
		char ip[16] = { 0 };
		inet_ntop(AF_INET, &addr, ip, 16);
		std::cout << std::endl << "数据量太大了，暂不做处理:" << ip << std::endl;
	}
	else
	{
		send(clientSocket, cache, strlen(cache), 0);
	}
}
