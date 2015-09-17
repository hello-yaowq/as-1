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
#ifndef RELEASE_ASCORE_VIRTUAL_INCLUDE_VMWINDOW_H_
#define RELEASE_ASCORE_VIRTUAL_INCLUDE_VMWINDOW_H_
/* ============================ [ INCLUDES  ] ====================================================== */
#include <QMainWindow>
#include <QToolBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include "vEcu.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
class vmAction : public QAction
{
Q_OBJECT
private:
    QString dll_name;
    vEcu* ecu;
public:
    explicit vmAction(QString dll,QWidget* parent = 0);
private slots:
    void start(void);

};
class vmWindow : public QMainWindow
{
Q_OBJECT
private:
    QToolBar* toolbar;
    QAction* startEcuAction[];
public:
    explicit vmWindow(QWidget* parent = 0);
    ~vmWindow();
private slots:
    void save ( void );
    void open ( void );
    void close( void );
};
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
#endif /* RELEASE_ASCORE_VIRTUAL_INCLUDE_VMWINDOW_H_ */
