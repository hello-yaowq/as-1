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
#define BL_FLASH_IDENTIFIER  0xFF
#define BL_EEPROM_IDENTIFIER 0xEE
#if defined(__LINUX__) || defined(__WINDOWS__)
#define FL_ERASE_SECTOR_PER_CYCLE 32
#define FL_WRITE_SECTOR_PER_CYCLE 32
#else
#define FL_ERASE_SECTOR_PER_CYCLE 8
#define FL_WRITE_SECTOR_PER_CYCLE 8
#endif
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static tFlashParam blFlashParam =
{
    .patchlevel  = FLASH_DRIVER_VERSION_PATCH,
    .minornumber = FLASH_DRIVER_VERSION_MINOR,
    .majornumber = FLASH_DRIVER_VERSION_MAJOR,
    .wdTriggerFct = NULL,
    .errorcode   = kFlashOk,
};
static uint8   blMemoryIdentifier;
static uint32  blMemoryAddress;
static uint32  blMemorySize;
static uint32* blMemoryData;
/* ============================ [ LOCALS    ] ====================================================== */
static Dcm_ReturnEraseMemoryType eraseFlash(Dcm_OpStatusType OpStatus,uint32 MemoryAddress,uint32 MemorySize)
{
	Dcm_ReturnEraseMemoryType rv;
	uint32 length;
	switch(OpStatus)
	{
		case DCM_INITIAL:
			FLASH_DRIVER_INIT(FLASH_DRIVER_STARTADDRESS,&blFlashParam);
			if(kFlashOk == blFlashParam.errorcode)
			{
				blMemoryAddress = MemoryAddress;
				blMemorySize = MemorySize;
				rv = DCM_ERASE_PENDING;
			}
			else
			{
				rv = DCM_ERASE_FAILED;
			}
			break;
		case DCM_PENDING:
			if(blMemorySize > (FL_ERASE_SECTOR_PER_CYCLE*FLASH_ERASE_SIZE))
			{
				length  = FL_ERASE_SECTOR_PER_CYCLE*FLASH_ERASE_SIZE;
			}
			else
			{
				length = blMemorySize;
			}
			blFlashParam.address = blMemoryAddress;
			blFlashParam.length  = length;
			blFlashParam.data    = (tData*)blMemoryData;
			FLASH_DRIVER_ERASE(FLASH_DRIVER_STARTADDRESS,&blFlashParam);
			blMemoryAddress += length;
			blMemorySize    -= length;
			if(kFlashOk == blFlashParam.errorcode)
			{
				if( 0 == blMemorySize)
				{
					rv = DCM_ERASE_OK;
				}
				else
				{
					rv = DCM_ERASE_PENDING;
				}
			}
			else
			{
				ASLOG(BL,"erase faile: errorcode = %X(addr=%X,size=%X)\n",
						blFlashParam.errorcode,blFlashParam.address,blFlashParam.length);
				rv = DCM_ERASE_FAILED;
			}
			break;
		default:
			asAssert(0);
			rv = DCM_ERASE_FAILED;
			break;
	}

	return rv;
}

static Dcm_ReturnEraseMemoryType writeFlash(Dcm_OpStatusType OpStatus,uint32 MemoryAddress,uint32 MemorySize,
		uint8* MemoryData)
{
	Dcm_ReturnEraseMemoryType rv;
	uint32 length;
	switch(OpStatus)
	{
		case DCM_INITIAL:
			blMemoryAddress = MemoryAddress;
			blMemorySize 	= MemorySize;
			blMemoryData 	= (uint32*)MemoryData; /* should be uint32 aligned */
			/* no break here intentionally */
		case DCM_PENDING:
			if(blMemorySize > (FL_WRITE_SECTOR_PER_CYCLE*FLASH_WRITE_SIZE))
			{
				length  = FL_WRITE_SECTOR_PER_CYCLE*FLASH_WRITE_SIZE;
			}
			else
			{
				length = blMemorySize;
			}
			blFlashParam.address = blMemoryAddress;
			blFlashParam.length = length;
			blFlashParam.data    = (tData*)blMemoryData;
			FLASH_DRIVER_ERASE(FLASH_DRIVER_STARTADDRESS,&blFlashParam);
			blMemoryAddress += length;
			blMemorySize    -= length;
			blMemoryData    = &blMemoryData[length/sizeof(uint32)];
			if(kFlashOk == blFlashParam.errorcode)
			{
				if( 0 == blMemorySize)
				{
					rv = DCM_WRITE_OK;
				}
				else
				{
					rv = DCM_WRITE_PENDING;
				}
			}
			else
			{
				ASLOG(BL,"write faile: errorcode = %X(addr=%X,size=%X)\n",
						blFlashParam.errorcode,blFlashParam.address,blFlashParam.length);
				rv = DCM_WRITE_FAILED;
			}
			break;
		default:
			asAssert(0);
			rv = DCM_WRITE_FAILED;
			break;
	}

	return rv;
}
/* ============================ [ FUNCTIONS ] ====================================================== */

Dcm_ReturnEraseMemoryType Dcm_EraseMemory(Dcm_OpStatusType OpStatus,
											   uint8 MemoryIdentifier,
											   uint32 MemoryAddress,
											   uint32 MemorySize)
{
	Dcm_ReturnEraseMemoryType rv;

	if(DCM_INITIAL == OpStatus)
	{
		blMemoryIdentifier = MemoryIdentifier;
	}
	switch(blMemoryIdentifier)
	{
		case BL_FLASH_IDENTIFIER:
			rv = eraseFlash(OpStatus,MemoryAddress,MemorySize);
			break;
		default:
			asAssert(0);
			rv = DCM_READ_FAILED;
			break;
	}
	return rv;
}

Dcm_ReturnWriteMemoryType Dcm_WriteMemory(Dcm_OpStatusType OpStatus,
											   uint8 MemoryIdentifier,
											   uint32 MemoryAddress,
											   uint32 MemorySize,
											   uint8* MemoryData)
{
	Dcm_ReturnEraseMemoryType rv;

	if(DCM_INITIAL == OpStatus)
	{
		blMemoryIdentifier = MemoryIdentifier;
	}
	switch(blMemoryIdentifier)
	{
		case BL_FLASH_IDENTIFIER:
			rv = writeFlash(OpStatus,MemoryAddress,MemorySize,MemoryData);
			break;
		default:
			asAssert(0);
			rv = DCM_READ_FAILED;
			break;
	}
	return rv;
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

