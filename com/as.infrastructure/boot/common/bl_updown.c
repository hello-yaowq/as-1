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
#include "Os.h"
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
static TimerType eraseTimer;
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */

Dcm_ReturnEraseMemoryType Dcm_EraseMemory(Dcm_OpStatusType OpStatus,
											   uint8 MemoryIdentifier,
											   uint32 MemoryAddress,
											   uint32 MemorySize)
{
	Dcm_ReturnEraseMemoryType rv;
	if(FALSE == fls_initialized)
	{
		FLASH_DRIVER_INIT(FLASH_DRIVER_STARTADDRESS,&FlashParam);
		fls_initialized = TRUE;
	}

	if(DCM_INITIAL == OpStatus)
	{
		FlashParam.address = MemoryAddress;
		FlashParam.length = MemorySize;

		ASLOG(BL,"%s(%X,%X)\n",__func__,MemoryAddress,MemorySize);

		FLASH_DRIVER_ERASE(FLASH_DRIVER_STARTADDRESS,&FlashParam);
		rv = DCM_ERASE_PENDING;

		StartTimer(&eraseTimer);
	}
	else
	{
		if(GetTimer(&eraseTimer) > 12000)
		{
			rv = DCM_ERASE_OK;
		}
		else
		{
			rv = DCM_ERASE_PENDING;
		}
	}

	return rv;
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

