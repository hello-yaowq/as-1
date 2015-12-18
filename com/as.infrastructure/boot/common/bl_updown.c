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
#include "bootloader.h"
#include "Flash.h"
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static boolean fls_initialized = FALSE;
static tFlashParam FlashParam =
{
    .patchlevel  = FLASH_DRIVER_VERSION_PATCH,
    .minornumber = FLASH_DRIVER_VERSION_MINOR,
    .majornumber = FLASH_DRIVER_VERSION_MAJOR,
    .wdTriggerFct = NULL,
    .errorcode   = kFlashOk,
};
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */

Std_ReturnType BL_StartEraseFlash(uint8 *inBuffer, uint8 *outBuffer, Dcm_NegativeResponseCodeType *errorCode)
{
	if(FALSE == fls_initialized)
	{
		FLASH_DRIVER_INIT(FLASH_DRIVER_STARTADDRESS,&FlashParam);
		fls_initialized = TRUE;
	}
	*errorCode = DCM_E_POSITIVE_RESPONSE;
	uint32 address = ((uint32)inBuffer[0]<<24) + ((uint32)inBuffer[1]<<16) + ((uint32)inBuffer[2]<<8) +((uint32)inBuffer[3]);
	uint32 length  = ((uint32)inBuffer[4]<<24) + ((uint32)inBuffer[5]<<16) + ((uint32)inBuffer[6]<<8) +((uint32)inBuffer[7]);

	FlashParam.address = address;
	FlashParam.length = length;

	ASLOG(BL,"%s(%X,%X)\n",__func__,address,length);

	FLASH_DRIVER_ERASE(FLASH_DRIVER_STARTADDRESS,&FlashParam);

	return E_OK;
}

Dcm_ReturnWriteMemoryType Dcm_WriteMemory(Dcm_OpStatusType OpStatus,
											   uint8 MemoryIdentifier,
											   uint32 MemoryAddress,
											   uint32 MemorySize,
											   uint8* MemoryData)
{
	FlashParam.address = MemoryAddress;
	FlashParam.length = MemorySize;
	FlashParam.data = (tData*)MemoryData;
	FLASH_DRIVER_WRITE(FLASH_DRIVER_STARTADDRESS,&FlashParam);

	return DCM_WRITE_OK;
}

/*@req Dcm495*/
Dcm_ReturnReadMemoryType Dcm_ReadMemory(Dcm_OpStatusType OpStatus,
											   uint8 MemoryIdentifier,
											   uint32 MemoryAddress,
											   uint32 MemorySize,
											   uint8* MemoryData)
{
	return DCM_WRITE_OK;
}

