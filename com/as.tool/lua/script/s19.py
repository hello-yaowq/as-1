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
import sys,os

__all__ = ['s19']

class s19:
    def __init__(self,file=None):
        self.file = file
        self.data = []
        if(file != None):self.parse(file)

    def append(self,address,bytes):
        sz = len(bytes)
        alen = len(self.data)
        if(alen>0):
            ss = self.data[alen-1]
        else:
            ss  = {'address':address,'size':0,'data':[]}
            self.data.append(ss)
        
        if(address == ss['address']+ss['size']):
            # append to this ss array, same section
            pass
        else:
            # new a section
            ss  = {'address':address,'size':0,'data':[]}
            self.data.append(ss)
        for b in bytes:
            ss['data'].append(b)
        ss['size'] += sz

    def __s3__(self,el,linno):
        bytes = []
        sz = int(el[2:4],16)-5
        address = int(el[4:12],16)
        for i in range(sz):
            bytes.append(int(el[(12+2*i):(14+2*i)],16))
        ck = int(el[(12+2*sz):(14+2*sz)],16)
        checksum = sz+5
        checksum += ((address>>24)&0xFF) + ((address>>16)&0xFF) + ((address>>8)&0xFF) +((address>>0)&0xFF)
        for d in bytes:
            checksum += d
        if((checksum&0xFF)+ck == 0xFF):
            pass
        else:
            print("%s::checksum error @ line %d %s",self.file,linno,el)
            return False
        
        self.append(address, bytes)
        
        return True

    def parse(self,file):
        fp = open(file)
        for linno,el in enumerate(fp.readlines()):
            if el[:2] == 'S0':
                pass # this is a header
            elif el[:2] == 'S3':
                if(self.__s3__(el,linno+1) == False): break
            elif el[:2] == 'S7':
                pass # this is the end
            else:
                print("unsupport S19 record format @ %s '%s'"%(linno,el))
        fp.close()
        self.dump('%s.debug'%(file))

    def getData(self):
        return self.data

    def dump(self,file):
        fp = open(file,'w')
        for ss in self.data:
            address = ss['address']
            size = ss['size']
            cstr = ''
            last_i = 0
            checksum = 0
            for i,b in enumerate(ss['data']):
                cstr += '%02X'%(b)
                checksum += b
                if(((i>0) and (((i+1)%32) == 0)) or (i+1 == size)):
                    sz = i- last_i + 1
                    addr = address + last_i
                    checksum += sz+5
                    checksum += ((addr>>24)&0xFF) + ((addr>>16)&0xFF) + ((addr>>8)&0xFF) +((addr>>0)&0xFF)
                    fp.write('S3%02X%08X%s%02X\n'%(sz+5,addr,cstr,0xFF-(checksum&0xFF)))
                    last_i = i + 1
                    cstr = ''
                    checksum = 0
        fp.close()

def merge(s1,s2,so):
    fo = open(so,'w')
    fp = open(s1)
    for el in fp.readlines():
        if(el[:2] != 'S7'):
            fo.write(el)
    fp.close()
    fp = open(s2)
    for el in fp.readlines():
        if(el[:2] != 'S0'):
            fo.write(el)
    fp.close()    
    fo.close()


if(__name__=='__main__'):
    if(len(sys.argv)==6 and sys.argv[1]=='-m' and sys.argv[4]=='-o'):
        merge(sys.argv[2],sys.argv[3],sys.argv[5])
    else:
        print('Usage: %s -m first.s19 sencond.s19 -o first_second.s19'%(sys.argv[0]))
        
