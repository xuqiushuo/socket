#include <stdio.h>
#include <Winsock2.h> 
#pragma   comment(lib, "ws2_32.lib ")
#include "stdio.h"
#include <io.h>
#include <direct.h>
#include "osfile.h"
#include "stdafx.h"

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
	char status;
	struct tagctrl_handle *next;
}ctrl_handle;

HANDLE handleMutex;
ctrl_handle *head_handle;

HANDLE handleMutex;

DWORD WINAPI downloadThread(LPVOID lpParameter);
DWORD WINAPI uploadThread(LPVOID lpParameter);

int main()
{
    WORD version_reqs;
	WSADATA wsaData;
	SOCKADDR_IN addrSrv;
	SOCKADDR_IN addrSrv2;
	SOCKET cmd_sock;
//	SOCKET new_sock;
	ctrl_handle *head_handle;
	ctrl_handle *pos;
	ctrl_handle *pos2;
	HANDLE new_task;
	DWORD dw;
	char cmd_buf[105];
	int err;
	char cwd[100];
	char cmd[5];
	char arg[100];
	unsigned file_type;
	char path_temp[100];
	char serIP[20];
	uint64_t temp_size;
	
	uint32_t ctrl_id;

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
	
	handleMutex = CreateMutex(NULL,FALSE,NULL);
	
	cmd_sock = socket(AF_INET,SOCK_STREAM,0);
//	new_sock = socket(AF_INET,SOCK_STREAM,0);
	memset(serIP,0,20);
	printf("input server ip:");
	scanf("%s",serIP);
   
    addrSrv.sin_addr.S_un.S_addr=inet_addr(serIP);
    addrSrv.sin_family=AF_INET;
    addrSrv.sin_port=htons(2120);
	
	addrSrv2.sin_addr.S_un.S_addr=inet_addr(serIP);
    addrSrv2.sin_family=AF_INET;
    addrSrv2.sin_port=htons(2125);

	memset(cmd_buf,0,105);
	memset(cmd,0,5);
	memset(arg,0,100);
	memset(path_temp,0,100);
	memset(cwd,0,100);
	
    connect(cmd_sock,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
	send(cmd_sock,"cmd_sock connect ok!",strlen("cmd_sock connect ok!")+1,0);
	recv(cmd_sock,cmd_buf,100,0);
    printf("%s\n",cmd_buf);

	handleMutex = CreateMutex(NULL,FALSE,NULL);
	pos = head_handle = (ctrl_handle*)malloc(sizeof(ctrl_handle));
	pos->next=NULL;

	recv(cmd_sock,cmd_buf,100,0);
	memcpy(cwd,cmd_buf,100);
	while(1)
	{
		printf("<%s>",cwd);
		scanf("%s %s",cmd,arg);
		//列出文件
		if(cmd[0]=='l'&&cmd[1]=='s')
		{
			memcpy(cmd_buf,cmd,5);
			send(cmd_sock,cmd_buf,105,0);
			memset(cmd_buf,0,105);
			recv(cmd_sock,cmd_buf,105,0);
			while(1)
			{
				memcpy(&file_type,cmd_buf,4);
				if(file_type==_A_SUBDIR)
				{
					printf("<DIR>	%s\n",cmd_buf+4);
				}
				if(file_type==_A_NORMAL||file_type==_A_ARCH||file_type==_A_RDONLY)
				{
					printf("	%s\n",cmd_buf+4);
				}
				recv(cmd_sock,cmd_buf,105,0);
				if(cmd_buf[0]=='F')
				{
					break;
				}
			}
			memset(cmd_buf,0,105);
			memset(arg,0,100);
			memset(cmd,0,5);
		}
		//打开目录
		else if(cmd[0]=='c'&&cmd[1]=='d')
		{
			memcpy(cmd_buf,cmd,5);
			memcpy(cmd_buf+5,arg,100);
			send(cmd_sock,cmd_buf,105,0);
			recv(cmd_sock,cmd_buf,105,0);
			memcpy(cwd,cmd_buf,100);
			memset(cmd_buf,0,105);
			memset(arg,0,100);
			memset(cmd,0,5);
		}
		//下载
		else if(cmd[0]=='d'&&cmd[1]=='l')
		{
			printf("input the path to store:");
			scanf("%s",path_temp);

			memcpy(cmd_buf,cmd,5);
			memcpy(cmd_buf+5,arg,100);
			send(cmd_sock,cmd_buf,105,0);

			pos->next= (ctrl_handle*)malloc(sizeof(ctrl_handle));
			pos=pos->next;
			pos->next=NULL;
			pos->current_size=0;
			filename_to_path(arg,path_temp);
			strcpy(pos->file_name,path_temp);
			pos->type='d';
			pos->socket=socket(AF_INET,SOCK_STREAM,0);
			new_task = CreateThread(NULL,0,downloadThread,pos,CREATE_SUSPENDED,&dw);
			pos->handle=new_task;
			recv(cmd_sock,cmd_buf,105,0);
			memcpy(&(pos->stream_id),cmd_buf+4,4);
			memcpy(&(pos->file_size),cmd_buf,4);
			if(connect(pos->socket,(SOCKADDR*)&addrSrv2,sizeof(SOCKADDR))==SOCKET_ERROR)
			{
				printf("connect err!\n");
			}
			pos->status='r';
			ResumeThread(pos->handle);
			memset(cmd_buf,0,105);
			memset(arg,0,100);
			memset(cmd,0,5);
			memset(path_temp,0,100);
		}
		//上传
		else if(cmd[0]=='u'&&cmd[1]=='l')
		{
			memcpy(cmd_buf,cmd,5);
			memcpy(cmd_buf+5,arg,100);
			send(cmd_sock,cmd_buf,105,0);
			pos->next= (ctrl_handle*)malloc(sizeof(ctrl_handle));
			pos=pos->next;
			pos->next=NULL;
			pos->current_size=0;
			strcpy(pos->file_name,arg);
			pos->file_size=get_file_size(arg);
			pos->type='u';
			pos->socket=socket(AF_INET,SOCK_STREAM,0);
			new_task = CreateThread(NULL,0,uploadThread,pos,CREATE_SUSPENDED,&dw);
			pos->handle=new_task;
			memcpy(cmd_buf,&(pos->file_size),4);
			send(cmd_sock,cmd_buf,105,0);
			recv(cmd_sock,cmd_buf,105,0);
			memcpy(&(pos->stream_id),cmd_buf,4);

			if(connect(pos->socket,(SOCKADDR*)&addrSrv2,sizeof(SOCKADDR))==SOCKET_ERROR)
			{
				printf("connect err!\n");
			}
			pos->status='r';
			ResumeThread(pos->handle);
			memset(cmd_buf,0,105);
			memset(arg,0,100);
			memset(cmd,0,5);

//			printf("%s\n",cmd_buf);
//			send(cmd_sock,cmd_buf,100,0);
		}
		//暂停
		else if(cmd[0]=='s'&&cmd[1]=='p')
		{

			ctrl_id=char_to_num(arg);
			WaitForSingleObject(handleMutex, INFINITE);
			pos2=head_handle->next;
			while(pos2)
			{
				if(pos2->stream_id==ctrl_id)
				{
					SuspendThread(pos2->handle);
					pos2->status='p';
				}
				pos2=pos2->next;
			}
			ReleaseMutex(handleMutex);
							
			memset(cmd_buf,0,105);
			memset(arg,0,100);
			memset(cmd,0,5);
		}
		//开始
		else if(cmd[0]=='s'&&cmd[1]=='r')
		{
			ctrl_id=char_to_num(arg);
			WaitForSingleObject(handleMutex, INFINITE);
			pos2=head_handle->next;
			while(pos2)
			{
				if(pos2->stream_id==ctrl_id)
				{
					pos2->status='r';
					ResumeThread(pos2->handle);
				}
				pos2=pos2->next;
			}
			ReleaseMutex(handleMutex);
			
			memset(cmd_buf,0,105);
			memset(arg,0,100);
			memset(cmd,0,5);
		}
		//显示进度
		else if(cmd[0]=='s'&&cmd[1]=='h')
		{
			WaitForSingleObject(handleMutex, INFINITE);
			pos2=head_handle->next;
			while(pos2)
			{
				printf("id(%d)	",pos2->stream_id);
				if(pos2->type=='u')
				{
					temp_size=100*((uint64_t)(pos2->current_size));
					printf("upload------->%d%%",temp_size/(pos2->file_size));
				}
				else if(pos2->type=='d')
				{
					temp_size=100*((uint64_t)(pos2->current_size));
					printf("download----->%d%%",temp_size/(pos2->file_size));
				}
				if(pos2->status=='r')
				{
					printf("	running\n");
				}
				else if(pos2->status=='p')
				{
					printf("	pause\n");
				}
				else if(pos2->status=='o')
				{
					printf("	over\n");
				}
				pos2=pos2->next;
			}
			ReleaseMutex(handleMutex);
		}
	}

	closesocket(cmd_sock);


    WSACleanup();
	system("pause");
	return 1;
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
		if(current_size==file_size)
		{
			handle->status='o';
		}
//printf("%8d:stream--%d ,current_size--%d\n",n,handle->stream_id,current_size);

		ReleaseMutex(handleMutex);
		

	}
	fclose(fp);
//	printf("%8d:stream--%d ,current_size--%d\n",n,handle->stream_id,current_size);
//	printf("trans over\n");
	
	return 0;
}


DWORD WINAPI uploadThread(LPVOID lpParameter)
{
	char file_name[PATH_MAX];
	uint32_t file_size=0;
	uint32_t current_size=0;
	uint32_t size=0;
	SOCKET mysock;
	uint32_t stream_id;
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
		
		if(current_size==file_size)
		{
			handle->status='o';
		}

//		printf("%8d----%d ,current_size--%d\n",n,handle->stream_id,current_size);

		ReleaseMutex(handleMutex);	
		

		
	}
	fclose(fp);
	
	
	//printf("trans over %d\n",handle->stream_id);
	
	return 0;
}