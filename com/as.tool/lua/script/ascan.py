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
from pyas.can import *
import sys

__all__ = ['UICan']

class UICan(QWidget):
    def __init__(self, parent=None):
        super(QWidget, self).__init__(parent)
        
        vbox = QVBoxLayout()
        
        grid = QGridLayout()
        
        self.cmbxCanBus = []
        self.cmbxCanDevice = []
        self.cmbxCanPort = []
        self.cmbxCanBaud = []
        self.btnOpen = []
        
        opens = [self.on_btnOpenClicked_0,self.on_btnOpenClicked_1,self.on_btnOpenClicked_2,self.on_btnOpenClicked_3]
        
        for i in range(4):
            self.cmbxCanBus.append(QComboBox())
            self.cmbxCanDevice.append(QComboBox())
            self.cmbxCanPort.append(QComboBox())
            self.cmbxCanBaud.append(QComboBox())
            self.btnOpen.append(QPushButton('Open'))
            
            self.cmbxCanBus[i].addItems(['bus 0','bus 1','bus 2','bus 3','bus 4','bus 5','bus 6','bus 7'])
            self.cmbxCanDevice[i].addItems(['serial','vxl','peak','socket'])
            self.cmbxCanPort[i].addItems(['port 0','port 1','port 2','port 3','port 4','port 5','port 6','port 7'])
            self.cmbxCanBaud[i].addItems(['125000','250000','500000','1000000','115200'])
        
            self.cmbxCanBus[i].setEditable(True)
            self.cmbxCanDevice[i].setEditable(True)
            self.cmbxCanBaud[i].setEditable(True)
            
            self.cmbxCanBus[i].setCurrentIndex(i)
            self.cmbxCanDevice[i].setCurrentIndex(i)
            if(i==0):
                self.cmbxCanBaud[i].setCurrentIndex(4)
            else:
                self.cmbxCanBaud[i].setCurrentIndex(3)
        
            grid.addWidget(QLabel('Can:'),i,0)
            grid.addWidget(self.cmbxCanBus[i],i,1)
            grid.addWidget(self.cmbxCanDevice[i],i,2)
            grid.addWidget(self.cmbxCanBaud[i],i,3)
            grid.addWidget(self.btnOpen[i],i,4) 
            
            self.btnOpen[i].clicked.connect(opens[i])                   


        vbox.addLayout(grid)
        self.setLayout(vbox)
    
    def on_btnOpen(self,id):
        bus = int(str(self.cmbxCanBus[id].currentText().replace('bus','')))
        device = str(self.cmbxCanDevice[id].currentText())
        port = int(str(self.cmbxCanPort[id].currentText().replace('port','')))
        baud = int(str(self.cmbxCanBaud[id].currentText()))
        can_open(bus,device,port,baud)
        self.btnOpen[id].setEnabled(False)
    
    def on_btnOpenClicked_0(self):
        self.on_btnOpen(0)
        
    def on_btnOpenClicked_1(self):
        self.on_btnOpen(1)
        
    def on_btnOpenClicked_2(self):
        self.on_btnOpen(2)
        
    def on_btnOpenClicked_3(self):
        self.on_btnOpen(3)                