//==============================================================================
// Include files

#include "Windows.h"
#include "toolbox.h"
#include <ansi_c.h>
#include <userint.h>
#include <utility.h>
#include <visa.h>
#include "GlobalDefines.h"
#include <formatio.h>
#include "hpesa.h"

//==============================================================================
// Constants

//==============================================================================
// Constants

#define				MAX_TIMEOUT			5000

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


int	SpectrumAnalyzer_error_query ( int hInstrumentHandle , int *piError , char *pszErrorDescription )
{
	int				error								=	0;

	char			szReadMessage[STD_STRING]			=	{0},
					szStartString[LOW_STRING]			=	{0},
					*pDescription						=	NULL,
					*pTemp								=	NULL;
	
	hpesa_cmdString_Q (hInstrumentHandle, ":SYST:ERR?" , STD_STRING , szReadMessage );

	sscanf( szReadMessage , "%d,\"%s\"" , &error , szStartString );
	
	pDescription = strstr( szReadMessage , szStartString );
		
	if ( pDescription )
		pTemp = strrchr( pDescription , '\"' );  
		
	if ( pTemp )
		*pTemp = 0;
	
	if ( pszErrorDescription )
		strcpy( pszErrorDescription , pDescription );
	
	if ( piError )
		*piError = error;
	
	return error;
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
	
	int				handle							=	0;
	
	//Ivi_GetAttributeViSession ( sessionHandle , VI_NULL, IVI_ATTR_IO_SESSION, 0, &handle );
	
	handle = sessionHandle;
	
	viGetAttribute ( handle , VI_ATTR_TMO_VALUE , &iTimeout );   
	
	SetBreakOnLibraryErrors (0); 
	
	viSetAttribute ( handle , VI_ATTR_TMO_VALUE , (lfLowLevelTimeout*1E3) );  
	
	GetCurrentDateTime(&lfStartTime);
	
	do
	{
		viPrintf( handle , "*OPC?\n" );
						
		viRead( handle, szReadFeedback , LOW_STRING , &count );
	
		iOpcStatus = atoi(szReadFeedback);
		
		if ( iOpcStatus )
			break;
		
		GetCurrentDateTime(&lfCurrentTime);
		
	} while((lfCurrentTime-lfStartTime) < lfTimeout );
	
	viSetAttribute ( handle , VI_ATTR_TMO_VALUE , iTimeout );  
	
	SetBreakOnLibraryErrors (1); 
	
	return iOpcStatus;
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
		IF (( strstr( szIdentificationText , "Hewlett-Packard" ) == NULL ) , "Is not supported" );
	
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
									vszSupportdedModels[][12]					=	{{"E4401B"},{"E4402B"},{"E4403B"},{"E4404B"},{"E4405B"},{"E4407B"},{"E4408B"},{"E4411B"}};
									
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
		if (( strstr( szIdentificationText ,"Agilent") == NULL ) && ( strstr( szIdentificationText ,"Hewlett-Packard") == NULL ))
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

/***** ***** ***** ***** ***** ***** Get Error Text Message ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT SpectrumAnalyzer_GetErrorTextMessage ( int hInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR			StdError							=	{0};
	
	char				szErrorDescription[STD_STRING]		=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	SpectrumAnalyzer_error_query ( hInstrumentHandle , &(StdError.error) , szErrorDescription );
	
	if ( pErrorMessage )
	{
		strcpy( pErrorMessage , szErrorDescription ); 
	}
	
	CALLOC_COPY_STRING( StdError.pszErrorDescription , szErrorDescription );
	
Error:

	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** Init ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT SpectrumAnalyzer_Init ( ViSession hParentInstrumentHandle , ViRsrc szRsrcAddress , ViSession *vhInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChannelsQuantity )
{			
	STD_ERROR				StdError							=	{0};    

	int						hVisaOpenLockHandle					=	0;
	
	int						bGetVisaLock						=	0;

	char					szReadFeedback[LOW_STRING]			=	{0};

	int						iTryCalibration						=	0,
							iCalibrationStatus					=	0;
	
	int						handle								=	0;
	
	int						count								=	0;
	
	if ( hParentInstrumentHandle == 0 )
	{
		CHK_CMT( CmtNewLock ( VISA_OPEN_LOCK , OPT_TL_PROCESS_EVENTS_WHILE_WAITING , &hVisaOpenLockHandle ));   

		if ( hVisaOpenLockHandle )
		{
			CHK_CMT( CmtGetLock ( hVisaOpenLockHandle ));
			bGetVisaLock = 1;
		}
		
		STDERR( hpesa_init ( szRsrcAddress , VI_FALSE , VI_FALSE , &handle )); 
		
		*vhInstrumentHandle = handle;
		
		IF (( IS_NOT_OK ) , "Spectrum Analyzer Initializing Error." );    
		
		for ( iTryCalibration = 0; iTryCalibration < 3; iTryCalibration++ )
		{
			viPrintf( handle , ":STAT:QUES:CAL:COND?\n" );
						
			viRead( handle, szReadFeedback , LOW_STRING , &count );
		
			iCalibrationStatus = atoi(szReadFeedback);
		
			if ( iCalibrationStatus > 0x800 )
			{
				MessagePopup ("Spectrum Aligning", "Connect Amptd Ref Output to Input."); 
	
				SetBreakOnLibraryErrors (0);   
				
				CHK_ERR( hpesa_alignDoAlignment ( handle , hpesa_ALIGN_ALL ));
				
				WaitForOperationCompletion( handle , 500.0 , 2.0 );  
				
				SetBreakOnLibraryErrors (1); 
			}
			
			if ( iCalibrationStatus < 0x800 )
				break;
		}
		
		IF (( iCalibrationStatus > 0x800 ) , "Can't align the spectrum analyzer." );
		
		hpesa_alignAutoSetMode ( handle , hpesa_ALIGN_AUTO_OFF );
		
		viSetAttribute ( handle , VI_ATTR_TMO_VALUE , 5000 );   
	}
	
	if ( InstrumentConnectStatus )
		*InstrumentConnectStatus = 1;
	
	if ( InstrumentChannelsQuantity )
		*InstrumentChannelsQuantity = 1;
	
Error: 
	
	if ( hVisaOpenLockHandle && bGetVisaLock ) 
	{
		CmtReleaseLock (hVisaOpenLockHandle);
		CmtDiscardLock (hVisaOpenLockHandle); 
	}
	
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** Close ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT SpectrumAnalyzer_Close ( ViSession *phInstrumentHandle )
{
	STD_ERROR				StdError					=	{0};    

	if (phInstrumentHandle)
	{
		hpesa_alignAutoSetMode ( *phInstrumentHandle , hpesa_ALIGN_AUTO_NRF ); 
		
		hpesa_close ( *phInstrumentHandle );
		*phInstrumentHandle = 0;
	}
		
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_Reset ( ViSession hInstrumentHandle )
{   
	STD_ERROR				StdError					=	{0};    
	
	CHK_ERR( hpesa_reset ( hInstrumentHandle ));
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_Preset ( ViSession hInstrumentHandle )
{   
	STD_ERROR				StdError					=	{0};    
	
	CHK_ERR( hpesa_cmd ( hInstrumentHandle , ":SYST:PRES" ));

Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetAuto ( ViSession hInstrumentHandle )
{
	STD_ERROR               StdError                    =   {0};         

	CHK_ERR( hpesa_couplingSetAuto ( hInstrumentHandle , 1 ));    
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_AutoTune ( ViSession hInstrumentHandle , double lfTimeout )
{
	STD_ERROR               StdError                    =   {0};        

Error: 

	RETURN_STDERR_POINTER;
} 
	
void* DLLEXPORT SpectrumAnalyzer_AlignNow ( ViSession hInstrumentHandle , double lfTimeout )
{
	STD_ERROR               StdError                    =   {0};        

	if ( ConfirmPopup ("Spectrum Aligning", "Connect Amptd Ref Output to Input.") )
	{
		CHK_ERR( hpesa_alignDoAlignment ( hInstrumentHandle , hpesa_ALIGN_ALL ));
	}
	
Error: 

	RETURN_STDERR_POINTER;
} 

void* DLLEXPORT SpectrumAnalyzer_SetFrequencyCenterSpan ( ViSession hInstrumentHandle , int channel , double lfFrequency , double lfSpan )
{   
	STD_ERROR               StdError                    =   {0};         
	
	CHK_ERR( hpesa_frequencySetCenter ( hInstrumentHandle , lfFrequency ));
	
	CHK_ERR( hpesa_frequencySetSpan ( hInstrumentHandle , lfSpan ));

Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_ConfigureMarker ( ViSession hInstrumentHandle , int channel , int marker , int bState )
{
	STD_ERROR               StdError                    =   {0};         

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetMarkerSearchPeakAuto ( ViSession hInstrumentHandle , int channel , int bMax , int bState )
{   
	STD_ERROR               StdError                    =   {0};         
	
	if ( bMax )
	{
		CHK_ERR( hpesa_markerPeakSearch ( hInstrumentHandle , hpesa_MARKER_1, hpesa_MARKER_PEAK ));
	}
	else
	{
		CHK_ERR( hpesa_markerPeakSearch ( hInstrumentHandle , hpesa_MARKER_1, hpesa_MARKER_MIN ));
	}
	
	WaitForOperationCompletion( hInstrumentHandle , 5.0 , 0.5 ); 
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetMarkerFrequencyCounter( ViSession hInstrumentHandle , int marker , int bState )
{   
	STD_ERROR               StdError                    =   {0};         
	
	if ( marker > 0 )
		marker--;
	
	CHK_ERR( hpesa_freqCounterSetOn( hInstrumentHandle , marker , bState ));
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_MeasureMarkerFrequencyCounter ( ViSession hInstrumentHandle , int marker , double lfTimeout , double *pFrequency )
{
	STD_ERROR               StdError                    =   {0};     
	
	double					lfFrequency					=	0.0;
	
	int						iTimeout					=	0;
	
	if ( marker > 0 )
		marker--;
	
	viGetAttribute ( hInstrumentHandle , VI_ATTR_TMO_VALUE , &iTimeout );   
	
	SetBreakOnLibraryErrors (0); 
	
	viSetAttribute ( hInstrumentHandle , VI_ATTR_TMO_VALUE , (lfTimeout*1E3) );  
	
	CHK_ERR( hpesa_freqCounterGetXValue( hInstrumentHandle , &lfFrequency )); 
	
	if ( pFrequency )
		*pFrequency = lfFrequency;
Error: 
	
	viSetAttribute ( hInstrumentHandle , VI_ATTR_TMO_VALUE , iTimeout );  
	
	SetBreakOnLibraryErrors (1); 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetMarkerMeasure ( ViSession hInstrumentHandle , int channel , int marker , double *pFrequency , double *pPower )
{
	STD_ERROR               StdError                    =   {0};     
	
	double					lfFrequency					=	0.0,
							lfPower						=	0.0;
	
	if ( marker > 0 )
		marker--;
	
	CHK_ERR( hpesa_markerGetYValue ( hInstrumentHandle , (ViInt16)marker , &lfPower ));
	CHK_ERR( hpesa_markerGetXValue( hInstrumentHandle , (ViInt16)marker , &lfFrequency )); 
	
	if ( pFrequency )
		*pFrequency = lfFrequency;
	
	if ( pPower )
		*pPower = lfPower;
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_PhaseNoise_GetMarker ( ViSession hInstrumentHandle , int channel , int marker , double *pPosition , double *pValue )
{
	STD_ERROR               StdError                    =   {0};     
	
	double					lfFrequency					=	0.0,
							lfPower						=	0.0;
	
	ViBoolean				sweepWaitComplete			=	0;
	
	if ( marker > 0 )
		marker--;
	
	CHK_ERR( hpesa_markerSetFunction ( hInstrumentHandle , (ViInt16)marker, hpesa_MARKER_NOISE));
		
	CHK_ERR( hpesa_sweepWaitForSweepComplete ( hInstrumentHandle , 3.0 , &sweepWaitComplete ));
	
	CHK_ERR( hpesa_markerGetYValue ( hInstrumentHandle , (ViInt16)marker , &lfPower ));
	CHK_ERR( hpesa_markerGetXValue( hInstrumentHandle , (ViInt16)marker , &lfFrequency )); 
	
	if ( pPosition )
		*pPosition = lfFrequency;
	
	if ( pValue )
		*pValue = lfPower;
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetTrace( ViSession hInstrumentHandle , int timeout , double **pTrace , int numberOfPoints , int *pCount )
{
	STD_ERROR               StdError                    =   {0};     

	int						iReadNumberOfPoints			=	0;
	
	IF (( pTrace == NULL ) , "NULL pointer" );   
	
	FREE ( *pTrace );
		
	FREE_CALLOC_ERR ( *pTrace , numberOfPoints , sizeof(double));
	
	IF (( *pTrace == NULL ) , "NULL pointer" );
	  
	hpesa_traceGetTraceData ( hInstrumentHandle , hpesa_TRACE_1, numberOfPoints , *pTrace , &iReadNumberOfPoints );

	if ( pCount )
		*pCount = iReadNumberOfPoints;
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetMaxHold ( ViSession hInstrumentHandle , int  bState )
{
	STD_ERROR               StdError                    =   {0};     

	switch ( bState )
	{
		case 0:
					CHK_ERR( hpesa_traceSetMode (hInstrumentHandle, hpesa_TRACE_1, hpesa_TRACE_WRITE));
					break;
		case 1:
					
					CHK_ERR( hpesa_traceSetMode (hInstrumentHandle, hpesa_TRACE_1, hpesa_TRACE_MAXHOLD));
					break;	
					
		default:
					break;
	}
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_ConfigChannelPowerMeasuring( ViSession hInstrumentHandle , double lfBandWidth )  
{
	STD_ERROR               StdError                    =   {0};     
	
Error: 
	
	RETURN_STDERR_POINTER;
}


void* DLLEXPORT SpectrumAnalyzer_GetChannelPowerResult( ViSession hInstrumentHandle , double *plfValue )
{
	return NULL;
}

void* DLLEXPORT SpectrumAnalyzer_GetStartStopFrequency( ViSession hInstrumentHandle , double *plfStartFrequency , double *plfStopFrequency )
{
	STD_ERROR               StdError                    =   {0};    
	
	if ( plfStartFrequency )
		CHK_ERR( hpesa_frequencyGetStart ( hInstrumentHandle , plfStartFrequency )); 

	if ( plfStopFrequency )
		CHK_ERR( hpesa_frequencyGetStop ( hInstrumentHandle , plfStopFrequency )); 
	
Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetStartStopFrequency( ViSession hInstrumentHandle , double lfStartFrequency , double lfStopFrequency )
{
	STD_ERROR               StdError                    =   {0};    
	
	CHK_ERR( hpesa_frequencySetStart ( hInstrumentHandle , lfStartFrequency )); 

	CHK_ERR( hpesa_frequencySetStop ( hInstrumentHandle , lfStopFrequency )); 
	
Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_ConfigBandWidth( ViSession hInstrumentHandle , double lfResolutionBandWidth , double lfVideoBandWidth )  
{
	STD_ERROR               StdError                    =   {0};    
	
	if ( lfResolutionBandWidth > 0.0 )
	{
		CHK_ERR( hpesa_BWSetResolution (hInstrumentHandle, lfResolutionBandWidth ));
	}
	else
		CHK_ERR( hpesa_BWSetResolutionAuto (hInstrumentHandle));
	
	if ( lfVideoBandWidth > 0.0 )
	{
		CHK_ERR( hpesa_BWSetVideo (hInstrumentHandle, lfVideoBandWidth ));
	}
	else
		CHK_ERR( hpesa_BWSetVideoAuto (hInstrumentHandle));
	
Error: 

	RETURN_STDERR_POINTER;
}
	
void* DLLEXPORT SpectrumAnalyzer_ConfigSource( ViSession hInstrumentHandle , int bExternal )  
{
	STD_ERROR               StdError                    =   {0};    

	RETURN_STDERR_POINTER;
}	

void* DLLEXPORT SpectrumAnalyzer_SetNumberOfPoints( ViSession hInstrumentHandle , int points )  
{
	STD_ERROR               StdError                    =   {0};        

	RETURN_STDERR_POINTER;
}



void* DLLEXPORT SpectrumAnalyzer_SetScaleDev( ViSession hInstrumentHandle , double lfScaleDev )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hpesa_amplitudeSetScale ( hInstrumentHandle , lfScaleDev )); 
	
Error: 
		
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_Attenuator( ViSession hInstrumentHandle , double lfAttenuator )  
{
	STD_ERROR               StdError                    =   {0};        

	if ( lfAttenuator >= 0.0 )
	{
		CHK_ERR( hpesa_amplitudeSetAttenuation ( hInstrumentHandle , lfAttenuator )); 
	}
	else
		CHK_ERR( hpesa_amplitudeSetAttenuationAuto ( hInstrumentHandle )); 
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetAttenuator( ViSession hInstrumentHandle , double *plfAttenuator )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hpesa_amplitudeGetAttenuation ( hInstrumentHandle , plfAttenuator )); 

Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetReferenceLevel( ViSession hInstrumentHandle , double lfReferenceLevel )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hpesa_amplitudeSetReferenceLevel ( hInstrumentHandle , lfReferenceLevel )); 
	
Error:
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetReferenceLevel( ViSession hInstrumentHandle , double *vlfReferenceLevel )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hpesa_amplitudeGetReferenceLevel(  hInstrumentHandle , vlfReferenceLevel ));

Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetMarkerType( ViSession hInstrumentHandle , int bDelta )  
{
	STD_ERROR               StdError                    =   {0};        

	if ( bDelta )
	{
		CHK_ERR( hpesa_markerSetMode ( hInstrumentHandle , hpesa_MARKER_1, hpesa_MARKER_DELTA));
	}
	else
	{
		CHK_ERR( hpesa_markerSetMode ( hInstrumentHandle , hpesa_MARKER_1, hpesa_MARKER_NORMAL));
	}
	
Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetMarkerFrequency( ViSession hInstrumentHandle , double lfFrequency )  
{
	STD_ERROR               StdError                    =   {0};        
	
	CHK_ERR( hpesa_markerSetXPosition ( hInstrumentHandle , hpesa_MARKER_1 , lfFrequency ));

Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SearchMarkerPeak( ViSession hInstrumentHandle  )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hpesa_markerPeakSearch ( hInstrumentHandle , hpesa_MARKER_1, hpesa_MARKER_PEAK));

	//WaitForOperationCompletion( hInstrumentHandle , 5.0 , 0.5 ); 
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SearchMarkerNextPeak( ViSession hInstrumentHandle  )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hpesa_markerPeakSearch ( hInstrumentHandle , hpesa_MARKER_1, hpesa_MARKER_PEAKNEXT));

	//WaitForOperationCompletion( hInstrumentHandle , 5.0 , 0.5 ); 
	
Error: 
	
	RETURN_STDERR_POINTER;
}																		

void* DLLEXPORT SpectrumAnalyzer_GetMarkerFrequency( ViSession hInstrumentHandle , int iMarkerNr , double *vlfFrequency )  
{
	STD_ERROR               StdError                    =   {0};        

	if ( iMarkerNr > 0 ) 
		iMarkerNr--;
	
	CHK_ERR( hpesa_markerGetXValue( hInstrumentHandle , (ViInt16)iMarkerNr , vlfFrequency )); 

Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetMarkerAmplitude( ViSession hInstrumentHandle , int iMarkerNr , double *vlfAmplitude )  
{
	STD_ERROR               StdError                    =   {0};        

	if ( iMarkerNr > 0 ) 
		iMarkerNr--;
	
	CHK_ERR( hpesa_markerGetYValue( hInstrumentHandle , (ViInt16)iMarkerNr , vlfAmplitude )); 

Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetMarkerDeltaFrequency( ViSession hInstrumentHandle , double *vlfFrequency )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hpesa_markerGetXValue( hInstrumentHandle , hpesa_MARKER_1 , vlfFrequency )); 

Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetMarkerDeltaAmplitude( ViSession hInstrumentHandle , double *vlfAmplitude )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hpesa_markerGetYValue( hInstrumentHandle , hpesa_MARKER_1 , vlfAmplitude )); 

Error: 

	RETURN_STDERR_POINTER;
}



void* DLLEXPORT SpectrumAnalyzer_SetCenterFrequencyFromMarker( ViSession hInstrumentHandle )  
{
	STD_ERROR               StdError                    =   {0};        
	
	CHK_ERR( hpesa_markerToMarker ( hInstrumentHandle , hpesa_MARKER_1, hpesa_MARKER_CENTERFREQ));
	
Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetReferenceLevelFrequencyFromMarker( ViSession hInstrumentHandle )  
{
	STD_ERROR               StdError                    =   {0};        
	
	CHK_ERR( hpesa_markerToMarker (hInstrumentHandle, hpesa_MARKER_1, hpesa_MARKER_REFLEVEL));
	
Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_WaitForIntrumentDone( ViSession hInstrumentHandle , double lfTimeOut )  
{
	STD_ERROR               StdError                    =   {0};        
																												  
	IF ( ( WaitForOperationCompletion( hInstrumentHandle , 5.0 , 0.5 ) == 0 ) , "Instrument hasn't responded." ); 
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetSweepMode( ViSession hInstrumentHandle , int bContinues )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hpesa_sweepSetMode ( hInstrumentHandle , bContinues )); 

Error: 
	
	RETURN_STDERR_POINTER;
}


void* DLLEXPORT SpectrumAnalyzer_InitiateMeasure( ViSession hInstrumentHandle )  
{
	STD_ERROR               StdError                    =   {0};        

	ViBoolean				sweepWaitComplete			=	0;
	
	CHK_ERR( hpesa_sweepWaitForSweepComplete ( hInstrumentHandle , 3.0 , &sweepWaitComplete ));
	
	WaitForOperationCompletion( hInstrumentHandle , 5.0 , 0.5 );
	
Error: 

	RETURN_STDERR_POINTER;
}


void* DLLEXPORT SpectrumAnalyzer_SetAverage( ViSession hInstrumentHandle , int iType  , int iAverage )  
{
	STD_ERROR               StdError                    =   {0};        
	
	if ( iAverage > 0 )
	{
		if ( iType )
		{
			CHK_ERR( hpesa_BWSetAverage (hInstrumentHandle, hpesa_AVERAGE_POWER, iAverage )); 
		}
		else
		{
			CHK_ERR( hpesa_BWSetAverage (hInstrumentHandle, hpesa_AVERAGE_LPOWER, iAverage ));  
		}
		
		CHK_ERR( hpesa_BWSetAverageOn (hInstrumentHandle, VI_TRUE ));
	}
	else
	{
		CHK_ERR( hpesa_BWSetAverageOn (hInstrumentHandle, VI_FALSE ));
	}
	
Error:
	
	RETURN_STDERR_POINTER;
}
  
void* DLLEXPORT SpectrumAnalyzer_SetOffset( ViSession hInstrumentHandle , double lfOffset )  
{
	STD_ERROR               StdError                    =   {0};        
	
	CHK_ERR( hpesa_amplitudeSetRefLevelOffset ( hInstrumentHandle , lfOffset ));
	
Error:
	
	RETURN_STDERR_POINTER;
}	 

void* DLLEXPORT SpectrumAnalyzer_RecallState( int hInstrumentHandle ,char *szFileName )  
{
	STD_ERROR               StdError             		=   {0};        

	char					*pszLoadSpectrumState		=	NULL;

	CALLOC_ERR( pszLoadSpectrumState , (strlen(szFileName)+32) , sizeof(char));
	
	sprintf( pszLoadSpectrumState , "MMEM:LOAD:STAT \"%s\"\n" , szFileName );
	
	CHK_ERR( hpesa_cmd ( hInstrumentHandle , "*CLS" ) ); 
	
	CHK_ERR( hpesa_cmd ( hInstrumentHandle , pszLoadSpectrumState ) ); 
	
	CALLOC( StdError.pszErrorDescription , STD_STRING , sizeof(char*));
				
	SpectrumAnalyzer_error_query ( hInstrumentHandle , &(StdError.error) , StdError.pszErrorDescription );
	
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
		
		CHK_ERR( hpesa_cmd ( hInstrumentHandle , "*CLS" ) );
	
		CHK_ERR( hpesa_cmd ( hInstrumentHandle , pszLoadSpectrumState ) ); 
	}
	
	FREE_CALLOC( StdError.pszErrorDescription , STD_STRING , sizeof(char*));
				
	SpectrumAnalyzer_error_query ( hInstrumentHandle , &(StdError.error) , StdError.pszErrorDescription );
	
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
	
	CHK_ERR( hpesa_cmd ( hInstrumentHandle , "*CLS" ) ); 
	
	CHK_ERR( hpesa_cmd ( hInstrumentHandle , pszStoreSpectrumState ) ); 
	
	CALLOC( StdError.pszErrorDescription , STD_STRING , sizeof(char*));
				
	SpectrumAnalyzer_error_query ( hInstrumentHandle , &(StdError.error) , StdError.pszErrorDescription );
	
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
		
		CHK_ERR( hpesa_cmd ( hInstrumentHandle , "*CLS" ) );
	
		CHK_ERR( hpesa_cmd ( hInstrumentHandle , pszStoreSpectrumState ) ); 
	}
	
	FREE_CALLOC( StdError.pszErrorDescription , STD_STRING , sizeof(char*));
				
	SpectrumAnalyzer_error_query ( hInstrumentHandle , &(StdError.error) , StdError.pszErrorDescription );
	
Error: 

	FREE(pszStoreSpectrumState);
	
	RETURN_STDERR_POINTER;
} 



int		RecursiveMakeDirectory( int sessionHandle , char *pPath )
{
	char		szCurrentPath[STD_STRING]		=	{0},
				szReadFeedback[STD_STRING]		=	{0},
				*pTemp							=	NULL;
														
	char		szEmptyDirectory[]				=	{"0,0,\"\"\n"};
	
	int			count							=	0,
				iTotalCount						=	0;
	
	int			status							=	0;
	
	int			iTimeout						=	0;
	
	if ( pPath == NULL )
		return -1;
	
	strcpy( szCurrentPath , pPath );
	
	viPrintf( sessionHandle , "MMEM:CAT? \"%s\"\n" , szCurrentPath );
	
	viGetAttribute ( sessionHandle , VI_ATTR_TMO_VALUE , &iTimeout );  
	
	SetBreakOnLibraryErrors (0);
	
	viSetAttribute ( sessionHandle , VI_ATTR_TMO_VALUE , 1000 );  
	
	do
	{
		viRead( sessionHandle, szReadFeedback , (STD_STRING-1) , &count );
		
		iTotalCount += count;
		
	} while(count);
	
	viSetAttribute ( sessionHandle , VI_ATTR_TMO_VALUE , iTimeout ); 
	
	SetBreakOnLibraryErrors (1);
	
	if ( iTotalCount == 0 ) 
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
				   
				DelayWithEventProcessing(0.5);
				
				viPrintf( sessionHandle , "MMEM:CAT? \"%s\"\n" , pPath );
	
				SetBreakOnLibraryErrors (0);
				
				viSetAttribute ( sessionHandle , VI_ATTR_TMO_VALUE , 1000 );   
				
				do
				{
					viRead( sessionHandle, szReadFeedback , (STD_STRING-1) , &count );
		
					iTotalCount += count;
		
				} while(count);
				
				viSetAttribute ( sessionHandle , VI_ATTR_TMO_VALUE , iTimeout ); 
				
				SetBreakOnLibraryErrors (1);
				
				status = iTotalCount;
			}
			
			return status;
		}
	}

	return 0;
}

void* DLLEXPORT SpectrumAnalyzer_SendFile( int hInstrumentHandle ,char *szLocalFileName ,char *szStateFileName  )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szLoadSpectrumState[512]			=	{0},
							szDirectoryPath[512]				=	{0},
							szFileSize[32]						=	{0};
	
	int						iActualSize							=	0;
	
	int						iFileSize							=	0,
							iReadSize							=	0, 
							iWriteCount							=	0,
							iFileHandle							=	0;
	
	char					*pFileBuffer						=	NULL,
							*pStateFile							=	NULL,  
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
	
	viPrintf( hInstrumentHandle , "*CLS\n" );
	
	strcpy( szDirectoryPath , szStateFileName );
	
	pTemp = strrchr( szDirectoryPath , '\\' );
	
	if ( pTemp )
	{
		*pTemp = 0;
	}
	
	if ( strstr( szDirectoryPath , ":\\" ))
	{
		RecursiveMakeDirectory( hInstrumentHandle , szDirectoryPath );
	}
	
	if ( strstr( szDirectoryPath , ":" ) == NULL )     
		strcpy( szDirectoryPath , "C:" );
		
	pTemp = strrchr( szStateFileName , '\\' );  
	
	if ( pTemp )
	{
		pStateFile = pTemp+1;		
	}
	else
	{
		pStateFile = szStateFileName;
	}
	
	viPrintf( hInstrumentHandle , "*CLS\n" );
	
	if ( strlen(szDirectoryPath))
		sprintf( szLoadSpectrumState , ":MMEM:DATA \"%s\\%s\",#%d%s" , szDirectoryPath , pStateFile , strlen(szFileSize) , szFileSize );
	else
		sprintf( szLoadSpectrumState , ":MMEM:DATA \"%s\",#%d%s" , szStateFileName , strlen(szFileSize) , szFileSize );

	CHK_VSA ( viPrintf( hInstrumentHandle , szLoadSpectrumState ));
	
	CHK_VSA ( viBufWrite( hInstrumentHandle , pFileBuffer , iFileSize ,&iWriteCount ));
	
	CHK_VSA ( viPrintf( hInstrumentHandle , "\n" )); 

Error: 

	FREE(pFileBuffer);
	
	if ( iFileHandle )
		CloseFile ( iFileHandle );
	
	if ( StdError.error == 0 )
		SpectrumAnalyzer_error_query ( hInstrumentHandle , &(StdError.error) , StdError.pszErrorDescription );
	
	RETURN_STDERR_POINTER;
} 

void* DLLEXPORT Equipment_SendStateFile ( int hInstrumentHandle , char *szLocalFileName ,char *szStateFileName  )
{
	return SpectrumAnalyzer_SendFile( hInstrumentHandle , szLocalFileName , szStateFileName );	
}

void* DLLEXPORT SpectrumAnalyzer_ReceiveFile( int hInstrumentHandle , char *szLocalFileName ,char *szStateFileName  )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szLoadSpectrumState[512]			=	{0},
							szDirectoryPath[512]				=	{0},
							szReadFeedback[LOW_STRING]			=	{0}, 
							szFileSize[32]						=	{0};
	
	int						iActualSize							=	0;
	
	int						iFileSize							=	0,
							iReadSize							=	0, 
							iWriteCount							=	0,
							iWriteSize						 	=	0,  
							iFileHandle							=	0;
	
	char					*pTemp								=	NULL,
							*pStateFile							=	NULL,  
							*pReadBuffer						=	NULL; 
							
	unsigned int			uiCount								=	0,
							uiNumberOfBinaryData				=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	IF (( szLocalFileName == NULL ), "File Name is empty" );
	
	IF (( szStateFileName == NULL ), "State File Name is empty" ); 
	
	iFileHandle = OpenFile ( szLocalFileName , VAL_WRITE_ONLY, VAL_TRUNCATE, VAL_BINARY);
	
	IF ( ( iFileHandle == 0 ) , "Can't open local file." );
	
	viPrintf( hInstrumentHandle , "*CLS\n" );

	pTemp = strrchr( szStateFileName , '\\' );  
	
	if ( pTemp )
	{
		pStateFile = pTemp+1;		
	}
	else
	{
		pStateFile = szStateFileName;
	}
	
	strcpy( szDirectoryPath , szStateFileName );
	
	if ( strstr( szDirectoryPath , ":" ) == NULL )     
	{
		strcpy( szDirectoryPath , "C:" );
	
		sprintf( szLoadSpectrumState , ":MMEM:DATA? \"%s\\%s\"\n" , szDirectoryPath , pStateFile );   
	}
	else
	{
		sprintf( szLoadSpectrumState , ":MMEM:DATA? \"%s\"\n" , szStateFileName );   
	}
		
	CHK_VSA ( viPrintf( hInstrumentHandle , szLoadSpectrumState ));
	
	do 
	{
		SetBreakOnLibraryErrors (0); 
		viRead ( hInstrumentHandle ,(ViPBuf)szReadFeedback , 1 , &uiCount );
		SetBreakOnLibraryErrors (1); 

		if ( szReadFeedback[0] == '#' )
			break;

	} while(uiCount);

	IF (( szReadFeedback[0] != '#' ) , "Wrong trace reading format" );  

	memset( szReadFeedback , 0 , LOW_STRING );

	SetBreakOnLibraryErrors (0); 
	viRead ( hInstrumentHandle ,(ViPBuf)szReadFeedback , 1 , &uiCount );
	SetBreakOnLibraryErrors (1); 

	IF (( uiCount == 0 ) , "Wrong trace reading format" );  

	uiCount = atoi(szReadFeedback); 

	memset( szReadFeedback , 0 , LOW_STRING );

	SetBreakOnLibraryErrors (0); 
	viRead ( hInstrumentHandle ,(ViPBuf)szReadFeedback , uiCount , &uiCount );  
	SetBreakOnLibraryErrors (1); 

	uiNumberOfBinaryData = atoi(szReadFeedback); 

	SetBreakOnLibraryErrors (0);
	
	CALLOC_ERR( pReadBuffer , uiNumberOfBinaryData , sizeof(char)); 
	
	viRead ( hInstrumentHandle ,(ViPBuf)pReadBuffer , uiNumberOfBinaryData , &uiCount ); 
	
	SetBreakOnLibraryErrors (1);

	IF (( uiNumberOfBinaryData != uiCount ) , "Wrong trace reading format" );     

	iWriteSize = WriteFile ( iFileHandle , pReadBuffer , uiNumberOfBinaryData );

	IF (( iWriteSize != uiNumberOfBinaryData ) , "Write File error" ); 
	
	SetBreakOnLibraryErrors (0);
	viRead ( hInstrumentHandle ,(ViPBuf)szReadFeedback , 1 , &uiCount );  
	SetBreakOnLibraryErrors (1);

Error: 

	FREE(pReadBuffer);
	
	if ( iFileHandle )
		CloseFile ( iFileHandle );
	
	if ( StdError.error == 0 )
		SpectrumAnalyzer_error_query ( hInstrumentHandle , &(StdError.error) , StdError.pszErrorDescription );
	
	RETURN_STDERR_POINTER;
} 

void* DLLEXPORT Equipment_ReceiveStateFile ( int hInstrumentHandle , char *szLocalFileName ,char *szStateFileName  )
{
	return SpectrumAnalyzer_ReceiveFile( hInstrumentHandle , szLocalFileName , szStateFileName );	
}

void* DLLEXPORT SpectrumAnalyzer_GetFileCatalog ( int hInstrumentHandle , char *szPath , char ***szFileNames , int **pvFileSizes , int *piNumberOfFiles )
{
	STD_ERROR               StdError                  			=   {0};        

	char					szLoadSpectrumCatalog[512]			=	{0},
							szDirectoryPath[512]				=	{0},
							szPreviousFeedback[512]				=	{0},
							szReadFeedback[LOW_STRING]			=	{0}, 
							szCurrentBuffer[512]				=	{0};
	
	int						iFileIndex							=	0;
	
	char					*pTemp								=	NULL,
							*pCurrent							=	NULL,
							*pNext								=	NULL; 
							
	int 					iNumberOfFile						=	0,
							iAllocatedMemory					=	0;

	unsigned int			uiCount								=	0,
							uiCurrentCount						=	0,
							uiPreviousCount						=	0;
								
	int						iTimeout							=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	viPrintf( hInstrumentHandle , "*CLS\n" ); 
	
	if ( szPath )
		strcpy( szDirectoryPath , szPath );
	
	if ( strstr( szDirectoryPath , ":" ) == NULL )     
	{	
		strcpy( szDirectoryPath , "C:" );
	
		if ( szPath )
		{
			if ( szPath[0] == '\\' )
				sprintf( szLoadSpectrumCatalog , ":MMEM:CAT? \"%s%s\"\n" , szDirectoryPath , szPath );   
			else
				sprintf( szLoadSpectrumCatalog , ":MMEM:CAT? \"%s\\%s\"\n" , szDirectoryPath , szPath );
		}
		else
		{
			sprintf( szLoadSpectrumCatalog , ":MMEM:CAT? \"%s\"\n" , szDirectoryPath );   	
		}
	}
	else
	{
		sprintf( szLoadSpectrumCatalog , ":MMEM:CAT? \"%s\"\n" , szPath );   
	}
	
	CHK_VSA ( viPrintf( hInstrumentHandle , szLoadSpectrumCatalog ));
	
	viGetAttribute ( hInstrumentHandle , VI_ATTR_TMO_VALUE , &iTimeout );    
	
	viSetAttribute ( hInstrumentHandle , VI_ATTR_TMO_VALUE , 2000 );  

	do
	{
		strcpy( szCurrentBuffer , szPreviousFeedback );
		
		memset( szReadFeedback , 0 , LOW_STRING );

		SetBreakOnLibraryErrors (0); 
		viRead ( hInstrumentHandle ,(ViPBuf)szReadFeedback , (LOW_STRING-1) , &uiCount );  
		SetBreakOnLibraryErrors (1); 
	
		if ( uiCount == 0 )
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
			
			pTemp = strstr( pNext , "\"" );
			
			if (( pTemp == NULL ) && ( uiPreviousCount != uiCurrentCount ))
				break;
			
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


Error: 

	if ( hInstrumentHandle )
		viSetAttribute ( hInstrumentHandle , VI_ATTR_TMO_VALUE , iTimeout );  
	
	if ( piNumberOfFiles )
		*piNumberOfFiles = iNumberOfFile;
	
	if ( StdError.error == 0 )
		SpectrumAnalyzer_error_query ( hInstrumentHandle , &(StdError.error) , StdError.pszErrorDescription );
	
	RETURN_STDERR_POINTER;
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
							szDirectoryPath[512]				=	{0},
							szMessage[LOW_STRING]				=	{0};

	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	IF (( szStateFileName == NULL ), "State File Name is empty" ); 
	
	viPrintf( hInstrumentHandle , "*CLS\n" );

	strcpy( szDirectoryPath , szStateFileName );
	
	if ( strstr( szDirectoryPath , ":" ) == NULL )     
	{
		strcpy( szDirectoryPath , "C:" );
	
		if ( szStateFileName[0] == '\\' )
			sprintf( szDeleteSpectrumState , ":MMEM:DEL \"%s%s\"\n" , szDirectoryPath , szStateFileName );
		else
			sprintf( szDeleteSpectrumState , ":MMEM:DEL \"%s\\%s\"\n" , szDirectoryPath , szStateFileName );   
	}
	else
	{
		sprintf( szDeleteSpectrumState , ":MMEM:DEL \"%s\"\n" , szStateFileName );   
	}
	
	sprintf( szMessage , "Are you sure you want to delete state file\n\"%s\"?" , szStateFileName );
	
	if ( ConfirmPopup ("Deleting Equipment State File", szMessage) )
	{
		CHK_VSA ( viPrintf( hInstrumentHandle , szDeleteSpectrumState ));
	}
	
Error: 
	
	if ( StdError.error == 0 )
		SpectrumAnalyzer_error_query ( hInstrumentHandle , &(StdError.error) , StdError.pszErrorDescription );
	
	RETURN_STDERR_POINTER;
} 

void* DLLEXPORT Equipment_DeleteStateFile ( int hInstrumentHandle , char *szStateFileName  )
{
	return SpectrumAnalyzer_DeleteFile( hInstrumentHandle , szStateFileName );	
}


void* DLLEXPORT SpectrumAnalyzer_DeleteFileCatalog( int hInstrumentHandle , char *szCatalogName  )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szDeleteSpectrumCatalog[512]		=	{0},
							szDirectoryPath[512]				=	{0},
							szMessage[LOW_STRING]				=	{0};

	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	IF (( szCatalogName == NULL ), "State File Name is empty" ); 
	
	viPrintf( hInstrumentHandle , "*CLS\n" );

	strcpy( szDirectoryPath , szCatalogName );
	
	if ( strstr( szDirectoryPath , ":" ) == NULL )     
	{
		strcpy( szDirectoryPath , "C:" );
	
		if ( szCatalogName[0] == '\\' )
			sprintf( szDeleteSpectrumCatalog , ":MMEM:RDIR \"%s%s\"\n" , szDirectoryPath , szCatalogName );   
		else
			sprintf( szDeleteSpectrumCatalog , ":MMEM:RDIR \"%s\\%s\"\n" , szDirectoryPath , szCatalogName );   
	}
	else
	{
		sprintf( szDeleteSpectrumCatalog , ":MMEM:RDIR \"%s\"\n" , szCatalogName );   
	}
	
	sprintf( szMessage , "Are you sure you want to delete catalog \n\"%s\"?" , szCatalogName );
	
	if ( ConfirmPopup ("Deleting Equipment Catalog", szMessage) )
	{
		CHK_VSA ( viPrintf( hInstrumentHandle , szDeleteSpectrumCatalog ));
	}
	
Error: 
	
	if ( StdError.error == 0 )
		SpectrumAnalyzer_error_query ( hInstrumentHandle , &(StdError.error) , StdError.pszErrorDescription );
	
	RETURN_STDERR_POINTER;
} 

void* DLLEXPORT Equipment_DeleteFileCatalog ( int hInstrumentHandle , char *szCatalogName  )
{
	return SpectrumAnalyzer_DeleteFileCatalog( hInstrumentHandle , szCatalogName );	
}

void* DLLEXPORT SpectrumAnalyzer_Recall( int hInstrumentHandle ,char *szFileName )  
{
	return SpectrumAnalyzer_RecallState( hInstrumentHandle ,szFileName );
} 

void* DLLEXPORT SpectrumAnalyzer_Read_EVM( ViSession hInstrumentHandle , double *plfEVM_Value )  
{
	STD_ERROR               StdError                  			=   {0};        

	RETURN_STDERR_POINTER;
}  


void* DLLEXPORT SpectrumAnalyzer_SetToMode_Specrum( ViSession hInstrumentHandle )  
{
	STD_ERROR               StdError                    =   {0};        

	RETURN_STDERR_POINTER;
} 

void* DLLEXPORT SpectrumAnalyzer_SetFrequency_Specrum( ViSession hInstrumentHandle , double lfFrequency )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hpesa_frequencySetCenter ( hInstrumentHandle , lfFrequency ));
	
Error: 

	RETURN_STDERR_POINTER;
} 

void* DLLEXPORT SpectrumAnalyzer_SetFrequency( ViSession hInstrumentHandle , double lfFrequency )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hpesa_frequencySetCenter ( hInstrumentHandle , lfFrequency ));
	
Error: 

	RETURN_STDERR_POINTER;
} 

void* DLLEXPORT SpectrumAnalyzer_SetFrequency_EVM( ViSession hInstrumentHandle , double lffrequency )  
{
	STD_ERROR               StdError                    =   {0};        

	RETURN_STDERR_POINTER;
} 

void* DLLEXPORT SpectrumAnalyzer_RestartMeasure( ViSession hInstrumentHandle )  
{
	STD_ERROR               StdError                    =   {0};        
	   
	
Error: 

	RETURN_STDERR_POINTER;
} 

void* DLLEXPORT SpectrumAnalyzer_Read_SymbolStream( ViSession hInstrumentHandle , int iNumberOfSymbols , char **pSymbolStream , int *piNumberOfSymbolsRead )  
{
	STD_ERROR               StdError                  			=   {0};        
	
	RETURN_STDERR_POINTER;
} 


int main (int argc, char *argv[])
{
	ViSession 			hInstrumentHandle	=	0;
		
	int					bConnectStatus		=	0,
						ChannelsQuantity	=	0; 
							
	double				lfFrequency			=	0.0,
						lfPower				=	0.0;
	
	InitCVIRTE (0, argv, 0 ); 

	SpectrumAnalyzer_Init ( 0 , "GPIB::18" , &hInstrumentHandle , &bConnectStatus , &ChannelsQuantity );
	
	SpectrumAnalyzer_SetFrequency( hInstrumentHandle , 100E6 );
	
	SpectrumAnalyzer_InitiateMeasure( hInstrumentHandle ); 
	SpectrumAnalyzer_WaitForIntrumentDone( hInstrumentHandle , 20.0 );  
	SpectrumAnalyzer_SearchMarkerPeak( hInstrumentHandle );  
	SpectrumAnalyzer_GetMarkerAmplitude( hInstrumentHandle , 1 , &lfPower ); 

						
						
	SpectrumAnalyzer_Reset(hInstrumentHandle);
	
	SpectrumAnalyzer_RecallState( hInstrumentHandle , "calibration" );  
	
	SpectrumAnalyzer_AutoTune( hInstrumentHandle , 20.0 );
		
	SpectrumAnalyzer_PhaseNoise_GetMarker ( hInstrumentHandle , 1 , 1 , &lfFrequency , &lfPower );   
	SpectrumAnalyzer_PhaseNoise_GetMarker ( hInstrumentHandle , 1 , 2 , &lfFrequency , &lfPower );   
	SpectrumAnalyzer_PhaseNoise_GetMarker ( hInstrumentHandle , 1 , 3 , &lfFrequency , &lfPower );   
	SpectrumAnalyzer_PhaseNoise_GetMarker ( hInstrumentHandle , 1 , 4 , &lfFrequency , &lfPower );   
	
	SpectrumAnalyzer_GetMarkerMeasure ( hInstrumentHandle , 0 , 1 , &lfFrequency , &lfPower );
		
	//SpectrumAnalyzer_RecallState( hInstrumentHandle , "spart.state" );
		
	//  

	//SpectrumAnalyzer_WaitForIntrumentDone(hInstrumentHandle,20000);
	
																			   
	SpectrumAnalyzer_Close(&hInstrumentHandle);	
	
	return 0;
	
}


