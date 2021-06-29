#include <iostream>  
#include <WS2tcpip.h>  
#include <WinSock2.H>  
using namespace std;
#pragma comment(lib, "ws2_32.lib")  

#define SERVER_ADDRESS      "127.0.0.1"  
#define SERVER_PORT         8000  

#define SOCKET_NUM          120       /// 客户端socket的个数,修改该值可以改变连接到服务器的客户端个数  

SOCKET socketClient;

char g_szBuf[512] = "";

HANDLE hMutex;

void Input();

void Recv();

DWORD WINAPI ThreadProc(
    _In_ LPVOID lpParameter
);

DWORD WINAPI SendDataThreadProc(
    _In_ LPVOID lpParameter
);

int main()
{
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;
    int err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) return 1;

    if (LOBYTE(wsaData.wVersion) != 2 ||
        HIBYTE(wsaData.wVersion) != 2)
    {
        WSACleanup();
        std::cout << "WSAStartup() error." << std::endl;
        return -1;
    }

    socketClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketClient == INVALID_SOCKET)
    {
        WSACleanup();
        std::cout << "socket() error." << std::endl;
        return -1;
    }


    SOCKADDR_IN server;
    memset(&server, 0, sizeof(SOCKADDR_IN));
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    inet_pton(server.sin_family, SERVER_ADDRESS, &server.sin_addr);

    err = connect(socketClient, (struct sockaddr*)&server, sizeof(SOCKADDR_IN));
    if (err == SOCKET_ERROR)
    {
        std::cout << "connect() error." << std::endl;
        closesocket(socketClient);
        WSACleanup();
        return -1;
    }

    hMutex = CreateMutex(NULL, false, (LPCWSTR)"MutexOne");
    if (NULL == hMutex)
    {
        printf("创建互斥对象失败！\n");
        return -1;
    }

    DWORD dwThreadId = 0;

    /*HANDLE hThread = CreateThread(NULL, NULL, &SendDataThreadProc, NULL, 0, &dwThreadId);
    if (NULL == hThread)
    {
    printf("创建发送线程失败！\n");
    return -1;
    }*/


    HANDLE hThread2 = CreateThread(NULL, NULL, &ThreadProc, NULL, 0, &dwThreadId);
    if (NULL == hThread2)
    {
        printf("创建接收线程失败！\n");
        return -1;
    }




    Input();
    //    Recv();

    closesocket(socketClient);
    WSACleanup();

    return 0;
}

void Input()
{
    while (true)
    {
        WaitForSingleObject(hMutex, INFINITE);
        char szInput[256] = "";
        printf("Me:");
        scanf_s("%s", szInput);
        if (strcmp(szInput, "exit") == 0)
        {
            break;
        }
        else
        {
            send(socketClient, szInput, strlen(szInput), 0);
        }
        //        printf("\n");
        ReleaseMutex(hMutex);

    }
}

void Recv()
{
    while (true)
    {

        recv(socketClient, g_szBuf, sizeof(g_szBuf), 0);
        printf("Me Recv Form Server:%s\n", g_szBuf);


    }
}

DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter)
{

    while (true)
    {
        WaitForSingleObject(hMutex, INFINITE);
        recv(socketClient, g_szBuf, sizeof(g_szBuf), 0);
        printf("Recv Form Server:%s\n\n", g_szBuf);
        ReleaseMutex(hMutex);
    }

    return 0;
}

DWORD WINAPI SendDataThreadProc(_In_ LPVOID lpParameter)
{

    while (true)
    {
        WaitForSingleObject(hMutex, INFINITE);
        char szInput[256] = "";
        printf("Me:");
        scanf_s("%s", szInput);
        if (strcmp(szInput, "exit") == 0)
        {
            break;
        }
        else
        {

            send(socketClient, szInput, strlen(szInput), 0);

        }
        printf("\n");
        ReleaseMutex(hMutex);
    }


    return 0;
}