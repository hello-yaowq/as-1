/* -------------------------------- Arctic Core ------------------------------
 * Arctic Core - the open source AUTOSAR platform http://arccore.com
 *
 * Copyright (C) 2009  ArcCore AB <contact@arccore.com>
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation; See <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * -------------------------------- Arctic Core ------------------------------*/

/*
 *  DESCRIPTION
 *    Provides functions to register and run shell commands.
 */

/* ----------------------------[Includes]------------------------------------*/

#include <ctype.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "shell.h"
#if defined(__LINUX__) || defined(__WINDOWS__)
#include <pthread.h>
#include <semaphore.h>
#ifdef USE_SCHM
#include "Os.h"
#endif
#else
#include "Os.h"
#endif
#include "asdebug.h"
/* ----------------------------[Private define]------------------------------*/

/* The maximum number of arguments when calling a shell function */
#if defined(__LINUX__) || defined(__WINDOWS__)
#define MAX_ARGS		128
#define CMDLINE_MAX		4096
#else
#define MAX_ARGS		32
#define CMDLINE_MAX		256
#endif

#define IBUFFER_MAX    CMDLINE_MAX

#define AS_LOG_SHELL 0

/* ----------------------------[Private macro]-------------------------------*/
/* ----------------------------[Private typedef]-----------------------------*/


struct shellWord {
	int initialized;
	TAILQ_HEAD(,ShellCmd) cmdHead;
};

/* ----------------------------[Private function prototypes]-----------------*/

static int shellHelp(int argc, char *argv[] );
#ifndef __LINUX__
extern char *strtok_r(char *s1, const char *s2, char **s3);
#endif
#if defined(USE_SHELL_SYMTAB)
extern const ShellCmdT* __ssymtab_start[];
extern const ShellCmdT* __ssymtab_end[];
#endif
/* ----------------------------[Private variables]---------------------------*/
struct shellWord shellWorld;
static SHELL_CONST ShellCmdT helpInfo  = {
		shellHelp,
		0,1,
		"help",
		"help <cmd>",
		"Show all commands all help no a specific command\n",
		{NULL,NULL}
};

SHELL_CMD_EXPORT(helpInfo);

static char cmdBuf[CMDLINE_MAX];

#if defined(__LINUX__) || defined(__WINDOWS__)
static sem_t semInput;
void* ProcessStdio(void* arg)
{
	char ch;
	(void)arg;
	while(1)
	{
		ch = getchar();
		SHELL_input(ch);
	}
	return NULL;
}
#endif

static uint32_t rpos=0;
static uint32_t wpos=0;
static volatile uint32_t isize=0;
static char     ibuffer[IBUFFER_MAX];

void SHELL_input(char c)
{
	imask_t imask;
	if(isize < IBUFFER_MAX)
	{
		ibuffer[wpos] = c;
		wpos ++;
		if(wpos >= IBUFFER_MAX)
		{
			wpos = 0;
		}
		Irq_Save(imask);
		isize ++;
		Irq_Restore(imask);
	}
	else
	{
		ASWARNING("shell input buffer overflow!\n");
	}
#if !defined(__LINUX__) && !defined(__WINDOWS__)
	if(E_OK != OsSetEvent(TaskShell, EventShellInput))
	{
		asAssert(0);
	}
#else
	sem_post(&semInput);
#endif
}

static char SHELL_getc(void)
{
	char c;
	imask_t imask;
	while(0 == isize)
	{
#if !defined(__LINUX__) && !defined(__WINDOWS__)
		if(E_OK != OsWaitEvent(TaskShell, EventShellInput))
		{
			asAssert(0);
		}
		OsClearEvent(TaskShell, EventShellInput);
#else
		sem_wait(&semInput);
#endif
	}

	c = ibuffer[rpos];

	rpos ++;
	if(rpos >= IBUFFER_MAX)
	{
		rpos = 0;
	}
	Irq_Save(imask);
	isize --;
	Irq_Restore(imask);

	return c;
}
/* ----------------------------[Private functions]---------------------------*/
/**
 * Split and string into tokens and strip the token from whitespace.
 *
 * @param s1 See strtok_r documentation
 * @param s2 See strtok_r documentation
 * @param s3 See strtok_r documentation
 * @return
 */
static char *strtokAndTrim(char *s1, const char *s2, char **s3)
{

  char *str = strtok_r(s1, s2, s3);

  return str;
}

/**
 * Implements the help command. Either type just "help" to list all available
 * command that is registered or "help <cmd>" to get detailed help on a specific
 * command.
 *
 * @param argc
 * @param argv
 * @return
 */

static int shellHelp(int argc, char *argv[] ) {
	char *cmd = NULL;
	ShellCmdT *iCmd;
#if defined(USE_SHELL_SYMTAB)
	const ShellCmdT** iter;
#endif

	if(argc == 1 ) {
		/* display "help" */
		SHELL_puts("List of commands:\n");
		TAILQ_FOREACH(iCmd,&shellWorld.cmdHead,cmdEntry ) {
			SHELL_printf("%-15s - %s\n",iCmd->cmd, iCmd->shortDesc);
		}
#if defined(USE_SHELL_SYMTAB)
		for(iter=__ssymtab_start; iter < __ssymtab_end; iter++)
		{
			SHELL_printf("%-15s - %s\n",(*iter)->cmd, (*iter)->shortDesc);
		}
#endif
	} else {
		cmd = argv[1];
		/* display "help <cmd>" */
		TAILQ_FOREACH(iCmd,&shellWorld.cmdHead,cmdEntry ) {
			if( strcmp(cmd,iCmd->cmd) == 0 ) {
				SHELL_printf("%-15s - %s\n",iCmd->cmd, iCmd->shortDesc);
				SHELL_printf("%s\n",iCmd->longDesc);
			}
		}
#if defined(USE_SHELL_SYMTAB)
		for(iter=__ssymtab_start; iter < __ssymtab_end; iter++)
		{
			if( strcmp(cmd,(*iter)->cmd) == 0 ) {
				SHELL_printf("%-15s - %s\n",(*iter)->cmd, (*iter)->shortDesc);
				SHELL_printf("%s\n",(*iter)->longDesc);
			}
		}
#endif
	}

	return 0;
}

/* ----------------------------[Public functions]----------------------------*/

/**
 *
 * @return
 */
int SHELL_Init( void ) {
	shellWorld.initialized = 1;
	TAILQ_INIT(&shellWorld.cmdHead);
#if !defined(USE_SHELL_SYMTAB)
	SHELL_AddCmd(&helpInfo);
#endif
	return 0;
}

/**
 * Add a command to list of commands
 * @param shellCmd
 * @return
 */
int SHELL_AddCmd(ShellCmdT *shellCmd) {
	if(shellWorld.initialized != 1 ) {
		SHELL_Init();
	}

	TAILQ_INSERT_TAIL(&shellWorld.cmdHead, shellCmd, cmdEntry );

	return 0;
}

/**
 * Run a command from current context
 *
 * @param cmdArgs    The command string
 * @param cmdRv		The return value from the command run
 * @return
 */
int SHELL_RunCmd(const char *cmdArgs, int *cmdRv ) {
	char *delim = " \t\n";
	char *token_r;
	ShellCmdT *iCmd;
	size_t len;
	char *cmdStr;
	const ShellCmdT *runCmd = NULL;
	int argc = 0;
	char *argv[MAX_ARGS];
	char *arg;
#if defined(USE_SHELL_SYMTAB)
	const ShellCmdT** iter;
#endif

	*cmdRv = 1;

	ASLOG(SHELL,"run cmd '%s'\n",cmdArgs);

	if (cmdArgs == NULL) {
		return SHELL_E_CMD_IS_NULL;
	}

	len = strlen(cmdArgs) + 1; /* Add termination */
	if (sizeof(cmdBuf) < len) {
		return SHELL_E_CMD_TOO_LONG;
	}

	/* Copy to a buffer we can mess with so that strtok have
	 * something to work on */
	memcpy(cmdBuf, cmdArgs, len);
	cmdStr = strtokAndTrim(cmdBuf, delim, &token_r);

	if(NULL == cmdStr)
	{
		ASLOG(SHELL,"error when parse cmdStr\n");
		return SHELL_E_CMD_IS_NULL;
	}

#if defined(USE_SHELL_SYMTAB)
	for(iter=__ssymtab_start; iter < __ssymtab_end; iter++)
	{
		if( strcmp(cmdStr,(*iter)->cmd) == 0 ) {
			runCmd = *iter;
			break;
		}
	}
#endif

	/* post add cmd has higher priority */
	/* Search for the command */
	TAILQ_FOREACH(iCmd,&shellWorld.cmdHead,cmdEntry ) {
		if( strcmp(cmdStr,iCmd->cmd) == 0 ) {
			runCmd = iCmd;
			break;
		}
	}

#ifdef USE_LIBDL
	if (NULL == runCmd) {
		extern SHELL_CONST ShellCmdT dllCmd;
		runCmd = &dllCmd;
	}
#endif

	/* Check arg count and deliver them into argc and argv */
	if (runCmd != NULL) {
		/* Add the cmd */
		argv[0] = cmdStr;
		argc++;

		while( (arg = strtokAndTrim(NULL, delim, &token_r)) != NULL ) {
			asAssert(argc<MAX_ARGS);

			if(NULL == arg)
			{
				ASLOG(SHELL,"error when parse arg\n");
				return SHELL_E_CMD_IS_NULL;
			}

			argv[argc++] = arg;
		}

		if ( FALSE == ( ((argc-1)>=runCmd->argMin) && ((argc-1)<=runCmd->argMax) ) )
		{
			SHELL_printf("Invalid number of args\n");
			return SHELL_E_INVAID_ARG;
		}

		*cmdRv = runCmd->func(argc, argv);

		SHELL_printf("\nexit(%d)\n", *cmdRv);

	} else {
		SHELL_printf("No such command:\"%s\",strlen=%d\n",cmdStr,(int)strlen(cmdStr));
		return SHELL_E_NO_SUCH_CMD;
	}
	return SHELL_E_OK;
}

static void doPrompt( void ) {
	SHELL_puts("[AS] $ ");
}


int SHELL_Mainloop( void ) {
	char c;
	static char cmdLine[CMDLINE_MAX];
	int lineIndex = 0;
	int cmdRv;
#if defined(__LINUX__) || defined(__WINDOWS__)
	pthread_t thread;
	pthread_create(&thread, NULL, ProcessStdio, NULL);
	sem_init(&semInput, 0, 0);
#endif
	SHELL_puts("AS Shell version 0.1\n");
	doPrompt();

	for(;;) {
		c = SHELL_getc();
		if( lineIndex >= CMDLINE_MAX ) {
			lineIndex = 0;
		}

		if( c == '\b') {
			lineIndex--;
			#ifdef ENABLE_SHELL_ECHO_BACK
			SHELL_putc(c);
			#endif
		} else if( c == '\r')
		{
			#ifdef ENABLE_SHELL_ECHO_BACK
			SHELL_putc(c);
			#endif
		}
		else if( c == '\n' ) {
			#ifdef ENABLE_SHELL_ECHO_BACK
			SHELL_putc(c);
			#endif
			cmdLine[lineIndex] = '\0';
			SHELL_RunCmd(cmdLine,&cmdRv);
			lineIndex = 0;
			doPrompt();
		} else {
			cmdLine[lineIndex++] = c;
			#ifdef ENABLE_SHELL_ECHO_BACK
			SHELL_putc(c);
			#endif
		}
	}
}

#ifdef USE_SCHM
TASK(TaskShell)
{
#if defined(__LINUX__) || defined(__WINDOWS__)
	pthread_t thread;
	pthread_create(&thread, NULL, (void * (*)(void *))SHELL_Mainloop, NULL);
#else
	SHELL_Mainloop();
#endif
	OsTerminateTask(TaskShell);
}
#endif

