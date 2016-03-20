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
#include "OsekNm.h"
/* ============================ [ MACROS    ] ====================================================== */
#define LocalNodeId 0x5A
// NM Main Task Tick = 10 ms
#define tTyp 10
#define tMax 26
#define tError 100
#define tWBS   500
#define tTx    2

#define NM_PDUID 0xFF  /* For NM purpose */

#define TaskNmInd 0

#define EventNmNormal   0x1
#define EventNmLimphome 0x2
#define EventNmStatus   0x4
#define EventRingData   0x8
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
uint32 argNMNodeId = NM_PDUID;
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void D_Init(NetIdType NetId,RoutineRefType InitRoutine)
{

}
void D_Offline(NetIdType NetId)
{

}
void D_Online(NetIdType NetId)
{

}
StatusType D_WindowDataReq(NetIdType NetId,NMPduType* NMPDU,uint8 DataLengthTx)
{
	return E_OK;
}


/* Function needed by DLL */
void BusInit(NetIdType NetId)
{

}
void BusSleep(NetIdType NetId)
{

}
void BusAwake(NetIdType NetId)
{

}
void BusRestart(NetIdType NetId)
{

}

TASK(TaskNmInd)
{
	StatusType ercd;
	EventMaskType mask;

	ercd = WaitEvent(EventNmNormal|EventNmLimphome|EventNmStatus|EventRingData);
	if(E_OK == ercd)
	{
		GetEvent(TaskNmInd,&mask);
		if((mask&EventNmNormal) != 0)
		{
			printf("In NM normal state,config changed.\n");
		}
		if((mask&EventNmLimphome) != 0)
		{
			printf("In NM limphome state,config changed.\n");
		}
		if((mask&EventNmStatus) != 0)
		{
			printf("NM network status changed.\n");
		}
		if((mask&EventRingData) != 0)
		{
			printf("NM Ring Data ind.\n");
		}
		(void)ClearEvent(EventNmNormal|EventNmLimphome|EventNmStatus|EventRingData);
	}
	TerminateTask();
}

/* This is an example of how to write NMInit() */
void NMInit(NetIdType NetId)
{
	uint8 config[32];
	NetworkStatusType status;
	(void)memset(config,0x01,32); /*care node :0,8,16,24,32,... */
	if(NetId == 0)
	{
		InitNMType(NetId,NM_DIRECT);
		InitNMScaling(NetId,0xFF); /* TODO: not used */
		InitCMaskTable(NetId,NM_ckNormal,config);
		InitCMaskTable(NetId,NM_ckLimphome,config);
		InitIndDeltaConfig(NetId,NM_ckNormal,SignalActivation,TaskNmInd,EventNmNormal);
		InitIndDeltaConfig(NetId,NM_ckLimphome,SignalActivation,TaskNmInd,EventNmLimphome);
		memset(&status,0,sizeof(NetworkStatusType));
		status.W.NMactive = 1;
		InitSMaskTable(NetId,&status); /* TODO : not implemented for indication */
		InitTargetStatusTable(NetId,&status);
		InitIndDeltaStatus(NetId,SignalActivation,TaskNmInd,EventNmStatus);
		InitDirectNMParams(NetId,argNMNodeId,tTyp,tMax,tError,tWBS,tTx);
		InitIndRingData(NetId,SignalActivation,TaskNmInd,EventRingData);
	}
}
