/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2018  AS <parai@foxmail.com>
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation; See <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */
/* ============================ [ INCLUDES  ] ====================================================== */
#include <stdlib.h>
#include "dlfcn.h"
#include "vfs.h"
#include "elf/elf.h"
#ifdef USE_SHELL
#include "shell.h"
#endif
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
typedef struct
{
	void* addr; /* module loaded in memory, the start address */
}dll_t;

/* ============================ [ DECLARES  ] ====================================================== */
#ifdef USE_SHELL
static int dllFunc(int argc, char* argv[]);
#endif
/* ============================ [ DATAS     ] ====================================================== */
#ifdef USE_SHELL
static SHELL_CONST ShellCmdT dllCmd  = {
	dllFunc,
	1,1,
	"dll",
	"dll [path]",
	"load dll and run the code from the main entry\n",
	{NULL,NULL}
};
SHELL_CMD_EXPORT(dllCmd);
#endif
/* ============================ [ LOCALS    ] ====================================================== */
#ifdef USE_SHELL
static int dllFunc(int argc, char* argv[])
{
	int r = 0;
	dll_t* dll = dlopen(argv[1], RTLD_NOW);
	if(NULL != dll)
	{
		
	}
	else
	{
		r = -1;
	}
	return r;
}
#endif
/* ============================ [ FUNCTIONS ] ====================================================== */
void *dlopen(const char *filename, int flag)
{
	dll_t* dll = NULL;
	VFS_FILE* fp;
	struct vfs_stat st;
	(void) flag; /* always RTLD_NOW */

	if( (0==vfs_stat(filename, &st)) &&
		(VFS_ISREG(st.st_mode)) )
	{
		fp = vfs_fopen(filename, "rb");
	
		if(NULL != fp)
		{
			dll = malloc(st.st_size+sizeof(dll_t));
			if(NULL != dll)
			{
				dll->addr = &dll[1];
				vfs_fread(dll->addr, st.st_size, 1, fp);
			}
			vfs_fclose(fp);
		}
	}

	if(NULL != dll)
	{
		if(0 != elf_checkFile(dll->addr))
		{
			free(dll);
			dll = NULL;
		}
	}

	return dll;
}

void *dlsym(void *handle, const char *symbol)
{
}

int dlclose(void *handle)
{

}

