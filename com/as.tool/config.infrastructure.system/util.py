__header = '''/**
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
import glob
import xml.etree.ElementTree as ET
__all__ = ['ScanPart']

def ScanPart(gendir,tag):
    a_list = []
    for xml in glob.glob('%s/*.xml'%(gendir)):
        root = ET.parse(xml).getroot();
        if(root.tag==tag):
            for sub in root:
                a_list.append(sub)
    return a_list
