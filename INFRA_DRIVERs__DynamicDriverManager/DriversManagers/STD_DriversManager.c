//==============================================================================
// Include files

#include "MAIN_DriversManager.h"
#include "STD_DriversManager.h"
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

int   DLLEXPORT	DRV_StandardDevice_GetErrorTextMessage( int Handle , int iError , char **pErrorMessage )
{		
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
	
	pfStandardDevice_GetErrorTextMessage		pWrapperFunction						=	NULL;
	
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
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
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
			
			pWrapperFunction = tDriverInfo.tInstrDB.standardDeviceDriverFunctions.StandardDevice_GetErrorTextMessage;

			CHK_PROCCESS_GET_LOCK ( LockHandle );
			
		    if ( pWrapperFunction && pDriverInfo && ( pDriverInfo->bDemoMode == 0 ))
			{
				FREE_STDERR_COPY( pWrapperFunction ( tDriverInfo.InstrumentHandle , iError , (*pErrorMessage) ));
			}
			
			if ( pWrapperFunction == NULL )
			{
				char	szMessage[LOW_STRING]	= {0};
				
				sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "StandardDevice_GetErrorTextMessage" , tDriverInfo.pDriverFileName );
				
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

STD_ERROR   DLLEXPORT	DRV_StandardDevice_Init( char *pszDriverLocation , int iDeviceID , char *pszConnectionName , char *pszAddressString , int *pHandle , ... )
{		   					
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
	
	pfStandardDevice_Init						pWrapperFunction						=	NULL;
	
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
		sprintf( szMessage , "Loading Standard Device Driver\n\"%s\"\n\nFailed, File has not found." , szDriverLocation );
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
	
			pDriverInfo->InstrumentType = DRIVER_TYPE_STANDARD_DEVICE;
	
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
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.Config_Install_CommentCallback							= (pfSTD_Config_Install_CommentCallback) GetProcAddress( pDriverInfo->LibraryHandle , 		"Config_Install_CommentCallback");      
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.Config_Install_ConfigValueCallback						= (pfSTD_Config_Install_ConfigValueCallback) GetProcAddress( pDriverInfo->LibraryHandle , 	"Config_Install_ConfigValueCallback");  
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.Config_Install_CheckForBreakCallback					= (pfSTD_Config_Install_CheckForBreakCallback) GetProcAddress( pDriverInfo->LibraryHandle , "Config_Install_CheckForBreakCallback");
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.Config_LOG_SetAllowState								= (pfSTD_Config_LOG_SetAllowState) GetProcAddress( pDriverInfo->LibraryHandle , "Config_LOG_SetAllowState");
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.Config_Copy_STD_CallBackSet								= (pfSTD_Config_Copy_STD_CallBackSet) GetProcAddress( pDriverInfo->LibraryHandle , "Config_Copy_STD_CallBackSet");
			
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.StandardDevice_GetErrorTextMessage     					= (pfStandardDevice_GetErrorTextMessage) GetProcAddress( pDriverInfo->LibraryHandle , "StandardDevice_GetErrorTextMessage");         
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.StandardDevice_Init                        				= (pfStandardDevice_Init) GetProcAddress( pDriverInfo->LibraryHandle , "StandardDevice_Init");                        
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.StandardDevice_Close                       				= (pfStandardDevice_Close) GetProcAddress( pDriverInfo->LibraryHandle , "StandardDevice_Close");                       
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.StandardDevice_Reset									= (pfStandardDevice_Reset) GetProcAddress( pDriverInfo->LibraryHandle , "StandardDevice_Reset"); 
		
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.StandardDevice_GetConnectionAddress    					= (pfStandardDevice_GetConnectionAddress) GetProcAddress( pDriverInfo->LibraryHandle , "StandardDevice_GetConnectionAddress");                    
		    pDriverInfo->tInstrDB.standardDeviceDriverFunctions.StandardDevice_SetValue         	  					= (pfStandardDevice_SetValue) GetProcAddress( pDriverInfo->LibraryHandle , "StandardDevice_SetValue");                 
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.StandardDevice_GetValue     	       					= (pfStandardDevice_GetValue) GetProcAddress( pDriverInfo->LibraryHandle , "StandardDevice_GetValue");            
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.StandardDevice_GetValue_Ex								= (pfStandardDevice_GetValue_Ex) GetProcAddress( pDriverInfo->LibraryHandle , "StandardDevice_GetValue_Ex");            
			
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.StandardDevice_Unplug   	  							= (pfStandardDevice_Unplug) GetProcAddress( pDriverInfo->LibraryHandle , "StandardDevice_Unplug");     
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.StandardDevice_PlugIn	      							= (pfStandardDevice_PlugIn) GetProcAddress( pDriverInfo->LibraryHandle , "StandardDevice_PlugIn");      
	
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.StandardDevice_EnablePlugInAutomatically    			= (pfStandardDevice_EnablePlugInAutomatically) GetProcAddress( pDriverInfo->LibraryHandle , "StandardDevice_EnablePlugInAutomatically");                    
		    pDriverInfo->tInstrDB.standardDeviceDriverFunctions.StandardDevice_InstallReceiveCallbackFunction         	= (pfStandardDevice_InstallReceiveCallbackFunction) GetProcAddress( pDriverInfo->LibraryHandle , "StandardDevice_InstallReceiveCallbackFunction");                 
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.StandardDevice_LoadConfigFile     	       				= (pfStandardDevice_LoadConfigFile) GetProcAddress( pDriverInfo->LibraryHandle , "StandardDevice_LoadConfigFile");            
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.StandardDevice_LoadConfigBuffer   	  					= (pfStandardDevice_LoadConfigBuffer) GetProcAddress( pDriverInfo->LibraryHandle , "StandardDevice_LoadConfigBuffer");     
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.StandardDevice_Check_Connection   	  					= (pfStandardDevice_Check_Connection) GetProcAddress( pDriverInfo->LibraryHandle , "StandardDevice_Check_Connection");  
		
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.StandardDevice_Get_Commands_List						= (pfStandardDevice_Get_Commands_List) GetProcAddress( pDriverInfo->LibraryHandle , "StandardDevice_Get_Commands_List");  

			pDriverInfo->tInstrDB.Equipment_Info      																	= (pfSTD_Equipment_Info) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_Info");      
			pDriverInfo->tInstrDB.Equipment_IsSupported																	= (pfSTD_Equipment_IsSupported) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_IsSupported");   
		
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.StandardDevice_Get_IdentificationNumber					= (pfStandardDevice_Get_IdentificationNumber) GetProcAddress( pDriverInfo->LibraryHandle , "StandardDevice_Get_IdentificationNumber");  
			
			pDriverInfo->tInstrDB.Equipment_GetLowLevelHandle															= (pfSTD_Equipment_GetLowLevelHandle) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_GetLowLevelHandle");  
			
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.StandardDevice_GetDeviceInfoBlock						= (pfStandardDevice_GetDeviceInfoBlock) GetProcAddress( pDriverInfo->LibraryHandle , "StandardDevice_GetDeviceInfoBlock");
			
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.StandardDevice_Save_Log_File_As							= (pfStandardDevice_Save_Log_File_As) GetProcAddress( pDriverInfo->LibraryHandle , "StandardDevice_Save_Log_File_As");
			
			pDriverInfo->tInstrDB.standardDeviceDriverFunctions.StandardDevice_ResetDriver								= (pfStandardDevice_ResetDriver) GetProcAddress( pDriverInfo->LibraryHandle , "StandardDevice_ResetDriver");
			
			pDriverInfo->tInstrDB.Equipment_SendStateFile																= (pfSTD_Equipment_SendStateFile) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_SendStateFile");
			pDriverInfo->tInstrDB.Equipment_ReceiveStateFile															= (pfSTD_Equipment_ReceiveStateFile) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_ReceiveStateFile");
			pDriverInfo->tInstrDB.Equipment_GetFileCatalog																= (pfSTD_Equipment_GetFileCatalog) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_GetFileCatalog"); 			
			pDriverInfo->tInstrDB.Equipment_MakeFileCatalog																= (pfSTD_Equipment_MakeFileCatalog) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_MakeFileCatalog"); 			
			pDriverInfo->tInstrDB.Equipment_DeleteFileCatalog															= (pfSTD_Equipment_DeleteFileCatalog) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_DeleteFileCatalog");
			pDriverInfo->tInstrDB.Equipment_DeleteStateFile																= (pfSTD_Equipment_DeleteStateFile) GetProcAddress( pDriverInfo->LibraryHandle , "Equipment_DeleteStateFile");
			
			pDriverInfo->tInstrDB.Config_SetAttribute																	= (pfSTD_Config_SetAttribute) GetProcAddress( pDriverInfo->LibraryHandle , "Config_SetAttribute"); 		
			
			pWrapperFunction = pDriverInfo->tInstrDB.standardDeviceDriverFunctions.StandardDevice_Init;
	
			if ( pWrapperFunction == NULL )
			{
				char	szMessage[LOW_STRING]	= {0};
				
				sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "StandardDevice_Init" , pDriverInfo->pDriverFileName );
				
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
			pConfig_Install_CommentCallback = pDriverInfo->tInstrDB.standardDeviceDriverFunctions.Config_Install_CommentCallback;		
			pConfig_Install_ConfigValueCallback	= pDriverInfo->tInstrDB.standardDeviceDriverFunctions.Config_Install_ConfigValueCallback;		
			pConfig_Install_CheckForBreakCallback = pDriverInfo->tInstrDB.standardDeviceDriverFunctions.Config_Install_CheckForBreakCallback;		
			pConfig_Copy_STD_CallBackSet = pDriverInfo->tInstrDB.standardDeviceDriverFunctions.Config_Copy_STD_CallBackSet;
		
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
					FREE_STDERR_COPY( pWrapperFunction( iDeviceID , pszConnectionName , pszAddressString , &pDriverInfo->InstrumentHandle )); 
				
					if ( IS_OK )
					{
						pDriverInfo->bConnected = 1;
					
						if ( pDriverInfo->tInstrDB.Equipment_GetLowLevelHandle )
						{
							pDriverInfo->tInstrDB.Equipment_GetLowLevelHandle( pDriverInfo->InstrumentHandle , &(pDriverInfo->InstrumentLowLevelHandle));
						}
					}
				}
				else
					break;
			
				if ( StdError.error )
					if ( ShowMessage ( INSTR_TYPE_YES_NO , "Standard Device Error !!!", "Check connection and Power On the Device." , NULL ) )
					{
						if ( pszAddressString && ( strstr(pszAddressString,"TCPIP")))
							LaunchExecutableEx ("cmd /c arp -d", LE_HIDE, NULL); 
						
						continue;
					}
				
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
		DRV_StandardDevice_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
		DRV_StandardDevice_Close( &VariableHandle ); 
		
		if ( pHandle )
			*pHandle = 0;
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** ***** Close ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_StandardDevice_Close ( int *pHandle )
{							
	STD_ERROR                                   StdError                                    =   {0};
								
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfStandardDevice_Close						pWrapperFunction						=	NULL;
	
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
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.standardDeviceDriverFunctions.StandardDevice_Close;
	
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

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "StandardDevice_Close" , tDriverInfo.pDriverFileName );

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

STD_ERROR   DLLEXPORT DRV_StandardDevice_GetLock ( int Handle )
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
	
	if ( InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
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

STD_ERROR   DLLEXPORT DRV_StandardDevice_TryToGetLock ( int Handle , int *pSuccess)
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
	
	if ( InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
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

STD_ERROR   DLLEXPORT DRV_StandardDevice_ReleaseLock ( int Handle )
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
	
	if ( InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
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

STD_ERROR   DLLEXPORT DRV_StandardDevice_LockStatus ( int Handle , int *iStatus )
{
	STD_ERROR                                   StdError                                    =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;
	
	IF ((iStatus == NULL) , "Error passed NULL pointer.");
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( pDriverInfo->InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	*iStatus = pDriverInfo->LockCounter;		
	
Error:	
	
	if (Handle)
		CmtReleaseTSVPtr ( Handle );
	
	return StdError;		
}

/***** ***** ***** ***** ***** ***** ***** Save State ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_StandardDevice_Reset( int Handle )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfStandardDevice_Reset						pWrapperFunction						=	NULL;
								  
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.standardDeviceDriverFunctions.StandardDevice_Reset;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && pDriverInfo && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "StandardDevice_Reset" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_StandardDevice_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_StandardDevice_ResetDriver( int Handle )
{							
	STD_ERROR                                   StdError								=   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfStandardDevice_ResetDriver				pWrapperFunction						=	NULL;
								  
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.standardDeviceDriverFunctions.StandardDevice_ResetDriver;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && pDriverInfo && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "StandardDevice_ResetDriver" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_StandardDevice_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Save State ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_StandardDevice_GetConnectionAddress( int Handle , char **pszAddress )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfStandardDevice_GetConnectionAddress		pWrapperFunction						=	NULL;
								  
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.standardDeviceDriverFunctions.StandardDevice_GetConnectionAddress;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && pDriverInfo && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pszAddress )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "StandardDevice_GetConnectionAddress" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_StandardDevice_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Set Value ***** ***** ***** ***** ***** ***** ***** *****/


STD_ERROR   DLLEXPORT	DRV_StandardDevice_SetValue( int Handle , char *pCommandName , void *pValue , int iValueLength , int iValueSize , ...  )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfStandardDevice_SetValue					pWrapperFunction						=	NULL;
								  
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.standardDeviceDriverFunctions.StandardDevice_SetValue;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && pDriverInfo && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pCommandName , pValue , iValueLength , iValueSize )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "StandardDevice_SetValue" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_StandardDevice_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


/***** ***** ***** ***** ***** ***** ***** Set List of Values ***** ***** ***** ***** ***** ***** ***** *****/


STD_ERROR   DLLEXPORT	DRV_StandardDevice_SetListOfValues( int Handle , char *pCommandsList , int bCheckResponds , int iNumberOfRetries )
{							
	STD_ERROR                                   StdError                                =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfStandardDevice_SetValue					pSetValueWrapperFunction				=	NULL;
	pfStandardDevice_GetValue					pGetValueWrapperFunction				=	NULL;
	
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL,
												*pLocalString							=	NULL;
	
	char										*pCurrent								=	NULL,
												*pNext									=	NULL,
												*pTemp									=	NULL,
												*pValue									=	NULL;
												
	int											iValue									=	0;
	double										lfValue									=	0.0;
	
	int											iGetValue								=	0;
	double										lfGetValue								=	0.0;
	
	char										*pGetCommand							=	NULL;

	void										*pVoidValue								=	NULL;
	
	int											bRealNumeric							=	0;
	
	int											iGetValueLength							=	0,
												iGetValueSize							=	0;
	
	int											iTryIndex								=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pSetValueWrapperFunction = tDriverInfo.tInstrDB.standardDeviceDriverFunctions.StandardDevice_SetValue;
	pGetValueWrapperFunction = tDriverInfo.tInstrDB.standardDeviceDriverFunctions.StandardDevice_GetValue;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
	if ( iNumberOfRetries == 0 )
		iNumberOfRetries = 1;
	
    if ( pSetValueWrapperFunction && pDriverInfo && ( pDriverInfo->bDemoMode == 0 ))
	{
		CALLOC_COPY_STRING_ERR( pLocalString , pCommandsList );
		
		pNext = pLocalString;
		
		do
		{
			pCurrent = pNext;
			
			pTemp = strchr( pNext , ',' );
			
			if ( pTemp )
			{
				*pTemp = 0;	
				
				pNext = ++pTemp;
			}
			else
			{
				pNext = NULL;	
			}
			
			if ( pNext )
				RemoveSurroundingWhiteSpace( pNext );
			
			if ( pCurrent && ( strlen(pCurrent)))
			{
				pTemp = strchr( pCurrent , ' ' );
				
				if ( pTemp )
				{
					*pTemp = 0;
					pValue = pTemp+1;
				}
				else
				{
					pValue = NULL;
				}
				
				for ( iTryIndex = 0; iTryIndex < iNumberOfRetries; iTryIndex++ ) 
				{
					if ( pValue )
					{
						if ( strchr( pValue , '.' ) || strchr( pValue , 'e' ) || strchr( pValue , 'E' ))
						{
							lfValue = atof(pValue);
						
							FREE_STDERR_COPY_ERR( pSetValueWrapperFunction( tDriverInfo.InstrumentHandle , pCurrent , &lfValue , 1 , sizeof(lfValue) )); 
							
							bRealNumeric = 1;
						}
						else
						{
							iValue = atol(pValue);
						
							FREE_STDERR_COPY_ERR( pSetValueWrapperFunction( tDriverInfo.InstrumentHandle , pCurrent , &iValue , 1 , sizeof(iValue) )); 
							
							bRealNumeric = 0;
						}
					}
					else
					{
						FREE_STDERR_COPY_ERR( pSetValueWrapperFunction( tDriverInfo.InstrumentHandle , pCurrent , NULL , 0 , 0 )); 
					}
				
					if ( bCheckResponds && pGetValueWrapperFunction )
					{
						FREE_STDERR_COPY_ERR( pGetValueWrapperFunction( tDriverInfo.InstrumentHandle , &pGetCommand , &pVoidValue , &iGetValueLength , &iGetValueSize )); 	
						
						if ( pVoidValue && iGetValueLength && iGetValueSize )
						{
							if ( bRealNumeric && ( iGetValueSize == 8 ))
							{
								memcpy( &lfGetValue , pVoidValue , sizeof(double));
								
								if ( lfGetValue == lfValue )
									break;
							}
							
							if (( bRealNumeric == 0 ) && ( iGetValueSize <= 4 ))
							{
								memcpy( &iGetValue , pVoidValue , iGetValueSize);
								
								if ( iGetValue == iValue )
									break;
							}
						}
					}
					else
					{
						break;
					}
				}
			}
			
		} while( pNext );
	}
	
	if ( pSetValueWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "StandardDevice_SetValue" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
	if ( pGetValueWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "StandardDevice_GetValue" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_StandardDevice_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	FREE(pLocalString);
	FREE(pVoidValue);
	FREE(pGetCommand);
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Set Value ***** ***** ***** ***** ***** ***** ***** *****/


STD_ERROR   DLLEXPORT	DRV_StandardDevice_GetIdentificationNumber( int Handle , unsigned __int64 *pullIDNumber )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfStandardDevice_Get_IdentificationNumber	pWrapperFunction						=	NULL;
								  
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.standardDeviceDriverFunctions.StandardDevice_Get_IdentificationNumber;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && pDriverInfo && ( pDriverInfo->bDemoMode == 0 ) && pullIDNumber )
	{
		*pullIDNumber = pWrapperFunction(); 
	}
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "StandardDevice_SetValue" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_StandardDevice_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Get Value ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_StandardDevice_GetValue( int Handle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize , ...  )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfStandardDevice_GetValue					pWrapperFunction						=	NULL;
								  
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.standardDeviceDriverFunctions.StandardDevice_GetValue;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && pDriverInfo && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pGetCommandName , pGetValue , piValueLength , piValueSize )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "StandardDevice_GetValue" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_StandardDevice_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Save State ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_StandardDevice_GetValue_Ex( int Handle , char *pCommandName , void **pGetValue , int *piValueLength , int *piValueSize , ...  )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfStandardDevice_GetValue_Ex				pWrapperFunction						=	NULL;
								  
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.standardDeviceDriverFunctions.StandardDevice_GetValue_Ex;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && pDriverInfo && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pCommandName , pGetValue , piValueLength , piValueSize )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "StandardDevice_GetValue_Ex" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_StandardDevice_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Save State ***** ***** ***** ***** ***** ***** ***** *****/
 

STD_ERROR   DLLEXPORT	DRV_StandardDevice_Unplug( int Handle , double lfTimeOut )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfStandardDevice_Unplug						pWrapperFunction						=	NULL;
								  
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.standardDeviceDriverFunctions.StandardDevice_Unplug;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && pDriverInfo && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfTimeOut )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "StandardDevice_Unplug" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_StandardDevice_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Save State ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_StandardDevice_PlugIn( int Handle , double lfTimeOut )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfStandardDevice_PlugIn						pWrapperFunction						=	NULL;
								  
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.standardDeviceDriverFunctions.StandardDevice_PlugIn;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && pDriverInfo && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , lfTimeOut )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "StandardDevice_PlugIn" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_StandardDevice_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Save State ***** ***** ***** ***** ***** ***** ***** *****/


STD_ERROR   DLLEXPORT	DRV_StandardDevice_EnablePlugInAutomatically( int Handle , int bEnable )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfStandardDevice_EnablePlugInAutomatically	pWrapperFunction						=	NULL;
								  
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.standardDeviceDriverFunctions.StandardDevice_EnablePlugInAutomatically;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && pDriverInfo && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , bEnable )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "StandardDevice_EnablePlugInAutomatically" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_StandardDevice_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Save State ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_StandardDevice_InstallReceiveCallbackFunction( int Handle , void *pfFunction , void *pData )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo									*pDriverInfo							=	NULL,
													tDriverInfo								=	{0};
	
	CmtTSVHandle 									VariableHandle							=	0;
	
	CmtThreadLockHandle 							LockHandle								=	0;
										
	pfStandardDevice_InstallReceiveCallbackFunction	pWrapperFunction						=	NULL;
								  
	int												bLocked									=	0;
	
	char											*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.standardDeviceDriverFunctions.StandardDevice_InstallReceiveCallbackFunction;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && pDriverInfo && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pfFunction , pData )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "StandardDevice_InstallReceiveCallbackFunction" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_StandardDevice_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Save State ***** ***** ***** ***** ***** ***** ***** *****/
	
STD_ERROR   DLLEXPORT	DRV_StandardDevice_LoadConfigFile( int Handle , char *pFileName )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfStandardDevice_LoadConfigFile				pWrapperFunction						=	NULL;
								  
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.standardDeviceDriverFunctions.StandardDevice_LoadConfigFile;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && pDriverInfo && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pFileName )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "StandardDevice_LoadConfigFile" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_StandardDevice_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

/***** ***** ***** ***** ***** ***** ***** Save State ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_StandardDevice_LoadConfigBuffer( int Handle , char *pBuffer , int iBufferSize )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfStandardDevice_LoadConfigBuffer			pWrapperFunction						=	NULL;
								  
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.standardDeviceDriverFunctions.StandardDevice_LoadConfigBuffer;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && pDriverInfo && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pBuffer , iBufferSize )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "StandardDevice_LoadConfigBuffer" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_StandardDevice_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


STD_ERROR   DLLEXPORT 	DRV_StandardDevice_EnableLogPrinting ( int Handle , int bAllow )  
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSTD_Config_LOG_SetAllowState				pWrapperFunction						=	NULL;
								  
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.standardDeviceDriverFunctions.Config_LOG_SetAllowState;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && pDriverInfo && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , bAllow )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Config_LOG_SetAllowState" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_StandardDevice_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT   DRV_StandardDevice_Check_Connection( int Handle , char *pCommandName , int *piStatus )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfStandardDevice_Check_Connection			pWrapperFunction						=	NULL;
								  
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.standardDeviceDriverFunctions.StandardDevice_Check_Connection;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && pDriverInfo && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pCommandName , piStatus )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "StandardDevice_Check_Connection" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_StandardDevice_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT   DRV_StandardDevice_Get_Commands_List( int Handle , char ***pCommandsList , int *piNumberOfCommands )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfStandardDevice_Get_Commands_List			pWrapperFunction						=	NULL;
								  
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.standardDeviceDriverFunctions.StandardDevice_Get_Commands_List;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && pDriverInfo && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pCommandsList , piNumberOfCommands )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "StandardDevice_Get_Commands_List" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_StandardDevice_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


/***** ***** ***** ***** ***** ***** ***** Get Device Information Block ***** ***** ***** ***** ***** ***** ***** *****/

STD_ERROR   DLLEXPORT	DRV_StandardDevice_GetDeviceInfoBlock( int Handle , char **pBuffer , int *piBufferSize )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfStandardDevice_GetDeviceInfoBlock			pWrapperFunction						=	NULL;
								  
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.standardDeviceDriverFunctions.StandardDevice_GetDeviceInfoBlock;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && pDriverInfo && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pBuffer , piBufferSize )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "StandardDevice_GetDeviceInfoBlock" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_StandardDevice_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}


STD_ERROR   DLLEXPORT	DRV_StandardDevice_Save_Log_File_As( int Handle , char *pFileNamePath )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfStandardDevice_Save_Log_File_As			pWrapperFunction						=	NULL;
								  
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.standardDeviceDriverFunctions.StandardDevice_Save_Log_File_As;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && pDriverInfo && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pFileNamePath )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "StandardDevice_Save_Log_File_As" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_StandardDevice_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRV_StandardDevice_GetLowLevelHandle( int Handle , int *pInstrumentLowLevelHandle )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSTD_Equipment_GetLowLevelHandle			pWrapperFunction						=	NULL;
								  
	int											bLocked									=	0;
	
	char										*pTempString							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = Handle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	if ( tDriverInfo.InstrumentType != DRIVER_TYPE_STANDARD_DEVICE )
		{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.Equipment_GetLowLevelHandle;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && pDriverInfo && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , pInstrumentLowLevelHandle )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};

		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Equipment_GetLowLevelHandle" , tDriverInfo.pDriverFileName );

		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) CmtReleaseLock (LockHandle);
	
	if ( StdError.error )
	{
		DRV_StandardDevice_GetErrorTextMessage ( VariableHandle , StdError.error , &pTempString );
		
		if ( pTempString && ( strlen(pTempString)))
		{
			SET_DESCRIPTION(pTempString);
		}
		
		FREE(pTempString);
	}
	
	return StdError;
}
