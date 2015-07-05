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
#ifndef OS_CFG_H
#define OS_CFG_H
/* ============================ [ INCLUDES  ] ====================================================== */
#include "Std_Types.h"
#include "kernel.h"
/* ============================ [ MACROS    ] ====================================================== */
#define AS_OS_BASED_ON_TOPPERS_OSEK

#define TASK_ID_TaskApp                            0 
#define TASK_ID_TaskCom                            1 
#define TASK_ID_TaskSg                             2 
#define TASK_ID_TaskIdle                           3 
#define TASK_NUM  4 

#define TASK_PRIORITY_TaskApp                            5 
#define TASK_PRIORITY_TaskCom                            7 
#define TASK_PRIORITY_TaskSg                             8 
#define TASK_PRIORITY_TaskIdle                           1 

#define EVENT_MASK_TaskApp_Event1                            ( 1 << 0 )
#define EVENT_MASK_TaskApp_Event2                            ( 1 << 1 )


#define ALARM_ID_Alarm5ms                           0 
#define ALARM_ID_Alarm10ms                          1 
#define ALARM_ID_Alarm20ms                          2 
#define ALARM_ID_Alarm50ms                          3 
#define ALARM_ID_Alarm100ms                         4 
#define ALARM_NUM  5 

#define ALARM(AlarmName)  ALARMCALLBACK(AlarmName)

/* OSEK OS Debugh helper */
#define LOG_STAOS_ENTER(mode)
#define LOG_STAOS_LEAVE()
#define LOG_TERTSK_ENTER()
#define LOG_TERTSK_LEAVE(ercd)
#define LOG_STUTOS_ENTER(ercd)
#define LOG_STUTOS_LEAVE()
#define LOG_ACTTSK_ENTER(tskid)
#define LOG_ACTTSK_LEAVE(ercd)
#define LOG_SIGCNT_ENTER(cntid)
#define LOG_SIGCNT_LEAVE(ercd)
#define LOG_SETREL_ENTER(almid, incr, cycle)
#define LOG_SETREL_LEAVE(ercd)
#define LOG_SETABS_ENTER(almid, incr, cycle)
#define LOG_SETABS_LEAVE(ercd)
#define LOG_SETEVT_ENTER(tskid, mask)
#define LOG_SETEVT_LEAVE(ercd)
#define LOG_CLREVT_ENTER(mask)
#define LOG_CLREVT_LEAVE(ercd)
#define LOG_GETEVT_ENTER(tskid, p_mask)
#define LOG_GETEVT_LEAVE(ercd, mask)
#define LOG_WAIEVT_ENTER(mask)
#define LOG_WAIEVT_LEAVE(ercd)
#define LOG_GETRES_ENTER(resid)
#define LOG_GETRES_LEAVE(ercd)
#define LOG_RELRES_ENTER(resid)
#define LOG_RELRES_LEAVE(ercd)
#define LOG_GETALB_ENTER(almid, p_info)
#define LOG_GETALB_LEAVE(ercd)
#define LOG_GETALM_ENTER(almid, p_tick)
#define LOG_GETALM_LEAVE(ercd)
#define LOG_CANALM_ENTER(almid)
#define LOG_CANALM_LEAVE(ercd)
#define LOG_DISINT_ENTER()
#define LOG_DISINT_LEAVE()
#define LOG_ENAINT_ENTER()
#define LOG_ENAINT_LEAVE()
#define LOG_SUSALL_ENTER()
#define LOG_SUSALL_LEAVE()
#define LOG_RSMALL_ENTER()
#define LOG_RSMALL_LEAVE()
#define LOG_SUSOSI_ENTER()
#define LOG_SUSOSI_LEAVE()
#define LOG_RSMOSI_ENTER()
#define LOG_RSMOSI_LEAVE()
#define LOG_GETAAM_ENTER()
#define LOG_GETAAM_LEAVE(mode)
#define LOG_CHNTSK_ENTER(tskid)
#define LOG_CHNTSK_LEAVE(ercd)
#define LOG_SCHED_ENTER()
#define LOG_SCHED_LEAVE(ercd)
#define LOG_GETTID_ENTER(p_tskid)
#define LOG_GETTID_LEAVE(ercd, p_tskid)
#define LOG_GETTST_ENTER(tskid, p_state)
#define LOG_GETTST_LEAVE(ercd,  p_state)
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
extern TASK(TaskApp);
extern TASK(TaskCom);
extern TASK(TaskSg);
extern TASK(TaskIdle);

extern ALARM(Alarm5ms);
extern ALARM(Alarm10ms);
extern ALARM(Alarm20ms);
extern ALARM(Alarm50ms);
extern ALARM(Alarm100ms);
#endif /* OS_CFG_H */

