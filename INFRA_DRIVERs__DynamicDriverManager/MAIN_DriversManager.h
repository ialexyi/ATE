#ifndef __DRIVER_MANAGER_DriversManager_H__
#define __DRIVER_MANAGER_DriversManager_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"
		
#include "GlobalDefines.h"
		
#include "PM_DriversManager.h"		
#include "PS_DriversManager.h"
#include "SG_DriversManager.h"
#include "SC_DriversManager.h"		
#include "SA_DriversManager.h"		
#include "MM_DriversManager.h"
#include "WA_DriversManager.h"		
#include "NF_DriversManager.h"
#include "NA_DriversManager.h"
#include "FG_DriversManager.h"
#include "STD_DriversManager.h"
#include "EQUIP_FileSupport.h"

#define		CHK_PROCCESS_GET_LOCK(xxLockHandle)		{ IF((xxLockHandle==0),"Handle is NULL."); do { CHK_CMT( CmtTryToGetLock ( xxLockHandle , &bLocked ));  if ( bLocked ) break; ProcessDrawEvents(); DelayWithEventProcessing(0.3); }while(bLocked==0); }

#define		DRIVER_MANAGER_STD_FUNCTION_VARIABLES_DECLARATION	STD_ERROR	StdError	=	{0}; tsSTD_CallBackSet	tSTD_CallBackSet =	{0};
		
#define			INSTR_TYPE_CONTINUE							0
#define			INSTR_TYPE_YES_NO							1
//==============================================================================
// Constants
//==============================================================================
// Types

typedef void* (*pfSTD_Config_Install_CommentCallback) ( int *pHandle , pfCommentCallback fFunction , void *pData , int type);
typedef void* (*pfSTD_Config_Install_ConfigValueCallback) ( int *pHandle , pfConfigValueCallback fFunction , void *pData , int type);
typedef void* (*pfSTD_Config_Install_CheckForBreakCallback) ( int *pHandle , pfCheckForBreakCallback fFunction , void *pData , int type);
typedef void* (*pfSTD_Config_LOG_SetAllowState) ( int iHandle , int bAllowLog ); 
typedef void* (*pfSTD_Config_Copy_STD_CallBackSet) ( int *pHandle , void *pCallBackSet );
typedef void* (*pfSTD_Config_SetAttribute) ( int *pHandle , char *pAttributeName , char *pAttributeValue );     

typedef void* (*pfSTD_Equipment_Info) ( int hLowLevelHandle , char *pAddress , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware ); 

typedef void* (*pfSTD_Equipment_IsSupported) ( int hLowLevelHandle , char *pAddress , char *pID_String , char *pIdentificationText , int *pbSupporting , void **pLocalData );

typedef void* (*pfSTD_Equipment_GetLowLevelHandle)( int hHandle , int *phLowLevelHandle );

typedef void* (*pfSTD_Equipment_SendStateFile)( int hHandle , char *szLocalFileName ,char *szStateFileName  );  
typedef void* (*pfSTD_Equipment_ReceiveStateFile)( int hHandle , char *szLocalFileName ,char *szStateFileName  );  
typedef void* (*pfSTD_Equipment_GetFileCatalog)( int hHandle , char *szPath , char ***szFileNames , int **pvFileSizes , int *piNumberOfFiles );
typedef void* (*pfSTD_Equipment_MakeFileCatalog)( int hHandle , char *szPath );
typedef void* (*pfSTD_Equipment_DeleteFileCatalog)( int hHandle , char *szCatalogName );  
typedef void* (*pfSTD_Equipment_DeleteStateFile)( int hInstrumentHandle , char *szStateFileName );

//**************************************************************************************//
//********************************* Multi Meter Structure ******************************//
//**************************************************************************************//
		
typedef void* (*pfMultiMeter_Config_Install_CommentCallback) ( int *pHandle , pfCommentCallback fFunction , void *pData , int type);
typedef void* (*pfMultiMeter_Config_Install_ConfigValueCallback) ( int *pHandle , pfConfigValueCallback fFunction , void *pData , int type);
typedef void* (*pfMultiMeter_Config_Install_CheckForBreakCallback) ( int *pHandle , pfCheckForBreakCallback fFunction , void *pData , int type);

typedef void* (*pfMultiMeter_GetErrorTextMessage) ( int handle , int iError , char *pErrorMessage );
typedef void* (*pfMultiMeter_Init) ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *phInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChannelsQuantity );
typedef void* (*pfMultiMeter_Close) ( int *phInstrumentHandle );
typedef void* (*pfMultiMeter_Reset) ( int hInstrumentHandle );
typedef void* (*pfMultiMeter_Measure) ( int hInstrumentHandle , int iChannel , int iType , double dDelay , double *pdMeasurement );
typedef void* (*pfMultiMeter_Config) ( int hInstrumentHandle , int iChannel , int iType , int iAverage , int iScale );
typedef void* (*pfMultiMeter_ClearStatus) ( int hInstrumentHandle );


typedef struct
{
			pfSTD_Config_Install_CommentCallback						Config_Install_CommentCallback;
			pfSTD_Config_Install_ConfigValueCallback					Config_Install_ConfigValueCallback;
			pfSTD_Config_Install_CheckForBreakCallback					Config_Install_CheckForBreakCallback;
			pfSTD_Config_LOG_SetAllowState								Config_LOG_SetAllowState;
			pfSTD_Config_Copy_STD_CallBackSet							Config_Copy_STD_CallBackSet;
			
			pfMultiMeter_GetErrorTextMessage							MultiMeter_GetErrorTextMessage;          
			pfMultiMeter_Init								 		 	MultiMeter_Init;                         
			pfMultiMeter_Close								 		 	MultiMeter_Close;                        
			pfMultiMeter_Reset											MultiMeter_Reset;
			pfMultiMeter_Measure										MultiMeter_Measure;
			pfMultiMeter_Config											MultiMeter_Config;
			pfMultiMeter_ClearStatus									MultiMeter_ClearStatus;

} tsMultiMeterFunctions;

//**************************************************************************************//		
		
//**************************************************************************************//
//********************************* Noise Figure Structure ******************************//
//**************************************************************************************//
		
typedef void* (*pfNoiseFigure_GetErrorTextMessage) ( int handle , int iError , char *pErrorMessage );
typedef void* (*pfNoiseFigure_Init) ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *phInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChannelsQuantity );
typedef void* (*pfNoiseFigure_Close) ( int *phInstrumentHandle );
typedef void* (*pfNoiseFigure_Reset) ( int hInstrumentHandle );
typedef void* (*pfNoiseFigure_ConfigInput) ( int hInstrumentHandle , int iInput , int iMeasureType , int iAverage , double lfRefMeasureFreq );
typedef void* (*pfNoiseFigure_SetMeasureType) ( int hInstrumentHandle , int iType );
typedef void* (*pfNoiseFigure_Config) ( int hInstrumentHandle , int bSweepMode , int iStartFreqMhz , int iStopFreqMhz , int iStepFreqMhz );
typedef void* (*pfNoiseFigure_Converter) ( int hInstrumentHandle , int bFixedIF_lo , int iIF_FreqMhz , int iLO_FreqMhz , int iSideBand );
typedef void* (*pfNoiseFigure_Calibration) ( int hInstrumentHandle , int bMode );
typedef void* (*pfNoiseFigure_SetLossFactor) ( int hInstrumentHandle , int bEnable , double lfInLoss , double lfOutLoss , double lfTempLoss );
typedef void* (*pfNoiseFigure_SweepMode) ( int hInstrumentHandle , int iMode );
typedef void* (*pfNoiseFigure_GetFixedMeasure) ( int hInstrumentHandle , int iInput , int iMeasureType , int *Frequency_MHz , double *vlfNoise_Figure , double *vlfWorstCase );
typedef void* (*pfNoiseFigure_GetPlotMeasure) ( int hInstrumentHandle , int *Frequency_MHz , double *vlfNoise_Figure , double *vlfGain , int *viCount );
typedef void* (*pfNoiseFigure_Mode) ( int hInstrumentHandle , int iPlotMode , int iPoints , double lfNoise_Max, double lfNoise_Min , double lfGain_Max, double lfGain_Min );


typedef struct
{
			pfSTD_Config_Install_CommentCallback						Config_Install_CommentCallback;
			pfSTD_Config_Install_ConfigValueCallback					Config_Install_ConfigValueCallback;
			pfSTD_Config_Install_CheckForBreakCallback					Config_Install_CheckForBreakCallback;
			pfSTD_Config_LOG_SetAllowState								Config_LOG_SetAllowState;
			pfSTD_Config_Copy_STD_CallBackSet							Config_Copy_STD_CallBackSet;
			
			pfNoiseFigure_GetErrorTextMessage							NoiseFigure_GetErrorTextMessage;          
			pfNoiseFigure_Init											NoiseFigure_Init;                         
			pfNoiseFigure_Close											NoiseFigure_Close;                        
			pfNoiseFigure_Reset											NoiseFigure_Reset;
			pfNoiseFigure_ConfigInput									NoiseFigure_ConfigInput;
			pfNoiseFigure_SetMeasureType								NoiseFigure_SetMeasureType;
			pfNoiseFigure_Config										NoiseFigure_Config;
			pfNoiseFigure_Converter										NoiseFigure_Converter;
			pfNoiseFigure_Calibration									NoiseFigure_Calibration;
			pfNoiseFigure_SetLossFactor									NoiseFigure_SetLossFactor;
			pfNoiseFigure_SweepMode										NoiseFigure_SweepMode;
			pfNoiseFigure_GetFixedMeasure								NoiseFigure_GetFixedMeasure;
			pfNoiseFigure_GetPlotMeasure								NoiseFigure_GetPlotMeasure;
			pfNoiseFigure_Mode											NoiseFigure_Mode;

} tsNoiseFigureFunctions;

//**************************************************************************************//

//**************************************************************************************//
//********************************* Power Meter Structure ******************************//
//**************************************************************************************//
		
typedef void* (*pfPowerMeter_GetErrorTextMessage) ( int handle , int iError , char *pErrorMessage );
typedef void* (*pfPowerMeter_Init) ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *phInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChannelsQuantity );
typedef void* (*pfPowerMeter_Close) ( int *phInstrumentHandle );
typedef void* (*pfPowerMeter_Reset) ( int hInstrumentHandle );
typedef void* (*pfPowerMeter_SetSingleSweep) ( int hInstrumentHandle , int channel ); 
typedef void* (*pfPowerMeter_SetContinuesSweep) ( int hInstrumentHandle , int channel ); 
typedef void* (*pfPowerMeter_InitSweep) ( int hInstrumentHandle , int channel ); 
typedef void* (*pfPowerMeter_Abort) ( int hInstrumentHandle , int channel ); 
typedef void* (*pfPowerMeter_SetAuto) ( int hInstrumentHandle , int channel ); 
typedef void* (*pfPowerMeter_SetChanelState) ( int hInstrumentHandle , int channel , int state );
typedef void* (*pfPowerMeter_SetVerticalScale) ( int hInstrumentHandle , int channel , double scale );
typedef void* (*pfPowerMeter_SetVerticalCenter) ( int hInstrumentHandle , int channel , double center );
typedef void* (*pfPowerMeter_Calibrate) ( int hInstrumentHandle , int channel );
typedef void* (*pfPowerMeter_Zero) ( int hInstrumentHandle , int channel );
typedef void* (*pfPowerMeter_SetUnits) ( int hInstrumentHandle , int channel , unsigned int units );
typedef void* (*pfPowerMeter_SetOffset) ( int hInstrumentHandle , int channel , double Offset );
typedef void* (*pfPowerMeter_SetCalFactor) ( int hInstrumentHandle , int channel , double CalFactor );
typedef void* (*pfPowerMeter_SetVideoBW) ( int hInstrumentHandle , int channel ,  unsigned int VideoBW );
typedef void* (*pfPowerMeter_SetPeakHold) ( int hInstrumentHandle , int channel , int state );
typedef void* (*pfPowerMeter_SetFrequency) ( int hInstrumentHandle , int channel , double Frequency );
typedef void* (*pfPowerMeter_SetMode) ( int hInstrumentHandle , int channel , unsigned int Mode );
typedef void* (*pfPowerMeter_Channel_SetMode) ( int hInstrumentHandle , int iChannel , unsigned int Mode );
typedef void* (*pfPowerMeter_OPC) ( int hInstrumentHandle , double timeout , int *state );
typedef void* (*pfPowerMeter_FetchCWpower) ( int hInstrumentHandle , int channel , double timeout , double *value );
typedef void* (*pfPowerMeter_MeasureModulatedPower) ( int hInstrumentHandle , int channel , double timeout , double *value );
typedef void* (*pfPowerMeter_FetchMarkerAverage) ( int hInstrumentHandle , int channel , int marker , double timeout , double *value );
typedef void* (*pfPowerMeter_FetchMarkerMax) ( int hInstrumentHandle , int channel , int marker , double timeout , double *value );
typedef void* (*pfPowerMeter_FetchMarkerMin) ( int hInstrumentHandle , int channel , int marker , double timeout , double *value );
typedef void* (*pfPowerMeter_MeasurePowerPeakHold) ( int hInstrumentHandle , int channel , double timeout , double *value );
typedef void* (*pfPowerMeter_MeasurePower) ( int hInstrumentHandle , int channel , double timeout , double *value );
typedef void* (*pfPowerMeter_Configuration) ( int hInstrumentHandle , int iChannel , int bContinue , int bDelayAutoState , int iTriggerSource , double lfTrigHoldOff , double lfGateOffsetTime , double lfGateTime , double lfTraceTime );
typedef void* (*pfPowerMeter_RecallState) ( int hInstrumentHandle , int iStateNumber ); 
typedef void* (*pfPowerMeter_ReadPower) ( int hInstrumentHandle , int channel , double timeout , double *value );
typedef void* (*pfPowerMeter_FetchPower) ( int hInstrumentHandle , int channel , double timeout , double *value );
typedef void* (*pfPowerMeter_SaveState) ( int hInstrumentHandle , int iStateNumber );  
typedef void* (*pfPowerMeter_Gating_ConfigOffsetTime) ( int hInstrumentHandle , int iChannel , int iGate , double lfTime , double lfOffset , double lfMidambleOffset, double lfMidambleLength );
typedef void* (*pfPowerMeter_Gating_ReadMaxPower) ( int hInstrumentHandle , int iChannel , int iGait , double timeout , double *value ); 
typedef void* (*pfPowerMeter_Gating_FetchMaxPower) ( int hInstrumentHandle , int iChannel , int iGait , double timeout , double *value ); 
typedef void* (*pfPowerMeter_Gating_FetchAveragePower) ( int hInstrumentHandle , int iGait , double timeout , int iChannel , double *value );   
typedef void* (*pfPowerMeter_Gating_FetchMinPower) ( int hInstrumentHandle , int iGait , double timeout , int iChannel , double *value ); 
typedef void* (*pfPowerMeter_SetStateName) ( int hInstrumentHandle , int iStateNumber , char *pszName );  
typedef void* (*pfPowerMeter_GetStateName) ( int hInstrumentHandle , int iStateNumber , char *pszName ); 
typedef void* (*pfPowerMeter_SetStateFile) ( int hInstrumentHandle , int iStateNumber , char *pszFileName );  
typedef void* (*pfPowerMeter_GetStateFile) ( int hInstrumentHandle , int iStateNumber , char *pszFileName );  
typedef void* (*pfPowerMeter_RecallState_ByName) ( int hInstrumentHandle , char *pszName ); 
typedef void* (*pfPowerMeter_SetActiveChannel)( int hInstrumentHandle , int iChannel );  
typedef void* (*pfPowerMeter_Configure_Marker)( int hInstrumentHandle , int iChannel , int iMarkerNumber , int bEnable , double lfPosition );                    	
typedef void* (*pfPowerMeter_Read_Marker)( int hInstrumentHandle , int iChannel , int iMarkerNumber , double timeout , double *vlfPosition , double *vlfPower );
typedef void* (*pfPowerMeter_Fetch_Marker)( int hInstrumentHandle , int iChannel , int iMarkerNumber , double *vlfPosition , double *vlfPower ); 
typedef void* (*pfPowerMeter_SetActivePort)( int hInstrumentHandle , int iSensorNumber ); 
typedef void* (*pfPowerMeter_EnableDualPortMode)( int hInstrumentHandle , int bEnable );
typedef void* (*pfPowerMeter_ReadCWpower)( int hInstrumentHandle , int iChannel , double timeout , double *value );
typedef void* (*pfPowerMeter_GetLastStateFileName)( int hInstrumentHandle ,char **pszFileName ); 
typedef void* (*pfPowerMeter_Gating_ReadPower)( int hInstrumentHandle , int iChannel , int iGait , int iGaitMeasureIndex , double timeout , double *value );
typedef void* (*pfPowerMeter_GetTrace) ( int hInstrumentHandle, int iChannel, int timeout, double **plfTime , double **pTrace , int numberOfPoints , int *pCount ); 
typedef void* (*pfPowerMeter_Set_Trigger_Source) ( int hInstrumentHandle, int iTriggerSorce ); 
typedef void* (*pfPowerMeter_Abort) ( int hInstrumentHandle, int iChannel ); 

typedef struct
{
			pfSTD_Config_Install_CommentCallback						Config_Install_CommentCallback;
			pfSTD_Config_Install_ConfigValueCallback					Config_Install_ConfigValueCallback;
			pfSTD_Config_Install_CheckForBreakCallback					Config_Install_CheckForBreakCallback;
			pfSTD_Config_LOG_SetAllowState								Config_LOG_SetAllowState;
			pfSTD_Config_Copy_STD_CallBackSet							Config_Copy_STD_CallBackSet;
			
			pfPowerMeter_GetErrorTextMessage							PowerMeter_GetErrorTextMessage;          
			pfPowerMeter_Init						  					PowerMeter_Init;                         
			pfPowerMeter_Close						  					PowerMeter_Close;                        
			pfPowerMeter_Reset											PowerMeter_Reset;
			pfPowerMeter_SetSingleSweep									PowerMeter_SetSingleSweep;
			pfPowerMeter_SetContinuesSweep								PowerMeter_SetContinuesSweep;                  
			pfPowerMeter_InitSweep			  							PowerMeter_InitSweep;             
			pfPowerMeter_SetAuto										PowerMeter_SetAuto;      
			pfPowerMeter_SetChanelState									PowerMeter_SetChanelState;       
			pfPowerMeter_SetVerticalScale			  					PowerMeter_SetVerticalScale;             
			pfPowerMeter_SetVerticalCenter								PowerMeter_SetVerticalCenter;      
			pfPowerMeter_Calibrate										PowerMeter_Calibrate;       
			pfPowerMeter_Zero	  										PowerMeter_Zero;     
			pfPowerMeter_SetUnits					  					PowerMeter_SetUnits;     
			pfPowerMeter_SetOffset	  									PowerMeter_SetOffset;    
			pfPowerMeter_SetCalFactor				  					PowerMeter_SetCalFactor;    
			pfPowerMeter_SetVideoBW										PowerMeter_SetVideoBW;               
			pfPowerMeter_SetPeakHold									PowerMeter_SetPeakHold;               
			pfPowerMeter_SetFrequency				  					PowerMeter_SetFrequency;    
			pfPowerMeter_SetMode	 				 					PowerMeter_SetMode;    
			pfPowerMeter_Channel_SetMode								PowerMeter_Channel_SetMode;
			pfPowerMeter_OPC											PowerMeter_OPC;           
			pfPowerMeter_FetchCWpower									PowerMeter_FetchCWpower;           
			pfPowerMeter_MeasureModulatedPower 							PowerMeter_MeasureModulatedPower; 
			pfPowerMeter_FetchMarkerAverage								PowerMeter_FetchMarkerAverage;       
			pfPowerMeter_FetchMarkerMax		  							PowerMeter_FetchMarkerMax;        
			pfPowerMeter_FetchMarkerMin			  						PowerMeter_FetchMarkerMin;            
			pfPowerMeter_MeasurePowerPeakHold							PowerMeter_MeasurePowerPeakHold;
			pfPowerMeter_MeasurePower									PowerMeter_MeasurePower;
			pfPowerMeter_Configuration									PowerMeter_Configuration;
			pfPowerMeter_RecallState									PowerMeter_RecallState;
			pfPowerMeter_ReadPower 										PowerMeter_ReadPower;
			pfPowerMeter_FetchPower										PowerMeter_FetchPower;	
			pfPowerMeter_SaveState										PowerMeter_SaveState;
			pfPowerMeter_Gating_ConfigOffsetTime						PowerMeter_Gating_ConfigOffsetTime;
			pfPowerMeter_Gating_ReadMaxPower							PowerMeter_Gating_ReadMaxPower; 
			pfPowerMeter_Gating_FetchAveragePower						PowerMeter_Gating_FetchAveragePower;
			pfPowerMeter_Gating_FetchMaxPower 							PowerMeter_Gating_FetchMaxPower; 
			pfPowerMeter_Gating_FetchMinPower 							PowerMeter_Gating_FetchMinPower;   
			pfPowerMeter_SetStateName									PowerMeter_SetStateName;           
			pfPowerMeter_GetStateName									PowerMeter_GetStateName;           
			pfPowerMeter_SetStateFile									PowerMeter_SetStateFile;           
			pfPowerMeter_GetStateFile									PowerMeter_GetStateFile;           
			pfPowerMeter_RecallState_ByName								PowerMeter_RecallState_ByName;
			pfPowerMeter_SetActiveChannel								PowerMeter_SetActiveChannel;
			pfPowerMeter_Configure_Marker								PowerMeter_Configure_Marker;
			pfPowerMeter_Read_Marker									PowerMeter_Read_Marker;
			pfPowerMeter_SetActivePort									PowerMeter_SetActivePort;
			pfPowerMeter_EnableDualPortMode								PowerMeter_EnableDualPortMode;
			pfPowerMeter_ReadCWpower									PowerMeter_ReadCWpower;

			//------------ Version 1.1.0.0 --------------------//  
			pfPowerMeter_GetLastStateFileName							PowerMeter_GetLastStateFileName;
			
			//------------ Version 1.7.4.0 --------------------//       
			pfPowerMeter_Gating_ReadPower								PowerMeter_Gating_ReadPower;
			pfPowerMeter_Fetch_Marker									PowerMeter_Fetch_Marker;
			
			//------------ Version 1.7.5.0 --------------------//       
			pfPowerMeter_GetTrace										PowerMeter_GetTrace;

			//------------ Version 1.7.6.0 --------------------//       
			pfPowerMeter_Set_Trigger_Source								PowerMeter_Set_Trigger_Source;
			pfPowerMeter_Abort											PowerMeter_Abort;
			
} tsPowerMeterFunctions;

//**************************************************************************************//

//**************************************************************************************//
//********************************* Power Supply Structure ******************************//
//**************************************************************************************//

typedef void* (*pfPowerSupply_GetErrorTextMessage) ( int hInstrumentHandle , int iError , char *pErrorMessage );
typedef void* (*pfPowerSupply_Init) ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *hInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChanelsQuantity );
typedef void* (*pfPowerSupply_Close) ( int *phInstrumentHandle );
typedef void* (*pfPowerSupply_Reset) ( int hInstrumentHandle ); 
typedef void* (*pfPowerSupply_ClearCommBufferAndRegisters) ( int hInstrumentHandle , int channel );
typedef void* (*pfPowerSupply_GetModel) ( int hInstrumentHandle , int channel , char *pReturnString );
typedef void* (*pfPowerSupply_GetRevision) ( int hInstrumentHandle , int channel , char *pReturnString );
typedef void* (*pfPowerSupply_SetOutputVoltage) ( int hInstrumentHandle , int channel , double lfVoltage );
typedef void* (*pfPowerSupply_GetOutputVoltageDefined) ( int hInstrumentHandle , int channel , double *plfVoltage );
typedef void* (*pfPowerSupply_GetOutputVoltageActual) ( int hInstrumentHandle , int channel , double *plfVoltage );
typedef void* (*pfPowerSupply_SetOutputCurrent) ( int hInstrumentHandle , int channel , double lfCurrent );
typedef void* (*pfPowerSupply_GetOutputCurrentDefined) ( int hInstrumentHandle , int channel , double *plfCurrent );
typedef void* (*pfPowerSupply_GetOutputCurrentActual) ( int hInstrumentHandle , int channel , double *plfCurrent );
typedef void* (*pfPowerSupply_GetOutputPowerActual) ( int hInstrumentHandle , int channel , double *plfCurrent );
typedef void* (*pfPowerSupply_SetVoltageProtectionOver) ( int hInstrumentHandle , int channel , double lfVoltage );
typedef void* (*pfPowerSupply_GetVoltageProtectionOver) ( int hInstrumentHandle , int channel , double *plfVoltage );
typedef void* (*pfPowerSupply_SetVoltageProtectionUnder) ( int hInstrumentHandle , int channel , double lfVoltage );
typedef void* (*pfPowerSupply_GetVoltageProtectionUnder) ( int hInstrumentHandle , int channel , double *plfVoltage );
typedef void* (*pfPowerSupply_SetOutputState) ( int hInstrumentHandle , int channel , int iState );
typedef void* (*pfPowerSupply_GetOutputState) ( int hInstrumentHandle , int channel , int *piState );
typedef void* (*pfPowerSupply_SetFoldbackProtectionMode) ( int hInstrumentHandle , int channel , int iMode );
typedef void* (*pfPowerSupply_GetFoldbackProtectionMode) ( int hInstrumentHandle , int channel , int *piMode );
typedef void* (*pfPowerSupply_SetAutoRestartMode) ( int hInstrumentHandle , int channel , int iMode );
typedef void* (*pfPowerSupply_GetAutoRestartMode) ( int hInstrumentHandle , int channel , int *piMode );
typedef void* (*pfPowerSupply_GetOperationalStatusRegister) ( int hInstrumentHandle , int channel , char *pReturnString );
typedef void* (*pfPowerSupply_GetAlarmStatusRegister) ( int hInstrumentHandle , int channel , char *pReturnString );
typedef void* (*pfPowerSupply_GetErrorCodesRegister) ( int hInstrumentHandle , int channel , char *pReturnString );
typedef void* (*pfPowerSupply_GetCompleteStatus) ( int hInstrumentHandle , int channel , char *pReturnString );
typedef void* (*pfPowerSupply_WaitForOperationComplete) ( int hInstrumentHandle , double lfTimeout ); 	
typedef void* (*pfPowerSupply_SetRange)( int hInstrumentHandle , int channel , int iRange );
typedef void* (*pfPowerSupply_GetRange)( int hInstrumentHandle , int channel , int *piRange );

typedef void* (*pfPowerSupply_SetRangeAuto) ( int hInstrumentHandle );   
typedef void* (*pfPowerSupply_GetOutputPowerApparent) ( int hInstrumentHandle , int channel , double *plfPower );
typedef void* (*pfPowerSupply_GetOutputPowerFactor)( int hInstrumentHandle , int channel , double *plfPowerFactor );
typedef void* (*pfPowerSupply_SetVoltageRange) ( int hInstrumentHandle , int channel , double lfVoltage );
typedef void* (*pfPowerSupply_GetVoltageRange) ( int hInstrumentHandle , int channel , double *plfVoltage );
typedef void* (*pfPowerSupply_SetCurrentRange) ( int hInstrumentHandle , int channel , double lfCurrent );
typedef void* (*pfPowerSupply_GetCurrentRange) ( int hInstrumentHandle , int channel , double *plfCurrent );
typedef void* (*pfPowerSupply_SetFrequency) ( int hInstrumentHandle , int channel , double lfFrequency );
typedef void* (*pfPowerSupply_GetFrequency) ( int hInstrumentHandle , int channel , double *plfFrequency );
typedef void* (*pfPowerSupply_SetOutputStarState) ( int hInstrumentHandle , int channel , int iState );
typedef void* (*pfPowerSupply_GetOutputStarState) ( int hInstrumentHandle , int channel , int *piState );
typedef void* (*pfPowerSupply_SetStarPhase) ( int hInstrumentHandle , int channel , double lfPhase );
typedef void* (*pfPowerSupply_GetStarPhase) ( int hInstrumentHandle , int channel , double *plfPhase );
typedef void* (*pfPowerSupply_SetFrequencyProtectionOver) ( int hInstrumentHandle , int channel , double lfFrequency );
typedef void* (*pfPowerSupply_GetFrequencyProtectionOver) ( int hInstrumentHandle , int channel , double *plfFrequency );
typedef void* (*pfPowerSupply_SetFrequencyProtectionUnder) ( int hInstrumentHandle , int channel , double lfFrequency );
typedef void* (*pfPowerSupply_GetFrequencyProtectionUnder) ( int hInstrumentHandle , int channel , double *plfFrequency );
typedef void* (*pfPowerSupply_SetTrackingMode) ( int viInstrumentHandle , int channel , int iMode );
typedef void* (*pfPowerSupply_GetTrackingMode) ( int viInstrumentHandle , int channel , int *piMode );
typedef void* (*pfPowerSupply_SetRemoteSense) ( int viInstrumentHandle , int channel , int iState );
typedef void* (*pfPowerSupply_GetRemoteSense) ( int viInstrumentHandle , int channel , int *piState );


typedef struct
{
			pfSTD_Config_Install_CommentCallback						Config_Install_CommentCallback;
			pfSTD_Config_Install_ConfigValueCallback					Config_Install_ConfigValueCallback;
			pfSTD_Config_Install_CheckForBreakCallback					Config_Install_CheckForBreakCallback;
			pfSTD_Config_LOG_SetAllowState								Config_LOG_SetAllowState;
			pfSTD_Config_Copy_STD_CallBackSet							Config_Copy_STD_CallBackSet;
			
			pfPowerSupply_GetErrorTextMessage							PowerSupply_GetErrorTextMessage;          
			pfPowerSupply_Init						  					PowerSupply_Init;                         
			pfPowerSupply_Close						  					PowerSupply_Close;  
			pfPowerSupply_Reset											PowerSupply_Reset;
			pfPowerSupply_ClearCommBufferAndRegisters					PowerSupply_ClearCommBufferAndRegisters;
			pfPowerSupply_GetModel					  					PowerSupply_GetModel;
			pfPowerSupply_GetRevision									PowerSupply_GetRevision;                  
			pfPowerSupply_SetOutputVoltage			  					PowerSupply_SetOutputVoltage;             
			pfPowerSupply_GetOutputVoltageDefined						PowerSupply_GetOutputVoltageDefined;      
			pfPowerSupply_GetOutputVoltageActual						PowerSupply_GetOutputVoltageActual;       
			pfPowerSupply_SetOutputCurrent			  					PowerSupply_SetOutputCurrent;             
			pfPowerSupply_GetOutputCurrentDefined						PowerSupply_GetOutputCurrentDefined;      
			pfPowerSupply_GetOutputCurrentActual						PowerSupply_GetOutputCurrentActual;       
			pfPowerSupply_GetOutputPowerActual							PowerSupply_GetOutputPowerActual;       
			pfPowerSupply_SetVoltageProtectionOver	  					PowerSupply_SetVoltageProtectionOver;     
			pfPowerSupply_GetVoltageProtectionOver	  					PowerSupply_GetVoltageProtectionOver;     
			pfPowerSupply_SetVoltageProtectionUnder	  					PowerSupply_SetVoltageProtectionUnder;    
			pfPowerSupply_GetVoltageProtectionUnder	  					PowerSupply_GetVoltageProtectionUnder;    
			pfPowerSupply_SetOutputState								PowerSupply_SetOutputState;               
			pfPowerSupply_GetOutputState								PowerSupply_GetOutputState;               
			pfPowerSupply_SetFoldbackProtectionMode	  					PowerSupply_SetFoldbackProtectionMode;    
			pfPowerSupply_GetFoldbackProtectionMode	  					PowerSupply_GetFoldbackProtectionMode;    
			pfPowerSupply_SetAutoRestartMode							PowerSupply_SetAutoRestartMode;           
			pfPowerSupply_GetAutoRestartMode							PowerSupply_GetAutoRestartMode;           
			pfPowerSupply_GetOperationalStatusRegister 					PowerSupply_GetOperationalStatusRegister; 
			pfPowerSupply_GetAlarmStatusRegister						PowerSupply_GetAlarmStatusRegister;       
			pfPowerSupply_GetErrorCodesRegister		  					PowerSupply_GetErrorCodesRegister;        
			pfPowerSupply_GetCompleteStatus			  					PowerSupply_GetCompleteStatus;            
			pfPowerSupply_WaitForOperationComplete						PowerSupply_WaitForOperationComplete;

			//------------ Version 1.5.2.0 --------------------//  
			pfPowerSupply_SetRangeAuto									PowerSupply_SetRangeAuto;                 
			pfPowerSupply_GetOutputPowerApparent						PowerSupply_GetOutputPowerApparent;      
			pfPowerSupply_GetOutputPowerFactor							PowerSupply_GetOutputPowerFactor;         
			pfPowerSupply_SetVoltageRange								PowerSupply_SetVoltageRange;              
			pfPowerSupply_GetVoltageRange								PowerSupply_GetVoltageRange;              
			pfPowerSupply_SetFrequency									PowerSupply_SetFrequency;                 
			pfPowerSupply_GetFrequency									PowerSupply_GetFrequency;                 
			pfPowerSupply_SetOutputStarState							PowerSupply_SetOutputStarState;           
			pfPowerSupply_GetOutputStarState							PowerSupply_GetOutputStarState;           
			pfPowerSupply_SetStarPhase									PowerSupply_SetStarPhase;                 
			pfPowerSupply_GetStarPhase									PowerSupply_GetStarPhase;                 
			pfPowerSupply_SetFrequencyProtectionOver					PowerSupply_SetFrequencyProtectionOver;   
			pfPowerSupply_GetFrequencyProtectionOver					PowerSupply_GetFrequencyProtectionOver;   
			pfPowerSupply_SetFrequencyProtectionUnder					PowerSupply_SetFrequencyProtectionUnder;  
			pfPowerSupply_GetFrequencyProtectionUnder					PowerSupply_GetFrequencyProtectionUnder;

			//------------ Version 1.7.3.0 --------------------//  
			pfPowerSupply_SetRange										PowerSupply_SetRange;
			pfPowerSupply_GetRange										PowerSupply_GetRange;
		
			//------------ Version 1.7.9.0 --------------------//  
			pfPowerSupply_SetTrackingMode								PowerSupply_SetTrackingMode;
			pfPowerSupply_GetTrackingMode								PowerSupply_GetTrackingMode;
			pfPowerSupply_SetCurrentRange								PowerSupply_SetCurrentRange;              
			pfPowerSupply_GetCurrentRange								PowerSupply_GetCurrentRange; 
			pfPowerSupply_SetRemoteSense								PowerSupply_SetRemoteSense;
			pfPowerSupply_GetRemoteSense								PowerSupply_GetRemoteSense;
			
} tsPowerSupplyFunctions;
		
//**************************************************************************************//

//**************************************************************************************//
//*************************** Spectrum Analyzer Structure ******************************//
//**************************************************************************************//

typedef void* (*pfSpectrumAnalyzer_GetErrorTextMessage) ( int hInstrumentHandle , int iError , char *pErrorMessage );
typedef void* (*pfSpectrumAnalyzer_Init) ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *vhInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChannelsQuantity );
typedef void* (*pfSpectrumAnalyzer_Close) ( int *phInstrumentHandle );  
typedef void* (*pfSpectrumAnalyzer_Reset) ( int hInstrumentHandle ); 
  
typedef void* (*pfSpectrumAnalyzer_SetAuto) ( int hInstrumentHandle , int state );
typedef void* (*pfSpectrumAnalyzer_Config) ( int hInstrumentHandle, int Mode , int Type , int Input , int bBeeperState );
typedef void* (*pfSpectrumAnalyzer_SetFrequency) ( int hInstrumentHandle , double dCenterFreq );
typedef void* (*pfSpectrumAnalyzer_SetStartStopFrequency) ( int hInstrumentHandle , double dStartFreq , double dStopFreq );
typedef void* (*pfSpectrumAnalyzer_GetStartStopFrequency) ( int hInstrumentHandle , double *plfStartFrequency , double *plfStopFrequency );
typedef void* (*pfSpectrumAnalyzer_SetSpan) ( int hInstrumentHandle , double dSpan );
typedef void* (*pfSpectrumAnalyzer_SetAmplitude) ( int hInstrumentHandle , double dScale , double dRefLevel );
typedef void* (*pfSpectrumAnalyzer_SetAttenuation) ( int hInstrumentHandle , double dAttenuation );
typedef void* (*pfSpectrumAnalyzer_SetRBW) ( int hInstrumentHandle , double dRBW );
typedef void* (*pfSpectrumAnalyzer_SetVBW) ( int hInstrumentHandle , double dVBW );
typedef void* (*pfSpectrumAnalyzer_SetBW) ( int hInstrumentHandle , double dRBW, double dVBW );
typedef void* (*pfSpectrumAnalyzer_GetSweep) ( int hInstrumentHandle , double lfTimeout );
typedef void* (*pfSpectrumAnalyzer_SetMarker) ( int hInstrumentHandle , int MarkerSetType , int iMarkerNr ,double dMarkerFreq );
typedef void* (*pfSpectrumAnalyzer_SetRLAuto) ( int hInstrumentHandle );
typedef void* (*pfSpectrumAnalyzer_GetFrequency) ( int hInstrumentHandle , double *pdFrequency );
typedef void* (*pfSpectrumAnalyzer_GetAmplitude) ( int hInstrumentHandle , int MarkerSetType , int iMarkerNr , double *pdAmplitude );
typedef void* (*pfSpectrumAnalyzer_SetFreqCountResol) ( int hInstrumentHandle , BOOL bMode, int iFreqCountResolution );
typedef void* (*pfSpectrumAnalyzer_SetAvg) ( int hInstrumentHandle , BOOL bMode, int iAvg );
typedef void* (*pfSpectrumAnalyzer_SetMaxHold) ( int hInstrumentHandle , int iState );
typedef void* (*pfSpectrumAnalyzer_GetMarkerLevel) ( int hInstrumentHandle , int iMarkerNr , double *pLevel );
typedef void* (*pfSpectrumAnalyzer_GetMarkerFrequency) ( int hInstrumentHandle, int iMarkerNr , double *pFrequency ); 
typedef void* (*pfSpectrumAnalyzer_AutoSetIsDone) ( int hInstrumentHandle ,  double lfTimeOut , int *bDone );
typedef void* (*pfSpectrumAnalyzer_SetAutoTuneThreshold) ( int hInstrumentHandle, double lfValue );
typedef void* (*pfSpectrumAnalyzer_GetTrace) ( int hInstrumentHandle , int timeout , double **pTrace , int numberOfPoints , int *pCount ); 
typedef void* (*pfSpectrumAnalyzer_SetNumberOfPoints) ( int hInstrumentHandle , int points );
typedef void* (*pfSpectrumAnalyzer_SetMode) ( int hInstrumentHandle , int iMode , int iSubMode );  
typedef void* (*pfSpectrumAnalyzer_Recall) ( int hInstrumentHandle , char *szFilePath );  
typedef void* (*pfSpectrumAnalyzer_GetWiMaxMeasure) ( int hInstrumentHandle , int iMeasureType , int iMeasureSubType , int iMeasureValueType , double *plfResult );  
typedef void* (*pfSpectrumAnalyzer_SetSweepTime) ( int hInstrumentHandle , double lfTime );
typedef void* (*pfSpectrumAnalyzer_SetTraceDetector) ( int hInstrumentHandle , int iType );
typedef void* (*pfSpectrumAnalyzer_Preset) ( int hInstrumentHandle);
typedef void* (*pfSpectrumAnalyzer_SetFrequencyCenterSpan) ( int hInstrumentHandle,int channel, double lfFrequency, double lfSpan);
typedef void* (*pfSpectrumAnalyzer_ConfigureMarker) ( int hInstrumentHandle,int channel, int marker, int bState);
typedef void* (*pfSpectrumAnalyzer_SetMarkerSearchPeakAuto) ( int hInstrumentHandle,int channel, int bMax, int bState);
typedef void* (*pfSpectrumAnalyzer_GetMarkerMeasure) ( int hInstrumentHandle,int channel, int marker, double *pFrequency,double *pPower);
typedef void* (*pfSpectrumAnalyzer_ConfigChannelPowerMeasuring) ( int hInstrumentHandle,double lfBandWidth);
typedef void* (*pfSpectrumAnalyzer_GetChannelPowerResult) ( int hInstrumentHandle,double *plfValue);
typedef void* (*pfSpectrumAnalyzer_ConfigBandWidth) ( int hInstrumentHandle,double lfResolutionBandWidth, double lfVideoBandWidth);
typedef void* (*pfSpectrumAnalyzer_ConfigSource) ( int hInstrumentHandle,   int bExternal);
typedef void* (*pfSpectrumAnalyzer_SetScaleDev) ( int hInstrumentHandle,double lfScaleDev);
typedef void* (*pfSpectrumAnalyzer_Attenuator) ( int hInstrumentHandle,double lfAttenuator);
typedef void* (*pfSpectrumAnalyzer_SetReferenceLevel) ( int hInstrumentHandle,double lfReferenceLevel);
typedef void* (*pfSpectrumAnalyzer_GetReferenceLevel) ( int hInstrumentHandle,double *vlfReferenceLevel);
typedef void* (*pfSpectrumAnalyzer_SetMarkerType) ( int hInstrumentHandle,int bDelta);
typedef void* (*pfSpectrumAnalyzer_SetMarkerFrequency) ( int hInstrumentHandle, int iMarkerNr ,double lfFrequency);
typedef void* (*pfSpectrumAnalyzer_SearchMarkerPeak) ( int hInstrumentHandle, int iMarkerNr);
typedef void* (*pfSpectrumAnalyzer_SearchMarkerNextPeak) ( int hInstrumentHandle, int iMarkerNr);
typedef void* (*pfSpectrumAnalyzer_GetMarkerAmplitude) ( int hInstrumentHandle, int iMarkerNr , double *vlfAmplitude);
typedef void* (*pfSpectrumAnalyzer_GetMarkerDeltaFrequency) ( int hInstrumentHandle,double *vlfFrequency);
typedef void* (*pfSpectrumAnalyzer_GetMarkerDeltaAmplitude) ( int hInstrumentHandle,double *vlfAmplitude);
typedef void* (*pfSpectrumAnalyzer_SetCenterFrequencyFromMarker) ( int hInstrumentHandle , int marker );
typedef void* (*pfSpectrumAnalyzer_SetReferenceLevelFrequencyFromMarker) ( int hInstrumentHandle , int marker );
typedef void* (*pfSpectrumAnalyzer_WaitForIntrumentDone) ( int hInstrumentHandle,double lfTimeOut);
typedef void* (*pfSpectrumAnalyzer_SetSweepMode) ( int hInstrumentHandle,int bContinues);
typedef void* (*pfSpectrumAnalyzer_InitiateMeasure) ( int hInstrumentHandle);
typedef void* (*pfSpectrumAnalyzer_SetAverage) ( int hInstrumentHandle, int iType  ,int iAverage);
typedef void* (*pfSpectrumAnalyzer_RecallState) ( int hInstrumentHandle,char *szFileName);
typedef void* (*pfSpectrumAnalyzer_Read_EVM) ( int hInstrumentHandle,double *plfEVM_Value);
typedef void* (*pfSpectrumAnalyzer_SetToMode_Specrum) ( int hInstrumentHandle);
typedef void* (*pfSpectrumAnalyzer_SetFrequency_Specrum) ( int hInstrumentHandle,double lfFrequency);
typedef void* (*pfSpectrumAnalyzer_SetFrequency_EVM) ( int hInstrumentHandle,double lffrequency);
typedef void* (*pfSpectrumAnalyzer_RestartMeasure) ( int hInstrumentHandle);
typedef void* (*pfSpectrumAnalyzer_Read_SymbolStream) ( int hInstrumentHandle,int iNumberOfSymbols, char **pSymbolStream,int *piNumberOfSymbolsRead);
typedef void* (*pfSpectrumAnalyzer_SetMarkerTimePosition) ( int hInstrumentHandle , double lfPosition );
typedef void* (*pfSpectrumAnalyzer_PhaseNoise_GetMarker) ( int hInstrumentHandle , int channel , int marker , double lfTimeOut , double *pPosition , double *pValue );
typedef void* (*pfSpectrumAnalyzer_AutoTune) ( int hInstrumentHandle , double lfTimeout );
typedef void* (*pfSpectrumAnalyzer_SaveState)( int hInstrumentHandle ,char *szFileName ); 
typedef void* (*pfSpectrumAnalyzer_GetLastStateFileName)( int hInstrumentHandle ,char **pszFileName ); 
typedef void* (*pfSpectrumAnalyzer_GetAttenuator) ( int hInstrumentHandle , double *vlfAttenuator );
typedef void* (*pfSpectrumAnalyzer_Read_IP3)( int hInstrumentHandle , double *plfIP3_Value );
typedef void* (*pfSpectrumAnalyzer_Read_IP2)( int hInstrumentHandle , double *plfIP2_Value );  
typedef void* (*pfSpectrumAnalyzer_Read_Harmonics_Distortion)( int hInstrumentHandle , double *plfTHD_Value );  
typedef void* (*pfSpectrumAnalyzer_Read_Harmonics_dBc)( int hInstrumentHandle , double *plfTHD_Value );  
typedef void* (*pfSpectrumAnalyzer_Read_Harmonics_Amplitude)( int hInstrumentHandle , int iHarmonicIndex , double *plfValue );  
typedef void* (*pfSpectrumAnalyzer_Read_Spurious_Emissions_List)( int hInstrumentHandle , int *piNumberOfSpurious , int **pvRangeNumber , int **pvPassFail , double **pvlfSpurFrequency , double **pvlfSpurAmplitude , double **pvlfSpurLimit );  
typedef void* (*pfSpectrumAnalyzer_SetOffset)( int hInstrumentHandle , double lfOffset );  
typedef void* (*pfSpectrumAnalyzer_GetMarkerFrequencyCounter)( int hInstrumentHandle , int iMarkerNr , double *vlfFrequency );  
typedef void* (*pfSpectrumAnalyzer_MeasureMarkerFrequencyCounter)( int hInstrumentHandle , int iMarkerNr , double lfTimeout , double *vlfFrequency );
typedef void* (*pfSpectrumAnalyzer_AlignNow)( int hInstrumentHandle , double lfTimeout );
typedef void* (*pfSpectrumAnalyzer_NoiseFigure_CalibrateNow)( int hInstrumentHandle , double lfTimeout );
typedef void* (*pfSpectrumAnalyzer_NoiseFigure_SetLossCompensationTable)( int hInstrumentHandle , int bAfterDUT , double *pvFrequencies, double *pvLoss , int iNumberOfPoints );
typedef void* (*pfSpectrumAnalyzer_SetMarkerFrequencyCounter)( int hInstrumentHandle , int iMarkerNr , int bState ); 
typedef void* (*pfSpectrumAnalyzer_NoiseFigure_ApplyCalibration)( int hInstrumentHandle , double lfTimeout );
typedef void* (*pfSpectrumAnalyzer_SetTriggerSource)( int hInstrumentHandle , int iSource );  
typedef void* (*pfSpectrumAnalyzer_PhaseNoise_GetIntegratedMarker)( int Handle , int iChannel , int marker , double lfTimeOut ,double lfMarkerStartFreq, double lfMarkerStopFreq, double *pPosition , double *pValue );
typedef void* (*pfSpectrumAnalyzer_PhaseNoise_SetIntegratedMarker_Start_Stop_Offsets)( int Handle , int iChannel , int marker , double lfTimeOut , double lfStartFreq , double lfStopFreq );


typedef struct
{
			pfSTD_Config_Install_CommentCallback								Config_Install_CommentCallback;
			pfSTD_Config_Install_ConfigValueCallback							Config_Install_ConfigValueCallback;
			pfSTD_Config_Install_CheckForBreakCallback							Config_Install_CheckForBreakCallback;
			pfSTD_Config_LOG_SetAllowState										Config_LOG_SetAllowState;
			pfSTD_Config_Copy_STD_CallBackSet									Config_Copy_STD_CallBackSet;
																			
			pfSpectrumAnalyzer_GetErrorTextMessage								SpectrumAnalyzer_GetErrorTextMessage;          
			pfSpectrumAnalyzer_Init						  						SpectrumAnalyzer_Init;                         
			pfSpectrumAnalyzer_Close											SpectrumAnalyzer_Close;  
			pfSpectrumAnalyzer_Reset											SpectrumAnalyzer_Reset;
			pfSpectrumAnalyzer_SetAuto											SpectrumAnalyzer_SetAuto;
			pfSpectrumAnalyzer_Config											SpectrumAnalyzer_Config;
			pfSpectrumAnalyzer_SetFrequency										SpectrumAnalyzer_SetFrequency;
			pfSpectrumAnalyzer_SetStartStopFrequency							SpectrumAnalyzer_SetStartStopFrequency;
			pfSpectrumAnalyzer_GetStartStopFrequency							SpectrumAnalyzer_GetStartStopFrequency;
			pfSpectrumAnalyzer_SetSpan											SpectrumAnalyzer_SetSpan;
			pfSpectrumAnalyzer_SetAmplitude										SpectrumAnalyzer_SetAmplitude;
			pfSpectrumAnalyzer_SetAttenuation									SpectrumAnalyzer_SetAttenuation;
			pfSpectrumAnalyzer_SetRBW											SpectrumAnalyzer_SetRBW;
			pfSpectrumAnalyzer_SetVBW											SpectrumAnalyzer_SetVBW;
			pfSpectrumAnalyzer_SetBW											SpectrumAnalyzer_SetBW;
			pfSpectrumAnalyzer_GetSweep											SpectrumAnalyzer_GetSweep;
			pfSpectrumAnalyzer_SetMarker										SpectrumAnalyzer_SetMarker;
			pfSpectrumAnalyzer_SetRLAuto										SpectrumAnalyzer_SetRLAuto;
			pfSpectrumAnalyzer_GetFrequency								SpectrumAnalyzer_GetFrequency;
			pfSpectrumAnalyzer_GetAmplitude								SpectrumAnalyzer_GetAmplitude;
			pfSpectrumAnalyzer_SetFreqCountResol						SpectrumAnalyzer_SetFreqCountResol;
			pfSpectrumAnalyzer_SetAvg									SpectrumAnalyzer_SetAvg;
			pfSpectrumAnalyzer_SetMaxHold								SpectrumAnalyzer_SetMaxHold;
			pfSpectrumAnalyzer_GetMarkerLevel							SpectrumAnalyzer_GetMarkerLevel;
			pfSpectrumAnalyzer_GetMarkerFrequency						SpectrumAnalyzer_GetMarkerFrequency;
			pfSpectrumAnalyzer_AutoSetIsDone							SpectrumAnalyzer_AutoSetIsDone;
			pfSpectrumAnalyzer_SetAutoTuneThreshold						SpectrumAnalyzer_SetAutoTuneThreshold;
			pfSpectrumAnalyzer_GetTrace									SpectrumAnalyzer_GetTrace;
			pfSpectrumAnalyzer_SetNumberOfPoints						SpectrumAnalyzer_SetNumberOfPoints;
			pfSpectrumAnalyzer_SetMode									SpectrumAnalyzer_SetMode;
			pfSpectrumAnalyzer_Recall									SpectrumAnalyzer_Recall;
			pfSpectrumAnalyzer_GetWiMaxMeasure							SpectrumAnalyzer_GetWiMaxMeasure;
			pfSpectrumAnalyzer_SetSweepTime								SpectrumAnalyzer_SetSweepTime;
			pfSpectrumAnalyzer_SetTraceDetector							SpectrumAnalyzer_SetTraceDetector;
			pfSpectrumAnalyzer_Preset									SpectrumAnalyzer_Preset;
			pfSpectrumAnalyzer_SetFrequencyCenterSpan					SpectrumAnalyzer_SetFrequencyCenterSpan;
			pfSpectrumAnalyzer_ConfigureMarker							SpectrumAnalyzer_ConfigureMarker;                                          
			pfSpectrumAnalyzer_SetMarkerSearchPeakAuto					SpectrumAnalyzer_SetMarkerSearchPeakAuto;                                  
			pfSpectrumAnalyzer_GetMarkerMeasure							SpectrumAnalyzer_GetMarkerMeasure;                                         
			pfSpectrumAnalyzer_ConfigChannelPowerMeasuring				SpectrumAnalyzer_ConfigChannelPowerMeasuring;                             
			pfSpectrumAnalyzer_GetChannelPowerResult					SpectrumAnalyzer_GetChannelPowerResult;                                    
			pfSpectrumAnalyzer_ConfigBandWidth							SpectrumAnalyzer_ConfigBandWidth;                                          
			pfSpectrumAnalyzer_ConfigSource								SpectrumAnalyzer_ConfigSource;                                             
			pfSpectrumAnalyzer_SetScaleDev								SpectrumAnalyzer_SetScaleDev;                                              
			pfSpectrumAnalyzer_Attenuator								SpectrumAnalyzer_Attenuator;                                               
			pfSpectrumAnalyzer_SetReferenceLevel						SpectrumAnalyzer_SetReferenceLevel;                                        
			pfSpectrumAnalyzer_GetReferenceLevel						SpectrumAnalyzer_GetReferenceLevel;                                        
			pfSpectrumAnalyzer_SetMarkerType							SpectrumAnalyzer_SetMarkerType;                                            
			pfSpectrumAnalyzer_SetMarkerFrequency						SpectrumAnalyzer_SetMarkerFrequency;                                       
			pfSpectrumAnalyzer_SearchMarkerPeak							SpectrumAnalyzer_SearchMarkerPeak;                                         
			pfSpectrumAnalyzer_SearchMarkerNextPeak						SpectrumAnalyzer_SearchMarkerNextPeak;                                     
			pfSpectrumAnalyzer_GetMarkerAmplitude						SpectrumAnalyzer_GetMarkerAmplitude;                                       
			pfSpectrumAnalyzer_GetMarkerDeltaFrequency					SpectrumAnalyzer_GetMarkerDeltaFrequency;                                  
			pfSpectrumAnalyzer_GetMarkerDeltaAmplitude					SpectrumAnalyzer_GetMarkerDeltaAmplitude;                                  
			pfSpectrumAnalyzer_SetCenterFrequencyFromMarker				SpectrumAnalyzer_SetCenterFrequencyFromMarker;                             
			pfSpectrumAnalyzer_SetReferenceLevelFrequencyFromMarker 	SpectrumAnalyzer_SetReferenceLevelFrequencyFromMarker;                     
			pfSpectrumAnalyzer_WaitForIntrumentDone						SpectrumAnalyzer_WaitForIntrumentDone;                                     
			pfSpectrumAnalyzer_SetSweepMode								SpectrumAnalyzer_SetSweepMode;                                             
			pfSpectrumAnalyzer_InitiateMeasure							SpectrumAnalyzer_InitiateMeasure;                                          
			pfSpectrumAnalyzer_SetAverage								SpectrumAnalyzer_SetAverage;                                               
			pfSpectrumAnalyzer_RecallState								SpectrumAnalyzer_RecallState;                                              
			pfSpectrumAnalyzer_Read_EVM									SpectrumAnalyzer_Read_EVM;                                                 
			pfSpectrumAnalyzer_SetToMode_Specrum						SpectrumAnalyzer_SetToMode_Specrum;                                        
			pfSpectrumAnalyzer_SetFrequency_Specrum						SpectrumAnalyzer_SetFrequency_Specrum;                                     
			pfSpectrumAnalyzer_SetFrequency_EVM							SpectrumAnalyzer_SetFrequency_EVM;                                         
			pfSpectrumAnalyzer_RestartMeasure							SpectrumAnalyzer_RestartMeasure;                                           
			pfSpectrumAnalyzer_Read_SymbolStream						SpectrumAnalyzer_Read_SymbolStream;                                        
			pfSpectrumAnalyzer_SetMarkerTimePosition					SpectrumAnalyzer_SetMarkerTimePosition;
			pfSpectrumAnalyzer_PhaseNoise_GetMarker						SpectrumAnalyzer_PhaseNoise_GetMarker;	
			pfSpectrumAnalyzer_AutoTune									SpectrumAnalyzer_AutoTune;
			pfSpectrumAnalyzer_SaveState								SpectrumAnalyzer_SaveState;
			pfSpectrumAnalyzer_GetLastStateFileName						SpectrumAnalyzer_GetLastStateFileName;
			
			//------------ Version 1.1.0.0 --------------------//  
			pfSpectrumAnalyzer_GetAttenuator							SpectrumAnalyzer_GetAttenuator;
			
			//------------ Version 1.2.2.0 --------------------// 
			pfSpectrumAnalyzer_Read_IP3									SpectrumAnalyzer_Read_IP3;                    
			pfSpectrumAnalyzer_Read_IP2									SpectrumAnalyzer_Read_IP2;                    
			pfSpectrumAnalyzer_Read_Harmonics_Distortion				SpectrumAnalyzer_Read_Harmonics_Distortion;   
			pfSpectrumAnalyzer_Read_Harmonics_dBc						SpectrumAnalyzer_Read_Harmonics_dBc;          
			pfSpectrumAnalyzer_Read_Harmonics_Amplitude					SpectrumAnalyzer_Read_Harmonics_Amplitude;    
			pfSpectrumAnalyzer_Read_Spurious_Emissions_List					SpectrumAnalyzer_Read_Spurious_Emissions_List;
			
			//------------ Version 1.2.3.0 --------------------// 
			pfSpectrumAnalyzer_SetOffset											SpectrumAnalyzer_SetOffset;
		
			//------------ Version 1.2.4.0 --------------------// 
			pfSpectrumAnalyzer_GetMarkerFrequencyCounter							SpectrumAnalyzer_GetMarkerFrequencyCounter;
			
			//------------ Version 1.2.6.0 --------------------// 
			pfSpectrumAnalyzer_MeasureMarkerFrequencyCounter						SpectrumAnalyzer_MeasureMarkerFrequencyCounter;
																														   
			//------------ Version 1.3.5.0 --------------------//  
			pfSpectrumAnalyzer_AlignNow												SpectrumAnalyzer_AlignNow;
			pfSpectrumAnalyzer_NoiseFigure_CalibrateNow								SpectrumAnalyzer_NoiseFigure_CalibrateNow;
			pfSpectrumAnalyzer_NoiseFigure_SetLossCompensationTable					SpectrumAnalyzer_NoiseFigure_SetLossCompensationTable;

			//------------ Version 1.6.2.0 --------------------//  
			pfSpectrumAnalyzer_SetMarkerFrequencyCounter							SpectrumAnalyzer_SetMarkerFrequencyCounter;

			//------------ Version 1.8.3.0 --------------------//  
			pfSpectrumAnalyzer_NoiseFigure_ApplyCalibration							SpectrumAnalyzer_NoiseFigure_ApplyCalibration;
		
			//------------ Version 1.8.5.0 --------------------//  
			pfSpectrumAnalyzer_SetTriggerSource										SpectrumAnalyzer_SetTriggerSource;
		
			//------------ Version 1.8.7.0 --------------------//  
			pfSpectrumAnalyzer_PhaseNoise_GetIntegratedMarker						SpectrumAnalyzer_PhaseNoise_GetIntegratedMarker;
			pfSpectrumAnalyzer_PhaseNoise_SetIntegratedMarker_Start_Stop_Offsets	SpectrumAnalyzer_PhaseNoise_SetIntegratedMarker_Start_Stop_Offsets
			
} tsSpectrumAnalyzerFunctions;

//**************************************************************************************//

//**************************************************************************************//
//******************************** Oscilloscope Structure ******************************//
//**************************************************************************************//

typedef void* (*pfOscilloscope_GetErrorTextMessage) ( int handle , int iError , char *pErrorMessage );
typedef void* (*pfOscilloscope_Init) ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *InstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChanelsQuantity );
typedef void* (*pfOscilloscope_Close) ( int *phInstrumentHandle );
typedef void* (*pfOscilloscope_Reset) ( int hInstrumentHandle );
typedef void* (*pfOscilloscope_AutoSetup) ( int hInstrumentHandle );
typedef void* (*pfOscilloscope_ChannelOnOff) ( int hInstrumentHandle , int iChannel , int OnOff );
typedef void* (*pfOscilloscope_ChannelVertical) ( int hInstrumentHandle , int iChannel, double dScale , double dOffset , int iUnits , int iCouping , double lfProbeAttenuation , double lfImpedance );
typedef void* (*pfOscilloscope_Horizontal) ( int hInstrumentHandle , double dScale , double dOffset );
typedef void* (*pfOscilloscope_SweepContinuous) ( int hInstrumentHandle ,  int iState );
typedef void* (*pfOscilloscope_InitTrigger) ( int hInstrumentHandle );
typedef void* (*pfOscilloscope_WaitForEvent) ( int hInstrumentHandle , double lfTimeOut );
typedef void* (*pfOscilloscope_TriggerConfig) ( int hInstrumentHandle , int iChannel , double lfTrigerLevel , int bPositiveEdge , double lfHoldTime , int iModeAuto );
typedef void* (*pfOscilloscope_GetWaveForm) ( int hInstrumentHandle , int iChannel, int iSize, double lfTimeOut, double *dpMeasureArray, int *ipCount, double *dpX_init, double *dpX_inc );
typedef void* (*pfOscilloscope_MeasureRiseTime) ( int hInstrumentHandle , int iChannel, double lfTimeOut, double *dpValue );         
typedef void* (*pfOscilloscope_MeasureFallTime) ( int hInstrumentHandle , int iChannel, double lfTimeOut, double *dpValue );         
typedef void* (*pfOscilloscope_MeasureFrequency) ( int hInstrumentHandle , int iChannel, double lfTimeOut, double *dpValue );        
typedef void* (*pfOscilloscope_MeasurePeriod) ( int hInstrumentHandle , int iChannel, double lfTimeOut, double *dpValue );           
typedef void* (*pfOscilloscope_MeasureVoltageRMS) ( int hInstrumentHandle , int iChannel, double lfTimeOut, double *dpValue );        
typedef void* (*pfOscilloscope_MeasureVoltagePk2Pk) ( int hInstrumentHandle , int iChannel, double lfTimeOut, double *dpValue );     	
typedef void* (*pfOscilloscope_MeasureVoltageMax) ( int hInstrumentHandle , int iChannel, double lfTimeOut, double *dpValue );       
typedef void* (*pfOscilloscope_MeasureVoltageMin) ( int hInstrumentHandle , int iChannel, double lfTimeOut, double *dpValue );       
typedef void* (*pfOscilloscope_MeasureVoltageHigh) ( int hInstrumentHandle , int iChannel, double lfTimeOut, double *dpValue );      
typedef void* (*pfOscilloscope_MeasureVoltageLow) ( int hInstrumentHandle , int iChannel, double lfTimeOut, double *dpValue );       
typedef void* (*pfOscilloscope_MeasureVoltageAverage) ( int hInstrumentHandle , int iChannel, double lfTimeOut, double *dpValue );   
typedef void* (*pfOscilloscope_MeasureWidthNegative) ( int hInstrumentHandle , int iChannel, double lfTimeOut, double *dpValue );    		
typedef void* (*pfOscilloscope_MeasureWidthPositive) ( int hInstrumentHandle , int iChannel, double lfTimeOut, double *dpValue );
typedef void* (*pfOscilloscope_MeasureDutyCycleNegative) ( int hInstrumentHandle , int iChannel, double lfTimeOut, double *dpValue );
typedef void* (*pfOscilloscope_MeasureDutyCyclePositive) ( int hInstrumentHandle , int iChannel, double lfTimeOut, double *dpValue );
typedef void* (*pfOscilloscope_MeasureAmplitude) ( int hInstrumentHandle , int iChannel, double lfTimeOut, double *dpValue );
typedef void* (*pfOscilloscope_MeasureVoltageCycleRMS) ( int hInstrumentHandle , int iChannel, double lfTimeOut, double *dpValue );     
typedef void* (*pfOscilloscope_MeasureVoltageCycleAverage) ( int hInstrumentHandle , int iChannel, double lfTimeOut, double *dpValue ); 
typedef void* (*pfOscilloscope_MeasurePositiveOvershoot) ( int hInstrumentHandle , int iChannel, double lfTimeOut, double *dpValue );
typedef void* (*pfOscilloscope_MeasureNegativeOvershoot) ( int hInstrumentHandle , int iChannel, double lfTimeOut, double *dpValue );
typedef void* (*pfOscilloscope_MeasureTimingBurstWidth) ( int hInstrumentHandle , int iChannel, double lfTimeOut, double *dpValue ); 	
typedef void* (*pfOscilloscope_MemoryState_Save) ( int hInstrumentHandle , char *pszFileName );
typedef void* (*pfOscilloscope_MemoryState_Recall) ( int hInstrumentHandle , char *pszFileName );	
typedef void* (*pfOscilloscope_HorizontalOffset) ( int hInstrumentHandle , double lfOffset );  
typedef void* (*pfOscilloscope_MeasureDelay) ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue );  
typedef void* (*pfOscilloscope_ConfigMeasureDelay) ( int hInstrumentHandle , int iSourceChannel , int iSourceSlope , int iSourceSpare, int iMeasureChannel , int iMeasureSlope , int iMeasureSpare );
	
typedef struct
{
			pfSTD_Config_Install_CommentCallback						Config_Install_CommentCallback;
			pfSTD_Config_Install_ConfigValueCallback					Config_Install_ConfigValueCallback;
			pfSTD_Config_Install_CheckForBreakCallback					Config_Install_CheckForBreakCallback;
			pfSTD_Config_LOG_SetAllowState								Config_LOG_SetAllowState;
			pfSTD_Config_Copy_STD_CallBackSet							Config_Copy_STD_CallBackSet;
			
			pfOscilloscope_GetErrorTextMessage							Oscilloscope_GetErrorTextMessage;
			pfOscilloscope_Init											Oscilloscope_Init;					
			pfOscilloscope_Close										Oscilloscope_Close;					  
			pfOscilloscope_Reset										Oscilloscope_Reset;					
			pfOscilloscope_AutoSetup									Oscilloscope_AutoSetup;					
			pfOscilloscope_ChannelOnOff									Oscilloscope_ChannelOnOff;					        
			pfOscilloscope_ChannelVertical								Oscilloscope_ChannelVertical;			  	
			pfOscilloscope_Horizontal									Oscilloscope_Horizontal;			
			pfOscilloscope_SweepContinuous								Oscilloscope_SweepContinuous;				
			pfOscilloscope_InitTrigger			  						Oscilloscope_InitTrigger;			   
			pfOscilloscope_WaitForEvent									Oscilloscope_WaitForEvent;           
			pfOscilloscope_TriggerConfig								Oscilloscope_TriggerConfig;          
			pfOscilloscope_GetWaveForm									Oscilloscope_GetWaveForm;            
			pfOscilloscope_MeasureRiseTime          					Oscilloscope_MeasureRiseTime;          
			pfOscilloscope_MeasureFallTime          					Oscilloscope_MeasureFallTime;          
			pfOscilloscope_MeasureFrequency         					Oscilloscope_MeasureFrequency;        
			pfOscilloscope_MeasurePeriod            					Oscilloscope_MeasurePeriod;           
			pfOscilloscope_MeasureVoltageRMS        					Oscilloscope_MeasureVoltageRMS;        
			pfOscilloscope_MeasureVoltagePk2Pk      					Oscilloscope_MeasureVoltagePk2Pk;     
			pfOscilloscope_MeasureVoltageMax        					Oscilloscope_MeasureVoltageMax;       
			pfOscilloscope_MeasureVoltageMin        					Oscilloscope_MeasureVoltageMin;       
			pfOscilloscope_MeasureVoltageHigh       					Oscilloscope_MeasureVoltageHigh;      
			pfOscilloscope_MeasureVoltageLow        					Oscilloscope_MeasureVoltageLow;       
			pfOscilloscope_MeasureVoltageAverage    					Oscilloscope_MeasureVoltageAverage;   
			pfOscilloscope_MeasureWidthNegative     					Oscilloscope_MeasureWidthNegative;    
			pfOscilloscope_MeasureWidthPositive     					Oscilloscope_MeasureWidthPositive;    
			pfOscilloscope_MeasureDutyCycleNegative						Oscilloscope_MeasureDutyCycleNegative;
			pfOscilloscope_MeasureDutyCyclePositive						Oscilloscope_MeasureDutyCyclePositive;
			pfOscilloscope_MeasureAmplitude								Oscilloscope_MeasureAmplitude;
			pfOscilloscope_MeasureVoltageCycleRMS   					Oscilloscope_MeasureVoltageCycleRMS;     
			pfOscilloscope_MeasureVoltageCycleAverage 					Oscilloscope_MeasureVoltageCycleAverage; 
			pfOscilloscope_MeasurePositiveOvershoot 					Oscilloscope_MeasurePositiveOvershoot;
			pfOscilloscope_MeasureNegativeOvershoot 					Oscilloscope_MeasureNegativeOvershoot;
			pfOscilloscope_MeasureTimingBurstWidth  					Oscilloscope_MeasureTimingBurstWidth; 
			pfOscilloscope_MemoryState_Save								Oscilloscope_MemoryState_Save;
			pfOscilloscope_MemoryState_Recall							Oscilloscope_MemoryState_Recall;
			pfOscilloscope_HorizontalOffset								Oscilloscope_HorizontalOffset;
			pfOscilloscope_MeasureDelay									Oscilloscope_MeasureDelay;
			pfOscilloscope_ConfigMeasureDelay							Oscilloscope_ConfigMeasureDelay;
			
} tsOscilloscopeFunctions;

//**************************************************************************************//

//**************************************************************************************//
//**************************** Signal Generator Structure ******************************//
//**************************************************************************************//

typedef void* (*pfSignalGenerator_GetErrorTextMessage) ( int handle , int iError , char *pErrorMessage );
typedef void* (*pfSignalGenerator_Init) ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *InstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChanelsQuantity );
typedef void* (*pfSignalGenerator_Close) ( int *pInstrumentHandle );
typedef void* (*pfSignalGenerator_Reset) ( int hInstrumentHandle );
typedef void* (*pfSignalGenerator_RF_Off) ( int hInstrumentHandle , int channel );
typedef void* (*pfSignalGenerator_RF_On) ( int hInstrumentHandle , int channel );
typedef void* (*pfSignalGenerator_Config) ( int hInstrumentHandle , int channel , double dFrequency , double dAmplitude );
typedef void* (*pfSignalGenerator_SetFrequency) ( int hInstrumentHandle , int channel , double dFrequency );
typedef void* (*pfSignalGenerator_SetPower) ( int hInstrumentHandle , int channel , double dPower );
typedef void* (*pfSignalGenerator_SetModulation) ( int hInstrumentHandle , int channel , int iSource , double lfModulationIndex , double lfFrequency );
typedef void* (*pfSignalGenerator_SetModulationType) ( int hInstrumentHandle , int iChannel , int iModulationType );
typedef void* (*pfSignalGenerator_SetRfOutPort) ( int hInstrumentHandle , int iChannel , int iPort );
typedef void* (*pfSignalGenerator_SetFunctionGen) ( int hInstrumentHandle , int iChannel , int iFunctionNum , int bState , double lfRmsLevel , int bUnitVolt , int bWaveFormSine , double lfFrequency );
typedef void* (*pfSignalGenerator_SetInOut) ( int hInstrumentHandle , int iChannel , int iInput , int bInImpedance , int iOutput );
typedef void* (*pfSignalGenerator_DisableModulation) ( int hInstrumentHandle , int iChannel , int iSource ); 
typedef void* (*pfSignalGenerator_DisableFunctionGen) ( int hInstrumentHandle , int iChannel , int iFunctionNum  );
typedef void* (*pfSignalGenerator_SetOffSetAttenuation) ( int hInstrumentHandle , int iChannel , double lfAttenuation );
typedef void* (*pfSignalGenerator_GetLastStateFileName)( int hInstrumentHandle ,char **pszFileName ); 
typedef void* (*pfSignalGenerator_RecallState) ( int hInstrumentHandle,char *szFileName);
typedef void* (*pfSignalGenerator_Modulation_Off) ( int hInstrumentHandle , int channel );
typedef void* (*pfSignalGenerator_Modulation_On) ( int hInstrumentHandle , int channel );

typedef struct
{
			pfSTD_Config_Install_CommentCallback						Config_Install_CommentCallback;
			pfSTD_Config_Install_ConfigValueCallback					Config_Install_ConfigValueCallback;
			pfSTD_Config_Install_CheckForBreakCallback					Config_Install_CheckForBreakCallback;
			pfSTD_Config_LOG_SetAllowState								Config_LOG_SetAllowState;
			pfSTD_Config_Copy_STD_CallBackSet							Config_Copy_STD_CallBackSet;
			
			pfSignalGenerator_GetErrorTextMessage						SignalGenerator_GetErrorTextMessage;      
			pfSignalGenerator_Init										SignalGenerator_Init;					  
			pfSignalGenerator_Close										SignalGenerator_Close;					  
			pfSignalGenerator_Reset										SignalGenerator_Reset;					
			pfSignalGenerator_RF_Off									SignalGenerator_RF_Off;					
			pfSignalGenerator_RF_On										SignalGenerator_RF_On;					        
			pfSignalGenerator_Config			  						SignalGenerator_Config;			  	
			pfSignalGenerator_SetFrequency								SignalGenerator_SetFrequency;			
			pfSignalGenerator_SetPower									SignalGenerator_SetPower;				
			pfSignalGenerator_SetModulation			  					SignalGenerator_SetModulation;			   
			pfSignalGenerator_SetModulationType							SignalGenerator_SetModulationType;
			pfSignalGenerator_SetRfOutPort								SignalGenerator_SetRfOutPort;
			pfSignalGenerator_SetFunctionGen							SignalGenerator_SetFunctionGen;
			pfSignalGenerator_SetInOut									SignalGenerator_SetInOut;
			pfSignalGenerator_DisableModulation							SignalGenerator_DisableModulation;
			pfSignalGenerator_DisableFunctionGen						SignalGenerator_DisableFunctionGen;
			pfSignalGenerator_SetOffSetAttenuation						SignalGenerator_SetOffSetAttenuation;

			//------------ Version 1.1.0.0 --------------------//  
			pfSignalGenerator_GetLastStateFileName						SignalGenerator_GetLastStateFileName;	
			pfSignalGenerator_RecallState								SignalGenerator_RecallState;
			
			//------------ Version 1.2.7.0 --------------------//  
			pfSignalGenerator_Modulation_Off							SignalGenerator_Modulation_Off;					
			pfSignalGenerator_Modulation_On								SignalGenerator_Modulation_On;	
																									
} tsSignalGeneratorFunctions;

//**************************************************************************************//

//**************************************************************************************//
//**************************** Wave Analyzer Structure ******************************//
//**************************************************************************************//

typedef void* (*pfWaveAnalyzer_GetErrorTextMessage) ( int handle , int iError , char *pErrorMessage );
typedef void* (*pfWaveAnalyzer_Init) ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *InstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChanelsQuantity );
typedef void* (*pfWaveAnalyzer_Close) ( int *pInstrumentHandle );
typedef void* (*pfWaveAnalyzer_Reset) ( int hInstrumentHandle );
typedef void* (*pfWaveAnalyzer_Config) ( int hInstrumentHandle , int iReciverType , double lfBandWidth , int bAmpifire , int bAgcAuto , double lfAgcLevel , int iAverage , int bMeasureType );
typedef void* (*pfWaveAnalyzer_GetModulationMeasure) ( int hInstrumentHandle , int iModulation , double *vlfAudioFreq , double *vlfAudioAmpliduteMV , double *vlfModRiple , double *vlfModDiv , double *vlfNegPeak , double *vlfRMS , double *vlfModuleAudioFreq );
typedef void* (*pfWaveAnalyzer_SetAudioUnits) ( int hInstrumentHandle , int iUnits );
typedef void* (*pfWaveAnalyzer_SetFilter) ( int hInstrumentHandle , int iAufioType , int iFilterType , int iPsophType );

typedef struct
{
			pfSTD_Config_Install_CommentCallback						Config_Install_CommentCallback;
			pfSTD_Config_Install_ConfigValueCallback					Config_Install_ConfigValueCallback;
			pfSTD_Config_Install_CheckForBreakCallback					Config_Install_CheckForBreakCallback;
			pfSTD_Config_LOG_SetAllowState								Config_LOG_SetAllowState;
			pfSTD_Config_Copy_STD_CallBackSet							Config_Copy_STD_CallBackSet;
			
			pfWaveAnalyzer_GetErrorTextMessage							WaveAnalyzer_GetErrorTextMessage;      
			pfWaveAnalyzer_Init											WaveAnalyzer_Init;					  
			pfWaveAnalyzer_Close										WaveAnalyzer_Close;					  
			pfWaveAnalyzer_Reset										WaveAnalyzer_Reset;					
			pfWaveAnalyzer_Config										WaveAnalyzer_Config;
			pfWaveAnalyzer_GetModulationMeasure							WaveAnalyzer_GetModulationMeasure;
			pfWaveAnalyzer_SetAudioUnits								WaveAnalyzer_SetAudioUnits;
			pfWaveAnalyzer_SetFilter									WaveAnalyzer_SetFilter;
																								
} tsWaveAnalyzerFunctions;

//**************************************************************************************//

//**************************************************************************************//
//********************************* Network Analyzer Structure ******************************//
//**************************************************************************************//
		
typedef void* (*pfNetworkAnalyzer_GetErrorTextMessage)( int hInstrumentHandle , int iError , char *pErrorMessage );
typedef void* (*pfNetworkAnalyzer_Init) ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *phInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChanelsQuantity );
typedef void* (*pfNetworkAnalyzer_Close) ( int *phInstrumentHandle );
typedef void* (*pfNetworkAnalyzer_Reset) ( int hInstrumentHandle );
typedef void* (*pfNetworkAnalyzer_GetConnectedPort) ( int hInstrumentHandle , int *iPort );
typedef void* (*pfNetworkAnalyzer_Calibration_SetFrequencyPlan) ( int hInstrumentHandle , double lfStartFrequncy , double lfStopFrequency , double lfPowerLevel , double lfReferenceLevel , int iNumberOfPoints );
typedef void* (*pfNetworkAnalyzer_SelectCalibrationFromFile) ( int hInstrumentHandle , char *pFileName );
typedef void* (*pfNetworkAnalyzer_Calibration_Measure) ( int hInstrumentHandle , int iCalibrationType , int iCalibrationStandard );
typedef void* (*pfNetworkAnalyzer_Calibration_Apply) ( int hInstrumentHandle , int iCalibrationType );
typedef void* (*pfNetworkAnalyzer_Kit_SelectFromFile) ( int hInstrumentHandle , char *szFileName , int port );
typedef void* (*pfNetworkAnalyzer_MeasureSingleSweep) ( int hInstrumentHandle , int iMeasurementType );
typedef void* (*pfNetworkAnalyzer_SetAverage) ( int hInstrumentHandle , int iAverage );
typedef void* (*pfNetworkAnalyzer_SetSmoothing) ( int hInstrumentHandle , int iSmoothing );
typedef void* (*pfNetworkAnalyzer_SetDwellTime) ( int hInstrumentHandle , double lfDwellTime );
typedef void* (*pfNetworkAnalyzer_SetPowerLevel) ( int hInstrumentHandle , int iPowerLevel );
typedef void* (*pfNetworkAnalyzer_SetReferencePlane) ( int hInstrumentHandle , int iMeasurementType , int iValue , int bAuto );
typedef void* (*pfNetworkAnalyzer_SaveMeasurementToMemory) ( int hInstrumentHandle , int iMeasurementType );
typedef void* (*pfNetworkAnalyzer_ApplyMemoryMath) ( int hInstrumentHandle , int iMeasurementType , int iFunction );
typedef void* (*pfNetworkAnalyzer_SetSystemImpedance) ( int hInstrumentHandle , int iSystemImpedance , int bExternalMatch);
typedef void* (*pfNetworkAnalyzer_GetData) ( int hInstrumentHandle , int iMeasurementType , int iMeasurementFormat , int iFrequencyIndex , double *plfFrequency , double *plfValue );
typedef void* (*pfNetworkAnalyzer_GetMemory) ( int hInstrumentHandle , int iMeasurementType , int iMeasurementFormat , int iFrequencyIndex , double *plfFrequency , double *plfValue );
typedef void* (*pfNetworkAnalyzer_FindDataPoint) ( int hInstrumentHandle , double lfFrequency , int iMeasurementType , int iMeasurementFormat , int iMeasurementFunction , double *plfFrequency , double *plfValue );
typedef void* (*pfNetworkAnalyzer_GetTrace) ( int hInstrumentHandle , int iChannel , int iMeasurementType , int iMeasurementFormat , double lfTimeout ,  double **plfFrequency , double **plfTrace , int *piNumberOfPoints );
typedef void* (*pfNetworkAnalyzer_GetInfo) ( int hInstrumentHandle , int iInfoType , char *pInfo );
typedef void* (*pfNetworkAnalyzer_SaveStateAndCalibration) ( int hInstrumentHandle , char *pFileName );
typedef void* (*pfNetworkAnalyzer_LoadStateAndCalibration) ( int hInstrumentHandle , char *pFileName );

typedef void* (*pfNetworkAnalyzer_SetFrequency)( int hInstrumentHandle , double lfStartFrequency ,  double lfStopFrequency ); 
typedef void* (*pfNetworkAnalyzer_GetFrequency)( int hInstrumentHandle , double *plfStartFrequency ,  double *plfStopFrequency );
typedef void* (*pfNetworkAnalyzer_GetMeasureVectors)(int hInstrumentHandle, int iGetDataType , double **plfFirstVector , double **plfSecondVector , double **plfThirdVector , double **plfFourthVector );
typedef void* (*pfNetworkAnalyzer_SetMeasureType)(int hInstrumentHandle, int iChannel, int iInSparam, int iConversation);
typedef void* (*pfNetworkAnalyzer_DisplaySelect)(int hInstrumentHandle, int iChannel, int iDataMode , int iDataFormat );
typedef void* (*pfNetworkAnalyzer_AutoScale)(int hInstrumentHandle );
typedef void* (*pfNetworkAnalyzer_AvarageSmooth)(int hInstrumentHandle, int iChannel, char *szSmoothRequest , double lfSetSmooth , double lfCurrentSmooth );
typedef void* (*pfNetworkAnalyzer_Smooth)(int hInstrumentHandle, int iChannel, int iState , double lfSetSmooth , int iNumberPoints );
typedef void* (*pfNetworkAnalyzer_SetBandWidth)(int hInstrumentHandle, int iChannel , double  lfBW_Value , char *szBW_Request , double  *lfGetBW_Value );
typedef void* (*pfNetworkAnalyzer_SetFrequencySweepType)(int hInstrumentHandle, int iChannel , int iFrequencySweepType ); 
typedef void* (*pfNetworkAnalyzer_SetPower)(int hInstrumentHandle, int iChannel , double lfPower , char *szPowerRequest , int iPortNum );
typedef void* (*pfNetworkAnalyzer_GetPower)(int hInstrumentHandle, int iChannel , double *lfPower , int iPortNum );
typedef void* (*pfNetworkAnalyzer_SetPowerRange)(int hInstrumentHandle, int iChannel , int iRange , char *szRangeRequest , int *iCurrentRange );
typedef void* (*pfNetworkAnalyzer_SetPowerSweep)(int hInstrumentHandle, int iChannel , double lfStartPower , double lfStopPower , char *szSweepRequest , double *lfGetStartPower , double *lfGetStopPower );
typedef void* (*pfNetworkAnalyzer_SetSweepTime)(int hInstrumentHandle, int iChannel , double lfSweepTime , char *szSweepTimeRequest , double *lfGetSweepTime );
typedef void* (*pfNetworkAnalyzer_SetMarker)(int hInstrumentHandle, int iChannel , double lfSetValue , int iMarker , char *szMarkerRequest );
typedef void* (*pfNetworkAnalyzer_GetMarker)(int hInstrumentHandle, int iChannel , int iMarker , char *szMarkerRequest , double *lfGetMarkerOneValue  , double *lfGetMarkerTwoValue , double *lfGetMarkerThreeValue , double *lfGetMarkerFourValue );
typedef void* (*pfNetworkAnalyzer_ReCallRegister)(int hInstrumentHandle , char *szRegister_FileName );
typedef void* (*pfNetworkAnalyzer_SaveToRegister)(int hInstrumentHandle, char *pszTitle , int iRegister );
typedef void* (*pfNetworkAnalyzer_QuaryCalibration)(int hInstrumentHandle , int iChannel  , char *pResponse  );
typedef void* (*pfNetworkAnalyzer_SetPointsNum)( int hInstrumentHandle , int points );  
typedef void* (*pfNetworkAnalyzer_GetPointsNum)(int hInstrumentHandle, int *points);    
typedef void* (*pfNetworkAnalyzer_SelectChannel) ( int hInstrumentHandle , int iChannel );
typedef void* (*pfNetworkAnalyzer_GetLastStateFileName)(int hInstrumentHandle, char **pszFileName );
typedef void* (*pfNetworkAnalyzer_SelectMathFunction) ( int hInstrumentHandle , int iChannel , char cFunction );
typedef void* (*pfNetworkAnalyzer_SetTraceToMathMemory) ( int hInstrumentHandle , int iChannel );
typedef void* (*pfNetworkAnalyzer_ECAL_GetInformation)(int hInstrumentHandle, char **pszCatalog );
typedef void* (*pfNetworkAnalyzer_ECAL_Calibrate)(int hInstrumentHandle, char *pszPortList , char *pUserCal , double lfTimeout );
typedef void* (*pfNetworkAnalyzer_SetElectricalDelayTime)( int hInstrumentHandle , int iChannel , double lfTime );
typedef void* (*pfNetworkAnalyzer_SelectTrace)( int hInstrumentHandle , iTraceNumber );
typedef void* (*pfNetworkAnalyzer_SelectWindow)( int hInstrumentHandle , iWindowNumber );   
typedef void* (*pfNetworkAnalyzer_Avarage)(int hInstrumentHandle, int iChannel, int bEnable , int iAverageValue );
typedef void* (*pfNetworkAnalyzer_ClearAvarage)(int hInstrumentHandle, int iChannel );
typedef void* (*pfNetworkAnalyzer_Reponse_OpenShortLoad_Calibrate)(int hInstrumentHandle , int iType_OSL , char *pszPortList , double lfTimeout );
typedef void* (*pfNetworkAnalyzer_Reponse_Thru_Calibrate)(int hInstrumentHandle, char *pszPortList , double lfTimeout );
typedef void* (*pfNetworkAnalyzer_Reponse_Done_Calibrate)(int hInstrumentHandle );
typedef void* (*pfNetworkAnalyzer_WaitSweep)(int hInstrumentHandle, int iChannel , int iCount );
typedef void* (*pfNetworkAnalyzer_SetCalibrationName)(int hInstrumentHandle, char *pszName );

typedef struct
{
			pfSTD_Config_Install_CommentCallback						Config_Install_CommentCallback;
			pfSTD_Config_Install_ConfigValueCallback					Config_Install_ConfigValueCallback;
			pfSTD_Config_Install_CheckForBreakCallback					Config_Install_CheckForBreakCallback;
			pfSTD_Config_LOG_SetAllowState								Config_LOG_SetAllowState;
			pfSTD_Config_Copy_STD_CallBackSet							Config_Copy_STD_CallBackSet;
			
			pfNetworkAnalyzer_GetErrorTextMessage						NetworkAnalyzer_GetErrorTextMessage;  
			pfNetworkAnalyzer_Init										NetworkAnalyzer_Init;                         
			pfNetworkAnalyzer_Close										NetworkAnalyzer_Close;
			pfNetworkAnalyzer_Reset										NetworkAnalyzer_Reset;                        
			pfNetworkAnalyzer_GetConnectedPort							NetworkAnalyzer_GetConnectedPort;
			pfNetworkAnalyzer_Calibration_SetFrequencyPlan				NetworkAnalyzer_Calibration_SetFrequencyPlan; 
			pfNetworkAnalyzer_SelectCalibrationFromFile					NetworkAnalyzer_SelectCalibrationFromFile;    
			pfNetworkAnalyzer_Calibration_Measure						NetworkAnalyzer_Calibration_Measure;          
			pfNetworkAnalyzer_Calibration_Apply							NetworkAnalyzer_Calibration_Apply;            
			pfNetworkAnalyzer_Kit_SelectFromFile						NetworkAnalyzer_Kit_SelectFromFile;
			pfNetworkAnalyzer_MeasureSingleSweep						NetworkAnalyzer_MeasureSingleSweep;           
			pfNetworkAnalyzer_SetAverage								NetworkAnalyzer_SetAverage;                   
			pfNetworkAnalyzer_SetSmoothing								NetworkAnalyzer_SetSmoothing;                 
			pfNetworkAnalyzer_SetDwellTime								NetworkAnalyzer_SetDwellTime;                 
			pfNetworkAnalyzer_SetPowerLevel								NetworkAnalyzer_SetPowerLevel;                
			pfNetworkAnalyzer_SetReferencePlane							NetworkAnalyzer_SetReferencePlane;            
			pfNetworkAnalyzer_SaveMeasurementToMemory					NetworkAnalyzer_SaveMeasurementToMemory;      
			pfNetworkAnalyzer_ApplyMemoryMath							NetworkAnalyzer_ApplyMemoryMath;              
			pfNetworkAnalyzer_SetSystemImpedance						NetworkAnalyzer_SetSystemImpedance;           
			pfNetworkAnalyzer_GetData									NetworkAnalyzer_GetData;                      
			pfNetworkAnalyzer_GetMemory									NetworkAnalyzer_GetMemory;                    
			pfNetworkAnalyzer_FindDataPoint								NetworkAnalyzer_FindDataPoint;                
			pfNetworkAnalyzer_GetTrace									NetworkAnalyzer_GetTrace;
			pfNetworkAnalyzer_GetInfo									NetworkAnalyzer_GetInfo;                      
			pfNetworkAnalyzer_SaveStateAndCalibration					NetworkAnalyzer_SaveStateAndCalibration;     
			pfNetworkAnalyzer_LoadStateAndCalibration					NetworkAnalyzer_LoadStateAndCalibration;     
			pfNetworkAnalyzer_SetFrequency								NetworkAnalyzer_SetFrequency;
			pfNetworkAnalyzer_GetFrequency								NetworkAnalyzer_GetFrequency;
			pfNetworkAnalyzer_GetMeasureVectors							NetworkAnalyzer_GetMeasureVectors;
			pfNetworkAnalyzer_SetMeasureType							NetworkAnalyzer_SetMeasureType;
			pfNetworkAnalyzer_DisplaySelect								NetworkAnalyzer_DisplaySelect;
			pfNetworkAnalyzer_AutoScale									NetworkAnalyzer_AutoScale;
			pfNetworkAnalyzer_AvarageSmooth								NetworkAnalyzer_AvarageSmooth;
			pfNetworkAnalyzer_Smooth									NetworkAnalyzer_Smooth;
			pfNetworkAnalyzer_SetBandWidth								NetworkAnalyzer_SetBandWidth;
			pfNetworkAnalyzer_SetPower									NetworkAnalyzer_SetPower;
			pfNetworkAnalyzer_GetPower									NetworkAnalyzer_GetPower;
			pfNetworkAnalyzer_SetPowerRange								NetworkAnalyzer_SetPowerRange;
			pfNetworkAnalyzer_SetPowerSweep								NetworkAnalyzer_SetPowerSweep;
			pfNetworkAnalyzer_SetSweepTime								NetworkAnalyzer_SetSweepTime;
			pfNetworkAnalyzer_SetMarker									NetworkAnalyzer_SetMarker;
			pfNetworkAnalyzer_GetMarker									NetworkAnalyzer_GetMarker;
			pfNetworkAnalyzer_ReCallRegister							NetworkAnalyzer_ReCallRegister;
			pfNetworkAnalyzer_SaveToRegister							NetworkAnalyzer_SaveToRegister;
			pfNetworkAnalyzer_QuaryCalibration							NetworkAnalyzer_QuaryCalibration;
			pfNetworkAnalyzer_SetPointsNum								NetworkAnalyzer_SetPointsNum;
			pfNetworkAnalyzer_GetPointsNum								NetworkAnalyzer_GetPointsNum;
			pfNetworkAnalyzer_SetFrequencySweepType						NetworkAnalyzer_SetFrequencySweepType;
			pfNetworkAnalyzer_SelectChannel								NetworkAnalyzer_SelectChannel;
			pfNetworkAnalyzer_GetLastStateFileName						NetworkAnalyzer_GetLastStateFileName;
			pfNetworkAnalyzer_SelectMathFunction						NetworkAnalyzer_SelectMathFunction;
			pfNetworkAnalyzer_SetTraceToMathMemory						NetworkAnalyzer_SetTraceToMathMemory;

			//------------ Version 1.2.1.0 --------------------//  
			
			pfNetworkAnalyzer_ECAL_GetInformation						NetworkAnalyzer_ECAL_GetInformation;
			pfNetworkAnalyzer_ECAL_Calibrate							NetworkAnalyzer_ECAL_Calibrate;

			//------------ Version 1.2.5.0 --------------------//  
			
			pfNetworkAnalyzer_SetElectricalDelayTime					NetworkAnalyzer_SetElectricalDelayTime;

			//------------ Version 1.2.XX.0 --------------------//  
			
			pfNetworkAnalyzer_SelectTrace								NetworkAnalyzer_SelectTrace;
			pfNetworkAnalyzer_SelectWindow								NetworkAnalyzer_SelectWindow;

			//------------ Version 1.3.5.0 --------------------//  
			pfNetworkAnalyzer_Avarage									NetworkAnalyzer_Avarage;
			pfNetworkAnalyzer_ClearAvarage								NetworkAnalyzer_ClearAvarage;
			
			pfNetworkAnalyzer_Reponse_OpenShortLoad_Calibrate			NetworkAnalyzer_Reponse_OpenShortLoad_Calibrate;
			pfNetworkAnalyzer_Reponse_Thru_Calibrate					NetworkAnalyzer_Reponse_Thru_Calibrate;
			pfNetworkAnalyzer_Reponse_Done_Calibrate					NetworkAnalyzer_Reponse_Done_Calibrate;

			//------------ Version 1.3.6.0 --------------------//  
			pfNetworkAnalyzer_WaitSweep									NetworkAnalyzer_WaitSweep;

			//------------ Version 1.7.2.0 --------------------//  
			pfNetworkAnalyzer_SetCalibrationName						NetworkAnalyzer_SetCalibrationName;  
			
} tsNetworkAnalyzerFunctions;

//**************************************************************************************//

//**************************************************************************************//
//**************************** Function Generator Structure ******************************//
//**************************************************************************************//

typedef void* (*pfFunctionGenerator_GetErrorTextMessage) ( int handle , int iError , char *pErrorMessage );
typedef void* (*pfFunctionGenerator_Init) ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *InstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChanelsQuantity );
typedef void* (*pfFunctionGenerator_Close) ( int *pInstrumentHandle );
typedef void* (*pfFunctionGenerator_Reset) ( int hInstrumentHandle );

typedef void* (*pfFunctionGenerator_Initiate) ( int hInstrumentHandle );

typedef void* (*pfFunctionGenerator_Signel_Off) ( int hInstrumentHandle , int channel );
typedef void* (*pfFunctionGenerator_Signel_On) ( int hInstrumentHandle , int channel );

typedef void* (*pfFunctionGenerator_Save_State) ( int hInstrumentHandle , int iStateNumber , char *pszFileName );
typedef void* (*pfFunctionGenerator_Load_State) ( int hInstrumentHandle , int iStateNumber , char *pszFileName );

typedef struct
{
			pfSTD_Config_Install_CommentCallback						Config_Install_CommentCallback;
			pfSTD_Config_Install_ConfigValueCallback					Config_Install_ConfigValueCallback;
			pfSTD_Config_Install_CheckForBreakCallback					Config_Install_CheckForBreakCallback;
			pfSTD_Config_LOG_SetAllowState								Config_LOG_SetAllowState;
			pfSTD_Config_Copy_STD_CallBackSet							Config_Copy_STD_CallBackSet;
			
			pfFunctionGenerator_GetErrorTextMessage						FunctionGenerator_GetErrorTextMessage;      
			pfFunctionGenerator_Init									FunctionGenerator_Init;					  
			pfFunctionGenerator_Close									FunctionGenerator_Close;					  
			pfFunctionGenerator_Reset									FunctionGenerator_Reset;					
			pfFunctionGenerator_Initiate								FunctionGenerator_Initiate;					
			pfFunctionGenerator_Signel_Off								FunctionGenerator_Signel_Off;					        
			pfFunctionGenerator_Signel_On			  					FunctionGenerator_Signel_On;			  	
			pfFunctionGenerator_Save_State								FunctionGenerator_Save_State;			
			pfFunctionGenerator_Load_State								FunctionGenerator_Load_State;				
			
} tsFunctionGeneratorFunctions;

//**************************************************************************************//

//**************************************************************************************//
//**************************** Function Generator Structure ******************************//
//**************************************************************************************//

typedef unsigned __int64 (*pfStandardDevice_Get_IdentificationNumber)( void ); 

typedef void* (*pfStandardDevice_GetErrorTextMessage) ( int handle , int iError , char *pErrorMessage );
typedef void* (*pfStandardDevice_Init) ( int iDeviceID , char *pszConnectionName , char *pszAddress , int *pHandle , ... );
typedef void* (*pfStandardDevice_Close) ( int *pHandle );
typedef void* (*pfStandardDevice_Reset) ( int hHandle );

typedef void* (*pfStandardDevice_GetConnectionAddress) ( int hHandle , char **pszAddress );

typedef void* (*pfStandardDevice_SetValue)( int hHandle , char *pCommandName , void *pValue , int iValueLength , int iValueSize , ...  );
typedef void* (*pfStandardDevice_GetValue)( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize , ...  ); 
typedef void* (*pfStandardDevice_GetValue_Ex)( int hHandle , char *pCommandName , void **pGetValue , int *piValueLength , int *piValueSize , ...  ); 
	
typedef void* (*pfStandardDevice_Unplug)( int hHandle , double lfTimeOut );
typedef void* (*pfStandardDevice_PlugIn)( int hHandle , double lfTimeOut );

typedef void* (*pfStandardDevice_EnablePlugInAutomatically)( int hHandle , int bEnable );
typedef void* (*pfStandardDevice_InstallReceiveCallbackFunction)( int hHandle , void *pfFunction , void *pData );
	
typedef void* (*pfStandardDevice_LoadConfigFile)( int hHandle , char *pFileName );
typedef void* (*pfStandardDevice_LoadConfigBuffer)( int hHandle , char *pBuffer , int iBufferSize );

typedef void* (*pfStandardDevice_Check_Connection)( int hHandle , char *pCommandName , int *piStatus );    
	
typedef void* (*pfStandardDevice_Get_Commands_List)( int hHandle , char ***pCommandsList , int *piNumberOfCommands );  

typedef void* (*pfStandardDevice_GetDeviceInfoBlock)( int hHandle , char **pBuffer , int *piBufferSize );

typedef void* (*pfStandardDevice_Save_Log_File_As)( int hHandle , char *pFileNamePath );

typedef void* (*pfStandardDevice_ResetDriver)( int hHandle ); 

typedef struct
{
			pfSTD_Config_Install_CommentCallback						Config_Install_CommentCallback;
			pfSTD_Config_Install_ConfigValueCallback					Config_Install_ConfigValueCallback;
			pfSTD_Config_Install_CheckForBreakCallback					Config_Install_CheckForBreakCallback;
			pfSTD_Config_LOG_SetAllowState								Config_LOG_SetAllowState;
			pfSTD_Config_Copy_STD_CallBackSet							Config_Copy_STD_CallBackSet;
			
			pfStandardDevice_GetErrorTextMessage						StandardDevice_GetErrorTextMessage;      
			pfStandardDevice_Init										StandardDevice_Init;					  
			pfStandardDevice_Close										StandardDevice_Close;					  
			pfStandardDevice_Reset										StandardDevice_Reset;					
	
			pfStandardDevice_GetConnectionAddress						StandardDevice_GetConnectionAddress;					
			pfStandardDevice_SetValue									StandardDevice_SetValue;					        
			pfStandardDevice_GetValue			  						StandardDevice_GetValue;
			pfStandardDevice_GetValue_Ex								StandardDevice_GetValue_Ex;
	
			pfStandardDevice_Unplug										StandardDevice_Unplug;			
			pfStandardDevice_PlugIn										StandardDevice_PlugIn;				
	
			pfStandardDevice_EnablePlugInAutomatically					StandardDevice_EnablePlugInAutomatically;					
			pfStandardDevice_InstallReceiveCallbackFunction				StandardDevice_InstallReceiveCallbackFunction;					        
			pfStandardDevice_LoadConfigFile			  					StandardDevice_LoadConfigFile;			  	
			pfStandardDevice_LoadConfigBuffer							StandardDevice_LoadConfigBuffer;			
	
			pfStandardDevice_Check_Connection							StandardDevice_Check_Connection;
			pfStandardDevice_Get_Commands_List							StandardDevice_Get_Commands_List;

			//------------ Version 1.3.4.0 --------------------//  
			pfStandardDevice_Get_IdentificationNumber					StandardDevice_Get_IdentificationNumber;

			//------------ Version 1.5.3.0 --------------------//    
			pfStandardDevice_GetDeviceInfoBlock							StandardDevice_GetDeviceInfoBlock;
			
			//------------ Version 1.6.0.0 --------------------//    
			pfStandardDevice_Save_Log_File_As							StandardDevice_Save_Log_File_As;

			//------------ Version 1.6.1.0 --------------------//    
			pfStandardDevice_ResetDriver								StandardDevice_ResetDriver;
			
} tsStandardDeviceFunctions;   


//**************************************************************************************//
//**************************************************************************************//
//**************************************************************************************//

typedef enum
{
//	NO_ERRORS = 0,
	
	DRV_ERROR_PASSED_NULL = -79701,
	DRV_ERROR_INCORRECT_DRIVER_TYPE = -79702,
	DRV_ERROR_CONNECTION = -79703,
	
	DRV_ERROR_CREATE_TSV_HANDLE = -79711,
	DRV_ERROR_GET_TSV_POINTER = -79712,
	DRV_ERROR_CREATE_LOCK_HANDLE = -79713,
	DRV_ERROR_DLL_FILE_NOT_FOUND = -79714,
	DRV_ERROR_DLL_FILE_NOT_OPENED = -79715,
	DRV_ERROR_WRONG_WRAPPER = -79716, 
	DRV_ERROR_WRONG_PARAMETER = -79717,
	
}teDriversErrorsDef;		


typedef enum { DRIVER_TYPE_NOT_DEFINED , 
			   
			   DRIVER_TYPE_POWER_SUPPLY , 
			   DRIVER_TYPE_POWER_METER , 
			   DRIVER_TYPE_SIGNAL_GENERATOR , 
			   DRIVER_TYPE_OSCILLOSCOPE , 
			   DRIVER_TYPE_SPECTRUM_ANALYZER,
			   DRIVER_TYPE_MULTI_METER,
			   DRIVER_TYPE_NOISE_FIGURE,
			   DRIVER_TYPE_WAVE_ANALYZER,
			   DRIVER_TYPE_NETWORK_ANALYZER,
			   DRIVER_TYPE_FUNCTION_GENERATOR,
			   DRIVER_TYPE_STANDARD_DEVICE,
			   
			   DRIVER_TYPE_SENTINEL 

} teDriversType;

typedef struct
{
			tsPowerSupplyFunctions				powerSupplyDriverFunctions;
			tsPowerMeterFunctions				powerMeterDriverFunctions;
			tsSignalGeneratorFunctions			signalGeneratorDriverFunctions;
			tsOscilloscopeFunctions				oscilloscopeDriverFunctions;
			tsSpectrumAnalyzerFunctions			spectrumAnalyzerDriverFunctions;
			tsMultiMeterFunctions				multiMeterDriverFunctions;
			tsNoiseFigureFunctions				noiseFigureDriverFunctions;
			tsWaveAnalyzerFunctions				waveAnalyzerDriverFunctions;
			tsNetworkAnalyzerFunctions			networkAnalyzerDriverFunctions;
			tsFunctionGeneratorFunctions		functionGeneratorDriverFunctions; 
			tsStandardDeviceFunctions			standardDeviceDriverFunctions;  

			//------------ Version 1.2.8.0 --------------------//  
			pfSTD_Equipment_Info				Equipment_Info;
			pfSTD_Equipment_IsSupported			Equipment_IsSupported;  
			
			//------------ Version 1.3.1.0 --------------------//  
			pfSTD_Equipment_GetLowLevelHandle	Equipment_GetLowLevelHandle;
			
			//------------ Version 1.6.2.0 --------------------//  
			
			pfSTD_Equipment_SendStateFile		Equipment_SendStateFile;
			pfSTD_Equipment_ReceiveStateFile	Equipment_ReceiveStateFile;
			pfSTD_Equipment_GetFileCatalog		Equipment_GetFileCatalog;
			pfSTD_Equipment_MakeFileCatalog		Equipment_MakeFileCatalog;
			pfSTD_Equipment_DeleteFileCatalog	Equipment_DeleteFileCatalog;
			pfSTD_Equipment_DeleteStateFile		Equipment_DeleteStateFile;
		
			//------------ Version 1.9.0.0 --------------------//  
			pfSTD_Config_SetAttribute			Config_SetAttribute;  
		
} tsInstrDB;

typedef struct
{
			int									bAllow;
	
			double								*vlfFrequency,
												*vlfLoss;
	
			int									iNumberOfPoints;
	
			double								lfStartFrequency,
												lfStopFrequency;

			double								lfSetPower;
	
			double								lfMeasureDelay;
	
			char								*pConnectionDescription,
												*pConnectionImage;
	
			char								*pCalibrationRefName;
			int									clbConfigType,
												clbLinkID;
	
			int									iCalibrationPortNumber;
	
			char								*pStateFileName;
			
			//------------ Version 1.1.0.0 --------------------//  
			
			double								lfReferenceLevel,
												lfAttenuator; 
			
			int									iNumberOfPowers; 
			
			double								*vlfPowerList,
												**vlfLossList;
			
			double								lfMeasureTimeout;

} tsCalibrationItem;

typedef struct
{

			int									InstrumentHandle;
	
			int									InstrumentLockHandle;
	
			unsigned int						LockCounter;
	
			HINSTANCE							LibraryHandle;
	
			char								*pInstrumentAddress;
	
			int									bConnected;
	
			int									ChannelQuantity;
	
			teDriversType						InstrumentType;
	
			tsSTD_CallBackSet					*ptCallbacks;
	
			tsCalibrationItem					*pCalibration;
	
			char								*pLastStateFileName;
			
			tsInstrDB							tInstrDB; 
			
			//------------ Version 1.1.0.0 --------------------// 
			double								lfPower,
												lfFrequency;
			//------------ Version 1.2.0.0 --------------------//  
			int									bDemoMode;
			
			//------------ Version 1.2.6.0 --------------------// 
			char								*pDriverFileName; 
			
			//------------ Version 1.3.1.0 --------------------// 
			int									InstrumentLowLevelHandle;
			
}tsDriverInfo;


typedef struct SINGLE_LIST_ITEM
{
			int									handle,
												lock;
	
			int									ignore_dup_address;
			
			char								*pszConnectString;
	
			teDriversType						InstrumentType; 
			
			struct SINGLE_LIST_ITEM				*pNextItem;
			//------------ Version 1.1.0.0 --------------------//  

} tsSingleListItem;

typedef struct
{
			tsSingleListItem					*pItemsList;
	
			tsSTD_CallBackSet					*pSTD_CallBackSet;
	
			//------------ Version 1.2.7.0 --------------------//  
			int									iNumberOfCallBackSet;

} tsGlobalHandle;

//==============================================================================
// External variables

//==============================================================================
// Global functions
int  ShowMessage ( int type , char *pTitle , char *pText, char *pPicture );

int DRIVER_MANAGER_IsConnectionExistsByType( teDriversType tInstrumentType );
int DRIVER_MANAGER_IsConnectionExists( char *pszAddress , int *phHandle , int *phLock );
int DRIVER_MANAGER_AddConnection( char *pszAddress , int *phHandle , teDriversType tInstrumentType , int *phLock );
int	DRIVER_MANAGER_UpdateConnection( char *pszAddress, int hHandle , int *phLock );
int	DRIVER_MANAGER_UpdateAddressByConnection( int hHandle , char *pszAddress );
int DRIVER_MANAGER_RemoveConnectionExists( char *pszAddress , int hHandle );
int DRIVER_MANAGER_UpdateIgnoreDupAddresses( char *pszAddress , int hHandle , int bIgnoreDupAddresses );

int	DRIVER_MANAGER_GetCopyCallbacksStructure( int hDeviceHandle , tsSTD_CallBackSet **pSTD_CallBackSet , int iNumberOfParams , ... );  


STD_ERROR DRIVER_MANAGER_CALIBRATION_FillCalibrationRanges( int hInstrumentHandle , char *pRangesString ); // sample "1E9,2E9-8E9:50"
STD_ERROR DRIVER_MANAGER_CALIBRATION_FillCalibrationInfo(int hInstrumentHandle,double *plfFrequency,double *plfLoss,int iNumberOfPoints,double lfStartFrequnecy,double lfStopFrequency);
STD_ERROR DRIVER_MANAGER_CALIBRATION_GetCalibrationInfo(int hInstrumentHandle,double **plfFrequency,double **plfLoss,int *piNumberOfPoints,double *plfStartFrequnecy,double *plfStopFrequency);
STD_ERROR DRIVER_MANAGER_CALIBRATION_GetCalibrationRangeInfo( int Handle , double *plfStart , double *plfStop , int *piNumberOfPoints ); 
STD_ERROR DRIVER_MANAGER_CALIBRATION_EnableUseCalibration(int hInstrumentHandle,int bAllow);
STD_ERROR DRIVER_MANAGER_CALIBRATION_FillStoreInformation(int hInstrumentHandle,char *pStoreName,int clbConfigType,int clbLinkID);
STD_ERROR DRIVER_MANAGER_CALIBRATION_GetStoreInformation(int hInstrumentHandle,char **pStoreName,int *pclbConfigType,int *pclbLinkID);
STD_ERROR DRIVER_MANAGER_CALIBRATION_FillMessageInfo(int hInstrumentHandle,char *pConnectionDescription,char *pConnectionImage);
STD_ERROR DRIVER_MANAGER_CALIBRATION_GelFactor(int hInstrumentHandle,double lfFrequency,double *plfFactor);
STD_ERROR DRIVER_MANAGER_CALIBRATION_Run(int StimulusHandle,int MeasureHandle, int StoreHandle , int iProgressPanel , int iProgressControl );

STD_ERROR DRIVER_MANAGER_CALIBRATION_Fill_AdditionalPower_CalibrationInfo( int Handle , double lfPower , double *plfLoss , int iNumberOfPoints );
STD_ERROR DRIVER_MANAGER_CALIBRATION_Get_AdditionalPower_CalibrationInfo( int Handle , int iPowerIndex , double **plfLoss , int *piNumberOfPoints , double *plfPower );
STD_ERROR DRIVER_MANAGER_CALIBRATION_Set_NumberOf_AdditionalPower( int Handle , double *plfPowerList , int iNumberOfPowers );

STD_ERROR DRIVER_MANAGER_CALIBRATION_Gel_3D_Factor( int Handle , double lfPower , double lfFrequency , double *plfFactor );

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __DRIVER_MANAGER_DriversManager_H__ */
