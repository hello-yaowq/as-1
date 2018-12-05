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
import socket
import struct

VSOMEIP_SD_SERVICE=0xFFFF
VSOMEIP_SD_METHOD =0x8100

class sdentry():
    def __init__(self, data=None):
        if(data != None):
            self.header = data[:16]
            self.payload = data[16:]
        else:
            self.header = [0 for i in range(16)]
            self.payload = []
            self.set_protocol(1)

class message():
    def __init__(self, data=None):
        if(data != None):
            self.header = data[:16]
            self.payload = data[16:]
        else:
            self.header = [0 for i in range(16)]
            self.payload = []
            self.set_protocol(1)

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
        self.set_length(len(payload))

    def get_payload(self):
        return self.payload

    @property
    def data(self):
        return bytes(self.header+self.payload)

    def __str__(self):
        cstr = 'SOMEIP Message:\n'
        cstr += ' Service=0x%04X, Method=0x%04X\n'%(self.get_service(), self.get_method())
        cstr += ' Length=0x%08X\n'%(self.get_length())
        cstr += ' Client=0x%04X, Session=0x%04X\n'%(self.get_client(), self.get_session())
        cstr += ' ProtocolVersion=%d, InterfaceVersion=%d, MessageType=0x%02X, ReturnCode=%d\n'%(
            self.get_protocol(), self.get_interface(), self.get_type(), self.get_return())
        if(self.get_length()):
            cstr += ' Payload=%s'%(self.get_payload())
        return cstr

class vsomeip():
    def __init__(self, url='172.18.0.100', port=30509, 
                 sdurl='224.244.224.245', sdport=30490,
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
        return message(data)

    def request_service(self, data):
        self.sock.send(data)
        return self.receive()

    def receive(self):
        data = self.sock.recv(4096)
        return message(data)

if(__name__ == '__main__'):
    import sys
    someip = vsomeip()
    msg = message()
    msg.set_service(0x1234)
    msg.set_method(0x5678)
    msg.set_client(1)
    msg.set_session(2)
    msg.set_payload([i for i in range(10)])
    print('TX', msg)
    #msg = someip.request_service(msg.data)
    msg = someip.find_service()
    print('RX', msg)
    msg = someip.find_service()
    print('RX', msg)


