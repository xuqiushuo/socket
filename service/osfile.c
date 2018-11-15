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