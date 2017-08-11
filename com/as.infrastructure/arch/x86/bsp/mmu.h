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
#ifndef __MMU_H_H_H
#define __MMU_H_H_H
/* ============================ [ INCLUDES  ] ====================================================== */
#include "Std_Types.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
typedef struct s_descriptor
{
    uint16_t    limit_low;      /* Limit */
    uint16_t    base_low;       /* Base */
    uint8_t base_mid;           /* Base */
    uint8_t attr1;              /* P(1) DPL(2) DT(1) TYPE(4) */
    uint8_t limit_high_attr2;   /* G(1) D(1) 0(1) AVL(1) LimitHigh(4) */
    uint8_t base_high;          /* Base */
} descriptor_t;
/* ============================ [ DECLARES  ] ====================================================== */

/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */

#endif /* __MMU_H_H_H */
