#include "stdio.h"
#include "stdlib.h"
#include <sys/stat.h>
#include <io.h>
#include <FCNTL.H>
#include <direct.h>
#include "stdafx.h"

char * find_name_from_path(char *path)
{
	char *pos;
	int len = 0;
	len = strlen(path);
	if(len==0)
	{
		return NULL;
	}
	else
	{
		pos=path+len-1;
		while(*pos!='\\')
		{
			pos--;
		}
		pos+=1;
	}
	return pos;
}

uint32_t get_file_size(char *path)
{
	struct stat buf;

	if (stat(path, &buf) < 0)
	{
        printf("Mesg: %s\n", strerror(errno));
        exit(-1);
	}
	return buf.st_size;
}

char * filename_to_path(char * filename,char * cwd)
{
	char * result=cwd;
	while(*cwd!='\0')
	{
		cwd++;
	}
	if(*(cwd-1)!='\\')
	{
		*cwd='\\';
		cwd++;
	}
	
	while(*filename!='\0')
	{
		*cwd=*filename;
		cwd++;
		filename++;
	}
	*cwd='\0';
	return result;
}

int ch_dir(char *dir,char *cwd)
{
	int ret;
	ret = _chdir(dir);
	_getcwd( cwd, 100 );
	return ret;
}

uint32_t char_to_num(char *num)
{
	char number[20];
	uint32_t ret=0;
	char *pos;
	int n,m;
	uint32_t ll=1;
	memset(number,0,20);
	strcpy(number,num);
	pos = number;
	while(*pos!='\0')
	{
		pos++;
	}
	pos--;
	for(n=0;pos>=number;n++)
	{
		m=n;
		while(--m+1)
		{
			ll*=10;
		}
		ret+=(*pos-'0')*ll;
		ll=1;
		pos--;
	}
	return ret;

}
