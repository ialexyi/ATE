//==============================================================================
// Include files
#include "Windows.h"

#include <utility.h>
#include <visa.h>
#include "GlobalDefines.h"
#include <formatio.h>
#include "toolbox.h"
#include <userint.h>
#include <ansi_c.h>

//==============================================================================
// Constants

#define				MAX_TIMEOUT			5000

/*
MODEL_TYPE_N9030A,
MODEL_TYPE_N9020A, 
MODEL_TYPE_N9010A, 
MODEL_TYPE_N9000A, 
MODEL_TYPE_N8201A,  
MODEL_TYPE_E4448A, 
MODEL_TYPE_E4447A, 
MODEL_TYPE_E4446A,
MODEL_TYPE_E4445A, 
MODEL_TYPE_E4443A, 
MODEL_TYPE_E4440A,
MODEL_TYPE_E4406A, 
MODEL_TYPE_E4411B,
MODEL_TYPE_E4408B, 
MODEL_TYPE_E4407B, 
MODEL_TYPE_E4405B,  
MODEL_TYPE_E4404B, 
MODEL_TYPE_E4403B,
MODEL_TYPE_E4402B, 
MODEL_TYPE_E4401B,
MODEL_TYPE_N9010B,
MODEL_TYPE_E7405A 
MODEL_TYPE_E7404A, 
MODEL_TYPE_E7403A,
MODEL_TYPE_E7402A, 
MODEL_TYPE_E7401A, 
*/



//==============================================================================
// Types
typedef enum { MODEL_TYPE_DEFAULT ,MODEL_TYPE_N9030A,MODEL_TYPE_N9020A, MODEL_TYPE_N9010A, MODEL_TYPE_N9010B,MODEL_TYPE_N9030B, MODEL_TYPE_N9000A, MODEL_TYPE_N8201A,  MODEL_TYPE_E4448A, MODEL_TYPE_E4447A, MODEL_TYPE_E4446A, MODEL_TYPE_E4445A, MODEL_TYPE_E4443A, MODEL_TYPE_E4440A, MODEL_TYPE_E4406A, MODEL_TYPE_E4411B, MODEL_TYPE_E4408B, MODEL_TYPE_E4407B, MODEL_TYPE_E4405B,  MODEL_TYPE_E4404B, MODEL_TYPE_E4403B,MODEL_TYPE_E4402B, MODEL_TYPE_E4401B, MODEL_TYPE_E7405A, MODEL_TYPE_E7404A, MODEL_TYPE_E7403A,MODEL_TYPE_E7402A, MODEL_TYPE_E7401A, MODEL_TYPE_SENTINEL} teModelSupport;

typedef struct
{
	ViSession			defaultRM,
						sessionHandle;
	
	char				*pLastStateFile,
						*pVendor,
						*pModel,
						*pFirmware,
						*pSerialNumber;
	
	double				lfDelay,
						lfTimeout,
						lfOpcLowLevelTimeout;
	
	double				lfStateFileDelay,
						lfStateFileTimeout;
	
	tsSTD_CallBackSet	*ptCallbacks;
	
	teModelSupport		tType;
	
} tsHandle;
//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions
int		WaitForOperationCompletion( int sessionHandle , double lfTimeout , double lfLowLevelTimeout );

int		RecursiveMakeDirectory( int sessionHandle , char *pPath );

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


void*	DLLEXPORT		Config_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet )
{
	STD_ERROR						StdError									=	{0};
	
	tsHandle						*pLocalStorage								=	NULL;			
	
	int								handle										=	0;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );
	
	if ( *pHandle == 0 )
	{
		CHK_CMT(CmtNewTSV ( sizeof(tsHandle) , pHandle ));
	}
	
	handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	if ( pLocalStorage->ptCallbacks )
	{
		FREE(pLocalStorage->ptCallbacks->pCommentCallbackData);
		FREE(pLocalStorage->ptCallbacks->pConfigValueCallbackData);
		FREE(pLocalStorage->ptCallbacks->pCheckForBreakCallbackData);
		FREE(pLocalStorage->ptCallbacks->pFileCallbackData);
	
		FREE(pLocalStorage->ptCallbacks);	
	}
		
	pLocalStorage->ptCallbacks = pCallBackSet;

Error:
	
	if ( handle )
		CmtReleaseTSVPtr (handle);
	
	RETURN_STDERR_POINTER;
}

void*	DLLEXPORT		Equipment_Info ( int hLowLevelHandle , char *pAddress , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware )
{
	STD_ERROR						StdError									=	{0};
	
	int								iCount										=	0, 
									status										=	0, 
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
		IF ((( strstr( szIdentificationText , "Agilent Technologies" ) == NULL )) && (( strstr( szIdentificationText , "Keysight Technologies" ) == NULL ))  , "Is not supported" );
	
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
	STD_ERROR						StdError										=	{0};

	tsHandle						*pLocalStorage									=	{0};
	
	if ( phLowLevelHandle )
	{
		if ( CmtGetTSVPtr ( hHandle , &pLocalStorage ) == 0 )
		{
			*phLowLevelHandle = pLocalStorage->sessionHandle; 
			
			CmtReleaseTSVPtr ( hHandle ); 
		}	
	}
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		Equipment_IsSupported ( int hLowLevelHandle , char *pAddress , char *pID_String , char *pIdentificationText , int *pbSupporting , void **pLocalData )
{
	STD_ERROR						StdError										=	{0};
	
	int								iCount											=	0, 
									status											=	0, 
									iIndex											=	0, 
									bSupport										=	0, 
									defaultRM										=	0,
									hConnectionHandle								=	0;
	
	char							szIdentificationText[LOW_STRING]				=	{0},
																					//MODEL_TYPE_N9030A,MODEL_TYPE_N9020A,MODEL_TYPE_N9010A,MODEL_TYPE_N9010B,MODEL_TYPE_N9030B,MODEL_TYPE_N9000A,MODEL_TYPE_N8201A,MODEL_TYPE_E4448A,MODEL_TYPE_E4447A,MODEL_TYPE_E4446A,MODEL_TYPE_E4445A, MODEL_TYPE_E4443A, MODEL_TYPE_E4440A,MODEL_TYPE_E4406A, MODEL_TYPE_E4411B,MODEL_TYPE_E4408B, MODEL_TYPE_E4407B, MODEL_TYPE_E4405B,  MODEL_TYPE_E4404B, MODEL_TYPE_E4403B,MODEL_TYPE_E4402B, MODEL_TYPE_E4401B, MODEL_TYPE_E7405A MODEL_TYPE_E7404A,MODEL_TYPE_E7403A,MODEL_TYPE_E7402A, MODEL_TYPE_E7401A
									vszSupportdedModels[][16]						=	{{"N9030A"},		{"N9020A"},		{"N9010A"},			{"N9010B"},		{"N9030B"},			{"N9000A"},		{"N8201A"},			{"E4448A"},		{"E4447A"},			{"E4446A"},		{"E4445A"},			{"E4443A"},			{"E4440A"},		{"E4406A"},			{"E4411B"},		{"E4408B"},			{"E4407B"},			{"E4405B"},			{"E4404B"},			{"E4403B"},		{"E4402B"},			{"E4401B"},			{"E7405A"},		{"E7404A"},			{"E7403A"},		{"E7402A"},			{"E7401A"}};

	tsHandle						*pLocalStorage									=	{0};
									
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
	
	if ( pLocalData )
	{
		pLocalStorage = (tsHandle*)(*pLocalData);
		pLocalStorage->tType = 0;
	}
	
	do
	{
		if (( strstr( szIdentificationText , "Agilent Technologies" ) == NULL ) && ( strstr( szIdentificationText , "Keysight Technologies" ) == NULL ))
			break;
	
		for ( iIndex = 0; iIndex < (sizeof(vszSupportdedModels) / sizeof(vszSupportdedModels[0])); iIndex++ )
			if ( strstr( szIdentificationText , vszSupportdedModels[iIndex]) )
			{		
				if ( pLocalStorage )
					pLocalStorage->tType = iIndex+1; 
				
				bSupport = 1; 
				break;
			}	
					
	}while(0);
	
	if (pbSupporting)
		*pbSupporting = bSupport;
	
	RETURN_STDERR_POINTER;
}


void* DLLEXPORT SpectrumAnalyzer_GetErrorTextMessage ( int hInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalStorage						=	{0};

	int				handle								=	0;
	
	char			szCommand[LOW_STRING]				=	{0},
					szReadBuffer[LOW_STRING]			=	{0},
					szErrorDescription[LOW_STRING]		=	{0};
	
	int				count								=	0;
	
	char			*pTemp								=	NULL;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	SetBreakOnLibraryErrors (0); 
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9010B:	
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
							 
					strcpy( szCommand , ":SYST:ERR?\n" );
					break;
			
		default :
					strcpy( szCommand , ":SYST:ERR?\n" );
					break;
	}
	
	WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout );  
	
	viPrintf( pLocalStorage->sessionHandle , szCommand );
	viRead( pLocalStorage->sessionHandle, szReadBuffer , LOW_STRING , &count );
	
	iError = atoi(szReadBuffer);
	
	if ( iError == -420 || iError == -144 || iError == -113)
	{
		viPrintf( pLocalStorage->sessionHandle , "*CLS\n" ); 
		
		strcpy( szReadBuffer , "No Error");
	}
	
	pTemp = strchr( szReadBuffer , '"' );
	
	if ( pTemp )
	{
		pTemp++;
		
		strcpy( szErrorDescription , pTemp );  
		
		pTemp = strchr( szErrorDescription , '"' );
		
		if ( pTemp )
			*pTemp = 0;
	}
	
	viPrintf( pLocalStorage->sessionHandle , "*CLS\n" );
	
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

/***** ***** ***** ***** ***** ***** ***** ***** Init ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT SpectrumAnalyzer_Init ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *vhInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChannelsQuantity )
{
	STD_ERROR						StdError										=	{0};
	
	tsHandle						*pLocalStorage									=	{0};
	
	CmtTSVHandle 					handle											=	0;
	
	int								supported										=	0;
	
	tsSTD_CallBackSet				tSTD_CallBackSet								=	{0};
	
	double							lfDelay											=	0.1,
									lfTimeout										=	0.0,
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
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));

	if ( pLocalStorage->defaultRM == 0 )
	{
		CHK_VSA ( viOpenDefaultRM (&(pLocalStorage->defaultRM)));
	}
	
	if ( pLocalStorage->sessionHandle == 0 )
	{
		CHK_VSA ( viOpen ( pLocalStorage->defaultRM , szRsrcAddress , VI_NULL, VI_NULL, &(pLocalStorage->sessionHandle)));
	}
	
	WaitForOperationCompletion( pLocalStorage->sessionHandle , 20.0 , 0.5 );
	
	CHK_VSA ( viSetAttribute ( pLocalStorage->sessionHandle , VI_ATTR_TMO_VALUE , 500 ));
	
	FREE_STDERR_COPY_ERR( Equipment_IsSupported ( pLocalStorage->sessionHandle , NULL , NULL , NULL , &supported , &pLocalStorage )); 

	IF (( supported == 0 ) , "This device has not supported." );

	FREE_STDERR_COPY_ERR( Equipment_Info ( pLocalStorage->sessionHandle , NULL , &pLocalStorage->pVendor , &pLocalStorage->pSerialNumber , &pLocalStorage->pModel , &pLocalStorage->pFirmware ));

	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks); 
	
	viPrintf( pLocalStorage->sessionHandle , ":CAL:AUTO OFF\n" );  
	
	viPrintf( pLocalStorage->sessionHandle ,"*CLS\n" ); 
	
	viPrintf( pLocalStorage->sessionHandle ,":FORM:DATA REAL\n" );  
	
	viPrintf( pLocalStorage->sessionHandle ,":FORM:BORD SWAP\n" );  
	
	STDF_CONFIG_VALUE("Timeout", VAL_DOUBLE , 1 , lfTimeout , lfDefaultTimeout );	
	
	if ( lfTimeout == 0.0 )
		lfTimeout = lfDefaultTimeout;
	
	viSetAttribute ( pLocalStorage->sessionHandle , VI_ATTR_TMO_VALUE , (lfTimeout*1E3) );
	
	STDF_CONFIG_VALUE("OPC_Timeout", VAL_DOUBLE , 1 , (pLocalStorage->lfTimeout) , lfDefaultTimeout );	
	
	if ( pLocalStorage->lfTimeout == 0.0 )
		pLocalStorage->lfTimeout = lfDefaultTimeout;
	
	STDF_CONFIG_VALUE("OPC_LowLevel_Timeout", VAL_DOUBLE , 1 , (pLocalStorage->lfOpcLowLevelTimeout) , lfDefaultTimeout );	 
	
	if ( pLocalStorage->lfOpcLowLevelTimeout == 0.0 )
		pLocalStorage->lfOpcLowLevelTimeout = lfDefaultTimeout;
	
		STDF_CONFIG_VALUE("State_File_Timeout", VAL_DOUBLE , 1 , (pLocalStorage->lfStateFileTimeout) , lfStateFileTimeout );	 
	
	if ( pLocalStorage->lfStateFileTimeout == 0.0 )
		pLocalStorage->lfStateFileTimeout = lfStateFileTimeout;
	
	STDF_CONFIG_VALUE("State_File_Delay", VAL_DOUBLE , 1 , (pLocalStorage->lfStateFileDelay) , lfStateFileDelay );	 
	
	if ( pLocalStorage->lfStateFileDelay == 0.0 )
		pLocalStorage->lfStateFileDelay = lfStateFileDelay;   
	
	STDF_CONFIG_VALUE("Delay", VAL_DOUBLE , 1 , (pLocalStorage->lfDelay) , lfDelay );	 
	
	if ( pLocalStorage->lfDelay == 0.0 )
		pLocalStorage->lfDelay = lfDelay;   
	
	
Error:
	
	if ( InstrumentConnectStatus )
		*InstrumentConnectStatus = supported;
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** Close ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT SpectrumAnalyzer_Close ( int *vhInstrumentHandle )
{
	STD_ERROR		StdError						=	{0};
	
	tsHandle		*pLocalStorage					=	{0};

	int				handle							=	0;
	
	IF (( vhInstrumentHandle == NULL ) , "Handle is empty" );
	
	handle = *vhInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	viPrintf( pLocalStorage->sessionHandle , ":CAL:AUTO ON\n" ); 
	viPrintf( pLocalStorage->sessionHandle , ":CAL\n" );  
	
	viClose (pLocalStorage->sessionHandle);  
	
	viClose (pLocalStorage->defaultRM);
	
	FREE(pLocalStorage->pLastStateFile);
	FREE(pLocalStorage->pFirmware);
	FREE(pLocalStorage->pSerialNumber);
	FREE(pLocalStorage->pModel);

	if ( pLocalStorage->ptCallbacks )
	{
		FREE(pLocalStorage->ptCallbacks->pCommentCallbackData);
		FREE(pLocalStorage->ptCallbacks->pConfigValueCallbackData);
		FREE(pLocalStorage->ptCallbacks->pCheckForBreakCallbackData);
		FREE(pLocalStorage->ptCallbacks->pFileCallbackData);
	
		FREE(pLocalStorage->ptCallbacks);	
	}
		
	CmtReleaseTSVPtr ( handle ); 
	
	CmtDiscardTSV (handle);
	
	*vhInstrumentHandle = 0;
	
Error:

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_Reset ( int hInstrumentHandle )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalStorage						=	{0};

	int				handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default :
				{
						CHK_VSA ( viPrintf( pLocalStorage->sessionHandle ,"*CLS\n" ));
					
						CHK_VSA ( viPrintf( pLocalStorage->sessionHandle ,"*RST\n" ));
						
						WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout );  
				}
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_AlignNow ( int hInstrumentHandle , double lfTimeout )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalStorage						=	{0};

	int				handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default :
				{
						CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":CAL\n" ));
						
						WaitForOperationCompletion( pLocalStorage->sessionHandle , lfTimeout ,  pLocalStorage->lfOpcLowLevelTimeout );  
				}
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_NoiseFigure_CalibrateNow ( int hInstrumentHandle , double lfTimeout )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalStorage						=	{0};

	int				handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default :
				{
						CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":NFIG:CAL:INIT\n" ));
						
						WaitForOperationCompletion( pLocalStorage->sessionHandle , lfTimeout ,  pLocalStorage->lfOpcLowLevelTimeout );  
				}
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_NoiseFigure_ApplyCalibration ( int hInstrumentHandle , double lfTimeout )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalStorage						=	{0};

	int				handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default :
				{
						CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":NFIG:CAL:STAT 1\n" ));
						
						WaitForOperationCompletion( pLocalStorage->sessionHandle , lfTimeout ,  pLocalStorage->lfOpcLowLevelTimeout );  
				}
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_NoiseFigure_SetLossCompensationTable ( int hInstrumentHandle , int bAfterDUT , double *pvFrequencies, double *pvLoss , int iNumberOfPoints )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalStorage						=	{0};

	int				handle								=	0;
	
	char			szString[LOW_STRING]				=	{0},
					vszDirection[2][32]					=	{{"AFT"},{"BEF"}};
	
	char			*pFormatedData						=	NULL;
	
	char			before								=	0;
	
	int				iPointIndex							=	0,
					iCurrentPosition					=	0;
	
	before = !bAfterDUT;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	IF (( pvFrequencies == NULL ) , "Frequency list is empty" );
	IF (( pvLoss == NULL ) , "Loss list is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default :
				{
						
						sprintf( szString , ":NFIG:CORR:LOSS:%s:MODE TABL\n" , vszDirection[before] ); 
						CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
						
						WaitForOperationCompletion( pLocalStorage->sessionHandle , 5.0 ,  pLocalStorage->lfOpcLowLevelTimeout );  
						
						sprintf( szString , ":NFIG:CORR:LOSS:%s:TABL:DATA:DEL\n" , vszDirection[before] ); 
						CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
						
						WaitForOperationCompletion( pLocalStorage->sessionHandle ,  5.0 ,  pLocalStorage->lfOpcLowLevelTimeout );  
						
						CALLOC_ERR( pFormatedData , (( iNumberOfPoints * 40 ) +  64 ), sizeof(char));
						
						iCurrentPosition = sprintf( pFormatedData , ":NFIG:CORR:LOSS:%s:TABL:DATA " , vszDirection[before] ); 
						
						for ( iPointIndex = 0; iPointIndex < iNumberOfPoints; iPointIndex++ )
						{
							iCurrentPosition += sprintf( pFormatedData+iCurrentPosition , "%E, " , pvFrequencies[iPointIndex] );
							iCurrentPosition += sprintf( pFormatedData+iCurrentPosition , "%E, " , ( 0.0 - pvLoss[iPointIndex] ) ); 
						}
						
						iCurrentPosition -= 2;
						
						sprintf( pFormatedData+iCurrentPosition , "\n" );
						CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , pFormatedData )); 
						
						WaitForOperationCompletion( pLocalStorage->sessionHandle , 20.0 ,  pLocalStorage->lfOpcLowLevelTimeout );  
						
				}
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	FREE(pFormatedData);
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_Preset ( int hInstrumentHandle )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalStorage						=	{0};

	int				handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
			
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , "SYSTem:PRESet" ));  
			
			break;
			
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default :
				{
						CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SOUR:PRES\n" ));
				}
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_SetAuto ( int hInstrumentHandle )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalStorage						=	{0};

	int				handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default :
				{
						CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
				}
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_AutoTune ( int hInstrumentHandle , double lfTimeout )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalStorage						=	{0};

	int				handle								=	0;
	
	char			szReadFeedback[LOW_STRING]			=	{0};
	
	int				iActualSize							=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default :
				{
					CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":INST:SEL?\n" )); 

					SetBreakOnLibraryErrors (0);
					viRead( pLocalStorage->sessionHandle, szReadFeedback , 20 , &iActualSize );
					SetBreakOnLibraryErrors (1);
					
					if ( strcmp( szReadFeedback , "SA" ) == 0 )
					{
						CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "RTSA" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "SEQAN" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "EMI" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "BASIC" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "WCDMA" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "EDGEGSM" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "WIMAXOFDMA" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "VSA" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "PNOISE" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "NFIGure" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "ADEMOD" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "BTooth" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "TDSCDMA" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "TDSCDMA" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "CDMA2K" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "CDMA1XEV" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "LTE" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "LTETDD" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "MSR" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "DVB" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "DTMB" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "DCATV" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "ISDBT" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "CMMB" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
					
					if ( strcmp( szReadFeedback , "WLAN" ) == 0 )
					{
						//CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:TUNE:IMM\n" ));
						break;
					}
				}
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}
	
void* DLLEXPORT SpectrumAnalyzer_SetFrequencyCenterSpan ( int hInstrumentHandle , int channel , double lfFrequency , double lfSpan )
{
	STD_ERROR				StdError							=	{0};
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default :
				{
						CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:CENT %E\n" , lfFrequency ));
	
						WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout );
	
						CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:SPAN %E\n" , lfSpan ));
				}
	}
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_ConfigureMarker ( int hInstrumentHandle , int channel , int marker , int bState )
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			if ( bState > 0 )
				sprintf( szString , ":CALC:MARK%d:STAT ON\n" , marker );
			else
				sprintf( szString , ":CALC:MARK%d:STAT OFF\n" , marker );
	
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 
	 

void* DLLEXPORT SpectrumAnalyzer_SetMarkerSearchPeakAuto ( int hInstrumentHandle , int iMarkerNr , int bMax , int bState )
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			sprintf( szString , ":CALC:MARK%d:STAT ON\n" , iMarkerNr );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
			
			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			if ( bState > 0 )
				sprintf( szString , ":CALC:MARK%d:CPS:STAT ON\n" , iMarkerNr );
			else
				sprintf( szString , ":CALC:MARK%d:CPS:STAT OFF\n" , iMarkerNr );
				
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
		
			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			if ( bMax > 0 )
			{
				sprintf( szString , ":CALC:MARK%d:MAX\n" , iMarkerNr ); 
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
			}
			else
			{
				sprintf( szString , ":CALC:MARK%d:MIN\n" , iMarkerNr );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
			}	
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_GetMarkerMeasure ( int hInstrumentHandle , int channel , int iMarkerNr , double *pFrequency , double *pPower )
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	int						iActualSize							=	0;
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			sprintf( szString , ":CALC:MARK%d:STAT ON\n" , iMarkerNr );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));

			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			sprintf( szString , ":CALC:MARK%d:X?\n" , iMarkerNr );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	
			SetBreakOnLibraryErrors (0);
			viRead( pLocalStorage->sessionHandle, szString , 20 , &iActualSize );
			SetBreakOnLibraryErrors (1);
				
			if ( pFrequency )
				*pFrequency = atof( szString );
			
			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			sprintf( szString , ":CALC:MARK%d:Y?\n" , iMarkerNr );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	
			SetBreakOnLibraryErrors (0);
			viRead( pLocalStorage->sessionHandle, szString , 20 , &iActualSize );
			SetBreakOnLibraryErrors (1);
				
			if ( pPower )
				*pPower = atof( szString );
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

void* DLLEXPORT SpectrumAnalyzer_PhaseNoise_GetMarker ( int hInstrumentHandle , int channel , int iMarkerNr , double lfTimeout , double *pPosition , double *pValue )
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[1025]						=	{0};

	int						iActualSize							=	0;
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	char					*pNext								=	NULL,
							*pCurrent							=	NULL,
							*pTemp								=	NULL;
	
	double					vlfDataMatrix[32][4]				=	{0.0};
	
	int						iMarkerIndex						=	0,
							iPositionIndex						=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":FETCH:LPLot6?\n" ));

			SetBreakOnLibraryErrors (0);
			viRead( pLocalStorage->sessionHandle, szString , 1024 , &iActualSize );
			SetBreakOnLibraryErrors (1);
			
			pNext = szString;
	
			while( pNext )
			{
				for( iPositionIndex = 0; iPositionIndex < 4; iPositionIndex++ )
				{
					pCurrent = pNext;
					pTemp = strchr( pNext , ',' );
				
					if (( pTemp == NULL ) || ( pCurrent == NULL ))
					{
						pNext = NULL;
						break;	
					}
					
					*pTemp = 0;
					pNext = pTemp+1;
					
					vlfDataMatrix[iMarkerIndex][iPositionIndex] = atof(pCurrent);
				}

				iMarkerIndex++;
				
				if ( iMarkerIndex >= 25 )
					break;
			};
	
			iMarkerIndex = iMarkerNr - 1;
			
			if (( iMarkerIndex >= 0 ) && ( iMarkerIndex < 25 ))
			{
				if ( pPosition )
					*pPosition = vlfDataMatrix[iMarkerIndex][0];
				
				if (( channel >= 1 ) && ( channel < 4 ))
				{
					if ( pValue )
						*pValue = vlfDataMatrix[iMarkerIndex][channel];	
				}
			}
			
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 


void* DLLEXPORT SpectrumAnalyzer_PhaseNoise_GetIntegratedMarker ( int hInstrumentHandle , int channel , int iMarkerNr , double lfTimeout ,double lfMarkerStartFreqkHz,double IntMarkerStoptFreqkHz, double *pPosition , double *pValue )
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[1025]						=	{0};

	int						iActualSize							=	0;
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	int						iCount								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_N9010B:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
									
			sprintf( szString , ":CALC:LPL:MARK:BAND:LEFT %0.2lfkHz", lfMarkerStartFreqkHz );
			CHK_VSA ( viWrite( pLocalStorage->sessionHandle , szString , strlen(szString), &iCount));  

			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 

			sprintf( szString , ":CALC:LPL:MARK:BAND:RIGHT %0.2lfkHz", IntMarkerStoptFreqkHz );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));

			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			sprintf( szString , ":CALC:LPL:MARK%d:X?\n" , iMarkerNr );
			CHK_VSA ( viWrite( pLocalStorage->sessionHandle , szString , strlen(szString), &iCount));  
	
			SetBreakOnLibraryErrors (0);
			viRead( pLocalStorage->sessionHandle, szString , 20 , &iActualSize );
			SetBreakOnLibraryErrors (1);
				
			if ( pPosition )
				*pPosition = atof( szString );
			
			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			sprintf( szString , ":CALC:LPL:MARK%d:Y?\n" , iMarkerNr );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	
			SetBreakOnLibraryErrors (0);
			viRead( pLocalStorage->sessionHandle, szString , 20 , &iActualSize );
			SetBreakOnLibraryErrors (1);
				
			if ( pValue )
				*pValue = atof( szString );
			
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_SetMaxHold ( int hInstrumentHandle , int  iCounter )
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_N9010B:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			if ( iCounter > 0 )
			{
				sprintf( szString , ":TRAC:TYPE MAXH\n" );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));  

				WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
				
				sprintf( szString , ":SENS:AVER:COUN %d\n" , iCounter );  
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
				
			}
			else
				sprintf( szString , ":TRAC:TYPE WRIT\n" );
	
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

void* DLLEXPORT SpectrumAnalyzer_PhaseNoise_SetIntegratedMarker_Start_Stop_Offsets ( int hInstrumentHandle , int iChannel , int marker , double lfTimeOut , double lfStartFreq , double lfStopFreq )
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	int						iCount								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_N9010B:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			

			sprintf( szString , ":CALC:LPL:MARK:BAND:LEFT %lf kHz", lfStartFreq );

			CHK_VSA ( viWrite( pLocalStorage->sessionHandle , szString , strlen(szString), &iCount));  

			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
				
			sprintf( szString , ":CALC:LPL:MARK:BAND:RIGH %lf kHz", lfStopFreq );

			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));

			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 

	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

void* DLLEXPORT SpectrumAnalyzer_ConfigChannelPowerMeasuring( int hInstrumentHandle , double lfBandWidth )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			sprintf( szString , ":CONF:CHP\n" );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));

			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
	
			sprintf( szString , ":SENS:CHP:BAND %E\n" , lfBandWidth );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));  
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}
  

void* DLLEXPORT SpectrumAnalyzer_GetChannelPowerResult( int hInstrumentHandle , double *plfValue )
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	int						iActualSize							=	0;
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			sprintf( szString , ":INIT:CHP\n" );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));

			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			sprintf( szString , ":MEAS:CHP?\n" );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	
			SetBreakOnLibraryErrors (0);
			viRead( pLocalStorage->sessionHandle, szString , 20 , &iActualSize );
			SetBreakOnLibraryErrors (1);
				
			if ( plfValue )
				*plfValue = atof( szString );
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_GetStartStopFrequency( int hInstrumentHandle , double *plfStartFrequency , double *plfStopFrequency )
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	int						iActualSize							=	0;
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:

			sprintf( szString , ":SENS:FREQ:STAR?\n" );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	
			SetBreakOnLibraryErrors (0);
			viRead( pLocalStorage->sessionHandle, szString , 20 , &iActualSize );
			SetBreakOnLibraryErrors (1);
				
			if ( plfStartFrequency )
				*plfStartFrequency = atof( szString );
			
			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			sprintf( szString , ":SENS:FREQ:STOP?\n" );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	
			SetBreakOnLibraryErrors (0);
			viRead( pLocalStorage->sessionHandle, szString , 20 , &iActualSize );
			SetBreakOnLibraryErrors (1);
				
			if ( plfStopFrequency )
				*plfStopFrequency = atof( szString );
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_SetStartStopFrequency( int hInstrumentHandle , double lfStartFrequency , double lfStopFrequency )
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:

			sprintf( szString , ":SENS:FREQ:STAR %0.6lf\n" , lfStartFrequency );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
			
			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			sprintf( szString , ":SENS:FREQ:STOP %0.6lf\n" , lfStopFrequency );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


void* DLLEXPORT SpectrumAnalyzer_SetRBW( int hInstrumentHandle , double lfResolutionBandWidth )
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:															 
			
			if ( lfResolutionBandWidth > 0.0 )
			{
				sprintf( szString , ":SENS:BAND:RES:AUTO OFF\n" );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 

				WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
				
				sprintf( szString , ":SENS:BAND:RES %E\n" , lfResolutionBandWidth );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 
			}
			else
			{
				sprintf( szString , ":SENS:BAND:RES:AUTO ON\n" );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 
			}
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_SetVBW( int hInstrumentHandle , double lfVideoBandWidth )
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:															 
			
			if ( lfVideoBandWidth > 0.0 )
			{
				sprintf( szString , ":SENS:BAND:VID:AUTO OFF\n" );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 

				WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
				
				sprintf( szString , ":SENS:BAND:VID %E\n" , lfVideoBandWidth );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 
			}
			else
			{
				sprintf( szString , ":SENS:BAND:VID:AUTO ON\n" );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 
			}
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_SetBW( int hInstrumentHandle , double lfResolutionBandWidth , double lfVideoBandWidth )
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:															 
			
			if ( lfResolutionBandWidth > 0.0 )
			{
				sprintf( szString , ":SENS:BAND:RES:AUTO OFF\n" );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 

				WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
				
				sprintf( szString , ":SENS:BAND:RES %E\n" , lfResolutionBandWidth );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 
			}
			else
			{
				sprintf( szString , ":SENS:BAND:RES:AUTO ON\n" );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 
			}
			
			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			if ( lfVideoBandWidth > 0.0 )
			{
				sprintf( szString , ":SENS:BAND:VID:AUTO OFF\n" );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 

				WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
				
				sprintf( szString , ":SENS:BAND:VID %E\n" , lfVideoBandWidth );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 
			}
			else
			{
				sprintf( szString , ":SENS:BAND:VID:AUTO ON\n" );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 
			}
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


void* DLLEXPORT SpectrumAnalyzer_ConfigBandWidth( int hInstrumentHandle , double lfResolutionBandWidth , double lfVideoBandWidth )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:															 
			
			if ( lfResolutionBandWidth > 0.0 )
			{
				sprintf( szString , ":SENS:BAND:RES:AUTO OFF\n" );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 

				WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
				
				sprintf( szString , ":SENS:BAND:RES %E\n" , lfResolutionBandWidth );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 
			}
			else
			{
				sprintf( szString , ":SENS:BAND:RES:AUTO ON\n" );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 
			}
			
			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			if ( lfVideoBandWidth > 0.0 )
			{
				sprintf( szString , ":SENS:BAND:VID:AUTO OFF\n" );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 

				WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
				
				sprintf( szString , ":SENS:BAND:VID %E\n" , lfVideoBandWidth );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 
			}
			else
			{
				sprintf( szString , ":SENS:BAND:VID:AUTO ON\n" );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 
			}
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_ConfigSource( int hInstrumentHandle , int bExternal )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:

			if ( bExternal > 0 ) 
			{
				sprintf( szString , ":SENS:ROSC:SOUR:TYPE EXT\n" );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
			}
			else
			{
				sprintf( szString , ":SENS:ROSC:SOUR:TYPE INT\n" );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
			}															   
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}
	


void* DLLEXPORT SpectrumAnalyzer_SetNumberOfPoints( int hInstrumentHandle , int points )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:

			sprintf( szString , ":SENS:SWE:POIN %d\n" , points );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}



void* DLLEXPORT SpectrumAnalyzer_SetScaleDev( int hInstrumentHandle , double lfScaleDev )  
{
	STD_ERROR				StdError							=	{0};
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default :
				{
						CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":DISP:WIND:TRAC:Y:SCAL:RLEV:OFFS %E\n" , lfScaleDev ));
				}
	}
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_Attenuator( int hInstrumentHandle , double lfAttenuator )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:															 
			
			if ( lfAttenuator > 0.0 )
			{
				sprintf( szString , ":SENS:POW:RF:ATT:AUTO OFF\n" );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 

				WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
				
				sprintf( szString , ":SENS:POW:RF:ATT %E\n" , lfAttenuator );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 
			}
			else
			{
				sprintf( szString , ":SENS:POW:RF:ATT:AUTO ON\n" );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 
			}
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_SetReferenceLevel( int hInstrumentHandle , double lfReferenceLevel )  
{
	STD_ERROR				StdError							=	{0};
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default :
				{
					CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":DISP:WIND:TRAC:Y:RLEV %E\n" , lfReferenceLevel ));
				}
	}
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_GetReferenceLevel( int hInstrumentHandle , double *vlfReferenceLevel )  
{
	STD_ERROR				StdError							=	{0};

	char					szString[128]						=	{0};
	
	int						iActualSize							=	0;
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default :
				{
					sprintf( szString , ":DISP:WIND:TRAC:Y:RLEV?\n" );
					CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	
					SetBreakOnLibraryErrors (0);
					viRead( pLocalStorage->sessionHandle, szString , 20 , &iActualSize );
					SetBreakOnLibraryErrors (1);
				
					if ( vlfReferenceLevel )
						*vlfReferenceLevel = atof( szString );
				}
	}
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_SetMarkerType( int hInstrumentHandle , int bDelta )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			if ( bDelta > 0 )
				sprintf( szString , ":CALC:MARK:MODE DELT\n" );
			else
				sprintf( szString , ":CALC:MARK:MODE POS\n" );
			
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_SetMarkerFrequency( int hInstrumentHandle , int iMarkerNr , double lfFrequency )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			sprintf( szString , ":CALC:MARK%d:STAT ON\n" , iMarkerNr );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));

			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			if ( lfFrequency > 0.0 )
			{
				sprintf( szString , ":CALC:MARK%d:X %E\n" , iMarkerNr , lfFrequency );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
			}
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_SetMarker ( int hInstrumentHandle, int MarkerSetType , int iMarkerNr ,double lfMarkerFreq )
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			sprintf( szString , ":CALC:MARK%d:STAT ON\n" , iMarkerNr );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
			
			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			if ( MarkerSetType > 0 )
				sprintf( szString , ":CALC:MARK%d:MODE DELT\n" , iMarkerNr );
			else
				sprintf( szString , ":CALC:MARK%d:MODE POS\n" , iMarkerNr );
			
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString )); 
			
			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			if ( lfMarkerFreq > 0.0 )
			{
				sprintf( szString , ":CALC:MARK%d:X %E\n" , lfMarkerFreq );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
			}
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}
	

void* DLLEXPORT SpectrumAnalyzer_SearchMarkerPeak( int hInstrumentHandle , int iMarkerNr )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			sprintf( szString , ":CALC:MARK%d:STAT ON\n" , iMarkerNr );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
			
			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			sprintf( szString , ":CALC:MARK%d:MAX\n" , iMarkerNr );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}
	
void* DLLEXPORT SpectrumAnalyzer_SearchMarkerNextPeak( int hInstrumentHandle , int iMarkerNr )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			sprintf( szString , ":CALC:MARK%d:STAT ON\n" , iMarkerNr );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
			
			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			sprintf( szString , ":CALC:MARK%d:MAX:NEXT\n" , iMarkerNr );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}																		

void* DLLEXPORT SpectrumAnalyzer_GetMarkerFrequency( int hInstrumentHandle , int iMarkerNr , double *vlfFrequency )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	int						iActualSize							=	0;
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			sprintf( szString , ":CALC:MARK%d:STAT ON\n" , iMarkerNr );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));

			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			sprintf( szString , ":CALC:MARK%d:X?\n" , iMarkerNr );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	
			SetBreakOnLibraryErrors (0);
			viRead( pLocalStorage->sessionHandle, szString , 20 , &iActualSize );
			SetBreakOnLibraryErrors (1);
				
			if ( vlfFrequency )
				*vlfFrequency = atof( szString );
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

void* DLLEXPORT SpectrumAnalyzer_GetMarkerAmplitude( int hInstrumentHandle , int iMarkerNr , double *vlfAmplitude )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	int						iActualSize							=	0;
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			sprintf( szString , ":CALC:MARK%d:STAT ON\n" , iMarkerNr );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));

			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			sprintf( szString , ":CALC:MARK%d:Y?\n" , iMarkerNr );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	
			SetBreakOnLibraryErrors (0);
			viRead( pLocalStorage->sessionHandle, szString , 20 , &iActualSize );
			SetBreakOnLibraryErrors (1);
				
			if ( vlfAmplitude )
				*vlfAmplitude = atof( szString );
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


void* DLLEXPORT SpectrumAnalyzer_GetMarkerFrequencyCounter( int hInstrumentHandle , int iMarkerNr , double *vlfFrequency )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	int						iActualSize							=	0;
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			sprintf( szString , ":CALC:MARK%d:STAT ON\n" , iMarkerNr );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));

			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":INIT:REST\n" ));

			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			sprintf( szString , ":CALC:MARK%d:FCO:X?\n" , iMarkerNr );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	
			SetBreakOnLibraryErrors (0);
			viRead( pLocalStorage->sessionHandle, szString , 20 , &iActualSize );
			SetBreakOnLibraryErrors (1);
				
			if ( vlfFrequency )
				*vlfFrequency = atof( szString );
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_MeasureMarkerFrequencyCounter( int hInstrumentHandle , int iMarkerNr , double lfTimeout , double *vlfFrequency )  
{
	STD_ERROR               StdError                  			=   {0};        

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	char					szString[128]						=	{0},
							szReadBuffer[128]					=	{0};

	int						iNumberOfBytesReaded				=	0;
	
	double					lfFrequency							=	0.0;
	
	double					lfCurrentTime						=	0.0,
							lfStartTime							=	0.0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			sprintf( szString , ":CALC:MARK%d:STAT ON\n" , iMarkerNr );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));

			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			sprintf( szString , ":CALC:MARK%d:FCO OFF\n" , iMarkerNr ); 
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	
			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			sprintf( szString , ":CALC:MARK%d:FCO ON\n" , iMarkerNr ); 
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	
			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout );
			
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":INIT:REST\n" )); 
			
			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			GetCurrentDateTime( &lfStartTime );
	
			do
			{
				sprintf( szString , ":CALC:MARK%d:FCO:X?\n" , iMarkerNr );
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	
				SetBreakOnLibraryErrors (0);
				viRead( pLocalStorage->sessionHandle, szReadBuffer , 128 , &iNumberOfBytesReaded );
				SetBreakOnLibraryErrors (1);
			
				lfFrequency = atof( szReadBuffer ); 
	
				if (( lfFrequency < 200E9 ) && ( lfFrequency > 0.0 ))
					break;
		
				GetCurrentDateTime( &lfCurrentTime );
		
			} while(( lfCurrentTime - lfStartTime ) < lfTimeout );
	}
	
Error: 

	if ( vlfFrequency )
		*vlfFrequency = lfFrequency;
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


void* DLLEXPORT SpectrumAnalyzer_GetMarkerDeltaFrequency( int hInstrumentHandle , double *vlfFrequency )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	int						iActualSize							=	0;
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			sprintf( szString , ":CALC:MARK:X?\n" );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	
			SetBreakOnLibraryErrors (0);
			viRead( pLocalStorage->sessionHandle, szString , 20 , &iActualSize );
			SetBreakOnLibraryErrors (1);
				
			if ( vlfFrequency )
				*vlfFrequency = atof( szString );
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_GetMarkerDeltaAmplitude( int hInstrumentHandle , double *vlfAmplitude )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	int						iActualSize							=	0;
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			sprintf( szString , ":CALC:MARK:Y?\n" );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	
			SetBreakOnLibraryErrors (0);
			viRead( pLocalStorage->sessionHandle, szString , 20 , &iActualSize );
			SetBreakOnLibraryErrors (1);
				
			if ( vlfAmplitude )
				*vlfAmplitude = atof( szString );
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


void* DLLEXPORT SpectrumAnalyzer_SetCenterFrequencyFromMarker( int hInstrumentHandle , int iMarkerNr )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			sprintf( szString , ":CALC:MARK%d:STAT ON\n" , iMarkerNr );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
			
			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			sprintf( szString , ":CALC:MARK%d:SET:CENT\n" , iMarkerNr );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_SetReferenceLevelFrequencyFromMarker( int hInstrumentHandle , int iMarkerNr )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			sprintf( szString , ":CALC:MARK%d:STAT ON\n" , iMarkerNr );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
			
			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
			
			sprintf( szString , ":CALC:MARK%d:SET:RLEV\n" , iMarkerNr );
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

void* DLLEXPORT SpectrumAnalyzer_WaitForIntrumentDone( int hInstrumentHandle , double lfTimeOut )  
{
	STD_ERROR               StdError                  			=   {0};        

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout ,  lfTimeOut );  
			
			break;
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_SetSweepMode( int hInstrumentHandle , int bContinues )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			if ( bContinues > 0 )
				sprintf( szString , ":INIT:CONT ON\n" );
			else
				sprintf( szString , ":INIT:CONT OFF\n" );
	
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 


void* DLLEXPORT SpectrumAnalyzer_GetSweep( int hInstrumentHandle , double lfTimeOut )
{
	STD_ERROR               StdError                  			=   {0};        

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	char					szReadFeedback[STD_STRING]			=	{0};
															
	int						count								=	0;

	double					lfSweepTime							=	0.0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , "SENS:SWE:TIME?\n" ));
			DelayWithEventProcessing( pLocalStorage->lfDelay );
			
			SetBreakOnLibraryErrors (0); 
			viRead( pLocalStorage->sessionHandle, szReadFeedback , LOW_STRING , &count );
			SetBreakOnLibraryErrors (1); 
	
			lfSweepTime = atof(szReadFeedback);
			
			if ( lfSweepTime > lfTimeOut )
				lfSweepTime = lfTimeOut;
			
			DelayWithEventProcessing(lfSweepTime);
			
			WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout );
			
			break;
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_InitiateMeasure( int hInstrumentHandle )  
{
	STD_ERROR               StdError                  			=   {0};        

	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":INIT:IMM\n" ));
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_SetAverage( int hInstrumentHandle , int iAverage )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[512]						=	{0};
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			if ( iAverage )
			{
				sprintf( szString , ":SENS:AVER:TYPE:AUTO ON\n" );

				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));  
				
				sprintf( szString , ":SENS:AVER:COUN %d\n" , iAverage ); 
			}
			else
			{
				sprintf( szString , ":SENS:AVER:TYPE:AUTO OFF\n" );
			}
	
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


void* DLLEXPORT SpectrumAnalyzer_SendFile( int hInstrumentHandle ,char *szLocalFileName ,char *szStateFileName  )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szLoadSpectrumState[512]			=	{0},
							szDirectoryPath[512]				=	{0},
							szFileSize[32]						=	{0};
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	int						iFileSize							=	0,
							iReadSize							=	0, 
							iWriteCount							=	0,
							iFileHandle							=	0;
	
	char					*pFileBuffer						=	NULL,
							*pTemp								=	NULL; 
							
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	IF (( szLocalFileName == NULL ), "File Name is empty" );
	
	IF (( szStateFileName == NULL ), "State File Name is empty" ); 
	
	IF (( FileExists(szLocalFileName , &iFileSize) == 0 ), "Local File Name is not exists." );     
	
	sprintf( szFileSize , "%d" , iFileSize );
	
	CALLOC_ERR( pFileBuffer ,iFileSize , sizeof(char)); 
	
	iFileHandle = OpenFile ( szLocalFileName , VAL_READ_ONLY, VAL_TRUNCATE, VAL_BINARY);
	
	IF ( ( iFileHandle == 0 ) , "Can't open local file." );
	
	iReadSize = ReadFile ( iFileHandle , pFileBuffer , iFileSize );

	IF ((( iReadSize != iFileSize ) || ( pFileBuffer == NULL ) ), "Read File error" ); 
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:		

			viPrintf( pLocalStorage->sessionHandle , "*CLS\n" );
			
			strcpy( szDirectoryPath , szStateFileName );
			
			pTemp = strrchr( szDirectoryPath , '\\' );
			
			if ( pTemp )
			{
				*pTemp = 0;
			}
			
			if ( strstr( szDirectoryPath , ":\\" ))
			{
				RecursiveMakeDirectory( pLocalStorage->sessionHandle , szDirectoryPath );
			}
			
			viPrintf( pLocalStorage->sessionHandle , "*CLS\n" );
			
			sprintf( szLoadSpectrumState , ":MMEM:DATA \"%s\",#%d%s" , szStateFileName , strlen(szFileSize) , szFileSize );   
	
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szLoadSpectrumState ));
			
			CHK_VSA ( viBufWrite( pLocalStorage->sessionHandle , pFileBuffer , iFileSize ,&iWriteCount ));
			
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , "\n" )); 
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	FREE(pFileBuffer);
	
	if ( iFileHandle )
		CloseFile ( iFileHandle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

void* DLLEXPORT Equipment_SendStateFile ( int hInstrumentHandle , char *szLocalFileName ,char *szStateFileName  )
{
	return SpectrumAnalyzer_SendFile( hInstrumentHandle , szLocalFileName , szStateFileName );	
}

void* DLLEXPORT SpectrumAnalyzer_ReceiveFile( int hInstrumentHandle ,char *szLocalFileName ,char *szStateFileName  )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szLoadSpectrumState[512]			=	{0},
							szReadFeedback[LOW_STRING]			=	{0};
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	int						iWriteSize						 	=	0,  
							iFileHandle							=	0;
	
	char					*pReadBuffer						=	NULL; 
							
	unsigned int			uiCount								=	0,
							uiNumberOfBinaryData				=	0;
							
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	IF (( szLocalFileName == NULL ), "File Name is empty" );
	
	IF (( szStateFileName == NULL ), "State File Name is empty" ); 
	
	iFileHandle = OpenFile ( szLocalFileName , VAL_WRITE_ONLY, VAL_TRUNCATE, VAL_BINARY);
	
	IF ( ( iFileHandle == 0 ) , "Can't open local file." );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:		

			viPrintf( pLocalStorage->sessionHandle , "*CLS\n" );
			
			sprintf( szLoadSpectrumState , ":MMEM:DATA? \"%s\"\n" , szStateFileName );   
	
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szLoadSpectrumState ));
			
			do 
			{
				SetBreakOnLibraryErrors (0); 
				viRead ( pLocalStorage->sessionHandle ,(ViPBuf)szReadFeedback , 1 , &uiCount );
				SetBreakOnLibraryErrors (1); 

				if ( szReadFeedback[0] == '#' )
					break;

			} while(uiCount);

			IF (( szReadFeedback[0] != '#' ) , "Wrong trace reading format" );  

			memset( szReadFeedback , 0 , LOW_STRING );

			SetBreakOnLibraryErrors (0); 
			viRead ( pLocalStorage->sessionHandle ,(ViPBuf)szReadFeedback , 1 , &uiCount );
			SetBreakOnLibraryErrors (1); 

			IF (( uiCount == 0 ) , "Wrong trace reading format" );  

			uiCount = atoi(szReadFeedback); 

			memset( szReadFeedback , 0 , LOW_STRING );

			SetBreakOnLibraryErrors (0); 
			viRead ( pLocalStorage->sessionHandle ,(ViPBuf)szReadFeedback , uiCount , &uiCount );  
			SetBreakOnLibraryErrors (1); 

			uiNumberOfBinaryData = atoi(szReadFeedback); 

			SetBreakOnLibraryErrors (0);
	
			CALLOC_ERR( pReadBuffer , uiNumberOfBinaryData , sizeof(char)); 
	
			viRead ( pLocalStorage->sessionHandle ,(ViPBuf)pReadBuffer , uiNumberOfBinaryData , &uiCount ); 
	
			SetBreakOnLibraryErrors (1);

			IF (( uiNumberOfBinaryData != uiCount ) , "Wrong trace reading format" );     

			iWriteSize = WriteFile ( iFileHandle , pReadBuffer , uiNumberOfBinaryData );

			IF (( iWriteSize != uiNumberOfBinaryData ) , "Write File error" ); 
	
			SetBreakOnLibraryErrors (0);
			viRead ( pLocalStorage->sessionHandle ,(ViPBuf)szReadFeedback , 1 , &uiCount );  
			SetBreakOnLibraryErrors (1);
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	FREE(pReadBuffer);
	
	if ( iFileHandle )
		CloseFile ( iFileHandle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

void* DLLEXPORT Equipment_ReceiveStateFile ( int hInstrumentHandle , char *szLocalFileName ,char *szStateFileName  )
{
	return SpectrumAnalyzer_ReceiveFile( hInstrumentHandle , szLocalFileName , szStateFileName );	
}



void* DLLEXPORT SpectrumAnalyzer_GetFileCatalog ( int hInstrumentHandle , char *szPath , char ***szFileNames , int **pvFileSizes , int *piNumberOfFiles )
{
	STD_ERROR               StdError                  			=   {0};        

	char					szLoadSpectrumCatalog[512]			=	{0},
							szPreviousFeedback[512]				=	{0},
							szReadFeedback[LOW_STRING]			=	{0}, 
							szCurrentBuffer[512]				=	{0},
							szLoadPath[512]						=	{0};  
																	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	int						iFileIndex							=	0;
	
	char					*pTemp								=	NULL,
							*pCurrent							=	NULL,
							*pNext								=	NULL; 
							
	int 					iLength								=	0, 
							iNumberOfFile						=	0,
							iAllocatedMemory					=	0;

	unsigned int			uiCount								=	0,
							uiCurrentCount						=	0,
							uiPreviousCount						=	0;
								
	int						iTimeout							=	0;
							
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:		

			viPrintf( pLocalStorage->sessionHandle , "*CLS\n" );

			if ( szPath )
			{
				strcpy( szLoadPath , szPath );
				
				iLength = strlen(szLoadPath);
				
				if ( szLoadPath[iLength-1] != '\\' )
					strcat( szLoadPath , "\\" );
				
				sprintf( szLoadSpectrumCatalog , ":MMEM:CAT? \"%s\"\n" , szLoadPath );   							
			}
			else
				sprintf( szLoadSpectrumCatalog , ":MMEM:CAT? \"C:\\\"\n" );   
			
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szLoadSpectrumCatalog ));
			
			viGetAttribute ( pLocalStorage->sessionHandle , VI_ATTR_TMO_VALUE , &iTimeout );    
	
			viSetAttribute ( pLocalStorage->sessionHandle , VI_ATTR_TMO_VALUE , 2000 );  

			do
			{
				strcpy( szCurrentBuffer , szPreviousFeedback );
		
				memset( szReadFeedback , 0 , LOW_STRING );

				SetBreakOnLibraryErrors (0); 
				viRead ( pLocalStorage->sessionHandle ,(ViPBuf)szReadFeedback , (LOW_STRING-1) , &uiCount );  
				SetBreakOnLibraryErrors (1); 
	
				if (( uiCount == 0 ) && ( uiPreviousCount == uiCurrentCount ))
					break;
		
				strcat( szCurrentBuffer , szReadFeedback ); 
		   
				uiPreviousCount = uiCurrentCount;
		
				uiCurrentCount = strlen( szCurrentBuffer );  
		
				pNext = szCurrentBuffer;
		
				do
				{
					strcpy( szPreviousFeedback , pNext );
			
					pTemp = strstr( pNext , ",\"" );
			
					if ( pTemp == NULL )
						break;
			
					pCurrent = pTemp+2;
			
					pTemp = strstr( pCurrent , ",," );   
			
					if ( pTemp == NULL )
						break;
			
					*pTemp = 0;
			
					pNext = pTemp+2;
			
					iNumberOfFile++;
			
					if ( iNumberOfFile >= iAllocatedMemory )
					{
						iAllocatedMemory = ( iAllocatedMemory * 2 ) + 8;
				
						if ( szFileNames )
						{
							*szFileNames = realloc( *szFileNames , ( iAllocatedMemory * sizeof(char*)));
						}
				
						if ( pvFileSizes )
						{
							*pvFileSizes = realloc( *pvFileSizes , ( iAllocatedMemory * sizeof(int))); 
						}
					}
			
					CALLOC_COPY_STRING( (*szFileNames)[iFileIndex] , pCurrent );
			
					if ( pvFileSizes )
					{
						if ( strchr( pCurrent , '[' ) == NULL )
							(*pvFileSizes)[iFileIndex] = atoi(pNext);
						else
							(*pvFileSizes)[iFileIndex] = -1;
					}
			
					iFileIndex++;
			
				} while(1);
		
			} while(1);
	}

Error: 

	if ( piNumberOfFiles )
		*piNumberOfFiles = iNumberOfFile;
	
	if ( handle )
	{
		if ( pLocalStorage->sessionHandle )
			viSetAttribute ( pLocalStorage->sessionHandle , VI_ATTR_TMO_VALUE , iTimeout );  
		
		CmtReleaseTSVPtr ( handle );  
	}
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT Equipment_GetFileCatalog( int hInstrumentHandle , char *szPath , char ***szFileNames , int **pvFileSizes , int *piNumberOfFiles )  
{
	return SpectrumAnalyzer_GetFileCatalog( hInstrumentHandle , szPath , szFileNames , pvFileSizes , piNumberOfFiles );	
}

void* DLLEXPORT Equipment_MakeFileCatalog( int hInstrumentHandle , char *szPath )  
{
	return RecursiveMakeDirectory( hInstrumentHandle , szPath );	
}
   
void* DLLEXPORT SpectrumAnalyzer_DeleteFile( int hInstrumentHandle , char *szStateFileName  )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szDeleteSpectrumState[512]			=	{0},
							szMessage[LOW_STRING]				=	{0};
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;

	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );

	IF (( szStateFileName == NULL ), "State File Name is empty" ); 
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:		

			viPrintf( pLocalStorage->sessionHandle , "*CLS\n" );

			sprintf( szDeleteSpectrumState , ":MMEM:DEL \"%s\"\n" , szStateFileName );   
			
			sprintf( szMessage , "Are you sure you want to delete state file\n\"%s\"?" , szStateFileName );
	
			if ( ConfirmPopup ("Deleting Equipment State File", szMessage) )
			{
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szDeleteSpectrumState ));
			}
		
	}

Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT Equipment_DeleteStateFile ( int hInstrumentHandle , char *szStateFileName  )
{
	return SpectrumAnalyzer_DeleteFile( hInstrumentHandle , szStateFileName );	
}

void* DLLEXPORT SpectrumAnalyzer_DeleteFileCatalog( int hInstrumentHandle , char *szCatalogName  )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szDeleteSpectrumCatalog[512]		=	{0},
							szMessage[LOW_STRING]				=	{0};
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;

	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );

	IF (( szCatalogName == NULL ), "State File Name is empty" ); 
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:		

			viPrintf( pLocalStorage->sessionHandle , "*CLS\n" );

			sprintf( szDeleteSpectrumCatalog , ":MMEM:RDIR \"%s\"\n" , szCatalogName );   
			
			sprintf( szMessage , "Are you sure you want to delete catalog \n\"%s\"?" , szCatalogName );
	
			if ( ConfirmPopup ("Deleting Equipment State File", szMessage) )
			{
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szDeleteSpectrumCatalog ));
			}
		
	}

Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT Equipment_DeleteFileCatalog ( int hInstrumentHandle , char *szCatalogName  )
{
	return SpectrumAnalyzer_DeleteFileCatalog( hInstrumentHandle , szCatalogName );	
}

void* DLLEXPORT SpectrumAnalyzer_RecallState( int hInstrumentHandle ,char *szFileName )  
{
	STD_ERROR           		    StdError                  						=   {0};        

	char							szLoadSpectrumState[512]						=	{0},
									szReadBuffer[LOW_STRING]						=	{0};
	
	tsHandle						*pLocalStorage									=	{0};

	int								handle											=	0;
	
	int								iError											=	0;
	
	char							*pFileNamePath									=	NULL;
	
	tsSTD_CallBackSet				tSTD_CallBackSet								=	{0};
	
	int								iTryIndex										=	0;
	
	int								count											=	0;
	
	int								bGetLock										=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	bGetLock = 1;
	
	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks);   
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			for ( iTryIndex = 0; iTryIndex < 3; iTryIndex++ )
			{
				viPrintf( pLocalStorage->sessionHandle , "*CLS\n" );
			
				sprintf( szLoadSpectrumState , ":MMEM:LOAD:STAT \"%s\"\n" , szFileName );
	
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szLoadSpectrumState ));
			
				DelayWithEventProcessing(pLocalStorage->lfStateFileDelay);
			
				viPrintf( pLocalStorage->sessionHandle , ":SYST:ERR?\n" );
	
				SetBreakOnLibraryErrors (0);
				viRead( pLocalStorage->sessionHandle, szReadBuffer , LOW_STRING , &count );
				SetBreakOnLibraryErrors (1);
	
				iError = atoi(szReadBuffer);
	
				if ( iError != -256 )
				{
					FREE_CALLOC_COPY_STRING(pLocalStorage->pLastStateFile,szFileName); 
					
					break;
				}
				
				STDF_GET_EQUIPMENT_STATE_FILE( szFileName , pFileNamePath );
				
				CHK_CMT ( CmtReleaseTSVPtr ( handle )); 
				bGetLock = 0;
				
				FREE_STDERR_COPY_ERR( SpectrumAnalyzer_SendFile( handle , pFileNamePath , szFileName ));    
				
				FREE(pFileNamePath);
				
				CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
				bGetLock = 1;
			}																   
	}
	
Error: 

	if ( handle && bGetLock )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

void* DLLEXPORT SpectrumAnalyzer_SaveState( int hInstrumentHandle ,char *szFileName )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szLoadSpectrumState[512]			=	{0};
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			viPrintf( pLocalStorage->sessionHandle , "*CLS\n" );
			
			sprintf( szLoadSpectrumState , ":MMEM:STOR:STAT \"%s\"\n" , szFileName );   
	
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szLoadSpectrumState ));
			
			FREE_CALLOC_COPY_STRING(pLocalStorage->pLastStateFile,szFileName); 
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 


void* DLLEXPORT SpectrumAnalyzer_Recall( int hInstrumentHandle ,char *szFileName )  
{
	return SpectrumAnalyzer_RecallState( hInstrumentHandle ,szFileName );
} 

void* DLLEXPORT   SpectrumAnalyzer_GetLastStateFileName(int hInstrumentHandle, char **pszFileName )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalStorage						=	{0};

	int				handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	IF (( pszFileName == NULL ) , "State is empty" ); 
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	FREE_CALLOC_COPY_STRING(*pszFileName,pLocalStorage->pLastStateFile); 
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_Read_EVM( int hInstrumentHandle , double *plfEVM_Value )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	int						iActualSize							=	0;
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			sprintf( szString , ":FETCH:DDEMod?\n" );
	
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	
			SetBreakOnLibraryErrors (0);
			viRead( pLocalStorage->sessionHandle, szString , 20 , &iActualSize );
			SetBreakOnLibraryErrors (1);
				
			if ( plfEVM_Value )
				*plfEVM_Value = atof(szString);
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

void* DLLEXPORT SpectrumAnalyzer_Read_IP3( int hInstrumentHandle , double *plfIP3_Value )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	int						iActualSize							=	0;
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			sprintf( szString , ":FETCH:TOI:IP3?\n" );
	
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	
			SetBreakOnLibraryErrors (0);
			viRead( pLocalStorage->sessionHandle, szString , 20 , &iActualSize );
			SetBreakOnLibraryErrors (1);
				
			if ( plfIP3_Value )
				*plfIP3_Value = atof(szString);
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

void* DLLEXPORT SpectrumAnalyzer_Read_IP2( int hInstrumentHandle , double *plfIP2_Value )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	int						iActualSize							=	0;
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			sprintf( szString , ":FETCH:TOI:IP2?\n" );
	
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	
			SetBreakOnLibraryErrors (0);
			viRead( pLocalStorage->sessionHandle, szString , 20 , &iActualSize );
			SetBreakOnLibraryErrors (1);
				
			if ( plfIP2_Value )
				*plfIP2_Value = atof(szString);
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 
 

void* DLLEXPORT SpectrumAnalyzer_Read_Harmonics_Distortion( int hInstrumentHandle , double *plfTHD_Value )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	int						iActualSize							=	0;
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			sprintf( szString , ":FETCH:HARM1?\n" );
	
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	
			SetBreakOnLibraryErrors (0);
			viRead( pLocalStorage->sessionHandle, szString , 20 , &iActualSize );
			SetBreakOnLibraryErrors (1);
				
			if ( plfTHD_Value )
				*plfTHD_Value = atof(szString);
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 
	 
void* DLLEXPORT SpectrumAnalyzer_Read_Harmonics_dBc( int hInstrumentHandle , double *plfTHD_Value )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	int						iActualSize							=	0;
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
			sprintf( szString , ":FETCH:HARM2?\n" );
	
			CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	
			SetBreakOnLibraryErrors (0);
			viRead( pLocalStorage->sessionHandle, szString , 20 , &iActualSize );
			SetBreakOnLibraryErrors (1);
				
			if ( plfTHD_Value )
				*plfTHD_Value = atof(szString);
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

void* DLLEXPORT SpectrumAnalyzer_Read_Harmonics_Amplitude( int hInstrumentHandle , int iHarmonicIndex , double *plfValue )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	int						iActualSize							=	0;
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
				sprintf( szString , ":FETCH:HARM:AMPL%d?\n" , (iHarmonicIndex+1) );     
	
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	
				SetBreakOnLibraryErrors (0);
				viRead( pLocalStorage->sessionHandle, szString , 20 , &iActualSize );
				SetBreakOnLibraryErrors (1);
	
				if ( plfValue )
					*plfValue = atof(szString);
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
		
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

void* DLLEXPORT SpectrumAnalyzer_Read_Spurious_Emissions_List( int hInstrumentHandle , int *piNumberOfSpurious , int **pvRangeNumber , int **pvPassFail , double **pvlfSpurFrequency , double **pvlfSpurAmplitude , double **pvlfSpurLimit )  
{
	STD_ERROR               StdError                  			=   {0};        

	int						iActualSize							=	0;
	
	int						iSpurIndex							=	0,
							iParamIndex							=	0;
							
	int						iNumberOfSpurious					=	0,
							iSpurNumber							=	0,
							iRangeNumber						=	0,
							bPassFail							=	0;
	
	double					lfSpurFrequency						=	0.0,
							lfSpurAmplitude						=	0.0, 
							lfSpurLimit							=	0.0;

	char					*pTemp								=	NULL,
							*pString							=	NULL,
							*pNext								=	NULL;
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	int						iTimeout							=	0;
	
	char					*pRespondsString					=	NULL;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CALLOC_ERR( pRespondsString , 10240 , sizeof(char));
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));

	viGetAttribute ( pLocalStorage->sessionHandle , VI_ATTR_TMO_VALUE , &iTimeout );   
	
	SetBreakOnLibraryErrors (0); 
	
	viSetAttribute ( pLocalStorage->sessionHandle , VI_ATTR_TMO_VALUE , (2E3) );  
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default:
			
				CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":FETCH:SPUR?\n" ));
	
				SetBreakOnLibraryErrors (0);
				viRead( pLocalStorage->sessionHandle, pRespondsString , (10239) , &iActualSize );
				SetBreakOnLibraryErrors (1);
						
				pString = pRespondsString;
	
				pTemp = strchr( pString , ',' );
	
				IF (( pTemp == NULL ) , "No Data found." );
		
				*pTemp = 0;
				pNext = pTemp+1;
	
				iNumberOfSpurious = atof(pString);
	
				if ( piNumberOfSpurious )
					*piNumberOfSpurious = iNumberOfSpurious;
	
				if ( pvRangeNumber )
				{
					CALLOC_ERR( *pvRangeNumber , iNumberOfSpurious , sizeof(int) );
				}
	
				if ( pvPassFail )
				{
					CALLOC_ERR( *pvPassFail , iNumberOfSpurious , sizeof(int) );
				}
	
				if ( pvlfSpurFrequency )
				{
					CALLOC_ERR( *pvlfSpurFrequency , iNumberOfSpurious , sizeof(double) );
				}
	
				if ( pvlfSpurAmplitude )
				{
					CALLOC_ERR( *pvlfSpurAmplitude , iNumberOfSpurious , sizeof(double) );
				}
	
				if ( pvlfSpurLimit )
				{
					CALLOC_ERR( *pvlfSpurLimit , iNumberOfSpurious , sizeof(double) );
				}
	
				for ( iSpurIndex = 0; iSpurIndex < iNumberOfSpurious; iSpurIndex++ )
				{
					for ( iParamIndex = 0; iParamIndex < 6; iParamIndex++ )
					{
						pString = pNext;
		
						pTemp = strchr( pString , ',' );
		
						if ( pTemp == NULL )
							break;
		
						*pTemp = 0;
						pNext = pTemp+1;
	
						switch(iParamIndex)
						{
							case 0:
								iSpurNumber = atof(pString); 
								break;
							case 1:
								iRangeNumber = atof(pString); 
								break;
							case 2:
								lfSpurFrequency = atof(pString); 
								break;
							case 3:
								lfSpurAmplitude = atof(pString); 
								break;		   
							case 4:
								lfSpurLimit = atof(pString); 
								break;
							case 5:
								bPassFail = atof(pString); 
								break;
						}
					}
		
					if ( pvRangeNumber )
					{
						( *pvRangeNumber )[iSpurIndex] = iRangeNumber;
					}
	
					if ( pvPassFail )
					{
						( *pvPassFail )[iSpurIndex] = bPassFail;
					}
	
					if ( pvlfSpurFrequency )
					{
						( *pvlfSpurFrequency )[iSpurIndex] = lfSpurFrequency;
					}
	
					if ( pvlfSpurAmplitude )
					{
						( *pvlfSpurAmplitude )[iSpurIndex] = lfSpurAmplitude;
					}
	
					if ( pvlfSpurLimit )
					{
						( *pvlfSpurLimit )[iSpurIndex] = lfSpurLimit;
					}
				}
	}
	
Error: 
	
	if ( handle )
	{
		viSetAttribute ( pLocalStorage->sessionHandle , VI_ATTR_TMO_VALUE , iTimeout ); 
		
		CmtReleaseTSVPtr ( handle );
	}
	
	SetBreakOnLibraryErrors (1);
	
	FREE(pRespondsString);
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

void* DLLEXPORT SpectrumAnalyzer_SetToMode_Specrum( int hInstrumentHandle )  
{
	STD_ERROR				StdError							=	{0};
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default :
				{
						CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":INST:SEL %s\n" , "SA" ));
				}
	}
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_SetFrequency_Specrum( int hInstrumentHandle , double lfFrequency )  
{
	STD_ERROR				StdError							=	{0};
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default :
				{
					CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:CENT %E\n" , lfFrequency ));
				}
	}
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_SetFrequency( int hInstrumentHandle , double lfFrequency )  
{
	STD_ERROR				StdError							=	{0};
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default :
				{
						CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:CENT %E\n" , lfFrequency ));
				}
	}
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


void* DLLEXPORT SpectrumAnalyzer_GetFrequency( int hInstrumentHandle , double *plfFrequency )  
{
	STD_ERROR				StdError							=	{0};
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;

	char					szString[128]						=	{0};

	int						iActualSize							=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default :
				{
						CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":SENS:FREQ:CENT?\n" ));
						
						SetBreakOnLibraryErrors (0);
						viRead( pLocalStorage->sessionHandle, szString , 20 , &iActualSize );
						SetBreakOnLibraryErrors (1);
	
						if ( plfFrequency )
							*plfFrequency = atof(szString);
				}
	}
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_SetFrequency_EVM( int hInstrumentHandle , double lfFrequency )  
{
	STD_ERROR				StdError							=	{0};
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default :
				{
					CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":FREQ:CENT %E\n" , lfFrequency ));
				}
	}
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_RestartMeasure( int hInstrumentHandle )  
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalStorage						=	{0};

	int				handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default :
				{
						CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":INIT:REST\n" ));
				}
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER;
}
	
void* DLLEXPORT SpectrumAnalyzer_Read_SymbolStream( int hInstrumentHandle , int iNumberOfSymbols , char **pSymbolStream , int *piNumberOfSymbolsRead )  
{
	STD_ERROR               StdError                  			=   {0};        
	
	char					szString[128]						=	{0};

	char					*pszBuffer							=	NULL,
							*pszSymbolSream						=	NULL,
							*pszOutputSymbolSream					=	NULL,
							*pTemp								=	NULL,
							*pNext								=	NULL,
							*pCurrent							=	NULL;

	int						iSymbolCounter						=	0,
							iNumberOfSymbolsRead				=	0; 
					
	int						iTryMeasureIndex					=	0;
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default :
			
					iNumberOfSymbols *= 16;
	
					CALLOC_ERR( pszBuffer , iNumberOfSymbols+1 , sizeof(char));
					CALLOC_ERR( pszSymbolSream , iNumberOfSymbols+1 , sizeof(char)); 
	
					for( iTryMeasureIndex = 0; iTryMeasureIndex < 5; iTryMeasureIndex++ )
					{
						sprintf( szString , ":CALC:DDEM:DATA4?\n" );
	
						CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , szString ));
	
						if ( IS_NOT_OK )
							continue;
		
						DelayWithEventProcessing(0.5);
	
						SetBreakOnLibraryErrors (0);
						viRead( pLocalStorage->sessionHandle, pszBuffer , iNumberOfSymbols , &iNumberOfSymbolsRead );
						SetBreakOnLibraryErrors (1);
	
						if ( iNumberOfSymbolsRead > 2 )
							break;
					}
	
					CHK_STD_ERROR;
	
					pNext = pszBuffer;
	
					do
					{
						pCurrent = pNext;
						pTemp = strchr( pNext , ',' );
		
						if ( pTemp )
						{
							*pTemp = 0;
							pNext = pTemp+1;
						}
		
						if ( pCurrent )
						{
							pszSymbolSream[iSymbolCounter++] = atoi(pCurrent);	
						}
		
					} while( pTemp );
		
					CALLOC_ERR( pszOutputSymbolSream , iSymbolCounter+1 , sizeof(char)); 
	
					memcpy(pszOutputSymbolSream,pszSymbolSream,iSymbolCounter);
	
					if ( pSymbolStream )
						*pSymbolStream = pszOutputSymbolSream;
	
					if ( piNumberOfSymbolsRead )
						*piNumberOfSymbolsRead = iSymbolCounter;
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
					
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

void* DLLEXPORT SpectrumAnalyzer_SetOffset( int hInstrumentHandle , double lfOffset )  
{
	STD_ERROR				StdError							=	{0};
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default :
				{
						CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":DISP:WIND:TRAC:Y:RLEV:OFFS %E\n" , lfOffset ));
				}
	}
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_SetTriggerSource( int hInstrumentHandle , int iType )  
{
	STD_ERROR				StdError							=	{0};
	
	tsHandle				*pLocalStorage						=	{0};

	int						handle								=	0;
	
	char					szvValues[][5]						=	{"IMM","EXT1","EXT2","VID","RFB"};
												
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	IF (( iType >= (sizeof(szvValues)/sizeof(szvValues[0]))) , "The parameter is out of range." );  
		
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	switch ( pLocalStorage->tType )
	{
		case MODEL_TYPE_N9030A:
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default :
				{
					CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":TRIG:SOUR %s\n" , szvValues[iType] ));    
				}
	}
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_GetTrace( int hInstrumentHandle , int timeout , double **pTrace , int numberOfPoints , int *piNumberOfPoints )
{
 	STD_ERROR				StdError						=	{0};
	
	int						iNumberOfPoints					=	0;
							
	double 					*plfLocalTrace					=	NULL;

	double					*pReadBuffer					=	NULL; 
	
	float       			*pFloatBuffer  					=	NULL; 
								
	unsigned int			uiCount							=	0,
							uiNumberOfBinaryData			=	0;
	
	double					doubleValue						=	0.0;
	
	float					floatValue						=	0.0;
	
	int						iIndex							=	0;
	
	int						iTraceIndex	    				=	0; 
	
	int						valueSize						=   4;
	
	char					szReadFeedback[LOW_STRING]		=	{0};
	
	tsHandle				*pLocalStorage					=	{0};

	int						handle							=	0;
	
	int						model							=	0;
	

	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	model =  pLocalStorage->tType;
	
	switch ( model )
	{
		case MODEL_TYPE_N9010A:
		case MODEL_TYPE_N9010B:
		case MODEL_TYPE_N9030A:
			
			{
				CHK_VSA (viPrintf( pLocalStorage->sessionHandle ,":INST?\n" ));    
				
				SetBreakOnLibraryErrors (0); 
				viRead ( pLocalStorage->sessionHandle ,(ViPBuf)szReadFeedback , 10 , &uiCount );
				SetBreakOnLibraryErrors (1);
				
				CHK_VSA (viPrintf( pLocalStorage->sessionHandle ,":FORMat REAL,32\n" ));  

				CHK_VSA (viPrintf( pLocalStorage->sessionHandle ,":FORM:BORD SWAP\n" ));  
			
				CHK_VSA (viPrintf( pLocalStorage->sessionHandle , "*WAI\n" )); 
					
				if ( strstr( szReadFeedback , "PNOISE" ))
				{   
				
					CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":READ:LPLot4?\n" ));
					
					DelayWithEventProcessing (10);
				}
				else
				{   
					CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":TRACe? TRACE1\n" )); 

					DelayWithEventProcessing (10);
				}

				do 
				{
					SetBreakOnLibraryErrors (0); 
					viRead ( pLocalStorage->sessionHandle ,(ViPBuf)szReadFeedback , 1 , &uiCount );
					SetBreakOnLibraryErrors (1); 

					if ( szReadFeedback[0] == '#' )
						break;

				} while(uiCount);

				IF (( szReadFeedback[0] != '#' ) , "Wrong trace reading format" );  

				memset( szReadFeedback , 0 , LOW_STRING );

				SetBreakOnLibraryErrors (0); 
				CHK_VSA (viRead ( pLocalStorage->sessionHandle ,(ViPBuf)szReadFeedback , 1 , &uiCount ));
				SetBreakOnLibraryErrors (1); 

				IF (( uiCount == 0 ) , "Wrong trace reading format" );  

				uiCount = atoi(szReadFeedback); 

				memset( szReadFeedback , 0 , LOW_STRING );

				SetBreakOnLibraryErrors (0); 
				CHK_VSA (viRead ( pLocalStorage->sessionHandle ,(ViPBuf)szReadFeedback , uiCount , &uiCount ));  
				SetBreakOnLibraryErrors (1); 
				
				uiNumberOfBinaryData = atoi(szReadFeedback); 

				CALLOC_ERR( pFloatBuffer , uiNumberOfBinaryData , sizeof(char)); 

				SetBreakOnLibraryErrors (0);
				CHK_VSA (viRead ( pLocalStorage->sessionHandle ,(ViPBuf)pFloatBuffer , uiNumberOfBinaryData , &uiCount ));    
				SetBreakOnLibraryErrors (1);
				
				IF (( uiNumberOfBinaryData != uiCount ) , "Wrong trace reading format" );
				
				iNumberOfPoints =  uiCount / valueSize;  

				CALLOC_ERR( plfLocalTrace , iNumberOfPoints , sizeof(double));   

				for ( iTraceIndex = 0; iTraceIndex < iNumberOfPoints; iTraceIndex++ )
				{ 
					floatValue =  pFloatBuffer[iTraceIndex];
					doubleValue = (double) floatValue;
					
					plfLocalTrace[iTraceIndex] = doubleValue; 
				}

				SetBreakOnLibraryErrors (0);
				CHK_VSA (viRead ( pLocalStorage->sessionHandle ,(ViPBuf)szReadFeedback , 1 , &uiCount ));  
				SetBreakOnLibraryErrors (1);
			}
			
			break;
			
		case MODEL_TYPE_N9020A:
		case MODEL_TYPE_N9000A:
		case MODEL_TYPE_N8201A:
		case MODEL_TYPE_E4448A:
		case MODEL_TYPE_E4447A:
		case MODEL_TYPE_E4446A:
		case MODEL_TYPE_E4445A:
		case MODEL_TYPE_E4443A:
		case MODEL_TYPE_E4440A:
		case MODEL_TYPE_E4406A:
		case MODEL_TYPE_E4411B:
		case MODEL_TYPE_E4408B:
		case MODEL_TYPE_E4407B:
		case MODEL_TYPE_E4405B:
		case MODEL_TYPE_E4404B:
		case MODEL_TYPE_E4403B:
		case MODEL_TYPE_E4402B:
		case MODEL_TYPE_E4401B:
		case MODEL_TYPE_E7405A:
		case MODEL_TYPE_E7404A:
		case MODEL_TYPE_E7403A:
		case MODEL_TYPE_E7402A:
		case MODEL_TYPE_E7401A:
		default :  	
		
				{   
						viPrintf( pLocalStorage->sessionHandle , "*WAI\n" ); 
						
						WaitForOperationCompletion( pLocalStorage->sessionHandle , pLocalStorage->lfTimeout  ,  pLocalStorage->lfOpcLowLevelTimeout ); 
						
						CHK_VSA ( viPrintf( pLocalStorage->sessionHandle , ":TRAC:MATH:MEAN? TRACE%d\n" )); 
	
						do 
						{
							SetBreakOnLibraryErrors (0); 
							viRead ( pLocalStorage->sessionHandle ,(ViPBuf)szReadFeedback , 1 , &uiCount );
							SetBreakOnLibraryErrors (1); 
	
							if ( szReadFeedback[0] == '#' )
								break;
	
						} while(uiCount);
	
						IF (( szReadFeedback[0] != '#' ) , "Wrong trace reading format" );  
	
						memset( szReadFeedback , 0 , LOW_STRING );
	
						SetBreakOnLibraryErrors (0); 
						viRead ( pLocalStorage->sessionHandle ,(ViPBuf)szReadFeedback , 1 , &uiCount );
						SetBreakOnLibraryErrors (1); 
	
						IF (( uiCount == 0 ) , "Wrong trace reading format" );  
	
						uiCount = atoi(szReadFeedback); 
	
						memset( szReadFeedback , 0 , LOW_STRING );
	
						SetBreakOnLibraryErrors (0); 
						viRead ( pLocalStorage->sessionHandle ,(ViPBuf)szReadFeedback , uiCount , &uiCount );  
						SetBreakOnLibraryErrors (1); 
						
						uiNumberOfBinaryData = atoi(szReadFeedback); 
	
						CALLOC_ERR( pReadBuffer , uiNumberOfBinaryData , sizeof(char)); 
	
						SetBreakOnLibraryErrors (0);
						viRead ( pLocalStorage->sessionHandle ,(ViPBuf)pReadBuffer , uiNumberOfBinaryData , &uiCount );    
						SetBreakOnLibraryErrors (1);
						
						IF (( uiNumberOfBinaryData != uiCount ) , "Wrong trace reading format" );     

						CALLOC_ERR( plfLocalTrace , iNumberOfPoints , sizeof(double));   
	
						for ( iIndex = 0; iIndex < iNumberOfPoints; iIndex++ )
							plfLocalTrace[iIndex] = pReadBuffer[(iIndex*2)];

						SetBreakOnLibraryErrors (0);
						viRead ( pLocalStorage->sessionHandle ,(ViPBuf)szReadFeedback , 1 , &uiCount );  
						SetBreakOnLibraryErrors (1);
				}
	}
Error: 
	   
	if (pTrace)
		*pTrace = plfLocalTrace;
	else
		FREE(plfLocalTrace);
	
	if ( piNumberOfPoints )
		*piNumberOfPoints = iNumberOfPoints;
	
	FREE(pReadBuffer);
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
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
	
		if ( iError == -420 || iError == -144 || iError == -113)
		{
			viPrintf( sessionHandle , "*CLS\n" );  
		}
	}
	
	viSetAttribute ( sessionHandle , VI_ATTR_TMO_VALUE , iTimeout );  
	
	SetBreakOnLibraryErrors (1); 
	
	return iOpcStatus;
}

int		RecursiveMakeDirectory( int sessionHandle , char *pPath )
{
	char		szCurrentPath[STD_STRING]		=	{0},
				szReadFeedback[STD_STRING]		=	{0},
				*pTemp							=	NULL;
														
	char		szEmptyDirectory[]				=	{"0,0,\"\"\n"};
	
	int			count							=	0;
	
	int			status							=	0;
	
	if ( pPath == NULL )
		return -1;
	
	strcpy( szCurrentPath , pPath );
	
	viPrintf( sessionHandle , "MMEM:CAT? \"%s\"\n" , szCurrentPath );
	
	SetBreakOnLibraryErrors (0);
	viRead( sessionHandle, szReadFeedback , (STD_STRING-1) , &count );
	SetBreakOnLibraryErrors (1);
	
	if ( strcmp( szReadFeedback , szEmptyDirectory ) == 0 ) 
	{
		pTemp = strrchr( szCurrentPath , '\\' );
		
		if ( pTemp )
		{
			pTemp++;
			*pTemp = 0;
			
			RecursiveMakeDirectory( sessionHandle , szCurrentPath );
		}														
		
		if ( strchr( pPath , '.' ) == NULL )
		{
			if ( strlen(pPath) )
			{
				viPrintf( sessionHandle , "MMEM:MDIR \"%s\"\n" , pPath );  
				   
				viPrintf( sessionHandle , "MMEM:CAT? \"%s\"\n" , pPath );
	
				SetBreakOnLibraryErrors (0);
				viRead( sessionHandle, szReadFeedback , (STD_STRING-1) , &count );
				SetBreakOnLibraryErrors (1);
				
				status = strcmp( szReadFeedback , szEmptyDirectory );
			}
			
			return status;
		}
	}

	return 0;
}


int main (int argc, char *argv[])
{
	int					status					=	0;
	
	char				szOutputText[1024]		=	{0};
	
	int 				hInstrumentHandle		=	0;
		
	int					bConnectStatus			=	0,
						ChannelsQuantity		=	0,
						Count					=	0; 
						
	double				lfFrequency				=	0.0,
						vlfPower[4]				=	{0.0};
	
	double 				*pTrace					=	NULL;
							  
	char 				*pSymbolStream			=	NULL;
		
	int					iNumberOfSymbolsRead	=	0;
	
	int					iNumberOfSpurious		=	0;
	
	int					*pvRangeNumber			=	NULL,
						*pvPassFail				=	NULL;
						
	double				*pvlfSpurFrequency		=	NULL,
						*pvlfSpurAmplitude		=	NULL,
						*pvlfSpurLimit			=	NULL;
	
	double				vFrequencies[10]		=	{1E9,2E9,3E9,4E9,5E9,6E9,7E9,8E9,9E9,10E9},
						vLoss[10]				=	{0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.3,1.4};
	
	InitCVIRTE (0, argv, 0 ); 

	SpectrumAnalyzer_Init ( 0 , "TCPIP::10.0.1.51" , &hInstrumentHandle , &bConnectStatus , &ChannelsQuantity );
	
	SpectrumAnalyzer_GetTrace( hInstrumentHandle , 20000 , &pTrace , 20000 , &Count );   

//	SpectrumAnalyzer_SendFile( hInstrumentHandle  , "C:\\HelloWorld.png" , "D:\\Ofir\\HelloWorld.png"  );  
	
//	SpectrumAnalyzer_NoiseFigure_SetLossCompensationTable ( hInstrumentHandle , 0 , vFrequencies, vLoss , 10 );
	
//	SpectrumAnalyzer_SetSweepMode( hInstrumentHandle , 0 );
	
//	SpectrumAnalyzer_RestartMeasure( hInstrumentHandle );  
	
//	SpectrumAnalyzer_GetSweep( hInstrumentHandle , 60.0 );
	
	//SpectrumAnalyzer_Reset(hInstrumentHandle);
	
	//SpectrumAnalyzer_RecallState( hInstrumentHandle , "state.state" );  
	
	 
	
//	SpectrumAnalyzer_SearchMarkerPeak( hInstrumentHandle , 1 ); 
	
	
		
//	SpectrumAnalyzer_WaitForIntrumentDone(hInstrumentHandle,20000);
	
	//SpectrumAnalyzer_Read_Spurious_Emissions_List( hInstrumentHandle , &iNumberOfSpurious , &pvRangeNumber , &pvPassFail , &pvlfSpurFrequency , &pvlfSpurAmplitude , &pvlfSpurLimit );     
	
	//
	//SpectrumAnalyzer_PhaseNoise_GetMarker ( hInstrumentHandle , 1 , 1 , 20.0 , &lfFrequency , &vlfPower[0] );   
	//SpectrumAnalyzer_PhaseNoise_GetMarker ( hInstrumentHandle , 1 , 2 , 20.0 , &lfFrequency , &vlfPower[1] );   
	//SpectrumAnalyzer_PhaseNoise_GetMarker ( hInstrumentHandle , 1 , 3 , 20.0 , &lfFrequency , &vlfPower[2] );   
	//SpectrumAnalyzer_PhaseNoise_GetMarker ( hInstrumentHandle , 1 , 4 , 20.0 , &lfFrequency , &vlfPower[3] );   
	//
	//SpectrumAnalyzer_GetMarkerMeasure ( hInstrumentHandle , 0 , 1 , &lfFrequency , &lfPower );
		
	//SpectrumAnalyzer_RecallState( hInstrumentHandle , "spart.state" );
		
	//  

	//SpectrumAnalyzer_WaitForIntrumentDone(hInstrumentHandle,20000);
	
	
/*	
	SpectrumAnalyzer_WaitForIntrumentDone(hInstrumentHandle,20000);
	
	SpectrumAnalyzer_Reset(hInstrumentHandle);	
	
	SpectrumAnalyzer_WaitForIntrumentDone(hInstrumentHandle,20000);
	
	SpectrumAnalyzer_RecallState( hInstrumentHandle , "dlt_ber.state" );
	
	SpectrumAnalyzer_WaitForIntrumentDone(hInstrumentHandle,20000);
	
	SpectrumAnalyzer_SetFrequency_EVM( hInstrumentHandle , 60e6 );
	
	SpectrumAnalyzer_WaitForIntrumentDone(hInstrumentHandle,20000);
	
	SpectrumAnalyzer_Read_SymbolStream( hInstrumentHandle , 5000 , &pSymbolStream , &iNumberOfSymbolsRead );
	
	SpectrumAnalyzer_RestartMeasure( hInstrumentHandle );
	SpectrumAnalyzer_RestartMeasure( hInstrumentHandle );
	SpectrumAnalyzer_RestartMeasure( hInstrumentHandle );
	SpectrumAnalyzer_RestartMeasure( hInstrumentHandle );
	SpectrumAnalyzer_RestartMeasure( hInstrumentHandle );
		
	SpectrumAnalyzer_Read_SymbolStream( hInstrumentHandle , 5000 , &pSymbolStream , &iNumberOfSymbolsRead );
	
	SpectrumAnalyzer_SearchMarkerPeak( hInstrumentHandle , 1 );  

	SpectrumAnalyzer_SearchMarkerNextPeak( hInstrumentHandle , 1 );  
	
	SpectrumAnalyzer_Reset(hInstrumentHandle);	 
		
	SpectrumAnalyzer_ConfigSource( hInstrumentHandle , 1 ); 
		
	SpectrumAnalyzer_SetSweepMode( hInstrumentHandle , 0 ); 
		
	SpectrumAnalyzer_InitiateMeasure( hInstrumentHandle );
		
	SpectrumAnalyzer_SetSweepMode( hInstrumentHandle , 1 ); 
		
	SpectrumAnalyzer_SetMarkerFrequency ( hInstrumentHandle , 1 , 15E9 );
	
	SpectrumAnalyzer_SetCenterFrequencyFromMarker ( hInstrumentHandle , 1 );  
	
	SpectrumAnalyzer_SetReferenceLevelFrequencyFromMarker ( hInstrumentHandle , 1 ); 
	
	//SpectrumAnalyzer_GetReferenceLevel( hInstrumentHandle , &lfPower );
		
	//lfPower += 10;
	
	//SpectrumAnalyzer_SetReferenceLevel( hInstrumentHandle , lfPower );
	
	SpectrumAnalyzer_SetNumberOfPoints( hInstrumentHandle , 1001 ); 
		
	SpectrumAnalyzer_GetTrace( hInstrumentHandle , 20000 , &pTrace , 20000 , &Count );
		
	SpectrumAnalyzer_GetErrorTextMessage ( hInstrumentHandle , status , szOutputText );
		
	SpectrumAnalyzer_Close(&hInstrumentHandle);	


	SpectrumAnalyzer_SetTriggerSource( hInstrumentHandle , 0 );
	SpectrumAnalyzer_SetTriggerSource( hInstrumentHandle , 1 );
	SpectrumAnalyzer_SetTriggerSource( hInstrumentHandle , 2 );
	SpectrumAnalyzer_SetTriggerSource( hInstrumentHandle , 3 );
	SpectrumAnalyzer_SetTriggerSource( hInstrumentHandle , 4 );
*/
	return 0;
	
}

