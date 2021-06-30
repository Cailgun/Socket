#include "MServer.h"
#include <iostream>
#include <thread>
#include <ws2tcpip.h>

void getMessage(MServer& s)
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(100));
		fd_set fdset;
		if (s.getCanReadClientSocketFdSet(fdset))
		{
			auto listenfd = s.getListenSocket();
			for (int i = 0; i < fdset.fd_count; ++i)
			{
				if (fdset.fd_array[i] == listenfd)
				{
					s.createClientSocket();
				}
				else
				{
					sockaddr_in addr;
					int len = sizeof(addr);
					getpeername(fdset.fd_array[i], (sockaddr*)&addr, &len);
					char ip[16] = { 0 };
					inet_ntop(AF_INET, &addr, ip, 16);
					auto str = s.readFromClient(fdset.fd_array[i]);
					if (str != std::string())
						std::cout << "【来自客户端" << ip << "：】" << str << std::endl;
				}
			}
		}
	}
}

int main()
{
    MServer s;
    s.init();
    s.createClientSocket();
	std::thread th(getMessage, std::ref(s));
	th.detach();
    while (true)
    {
		s.showAllClientSocketLists();
		std::cout << "请选择要发送的客户端序号:";
		int index = 0;
		std::cin >> index;
		std::cout << "请输入要发送的信息:";
		std::string str;
		std::cin >> str;
		s.writeToClient(str, index);
    }
	WSACleanup();
    return 0;
}

