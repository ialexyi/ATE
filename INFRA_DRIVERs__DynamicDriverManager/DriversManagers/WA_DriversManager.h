#ifndef __WA_DriversManager_H__
#define __WA_DriversManager_H__

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
int	DLLEXPORT	DRIVER_MANAGER_SETUP_UpdateCallbacksStructure( void *pSTD_CallBackSet  , int iNumberOfParams , ... );
int	DLLEXPORT	DRIVER_MANAGER_FreeCallbacksStructure( void **pSTD_GetCallBackSet , int iNumberOfParams , ...  );  
int	DLLEXPORT	DRIVER_MANAGER_Get_Number_Of_Channels( int Handle , int *piNumberOfChannels );
int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_Port_Number( int Handle , int iCalibrationPortNumber );
int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_Power( int Handle , double lfSetPower ) ;
int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_MeasureDelay( int Handle , double lfMeasureDelay );
int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_StateFileName( int Handle , char *pStateFileName );
int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_ReferenceLevel( int Handle , double lfReferenceLevel );
int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_Attenuator( int Handle , double lfAttenuator );

STD_ERROR   DLLEXPORT	DRIVER_MANAGER_Equipment_BrowseSelectStateFiles ( int hHandle , char *pStoreFilePath , int *piNumberOfSelectedFiles , char ***pvszSelectedFileNamesList , int **pvszSelectedFileSizesList );

STD_ERROR   DLLEXPORT	DRV_Config_SetAttribute( int hHandle , char *pAttributeName , char *pAttributeValue );
//==============================================================================
// Global functions

int 		DLLEXPORT	DRV_WaveAnalyzer_GetErrorTextMessage ( int Handle , int iError , char **pErrorMessage );

STD_ERROR   DLLEXPORT	DRV_WaveAnalyzer_Init ( char *pszDriverLocation , char *pszAddressString , int *pHandle );		
STD_ERROR   DLLEXPORT	DRV_WaveAnalyzer_Close ( int *pHandle );		
STD_ERROR   DLLEXPORT	DRV_WaveAnalyzer_Reset ( int Handle );

STD_ERROR   DLLEXPORT	DRV_WaveAnalyzer_Config ( int Handle , int iReciverType , double lfBandWidth , int bAmpifire , int bAgcAuto , double lfAgcLevel , int iAverage , int bMeasureType );

STD_ERROR   DLLEXPORT	DRV_WaveAnalyzer_GetModulationMeasure ( int Handle , int iModulation , double *vlfAudioFreq , double *vlfAudioAmpliduteMV , double *vlfModRiple , double *vlfModDiv , double *vlfNegPeak , double *vlfRMS , double *vlfModuleAudioFreq );

STD_ERROR   DLLEXPORT	DRV_WaveAnalyzer_SetAudioUnits( int Handle , int iUnits );
STD_ERROR   DLLEXPORT	DRV_WaveAnalyzer_SetFilter( int Handle , int iAufioType , int iFilterType , int iPsophType );
	
 
	
#ifdef __cplusplus
    }
#endif

#endif  /* ndef __WA_DriversManager_H__ */
