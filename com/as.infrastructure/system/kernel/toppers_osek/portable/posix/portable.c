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
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <sys/times.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include "osek_kernel.h"
#include "task.h"
#include <assert.h>
#include <setjmp.h>

/* ============================ [ MACROS    ] ====================================================== */
#define configASSERT(x)	assert(x)

#define MAX_NUMBER_OF_TASKS 		( _POSIX_THREAD_THREADS_MAX )

#define pdFALSE			( ( BaseType_t ) 0 )
#define pdTRUE			( ( BaseType_t ) 1 )

#define pdPASS			( pdTRUE )
#define pdFAIL			( pdFALSE )

#define portTICK_PERIOD_MS 1

#define portMAX_INTERRUPTS				( ( uint32_t ) sizeof( uint32_t ) * 8UL ) /* The number of bits in an uint32_t. */
#define portNO_CRITICAL_NESTING 		( ( uint32_t ) 0 )

#define portSTACK_TYPE  unsigned long
#define portBASE_TYPE   long
#define portLONG		int

#define portTickType TickType

/* Posix Signal definitions that can be changed or read as appropriate. */
#define SIG_SUSPEND					SIGUSR1
#define SIG_RESUME					SIGUSR2

/* Enable the following hash defines to make use of the real-time tick where time progresses at real-time. */
#define SIG_TICK					SIGALRM
#define TIMER_TYPE					ITIMER_REAL

#define configTICK_RATE_HZ   1000
#define portTICK_RATE_MS				( ( portTickType ) 1000 / configTICK_RATE_HZ )
#define portTICK_RATE_MICROSECONDS		( ( portTickType ) 1000000 / configTICK_RATE_HZ )

/* ============================ [ TYPES     ] ====================================================== */
/* The WIN32 simulator runs each task in a thread.  The context switching is
managed by the threads, so the task stack does not have to be managed directly,
although the task stack is still used to hold an xThreadState structure this is
the only thing it will ever hold.  The structure indirectly maps the task handle
to a thread handle. */
typedef struct
{
	/* Handle of the thread that executes the task. */
	pthread_t hThread;
	/* Handle of the thread that task has been activated */
	pthread_cond_t hEvent;
	pthread_mutex_t hMutex;
	jmp_buf jmp;
} xThreadState;

typedef long BaseType_t;

typedef TickType TickType_t;
typedef uint8    StackType_t;
typedef void (*TaskFunction_t)( void * );
/* ============================ [ DECLARES  ] ====================================================== */
void vPortEnterCritical( void );
void vPortExitCritical( void );
void vPortEnableInterrupts( void );
void vPortDisableInterrupts( void );
/* ============================ [ DATAS     ] ====================================================== */
static xThreadState pxThreads[TASK_NUM];
static pthread_once_t hSigSetupThread = PTHREAD_ONCE_INIT;
static pthread_attr_t xThreadAttributes;
static pthread_mutex_t xSuspendResumeThreadMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t xSingleThreadMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t hMainThread = ( pthread_t )NULL;
/*-----------------------------------------------------------*/

static volatile portBASE_TYPE xSentinel = 0;
static volatile portBASE_TYPE xSchedulerEnd = pdFALSE;
static volatile portBASE_TYPE xInterruptsEnabled = pdTRUE;
static volatile portBASE_TYPE xServicingTick = pdFALSE;
static volatile portBASE_TYPE xPendYield = pdFALSE;
static volatile unsigned portBASE_TYPE uxCriticalNesting;

/* ============================ [ LOCALS    ] ====================================================== */


/*-----------------------------------------------------------*/

/*
 * Setup the timer to generate the tick interrupts.
 */
static void prvSetupTimerInterrupt( void );
static void prvSuspendSignalHandler(int sig);
static void prvResumeSignalHandler(int sig);
static void prvSetupSignalsAndSchedulerPolicy( void );
static void prvSuspendThread( pthread_t xThreadId );
static void prvResumeThread( pthread_t xThreadId );
/*-----------------------------------------------------------*/

/*
 * Exception handlers.
 */
void vPortYield( void );
void vPortSystemTickHandler( int sig );

/*
 * Start first task is a separate function so it can be tested in isolation.
 */
void vPortStartFirstTask( void );
/*-----------------------------------------------------------*/
void vPortStartFirstTask( void )
{
	/* Initialise the critical nesting count ready for the first task. */
	uxCriticalNesting = 0;

	/* Start the first task. */
	vPortEnableInterrupts();

	/* Start the first task. */
	prvResumeThread( pxThreads[runtsk].hThread  );
}
/*-----------------------------------------------------------*/

void vPortYieldFromISR( void )
{
	/* Calling Yield from a Interrupt/Signal handler often doesn't work because the
	 * xSingleThreadMutex is already owned by an original call to Yield. Therefore,
	 * simply indicate that a yield is required soon.
	 */
	xPendYield = pdTRUE;
}
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
	vPortDisableInterrupts();
	uxCriticalNesting++;
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
	/* Check for unmatched exits. */
	if ( uxCriticalNesting > 0 )
	{
		uxCriticalNesting--;
	}

	/* If we have reached 0 then re-enable the interrupts. */
	if( uxCriticalNesting == 0 )
	{
		/* Have we missed ticks? This is the equivalent of pending an interrupt. */
		if ( pdTRUE == xPendYield )
		{
			xPendYield = pdFALSE;
			vPortYield();
		}
		vPortEnableInterrupts();
	}
}
/*-----------------------------------------------------------*/

void vPortYield( void )
{
pthread_t xTaskToSuspend;
pthread_t xTaskToResume;

	if ( 0 == pthread_mutex_lock( &xSingleThreadMutex ) )
	{
		xTaskToSuspend = pxThreads[runtsk].hThread;


		xTaskToResume = pxThreads[schedtsk].hThread;
		if ( xTaskToSuspend != xTaskToResume )
		{
			/* Switch tasks. */
			prvResumeThread( xTaskToResume );
			prvSuspendThread( xTaskToSuspend );
		}
		else
		{
			/* Yielding to self */
			(void)pthread_mutex_unlock( &xSingleThreadMutex );
		}
	}
}
/*-----------------------------------------------------------*/

void vPortDisableInterrupts( void )
{
	xInterruptsEnabled = pdFALSE;
}
/*-----------------------------------------------------------*/

void vPortEnableInterrupts( void )
{
	xInterruptsEnabled = pdTRUE;
}
/*-----------------------------------------------------------*/

portBASE_TYPE xPortSetInterruptMask( void )
{
portBASE_TYPE xReturn = xInterruptsEnabled;
	xInterruptsEnabled = pdFALSE;
	return xReturn;
}
/*-----------------------------------------------------------*/

void vPortClearInterruptMask( portBASE_TYPE xMask )
{
	xInterruptsEnabled = xMask;
}
/*-----------------------------------------------------------*/

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
void prvSetupTimerInterrupt( void )
{
struct itimerval itimer, oitimer;
portTickType xMicroSeconds = portTICK_RATE_MICROSECONDS;

	/* Initialise the structure with the current timer information. */
	if ( 0 == getitimer( TIMER_TYPE, &itimer ) )
	{
		/* Set the interval between timer events. */
		itimer.it_interval.tv_sec = 0;
		itimer.it_interval.tv_usec = xMicroSeconds;

		/* Set the current count-down. */
		itimer.it_value.tv_sec = 0;
		itimer.it_value.tv_usec = xMicroSeconds;

		/* Set-up the timer interrupt. */
		if ( 0 != setitimer( TIMER_TYPE, &itimer, &oitimer ) )
		{
			PRINTF( "Set Timer problem.\n" );
		}
	}
	else
	{
		PRINTF( "Get Timer problem.\n" );
	}
}
static uint32_t prvProcessTickInterrupt( void )
{
	uint32_t ulSwitchRequired;


	callevel = TCL_ISR2;
	SignalCounter(0);
	callevel = TCL_TASK;

	if(schedtsk != runtsk)
	{
		ulSwitchRequired = TRUE;
	}
	else
	{
		ulSwitchRequired = FALSE;
	}

	return ulSwitchRequired;
}
/*-----------------------------------------------------------*/
void vPortSystemTickHandler( int sig )
{
pthread_t xTaskToSuspend;
pthread_t xTaskToResume;
uint32_t ulSwitchRequired;

	if ( ( pdTRUE == xInterruptsEnabled ) && ( pdTRUE != xServicingTick ) )
	{
		if ( 0 == pthread_mutex_trylock( &xSingleThreadMutex ) )
		{
			xServicingTick = pdTRUE;

			xTaskToSuspend = pxThreads[runtsk].hThread;

			/* Tick Increment. */
			ulSwitchRequired = ( uint32_t ) prvProcessTickInterrupt();

			xTaskToResume = pxThreads[schedtsk].hThread;

			if( ulSwitchRequired != pdFALSE )
			{
				/* If the task selected to enter the running state is not the task
				that is already in the running state. */
				if( runtsk != schedtsk )
				{
					if(runtsk!=INVALID_TASK)
					{
						/* Suspend the old thread. */
						prvSuspendThread( xTaskToSuspend );
					}

					runtsk = schedtsk;

					if(runtsk!=INVALID_TASK)
					{
						callevel = TCL_TASK;
						/* Obtain the state of the task now selected to enter the
						Running state. */
						prvResumeThread( xTaskToResume );
					}
					else
					{
						callevel = TCL_NULL;
					}
				}
			}
		}
		else
		{
			xPendYield = pdTRUE;
		}
	}
	else
	{
		xPendYield = pdTRUE;
	}
}
/*-----------------------------------------------------------*/

void prvSuspendSignalHandler(int sig)
{
sigset_t xSignals;

	/* Only interested in the resume signal. */
	sigemptyset( &xSignals );
	sigaddset( &xSignals, SIG_RESUME );
	xSentinel = 1;

	/* Unlock the Single thread mutex to allow the resumed task to continue. */
	if ( 0 != pthread_mutex_unlock( &xSingleThreadMutex ) )
	{
		PRINTF( "Releasing someone else's lock.\n" );
	}

	/* Wait on the resume signal. */
	if ( 0 != sigwait( &xSignals, &sig ) )
	{
		PRINTF( "SSH: Sw %d\n", sig );
	}

	/* Will resume here when the SIG_RESUME signal is received. */
	/* Need to set the interrupts based on the task's critical nesting. */
	if ( uxCriticalNesting == 0 )
	{
		vPortEnableInterrupts();
	}
	else
	{
		vPortDisableInterrupts();
	}
}
/*-----------------------------------------------------------*/

void prvSuspendThread( pthread_t xThreadId )
{
portBASE_TYPE xResult = pthread_mutex_lock( &xSuspendResumeThreadMutex );
	if ( 0 == xResult )
	{
		/* Set-up for the Suspend Signal handler? */
		xSentinel = 0;
		xResult = pthread_mutex_unlock( &xSuspendResumeThreadMutex );
		xResult = pthread_kill( xThreadId, SIG_SUSPEND );
		while ( ( xSentinel == 0 ) && ( pdTRUE != xServicingTick ) )
		{
			sched_yield();
		}
	}
}
/*-----------------------------------------------------------*/

void prvResumeSignalHandler(int sig)
{
	/* Yield the Scheduler to ensure that the yielding thread completes. */
	if ( 0 == pthread_mutex_lock( &xSingleThreadMutex ) )
	{
		(void)pthread_mutex_unlock( &xSingleThreadMutex );
	}
}
/*-----------------------------------------------------------*/

void prvResumeThread( pthread_t xThreadId )
{
portBASE_TYPE xResult;
	if ( 0 == pthread_mutex_lock( &xSuspendResumeThreadMutex ) )
	{
		if ( pthread_self() != xThreadId )
		{
			xResult = pthread_kill( xThreadId, SIG_RESUME );
		}
		xResult = pthread_mutex_unlock( &xSuspendResumeThreadMutex );
	}
	(void)xResult;
}
/*-----------------------------------------------------------*/

void prvSetupSignalsAndSchedulerPolicy( void )
{
/* The following code would allow for configuring the scheduling of this task as a Real-time task.
 * The process would then need to be run with higher privileges for it to take affect.
int iPolicy;
int iResult;
int iSchedulerPriority;
	iResult = pthread_getschedparam( pthread_self(), &iPolicy, &iSchedulerPriority );
	iResult = pthread_attr_setschedpolicy( &xThreadAttributes, SCHED_FIFO );
	iPolicy = SCHED_FIFO;
	iResult = pthread_setschedparam( pthread_self(), iPolicy, &iSchedulerPriority );		*/

struct sigaction sigsuspendself, sigresume, sigtick;


	sigsuspendself.sa_flags = 0;
	sigsuspendself.sa_handler = prvSuspendSignalHandler;
	sigfillset( &sigsuspendself.sa_mask );

	sigresume.sa_flags = 0;
	sigresume.sa_handler = prvResumeSignalHandler;
	sigfillset( &sigresume.sa_mask );

	sigtick.sa_flags = 0;
	sigtick.sa_handler = vPortSystemTickHandler;
	sigfillset( &sigtick.sa_mask );

	if ( 0 != sigaction( SIG_SUSPEND, &sigsuspendself, NULL ) )
	{
		PRINTF( "Problem installing SIG_SUSPEND_SELF\n" );
	}
	if ( 0 != sigaction( SIG_RESUME, &sigresume, NULL ) )
	{
		PRINTF( "Problem installing SIG_RESUME\n" );
	}
	if ( 0 != sigaction( SIG_TICK, &sigtick, NULL ) )
	{
		PRINTF( "Problem installing SIG_TICK\n" );
	}
	PRINTF( "Running as PID: %d\n", getpid() );
}

/*-----------------------------------------------------------*/

void vPortFindTicksPerSecond( void )
{
	/* Needs to be reasonably high for accuracy. */
	unsigned long ulTicksPerSecond = sysconf(_SC_CLK_TCK);
	PRINTF( "Timer Resolution for Run TimeStats is %ld ticks per second.\n", ulTicksPerSecond );
}
/*-----------------------------------------------------------*/

unsigned long ulPortGetTimerValue( void )
{
struct tms xTimes;
	unsigned long ulTotalTime = times( &xTimes );
	/* Return the application code times.
	 * The timer only increases when the application code is actually running
	 * which means that the total execution times should add up to 100%.
	 */
	return ( unsigned long ) xTimes.tms_utime;

	/* Should check ulTotalTime for being clock_t max minus 1. */
	(void)ulTotalTime;
}
/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
#include "Std_Types.h"
imask_t portGetIrqStateAndDisableIt(void)
{
	vPortEnterCritical();
	return uxCriticalNesting;
}
void portRestroeIrqState(imask_t irq_state)
{
	vPortExitCritical();
}
static void* prvToppersOSEK_TaskProcess(void * param)
{
	TaskType taskId= (TaskType)(unsigned long)param;
	for(;;)
	{
		int rv;
		(void)pthread_cond_wait(&(pxThreads[taskId].hEvent),&(pxThreads[taskId].hMutex));
		(void)pthread_mutex_unlock( &(pxThreads[taskId].hMutex) );

		rv = setjmp(pxThreads[taskId].jmp);
		if(0 == rv)
		{
			lock_cpu();
			callevel = TCL_PREPOST;
			PreTaskHook();
			unlock_cpu();
			callevel = TCL_TASK;
			tcb_curpri[runtsk] = tinib_exepri[runtsk];
			tinib_task[taskId]();
		}
		else
		{
			/* terminate */
		}

	}
	return NULL;
}


/* ============================ [ FUNCTIONS ] ====================================================== */

void disable_int(void)
{
	assert(uxCriticalNesting==0);
	vPortEnterCritical();
	assert(uxCriticalNesting==1);
}
void enable_int(void)
{
	assert(uxCriticalNesting==1);
	vPortExitCritical();
	assert(uxCriticalNesting==0);
}

void dispatch(void)
{
	unlock_cpu();
	vPortYield();
}

/*-----------------------------------------------------------*/
void set_ipl(IPL ipl)
{

}
IPL  current_ipl(void)
{
	return 0;
}
void start_dispatch(void)
{
	portBASE_TYPE xResult;
	int iSignal;
	sigset_t xSignals;
	sigset_t xSignalToBlock;
	sigset_t xSignalsBlocked;

	/* Establish the signals to block before they are needed. */
	sigfillset( &xSignalToBlock );

	/* Block until the end */
	(void)pthread_sigmask( SIG_SETMASK, &xSignalToBlock, &xSignalsBlocked );

	/* Start the timer that generates the tick ISR.  Interrupts are disabled
	here already. */
	prvSetupTimerInterrupt();

	/* Start the first task. Will not return unless all threads are killed. */
	vPortStartFirstTask();

	/* This is the end signal we are looking for. */
	sigemptyset( &xSignals );
	sigaddset( &xSignals, SIG_RESUME );

	while ( pdTRUE != xSchedulerEnd )
	{
		if ( 0 != sigwait( &xSignals, &iSignal ) )
		{
			PRINTF( "Main thread spurious signal: %d\n", iSignal );
		}
	}

	PRINTF( "Cleaning Up, Exiting.\n" );
	/* Cleanup the mutexes */
	xResult = pthread_mutex_destroy( &xSuspendResumeThreadMutex );
	xResult = pthread_mutex_destroy( &xSingleThreadMutex );

	(void)xResult;
	/* Should not get here! */
	return;
}
void exit_and_dispatch(void)
{
	TaskType save_runtsk = runtsk;

	callevel = TCL_PREPOST;
	PostTaskHook();

	unlock_cpu();

	vPortYield();

	longjmp(pxThreads[save_runtsk].jmp,1);
}
void activate_context(TaskType TaskID)
{
	(void)pthread_once( &hSigSetupThread, prvSetupSignalsAndSchedulerPolicy );

	if ( (pthread_t)NULL == hMainThread )
	{
		hMainThread = pthread_self();
	}

	/* No need to join the threads. */
	pthread_attr_init( &xThreadAttributes );
	pthread_attr_setdetachstate( &xThreadAttributes, PTHREAD_CREATE_DETACHED );

	vPortEnterCritical();

	/* Create the new pThread. */
	if ( 0 == pthread_mutex_lock( &xSingleThreadMutex ) )
	{
		xSentinel = 0;
		if(0 == pxThreads[TaskID].hThread)
		{
			int ercd = pthread_create( &( pxThreads[ TaskID ].hThread ), &xThreadAttributes, prvToppersOSEK_TaskProcess, (void*)(unsigned long)TaskID );
			/* Create the thread itself. */
			assert( (0==ercd) && pxThreads[TaskID].hThread );
		}

		/* Wait until the task suspends. */
		(void)pthread_mutex_unlock( &xSingleThreadMutex );
		while ( xSentinel == 0 );
		vPortExitCritical();
	}

	/* each time when do activate, make event ready */
	(void)pthread_mutex_lock( &(pxThreads[TaskID].hMutex) );
	(void)pthread_cond_signal(&(pxThreads[TaskID].hEvent));
	(void)pthread_mutex_unlock( &(pxThreads[TaskID].hMutex) );

}

void cpu_initialize(void)
{
	memset(pxThreads,0,sizeof(pxThreads));
}
void sys_initialize(void)
{

}
void tool_initialize(void)
{

}

void cpu_terminate(void)
{

}
void sys_exit(void)
{
}

