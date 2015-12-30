#! /bin/python
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
import os,sys

from PyQt5 import QtCore, QtGui
from PyQt5.QtGui import *
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from asserial import UISerial
from pyas.flashloader import *
    
class Window(QWidget):
    def __init__(self, parent=None):
        super(QWidget, self).__init__(parent) 
        self.creGui()
        self.setWindowTitle("AsOne")
           
    def closeEvent(self,Event):
        pass
    def creGui(self):
        grid = QVBoxLayout()
        tabWidget = QTabWidget(self)
        tabWidget.addTab(UISerial(), 'Serial')
        tabWidget.addTab(UIFlashloader(), 'Flashloader')
        grid.addWidget(tabWidget)
        self.setLayout(grid)

def main():
    app = QApplication(sys.argv)
    app.setFont(QFont('Consolas', 10)) 
    mWain = Window()
    mWain.show()
    sys.exit(app.exec_()) 
    
if(__name__ == '__main__'):
    main()
