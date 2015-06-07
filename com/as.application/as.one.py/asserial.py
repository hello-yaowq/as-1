from PyQt4 import QtCore, QtGui
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from serial import Serial
from time import sleep

class AsSerial(QThread):
    recv_msg = QtCore.pyqtSignal(int)
    def __init__(self,parent=None):
        super(QThread, self).__init__(parent)
        
    def open(self, settings):
        try:
            self.serial = Serial(settings["port"], settings["baund"], settings["bytesize"],
                    settings["parity"], settings["stopbits"], settings["timeout"])
            self.serial.flushInput()
            self.serial.flushOutput()
        except Exception, msg:
            return (False, msg.message.decode("gbk"))
        
        return (True, "success")
    def resetArduino(self):
        self.serial.setDTR(0)
        sleep(0.1)
        self.serial.setDTR(1)
        
    def terminate(self):
        self.__terminate = True
        
    def send(self, data, _type):
        self.serial.write(data)
    
    def __recv(self):
        data, quit = None, False
        while(True):
            if(self.__terminate):
                break
            data = self.serial.read(1)
            if(data == ''):
                continue
            while(True):
                n = self.serial.inWaiting()
                if( n > 0):
                    data = "%s%s" % (data, self.serial.read(n))
                    sleep(0.02) # data is this interval will be merged
                else:
                    quit = True
                    break
            if(quit==True):
                break

        return data
    
    def close(self):
        if self.serial.isOpen():
            self.serial.close()
    
    def run(self):
        while(True):
            data = self.__recv()
            if not data:
                break
            self.recv_msg.emit(data)

        self.serial.close()  
        
        
class UISerial(QWidget):
    def __init__(self, parent=None):
        super(QWidget, self).__init__(parent)
        self.creGui()
        
    def creGui(self):
        grid = QtGui.QGridLayout()
        self.lblInfo = QtGui.QLabel('Status')
        grid.addWidget(self.lblInfo, 0, 0)
        self.cmdPorts = QtGui.QComboBox()
        ports = []
        for i in range(10):
            ports.append('COM%s:115200:8:N:1'%(i))
        self.cmdPorts.addItems(QStringList(ports))
        grid.addWidget(self.cmdPorts, 0, 1)
        self.cmdPorts.setCurrentIndex(5)
    
        self.btnOpenClose = QPushButton('Open')
        self.connect(self.btnOpenClose,SIGNAL('clicked()'),self.on_btnOpenClose_clicked)
        grid.addWidget(self.btnOpenClose, 0, 2)
        self.setLayout(grid)
        
    def on_btnOpenClose_clicked(self):
        pass          