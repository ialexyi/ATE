#include <windows.h>
#include <userint.h>
#include <ansi_c.h>
#include <utility.h>
#include "GlobalDefines.h"
#include "XanM9B.h"
#include "toolbox.h"

//==============================================================================
// Include files

#include "Xantrex_XHR_wrapper.h"

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
	
	STD_ERR ( XanM9B_error_message	( viInstrumentHandle , iError , pErrorMessage ));
	
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
		status = viPrintf ( hConnectionHandle , "ID?\n" );
	
		if ( status == 0 )
		{
			viRead ( hConnectionHandle , szIdentificationText , (LOW_STRING-1) , &iCount );
			RemoveSurroundingWhiteSpace (szIdentificationText); 
		}
	}
		
	do
	{
		IF ((( strstr( szIdentificationText , "ID" ) == NULL ) && ( strstr( szIdentificationText , "Version" ) == NULL )), "Is not supported" );
	
		CALLOC_COPY_STRING( *pVendor , "Xantrex" );  
		
		pTemp = strrchr( szIdentificationText , ' ' );

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
	
		pTemp = strrchr( szIdentificationText , ' ' );

		if ( pTemp )
		{
			*pTemp = 0;
			pTemp++;
		}
		else
			break;
	
		pTemp = strchr( szIdentificationText , ' ' );

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
	
	char							vszSupportdedModels[][32]					=	{{"XHR7_5-80"},{"XHR20-30"},{"XHR33-18"},{"XHR40-15"},{"XHR60-10"},
										                                             {"XHR10-06"},{"XHR15-04"},{"XHR30-02"},{"XHR60-01"},
										                                             {"XHR7-5130"},{"XHR20-50"},{"XHR33-33"},{"XHR40-25"},{"XHR60-18"},
										                                             {"XHR100-10"},{"XHR15-07"},{"XHR30-03_5"},{"XHR60-01_7"},
										                                             {"XFR62-00"},{"XFR7_5-140"},{"XFR12-100"},{"XFR20-60"},{"XFR35-35"},
										                                             {"XFR40-30"},{"XFR45-30"},{"XFR60-20"},{"XFR100-12"},{"XFR15-08"},
										                                             {"XFR160-7_5"},{"XFR30-04"},{"XFR60-02"},
										                                             {"XFR7_5-300"},{"XFR12-220"},{"XFR20-130"},{"XFR33-85"},{"XFR40-70"},
										                                             {"XFR50-60"},{"XFR60-46"},{"XFR80-37"},{"XFR100-28"},{"XFR150-18"},
										                                             {"XFR30-09"},{"XFR42-07"},{"XFR60-04"},
										                                             {"XPD7_5-67"},{"XPD18-30"},{"XPD33-16"},{"XPD60-9"},{"XPD120-4_5"},
										                                             {"HPD15-20"},{"HPD30-10"},{"HPD6-05"},
										                                             {"XT7-6"},{"XT15-4"},{"XT20-3"},{"XT30-2"},{"XT60-1"},{"XT120-0_5"},{"XT250-0_25"}};
									
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
		if (( strstr( szIdentificationText , "ID" ) == NULL ) && ( strstr( szIdentificationText , "Version" ) == NULL ))
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
		STD_ERR ( XanM9B_init ( szRsrcAddress, VI_TRUE, VI_FALSE, &handle));
	}
	else
		handle = hParentInstrumentHandle;
		
	if ( viInstrumentHandle  ) 
			*viInstrumentHandle = handle;
	
	XanM9B_ClearStatus (handle);
	XanM9B_ClearError (handle);
	
	STD_ERR ( XanM9B_GetAttributeViInt32 (handle, NULL,XANM9B_ATTR_CHANNEL_COUNT,  &iChannelCount));

	if ( InstrumentChanelsQuantity )
		*InstrumentChanelsQuantity = iChannelCount;

Error:
	
	if ((StdError.error < 0) || (iChannelCount < 1))
	{ 
		XanM9B_error_message (handle, StdError.error, szErrorTextMessage);
		
		MessagePopup("Power Supply Initializing Error!",szErrorTextMessage);
		
		XanM9B_close (handle);
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

	STD_ERR ( XanM9B_close ( handle ));
		
	*phInstrumentHandle = 0;

Error:
	
	
	
	RETURN_STDERR_POINTER;
}

/*********************************************** Reset ***********************************************/

void* DLLEXPORT PowerSupply_Reset ( ViSession viInstrumentHandle )   
{   
	STD_ERROR                   StdError					=	{0};
	 
	
	STD_ERR ( XanM9B_reset ( viInstrumentHandle ));
	
Error: 
	
	RETURN_STDERR_POINTER; 
}


/*********************************** Wait For Operation Complete ***********************************************/

void* DLLEXPORT PowerSupply_WaitForOperationComplete ( ViSession viInstrumentHandle , double lfTimeout )   
{   
	STD_ERROR                   StdError					=	{0};
	
Error: 
	
	RETURN_STDERR_POINTER; 
}

/*********************************************** Get Model *****************************************/

void* DLLEXPORT PowerSupply_GetModel ( ViSession viInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR                   StdError					=	{0};
	
	STD_ERR ( XanM9B_GetAttributeViString (viInstrumentHandle, NULL , XANM9B_ATTR_INSTRUMENT_MODEL, 64 , pReturnString));
		
Error:
	
	RETURN_STDERR_POINTER;
}

/*********************************************** Get Revision *****************************************/

void* DLLEXPORT PowerSupply_GetRevision ( ViSession viInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR                   StdError					=	{0};
	
	STD_ERR ( XanM9B_GetAttributeViString (viInstrumentHandle, NULL, XANM9B_ATTR_INSTRUMENT_FIRMWARE_REVISION, 64 , pReturnString));

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
	
	//STD_ERR ( XanM9B_SystemWaitForOperationComplete ( viInstrumentHandle , 10000 )); 
	
	STD_ERR ( XanM9B_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	for ( iTryIndex = 0; iTryIndex < 5; iTryIndex++ )
	{
		STD_ERR ( XanM9B_ConfigureVoltageLevel (viInstrumentHandle, szChannelName, lfVoltage));
	
		//STD_ERR ( XanM9B_SystemWaitForOperationComplete ( viInstrumentHandle , 10000 )); 
	
		STD_ERR ( XanM9B_GetAttributeViReal64 (viInstrumentHandle, szChannelName, XANM9B_ATTR_VOLTAGE_LEVEL, &lfGetVoltage););

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
	
	STD_ERR ( XanM9B_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( XanM9B_GetAttributeViReal64 (viInstrumentHandle, szChannelName, XANM9B_ATTR_VOLTAGE_LEVEL, plfVoltage););
		
Error:
	
	RETURN_STDERR_POINTER;
}

/*********************************** Get Output Voltage Actual ***********************************/

void* DLLEXPORT PowerSupply_GetOutputVoltageActual ( ViSession viInstrumentHandle , int channel , double *plfVoltage )
{
	STD_ERROR                   StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( XanM9B_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( XanM9B_Measure (viInstrumentHandle, szChannelName, XANM9B_VAL_MEASURE_VOLTAGE, plfVoltage););
		
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
	
	//STD_ERR ( XanM9B_SystemWaitForOperationComplete ( viInstrumentHandle , 10000 )); 

	STD_ERR ( XanM9B_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	for ( iTryIndex = 0; iTryIndex < 5; iTryIndex++ )
	{
		STD_ERR ( XanM9B_ConfigureCurrentLimit (viInstrumentHandle, szChannelName, XANM9B_VAL_CURRENT_REGULATE, lfCurrent));
	
		//STD_ERR ( XanM9B_SystemWaitForOperationComplete ( viInstrumentHandle , 10000 )); 

		STD_ERR ( XanM9B_GetAttributeViReal64 (viInstrumentHandle, szChannelName, XANM9B_ATTR_CURRENT_LIMIT, &lfGetCurrent););
		
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
	
	STD_ERR ( XanM9B_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( XanM9B_GetAttributeViReal64 (viInstrumentHandle, szChannelName, XANM9B_ATTR_CURRENT_LIMIT, plfCurrent););
		
Error:
	
	RETURN_STDERR_POINTER;
}

/*********************************** Get Output Current Actual ***********************************/

void* DLLEXPORT PowerSupply_GetOutputCurrentActual ( ViSession viInstrumentHandle , int channel , double *plfCurrent )
{
	STD_ERROR                   StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( XanM9B_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( XanM9B_Measure (viInstrumentHandle, szChannelName, XANM9B_VAL_MEASURE_CURRENT, plfCurrent););
		
Error:
	
	
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT PowerSupply_GetOutputPowerActual ( ViSession viInstrumentHandle , int channel , double *plfPower )
{
	STD_ERROR                   StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	double		lfVoltage					=	0.0,
				lfCurrent					=	0.0;
	
	STD_ERR ( XanM9B_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( XanM9B_Measure (viInstrumentHandle, szChannelName, XANM9B_VAL_MEASURE_CURRENT, &lfCurrent););
	STD_ERR ( XanM9B_Measure (viInstrumentHandle, szChannelName, XANM9B_VAL_MEASURE_VOLTAGE, &lfVoltage);); 
	
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
	
	STD_ERR ( XanM9B_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( XanM9B_ConfigureOVP (viInstrumentHandle, szChannelName, VI_TRUE, lfVoltage));
		
Error:
	
	
	
	RETURN_STDERR_POINTER;
}

/***************************************** Get Voltage Protection Over ***********************************/

void* DLLEXPORT PowerSupply_GetVoltageProtectionOver ( ViSession viInstrumentHandle , int channel , double *plfVoltage )
{
	STD_ERROR                   StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( XanM9B_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( XanM9B_GetAttributeViReal64 (viInstrumentHandle, szChannelName, XANM9B_ATTR_OVP_LIMIT, plfVoltage));
		
Error:
	
	
	
	RETURN_STDERR_POINTER;
}

/***************************************** Set Current Protection Over ***********************************/

void* DLLEXPORT PowerSupply_SetCurrentProtectionOver ( ViSession viInstrumentHandle , int channel , double lfCurrent )
{
	STD_ERROR                   StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( XanM9B_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	//STD_ERR ( XanM9B_OutputProtectionConfigureOCP (viInstrumentHandle, szChannelName, VI_TRUE, lfCurrent));
		
Error:
	
	
	
	RETURN_STDERR_POINTER;
}

/***************************************** Get Current Protection Over ***********************************/

void* DLLEXPORT PowerSupply_GetCurrentProtectionOver ( ViSession viInstrumentHandle , int channel , double *plfCurrent )
{
	STD_ERROR                   StdError					=	{0};
	
	
	char		szChannelName[CH_NAME_LEN]	=	{0};
	
	STD_ERR ( XanM9B_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	//STD_ERR ( XanM9B_GetAttributeViReal64 (viInstrumentHandle, szChannelName, XANM9B_ATTR_OUTPUT_PROTECTION_OCP_LIMIT, plfCurrent));
		
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
	
	//STD_ERR ( XanM9B_SystemWaitForOperationComplete ( viInstrumentHandle , 10000 )); 
	
	STD_ERR ( XanM9B_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));

	for ( iTryIndex = 0; iTryIndex < 5; iTryIndex++ )
	{
		STD_ERR ( XanM9B_ConfigureOutputEnabled ( viInstrumentHandle , szChannelName , iState ));
	
		//STD_ERR ( XanM9B_SystemWaitForOperationComplete ( viInstrumentHandle , 10000 )); 

		STD_ERR ( XanM9B_GetAttributeViBoolean (viInstrumentHandle, szChannelName, XANM9B_ATTR_OUTPUT_ENABLED, (ViBoolean*)&iGetState));
	
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
	
	STD_ERR ( XanM9B_GetChannelName (viInstrumentHandle, channel, CH_NAME_LEN, szChannelName));
	
	STD_ERR ( XanM9B_GetAttributeViBoolean (viInstrumentHandle, szChannelName, XANM9B_ATTR_OUTPUT_ENABLED, (ViBoolean*)piState));
	
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
	
	PowerSupply_Init ( 0 , "GPIB0::27::INSTR" , &viInstrumentHandle , &InstrumentConnectStatus , &InstrumentChanelsQuantity );
	
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
