from email._header_value_parser import Address
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
 
from .dcm import *
from .s19 import *


from PyQt5 import QtCore, QtGui
from PyQt5.QtGui import *
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *

import os

__all__ = ['UIFlashloader']

class AsFlashloader(QThread):
    infor = QtCore.pyqtSignal(str)
    progress = QtCore.pyqtSignal(int)
    def __init__(self,parent=None):
        super(QThread, self).__init__(parent)
        self.steps = [ self.enter_extend_session, self.security_extds_access,
                  self.enter_program_session,self.security_prgs_access,
                  self.download_flash_driver,
                  self.routine_erase_flash, self.download_application ]
        self.enable = []
        for s in self.steps:
            self.enable.append(True)
        self.dcm = dcm(0,0x732,0x731)
        self.progress_value = 0
        self.app = None
        self.flsdrv = None

    def setTarget(self,app,flsdrv=None):
        self.app = app
        self.flsdrv = flsdrv

    def GetSteps(self):
        ss = []
        for s in self.steps:
            ss.append(s.__name__.replace('_',' '))
        return ss
    
    def SetEnable(self,step,enable):
        for id,s in enumerate(self.steps):
            if(step == s.__name__.replace('_',' ')):
                self.enable[id] = enable

    def transmit(self,req,exp):
        ercd,res = self.dcm.transmit(req)
        if(ercd == True):
            if(len(res)>=len(exp)):
                for i in range(len(exp)):
                    if((res[i]!=exp[i]) and (exp[i]!=-1)):
                        ercd = False
                        break
            else:
                ercd = False
        if(ercd == True): 
            self.infor.emit('  success')
            self.progress.emit(self.progress_value)
        else:
            self.infor.emit('  failed')
        return ercd,res
    def enter_extend_session(self):
        return self.transmit([0x10,0x03], [0x50,0x03])
    def security_extds_access(self):
        ercd,res = self.transmit([0x27,0x01], [0x67,0x01,-1,-1,-1,-1])
        if(ercd):
            seed = (res[2]<<24) + (res[3]<<16) + (res[4]<<8) +(res[5]<<0)
            key = (seed^0x78934673)
            self.infor.emit(' send key %X from seed %X'%(key,seed))
            ercd,res = self.transmit([0x27,0x02,(key>>24)&0xFF,(key>>16)&0xFF,(key>>8)&0xFF,(key>>0)&0xFF],[0x67,0x02])
        return ercd,res
    def enter_program_session(self):
        return self.transmit([0x10,0x02], [0x50,0x02])
    def security_prgs_access(self):
        ercd,res = self.transmit([0x27,0x03], [0x67,0x03,-1,-1,-1,-1])
        if(ercd):
            seed = (res[2]<<24) + (res[3]<<16) + (res[4]<<8) +(res[5]<<0)
            key = (seed^0x94586792)
            self.infor.emit(' send key %X from seed %X'%(key,seed))
            ercd,res = self.transmit([0x27,0x04,(key>>24)&0xFF,(key>>16)&0xFF,(key>>8)&0xFF,(key>>0)&0xFF],[0x67,0x04])
        return ercd,res
    def request_download(self,address,size,identifier):
        self.infor.emit(' request download')
        return self.transmit([0x34,0x00,0x44,     \
                            (address>>24)&0xFF,(address>>16)&0xFF,(address>>8)&0xFF,(address>>0)&0xFF,   \
                            (size>>24)&0xFF,(size>>16)&0xFF,(size>>8)&0xFF,(size>>0)&0xFF,  \
                            identifier],[0x74])

    def request_upload(self,address,size,identifier):
        self.infor.emit(' request upload')
        return self.transmit([0x35,0x00,0x44,     \
                            (address>>24)&0xFF,(address>>16)&0xFF,(address>>8)&0xFF,(address>>0)&0xFF,   \
                            (size>>24)&0xFF,(size>>16)&0xFF,(size>>8)&0xFF,(size>>0)&0xFF,  \
                            identifier],[0x75])
    
    def request_transfer_exit(self):
        self.infor.emit(' request transfer exit')
        return self.transmit([0x37],[0x77])
    
    def download_one_section(self,address,size,data,identifier):
        FLASH_WRITE_SIZE = 512
        blockSequenceCounter = 1
        left_size = size
        pos = 0
        ability = int(((4096-4)/FLASH_WRITE_SIZE)) * FLASH_WRITE_SIZE
        ercd,res = self.request_download(address,size,identifier)
        if(ercd == False):return ercd,res
        while(left_size>0 and ercd==True):
            req = [0x36,blockSequenceCounter,0,identifier]
            if(left_size > ability):
                sz = ability
            else:
                sz = int((left_size+FLASH_WRITE_SIZE-1)/FLASH_WRITE_SIZE)*FLASH_WRITE_SIZE
                left_size = 0
            for i in range(sz):
                if((pos+i)<size):
                    req.append(data[pos+i])
                else:
                    req.append(0xFF)
            self.infor.emit(' transfer block %s'%(blockSequenceCounter))
            ercd,res = self.transmit(req,[0x76,blockSequenceCounter])
            if(ercd == False):return ercd,res
            blockSequenceCounter = (blockSequenceCounter + 1)&0xFF
            pos += sz
        ercd,res = self.request_transfer_exit()
        if(ercd == False):return ercd,res
        return ercd,res

    def upload_one_section(self,address,size,identifier):
        FLASH_READ_SIZE = 512
        blockSequenceCounter = 1
        left_size = size
        ability = int(((4096-4)/FLASH_READ_SIZE)) * FLASH_READ_SIZE
        ercd,res = self.request_upload(address,size,identifier)
        if(ercd == False):return ercd,res,None
        data = []
        while(left_size>0 and ercd==True):
            req = [0x36,blockSequenceCounter,0,identifier]
            self.infor.emit(' transfer block %s'%(blockSequenceCounter))
            ercd,res = self.transmit(req,[0x76,blockSequenceCounter])
            if(ercd == False):return ercd,res,None
            blockSequenceCounter = (blockSequenceCounter + 1)&0xFF
            sz = len(res)-2
            if (left_size > sz):
                left_size = left_size - sz
            else:
                left_size = 0
            for b in res[2:]:
                data.append(b)
        ercd,res = self.request_transfer_exit()
        if(ercd == False):return ercd,res,None
        return ercd,res,data
    
    def compare(self,d1,d2):
        for i,b in enumerate(d1):
            if(b!=d2[i]):return False
        return True

    def download_flash_driver(self):
        flsdrv = s19(self.flsdrv)
        ary = flsdrv.getData()
        for ss in ary:
            ercd,res = self.download_one_section(ss['address']-ary[0]['address'],ss['size'],ss['data'],0xFD)
            if(ercd == False):return ercd,res
        
        for ss in ary:
            ercd,res,up = self.upload_one_section(ss['address']-ary[0]['address'],ss['size'],0xFD)
            if(ercd and self.compare(ss['data'], up)):
                self.infor.emit('  check flash driver pass!')
            else:
                self.infor.emit('  check flash driver failed!')
                return False,res
        return ercd,res
    
    def routine_erase_flash(self):
        return self.transmit([0x31,0x01,0xFF,0x01,0x00,0x01,0x00,0x00,0x00,0x03,0x00,0x00,0xFF],[0x71,0x01,0xFF,0x01])
    
    def download_application(self):
        app = s19(self.app)
        ary = app.getData()
        for ss in ary:
            ercd,res = self.download_one_section(ss['address'],ss['size'],ss['data'],0xFF)
            if(ercd == False):return ercd,res
        
        for ss in ary:
            ercd,res,up = self.upload_one_section(ss['address'],ss['size'],0xFF)
            if(ercd and self.compare(ss['data'], up)):
                self.infor.emit('  check application pass!')
            else:
                self.infor.emit('  check application failed!')
                return False,res
        return ercd,res
    
    def run(self):
        self.infor.emit("starting ... ")
        self.progress_value = 0
        for id,s in enumerate(self.steps):
            if(self.enable[id] == True):
                self.progress_value += 50/(len(self.steps))
                self.infor.emit('>> '+s.__name__.replace('_',' '))
                ercd,res = s()
                if(ercd == False):
                    self.infor.emit("\n\n  >> boot failed <<\n\n")
                    return
        self.progress.emit(100)

class AsStepEnable(QCheckBox):
    enableChanged=QtCore.pyqtSignal(str,bool)
    def __init__(self,text,parent=None):
        super(QCheckBox, self).__init__(text,parent)
        self.stateChanged.connect(self.on_stateChanged)
    def on_stateChanged(self,state):
        self.enableChanged.emit(self.text(),state)
        
class UIFlashloader(QWidget):
    def __init__(self, parent=None):
        super(QWidget, self).__init__(parent)
        
        self.loader = AsFlashloader()
        self.loader.infor.connect(self.on_loader_infor)
        self.loader.progress.connect(self.on_loader_progress)
        
        vbox = QVBoxLayout()
        
        grid = QGridLayout()
        grid.addWidget(QLabel('Application'),0,0)
        self.leApplication = QLineEdit()
        grid.addWidget(self.leApplication,0,1)
        self.btnOpenApp = QPushButton('...')
        grid.addWidget(self.btnOpenApp,0,2)
        
        grid.addWidget(QLabel('Flash Driver'),1,0)
        self.leFlsDrv = QLineEdit()
        grid.addWidget(self.leFlsDrv,1,1)
        self.btnOpenFlsDrv = QPushButton('...')
        grid.addWidget(self.btnOpenFlsDrv,1,2)

        grid.addWidget(QLabel('Progress'),2,0)
        self.pgbProgress = QProgressBar()
        self.pgbProgress.setRange(0,100)
        grid.addWidget(self.pgbProgress,2,1)
        self.btnStart=QPushButton('Start')
        grid.addWidget(self.btnStart,2,2)
        vbox.addLayout(grid)
        
        hbox = QHBoxLayout()
        vbox2 = QVBoxLayout()
        for s in self.loader.GetSteps():
            cbxEnable = AsStepEnable(s)
            cbxEnable.setChecked(True)
            cbxEnable.enableChanged.connect(self.on_enableChanged)
            vbox2.addWidget(cbxEnable)
        hbox.addLayout(vbox2)
        self.leinfor = QTextEdit()
        self.leinfor.setReadOnly(True)
        hbox.addWidget(self.leinfor)
        
        vbox.addLayout(hbox)
        
        self.setLayout(vbox)
        
        self.btnOpenApp.clicked.connect(self.on_btnOpenApp_clicked)
        self.btnOpenFlsDrv.clicked.connect(self.on_btnOpenFlsDrv_clicked)
        self.btnStart.clicked.connect(self.on_btnStart_clicked)
        
        self.app=''
        self.flsdrv=''
        
        default_app = '/home/parai/workspace/as/release/ascore/out/stm32f107vc.s19'
        default_flsdrv = '/home/parai/workspace/as/release/asboot/out/stm32f107vc-flsdrv.s19'
        if(os.path.exists(default_app)):
            self.app = default_app
            self.leApplication.setText(default_app)
        if(os.path.exists(default_flsdrv)):
            self.flsdrv = default_flsdrv
            self.leFlsDrv.setText(default_flsdrv)

    def on_enableChanged(self,step,enable):
        self.loader.SetEnable(step, enable)

    def on_loader_infor(self,text):
        self.leinfor.append(text)
    
    def on_loader_progress(self,prg):
        self.pgbProgress.setValue(prg)

    def on_btnOpenApp_clicked(self):
        rv = QFileDialog.getOpenFileName(None,'application file', '','application (*.s19 *.bin)')
        self.app = rv[0]
        self.leApplication.setText(self.app)

    def on_btnOpenFlsDrv_clicked(self):
        rv = QFileDialog.getOpenFileName(None,'flash driver file', '','flash driver (*.s19 *.bin)')
        self.flsdrv = rv[0]
        self.leFlsDrv.setText(self.flsdrv)


    def on_btnStart_clicked(self):
        if(os.path.exists(self.app)):
            self.pgbProgress.setValue(1)
            self.loader.setTarget(self.app, self.flsdrv)
            self.loader.start()
        else:
            QMessageBox.information(self, 'Tips', 'Please load a valid application first!')
        