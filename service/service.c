#include <stdio.h>
#include <Winsock2.h>
#pragma   comment(lib, "ws2_32.lib ")

#include "stdio.h"
#include <io.h>
#include <direct.h>
#include "osfile.h"
#include "stdafx.h"
#include "stdlib.h"

#define PATH_MAX	100
#define BUF_SIZE	512

typedef struct tagctrl_handle{
	HANDLE handle;
	SOCKET socket;
	uint32_t stream_id;
	char file_name[PATH_MAX];
	uint32_t file_size;
	uint32_t current_size;
	char type;
	struct tagctrl_handle *next;
}ctrl_handle;

HANDLE handleMutex;
//HANDLE newSockMutex;

SOCKET new_sock;
unsigned char new_sock_flag;
SOCKET new_sock2;
unsigned char new_sock_flag2;
uint32_t stream_id;

DWORD WINAPI downloadThread(LPVOID lpParameter);
DWORD WINAPI uploadThread(LPVOID lpParameter);
DWORD WINAPI cmdThread(LPVOID lpParameter);
DWORD WINAPI acceptThread(LPVOID lpParameter);

int main()
{
	WORD version_reqs;
	WSADATA wsaData;
	SOCKADDR_IN addrSrv;
	SOCKADDR_IN addrClient;
	SOCKET serv_sock;
	
	HANDLE cmd_thread;
	HANDLE accept_thread;
	DWORD dw,dw2;
	int err;
	int len;
	

	

    version_reqs = MAKEWORD( 1, 1 );
	
    err = WSAStartup(version_reqs, &wsaData );
    if ( err != 0 ) 
	{
		printf("we couldn't find a useable  winsock.dll");
        return 0;
    }
	
    if ( LOBYTE( wsaData.wVersion ) != 1 || HIBYTE( wsaData.wVersion ) != 1 ) 
	{
        WSACleanup( );
		printf("we couldn't find a useable  winsock.dll");
        return 0;
    }
	
    serv_sock = socket(AF_INET,SOCK_STREAM,0);  //AF_INET(又称 PF_INET)是 IPv4 网络协议的套接字类型,AF_INET6 则是 IPv6 的;而 AF_UNIX 则是 Unix 系统本地通信
	
    addrSrv.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	
    addrSrv.sin_family=AF_INET;
	
    addrSrv.sin_port=htons(2120);
	
	bind(serv_sock,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
	
    listen(serv_sock,5);
	
	len=sizeof(SOCKADDR);
	stream_id=0;
	new_sock_flag = 0;
	new_sock_flag2 = 0;
	accept_thread = CreateThread(NULL,0,acceptThread,NULL,0,&dw2);
	while(1)
	{
		if(new_sock_flag==0)
		{
			new_sock = accept(serv_sock,(SOCKADDR*)&addrClient,&len);
			new_sock_flag = 1;
			cmd_thread = CreateThread(NULL,0,cmdThread,&new_sock,0,&dw);
		}
	}
	
    WSACleanup();
	return 1;
}

DWORD WINAPI acceptThread(LPVOID lpParameter)
{
	SOCKADDR_IN addrSrv2;
	SOCKADDR_IN addrClient2;
	SOCKET serv_sock2;
	int len;
	serv_sock2 = socket(AF_INET,SOCK_STREAM,0);
	addrSrv2.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	addrSrv2.sin_family=AF_INET;
	addrSrv2.sin_port=htons(2125);
	bind(serv_sock2,(SOCKADDR*)&addrSrv2,sizeof(SOCKADDR));
	listen(serv_sock2,5);
	len=sizeof(SOCKADDR);
	while(1)
	{
		if(new_sock_flag2==0)
		{
			new_sock2 = accept(serv_sock2,(SOCKADDR*)&addrClient2,&len);
			new_sock_flag2 = 1;
		}
	}
}

DWORD WINAPI cmdThread(LPVOID lpParameter)
{
	SOCKET cmd_sock;
	char cmd_buf[105];
	char cwd[PATH_MAX];
	ctrl_handle *head_handle;
	ctrl_handle *pos;
	HANDLE new_task;
	DWORD dw;
	struct _finddata_t *fileinfo;
	long find_handle;
	char temp_path[100];
	
	memset(cmd_buf,0,105);
	memset(cwd,0,100);
	memset(temp_path,0,100);
	cmd_sock=*((SOCKET *)lpParameter);
	while(!new_sock_flag);
	
	cmd_sock=new_sock;
	new_sock_flag=0;


	recv(cmd_sock,cmd_buf,105,0);
	printf("%s\n",cmd_buf);
	send(cmd_sock,"welcome!",strlen("welcome!")+1,0);

	handleMutex = CreateMutex(NULL,FALSE,NULL);
	pos = head_handle = (ctrl_handle*)malloc(sizeof(ctrl_handle));
	pos->next=NULL;

	strcpy(cwd,"D:");
	ch_dir(cwd,cwd);
	memcpy(cmd_buf,cwd,100);
	send(cmd_sock,cmd_buf,105,0);

	fileinfo=(struct _finddata_t *)malloc(sizeof(struct _finddata_t));
	memset(fileinfo,1,sizeof(struct _finddata_t));
	while(1)
	{
		recv(cmd_sock,cmd_buf,105,0);
		//列出文件
		if(cmd_buf[0]=='l'&&cmd_buf[1]=='s')
		{
			find_handle=_findfirst( "*.*", fileinfo );
			if(find_handle==-1)
			{
				continue;
			}
			else 
			{
				memcpy(cmd_buf,&(fileinfo->attrib),4);
				memcpy(cmd_buf+4,fileinfo->name,100);
				send(cmd_sock,cmd_buf,105,0);
				while(!_findnext( find_handle, fileinfo ))
				{
					memcpy(cmd_buf,&(fileinfo->attrib),4);
					memcpy(cmd_buf+4,fileinfo->name,100);
					send(cmd_sock,cmd_buf,105,0);
				}
				cmd_buf[0]='F';
				send(cmd_sock,cmd_buf,105,0);
			}
		}
		//打开目录
		else if(cmd_buf[0]=='c'&&cmd_buf[1]=='d')
		{
			ch_dir(cmd_buf+5,cwd);
			memcpy(cmd_buf,cwd,100);
			send(cmd_sock,cmd_buf,105,0);
		}
		//下载
		else if(cmd_buf[0]=='d'&&cmd_buf[1]=='l')
		{
			memcpy(temp_path,cwd,100);
			pos->next= (ctrl_handle*)malloc(sizeof(ctrl_handle));
			pos=pos->next;
			pos->next=NULL;
			pos->current_size=0;
			memcpy(pos->file_name,filename_to_path(cmd_buf+5,temp_path),100);
			pos->file_size=get_file_size(pos->file_name);
			pos->stream_id = stream_id;
			pos->type='u';
			new_task = CreateThread(NULL,0,uploadThread,pos,CREATE_SUSPENDED,&dw);
			pos->handle=new_task;
			
			memcpy(cmd_buf,&(pos->file_size),4);
			memcpy(cmd_buf+4,&(pos->stream_id),4);
			send(cmd_sock,cmd_buf,105,0);

			while(!new_sock_flag2);
			pos->socket=new_sock2;
			new_sock_flag2=0;
			ResumeThread(pos->handle);
			stream_id++;

			memset(cmd_buf,0,105);
			memset(temp_path,0,100);
//			printf("%s\n",cmd_buf);
		}
		//上传
		else if(cmd_buf[0]=='u'&&cmd_buf[1]=='l')
		{
			memcpy(temp_path,cwd,100);
			pos->next= (ctrl_handle*)malloc(sizeof(ctrl_handle));
			pos=pos->next;
			pos->next=NULL;
			pos->current_size=0;
			strcpy(pos->file_name,filename_to_path(find_name_from_path(cmd_buf+5),temp_path));
			pos->stream_id = stream_id;
			pos->type='d';
			new_task = CreateThread(NULL,0,downloadThread,pos,CREATE_SUSPENDED,&dw);
			pos->handle=new_task;
			recv(cmd_sock,cmd_buf,105,0);
			memcpy(&(pos->file_size),cmd_buf,4);
			send(cmd_sock,&stream_id,4,0);
			while(!new_sock_flag2);
			pos->socket=new_sock2;
			new_sock_flag2=0;
			ResumeThread(pos->handle);
			stream_id++;

			memset(cmd_buf,0,105);
			memset(temp_path,0,100);
//			printf("%s\n",cmd_buf);
		}
		//暂停
		else if(cmd_buf[0]=='s'&&cmd_buf[1]=='p')
		{
		
		}
		//开始
		else if(cmd_buf[0]=='s'&&cmd_buf[1]=='r')
		{
		
		}
		//显示进度
		else if(cmd_buf[0]=='s'&&cmd_buf[1]=='h')
		{
			
		}
	}
}

DWORD WINAPI downloadThread(LPVOID lpParameter)
{
	
	char file_name[PATH_MAX];
	uint32_t file_size=0;
	uint32_t current_size=0;
	uint32_t size=0;
	SOCKET mysock;
	uint32_t stream_id=-1;
	char data_buf[BUF_SIZE];
	FILE *fp;

unsigned long n=0;

	ctrl_handle *handle = (ctrl_handle*)lpParameter;
	
	WaitForSingleObject(handleMutex, INFINITE);
	memcpy(file_name,handle->file_name,PATH_MAX);
	file_size = handle->file_size;
	mysock = handle->socket;
	stream_id = handle->stream_id;
	ReleaseMutex(handleMutex);
	
	memset(data_buf,0,BUF_SIZE);
	fp = fopen(file_name,"wb");
	if(fp==NULL)
	{
		printf("open file failed!\n");
		return -1;
	}
	
	while(current_size<file_size)
	{
n++;
		recv(mysock,data_buf,BUF_SIZE,0);
		memcpy(&size,data_buf,4);
		fwrite(data_buf+4,1,size,fp);
		current_size +=size;

		WaitForSingleObject(handleMutex, INFINITE);
		handle->current_size=current_size;

//printf("%8d:stream--%d ,current_size--%d\n",n,handle->stream_id,current_size);

		ReleaseMutex(handleMutex);
		

	}
	fclose(fp);
	
printf("trans over\n");
	
	return 0;
}


DWORD WINAPI uploadThread(LPVOID lpParameter)
{
	char file_name[PATH_MAX];
	uint32_t file_size=0;
	uint32_t current_size=0;
	uint32_t size=0;
	SOCKET mysock;
	uint32_t stream_id=-1;
	char data_buf[BUF_SIZE];
	FILE *fp;

unsigned long n=0;
	
	ctrl_handle *handle = (ctrl_handle*)lpParameter;

	WaitForSingleObject(handleMutex, INFINITE);
	memcpy(file_name,handle->file_name,PATH_MAX);
	file_size = handle->file_size;
	mysock = handle->socket;
	stream_id = handle->stream_id;
	ReleaseMutex(handleMutex);
	
	memset(data_buf,0,BUF_SIZE);
	fp = fopen(file_name,"rb");
	if(fp==NULL)
	{
		printf("open file failed!\n");
		return -1;
	}

	while(!feof(fp))
	{
n++;
		size = fread(data_buf+4,1,BUF_SIZE-4,fp);
		memcpy(data_buf,&size,4);
		send(mysock,data_buf,BUF_SIZE,0);
		current_size +=size;

		WaitForSingleObject(handleMutex, INFINITE);
		handle->current_size=current_size;

//printf("%8d----%d ,current_size--%d\n",n,handle->stream_id,current_size);

		ReleaseMutex(handleMutex);	
	

		
	}
	fclose(fp);


//printf("trans over %d\n",handle->stream_id);

	return 0;
}