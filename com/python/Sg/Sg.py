__hh__ = '''
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
from PyQt4 import QtGui
from PyQt4.QtGui import *
import xml.etree.ElementTree as ET
import re

rePic = re.compile(r'picture\{(\d+)\}')

qtApp = QtGui.QApplication(sys.argv)

__SGL_MAX = 0
class Sg():
    def __init__(self,file,option=None):
        self.file = file
        self.option = option
    def toU8Dot(self,fp):
        name = os.path.basename(self.file).replace('.','_')
        aname = os.path.abspath(self.file)
        fp.write('#include "Sg.h"\n')
        fp.write('static const uint8 %s_dmp[] = \n{'%(name))
        IM = QImage(self.file)
        fp.write('\n\t/* size(%s,%s) */'%(IM.size().width(),IM.size().height()))
        for y in range(0,IM.size().height()):
            fp.write('\n\t')
            DOT = B = 0
            for x in range(0,IM.size().width()):
                rgb = IM.pixel(x,y)
                r = (rgb>>16)&0xFF
                g = (rgb>>8)&0xFF
                b = (rgb>>0)&0xFF
                # only output the black one
                if(r<5 and g<5 and b<5):
                    DOT = DOT|(1<<B);    
                B += 1;
                if(B == 8):
                    fp.write('0x%-2X,'%(DOT))
                    DOT = 0;B = 0;
            if(B>0):
                fp.write('0x%-2X,'%(DOT))
        fp.write('\n};\n')
        fp.write('static const SgDMP %s_DMP=\n'%(name))
        fp.write('{  /* %s */\n'%(aname))
        fp.write('\t/*t=*/SGT_DMP,\n')
        fp.write('\t/*w=*/%s,\n'%(IM.size().width()))
        fp.write('\t/*h=*/%s,\n'%(IM.size().height()))
        fp.write('\t/*p=*/%s_dmp\n};\n'%(name))
    def toU8Pixel(self,fp):
        name = os.path.basename(self.file).replace('.','_')
        aname = os.path.abspath(self.file)
        fp.write('#include "SgRes.h"\n')
        fp.write('static const uint32 %s_bmp[] = \n{'%(name))
        IM = QImage(self.file)
        for y in range(0,IM.size().height()):
            fp.write('\n\t')
            for x in range(0,IM.size().width()):
                rgb = IM.pixel(x,y)
                fp.write('0x%-8X,'%(rgb))
        fp.write('\n};\n')
        fp.write('static const SgBMP %s_BMP=\n'%(name))
        fp.write('{  /* %s */\n'%(aname))
        fp.write('\t/*t=*/SGT_BMP,\n')
        fp.write('\t/*w=*/%s,\n'%(IM.size().width()))
        fp.write('\t/*h=*/%s,\n'%(IM.size().height()))
        fp.write('\t/*p=*/%s_bmp\n};\n'%(name))

def GetSgImage(IML=[],fp=None):
    for image in IML:
        if(fp != None):
            Sg(image).toU8Pixel(fp)

def GetSgFont(IML=[],fp=None):
    for image in IML:
        if(fp != None):
            Sg(image).toU8Dot(fp)

def GenerateWidget(widget,fph,fpc): 
    global __SGL_MAX
    print('## Process Widget %s'%(widget.attrib['name']))
    fp = open('SgRes/%s.c'%(widget.attrib['name']),'w')
    if(rePic.search(widget.attrib['type']) != None):
        size = int(rePic.search(widget.attrib['type']).groups()[0],10)
        IML = []
        for p in widget:
            if(p.tag=='SgPciture'):IML.append(p.attrib['locate'])
        if(len(IML) == size):
            GetSgImage(IML,fp)
        else:
            raise Exception('size SG widget picture is not right <size=%s,len(SgPciture)=%s>!'%(size,len(IML)))
        fp.write('const SgBMP* %s_BMPS[%s] = \n{\n'%(widget.attrib['name'],size+1))
        for i,file in enumerate(IML):
            name = os.path.basename(file).replace('.','_')
            fp.write('\t&%s_BMP,\n'%(name))
            fph.write("#define SGR_%-32s %s\n"%(name.upper(),i))
        fph.write('extern const SgBMP* %s_BMPS[%s];\n'%(widget.attrib['name'],size+1))
        fp.write('\tNULL\n};\n\n')
        
        fpc.write('\t{ /* SGW_%s */\n'%(widget.attrib['name'].upper()))
        fpc.write('\t\t/*x =*/%s,\n'%(widget.attrib['x']))
        fpc.write('\t\t/*y =*/%s,\n'%(widget.attrib['y']))
        fpc.write('\t\t/*w =*/%s,\n'%(widget.attrib['w']))
        fpc.write('\t\t/*h =*/%s,\n'%(widget.attrib['h']))
        fpc.write('\t\t/*l =*/%s,\n'%(widget.attrib['layer']))
        if(int(widget.attrib['layer'],10) > __SGL_MAX ): __SGL_MAX = int(widget.attrib['layer'],10)
        name = os.path.basename(IML[0]).replace('.','_')
        fpc.write('\t\t/*ri=*/SGR_%s,\n'%(name.upper()))
        fpc.write('\t\t/*r =*/(const SgRes**)%s_BMPS,\n'%(widget.attrib['name']))
        fpc.write('\t},\n')
    else:
        raise Exception('unknown SG widget type!')
    fp.close()
def GenerateSg(file):
    global __SGL_MAX
    if(os.path.exists('SgRes')==False): 
        os.mkdir('SgRes') 
    root = ET.parse(file).getroot();
    fph = open('SgRes/SgRes.h','w')
    fpc = open('SgRes/SgRes.c','w')
    fpc.write(__hh__)
    fpc.write('\n#include "SgRes.h"\n')
    fph.write(__hh__)
    fph.write('\n#ifndef SGRES_H\n#define SGRES_H')
    fph.write('\n#include "Sg.h"\n')
    
    fpc.write('SgWidget SGWidget[%s] = \n{\n'%(len(root)))
    for w in root:
        GenerateWidget(w,fph,fpc)
    fpc.write('};\n\n')
    fph.write('\n\n')
    for i,w in enumerate(root):
        fph.write('#define SGW_%-32s %s\n'%(w.attrib['name'].upper(),i))
    fph.write('#define SGW_%-32s %s\n'%('MAX',len(root)))
    fph.write('\n\nextern SgWidget SGWidget[%s];\n\n'%(len(root)))
    
    fph.write("\n\n#define SGL_MAX %s\n\n"%(__SGL_MAX+1))
    fph.write('#endif\n\n')
    fph.close()
    fpc.close()
    print(">>>>>>>> DONE! <<<<<<<<<")
if(__name__ == '__main__'):
    if(len(sys.argv) == 2):
        GenerateSg(sys.argv[1])

            
        