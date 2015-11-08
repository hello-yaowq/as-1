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
#include "vmWindow.h"
#include "vEcu.h"
#include <QDebug>
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
vmAction::vmAction(QString dll,QWidget* parent)
    :QAction(dll,parent), dll_name(dll)
{
    connect(this,SIGNAL(triggered()),this,SLOT(start()));
}

void vmAction::start(void)
{
    ASLOG(OFF,"Start ECU<%s>\n",dll_name.toStdString().c_str());
    ecu = new vEcu(dll_name);
    ecu->start();
    setDisabled(true);
}
