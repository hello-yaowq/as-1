/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2017  AS <parai@foxmail.com>
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
#include "CanIf.h"
#include "EcuM.h"
#include "asdebug.h"
#ifdef USE_SHELL
#include "shell.h"
#endif
/* ============================ [ MACROS    ] ====================================================== */
#ifndef CAN_STDIO_IBUFFER_SIZE
#define CAN_STDIO_IBUFFER_SIZE 512
#endif
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static uint32_t rpos=0;
static uint32_t wpos=0;
static volatile uint32_t isize=0;
static char     ibuffer[CAN_STDIO_IBUFFER_SIZE];
static uint32_t wmissing=0;
/* ============================ [ LOCALS    ] ====================================================== */
static boolean is_can_online(void)
{
	boolean online = FALSE;
	Std_ReturnType ercd;
	CanIf_ChannelGetModeType mode;
	EcuM_StateType state;

	EcuM_GetState(&state);
	if(ECUM_STATE_APP_RUN == state)
	{
		ercd = CanIf_GetPduMode(CANIF_CHL_LS, &mode);

		if(E_OK == ercd)
		{
			if((CANIF_GET_TX_ONLINE == mode) || (CANIF_GET_ONLINE == mode))
			{
				online = TRUE;
			}
		}
	}

	return online;
}

static void flush_can(void)
{
	PduInfoType pdu;
	uint8 data[8];
	int sz;
	int index;
	int trpos;
	Std_ReturnType ercd;
	imask_t imask;

	if(FALSE == is_can_online())
	{
		return;
	}

	sz = isize;
	if(sz > 8)
	{
		sz = 8;
	}

	if(sz > 0)
	{
		pdu.SduDataPtr = data;
		pdu.SduLength = sz;

		index = 0;
		trpos = rpos;
		while(sz > index)
		{
			data[index] = ibuffer[trpos];
			trpos ++;
			if(trpos >= CAN_STDIO_IBUFFER_SIZE)
			{
				trpos = 0;
			}
			index ++;
		}

		ercd = CanIf_Transmit(CANIF_ID_STDOUT, &pdu);
		if(E_OK == ercd)
		{
			Irq_Save(imask);
			rpos = trpos;
			isize -= sz;
			Irq_Restore(imask);
		}
	}
}
/* ============================ [ FUNCTIONS ] ====================================================== */

void Can_putc(char ch)
{
	imask_t imask;

#ifndef CAN_STDIO_IBUFFER_FULL_IGNORE
	while((isize >= CAN_STDIO_IBUFFER_SIZE) && is_can_online())
	{
		Can_MainFunction_Write();
#if defined(CANIF_TASK_FIFO_MODE) && (CANIF_TASK_FIFO_MODE == STD_ON)
		CanIf_MainFunction();
#endif
		flush_can();
	}
#endif

	if(isize < CAN_STDIO_IBUFFER_SIZE)
	{
		Irq_Save(imask);
		ibuffer[wpos] = ch;
		wpos ++;
		if(wpos >= CAN_STDIO_IBUFFER_SIZE)
		{
			wpos = 0;
		}
		isize ++;
		Irq_Restore(imask);
	}
	else
	{
		/* do noting as full */
		wmissing ++;
	}

	if( ('\r' == ch) || ('\n' == ch) || (isize >= 8) )
	{
		flush_can();
	}

}


void CanIf_StdioRxIndication(uint8 channel, PduIdType pduId, const uint8 *sduPtr, uint8 dlc, Can_IdType canId)
{
#ifdef USE_SHELL
	int i;

	for(i=0; i< dlc; i++)
	{
		SHELL_input(sduPtr[i]);
	}
#endif
}

void CanIf_StdioTxConfirmation(PduIdType id)
{
	(void) id;
	if(isize > 0)
	{
		flush_can();
	}
	else if(wmissing > 0)
	{
		printf("CANIO missing %d!\n", wmissing);
		wmissing = 0;
	}
	else
	{
		/* nothing */
	}
}
