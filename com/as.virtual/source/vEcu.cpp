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
#include "QDebug"

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
typedef void (*qt_set_param_t)(Ipc_ChannelType chl, void* r_lock, void* r_event, void* w_lock, void* w_event);
typedef void (*qt_get_fifo_t)(Ipc_ChannelType chl, Ipc_FifoType** r_fifo, Ipc_FifoType** w_fifo);
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */

/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
vEcu::vEcu ( QString dll, QObject *parent )
    : QThread(parent)
{
    setlog_t p_setlog;
    qt_set_param_t p_set_param;
    qt_get_fifo_t p_get_fifo;
#ifdef __WINDOWS__
    hxDll = LoadLibrary(dll.toStdString().c_str());
#else
    char* full_path = realpath(dll.toStdString().c_str(),NULL);
    hxDll = dlopen(full_path,RTLD_NOW);
    free(full_path);
#endif
    assert(hxDll);

    sz_fifo = IPC_FIFO_SIZE;

#ifdef __WINDOWS__
    r_lock = CreateMutex( NULL, FALSE, NULL );
    w_lock = CreateMutex( NULL, FALSE, NULL );
    r_event = CreateEvent( NULL, FALSE, FALSE, NULL );
    w_event = CreateEvent( NULL, FALSE, FALSE, NULL );

    pfMain = (PF_MAIN)GetProcAddress(hxDll,"main");
    p_setlog  = (setlog_t)GetProcAddress(hxDll,"aslog_init");
    p_set_param = (qt_set_param_t)GetProcAddress(hxDll,"Qt_SetParam");
    p_get_fifo = (qt_get_fifo_t)GetProcAddress(hxDll,"Qt_GetFifo");
#else
    r_lock = &r_mutex;
    w_lock = &w_mutex;
    r_event = &r_cond;
    w_event = &w_cond;

    pfMain = (PF_MAIN)dlsym(hxDll,"main");
    p_setlog  = (setlog_t)dlsym(hxDll,"aslog_init");
    p_set_param = (qt_set_param_t)dlsym(hxDll,"Qt_SetParam");
    p_get_fifo = (qt_get_fifo_t)dlsym(hxDll,"Qt_GetFifo");
#endif

     assert(pfMain);
     assert(p_setlog);
     assert(p_set_param);
     assert(p_get_fifo);
     p_setlog(dll.toStdString().c_str(),(aslog_t)aslog);

     p_set_param(0,r_lock,r_event,w_lock,w_event);
     p_get_fifo(0,&r_fifo,&w_fifo);
}

vEcu::~vEcu ( )
{
#ifdef __WINDOWS__
    FreeLibrary( hxDll );
    CloseHandle(r_lock);
    CloseHandle(w_lock);
    CloseHandle(r_event);
    CloseHandle(w_event);
    TerminateThread(pvThread,0);
    CloseHandle(pvThread);
#else
    dlclose(hxDll);
#endif
    exit(0);
}

void vEcu::run(void)
{

    VirtQ_IdxType id;
    bool ercd;

#ifdef __WINDOWS__
    HANDLE pvObjectList[ 2 ];
    pvObjectList[ 0 ] = r_lock;
    pvObjectList[ 1 ] = r_event;

    pvThread = CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE ) pfMain, NULL, 0, NULL );
#else
    pthread_create((pthread_t*)&pvThread,NULL,pfMain,NULL);
#endif
    while(true)
    {
#ifdef __WINDOWS__
        WaitForMultipleObjects( sizeof( pvObjectList ) / sizeof( HANDLE ), pvObjectList, TRUE, INFINITE );
#else
        (void)pthread_cond_wait ((pthread_cond_t *)r_event,(pthread_mutex_t *)r_lock);
#endif
        do {
            ercd = fifo_read(&id);
            if(ercd)
            {
                fifo_write(0x1234);
            }
            else
            {
                //qDebug() << "  >> Empty" << "\n";
            }
        }while(ercd);

#ifdef __WINDOWS__
        ReleaseMutex( r_lock );
#else
        (void)pthread_mutex_unlock( (pthread_mutex_t *)r_lock );
#endif
    }
}

bool vEcu::fifo_read(VirtQ_IdxType* id)
{
    bool ercd;
    if(r_fifo->count > 0)
    {
        *id = r_fifo->idx[r_pos];
        r_pos = (r_pos + 1)%(sz_fifo);
        r_fifo->count -= 1;
        ercd = true;
        //aslog("vEcu","fifo_read(%x) fifo=%x lock=%x event=%x\n",*id, r_fifo, r_lock, r_event);
        aslog("vEcu","Incoming message: 0x%X",*id);
    }
    else
    {
        ercd  = false;
    }
    return ercd;
}
bool vEcu::fifo_write(VirtQ_IdxType id)
{
    bool ercd;
#ifdef __WINDOWS__
    WaitForSingleObject(w_lock,INFINITE);
#else
    (void)pthread_mutex_lock((pthread_mutex_t *)w_lock);
#endif
    if(w_fifo->count < sz_fifo)
    {
        w_fifo->idx[w_pos] = id;
        w_pos = (w_pos + 1)%(sz_fifo);
        w_fifo->count += 1;
        ercd = true;
        //aslog("vEcu","fifo_write(%x) fifo=%x lock=%x event=%x\n",id, w_fifo, w_lock, w_event);
        aslog("vEcu","Transmit message: 0x%X",id);
    }
    else
    {
        assert(0);
        ercd = false;
    }
#ifdef __WINDOWS__
    ReleaseMutex(w_lock);
    SetEvent( w_event );
#else
    (void)pthread_mutex_unlock( (pthread_mutex_t *)w_lock );
    (void)pthread_cond_signal ((pthread_cond_t *)w_event);
#endif
    return ercd;
}

void aslog(const char* who,const char* log,...)
{
    static char* buf = NULL;
    static char* name = NULL;
    va_list args;

    va_start(args , log);
    if(NULL == buf)
    {
        buf = (char*)malloc(1024);
        name = (char*)malloc(256);
        assert(buf);
        assert(name);
    }
    vsprintf(buf,log,args);
    sprintf(name,"%-16s",who);

    int length = strlen(buf);
    if('\n'==buf[length-1])
    {
        buf[length-1] = '\0';   /* drop end line */
    }
    qDebug() << name << "::" << buf;

    va_end(args);
}
