#include <windows.h>
#include <userint.h>
#include <ansi_c.h>
#include <utility.h>
#include "chr62000.h"
#include <ividcpwr.h> 
#include "GlobalDefines.h"
#include "toolbox.h"

//==============================================================================
// Include files

#include "Chroma_62XXX_wrapper.h"

//==============================================================================
// Constants

#define			MODULE_NAME			"Chroma_62XXX_wrapper"

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

//==============================================================================
// Global functions

/***** ***** ***** ***** ***** ***** Get Error Text Message ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetErrorTextMessage ( ViSession viInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR						StdError									=	{0};

	STD_ERR ( chr62000_error_message	( viInstrumentHandle , iError , pErrorMessage ));
	
Error:

	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		Equipment_Info ( int hLowLevelHandle , char *pAddress , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware )
{
	STD_ERROR						StdError									=	{0};
	
	int								iCount										=	0, 
									status										=	0, 
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
		IF (( strstr( szIdentificationText , "CHROMA" ) == NULL ) , "Is not supported" );
	
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
									vszSupportdedModels[][12]					=	{{"62006P-100-25"},{"62012P-30-160"},{"62012P-80-60"},{"62012P-100-50"},{"62012P-600-8"}};
	
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
		if ( strstr( szIdentificationText , "CHROMA" ) == NULL )
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

/***** ***** ***** ***** ***** ***** ***** ***** Init ***** ***** ***** ***** ***** ***** ***** *****/

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
		
		STD_ERR ( chr62000_init ( szRsrcAddress, VI_TRUE, VI_FALSE, &handle));
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
	
	STD_ERR ( chr62000_ClearErrorInfo (handle));
	
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
		chr62000_error_message (handle, StdError.error, szErrorTextMessage);
		
		MessagePopup("Power Supply Initializing Error!",szErrorTextMessage);
		
		chr62000_close (handle);
	}
	else
		if ( InstrumentConnectStatus )
			*InstrumentConnectStatus = 1;	
	
	
	
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** Close ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_Close ( ViSession *phInstrumentHandle )
{
	STD_ERROR            			StdError										=	{0};
	
	ViSession						handle											=	0;

	tsHandleList					*ptHandleList									=	NULL;

	int								index											=	0;
		
	IF ( (phInstrumentHandle == NULL) , "Pointer passed NULL.");
	
	handle = *phInstrumentHandle;
	
	IF ( (handle == 0) , "Pointer passed 0.");

	STD_ERR ( chr62000_close ( handle ));
		
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

/***** ***** ***** ***** ***** ***** ***** ***** Reset ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerSupply_Reset ( ViSession viInstrumentHandle )
{
	STD_ERROR             StdError					=	{0};

	STD_ERR ( chr62000_reset (viInstrumentHandle));
	
Error: 
		 
	RETURN_STDERR_POINTER; 
}

/*********************************** Wait For Operation Complete ***********************************************/

void* DLLEXPORT PowerSupply_WaitForOperationComplete ( ViSession viInstrumentHandle , double lfTimeout )   
{   
	STD_ERROR                   StdError					=	{0};
	 
	STD_ERR ( WaitForOperationCompletion ( viInstrumentHandle , lfTimeout , 0.5 )); 
	
Error: 
	
	RETURN_STDERR_POINTER; 
}

/***** ***** ***** ***** ***** ***** ***** ***** Get Model ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetModel ( ViSession viInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR             StdError					=	{0};

	STD_ERR ( Ivi_GetAttributeViString (viInstrumentHandle, NULL , IVI_ATTR_INSTRUMENT_MODEL , 0 , sizeof(pReturnString), pReturnString));
		
Error:

	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** Get Revision ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetRevision ( ViSession viInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR             StdError					=	{0};
	
	STD_ERR ( Ivi_GetAttributeViString (viInstrumentHandle, NULL, IVI_ATTR_INSTRUMENT_FIRMWARE_REVISION , 0 , sizeof(pReturnString), pReturnString));
		
Error:

	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** ***** Set Output Voltage ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_SetOutputVoltage ( ViSession viInstrumentHandle , int channel , double lfVoltage )
{
	STD_ERROR             StdError					=	{0};
	
	STD_ERR ( chr62000_SetVoltage ( viInstrumentHandle , lfVoltage));
		
Error:

	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** Get Output Voltage Defined ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetOutputVoltageDefined ( ViSession viInstrumentHandle , int channel , double *plfVoltage )
{
	STD_ERROR             StdError					=	{0};

	STD_ERR ( Ivi_GetAttributeViReal64 ( viInstrumentHandle , NULL , CHR62000_ATTR_VOLTAGE , 0 , plfVoltage ));
		
Error:

	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** Get Output Voltage Actual ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetOutputVoltageActual ( ViSession viInstrumentHandle , int channel , double *plfVoltage )
{
	STD_ERROR             StdError					=	{0};
	
	STD_ERR ( chr62000_Measure (viInstrumentHandle, 0 , plfVoltage ););
		
Error:

	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** ***** Set Output Current ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_SetOutputCurrent ( ViSession viInstrumentHandle , int channel , double lfCurrent )
{
	STD_ERROR             StdError					=	{0};
	
	STD_ERR ( chr62000_SetCurrent (viInstrumentHandle , lfCurrent ));
		
Error:

	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** Get Output Current Defined ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetOutputCurrentDefined ( ViSession viInstrumentHandle , int channel , double *plfCurrent )
{
	STD_ERROR             StdError					=	{0};
	
	STD_ERR ( Ivi_GetAttributeViReal64 ( viInstrumentHandle , NULL , CHR62000_ATTR_CURRENT , 0 , plfCurrent ));
		
Error:

	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** Get Output Current Actual ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetOutputCurrentActual ( ViSession viInstrumentHandle , int channel , double *plfCurrent )
{
	STD_ERROR             StdError					=	{0};
	
	STD_ERR ( chr62000_Measure (viInstrumentHandle, 1 , plfCurrent ););
		
Error:

	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** Get Output Current Actual ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetOutputPowerActual ( ViSession viInstrumentHandle , int channel , double *plfPower )
{
	STD_ERROR             StdError					=	{0};
	
	STD_ERR ( chr62000_Measure (viInstrumentHandle, 2 , plfPower ););
		
Error:

	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** ***** Set Voltage Protection Over ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_SetVoltageProtectionOver ( ViSession viInstrumentHandle , int channel , double lfVoltage )
{
	STD_ERROR             StdError					=	{0};
	
	STD_ERR ( chr62000_SetVoltageProtect (viInstrumentHandle , lfVoltage));
		
Error:

	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** ***** Get Voltage Protection Over ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetVoltageProtectionOver ( ViSession viInstrumentHandle , int channel , double *plfVoltage )
{
	STD_ERROR             StdError					=	{0};
	
	STD_ERR ( Ivi_GetAttributeViReal64 ( viInstrumentHandle , NULL , IVIDCPWR_VAL_OUTPUT_OVER_VOLTAGE , 0 , plfVoltage )); 
		
Error:

	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** ***** Set Voltage Protection Under ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_SetVoltageProtectionUnder ( ViSession viInstrumentHandle , int channel , double lfVoltage )
{
	STD_ERROR             StdError					=	{0};

Error:

	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** ***** Get Voltage Protection Under ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetVoltageProtectionUnder ( ViSession viInstrumentHandle , int channel , double *plfVoltage )
{
	STD_ERROR             StdError					=	{0};

Error:

	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** ***** ***** Set Output State ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_SetOutputState ( ViSession viInstrumentHandle , int channel , int iState )
{
	STD_ERROR             StdError					=	{0};
	
	return ( chr62000_SetOutput ( viInstrumentHandle , iState ));
	
Error:

	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** ***** ***** Get Output State ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetOutputState ( ViSession viInstrumentHandle , int channel , int *piState )
{	
	STD_ERROR             StdError					=	{0};
	
	char		channels[64]				=	{0};
	
	STD_ERR ( chr62000_QueryOutput ( viInstrumentHandle , channels )); 
	
	if ( piState )
		*piState = channels[0];
	
Error:

	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** ***** Set Foldback Protection Mode ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_SetFoldbackProtectionMode ( ViSession viInstrumentHandle , int channel , int iMode )
{
	STD_ERROR             StdError					=	{0};

Error:

	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** ***** Get Foldback Protection Mode ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetFoldbackProtectionMode ( ViSession viInstrumentHandle , int channel , int *piMode )
{
	STD_ERROR             StdError					=	{0};
	
Error:

	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** ***** Set Auto Restart Mode ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_SetAutoRestartMode ( ViSession viInstrumentHandle , int channel , int iMode )
{
	STD_ERROR             StdError					=	{0};
	
Error:

	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** ***** Get Auto Restart Mode ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetAutoRestartMode ( ViSession viInstrumentHandle , int channel , int *piMode )
{
	STD_ERROR             StdError					=	{0};
	
Error:

	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** Get Operational Status Register ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetOperationalStatusRegister ( ViSession viInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR             StdError					=	{0};
	
Error:

	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** Get Alarm Status Register ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetAlarmStatusRegister ( ViSession viInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR             StdError					=	{0};
	
Error:

	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** Get Error Codes Register ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetErrorCodesRegister ( ViSession viInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR             StdError					=	{0};
	
Error:

	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** Get Complete Status ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetCompleteStatus ( ViSession viInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR             StdError					=	{0};
	
Error:

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
