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
#if defined(__SMALL_OS__) || defined(__CONTIKI_OS__)
#include <sys/time.h>
#include <time.h>
#endif
#ifdef USE_AWS
#include "afb-wsj1.h"
#include "asocket.h"
#endif
#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
#ifdef USE_AWS
typedef struct {
	char* uri;
	int port;
	int s;
	struct afb_wsj1 * wsj1;
} LAS_WebsockParamType;
#endif
/* ============================ [ DECLARES  ] ====================================================== */
extern void Can_SimulatorRunning(void);
#ifdef USE_AWS
static void on_hangup(void *closure, struct afb_wsj1 *wsj1);
static void on_call(void *closure, const char *api, const char *verb, struct afb_wsj1_msg *msg);
static void on_event(void *closure, const char *event, struct afb_wsj1_msg *msg);
static void on_reply(void *closure, struct afb_wsj1_msg *msg);
#endif
/* ============================ [ DATAS     ] ====================================================== */

#if defined(__SMALL_OS__) || defined(__CONTIKI_OS__)
static clock_t previous = 0;
#endif
#ifdef USE_AWS
static const struct afb_wsj1_itf wsj1_itf =
{
	.on_hangup = on_hangup,
	.on_call = on_call,
	.on_event = on_event
};

static LAS_WebsockParamType wsParam =
{
	.s = -1,
};
#endif
/* ============================ [ LOCALS    ] ====================================================== */
#ifdef USE_AWS
static void on_hangup(void *closure, struct afb_wsj1 *wsj1)
{
	LAS_WebsockParamType *param=(LAS_WebsockParamType*)closure;
	ASLOG(ON,"ON-HANGUP on socket <%d> %s:%d\n",param->s,param->uri,param->port);
}
static void on_call(void *closure, const char *api, const char *verb, struct afb_wsj1_msg *msg)
{
	LAS_WebsockParamType *param=(LAS_WebsockParamType*)closure;
	ASLOG(ON,"ON-CALL %s/%s(%s) on socket <%d> %s:%d\n", api, verb, afb_wsj1_msg_object_s(msg),param->s,param->uri,param->port);
	int rv=afb_wsj1_reply_s(msg, "{\"hi\": \"this is an example\"}", "122346", 0);
	if(rv)
		printf("reply failed\n");
	else
		printf("reply okay\n");
	afb_wsj1_msg_unref(msg);
}
static void on_event(void *closure, const char *event, struct afb_wsj1_msg *msg)
{
	LAS_WebsockParamType *param=(LAS_WebsockParamType*)closure;
	ASLOG(ON,"ON-EVENT %s(%s)  on socket <%d> %s:%d\n", event, afb_wsj1_msg_object_s(msg),param->s,param->uri,param->port);
	afb_wsj1_msg_unref(msg);
}
static void on_reply(void *closure, struct afb_wsj1_msg *msg)
{
	LAS_WebsockParamType *param=(LAS_WebsockParamType*)closure;
	ASLOG(ON,"ON-REPLY : %s  on socket <%d> %s:%d\n",afb_wsj1_msg_object_s(msg),param->s,param->uri,param->port);
	afb_wsj1_msg_unref(msg);
}
#endif
/* ============================ [ FUNCTIONS ] ====================================================== */
KSM(Simulator,Init)
{
#ifdef USE_AWS	
	int s,port;
	char* p;
	char* uri = strdup(ASENV(2));
	p = strstr(uri,":");
	if(p != NULL)
	{
		*p = 0;
		p++;
		port = atoi(p);
		s = ask_create(0,uri,port);
		if(s > 0)
		{
			wsParam.s = s;
			wsParam.port = port;
			wsParam.uri = uri;
			wsParam.wsj1 = afb_wsj1_create(s,&wsj1_itf,&wsParam);
			asAssert(wsParam.wsj1);
			ASLOG(STDOUT,"websock wsj(%d) online %s:%d\n",s,uri,port);
			afb_wsj1_call_s(wsParam.wsj1, "gui", "hello", NULL, on_reply,&wsParam);
		}
	}

	if(wsParam.s <= 0)
	{
		free(uri);
		ASLOG(STDOUT,"websock wsj offline\n");
	}
#endif
	KGS(Simulator,Running);
}
KSM(Simulator,Start)
{

}
KSM(Simulator,Stop)
{

}
KSM(Simulator,Running)
{
	Can_SimulatorRunning();

#if defined(__SMALL_OS__) || defined(__CONTIKI_OS__)
	clock_t now = clock();
	if( (0u == (now%(CLOCKS_PER_SEC/1000))) &&  (now != previous) )
	{
		previous = clock();
		OsTick();
	}
#endif
}

#ifdef __AS_BOOTLOADER__
void application_main(void) { }
uint32_t FlashDriverRam[1024];
#endif

int AsWsjOnline(void)
{
	return (wsParam.s > 0);
}
