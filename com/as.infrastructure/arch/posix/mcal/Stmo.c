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
#include "Stmo.h"

/* ============================ [ MACROS    ] ====================================================== */

#define VALIDATE_ID(Id,_api)		\
	if((Id) > STMO_CFG_NUM)			\
	{								\
		goto cleanup;				\
	}

#define GET_STMO_SOFTWARE_ZERO(Id)	\
		(pConfig->Channels[Id].SoftwareZero)
/* ============================ [ TYPES     ] ====================================================== */
typedef struct
{
	Stmo_DegreeType Current;
	Stmo_DegreeType Command;
}Stmo_ControllerType;
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static Stmo_ControllerType  StmoCtrl[STMO_CFG_NUM];
static const Stmo_ConfigType* pConfig = NULL;
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void Stmo_Init(const Stmo_ConfigType *Config)
{
	uint32_t i;
	pConfig = Config;
	memset(StmoCtrl,0,sizeof(StmoCtrl));

	for(i=0;i<STMO_CFG_NUM;i++)
	{
		StmoCtrl[i].Command = GET_STMO_SOFTWARE_ZERO(i);
		StmoCtrl[i].Current = GET_STMO_SOFTWARE_ZERO(i);
	}
}

Std_ReturnType Stmo_SetPosDegree(Stmo_IdType Id,Stmo_DegreeType Degree)
{
	Std_ReturnType ercd = E_NOT_OK;
	VALIDATE_ID(Id,Stmo_SetPosDegree);

	if(Degree <= STMO_MAX_DEGREE)
	{
		StmoCtrl[Id].Command = Degree+GET_STMO_SOFTWARE_ZERO(Id);
		ercd = E_OK;
	}

cleanup:
	return ercd;
}

Std_ReturnType Stmo_GetPosDegree(Stmo_IdType Id,Stmo_DegreeType* Degree)
{
	Std_ReturnType ercd = E_NOT_OK;
	VALIDATE_ID(Id,Stmo_GetPosDegree);

	*Degree = StmoCtrl[Id].Current;
	ercd = E_OK;

cleanup:
	return ercd;
}

void Stmo_MainFunction(void)
{
	for(int i=0;i<STMO_CFG_NUM;i++)
	{
		if(StmoCtrl[i].Command != StmoCtrl[i].Current)
		{
			if(StmoCtrl[i].Command > StmoCtrl[i].Current)
			{
				if(StmoCtrl[i].Command > (StmoCtrl[i].Current+STMO_ONE_STEP))
				{
					StmoCtrl[i].Current= StmoCtrl[i].Current+STMO_ONE_STEP;
				}
				else
				{
					StmoCtrl[i].Current = StmoCtrl[i].Command;
				}
			}
			else if(StmoCtrl[i].Command < StmoCtrl[i].Current)
			{
				if(StmoCtrl[i].Command < (StmoCtrl[i].Current-STMO_ONE_STEP))
				{
					StmoCtrl[i].Current= StmoCtrl[i].Current-STMO_ONE_STEP;
				}
				else
				{
					StmoCtrl[i].Current = StmoCtrl[i].Command;
				}
			}
			else
			{
				StmoCtrl[i].Current = StmoCtrl[i].Command;
			}
		}
	}
}
