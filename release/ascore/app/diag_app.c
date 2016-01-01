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
#include "Dcm.h"
#include "Os.h"
#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */
#define AS_LOG_DIAG 1
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static uint32 bl_extds_seed= 0xbeafdada;
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
Std_ReturnType Diag_RequestServiceStart (Dcm_ProtocolType protocolID)
{
    return E_OK;
}
Std_ReturnType Diag_RequestServiceStop (Dcm_ProtocolType protocolID)
{
    return E_OK;
}
Std_ReturnType Diag_RequestServiceIndication(uint8 *requestData, uint16 dataSize)
{
    return E_OK;
}

Std_ReturnType Diag_GetSesChgPer(Dcm_SesCtrlType sesCtrlTypeActive,
                                            Dcm_SesCtrlType sesCtrlTypeNew)
{
    return E_OK;
}

Std_ReturnType Diag_GetSeedEXTDS(uint8 *securityAccessDataRecord, uint8 *seed, Dcm_NegativeResponseCodeType *errorCode)
{
	uint32 u32Seed; /* intentional not initialized to use the stack random value */
	uint32 u32Time = GetOsTick();
	*errorCode = DCM_E_POSITIVE_RESPONSE;
	(void)securityAccessDataRecord;
	*errorCode = DCM_E_POSITIVE_RESPONSE;

	bl_extds_seed = bl_extds_seed ^ u32Seed ^ u32Time ^ 0x95774321;

	ASLOG(DIAG,"%s(seed = %X)\n",__func__,bl_extds_seed);

	seed[0] = (uint8)(bl_extds_seed>>24);
	seed[1] = (uint8)(bl_extds_seed>>16);
	seed[2] = (uint8)(bl_extds_seed>>8);
	seed[3] = (uint8)(bl_extds_seed);
	return E_OK;
}
Std_ReturnType Diag_CompareKeyEXTDS(uint8 *key){
	Std_ReturnType ercd;
	uint32 u32Key = ((uint32)key[0]<<24) + ((uint32)key[1]<<16) + ((uint32)key[2]<<8) +((uint32)key[3]);
	uint32 u32KeyExpected = bl_extds_seed ^ 0x78934673;

	ASLOG(DIAG,"%s(key = %X(%X))\n",__func__,u32Key,u32KeyExpected);

	if(u32KeyExpected == u32Key)
	{
		ercd = E_OK;
	}
	else
	{
		ercd = E_NOT_OK;
	}
	return ercd;
}


Std_ReturnType Diag_StartProtocolCbk (Dcm_ProtocolType protocolID)
{
	return E_OK;
}
Std_ReturnType Diag_StopProtocolCbk (Dcm_ProtocolType protocolID)
{
	return E_OK;
}
Std_ReturnType Diag_ProtocolIndicationCbk(uint8 *requestData, uint16 dataSize)
{
	return E_OK;
}



