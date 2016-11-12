/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2016  AS <parai@foxmail.com>
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
#ifndef COM_AS_APPLICATION_COMMON_CONFIG_LWIP_CFG_H_
#define COM_AS_APPLICATION_COMMON_CONFIG_LWIP_CFG_H_
/* ============================ [ INCLUDES  ] ====================================================== */
#include "sys/time.h"
/* ============================ [ MACROS    ] ====================================================== */
#define LWIP_ICMP 0
#define LWIP_DHCP 1

#define ETH_MAC_ADDR {0xde,0xed,0xbe,0xef,0xaa,0xbb}

#define EVENT_MASK_SLEEP_TCPIP EVENT_MASK_TaskLwip_Event30
#define EVENT_MASK_START_TCPIP EVENT_MASK_TaskLwip_Event31
#define TASK_ID_tcpip_task TASK_ID_TaskLwip


#define LWIP_DEBUG 1
/* ============================ [ TYPES     ] ====================================================== */
#ifdef __WINDOWS__
#include <errno.h>
typedef enum {
	EWOULDBLOCK = 0x0043,		// Operation would block
	EINPROGRESS = 0x0044,		// Operation now in progress
	EALREADY = 0x0045,			// Operation already in progress
	ENOTSOCK = 0x0046,			// Socket operation on non-socket
	EDESTADDRREQ = 0x0047,		// Destination address required
	EMSGSIZE = 0x0048,			// Message too long
	EPROTOTYPE = 0x0049,		// Protocol wrong type for socket
	ENOPROTOOPT = 0x004a,		// Protocol not available
	EPROTONOSUPPORT = 0x004b,	// Protocol not supported
	EOPNOTSUPP = 0x004d,		// Operation not supported
	// ENOTSUP  				// EOPNOTSUPP: Operation not supported
	EAFNOSUPPORT = 0x004f,		// Address family not supported by protocol family
	EADDRINUSE = 0x0050,		// Address already in use
	EADDRNOTAVAIL = 0x0051,		// Can't assign requested address
	ENETDOWN = 0x0052,			// Network is down
	ENETUNREACH = 0x0053,		// Network is unreachable
	ENETRESET = 0x0054,			// Network dropped connection on reset
	ECONNABORTED = 0x0055,		// Software caused connection abort
	ECONNRESET = 0x0056,		// Connection reset by peer
	ENOBUFS = 0x0057,			// No buffer space available
	EISCONN = 0x0058,			// Socket is already connected
	ENOTCONN = 0x0059,			// Socket is not connected
	ETIMEDOUT = 0x005c,			// Operation timed out
	ECONNREFUSED = 0x005d,		// Connection refused
	ELOOP = 0x005e,				// Too many levels of symbolic links
	EHOSTUNREACH = 0x0061,		// No route to host
} SoAd_TcpIpErrorType; /** @req SOAD117 */
#endif
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
#endif /* COM_AS_APPLICATION_COMMON_CONFIG_LWIP_CFG_H_ */
