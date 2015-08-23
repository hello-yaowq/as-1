import os
import sys
import glob

def LNFS(res,tgt):
    abs = os.path.abspath(res)
    if(os.path.isdir(abs) and (tgt=='TRUE' or tgt=='FALSE')):
        for res2 in glob.glob('%s/*'%(abs)):
            # fork sub directory only if tgt is TRUE
            if(tgt == 'TRUE' and os.path.isdir(res2)):
                LNFS(res2,'TRUE')
            elif(False == os.path.isdir(res2)):
                LNFS(res2,os.path.basename(res2))
    elif(os.path.exists(res)):
        if(os.name=='nt'):
            if(os.path.isdir(abs)):
                os.system('mklink /D %s %s'%(tgt,abs))
            else:
                os.system('mklink %s %s'%(tgt,abs))
        else:
            os.system('ln -fs %s %s'%(res,tgt))
            

def LNFS2(res):
    tgt = os.path.basename(res)
    abs = os.path.abspath(res)
    if(os.name=='nt'):
        if(os.path.isdir(abs)):
            os.system('mklink /D %s %s'%(tgt,abs))
        elif(os.path.exists(res)):
            os.system('mklink %s %s'%(tgt,abs))
    else:
        if(os.path.isdir(abs)):
            os.system('ln -fs %s %s'%(res,tgt))
        elif(os.path.exists(res)):
            os.system('ln -fs %s %s'%(res,tgt))
            

if(__name__ == '__main__'):
    if(len(sys.argv) == 3):
        LNFS(sys.argv[1],sys.argv[2])
    elif(len(sys.argv) == 2):
        LNFS2(sys.argv[1])
    else:
        print("usage:lnfs.py resource_location target_link")