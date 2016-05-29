//==============================================================================
// Include files
#include <windows.h>
#include <userint.h>
#include <ansi_c.h>
#include <utility.h>
#include "GlobalDefines.h"
#include "CPX400.h"
#include <Ivi.h>
#include "toolbox.h"
#include <visa.h>

//==============================================================================
// Constants

//==============================================================================
// Types


typedef struct
{
	int					vHandles[1024];
	
	int					count;
	
} tsHandleList;

//==============================================================================
// Static global variables
int		volatile		gHandleListHandle									=	0;
//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions
int		WaitForOperationCompletion( int sessionHandle , double lfTimeout , double lfLowLevelTimeout );

void* DLLEXPORT PowerSupply_Close ( ViSession *phInstrumentHandle );

//==============================================================================
// DLL main entry-point functions

int __stdcall DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	tsHandleList			*ptHandleList				=	NULL;
	
	int						index						=	0;

	int						vHandles[1024]				=	{0};
	
	int						count						=	0;
	
    switch (fdwReason) 
	{
        case DLL_PROCESS_ATTACH:
			
            if (InitCVIRTE (hinstDLL, 0, 0) == 0)
                return 0;     /* out of memory */
			
			CmtNewTSV ( sizeof(tsHandleList) , &gHandleListHandle );

			break;
			
        case DLL_PROCESS_DETACH:
            
			if ( CmtGetTSVPtr ( gHandleListHandle , &ptHandleList ) == 0 )
			{
				count = ptHandleList->count;
				
				memcpy( vHandles , ptHandleList->vHandles , sizeof(vHandles));
				
				CmtReleaseTSVPtr ( gHandleListHandle ); 
			}
			
			for ( index = 0; index < count; index++ )
			{
				if ( vHandles[index] )
					PowerSupply_Close ( &(vHandles[index]) );
			}
			
			CmtDiscardTSV (gHandleListHandle);
			
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
	
	
	STD_ERR ( CPX400_error_message	( viInstrumentHandle , iError , pErrorMessage ));
	
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
		IF (( strstr( szIdentificationText , "THURLBY THANDAR" ) == NULL ) , "Is not supported" );
	
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
			CALLOC_COPY_STRING( *pVendor , "TTI" );  
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
									vszSupportdedModels[][32]					=	{{"CPX400DP"},{"CPX400SP"},{"XPF6020DP"},{"XPF6020SP"}};
									
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
		if ( strstr( szIdentificationText , "THURLBY THANDAR" ) == NULL ) 
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
	STD_ERROR           			StdError										=	{0};
	
	int								iChannelCount									=	0;
	
	char							szErrorTextMessage[256]							=	{0};
	
	ViSession						handle											=	0;
	
	int								hVisaOpenLockHandle								=	0;
	
	int								bGetVisaLock									=	0;

	int								iHandleIndex									=	0;
	
	tsHandleList					*ptHandleList									=	NULL;
	
	int								index											=	0;
	
	int								bFound											=	0;
	
	if ( hParentInstrumentHandle == 0 )
	{
		CHK_CMT( CmtNewLock ( VISA_OPEN_LOCK , OPT_TL_PROCESS_EVENTS_WHILE_WAITING , &hVisaOpenLockHandle ));   

		if ( hVisaOpenLockHandle )
		{
			CHK_CMT( CmtGetLock ( hVisaOpenLockHandle ));
			bGetVisaLock = 1;
		}
		
		STD_ERR ( CPX400_init ( szRsrcAddress, VI_TRUE, VI_FALSE, &handle));
	}
	else
		handle = hParentInstrumentHandle;
		
	if ( viInstrumentHandle  ) 
			*viInstrumentHandle = handle;
	
	if ( CmtGetTSVPtr ( gHandleListHandle , &ptHandleList ) == 0 )
	{
		for ( index = 0; index < ptHandleList->count; index++ )
			if ( ptHandleList->vHandles[index] == handle )
				bFound = 1;	
		
		if ( bFound == 0 )
		{
			iHandleIndex = ptHandleList->count++;
			
			ptHandleList->vHandles[iHandleIndex] = handle;
		}
		
		CmtReleaseTSVPtr ( gHandleListHandle ); 
	}
	
	STD_ERR ( CPX400_ClearStatus (handle));
	
	STD_ERR ( CPX400_ClearInterchangeWarnings (handle));
	
	STD_ERR ( Ivi_GetAttributeViInt32 (handle, VI_NULL, IVI_ATTR_CHANNEL_COUNT, 0, &iChannelCount));

	if ( InstrumentChanelsQuantity )
		*InstrumentChanelsQuantity = iChannelCount;

Error:
	
	if ( hVisaOpenLockHandle && bGetVisaLock ) 
	{
		CmtReleaseLock (hVisaOpenLockHandle);
		CmtDiscardLock (hVisaOpenLockHandle); 
	}
	
	if ((StdError.error < 0) || (iChannelCount < 1))
	{ 
		CPX400_error_message (handle, StdError.error, szErrorTextMessage);
		
		MessagePopup("Power Supply Initializing Error!",szErrorTextMessage);
		
		CPX400_close (handle);
	}
	else
		if ( InstrumentConnectStatus )
			*InstrumentConnectStatus = 1;	
	
	RETURN_STDERR_POINTER;
}

/*********************************************** Close ***********************************************/

void* DLLEXPORT PowerSupply_Close ( ViSession *phInstrumentHandle )
{
	STD_ERROR            			StdError										=	{0};
	
	ViSession						handle											=	0;

	tsHandleList					*ptHandleList									=	NULL;

	int								index											=	0;
		
	IF ( (phInstrumentHandle == NULL) , "Pointer passed NULL.");
	
	handle = *phInstrumentHandle;
	
	IF ( (handle == 0) , "Pointer passed 0.");

	STD_ERR ( CPX400_close ( handle ));
		
	if ( CmtGetTSVPtr ( gHandleListHandle , &ptHandleList ) == 0 )
	{
		for ( index = 0; index < ptHandleList->count; index++ )
			if ( ptHandleList->vHandles[index] == handle )
				ptHandleList->vHandles[index] = 0;	
		
		CmtReleaseTSVPtr ( gHandleListHandle ); 
	}
	
	*phInstrumentHandle = 0;

Error:
	
	
	
	RETURN_STDERR_POINTER;
}

/*********************************************** Reset ***********************************************/

void* DLLEXPORT PowerSupply_Reset ( ViSession viInstrumentHandle )   
{   
	STD_ERROR             StdError					=	{0};
	 
	
	STD_ERR ( CPX400_reset ( viInstrumentHandle ));
	
Error: 
		 
	RETURN_STDERR_POINTER; 
}

/*********************************** Wait For Operation Complete ***********************************************/

void* DLLEXPORT PowerSupply_WaitForOperationComplete ( ViSession viInstrumentHandle , double lfTimeout )   
{   
	STD_ERROR                   StdError					=	{0};
	 
	STD_ERR ( WaitForOperationCompletion ( viInstrumentHandle , 5.0 , 0.5 ));  
	
Error: 
	
	RETURN_STDERR_POINTER; 
}

/*********************************************** Get Model *****************************************/

void* DLLEXPORT PowerSupply_GetModel ( ViSession viInstrumentHandle , int channel , char szReturnString[256] )
{
	STD_ERROR             StdError					=	{0};

	STD_ERR ( CPX400_GetAttributeViString (viInstrumentHandle, NULL, CPX400_ATTR_INSTRUMENT_MODEL, 256 , szReturnString ));
		
Error:

	RETURN_STDERR_POINTER;
}

/*********************************************** Get Revision *****************************************/

void* DLLEXPORT PowerSupply_GetRevision ( ViSession viInstrumentHandle , int channel , char szReturnString[256] ) 
{
	STD_ERROR             StdError					=	{0};
	
	STD_ERR ( CPX400_GetAttributeViString (viInstrumentHandle, NULL, CPX400_ATTR_INSTRUMENT_FIRMWARE_REVISION, 256 , szReturnString ));
		
Error:

	RETURN_STDERR_POINTER;
}

/***************************************** Set Output Voltage ***********************************/

void* DLLEXPORT PowerSupply_SetOutputVoltage ( ViSession viInstrumentHandle , int channel , double lfVoltage )
{
	STD_ERROR            	StdError					=	{0};
	
	char					szChannelName[LOW_STRING]	=	{0};
	
	double 					lfGetVoltage				=	0.0;
		
	int						iTryIndex					=	0;
	
	STD_ERR ( WaitForOperationCompletion ( viInstrumentHandle , 5.0 , 0.5 ));    
	
	STD_ERR ( CPX400_GetChannelName (viInstrumentHandle, channel, LOW_STRING, szChannelName));
	
	for ( iTryIndex = 0; iTryIndex < 5; iTryIndex++ )
	{
		STD_ERR ( CPX400_ConfigureVoltageLevel (viInstrumentHandle, szChannelName, lfVoltage));   
	
		STD_ERR ( WaitForOperationCompletion ( viInstrumentHandle , 5.0 , 0.5 ));    
	
		STD_ERR ( CPX400_GetAttributeViReal64 (viInstrumentHandle, szChannelName, CPX400_ATTR_VOLTAGE_LEVEL, &lfGetVoltage));                
	
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
	
	
	char		szChannelName[LOW_STRING]	=	{0};
	
	STD_ERR ( WaitForOperationCompletion ( viInstrumentHandle , 5.0 , 0.5 ));    
	
	STD_ERR ( CPX400_GetChannelName (viInstrumentHandle, channel, LOW_STRING, szChannelName));
	
	STD_ERR ( CPX400_GetAttributeViReal64 (viInstrumentHandle, szChannelName, CPX400_ATTR_VOLTAGE_LEVEL, plfVoltage));
		
Error:

	RETURN_STDERR_POINTER;
}

/*********************************** Get Output Voltage Actual ***********************************/

void* DLLEXPORT PowerSupply_GetOutputVoltageActual ( ViSession viInstrumentHandle , int channel , double *plfVoltage )
{
	STD_ERROR             StdError					=	{0};
	
	
	char		szChannelName[LOW_STRING]	=	{0};
	
	STD_ERR ( WaitForOperationCompletion ( viInstrumentHandle , 5.0 , 0.5 ));    
	
	STD_ERR ( CPX400_GetChannelName (viInstrumentHandle, channel, LOW_STRING, szChannelName));
	
	STD_ERR ( CPX400_Measure (viInstrumentHandle, szChannelName, CPX400_VAL_MEASURE_VOLTAGE, plfVoltage););
		
Error:

	RETURN_STDERR_POINTER;
}

/***************************************** Set Output Current ***********************************/

void* DLLEXPORT PowerSupply_SetOutputCurrent ( ViSession viInstrumentHandle , int channel , double lfCurrent )
{
	STD_ERROR      			StdError					=	{0};
	
	char					szChannelName[LOW_STRING]	=	{0};
	
	double 					lfGetCurrent 				=	0.0;
	
	int						iTryIndex					=	0;
	
	STD_ERR ( WaitForOperationCompletion ( viInstrumentHandle , 5.0 , 0.5 ));    
	
	STD_ERR ( CPX400_GetChannelName (viInstrumentHandle, channel, LOW_STRING, szChannelName));
	
	for ( iTryIndex = 0; iTryIndex < 5; iTryIndex++ )
	{
		STD_ERR ( CPX400_ConfigureCurrentLimit (viInstrumentHandle, szChannelName, CPX400_VAL_CURRENT_REGULATE , lfCurrent));
	
		STD_ERR ( WaitForOperationCompletion ( viInstrumentHandle , 5.0 , 0.5 ));    
	
		STD_ERR ( CPX400_GetAttributeViReal64 (viInstrumentHandle, szChannelName, CPX400_ATTR_CURRENT_LIMIT, &lfGetCurrent));
	
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
	STD_ERROR             StdError					=	{0};
	
	
	char		szChannelName[LOW_STRING]	=	{0};
	
	STD_ERR ( WaitForOperationCompletion ( viInstrumentHandle , 5.0 , 0.5 ));    
	
	STD_ERR ( CPX400_GetChannelName (viInstrumentHandle, channel, LOW_STRING, szChannelName));
	
	STD_ERR ( CPX400_GetAttributeViReal64 (viInstrumentHandle, szChannelName, CPX400_ATTR_CURRENT_LIMIT, plfCurrent));
		
Error:

	RETURN_STDERR_POINTER;
}

/*********************************** Get Output Current Actual ***********************************/

void* DLLEXPORT PowerSupply_GetOutputCurrentActual ( ViSession viInstrumentHandle , int channel , double *plfCurrent )
{
	STD_ERROR             StdError					=	{0};
	
	
	char		szChannelName[LOW_STRING]	=	{0};
	
	STD_ERR ( WaitForOperationCompletion ( viInstrumentHandle , 5.0 , 0.5 ));    
	
	STD_ERR ( CPX400_GetChannelName (viInstrumentHandle, channel, LOW_STRING, szChannelName));
	
	STD_ERR ( CPX400_Measure (viInstrumentHandle, szChannelName, CPX400_VAL_MEASURE_CURRENT, plfCurrent););
		
Error:

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT PowerSupply_GetOutputPowerActual ( ViSession viInstrumentHandle , int channel , double *plfPower )
{
	STD_ERROR             StdError					=	{0};
	
	
	char		szChannelName[LOW_STRING]	=	{0};
	
	double		lfVoltage					=	0.0,
				lfCurrent					=	0.0;
	
	STD_ERR ( WaitForOperationCompletion ( viInstrumentHandle , 5.0 , 0.5 ));    
	
	STD_ERR ( CPX400_GetChannelName (viInstrumentHandle, channel, LOW_STRING, szChannelName));
	
	STD_ERR ( CPX400_Measure (viInstrumentHandle, szChannelName, CPX400_VAL_MEASURE_CURRENT, &lfCurrent););
	STD_ERR ( CPX400_Measure (viInstrumentHandle, szChannelName, CPX400_VAL_MEASURE_VOLTAGE, &lfVoltage);); 
	
	if ( plfPower )
		*plfPower = lfCurrent * lfVoltage;
Error:

	RETURN_STDERR_POINTER;
}

/***************************************** Set Voltage Protection Over ***********************************/

void* DLLEXPORT PowerSupply_SetVoltageProtectionOver ( ViSession viInstrumentHandle , int channel , double lfVoltage )
{
	STD_ERROR             StdError					=	{0};
	
	
	char		szChannelName[LOW_STRING]	=	{0};
	
	STD_ERR ( WaitForOperationCompletion ( viInstrumentHandle , 5.0 , 0.5 ));    
	
	STD_ERR ( CPX400_GetChannelName (viInstrumentHandle, channel, LOW_STRING, szChannelName));
	
	STD_ERR ( CPX400_ConfigureOVP (viInstrumentHandle, szChannelName, VI_TRUE, lfVoltage));
		
Error:

	RETURN_STDERR_POINTER;
}

/***************************************** Get Voltage Protection Over ***********************************/

void* DLLEXPORT PowerSupply_GetVoltageProtectionOver ( ViSession viInstrumentHandle , int channel , double *plfVoltage )
{
	STD_ERROR             StdError					=	{0};
	
	
	char		szChannelName[LOW_STRING]	=	{0};
	
	STD_ERR ( WaitForOperationCompletion ( viInstrumentHandle , 5.0 , 0.5 ));    
	
	STD_ERR ( CPX400_GetChannelName (viInstrumentHandle, channel, LOW_STRING, szChannelName));
	
	STD_ERR ( CPX400_GetAttributeViReal64 (viInstrumentHandle, szChannelName, CPX400_ATTR_OVP_LIMIT, plfVoltage));
		
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
	
	char					szChannelName[LOW_STRING]	=	{0};
	
	int						iGetState					=	0;
	
	STD_ERR ( WaitForOperationCompletion ( viInstrumentHandle , 5.0 , 0.5 ));    
	
	STD_ERR ( CPX400_GetChannelName (viInstrumentHandle, channel, LOW_STRING, szChannelName));
	
	for ( iTryIndex = 0; iTryIndex < 5; iTryIndex++ )
	{
		STD_ERR ( CPX400_ConfigureOutputEnabled ( viInstrumentHandle , szChannelName , iState ));
	
		STD_ERR ( WaitForOperationCompletion ( viInstrumentHandle , 5.0 , 0.5 ));    
	
		STD_ERR ( CPX400_GetAttributeViBoolean (viInstrumentHandle, szChannelName, CPX400_ATTR_OUTPUT_ENABLED, (ViBoolean*)&iGetState));                   
	
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
	STD_ERROR             StdError					=	{0};
	
	
	char		szChannelName[LOW_STRING]	=	{0};
	
	STD_ERR ( WaitForOperationCompletion ( viInstrumentHandle , 5.0 , 0.5 ));    
	
	STD_ERR ( CPX400_GetChannelName (viInstrumentHandle, channel, LOW_STRING, szChannelName));
	
	STD_ERR ( CPX400_GetAttributeViBoolean (viInstrumentHandle, szChannelName, CPX400_ATTR_OUTPUT_ENABLED, (ViBoolean*)piState));
	
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


int		WaitForOperationCompletion( int sessionHandle , double lfTimeout , double lfLowLevelTimeout )
{
	double			lfStartTime						=	0.0,
					lfCurrentTime					=	0.0;

	int				iOpcStatus						=	0;
	
	char			szReadFeedback[LOW_STRING]		=	{0};
	
	int				count							=	0;
	
	int				iTimeout						=	0;
	
	int				iError							=	0;
	
	int				handle							=	0;
	
	Ivi_GetAttributeViSession ( sessionHandle , VI_NULL, IVI_ATTR_IO_SESSION, 0, &handle );
	
	viGetAttribute ( handle , VI_ATTR_TMO_VALUE , &iTimeout );   
	
	SetBreakOnLibraryErrors (0); 
	
	viSetAttribute ( handle , VI_ATTR_TMO_VALUE , (lfLowLevelTimeout*1E3) );  
	
	GetCurrentDateTime(&lfStartTime);
	
	do
	{
		viPrintf( handle , "*OPC?\n" );
						
		viRead( handle, szReadFeedback , LOW_STRING , &count );
	
		iOpcStatus = atoi(szReadFeedback);
		
		if ( iOpcStatus )
			break;
		
		GetCurrentDateTime(&lfCurrentTime);
		
	} while((lfCurrentTime-lfStartTime) < lfTimeout );
	
	viSetAttribute ( handle , VI_ATTR_TMO_VALUE , iTimeout );  
	
	SetBreakOnLibraryErrors (1); 
	
	return iOpcStatus;
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
	
	PowerSupply_Init ( 0 , "GPIB::7" , &viInstrumentHandle , &InstrumentConnectStatus , &InstrumentChanelsQuantity );
	
	PowerSupply_Reset ( viInstrumentHandle );   
	PowerSupply_GetModel ( viInstrumentHandle , 1 , szReturnString );
	PowerSupply_GetRevision ( viInstrumentHandle , 1 , szReturnString );    
	
	PowerSupply_SetOutputVoltage ( viInstrumentHandle , 1 , 1.2 );
	PowerSupply_SetOutputVoltage ( viInstrumentHandle , 2 , 2.4 ); 
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
