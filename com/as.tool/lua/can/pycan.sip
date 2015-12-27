/* Define the SIP wrapper to the Can library. 
 * python version 3 */  
%Module(name=Can, language="C")  

int open(unsigned long busid,char* device,unsigned long port, unsigned long baudrate);
