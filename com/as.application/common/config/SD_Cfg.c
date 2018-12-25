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
static const Sd_ClientTimerType Sd_ClientTimer[1];
static const Sd_ServerTimerType  Sd_ServerTimer[1];
static const Sd_ClientServiceType Sd_ClientServiceCfg[] =
{
	{
		.AutoRequire = TRUE,
		.HandleId = 0xDB,
		.Id = 0x1234,
		.InstanceId = 0x5678,
		.MajorVersion = 0,
		.MinorVersion = 0,
		.TcpSocketConnectionGroupId = 0xDB,
		.TcpSocketConnectionGroupSocketConnectionIdsPtr = NULL,
		.TimerRef = Sd_ClientTimer,
		.UdpSocketConnectionGroupId = 0x1234,
		.UdpSocketConnectionGroupSocketConnectionIdsPtr = NULL,
		.CapabilityRecord = NULL,
		.SdNoOfCapabiltyRecord = 0,
		.ConsumedEventGroup = NULL,
		.NoOfConsumedEventGroups = 0,
		.ConsumedMethods.ClientServiceActivationRef = 0x5678
	},
	{
		.AutoRequire = TRUE,
		.HandleId = 0xDB,
		.Id = 0x1235,
		.InstanceId = 0x5678,
		.MajorVersion = 0,
		.MinorVersion = 0,
		.TcpSocketConnectionGroupId = 0xDB,
		.TcpSocketConnectionGroupSocketConnectionIdsPtr = NULL,
		.TimerRef = Sd_ClientTimer,
		.UdpSocketConnectionGroupId = 0x1235,
		.UdpSocketConnectionGroupSocketConnectionIdsPtr = NULL,
		.CapabilityRecord = NULL,
		.SdNoOfCapabiltyRecord = 0,
		.ConsumedEventGroup = NULL,
		.NoOfConsumedEventGroups = 0,
		.ConsumedMethods.ClientServiceActivationRef = 0x5678
	}
};

static const Sd_ServerServiceType Sd_ServerServiceCfg[] =
{
	{
		.AutoAvailable = TRUE,
		.HandleId = 0xDB,
		.Id = 0xDB,
		.InstanceId = 0xDB,
		.MajorVersion = 1,
		.MinorVersion = 0,
		.TcpSocketConnectionGroupId = 0xDB,
		.TcpSocketConnectionGroupSocketConnectionIdsPtr = NULL,
		.TimerRef = Sd_ServerTimer,
		.UdpSocketConnectionGroupId = 0xDB,
		.UdpSocketConnectionGroupSocketConnectionIdsPtr = NULL,
		.CapabilityRecord = NULL,
		.SdNoOfCapabiltyRecord = 0,
		.EventHandler = NULL,
		.NoOfEventHandlers = 0,
		.ProvidedMethods.ServerServiceActivationRef = 0xDB
	},
};

static Sd_DynClientServiceType Sd_DynClientService[] =
{
	{
		.ClientServiceCfg = &Sd_ClientServiceCfg[0]
	},
	{
		.ClientServiceCfg = &Sd_ClientServiceCfg[1]
	}
};

static Sd_DynServerServiceType Sd_DynServerService[] =
{
	{
		.ServerServiceCfg = &Sd_ServerServiceCfg[0]
	}
};

static const Sd_InstanceType Sd_InstanceCfg =
{
	.HostName = "someip",
	.SdNoOfClientServices = ARRAY_SIZE(Sd_ClientServiceCfg),
	.SdClientService = Sd_ClientServiceCfg,
	.SdClientTimer = Sd_ClientTimer,
#if defined(USE_DEM)
	.DemEventParameterRefs = NULL,
#endif
	.MulticastRxPduId = SD_RX_MULTICAST_PDUID,
	.MulticastRxPduSoConRef = 0xDB,
	.TxPduId = 0xDB,
	.UnicastRxPduId = SD_RX_UNICAST_PDUID,
	.UnicastRxPduSoConRef = 0xDB,
	.SdNoOfServerServices = ARRAY_SIZE(Sd_ServerServiceCfg),
	.SdServerService = Sd_ServerServiceCfg,
	.SdServerTimer = Sd_ServerTimer
};

static Sd_DynInstanceType Sd_DynInstance = {
	.InstanceCfg = &Sd_InstanceCfg,
	.SdClientService = Sd_DynClientService,
	.SdServerService = Sd_DynServerService,
};

const Sd_ConfigType Sd_Config = {
	.Instance = &Sd_InstanceCfg
};

const Sd_DynConfigType Sd_DynConfig =
{
	.Instance = &Sd_DynInstance
};
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

