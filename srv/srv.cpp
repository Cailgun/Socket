#include "MServer.h"
#include <iostream>


int main()
{
    MServer s;
    s.init();
    s.createClientSocket();
    while (true)
    {
        auto str = s.read();
        if (str != std::string())
            std::cout <<"接收到字符串:"<< str << std::endl;
    }

    return 0;
}

