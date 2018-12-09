
#ifndef SCHM_SOAD_H_
#define SCHM_SOAD_H_

#include "Os.h"
#include "SchM.h"


#define SCHM_MAINFUNCTION_SOAD() 	SCHM_MAINFUNCTION(SOAD,SoAd_MainFunction())

#ifdef USE_SD
extern void Sd_MainFunction(void);
#define SCHM_MAINFUNCTION_SD()		SCHM_MAINFUNCTION(SD,Sd_MainFunction())
#else
#define SCHM_MAINFUNCTION_SD()
#endif


#endif /* SCHM_SOAD_H_ */
