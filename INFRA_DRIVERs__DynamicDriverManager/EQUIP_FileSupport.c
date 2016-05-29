//==============================================================================
// Include files
#include <windows.h> 
#include <utility.h>
#include "GBL_CommonSection.h"
#include "GlobalDefines.h"
#include <formatio.h>
#include <ansi_c.h>
#include <userint.h>
#include "MAIN_DriversManager.h"
#include "toolbox.h"
#include "DriversManagerGUI.h"
#include "StringParsing.h"
#include "EQUIP_FileSupport.h"

//==============================================================================
// Constants

#define			EVENT_LIST_TRIGGER_CHANGE									1001
#define			EVENT_LIST_TRIGGER_DIR										1002

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


STD_ERROR   DLLEXPORT	DRIVER_MANAGER_Equipment_BrowseSelectStateFiles ( int hHandle , char *pStoreFilePath , int *piNumberOfSelectedFiles , char ***pvszSelectedFileNamesList , int **pvszSelectedFileSizesList )
{		
	STD_ERROR						StdError											=	{0};

	int								hPanelBrowser										=	0;

	int								iGetDirectoryIndex									=	0,
									iCurrentDirectoryIndex								=	0,
									iPreviousDirectoryIndex								=	0;
	
	char							**vszFileNames										=	NULL,
									**vszGetFileNames									=	NULL;
	
	char							*pTemp												=	NULL,
									*pFileName											=	NULL,    
									*pDirectoryName										=	NULL;
	
	int								*pvFileSizes										=	NULL,
									*pvGetFileSizes										=	NULL;
	
	int								iNumberOfFiles										=	0,
									iNumberOfDirItems									=	0,
									iPreviousNumberOfFiles								=	0; 
														  
	int								iFileIndex											=	0,
									iDirectoryIndex										=	0,
									iCurrentSelectedFileIndex							=	0;  
									
	int								bChangeDirectory									=	0,
									bRootDirectorySetted								=	0;
	
	char							szTransferDirectory[LOW_STRING]						=	{0}, 
									szCurrentSelectedDir[LOW_STRING]					=	{0},
									szSelectedStateFile[LOW_STRING]						=	{0}, 
									szSelectionDirectory[LOW_STRING]					=	{0},
									szTransferFilePath[STD_STRING]						=	{0};
	
	int								bPopup												=	0;
	
	int								event												=	0,
									panel												=	0,
									control												=	0;
	
	SetWaitCursor (1);
	
	CHK_STDERR( DRIVER_MANAGER_Equipment_GetFileCatalog ( hHandle , NULL , &vszFileNames , &pvFileSizes , &iNumberOfFiles )); 
	
	IF ((( iNumberOfFiles == 0 ) || ( vszFileNames == NULL )) , "Can't get file names list" );
		
	CHK_UIL ( hPanelBrowser = LoadPanelEx (0, "DriversManagerGUI.uir" , FILES , __CVIUserHInst));
	
	InstallPopup (hPanelBrowser);
	bPopup = 1;
	 
	for ( iFileIndex = 0; iFileIndex < iNumberOfFiles; iFileIndex++ )
		InsertListItem( hPanelBrowser , FILES_LIST, -1, vszFileNames[iFileIndex], (iFileIndex+1) );  
	
	do
	{
		SetWaitCursor (0);
		
		MakeApplicationActive ();
		SetActivePanel ( hPanelBrowser );
		ProcessDrawEvents();
		
		event = GetUserEvent ( 1 , &panel , &control );   
		
		if ( panel != hPanelBrowser )
			continue;
		
		if ( control == FILES_LIST ) 
		{
			GetCtrlIndex( panel , control , &iCurrentSelectedFileIndex );
			
			if ( event == EVENT_LIST_TRIGGER_DIR )
			{
				pDirectoryName = strchr( vszFileNames[iCurrentSelectedFileIndex] , '[' );
				
				if ( pDirectoryName )
				{
					pDirectoryName++;
					
					pTemp = strchr( pDirectoryName , ']' ); 
					
					if ( pTemp )
						*pTemp = 0;
					
					if ( bRootDirectorySetted == 0 )
					{
						GetCtrlIndex( panel , FILES_DIR , &iGetDirectoryIndex );
			
						GetLabelFromIndex( panel , FILES_DIR , iGetDirectoryIndex , szCurrentSelectedDir );
			
						bRootDirectorySetted = 1;
			
						DeleteListItem( panel , FILES_DIR , 0 , -1 );     
			
						InsertListItem( panel , FILES_DIR, -1, szCurrentSelectedDir , 0 ); 
					}
			
					iPreviousDirectoryIndex = iCurrentDirectoryIndex;
					
					iCurrentDirectoryIndex++;
					
					InsertListItem( panel , FILES_DIR, iCurrentDirectoryIndex , pDirectoryName , iCurrentDirectoryIndex ); 
					
					GetNumListItems( panel , FILES_DIR , &iNumberOfDirItems );  
					
					if ( iNumberOfDirItems > ( iCurrentDirectoryIndex + 1 ))
						DeleteListItem( panel , FILES_DIR , (iCurrentDirectoryIndex+1) , -1 );  
					
					SetCtrlIndex( panel , FILES_DIR , iCurrentDirectoryIndex );
					
					bChangeDirectory = 1;
				}
			}
		}
		
		if ( control == FILES_LOAD )
		{
			if ( FileSelectPopup ("", "*.*", "*.state;*.sta;*.*", "", VAL_LOAD_BUTTON, 0, 0, 1, 1, szTransferFilePath ) == VAL_EXISTING_FILE_SELECTED )
			{
				pFileName =  strrchr( szTransferFilePath , '\\' );
				
				if ( pFileName )
				{
					pFileName++;
					
					GetCtrlIndex( panel , FILES_DIR , &iGetDirectoryIndex );  
			
					szSelectionDirectory[0] = 0;
			
					if ( bRootDirectorySetted )
					{
						for( iDirectoryIndex = 0; iDirectoryIndex <= iGetDirectoryIndex; iDirectoryIndex++ )
						{
							strcpy( szTransferDirectory , szSelectionDirectory );
				
							GetLabelFromIndex( panel , FILES_DIR , iDirectoryIndex , szCurrentSelectedDir ); 
				
							if ( strlen(szSelectionDirectory))
								sprintf( szSelectionDirectory , "%s\\%s" , szTransferDirectory , szCurrentSelectedDir );
							else
								strcpy( szSelectionDirectory , szCurrentSelectedDir );
						}
					}
			
					strcpy( szTransferDirectory , szSelectionDirectory ); 
			
					sprintf( szSelectedStateFile , "%s\\%s" , szTransferDirectory , pFileName );   
			
					UPDATERR( DRIVER_MANAGER_Equipment_SendStateFile ( hHandle , szTransferFilePath , szSelectedStateFile )); 
			
					if ( IS_OK )
					{
						bChangeDirectory = 1;
					}
				}
			}
		}
		
		if ( control == FILES_DELETE )
		{
			GetCtrlIndex( panel , FILES_DIR , &iGetDirectoryIndex );  
			
			szSelectionDirectory[0] = 0;
		
			if ( bRootDirectorySetted )
			{
				for( iDirectoryIndex = 0; iDirectoryIndex <= iGetDirectoryIndex; iDirectoryIndex++ )
				{
					strcpy( szTransferDirectory , szSelectionDirectory );
			
					GetLabelFromIndex( panel , FILES_DIR , iDirectoryIndex , szCurrentSelectedDir ); 
			
					if ( strlen(szSelectionDirectory))
						sprintf( szSelectionDirectory , "%s\\%s" , szTransferDirectory , szCurrentSelectedDir );
					else
						strcpy( szSelectionDirectory , szCurrentSelectedDir );
				}
			}
		
			GetCtrlIndex( panel , FILES_LIST , &iCurrentSelectedFileIndex );
		
			strcpy( szTransferDirectory , szSelectionDirectory ); 
		
			sprintf( szSelectedStateFile , "%s\\%s" , szTransferDirectory , vszFileNames[iCurrentSelectedFileIndex] );   
		
			UPDATERR( DRIVER_MANAGER_Equipment_DeleteStateFile ( hHandle , szSelectedStateFile )); 
		
			if ( IS_OK )
			{
				bChangeDirectory = 1;
			}
			
		}
		
		if ((( control == FILES_DIR ) && ( event == EVENT_COMMIT )) || ( bChangeDirectory ))
		{
			GetCtrlIndex( panel , FILES_DIR , &iGetDirectoryIndex );  
			
			szSelectionDirectory[0] = 0;
			
			if ( bRootDirectorySetted )
			{
				for( iDirectoryIndex = 0; iDirectoryIndex <= iGetDirectoryIndex; iDirectoryIndex++ )
				{
					strcpy( szTransferDirectory , szSelectionDirectory );
				
					GetLabelFromIndex( panel , FILES_DIR , iDirectoryIndex , szCurrentSelectedDir ); 
				
					if ( strlen(szSelectionDirectory))
						sprintf( szSelectionDirectory , "%s\\%s" , szTransferDirectory , szCurrentSelectedDir );
					else
						strcpy( szSelectionDirectory , szCurrentSelectedDir );
				}
			}
			else
			{
				GetCtrlIndex( panel , FILES_DIR , &iGetDirectoryIndex );
			
				GetLabelFromIndex( panel , FILES_DIR , iGetDirectoryIndex , szCurrentSelectedDir );
						
				strcpy( szSelectionDirectory , szCurrentSelectedDir ); 
			}
			
			SetWaitCursor (1);
			
			iPreviousNumberOfFiles = iNumberOfFiles;
			
			UPDATERR( DRIVER_MANAGER_Equipment_GetFileCatalog ( hHandle , szSelectionDirectory , &vszGetFileNames , &pvGetFileSizes , &iNumberOfFiles ));
			
			if ( IS_OK )
			{
				SetCtrlVal( panel , FILES_PATH, szSelectionDirectory );
				
				FREE_LIST(vszFileNames,iPreviousNumberOfFiles);
				FREE(pvFileSizes);
			
				vszFileNames = vszGetFileNames;
				pvFileSizes = pvGetFileSizes;
				
				vszGetFileNames = NULL;
				pvGetFileSizes = NULL;
				
				DeleteListItem( panel , FILES_LIST , 0 , -1 );  
			
				for ( iFileIndex = 0; iFileIndex < iNumberOfFiles; iFileIndex++ )
					InsertListItem( panel , FILES_LIST, -1, vszFileNames[iFileIndex], (iFileIndex+1) ); 
				
				if ( bRootDirectorySetted == 0 )
				{
					bRootDirectorySetted = 1;
				
					DeleteListItem( panel , FILES_DIR , 0 , -1 );     
				
					InsertListItem( panel , FILES_DIR, -1, szCurrentSelectedDir , 0 ); 
				}
				
				iPreviousDirectoryIndex = iCurrentDirectoryIndex;   
				
				GetCtrlIndex( panel , FILES_DIR , &iCurrentDirectoryIndex );
			}
			else
			{
				SetCtrlIndex( panel , FILES_DIR , iPreviousDirectoryIndex ); 
			}
			
			bChangeDirectory = 0; 
		}

		if ( control == FILES_SAVE )
		{
			GetCtrlIndex( panel , FILES_DIR , &iGetDirectoryIndex );  
			
			szSelectionDirectory[0] = 0;
			
			if ( bRootDirectorySetted )
			{
				for( iDirectoryIndex = 0; iDirectoryIndex <= iGetDirectoryIndex; iDirectoryIndex++ )
				{
					strcpy( szTransferDirectory , szSelectionDirectory );
				
					GetLabelFromIndex( panel , FILES_DIR , iDirectoryIndex , szCurrentSelectedDir ); 
				
					if ( strlen(szSelectionDirectory))
						sprintf( szSelectionDirectory , "%s\\%s" , szTransferDirectory , szCurrentSelectedDir );
					else
						strcpy( szSelectionDirectory , szCurrentSelectedDir );
				}
			}
			
			GetCtrlIndex( panel , FILES_LIST , &iCurrentSelectedFileIndex );
			
			strcpy( szTransferDirectory , szSelectionDirectory ); 
			
			sprintf( szSelectedStateFile , "%s\\%s" , szTransferDirectory , vszFileNames[iCurrentSelectedFileIndex] );   
			
			UPDATERR( DRIVER_MANAGER_Equipment_ReceiveStateFile ( hHandle , "temp_state_file" , szSelectedStateFile )); 
			
			if ( IS_OK )
			{
				if ( DirSelectPopup ("", "Select Directory", 1, 1, szTransferFilePath ))
				{
					strcat( szTransferFilePath , "\\" );
					strcat( szTransferFilePath , vszFileNames[iCurrentSelectedFileIndex] );
					
					CopyFile ("temp_state_file", szTransferFilePath );	
				}
			}
		}
		
	}while ( control != FILES_QUIT );
	
Error:
	
	SetWaitCursor (0); 
	
	if ( hPanelBrowser )
	{
		if (bPopup)
			RemovePopup (0);
		
		DiscardPanel ( hPanelBrowser );
	}
	
	FREE_LIST(vszFileNames,iNumberOfFiles);
	FREE(pvFileSizes);
				
	return StdError;
}

int CVICALLBACK clbListTrigger (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			QueueUserEvent ( EVENT_LIST_TRIGGER_CHANGE , panel, FILES_LIST ); 
			
			break;
		
		case EVENT_LEFT_DOUBLE_CLICK:

			QueueUserEvent ( EVENT_LIST_TRIGGER_DIR , panel, FILES_LIST ); 
			
			break;
	}
	
	return 0;
}


STD_ERROR   DLLEXPORT	DRIVER_MANAGER_Equipment_SendStateFile ( int hHandle , char *szLocalFileName ,char *szStateFileName )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo								=	NULL,
												tDriverInfo									=	{0};
	
	CmtTSVHandle 								VariableHandle								=	0;
	
	CmtThreadLockHandle 						LockHandle									=	0;
										
	pfSTD_Equipment_SendStateFile				pWrapperFunction							=	NULL;
								  
	int											bLocked										=	0;
	
	if ( hHandle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = hHandle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.Equipment_SendStateFile;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , szLocalFileName , szStateFileName ));    
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Equipment_SendStateFile" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) 
		CmtReleaseLock ( LockHandle );
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRIVER_MANAGER_Equipment_ReceiveStateFile ( int hHandle , char *szLocalFileName ,char *szStateFileName )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo								=	NULL,
												tDriverInfo									=	{0};
	
	CmtTSVHandle 								VariableHandle								=	0;
	
	CmtThreadLockHandle 						LockHandle									=	0;
										
	pfSTD_Equipment_ReceiveStateFile			pWrapperFunction							=	NULL;
								  
	int											bLocked										=	0;
	
	if ( hHandle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = hHandle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.Equipment_ReceiveStateFile;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , szLocalFileName , szStateFileName ));    
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Equipment_ReceiveStateFile" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) 
		CmtReleaseLock ( LockHandle );
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRIVER_MANAGER_Equipment_GetFileCatalog ( int hHandle , char *szPath , char ***szFileNames , int **pvFileSizes , int *piNumberOfFiles )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo								=	NULL,
												tDriverInfo									=	{0};
	
	CmtTSVHandle 								VariableHandle								=	0;
	
	CmtThreadLockHandle 						LockHandle									=	0;
										
	pfSTD_Equipment_GetFileCatalog				pWrapperFunction							=	NULL;
								  
	int											bLocked										=	0;
	
	if ( hHandle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = hHandle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.Equipment_GetFileCatalog;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , szPath , szFileNames , pvFileSizes , piNumberOfFiles ));    
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Equipment_GetFileCatalog" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) 
		CmtReleaseLock ( LockHandle );
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRIVER_MANAGER_Equipment_MakeFileCatalog ( int hHandle , char *szPath )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo								=	NULL,
												tDriverInfo									=	{0};
	
	CmtTSVHandle 								VariableHandle								=	0;
	
	CmtThreadLockHandle 						LockHandle									=	0;
										
	pfSTD_Equipment_MakeFileCatalog				pWrapperFunction							=	NULL;
								  
	int											bLocked										=	0;
	
	if ( hHandle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = hHandle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.Equipment_MakeFileCatalog;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , szPath ));    
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Equipment_MakeFileCatalog" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) 
		CmtReleaseLock ( LockHandle );
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRIVER_MANAGER_Equipment_DeleteFileCatalog ( int hHandle , char *szCatalogName )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo								=	NULL,
												tDriverInfo									=	{0};
	
	CmtTSVHandle 								VariableHandle								=	0;
	
	CmtThreadLockHandle 						LockHandle									=	0;
										
	pfSTD_Equipment_DeleteFileCatalog			pWrapperFunction							=	NULL;
								  
	int											bLocked										=	0;
	
	if ( hHandle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = hHandle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.Equipment_DeleteFileCatalog;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , szCatalogName ));    
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Equipment_DeleteFileCatalog" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) 
		CmtReleaseLock ( LockHandle );
	
	return StdError;
}

STD_ERROR   DLLEXPORT	DRIVER_MANAGER_Equipment_DeleteStateFile ( int hHandle , char *szStateFileName )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo								=	NULL,
												tDriverInfo									=	{0};
	
	CmtTSVHandle 								VariableHandle								=	0;
	
	CmtThreadLockHandle 						LockHandle									=	0;
										
	pfSTD_Equipment_DeleteStateFile				pWrapperFunction							=	NULL;
								  
	int											bLocked										=	0;
	
	if ( hHandle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = hHandle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.Equipment_DeleteStateFile;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , szStateFileName ));    
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Equipment_DeleteStateFile" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) 
		CmtReleaseLock ( LockHandle );
	
	return StdError;
}

