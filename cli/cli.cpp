#include "MClient.h"
#include <iostream>
#include <thread>
#include <future>

void getMessage(MClient& s, std::promise<int>& p)
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(100));
		if (s.getClientSocket() == 0)
			break;
		auto str = s.read();
		if (str != std::string())
		{
			if (str == "shutdown")
			{
				s.end();
				p.set_value(-1);
				break;
			}
			std::cout << "【来自对方】:" << str << std::endl;
		}

	}
}

void getInput(std::string& str)
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(100));
		std::cin >> str;
	}
}

int main()
{
    MClient c;
	c.init();
    //c.init(10, "47.97.196.125");
	std::cout << "开始连接" << std::endl;
	while (true)
	{
		if (-1 != c.createClientSocket())
		{
			break;
		}
	}

	std::promise<int> prms;
	std::future<int> fu = prms.get_future();
	std::thread th(getMessage, std::ref(c), std::ref(prms));
	
	std::string str;
	std::thread th2(getInput, std::ref(str));
	th.detach();
	th2.detach();
	while (true)
	{
		auto status = fu.wait_for(std::chrono::microseconds(100));
		if (status == std::future_status::ready)
			break;
		if (str == ":q")
		{
			c.end();
			break;
		}
		else if (str != "")
		{
			c.write(str);
			str = "";
		}
	}
    return 0;
}
