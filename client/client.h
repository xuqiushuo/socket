#if 0
#ifndef __CLIENT_H__
#define __CLIENT_H__
#include <Winsock2.h>
#include "stdafx.h"

#define HAVE_DATA	1
#define EMPTY	0

typedef struct tagthread_handle{
	uint32_t stream_id;
	char file_name[100];
	uint64_t file_size;
	uint32_t counter;
	struct tagthread_handle *next;
	struct tagthread_handle *pre;
}thread_handle;

//extern SOCKET cmd_sock;
//extern SOCKET send_sock;
//extern SOCKET recv_sock;
//extern char cmd_send_buf[100];
//extern char cmd_recv_buf[100];
//extern char send_buf[264];
//extern char recv_buf[264];
//extern HANDLE sendMutex;
//extern HANDLE recvMutex;
//extern HANDLE handleMutex;
//extern unsigned char send_buf_flag;
//extern unsigned char recv_buf_flag;
//extern thread_handle *head;

#endif

#endif