__lic__ = '''
/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2017  AS <parai@foxmail.com>
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

__all__ = ['doip']

import socket

class doip():
    def __init__(self,uri,port):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((uri, port))

    def transmit(self,payloadType,payload):
        s = '%c%c'%(2,(~2)&0xFF) # version
        s += '%c%c'%((payloadType>>8)&0xFF,payloadType&0xFF)
        length = len(payload)
        s += '%c%c%c%c'%((length>>24)&0xFF,(length>>16)&0xFF,(length>>8)&0xFF,(length>>0)&0xFF)
        for i in payload:
            try:
                s += '%c'%(ord(i)&0xFF)
            except TypeError:
                s += '%c'%(i&0xFF)

        self.sock.send(s.encode('utf-8'))
        return self.sock.recv(4096)

if __name__ == '__main__':
    dip = doip('172.18.0.200',8989)
    print(dip.transmit(1,{0x23,0x34}))
