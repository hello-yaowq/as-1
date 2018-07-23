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

/* design according to ISR 14229 (2006).pdf */
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

#define dcmSetAlarm(Alarm,timeMs)						\
do{												\
	DCM_RTE.timer##Alarm = msToDcmTick(timeMs)+1;	\
}while(0)

#define dcmSignalAlarm(Alarm)					\
do{										\
	if(DCM_RTE.timer##Alarm > 1)				\
	{									\
		DCM_RTE.timer##Alarm --;				\
	}									\
}while(0)
#define dcmCancelAlarm(Alarm) { DCM_RTE.timer##Alarm = 0;}
#define dcmIsAlarmTimeout(Alarm) ( 1u == DCM_RTE.timer##Alarm )
#define dcmIsAlarmStarted(Alarm) ( 0u != DCM_RTE.timer##Alarm )

#define DCM_RTE dcmRTE[Instance]

#define DCM_EOL	(-1)

#ifdef __AS_BOOTLOADER__
#define DCM_GET_SESSION_CHANGE_PERMISSION_FNC BL_GetSessionChangePermission
#else
#define DCM_GET_SESSION_CHANGE_PERMISSION_FNC Dcm_GetSessionChangePermission
#endif

#define DCM_RXSDU   ((const PduInfoType *)DCM_RTE.parameter[0])
#define DCM_RXSDU_SIZE (((const PduInfoType *)DCM_RTE.parameter[0])->SduLength)
#define DCM_RXSDU_DATA (((const PduInfoType *)DCM_RTE.parameter[0])->SduDataPtr)
#define DCM_TXSDU ((const PduInfoType *)DCM_RTE.parameter[1])
#define DCM_TXSDU_SIZE (((const PduInfoType *)DCM_RTE.parameter[1])->SduLength)
#define DCM_TXSDU_DATA (((const PduInfoType *)DCM_RTE.parameter[1])->SduDataPtr)
#define DCM_SESSION_LIST ((const uint8 *)DCM_RTE.parameter[2])
#define DCM_GET_SESSION_CHANGE_PERMISSION ((const Dcm_CallbackGetSesChgPermissionFncType)DCM_RTE.parameter[3])
#define DCM_SECURITY_LEVEL_LIST ((const uint8 *)DCM_RTE.parameter[4])
#define DCM_SECURITY_SEED_SIZE_LIST ((const uint8 *)DCM_RTE.parameter[5])
#define DCM_SECURITY_KEY_SIZE_LIST ((const uint8 *)DCM_RTE.parameter[6])
#define DCM_SECURITY_GET_SEED_FNC_LIST ((const Dcm_CallbackGetSeedFncType*)DCM_RTE.parameter[7])
#define DCM_SECURITY_COMPARE_KEY_FNC_LIST ((const Dcm_CallbackCompareKeyFncType*)DCM_RTE.parameter[8])

#define DCM_S3SERVER_TIMEOUT_MS ((uint32)DCM_RTE.parameter[9])
#define DCM_P2SERVER_TIMEOUT_MS ((uint32)DCM_RTE.parameter[10])

#define DCM_SERVICE_LIST_IN_SESSION ((uint8**)DCM_RTE.parameter[11])
#define DCM_SERVICE_LIST_IN_LEVEL   ((uint8**)DCM_RTE.parameter[12])

#define DCM_INSTANCE_DEFAULT_PARAMETER	\
	{ (Dcm_ParameterType)&rxPduInfo, (Dcm_ParameterType)&txPduInfo, \
	  (Dcm_ParameterType)sesList, (Dcm_ParameterType)DCM_GET_SESSION_CHANGE_PERMISSION_FNC, \
	  (Dcm_ParameterType)secList, (Dcm_ParameterType)secSeedKeySizeList,  (Dcm_ParameterType)secSeedKeySizeList, \
	  (Dcm_ParameterType)getSeedList, (Dcm_ParameterType)compareKeyList, \
	  (Dcm_ParameterType)5000, (Dcm_ParameterType)100, \
	  (Dcm_ParameterType)sesServiseList, (Dcm_ParameterType)secServiseList, \
	}
/* ============================ [ TYPES     ] ====================================================== */
enum {
	DCM_BUFFER_IDLE = 0,
	DCM_BUFFER_PROVIDED,
	DCM_BUFFER_FULL,
};

/* UDT = Upload Download Transfer */
typedef enum
{
	DCM_UDT_IDLE_STATE = 0,
	DCM_UDT_UPLOAD_STATE,
	DCM_UDT_DOWNLOAD_STATE
}Dcm_UDTStateType;

typedef struct
{
	Dcm_UDTStateType  state;
	uint32 memoryAddress;
	uint32 memorySize;
	uint8  blockSequenceCounter;
	uint8  dataFormatIdentifier;
	uint8  memoryIdentifier;
}Dcm_UDTType;

typedef struct
{
	TimerType timerS3Server;
	TimerType timerP2Server;
	const Dcm_ParameterType* parameter;
	PduLengthType rxPduLength;
	PduLengthType txPduLength;
	uint8 currentSID;
	uint8 currentSession;
	uint8 currentLevel;
	uint8 supressPositiveResponse;
	uint8 rxPduState;
	uint8 txPduState;
	uint8 seedRequested;
#if defined(DCM_USE_SERVICE_REQUEST_DOWNLOAD) || defined(DCM_USE_SERVICE_REQUEST_UPLOAD)
	Dcm_UDTType UDTData;
#endif
} Dcm_RuntimeType; /* RTE */
/* ============================ [ DECLARES  ] ====================================================== */
static void SendNRC(PduIdType Instance, uint8 nrc);
static void SendPRC(PduIdType Instance);

#ifdef __AS_BOOTLOADER__
Std_ReturnType BL_GetSessionChangePermission(Dcm_SesCtrlType sesCtrlTypeActive, Dcm_SesCtrlType sesCtrlTypeNew);
Std_ReturnType BL_GetProgramSessionSeed (uint8 *securityAccessDataRecord, uint8 *seed,
		Dcm_NegativeResponseCodeType *errorCode);
Std_ReturnType BL_CompareProgramSessionKey (uint8 *key);
Std_ReturnType BL_GetExtendedSessionSeed (uint8 *securityAccessDataRecord, uint8 *seed,
		Dcm_NegativeResponseCodeType *errorCode);
Std_ReturnType BL_CompareExtendedSessionKey (uint8 *key);
#endif

Std_ReturnType Dcm_GetSessionChangePermission(Dcm_SesCtrlType sesCtrlTypeActive, Dcm_SesCtrlType sesCtrlTypeNew);
Std_ReturnType Dcm_GetSeed(uint8 *securityAccessDataRecord, uint8 *seed, Dcm_NegativeResponseCodeType *errorCode);
Std_ReturnType Dcm_CompareKey(uint8 *key);
/* ============================ [ DATAS     ] ====================================================== */
static Dcm_RuntimeType dcmRTE[DCM_INSTANCE_NUM];
/* uint64 for 8 byte alignment */
static uint64 rxBuffer[(DCM_DEFAULT_RXBUF_SIZE+sizeof(uint64)-1)/sizeof(uint64)];
static uint64 txBuffer[(DCM_DEFAULT_TXBUF_SIZE+sizeof(uint64)-1)/sizeof(uint64)];
static const PduInfoType rxPduInfo =
{
	(uint8*)rxBuffer, sizeof(rxBuffer)
};

static const PduInfoType txPduInfo =
{
	(uint8*)txBuffer, sizeof(txBuffer)
};
static const uint8  sesList[] = { DCM_DEFAULT_SESSION, DCM_PROGRAMMING_SESSION, DCM_EXTENDED_DIAGNOSTIC_SESSION, DCM_EOL };
static const uint8  servicesInDFTS[] =  { SID_DIAGNOSTIC_SESSION_CONTROL, DCM_EOL };
static const uint8  servicesInPRGS[] =  { SID_DIAGNOSTIC_SESSION_CONTROL, SID_SECURITY_ACCESS,
		SID_REQUEST_DOWNLOAD, SID_REQUEST_UPLOAD, SID_TRANSFER_DATA, SID_REQUEST_TRANSFER_EXIT,
		SID_ECU_RESET, SID_ROUTINE_CONTROL, DCM_EOL };
static const uint8  servicesInEXTDS[] = { SID_DIAGNOSTIC_SESSION_CONTROL, SID_SECURITY_ACCESS, DCM_EOL };
static const uint8* sesServiseList[] = { servicesInDFTS, servicesInPRGS, servicesInEXTDS };
static const uint8 secList[] = { 1 /* EXTDS */, 2 /* PRGS */, DCM_EOL };
static const uint8  servicesInLevel1[] = { DCM_EOL };
static const uint8  servicesInLevel2[] = { SID_REQUEST_DOWNLOAD, SID_REQUEST_UPLOAD, SID_TRANSFER_DATA,
		SID_REQUEST_TRANSFER_EXIT, SID_ECU_RESET, SID_ROUTINE_CONTROL, DCM_EOL };
static const uint8* secServiseList[] = { servicesInLevel1, servicesInLevel2 };
static const uint8 secSeedKeySizeList[] = { 4 /* EXTDS */, 4 /* PRGS */ };
#ifdef __AS_BOOTLOADER__
static const Dcm_CallbackGetSeedFncType getSeedList[] = { BL_GetExtendedSessionSeed, BL_GetProgramSessionSeed };
static const Dcm_CallbackCompareKeyFncType compareKeyList[] = { BL_CompareExtendedSessionKey, BL_CompareProgramSessionKey };
#else
static const Dcm_CallbackGetSeedFncType getSeedList[] = { Dcm_GetSeed, Dcm_GetSeed };
static const Dcm_CallbackCompareKeyFncType compareKeyList[] = { Dcm_CompareKey, Dcm_CompareKey };
#endif
static const uint32 dcmInstanceDefaultParameter[] = DCM_INSTANCE_DEFAULT_PARAMETER;

/* ============================ [ LOCALS    ] ====================================================== */
Std_ReturnType __weak Dcm_GetSessionChangePermission(Dcm_SesCtrlType sesCtrlTypeActive, Dcm_SesCtrlType sesCtrlTypeNew)
{
	Std_ReturnType ercd = E_OK;
	return ercd;
}

Std_ReturnType __weak Dcm_GetSeed(uint8 *securityAccessDataRecord, uint8 *seed, Dcm_NegativeResponseCodeType *errorCode)
{
	Std_ReturnType ercd = E_OK;
	*errorCode = DCM_E_POSITIVE_RESPONSE;
	return ercd;
}
Std_ReturnType __weak Dcm_CompareKey(uint8 *key)
{
	Std_ReturnType ercd = E_OK;
	return ercd;
}

#if defined(DCM_USE_SERVICE_REQUEST_DOWNLOAD) \
	|| defined(DCM_USE_SERVICE_REQUEST_UPLOAD)
boolean __weak Dcm_CheckMemory(uint8 attr, uint8 memoryIdentifier, uint32 memoryAddress, uint32 length)
{	/*attr: bit mask 0x04=READ 0x02=WRITE 0x01=EXECUTE */
	return TRUE;
}

boolean __weak Dcm_CheckDataFormatIdentifier(uint8 dataFormatIdentifier)
{
	return TRUE;
}
#endif


static void HandleSubFunction(PduIdType Instance)
{
	if(DCM_RXSDU_DATA[1]&0x80)
	{
		DCM_RXSDU_DATA[1] &= 0x7F;
		DCM_RTE.supressPositiveResponse = TRUE;
	}
}

static uint8 u8IndexOfList(uint8 val, const uint8* pList)
{
	uint8 index = DCM_EOL;
	const uint8* pVar = pList;
	while(0xFF != *pVar)
	{
		if(*pVar == val)
		{
			index = pVar - pList;
			break;
		}
		pVar++;
	}

	return index;
}

static void HandleSessionControl(PduIdType Instance)
{
	uint8 session;
	uint8 index;
	Std_ReturnType ercd;
	if(2 == DCM_RTE.rxPduLength)
	{
		HandleSubFunction(Instance);

		session = DCM_RXSDU_DATA[1];
		index = u8IndexOfList(session, DCM_SESSION_LIST);
		if(index != (uint8)DCM_EOL)
		{
			ercd = DCM_GET_SESSION_CHANGE_PERMISSION(DCM_RTE.currentSession, session);

			if(E_OK == ercd)
			{
				DCM_RTE.currentSession = session;
				Dcm_AppendTX(Instance, session);
				Dcm_AppendTX(Instance, (DCM_P2SERVER_TIMEOUT_MS>>8)&0xFF);
				Dcm_AppendTX(Instance, (DCM_P2SERVER_TIMEOUT_MS)&0xFF);
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
			SendNRC(Instance, DCM_E_SUB_FUNCTION_NOT_SUPPORTED);
		}
	}
	else
	{
		SendNRC(Instance, DCM_E_INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
	}
}

static void HandleSecurityAccess(PduIdType Instance)
{
	uint8 level;
	uint8 index;
	Std_ReturnType ercd;
	Dcm_NegativeResponseCodeType errorCode = DCM_E_REQUEST_OUT_OF_RANGE;
	if(DCM_RTE.rxPduLength >= 2)
	{
		HandleSubFunction(Instance);
		level = (DCM_RXSDU_DATA[1] + 1) >> 1;
		index = u8IndexOfList(level, DCM_SECURITY_LEVEL_LIST);
		if(index != (uint8)DCM_EOL)
		{
			if(DCM_RXSDU_DATA[1]&0x01)
			{	/* requestSeed */
				if(2 != DCM_RTE.rxPduLength)
				{
					SendNRC(Instance, DCM_E_INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
				}
				else
				{
					if(level == DCM_RTE.currentLevel)
					{	/* already unlocked */
						DCM_TXSDU_DATA[1] = DCM_RXSDU_DATA[1];
						memset(&DCM_TXSDU_DATA[2], 0, DCM_SECURITY_SEED_SIZE_LIST[index]);
						DCM_RTE.txPduLength = DCM_SECURITY_SEED_SIZE_LIST[index]+1;
						SendPRC(Instance);
					}
					else
					{
						ercd = DCM_SECURITY_GET_SEED_FNC_LIST[index](NULL, &DCM_TXSDU_DATA[2], &errorCode);
						if(E_OK == ercd)
						{
							DCM_TXSDU_DATA[1] = DCM_RXSDU_DATA[1];
							DCM_RTE.txPduLength = DCM_SECURITY_SEED_SIZE_LIST[index]+1;
							SendPRC(Instance);

							DCM_RTE.seedRequested = TRUE;
						}
						else
						{
							SendNRC(Instance, errorCode);
						}
					}
				}
			}
			else
			{	/* sendKey */
				if((DCM_SECURITY_KEY_SIZE_LIST[index]+2) != DCM_RTE.rxPduLength)
				{
					SendNRC(Instance, DCM_E_INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
				}
				else if(FALSE == DCM_RTE.seedRequested)
				{
					SendNRC(Instance, DCM_E_REQUEST_SEQUENCE_ERROR);
				}
				else
				{
					ercd = DCM_SECURITY_COMPARE_KEY_FNC_LIST[index](&DCM_RXSDU_DATA[2]);
					if(E_OK == ercd)
					{
						DCM_TXSDU_DATA[1] = DCM_RXSDU_DATA[1];
						DCM_RTE.txPduLength = 1;
						SendPRC(Instance);

						DCM_RTE.seedRequested = FALSE;
						DCM_RTE.currentLevel = level;
					}
					else
					{
						SendNRC(Instance, DCM_E_INVALID_KEY);
					}
				}
			}
		}
		else
		{
			SendNRC(Instance, DCM_E_SUB_FUNCTION_NOT_SUPPORTED);
		}
	}
	else
	{
		SendNRC(Instance, DCM_E_INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
	}
}
#if defined(DCM_USE_SERVICE_REQUEST_DOWNLOAD) || defined(DCM_USE_SERVICE_REQUEST_UPLOAD)
static void HandleRequestDownloadOrUpload(PduIdType Instance, Dcm_UDTStateType state)
{
	Std_ReturnType ercd;
	uint8 dataFormatIdentifier;
	uint8 addressFormat;
	uint8 lengthFormat;
	uint8 memoryIdentifier;
	if(DCM_RTE.rxPduLength >= 5)
	{
		dataFormatIdentifier = DCM_RXSDU_DATA[1];
		addressFormat = (uint8)((DCM_RXSDU_DATA[2]>>0u)&0x0Fu);
		lengthFormat  = (uint8)((DCM_RXSDU_DATA[2]>>4u)&0x0Fu);
		memoryIdentifier = DCM_RXSDU_DATA[3+addressFormat+lengthFormat];
		if((addressFormat+lengthFormat+4u) == DCM_RTE.rxPduLength)
		{
			if( (addressFormat<=4) && (lengthFormat<=4) && 	\
				(Dcm_CheckDataFormatIdentifier(dataFormatIdentifier)))
			{
				uint32 memoryAddress=0u,memorySize=0u;
				for(int i=0;i<addressFormat;i++)
				{
					memoryAddress = (memoryAddress<<8) + DCM_RXSDU_DATA[3+i];
				}
				for(int i=0;i<lengthFormat;i++)
				{
					memorySize = (memorySize<<8) + DCM_RXSDU_DATA[3+addressFormat+i];
				}

				if(Dcm_CheckMemory((DCM_UDT_DOWNLOAD_STATE==state)?0x02:0x04,
						memoryIdentifier, memoryAddress, memorySize))
				{
					DCM_RTE.UDTData.state   = state;
					DCM_RTE.UDTData.memoryAddress = memoryAddress;
					DCM_RTE.UDTData.memorySize    = memorySize;
					DCM_RTE.UDTData.dataFormatIdentifier = dataFormatIdentifier;
					DCM_RTE.UDTData.memoryIdentifier = memoryIdentifier;
					DCM_RTE.UDTData.blockSequenceCounter = 1u;

					ASLOG(DCM,"request %s addr(%X) size(%X),memory=%X\n",
							(DCM_UDT_DOWNLOAD_STATE==state)?"download":"upload",
							memoryAddress, memorySize, memoryIdentifier);
					/* create positive response code */
					DCM_TXSDU_DATA[1] = 0x20;  /* lengthFormatIdentifier = 2 Bytes */

					DCM_TXSDU_DATA[2] = (DCM_TXSDU_SIZE >> 8) & 0xFF;
					DCM_TXSDU_DATA[3] = (DCM_TXSDU_SIZE >> 0) & 0xFF;
					DCM_RTE.txPduLength = 3;
					SendPRC(Instance);
				}
				else
				{
					SendNRC(Instance, DCM_E_REQUEST_OUT_OF_RANGE);
				}
			}
			else
			{
				SendNRC(Instance, DCM_E_REQUEST_OUT_OF_RANGE);
			}
		}
		else
		{
			SendNRC(Instance, DCM_E_INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
		}
	}
	else
	{
		SendNRC(Instance, DCM_E_INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
	}
}
#endif

static boolean CheckSessionSecurity(PduIdType Instance)
{
	uint8 index;
	uint8 index2;
	boolean bPassCheck = TRUE;
	index = u8IndexOfList(DCM_RTE.currentSession, DCM_SESSION_LIST);
	if((uint8)DCM_EOL == index)
	{ /* FATAL ERROR */
		SendNRC(Instance, DCM_E_CONDITIONS_NOT_CORRECT);
		bPassCheck = FALSE;
	}
	else
	{
		index = u8IndexOfList(DCM_RTE.currentSID, DCM_SERVICE_LIST_IN_SESSION[index]);
		if((uint8)DCM_EOL == index)
		{
			SendNRC(Instance, DCM_E_SERVICE_NOT_SUPPORTED);
			bPassCheck = FALSE;
		}
		else
		{
			bPassCheck = FALSE;	/* by default, assume this currentSID can run in any security level */
			for(index=0; (DCM_SECURITY_LEVEL_LIST[index] != (uint8)DCM_EOL); index++)
			{
				index2 = u8IndexOfList(DCM_RTE.currentSID, DCM_SERVICE_LIST_IN_LEVEL[index]);
				if((uint8)DCM_EOL != index2)
				{	/* This currentSID hit in one of the level List, need security check */
					bPassCheck = TRUE;
					break;
				}
			}

			if(bPassCheck)
			{	/* Need security check */
				index = u8IndexOfList(DCM_RTE.currentLevel, DCM_SECURITY_LEVEL_LIST);
				if((uint8)DCM_EOL == index)
				{
					SendNRC(Instance, DCM_E_SECUTITY_ACCESS_DENIED);
					bPassCheck = FALSE;
				}
				else
				{
					index = u8IndexOfList(DCM_RTE.currentSID, DCM_SERVICE_LIST_IN_LEVEL[index]);
					if((uint8)DCM_EOL == index)
					{
						SendNRC(Instance, DCM_E_SECUTITY_ACCESS_DENIED);
						bPassCheck = FALSE;
					}
				}
			}
			else
			{
				bPassCheck = TRUE;	/* service can run any level */
			}
		}
	}
	return bPassCheck;
}

static void HandleRequest(PduIdType Instance)
{
	boolean bPassCheck = TRUE;
	DCM_RTE.currentSID = DCM_RXSDU_DATA[0];

	ASLOG(DCM, "Service %02X, L=%d\n", DCM_RTE.currentSID, DCM_RTE.rxPduLength);

	DCM_RTE.txPduLength = 0;
	DCM_RTE.supressPositiveResponse = FALSE;
	dcmSetAlarm(S3Server,DCM_S3SERVER_TIMEOUT_MS);
	dcmSetAlarm(P2Server,DCM_P2SERVER_TIMEOUT_MS);

	bPassCheck = CheckSessionSecurity(Instance);

	if(bPassCheck)
	{
		switch(DCM_RTE.currentSID)
		{
			case SID_DIAGNOSTIC_SESSION_CONTROL:
				HandleSessionControl(Instance);
				break;
			case SID_SECURITY_ACCESS:
				HandleSecurityAccess(Instance);
				break;
			#if defined(DCM_USE_SERVICE_REQUEST_DOWNLOAD)
			case SID_REQUEST_DOWNLOAD:
				HandleRequestDownloadOrUpload(Instance, DCM_UDT_DOWNLOAD_STATE);
				break;
			#endif
			#if defined(DCM_USE_SERVICE_REQUEST_UPLOAD)
			case SID_REQUEST_UPLOAD:
				HandleRequestDownloadOrUpload(Instance, DCM_UDT_UPLOAD_STATE);
				break;
			#endif
			case SID_TRANSFER_DATA:
				break;
			default:
				SendNRC(Instance, DCM_E_SERVICE_NOT_SUPPORTED);
				break;
		}
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
		DCM_TXSDU_DATA[1] = DCM_RTE.currentSID;
		DCM_TXSDU_DATA[2] = nrc;
		DCM_RTE.txPduLength = 3;
		DCM_RTE.txPduState = DCM_BUFFER_FULL;

		HandleTransmit(Instance);

		if(DCM_E_RESPONSE_PENDING != nrc)
		{
			dcmCancelAlarm(P2Server);
		}
		else
		{
			dcmSetAlarm(P2Server,DCM_P2SERVER_TIMEOUT_MS);
		}
	}
}

static void SendPRC(PduIdType Instance)
{ /* send Positive Response Code */
	if(DCM_BUFFER_IDLE == DCM_RTE.txPduState)
	{
		DCM_RTE.rxPduState = DCM_BUFFER_IDLE;
		if(FALSE == DCM_RTE.supressPositiveResponse)
		{
			DCM_TXSDU_DATA[0] = DCM_RTE.currentSID | SID_RESPONSE_BIT;
			DCM_RTE.txPduLength += 1;
			DCM_RTE.txPduState = DCM_BUFFER_FULL;
		}

		HandleTransmit(Instance);

		dcmCancelAlarm(P2Server);
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

void Dcm_SetParameter(PduIdType Instance, const Dcm_ParameterType* parameter)
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
		*pduInfoPtr = (PduInfoType *)DCM_RXSDU;
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
		*pduInfoPtr = (PduInfoType *)DCM_TXSDU;
		DCM_RTE.txPduState = DCM_BUFFER_PROVIDED;
	}
	return ret;
}

void Dcm_MainFunction(void)
{
	PduIdType Instance;
	const Dcm_ParameterType* parameter;
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

		if(dcmIsAlarmStarted(P2Server))
		{
			dcmSignalAlarm(P2Server);
			if(dcmIsAlarmTimeout(P2Server))
			{
				SendNRC(Instance, DCM_E_RESPONSE_PENDING);
			}
		}

		if(dcmIsAlarmStarted(S3Server))
		{
			dcmSignalAlarm(S3Server);
			if(dcmIsAlarmTimeout(S3Server))
			{
				parameter = DCM_RTE.parameter;
				memset(&DCM_RTE, 0, sizeof(DCM_RTE));
				DCM_RTE.parameter = parameter;
				DCM_RTE.currentSession = DCM_DEFAULT_SESSION;
				ASLOG(DCM,"S3Server Timeout!\n");
			}
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
