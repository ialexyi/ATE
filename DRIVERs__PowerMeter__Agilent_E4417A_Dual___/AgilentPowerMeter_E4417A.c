
//==============================================================================
// Include files

#include <windows.h>
#include "toolbox.h"
#include "age441x.h"
#include <cvirte.h>
#include <userint.h>
#include <visa.h>
#include <utility.h>
#include "GlobalDefines.h"

//==============================================================================
// Constants

#define			MODULE_NAME			"AgilentPowerMeter_441X"

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
int		Delay_LastComplete( int handle );

void* DLLEXPORT PowerMeter_Close( int *phInstrumentHandle );


void* DLLEXPORT PowerMeter_GetLastErrorTextMessage( int handle , int iError , char **pErrorMessage )
{
	errEV					ret									=	{0};
	errEV					*pRet								=	NULL;
													
	char					szLocalErrorMessage[5000]			=	{0};
	
	int						iSize								=	0;
	
	if ( pErrorMessage ) 
	{
		age441x_error_message ( handle , iError , szLocalErrorMessage );

		iSize = strlen(szLocalErrorMessage);
	
		CALLOC_COPY_ERR( *pErrorMessage , iSize , sizeof(char) , szLocalErrorMessage , iSize );
	}
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
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

void* DLLEXPORT PowerMeter_Init( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *pInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChannelsQuantity )
{			
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	IF(( szRsrcAddress == NULL ) , "Empty Instrument driver" , MODULE_NAME );
	IF(( InstrumentConnectStatus == NULL ) , "Passed null pointer" , MODULE_NAME );
	IF(( InstrumentChannelsQuantity == NULL ) , "Passed null pointer" , MODULE_NAME );
	
	*InstrumentChannelsQuantity = 0;
	*InstrumentConnectStatus = 0;
	
	if ( hParentInstrumentHandle == 0 )
	{
		ret.error = age441x_init ( szRsrcAddress , VI_ON , VI_OFF , pInstrumentHandle );
		
		if ( ret.error == 0 )
		{
			*InstrumentConnectStatus = 1;
			
			CHK_ERR(  age441x_configureInput ( *pInstrumentHandle , 0 , 50000 , AGE441X_MEAS_DOUBLE )); 
		}
			
	}
	else
	{
		*InstrumentConnectStatus = 1;
	}
		
Error:
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}


void* DLLEXPORT PowerMeter_Close( int *phInstrumentHandle )
{	
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	IF ( (phInstrumentHandle == NULL) , "Pointer passed NULL." , MODULE_NAME);
						  
	age441x_close (*phInstrumentHandle);
	*phInstrumentHandle=0;
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_Reset( int hInstrumentHandle )
{	
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	ret.error = age441x_reset ( hInstrumentHandle );
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_Configuration( int hInstrumentHandle , int iChannel , int bContinue , int bDelayAutoState , int iTriggerSource , double lfTrigHoldOff , double lfGateOffsetTime , double lfGateTime , double lfTraceTime )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
		switch(iChannel)
	{
		case 1:
				iChannel = AGE441X_CHANNEL_A;
				break;
		case 2:
				iChannel = AGE441X_CHANNEL_B;
				break;
				
		default:
				iChannel = AGE441X_CHANNEL_A;
				break;
	}
		
	CHK_ERR( age441x_setInitiateContinuousState ( hInstrumentHandle , iChannel , bContinue ));
	
	CHK_ERR( age441x_configureTrigger ( hInstrumentHandle , iChannel , bDelayAutoState , iTriggerSource ));
	
	CHK_ERR( age441x_configureChannelATrigger ( hInstrumentHandle , 0.000 , lfTrigHoldOff , 0.00 , 0.0 , AGE441X_STATE_ON, AGE441X_SLOPE_POS));
																			 
	CHK_ERR( age441x_e9320ConfigureGate ( hInstrumentHandle , iChannel , 1 , lfGateOffsetTime , lfGateTime ));
	
	CHK_ERR( age441x_e9320ConfigureTrace ( hInstrumentHandle , iChannel , VI_TRUE , 0.0 , lfTraceTime , -50.0 , 20.0 , AGE441X_UNIT_DBM));

Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

//age441x_setInitiateContinuousState (instrHandle, 0, 1);
//age441x_configureTrigger (instrHandle, 0, 1,4);
//age441x_configureChannelATrigger (instrHandle, 0.000, Trig_HoldOff, 0.00, 0.0,1, 1); 
//age441x_e9320ConfigureGate (instrHandle, 0, 1, Gate_OffsetTime, Gate_Time); 
//age441x_e9320ConfigureTrace (instrHandle, 0,1, 0.0, 0.004, -50.0,20.0, 0);
	

void* DLLEXPORT PowerMeter_SetSingleSweep( int hInstrumentHandle , int iChannel )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
		switch(iChannel)
	{
		case 1:
				iChannel = AGE441X_CHANNEL_A;
				break;
		case 2:
				iChannel = AGE441X_CHANNEL_B;
				break;
				
		default:
				iChannel = AGE441X_CHANNEL_A;
				break;
	}
		
	CHK_ERR(  age441x_setInitiateContinuousState ( hInstrumentHandle , iChannel , VI_FALSE ));
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_SetContinuesSweep( int hInstrumentHandle , int iChannel ) 
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
		switch(iChannel)
	{
		case 1:
				iChannel = AGE441X_CHANNEL_A;
				break;
		case 2:
				iChannel = AGE441X_CHANNEL_B;
				break;
				
		default:
				iChannel = AGE441X_CHANNEL_A;
				break;
	}
		
	CHK_ERR(  age441x_setInitiateContinuousState ( hInstrumentHandle , iChannel , VI_TRUE ));
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}


void* DLLEXPORT PowerMeter_InitSweep( int hInstrumentHandle , int iChannel ) 
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
		switch(iChannel)
	{
		case 1:
				iChannel = AGE441X_CHANNEL_A;
				break;
		case 2:
				iChannel = AGE441X_CHANNEL_B;
				break;
				
		default:
				iChannel = AGE441X_CHANNEL_A;
				break;
	}
		
	CHK_ERR(  age441x_initiate ( hInstrumentHandle , iChannel ));
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}


void* DLLEXPORT PowerMeter_RecallState( int hInstrumentHandle , int iStateNumber ) 
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	CHK_ERR(  age441x_loadFromRegister( hInstrumentHandle , iStateNumber ));  
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_SetAuto( int hInstrumentHandle , int channel ) 
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_SetChanelState( int hInstrumentHandle , int channel , int state )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
		
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_SetVerticalScale( int hInstrumentHandle , int channel , double scale )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_SetVerticalCenter( int hInstrumentHandle , int channel , double center )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_Calibrate( int hInstrumentHandle , int iChannel )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	short					iStatus				=	0;
	
	switch(iChannel)
	{
		case 1:
				iChannel = AGE441X_CHANNEL_A;
				break;
		case 2:
				iChannel = AGE441X_CHANNEL_B;
				break;
				
		default:
				iChannel = AGE441X_CHANNEL_A;
				break;
	}
		
	CHK_ERR(  age441x_calibration ( hInstrumentHandle , iChannel , AGE441X_CAL_ALL , &iStatus ));
	
	if ( iStatus )
	{
		ret.error = -1;
		strcpy( ret.szErrorDescription , "Calibration Fail." );
	}   
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_Zero( int hInstrumentHandle , int iChannel )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	short					iStatus				=	0;
	
	switch(iChannel)
	{
		case 1:
				iChannel = AGE441X_CHANNEL_A;
				break;
		case 2:
				iChannel = AGE441X_CHANNEL_B;
				break;
				
		default:
				iChannel = AGE441X_CHANNEL_A;
				break;
	}
		
	CHK_ERR(  age441x_calibration ( hInstrumentHandle , iChannel , AGE441X_CAL_ZERO , &iStatus ));
	
	if ( iStatus )
	{
		ret.error = -1;
		strcpy( ret.szErrorDescription , "Calibration Fail." );
	}
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_SetUnits( int hInstrumentHandle , int channel , unsigned int units )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

void* DLLEXPORT PowerMeter_SetOffset( int hInstrumentHandle , int iChannel , double Offset )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	 	
	switch(iChannel)
	{
		case 1:
				iChannel = AGE441X_CHANNEL_A;
				break;
		case 2:
				iChannel = AGE441X_CHANNEL_B;
				break;
				
		default:
				iChannel = AGE441X_CHANNEL_A;
				break;
	}
	
	CHK_ERR(  age441x_configureChannelOffset ( hInstrumentHandle , iChannel , VI_TRUE , Offset ));
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_SetCalFactor( int hInstrumentHandle , int channel , double CalFactor )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_SetVideoBW( int hInstrumentHandle , int channel ,  unsigned int VideoBW )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_SetPeakHold( int hInstrumentHandle , int channel , int state )
{
	return 0;
}

void* DLLEXPORT PowerMeter_SetFrequency( int hInstrumentHandle , int iChannel , double dFreq )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	switch(iChannel)
	{
		case 1:
				iChannel = AGE441X_CHANNEL_A;
				break;
		case 2:
				iChannel = AGE441X_CHANNEL_B;
				break;
				
		default:
				iChannel = AGE441X_CHANNEL_A;
				break;
	}
			  
	CHK_ERR(  age441x_configureFrequency ( hInstrumentHandle , iChannel , dFreq ));
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_SetMode( int hInstrumentHandle , unsigned int Mode )
{
	return 0;
}

void* DLLEXPORT PowerMeter_OPC( int hInstrumentHandle , double lfTimeOut , int *state )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;

	ViAttrState				attrValue			=	0;
	
	attrValue = lfTimeOut * 1E3;
	
	viSetAttribute ( hInstrumentHandle , VI_ATTR_TMO_VALUE , lfTimeOut );
		
	if ( state )
		*state = Delay_LastComplete( hInstrumentHandle );
		
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_FetchCWpower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	double			 		lfPower				=	0;
	
	CHK_ERR(  age441x_fetch ( hInstrumentHandle , 2 , AGE441X_MEAS_SINGLE , &lfPower ));
	
	if ( value )
		*value = lfPower;
		
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_ReadPower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	double			 		lfPower				=	0;
	
	CHK_ERR(  age441x_read ( hInstrumentHandle , 1 , AGE441X_MEAS_SINGLE , &lfPower ));
	
	if ( value )
		*value = lfPower;
		
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_FetchPower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	double			 		lfPower				=	0;
	
	CHK_ERR(  age441x_fetch ( hInstrumentHandle , 2 , AGE441X_MEAS_SINGLE , &lfPower ));
	
	if ( value )
		*value = lfPower;
		
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_MeasurePower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	double			 		lfPower				=	0;
	
	ViBoolean				bLimit				=	0;
	
	switch(iChannel)
	{
		case 1:
				iChannel = AGE441X_CHANNEL_A;
				break;
		case 2:
				iChannel = AGE441X_CHANNEL_B;
				break;
				
		default:
				iChannel = AGE441X_CHANNEL_A;
				break;
	}
		
	CHK_ERR(  age441x_powerMeasurement ( hInstrumentHandle , iChannel , AGE441X_UNIT_DBM , -90.0, 90.0 , &bLimit , &lfPower ));

	if ( value )
		*value = lfPower;
	
	//CHK_ERR(  age441x_setInitiateContinuousState ( hInstrumentHandle , iChannel , VI_TRUE ));
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}
  
void* DLLEXPORT PowerMeter_MeasureModulatedPower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	float 					fPower				=	0;
   
   	short					statusbyte			=	0,
							failbyte			=	0,
							averageCount		=	0;
	
	switch(iChannel)
	{
		case 1:
				iChannel = AGE441X_CHANNEL_A;
				break;
		case 2:
				iChannel = AGE441X_CHANNEL_B;
				break;
				
		default:
				iChannel = AGE441X_CHANNEL_A;
				break;
	}
	
   	DelayWithEventProcessing( timeout ); 
   						   
	*value = (double)fPower;
		
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);     
}

void* DLLEXPORT PowerMeter_FetchMarkerAverage( int hInstrumentHandle , int iChannel , int marker , double timeout , double *value )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	switch(iChannel)
	{
		case 1:
				iChannel = AGE441X_CHANNEL_A;
				break;
		case 2:
				iChannel = AGE441X_CHANNEL_B;
				break;
				
		default:
				iChannel = AGE441X_CHANNEL_A;
				break;
	}
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_FetchMarkerMax( int hInstrumentHandle , int iChannel , int marker , double timeout , double *value )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	switch(iChannel)
	{
		case 1:
				iChannel = AGE441X_CHANNEL_A;
				break;
		case 2:
				iChannel = AGE441X_CHANNEL_B;
				break;
				
		default:
				iChannel = AGE441X_CHANNEL_A;
				break;
	}
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_FetchMarkerMin( int hInstrumentHandle , int iChannel , int marker , double timeout , double *value )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	switch(iChannel)
	{
		case 1:
				iChannel = AGE441X_CHANNEL_A;
				break;
		case 2:
				iChannel = AGE441X_CHANNEL_B;
				break;
				
		default:
				iChannel = AGE441X_CHANNEL_A;
				break;
	}
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}


int		Delay_LastComplete( int handle )
{
	double		lfCurrentTime		=	0.0,
				lfTimeOut			=	10.0, 
				lfStartTime			=	0.0;
	
	int			iOpcValue			=	0;
	
	int			status				=	0;
	
	ViAttrState	attrValue			=	0;
		
	if ( viGetAttribute ( handle , VI_ATTR_TMO_VALUE , &attrValue ) == 0 )
		lfTimeOut = attrValue / 1E3;

	GetCurrentDateTime ( &lfStartTime );
	GetCurrentDateTime ( &lfCurrentTime ); 
	
	while(( lfCurrentTime - lfStartTime ) < lfTimeOut ) 
	{
		
		GetCurrentDateTime ( &lfCurrentTime );  
		
		status = viPrintf ( handle , "*OPC?\n");
	
		if ( status )
			continue;
		
		Delay ( 0.2 );
		
		status = viScanf ( handle , "%d" , &iOpcValue);

		if ( status )
			continue;
		
		if ( iOpcValue == 1 )
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
	
	PowerMeter_Init( 0 , "GPIB::13" , &InstrumentHandle , &InstrumentConnectStatus , &InstrumentChannelsQuantity );
	
	PowerMeter_Reset( InstrumentHandle ); 
	
	PowerMeter_SetFrequency( InstrumentHandle , 0 , 1.3E9 );
		
	PowerMeter_ReadPower( InstrumentHandle , 0 , 10.0 , &value ); 
	
	PowerMeter_SetFrequency( InstrumentHandle , 0 , 1.3E9 );
		
	PowerMeter_ReadPower( InstrumentHandle , 0 , 10.0 , &value ); 
	
	PowerMeter_SetFrequency( InstrumentHandle , 0 , 1.3E9 );
		
	PowerMeter_ReadPower( InstrumentHandle , 0 , 10.0 , &value ); 
	
	
	PowerMeter_SetFrequency( InstrumentHandle , 0 , 1.3E9 );
		
	PowerMeter_ReadPower( InstrumentHandle , 0 , 10.0 , &value ); 
	
	
	PowerMeter_SetFrequency( InstrumentHandle , 0 , 1.3E9 );
		
	PowerMeter_ReadPower( InstrumentHandle , 0 , 10.0 , &value ); 
	
	
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

