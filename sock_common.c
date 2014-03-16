#include "sock_common.h"



/*------------------------------------------------------------------------
* WinSockInit - 初始化WSA
*------------------------------------------------------------------------
*/
int WinSockInit()
{
	WSADATA wsadata;
	return WSAStartup(MAKEWORD(2,2),&wsadata);
}

/*------------------------------------------------------------------------
* WinSockDestroy - 注销WSA
*------------------------------------------------------------------------
*/
void WinSockDestroy()
{
	WSACleanup();
}

/***************************************************************************************************************
* sockerror 用来输出网络函数的错误信息
***************************************************************************************************************/
void sockerror(const char *format, ...)
{
	int errno;
	va_list	args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);

	errno = WSAGetLastError();

	switch (errno)
	{
		case WSAEADDRINUSE:
			fprintf(stderr,"WSAEADDRINUSE:The specified address is already in use.");
			break;
		case WSAEADDRNOTAVAIL:
			fprintf(stderr,"WSAEADDRNOTAVAIL:The specified address is not available from the local machine.");
			break;
		case WSAEAFNOSUPPORT:
			fprintf(stderr,"WSAEAFNOSUPPORT:Addresses in the specified family cannot be used with this socket.");
			break;
		case WSAECONNREFUSED:
			fprintf(stderr,"WSAECONNREFUSED:The attempt to connect was forcefully rejected.");
			break;
		case WSAEDESTADDRREQ:
			fprintf(stderr,"WSAEDESTADDRREQ:A destination address is required.");
			break;
		case WSAEFAULT:
			fprintf(stderr,"WSAEFAULT:The lpSockAddrLen argument is incorrect.");
			break;
		case WSAEINVAL:
			fprintf(stderr,"WSAEINVAL:The socket is already bound to an address.");
			break;
		case WSAEISCONN:
			fprintf(stderr,"WSAEISCONN:The socket is already connected.");
			break;
		case WSAEMFILE:
			fprintf(stderr,"WSAEMFILE:No more file descriptors are available.");
			break;
		case WSAENETUNREACH:
			fprintf(stderr,"WSAENETUNREACH:The network cannot be reached from this host at this time.");
			break;
		case WSAENOBUFS:
			fprintf(stderr,"WSAENOBUFS:No buffer space is available. The socket cannot be connected.");
			break;
		case WSAENOTCONN:
			fprintf(stderr,"WSAENOTCONN:The socket is not connected.");
			break;
		case WSAENOTSOCK:
			fprintf(stderr,"WSAENOTSOCK:The descriptor is a file, not a socket.");
			break;
		case WSAETIMEDOUT:
			fprintf(stderr,"WSAETIMEDOUT:The attempt to connect timed out without establishing a connection. ");
			break;
		default:
			fprintf(stderr,"WSAEError: Unknown! ");
			break;	
	}
	fprintf(stderr,"\n");
}

/*------------------------------------------------------------------------
 * connectsock - TCP/UDP创建客户端socket,并连接服务器
 *  serv_ip--服务器ip地址
 *  port--端口号
 *  transport--协议类型(TCP/UDP)
 *------------------------------------------------------------------------
 */
SOCKET connectsock(const char *serv_ip, const int port, const char *transport )
{
	struct sockaddr_in sin;	
	SOCKET s;
	int type,protocol; //协议、报文类型

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;

	 /* 设置端口号 */
	if ( (sin.sin_port = htons(port)) == 0)
	{
		fprintf(stderr,"Can't get \"%d\" port number \n", port);
		return INVALID_SOCKET;
	}

    /* 设置IP地址 */
	if ( (sin.sin_addr.s_addr = inet_addr(serv_ip)) == INADDR_NONE)
	{
		fprintf(stderr,"Can't get \"%s\" IP address \n", serv_ip);
		return INVALID_SOCKET;
	}

	if (strcmp(transport, "udp") == 0)
	{
		type = SOCK_DGRAM;
		protocol = IPPROTO_UDP;
	}
	else
	{
		type = SOCK_STREAM;
		protocol = IPPROTO_TCP;
	}
	/* 创建socket */
	s = socket(AF_INET, type, protocol);
	if (s == INVALID_SOCKET)
	{
		sockerror("Can't create socket!");
		return INVALID_SOCKET;
	}

    /* 连接socket */
	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		fprintf(stderr,"Can't connect to %s.%d:\n",  serv_ip, port);
		sockerror("");
		return INVALID_SOCKET;
	}
	return s;
}

/*------------------------------------------------------------------------
 * connectTCP - TCP客户端创建socket，并连接到服务器
 *------------------------------------------------------------------------
 */
SOCKET connectTCP(const char *serv_ip, const int port )
{
	return connectsock( serv_ip, port, "tcp");
}


/*------------------------------------------------------------------------
 * connectUDP - UDP客户端创建socket，并连接到服务器
 *------------------------------------------------------------------------
 */
SOCKET connectUDP(const char *serv_ip, const int port )
{
	return connectsock( serv_ip, port, "udp");
}

/*------------------------------------------------------------------------
 * passivesock - TCP/UDP服务器端创建socket,并绑定端口号&监听连接请求(TCP)
 *  port--端口号
 *  transport--协议类型(TCP/UDP)
 *  qlen--监听的最大连接数
 *------------------------------------------------------------------------
 */
SOCKET passivesock(const int port, const char *transport, int qlen)
{
	struct sockaddr_in sin;	
	SOCKET		s;	
	int		type, protocol; //协议、报文类型

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);

	/* 设置端口号 */
	if ( (sin.sin_port = htons(port)) == 0 )
	{
		fprintf(stderr,"Can't get \"%d\" port number \n", port);
		return INVALID_SOCKET;
	}
	
    /* 设置ip地址 */
	if ( sin.sin_addr.s_addr  == INADDR_NONE)
	{
		fprintf(stderr,"Can't get IP address \n");
		return INVALID_SOCKET;
	}
    
	if (strcmp(transport, "udp") == 0)
	{
		type = SOCK_DGRAM;
		protocol = IPPROTO_UDP;
	}
	else
	{
		type = SOCK_STREAM;
		protocol = IPPROTO_TCP;
	}

   /* 创建socket */
	s = socket(AF_INET, type, protocol);
	if (s == INVALID_SOCKET)
	{
		sockerror("Can't create socket!");
		return INVALID_SOCKET;
	}

    /* 绑定socket */
	if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		fprintf(stderr, "Can't bind to port: %d \n",  port);
		sockerror("");
		return INVALID_SOCKET;
	}

	if (type == SOCK_STREAM && listen(s, qlen) == SOCKET_ERROR)
	{
		fprintf(stderr,"Can't listen to port: %d \n",  port);
		sockerror("");
		return INVALID_SOCKET;
	}
	return s;
}

/*------------------------------------------------------------------------
 * passiveTCP - TCP服务器端创建socket,并绑定端口号,监听连接请求
 *------------------------------------------------------------------------
 */
SOCKET passiveTCP(const int port, int qlen)
{
	return passivesock(port, "tcp", qlen);
}

/*------------------------------------------------------------------------
 * passiveUDP - UDP服务器端创建socket,并绑定端口号
 *------------------------------------------------------------------------
 */
SOCKET passiveUDP(const int port)
{
	return passivesock(port, "udp", 0);
}
