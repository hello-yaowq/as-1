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
#include "Virtio.h"

/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
typedef void (*qt_set_param_t)(Ipc_ChannelType chl, void* r_lock, void* r_event, void* w_lock, void* w_event);
typedef void (*qt_get_fifo_t)(Ipc_ChannelType chl, Ipc_FifoType** r_fifo, Ipc_FifoType** w_fifo);
typedef Rproc_ResourceTableType* (*qt_get_rproc_resource_table_t)(void);
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
Virtio::Virtio ( void* dll, QObject *parent)  : QThread(parent)
{
    qt_set_param_t p_set_param;
    qt_get_fifo_t p_get_fifo;
    qt_get_rproc_resource_table_t p_get_rsc_tbl;

    hxDll = dll;

    chl = 0;    /* now only support 1 channel */

    sz_fifo = IPC_FIFO_SIZE;

#ifdef __WINDOWS__
    r_lock = CreateMutex( NULL, FALSE, NULL );
    w_lock = CreateMutex( NULL, FALSE, NULL );
    r_event = CreateEvent( NULL, FALSE, FALSE, NULL );
    w_event = CreateEvent( NULL, FALSE, FALSE, NULL );

    p_set_param = (qt_set_param_t)GetProcAddress((HMODULE)hxDll,"Qt_SetIpcParam");
    p_get_fifo = (qt_get_fifo_t)GetProcAddress((HMODULE)hxDll,"Qt_GetIpcFifo");
    p_get_rsc_tbl = (qt_get_rproc_resource_table_t)GetProcAddress((HMODULE)hxDll,"Qt_GetRprocResourceTable");
    pfIsIpcReady = (PF_IPC_IS_READY)GetProcAddress((HMODULE)hxDll,"Ipc_IsReady");
#else
    r_lock = &r_mutex;
    w_lock = &w_mutex;
    r_event = &r_cond;
    w_event = &w_cond;

    p_set_param = (qt_set_param_t)dlsym(hxDll,"Qt_SetIpcParam");
    p_get_fifo = (qt_get_fifo_t)dlsym(hxDll,"Qt_GetIpcFifo");
    p_get_rsc_tbl = (qt_get_rproc_resource_table_t)dlsym(hxDll,"Qt_GetRprocResourceTable");
    pfIsIpcReady = (PF_IPC_IS_READY)dlsym(hxDll,"Ipc_IsReady");
#endif

     assert(p_set_param);
     assert(p_get_fifo);
     assert(p_get_rsc_tbl);

     p_set_param(0,r_lock,r_event,w_lock,w_event);
     p_get_fifo(0,&r_fifo,&w_fifo);

     r_pos=0;
     w_pos=0;
     rsc_tbl = p_get_rsc_tbl();

     rsc_tbl->rpmsg_vdev.gfeatures = rsc_tbl->rpmsg_vdev.dfeatures;
     ASLOG(VIRTIO,"r_lock=%08X, w_lock=%08X, r_event=%08X, w_event=%08X, r_fifo=%08X, w_fifo=%08X\n",
           r_lock,w_lock,r_event,w_event,r_fifo,w_fifo);
}

Virtio::~Virtio ( )
{
#ifdef __WINDOWS__
    CloseHandle(r_lock);
    CloseHandle(w_lock);
    CloseHandle(r_event);
    CloseHandle(w_event);
#endif
}
void Virtio::run ( void )
{
    VirtQ_IdxType idx;
    bool ercd;

#ifdef __WINDOWS__
    HANDLE pvObjectList[ 2 ];
    pvObjectList[ 0 ] = r_lock;
    pvObjectList[ 1 ] = r_event;
#endif
    while(false==pfIsIpcReady(chl))
    {
        sleep(1);   /* make sure ecu ready */
    }
    RPmsg* rpmsg = new  RPmsg(&rsc_tbl->rpmsg_vdev);
    connect(rpmsg,SIGNAL(kick(unsigned int)),this,SLOT(kick(unsigned int)));
    vdev_list.append(rpmsg);
    rpmsg->start();

    while(true)
    {
#ifdef __WINDOWS__
        WaitForMultipleObjects( sizeof( pvObjectList ) / sizeof( HANDLE ), pvObjectList, TRUE, INFINITE );
#else
        (void)pthread_cond_wait ((pthread_cond_t *)r_event,(pthread_mutex_t *)r_lock);
#endif
        do {
            ercd = fifo_read(&idx);
            if(ercd)
            {
                for(int i=0;i<vdev_list.size();i++)
                {
                    Vdev* vdev = vdev_list[i];
                    if(vdev->notify(idx))
                    {
                        break;
                    }
                }
            }
            else
            {
            }
        }while(ercd);

#ifdef __WINDOWS__
        ReleaseMutex( r_lock );
#else
        (void)pthread_mutex_unlock( (pthread_mutex_t *)r_lock );
#endif
    }
}
void Virtio::kick(unsigned int idx)
{
    fifo_write(idx);
}

bool Virtio::fifo_read(VirtQ_IdxType* id)
{
    bool ercd;
    if(r_fifo->count > 0)
    {
        *id = r_fifo->idx[r_pos];
        ASLOG(VIRTIO,"Incoming message: 0x%X,pos=%d,count=%d\n",*id,r_pos,r_fifo->count);
        r_pos = (r_pos + 1)%(sz_fifo);
        r_fifo->count -= 1;
        ercd = true;
    }
    else
    {
        ercd  = false;
    }
    return ercd;
}
bool Virtio::fifo_write(VirtQ_IdxType id)
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
        w_fifo->count += 1;
         ASLOG(VIRTIO,"Transmit message: 0x%X,pos=%d,count=%d\n",id,w_pos,w_fifo->count);
        w_pos = (w_pos + 1)%(sz_fifo);
        ercd = true;
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
