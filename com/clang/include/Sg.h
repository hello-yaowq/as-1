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
#ifndef COM_CLANG_INCLUDE_SG_H_
#define COM_CLANG_INCLUDE_SG_H_
/* ============================ [ INCLUDES  ] ====================================================== */
#include "Std_Types.h"

/* ============================ [ MACROS    ] ====================================================== */
/* declaration of the LCD size */
#define SG_LCD_WIGTH 	300
#define SG_LCD_WHEIGHT  400
/* ============================ [ TYPES     ] ====================================================== */

/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void Sg_Init(void);
void Sg_ManagerTask(void);
#endif /* COM_CLANG_INCLUDE_SG_H_ */
