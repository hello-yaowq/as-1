/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2018  AS <parai@foxmail.com>
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
#include "SD.h"
#include "SD_Internal.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static Sd_InstanceType Sd_Instance[SD_NUMBER_OF_INSTANCES] =
{
	{
		.HostName = "someip",
	}
};
const Sd_ConfigType Sd_Config = {
	.Instance = Sd_Instance
};

Sd_DynConfigType Sd_DynConfig;
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void BswM_Sd_ClientServiceCurrentState(uint16 SdClientServiceHandleId,
			Sd_ClientServiceCurrentStateType CurrentClientState)
{

}

void BswM_Sd_ConsumedEventGroupCurrentState(uint16 SdConsumedEventGroupHandleId,
			Sd_ConsumedEventGroupCurrentStateType ConsumedEventGroupState)
{

}

void BswM_Sd_EventHandlerCurrentState(uint16 SdEventHandlerHandleId,
			Sd_EventHandlerCurrentStateType EventHandlerStatus)
{

}

