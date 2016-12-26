
//==============================================================================
// Include files

#include <windows.h>
#include "toolbox.h"
#include <cvirte.h>
#include <userint.h>
#include <visa.h>
#include <utility.h>
#include "GlobalDefines.h"
#include <analysis.h>
#include <formatio.h>

//==============================================================================
// Constants

//==============================================================================
// Types
typedef struct
{
	ViSession			defaultRM,
						sessionHandle;
	
	char				*pLastStateFile,
						*pVendor,
						*pModel,
						*pFirmware,
						*pSerialNumber;
	
	double				lfTimeout,
						lfOpcLowLevelTimeout;
	
	double				lfStateFileDelay,
						lfStateFileTimeout;
	
	tsSTD_CallBackSet	*ptCallbacks;
	
} tsHandle;
//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

/***** ***** ***** ***** ***** ***** ***** ***** Wait For Operation Completion ***** ***** ***** ***** ***** ***** ***** *****/     

int		WaitForOperationCompletion( int sessionHandle , double lfTimeout , double lfLowLevelTimeout );

/***** ***** ***** ***** ***** ***** ***** ***** Recursive Make Directory ***** ***** ***** ***** ***** ***** ***** *****/    

int		RecursiveMakeDirectory( int sessionHandle , char *pPath );

/***** ***** ***** ***** ***** ***** ***** ***** Recall State ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerMeter_RecallState( int hInstrumentHandle ,char *szFileName );


/***** ***** ***** ***** ***** ***** ***** ***** Config Copy STD Call Back Set ***** ***** ***** ***** ***** ***** ***** *****/ 

void*	DLLEXPORT		Config_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet )
{
	STD_ERROR						StdError									=	{0};
	
	tsHandle						*pLocalStorage								=	NULL;			
	
	int								handle										=	0;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );
	
	if ( *pHandle == 0 )
	{
		CHK_CMT(CmtNewTSV ( sizeof(tsHandle) , pHandle ));
	}
	
	handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	if ( pLocalStorage->ptCallbacks )
	{
		FREE(pLocalStorage->ptCallbacks->pCommentCallbackData);
		FREE(pLocalStorage->ptCallbacks->pConfigValueCallbackData);
		FREE(pLocalStorage->ptCallbacks->pCheckForBreakCallbackData);
		FREE(pLocalStorage->ptCallbacks->pFileCallbackData);
	
		FREE(pLocalStorage->ptCallbacks);	
	}
		
	pLocalStorage->ptCallbacks = pCallBackSet;

Error:
	
	if ( handle )
		CmtReleaseTSVPtr (handle);
	
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** Get Error Text Message ***** ***** ***** ***** ***** ***** ***** *****/   

void* DLLEXPORT PowerMeter_GetErrorTextMessage ( int hInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR		StdError							=	{0};
																			
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char			szReadBuffer[LOW_STRING]			=	{0},
					szErrorDescription[LOW_STRING]		=	{0};
	
	int				count								=	0;
	
	char			*pTemp								=	NULL;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	SetBreakOnLibraryErrors (0); 
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if( pLocalHandle->lfTimeout != 0 && pLocalHandle->lfOpcLowLevelTimeout != 0 ) 
		WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	else
		DelayWithEventProcessing(0.1);
								 
	viPrintf( pLocalHandle->sessionHandle , ":SYST:ERR?\n" ); 
	viRead( pLocalHandle->sessionHandle, szReadBuffer , LOW_STRING , &count );
	
	iError = atoi(szReadBuffer);
	
	pTemp = strchr( szReadBuffer , '"' );
	
	if ( pTemp )
	{
		pTemp++;
		
		strcpy( szErrorDescription , pTemp );  
		
		pTemp = strchr( szErrorDescription , '"' );
		
		if ( pTemp )
			*pTemp = 0;
	}
	
	viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );
	
	if ( pErrorMessage )
	{
		strcpy( pErrorMessage , szErrorDescription ); 
	}

	StdError.error = iError;
	
	if ( iError )
	{
		CALLOC_COPY_STRING( StdError.pszErrorDescription , szErrorDescription );
	}
	
Error:

	SetBreakOnLibraryErrors (1);
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** Equipment Info ***** ***** ***** ***** ***** ***** ***** *****/  

void*	DLLEXPORT		Equipment_Info ( int hLowLevelHandle , char *pAddress , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware )
{
	STD_ERROR						StdError									=	{0};
	
	int								iCount										=	0, 
									status										=	0, 
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
		
		IF ((( strstr( szIdentificationUpperText , "Agilent Technologies" ) == NULL )) && (( strstr( szIdentificationUpperText , "Keysight Technologies" ) == NULL ))  , "Is not supported" );
	
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

/***** ***** ***** ***** ***** ***** ***** ***** Get Low Level Handle ***** ***** ***** ***** ***** ***** ***** *****/  

void*	DLLEXPORT		Equipment_GetLowLevelHandle( int hHandle , int *phLowLevelHandle )
{
	STD_ERROR						StdError									=	{0};
	
	if ( phLowLevelHandle )
		*phLowLevelHandle = hHandle;
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** ***** ***** Equipment IsSupported ***** ***** ***** ***** ***** ***** ***** *****/ 

void*	DLLEXPORT		Equipment_IsSupported ( int hLowLevelHandle , char *pAddress , char *pID_String , char *pIdentificationText , int *pbSupporting , void **pLocalData )
{
	STD_ERROR						StdError									=	{0};
	
	int								iCount										=	0, 
									iIndex										=	0;
	
	int								status										=	0,
									defaultRM									=	0;
	
	int								bSupport									=	0, 
									hConnectionHandle							=	0;
	
	char							szIdentificationText[LOW_STRING]			=	{0}; 

	char							vszSupportedModels[][17]					=	{"N1912A","N1911A","N8262A","E9325A","E9326A","E9327A","N1921A","N1922A","U2000A","U2001A","U2002A","U2004A","U2001H","U2001B","U2000H","U2000B","U2002H","U2002B","E4416A","E4417A","E4418B","E4419B","8481A","8482A","8483A","8485A","8487A","8481B","8482B","8481H","8482H","8487D","8485D","8481D","N1913A","N1914A","N432A","U2021XA","U2022XA","U2042XA","U8481A", "U8485A", "U8487A", "U8488A" };
			
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
		if (( strstr( szIdentificationText , "Agilent Technologies" ) == NULL ) && ( strstr( szIdentificationText , "Keysight Technologies" ) == NULL )) 
			break;
	
		for ( iIndex = 0; iIndex < (sizeof(vszSupportedModels) / sizeof(vszSupportedModels[0])); iIndex++ )
			if ( strstr( szIdentificationText , vszSupportedModels[iIndex]) )
			{		
				bSupport = 1; 
				break;
			}	
					
	}while(0);
	
	if (pbSupporting)
		*pbSupporting = bSupport;
	
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** Init  ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerMeter_Init ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *vhInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChannelsQuantity )
{
	STD_ERROR						StdError										=	{0};
	
	tsHandle						*pLocalHandle									=	{0};
	
	CmtTSVHandle 					handle											=	0;
	
	int								supported										=	0;
	
	tsSTD_CallBackSet				tSTD_CallBackSet								=	{0};
	
	double							lfTimeout										=	0.0,
									lfStateFileDelay								=	1.0,  
									lfStateFileTimeout								=	10.0, 
									lfDefaultTimeout								=	5.0;
									
	IF (( szRsrcAddress == NULL ) , "Address string is empty" );
	
	if ( hParentInstrumentHandle == 0 ) 
	{
		CHK_CMT ( CmtNewTSV ( sizeof(tsHandle) , &handle ));
	}
	else
		handle = hParentInstrumentHandle;
		
	if ( vhInstrumentHandle )
		*vhInstrumentHandle = handle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));

	if ( pLocalHandle->defaultRM == 0 )
	{
		CHK_VSA ( viOpenDefaultRM (&(pLocalHandle->defaultRM)));
	}
	
	if ( pLocalHandle->sessionHandle == 0 )
	{
		CHK_VSA ( viOpen ( pLocalHandle->defaultRM , szRsrcAddress , VI_NULL, VI_NULL, &(pLocalHandle->sessionHandle)));
	}
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , 20.0 , 0.5 );
	
	CHK_VSA ( viSetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , 500 ));
	
	FREE_STDERR_COPY_ERR( Equipment_IsSupported ( pLocalHandle->sessionHandle , NULL , NULL , NULL , &supported , &pLocalHandle )); 

	IF (( supported == 0 ) , "This device has not supported." );

	FREE_STDERR_COPY_ERR( Equipment_Info ( pLocalHandle->sessionHandle , NULL , &pLocalHandle->pVendor , &pLocalHandle->pSerialNumber , &pLocalHandle->pModel , &pLocalHandle->pFirmware ));

	STDF_UPDATE_CALLBACK_DATA(pLocalHandle->ptCallbacks); 
	
	viPrintf( pLocalHandle->sessionHandle ,"*CLS\n" ); 
	
	viPrintf( pLocalHandle->sessionHandle ,":FORM:BORD SWAP\n" );  
	
	STDF_CONFIG_VALUE("Timeout", VAL_DOUBLE , 1 , lfTimeout , lfDefaultTimeout );	
	
	if ( lfTimeout == 0.0 )
		lfTimeout = lfDefaultTimeout;
	
	viSetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , (lfTimeout*1E3) );
	
	STDF_CONFIG_VALUE("OPC_Timeout", VAL_DOUBLE , 1 , (pLocalHandle->lfTimeout) , lfDefaultTimeout );	
	
	if ( pLocalHandle->lfTimeout == 0.0 )
		pLocalHandle->lfTimeout = lfDefaultTimeout;
	
	STDF_CONFIG_VALUE("OPC_LowLevel_Timeout", VAL_DOUBLE , 1 , (pLocalHandle->lfOpcLowLevelTimeout) , lfDefaultTimeout );	 
	
	if ( pLocalHandle->lfOpcLowLevelTimeout == 0.0 )
		pLocalHandle->lfOpcLowLevelTimeout = lfDefaultTimeout;
	
		STDF_CONFIG_VALUE("State_File_Timeout", VAL_DOUBLE , 1 , (pLocalHandle->lfStateFileTimeout) , lfStateFileTimeout );	 
	
	if ( pLocalHandle->lfStateFileTimeout == 0.0 )
		pLocalHandle->lfStateFileTimeout = lfStateFileTimeout;
	
	STDF_CONFIG_VALUE("State_File_Delay", VAL_DOUBLE , 1 , (pLocalHandle->lfStateFileDelay) , lfStateFileDelay );	 
	
	if ( pLocalHandle->lfStateFileDelay == 0.0 )
		pLocalHandle->lfStateFileDelay = lfStateFileDelay;   
Error:
	
	if ( InstrumentConnectStatus )
		*InstrumentConnectStatus = supported;
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** Close ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerMeter_Close ( int *vhInstrumentHandle )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;
	
	IF (( vhInstrumentHandle == NULL ) , "Handle is empty" );
	
	handle = *vhInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	viClose (pLocalHandle->sessionHandle);  
	
	viClose (pLocalHandle->defaultRM);
	
	FREE(pLocalHandle->pLastStateFile);
	FREE(pLocalHandle->pFirmware);
	FREE(pLocalHandle->pSerialNumber);
	FREE(pLocalHandle->pModel);
	
	CmtReleaseTSVPtr ( handle ); 
	
	CmtDiscardTSV (handle);
	
	*vhInstrumentHandle = 0;
	
Error:

	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** Reset   ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerMeter_Reset ( int hInstrumentHandle )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,"*CLS\n" ));
				
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,"*RST\n" ));
						
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Configuration   ***** ***** ***** ***** ***** ***** ***** *****/      

void* DLLEXPORT PowerMeter_Configuration( int hInstrumentHandle , int iChannel , int bContinue , int bDelayAutoState , int iTriggerSource , double lfTrigHoldOff , double lfGateOffsetTime , double lfGateTime , double lfTraceTime )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;
	
	char					szCommand[LOW_STRING]					=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if ( bContinue )
		bContinue = 1;
	
	sprintf( szCommand , "INIT%d:CONT %d\n" , iChannel , bContinue );
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Set Single Sweep   ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerMeter_SetSingleSweep( int hInstrumentHandle , int iChannel )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;

	char					szCommand[LOW_STRING]					=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szCommand , "INIT%d:CONT OFF\n" , iChannel );
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
	sprintf( szCommand , "INIT%d:IMM\n" , iChannel );
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Set Continues Sweep   ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerMeter_SetContinuesSweep( int hInstrumentHandle , int iChannel ) 
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;

	char					szCommand[LOW_STRING]					=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szCommand , "INIT%d:CONT ON\n" , iChannel );
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Init Sweep   ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerMeter_InitSweep( int hInstrumentHandle , int iChannel ) 
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};
	
	char					szCommand[LOW_STRING]					=	{0};

	int						handle									=	0;

	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	//CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "TRIG:IMM\n" ));
	//sprintf( szCommand , "*CLS"); 
	//CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));
	
	//viClear(pLocalHandle->sessionHandle); 
	//sprintf( szCommand , "SENS%d:TRAC:AUT\n", iChannel); 
	//CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));
	
	
	//DelayWithEventProcessing(1); 
	
	sprintf( szCommand , "INIT%d\n", iChannel); 
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Recall State By Name  ***** ***** ***** ***** ***** ***** ***** *****/   

void* DLLEXPORT PowerMeter_RecallState_ByName( int hInstrumentHandle , char *szFileName ) 
{
	return PowerMeter_RecallState( hInstrumentHandle ,szFileName );
}

/***** ***** ***** ***** ***** ***** ***** ***** Set Auto  ***** ***** ***** ***** ***** ***** ***** *****/   

void* DLLEXPORT PowerMeter_SetAuto( int hInstrumentHandle , int channel ) 
{
	STD_ERROR				StdError								=	{0};

	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Set Chanel State  ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerMeter_SetChanelState( int hInstrumentHandle , int channel , int state )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
		
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Set Vertical Scale  ***** ***** ***** ***** ***** ***** ***** *****/      

void* DLLEXPORT PowerMeter_SetVerticalScale( int hInstrumentHandle , int channel , double scale )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Set Vertical Center  ***** ***** ***** ***** ***** ***** ***** *****/   

void* DLLEXPORT PowerMeter_SetVerticalCenter( int hInstrumentHandle , int channel , double center )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Calibrate  ***** ***** ***** ***** ***** ***** ***** *****/   

void* DLLEXPORT PowerMeter_Calibrate( int hInstrumentHandle , int iChannel )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;

	char					szCommand[LOW_STRING]					=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szCommand , "CAL\n" );
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Zero  ***** ***** ***** ***** ***** ***** ***** *****/     

void* DLLEXPORT PowerMeter_Zero( int hInstrumentHandle , int iChannel )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;

	char					szCommand[LOW_STRING]					=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szCommand , "ZERO\n" );
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** SetUnits ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerMeter_SetUnits( int hInstrumentHandle , int channel , unsigned int units )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	switch(units)
	{
		case 1:
			
			CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "UNIT:POW W\n" ));
			break;
			
		default:	
			
			CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "UNIT:POW DBM\n" ));
			break;
	}
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Set Offset ***** ***** ***** ***** ***** ***** ***** *****/  

void* DLLEXPORT PowerMeter_SetOffset( int hInstrumentHandle , int iChannel , double lfOffset )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;

	char					szCommand[LOW_STRING]					=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szCommand , "PST:CCDF:SWE:OFF:TIME %E\n" , lfOffset );
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
		
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Set Cal Factor ***** ***** ***** ***** ***** ***** ***** *****/   

void* DLLEXPORT PowerMeter_SetCalFactor( int hInstrumentHandle , int channel , double CalFactor )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Set Video BW ***** ***** ***** ***** ***** ***** ***** *****/  

void* DLLEXPORT PowerMeter_SetVideoBW( int hInstrumentHandle , int channel ,  unsigned int VideoBW )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	switch(VideoBW)
	{
		case 1:
			CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "BAND:VID LOW\n" ));
			break;
			
		case 2:
			CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "BAND:VID MED\n" ));
			break;
			
		case 3:
			CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "BAND:VID HIGH\n" ));
			break;
			
		default:
			CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "BAND:VID OFF\n" ));
			break;	
	}
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
		
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Set Peak Hold ***** ***** ***** ***** ***** ***** ***** *****/  

void* DLLEXPORT PowerMeter_SetPeakHold( int hInstrumentHandle , int channel , int state )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
		
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Set Frequency ***** ***** ***** ***** ***** ***** ***** *****/   

void* DLLEXPORT PowerMeter_SetFrequency( int hInstrumentHandle , int iChannel , double lfFrequency )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;
	
	char					szCommand[LOW_STRING]					=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szCommand , "SENSe:FREQuency %E\n", lfFrequency );
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Set Mode ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerMeter_SetMode( int hInstrumentHandle , unsigned int Mode )
{ 
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** OPC ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerMeter_OPC( int hInstrumentHandle , double lfTimeOut , int *state )
{
	STD_ERROR					StdError							=	{0};
	
	tsHandle					*pLocalHandle						=	{0};

	int							iStatus								=	0;

	int							handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	iStatus = WaitForOperationCompletion( pLocalHandle->sessionHandle , lfTimeOut  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
	if ( state )
		*state = iStatus;
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Fetch CW power ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerMeter_FetchCWpower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	STD_ERROR					StdError							=	{0};
	
	tsHandle					*pLocalHandle						=	{0};

	int							handle								=	0;

	char						szReadBuffer[LOW_STRING]			=	{0};
	
	int							count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "FETC?\n" ));

	viRead( pLocalHandle->sessionHandle, szReadBuffer , LOW_STRING , &count );    

	if ( value )
		*value = atof(szReadBuffer);
		
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Read CW power ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerMeter_ReadCWpower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	STD_ERROR					StdError							=	{0};
	
	tsHandle					*pLocalHandle						=	{0};
														
	int							handle								=	0;
	
	char						szReadBuffer[LOW_STRING]			=	{0};
	
	int							count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "READ?\n" ));

	viRead( pLocalHandle->sessionHandle, szReadBuffer , LOW_STRING , &count );    

	if ( value )
		*value = atof(szReadBuffer);
		
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

/***** ***** ***** ***** ***** ***** ***** ***** Read Power ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerMeter_ReadPower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	STD_ERROR					StdError							=	{0};
	
	tsHandle					*pLocalHandle						=	{0};

	int							handle								=	0;
	
	char						szReadBuffer[LOW_STRING]			=	{0};
																										
	int							count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "READ?\n" ));

	viRead( pLocalHandle->sessionHandle, szReadBuffer , LOW_STRING , &count );    

	if ( value )
		*value = atof(szReadBuffer);   
		
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Fetch Power ***** ***** ***** ***** ***** ***** ***** *****/   

void* DLLEXPORT PowerMeter_FetchPower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	STD_ERROR					StdError							=	{0};
	
	tsHandle					*pLocalHandle						=	{0};

	int							handle								=	0;
	
	char						szReadBuffer[LOW_STRING]			=	{0};
	
	int							count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));

	viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "FETCh:SCALar:POWer:AC?\n" ));

	viRead( pLocalHandle->sessionHandle, szReadBuffer , LOW_STRING , &count );    

	if ( value )
		*value = atof(szReadBuffer);
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


/***** ***** ***** ***** ***** ***** ***** ***** Measure Power ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerMeter_MeasurePower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	STD_ERROR					StdError							=	{0};
															
	tsHandle					*pLocalHandle						=	{0};

	int							handle								=	0;
	
	char						szReadBuffer[LOW_STRING]			=	{0};
	
	int							count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "MEAS?\n" ));

	viRead( pLocalHandle->sessionHandle, szReadBuffer , LOW_STRING , &count );    

	if ( value )
		*value = atof(szReadBuffer);
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Measure Modulated Power ***** ***** ***** ***** ***** ***** ***** *****/  
  
void* DLLEXPORT PowerMeter_MeasureModulatedPower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	STD_ERROR					StdError							 =	{0};
	
	tsHandle					*pLocalHandle						 =	{0};

	int							handle								 =	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Fetch Marker Average ***** ***** ***** ***** ***** ***** ***** *****/    

void* DLLEXPORT PowerMeter_FetchMarkerAverage( int hInstrumentHandle , int iChannel , int marker , double timeout , double *value )
{
	STD_ERROR					StdError							=	{0};
	
	tsHandle					*pLocalHandle						=	{0};

	int							handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Fetch Marker Max ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerMeter_FetchMarkerMax( int hInstrumentHandle , int iChannel , int marker , double timeout , double *value )
{
	STD_ERROR					StdError							=	{0};

	tsHandle					*pLocalHandle						=	{0};

	int							handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Fetch Marker Min ***** ***** ***** ***** ***** ***** ***** *****/  

void* DLLEXPORT PowerMeter_FetchMarkerMin( int hInstrumentHandle , int iChannel , int marker , double timeout , double *value )
{
	STD_ERROR					StdError							=	{0};

	tsHandle					*pLocalHandle						=	{0};

	int							handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

 /***** ***** ***** ***** ***** ***** ***** ***** Set Active Channel ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerMeter_SetActiveChannel( int hInstrumentHandle , int iChannel )
{
	STD_ERROR					StdError							=	{0};
	
	tsHandle					*pLocalHandle						=	{0};

	int							handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

 /***** ***** ***** ***** ***** ***** ***** ***** Set Active Port ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerMeter_SetActivePort( int hInstrumentHandle , int iSensorNumber )
{
	STD_ERROR					StdError							=	{0};
	
	tsHandle					*pLocalHandle						=	{0};

	int							handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

 /***** ***** ***** ***** ***** ***** ***** ***** Gating Config Offset Time ***** ***** ***** ***** ***** ***** ***** *****/   

void* DLLEXPORT PowerMeter_Gating_ConfigOffsetTime( int hInstrumentHandle , int iChannel , int bGate , double lfTime , double lfOffset , double lfMidambleOffset, double lfMidambleLength )
{
	STD_ERROR					StdError							=	{0};
	
	tsHandle					*pLocalHandle						=	{0};

	int							handle								=	0;
	
	char						szCommand[LOW_STRING]				=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if ( bGate )
		bGate = 1;
	
	//   Set Time Window
	sprintf( szCommand , "SENSe%d:TRAC:TIME %E\n" , iChannel , lfTime);
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));   
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
	// Set Time Offset
	sprintf( szCommand , "SENSe%d:TRAC:OFFS:TIME %E\n" , iChannel , lfOffset);
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));   
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
	
	//  Set Gate Time
	sprintf( szCommand , "SENSe%d:SWEep%d:TIME %E\n" , iChannel, bGate, lfMidambleLength);
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));   
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout ); 
	
	// Set Gate Offset
	sprintf( szCommand , "SENSe%d:SWEep%d:OFFSet:TIME %E\n" , iChannel, bGate , lfMidambleOffset);
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));   
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout ); 
	
	//sprintf( szCommand , "PST:CCDF:SWE %d\n" , bGate );
	//CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));   

	//WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
	//sprintf( szCommand , "PST:CCDF:SWE:OFF:TIME %E\n" , lfOffset );
	//CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));
	
	//WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
	//sprintf( szCommand , "PST:CCDF:SWE:TIME %E\n" , lfTime );
	//CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));
	
	//WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

 /***** ***** ***** ***** ***** ***** ***** ***** Init Measurement ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerMeter_Init_Measurement( int hInstrumentHandle, int iChannel)
{
	STD_ERROR					StdError							=	{0};
	
	tsHandle					*pLocalHandle						=	{0};

	int							handle								=	0; 
	
	char						szCommand[LOW_STRING]				=	{0}; 
	
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
    //  Init  Measurement
	sprintf( szCommand , "INIT%d\n", iChannel);
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));   
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout ); 

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}
 /***** ***** ***** ***** ***** ***** ***** ***** Abort ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerMeter_Abort( int hInstrumentHandle, int iChannel)
{
	STD_ERROR					StdError							=	{0};
	
	tsHandle					*pLocalHandle						=	{0};

	int							handle								=	0; 
	
	char						szCommand[LOW_STRING]				=	{0}; 
	
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
    //  Init  Measurement
	sprintf( szCommand , "ABORt%d\n", iChannel);
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));   
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout ); 

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}
 /***** ***** ***** ***** ***** ***** ***** ***** Set Trigger ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerMeter_Set_Trigger_Source( int hInstrumentHandle, int iTriggerSorce )
{
	STD_ERROR					StdError							=	{0};
	
	tsHandle					*pLocalHandle						=	{0};

	int							handle								=	0; 
	
	char						szCommand[LOW_STRING]				=	{0}; 
	
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
    switch ( iTriggerSorce )
	{
		case BUS_TRIGGER_SOURCE:
			
			sprintf( szCommand , "TRIGger:SOURce BUS\n" );
			break;

 		case EXT_TRIGGER_SOURCE:
			sprintf( szCommand , "TRIGger:SOURce EXT\n" );
			break;

		case HOLD_TRIGGER_SOURCE:
			sprintf( szCommand , "TRIGger:SOURce HOLD\n" );
			break;

		case IMM_TRIGGER_SOURCE:
			sprintf( szCommand , "TRIGger:SOURce IMM\n" );
			break;

		case INT_TRIGGER_SOURCE:
			sprintf( szCommand , "TRIGger:SOURce INT\n" );
			break;

		default:
			sprintf( szCommand , "TRIGger:SOURce INT\n" );

	}

	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));   
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout ); 

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

 /***** ***** ***** ***** ***** ***** ***** ***** Gating Fetch Max Power ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerMeter_Gating_FetchMaxPower( int hInstrumentHandle , int iChannel, int iGate, double timeout , double *value )
{
	STD_ERROR					StdError							=	{0};
	
	tsHandle					*pLocalHandle						=	{0};

	int							handle								=	0;
	
	char						szReadBuffer[LOW_STRING]			=	{0};
	
	char						szCommand[LOW_STRING]				=	{0};   
	
	int							count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle )); 
	
	//  Config Measurement
	sprintf( szCommand , "CALCulate:FEED \"POW:PEAK ON SWEEP\"\n"); 
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
	
	DelayWithEventProcessing(0.1);       
	
	// Fetch Result
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "FETC?\n" ));

	viRead( pLocalHandle->sessionHandle, szReadBuffer , LOW_STRING , &count ); 

	RemoveSurroundingWhiteSpace(szReadBuffer);
	
	if ( value )
		*value = atof(szReadBuffer);
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

 /***** ***** ***** ***** ***** ***** ***** ***** Gating Fetch Average Power ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerMeter_Gating_FetchAveragePower( int hInstrumentHandle , int iChannel, int iGate, double timeout , double *value )
{
	STD_ERROR					StdError							=	{0};
	
	tsHandle					*pLocalHandle						=	{0};

	int							handle								=	0;
	
	char						szReadBuffer[LOW_STRING]			=	{0};
	
	char						szCommand[LOW_STRING]				=	{0};   
	
	int							count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle )); 
	
	//  Config Measurement
	sprintf( szCommand , "CALCulate:FEED \"POW:AVER ON SWEEP\"\n"); 
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));   
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
	
	DelayWithEventProcessing(0.1);       
	
	// Fetch Result
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "FETC?\n" ));

	viRead( pLocalHandle->sessionHandle, szReadBuffer , LOW_STRING , &count ); 

	
	RemoveSurroundingWhiteSpace(szReadBuffer);

	if ( value )									
		*value = atof(szReadBuffer);
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

 /***** ***** ***** ***** ***** ***** ***** ***** Gating Fetch Min Power ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerMeter_Gating_FetchMinPower( int hInstrumentHandle , int iChannel, int iGate, double timeout , double *value )
{
	STD_ERROR					StdError							=	{0};
	
	tsHandle					*pLocalHandle						=	{0};

	int							handle								=	0;
	
	char						szReadBuffer[LOW_STRING]			=	{0};
	
	char						szCommand[LOW_STRING]				=	{0};   
	
	int							count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));  
	
	//  Config Measurement
	sprintf( szCommand , "CALCulate:FEED \"POW:MIN ON SWEEP\"\n"); 
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));   
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
	
	DelayWithEventProcessing(0.1);       
	
	// Fetch Result
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "FETC?\n" ));

	viRead( pLocalHandle->sessionHandle, szReadBuffer , LOW_STRING , &count );  
	
	
	RemoveSurroundingWhiteSpace(szReadBuffer);

	if ( value )
		*value = atof(szReadBuffer);
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 


 /***** ***** ***** ***** ***** ***** ***** ***** Configure Marker ***** ***** ***** ***** ***** ***** ***** *****/  

void* DLLEXPORT PowerMeter_Configure_Marker( int hInstrumentHandle , int iChannel , int iMarkerNumber , int bEnable , double lfPosition )
{
	STD_ERROR					StdError							=	{0};
	
	tsHandle					*pLocalHandle						=	{0};

	int							handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

 /***** ***** ***** ***** ***** ***** ***** ***** Read Marker ***** ***** ***** ***** ***** ***** ***** *****/  

void* DLLEXPORT PowerMeter_Read_Marker( int hInstrumentHandle , int iChannel , int iMarkerNumber , double timeout , double *vlfPosition , double *vlfPower )
{
	STD_ERROR					StdError							=	{0};
	
	tsHandle					*pLocalHandle						=	{0};

	int							handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

 /***** ***** ***** ***** ***** ***** ***** ***** Send File ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerMeter_SendFile( int hInstrumentHandle ,char *szLocalFileName ,char *szStateFileName  )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szLoadSpectrumState[512]			=	{0},
							szDirectoryPath[512]				=	{0},
							szFileSize[32]						=	{0};
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
	
	int						iFileSize							=	0,
							iReadSize							=	0, 
							iWriteCount							=	0,
							iFileHandle							=	0;
	
	char					*pFileBuffer						=	NULL,
							*pTemp								=	NULL; 
							
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	IF (( szLocalFileName == NULL ), "File Name is empty" );
	
	IF (( szStateFileName == NULL ), "State File Name is empty" ); 
	
	IF (( FileExists(szLocalFileName , &iFileSize) == 0 ), "Local File Name is not exists." );     
	
	sprintf( szFileSize , "%d" , iFileSize );
	
	CALLOC_ERR( pFileBuffer ,iFileSize , sizeof(char)); 
	
	iFileHandle = OpenFile ( szLocalFileName , VAL_READ_ONLY, VAL_TRUNCATE, VAL_BINARY);
																												
	IF ( ( iFileHandle == 0 ) , "Can't open local file." );
	
	iReadSize = ReadFile ( iFileHandle , pFileBuffer , iFileSize );

	IF ((( iReadSize != iFileSize ) || ( pFileBuffer == NULL ) ), "Read File error" ); 
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );
	
	strcpy( szDirectoryPath , szStateFileName );
	
	pTemp = strrchr( szDirectoryPath , '\\' );
	
	if ( pTemp )
	{
		*pTemp = 0;
	}
	
	if ( strstr( szDirectoryPath , ":\\" ))
	{
		RecursiveMakeDirectory( pLocalHandle->sessionHandle , szDirectoryPath );
	}
	
	viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );
	
	sprintf( szLoadSpectrumState , ":MMEM:DATA \"%s\",#%d%s" , szStateFileName , strlen(szFileSize) , szFileSize );   

	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szLoadSpectrumState ));
	
	CHK_VSA ( viBufWrite( pLocalHandle->sessionHandle , pFileBuffer , iFileSize ,&iWriteCount ));
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "\n" )); 

Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	FREE(pFileBuffer);
	
	if ( iFileHandle )
		CloseFile ( iFileHandle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

 /***** ***** ***** ***** ***** ***** ***** ***** Send State File ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT Equipment_SendStateFile ( int hInstrumentHandle , char *szLocalFileName ,char *szStateFileName  )
{
	return PowerMeter_SendFile( hInstrumentHandle , szLocalFileName , szStateFileName );	
}

 /***** ***** ***** ***** ***** ***** ***** ***** Receive File ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerMeter_ReceiveFile( int hInstrumentHandle ,char *szLocalFileName ,char *szStateFileName  )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szLoadSpectrumState[512]			=	{0},
							szReadFeedback[LOW_STRING]			=	{0};
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
	
	int						iWriteSize						 	=	0,  
							iFileHandle							=	0;
	
	char					*pReadBuffer						=	NULL; 
							
	unsigned int			uiCount								=	0,
							uiNumberOfBinaryData				=	0;
							
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	IF (( szLocalFileName == NULL ), "File Name is empty" );
	
	IF (( szStateFileName == NULL ), "State File Name is empty" ); 
	
	iFileHandle = OpenFile ( szLocalFileName , VAL_WRITE_ONLY, VAL_TRUNCATE, VAL_BINARY);
	
	IF ( ( iFileHandle == 0 ) , "Can't open local file." );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );
	
	sprintf( szLoadSpectrumState , ":MMEM:DATA? \"%s\"\n" , szStateFileName );   

	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szLoadSpectrumState ));
	
	do 
	{
		SetBreakOnLibraryErrors (0); 
		viRead ( pLocalHandle->sessionHandle ,(ViPBuf)szReadFeedback , 1 , &uiCount );
		SetBreakOnLibraryErrors (1); 

		if ( szReadFeedback[0] == '#' )
			break;

	} while(uiCount);

	IF (( szReadFeedback[0] != '#' ) , "Wrong trace reading format" );  

	memset( szReadFeedback , 0 , LOW_STRING );

	SetBreakOnLibraryErrors (0); 
	viRead ( pLocalHandle->sessionHandle ,(ViPBuf)szReadFeedback , 1 , &uiCount );
	SetBreakOnLibraryErrors (1); 

	IF (( uiCount == 0 ) , "Wrong trace reading format" );  

	uiCount = atoi(szReadFeedback); 

	memset( szReadFeedback , 0 , LOW_STRING );

	SetBreakOnLibraryErrors (0); 
	viRead ( pLocalHandle->sessionHandle ,(ViPBuf)szReadFeedback , uiCount , &uiCount );  
	SetBreakOnLibraryErrors (1); 

	uiNumberOfBinaryData = atoi(szReadFeedback); 

	SetBreakOnLibraryErrors (0);

	CALLOC_ERR( pReadBuffer , uiNumberOfBinaryData , sizeof(char)); 

	viRead ( pLocalHandle->sessionHandle ,(ViPBuf)pReadBuffer , uiNumberOfBinaryData , &uiCount ); 

	SetBreakOnLibraryErrors (1);

	IF (( uiNumberOfBinaryData != uiCount ) , "Wrong trace reading format" );     

	iWriteSize = WriteFile ( iFileHandle , pReadBuffer , uiNumberOfBinaryData );

	IF (( iWriteSize != uiNumberOfBinaryData ) , "Write File error" ); 

	SetBreakOnLibraryErrors (0);
	viRead ( pLocalHandle->sessionHandle ,(ViPBuf)szReadFeedback , 1 , &uiCount );  
	SetBreakOnLibraryErrors (1);
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	FREE(pReadBuffer);
	
	if ( iFileHandle )
		CloseFile ( iFileHandle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

 /***** ***** ***** ***** ***** ***** ***** ***** Receive State File  ***** ***** ***** ***** ***** ***** ***** *****/   

void* DLLEXPORT Equipment_ReceiveStateFile ( int hInstrumentHandle , char *szLocalFileName ,char *szStateFileName  )
{
	return PowerMeter_ReceiveFile( hInstrumentHandle , szLocalFileName , szStateFileName );	
}

 /***** ***** ***** ***** ***** ***** ***** ***** Get File Catalog   ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerMeter_GetFileCatalog ( int hInstrumentHandle , char *szPath , char ***szFileNames , int **pvFileSizes , int *piNumberOfFiles )
{
	STD_ERROR               StdError                  			=   {0};        

	char					szLoadSpectrumCatalog[512]			=	{0},
							szPreviousFeedback[512]				=	{0},
							szReadFeedback[LOW_STRING]			=	{0}, 
							szCurrentBuffer[512]				=	{0},
							szLoadPath[512]						=	{0};  
																	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
	
	int						iFileIndex							=	0;
	
	char					*pTemp								=	NULL,
							*pCurrent							=	NULL,
							*pNext								=	NULL; 
							
	int 					iLength								=	0, 
							iNumberOfFile						=	0,
							iAllocatedMemory					=	0;

	unsigned int			uiCount								=	0,
							uiCurrentCount						=	0,
							uiPreviousCount						=	0;
								
	int						iTimeout							=	0;
							
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );

	if ( szPath )
	{
		strcpy( szLoadPath , szPath );
		
		iLength = strlen(szLoadPath);
		
		if ( szLoadPath[iLength-1] != '\\' )
			strcat( szLoadPath , "\\" );
		
		sprintf( szLoadSpectrumCatalog , ":MMEM:CAT? \"%s\"\n" , szLoadPath );   
	}
	else
		sprintf( szLoadSpectrumCatalog , ":MMEM:CAT? \"C:\\\"\n" );   
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szLoadSpectrumCatalog ));
	
	viGetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , &iTimeout );    

	viSetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , 2000 );  

	do
	{
		strcpy( szCurrentBuffer , szPreviousFeedback );

		memset( szReadFeedback , 0 , LOW_STRING );

		SetBreakOnLibraryErrors (0); 
		viRead ( pLocalHandle->sessionHandle ,(ViPBuf)szReadFeedback , (LOW_STRING-1) , &uiCount );  
		SetBreakOnLibraryErrors (1); 

		if (( uiCount == 0 ) && ( uiPreviousCount == uiCurrentCount ))
			break;

		strcat( szCurrentBuffer , szReadFeedback ); 
   
		uiPreviousCount = uiCurrentCount;

		uiCurrentCount = strlen( szCurrentBuffer );  

		pNext = szCurrentBuffer;

		do
		{
			strcpy( szPreviousFeedback , pNext );
	
			pTemp = strstr( pNext , ",\"" );
	
			if ( pTemp == NULL )
				break;
	
			pCurrent = pTemp+2;
	
			pTemp = strstr( pCurrent , ",," );   
	
			if ( pTemp == NULL )
				break;
	
			*pTemp = 0;
	
			pNext = pTemp+2;
	
			iNumberOfFile++;
	
			if ( iNumberOfFile >= iAllocatedMemory )
			{
				iAllocatedMemory = ( iAllocatedMemory * 2 ) + 8;
		
				if ( szFileNames )
				{
					*szFileNames = realloc( *szFileNames , ( iAllocatedMemory * sizeof(char*)));
				}
		
				if ( pvFileSizes )
				{
					*pvFileSizes = realloc( *pvFileSizes , ( iAllocatedMemory * sizeof(int))); 
				}
			}
	
			CALLOC_COPY_STRING( (*szFileNames)[iFileIndex] , pCurrent );
	
			if ( pvFileSizes )
			{
				if ( strchr( pCurrent , '[' ) == NULL )
					(*pvFileSizes)[iFileIndex] = atoi(pNext);
				else
					(*pvFileSizes)[iFileIndex] = -1;
			}
	
			iFileIndex++;
	
		} while(1);

	} while(1);

Error: 

	if ( piNumberOfFiles )
		*piNumberOfFiles = iNumberOfFile;
	
	if ( handle )
	{
		if ( pLocalHandle->sessionHandle )
			viSetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , iTimeout );  
		
		CmtReleaseTSVPtr ( handle );  
	}
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

 /***** ***** ***** ***** ***** ***** ***** ***** Get File Catalog  ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT Equipment_GetFileCatalog( int hInstrumentHandle , char *szPath , char ***szFileNames , int **pvFileSizes , int *piNumberOfFiles )  
{
	return PowerMeter_GetFileCatalog( hInstrumentHandle , szPath , szFileNames , pvFileSizes , piNumberOfFiles );	
}

 /***** ***** ***** ***** ***** ***** ***** ***** Make File Catalog  ***** ***** ***** ***** ***** ***** ***** *****/   

void* DLLEXPORT Equipment_MakeFileCatalog( int hInstrumentHandle , char *szPath )  
{
	return RecursiveMakeDirectory( hInstrumentHandle , szPath );	
}

 /***** ***** ***** ***** ***** ***** ***** ***** Delete File  ***** ***** ***** ***** ***** ***** ***** *****/ 
   
void* DLLEXPORT PowerMeter_DeleteFile( int hInstrumentHandle , char *szStateFileName  )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szDeleteSpectrumState[512]			=	{0},
							szMessage[LOW_STRING]				=	{0};
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;

	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );

	IF (( szStateFileName == NULL ), "State File Name is empty" ); 
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );

	sprintf( szDeleteSpectrumState , ":MMEM:DEL \"%s\"\n" , szStateFileName );   
	
	sprintf( szMessage , "Are you sure you want to delete state file\n\"%s\"?" , szStateFileName );

	if ( ConfirmPopup ("Deleting Equipment State File", szMessage) )
	{
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szDeleteSpectrumState ));
	}

Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

 /***** ***** ***** ***** ***** ***** ***** ***** Delete State File  ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT Equipment_DeleteStateFile ( int hInstrumentHandle , char *szStateFileName  )
{
	return PowerMeter_DeleteFile( hInstrumentHandle , szStateFileName );	
}

 /***** ***** ***** ***** ***** ***** ***** ***** Delete File Catalog  ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerMeter_DeleteFileCatalog( int hInstrumentHandle , char *szCatalogName  )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szDeleteSpectrumCatalog[512]		=	{0},
							szMessage[LOW_STRING]				=	{0};
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;

	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );

	IF (( szCatalogName == NULL ), "State File Name is empty" ); 
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));

	viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );

	sprintf( szDeleteSpectrumCatalog , ":MMEM:RDIR \"%s\"\n" , szCatalogName );   
	
	sprintf( szMessage , "Are you sure you want to delete catalog \n\"%s\"?" , szCatalogName );

	if ( ConfirmPopup ("Deleting Equipment State File", szMessage) )
	{
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szDeleteSpectrumCatalog ));
	}
		
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

 /***** ***** ***** ***** ***** ***** ***** ***** Delete File Catalog  ***** ***** ***** ***** ***** ***** ***** *****/   

void* DLLEXPORT Equipment_DeleteFileCatalog ( int hInstrumentHandle , char *szCatalogName  )
{
	return PowerMeter_DeleteFileCatalog( hInstrumentHandle , szCatalogName );	
}

 /***** ***** ***** ***** ***** ***** ***** ***** Recall State  ***** ***** ***** ***** ***** ***** ***** *****/   

void* DLLEXPORT PowerMeter_RecallState( int hInstrumentHandle ,char *szFileName )  
{
	STD_ERROR           		    StdError                  						=   {0};        

	char							szLoadSpectrumState[512]						=	{0},
									szReadBuffer[LOW_STRING]						=	{0};
	
	tsHandle						*pLocalHandle									=	{0};

	int								handle											=	0;
	
	int								iError											=	0;
	
	char							*pFileNamePath									=	NULL;
	
	tsSTD_CallBackSet				tSTD_CallBackSet								=	{0};
	
	int								iTryIndex										=	0;
	
	int								count											=	0;
	
	int								bGetLock										=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	bGetLock = 1;
	
	STDF_UPDATE_CALLBACK_DATA(pLocalHandle->ptCallbacks);   
	
	for ( iTryIndex = 0; iTryIndex < 3; iTryIndex++ )
	{
		viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );
		
		DelayWithEventProcessing(1); 
	
		sprintf( szLoadSpectrumState , "*RCL %s\n" , szFileName ); 
		//sprintf( szLoadSpectrumState , ":MMEM:LOAD:STAT \"%s\"\n" , szFileName );

		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szLoadSpectrumState ));
	
		DelayWithEventProcessing(1);
	
		viPrintf( pLocalHandle->sessionHandle , ":SYST:ERR?\n" );

		SetBreakOnLibraryErrors (0);
		viRead( pLocalHandle->sessionHandle, szReadBuffer , LOW_STRING , &count );
		SetBreakOnLibraryErrors (1);

		iError = atoi(szReadBuffer);

		if ( iError != -256 )
		{
			FREE_CALLOC_COPY_STRING(pLocalHandle->pLastStateFile,szFileName); 
			
			break;
		}
		
		STDF_GET_EQUIPMENT_STATE_FILE( szFileName , pFileNamePath );
		
		CHK_CMT ( CmtReleaseTSVPtr ( handle )); 
		bGetLock = 0;
		
		FREE_STDERR_COPY_ERR( PowerMeter_SendFile( handle , pFileNamePath , szFileName ));    
		
		FREE(pFileNamePath);
		
		CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
		bGetLock = 1;
	}																   
	
Error: 

	if ( handle && bGetLock )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 


 /***** ***** ***** ***** ***** ***** ***** ***** Save State ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerMeter_SaveState( int hInstrumentHandle ,char *szFileName )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szLoadSpectrumState[512]			=	{0};
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );
	
	sprintf( szLoadSpectrumState , "*SAV %s\n" , szFileName ); 
	//sprintf( szLoadSpectrumState , ":MMEM:STOR:STAT \"%s\"\n" , szFileName );   

	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szLoadSpectrumState ));
	
	FREE_CALLOC_COPY_STRING(pLocalHandle->pLastStateFile,szFileName); 

Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

 /***** ***** ***** ***** ***** ***** ***** ***** Recall ***** ***** ***** ***** ***** ***** ***** *****/       

void* DLLEXPORT PowerMeter_Recall( int hInstrumentHandle ,char *szFileName )  
{
	return PowerMeter_RecallState( hInstrumentHandle ,szFileName );
} 

 /***** ***** ***** ***** ***** ***** ***** ***** Get Last State File Name ***** ***** ***** ***** ***** ***** ***** *****/  

void* DLLEXPORT   PowerMeter_GetLastStateFileName(int hInstrumentHandle, char **pszFileName )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};  

	int				handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	IF (( pszFileName == NULL ) , "State is empty" ); 
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	FREE_CALLOC_COPY_STRING(*pszFileName,pLocalHandle->pLastStateFile); 
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
	
}

 /***** ***** ***** ***** ***** ***** ***** ***** Get Trace ***** ***** ***** ***** ***** ***** ***** *****/ 

void* DLLEXPORT PowerMeter_GetTrace( int hInstrumentHandle, int iChannel, int timeout, double **pTime  , double **pTrace , int numberOfPoints , int *piNumberOfPoints )
{
 	STD_ERROR				StdError						=	{0};   
	
	int						iNumberOfPoints					=	0; 
	
	double 					*plfLocalTime   				=	NULL;  
							
	double 					*plfLocalTrace					=	NULL;

	unsigned char			*pReadBuffer					=	NULL; 
								
	unsigned int			uiCount							=	0;
	
	unsigned int			uiNumberOfBinaryData			=	0;
	
	int						iIndex      					=	0;
	
	int						iBufferIndex					=	0;
	
	int						iTraceIndex	    				=	0;
	
	char        			szCommand[512]	           		=	{0}; 
	
	char					szReadFeedback[LOW_STRING]		=	{0};
	
	tsHandle				*pLocalHandle					=	{0};

	int						handle							=	0;
	
	tuConvert				ConvertBuffer					=	{0}; 
	
	int						valueSize						=   4;
	
	double					doubleValue						=	0.0; 
	
	float					floatValue						=	0.0; 
	
	unsigned char			charValue						=	0;
	
	double					startTime						=	0.0;
	
	double					actualTime						=	0.0; 
	
	double					timeWindow						=	0.0;   
	
	double					windowStep						=	0.0;  

	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle; 
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	 
	viPrintf( pLocalHandle->sessionHandle , "*WAI\n" ); 
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
	
	sprintf( szCommand , "TRAC%d:DATA? HRES\n" , iChannel );
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand )); 

	do 
	{
		SetBreakOnLibraryErrors (0); 
		viRead ( pLocalHandle->sessionHandle ,(ViPBuf)szReadFeedback , 1 , &uiCount );
		SetBreakOnLibraryErrors (1); 

		if ( szReadFeedback[0] == '#' )
			break;

	} while(uiCount);

	IF (( szReadFeedback[0] != '#' ) , "Wrong trace reading format" );  

	memset( szReadFeedback , 0 , LOW_STRING );

	SetBreakOnLibraryErrors (0); 
	viRead ( pLocalHandle->sessionHandle ,(ViPBuf)szReadFeedback , 1 , &uiCount );
	SetBreakOnLibraryErrors (1); 

	IF (( uiCount == 0 ) , "Wrong trace reading format" );  

	uiCount = atoi(szReadFeedback); 

	memset( szReadFeedback , 0 , LOW_STRING );

	SetBreakOnLibraryErrors (0); 
	viRead ( pLocalHandle->sessionHandle ,(ViPBuf)szReadFeedback , uiCount , &uiCount );  
	SetBreakOnLibraryErrors (1); 
	
	uiNumberOfBinaryData = atoi(szReadFeedback); 

	CALLOC_ERR( pReadBuffer , uiNumberOfBinaryData , sizeof(char)); 

	SetBreakOnLibraryErrors (0);
	viRead ( pLocalHandle->sessionHandle ,(ViPBuf)pReadBuffer , uiNumberOfBinaryData , &uiCount );    
	SetBreakOnLibraryErrors (1);
	
	IF (( uiNumberOfBinaryData != uiCount ) , "Wrong trace reading format" ); 
	
	iNumberOfPoints =  uiCount / valueSize;

	CALLOC_ERR( plfLocalTrace , iNumberOfPoints , sizeof(double));  
	CALLOC_ERR( plfLocalTime , iNumberOfPoints , sizeof(double));    

	for ( iTraceIndex = 0; iTraceIndex < iNumberOfPoints; iTraceIndex++ )
	{
		
		for ( iIndex = 1; iIndex <= valueSize; iIndex++ )
		{ 
			charValue =  pReadBuffer[iBufferIndex];
			
			ConvertBuffer.buffer[valueSize - iIndex] =  charValue;
			
			iBufferIndex ++;
		} 
		
		floatValue =  ConvertBuffer.fWord;
		
		doubleValue = (double) floatValue;
		
		plfLocalTrace[iTraceIndex] = doubleValue;
	}
	
	sprintf( szCommand , "SENSe%d:TRACe:OFFSet:TIME?\n" , iChannel );      
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));

	SetBreakOnLibraryErrors (0); 
	viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &uiCount );  
	SetBreakOnLibraryErrors (1);   

	startTime = atof(szReadFeedback); 
	
	sprintf( szCommand , "SENSe%d:TRACe:TIME?\n" , iChannel );      
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));

	SetBreakOnLibraryErrors (0);
	viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &uiCount );
	SetBreakOnLibraryErrors (1);   

	timeWindow = atof(szReadFeedback);
	
	if( timeWindow )
	{
		windowStep =  timeWindow /  iNumberOfPoints;
		actualTime = startTime;
	
		for ( iTraceIndex = 0; iTraceIndex < iNumberOfPoints; iTraceIndex++ )
		{
			plfLocalTime[iTraceIndex] =  actualTime;
			
			actualTime += windowStep;
		}
	}
	
	
Error: 
	   
	if (pTrace)
		*pTrace = plfLocalTrace; 
	
	if (pTime)
		*pTime = plfLocalTime;
		
	
	if ( piNumberOfPoints )
		*piNumberOfPoints = iNumberOfPoints;
	
	FREE(plfLocalTrace);
	FREE(plfLocalTime); 
	FREE(pReadBuffer);
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);   
}

 /***** ***** ***** ***** ***** ***** ***** ***** Wait For Operation Completion ***** ***** ***** ***** ***** ***** ***** *****/   

int		WaitForOperationCompletion( int sessionHandle , double lfTimeout , double lfLowLevelTimeout )
{
	double			lfStartTime						=	0.0,
					lfCurrentTime					=	0.0;

	int				iOpcStatus						=	0;
	
	char			szReadFeedback[LOW_STRING]		=	{0};
	
	int				count							=	0;
	
	int				iTimeout						=	0;
	
	int				iError							=	0;
	
	viGetAttribute ( sessionHandle , VI_ATTR_TMO_VALUE , &iTimeout );   
	
	SetBreakOnLibraryErrors (0); 
	
	viSetAttribute ( sessionHandle , VI_ATTR_TMO_VALUE , (lfLowLevelTimeout*1E3) );  
	
	GetCurrentDateTime(&lfStartTime);
	
	do
	{
		viPrintf( sessionHandle , "*OPC?\n" );
						
		viRead( sessionHandle, szReadFeedback , (LOW_STRING-1) , &count );
	
		iOpcStatus = atoi(szReadFeedback);
		
		if ( iOpcStatus )
			break;
		
		GetCurrentDateTime(&lfCurrentTime);
		
	} while((lfCurrentTime-lfStartTime) < lfTimeout );
	
	if ( iOpcStatus )
	{
		viPrintf( sessionHandle , ":SYST:ERR?\n" );
		SetBreakOnLibraryErrors (0);
		viRead( sessionHandle, szReadFeedback , LOW_STRING , &count );
		SetBreakOnLibraryErrors (1);
	
		iError = atoi(szReadFeedback);
	
		if ( iError == -420 )
		{
			viPrintf( sessionHandle , "*CLS\n" );  
		}
	}
	
	viSetAttribute ( sessionHandle , VI_ATTR_TMO_VALUE , iTimeout );  
	
	SetBreakOnLibraryErrors (1); 
	
	return iOpcStatus;
}

 /***** ***** ***** ***** ***** ***** ***** ***** Recursive Make Directory ***** ***** ***** ***** ***** ***** ***** *****/    

int		RecursiveMakeDirectory( int sessionHandle , char *pPath )
{
	char		szCurrentPath[STD_STRING]		=	{0},
				szReadFeedback[STD_STRING]		=	{0},
				*pTemp							=	NULL;
														
	char		szEmptyDirectory[]				=	{"0,0,\"\"\n"};
	
	int			count							=	0;
	
	int			status							=	0;
	
	if ( pPath == NULL )
		return -1;
	
	strcpy( szCurrentPath , pPath );
	
	viPrintf( sessionHandle , "MMEM:CAT? \"%s\"\n" , szCurrentPath );
	
	SetBreakOnLibraryErrors (0);
	viRead( sessionHandle, szReadFeedback , (STD_STRING-1) , &count );
	SetBreakOnLibraryErrors (1);
	
	if ( strcmp( szReadFeedback , szEmptyDirectory ) == 0 ) 
	{
		pTemp = strrchr( szCurrentPath , '\\' );
		
		if ( pTemp )
		{
			pTemp++;
			*pTemp = 0;
			
			RecursiveMakeDirectory( sessionHandle , szCurrentPath );
		}														
		
		if ( strchr( pPath , '.' ) == NULL )
		{
			if ( strlen(pPath) )
			{
				viPrintf( sessionHandle , "MMEM:MDIR \"%s\"\n" , pPath );  
				   
				viPrintf( sessionHandle , "MMEM:CAT? \"%s\"\n" , pPath );
	
				SetBreakOnLibraryErrors (0);
				viRead( sessionHandle, szReadFeedback , (STD_STRING-1) , &count );
				SetBreakOnLibraryErrors (1);
				
				status = strcmp( szReadFeedback , szEmptyDirectory );
			}
			
			return status;
		}
	}

	return 0;
}

  /***** ***** ***** ***** ***** ***** ***** ***** MAIN ***** ***** ***** ***** ***** ***** ***** *****/      
/*

int main(void)
{
	int					InstrumentHandle			=	0,
						InstrumentConnectStatus		=	0,
						InstrumentChannelsQuantity	=	0; 
	
    double				*plfTrace					=	NULL; 
	double				*plfTime					=	NULL;  
	
	int                 iActualyPointsNumber		=   0,
						iStatus						=	0, 
						iMinimumIndex				=	0, 
						iMaximumIndex				=	0;
	
	double				lfMaximumValue				=	0.0,
						lfMinimumValue				=	0.0,
						lfMaxPower		  			=	0.0,
						lfAveragePower	  			=	0.0,
						lfMinPower		  			=	0.0;
	
	
	PowerMeter_Init( 0 , "USB0::0x2A8D::0x1C01::MY55230008::INSTR" , &InstrumentHandle , &InstrumentConnectStatus , &InstrumentChannelsQuantity );
	
	PowerMeter_Reset ( InstrumentHandle ); 
	PowerMeter_RecallState( InstrumentHandle, "1" ); 
	PowerMeter_Gating_ConfigOffsetTime( InstrumentHandle , 1 , 1 , 60.0E-6 , -5.0E-6 , 45.0E-6, 2.5E-6 );
	PowerMeter_SetFrequency ( InstrumentHandle , 0,  2.2E9 );
	PowerMeter_InitSweep( InstrumentHandle, 1 );
	
	PowerMeter_Gating_FetchMaxPower( InstrumentHandle, 1 , 1 , 5.0 , &lfMaxPower );
	PowerMeter_Gating_FetchAveragePower( InstrumentHandle, 1 , 1 , 5.0 , &lfAveragePower );
	PowerMeter_Gating_FetchMinPower( InstrumentHandle, 1 , 1 , 5.0 , &lfMinPower );
	
	DRV_PowerMeter_GetTrace( InstrumentHandle, 1 , (10 * 1000), &(plfTime) , &(plfTrace) , (20 * 1000) , &iActualyPointsNumber ); 
	iStatus = MaxMin1D (plfTrace, iActualyPointsNumber, &lfMaximumValue, &iMaximumIndex, &lfMinimumValue , &iMinimumIndex); 

	PowerMeter_Close( &InstrumentHandle );
	
	return 0;
}

*/
