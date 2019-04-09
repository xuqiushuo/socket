#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<netinet/in.h>
#include<stdint.h>
#include<errno.h>
#include<string.h>
#include<stdio.h>
#include<arpa/inet.h>
#define MAXSIZE 4096
/* 传入ip地址 */
int main(int argc,char **argv)
{
 
	if(2 != argc)
	{
		printf("error:%s\n",argv[0]);
		return 0;
	}

	int sfd;
	int domain = AF_INET;  //ipv4
	int type = SOCK_STREAM; //TCP
	int protocol = 0;
	struct sockaddr_in seraddr;
	char sendline[MAXSIZE],recvline[MAXSIZE];

	if(-1 == (sfd = socket(domain,type,protocol)))
	{
		printf("socket error :%s\n",strerror(errno));
		return 0;
	}

	memset(&seraddr,0,sizeof(seraddr));

	seraddr.sin_family = domain;
	seraddr.sin_port = htons(8000);

	//inet_pton：将“点分十进制” -> “二进制整数”
	if(1 != inet_pton(domain,argv[1],&seraddr.sin_addr))
	{
		printf("inet_pton2 error :%s\n",strerror(errno));
		return 0;
	}

	printf("AF_INET=%d,%d\n",AF_INET,domain);
	printf("sockaddr->family:%d\n",seraddr.sin_family);
	printf("sockaddr->port:%d\n",seraddr.sin_port);
	printf("sockaddr->sin_addr:%d\n",seraddr.sin_addr.s_addr);

	//与服务端建立连接
	if(-1 == connect(sfd,(struct sockaddr *)(&seraddr),sizeof(seraddr)))
	{
		printf("connect error :%s\n",strerror(errno));
		return 0;
	}

	printf("send msg to server:");
	scanf("%s",sendline);
	
	//传、收
	if(send(sfd,sendline,strlen(sendline),0) < 0)
	{
		printf("send msg error :%s\n",strerror(errno));
		return 0;
	}

	int rec_len;
	if((rec_len = recv(sfd,recvline,MAXSIZE,0)) == -1)
	{
		printf("recv msg error\n");
		return 0;
	}
	recvline[rec_len] = '\0';
	printf("the recv msg:%s\n",recvline);
	close(sfd);
	return 0;
}

