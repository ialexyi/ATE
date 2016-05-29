//==============================================================================
// Include files
#include "toolbox.h"
#include <utility.h>
#include "ki4800.h"
#include <ansi_c.h>
#include "GlobalDefines.h"

//==============================================================================
// Constants

#define			MODULE_NAME		"PowerSupply Kikusui Multi"

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

void* PowerSupply_Close ( int *phInstrumentHandle );

/***** ***** ***** ***** ***** ***** Get Error Text Message ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetErrorTextMessage ( int iInstrumentHandle , int iError , char *pErrorMessage )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
													
	if ( pErrorMessage ) 
	{
		ki4800_error_message ( iInstrumentHandle , iError , pErrorMessage );
	}
	
Error:		
		
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
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

/***** ***** ***** ***** ***** ***** ***** ***** Init ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_Init ( int hParentInstrumentHandle , char *szAddress , int *phInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChanelsQuantity )
{			
	errEV					ret									=	{0};
	errEV					*pRet								=	NULL;
	
	int						iKikusuiChanelsQuantity				=	0;
	
	int						handle								=	0;
	
	IF (((phInstrumentHandle == NULL) || (InstrumentConnectStatus == NULL) || (InstrumentChanelsQuantity == NULL)), "Error passed NULL pointer." , MODULE_NAME);
	
	if ( hParentInstrumentHandle == 0 )
	{
		IF (( szAddress == NULL ) , "Error passed NULL pointer." , MODULE_NAME);   
		
		CHK_ERR( ki4800_init ( szAddress , VI_TRUE , VI_FALSE , &handle ));   
		
		*phInstrumentHandle = handle;
	}
	else
	{
		handle = hParentInstrumentHandle;
		
	}
	
	CHK_ERR( ki4800_GetAttributeViInt32 ( handle , "", KI4800_ATTR_CHANNEL_COUNT , &iKikusuiChanelsQuantity ));

	ki4800_ClearError ( handle );
	
	if (iKikusuiChanelsQuantity < 1)
		*InstrumentConnectStatus = 0;
	else
	{
		*phInstrumentHandle = handle;
		*InstrumentConnectStatus = 1;
		*InstrumentChanelsQuantity = iKikusuiChanelsQuantity;
	}
	
Error:
		
	if (ret.error)
		PowerSupply_Close( &handle );
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

/***** ***** ***** ***** ***** ***** ***** ***** Close ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_Close ( int *phInstrumentHandle )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL; 
	
	IF ( (phInstrumentHandle == NULL), "Handle can't be NULL." , MODULE_NAME );
	
	CHK_ERR( ki4800_close ( *phInstrumentHandle ));
	
	*phInstrumentHandle = 0;
	
Error:	
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

/***** ***** ***** ***** ***** ***** ***** ***** Reset ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_Reset ( int iInstrumentHandle ) 
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	CHK_ERR( ki4800_reset ( iInstrumentHandle ));
	
Error:	
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

/***** ***** ***** ***** ***** ***** ***** ***** Get Model ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetModel ( int iInstrumentHandle , int channel , char *pReturnString )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	char					szCannelName[256]	=	{0};
	
//	int						iRetryNumber			=	3;
	
	CHK_ERR( ki4800_GetChannelName ( iInstrumentHandle , channel , 256 , szCannelName ));

	CHK_ERR( ki4800_QueryTrackName ( iInstrumentHandle , szCannelName , 256 , pReturnString ));
	
Error:	
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** ***** Get Revision ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetRevision ( int iInstrumentHandle , int channel , char *pReturnString )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	char					szCannelName[256]	=	{0};
	
//	int						iRetryNumber			=	3;
	
	CHK_ERR( ki4800_GetChannelName ( iInstrumentHandle , channel , 256 , szCannelName ));

	CHK_ERR( ki4800_GetAttributeViString ( iInstrumentHandle , szCannelName , KI4800_ATTR_INSTRUMENT_FIRMWARE_REVISION , 256 , pReturnString ));
	
Error:	
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}


/***** ***** ***** ***** ***** ***** ***** Set Output Voltage ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_SetOutputVoltage ( int iInstrumentHandle , int channel , double lfVoltage )
{
	errEV					ret						=	{0};
	errEV					*pRet					=	NULL;
	
	double					lfVoltageDefined		=	0.0,
							lfVoltageDelta			=	0.0,
							lfOffsetVoltage			=	0.0;
	
	char					szCannelName[256]		=	{0}; 
	
//	int						iRetryNumber			=	3;
	
	CHK_ERR( ki4800_GetChannelName ( iInstrumentHandle , channel , 256 , szCannelName ));

	CHK_ERR( ki4800_ConfigureVoltageLevel ( iInstrumentHandle , szCannelName , lfVoltage ));

Error:
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** Get Output Voltage Defined ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetOutputVoltageDefined ( int iInstrumentHandle , int channel , double *plfVoltage )
{
	errEV					ret						=	{0};
	errEV					*pRet					=	NULL;
	
	char					szCannelName[256]		=	{0}; 
	
//	int						iRetryNumber			=	3;
	
	CHK_ERR( ki4800_GetChannelName ( iInstrumentHandle , channel , 256 , szCannelName ));

	CHK_ERR( ki4800_GetAttributeViReal64 ( iInstrumentHandle , szCannelName , KI4800_ATTR_VOLTAGE_LEVEL , plfVoltage ));

Error:
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** Get Output Voltage Actual ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetOutputVoltageActual ( int iInstrumentHandle , int channel , double *plfVoltage )
{
	errEV					ret						=	{0};
	errEV					*pRet					=	NULL;
	
	char					szCannelName[256]		=	{0}; 
	
//	int						iRetryNumber			=	3;
	
	CHK_ERR( ki4800_GetChannelName ( iInstrumentHandle , channel , 256 , szCannelName ));

	CHK_ERR( ki4800_Measure ( iInstrumentHandle , szCannelName , KI4800_VAL_MEASURE_VOLTAGE , plfVoltage ));

Error:
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** Set Output Current ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_SetOutputCurrent ( int iInstrumentHandle , int channel , double lfCurrent )
{
	errEV					ret						=	{0};
	errEV					*pRet					=	NULL;
	
	double					lfVoltageDefined		=	0.0,
							lfVoltageDelta			=	0.0,
							lfOffsetVoltage			=	0.0;
	
	char					szCannelName[256]		=	{0}; 
	
//	int						iRetryNumber			=	3;
	
	CHK_ERR( ki4800_GetChannelName ( iInstrumentHandle , channel , 256 , szCannelName ));

	CHK_ERR( ki4800_ConfigureCurrentLimit ( iInstrumentHandle , szCannelName , KI4800_VAL_CURRENT_REGULATE , lfCurrent ));

Error:
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** Get Output Current Defined ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetOutputCurrentDefined ( int iInstrumentHandle , int channel , double *plfCurrent )
{
	errEV					ret						=	{0};
	errEV					*pRet					=	NULL;
	
	char					szCannelName[256]		=	{0}; 
	
//	int						iRetryNumber			=	3;
	
	CHK_ERR( ki4800_GetChannelName ( iInstrumentHandle , channel , 256 , szCannelName ));

	CHK_ERR( ki4800_GetAttributeViReal64 ( iInstrumentHandle , szCannelName , KI4800_ATTR_CURRENT_LIMIT , plfCurrent ));

Error:
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** Get Output Current Actual ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetOutputCurrentActual ( int iInstrumentHandle , int channel , double *plfCurrent )
{
	errEV					ret						=	{0};
	errEV					*pRet					=	NULL;
	
	char					szCannelName[256]		=	{0}; 
	
//	int						iRetryNumber			=	3;
	
	CHK_ERR( ki4800_GetChannelName ( iInstrumentHandle , channel , 256 , szCannelName ));

	CHK_ERR( ki4800_Measure ( iInstrumentHandle , szCannelName , KI4800_VAL_MEASURE_CURRENT , plfCurrent ));			

Error:
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** Get Output Power Actual ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetOutputPowerActual ( int iInstrumentHandle , int channel , double *plfPower )
{
	errEV					ret						=	{0};
	errEV					*pRet					=	NULL;
	
	double					lfCurrent				=	0.0,
							lfVoltage				=	0.0;
	
//	int						iRetryNumber			=	3;
	
	pRet = PowerSupply_GetOutputCurrentActual ( iInstrumentHandle , channel , &lfCurrent );

	if ( pRet && pRet->error )
		RETURN;
		
	pRet = PowerSupply_GetOutputVoltageActual ( iInstrumentHandle , channel , &lfVoltage );
	
	if ( pRet && pRet->error )
		RETURN;
	
	if ( plfPower )
		*plfPower = lfCurrent * lfVoltage;
	
Error:	
	
	return ((void*)pRet);
}
/***** ***** ***** ***** ***** ***** ***** Set Voltage Protection Over ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_SetVoltageProtectionOver ( int iInstrumentHandle , int channel , double lfVoltage )
{
	errEV					ret						=	{0};
	errEV					*pRet					=	NULL;
	
	double					lfVoltageDefined		=	0.0,
							lfVoltageDelta			=	0.0,
							lfOffsetVoltage			=	0.0;
	
	char					szCannelName[256]		=	{0}; 
	
	//int						iRetryNumber			=	3;
	
	CHK_ERR( ki4800_GetChannelName ( iInstrumentHandle , channel , 256 , szCannelName ));

	CHK_ERR( ki4800_ConfigureOVPLimit ( iInstrumentHandle , szCannelName , lfVoltage ));

Error:
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** Get Voltage Protection Over ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetVoltageProtectionOver ( int iInstrumentHandle , int channel , double *plfVoltage )
{
	errEV					ret						=	{0};
	errEV					*pRet					=	NULL;
	
	char					szCannelName[256]		=	{0}; 
	
//	int						iRetryNumber			=	3;
	
	CHK_ERR( ki4800_GetChannelName ( iInstrumentHandle , channel , 256 , szCannelName ));

	CHK_ERR( ki4800_GetAttributeViReal64 ( iInstrumentHandle , szCannelName , KI4800_ATTR_OVP_LIMIT , plfVoltage ));

Error:
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** Set Voltage Protection Under ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_SetVoltageProtectionUnder ( int iInstrumentHandle , int channel , double lfVoltage )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
Error:
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** Get Voltage Protection Under ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetVoltageProtectionUnder ( int iInstrumentHandle , int channel , double *plfVoltage )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
Error:
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** ***** Set Output State ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_SetOutputState ( int iInstrumentHandle , int channel , int iState )
{
	errEV					ret						=	{0};
	errEV					*pRet					=	NULL;
	
	double					lfVoltageDefined		=	0.0,
							lfVoltageDelta			=	0.0,
							lfOffsetVoltage			=	0.0;
	
	char					szCannelName[256]		=	{0}; 
	
//	int						iRetryNumber			=	3;
	
	CHK_ERR( ki4800_GetChannelName ( iInstrumentHandle , channel , 256 , szCannelName ));

	CHK_ERR( ki4800_ConfigureOutputEnabled ( iInstrumentHandle , szCannelName , iState ));

Error:
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** ***** Get Output State ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetOutputState ( int iInstrumentHandle , int channel , int *piState )
{
	errEV					ret						=	{0};
	errEV					*pRet					=	NULL;
	
	char					szCannelName[256]		=	{0}; 
	
	short					iState					=	0;
	
//	int						iRetryNumber			=	3;
	
	CHK_ERR( ki4800_GetChannelName ( iInstrumentHandle , channel , 256 , szCannelName ));

	CHK_ERR( ki4800_GetAttributeViBoolean ( iInstrumentHandle , szCannelName , KI4800_ATTR_OUTPUT_ENABLED , &iState ));

	if ( piState )
		*piState = iState;
Error:
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** Set Foldback Protection Mode ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_SetFoldbackProtectionMode ( int iInstrumentHandle , int channel , int iMode )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
Error:
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** Get Foldback Protection Mode ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetFoldbackProtectionMode ( int iInstrumentHandle , int channel , int *piMode )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
Error:
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** Set Auto Restart Mode ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_SetAutoRestartMode ( int iInstrumentHandle , int channel , int iMode )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
Error:
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** Get Auto Restart Mode ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetAutoRestartMode ( int iInstrumentHandle , int channel , int *piMode )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
Error:
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** Get Operational Status Register ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetOperationalStatusRegister ( int iInstrumentHandle , int channel , char *pReturnString )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
Error:
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** Get Alarm Status Register ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetAlarmStatusRegister ( int iInstrumentHandle , int channel , char *pReturnString )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
Error:
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** Get Error Codes Register ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetErrorCodesRegister ( int iInstrumentHandle , int channel , char *pReturnString )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
Error:
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** Get Complete Status ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetCompleteStatus ( int iInstrumentHandle , int channel , char *pReturnString )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
Error:
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** Clear Comm Buffer And Registers ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_ClearCommBufferAndRegisters ( int iInstrumentHandle , int channel )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
Error:
	
	FREE_CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}


int main (int argc, char *argv[])
{
	errEV					*pRet							=	NULL;
	
	int						hInstrumentHandle				=	0,
							InstrumentConnectStatus			=	0,
							InstrumentChanelsQuantity		=	0;
	
	double					lfVoltage						=	0.0,
							lfCurrent						=	0.0,
							lfPower							=	0.0;
	
	char					string[512]						=	{0};
	
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;  /* out of memory */
  
	pRet = PowerSupply_Init ( 0 , "USB0::0x0B3E::0x1014::RD002071::INSTR" , &hInstrumentHandle , &InstrumentConnectStatus , &InstrumentChanelsQuantity );
	FREE(pRet);
	
	pRet = PowerSupply_Reset( hInstrumentHandle );
	FREE(pRet);
	
	pRet = PowerSupply_GetModel( hInstrumentHandle , 1 ,  string);
	FREE(pRet);
	
	pRet = PowerSupply_GetRevision( hInstrumentHandle , 1 , string );
	FREE(pRet);
	
	//------------------ Setting channel 1 ------------------------//
	pRet = PowerSupply_SetOutputState ( hInstrumentHandle , 1 , 0 ); 
	FREE(pRet);
	
	lfVoltage = 5.1;
	
	pRet = PowerSupply_SetOutputVoltage( hInstrumentHandle , 1 , lfVoltage);
	FREE(pRet);
	
	lfCurrent = 3.1;
	
	pRet = PowerSupply_SetOutputCurrent( hInstrumentHandle , 1 , lfCurrent);
	FREE(pRet);
	
	lfVoltage = 6.1;
	
	pRet = PowerSupply_SetVoltageProtectionOver( hInstrumentHandle , 1 , lfVoltage);
	FREE(pRet);
	
	pRet = PowerSupply_SetOutputState ( hInstrumentHandle , 1 , 1 );  
	FREE(pRet);	
	
	//------------------ Setting channel 2 ------------------------//
	pRet = PowerSupply_SetOutputState ( hInstrumentHandle , 2 , 0 ); 
	FREE(pRet);
	
	lfVoltage = 3.2;
	
	pRet = PowerSupply_SetOutputVoltage( hInstrumentHandle , 2 , lfVoltage);
	FREE(pRet);
	
	lfCurrent = 1.2;
	
	pRet = PowerSupply_SetOutputCurrent( hInstrumentHandle , 2 , lfCurrent);
	FREE(pRet);
	
	lfVoltage = 7.2;
	
	pRet = PowerSupply_SetVoltageProtectionOver( hInstrumentHandle , 2 , lfVoltage);
	FREE(pRet);
	
	pRet = PowerSupply_SetOutputState ( hInstrumentHandle , 2 , 1 );  
	FREE(pRet);	
	
	//------------------ Read channel 1 ------------------------//
	pRet = PowerSupply_GetOutputVoltageDefined( hInstrumentHandle , 1 , &lfVoltage);
	FREE(pRet);
	
	pRet = PowerSupply_GetOutputVoltageActual( hInstrumentHandle , 1 , &lfVoltage);
	FREE(pRet);
	
	pRet = PowerSupply_GetOutputCurrentDefined( hInstrumentHandle , 1 , &lfCurrent);
	FREE(pRet);
	
	pRet = PowerSupply_GetOutputCurrentActual( hInstrumentHandle , 1 , &lfCurrent );
	FREE(pRet);
	
	pRet = PowerSupply_GetOutputPowerActual( hInstrumentHandle , 1 , &lfPower);
	FREE(pRet);
	
	pRet = PowerSupply_GetVoltageProtectionOver( hInstrumentHandle , 1 , &lfVoltage );
	FREE(pRet);
	
		//------------------ Read channel 2 ------------------------//
	pRet = PowerSupply_GetOutputVoltageDefined( hInstrumentHandle , 2 , &lfVoltage);
	FREE(pRet);
	
	pRet = PowerSupply_GetOutputVoltageActual( hInstrumentHandle , 2 , &lfVoltage);
	FREE(pRet);
	
	pRet = PowerSupply_GetOutputCurrentDefined( hInstrumentHandle , 2 , &lfCurrent);
	FREE(pRet);
	
	pRet = PowerSupply_GetOutputCurrentActual( hInstrumentHandle , 2 , &lfCurrent );
	FREE(pRet);
	
	pRet = PowerSupply_GetOutputPowerActual( hInstrumentHandle , 2 , &lfPower);
	FREE(pRet);
	
	pRet = PowerSupply_GetVoltageProtectionOver( hInstrumentHandle , 2 , &lfVoltage );
	FREE(pRet);
	
	pRet = PowerSupply_Close( &hInstrumentHandle );
	FREE(pRet);
	
	return 0;
}
