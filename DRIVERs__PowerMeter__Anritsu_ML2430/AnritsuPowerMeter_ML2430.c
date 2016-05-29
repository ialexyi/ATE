//==============================================================================
// Include files

#include <windows.h>
#include "toolbox.h"
#include "ML2430.h"
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
	
	char						ExeErr[9]									=	{0},
								ComErr[9]									=	{0};
	
	if ( pErrorMessage ) 
	{
		ML2430_ErrorListing( handle , NULL , NULL , NULL , NULL , &iError, ExeErr , ComErr );
		
		CALLOC_COPY_ERR( *pErrorMessage , 8 , sizeof(char) , ExeErr , 8 );   
		
	}
	
Error:	
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT PowerMeter_GetErrorTextMessage( int handle , int iError , char *pErrorMessage )
{
	STD_ERROR					StdError									=	{0};
	
	char						ExeErr[9]									=	{0},
								ComErr[9]									=	{0};
	
	if ( pErrorMessage ) 
	{
		ML2430_ErrorListing( handle , NULL , NULL , NULL , NULL , &iError, ExeErr , ComErr );
		
		strcpy( pErrorMessage , ExeErr );   
		
	}
	
Error:	
	
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
	
	char							vszSupportdedModels[][32]					=	{{"ML2437A"},{"ML2438A"}};
									
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
	STD_ERROR					StdError									=	{0};
	
	int							hConnectionHandle							=	0;

	char						szIdentificationText[LOW_STRING]			=	{0};

	int							status										=	0;
	
	int							iCount										=	0;
	
	IF(( szRsrcAddress == NULL ) , "Empty Instrument driver"  );
	IF(( InstrumentConnectStatus == NULL ) , "Passed null pointer"  );
	IF(( InstrumentChannelsQuantity == NULL ) , "Passed null pointer"  );
	
	*InstrumentChannelsQuantity = 0;
	*InstrumentConnectStatus = 0;
	
	if ( hParentInstrumentHandle == 0 )
	{
		StdError.error = ML2430_Initialize ( szRsrcAddress , VI_OFF , pInstrumentHandle );
		
		if ( StdError.error == 0 )
		{
			hConnectionHandle = *pInstrumentHandle;
			
			status = viPrintf ( hConnectionHandle , "*IDN?\n" );
		
			if ( status == 0 )
			{
				viRead ( hConnectionHandle , szIdentificationText , (LOW_STRING-1) , &iCount );
				RemoveSurroundingWhiteSpace (szIdentificationText); 
				
				if ( strstr( szIdentificationText , "ML2437A" ) == NULL ) 
					*InstrumentConnectStatus = 1;
				
				if ( strstr( szIdentificationText , "ML2438A" ) == NULL ) 
					*InstrumentConnectStatus = 2;
			}
			
			Delay_LastComplete( *pInstrumentHandle , 10.0 ); 
			
			
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
						  
	ML2430_Close (*phInstrumentHandle);
	
	*phInstrumentHandle=0;
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Reset( int hInstrumentHandle )
{	
	STD_ERROR					StdError					=	{0};
	
	
	StdError.error = ML2430_Reset ( hInstrumentHandle );
	
	Delay_LastComplete( hInstrumentHandle , 10.0 );
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Configuration( int hInstrumentHandle , int iChannel , int bContinue , int bDelayAutoState , int iTriggerSource , double lfTrigHoldOff , double lfGateOffsetTime , double lfGateTime , double lfTraceTime )
{
	STD_ERROR					StdError					=	{0};
	
	char						szChannelName[LOW_STRING]   =	{0};

	/* 
	sprintf( szChannelName , "CHACTIV %d\n" , iChannel );
		
	CHK_ERR( ML2430_WriteInstrData ( hInstrumentHandle , szChannelName ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );   
		
	CHK_ERR( ML2430_ConfigureActiveChannel( hInstrumentHandle , szChannelName ));

	CHK_ERR( ML2430_Initiate ( hInstrumentHandle ));
	
	CHK_ERR( ML2430_configureTrigger ( hInstrumentHandle , iChannel , bDelayAutoState , iTriggerSource ));
	
	CHK_ERR( ML2430_configureChannelATrigger ( hInstrumentHandle , 0.000 , lfTrigHoldOff , 0.00 , 0.0 , ML2430_STATE_ON, ML2430_SLOPE_POS));
																			 
	CHK_ERR( ML2430_e9320ConfigureGate ( hInstrumentHandle , iChannel , 1 , lfGateOffsetTime , lfGateTime ));
	
	CHK_ERR( ML2430_e9320ConfigureTrace ( hInstrumentHandle , iChannel , VI_TRUE , 0.0 , lfTraceTime , -50.0 , 20.0 , ML2430_UNIT_DBM));
	*/
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetSingleSweep( int hInstrumentHandle , int iChannel )
{
	STD_ERROR					StdError					=	{0};
	
	char						szChannelName[LOW_STRING]   =	{0};
	
	CHK_ERR( ML2430_SetTriggerSource( hInstrumentHandle , iChannel , Profile_MANUAL ));
	
	Delay_LastComplete( hInstrumentHandle , 10.0 );  
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetContinuesSweep( int hInstrumentHandle , int iChannel ) 
{
	STD_ERROR					StdError					=	{0};

	CHK_ERR( ML2430_SetTriggerSource( hInstrumentHandle , iChannel , Profile_CONT ));
	
	Delay_LastComplete( hInstrumentHandle , 10.0 );  
	
Error:	
	
	RETURN_STDERR_POINTER;  
}


void* DLLEXPORT PowerMeter_InitSweep( int hInstrumentHandle , int iChannel ) 
{
	STD_ERROR					StdError					=	{0};

	CHK_VSA( viPrintf ( hInstrumentHandle , "TR1\n" )); 

	Delay_LastComplete( hInstrumentHandle , 60.0 );     
	
Error:	
	
	RETURN_STDERR_POINTER;  
}


void* DLLEXPORT PowerMeter_RecallState( int hInstrumentHandle , int iStateNumber ) 
{
	STD_ERROR					StdError					=	{0};
	
	char						szBuffer[LOW_STRING]  		=	{0};  
	
	sprintf( szBuffer , "*RCL %d\n" , iStateNumber );
	
	CHK_VSA( viPrintf ( hInstrumentHandle , szBuffer ));  
	
	Delay_LastComplete( hInstrumentHandle , 20.0 );  
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_RecallState_ByName( int hInstrumentHandle , char *pszName ) 
{
	STD_ERROR					StdError					=	{0};
	
	char						szBuffer[STD_STRING+1]  	=	{0};
	
	int							iStateNumber				=	0;
	
	Delay_LastComplete( hInstrumentHandle , 20.0 ); 
	
	if ( pszName )
		iStateNumber = atoi(pszName);
	
	sprintf( szBuffer , "*RCL %d\n" , iStateNumber );
	
	CHK_VSA( viPrintf ( hInstrumentHandle , szBuffer ));  
	
	Delay_LastComplete( hInstrumentHandle , 20.0 );  
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SaveState( int hInstrumentHandle , int iStateNumber ) 
{
	STD_ERROR					StdError					=	{0};
	
	char						szBuffer[LOW_STRING]  		=	{0};
	
	sprintf( szBuffer , "*SAV %d\n" , iStateNumber );
	
	CHK_VSA( viPrintf ( hInstrumentHandle , szBuffer ));  

	Delay_LastComplete( hInstrumentHandle , 10.0 );  
	
Error:	
	
	RETURN_STDERR_POINTER;  
}


void* DLLEXPORT PowerMeter_GetStateFile( int hInstrumentHandle , int iStateNumber , char *pszFileName ) 
{
	STD_ERROR					StdError					=	{0};
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetStateFile( int hInstrumentHandle , int iStateNumber , char *pszFileName ) 
{
	STD_ERROR					StdError					=	{0};
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_GetStateName( int hInstrumentHandle , int iStateNumber , char *pszName ) 
{
	STD_ERROR					StdError					=	{0};

Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetStateName( int hInstrumentHandle , int iStateNumber , char *pszName ) 
{
	STD_ERROR					StdError					=	{0};

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

	CHK_ERR( ML2430_CalibrateSensor ( hInstrumentHandle , (iChannel-1) ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );  
	
	CHK_ERR( ML2430_ZeroSensor ( hInstrumentHandle , (iChannel-1) ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );  
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Zero( int hInstrumentHandle , int iChannel )
{
	STD_ERROR					StdError					=	{0};
		
	CHK_ERR( ML2430_ZeroSensor ( hInstrumentHandle , (iChannel-1) ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );   
	
Error:	
	
	RETURN_STDERR_POINTER;  
}


void* DLLEXPORT PowerMeter_SetActiveChannel( int hInstrumentHandle , int iChannel )
{
	STD_ERROR					StdError					=	{0};
	
	char						szChannelName[LOW_STRING]   =	{0};
	
	ML2430_SetProfileChannelSelection( hInstrumentHandle , (iChannel-1));  
	
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
	
	CHK_ERR( ML2430_WriteInstrData ( hInstrumentHandle , "CHACTIV?\n" ));  
	
	DelayWithEventProcessing(0.2); 
	
	CHK_ERR( ML2430_ReadInstrData ( hInstrumentHandle , LOW_STRING , szGetBuffer , &iCount ));  
	
	sscanf( szGetBuffer , "CHACTIV %d" , &iChannel );
	
	sprintf( szInputConfig , "CHCFG %d,%c\n" , iChannel , ('A' + iSensorNumber - 1 ));
	
	CHK_ERR( ML2430_WriteInstrData ( hInstrumentHandle , szInputConfig ));  
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );  
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_EnableDualPortMode( int hInstrumentHandle , int bEnable )
{
	STD_ERROR					StdError					=	{0};

	int							iNumberOfPorts				=	0;
	
	CHK_ERR( ML2430_GetAttributeViInt32 ( hInstrumentHandle , "" , ML2430_ATTR_CHANNEL_COUNT , &iNumberOfPorts )); 
			
	if ( iNumberOfPorts > 1 )
	{
		CHK_ERR( ML2430_ConfigureDualChannelEnabled( hInstrumentHandle, bEnable ));
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
		
	CHK_ERR( ML2430_ConfigureSensorOffset ( hInstrumentHandle , szChannelName , Offset ));
	
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
	
	double						lfFrequency					=	0.0;
	
	ViSession   				handle						=	0;
	
	CHK_ERR( ML2430_SetCalFrequency( hInstrumentHandle , iChannel , 0 , dFreq ));

	Delay_LastComplete( hInstrumentHandle , 60.0 );   

	CHK_ERR( ML2430_GetCalFrequency( hInstrumentHandle , iChannel , NULL , NULL , &lfFrequency )
	
	IF (( lfFrequency != dFreq ) , "Can't set Power Meter Frequency." );
		
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
		
	CHK_ERR( ML2430_WriteInstrData ( hInstrumentHandle , szChannelName ));

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
	
	CHK_ERR(  ML2430_FetchCWData ( hInstrumentHandle , szChannelName , &lfPower ));
	
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
	
	CHK_ERR(  ML2430_ReadCWData ( hInstrumentHandle , szChannelName , &lfPower , ( timeout * 1E3 )));
	
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
		
	CHK_ERR( ML2430_WriteInstrData ( hInstrumentHandle , szChannelName ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );   
	
	CHK_ERR( ML2430_Read ( hInstrumentHandle , ( timeout * 1E3 ) , &lfPower ));
	
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
		
	CHK_ERR( ML2430_WriteInstrData ( hInstrumentHandle , szChannelName ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );   
	
	CHK_ERR(  ML2430_Fetch ( hInstrumentHandle , &lfPower ));
	
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
		
	CHK_ERR( ML2430_WriteInstrData ( hInstrumentHandle , szChannelName ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );   
		 
	CHK_ERR( ML2430_Read ( hInstrumentHandle , ( timeout * 1E3 ) , &lfPower ));

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
		
	CHK_ERR( ML2430_WriteInstrData ( hInstrumentHandle , szChannelName ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );   
	
Error:	
	
	RETURN_STDERR_POINTER;      
}

void* DLLEXPORT PowerMeter_FetchMarkerAverage( int hInstrumentHandle , int iChannel , int marker , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};

	char						szChannelName[LOW_STRING]   =	{0};
	
	sprintf( szChannelName , "CHACTIV %d" , iChannel );
		
	CHK_ERR( ML2430_WriteInstrData ( hInstrumentHandle , szChannelName ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );   
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_FetchMarkerMax( int hInstrumentHandle , int iChannel , int marker , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};

	char						szChannelName[LOW_STRING]   =	{0};
	
	sprintf( szChannelName , "CHACTIV %d" , iChannel );
		
	CHK_ERR( ML2430_WriteInstrData ( hInstrumentHandle , szChannelName ));
	
	Delay_LastComplete( hInstrumentHandle , 60.0 );   
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_FetchMarkerMin( int hInstrumentHandle , int iChannel , int marker , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};

	char						szChannelName[LOW_STRING]   =	{0};
	
	sprintf( szChannelName , "CHACTIV %d" , iChannel );
		
	CHK_ERR( ML2430_WriteInstrData ( hInstrumentHandle , szChannelName ));
	
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
	
	CHK_ERR( ML2430_FetchGating ( hInstrumentHandle , szChannelName , 1, &iNumberOfPoints , vlfMeasureList )); 
	
	*value = vlfMeasureList[1];
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Configure_Marker( int hInstrumentHandle , int iChannel , int iMarkerNumber , int bEnable , double lfPosition )
{
	STD_ERROR					StdError					=	{0};

	char						szChannelName[LOW_STRING]   =	{0};
	
	sprintf( szChannelName , "%d" , iChannel );
	
	CHK_ERR( ML2430_ConfigureMarkerNumber ( hInstrumentHandle , szChannelName , iMarkerNumber ));

	CHK_ERR( ML2430_ConfigureMarker ( hInstrumentHandle , szChannelName , bEnable, lfPosition ));

Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Read_Marker( int hInstrumentHandle , int iChannel , int iMarkerNumber , double timeout , double *vlfPosition , double *vlfPower )
{
	STD_ERROR					StdError					=	{0};

	char						szChannelName[LOW_STRING]   =	{0};
	
	int							iNumberOfPoints				=	0;
	
	sprintf( szChannelName , "%d" , iChannel );
	
	CHK_ERR( ML2430_ReadMarker ( hInstrumentHandle , szChannelName , iMarkerNumber , vlfPower , vlfPosition , (timeout*1E3) )); 

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
	
	CHK_ERR( ML2430_ReadGating ( hInstrumentHandle , szChannelName , iGait , &iNumberOfPoints , vlfMeasureList , ( timeout*1E3 ))); 
	
	*value = vlfMeasureList[1];
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Gating_ConfigOffsetTime( int hInstrumentHandle , int iChannel , int iGate , double lfTime , double lfOffset , double lfMidambleOffset, double lfMidambleLength )
{
	STD_ERROR					StdError					=	{0};

	char						szChannelName[LOW_STRING]   =	{0};

	sprintf( szChannelName , "%d" , iChannel );
	
	CHK_ERR( ML2430_SetAttributeViReal64 ( hInstrumentHandle , szChannelName , ML2430_ATTR_TRIGGER_DELAY_TIME , lfOffset ));
	
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
		
		status = ML2430_WriteInstrData ( handle , "*OPC?\n");
	
		if ( status )
			continue;
		
		Delay ( 0.2 );
		
		status = ML2430_ReadInstrData ( handle , STD_STRING , szBuffer , &iCount);

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

