#ifndef __STD_DriversManager_H__
#define __STD_DriversManager_H__

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
//typedef void* (*tfStdReceiveCallback) ( void *pHandle , char *pCommand , int *pValue , int iValueSize , char *pReceiveBuffer , int iBufferSize );
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
int	DLLEXPORT	DRIVER_MANAGER_Get_SimulationMode( int Handle , int *piSimulationMode );

STD_ERROR   DLLEXPORT	DRIVER_MANAGER_GetEquipmentInformation( int hHandle , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware );

STD_ERROR   DLLEXPORT	DRIVER_MANAGER_Equipment_BrowseSelectStateFiles ( int hHandle , char *pStoreFilePath , int *piNumberOfSelectedFiles , char ***pvszSelectedFileNamesList , int **pvszSelectedFileSizesList );

STD_ERROR   DLLEXPORT	DRV_Config_SetAttribute( int hHandle , char *pAttributeName , char *pAttributeValue );
//==============================================================================
// Global functions
int			DLLEXPORT	DRV_StandardDevice_GetErrorTextMessage( int handle , int iError , char **pErrorMessage );

STD_ERROR   DLLEXPORT	DRV_StandardDevice_GetIdentificationNumber( int Handle , unsigned __int64 *pullIDNumber );

STD_ERROR   DLLEXPORT 	DRV_StandardDevice_EnableLogPrinting ( int Handle , int bAllow );  

STD_ERROR   DLLEXPORT 	DRV_StandardDevice_GetLock ( int Handle );
STD_ERROR   DLLEXPORT 	DRV_StandardDevice_TryToGetLock ( int Handle , int *pSuccess);
STD_ERROR   DLLEXPORT 	DRV_StandardDevice_ReleaseLock ( int Handle );
STD_ERROR   DLLEXPORT 	DRV_StandardDevice_LockStatus ( int Handle , int *iStatus );

STD_ERROR   DLLEXPORT	DRV_StandardDevice_Init( char *pszDriverLocation , int iDeviceID , char *pszConnectionName , char *pszAddress , int *pHandle , ... );
STD_ERROR   DLLEXPORT	DRV_StandardDevice_Close( int *pHandle );
STD_ERROR   DLLEXPORT	DRV_StandardDevice_Reset( int hHandle );

STD_ERROR   DLLEXPORT	DRV_StandardDevice_ResetDriver( int hHandle );

STD_ERROR   DLLEXPORT	DRV_StandardDevice_GetConnectionAddress( int hHandle , char **pszAddress );

STD_ERROR   DLLEXPORT	DRV_StandardDevice_SetValue( int hHandle , char *pCommandName , void *pValue , int iValueLength , int  iValueSize, ...  );
STD_ERROR   DLLEXPORT	DRV_StandardDevice_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize , ...  ); 
STD_ERROR   DLLEXPORT	DRV_StandardDevice_GetValue_Ex( int Handle , char *pCommandName , void **pGetValue , int *piValueLength , int *piValueSize , ...  );
STD_ERROR   DLLEXPORT	DRV_StandardDevice_SetListOfValues( int Handle , char *pCommandsList , int bCheckResponds , int iNumberOfRetries );

STD_ERROR   DLLEXPORT   DRV_StandardDevice_Check_Connection( int hHandle , char *pCommandName , int *piStatus );

STD_ERROR   DLLEXPORT	DRV_StandardDevice_Unplug( int hHandle , double lfTimeOut );
STD_ERROR   DLLEXPORT	DRV_StandardDevice_PlugIn( int hHandle , double lfTimeOut );

STD_ERROR   DLLEXPORT	DRV_StandardDevice_EnablePlugInAutomatically( int hHandle , int bEnable );
STD_ERROR   DLLEXPORT	DRV_StandardDevice_InstallReceiveCallbackFunction( int hHandle , void *pfFunction , void *pData );
	
STD_ERROR   DLLEXPORT	DRV_StandardDevice_LoadConfigFile( int hHandle , char *pFileName );
STD_ERROR   DLLEXPORT	DRV_StandardDevice_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize );

STD_ERROR   DLLEXPORT   DRV_StandardDevice_Get_Commands_List( int Handle , char ***pCommandsList , int *piNumberOfCommands );

STD_ERROR   DLLEXPORT	DRV_StandardDevice_GetDeviceInfoBlock( int Handle , char **pBuffer , int *piBufferSize );

STD_ERROR   DLLEXPORT	DRV_StandardDevice_Save_Log_File_As( int Handle , char *pFileNamePath );

STD_ERROR   DLLEXPORT	DRV_StandardDevice_GetLowLevelHandle( int Handle , int *pInstrumentLowLevelHandle );

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __STD_DriversManager_H__ */
