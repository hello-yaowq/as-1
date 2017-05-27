#ifndef _LASDEVLIB_H_
#define _LASDEVLIB_H_
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
#include "Std_Types.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
/* ============================ [ MACROS    ] ====================================================== */
#define LAS_DEVICE_NAME_SIZE 32
/* ============================ [ TYPES     ] ====================================================== */
/* device: RS232 - COM.1 COM.2 .... */
typedef int (*lasdev_open_t)  (const char* device, lua_State *L, void** param);
typedef int (*lasdev_read_t)  (void* param,lua_State *L);
typedef int (*lasdev_write_t) (void* param,const char* data,size_t size);
typedef int (*lasdev_ioctl_t) (void* param,int type, const char* data,size_t size);
typedef void (*lasdev_close_t) (void* param);

typedef struct
{
	char name[LAS_DEVICE_NAME_SIZE];
	lasdev_open_t open;
	lasdev_read_t read;
	lasdev_write_t write;
	lasdev_ioctl_t ioctl;
	lasdev_close_t close;
}LAS_DeviceOpsType;

/* ============================ [ DECLARES  ] ====================================================== */
extern const LAS_DeviceOpsType rs232_dev_ops;
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void luai_asdevlib_open(void);
int luai_as_open  (lua_State *L);
int luai_as_read  (lua_State *L);
int luai_as_write  (lua_State *L);
int luai_as_ioctl  (lua_State *L);
int luai_as_close  (lua_State *L);
#endif /* _LASDEVLIB_H_ */
