/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2016  AS <parai@foxmail.com>
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
#ifdef USE_SOAD
#include "SoAd.h"
#include "PduR.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static const SoAd_SocketConnectionType SoAd_SocketConnection [SOAD_SOCKET_COUNT] =
{
	{
		.SocketRemoteIpAddress = "172.18.0.200",
		.SocketRemotePort = 8989,
		.SocketProtocol = SOAD_SOCKET_PROT_TCP,
		.SocketLocalPort = 8989,
		.AutosarConnectorType = SOAD_AUTOSAR_CONNECTOR_DOIP,
	}
};
static const DoIp_TargetAddressConfigType SoAd_DoIpTargetAddresses[SOAD_SOCKET_COUNT]=
{
	{
		.addressValue=0xfeed,  /* this 16 bit ta */
		.txPdu=PDUR_ID_SOAD_TX,
		.rxPdu=PDUR_ID_SOAD_RX

	},
};
static const DoIp_TesterConfigType SoAd_DoIpTesters[DOIP_TESTER_COUNT] =
{
	{
		.address = 0xdead,
	},
	{
		.address = 0xbeef,
	}
};
static const DoIp_RoutingActivationConfigType SoAd_DoIpRoutingActivations[DOIP_ROUTINGACTIVATION_COUNT] =
{
	{
		.activationNumber = 0xda,
		.authenticationCallback = NULL,
		.confirmationCallback = NULL
	},
	{
		.activationNumber = 0xbe,
		.authenticationCallback = NULL,
		.confirmationCallback = NULL
	},
};
static const DoIp_RoutingActivationToTargetAddressMappingType SoAd_DoIpRoutingActivationToTargetAddressMap[DOIP_ROUTINGACTIVATION_TO_TARGET_RELATION_COUNT] =
{
	{
		.routingActivation = 0,
		.target = 0,
	},
	{
		.routingActivation = 1,
		.target = 0,
	},
};
const SoAd_ConfigType SoAd_Config =
{
	.SocketConnection = SoAd_SocketConnection,
	.DoIpTargetAddresses = SoAd_DoIpTargetAddresses,
	.DoIpTesters= SoAd_DoIpTesters,
	.DoIpRoutingActivations = SoAd_DoIpRoutingActivations,
	.DoIpRoutingActivationToTargetAddressMap = SoAd_DoIpRoutingActivationToTargetAddressMap
};
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
Std_ReturnType SoAd_DoIp_Arc_GetVin(uint8* buf, uint8 len)
{
	return E_OK;
}

Std_ReturnType SoAd_DoIp_Arc_GetEid(uint8* buf, uint8 len)
{
	return E_OK;
}

Std_ReturnType SoAd_DoIp_Arc_GetGid(uint8* buf, uint8 len)
{
	return E_OK;
}

Std_ReturnType SoAd_DoIp_Arc_GetFurtherActionRequired(uint8* buf)
{
	return E_OK;
}
#endif
