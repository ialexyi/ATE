//==============================================================================
// Include files

#include "MAIN_DriversManager.h"
#include "PM_DriversManager.h"
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

int DLLEXPORT	DRV_PowerMeter_GetErrorTextMessage ( int Handle , int iError , char **pErrorMessage )  
{   	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
	
	pfPowerMeter_GetErrorTextMessage			pWrapperFunction						=	NULL;
	
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
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
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
			
			pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_GetErrorTextMessage;

			CHK_PROCCESS_GET_LOCK ( LockHandle );
		
			if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
			{
				FREE_STDERR_COPY( pWrapperFunction ( tDriverInfo.InstrumentHandle , iError , (*pErrorMessage) ));
			}
			
			if ( pWrapperFunction == NULL )
			{
				char	szMessage[LOW_STRING]	= {0};
				
				sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_GetErrorTextMessage" , tDriverInfo.pDriverFileName );
				
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

STD_ERROR   DLLEXPORT	DRV_PowerMeter_Init ( char *pszDriverLocation , char *pszAddressString , int *pHandle )
{
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
	
	pfPowerMeter_Init							pWrapperFunction						=	NULL;
	
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
		sprintf( szMessage , "Loading Power Meter Driver\n\"%s\"\n\nFailed, File has not found." , szDriverLocation );
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
	
			pDriverInfo->InstrumentType = DRIVER_TYPE_POWER_METER;
	
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
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.Config_Install_CommentCallback						= (pfSTD_Config_Install_CommentCallback) GetProcAddress( pDriverInfo->LibraryHandle , 		"Config_Install_CommentCallback");      
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.Config_Install_ConfigValueCallback					= (pfSTD_Config_Install_ConfigValueCallback) GetProcAddress( pDriverInfo->LibraryHandle , 	"Config_Install_ConfigValueCallback");  
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.Config_Install_CheckForBreakCallback				= (pfSTD_Config_Install_CheckForBreakCallback) GetProcAddress( pDriverInfo->LibraryHandle , "Config_Install_CheckForBreakCallback");
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.Config_LOG_SetAllowState							= (pfSTD_Config_LOG_SetAllowState) GetProcAddress( pDriverInfo->LibraryHandle , "Config_LOG_SetAllowState");
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.Config_Copy_STD_CallBackSet							= (pfSTD_Config_Copy_STD_CallBackSet) GetProcAddress( pDriverInfo->LibraryHandle , "Config_Copy_STD_CallBackSet");
			
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_GetErrorTextMessage     					= (pfPowerMeter_GetErrorTextMessage) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_GetErrorTextMessage");         
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_Init                       			 	= (pfPowerMeter_Init) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_Init");                        
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_Close                      			 	= (pfPowerMeter_Close) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_Close");                       
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_Reset									= (pfPowerMeter_Reset) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_Reset"); 
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_SetSingleSweep             			 	= (pfPowerMeter_SetSingleSweep) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_SetSingleSweep");                    
		    pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_SetContinuesSweep          			 	= (pfPowerMeter_SetContinuesSweep) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_SetContinuesSweep");                 
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_InitSweep            					= (pfPowerMeter_InitSweep) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_InitSweep");            
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_SetAuto     								= (pfPowerMeter_SetAuto) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_SetAuto");     
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_SetChanelState      						= (pfPowerMeter_SetChanelState) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_SetChanelState");      
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_SetVerticalScale         			   	= (pfPowerMeter_SetVerticalScale) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_SetVerticalScale");            
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_SetVerticalCenter     					= (pfPowerMeter_SetVerticalCenter) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_SetVerticalCenter");     
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_Calibrate      							= (pfPowerMeter_Calibrate) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_Calibrate");      
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_Zero    									= (pfPowerMeter_Zero) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_Zero");    
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_SetUnits    								= (pfPowerMeter_SetUnits) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_SetUnits");    
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_SetOffset   								= (pfPowerMeter_SetOffset) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_SetOffset");   
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_SetCalFactor 							= (pfPowerMeter_SetCalFactor) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_SetCalFactor");   
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_SetVideoBW              					= (pfPowerMeter_SetVideoBW) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_SetVideoBW");              
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_SetPeakHold             				 	= (pfPowerMeter_SetPeakHold) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_SetPeakHold");              
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_SetFrequency   							= (pfPowerMeter_SetFrequency) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_SetFrequency");   
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_SetMode   								= (pfPowerMeter_SetMode) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_SetMode");   
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_Channel_SetMode							= (pfPowerMeter_Channel_SetMode) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_Channel_SetMode");   
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_OPC          							= (pfPowerMeter_OPC) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_OPC");          
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_FetchCWpower          					= (pfPowerMeter_FetchCWpower) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_FetchCWpower");          
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_MeasureModulatedPower					= (pfPowerMeter_MeasureModulatedPower) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_MeasureModulatedPower");
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_FetchMarkerAverage     			 		= (pfPowerMeter_FetchMarkerAverage) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_FetchMarkerAverage");      
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_FetchMarkerMax       					= (pfPowerMeter_FetchMarkerMax) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_FetchMarkerMax");       
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_FetchMarkerMin          					= (pfPowerMeter_FetchMarkerMin) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_FetchMarkerMin");
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_MeasurePowerPeakHold    					= (pfPowerMeter_MeasurePowerPeakHold) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_MeasurePowerPeakHold");       
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_MeasurePower          					= (pfPowerMeter_MeasurePower) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_MeasurePower");       
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_Configuration           					= (pfPowerMeter_Configuration) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_Configuration");       
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_RecallState								= (pfPowerMeter_RecallState) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_RecallState");       
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_ReadPower								= (pfPowerMeter_ReadPower) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_ReadPower");       	
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_FetchPower								= (pfPowerMeter_FetchPower) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_FetchPower");       	
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_SaveState								= (pfPowerMeter_SaveState) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_SaveState"); 
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_Gating_ConfigOffsetTime					= (pfPowerMeter_Gating_ConfigOffsetTime) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_Gating_ConfigOffsetTime");
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_Gating_ReadMaxPower						= (pfPowerMeter_Gating_ReadMaxPower) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_Gating_ReadMaxPower");     
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_Gating_ReadPower							= (pfPowerMeter_Gating_ReadPower) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_Gating_ReadPower");     

			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_Gating_FetchMaxPower						= (pfPowerMeter_Gating_FetchMaxPower) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_Gating_FetchMaxPower"); 
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_Gating_FetchAveragePower					= (pfPowerMeter_Gating_FetchAveragePower) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_Gating_FetchAveragePower"); 
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_Gating_FetchMinPower						= (pfPowerMeter_Gating_FetchMinPower) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_Gating_FetchMinPower"); 
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_SetStateName								= (pfPowerMeter_SetStateName) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_SetStateName");            
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_GetStateName								= (pfPowerMeter_GetStateName) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_GetStateName");            
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_SetStateFile								= (pfPowerMeter_SetStateFile) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_SetStateFile");           
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_GetStateFile								= (pfPowerMeter_GetStateFile) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_GetStateFile");  
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_RecallState_ByName						= (pfPowerMeter_RecallState_ByName) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_RecallState_ByName"); 
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_SetActiveChannel							= (pfPowerMeter_SetActiveChannel) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_SetActiveChannel"); 
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_Configure_Marker							= (pfPowerMeter_Configure_Marker) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_Configure_Marker");
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_Read_Marker								= (pfPowerMeter_Read_Marker) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_Read_Marker");
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_Fetch_Marker								= (pfPowerMeter_Fetch_Marker) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_Fetch_Marker");
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_SetActivePort							= (pfPowerMeter_SetActivePort) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_SetActivePort");
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_EnableDualPortMode						= (pfPowerMeter_EnableDualPortMode) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_EnableDualPortMode");
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_ReadCWpower								= (pfPowerMeter_ReadCWpower) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_ReadCWpower");             
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_GetLastStateFileName						= (pfPowerMeter_GetLastStateFileName) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_GetLastStateFileName");             
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_Gating_ReadPower							= (pfPowerMeter_Gating_ReadPower) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_Gating_ReadPower");  
			pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_GetTrace									= (pfPowerMeter_GetTrace) GetProcAddress( pDriverInfo->LibraryHandle , "PowerMeter_GetTrace");
			
			pDriverInfo->tInstrDB.Equipment_Info     															= (pfSTD_Equipment_Info) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_Info");      
			pDriverInfo->tInstrDB.Equipment_IsSupported															= (pfSTD_Equipment_IsSupported) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_IsSupported");   
		
			pDriverInfo->tInstrDB.Equipment_SendStateFile														= (pfSTD_Equipment_SendStateFile) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_SendStateFile");
			pDriverInfo->tInstrDB.Equipment_ReceiveStateFile													= (pfSTD_Equipment_ReceiveStateFile) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_ReceiveStateFile");
			pDriverInfo->tInstrDB.Equipment_GetFileCatalog														= (pfSTD_Equipment_GetFileCatalog) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_GetFileCatalog"); 			
			pDriverInfo->tInstrDB.Equipment_MakeFileCatalog														= (pfSTD_Equipment_MakeFileCatalog) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_MakeFileCatalog"); 			
			pDriverInfo->tInstrDB.Equipment_DeleteFileCatalog													= (pfSTD_Equipment_DeleteFileCatalog) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_DeleteFileCatalog");
			pDriverInfo->tInstrDB.Equipment_DeleteStateFile														= (pfSTD_Equipment_DeleteStateFile) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_DeleteStateFile");
			
			pDriverInfo->tInstrDB.Config_SetAttribute															= (pfSTD_Config_SetAttribute) GetProcAddress( pDriverInfo->LibraryHandle , "Config_SetAttribute");
			
			pWrapperFunction = pDriverInfo->tInstrDB.powerMeterDriverFunctions.PowerMeter_Init;
	
			if ( pWrapperFunction == NULL )
			{
				char	szMessage[LOW_STRING]	= {0};
				
				sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_Init" , pDriverInfo->pDriverFileName );
				
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
			pConfig_Install_CommentCallback = pDriverInfo->tInstrDB.powerMeterDriverFunctions.Config_Install_CommentCallback;		
			pConfig_Install_ConfigValueCallback	= pDriverInfo->tInstrDB.powerMeterDriverFunctions.Config_Install_ConfigValueCallback;		
			pConfig_Install_CheckForBreakCallback = pDriverInfo->tInstrDB.powerMeterDriverFunctions.Config_Install_CheckForBreakCallback;		
			pConfig_Copy_STD_CallBackSet = pDriverInfo->tInstrDB.powerMeterDriverFunctions.Config_Copy_STD_CallBackSet;
		
			DRIVER_MANAGER_GetCopyCallbacksStructure( VariableHandle , &(pDriverInfo->ptCallbacks) , 1 , pszAddressString ); 
			
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
					if ( ShowMessage ( INSTR_TYPE_YES_NO , "Power Meter Error !!!", "Check connection and Power On the Device." , NULL ) )
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
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
		DRV_PowerMeter_Close( &VariableHandle ); 
		
		if ( pHandle )
			*pHandle = 0;
	}
		
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** ***** Close ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_Close ( int *pHandle )
{				
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_Close							pWrapperFunction						=	NULL;
	
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
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_Close;
	
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
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_Close" , tDriverInfo.pDriverFileName );
		
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


/***** ***** ***** ***** ***** ***** ***** Get Lock ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT DRV_PowerMeter_GetLock ( int Handle )
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
	
	if ( InstrumentType != DRIVER_TYPE_POWER_METER )
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

STD_ERROR   DLLEXPORT DRV_PowerMeter_TryToGetLock ( int Handle , int *pSuccess)
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
	
	if ( InstrumentType != DRIVER_TYPE_POWER_METER )
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

STD_ERROR   DLLEXPORT DRV_PowerMeter_ReleaseLock ( int Handle )
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
	
	if ( InstrumentType != DRIVER_TYPE_POWER_METER )
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

STD_ERROR   DLLEXPORT DRV_PowerMeter_LockStatus ( int Handle , int *iStatus )
{
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;
	
	IF ((iStatus == NULL) , "Error passed NULL pointer.");
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( pDriverInfo->InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	*iStatus = pDriverInfo->LockCounter;		
	
Error:	
	
	if (Handle)
		CmtReleaseTSVPtr ( Handle );
	
	return StdError;		
}

/***** ***** ***** ***** ***** ***** ***** Reset ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_Reset ( int Handle )
{					
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_Reset							pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_Reset;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	    
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_Reset" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** ***** Set Single Sweep ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetSingleSweep ( int Handle , int channel ) 
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_SetSingleSweep					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_SetSingleSweep;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , channel )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_SetSingleSweep" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** ***** Set Continues Sweep ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetContinuesSweep ( int Handle , int channel ) 
{									
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_SetContinuesSweep				pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_SetContinuesSweep;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , channel )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_SetContinuesSweep" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Init Sweep ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_InitSweep ( int Handle , int channel ) 
{		
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_InitSweep						pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_InitSweep;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , channel )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_InitSweep" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** Set Auto ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetAuto ( int Handle , int channel ) 
{       
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_SetAuto						pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_SetAuto;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , channel )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_SetAuto" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** Set Chanel State ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetChanelState ( int Handle , int iChannel , int iState )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_SetChanelState					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_SetChanelState;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , iState )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_SetChanelState" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Set Vertical Scale ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetVerticalScale ( int Handle , int iChannel , double lfScale )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_SetVerticalScale				pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_SetVerticalScale;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , lfScale )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_SetVerticalScale" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** Set Vertical Center ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetVerticalCenter ( int Handle , int iChannel , double lfCenter )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_SetVerticalCenter				pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_SetVerticalCenter;
	
 	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , lfCenter )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_SetVerticalCenter" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** Calibrate ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_Calibrate ( int Handle , int iChannel )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_Calibrate						pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_Calibrate;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	    
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_Calibrate" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Zero ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_Zero ( int Handle , int iChannel )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_Zero							pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_Zero;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	    
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_Zero" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Set Units ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetUnits ( int Handle , int iChannel , unsigned int Units )
{		
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_SetUnits						pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_SetUnits;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	    
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , Units )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_SetUnits" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Set Offset ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetOffset ( int Handle , int iChannel , double lfOffset )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_SetOffset						pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_SetOffset;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	    
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , lfOffset )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_SetOffset" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Set Cal Factor ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetCalFactor ( int Handle , int iChannel , double lfCalFactor )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_SetCalFactor					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_SetCalFactor;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );

    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , lfCalFactor )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_SetCalFactor" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** ***** Set Video BW ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetVideoBW ( int Handle , int iChannel , unsigned int VideoBW )
{		
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_SetVideoBW						pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_SetVideoBW;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , VideoBW )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_SetVideoBW" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** ***** Set Peak Hold ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetPeakHold ( int Handle , int iChannel , int iState )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo					  			*pDriverInfo							=	NULL,
									  			tDriverInfo								=	{0};
	
	CmtTSVHandle 					  			VariableHandle							=	0;
	
	CmtThreadLockHandle 			  			LockHandle								=	0;
									  
	pfPowerMeter_SetPeakHold		  			pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_SetPeakHold;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , iState )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_SetPeakHold" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Set Frequency ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetFrequency ( int Handle , int iChannel , double lfFrequency )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_SetFrequency					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_SetFrequency;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , lfFrequency  )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_SetFrequency" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Set Mode ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetMode ( int Handle , int channel , unsigned int Mode )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_SetMode						pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_SetMode;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , channel , Mode )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_SetMode" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


/***** ***** ***** ***** ***** ***** ***** Set Mode ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_Channel_SetMode( int Handle , int iChannel , unsigned int Mode  )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_Channel_SetMode				pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_Channel_SetMode;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , Mode )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_Channel_SetMode" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

	
/***** ***** ***** ***** ***** ***** ***** OPC ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_OPC ( int Handle , double lfTimeout , int *piState )
{		
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_OPC							pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_OPC;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfTimeout , piState )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_OPC" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Fetch CW power ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_FetchCWpower ( int Handle , int iChannel , double lfTimeout , double *value )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_FetchCWpower					pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_FetchCWpower;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , lfTimeout , value )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_FetchCWpower" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** Measure Modulated Power ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_MeasureModulatedPower ( int Handle , int iChannel , double lfTimeout , double *value )
{			
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_MeasureModulatedPower			pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_MeasureModulatedPower;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , lfTimeout , value )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_MeasureModulatedPower" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** Fetch Marker Average ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_FetchMarkerAverage ( int Handle , int iChannel , int iMarker , double lfTimeout , double *value )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_FetchMarkerAverage				pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_FetchMarkerAverage;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , iMarker , lfTimeout , value )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_FetchMarkerAverage" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** Fetch Marker Max ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_FetchMarkerMax ( int Handle , int iChannel , int iMarker , double lfTimeout , double *value )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_FetchMarkerMax					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_FetchMarkerMax;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , iMarker , lfTimeout , value )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_FetchMarkerMax" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** Fetch Marker Min ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_FetchMarkerMin ( int Handle , int iChannel , int iMarker , double lfTimeout , double *value )
{		
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_FetchMarkerMin					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_FetchMarkerMin;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , iMarker , lfTimeout , value )); 

	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_FetchMarkerMin" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}

Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** Measured Power ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_MeasurePower ( int Handle , int iChannel , double lfTimeout , double *value )
{			
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_MeasurePower					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_MeasurePower;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , lfTimeout , value )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_MeasurePower" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** Measured Power Peak Hold ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_MeasurePowerPeakHold ( int Handle , int iChannel , double lfTimeout , double *value )
{			
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_MeasurePowerPeakHold			pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_MeasurePowerPeakHold;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , lfTimeout , value )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_MeasurePowerPeakHold" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}
/**************************************************************************************************/

/***** ***** ***** ***** ***** ***** Configuration ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_PowerMeter_Configuration( int Handle , int iChannel , int bContinue , int bDelayAutoState , int iTriggerSource , double lfTrigHoldOff , double lfGateOffsetTime , double lfGateTime , double lfTraceTime )
{						
	STD_ERROR                                   StdError                                    =   {0};
							
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_Configuration					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_Configuration;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , bContinue , bDelayAutoState , iTriggerSource , lfTrigHoldOff , lfGateOffsetTime , lfGateTime , lfTraceTime )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_Configuration" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerSupply_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}



STD_ERROR   DLLEXPORT	DRV_PowerMeter_RecallState( int Handle , int iStateNumber )
{						
	STD_ERROR                                   StdError                                    =   {0};
							
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_RecallState					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_RecallState;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iStateNumber )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_RecallState" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerSupply_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_PowerMeter_ReadPower ( int Handle , int iChannel , double lfTimeout , double *value )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_ReadPower						pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_ReadPower;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , lfTimeout , value )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_ReadPower" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_PowerMeter_FetchPower ( int Handle , int iChannel , double lfTimeout , double *value )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_FetchPower						pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_FetchPower;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , lfTimeout , value )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_FetchPower" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


STD_ERROR   DLLEXPORT	DRV_PowerMeter_SaveState ( int Handle , int iStateNumber)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_SaveState						pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_SaveState;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iStateNumber )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_SaveState" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}
	
STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetActiveChannel( int Handle , int iChannelNumber )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_SetActiveChannel				pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_SetActiveChannel;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannelNumber )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_SetActiveChannel" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}
	
	
STD_ERROR   DLLEXPORT	DRV_PowerMeter_Gating_ConfigOffsetTime ( int Handle , int iChannel , int iGate , double lfTime , double lfOffset , double lfMidambleOffset, double lfMidambleLength )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_Gating_ConfigOffsetTime		pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_Gating_ConfigOffsetTime;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , iGate , lfTime , lfOffset , lfMidambleOffset, lfMidambleLength )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_Gating_ConfigOffsetTime" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
} 


STD_ERROR   DLLEXPORT	DRV_PowerMeter_Gating_ReadMaxPower ( int Handle , int iChannel , int iGait , double timeout , double *value )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_Gating_ReadMaxPower			pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_Gating_ReadMaxPower;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , iGait , timeout , value )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_Gating_ReadMaxPower" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_PowerMeter_Gating_FetchMaxPower (int Handle , int iChannel , int iGate , double timeout , double *value)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_Gating_FetchMaxPower			pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_Gating_FetchMaxPower;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , iGate , timeout , value )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_Gating_FetchMaxPower" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
} 



STD_ERROR   DLLEXPORT	DRV_PowerMeter_Gating_ReadPower( int Handle , int iChannel , int iGait , int iGaitMeasureIndex , double timeout , double *value )
{	
	STD_ERROR                                   StdError                                =   {0};
																			   
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_Gating_ReadPower				pWrapperFunction						=	NULL;

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_Gating_ReadPower;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , iGait , iGaitMeasureIndex , timeout , value )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_Gating_ReadPower" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_PowerMeter_Gating_FetchAveragePower (int Handle , int iChannel , int iGate , double timeout , double *value)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_Gating_FetchMaxPower			pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_Gating_FetchAveragePower;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , iGate , timeout , value )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_Gating_FetchMaxPower" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
} 

STD_ERROR   DLLEXPORT	DRV_PowerMeter_Gating_FetchMinPower (int Handle , int iChannel , int iGate , double timeout , double *value)
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_Gating_FetchMaxPower			pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_Gating_FetchMinPower;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , iGate , timeout , value )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_Gating_FetchMaxPower" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
} 

STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetStateName ( int Handle , int iStateNumber , char *pszName )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_SetStateName					pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	FREE_CALLOC_COPY_STRING( pDriverInfo->pLastStateFileName , pszName );
	
	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_SetStateName;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iStateNumber , pszName )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_SetStateName" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_PowerMeter_GetStateName ( int Handle , int iStateNumber , char *pszName )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_GetStateName					pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_GetStateName;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iStateNumber , pszName )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_GetStateName" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetStateFile ( int Handle , int iStateNumber , char *pszFileName )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_SetStateFile					pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	FREE_CALLOC_COPY_STRING( pDriverInfo->pLastStateFileName , pszFileName );
	
	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_SetStateFile;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iStateNumber , pszFileName )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_SetStateFile" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_PowerMeter_GetStateFile ( int Handle , int iStateNumber , char *pszFileName )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_GetStateFile					pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_GetStateFile;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iStateNumber , pszFileName )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_GetStateFile" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


STD_ERROR   DLLEXPORT	DRV_PowerMeter_RecallState_ByName ( int Handle , char *pszName )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_RecallState_ByName				pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	FREE_CALLOC_COPY_STRING( pDriverInfo->pLastStateFileName , pszName );

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_RecallState_ByName;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pszName )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_RecallState_ByName" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


STD_ERROR   DLLEXPORT	DRV_PowerMeter_GetLastStateFileName( int Handle ,char **pszFileName )
{				
	STD_ERROR                                   StdError                                =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_GetLastStateFileName			pWrapperFunction						=	NULL;
	
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
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_GetLastStateFileName;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
	{
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pszFileName )); 
	}
	else
	{
		FREE_CALLOC_COPY_STRING( *pszFileName , pLastStateFileName );
	}
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_GetLastStateFileName" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	FREE(pLastStateFileName);
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_PowerMeter_Configure_Marker( int Handle , int iChannel , int iMarkerNumber , int bEnable , double lfPosition )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_Configure_Marker				pWrapperFunction						=	NULL;
								 
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_Configure_Marker;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , iMarkerNumber , bEnable , lfPosition )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_Configure_Marker" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_PowerMeter_Read_Marker( int Handle , int iChannel , int iMarkerNumber , double timeout , double *vlfPosition , double *vlfPower )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_Read_Marker					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_Read_Marker;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , iMarkerNumber , timeout , vlfPosition , vlfPower )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_Read_Marker" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


STD_ERROR   DLLEXPORT	DRV_PowerMeter_Fetch_Marker( int Handle , int iChannel , int iMarkerNumber , double *vlfPosition , double *vlfPower )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_Fetch_Marker					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_Fetch_Marker;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , iMarkerNumber , vlfPosition , vlfPower )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_Fetch_Marker" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_PowerMeter_SetActivePort( int Handle , int iSensorNumber )
{	
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_SetActivePort					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_SetActivePort;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iSensorNumber )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_SetActivePort" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_PowerMeter_EnableDualPortMode( int Handle , int bEnable )
{	
	STD_ERROR                                   StdError                                =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_EnableDualPortMode				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_EnableDualPortMode;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , bEnable )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_EnableDualPortMode" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_PowerMeter_ReadCWpower( int Handle , int iChannel , double timeout , double *value )
{	
	STD_ERROR                                   StdError                                =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_ReadCWpower					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_ReadCWpower;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , timeout , value )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_ReadCWpower" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_PowerMeter_GetTrace( int Handle , int iChannel , double timeout, double **plfTime , double **plfTrace , int numberOfPoints , int *pCount )
{	
	STD_ERROR                                   StdError                                =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfPowerMeter_GetTrace						pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_POWER_METER )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.powerMeterDriverFunctions.PowerMeter_GetTrace;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , timeout , plfTime , plfTrace , numberOfPoints , pCount )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "PowerMeter_GetTrace" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_PowerMeter_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}
