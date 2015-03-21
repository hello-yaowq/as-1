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
#include "Can.h"
#include <windows.h>
#include "vxlapi.h"
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */

/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
static boolean 		IsXLReady = FALSE;
static XLportHandle xlPortHandle[32];
static XLaccess     xlAccess[32];
/* ============================ [ FUNCTIONS ] ====================================================== */
Std_ReturnType Can_Init(uint32 port,uint32 baudrate)
{
	char userName[32];
	XLstatus status;
	XLaccess accessMask;
	if(FALSE == IsXLReady)
	{
		status = xlOpenDriver();

		if(XL_SUCCESS == status)
		{
			IsXLReady = TRUE;
		}
		else
		{
			IsXLReady = FALSE;
			printf("CAM::start XL device failed.\n");
			return E_NOT_OK;
		}
	}


	sprintf(userName,"port%d",(int)port);
	accessMask = 1<<port;
	status= xlOpenPort(&xlPortHandle[port],userName,accessMask,&xlAccess[port],512,XL_INTERFACE_VERSION,XL_BUS_TYPE_CAN);
	if(XL_SUCCESS != status)
	{
		printf(xlGetErrorString(status));
		return E_NOT_OK;
	}

	status = xlCanSetChannelBitrate(xlPortHandle[port],xlAccess[port],baudrate);
	if(XL_SUCCESS != status)
	{
		printf(xlGetErrorString(status));
		return E_NOT_OK;
	}

	status = xlActivateChannel(xlPortHandle[port],xlAccess[port],XL_BUS_TYPE_CAN,XL_ACTIVATE_RESET_CLOCK);
	if(XL_SUCCESS != status)
	{
		printf(xlGetErrorString(status));
		return E_NOT_OK;
	}
	return E_OK;
}
