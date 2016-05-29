
//==============================================================================
// Include files

#include <windows.h>
#include <userint.h>
#include <ansi_c.h>
#include <utility.h>
#include "AgInfiniiVision.h"
#include "GlobalDefines.h"
#include "toolbox.h"
#include <Ivi.h>
#include <visa.h>
#include "Agilent_DSO_X_Wrapper.h" 

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

/*********************************** Get Error Text Message *****************************************/

void* DLLEXPORT Oscilloscope_GetErrorTextMessage ( ViSession viInstrumentHandle , int iError , char *pErrorMessage )
{   
	STD_ERROR               StdError					=	{0};
	
	STDERR ( AgInfiniiVision_error_message	( viInstrumentHandle , iError , pErrorMessage ));
	
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
		IF (( strstr( szIdentificationText , "Agilent Technologies" ) == NULL ) , "Is not supported" );
	
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
									vszSupportdedModels[][12]					=	{	{"DSOX2002A"},{"DSOX2004A"},{"DSOX2012A"},{"DSOX2014A"},{"DSOX2022A"},{"DSOX2024A"},{"MSOX2002A"},{"MSOX2004A"},{"MSOX2012A"},{"MSOX2014A"},{"MSOX2022A"},
																						{"MSOX2024A"},{"DSOX3012A"},{"DSOX3014A"},{"DSOX3024A"},{"DSOX3032A"},{"DSOX3034A"},{"DSOX3052A"},{"DSOX3054A"},{"MSOX3012A"},{"MSOX3014A"},
																						{"MSOX3024A"},{"MSOX3032A"},{"MSOX3034A"},{"MSOX3052A"},{"MSOX3054A"},{"MSOX3102A"},{"MSOX3104A"},{"DSOX3102A"},{"DSOX3104A"},{"MSOX4022A"},{"MSOX4032A"},
																						{"MSOX4052A"},{"MSOX4024A"},{"MSOX4034A"},{"MSOX4054A"},{"MSOX4104A"},{"MSOX4154A"},{"DSOX4022A"},{"DSOX4032A"},{"DSOX4052A"},{"DSOX4024A"},
																						{"DSOX4034A"},{"DSOX4054A"},{"DSOX4104A"},{"DSOX4154A"}};
									
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
		if ( strstr( szIdentificationText , "Agilent Technologies" ) == NULL ) 
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

/*********************************************** Init ***********************************************/

void* DLLEXPORT Oscilloscope_Init ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *viInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChanelsQuantity )
{			
	STD_ERROR               StdError					=	{0};
	
	
	int			iChanelsQuantity			=	0;
	
	char		szErrorTextMessage[512]		=	{0};
	
	ViSession	handle						=	0;
	
	if ( hParentInstrumentHandle == 0 )
	{
		StdError.error = AgInfiniiVision_init (szRsrcAddress, VI_TRUE, VI_FALSE, &handle);
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

	STDERR ( AgInfiniiVision_GetAttributeViInt32 ( handle , VI_NULL , AGINFINIIVISION_ATTR_CHANNEL_COUNT , &iChanelsQuantity )); //
		
	if ( InstrumentChanelsQuantity )
		*InstrumentChanelsQuantity = iChanelsQuantity; 
	
	if (iChanelsQuantity < 1)
	{
		MessagePopup("Error!" , "Oscilloscope Initializing Error.");	
		
		AgInfiniiVision_close ( handle );
	}
	else
		if ( InstrumentConnectStatus )
			*InstrumentConnectStatus = 1;
	
Error:	
	
	RETURN_STDERR_POINTER;
}											 

/*********************************************** Close ***********************************************/

void* DLLEXPORT Oscilloscope_Close ( int *phInstrumentHandle )
{
	STD_ERROR               StdError					=	{0};
	  
	
	ViSession	handle						=	0;

	IF ( (phInstrumentHandle == NULL) , "Pointer passed NULL.");
	
	handle = *phInstrumentHandle;
	
	IF ( (handle == 0) , "Pointer passed 0.");
	
	STDERR ( AgInfiniiVision_close ( handle ));
	
	*phInstrumentHandle = 0;
	
Error:	
	
	RETURN_STDERR_POINTER;
}

/*********************************************** Reset ***********************************************/

void* DLLEXPORT Oscilloscope_Reset ( int hInstrumentHandle )
{
	STD_ERROR               StdError					=	{0};
	
	
	STDERR (AgInfiniiVision_reset ( hInstrumentHandle ));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/*********************************************** Auto Setup ***********************************************/

void* DLLEXPORT Oscilloscope_AutoSetup ( int hInstrumentHandle )
{
	STD_ERROR               StdError					=	{0};
	
	
	STDERR (AgInfiniiVision_AutoSetup ( hInstrumentHandle ));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/***************************************** Measurements Initiate ***********************************************/

void* DLLEXPORT Oscilloscope_InitTrigger ( int hInstrumentHandle )
{
	STD_ERROR               StdError					=	{0};
	
	
	STDERR (AgInfiniiVision_MeasurementsInitiate ( hInstrumentHandle ));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/****************************************** Wait For Event *********************************************************/
void* DLLEXPORT Oscilloscope_WaitForEvent ( int Handle , double lfTimeOut )  
{
	STD_ERROR               StdError					=	{0};
	
	int						iStatus						=	0;
	
	double					lfStartTime					=	0.0,
							lfCurrentTime				=	0.0;
	
	GetCurrentDateTime(&lfStartTime);
	
	do
	{
		STDERR (AgInfiniiVision_MeasurementsAcquisitionStatus ( Handle, &iStatus ));  
			
		if (iStatus) 
			break;
			
		GetCurrentDateTime(&lfCurrentTime);
			
	} while((lfCurrentTime-lfStartTime)<lfTimeOut);
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/****************************************** Horizontal *********************************************************/
void* DLLEXPORT Oscilloscope_Horizontal( int Handle , double dScale , double dOffset )
{
	STD_ERROR               StdError					=	{0};
	
	
	STDERR ( AgInfiniiVision_SetAttributeViReal64 (Handle, NULL , AGINFINIIVISION_ATTR_TIMEBASE_HORIZONTAL_SCALE , dScale)); 
	
	DelayWithEventProcessing(0.2);  
	
	STDERR ( AgInfiniiVision_SetAttributeViReal64 (Handle, NULL , AGINFINIIVISION_ATTR_ACQUISITION_START_TIME , dOffset - (dScale * 5.0)));   

Error:	
	
	RETURN_STDERR_POINTER;	
}	


/****************************************** Horizontal Offset *********************************************************/
void* DLLEXPORT Oscilloscope_HorizontalOffset ( int Handle , double lfOffset )  
{
	STD_ERROR               StdError					=	{0};
	
	double					lfOffsetTime				=	0.0;

	STDERR ( AgInfiniiVision_GetAttributeViReal64 (Handle, NULL , AGINFINIIVISION_ATTR_TIMEBASE_HORIZONTAL_SCALE , &lfOffsetTime )); 
					
	DelayWithEventProcessing(0.2);
	
	STDERR ( AgInfiniiVision_SetAttributeViReal64 (Handle, NULL , AGINFINIIVISION_ATTR_ACQUISITION_START_TIME , lfOffset - (lfOffsetTime*5.0)));  
	
Error:	
	
	RETURN_STDERR_POINTER;	
}	

/*********************************************** Channel On/Off ***********************************************/

void* DLLEXPORT Oscilloscope_SweepContinuous ( int hInstrumentHandle , int iState )
{
	STD_ERROR               StdError					=	{0};
	
	STDERR ( AgInfiniiVision_ConfigureInitiateContinuous (hInstrumentHandle, iState ));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

	
/*********************************************** Channel On/Off ***********************************************/

void* DLLEXPORT Oscilloscope_ChannelOnOff ( int hInstrumentHandle , int iChannel , int OnOff )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_SetAttributeViBoolean (hInstrumentHandle, szChannelName, AGINFINIIVISION_ATTR_CHANNEL_ENABLED, (ViBoolean)OnOff));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/*********************************************** Get Wave Formf ***********************************************/

void* DLLEXPORT Oscilloscope_GetWaveForm ( int hInstrumentHandle , int iChannel, int iSize, double lfTimeOut, double *pvlfMeasureArray, int *piCount, double *plfX_init, double *plfX_inc )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_SystemWaitForOperationComplete ( hInstrumentHandle , lfTimeOut*1E3 ));
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_MeasurementFetchWaveform( hInstrumentHandle , szChannelName , iSize , pvlfMeasureArray , piCount , plfX_init , plfX_inc ));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

///*********************************************** Channel Vertical ***********************************************/
//
//void* DLLEXPORT Oscilloscope_ChannelVertical ( int hInstrumentHandle , int iChannel, double dScale , double dOffset , int iUnits , int iCouping , double lfProbeAttenuation , double lfImpedance )
//{
//	STD_ERROR               StdError					=	{0};
//	
//	
//	char		szChannelName[CH_NAME_LEN]	=	{0};
//	
//	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
//	
//	STDERR ( AgInfiniiVision_SetAttributeViReal64 (hInstrumentHandle, szChannelName, AGINFINIIVISION_ATTR_VERTICAL_RANGE, dScale));
//	STDERR ( AgInfiniiVision_SetAttributeViReal64 (hInstrumentHandle, szChannelName, AGINFINIIVISION_ATTR_VERTICAL_OFFSET, dOffset));
//	STDERR ( AgInfiniiVision_SetAttributeViInt32  (hInstrumentHandle, szChannelName, AGINFINIIVISION_ATTR_VERTICAL_COUPLING, iCouping));
//	STDERR ( AgInfiniiVision_SetAttributeViReal64 (hInstrumentHandle, szChannelName, AGINFINIIVISION_ATTR_PROBE_ATTENUATION, lfProbeAttenuation));
//	STDERR ( AgInfiniiVision_SetAttributeViReal64 (hInstrumentHandle, szChannelName, AGINFINIIVISION_ATTR_INPUT_IMPEDANCE, lfImpedance));
//		
//Error:	
//	
//	CALLOC_COPY(pRet,1,sizeof(errEV),&ret);
//	
//	RETURN_STDERR_POINTER;
//}


/*********************************************** Measure Rise Time ***********************************************/

void* DLLEXPORT Oscilloscope_MeasureRiseTime ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_MeasurementFetchWaveformMeasurement (hInstrumentHandle, szChannelName, AGINFINIIVISION_VAL_RISE_TIME,  dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/*********************************************** Measure Fall Time ***********************************************/

void* DLLEXPORT Oscilloscope_MeasureFallTime ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_MeasurementFetchWaveformMeasurement (hInstrumentHandle, szChannelName, AGINFINIIVISION_VAL_FALL_TIME,  dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;
}

/*********************************************** Measure Frequency ***********************************************/

void* DLLEXPORT Oscilloscope_MeasureFrequency ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_MeasurementFetchWaveformMeasurement (hInstrumentHandle, szChannelName, AGINFINIIVISION_VAL_FREQUENCY ,  dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/*********************************************** Measure Period ***********************************************/

void* DLLEXPORT Oscilloscope_MeasurePeriod ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_MeasurementFetchWaveformMeasurement (hInstrumentHandle, szChannelName, AGINFINIIVISION_VAL_PERIOD,  dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/*********************************************** Measure Voltage RMS ***********************************************/

void* DLLEXPORT Oscilloscope_MeasureVoltageRMS ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_MeasurementFetchWaveformMeasurement (hInstrumentHandle, szChannelName, AGINFINIIVISION_VAL_VOLTAGE_RMS,  dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/*********************************************** Measure Voltage Pk2Pk ***********************************************/

void* DLLEXPORT Oscilloscope_MeasureVoltagePk2Pk ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_MeasurementFetchWaveformMeasurement (hInstrumentHandle, szChannelName, AGINFINIIVISION_VAL_VOLTAGE_PEAK_TO_PEAK,  dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/*********************************************** Measure Voltage Max ***********************************************/

void* DLLEXPORT Oscilloscope_MeasureVoltageMax ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_MeasurementFetchWaveformMeasurement (hInstrumentHandle, szChannelName, AGINFINIIVISION_VAL_VOLTAGE_MAX,  dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/*********************************************** Measure Voltage Min ***********************************************/

void* DLLEXPORT Oscilloscope_MeasureVoltageMin ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_MeasurementFetchWaveformMeasurement (hInstrumentHandle, szChannelName, AGINFINIIVISION_VAL_VOLTAGE_MIN,  dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/*********************************************** Measure Voltage High ***********************************************/

void* DLLEXPORT Oscilloscope_MeasureVoltageHigh ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_MeasurementFetchWaveformMeasurement (hInstrumentHandle, szChannelName, AGINFINIIVISION_VAL_VOLTAGE_HIGH,  dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/*********************************************** Measure Voltage Low ***********************************************/

void* DLLEXPORT Oscilloscope_MeasureVoltageLow ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_MeasurementFetchWaveformMeasurement (hInstrumentHandle, szChannelName, AGINFINIIVISION_VAL_VOLTAGE_LOW,  dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/*********************************************** Measure Voltage Average ***********************************************/

void* DLLEXPORT Oscilloscope_MeasureVoltageAverage ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_MeasurementFetchWaveformMeasurement (hInstrumentHandle, szChannelName, AGINFINIIVISION_VAL_VOLTAGE_AVERAGE,  dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/*********************************************** Measure Width Negative ***********************************************/

void* DLLEXPORT Oscilloscope_MeasureWidthNegative ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_MeasurementFetchWaveformMeasurement (hInstrumentHandle, szChannelName, AGINFINIIVISION_VAL_WIDTH_NEG,  dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;
}

/*********************************************** Measure Width Positive ***********************************************/

void* DLLEXPORT Oscilloscope_MeasureWidthPositive ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_MeasurementFetchWaveformMeasurement (hInstrumentHandle, szChannelName, AGINFINIIVISION_VAL_WIDTH_POS,  dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;
}

/*********************************************** Measure Duty Cycle Negative ***********************************************/

void* DLLEXPORT Oscilloscope_MeasureDutyCycleNegative ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_MeasurementFetchWaveformMeasurement (hInstrumentHandle, szChannelName, AGINFINIIVISION_VAL_DUTY_CYCLE_NEG,  dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;
}

/*********************************************** Measure Duty Cycle Positive ***********************************************/

void* DLLEXPORT Oscilloscope_MeasureDutyCyclePositive ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_MeasurementFetchWaveformMeasurement (hInstrumentHandle, szChannelName, AGINFINIIVISION_VAL_DUTY_CYCLE_POS,  dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/*********************************************** Measure Amplitude ***********************************************/

void* DLLEXPORT Oscilloscope_MeasureAmplitude ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_MeasurementFetchWaveformMeasurement (hInstrumentHandle, szChannelName, AGINFINIIVISION_VAL_AMPLITUDE,  dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/*********************************************** Measure Voltage Cycle RMS ***********************************************/

void* DLLEXPORT Oscilloscope_MeasureVoltageCycleRMS ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_MeasurementFetchWaveformMeasurement (hInstrumentHandle, szChannelName, AGINFINIIVISION_VAL_VOLTAGE_CYCLE_RMS,  dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/*********************************************** Measure Voltage Cycle Average ***********************************************/

void* DLLEXPORT Oscilloscope_MeasureVoltageCycleAverage ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_MeasurementFetchWaveformMeasurement (hInstrumentHandle, szChannelName, AGINFINIIVISION_VAL_VOLTAGE_CYCLE_AVERAGE,  dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/*********************************************** Measure Positive Overshoot ***********************************************/

void* DLLEXPORT Oscilloscope_MeasurePositiveOvershoot ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_MeasurementFetchWaveformMeasurement (hInstrumentHandle, szChannelName, AGINFINIIVISION_VAL_OVERSHOOT,  dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/*********************************************** Measure Negative Overshoot ***********************************************/

void* DLLEXPORT Oscilloscope_MeasureNegativeOvershoot ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_MeasurementFetchWaveformMeasurement (hInstrumentHandle, szChannelName, AGINFINIIVISION_VAL_PRESHOOT,  dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/*********************************************** Measure Timing Burst Width ***********************************************/

void* DLLEXPORT Oscilloscope_MeasureTimingBurstWidth ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )
{
	STD_ERROR               StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName));
	
	STDERR ( AgInfiniiVision_MeasurementFetchWaveformMeasurement (hInstrumentHandle, szChannelName, AGINFINIIVISION_VAL_MEASUREMENT_BURST_WIDTH,  dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/*********************************************** Measure Timing Burst Width ***********************************************/

void* DLLEXPORT Oscilloscope_MeasureDelay ( int hInstrumentHandle , int iChannel , double lfTimeOut, double *dpValue )  
{
	STD_ERROR               StdError					=	{0};
	
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iChannel, CH_NAME_LEN, szChannelName)); 

	STDERR ( AgInfiniiVision_MeasurementFetchWaveformMeasurement (hInstrumentHandle, szChannelName, AGINFINIIVISION_VAL_MEASUREMENT_DELAY,  dpValue));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}
	
/*********************************************** Measure Timing Burst Width ***********************************************/

void* DLLEXPORT Oscilloscope_ConfigMeasureDelay ( int hInstrumentHandle , int iSourceChannel , int iSourceSlope , int iSourceSpare, int iMeasureChannel , int iMeasureSlope , int iMeasureSpare )
{
	STD_ERROR               StdError					=	{0}; 
	
	char					szChannelName[CH_NAME_LEN]	=	{0}; 
	
	
	STDERR ( AgInfiniiVision_GetChannelName (hInstrumentHandle, iSourceChannel, CH_NAME_LEN, szChannelName)); 
	
	STDERR ( AgInfiniiVision_SetAttributeViString( hInstrumentHandle, NULL , AGINFINIIVISION_ATTR_MEASUREMENT_CONFIGURATION_SOURCE2 , szChannelName ));

	STDERR ( AgInfiniiVision_SetAttributeViInt32( hInstrumentHandle, NULL , AGINFINIIVISION_ATTR_MEASUREMENT_CONFIGURATION_DELAY_SLOPE1 , iSourceSlope ));
	STDERR ( AgInfiniiVision_SetAttributeViInt32( hInstrumentHandle, NULL , AGINFINIIVISION_ATTR_MEASUREMENT_CONFIGURATION_DELAY_SLOPE2 , iMeasureSlope )); 
	
	DelayWithEventProcessing(0.2);
	
	STDERR ( AgInfiniiVision_SetAttributeViInt32( hInstrumentHandle, NULL , AGINFINIIVISION_ATTR_MEASUREMENT_CONFIGURATION_DELAY_OCCURRENCE1 , iSourceSpare ));
	STDERR ( AgInfiniiVision_SetAttributeViInt32( hInstrumentHandle, NULL , AGINFINIIVISION_ATTR_MEASUREMENT_CONFIGURATION_DELAY_OCCURRENCE2 , iMeasureSpare )); 
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/*********************************************** Memory State Save ***********************************************/

void* DLLEXPORT Oscilloscope_MemoryState_Save ( int hInstrumentHandle , char *pszFileName )
{
	STD_ERROR               StdError					=	{0};
	
	IF (((pszFileName==NULL)&&(strlen(pszFileName)==0)),"file name is empty");
		
	STDERR ( AgInfiniiVision_MemorySave (hInstrumentHandle, AGINFINIIVISION_VAL_MEMORY_SAVE_OPTIONS_SETUP , pszFileName));
	
	ProcessSystemEvents(); 
	
	STDERR ( AgInfiniiVision_SystemWaitForOperationComplete ( hInstrumentHandle , 20000 ));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/*********************************************** Memory State Save ***********************************************/

void* DLLEXPORT Oscilloscope_MemoryState_Recall( int hInstrumentHandle , char *pszFileName )
{
	STD_ERROR               StdError					=	{0};
	
	char					szFileName[512]				=	{0},
							szCurrentDir[1024]			=	{0};
	
	int						iCount						=	0;
	
	char					*pStateName					=	NULL;
	
	IF (((pszFileName==NULL)&&(strlen(pszFileName)==0)),"file name is empty");
	
	pStateName = SkipWhiteSpace (pszFileName); 
		
	STDERR ( AgInfiniiVision_MemoryRecall( hInstrumentHandle , AGINFINIIVISION_VAL_MEMORY_RECALL_OPTIONS_SETUP , pStateName ));

	ProcessSystemEvents(); 
	
	STDERR ( AgInfiniiVision_SystemWaitForOperationComplete ( hInstrumentHandle , 20000 ));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

void main (void)
{
	int					error									=	0;
	
	int					InstrumentChanelsQuantity				=	0,
						InstrumentConnectStatus					=	0,
						viInstrumentHandle						=	0;
	
	double				lfValue									=	0.0,
						lfX_init								=	0.0,  
						lfX_inc									=	0.0;  
						
	char				szBuffer[512]							=	{0};
		
	double				vlfMeasureArray[100000]					=	{0.0};
	
	int					iCount									=	0;
	
	//Oscilloscope_Init ( 0 , "USB0::0x0957::0x0588::CN49322824::INSTR" , &viInstrumentHandle , &InstrumentConnectStatus , &InstrumentChanelsQuantity );

	//Oscilloscope_Reset(viInstrumentHandle); 
	
	//Oscilloscope_HorizontalOffset ( viInstrumentHandle, 1.5E-6 ); 
		
	//Oscilloscope_MemoryState_Recall( viInstrumentHandle , "\\usb\\scope_setup_IFF\\power_up_bit.scp");  
	
	//Oscilloscope_GetWaveForm ( viInstrumentHandle , 1 , 100000 , 20 , vlfMeasureArray, &iCount, &lfX_init, &lfX_inc );
		
	//Oscilloscope_Reset(viInstrumentHandle);
		
	//Oscilloscope_Close(&viInstrumentHandle);   
	
		
	//Oscilloscope_MemoryState_Recall( viInstrumentHandle , "setup_1");  

	//Oscilloscope_AutoSetup ( viInstrumentHandle ); 
	
	//Oscilloscope_MeasureFrequency ( viInstrumentHandle , 1 , 20.0 , &lfValue );
	
	//Oscilloscope_ConfigMeasureDelay ( viInstrumentHandle , 1 , 1 , 1 , 2 , 1 , 1 );
	//Oscilloscope_MeasureDelay ( viInstrumentHandle , 2 , 20.0 , &lfValue );  
	
	//Oscilloscope_MeasureRiseTime ( viInstrumentHandle , 1 , 20.0 , &lfValue );
	//Oscilloscope_MeasureFallTime ( viInstrumentHandle , 1 , 20.0 , &lfValue );
	
	//Oscilloscope_MeasurePeriod ( viInstrumentHandle , 1 , 20.0 , &lfValue );
	//Oscilloscope_MeasureVoltageRMS ( viInstrumentHandle , 1 , 20.0 , &lfValue );
	//Oscilloscope_MeasureVoltagePk2Pk ( viInstrumentHandle , 1 , 20.0 , &lfValue );
	//Oscilloscope_MeasureVoltageMax ( viInstrumentHandle , 1 , 20.0 , &lfValue );
	//Oscilloscope_MeasureVoltageMin ( viInstrumentHandle , 1 , 20.0 , &lfValue );
	//Oscilloscope_MeasureVoltageHigh ( viInstrumentHandle , 1 , 20.0 , &lfValue );
	//Oscilloscope_MeasureVoltageLow ( viInstrumentHandle , 1 , 20.0 , &lfValue );
	//Oscilloscope_MeasureVoltageAverage ( viInstrumentHandle , 1 , 20.0 , &lfValue );
	//Oscilloscope_MeasureWidthNegative ( viInstrumentHandle , 1 , 20.0 , &lfValue );
	//Oscilloscope_MeasureWidthPositive ( viInstrumentHandle , 1 , 20.0 , &lfValue );
	//Oscilloscope_MeasureDutyCycleNegative ( viInstrumentHandle , 1 , 20.0 , &lfValue );
	//Oscilloscope_MeasureDutyCyclePositive ( viInstrumentHandle , 1 , 20.0 , &lfValue );
	//Oscilloscope_MeasureAmplitude ( viInstrumentHandle , 1 , 20.0 , &lfValue );
	//Oscilloscope_MeasureVoltageCycleRMS ( viInstrumentHandle , 1 , 20.0 , &lfValue );
	//Oscilloscope_MeasureVoltageCycleAverage ( viInstrumentHandle , 1 , 20.0 , &lfValue );
	//Oscilloscope_MeasurePositiveOvershoot ( viInstrumentHandle , 1 , 20.0 , &lfValue );
	//Oscilloscope_MeasureNegativeOvershoot ( viInstrumentHandle , 1 , 20.0 , &lfValue );
	
	//Oscilloscope_Close(&viInstrumentHandle);
	
	return;
}
