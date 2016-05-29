#ifndef __MM_DriversManager_H__
#define __MM_DriversManager_H__

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
int	DLLEXPORT	DRIVER_MANAGER_FreeCallbacksStructure( void **pSTD_GetCallBackSet , int iNumberOfParams , ... );  
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

int 		DLLEXPORT	DRV_MultiMeter_GetErrorTextMessage ( int Handle , int iError , char **pErrorMessage );

STD_ERROR   DLLEXPORT	DRV_MultiMeter_Init ( char *pszDriverLocation , char *pszAddressString , int *pHandle );
STD_ERROR   DLLEXPORT	DRV_MultiMeter_Close ( int *pHandle );
STD_ERROR   DLLEXPORT	DRV_MultiMeter_Reset ( int Handle );

STD_ERROR   DLLEXPORT	DRV_MultiMeter_Measure ( int Handle , int iChannel , int iType , double dDelay , double *pdMeasurement );
STD_ERROR   DLLEXPORT	DRV_MultiMeter_Config ( int Handle , int iChannel , int iType , int iAverage , int iScale );
STD_ERROR   DLLEXPORT	DRV_MultiMeter_ClearStatus ( int Handle );


#ifdef __cplusplus
    }
#endif

#endif  /* ndef __MM_DriversManager_H__ */
