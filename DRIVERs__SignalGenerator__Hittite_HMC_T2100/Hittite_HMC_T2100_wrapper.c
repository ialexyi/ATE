#include <Ivi.h>
#include <userint.h>
#include <ansi_c.h>
#include "GlobalDefines.h"
#include "toolbox.h"
#include <utility.h>

//==============================================================================
// Include files

//#include "Hittite_HMC_T2100_wrapper.h"

//==============================================================================
// Constants

//==============================================================================
// Types
typedef struct
{
	ViSession			defaultRM,
						sessionHandle;
	
	char				*pLastStateFile,
						*pVendor,
						*pModel,
						*pFirmware,
						*pSerialNumber;
	
	double				lfTimeout,
						lfOpcLowLevelTimeout;
	
	double				lfStateFileDelay,
						lfStateFileTimeout;
	
	tsSTD_CallBackSet	*ptCallbacks;
	
} tsHandle;

//==============================================================================
// Static global variables

//==============================================================================
// Static functions
int		WaitForOperationCompletion( int sessionHandle , double lfTimeout , double lfLowLevelTimeout );
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

void*	DLLEXPORT		Config_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet )
{
	STD_ERROR						StdError									=	{0};
	
	tsHandle						*pLocalHandle								=	NULL;			
	
	int								handle										=	0;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );
	
	if ( *pHandle == 0 )
	{
		CHK_CMT(CmtNewTSV ( sizeof(tsHandle) , pHandle ));
	}
	
	handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if ( pLocalHandle->ptCallbacks  == NULL )
	{
		CALLOC( pLocalHandle->ptCallbacks , 1 , sizeof(tsSTD_CallBackSet)); 
	}
	
	memcpy( pLocalHandle->ptCallbacks , pCallBackSet , sizeof(tsSTD_CallBackSet));  

Error:
	
	if ( handle )
		CmtReleaseTSVPtr (handle);
	
	RETURN_STDERR_POINTER;
}

void*	DLLEXPORT		Equipment_GetLowLevelHandle( int hHandle , int *phLowLevelHandle )
{
	STD_ERROR						StdError										=	{0};

	tsHandle						*pLocalHandle									=	{0};
	
	if ( phLowLevelHandle )
	{
		if ( CmtGetTSVPtr ( hHandle , &pLocalHandle ) == 0 )
		{
			*phLowLevelHandle = pLocalHandle->sessionHandle; 
			
			CmtReleaseTSVPtr ( hHandle ); 
		}	
	}
	
	RETURN_STDERR_POINTER;	
}



void* DLLEXPORT SignalGenerator_GetErrorTextMessage ( int hInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char			szReadBuffer[LOW_STRING]			=	{0},
					szErrorDescription[LOW_STRING]		=	{0};
	
	int				count								=	0;
	
	char			*pTemp								=	NULL;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	SetBreakOnLibraryErrors (0); 
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
	viPrintf( pLocalHandle->sessionHandle , ":SYST:ERR?\n" );
	viRead( pLocalHandle->sessionHandle, szReadBuffer , LOW_STRING , &count );
	
	iError = atoi(szReadBuffer);
	
	pTemp = strchr( szReadBuffer , '"' );
	
	if ( pTemp )
	{
		pTemp++;
		
		strcpy( szErrorDescription , pTemp );  
		
		pTemp = strchr( szErrorDescription , '"' );
		
		if ( pTemp )
			*pTemp = 0;
	}
	
	viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );
	
	if ( pErrorMessage )
	{
		strcpy( pErrorMessage , szErrorDescription ); 
	}

	StdError.error = iError;
	
	if ( iError )
	{
		CALLOC_COPY_STRING( StdError.pszErrorDescription , szErrorDescription );
	}
	
Error:

	SetBreakOnLibraryErrors (1);
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
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
		IF (( strstr( szIdentificationText , "Hittite" ) == NULL ) , "Is not supported" );
	
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
									vszSupportdedModels[][12]					=	{{"HMC-T2100"}};
									
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
		if ( strstr( szIdentificationText , "Hittite" ) == NULL ) 
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

/***** ***** ***** ***** ***** ***** ***** ***** Init ***** ***** ***** ***** ***** ***** ***** *****/


void* DLLEXPORT SignalGenerator_Init ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *vhInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChannelsQuantity )
{
	STD_ERROR						StdError										=	{0};
	
	tsHandle						*pLocalHandle									=	{0};
	
	CmtTSVHandle 					handle											=	0;
	
	int								supported										=	0;
	
	tsSTD_CallBackSet				tSTD_CallBackSet								=	{0};
	
	double							lfTimeout										=	0.0,
									lfStateFileDelay								=	1.0,  
									lfStateFileTimeout								=	10.0, 
									lfDefaultTimeout								=	5.0;
									
	IF (( szRsrcAddress == NULL ) , "Address string is empty" );
	
	if ( hParentInstrumentHandle == 0 ) 
	{
		CHK_CMT ( CmtNewTSV ( sizeof(tsHandle) , &handle ));
	}
	else
		handle = hParentInstrumentHandle;
		
	if ( vhInstrumentHandle )
		*vhInstrumentHandle = handle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));

	if ( pLocalHandle->defaultRM == 0 )
	{
		CHK_VSA ( viOpenDefaultRM (&(pLocalHandle->defaultRM)));
	}
	
	if ( pLocalHandle->sessionHandle == 0 )
	{
		CHK_VSA ( viOpen ( pLocalHandle->defaultRM , szRsrcAddress , VI_NULL, VI_NULL, &(pLocalHandle->sessionHandle)));
	}
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , 20.0 , 0.5 );
	
	CHK_VSA ( viSetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , 500 ));
	
	FREE_STDERR_COPY_ERR( Equipment_IsSupported ( pLocalHandle->sessionHandle , NULL , NULL , NULL , &supported , &pLocalHandle )); 

	IF (( supported == 0 ) , "This device has not supported." );

	FREE_STDERR_COPY_ERR( Equipment_Info ( pLocalHandle->sessionHandle , NULL , &pLocalHandle->pVendor , &pLocalHandle->pSerialNumber , &pLocalHandle->pModel , &pLocalHandle->pFirmware ));

	STDF_UPDATE_CALLBACK_DATA(pLocalHandle->ptCallbacks); 
	
	viPrintf( pLocalHandle->sessionHandle ,"*CLS\n" ); 
	
	STDF_CONFIG_VALUE("Timeout", VAL_DOUBLE , 1 , lfTimeout , lfDefaultTimeout );	
	
	if ( lfTimeout == 0.0 )
		lfTimeout = lfDefaultTimeout;
	
	viSetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , (lfTimeout*1E3) );
	
	STDF_CONFIG_VALUE("OPC_Timeout", VAL_DOUBLE , 1 , (pLocalHandle->lfTimeout) , lfDefaultTimeout );	
	
	if ( pLocalHandle->lfTimeout == 0.0 )
		pLocalHandle->lfTimeout = lfDefaultTimeout;
	
	STDF_CONFIG_VALUE("OPC_LowLevel_Timeout", VAL_DOUBLE , 1 , (pLocalHandle->lfOpcLowLevelTimeout) , lfDefaultTimeout );	 
	
	if ( pLocalHandle->lfOpcLowLevelTimeout == 0.0 )
		pLocalHandle->lfOpcLowLevelTimeout = lfDefaultTimeout;
	
		STDF_CONFIG_VALUE("State_File_Timeout", VAL_DOUBLE , 1 , (pLocalHandle->lfStateFileTimeout) , lfStateFileTimeout );	 
	
	if ( pLocalHandle->lfStateFileTimeout == 0.0 )
		pLocalHandle->lfStateFileTimeout = lfStateFileTimeout;
	
	STDF_CONFIG_VALUE("State_File_Delay", VAL_DOUBLE , 1 , (pLocalHandle->lfStateFileDelay) , lfStateFileDelay );	 
	
	if ( pLocalHandle->lfStateFileDelay == 0.0 )
		pLocalHandle->lfStateFileDelay = lfStateFileDelay;   
Error:
	
	if ( InstrumentConnectStatus )
		*InstrumentConnectStatus = supported;
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** Close ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT SignalGenerator_Close ( int *vhInstrumentHandle )
{
	STD_ERROR		StdError						=	{0};
	
	tsHandle		*pLocalHandle					=	{0};

	int				handle							=	0;
	
	IF (( vhInstrumentHandle == NULL ) , "Handle is empty" );
	
	handle = *vhInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	viClose (pLocalHandle->sessionHandle);  
	
	viClose (pLocalHandle->defaultRM);
	
	FREE(pLocalHandle->pLastStateFile);
	FREE(pLocalHandle->pFirmware);
	FREE(pLocalHandle->pSerialNumber);
	FREE(pLocalHandle->pModel);
	
	CmtReleaseTSVPtr ( handle ); 
	
	CmtDiscardTSV (handle);
	
	*vhInstrumentHandle = 0;
	
Error:

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SignalGenerator_Reset ( int hInstrumentHandle )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,"*CLS\n" ));
					
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,"*RST\n" ));
					
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SignalGenerator_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


void* DLLEXPORT SignalGenerator_RF_Off ( int hInstrumentHandle , int channel )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,"OUTP OFF\n" ));
					
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SignalGenerator_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SignalGenerator_RF_On ( int hInstrumentHandle , int channel )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,"OUTP ON\n" ));
					
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SignalGenerator_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SignalGenerator_Config ( int hInstrumentHandle , int channel , double lfFrequency , double lfAmplitude )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,"freq %lf\n" , lfFrequency ));
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,"pow %lf\n" , lfAmplitude ));
					
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SignalGenerator_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SignalGenerator_SetFrequency ( int hInstrumentHandle , int channel , double lfFrequency )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,"freq %lf\n" , lfFrequency ));
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SignalGenerator_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


void* DLLEXPORT SignalGenerator_SetPower ( int hInstrumentHandle , int channel , double lfAmplitude )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,"pow %lf\n" , lfAmplitude ));
					
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SignalGenerator_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

int		WaitForOperationCompletion( int sessionHandle , double lfTimeout , double lfLowLevelTimeout )
{
	double			lfStartTime						=	0.0,
					lfCurrentTime					=	0.0;

	int				iOpcStatus						=	0;
	
	char			szReadFeedback[LOW_STRING]		=	{0};
	
	int				count							=	0;
	
	int				iTimeout						=	0;
	
	int				iError							=	0;
	
	viGetAttribute ( sessionHandle , VI_ATTR_TMO_VALUE , &iTimeout );   
	
	SetBreakOnLibraryErrors (0); 
	
	viSetAttribute ( sessionHandle , VI_ATTR_TMO_VALUE , (lfLowLevelTimeout*1E3) );  
	
	GetCurrentDateTime(&lfStartTime);
	
	do
	{
		viPrintf( sessionHandle , "*OPC?\n" );
						
		viRead( sessionHandle, szReadFeedback , (LOW_STRING-1) , &count );
	
		iOpcStatus = atoi(szReadFeedback);
		
		if ( iOpcStatus )
			break;
		
		GetCurrentDateTime(&lfCurrentTime);
		
	} while((lfCurrentTime-lfStartTime) < lfTimeout );
	
	if ( iOpcStatus )
	{
		viPrintf( sessionHandle , ":SYST:ERR?\n" );
		SetBreakOnLibraryErrors (0);
		viRead( sessionHandle, szReadFeedback , LOW_STRING , &count );
		SetBreakOnLibraryErrors (1);
	
		iError = atoi(szReadFeedback);
	
		if ( iError == -420 )
		{
			viPrintf( sessionHandle , "*CLS\n" );  
		}
	}
	
	viSetAttribute ( sessionHandle , VI_ATTR_TMO_VALUE , iTimeout );  
	
	SetBreakOnLibraryErrors (1); 
	
	return iOpcStatus;
}

void main ( void)
{
	int							hInstrumentHandle									=	0,
								InstrumentConnectStatus								=	0,
								InstrumentChannelsQuantity							=	0;
		
	int							status												=	0;
	
	SignalGenerator_Init( 0 , "GPIB::2::INSTR" , &hInstrumentHandle , &InstrumentConnectStatus , &InstrumentChannelsQuantity );
	 
	SignalGenerator_Reset ( hInstrumentHandle ); 
	
	SignalGenerator_SetFrequency ( hInstrumentHandle , 0 , 2.23E9 );

	SignalGenerator_SetPower ( hInstrumentHandle , 0 , -11.5 );
	
	SignalGenerator_RF_On ( hInstrumentHandle , 0 );    
	
	SignalGenerator_RF_Off ( hInstrumentHandle , 0 );   
	
	SignalGenerator_Close ( &hInstrumentHandle ); 
	
	return;
}


