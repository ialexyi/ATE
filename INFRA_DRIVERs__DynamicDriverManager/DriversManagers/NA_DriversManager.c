//==============================================================================
// Include files

#include "MAIN_DriversManager.h"
#include "NA_DriversManager.h"
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

int DLLEXPORT	DRV_NetworkAnalyzer_GetErrorTextMessage ( int Handle , int iError , char **pErrorMessage )  
{		  	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
	
	pfNetworkAnalyzer_GetErrorTextMessage		pWrapperFunction						=	NULL;
	
	STD_ERROR                 					StdError								=	{0};
	
	int											bLocked									=	0;
	
	if ((pErrorMessage == NULL) || (Handle == 0))
		return DRV_ERROR_PASSED_NULL;
	
	CALLOC_ERR( *pErrorMessage , 256 , sizeof(char));
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		return DRV_ERROR_GET_TSV_POINTER;
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_MULTI_METER )
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
			
			LockHandle = tDriverInfo.InstrumentLockHandle;
			
			pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetErrorTextMessage;

			CHK_PROCCESS_GET_LOCK ( LockHandle );
			
		    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
				FREE_STDERR_COPY( pWrapperFunction( tDriverInfo.InstrumentHandle , iError , (*pErrorMessage) ));
	
			if ( pWrapperFunction == NULL )
			{
				char	szMessage[LOW_STRING]	= {0};
		
				sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_GetErrorTextMessage" , tDriverInfo.pDriverFileName );
		
				ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
			}
	
			break;
	}
	
Error:
	
	if ( LockHandle && bLocked )
		CmtReleaseLock (LockHandle);
	
	RETURN_ERROR_VALUE;
}

/***** ***** ***** ***** ***** ***** ***** ***** Init ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Init ( char *pszDriverLocation , char *pszAddressString , int *pHandle )
{
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
	
	pfNetworkAnalyzer_Init						pWrapperFunction						=	NULL;
	
	char										szCurrentDirectory[STD_STRING]			=	{0},
												szDriverLocation[STD_STRING]			=	{0},
												szMessage[STD_STRING]					=	{0}, 
												*pTemp									=	NULL;

	char										*pTempString							=	NULL;
	
	int											iTry									=	0,
												bHandleExists							=	0;
	
	int											bLocked									=	0;
	
	int											iPort									=	0;
												
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
		sprintf( szMessage , "Loading Network Analyzer Driver\n\"%s\"\n\nFailed, File has not found." , szDriverLocation );
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
			if ( pHandle )
				*pHandle = VariableHandle;
	
			if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
			{
				{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
				break;
			}
	
			pDriverInfo->InstrumentType = DRIVER_TYPE_NETWORK_ANALYZER;
	
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
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.Config_Install_CommentCallback							= (pfSTD_Config_Install_CommentCallback) GetProcAddress( pDriverInfo->LibraryHandle , 		"Config_Install_CommentCallback");      
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.Config_Install_ConfigValueCallback						= (pfSTD_Config_Install_ConfigValueCallback) GetProcAddress( pDriverInfo->LibraryHandle , 	"Config_Install_ConfigValueCallback");  
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.Config_Install_CheckForBreakCallback					= (pfSTD_Config_Install_CheckForBreakCallback) GetProcAddress( pDriverInfo->LibraryHandle , "Config_Install_CheckForBreakCallback");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.Config_LOG_SetAllowState								= (pfSTD_Config_LOG_SetAllowState) GetProcAddress( pDriverInfo->LibraryHandle , "Config_LOG_SetAllowState");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.Config_Copy_STD_CallBackSet							= (pfSTD_Config_Copy_STD_CallBackSet) GetProcAddress( pDriverInfo->LibraryHandle , "Config_Copy_STD_CallBackSet");
			
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Init                  	  		    	= (pfNetworkAnalyzer_Init) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_Init");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Close                  	  		    = (pfNetworkAnalyzer_Close) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_Close");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Reset                  	  		    = (pfNetworkAnalyzer_Reset) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_Reset");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetConnectedPort              		    = (pfNetworkAnalyzer_GetConnectedPort) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_GetConnectedPort");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Calibration_SetFrequencyPlan           = (pfNetworkAnalyzer_Calibration_SetFrequencyPlan) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_Calibration_SetFrequencyPlan");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SelectCalibrationFromFile              = (pfNetworkAnalyzer_SelectCalibrationFromFile) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SelectCalibrationFromFile");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Calibration_Measure                  	= (pfNetworkAnalyzer_Calibration_Measure) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_Calibration_Measure");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Calibration_Apply                  	= (pfNetworkAnalyzer_Calibration_Apply) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_Calibration_Apply");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Kit_SelectFromFile                  	= (pfNetworkAnalyzer_Kit_SelectFromFile) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_Kit_SelectFromFile");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_MeasureSingleSweep                  	= (pfNetworkAnalyzer_MeasureSingleSweep) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_MeasureSingleSweep");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetAverage                  	  		= (pfNetworkAnalyzer_SetAverage) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SetAverage");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetSmoothing                  	  		= (pfNetworkAnalyzer_SetSmoothing) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SetSmoothing");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetDwellTime                  	  		= (pfNetworkAnalyzer_SetDwellTime) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SetDwellTime");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetPowerLevel                  	  	= (pfNetworkAnalyzer_SetPowerLevel) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SetPowerLevel");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetReferencePlane                  	= (pfNetworkAnalyzer_SetReferencePlane) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SetReferencePlane");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SaveMeasurementToMemory                = (pfNetworkAnalyzer_SaveMeasurementToMemory) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SaveMeasurementToMemory");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_ApplyMemoryMath                  	  	= (pfNetworkAnalyzer_ApplyMemoryMath) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_ApplyMemoryMath");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetSystemImpedance                  	= (pfNetworkAnalyzer_SetSystemImpedance) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SetSystemImpedance");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetData                  	  		    = (pfNetworkAnalyzer_GetData) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_GetData");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetMemory                  	  		= (pfNetworkAnalyzer_GetMemory) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_GetMemory");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_FindDataPoint                  	  	= (pfNetworkAnalyzer_FindDataPoint) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_FindDataPoint");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetTrace                  				= (pfNetworkAnalyzer_GetTrace) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_GetTrace");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetInfo                  	  		    = (pfNetworkAnalyzer_GetInfo) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_GetInfo");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SaveStateAndCalibration               = (pfNetworkAnalyzer_SaveStateAndCalibration) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SaveStateAndCalibration");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_LoadStateAndCalibration               = (pfNetworkAnalyzer_LoadStateAndCalibration) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_LoadStateAndCalibration");
		
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetErrorTextMessage               		= (pfNetworkAnalyzer_GetErrorTextMessage) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_GetErrorTextMessage");   
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Reset             				  		= (pfNetworkAnalyzer_Reset) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_Reset");                 
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetFrequency							= (pfNetworkAnalyzer_SetFrequency) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SetFrequency");          
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetFrequency							= (pfNetworkAnalyzer_GetFrequency) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_GetFrequency");          
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetPointsNum							= (pfNetworkAnalyzer_SetPointsNum) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SetPointsNum");          
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetPointsNum							= (pfNetworkAnalyzer_GetPointsNum) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_GetPointsNum");          
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetMeasureVectors						= (pfNetworkAnalyzer_GetMeasureVectors) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_GetMeasureVectors");     
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetMeasureType							= (pfNetworkAnalyzer_SetMeasureType) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SetMeasureType");        
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_DisplaySelect							= (pfNetworkAnalyzer_DisplaySelect) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_DisplaySelect");         
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_AutoScale 								= (pfNetworkAnalyzer_AutoScale) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_AutoScale");             
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_AvarageSmooth							= (pfNetworkAnalyzer_AvarageSmooth) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_AvarageSmooth");         
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Smooth									= (pfNetworkAnalyzer_Smooth) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_Smooth");                
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetBandWidth							= (pfNetworkAnalyzer_SetBandWidth) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SetBandWidth");          
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetFrequencySweepType					= (pfNetworkAnalyzer_SetFrequencySweepType) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SetFrequencySweepType"); 
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetPower								= (pfNetworkAnalyzer_SetPower) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SetPower");              
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetPower								= (pfNetworkAnalyzer_GetPower) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_GetPower");              
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetPowerRange							= (pfNetworkAnalyzer_SetPowerRange) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SetPowerRange");         
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetPowerSweep							= (pfNetworkAnalyzer_SetPowerSweep) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SetPowerSweep");         
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetSweepTime							= (pfNetworkAnalyzer_SetSweepTime) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SetSweepTime");          
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetMarker								= (pfNetworkAnalyzer_SetMarker) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SetMarker");             
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetMarker								= (pfNetworkAnalyzer_GetMarker) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_GetMarker");             
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_ReCallRegister							= (pfNetworkAnalyzer_ReCallRegister) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_ReCallRegister");        
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SaveToRegister							= (pfNetworkAnalyzer_SaveToRegister) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SaveToRegister");        
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_QuaryCalibration						= (pfNetworkAnalyzer_QuaryCalibration) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_QuaryCalibration");      
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetPointsNum							= (pfNetworkAnalyzer_SetPointsNum) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SetPointsNum");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetPointsNum							= (pfNetworkAnalyzer_GetPointsNum) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_GetPointsNum");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetFrequencySweepType					= (pfNetworkAnalyzer_SetFrequencySweepType) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_GetPointsNum");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SelectChannel							= (pfNetworkAnalyzer_SelectChannel) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SelectChannel");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetLastStateFileName					= (pfNetworkAnalyzer_GetLastStateFileName) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_GetLastStateFileName");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SelectMathFunction						= (pfNetworkAnalyzer_SelectMathFunction) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SelectMathFunction");     
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetTraceToMathMemory					= (pfNetworkAnalyzer_SetTraceToMathMemory) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SetTraceToMathMemory");     
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_ECAL_GetInformation					= (pfNetworkAnalyzer_ECAL_GetInformation) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_ECAL_GetInformation");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_ECAL_Calibrate							= (pfNetworkAnalyzer_ECAL_Calibrate) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_ECAL_Calibrate");     
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetElectricalDelayTime					= (pfNetworkAnalyzer_SetElectricalDelayTime) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SetElectricalDelayTime");

			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SelectTrace							= (pfNetworkAnalyzer_SelectTrace) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SelectTrace");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SelectWindow							= (pfNetworkAnalyzer_SelectWindow) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SelectWindow");

			pDriverInfo->tInstrDB.Equipment_Info  																		= (pfSTD_Equipment_Info) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_Info");      
			pDriverInfo->tInstrDB.Equipment_IsSupported																	= (pfSTD_Equipment_IsSupported) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_IsSupported");   
		
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Avarage								= (pfNetworkAnalyzer_Avarage) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_Avarage");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_ClearAvarage							= (pfNetworkAnalyzer_ClearAvarage) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_ClearAvarage");

			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Reponse_OpenShortLoad_Calibrate		= (pfNetworkAnalyzer_Reponse_OpenShortLoad_Calibrate) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_Reponse_OpenShortLoad_Calibrate");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Reponse_Thru_Calibrate					= (pfNetworkAnalyzer_Reponse_Thru_Calibrate) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_Reponse_Thru_Calibrate");
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Reponse_Done_Calibrate					= (pfNetworkAnalyzer_Reponse_Done_Calibrate) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_Reponse_Done_Calibrate");
			
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_WaitSweep								= (pfNetworkAnalyzer_WaitSweep) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_WaitSweep");
			
			pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetCalibrationName						= (pfNetworkAnalyzer_SetCalibrationName) GetProcAddress( pDriverInfo->LibraryHandle , "NetworkAnalyzer_SetCalibrationName");
			
			pDriverInfo->tInstrDB.Equipment_SendStateFile																= (pfSTD_Equipment_SendStateFile) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_SendStateFile");
			pDriverInfo->tInstrDB.Equipment_ReceiveStateFile															= (pfSTD_Equipment_ReceiveStateFile) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_ReceiveStateFile");
			pDriverInfo->tInstrDB.Equipment_GetFileCatalog																= (pfSTD_Equipment_GetFileCatalog) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_GetFileCatalog"); 			
			pDriverInfo->tInstrDB.Equipment_MakeFileCatalog																= (pfSTD_Equipment_MakeFileCatalog) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_MakeFileCatalog"); 			
			pDriverInfo->tInstrDB.Equipment_DeleteFileCatalog															= (pfSTD_Equipment_DeleteFileCatalog) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_DeleteFileCatalog");
			pDriverInfo->tInstrDB.Equipment_DeleteStateFile																= (pfSTD_Equipment_DeleteStateFile) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_DeleteStateFile");
			
			pDriverInfo->tInstrDB.Config_SetAttribute																	= (pfSTD_Config_SetAttribute) GetProcAddress( pDriverInfo->LibraryHandle , "Config_SetAttribute");
			
			pWrapperFunction = pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Init;
	
			if ( pWrapperFunction == NULL )
			{
				char	szMessage[LOW_STRING]	= {0};
				
				sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_Init" , pDriverInfo->pDriverFileName );
				
				ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
			
				{STD_ERR (DRV_ERROR_WRONG_WRAPPER);}
			}
		
			if ( !bHandleExists )
				DRIVER_MANAGER_AddConnection( pszAddressString , &(pDriverInfo->InstrumentHandle) , &(pDriverInfo->InstrumentLockHandle) );
		
			LockHandle = pDriverInfo->InstrumentLockHandle;
		
			if ( LockHandle == 0 )
				{STD_ERR (DRV_ERROR_CREATE_LOCK_HANDLE);}
		
			CHK_PROCCESS_GET_LOCK ( LockHandle );
		
			DRIVER_MANAGER_IsConnectionExists( pszAddressString, &(pDriverInfo->InstrumentHandle) , NULL );
	
			//===================== Install Calbacks ==========================================================================================//
			pConfig_Install_CommentCallback = pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.Config_Install_CommentCallback;		
			pConfig_Install_ConfigValueCallback	= pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.Config_Install_ConfigValueCallback;		
			pConfig_Install_CheckForBreakCallback = pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.Config_Install_CheckForBreakCallback;		
			pConfig_Copy_STD_CallBackSet = pDriverInfo->tInstrDB.networkAnalyzerDriverFunctions.Config_Copy_STD_CallBackSet;
		
			DRIVER_MANAGER_GetCopyCallbacksStructure( VariableHandle , &(pDriverInfo->ptCallbacks) , 1 , pszAddressString ); 
			
			if ( pConfig_Install_CommentCallback )
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

			if ( pConfig_Copy_STD_CallBackSet && ( pDriverInfo->ptCallbacks ))
			{
				FREE_STDERR_COPY_ERR( pConfig_Copy_STD_CallBackSet( &pDriverInfo->InstrumentHandle , pDriverInfo->ptCallbacks ));
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
					if ( ShowMessage ( INSTR_TYPE_YES_NO , "Network Analyzer Error !!!", "Check connection and Power On the Device." , NULL ) )
						continue;
			
				if (!HasNonWhiteSpace (pszAddressString))
				{
					CHK_STDERR( DRV_NetworkAnalyzer_GetConnectedPort ( VariableHandle , &iPort ));
				
					sprintf(pDriverInfo->pInstrumentAddress , "ASRL%d" , iPort);
				}
			
				break;
			}
		
			CHK_STDERR(StdError);
		
			if (pDriverInfo->bConnected == 0)
				{STD_ERR (DRV_ERROR_CONNECTION);}
	
			if ( !bHandleExists ) 
				DRIVER_MANAGER_UpdateConnection( pDriverInfo->pInstrumentAddress , pDriverInfo->InstrumentHandle , NULL );

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
		DRV_NetworkAnalyzer_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
		DRV_NetworkAnalyzer_Close( &VariableHandle ); 
		
		if ( pHandle )
			*pHandle = 0;
	}

	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** ***** Close ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Close ( int *pHandle )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_Close						pWrapperFunction						=	NULL;
	
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
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Close;
	
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
		
		DRIVER_MANAGER_RemoveConnectionExists( tDriverInfo.pInstrumentAddress );
	}
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_Close" , tDriverInfo.pDriverFileName );

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
	
	if ( tDriverInfo.ptCallbacks )
	{
		FREE( (tDriverInfo.ptCallbacks)->pCommentCallbackData );
		FREE( (tDriverInfo.ptCallbacks)->pConfigValueCallbackData );
		FREE( (tDriverInfo.ptCallbacks)->pCheckForBreakCallbackData ); 
		FREE( (tDriverInfo.ptCallbacks)->pFileCallbackData );
		
		FREE(tDriverInfo.ptCallbacks);
	}
	
Error:
	
	if ( LockHandle && bLocked )
		CmtReleaseLock ( LockHandle ); 

	if ( VariableHandle )
		CmtDiscardTSV ( VariableHandle ); 
	
	FreeLibrary( tDriverInfo.LibraryHandle );
	
	*pHandle = 0;
	
	if ( StdError.error )
	{FREE_CALLOC_COPY_STRING (StdError.pszErrorDescription, "Error while closing instrument driver.");}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** ***** Connected Port ***** ***** ***** ***** ***** ***** ***** *****/ 

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_GetConnectedPort ( int Handle , int *iPort )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_GetConnectedPort			pWrapperFunction						=	NULL;

	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetConnectedPort;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iPort )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_GetConnectedPort" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}



/***** ***** ***** ***** ***** ***** ***** Get Lock ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT DRV_NetworkAnalyzer_GetLock ( int Handle )
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
	
	if ( InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
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

STD_ERROR   DLLEXPORT DRV_NetworkAnalyzer_TryToGetLock ( int Handle , int *pSuccess)
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
	
	if ( InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
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

STD_ERROR   DLLEXPORT DRV_NetworkAnalyzer_ReleaseLock ( int Handle )
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
	
	if ( InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
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

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_LockStatus ( int Handle , int *iStatus )
{
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;
	
	IF ((iStatus == NULL) , "Error passed NULL pointer.");
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( pDriverInfo->InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	*iStatus = pDriverInfo->LockCounter;		
	
Error:	
	
	if (Handle)
		CmtReleaseTSVPtr ( Handle );
	
	return StdError;		
}

/***** ***** ***** ***** ***** ***** ***** Reset ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Reset ( int Handle )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_Reset						pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Reset;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_Reset" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Calibration_SetFrequencyPlan ( int Handle , double lfStartFrequncy , double lfStopFrequency , double lfPowerLevel , double lfReferenceLevel , int iNumberOfPoints )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo									*pDriverInfo							=	NULL,
													tDriverInfo								=	{0};
	
	CmtTSVHandle 									VariableHandle							=	0;
	
	CmtThreadLockHandle 							LockHandle								=	0;
										
	pfNetworkAnalyzer_Calibration_SetFrequencyPlan	pWrapperFunction						=	NULL;
	
	int												bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Calibration_SetFrequencyPlan;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfStartFrequncy , lfStopFrequency , lfPowerLevel , lfReferenceLevel , iNumberOfPoints )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_Calibration_SetFrequencyPlan" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SelectCalibrationFromFile ( int Handle , char *pFileName )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SelectCalibrationFromFile	pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SelectCalibrationFromFile;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pFileName )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SelectCalibrationFromFile" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Calibration_Measure ( int Handle , int iCalibrationType , int iCalibrationStandard )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_Calibration_Measure		pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Calibration_Measure;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iCalibrationType , iCalibrationStandard )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_Calibration_Measure" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Calibration_Apply ( int Handle , int iCalibrationType )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_Calibration_Apply			pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Calibration_Apply;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iCalibrationType )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_Calibration_Apply" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Kit_SelectFromFile ( int Handle , char *szFileName , int port )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_Kit_SelectFromFile		pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Kit_SelectFromFile;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , szFileName , port )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_Kit_SelectFromFile" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_MeasureSingleSweep ( int Handle , int iMeasurementType )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_MeasureSingleSweep		pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_MeasureSingleSweep;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iMeasurementType )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_MeasureSingleSweep" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetAverage ( int Handle , int iAverage )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SetAverage				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetAverage;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iAverage )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SetAverage" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetSmoothing ( int Handle , int iSmoothing )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SetSmoothing				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetSmoothing;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iSmoothing )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SetSmoothing" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetDwellTime ( int Handle , double lfDwellTime )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SetDwellTime				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetDwellTime;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfDwellTime )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SetDwellTime" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetPowerLevel ( int Handle , int iPowerLevel )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SetPowerLevel				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetPowerLevel;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iPowerLevel )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SetPowerLevel" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetReferencePlane ( int Handle , int iMeasurementType , int iValue , int bAuto )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SetReferencePlane			pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetReferencePlane;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iMeasurementType , iValue , bAuto )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SetReferencePlane" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SaveMeasurementToMemory ( int Handle , int iMeasurementType )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SaveMeasurementToMemory	pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SaveMeasurementToMemory;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iMeasurementType )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SaveMeasurementToMemory" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_ApplyMemoryMath ( int Handle , int iMeasurementType , int iFunction )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_ApplyMemoryMath			pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_ApplyMemoryMath;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iMeasurementType , iFunction )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_ApplyMemoryMath" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetSystemImpedance ( int Handle , int iSystemImpedance , int bExternalMatch )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SetSystemImpedance		pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetSystemImpedance;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iSystemImpedance , bExternalMatch )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SetSystemImpedance" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_GetData ( int Handle , int iMeasurementType , int iMeasurementFormat , int iFrequencyIndex , double *plfFrequency , double *plfValue )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_GetData					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetData;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iMeasurementType , iMeasurementFormat , iFrequencyIndex , plfFrequency , plfValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_GetData" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_GetMemory ( int Handle , int iMeasurementType , int iMeasurementFormat , int iFrequencyIndex , double *plfFrequency , double *plfValue )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_GetMemory					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetMemory;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iMeasurementType , iMeasurementFormat , iFrequencyIndex , plfFrequency , plfValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_GetMemory" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_FindDataPoint ( int Handle , double lfFrequency , int iMeasurementType , int iMeasurementFormat , int iMeasurementFunction , double *plfFrequency , double *plfValue )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_FindDataPoint				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_FindDataPoint;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfFrequency , iMeasurementType , iMeasurementFormat , iMeasurementFunction , plfFrequency , plfValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_FindDataPoint" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}


STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_GetTrace ( int Handle , int iChannel , int iMeasurementType , int iMeasurementFormat , double lfTimeout ,  double **plfFrequency , double **plfTrace , int *piNumberOfPoints )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_GetTrace					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetTrace;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( (tDriverInfo.InstrumentHandle) , iChannel , iMeasurementType , iMeasurementFormat , lfTimeout ,  plfFrequency , plfTrace , piNumberOfPoints )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_GetTrace" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_GetInfo ( int Handle , int iInfoType , char *pInfo )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_GetInfo					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetInfo;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iInfoType , pInfo )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_GetInfo" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SaveStateAndCalibration ( int Handle , char *pFileName )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SaveStateAndCalibration	pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SaveStateAndCalibration;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pFileName )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SaveStateAndCalibration" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_LoadStateAndCalibration ( int Handle , char *pFileName )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_LoadStateAndCalibration	pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_LoadStateAndCalibration;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pFileName )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_LoadStateAndCalibration" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

//=================================================== NEW API =======================================================//

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetFrequency(int Handle,double lfStartFrequency, double lfStopFrequency)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SetFrequency				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetFrequency;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfStartFrequency , lfStopFrequency )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SetFrequency" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_GetFrequency(int Handle,double *plfStartFrequency, double *plfStopFrequency)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_GetFrequency				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetFrequency;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , plfStartFrequency , plfStopFrequency )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_GetFrequency" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetPointsNum(int Handle,int points)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SetPointsNum				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetPointsNum;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , points )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SetPointsNum" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_GetPointsNum(int Handle,int *points)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_GetPointsNum				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetPointsNum;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , points )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_GetPointsNum" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_GetMeasureVectors(int Handle,int iGetDataType, double **plfFirstVector,double **plfSecondVector, double **plfThirdVector,double **plfFourthVector)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_GetMeasureVectors			pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetMeasureVectors;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iGetDataType, plfFirstVector, plfSecondVector,  plfThirdVector, plfFourthVector )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_GetMeasureVectors" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetMeasureType(int Handle,int iChannel, int iInSparam, int iConversation)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SetMeasureType			pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetMeasureType;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, iInSparam, iConversation )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SetMeasureType" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_DisplaySelect(int Handle,int iChannel, int iDataMode, int iDataFormat)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_DisplaySelect				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_DisplaySelect;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, iDataMode, iDataFormat )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_DisplaySelect" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_AutoScale(int Handle)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_AutoScale					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_AutoScale;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_AutoScale" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_AvarageSmooth(int Handle,int iChannel, char *szSmoothRequest, double lfSetSmooth,double lfCurrentSmooth)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_AvarageSmooth				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_AvarageSmooth;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, szSmoothRequest, lfSetSmooth, lfCurrentSmooth )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_AvarageSmooth" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Smooth(int Handle,int iChannel, int iState, double lfSetSmooth,int iNumberPoints)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_Smooth					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Smooth;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, iState, lfSetSmooth, iNumberPoints )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_Smooth" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetBandWidth(int Handle,int iChannel, double lfBW_Value, char *szBW_Request,double *lfGetBW_Value)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SetBandWidth				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetBandWidth;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfBW_Value, szBW_Request, lfGetBW_Value )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SetBandWidth" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetFrequencySweepType(int Handle,int iChannel, int iFrequencySweepType)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SetFrequencySweepType		pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetFrequencySweepType;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, iFrequencySweepType )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SetFrequencySweepType" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetPower(int Handle,int iChannel, double lfPower, char *szPowerRequest,int iPortNum)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SetPower					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetPower;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfPower, szPowerRequest, iPortNum )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SetPower" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_GetPower(int Handle,int iChannel, double *lfPower, int iPortNum)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_GetPower					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetPower;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfPower, iPortNum )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_GetPower" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetPowerRange(int Handle,int iChannel, int iRange, char *szRangeRequest,int *iCurrentRange)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SetPowerRange				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetPowerRange;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, iRange, szRangeRequest, iCurrentRange )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SetPowerRange" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetPowerSweep(int Handle,int iChannel, double lfStartPower, double lfStopPower,char *szSweepRequest, double *lfGetStartPower,double *lfGetStopPower)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SetPowerSweep				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetPowerSweep;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfStartPower, lfStopPower,szSweepRequest, lfGetStartPower,lfGetStopPower )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SetPowerSweep" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetSweepTime(int Handle,int iChannel, double lfSweepTime, char *szSweepTimeRequest,double *lfGetSweepTime)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SetSweepTime				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetSweepTime;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfSweepTime, szSweepTimeRequest, lfGetSweepTime )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SetSweepTime" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetMarker(int Handle,int iChannel, double lfSetValue, int iMarker,char *szMarkerRequest)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SetMarker					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetMarker;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfSetValue, iMarker, szMarkerRequest )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SetMarker" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_GetMarker(int Handle,int iChannel, int iMarker, char *szMarkerRequest,double *lfGetMarkerOneValue, double *lfGetMarkerTwoValue,double *lfGetMarkerThreeValue, double *lfGetMarkerFourValue)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_GetMarker					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetMarker;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, iMarker, szMarkerRequest,lfGetMarkerOneValue, lfGetMarkerTwoValue,lfGetMarkerThreeValue, lfGetMarkerFourValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_GetMarker" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_ReCallRegister(int Handle,char *szRegister_FileName)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_ReCallRegister			pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_ReCallRegister;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , szRegister_FileName )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_ReCallRegister" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SaveToRegister(int Handle,char *szTitle, int iRegister)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SaveToRegister			pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SaveToRegister;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , szTitle, iRegister )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SaveToRegister" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}
		
STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_QuaryCalibration(int Handle,int iChannel, char *pResponse)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_QuaryCalibration			pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_QuaryCalibration;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, pResponse )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_QuaryCalibration" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}


STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SelectChannel ( int Handle , int iChannel )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SelectChannel				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SelectChannel;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SelectChannel" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SelectTrace ( int Handle , iTraceNumber )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SelectTrace				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SelectTrace;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iTraceNumber )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SelectTrace" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SelectWindow ( int Handle , iWindowNumber )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SelectWindow				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SelectWindow;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iWindowNumber )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SelectWindow" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_GetLastStateFileName(int Handle, char **pszFileName )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_GetLastStateFileName		pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_GetLastStateFileName;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pszFileName )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_GetLastStateFileName" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SelectMathFunction ( int Handle , int iChannel , char cFunction )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SelectMathFunction		pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SelectMathFunction;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , cFunction )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SelectMathFunction" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetTraceToMathMemory ( int Handle , int iChannel )  
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SetTraceToMathMemory		pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetTraceToMathMemory;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SetTraceToMathMemory" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_ECAL_GetInformation ( int Handle , char **pszCatalog )  
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_ECAL_GetInformation		pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_ECAL_GetInformation;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pszCatalog )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_ECAL_GetInformation" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}	

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_ECAL_Calibrate ( int Handle , char *pszPortList , char *pUserCal , double lfTimeout )  
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_ECAL_Calibrate			pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_ECAL_Calibrate;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pszPortList , pUserCal , lfTimeout )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_ECAL_Calibrate" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetElectricalDelayTime( int Handle , int iChannel , double lfTime )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SetElectricalDelayTime	pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetElectricalDelayTime;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , lfTime )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SetElectricalDelayTime" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Avarage( int Handle , int iChannel, int bEnable , int iAverageValue )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_Avarage					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Avarage;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , bEnable , iAverageValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_Avarage" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_ClearAvarage( int Handle , int iChannel )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_ClearAvarage				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_ClearAvarage;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_ClearAvarage" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Reponse_OpenShortLoad_Calibrate(int Handle , int iType_OSL , char *pszPortList , double lfTimeout )
{	
	STD_ERROR                                  			StdError                                =   {0};
	
	tsDriverInfo										*pDriverInfo							=	NULL,
														tDriverInfo								=	{0};
	
	CmtTSVHandle 										VariableHandle							=	0;
	
	CmtThreadLockHandle 								LockHandle								=	0;
										
	pfNetworkAnalyzer_Reponse_OpenShortLoad_Calibrate	pWrapperFunction						=	NULL;
	
	int													bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Reponse_OpenShortLoad_Calibrate;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iType_OSL , pszPortList , lfTimeout )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_Reponse_OpenShortLoad_Calibrate" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Reponse_Thru_Calibrate(int Handle, char *pszPortList , double lfTimeout )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_Reponse_Thru_Calibrate	pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Reponse_Thru_Calibrate;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pszPortList , lfTimeout )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_Reponse_Thru_Calibrate" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_Reponse_Done_Calibrate(int Handle )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_Reponse_Done_Calibrate	pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_Reponse_Done_Calibrate;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_Reponse_Done_Calibrate" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}


STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_WaitSweep( int Handle , int iChannel , int iCount )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_WaitSweep					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_WaitSweep;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , iCount )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_WaitSweep" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}


STD_ERROR   DLLEXPORT	DRV_NetworkAnalyzer_SetCalibrationName( int Handle , char *pszName )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfNetworkAnalyzer_SetCalibrationName		pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.networkAnalyzerDriverFunctions.NetworkAnalyzer_SetCalibrationName;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pszName )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "NetworkAnalyzer_SetCalibrationName" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	return StdError;
}

/*
extern CmtTSVHandle			gGlobalDriverCommonHandle;

int main ()
{
	STD_ERROR									StdError									=	{0};

	double							*plfCurrentTraces								=	NULL,
									*plfCurrentFrequencies							=	NULL;
	
	int								iActualyPointsNumber							=	0,
									hNetworkAnalyzer								=	0; 
										
	CmtNewTSV ( sizeof(tsGlobalHandle) , &gGlobalDriverCommonHandle );

	CHK_STDERR( DRV_NetworkAnalyzer_Init( "Agilent_NetworkAnalyzer_Driver.dll" , "GPIB::17" , &(hNetworkAnalyzer)) );
	
	CHK_STDERR( DRV_NetworkAnalyzer_GetTrace ( hNetworkAnalyzer , 1 , 0 , 0 , 60.0 , &plfCurrentFrequencies , &plfCurrentTraces , &iActualyPointsNumber ));

Error:
	
	return 0;
}
*/
