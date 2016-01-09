#ifndef COM_AS_INFRASTRUCTURE_SYSTEM_KERNEL_TOPPERS_ATK_ATK2_SC4_PORTABLE_POSIX_PORTABLE_H_
#define COM_AS_INFRASTRUCTURE_SYSTEM_KERNEL_TOPPERS_ATK_ATK2_SC4_PORTABLE_POSIX_PORTABLE_H_
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

/* ============================ [ MACROS    ] ====================================================== */
#define OMIT_STKMPUINFOB
#define OMIT_OSAPMPUINFOB

#define TOPPERS_SVC_FUNCCALL
#define TARGET_SVC_NUM 0
#define TARGET_SVC_TABLE

#define SIL_PRE_LOC
#define SIL_LOC_INT()
#define SIL_UNL_INT()
/* ============================ [ TYPES     ] ====================================================== */
typedef uint32   uintptr;
typedef sint32   sintptr;
typedef char     char8;
typedef uint32	 TickType;
typedef uint8 	 StatusType;

typedef void*    TSKCTXB;
/* ============================ [ DECLARES  ] ====================================================== */
#define TP_TIMER_MAX_TICK ((TickType)0xFFFFFFFF)
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
extern void x_nested_lock_os_int(void);
extern void x_nested_unlock_os_int(void);
extern void x_lock_all_int(void);
extern void x_unlock_all_int(void);
extern void x_config_int(uint32 intno,uint32 attr,sint32 prio);
extern boolean x_is_called_in_c1isr(void);
extern void    x_set_ipm_c2isr_disable(void);
extern boolean target_is_int_controllable(uint32 intno);
extern void    x_enable_int(uint32 intno);
extern void    x_disable_int(uint32 intno);

extern void target_tp_initialize(void);
extern void target_tp_terminate(void);
extern void target_tp_start_timer(TickType tick);
extern TickType target_tp_stop_timer(void);
extern void target_clear_tp_fault_status(void);
extern void target_clear_tp_timer_status(void);
extern TickType target_tp_get_remaining_ticks(void);
extern TickType target_tp_get_elapsed_ticks(void);
extern boolean  target_tp_sense_interrupt(void);

extern void x_set_ipm(sint32 prio);
extern sint32 x_get_ipm(void);

extern void dispatch(void);
extern void start_dispatch(void);
extern void exit_and_dispatch(void);
extern void exit_and_dispatch_nohook(void);
extern void activate_force_term_osap_main(void* tcb);
extern void activate_context(void* tcb);
extern StatusType trustedfunc_stack_check(uintptr sz);
#endif /* COM_AS_INFRASTRUCTURE_SYSTEM_KERNEL_TOPPERS_ATK_ATK2_SC4_PORTABLE_POSIX_PORTABLE_H_ */
