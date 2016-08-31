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
#ifndef COM_AS_APPLICATION_COMMON_CONFIG_XCP_CFG_H_
#define COM_AS_APPLICATION_COMMON_CONFIG_XCP_CFG_H_
/* ============================ [ INCLUDES  ] ====================================================== */
#include "Xcp_ConfigTypes.h"
/* ============================ [ MACROS    ] ====================================================== */
#define XCP_IDENTIFICATION 0
#define XCP_DAQ_COUNT  8
#define XCP_ODT_COUNT  8
#define XCP_ODT_ENTRIES_COUNT 8
#define XCP_MAX_CTO 1
#define XCP_MAX_DTO 1
#define XCP_MAX_RXTX_QUEUE 8

#define XCP_PROTOCOL XCP_PROTOCOL_CAN

#define XCP_PDU_ID_TX 0
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
#endif /* COM_AS_APPLICATION_COMMON_CONFIG_XCP_CFG_H_ */
