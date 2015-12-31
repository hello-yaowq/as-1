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


from PyQt5 import QtCore, QtGui
from PyQt5.QtGui import *
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *

__all__ = ['UIFlashloader']

class AsFlashloader(QThread):
    infor = QtCore.pyqtSignal(str)
    progress = QtCore.pyqtSignal(int)
    def __init__(self,parent=None):
        super(QThread, self).__init__(parent)
        self.steps = [ self.enter_extend_session, self.security_extds_access,
                  self.enter_program_session,self.security_prgs_access,
                  self.routine_erase_flash, self.download_application ]

    def enter_extend_session(self):
        pass
    def security_extds_access(self):
        pass
    def enter_program_session(self):
        pass
    def security_prgs_access(self):
        pass
    def routine_erase_flash(self):
        pass
    def download_application(self):
        pass
    
    def run(self):
        self.infor.emit("starting ... ")
        for s in self.steps:
            self.infor.emit('  '+s.__name__.replace('_',' '))
            s()
        
class UIFlashloader(QWidget):
    def __init__(self, parent=None):
        super(QWidget, self).__init__(parent)
        vbox = QVBoxLayout()
        
        grid = QGridLayout()
        grid.addWidget(QLabel('Application'),0,0)
        self.leApplication = QLineEdit()
        grid.addWidget(self.leApplication,0,1)
        self.btnOpen = QPushButton('...')
        grid.addWidget(self.btnOpen,0,2)

        grid.addWidget(QLabel('Progress'),1,0)
        self.pgbProgress = QProgressBar()
        self.pgbProgress.setRange(0,100)
        grid.addWidget(self.pgbProgress,1,1)
        self.btnStart=QPushButton('Start')
        grid.addWidget(self.btnStart,1,2)
        vbox.addLayout(grid)
        
        self.leinfor = QTextEdit()
        self.leinfor.setReadOnly(True)
        vbox.addWidget(self.leinfor)
        
        self.setLayout(vbox)
        
        self.btnOpen.clicked.connect(self.on_btnOpen_clicked)
        self.btnStart.clicked.connect(self.on_btnStart_clicked)
        self.loader = AsFlashloader()
        self.loader.infor.connect(self.on_loader_infor)
        self.loader.progress.connect(self.on_loader_progress)
        
        self.app = ''

    def on_loader_infor(self,text):
        self.leinfor.append(text)
    
    def on_loader_progress(self,prg):
        self.pgbProgress.setValue(prg)
        
    def on_btnOpen_clicked(self):
        rv = QFileDialog.getOpenFileName(None,'application file', '','application (*.s19 *.bin)')
        self.app = rv[0]
        self.leApplication.setText(self.app)
    
    def on_btnStart_clicked(self):
        self.pgbProgress.setValue(1)
        self.loader.start()