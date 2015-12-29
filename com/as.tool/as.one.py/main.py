#! /bin/python

import os,sys

from PyQt4 import QtCore, QtGui
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from asserial import UISerial
    
class Window(QtGui.QWidget):
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
        grid.addWidget(tabWidget)
        self.setLayout(grid)

def main():
    app = QtGui.QApplication(sys.argv)
    app.setFont(QFont('Consolas', 10)) 
    mWain = Window()
    mWain.show()
    sys.exit(app.exec_()) 
    
if(__name__ == '__main__'):
    main()
