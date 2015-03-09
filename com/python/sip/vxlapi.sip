/* Define the SIP wrapper to the vxlapi library. */  
%Module(name=vxlapi, language="C")  

typedef short XLstatus;

/* Hint for type declare
 * struct Word {
 * 		%TypeHeaderCode  
 * 		#include <word.h>  
 * 		%End
 * 		const char *the_word;
 * };
 */
struct s_xl_application_notification {
%TypeHeaderCode  
#include "vxlapi.h"
%End
	unsigned int 	notifyReason;
  	unsigned int*	reserved
	{
		%GetCode
			
		%End

		%SetCode
			
		%End
	};
};

XLstatus xlOpenDriver(void);
XLstatus xlCloseDriver(void);


