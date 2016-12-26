//==============================================================================
// Include files

#include "MAIN_DriversManager.h"
#include "SA_DriversManager.h"
#include "toolbox.h"
#include <utility.h>
#include <ansi_c.h>

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

/***** ***** ***** ***** ***** ***** Get Error Text Message ***** ***** ***** ***** ***** ***** *****/

int DLLEXPORT	DRV_SpectrumAnalyzer_GetErrorTextMessage ( int Handle , int iError , char **pErrorMessage )  
{		
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
	
	pfSpectrumAnalyzer_GetErrorTextMessage		pWrapperFunction						=	NULL;
	
	STD_ERROR                  					StdError								=	{0};
	
	int											bLocked									=	0;
	
	if ((pErrorMessage == NULL) || (Handle == 0))
		return DRV_ERROR_PASSED_NULL;
	
	CALLOC_ERR( *pErrorMessage , 256 , sizeof(char)); 
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		return DRV_ERROR_GET_TSV_POINTER;
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		return DRV_ERROR_INCORRECT_DRIVER_TYPE;
	
	switch (iError)
	{
		case NO_ERROR:
			
			strcpy (*pErrorMessage, "OK.");
			break;
			
		case DRV_ERROR_PASSED_NULL:
			
			strcpy (*pErrorMessage, "Function parameter passed NULL.");
			break;
			
		case DRV_ERROR_INCORRECT_DRIVER_TYPE:
			
			strcpy (*pErrorMessage, "Unknown driver type.");
			break;
			
		case DRV_ERROR_CONNECTION:
			
			strcpy (*pErrorMessage, "Can't connect to the instrument.");
			break;
			
		case DRV_ERROR_CREATE_TSV_HANDLE:
			
			strcpy (*pErrorMessage, "Can't create new Thread Safe Variable.");
			break;
			
		case DRV_ERROR_GET_TSV_POINTER:
			
			strcpy (*pErrorMessage, "Can't get pointer to the Thread Safe Variable.");
			break;
			
		case DRV_ERROR_CREATE_LOCK_HANDLE:
			
			strcpy (*pErrorMessage, "Can't create new Lock.");
			break;
			
		case DRV_ERROR_DLL_FILE_NOT_FOUND:
			
			strcpy (*pErrorMessage, "The dll file not found.");
			break;
			
		case DRV_ERROR_DLL_FILE_NOT_OPENED:
			
			strcpy (*pErrorMessage, "Can't open the dll.");
			break;
			
		default:
			
			if ((iError < (-7700)) && (iError > (-7799)))
			{
				LockHandle = tDriverInfo.InstrumentLockHandle;
				
				pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetErrorTextMessage;
	
				CHK_PROCCESS_GET_LOCK ( LockHandle );
					
				if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
				{
					FREE_STDERR_COPY( pWrapperFunction ( tDriverInfo.InstrumentHandle , iError , (*pErrorMessage) ));
				}
			
				if ( pWrapperFunction == NULL )
				{
					char	szMessage[LOW_STRING]	= {0};
				
					sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_GetErrorTextMessage" , tDriverInfo.pDriverFileName );
				
					ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
				}
			}
			else
				strcpy (*pErrorMessage, "Unknown error.");	
				
			break;
	}
Error:	
	
	if ( LockHandle && bLocked )
		CmtReleaseLock (LockHandle);
	
	RETURN_ERROR_VALUE;
}
/***** ***** ***** ***** ***** ***** ***** ***** Init ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Init ( char *pszDriverLocation , char *pszAddressString , int *pHandle )
{			  			
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
	
	pfSpectrumAnalyzer_Init						pWrapperFunction						=	NULL;
	
	char										szCurrentDirectory[STD_STRING]			=	{0},
												szDriverLocation[STD_STRING]			=	{0},
												szMessage[STD_STRING]					=	{0},
												*pTemp									=	NULL;
	
	int											iTry									=	0,
												bHandleExists							=	0;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	pfSTD_Config_Install_CommentCallback        pConfig_Install_CommentCallback			=	NULL;
	pfSTD_Config_Install_ConfigValueCallback    pConfig_Install_ConfigValueCallback		=	NULL;
	pfSTD_Config_Install_CheckForBreakCallback  pConfig_Install_CheckForBreakCallback	=	NULL;
	pfSTD_Config_Copy_STD_CallBackSet			pConfig_Copy_STD_CallBackSet			=	NULL;

	HINSTANCE									LibraryHandle							=	NULL;
	
	if (( pszDriverLocation == NULL ) || ( pszAddressString == NULL ))
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	strcpy( szDriverLocation , pszDriverLocation );
	
	do
	{
		pTemp = strstr( pszDriverLocation , ":\\" );
	
		if ( pTemp )
			break;
		
		pTemp = strchr( pszDriverLocation , '\\' ); 
		
		if ( pTemp == NULL )
			break;
			
		GetModuleDir( __CVIUserHInst , szCurrentDirectory ); 
		
		if ( pTemp == pszDriverLocation )
			sprintf( szDriverLocation , "%s%s" , szCurrentDirectory , pszDriverLocation );
		else
			sprintf( szDriverLocation , "%s\\%s" , szCurrentDirectory , pszDriverLocation ); 
		
	} while(0);
			
	if ( FileExists(szDriverLocation,NULL) == 0 )
	{
		sprintf( szMessage , "Loading Spectrum Analyzer Driver\n\"%s\"\n\nFailed, File has not found." , szDriverLocation );
		ShowMessage ( INSTR_TYPE_CONTINUE , "Equipment driver file error..." , szMessage , NULL );
		
		FORCE_ERR(-5,szMessage);
	}
	
	if ( pHandle )
	{
		if ( CmtNewTSV ( sizeof(tsDriverInfo) , &VariableHandle ) != 0 )
			{STD_ERR (DRV_ERROR_CREATE_TSV_HANDLE);}

		if ( VariableHandle == 0 )
			{STD_ERR (DRV_ERROR_CREATE_TSV_HANDLE);}
	
		do
		{
			*pHandle = VariableHandle;
	
			if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
			{
				{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
				break;
			}
	
			pDriverInfo->InstrumentType = DRIVER_TYPE_SPECTRUM_ANALYZER;
	
			CALLOC_COPY_STRING( pDriverInfo->pInstrumentAddress , pszAddressString );  
		
			bHandleExists = DRIVER_MANAGER_IsConnectionExists( pszAddressString , &(pDriverInfo->InstrumentHandle) , &(pDriverInfo->InstrumentLockHandle) );

			//--------- Load Library ---------------------//
	
			pDriverInfo->LibraryHandle = LoadLibrary(szDriverLocation);

			CALLOC_COPY_STRING( pDriverInfo->pDriverFileName,szDriverLocation);
		
			if ( pDriverInfo->LibraryHandle == 0 )
			{
				{STD_ERR (DRV_ERROR_DLL_FILE_NOT_OPENED);}
				break;
			}	
			//--------------- Load functions ------------------//
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.Config_Install_CommentCallback							= (pfSTD_Config_Install_CommentCallback) GetProcAddress( pDriverInfo->LibraryHandle , 		"Config_Install_CommentCallback");      
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.Config_Install_ConfigValueCallback						= (pfSTD_Config_Install_ConfigValueCallback) GetProcAddress( pDriverInfo->LibraryHandle , 	"Config_Install_ConfigValueCallback");  
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.Config_Install_CheckForBreakCallback						= (pfSTD_Config_Install_CheckForBreakCallback) GetProcAddress( pDriverInfo->LibraryHandle , "Config_Install_CheckForBreakCallback");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.Config_LOG_SetAllowState									= (pfSTD_Config_LOG_SetAllowState) GetProcAddress( pDriverInfo->LibraryHandle , "Config_LOG_SetAllowState");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.Config_Copy_STD_CallBackSet								= (pfSTD_Config_Copy_STD_CallBackSet) GetProcAddress( pDriverInfo->LibraryHandle , "Config_Copy_STD_CallBackSet");
			
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetErrorTextMessage         				= (pfSpectrumAnalyzer_GetErrorTextMessage) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_GetErrorTextMessage");         
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Init                        				= (pfSpectrumAnalyzer_Init) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_Init");                        
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Close                       				= (pfSpectrumAnalyzer_Close) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_Close");                       
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Reset									= (pfSpectrumAnalyzer_Reset) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_Reset");                       
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Preset 									= (pfSpectrumAnalyzer_Preset) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_Preset"); 
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetAuto                    				= (pfSpectrumAnalyzer_SetAuto) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetAuto");                    
		    pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetFrequencyCenterSpan      			 	= (pfSpectrumAnalyzer_SetFrequencyCenterSpan) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetFrequencyCenterSpan");                 
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_ConfigureMarker            				= (pfSpectrumAnalyzer_ConfigureMarker) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_ConfigureMarker");            
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetMarkerSearchPeakAuto     				= (pfSpectrumAnalyzer_SetMarkerSearchPeakAuto) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetMarkerSearchPeakAuto");     
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetMarkerMeasure      					= (pfSpectrumAnalyzer_GetMarkerMeasure) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_GetMarkerMeasure");      
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetTrace            						= (pfSpectrumAnalyzer_GetTrace) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_GetTrace");            
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetMaxHold     							= (pfSpectrumAnalyzer_SetMaxHold) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetMaxHold");     
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_ConfigChannelPowerMeasuring				= (pfSpectrumAnalyzer_ConfigChannelPowerMeasuring) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_ConfigChannelPowerMeasuring");      
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetChannelPowerResult   					= (pfSpectrumAnalyzer_GetChannelPowerResult) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_GetChannelPowerResult");      
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetStartStopFrequency    				= (pfSpectrumAnalyzer_GetStartStopFrequency) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_GetStartStopFrequency");    
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_ConfigBandWidth    						= (pfSpectrumAnalyzer_ConfigBandWidth) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_ConfigBandWidth");    
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_ConfigSource			    				= (pfSpectrumAnalyzer_ConfigSource) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_ConfigSource");    
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetNumberOfPoints						= (pfSpectrumAnalyzer_SetNumberOfPoints) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetNumberOfPoints");    
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetScaleDev                              = (pfSpectrumAnalyzer_SetScaleDev) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetScaleDev");     					 
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Attenuator                               = (pfSpectrumAnalyzer_Attenuator) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_Attenuator");                             
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetReferenceLevel                        = (pfSpectrumAnalyzer_SetReferenceLevel) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetReferenceLevel");                      
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetReferenceLevel                        = (pfSpectrumAnalyzer_GetReferenceLevel) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_GetReferenceLevel");                  
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetMarkerType                            = (pfSpectrumAnalyzer_SetMarkerType) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetMarkerType");                      
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetMarkerFrequency                       = (pfSpectrumAnalyzer_SetMarkerFrequency) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetMarkerFrequency");                 
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetMarkerFrequency                       = (pfSpectrumAnalyzer_GetMarkerFrequency) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_GetMarkerFrequency");                 
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetMarkerAmplitude                       = (pfSpectrumAnalyzer_GetMarkerAmplitude) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_GetMarkerAmplitude");                 
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetMarkerDeltaFrequency                  = (pfSpectrumAnalyzer_GetMarkerDeltaFrequency) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_GetMarkerDeltaFrequency");            
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetMarkerDeltaAmplitude                  = (pfSpectrumAnalyzer_GetMarkerDeltaAmplitude) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_GetMarkerDeltaAmplitude");            
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetCenterFrequencyFromMarker             = (pfSpectrumAnalyzer_SetCenterFrequencyFromMarker) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetCenterFrequencyFromMarker");       
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetReferenceLevelFrequencyFromMarker     = (pfSpectrumAnalyzer_SetReferenceLevelFrequencyFromMarker) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetReferenceLevelFrequencyFromMarker");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_WaitForIntrumentDone                     = (pfSpectrumAnalyzer_WaitForIntrumentDone) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_WaitForIntrumentDone");               
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetSweepMode                             = (pfSpectrumAnalyzer_SetSweepMode) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetSweepMode");                       
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_InitiateMeasure                          = (pfSpectrumAnalyzer_InitiateMeasure) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_InitiateMeasure");                    
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetAverage                               = (pfSpectrumAnalyzer_SetAverage) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetAverage");                         
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SearchMarkerPeak                         = (pfSpectrumAnalyzer_SearchMarkerPeak) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SearchMarkerPeak"); 
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SearchMarkerNextPeak                     = (pfSpectrumAnalyzer_SearchMarkerNextPeak) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SearchMarkerNextPeak"); 
			
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Config									= (pfSpectrumAnalyzer_Config) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_Config");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetFrequency								= (pfSpectrumAnalyzer_SetFrequency) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetFrequency");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetStartStopFrequency					= (pfSpectrumAnalyzer_SetStartStopFrequency) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetStartStopFrequency");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetSpan									= (pfSpectrumAnalyzer_SetSpan) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetSpan");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetAmplitude								= (pfSpectrumAnalyzer_SetAmplitude) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetAmplitude");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetAttenuation							= (pfSpectrumAnalyzer_SetAttenuation) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetAttenuation");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetRBW									= (pfSpectrumAnalyzer_SetRBW) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetRBW");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetVBW									= (pfSpectrumAnalyzer_SetVBW) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetVBW");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetBW									= (pfSpectrumAnalyzer_SetBW) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetBW");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetSweep									= (pfSpectrumAnalyzer_GetSweep) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_GetSweep");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetMarker								= (pfSpectrumAnalyzer_SetMarker) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetMarker");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetRLAuto								= (pfSpectrumAnalyzer_SetRLAuto) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetRLAuto");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetFrequency								= (pfSpectrumAnalyzer_GetFrequency) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_GetFrequency");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetAmplitude								= (pfSpectrumAnalyzer_GetAmplitude) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_GetAmplitude");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetFreqCountResol						= (pfSpectrumAnalyzer_SetFreqCountResol) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetFreqCountResol");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetAvg									= (pfSpectrumAnalyzer_SetAvg) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetAvg");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetMarkerLevel							= (pfSpectrumAnalyzer_GetMarkerLevel) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_GetMarkerLevel");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_AutoSetIsDone							= (pfSpectrumAnalyzer_AutoSetIsDone) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_AutoSetIsDone");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetAutoTuneThreshold						= (pfSpectrumAnalyzer_SetAutoTuneThreshold) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetAutoTuneThreshold");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetMode									= (pfSpectrumAnalyzer_SetMode) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetMode"); 
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Recall									= (pfSpectrumAnalyzer_Recall) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_Recall"); 
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetWiMaxMeasure 							= (pfSpectrumAnalyzer_GetWiMaxMeasure) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_GetWiMaxMeasure");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetSweepTime 							= (pfSpectrumAnalyzer_SetSweepTime) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetSweepTime");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetTraceDetector 						= (pfSpectrumAnalyzer_SetTraceDetector) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetTraceDetector"); 
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetMarkerTimePosition 					= (pfSpectrumAnalyzer_SetMarkerTimePosition) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetMarkerTimePosition");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetToMode_Specrum						= (pfSpectrumAnalyzer_SetToMode_Specrum) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetToMode_Specrum");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_RecallState								= (pfSpectrumAnalyzer_RecallState) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_RecallState");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Read_EVM									= (pfSpectrumAnalyzer_Read_EVM) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_Read_EVM");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetFrequency_EVM							= (pfSpectrumAnalyzer_SetFrequency_EVM) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetFrequency_EVM");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_RestartMeasure							= (pfSpectrumAnalyzer_RestartMeasure) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_RestartMeasure");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Read_SymbolStream						= (pfSpectrumAnalyzer_Read_SymbolStream) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_Read_SymbolStream");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_PhaseNoise_GetMarker						= (pfSpectrumAnalyzer_PhaseNoise_GetMarker) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_PhaseNoise_GetMarker");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_PhaseNoise_GetIntegratedMarker			= (pfSpectrumAnalyzer_PhaseNoise_GetIntegratedMarker) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_PhaseNoise_GetIntegratedMarker");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_AutoTune									= (pfSpectrumAnalyzer_AutoTune) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_AutoTune");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SaveState								= (pfSpectrumAnalyzer_SaveState) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SaveState");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetLastStateFileName						= (pfSpectrumAnalyzer_GetLastStateFileName) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_GetLastStateFileName");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetAttenuator							= (pfSpectrumAnalyzer_GetAttenuator) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_GetAttenuator");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Read_IP3									= (pfSpectrumAnalyzer_Read_IP3) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_Read_IP3"); 
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Read_IP2									= (pfSpectrumAnalyzer_Read_IP2) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_Read_IP2"); 
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Read_Harmonics_Distortion				= (pfSpectrumAnalyzer_Read_Harmonics_Distortion) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_Read_Harmonics_Distortion"); 
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Read_Harmonics_dBc						= (pfSpectrumAnalyzer_Read_Harmonics_dBc) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_Read_Harmonics_dBc"); 
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Read_Harmonics_Amplitude					= (pfSpectrumAnalyzer_Read_Harmonics_Amplitude) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_Read_Harmonics_Amplitude"); 
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Read_Spurious_Emissions_List				= (pfSpectrumAnalyzer_Read_Spurious_Emissions_List) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_Read_Spurious_Emissions_List"); 
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetOffset								= (pfSpectrumAnalyzer_SetOffset) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetOffset"); 
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetMarkerFrequencyCounter				= (pfSpectrumAnalyzer_GetMarkerFrequencyCounter) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_GetMarkerFrequencyCounter");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_MeasureMarkerFrequencyCounter			= (pfSpectrumAnalyzer_MeasureMarkerFrequencyCounter) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_MeasureMarkerFrequencyCounter");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetTriggerSource							= (pfSpectrumAnalyzer_SetTriggerSource) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetTriggerSource");
			pDriverInfo->tInstrDB.Equipment_Info      																		= (pfSTD_Equipment_Info) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_Info");      
			pDriverInfo->tInstrDB.Equipment_IsSupported																		= (pfSTD_Equipment_IsSupported) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_IsSupported");   
		
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_AlignNow									= (pfSpectrumAnalyzer_AlignNow) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_AlignNow"); 
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_NoiseFigure_CalibrateNow					= (pfSpectrumAnalyzer_NoiseFigure_CalibrateNow) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_NoiseFigure_CalibrateNow");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_NoiseFigure_SetLossCompensationTable		= (pfSpectrumAnalyzer_NoiseFigure_SetLossCompensationTable) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_NoiseFigure_SetLossCompensationTable");
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_NoiseFigure_ApplyCalibration				= (pfSpectrumAnalyzer_NoiseFigure_ApplyCalibration) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_NoiseFigure_ApplyCalibration");

			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetMarkerFrequencyCounter				= (pfSpectrumAnalyzer_SetMarkerFrequencyCounter) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_SetMarkerFrequencyCounter");
			
			pDriverInfo->tInstrDB.Equipment_SendStateFile																	= (pfSTD_Equipment_SendStateFile) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_SendStateFile");
			pDriverInfo->tInstrDB.Equipment_ReceiveStateFile																= (pfSTD_Equipment_ReceiveStateFile) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_ReceiveStateFile");
			pDriverInfo->tInstrDB.Equipment_GetFileCatalog																	= (pfSTD_Equipment_GetFileCatalog) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_GetFileCatalog"); 			
			pDriverInfo->tInstrDB.Equipment_MakeFileCatalog																	= (pfSTD_Equipment_MakeFileCatalog) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_MakeFileCatalog"); 			
			pDriverInfo->tInstrDB.Equipment_DeleteFileCatalog																= (pfSTD_Equipment_DeleteFileCatalog) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_DeleteFileCatalog");
			pDriverInfo->tInstrDB.Equipment_DeleteStateFile																	= (pfSTD_Equipment_DeleteStateFile) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_DeleteStateFile");
			
			pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_PhaseNoise_SetIntegratedMarker_Start_Stop_Offsets	= (pfSpectrumAnalyzer_PhaseNoise_SetIntegratedMarker_Start_Stop_Offsets) GetProcAddress( pDriverInfo->LibraryHandle , "SpectrumAnalyzer_PhaseNoise_SetIntegratedMarker_Start_Stop_Offsets");	

																																																															
			pDriverInfo->tInstrDB.Config_SetAttribute																		= (pfSTD_Config_SetAttribute) GetProcAddress( pDriverInfo->LibraryHandle , "Config_SetAttribute"); 		
			
			pWrapperFunction = pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Init;
			
			if ( pWrapperFunction == NULL )
			{
				char	szMessage[LOW_STRING]	= {0};
				
				sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_Init" , pDriverInfo->pDriverFileName );
				
				ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
			
				{STD_ERR (DRV_ERROR_WRONG_WRAPPER);}
			}
		
			if ( !bHandleExists )
				DRIVER_MANAGER_AddConnection( pszAddressString , &(pDriverInfo->InstrumentHandle) , DRIVER_TYPE_SPECTRUM_ANALYZER , &(pDriverInfo->InstrumentLockHandle) );
		
			LockHandle = pDriverInfo->InstrumentLockHandle;
		
			if ( LockHandle == 0 )
				{STD_ERR (DRV_ERROR_CREATE_LOCK_HANDLE);}
		
			CHK_PROCCESS_GET_LOCK ( LockHandle );
		
			DRIVER_MANAGER_IsConnectionExists( pszAddressString, &(pDriverInfo->InstrumentHandle) , NULL );
	
			//===================== Install Calbacks ==========================================================================================//
			pConfig_Install_CommentCallback = pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.Config_Install_CommentCallback;		
			pConfig_Install_ConfigValueCallback	= pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.Config_Install_ConfigValueCallback;		
			pConfig_Install_CheckForBreakCallback = pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.Config_Install_CheckForBreakCallback;		
			pConfig_Copy_STD_CallBackSet = pDriverInfo->tInstrDB.spectrumAnalyzerDriverFunctions.Config_Copy_STD_CallBackSet;
		
			DRIVER_MANAGER_GetCopyCallbacksStructure( VariableHandle , &(pDriverInfo->ptCallbacks) , 1 , pszAddressString ); 
			
			if ( pConfig_Copy_STD_CallBackSet && ( pDriverInfo->ptCallbacks ))
			{
				FREE_STDERR_COPY_ERR( pConfig_Copy_STD_CallBackSet( &pDriverInfo->InstrumentHandle , pDriverInfo->ptCallbacks ));
				
				pDriverInfo->ptCallbacks = NULL;
			}
			else
			{
				if ( pConfig_Install_CommentCallback && ( pDriverInfo->ptCallbacks ))
				{
					FREE_STDERR_COPY_ERR( pConfig_Install_CommentCallback( &pDriverInfo->InstrumentHandle , (pDriverInfo->ptCallbacks)->fCommentCallback , (pDriverInfo->ptCallbacks)->pCommentCallbackData , (pDriverInfo->ptCallbacks)->commentType ));
				}
				
				if ( pConfig_Install_ConfigValueCallback && ( pDriverInfo->ptCallbacks ))
				{
					FREE_STDERR_COPY_ERR( pConfig_Install_ConfigValueCallback( &pDriverInfo->InstrumentHandle , (pDriverInfo->ptCallbacks)->fConfigValueCallback , (pDriverInfo->ptCallbacks)->pConfigValueCallbackData , (pDriverInfo->ptCallbacks)->configType ));
				}
		
				if ( pConfig_Install_CheckForBreakCallback && ( pDriverInfo->ptCallbacks ))
				{
					FREE_STDERR_COPY_ERR( pConfig_Install_CheckForBreakCallback( &pDriverInfo->InstrumentHandle , (pDriverInfo->ptCallbacks)->fCheckForBreakCallback , (pDriverInfo->ptCallbacks)->pCheckForBreakCallbackData , (pDriverInfo->ptCallbacks)->breakType ));
				}
			}
		
			if (pDriverInfo->ptCallbacks)
				pDriverInfo->bDemoMode = (pDriverInfo->ptCallbacks)->bDemoMode; 
			//=================================================================================================================================//
		
			for ( iTry = 0 ; iTry < 3 ; iTry++ )
			{
				if ( pDriverInfo->bDemoMode == 1 )
				{
					pDriverInfo->bConnected = 1;
					break;
				}
			
				if ( pWrapperFunction )
				{
					FREE_STDERR_COPY( pWrapperFunction( pDriverInfo->InstrumentHandle , pszAddressString , &pDriverInfo->InstrumentHandle , &pDriverInfo->bConnected , &pDriverInfo->ChannelQuantity )); 
				}
			
				if ( StdError.error )
					if ( ShowMessage ( INSTR_TYPE_YES_NO , "Spectrum Analyzer Error !!!", "Check connection and Power On the Device." , NULL ) )
						continue;
			
				break;
			}
		
			CHK_STDERR(StdError);
		
			if (pDriverInfo->bConnected == 0)
				{STD_ERR (DRV_ERROR_CONNECTION);}
	
			if ( !bHandleExists ) 
				DRIVER_MANAGER_UpdateConnection( pszAddressString , pDriverInfo->InstrumentHandle , NULL );

		} while (0);
	}
	else
	{
		pfSTD_Equipment_IsSupported					pWrapperFunction							=	NULL;

		int											iCount										=	0, 
													status										=	0, 
													bSupport									=	0, 
													defaultRM									=	0,
													hConnectionHandle							=	0;
		char										szReadBuffer[LOW_STRING]					=	{0};
		//------------------ Checking for driver supporting ------------------------//
		
		viOpenDefaultRM (&defaultRM);

		SetBreakOnLibraryErrors (0);
		
		status = viOpen ( defaultRM , pszAddressString , NULL, NULL, &hConnectionHandle );
		
		if ( status == 0 )
		{	
			status = viPrintf ( hConnectionHandle , "*IDN?\n" );
		
			if ( status == 0 )
			{
				viRead ( hConnectionHandle , szReadBuffer , (LOW_STRING-1) , &iCount );
			}
		}
		
		if ( hConnectionHandle )
			viClose(hConnectionHandle); 
		
		SetBreakOnLibraryErrors (1);  
		
		LibraryHandle = LoadLibrary(szDriverLocation);
		
		if ( LibraryHandle == 0 )
		{
			sprintf( szMessage , "Loading Driver File\n\"%s\"\n\nFailed, File corrupted." , szDriverLocation );
			ShowMessage ( INSTR_TYPE_CONTINUE , "Equipment driver file error..." , szMessage , NULL );
		
			FORCE_ERR(-6,szMessage);
		}
		
		pWrapperFunction = (pfSTD_Equipment_IsSupported) GetProcAddress( LibraryHandle , "Equipment_IsSupported");   
	
		if ( pWrapperFunction )
		{
			FREE_STDERR_COPY_ERR( pWrapperFunction( 0, pszAddressString , NULL , szReadBuffer , &bSupport , NULL )); 
			
			IF (( bSupport == 0 ) , "This driver is not support the current Equipment." );
		}
		else
		{
			char	szMessage[LOW_STRING]	= {0};
		
			sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Equipment_IsSupported" , szDriverLocation );
		
			ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
		}			
	}
	
Error:
	
	if ( LibraryHandle )
		FreeLibrary(LibraryHandle); 

	if ( LockHandle && bLocked ) 
		CmtReleaseLock (LockHandle);
	
	if ( VariableHandle ) 
		CmtReleaseTSVPtr ( VariableHandle );
		
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
		DRV_SpectrumAnalyzer_Close( &VariableHandle ); 
		
		if ( pHandle )
			*pHandle = 0;
	}

	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** ***** Close ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Close ( int *pHandle )
{						
	STD_ERROR                                   StdError                                    =   {0};
							
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_Close					pWrapperFunction						=	NULL;
	
	int											bHandleExists							=	0;
	
	
	
	int											bLocked									=	0;
	
	if ( pHandle == NULL )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = *pHandle;
	*pHandle = 0;
	
	if ( VariableHandle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	FREE( pDriverInfo->pLastStateFileName );
	
	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Close;
	
	bHandleExists = DRIVER_MANAGER_IsConnectionExists( tDriverInfo.pInstrumentAddress , NULL , NULL );

	if ( pWrapperFunction && bHandleExists )
	{	
		CHK_PROCCESS_GET_LOCK ( LockHandle );   
		
		if ( pDriverInfo->bDemoMode == 0 )
		{
			FREE_STDERR_COPY( pWrapperFunction( &tDriverInfo.InstrumentHandle )); 
		}
		
		CmtReleaseLock ( LockHandle ); 
		LockHandle=0;
		bLocked=0;
		
		DRIVER_MANAGER_RemoveConnectionExists( tDriverInfo.pInstrumentAddress , tDriverInfo.InstrumentHandle );
	}
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_Close" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
	FREE(tDriverInfo.pInstrumentAddress); 
	FREE(tDriverInfo.pDriverFileName);
	
	if ( tDriverInfo.pCalibration )
	{
		FREE(tDriverInfo.pCalibration->pStateFileName);
		FREE(tDriverInfo.pCalibration->pCalibrationRefName);
		FREE(tDriverInfo.pCalibration->pConnectionDescription); 
		FREE(tDriverInfo.pCalibration->pConnectionImage); 
		FREE(tDriverInfo.pCalibration->vlfFrequency); 
		FREE(tDriverInfo.pCalibration->vlfLoss); 
		
		FREE(tDriverInfo.pCalibration->vlfPowerList);
		FREE_LIST(tDriverInfo.pCalibration->vlfLossList,tDriverInfo.pCalibration->iNumberOfPowers);
		
		FREE(tDriverInfo.pCalibration);
	}  
	
	FREE( tDriverInfo.ptCallbacks );	

Error:
	
	if ( LockHandle && bLocked )
		CmtReleaseLock ( LockHandle ); 

	if ( VariableHandle )
		CmtDiscardTSV ( VariableHandle ); 
	
	if ( DRIVER_MANAGER_IsConnectionExistsByType( DRIVER_TYPE_SPECTRUM_ANALYZER ) == 0 )
		FreeLibrary( tDriverInfo.LibraryHandle );
	
	*pHandle = 0;
	
	if ( StdError.error )
	{FREE_CALLOC_COPY_STRING (StdError.pszErrorDescription, "Error while closing instrument driver.");}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Get Lock ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT DRV_SpectrumAnalyzer_GetLock ( int Handle )
{
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;
	
	CmtThreadLockHandle 						LockHandle								=	0;
	
	int											bLocked									=	0;
	
	int											InstrumentType							=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	InstrumentType = pDriverInfo->InstrumentType;
	LockHandle = pDriverInfo->InstrumentLockHandle;
	
	CmtReleaseTSVPtr ( Handle ); 
	
	if ( InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
		
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) == 0 ) 
	{
		pDriverInfo->LockCounter++;
		
		CmtReleaseTSVPtr ( Handle ); 
	}
	
Error:	
	
	return StdError;		
}

/***** ***** ***** ***** ***** ***** ***** Try To Get Lock ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT DRV_SpectrumAnalyzer_TryToGetLock ( int Handle , int *pSuccess)
{
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;
	
	CmtThreadLockHandle 						LockHandle								=	0;
	
	int											iSucces									=	0;
	
	int											InstrumentType							=	0;
	
	IF ((pSuccess == NULL) , "Error passed NULL pointer.");  
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	InstrumentType = pDriverInfo->InstrumentType;
	LockHandle = pDriverInfo->InstrumentLockHandle;
	
	CmtReleaseTSVPtr ( Handle ); 
	
	if ( InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	CmtTryToGetLock ( LockHandle , &iSucces);
	
	*pSuccess = iSucces;
	
	if (iSucces)
		if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) == 0 ) 
		{
			pDriverInfo->LockCounter++;
		
			CmtReleaseTSVPtr ( Handle ); 
		}
Error:	
	
	return StdError;			
}

/***** ***** ***** ***** ***** ***** ***** Release Lock ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT DRV_SpectrumAnalyzer_ReleaseLock ( int Handle )
{
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;
	
	CmtThreadLockHandle 						LockHandle								=	0;
		
	int											LockCounter								=	0;
	
	int											InstrumentType							=	0;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	InstrumentType = pDriverInfo->InstrumentType;
	LockHandle = pDriverInfo->InstrumentLockHandle;	
	LockCounter = pDriverInfo->LockCounter;
	
	CmtReleaseTSVPtr ( Handle );   
	
	if ( InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	if ( LockHandle && LockCounter) 
	{
		CmtReleaseLock (LockHandle);		
		
		if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) == 0 ) 
		{
			pDriverInfo->LockCounter--;
		
			CmtReleaseTSVPtr ( Handle ); 
		}
	}

Error:	
		
	return StdError;		
}

/***** ***** ***** ***** ***** ***** ***** Lock Status ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT DRV_SpectrumAnalyzer_LockStatus ( int Handle , int *iStatus )
{
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;
	
	IF ((iStatus == NULL) , "Error passed NULL pointer.");
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( pDriverInfo->InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	*iStatus = pDriverInfo->LockCounter;		
	
Error:	
	
	if (Handle)
		CmtReleaseTSVPtr ( Handle );
	
	return StdError;		
}

/***** ***** ***** ***** ***** ***** ***** ***** Reset ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Reset ( int Handle )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_Reset					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Reset;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_Reset" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** ***** Preset ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Preset ( int Handle )
{					
	STD_ERROR                                   StdError                                    =   {0};
								
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
	
	CmtThreadLockHandle 								LockHandle								=	0;

	pfSpectrumAnalyzer_Preset							pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Preset;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_Preset" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** ***** Set Frequency Center Span ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetFrequencyCenterSpan ( int Handle , int iChannel , double lfFrequency , double lfSpan )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetFrequencyCenterSpan			pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;	

	char												*pTempString							=	NULL;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetFrequencyCenterSpan;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , lfFrequency , lfSpan )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetFrequencyCenterSpan" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}	  

/***** ***** ***** ***** ***** ***** ***** ***** Configure Marker ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_ConfigureMarker ( int Handle , int iChannel , int marker , int bState )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_ConfigureMarker					pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;	

	char												*pTempString							=	NULL;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_ConfigureMarker;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , marker , bState )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_ConfigureMarker" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}	  

/***** ***** ***** ***** ***** ***** ***** ***** Configure Marker ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetMarkerSearchPeakAuto ( int Handle , int iChannel , int bMax , int bState )
{					
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetMarkerSearchPeakAuto			pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;	
	
	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetMarkerSearchPeakAuto;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , bMax , bState )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetMarkerSearchPeakAuto" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}	  

/***** ***** ***** ***** ***** ***** ***** ***** Get Marker Measure ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetMarkerMeasure ( int Handle , int iChannel , int marker , double *pFrequency , double *pPower )
{					
	STD_ERROR                                   StdError                                    =   {0};
								
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_GetMarkerMeasure					pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;	
	
	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetMarkerMeasure;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , marker , pFrequency , pPower )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_GetMarkerMeasure" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}	  

/***** ***** ***** ***** ***** ***** ***** ***** Get Trace ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetTrace ( int Handle , int iTimeOut , double **pTrace , int numberOfPoints , int *pCount )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_GetTrace							pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;	

	char												*pTempString							=	NULL;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetTrace;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iTimeOut , pTrace , numberOfPoints , pCount )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_GetTrace" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}	  

/***** ***** ***** ***** ***** ***** ***** ***** Config Channel Power Measuring ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_ConfigChannelPowerMeasuring ( int Handle , double lfBandWidth )
{						
	STD_ERROR                                   StdError                                    =   {0};
							
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_ConfigChannelPowerMeasuring		pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;
		
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_ConfigChannelPowerMeasuring;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfBandWidth )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_ConfigChannelPowerMeasuring" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}	

/***** ***** ***** ***** ***** ***** ***** ***** Get Power Result ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetChannelPowerResult ( int Handle , double *plfValue )
{					
	STD_ERROR                                   StdError                                    =   {0};
								
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_GetChannelPowerResult			pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetChannelPowerResult;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , plfValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_GetChannelPowerResult" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}	

/***** ***** ***** ***** ***** ***** ***** ***** Get Start Stop Frequency ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetStartStopFrequency ( int Handle , double *plfStartFrequency , double *plfStopFrequency )
{					
	STD_ERROR                                   StdError                                    =   {0};
								
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_GetStartStopFrequency			pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetStartStopFrequency;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , plfStartFrequency , plfStopFrequency )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_GetStartStopFrequency" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}	

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_ConfigBandWidth( int Handle , double lfResolutionBandWidth , double lfVideoBandWidth )
{					
	STD_ERROR                                   StdError                                    =   {0};
								
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_ConfigBandWidth					pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_ConfigBandWidth;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfResolutionBandWidth , lfVideoBandWidth )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_ConfigBandWidth" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_ConfigSource ( int Handle , int bExternal )
{					
	STD_ERROR                                   StdError                                    =   {0};
								
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_ConfigSource						pWrapperFunction						=	NULL;
												
	int													bLocked									=	0;

	char												*pTempString							=	NULL;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_ConfigSource;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , bExternal )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_ConfigSource" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}




STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetScaleDev( int Handle , double lfScaleDev )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetScaleDev						pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetScaleDev;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfScaleDev )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetScaleDev" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Attenuator( int Handle , double lfAttenuator )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_Attenuator						pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Attenuator;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfAttenuator )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_Attenuator" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetReferenceLevel( int Handle , double lfReferenceLevel )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetReferenceLevel				pWrapperFunction						=	NULL;

	int													bLocked									=	0;

	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetReferenceLevel;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfReferenceLevel )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetReferenceLevel" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
} 

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetReferenceLevel( int Handle , double *vlfReferenceLevel )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_GetReferenceLevel				pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetReferenceLevel;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , vlfReferenceLevel )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_GetReferenceLevel" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
} 

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetAttenuator( int Handle , double *plfAttenuator )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_GetAttenuator					pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetAttenuator;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , plfAttenuator )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_GetAttenuator" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetMarkerType( int Handle , int bDelta )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetMarkerType					pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetMarkerType;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , bDelta )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetMarkerType" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
} 

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetMarkerFrequency( int Handle , int iMarkerNr , double lfFrequency )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetMarkerFrequency				pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetMarkerFrequency;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iMarkerNr , lfFrequency )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetMarkerFrequency" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}  

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetMarkerTimePosition( int Handle , double lfTimePosition )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetMarkerTimePosition			pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetMarkerTimePosition;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfTimePosition )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetMarkerTimePosition" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}  

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetMarkerAmplitude( int Handle , int iMarkerNr , double *vlfAmplitude )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_GetMarkerAmplitude				pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetMarkerAmplitude;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iMarkerNr , vlfAmplitude )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_GetMarkerAmplitude" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}  

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetMarkerDeltaFrequency( int Handle , double *vlfFrequency )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_GetMarkerDeltaFrequency			pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetMarkerDeltaFrequency;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , vlfFrequency )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_GetMarkerDeltaFrequency" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}  

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetMarkerFrequencyCounter( int Handle , int iMarkerNr , int bState )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetMarkerFrequencyCounter		pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetMarkerFrequencyCounter;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iMarkerNr , bState )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetMarkerFrequencyCounter" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetMarkerFrequencyCounter( int Handle , int iMarkerNr , double *vlfFrequency )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_GetMarkerFrequencyCounter		pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetMarkerFrequencyCounter;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iMarkerNr , vlfFrequency )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_GetMarkerFrequencyCounter" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
} 

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetMarkerDeltaAmplitude( int Handle , double *vlfAmplitude )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
											
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_GetMarkerDeltaAmplitude			pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetMarkerDeltaAmplitude;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , vlfAmplitude )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_GetMarkerDeltaAmplitude" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetCenterFrequencyFromMarker( int Handle , int marker )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetCenterFrequencyFromMarker		pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetCenterFrequencyFromMarker;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , marker )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetCenterFrequencyFromMarker" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}  

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetReferenceLevelFrequencyFromMarker( int Handle , int marker )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo												*pDriverInfo							=	NULL,
																tDriverInfo								=	{0};
	
	CmtTSVHandle 												VariableHandle							=	0;
									
	CmtThreadLockHandle 										LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetReferenceLevelFrequencyFromMarker		pWrapperFunction						=	NULL;
	
	int															bLocked									=	0;

	char												*pTempString							=	NULL;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetReferenceLevelFrequencyFromMarker;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , marker )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetReferenceLevelFrequencyFromMarker" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}  

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_WaitForIntrumentDone( int Handle , double lfTimeOut )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_WaitForIntrumentDone				pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_WaitForIntrumentDone;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfTimeOut )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_WaitForIntrumentDone" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}  

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetSweepMode( int Handle , int bContinues )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetSweepMode						pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetSweepMode;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , bContinues )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetSweepMode" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_InitiateMeasure( int Handle )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_InitiateMeasure					pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_InitiateMeasure;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzerSpectrumAnalyzer_InitiateMeasure" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}  

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetAverage( int Handle , int iType  , int iAverage )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetAverage						pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetAverage;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iType  , iAverage )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetAverage" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}  


STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetOffset( int Handle , double lfOffset )  
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetOffset						pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetOffset;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfOffset )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetOffset" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SearchMarkerPeak( int Handle , int iMarkerNr )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_SearchMarkerPeak					pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SearchMarkerPeak;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iMarkerNr )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SearchMarkerPeak" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SearchMarkerNextPeak( int Handle , int iMarkerNr )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_SearchMarkerNextPeak				pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;
											
	char												*pTempString							=	NULL;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SearchMarkerNextPeak;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iMarkerNr )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SearchMarkerNextPeak" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetAuto ( int Handle , int state )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetAuto					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetAuto;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , state )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetAuto" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Config ( int Handle , int Mode , int Type , int Input , int bBeeperState )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_Config					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Config;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , Mode , Type , Input , bBeeperState )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_Config" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetFrequency ( int Handle , double dCenterFreq )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetFrequency				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetFrequency;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , dCenterFreq )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetFrequency" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetStartStopFrequency ( int Handle , double dStartFreq , double dStopFreq )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetStartStopFrequency	pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetStartStopFrequency;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , dStartFreq , dStopFreq )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetStartStopFrequency" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}
  
STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetSpan ( int Handle , double dSpan )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetSpan					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetSpan;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , dSpan )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetSpan" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetAmplitude ( int Handle , double dScale , double dRefLevel )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetAmplitude				pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetAmplitude;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , dScale , dRefLevel )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetAmplitude" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) 
		CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetAttenuation ( int Handle , double dAttenuation )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetAttenuation			pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetAttenuation;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , dAttenuation )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetAttenuation" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetRBW ( int Handle , double dRBW )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetRBW					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetRBW;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , dRBW )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetRBW" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetVBW ( int Handle , double dVBW )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetVBW					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetVBW;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , dVBW )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetVBW" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetBW ( int Handle , double dRBW, double dVBW )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetBW					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetBW;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , dRBW, dVBW )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetBW" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetTraceDetector ( int Handle , int iType )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetTraceDetector			pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetTraceDetector;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iType )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetTraceDetector" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetSweepTime ( int Handle , double lfTime )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetSweepTime				pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetSweepTime;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfTime )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetSweepTime" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetSweep ( int Handle , double lfTimeout )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_GetSweep					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetSweep;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfTimeout )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_GetSweep" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetMarker ( int Handle, int MarkerSetType , int iMarkerNr ,double dMarkerFreq )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetMarker				pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetMarker;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , MarkerSetType , iMarkerNr , dMarkerFreq )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetMarker" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetRLAuto ( int Handle )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetRLAuto				pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetRLAuto;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetRLAuto" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetFrequency ( int Handle , double *pdFrequency )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_GetFrequency				pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetFrequency;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pdFrequency )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_GetFrequency" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetAmplitude ( int Handle , int MarkerSetType , int iMarkerNr , double *pdAmplitude )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_GetAmplitude				pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetAmplitude;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , MarkerSetType , iMarkerNr , pdAmplitude )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_GetAmplitude" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetFreqCountResol ( int Handle , BOOL bMode, int iFreqCountResolution )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetFreqCountResol		pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetFreqCountResol;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , bMode, iFreqCountResolution )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetFreqCountResol" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetAvg ( int Handle , BOOL bMode, int iAvg )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetAvg					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetAvg;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , bMode, iAvg )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetAvg" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetMaxHold ( int Handle , int iState )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetMaxHold				pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetMaxHold;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iState )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetMaxHold" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetMarkerLevel ( int Handle , int iMarkerNr , double *pLevel )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_GetMarkerLevel			pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetMarkerLevel;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iMarkerNr , pLevel )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_GetMarkerLevel" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetMarkerFrequency ( int Handle , int iMarkerNr , double *pFrequecy )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_GetMarkerFrequency		pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetMarkerFrequency;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iMarkerNr , pFrequecy )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_GetMarkerFrequency" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_AutoSetIsDone ( int Handle , double lfTimeOut , int *bDone )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_AutoSetIsDone			pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_AutoSetIsDone;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfTimeOut , bDone )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_AutoSetIsDone" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT DRV_SpectrumAnalyzer_SetAutoTuneThreshold( int Handle, double lfValue ) 
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetAutoTuneThreshold		pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetAutoTuneThreshold;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetAutoTuneThreshold" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetNumberOfPoints( int Handle , int points )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
									
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetNumberOfPoints		pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetNumberOfPoints;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , points )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetNumberOfPoints" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
} 

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Recall ( int Handle , char *szFileName  )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo								=	NULL,
												tDriverInfo									=	{0};
	
	CmtTSVHandle 								VariableHandle								=	0;
	
	CmtThreadLockHandle 						LockHandle									=	0;
										
	pfSpectrumAnalyzer_Recall					pWrapperFunction							=	NULL;
	
	int											iTryIndex									=	0;
	
	int											bLocked										=	0;
	
	char										*pTempString								=	NULL,
												*pFileNamePath								=	NULL;   
												
	tsSTD_CallBackSet							tSTD_CallBackSet							=	{0};

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	STDF_UPDATE_CALLBACK_DATA( pDriverInfo->ptCallbacks); 
	
	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Recall;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
	for ( iTryIndex = 0; iTryIndex < 2; iTryIndex++ )
	{
	    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		{
			FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , szFileName )); 
			
			if ( IS_NOT_OK )
			{
				if ( szFileName )
				{
					STDF_GET_EQUIPMENT_STATE_FILE( szFileName , pFileNamePath );
				
					CmtReleaseLock (LockHandle);  
					bLocked = 0;
					
					CHK_STDERR( DRIVER_MANAGER_Equipment_SendStateFile ( Handle , pFileNamePath , szFileName )); 
					
					CHK_PROCCESS_GET_LOCK ( LockHandle ); 
				}
			}
			else
			{
				break;	
			}
		}
		else
		{
			break;
		}
	}
	
	
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_Recall" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetMode ( int Handle , int iMode , int iSubMode )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetMode					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetMode;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iMode , iSubMode )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetMode" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) 
		CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetWiMaxMeasure ( int Handle , int iMeasureType , int iMeasureSubType , int iMeasureValueType , double *plfResult )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_GetWiMaxMeasure			pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetWiMaxMeasure;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iMeasureType , iMeasureSubType , iMeasureValueType , plfResult )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_GetWiMaxMeasure" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetToMode_Specrum ( int Handle )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetToMode_Specrum		pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetToMode_Specrum;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetToMode_Specrum" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_RecallState( int Handle , char *szFileName )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo								=	NULL,
												tDriverInfo									=	{0};
	
	CmtTSVHandle 								VariableHandle								=	0;
	
	CmtThreadLockHandle 						LockHandle									=	0;
										
	pfSpectrumAnalyzer_RecallState				pWrapperFunction							=	NULL;
																								  
	int											iTryIndex									=	0;
	
	int											bLocked										=	0;
	
	char										*pTempString								=	NULL,
												*pFileNamePath								=	NULL;   
												
	tsSTD_CallBackSet							tSTD_CallBackSet							=	{0};
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	FREE_CALLOC_COPY_STRING( pDriverInfo->pLastStateFileName , szFileName );
	
	STDF_UPDATE_CALLBACK_DATA( pDriverInfo->ptCallbacks); 
	
	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_RecallState;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	for ( iTryIndex = 0; iTryIndex < 2; iTryIndex++ )
	{
	    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		{
			FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , szFileName )); 
			
			if ( IS_NOT_OK )
			{
				if ( szFileName )
				{
					STDF_GET_EQUIPMENT_STATE_FILE( szFileName , pFileNamePath );
				
					CmtReleaseLock (LockHandle);  
					bLocked = 0;
					
					CHK_STDERR( DRIVER_MANAGER_Equipment_SendStateFile ( Handle , pFileNamePath , szFileName )); 
					
					CHK_PROCCESS_GET_LOCK ( LockHandle ); 
				}
			}
		}
		else
		{
			break;
		}
	}
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_RecallState" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) 
		CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SaveState( int Handle ,char *szFileName )
{				
	STD_ERROR                                   StdError                                =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_SaveState				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	FREE_CALLOC_COPY_STRING( pDriverInfo->pLastStateFileName , szFileName );
	
	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SaveState;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , szFileName )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SaveState" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetLastStateFileName( int Handle ,char **pszFileName )
{				
	STD_ERROR                                   StdError                                =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_GetLastStateFileName		pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL,
												*pLastStateFileName						=	NULL;
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	FREE_CALLOC_COPY_STRING( pLastStateFileName  , pDriverInfo->pLastStateFileName );
	
	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetLastStateFileName;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
	{
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pszFileName )); 
	}
	else
	{
		FREE_CALLOC_COPY_STRING( *pszFileName , pLastStateFileName );
	}

Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	FREE(pLastStateFileName);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Read_EVM( int Handle , double *plfEVM_Value )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_Read_EVM					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Read_EVM;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , plfEVM_Value )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_Read_EVM" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetFrequency_EVM ( int Handle , double lfFrequency )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_SetFrequency_EVM			pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetFrequency_EVM;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfFrequency )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetFrequency_EVM" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_RestartMeasure( int Handle )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_RestartMeasure			pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_RestartMeasure;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_RestartMeasure" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Read_SymbolStream( int Handle ,int iNumberOfSymbols, char **pSymbolStream,int *piNumberOfSymbolsRead)
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSpectrumAnalyzer_Read_SymbolStream		pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Read_SymbolStream;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iNumberOfSymbols , pSymbolStream , piNumberOfSymbolsRead )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_Read_SymbolStream" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** ***** Get Marker Measure ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_PhaseNoise_GetMarker ( int Handle , int iChannel , int marker , double lfTimeOut , double *pPosition , double *pValue )
{					
	STD_ERROR                                   StdError                                    =   {0};
								
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_PhaseNoise_GetMarker				pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;	
	
	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_PhaseNoise_GetMarker;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , marker , lfTimeOut , pPosition , pValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_PhaseNoise_GetMarker" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** ***** Get Integrated Marker Measure ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_PhaseNoise_GetIntegratedMarker ( int Handle , int iChannel , int marker , double lfTimeOut ,double lfMarkerStartFreq, double lfMarkerStopFreq, double *pPosition , double *pValue )
{					
	STD_ERROR                                 			StdError								=   {0};
								
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_PhaseNoise_GetIntegratedMarker	pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;	
	
	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
																				  
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_PhaseNoise_GetIntegratedMarker;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , marker , lfTimeOut ,lfMarkerStartFreq, lfMarkerStopFreq,  pPosition , pValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_PhaseNoise_GetIntegratedMarker" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}
/***** ***** ***** ***** ***** ***** ***** ***** Set Integrated Marker Start Stop Offsets ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_PhaseNoise_SetIntegratedMarker_Start_Stop_Offsets ( int Handle , int iChannel , int marker , double lfTimeOut , double lfStartFreq , double lfStopFreq )
{					
	STD_ERROR                                   							StdError								=   {0};
								
	tsDriverInfo															*pDriverInfo							=	NULL,
																			tDriverInfo								=	{0};
	
	CmtTSVHandle 															VariableHandle							=	0;
									
	CmtThreadLockHandle 													LockHandle								=	0;
										
	pfSpectrumAnalyzer_PhaseNoise_SetIntegratedMarker_Start_Stop_Offsets	pWrapperFunction						=	NULL;
	
	int																		bLocked									=	0;	
	
	char																	*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_PhaseNoise_SetIntegratedMarker_Start_Stop_Offsets;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , marker , lfTimeOut , lfStartFreq , lfStopFreq )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[1024]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_PhaseNoise_SetIntegratedMarker_Start_Stop_Offsets" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** ***** Auto Tune ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_AutoTune ( int Handle , double lfTimeout )
{					
	STD_ERROR                                   StdError                                    =   {0};
								
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_AutoTune							pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;	
	
	char												*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_AutoTune;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfTimeout )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_AutoTune" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_GetLossFactor( int Handle , double lfFrequency , double *vlfLossFactor )
{				
	STD_ERROR                           		        StdError                                    =   {0};
									
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
									
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfSpectrumAnalyzer_GetReferenceLevel				pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;

	char												*pTempString							=	NULL;

	double												lfCurrentReferenceLevel					=	0.0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_GetReferenceLevel;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if (( pWrapperFunction ) && ( pDriverInfo->bDemoMode == 0 ))
	{
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , &lfCurrentReferenceLevel )); 
	}
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_GetReferenceLevel" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( IS_OK )
	{
		UPDATERR( DRIVER_MANAGER_CALIBRATION_Gel_3D_Factor( Handle , lfCurrentReferenceLevel , lfFrequency , vlfLossFactor ));   
	}
	
	if ( IS_NOT_OK )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Read_IP3( int Handle , double *plfIP3_Value )
{					
	STD_ERROR                                   		StdError                                    =   {0};
								
	tsDriverInfo										*pDriverInfo								=	NULL,
														tDriverInfo									=	{0};
	
	CmtTSVHandle 										VariableHandle								=	0;
									
	CmtThreadLockHandle 								LockHandle									=	0;
										
	pfSpectrumAnalyzer_Read_IP3							pWrapperFunction							=	NULL;
	
	int													bLocked										=	0;	
	
	char												*pTempString								=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Read_IP3;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , plfIP3_Value )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_Read_IP3" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Read_IP2( int Handle , double *plfIP2_Value )  
{					
	STD_ERROR                                   		StdError                                    =   {0};
								
	tsDriverInfo										*pDriverInfo								=	NULL,
														tDriverInfo									=	{0};
	
	CmtTSVHandle 										VariableHandle								=	0;
									
	CmtThreadLockHandle 								LockHandle									=	0;
										
	pfSpectrumAnalyzer_Read_IP2							pWrapperFunction							=	NULL;
	
	int													bLocked										=	0;	
	
	char												*pTempString								=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Read_IP2;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , plfIP2_Value )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_Read_IP2" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Read_Harmonics_Distortion( int Handle , double *plfTHD_Value )  
{					
	STD_ERROR                                   		StdError                                    =   {0};
								
	tsDriverInfo										*pDriverInfo								=	NULL,
														tDriverInfo									=	{0};
	
	CmtTSVHandle 										VariableHandle								=	0;
									
	CmtThreadLockHandle 								LockHandle									=	0;
										
	pfSpectrumAnalyzer_Read_Harmonics_Distortion		pWrapperFunction							=	NULL;
	
	int													bLocked										=	0;	
	
	char												*pTempString								=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Read_Harmonics_Distortion;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , plfTHD_Value )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_Read_Harmonics_Distortion" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Read_Harmonics_dBc( int Handle , double *plfTHD_Value )  
{					
	STD_ERROR                                   		StdError                                    =   {0};
								
	tsDriverInfo										*pDriverInfo								=	NULL,
														tDriverInfo									=	{0};
	
	CmtTSVHandle 										VariableHandle								=	0;
									
	CmtThreadLockHandle 								LockHandle									=	0;
										
	pfSpectrumAnalyzer_Read_Harmonics_dBc				pWrapperFunction							=	NULL;
	
	int													bLocked										=	0;	
	
	char												*pTempString								=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Read_Harmonics_dBc;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , plfTHD_Value )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_Read_Harmonics_dBc" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Read_Harmonics_Amplitude( int Handle , int iHarmonicIndex , double *plfValue )  
{					
	STD_ERROR                                   		StdError                                    =   {0};
								
	tsDriverInfo										*pDriverInfo								=	NULL,
														tDriverInfo									=	{0};
	
	CmtTSVHandle 										VariableHandle								=	0;
									
	CmtThreadLockHandle 								LockHandle									=	0;
										
	pfSpectrumAnalyzer_Read_Harmonics_Amplitude			pWrapperFunction							=	NULL;
	
	int													bLocked										=	0;	
	
	char												*pTempString								=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Read_Harmonics_Amplitude;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iHarmonicIndex , plfValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_Read_Harmonics_Amplitude" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_Read_Spurious_Emissions_List( int Handle , int *piNumberOfSpurious , int **pvRangeNumber , int **pvPassFail , double **pvlfSpurFrequency , double **pvlfSpurAmplitude , double **pvlfSpurLimit )
{					
	STD_ERROR                                   		StdError                                    =   {0};
								
	tsDriverInfo										*pDriverInfo								=	NULL,
														tDriverInfo									=	{0};
	
	CmtTSVHandle 										VariableHandle								=	0;
									
	CmtThreadLockHandle 								LockHandle									=	0;
										
	pfSpectrumAnalyzer_Read_Spurious_Emissions_List		pWrapperFunction							=	NULL;
	
	int													bLocked										=	0;	
	
	char												*pTempString								=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_Read_Spurious_Emissions_List;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , piNumberOfSpurious , pvRangeNumber , pvPassFail , pvlfSpurFrequency , pvlfSpurAmplitude , pvlfSpurLimit )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_Read_Spurious_Emissions_List" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_MeasureMarkerFrequencyCounter( int Handle , int iMarkerNr , double lfTimeout , double *vlfFrequency )  
{					
	STD_ERROR                                   		StdError                                    =   {0};
								
	tsDriverInfo										*pDriverInfo								=	NULL,
														tDriverInfo									=	{0};
	
	CmtTSVHandle 										VariableHandle								=	0;
									
	CmtThreadLockHandle 								LockHandle									=	0;
										
	pfSpectrumAnalyzer_MeasureMarkerFrequencyCounter	pWrapperFunction							=	NULL;
	
	int													bLocked										=	0;	
	
	char												*pTempString								=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_MeasureMarkerFrequencyCounter;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iMarkerNr , lfTimeout , vlfFrequency )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_MeasureMarkerFrequencyCounter" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_AlignNow ( int Handle , double lfTimeout )
{					
	STD_ERROR                                   		StdError                                    =   {0};
								
	tsDriverInfo										*pDriverInfo								=	NULL,
														tDriverInfo									=	{0};
	
	CmtTSVHandle 										VariableHandle								=	0;
									
	CmtThreadLockHandle 								LockHandle									=	0;
										
	pfSpectrumAnalyzer_AlignNow							pWrapperFunction							=	NULL;
	
	int													bLocked										=	0;	
	
	char												*pTempString								=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_AlignNow;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfTimeout )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_AlignNow" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_NoiseFigure_CalibrateNow ( int Handle , double lfTimeout )
{					
	STD_ERROR                                   		StdError                                    =   {0};
								
	tsDriverInfo										*pDriverInfo								=	NULL,
														tDriverInfo									=	{0};
	
	CmtTSVHandle 										VariableHandle								=	0;
									
	CmtThreadLockHandle 								LockHandle									=	0;
										
	pfSpectrumAnalyzer_NoiseFigure_CalibrateNow			pWrapperFunction							=	NULL;
	
	int													bLocked										=	0;	
	
	char												*pTempString								=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_NoiseFigure_CalibrateNow;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfTimeout )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_NoiseFigure_CalibrateNow" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_NoiseFigure_ApplyCalibration ( int Handle , double lfTimeout )
{					
	STD_ERROR                                   		StdError                                    =   {0};
								
	tsDriverInfo										*pDriverInfo								=	NULL,
														tDriverInfo									=	{0};
	
	CmtTSVHandle 										VariableHandle								=	0;
									
	CmtThreadLockHandle 								LockHandle									=	0;
										
	pfSpectrumAnalyzer_NoiseFigure_ApplyCalibration		pWrapperFunction							=	NULL;
	
	int													bLocked										=	0;	
	
	char												*pTempString								=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_NoiseFigure_ApplyCalibration;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfTimeout )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_NoiseFigure_ApplyCalibration" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_NoiseFigure_SetLossCompensationTable ( int Handle , int bAfterDUT , double *pvFrequencies, double *pvLoss , int iNumberOfPoints )
{					
	STD_ERROR                             			      			StdError                                    =   {0};
								
	tsDriverInfo													*pDriverInfo								=	NULL,
																	tDriverInfo									=	{0};
	
	CmtTSVHandle 													VariableHandle								=	0;
									
	CmtThreadLockHandle 											LockHandle									=	0;
										
	pfSpectrumAnalyzer_NoiseFigure_SetLossCompensationTable			pWrapperFunction							=	NULL;
	
	int																bLocked										=	0;	
	
	char															*pTempString								=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_NoiseFigure_SetLossCompensationTable;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , bAfterDUT , pvFrequencies, pvLoss , iNumberOfPoints )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_NoiseFigure_SetLossCompensationTable" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


STD_ERROR   DLLEXPORT	DRV_SpectrumAnalyzer_SetTriggerSource( int Handle , int iSource )
{					
	STD_ERROR                             			      			StdError                                    =   {0};
								
	tsDriverInfo													*pDriverInfo								=	NULL,
																	tDriverInfo									=	{0};
	
	CmtTSVHandle 													VariableHandle								=	0;
									
	CmtThreadLockHandle 											LockHandle									=	0;
										
	pfSpectrumAnalyzer_SetTriggerSource								pWrapperFunction							=	NULL;
	
	int																bLocked										=	0;	
	
	char															*pTempString								=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SPECTRUM_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.spectrumAnalyzerDriverFunctions.SpectrumAnalyzer_SetTriggerSource;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iSource )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SpectrumAnalyzer_SetTriggerSource" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SpectrumAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


/*
extern CmtTSVHandle			gGlobalDriverCommonHandle;

int main ()
{
	STD_ERROR									StdError									=	{0};

	int								hInstrumentHandle								=	0; 
										

	CmtNewTSV ( sizeof(tsGlobalHandle) , &gGlobalDriverCommonHandle );

	DRV_SpectrumAnalyzer_Init ( "Agilent_SpectrumAnalyzer_Driver.dll" , "TCPIP::10.0.1.11" , &hInstrumentHandle );  
	
	DRV_SpectrumAnalyzer_Reset(hInstrumentHandle);
	
	DRV_SpectrumAnalyzer_RecallState( hInstrumentHandle , "calibration.state" );  

Error:
	
	return 0;
}
*/


