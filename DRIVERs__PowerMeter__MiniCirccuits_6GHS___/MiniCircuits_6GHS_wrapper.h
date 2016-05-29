#ifndef __MiniCircuits_6GHS_wrapper_H__
#define __MiniCircuits_6GHS_wrapper_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"

//==============================================================================
// Constants

#define		SUPPORTED_DEVICE_NAME	"PWR-SEN-6GHS"	
		
#define		GLOBAL_DELAY			0.2
		
//==============================================================================
// Types
		
typedef enum { PM_UNIT_WATT , PM_UNIT_MILI_WATT , PM_UNIT_SENTINEL }teUnits;		

//==============================================================================
// External variables

//==============================================================================
// Global functions

int		Delay_LastComplete( int handle );

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __MiniCircuits_6GHS_wrapper_H__ */
