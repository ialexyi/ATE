//==============================================================================
// Include files

#include "MAIN_DriversManager.h"
#include "SG_DriversManager.h"
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

int DLLEXPORT	DRV_SignalGenerator_GetErrorTextMessage ( int Handle , int iError , char **pErrorMessage )  
{		
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
	
	pfSignalGenerator_GetErrorTextMessage		pWrapperFunction						=	NULL;
	
	STD_ERROR                   				StdError								=	{0};

	int											bLocked									=	0;
	
	if ((pErrorMessage == NULL) || (Handle == 0))
		return DRV_ERROR_PASSED_NULL;
	
	CALLOC_ERR( *pErrorMessage , 256 , sizeof(char)); 
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		return DRV_ERROR_GET_TSV_POINTER;
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
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
			
			pWrapperFunction = tDriverInfo.tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_GetErrorTextMessage;

			CHK_PROCCESS_GET_LOCK ( LockHandle );
			
		    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
			{
				FREE_STDERR_COPY( pWrapperFunction ( tDriverInfo.InstrumentHandle , iError , (*pErrorMessage) ));
			}
			
			if ( pWrapperFunction == NULL )
			{
				char	szMessage[LOW_STRING]	= {0};
				
				sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SignalGenerator_GetErrorTextMessage" , tDriverInfo.pDriverFileName );
				
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

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_Init ( char *pszDriverLocation , char *pszAddressString , int *pHandle )
{		   					
	STD_ERROR									StdError									=	{0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
	
	pfSignalGenerator_Init						pWrapperFunction						=	NULL;
	
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
		sprintf( szMessage , "Loading Signal Generator Driver\n\"%s\"\n\nFailed, File has not found." , szDriverLocation );
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
	
			pDriverInfo->InstrumentType = DRIVER_TYPE_SIGNAL_GENERATOR;
	
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
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.Config_Install_CommentCallback								= (pfSTD_Config_Install_CommentCallback) GetProcAddress( pDriverInfo->LibraryHandle , 		"Config_Install_CommentCallback");      
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.Config_Install_ConfigValueCallback							= (pfSTD_Config_Install_ConfigValueCallback) GetProcAddress( pDriverInfo->LibraryHandle , 	"Config_Install_ConfigValueCallback");  
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.Config_Install_CheckForBreakCallback						= (pfSTD_Config_Install_CheckForBreakCallback) GetProcAddress( pDriverInfo->LibraryHandle , "Config_Install_CheckForBreakCallback");
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.Config_LOG_SetAllowState									= (pfSTD_Config_LOG_SetAllowState) GetProcAddress( pDriverInfo->LibraryHandle , "Config_LOG_SetAllowState");
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.Config_Copy_STD_CallBackSet								= (pfSTD_Config_Copy_STD_CallBackSet) GetProcAddress( pDriverInfo->LibraryHandle , "Config_Copy_STD_CallBackSet");
			
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_GetErrorTextMessage     					= (pfSignalGenerator_GetErrorTextMessage) GetProcAddress( pDriverInfo->LibraryHandle , "SignalGenerator_GetErrorTextMessage");         
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_Init                        				= (pfSignalGenerator_Init) GetProcAddress( pDriverInfo->LibraryHandle , "SignalGenerator_Init");                        
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_Close                       				= (pfSignalGenerator_Close) GetProcAddress( pDriverInfo->LibraryHandle , "SignalGenerator_Close");                       
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_Reset										= (pfSignalGenerator_Reset) GetProcAddress( pDriverInfo->LibraryHandle , "SignalGenerator_Reset"); 
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_RF_Off              						= (pfSignalGenerator_RF_Off) GetProcAddress( pDriverInfo->LibraryHandle , "SignalGenerator_RF_Off");                    
		    pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_RF_On           							= (pfSignalGenerator_RF_On) GetProcAddress( pDriverInfo->LibraryHandle , "SignalGenerator_RF_On");                 
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_Config            							= (pfSignalGenerator_Config) GetProcAddress( pDriverInfo->LibraryHandle , "SignalGenerator_Config");            
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_SetFrequency     							= (pfSignalGenerator_SetFrequency) GetProcAddress( pDriverInfo->LibraryHandle , "SignalGenerator_SetFrequency");     
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_SetPower      								= (pfSignalGenerator_SetPower) GetProcAddress( pDriverInfo->LibraryHandle , "SignalGenerator_SetPower");      
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_SetModulation          				  	= (pfSignalGenerator_SetModulation) GetProcAddress( pDriverInfo->LibraryHandle , "SignalGenerator_SetModulation");
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_SetModulationType          				= (pfSignalGenerator_SetModulationType) GetProcAddress( pDriverInfo->LibraryHandle , "SignalGenerator_SetModulationType");            
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_SetRfOutPort          	  					= (pfSignalGenerator_SetRfOutPort) GetProcAddress( pDriverInfo->LibraryHandle , "SignalGenerator_SetRfOutPort");            
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_SetFunctionGen          				  	= (pfSignalGenerator_SetFunctionGen) GetProcAddress( pDriverInfo->LibraryHandle , "SignalGenerator_SetFunctionGen");            
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_SetInOut          	  						= (pfSignalGenerator_SetInOut) GetProcAddress( pDriverInfo->LibraryHandle , "SignalGenerator_SetInOut");            
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_DisableModulation							= (pfSignalGenerator_DisableModulation) GetProcAddress( pDriverInfo->LibraryHandle , "SignalGenerator_DisableModulation");            
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_DisableFunctionGen							= (pfSignalGenerator_DisableFunctionGen) GetProcAddress( pDriverInfo->LibraryHandle , "SignalGenerator_DisableFunctionGen");            
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_SetOffSetAttenuation						= (pfSignalGenerator_SetOffSetAttenuation) GetProcAddress( pDriverInfo->LibraryHandle , "SignalGenerator_SetOffSetAttenuation");            
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_GetLastStateFileName						= (pfSignalGenerator_GetLastStateFileName) GetProcAddress( pDriverInfo->LibraryHandle , "SignalGenerator_GetLastStateFileName");             
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_RecallState								= (pfSignalGenerator_RecallState) GetProcAddress( pDriverInfo->LibraryHandle , "SignalGenerator_RecallState");             
			pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_Modulation_Off        						= (pfSignalGenerator_Modulation_Off) GetProcAddress( pDriverInfo->LibraryHandle , "SignalGenerator_Modulation_Off");                    
		    pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_Modulation_On      						= (pfSignalGenerator_Modulation_On) GetProcAddress( pDriverInfo->LibraryHandle , "SignalGenerator_Modulation_On");                 

			pDriverInfo->tInstrDB.Equipment_Info  																			= (pfSTD_Equipment_Info) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_Info");      
			pDriverInfo->tInstrDB.Equipment_IsSupported																		= (pfSTD_Equipment_IsSupported) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_IsSupported");   
		
			pDriverInfo->tInstrDB.Equipment_SendStateFile																	= (pfSTD_Equipment_SendStateFile) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_SendStateFile");
			pDriverInfo->tInstrDB.Equipment_ReceiveStateFile																= (pfSTD_Equipment_ReceiveStateFile) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_ReceiveStateFile");
			pDriverInfo->tInstrDB.Equipment_GetFileCatalog																	= (pfSTD_Equipment_GetFileCatalog) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_GetFileCatalog"); 			
			pDriverInfo->tInstrDB.Equipment_MakeFileCatalog																	= (pfSTD_Equipment_MakeFileCatalog) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_MakeFileCatalog"); 			
			pDriverInfo->tInstrDB.Equipment_DeleteFileCatalog																= (pfSTD_Equipment_DeleteFileCatalog) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_DeleteFileCatalog");
			pDriverInfo->tInstrDB.Equipment_DeleteStateFile																	= (pfSTD_Equipment_DeleteStateFile) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_DeleteStateFile");
			
			pDriverInfo->tInstrDB.Config_SetAttribute																		= (pfSTD_Config_SetAttribute) GetProcAddress( pDriverInfo->LibraryHandle , "Config_SetAttribute"); 		
			
			pWrapperFunction = pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_Init;
	
			if ( pWrapperFunction == NULL )
			{
				char	szMessage[LOW_STRING]	= {0};
				
				sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SignalGenerator_Init" , pDriverInfo->pDriverFileName );
				
				ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
			
				{STD_ERR (DRV_ERROR_WRONG_WRAPPER);}
			}
		
			if ( !bHandleExists )
				DRIVER_MANAGER_AddConnection( pszAddressString , &(pDriverInfo->InstrumentHandle) , DRIVER_TYPE_SIGNAL_GENERATOR , &(pDriverInfo->InstrumentLockHandle) );
		
			LockHandle = pDriverInfo->InstrumentLockHandle;
		
			if ( LockHandle == 0 )
				{STD_ERR (DRV_ERROR_CREATE_LOCK_HANDLE);}
		
			CHK_PROCCESS_GET_LOCK ( LockHandle );
		
			DRIVER_MANAGER_IsConnectionExists( pszAddressString, &(pDriverInfo->InstrumentHandle) , NULL );
	
			//===================== Install Calbacks ==========================================================================================//
			pConfig_Install_CommentCallback = pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.Config_Install_CommentCallback;		
			pConfig_Install_ConfigValueCallback	= pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.Config_Install_ConfigValueCallback;		
			pConfig_Install_CheckForBreakCallback = pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.Config_Install_CheckForBreakCallback;		
			pConfig_Copy_STD_CallBackSet = pDriverInfo->tInstrDB.signalGeneratorDriverFunctions.Config_Copy_STD_CallBackSet;
		
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
				else
					break;
			
				if ( StdError.error )
					if ( ShowMessage ( INSTR_TYPE_YES_NO , "Signal Genrator Error !!!", "Check connection and Power On the Device." , NULL ) )
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
		DRV_SignalGenerator_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
		DRV_SignalGenerator_Close( &VariableHandle ); 
		
		if ( pHandle )
			*pHandle = 0;
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** ***** Close ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_Close ( int *pHandle )
{							
	STD_ERROR									StdError									=	{0};
								
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSignalGenerator_Close						pWrapperFunction						=	NULL;
	
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
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_Close;
	
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

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SignalGenerator_Close" , tDriverInfo.pDriverFileName );

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
	
	if ( DRIVER_MANAGER_IsConnectionExistsByType( DRIVER_TYPE_SIGNAL_GENERATOR ) == 0 )
		FreeLibrary( tDriverInfo.LibraryHandle );
	
	*pHandle = 0;
	
	if ( StdError.error )
	{FREE_CALLOC_COPY_STRING (StdError.pszErrorDescription, "Error while closing instrument driver.");}
	
	return StdError;
}


/***** ***** ***** ***** ***** ***** ***** Get Lock ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT DRV_SignalGenerator_GetLock ( int Handle )
{
	STD_ERROR									StdError									=	{0};
	
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
	
	if ( InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
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

STD_ERROR   DLLEXPORT DRV_SignalGenerator_TryToGetLock ( int Handle , int *pSuccess)
{
	STD_ERROR									StdError									=	{0};
	
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
	
	if ( InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
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

STD_ERROR   DLLEXPORT DRV_SignalGenerator_ReleaseLock ( int Handle )
{
	STD_ERROR									StdError									=	{0};
	
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
	
	if ( InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
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

STD_ERROR   DLLEXPORT DRV_SignalGenerator_LockStatus ( int Handle , int *iStatus )
{
	STD_ERROR									StdError									=	{0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;
	
	IF ((iStatus == NULL) , "Error passed NULL pointer.");
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( pDriverInfo->InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	*iStatus = pDriverInfo->LockCounter;		
	
Error:	
	
	if (Handle)
		CmtReleaseTSVPtr ( Handle );
	
	return StdError;		
}

/***** ***** ***** ***** ***** ***** ***** Reset ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_Reset ( int Handle )
{							
	STD_ERROR									StdError									=	{0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSignalGenerator_Reset						pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_Reset;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SignalGenerator_Reset" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
	if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SignalGenerator_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** RfOff ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_RF_Off ( int Handle , int Channel )
{							
	STD_ERROR									StdError									=	{0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSignalGenerator_RF_Off					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_RF_Off;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , Channel )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SignalGenerator_RF_Off" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SignalGenerator_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** RfOn ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_RF_On ( int Handle , int Channel )
{							
	STD_ERROR									StdError									=	{0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSignalGenerator_RF_On						pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_RF_On;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , Channel )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SignalGenerator_RF_On" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SignalGenerator_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_Modulation_Off ( int Handle , int Channel )
{							
	STD_ERROR									StdError									=	{0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSignalGenerator_Modulation_Off			pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_Modulation_Off;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , Channel )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SignalGenerator_Modulation_Off" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SignalGenerator_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_Modulation_On ( int Handle , int Channel )
{							
	STD_ERROR									StdError								=	{0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSignalGenerator_Modulation_On				pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_Modulation_On;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , Channel )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SignalGenerator_Modulation_On" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SignalGenerator_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Config ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_Config ( int Handle , int Channel , double dFrequency , double dAmplitude )
{							
	STD_ERROR									StdError									=	{0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSignalGenerator_Config					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_Config;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , Channel , dFrequency , dAmplitude )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SignalGenerator_Config" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SignalGenerator_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Set Frequency ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_SetFrequency ( int Handle , int Channel , double dFrequency )
{							
	STD_ERROR									StdError									=	{0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSignalGenerator_SetFrequency				pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	pDriverInfo->lfFrequency = dFrequency;
	
	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_SetFrequency;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , Channel , dFrequency )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SignalGenerator_SetFrequency" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SignalGenerator_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Set Power ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_SetPower ( int Handle , int Channel , double dPower )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSignalGenerator_SetPower					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	pDriverInfo->lfPower = dPower;
	
	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_SetPower;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , Channel , dPower )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SignalGenerator_SetPower" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SignalGenerator_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Set Power ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_ApplyPowerCorrection_ByLoss ( int Handle , int Channel , double lfOutputPower , double lfAdditionalLoss )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSignalGenerator_SetPower					pWrapperFunction						=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	double										lfFrequency								=	0.0,
												lfLossFactor							=	0.0,
												lfVariablePower							=	0.0;
	
	int											iCorrectionIndex						=	0;
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	lfFrequency = pDriverInfo->lfFrequency;
	
	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_SetPower;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
	for ( iCorrectionIndex = 0; iCorrectionIndex < 5; iCorrectionIndex++ )
	{
		DRIVER_MANAGER_CALIBRATION_Gel_3D_Factor( tDriverInfo.InstrumentHandle , lfVariablePower , lfFrequency , &lfLossFactor );  
	
		lfVariablePower = lfOutputPower - lfAdditionalLoss - lfLossFactor;
	}
	
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , Channel , lfVariablePower )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SignalGenerator_SetPower" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SignalGenerator_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Set Modulation ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_SetModulation ( int Handle , int Channel , int iSource , double lfModulationIndex , double lfFrequency )
{							
	STD_ERROR									StdError									=	{0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSignalGenerator_SetModulation				pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_SetModulation;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , Channel , iSource , lfModulationIndex , lfFrequency )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SignalGenerator_SetModulation" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SignalGenerator_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Set Modulation Type ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_SetModulationType ( int Handle , int Channel , int iModulationType )
{							
	STD_ERROR									StdError									=	{0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSignalGenerator_SetModulationType			pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_SetModulationType;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , Channel ,iModulationType)); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SignalGenerator_SetModulationType" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SignalGenerator_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Set Rf Out Port ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_SetRfOutPort ( int Handle , int Channel , int iPort )
{							
	STD_ERROR									StdError									=	{0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSignalGenerator_SetRfOutPort				pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_SetRfOutPort;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , Channel ,iPort)); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SignalGenerator_SetRfOutPort" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SignalGenerator_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Set Function Gen ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_SetFunctionGen ( int Handle , int Channel , int iFunctionNum , int bState , double lfRmsLevel , int bUnitVolt , int bWaveFormSine , double lfFrequency )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSignalGenerator_SetFunctionGen			pWrapperFunction						=	NULL;
	
	

	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_SetFunctionGen;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , Channel , iFunctionNum , bState , lfRmsLevel , bUnitVolt , bWaveFormSine , lfFrequency)); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SignalGenerator_SetFunctionGen" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SignalGenerator_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Set In Out ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_SetInOut ( int Handle , int Channel , int iInput , int bInImpedance , int iOutput )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSignalGenerator_SetInOut					pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_SetInOut;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , Channel , iInput , bInImpedance , iOutput)); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SignalGenerator_SetInOut" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SignalGenerator_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


/***** ***** ***** ***** ***** ***** ***** Disable Modulation ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_DisableModulation ( int Handle , int Channel , int iSource  )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSignalGenerator_DisableModulation			pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_DisableModulation;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , Channel , iSource )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SignalGenerator_DisableModulation" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SignalGenerator_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Disable Modulation ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_DisableFunctionGen ( int Handle , int Channel , int iFunctionNum  )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSignalGenerator_DisableFunctionGen		pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_DisableFunctionGen;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , Channel , iFunctionNum )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SignalGenerator_DisableFunctionGen" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SignalGenerator_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


/***** ***** ***** ***** ***** ***** ***** Set OffSet Attenuation ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_SignalGenerator_SetOffSetAttenuation ( int Handle , int Channel , double lfAttenuation  )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSignalGenerator_SetOffSetAttenuation		pWrapperFunction						=	NULL;
	
	
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_SetOffSetAttenuation;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , Channel , lfAttenuation )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SignalGenerator_SetOffSetAttenuation" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_SignalGenerator_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


STD_ERROR   DLLEXPORT	DRV_SignalGenerator_GetLastStateFileName( int Handle ,char **pszFileName )
{				
	STD_ERROR                                   StdError                                =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSignalGenerator_GetLastStateFileName		pWrapperFunction						=	NULL;
	
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
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_GetLastStateFileName;
	
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

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SignalGenerator_GetLastStateFileName" , tDriverInfo.pDriverFileName );

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


STD_ERROR   DLLEXPORT	DRV_SignalGenerator_RecallState( int Handle , char *szFileName )
{				
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSignalGenerator_RecallState				pWrapperFunction						=	NULL;
	
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
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_RecallState;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
		
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , szFileName )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[STD_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "SignalGenerator_RecallState" , tDriverInfo.pDriverFileName );

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
