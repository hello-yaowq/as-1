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
#define AS_LOG_OS 0

#define CMD_DISPATCH       0
#define CMD_START_DISPATCH 1
#define CMD_EXIT_SIGCALL   2
/* ============================ [ TYPES     ] ====================================================== */

/* ============================ [ DECLARES  ] ====================================================== */
extern void init_prot(void);
extern void serial_init(void);
extern void init_descriptor(mmu_descriptor_t * p_desc, uint32_t base, uint32_t limit, uint16_t attribute);
extern uint32_t seg2phys(uint16_t seg);
extern void init_clock(void);
extern void restart(void);
extern void dispatch(int cmd, void* param);
static void sys_dispatch(int cmd, void* param);

/* ============================ [ DATAS     ] ====================================================== */
uint8_t             gdt_ptr[6]; /* 0~15:Limit  16~47:Base */
mmu_descriptor_t    gdt[GDT_SIZE];
uint8_t             idt_ptr[6]; /* 0~15:Limit  16~47:Base */
mmu_gate_t          idt[IDT_SIZE];

uint32_t disp_pos;
int k_reenter;

tss_t tss;
void* sys_call_table[] = {
	sys_dispatch,
};

static boolean knl_started = FALSE;
/* ============================ [ LOCALS    ] ====================================================== */
static void sys_dispatch(int cmd, void* param)
{
	imask_t mask;

	Irq_Save(mask);

	asAssert(RunningVar);
	asAssert(ReadyVar);

	if(CMD_EXIT_SIGCALL == cmd)
	{
		/* restore signal call */
		memcpy(&RunningVar->context.regs, param, sizeof(cpu_context_t));
	}
	else
	{
		if(CMD_START_DISPATCH == cmd)
		{
			/* reinitialize the context as the context modified
			 * by "save" which is the first action of sys_call */
			Os_PortInitContext(RunningVar);
		}

		RunningVar = ReadyVar;
		#ifdef MULTIPLY_TASK_ACTIVATION
		asAssert(RunningVar->activation > 0);
		#endif
	}
	asAssert(0 == k_reenter);

	Irq_Restore(mask);
}

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
	int i;
	uint16_t selector_ldt = INDEX_LDT_FIRST << 3;
	for(i=0;i<TASK_NUM+OS_PTHREAD_NUM;i++){
		asAssert((selector_ldt>>3) < GDT_SIZE);
		init_descriptor(&gdt[selector_ldt>>3],
				vir2phys(seg2phys(SELECTOR_KERNEL_DS), TaskVarArray[i].context.ldts),
				LDT_SIZE * sizeof(mmu_descriptor_t) - 1,
				DA_LDT);
		selector_ldt += 1 << 3;
	}
}

void Os_PortInitContext(TaskVarType* pTaskVar)
{
	uint16_t selector_ldt = SELECTOR_LDT_FIRST+(pTaskVar-TaskVarArray)*(1<<3);
	uint8_t privilege;
	uint8_t rpl;
	int	eflags;
	privilege	= PRIVILEGE_TASK;
	rpl		= RPL_TASK;
	eflags = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1 */

	ASLOG(OS, "InitContext %s(%d)\n",
			pTaskVar->pConst->name?:"null",
			pTaskVar->pConst->initPriority);

	pTaskVar->context.ldt_sel	= selector_ldt;
	memcpy(&pTaskVar->context.ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(mmu_descriptor_t));
	pTaskVar->context.ldts[0].attr1 = DA_C | privilege << 5;	/* change the DPL */
	memcpy(&pTaskVar->context.ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3], sizeof(mmu_descriptor_t));
	pTaskVar->context.ldts[1].attr1 = DA_DRW | privilege << 5;/* change the DPL */
	pTaskVar->context.regs.cs		= ((8 * 0) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
	pTaskVar->context.regs.ds		= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
	pTaskVar->context.regs.es		= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
	pTaskVar->context.regs.fs		= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
	pTaskVar->context.regs.ss		= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
	pTaskVar->context.regs.gs		= (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;
	pTaskVar->context.regs.eip	= (uint32_t)Os_PortActivate;
	pTaskVar->context.regs.esp	= (uint32_t)(pTaskVar->pConst->pStack + pTaskVar->pConst->stackSize-4);
	pTaskVar->context.regs.eflags	= eflags;
}

void Os_PortSysTick(void)
{
	EnterISR();

	OsTick();
	SignalCounter(0);

	LeaveISR();
}

void Os_PortStartDispatch(void)
{
	if(FALSE == knl_started)
	{
		knl_started = TRUE;
		init_clock();
		RunningVar = ReadyVar;
		restart();
	}

	dispatch(CMD_START_DISPATCH, NULL);
	/* should never return */
	asAssert(0);
}

void Os_PortDispatch(void)
{
	dispatch(CMD_DISPATCH,NULL);
}
void cstart(void)
{
	disp_pos = 0;

	serial_init();
	ASLOG(OS,"cstart begins\n");

	/* copy the GDT of LOADER to the new GDT */
	memcpy(&gdt,    /* New GDT */
		   (void*)(*((uint32_t*)(&gdt_ptr[2]))),   /* Base  of Old GDT */
		   *((uint16_t*)(&gdt_ptr[0])) + 1    /* Limit of Old GDT */
		);
	/* gdt_ptr[6] has 6 bytes : 0~15:Limit  16~47:Base, acting as parameter of instruction sgdt & lgdt */
	uint16_t* p_gdt_limit = (uint16_t*)(&gdt_ptr[0]);
	uint32_t* p_gdt_base  = (uint32_t*)(&gdt_ptr[2]);
	*p_gdt_limit = GDT_SIZE * sizeof(mmu_descriptor_t) - 1;
	*p_gdt_base  = (uint32_t)&gdt;

	/* idt_ptr[6] 共 6 个字节：0~15:Limit  16~47:Base。用作 sidt 以及 lidt 的参数。*/
	uint16_t* p_idt_limit = (uint16_t*)(&idt_ptr[0]);
	uint32_t* p_idt_base  = (uint32_t*)(&idt_ptr[2]);
	*p_idt_limit = IDT_SIZE * sizeof(mmu_gate_t) - 1;
	*p_idt_base = (uint32_t)&idt;

	init_prot();

	/* 填充 GDT 中 TSS 这个描述符 */
	memset(&tss, 0, sizeof(tss));
	tss.ss0		= SELECTOR_KERNEL_DS;
	init_descriptor(&gdt[INDEX_TSS],
			vir2phys(seg2phys(SELECTOR_KERNEL_DS), &tss),
			sizeof(tss) - 1,
			DA_386TSS);
	tss.iobase	= sizeof(tss);	/* 没有I/O许可位图 */

	ASLOG(OS,"cstart finished\n");
}

int ffs(int v)
{
	int i;
	int r = 0;

	for(i=0;i<32;i++)
	{
		if(v&(1<<i))
		{
			r = i+1;
		}
	}

	return r;
}

#ifdef USE_PTHREAD_SIGNAL
void Os_PortCallSignal(int sig, void (*handler)(int), void* sp)
{
	asAssert(NULL != handler);

	handler(sig);

	Sched_GetReady();
	/* restore its previous stack */
	dispatch(CMD_EXIT_SIGCALL, sp);
}

void Os_PortExitSignalCall(void)
{
	asAssert(0);
}

void Os_PortInstallSignal(TaskVarType* pTaskVar, int sig, void* handler)
{
	uint32_t* stk;
	cpu_context_t *regs;

	stk = (void*)pTaskVar->context.regs.esp;

	if((sp - pTaskVar->pConst->pStack) < (pTaskVar->pConst->stackSize*3/4))
	{
		/* stack 75% usage, ignore this signal call */
		ASLOG(OS,"install signal %d failed\n", sig);
		return;
	}

	/* saving previous task context to stack */
	stk = ((void*)stk) - sizeof(cpu_context_t);
	memcpy(stk, &pTaskVar->context.regs, sizeof(cpu_context_t));

	*(stk-1) = (uint32_t)stk;
	--stk;
	*(--stk) = (uint32_t)handler;
	*(--stk) = (uint32_t)sig;
	*(--stk) = (uint32_t)Os_PortExitSignalCall;

	pTaskVar->context.regs.eip = (uint32_t)Os_PortCallSignal;
	pTaskVar->context.regs.esp = (uint32_t)stk;
}

#endif /* USE_PTHREAD_SIGNAL */

