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
#include "elfloader.h"
#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */
#define AS_LOG_ELF32 1
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
static boolean ELF32_GetVirtualAddress(void* elfFile, uint32_t *vstart_addr, uint32_t *vend_addr)
{
	uint32_t i;
	boolean has_vstart;
	struct Elf32_Header *fileHdr = elfFile;
	struct Elf32_Phdr *phdr = elf32_getProgramHeaderTable(elfFile);

	has_vstart = FALSE;
	for(i=0; i < fileHdr->e_phnum; i++)
	{
		if(phdr[i].p_type != PT_LOAD)
		{
			continue;
		}
		ASLOG(ELF32, "LOAD segment: %d, 0x%08X, 0x%08x\n",
				i, phdr[i].p_vaddr, phdr[i].p_memsz);
		if(phdr[i].p_memsz < phdr[i].p_filesz)
		{
			ASLOG(ERROR, "invalid elf: segment %d: p_vaddr: %d, p_memsz: %d\n",
					i, phdr[i].p_vaddr, phdr[i].p_memsz);
			has_vstart = FALSE;
			break;
		}
		if (!has_vstart)
		{
			*vstart_addr = phdr[i].p_vaddr;
			*vend_addr = phdr[i].p_vaddr + phdr[i].p_memsz;
			has_vstart = TRUE;
			if (*vend_addr < *vstart_addr)
			{
				ASLOG(ERROR, "invalid elf: segment %d: p_vaddr: %d, p_memsz: %d\n",
						i, phdr[i].p_vaddr, phdr[i].p_memsz);
				has_vstart = FALSE;
				break;
			}
		}
		else
		{
			if (phdr[i].p_vaddr < *vend_addr)
			{
				ASLOG(ERROR, "invalid elf: segment should be sorted and not overlapped\n");
				has_vstart = FALSE;
				break;
			}
			if (phdr[i].p_vaddr > *vend_addr + 16)
			{
				/* There should not be too much padding in the object files. */
				ASWARNING("too much padding before segment %d\n", i);
			}

			*vend_addr = phdr[i].p_vaddr + phdr[i].p_memsz;
			if (*vend_addr < phdr[i].p_vaddr)
			{
				ASLOG(ERROR, "invalid elf: "
						"segment %d address overflow\n", i);
				has_vstart = FALSE;
				break;
			}
		}
	}

	if(*vstart_addr >= *vend_addr)
	{
		ASLOG(ERROR, "invalid eld: start=%08X end=%08X\n",
				*vstart_addr, *vend_addr);
		has_vstart = FALSE;
	}

	return has_vstart;
}
static boolean ELF32_LoadObject(void* elfFile,ELF32_ObjectType* elfObj)
{
	boolean r = TRUE;
	uint32_t i;
	struct Elf32_Header *fileHdr = elfFile;
	struct Elf32_Phdr *phdr = elf32_getProgramHeaderTable(elfFile);
	struct Elf32_Shdr *shdr = elf32_getSectionTable(elfFile);

	for(i=0; i < fileHdr->e_phnum; i++)
	{
		if(PT_LOAD == phdr[i].p_type)
		{
			memcpy(elfObj->space + phdr[i].p_vaddr - elfObj->vstart_addr,
					elfFile + phdr[i].p_offset, phdr[i].p_filesz);
		}
	}

	elfObj->entry = elfObj->space + fileHdr->e_entry - elfObj->vstart_addr;

	/* handle relocation section */
	for (i = 0; i < fileHdr->e_shnum; i ++)
	{
		uint32_t j, nr_reloc;
		struct Elf32_Sym *symtab;
		struct Elf32_Rel *rel;
		uint8_t *strtab;

		if(SHT_REL == shdr[i].sh_type)
		{
			/* get relocate item */
			rel = (struct Elf32_Rel *)(elfFile + shdr[i].sh_offset);
			/* locate .rel.plt and .rel.dyn section */
			symtab = (struct Elf32_Sym *)(elfFile + shdr[shdr[i].sh_link].sh_offset);
			strtab = (uint8_t *)(elfFile +
					shdr[shdr[shdr[i].sh_link].sh_link].sh_offset);
			nr_reloc = (uint32_t)(shdr[i].sh_size / sizeof(struct Elf32_Rel));
			/* relocate every items */
			for (j = 0; j < nr_reloc; j ++)
			{
				struct Elf32_Sym *sym = &symtab[ELF32_R_SYM(rel->r_info)];

				ASLOG(ELF32, "relocate symbol %s shndx %d\n",
						strtab + sym->st_name, sym->st_shndx);
				if ((sym->st_shndx != SHT_NULL) ||
					(ELF_ST_BIND(sym->st_info) == STB_LOCAL) ||
					((ELF_ST_BIND(sym->st_info) == STB_GLOBAL) &&
					 (ELF_ST_TYPE(sym->st_info) == STT_OBJECT)) )
				{

				}
				else
				{
					uint32_t addr;

					ASLOG(ELF32, "relocate symbol: %s\n", strtab + sym->st_name);

				}
				rel ++;
			}
		}
	}

	return r;
}
static ELF32_ObjectType* ELF32_LoadSharedObject(void* elfFile)
{
	ELF32_ObjectType* elfObj = NULL;
	uint32_t vstart_addr, vend_addr;
	uint32_t elf_size;

	if(ELF32_GetVirtualAddress(elfFile, &vstart_addr, &vend_addr))
	{
		elf_size = vend_addr - vstart_addr;

		elfObj = malloc(sizeof(ELF32_ObjectType)+elf_size);
		if(NULL != elfObj)
		{
			elfObj->magic = ELF32_MAGIC;
			elfObj->space = &elfObj[1];
			elfObj->size  = elf_size;
			elfObj->vstart_addr = vstart_addr;
			memset(elfObj->space, 0, elf_size);
			if(FALSE == ELF32_LoadObject(elfFile, elfObj))
			{
				free(elfObj);
				elfObj = NULL;
			}
		}
	}

	return elfObj;
}
/* ============================ [ FUNCTIONS ] ====================================================== */
void* ELF32_Load(void* elfFile)
{
	void* elf = NULL;
	switch(((struct Elf32_Header*)elfFile)->e_type)
	{
		case ET_REL:
			break;
		case ET_EXEC:
			break;
		case ET_DYN:
			elf = ELF32_LoadSharedObject(elfFile);
			break;
		default:
		break;
	}
	return elf;
}
