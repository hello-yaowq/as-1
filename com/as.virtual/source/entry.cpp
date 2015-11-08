/**
 * WhatsApp - the open source AUTOSAR platform https://github.com/parai
 *
 * Copyright (C) 2014  WhatsApp <parai@foxmail.com>
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
#include "entry.h"
#include "arcan.h"
#include "vmWindow.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
static class Entry* self = NULL;

Entry::Entry ( QWidget *parent )
		: QMainWindow(parent)
{
	self = this;
	this->setWindowTitle(tr("WhatsApp ( parai@foxmail.com )"));
	this->createMenuAndToolbar();

    this->setGeometry(50,50,600,20);

    registerDevice(new arCan(CAN_DEVICE_NAME,CAN_CTRL_NUM));
}

class Entry* Entry::Self ( void )
{
	return self;
}
Entry::~Entry ( )
{
	map_device.clear(); // this in charge to destroy VirtualDevice
}

void Entry::registerDevice ( arDevice* device )
{
	if ( map_device.contains(device->Name()) )
	{
		qDebug() << "System error: Device " << device->Name() << " re-registeration!\n";
		delete device;
	}
	else
	{
		map_device[device->Name()] = device;
		QAction * action = new QAction(device->Name(),this);
		this->connect(action,SIGNAL(triggered()),device,SLOT(wakeup()));
		menuVD->addAction(action);
	}
}
void Entry::deleteDevice ( QString name )
{
	if ( map_device.contains(name) )
	{
		arDevice* device = map_device.take(name);
		delete device;
		/* menu virtual device re-build */
		menuVD->clear();
		QList<arDevice*> devices = 	map_device.values();
		for(int i=0;i<devices.size();i++)
		{
			device = devices[i];
			QAction * action = new QAction(device->Name(),this);
			this->connect(action,SIGNAL(triggered()),device,SLOT(wakeup()));
			menuVD->addAction(action);
		}
	}
	else
	{
		qDebug() << "System error: Virtual device " << name  << " delete!\n";
	}
}

arDevice* Entry::getDevice ( QString name )
{
	if ( map_device.contains(name) )
	{
		return map_device[name];
	}
	else
	{
		return NULL;
	}
}

// ==================== [ SIGNALS       ] =====================================

// ==================== [ PRIVATE SLOTS ] ======================================
void Entry::open ( void )
{

}

void Entry::save ( void )
{

}

// ==================== [ PRIVATE FUNCTIONS ] ==================================
void Entry::createMenuAndToolbar ( void )
{
    QAction * action = NULL;
    QToolBar* toolbar = this->addToolBar("virtual machine");

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

	this->menuBSW = this->menuBar()->addMenu(tr("BSW"));

	this->menuVD = menuBSW->addMenu(tr("Device"));
}
