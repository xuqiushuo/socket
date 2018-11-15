#if 0

#include "service.h"
#include "stdafx.h"

DWORD WINAPI downloadThread(LPVOID lpParameter)
{
	uint64_t current_size = 0;
	uint32_t stream_id=-1;
	size_t size = 0;
	char databuf[256];

	thread_handle *handle = (thread_handle*)lpParameter;
	
	FILE *fp;
	fp = fopen(handle->file_name,"wb");

	while(current_size<handle->file_size)
	{
		WaitForSingleObject(recvMutex, INFINITE);
		if(recv_buf_flag == HAVE_DATA)
		{
			stream_id = recv_buf[0]<<24 | recv_buf[1]<<16 | recv_buf[2]<<8 | recv_buf[3];
			if(stream_id == handle->stream_id)
			{
				size = recv_buf[4]<<24 | recv_buf[5]<<16 | recv_buf[6]<<8 | recv_buf[7];
				memcpy(databuf, recv_buf+8, size);
				recv_buf_flag = EMPTY;
				ReleaseMutex(recvMutex);
				fwrite(databuf,1,size,fp);
				current_size +=current_size;
			}
			else
			{
				ReleaseMutex(recvMutex);
			}
		}
		else
		{
			recv(recv_sock,recv_buf,264,0);
			recv_buf_flag = HAVE_DATA;
			stream_id = recv_buf[0]<<24 | recv_buf[1]<<16 | recv_buf[2]<<8 | recv_buf[3];
			if(stream_id == handle->stream_id)
			{
				size = recv_buf[4]<<24 | recv_buf[5]<<16 | recv_buf[6]<<8 | recv_buf[7];
				memcpy(databuf, recv_buf+8, size);
				recv_buf_flag = EMPTY;
				ReleaseMutex(recvMutex);
				fwrite(databuf,1,size,fp);
				current_size +=current_size;
			}
			else
			{
				ReleaseMutex(recvMutex);
			}
		}
	}
	fclose(fp);

	WaitForSingleObject(handleMutex, INFINITE);
	if(handle->next==NULL)
	{
		handle->pre->next = NULL;
	}
	else
	{
		handle->pre->next = handle->next;
		handle->next->pre = handle->pre;
	}
	free(handle);
	head->counter--;
	ReleaseMutex(handleMutex);
	return 0;
}

#endif