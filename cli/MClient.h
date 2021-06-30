#ifndef _H_MCLIENT_H_
#define _H_MCLIENT_H_

#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#pragma comment(lib,"Ws2_32.lib")

class MClient
{
public:
	MClient() {}
	~MClient() {}

	void init(int port = 4420, std::string ip = "127.0.0.1");
	SOCKET createClientSocket();
	SOCKET getClientSocket();
	std::string read();
	void write(std::string data);
	void end();
private:
	SOCKET _clientSocket;
	sockaddr_in _serverAddr;
};


#endif 

