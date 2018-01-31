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
/* http://infocenter.arm.com/help/topic/com.arm.doc.ihi0044f/IHI0044F_aaelf.pdf */
/* ============================ [ INCLUDES  ] ====================================================== */
#include <stdlib.h>
#include "vfs.h"
#include "elf/elf.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
static void* do_load_elf(void* elfFile)
{
	void* elf = NULL;

	return elf;
}
/* ============================ [ FUNCTIONS ] ====================================================== */
void* ELF_LoadFile(const char* filename)
{
	void* elf = NULL;
	void* faddr = NULL;
	VFS_FILE* fp;
	struct vfs_stat st;

	if( (0==vfs_stat(filename, &st)) &&
		(VFS_ISREG(st.st_mode)) )
	{
		fp = vfs_fopen(filename, "rb");

		if(NULL != fp)
		{
			faddr = malloc(st.st_size);
			if(NULL != faddr)
			{
				vfs_fread(faddr, st.st_size, 1, fp);
			}
			vfs_fclose(fp);
		}
	}

	if(NULL != faddr)
	{
		free(faddr);
		elf = do_load_elf(faddr);
	}

	return elf;
}
