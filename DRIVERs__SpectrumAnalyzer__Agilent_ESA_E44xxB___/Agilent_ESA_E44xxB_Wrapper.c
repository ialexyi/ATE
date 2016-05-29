//==============================================================================
// Include files
#include "Windows.h"
#include "toolbox.h"
#include <ansi_c.h>
#include <userint.h>
#include <utility.h>
#include "GlobalDefines.h"

#include "Agilent_ESA_E44xxB_Wrapper.h"

//==============================================================================
// Constants

#define				MAX_TIMEOUT			5000

//==============================================================================
// Types

typedef struct
{
	ViSession			defaultRM,
						sessionHandle;
	
	char				*pLastStateFile;
	
} tsHandle;

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


int	SpectrumAnalyzer_error_query ( int viInstrumentHandle , int *piError , char *pszErrorDescription )
{
	int				error								=	0;

	char			szReadMessage[STD_STRING]			=	{0};
	
	if ( viQueryf( viInstrumentHandle , ":SYST:ERR?" , "%s" , szReadMessage ) == 0 )
		sscanf( szReadMessage , "%d,\"%s\"" , &error , pszErrorDescription );
		
	return error;
}

/***** ***** ***** ***** ***** ***** Get Error Text Message ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT SpectrumAnalyzer_GetErrorTextMessage ( int hInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char			szErrorDescription[STD_STRING]		=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	SpectrumAnalyzer_error_query ( pLocalHandle->sessionHandle , &(StdError.error) , szErrorDescription );
	
	if ( pErrorMessage )
	{
		strcpy( pErrorMessage , szErrorDescription ); 
	}
	
	CALLOC_COPY_STRING( StdError.pszErrorDescription , szErrorDescription );
	
Error:

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
	
		if ( strstr( szIdentificationText , "8719d" ))
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

void* DLLEXPORT SpectrumAnalyzer_Init ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *phInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChanelsQuantity )
{
	STD_ERROR		StdError						=	{0};
	
	tsHandle		*pLocalHandle					=	{0};
	
	CmtTSVHandle 	handle							=	0;
	
	int				supported						=	0;
	
	char			szModel[LOW_STRING]				=	{0},
					vszSupportdedModels[][32]		=	{{"E4401B"},{"E4402B"},{"E4404B"},{"E4405B"},{"E4407B"},{"E4411B"},{"E4403B"},{"E4408B"}};
	
	int				iIndex							=	0;
	
	unsigned int	uiCount							=	0;
	
	IF (( szRsrcAddress == NULL ) , "Address string is empty" );
	
	if ( hParentInstrumentHandle == 0 ) 
	{
		CHK_CMT ( CmtNewTSV ( sizeof(tsHandle) , &handle ));
	}
	else
		handle = hParentInstrumentHandle;
		
	if ( phInstrumentHandle )
		*phInstrumentHandle = handle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));

	if ( pLocalHandle->defaultRM == 0 )
	{
		CHK_VSA ( viOpenDefaultRM (&(pLocalHandle->defaultRM)));
	}
	
	if ( pLocalHandle->sessionHandle == 0 )
	{
		CHK_VSA ( viOpen ( pLocalHandle->defaultRM , szRsrcAddress , VI_NULL, VI_NULL, &(pLocalHandle->sessionHandle)));
		
		viSetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , 5000 );
		
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,"*CLS\n" ));  
		
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,"*RST\n" ));  
		
		CHK_VSA ( viWrite( pLocalHandle->sessionHandle ,(ViBuf)"*IDN?" , 5 , &uiCount ));  
		
		CHK_VSA ( viRead ( pLocalHandle->sessionHandle ,(ViBuf)szModel, LOW_STRING , &uiCount ));
		
		for ( iIndex = 0; iIndex < (sizeof(vszSupportdedModels) / sizeof(vszSupportdedModels[0])); iIndex++ )
			if ( strstr( szModel , vszSupportdedModels[iIndex]) )
			{
				supported = 1; 
				break;
			}
	}
	
	IF (( supported == 0 ) , "This device has not supported." );
	
Error:
	
	if ( InstrumentConnectStatus )
		*InstrumentConnectStatus = supported;
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** Close ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT SpectrumAnalyzer_Close ( int *hInstrumentHandle )
{
	STD_ERROR		StdError						=	{0};
	
	tsHandle		*pLocalHandle					=	{0};

	int				handle							=	0;
	
	IF (( hInstrumentHandle == NULL ) , "Handle is empty" );
	
	handle = *hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	viClose (pLocalHandle->sessionHandle);  
	
	viClose (pLocalHandle->defaultRM);
	
	FREE(pLocalHandle->pLastStateFile);
	
	CmtReleaseTSVPtr ( handle ); 
	
	CmtDiscardTSV (handle);
	
	*hInstrumentHandle = 0;
	
Error:

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_Reset ( int hInstrumentHandle )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char			szReadFeedback[STD_STRING]			=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "*CLS\n" ));
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "*RST\n" ));
	
	CHK_VSA ( viQueryf( pLocalHandle->sessionHandle , "*OPC?\n" , "%s" , szReadFeedback ));
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_Preset ( int hInstrumentHandle )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char			szReadFeedback[STD_STRING]			=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SYST:PRES\n" ));
	
	CHK_VSA ( viQueryf( pLocalHandle->sessionHandle , "*OPC?\n" , "%s" , szReadFeedback ));
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_SetAuto ( int hInstrumentHandle )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char			szReadFeedback[STD_STRING]			=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	CHK_VSA ( viQueryf( pLocalHandle->sessionHandle , "*OPC?\n" , "%s" , szReadFeedback ));
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_AutoTune ( int hInstrumentHandle , double lfTimeout )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char			szReadFeedback[STD_STRING]			=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	CHK_VSA ( viQueryf( pLocalHandle->sessionHandle , "*OPC?\n" , "%s" , szReadFeedback ));
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_SetFrequencyCenterSpan ( int hInstrumentHandle , int channel , double lfFrequency , double lfSpan )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char			szReadFeedback[STD_STRING]			=	{0},
					szWriteFeedback[STD_STRING]			=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szWriteFeedback , ":FREQ:CENT %E" , lfFrequency ); 
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szWriteFeedback ));
	
	sprintf( szWriteFeedback , ":FREQ:SPAN %E" , lfSpan ); 
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szWriteFeedback ));
	
	CHK_VSA ( viQueryf( pLocalHandle->sessionHandle , "*OPC?\n" , "%s" , szReadFeedback ));
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_ConfigureMarker ( int hInstrumentHandle , int channel , int marker , int bState )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char			szReadFeedback[STD_STRING]			=	{0},
					szWriteFeedback[STD_STRING]			=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if ( marker > 0 )
	{
		if ( bState )
			sprintf( szWriteFeedback , ":CALC:MARK%d:STAT ON" , marker );
		else
			sprintf( szWriteFeedback , ":CALC:MARK%d:STAT OFF" , marker ); 
	}
	else
	{
		
		sprintf( szWriteFeedback , ":CALCulate:MARKer:AOFF" );
	}
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szWriteFeedback ));
	
	CHK_VSA ( viQueryf( pLocalHandle->sessionHandle , "*OPC?\n" , "%s" , szReadFeedback ));
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_SetMarkerSearchPeakAuto ( int hInstrumentHandle , int channel , int bMax , int marker )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char			szReadFeedback[STD_STRING]			=	{0},
					szWriteFeedback[STD_STRING]			=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if ( bMax )
	{
		sprintf( szWriteFeedback , ":CALC:MARK%d:MAX" , marker );
	}
	else
	{
		sprintf( szWriteFeedback , ":CALC:MARK%d:MIN" , marker );
	}
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szWriteFeedback ));
	
	CHK_VSA ( viQueryf( pLocalHandle->sessionHandle , "*OPC?\n" , "%s" , szReadFeedback ));
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_GetMarkerMeasure ( int hInstrumentHandle , int channel , int marker , double *pFrequency , double *pPower )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char			szReadFeedback[STD_STRING]			=	{0},
					szWriteFeedback[STD_STRING]			=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szWriteFeedback , ":CALC:MARK%d:X?" , marker ); 
	
	CHK_VSA ( viQueryf( pLocalHandle->sessionHandle , szWriteFeedback , "%s" , szReadFeedback ));
	
	if ( pFrequency )
	{
		*pFrequency = atof(szReadFeedback);
	}
	
	sprintf( szWriteFeedback , ":CALC:MARK%d:Y?" , marker ); 
	
	CHK_VSA ( viQueryf( pLocalHandle->sessionHandle , szWriteFeedback , "%s" , szReadFeedback ));
	
	if ( pPower )
	{
		*pPower = atof(szReadFeedback);
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_PhaseNoise_GetMarker ( int hInstrumentHandle , int channel , int marker , double *pPosition , double *pValue )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char			szReadFeedback[STD_STRING]			=	{0},
					szWriteFeedback[STD_STRING]			=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szWriteFeedback , ":CALC:MARK%d:FUNC NOIS" , marker );
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szWriteFeedback ));
	
	CHK_VSA ( viQueryf( pLocalHandle->sessionHandle , "*OPC?\n" , "%s" , szReadFeedback )); 
	
	sprintf( szWriteFeedback , ":CALC:MARK%d:X?" , marker ); 
	
	CHK_VSA ( viQueryf( pLocalHandle->sessionHandle , szWriteFeedback , "%s" , szReadFeedback ));
	
	if ( pPosition )
	{
		*pPosition = atof(szReadFeedback);
	}
	
	sprintf( szWriteFeedback , ":CALC:MARK%d:Y?" , marker ); 
	
	CHK_VSA ( viQueryf( pLocalHandle->sessionHandle , szWriteFeedback , "%s" , szReadFeedback ));
	
	if ( pValue )
	{
		*pValue = atof(szReadFeedback);
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return SpectrumAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_GetTrace( int hInstrumentHandle , int timeout , double **pTrace , int numberOfPoints , int *pCount )
{
 	STD_ERROR		StdError							=	{0};
	
	int				iNumberOfPoints						=	0;
					
	double 			*plfLocalTrace						=	NULL,
					*plfLocalFrequency					=	NULL;

	double			*pReadBuffer						=	NULL; 
						
	unsigned int	uiCount								=	0,
					uiNumberOfBinaryData				=	0;
	
	int				iIndex								=	0;
	
	char			szReadFeedback[LOW_STRING]			=	{0};
	
	int				iChannelNumber						=	0;
	
	double			lfStartFrequency					=	0.0,
					lfStopFrequency						=	0.0,
					lfStepFrequency						=	0.0,
					lfCurrentFrequency					=	0.0;
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char			szReadFeedback[STD_STRING]			=	{0},
					szWriteFeedback[STD_STRING]			=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "INIT:IMM \n" ));

	CHK_VSA ( viQueryf( pLocalHandle->sessionHandle , "*OPC?\n" , "%s" , szReadFeedback )); 
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "TRAC? TRACE1\n" )); 

	
	pNext = pReadBuffer;
	
	while( pNext )
	{
		pCurrent = pNext;
		
		pTemp = strchr( pNext  , ',' );   
		
		if ( pTemp )
		{
			*pTemp = 0;
			
			pNext = pTemp + 1;
		}
		
		pTemp = strchr( pNext  , ',' );   
		
		if ( pTemp )
		{
			*pTemp = 0;
			
			pNext = pTemp + 1;
		}
		
		plfLocalTrace[iPointsCounter++] = atof(pCurrent);
		
		if ( iPointsCounter >= iNumberOfPoints )
			break;
	}
	
Error: 
	   
	if (plfTrace)
		*plfTrace = plfLocalTrace;
	else
		FREE(plfLocalTrace);
	
	if ( piNumberOfPoints )
		*piNumberOfPoints = iNumberOfPoints;
	
	FREE(pReadBuffer);
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT SpectrumAnalyzer_SetMaxHold ( int hInstrumentHandle , int  bState )
{
	STD_ERROR               StdError                    =   {0};     

	char					szTraceName[512]			=	{"TR1"};
	
	CHK_ERR( AgSAn_GetTraceName(viInstrumentHandle,1,512,szTraceName));
	
	switch ( bState )
	{
		case 0:
					CHK_ERR( AgSAn_ConfigureTraceType( viInstrumentHandle , szTraceName , AGSAN_VAL_TRACE_TYPE_CLEAR_WRITE ));
					break;
		case 1:
					
					CHK_ERR( AgSAn_ConfigureTraceType( viInstrumentHandle , szTraceName , AGSAN_VAL_TRACE_TYPE_MAX_HOLD ));
					break;	
					
		default:
					break;
	}
	
Error: 
	
	RETURN_STDERR_POINTER;
}

:TRACe:[1]|2|3:MODE MAXHold

void* DLLEXPORT SpectrumAnalyzer_ConfigChannelPowerMeasuring( int hInstrumentHandle , double lfBandWidth )  
{
	STD_ERROR               StdError                    =   {0};     
	
	ViBoolean 				Value						=	0;
		
	char					szCommand[128]				=	{0};
	
	CHK_ERR( AgSAn_Basic_ChannelPowerConfigure ( viInstrumentHandle ));
	
Error: 
	
	RETURN_STDERR_POINTER;
}



void* DLLEXPORT SpectrumAnalyzer_GetChannelPowerResult( int hInstrumentHandle , double *plfValue )
{
	return NULL;
}

void* DLLEXPORT SpectrumAnalyzer_GetStartStopFrequency( int hInstrumentHandle , double *plfStartFrequency , double *plfStopFrequency )
{
	STD_ERROR               StdError                    =   {0};    
	
	int						count						=	0,
							index						=	0;
	
	double					results[1024]				=	{0},
							average						=	0.0,
							result						=	0.0; 
	
	if ( plfStartFrequency )
		CHK_ERR( AgSAn_GetAttributeViReal64 ( viInstrumentHandle , NULL , AGSAN_ATTR_FREQUENCY_START , plfStartFrequency )); 

	if ( plfStopFrequency )
		CHK_ERR( AgSAn_GetAttributeViReal64 ( viInstrumentHandle , NULL , AGSAN_ATTR_FREQUENCY_STOP , plfStopFrequency )); 
	
Error: 

	RETURN_STDERR_POINTER;
}


void* DLLEXPORT SpectrumAnalyzer_ConfigBandWidth( int hInstrumentHandle , double lfResolutionBandWidth , double lfVideoBandWidth )  
{
	STD_ERROR               StdError                    =   {0};    
	
	if ( lfResolutionBandWidth > 0.0 )
	{
		CHK_ERR( AgSAn_SetAttributeViBoolean ( viInstrumentHandle , NULL , AGSAN_ATTR_RESOLUTION_BANDWIDTH_AUTO , 0 )); 

		CHK_ERR( AgSAn_SetAttributeViReal64 ( viInstrumentHandle , NULL , AGSAN_ATTR_RESOLUTION_BANDWIDTH , lfResolutionBandWidth )); 
	}
	else
		CHK_ERR( AgSAn_SetAttributeViBoolean ( viInstrumentHandle , NULL , AGSAN_ATTR_RESOLUTION_BANDWIDTH_AUTO , 1 )); 
	
	if ( lfVideoBandWidth > 0.0 )
	{
		CHK_ERR( AgSAn_SetAttributeViBoolean ( viInstrumentHandle , NULL , AGSAN_ATTR_VIDEO_BANDWIDTH_AUTO , 0 )); 

		CHK_ERR( AgSAn_SetAttributeViReal64 ( viInstrumentHandle , NULL , AGSAN_ATTR_VIDEO_BANDWIDTH , lfVideoBandWidth )); 
	}
	else
		CHK_ERR( AgSAn_SetAttributeViBoolean ( viInstrumentHandle , NULL , AGSAN_ATTR_VIDEO_BANDWIDTH_AUTO , 1 )); 
	
Error: 

	RETURN_STDERR_POINTER;
}
	
void* DLLEXPORT SpectrumAnalyzer_ConfigSource( int hInstrumentHandle , int bExternal )  
{
	STD_ERROR               StdError                    =   {0};    

	if ( bExternal )
	{
		CHK_ERR( AgSAn_SetAttributeViInt32 ( viInstrumentHandle , NULL , AGSAN_ATTR_REF_OSCILLATOR_SOURCE , AGSAN_VAL_REF_OSCILLATOR_SOURCE_EXTERNAL )); 
	}
	else
	{
		CHK_ERR( AgSAn_SetAttributeViInt32 ( viInstrumentHandle , NULL , AGSAN_ATTR_REF_OSCILLATOR_SOURCE , AGSAN_VAL_REF_OSCILLATOR_SOURCE_INTERNAL )); 
	}
	
Error: 
	
	RETURN_STDERR_POINTER;
}	

void* DLLEXPORT SpectrumAnalyzer_SetNumberOfPoints( int hInstrumentHandle , int points )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( AgSAn_SetAttributeViInt32 ( viInstrumentHandle , NULL , AGSAN_ATTR_SA_ACP_SWEEP_POINTS , points )); 
	
Error: 

	RETURN_STDERR_POINTER;
}



void* DLLEXPORT SpectrumAnalyzer_SetScaleDev( int hInstrumentHandle , double lfScaleDev )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( AgSAn_SetAttributeViReal64 ( viInstrumentHandle , NULL , AGSAN_ATTR_SA_ACP_DISPLAY_Y_SCALE , lfScaleDev )); 
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_Attenuator( int hInstrumentHandle , double lfAttenuator )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( AgSAn_SetAttributeViReal64 ( viInstrumentHandle , NULL , AGSAN_ATTR_ATTENUATION , lfAttenuator )); 
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetReferenceLevel( int hInstrumentHandle , double lfReferenceLevel )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( AgSAn_SetAttributeViReal64 ( viInstrumentHandle , NULL , AGSAN_ATTR_REFERENCE_LEVEL , lfReferenceLevel )); 
	
Error:
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetReferenceLevel( int hInstrumentHandle , double *vlfReferenceLevel )  
{
	STD_ERROR               StdError                    =   {0};        

	if ( vlfReferenceLevel )
		CHK_ERR( AgSAn_GetAttributeViReal64 ( viInstrumentHandle , NULL , AGSAN_ATTR_REFERENCE_LEVEL , vlfReferenceLevel )); 
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetMarkerType( int hInstrumentHandle , int bDelta )  
{
	STD_ERROR               StdError                    =   {0};        

	if ( bDelta )
	{
		CHK_ERR( AgSAn_SetAttributeViInt32 ( viInstrumentHandle , NULL , AGSAN_ATTR_MARKER_TYPE , AGSAN_VAL_MARKER_TYPE_DELTA )); 
	}
	else
	{
		CHK_ERR( AgSAn_SetAttributeViInt32 ( viInstrumentHandle , NULL , AGSAN_ATTR_MARKER_TYPE , AGSAN_VAL_MARKER_TYPE_NORMAL )); 
	}
	
Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetMarkerFrequency( int hInstrumentHandle , double lfFrequency )  
{
	STD_ERROR               StdError                    =   {0};        

	
	CHK_ERR( AgSAn_SetAttributeViBoolean( viInstrumentHandle , NULL , AGSAN_ATTR_MARKER_ENABLED , 1 )); 

	CHK_ERR( AgSAn_SetAttributeViReal64( viInstrumentHandle , NULL , AGSAN_ATTR_MARKER_POSITION , lfFrequency )); 
	
Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SearchMarkerPeak( int hInstrumentHandle  )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( AgSAn_SetAttributeViBoolean( viInstrumentHandle , NULL , AGSAN_ATTR_MARKER_ENABLED , 1 )); 

	CHK_ERR( AgSAn_MarkerSearch ( viInstrumentHandle , AGSAN_VAL_MARKER_SEARCH_HIGHEST ));
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SearchMarkerNextPeak( int hInstrumentHandle  )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( AgSAn_SetAttributeViBoolean( viInstrumentHandle , NULL , AGSAN_ATTR_MARKER_ENABLED , 1 )); 

	CHK_ERR( AgSAn_MarkerSearch ( viInstrumentHandle , AGSAN_VAL_MARKER_SEARCH_NEXT_PEAK ));
	
Error: 
	
	RETURN_STDERR_POINTER;
}																		

void* DLLEXPORT SpectrumAnalyzer_GetMarkerFrequency( int hInstrumentHandle , int iMarkerNr , double *vlfFrequency )  
{
	STD_ERROR               StdError                    =   {0};        

	if ( vlfFrequency )
	{
		AgSAn_SystemWaitForOperationComplete ( viInstrumentHandle , 10000 );
		
		CHK_ERR( AgSAn_GetAttributeViReal64( viInstrumentHandle , NULL , AGSAN_ATTR_MARKER_POSITION , vlfFrequency )); 
	}
	
Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetMarkerAmplitude( int hInstrumentHandle , int iMarkerNr , double *vlfAmplitude )  
{
	STD_ERROR               StdError                    =   {0};        

	if ( vlfAmplitude )
	{
		AgSAn_SystemWaitForOperationComplete ( viInstrumentHandle , 10000 );

		CHK_ERR( AgSAn_GetAttributeViReal64( viInstrumentHandle , NULL , AGSAN_ATTR_MARKER_AMPLITUDE , vlfAmplitude )); 
	}
	
Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetMarkerDeltaFrequency( int hInstrumentHandle , double *vlfFrequency )  
{
	STD_ERROR               StdError                    =   {0};        

	if ( vlfFrequency )
		CHK_ERR( AgSAn_GetAttributeViReal64( viInstrumentHandle , NULL , AGSAN_ATTR_REFERENCE_MARKER_POSITION , vlfFrequency )); 
	
Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetMarkerDeltaAmplitude( int hInstrumentHandle , double *vlfAmplitude )  
{
	STD_ERROR               StdError                    =   {0};        

	if ( vlfAmplitude )
		CHK_ERR( AgSAn_GetAttributeViReal64( viInstrumentHandle , NULL , AGSAN_ATTR_REFERENCE_MARKER_AMPLITUDE , vlfAmplitude )); 
	
Error: 

	RETURN_STDERR_POINTER;
}



void* DLLEXPORT SpectrumAnalyzer_SetCenterFrequencyFromMarker( int hInstrumentHandle )  
{
	STD_ERROR               StdError                    =   {0};        
	
	CHK_ERR( AgSAn_SetInstrumentFromMarker (viInstrumentHandle, AGSAN_VAL_INSTRUMENT_SETTING_FREQUENCY_CENTER));
	
Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetReferenceLevelFrequencyFromMarker( int hInstrumentHandle )  
{
	STD_ERROR               StdError                    =   {0};        
	
	CHK_ERR( AgSAn_SetInstrumentFromMarker (viInstrumentHandle, AGSAN_VAL_INSTRUMENT_SETTING_REFERENCE_LEVEL));
	
Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_WaitForIntrumentDone( int hInstrumentHandle , double lfTimeOut )  
{
	STD_ERROR               StdError                    =   {0};        
	
	CHK_ERR( AgSAn_SystemWaitForOperationComplete ( viInstrumentHandle , ( lfTimeOut * 1E3 )));
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetSweepMode( int hInstrumentHandle , int bContinues )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( AgSAn_SetAttributeViBoolean( viInstrumentHandle , NULL , AGSAN_ATTR_SWEEP_MODE_CONTINUOUS , bContinues )); 
	
Error: 
	
	RETURN_STDERR_POINTER;
}


void* DLLEXPORT SpectrumAnalyzer_InitiateMeasure( int hInstrumentHandle )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( AgSAn_Initiate(viInstrumentHandle)); 
	
Error: 

	RETURN_STDERR_POINTER;
}


void* DLLEXPORT SpectrumAnalyzer_SetAverage( int hInstrumentHandle , int iAverage )  
{
	STD_ERROR               StdError                    =   {0};        

	if ( iAverage )
	{
		CHK_ERR( AgSAn_SetAttributeViBoolean( viInstrumentHandle , NULL , AGSAN_ATTR_SA_ACP_AVERAGE_ENABLED , 1 )); 

		CHK_ERR( AgSAn_SetAttributeViInt32( viInstrumentHandle , NULL , AGSAN_ATTR_SA_ACP_AVERAGE_COUNT , iAverage )); 
	}
	else
		CHK_ERR( AgSAn_SetAttributeViBoolean( viInstrumentHandle , NULL , AGSAN_ATTR_SA_ACP_AVERAGE_ENABLED , 0 )); 
	
Error: 
	
	RETURN_STDERR_POINTER;
}
  
void* DLLEXPORT SpectrumAnalyzer_RecallState( int hInstrumentHandle ,char *szFileName )  
{
	STD_ERROR               StdError             		=   {0};        

	char					*pszLoadSpectrumState		=	NULL;

	CALLOC_ERR( pszLoadSpectrumState , (strlen(szFileName)+32) , sizeof(char));
	
	sprintf( pszLoadSpectrumState , "MMEM:LOAD:STAT \"%s\"\n" , szFileName );
	
	CHK_ERR( AgSAn_SystemWrite ( viInstrumentHandle , "*CLS" ) ); 
	
	CHK_ERR( AgSAn_SystemWrite ( viInstrumentHandle , pszLoadSpectrumState ) ); 
	
	CALLOC( StdError.pszErrorDescription , STD_STRING , sizeof(char*));
				
	if ( StdError.pszErrorDescription )
		AgSAn_error_query ( viInstrumentHandle , &(StdError.error) , StdError.pszErrorDescription );
	
	if ( IS_NOT_OK )
	{
		if ( strstr( szFileName , ".sta" ) ) 
		{	   
			if ( strstr( szFileName , ":\\" ) )
				sprintf( pszLoadSpectrumState , "MMEM:LOAD:STAT 0,\"%s\"\n" , szFileName );
			else
				sprintf( pszLoadSpectrumState , "MMEM:LOAD:STAT 0,\"c:\\%s\"\n" , szFileName );
		}
		else
		{	   
			if ( strstr( szFileName , ":\\" ) )
				sprintf( pszLoadSpectrumState , "MMEM:LOAD:STAT 0,\"%s.sta\"\n" , szFileName );
			else
				sprintf( pszLoadSpectrumState , "MMEM:LOAD:STAT 0,\"c:\\%s.sta\"\n" , szFileName );
		}
		
		CHK_ERR( AgSAn_SystemWrite ( viInstrumentHandle , "*CLS" ) );
	
		CHK_ERR( AgSAn_SystemWrite ( viInstrumentHandle , pszLoadSpectrumState ) ); 
	}
	
	FREE_CALLOC( StdError.pszErrorDescription , STD_STRING , sizeof(char*));
				
	if ( StdError.pszErrorDescription )
		AgSAn_error_query ( viInstrumentHandle , &(StdError.error) , StdError.pszErrorDescription );
	
Error: 

	FREE(pszLoadSpectrumState);
	
	RETURN_STDERR_POINTER;
}  

void* DLLEXPORT SpectrumAnalyzer_SaveState( int hInstrumentHandle ,char *szFileName )  
{
	STD_ERROR               StdError                    =   {0};        

	char					*pszStoreSpectrumState		=	NULL;

	CALLOC_ERR( pszStoreSpectrumState , (strlen(szFileName)+32) , sizeof(char));
	
	sprintf( pszStoreSpectrumState , "MMEM:STOR:STAT \"%s\"\n" , szFileName );
	
	CHK_ERR( AgSAn_SystemWrite ( viInstrumentHandle , "*CLS" ) ); 
	
	CHK_ERR( AgSAn_SystemWrite ( viInstrumentHandle , pszStoreSpectrumState ) ); 
	
	CALLOC( StdError.pszErrorDescription , STD_STRING , sizeof(char*));
				
	if ( StdError.pszErrorDescription )
		AgSAn_error_query ( viInstrumentHandle , &(StdError.error) , StdError.pszErrorDescription );
	
	if ( IS_NOT_OK )
	{
		if ( strstr( szFileName , ".sta" ) ) 
		{	   
			if ( strstr( szFileName , ":\\" ) )
				sprintf( pszStoreSpectrumState , "MMEM:STOR:STAT 0,\"%s\"\n" , szFileName );
			else
				sprintf( pszStoreSpectrumState , "MMEM:STOR:STAT 0,\"c:\\%s\"\n" , szFileName );
		}
		else
		{	   
			if ( strstr( szFileName , ":\\" ) )
				sprintf( pszStoreSpectrumState , "MMEM:STOR:STAT 0,\"%s.sta\"\n" , szFileName );
			else
				sprintf( pszStoreSpectrumState , "MMEM:STOR:STAT 0,\"c:\\%s.sta\"\n" , szFileName );
		}
		
		CHK_ERR( AgSAn_SystemWrite ( viInstrumentHandle , "*CLS" ) );
	
		CHK_ERR( AgSAn_SystemWrite ( viInstrumentHandle , pszStoreSpectrumState ) ); 
	}
	
	FREE_CALLOC( StdError.pszErrorDescription , STD_STRING , sizeof(char*));
				
	if ( StdError.pszErrorDescription )
		AgSAn_error_query ( viInstrumentHandle , &(StdError.error) , StdError.pszErrorDescription );
	
Error: 

	FREE(pszStoreSpectrumState);
	
	RETURN_STDERR_POINTER;
} 

void* DLLEXPORT SpectrumAnalyzer_Recall( int hInstrumentHandle ,char *szFileName )  
{
	return SpectrumAnalyzer_RecallState( viInstrumentHandle ,szFileName );
} 

void* DLLEXPORT SpectrumAnalyzer_Read_EVM( int hInstrumentHandle , double *plfEVM_Value )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szString[128]						=	{0};

	int						iActualSize							=	0;
	
	sprintf( szString , ":READ:DDEMod?\n" );
	
	CHK_ERR( AgSAn_SystemWrite ( viInstrumentHandle , szString ));
	
	CHK_ERR( AgSAn_SystemRead ( viInstrumentHandle , 20 , szString , &iActualSize ));  
	
	if ( plfEVM_Value )
		*plfEVM_Value = atof(szString);
	
Error: 

	RETURN_STDERR_POINTER;
}  


void* DLLEXPORT SpectrumAnalyzer_SetToMode_Specrum( int hInstrumentHandle )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( AgSAn_SA_SystemPersistentPresetCurrentMode(viInstrumentHandle));
	
Error: 

	RETURN_STDERR_POINTER;
} 

void* DLLEXPORT SpectrumAnalyzer_SetFrequency_Specrum( int hInstrumentHandle , double lfFrequency )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( AgSAn_SetAttributeViReal64 ( viInstrumentHandle , 0 , AGSAN_ATTR_SA_FREQUENCY_CENTER , lfFrequency ));;
Error: 

	RETURN_STDERR_POINTER;
} 

void* DLLEXPORT SpectrumAnalyzer_SetFrequency( int hInstrumentHandle , double lfFrequency )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( AgSAn_SetAttributeViReal64 ( viInstrumentHandle , 0 , AGSAN_ATTR_SA_FREQUENCY_CENTER , lfFrequency ));
	
Error: 

	RETURN_STDERR_POINTER;
} 

void* DLLEXPORT SpectrumAnalyzer_SetFrequency_EVM( int hInstrumentHandle , double lffrequency )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( AgSAn_SetAttributeViReal64 ( viInstrumentHandle , 0 , AGSAN_ATTR_EDGEGSM_FREQUENCY_CENTER , lffrequency ));;
Error: 

	RETURN_STDERR_POINTER;
} 

void* DLLEXPORT SpectrumAnalyzer_RestartMeasure( int hInstrumentHandle )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( AgSAn_SystemWrite ( viInstrumentHandle , "INIT:REST\n" )); 
Error: 

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
	
	iNumberOfSymbols *= 16;
	
	CALLOC_ERR( pszBuffer , iNumberOfSymbols+1 , sizeof(char));
	CALLOC_ERR( pszSymbolSream , iNumberOfSymbols+1 , sizeof(char)); 
	
	for( iTryMeasureIndex = 0; iTryMeasureIndex < 5; iTryMeasureIndex++ )
	{
		sprintf( szString , ":CALC:DDEM:DATA4?\n" );
	
		STDERR( AgSAn_SystemWrite ( viInstrumentHandle , szString ));
	
		if ( IS_NOT_OK )
			continue;
		
		DelayWithEventProcessing(0.5);
	
		STDERR( AgSAn_SystemRead ( viInstrumentHandle , iNumberOfSymbols , pszBuffer , &iNumberOfSymbolsRead ));  
		
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
	
Error: 

	RETURN_STDERR_POINTER;
} 

int main (int argc, char *argv[])
{
	int					status				=	0;
	
	char				szOutputText[1024]	=	{0};
	
	ViSession 			hInstrumentHandle	=	0;
		
	int					bConnectStatus		=	0,
						ChannelsQuantity	=	0,
						Count				=	0; 
							
	double				lfFrequency			=	0.0,
						lfPower				=	0.0;
	
	double 				*pTrace				=	NULL;
							  
	char 				*pSymbolStream			=	NULL;
		
	int					iNumberOfSymbolsRead	=	0;
	
	InitCVIRTE (0, argv, 0 ); 

	SpectrumAnalyzer_Init ( 0 , "GPIB::18" , &hInstrumentHandle , &bConnectStatus , &ChannelsQuantity );
	
	SpectrumAnalyzer_Reset(hInstrumentHandle);
	
	SpectrumAnalyzer_SaveState( hInstrumentHandle , "spart" );
	
	SpectrumAnalyzer_RecallState( hInstrumentHandle , "calib" );  
	
	SpectrumAnalyzer_AutoTune( hInstrumentHandle , 20.0 );
		
	SpectrumAnalyzer_PhaseNoise_GetMarker ( hInstrumentHandle , 1 , 1 , &lfFrequency , &lfPower );   
	SpectrumAnalyzer_PhaseNoise_GetMarker ( hInstrumentHandle , 1 , 2 , &lfFrequency , &lfPower );   
	SpectrumAnalyzer_PhaseNoise_GetMarker ( hInstrumentHandle , 1 , 3 , &lfFrequency , &lfPower );   
	SpectrumAnalyzer_PhaseNoise_GetMarker ( hInstrumentHandle , 1 , 4 , &lfFrequency , &lfPower );   
	
	SpectrumAnalyzer_GetMarkerMeasure ( hInstrumentHandle , 0 , 1 , &lfFrequency , &lfPower );
		
	//SpectrumAnalyzer_RecallState( hInstrumentHandle , "spart.state" );
		
	//  

	//SpectrumAnalyzer_WaitForIntrumentDone(hInstrumentHandle,20000);
	
	
	
	AgSAn_SA_SystemPersistentPresetCurrentMode (hInstrumentHandle);
	
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
	
	SpectrumAnalyzer_SearchMarkerPeak( hInstrumentHandle );  

	SpectrumAnalyzer_SearchMarkerNextPeak( hInstrumentHandle );  
	
	SpectrumAnalyzer_Reset(hInstrumentHandle);	 
		
	SpectrumAnalyzer_ConfigSource( hInstrumentHandle , 1 ); 
		
	SpectrumAnalyzer_SetSweepMode( hInstrumentHandle , 0 ); 
		
	SpectrumAnalyzer_InitiateMeasure( hInstrumentHandle );
		
	SpectrumAnalyzer_SetSweepMode( hInstrumentHandle , 1 ); 
		
	SpectrumAnalyzer_SetMarkerFrequency ( hInstrumentHandle , 15E9 );
	
	SpectrumAnalyzer_SetCenterFrequencyFromMarker ( hInstrumentHandle );  
	
	SpectrumAnalyzer_SetReferenceLevelFrequencyFromMarker ( hInstrumentHandle ); 
	
	SpectrumAnalyzer_GetReferenceLevel( hInstrumentHandle , &lfPower );
		
	lfPower += 10;
	
	SpectrumAnalyzer_SetReferenceLevel( hInstrumentHandle , lfPower );
	
	SpectrumAnalyzer_SetNumberOfPoints( hInstrumentHandle , 1001 ); 
		
	SpectrumAnalyzer_GetTrace( hInstrumentHandle , 20000 , &pTrace , 20000 , &Count );
		
	SpectrumAnalyzer_GetErrorTextMessage ( hInstrumentHandle , status , szOutputText );
		
	SpectrumAnalyzer_Close(&hInstrumentHandle);	
	
	return 0;
	
}

