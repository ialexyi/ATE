#include <Ivi.h>
#include <userint.h>
#include <ansi_c.h>
//==============================================================================
// Include files

//#include "ifr2025_wrapper.h"
#include "GlobalDefines.h"
#include "toolbox.h"
#include "ifr2025.h"
#include <utility.h>

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

/***** ***** ***** ***** ***** ***** Get Last Error Text Message ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT SignalGenerator_GetLastErrorTextMessage ( int hInstrumentHandle , char **pErrorMessage )
{
	STD_ERROR				StdError					=	{0};    

	//ifr2025_GetError( pErrorMessage);
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SignalGenerator_GetErrorTextMessage ( int hInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR				StdError					=	{0};    

	FREE_CALLOC( StdError.pszErrorDescription , STD_STRING , sizeof(char*));
				
	if ( StdError.pszErrorDescription )
	{
		STD_ERR ( ifr2025_error_message	( hInstrumentHandle , iError , StdError.pszErrorDescription ));
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
		IF ((( strstr( szIdentificationText , "IFR," ) == NULL ) && ( strstr( szIdentificationText , "MARCONI INSTRUMENTS" ) == NULL ) && ( strstr( szIdentificationText , "RACAL" ) == NULL ) && ( strstr( szIdentificationText , "RI" ) == NULL )) , "Is not supported" );
	
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
									vszSupportdedModels[][12]					=	{{",2025"},{",2024"},{",2023"},{",3002"},{",3271"}};
									
	if (( pIdentificationText == NULL ) || ( strlen(pIdentificationText) < 5 ))
	{
		viOpenDefaultRM (&defaultRM);

		SetBreakOnLibraryErrors (0);
		
		status = viOpen ( defaultRM , pAddress , NULL, NULL, &hConnectionHandle );
		
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
		if (( strstr( szIdentificationText , "IFR," ) == NULL ) && ( strstr( szIdentificationText , "MARCONI INSTRUMENTS" ) == NULL ) && ( strstr( szIdentificationText , "RACAL" ) == NULL ) && ( strstr( szIdentificationText , "RI" ) == NULL ))
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

void*		DLLEXPORT	SignalGenerator_Init( int hParenthInstrumentHandle , char* szRsrcAddress , int *phInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChannelsQuantity )
{	
	STD_ERROR         	 	 StdError                    =   {0};
						
	int						iChannelCount				=	1;
	
	ViSession				handle						=	0;
	
	if ( hParenthInstrumentHandle == 0 )
	{
		IF (( szRsrcAddress == NULL ) , "Address is empty" );
		
		STD_ERR ( ifr2025_init ( szRsrcAddress, VI_TRUE, VI_FALSE, &(handle)));
		
		STD_ERR ( ifr2025_modControl ( handle , ifr2025_MOD_OFF, ifr2025_PULSE_OFF ));
	}
	else
		handle = hParenthInstrumentHandle;
		
	if ( phInstrumentHandle  ) 
		*phInstrumentHandle = handle;
	
	//STD_ERR ( ifr2025_ClearError (handle));
	
	if ( InstrumentChannelsQuantity )
		*InstrumentChannelsQuantity = iChannelCount;

Error:
	
	if (( IS_NOT_OK ) || (iChannelCount < 1))
	{ 
		FREE_CALLOC( StdError.pszErrorDescription , STD_STRING , sizeof(char*));
				
		if ( StdError.pszErrorDescription )
		{
			ifr2025_error_message (handle, StdError.error , StdError.pszErrorDescription );
		
			MessagePopup("Signal Generator Initializing Error!", StdError.pszErrorDescription  );
		}
		
		ifr2025_close (handle);
	}
	else
		if ( InstrumentConnectStatus )
			*InstrumentConnectStatus = 1;	
	
	RETURN_STDERR_POINTER;
}


void*	DLLEXPORT	SignalGenerator_Close( int *pvihInstrumentHandle )
{
	STD_ERROR				StdError					=	{0};    

	int 					iStatus						=	0;  
	
	int						handle						=	0;
	
	if (pvihInstrumentHandle)
	{
		handle = *pvihInstrumentHandle;
		
		iStatus = ifr2025_close( handle );
		
		*pvihInstrumentHandle = 0;
	}
		
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SignalGenerator_Reset ( int hInstrumentHandle )   
{   
	STD_ERROR         		StdError                    =   {0};

	int						handle						=	0;
	
	handle = hInstrumentHandle; 
	
	STD_ERR( ifr2025_reset ( handle ));

Error:
	
	RETURN_STDERR_POINTER; 
}


void* DLLEXPORT 	SignalGenerator_RF_Off ( int hInstrumentHandle , int channel )
{
	STD_ERROR         		StdError                    =   {0};

	int						handle						=	0;
	
	handle = hInstrumentHandle; 
	
	STD_ERR( ifr2025_rfState ( handle , VI_FALSE ));
	
	STD_ERR( Delay_LastComplete ( handle , 10000 ));

Error:
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_RF_On ( int hInstrumentHandle , int channel )
{
	STD_ERROR         		StdError                    =   {0};

	int						handle						=	0;
	
	handle = hInstrumentHandle; 
	
	STD_ERR( ifr2025_rfState ( handle , VI_TRUE));
	
	STD_ERR( Delay_LastComplete ( handle , 10000 ));

Error:
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_Config ( int hInstrumentHandle , int channel , double lfFrequency , double lfAmplitude )
{
	STD_ERROR         		StdError                    =   {0};

	int						handle						=	0;
	
	handle = hInstrumentHandle; 
	
	STD_ERR( ifr2025_rfFreq ( handle , lfFrequency ));
	STD_ERR( ifr2025_rfLevel ( handle , lfAmplitude , ifr2025_RFUNIT_DBM ));
		
	STD_ERR( Delay_LastComplete ( handle , 10000 ));

Error:
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_SetFrequency ( int hInstrumentHandle , int channel , double lfFrequency )
{
	STD_ERROR         		StdError                    =   {0};

	int						handle						=	0;
	
	handle = hInstrumentHandle; 
	
	STD_ERR( ifr2025_rfFreq ( handle , lfFrequency ));
	
	STD_ERR( Delay_LastComplete ( handle , 10000 ));

Error:
	
	RETURN_STDERR_POINTER; 
}


void* DLLEXPORT 	SignalGenerator_SetPower ( int hInstrumentHandle , int channel , double lfAmplitude )
{
	STD_ERROR         		StdError                    =   {0};

	int						handle						=	0;
	
	handle = hInstrumentHandle; 
	
	STD_ERR( ifr2025_rfLevel ( handle , lfAmplitude , ifr2025_RFUNIT_DBM ));
	
	STD_ERR( Delay_LastComplete ( handle , 10000 ));

Error:
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_SetModulation ( int hInstrumentHandle , int channel , int source , int iPulseMode , double dWidth , double dPeriod )
{
	STD_ERROR         		StdError                    =   {0};

	int						handle						=	0;
	
	handle = hInstrumentHandle; 
	
	ifr2025_modMode ( handle , source, ifr2025_PULSE_OFF);
	
Error:
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_SetModulationState ( int hInstrumentHandle , int channel , int iState )
{
	STD_ERROR          		StdError                    =   {0};

	int						handle						=	0;
	
	handle = hInstrumentHandle; 
	
	STD_ERR( ifr2025_modControl ( handle , iState, ifr2025_PULSE_OFF ));

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
		
		status = viPrintf ( handle , "*OPC?\n");
	
		if ( status )
			continue;
		
		Delay ( 0.2 );
		
		status = viRead ( handle , szBuffer , STD_STRING , &iCount);

		if ( iCount )
			iOpcValue = atoi(szBuffer);
			
		if ( status )
			continue;
		
		if ( iOpcValue == 1 )
			return 0;
	}
	
	return -1;	
}

void main ( void)
{
	int							hInstrumentHandle									=	0,
								InstrumentConnectStatus								=	0,
								InstrumentChannelsQuantity							=	0;
		
	int							status												=	0;
	
	SignalGenerator_Init( 0 , "GPIB::25::INSTR" , &hInstrumentHandle , &InstrumentConnectStatus , &InstrumentChannelsQuantity );
	                                                
	SignalGenerator_Reset ( hInstrumentHandle );
	
	SignalGenerator_Close ( &hInstrumentHandle ); 
	
	return;
}

