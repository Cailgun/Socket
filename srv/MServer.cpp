#include "MServer.h"

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
}

SOCKET MServer::createClientSocket(int type /*= AF_INET*/)
{
	sockaddr_in clientAddr;
	int addrLength = sizeof(clientAddr);

	std::cout << "开始等待客户端" << std::endl;
	auto r = accept(_listenSocket, (sockaddr*)&clientAddr, &addrLength);
	if (r == -1)
		return -1;
	std::cout << "客户端已连接:" << clientAddr.sin_addr.s_addr << std::endl;;
	_clientSocket = r;
	return r;
}

std::string MServer::read()
{
	std::string ret;
	if (_clientSocket == 0)
		return ret;

	char cache[1500];
	recv(_clientSocket, cache, 1500, 0);
	ret = cache;
	return ret;
}

void MServer::write(std::string data)
{
	char* cache = (char*)data.c_str();
	send(_clientSocket, cache, strlen(cache), 0);
}
