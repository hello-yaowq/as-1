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

/* ============================ [ MACROS    ] ====================================================== */
#define LWIP_ICMP 0
#define LWIP_DHCP 1

#define ETH_MAC_ADDR {0xde,0xed,0xbe,0xef,0xaa,0xbb}

#define EVENT_MASK_SLEEP_TCPIP EVENT_MASK_TaskLwip_Event30
#define EVENT_MASK_START_TCPIP EVENT_MASK_TaskLwip_Event31
#define TASK_ID_tcpip_task TASK_ID_TaskLwip


#define LWIP_DEBUG 1
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
#endif /* COM_AS_APPLICATION_COMMON_CONFIG_LWIP_CFG_H_ */
