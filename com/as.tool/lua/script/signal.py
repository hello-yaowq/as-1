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

import os,sys,time
import threading
from bitarray import bitarray

__all__ = ['Network']

# big endian bits map
_bebm = []

for i in range(8):
    for j in range(8):
        _bebm.append(i*8 + 7-j)

class Sdu():
    def __init__(self, length):
        self.data = []
        for i in range(0,length):
            self.data.append(0x55)

    def set(self, start, size, value):
        # for big endian only
        rBit = size-1
        nBit = _bebm.index(start)
        wByte = 0
        wBit = 0
        for i in range(size):
            wBit = _bebm[nBit]
            wByte = int(wBit/8)
            wBit  = wBit%8
            if(value&(1<<rBit) != 0):
                self.data[wByte] |= 1<<wBit
            else:
                self.data[wByte] &= ~(1<<wBit)
            nBit += 1
            rBit -= 1

    def __str__(self):
        cstr = '[ '
        for b in self.data:
            cstr += '%02X, '%(b)
        cstr += ']'
        return cstr

class Signal():
    def __init__(self, sg):
        self.sg = sg
        self.mask = (1<<sg['size'])-1
        self.set_value(0xA55A5AA5)

    def set_value(self, v):
        self.value = v&self.mask

    def __str__(self):
        return str(self.sg)

    def __getitem__(self, key):
        return  self.sg[key]

class Message():
    def __init__(self, msg):
        self.msg = msg
        self.sgs = {}
        self.sdu = Sdu(8)
        if('period' in msg):
            self.period = msg['period']
        else:
            self.period = 1000
        self.timer = time.time()
        for sg in msg['sgList']:
            sg = sg['sg']
            self.sgs[sg['name']] = Signal(sg)

    def ProcessTX(self):
        elapsed = time.time() - self.timer
        if(self.period <= elapsed*1000):
            self.timer = time.time()
            for sig in self:
                self.sdu.set(sig['start'], sig['size'], sig.value)
            print('TX', self.msg['name'], self.sdu)

    def ProcessRX(self):
        pass

    def Process(self):
        if(self.msg['node'] == 'AS'):
            self.ProcessTX()
        else:
            self.ProcessRX()

    def __str__(self):
        return str(self.msg)

    def __iter__(self):
        for key,sig in self.sgs.items():
            yield sig

    def __getitem__(self, key):
        return  self.sgs[key]

class Network(threading.Thread):
    def __init__(self, dbcf):
        threading.Thread.__init__(self)
        dbc = self.parseCANDBC(dbcf)
        self.msgs = {}
        for msg in dbc['boList']:
            msg = msg['bo']
            self.msgs[msg['name']] = Message(msg)
        self.start()

    def stop(self):
        self.is_running = False

    def run(self):
        self.is_running = True
        while(self.is_running):
            for msg in self:
                msg.Process()
            time.sleep(0.001)

    def __iter__(self):
        for key,msg in self.msgs.items():
            yield msg

    def __getitem__(self, key):
        return  self.msgs[key]

    def parseCANDBC(self, dbc):
        pydbc = os.path.abspath('../py.can.database.access/ascc')
        assert(os.path.exists(pydbc))
        sys.path.append(pydbc)
        import cc.ascp as ascp
        return ascp.parse(dbc)

if(__name__ == '__main__'):
    nt = Network(sys.argv[1])
    time.sleep(5)
    nt.stop()

