/* Define the SIP wrapper to the Can library. 
 * python version 3 */  
%Module(name=Can, language="C")  

typedef unsigned int boolean;
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;

boolean open(uint32_t busid,char* device,uint32_t port, uint32_t baudrate);
