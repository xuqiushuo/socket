#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
/* 服务端绑定ip地址端口号并监听端口--准备连接*/
int initserver(int type,const struct sockaddr *addr,socklen_t len,int qlen)
{
	printf("Server is running\n");
	int fd;

	if((fd = socket(addr->sa_family,type,0)) < 0)
	{
		printf("socket error\n");
		return 0;
	}

	//addr:服务端自己的地址信息(协议族、IP、端口号)
	if(bind(fd,addr,len) < 0)
	{
		printf("bind error\n");
		close(fd);
		return -1;
	}

	if(type == SOCK_STREAM || type == SOCK_SEQPACKET)
	{
		if(listen(fd,qlen) < 0)
		{
			printf("listen error\n");
			close(fd);
			return -1;
		}
	}

	return fd;
}
 
int main(void)
{
	int socket_fd, connect_fd;
	struct sockaddr_in seraddr;
	char buff[4096];

	seraddr.sin_family = AF_INET;
	seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	seraddr.sin_port = htons(8000);

	if((socket_fd = initserver(SOCK_STREAM,(struct sockaddr *)&seraddr,sizeof(seraddr),10)) == -1)
	{
		printf("initserver error\n");
		return 0;
	}

	while(1)
	{
		//与客户端建立连接
		if((connect_fd=accept(socket_fd,NULL,NULL)) == -1)
		{
			printf("accept error:%s\n",strerror(errno));
			continue;
		}

		//收
		int n = recv(connect_fd,buff,4096,0);
		if(!fork())
		{
			//接收消息成功，回应客户端
			if(send(connect_fd,"Hello,you are connected!\n",26,0) == -1)
			{
				printf("send error\n");
			}
			
			close(connect_fd);
			return 0;
		}
		buff[n] = '\0';
		printf("recv msg from client:%s\n", buff);
		close(connect_fd);
	}
	close(socket_fd);
	return 0;
}

