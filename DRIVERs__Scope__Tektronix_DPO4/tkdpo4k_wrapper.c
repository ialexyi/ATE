//==============================================================================
// Include files

#include <windows.h>
#include <userint.h>
#include <ansi_c.h>
#include <utility.h>
#include "tkdpo4k.h"
#include "GlobalDefines.h"
#include <analysis.h>
#include "toolbox.h"
#include "tkdpo4k_wrapper.h" 

//==============================================================================
// Constants

#define							    WAVEFORM_SIZE							100 * 1000 

//==============================================================================
// Types

//==============================================================================
// Static global variables

static	char                    szResourceName[LOW_STRING]			   =	{0};

static	char					SourceChannelName[CH_NAME_LEN]	       =	{0};  

static	char					MeasureChannelName[CH_NAME_LEN]	       =	{0}; 

static	int						SourceSlope							   =   0;

static	int						MeasureSlope						   =   0; 



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

void* DLLEXPORT Oscilloscope_MemoryState_Recall( int hInstrumentHandle , char *pszFileName ); 


/***** ***** ***** ***** ***** ***** Get Error Text Message ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_GetErrorTextMessage ( ViSession viInstrumentHandle , int iError , char *pErrorMessage )
{   
	STD_ERROR               StdError					=	{0};
	
	STD_ERR( tkdpo4k_error_message	( viInstrumentHandle , iError , pErrorMessage ));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** Equipment Info ***** ***** ***** ***** ***** ***** *****/   

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
		IF (( strstr( szIdentificationText , "TEKTRONIX" ) == NULL ) , "Is not supported" );
	
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

/***** ***** ***** ***** ***** ***** Equipment Get Low Level Handle ***** ***** ***** ***** ***** ***** *****/ 

void*	DLLEXPORT		Equipment_GetLowLevelHandle( int hHandle , int *phLowLevelHandle )
{
	STD_ERROR						StdError									=	{0};
	
	if ( phLowLevelHandle )
		*phLowLevelHandle = hHandle;
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** Equipment IsSupported ***** ***** ***** ***** ***** ***** *****/   

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
									vszSupportdedModels[][12]					=	{{"DPO4104"},{"DPO4054"},{"DPO4034"},{"DPO4032"},{"MSO4104"},{"MSO4054"},{"MSO4034"},{"MSO4032"},{"MSO3012"},{"MSO3014"},{"MSO3032"},{"MSO3034"},{"MSO3052"},{"MSO3054 DPO3012"},{"DPO3014"},{"DPO3032"},{"DPO3034"},{"DPO3052"},{"DPO3054"},{"DPO4104B"},{"DPO4054B"},{"DPO4034B"},{"MSO4104B"},{"MSO4054B"},{"MSO4034B"}};
									
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
		if ( strstr( szIdentificationText , "TEKTRONIX" ) == NULL ) 
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

/***** ***** ***** ***** ***** ***** Config Copy STD CallBackSet  ***** ***** ***** ***** ***** ***** *****/   

void*	DLLEXPORT		Config_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet )
{
	STD_ERROR						StdError									=	{0};
	
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** Init ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_Init ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *viInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChanelsQuantity )
{			
	STD_ERROR               StdError					=	{0};
	
	int						iChanelsQuantity			=	0;
	
	char					szErrorTextMessage[512]		=	{0};
	
	ViSession				handle						=	0;
	
	strcpy(szResourceName, szRsrcAddress); 
	
	if ( hParentInstrumentHandle == 0 )
	{
		StdError.error = tkdpo4k_init ( szRsrcAddress , VI_TRUE , VI_TRUE , &handle );  
	}
	else
		handle = hParentInstrumentHandle;
		
	if ( viInstrumentHandle  ) 
			*viInstrumentHandle = handle;
	
	if ( IS_NOT_OK )
	{
		MessagePopup("Error!" , "Oscilloscope Initializing Error.");
		
		RETURN_STDERR_POINTER;
	}

	STD_ERR ( tkdpo4k_GetAttributeViInt32 ( handle , VI_NULL , TKDPO4K_ATTR_CHANNEL_COUNT , &iChanelsQuantity )); //TKDPO4K_ATTR_NUM_CHANNELS
		
	if ( InstrumentChanelsQuantity )
		*InstrumentChanelsQuantity = iChanelsQuantity; 
	
	if (iChanelsQuantity < 1)
	{
		MessagePopup("Error!" , "Oscilloscope Initializing Error.");	
		
		tkdpo4k_close ( handle );
	}
	else
		if ( InstrumentConnectStatus )
		{
			
			*InstrumentConnectStatus = 1;
		}
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** ***** ***** Close ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_Close ( int *phInstrumentHandle )
{
	STD_ERROR               StdError					=	{0};
	
	ViSession				handle						=	0;

	IF ( (phInstrumentHandle == NULL) , "Pointer passed NULL." );
	
	handle = *phInstrumentHandle;
	
	IF ( (handle == 0) , "Pointer passed 0." );
	
	STD_ERR ( tkdpo4k_close ( handle ));
	
	*phInstrumentHandle = 0;
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** ***** ***** Reset ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_Reset ( int hInstrumentHandle )
{
	STD_ERROR               StdError					=	{0};
	
	STD_ERR (tkdpo4k_reset ( hInstrumentHandle ));
	
	Oscilloscope_MemoryState_Recall( hInstrumentHandle , "1" ) ;// For debug only   
	
Error:	
	
	RETURN_STDERR_POINTER;		
}

/***** ***** ***** ***** ***** ***** ***** ***** Auto Setup ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_AutoSetup ( int hInstrumentHandle )
{
	STD_ERROR               StdError					=	{0};
	
	STD_ERR (tkdpo4k_AutoSetup ( hInstrumentHandle ));
	
Error:	
	
	RETURN_STDERR_POINTER;		
}

/***************************************** Measurements Initiate ***********************************************/

void* DLLEXPORT Oscilloscope_InitTrigger ( int hInstrumentHandle )
{
	STD_ERROR               StdError					=	{0};
	
	STDERR ( tkdpo4k_InitiateAcquisition ( hInstrumentHandle ));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** ***** ***** Channel On/Off ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_ChannelOnOff ( int hInstrumentHandle , int iChannel , int OnOff )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( tkdpo4k_SetAttributeViBoolean (hInstrumentHandle, szChannelName, TKDPO4K_ATTR_CHANNEL_ENABLED, (ViBoolean)OnOff));
	
Error:	
	
	RETURN_STDERR_POINTER;		
}


/***** ***** ***** ***** ***** ***** ***** ***** Trigger Config ***** ***** ***** ***** ***** ***** ***** *****/   

void* DLLEXPORT Oscilloscope_TriggerConfig ( int hInstrumentHandle , int iChannel , double lfTrigerLevel , int bPositiveEdge , double lfHoldTime , int iModeAuto )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	
	STD_ERR ( tkdpo4k_ConfigureEdgeTriggerSource (hInstrumentHandle, szChannelName, lfTrigerLevel, bPositiveEdge ));
	
	//STD_ERR ( tkdpo4k_ConfigureTriggerLevel (hInstrumentHandle, szChannelName, lfTrigerLevel )); 
	
	STD_ERR ( tkdpo4k_ConfigureTrigger (hInstrumentHandle, TKDPO4K_VAL_EDGE_TRIGGER, lfHoldTime ));
	
	if( iModeAuto == 1)
	{
		STD_ERR ( tkdpo4k_ConfigureTriggerModifier (hInstrumentHandle, TKDPO4K_VAL_AUTO ));
	}
	else
	{
		STD_ERR ( tkdpo4k_ConfigureTriggerModifier (hInstrumentHandle, TKDPO4K_VAL_NO_TRIGGER_MOD )); 
	}
	
	
	
Error:	
	
	RETURN_STDERR_POINTER;
} 


/***** ***** ***** ***** ***** ***** ***** ***** Get Waveform ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT Oscilloscope_GetWaveForm ( int hInstrumentHandle , int iChannel, int iSize, double lfTimeOut, double *dpMeasureArray, int *ipCount, double *dpX_init, double *dpX_inc )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};  
	
	int						iAcquisitionStatus          =   0;
	
	STD_ERR ( tkdpo4k_ConfigureAcquisitionType (hInstrumentHandle, TKDPO4K_VAL_HI_RES )); 
	
	STD_ERR ( tkdpo4k_ConfigureWfmBufferSize (hInstrumentHandle, iSize ));
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName ));
	
	
	STD_ERR ( tkdpo4k_AcquisitionStatus (hInstrumentHandle, &iAcquisitionStatus )); 
	
	if( iAcquisitionStatus == 0)  // Continues mode
	{
		STD_ERR ( tkdpo4k_InitiateContinuousAcquisition (hInstrumentHandle, iSize ));
		
		DelayWithEventProcessing(lfTimeOut);   
		
		STD_ERR ( tkdpo4k_FetchContinuousWaveform (hInstrumentHandle, szChannelName, dpMeasureArray, ipCount, dpX_init, dpX_inc ));
	}
	else  // Single mode
	{
		STD_ERR ( tkdpo4k_InitiateAcquisition (hInstrumentHandle)); 
	
		DelayWithEventProcessing(lfTimeOut); 
	
		STD_ERR ( tkdpo4k_FetchWaveform (hInstrumentHandle, szChannelName, iSize, dpMeasureArray, ipCount, dpX_init, dpX_inc ));
	
	}
	
	
	
	
Error:	
	
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** Channel Vertical ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_ChannelVertical ( int hInstrumentHandle , int iChannel, double dScale , double dOffset , int iUnits , int iCouping , double lfProbeAttenuation , double lfImpedance )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	 
	STD_ERR ( tkdpo4k_ConfigureChannel (hInstrumentHandle, szChannelName, dScale, dOffset, TKDPO4K_VAL_DC, lfProbeAttenuation, VI_TRUE));
	
//	STD_ERR ( tkdpo4k_SetAttributeViReal64 (hInstrumentHandle, szChannelName, TKDPO4K_ATTR_VERTICAL_RANGE, dScale));
//	STD_ERR ( tkdpo4k_SetAttributeViReal64 (hInstrumentHandle, szChannelName, TKDPO4K_ATTR_VERTICAL_OFFSET, dOffset));
//	STD_ERR ( tkdpo4k_SetAttributeViInt32  (hInstrumentHandle, szChannelName, TKDPO4K_ATTR_VERTICAL_COUPLING, iCouping));
//	STD_ERR ( tkdpo4k_SetAttributeViReal64 (hInstrumentHandle, szChannelName, TKDPO4K_ATTR_PROBE_ATTENUATION, lfProbeAttenuation));
//	STD_ERR ( tkdpo4k_SetAttributeViReal64 (hInstrumentHandle, szChannelName, TKDPO4K_ATTR_INPUT_IMPEDANCE, lfImpedance));
//		
Error:	
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** ***** ***** Horizontal ***** ***** ***** ***** ***** ***** ***** *****/  


void* DLLEXPORT Oscilloscope_Horizontal ( int hInstrumentHandle , double dScale , double dOffset )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0}; 
	
	STD_ERR ( tkdpo4k_ConfigureAcquisitionRecord (hInstrumentHandle, dScale, WAVEFORM_SIZE, dOffset));
	
	Error:	
	
	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** ***** ***** Measure Rise Time ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_MeasureRiseTime ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	
	STD_ERR ( tkdpo4k_InitiateAcquisition ( hInstrumentHandle ));
	
	DelayWithEventProcessing(lfTimeOut);  
	
	STD_ERR ( tkdpo4k_FetchWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_RISE_TIME, dpValue ));
	
	//STD_ERR ( tkdpo4k_ReadWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_RISE_TIME, (int)(lfTimeOut*1E3) , dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;		
}

/***** ***** ***** ***** ***** ***** ***** ***** Measure Fall Time ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_MeasureFallTime ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	
	STD_ERR ( tkdpo4k_InitiateAcquisition ( hInstrumentHandle ));
	
	DelayWithEventProcessing(lfTimeOut); 
	
	STD_ERR ( tkdpo4k_FetchWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_FALL_TIME, dpValue ));
	
	// STD_ERR ( tkdpo4k_ReadWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_FALL_TIME, (int)(lfTimeOut*1E3) , dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** ***** ***** Measure Frequency ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_MeasureFrequency ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( tkdpo4k_ReadWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_FREQUENCY, (int)(lfTimeOut*1E3) , dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** ***** ***** Measure Period ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_MeasurePeriod ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( tkdpo4k_ReadWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_PERIOD, (int)(lfTimeOut*1E3) , dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;		
}

/***** ***** ***** ***** ***** ***** ***** ***** Measure Voltage RMS ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_MeasureVoltageRMS ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( tkdpo4k_ReadWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_VOLTAGE_RMS, (int)(lfTimeOut*1E3) , dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;		
}

/***** ***** ***** ***** ***** ***** ***** ***** Measure Voltage Pk2Pk ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_MeasureVoltagePk2Pk ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( tkdpo4k_ReadWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_VOLTAGE_PEAK_TO_PEAK, (int)(lfTimeOut*1E3) , dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;		
}

/***** ***** ***** ***** ***** ***** ***** ***** Measure Voltage Max ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_MeasureVoltageMax ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( tkdpo4k_ReadWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_VOLTAGE_MAX, (int)(lfTimeOut*1E3) , dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;		
}

/***** ***** ***** ***** ***** ***** ***** ***** Measure Voltage Min ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_MeasureVoltageMin ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( tkdpo4k_ReadWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_VOLTAGE_MIN, (int)(lfTimeOut*1E3) , dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;		
}

/***** ***** ***** ***** ***** ***** ***** ***** Measure Voltage High ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_MeasureVoltageHigh ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( tkdpo4k_ReadWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_VOLTAGE_HIGH, (int)(lfTimeOut*1E3) , dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;		
}

/***** ***** ***** ***** ***** ***** ***** ***** Measure Voltage Low ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_MeasureVoltageLow ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( tkdpo4k_ReadWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_VOLTAGE_LOW, (int)(lfTimeOut*1E3) , dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;		
}

/***** ***** ***** ***** ***** ***** ***** ***** Measure Voltage Average ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_MeasureVoltageAverage ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( tkdpo4k_ReadWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_VOLTAGE_AVERAGE, (int)(lfTimeOut*1E3) , dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** ***** ***** Measure Width Negative ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_MeasureWidthNegative ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( tkdpo4k_ReadWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_WIDTH_NEG, (int)(lfTimeOut*1E3) , dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** ***** ***** Measure Width Positive ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_MeasureWidthPositive ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( tkdpo4k_InitiateAcquisition ( hInstrumentHandle ));
	
	DelayWithEventProcessing(lfTimeOut); 
	
	STD_ERR ( tkdpo4k_FetchWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_WIDTH_POS, dpValue ));
	
	//STD_ERR ( tkdpo4k_ReadWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_WIDTH_POS, (int)(lfTimeOut*1E3) , dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** ***** ***** Measure Duty Cycle Negative ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_MeasureDutyCycleNegative ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( tkdpo4k_ReadWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_DUTY_CYCLE_NEG, (int)(lfTimeOut*1E3) , dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** ***** ***** Measure Duty Cycle Positive ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_MeasureDutyCyclePositive ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( tkdpo4k_ReadWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_DUTY_CYCLE_POS, (int)(lfTimeOut*1E3) , dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;		
}

/***** ***** ***** ***** ***** ***** ***** ***** Measure Amplitude ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_MeasureAmplitude ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( tkdpo4k_ReadWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_AMPLITUDE, (int)(lfTimeOut*1E3) , dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;		
}

/***** ***** ***** ***** ***** ***** ***** ***** Measure Voltage Cycle RMS ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_MeasureVoltageCycleRMS ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( tkdpo4k_ReadWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_VOLTAGE_CYCLE_RMS, (int)(lfTimeOut*1E3) , dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;		
}

/***** ***** ***** ***** ***** ***** ***** ***** Measure Voltage Cycle Average ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_MeasureVoltageCycleAverage ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( tkdpo4k_ReadWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_VOLTAGE_CYCLE_AVERAGE, (int)(lfTimeOut*1E3) , dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;		
}

/***** ***** ***** ***** ***** ***** ***** ***** Measure Positive Overshoot ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_MeasurePositiveOvershoot ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( tkdpo4k_ReadWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_POSITIVE_OVERSHOOT, (int)(lfTimeOut*1E3) , dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;		
}

/***** ***** ***** ***** ***** ***** ***** ***** Measure Negative Overshoot ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_MeasureNegativeOvershoot ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( tkdpo4k_ReadWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_NEGATIVE_OVERSHOOT, (int)(lfTimeOut*1E3) , dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;		
}

/***** ***** ***** ***** ***** ***** ***** ***** Measure Timing Burst Width ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT Oscilloscope_MeasureTimingBurstWidth ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( tkdpo4k_ReadWaveformMeasurement (hInstrumentHandle, szChannelName, TKDPO4K_VAL_TIMING_BURST_WIDTH, (int)(lfTimeOut*1E3) , dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;		
}

/***** ***** ***** ***** ***** ***** ***** ***** Config Measure Delay  ***** ***** ***** ***** ***** ***** ***** *****/  

void* DLLEXPORT Oscilloscope_ConfigMeasureDelay ( int hInstrumentHandle , int iSourceChannel , int iSourceSlope , int iSourceSpare, int iMeasureChannel , int iMeasureSlope , int iMeasureSpare )
{
	STD_ERROR               StdError					=	{0}; 
	
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iSourceChannel, CH_NAME_LEN, SourceChannelName));
	
	STD_ERR ( tkdpo4k_GetChannelName (hInstrumentHandle, iMeasureChannel, CH_NAME_LEN, MeasureChannelName));
	
	SourceSlope	 =  iSourceSlope;
	
	MeasureSlope = iMeasureSlope;
	

	
	Error:	
	
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** Measure Delay  ***** ***** ***** ***** ***** ***** ***** *****/    

void* DLLEXPORT Oscilloscope_MeasureDelay ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0}; 
	
	STD_ERR ( tkdpo4k_InitiateAcquisition ( hInstrumentHandle ));
	
	DelayWithEventProcessing(lfTimeOut); 
	
	STD_ERR ( tkdpo4k_FetchDelayMeasurement (hInstrumentHandle, SourceChannelName, MeasureChannelName, SourceSlope, MeasureSlope, TKDPO4K_VAL_DEL_DIR_FORWARD, dpValue )); 
	
	Error:	
	
	RETURN_STDERR_POINTER;	
}


/*********************************************** Memory State Recall ***********************************************/

void* DLLEXPORT Oscilloscope_MemoryState_Recall( int hInstrumentHandle , char *pszFileName )
{
	STD_ERROR               StdError					=	{0};
	
	char					szFileName[512]				=	{0};
	
	int						iStatus						=	0,
							iInterType  				=	0,
						    iInterName	  				=	0,
						    iCount						=	0; 
	
	int                     hConnectionHandle			=	0,
	  						hHandle						=	0;  
	
	char					*pStateName					=	NULL;  

	
	
	IF (((pszFileName==NULL)&&(strlen(pszFileName)==0)),"file name is empty");
	
	
	iStatus = viOpenDefaultRM (&hConnectionHandle);
	iStatus = viParseRsrc (hConnectionHandle, szResourceName, &iInterType, &iInterName);
	iStatus = viOpen (hConnectionHandle, szResourceName, VI_NULL, VI_NULL, &hHandle);
	
	pStateName = SkipWhiteSpace (pszFileName); 
		
	STD_ERR (viPrintf( hHandle , "*CLS\n" ));
	
	sprintf( szFileName , "RECALL:SETUP %s\n" , pStateName ); 
	
	STD_ERR (viPrintf( hHandle , szFileName )); 
	
	viClose(hHandle);
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

//==================================================================================================================================

//                                      Main Entry Point Using For Teating

//==================================================================================================================================
int main (int argc, char *argv[])
{
	int					status									=	0; 
	
	int     			hConnectionHandle						=	0,
	        			hInstrumentHandle						=	0;
		
	int					bConnectStatus							=	0,
						iChannelsQuantity						=	0,
						iInterType  							=	0,
						iInterName	  							=	0,
						iChannel								=	0,
						iCount									=	0; 
	
	char				szCommandName[LOW_STRING]			    =	{0},
						szChannelName[LOW_STRING]			    =	{0};
	
	double              *dpMeasureArray					        =   NULL;
	
	
	int					iSize									=   10000;
	int    				ipCount									=	0;
	double 				dpX_init								=	0; 
	double 				dpX_inc							    	=	0; 
	int					iAcquisitionStatus						=	0;
	
	double				dMinValue	                			=   0.0;
	double				dMaxValue				                =   0.0;
	int					iMinIndex                  				=   0;
	int					iMaxIndex               				=   0;
	
	int					iPulseCounterResult       				=   0;
	double				dThreshold								=   0;
	double              *dpPeakLocations				        =   NULL;   
	double              *dpPeakAmplitudes				        =   NULL;
	double              *dpPeakDerivatives					    =   NULL;
	
	double				dPwidthCh1  								=   0;
	double				dPwidthCh2  								=   0;  
	double				dPwidthCh3  								=   0;  
	double				dPwidthCh4  								=   0;  

	
	
	InitCVIRTE (0, argv, 0 );
	
	
	strcpy(szResourceName, "TCPIP0::10.0.0.54::inst0::INSTR");
	
	status = Oscilloscope_Init( 0 , szResourceName , &hInstrumentHandle , &bConnectStatus , &iChannelsQuantity );
	
	Oscilloscope_MemoryState_Recall( hInstrumentHandle , "1" ) ;
	
    //Oscilloscope_Horizontal( hInstrumentHandle, 1E-3, 0 );
	
	//iChannel = 3; 
	
	//Oscilloscope_TriggerConfig ( hInstrumentHandle , iChannel , 0.5 , TKDPO4K_VAL_POSITIVE , 20.0E-9 , TKDPO4K_VAL_NO_TRIGGER_MOD);//TKDPO4K_VAL_AUTO ) ; 
	
	
	//status = tkdpo4k_ConfigureAcquisitionType (hInstrumentHandle, TKDPO4K_VAL_HI_RES); 
	
	//status = tkdpo4k_ConfigureWfmBufferSize (hInstrumentHandle, 10000);
	
	//status = tkdpo4k_InitiateAcquisition (hInstrumentHandle);
	//status = tkdpo4k_AcquisitionStatus (hInstrumentHandle, &iAcquisitionStatus); 
	//status = tkdpo4k_FetchWaveform (hInstrumentHandle, TKDPO4K_VAL_CH_3, iSize, dpMeasureArray, &ipCount, &dpX_init, &dpX_inc);
	
	//dpMeasureArray = calloc(iSize, sizeof(double));
  
	
	//status = tkdpo4k_ConfigureAcquisitionRecord (hInstrumentHandle, 10.0e-3, 10000, 0.0);
	//status = Oscilloscope_GetWaveForm ( hInstrumentHandle , 3, iSize, 3.0, dpMeasureArray, &ipCount, &dpX_init, &dpX_inc);
	
	//status = MaxMin1D (dpMeasureArray, ipCount, &dMaxValue, &iMaxIndex, &dMinValue, &iMinIndex);
	//dThreshold = dMaxValue * 0.75; 
	//status = PeakDetector (dpMeasureArray, ipCount, dThreshold, 3, DETECT_PEAKS, ANALYSIS_TRUE, ANALYSIS_TRUE, &iPulseCounterResult, &dpPeakLocations, &dpPeakAmplitudes, &dpPeakDerivatives);
	
	//status = tkdpo4k_InitiateAcquisition (hInstrumentHandle);
	//status = tkdpo4k_AcquisitionStatus (hInstrumentHandle, &iAcquisitionStatus);  
	//status = tkdpo4k_FetchWaveformMeasurement (hInstrumentHandle, TKDPO4K_VAL_CH_2, TKDPO4K_VAL_WIDTH_POS, &dPwidthCh1); 
	
	status = tkdpo4k_ConfigureAcquisitionRecord (hInstrumentHandle, 6.0E-6, 10000, -1.0E-6);
    status = tkdpo4k_ConfigureAcquisitionRecord (hInstrumentHandle, 4.0E-6, 10000, -1.0E-6); 
	status = tkdpo4k_ConfigureAcquisitionRecord (hInstrumentHandle, 60.0E-6, 10000, -15.0E-6); 
	status = tkdpo4k_ConfigureAcquisitionRecord (hInstrumentHandle, 30.0E-6, 10000, -3.0E-6);
	
	status =Oscilloscope_MeasureWidthPositive ( hInstrumentHandle , 4 , 0.5, &dPwidthCh4 ); 
	status = Oscilloscope_MeasureRiseTime ( hInstrumentHandle , 4 , 0.5 , &dPwidthCh4 );
	status = Oscilloscope_MeasureFallTime ( hInstrumentHandle , 4 , 0.5, &dPwidthCh4 );   
	
	status = tkdpo4k_FetchDelayMeasurement (hInstrumentHandle, TKDPO4K_VAL_CH_1, TKDPO4K_VAL_CH_4, TKDPO4K_VAL_DEL_EDGE_RISE, TKDPO4K_VAL_DEL_EDGE_RISE, TKDPO4K_VAL_DEL_DIR_BACKWARD, &dPwidthCh4);  
	
	status = Oscilloscope_Close(&hInstrumentHandle);	
	
	return 0;
	
}
