#include <Ivi.h>
#include <userint.h>
#include <ansi_c.h>

//==============================================================================
// Include files

//#include "hp8647a_wrapper.h"
#include "GlobalDefines.h"
#include <formatio.h>
#include "toolbox.h"
#include "hp8647a.h"
#include <utility.h>

//==============================================================================
// Constants

//==============================================================================
// Types

typedef struct
{

	double				lfAttenuator,
						lfReferenceLevel,
						lfFrequency,
						lfPower;
	
	int					handle;
	
	int					bRfState,
						bModulationState;
	
	int					iModulation;
	
	char				szAddress[LOW_STRING];
	
} tsLocalStore;

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

	//hp8647a_GetError( pErrorMessage);
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SignalGenerator_GetErrorTextMessage ( int hInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR				StdError					=	{0};    

	//iError = hp8647a_error_message	( hInstrumentHandle , iError , pErrorMessage );
	
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
									szIdentificationUpperText[LOW_STRING]		=	{0},
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
		strcpy( szIdentificationUpperText , szIdentificationText );
		StringUpperCase (szIdentificationUpperText);
		
		IF ((( strstr( szIdentificationUpperText , "AGILENT TECHNOLOGIES" ) == NULL ) && ( strstr( szIdentificationUpperText ,"HEWLETT-PACKARD") == NULL )) , "Is not supported" );
	
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
									bSupport									=	0, 
									defaultRM									=	0,
									hConnectionHandle							=	0;
	
	char							szIdentificationText[LOW_STRING]			=	{0},
									szIdentificationUpperText[LOW_STRING]		=	{0}; 
	
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
		strcpy( szIdentificationUpperText , szIdentificationText );
		StringUpperCase (szIdentificationUpperText);
		
		if (( strstr( szIdentificationUpperText , "AGILENT TECHNOLOGIES" ) == NULL ) && ( strstr( szIdentificationUpperText ,"HEWLETT-PACKARD") == NULL ))
			break;	
	
		if ( strstr( szIdentificationText , "8647A" ))
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
	
	char					szErrorTextMessage[256]		=	{0};
	
	ViSession				handle						=	0;
	
	int						iGPIB_Address				=	0;
	
	tsLocalStore			*pLocalStore				=	NULL;
		
	int						bGetLock					=	0;
	
	if ( hParenthInstrumentHandle == 0 )
	{
		CHK_CMT( CmtNewTSV ( sizeof(tsLocalStore) , &handle ));
		
		CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStore ));
		bGetLock = 1;
		
		IF (( szRsrcAddress == NULL ) , "Address is empty" );
		
		strcpy( pLocalStore->szAddress , szRsrcAddress );
		
		sscanf( szRsrcAddress , "GPIB::%d" , &iGPIB_Address);
		
		STD_ERR ( hp8647a_init ( iGPIB_Address, VI_TRUE, VI_FALSE, &(pLocalStore->handle)));
	}
	else
		handle = hParenthInstrumentHandle;
		
	if ( phInstrumentHandle  ) 
		*phInstrumentHandle = handle;
	
	//STD_ERR ( hp8647a_ClearError (handle));
	
	if ( InstrumentChannelsQuantity )
		*InstrumentChannelsQuantity = iChannelCount;

Error:
	
	if (( bGetLock ) && ( handle ))
		CmtReleaseTSVPtr ( handle );
	
	if (( IS_NOT_OK ) || (iChannelCount < 1))
	{ 
		//FREE_CALLOC( StdError.pszErrorDescription , STD_STRING , sizeof(char*));
				
		//if ( StdError.pszErrorDescription )
		//	hp8647a_error_message (handle, StdError.error , StdError.pszErrorDescription );
		
		MessagePopup("Power Supply Initializing Error!",szErrorTextMessage);
		
		hp8647a_close (handle);
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
	
	tsLocalStore			*pLocalStore				=	NULL;
	
	int						handle						=	0;
	
	if (pvihInstrumentHandle)
	{
		handle = *pvihInstrumentHandle;
		
		CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStore ));

		iStatus = hp8647a_close( pLocalStore->handle );
		
		CmtReleaseTSVPtr (handle);    
		CmtDiscardTSV (handle);
		
		*pvihInstrumentHandle = 0;
	}
		
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SignalGenerator_Reset ( int hInstrumentHandle )   
{   
	STD_ERROR         		StdError                    =   {0};

	tsLocalStore			*pLocalStore				=	NULL;

	int						handle						=	0;
	
	handle = hInstrumentHandle; 
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStore ));
	
	STD_ERR( hp8647a_reset ( pLocalStore->handle ));

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER; 
}


void* DLLEXPORT 	SignalGenerator_RF_Off ( int hInstrumentHandle , int channel )
{
	STD_ERROR         		StdError                    =   {0};

	tsLocalStore			*pLocalStore				=	NULL;

	int						handle						=	0;
	
	handle = hInstrumentHandle; 
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStore ));
	
	STD_ERR( hp8647a_gen_signl ( pLocalStore->handle , VI_FALSE , VI_FALSE ));
	
	STD_ERR( Delay_LastComplete ( pLocalStore->handle , 10000 ));

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_RF_On ( int hInstrumentHandle , int channel )
{
	STD_ERROR         		StdError                    =   {0};

	tsLocalStore			*pLocalStore				=	NULL;

	int						handle						=	0;
	
	handle = hInstrumentHandle; 
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStore ));
	
	STD_ERR( hp8647a_gen_signl ( pLocalStore->handle , VI_FALSE , VI_TRUE));
	
	STD_ERR( Delay_LastComplete ( pLocalStore->handle , 10000 ));

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_Config ( int hInstrumentHandle , int channel , double lfFrequency , double lfAmplitude )
{
	STD_ERROR         		StdError                    =   {0};

	tsLocalStore			*pLocalStore				=	NULL;

	int						handle						=	0;
	
	handle = hInstrumentHandle; 
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStore ));
	
	pLocalStore->lfFrequency = lfFrequency;
	pLocalStore->lfPower = lfAmplitude;
	
	STD_ERR( hp8647a_rf_signal ( pLocalStore->handle , ( pLocalStore->lfFrequency / 1E6 ), 1, 0.0, pLocalStore->lfPower , 1 ));
		
	STD_ERR( Delay_LastComplete ( pLocalStore->handle , 10000 ));

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_SetFrequency ( int hInstrumentHandle , int channel , double lfFrequency )
{
	STD_ERROR         		StdError                    =   {0};

	tsLocalStore			*pLocalStore				=	NULL;

	int						handle						=	0;
	
	handle = hInstrumentHandle; 
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStore ));
	
	pLocalStore->lfFrequency = lfFrequency;
	
	STD_ERR( hp8647a_rf_signal ( pLocalStore->handle , ( pLocalStore->lfFrequency / 1E6 ), 1, 0.0, pLocalStore->lfPower , 1 ));
	
	STD_ERR( Delay_LastComplete ( pLocalStore->handle , 10000 ));

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER; 
}


void* DLLEXPORT 	SignalGenerator_SetPower ( int hInstrumentHandle , int channel , double lfAmplitude )
{
	STD_ERROR         		StdError                    =   {0};

	tsLocalStore			*pLocalStore				=	NULL;

	int						handle						=	0;
	
	handle = hInstrumentHandle; 
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStore ));
	
	if ( pLocalStore->lfFrequency == 0.0 )
		pLocalStore->lfFrequency = 80E6;
		
	pLocalStore->lfPower = lfAmplitude;
	
	STD_ERR( hp8647a_rf_signal ( pLocalStore->handle , ( pLocalStore->lfFrequency / 1E6 ), 1, 0.0, pLocalStore->lfPower , 1 ));
	
	STD_ERR( Delay_LastComplete ( pLocalStore->handle , 10000 ));

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_SetModulation ( int hInstrumentHandle , int channel , int source , int iPulseMode , double dWidth , double dPeriod )
{
	STD_ERROR         		StdError                    =   {0};

	tsLocalStore			*pLocalStore				=	NULL;

	int						handle						=	0;
	
	handle = hInstrumentHandle; 
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStore ));

	pLocalStore->iModulation  = source;

	STD_ERR( hp8647a_mod_on_off ( pLocalStore->handle , pLocalStore->iModulation , pLocalStore->bModulationState ));
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );	
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_SetModulationState ( int hInstrumentHandle , int channel , int iState )
{
	STD_ERROR          		StdError                    =   {0};

	tsLocalStore			*pLocalStore				=	NULL;

	int						handle						=	0;
	
	handle = hInstrumentHandle; 
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStore ));
	
	pLocalStore->bModulationState  = iState;
	
	STD_ERR( hp8647a_mod_on_off ( pLocalStore->handle , pLocalStore->iModulation , pLocalStore->bModulationState ));
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
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
		
		status = hp8647a_write ( handle , "*OPC?\n");
	
		if ( status )
			continue;
		
		Delay ( 0.2 );
		
		status = hp8647a_read ( handle , STD_STRING , szBuffer , &iCount);

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
	
	SignalGenerator_Init( 0 , "TCPIP::10.0.0.62::INSTR" , &hInstrumentHandle , &InstrumentConnectStatus , &InstrumentChannelsQuantity );
	                                                
	SignalGenerator_Reset ( hInstrumentHandle );
	
	SignalGenerator_Close ( &hInstrumentHandle ); 
	
	return;
}

