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
#include "Dcm.h"
#include "Dcm_Internal.h"
#include "CanTp.h"
#include "Os.h"
#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */
#define AS_LOG_DCM  1
#define AS_LOG_DCME 1

#ifndef DCM_INSTANCE_NUM
#define DCM_INSTANCE_NUM 1
#endif

#ifndef DCM_DEFAULT_RXBUF_SIZE
#define DCM_DEFAULT_RXBUF_SIZE 4095
#endif
#ifndef DCM_DEFAULT_TXBUF_SIZE
#define DCM_DEFAULT_TXBUF_SIZE 4095
#endif

#ifndef DCM_MAIN_FUNCTION_PERIOD
#define DCM_MAIN_FUNCTION_PERIOD 10
#endif
#define msToDcmTick(__ms) (((__ms)+DCM_MAIN_FUNCTION_PERIOD-1)/DCM_MAIN_FUNCTION_PERIOD)

#define DCM_RTE dcmRTE[Instance]

#define DCM_RXSDU   (PduInfoType *)DCM_RTE.parameter[0]
#define DCM_RXSDU_SIZE (((PduInfoType *)DCM_RTE.parameter[0])->SduLength)
#define DCM_RXSDU_DATA (((PduInfoType *)DCM_RTE.parameter[0])->SduDataPtr)
#define DCM_TXSDU (PduInfoType *)DCM_RTE.parameter[1]
#define DCM_TXSDU_SIZE (((PduInfoType *)DCM_RTE.parameter[1])->SduLength)
#define DCM_TXSDU_DATA (((PduInfoType *)DCM_RTE.parameter[1])->SduDataPtr)

#define DCM_INSTANCE_DEFAULT_PARAMETER	\
	{ (Dcm_ParameterType)&rxPduInfo, (Dcm_ParameterType)&txPduInfo }
/* ============================ [ TYPES     ] ====================================================== */
typedef uint32 Dcm_ParameterType;

enum {
	DCM_BUFFER_IDLE = 0,
	DCM_BUFFER_PROVIDED,
	DCM_BUFFER_FULL,
};

typedef struct
{
	TimerType timer;
	const Dcm_ParameterType* parameter;
	PduLengthType rxPduLength;
	PduLengthType txPduLength;
	uint8 sid;
	uint8 currentSession;
	uint8 currentLevel;
	uint8 supressPositiveResponse;
	uint8 rxPduState;
	uint8 txPduState;
} Dcm_RuntimeType; /* RTE */
/* ============================ [ DECLARES  ] ====================================================== */
static void SendNRC(PduIdType Instance, uint8 nrc);
static void SendPRC(PduIdType Instance);

Std_ReturnType Dcm_GetSessionChangePermission(Dcm_SesCtrlType sesCtrlTypeActive, Dcm_SesCtrlType sesCtrlTypeNew);
/* ============================ [ DATAS     ] ====================================================== */
static Dcm_RuntimeType dcmRTE[DCM_INSTANCE_NUM];
static uint8 rxBuffer[DCM_DEFAULT_RXBUF_SIZE];
static uint8 txBuffer[DCM_DEFAULT_TXBUF_SIZE];
static const PduInfoType rxPduInfo =
{
	rxBuffer, sizeof(rxBuffer)
};

static const PduInfoType txPduInfo =
{
	txBuffer, sizeof(txBuffer)
};
static const uint32 dcmInstanceDefaultParameter[] = DCM_INSTANCE_DEFAULT_PARAMETER;

/* ============================ [ LOCALS    ] ====================================================== */
Std_ReturnType __weak Dcm_GetSessionChangePermission(Dcm_SesCtrlType sesCtrlTypeActive, Dcm_SesCtrlType sesCtrlTypeNew)
{
	Std_ReturnType ercd = E_NOT_OK;
#ifdef __AS_BOOTLOADER__
	extern Std_ReturnType BL_GetSessionChangePermission(Dcm_SesCtrlType sesCtrlTypeActive, Dcm_SesCtrlType sesCtrlTypeNew);
	ercd = BL_GetSessionChangePermission(sesCtrlTypeActive, sesCtrlTypeNew);
#endif
	return ercd;
}
static void HandleSessionControl(PduIdType Instance)
{
	uint8 session;
	Std_ReturnType ercd;
	if(2 != DCM_RXSDU_SIZE)
	{
		session = DCM_RXSDU_DATA[1];

		ercd = Dcm_GetSessionChangePermission(DCM_RTE.currentSession, session);

		if(E_OK == ercd)
		{
			Dcm_AppendTX(Instance, session);
			Dcm_AppendTX(Instance, 0x00);
			Dcm_AppendTX(Instance, 0x64);
			Dcm_AppendTX(Instance, 0x00);
			Dcm_AppendTX(Instance, 0x0A);
			SendPRC(Instance);
		}
		else
		{
			SendNRC(Instance, DCM_E_CONDITIONS_NOT_CORRECT);
		}
	}
	else
	{
		SendNRC(Instance, DCM_E_INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
	}
}
static void HandleRequest(PduIdType Instance)
{
	DCM_RTE.sid = DCM_RXSDU_DATA[0];

	ASLOG(DCM, "Service %02X\n", DCM_RTE.sid);

	DCM_RTE.txPduLength = 0;
	DCM_RTE.supressPositiveResponse = FALSE;
	switch(DCM_RTE.sid)
	{
		case SID_DIAGNOSTIC_SESSION_CONTROL:
			HandleSessionControl(Instance);
			break;
		default:
			SendNRC(Instance, DCM_E_SERVICE_NOT_SUPPORTED);
			break;
	}
}

static void HandleTransmit(PduIdType Instance)
{
	Std_ReturnType ercd;
	PduInfoType pdu;

	pdu.SduLength = DCM_RTE.txPduLength;
	pdu.SduDataPtr = DCM_TXSDU_DATA;
	ercd = CanTp_Transmit(Instance+DCM_INSTANCE_NUM, &pdu);

	if(E_OK == ercd)
	{
		/* nothing */
	}
}

static void SendNRC(PduIdType Instance, uint8 nrc) /* send Negative Response Code */
{
	if(DCM_BUFFER_IDLE == DCM_RTE.txPduState)
	{
		DCM_RTE.rxPduState = DCM_BUFFER_IDLE;

		DCM_TXSDU_DATA[0] = SID_NEGATIVE_RESPONSE;
		DCM_TXSDU_DATA[1] = DCM_RTE.sid;
		DCM_TXSDU_DATA[2] = nrc;
		DCM_RTE.txPduLength = 3;
		DCM_RTE.txPduState = DCM_BUFFER_FULL;

		HandleTransmit(Instance);
	}
}

static void SendPRC(PduIdType Instance)
{ /* send Positive Response Code */
	if(DCM_BUFFER_IDLE == DCM_RTE.txPduState)
	{
		DCM_RTE.rxPduState = DCM_BUFFER_IDLE;
		if(FALSE == DCM_RTE.supressPositiveResponse)
		{
			DCM_TXSDU_DATA[0] = DCM_RTE.sid | SID_RESPONSE_BIT;
			DCM_RTE.txPduLength += 1;
			DCM_RTE.txPduState = DCM_BUFFER_FULL;
		}

		HandleTransmit(Instance);
	}
}
/* ============================ [ FUNCTIONS ] ====================================================== */
void Dcm_Init(void)
{
	PduIdType Instance;
	for(Instance=0; Instance<DCM_INSTANCE_NUM; Instance++)
	{
		memset(&DCM_RTE, 0, sizeof(DCM_RTE));
		DCM_RTE.parameter = dcmInstanceDefaultParameter;
		DCM_RTE.currentSession = DCM_DEFAULT_SESSION;
	}
}

void Dcm_SetParameter(PduIdType Instance, const uint32* parameter)
{
	asAssert(Instance < DCM_INSTANCE_NUM);
	DCM_RTE.parameter = parameter;
}

void Dcm_AppendTX(PduIdType Instance, uint8 v)
{
	DCM_TXSDU_DATA[1+DCM_RTE.txPduLength] = v;
	DCM_RTE.txPduLength ++;
}

void Dcm_RxIndication(PduIdType Instance, NotifResultType result)
{
	asAssert(Instance < DCM_INSTANCE_NUM);

	if(result == NTFRSLT_OK)
	{
		DCM_RTE.rxPduState = DCM_BUFFER_FULL;
	}
	else
	{
		DCM_RTE.rxPduState = DCM_BUFFER_IDLE;
	}
}

void Dcm_TxConfirmation(PduIdType Instance, NotifResultType result)
{
	asAssert(Instance < DCM_INSTANCE_NUM);

	if(result == NTFRSLT_OK)
	{

	}
	else
	{

	}

	DCM_RTE.txPduState = DCM_BUFFER_IDLE;
}

BufReq_ReturnType Dcm_ProvideRxBuffer(PduIdType Instance, PduLengthType tpSduLength, PduInfoType **pduInfoPtr)
{
	BufReq_ReturnType ret = BUFREQ_OK;

	asAssert(Instance < DCM_INSTANCE_NUM);

	if(DCM_BUFFER_IDLE != DCM_RTE.rxPduState)
	{
		ret = BUFREQ_BUSY;
	}
	else if(NULL == pduInfoPtr)
	{
		ret = BUFREQ_NOT_OK;
	}
	else if((tpSduLength > DCM_RXSDU_SIZE) || (0 == tpSduLength))
	{
		ret = BUFREQ_OVFL;
	}
	else
	{
		*pduInfoPtr = DCM_RXSDU;
		DCM_RTE.rxPduLength = tpSduLength;
		DCM_RTE.rxPduState = DCM_BUFFER_PROVIDED;
	}

	return ret;
}

BufReq_ReturnType Dcm_ProvideTxBuffer(PduIdType Instance, PduInfoType **pduInfoPtr, PduLengthType length)
{
	BufReq_ReturnType ret = BUFREQ_OK;

	asAssert(Instance < DCM_INSTANCE_NUM);

	if(DCM_BUFFER_FULL != DCM_RTE.txPduState)
	{
		ret = BUFREQ_BUSY;
	}
	else if(NULL == pduInfoPtr)
	{
		ret = BUFREQ_NOT_OK;
	}
	else if((length > DCM_TXSDU_SIZE) || (0 == length))
	{
		ret = BUFREQ_OVFL;
	}
	else
	{
		*pduInfoPtr = DCM_TXSDU;
		DCM_RTE.txPduState = DCM_BUFFER_PROVIDED;
	}
	return ret;
}

void Dcm_MainFunction(void)
{
	PduIdType Instance;
	for(Instance=0; Instance<DCM_INSTANCE_NUM; Instance++)
	{
		if(DCM_BUFFER_FULL == DCM_RTE.rxPduState)
		{
			HandleRequest(Instance);
		}

		if(DCM_BUFFER_FULL == DCM_RTE.txPduState)
		{
			HandleTransmit(Instance);
		}
	}
}

Std_ReturnType Dcm_GetSecurityLevel(Dcm_SecLevelType *secLevel)
{
	Std_ReturnType ercd = E_NOT_OK;
	PduIdType Instance;

	*secLevel = 0;
	for(Instance=0; Instance<DCM_INSTANCE_NUM; Instance++)
	{
		if(DCM_BUFFER_FULL == DCM_RTE.rxPduState)
		{	/* This is active instance */
			*secLevel = DCM_RTE.currentLevel;
			ercd = E_OK;
			break;
		}
	}

	return ercd;
}
