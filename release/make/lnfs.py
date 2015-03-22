import os
import sys
import glob

def LNFS(res,tgt):
    if(os.name=='nt'):
        abs = os.path.abspath(res)
        if(os.path.isdir(abs)):
            for res2 in glob.glob('%s/*'%(abs)):
                LNFS(res2,os.path.basename(res2))
        elif(os.path.exists(res)):
            os.system('mklink %s %s'%(tgt,abs))
    else:
        abs = os.path.abspath(res)
        if(os.path.isdir(abs)):
            for res2 in glob.glob('%s/*'%(abs)):
                LNFS(res2,os.path.basename(res2))
        elif(os.path.exists(res)):
            os.system('ln -fs %s %s'%(res,tgt))
        

if(__name__ == '__main__'):
    if(len(sys.argv) == 3):
        LNFS(sys.argv[1],sys.argv[2])
    else:
        print("usage:lnfs.py resource_location target_link")