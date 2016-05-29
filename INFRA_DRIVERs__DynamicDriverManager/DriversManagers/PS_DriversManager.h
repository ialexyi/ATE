#ifndef __PS_DriversManager_H__
#define __PS_DriversManager_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files
		
#include <windows.h>
#include <visa.h>
		
#include "cvidef.h"

#include "GlobalDefines.h" 
		
//==============================================================================
// Constants

//==============================================================================
// Types

		
//==============================================================================
// External variables
int	DLLEXPORT	DRIVER_MANAGER_SETUP_UpdateCallbacksStructure( void *pSTD_CallBackSet , int iNumberOfParams , ... );
int	DLLEXPORT	DRIVER_MANAGER_FreeCallbacksStructure( void **pSTD_GetCallBackSet , int iNumberOfParams , ...  );  
int	DLLEXPORT	DRIVER_MANAGER_Get_Number_Of_Channels( int Handle , int *piNumberOfChannels );
int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_Port_Number( int Handle , int iCalibrationPortNumber );
int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_Power( int Handle , double lfSetPower ) ;
int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_MeasureDelay( int Handle , double lfMeasureDelay );
int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_StateFileName( int Handle , char *pStateFileName );
int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_ReferenceLevel( int Handle , double lfReferenceLevel );
int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_Attenuator( int Handle , double lfAttenuator );

STD_ERROR   DLLEXPORT	DRIVER_MANAGER_GetEquipmentInformation( int hHandle , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware );

STD_ERROR   DLLEXPORT	DRIVER_MANAGER_Equipment_BrowseSelectStateFiles ( int hHandle , char *pStoreFilePath , int *piNumberOfSelectedFiles , char ***pvszSelectedFileNamesList , int **pvszSelectedFileSizesList );

STD_ERROR   DLLEXPORT	DRV_Config_SetAttribute( int hHandle , char *pAttributeName , char *pAttributeValue );
//==============================================================================
// Global functions

int 		DLLEXPORT	DRV_PowerSupply_GetErrorTextMessage ( int Handle , int iError , char **pErrorMessage );
	
STD_ERROR   DLLEXPORT	DRV_PowerSupply_Init( char *pszDriverLocation , char *pszAddressString , int *pHandle ); 
STD_ERROR   DLLEXPORT	DRV_PowerSupply_Close( int *pHandle ); 
STD_ERROR   DLLEXPORT	DRV_PowerSupply_Reset( int Handle ); 

STD_ERROR   DLLEXPORT 	DRV_PowerSupply_GetLock ( int Handle );   
STD_ERROR   DLLEXPORT 	DRV_PowerSupply_TryToGetLock ( int Handle , int *pSuccess); 
STD_ERROR   DLLEXPORT 	DRV_PowerSupply_ReleaseLock ( int Handle ); 
STD_ERROR   DLLEXPORT 	DRV_PowerSupply_LockStatus ( int Handle , int *iStatus );  

STD_ERROR   DLLEXPORT	DRV_PowerSupply_ClearCommBufferAndRegisters ( int Handle , int iChannel );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetModel ( int Handle , int iChannel , char *pReturnString );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetRevision ( int Handle , int iChannel , char *pReturnString );

STD_ERROR   DLLEXPORT	DRV_PowerSupply_SetOutputVoltage ( int Handle , int iChannel , double lfVoltage );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetOutputVoltageDefined ( int Handle , int iChannel , double *plfVoltage );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetOutputVoltageActual ( int Handle , int iChannel , double *plfVoltage );

STD_ERROR   DLLEXPORT	DRV_PowerSupply_SetOutputCurrent ( int Handle , int iChannel , double lfCurrent );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetOutputCurrentDefined ( int Handle , int iChannel , double *plfCurrent );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetOutputCurrentActual ( int Handle , int iChannel , double *plfCurrent );

STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetOutputPowerActual ( int Handle , int iChannel , double *plfPower );

STD_ERROR   DLLEXPORT	DRV_PowerSupply_SetVoltageProtectionOver ( int Handle , int iChannel , double lfVoltage );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetVoltageProtectionOver ( int Handle , int iChannel , double *plfVoltage );

STD_ERROR   DLLEXPORT	DRV_PowerSupply_SetVoltageProtectionUnder ( int Handle , int iChannel , double lfVoltage );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetVoltageProtectionUnder ( int Handle , int iChannel , double *plfVoltage );

STD_ERROR   DLLEXPORT	DRV_PowerSupply_SetOutputState( int Handle , int iChannel , int iState );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetOutputState( int Handle , int iChannel , int *piState );

STD_ERROR   DLLEXPORT	DRV_PowerSupply_SetFoldbackProtectionMode ( int Handle , int iChannel , int iMode );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetFoldbackProtectionMode ( int Handle , int iChannel , int *piMode );

STD_ERROR   DLLEXPORT	DRV_PowerSupply_SetAutoRestartMode ( int Handle , int iChannel , int iMode );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetAutoRestartMode ( int Handle , int iChannel , int *piMode );

STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetOperationalStatusRegister ( int Handle , int iChannel , char *pReturnString );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetAlarmStatusRegister ( int Handle , int iChannel , char *pReturnString );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetErrorCodesRegister ( int Handle , int iChannel , char *pReturnString );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetCompleteStatus ( int Handle , int iChannel , char *pReturnString );

STD_ERROR   DLLEXPORT	DRV_PowerSupply_WaitForOperationComplete ( int Handle , double lfTimeout );

STD_ERROR   DLLEXPORT	DRV_PowerSupply_SetRange( int Handle , int iChannel , int iRange );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetRange( int Handle , int iChannel , int *piRange );

STD_ERROR   DLLEXPORT	DRV_PowerSupply_SetTrackingMode( int Handle , int channel , int iMode );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetTrackingMode( int Handle , int channel , int *piMode );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_SetCurrentRange( int Handle , int channel , double lfCurrent );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetCurrentRange( int Handle , int channel , double *plfCurrent );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_SetRemoteSense( int Handle , int channel , int iState );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetRemoteSense( int Handle , int channel , int *piState );

//--------------------------------- AC Features --------------------------------------//
STD_ERROR   DLLEXPORT	DRV_PowerSupply_SetRangeAuto ( int Handle );   
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetOutputPowerApparent ( int Handle , int channel , double *plfPower );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetOutputPowerFactor( int Handle , int channel , double *plfPowerFactor );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_SetVoltageRange ( int Handle , int channel , double lfVoltage );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetVoltageRange ( int Handle , int channel , double *plfVoltage );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_SetFrequency ( int Handle , int channel , double lfFrequency );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetFrequency ( int Handle , int channel , double *plfFrequency );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_SetOutputStarState ( int Handle , int channel , int iState );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetOutputStarState ( int Handle , int channel , int *piState );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_SetStarPhase ( int Handle , int channel , double lfPhase );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetStarPhase ( int Handle , int channel , double *plfPhase );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_SetFrequencyProtectionOver ( int Handle , int channel , double lfFrequency );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetFrequencyProtectionOver ( int Handle , int channel , double *plfFrequency );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_SetFrequencyProtectionUnder ( int Handle , int channel , double lfFrequency );
STD_ERROR   DLLEXPORT	DRV_PowerSupply_GetFrequencyProtectionUnder ( int Handle , int channel , double *plfFrequency );
		  
#ifdef __cplusplus
    }
#endif

#endif  /* ndef __PS_DriversManager_H__ */
