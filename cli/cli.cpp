#include "MClient.h"
#include <iostream>
#include <thread>

void getMessage(MClient& s)
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
    MClient c;
    c.init(10, "47.97.196.125");
	std::cout << "开始连接" << std::endl;
	while (true)
	{
		if (-1 != c.createClientSocket())
		{
			break;
		}
	}


	std::thread th(getMessage, std::ref(c));
	th.detach();
	while (true)
	{
		std::string str;
		std::cin >> str;
		c.write(str);
	}
    return 0;
}
