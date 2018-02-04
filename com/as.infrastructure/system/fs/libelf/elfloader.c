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
#include "elfinternal.h"
/* ============================ [ MACROS    ] ====================================================== */
#define ISELF(elfFile) ( (((Elf32_Ehdr*)elfFile)->e_ident[EI_MAG0] == ELFMAG0) && \
						 (((Elf32_Ehdr*)elfFile)->e_ident[EI_MAG1] == ELFMAG1) && \
						 (((Elf32_Ehdr*)elfFile)->e_ident[EI_MAG2] == ELFMAG2) && \
						 (((Elf32_Ehdr*)elfFile)->e_ident[EI_MAG3] == ELFMAG3) )
#define ISELF32(elfFile) ( ((Elf32_Ehdr*)elfFile)->e_ident[EI_CLASS] == ELFCLASS32 )
#define ISELF64(elfFile) ( ((Elf32_Ehdr*)elfFile)->e_ident[EI_CLASS] == ELFCLASS64 )

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
static void* do_load_elf(void* elfFile)
{
	void* elf = NULL;

	if (!ISELF(elfFile))
	{
		/* Invalid ELF */
	}
	else if (ISELF32(elfFile))
	{
		elf = ELF32_Load(elfFile);
	}
	else
	{
		/*ELF64 not supported now */
	}

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
		elf = do_load_elf(faddr);
		free(faddr);
	}

	return elf;
}

Elf32_Addr ELF_FindSymbol(const char* name)
{
	Elf32_Addr addr = 0;

	return addr;
}
