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
#include "Std_Types.h"
#include "Flash.h"
#include "stm32f10x_flash.h"
/* ============================ [ MACROS    ] ====================================================== */
#define FLASH_ERASE_SIZE  512 	/* TODO */
#define FLASH_IS_ERASE_ADDRESS_ALIGNED(a)  ( 0 == ((FLASH_ERASE_SIZE-1)&(a)) )

#define FLASH_WRITE_SIZE  2 		/* TODO */
#define FLASH_IS_WRITE_ADDRESS_ALIGNED(a)  ( 0 == ((FLASH_WRITE_SIZE-1)&(a)) )
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
tFlashHeader FlashHeader =
{
	.Info.w    = 0x01004512,
	.Init      = FlashInit,
	.Deinit    = FlashDeinit,
	.Erase     = FlashErase,
	.Write     = FlashWrite
};
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void FlashInit(tFlashParam* FlashParam)
{
	if ( (FLASH_DRIVER_VERSION_PATCH == FlashParam->patchlevel) ||
		 (FLASH_DRIVER_VERSION_MINOR == FlashParam->minornumber) ||
		 (FLASH_DRIVER_VERSION_MAJOR == FlashParam->majornumber) )
	{
		/* Unlock the Flash Program Erase controller */
		FLASH_Unlock();
		/* Clear All pending flags */
		FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

		FlashParam->errorcode = kFlashOk;
	}
	else
	{
		FlashParam->errorcode = kFlashFailed;
	}
}

void FlashDeinit(tFlashParam* FlashParam)
{
	FLASH_Lock();
	FlashParam->errorcode = kFlashOk;
}

void FlashErase(tFlashParam* FlashParam)
{
	tAddress address;
	tAddress length;
	if ( (FLASH_DRIVER_VERSION_PATCH == FlashParam->patchlevel) ||
		 (FLASH_DRIVER_VERSION_MINOR == FlashParam->minornumber) ||
		 (FLASH_DRIVER_VERSION_MAJOR == FlashParam->majornumber) )
	{
		length = FlashParam->length;
		address = FlashParam->address;
		if ( (FALSE == FLASH_IS_ERASE_ADDRESS_ALIGNED(address)) ||
			 (FALSE == IS_FLASH_ADDRESS(address)) )
		{
			FlashParam->errorcode = kFlashInvalidAddress;
		}
		else if( (FALSE == IS_FLASH_ADDRESS(address+length)) ||
				 (FALSE == FLASH_IS_ERASE_ADDRESS_ALIGNED(length)) )
		{
			FlashParam->errorcode = kFlashInvalidSize;
		}
		else
		{
			/* Erase the sector... */
			while(length > 0)
			{
				volatile FLASH_Status status = FLASH_BUSY;
				while(status != FLASH_COMPLETE){
					status = FLASH_ErasePage((uint32_t)address);
				}

				length  -= FLASH_ERASE_SIZE;
				address += FLASH_ERASE_SIZE;
			}
			FlashParam->errorcode = kFlashOk;
		}
	}
	else
	{
		FlashParam->errorcode = kFlashFailed;
	}
}

void FlashWrite(tFlashParam* FlashParam)
{
	tAddress address;
	tAddress length;
	tData*    data;
	if ( (FLASH_DRIVER_VERSION_PATCH == FlashParam->patchlevel) ||
		 (FLASH_DRIVER_VERSION_MINOR == FlashParam->minornumber) ||
		 (FLASH_DRIVER_VERSION_MAJOR == FlashParam->majornumber) )
	{
		length = FlashParam->length;
		address = FlashParam->address;
		data = FlashParam->data;
		if ( (FALSE == FLASH_IS_ERASE_ADDRESS_ALIGNED(address)) ||
			 (FALSE == IS_FLASH_ADDRESS(address)) )
		{
			FlashParam->errorcode = kFlashInvalidAddress;
		}
		else if( (FALSE == IS_FLASH_ADDRESS(address+length)) ||
				 (FALSE == FLASH_IS_ERASE_ADDRESS_ALIGNED(length)) )
		{
			FlashParam->errorcode = kFlashInvalidSize;
		}
		else if( NULL == data )
		{
			FlashParam->errorcode = kFlashInvalidData;
		}
		else
		{
			FlashParam->errorcode = kFlashOk;
		}

	}
	else
	{
		FlashParam->errorcode = kFlashFailed;
	}
}
