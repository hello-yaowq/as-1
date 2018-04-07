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
try:
    from .can import *
except:
    from can import *
import time


__all__ = ['cantp']

class J1939Tp():
    def __init__(self):
        pass

if(__name__ == '__main__'):
    can_open(0,'socket',1,1000000)
    can_write(0,0x754,[0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77])
