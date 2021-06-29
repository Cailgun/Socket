#ifndef _H_MSERVER_H_
#define _H_MSERVER_H_

#include <windows.h>
#include <iostream>
#pragma comment(lib,"Ws2_32.lib")
#include <vector>

class MServer
{
public:
	MServer(){}
	~MServer(){}

	void init(int port = 4420);
	SOCKET createClientSocket(int type = AF_INET);
	std::string read();
	void write(std::string data);
private:
	SOCKET _listenSocket;
	SOCKET _clientSocket;
};

#endif

