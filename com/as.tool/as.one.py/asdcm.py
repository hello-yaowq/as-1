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
from time import sleep
from time import ctime
from binascii import hexlify, unhexlify
import sys,os
import xml.etree.ElementTree as ET
from pyas.dcm import *
import traceback

from bitarray import bitarray

__all__ = ['UIDcm']


class dcmbits():
    def __init__(self):
        self.bits = bitarray()

    def append(self,d, num=1):
        for i in range(num):
            if((d&(1<<(num-1-i))) != 0):
                self.bits.append(True)
            else:
                self.bits.append(False)
    
    def toarray(self):
        return self.bits.tobytes()

def str2int(sstr):
    if(sstr[:2].lower() == '0x'):
        return int(sstr,16)
    elif(sstr[:2].lower() == '0b'):
        return int(sstr,2)
    else:
        return int(sstr,10)

__dcm__ = dcm(0,0x732,0x731)

def Dcm_TransmitMessage(req):
    ercd,res = __dcm__.transmit(req)
    if(ercd==True):
        return res
    return None

class wDataUS(QComboBox):
    '''Data U8/U16/U32 Select'''
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
        a = dcmbits()
        if(self.Data.attrib['type']=='U32  Select'):
            num = 32
        elif(self.Data.attrib['type']=='U16  Select'):
            num = 16
        elif(self.Data.attrib['type']=='U8 Select'):
            num=8
        else:
            assert(0)
        data.append(d, num)
    
    def setValue(self,data,start):
        try:
            if(self.Data.attrib['type']=='U32 Select'):
                value = (data[start+0]<<24) + (data[start+1]<<16) + (data[start+2]<<8) + (data[start+3]<<0)
                start += 4
            elif(self.Data.attrib['type']=='U16 Select'):
                value = (data[start+0]<<8) + (data[start+1]<<0)
                start += 2
            elif(self.Data.attrib['type']=='U8 Select'):
                value = (data[start+0]<<0)
                start += 1
            else:
                assert(0)
        except IndexError:
            QMessageBox(QMessageBox.Critical, 'Error', 'Data record witn Invalid Length  %s.'%(Dcm_GetResponse())).exec_();
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
    '''Data U8/U16/U32 U8Array'''
    def __init__(self,xml,parent=None):
        super(QLineEdit, self).__init__(parent)
        self.Data = xml          
        try:
            self.setText(xml.attrib['default'])
        except:
            pass
    def setValue(self,data,start):
        try:
            if(self.Data.attrib['type']=='U32'):
                value = (data[start+0]<<24) + (data[start+1]<<16) + (data[start+2]<<8) + (data[start+3]<<0)
                start += 4
            elif(self.Data.attrib['type']=='U16'):
                value = (data[start+0]<<8) + (data[start+1]<<0)
                start += 2
            elif(self.Data.attrib['type']=='U8'):
                value = (data[start+0]<<0)
                start += 1
            elif(self.Data.attrib['type']=='U8Array'):
                value = '[ '
                size = str2int(self.Data.attrib['size'])
                for i in range(0,size):
                    if(self.Data.attrib['display'] == 'hex'):
                        value += '0x%0-2X,'%(data[start+i])
                    else:
                        value += '0x%-3d,'%(data[start+i])
                value = value[:-1] + ' ]'
                start += size
            else:
                assert(0)
        except IndexError:
            QMessageBox(QMessageBox.Critical, 'Error', 'Data record witn Invalid Length  %s.'%(Dcm_GetResponse())).exec_();
            return
        if(self.Data.attrib['type']=='U8Array'):
            self.setText(value)
        else:
            if(self.Data.attrib['display'] == 'hex'):
                self.setText('0x%X'%(value))
            else:
                self.setText('%d'%(value))
        return start
    def getValue(self,data):
        if(self.Data.attrib['type']=='U8Array'):
            size = str2int(self.Data.attrib['size'])
            string = str(self.text())
            string = string.replace('[', '').replace(']', '')
            grp = string.split(',')
            for i in range(0,size):
                try:
                    data.append(str2int(grp[i])&0xFF,8)
                except:
                    #print(traceback.format_exc())
                    data.append(0xFF,8)
        else:
            d = str2int(str(self.text()))
            num = 0
            if(self.Data.attrib['type']=='U32'):
                num = 32
            elif(self.Data.attrib['type']=='U16'):
                num = 16
            elif(self.Data.attrib['type']=='U8'):
                num = 8
            else:
                assert(0)
            data.append(d,num)

class UIInputOutputControl(QGroupBox):
    def __init__(self,xml,parent=None):
        super(QGroupBox, self).__init__(xml.attrib['name'],parent)
        self.IOC = xml
        grid = QGridLayout()
        
        self.leDatas = []
        row=0; col=0;
        for data in xml:
            if(data.attrib['type'][-6:] == 'Select'):
                leData = wDataUS(data)
            else:
                leData = wDataU(data)
            self.leDatas.append(leData)
            grid.addWidget(QLabel(data.attrib['name']), row, col+0)
            grid.addWidget(leData, row, col+1)
            col += 2
            
            if(col >= 8):
                row += 1
                col = 0
        row += 1
            
        self.btnStart = QPushButton('Start')
        self.btnRtce = QPushButton('Return Ctrl to ECU')
        grid.addWidget(self.btnStart, 2, 0)
        grid.addWidget(self.btnRtce, 2, 1)
        self.btnStart.clicked.connect(self.on_btnStart_clicked)
        self.btnRtce.clicked.connect(self.on_btnRtce_clicked) 
        
        self.setLayout(grid)
        
    def on_btnStart_clicked(self):
        data = dcmbits()
        data.append(0x2F,8)
        did = str2int(self.IOC.attrib['ID'])
        data.append(did,16)
        data.append(0x03,8)
        
        for leData in self.leDatas:
            leData.getValue(data)

        res = Dcm_TransmitMessage(data.toarray()) 
        if(res==None):QMessageBox(QMessageBox.Critical, 'Error', 'Communication Error or Timeout').exec_();return
        if(res[0]!=0x6F):
            QMessageBox(QMessageBox.Critical, 'Error', 'IOC Start Failed!  %s.'%(Dcm_GetLastError())).exec_();
              
    def on_btnRtce_clicked(self):
        data = dcmbits()
        data.append(0x2F,8)
        did = str2int(self.IOC.attrib['ID'])
        data.append(did,16)
        data.append(0x00,8)
        res = Dcm_TransmitMessage(data.toarray())  
        if(res==None):QMessageBox(QMessageBox.Critical, 'Error', 'Communication Error or Timeout').exec_();return 
        if(res[0]!=0x6F):
            QMessageBox(QMessageBox.Critical, 'Error', 'IOC Return Control to ECU Failed!  %s.'%(Dcm_GetLastError())).exec_();

class UIDataIdentifier(QGroupBox):
    def __init__(self,xml,parent=None):
        super(QGroupBox, self).__init__(xml.attrib['name'],parent)
        self.DID = xml
        grid = QGridLayout()
        
        self.leDatas = []
        row=0; col=0;
        for data in xml:
            if(data.attrib['type'][-6:] == 'Select'):
                leData = wDataUS(data)
            else:
                leData = wDataU(data)
            self.leDatas.append(leData)
            grid.addWidget(QLabel(data.attrib['name']), row, col+0)
            grid.addWidget(leData, row, col+1)
            col += 2
            
            if(col >= 8):
                row += 1
                col = 0
        row += 1      
        self.btnRead = QPushButton('Read')
        self.btnWrite = QPushButton('Write')
        grid.addWidget(self.btnRead, row, 0)
        grid.addWidget(self.btnWrite, row, 1)
        self.btnRead.clicked.connect(self.on_btnRead_clicked)
        self.btnWrite.clicked.connect(self.on_btnWrite_clicked)
        
        if(xml.attrib['attribute']=='r'): # read-only
            self.btnWrite.setDisabled(True)
            
        if(xml.attrib['attribute']=='w'): # write-only
            self.btnRead.setDisabled(True)
        
        self.setLayout(grid)
    def on_btnRead_clicked(self):
        data = dcmbits()
        data.append(0x22,8)
        did = str2int(self.DID.attrib['ID'])
        data.append(did,16)
       
        res = Dcm_TransmitMessage(data.toarray()) 
        if(res==None):QMessageBox(QMessageBox.Critical, 'Error', 'Communication Error or Timeout').exec_();return
        start = 3
        if(res[0]!=0x62):
            QMessageBox(QMessageBox.Critical, 'Error', 'DID Start Failed!  %s.'%(Dcm_GetLastError())).exec_();
        else:
            for leData in self.leDatas:
                start = leData.setValue(res,start)
    def on_btnWrite_clicked(self):
        data = dcmbits()
        data.append(0x2E,8)
        did = str2int(self.DID.attrib['ID'])
        data.append(did,16)
        for leData in self.leDatas:
            leData.getValue(data)
        res = Dcm_TransmitMessage(data.toarray())  
        if(res==None):QMessageBox(QMessageBox.Critical, 'Error', 'Communication Error or Timeout').exec_();return 
        if(res[0]!=0x6E):
            QMessageBox(QMessageBox.Critical, 'Error', 'DID Write Failed!  %s.'%(Dcm_GetLastError())).exec_();

class UIRoutineControl(QGroupBox):
    def __init__(self,xml,parent=None):
        super(QGroupBox, self).__init__(xml.attrib['name'],parent)
        self.SRI = xml
        grid = QGridLayout()
        
        self.leDatas = []
        row=0; col=0;
        for data in xml:
            if(data.attrib['type'][-6:] == 'Select'):
                leData = wDataUS(data)
            else:
                leData = wDataU(data)
            self.leDatas.append(leData)
            grid.addWidget(QLabel(data.attrib['name']), row, col+0)
            grid.addWidget(leData, row, col+1)
            col += 2
            
            if(col >= 8):
                row += 1
                col = 0
        row += 1      
        
        grid.addWidget(QLabel('Result:'), row, 0)
        self.leResult = QLineEdit('No Result')
        self.leResult.setEnabled(False)
        grid.addWidget(self.leResult, row, 1)
            
        self.btnStart = QPushButton('Start')
        self.btnStop = QPushButton('Stop')
        self.btnResult = QPushButton('Result')
        grid.addWidget(self.btnStart, row+1, 0)
        grid.addWidget(self.btnStop, row+1, 1)
        grid.addWidget(self.btnResult, row+1, 2)
        
        self.btnStart.clicked.connect(self.on_btnStart_clicked)
        self.btnStop.clicked.connect(self.on_btnStop_clicked)
        self.btnResult.clicked.connect(self.on_btnResult_clicked)
        
        try:
            if(xml.attrib['stop']=='true'):
                self.btnStop.setDisabled(False)
            else:
                self.btnStop.setDisabled(True)
        except:
            self.btnStop.setDisabled(True)
            
        try:
            if(xml.attrib['result']=='true'):
                self.btnResult.setDisabled(False)
            else:
                self.btnResult.setDisabled(True)
        except:
            self.btnResult.setDisabled(True)            
        
        self.setLayout(grid)
        
    def on_btnStart_clicked(self):
        data = dcmbits()
        data.append(0x3101, 16)
        did = str2int(self.SRI.attrib['ID'])
        data.append(did,16)
        
        for leData in self.leDatas:
            leData.getValue(data)

        res = Dcm_TransmitMessage(data.toarray()) 
        if(res==None):QMessageBox(QMessageBox.Critical, 'Error', 'Communication Error or Timeout').exec_();return
        if(res[0]!=0x71):
            QMessageBox(QMessageBox.Critical, 'Error', 'SRI Start Failed!  %s.'%(Dcm_GetLastError())).exec_();
        else:
            self.leResult.setText('Please Click Button \'Result\' to Read the Result.')
    
    def on_btnStop_clicked(self):
        data = dcmbits()
        data.append(0x3102, 16)
        did = str2int(self.SRI.attrib['ID'])
        data.append(did,16)
        res = Dcm_TransmitMessage(data.toarray())  
        if(res==None):QMessageBox(QMessageBox.Critical, 'Error', 'Communication Error or Timeout').exec_();return 
        if(res[0]!=0x71):
            QMessageBox(QMessageBox.Critical, 'Error', 'SRI Stop Failed!  %s.'%(Dcm_GetLastError())).exec_();   
    def on_btnResult_clicked(self):
        data = dcmbits()
        data.append(0x3103,16)
        did = str2int(self.SRI.attrib['ID'])
        data.append(did,16)
        res = Dcm_TransmitMessage(data.toarray())  
        if(res==None):QMessageBox(QMessageBox.Critical, 'Error', 'Communication Error or Timeout').exec_();return 
        if(res[0]!=0x71):
            QMessageBox(QMessageBox.Critical, 'Error', 'SRI Request Result Failed!  %s.'%(Dcm_GetLastError())).exec_();
        else:
            self.leResult.setText('check it by the raw response: %s'%(FormatMessage(res)))

class UIGroup(QWidget):
    def __init__(self, xml,parent=None):
        super(QWidget, self).__init__(parent)
        
        vBox = QVBoxLayout()
        for service in xml:
            if(service.tag=='InputOutputControl'):
                vBox.addWidget(UIInputOutputControl(service))
            elif(service.tag=='DataIdentifier'):
                vBox.addWidget(UIDataIdentifier(service))
            elif(service.tag=='RoutineControl'):
                vBox.addWidget(UIRoutineControl(service))
        self.setLayout(vBox)
        
class UIDcm(QWidget):
    def __init__(self, parent=None):
        super(QWidget, self).__init__(parent)
        self.vbox = QVBoxLayout()
        
        grid = QGridLayout()
        grid.addWidget(QLabel('load diagnostic description file:'),0,0)
        self.leDml= QLineEdit()
        grid.addWidget(self.leDml,0,1)
        self.btnOpenDml = QPushButton('...')
        grid.addWidget(self.btnOpenDml,0,2)
        self.vbox.addLayout(grid)
        self.tabWidget = QTabWidget(self)
        self.vbox.addWidget(self.tabWidget)
        self.setLayout(self.vbox)
        
        if(os.name == 'nt'):
            default_dml = 'D:/repository/as/com/as.application/common/diagnostic.dml'
        else:
            default_dml = '/home/parai/workspace/as/com/as.application/common/diagnostic.dml'

        self.leDml.setText(default_dml)
        self.loadDml(default_dml)

        self.btnOpenDml.clicked.connect(self.on_btnOpenDml_clicked)
    
    def loadDml(self,dml):
        self.tabWidget.clear()
        root = ET.parse(dml).getroot()
        for mm in root:
            self.tabWidget.addTab(UIGroup(mm), mm.tag)
        
    def on_btnOpenDml_clicked(self):
        rv = QFileDialog.getOpenFileName(None,'diagnostic description file', '','diagnostic description file (*.dml)')
        if(rv[0] != ''):
            self.leDml.setText(rv[0])
            self.loadDml(rv[0])

if(__name__ == '__main__'):
    a = dcmbits()
    a.append(0xdeadbeef,32)
    print(a.toarray())
    