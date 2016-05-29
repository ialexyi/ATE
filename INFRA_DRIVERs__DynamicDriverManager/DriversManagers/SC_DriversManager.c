//==============================================================================
// Include files

#include "MAIN_DriversManager.h"
#include "SC_DriversManager.h"
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

int DLLEXPORT	DRV_Oscilloscope_GetErrorTextMessage ( int Handle , int iError , char **pErrorMessage )  
{		
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
	
	pfOscilloscope_GetErrorTextMessage			pWrapperFunction						=	NULL;
	
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
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
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
			
			pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_GetErrorTextMessage;

			CHK_PROCCESS_GET_LOCK ( LockHandle );
				
			if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
			{
				FREE_STDERR_COPY( pWrapperFunction ( tDriverInfo.InstrumentHandle , iError , (*pErrorMessage) ));
			}
			
			if ( pWrapperFunction == NULL )
			{
				char	szMessage[LOW_STRING]	= {0};
				
				sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_GetErrorTextMessage" , tDriverInfo.pDriverFileName );
				
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

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_Init ( char *pszDriverLocation , char *pszAddressString , int *pHandle )
{		  			
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
															
	pfOscilloscope_Init							pWrapperFunction						=	NULL;
	
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
		sprintf( szMessage , "Loading Oscilloscope Driver\n\"%s\"\n\nFailed, File has not found." , szDriverLocation );
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
	
			pDriverInfo->InstrumentType = DRIVER_TYPE_OSCILLOSCOPE;
	
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
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Config_Install_CommentCallback						= (pfSTD_Config_Install_CommentCallback) GetProcAddress( pDriverInfo->LibraryHandle , 		"Config_Install_CommentCallback");      
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Config_Install_ConfigValueCallback					= (pfSTD_Config_Install_ConfigValueCallback) GetProcAddress( pDriverInfo->LibraryHandle , 	"Config_Install_ConfigValueCallback");  
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Config_Install_CheckForBreakCallback					= (pfSTD_Config_Install_CheckForBreakCallback) GetProcAddress( pDriverInfo->LibraryHandle , "Config_Install_CheckForBreakCallback");
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Config_LOG_SetAllowState								= (pfSTD_Config_LOG_SetAllowState) GetProcAddress( pDriverInfo->LibraryHandle , "Config_LOG_SetAllowState");
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Config_Copy_STD_CallBackSet							= (pfSTD_Config_Copy_STD_CallBackSet) GetProcAddress( pDriverInfo->LibraryHandle , "Config_Copy_STD_CallBackSet");
			
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_GetErrorTextMessage     					= (pfOscilloscope_GetErrorTextMessage) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_GetErrorTextMessage");         
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_Init                      			  	= (pfOscilloscope_Init) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_Init");                        
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_Close                     			  	= (pfOscilloscope_Close) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_Close");                       
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_Reset									= (pfOscilloscope_Reset) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_Reset"); 
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_AutoSetup              					= (pfOscilloscope_AutoSetup) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_AutoSetup");                    
		    pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_ChannelOnOff           					= (pfOscilloscope_ChannelOnOff) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_ChannelOnOff");                 
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_ChannelVertical           			 	= (pfOscilloscope_ChannelVertical) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_ChannelVertical");            
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_Horizontal     							= (pfOscilloscope_Horizontal) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_Horizontal");     
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_SweepContinuous      					= (pfOscilloscope_SweepContinuous) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_SweepContinuous");      
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_InitTrigger          	  				= (pfOscilloscope_InitTrigger) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_InitTrigger");
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_WaitForEvent          	 			 	= (pfOscilloscope_WaitForEvent) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_WaitForEvent");            
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_TriggerConfig          	 			 	= (pfOscilloscope_TriggerConfig) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_TriggerConfig");            
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_GetWaveForm          	  				= (pfOscilloscope_GetWaveForm) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_GetWaveForm");            
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureRiseTime             				= (pfOscilloscope_MeasureRiseTime) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MeasureRiseTime");          
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureFallTime             				= (pfOscilloscope_MeasureFallTime) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MeasureFallTime");          
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureFrequency            				= (pfOscilloscope_MeasureFrequency) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MeasureFrequency");         
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasurePeriod            				= (pfOscilloscope_MeasurePeriod) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MeasurePeriod");            
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureVoltageRMS          			 	= (pfOscilloscope_MeasureVoltageRMS) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MeasureVoltageRMS");        
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureVoltagePk2Pk       				= (pfOscilloscope_MeasureVoltagePk2Pk) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MeasureVoltagePk2Pk");      
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureVoltageMax         				= (pfOscilloscope_MeasureVoltageMax) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MeasureVoltageMax");        
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureVoltageMin         			 	= (pfOscilloscope_MeasureVoltageMin) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MeasureVoltageMin");        
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureVoltageHigh        			 	= (pfOscilloscope_MeasureVoltageHigh) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MeasureVoltageHigh");       
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureVoltageLow          			 	= (pfOscilloscope_MeasureVoltageLow) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MeasureVoltageLow");        
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureVoltageAverage      			 	= (pfOscilloscope_MeasureVoltageAverage) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MeasureVoltageAverage");    
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureWidthNegative       			 	= (pfOscilloscope_MeasureWidthNegative) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MeasureWidthNegative");     
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureWidthPositive       			 	= (pfOscilloscope_MeasureWidthPositive) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MeasureWidthPositive");
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureDutyCycleNegative    				= (pfOscilloscope_MeasureDutyCycleNegative) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MeasureDutyCycleNegative");     
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureDutyCyclePositive   				= (pfOscilloscope_MeasureDutyCyclePositive) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MeasureDutyCyclePositive");     
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureAmplitude        					= (pfOscilloscope_MeasureAmplitude) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MeasureAmplitude");     
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureVoltageCycleRMS      				= (pfOscilloscope_MeasureVoltageCycleRMS) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MeasureVoltageCycleRMS");      
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureVoltageCycleAverage  				= (pfOscilloscope_MeasureVoltageCycleAverage) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MeasureVoltageCycleAverage");  
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasurePositiveOvershoot     			= (pfOscilloscope_MeasurePositiveOvershoot) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MeasurePositiveOvershoot"); 
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureNegativeOvershoot    				= (pfOscilloscope_MeasureNegativeOvershoot) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MeasureNegativeOvershoot"); 
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureTimingBurstWidth	 				= (pfOscilloscope_MeasureTimingBurstWidth) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MeasureTimingBurstWidth");  
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MemoryState_Save							= (pfOscilloscope_MemoryState_Save) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MemoryState_Save");  
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MemoryState_Recall						= (pfOscilloscope_MemoryState_Recall) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MemoryState_Recall");  
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_HorizontalOffset							= (pfOscilloscope_HorizontalOffset) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_HorizontalOffset");  
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureDelay								= (pfOscilloscope_MeasureDelay) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_MeasureDelay");
			pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_ConfigMeasureDelay						= (pfOscilloscope_ConfigMeasureDelay) GetProcAddress( pDriverInfo->LibraryHandle , "Oscilloscope_ConfigMeasureDelay");	

			pDriverInfo->tInstrDB.Equipment_Info      																= (pfSTD_Equipment_Info) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_Info");      
			pDriverInfo->tInstrDB.Equipment_IsSupported																= (pfSTD_Equipment_IsSupported) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_IsSupported");   
		
			pDriverInfo->tInstrDB.Equipment_SendStateFile															= (pfSTD_Equipment_SendStateFile) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_SendStateFile");
			pDriverInfo->tInstrDB.Equipment_ReceiveStateFile														= (pfSTD_Equipment_ReceiveStateFile) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_ReceiveStateFile");
			pDriverInfo->tInstrDB.Equipment_GetFileCatalog															= (pfSTD_Equipment_GetFileCatalog) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_GetFileCatalog"); 			
			pDriverInfo->tInstrDB.Equipment_MakeFileCatalog															= (pfSTD_Equipment_MakeFileCatalog) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_MakeFileCatalog"); 			
			pDriverInfo->tInstrDB.Equipment_DeleteFileCatalog														= (pfSTD_Equipment_DeleteFileCatalog) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_DeleteFileCatalog");
			pDriverInfo->tInstrDB.Equipment_DeleteStateFile															= (pfSTD_Equipment_DeleteStateFile) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_DeleteStateFile");
			
			pDriverInfo->tInstrDB.Config_SetAttribute																= (pfSTD_Config_SetAttribute) GetProcAddress( pDriverInfo->LibraryHandle , "Config_SetAttribute"); 		
			
			pWrapperFunction = pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_Init;
		
			if ( pWrapperFunction == NULL )
			{
				char	szMessage[LOW_STRING]	= {0};
				
				sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_Init" , pDriverInfo->pDriverFileName );
				
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
			pConfig_Install_CommentCallback = pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Config_Install_CommentCallback;		
			pConfig_Install_ConfigValueCallback	= pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Config_Install_ConfigValueCallback;		
			pConfig_Install_CheckForBreakCallback = pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Config_Install_CheckForBreakCallback;		
			pConfig_Copy_STD_CallBackSet = pDriverInfo->tInstrDB.oscilloscopeDriverFunctions.Config_Copy_STD_CallBackSet;
		
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
					if ( ShowMessage ( INSTR_TYPE_YES_NO , "Scope Error !!!", "Check connection and Power On the Device." , NULL ) )
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
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
		DRV_Oscilloscope_Close( &VariableHandle ); 
		
		if ( pHandle )
			*pHandle = 0;
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** ***** Close ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_Close ( int *pHandle )
{					
	STD_ERROR                                   StdError                                    =   {0};
								
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_Close						pWrapperFunction						=	NULL;
	
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
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_Close;
		
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

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_Close" , tDriverInfo.pDriverFileName );

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

STD_ERROR   DLLEXPORT DRV_Oscilloscope_GetLock ( int Handle )
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
	
	if ( InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
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

STD_ERROR   DLLEXPORT DRV_Oscilloscope_TryToGetLock ( int Handle , int *pSuccess)
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
	
	if ( InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
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

STD_ERROR   DLLEXPORT DRV_Oscilloscope_ReleaseLock ( int Handle )
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
	
	if ( InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
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

STD_ERROR   DLLEXPORT DRV_Oscilloscope_LockStatus ( int Handle , int *iStatus )
{
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;
	
	IF ((iStatus == NULL) , "Error passed NULL pointer.");
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( pDriverInfo->InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	*iStatus = pDriverInfo->LockCounter;		
	
Error:	
	
	if (Handle)
		CmtReleaseTSVPtr ( Handle );
	
	return StdError;		
}

/***** ***** ***** ***** ***** ***** ***** Reset ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_Reset ( int Handle )
{						
	STD_ERROR                                   StdError                                    =   {0};
							
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_Reset						pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_Reset;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_Reset" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Auto Setup ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_AutoSetup ( int Handle )
{						
	STD_ERROR                                   StdError                                    =   {0};
							
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_AutoSetup					pWrapperFunction						=	NULL;

	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_AutoSetup;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_AutoSetup" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Channel On/Off ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_ChannelOnOff ( int Handle , int iChannel , int OnOff)
{						
	STD_ERROR                                   StdError                                    =   {0};
							
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_ChannelOnOff					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_ChannelOnOff;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , OnOff )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_ChannelOnOff" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Channel Vertical ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_ChannelVertical ( int Handle , int iChannel, double dScale , double dOffset , int iUnits , int iCouping , double lfProbeAttenuation , double lfImpedance )
{							
	STD_ERROR                                   StdError                                    =   {0};
						
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_ChannelVertical				pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_ChannelVertical;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, dScale , dOffset , iUnits , iCouping , lfProbeAttenuation , lfImpedance )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_ChannelVertical" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Horizontal ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_Horizontal ( int Handle , double dScale , double dOffset )
{						
	STD_ERROR                                   StdError                                    =   {0};
							
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_Horizontal					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_Horizontal;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , dScale , dOffset )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_Horizontal" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Sweep Continuous ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_SweepContinuous ( int Handle , int iState )
{							
	STD_ERROR                                   StdError                                    =   {0};
							
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_SweepContinuous				pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_SweepContinuous;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iState)); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_SweepContinuous" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Init Trigger ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_InitTrigger ( int Handle )
{							
	STD_ERROR                                   StdError                                    =   {0};
						
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_InitTrigger					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_InitTrigger;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_InitTrigger" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Wait For Event ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_WaitForEvent ( int Handle , double lfTimeOut )
{						
	STD_ERROR                                   StdError                                    =   {0};
							
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_WaitForEvent					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_WaitForEvent;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfTimeOut )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_WaitForEvent" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Trigger Config ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_TriggerConfig ( int Handle , int iChannel , double lfTrigerLevel , int bPositiveEdge , double lfHoldTime , int iModeAuto )
{						
	STD_ERROR                                   StdError                                    =   {0};
							
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_TriggerConfig				pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_TriggerConfig;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , lfTrigerLevel , bPositiveEdge , lfHoldTime , iModeAuto )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_TriggerConfig" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Get Wave Form ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_GetWaveForm ( int Handle , int iChannel, int iSize, double lfTimeOut, double *dpMeasureArray, int *ipCount, double *dpX_init, double *dpX_inc )
{						
	STD_ERROR                                   StdError                                    =   {0};
							
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_GetWaveForm					pWrapperFunction						=	NULL;

	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_GetWaveForm;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, iSize, lfTimeOut, dpMeasureArray, ipCount, dpX_init, dpX_inc )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_GetWaveForm" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Measure Rise Time ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureRiseTime ( int Handle , int iChannel, double lfTimeOut, double *dpValue )
{						
	STD_ERROR                                   StdError                                    =   {0};
							
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MeasureRiseTime				pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureRiseTime;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfTimeOut, dpValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MeasureRiseTime" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Measure Fall Time ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureFallTime ( int Handle , int iChannel, double lfTimeOut, double *dpValue )
{						
	STD_ERROR                                   StdError                                    =   {0};
							
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MeasureFallTime				pWrapperFunction						=	NULL;

	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureFallTime;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfTimeOut, dpValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MeasureFallTime" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Measure Frequency ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureFrequency ( int Handle , int iChannel, double lfTimeOut, double *dpValue )
{						
	STD_ERROR                                   StdError                                    =   {0};
							
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MeasureFrequency				pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureFrequency;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfTimeOut, dpValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MeasureFrequency" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Measure Period ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasurePeriod ( int Handle , int iChannel, double lfTimeOut, double *dpValue )
{						
	STD_ERROR                                   StdError                                    =   {0};
							
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MeasurePeriod				pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasurePeriod;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfTimeOut, dpValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MeasurePeriod" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Measure Voltage RMS ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureVoltageRMS ( int Handle , int iChannel, double lfTimeOut, double *dpValue )
{						
	STD_ERROR                                   StdError                                    =   {0};
							
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MeasureVoltageRMS			pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureVoltageRMS;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfTimeOut, dpValue )); 
			
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MeasureVoltageRMS" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Measure Voltage Pk2Pk ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureVoltagePk2Pk ( int Handle , int iChannel, double lfTimeOut, double *dpValue )
{							
	STD_ERROR                                   StdError                                    =   {0};
						
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MeasureVoltagePk2Pk			pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureVoltagePk2Pk;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfTimeOut, dpValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MeasureVoltagePk2Pk" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Measure Voltage Max ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureVoltageMax ( int Handle , int iChannel, double lfTimeOut, double *dpValue )
{						
	STD_ERROR                                   StdError                                    =   {0};
							
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MeasureVoltageMax			pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureVoltageMax;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfTimeOut, dpValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MeasureVoltageMax" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Measure Voltage Min ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureVoltageMin ( int Handle , int iChannel, double lfTimeOut, double *dpValue )
{							
	STD_ERROR                                   StdError                                    =   {0};
						
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MeasureVoltageMin			pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
												  
	char										*pTempString							=	NULL;
												   
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureVoltageMin;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfTimeOut, dpValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MeasureVoltageMin" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Measure Voltage High ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureVoltageHigh ( int Handle , int iChannel, double lfTimeOut, double *dpValue )
{						
	STD_ERROR                                   StdError                                    =   {0};
							
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MeasureVoltageHigh			pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureVoltageHigh;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfTimeOut, dpValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MeasureVoltageHigh" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Measure Voltage Low ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureVoltageLow ( int Handle , int iChannel, double lfTimeOut, double *dpValue )
{						
	STD_ERROR                                   StdError                                    =   {0};
							
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MeasureVoltageLow			pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureVoltageLow;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfTimeOut, dpValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MeasureVoltageLow" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}
												   
/***** ***** ***** ***** ***** ***** ***** Measure Voltage Average ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureVoltageAverage ( int Handle , int iChannel, double lfTimeOut, double *dpValue )
{							
	STD_ERROR                                   StdError                                    =   {0};
								
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MeasureVoltageAverage		pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureVoltageAverage;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfTimeOut, dpValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MeasureVoltageAverage" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


/***** ***** ***** ***** ***** ***** ***** Measure Width Negative ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureWidthNegative ( int Handle , int iChannel, double lfTimeOut, double *dpValue )
{							
	STD_ERROR                                   StdError                                    =   {0};
								
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MeasureWidthNegative			pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureWidthNegative;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfTimeOut, dpValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MeasureWidthNegative" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Measure Width Positive ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureWidthPositive ( int Handle , int iChannel, double lfTimeOut, double *dpValue )
{							
	STD_ERROR                                   StdError                                    =   {0};
								
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MeasureWidthPositive			pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureWidthPositive;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfTimeOut, dpValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MeasureWidthPositive" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Measure Duty Cycle Negative ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureDutyCycleNegative ( int Handle , int iChannel, double lfTimeOut, double *dpValue )
{							
	STD_ERROR                                   StdError                                    =   {0};
								
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MeasureDutyCycleNegative		pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureDutyCycleNegative;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfTimeOut, dpValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MeasureDutyCycleNegative" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Measure Duty Cycle Positive ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureDutyCyclePositive ( int Handle , int iChannel, double lfTimeOut, double *dpValue )
{							
	STD_ERROR                                   StdError                                    =   {0};
								
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MeasureDutyCyclePositive		pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureDutyCyclePositive;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfTimeOut, dpValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MeasureDutyCyclePositive" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Measure Amplitude ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureAmplitude ( int Handle , int iChannel, double lfTimeOut, double *dpValue )
{						
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MeasureAmplitude				pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureAmplitude;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfTimeOut, dpValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MeasureAmplitude" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Measure Voltage Cycle RMS ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureVoltageCycleRMS ( int Handle , int iChannel, double lfTimeOut, double *dpValue )
{								
	STD_ERROR                                   StdError                                    =   {0};
							
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MeasureVoltageCycleRMS		pWrapperFunction						=	NULL;

	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureVoltageCycleRMS;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfTimeOut, dpValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MeasureVoltageCycleRMS" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Measure Voltage Cycle Average ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureVoltageCycleAverage ( int Handle , int iChannel, double lfTimeOut, double *dpValue )
{							
	STD_ERROR                                   StdError                                    =   {0};
								
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MeasureVoltageCycleAverage	pWrapperFunction						=	NULL;

	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureVoltageCycleAverage;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfTimeOut, dpValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MeasureVoltageCycleAverage" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Measure Positive Overshoot ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasurePositiveOvershoot ( int Handle , int iChannel, double lfTimeOut, double *dpValue )
{								
	STD_ERROR                                   StdError                                    =   {0};
							
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MeasurePositiveOvershoot		pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasurePositiveOvershoot;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfTimeOut, dpValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MeasurePositiveOvershoot" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Measure Negative Overshoot ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureNegativeOvershoot ( int Handle , int iChannel, double lfTimeOut, double *dpValue )
{						
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MeasureNegativeOvershoot		pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureNegativeOvershoot;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfTimeOut, dpValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MeasureNegativeOvershoot" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Measure Timing Burst Width ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureTimingBurstWidth ( int Handle , int iChannel, double lfTimeOut, double *dpValue )
{							
	STD_ERROR                                   StdError                                    =   {0};
								
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MeasureTimingBurstWidth		pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureTimingBurstWidth;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel, lfTimeOut, dpValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MeasureTimingBurstWidth" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Oscilloscope Memory State Save ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MemoryState_Save( int Handle , char *pszFileName )
{							
	STD_ERROR                                   StdError                                    =   {0};
								
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MemoryState_Save				pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MemoryState_Save;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pszFileName)); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MemoryState_Save" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Oscilloscope Memory State Recall ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MemoryState_Recall( int Handle , char *pszFileName )
{							
	STD_ERROR                                   StdError                                    =   {0};
								
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MemoryState_Recall			pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MemoryState_Recall;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pszFileName )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MemoryState_Recall" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}	


/***** ***** ***** ***** ***** ***** ***** Oscilloscope Horizontal Offset ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_HorizontalOffset( int Handle , double lfOffset )
{							
	STD_ERROR                                   StdError                                    =   {0};
								
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_HorizontalOffset				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_HorizontalOffset;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfOffset )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_HorizontalOffset" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_MeasureDelay ( int Handle , int iChannel , double lfTimeOut, double *dpValue )  
{							
	STD_ERROR                                   StdError                                    =   {0};
								
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_MeasureDelay					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_MeasureDelay;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iChannel , lfTimeOut , dpValue )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_MeasureDelay" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_Oscilloscope_ConfigMeasureDelay ( int Handle , int iSourceChannel , int iSourceSlope , int iSourceSpare, int iMeasureChannel , int iMeasureSlope , int iMeasureSpare )
{							
	STD_ERROR                                   StdError                                    =   {0};
								
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfOscilloscope_ConfigMeasureDelay			pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_OSCILLOSCOPE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.oscilloscopeDriverFunctions.Oscilloscope_ConfigMeasureDelay;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , iSourceChannel , iSourceSlope , iSourceSpare, iMeasureChannel , iMeasureSlope , iMeasureSpare )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Oscilloscope_ConfigMeasureDelay" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_Oscilloscope_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}
