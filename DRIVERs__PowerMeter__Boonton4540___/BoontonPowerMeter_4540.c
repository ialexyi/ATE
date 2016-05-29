//==================================================================================================================================

//                                      Include files 

//==================================================================================================================================   

#include <windows.h>
#include "toolbox.h"
#include <cvirte.h>
#include <userint.h>
#include <visa.h>
#include <utility.h>
#include "GlobalDefines.h"
#include <analysis.h>
#include <formatio.h>				   

//==================================================================================================================================

//                                      Variables

//================================================================================================================================== 


//==================================================================================================================================

//                                      Types

//==================================================================================================================================

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


//==================================================================================================================================

//                                      DLL main entry-point functions 

//==================================================================================================================================

// Dll Main
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

//  Dll Entry Point
int __stdcall DllEntryPoint (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    /* Included for compatibility with Borland */

    return DllMain (hinstDLL, fdwReason, lpvReserved);
}

//==================================================================================================================================

//                                      Internal functions

//==================================================================================================================================

int SplitStringByDelimiter( char * str, char delim, char ***array, int *length ) 
{
  char *p;
  char **res;
  int count = 1;
  int index = 0;

  p = str;
  
  // Count occurance of delim in string
  while( (p=strchr(p,delim)) != NULL ) 
  {
    *p = 0; // Null terminate the deliminator.
    p++; // Skip past our new null
    count++;
  }

  // allocate dynamic array
  res = calloc( 1, count * sizeof(char *));
  
  if( !res ) return -1;

  p = str;
  
  for( index =0; index < count; index++ )
  {
    if( *p )
		res[index] = p;
	
	// Copy start of string
    p = strchr(p, 0 );    // Look for next null
	
    p++; // Start of next string
  } 
 

  *array = res;
  *length = count;

  return 0;
} 

// Wait For Operation Completion
int		WaitForOperationCompletion( int sessionHandle , double lfTimeout , double lfLowLevelTimeout )
{
	double			lfStartTime						=	0.0,
					lfCurrentTime					=	0.0;

	int				iOpcStatus						=	0;
	
	char			szCommandName[LOW_STRING]  		=	{0}; 
	
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
		strcpy(szCommandName, "*OPC?\n");
		
		viWrite( sessionHandle , szCommandName, strlen(szCommandName), &count);
						
		viRead( sessionHandle, szReadFeedback , (LOW_STRING-1) , &count );
	
		iOpcStatus = atoi(szReadFeedback);
		
		if ( iOpcStatus )
			break;
		
		GetCurrentDateTime(&lfCurrentTime);
		
	} while((lfCurrentTime-lfStartTime) < lfTimeout );
	
	if ( iOpcStatus )
	{
		strcpy(szCommandName, ":SYST:ERR?\n");
		viWrite( sessionHandle , szCommandName, strlen(szCommandName), &count);
		
		SetBreakOnLibraryErrors (0);
		
		viRead( sessionHandle, szReadFeedback , LOW_STRING , &count );
		
		SetBreakOnLibraryErrors (1);
	
		iError = atoi(szReadFeedback);
	
		if ( iError == -420 )
		{
			strcpy(szCommandName, "*CLS\n");
		
			viWrite( sessionHandle , szCommandName, strlen(szCommandName), &count);
		}
	}
	
	viSetAttribute ( sessionHandle , VI_ATTR_TMO_VALUE , iTimeout );  
	
	SetBreakOnLibraryErrors (1); 
	
	return iOpcStatus;
} 


//==================================================================================================================================

//                                      Wrapper functions

//==================================================================================================================================

//  Config Copy STD Call Back Set
void*	DLLEXPORT		Config_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet )
{
	STD_ERROR						StdError									=	{0};
	
	tsHandle						*pLocalHandle								=	NULL;			
	
	int								handle										=	0;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );
	
	if ( *pHandle == 0 )
	{
		CHK_CMT(CmtNewTSV ( sizeof(tsHandle) , pHandle ));
	}
	
	handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if ( pLocalHandle->ptCallbacks  == NULL )
	{
		CALLOC( pLocalHandle->ptCallbacks , 1 , sizeof(tsSTD_CallBackSet)); 
	}
	
	memcpy( pLocalHandle->ptCallbacks , pCallBackSet , sizeof(tsSTD_CallBackSet));  

Error:
	
	if ( handle )
		CmtReleaseTSVPtr (handle);
	
	RETURN_STDERR_POINTER;
}

// Power Meter Get Error Text Message
void* DLLEXPORT PowerMeter_GetErrorTextMessage ( int hInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char			szCommandName[LOW_STRING]			=	{0},
					szReadBuffer[LOW_STRING]			=	{0},
					szErrorDescription[LOW_STRING]		=	{0};
	
	int				count								=	0;
	
	char			*pTemp								=	NULL;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	SetBreakOnLibraryErrors (0); 
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle )); 
	
	strcpy(szCommandName, "SYSTem:ERRor?\n"); 
	viWrite( pLocalHandle->sessionHandle , szCommandName, strlen(szCommandName), &count); 

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
	
	strcpy(szCommandName, "*CLS"); 
	viWrite( pLocalHandle->sessionHandle , szCommandName, strlen(szCommandName), &count); 
	
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

// Equipment Info
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
		IF (( strstr( szIdentificationText , "BOONTON ELECTRONICS" ) == NULL ) , "Is not supported" );
	
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


//  Equipment Get Low Level Handle
void*	DLLEXPORT		Equipment_GetLowLevelHandle( int hHandle , int *phLowLevelHandle )
{
	STD_ERROR						StdError									=	{0};
	
	if ( phLowLevelHandle )
		*phLowLevelHandle = hHandle;
	
	RETURN_STDERR_POINTER;	
}


// Equipment Is Supported
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
		if (( strstr( szIdentificationText , "BOONTON ELECTRONICS" ) == NULL ))
			break;
	
		if ( strstr( szIdentificationText , "4542" ))
		{
			bSupport = 1;
			break;
		}
					
	}while(0);
	
	if (pbSupporting)
		*pbSupporting = bSupport;
	
	RETURN_STDERR_POINTER;
}

//  Power Meter Init
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
	
	viPrintf( pLocalHandle->sessionHandle ,"*CLS" );
	
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

// Power Meter Close
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

// Power Meter Reset
void* DLLEXPORT PowerMeter_Reset ( int hInstrumentHandle )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};
	
	char					szCommandName[LOW_STRING]			   =	{0}; 

	int						handle									=	0;
	
	int						count									=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle; 
	
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	strcpy(szCommandName, "*CLS"); 
	viWrite( pLocalHandle->sessionHandle , szCommandName, strlen(szCommandName), &count); 
	
	strcpy(szCommandName, "*RST"); 
	viWrite( pLocalHandle->sessionHandle , szCommandName, strlen(szCommandName), &count);
						
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

// Power Meter Calibrate
void* DLLEXPORT PowerMeter_Calibrate( int hInstrumentHandle , int iChannel )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;

	char					szCommand[LOW_STRING]					=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szCommand , "CAL%d:INT:AUTO" , iChannel );   
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

//  Power Meter Zero
void* DLLEXPORT PowerMeter_Zero( int hInstrumentHandle , int iChannel )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;

	char					szCommand[LOW_STRING]					=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szCommand , "CAL%d:INT:ZERO" , iChannel );
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

//  Power Meter Set Channel State
void* DLLEXPORT PowerMeter_SetChanelState ( int hInstrumentHandle , int iChannel , int iState )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};
	
	char					szCommand[LOW_STRING]					=	{0};

	int						handle									=	0;

	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle )); 
	
	sprintf( szCommand , "CALCULATE%d:STATE %d", iChannel, iState); 
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);	
}

// Power Meter Set Units
void* DLLEXPORT PowerMeter_SetUnits( int hInstrumentHandle , int iChannel , unsigned int units )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	char					szCommand[LOW_STRING]					=	{0}; 	

	int						handle									=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	switch(units)
	{
		case 1:
			
			sprintf( szCommand , "CALCULATE%d:UNITS DBM", iChannel);
			break;
			
		default:	
			
			sprintf( szCommand , "CALCULATE%d:UNITS Watts", iChannel); 
			break;
	}
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

// Power Meter Set Continues Sweep
void* DLLEXPORT PowerMeter_SetContinuesSweep ( int hInstrumentHandle , int iChannel )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};
	
	char					szCommand[LOW_STRING]					=	{0};

	int						handle									=	0;

	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle )); 
	
	sprintf( szCommand , "INITiate:CONTinuous ON"); 
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommand ));
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

//  Power Meter Set Auto
void* DLLEXPORT PowerMeter_SetAuto ( int hInstrumentHandle , int iChannel )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};
	
	char					szCommand[LOW_STRING]					=	{0}; 

	int						handle									=	0;
	
	int						count									=	0;   
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ))
	
	strcpy(szCommand, "SYSTem:AUTOSET"); 
	viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count);
	
	DelayWithEventProcessing(0.1);   
	
	//strcpy( szCommand , "CALCULATE1:STATE 1");
	//viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count); 
	
	//strcpy( szCommand , "CALCULATE1:UNITs DBM");
	//viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count); 
	
	//strcpy( szCommand , "INITiate:CONTinuous ON");
	//viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count); 
	
	//strcpy( szCommand , "CALCULATE:MODE PULSE");
	//viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count); 
	
	//strcpy( szCommand , "DISPLAY:MODE GRAPH");
	//viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count); 
	
	//strcpy( szCommand , "TRIGger:SOURce CH1");
	//viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count); 
	
	//strcpy( szCommand , "TRIGger:LEVel 2");
	//viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count); 
	
	//strcpy( szCommand , "TRIGger:SLOPe POS");
	//viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count); 
	
	//strcpy( szCommand , "TRIGger:MODE AUTO ");
	//viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count);
	
	strcpy( szCommand , "TRIGger:POSition LEFT ");
	viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count); 
	
	//strcpy( szCommand , "TRIGGER:DELAY 0");
	//viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count);
	
	DelayWithEventProcessing(0.1); 
	
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

// Power Meter Set Mode
void* DLLEXPORT PowerMeter_SetMode( int hInstrumentHandle , unsigned int Mode )
{ 
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	switch(Mode)
	{
		case 1:
			
			CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "CALCULATE:MODE PULSE" )); 
			DelayWithEventProcessing(10);
			CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "DISPLAY:MODE GRAPH" )); 
			
			break;
			
		default:	
			
			CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "CALCULATE:MODE MODULATED" )); 
			DelayWithEventProcessing(10);
			CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "DISPLAY:MODE TEXT" )); 
			
			break;
	}
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

// Power Meter Set Vertical Scale
void* DLLEXPORT PowerMeter_SetVerticalScale( int hInstrumentHandle , int iChannel , double scale )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;
	
	int						count									=	0; 
	
	char					szCommand[LOW_STRING]					=	{0}; 
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szCommand , "DISPlay:TRACe%d:VSCALe %f", iChannel, scale); 
	viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count); 
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );     
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

// Power Meter Set Vertical Center
void* DLLEXPORT PowerMeter_SetVerticalCenter( int hInstrumentHandle , int iChannel , double scale )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;
	
	int						count									=	0;
	
	char					szCommand[LOW_STRING]					=	{0}; 
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szCommand , "DISPlay:TRACe%d:VCENTer %f", iChannel, scale);
	viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count);
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );     
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

//  Power Meter Set Frequency
void* DLLEXPORT PowerMeter_SetFrequency( int hInstrumentHandle , int iChannel , double lfFrequency )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;
	
	int						count									=   0;
	
	char					szCommand[LOW_STRING]					=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szCommand , "SENSe%d:CORRection:FREQuencyy %E", iChannel, lfFrequency );
	viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count);

	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

//  PowerMeter_SetCalFactor
void* DLLEXPORT PowerMeter_SetCalFactor( int hInstrumentHandle , int iChannel , double CalFactor )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;
	
	int						count									=   0;
	
	char					szCommand[LOW_STRING]					=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szCommand , "SENSe%d:CORRection:OFFSet %E", iChannel, CalFactor ); 
	viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count); 
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL); 

}

// Power Meter Set Offset
void* DLLEXPORT PowerMeter_SetOffset( int hInstrumentHandle , int iChannel , double lfPulseWidth )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;

	char					szCommand[LOW_STRING]					=	{0};
	
	int						count							    	=	0;  
	
	double					delay							    	=	0;  
	
	
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	delay = lfPulseWidth * -0.1;
	
	// Set Time Window size 
	sprintf( szCommand , "DISPLAY:PULSE:TSPAN %E", ( lfPulseWidth * 1.2 ));
	viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count);
	
	// Set Offset 10 % of Pulse
	sprintf( szCommand , "TRIGGER:DELAY %E", delay );
	viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count);
	
	// Set Markers 1 on left side of pulse
	strcpy( szCommand , "MARKer1:POSItion:TIMe 0");
	viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count);
	
	// Set Markers 7 on rigth side of pulse
	sprintf( szCommand , "MARKer2:POSItion:TIMe %E", (lfPulseWidth / 2));
	viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count);
	
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
		
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

// Power Meter Configure Marker
void* DLLEXPORT PowerMeter_Configure_Marker( int hInstrumentHandle , int iChannel , int iMarkerNumber , int bEnable , double lfPosition )
{
	STD_ERROR					StdError							=	{0};
	
	tsHandle					*pLocalHandle						=	{0};

	int							handle								=	0;
	
	int             		    count								=   0;
	
	char					    szCommand[LOW_STRING]				=	{0};
	
	char					    szReadBuffer[LOW_STRING]			=	{0};
	
	char						**pCharArray						=   NULL;
	
	double             		    delay								=   0; 
	
	double             		    tiggerDelay							=   0; 
	
	double             		    edgeDelay							=   0;  
	
	
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	/*
	
	strcpy( szCommand , "TRIGGER:DELAY?");
	viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count); 

	viRead( pLocalHandle->sessionHandle, szReadBuffer , LOW_STRING , &count );
	
	if( count > 0 )
	{
		tiggerDelay =  atof(szReadBuffer);
	}
	
	// Get Edge Delay
	sprintf( szCommand , "FETCh%d:ARRay:AMEAsure:TIMe?", iChannel);
	viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count); 

	viRead( pLocalHandle->sessionHandle, szReadBuffer , LOW_STRING , &count );
	
	if( count > 0 )
	{
		SplitStringByDelimiter (szReadBuffer, ',', &pCharArray, &count ); 
		 
		if( count > 0 )
		{ 
			edgeDelay =  atof(pCharArray[15]); 
			
			// Real Delay
			delay =  (edgeDelay + tiggerDelay);
			
			lfPosition -= delay; 
		}
	} 
	
    */
	
	sprintf( szCommand , "MARKer%d:POSItion:TIMe %E", iMarkerNumber, lfPosition);
	viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count); 
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );    
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

//  Power Meter Fetch Marker
void* DLLEXPORT PowerMeter_Fetch_Marker( int hInstrumentHandle , int iChannel , int iMarkerNumber, double *vlfPosition , double *vlfPower )
{
	STD_ERROR				StdError								=	{0};
	
	tsHandle				*pLocalHandle							=	{0};

	int						handle									=	0;

	char					szCommand[LOW_STRING]					=	{0};
	
	char					szReadBuffer[LOW_STRING]		    	=	{0}; 
	
	char                    **pCharArray                            =   NULL;   
	
	double					power									=	{0}; 
	
	int						count								    =	0;  
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szCommand , "FETCh%d:ARRay:MARKer:POWer?", iChannel);
	viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count);

	viRead( pLocalHandle->sessionHandle, szReadBuffer , LOW_STRING , &count );
	
	if( count > 0 )
	{
		SplitStringByDelimiter (szReadBuffer, ',', &pCharArray, &count); 
		 
		if( count > 0 )
		{ 
			
			switch (iMarkerNumber)
			{
				case 1:
					
					power =  atof(pCharArray[9]);  
					
					
					break;
					
				case 2:
					
					power =  atof(pCharArray[11]); 
			
					break;
			} 
			
				
			if ( power )
				*vlfPower = power;
			
		}
	}
	
	FREE(pCharArray);
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

//  Power Meter Recall State ByName
void* DLLEXPORT PowerMeter_RecallState_ByName( int hInstrumentHandle , char *szFileName ) 
{
	STD_ERROR           		    StdError                  						=   {0};        

	char							szLoadState[512]                       			=	{0},
									szCommand[LOW_STRING]    						=	{0},
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
		strcpy(szCommand, "*CLS"); 
		viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count);
		 
		sprintf( szLoadState , "MEMory:SYS:LOAD %s\n" , szFileName );
		viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count); 
	
		DelayWithEventProcessing(pLocalHandle->lfStateFileDelay);
		
		/*   
		
		strcpy( szCommand , "CALCULATE1:STATE 1");
		viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count);
		
		DelayWithEventProcessing(0.1);
		
		strcpy( szCommand , "CALCULATE1:UNITs DBM");
		viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count); 
		
		DelayWithEventProcessing(0.1);
		
		strcpy( szCommand , "INITiate:CONTinuous ON");
		viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count);
		
		DelayWithEventProcessing(0.1);
		
		strcpy( szCommand , "CALCULATE:MODE PULSE");
		viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count);
		
		DelayWithEventProcessing(0.1);
		
		strcpy( szCommand , "DISPLAY:MODE GRAPH");
		viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count);
		
		DelayWithEventProcessing(0.1);
		
		strcpy( szCommand , "TRIGger:SOURce CH1");
		viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count);
		
		DelayWithEventProcessing(0.1);
		
		strcpy( szCommand , "TRIGger:LEVel 2");
		viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count);
		
		DelayWithEventProcessing(0.1);
		
		strcpy( szCommand , "TRIGger:SLOPe POS");
		viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count);
		
		DelayWithEventProcessing(0.1);
		
		strcpy( szCommand , "TRIGger:MODE AUTO ");
		viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count);
		
		DelayWithEventProcessing(0.1);
		
		strcpy( szCommand , "TRIGger:POSition LEFT ");
		viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count);
		
		DelayWithEventProcessing(0.1);
		
		strcpy( szCommand , "TRIGGER:DELAY 0");
		viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count);
		
		DelayWithEventProcessing(0.1);
		
		*/  
		
		strcpy( szCommand , "SYST:ERR?\n" );
		viWrite( pLocalHandle->sessionHandle , szCommand, strlen(szCommand), &count); 

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
		
		    
		
		FREE(pFileNamePath);
		
		CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
		bGetLock = 1;
	}																   
	
Error: 

	if ( handle && bGetLock )
		CmtReleaseTSVPtr ( handle );
	
	return PowerMeter_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}



//==================================================================================================================================

//                                      Main Entry Point Using For Teating

//==================================================================================================================================
int main (int argc, char *argv[])
{
	int					status				=	0;
	
	char				szOutputText[1024]	=	{0};
	
	ViSession 			hInstrumentHandle	=	0;
		
	int					bConnectStatus		=	0,
						ChannelsQuantity	=	0,
						Count				=	0; 
							
	double				lfFrequency			=	0.0,
						lfPower				=	0.0;
	
	double 				*pTrace				=	NULL;
	
	char				szToken[LOW_STRING]				=	{0}; 
	
	
	InitCVIRTE (0, argv, 0 );   
	

	status = PowerMeter_Init( 0 , "GPIB::14::INSTR" , &hInstrumentHandle , &bConnectStatus , &ChannelsQuantity );
	
	status = PowerMeter_Reset ( hInstrumentHandle ); 
		
	status = PowerMeter_Close(&hInstrumentHandle);	
	
	return 0;
	
}

/*

typedef struct
{
	ViStatus			defaultRM,
						sessionHandle;
	
	int			    	visaError,	  
						error;		  
	
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

int		QueryCommand( int handle , char *szCommand , double lfTimeout , char **pReceive ); 
 

//==================================================================================================================================

//                                 Old  Internal functions

//==================================================================================================================================

// Wait For Operation Completion
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

// PowerMeter Get Last Error
char		*PowerMeter_GetLastError( int handle )
{
	char		*pRetError			=	NULL,
				szErrorDescr[256]	=	{0};
	
	tsHandle	*pLocalHandle			=	{0}; 
	
	do
	{
		
		if ( CmtGetTSVPtr ( handle , &pLocalHandle ) < 0 )
		{
			sprintf( szErrorDescr , "Wrong Handle" );
			break;
		}
	
		if (( pLocalHandle->error == VISA_ERROR ) || ( pLocalHandle->error == CANNOT_CONNECT ))
			 viStatusDesc( pLocalHandle->sessionHandle , pLocalHandle->visaError , szErrorDescr );
	
	}while(0);
	
Error:

	if ( strlen ( szErrorDescr ))
	{
		pRetError = calloc( 256 , 1 );
		
		if ( pRetError )
			strcpy( pRetError , szErrorDescr );
	}
	
	CmtReleaseTSVPtr ( handle );
	
	return	pRetError;
}


//  Write Command
int		WriteCommand( int handle , char *szCommand )
{
	int			error					=	0;
	
	tsHandle	*pLocalHandle			=	{0};
	
	int			count					=	0;
	
	do
	{
		
		if ( CmtGetTSVPtr ( handle , &pLocalHandle ) < 0 )
		{
			error = WRONG_HANDLE;
			break;
		}
		
		

		pLocalHandle->visaError = viWrite ( pLocalHandle->sessionHandle , szCommand , strlen(szCommand) , &count );
			
		if ( pLocalHandle->visaError < 0 )
		{
			error = VISA_ERROR;
			break;
		}
		
		if ( strlen(szCommand) != count )
		{
			error = NO_DATA_SENT;
			break;
		}
		

	} while(0);
	
Error:
	
	pLocalHandle->error = error;
		
	CmtReleaseTSVPtr ( handle );
	
	return error;
}

//  Query Command
int		QueryCommand( int handle , char *szCommand , double lfTimeout , char **pReceive )
{
	int						error					=	0;
	
	tsHandle				*pLocalHandle			=	{0};
	
	int						count					=	0;
	
	int						iCount							=	0,
							iCounter						=	0,
							iCurrentBufferSize				=	0,
							bFail							=	0;
	
	double					lfStartTime						=	0.0,
							lfPrevTime						=	0.0,
							lfCurrentTime					=	0.0,
							lfMaxFailInterval				=	0.0;
	
	unsigned char			cGetByte						=	0,
							*pBuffer						=	NULL,
							*pTempBuffer					=	NULL;
	
	do
	{
		
		if ( CmtGetTSVPtr ( handle , &pLocalHandle ) < 0 )
		{
			error = WRONG_HANDLE;
			break;
		}
		
		pLocalHandle->visaError = viWrite ( pLocalHandle->sessionHandle , szCommand , strlen(szCommand) , &count );
			
		if ( pLocalHandle->visaError < 0 )
		{
			error = VISA_ERROR;
			break;
		}
		
		if ( strlen(szCommand) != count )
		{
			error = NO_DATA_SENT;
			break;
		}
		
		///-------------- Read -----------------------------------//
		iCounter = 0;
		iCurrentBufferSize = 512;
		lfMaxFailInterval = 0.2;
		
		pBuffer = calloc ( iCurrentBufferSize , 1 ); 
			
		GetCurrentDateTime(&lfStartTime);
	
		SetBreakOnLibraryErrors (0);
	
		do
		{
			if ( viRead ( pLocalHandle->sessionHandle , &cGetByte , 1 , &iCount) >= 0 )
			{
				if ( iCounter >= iCurrentBufferSize )
				{
					iCurrentBufferSize *= 2;
				
					pTempBuffer = calloc ( iCurrentBufferSize , 1 );
				
					if ( pTempBuffer == NULL )
					{
						error = FUNC_IN_NULL_POINTER;
						break;
					}
					
					memcpy( pTempBuffer , pBuffer , iCounter );
				
					free(pBuffer);
				
					pBuffer = pTempBuffer; 
				}
			
				pBuffer[iCounter] = cGetByte;
				iCounter++;
			
				GetCurrentDateTime(&lfPrevTime); 
				bFail = 0;
			}
			else
				bFail = 1;
		
			GetCurrentDateTime(&lfCurrentTime); 
		
			if ( bFail && ( lfPrevTime > 0.0 ))
				if ( fabs( lfCurrentTime - lfPrevTime ) > lfMaxFailInterval )
					break;
		
		}while((lfCurrentTime-lfStartTime)<lfTimeout);

	} while(0);
	
Error:
	
	if ( pReceive )
	{
		*pReceive = pBuffer; 
	}
	else
	{
		if ( pBuffer )
			free(pBuffer);
	}
	
	SetBreakOnLibraryErrors (1); 
	
	pLocalHandle->error = error;
		
	CmtReleaseTSVPtr ( handle );
	
	return error;
}


//  Query Command Int
int		QueryCommandInt( int handle , char *szCommand , double lfTimeout , int *value )
{
	int			error				=	0;
	char		*pReceiveValue		=	NULL;
	
	error = QueryCommand( handle , szCommand , lfTimeout , &pReceiveValue );
	
	
	if ( pReceiveValue )
	{
		if ( value )
			*value = atoi(pReceiveValue);
	
		free(pReceiveValue);
	}
	return error;
}


//  Query Command Double
int		QueryCommandDouble( int handle , char *szCommand , double lfTimeout , double *value )
{
	int			error				=	0;
	char		*pReceiveValue		=	NULL;
	
	error = QueryCommand( handle , szCommand , lfTimeout , &pReceiveValue );
	
	
	if ( pReceiveValue )
	{
		if ( value )
			*value = atof(pReceiveValue);
	
		free(pReceiveValue);
	}
	return error;
}


// OPC
int		OPC( int handle , double lfTimeout )
{
	double					lfStartTime						=	0.0,
							lfCurrentTime					=	0.0;
	
	int						status							=	0;
	
	GetCurrentDateTime(&lfStartTime);
	
	do
	{
		
		QueryCommandInt( handle , "*OPC?" , lfTimeout / 10.0 , &status );
		
	} while((status==0)&&((lfCurrentTime-lfStartTime) < lfTimeout ));
	
	return	status;
} 

//==================================================================================================================================

//                                      Old Wrapper functions 

//================================================================================================================================== 

// Power Meter Init
int		DLLEXPORT	PowerMeter_Initiate( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *vhInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChannelsQuantity )  
{
	int				error					=	0;
	
	tsHandle		*pLocalHandle			=	{0};
	
	CmtTSVHandle 	handle					=	0;
	
	int				supported				=	0;
	
	char			*Model					=	NULL;
	
	if ( szRsrcAddress == NULL )
		return FUNC_IN_NULL_POINTER;
	
	if ( hParentInstrumentHandle == 0 ) 
	{
		if ( CmtNewTSV ( sizeof(tsHandle) , &handle ) < 0 )
			return CANNOT_ALLOCATE_MEMORY;
	}
	else
		handle = hParentInstrumentHandle;
		
	if ( vhInstrumentHandle )
			*vhInstrumentHandle = handle;
	
	do
	{
		if ( CmtGetTSVPtr ( handle , &pLocalHandle ) < 0 )
		{
			error = WRONG_HANDLE;
			break;
		}
	
		if ( pLocalHandle->defaultRM == 0 )
		{
			pLocalHandle->visaError = viOpenDefaultRM (&(pLocalHandle->defaultRM));
			
			if ( pLocalHandle->visaError < 0 )
			{
				error = VISA_ERROR;
				break;
			}
		}
		
		if ( pLocalHandle->sessionHandle == 0 )
		{
			
			pLocalHandle->visaError = viOpen ( pLocalHandle->defaultRM , szRsrcAddress , VI_NULL, VI_NULL, &(pLocalHandle->sessionHandle));
			
			if ( pLocalHandle->visaError < 0 )
			{
				error = CANNOT_CONNECT;
				break;
			}
			
			viSetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , 100 );
			
			if ( QueryCommand( handle , "*IDN?" , 10.0 , &Model ) != 0 ) 
				break;
				
			if ( strstr( Model , "BOONTON ELECTRONICS") == NULL )
				break;
			
			if ( strstr( Model , "4542"))
				supported = 1;
			
		}
		
	} while(0);
	
Error:
	
	if ( InstrumentConnectStatus )
		*InstrumentConnectStatus = supported;
	
	pLocalHandle->error = error;
		
	CmtReleaseTSVPtr ( handle );
	
	if ( Model )
		free( Model );
	
	return error;
}


//  Power Meter Close
int		DLLEXPORT	PowerMeter_Close( int *pHandle )
{
	int			error					=	0;
	
	tsHandle	*pLocalHandle			=	{0};

	int			handle					=	0;
	
	if ( pHandle == NULL )
		return FUNC_IN_NULL_POINTER;
	
	handle = *pHandle;
	
	if ( handle == 0 )
		return 0;
	
	if ( CmtGetTSVPtr ( handle , &pLocalHandle ) < 0 )
		return WRONG_HANDLE;
	
	viClose (pLocalHandle->sessionHandle);  
	
	viClose (pLocalHandle->defaultRM);
	
	CmtReleaseTSVPtr ( handle ); 
	
	CmtDiscardTSV (handle);
	
	*pHandle = 0;
	
	return error;
}


// Power Meter Reset
int		DLLEXPORT	PowerMeter_Reset( int handle )
{
	return WriteCommand( handle , "*RST" );
} 

// Power Meter Set Single Sweep
int		DLLEXPORT	PowerMeter_SetSingleSweep( int handle , int channel ) 
{
	return WriteCommand( handle , "INIT:CONT 0" );
}

//  Power Meter Set Continues Sweep
int		DLLEXPORT	PowerMeter_SetContinuesSweep( int handle , int channel ) 
{
	return WriteCommand( handle , "INIT:CONT 1" );
}

// Power Meter Init Sweep
int		DLLEXPORT	PowerMeter_InitSweep( int handle , int channel ) 
{
	return WriteCommand( handle , "INIT:IMM:ALL" );
}


// Power Meter SetAuto
int		DLLEXPORT	PowerMeter_SetAuto( int handle , int channel ) 
{
	return WriteCommand( handle , "SYST:AUTOSET" );
}


// Power Meter Set Chanel State
int		DLLEXPORT	PowerMeter_SetChanelState( int handle , int channel , int state )
{
	char		szCommand[512]		=	{0};
	
	sprintf( szCommand , "CALC%d:STAT %d" , channel , state );
	
	return WriteCommand( handle , szCommand);
}


// Power Meter Set Verical Scale
int		DLLEXPORT	PowerMeter_SetVericalScale( int handle , int channel , double scale )
{
	char		szCommand[512]		=	{0};
	
	sprintf( szCommand , "DISP:TRAC%d:VSCAL %lf" , channel , scale );
	
	return WriteCommand( handle , szCommand);
}

// Power Meter Set Verical Center
int		DLLEXPORT	PowerMeter_SetVericalCenter( int handle , int channel , double center )
{
	char		szCommand[512]		=	{0};
	
	sprintf( szCommand , "DISP:TRAC%d:VCENT %lf" , channel , center );
	
	return WriteCommand( handle , szCommand);
}

// Power Meter Calibrate
int		DLLEXPORT	PowerMeter_Calibrate( int handle , int channel )
{
	char		szCommand[512]		=	{0};
	
	sprintf( szCommand , "CAL%d:INT:AUTO" , channel );
	
	return WriteCommand( handle , szCommand);
}

// Power Meter Zero
int		DLLEXPORT	PowerMeter_Zero( int handle , int channel )
{
	char		szCommand[512]		=	{0};
	
	sprintf( szCommand , "CAL%d:INT:ZERO" , channel );
	
	return WriteCommand( handle , szCommand);
}

// Power Meter SetUnits
int		DLLEXPORT	PowerMeter_SetUnits( int handle , int channel , unsigned int units )
{
	char		szCommand[512]		=	{0},
				szUnits[][32]		=	{{"dBm"} , {"Watts"}, {"Volts"} , {"dBV"} , {"dBmV"} , {"dBÏV"}};
	
	if ( units < (sizeof(szUnits) / sizeof(szUnits[0])) )
		sprintf( szCommand , "CALC%d:UNIT %s" , channel , szUnits[units] );
	else
		sprintf( szCommand , "CALC%d:UNIT dBm" , channel );
	
	return WriteCommand( handle , szCommand);
}

//  Power Meter Set Offset
int		DLLEXPORT	PowerMeter_SetOffset( int handle , int channel , double Offset )
{
	char		szCommand[512]		=	{0};
	
	sprintf( szCommand , "SENS%d:CORR:OFFS %lf" , channel , Offset );
	
	return WriteCommand( handle , szCommand);
}

// Power Meter Set Cal Factor
int		DLLEXPORT	PowerMeter_SetCalFactor( int handle , int channel , double CalFactor )
{
	char		szCommand[512]		=	{0};
	
	sprintf( szCommand , "SENS%d:CORR:CALF %lf" , channel , CalFactor );
	
	return WriteCommand( handle , szCommand);
}

// Power Meter Set Video BW
int		DLLEXPORT	PowerMeter_SetVideoBW( int handle , int channel , unsigned int VideoBW )
{
	char		szCommand[512]		=	{0},
				szVideoBW[][32]		=	{{"Low"} , {"High"}};
	
	if ( VideoBW < (sizeof(szVideoBW) / sizeof(szVideoBW[0])) )
		sprintf( szCommand , "SENS%d:BAND %s" , channel , szVideoBW[VideoBW] );
	else
		sprintf( szCommand , "SENS%d:BAND dBm" , channel );
	
	return WriteCommand( handle , szCommand);
}

//  Power Meter Set Peak Hold
int		DLLEXPORT	PowerMeter_SetPeakHold( int handle , int channel , int state )
{
	char		szCommand[512]		=	{0};
	
	sprintf( szCommand , "CALC%d:PKHLD %d" , channel , state );
	
	return WriteCommand( handle , szCommand);
}

//  Power Meter Set Frequency
int		DLLEXPORT	PowerMeter_SetFrequency( int handle , int channel , double Frequency )
{
	char		szCommand[512]		=	{0};
	
	sprintf( szCommand , "SENS%d:AVER %lf" , channel , Frequency );
	
	return WriteCommand( handle , szCommand);
}

// Power Meter Set Mode
int		DLLEXPORT	PowerMeter_SetMode( int handle , unsigned int Mode )
{
	char		szCommand[512]		=	{0},
				szMode[][32]		=	{{"Auto"} , {"Auto PK-PK"}, {"Normal"}, {"Freerun"}};
	
	if ( Mode < (sizeof(szMode) / sizeof(szMode[0])) )
		sprintf( szCommand , "TRIG:MOD %s" , szMode[Mode] );
	else
		sprintf( szCommand , "TRIG:MOD Auto" );
	
	return WriteCommand( handle , szCommand);
}

//  Power Meter OPC
int		DLLEXPORT	PowerMeter_OPC( int handle , double timeout , int *state )
{
	char		szCommand[512]		=	{"*OPT?"};
	
	return QueryCommandInt( handle , szCommand , timeout , state );
}

//  Power Meter Fetch CWpower
int		DLLEXPORT	PowerMeter_FetchCWpower( int handle , int channel , double timeout , double *value )
{
	char		szCommand[512]		=	{0};
	
	sprintf( szCommand , "FETC%d:CW:POW?" , channel );  
	
	return QueryCommandDouble( handle , szCommand , timeout , value );
}

// Power Meter Measure Modulated Power
int		DLLEXPORT	PowerMeter_MeasureModulatedPower( int handle , int channel , double timeout , double *value )
{
	char		szCommand[512]		=	{0};
	
	sprintf( szCommand , "MEAS%d:POW?" , channel );  
	
	return QueryCommandDouble( handle , szCommand , timeout , value );
}

//  Power Meter Fetch Marker Average
int		DLLEXPORT	PowerMeter_FetchMarkerAverage( int handle , int channel , int marker , double timeout , double *value )
{
	char		szCommand[512]		=	{0};
	
	sprintf( szCommand , "FETC%d:MARK%d:AVER?" , channel );  
	
	return QueryCommandDouble( handle , szCommand , timeout , value );
}

// Power Meter Fetch Marker Max
int		DLLEXPORT	PowerMeter_FetchMarkerMax( int handle , int channel , int marker , double timeout , double *value )
{
	char		szCommand[512]		=	{0};
	
	sprintf( szCommand , "FETC%d:MARK%d:MAX?" , channel );  
	
	return QueryCommandDouble( handle , szCommand , timeout , value );
}

// Power Meter Fetch Marker Min
int		DLLEXPORT	PowerMeter_FetchMarkerMin( int handle , int channel , int marker , double timeout , double *value )
{
	char		szCommand[512]		=	{0};
	
	sprintf( szCommand , "FETC%d:MARK%d:MIN?" , channel );  
	
	return QueryCommandDouble( handle , szCommand , timeout , value );
}

*/
