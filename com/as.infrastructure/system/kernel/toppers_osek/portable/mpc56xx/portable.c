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
#include "osek_kernel.h"
#include "task.h"
#include "mpc56xx.h"
/* ============================ [ MACROS	] ====================================================== */
/***************************************************************************************************
		Regeister Map
***************************************************************************************************/
#define XR1     0x00
#define XBLK    0x04
#define XR2     0x08  /* _SDA2_BASE_ */
#define XR3     0x0C
#define XR4     0x10
#define XR5     0x14
#define XR6     0x18
#define XR7     0x1C
#define XR8     0x20
#define XR9     0x24
#define XR10    0x28
#define XR11    0x2C
#define XR12    0x30
#define XR13    0x34  /* _SDA_BASE_ */
#define XR14    0x38
#define XR15    0x3C
#define XR16    0x40
#define XR17    0x44
#define XR18    0x48
#define XR19    0x4C
#define XR20    0x50
#define XR21    0x54
#define XR22    0x58
#define XR23    0x5C
#define XR24    0x60
#define XR25    0x64
#define XR26    0x68
#define XR27    0x6C
#define XR28    0x70
#define XR29    0x74
#define XR30    0x78
#define XR31    0x7C
#define XR0     0x80
#define XSRR0   0x84
#define XSRR1   0x88
#define XUSPRG  0x8C
#define XCTR    0x90
#define XXER    0x94
#define XCR     0x98
#define XLR     0x9C
#define XSPEFSCR 0xA0
#define XPAD2   0xA4
#define XPAD3   0xA8
#define XMSR    0xAC

#define STACK_FRAME_SIZE   0xB0

#define OS_RESTORE_CONTEXT()	\
	wrteei 0; \
	/*restore R0,R2-R31*/	\
	lmw		r2,  XR2(r1);	\
	/*restore CR*/	\
    lwz     r0,  XCR(r1);	\
    mtcrf   0xff,r0;	\
	/*restore XER*/	\
	lwz     r0,  XXER(r1);	\
	mtxer   r0;	\
	/*restore CTR*/	\
	lwz     r0,  XCTR(r1);	\
	mtctr   r0;	\
	/*restore LR */	\
	lwz     r0,  XLR(r1);	\
	mtlr    r0;	\
	/*restore SRR1*/	\
	lwz     r0,  XSRR1(r1);	\
	mtspr   SRR1,r0;	\
	/*restore SRR0*/	\
	lwz     r0,  XSRR0(r1);	\
	mtspr   SRR0,r0;	\
	/*restore USPRG*/	\
	lwz     r0,  XUSPRG(r1);	\
	mtspr   USPRG0,r0;	\
	/*restore SPEFSCR :for float point, if not used, canbe not saved */ \
	lwz     r0,  XSPEFSCR(r1);	\
	mtspr   SPEFSCR,r0;	\
	/*restore rsp*/ \
	lwz     r0,  XR0(r1);	\
	addi    r1,  r1, STACK_FRAME_SIZE

#define OS_SAVE_CONTEXT()	\
	/* Remain frame from stack */	\
	subi    r1, r1, -STACK_FRAME_SIZE; \
	/*Store R0,R2,R3-R31*/	\
	stw     r0,  XR0(r1);		\
	stmw	r2,  XR2(r1);	  \
	/*Store XMSR ang SPEFSCR  */ \
	mfmsr   r0; \
	stw     r0,  XMSR(r1); \
	mfspr   r0,  SPEFSCR; \
	stw     r0,  XSPEFSCR(r1); \
	/*Store LR(SRR0)*/	\
	mfspr   r0,  SRR0; \
	stw     r0,  XSRR0(r1); \
	/*Store MSR(SRR1)*/	\
	mfspr   r0,  SRR1; \
	stw     r0,  XSRR1(r1); \
	/*Store USPRG0*/	\
	mfspr   r0,  USPRG0; \
	stw     r0,  XUSPRG(r1); \
	/*Store LR*/  \
	mflr    r0; \
	stw     r0,  XLR(r1); \
	/*Store CTR*/	\
	mfctr   r0; \
	stw     r0,  XCTR(r1); \
	/*Store XER*/	\
	mfxer   r0; \
	stw     r0,  XXER(r1); \
	/*Store CR*/	\
	mfcr    r0; \
	stw     r0,  XCR(r1); \
	mfmsr   r0
/* ============================ [ TYPES	 ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
extern void knl_system_tick(void);
extern void knl_dispatch_entry(void);
extern void knl_start_dispatch(void);
extern void OsTick(void);
/* ============================ [ DATAS	 ] ====================================================== */
uint32 knl_taskindp;
VP tcxb_sp[TASK_NUM];
FP tcxb_pc[TASK_NUM];
#if (ISR_NUM > 0)
extern const FP tisr_pc[ISR_NUM];
#endif
/* ============================ [ LOCALS	] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void set_ipl(IPL ipl)
{
	if(ipl > 0)
	{
		disable_int();
	}
}
IPL  current_ipl(void)
{
	return 0;
}
void activate_r(void)
{
	tcb_curpri[runtsk] = tinib_exepri[runtsk];
	callevel = TCL_PREPOST;
	PreTaskHook();
	callevel = TCL_TASK;
	enable_int();
	tinib_task[runtsk]();
}
void activate_context(TaskType TaskID)
{
	tcxb_pc[TaskID] = (FP)activate_r;

	tcxb_sp[TaskID] = (VP)( (UINT32)tinib_stk[TaskID] + (UINT32)tinib_stksz[TaskID]);
}
void cpu_terminate(void)
{

}
void sys_exit(void)
{

}

void cpu_initialize(void)
{
	knl_taskindp = 0;

	__asm {
		/* IVOR8 System call interrupt (SPR 408) */
		lis r0, knl_dispatch_entry@h
		ori r0, r0, knl_dispatch_entry@l
		mtivor8 r0
		/* IVOR10 Decrementer interrupt (SPR 410) */
		lis     r0, knl_system_tick@h
		ori     r0, r0, knl_system_tick@l
		mtivor10 r0
	}
}

void knl_isr_handler(uint32 intno)
{
#if (ISR_NUM > 0)
	if((intno<ISR_NUM) && (tisr_pc[intno]!=NULL))
	{
		tisr_pc[intno]();
	}
	else
#endif
	{
		ShutdownOS(0xFF);
	}
}

void sys_initialize(void)
{

}
void tool_initialize(void)
{

}

void start_dispatch(void)
{
	runtsk = INVALID_TASK;
	knl_start_dispatch();
}

void exit_and_dispatch(void)
{
	disable_int();
	callevel = TCL_PREPOST;
	PostTaskHook();
	callevel = TCL_TASK;
	runtsk = INVALID_TASK;
	knl_start_dispatch();
}

#pragma section RX ".__exception_handlers"
#pragma push /* Save the current state */
__declspec (section ".__exception_handlers") extern long EXCEPTION_HANDLERS;
#pragma force_active on
#pragma function_align 16 /* We use 16 bytes alignment for Exception handlers */
__declspec(interrupt)
__declspec (section ".__exception_handlers")
static __asm void knl_dispatch_r(void)
{
nofralloc
	OS_RESTORE_CONTEXT();
	rfi
}

__asm void knl_start_dispatch(void)
{
nofralloc
	wrteei 0
	lis r11, schedtsk@h
	ori r11, r11, schedtsk@l
l_loop:
	lbz r0, 0(r11)
	cmpwi r0, 0xFF /* 0xFF means invalid task */
	bne l_exit
	wrteei 1
	nop
	nop
	nop
	nop
	wrteei 0
	b l_loop
l_exit:
	lis r11, runtsk@h
	stb r0, runtsk@l(r11) /* runtsk = schedtsk */

	se_slwi r0, 2
	lis r8, tcxb_sp@h
	ori r8, r8, tcxb_sp@l
	lwzx r1, r8, r0

	lis r8, tcxb_pc@h
	ori r8, r8, tcxb_pc@l
	lwzx r12, r8, r0
	mtctr r12
	se_bctrl
}

__asm void knl_dispatch_entry(void)
{
nofralloc
	OS_SAVE_CONTEXT();

	lis r11, runtsk@h
	lbz r0, runtsk@l(r11)
	se_slwi r0, 2
	lis r8, tcxb_sp@h
	ori r8, r8, tcxb_sp@l
	stwx r1, r8, r0

	lis r11, knl_dispatch_r@h
	ori r11, r11, knl_dispatch_r@l
	lis r8, tcxb_pc@h
	ori r8, r8, tcxb_pc@l
	stwx r11, r8, r0

	b knl_start_dispatch
}

__asm void knl_system_tick(void)
{
nofralloc
	OS_SAVE_CONTEXT();

	lis r11, runtsk@h
	lbz r0, runtsk@l(r11)
	cmpwi r0, 0xFF
	beq l_nosave

	lis r11, knl_taskindp@h
	lwz r12, knl_taskindp@l(r11)
	addi r12, r12, 1
	stw r12, knl_taskindp@l(r11)
	cmpwi r12, 1
	bne l_nosave

	se_slwi r0, 2
	lis r8, tcxb_sp@h
	ori r8, r8, tcxb_sp@l
	stwx r1, r8, r0

	lis r11, knl_dispatch_r@h
	ori r11, r11, knl_dispatch_r@l
	lis r8, tcxb_pc@h
	ori r8, r8, tcxb_pc@l
	stwx r11, r8, r0

	/* load system stack */

l_nosave:
	lis r11, callevel@h
	lbz r12, callevel@l(r11) /* save CallLevel in R12 */
	li r3, 2
	stb r3, callevel@l(r11)

	wrteei 1
	bl OsTick
	li r3, 0
	bl SignalCounter
	wrteei 0

	stw r12, callevel@l(r11)

	lis r11, knl_taskindp@h
	lwz r12, knl_taskindp@l(r11)
	subi r12, r12, 1
	stw r12, knl_taskindp@l(r11)
	cmpwi r12, 0
	bne l_nodispatch

	lis r11, runtsk@h
	lbz r0, runtsk@l(r11)
	cmpwi r0, 0xFF
	bne l_nodispatch

	lis r11, schedtsk@h
	lbz r12, schedtsk@l(r11)
	cmpwi r12, 0xFF
	beq l_nopreempt

	cmpw r0, r12
	beq l_nopreempt

	b knl_start_dispatch

l_nopreempt:
	se_slwi r0, 2

	lis r8, tcxb_sp@h
	ori r8, r8, tcxb_sp@l
	lwzx r1, r8, r0

l_nodispatch:
	OS_RESTORE_CONTEXT();
}

__asm void knl_isr_process(void)
{

}
