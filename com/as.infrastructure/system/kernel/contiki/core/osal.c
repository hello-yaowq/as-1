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
 *
 * osal: Operating System Abstraction Layer
 */
/* ============================ [ INCLUDES  ] ====================================================== */
#include "Os.h"
#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */
#define AS_LOG_OS 1
/* ============================ [ TYPES     ] ====================================================== */
typedef struct
{
	struct ctimer timer;
	TickType period;
}AlarmContrlBlockType;
/* ============================ [ DECLARES  ] ====================================================== */
extern CONST(alarm_declare_t,AUTOMATIC) AlarmList[ALARM_NUM];
extern struct process * const TaskList[TASK_NUM];
/* ============================ [ DATAS     ] ====================================================== */
PROCINIT(&etimer_process);
static AlarmContrlBlockType alarmcb[ALARM_NUM];	/* alarm control block */

TickType OsTickCounter = 0;
/* ============================ [ LOCALS    ] ====================================================== */
static void AlarmProcess(void* param)
{
	AlarmType AlarmId = (AlarmType)param;

	AlarmList[AlarmId].main();

	if(0 != alarmcb[AlarmId].period)
	{
		ctimer_set(&(alarmcb[AlarmId].timer),alarmcb[AlarmId].period,AlarmProcess,(void*)AlarmId);
	}
}
/* ============================ [ FUNCTIONS ] ====================================================== */
void OsTick ( void )
{
	OsTickCounter ++;
}

TickType GetOsTick(void)
{
	return OsTickCounter;
}

void ShutdownOS(StatusType ercd)
{

}

void StartOS(AppModeType app_mode)
{
	process_init();

	procinit_init();

	ctimer_init();

	StartupHook();

	autostart_start(autostart_processes);


	while(1) {

		process_run();

		etimer_request_poll();

		KSM_EXECUTE();

	}
}

void Schedule(void)
{
	process_run();

	etimer_request_poll();

	KSM_EXECUTE();
}

FUNC(StatusType,MEM_SetRelAlarm) SetRelAlarm ( AlarmType AlarmId, TickType Increment, TickType Cycle )
{
	asAssert(AlarmId<ALARM_NUM);

	alarmcb[AlarmId].period = Cycle;

	ctimer_set(&(alarmcb[AlarmId].timer),Increment,AlarmProcess,(void*)AlarmId);

	return E_OK;
}

FUNC(StatusType,MEM_SetAbsAlarm) SetAbsAlarm ( AlarmType AlarmId, TickType Start, TickType Cycle )
{
	asAssert(AlarmId<ALARM_NUM);

	return E_OK;
}

FUNC(StatusType,MEM_CancelAlarm) CancelAlarm ( AlarmType AlarmId )
{
	asAssert(AlarmId<ALARM_NUM);

	return E_OK;
}

FUNC(StatusType,MEM_ACTIVATE_TASK) 	 ActivateTask    ( TaskType TaskId)
{
	StatusType ercd = E_OK;

	if(TaskId < TASK_NUM)
	{
		process_start(TaskList[TaskId], NULL);
	}
	else
	{
		ercd = E_OS_ID;
	}
	return ercd;
}

imask_t __Irq_Save(void)
{
	return 0;
}
void Irq_Restore(imask_t irq_state)
{
	(void)irq_state;
}

/* contiki stubs */
int nullradio_driver;
int uip_ds6_if;
void dhcpc_configured(void) { asAssert(0); }
void dhcpc_unconfigured(void) { asAssert(0); }
void rpl_link_neighbor_callback(void) { asAssert(0); }
void rpl_ipv6_neighbor_callback(void) { asAssert(0); }
void uip_ds6_defrt_lookup(void) { asAssert(0); }
void uip_ds6_defrt_rm(void) { asAssert(0); }
void uip_nd6_ns_output(void) { asAssert(0); }

