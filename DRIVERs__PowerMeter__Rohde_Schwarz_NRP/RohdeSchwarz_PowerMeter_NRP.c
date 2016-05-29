//==============================================================================
// Include files

//#include "RohdeSchwarz_PowerMeter_NRP.h"

//==============================================================================
// Constants
#include <windows.h>
#include "toolbox.h"
#include <cvirte.h>
#include <userint.h>
#include <visa.h>
#include <utility.h>
#include "GlobalDefines.h"
#include <Ivi.h>
#include "rsnrpz.h"
#include <formatio.h>

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions
int		Wait_MeasureComplete( int handle , int iChannel , double lfTimeout );
int		Wait_CalibrationComplete( int handle , int iChannel , double lfTimeout );
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

void* DLLEXPORT PowerMeter_GetLastErrorTextMessage( int handle , int iError , char **pErrorMessage )
{
	STD_ERROR					StdError									=	{0};
	
													
	char					szLocalErrorMessage[5000]			=	{0};
	
	int						iSize								=	0;
	
	if ( pErrorMessage ) 
	{
		rsnrpz_error_message ( handle , iError , szLocalErrorMessage );

		iSize = strlen(szLocalErrorMessage);
	
		CALLOC_COPY_ERR( *pErrorMessage , iSize , sizeof(char) , szLocalErrorMessage , iSize );
	}
	
Error:	
	
	RETURN_STDERR_POINTER;
}

void*	DLLEXPORT		Equipment_Info ( int hLowLevelHandle , char *pAddress , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware )
{
	STD_ERROR						StdError									=	{0};
	
/*	ViSession						hInstrumentHandle							=	0;
	
	if ( pAddress )
	{
		if ( rsnrpz_init (pAddress, VI_TRUE, VI_TRUE, &hInstrumentHandle ) == 0 )
			if ( hInstrumentHandle )
			{
				if ( pVendor )
				{
					CALLOC_COPY_STRING( *pVendor , "Rohde & Schwarz" ); 
				}
			
				rsnrpz_GetSensorInfo ( hInstrumentHandle , 1, NULL, pModel , pSerialNumber );
			
				//rsnrpz_revision_query ( hInstrumentHandle , NULL  , pFirmware);
			
				rsnrpz_close (hInstrumentHandle);
			}
	}
	
	
Error:
*/		
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
/*	
	ViSession						hInstrumentHandle							=	0;
	
	if ( rsnrpz_init (pAddress, VI_TRUE, VI_TRUE, &hInstrumentHandle ) == 0 )
		if ( hInstrumentHandle )
		{
			if( pbSupporting )
				*pbSupporting = 1;
			
			rsnrpz_close (hInstrumentHandle);
		}
*/	
	RETURN_STDERR_POINTER;
}

void*	DLLEXPORT		Config_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet )
{
	STD_ERROR						StdError									=	{0};
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT PowerMeter_Init( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *pInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChannelsQuantity )
{			
	STD_ERROR					StdError					=	{0};
	
	
	IF(( szRsrcAddress == NULL ) , "Empty Instrument driver"  );
	IF(( InstrumentConnectStatus == NULL ) , "Passed null pointer"  );
	IF(( InstrumentChannelsQuantity == NULL ) , "Passed null pointer"  );
	
	*InstrumentChannelsQuantity = 0;
	*InstrumentConnectStatus = 0;
	
	if ( hParentInstrumentHandle == 0 )
	{
		StdError.error = rsnrpz_init (szRsrcAddress, VI_TRUE, VI_TRUE, (ViSession*)pInstrumentHandle);
		
		if ( StdError.error == 0 )
		{
			rsnrpz_status_preset (*pInstrumentHandle);
			
			rsnrpz_chans_getCount ( *pInstrumentHandle , InstrumentChannelsQuantity );
			
			//rsnrpz_chans_zero(*pInstrumentHandle);  
			
			*InstrumentConnectStatus = 1;
		}
			
	}
	else
	{
		*InstrumentConnectStatus = 1;
	}
		
Error:
	
	RETURN_STDERR_POINTER;  
}


void* DLLEXPORT PowerMeter_Close( int *phInstrumentHandle )
{	
	STD_ERROR					StdError					=	{0};
	
	
	IF ( (phInstrumentHandle == NULL) , "Pointer passed NULL." );
						  
	rsnrpz_close (*phInstrumentHandle);
	*phInstrumentHandle=0;
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Reset( int hInstrumentHandle )
{	
	STD_ERROR					StdError					=	{0};
	
	STD_ERR( rsnrpz_reset ( hInstrumentHandle ));
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Configuration( int hInstrumentHandle , int iChannel , int bContinue , int bDelayAutoState , int iTriggerSource , double lfTrigHoldOff , double lfGateOffsetTime , double lfGateTime , double lfTraceTime )
{
	STD_ERROR					StdError					=	{0};

	STD_ERR( rsnrpz_chan_setInitContinuousEnabled ( hInstrumentHandle , iChannel , bContinue ));

	STD_ERR( rsnrpz_chan_initiate ( hInstrumentHandle , iChannel ));
	/*
	STD_ERR( rsnrpz_configureTrigger ( hInstrumentHandle , iChannel , bDelayAutoState , iTriggerSource ));
	
	STD_ERR( rsnrpz_configureChannelATrigger ( hInstrumentHandle , 0.000 , lfTrigHoldOff , 0.00 , 0.0 , rsnrpz_STATE_ON, rsnrpz_SLOPE_POS));
																			 
	STD_ERR( rsnrpz_e9320ConfigureGate ( hInstrumentHandle , iChannel , 1 , lfGateOffsetTime , lfGateTime ));
	
	STD_ERR( rsnrpz_e9320ConfigureTrace ( hInstrumentHandle , iChannel , VI_TRUE , 0.0 , lfTraceTime , -50.0 , 20.0 , rsnrpz_UNIT_DBM));
	*/
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetSingleSweep( int hInstrumentHandle , int iChannel )
{
	STD_ERROR					StdError					=	{0};

	STD_ERR( rsnrpz_chan_setInitContinuousEnabled ( hInstrumentHandle , iChannel , 0 ));

	STD_ERR( rsnrpz_chan_initiate ( hInstrumentHandle , iChannel ));
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetContinuesSweep( int hInstrumentHandle , int iChannel ) 
{
	STD_ERROR					StdError					=	{0};

	STD_ERR( rsnrpz_chan_setInitContinuousEnabled ( hInstrumentHandle , iChannel , 1 ));

	STD_ERR( rsnrpz_chan_initiate ( hInstrumentHandle , iChannel ));
	
Error:	
	
	RETURN_STDERR_POINTER;  
}


void* DLLEXPORT PowerMeter_InitSweep( int hInstrumentHandle , int iChannel ) 
{
	STD_ERROR					StdError					=	{0};

	STD_ERR( rsnrpz_chan_initiate ( hInstrumentHandle , iChannel ));
	
Error:	
	
	RETURN_STDERR_POINTER;  
}


void* DLLEXPORT PowerMeter_RecallState( int hInstrumentHandle , int iStateNumber ) 
{
	STD_ERROR					StdError					=	{0};

//Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_RecallState_ByName( int hInstrumentHandle , char *pszName ) 
{
	STD_ERROR					StdError					=	{0};

//Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SaveState( int hInstrumentHandle , int iStateNumber ) 
{
	STD_ERROR					StdError					=	{0};
	
//Error:	
	
	
	RETURN_STDERR_POINTER;  
}


void* DLLEXPORT PowerMeter_GetStateFile( int hInstrumentHandle , int iStateNumber , char *pszFileName ) 
{
	STD_ERROR					StdError					=	{0};
	
//Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetStateFile( int hInstrumentHandle , int iStateNumber , char *pszFileName ) 
{
	STD_ERROR					StdError					=	{0};
	
//Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_GetStateName( int hInstrumentHandle , int iStateNumber , char *pszName ) 
{
	STD_ERROR					StdError					=	{0};
	
//Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetStateName( int hInstrumentHandle , int iStateNumber , char *pszName ) 
{
	STD_ERROR					StdError					=	{0};
	
//Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetAuto( int hInstrumentHandle , int channel ) 
{
	STD_ERROR					StdError					=	{0};
	
//Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetChanelState( int hInstrumentHandle , int channel , int state )
{
	STD_ERROR					StdError					=	{0};
	
//Error:	
	
	RETURN_STDERR_POINTER;  

}

void* DLLEXPORT PowerMeter_SetVerticalScale( int hInstrumentHandle , int channel , double scale )
{
	STD_ERROR					StdError					=	{0};

//Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetVerticalCenter( int hInstrumentHandle , int channel , double center )
{
	STD_ERROR					StdError					=	{0};
	
//Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Calibrate( int hInstrumentHandle , int iChannel )
{
	STD_ERROR					StdError					=	{0};
	
	STD_ERR( rsnrpz_chan_zero ( hInstrumentHandle , iChannel ));
	
	Wait_CalibrationComplete( hInstrumentHandle , iChannel , 20.0 );
		
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Zero( int hInstrumentHandle , int iChannel )
{
	STD_ERROR					StdError					=	{0};
	
	STD_ERR( rsnrpz_chan_zero ( hInstrumentHandle , iChannel ));
	
	Wait_CalibrationComplete( hInstrumentHandle , iChannel , 20.0 );
	
Error:	
	
	RETURN_STDERR_POINTER;  
}


void* DLLEXPORT PowerMeter_SetActiveChannel( int hInstrumentHandle , int iChannel )
{
	STD_ERROR					StdError					=	{0};
//Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetUnits( int hInstrumentHandle , int channel , unsigned int units )
{
	STD_ERROR					StdError					=	{0};
	
//Error:	
	
	RETURN_STDERR_POINTER;  	
}

void* DLLEXPORT PowerMeter_SetOffset( int hInstrumentHandle , int iChannel , double Offset )
{
	STD_ERROR					StdError					=	{0};
	
	STD_ERR( rsnrpz_corr_setOffset ( hInstrumentHandle , iChannel , Offset ));	
	
	STD_ERR( rsnrpz_corr_setOffsetEnabled ( hInstrumentHandle , iChannel , VI_TRUE ));
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetCalFactor( int hInstrumentHandle , int channel , double CalFactor )
{
	STD_ERROR					StdError					=	{0};
	
//Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetVideoBW( int hInstrumentHandle , int channel ,  unsigned int VideoBW )
{
	STD_ERROR					StdError					=	{0};
	
	
	STD_ERR( rsnrpz_bandwidth_setBw ( hInstrumentHandle , channel , VideoBW ));
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetPeakHold( int hInstrumentHandle , int channel , int state )
{
	STD_ERROR					StdError					=	{0};   

//Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetFrequency( int hInstrumentHandle , int iChannel , double dFreq )
{
	STD_ERROR					StdError					=	{0};

	STD_ERR( rsnrpz_chan_setCorrectionFrequency ( hInstrumentHandle , iChannel , dFreq ));
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_SetMode( int hInstrumentHandle , unsigned int Mode )
{
	STD_ERROR					StdError					=	{0};

	STD_ERR( rsnrpz_chan_mode ( hInstrumentHandle , 1 , Mode ));

Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Channel_SetMode( int hInstrumentHandle , int iChannel , unsigned int Mode )
{
	STD_ERROR					StdError					=	{0};

	STD_ERR( rsnrpz_chan_mode ( hInstrumentHandle , iChannel , Mode ));
	
Error:
		
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_FetchCWpower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};
	
	double			 			lfPower						=	0;
	
	STD_ERR( rsnrpz_meass_fetchMeasurement ( hInstrumentHandle , iChannel , &lfPower ));
	
	if ( value )
		*value = lfPower;
		
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_ReadPower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};
	
	double			 			lfPower						=	0;
	
	STD_ERR( rsnrpz_meass_readMeasurement ( hInstrumentHandle , iChannel , (timeout*1E3), &lfPower ));
	
	if ( value )
		*value = lfPower;
		
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_FetchPower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};
	
	double			 			lfPower						=	0;
	
	STD_ERR( rsnrpz_meass_fetchMeasurement ( hInstrumentHandle , iChannel , &lfPower ));
	
	if ( value )
		*value = lfPower;
		
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_MeasurePower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};
	
	double			 			lfPower						=	0;
	
	STD_ERR( rsnrpz_meass_readMeasurement ( hInstrumentHandle , iChannel , (timeout*1E3), &lfPower ));
	
	if ( value )
		*value = lfPower;
		
Error:	
	
	RETURN_STDERR_POINTER;  
}
  
void* DLLEXPORT PowerMeter_MeasureModulatedPower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};
	
	double			 			lfPower						=	0;
	
	STD_ERR( rsnrpz_meass_readMeasurement ( hInstrumentHandle , iChannel , (timeout*1E3), &lfPower ));
	
	if ( value )
		*value = lfPower;
		
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_FetchMarkerAverage( int hInstrumentHandle , int iChannel , int marker , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};

//Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_FetchMarkerMax( int hInstrumentHandle , int iChannel , int marker , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};

//Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_FetchMarkerMin( int hInstrumentHandle , int iChannel , int marker , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};

//Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Gating_FetchMaxPower( int hInstrumentHandle , int iChannel , double *value )
{
	STD_ERROR					StdError					=	{0};
	
	double			 			lfPower						=	0;
	
	STD_ERR( rsnrpz_meass_fetchMeasurement ( hInstrumentHandle , iChannel , &lfPower ));
	
	if ( value )
		*value = lfPower;
		
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Gating_ReadMaxPower( int hInstrumentHandle , int iChannel , int iGait , double timeout , double *value )
{
	STD_ERROR					StdError					=	{0};
	
	double			 			lfPower						=	0;
	
	STD_ERR( rsnrpz_meass_readMeasurement ( hInstrumentHandle , iChannel , (timeout*1E3), &lfPower ));
	
	if ( value )
		*value = lfPower;
		
Error:	
	
	RETURN_STDERR_POINTER;  
}

void* DLLEXPORT PowerMeter_Gating_ConfigOffsetTime( int hInstrumentHandle , int iChannel , int iGate , double lfTime , double lfOffset , double lfMidambleOffset, double lfMidambleLength )
{
	STD_ERROR					StdError					=	{0};

	STD_ERR( rsnrpz_stat_confTimegate ( hInstrumentHandle , iChannel , lfOffset, lfTime , lfMidambleOffset , lfMidambleLength ));
	
Error:	
	
	RETURN_STDERR_POINTER;  
}

int		Wait_MeasureComplete( int handle , int iChannel , double lfTimeout )
{
	double		lfCurrentTime			=	0.0,
				lfStartTime				=	0.0;
	
	ViBoolean	bOpcValue				=	0;

	GetCurrentDateTime ( &lfStartTime );
	GetCurrentDateTime ( &lfCurrentTime ); 
	
	while(( lfCurrentTime - lfStartTime ) < lfTimeout ) 
	{
		
		GetCurrentDateTime ( &lfCurrentTime );  
		
		rsnrpz_chan_isMeasurementComplete ( handle , iChannel , &bOpcValue );

		if ( bOpcValue == 1 )
			return 0;
	}
	
	return -1;	
}

int		Wait_CalibrationComplete( int handle , int iChannel , double lfTimeout )
{
	double		lfCurrentTime			=	0.0,
				lfStartTime				=	0.0;
	
	ViBoolean	bOpcValue				=	0;

	GetCurrentDateTime ( &lfStartTime );
	GetCurrentDateTime ( &lfCurrentTime ); 
	
	while(( lfCurrentTime - lfStartTime ) < lfTimeout ) 
	{
		
		GetCurrentDateTime ( &lfCurrentTime );  
		
		rsnrpz_chan_isZeroComplete ( handle , iChannel , &bOpcValue);

		if ( bOpcValue == 1 )
			return 0;
	}
	
	return -1;	
}

int main(void)
{
	int					InstrumentHandle			=	0,
						InstrumentConnectStatus		=	0,
						InstrumentChannelsQuantity	=	0;
		
	double				value						=	0.0;
	
	PowerMeter_Init( 0 , "*" , &InstrumentHandle , &InstrumentConnectStatus , &InstrumentChannelsQuantity );
	
	PowerMeter_ReadPower( InstrumentHandle , 1 , 10.0 , &value );
	PowerMeter_ReadPower( InstrumentHandle , 1 , 10.0 , &value );
	PowerMeter_ReadPower( InstrumentHandle , 1 , 10.0 , &value );
	PowerMeter_ReadPower( InstrumentHandle , 1 , 10.0 , &value );
	PowerMeter_ReadPower( InstrumentHandle , 1 , 10.0 , &value );
	PowerMeter_ReadPower( InstrumentHandle , 1 , 10.0 , &value );
	PowerMeter_ReadPower( InstrumentHandle , 1 , 10.0 , &value );
	PowerMeter_ReadPower( InstrumentHandle , 1 , 10.0 , &value );
	PowerMeter_ReadPower( InstrumentHandle , 1 , 10.0 , &value );

	PowerMeter_SetActiveChannel( InstrumentHandle , 2 );
		
	PowerMeter_FetchCWpower( InstrumentHandle , 2 , 10.0 , &value ); 
	
	
	//PowerMeter_Reset( InstrumentHandle ); 
		
	//PowerMeter_RecallState( InstrumentHandle , 10 ); 
	
	PowerMeter_RecallState_ByName( InstrumentHandle , "iff_tester" );   
		
	//PowerMeter_Gating_ConfigOffsetTime( InstrumentHandle , 1 , 1 , 20E-6 ); 
		
	PowerMeter_Gating_ReadMaxPower( InstrumentHandle , 1 , 1 , 20 , &value);
	
	//PowerMeter_FetchCWpower( InstrumentHandle , 1 , 0.0 , &value ); 
	//PowerMeter_ReadPower( InstrumentHandle , 1 , 10.0 , &value );
	
	//PowerMeter_GetStateFile( InstrumentHandle , 10 , "d:\\iff_ate_tester_pm.state" ); 
	//PowerMeter_SetStateFile( InstrumentHandle , 1 , "d:\\iff_ate_tester_pm.state" ); 
	//PowerMeter_RecallState( InstrumentHandle , 1 ); 
	
	//PowerMeter_GetStateName( InstrumentHandle , 10 , szName ); 
	//PowerMeter_SetStateName( InstrumentHandle , 10 , "iff_tester" ); 
	
	//PowerMeter_Reset( InstrumentHandle ); 
	
	PowerMeter_SetFrequency( InstrumentHandle , 0 , 1.3E9 );
		
	PowerMeter_ReadPower( InstrumentHandle , 0 , 10.0 , &value ); 
	

	/*PowerMeter_FetchCWpower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_FetchCWpower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_FetchCWpower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_FetchCWpower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_FetchCWpower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_FetchCWpower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_FetchCWpower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_FetchCWpower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_FetchCWpower( InstrumentHandle , 0 , 0.0 , &value ); 
	
	PowerMeter_SetContinuesSweep( InstrumentHandle , 0 ); 
		
	PowerMeter_MeasurePower( InstrumentHandle , 0 , 0.0 , &value );  
	
	PowerMeter_SetContinuesSweep( InstrumentHandle , 0 );
	
	PowerMeter_MeasurePower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_MeasurePower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_MeasurePower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_MeasurePower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_MeasurePower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_MeasurePower( InstrumentHandle , 0 , 0.0 , &value ); 
	PowerMeter_MeasurePower( InstrumentHandle , 0 , 0.0 , &value ); 
	
	PowerMeter_FetchCWpower( InstrumentHandle , 0 , 0.0 , &value );
	*/	
	PowerMeter_Close( &InstrumentHandle );
	
	return 0;
}

