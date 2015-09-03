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
#include "Std_Types.h"
#include <windows.h>
#include <QThread>
#include <QString>
#include <assert.h>
/* ============================ [ MACROS    ] ====================================================== */
#define VIRTUAL_ECU1 "D:/repository/parai/as/release/ascore/out/mingw.dll"
/* ============================ [ TYPES     ] ====================================================== */
enum rp_fifo_messages {
    RP_FIFO_READY		= 0xFFFFFF00,
    RP_FIFO_PENDING_MSG	= 0xFFFFFF01,
    RP_FIFO_CRASH		= 0xFFFFFF02,
    RP_FIFO_ECHO_REQUEST	= 0xFFFFFF03,
    RP_FIFO_ECHO_REPLY	= 0xFFFFFF04,
    RP_FIFO_ABORT_REQUEST	= 0xFFFFFF05,
};

struct rsc_fifo {
    uint32 count;
    uint32 size;	/* size of identifier in u32 */
    uint32 r_pos;
    uint32 w_pos;
    uint32 identifier[0];
} __attribute__((__packed__));

typedef void (*PF_MAIN)(void);
typedef bool (*PF_RPROC_INIT)(void* address, size_t size,HANDLE r_lock,HANDLE w_lock,HANDLE r_event, HANDLE w_event,size_t sz_fifo);
class vEcu: public QThread
{
Q_OBJECT
    private:
    HMODULE hxDll;
    void* rsc_tbl_address;
    size_t rsc_tbl_size;
    size_t sz_fifo;
    HANDLE  r_lock;
    HANDLE  w_lock;
    HANDLE  r_event;
    HANDLE  w_event;
    HANDLE pvThread;
    PF_MAIN pfMain;
    PF_RPROC_INIT pfRprocInit;

    struct rsc_fifo* r_fifo;
    struct rsc_fifo* w_fifo;
public:
    explicit vEcu ( QString dll, QObject *parent = 0);
    ~vEcu ( );

    void run(void);

    bool fifo_read(uint32* id);
signals:

protected:

private slots:

};
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
#endif /* RELEASE_ASCORE_VIRTUAL_INCLUDE_VECU_H_ */
