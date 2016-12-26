#ifndef __SG_DriversManager_H__
#define __SG_DriversManager_H__

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


STD_ERROR DLLEXPORT DRIVER_MANAGER_CALIBRATION_FillCalibrationRanges( int Handle , char *pRangesString ); // sample "1E9,2E9-8E9:50"
STD_ERROR DLLEXPORT DRIVER_MANAGER_CALIBRATION_FillCalibrationInfo(int Handle,double *plfFrequency,double *plfLoss,int iNumberOfPoints,double lfStartFrequnecy,double lfStopFrequency);
STD_ERROR DLLEXPORT DRIVER_MANAGER_CALIBRATION_GetCalibrationInfo(int Handle,double **plfFrequency,double **plfLoss,int *piNumberOfPoints,double *plfStartFrequnecy,double *plfStopFrequency);
STD_ERROR DLLEXPORT DRIVER_MANAGER_CALIBRATION_EnableUseCalibration(int Handle,int bAllow);
STD_ERROR DLLEXPORT DRIVER_MANAGER_CALIBRATION_FillStoreInformation(int Handle,char *pStoreName,int clbConfigType,int clbLinkID);
STD_ERROR DLLEXPORT DRIVER_MANAGER_CALIBRATION_GetStoreInformation(int Handle,char **pStoreName,int *pclbConfigType,int *pclbLinkID);
STD_ERROR DLLEXPORT DRIVER_MANAGER_CALIBRATION_FillMessageInfo(int Handle,char *pConnectionDescription,char *pConnectionImage);
STD_ERROR DLLEXPORT DRIVER_MANAGER_CALIBRATION_GelFactor(int Handle,double lfFrequency,double *plfFactor);
STD_ERROR DLLEXPORT DRIVER_MANAGER_CALIBRATION_Run(int StimulusHandle,int MeasureHandle, int StoreHandle , int iProgressPanel , int iProgressControl );

STD_ERROR DLLEXPORT DRIVER_MANAGER_CALIBRATION_Fill_AdditionalPower_CalibrationInfo( int Handle , double lfPower , double *plfLoss , int iNumberOfPoints );
STD_ERROR DLLEXPORT	DRIVER_MANAGER_CALIBRATION_Get_AdditionalPower_CalibrationInfo( int Handle , int iPowerIndex , double **plfLoss , int *piNumberOfPoints , double *plfPower );
STD_ERROR DLLEXPORT	DRIVER_MANAGER_CALIBRATION_Set_NumberOf_AdditionalPower( int Handle , double *plfPowerList , int iNumberOfPowers );

STD_ERROR DLLEXPORT DRIVER_MANAGER_CALIBRATION_Gel_3D_Factor( int Handle , double lfPower , double lfFrequency , double *plfFactor );
STD_ERROR DLLEXPORT DRIVER_MANAGER_CALIBRATION_GetCalibrationRangeInfo( int Handle , double *plfStart , double *plfStop , int *piNumberOfPoints ); 

STD_ERROR   DLLEXPORT	DRIVER_MANAGER_GetEquipmentInformation( int hHandle , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware );

STD_ERROR   DLLEXPORT	DRIVER_MANAGER_Equipment_BrowseSelectStateFiles ( int hHandle , char *pStoreFilePath , int *piNumberOfSelectedFiles , char ***pvszSelectedFileNamesList , int **pvszSelectedFileSizesList );

STD_ERROR   DLLEXPORT	DRV_Config_SetAttribute( int hHandle , char *pAttributeName , char *pAttributeValue );
//==============================================================================
// Global functions

int 		DLLEXPORT	DRV_SignalGenerator_GetErrorTextMessage ( int Handle , int iError , char **pErrorMessage );

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_Init ( char *pszDriverLocation , char *pszAddressString , int *pHandle );
STD_ERROR   DLLEXPORT	DRV_SignalGenerator_Close ( int *pHandle );
STD_ERROR   DLLEXPORT	DRV_SignalGenerator_Reset ( int Handle );

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_RF_Off ( int Handle , int Channel );
STD_ERROR   DLLEXPORT	DRV_SignalGenerator_RF_On ( int Handle , int Channel );

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_Modulation_Off ( int Handle , int Channel );
STD_ERROR   DLLEXPORT	DRV_SignalGenerator_Modulation_On ( int Handle , int Channel );

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_Config ( int Handle , int Channel , double dFrequency , double dAmplitude );

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_SetFrequency ( int Handle , int Channel , double dFrequency );
STD_ERROR   DLLEXPORT	DRV_SignalGenerator_SetPower ( int Handle , int Channel , double dPower );
STD_ERROR   DLLEXPORT	DRV_SignalGenerator_SetModulation ( int Handle , int Channel , int iSource , double lfModulationIndex , double lfFrequency );
STD_ERROR   DLLEXPORT	DRV_SignalGenerator_SetModulationType ( int Handle , int Channel , int iModulationType );
STD_ERROR   DLLEXPORT	DRV_SignalGenerator_SetRfOutPort ( int Handle , int Channel , int iPort );
STD_ERROR   DLLEXPORT	DRV_SignalGenerator_SetFunctionGen ( int Handle , int Channel , int iFunctionNum , int bState , double lfRmsLevel , int bUnitVolt , int bWaveFormSine , double lfFrequency );
STD_ERROR   DLLEXPORT	DRV_SignalGenerator_SetInOut ( int Handle , int Channel , int iInput , int bInImpedance , int iOutput );
	
STD_ERROR   DLLEXPORT	DRV_SignalGenerator_DisableModulation ( int Handle , int Channel , int iSource  );
STD_ERROR   DLLEXPORT	DRV_SignalGenerator_DisableFunctionGen ( int Handle , int Channel , int iFunctionNum  );
STD_ERROR   DLLEXPORT	DRV_SignalGenerator_SetOffSetAttenuation ( int Handle , int Channel , double lfAttenuation  );

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_GetLastStateFileName( int Handle ,char **pszFileName );
STD_ERROR   DLLEXPORT	DRV_SignalGenerator_RecallState( int Handle , char *szFileName );

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_ApplyPowerCorrection_ByLoss ( int Handle , int Channel , double lfOutputPower , double lfAdditionalLoss );

STD_ERROR   DLLEXPORT 	DRV_SignalGenerator_GetLock ( int Handle );
STD_ERROR   DLLEXPORT 	DRV_SignalGenerator_TryToGetLock ( int Handle , int *pSuccess);
STD_ERROR   DLLEXPORT 	DRV_SignalGenerator_ReleaseLock ( int Handle );
STD_ERROR   DLLEXPORT 	DRV_SignalGenerator_LockStatus ( int Handle , int *iStatus );

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __SG_DriversManager_H__ */
