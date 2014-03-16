#include "sock_common.h"

#define BUFFER_SIZE    500

/*
* 编译的时候需要和sock_common.c一起编译,否则函数将无法调用
*/

void main(int argc, char *argv[])
{
	SOCKET               client_sock;
	int                  Ret;
	int					 nTotalBytes;
	const int            port=7000;   //指定端口号为7000  
	char				 pBuffer[BUFFER_SIZE];  //保存从服务器接收到的数据

	if (argc <= 2)
	{
		printf("USAGE: %s <Server IP> <Word>.\n",argv[0]);
		return;
	}
	//初始化WSA	
	if ((Ret = WinSockInit()) != 0)
	{
		printf("WSAStartup failed with error %d \n",Ret);
		return;
	}
	//创建socket，并连接到服务器
	client_sock=connectTCP(argv[1], port );
	//向服务器send数据
	send(client_sock,argv[2],strlen(argv[2]),0);
	nTotalBytes = 0;
	//从服务器recv数据
	while(1)
	{
		Ret = recv(client_sock,pBuffer,BUFFER_SIZE,0);
		if (Ret == SOCKET_ERROR)
		{
			sockerror("Receive data failed!");
			closesocket(client_sock);
			WinSockDestroy();
			return;
		}
		if (Ret == 0) break;
		nTotalBytes += Ret;
		pBuffer[Ret] = '\0';
		printf(pBuffer);
	}

	//关闭socket
	closesocket(client_sock);
	// 注销WSA
	WinSockDestroy();
	return ;
}
