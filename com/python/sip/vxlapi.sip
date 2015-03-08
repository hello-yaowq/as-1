/* Define the SIP wrapper to the vxlapi library. */  
%Module(name=vxlapi, language="C")  

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
#include "vxlapi2.h"
%End
};


