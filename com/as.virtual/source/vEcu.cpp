/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2015  AS <parai@foxmail.com>
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
#include "vEcu.h"
#include "asdebug.h"
#include <QDebug>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <stdarg.h>
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
typedef void (*aslog_t)(const char*,const char*,...);
typedef void (*setlog_t)(const char*,aslog_t);
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */

/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
vEcu::vEcu ( QString dll, QObject *parent )
    : QThread(parent),name(dll)
{
    setlog_t p_setlog;
#ifdef __WINDOWS__
    hxDll = LoadLibrary(dll.toStdString().c_str());
#else
    char* full_path = realpath(dll.toStdString().c_str(),NULL);
    hxDll = dlopen(full_path,RTLD_NOW);
    free(full_path);
#endif
    assert(hxDll);

#ifdef __WINDOWS__
    pfMain = (PF_MAIN)GetProcAddress(hxDll,"main");
    p_setlog  = (setlog_t)GetProcAddress(hxDll,"aslog_init");
#else

    pfMain = (PF_MAIN)dlsym(hxDll,"main");
    p_setlog  = (setlog_t)dlsym(hxDll,"aslog_init");
#endif

     assert(pfMain);
     assert(p_setlog);
     p_setlog(dll.toStdString().c_str(),(aslog_t)aslog);

     virtio = new Virtio(hxDll,this);
}

vEcu::~vEcu ( )
{
#ifdef __WINDOWS__
    FreeLibrary( hxDll );
    TerminateThread(pvThread,0);
    CloseHandle(pvThread);
#else
    dlclose(hxDll);
#endif
    delete virtio;
    exit(0);
}

void vEcu::run(void)
{
    ASLOG(VECU,"starting the Ecu<%s>\n",name.toStdString().c_str());
    virtio->start();
    pfMain();
}
