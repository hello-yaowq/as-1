__lic__ = '''
/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2018  AS <parai@foxmail.com>
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

# REF: https://github.com/GENIVI/vsomeip/wiki/vsomeip-in-10-minutes
# CMD: route add -nv 224.224.224.245 dev eth0
# CMD: route add -nv 224.224.224.246 dev wifi0
import socket
import struct

VSOMEIP_SD_SERVICE=0xFFFF
VSOMEIP_SD_METHOD =0x8100

class Message():
    def __init__(self, data=None):
        if(data != None):
            self.header = data[:16]
            self.payload = data[16:]
        else:
            self.header = [0 for i in range(16)]
            self.payload = []
            self.set_protocol(1)

    def is_malformed(self):
        ret = False
        if((self.get_length()+4) != (len(self.header)+len(self.payload))):
            ret = True
        return ret

    def set_message(self, message):
        self.header[0] = (message>>24)&0xFF
        self.header[1] = (message>>16)&0xFF
        self.header[2] = (message>> 8)&0xFF
        self.header[3] = (message>> 0)&0xFF

    def get_message(self):
        return (self.header[0]<<24)+(self.header[1]<<16)+(self.header[2]<<8)+self.header[3]

    def set_service(self, service):
        self.header[0] = (service>>8)&0xFF
        self.header[1] = (service>>0)&0xFF

    def get_service(self):
        return (self.header[0]<<8)+self.header[1]

    def set_method(self, method):
        self.header[2] = (method>>8)&0xFF
        self.header[3] = (method>>0)&0xFF

    def set_event(self, event):
        self.header[2] = ((event>>8)|0x80)&0xFF
        self.header[3] = (event>>0)&0xFF

    def get_method(self):
        return (self.header[2]<<8)+self.header[3]

    def set_length(self, length):
        self.header[4] = (length>>24)&0xFF
        self.header[5] = (length>>16)&0xFF
        self.header[6] = (length>> 8)&0xFF
        self.header[7] = (length>> 0)&0xFF

    def get_length(self):
        return (self.header[4]<<24)+(self.header[5]<<16)+(self.header[6]<<8)+self.header[7]

    def set_request(self, request):
        self.header[8]  = (request>>24)&0xFF
        self.header[9]  = (request>>16)&0xFF
        self.header[10] = (request>> 8)&0xFF
        self.header[11] = (request>> 0)&0xFF

    def get_request(self):
        return (self.header[8]<<24)+(self.header[9]<<16)+(self.header[10]<<8)+self.header[11]

    def set_client(self, client):
        self.header[8] = (client>>8)&0xFF
        self.header[9] = (client>>0)&0xFF

    def get_client(self):
        return (self.header[8]<<8)+self.header[9]

    def set_session(self, session):
        self.header[10] = (session>>8)&0xFF
        self.header[11] = (session>>0)&0xFF

    def get_session(self):
        return (self.header[10]<<8)+self.header[11]

    def set_protocol(self, protocol):
        self.header[12] = protocol

    def get_protocol(self):
        return self.header[12]

    def set_interface(self, interface):
        self.header[13] = interface

    def get_interface(self):
        return self.header[13]

    def set_type(self, type):
        self.header[14] = type

    def get_type(self):
        return self.header[14]

    def set_return(self, ret):
        self.header[15] = ret

    def get_return(self):
        return self.header[15]

    def set_payload(self, payload):
        self.payload = payload
        self.set_length(len(payload)+12)

    def get_payload(self):
        return self.payload

    @property
    def data(self):
        return bytes(self.header+self.payload)

    def tostr_header(self):
        cstr = 'SOMEIP Message:\n'
        cstr += ' Service=0x%04X, Method=0x%04X\n'%(self.get_service(), self.get_method())
        cstr += ' Length=0x%08X\n'%(self.get_length())
        cstr += ' Client=0x%04X, Session=0x%04X\n'%(self.get_client(), self.get_session())
        cstr += ' ProtocolVersion=%d, InterfaceVersion=%d, MessageType=0x%02X, ReturnCode=%d\n'%(
                self.get_protocol(), self.get_interface(), self.get_type(), self.get_return())
        return cstr

    def __str__(self):
        cstr = self.tostr_header()
        if(self.get_length()):
            cstr += ' Payload=%s'%(self.get_payload())
        return cstr

class SDMessage(Message):
    def __init__(self, data=None):
        Message.__init__(self,data)
        if(data != None):
            self.is_malformed()
    
    def is_malformed(self):
        cstr=''
        if(self.get_service() != VSOMEIP_SD_SERVICE):
            cstr+=' serivce is 0x%04X != 0x%04X'%(self.get_service(), VSOMEIP_SD_SERVICE)
        if(self.get_method() != VSOMEIP_SD_METHOD):
            cstr+=' method is 0x%04X != 0x%04X'%(self.get_method(), VSOMEIP_SD_METHOD)
        numberOfEntry = int(self.get_entry_length()/16)
        for index in range(numberOfEntry):
            if(self.get_entry_type(index) not in [0,1,6,7]):
                cstr += ' entry %d wrong type %d'%(index, self.get_entry_type(index))
        if(cstr != ''):
            print('malformed SD message:\n%s'%(cstr))
            return True
        else:
            return False

    def get_flags(self):
        return self.payload[0]

    def get_reserved(self):
        return (self.payload[1]<<16)+(self.payload[2]<<8)+self.payload[3]

    def get_entry_length(self):
        return (self.payload[4]<<24)+(self.payload[5]<<16)+(self.payload[6]<<8)+self.payload[7]

    def get_entry_type(self, index):
        return self.payload[8+index*16+0]

    def get_entry_index_1st_options(self, index):
        return self.payload[8+index*16+1]

    def get_entry_index_2nd_options(self, index):
        return self.payload[8+index*16+2]

    def get_entry_of_opt_1(self, index):
        return (self.payload[8+index*16+3]>>4)&0xF

    def get_entry_of_opt_2(self, index):
        return self.payload[8+index*16+3]&0xF

    def get_entry_service(self, index):
        return (self.payload[8+index*16+4]<<8)+self.payload[8+index*16+5]

    def get_entry_instance(self, index):
        return (self.payload[8+index*16+6]<<8)+self.payload[8+index*16+7]

    def get_entry_major(self, index):
        return self.payload[8+index*16+8]

    def get_entry_ttl(self, index):
        return (self.payload[8+index*16+9]<<16)+(self.payload[8+index*16+10]<<8)+self.payload[8+index*16+11]

    def get_entry_reserved(self, index):
        return (self.payload[8+index*16+12]<<4)+((self.payload[8+index*16+13]>>4)&0xF)

    def get_entry_counter(self, index):
        return (self.payload[8+index*16+13]&0xF)

    def get_entry_enent_group(self, index):
        return (self.payload[8+index*16+14]<<8)+self.payload[8+index*16+15]

    def get_entry_minor(self, index):
        return (self.payload[8+index*16+12]<<24)+(self.payload[8+index*16+13]<<16)+(self.payload[8+index*16+14]<<8)+self.payload[8+index*16+15]

    def __str__(self):
        cstr = self.tostr_header()
        #cstr += ' Payload=%s\n'%(self.get_payload())
        cstr += ' flags=0x%02X, reserved=0x%06X\n'%(self.get_flags(), self.get_reserved())
        cstr += ' entry length=0x%08X\n'%(self.get_entry_length())
        numberOfEntry = int(self.get_entry_length()/16)
        for index in range(numberOfEntry):
            cstr += ' Entry %d:\n'%(index)
            cstr += '  type=0x%02X, index 1st options=0x%02X, index 2nd options=0x%02X, # of opt 1=0x%X, # of opt 2=0x%X\n'%(
                self.get_entry_type(index), self.get_entry_index_1st_options(index), self.get_entry_index_2nd_options(index),
                self.get_entry_of_opt_1(index), self.get_entry_of_opt_2(index))
            cstr += '  service=0x%04X, instance=0x%04X\n'%(self.get_entry_service(index), self.get_entry_instance(index))
            cstr += '  major=0x%02X, ttl=0x%06X\n'%(self.get_entry_major(index), self.get_entry_ttl(index))
            if((self.get_entry_type(index)==6) or (self.get_entry_type(index)==7)):
                cstr += '  minor=0x%08X\n'%(self.get_entry_minor(index))
            else:
                cstr += '  minor=0x%08X\n'%(self.get_entry_minor(index))
        return cstr

class vsomeip():
    def __init__(self, url='192.168.1.100', port=30509, 
                 sdurl='224.244.224.246', sdport=30490,
                 udp=True):
        if(udp):
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self.sock.connect((url, port))
        else:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.connect((url, port))

        self.sdsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.sdsock.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
        self.sdsock.bind(('', sdport))
        mreq = struct.pack("4sl", socket.inet_aton(sdurl), socket.INADDR_ANY)
        self.sdsock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

    def find_service(self):
        data = self.sdsock.recv(4096)
        return SDMessage(data)

    def request_service(self, data):
        self.sock.send(data)
        return self.receive()

    def receive(self):
        data = self.sock.recv(4096)
        return Message(data)

if(__name__ == '__main__'):
    import sys
    someip = vsomeip()
    msg = Message()
    msg.set_service(0x1234)
    msg.set_method(0x5678)
    msg.set_client(0)
    msg.set_session(0x053E)
    msg.set_payload([i for i in range(10)])
    print('TX', msg)
    #msg = someip.request_service(msg.data)
    #msg = receive()
    print('ACK', msg)
    msg = someip.find_service()
    print('RX', msg)
    msg = someip.find_service()
    print('RX', msg)


