//==============================================================================
// Include files

//#include "aumg369xb_wrapper.h"
#include "GlobalDefines.h"
#include "toolbox.h"
#include <visa.h>
#include "aumg369xb.h"
#include <userint.h>
#include <utility.h>
#include "Anritsu_SignalGenerator_MG369X.h"  

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

void* DLLEXPORT SignalGenerator_GetLastErrorTextMessage ( int hInstrumentHandle , char **pErrorMessage )
{
	STD_ERROR				StdError					=	{0};    

	//aumg369xb_GetError( pErrorMessage);
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SignalGenerator_GetErrorTextMessage ( int hInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR				StdError					=	{0};    

	iError = aumg369xb_errorMessage	( hInstrumentHandle , iError , pErrorMessage );
	
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
		IF (( strstr( szIdentificationText , "ANRITSU," ) == NULL ) , "Is not supported" );
	
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
									vszSupportdedModels[][12]					=	{{"MG3691B"},{"MG3692B"},{"MG3693B"},{"MG3694B"},{"MG3695B"},{"MG3696B"}};
									
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
		if ( strstr( szIdentificationText , "ANRITSU," ) == NULL ) 
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

void*		DLLEXPORT	SignalGenerator_Init( int hParentInstrumentHandle , char* szRsrcAddress , int *phInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChannelsQuantity )
{	
	STD_ERROR           StdError                    =   {0};
						
	int			iChannelCount				=	1;
	
	char		szErrorTextMessage[256]		=	{0};
	
	ViSession	handle						=	0;
	
	if ( hParentInstrumentHandle == 0 )
	{
		STD_ERR ( aumg369xb_init ( szRsrcAddress, VI_FALSE, VI_FALSE, &handle));
	}
	else
		handle = hParentInstrumentHandle;
		
	if ( phInstrumentHandle  ) 
		*phInstrumentHandle = handle;
	
	if ( InstrumentChannelsQuantity )
		*InstrumentChannelsQuantity = iChannelCount;

Error:
	
	if (( IS_NOT_OK ) || (iChannelCount < 1))
	{ 
		FREE_CALLOC( StdError.pszErrorDescription , STD_STRING , sizeof(char*));
				
		if ( StdError.pszErrorDescription )
			aumg369xb_errorMessage (handle, StdError.error , StdError.pszErrorDescription );
		
		MessagePopup("Power Supply Initializing Error!",szErrorTextMessage);
		
		aumg369xb_close (handle);
	}
	else
		if ( InstrumentConnectStatus )
			*InstrumentConnectStatus = 1;	
	
	RETURN_STDERR_POINTER;
}


void*	DLLEXPORT	SignalGenerator_Close( int *pviInstrumentHandle )
{
	STD_ERROR				StdError					=	{0};    

	int 					iStatus						=	0;  
	
	if (pviInstrumentHandle)
	{
		iStatus = aumg369xb_close( *pviInstrumentHandle );
		*pviInstrumentHandle = 0;
	}
		
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SignalGenerator_Reset ( int hInstrumentHandle )   
{   
	STD_ERROR           StdError                    =   {0};

	//STD_ERR( aumg369xb_DisableAllModulation ( viInstrumentHandle ));
	
	STD_ERR( aumg369xb_reset ( hInstrumentHandle ));

Error:
	
	RETURN_STDERR_POINTER; 
}


void* DLLEXPORT 	SignalGenerator_RF_Off ( int InstrumentHandle , int channel )
{
	STD_ERROR           StdError                    =   {0};

	STD_ERR( aumg369xb_configureOutputEnabled ( InstrumentHandle , VI_FALSE ));
	
	//STD_ERR( aumg369xb_WaitUntilSettled ( InstrumentHandle , 10000 ));

Error:
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_RF_On ( int InstrumentHandle , int channel )
{
	STD_ERROR           StdError                    =   {0};

	STD_ERR( aumg369xb_configureOutputEnabled ( InstrumentHandle , VI_TRUE));
	
	//STD_ERR( aumg369xb_WaitUntilSettled ( InstrumentHandle , 10000 ));

Error:
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_Config ( int InstrumentHandle , int channel , double lfFrequency , double lfAmplitude )
{
	STD_ERROR           StdError                    =   {0};

	STD_ERR (aumg369xb_configureFrequencyEntry ( InstrumentHandle , AUMG369XB_FREQUENCY_F1, lfFrequency ));
	
	STD_ERR (aumg369xb_configurePowerLevelEntry ( InstrumentHandle , AUMG369XB_POWER_LEVEL_L1, lfAmplitude ));
	
	//STD_ERR( aumg369xb_WaitUntilSettled ( InstrumentHandle , 10000 ));

Error:
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_SetFrequency ( int InstrumentHandle , int channel , double lfFrequency )
{
	STD_ERROR           StdError                    =   {0};

	STD_ERR (aumg369xb_configureFrequencyEntry ( InstrumentHandle , AUMG369XB_FREQUENCY_F1, lfFrequency ));
	
	//STD_ERR( aumg369xb_WaitUntilSettled ( InstrumentHandle , 10000 ));

Error:
	
	RETURN_STDERR_POINTER; 
}


void* DLLEXPORT 	SignalGenerator_SetPower ( int InstrumentHandle , int channel , double lfAmplitude )
{
	STD_ERROR           StdError                    =   {0};

	STD_ERR (aumg369xb_configurePowerLevelEntry ( InstrumentHandle , AUMG369XB_POWER_LEVEL_L1, lfAmplitude ));
	
	//STD_ERR( aumg369xb_WaitUntilSettled ( InstrumentHandle , 10000 ));

Error:
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_SetModulation ( int InstrumentHandle , int channel , int source , int iPulseMode , double dWidth , double dPeriod )
{
	STD_ERROR           StdError                    =   {0};

	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_SetModulationState ( int InstrumentHandle , int channel , int iState )
{
	STD_ERROR           StdError                    =   {0};

	RETURN_STDERR_POINTER; 
}

void main ( void)
{
	int							hInstrumentHandle									=	0,
								InstrumentConnectStatus								=	0,
								InstrumentChannelsQuantity							=	0;
		
	int							status												=	0;
	
	SignalGenerator_Init( 0 , "TCPIP::10.0.0.62::INSTR" , &hInstrumentHandle , &InstrumentConnectStatus , &InstrumentChannelsQuantity );
	                                                
	SignalGenerator_Reset ( hInstrumentHandle );
	
	SignalGenerator_Close ( &hInstrumentHandle ); 
	
	return;
}
