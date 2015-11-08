/**
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
/* ============================ [ INCLUDES  ] ====================================================== */
#include "EcuM.h"
#include "Dcm.h"

/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
Std_ReturnType BL_GetProgramSessionSeed (uint8 *securityAccessDataRecord, uint8 *seed, Dcm_NegativeResponseCodeType *errorCode)
{
	return E_OK;
}
Std_ReturnType BL_CompareProgramSessionSeed (uint8 *key)
{
	return E_OK;
}

Std_ReturnType BL_GetExtendedSessionSeed (uint8 *securityAccessDataRecord, uint8 *seed, Dcm_NegativeResponseCodeType *errorCode)
{
	return E_OK;
}
Std_ReturnType BL_CompareExtendedSessionSeed (uint8 *key)
{
	return E_OK;
}
Std_ReturnType BL_StartProtocolCbk (Dcm_ProtocolType protocolID)
{
	return E_OK;
}
Std_ReturnType BL_StopProtocolCbk (Dcm_ProtocolType protocolID)
{
	return E_OK;
}
Std_ReturnType BL_ProtocolIndicationCbk(uint8 *requestData, uint16 dataSize)
{
	return E_OK;
}

Std_ReturnType BL_GetSessionChangePermission(Dcm_SesCtrlType sesCtrlTypeActive, Dcm_SesCtrlType sesCtrlTypeNew)
{
	return E_OK;
}
Std_ReturnType BL_StartEraseFlash(uint8 *inBuffer, uint8 *outBuffer, Dcm_NegativeResponseCodeType *errorCode)
{
	return E_OK;
}
Std_ReturnType BL_GetEraseFlashResult(uint8 *outBuffer, Dcm_NegativeResponseCodeType *errorCode)
{
	return E_OK;
}
int main(int argc,char* argv[])
{
#if defined(__WINDOWS__) || defined(__LINUX__)
	ASLOG(STDOUT,"start bootloaster\n");
#endif
	EcuM_Init();
	while(1);

	return 0;
}

#if defined(__WINDOWS__) || defined(__LINUX__)
imask_t __Irq_Save(void)
{
	return 0;
}
void Irq_Restore(imask_t irq_state)
{

}

#endif
