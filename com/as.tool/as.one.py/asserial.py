from PyQt4 import QtCore, QtGui
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from serial import Serial
from time import sleep

class AsSerial(QThread):
    recv_msg = QtCore.pyqtSignal(str)
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
        vbox = QVBoxLayout()
        grid = QtGui.QGridLayout()
        grid.addWidget(QLabel('Port:'), 0, 0)
        self.cmdPorts = QtGui.QComboBox()
        self.cmdPorts.addItems(QStringList(['COM0','COM1','COM2','COM3','COM4','COM5','COM6','COM7','COM8','COM9']))
        grid.addWidget(self.cmdPorts, 0, 1)
        self.cmdPorts.setCurrentIndex(5)
        self.cmdPorts.setEditable(True)
        
        grid.addWidget(QLabel('Baudrate:'), 0, 2)
        self.cmdBaudrate = QtGui.QComboBox()
        self.cmdBaudrate.addItems(QStringList(['4800','7200','9600','14400','19200','38400','57600','115200','128000']))
        grid.addWidget(self.cmdBaudrate, 0, 3)
        self.cmdBaudrate.setCurrentIndex(7)
        self.cmdBaudrate.setEditable(True) 
        
        grid.addWidget(QLabel('Data:'), 0, 4)
        self.cmdData = QtGui.QComboBox()
        self.cmdData.addItems(QStringList(['8','7','6','5','4']))
        grid.addWidget(self.cmdData, 0, 5)
        self.cmdData.setCurrentIndex(0) 
        
        grid.addWidget(QLabel('Parity:'), 0, 6)
        self.cmdParity = QtGui.QComboBox()
        self.cmdParity.addItems(QStringList(['Odd','Even','Flag','Space','None']))
        grid.addWidget(self.cmdParity, 0, 7)
        self.cmdParity.setCurrentIndex(4)
        
        grid.addWidget(QLabel('Stop:'), 0, 8)
        self.cmdStop = QtGui.QComboBox()
        self.cmdStop.addItems(QStringList(['1','1.5','2']))
        grid.addWidget(self.cmdStop, 0, 9)
        self.cmdStop.setCurrentIndex(0)
        
        grid.addWidget(QLabel('FlowControl:'), 0, 10)
        self.cmdFlowControl = QtGui.QComboBox()
        self.cmdFlowControl.addItems(QStringList(['Xon / Xoff','Hardware','None']))
        grid.addWidget(self.cmdFlowControl, 0, 11)
        self.cmdFlowControl.setCurrentIndex(2)                              
    
        self.btnOpenClose = QPushButton('Open')
        self.connect(self.btnOpenClose,SIGNAL('clicked()'),self.on_btnOpenClose_clicked)
        grid.addWidget(self.btnOpenClose, 1, 11)
        
        self.rbAscii = QRadioButton('ASCII')
        self.rbAscii.setChecked(True)
        self.rbHex = QRadioButton('HEX')
        grid.addWidget(self.rbAscii, 1, 0)
        grid.addWidget(self.rbHex, 1, 1)
        
        self.btnClearHistory = QPushButton('Clear history')
        self.connect(self.btnOpenClose,SIGNAL('clicked()'),self.on_btnClearHistory_clicked)
        grid.addWidget(self.btnClearHistory, 1, 7)
        vbox.addLayout(grid)
        
        self.tbHistory = QTextBrowser()
        vbox.addWidget(self.tbHistory)
        
        self.teInput = QTextEdit()
        vbox.addWidget(self.teInput)
        
        hbox = QHBoxLayout()
        self.btnResetArduin = QPushButton('ResetArduino')
        self.btnSend = QPushButton('Send')
        self.leRecvNbr = QLineEdit()
        self.leSendNbr = QLineEdit()
        self.leRecvNbr.setDisabled(True)
        self.leSendNbr.setDisabled(True)
        hbox.addWidget(QLabel('Recv::'))
        hbox.addWidget(self.leRecvNbr)
        hbox.addWidget(QLabel('Send::'))
        hbox.addWidget(self.leSendNbr)
        hbox.addWidget(self.btnResetArduin)
        hbox.addWidget(self.btnSend)
        
        vbox.addLayout(hbox)
        
        self.setLayout(vbox)
        
        self.serial = AsSerial()
    
    def on_message_received(self,message):
        pass
        
    def on_btnOpenClose_clicked(self):
        if(self.btnOpenClose.text()=='Open'):
            settings = {}
            settings['port'] = str(self.cmdPorts.currentText())
            settings["baund"] = int(str(self.cmdBaudrate.currentText()),10)
            settings["bytesize"] = int(str(self.cmdData.currentText()),10)
            settings["parity"]=str(self.cmdParity.currentText())[:1]
            settings["stopbits"]=float(str(self.cmdStop.currentText()))
            settings["timeout"] = 100
            self.serial.recv_msg.connect(self.on_message_received)
            ret, msg = self.serial.open(settings)
            print ret,msg
            self.btnOpenClose.setText('Close')
        else:
            self.btnOpenClose.setText('Open')
    
    def on_btnClearHistory_clicked(self):
        pass         