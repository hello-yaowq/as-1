from PyQt4 import QtCore, QtGui
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from serial import Serial
from time import sleep
from time import ctime
from binascii import hexlify, unhexlify
import sys

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class AsSerial(QThread):
    recv_msg = QtCore.pyqtSignal(str)
    def __init__(self,parent=None):
        super(QThread, self).__init__(parent)
        
    def open(self, settings):
        self.__terminate = False
        try:
            self.serial = Serial(settings['port'], settings['baund'], settings['bytesize'],
                    settings['parity'], settings['stopbits'], settings['timeout'])
            self.serial.flushInput()
            self.serial.flushOutput()
        except:
            return (False, "%s"%(sys.exc_info()[1]))
        
        self.start()
        return (True, 'success')
    def resetArduino(self):
        self.serial.setDTR(0)
        sleep(0.1)
        self.serial.setDTR(1)
        
    def terminate(self):
        self.__terminate = True
        
    def send(self, data):
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
                    data = '%s%s' % (data, self.serial.read(n))
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
    toVisualHex = lambda self,data: ' '.join([hexlify(c) for c in data]).upper()
    toHex = lambda self,data: ''.join([unhexlify(data[i:i+2]) for i in xrange(0, len(data), 2)])
    def __init__(self, parent=None):
        super(QWidget, self).__init__(parent)
        self.creGui()
        self.flags = {'opened':False}
        
    def creGui(self):
        vbox = QVBoxLayout()
        grid = QtGui.QGridLayout()
        grid.addWidget(QLabel('Port:'), 0, 0)
        self.cmdPorts = QtGui.QComboBox()
        #self.cmdPorts.addItems(QStringList(['COM0','COM1','COM2','COM3','COM4','COM5','COM6','COM7','COM8','COM9']))
        for i in ['COM0','COM1','COM2','COM3','COM4','COM5','COM6','COM7','COM8','COM9']:
            self.cmdPorts.addItem(i)
        grid.addWidget(self.cmdPorts, 0, 1)
        self.cmdPorts.setCurrentIndex(6)
        self.cmdPorts.setEditable(True)
        
        grid.addWidget(QLabel('Baudrate:'), 0, 2)
        self.cmdBaudrate = QtGui.QComboBox()
        #self.cmdBaudrate.addItems(QStringList(['4800','7200','9600','14400','19200','38400','57600','115200','128000']))
        for i in ['4800','7200','9600','14400','19200','38400','57600','115200','128000']:
            self.cmdBaudrate.addItem(i)
        grid.addWidget(self.cmdBaudrate, 0, 3)
        self.cmdBaudrate.setCurrentIndex(7)
        self.cmdBaudrate.setEditable(True) 
        
        grid.addWidget(QLabel('Data:'), 0, 4)
        self.cmdData = QtGui.QComboBox()
        #self.cmdData.addItems(QStringList(['8','7','6','5','4']))
        for i in ['8','7','6','5','4']:
            self.cmdData.addItem(i)
        grid.addWidget(self.cmdData, 0, 5)
        self.cmdData.setCurrentIndex(0) 
        
        grid.addWidget(QLabel('Parity:'), 0, 6)
        self.cmdParity = QtGui.QComboBox()
        #self.cmdParity.addItems(QStringList(['Odd','Even','Flag','Space','None']))
        for i in ['Odd','Even','Flag','Space','None']:
            self.cmdParity.addItem(i)
        grid.addWidget(self.cmdParity, 0, 7)
        self.cmdParity.setCurrentIndex(4)
        
        grid.addWidget(QLabel('Stop:'), 0, 8)
        self.cmdStop = QtGui.QComboBox()
        #self.cmdStop.addItems(QStringList(['1','1.5','2']))
        for i in ['1','1.5','2']:
            self.cmdStop.addItem(i)        
        grid.addWidget(self.cmdStop, 0, 9)
        self.cmdStop.setCurrentIndex(0)
        
        grid.addWidget(QLabel('FlowControl:'), 0, 10)
        self.cmdFlowControl = QtGui.QComboBox()
        #self.cmdFlowControl.addItems(QStringList(['Xon / Xoff','Hardware','None']))
        for i in ['Xon / Xoff','Hardware','None']:
            self.cmdFlowControl.addItem(i)          
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
        self.tbHistory.setStyleSheet(_fromUtf8("background-color: rgb(36, 36, 36);\ncolor: rgb(12, 190, 255);"))
        vbox.addWidget(self.tbHistory)
       
        self.teInput = QTextEdit()
        self.teInput.setStyleSheet(_fromUtf8("background-color: rgb(36, 36, 36);\ncolor: rgb(12, 190, 255);"))
        vbox.addWidget(self.teInput)
        
        hbox = QHBoxLayout()
        self.btnResetArduin = QPushButton('ResetArduino')
        self.btnSend = QPushButton('Send')
        self.lcdRecvNbr = QLCDNumber()
        self.lcdSendNbr = QLCDNumber()
        self.lcdRecvNbr.display(0)
        self.lcdSendNbr.display(0)
        hbox.addWidget(QLabel('Recv::'))
        hbox.addWidget(self.lcdRecvNbr)
        hbox.addWidget(QLabel('Send::'))
        hbox.addWidget(self.lcdSendNbr)
        hbox.addWidget(self.btnResetArduin)
        hbox.addWidget(self.btnSend)
        self.connect(self.btnSend,SIGNAL('clicked()'),self.on_btnSend_clicked)
        
        vbox.addLayout(hbox)
        
        self.setLayout(vbox)
        
        self.serial = AsSerial()
    
    def checkData(self, data):
        if data == '':
            return (False, 'data can\'t be null')
    
        errch, msg = None, 'success'
        if(self.rbHex.isChecked()):
            data = ''.join(data.split())
            if len(data) % 2 != 0:
                errch, msg = True, 'HEX mode, data length should be odd'
            else:
                for ch in data.upper():
                    if not ('0' <= ch <= '9' or 'A' <= ch <= 'F'):
                        errch, msg = ch, 'invalid char in HEX mode'
                        break           
        return ((not errch), msg)
     
    def onSendData(self, data=None):
        if(not data): data = self.teInput.toPlainText()
        if(self.rbHex.isChecked()):
            data = ''.join(data.split())
            data = ' '.join([data[i:i+2] for i in xrange(0, len(data), 2)]).upper()
        else:
            data = data.replace('\n', '<br/>')
        self.tbHistory.append('<b>Send</b> @%s<br/><font color="white">%s</font><br/><br/>'
                                    % (ctime(), data))
        self.teInput.clear()
        bytes = self.rbAscii.isChecked() and len(data) or len(data) / 2
        self.lcdSendNbr.display(self.lcdSendNbr.intValue() + bytes)
             
    def on_btnSend_clicked(self):
        if(self.flags['opened']==False):
            QtGui.QMessageBox.information(self, 'Tips', 'Please open COM fistly.')
            return
        data = self.teInput.toPlainText().toUtf8().data()
        ret, msg = self.checkData(data)
        if not ret:
            QtGui.QMessageBox.critical(self, 'Error', msg)
            return
        
        self.onSendData(data)
        if(self.rbHex.isChecked()):
            data = self.toHex(''.join(data.split()))
        self.serial.send(data)
    def on_message_received(self,data):
        bytes = len(data)
        if(self.rbHex.isChecked()):
            data = self.toVisualHex(data)
        else:
            data = data.replace('\n', '<br/>')
        self.tbHistory.append('<b>Recv</b> @%s<br/><font color="yellow">%s</font><br/><br/>'
                                    % (ctime(), data))
        self.lcdRecvNbr.display(self.lcdRecvNbr.intValue() + bytes)
        
    def on_btnOpenClose_clicked(self):
        if(self.btnOpenClose.text()=='Open'):
            settings = {}
            settings['port'] = str(self.cmdPorts.currentText())
            settings['baund'] = int(str(self.cmdBaudrate.currentText()),10)
            settings['bytesize'] = int(str(self.cmdData.currentText()),10)
            settings['parity']=str(self.cmdParity.currentText())[:1]
            settings['stopbits']=float(str(self.cmdStop.currentText()))
            settings['timeout'] = 100
            self.serial.recv_msg.connect(self.on_message_received)
            ret, msg = self.serial.open(settings)
            if(ret==False): # open failed
                print(msg)
                QMessageBox.critical(self, 'Error', msg)
            else:
                self.btnOpenClose.setText('Close')
                self.flags['opened'] = True
        else:
            self.serial.terminate()
            self.serial.close()
            self.btnOpenClose.setText('Open')
            self.flags['opened'] = False
    
    def on_btnClearHistory_clicked(self):
        pass         