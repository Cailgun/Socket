#ifndef _H_MSERVER_H_
#define _H_MSERVER_H_

#include <WinSock2.h>
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

	SOCKET getListenSocket();
	SOCKET getClientSocket(int index);
	void showAllClientSocketLists();

	int getCanReadClientSocketFdSet(fd_set& set);

	std::string readFromClient(SOCKET clientSocket);
	void writeToClient(std::string data, int index);
private:
	fd_set _sockets;
	SOCKET _listenSocket = 0;
};

#endif

