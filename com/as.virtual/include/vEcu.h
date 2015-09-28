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
#ifndef RELEASE_ASCORE_VIRTUAL_INCLUDE_VECU_H_
#define RELEASE_ASCORE_VIRTUAL_INCLUDE_VECU_H_
/* ============================ [ INCLUDES  ] ====================================================== */
#include <stdint.h>
#ifdef __WINDOWS__
#include <windows.h>
#else
#include <pthread.h>
#include <dlfcn.h>
#endif
#include <QThread>
#include <QString>
#include <QDebug>
#include <assert.h>
/* ============================ [ MACROS    ] ====================================================== */
#define IPC_FIFO_SIZE 1024
/* ============================ [ TYPES     ] ====================================================== */
typedef uint8_t Ipc_ChannelType;
typedef uint16_t VirtQ_IdxType;
typedef uint16_t VirtQ_IdxSizeType;
typedef struct
{
    VirtQ_IdxSizeType count;
    VirtQ_IdxType     idx[IPC_FIFO_SIZE];
}Ipc_FifoType;

typedef void* (*PF_MAIN)(void*);

class vEcu: public QThread
{
Q_OBJECT
    private:
    void* hxDll;
    void* rsc_tbl_address;
    size_t rsc_tbl_size;
    size_t sz_fifo;
    void*  r_lock;
    void*  w_lock;
    void*  r_event;
    void*  w_event;
    void*  pvThread;
    PF_MAIN pfMain;

    Ipc_FifoType* r_fifo;
    Ipc_FifoType* w_fifo;
    uint32_t r_pos;
    uint32_t w_pos;

#ifdef __LINUX__
    pthread_mutex_t w_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t r_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t  w_cond  = PTHREAD_COND_INITIALIZER;
    pthread_cond_t  r_cond  = PTHREAD_COND_INITIALIZER;
#endif
public:
    explicit vEcu ( QString dll, QObject *parent = 0);
    ~vEcu ( );

    void run(void);

    bool fifo_read(VirtQ_IdxType* id);
    bool fifo_write(VirtQ_IdxType id);
signals:

protected:

private slots:

};
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void aslog(const char* who,const char* log,...);

#endif /* RELEASE_ASCORE_VIRTUAL_INCLUDE_VECU_H_ */
