//==============================================================================
// Include files

#include <windows.h>
#include "toolbox.h"
#include "auml24xx.h"
#include <cvirte.h>
#include <userint.h>
#include <visa.h>
#include <utility.h>
#include "GlobalDefines.h"
#include <formatio.h>

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
int		Delay_LastComplete( int handle , double lfTimeout );

//==============================================================================
// DLL main entry-point functions

int __stdcall DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            if (InitCVIRTE (hinstDLL, 0, 0) == 0)
                return 0;     /* out of memory */
            break;
        case DLL_PROCESS_DETACH:
            CloseCVIRTE ();
            break;
    }
    
    return 1;
}

int __stdcall DllEntryPoint (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    /* Included for compatibility with Borland */

    return DllMain (hinstDLL, fdwReason, lpvReserved);
}

void* DLLEXPORT PowerMeter_GetLastErrorTextMessage( int handle , int iError , char **pErrorMessage )
{
	STD_ERROR					StdError									=	{0};
	
													
	char					szLocalErrorMessage[5000]			=	{0};
	
	int						iSize								=	0;
	
	if ( pErrorMessage ) 
	{
		auml24xx_error_message ( handle , iError , szLocalErrorMessage );

		iSize = strlen(szLocalErrorMessage);
	
		CALLOC_COPY_ERR( *pErrorMessage , iSize , sizeof(char) , szLocalErrorMessage , iSize );
	}
	
Error:	
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT PowerMeter_GetErrorTextMessage( int handle , int iError , char *pErrorMessage )
{
	STD_ERROR					StdError									=	{0};
	
	if ( pErrorMessage ) 
	{
		auml24xx_error_message ( handle , iError , pErrorMessage );

	}
	
//Error:	
	
	RETURN_STDERR_POINTER;
}

void*	DLLEXPORT		Equipment_Info ( int hLowLevelHandle , char *pAddress , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware )
{
	STD_ERROR						StdError									=	{0};
	
	int								iCount										=	0, 
									status										=	0, 
									iIndex										=	0, 
									bSupport									=	0, 
									defaultRM									=	0,
									hConnectionHandle							=	0;
	
	char							szIdentificationText[LOW_STRING]			=	{0},
									*pTemp										=	NULL;

	if ( hLowLevelHandle == 0 )
	{
		viOpenDefaultRM (&defaultRM);

		SetBreakOnLibraryErrors (0);
	
		status = viOpen ( defaultRM , pAddress , NULL, NULL, &hConnectionHandle );
	}
	else
	{
		hConnectionHandle = hLowLevelHandle;	
	}
		
	if ( status == 0 )
	{	
		status = viPrintf ( hConnectionHandle , "*IDN?\n" );
	
		if ( status == 0 )
		{
			viRead ( hConnectionHandle , szIdentificationText , (LOW_STRING-1) , &iCount );
			RemoveSurroundingWhiteSpace (szIdentificationText); 
		}
	}
		
	if (( hLowLevelHandle == 0 ) && hConnectionHandle )
		viClose(hConnectionHandle); 
		
	SetBreakOnLibraryErrors (1);

	do
	{
		IF (( strstr( szIdentificationText , "ANRITSU" ) == NULL ) , "Is not supported" );
	
		pTemp = strrchr( szIdentificationText , ',' );

		if ( pTemp )
		{
			*pTemp = 0;
			pTemp++;
	
			if ( pFirmware )
			{
				CALLOC_COPY_STRING( *pFirmware , pTemp );
			}
		}
		else
			break;
	
		pTemp = strrchr( szIdentificationText , ',' );

		if ( pTemp )
		{
			*pTemp = 0;
			pTemp++;
	
			if ( pSerialNumber )
			{
				CALLOC_COPY_STRING( *pSerialNumber , pTemp );
			}
		}
		else
			break;
	
		pTemp = strrchr( szIdentificationText , ',' );

		if ( pTemp )
		{
			*pTemp = 0;
			pTemp++;
	
			if ( pModel )
			{
				CALLOC_COPY_STRING( *pModel , pTemp );
			}
		}
		else
			break;
	
		if ( pVendor )
		{
			CALLOC_COPY_STRING( *pVendor , szIdentificationText );  
		}
		
	} while(0);
		
Error:
		
	RETURN_STDERR_POINTER;
}

void*	DLLEXPORT		Equipment_GetLowLevelHandle( int hHandle , int *phLowLevelHandle )
{
	STD_ERROR						StdError									=	{0};
	
	if ( phLowLevelHandle )
		*phLowLevelHandle = hHandle;
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		Equipment_IsSupported ( int hLowLevelHandle , char *pAddress , char *pID_String , char *pIdentificationText , int *pbSupporting , void **pLocalData )
{
	STD_ERROR						StdError									=	{0};
	
	int								iCount										=	0, 
									status										=	0, 
									iIndex										=	0, 
									bSupport									=	0, 
									defaultRM									=	0,
									hConnectionHandle							=	0;
	
	char							szIdentificationText[LOW_STRING]			=	{0};
	
	char							vszSupportdedModels[][32]					=	{{"ML2487A"},{"ML2488A"},{"ML2495A"},{"ML2496A"},{"ML2487B"},{"ML2488B"}};
									
	if (( pIdentificationText == NULL ) || ( strlen(pIdentificationText) < 10 ))
	{
		SetBreakOnLibraryErrors (0);   
		
		if ( hLowLevelHandle == 0 )
		{
			viOpenDefaultRM (&defaultRM);

			SetBreakOnLibraryErrors (0);
	
			status = viOpen ( defaultRM , pAddress , NULL, NULL, &hConnectionHandle );
		}
		else
		{
			hConnectionHandle = hLowLevelHandle;	
		}
		
		if ( status == 0 )
		{	
			status = viPrintf ( hConnectionHandle , "*IDN?\n" );
		
			if ( status == 0 )
			{
				viRead ( hConnectionHandle , szIdentificationText , (LOW_STRING-1) , &iCount );
				RemoveSurroundingWhiteSpace (szIdentificationText); 
			}
		}
		
		if (( hLowLevelHandle == 0 ) && hConnectionHandle )
			viClose(hConnectionHandle); 
		
		SetBreakOnLibraryErrors (1);  	
	}
	else
	{
		if ( strlen(pIdentificationText) < LOW_STRING )
			strcpy( szIdentificationText , pIdentificationText );
		else
			memcpy( szIdentificationText , pIdentificationText , (LOW_STRING-1) );  
	}
		
	do
	{
		if ( strstr( szIdentificationText , "ANRITSU" ) == NULL ) 
			break;
	
		for ( iIndex = 0; iIndex < (sizeof(vszSupportdedModels) / sizeof(vszSupportdedModels[0])); iIndex++ )
			if ( strstr( szIdentificationText , vszSupportdedModels[iIndex]) )
			{
				bSupport = 1; 
				break;
			}
					
	}while(0);
	
	if (pbSupporting)
		*pbSupporting = bSupport;
	
	RETURN_STDERR_POINTER;
}

void*	DLLEXPORT		Config_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet )
{
	STD_ERROR						StdError									=	{0};
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT PowerMeter_Init( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *pInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChannelsQuantity )
{			
	STD_ERROR					StdError					=	{0};
	
	IF(( szRsrcAddress == NULL ) , "Empty Instrument driver"  );
	IF(( InstrumentConnectStatus == NULL ) , "Passed null pointer"  );
	IF(( InstrumentChannelsQuantity == NULL ) , "Passed null pointer"  );
	
	*InstrumentChannelsQuantity = 0;
	*InstrumentConnectStatus = 0;
	
	if ( hParentInstrumentHandle == 0 )
	{
		StdError.error = auml24xx_init ( szRsrcAddress , VI_TRUE , VI_OFF , pInstrumentHandle );
		
		if ( StdError.error == 0 )
		{
			auml24xx_GetAttributeViInt32 ( *pInstrumentHandle , "" , AUML24XX_ATTR_CHANNEL_COUNT , InstrumentChannelsQuantity ); 
			
			Delay_LastComplete( *pInstrumentHandle , 10.0 ); 
			
			*InstrumentConnectStatus = 1;
		}
			
	}
	else
	{
		*InstrumentConnectStatus = 1;
	}
		
Error:
	
	RETURN_STDERR_POINTER;  
}


void* DLLEXPORT PowerMeter_Close( int *phInstrumentHandle )
{	
	STD_ERROR					StdError					=	{0};
	
	
	IF ( (phInstrumentHandle == NULL) , "Pointer passed NULL." );
						  
	auml24xx_close (*phInstrumentHandle);
	*phInstrumentHandle=0;
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Reset( int hInstrumentHandle )
{	
	STD_ERROR					StdError					=	{0};
	
	
	StdError.error = auml24xx_reset ( hInstrumentHandle );
	
	Delay_LastComplete( hInstrumentHandle , 10.0 );
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Configuration( int hInstrumentHandle , int iChannel , int bContinue , int bDelayAutoState , int iTriggerSource , double lfTrigHoldOff , double lfGateOffsetTime , double lfGateTime , double lfTraceTime )
{
	STD_ERROR					StdError					=	{0};
	
	char						szChannelName[LOW_STRING]   =	{0};
	
	sprintf( szChannelName , "CHACTIV %d\n" , iChannel );
		
	CHK_ERR( auml24xx_WriteInstrData ( hInstrumentHandle , szChannelName ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );   
		
	CHK_ERR( auml24xx_ConfigureActiveChannel( hInstrumentHandle , szChannelName ));

	CHK_ERR( auml24xx_Initiate ( hInstrumentHandle ));
	/*
	CHK_ERR( auml24xx_configureTrigger ( hInstrumentHandle , iChannel , bDelayAutoState , iTriggerSource ));
	
	CHK_ERR( auml24xx_configureChannelATrigger ( hInstrumentHandle , 0.000 , lfTrigHoldOff , 0.00 , 0.0 , auml24xx_STATE_ON, auml24xx_SLOPE_POS));
																			 
	CHK_ERR( auml24xx_e9320ConfigureGate ( hInstrumentHandle , iChannel , 1 , lfGateOffsetTime , lfGateTime ));
	
	CHK_ERR( auml24xx_e9320ConfigureTrace ( hInstrumentHandle , iChannel , VI_TRUE , 0.0 , lfTraceTime , -50.0 , 20.0 , auml24xx_UNIT_DBM));
	*/
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetSingleSweep( int hInstrumentHandle , int iChannel )
{
	STD_ERROR					StdError					=	{0};
	
	char						szChannelName[LOW_STRING]   =	{0};
	
	sprintf( szChannelName , "CHACTIV %d\n" , iChannel );
		
	CHK_ERR( auml24xx_WriteInstrData ( hInstrumentHandle , szChannelName ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );   
	
	CHK_ERR(  auml24xx_ConfigureTriggerSource ( hInstrumentHandle ,  AUML24XX_VAL_IMMEDIATE ));
	
	Delay_LastComplete( hInstrumentHandle , 10.0 );  
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetContinuesSweep( int hInstrumentHandle , int iChannel ) 
{
	STD_ERROR					StdError					=	{0};

	char						szChannelName[LOW_STRING]   =	{0};
	
	sprintf( szChannelName , "CHACTIV %d\n" , iChannel );
		
	CHK_ERR( auml24xx_WriteInstrData ( hInstrumentHandle , szChannelName ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );   
	
	CHK_ERR(  auml24xx_ConfigureTriggerSource ( hInstrumentHandle , AUML24XX_VAL_CONT ));
	
	Delay_LastComplete( hInstrumentHandle , 10.0 );  
	
Error:	
	
	RETURN_STDERR_POINTER;  
}


void* DLLEXPORT PowerMeter_InitSweep( int hInstrumentHandle , int iChannel ) 
{
	STD_ERROR					StdError					=	{0};

	CHK_ERR( auml24xx_Initiate ( hInstrumentHandle ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );     
	
Error:	
	
	RETURN_STDERR_POINTER;  
}


void* DLLEXPORT PowerMeter_RecallState( int hInstrumentHandle , int iStateNumber ) 
{
	STD_ERROR					StdError					=	{0};
	
	char						szBuffer[LOW_STRING]  		=	{0};  
	
	sprintf( szBuffer , "*RCL %d\n" , iStateNumber );
	
	CHK_ERR( auml24xx_WriteInstrData ( hInstrumentHandle , szBuffer ));  
	
	Delay_LastComplete( hInstrumentHandle , 20.0 );  
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_RecallState_ByName( int hInstrumentHandle , char *pszName ) 
{
	STD_ERROR					StdError					=	{0};
	
	char						szBuffer[STD_STRING+1]  	=	{0},
								*pState						=	NULL,
								*pSeparator					=	NULL,
								*pStateName					=	NULL;  
	
	int							iStateNumber				=	0,
								iCount						=	0;
	
	Delay_LastComplete( hInstrumentHandle , 20.0 ); 
	
	CHK_ERR( auml24xx_WriteInstrData ( hInstrumentHandle , "NVNAME?\n" ));  
	
	DelayWithEventProcessing(0.2);
	
	CHK_ERR( auml24xx_ReadInstrData ( hInstrumentHandle , STD_STRING , szBuffer , &iCount )); 
	
	pStateName = SkipWhiteSpace (pszName);
	
	pSeparator = strstr( szBuffer , pStateName );
	
	IF (( pSeparator == NULL ) , "Error" );
	
	*pSeparator = 0;
		
	pSeparator = strrchr( szBuffer , ',' ); 
	
	IF (( pSeparator == NULL ) , "Error" );  
	
	*pSeparator = 0; 
	
	pState = strrchr( szBuffer , ',' );  
		
	if ( pState == NULL)
	{
		pState = strchr( szBuffer , ' ' );  
	}
	
	pState++; 
		
	iStateNumber = atoi(pState);
	
	sprintf( szBuffer , "*RCL %d\n" , iStateNumber );
	
	CHK_ERR( auml24xx_WriteInstrData ( hInstrumentHandle , szBuffer ));  
	
	Delay_LastComplete( hInstrumentHandle , 20.0 );  
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SaveState( int hInstrumentHandle , int iStateNumber ) 
{
	STD_ERROR					StdError					=	{0};
	
	char						szBuffer[LOW_STRING]  		=	{0};
	
	sprintf( szBuffer , "*SAV %d\n" , iStateNumber );
	
	CHK_ERR( auml24xx_WriteInstrData ( hInstrumentHandle , szBuffer ));  

	Delay_LastComplete( hInstrumentHandle , 10.0 );  
	
Error:	
	
	RETURN_STDERR_POINTER;  
}


void* DLLEXPORT PowerMeter_GetStateFile( int hInstrumentHandle , int iStateNumber , char *pszFileName ) 
{
	STD_ERROR					StdError					=	{0};
	
	
	char						szBuffer[LOW_STRING]  		=	{0},
								*pBuffer					=	NULL;  
	
	int							iCount						=	0;
	
	int							iNumOfDigits				=	0,
								iNumOfData					=	0;
	
	int							hFileHandle					=	0;
	
	sprintf( szBuffer , "NVOUT %d\n" , iStateNumber );
	
	CHK_ERR( auml24xx_WriteInstrData ( hInstrumentHandle , szBuffer ));  
	
	DelayWithEventProcessing(0.2);
	
	CHK_ERR( auml24xx_ReadInstrData ( hInstrumentHandle , 8 , szBuffer , &iCount )); 
		
	sscanf( szBuffer , "NVOUT #%d" , &iNumOfDigits );
	
	if ( iNumOfDigits )
	{
		CHK_ERR( auml24xx_ReadInstrData ( hInstrumentHandle , iNumOfDigits+1 , szBuffer , &iCount ));   
		
		sscanf( szBuffer , "%d" , &iNumOfData );
		
		if ( iNumOfData )
		{   
			CALLOC_ERR( pBuffer , iNumOfData+20 , sizeof(char));
			
			CHK_ERR( auml24xx_ReadInstrData ( hInstrumentHandle , iNumOfData , pBuffer , &iCount ));
				
			IF (( iCount != iNumOfData ) , "Wrong date" );

			hFileHandle = OpenFile ( pszFileName , VAL_WRITE_ONLY, VAL_TRUNCATE, VAL_BINARY);
			
			iCount = WriteFile ( hFileHandle , pBuffer , iNumOfData );
			
			CloseFile ( hFileHandle );
			
			IF (( iCount != iNumOfData ) , "Wrong date" ); 
		}
	}
Error:	
	
	FREE(pBuffer);
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetStateFile( int hInstrumentHandle , int iStateNumber , char *pszFileName ) 
{
	STD_ERROR					StdError					=	{0};
	
	
	char						szBuffer[LOW_STRING]  		=	{0},
								*pBuffer					=	NULL;  
	
	int							iCount						=	0,
								iSendCount					=	0;
	
	int							iPrefixCount				=	0,
								iNumOfData					=	0;

	int							hFileHandle					=	0,
								hVisaSession				=	0;
	
	IF (( FileExists( pszFileName , &iNumOfData ) == 0 ) , "File error" );
	
	iPrefixCount = sprintf( szBuffer , "NVLOAD %d,%d," , iStateNumber , iNumOfData ); 
	
	CALLOC_ERR( pBuffer , iNumOfData+iPrefixCount+2 , sizeof(char)); 
	
	strcpy( pBuffer , szBuffer );
	
	hFileHandle = OpenFile (pszFileName, VAL_READ_ONLY, VAL_TRUNCATE,VAL_BINARY);
	
	iCount = ReadFile ( hFileHandle , pBuffer+iPrefixCount , iNumOfData );
	
	iSendCount = iNumOfData+iPrefixCount;
	
	CloseFile(hFileHandle);
	
	IF (( iCount != iNumOfData ) , "File corrupt" );  

	Ivi_GetAttributeViSession ( hInstrumentHandle , "", IVI_ATTR_IO_SESSION, 0, &hVisaSession );
		
	CHK_ERR( viWrite ( hVisaSession , pBuffer , iSendCount , &iCount ));
		
Error:	
	
	FREE(pBuffer);
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_GetStateName( int hInstrumentHandle , int iStateNumber , char *pszName ) 
{
	STD_ERROR					StdError					=	{0};
	
	
	char						szBuffer[LOW_STRING]  		=	{0},
								szGetBuffer[LOW_STRING]  	=	{0},
								*pTemp						=	NULL;  
	
	int							iCount						=	0;
	
	int							iNumOfDigits				=	0,
								iNumOfData					=	0;

	int							hFileHandle					=	0;
	
	sprintf( szBuffer , "NVNAME? %d\n" , iStateNumber );
	
	CHK_ERR( auml24xx_WriteInstrData ( hInstrumentHandle , szBuffer ));  
	
	DelayWithEventProcessing(0.2); 
	
	CHK_ERR( auml24xx_ReadInstrData ( hInstrumentHandle , strlen(szBuffer) , szGetBuffer , &iCount ));  
	
	CHK_ERR( auml24xx_ReadInstrData ( hInstrumentHandle , 32 , pszName , &iCount )); 
	
	pTemp = strchr( pszName , '\n' );
	
	if (pTemp) 
		*pTemp = 0;
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetStateName( int hInstrumentHandle , int iStateNumber , char *pszName ) 
{
	STD_ERROR					StdError					=	{0};
	
	
	char						szBuffer[LOW_STRING]  		=	{0},
								szGetBuffer[LOW_STRING]  	=	{0};  
	
	int							iCount						=	0;
	
	int							iNumOfDigits				=	0,
								iNumOfData					=	0;

	int							hFileHandle					=	0;
	
	char						*pStateName					=	NULL;
	
	pStateName = SkipWhiteSpace (pszName); 
	
	sprintf( szBuffer , "NVNAME %d,%s\n" , iStateNumber , pStateName );
	
	CHK_ERR( auml24xx_WriteInstrData ( hInstrumentHandle , szBuffer ));  
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetAuto( int hInstrumentHandle , int channel ) 
{
	STD_ERROR					StdError					=	{0};
	
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetChanelState( int hInstrumentHandle , int channel , int state )
{
	STD_ERROR					StdError					=	{0};
	
	
		
Error:	
	
	RETURN_STDERR_POINTER;  

}

void* DLLEXPORT PowerMeter_SetVerticalScale( int hInstrumentHandle , int channel , double scale )
{
	STD_ERROR					StdError					=	{0};
	
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetVerticalCenter( int hInstrumentHandle , int channel , double center )
{
	STD_ERROR					StdError					=	{0};
	
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Calibrate( int hInstrumentHandle , int iChannel )
{
	STD_ERROR					StdError					=	{0};
	
	char						szChannelName[LOW_STRING]   =	{0};

	sprintf( szChannelName , "%c" , ('A' + iChannel - 1 ));

	CHK_ERR( auml24xx_Calibrate ( hInstrumentHandle , szChannelName ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );  
	
	CHK_ERR( auml24xx_Zero ( hInstrumentHandle , szChannelName ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );  
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Zero( int hInstrumentHandle , int iChannel )
{
	STD_ERROR					StdError					=	{0};
	
	char						szChannelName[LOW_STRING]   =	{0};
	
	sprintf( szChannelName , "%d" , iChannel );
		
	CHK_ERR( auml24xx_Zero ( hInstrumentHandle , szChannelName ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );   
	
Error:	
	
	RETURN_STDERR_POINTER;  
}


void* DLLEXPORT PowerMeter_SetActiveChannel( int hInstrumentHandle , int iChannel )
{
	STD_ERROR					StdError					=	{0};
	
	char						szChannelName[LOW_STRING]   =	{0};
	
	sprintf( szChannelName , "CHACTIV %d\n" , iChannel );
		
	CHK_ERR( auml24xx_WriteInstrData ( hInstrumentHandle , szChannelName ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );   
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetActivePort( int hInstrumentHandle , int iSensorNumber )
{
	STD_ERROR					StdError					=	{0};
	
	char						szInputConfig[LOW_STRING]   =	{0};
	
	char						szGetBuffer[LOW_STRING]  	=	{0};  
	
	int							iCount						=	0;

	int							iChannel					=	0;
	
	CHK_ERR( auml24xx_WriteInstrData ( hInstrumentHandle , "CHACTIV?\n" ));  
	
	DelayWithEventProcessing(0.2); 
	
	CHK_ERR( auml24xx_ReadInstrData ( hInstrumentHandle , LOW_STRING , szGetBuffer , &iCount ));  
	
	sscanf( szGetBuffer , "CHACTIV %d" , &iChannel );
	
	sprintf( szInputConfig , "CHCFG %d,%c\n" , iChannel , ('A' + iSensorNumber - 1 ));
	
	CHK_ERR( auml24xx_WriteInstrData ( hInstrumentHandle , szInputConfig ));  
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );  
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_EnableDualPortMode( int hInstrumentHandle , int bEnable )
{
	STD_ERROR					StdError					=	{0};

	int							iNumberOfPorts				=	0;
	
	CHK_ERR( auml24xx_GetAttributeViInt32 ( hInstrumentHandle , "" , AUML24XX_ATTR_CHANNEL_COUNT , &iNumberOfPorts )); 
			
	if ( iNumberOfPorts > 1 )
	{
		CHK_ERR( auml24xx_ConfigureDualChannelEnabled( hInstrumentHandle, bEnable ));
	}
		
	Delay_LastComplete( hInstrumentHandle , 60.0 );  
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetUnits( int hInstrumentHandle , int channel , unsigned int units )
{
	STD_ERROR					StdError					=	{0};
	
	
Error:	
	
	RETURN_STDERR_POINTER;  	
}

void* DLLEXPORT PowerMeter_SetOffset( int hInstrumentHandle , int iChannel , double Offset )
{
	STD_ERROR					StdError					=	{0};
	
	char						szChannelName[LOW_STRING]   =	{0};

	sprintf( szChannelName , "%d" , iChannel );
		
	CHK_ERR(  auml24xx_ConfigureSensorOffset ( hInstrumentHandle , szChannelName , Offset ));
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetCalFactor( int hInstrumentHandle , int channel , double CalFactor )
{
	STD_ERROR					StdError					=	{0};
	
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetVideoBW( int hInstrumentHandle , int channel ,  unsigned int VideoBW )
{
	STD_ERROR					StdError					=	{0};
	
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetPeakHold( int hInstrumentHandle , int channel , int state )
{
	STD_ERROR					StdError					=	{0};   
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetFrequency( int hInstrumentHandle , int iChannel , double dFreq )
{
	STD_ERROR					StdError					=	{0};
	
	char						szChannelName[LOW_STRING]   =	{0},
								szCommand[LOW_STRING]  		=	{0},
								szBuffer[LOW_STRING]  		=	{0}; 
									
	int							iCount						=	0,
								iIndex						=	0;
								
	int							iActiveChannel				=	0;
	
	double						lfFrequency					=	0.0;
	
	ViSession   				handle						=	0;
	
	double						lfCurrentDelta				=	0.0,
								lfPreviousDelta				=	0.0;
	
	handle = Ivi_IOSession(hInstrumentHandle); 
	
	switch ( iChannel )
	{
		case 1:
				strcpy( szChannelName , "A" );
				break;
				
		case 2:
				strcpy( szChannelName , "B" );
				break;
			
		default:
			
				sprintf( szCommand , "CHACTIV?\n" );
	
				CHK_ERR( viWrite ( handle , szCommand , strlen(szCommand) , &iCount )); 
		
				CHK_ERR( viRead ( handle , szBuffer , LOW_STRING , &iCount ));   
				
				sscanf( szBuffer , "CHACTIV %d" , &iActiveChannel );
	
				sprintf( szCommand , "CHCFG? %d\n" , iActiveChannel );
	
				CHK_ERR( viWrite ( handle , szCommand , strlen(szCommand) , &iCount )); 
		
				CHK_ERR( viRead ( handle , szBuffer , LOW_STRING , &iCount ));   

				sscanf( szBuffer , "CHCFG %d,%s" , &iActiveChannel , szChannelName );   
				
				break; 	
	}

	for ( iIndex = 0; iIndex < 5; iIndex++ )
	{
		if (( dFreq / 1E9 ) < 1E3 )
			sprintf( szCommand , "SNCFRQ %s,%0.2lf GHz\n" , szChannelName , ( dFreq / 1E9 ) );
		
		if (( dFreq / 1E6 ) < 1E3 )
			sprintf( szCommand , "SNCFRQ %s,%0.2lf MHz\n" , szChannelName , ( dFreq / 1E6 ) );
	
		if (( dFreq / 1E3 ) < 1E3 )
			sprintf( szCommand , "SNCFRQ %s,%0.2lf kHz\n" , szChannelName , ( dFreq / 1E3 ) );
	
		if ( strlen(szCommand) == 0 )
			sprintf( szCommand , "SNCFRQ %s,%0.2lf Hz\n" , szChannelName , dFreq );

		CHK_ERR( viWrite ( handle , szCommand , strlen(szCommand) , &iCount )); 
	
		Delay_LastComplete( hInstrumentHandle , 60.0 );   
	
		sprintf( szCommand , "SNCFRQ? %s\n" , szChannelName );
	
		CHK_ERR( viWrite ( handle , szCommand , strlen(szCommand) , &iCount )); 
	
		CHK_ERR( viRead ( handle , szBuffer , LOW_STRING , &iCount ));
	
		sprintf( szCommand , "SNCFRQ %s,%%le" , szChannelName );  
	
		sscanf( szBuffer , szCommand , &lfFrequency );
	
		lfCurrentDelta = fabs( dFreq - lfFrequency );
			
		if ( lfPreviousDelta == lfCurrentDelta )
			break;
			
		lfPreviousDelta = lfCurrentDelta;	
	}
	
	IF (( lfPreviousDelta != lfCurrentDelta ) , "Can't set Power Meter Frequency." );
		
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetMode( int hInstrumentHandle , int iChannel , unsigned int Mode )
{
	STD_ERROR					StdError					=	{0};

	char						szChannelName[LOW_STRING]   =	{0},
								vModeList[][16]				=	{"CW","PMOD"};

	int							iNumberOfModes				=	0;
	
	
	iNumberOfModes = sizeof(vModeList) / 16;
	
	IF (( Mode >= iNumberOfModes ) , "Wrong channel number" );
	
	sprintf( szChannelName , "CHMODE %d,%s" , iChannel , vModeList[Mode] );
		
	CHK_ERR( auml24xx_WriteInstrData ( hInstrumentHandle , szChannelName ));

	Delay_LastComplete( hInstrumentHandle , 60.0 );   
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_OPC( int hInstrumentHandle , double lfTimeOut , int *state )
{
	STD_ERROR					StdError					=	{0};
	
	
	viSetAttribute ( hInstrumentHandle , VI_ATTR_TMO_VALUE , ( lfTimeOut * 1E3 ) );
		
	if ( state )
		*state = Delay_LastComplete( hInstrumentHandle , lfTimeOut );
		
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_FetchCWpower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};
	
	double			 			lfPower						=	0;
	
	char						szChannelName[LOW_STRING]   =	{0};

	sprintf( szChannelName , "%d" , iChannel );
	
	CHK_ERR(  auml24xx_FetchCWData ( hInstrumentHandle , szChannelName , &lfPower ));
	
	if ( value )
		*value = lfPower;
		
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_ReadCWpower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};
	
	double			 			lfPower						=	0;
	
	char						szChannelName[LOW_STRING]   =	{0};

	sprintf( szChannelName , "%d" , iChannel );
	
	CHK_ERR(  auml24xx_ReadCWData ( hInstrumentHandle , szChannelName , &lfPower , ( timeout * 1E3 )));
	
	if ( value )
		*value = lfPower;
		
Error:	
	
	RETURN_STDERR_POINTER;  
}
	
void* DLLEXPORT PowerMeter_ReadPower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};
	
	double			 			lfPower						=	0;
	
	char						szChannelName[LOW_STRING]   =	{0};
	
	sprintf( szChannelName , "CHACTIV %d" , iChannel );
		
	CHK_ERR( auml24xx_WriteInstrData ( hInstrumentHandle , szChannelName ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );   
	
	CHK_ERR( auml24xx_Read ( hInstrumentHandle , ( timeout * 1E3 ) , &lfPower ));
	
	if ( value )
		*value = lfPower;
		
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_FetchPower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};
	
	double			 			lfPower						=	0;
	
	char						szChannelName[LOW_STRING]   =	{0};
	
	sprintf( szChannelName , "CHACTIV %d" , iChannel );
		
	CHK_ERR( auml24xx_WriteInstrData ( hInstrumentHandle , szChannelName ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );   
	
	CHK_ERR(  auml24xx_Fetch ( hInstrumentHandle , &lfPower ));
	
	if ( value )
		*value = lfPower;
		
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_MeasurePower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};

	double			 			lfPower						=	0;
	
	char						szChannelName[LOW_STRING]   =	{0};
	
	sprintf( szChannelName , "CHACTIV %d" , iChannel );
		
	CHK_ERR( auml24xx_WriteInstrData ( hInstrumentHandle , szChannelName ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );   
		 
	CHK_ERR( auml24xx_Read ( hInstrumentHandle , ( timeout * 1E3 ) , &lfPower ));

	if ( value )
		*value = lfPower;
	
Error:	
	
	RETURN_STDERR_POINTER;  
}
  
void* DLLEXPORT PowerMeter_MeasureModulatedPower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};

	char						szChannelName[LOW_STRING]   =	{0};
	
	sprintf( szChannelName , "CHACTIV %d" , iChannel );
		
	CHK_ERR( auml24xx_WriteInstrData ( hInstrumentHandle , szChannelName ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );   
	
Error:	
	
	RETURN_STDERR_POINTER;      
}

void* DLLEXPORT PowerMeter_FetchMarkerAverage( int hInstrumentHandle , int iChannel , int marker , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};

	char						szChannelName[LOW_STRING]   =	{0};
	
	sprintf( szChannelName , "CHACTIV %d" , iChannel );
		
	CHK_ERR( auml24xx_WriteInstrData ( hInstrumentHandle , szChannelName ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );   
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_FetchMarkerMax( int hInstrumentHandle , int iChannel , int marker , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};

	char						szChannelName[LOW_STRING]   =	{0};
	
	sprintf( szChannelName , "CHACTIV %d" , iChannel );
		
	CHK_ERR( auml24xx_WriteInstrData ( hInstrumentHandle , szChannelName ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );   
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_FetchMarkerMin( int hInstrumentHandle , int iChannel , int marker , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};

	char						szChannelName[LOW_STRING]   =	{0};
	
	sprintf( szChannelName , "CHACTIV %d" , iChannel );
		
	CHK_ERR( auml24xx_WriteInstrData ( hInstrumentHandle , szChannelName ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );   
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Gating_FetchMaxPower( int hInstrumentHandle , int iChannel , double *value )
{
	STD_ERROR					StdError					=	{0};

	char						szChannelName[LOW_STRING]   =	{0};
	
	double						vlfMeasureList[128]			=	{0.0};

	int							iNumberOfPoints				=	0;
	
	sprintf( szChannelName , "%d" , iChannel );
	
	CHK_ERR( auml24xx_FetchGating ( hInstrumentHandle , szChannelName , 1, &iNumberOfPoints , vlfMeasureList )); 
	
	*value = vlfMeasureList[1];
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Configure_Marker( int hInstrumentHandle , int iChannel , int iMarkerNumber , int bEnable , double lfPosition )
{
	STD_ERROR					StdError					=	{0};

	char						szChannelName[LOW_STRING]   =	{0};
	
	sprintf( szChannelName , "%d" , iChannel );
	
	CHK_ERR( auml24xx_ConfigureMarkerNumber ( hInstrumentHandle , szChannelName , iMarkerNumber ));

	CHK_ERR( auml24xx_ConfigureMarker ( hInstrumentHandle , szChannelName , bEnable, lfPosition ));

Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Read_Marker( int hInstrumentHandle , int iChannel , int iMarkerNumber , double timeout , double *vlfPosition , double *vlfPower )
{
	STD_ERROR					StdError					=	{0};

	char						szChannelName[LOW_STRING]   =	{0};
	
	int							iNumberOfPoints				=	0;
	
	sprintf( szChannelName , "%d" , iChannel );
	
	CHK_ERR( auml24xx_ReadMarker ( hInstrumentHandle , szChannelName , iMarkerNumber , vlfPower , vlfPosition , (timeout*1E3) )); 

Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Fetch_Marker( int hInstrumentHandle , int iChannel , int iMarkerNumber , double timeout , double *vlfPosition , double *vlfPower )
{
	STD_ERROR					StdError					=	{0};

	char						szChannelName[LOW_STRING]   =	{0};
	
	int							iNumberOfPoints				=	0;
	
	sprintf( szChannelName , "%d" , iChannel );
	
	CHK_ERR( auml24xx_FetchMarker ( hInstrumentHandle , szChannelName , iMarkerNumber , vlfPower , vlfPosition )); 

Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Gating_ReadMaxPower( int hInstrumentHandle , int iChannel , int iGait , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};

	char						szChannelName[LOW_STRING]   =	{0};
	
	double						vlfMeasureList[128]			=	{0.0};

	int							iNumberOfPoints				=	0;
	
	sprintf( szChannelName , "%d" , iChannel );
	
	CHK_ERR( auml24xx_ReadGating ( hInstrumentHandle , szChannelName , iGait , &iNumberOfPoints , vlfMeasureList , ( timeout*1E3 ))); 
	
	*value = vlfMeasureList[1];
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Gating_ReadPower( int hInstrumentHandle , int iChannel , int iGait , int iGaitMeasureIndex , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};

	char						szChannelName[LOW_STRING]   =	{0};
	
	double						vlfMeasureList[128]			=	{0.0};

	int							iNumberOfPoints				=	0;
	
	sprintf( szChannelName , "%d" , iChannel );
	
	CHK_ERR( auml24xx_ReadGating ( hInstrumentHandle , szChannelName , iGait , &iNumberOfPoints , vlfMeasureList , ( timeout*1E3 ))); 
	
	if (( iGaitMeasureIndex >= 0 ) || ( iGaitMeasureIndex < 128 ))
		*value = vlfMeasureList[iGaitMeasureIndex];
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Gating_ConfigOffsetTime( int hInstrumentHandle , int iChannel , int iGate , double lfTime , double lfOffset , double lfMidambleOffset, double lfMidambleLength )
{
	STD_ERROR					StdError					=	{0};

	char						szChannelName[LOW_STRING]   =	{0};

	sprintf( szChannelName , "%d" , iChannel );
	
	CHK_ERR( auml24xx_SetAttributeViReal64 ( hInstrumentHandle , szChannelName , AUML24XX_ATTR_TRIGGER_DELAY_TIME , lfOffset ));
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

int		Delay_LastComplete( int handle , double lfTimeout )
{
	double		lfCurrentTime			=	0.0,
				lfStartTime				=	0.0;
	
	int			iOpcValue				=	0;
	
	int			status					=	0;
	
	int			iCount					=	0;
	
	char		szBuffer[STD_STRING]	=	{0};
	
	GetCurrentDateTime ( &lfStartTime );
	GetCurrentDateTime ( &lfCurrentTime ); 
	
	while(( lfCurrentTime - lfStartTime ) < lfTimeout ) 
	{
		
		GetCurrentDateTime ( &lfCurrentTime );  
		
		status = auml24xx_WriteInstrData ( handle , "*OPC?\n");
	
		if ( status )
			continue;
		
		Delay ( 0.2 );
		
		status = auml24xx_ReadInstrData ( handle , STD_STRING , szBuffer , &iCount);

		if ( iCount )
			iOpcValue = atoi(szBuffer);
			
		if ( status )
			continue;
		
		if ( iOpcValue == 1 )
			return 0;
	}
	
	return -1;	
}

int main(void)
{
	int					InstrumentHandle			=	0,
						InstrumentConnectStatus		=	0,
						InstrumentChannelsQuantity	=	0;
		
	double				value						=	0.0,
						position					=	0.0;  
							
	char				szName[LOW_STRING]			=	{0};
	
	int					error						=	0;
	
	PowerMeter_Init( 0 , "GPIB::15" , &InstrumentHandle , &InstrumentConnectStatus , &InstrumentChannelsQuantity );
	
	PowerMeter_SetFrequency( InstrumentHandle , 0 , 100E6 ); 
	PowerMeter_SetFrequency( InstrumentHandle , 0 , 200E6 ); 
	PowerMeter_SetFrequency( InstrumentHandle , 0 , 300E6 ); 
	
	PowerMeter_InitSweep( InstrumentHandle , 1 ); 
	
	PowerMeter_SetFrequency( InstrumentHandle , 2 , 3400E6 ); 
	
	PowerMeter_FetchCWpower ( InstrumentHandle , 1 , 10.0 , &value );  
						
	PowerMeter_SetFrequency( InstrumentHandle , 2 , 500E6 );   
	PowerMeter_SetFrequency( InstrumentHandle , 2 , 500E6 ); 
	PowerMeter_SetFrequency( InstrumentHandle , 2 , 500E6 ); 
	PowerMeter_SetFrequency( InstrumentHandle , 2 , 500E6 ); 
	
	PowerMeter_InitSweep( InstrumentHandle , 1 ); 
	PowerMeter_FetchCWpower ( InstrumentHandle , 1 , 10.0 , &value );  

	PowerMeter_SetFrequency( InstrumentHandle , 2 , 800E6 );
	
	PowerMeter_InitSweep( InstrumentHandle , 1 ); 
	PowerMeter_FetchCWpower ( InstrumentHandle , 1 , 10.0 , &value );  

	PowerMeter_SetFrequency( InstrumentHandle , 2 , 1E9 );  
	
	PowerMeter_InitSweep( InstrumentHandle , 1 ); 
	PowerMeter_FetchCWpower ( InstrumentHandle , 1 , 10.0 , &value );  

	PowerMeter_SetFrequency( InstrumentHandle , 2 , 10E9 ); 
	
	PowerMeter_InitSweep( InstrumentHandle , 1 ); 
	PowerMeter_FetchCWpower ( InstrumentHandle , 1 , 10.0 , &value );  

	PowerMeter_SetFrequency( InstrumentHandle , 2 , 35E9 );
	
	PowerMeter_InitSweep( InstrumentHandle , 1 ); 
	PowerMeter_ReadCWpower ( InstrumentHandle , 1 , 10.0 , &value );  

		
	PowerMeter_Reset( InstrumentHandle );
	
	PowerMeter_SetMode( InstrumentHandle , 1 , 0 ); 
	
	PowerMeter_SetActivePort( InstrumentHandle , 1 ); 
	PowerMeter_SetActivePort( InstrumentHandle , 2 ); 
	PowerMeter_SetActivePort( InstrumentHandle , 1 );
	
	PowerMeter_EnableDualPortMode( InstrumentHandle , 1 ); 
	
	PowerMeter_SetActivePort( InstrumentHandle , 1 ); 
	PowerMeter_SetActivePort( InstrumentHandle , 2 );
	PowerMeter_SetActivePort( InstrumentHandle , 1 );
	
	
		
	PowerMeter_SetMode( InstrumentHandle , 1 , 0 );
	PowerMeter_SetMode( InstrumentHandle , 2 , 0 );
	PowerMeter_SetMode( InstrumentHandle , 1 , 1 );
	PowerMeter_SetMode( InstrumentHandle , 2 , 1 );
	
	PowerMeter_SetActiveChannel( InstrumentHandle , 1 );
	
		 
	//PowerMeter_Calibrate( InstrumentHandle , 2 );   
	//PowerMeter_Calibrate( InstrumentHandle , 1 );   
	
	PowerMeter_SetActivePort( InstrumentHandle , 1 );	
	PowerMeter_SetActivePort( InstrumentHandle , 2 );

	PowerMeter_Calibrate( InstrumentHandle , 1 );   
		
	//PowerMeter_Read_Marker( InstrumentHandle , 1 , 20 , &position , &value );                           
		
	PowerMeter_Reset( InstrumentHandle );
	
	PowerMeter_RecallState_ByName( InstrumentHandle , "macs-power" ); 
	
	PowerMeter_SetFrequency( InstrumentHandle , 0 , 34.1E9 );  
	
	PowerMeter_Gating_ConfigOffsetTime( InstrumentHandle , 1 , 1 , 0.0 , 376.88E-9 , 0.0 , 0.0 );  
	
   PowerMeter_FetchPower( InstrumentHandle , 1 , 10.0 , &value ); 
   PowerMeter_FetchPower( InstrumentHandle , 1 , 10.0 , &value );
   PowerMeter_FetchPower( InstrumentHandle , 1 , 10.0 , &value );
   PowerMeter_FetchPower( InstrumentHandle , 1 , 10.0 , &value );
   PowerMeter_FetchPower( InstrumentHandle , 1 , 10.0 , &value );
   PowerMeter_FetchPower( InstrumentHandle , 1 , 10.0 , &value );
   PowerMeter_FetchPower( InstrumentHandle , 1 , 10.0 , &value );
   PowerMeter_ReadPower( InstrumentHandle , 1 , 10.0 , &value );
   PowerMeter_ReadPower( InstrumentHandle , 1 , 10.0 , &value );
   PowerMeter_ReadPower( InstrumentHandle , 1 , 10.0 , &value );
   PowerMeter_ReadPower( InstrumentHandle , 1 , 10.0 , &value );
   PowerMeter_ReadPower( InstrumentHandle , 1 , 10.0 , &value );
   PowerMeter_ReadPower( InstrumentHandle , 1 , 10.0 , &value );
   PowerMeter_ReadPower( InstrumentHandle , 1 , 10.0 , &value );
   PowerMeter_ReadPower( InstrumentHandle , 1 , 10.0 , &value );
   PowerMeter_ReadPower( InstrumentHandle , 1 , 10.0 , &value );
   PowerMeter_ReadPower( InstrumentHandle , 1 , 10.0 , &value );
	
	PowerMeter_SetActiveChannel( InstrumentHandle , 2 );
		
	PowerMeter_FetchCWpower( InstrumentHandle , 2 , 10.0 , &value ); 
	
	
	//PowerMeter_Reset( InstrumentHandle ); 
		
	//PowerMeter_RecallState( InstrumentHandle , 10 ); 
	
	PowerMeter_RecallState_ByName( InstrumentHandle , "iff_tester" );   
		
	PowerMeter_Gating_ConfigOffsetTime( InstrumentHandle , 1 , 1 , 0.0 , 20E-6 , 0.0 , 0.0 ); 
		
	PowerMeter_Gating_ReadMaxPower( InstrumentHandle , 1 , 1 , 20 , &value);
	
	//PowerMeter_FetchCWpower( InstrumentHandle , 1 , 0.0 , &value ); 
	//PowerMeter_ReadPower( InstrumentHandle , 1 , 10.0 , &value );
	
	//PowerMeter_GetStateFile( InstrumentHandle , 10 , "d:\\iff_ate_tester_pm.state" ); 
	//PowerMeter_SetStateFile( InstrumentHandle , 1 , "d:\\iff_ate_tester_pm.state" ); 
	//PowerMeter_RecallState( InstrumentHandle , 1 ); 
	
	//PowerMeter_GetStateName( InstrumentHandle , 10 , szName ); 
	//PowerMeter_SetStateName( InstrumentHandle , 10 , "iff_tester" ); 
	
	//PowerMeter_Reset( InstrumentHandle ); 
	
	PowerMeter_SetFrequency( InstrumentHandle , 0 , 1.3E9 );
		
	PowerMeter_ReadPower( InstrumentHandle , 0 , 10.0 , &value ); 
	

	/*PowerMeter_FetchCWpower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_FetchCWpower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_FetchCWpower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_FetchCWpower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_FetchCWpower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_FetchCWpower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_FetchCWpower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_FetchCWpower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_FetchCWpower( InstrumentHandle , 0 , 0.0 , &value ); 
	
	PowerMeter_SetContinuesSweep( InstrumentHandle , 0 ); 
		
	PowerMeter_MeasurePower( InstrumentHandle , 0 , 0.0 , &value );  
	
	PowerMeter_SetContinuesSweep( InstrumentHandle , 0 );
	
	PowerMeter_MeasurePower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_MeasurePower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_MeasurePower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_MeasurePower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_MeasurePower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_MeasurePower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_MeasurePower( InstrumentHandle , 0 , 0.0 , &value ); 
	
	PowerMeter_FetchCWpower( InstrumentHandle , 0 , 0.0 , &value );
	*/	
	PowerMeter_Close( &InstrumentHandle );
	
	return 0;
}

