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
#include "Std_Types.h"
#include "Os.h"
#include "kernel_impl.h"
#include "osap.h"
#include "counter.h"
#include "interrupt.h"
#include "scheduletable.h"
#include "task.h"
#include "alarm.h"
#include "ioc_impl.h"
#include "memory.h"
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
const TaskType tnum_task    = 0;
const TaskType tnum_exttask = 0;
TCB			*p_runtsk;
TCB			*p_schedtsk;
PriorityType	nextpri;
QUEUE		ready_queue[TNUM_TPRI];
uint16		ready_primap;
const TINIB	tinib_table[1];
TCB			tcb_table[1];

const AppModeType	tnum_appmode = 1;

const InterruptNumberType	tnum_intno = 0;
const INTINIB				intinib_table[1];

const CounterType	tnum_hardcounter=0;
const CounterType	tnum_counter=0;
const CNTINIB		cntinib_table[1];
CNTCB				cntcb_table[1];
const HWCNTINIB		hwcntinib_table[1];

const ISRType	tnum_isr2=0;                         /* C2ISRの数 */
const ISRINIB	isrinib_table[1];
ISRCB			isrcb_table[1];
ISRCB			*p_runisr;
uint8			sus_all_cnt;
uint8			sus_os_cnt;
PriorityType		sus_os_prevpri;
const TickType	osinterruptlock_budget;
TickType			os_difftime;
MonitoringType	os_saved_watchtype;

const ScheduleTableType	tnum_scheduletable=0;
const ScheduleTableType	tnum_implscheduletable=0;
const SCHTBLINIB			schtblinib_table[1];
SCHTBLCB					schtblcb_table[1];


const ApplicationType			tnum_osap=0;
const TrustedFunctionIndexType	tnum_tfn=0;
OSAPCB			osapcb_table[1];
OSAPCB			*p_runosap;
const OSAPINIB	osapinib_table[1];
const TFINIB		tfinib_table[1];


const AlarmType	tnum_alarm=0;
const ALMINIB	alminib_table[1];
ALMCB			almcb_table[1];

const IocType	tnum_ioc=0;
const IocType	tnum_queueioc=0;
const IOCWRPINIB	iocwrpinib_table[1];
const IOCINIB	iocinib_table[1];
IOCCB			ioccb_table[1];
const IOCWRPINIB	iocwrpinib_table[1];
void				*ioc_inival_table[1];

const ResourceType	tnum_stdresource=0;
const RESINIB		resinib_table[1];
RESCB				rescb_table[1];

const uint32		tnum_meminib=0;
void * const	memtop_table[1];
const MEMINIB	meminib_table[1];

const uint32			tnum_datasec=0;
const DATASECINIB	datasecinib_table[1];
const uint32		tnum_bsssec=0;
const BSSSECINIB	bsssecinib_table[1];
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void target_initialize(void) {}
void object_initialize(void) {}
void object_terminate(void)  {}
void target_exit(void)       {}
