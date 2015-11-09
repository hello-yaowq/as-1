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
#ifndef ARCAN_H
#define ARCAN_H
/* ============================ [ INCLUDES  ] ====================================================== */
#include "ardevice.h"
#include "ocdevice.h"
#ifdef __WINDOWS__
typedef void* HANDLE;
#define POINTER_32
#include "vxlapi.h"
#endif
/* ============================ [ MACROS    ] ====================================================== */
#define TICK_MAX (TickType)-1
#define CAN_DEVICE_NAME   "Can"
#define CAN_CTRL_NUM   1
/* ============================ [ TYPES     ] ====================================================== */
typedef quint32 TickType;
/* ============================ [ CLASS     ] ====================================================== */
class arCanBus : public OcDevice
{
    Q_OBJECT
private:
    unsigned long canCardId;
    QList<OcMessage*> rxMsgList;
    QList<OcMessage*> txMsgList;
#ifdef __WINDOWS__
    XLportHandle xlPortHandle;
    XLaccess     xlAccess;
#endif
    TickType      osTick;
    TickType      prevMsgTimeStamp;
public:
    explicit arCanBus(unsigned long canCardId);
    ~arCanBus();
    void clear(void);
private:

public:
    OcStatus startup();
    OcStatus shutdown();
    OcStatus sendMessage(const OcMessage &msg);
    OcStatus receivedMessage();
    OcStatus internalGetMessage(OcMessage *msg);
    int getBaudRate();
    OcStatus setBaudRate(int baud);
    /* if atFirst true, means execute it as soon as possible */
    void registerRxMsg(OcMessage *msg,bool atFirst=false);
    void registerTxMsg(OcMessage *msg,bool atFirst=false);
};

class arCan : public arDevice
{
    Q_OBJECT
private:
    unsigned long channelNumber;
    QList<arCanBus*> canBusList;
    QPushButton* btnPlayPause;
    QPushButton* btnHexlDeci;
    QPushButton* btnAbsRelTime;
    QPushButton* btnLoadTrace;
    QPushButton* btnStop;
    QComboBox*   cbBus;
    QLineEdit*   leId;
    QLineEdit*   leData;
    QTableWidget* tableTrace;
    QList<OcMessage*> swMsg;
public:
    explicit arCan(QString name,unsigned long channelNumber,QWidget *parent=0);
    ~arCan();
    static class arCan* Self ( void );
    void WriteMessage(OcMessage *msg);
    void ReceiveMessage(OcMessage *msg);
private slots:
    void on_btnPlayPause_clicked(void);
    void on_btnStop_clicked(void);
    void on_btnClearTrace_clicked(void);
    void on_btnSaveTrace_clicked(void);
    void on_btnLoadTrace_clicked(void);
    void on_btnHexlDeci_clicked(void);
    void on_btnAbsRelTime_clicked(void);
    void on_messageReceived(OcMessage *, const QTime &);
    void on_btnTriggerTx_clicked(void);
private:
    void createGui(void);
    OcMessage* entry2msg(QRegularExpressionMatch match);
    void putMsg(OcMessage*,bool isRx=true);
    void clear(void);
signals:
    void messageReceived(OcMessage *);
};
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
TickType GetOsTick(void);
TickType  GetOsElapsedTick  ( TickType prevTick );
#endif /* ARCAN_H */
