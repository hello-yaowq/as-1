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
TaskVarType* RunningVar;
unsigned int CallLevel;
boolean      NeedSched;

static TaskVarType TaskVarArray[TASK_NUM];
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
StatusType ActivateTask ( TaskType TaskID )
{
	StatusType ercd = E_OK;

	TaskVarType* pTaskVar;

	if( TaskID < TASK_NUM )
	{
		pTaskVar   = &TaskVarArray[TaskID];

		if(SUSPENDED == pTaskVar->state)
		{
			Os_PortInitContext(pTaskVar);
		}
		else
		{
			ercd = E_OS_LIMIT;
		}
	}
	else
	{
		ercd = E_OS_ID;
	}
	return ercd;
}

StatusType TerminateTask( void )
{
	StatusType ercd = E_OK;

	return ercd;
}

void Os_TaskInit(void)
{
	TaskType id;

	const TaskConstType* pTaskConst;
	TaskVarType* pTaskVar;

	RunningVar = NULL;
	NeedSched = FALSE;

	for(id=0; id < TASK_NUM; id++)
	{
		pTaskConst = &TaskConstArray[id];
		pTaskVar   = &TaskVarArray[id];

		pTaskVar->state = SUSPENDED;
		pTaskVar->pConst = pTaskConst;

		if(TRUE == pTaskConst->autoStart)
		{
			(void)ActivateTask(id);
		}

		RunningVar = pTaskVar;
	}
}
