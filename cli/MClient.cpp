#include "MClient.h"


void MClient::init(int port /*= 4420*/, std::string ip)
{
	WSAData data;
	auto version = MAKEWORD(2, 2);
	WSAStartup(version, &data);

	SOCKET clientfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_pton(serverAddr.sin_family, ip.c_str(), &serverAddr.sin_addr);

	_clientSocket = clientfd;
	_serverAddr = serverAddr;
}

SOCKET MClient::createClientSocket()
{
	SOCKET ret;
	int length = sizeof(_serverAddr);
	std::cout << "开始连接服务端" << std::endl;
	ret = connect(_clientSocket, (sockaddr*)&_serverAddr, length);
	if (ret != -1)
		std::cout << "服务器已连接:" << _serverAddr.sin_addr.s_addr <<std::endl;
	return ret;
}

std::string MClient::read()
{
	std::string ret;
	if (_clientSocket == 0)
		return ret;

	char cache[1500] = { 0 };
	recv(_clientSocket, cache, 1500, 0);
	ret = cache;
	return ret;
}

void MClient::write(std::string data)
{
	char* cache = (char*)data.c_str();
	send(_clientSocket, cache, strlen(cache), 0);
}


