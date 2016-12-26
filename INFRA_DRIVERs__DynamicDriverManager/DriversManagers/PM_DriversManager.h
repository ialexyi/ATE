#ifndef __PM_DriversManager_H__
#define __PM_DriversManager_H__

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
//                       PowerMeter_Set_Trigger_Source

#define				BUS_TRIGGER_SOURCE								1//PowerMeter Trigger sorce
#define				EXT_TRIGGER_SOURCE								2//PowerMeter Trigger sorce
#define				HOLD_TRIGGER_SOURCE								3//PowerMeter Trigger sorce
#define				IMM_TRIGGER_SOURCE								4//PowerMeter Trigger sorce
#define				INT_TRIGGER_SOURCE								5//PowerMeter Trigger sorce

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

int 		DLLEXPORT	DRV_PowerMeter_GetErrorTextMessage (  int Handle , int iError , char **pErrorMessage );
	
STD_ERROR   DLLEXPORT	DRV_PowerMeter_Init ( char *pszDriverLocation , char *pszAddressString , int *pHandle ); 
STD_ERROR   DLLEXPORT	DRV_PowerMeter_Close ( int *pHandle ); 
STD_ERROR   DLLEXPORT	DRV_PowerMeter_Reset ( int Handle );

STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetActiveChannel( int Handle , int iChannelNumber ); 

STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetActivePort( int Handle , int iSensorNumber );
STD_ERROR   DLLEXPORT	DRV_PowerMeter_EnableDualPortMode( int Handle , int bEnable );
	
STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetSingleSweep ( int Handle , int channel ); 
STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetContinuesSweep ( int Handle , int channel ); 
STD_ERROR   DLLEXPORT	DRV_PowerMeter_InitSweep ( int Handle , int channel ); 

STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetAuto ( int Handle , int channel ); 
STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetChanelState ( int Handle , int iChannel , int iState );

STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetVerticalScale ( int Handle , int iChannel , double lfScale );
STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetVerticalCenter ( int Handle , int iChannel , double lfCenter );

STD_ERROR   DLLEXPORT	DRV_PowerMeter_Calibrate ( int Handle , int iChannel );
STD_ERROR   DLLEXPORT	DRV_PowerMeter_Zero ( int Handle , int iChannel );

STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetUnits ( int Handle , int iChannel , unsigned int Units );
STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetOffset ( int Handle , int iChannel , double lfOffset );
STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetCalFactor ( int Handle , int iChannel , double lfCalFactor );
STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetVideoBW ( int Handle , int iChannel , unsigned int VideoBW );
STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetPeakHold ( int Handle , int iChannel , int iState );
STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetFrequency ( int Handle , int iChannel , double lfFrequency );
STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetMode ( int Handle , int iChannel , unsigned int Mode );

STD_ERROR   DLLEXPORT	DRV_PowerMeter_OPC ( int Handle , double lfTimeout , int *piState );

STD_ERROR   DLLEXPORT	DRV_PowerMeter_ReadCWpower( int Handle , int iChannel , double timeout , double *value );
STD_ERROR   DLLEXPORT	DRV_PowerMeter_FetchCWpower ( int Handle , int iChannel , double lfTimeout , double *value );

STD_ERROR   DLLEXPORT	DRV_PowerMeter_MeasureModulatedPower ( int Handle , int iChannel , double lfTimeout , double *value );
STD_ERROR   DLLEXPORT	DRV_PowerMeter_FetchMarkerAverage ( int Handle , int iChannel , int iMarker , double lfTimeout , double *value );
STD_ERROR   DLLEXPORT	DRV_PowerMeter_FetchMarkerMax ( int Handle , int iChannel , int iMarker , double lfTimeout , double *value );
STD_ERROR   DLLEXPORT	DRV_PowerMeter_FetchMarkerMin ( int Handle , int iChannel , int iMarker , double lfTimeout , double *value );

STD_ERROR   DLLEXPORT	DRV_PowerMeter_MeasurePower ( int Handle , int iChannel , double lfTimeout , double *value );
STD_ERROR   DLLEXPORT	DRV_PowerMeter_MeasurePowerPeakHold ( int Handle , int iChannel , double lfTimeout , double *value );

STD_ERROR   DLLEXPORT	DRV_PowerMeter_Configuration( int Handle , int iChannel , int bContinue , int bDelayAutoState , int iTriggerSource , double lfTrigHoldOff , double lfGateOffsetTime , double lfGateTime , double lfTraceTime );

STD_ERROR   DLLEXPORT	DRV_PowerMeter_RecallState( int Handle , int iStateNumber ); 

STD_ERROR   DLLEXPORT	DRV_PowerMeter_ReadPower ( int Handle , int iChannel , double lfTimeout , double *value );

STD_ERROR   DLLEXPORT	DRV_PowerMeter_FetchPower ( int Handle , int iChannel , double lfTimeout , double *value );


STD_ERROR   DLLEXPORT	DRV_PowerMeter_SaveState ( int Handle , int iStateNumber);
STD_ERROR   DLLEXPORT	DRV_PowerMeter_Gating_ConfigOffsetTime ( int Handle , int iChannel , int iGate , double lfTime , double lfOffset , double lfMidambleOffset, double lfMidambleLength );

STD_ERROR   DLLEXPORT	DRV_PowerMeter_Gating_ReadMaxPower ( int Handle , int iChannel , int iGait , double timeout , double *value );
STD_ERROR   DLLEXPORT	DRV_PowerMeter_Gating_ReadPower( int Handle , int iChannel , int iGait , int iGaitMeasureIndex , double timeout , double *value );

STD_ERROR   DLLEXPORT	DRV_PowerMeter_Gating_FetchMaxPower ( int Handle , int iChannel , int iGate , double timeout , double *value );
STD_ERROR   DLLEXPORT	DRV_PowerMeter_Gating_FetchAveragePower( int Handle , int iChannel , int iGate , double timeout , double *value );
STD_ERROR   DLLEXPORT	DRV_PowerMeter_Gating_FetchMinPower( int Handle , int iChannel , int iGate , double timeout , double *value );

STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetStateName ( int Handle , int iStateNumber , char *pszName );
STD_ERROR   DLLEXPORT	DRV_PowerMeter_GetStateName ( int Handle , int iStateNumber , char *pszName );
STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetStateFile ( int Handle , int iStateNumber , char *pszFileName );
STD_ERROR   DLLEXPORT	DRV_PowerMeter_GetStateFile ( int Handle , int iStateNumber , char *pszFileName );

STD_ERROR   DLLEXPORT	DRV_PowerMeter_RecallState_ByName ( int Handle , char *pszName ); 
STD_ERROR   DLLEXPORT	DRV_PowerMeter_GetLastStateFileName( int Handle ,char **pszFileName );

STD_ERROR   DLLEXPORT	DRV_PowerMeter_Configure_Marker( int Handle , int iChannel , int iMarkerNumber , int bEnable , double lfPosition );
STD_ERROR   DLLEXPORT	DRV_PowerMeter_Read_Marker( int Handle , int iChannel , int iMarkerNumber , double timeout , double *vlfPosition , double *vlfPower );
STD_ERROR   DLLEXPORT	DRV_PowerMeter_Fetch_Marker( int Handle , int iChannel , int iMarkerNumber , double *vlfPosition , double *vlfPower );

STD_ERROR   DLLEXPORT	DRV_PowerMeter_GetTrace( int Handle , int iChannel , double timeout, double **plfTime , double **plfTrace , int numberOfPoints , int *pCount );

STD_ERROR	DLLEXPORT	DRV_PowerMeter_Set_Trigger_Source( int hInstrumentHandle, int iTriggerSource );

STD_ERROR	DLLEXPORT	DRV_PowerMeter_Abort( int hInstrumentHandle, int iChannel);



#ifdef __cplusplus
    }
#endif

#endif  /* ndef __PM_DriversManager_H__ */
