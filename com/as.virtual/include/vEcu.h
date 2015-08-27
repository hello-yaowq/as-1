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
typedef void (*PF_MAIN)(void);
typedef bool (*PF_RPROC_INIT)(void* address, size_t size,HANDLE r_lock,HANDLE w_lock,HANDLE r_event, HANDLE w_event);
class vEcu: public QThread
{
Q_OBJECT
    private:
    HMODULE hxDll;
    void* rsc_tbl_address;
    size_t rsc_tbl_size;
    HANDLE  r_lock;
    HANDLE  w_lock;
    HANDLE  r_event;
    HANDLE  w_event;
    HANDLE pvThread;
    PF_MAIN pfMain;
    PF_RPROC_INIT pfRprocInit;
public:
    explicit vEcu ( QString dll, QObject *parent = 0);
    ~vEcu ( );

    void run(void);
signals:

protected:

private slots:

};
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
#endif /* RELEASE_ASCORE_VIRTUAL_INCLUDE_VECU_H_ */
