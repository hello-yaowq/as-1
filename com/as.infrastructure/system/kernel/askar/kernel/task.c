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
#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static TaskVarType TaskVarArray[TASK_NUM];
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
/* |------------------+------------------------------------------------------------| */
/* | Syntax:          | StatusType ActivateTask ( TaskType <TaskID> )              | */
/* |------------------+------------------------------------------------------------| */
/* | Parameter (In):  | TaskID: Task reference                                     | */
/* |------------------+------------------------------------------------------------| */
/* | Parameter (Out): | none                                                       | */
/* |------------------+------------------------------------------------------------| */
/* | Description:     | The task <TaskID> is transferred from the suspended state  | */
/* |                  | into the ready state. The operating system ensures that    | */
/* |                  | the task code is being executed from the first statement.  | */
/* |------------------+------------------------------------------------------------| */
/* | Particularities: | 1) The service may be called from interrupt level and from | */
/* |                  | task level (see Figure 12-1(os223.doc)).                   | */
/* |                  | 2) Rescheduling after the call to ActivateTask depends on  | */
/* |                  | the place it is called from (ISR, non preemptable task,    | */
/* |                  | preemptable task).                                         | */
/* |                  | 3)If E_OS_LIMIT is returned the activation is ignored.     | */
/* |                  | 4)When an extended task is transferred from suspended      | */
/* |                  | state into ready state all its events are cleared.         | */
/* |------------------+------------------------------------------------------------| */
/* | Status:          | Standard:                                                  | */
/* |                  | 1) No error, E_OK                                          | */
/* |                  | 2) Too many task activations of <TaskID>, E_OS_LIMIT       | */
/* |                  | Extended:                                                  | */
/* |                  | 1) Task <TaskID> is invalid, E_OS_ID                       | */
/* |------------------+------------------------------------------------------------| */
/* | Conformance:     | BCC1, BCC2, ECC1, ECC2                                     | */
/* |------------------+------------------------------------------------------------| */
StatusType ActivateTask ( TaskType TaskID )
{
	StatusType ercd = E_OK;
	TaskVarType* pTaskVar;
	imask_t imask;

	#if(OS_STATUS == EXTENDED)
	if( TaskID < TASK_NUM )
	{
	#endif
		pTaskVar   = &TaskVarArray[TaskID];
		Irq_Save(imask);
		if(SUSPENDED == pTaskVar->state)
		{
			RunningVar->state = READY;
			Os_PortInitContext(pTaskVar);

			#ifdef MULTIPLY_TASK_ACTIVATION
			pTaskVar-> activation = 1;
			#endif

			pTaskVar->priority = pTaskVar->pConst->initPriority;

			Sched_AddReady(pTaskVar);
		}
		else
		{
			#ifdef MULTIPLY_TASK_ACTIVATION
			if(pTaskVar->activation < pTaskVar->pConst->maxActivation)
			{
				pTaskVar-> activation++;
				Sched_AddReady(pTaskVar);
			}
			else
			#endif
			{
				ercd = E_OS_LIMIT;
			}
		}

		if( (TCL_TASK == CallLevel) &&
			(ReadyVar->priority > RunningVar->priority) )
		{
			Sched_Preempt();
			Os_PortDispatch();
		}

		Irq_Restore(imask);
	#if(OS_STATUS == EXTENDED)
	}
	else
	{
		ercd = E_OS_ID;
	}
	#endif

	OSErrorOne(ActivateTask,TaskID);

	return ercd;
}

/* |------------------+--------------------------------------------------------------| */
/* | Syntax:          | StatusType TerminateTask ( void )                            | */
/* |------------------+--------------------------------------------------------------| */
/* | Parameter (In):  | none                                                         | */
/* |------------------+--------------------------------------------------------------| */
/* | Parameter (Out): | none                                                         | */
/* |------------------+--------------------------------------------------------------| */
/* | Description:     | This service causes the termination of the calling task. The | */
/* |                  | calling task is transferred from the running state into the  | */
/* |                  | suspended state.                                             | */
/* |------------------+--------------------------------------------------------------| */
/* | Particularities: | 1) An internal resource assigned to the calling task is      | */
/* |                  | automatically released. Other resources occupied by the task | */
/* |                  | shall have been released before the call to TerminateTask.   | */
/* |                  | If a resource is still occupied in standard status the       | */
/* |                  | behaviour is undefined.                                      | */
/* |                  | 2) If the call was successful, TerminateTask does not return | */
/* |                  | to the call level and the status can not be evaluated.       | */
/* |                  | 3) If the version with extended status is used, the service  | */
/* |                  | returns in case of error, and provides a status which can be | */
/* |                  | evaluated in the application.                                | */
/* |                  | 4) If the service TerminateTask is called successfully, it   | */
/* |                  | enforces a rescheduling.                                     | */
/* |                  | 5) Ending a task function without call to TerminateTask or   | */
/* |                  | ChainTask is strictly forbidden and may leave the system in  | */
/* |                  | an undefined state.                                          | */
/* |------------------+--------------------------------------------------------------| */
/* | Status:          | Standard:                                                    | */
/* |                  | 1)No return to call level                                    | */
/* |                  | Extended:                                                    | */
/* |                  | 1) Task still occupies resources, E_OS_RESOURCE              | */
/* |                  | 2) Call at interrupt level, E_OS_CALLEVEL                    | */
/* |------------------+--------------------------------------------------------------| */
/* | Conformance:     | BCC1, BCC2, ECC1, ECC2                                       | */
/* |------------------+--------------------------------------------------------------| */
StatusType TerminateTask( void )
{
	StatusType ercd = E_OK;

	#if(OS_STATUS == EXTENDED)
	if( CallLevel != TCL_TASK )
	{
		ercd = E_OS_CALLEVEL;
	}
	#endif

	if(E_OK == ercd)
	{
		/* release internal resource or NON schedule */
		RunningVar->priority = RunningVar->pConst->initPriority;
		#ifdef MULTIPLY_TASK_ACTIVATION
		asAssert(RunningVar->activation > 0);
		RunningVar->activation--;
		if(RunningVar->activation > 0)
		{
			RunningVar->state = READY;
			Os_PortInitContext(RunningVar);
		}
		else
		#endif
		{
			RunningVar->state = SUSPENDED;
		}

		Sched_GetReady();

		Os_PortStartDispatch();
	}

	OSErrorNone(TerminateTask);
	return ercd;
}

void Os_TaskInit(void)
{
	TaskType id;

	const TaskConstType* pTaskConst;
	TaskVarType* pTaskVar;

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
	}
}
