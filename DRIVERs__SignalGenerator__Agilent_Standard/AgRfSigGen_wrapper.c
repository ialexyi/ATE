#include <Ivi.h>
#include <userint.h>
#include "AgRfSigGen.h"
#include <ansi_c.h>
#include "GlobalDefines.h"
#include "toolbox.h"
#include <utility.h>

//==============================================================================
// Include files

//#include "AgRfSigGen_wrapper.h"

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

/***** ***** ***** ***** ***** ***** Get Last Error Text Message ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT SignalGenerator_GetLastErrorTextMessage ( int InstrumentHandle , char **pErrorMessage )
{
	STD_ERROR				StdError					=	{0};    

	char					szErrorDesc[1024]			=	{0};
	
	AgRfSigGen_error_query ( InstrumentHandle , &(StdError.error), szErrorDesc );
	
	AgRfSigGen_ClearError ( InstrumentHandle );  
	
	if ( IS_NOT_OK && ( StdError.error != -113 ))
	{
		FREE_FILL_ERROR_POSITION;
		
		CALLOC_COPY_STRING( StdError.pszErrorDescription , szErrorDesc );		
	}
	else
	{
		StdError.error = 0;
	}
	
Error:  
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT SignalGenerator_GetErrorTextMessage ( ViSession viInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR				StdError					=	{0};    

	iError = AgRfSigGen_error_message	( viInstrumentHandle , iError , pErrorMessage );
	
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
		IF (( strstr( szIdentificationText , "Agilent Technologies" ) == NULL ) , "Is not supported" );
	
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
	
	char							szIdentificationText[LOW_STRING]			=	{0},
									vszSupportdedModels[][12]					=	{{"E4428C"},{"E4438C"},{"N5181A"},{"N5182A"},{"N5183A"},{"N5171B"},{"N5181B"},{"N5172B"},{"N5173B"},{"N5182B"},{"E8241A"},{"E8244A"},{"E8251A"},{"E8254A"},{"E8247C"},{"E8257C"},{"E8267C"},{"E8257D"},{"E8267D"},{"E8663B"},{"E8257N"}};
									
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
		if ( strstr( szIdentificationText , "Agilent Technologies" ) == NULL ) 
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

/***** ***** ***** ***** ***** ***** ***** ***** Init ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT SignalGenerator_Init( int hParentInstrumentHandle , char* szRsrcAddress , int *phInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChannelsQuantity )
{	
	STD_ERROR				StdError					=	{0};    
						
	int						iChannelCount				=	1;
	
	char					szErrorTextMessage[256]		=	{0};
	
	ViSession				handle						=	0;
	
	if ( hParentInstrumentHandle == 0 )
	{
		STD_ERR ( AgRfSigGen_init ( szRsrcAddress, VI_FALSE, VI_FALSE, &handle));
	}
	else
		handle = hParentInstrumentHandle;
		
	if ( phInstrumentHandle  ) 
			*phInstrumentHandle = handle;
	
	STD_ERR ( AgRfSigGen_ClearError (handle));
	
	if ( InstrumentChannelsQuantity )
		*InstrumentChannelsQuantity = iChannelCount;

Error:
	
	if (( IS_NOT_OK ) || (iChannelCount < 1))
	{ 
		AgRfSigGen_error_message (handle, StdError.error , szErrorTextMessage);
		
		//MessagePopup("Signal Generator Initializing Error!",szErrorTextMessage);
		
		AgRfSigGen_close (handle);
		*phInstrumentHandle = 0;
	}
	else
		if ( InstrumentConnectStatus )
			*InstrumentConnectStatus = 1;	
	
	RETURN_STDERR_POINTER;	
}


void* DLLEXPORT SignalGenerator_Close( int *pHandle )
{
	STD_ERROR				StdError					=	{0};    
	
	ViSession				handle						=	0;

	if ( pHandle == NULL )
		return NULL;
	
	handle = *pHandle;
	
	if ( handle == 0 )
		return 0;
	
	StdError.error = AgRfSigGen_close ( handle );
		
	*pHandle = 0;
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT SignalGenerator_Reset ( ViSession InstrumentHandle )   
{   
	STD_ERROR				StdError					=	{0};    

	STD_ERR( AgRfSigGen_reset ( InstrumentHandle ));
	
	STD_ERR( AgRfSigGen_AnalogModulationDisableAll ( InstrumentHandle ));
	
Error:
	
	return SignalGenerator_GetLastErrorTextMessage(InstrumentHandle,NULL);
}


void* DLLEXPORT SignalGenerator_RF_Off ( int InstrumentHandle , int channel )
{
	STD_ERROR				StdError					=	{0};    

	STD_ERR( AgRfSigGen_ConfigureOutputEnabled ( InstrumentHandle , VI_FALSE ));
	
	STD_ERR( AgRfSigGen_WaitUntilSettled ( InstrumentHandle , 10000 ));

Error:
	
	return SignalGenerator_GetLastErrorTextMessage(InstrumentHandle,NULL);
}

void* DLLEXPORT SignalGenerator_RF_On ( int InstrumentHandle , int channel )
{
	STD_ERROR				StdError					=	{0};    

	STD_ERR( AgRfSigGen_ConfigureOutputEnabled ( InstrumentHandle , VI_TRUE));
	
	STD_ERR( AgRfSigGen_WaitUntilSettled ( InstrumentHandle , 10000 ));

Error:
	
	return SignalGenerator_GetLastErrorTextMessage(InstrumentHandle,NULL);
}

void* DLLEXPORT SignalGenerator_Config ( int InstrumentHandle , int channel , double lfFrequency , double lfAmplitude )
{
	STD_ERROR				StdError					=	{0};    

	STD_ERR( AgRfSigGen_ConfigureRF ( InstrumentHandle , lfFrequency , lfAmplitude ));
	
	STD_ERR( AgRfSigGen_WaitUntilSettled ( InstrumentHandle , 10000 ));

Error:
	
	return SignalGenerator_GetLastErrorTextMessage(InstrumentHandle,NULL);
}

void* DLLEXPORT SignalGenerator_SetFrequency ( int InstrumentHandle , int channel , double lfFrequency )
{
	STD_ERROR				StdError					=	{0};    

	STD_ERR( AgRfSigGen_SetAttributeViReal64 ( InstrumentHandle , VI_NULL , AGRFSIGGEN_ATTR_FREQUENCY , lfFrequency ));
	
	STD_ERR( AgRfSigGen_WaitUntilSettled ( InstrumentHandle , 10000 ));

Error:
	
	return SignalGenerator_GetLastErrorTextMessage(InstrumentHandle,NULL);
}


void* DLLEXPORT SignalGenerator_SetPower ( int InstrumentHandle , int channel , double lfAmplitude )
{
	STD_ERROR				StdError					=	{0};    

	STD_ERR( AgRfSigGen_SetAttributeViReal64 ( InstrumentHandle , VI_NULL , AGRFSIGGEN_ATTR_POWER_LEVEL , lfAmplitude ));
	
	STD_ERR( AgRfSigGen_WaitUntilSettled ( InstrumentHandle , 10000 ));

Error:
	
	return SignalGenerator_GetLastErrorTextMessage(InstrumentHandle,NULL);
}

void* DLLEXPORT SignalGenerator_SetModulation ( int InstrumentHandle , int channel , int source , int iPulseMode , double dWidth , double dPeriod )
{
	STD_ERROR				StdError					=	{0};    

	return SignalGenerator_GetLastErrorTextMessage(InstrumentHandle,NULL);
}

void* DLLEXPORT SignalGenerator_SetModulationState ( int InstrumentHandle , int channel , int iState )
{
	STD_ERROR				StdError					=	{0};    

	return SignalGenerator_GetLastErrorTextMessage(InstrumentHandle,NULL);
}

void* DLLEXPORT SignalGenerator_RecallState ( int InstrumentHandle , char *pszStateFile )
{
	STD_ERROR				StdError					=	{0};    

	STD_ERR( AgRfSigGen_SystemRecallState ( InstrumentHandle , pszStateFile ));
	
Error:
	
	return SignalGenerator_GetLastErrorTextMessage(InstrumentHandle,NULL);
}

void main ( void)
{
	int							hInstrumentHandle									=	0,
								InstrumentConnectStatus								=	0,
								InstrumentChannelsQuantity							=	0;
		
	int							status												=	0;
	
	SignalGenerator_Init( 0 , "visa://192.168.1.112/GPIB::1" , &hInstrumentHandle , &InstrumentConnectStatus , &InstrumentChannelsQuantity );
	  
	SignalGenerator_RF_On ( hInstrumentHandle , 0 );    
	
	//SignalGenerator_Reset ( hInstrumentHandle );
	
	SignalGenerator_Close ( &hInstrumentHandle ); 
	
	return;
}


