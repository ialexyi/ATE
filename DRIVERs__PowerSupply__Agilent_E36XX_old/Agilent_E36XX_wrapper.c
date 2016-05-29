#include <windows.h>
#include <userint.h>
#include <ansi_c.h>
#include <utility.h>
#include "AgE36xx.h"
#include "GlobalDefines.h"
#include <formatio.h>
#include <visa.h>
#include "toolbox.h"

//==============================================================================
// Include files

#include "Agilent_E36XX_wrapper.h"

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
	STD_ERROR                   StdError					=	{0};
	
	STD_ERR ( AgE36xx_error_message	( viInstrumentHandle , iError , pErrorMessage ));
	
Error:	
	
	RETURN_STDERR_POINTER;
}

void*	DLLEXPORT		Equipment_Info ( int hHandle , char *pAddress , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware )
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

	if ( hHandle == 0 )
	{
		viOpenDefaultRM (&defaultRM);

		SetBreakOnLibraryErrors (0);
	
		status = viOpen ( defaultRM , pAddress , NULL, NULL, &hConnectionHandle );
	}
	else
	{
		hConnectionHandle = hHandle;	
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
		
		IF ((( strstr( szIdentificationUpperText , "AGILENT TECHNOLOGIES" ) == NULL ) && ( strstr( szIdentificationUpperText , "HEWLETT-PACKARD" ) == NULL )) , "Is not supported" );
	
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

void*	DLLEXPORT		Equipment_IsSupported ( char *pAddress , char *pID_String , char *pIdentificationText , int *pbSupporting )
{
	STD_ERROR						StdError									=	{0};
	
	int								iCount										=	0, 
									status										=	0, 
									iIndex										=	0, 
									bSupport									=	0, 
									defaultRM									=	0,
									hConnectionHandle							=	0;
	
	char							szIdentificationText[LOW_STRING]			=	{0},
									szIdentificationUpperText[LOW_STRING]		=	{0}; 
	
	char							vszSupportdedModels[][32]					=	{{"E3632A"},{"E3633A"},{"E3634A"},{"E3640A"},{"E3641A"},{"E3642A"},{"E3643A"},{"E3644A"},{"E3631A"},{"E3645A"},{"E3646A"},{"E3647A"},{"E3648A"},{"E3649A"}};
									
	if (( pIdentificationText == NULL ) || ( strlen(pIdentificationText) < 10 ))
	{
		viOpenDefaultRM (&defaultRM);

		SetBreakOnLibraryErrors (0);
		
		status = viOpen ( defaultRM , pAddress , NULL, NULL, &hConnectionHandle );
		
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
		
		if (( strstr( szIdentificationUpperText , "AGILENT TECHNOLOGIES" ) == NULL ) && ( strstr( szIdentificationUpperText , "HEWLETT-PACKARD" ) == NULL )) 
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
	STD_ERROR                   StdError					=	{0};
	
						
	int			iChannelCount				=	0;
	
	char		szErrorTextMessage[256]		=	{0};
	
	ViSession	handle						=	0;
	
	if ( hParentInstrumentHandle == 0 )
	{
		STD_ERR ( AgE36xx_init ( szRsrcAddress, VI_TRUE, VI_FALSE, &handle));
	}
	else
		handle = hParentInstrumentHandle;
		
	if ( viInstrumentHandle  ) 
			*viInstrumentHandle = handle;
	
	AgE36xx_StatusClear (handle);
	AgE36xx_ClearInterchangeWarnings (handle);
	
	STD_ERR ( AgE36xx_GetAttributeViInt32 (handle, NULL,AGE36XX_ATTR_CHANNEL_COUNT,  &iChannelCount));

	if ( InstrumentChanelsQuantity )
		*InstrumentChanelsQuantity = iChannelCount;

Error:
	
	if ((StdError.error < 0) || (iChannelCount < 1))
	{ 
		AgE36xx_error_message (handle, StdError.error, szErrorTextMessage);
		
		MessagePopup("Power Supply Initializing Error!",szErrorTextMessage);
		
		AgE36xx_close (handle);
	}
	else
		if ( InstrumentConnectStatus )
			*InstrumentConnectStatus = 1;	
	
	RETURN_STDERR_POINTER;
}

/*********************************************** Close ***********************************************/

void* DLLEXPORT PowerSupply_Close ( ViSession *phInstrumentHandle )
{
	STD_ERROR                   StdError					=	{0};
	  
	
	ViSession	handle						=	0;

	IF ( (phInstrumentHandle == NULL) , "Pointer passed NULL.");
	
	handle = *phInstrumentHandle;
	
	IF ( (handle == 0) , "Pointer passed 0.");

	STD_ERR ( AgE36xx_close ( handle ));
		
	*phInstrumentHandle = 0;

Error:
	
	
	
	RETURN_STDERR_POINTER;
}

/*********************************************** Reset ***********************************************/

void* DLLEXPORT PowerSupply_Reset ( ViSession viInstrumentHandle )   
{   
	STD_ERROR                   StdError					=	{0};
	 
	
	STD_ERR ( AgE36xx_reset ( viInstrumentHandle ));
	
Error: 
	
	RETURN_STDERR_POINTER; 
}


/*********************************** Wait For Operation Complete ***********************************************/

void* DLLEXPORT PowerSupply_WaitForOperationComplete ( ViSession viInstrumentHandle , double lfTimeout )   
{   
	STD_ERROR                   StdError					=	{0};
	
	STD_ERR ( AgE36xx_SystemWaitForOperationComplete ( viInstrumentHandle , (ViInt32)(lfTimeout*1E3) )); 
	
Error: 
	
	RETURN_STDERR_POINTER; 
}

/*********************************************** Get Model *****************************************/

void* DLLEXPORT PowerSupply_GetModel ( ViSession viInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR                   StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( AgE36xx_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( AgE36xx_GetAttributeViString (viInstrumentHandle, szChannelName, AGE36XX_ATTR_INSTRUMENT_MODEL, sizeof(pReturnString), pReturnString));
		
Error:
	
	
	
	RETURN_STDERR_POINTER;
}

/*********************************************** Get Revision *****************************************/

void* DLLEXPORT PowerSupply_GetRevision ( ViSession viInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR                   StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( AgE36xx_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( AgE36xx_GetAttributeViString (viInstrumentHandle, szChannelName, AGE36XX_ATTR_INSTRUMENT_FIRMWARE_REVISION, sizeof(pReturnString), pReturnString));
		
Error:
	
	
	
	RETURN_STDERR_POINTER;
}

/***************************************** Set Output Voltage ***********************************/
void* DLLEXPORT PowerSupply_SetOutputVoltage ( ViSession viInstrumentHandle , int channel , double lfVoltage )
{
	STD_ERROR            	StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	double 					lfGetVoltage				=	0.0;
		
	int						iTryIndex					=	0;
	
	STD_ERR ( AgE36xx_SystemWaitForOperationComplete ( viInstrumentHandle , 10000 )); 
	
	STD_ERR ( AgE36xx_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	for ( iTryIndex = 0; iTryIndex < 5; iTryIndex++ )
	{
		STD_ERR ( AgE36xx_ConfigureOutputRange (viInstrumentHandle, szChannelName, AGE36XX_VAL_RANGE_VOLTAGE, lfVoltage));
	
		STD_ERR ( AgE36xx_SystemWaitForOperationComplete ( viInstrumentHandle , 10000 )); 
	
		STD_ERR ( AgE36xx_GetAttributeViReal64 (viInstrumentHandle, szChannelName, AGE36XX_ATTR_VOLTAGE_LEVEL, &lfGetVoltage););

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
	STD_ERROR                   StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( AgE36xx_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( AgE36xx_GetAttributeViReal64 (viInstrumentHandle, szChannelName, AGE36XX_ATTR_VOLTAGE_LEVEL, plfVoltage););
		
Error:
	
	RETURN_STDERR_POINTER;
}

/*********************************** Get Output Voltage Actual ***********************************/

void* DLLEXPORT PowerSupply_GetOutputVoltageActual ( ViSession viInstrumentHandle , int channel , double *plfVoltage )
{
	STD_ERROR                   StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( AgE36xx_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( AgE36xx_Measure (viInstrumentHandle, szChannelName, AGE36XX_VAL_MEASURE_VOLTAGE, plfVoltage););
		
Error:
	
	RETURN_STDERR_POINTER;
}

/***************************************** Set Output Current ***********************************/

void* DLLEXPORT PowerSupply_SetOutputCurrent ( ViSession viInstrumentHandle , int channel , double lfCurrent )
{
	STD_ERROR      			StdError					=	{0};
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	double 					lfGetCurrent 				=	0.0;
	
	int						iTryIndex					=	0;
	
	STD_ERR ( AgE36xx_SystemWaitForOperationComplete ( viInstrumentHandle , 10000 )); 

	STD_ERR ( AgE36xx_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	for ( iTryIndex = 0; iTryIndex < 5; iTryIndex++ )
	{
		STD_ERR ( AgE36xx_ConfigureOutputRange (viInstrumentHandle, szChannelName, AGE36XX_VAL_RANGE_CURRENT, lfCurrent));
	
		STD_ERR ( AgE36xx_SystemWaitForOperationComplete ( viInstrumentHandle , 10000 )); 

		STD_ERR ( AgE36xx_GetAttributeViReal64 (viInstrumentHandle, szChannelName, AGE36XX_ATTR_CURRENT_LIMIT, &lfGetCurrent););
		
		if ( lfGetCurrent == lfCurrent )
			break;
		
		DelayWithEventProcessing(0.2);
	}
	
	IF (( lfGetCurrent != lfCurrent ) , "Cann't set Voltage" );
	
Error:

	RETURN_STDERR_POINTER;
}

/*********************************** Get Output Current Defined ***********************************/

void* DLLEXPORT PowerSupply_GetOutputCurrentDefined ( ViSession viInstrumentHandle , int channel , double *plfCurrent )
{
	STD_ERROR                   StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( AgE36xx_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( AgE36xx_GetAttributeViReal64 (viInstrumentHandle, szChannelName, AGE36XX_ATTR_CURRENT_LIMIT, plfCurrent););
		
Error:
	
	RETURN_STDERR_POINTER;
}

/*********************************** Get Output Current Actual ***********************************/

void* DLLEXPORT PowerSupply_GetOutputCurrentActual ( ViSession viInstrumentHandle , int channel , double *plfCurrent )
{
	STD_ERROR                   StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( AgE36xx_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( AgE36xx_Measure (viInstrumentHandle, szChannelName, AGE36XX_VAL_MEASURE_CURRENT, plfCurrent););
		
Error:
	
	
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT PowerSupply_GetOutputPowerActual ( ViSession viInstrumentHandle , int channel , double *plfPower )
{
	STD_ERROR                   StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	double		lfVoltage					=	0.0,
				lfCurrent					=	0.0;
	
	STD_ERR ( AgE36xx_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( AgE36xx_Measure (viInstrumentHandle, szChannelName, AGE36XX_VAL_MEASURE_CURRENT, &lfCurrent););
	STD_ERR ( AgE36xx_Measure (viInstrumentHandle, szChannelName, AGE36XX_VAL_MEASURE_VOLTAGE, &lfVoltage);); 
	
	if ( plfPower )
		*plfPower = lfCurrent * lfVoltage;
Error:
	
	
	
	RETURN_STDERR_POINTER;
}

/***************************************** Set Voltage Protection Over ***********************************/

void* DLLEXPORT PowerSupply_SetVoltageProtectionOver ( ViSession viInstrumentHandle , int channel , double lfVoltage )
{
	STD_ERROR                   StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( AgE36xx_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( AgE36xx_OutputProtectionConfigureOVP (viInstrumentHandle, szChannelName, VI_TRUE, lfVoltage));
		
Error:
	
	
	
	RETURN_STDERR_POINTER;
}

/***************************************** Get Voltage Protection Over ***********************************/

void* DLLEXPORT PowerSupply_GetVoltageProtectionOver ( ViSession viInstrumentHandle , int channel , double *plfVoltage )
{
	STD_ERROR                   StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( AgE36xx_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( AgE36xx_GetAttributeViReal64 (viInstrumentHandle, szChannelName, AGE36XX_ATTR_OUTPUT_PROTECTION_OVP_LIMIT, plfVoltage));
		
Error:
	
	
	
	RETURN_STDERR_POINTER;
}

/***************************************** Set Current Protection Over ***********************************/

void* DLLEXPORT PowerSupply_SetCurrentProtectionOver ( ViSession viInstrumentHandle , int channel , double lfCurrent )
{
	STD_ERROR                   StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( AgE36xx_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( AgE36xx_OutputProtectionConfigureOCP (viInstrumentHandle, szChannelName, VI_TRUE, lfCurrent));
		
Error:
	
	
	
	RETURN_STDERR_POINTER;
}

/***************************************** Get Current Protection Over ***********************************/

void* DLLEXPORT PowerSupply_GetCurrentProtectionOver ( ViSession viInstrumentHandle , int channel , double *plfCurrent )
{
	STD_ERROR                   StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( AgE36xx_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( AgE36xx_GetAttributeViReal64 (viInstrumentHandle, szChannelName, AGE36XX_ATTR_OUTPUT_PROTECTION_OCP_LIMIT, plfCurrent));
		
Error:
	
	
	
	RETURN_STDERR_POINTER;
}

/***************************************** Set Voltage Protection Under ***********************************/

void* DLLEXPORT PowerSupply_SetVoltageProtectionUnder ( ViSession viInstrumentHandle , int channel , double lfVoltage )
{
	STD_ERROR                   StdError					=	{0};
	
//Error:
	
	RETURN_STDERR_POINTER;
}

/***************************************** Get Voltage Protection Under ***********************************/

void* DLLEXPORT PowerSupply_GetVoltageProtectionUnder ( ViSession viInstrumentHandle , int channel , double *plfVoltage )
{
	STD_ERROR                   StdError					=	{0};
	
//Error:	
	
	RETURN_STDERR_POINTER;
}

/*********************************************** Set Output State *****************************************/

void* DLLEXPORT PowerSupply_SetOutputState ( ViSession viInstrumentHandle , int channel , int iState )
{
	STD_ERROR            	StdError					=	{0};
	
	int						iTryIndex					=	0;
	
	char					szChannelName[CH_NAME_LEN]	=	{0};
	
	int						iGetState					=	0;
	
	STD_ERR ( AgE36xx_SystemWaitForOperationComplete ( viInstrumentHandle , 10000 )); 
	
	STD_ERR ( AgE36xx_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));

	for ( iTryIndex = 0; iTryIndex < 5; iTryIndex++ )
	{
		STD_ERR ( AgE36xx_ConfigureOutputEnabled ( viInstrumentHandle , szChannelName , iState ));
	
		STD_ERR ( AgE36xx_SystemWaitForOperationComplete ( viInstrumentHandle , 10000 )); 

		STD_ERR ( AgE36xx_GetAttributeViBoolean (viInstrumentHandle, szChannelName, AGE36XX_ATTR_OUTPUT_ENABLED, (ViBoolean*)&iGetState));
	
		if ( iGetState == iState )
			break;
		
		DelayWithEventProcessing(0.2);
	}
	
	IF (( iGetState != iState ) , "Cann't set State" );
	
Error:

	RETURN_STDERR_POINTER;	
}


/*********************************************** Get Output State *****************************************/

void* DLLEXPORT PowerSupply_GetOutputState ( ViSession viInstrumentHandle , int channel , int *piState )
{	
	STD_ERROR                   StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( AgE36xx_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( AgE36xx_GetAttributeViBoolean (viInstrumentHandle, szChannelName, AGE36XX_ATTR_OUTPUT_ENABLED, (ViBoolean*)piState));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

/***************************************** Set Foldback Protection Mode ***********************************/

void* DLLEXPORT PowerSupply_SetFoldbackProtectionMode ( ViSession viInstrumentHandle , int channel , int iMode )
{
	STD_ERROR                   StdError					=	{0};
//Error:	

	RETURN_STDERR_POINTER;
}

/***************************************** Get Foldback Protection Mode ***********************************/

void* DLLEXPORT PowerSupply_GetFoldbackProtectionMode ( ViSession viInstrumentHandle , int channel , int *piMode )
{
	STD_ERROR                   StdError					=	{0};
	
//Error:	

	RETURN_STDERR_POINTER;
}

/***************************************** Set Auto Restart Mode ***********************************/

void* DLLEXPORT PowerSupply_SetAutoRestartMode ( ViSession viInstrumentHandle , int channel , int iMode )
{
	STD_ERROR                   StdError					=	{0};
	
//Error:	
	
	RETURN_STDERR_POINTER;
}

/***************************************** Get Auto Restart Mode ***********************************/

void* DLLEXPORT PowerSupply_GetAutoRestartMode ( ViSession viInstrumentHandle , int channel , int *piMode )
{
	STD_ERROR                   StdError					=	{0};
	
//Error:	
	
	RETURN_STDERR_POINTER;
}


/*********************************** Get Operational Status Register ***********************************/

void* DLLEXPORT PowerSupply_GetOperationalStatusRegister ( ViSession viInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR                   StdError					=	{0};
	
//Error:	
	
	RETURN_STDERR_POINTER;
}


/*********************************** Get Alarm Status Register ***********************************/

void* DLLEXPORT PowerSupply_GetAlarmStatusRegister ( ViSession viInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR                   StdError					=	{0};
	
//Error:	
	
	RETURN_STDERR_POINTER;
}


/*********************************** Get Error Codes Register ***********************************/

void* DLLEXPORT PowerSupply_GetErrorCodesRegister ( ViSession viInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR                   StdError					=	{0};
	
//Error:	
	
	RETURN_STDERR_POINTER;
}


/*********************************** Get Complete Status ***********************************/

void* DLLEXPORT PowerSupply_GetCompleteStatus ( ViSession viInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR                   StdError					=	{0};
	
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
	
	PowerSupply_Init ( 0 , "GPIB0::5::INSTR" , &viInstrumentHandle , &InstrumentConnectStatus , &InstrumentChanelsQuantity );
	
	PowerSupply_Reset ( viInstrumentHandle );   
	PowerSupply_GetModel ( viInstrumentHandle , 1 , szReturnString );
	PowerSupply_GetRevision ( viInstrumentHandle , 1 , szReturnString );    
	
	PowerSupply_SetOutputVoltage ( viInstrumentHandle , 1 , 5.6 );
	PowerSupply_SetOutputCurrent ( viInstrumentHandle , 1 , 1.5 );
	
	PowerSupply_SetOutputVoltage ( viInstrumentHandle , 2 , 3.3 );
	PowerSupply_SetOutputCurrent ( viInstrumentHandle , 2 , 1.0 );
	
	PowerSupply_SetOutputState ( viInstrumentHandle , 2 , 1 ); 
	PowerSupply_SetOutputState ( viInstrumentHandle , 1 , 1 ); 
		
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
	
	PowerSupply_Close ( &viInstrumentHandle ); 
	
	return;
}
