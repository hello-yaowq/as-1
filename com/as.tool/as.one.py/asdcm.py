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

__all__ = ['UIDcm']
class UIInputOutputControl(QGroupBox):
    def __init__(self,xml,parent=None):
        super(QGroupBox, self).__init__(xml.attrib['Name'],parent)

class UIDataIdentifier(QGroupBox):
    def __init__(self,xml,parent=None):
        super(QGroupBox, self).__init__(xml.attrib['Name'],parent)

class UIRoutineControl(QGroupBox):
    def __init__(self,xml,parent=None):
        super(QGroupBox, self).__init__(xml.attrib['Name'],parent)

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