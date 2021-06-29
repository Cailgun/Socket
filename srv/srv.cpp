#include "MServer.h"
#include <iostream>
#include <thread>

void getMessage(MServer& s)
{
	while (true)
	{
		auto str = s.read();
		if (str != std::string())
			std::cout << "【来自对方】:" << str << std::endl;
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
		std::string str;
		std::cin >> str;
		s.write(str);
    }

    return 0;
}

