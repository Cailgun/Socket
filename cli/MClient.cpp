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
	ret = connect(_clientSocket, (sockaddr*)&_serverAddr, length);
	if (ret != -1)
		std::cout << "已连接:" << _serverAddr.sin_addr.s_addr <<std::endl;
	return ret;
}

SOCKET MClient::getClientSocket()
{
	return _clientSocket;
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
	int length = data.length() + 1;
	std::unique_ptr<char> cache(new char[sizeof(int) + length]);
	memcpy(cache.get(), &length, sizeof(int));
	memcpy(cache.get() + sizeof(int), data.c_str(), length - 1);
	memcpy(cache.get() + sizeof(int) + length - 1, "\0", 1);
	if (data.length() > 65536)
	{
		std::cout << std::endl << "数据量太大了，暂不做处理:" << std::endl;
	}
	else
	{
		send(_clientSocket, cache.get(), sizeof(int) + length, 0);
	}
}

void MClient::end()
{
	closesocket(_clientSocket);
	//_clientSocket = 0;
	WSACleanup();
}

