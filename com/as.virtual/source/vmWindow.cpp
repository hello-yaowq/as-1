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
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
vmWindow::vmWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Virtual Machine for AUTOSAR(parai@foxmail.com)");

    QAction * action = NULL;
    toolbar = addToolBar("Console");
    QMenu* menubar = menuBar()->addMenu(tr("File"));

    action = new QAction(tr("&Open"),this);
    action->setShortcut(tr("Ctrl+O"));
    connect(action,SIGNAL(triggered()),this,SLOT(open()));
    menubar->addAction(action);

    action = new QAction(tr("&Save"),this);
    action->setShortcut(tr("Ctrl+S"));
    connect(action,SIGNAL(triggered()),this,SLOT(save()));
    menubar->addAction(action);

    action = new QAction(tr("&Exit"),this);
    action->setShortcut(tr("Ctrl+Q"));
    connect(action,SIGNAL(triggered()),this,SLOT(close()));
    menubar->addAction(action);

    startEcu1Action = new QAction(tr("&Start Ecu1"),this);
    connect(startEcu1Action,SIGNAL(triggered()),this,SLOT(start_ecu1()));
    toolbar->addAction(startEcu1Action);

    setGeometry(25,30,600,20);
}
void vmWindow::save ( void )
{

}

void vmWindow::open ( void )
{

}

void vmWindow::close( void )
{
}
void vmWindow::start_ecu1(void)
{
    ecu1 = new vEcu(VIRTUAL_ECU1);
    ecu1->start();
    startEcu1Action->setDisabled(true);
}

vmWindow::~vmWindow()
{

}
