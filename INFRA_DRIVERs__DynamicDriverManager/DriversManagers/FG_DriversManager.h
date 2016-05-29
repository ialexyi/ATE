#ifndef __FG_DriversManager_H__
#define __FG_DriversManager_H__

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

int 		DLLEXPORT	DRV_FunctionGenerator_GetErrorTextMessage ( int Handle , int iError , char **pErrorMessage );

STD_ERROR   DLLEXPORT	DRV_FunctionGenerator_Init ( char *pszDriverLocation , char *pszAddressString , int *pHandle );
STD_ERROR   DLLEXPORT	DRV_FunctionGenerator_Close ( int *pHandle );
STD_ERROR   DLLEXPORT	DRV_FunctionGenerator_Reset ( int Handle );

STD_ERROR   DLLEXPORT	DRV_FunctionGenerator_Initiate ( int Handle );
	
STD_ERROR   DLLEXPORT	DRV_FunctionGenerator_Signel_Off ( int Handle , int Channel );
STD_ERROR   DLLEXPORT	DRV_FunctionGenerator_Signel_On ( int Handle , int Channel );

STD_ERROR   DLLEXPORT	DRV_FunctionGenerator_Save_State ( int InstrumentHandle , int iStateNumber , char *pszFileName );
STD_ERROR   DLLEXPORT	DRV_FunctionGenerator_Load_State ( int InstrumentHandle , int iStateNumber , char *pszFileName );

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __FG_DriversManager_H__ */
