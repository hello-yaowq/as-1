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
#ifndef STD_TYPES_H
#define STD_TYPES_H
/* ============================ [ INCLUDES  ] ====================================================== */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/* ============================ [ MACROS    ] ====================================================== */
#ifndef MIN
#define MIN(_x,_y) (((_x) < (_y)) ? (_x) : (_y))
#endif
#ifndef MAX
#define MAX(_x,_y) (((_x) > (_y)) ? (_x) : (_y))
#endif
#ifndef E_OK
#define E_OK 					(Std_ReturnType)0
#endif
#ifndef E_NOT_OK
#define E_NOT_OK 				(Std_ReturnType)1
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef NULL_PTR
#define NULL_PTR (void*)0
#endif

#ifndef TRUE
#define TRUE                    (boolean)1
#endif
#ifndef FALSE
#define FALSE                   (boolean)0
#endif

#define STD_HIGH		0x01
#define STD_LOW			0x00

#define STD_ACTIVE		0x01
#define STD_IDLE		0x00

#define STD_ON			0x01
#define STD_OFF			0x00

#ifndef imask_t
#define imask_t uint32
#endif

#define Irq_Save(irq_state)  		irq_state = portGetIrqStateAndDisableIt()
#define Irq_Restore(irq_state)		portRestroeIrqState(irq_state)
/* ============================ [ TYPES     ] ====================================================== */
typedef unsigned char               boolean;
typedef signed char         		sint8;
typedef unsigned char       		uint8;
typedef signed short        		sint16;
typedef unsigned short      		uint16;
typedef signed long         	    sint32;
typedef unsigned long       		uint32;
typedef signed long long  			sint64;
typedef unsigned long long  		uint64;
typedef uint32                      uint8_least;
typedef uint32                      uint16_least;
typedef uint32                      uint32_least;
typedef uint32                      sint8_least;
typedef uint32                      sint16_least;
typedef uint32                      sint32_least;
typedef float                       float32;
typedef double                      float64;

typedef uint8                       Std_ReturnType;

typedef struct {
	// TODO: not done!!
	uint16 vendorID;
	uint16 moduleID;
	uint8  instanceID;

	uint8 sw_major_version;    /**< Vendor numbers */
	uint8 sw_minor_version;    /**< Vendor numbers */
	uint8 sw_patch_version;    /**< Vendor numbers */

	uint8 ar_major_version;    /**< Autosar spec. numbers */
	uint8 ar_minor_version;    /**< Autosar spec. numbers */
	uint8 ar_patch_version;    /**< Autosar spec. numbers */
} Std_VersionInfoType;

/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
extern imask_t portGetIrqStateAndDisableIt(void);
extern void portRestroeIrqState(imask_t irq_state);
#endif /* STD_TYPES_H */
