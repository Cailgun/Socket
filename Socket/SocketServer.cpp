#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h> 
#pragma comment(lib,"ws2_32.lib")   
#include <stdio.h> 

int main()
{
	/// 初始化socket  
	WSADATA wsaData;
	WORD version = MAKEWORD(2, 2);
	int result = 0;
	result = WSAStartup(version, &wsaData);
	if (0 != result)
	{
		std::cout << "WSAStartup() error." << std::endl;
		return -1;
	}

	/// 创建侦听socket   
	SOCKET socketListen;
	socketListen = socket(AF_INET, SOCK_STREAM, 0);
	if (socketListen == INVALID_SOCKET)
	{
		WSACleanup();
		std::cout << "socket() error." << std::endl;
		return -1;
	}

	/// 服务器地址结构   
	sockaddr_in svrAddress;
	svrAddress.sin_family = AF_INET;
	svrAddress.sin_addr.s_addr = INADDR_ANY;
	svrAddress.sin_port = htons(8000);

	/// 绑定服务器套接字   
	result = bind(socketListen, (sockaddr*)&svrAddress, sizeof(svrAddress));
	if (result == SOCKET_ERROR)
	{
		closesocket(socketListen);
		WSACleanup();
		std::cout << "bind() error." << std::endl;
		return -1;
	}

	/// 开启监听  
	result = listen(socketListen, 5);
	if (result == SOCKET_ERROR)
	{
		closesocket(socketListen);
		WSACleanup();
		std::cout << "listen() error." << std::endl;
		return -1;
	}
	std::cout << "服务器启动成功，监听端口：" << ntohs(svrAddress.sin_port) << std::endl;

	/// select模型   
	fd_set allSockSet;
	FD_ZERO(&allSockSet);

	FD_SET(socketListen, &allSockSet); // 将socketListen加入套接字集合中   

	while (true)
	{
		fd_set readSet;
		FD_ZERO(&readSet);                    //初始化可读队列
		readSet = allSockSet;

		result = select(0, &readSet, NULL, NULL, NULL);
		if (result == SOCKET_ERROR)
		{
			std::cout << "select() error." << std::endl;
			break;
		}

		if (FD_ISSET(socketListen, &readSet))
		{
			sockaddr_in clientAddr;
			int len = sizeof(clientAddr);

			SOCKET clientSocket = accept(socketListen, (sockaddr*)&clientAddr, &len);
			if (clientSocket == INVALID_SOCKET)
			{
				std::cout << "accept() error." << std::endl;
				break;
			}
			FD_SET(clientSocket, &allSockSet);   /// 将新创建的套接字加入到集合中   

			char ipAddress[16] = { 0 };
			inet_ntop(AF_INET, &clientAddr, ipAddress, 16);
			std::cout << "有新的连接[" << ipAddress << ":" << ntohs(clientAddr.sin_port)
				<< "], 目前客户端的数量为：" << allSockSet.fd_count - 1 << std::endl;       //去掉一个侦听套接字

			continue;
		}

		for (u_int i = 0; i < allSockSet.fd_count; ++i)
		{
			SOCKET socket = allSockSet.fd_array[i];

			sockaddr_in clientAddr;
			int len = sizeof(clientAddr);
			getpeername(socket, (struct sockaddr*)&clientAddr, &len);      //获得与套接口相连的远程协议地址
			char ipAddress[16] = { 0 };
			inet_ntop(AF_INET, &clientAddr, ipAddress, 16);

			/// 可读性监视，可读性指有连接到来、有数据到来、连接已关闭、重置或终止  
			if (FD_ISSET(socket, &readSet))
			{
				char bufRecv[100];
				result = recv(socket, bufRecv, 100, 0);
				if (result == SOCKET_ERROR)
				{
					DWORD err = WSAGetLastError();
					if (err == WSAECONNRESET)       /// 客户端的socket没有被正常关闭,即没有调用closesocket  
					{
						std::cout << "客户端[" << ipAddress << ":" << ntohs(clientAddr.sin_port) << "]被强行关闭, ";
					}
					else
					{
						std::cout << "recv() error，" << std::endl;
					}

					closesocket(socket);
					FD_CLR(socket, &allSockSet);

					std::cout << "目前客户端的数量为：" << allSockSet.fd_count - 1 << std::endl;
					break;
				}
				else if (result == 0)               /// 客户端的socket调用closesocket正常关闭  
				{
					closesocket(socket);
					FD_CLR(socket, &allSockSet);

					std::cout << "客户端[" << ipAddress << ":" << ntohs(clientAddr.sin_port)
						<< "]已经退出，目前客户端的数量为：" << allSockSet.fd_count - 1 << std::endl;
					break;
				}

				bufRecv[result] = '\0';
				std::cout << "来自客户端[" << ipAddress << ":" << ntohs(clientAddr.sin_port)
					<< "]的消息：" << bufRecv << std::endl;

				char szSend[256] = "朕已经收到，你可以滚了！";
				send(socket, szSend, strlen(szSend), 0);
			}
		}
	}

	for (u_int i = 0; i < allSockSet.fd_count; ++i)
	{
		SOCKET socket = allSockSet.fd_array[i];
		closesocket(socket);
	}

	WSACleanup();
	return 0;
}