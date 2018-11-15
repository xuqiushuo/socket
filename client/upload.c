#if 0

DWORD WINAPI downloadThread(LPVOID lpParameter)
{
	uint64_t current_size = 0;
	size_t size = 0;
	char databuf[256];
	
	thread_handle *handle = (thread_handle*)lpParameter;
	
	FILE *fp;
	fp = fopen(handle->file_name,"rb");
	
	while(!feof(fp))
	{
		size = fread(databuf,1,256,fp);
		WaitForSingleObject(sendMutex, INFINITE);
		memcpy(send_buf,&(handle->stream_id), 4);
		memcpy(send_buf+4,&size,4);
		memcpy(send_buf+8,databuf,size);
		send(send_sock,send_buf,264,0);
		current_size+=size;
		ReleaseMutex(sendMutex);
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