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
 
from AS import *

__all__ = ['aws']

dev__ = asdev()
index__=0

class aws():
    def server(uri,port):
        device='websock%d'%(index__)
        option='%s\0%d\0%d'%(uri,port,1)
        index__+=1
        return dev__.open(device.encode('utf-8'), option.encode('utf-8'))

    def client(uri,port):
        device='websock%d'%(index__)
        option='%s\0%d\0%d'%(uri,port,0)
        index__+=1
        return dev__.open(device.encode('utf-8'), option.encode('utf-8'))

    def call(fd,api,verb,obj):
        ds = 'c%s\0%s\0%s'%(api,verb,obj)
        return dev__.write(fd,ds.encode('utf-8'),-1)

    def pend(fd):
        while(True):
            len,data= dev__.read(fd)
    
            if(len):
                break
            else:
                data = None

        return len,data

    def reply_okay(s,msg,robj):
        ds = 'r%s\0%s\0%s\0%s'%('okay', robj, msg.param, msg.msg)
        return dev__.write(fd,ds.encode('utf-8'),-1)

    def reply_fail(s,msg,robj):
        ds = 'r%s\0%s\0%s\0%s'%('fail', robj, msg.param, msg.msg)
        return dev__.write(fd,ds.encode('utf-8'),-1)

    def close(fd):
        dev__.close(fd)

if __name__ == '__main__':
    aw = aws()

    s = aw.server('127.0.0.1',8080)
    c = aw.client('127.0.0.1',8080)
    aw.call(c,'hello','ping','{}')
    msg = pend(s)
    
