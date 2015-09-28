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
#ifndef COM_AS_INFRASTRUCTURE_ARCH_POSIX_MCAL_IPC_H_
#define COM_AS_INFRASTRUCTURE_ARCH_POSIX_MCAL_IPC_H_
/* ============================ [ INCLUDES  ] ====================================================== */
#include "Std_Types.h"
#include "VirtQ.h"
/* ============================ [ MACROS    ] ====================================================== */
#define IPC_MAP_PA_TO_VA(addr) ((void*)(unsigned long)(addr))
/* ============================ [ TYPES     ] ====================================================== */
typedef uint8 Ipc_ChannelType;
typedef void (*Ipc_RxNotificationType)(VirtQ_ChannerlType chl);
typedef void (*Ipc_TxConfirmationType)(VirtQ_ChannerlType chl);
#include "Ipc_Cfg.h"
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void Ipc_Init(const Ipc_ConfigType* config);
void Ipc_WriteIdx(Ipc_ChannelType chl,uint16 idx);
#endif /* COM_AS_INFRASTRUCTURE_ARCH_POSIX_MCAL_IPC_H_ */
