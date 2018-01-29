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
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
typedef struct
{
	void* addr; /* module loaded in memory, the start address */
}dll_t;
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void *dlopen(const char *filename, int flag)
{
	dll_t* dll = NULL;
	VFS_FILE* fp;

	fp = vfs_fopen(filename, "rb");
	
	if(NULL != fp)
	{

	}

	return dll;
}

void *dlsym(void *handle, const char *symbol)
{
}

int dlclose(void *handle)
{

}

