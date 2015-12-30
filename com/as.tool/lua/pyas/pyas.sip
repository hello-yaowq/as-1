/* Define the SIP wrapper to the AS lua library. 
 * python version 3 -- sip 4.17  
 * python version 2 -- sip 4.15 */  

%Module AS

class can {

%TypeHeaderCode
#include "pycan.h"
%End

public:
    /* example: can.open(0,'peak'.encode('utf-8'),0,1000000) */
    int open(unsigned long busid,const char* device,unsigned long port, unsigned long baudrate);
    int write(unsigned long busid,unsigned long canid,unsigned long dlc,unsigned char* data);
    int read(unsigned long busid,unsigned long canid,unsigned long *dlc /Out/,unsigned char** data /Out/);
};  


