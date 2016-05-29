#ifndef __SC_DriversManager_H__
#define __SC_DriversManager_H__

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

int 		DLLEXPORT	DRV_Oscilloscope_GetErrorTextMessage ( int Handle , int iError , char **pErrorMessage );

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_Init ( char *pszDriverLocation , char *pszAddressString , int *pHandle );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_Close ( int *pHandle );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_Reset ( int Handle );

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_AutoSetup ( int Handle );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_ChannelOnOff ( int Handle , int iChannel , int OnOff);
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_ChannelVertical ( int Handle , int iChannel, double dScale , double dOffset , int iUnits , int iCouping , double lfProbeAttenuation , double lfImpedance );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_Horizontal ( int Handle , double dScale , double dOffset );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_SweepContinuous ( int Handle , int iState);
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_InitTrigger ( int Handle );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_WaitForEvent ( int Handle , double lfTimeOut );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_TriggerConfig ( int Handle , int iChannel , double lfTrigerLevel , int bPositiveEdge , double lfHoldTime , int iModeAuto );

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_GetWaveForm ( int Handle , int iChannel, int iSize, double lfTimeOut, double *dpMeasureArray, int *ipCount, double *dpX_init, double *dpX_inc );

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureRiseTime ( int Handle , int iChannel, double lfTimeOut, double *dpValue );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureFallTime ( int Handle , int iChannel, double lfTimeOut, double *dpValue );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureFrequency ( int Handle , int iChannel, double lfTimeOut, double *dpValue );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasurePeriod ( int Handle , int iChannel, double lfTimeOut, double *dpValue );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureVoltageRMS( int Handle , int iChannel, double lfTimeOut, double *dpValue );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureVoltagePk2Pk ( int Handle , int iChannel, double lfTimeOut, double *dpValue );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureVoltageMax ( int Handle , int iChannel, double lfTimeOut, double *dpValue );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureVoltageMin ( int Handle , int iChannel, double lfTimeOut, double *dpValue );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureVoltageHigh ( int Handle , int iChannel, double lfTimeOut, double *dpValue );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureVoltageLow ( int Handle , int iChannel, double lfTimeOut, double *dpValue );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureVoltageAverage ( int Handle , int iChannel, double lfTimeOut, double *dpValue );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureWidthNegative ( int Handle , int iChannel, double lfTimeOut, double *dpValue );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureWidthPositive ( int Handle , int iChannel, double lfTimeOut, double *dpValue );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureDutyCycleNegative ( int Handle , int iChannel, double lfTimeOut, double *dpValue );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureDutyCyclePositive ( int Handle , int iChannel, double lfTimeOut, double *dpValue );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureAmplitude ( int Handle , int iChannel, double lfTimeOut, double *dpValue );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureVoltageCycleRMS ( int Handle , int iChannel, double lfTimeOut, double *dpValue );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureVoltageCycleAverage ( int Handle , int iChannel, double lfTimeOut, double *dpValue );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasurePositiveOvershoot ( int Handle , int iChannel, double lfTimeOut, double *dpValue );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureNegativeOvershoot ( int Handle , int iChannel, double lfTimeOut, double *dpValue );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureTimingBurstWidth ( int Handle , int iChannel, double lfTimeOut, double *dpValue );

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MemoryState_Save( int Handle , char *pszFileName );
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MemoryState_Recall( int Handle , char *pszFileName );
	
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_HorizontalOffset( int Handle , double lfOffset );

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureDelay ( int Handle , int iChannel , double lfTimeOut, double *dpValue );  
STD_ERROR   DLLEXPORT	DRV_Oscilloscope_ConfigMeasureDelay ( int Handle , int iSourceChannel , int iSourceSlope , int iSourceSpare, int iMeasureChannel , int iMeasureSlope , int iMeasureSpare );

	
#ifdef __cplusplus
    }
#endif

#endif  /* ndef __SC_DriversManager_H__ */
