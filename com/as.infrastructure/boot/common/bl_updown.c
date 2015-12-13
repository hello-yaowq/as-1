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
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */

Std_ReturnType BL_StartEraseFlash(uint8 *inBuffer, uint8 *outBuffer, Dcm_NegativeResponseCodeType *errorCode)
{
	*errorCode = DCM_E_RESPONSE_PENDING;
	uint32 address = ((uint32)inBuffer[0]<<24) + ((uint32)inBuffer[1]<<16) + ((uint32)inBuffer[2]<<8) +((uint32)inBuffer[3]);
	uint32 length  = ((uint32)inBuffer[4]<<24) + ((uint32)inBuffer[5]<<16) + ((uint32)inBuffer[6]<<8) +((uint32)inBuffer[7]);

	ASLOG(BL,"%s(%X,%X)\n",__func__,address,length);
	return E_OK;
}
