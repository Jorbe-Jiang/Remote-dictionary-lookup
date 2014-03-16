#include "sock_common.h"
#include <time.h>
#include <conio.h>
#include <string.h>

#define BUFFER_SIZE    500


/*
* 编译的时候需要和sock_common.c一起编译,否则函数将无法调用
*/

struct Dict_Tree
{
	struct Dict_Tree *child[26];  //26个子节点代表26个字母
	char c;     //节点关键字
	int flag;  //记录单词所在行数
};
struct Dict_Tree *root;  //字典树的根节点
int find(char *s1);   //从字典树中查找单词
void insert_word();  //从文件中读取单词并插入到字典树中
void find_word(char *pString,char *pBuffer);  //从字典树中查找单词，并将单词的词性和意思保存到pBuffer数组

/* 插入单词和行数到字典树中 */ 
void insert(char *s,int line_no)
{
	
	int m=0,j=0;  
	int index;
	int len=strlen(s);
	struct Dict_Tree *current;
	if(len==0) return;
	current=root;
	while(m<len)
	{
		index=s[m]-97;
		if(current->child[index]==NULL)
		{
			current->child[index]=(struct Dict_Tree *)malloc(sizeof(struct Dict_Tree));
			current->child[index]->c=s[m];
			current->child[index]->flag=0;
			for(j=0;j<26;j++)
			{
				current->child[index]->child[j]=NULL;
			}
		}
		current=current->child[index];
		if(m==len-1)
			current->flag=line_no;   //把单词所在行数插入到字典树末端
		m++;
	}
}


/* 从字典树中查找单词 */
int find(char *s1)
{
	
	int no;   //单词在文件中的行数 
	int m=0;  
	int index;
	int len=strlen(s1);
	struct Dict_Tree *current;
	if(len==0) return 0;
	current=root;
	while(m<len)
	{
		index=s1[m]-97;
		if(current->child[index]==NULL)
			return 0;
		current=current->child[index];
		m++;
		if(m==len)
		{
			no=current->flag;  //单词的行数
			return no;
		}
	}
	return 0;
}


//从文件中读取单词并插入到字典树中  
void insert_word()
{
	int begin=0; //第一个空格字符出现的位置 
	char line[500];  //保存从文件里读取的每行数据 
	char word[500];  //保存每行数据里的单词 
	int k=0,j=0;
	int line_no=1;   //记录单词在文件中的行数 
	
	FILE *fp;
	fp=fopen("a.txt","r");
	memset(line,0,sizeof(line));
	memset(word,0,sizeof(word));
	if(fp==NULL)
	{
		printf("Failed open file !\n");
		return;
	}
	else
	{
		while(!feof(fp))
		{	
			fgets(line,500,fp);
			for(k=0;k<strlen(line);k++)
			{
				if(line[k]==' ')
				{
					begin=k;
					break;
				}
			}
			
			for(j=0;j < begin;j++)
			{
				word[j]=line[j];
			}
			insert(word,line_no);  //将单词和行数插入到字典树中 
			memset(line,0,strlen(line));  //清空line数组 
			memset(word,0,strlen(word)); //清空word数组    
			line_no++;
		}
		fclose(fp);
	}
}

void main(int argc, char *argv[])
{
	SOCKET               serv_sock;
	SOCKET               NewConnection;
	SOCKADDR_IN          ClientAddr;
	int                  ClientAddrLen;
	int                  Ret;     //从客户端接收到的字节数
	const int            port=7000; //指定端口号为7000
	char				 pBuffer[BUFFER_SIZE];  //保存将发送到客户端的数据
	char                 pString[BUFFER_SIZE]; //保存从客户端接收到的数据
	int                  i;

	/* 初始化字典树根节点 */
	root=(struct Dict_Tree *)malloc(sizeof(struct Dict_Tree));   
    for(i=0;i<26;i++)   
        root->child[i]=NULL;   


	//初始化WSA	
	if ((Ret = WinSockInit()) != 0)
	{
		printf("WSAStartup failed with error %d \n",Ret);
		return;
	}

	serv_sock=passiveTCP(port, 7);

	insert_word();  //将文件中的单词插入到字典树中

	//循环监听接收与发送数据
	while(!kbhit())   
	{
		// Accept a new connection
		ClientAddrLen = sizeof (ClientAddr);
		if ((NewConnection = accept(serv_sock, (SOCKADDR *) &ClientAddr,&ClientAddrLen)) == INVALID_SOCKET)
		{
			sockerror("Accept socket failed!\n ");
			closesocket(serv_sock);
			WinSockDestroy();
			return;
		}

		// Start sending and receiving data on NewConnection
		Ret = recv(NewConnection,pString,BUFFER_SIZE,0);
		if (Ret == SOCKET_ERROR)
		{
			sockerror("Receive data failed!");
			closesocket(NewConnection);
			closesocket(serv_sock);
			WinSockDestroy();
			return;
		}
		pString[Ret] = '\0';

		printf("Word : %s \n",pString);
		find_word(pString,pBuffer);   //从字典树中查找单词
		printf("Property & Meaning : %s \n",pBuffer);
		if ((Ret = send(NewConnection,pBuffer,strlen(pBuffer), 0)) == SOCKET_ERROR)
		{
			sockerror("Send date failed!\n");
			closesocket(NewConnection);
			closesocket(serv_sock);
			WinSockDestroy();
			return;
		} 
	
        //关闭新的socket
		closesocket(NewConnection);
	}
	
   //关闭listeningsocket
	closesocket(serv_sock);

	// 注销WSA
	WinSockDestroy();
}

//从字典树中查找单词，并将单词的词性和意思保存到pBuffer数组里 
void find_word(char *pString,char *pBuffer)
{
	int begin=0; //第一个空格字符出现的位置 
	char line[500];  //保存从文件里读取的每行数据 
	int no=0,k=0;
	char *a;  //保存strchr函数返回的指针 
	int line_no=find(pString); //单词在文件中的行数 
	memset(line,0,strlen(line));

	if(line_no)
	{
		FILE *fp;
		fp=fopen("a.txt","r");
		memset(line,0,sizeof(line));
		if(fp==NULL)
		{
			printf("Failed open file !\n");
			return;
		}
		else
		{
			while(!feof(fp))
			{
				no++;
				fgets(line,500,fp);
				if(no==line_no)
				{
					//将空格之后的字符存入pBuffer数组 
					a=strchr(line,' '); 
					for(k=0;k<strlen(a)+1;k++)
					{
						pBuffer[k]=a[k];
					}
					break;
				}
				memset(line,0,strlen(line));
			}
		}
		fclose(fp);
	}		
	if(line_no==0)
	{
		strcpy(pBuffer,"Can't find the word !\n");
	}
	
}
