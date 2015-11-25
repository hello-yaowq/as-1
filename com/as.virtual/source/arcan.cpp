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
/* ============================ [ INCLUDES  ] ====================================================== */
#include "arcan.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <sys/time.h>
#include <assert.h>
#include <entry.h>
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
TickType GetOsTick(void)
{
    return (TickType)clock();
}
TickType  GetOsElapsedTick  ( TickType prevTick )
{
    if ((TickType)clock() >= prevTick) {
        return((TickType)clock() - prevTick);
    }
    else {
        return(prevTick - (TickType)clock() + (TICK_MAX + 1));
    }
}

/* ============================ [ FUNCTIONS ] ====================================================== */
arCan::arCan(QString name,unsigned long channelNumber, QWidget *parent) : arDevice(name,parent)
{
    this->channelNumber = channelNumber;
    this->createGui();
    this->setGeometry(50,150,1200,500);

    setVisible(true);
}

arCan::~arCan()
{
}

void arCan::on_btnClearTrace_clicked(void)
{
    QStringList  list;
    tableTrace->clear();
    list<<"from"<<"Bus"<<"Time(ms)"<<"Dir"<<"Id"<<"dlc"<<"B0"<<"B1"<< "B2"<< "B3"<< "B4"<< "B5"<< "B6"<< "B7";
    tableTrace->setColumnCount(list.size());
    tableTrace->setRowCount(0);
    tableTrace->setHorizontalHeaderLabels(list);
    tableTrace->setColumnWidth(0,150);
    tableTrace->setColumnWidth(1,60);
    tableTrace->setColumnWidth(2,150);
    tableTrace->setColumnWidth(3,60);
    tableTrace->setColumnWidth(4,80);
    for(int i=5;i<list.size();i++)
    {
        tableTrace->setColumnWidth(i,60);
    }
    tableTrace->setRowCount(0);
    tableTrace->setHorizontalHeaderLabels(list);
}

void arCan::on_btnSaveTrace_clicked(void)
{
    QString filename = QFileDialog::getSaveFileName(this,"Save CAN Trace",".",tr("CAN Trace (*.rec *.txt)"));
    QFile file(filename);

    if(file.open(QFile::WriteOnly))
    {
        //file.write()

    	file.close();
    }
}
void arCan::on_btnHexlDeci_clicked(void)
{
    QString str = btnHexlDeci->text();

    if(str == "Hexl")
    {
        btnHexlDeci->setText("Decimal");
    }
    else
    {
        btnHexlDeci->setText("Hexl");
    }
}

void arCan::on_btnAbsRelTime_clicked(void)
{
    QString str = btnAbsRelTime->text();

    if(str == "Real Time")
    {
        btnAbsRelTime->setText("Absolute Time");
    }
    else
    {
        btnAbsRelTime->setText("Real Time");
    }
}
void arCan::putMsg(QString from,quint8 busid,quint32 canid,quint8 dlc,quint8* data,bool isRx)
{
    quint32 index = tableTrace->rowCount();
    tableTrace->setRowCount(index+1);
    tableTrace->setItem(index,0,new QTableWidgetItem(from));
    tableTrace->setItem(index,1,new QTableWidgetItem(QString("%1").arg(busid)));
    tableTrace->setItem(index,2,new QTableWidgetItem(QString("%1").arg(GetOsTick())));
    if(isRx)
    {
        tableTrace->setItem(index,3,new QTableWidgetItem(QString("RX")));
    }
    else
    {
        tableTrace->setItem(index,3,new QTableWidgetItem(QString("TX")));
    }
    tableTrace->setItem(index,4,new QTableWidgetItem(QString("%1").arg(canid,0,16)));
    tableTrace->setItem(index,5,new QTableWidgetItem(QString("%1").arg(dlc,0,16)));
    for(int i=0;i<8;i++)
    {
        tableTrace->setItem(index,6+i,new QTableWidgetItem(QString("%1").arg((uint)data[i],2,16,QLatin1Char('0')).toUpper()));
    }
    tableTrace->setCurrentCell(index,0);
}

void arCan::RxIndication(QString from,quint8 busid,quint32 canid,quint8 dlc,quint8* data)
{
    putMsg(from,busid,canid,dlc,data);
}

void arCan::Transmit(quint8 busid,quint32 canid,quint8 dlc,quint8* data)
{
    (void)time;
    putMsg("Qt",busid,canid,dlc,data,false);

    Entry::Self()->Can_Write(busid,canid,dlc,data);
}

void arCan::on_btnTriggerTx_clicked(void)
{
    bool ok;
    quint32 id = leId->text().toLong(&ok,16);
    if(ok)
    {
        int i;
        quint8 data[8];
        quint32 bus = cbBus->currentIndex();
        QStringList sdata = leData->text().split(" ",QString::SkipEmptyParts);
        ok = true;
        if((sdata.size()>0) && (sdata.size()<=8))
        {
            for(i=0;i<sdata.size();i++)
            {
                bool iOk;
                data[i] = sdata[i].toLong(&iOk,16);
                if(!iOk)
                {
                    ok = false;
                }
            }
            for(;i<8;i++)
            {
                data[i] = 0x55;
            }
        }
        else
        {
            ok = false;
        }

        if(ok)
        {
            this->Transmit(bus,id,8,data);
        }
        else
        {
            QMessageBox::warning(this,"Invalid Data",QString("Invalid Data: \" %1 \"" ).arg(leData->text()));
        }
    }
    else
    {
        QMessageBox::warning(this,"Invalid Id",QString("Invalid Id: \" %1 \"").arg(leId->text()));
    }
}

void arCan::createGui(void)
{
    QWidget* widget= new QWidget(this);
    QVBoxLayout* vbox = new QVBoxLayout();

    {   // create trace
        tableTrace = new QTableWidget();
        QStringList  list;
        list<<"from"<<"Bus"<<"Time(ms)"<<"Dir"<<"Id"<<"dlc"<<"B0"<<"B1"<< "B2"<< "B3"<< "B4"<< "B5"<< "B6"<< "B7";
        tableTrace->setColumnCount(list.size());
        tableTrace->setRowCount(0);
        tableTrace->setHorizontalHeaderLabels(list);
        tableTrace->setColumnWidth(0,150);
        tableTrace->setColumnWidth(1,60);
        tableTrace->setColumnWidth(2,150);
        tableTrace->setColumnWidth(3,60);
        tableTrace->setColumnWidth(4,80);
        for(int i=5;i<list.size();i++)
        {
            tableTrace->setColumnWidth(i,60);
        }
        tableTrace->verticalHeader()->setResizeContentsPrecision(QHeaderView::ResizeToContents);
        vbox->addWidget(tableTrace);
    }

    {   // create control
        QPushButton* button;
        QHBoxLayout* hbox = new QHBoxLayout();

        button = new QPushButton("Clear Trace");
        this->connect(button,SIGNAL(clicked()),this,SLOT(on_btnClearTrace_clicked()));
        hbox->addWidget(button);

        button = new QPushButton("Save Trace");
        this->connect(button,SIGNAL(clicked()),this,SLOT(on_btnSaveTrace_clicked()));
        hbox->addWidget(button);

        btnHexlDeci = new QPushButton("Hexl");
        this->connect(btnHexlDeci,SIGNAL(clicked()),this,SLOT(on_btnHexlDeci_clicked()));
        hbox->addWidget(btnHexlDeci);
        btnHexlDeci->setDisabled(true); // TODO

        btnAbsRelTime = new QPushButton("Real Time");
        this->connect(btnAbsRelTime,SIGNAL(clicked()),this,SLOT(on_btnAbsRelTime_clicked()));
        hbox->addWidget(btnAbsRelTime);
        btnAbsRelTime->setDisabled(true);   // TODO

        hbox->setSpacing(20);

        vbox->addItem(hbox);
    }
    // automatic send
    {
        QHBoxLayout* hbox = new QHBoxLayout();
        QLabel *label = new QLabel("Bus:");
        QPushButton *button;
        hbox->addWidget(label);
        cbBus = new QComboBox();
        for(unsigned long i=0;i<channelNumber;i++)
        {
            cbBus->addItem(QString("%1").arg(i));
        }
        cbBus->setCurrentIndex(0);
        hbox->addWidget(cbBus);

        label = new QLabel("Id:");
        hbox->addWidget(label);
        leId = new QLineEdit();
        hbox->addWidget(leId);
        leId->setMaximumWidth(50);
        leId->setText("731");

        label = new QLabel("Data:");
        hbox->addWidget(label);
        leData = new QLineEdit();
        hbox->addWidget(leData);
        leData->setText(("55 55 55 55 55 55 55 55"));

        button = new QPushButton("Trigger Tx");
        connect(button,SIGNAL(clicked()),this,SLOT(on_btnTriggerTx_clicked()));
        hbox->addWidget(button);

        hbox->setSpacing(5);

        vbox->addItem(hbox);
    }

    widget->setLayout(vbox);
    this->setCentralWidget(widget);
}

