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
#include "mpc56xx.h"
/* ============================ [ MACROS    ] ====================================================== */
#define AS_LOG_OS 0

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

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
void Os_PortResume(void);
#ifdef OS_USE_PRETASK_HOOK
void Os_PortResumePreHook(void);
#endif
void Os_PortDispatchEntry(void);
void Os_PortTickISR(void);
/* ============================ [ DATAS     ] ====================================================== */
uint32 ISR2Counter;
static unsigned int SavedCallLevel;
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void Os_PortActivate(void)
{
	/* get internal resource or NON schedule */
	RunningVar->priority = RunningVar->pConst->runPriority;

	ASLOG(OS, "%s(%d) is running\n", RunningVar->pConst->name,
			RunningVar->pConst->initPriority);

	OSPreTaskHook();

	CallLevel = TCL_TASK;
	Irq_Enable();

	RunningVar->pConst->entry();

	/* Should not return here */
	TerminateTask();
}

void Os_PortInit(void)
{
	ISR2Counter = 0;
	__asm {
		/* IVOR8 System call interrupt (SPR 408) */
		lis r0, Os_PortDispatchEntry@h
		ori r0, r0, Os_PortDispatchEntry@l
		mtivor8 r0
		/* IVOR10 Decrementer interrupt (SPR 410) */
		lis     r0, Os_PortTickISR@h
		ori     r0, r0, Os_PortTickISR@l
		mtivor10 r0
	}
}

void Os_PortInitContext(TaskVarType* pTaskVar)
{
	pTaskVar->context.sp = pTaskVar->pConst->pStack + pTaskVar->pConst->stackSize-4;
	pTaskVar->context.pc = Os_PortActivate;
}

void EnterISR(void)
{
	/* do nothing */
}

void LeaveISR(void)
{
	/* do nothing */
}

#ifdef USE_PTHREAD_SIGNAL
void Os_PortCallSignal(int sig, void (*handler)(int), void* sp, void (*pc)(void))
{
	asAssert(NULL != handler);

	handler(sig);

	/* restore its previous stack */
	RunningVar->context.sp = sp;
	RunningVar->context.pc = pc;
}

void Os_PortExitSignalCall(void)
{
	Sched_GetReady();
	Os_PortStartDispatch();
}

int Os_PortInstallSignal(TaskVarType* pTaskVar, int sig, void* handler)
{
	void* sp;
	uint32_t* stk;

	sp = pTaskVar->context.sp;

	if((sp - pTaskVar->pConst->pStack) < (pTaskVar->pConst->stackSize*3/4))
	{
		/* stack 75% usage, ignore this signal call */
		ASLOG(OS,"install signal %d failed\n", sig);
		return -1;
	}

	stk = sp;

	/* TODO */

	pTaskVar->context.sp = stk;
	pTaskVar->context.pc = Os_PortResume;

	return 0;
}
#endif

__asm void Os_PortDispatch(void)
{
nofralloc
	sc
	blr
}

#pragma section RX ".__exception_handlers"
#pragma push /* Save the current state */
__declspec (section ".__exception_handlers") extern long EXCEPTION_HANDLERS;
#pragma force_active on
#pragma function_align 16 /* We use 16 bytes alignment for Exception handlers */
__declspec(interrupt)
__declspec (section ".__exception_handlers")

__asm void Os_PortStartDispatch( void )
{
nofralloc
	/* Interrupt disable */
	wrteei 0
	/* R11 = &ReadyVar */
	lis r11, ReadyVar@h
	ori r11, r11,ReadyVar@l
l_loop:
	lwz r12, 0(r11)
	cmpwi r12, 0
	bne l_exit
	/* Interrupt enable */
	wrteei 1
	nop
	nop
	nop
	nop
	wrteei 0
	bl Sched_GetReady
	b l_loop
l_exit:
	lis r11, RunningVar@h
	stw r12, RunningVar@l(r11)
	/* Restore 'sp' from TCB */
	lwz r1, 0(r12)

	/* Restore 'pc' from TCB */
	lwz r11, 4(r12)
	mtctr r11
	se_bctrl
}

#ifdef OS_USE_PRETASK_HOOK
__asm void Os_PortResumePreHook(void)
{
nofralloc
	lis r11, CallLevel@h
	lwz r12, CallLevel@l(r11) /* save CallLevel in R12 */
	li r3, 8
	stw r3, CallLevel@l(r11)
	wrteei 1
	bl PreTaskHook
	wrteei 0
	stw r12, CallLevel@l(r11)
	b Os_PortResume
}
#endif

__asm void Os_PortResume(void)
{
nofralloc
	OS_RESTORE_CONTEXT();
	rfi
}

__asm void Os_PortDispatchEntry(void)
{
nofralloc
	OS_SAVE_CONTEXT();

	/* Save 'ssp' to TCB */
	lis r11, RunningVar@h
	lwz r12, RunningVar@l(r11)
	stw r1, 0(r12);

	lis r11, Os_PortResume@h
	ori r11, r11, Os_PortResume@l
	stw r11, 4(r12)

	b Os_PortStartDispatch
}

__asm void Os_PortTickISR(void)
{
nofralloc
	OS_SAVE_CONTEXT();

	lis r11, RunningVar@h
	lwz r10, RunningVar@l(r11)
	cmpwi r10, 0
	beq l_nosave

	lis r11, ISR2Counter@h
	lwz r12, ISR2Counter@l(r11)
	addi r12, r12, 1
	stw r12, ISR2Counter@l(r11)
	cmpwi r12, 1
	bne l_nosave

	/* Save 'ssp' to TCB */
	stw r1, 0(r10);

	lis r11, Os_PortResume@h
	ori r11, r11, Os_PortResume@l
	stw r11, 4(r12)

	/* load system stack */
	li r0, 0
	addi r1, r0, _stack_addr@h
	addi r1, r1, _stack_addr@l

l_nosave:
	lis r11, CallLevel@h
	lwz r12, CallLevel@l(r11) /* save CallLevel in R12 */
	li r3, 2
	stw r3, CallLevel@l(r11)

	wrteei 1
	bl OsTick
	li r3, 0
	bl SignalCounter
	wrteei 0

	stw r12, CallLevel@l(r11)

	lis r11, ISR2Counter@h
	lwz r12, ISR2Counter@l(r11)
	subi r12, r12, 1
	stw r12, ISR2Counter@l(r11)
	cmpwi r12, 0
	bne l_nodispatch

	lis r11, RunningVar@h
	lwz r10, RunningVar@l(r11)
	cpmwi r10, 0
	bne l_nodispatch

	lis r11, ReadyVar@h
	lwz r12, ReadyVar@l(r11)
	cmpwi r12, 0
	beq l_nopreempt

	lwz r3, 8(r12) /* priority of ReadyVar */
	lwz r4, 8(r10) /* priority of RunningVar */
	cmpw r4, r3
	bge l_nopreempt

	#ifdef OS_USE_PRETASK_HOOK
	lis r11, Os_PortResumePreHook@h
	lwz r12, Os_PortResumePreHook@l(r11)
	stw r12, 4(r10)
	#endif

	bl Sched_Preempt

	b Os_PortStartDispatch

l_nopreempt:
	lis r11, RunningVar@h
	lwz r12, RunningVar@l(r11)
	lwz r1, 0(r12)

l_nodispatch:
	OS_RESTORE_CONTEXT();
}
