#ifndef __NA_DriversManager_H__
#define __NA_DriversManager_H__

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
int	DLLEXPORT	DRIVER_MANAGER_FreeCallbacksStructure( void **pSTD_GetCallBackSet , int iNumberOfParams , ... );
int	DLLEXPORT	DRIVER_MANAGER_Get_Number_Of_Channels( int Handle , int *piNumberOfChannels );
int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_Port_Number( int Handle , int iCalibrationPortNumber );
int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_Power( int Handle , double lfSetPower ) ;
int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_MeasureDelay( int Handle , double lfMeasureDelay );
int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_StateFileName( int Handle , char *pStateFileName );
int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_ReferenceLevel( int Handle , double lfReferenceLevel );
int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_Attenuator( int Handle , double lfAttenuator );

STD_ERROR DLLEXPORT DRIVER_MANAGER_CALIBRATION_GetCalibrationRangeInfo( int Handle , double *plfStart , double *plfStop , int *piNumberOfPoints ); 
STD_ERROR DLLEXPORT DRIVER_MANAGER_CALIBRATION_CreateEmptyHandle( int *pHandle ); 
STD_ERROR DLLEXPORT DRIVER_MANAGER_CALIBRATION_DiscardHandle( int *pHandle ); 

STD_ERROR   DLLEXPORT	DRIVER_MANAGER_GetEquipmentInformation( int hHandle , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware );

STD_ERROR   DLLEXPORT	DRIVER_MANAGER_Equipment_BrowseSelectStateFiles ( int hHandle , char *pStoreFilePath , int *piNumberOfSelectedFiles , char ***pvszSelectedFileNamesList , int **pvszSelectedFileSizesList );

STD_ERROR   DLLEXPORT	DRV_Config_SetAttribute( int hHandle , char *pAttributeName , char *pAttributeValue );
//==============================================================================
// Global functions

/***** ***** ***** ***** ***** ***** ***** ***** Init ***** ***** ***** ***** ***** ***** ***** *****/
int 		DLLEXPORT	DRV_NetworkAnalyzer_GetErrorTextMessage ( int Handle , int iError , char **pErrorMessage );  

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Init ( char *pszDriverLocation , char *pszAddressString , int *pHandle );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Close ( int *pHandle );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Reset ( int Handle );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_GetConnectedPort ( int Handle , int *iPort );

//*********************************** calibration *********************************************//

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Calibration_SetFrequencyPlan ( int Handle , double lfStartFrequncy , double lfStopFrequency , double lfPowerLevel , double lfReferenceLevel , int iNumberOfPoints );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SelectCalibrationFromFile ( int Handle , char *pFileName );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Calibration_Measure ( int Handle , int iCalibrationType , int iCalibrationStandard );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Calibration_Apply ( int Handle , int iCalibrationType );

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Kit_SelectFromFile ( int Handle , char *szFileName , int port );

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetCalibrationName( int Handle , char *pszName );

//********************************** measurements **********************************************//

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_MeasureSingleSweep ( int Handle , int iMeasurementType );

//********************************** signal processing **********************************************//

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetAverage ( int Handle , int iAverage );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetSmoothing ( int Handle , int iSmoothing );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetDwellTime ( int Handle , double lfDwellTime );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetPowerLevel ( int Handle , int iPowerLevel );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetReferencePlane ( int Handle , int iMeasurementType , int iValue , int bAuto );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SaveMeasurementToMemory ( int Handle , int iMeasurementType );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_ApplyMemoryMath ( int Handle , int iMeasurementType , int iFunction );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetSystemImpedance ( int Handle , int iSystemImpedance , int bExternalMatch );

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetElectricalDelayTime( int Handle , int iChannel , double lfTime );

//********************************** get processed data **********************************************//

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_GetData ( int Handle , int iMeasurementType , int iMeasurementFormat , int iFrequencyIndex , double *plfFrequency , double *plfValue );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_GetMemory ( int Handle , int iMeasurementType , int iMeasurementFormat , int iFrequencyIndex , double *plfFrequency , double *plfValue );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_FindDataPoint ( int Handle , double lfFrequency , int iMeasurementType , int iMeasurementFormat , int iMeasurementFunction , double *plfFrequency , double *plfValue );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_GetTrace ( int Handle , int iChannel , int iMeasurementType , int iMeasurementFormat , double lfTimeout ,  double **plfFrequency , double **plfTrace , int *piNumberOfPoints );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_GetInfo ( int Handle , int iInfoType , char *pInfo );

//********************************** data storage **********************************************//

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SaveStateAndCalibration ( int Handle , char *pFileName );

//********************************** data retrieval **********************************************//

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_LoadStateAndCalibration ( int Handle , char *pFileName );

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Reset(int Handle);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetFrequency(int Handle,double lfStartFrequency, double lfStopFrequency);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_GetFrequency(int Handle,double *plfStartFrequency, double *plfStopFrequency);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetPointsNum(int Handle,int points);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_GetPointsNum(int Handle,int *points);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_GetMeasureVectors(int Handle,int iGetDataType, double **plfFirstVector,double **plfSecondVector, double **plfThirdVector,double **plfFourthVector);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetMeasureType(int Handle,int iChannel, int iInSparam, int iConversation);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_DisplaySelect(int Handle,int iChannel, int iDataMode, int iDataFormat);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_AutoScale(int Handle);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_AvarageSmooth(int Handle,int iChannel, char *szSmoothRequest, double lfSetSmooth,double lfCurrentSmooth);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Smooth(int Handle,int iChannel, int iState, double lfSetSmooth,int iNumberPoints);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetBandWidth(int Handle,int iChannel, double lfBW_Value, char *szBW_Request,double *lfGetBW_Value);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetFrequencySweepType(int Handle,int iChannel, int iFrequencySweepType);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetPower(int Handle,int iChannel, double lfPower, char *szPowerRequest,int iPortNum);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_GetPower(int Handle,int iChannel, double *lfPower, int iPortNum);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetPowerRange(int Handle,int iChannel, int iRange, char *szRangeRequest,int *iCurrentRange);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetPowerSweep(int Handle,int iChannel, double lfStartPower, double lfStopPower,char *szSweepRequest, double *lfGetStartPower,double *lfGetStopPower);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetSweepTime(int Handle,int iChannel, double lfSweepTime, char *szSweepTimeRequest,double *lfGetSweepTime);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetMarker(int Handle,int iChannel, double lfSetValue, int iMarker,char *szMarkerRequest);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_GetMarker(int Handle,int iChannel, int iMarker, char *szMarkerRequest,double *lfGetMarkerOneValue, double *lfGetMarkerTwoValue,double *lfGetMarkerThreeValue, double *lfGetMarkerFourValue);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_ReCallRegister(int Handle,char *szRegister_FileName);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SaveToRegister(int Handle,char *szTitle, int iRegister);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_QuaryCalibration(int Handle,int iChannel, char *pResponse);
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SelectChannel ( int Handle , int iChannel );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SelectTrace ( int Handle , iTraceNumber );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SelectWindow ( int Handle , iWindowNumber ); 

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_GetLastStateFileName(int Handle, char **pszFileName );

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SelectMathFunction ( int hInstrumentHandle , int iChannel , char cFunction );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetTraceToMathMemory ( int hInstrumentHandle , int iChannel );  
	
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_ECAL_GetInformation ( int Handle , char **pszCatalog );  
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_ECAL_Calibrate ( int Handle , char *pszPortList , char *pUserCal , double lfTimeout );  

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_ClearAvarage( int Handle , int iChannel );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Avarage( int Handle , int iChannel, int bEnable , int iAverageValue );

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Reponse_OpenShortLoad_Calibrate(int hInstrumentHandle , int iType_OSL , char *pszPortList , double lfTimeout );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Reponse_Thru_Calibrate(int hInstrumentHandle, char *pszPortList , double lfTimeout );
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Reponse_Done_Calibrate(int hInstrumentHandle );

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_WaitSweep( int Handle , int iChannel , int iCount );


#ifdef __cplusplus
    }
#endif

#endif  /* ndef __NA_DriversManager_H__ */
