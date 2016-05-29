//==============================================================================
// Include files
#include "Windows.h"
#include "toolbox.h"
#include <ansi_c.h>
#include <userint.h>
#include <utility.h>
#include "Anritsu_MS2668XX_Spectrum_Wrapper.h"
#include <visa.h>
#include "GlobalDefines.h"

//==============================================================================
// Constants

#define				MAX_TIMEOUT			20.0

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
int		CkeckComplete( int handle , char *pCommand , char *pCompare , double lfTimeout );

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


/***** ***** ***** ***** ***** ***** Get Error Text Message ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT SpectrumAnalyzer_GetErrorTextMessage ( ViSession vhInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR				StdError					=	{0};    

	StdError.error = viStatusDesc( vhInstrumentHandle , iError , pErrorMessage );
	
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
									bSupport									=	0, 
									defaultRM									=	0,
									hConnectionHandle							=	0;
	
	char							szIdentificationText[LOW_STRING]			=	{0};
	
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
	
		if ( strstr( szIdentificationText , ",MS266" ))
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

void* DLLEXPORT SpectrumAnalyzer_Init ( ViSession hParentInstrumentHandle , ViRsrc szRsrcAddress , ViSession *vhInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChannelsQuantity )
{			
	STD_ERROR				StdError					=	{0};    

	char		szErrorTextMessage[512]		=	{0};
	
	ViSession	hDefaultRM					=	0;
	
	if ( hParentInstrumentHandle == 0 )
	{
		viOpenDefaultRM (&hDefaultRM);
		
		StdError.error = viOpen ( hDefaultRM , szRsrcAddress , VI_FALSE , VI_FALSE , vhInstrumentHandle ); 
	
		if ( StdError.error )
		{
			MessagePopup("Error!" , "Spectrum Analyzer Initializing Error.");
		
			RETURN_STDERR_POINTER;
		}
	}
	
	if ( InstrumentConnectStatus )
		*InstrumentConnectStatus = 1;
	
	if ( InstrumentChannelsQuantity )
		*InstrumentChannelsQuantity = 1;
	
Error: 
	
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** Close ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT SpectrumAnalyzer_Close ( ViSession *pviInstrumentHandle )
{
	STD_ERROR				StdError					=	{0};    

	int 					iStatus						=	0;  
	
	if (pviInstrumentHandle)
	{
		iStatus = viClose ( *pviInstrumentHandle );
		*pviInstrumentHandle = 0;
	}
		
Error: 
	
	RETURN_STDERR_POINTER;
}


void* DLLEXPORT SpectrumAnalyzer_Reset ( ViSession viInstrumentHandle )
{   
	STD_ERROR				StdError					=	{0};    
	
	CHK_ERR( viPrintf ( viInstrumentHandle , "*RST\n"));
		
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));    
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_Preset ( ViSession viInstrumentHandle )
{   
	STD_ERROR				StdError					=	{0};    
	
	CHK_ERR( viPrintf ( viInstrumentHandle , "IP\n"));
		
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));    	

Error: 
	
	RETURN_STDERR_POINTER;

}

void* DLLEXPORT SpectrumAnalyzer_SetAuto ( ViSession viInstrumentHandle )
{
	STD_ERROR				StdError					=	{0};    

	CHK_ERR( viPrintf ( viInstrumentHandle , "PRESEL AUTO\n"));
		
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetFrequencyCenterSpan ( ViSession viInstrumentHandle , int channel , double lfFrequency , double lfSpan )
{   
	STD_ERROR				StdError					=	{0};    
	
	CHK_ERR( viPrintf ( viInstrumentHandle , "SP %0.0lfHZ\n" , lfSpan ));
		
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
	CHK_ERR( viPrintf ( viInstrumentHandle , "CF %0.0lfHZ\n" , lfFrequency ));  
	
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_ConfigureMarker ( ViSession viInstrumentHandle , int channel , int marker , int bState )
{
	char		szMarkerName[512]		=	{0};

	STD_ERROR				StdError					=	{0};    
	
	CHK_ERR( viPrintf ( viInstrumentHandle , "MKR %d\n" , marker ));  
	
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetMarkerSearchPeakAuto ( ViSession viInstrumentHandle , int channel , int bMax , int bState )
{   
	STD_ERROR				StdError					=	{0};    
	
	CHK_ERR( viPrintf ( viInstrumentHandle , "MKPK\n"));
		
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}
	
void* DLLEXPORT SpectrumAnalyzer_GetMarkerMeasure ( ViSession viInstrumentHandle , int channel , int marker , double *pFrequency , double *pPower )
{
	STD_ERROR				StdError					=	{0};    
	
	CHK_ERR( viPrintf ( viInstrumentHandle , "MKL?\n"));
		
	CHK_ERR( viScanf ( viInstrumentHandle , "%lf" , pPower ));
	
	CHK_ERR( viPrintf ( viInstrumentHandle , "MKF?\n"));
		
	CHK_ERR( viScanf ( viInstrumentHandle , "%lf" , pFrequency ));
	
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetTrace( ViSession viInstrumentHandle , int timeout , double **pTrace , int numberOfPoints , int *pCount )
{
	STD_ERROR				StdError					=	{0};    
	
	double					*plfTraceData				=	NULL;
	
	char					*pCurrentNumber				=	NULL,
							*pBuffer					=	NULL,
							*pTemp						=	NULL, 
							*pNext						=	NULL; 
								
	int						iCount						=	0,
							iTraceCounter				=	0;  
								
	if ( numberOfPoints > 500 )
		numberOfPoints = 500;
	
	CALLOC_ERR( plfTraceData , numberOfPoints , sizeof(double)); 
	CALLOC_ERR( pBuffer , ( numberOfPoints * 20 ) , sizeof(char));  
	
	if ( timeout > 0.0 )
	{
		CHK_ERR( CkeckComplete( viInstrumentHandle , "TS;SWP?\n" , "S" , timeout ));  
	}
	
	CHK_ERR( viPrintf ( viInstrumentHandle , "XMA? 1,%d\n" , numberOfPoints ));
	
	DelayWithEventProcessing(0.3);
	
	CHK_ERR( viRead ( viInstrumentHandle , (ViPBuf)pBuffer , ( numberOfPoints * 20 ) , (ViPUInt32)&iCount ));
	
	pNext = pBuffer;
	
	do
	{
		pCurrentNumber = pNext;
		
		pTemp = strchr( pNext , ',' );
		
		if ( pTemp )
		{
			*pTemp = 0;
			pNext = pTemp+1;
		}
		else
			pNext = pTemp;
		
		plfTraceData[iTraceCounter++] = (atof(pCurrentNumber) / 100.0);
		
	}while(pNext);
	
	
Error: 
	
	if (pCount)
		*pCount = iTraceCounter;
	
	if ( pTrace )
	{
		*pTrace = plfTraceData;
	}
	else
	{
		FREE(plfTraceData);	
	}
	
	FREE(pBuffer);

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetSweep( ViSession viInstrumentHandle , double timeout )
{
	STD_ERROR				StdError					=	{0};    

	CHK_ERR( CkeckComplete( viInstrumentHandle , "TS;SWP?\n" , "S" , timeout ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetMaxHold ( ViSession viInstrumentHandle , int  bState )
{
	STD_ERROR				StdError					=	{0};    

	CHK_ERR( viPrintf ( viInstrumentHandle , "AMD %d\n" , bState ));
		
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_ConfigChannelPowerMeasuring( ViSession viInstrumentHandle , double lfBandWidth )  
{
	STD_ERROR				StdError					=	{0};    
	
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetChannelPowerResult( ViSession viInstrumentHandle , double *plfValue )
{
	STD_ERROR				StdError					=	{0};    
	
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetStartStopFrequency( ViSession viInstrumentHandle , double *plfStartFrequency , double *plfStopFrequency )
{
	STD_ERROR				StdError					=	{0};    
	
	CHK_ERR( viPrintf ( viInstrumentHandle , "FA?\n"));
	CHK_ERR( viScanf ( viInstrumentHandle , "%lf" , plfStartFrequency )); 
		
	CHK_ERR( viPrintf ( viInstrumentHandle , "FB?\n"));
	CHK_ERR( viScanf ( viInstrumentHandle , "%lf" , plfStopFrequency )); 
		
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_ConfigBandWidth( ViSession viInstrumentHandle , double lfResolutionBandWidth , double lfVideoBandWidth )  
{
	STD_ERROR				StdError					=	{0};    
	
	if ( lfVideoBandWidth < 0.0 )
	{
		CHK_ERR( viPrintf ( viInstrumentHandle , "AVB 1\n" ));  
	}
	else
	{
		CHK_ERR( viPrintf ( viInstrumentHandle , "AVB 0\n" )); 
		CHK_ERR( viPrintf ( viInstrumentHandle , "VB %0.0lfHZ\n" , lfVideoBandWidth ));  
	}
	
	if ( lfResolutionBandWidth < 0.0 )
	{
		CHK_ERR( viPrintf ( viInstrumentHandle , "ARB 1\n" ));  
	}
	else
	{
		CHK_ERR( viPrintf ( viInstrumentHandle , "ARB 0\n" )); 
		CHK_ERR( viPrintf ( viInstrumentHandle , "RB %0.0lfHZ\n" , lfResolutionBandWidth ));  
	}
	
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}
	
void* DLLEXPORT SpectrumAnalyzer_ConfigSource( ViSession viInstrumentHandle , int bExternal )  
{
	STD_ERROR				StdError					=	{0};    

	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetNumberOfPoints( ViSession viInstrumentHandle , int points )  
{
	STD_ERROR				StdError					=	{0};    
	
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}



void* DLLEXPORT SpectrumAnalyzer_SetScaleDev( ViSession viInstrumentHandle , double lfScaleDev )  
{
	STD_ERROR				StdError					=	{0};    

	CHK_ERR( viPrintf ( viInstrumentHandle , "LG %0.0lfDB\n" , lfScaleDev ));
		
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_Attenuator( ViSession viInstrumentHandle , double lfAttenuator )  
{
	STD_ERROR				StdError					=	{0};    

	if ( lfAttenuator < 0.0 )
	{
		CHK_ERR( viPrintf ( viInstrumentHandle , "ATT 0\n" ));
	}
	else
	{
		CHK_ERR( viPrintf ( viInstrumentHandle , "ATT 1\n" )); 
		CHK_ERR( viPrintf ( viInstrumentHandle , "AT %0.0lf\n" , lfAttenuator ));
	}
	
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetReferenceLevel( ViSession viInstrumentHandle , double lfReferenceLevel )  
{
	STD_ERROR				StdError					=	{0};    

	CHK_ERR( viPrintf ( viInstrumentHandle , "RL %0.2lf\n" , lfReferenceLevel ));
		
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetReferenceLevel( ViSession viInstrumentHandle , double *vlfReferenceLevel )  
{
	STD_ERROR				StdError					=	{0};    

	CHK_ERR( viPrintf ( viInstrumentHandle , "RL?\n"));
		
	CHK_ERR( viScanf ( viInstrumentHandle , "%lf" , vlfReferenceLevel )); 
		
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetMarkerType( ViSession viInstrumentHandle , int bDelta )  
{
	STD_ERROR				StdError					=	{0};    

	if ( bDelta )
	{
		CHK_ERR( viPrintf ( viInstrumentHandle , "M3\n"));
	}
	else
	{
		CHK_ERR( viPrintf ( viInstrumentHandle , "M2\n"));
	}	
		
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetMarkerFrequency( ViSession viInstrumentHandle , double lfFrequency )  
{
	STD_ERROR				StdError					=	{0};    

	CHK_ERR( viPrintf ( viInstrumentHandle , "MKZF %0.0lfHZ\n" , lfFrequency ));
		
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetMarkerTimePosition( ViSession viInstrumentHandle , double lfPosition )  
{
	STD_ERROR				StdError					=	{0};    

	lfPosition *= 1E6;
	
	CHK_ERR( viPrintf ( viInstrumentHandle , "MKZF %0.3lfUS\n" , lfPosition ));
		
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SearchMarkerPeak( ViSession viInstrumentHandle  )  
{
	STD_ERROR				StdError					=	{0};    

	CHK_ERR( viPrintf ( viInstrumentHandle , "MKPK\n"));
		
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SearchMarkerNextPeak( ViSession viInstrumentHandle  )  
{
	STD_ERROR				StdError					=	{0};    

	CHK_ERR( viPrintf ( viInstrumentHandle , "MKPK NH\n"));
		
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetMarkerFrequency( ViSession viInstrumentHandle , int iMarkerNr , double *vlfFrequency )  
{
	STD_ERROR				StdError					=	{0};    

	CHK_ERR( viPrintf ( viInstrumentHandle , "MKF?\n"));
		
	CHK_ERR( viScanf ( viInstrumentHandle , "%lf" , vlfFrequency ));
			
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetMarkerAmplitude( ViSession viInstrumentHandle , int iMarkerNr , double *vlfAmplitude )  
{
	STD_ERROR				StdError					=	{0};    

	CHK_ERR( viPrintf ( viInstrumentHandle , "MKL?\n"));
		
	CHK_ERR( viScanf ( viInstrumentHandle , "%lf" , vlfAmplitude ));
	
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT )); 
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetMarkerDeltaFrequency( ViSession viInstrumentHandle , double *vlfFrequency )  
{
	STD_ERROR				StdError					=	{0};    

	CHK_ERR( viPrintf ( viInstrumentHandle , "MKF?\n"));
		
	CHK_ERR( viScanf ( viInstrumentHandle , "%lf" , vlfFrequency ));
			
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetMarkerDeltaAmplitude( ViSession viInstrumentHandle , double *vlfAmplitude )  
{
	STD_ERROR				StdError					=	{0};    

	CHK_ERR( viPrintf ( viInstrumentHandle , "MKL?\n"));
		
	CHK_ERR( viScanf ( viInstrumentHandle , "%lf" , vlfAmplitude ));
	
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetCenterFrequencyFromMarker( ViSession viInstrumentHandle )  
{
	STD_ERROR				StdError					=	{0};    

	CHK_ERR( viPrintf ( viInstrumentHandle , "PCF\n"));
		
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetReferenceLevelFrequencyFromMarker( ViSession viInstrumentHandle )  
{
	STD_ERROR				StdError					=	{0};    

	CHK_ERR( viPrintf ( viInstrumentHandle , "PRL\n"));
		
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_WaitForIntrumentDone( ViSession viInstrumentHandle , double lfTimeOut )  
{
	STD_ERROR				StdError					=	{0};    

	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetSweepMode( ViSession viInstrumentHandle , int bContinues )  
{
	STD_ERROR				StdError					=	{0};    

	if ( bContinues )
	{
		CHK_ERR( viPrintf ( viInstrumentHandle , "CONTS\n"));
	}
	else
	{
		CHK_ERR( viPrintf ( viInstrumentHandle , "S2\n"));
	}
	
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}


void* DLLEXPORT SpectrumAnalyzer_InitiateMeasure( ViSession viInstrumentHandle )  
{
	STD_ERROR				StdError					=	{0};    

	CHK_ERR( viPrintf ( viInstrumentHandle , "INI\n"));
		
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}


void* DLLEXPORT SpectrumAnalyzer_SetAverage( ViSession viInstrumentHandle , int iAverage )  
{
	STD_ERROR				StdError					=	{0};    

	CHK_ERR( viPrintf ( viInstrumentHandle , "VAVG %d\n" , iAverage ));
		
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}
  
void* DLLEXPORT SpectrumAnalyzer_Recall( ViSession viInstrumentHandle ,char *szFileName )  
{
	STD_ERROR				StdError					=	{0};
		
	CHK_ERR( viPrintf ( viInstrumentHandle , "RCM %s\n" , szFileName ));
	
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}  
	
void* DLLEXPORT SpectrumAnalyzer_Read_EVM( ViSession viInstrumentHandle , double *plfEVM_Value )  
{
	STD_ERROR				StdError					=	{0};

Error: 
	
	RETURN_STDERR_POINTER;
}  

void* DLLEXPORT SpectrumAnalyzer_SetToMode_Specrum( ViSession viInstrumentHandle )  
{
	STD_ERROR				StdError					=	{0};    

	CHK_ERR( viPrintf ( viInstrumentHandle , "IP\n"));
		
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetFrequency( ViSession viInstrumentHandle , double lfFrequency )  
{
	STD_ERROR				StdError					=	{0};    

	CHK_ERR( viPrintf ( viInstrumentHandle , "CF %0.0lfHZ\n" , lfFrequency ));  
	
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}
	
void* DLLEXPORT SpectrumAnalyzer_SetFrequency_EVM( ViSession viInstrumentHandle , double lffrequency )  
{
	STD_ERROR				StdError					=	{0};    

Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_RestartMeasure( ViSession viInstrumentHandle )  
{
	STD_ERROR				StdError					=	{0};    

	CHK_ERR( viPrintf ( viInstrumentHandle , "INI\n"));
		
	CHK_ERR( Delay_LastComplete( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}  

void* DLLEXPORT SpectrumAnalyzer_Read_SymbolStream( ViSession viInstrumentHandle , int iNumberOfSymbols , char **pSymbolStream , int *piNumberOfSymbolsRead )  
{
	STD_ERROR				StdError							=	{0};    
	
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
	
	pszBuffer = calloc(iNumberOfSymbols+1,sizeof(char));
	pszSymbolSream = calloc(iNumberOfSymbols+1,sizeof(char)); 
	
	if ((pszBuffer==NULL)||(pszSymbolSream==NULL))
		return NULL;
	
	for( iTryMeasureIndex = 0; iTryMeasureIndex < 5; iTryMeasureIndex++ )
	{
		sprintf( szString , ":CALC:DDEM:DATA4?\n" );
	
		CHK_VSA( viPrintf ( viInstrumentHandle , szString ));
	
		if ( IS_NOT_OK )
			continue;
		
		DelayWithEventProcessing(0.5);
	
		CHK_VSA( viRead ( viInstrumentHandle , (ViPBuf)pszBuffer , iNumberOfSymbols , (ViPUInt32)&iNumberOfSymbolsRead ));  
		
		if ( iNumberOfSymbolsRead > 2 )
			break;
	}
	
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
		
	pszOutputSymbolSream = calloc(iSymbolCounter+1,sizeof(char)); 
	memcpy(pszOutputSymbolSream,pszSymbolSream,iSymbolCounter);
	
	if ( pSymbolStream )
		*pSymbolStream = pszOutputSymbolSream;
	
	if ( piNumberOfSymbolsRead )
		*piNumberOfSymbolsRead = iSymbolCounter;
	
Error: 
	
	RETURN_STDERR_POINTER;
}


int		CkeckComplete( int handle , char *pCommand , char *pCompare , double lfTimeout )
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
		
		status = viPrintf ( handle , pCommand );
	
		if ( status )
			continue;
		
		Delay ( 0.2 );
		
		status = viRead ( handle , (ViPBuf)szBuffer , 1 , (ViPUInt32)&iCount);

		if ( iCount )
			if ( strstr( szBuffer , pCompare ))
				iOpcValue = 1;
			
		if ( status < 0 )
			continue;
		
		if ( iOpcValue == 1 )
			return 0;
	}
	
	return -1;	
}

int		Delay_LastComplete( int handle , double lfTimeout )   
{
	return CkeckComplete( handle , "*OPC?\n" , "1" , lfTimeout );
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
	
	SpectrumAnalyzer_SetMarkerType( hInstrumentHandle , 0 );
	
	SpectrumAnalyzer_SetMarkerTimePosition( hInstrumentHandle , 50e-6 );
		
	SpectrumAnalyzer_SetMarkerType( hInstrumentHandle , 1 ); 
		
	SpectrumAnalyzer_SetMarkerTimePosition( hInstrumentHandle , 150e-6 );
		
	SpectrumAnalyzer_GetMarkerAmplitude( hInstrumentHandle, 0 , &lfPower );  
	
	SpectrumAnalyzer_GetTrace( hInstrumentHandle , 0 , &pTrace , 500 , &Count );  
		
	SpectrumAnalyzer_Reset( hInstrumentHandle ); 
	
	SpectrumAnalyzer_Recall( hInstrumentHandle , "1" );  
		
	SpectrumAnalyzer_SetMaxHold ( hInstrumentHandle , 1 ); 
	
	SpectrumAnalyzer_SetMaxHold ( hInstrumentHandle , 0 ); 
	
	lfFrequency = 1.09E9;
	
	SpectrumAnalyzer_SetFrequency( hInstrumentHandle , lfFrequency );
	
	lfFrequency += 1.09E9; 
	
	SpectrumAnalyzer_SetFrequency( hInstrumentHandle , lfFrequency ); 
	
	lfFrequency += 1.09E9; 
	
	SpectrumAnalyzer_SetFrequency( hInstrumentHandle , lfFrequency ); 
	
	SpectrumAnalyzer_SetMaxHold ( hInstrumentHandle , 1 );
	
	SpectrumAnalyzer_SearchMarkerPeak ( hInstrumentHandle ); 
		
	SpectrumAnalyzer_GetMarkerAmplitude( hInstrumentHandle, 0 , &lfPower );  
		
	SpectrumAnalyzer_Close(&hInstrumentHandle);	

	return 0;
	
}

