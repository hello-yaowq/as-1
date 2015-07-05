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
#include "Os.h"
#include "osek_kernel.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
typedef void (*void_function_void_t)(void);
/* ============================ [ DECLARES  ] ====================================================== */
extern void USART1_IrqHandler ( void );
extern void TC0_IrqHandler ( void );
extern void USBD_IrqHandler ( void );
extern void task_initialize(void);
extern void alarm_initialize(void);
extern void resource_initialize(void);
/* ============================ [ DATAS     ] ====================================================== */
const UINT8 tnum_task    = TASK_NUM;
const UINT8 tnum_exttask = TASK_NUM;

const Priority  tinib_inipri[TASK_NUM] = {
     5 , /* TaskApp */
     7 , /* TaskCom */
     8 , /* TaskSg */
     1 , /* TaskIdle */
};
const Priority  tinib_exepri[TASK_NUM] = {
     5 , /* TaskApp */
     7 , /* TaskCom */
     8 , /* TaskSg */
     1 , /* TaskIdle */
};
const UINT8  tinib_maxact[TASK_NUM] = {
    1, /* TaskApp */
    1, /* TaskCom */
    1, /* TaskSg */
    1, /* TaskIdle */
};
const AppModeType  tinib_autoact[TASK_NUM] = {
    OSDEFAULTAPPMODE,
    OSDEFAULTAPPMODE,
    OSDEFAULTAPPMODE,
    OSDEFAULTAPPMODE,
};
const FP  tinib_task[TASK_NUM] = {
    TASKNAME( TaskApp ),
    TASKNAME( TaskCom ),
    TASKNAME( TaskSg ),
    TASKNAME( TaskIdle ),
};
static UINT8  TaskApp_stk[ 512 ];
static UINT8  TaskCom_stk[ 512 ];
static UINT8  TaskSg_stk[ 512 ];
static UINT8  TaskIdle_stk[ 512 ];
const VP  tinib_stk[TASK_NUM] = {
    TaskApp_stk,
    TaskCom_stk,
    TaskSg_stk,
    TaskIdle_stk,
};
const UINT16  tinib_stksz[TASK_NUM] = {
     512 ,
     512 ,
     512 ,
     512 ,
};
const IPL       ipl_maxisr2 = 1;
/*
 *  os_cfg.c
 *
 *  used to manage tasks < tasks queue, state, proority, activate count >
 */
TaskType        tcb_next[TASK_NUM];         /* task linker,indicate the next task in the queue */
UINT8           tcb_tstat[TASK_NUM];        /* task state */
Priority        tcb_curpri[TASK_NUM];       /* task current priority */
UINT8           tcb_actcnt[TASK_NUM];       /* task current activate count */
EventMaskType   tcb_curevt[TASK_NUM];       /* task current event has been set */
EventMaskType   tcb_waievt[TASK_NUM];       /* task wait event */
ResourceType    tcb_lastres[TASK_NUM];      /* task acquired resource recently */
/*
 *  number of ALARM and Counter
 */
const UINT8     tnum_counter = 1;                /* Counter number configured */
const UINT8     tnum_alarm = ALARM_NUM;          /* Alarm number configured */
/*
 *  configured information for counter
 */
const TickType  cntinib_maxval[1] =
{
    65535
};
const TickType  cntinib_maxval2[1] =
{
    65535*2+1
};
const TickType  cntinib_tickbase[1] =
{
    1
};
const TickType  cntinib_mincyc[1] =
{
     1
};
/*
 *  counter control block
 */
TickType            cntcb_curval[1];      /* counter current value*/
AlarmType           cntcb_almque[1];      /* counter queue */
/*
 *  configured information for alarm
 */
const CounterType alminib_cntid[ALARM_NUM] = {
    0,
    0,
    0,
    0,
    0,
};
const FP             alminib_cback[ALARM_NUM] = {
    ALARMCALLBACKNAME(Alarm5ms),
    ALARMCALLBACKNAME(Alarm10ms),
    ALARMCALLBACKNAME(Alarm20ms),
    ALARMCALLBACKNAME(Alarm50ms),
    ALARMCALLBACKNAME(Alarm100ms),
};
const AppModeType alminib_autosta[ALARM_NUM]= {
    OSDEFAULTAPPMODE,
    OSDEFAULTAPPMODE,
    OSDEFAULTAPPMODE,
    OSDEFAULTAPPMODE,
    OSDEFAULTAPPMODE,
};
const TickType   alminib_almval[ALARM_NUM] = {
     5 , /* Alarm5ms */
     6 , /* Alarm10ms */
     7 , /* Alarm20ms */
     8 , /* Alarm50ms */
     9 , /* Alarm100ms */
};
const TickType   alminib_cycle[ALARM_NUM] = {
     5 , /* Alarm5ms */
     10 , /* Alarm10ms */
     20 , /* Alarm20ms */
     50 , /* Alarm50ms */
     100 , /* Alarm100ms */
};
/*
 *  alarm control block
 */
AlarmType       almcb_next[ALARM_NUM];      /* next alarm in queue */
AlarmType       almcb_prev[ALARM_NUM];      /* previous alarm in queue */
TickType        almcb_almval[ALARM_NUM];    /* expire time */
TickType        almcb_cycle[ALARM_NUM];     /* alarm cycle time */

const UINT8     tnum_resource = 1;
const Priority  resinib_ceilpri[1] =
{
    15,
};
/*
 *  resource control block
 *
 */
Priority            rescb_prevpri[1];    /* previous priority of task which has acquired this res */
ResourceType        rescb_prevres[1];

const UINT8     tnum_isr2 = 0;
const Priority  isrinib_intpri[] =
{
    0
};
ResourceType        isrcb_lastres[1];

const FP tisr_pc[ 35 ] =
{
    NULL,/* 0 */
    NULL,/* 1 */
    NULL,/* 2 */
    NULL,/* 3 */
    NULL,/* 4 */
    NULL,/* 5 */
    NULL,/* 6 */
    NULL,/* 7 */
    NULL,/* 8 */
    NULL,/* 9 */
    NULL,/* 10 */
    NULL,/* 11 */
    NULL,/* 12 */
    NULL,/* 13 */
    NULL,/* 14 */
    USART1_IrqHandler,/* 15 */
    NULL,/* 16 */
    NULL,/* 17 */
    NULL,/* 18 */
    NULL,/* 19 */
    NULL,/* 20 */
    NULL,/* 21 */
    NULL,/* 22 */
    TC0_IrqHandler,/* 23 */
    NULL,/* 24 */
    NULL,/* 25 */
    NULL,/* 26 */
    NULL,/* 27 */
    NULL,/* 28 */
    NULL,/* 29 */
    NULL,/* 30 */
    NULL,/* 31 */
    NULL,/* 32 */
    NULL,/* 33 */
    USBD_IrqHandler,/* 34 */
};
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void object_initialize(void)
{
    task_initialize();
    alarm_initialize();
    resource_initialize();
}

