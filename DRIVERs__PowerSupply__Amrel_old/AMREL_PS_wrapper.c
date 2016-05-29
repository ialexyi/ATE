//==============================================================================
// Include files
#include <windows.h>
#include <userint.h>
#include <ansi_c.h>
#include <utility.h>
#include "GlobalDefines.h"
#include "PPSMPS.h"
#include <Ivi.h>
#include "toolbox.h"
#include "AMREL_PS_wrapper.h"
#include <visa.h>

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

/*********************************** Get Error Text Message *****************************************/

void* DLLEXPORT PowerSupply_GetErrorTextMessage ( ViSession viInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR             StdError					=	{0};
	
	
	ppsmps_get_arg_range_error_msg ( iError , pErrorMessage );

	FREE_CALLOC( StdError.pszErrorDescription , STD_STRING , sizeof(char*));
				
	if ( StdError.pszErrorDescription )
	{
		CHK_ERR ( ppsmps_query_error_state	( viInstrumentHandle , &(StdError.error) , StdError.pszErrorDescription ));
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
		if (( strstr( szIdentificationText , "Agilent Technologies" ) == NULL ) && ( strstr( szIdentificationText ,"Hewlett-Packard") == NULL ))
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

/*********************************************** Init ***********************************************/

void* DLLEXPORT PowerSupply_Init ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , ViSession *viInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChanelsQuantity )
{	
	STD_ERROR            	StdError					=	{0};
						
	char					szErrorTextMessage[256]		=	{0};
	
	ViSession				handle						=	0;
	
	int						iAddress					=	0;
	
	if ( hParentInstrumentHandle == 0 )
	{
		sscanf( szRsrcAddress , "GPIB::%d" , &iAddress);
		CHK_ERR ( ppsmps_initialize (iAddress, PPSMPS_DUAL, 1, &handle, 1 ));
	}
	else
		handle = hParentInstrumentHandle;
		
	if ( viInstrumentHandle  ) 
			*viInstrumentHandle = handle;
	
	if ( InstrumentChanelsQuantity )
		*InstrumentChanelsQuantity = 2;

Error:
	
	if (StdError.error < 0) 
	{ 
		ppsmps_query_error_state( handle , &(StdError.error) , szErrorTextMessage );
		
		MessagePopup("Power Supply Initializing Error!",szErrorTextMessage);
		
		ppsmps_close (handle);
	}
	else
		if ( InstrumentConnectStatus )
			*InstrumentConnectStatus = 1;	
	
	RETURN_STDERR_POINTER;
}

/*********************************************** Close ***********************************************/

void* DLLEXPORT PowerSupply_Close ( ViSession *phInstrumentHandle )
{
	STD_ERROR             StdError					=	{0};
	  
	
	ViSession	handle						=	0;

	IF ( (phInstrumentHandle == NULL) , "Pointer passed NULL.");
	
	handle = *phInstrumentHandle;
	
	IF ( (handle == 0) , "Pointer passed 0.");

	CHK_ERR ( ppsmps_close ( handle ));
		
	*phInstrumentHandle = 0;

Error:
	
	
	
	RETURN_STDERR_POINTER;
}

/*********************************************** Reset ***********************************************/

void* DLLEXPORT PowerSupply_Reset ( ViSession viInstrumentHandle )   
{   
	STD_ERROR             StdError					=	{0};
	 
	CHK_ERR ( ppsmps_set_ch_on_off ( viInstrumentHandle , PPSMPS_DUAL, PPSMPS_CHAN_BOTH, 0 ));
	
Error: 
		 
	RETURN_STDERR_POINTER; 
}

/*********************************** Wait For Operation Complete ***********************************************/

void* DLLEXPORT PowerSupply_WaitForOperationComplete ( ViSession viInstrumentHandle , double lfTimeout )   
{   
	STD_ERROR                   StdError					=	{0};
	 
	FREE_CALLOC( StdError.pszErrorDescription , STD_STRING , sizeof(char*));
				
	if ( StdError.pszErrorDescription )
		ppsmps_query_error_state( viInstrumentHandle , &(StdError.error) , StdError.pszErrorDescription );
	
Error: 
	
	RETURN_STDERR_POINTER; 
}

/*********************************************** Get Model *****************************************/

void* DLLEXPORT PowerSupply_GetModel ( ViSession viInstrumentHandle , int channel , char szReturnString[256] )
{
	STD_ERROR             StdError					=	{0};

	CHK_ERR ( ppsmps_query_model_number (viInstrumentHandle, szReturnString ));
		
Error:

	RETURN_STDERR_POINTER;
}

/*********************************************** Get Revision *****************************************/

void* DLLEXPORT PowerSupply_GetRevision ( ViSession viInstrumentHandle , int channel , char szReturnString[256] ) 
{
	STD_ERROR             StdError					=	{0};
	
	CHK_ERR ( ppsmps_query_model_number (viInstrumentHandle, szReturnString ));
		
Error:

	RETURN_STDERR_POINTER;
}

/***************************************** Set Output Voltage ***********************************/

void* DLLEXPORT PowerSupply_SetOutputVoltage ( ViSession viInstrumentHandle , int channel , double lfVoltage )
{
	STD_ERROR            	StdError					=	{0};
	
	double 					lfGetVoltage				=	0.0;
		
	int						iTryIndex					=	0;
	
	switch(channel)
	{
		case 1:
				channel = PPSMPS_CHAN1;
				break;
				
		case 2:
				channel = PPSMPS_CHAN2;
				break;
				
		default:
				channel = PPSMPS_CHAN1;
				break;
	}
	
	for ( iTryIndex = 0; iTryIndex < 5; iTryIndex++ )
	{
		CHK_ERR ( ppsmps_set_voltage ( viInstrumentHandle , PPSMPS_DUAL, channel, lfVoltage, lfVoltage));
	
		CHK_ERR ( ppsmps_query_set_curr_volt ( viInstrumentHandle , PPSMPS_DUAL, channel, PPSMPS_RD_VOLTAGE, &lfGetVoltage ));                
	
		if ( lfGetVoltage == lfVoltage )
			break;
		
		DelayWithEventProcessing(0.2);
	}
	
	IF (( lfGetVoltage != lfVoltage ) , "Cann't set Voltage" );
		
Error:

	RETURN_STDERR_POINTER;
}

/*********************************** Get Output Voltage Defined ***********************************/

void* DLLEXPORT PowerSupply_GetOutputVoltageDefined ( ViSession viInstrumentHandle , int channel , double *plfVoltage )
{
	STD_ERROR             StdError					=	{0};
	
	switch(channel)
	{
		case 1:
				channel = PPSMPS_CHAN1;
				break;
				
		case 2:
				channel = PPSMPS_CHAN2;
				break;
				
		default:
				channel = PPSMPS_CHAN1;
				break;
	}
	
	CHK_ERR ( ppsmps_query_set_curr_volt ( viInstrumentHandle , PPSMPS_DUAL, channel, PPSMPS_RD_VOLTAGE, plfVoltage ));                
	
Error:

	RETURN_STDERR_POINTER;
}

/*********************************** Get Output Voltage Actual ***********************************/

void* DLLEXPORT PowerSupply_GetOutputVoltageActual ( ViSession viInstrumentHandle , int channel , double *plfVoltage )
{
	STD_ERROR             StdError					=	{0};
	
	switch(channel)
	{
		case 1:
				channel = PPSMPS_CHAN1;
				break;
				
		case 2:
				channel = PPSMPS_CHAN2;
				break;
				
		default:
				channel = PPSMPS_CHAN1;
				break;
	}
	
	CHK_ERR ( ppsmps_measure ( viInstrumentHandle , PPSMPS_DUAL, channel, PPSMPS_RD_VOLTAGE, plfVoltage ));                
		
Error:

	RETURN_STDERR_POINTER;
}

/***************************************** Set Output Current ***********************************/

void* DLLEXPORT PowerSupply_SetOutputCurrent ( ViSession viInstrumentHandle , int channel , double lfCurrent )
{
	STD_ERROR      			StdError					=	{0};
	
	double 					lfGetCurrent 				=	0.0;
	
	int						iTryIndex					=	0;
	
	switch(channel)
	{
		case 1:
				channel = PPSMPS_CHAN1;
				break;
				
		case 2:
				channel = PPSMPS_CHAN2;
				break;
				
		default:
				channel = PPSMPS_CHAN1;
				break;
	}
	
	for ( iTryIndex = 0; iTryIndex < 5; iTryIndex++ )
	{
		CHK_ERR ( ppsmps_set_current ( viInstrumentHandle , PPSMPS_DUAL, channel, lfCurrent, lfCurrent));
	
		CHK_ERR ( ppsmps_query_set_curr_volt ( viInstrumentHandle , PPSMPS_DUAL, channel, PPSMPS_RD_CURRENT, &lfGetCurrent ));                
	
		if ( lfGetCurrent == lfCurrent )
			break;
		
		DelayWithEventProcessing(0.2);
	}
	
	IF (( lfGetCurrent != lfCurrent ) , "Cann't set Current" );
	
	
Error:

	RETURN_STDERR_POINTER;
}

/*********************************** Get Output Current Defined ***********************************/

void* DLLEXPORT PowerSupply_GetOutputCurrentDefined ( ViSession viInstrumentHandle , int channel , double *plfCurrent )
{
	STD_ERROR             StdError					=	{0};
	
	switch(channel)
	{
		case 1:
				channel = PPSMPS_CHAN1;
				break;
				
		case 2:
				channel = PPSMPS_CHAN2;
				break;
				
		default:
				channel = PPSMPS_CHAN1;
				break;
	}
	
	CHK_ERR ( ppsmps_query_set_curr_volt ( viInstrumentHandle , PPSMPS_DUAL, channel, PPSMPS_RD_CURRENT, plfCurrent ));                
		
Error:

	RETURN_STDERR_POINTER;
}

/*********************************** Get Output Current Actual ***********************************/

void* DLLEXPORT PowerSupply_GetOutputCurrentActual ( ViSession viInstrumentHandle , int channel , double *plfCurrent )
{
	STD_ERROR             StdError					=	{0};
	
	switch(channel)
	{
		case 1:
				channel = PPSMPS_CHAN1;
				break;
				
		case 2:
				channel = PPSMPS_CHAN2;
				break;
				
		default:
				channel = PPSMPS_CHAN1;
				break;
	}
	
	CHK_ERR ( ppsmps_measure ( viInstrumentHandle , PPSMPS_DUAL, channel, PPSMPS_RD_CURRENT, plfCurrent ));                
		
Error:

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT PowerSupply_GetOutputPowerActual ( ViSession viInstrumentHandle , int channel , double *plfPower )
{
	STD_ERROR             StdError					=	{0};

	double				lfVoltage					=	0.0,
						lfCurrent					=	0.0;
	
	switch(channel)
	{
		case 1:
				channel = PPSMPS_CHAN1;
				break;
				
		case 2:
				channel = PPSMPS_CHAN2;
				break;
				
		default:
				channel = PPSMPS_CHAN1;
				break;
	}
	
	CHK_ERR ( ppsmps_measure ( viInstrumentHandle , PPSMPS_DUAL, channel, PPSMPS_RD_VOLTAGE, &lfVoltage ));                
	CHK_ERR ( ppsmps_measure ( viInstrumentHandle , PPSMPS_DUAL, channel, PPSMPS_RD_CURRENT, &lfCurrent ));                
	
	if ( plfPower )
		*plfPower = lfCurrent * lfVoltage;
Error:

	RETURN_STDERR_POINTER;
}

/***************************************** Set Voltage Protection Over ***********************************/

void* DLLEXPORT PowerSupply_SetVoltageProtectionOver ( ViSession viInstrumentHandle , int channel , double lfVoltage )
{
	STD_ERROR             StdError					=	{0};
	
	switch(channel)
	{
		case 1:
				channel = PPSMPS_CHAN1;
				break;
				
		case 2:
				channel = PPSMPS_CHAN2;
				break;
				
		default:
				channel = PPSMPS_CHAN1;
				break;
	}
	
	
	CHK_ERR ( ppsmps_set_ovp( viInstrumentHandle , PPSMPS_DUAL, channel, lfVoltage, lfVoltage)); 
	CHK_ERR ( ppsmps_set_ovp_enable (viInstrumentHandle, PPSMPS_DUAL, channel, 1));
		
Error:

	RETURN_STDERR_POINTER;
}

/***************************************** Get Voltage Protection Over ***********************************/

void* DLLEXPORT PowerSupply_GetVoltageProtectionOver ( ViSession viInstrumentHandle , int channel , double *plfVoltage )
{
	STD_ERROR             StdError					=	{0};

Error:

	RETURN_STDERR_POINTER;
}

/***************************************** Set Voltage Protection Under ***********************************/

void* DLLEXPORT PowerSupply_SetVoltageProtectionUnder ( ViSession viInstrumentHandle , int channel , double lfVoltage )
{
	STD_ERROR             StdError					=	{0};
	
//Error:	

	RETURN_STDERR_POINTER;
}

/***************************************** Get Voltage Protection Under ***********************************/

void* DLLEXPORT PowerSupply_GetVoltageProtectionUnder ( ViSession viInstrumentHandle , int channel , double *plfVoltage )
{
	STD_ERROR             StdError					=	{0};
	
//Error:	

	RETURN_STDERR_POINTER;
}

/*********************************************** Set Output State *****************************************/

void* DLLEXPORT PowerSupply_SetOutputState ( ViSession viInstrumentHandle , int channel , int iState )
{
	STD_ERROR            	StdError					=	{0};
	
	int						iTryIndex					=	0;
	
	double					lfGetCurrent				=	0;
	
	int						iGetState					=	0;
	
	switch(channel)
	{
		case 1:
				channel = PPSMPS_CHAN1;
				break;
				
		case 2:
				channel = PPSMPS_CHAN2;
				break;
				
		default:
				channel = PPSMPS_CHAN1;
				break;
	}
	
	for ( iTryIndex = 0; iTryIndex < 5; iTryIndex++ )
	{
		CHK_ERR ( ppsmps_set_ch_on_off (viInstrumentHandle, PPSMPS_DUAL, channel, iState));
	
		CHK_ERR ( ppsmps_query_set_curr_volt ( viInstrumentHandle , PPSMPS_DUAL, channel, PPSMPS_RD_CURRENT, &lfGetCurrent ));                
	
		if ( iState && ( lfGetCurrent > 0.0 ))
			break;
		
		if (( iState == 0 ) && ( lfGetCurrent == 0.0 ))
			break;
		
		DelayWithEventProcessing(0.2);
	}
	
Error:

	RETURN_STDERR_POINTER;	
}

/*********************************************** Get Output State *****************************************/

void* DLLEXPORT PowerSupply_GetOutputState ( ViSession viInstrumentHandle , int channel , int *piState )
{	
	STD_ERROR             StdError					=	{0};
	
Error:
	
	RETURN_STDERR_POINTER;	
}

/***************************************** Set Foldback Protection Mode ***********************************/

void* DLLEXPORT PowerSupply_SetFoldbackProtectionMode ( ViSession viInstrumentHandle , int channel , int iMode )
{
	STD_ERROR             StdError					=	{0};
	
	
//Error:	
	
	RETURN_STDERR_POINTER;
}

/***************************************** Get Foldback Protection Mode ***********************************/

void* DLLEXPORT PowerSupply_GetFoldbackProtectionMode ( ViSession viInstrumentHandle , int channel , int *piMode )
{
	STD_ERROR             StdError					=	{0};
	
	
//Error:	
	
	RETURN_STDERR_POINTER;
}

/***************************************** Set Auto Restart Mode ***********************************/

void* DLLEXPORT PowerSupply_SetAutoRestartMode ( ViSession viInstrumentHandle , int channel , int iMode )
{
	STD_ERROR             StdError					=	{0};
	
	
//Error:	
	
	RETURN_STDERR_POINTER;
}

/***************************************** Get Auto Restart Mode ***********************************/

void* DLLEXPORT PowerSupply_GetAutoRestartMode ( ViSession viInstrumentHandle , int channel , int *piMode )
{
	STD_ERROR             StdError					=	{0};
	
	
//Error:	
	
	RETURN_STDERR_POINTER;
}

/*********************************** Get Operational Status Register ***********************************/

void* DLLEXPORT PowerSupply_GetOperationalStatusRegister ( ViSession viInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR             StdError					=	{0};
	
	
//Error:	
	
	RETURN_STDERR_POINTER;
}

/*********************************** Get Alarm Status Register ***********************************/

void* DLLEXPORT PowerSupply_GetAlarmStatusRegister ( ViSession viInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR             StdError					=	{0};
	
	
//Error:	
	
	RETURN_STDERR_POINTER;
}

/*********************************** Get Error Codes Register ***********************************/

void* DLLEXPORT PowerSupply_GetErrorCodesRegister ( ViSession viInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR             StdError					=	{0};
	
	
//Error:	
	
	RETURN_STDERR_POINTER;
}

/*********************************** Get Complete Status ***********************************/

void* DLLEXPORT PowerSupply_GetCompleteStatus ( ViSession viInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR             StdError					=	{0};
	
	
//Error:	
	
	RETURN_STDERR_POINTER;
}


void main ( void )
{
	ViSession		viInstrumentHandle				=	0;
	
	int				InstrumentConnectStatus			=	0,
					InstrumentChanelsQuantity		=	0;
	
	int				iState							=	1;
	
	char			szReturnString[256]				=	{0};
	
	double			lfVoltage						=	0.0,
					lfCurrent						=	0.0;
	
	PowerSupply_Init ( 0 , "GPIB::1" , &viInstrumentHandle , &InstrumentConnectStatus , &InstrumentChanelsQuantity );
	
	PowerSupply_Reset ( viInstrumentHandle );   
	PowerSupply_GetModel ( viInstrumentHandle , 1 , szReturnString );
	PowerSupply_GetRevision ( viInstrumentHandle , 1 , szReturnString );    
	
	PowerSupply_SetOutputVoltage ( viInstrumentHandle , 1 , 15.0 );
	PowerSupply_SetOutputVoltage ( viInstrumentHandle , 2 , 5.0 ); 
	PowerSupply_SetOutputCurrent ( viInstrumentHandle , 1 , 1.0 );
	PowerSupply_SetOutputCurrent ( viInstrumentHandle , 2 , 2.0 );	
		
	PowerSupply_SetOutputState ( viInstrumentHandle , 1 , iState ); 
	PowerSupply_SetOutputState ( viInstrumentHandle , 2 , iState ); 

		
	PowerSupply_GetOutputVoltageDefined ( viInstrumentHandle , 1 , &lfVoltage );
	PowerSupply_GetOutputVoltageActual ( viInstrumentHandle , 1 , &lfVoltage );
	
	
	PowerSupply_GetOutputCurrentDefined ( viInstrumentHandle , 1 , &lfCurrent );
	PowerSupply_GetOutputCurrentActual ( viInstrumentHandle , 1 , &lfCurrent );
	
	//PowerSupply_GetOutputPowerActual ( viInstrumentHandle , 1 , double *plfPower )
	
	//PowerSupply_SetVoltageProtectionOver ( viInstrumentHandle , 1 , double lfVoltage )
	//PowerSupply_GetVoltageProtectionOver ( viInstrumentHandle , 1 , &lfVoltage );
	//PowerSupply_SetVoltageProtectionUnder ( viInstrumentHandle , 1 , double lfVoltage )
	//PowerSupply_GetVoltageProtectionUnder ( viInstrumentHandle , 1 , &lfVoltage );

	PowerSupply_GetOutputState ( viInstrumentHandle , 1 , &iState ); 
	PowerSupply_GetOutputState ( viInstrumentHandle , 2 , &iState ); 
	
	PowerSupply_Close ( &viInstrumentHandle ); 
	
	return;
}
