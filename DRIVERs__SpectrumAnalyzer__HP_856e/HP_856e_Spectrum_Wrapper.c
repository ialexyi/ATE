//==============================================================================
// Include files

#include "Windows.h"
#include "toolbox.h"
#include <ansi_c.h>
#include <userint.h>
#include <utility.h>
#include "HP_856e_Spectrum_Wrapper.h"
#include <visa.h>
#include "GlobalDefines.h"
#include <formatio.h>
#include "hp8563e.h"

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


/***** ***** ***** ***** ***** ***** Get Error Text Message ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT SpectrumAnalyzer_GetErrorTextMessage ( ViSession vhInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR				StdError					=	{0};    

	FREE_CALLOC( StdError.pszErrorDescription , STD_STRING , sizeof(char*));
				
	if ( StdError.pszErrorDescription )
		
	{
		CHK_ERR( hp8563e_errorMessage ( vhInstrumentHandle , iError ,(char*)StdError.pszErrorDescription ));
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
		status = viPrintf ( hConnectionHandle , "ID?\n" );
	
		if ( status == 0 )
		{
			viRead ( hConnectionHandle , szIdentificationText , (LOW_STRING-1) , &iCount );
			RemoveSurroundingWhiteSpace (szIdentificationText); 
		}
	}
		
	do
	{
		strcpy( szIdentificationUpperText , szIdentificationText );
		StringUpperCase (szIdentificationUpperText);
		
		IF ((( strstr( szIdentificationUpperText , "AGILENT TECHNOLOGIES" ) == NULL ) && ( strstr( szIdentificationUpperText ,"HEWLETT-PACKARD") == NULL ) && ( strstr( szIdentificationUpperText ,"HP") == NULL )) , "Is not supported" );
	
		if ( strstr( szIdentificationUpperText ,"HP") == NULL )
		{
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
		}
		else
		{
			if ( pVendor )
			{
				CALLOC_COPY_STRING( *pVendor , "HP" );       
			}
			
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
			
			pTemp = strstr( szIdentificationText , "HP" );

			if ( pTemp )
			{
				*pTemp = 0;
				pTemp += 2;
	
				if ( pModel )
				{
					CALLOC_COPY_STRING( *pModel , pTemp );
				}
			}
			else
				break;
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
			status = viPrintf ( hConnectionHandle , "ID?\n" );
		
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
		
		if (( strstr( szIdentificationUpperText , "AGILENT TECHNOLOGIES" ) == NULL ) && ( strstr( szIdentificationUpperText ,"HEWLETT-PACKARD") == NULL ) && ( strstr( szIdentificationUpperText ,"HP") == NULL ))
			break;	
	
		if ( strstr( szIdentificationText , "856" ))
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

	if ( hParentInstrumentHandle == 0 )
	{
		STDERR( hp8563e_init ( szRsrcAddress , VI_FALSE , VI_FALSE , vhInstrumentHandle )); 
		
		IF (( IS_NOT_OK ) , "Spectrum Analyzer Initializing Error." );    
			
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

	if (pviInstrumentHandle)
	{
		hp8563e_close ( *pviInstrumentHandle );
		*pviInstrumentHandle = 0;
	}
		
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_Reset ( ViSession viInstrumentHandle )
{   
	STD_ERROR				StdError					=	{0};    
	
	CHK_ERR( hp8563e_reset ( viInstrumentHandle ));
	
	CHK_ERR( hp8563e_waitUntilDone ( viInstrumentHandle , MAX_TIMEOUT ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_Preset ( ViSession viInstrumentHandle )
{   
	STD_ERROR				StdError					=	{0};    
	
	unsigned int			iCount						=	0;
	
	CHK_ERR( viWrite ( viInstrumentHandle , (unsigned char*)"IP\n" , 3 , &iCount )); 

	CHK_ERR( hp8563e_waitUntilDone ( viInstrumentHandle , MAX_TIMEOUT ));    
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetAuto ( ViSession viInstrumentHandle )
{
	STD_ERROR               StdError                    =   {0};         

	CHK_ERR( hp8563e_configCoupl ( viInstrumentHandle , 0 , VI_OFF , 0 ));    
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_AutoTune ( ViSession viInstrumentHandle , double lfTimeout )
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hp8563e_waitUntilDone ( viInstrumentHandle , 10.0 ));    
	
Error: 

	RETURN_STDERR_POINTER;
} 
	
void* DLLEXPORT SpectrumAnalyzer_SetFrequencyCenterSpan ( ViSession viInstrumentHandle , int channel , double lfFrequency , double lfSpan )
{   
	STD_ERROR               StdError                    =   {0};         
	
	CHK_ERR( hp8563e_configFreq ( viInstrumentHandle , 0, lfFrequency ));
	
	CHK_ERR( hp8563e_waitUntilDone ( viInstrumentHandle , ( 10.0 * 1E3 )));   
		
	CHK_ERR( hp8563e_configSpan ( viInstrumentHandle , 0, lfSpan ));

	CHK_ERR( hp8563e_waitUntilDone ( viInstrumentHandle , ( 10.0 * 1E3 )));   
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_ConfigureMarker ( ViSession viInstrumentHandle , int channel , int marker , int bState )
{
	STD_ERROR               StdError                    =   {0};         

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetMarkerSearchPeakAuto ( ViSession viInstrumentHandle , int channel , int bMax , int bState )
{   
	STD_ERROR               StdError                    =   {0};         
	
	if ( bMax )
	{
		CHK_ERR( hp8563e_placeMarker ( viInstrumentHandle , 6 , 0.0 ));
	}
	else
	{
		CHK_ERR( hp8563e_placeMarker ( viInstrumentHandle , 4 , 0.0 ));
	}
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetMarkerMeasure ( ViSession viInstrumentHandle , int channel , int marker , double *pFrequency , double *pPower )
{
	STD_ERROR               StdError                    =   {0};     
	
	double					lfFrequency					=	0.0,
							lfPower						=	0.0;
	
	CHK_ERR( hp8563e_queryAmpl ( viInstrumentHandle , 0 , &lfPower ));
	CHK_ERR( hp8563e_queryFreq( viInstrumentHandle , 1 , &lfFrequency )); 
	
	if ( pFrequency )
		*pFrequency = lfFrequency;
	
	if ( pPower )
		*pPower = lfPower;
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_PhaseNoise_GetMarker ( ViSession viInstrumentHandle , int channel , int marker , double *pPosition , double *pValue )
{
	STD_ERROR               StdError                    =   {0};     
	
	double					lfFrequency					=	0.0,
							lfPower						=	0.0;
	
	CHK_ERR( hp8563e_confMkr( viInstrumentHandle , VI_ON , VI_OFF ));
		
	CHK_ERR( hp8563e_perfmSwp ( viInstrumentHandle ));
	
	CHK_ERR( hp8563e_queryAmpl( viInstrumentHandle , 0 , &lfPower )); 
	CHK_ERR( hp8563e_queryFreq( viInstrumentHandle , 1 , &lfFrequency )); 

	if ( pPosition )
		*pPosition = lfFrequency;
	
	if ( pValue )
		*pValue = lfPower;
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetTrace( ViSession viInstrumentHandle , int timeout , double **pTrace , int numberOfPoints , int *pCount )
{
	STD_ERROR               StdError                    =   {0};     

	IF (( pTrace == NULL ) , "NULL pointer" );   
	
	FREE ( *pTrace );
		
	FREE_CALLOC_ERR ( *pTrace , 601 , sizeof(double));
	
	IF (( *pTrace == NULL ) , "NULL pointer" );
		
	CHK_ERR( hp8563e_waitUntilDone ( viInstrumentHandle , MAX_TIMEOUT ));  
	
	CHK_ERR( hp8563e_queryTrace ( viInstrumentHandle , 1 , *pTrace ));

	if ( pCount )
		*pCount = 601;
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetMaxHold ( ViSession viInstrumentHandle , int  bState )
{
	STD_ERROR               StdError                    =   {0};     

	switch ( bState )
	{
		case 0:
					CHK_ERR( hp8563e_perfmTrace( viInstrumentHandle , 3 , 1 ));
					break;
		case 1:
					
					CHK_ERR( hp8563e_perfmTrace( viInstrumentHandle , 4 , 1 ));
					break;	
					
		default:
					break;
	}
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_ConfigChannelPowerMeasuring( ViSession viInstrumentHandle , double lfBandWidth )  
{
	STD_ERROR               StdError                    =   {0};     
	
	CHK_ERR( hp8563e_confChanPow ( viInstrumentHandle , 1 , lfBandWidth ));
	
Error: 
	
	RETURN_STDERR_POINTER;
}



void* DLLEXPORT SpectrumAnalyzer_GetChannelPowerResult( ViSession viInstrumentHandle , double *plfValue )
{
	return NULL;
}

void* DLLEXPORT SpectrumAnalyzer_GetStartStopFrequency( ViSession viInstrumentHandle , double *plfStartFrequency , double *plfStopFrequency )
{
	STD_ERROR               StdError                    =   {0};    
	
	if ( plfStartFrequency )
		CHK_ERR( hp8563e_queryFreq ( viInstrumentHandle , 4 , plfStartFrequency )); 

	if ( plfStopFrequency )
		CHK_ERR( hp8563e_queryFreq ( viInstrumentHandle , 5 , plfStopFrequency )); 
	
Error: 

	RETURN_STDERR_POINTER;
}


void* DLLEXPORT SpectrumAnalyzer_ConfigBandWidth( ViSession viInstrumentHandle , double lfResolutionBandWidth , double lfVideoBandWidth )  
{
	STD_ERROR               StdError                    =   {0};    
	
	if ( lfResolutionBandWidth > 0.0 )
	{
		CHK_ERR( hp8563e_configCoupl (viInstrumentHandle, 1, 1 , lfResolutionBandWidth ));
	}
	else
		CHK_ERR( hp8563e_configCoupl (viInstrumentHandle, 1, 0, 0.0 ));
	
	if ( lfVideoBandWidth > 0.0 )
	{
		CHK_ERR( hp8563e_configCoupl (viInstrumentHandle, 2, 1 , lfVideoBandWidth ));
	}
	else
		CHK_ERR( hp8563e_configCoupl (viInstrumentHandle, 2, 0, 0.0 ));
	
Error: 

	RETURN_STDERR_POINTER;
}
	
void* DLLEXPORT SpectrumAnalyzer_ConfigSource( ViSession viInstrumentHandle , int bExternal )  
{
	STD_ERROR               StdError                    =   {0};    

	RETURN_STDERR_POINTER;
}	

void* DLLEXPORT SpectrumAnalyzer_SetNumberOfPoints( ViSession viInstrumentHandle , int points )  
{
	STD_ERROR               StdError                    =   {0};        

	RETURN_STDERR_POINTER;
}



void* DLLEXPORT SpectrumAnalyzer_SetScaleDev( ViSession viInstrumentHandle , double lfScaleDev )  
{
	STD_ERROR               StdError                    =   {0};        

	double					lfReferenceLevel			=	0.0;
		
	CHK_ERR( hp8563e_queryAmpl(  viInstrumentHandle , 1 , &lfReferenceLevel ));

	CHK_ERR( hp8563e_confAmpltde1 ( viInstrumentHandle , lfScaleDev , lfReferenceLevel )); 
	
Error: 
		
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_Attenuator( ViSession viInstrumentHandle , double lfAttenuator )  
{
	STD_ERROR               StdError                    =   {0};        

	if ( lfAttenuator < 0 )
	{
		CHK_ERR( hp8563e_configCoupl ( viInstrumentHandle , 4 , 0 , 0.0 ));
	}
	else
	{
		CHK_ERR( hp8563e_configCoupl ( viInstrumentHandle , 4 , 1 ,  lfAttenuator )); 
	}
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetReferenceLevel( ViSession viInstrumentHandle , double lfReferenceLevel )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hp8563e_confAmpltde1 ( viInstrumentHandle , 3 , lfReferenceLevel )); 
	
Error:
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetReferenceLevel( ViSession viInstrumentHandle , double *vlfReferenceLevel )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hp8563e_queryAmpl(  viInstrumentHandle , 1 , vlfReferenceLevel ));

Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetMarkerType( ViSession viInstrumentHandle , int bDelta )  
{
	STD_ERROR               StdError                    =   {0};        

	if ( bDelta )
	{
		CHK_ERR( hp8563e_placeMarker ( viInstrumentHandle , 2 , 0.0 ));
	}
	else
	{
		CHK_ERR( hp8563e_placeMarker ( viInstrumentHandle , 0 , 0.0 ));
	}
	
Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetMarkerFrequency( ViSession viInstrumentHandle , double lfFrequency )  
{
	STD_ERROR               StdError                    =   {0};        
	
	CHK_ERR( hp8563e_placeMarker ( viInstrumentHandle , 3 , lfFrequency ));

Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SearchMarkerPeak( ViSession viInstrumentHandle  )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hp8563e_placeMarker ( viInstrumentHandle , 5 , 0.0 ));

Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SearchMarkerNextPeak( ViSession viInstrumentHandle  )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hp8563e_placeMarker ( viInstrumentHandle , 6 , 0.0 ));

Error: 
	
	RETURN_STDERR_POINTER;
}																		

void* DLLEXPORT SpectrumAnalyzer_GetMarkerFrequency( ViSession viInstrumentHandle , int iMarkerNr , double *vlfFrequency )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hp8563e_queryFreq( viInstrumentHandle , 1 , vlfFrequency )); 

Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetMarkerAmplitude( ViSession viInstrumentHandle , int iMarkerNr , double *vlfAmplitude )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hp8563e_queryAmpl ( viInstrumentHandle , 0 , vlfAmplitude ));

Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetMarkerDeltaFrequency( ViSession viInstrumentHandle , double *vlfFrequency )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hp8563e_queryFreq ( viInstrumentHandle , 2 , vlfFrequency ));

Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetMarkerDeltaAmplitude( ViSession viInstrumentHandle , double *vlfAmplitude )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hp8563e_queryAmpl ( viInstrumentHandle , 2 , vlfAmplitude ));

Error: 

	RETURN_STDERR_POINTER;
}



void* DLLEXPORT SpectrumAnalyzer_SetCenterFrequencyFromMarker( ViSession viInstrumentHandle )  
{
	STD_ERROR               StdError                    =   {0};        
	
	CHK_ERR( hp8563e_perfmTo (viInstrumentHandle, 0 ));
	
Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetReferenceLevelFrequencyFromMarker( ViSession viInstrumentHandle )  
{
	STD_ERROR               StdError                    =   {0};        
	
	CHK_ERR( hp8563e_perfmTo (viInstrumentHandle, 1 ));
	
Error: 

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_WaitForIntrumentDone( ViSession viInstrumentHandle , double lfTimeOut )  
{
	STD_ERROR               StdError                    =   {0};        
	
	CHK_ERR( hp8563e_waitUntilDone ( viInstrumentHandle , ( lfTimeOut * 1E3 )));
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_SetSweepMode( ViSession viInstrumentHandle , int bContinues )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hp8563e_confSweep ( viInstrumentHandle , VI_OFF, 50.0e-3, (!bContinues) , VI_OFF, 2.0e-3 )); 	
	
	CHK_ERR( hp8563e_waitUntilDone ( viInstrumentHandle , MAX_TIMEOUT ));   
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_GetSweep( ViSession viInstrumentHandle , double lfTimeOut )
{
	STD_ERROR               StdError                    =   {0};        

	double					sweepTime					=	0.0;
	
	CHK_ERR( hp8563e_querySwpTime ( viInstrumentHandle , &sweepTime ));
	
	if ( sweepTime > lfTimeOut )
		sweepTime = lfTimeOut;
	
	DelayWithEventProcessing( sweepTime );
	
	CHK_ERR( hp8563e_waitUntilDone ( viInstrumentHandle , MAX_TIMEOUT ));   
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SpectrumAnalyzer_AlignNow ( ViSession hInstrumentHandle , double lfTimeout )
{
	STD_ERROR               StdError                    =   {0};        

Error: 

	RETURN_STDERR_POINTER;
} 

void* DLLEXPORT SpectrumAnalyzer_InitiateMeasure( ViSession viInstrumentHandle )  
{
	STD_ERROR               StdError                    =   {0};        

	unsigned int			iCount						=	0;
	
	CHK_ERR( viWrite ( viInstrumentHandle , (unsigned char*)"TS\n" , 3 , &iCount )); 

	CHK_ERR( hp8563e_waitUntilDone ( viInstrumentHandle , MAX_TIMEOUT ));   
	
Error: 

	RETURN_STDERR_POINTER;
}


void* DLLEXPORT SpectrumAnalyzer_SetAverage( ViSession viInstrumentHandle , int iAverage )  
{
	STD_ERROR               StdError                    =   {0};        
	
	RETURN_STDERR_POINTER;
}
  
void* DLLEXPORT SpectrumAnalyzer_RecallState( ViSession viInstrumentHandle ,char *szFileName )  
{
	STD_ERROR               StdError                    =   {0};        

	int						iStateNumber				=	0;

	if ( szFileName )
		iStateNumber = atoi( szFileName );
	
	CHK_ERR( hp8563e_saveRecall ( viInstrumentHandle , 1, 0, iStateNumber ));

	CHK_ERR( hp8563e_waitUntilDone ( viInstrumentHandle , MAX_TIMEOUT ));   
	
Error: 

	RETURN_STDERR_POINTER;
}  

void* DLLEXPORT SpectrumAnalyzer_SaveState( ViSession viInstrumentHandle ,char *szFileName )  
{
	STD_ERROR               StdError                    =   {0};        

	int						iStateNumber				=	0;

	if ( szFileName )
		iStateNumber = atoi( szFileName );
	
	CHK_ERR( hp8563e_saveRecall ( viInstrumentHandle , 0 , 0 , iStateNumber ));
	
	CHK_ERR( hp8563e_waitUntilDone ( viInstrumentHandle , MAX_TIMEOUT ));   
	
Error: 

	RETURN_STDERR_POINTER;
} 

void* DLLEXPORT SpectrumAnalyzer_Recall( ViSession viInstrumentHandle ,char *szFileName )  
{
	return SpectrumAnalyzer_RecallState( viInstrumentHandle ,szFileName );
} 

void* DLLEXPORT SpectrumAnalyzer_Read_EVM( ViSession viInstrumentHandle , double *plfEVM_Value )  
{
	STD_ERROR               StdError                  			=   {0};        

	RETURN_STDERR_POINTER;
}  


void* DLLEXPORT SpectrumAnalyzer_SetToMode_Specrum( ViSession viInstrumentHandle )  
{
	STD_ERROR               StdError                    =   {0};        

	RETURN_STDERR_POINTER;
} 

void* DLLEXPORT SpectrumAnalyzer_SetFrequency_Specrum( ViSession viInstrumentHandle , double lfFrequency )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hp8563e_configFreq ( viInstrumentHandle , 0, lfFrequency ));
	
Error: 

	RETURN_STDERR_POINTER;
} 

void* DLLEXPORT SpectrumAnalyzer_SetFrequency( ViSession viInstrumentHandle , double lfFrequency )  
{
	STD_ERROR               StdError                    =   {0};        

	CHK_ERR( hp8563e_configFreq ( viInstrumentHandle , 0, lfFrequency ));
	
Error: 

	RETURN_STDERR_POINTER;
} 

void* DLLEXPORT SpectrumAnalyzer_SetFrequency_EVM( ViSession viInstrumentHandle , double lffrequency )  
{
	STD_ERROR               StdError                    =   {0};        

	RETURN_STDERR_POINTER;
} 

void* DLLEXPORT SpectrumAnalyzer_RestartMeasure( ViSession viInstrumentHandle )  
{
	STD_ERROR               StdError                    =   {0};        

	unsigned int			iCount						=	0;
	
	CHK_ERR( viWrite ( viInstrumentHandle , (unsigned char*)"TS\n" , 3 , &iCount )); 

	CHK_ERR( hp8563e_waitUntilDone ( viInstrumentHandle , MAX_TIMEOUT ));   
	
Error: 

	RETURN_STDERR_POINTER;
} 

void* DLLEXPORT SpectrumAnalyzer_Read_SymbolStream( ViSession viInstrumentHandle , int iNumberOfSymbols , char **pSymbolStream , int *piNumberOfSymbolsRead )  
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

	SpectrumAnalyzer_Init ( 0 , "GPIB::5" , &hInstrumentHandle , &bConnectStatus , &ChannelsQuantity );
	
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


