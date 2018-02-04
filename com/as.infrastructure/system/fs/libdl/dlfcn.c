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
#include "dlfcn.h"
#include "elfloader.h"
#ifdef USE_SHELL
#include "shell.h"
#endif
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
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
	ShellFuncT mentry;
	void* dll = dlopen(argv[1], RTLD_NOW);
	if(NULL != dll)
	{
		mentry = dlsym(dll, "main");
		if(NULL != mentry)
		{
			r = mentry(argc,argv);
		}
		else
		{
			SHELL_printf("no 'main' entry in dll\n");
		}
		dlclose(dll);
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
	(void) flag; /* always RTLD_NOW */

	return ELF_Open(filename);
}

void *dlsym(void *handle, const char *symbol)
{
	return ELF_LookupSymbol(handle, symbol);
}

int dlclose(void *handle)
{
	return ELF_Close(handle);
}

