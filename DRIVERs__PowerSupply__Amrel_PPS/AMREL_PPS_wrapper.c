//==============================================================================
// Include files
#include <windows.h>
#include <userint.h>
#include <ansi_c.h>
#include <utility.h>
#include "GlobalDefines.h"
#include "toolbox.h"
#include "arpps.h"

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

void* DLLEXPORT PowerSupply_GetErrorTextMessage ( ViSession viInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR             StdError					=	{0};
	
	if ( pErrorMessage )
		arpps_error_message ( viInstrumentHandle , iError , pErrorMessage );
	
	if ( viInstrumentHandle )
	{
		CALLOC_ERR( StdError.pszErrorDescription , STD_STRING , sizeof(char));
		arpps_error_query	( viInstrumentHandle , &(StdError.error) , StdError.pszErrorDescription ); 
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
		status = viPrintf ( hConnectionHandle , "MODEL?\n" );
	
		if ( status == 0 )
		{
			viRead ( hConnectionHandle , szIdentificationText , (LOW_STRING-1) , &iCount );
			RemoveSurroundingWhiteSpace (szIdentificationText); 
		}
	}
		
	do
	{
		IF (( strstr( szIdentificationText , "PPS" ) == NULL ) , "Is not supported" );
	
		if ( pModel )
		{
			CALLOC_COPY_STRING( *pModel , szIdentificationText );  
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
	
	char							szIdentificationText[LOW_STRING]			=	{0};
	
	char							vszSupportdedModels[][32]					=	{{"PPS8-10"},{"PPS18-4"},{"PPS30-2.5"},{"PPS35-2"},{"PPS60-10"},{"PPS128-0.5"},{"PPS250-0.2"},{"PPS5-30"},{"PPS35-3R"},{"PPS60-1.5R"},{"PPS8-6D"},{"PPS18-4D"},{"PPS35-2D"},{"PPS30-3D"},{"PPS60-1D"},{"PPS128-0.5D"},{"PPS30-6"},{"PPS35-5"},{"PPS8-20"},{"PPS18-10"},{"PPS60-3"},{"PPS128-1.5"},{"PPS250-0.8"},{"PPS-1202"}};
									
	if (( pIdentificationText == NULL ) || ( strlen(pIdentificationText) < 5 ))
	{
		viOpenDefaultRM (&defaultRM);

		SetBreakOnLibraryErrors (0);
		
		status = viOpen ( defaultRM , pAddress , NULL, NULL, &hConnectionHandle );
		
		if ( status == 0 )
		{	
			status = viPrintf ( hConnectionHandle , "MODEL?\n" );
		
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
		if ( strstr( szIdentificationText , "PPS" ) == NULL ) 
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

void* DLLEXPORT PowerSupply_Init ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , ViSession *viInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChanelsQuantity )
{	
	STD_ERROR            	StdError					=	{0};
						
	char					szErrorTextMessage[256]		=	{0};
	
	ViSession				handle						=	0;
	
	if ( hParentInstrumentHandle == 0 )
	{
		CHK_ERR ( arpps_init ( szRsrcAddress , 1, 0, &handle ));
	}
	else
		handle = hParentInstrumentHandle;
		
	if ( viInstrumentHandle  ) 
		*viInstrumentHandle = handle;
	
	if ( InstrumentChanelsQuantity )
	{
		arpps_GetAttributeViInt32( handle , NULL , ARPPS_ATTR_CHANNEL_COUNT , InstrumentChanelsQuantity );
	}
	
Error:
	
	if (StdError.error < 0) 
	{ 
		arpps_error_message( handle , StdError.error , szErrorTextMessage );
		
		MessagePopup("Power Supply Initializing Error!",szErrorTextMessage);
		
		arpps_close (handle);
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

	CHK_ERR ( arpps_close ( handle ));
		
	*phInstrumentHandle = 0;

Error:
	
	
	
	RETURN_STDERR_POINTER;
}

/*********************************************** Reset ***********************************************/

void* DLLEXPORT PowerSupply_Reset ( ViSession viInstrumentHandle )   
{   
	STD_ERROR          		StdError					=	{0};
	 
	int						iNumberOfChannels			=	0,
							iChannelIndex				=	0;

	char					szChannelName[32]			=	{0};
	
	arpps_reset ( viInstrumentHandle );
	arpps_ResetWithDefaults ( viInstrumentHandle );

	arpps_GetAttributeViInt32( viInstrumentHandle , NULL , ARPPS_ATTR_CHANNEL_COUNT , &iNumberOfChannels );
		
	for ( iChannelIndex = 0; iChannelIndex < iNumberOfChannels; iChannelIndex++ )
	{
		arpps_GetChannelName ( viInstrumentHandle , (iChannelIndex+1) , 32 , szChannelName );
	
		arpps_ConfigureOutputEnabled ( viInstrumentHandle , szChannelName , VI_FALSE );
	}
	
Error: 
		 
	RETURN_STDERR_POINTER; 
}

/*********************************** Wait For Operation Complete ***********************************************/

void* DLLEXPORT PowerSupply_WaitForOperationComplete ( ViSession viInstrumentHandle , double lfTimeout )   
{   
	STD_ERROR                   StdError					=	{0};
	 
//Error: 
	
	RETURN_STDERR_POINTER; 
}

/*********************************************** Get Model *****************************************/

void* DLLEXPORT PowerSupply_GetModel ( ViSession viInstrumentHandle , int channel , char *pszReturnString )
{
	STD_ERROR             		StdError					=	{0};

	char						szReturnString[LOW_STRING]	=	{0};
	
	CHK_ERR ( arpps_GetAttributeViString (viInstrumentHandle, NULL , ARPPS_ATTR_INSTRUMENT_MODEL , LOW_STRING , szReturnString ));
	
	strcpy( pszReturnString , szReturnString );
	
Error:

	RETURN_STDERR_POINTER;
}

/*********************************************** Get Revision *****************************************/

void* DLLEXPORT PowerSupply_GetRevision ( ViSession viInstrumentHandle , int channel , char *pszReturnString ) 
{
	STD_ERROR             		StdError					=	{0};

	char						szReturnString[LOW_STRING]	=	{0};
	
	CHK_ERR ( arpps_GetAttributeViString (viInstrumentHandle, NULL , ARPPS_ATTR_INSTRUMENT_FIRMWARE_REVISION , LOW_STRING , szReturnString ));
	
	strcpy( pszReturnString , szReturnString );
	
Error:

	RETURN_STDERR_POINTER;
}

/***************************************** Set Output Voltage ***********************************/

void* DLLEXPORT PowerSupply_SetOutputVoltage ( ViSession viInstrumentHandle , int channel , double lfVoltage )
{
	STD_ERROR            	StdError					=	{0};
	
	double 					lfGetVoltage				=	0.0;
		
	int						iTryIndex					=	0;
	
	char					szChannelName[32]			=	{0};
	
	CHK_ERR ( arpps_GetChannelName ( viInstrumentHandle , channel , 32 , szChannelName ));
	
	for ( iTryIndex = 0; iTryIndex < 5; iTryIndex++ )
	{
		CHK_ERR ( arpps_ConfigureVoltageLevel ( viInstrumentHandle , szChannelName, lfVoltage));
	
		CHK_ERR ( arpps_GetAttributeViReal64 ( viInstrumentHandle , szChannelName , ARPPS_ATTR_VOLTAGE_LEVEL , &lfGetVoltage ));
		
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
	
	char					szChannelName[32]			=	{0};
	
	CHK_ERR ( arpps_GetChannelName ( viInstrumentHandle , channel , 32 , szChannelName ));
	
	CHK_ERR ( arpps_GetAttributeViReal64 ( viInstrumentHandle , szChannelName , ARPPS_ATTR_VOLTAGE_LEVEL , plfVoltage ));
	
Error:

	RETURN_STDERR_POINTER;
}

/*********************************** Get Output Voltage Actual ***********************************/

void* DLLEXPORT PowerSupply_GetOutputVoltageActual ( ViSession viInstrumentHandle , int channel , double *plfVoltage )
{
	STD_ERROR             StdError					=	{0};
	
	char					szChannelName[32]			=	{0};
	
	CHK_ERR ( arpps_GetChannelName ( viInstrumentHandle , channel , 32 , szChannelName ));
	
	CHK_ERR ( arpps_Measure ( viInstrumentHandle , szChannelName , ARPPS_VAL_MEASURE_VOLTAGE, plfVoltage ));
		
Error:

	RETURN_STDERR_POINTER;
}

/***************************************** Set Output Current ***********************************/

void* DLLEXPORT PowerSupply_SetOutputCurrent ( ViSession viInstrumentHandle , int channel , double lfCurrent )
{
	STD_ERROR      			StdError					=	{0};
	
	double 					lfGetCurrent 				=	0.0;
	
	int						iTryIndex					=	0;
	
	char					szChannelName[32]			=	{0};
	
	CHK_ERR ( arpps_GetChannelName ( viInstrumentHandle , channel , 32 , szChannelName ));
	
	for ( iTryIndex = 0; iTryIndex < 5; iTryIndex++ )
	{
		CHK_ERR ( arpps_ConfigureCurrentLimit (viInstrumentHandle , szChannelName , ARPPS_VAL_CURRENT_REGULATE, lfCurrent ));

		CHK_ERR ( arpps_GetAttributeViReal64 ( viInstrumentHandle , szChannelName , ARPPS_ATTR_CURRENT_LIMIT , &lfGetCurrent ));
	
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
	
	char					szChannelName[32]			=	{0};
	
	CHK_ERR ( arpps_GetChannelName ( viInstrumentHandle , channel , 32 , szChannelName ));
	
	CHK_ERR ( arpps_GetAttributeViReal64 ( viInstrumentHandle , szChannelName , ARPPS_ATTR_CURRENT_LIMIT , plfCurrent ));
		
Error:

	RETURN_STDERR_POINTER;
}

/*********************************** Get Output Current Actual ***********************************/

void* DLLEXPORT PowerSupply_GetOutputCurrentActual ( ViSession viInstrumentHandle , int channel , double *plfCurrent )
{
	STD_ERROR             StdError					=	{0};
	
	char					szChannelName[32]			=	{0};
	
	CHK_ERR ( arpps_GetChannelName ( viInstrumentHandle , channel , 32 , szChannelName ));
	
	CHK_ERR ( arpps_Measure ( viInstrumentHandle , szChannelName , ARPPS_VAL_MEASURE_CURRENT, plfCurrent ));
		
Error:

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT PowerSupply_GetOutputPowerActual ( ViSession viInstrumentHandle , int channel , double *plfPower )
{
	STD_ERROR             StdError					=	{0};

	double				lfVoltage					=	0.0,
						lfCurrent					=	0.0;
	
	char					szChannelName[32]			=	{0};
	
	CHK_ERR ( arpps_GetChannelName ( viInstrumentHandle , channel , 32 , szChannelName ));
	
	CHK_ERR ( arpps_Measure ( viInstrumentHandle , szChannelName , ARPPS_VAL_MEASURE_VOLTAGE, &lfVoltage ));                
	CHK_ERR ( arpps_Measure ( viInstrumentHandle , szChannelName , ARPPS_VAL_MEASURE_CURRENT, &lfCurrent ));                
	
	if ( plfPower )
		*plfPower = lfCurrent * lfVoltage;
Error:

	RETURN_STDERR_POINTER;
}

/***************************************** Set Voltage Protection Over ***********************************/

void* DLLEXPORT PowerSupply_SetVoltageProtectionOver ( ViSession viInstrumentHandle , int channel , double lfVoltage )
{
	STD_ERROR             StdError					=	{0};
	
	char					szChannelName[32]			=	{0};
	
	CHK_ERR ( arpps_GetChannelName ( viInstrumentHandle , channel , 32 , szChannelName ));
	
	CHK_ERR ( arpps_ConfigureOVP ( viInstrumentHandle , szChannelName , VI_TRUE, lfVoltage ));
			  
Error:

	RETURN_STDERR_POINTER;
}

/***************************************** Get Voltage Protection Over ***********************************/

void* DLLEXPORT PowerSupply_GetVoltageProtectionOver ( ViSession viInstrumentHandle , int channel , double *plfVoltage )
{
	STD_ERROR             StdError					=	{0};

	char					szChannelName[32]			=	{0};
	
	CHK_ERR ( arpps_GetChannelName ( viInstrumentHandle , channel , 32 , szChannelName ));
	
	CHK_ERR ( arpps_GetAttributeViReal64 ( viInstrumentHandle , szChannelName , ARPPS_ATTR_OVP_LIMIT , plfVoltage ));
	
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
	
	int						iGetState					=	0;
	
	char					szChannelName[32]			=	{0};
	
	CHK_ERR ( arpps_GetChannelName ( viInstrumentHandle , channel , 32 , szChannelName ));
	
	for ( iTryIndex = 0; iTryIndex < 5; iTryIndex++ )
	{
		CHK_ERR ( arpps_ConfigureOutputEnabled (viInstrumentHandle, szChannelName , iState));
	
		CHK_ERR ( arpps_GetAttributeViBoolean ( viInstrumentHandle , szChannelName , ARPPS_ATTR_OUTPUT_ENABLED , &iGetState ));
	
		if ( iGetState == iState )
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

	char					szChannelName[32]			=	{0};
	
	CHK_ERR ( arpps_GetChannelName ( viInstrumentHandle , channel , 32 , szChannelName ));
	
	CHK_ERR ( arpps_GetAttributeViBoolean ( viInstrumentHandle , szChannelName , ARPPS_ATTR_OUTPUT_ENABLED , piState ));
	
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
	
	PowerSupply_Init ( 0 , "GPIB::4" , &viInstrumentHandle , &InstrumentConnectStatus , &InstrumentChanelsQuantity );
	
	PowerSupply_Reset ( viInstrumentHandle );   
	PowerSupply_GetModel ( viInstrumentHandle , 1 , szReturnString );
	PowerSupply_GetRevision ( viInstrumentHandle , 1 , szReturnString );    
	
	PowerSupply_SetOutputVoltage ( viInstrumentHandle , 1 , 3.3 );
	PowerSupply_SetOutputVoltage ( viInstrumentHandle , 2 , 5.6 ); 
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
