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
#include "Os.h"
#include "Can.h"
#include "Mcu.h"
#include "CanIf_Cbk.h"
#if defined(USE_DET)
#include "Det.h"
#endif
#if defined(USE_DEM)
#include "Dem.h"
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* ============================ [ MACROS    ] ====================================================== */
#define USE_CAN_STATISTICS      STD_ON

/* CONFIGURATION NOTES
 * ------------------------------------------------------------------
 * - CanHandleType must be CAN_ARC_HANDLE_TYPE_BASIC
 *   i.e. CanHandleType=CAN_ARC_HANDLE_TYPE_FULL NOT supported
 *   i.e CanIdValue is NOT supported
 * - All CanXXXProcessing must be CAN_ARC_PROCESS_TYPE_INTERRUPT
 *   ie CAN_ARC_PROCESS_TYPE_POLLED not supported
 * - HOH's for Tx are global and Rx are for each controller
 */

/* IMPLEMENTATION NOTES
 * -----------------------------------------------
 * - A HOH us unique for a controller( not a config-set )
 * - Hrh's are numbered for each controller from 0
 * - Only one transmit mailbox is used because otherwise
 *   we cannot use tx_confirmation since there is no way to know
 *   which mailbox caused the tx interrupt. TP will need this feature.
 * - Sleep,wakeup not fully implemented since other modules lack functionality
 */

/* ABBREVATIONS
 *  -----------------------------------------------
 * - Can Hardware unit - One or multiple Can controllers of the same type.
 * - Hrh - HOH with receive definitions
 * - Hth - HOH with transmit definitions
 *
 */


#define GET_CONTROLLER_CONFIG(_controller)	\
        					&Can_Global.config->CanConfigSet->CanController[(_controller)]

#define GET_CALLBACKS() \
							(Can_Global.config->CanConfigSet->CanCallbacks)

#define GET_PRIVATE_DATA(_controller) \
									&CanUnit[_controller]

#define GET_CONTROLLER_CNT() (CAN_CONTROLLER_CNT)


#if ( CAN_DEV_ERROR_DETECT == STD_ON )
#define VALIDATE(_exp,_api,_err ) \
        if( !(_exp) ) { \
          Det_ReportError(MODULE_ID_CAN,0,_api,_err); \
          return CAN_NOT_OK; \
        }

#define VALIDATE_NO_RV(_exp,_api,_err ) \
        if( !(_exp) ) { \
          Det_ReportError(MODULE_ID_CAN,0,_api,_err); \
          return; \
        }

#define DET_REPORTERROR(_x,_y,_z,_q) Det_ReportError(_x, _y, _z, _q)
#else
#define VALIDATE(_exp,_api,_err )
#define VALIDATE_NO_RV(_exp,_api,_err )
#define DET_REPORTERROR(_x,_y,_z,_q)
#endif

#if defined(USE_DEM)
#define VALIDATE_DEM_NO_RV(_exp,_err ) \
        if( !(_exp) ) { \
          Dem_ReportErrorStatus(_err, DEM_EVENT_STATUS_FAILED); \
          return; \
        }
#else
#define VALIDATE_DEM_NO_RV(_exp,_err )
#endif

#define CAN_EMPTY_MESSAGE_BOX 0xFFFF

/* FILE: transmit message and receive message through file io simulation
 * This RAM_DISK is a disk created from RAM memory by tool RAMDisk */
#define CAN_RAM_DISK  "E:"
/* ============================ [ TYPES     ] ====================================================== */

typedef enum
{
  CAN_UNINIT = 0,
  CAN_READY
} Can_DriverStateType;

// Mapping between HRH and Controller//HOH
typedef struct Can_Arc_ObjectHOHMapStruct
{
  CanControllerIdType CanControllerRef;    // Reference to controller
  const Can_HardwareObjectType* CanHOHRef;       // Reference to HOH.
} Can_Arc_ObjectHOHMapType;

/* Type for holding global information used by the driver */
typedef struct {
  Can_DriverStateType initRun;

  // Our config
  const Can_ConfigType *config;

  // One bit for each channel that is configured.
  // Used to determine if validity of a channel
  // 1 - configured
  // 0 - NOT configured
  uint32  configured;
  // Maps the a channel id to a configured channel id
  uint8   channelMap[CAN_CONTROLLER_CNT];

  // This is a map that maps the HTH:s with the controller and Hoh. It is built
  // during Can_Init and is used to make things faster during a transmit.
  Can_Arc_ObjectHOHMapType CanHTHMap[NUM_OF_HTHS];
} Can_GlobalType;

// Global config
Can_GlobalType Can_Global =
{
  .initRun = CAN_UNINIT,
};

/* Type for holding information about each controller */
typedef struct {
  CanIf_ControllerModeType state;
  uint8		lock_cnt;

  // Statistics
#if (USE_CAN_STATISTICS == STD_ON)
    Can_Arc_StatisticsType stats;
#endif

  // Data stored for Txconfirmation callbacks to CanIf
  PduIdType swPduHandle; //
} Can_UnitType;

/* ============================ [ DATAS     ] ====================================================== */
Can_UnitType CanUnit[CAN_CONTROLLER_CNT] =
{
  {
    .state = CANIF_CS_UNINIT,
  },
  {
    .state = CANIF_CS_UNINIT,
  },
  {
    .state = CANIF_CS_UNINIT,
  },
  {
    .state = CANIF_CS_UNINIT,
  },
  {
    .state = CANIF_CS_UNINIT,
  },
};
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== *//**
 * Function that finds the Hoh( HardwareObjectHandle ) from a Hth
 * A HTH may connect to one or several HOH's. Just find the first one.
 *
 * @param hth The transmit handle
 * @returns Ptr to the Hoh
 */
static const Can_HardwareObjectType * Can_FindHoh( Can_Arc_HTHType hth , uint32* controller)
{
  const Can_HardwareObjectType *hohObj;
  const Can_Arc_ObjectHOHMapType *map;

  map = &Can_Global.CanHTHMap[hth];

  // Verify that this is the correct map
  if (map->CanHOHRef->CanObjectId != hth)
  {
    DET_REPORTERROR(MODULE_ID_CAN, 0, 0x6, CAN_E_PARAM_HANDLE);
  }
  hohObj = map->CanHOHRef;

  // Verify that this is the correct Hoh type
  if ( hohObj->CanObjectType == CAN_OBJECT_TYPE_TRANSMIT)
  {
    *controller = map->CanControllerRef;
    return hohObj;
  }

  DET_REPORTERROR(MODULE_ID_CAN, 0, 0x6, CAN_E_PARAM_HANDLE);

  return NULL;
}

// This initiates ALL can controllers
void Can_Init( const Can_ConfigType *config ) {
  Can_UnitType *canUnit;
  const Can_ControllerConfigType *canHwConfig;
  uint8 ctlrId;

  VALIDATE_NO_RV( (Can_Global.initRun == CAN_UNINIT), 0x0, CAN_E_TRANSITION );
  VALIDATE_NO_RV( (config != NULL ), 0x0, CAN_E_PARAM_POINTER );

  // Save config
  Can_Global.config = config;
  Can_Global.initRun = CAN_READY;


  for (int configId=0; configId < CAN_CTRL_CONFIG_CNT; configId++) {
    canHwConfig = GET_CONTROLLER_CONFIG(configId);
    ctlrId = canHwConfig->CanControllerId;

    // Assign the configuration channel used later..
    Can_Global.channelMap[canHwConfig->CanControllerId] = configId;
    Can_Global.configured |= (1<<ctlrId);

    canUnit = GET_PRIVATE_DATA(ctlrId);
    canUnit->state = CANIF_CS_STOPPED;

    canUnit->lock_cnt = 0;
    canUnit->swPduHandle = 0xFFFF;	/* 0xFFFF marked as Empty and invalid */

    // Clear stats
#if (USE_CAN_STATISTICS == STD_ON)
    memset(&canUnit->stats, 0, sizeof(Can_Arc_StatisticsType));
#endif


    Can_InitController(ctlrId, canHwConfig);

    // Loop through all Hoh:s and map them into the HTHMap
    const Can_HardwareObjectType* hoh;
    hoh = canHwConfig->Can_Arc_Hoh;
    hoh--;
    do
    {
      hoh++;

      if (hoh->CanObjectType == CAN_OBJECT_TYPE_TRANSMIT)
      {
        Can_Global.CanHTHMap[hoh->CanObjectId].CanControllerRef = canHwConfig->CanControllerId;
        Can_Global.CanHTHMap[hoh->CanObjectId].CanHOHRef = hoh;
      }
    } while (!hoh->Can_Arc_EOL);
  }
  return;
}

// Unitialize the module
void Can_DeInit()
{
  Can_UnitType *canUnit;
  const Can_ControllerConfigType *canHwConfig;
  uint32 ctlrId;

  for (int configId=0; configId < CAN_CTRL_CONFIG_CNT; configId++) {
    canHwConfig = GET_CONTROLLER_CONFIG(configId);
    ctlrId = canHwConfig->CanControllerId;

    canUnit = GET_PRIVATE_DATA(ctlrId);
    canUnit->state = CANIF_CS_UNINIT;

    Can_DisableControllerInterrupts(ctlrId);

    canUnit->lock_cnt = 0;

    // Clear stats
#if (USE_CAN_STATISTICS == STD_ON)
    memset(&canUnit->stats, 0, sizeof(Can_Arc_StatisticsType));
#endif
  }

  Can_Global.config = NULL;
  Can_Global.initRun = CAN_UNINIT;

  return;
}

void Can_InitController( uint8 controller, const Can_ControllerConfigType *config)
{
  Can_UnitType *canUnit;
  uint8 cId = controller;
  const Can_ControllerConfigType *canHwConfig;
  const Can_HardwareObjectType *hohObj;

  VALIDATE_NO_RV( (Can_Global.initRun == CAN_READY), 0x2, CAN_E_UNINIT );
  VALIDATE_NO_RV( (config != NULL ), 0x2,CAN_E_PARAM_POINTER);
  VALIDATE_NO_RV( (controller < GET_CONTROLLER_CNT()), 0x2, CAN_E_PARAM_CONTROLLER );

  canUnit = GET_PRIVATE_DATA(controller);

  VALIDATE_NO_RV( (canUnit->state==CANIF_CS_STOPPED), 0x2, CAN_E_TRANSITION );

  canHwConfig = GET_CONTROLLER_CONFIG(Can_Global.channelMap[cId]);

  // Start this baby up
  // set CAN enable bit, deactivate listen-only mode,
  // use Bus Clock as clock source and select loop back mode on/off
  // acceptance filters
   hohObj = canHwConfig->Can_Arc_Hoh;
   --hohObj;
   do {
     ++hohObj;
     if (hohObj->CanObjectType == CAN_OBJECT_TYPE_RECEIVE)
     {

     }
   }while( !hohObj->Can_Arc_EOL );

  canUnit->state = CANIF_CS_STOPPED;
  Can_EnableControllerInterrupts(cId);

  return;
}


Can_ReturnType Can_SetControllerMode( uint8 controller, Can_StateTransitionType transition ) {

  Can_ReturnType rv = CAN_OK;
  VALIDATE( (controller < GET_CONTROLLER_CNT()), 0x3, CAN_E_PARAM_CONTROLLER );

  Can_UnitType *canUnit = GET_PRIVATE_DATA(controller);

  VALIDATE( (canUnit->state!=CANIF_CS_UNINIT), 0x3, CAN_E_UNINIT );
  switch(transition )
  {
	  case CAN_T_START:
		canUnit->state = CANIF_CS_STARTED;
		if (canUnit->lock_cnt == 0){   // REQ CAN196
		  Can_EnableControllerInterrupts(controller);
		}
		break;
	  case CAN_T_WAKEUP:
		VALIDATE(canUnit->state == CANIF_CS_SLEEP, 0x3, CAN_E_TRANSITION);
		canUnit->state = CANIF_CS_STOPPED;
		break;
	  case CAN_T_SLEEP:  //CAN258, CAN290
		// Should be reported to DEM but DET is the next best
		VALIDATE(canUnit->state == CANIF_CS_STOPPED, 0x3, CAN_E_TRANSITION);
		canUnit->state = CANIF_CS_SLEEP;
		break;
	  case CAN_T_STOP:
		// Stop
		canUnit->state = CANIF_CS_STOPPED;
		break;
	  default:
		// Should be reported to DEM but DET is the next best
		VALIDATE(canUnit->state == CANIF_CS_STOPPED, 0x3, CAN_E_TRANSITION);
		break;
  }
  return rv;
}

void Can_DisableControllerInterrupts( uint8 controller )
{
  Can_UnitType *canUnit;

  VALIDATE_NO_RV( (controller < GET_CONTROLLER_CNT()), 0x4, CAN_E_PARAM_CONTROLLER );

  canUnit = GET_PRIVATE_DATA(controller);

  VALIDATE_NO_RV( (canUnit->state!=CANIF_CS_UNINIT), 0x4, CAN_E_UNINIT );

  if(canUnit->lock_cnt > 0 )
  {
    // Interrupts already disabled
    canUnit->lock_cnt++;
    return;
  }
  canUnit->lock_cnt++;

 }

void Can_EnableControllerInterrupts( uint8 controller ) {
  Can_UnitType *canUnit;
  VALIDATE_NO_RV( (controller < GET_CONTROLLER_CNT()), 0x5, CAN_E_PARAM_CONTROLLER );

  canUnit = GET_PRIVATE_DATA(controller);

  VALIDATE_NO_RV( (canUnit->state!=CANIF_CS_UNINIT), 0x5, CAN_E_UNINIT );

  if( canUnit->lock_cnt > 1 )
  {
    // IRQ should still be disabled so just decrement counter
    canUnit->lock_cnt--;
    return;
  } else if (canUnit->lock_cnt == 1)
  {
    canUnit->lock_cnt = 0;
  }

  return;
}

Can_ReturnType Can_Write( Can_HwHandleType hth, Can_PduType *pduInfo ) {
  Can_ReturnType rv = CAN_OK;
  const Can_HardwareObjectType *hohObj;
  uint32 controller;

  VALIDATE( (Can_Global.initRun == CAN_READY), 0x6, CAN_E_UNINIT );
  VALIDATE( (pduInfo != NULL), 0x6, CAN_E_PARAM_POINTER );
  VALIDATE( (pduInfo->length <= 8), 0x6, CAN_E_PARAM_DLC );
  VALIDATE( (hth < NUM_OF_HTHS ), 0x6, CAN_E_PARAM_HANDLE );

  hohObj = Can_FindHoh(hth, &controller);
  if (hohObj == NULL)
  {
    return CAN_NOT_OK;
  }

  Can_UnitType *canUnit = GET_PRIVATE_DATA(controller);

  if(CANIF_CS_STARTED == canUnit->state)
  {
	  if(CAN_EMPTY_MESSAGE_BOX == canUnit->swPduHandle)	/* check for any free box */
	  {
		  FILE* fp;
		  char file_name[64];
		  sprintf(file_name,"%s/autosar.can.tx.bus%d",CAN_RAM_DISK,(int)controller);
		  fp = fopen(file_name,"w");
		  if(fp!=NULL)
		  {
			  printf("  >> CAN%d TX ID=0x%-3X,DLC=%d,DATA=[",controller,pduInfo->id,pduInfo->length);
			  for(int i=0;i<pduInfo->length;i++)
			  {
				  printf("%02X,",pduInfo->sdu[i]);
			  }
			  printf("]\n");
			  canUnit->swPduHandle = pduInfo->swPduHandle;
			  // Increment statistics
			  #if (USE_CAN_STATISTICS == STD_ON)
			  canUnit->stats.txSuccessCnt++;
			  #endif

			  fclose(fp);
		  }
		  else
		  {
			  rv = CAN_NOT_OK;
		  }
	  }
	  else
	  {
		  rv = CAN_BUSY;
	  }
  } else {
    rv = CAN_NOT_OK;
  }

  return rv;
}

void Can_MainFunction_Read( void ) {

	/* NOT SUPPORTED */
}

void Can_MainFunction_BusOff( void ) {
  /* Bus-off polling events */

	/* NOT SUPPORTED */
}

void Can_MainFunction_Wakeup( void ) {
  /* Wakeup polling events */

	/* NOT SUPPORTED */
}


void Can_MainFunction_Write( void ) {
    /* NOT SUPPORTED */
}

void Can_MainFunction_Error( void ) {
    /* NOT SUPPORTED */
}


/**
 * Get send/receive/error statistics for a controller
 *
 * @param controller The controller
 * @param stats Pointer to data to copy statistics to
 */

#if (USE_CAN_STATISTICS == STD_ON)
void Can_Arc_GetStatistics( uint8 controller, Can_Arc_StatisticsType *stats)
{
  Can_UnitType *canUnit = GET_PRIVATE_DATA(controller);
  *stats = canUnit->stats;
}
#endif
/*
 * Called by KSM to simulate CAN TX ISR and RX ISR
 */
void Can_SimulatorRunning(void)
{
	Can_UnitType *canUnit;
	const Can_ControllerConfigType *canHwConfig;
	uint8 ctlrId;
	if(Can_Global.initRun == CAN_READY)
	{
		/* Tx Confirmation Process*/
		for (int configId=0; configId < CAN_CTRL_CONFIG_CNT; configId++)
		{
			canHwConfig = GET_CONTROLLER_CONFIG(configId);
			ctlrId = canHwConfig->CanControllerId;

			canUnit = GET_PRIVATE_DATA(ctlrId);
			if(0xFFFF != canUnit->swPduHandle)
			{
				if(NULL != Can_Global.config->CanConfigSet->CanCallbacks->TxConfirmation)
				{
					Can_Global.config->CanConfigSet->CanCallbacks->TxConfirmation(canUnit->swPduHandle);
				}
				canUnit->swPduHandle = CAN_EMPTY_MESSAGE_BOX;
			}
		}

		/* Rx Process */
	}
}


