#include "MClient.h"
#include <iostream>

int main()
{
    MClient c;
    c.init(10, "47.97.196.125");
    c.createClientSocket();
	while (true)
	{
		std::string str;
		std::cin >> str;
		c.write(str);
	}
    return 0;
}
