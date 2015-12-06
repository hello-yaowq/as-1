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
/* LUA version is 5.3.1 */
/* ============================ [ INCLUDES  ] ====================================================== */
#include "lascanlib.h"
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
typedef void (*luaL_open_t)(void);
typedef void (*luaL_close_t)(void);
/* ============================ [ DECLARES  ] ====================================================== */

/* ============================ [ DATAS     ] ====================================================== */
static const luaL_Reg aslib[] = {
		{"can_write",luai_can_write},
		{"can_read", luai_can_read},
		{"can_open", luai_can_open},
		{NULL,NULL}
};
static const luaL_open_t open_ops[] =
{
	luai_canlib_open,
	NULL
};
static const luaL_open_t close_ops[] =
{
	luai_canlib_close,
	NULL
};
/* ============================ [ LOCALS    ] ====================================================== */

/* ============================ [ FUNCTIONS ] ====================================================== */
LUAMOD_API int luaopen_as (lua_State *L)
{
	const luaL_open_t* libopen = open_ops;
	while((*libopen) != NULL)
	{
		(*libopen)();
		libopen ++;
	}
	luaL_newlib(L, aslib);
	return 1;
}

void luaclose_as(void)
{
	const luaL_close_t* libclose = close_ops;
	while((*libclose) != NULL)
	{
		(*libclose)();
		libclose ++;
	}
}
