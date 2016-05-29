#include <windows.h>
#include <userint.h>
#include <ansi_c.h>
#include <utility.h>
#include "GlobalDefines.h"
#include <formatio.h>
#include <visa.h>
#include "toolbox.h"

//==============================================================================
// Include files

#include "Agilent_N67XX_wrapper.h"

//==============================================================================
// Constants

//==============================================================================
// Types
typedef struct
{				
	int					defaultRM,
						sessionHandle;
	
	char				*pLastStateFile,
						*pVendor,
						*pSerialNumber,
						*pModel,
						*pFirmware;
	
	double				lfTimeout,
						lfOpcLowLevelTimeout;
	
	int					iChannelsQuantity,
						iStartFromChannel;
	
	tsSTD_CallBackSet	*ptCallbacks;
	
} tsHandle;

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
//==============================================================================
// Global functions

void* DLLEXPORT PowerSupply_Close ( int *phInstrumentHandle );

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

void* DLLEXPORT PowerSupply_GetErrorTextMessage ( int hInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char			szCommand[LOW_STRING]				=	{0},
					szReadBuffer[LOW_STRING]			=	{0},
					szErrorDescription[LOW_STRING]		=	{0};
	
	int				count								=	0;
	
	char			*pTemp								=	NULL;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	strcpy( szCommand , ":SYST:ERR?\n" );
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
	viPrintf( pLocalHandle->sessionHandle , szCommand );
	SetBreakOnLibraryErrors (0);
	viRead( pLocalHandle->sessionHandle, szReadBuffer , LOW_STRING , &count );
	SetBreakOnLibraryErrors (1);
	
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

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
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
		
		IF (( strstr( szIdentificationUpperText , "AGILENT TECHNOLOGIES" ) == NULL ) , "Is not supported" );
	
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
	STD_ERROR						StdError										=	{0};

	tsHandle						*pLocalHandle									=	{0};
	
	if ( phLowLevelHandle )
	{
		if ( CmtGetTSVPtr ( hHandle , &pLocalHandle ) == 0 )
		{
			*phLowLevelHandle = pLocalHandle->sessionHandle; 
			
			CmtReleaseTSVPtr ( hHandle ); 
		}	
	}
	
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
									szIdentificationUpperText[LOW_STRING]		=	{0}; 
	
	char							vszSupportdedModels[][32]					=	{{"N6700B"},{"N6701A"},{"N5747A"}};
									
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
		
		if ( strstr( szIdentificationUpperText , "AGILENT TECHNOLOGIES" ) == NULL )
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

/*********************************************** Init ***********************************************/

void* DLLEXPORT PowerSupply_Init ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *phInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChanelsQuantity )
{
	STD_ERROR						StdError										=	{0};
	
	tsHandle						*pLocalHandle									=	{0};
	
	CmtTSVHandle 					handle											=	0;
	
	int								supported										=	0;
	
	int								iIndex											=	0;
	
	unsigned int					uiCount											=	0;
	
	char							*pTemp											=	NULL;

	tsSTD_CallBackSet				tSTD_CallBackSet								=	{0};
	
	double							lfTimeout										=	0.0,
									lfDefaultTimeout								=	5.0;

	int								iChannelsQuantity								=	0;
	
	int								iHandleIndex									=	0;
	
	tsHandleList					*ptHandleList									=	NULL;
	
	int								index											=	0;
	
	int								bFound											=	0;
	
	IF (( szRsrcAddress == NULL ) , "Address string is empty" );
	
	if ( hParentInstrumentHandle == 0 ) 
	{
		CHK_CMT ( CmtNewTSV ( sizeof(tsHandle) , &handle ));
	}
	else
		handle = hParentInstrumentHandle;
		
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
	
	if ( phInstrumentHandle )
		*phInstrumentHandle = handle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));

	if ( pLocalHandle->defaultRM == 0 )
	{
		CHK_VSA ( viOpenDefaultRM (&(pLocalHandle->defaultRM)));
	}
	
	if ( pLocalHandle->sessionHandle == 0 )
	{
		CHK_VSA ( viOpen ( pLocalHandle->defaultRM , szRsrcAddress , VI_NULL, VI_NULL, &(pLocalHandle->sessionHandle)));
	}
	
	CHK_VSA ( viSetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , 500 ));
	
	FREE_STDERR_COPY_ERR( Equipment_IsSupported ( pLocalHandle->sessionHandle , NULL , NULL , NULL , &supported , &pLocalHandle )); 

	IF (( supported == 0 ) , "This device has not supported." );

	if ( InstrumentChanelsQuantity )
		*InstrumentChanelsQuantity = pLocalHandle->iChannelsQuantity;
		
	FREE_STDERR_COPY_ERR( Equipment_Info ( pLocalHandle->sessionHandle , NULL , &pLocalHandle->pVendor , &pLocalHandle->pSerialNumber , &pLocalHandle->pModel , &pLocalHandle->pFirmware ));

	STDF_UPDATE_CALLBACK_DATA(pLocalHandle->ptCallbacks); 
	
	STDF_CONFIG_VALUE("Timeout", VAL_DOUBLE , 1 , lfTimeout , lfDefaultTimeout );	
	
	if ( lfTimeout == 0.0 )
		lfTimeout = lfDefaultTimeout;
	
	viSetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , (lfTimeout*1E3) );
	
	if ( viPrintf ( pLocalHandle->sessionHandle , "SYST:CHAN:COUN?\n" ) == 0 )
	{
		viScanf ( pLocalHandle->sessionHandle , "%d" , &iChannelsQuantity );
		
		pLocalHandle->iChannelsQuantity = iChannelsQuantity;
		pLocalHandle->iStartFromChannel = 1;
	}
	
	STDF_CONFIG_VALUE("OPC_Timeout", VAL_DOUBLE , 1 , (pLocalHandle->lfTimeout) , lfDefaultTimeout );	
	
	if ( pLocalHandle->lfTimeout == 0.0 )
		pLocalHandle->lfTimeout = lfDefaultTimeout;
	
	STDF_CONFIG_VALUE("OPC_LowLevel_Timeout", VAL_DOUBLE , 1 , (pLocalHandle->lfOpcLowLevelTimeout) , lfDefaultTimeout );	 
	
	if ( pLocalHandle->lfOpcLowLevelTimeout == 0.0 )
		pLocalHandle->lfOpcLowLevelTimeout = lfDefaultTimeout;
	
Error:
	
	if ( InstrumentConnectStatus )
		*InstrumentConnectStatus = iChannelsQuantity;
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER;
}

/*********************************************** Close ***********************************************/

void* DLLEXPORT PowerSupply_Close ( int *phInstrumentHandle )
{
	STD_ERROR						StdError										=	{0};
	
	tsHandle						*pLocalHandle									=	{0};

	int								handle											=	0;
	
	tsHandleList					*ptHandleList									=	NULL;

	int								index											=	0;
	
	IF (( phInstrumentHandle == NULL ) , "Handle is empty" );
	
	handle = *phInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	viPrintf(pLocalHandle->sessionHandle , "*RST\n" );  
	
	viClose (pLocalHandle->sessionHandle);  
	
	viClose (pLocalHandle->defaultRM);
	
	if ( CmtGetTSVPtr ( gHandleListHandle , &ptHandleList ) == 0 )
	{
		for ( index = 0; index < ptHandleList->count; index++ )
			if ( ptHandleList->vHandles[index] == handle )
				ptHandleList->vHandles[index] = 0;	
		
		CmtReleaseTSVPtr ( gHandleListHandle ); 
	}
		
	FREE(pLocalHandle->pLastStateFile);
	FREE(pLocalHandle->pFirmware);
	FREE(pLocalHandle->pSerialNumber);
	FREE(pLocalHandle->pModel);
	
	CmtReleaseTSVPtr ( handle ); 
	
	CmtDiscardTSV (handle);
	
	*phInstrumentHandle = 0;
	
Error:

	RETURN_STDERR_POINTER;
}

/*********************************************** Reset ***********************************************/

void* DLLEXPORT PowerSupply_Reset ( int hInstrumentHandle )   
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char			szReadFeedback[LOW_STRING]			=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "*RST\n" ));
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerSupply_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


/*********************************** Wait For Operation Complete ***********************************************/

void* DLLEXPORT PowerSupply_WaitForOperationComplete ( int hInstrumentHandle , double lfTimeout )   
{   
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));	
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerSupply_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/*********************************************** Get Model *****************************************/

void* DLLEXPORT PowerSupply_GetModel ( int hInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));	
	
	if ( pReturnString )
		strcpy( pReturnString , pLocalHandle->pModel );
		
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerSupply_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


/*********************************************** Get Revision *****************************************/

void* DLLEXPORT PowerSupply_GetRevision ( int hInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));	
	
	if ( pReturnString )
		strcpy( pReturnString , pLocalHandle->pFirmware );
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerSupply_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


/***************************************** Set Output Voltage ***********************************/
void* DLLEXPORT PowerSupply_SetOutputVoltage ( int hInstrumentHandle , int channel , double lfVoltage )
{
	STD_ERROR       StdError							=	{0};
	
	double 			lfGetVoltage						=	0.0;
		
	int				iTryIndex							=	0;
	
	int				handle								=	0;
	
	tsHandle		*pLocalHandle						=	{0};
	
	char			szCommandString[LOW_STRING]			=	{0},  
					szReadFeedback[LOW_STRING]			=	{0};
	
	int				bSettingSuccess						=	0;
	
	int				count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));	
	
	IF ((( channel < pLocalHandle->iStartFromChannel ) || ( channel >= ( pLocalHandle->iStartFromChannel + pLocalHandle->iChannelsQuantity ))) , "Wrong channel number" );
	
	for ( iTryIndex = 0; iTryIndex < 5; iTryIndex++ )
	{
		sprintf( szCommandString , "VOLT %E,(@%d)\n" , lfVoltage , channel );
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommandString ));
	
		WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout , pLocalHandle->lfOpcLowLevelTimeout ); 
	
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "VOLT? (@%d)\n" , channel )); 
		CHK_VSA ( viRead ( pLocalHandle->sessionHandle , szReadFeedback , LOW_STRING , &count ));
			
		lfGetVoltage = atof(szReadFeedback);
		
		if ( fabs( lfGetVoltage - lfVoltage ) < (lfVoltage/1000.0))
		{
			bSettingSuccess = 1;
			break;
		}
		
		DelayWithEventProcessing(0.2);
	}
	
	IF (( bSettingSuccess == 0 ) , "Cann't set Voltage" );
		
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerSupply_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}



/*********************************** Get Output Voltage Defined ***********************************/

void* DLLEXPORT PowerSupply_GetOutputVoltageDefined ( int hInstrumentHandle , int channel , double *plfVoltage )
{
	STD_ERROR       StdError							=	{0};
	
	double 			lfGetVoltage						=	0.0;
		
	int				iTryIndex							=	0;
	
	int				handle								=	0;
	
	tsHandle		*pLocalHandle						=	{0};
	
	char			szCommandString[LOW_STRING]			=	{0},  
					szReadFeedback[LOW_STRING]			=	{0};
	
	int				count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));	
	
	IF ((( channel < pLocalHandle->iStartFromChannel ) || ( channel >= ( pLocalHandle->iStartFromChannel + pLocalHandle->iChannelsQuantity ))) , "Wrong channel number" );

	if ( plfVoltage )
	{
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "VOLT? (@%d)\n" , channel )); 
		CHK_VSA ( viRead ( pLocalHandle->sessionHandle , szReadFeedback , LOW_STRING , &count ));
			
		*plfVoltage = atof(szReadFeedback);
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerSupply_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/*********************************** Get Output Voltage Actual ***********************************/

void* DLLEXPORT PowerSupply_GetOutputVoltageActual ( int hInstrumentHandle , int channel , double *plfVoltage )
{
	STD_ERROR       StdError							=	{0};
	
	double 			lfGetVoltage						=	0.0;
		
	int				iTryIndex							=	0;
	
	int				handle								=	0;
	
	tsHandle		*pLocalHandle						=	{0};
	
	char			szCommandString[LOW_STRING]			=	{0},  
					szReadFeedback[LOW_STRING]			=	{0};
	
	int				count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));	
	
	IF ((( channel < pLocalHandle->iStartFromChannel ) || ( channel >= ( pLocalHandle->iStartFromChannel + pLocalHandle->iChannelsQuantity ))) , "Wrong channel number" );
	
	if ( plfVoltage )
	{
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "MEAS:VOLT? (@%d)\n" , channel )); 
		CHK_VSA ( viRead ( pLocalHandle->sessionHandle , szReadFeedback , LOW_STRING , &count ));
			
		*plfVoltage = atof(szReadFeedback);
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerSupply_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***************************************** Set Output Current ***********************************/

void* DLLEXPORT PowerSupply_SetOutputCurrent ( int hInstrumentHandle , int channel , double lfCurrent )
{
	STD_ERROR       StdError							=	{0};
	
	double 			lfGetCurrent						=	0.0;
		
	int				iTryIndex							=	0;
	
	int				handle								=	0;
	
	tsHandle		*pLocalHandle						=	{0};
	
	char			szCommandString[LOW_STRING]			=	{0},  
					szReadFeedback[LOW_STRING]			=	{0};
	
	int				bSettingSuccess						=	0;
	
	int				count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));	
	
	IF ((( channel < pLocalHandle->iStartFromChannel ) || ( channel >= ( pLocalHandle->iStartFromChannel + pLocalHandle->iChannelsQuantity ))) , "Wrong channel number" );

	for ( iTryIndex = 0; iTryIndex < 5; iTryIndex++ )
	{
		sprintf( szCommandString , "CURR %E,(@%d)\n" , lfCurrent , channel );
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommandString ));
	
		WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout , pLocalHandle->lfOpcLowLevelTimeout ); 
	
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "CURR? (@%d)\n" , channel )); 
		CHK_VSA ( viRead ( pLocalHandle->sessionHandle , szReadFeedback , LOW_STRING , &count ));
			
		lfGetCurrent = atof(szReadFeedback);
		
		if ( fabs( lfGetCurrent - lfCurrent ) < (lfCurrent/1000.0))
		{
			bSettingSuccess = 1;
			break;
		}
		
		DelayWithEventProcessing(0.2);
	}
	
	IF (( bSettingSuccess == 0 ) , "Cann't set Voltage" );
		
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerSupply_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


/*********************************** Get Output Current Defined ***********************************/

void* DLLEXPORT PowerSupply_GetOutputCurrentDefined ( int hInstrumentHandle , int channel , double *plfCurrent )
{
	STD_ERROR       StdError							=	{0};
	
	double 			lfGetVoltage						=	0.0;
		
	int				iTryIndex							=	0;
	
	int				handle								=	0;
	
	tsHandle		*pLocalHandle						=	{0};
	
	char			szCommandString[LOW_STRING]			=	{0},  
					szReadFeedback[LOW_STRING]			=	{0};
	
	int				count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));	
	
	IF ((( channel < pLocalHandle->iStartFromChannel ) || ( channel >= ( pLocalHandle->iStartFromChannel + pLocalHandle->iChannelsQuantity ))) , "Wrong channel number" );

	if ( plfCurrent )
	{
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "CURR? (@%d)\n" , channel )); 
		CHK_VSA ( viRead ( pLocalHandle->sessionHandle , szReadFeedback , LOW_STRING , &count ));
			
		*plfCurrent = atof(szReadFeedback);
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerSupply_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


/*********************************** Get Output Current Actual ***********************************/

void* DLLEXPORT PowerSupply_GetOutputCurrentActual ( int hInstrumentHandle , int channel , double *plfCurrent )
{
	STD_ERROR       StdError							=	{0};
	
	double 			lfGetVoltage						=	0.0;
		
	int				iTryIndex							=	0;
	
	int				handle								=	0;
	
	tsHandle		*pLocalHandle						=	{0};
	
	char			szCommandString[LOW_STRING]			=	{0},  
					szReadFeedback[LOW_STRING]			=	{0};
	
	int				count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));	
	
	IF ((( channel < pLocalHandle->iStartFromChannel ) || ( channel >= ( pLocalHandle->iStartFromChannel + pLocalHandle->iChannelsQuantity ))) , "Wrong channel number" );
	
	if ( plfCurrent )
	{
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "MEAS:CURR? (@%d)\n" , channel )); 
		CHK_VSA ( viRead ( pLocalHandle->sessionHandle , szReadFeedback , LOW_STRING , &count ));
			
		*plfCurrent = atof(szReadFeedback);
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerSupply_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


void* DLLEXPORT PowerSupply_GetOutputPowerActual ( int hInstrumentHandle , int channel , double *plfPower )
{
	STD_ERROR       StdError							=	{0};
	
	double 			lfGetVoltage						=	0.0,
					lfGetCurrent						=	0.0;
	
	int				iTryIndex							=	0;
	
	int				handle								=	0;
	
	tsHandle		*pLocalHandle						=	{0};
	
	char			szCommandString[LOW_STRING]			=	{0},  
					szReadFeedback[LOW_STRING]			=	{0};
	
	int				count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));	
	
	IF ((( channel < pLocalHandle->iStartFromChannel ) || ( channel >= ( pLocalHandle->iStartFromChannel + pLocalHandle->iChannelsQuantity ))) , "Wrong channel number" );

	if ( plfPower )
	{
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "MEAS:VOLT? (@%d)\n" , channel )); 
		CHK_VSA ( viRead ( pLocalHandle->sessionHandle , szReadFeedback , LOW_STRING , &count ));
			
		lfGetVoltage = atof(szReadFeedback);
		
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "MEAS:CURR? (@%d)\n" , channel )); 
		CHK_VSA ( viRead ( pLocalHandle->sessionHandle , szReadFeedback , LOW_STRING , &count ));
			
		lfGetCurrent = atof(szReadFeedback);
		
		*plfPower = lfGetVoltage * lfGetCurrent;
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerSupply_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


/***************************************** Set Voltage Protection Over ***********************************/

void* DLLEXPORT PowerSupply_SetVoltageProtectionOver ( int hInstrumentHandle , int channel , double lfVoltage )
{
	STD_ERROR       StdError							=	{0};
	
	double 			lfGetVoltage						=	0.0;
		
	int				iTryIndex							=	0;
	
	int				handle								=	0;
	
	tsHandle		*pLocalHandle						=	{0};
	
	char			szCommandString[LOW_STRING]			=	{0},  
					szReadFeedback[LOW_STRING]			=	{0};
	int				bSettingSuccess						=	0;
	
	int				count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));	
	
	IF ((( channel < pLocalHandle->iStartFromChannel ) || ( channel >= ( pLocalHandle->iStartFromChannel + pLocalHandle->iChannelsQuantity ))) , "Wrong channel number" );
	
	for ( iTryIndex = 0; iTryIndex < 5; iTryIndex++ )
	{
		sprintf( szCommandString , "VOLT:LEV %E,(@%d)\n" , lfVoltage , channel );
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommandString ));
	
		WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout , pLocalHandle->lfOpcLowLevelTimeout ); 
	
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "VOLT:LEV? (@%d)\n" , channel )); 
		CHK_VSA ( viRead ( pLocalHandle->sessionHandle , szReadFeedback , LOW_STRING , &count ));
			
		lfGetVoltage = atof(szReadFeedback);
		
		if ( fabs( lfGetVoltage - lfVoltage ) < (lfVoltage/1000.0))
		{
			bSettingSuccess = 1;
			break;
		}
		
		DelayWithEventProcessing(0.2);
	}
	
	IF (( bSettingSuccess == 0 ) , "Cann't set Voltage" );
		
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerSupply_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


/***************************************** Get Voltage Protection Over ***********************************/

void* DLLEXPORT PowerSupply_GetVoltageProtectionOver ( int hInstrumentHandle , int channel , double *plfVoltage )
{
	STD_ERROR       StdError							=	{0};
	
	double 			lfGetCurrent						=	0.0;
		
	int				iTryIndex							=	0;
	
	int				handle								=	0;
	
	tsHandle		*pLocalHandle						=	{0};
	
	char			szCommandString[LOW_STRING]			=	{0},  
					szReadFeedback[LOW_STRING]			=	{0};
	int				bSettingSuccess						=	0;
	
	int				count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));	
	
	IF ((( channel < pLocalHandle->iStartFromChannel ) || ( channel >= ( pLocalHandle->iStartFromChannel + pLocalHandle->iChannelsQuantity ))) , "Wrong channel number" );

	if ( plfVoltage )
	{
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "VOLT:LEV? (@%d)\n" , channel )); 
		CHK_VSA ( viRead ( pLocalHandle->sessionHandle , szReadFeedback , LOW_STRING , &count ));
			
		*plfVoltage = atof(szReadFeedback);
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerSupply_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


/***************************************** Set Current Protection Over ***********************************/

void* DLLEXPORT PowerSupply_SetCurrentProtectionOver ( int hInstrumentHandle , int channel , double lfCurrent )
{
	STD_ERROR       StdError							=	{0};
	
	double 			lfGetCurrent						=	0.0;
		
	int				iTryIndex							=	0;
	
	int				handle								=	0;
	
	tsHandle		*pLocalHandle						=	{0};
	
	char			szCommandString[LOW_STRING]			=	{0},  
					szReadFeedback[LOW_STRING]			=	{0};
	int				bSettingSuccess						=	0;
	
	int				count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));	
	
	IF ((( channel < pLocalHandle->iStartFromChannel ) || ( channel >= ( pLocalHandle->iStartFromChannel + pLocalHandle->iChannelsQuantity ))) , "Wrong channel number" );

	for ( iTryIndex = 0; iTryIndex < 5; iTryIndex++ )
	{
		sprintf( szCommandString , "CURR:LEV %E,(@%d)\n" , lfCurrent , channel );
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szCommandString ));
	
		WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout , pLocalHandle->lfOpcLowLevelTimeout ); 
	
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "CURR:LEV? (@%d)\n" , channel )); 
		CHK_VSA ( viRead ( pLocalHandle->sessionHandle , szReadFeedback , LOW_STRING , &count ));
			
		lfGetCurrent = atof(szReadFeedback);
		
		if ( fabs( lfGetCurrent - lfCurrent ) < (lfCurrent/1000.0))
		{
			bSettingSuccess = 1;
			break;
		}
		
		DelayWithEventProcessing(0.2);
	}
	
	IF (( bSettingSuccess == 0 ) , "Cann't set Voltage" );
		
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerSupply_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***************************************** Get Current Protection Over ***********************************/

void* DLLEXPORT PowerSupply_GetCurrentProtectionOver ( int hInstrumentHandle , int channel , double *plfCurrent )
{
	STD_ERROR       StdError							=	{0};
	
	double 			lfGetCurrent						=	0.0;
		
	int				iTryIndex							=	0;
	
	int				handle								=	0;
	
	tsHandle		*pLocalHandle						=	{0};
	
	char			szCommandString[LOW_STRING]			=	{0},  
					szReadFeedback[LOW_STRING]			=	{0};
	int				bSettingSuccess						=	0;
	
	int				count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));	
	
	IF ((( channel < pLocalHandle->iStartFromChannel ) || ( channel >= ( pLocalHandle->iStartFromChannel + pLocalHandle->iChannelsQuantity ))) , "Wrong channel number" );

	if ( plfCurrent )
	{
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "CURR:LEV? (@%d)\n" , channel )); 
		CHK_VSA ( viRead ( pLocalHandle->sessionHandle , szReadFeedback , LOW_STRING , &count ));
			
		*plfCurrent = atof(szReadFeedback);
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerSupply_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


/***************************************** Set Voltage Protection Under ***********************************/

void* DLLEXPORT PowerSupply_SetVoltageProtectionUnder ( int hInstrumentHandle , int channel , double lfVoltage )
{
	STD_ERROR                   StdError					=	{0};
	
//Error:
	
	RETURN_STDERR_POINTER;
}

/***************************************** Get Voltage Protection Under ***********************************/

void* DLLEXPORT PowerSupply_GetVoltageProtectionUnder ( int hInstrumentHandle , int channel , double *plfVoltage )
{
	STD_ERROR                   StdError					=	{0};
	
//Error:	
	
	RETURN_STDERR_POINTER;
}

/*********************************************** Set Output State *****************************************/

void* DLLEXPORT PowerSupply_SetOutputState ( int hInstrumentHandle , int channel , int iState )
{
	STD_ERROR       StdError							=	{0};
	
	int				iTryIndex							=	0;
	
	int				iGetState							=	0;
	
	int				handle								=	0;
	
	tsHandle		*pLocalHandle						=	{0};
	
	char			szCommandString[LOW_STRING]			=	{0},  
					szReadFeedback[LOW_STRING]			=	{0};
	
	int				count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));	
	
	IF ((( channel < pLocalHandle->iStartFromChannel ) || ( channel >= ( pLocalHandle->iStartFromChannel + pLocalHandle->iChannelsQuantity ))) , "Wrong channel number" );
	
	for ( iTryIndex = 0; iTryIndex < 5; iTryIndex++ )
	{
		if ( iState )
		{
			CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "OUTP:STAT ON,(@%d)\n" , channel ));
		}
		else
		{
			CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "OUTP:STAT OFF,(@%d)\n" , channel ));
		}
		
		WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout , pLocalHandle->lfOpcLowLevelTimeout ); 
	
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "OUTP:STAT? (@%d)\n" , channel )); 
		CHK_VSA ( viRead ( pLocalHandle->sessionHandle , szReadFeedback , LOW_STRING , &count ));
			
		iGetState = atoi(szReadFeedback);
		
		if ( iGetState == iState )
			break;
		
		DelayWithEventProcessing(0.2);
	}
	
	IF (( iGetState != iState ) , "Cann't set State" );
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerSupply_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}



/*********************************************** Get Output State *****************************************/

void* DLLEXPORT PowerSupply_GetOutputState ( int hInstrumentHandle , int channel , int *piState )
{
	STD_ERROR       StdError							=	{0};
	
	double 			lfGetVoltage						=	0.0;
		
	int				iTryIndex							=	0;
	
	int				handle								=	0;
	
	tsHandle		*pLocalHandle						=	{0};
	
	char			szCommandString[LOW_STRING]			=	{0},  
					szReadFeedback[LOW_STRING]			=	{0};
	
	int				count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));	
	
	IF ((( channel < pLocalHandle->iStartFromChannel ) || ( channel >= ( pLocalHandle->iStartFromChannel + pLocalHandle->iChannelsQuantity ))) , "Wrong channel number" );

	if ( piState )
	{
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "OUTP:STAT? (@%d)\n" , channel )); 
		CHK_VSA ( viRead ( pLocalHandle->sessionHandle , szReadFeedback , LOW_STRING , &count ));
			
		*piState = atoi(szReadFeedback);		
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return PowerSupply_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

/***************************************** Set Foldback Protection Mode ***********************************/

void* DLLEXPORT PowerSupply_SetFoldbackProtectionMode ( int hInstrumentHandle , int channel , int iMode )
{
	STD_ERROR                   StdError					=	{0};
//Error:	

	RETURN_STDERR_POINTER;
}

/***************************************** Get Foldback Protection Mode ***********************************/

void* DLLEXPORT PowerSupply_GetFoldbackProtectionMode ( int hInstrumentHandle , int channel , int *piMode )
{
	STD_ERROR                   StdError					=	{0};
	
//Error:	

	RETURN_STDERR_POINTER;
}

/***************************************** Set Auto Restart Mode ***********************************/

void* DLLEXPORT PowerSupply_SetAutoRestartMode ( int hInstrumentHandle , int channel , int iMode )
{
	STD_ERROR                   StdError					=	{0};
	
//Error:	
	
	RETURN_STDERR_POINTER;
}

/***************************************** Get Auto Restart Mode ***********************************/

void* DLLEXPORT PowerSupply_GetAutoRestartMode ( int hInstrumentHandle , int channel , int *piMode )
{
	STD_ERROR                   StdError					=	{0};
	
//Error:	
	
	RETURN_STDERR_POINTER;
}


/*********************************** Get Operational Status Register ***********************************/

void* DLLEXPORT PowerSupply_GetOperationalStatusRegister ( int hInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR                   StdError					=	{0};
	
//Error:	
	
	RETURN_STDERR_POINTER;
}


/*********************************** Get Alarm Status Register ***********************************/

void* DLLEXPORT PowerSupply_GetAlarmStatusRegister ( int hInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR                   StdError					=	{0};
	
//Error:	
	
	RETURN_STDERR_POINTER;
}


/*********************************** Get Error Codes Register ***********************************/

void* DLLEXPORT PowerSupply_GetErrorCodesRegister ( int hInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR                   StdError					=	{0};
	
//Error:	
	
	RETURN_STDERR_POINTER;
}


/*********************************** Get Complete Status ***********************************/

void* DLLEXPORT PowerSupply_GetCompleteStatus ( int hInstrumentHandle , int channel , char *pReturnString )
{
	STD_ERROR                   StdError					=	{0};
	
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
	
	viGetAttribute ( sessionHandle , VI_ATTR_TMO_VALUE , &iTimeout );   
	
	SetBreakOnLibraryErrors (0); 
	
	viSetAttribute ( sessionHandle , VI_ATTR_TMO_VALUE , (lfLowLevelTimeout*1E3) );  
	
	GetCurrentDateTime(&lfStartTime);
	
	do
	{
		viPrintf( sessionHandle , "*OPC?\n" );
						
		viRead( sessionHandle, szReadFeedback , LOW_STRING , &count );
	
		iOpcStatus = atoi(szReadFeedback);
		
		if ( iOpcStatus )
			break;
		
		GetCurrentDateTime(&lfCurrentTime);
		
	} while((lfCurrentTime-lfStartTime) < lfTimeout );
	
	viSetAttribute ( sessionHandle , VI_ATTR_TMO_VALUE , iTimeout );  
	
	SetBreakOnLibraryErrors (1); 
	
	return iOpcStatus;
}


void main ( void )
{
	int		hInstrumentHandle				=	0;
	
	int				InstrumentConnectStatus			=	0,
					InstrumentChanelsQuantity		=	0;
	
	int				iState							=	1;
	
	char			szReturnString[256]				=	{0};
	
	double			lfVoltage						=	0.0,
					lfCurrent						=	0.0;
	
	PowerSupply_Init ( 0 , "TCPIP0::10.0.0.53::inst0::INSTR" , &hInstrumentHandle , &InstrumentConnectStatus , &InstrumentChanelsQuantity );
	
	PowerSupply_Reset ( hInstrumentHandle );   
	PowerSupply_GetModel ( hInstrumentHandle , 1 , szReturnString );
	PowerSupply_GetRevision ( hInstrumentHandle , 1 , szReturnString );    
	
	PowerSupply_SetOutputVoltage ( hInstrumentHandle , 1 , 5.6 );
	PowerSupply_SetOutputCurrent ( hInstrumentHandle , 1 , 1.5 );
	
	PowerSupply_SetOutputState ( hInstrumentHandle , 1 , 1 ); 
		
	PowerSupply_GetOutputVoltageDefined ( hInstrumentHandle , 1 , &lfVoltage );
	PowerSupply_GetOutputVoltageActual ( hInstrumentHandle , 1 , &lfVoltage );
	
	PowerSupply_GetOutputCurrentDefined ( hInstrumentHandle , 1 , &lfCurrent );
	PowerSupply_GetOutputCurrentActual ( hInstrumentHandle , 1 , &lfCurrent );
	
	//PowerSupply_GetOutputPowerActual ( hInstrumentHandle , 1 , double *plfPower )
	
	//PowerSupply_SetVoltageProtectionOver ( hInstrumentHandle , 1 , double lfVoltage )
	//PowerSupply_GetVoltageProtectionOver ( hInstrumentHandle , 1 , &lfVoltage );
	//PowerSupply_SetVoltageProtectionUnder ( hInstrumentHandle , 1 , double lfVoltage )
	//PowerSupply_GetVoltageProtectionUnder ( hInstrumentHandle , 1 , &lfVoltage );

	PowerSupply_GetOutputState ( hInstrumentHandle , 1 , &iState ); 
	
	PowerSupply_Close ( &hInstrumentHandle ); 
	
	return;
}
