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
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
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

    char* cwd = getcwd(NULL,0);
    ASLOG(OFF,cwd);
    chdir("../../out");
    char* workpath = getcwd(NULL,0);

    free(cwd);
    free(workpath);

    DIR* d = opendir(".");
    struct dirent *file;
    while((file = readdir(d)) != NULL)
    {
        if(strstr(file->d_name,".dll"))
        {
            ASLOG(OFF,"load %s\n",file->d_name);
            action = new vmAction(QString(file->d_name),this);
            toolbar->addAction(action);
        }
    }
    closedir(d);

    setGeometry(50,50,600,20);
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

vmWindow::~vmWindow()
{

}
