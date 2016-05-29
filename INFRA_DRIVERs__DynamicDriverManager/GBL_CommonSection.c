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


STD_ERROR   DLLEXPORT	DRIVER_MANAGER_GetEquipmentInformation( int hHandle , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo								=	NULL,
												tDriverInfo									=	{0};
	
	CmtTSVHandle 								VariableHandle								=	0;
	
	CmtThreadLockHandle 						LockHandle									=	0;
										
	pfSTD_Equipment_Info						pWrapperFunction							=	NULL;
								  
	int											bLocked										=	0;
	
	if ( hHandle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = hHandle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.Equipment_Info;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentHandle , NULL , pVendor , pSerialNumber , pModel , pFirmware )); 
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Equipment_Info" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) 
		CmtReleaseLock ( LockHandle );
	
	return StdError;
}


STD_ERROR   DLLEXPORT	DRIVER_MANAGER_Equipment_IsSupported ( int hHandle ,  char *pAddress , char *pID_String , char *pIdentificationText , int *pbSupporting )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo								=	NULL,
												tDriverInfo									=	{0};
	
	CmtTSVHandle 								VariableHandle								=	0;
	
	CmtThreadLockHandle 						LockHandle									=	0;
										
	pfSTD_Equipment_IsSupported					pWrapperFunction							=	NULL;
								  
	int											bLocked										=	0;
	
	if ( hHandle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = hHandle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.Equipment_IsSupported;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentLowLevelHandle , NULL , pID_String , pIdentificationText , pbSupporting , NULL ));    
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Equipment_Info" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) 
		CmtReleaseLock ( LockHandle );
	
	return StdError;
}

int	DLLEXPORT	DRIVER_MANAGER_GetDriversFileList( char *pRootDirectory , char ***pDriversList , int **pDriversTypes , int *pCount )
{
	int											iStatus										=	0,
												iLength										=	0,
												iIndex										=	0,
												iTemp										=	0;
	
	char										szCurrentPath[STD_STRING]					=	{0},
												szSearchPath[STD_STRING]					=	{0}, 
												szCheckFile[STD_STRING]						=	{0}, 
												szCurrentFile[STD_STRING]					=	{0},
												szCurrentFilePath[STD_STRING]				=	{0};
	
	HINSTANCE									LibraryHandle;
	
	pfSTD_Equipment_IsSupported					pWrapperFunction							=	NULL;
	
	int											viEquipmentTypes[]							=	{EQUIP_TYPE_DV,EQUIP_TYPE_PS,EQUIP_TYPE_NA,EQUIP_TYPE_SG,EQUIP_TYPE_SA,EQUIP_TYPE_SC,EQUIP_TYPE_FG,EQUIP_TYPE_PM,EQUIP_TYPE_NF,EQUIP_TYPE_MM,EQUIP_TYPE_WA};
	
	int											iTypeIndex									=	0;
	
	if (( pDriversList == NULL ) || ( pCount == NULL ))
		return 0;
		
	if ( pRootDirectory )
		strcpy( szCurrentPath ,pRootDirectory );
	else
		GetModuleDir ( __CVIUserHInst , szCurrentPath );
	
	sprintf( szSearchPath , "%s\\*" , szCurrentPath);
	
	if ( GetFirstFile (szSearchPath, 1, 0, 0, 0, 0, 1, szCurrentFile ) == 0 )
	{
		do
		{
			strcpy( szCheckFile , szCurrentFile );
			
			StringLowerCase (szCheckFile);
			
			if ( strstr( szCheckFile , ".dll" ))
			{
				sprintf( szCurrentFilePath , "%s\\%s" , szCurrentPath , szCurrentFile );    
				
				LibraryHandle = LoadLibrary(szCurrentFilePath);
		
				if ( LibraryHandle )
				{		
					pWrapperFunction = (pfSTD_Equipment_IsSupported) GetProcAddress( LibraryHandle , "Equipment_IsSupported");   
	
					if ( pWrapperFunction )
					{
						iLength = strlen(szCurrentFilePath);
					
						iIndex = *pCount;
					
						*pDriversList = realloc( (*pDriversList) , ((iIndex+1) * sizeof(char*)));
						*pDriversTypes = realloc( (*pDriversTypes) , ((iIndex+1) * sizeof(int)));
						
						//----------- store file name path ------------------------------//
						(*pDriversList)[iIndex] = calloc(( iLength+1 ) , sizeof(char));  
						strcpy( (*pDriversList)[iIndex] , szCurrentFilePath );

						//----------- Determine driver type  ------------------------------//
						for ( iTypeIndex = 0; iTypeIndex < (sizeof(viEquipmentTypes) / sizeof(viEquipmentTypes[0])); iTypeIndex++ )
						{
							switch(viEquipmentTypes[iTypeIndex])
							{
								case EQUIP_TYPE_DV:
													pWrapperFunction = (pfStandardDevice_Init) GetProcAddress( LibraryHandle , "StandardDevice_Init");
													break;
	
								case EQUIP_TYPE_PS:
													pWrapperFunction = (pfPowerSupply_Init) GetProcAddress( LibraryHandle , "PowerSupply_Init");
													break;
	
								case EQUIP_TYPE_NA:
													pWrapperFunction = (pfNetworkAnalyzer_Init) GetProcAddress( LibraryHandle , "NetworkAnalyzer_Init");
													break;
	
								case EQUIP_TYPE_SG:
													pWrapperFunction = (pfSignalGenerator_Init) GetProcAddress( LibraryHandle , "SignalGenerator_Init");
													break;
	
								case EQUIP_TYPE_SA:
													pWrapperFunction = (pfSpectrumAnalyzer_Init) GetProcAddress( LibraryHandle , "SpectrumAnalyzer_Init");
													break;
	
								case EQUIP_TYPE_SC:
													pWrapperFunction = (pfOscilloscope_Init) GetProcAddress( LibraryHandle , "Oscilloscope_Init");
													break;
	
								case EQUIP_TYPE_FG:
													pWrapperFunction = (pfFunctionGenerator_Init) GetProcAddress( LibraryHandle , "FunctionGenerator_Init");
													break;
	
								case EQUIP_TYPE_PM:
													pWrapperFunction = (pfPowerMeter_Init) GetProcAddress( LibraryHandle , "PowerMeter_Init");
													break;
	
								case EQUIP_TYPE_NF:
													pWrapperFunction = (pfNoiseFigure_Init) GetProcAddress( LibraryHandle , "NoiseFigure_Init");
													break;
	
								case EQUIP_TYPE_MM:
													pWrapperFunction = (pfMultiMeter_Init) GetProcAddress( LibraryHandle , "MultiMeter_Init");
													break;
	
								case EQUIP_TYPE_WA:
													pWrapperFunction = (pfWaveAnalyzer_Init) GetProcAddress( LibraryHandle , "WaveAnalyzer_Init");
													break;
	
								default:
													break;
							}
							
							if ( pWrapperFunction )
							{
								(*pDriversTypes)[iIndex] = viEquipmentTypes[iTypeIndex];
								break;
							}
						}
						
						*pCount = iIndex + 1;											 
					}
				
					FreeLibrary(LibraryHandle); 
				}
			}
			
			if ( GetFileAttrs ( szCurrentFilePath , &iTemp , &iTemp , &iTemp , &iTemp ) == 1 )
			{
				iStatus = DRIVER_MANAGER_GetDriversFileList( szCurrentFilePath , pDriversList , pDriversTypes , pCount ); 	
			}
			
		}while(GetNextFile (szCurrentFile) == 0 );
	}
	
	return iStatus;
}

STD_ERROR	DLLEXPORT	DRIVER_MANAGER_CheckEquipmentDriverCompatibility( char *pszDriverLocation , char *pszAddressString , int *pStatus , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware , char **pIDN , char **pID_Responds )
{
	STD_ERROR                                   StdError                                    =   {0};

	HINSTANCE									LibraryHandle;

	pfSTD_Equipment_Info						pInfoWrapperFunction						=	NULL;

	pfSTD_Equipment_IsSupported					pIsSupportedWrapperFunction					=	NULL;
	
	int											iCount										=	0, 
												status										=	0, 
												bSupport									=	0, 
												defaultRM									=	0,
												hConnectionHandle							=	0;
	
	char										szReadBuffer[LOW_STRING]					=	{0},
												szIdString[LOW_STRING]						=	{0};
	
	IF ((( pszDriverLocation == NULL ) || ( pszAddressString == NULL ) || ( pStatus == NULL )) , "The function has received a NULL pointer argument." );
		
	LibraryHandle = LoadLibrary(pszDriverLocation);   
	
	IF (( LibraryHandle == 0 ) , "Loading driver error." );

	if (( pIDN ) && ( *pIDN ))
		strcpy( szIdString , (*pIDN) );
	
	if ( strlen( szIdString ) == 0 )
	{
		strcpy( szIdString , "*IDN?\n" );	
	}
	
	//------------------ Checking for driver supporting ------------------------//
	
	do
	{
		if ( pID_Responds && (*pID_Responds) && strlen(*pID_Responds))
		{
			strcpy ( szReadBuffer , *pID_Responds );
			break;
		}
		
		viOpenDefaultRM (&defaultRM);

		SetBreakOnLibraryErrors (0);
	
		status = viOpen ( defaultRM , pszAddressString , NULL, NULL, &hConnectionHandle );
	
		if ( status == 0 )
		{	
			status = viPrintf ( hConnectionHandle , szIdString );
	
			if ( status == 0 )
			{
				viRead ( hConnectionHandle , szReadBuffer , (LOW_STRING-1) , &iCount );
			}
		}
	
		if ( hConnectionHandle )
			viClose(hConnectionHandle); 
	
		if ( pID_Responds )
		{
			FREE_CALLOC_COPY_STRING( *pID_Responds , szReadBuffer );
		}
		
	}while(0);
	
	SetBreakOnLibraryErrors (1);  
	
	pIsSupportedWrapperFunction = (pfSTD_Equipment_IsSupported) GetProcAddress( LibraryHandle , "Equipment_IsSupported");   

	if ( pIsSupportedWrapperFunction )
	{
		FREE_STDERR_COPY_ERR( pIsSupportedWrapperFunction( 0 , pszAddressString , szIdString , szReadBuffer , &bSupport , NULL )); 
		
		IF (( bSupport == 0 ) , "This driver is not support the current Equipment." );
	}
	else
	{
		char	szMessage[LOW_STRING]	= {0};
	
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Equipment_IsSupported" , pszDriverLocation );
	
		FORCE_ERR( -2 , szMessage );
	}			
	
	if ( IS_OK )
	{
		*pStatus = 1;

		pInfoWrapperFunction = (pfSTD_Equipment_Info) GetProcAddress( LibraryHandle , "Equipment_Info");   

		if ( pInfoWrapperFunction )
		{
			FREE_STDERR_COPY_ERR( pInfoWrapperFunction( 0 , pszAddressString , pVendor , pSerialNumber , pModel , pFirmware )); 
			
			if ( pVendor && *pVendor )
			{
				RemoveSurroundingWhiteSpace (*pVendor);
			}
			
			if ( pSerialNumber && *pSerialNumber )
			{
				RemoveSurroundingWhiteSpace (*pSerialNumber);
			}
			
			if ( pModel && *pModel )
			{
				RemoveSurroundingWhiteSpace (*pModel);
			}
			
			if ( pFirmware && *pFirmware )
			{
				RemoveSurroundingWhiteSpace (*pFirmware);
			}
		}
	}

Error:
		
	if ( pIDN )
	{
		if (( *pIDN == NULL ) || strcmp( *pIDN , szIdString )) 
		{
			FREE_CALLOC_COPY_STRING( *pIDN , szIdString );
		}
	}
	
	if ( LibraryHandle )
		FreeLibrary(LibraryHandle); 
	
	return StdError;
}



int  ShowMessage ( int type , char *pTitle , char *pText, char *pPicture )
{
	STD_ERROR						StdError											=	{0};

	int  							setupHandle											=	0;
	int 							pan													=	0,
									ctl													=	0;
	
	int								iStatus												=	0,
									iTextLength											=	0;
										
	char							*pTemp												=	NULL,
									*pInstructionText									=	NULL,
									*pTestName											=	NULL;
	
	char							szPicLocation[STD_STRING]							=	{0},
									szCurrentDirectory[STD_STRING]						=	{0};
	
	double							lfStartTime											=	0.0, 
									lfCurrentTime										=	0.0;
										
	if ( pText == NULL )
		return 0;
	
	if ( pText )
		iTextLength = strlen(pText);
	
	CALLOC_ERR( pInstructionText , iTextLength + 10 , sizeof(char));
		
	if ( pText )
		strcpy( pInstructionText , pText );

	setupHandle = LoadPanelEx (0, "DriversManagerGUI.uir", SETUP_MSG , __CVIUserHInst);

	switch (type)
	{
		case INSTR_TYPE_CONTINUE:
			
			SetCtrlAttribute(setupHandle,SETUP_MSG_CONTINUE,ATTR_VISIBLE,1);
			
			break;
			
		case INSTR_TYPE_YES_NO:
			
			SetCtrlAttribute(setupHandle,SETUP_MSG_YES,ATTR_VISIBLE,1);
			SetCtrlAttribute(setupHandle,SETUP_MSG_NO,ATTR_VISIBLE,1);
			
			break;
	}
	
	if ( iTextLength )
	{
		pTemp = StringReplaceAll( pInstructionText , "\\n" , "\n" , NULL); 
		
		if ( pTemp )
		{
			strcpy( pInstructionText , pTemp );
			FREE(pTemp);
		}
		
		SetCtrlVal( setupHandle , SETUP_MSG_INSTRACTION_TEXT , pInstructionText );
	}
	
	if (pPicture)
	{
		strcpy( szPicLocation , pPicture );
	
		do
		{
			pTemp = strstr( pPicture , ":\\" );
	
			if ( pTemp )
				break;
		
			pTemp = strchr( pPicture , '\\' ); 
		
			if ( pTemp == NULL )
				break;
			
			GetModuleDir( __CVIUserHInst , szCurrentDirectory ); 
		
			if ( pTemp == pPicture )
				sprintf( szPicLocation , "%s%s" , szCurrentDirectory , pPicture );
			else
				sprintf( szPicLocation , "%s\\%s" , szCurrentDirectory , pPicture ); 
		
		} while(0);
	
		if (( strlen(szPicLocation) > 5 ) && (FileExists(szPicLocation,NULL)))
			DisplayImageFile (setupHandle, SETUP_MSG_INSTRACTION_PICTURE, szPicLocation);
	}
	
	iStatus = 1;

	SetPanelPos ( setupHandle , VAL_AUTO_CENTER , VAL_AUTO_CENTER );
	InstallPopup (setupHandle);
	
	if ( pTitle )
		SetPanelAttribute( setupHandle , ATTR_TITLE , pTitle );    
		
	while(1)
	{
		GetCurrentDateTime(&lfCurrentTime); 
			
		if (( lfCurrentTime - lfStartTime ) > 10.0 )
		{																			   
			MakeApplicationActive ();   
			SetActivePanel( setupHandle ); 
			
			GetCurrentDateTime(&lfStartTime);  
		}
				
		GetUserEvent (0, &pan, &ctl);
	    
	  	if (ctl == SETUP_MSG_CONTINUE)
	    	break;
  
	  	if (ctl == SETUP_MSG_YES)
	    	break;
  
	  	if (ctl == SETUP_MSG_NO)
	  	{
		  	iStatus = 0;
		  	break;
	  	}
	}
	
Error:
	
	RemovePopup (0);

	if ( setupHandle )
		DiscardPanel(setupHandle);
	
	FREE(pInstructionText);
	FREE(pTestName);
	
	return iStatus;
}

STD_ERROR   DLLEXPORT DRV_Config_SetAttribute( int hHandle , char *pAttributeName , char *pAttributeValue )
{							
	STD_ERROR                                   StdError                                    =   {0};
									
	tsDriverInfo								*pDriverInfo								=	NULL,
												tDriverInfo									=	{0};
	
	CmtTSVHandle 								VariableHandle								=	0;
	
	CmtThreadLockHandle 						LockHandle									=	0;
										
	pfSTD_Config_SetAttribute					pWrapperFunction							=	NULL;
								  
	int											bLocked										=	0;
	
	if ( hHandle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = hHandle;
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.Config_SetAttribute;
	
	CHK_PROCCESS_GET_LOCK ( LockHandle );
	
    if ( pWrapperFunction && ( pDriverInfo->bDemoMode == 0 ))
		FREE_STDERR_COPY_ERR( pWrapperFunction( tDriverInfo.InstrumentLowLevelHandle , pAttributeName , pAttributeValue ));    
	
	if ( pWrapperFunction == NULL )
	{
		char	szMessage[LOW_STRING]	= {0};
		
		sprintf( szMessage , "Method \"%s\"\nis not implemented in driver:\n%s" , "Config_SetAttribute" , tDriverInfo.pDriverFileName );
		
		ShowMessage ( INSTR_TYPE_CONTINUE , "Implementation Error . . ." , szMessage , NULL );
	}
	
Error:
	
    if ( LockHandle && bLocked ) 
		CmtReleaseLock ( LockHandle );
	
	return StdError;
}
