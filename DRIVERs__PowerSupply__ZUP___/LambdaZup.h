#ifndef __LambdaZup_H__
#define __LambdaZup_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include <visa.h>
#include "cvidef.h"
#include "LambdaZup_Types.h"
#include "GlobalDefines.h"

//==============================================================================
// Constants

#define		WR_BUF					64
#define		RD_BUF					128
		
//==============================================================================
// Types

/***************************************************/
typedef struct 
{
	double						lfMaxValue,
								lfMinValue,
								lfAccuracy;
	
	char						szProtect[64];
	
} tsLambdaChItem;

/***************************************************/

typedef struct 
{
	char						szChModelName[128];
	
	char						szChAddress[128];
	ViUInt32					iAddrBufLen;
	
	tsLambdaChItem				currentDefine,
								voltageDefine,
								overVoltageDefine,
								underVoltageDefine;

	char						szProtect[64];
	
} tsLambdaChannel;

/***************************************************/

typedef struct 
{
	ViSession 					InstrumentHandle;
	
	int							iChanelsQuantity;
	
	tsLambdaChannel				*pLamdaChannel;

	char						szProtect[64];
} tsLambdaHandle;

//==============================================================================
// External variables

//==============================================================================
// Global functions

errEV ZUP_GetErrorTextMessage	( int handle , int iError , char *pErrorMessage );
	
errEV ZUP_Init ( ViRsrc szRsrcAddress , int *pHandle , int *pChannelsQuantity );
errEV ZUP_Close ( int *pHandle );

errEV ZUP_ClearCommBufferAndRegisters ( int handle , int iPSAddress );

errEV ZUP_GetPowerSupplyModel ( int handle , int iPSAddress , char pReturnString[RD_BUF] );
errEV ZUP_GetPowerSupplyRevision ( int handle , int iPSAddress , char pReturnString[RD_BUF] );

errEV ZUP_SetOutputVoltage ( int handle , int iPSAddress , double lfVolts );
errEV ZUP_GetOutputVoltageDefined ( int handle , int iPSAddress , double *plfVolts );
errEV ZUP_GetOutputVoltageActual ( int handle , int iPSAddress , double *plfVolts );
	
errEV ZUP_SetOutputCurrent ( int handle , int iPSAddress , double lfAmpers );
errEV ZUP_GetOutputCurrentDefined ( int handle , int iPSAddress , double *plfAmpers );
errEV ZUP_GetOutputCurrentActual ( int handle , int iPSAddress , double *plfAmpers );

errEV ZUP_SetVoltageProtectionOver ( int handle , int iPSAddress , double lfVolts );
errEV ZUP_GetVoltageProtectionOver ( int handle , int iPSAddress , double *plfVolts );
	
errEV ZUP_SetVoltageProtectionUnder ( int handle , int iPSAddress , double lfVolts );
errEV ZUP_GetVoltageProtectionUnder ( int handle , int iPSAddress , double *plfVolts );

errEV ZUP_SetOutputState ( int handle , int iPSAddress , int iState );
errEV ZUP_GetOutputState ( int handle , int iPSAddress , int *piState );

errEV ZUP_SetFoldbackProtectionMode ( int handle , int iPSAddress , int iMode );
errEV ZUP_GetFoldbackProtectionMode ( int handle , int iPSAddress , int *piMode );

errEV ZUP_SetAutoRestartMode ( int handle , int iPSAddress , int iMode );
errEV ZUP_GetAutoRestartMode ( int handle , int iPSAddress , int *piMode );

errEV ZUP_GetOperationalStatusRegister ( int handle , int iPSAddress , char *pReturnString );
errEV ZUP_GetAlarmStatusRegister ( int handle , int iPSAddress , char *pReturnString );
errEV ZUP_GetErrorCodesRegister ( int handle , int iPSAddress , char *pReturnString );

errEV ZUP_GetCompleteStatus ( int handle , int iPSAddress , char *pReturnString );

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __LambdaZup_H__ */
