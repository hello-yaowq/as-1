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
#include "kernel_internal.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
/* |------------------+----------------------------------------------------------| */
/* | Syntax:          | StatusType SetEvent ( TaskType <TaskID>                  | */
/* |                  | EventMaskType <Mask> )                                   | */
/* |------------------+----------------------------------------------------------| */
/* | Parameter (In):  | TaskID:Reference to the task for which one or several    | */
/* |                  | events are to be set.                                    | */
/* |                  | Mask:Mask of the events to be set                        | */
/* |------------------+----------------------------------------------------------| */
/* | Parameter (Out): | none                                                     | */
/* |------------------+----------------------------------------------------------| */
/* | Description:     | 1.The service may be called from an interrupt service    | */
/* |                  | routine and from the task level, but not from hook       | */
/* |                  | routines.                                                | */
/* |                  | 2.The events of task <TaskID> are set according to the   | */
/* |                  | event mask <Mask>. Calling SetEvent causes the task      | */
/* |                  | <TaskID> to be transferred to the ready state, if it     | */
/* |                  | was waiting for at least one of the events specified     | */
/* |                  | in <Mask>.                                               | */
/* |------------------+----------------------------------------------------------| */
/* | Particularities: | Any events not set in the event mask remain unchanged.   | */
/* |------------------+----------------------------------------------------------| */
/* | Status:          | Standard: 1.No error, E_OK                               | */
/* |                  | Extended: 2.Task <TaskID> is invalid, E_OS_ID            | */
/* |                  | 3.Referenced task is no extended task, E_OS_ACCESS       | */
/* |                  | 4.Events can not be set as the referenced task is in the | */
/* |                  | suspended state, E_OS_STATE                              | */
/* |------------------+----------------------------------------------------------| */
/* | Conformance:     | ECC1, ECC2                                               | */
/* |------------------+----------------------------------------------------------| */
StatusType SetEvent  ( TaskType TaskID , EventMaskType Mask )
{
	StatusType ercd = E_OK;
	#if(OS_STATUS == EXTENDED)
	if( TaskID >= TASK_NUM )
	{
		ercd = E_OS_ID;
	}
	else if( NULL == TaskConstArray[TaskID].pEventVar )
	{
		ercd = E_OS_ACCESS;
	}
	else if( SUSPENDED == TaskVarArray[TaskID].state )
	{
		ercd = E_OS_STATE;
	}
	#endif

	if( E_OK == ercd )
	{
		
	}

	OSErrorTwo(SetEvent, TaskID, Mask);
	return ercd;
}

StatusType ClearEvent( EventMaskType Mask )
{
	StatusType ercd = E_OK;


	return ercd;
}

StatusType GetEvent  ( TaskType TaskID , EventMaskRefType pEvent )
{
	StatusType ercd = E_OK;


	return ercd;
}

StatusType WaitEvent ( EventMaskType Mask )
{
	StatusType ercd = E_OK;


	return ercd;
}

