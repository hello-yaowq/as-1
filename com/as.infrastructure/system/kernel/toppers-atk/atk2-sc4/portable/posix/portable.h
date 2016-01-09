#ifndef COM_AS_INFRASTRUCTURE_SYSTEM_KERNEL_TOPPERS_ATK_ATK2_SC4_PORTABLE_POSIX_PORTABLE_H_
#define COM_AS_INFRASTRUCTURE_SYSTEM_KERNEL_TOPPERS_ATK_ATK2_SC4_PORTABLE_POSIX_PORTABLE_H_
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

/* ============================ [ MACROS    ] ====================================================== */
#define OMIT_STKMPUINFOB
#define OMIT_OSAPMPUINFOB

#define TARGET_SVC_NUM 0
#define TARGET_SVC_TABLE
/* ============================ [ TYPES     ] ====================================================== */
typedef uint32   uintptr;
typedef sint32   sintptr;
typedef char     char8;
typedef uint32	 TickType;
typedef uint8 	 StatusType;

typedef void*    TSKCTXB;
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */


#endif /* COM_AS_INFRASTRUCTURE_SYSTEM_KERNEL_TOPPERS_ATK_ATK2_SC4_PORTABLE_POSIX_PORTABLE_H_ */
