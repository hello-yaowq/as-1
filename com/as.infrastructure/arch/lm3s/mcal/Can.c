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
#include "Mcu.h"
#include "CanIf_Cbk.h"
#if defined(USE_DET)
#include "Det.h"
#endif
#if defined(USE_DEM)
#include "Dem.h"
#endif
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "Os.h"
#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
#define CAN_SERIAL_Q_SIZE 32
#define CAN_EMPTY_MESSAGE_BOX 0xFFFF
typedef struct {
    uint32_t 	canid;
    uint8_t     busid;
    uint8_t		dlc;
    uint8_t 	data[8];
} Can_SerialPduType;

static char r_cache[32];
static uint32_t  r_size;

static Can_SerialPduType rQ[CAN_SERIAL_Q_SIZE];
static uint32_t r_pos;
static uint32_t w_pos;
static volatile uint32_t r_counter;
static PduIdType swPduHandle;;
static uint32_t IntH(char chr)
{
	uint32_t v;
	if( (chr>='0') && (chr<='9'))
	{
		v= chr - '0';
	}
	else if( (chr>='A') && (chr<='F'))
	{
		v= chr - 'A' + 10;
	}
	else if( (chr>='a') && (chr<='f'))
	{
		v= chr - 'a' + 10;
	}
	else
	{
		ASWARNING("CAN serial receiving invalid data '0x%02X', cast to 0\n",chr);
		v = 0;
	}

	return v;
}
static void rx_notifiy( void )
{
	Can_SerialPduType* pdu;
	uint32_t i;
	if( (r_size >= 12) &&
		('S' == r_cache[0]) &&
		('C' == r_cache[1]) &&
		('A' == r_cache[2]) &&
		('N' == r_cache[3]) )
	{
		if ( r_counter < CAN_SERIAL_Q_SIZE )
		{
			pdu = &rQ[w_pos];
			pdu->busid = IntH(r_cache[4])*16 + IntH(r_cache[5]);
			pdu->canid = IntH(r_cache[6])*16*16*16 + IntH(r_cache[7])*16*16 + IntH(r_cache[8])*16 + IntH(r_cache[9]);
			pdu->dlc   = IntH(r_cache[10])*16 + IntH(r_cache[11]);
			if((pdu->dlc>0) &&(pdu->dlc<=8))
			{
				for(i=0;i<pdu->dlc;i++)
				{
					pdu->data[i] = IntH(r_cache[12+2*i])*16 + IntH(r_cache[13+2*i]);
				}
				r_counter ++;
				w_pos ++;
				if(w_pos >= CAN_SERIAL_Q_SIZE)
				{
					w_pos = 0;
				}
			}
			else
			{
				ASWARNING("CAN serial receiving data with invalid dlc = %d\n",(int)pdu->dlc);
			}
		}
		else
		{
			ASWARNING("CAN serial receiving queue full, missing message\n");
		}
	}
	else
	{
		ASWARNING("CAN serial receiving invalid data:: %d/%s\n",r_size,r_cache);
	}

	r_size = 0;
}

void Can_Init( const Can_ConfigType *Config )
{
  // Do initial configuration of layer here
	(void)Config;
	r_size = 0;
	r_pos = 0;
	w_pos = 0;
	r_counter = 0;
	swPduHandle = CAN_EMPTY_MESSAGE_BOX;
}

void Can_InitController( uint8 controller, const Can_ControllerConfigType *config)
{
	(void)controller;
	(void)config;
}

Can_ReturnType Can_SetControllerMode( uint8 Controller, Can_StateTransitionType transition )
{
	(void)Controller;
	switch(transition )
	{
		case CAN_T_START:	/* no break here */
		case CAN_T_WAKEUP:
		break;
		case CAN_T_SLEEP:
		case CAN_T_STOP:
		default:
		break;
	}
	return E_OK;
}

Can_ReturnType Can_Write( Can_Arc_HTHType hth, Can_PduType *pduInfo )
{
	uint32_t i;
	imask_t imask;
	Can_ReturnType rv = CAN_OK;

	Irq_Save(imask);

	if(CAN_EMPTY_MESSAGE_BOX == swPduHandle)
	{
		printf("SCAN%02X%04X%02X",hth,(unsigned int)pduInfo->id,pduInfo->length);
		asAssert(pduInfo->length <= 8);
		for(i=0;i<pduInfo->length;i++)	/* maximum 16 */
		{
			printf("%02X",pduInfo->sdu[i]);
		}
		printf("\n");

		swPduHandle = pduInfo->swPduHandle;
	}
	else
	{
		rv = CAN_BUSY;
	}

	Irq_Restore(imask);

	return rv;
}

void Can_DisableControllerInterrupts( uint8 controller )
{
	(void)controller;
}

void Can_EnableControllerInterrupts( uint8 controller )
{
	(void)controller;
}


// Hth - for Flexcan, the hardware message box number... .We don't care
void Can_Cbk_CheckWakeup( uint8 controller ){(void)controller;}

void Can_MainFunction_Write( void )
{
	if(CAN_EMPTY_MESSAGE_BOX != swPduHandle)
	{
		CanIf_TxConfirmation(swPduHandle);
		swPduHandle = CAN_EMPTY_MESSAGE_BOX;
	}
}

void Can_MainFunction_Read( void )
{
	imask_t imask;
	Can_SerialPduType* pdu;

	if(0)
	{

		char chr = (char)0;
		r_cache[r_size++] = chr;

		if('\n' == chr)
		{
			rx_notifiy();
		}

		if(r_size >= sizeof(r_cache))
		{
			r_size = 0;
		}
	}

	while(r_counter > 0)
	{
		Irq_Save(imask);
		r_counter --;
		pdu = &rQ[r_pos++];
		if(r_pos >= CAN_SERIAL_Q_SIZE)
		{
			r_pos = 0;
		}
		Irq_Restore(imask);

		CanIf_RxIndication(pdu->busid,pdu->canid,pdu->dlc,pdu->data);
	}


}
void Can_MainFunction_BusOff( void ){}
void Can_MainFunction_Wakeup( void ){}
void Can_MainFunction_Error ( void ){}

void Can_Arc_GetStatistics( uint8 controller, Can_Arc_StatisticsType * stat){(void)controller;(void)stat;}
