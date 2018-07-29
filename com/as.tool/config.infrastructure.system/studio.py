'''/**
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
import sys,os
from arxml.Argui import *
from arxml.Arxml import *
from argen.ArGen import *
import xml.etree.ElementTree as ET

__all__ = ['easySAR']

gDefault_GEN = '..'

class easyDockWidget(QDockWidget):
    isClosed = False
    def __init__(self,title,parent=None):
        QDockWidget.__init__(self,title,parent)
        self.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea|QtCore.Qt.RightDockWidgetArea)  
        #self.setFeatures(QDockWidget.DockWidgetClosable|QDockWidget.DockWidgetMovable)
    def closeEvent(self,event):
        self.isClosed = True

class easySARGui(QMainWindow):
    def __init__(self):
        global gDefault_GEN
        self.modules = []
        self.docks   = []
        self.actions = []
        self.pdir = ''
        
        QMainWindow.__init__(self, None)
        self.setWindowTitle('easy OpenSAR Studio( parai@foxmail.com ^_^ @ %s)'%(gDefault_GEN));
        self.setMinimumSize(800, 400)
        
        self.creStatusBar()
        self.systemDescriptor = ET.parse('./arxml/easySAR.arxml').getroot()
        self.creMenu()
        # try to open a default configuration file
        self.mOpen(gDefault_GEN)
        
        self.showMaximized()

    def creMenu(self):
        # File
        tMenu=self.menuBar().addMenu(self.tr('File'))
        ## Open Ctrl+O  
        sItem=QAction(self.tr('Open'),self) 
        sItem.setShortcut('Ctrl+O'); 
        sItem.setStatusTip('Open a OpenSAR configure file.')
        sItem.triggered.connect(self.mOpen)
        tMenu.addAction(sItem) 
        ## Save Ctrl+S
        sItem=QAction(self.tr('Save'),self) 
        sItem.setShortcut('Ctrl+S'); 
        sItem.setStatusTip('Save the OpenSAR configure file.')
        sItem.triggered.connect(self.mSave) 
        tMenu.addAction(sItem)  
        ## Save Ctrl+G
        sItem=QAction(self.tr('Generate'),self) 
        sItem.setShortcut('Ctrl+G'); 
        sItem.setStatusTip('Convert the OpenSAR configure file to C Code.')
        sItem.triggered.connect(self.mGen)  
        tMenu.addAction(sItem)
        # easySAR Module
        tMenu=self.menuBar().addMenu(self.tr('Module'))
        for desc in self.systemDescriptor:
            sItem=ArgAction(self.tr(desc.tag),self) 
            sItem.setStatusTip('Open easy%s console.'%(desc.tag)) 
            tMenu.addAction(sItem)
            module = ArgModule(Arxml(desc),self)
            self.modules.append(module)
            self.docks.append(None)
        # some plugins, uncommon stuff of studio
        tMenu=self.menuBar().addMenu(self.tr('Plugin'))
        sItem=QAction(self.tr('Import Vector CAN DBC Sinals'),self) 
        sItem.setStatusTip('Import Vector CAN DBC Sinals.')
        sItem.triggered.connect(self.mImportVectorCANDBCSignals)
        tMenu.addAction(sItem)

    def parseCANDBC(self, dbc):
        pydbc = os.path.abspath('../py.can.database.access/ascc')
        assert(os.path.exists(pydbc))
        sys.path.append(pydbc)
        import cc.ascp as ascp
        return ascp.parse(dbc)

    def removeXml(self, L, R):
        rl = []
        for r in L:
            if(r.attrib['Name'] in R):
                rl.append(r)
        for r in rl:
            L.remove(r)

    def modifyEcuCbyCANDBC(self, dbc):
        self.onAction('EcuC')
        ecuc = self.getModule('EcuC')
        arxml = ecuc.toArxml()
        R = []
        for msg in dbc['boList']:
            R.append(msg['bo']['name'])
        PduList = arxml.find('PduList')
        self.removeXml(PduList, R)
        for msg in dbc['boList']:
            pdu = ET.Element('Pdu')
            pdu.attrib['Name'] = msg['bo']['name']
            pdu.attrib['Size'] = str(msg['bo']['length']*8)
            PduList.append(pdu)
        ecuc.reloadArxml(Arxml(self.systemDescriptor.find('EcuC'),arxml))

    def modifyCanIfbyCANDBC(self, dbc, bus):
        self.onAction('CanIf')
        for s in [' ','-']:
            bus = bus.replace(s,'')
        canif = self.getModule('CanIf')
        arxml = canif.toArxml()
        chlList = arxml.find('ChannelList')
        chl = None
        for c in chlList:
            if(c.attrib['Name'] == bus):
                chl = c
                break
        if(chl is None):
            chl = ET.Element('Channel')
            chl.attrib['Name']=bus
            chl.append(ET.Element('HthList'))
            chl.append(ET.Element('HrhList'))
            chl.append(ET.Element('TxPduList'))
            chl.append(ET.Element('RxPduList'))
            chlList.append(chl)
        rxList = []
        rR = []
        txList = []
        tR = []
        for msg in dbc['boList']:
            msg = msg['bo']
            if(msg['node'] == self.CAN_SELF_DBC_NODE):
                txList.append(msg)
                tR.append(msg['name'])
            else:
                rxList.append(msg)
                rR.append(msg['name'])
        TxPduList = chl.find('TxPduList')
        RxPduList = chl.find('RxPduList')
        self.removeXml(TxPduList, tR)
        self.removeXml(RxPduList, rR)
        def appendPduList(pduList, xList):
            for msg in xList:
                pdu = ET.Element('Pdu')
                pdu.attrib['Name'] = msg['name']
                pdu.attrib['EcuCPduRef'] = msg['name']
                pdu.attrib['DataLengthCode'] = str(msg['length'])
                pdu.attrib['Identifier'] = str(msg['id'])
                pduList.append(pdu)
        appendPduList(TxPduList, txList)
        appendPduList(RxPduList, rxList)
        canif.reloadArxml(Arxml(self.systemDescriptor.find('CanIf'),arxml))

    def modifyPduRbyCANDBC(self, dbc):
        self.onAction('PduR')
        pdur = self.getModule('PduR')
        arxml = pdur.toArxml()
        R = []
        for msg in dbc['boList']:
            R.append(msg['bo']['name'])
        RoutineList = arxml.find('RoutineList')
        self.removeXml(RoutineList, R)
        for msg in dbc['boList']:
            msg = msg['bo']
            src = ET.Element('Source')
            src.attrib['Name'] = msg['name']
            src.attrib['PduRef'] = msg['name']
            if(msg['node'] == self.CAN_SELF_DBC_NODE):
                src.attrib['Module'] = 'Com'
            else:
                src.attrib['Module'] = 'CanIf'
            dlist = ET.Element('DestinationList')
            dst = ET.Element('Destination')
            dst.attrib['Name'] = msg['name']
            dst.attrib['PduRef'] = msg['name']
            if(msg['node'] == self.CAN_SELF_DBC_NODE):
                dst.attrib['Module'] = 'CanIf'
            else:
                dst.attrib['Module'] = 'Com'
            dlist.append(dst)
            src.append(dlist)
            RoutineList.append(src)
            pdur.reloadArxml(Arxml(self.systemDescriptor.find('PduR'),arxml))

    def modifyCombyCANDBC(self, dbc):
        self.onAction('Com')
        com = self.getModule('Com')
        arxml = com.toArxml()
        R = []
        for msg in dbc['boList']:
            R.append(msg['bo']['name'])
        IPduList = arxml.find('IPduList')
        self.removeXml(IPduList, R)
        for msg in dbc['boList']:
            msg = msg['bo']
            ipdu = ET.Element('IPdu')
            ipdu.attrib['Name'] = msg['name']
            ipdu.attrib['PduRef'] = msg['name']
            if(msg['node'] == self.CAN_SELF_DBC_NODE):
                ipdu.attrib['Direction'] = 'SEND'
            else:
                ipdu.attrib['Direction'] = 'RECEIVE'
            slist = ET.Element('SignalList')
            for sg in msg['sgList']:
                sg = sg['sg']
                sig = ET.Element('Signal')
                sig.attrib['Name'] = sg['name']
                sig.attrib['Endianess'] = 'BIG_ENDIAN'
                sig.attrib['StartBit'] = str(sg['start'])
                sig.attrib['Size'] = str(sg['size'])
                try: 
                    sig.attrib['InitialValue'] = str(sg['init']) 
                except KeyError:
                    pass
                slist.append(sig)
            ipdu.append(slist)
            IPduList.append(ipdu)
        com.reloadArxml(Arxml(self.systemDescriptor.find('Com'),arxml))

    def mImportVectorCANDBCSignals(self):
        self.CAN_SELF_DBC_NODE = 'AS'
        if(os.getenv('CAN_SELF_DBC_NODE') is not None):
            self.CAN_SELF_DBC_NODE = os.getenv('CAN_SELF_DBC_NODE')
        dbc,_ = QFileDialog.getOpenFileName(None,'Open Vector CAN DBC',
                                '', '*.dbc','*.dbc',
                                QFileDialog.DontResolveSymlinks)
        if(os.path.exists(dbc)):
            bus = os.path.basename(dbc)[:-4]
            dbc = self.parseCANDBC(dbc)
            self.modifyEcuCbyCANDBC(dbc)
            self.modifyCanIfbyCANDBC(dbc,bus)
            self.modifyPduRbyCANDBC(dbc)
            self.modifyCombyCANDBC(dbc)
        self.mSave('-import-dbc')
        print('!!!Importing Done!!!')

    def getModule(self, name):
        for module in self.modules:
            if(name == module.tag):
                return module
        return None

    def getURL(self,ref):
        ar = ET.Element('AUTOSAR')
        for module in self.modules:
            ar.append(module.toArxml())
        return ArxmlGetURL(ar,ref)

    def mOpen(self,default=None):
        if(default == None):
            self.pdir = QFileDialog.getExistingDirectory(None,'Open OpenSAR Config',gDefault_GEN,QFileDialog.DontResolveSymlinks)
            if(self.pdir == ''):
                return
        else:
            self.pdir = default
        wfxml = '%s/autosar.arxml'%(self.pdir)
        if(os.path.exists(wfxml)==False):
            return
        root = ET.parse(wfxml).getroot();
        for module in self.modules:
            if(root.find(module.tag) != None):
                module.reloadArxml(Arxml(self.systemDescriptor.find(module.tag),
                                         root.find(module.tag)))
                self.onAction(module.tag)
        if(default == None):
            QMessageBox(QMessageBox.Information, 'Info', 
                        'Open OpenSAR Configuration arxml Successfully !').exec_();
    def mSave(self, alt=''):
        if(self.pdir == ''):
            self.pdir = QFileDialog.getExistingDirectory(None,'Save OpenSAR Configuration',gDefault_GEN,QFileDialog.DontResolveSymlinks)
        if(self.pdir == ''):
            return
        wfxml = '%s/autosar%s.arxml'%(self.pdir,alt)
        ROOT = ET.Element('AUTOSAR')
        for module in self.modules:
            ROOT.append(module.toArxml())
        tree = ET.ElementTree(ROOT)
        tree.write(wfxml, encoding="utf-8", xml_declaration=True);
        fp = open(wfxml,'r')
        content = fp.read()
        fp.close()
        fp = open(wfxml,'w')
        fp.write(content.replace('>','>\n'))
        fp.close()
        print('!!!Saving to %s done!!!'%(wfxml))
        QMessageBox(QMessageBox.Information, 'Info', 
                    'Save OpenSAR Configuration arxml Successfully !').exec_();
    def mGen(self):
        if(self.pdir == ''):
            QMessageBox(QMessageBox.Information, 'Info', 
                        'Open or Configure a Workspace first !').exec_();
            return
        for module in self.modules:
            ArGen(module.toArxml(),self.pdir, False)
        QMessageBox(QMessageBox.Information, 'Info', 
                        'Generate OpenSAR Configuration C Code Successfully !\n<%s>\n'%(self.pdir)).exec_();
    def onAction(self,text):
        I = 0
        for module in self.modules:
            if(text == module.tag):
                break
            else:
                I += 1
        if(I >= len(self.modules)):
            return
        
        if(self.docks[I]==None):
            self.docks[I] = easyDockWidget(self.modules[I].tag, self)  
            self.docks[I].setWidget(self.modules[I])  
            self.addDockWidget(QtCore.Qt.RightDockWidgetArea, self.docks[I])
        elif(self.docks[I].isClosed==True):
            self.docks[I] = easyDockWidget(self.modules[I].tag, self)  
            self.docks[I].setWidget(self.modules[I])  
            self.addDockWidget(QtCore.Qt.RightDockWidgetArea, self.docks[I])
        else:
            print('%s already started.'%(self.modules[I].tag))
             
        J=I-1
        if(I==0):
            J=1
        if(self.docks[J]!=None):
            self.tabifyDockWidget(self.docks[J],self.docks[I]) 
    def creStatusBar(self):
        self.statusBar = QStatusBar()
        self.setStatusBar(self.statusBar)
        self.statusBar.showMessage('easy OpenSAR Studio Platform',0)
        
def easySAR():
    qtApp = QApplication(sys.argv)
    if(os.name == 'nt'):
        qtApp.setFont(QFont('Consolas')) 
    elif(os.name == 'posix'):
        qtApp.setFont(QFont('Monospace'))
    else:
        print('unKnown platform.')
    qtGui = easySARGui()
    qtGui.show()
    qtApp.exec_()
    

if(__name__ == '__main__'):
    if(len(sys.argv)==2):
        gDefault_GEN = sys.argv[1]
    easySAR()