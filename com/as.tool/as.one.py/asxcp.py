__lic__ = '''
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
 '''
from PyQt5 import QtCore, QtGui
from PyQt5.QtGui import *
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from serial import Serial
import time
from binascii import hexlify, unhexlify
import sys,os
import xml.etree.ElementTree as ET
from pyas.dcm import *
import traceback
import re

from pyas.can import *

from bitarray import bitarray

__all__ = ['UIXcp']

__canbus__   = 0
__tx_canid__ = 0x554
__rx_canid__ = 0x555

# 0 = little endian; 1 = big endian
xcp_cpu_endian = 1


__last_response = None

__error_code = {
    0x00:'XCP_ERR_CMD_SYNCH',           0x10:'XCP_ERR_CMD_BUSY',
    0x11:'XCP_ERR_DAQ_ACTIVE',          0x12:'XCP_ERR_PGM_ACTIVE',

    0x20:'XCP_ERR_CMD_UNKNOWN',         0x21:'XCP_ERR_CMD_SYNTAX',
    0x22:'XCP_ERR_OUT_OF_RANGE',        0x23:'XCP_ERR_WRITE_PROTECTED',
    0x24:'XCP_ERR_ACCESS_DENIED',       0x25:'XCP_ERR_ACCESS_LOCKED',
    0x26:'XCP_ERR_PAGE_NOT_VALID',      0x27:'XCP_ERR_MODE_NOT_VALID',
    0x28:'XCP_ERR_SEGMENT_NOT_VALID',   0x29:'XCP_ERR_SEQUENCE',
    0x2A:'XCP_ERR_DAQ_CONFIG',

    0x30:'XCP_ERR_MEMORY_OVERFLOW',     0x31:'XCP_ERR_GENERIC',
    0x32:'XCP_ERR_VERIFY'
}

def __show_request__(req):
    ss = "  >> xcp request = ["
    length = len(req)
    for i in range(length):
        ss += '%02X,'%(req[i])
    ss+=']'
    print(ss)

def __show_response__(res):
    ss = "  >> xcp response = ["
    length = len(res)
    for i in range(length):
        ss += '%02X,'%(res[i])
    ss+=']'
    print(ss)

def Xcp_PollDAQMessage():
    if(__last_response==None):return
    result,canid,data= can_read(__canbus__,__rx_canid__)
    if((True == result) and (__rx_canid__ == canid)):
        ss = "  >> xcp DAQ response = ["
        length = len(data)
        for i in range(length):
            ss += '%02X,'%(data[i])
        ss+=']'
        print(ss)

def Xcp_TransmitMessage(req):
    global __last_response
    __show_request__(req)
    can_write(__canbus__,__tx_canid__,req)
    ercd = False
    data=None
    pre = time.time()
    while ( ((time.time() - pre) < 1) and (ercd == False)): # 1s timeout
        result,canid,data= can_read(__canbus__,__rx_canid__)
        if((True == result) and (__rx_canid__ == canid)):
            ercd = True
            break
        else:
            time.sleep(0.001) # sleep 1 ms
    
    if (False == ercd):
        print("XCP timeout when receiving a frame! elapsed time = %s ms"%(time.time() -pre))
        return None
  
    __last_response = xcpbits()
    for d in data:
        __last_response.append(d, 8)
    __show_response__(__last_response.toarray())
    return __last_response

def Xcp_GetLastError():
    ercd = __last_response.toarray()[1]
    ss = Xcp_GetResponse()
    try:
        ss += ' %s'%(__error_code[ercd])
    except KeyError:
        ss += ' unknown error code %s'%(ercd)
    return ss

def Xcp_GetResponse():
    data = __last_response.toarray()
    ss = "["
    length = len(data)
    for i in range(length):
        ss += '%02X,'%(data[i])
    ss+=']'
    return ss

def str2int(sstr):
    if(sstr[:2].lower() == '0x'):
        return int(sstr,16)
    elif(sstr[:2].lower() == '0b'):
        return int(sstr,2)
    else:
        return int(sstr,10)

class xcpbits():
    global xcp_cpu_endian
    def __init__(self):
        self.bits = bitarray()

    def append(self,d, num=8):
        assert(num%8 == 0)
        if(xcp_cpu_endian == 1):  # big endian
            for i in range(num):
                if((d&(1<<(num-1-i))) != 0):
                    self.bits.append(True)
                else:
                    self.bits.append(False)
        else:       # little endian
            bitmap = [ 7, 6, 5, 4, 3, 2, 1, 0,
                      15,14,13,12,11,10, 9, 8,
                      23,22,21,20,19,18,17,16,
                      31,30,29,28,27,26,25,24]
            for i in range(num):
                if((d&(1<<(bitmap[i]))) != 0):
                    self.bits.append(True)
                else:
                    self.bits.append(False)

    def toint(self,pos,num):
        bits = self.bits[pos:pos+num]
        bytes = int((num+4)/8)
        left = bytes*8 - num
        v = 0
        for b in bits.tobytes():
            v = (v<<8) + b
        v =v >> left
        if(xcp_cpu_endian == 0):  # little endian
            bytes = []
            while(v > 0):
                bytes.append(v&0xFF)
                v = v>>8
            for b in bytes:
                 v = (v<<8) + b
        return v
        
    def toarray(self):
        return self.bits.tobytes()


class wDataUS(QComboBox):
    '''Data UxxSelect, 0<xx<=32'''
    def __init__(self,xml,parent=None):
        super(QComboBox, self).__init__(parent)
        self.Data = xml
        self.OptionInvalid= 0
        list = []
        for select in xml:
            list.append(select.attrib['name'])
            self.OptionInvalid += 1
        list.append('Invalid')
        self.addItems(list)
        
        try:
            default = str2int(xml.attrib['default'])
            self.setCurrentIndex(default)
        except:
            pass
        
    def getValue(self,data):
        index = self.currentIndex()
        
        si = 0
        svalue = None
        for select in self.Data:
            if(index == si):
                svalue = select.attrib['value']
                break
            else:
                si = si + 1
        assert(svalue)

        d = str2int(svalue)
        a = xcpbits()
        num = str2int(self.Data.attrib['type'][1:-6])
        data.append(d, num)
    
    def setValue(self,data,start):
        try:
            num = str2int(self.Data.attrib['type'][1:-6])
            value = data.toint(start,num)
            start += num
            
        except IndexError:
            QMessageBox(QMessageBox.Critical, 'Error', 'Data record witn Invalid Length  %s.'%(Xcp_GetResponse())).exec_();
            return
        index = 0
        for select in self.Data:
            if(str2int(select.attrib['value']) == value):
                break
            else:
                index += 1
        self.setCurrentIndex(index)
        return start
    
class wDataU(QLineEdit):
    '''Data Uxx UxxArray 0<xx<=32'''
    def __init__(self,xml,parent=None):
        super(QLineEdit, self).__init__(parent)
        self.Data = xml          
        try:
            self.setText(xml.attrib['default'])
        except:
            pass
    def setValue(self,data,start):
        try:
            if(self.Data.attrib['type'][-5:]=='Array'):
                num = str2int(self.Data.attrib['type'][1:-5])
                value = '[ '
                size = str2int(self.Data.attrib['size'])
                for i in range(0,size):
                    v = data.toint(start,num)
                    if(self.Data.attrib['display'] == 'hex'):
                        value += '0x%X,'%(v)
                    else:
                        value += '%d,'%(v)
                    start += num
                value = value[:-1] + ' ]'
                
            else:
                num = str2int(self.Data.attrib['type'][1:])
                value = data.toint(start,num)
                start += num
        except IndexError:
            QMessageBox(QMessageBox.Critical, 'Error', 'Data record witn Invalid Length  %s.'%(Xcp_GetResponse())).exec_();
            return
        if(self.Data.attrib['type'][-5:]=='Array'):
            self.setText(value)
        else:
            if(self.Data.attrib['display'] == 'hex'):
                self.setText('0x%X'%(value))
            else:
                self.setText('%d'%(value))
        return start
    def getValue(self,data):
        stype = self.Data.attrib['type']
        if(stype[-5:]=='Array'):
            num = str2int(stype[1:-5])
            assert(num<=32)
            size = str2int(self.Data.attrib['size'])
            string = str(self.text())
            if(string[:5]=='text='):
                va = []
                for c in string[5:]:
                    va.append(ord(c))
            else:
                string = string.replace('[', '').replace(']', '')
                grp = string.split(',')
                va = []
                for g in grp:
                    va.append(str2int(g))
            for i in range(0,size):
                try:
                    data.append(va[i],num)
                except:
                    #print(traceback.format_exc())
                    data.append(0,num)
        else:
            d = str2int(str(self.text()))
            num = str2int(stype[1:])
            assert(num<=32)
            data.append(d,num)

class UICommand(QGroupBox):
    def __init__(self,xml,parent=None):
        super(QGroupBox, self).__init__('%s %s'%(xml.attrib['name'],xml.attrib['ID']),parent)
        self.xml = xml
        grid = QGridLayout()
        
        self.leDataRequest = []
        self.leDataResponse = []
        row=0; col=0;
        for data in xml.find('Request'):
            if(data.attrib['type'][-6:] == 'Select'):
                leData = wDataUS(data)
            else:
                leData = wDataU(data)
            self.leDataRequest.append(leData)
            grid.addWidget(QLabel(data.attrib['name']), row, col+0)
            grid.addWidget(leData, row, col+1)
            col += 2
            
            if(col >= 8):
                row += 1
                col = 0
        if(col >=8):
            row += 1
            col = 0
        self.btnExecute = QPushButton('Execute')
        grid.addWidget(self.btnExecute, row, col)
        self.btnExecute.clicked.connect(self.on_btnExecute_clicked)
        row += 1;col = 0;
        for data in xml.find('Response'):
            if(data.attrib['type'][-6:] == 'Select'):
                leData = wDataUS(data)
            else:
                leData = wDataU(data)
            leData.setDisabled(True)
            self.leDataResponse.append(leData)
            grid.addWidget(QLabel(data.attrib['name']), row, col+0)
            grid.addWidget(leData, row, col+1)
            col += 2

            if(col >= 8):
                row += 1
                col = 0

        self.setLayout(grid)

    def on_btnExecute_clicked(self):
        global xcp_cpu_endian
        data = xcpbits()
        pid = str2int(self.xml.attrib['ID'])
        data.append(pid,8)

        for leData in self.leDataRequest:
            leData.getValue(data)

        res = Xcp_TransmitMessage(data.toarray())
        start = 8
        if(res==None):QMessageBox(QMessageBox.Critical, 'Error', 'Communication Error or Timeout').exec_();return
        if(res.toarray()[0]!=0xFF):
            QMessageBox(QMessageBox.Critical, 'Error', 'Command execute Failed!  %s.'%(Xcp_GetLastError())).exec_();
        else:
            if(pid == 0xFF):
                xcp_cpu_endian = res.toarray()[2]&0x01;
                #QMessageBox(QMessageBox.Information, 'XCP', 'XCP slave is online with CPU endian is %s(0=little,1=big)!'%(xcp_cpu_endian)).exec_()
                print('XCP slave is online with CPU endian is %s(0=little,1=big)!'%(xcp_cpu_endian))
            for leData in self.leDataResponse:
                start = leData.setValue(res,start)

class UIMTA(QGroupBox):
    def __init__(self,xml,parent=None):
        super(QGroupBox, self).__init__('%s'%(xml.attrib['name']),parent)
        self.xml = xml

        vbox = QVBoxLayout()

        grid = QGridLayout()

        grid.addWidget(QLabel('Address'), 0, 0)
        self.leAddress = QLineEdit()
        grid.addWidget(self.leAddress, 0, 1)
        self.leAddress.setText('0x8a01a0')

        grid.addWidget(QLabel('Size'), 0, 2)
        self.leSize = QLineEdit()
        grid.addWidget(self.leSize, 0, 3)
        self.leSize.setText('100')

        self.btnUpload = QPushButton('Upload')
        grid.addWidget(self.btnUpload, 0, 4)
        self.btnUpload.clicked.connect(self.on_btnUpload_clicked)

        self.btnDownload = QPushButton('Download')
        grid.addWidget(self.btnDownload, 0, 5)
        self.btnDownload.clicked.connect(self.on_btnDownload_clicked)

        vbox.addLayout(grid)

        self.leMemory = QTextEdit()
        vbox.addWidget(self.leMemory)
        self.leMemory.setMinimumWidth(1000)

        self.setLayout(vbox)

    def send(self,data):
        res = Xcp_TransmitMessage(data.toarray())
        if(res==None):QMessageBox(QMessageBox.Critical, 'Error', 'Communication Error or Timeout').exec_();return
        if(res.toarray()[0]!=0xFF):
            QMessageBox(QMessageBox.Critical, 'Error', 'Command execute Failed!  %s.'%(Xcp_GetLastError())).exec_();
            return
        return res

    def on_btnUpload_clicked(self):
        address = str2int(self.leAddress.text())
        size = str2int(self.leSize.text())
        content = []
        # short upload
        while(size > 0):
            data = xcpbits()
            data.append(0xf4)
            length = 7
            if(size < length):length=size
            data.append(length)
            data.append(0x00)   # resered
            data.append(0x00)   # type: memory
            data.append(address,32)
            size -= length
            address += length
            res = self.send(data)
            if(None==res):return
            bytes = res.toarray()
            vlen = len(bytes)-1
            for i in range(0,vlen):
                content.append(bytes[1+i])

        address = str2int(self.leAddress.text())
        cstr = ' address : 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F'
        astr = ''
        for i,d in enumerate(content):
            if(i%16==0):astr=' : ';cstr+=astr;cstr+='\n%08X :'%(address+i)
            cstr += ' %02X'%(content[i])
            # TODO astr += '%c'%(content[i])
        if((i+1)%16==0):
            astr=' : ';cstr+=astr;
        else:
            for j in range(0,16-(i+1)%16):
                cstr += ' FF'
            astr=' : ';cstr+=astr;
        self.leMemory.setText(cstr)
    def getMemoryContent(self):
        '''1234ABCD : 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F'''
        reItem = re.compile(r'([0-9A-F]+) : ([0-9A-F]+) ([0-9A-F]+) ([0-9A-F]+) ([0-9A-F]+) ([0-9A-F]+) ([0-9A-F]+) ([0-9A-F]+) ([0-9A-F]+) ([0-9A-F]+) ([0-9A-F]+) ([0-9A-F]+) ([0-9A-F]+) ([0-9A-F]+) ([0-9A-F]+) ([0-9A-F]+) ([0-9A-F]+)')
        cstr = self.leMemory.toPlainText()
        content = []
        for el in cstr.split('\n'):
            if(reItem.search(el)):
                for d in reItem.search(el).groups()[1:]:
                    content.append(str2int('0x%s'%(d)))
        return content

    def on_btnDownload_clicked(self):
        content = self.getMemoryContent()
        address = str2int(self.leAddress.text())
        size = str2int(self.leSize.text())
        # set MTA
        data = xcpbits()
        data.append(0xf6)
        data.append(0x0000,16)   # reserved
        data.append(0x00)   # type: memory
        data.append(address,32)
        res = self.send(data)
        if(None == res):return
        # download
        dlen = 0
        while(size > 0):
            length = 6
            if(size < length): length = size
            data = xcpbits()
            data.append(0xf0)
            data.append(length)
            for i in range(0,length):
                data.append(content[dlen+i])
            res = self.send(data)
            if(res==None):return
            size -= length
            dlen += length

class UIGroup(QScrollArea):
    def __init__(self, xml,parent=None):
        super(QScrollArea, self).__init__(parent)
        wd = QWidget()
        vBox = QVBoxLayout()
        for service in xml:
            if(service.tag=='Command'):
                vBox.addWidget(UICommand(service))
            elif(service.tag=='MTA'):
                vBox.addWidget(UIMTA(service))
        wd.setLayout(vBox)
        self.setWidget(wd)

class UIXcp(QWidget):
    def __init__(self, parent=None):
        super(QWidget, self).__init__(parent)
        self.vbox = QVBoxLayout()
        
        grid = QGridLayout()
        grid.addWidget(QLabel('load XCP calibration description file:'),0,0)
        self.leCml= QLineEdit()
        grid.addWidget(self.leCml,0,1)
        self.btnOpenCml = QPushButton('...')
        grid.addWidget(self.btnOpenCml,0,2)
        self.vbox.addLayout(grid)
        self.tabWidget = QTabWidget(self)
        self.vbox.addWidget(self.tabWidget)
        self.setLayout(self.vbox)

        default_cml = os.path.abspath('%s/../../../com/as.application/common/xcp.cml'%(os.curdir))

        self.leCml.setText(default_cml)
        self.loadCml(default_cml)

        self.btnOpenCml.clicked.connect(self.on_btnOpenCml_clicked)
        
        self.startTimer(1)
    
    def timerEvent(self,e):
        Xcp_PollDAQMessage()
    
    def loadCml(self,cml):
        self.tabWidget.clear()
        root = ET.parse(cml).getroot()
        for mm in root:
            self.tabWidget.addTab(UIGroup(mm), mm.tag)
        
    def on_btnOpenCml_clicked(self):
        rv = QFileDialog.getOpenFileName(None,'XCP calibration description file', '','XCP calibration description file (*.cml)')
        if(rv[0] != ''):
            self.leCml.setText(rv[0])
            self.loadCml(rv[0])