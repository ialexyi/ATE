#ifndef __NF_DriversManager_H__
#define __NF_DriversManager_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"
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

int 		DLLEXPORT	DRV_NoiseFigure_GetErrorTextMessage ( int Handle , int iError , char **pErrorMessage );

STD_ERROR   DLLEXPORT	DRV_NoiseFigure_Init ( char *pszDriverLocation , char *pszAddressString , int *pHandle );
STD_ERROR   DLLEXPORT	DRV_NoiseFigure_Close ( int *pHandle );
STD_ERROR   DLLEXPORT	DRV_NoiseFigure_Reset ( int Handle );

STD_ERROR   DLLEXPORT	DRV_NoiseFigure_ConfigInput ( int Handle , int iInput , int iMeasureType , int iAverage , double lfRefMeasureFreq );
STD_ERROR   DLLEXPORT	DRV_NoiseFigure_SetMeasureType ( int Handle , int iType );
STD_ERROR   DLLEXPORT	DRV_NoiseFigure_Config ( int Handle , int bSweepMode , int iStartFreqMhz , int iStopFreqMhz , int iStepFreqMhz );
STD_ERROR   DLLEXPORT	DRV_NoiseFigure_Converter ( int Handle , int bFixedIF_lo , int iIF_FreqMhz , int iLO_FreqMhz , int iSideBand );
STD_ERROR   DLLEXPORT	DRV_NoiseFigure_Calibration ( int Handle , int bMode );
STD_ERROR   DLLEXPORT	DRV_NoiseFigure_SetLossFactor ( int Handle , int bEnable , double lfInLoss , double lfOutLoss , double lfTempLoss );
STD_ERROR   DLLEXPORT	DRV_NoiseFigure_SweepMode ( int Handle , int iMode );
STD_ERROR   DLLEXPORT	DRV_NoiseFigure_GetFixedMeasure ( int Handle , int iInput , int iMeasureType , int *Frequency_MHz , double *vlfNoise_Figure , double *vlfWorstCase );
STD_ERROR   DLLEXPORT	DRV_NoiseFigure_GetPlotMeasure ( int Handle , int *Frequency_MHz , double *vlfNoise_Figure , double *vlfGain , int *viCount );
STD_ERROR   DLLEXPORT	DRV_NoiseFigure_Mode ( int Handle , int iPlotMode , int iPoints , double lfNoise_Max, double lfNoise_Min , double lfGain_Max, double lfGain_Min );


#ifdef __cplusplus
    }
#endif

#endif  /* ndef __NF_DriversManager_H__ */
