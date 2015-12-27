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

import AS
import time

ISO15765_TPCI_MASK =  0x30
ISO15765_TPCI_SF = 0x00         #/* Single Frame */
ISO15765_TPCI_FF = 0x10         #/* First Frame */
ISO15765_TPCI_CF = 0x20         #/* Consecutive Frame */
ISO15765_TPCI_FC = 0x30         #/* Flow Control */
ISO15765_TPCI_DL = 0x7          #/* Single frame data length mask */
ISO15765_TPCI_FS_MASK = 0x0F    #/* Flow control status mask */


ISO15765_FLOW_CONTROL_STATUS_CTS    =    0
ISO15765_FLOW_CONTROL_STATUS_WAIT   =    1
ISO15765_FLOW_CONTROL_STATUS_OVFLW  =    2

CANTP_ST_IDLE = 0
CANTP_ST_START_TO_SEND = 1
CANTP_ST_SENDING = 2
CANTP_ST_WAIT_FC = 3
CANTP_ST_WAIT_CF = 4
CANTP_ST_SEND_CF = 5
CANTP_ST_SEND_FC = 6

cfgSTmin = 10
cfgBS    = 8
cfgPadding = 0x55

class cantp():
    def __init__(self,can,canbus,rxid,txid,padding=0x55):
        self.can = can
        self.canbus  = canbus
        self.rxid = rxid
        self.txid = txid
        self.padding = padding
    
    def __sendFF__(self,request):
        length = len(request)
        data = '%c'%(ISO15765_TPCI_SF | (length&0x0F))
        for i,c in enumerate(request):
            data += '%c'%(c&0xFF)
        i += 2
        while(i<8):
            data += '%c'%(self.padding)
            i += 1
        return self.can.write(self.canbus,self.txid,8,data.encode('utf-8'))
        
    def transmit(self,request):
        if(len(request) < 7):
            return self.__sendFF__(request)
    
    def __str2ary__(self,cstr):
        data = []
        for i in range(len(cstr)>>1):
            data.append(int(cstr[2*i:2*i+2],16))
            
        return data

    def waitRF(self):
        ercd = False
        timeout = 0
        while ( (timeout < 1000000) and (ercd == False)): # 1s timeout
            result,dlc,data= self.can.read(self.canbus,self.rxid)
            if(True == result):
                data = self.__str2ary__(data)
                ercd = True
                break
            else:
                time.sleep(0.00001) # sleep a while 
                timeout += 1
        
        if (False == ercd):
            print("cantp timeout when receiving a frame! elapsed time = %s ms"%(timeout))
  
        return ercd,data
   
    def receive(self):
        ercd,data = self.waitRF()
        print(ercd,data)




if(__name__ == '__main__'):
    # open COM4
    can = AS.can()
    can.open(0,'serial'.encode('utf-8'),3,115200)
    tp  = cantp(can,0,0x732,0x731)
    tp.transmit([0x10,0x03])
    tp.receive()