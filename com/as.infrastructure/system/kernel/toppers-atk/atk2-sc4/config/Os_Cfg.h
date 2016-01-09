#ifndef OS_CFG_H_
#define OS_CFG_H_
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
#include "atk_os.h"
#include "allfunc.h"
/* ============================ [ MACROS    ] ====================================================== */
#define __TOPPERS_ATK2_SC4__

#define RES_SCHEDULER 0

#define OSDEFAULTAPPMODE 1

#define ALARM_ID_AlarmApp 0
#define ALARM_ID_Alarm_BswService 1

#define TASK_ID_TaskApp    0
#define TASK_ID_TaskCanIf  1
#define TASK_ID_SchM_BswService 2
#define TASK_NUM  3

//#define CFG_USE_ERRORHOOK

#define ALARM(a)  void AlarmMain##a(void)
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
extern void target_initialize(void);
extern void object_initialize(void);

#endif /* OS_CFG_H_ */
