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
import sys
#print('system version :: ',sys.version)
if (float(sys.version[:3]) < 3.0):
    import as27 as AS
else:
    import as34 as AS

__all__ = ['can_open','can_write','can_read']

__can__ = AS.can()

def can_open(busid,device,port,baudrate):
    
    return __can__.open(busid,device.encode('utf-8'),port,baudrate)

def can_write(busid,canid,data):
    '''can request write on can bus <busid>'''
    sd = ''
    for i,c in enumerate(data):
        sd += '%c'%(c&0xFF)
    while(i<8):
        sd += '%c'%(0x55) # default padding with 0x55
        i += 1
    dlc = len(data)
    return __can__.write(busid,canid,dlc,sd.encode('utf-8'))

def can_read(busid,canid):
    ''' can request read a can frame from <canid> queue of <busid>'''
    result,dlc,cstr= __can__.read(busid,canid)
    
    if(result):
        data = []
        for i in range(len(cstr)>>1):
            data.append(int(cstr[2*i:2*i+2],16))
        assert(dlc == len(data))
    else:
        data = None

    return result,data

    