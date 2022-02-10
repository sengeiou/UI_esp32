#ifndef NR_OCS_DBG_H_
#define NR_OCS_DBG_H_

#include "types.h"

typedef enum
{
	FSM_STATE_INIT          = 0x00,     /* Machine Initialization */
	FSM_STATE_STANDBY       = 0x01,
	FSM_STATE_HEATING       = 0x02,
	FSM_STATE_READY_TO_BREW = 0x03,
	FSM_STATE_DISCHARGE     = 0x04,		/* Pressure Discharge */
	FSM_STATE_PRE_BREWING   = 0x05,
	FSM_STATE_PRE_INFUSION  = 0x06,
	FSM_STATE_BREWING       = 0x07,
	FSM_STATE_STEAMING      = 0x08,
	FSM_STATE_FAULT         = 0x09,
	FSM_STATE_FACTORY_RESET = 0x0A,
	FSM_STATE_CLEANING      = 0x0B,
	FSM_STATE_WATER_EMPTY   = 0x0C,
	FSM_STATE_POD_FULL      = 0x0D,

	FSM_MAX_STATE
} FSM_StateID_t;

#endif // NR_OCS_DBG_H_