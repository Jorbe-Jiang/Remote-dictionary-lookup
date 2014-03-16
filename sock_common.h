#ifndef		SOCK_COMMON_H
#define		SOCK_COMMON_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
*WinSockInit - 初始化WSA
*------------------------------------------------------------------------
*/
int WinSockInit();

/*------------------------------------------------------------------------
* WinSockDestroy - 注销WSA
*------------------------------------------------------------------------
*/
void WinSockDestroy();

/*------------------------------------------------------------------------
 * sockerror 用来输出网络函数的错误信息
 *------------------------------------------------------------------------
 */
void sockerror(const char *format, ...);

/*------------------------------------------------------------------------
 * connectsock - TCP/UDP创建客户端socket,并连接服务器
 *  serv_ip--服务器ip地址
 *  port--端口号
 *  transport--协议类型(TCP/UDP)
 *------------------------------------------------------------------------
 */
SOCKET connectsock(const char *serv_ip, const int port, const char *transport );

/*------------------------------------------------------------------------
 * connectTCP - TCP客户端创建socket，并连接到服务器
 *------------------------------------------------------------------------
 */
SOCKET connectTCP(const char *serv_ip, const int port );

/*------------------------------------------------------------------------
 * connectUDP - UDP客户端创建socket，并连接到服务器
 *------------------------------------------------------------------------
 */
SOCKET connectUDP(const char *serv_ip, const int port );

/*------------------------------------------------------------------------
 * passivesock - TCP/UDP服务器端创建socket,并绑定端口号&监听连接请求(TCP)
 *  port--端口号
 *  transport--协议类型(TCP/UDP)
 *  qlen--监听的最大连接数
 *------------------------------------------------------------------------
 */
SOCKET passivesock(const int port, const char *transport, int qlen);

/*------------------------------------------------------------------------
 * passiveTCP - TCP服务器端创建socket,并绑定端口号,监听连接请求
 *------------------------------------------------------------------------
 */
SOCKET passiveTCP(const int port, int qlen);

/*------------------------------------------------------------------------
 * passiveUDP - UDP服务器端创建socket,并绑定端口号
 *------------------------------------------------------------------------
 */
SOCKET passiveUDP(const int port);

#ifdef __cplusplus
}
#endif

#endif		//SOCK_COMMON_H
