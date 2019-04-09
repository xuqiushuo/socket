#ifndef __OSFILE_H__
#define __OSFILE_H__
#include "stdafx.h"

extern char * find_name_from_path(char *path);
extern uint32_t get_file_size(char *path);
extern char * filename_to_path(char * filename,char * cwd);
extern int ch_dir(char *dir,char *cwd);

#endif