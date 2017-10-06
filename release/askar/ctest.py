#! /usr/bin/python3
'''/**
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

import os,sys,re
import xml.etree.ElementTree as ET

sys.path.append('../../com/as.tool/config.infrastructure.system/aroil')

import reoil

CTEST='../../com/as.infrastructure/system/kernel/freeosek/tst/ctest'

#/usr/lib/python3.4/re.py
reHeader=re.compile(r'^(\w+):(.+)$')
reCase=re.compile(r'^\s+([^:]+)$')
reVar=re.compile(r'^\s+(\w+):(\w+)$')
def parse():
    fp = open('%s/cfg/ctestcases.cfg'%(CTEST),'r')
    cfg = []
    target=''
    case=''
    id = -1
    for el in fp.readlines():
        if(reHeader.search(el)):
            target,desc = reHeader.search(el).groups()
            cfg.append({'desc':desc, 'target':target, 'case':{} })
            id += 1
        elif(reCase.search(el)):
            case = reCase.search(el).groups()[0].strip().replace(' ','-')
            cfg[id]['case'][case]={}
        elif(reVar.search(el)):
            name,val = reVar.search(el).groups()
            cfg[id]['case'][case][name] = val
        else:
            if(el.strip() != ''):
                print('warning:', el)
    fp.close()
    return cfg

def fixXml(xml,vv):
    for obj in xml:
        fixXml(obj,vv)
        for k,v in obj.items():
            try:
                name = obj.attrib['Name']
            except KeyError:
                name = ''
            try:
                v2 = vv[v]
                obj.attrib[k] = v2
                print('fix %s %s[%s] from %s to %s'%(obj,name,k,v,v2))
            except KeyError:
                pass

def saveXml(xml,File):
    root = ET.Element('AS')
    root.append(xml)
    tree = ET.ElementTree(root)
    tree.write(File, encoding="utf-8", xml_declaration=True);
    fp = open(File,'r')
    content = fp.read()
    fp.close()
    fp = open(File,'w')
    fp.write(content.replace('>','>\n'))
    fp.close()

def genCTEST_CFGH(xml, path):
    fp = open('%s/ctest_cfg.h'%(path), 'w')
    fp.write('#ifndef _CTEST_CFG_H_\n#define _CTEST_CFG_H_\n\n')
    general = xml.find('General')
    fp.write('#define CT_ERROR_CHECKING_TYPE	CT_ERROR_CHECKING_%s\n\n'%(general.attrib['Status']))
    for tsk in xml:
        if(tsk.tag == 'Task'):
            if(tsk.attrib['Schedule'] == 'FULL'):
                fp.write('#define CT_SCHEDULING_%s CT_PREEMPTIVE\n\n'%(tsk.attrib['Name']))
            else:
                fp.write('#define CT_SCHEDULING_%s CT_NON_PREEMPTIVE\n\n'%(tsk.attrib['Name']))
    fp.write('#endif\n\n')
    fp.close()

def test(target,case,vv):
    cmd = 'mkdir -pv src/%s/%s'%(target, case)
    os.system(cmd)
    xml = reoil.to_xml('%s/etc/%s.oil'%(CTEST,target))
    fixXml(xml,vv)
    saveXml(xml, 'src/%s/%s/test.xml'%(target, case))
    genCTEST_CFGH(xml,'src/%s/%s'%(target, case))
    cmd='make dep-os TARGET=%s CASE=%s'%(target, case)
    os.system(cmd)
    cmd='make all TARGET=%s CASE=%s'%(target, case)
    os.system(cmd)

if(__name__ == '__main__'):
    if(len(sys.argv) == 2 and sys.argv[1] == 'all'):
        cfg = parse()
        for v in cfg:
            target = v['target']
            for case,vv in v['case'].items():
                test(target,case,vv)
                c = input('Continue <Enter> Exit <q>')
                if(c == 'q'): exit(1)                
    if(len(sys.argv) == 3):
        cfg = parse()
        for v in cfg:
            target = v['target']
            if(target != sys.argv[1]): continue
            for case,vv in v['case'].items():
                if(case != sys.argv[2]): continue
                test(target,case,vv)
    else:
        print('Usage: %s all'%(sys.argv[0]))
        print('       %s target case\nExample:'%(sys.argv[0]))
        cfg = parse()
        count = 0
        for v in cfg:
            target = v['target']
            for case,vv in v['case'].items():
                print('\t%s %s %s'%(sys.argv[0],target,case))
                count += 1
                if(count > 8):
                    count = 0;
                    c = input('More <Enter> Exit <q>')
                    if(c == 'q'): exit(1)

