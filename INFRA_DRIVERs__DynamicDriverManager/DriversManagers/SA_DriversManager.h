#ifndef __SA_DriversManager_H__
#define __SA_DriversManager_H__

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
typedef enum { NEXT_R_HIGH_PEAK_MARKER , NEXT_L_HIGH_PEAK_MARKER , HIGH_PEAK_MARKER , MINIMUM_MARKER , TIME_MARKER , NORMAL_MARKER , CENTER_MARKER , DELTA_MARKER , FM_MOD_MARKER , MARKER_SEARCH_NEXT_PEAK , MARKER_SEARCH_NEXT_MINIMUM , MARKER_SEARCH_NEXT_MINIMUM_LEFT , MARKER_SEARCH_NEXT_MINIMUM_RIGHT , MARKER_SENTINEL } teMARKER;
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

STD_ERROR   DLLEXPORT	DRIVER_MANAGER_GetEquipmentInformation( int hHandle , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware );

STD_ERROR   DLLEXPORT	DRIVER_MANAGER_Equipment_BrowseSelectStateFiles ( int hHandle , char *pStoreFilePath , int *piNumberOfSelectedFiles , char ***pvszSelectedFileNamesList , int **pvszSelectedFileSizesList );

STD_ERROR   DLLEXPORT	DRV_Config_SetAttribute( int hHandle , char *pAttributeName , char *pAttributeValue );
//==============================================================================
// Global functions

int 		DLLEXPORT	DRV_SpectrumAnalyzer_GetErrorTextMessage ( int Handle , int iError , char **pErrorMessage );

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Init ( char *pszDriverLocation , char *pszAddressString , int *pHandle );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Close ( int *pHandle );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Reset ( int Handle );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Preset ( int Handle );

STD_ERROR   DLLEXPORT 	DRV_SpectrumAnalyzer_GetLock ( int Handle );   
STD_ERROR   DLLEXPORT 	DRV_SpectrumAnalyzer_TryToGetLock ( int Handle , int *pSuccess); 
STD_ERROR   DLLEXPORT 	DRV_SpectrumAnalyzer_ReleaseLock ( int Handle ); 
STD_ERROR   DLLEXPORT 	DRV_SpectrumAnalyzer_LockStatus ( int Handle , int *iStatus );  

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetFrequencyCenterSpan ( int Handle , int iChannel , double lfFrequency , double lfSpan );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_ConfigureMarker ( int Handle , int iChannel , int marker , int bState );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetMarkerSearchPeakAuto ( int Handle , int iChannel , int bMax , int bState );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetMarkerMeasure ( int Handle , int iChannel , int marker , double *pFrequency , double *pPower );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetTrace ( int Handle , int iTimeOut , double **pTrace , int numberOfPoints , int *pCount );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetMaxHold ( int Handle , int  bState );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_ConfigChannelPowerMeasuring ( int Handle , double lfBandWidth );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetChannelPowerResult ( int Handle , double *plfValue );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetStartStopFrequency ( int Handle , double *plfStartFrequency , double *plfStopFrequency );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_ConfigBandWidth( int Handle , double lfResolutionBandWidth , double lfVideoBandWidth );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_ConfigSource( int Handle , int bExternal );  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetNumberOfPoints( int Handle , int points ); 

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetScaleDev( int Handle , double lfScaleDev );  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Attenuator( int Handle , double lfAttenuator );  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetAttenuator( int Handle , double *plfAttenuator );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetReferenceLevel( int Handle , double lfReferenceLevel );  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetReferenceLevel( int Handle , double *vlfReferenceLevel );  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetMarkerType( int Handle , int bDelta );  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetMarkerFrequency( int Handle , int iMarkerNr , double lfFrequency );  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetMarkerFrequency( int Handle , int iMarkerNr , double *vlfFrequency );  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetMarkerAmplitude( int Handle , int iMarkerNr , double *vlfAmplitude );  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetMarkerDeltaFrequency( int Handle , double *vlfFrequency );  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetMarkerDeltaAmplitude( int Handle , double *vlfAmplitude );  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetCenterFrequencyFromMarker( int Handle , int marker );  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetReferenceLevelFrequencyFromMarker( int Handle , int marker );  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_WaitForIntrumentDone( int Handle , double lfTimeOut );  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetSweepMode( int Handle , int bContinues );  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_InitiateMeasure( int Handle );  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetAverage( int Handle , int iType  , int iAverage );  

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SearchMarkerPeak( int Handle , int iMarkerNr );  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SearchMarkerNextPeak( int Handle , int iMarkerNr );    

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetAuto ( int Handle , int state );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Config ( int Handle , int Mode , int Type , int Input , int bBeeperState );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetFrequency ( int Handle , double dCenterFreq );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetStartStopFrequency ( int Handle , double dStartFreq , double dStopFreq );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetStartStopFrequency ( int Handle , double *pdStartFreq , double *pdStopFreq );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetSpan ( int Handle , double dSpan );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetAmplitude ( int Handle , double dScale , double dRefLevel );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetAttenuation ( int Handle , double dAttenuation );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetRBW ( int Handle , double dRBW );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetVBW ( int Handle , double dVBW );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetBW ( int Handle , double dRBW, double dVBW );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetSweep ( int Handle , double lfTimeout );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetMarker ( int Handle , int MarkerSetType , int iMarkerNr ,double dMarkerFreq );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetRLAuto ( int Handle );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetFrequency ( int Handle , double *pdFrequency );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetAmplitude ( int Handle , int MarkerSetType , int iMarkerNr , double *pdAmplitude );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetFreqCountResol ( int Handle , BOOL bMode, int iFreqCountResolution );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetAvg ( int Handle , BOOL bMode, int iAvg );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetMaxHold ( int Handle , int iState );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetMarkerLevel ( int Handle , int iMarkerNr , double *pLevel );

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_AutoSetIsDone ( int Handle , double lfTimeOut , int *bDone );
STD_ERROR   DLLEXPORT 	DRV_SpectrumAnalyzer_SetAutoTuneThreshold( int Handle, double lfValue );  

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetTrace ( int Handle , int timeout , double **pTrace , int numberOfPoints , int *pCount );

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Recall ( int Handle , char *szFilePath  );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetMode ( int Handle , int iMode , int iSubMode );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetWiMaxMeasure ( int Handle , int iMeasureType , int iMeasureSubType , int iMeasureValueType , double *plfResult );

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetSweepTime ( int Handle , double lfTime );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetTraceDetector ( int Handle , int iType );

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetMarkerTimePosition( int Handle , double lfTimePosition );

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetToMode_Specrum ( int Handle );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_RecallState( int Handle , char *szFileName );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SaveState( int Handle ,char *szFileName );  

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetLastStateFileName( int Handle ,char **szFileName );  

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Read_EVM( int Handle , double *plfEVM_Value );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetFrequency_EVM ( int Handle , double lfFrequency );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_RestartMeasure( int Handle );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Read_SymbolStream( int Handle ,int iNumberOfSymbols, char **pSymbolStream,int *piNumberOfSymbolsRead);

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_PhaseNoise_GetMarker ( int Handle , int iChannel , int marker , double lfTimeOut , double *pPosition , double *pValue );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_AutoTune ( int Handle , double lfTimeout );

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetLossFactor( int Handle , double lfFrequency , double *vlfLossFactor );

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Read_IP3( int Handle , double *plfIP3_Value );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Read_IP2( int Handle , double *plfIP2_Value );  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Read_Harmonics_Distortion( int Handle , double *plfTHD_Value );  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Read_Harmonics_dBc( int Handle , double *plfTHD_Value );  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Read_Harmonics_Amplitude( int Handle , int iHarmonicIndex , double *plfValue );  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Read_Spurious_Emissions_List( int Handle , int *piNumberOfSpurious , int **pvRangeNumber , int **pvPassFail , double **pvlfSpurFrequency , double **pvlfSpurAmplitude , double **pvlfSpurLimit );  

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetOffset( int Handle , double lfOffset );

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetMarkerFrequencyCounter( int Handle , int iMarkerNr , int bState );

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetMarkerFrequencyCounter( int Handle , int iMarkerNr , double *vlfFrequency );

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_MeasureMarkerFrequencyCounter( int Handle , int iMarkerNr , double lfTimeout , double *vlfFrequency );  

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_AlignNow ( int Handle , double lfTimeout );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_NoiseFigure_CalibrateNow ( int Handle , double lfTimeout );

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_NoiseFigure_SetLossCompensationTable ( int Handle , int bAfterDUT , double *pvFrequencies, double *pvLoss , int iNumberOfPoints );
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_NoiseFigure_ApplyCalibration ( int Handle , double lfTimeout );

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __SA_DriversManager_H__ */
