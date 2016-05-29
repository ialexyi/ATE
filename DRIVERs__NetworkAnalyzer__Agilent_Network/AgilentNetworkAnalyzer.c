
//==============================================================================
// Include files
#include "Windows.h"

#include <utility.h>
#include <visa.h>
#include "GlobalDefines.h"
#include <formatio.h>
#include "toolbox.h"
#include <userint.h>
#include <ansi_c.h>

//==============================================================================
// Constants

//==============================================================================
// Types
typedef enum { FW_TYPE_DEFAULT , FW_TYPE_A_11_22 , FW_TYPE_A_11_02 , FW_TYPE_A_09_61 , FW_TYPE_A_09_42 , FW_TYPE_A_09_20 , FW_TYPE_A_09_10 , FW_TYPE_A_08_50 , FW_TYPE_A_07_50 , FW_TYPE_A_06_04 , FW_TYPE_A_06_01 , FW_TYPE_A_04_87 , FW_TYPE_SENTINEL} teFirmwareSupport;

typedef struct
{
	ViSession			defaultRM,
						sessionHandle;
	
	char				*pLastStateFile,
						*pCalibrationName,  
						*pVendor,
						*pModel,
						*pFirmware,
						*pSerialNumber;
	
	double				lfTimeout,
						lfOpcLowLevelTimeout;
	
	double				lfDelay,
						lfEcalDelay,
						lfReadingTraceDelay;
	
	double				lfStateFileTimeout,   
						lfStateFileDelay;
	
	int					bEquipmentIsApna;
	
	tsSTD_CallBackSet	*ptCallbacks;
	
	teFirmwareSupport	tType;
	
	int					iCurrentChannel,
						iCurrentTrace,
						iCurrentWindow;
	
	int					bSwapData;
	
} tsHandle;
//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

int		WaitForOperationCompletion( int sessionHandle , double lfTimeout , double lfLowLevelTimeout );

int		RecursiveMakeDirectory( int sessionHandle , char *pszEmptyDirectory , char *pPath );

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
		
		IF ((( strstr( szIdentificationUpperText , "AGILENT TECHNOLOGIES" ) == NULL ) && ( strstr( szIdentificationUpperText ,"HEWLETT-PACKARD") == NULL )) , "Is not supported" );
	
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
	
	int								vbEquipmentPNA[32]							=	{/*E5070B*/0,/*E5071B*/0,/*E5071C*/0,/*E5072A*/0,/*E5061A*/0,/*E5062A*/0,/*E5061B*/0,/*E8361A*/1,/*E8362B*/1,/*E8363B*/1,/*E8364B*/1,/*E8361C*/1,/*E8362C*/1,/*E8363C*/1,/*E8364C*/1,/*N5230A*/1,/*N5230C*/1,/*N5241A*/1,/*N5242A*/1,/*N5244A*/1,/*N5245A*/1,/*N5264A*/1,/*N5247A*/1,/*N5221A*/1,/*N5222A*/1,/*N5224A*/1,/*N5225A*/1,/*N5227A*/1,/*N5239A*/1,/*N5231A*/1,/*N5232A*/1,/*N5234A*/1,/*N5235A*/1};								

	char							vszSupportdedModels[][16]					=	{{"E5070B"}, {"E5071B"}, {"E5071C"}, {"E5072A"}, {"E5061A"}, {"E5062A"}, {"E5061B"}, {"E8361A"}, {"E8362B"}, {"E8363B"}, {"E8364B"}, {"E8361C"}, {"E8362C"}, {"E8363C"}, {"E8364C"}, {"N5230A"}, {"N5230C"}, {"N5241A"}, {"N5242A"}, {"N5244A"}, {"N5245A"}, {"N5264A"}, {"N5247A"}, {"N5221A"}, {"N5222A"}, {"N5224A"}, {"N5225A"}, {"N5227A"}, {"N5239A"}, {"N5231A"}, {"N5232A"}, {"N5234A"}, {"N5235A"}},
																						//FW_TYPE_A_11_22 , FW_TYPE_A_11_02 , FW_TYPE_A_09_61 , FW_TYPE_A_09_42 , FW_TYPE_A_09_20 , FW_TYPE_A_09_10 , FW_TYPE_A_08_50 , FW_TYPE_A_07_50 , FW_TYPE_A_06_04 , FW_TYPE_A_06_01, FW_TYPE_A_04_87
									vszSupportdedFirmwares[][16]				=	{{"0"},{"A.11.22"}    ,{"A.11.02"}    ,{"A.09.61"}      ,{"A.09.42"}     ,{"A.09.20"}      ,{"A.09.10"}      ,{"A.08.50"}      ,{"A.07.50"}       ,{"A.06.04"}      ,{"A.06.01"}		,{"A.04.87"}};

	tsHandle						*pLocalHandle								=	{0};
									
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
		
	if ( pLocalData )
	{
		pLocalHandle = (tsHandle*)(*pLocalData);
		pLocalHandle->tType = 0;  
	}
			
	do
	{
		strcpy( szIdentificationUpperText , szIdentificationText );
		StringUpperCase (szIdentificationUpperText);
		
		if (( strstr( szIdentificationUpperText , "AGILENT TECHNOLOGIES" ) == NULL ) && ( strstr( szIdentificationUpperText ,"HEWLETT-PACKARD") == NULL ))
			break;
	
		for ( iIndex = 0; iIndex < (sizeof(vszSupportdedModels) / sizeof(vszSupportdedModels[0])); iIndex++ )
			if ( strstr( szIdentificationText , vszSupportdedModels[iIndex]) )
			{
				bSupport = 1; 
				
				if ( pLocalHandle )
					pLocalHandle->bEquipmentIsApna = vbEquipmentPNA[iIndex];
				
				break;
			}
			
		for ( iIndex = 1; iIndex < (sizeof(vszSupportdedFirmwares) / sizeof(vszSupportdedFirmwares[0])); iIndex++ )
			if ( strstr( szIdentificationText , vszSupportdedFirmwares[iIndex]) )
			{
				if ( pLocalHandle )  
					pLocalHandle->tType = iIndex; 
				
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

void* DLLEXPORT   NetworkAnalyzer_Init ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *phInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChanelsQuantity )
{
	STD_ERROR						StdError										=	{0};
	
	tsHandle						*pLocalHandle									=	{0};
	
	CmtTSVHandle 					handle											=	0;
	
	int								supported										=	0;
	
	int								iIndex											=	0;
	
	unsigned int					uiCount											=	0;
	
	char							*pTemp											=	NULL;

	tsSTD_CallBackSet				tSTD_CallBackSet								=	{0};
	
	double							lfDelay											=	0.0,
									lfTimeout										=	0.0,
									lfEcalDelay										=	3.0,    
									lfDefaultTimeout								=	0.5,
									lfStateFileDelay								=	1.0,  
									lfStateFileTimeout								=	10.0,         
									lfReadingTraceDelay								=	0.0;
									
	int								bSwapData										=	1;
	
	IF (( szRsrcAddress == NULL ) , "Address string is empty" );
	
	if ( hParentInstrumentHandle == 0 ) 
	{
		CHK_CMT ( CmtNewTSV ( sizeof(tsHandle) , &handle ));
	}
	else
		handle = hParentInstrumentHandle;
		
	if ( phInstrumentHandle )
		*phInstrumentHandle = handle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));

	pLocalHandle->iCurrentChannel = -1;
	pLocalHandle->iCurrentTrace = -1;
	pLocalHandle->iCurrentWindow = -1;
	
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

	FREE_STDERR_COPY_ERR( Equipment_Info ( pLocalHandle->sessionHandle , NULL , &pLocalHandle->pVendor , &pLocalHandle->pSerialNumber , &pLocalHandle->pModel , &pLocalHandle->pFirmware ));

	STDF_UPDATE_CALLBACK_DATA(pLocalHandle->ptCallbacks); 
	
	STDF_CONFIG_VALUE("Timeout", VAL_DOUBLE , 1 , lfTimeout , lfDefaultTimeout );	
	
	if ( lfTimeout == 0.0 )
		lfTimeout = lfDefaultTimeout;
	
	viSetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , (lfTimeout*1E3) );
	
	STDF_CONFIG_VALUE("OPC_Timeout", VAL_DOUBLE , 1 , (pLocalHandle->lfTimeout) , lfDefaultTimeout );	
	
	if ( pLocalHandle->lfTimeout == 0.0 )
		pLocalHandle->lfTimeout = lfDefaultTimeout;
	
	STDF_CONFIG_VALUE("Delay", VAL_DOUBLE , 1 , (pLocalHandle->lfDelay) , lfDelay );	 
	
	if ( pLocalHandle->lfDelay == 0.0 )
		pLocalHandle->lfDelay = lfDelay;   
	
	STDF_CONFIG_VALUE("ReadingTraceDelay", VAL_DOUBLE , 1 , (pLocalHandle->lfReadingTraceDelay) , lfReadingTraceDelay );	 
	
	if ( pLocalHandle->lfReadingTraceDelay == 0.0 )
		pLocalHandle->lfReadingTraceDelay = lfReadingTraceDelay;  	
	
	STDF_CONFIG_VALUE("OPC_LowLevel_Timeout", VAL_DOUBLE , 1 , (pLocalHandle->lfOpcLowLevelTimeout) , lfDefaultTimeout );	 
	
	if ( pLocalHandle->lfOpcLowLevelTimeout == 0.0 )
		pLocalHandle->lfOpcLowLevelTimeout = lfDefaultTimeout;
	
	STDF_CONFIG_VALUE("State_File_Timeout", VAL_DOUBLE , 1 , (pLocalHandle->lfStateFileTimeout) , lfStateFileTimeout );	 
	
	if ( pLocalHandle->lfStateFileTimeout == 0.0 )
		pLocalHandle->lfStateFileTimeout = lfStateFileTimeout;
	
	STDF_CONFIG_VALUE("State_File_Delay", VAL_DOUBLE , 1 , (pLocalHandle->lfStateFileDelay) , lfStateFileDelay );	 
	
	if ( pLocalHandle->lfStateFileDelay == 0.0 )
		pLocalHandle->lfStateFileDelay = lfStateFileDelay;   
	
	STDF_CONFIG_VALUE("ECAL_Delay", VAL_DOUBLE , 1 , (pLocalHandle->lfEcalDelay) , lfEcalDelay );	 
	
	if ( pLocalHandle->lfEcalDelay == 0.0 )
		pLocalHandle->lfEcalDelay = lfEcalDelay;   
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );

	viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );  
	DelayWithEventProcessing( pLocalHandle->lfDelay );
					
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
	
	viPrintf( pLocalHandle->sessionHandle ,":FORM:DATA REAL\n" );  
	DelayWithEventProcessing( pLocalHandle->lfDelay );
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
	
	STDF_CONFIG_VALUE("Swap_Data", VAL_INTEGER , 1 , (pLocalHandle->bSwapData) , bSwapData );	 
	
	if ( pLocalHandle->bSwapData )
	{
		viPrintf( pLocalHandle->sessionHandle ,":FORM:BORD SWAP\n" ); 	
	}
	else
	{
		viPrintf( pLocalHandle->sessionHandle ,":FORM:BORD NORM\n" ); 	
	}
	
	DelayWithEventProcessing( pLocalHandle->lfDelay );
	
	CALLOC_COPY_STRING( pLocalHandle->pCalibrationName , "Calibration1" );
	
Error:
	
	if ( InstrumentConnectStatus )
		*InstrumentConnectStatus = supported;
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_Close(int *hInstrumentHandle)
{
	STD_ERROR		StdError						=	{0};
	
	tsHandle		*pLocalHandle					=	{0};

	int				handle							=	0;
	
	IF (( hInstrumentHandle == NULL ) , "Handle is empty" );
	
	handle = *hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	viClose (pLocalHandle->sessionHandle);  
	
	viClose (pLocalHandle->defaultRM);
	
	FREE(pLocalHandle->pLastStateFile);
	FREE(pLocalHandle->pFirmware);
	FREE(pLocalHandle->pSerialNumber);
	FREE(pLocalHandle->pModel);
	FREE(pLocalHandle->pCalibrationName);

	CmtReleaseTSVPtr ( handle ); 
	
	CmtDiscardTSV (handle);
	
	*hInstrumentHandle = 0;
	
Error:

	RETURN_STDERR_POINTER;
}

void* DLLEXPORT NetworkAnalyzer_GetErrorTextMessage ( int hInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char			szCommand[LOW_STRING]				=	{0},
					szReadFeedback[LOW_STRING]			=	{0},
					szErrorDescription[LOW_STRING]		=	{0};
	
	int				count								=	0;
	
	char			*pTemp								=	NULL;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	switch ( pLocalHandle->tType )
	{
		case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
			
					strcpy( szCommand , ":SYST:ERR?\n" );
					break;
			
		default :
					strcpy( szCommand , ":SYST:ERR?\n" );
					break;
	}
	
	viPrintf( pLocalHandle->sessionHandle , szCommand );
	SetBreakOnLibraryErrors (0);
	viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
	SetBreakOnLibraryErrors (1);
	
	iError = atoi(szReadFeedback);
	
	pTemp = strchr( szReadFeedback , '"' );
	
	if ( pTemp )
	{
		pTemp++;
		
		strcpy( szErrorDescription , pTemp );  
		
		pTemp = strchr( szErrorDescription , '"' );
		
		if ( pTemp )
			*pTemp = 0;
	}
	
	viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );
	DelayWithEventProcessing( pLocalHandle->lfDelay );
	
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
 
void* DLLEXPORT   NetworkAnalyzer_Reset( int hInstrumentHandle )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char			szReadFeedback[LOW_STRING]			=	{0};
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	pLocalHandle->iCurrentChannel = -1;
	pLocalHandle->iCurrentTrace = -1;
	pLocalHandle->iCurrentWindow = -1;
	
	switch ( pLocalHandle->tType )
	{
		case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

		default :
				{
						CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "*RST\n" ));
						DelayWithEventProcessing(0.5);
						
						WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
	
				}
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_Calibration_SetFrequencyPlan ( int hInstrumentHandle , double lfStartFrequency , double lfStopFrequency , double lfPowerLevel , double lfReferenceLevel , int iNumberOfPoints )
{
	STD_ERROR				StdError							=	{0};
	
	int						iChannelNumber						=	0;
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
	
	char					szReadFeedback[STD_STRING]			=	{0};

	int						count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	//---------------------- Getting Active Channel ---------------------------------------//
	if ( pLocalHandle->iCurrentChannel < 0 )
	{
		if ( pLocalHandle->bEquipmentIsApna )
		{
			//=====****************************** PNA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SYST:ACT:CHAN?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}
		}
		else
		{
			//=====****************************** ENA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SERV:CHAN:ACT?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}		
		}
	
		pLocalHandle->iCurrentChannel = iChannelNumber;
	}
	//-------------------------------------------------------------------------------------//
	switch ( pLocalHandle->tType )
	{
		case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
		default :
				{
						CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SENS%d:FREQ:STAR %E\n" , pLocalHandle->iCurrentChannel , lfStartFrequency ));
						DelayWithEventProcessing( pLocalHandle->lfDelay );
	
						WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
	
						CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SENS%d:FREQ:STOP %E\n" , pLocalHandle->iCurrentChannel , lfStopFrequency ));
						DelayWithEventProcessing( pLocalHandle->lfDelay );
	
						WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
						
						CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SOUR%d:POW %E\n" , pLocalHandle->iCurrentChannel , lfPowerLevel ));   
						DelayWithEventProcessing( pLocalHandle->lfDelay );
						
						WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
						
						CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "DISP:WIND:TRAC:Y:RLEV %E\n" , lfReferenceLevel )); 
						DelayWithEventProcessing( pLocalHandle->lfDelay );
						
						WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );   
						
						CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENS:SWE:POIN %d\n" , iNumberOfPoints )); 
						DelayWithEventProcessing( pLocalHandle->lfDelay );
						
						WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );   
				}
	}
	
	
	
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_SetFrequency( int hInstrumentHandle , double lfStartFrequency ,  double lfStopFrequency )
{
	STD_ERROR				StdError							=	{0};
	
	int						iChannelNumber						=	0;
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
	
	char					szReadFeedback[STD_STRING]			=	{0};

	int						count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	//---------------------- Getting Active Channel ---------------------------------------//
	if ( pLocalHandle->iCurrentChannel < 0 )
	{
		if ( pLocalHandle->bEquipmentIsApna )
		{
			//=====****************************** PNA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SYST:ACT:CHAN?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}
		}
		else
		{
			//=====****************************** ENA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SERV:CHAN:ACT?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}		
		}
	
		pLocalHandle->iCurrentChannel = iChannelNumber;
	}
	
	//-------------------------------------------------------------------------------------//
	
	switch ( pLocalHandle->tType )
	{
		case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

		default :
				{
						CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SENS%d:FREQ:STAR %E\n" , pLocalHandle->iCurrentChannel , lfStartFrequency ));
						DelayWithEventProcessing( pLocalHandle->lfDelay );
	
						WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
	
						CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SENS%d:FREQ:STOP %E\n" , pLocalHandle->iCurrentChannel , lfStopFrequency ));
						DelayWithEventProcessing( pLocalHandle->lfDelay );
	
						WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
				}
	}
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


void* DLLEXPORT   NetworkAnalyzer_GetFrequency( int hInstrumentHandle , double *plfStartFrequency ,  double *plfStopFrequency )
{
	STD_ERROR				StdError							=	{0};
	
	int						iChannelNumber						=	0;
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;

	char					szReadFeedback[STD_STRING]			=	{0};

	int						count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	//---------------------- Getting Active Channel ---------------------------------------//
	if ( pLocalHandle->iCurrentChannel < 0 )
	{
		if ( pLocalHandle->bEquipmentIsApna )
		{
			//=====****************************** PNA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SYST:ACT:CHAN?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}
		}
		else
		{
			//=====****************************** ENA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SERV:CHAN:ACT?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}		
		}
	
		pLocalHandle->iCurrentChannel = iChannelNumber;
	}
	//-------------------------------------------------------------------------------------//	
	switch ( pLocalHandle->tType )
	{
		case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

		default :
				{
						if ( plfStartFrequency )
						{
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SENS%d:FREQ:STAR?\n" , pLocalHandle->iCurrentChannel ));
	
							SetBreakOnLibraryErrors (0); 
							viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
							SetBreakOnLibraryErrors (1); 
							
							*plfStartFrequency = atof(szReadFeedback);
						}
	
						if ( plfStopFrequency )
						{
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SENS%d:FREQ:STOP?\n" , pLocalHandle->iCurrentChannel ));
	
							SetBreakOnLibraryErrors (0); 
							viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
							SetBreakOnLibraryErrors (1); 
							
							*plfStopFrequency = atof(szReadFeedback);
						}
				}
	}

Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_SetPointsNum( int hInstrumentHandle , int points )
{
	STD_ERROR				StdError							=	{0};
	
	int						iChannelNumber						=	0;
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
	
	char					szReadFeedback[STD_STRING]			=	{0};

	int						count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	//---------------------- Getting Active Channel ---------------------------------------//
	if ( pLocalHandle->iCurrentChannel < 0 )
	{
		if ( pLocalHandle->bEquipmentIsApna )
		{
			//=====****************************** PNA ******************************************=====//
			
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SYST:ACT:CHAN?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}
		}
		else
		{
			//=====****************************** ENA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SERV:CHAN:ACT?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}		
		}
	
		pLocalHandle->iCurrentChannel = iChannelNumber;
	}
	//-------------------------------------------------------------------------------------//	

	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
					{
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENS:SWE:POIN %d\n" , points ));
							DelayWithEventProcessing( pLocalHandle->lfDelay );
	
							WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
					}
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
					{
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SENS%d:SWE:POIN %d\n" , pLocalHandle->iCurrentChannel , points ));
							DelayWithEventProcessing( pLocalHandle->lfDelay );
	
							WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
					}
		}		
	}
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_GetPointsNum(int hInstrumentHandle, int *points)
{
	STD_ERROR				StdError							=	{0};
	
	int						iChannelNumber						=	0;
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;

	char					szReadFeedback[STD_STRING]			=	{0};

	int						count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	//---------------------- Getting Active Channel ---------------------------------------//
	if ( pLocalHandle->iCurrentChannel < 0 )
	{
		if ( pLocalHandle->bEquipmentIsApna )
		{
			//=====****************************** PNA ******************************************=====//
			
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SYST:ACT:CHAN?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}
		}
		else
		{
			//=====****************************** ENA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SERV:CHAN:ACT?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}		
		}
	
		pLocalHandle->iCurrentChannel = iChannelNumber;
	}
	//-------------------------------------------------------------------------------------//	
	
	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
					{
						if ( points )
						{
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SENS:SWE:POIN?\n" , pLocalHandle->iCurrentChannel ));
		
							SetBreakOnLibraryErrors (0); 
							viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
							SetBreakOnLibraryErrors (1); 
							
							*points = atoi(szReadFeedback);
						}
					}
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
					{
						if ( points )
						{
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SENS%d:SWE:POIN?\n" , pLocalHandle->iCurrentChannel ));
		
							SetBreakOnLibraryErrors (0); 
							viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
							SetBreakOnLibraryErrors (1); 
							
							*points = atoi(szReadFeedback);
						}
					}
		}		
	}

Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_SelectChannel ( int hInstrumentHandle , int iChannel )
{
	STD_ERROR				StdError							=	{0};
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
	
	char					szReadFeedback[STD_STRING]			=	{0};
															
	int						count								=	0;

	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	pLocalHandle->iCurrentChannel = iChannel;  

	//---------------------- Getting Active Channel ---------------------------------------//
	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		/*
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
			default :
					{
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SYST:ACT:CHAN %d\n" , iChannel ));
	
							WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );

					}
		}
		*/
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
			default :
					{
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SERV:CHAN:ACT %d\n" , iChannel ));
							DelayWithEventProcessing( pLocalHandle->lfDelay );
							
							WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
					}
		}		
	}
	//-------------------------------------------------------------------------------------//	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


void* DLLEXPORT   NetworkAnalyzer_SelectTrace ( int hInstrumentHandle , int iTrace )
{
	STD_ERROR				StdError							=	{0};
	
	int						iChannelNumber						=	0;
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
	
	char					szReadFeedback[STD_STRING]			=	{0};
															
	int						count								=	0;

	char					**pParametersList					=	NULL;
	
	int						iNumberOfTraces						=	0;
	
	char					*pCurrent							=	NULL,
							*pNext								=	NULL,
							*pTemp								=	NULL;
							
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if ( iTrace < 1 )
		iTrace = 1;
	
	pLocalHandle->iCurrentTrace = iTrace;  
	//---------------------- Getting Active Channel ---------------------------------------//
	if ( pLocalHandle->iCurrentChannel < 0 )
	{
		if ( pLocalHandle->bEquipmentIsApna )
		{
			//=====****************************** PNA ******************************************=====//
			
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SYST:ACT:CHAN?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}
		}
		else
		{
			//=====****************************** ENA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SERV:CHAN:ACT?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}		
		}
	
		pLocalHandle->iCurrentChannel = iChannelNumber;
	}
	//-------------------------------------------------------------------------------------//	

	if ( pLocalHandle->iCurrentWindow < 0 )
		pLocalHandle->iCurrentWindow = 1;
	
	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
					{
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "CALC%d:PAR:CAT?\n" , pLocalHandle->iCurrentChannel ));  
							
							SetBreakOnLibraryErrors (0); 
							viRead( pLocalHandle->sessionHandle, szReadFeedback , STD_STRING , &count );
							SetBreakOnLibraryErrors (1); 
						
							CALLOC_ERR( pParametersList , ((count / 10)*2) , sizeof(char*));
							
							pNext = szReadFeedback;
							
							do
							{
								if ( *pNext == '"' )
									pNext++;
									
								pTemp = strchr( pNext , ',' );
								pCurrent = pNext;
								
								if ( pTemp && pCurrent )
								{
									*pTemp = 0;
									
									pNext = pTemp + 1;
									
									CALLOC_COPY_STRING( pParametersList[iNumberOfTraces] , pCurrent );
									
									iNumberOfTraces++;
									
									pTemp = strchr( pNext , ',' );  
									
									if ( pTemp )
										pNext = pTemp + 1;  
								}
								else
								{
									break;
								}
								
							} while( pNext );
							
							//"CH1_S11_1,S11,CH1_S21_2,S21,CH1_S22_3,S22,CH1_S21_4,S21,CH1_S21_5,S21,CH1_S21_6,S21,CH1_S21_7,S21"

							if ( iNumberOfTraces )
							{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "CALC%d:PAR:SEL '%s'\n" , pLocalHandle->iCurrentChannel , pParametersList[(iTrace-1)] ));   
								DelayWithEventProcessing( pLocalHandle->lfDelay );
							}   
							
							WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
							
							break;
					}
					
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:

			default :
					{
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "CALC%d:PAR:MNUM:SEL %d\n" , pLocalHandle->iCurrentChannel , iTrace ));   
							DelayWithEventProcessing( pLocalHandle->lfDelay );
							
							WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
					}
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
					{
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":CALC%d:PAR%d:SEL\n" , pLocalHandle->iCurrentChannel , iTrace ));
							DelayWithEventProcessing( pLocalHandle->lfDelay );
							
							WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
					}
		}
	}
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	FREE_LIST( pParametersList , iNumberOfTraces );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_SelectWindow ( int hInstrumentHandle , int iWindow )
{
	STD_ERROR				StdError							=	{0};
	
	int						iChannelNumber						=	0;
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
	
	char					szReadFeedback[STD_STRING]			=	{0};
															
	int						count								=	0;

	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	pLocalHandle->iCurrentWindow = iWindow;  
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_SetElectricalDelayTime( int hInstrumentHandle , int iChannel , double lfTime )
{
	STD_ERROR				StdError							=	{0};
	
	int						iChannelNumber						=	0;
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
	
	char					szReadFeedback[STD_STRING]			=	{0};
															
	int						count								=	0;

	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	//---------------------- Getting Active Channel ---------------------------------------//
	if ( pLocalHandle->iCurrentChannel < 0 )
	{
		if ( pLocalHandle->bEquipmentIsApna )
		{
			//=====****************************** PNA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SYST:ACT:CHAN?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}
		}
		else
		{
			//=====****************************** ENA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SERV:CHAN:ACT?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}		
		}
	
		pLocalHandle->iCurrentChannel = iChannelNumber;
	}
	//-------------------------------------------------------------------------------------//	

	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
			default :
					{
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":CALC%d:CORR:EDEL:TIME %E\n" , pLocalHandle->iCurrentChannel , lfTime ));
							DelayWithEventProcessing( pLocalHandle->lfDelay );
							
							WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
						
							break;
					}
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
			default :

					{
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":CALC%d:TRAC%d:CORR:EDEL:TIME %E\n" , pLocalHandle->iCurrentChannel , iChannel , lfTime ));
							DelayWithEventProcessing( pLocalHandle->lfDelay );
							
							WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
						
							break;
					}
		}
	}
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


void* DLLEXPORT   NetworkAnalyzer_SelectMathFunction ( int hInstrumentHandle , int iTrace , char cFunction )
{
	STD_ERROR				StdError							=	{0};
	
	int						iChannelNumber						=	0;
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;

	char					szReadFeedback[STD_STRING]			=	{0};

	int						count								=	0;

	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	FREE_STDERR_COPY_ERR( NetworkAnalyzer_SelectTrace ( hInstrumentHandle , iTrace )); 
	FREE_STDERR;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
		
	viPrintf( pLocalHandle->sessionHandle , "*WAI;\n" ); 
	DelayWithEventProcessing( pLocalHandle->lfDelay );
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
							
	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
					{
							switch(cFunction)
							{
								case '/':
										CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "CALC%d:MATH:FUNC DIV\n" , pLocalHandle->iCurrentChannel ));  
										break;
				
								case '*':
										CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "CALC%d:MATH:FUNC MULT\n" , pLocalHandle->iCurrentChannel ));  
										break;
				
								case '-':
										CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "CALC%d:MATH:FUNC SUBT\n" , pLocalHandle->iCurrentChannel ));  
										break;
				
								case '+':
										CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "CALC%d:MATH:FUNC ADD\n" , pLocalHandle->iCurrentChannel ));  
										break;						
				
								default:
			
										CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "CALC%d:MATH:FUNC NORM\n" , pLocalHandle->iCurrentChannel ));  
										break;
							}
						
							DelayWithEventProcessing( pLocalHandle->lfDelay );
							
							WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
					}
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
					{
							switch(cFunction)
							{
								case '/':
										CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":CALC%d:MATH:FUNC DIV\n" , pLocalHandle->iCurrentChannel ));  
										break;
				
								case '*':
										CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":CALC%d:MATH:FUNC MULT\n" , pLocalHandle->iCurrentChannel ));  
										break;
				
								case '-':
										CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":CALC%d:MATH:FUNC SUBT\n" , pLocalHandle->iCurrentChannel ));  
										break;
				
								case '+':
										CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":CALC%d:MATH:FUNC ADD\n" , pLocalHandle->iCurrentChannel ));  
										break;						
				
								default:
			
										CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":CALC%d:MATH:FUNC NORM\n" , pLocalHandle->iCurrentChannel ));  
										break;
							}
						
							DelayWithEventProcessing( pLocalHandle->lfDelay );
							
							WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
					}
		}	
	}
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


void* DLLEXPORT   NetworkAnalyzer_SetTraceToMathMemory ( int hInstrumentHandle , int iTrace )
{
	STD_ERROR		StdError							=	{0};
	
	int				iChannelNumber						=	0;
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;

	char			szReadFeedback[STD_STRING]			=	{0};
	
	int						count							=	0;

	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	FREE_STDERR_COPY_ERR( NetworkAnalyzer_SelectTrace ( hInstrumentHandle , iTrace )); 
	FREE_STDERR;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
		
	viPrintf( pLocalHandle->sessionHandle , "*WAI;\n" ); 
	DelayWithEventProcessing( pLocalHandle->lfDelay );
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
	
	//-------------------------------------------------------------------------------------//	
	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
					{
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "CALC%d:MATH:MEM\n" , pLocalHandle->iCurrentChannel ));
							DelayWithEventProcessing( pLocalHandle->lfDelay );
						
							viPrintf( pLocalHandle->sessionHandle , "*WAI;\n" ); 
							DelayWithEventProcessing( pLocalHandle->lfDelay );
							
							WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
					}
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
					{
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":CALC%d:MATH:MEM\n" , pLocalHandle->iCurrentChannel ));
							DelayWithEventProcessing( pLocalHandle->lfDelay );
						
							viPrintf( pLocalHandle->sessionHandle , "*WAI;\n" ); 
							DelayWithEventProcessing( pLocalHandle->lfDelay );
							
							WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
					}
		}
	}
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_GetMeasureVectors(int hInstrumentHandle, int iGetDataType , double **plfFirstVector , double **plfSecondVector , double **plfThirdVector , double **plfFourthVector )
{
	STD_ERROR				StdError						=	{0};

//Error: 
	   
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


void* DLLEXPORT   NetworkAnalyzer_GetTrace ( int hInstrumentHandle , int iTrace , int iMeasurementType , int iMeasurementFormat , double lfTimeout , double **plfFrequency , double **plfTrace , int *piNumberOfPoints )
{
 	STD_ERROR				StdError						=	{0};
	
	int						iNumberOfPoints					=	0;
							
	double 					*plfLocalTrace					=	NULL,
							*plfLocalFrequency				=	NULL;

	double					*pReadBuffer					=	NULL; 
	
	float					*pShortReadBuffer				=	NULL;
	
	unsigned int			uiCount							=	0,
							uiNumberOfBinaryData			=	0;
	
	int						iIndex							=	0;
	
	char					szReadFeedback[LOW_STRING]		=	{0};
	
	int						iChannelNumber					=	0;
	
	double					lfStartFrequency				=	0.0,
							lfStopFrequency					=	0.0,
							lfStepFrequency					=	0.0,
							lfCurrentFrequency				=	0.0;
	
	tsHandle				*pLocalHandle					=	{0};

	int						handle							=	0;

	int						count							=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	if ( iTrace > 0 )
	{
		FREE_STDERR_COPY_ERR( NetworkAnalyzer_SelectTrace ( hInstrumentHandle , iTrace )); 
		FREE_STDERR;
	}
	
	FREE_STDERR_COPY_ERR( NetworkAnalyzer_GetPointsNum( hInstrumentHandle, &iNumberOfPoints )); 
	FREE_STDERR;  
	
	FREE_STDERR_COPY_ERR( NetworkAnalyzer_GetFrequency( hInstrumentHandle , &lfStartFrequency ,  &lfStopFrequency )); 
	FREE_STDERR; 
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
		
	viPrintf( pLocalHandle->sessionHandle , "*WAI;\n" ); 
	DelayWithEventProcessing( pLocalHandle->lfDelay );
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
	
	//-------------------------------------------------------------------------------------//	
	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
					{   
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "CALC%d:DATA? FDATA\n" , pLocalHandle->iCurrentChannel )); 
	
							DelayWithEventProcessing( pLocalHandle->lfReadingTraceDelay ); 
							
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
							
							if (( iNumberOfPoints * sizeof(float)) == uiNumberOfBinaryData )
							{
								CALLOC_ERR( pShortReadBuffer , uiNumberOfBinaryData , sizeof(char));  
							
								viRead ( pLocalHandle->sessionHandle ,(ViPBuf)pShortReadBuffer , uiNumberOfBinaryData , &uiCount ); 
							}
							else
							{
								CALLOC_ERR( pReadBuffer , uiNumberOfBinaryData , sizeof(char)); 
							
								viRead ( pLocalHandle->sessionHandle ,(ViPBuf)pReadBuffer , uiNumberOfBinaryData , &uiCount ); 
							}
							
							SetBreakOnLibraryErrors (1);
						
							IF (( uiNumberOfBinaryData != uiCount ) , "Wrong trace reading format" );     

							CALLOC_ERR( plfLocalTrace , iNumberOfPoints , sizeof(double));   
	
							if ( pReadBuffer )
							{
								for ( iIndex = 0; iIndex < iNumberOfPoints; iIndex++ )
									plfLocalTrace[iIndex] = pReadBuffer[iIndex];
							}
							
							if ( pShortReadBuffer )
							{
								for ( iIndex = 0; iIndex < iNumberOfPoints; iIndex++ )
									plfLocalTrace[iIndex] = (double)pShortReadBuffer[iIndex];
							}
							
							SetBreakOnLibraryErrors (0);
							viRead ( pLocalHandle->sessionHandle ,(ViPBuf)szReadFeedback , 1 , &uiCount );  
							SetBreakOnLibraryErrors (1);
					}
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
					{   
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":CALC%d:DATA:FDAT?\n" , pLocalHandle->iCurrentChannel )); 
	
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

							CALLOC_ERR( plfLocalTrace , iNumberOfPoints , sizeof(double));   
	
							for ( iIndex = 0; iIndex < iNumberOfPoints; iIndex++ )
								plfLocalTrace[iIndex] = pReadBuffer[(iIndex*2)];

							SetBreakOnLibraryErrors (0);
							viRead ( pLocalHandle->sessionHandle ,(ViPBuf)szReadFeedback , 1 , &uiCount );  
							SetBreakOnLibraryErrors (1);
					}
		}
	}	
	
	if ( plfFrequency )
	{
		CALLOC_ERR( plfLocalFrequency , iNumberOfPoints , sizeof(double))
	
		lfStepFrequency = ( lfStopFrequency - lfStartFrequency ) / ( iNumberOfPoints - 1 );

		lfCurrentFrequency = lfStartFrequency;

		for ( iIndex = 0; iIndex < iNumberOfPoints; iIndex++ )
		{
			plfLocalFrequency[iIndex] = lfCurrentFrequency;	

			lfCurrentFrequency += lfStepFrequency;
		}

		(*plfFrequency) = plfLocalFrequency;  
	}
	
Error: 
	   
	if (plfTrace)
		*plfTrace = plfLocalTrace;
	else
		FREE(plfLocalTrace);
	
	if ( piNumberOfPoints )
		*piNumberOfPoints = iNumberOfPoints;
	
	FREE(pReadBuffer);
	FREE(pShortReadBuffer);
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}
	
	

void* DLLEXPORT   NetworkAnalyzer_SetMeasureType(int hInstrumentHandle , int iChannel , int iInSparam, int iConversation)
{			   
	STD_ERROR				StdError				=	{0};

//Error: 
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_DisplaySelect(int hInstrumentHandle, int iChannel, int iDataMode , int iDataFormat )
{			   
	STD_ERROR				StdError				=	{0};

//Error: 
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_AutoScale(int hInstrumentHandle )
{			   
	STD_ERROR				StdError				=	{0};

//Error: 
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_WaitSweep(int hInstrumentHandle, int iChannel , int iCount )
{
	STD_ERROR				StdError							=	{0};
	
	int						iChannelNumber						=	0;
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
	
	char					szReadFeedback[STD_STRING]			=	{0};
															
	int						count								=	0;

	double					lfSweepTime							=	0.0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	//---------------------- Getting Active Channel ---------------------------------------//
	if ( pLocalHandle->iCurrentChannel < 0 )
	{
		if ( pLocalHandle->bEquipmentIsApna )
		{
			//=====****************************** PNA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SYST:ACT:CHAN?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}
		}
		else
		{
			//=====****************************** ENA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SERV:CHAN:ACT?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}		
		}
	
		pLocalHandle->iCurrentChannel = iChannelNumber;
	}
	//-------------------------------------------------------------------------------------//	

	if ( pLocalHandle->bEquipmentIsApna )														
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
			default :
					{
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENS%d:SWE:TIME?\n" , pLocalHandle->iCurrentChannel ));
							DelayWithEventProcessing( pLocalHandle->lfDelay );
							
							SetBreakOnLibraryErrors (0); 
							viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
							SetBreakOnLibraryErrors (1); 
					
							lfSweepTime = atof(szReadFeedback);
							
							DelayWithEventProcessing(lfSweepTime*iCount);
							
							WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
							
							break;
					}
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
			default :
					{
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENS%d:SWE:TIME?\n" , pLocalHandle->iCurrentChannel ));
							DelayWithEventProcessing( pLocalHandle->lfDelay );
							
							SetBreakOnLibraryErrors (0); 
							viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
							SetBreakOnLibraryErrors (1); 
					
							lfSweepTime = atof(szReadFeedback);
							
							DelayWithEventProcessing(lfSweepTime*iCount);
							
							WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
							
							break;
					}
		}
	}
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_Avarage(int hInstrumentHandle, int iChannel, int bEnable , int iAverageValue )
{
	STD_ERROR				StdError							=	{0};
	
	int						iChannelNumber						=	0;
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
	
	char					szReadFeedback[STD_STRING]			=	{0};
															
	int						count								=	0;

	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	//---------------------- Getting Active Channel ---------------------------------------//
	if ( pLocalHandle->iCurrentChannel < 0 )
	{
		if ( pLocalHandle->bEquipmentIsApna )
		{
			//=====****************************** PNA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SYST:ACT:CHAN?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}
		}
		else
		{
			//=====****************************** ENA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SERV:CHAN:ACT?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}		
		}
	
		pLocalHandle->iCurrentChannel = iChannelNumber;
	}
	//-------------------------------------------------------------------------------------//	

	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
			default :
					{
							if ( bEnable )
							{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENS%d:AVER ON\n" , pLocalHandle->iCurrentChannel ));
								DelayWithEventProcessing( pLocalHandle->lfDelay );
							
								WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
								
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENS%d:AVER:COUN %d\n" , pLocalHandle->iCurrentChannel , iAverageValue ));
								DelayWithEventProcessing( pLocalHandle->lfDelay );
							
								WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
							}
							else
							{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENS%d:AVER OFF\n" , pLocalHandle->iCurrentChannel ));
								DelayWithEventProcessing( pLocalHandle->lfDelay );
							
								WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
							}
							
							break;
					}
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
			default :
					{
							if ( bEnable )
							{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENS%d:AVER ON\n" , pLocalHandle->iCurrentChannel ));
								DelayWithEventProcessing( pLocalHandle->lfDelay );
							
								WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
								
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENS%d:AVER:COUN %d\n" , pLocalHandle->iCurrentChannel , iAverageValue ));
								DelayWithEventProcessing( pLocalHandle->lfDelay );
							
								WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
								
							}
							else
							{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENS%d:AVER OFF\n" , pLocalHandle->iCurrentChannel ));
								DelayWithEventProcessing( pLocalHandle->lfDelay );
							
								WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
							}
							
							break;
					}
		}
	}
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_ClearAvarage(int hInstrumentHandle, int iChannel )
{
	STD_ERROR				StdError							=	{0};
	
	int						iChannelNumber						=	0;
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
	
	char					szReadFeedback[STD_STRING]			=	{0};
															
	int						count								=	0;

	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	//---------------------- Getting Active Channel ---------------------------------------//
	if ( pLocalHandle->iCurrentChannel < 0 )
	{
		if ( pLocalHandle->bEquipmentIsApna )
		{
			//=====****************************** PNA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SYST:ACT:CHAN?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}
		}
		else
		{
			//=====****************************** ENA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SERV:CHAN:ACT?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}		
		}
	
		pLocalHandle->iCurrentChannel = iChannelNumber;
	}
	//-------------------------------------------------------------------------------------//	

	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
			default :
					{
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENS%d:AVER:CLE\n" , pLocalHandle->iCurrentChannel ));
							DelayWithEventProcessing( pLocalHandle->lfDelay );
							
							WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
							
							break;
					}
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
			default :
					{
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENS%d:AVER:CLE\n" , pLocalHandle->iCurrentChannel ));
							DelayWithEventProcessing( pLocalHandle->lfDelay );
							
							WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
							
							break;
					}
		}
	}
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_AvarageSmooth(int hInstrumentHandle, int iChannel, char *szSmoothRequest , double lfSetSmooth , double lfCurrentSmooth )
{			   
	STD_ERROR				StdError				=	{0};

//Error: 
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_Smooth(int hInstrumentHandle, int iChannel, int iState , double lfSetSmooth , int iNumberPoints )
{		
	STD_ERROR				StdError				=	{0};

//Error: 
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_SetBandWidth(int hInstrumentHandle, int iChannel , double  lfBW_Value , char *szBW_Request , double  *lfGetBW_Value )
{			   
	STD_ERROR				StdError				=	{0};

//Error: 
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_SetFrequencySweepType(int hInstrumentHandle, int iChannel , int iFrequencySweepType )
{			   
	STD_ERROR				StdError				=	{0};

//Error: 
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_SetPower(int hInstrumentHandle, int iChannel , double lfPower , char *szPowerRequest , int iPortNum )
{			   
	STD_ERROR				StdError							=	{0};
	
	int						iChannelNumber						=	0;
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
	
	char					szReadFeedback[STD_STRING]			=	{0};

	int						count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));

	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SOUR%d:POW %E\n" , pLocalHandle->iCurrentChannel , lfPower ));   

	DelayWithEventProcessing( pLocalHandle->lfDelay );
						
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_GetPower(int hInstrumentHandle, int iChannel , double *lfPower , int iPortNum )
{			   
	STD_ERROR				StdError				=	{0};

//Error: 
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_SetPowerRange(int hInstrumentHandle, int iChannel , int iRange , char *szRangeRequest , int *iCurrentRange )
{			   
	STD_ERROR				StdError				=	{0};

//Error: 
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_SetPowerSweep(int hInstrumentHandle, int iChannel , double lfStartPower , double lfStopPower , char *szSweepRequest , double *lfGetStartPower , double *lfGetStopPower )
{			   
	STD_ERROR				StdError				=	{0};

//Error: 
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_SetSweepTime(int hInstrumentHandle, int iChannel , double lfSweepTime , char *szSweepTimeRequest , double *lfGetSweepTime )
{			   
	STD_ERROR				StdError				=	{0};

//Error: 
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_SetMarker(int hInstrumentHandle, int iChannel , double lfSetValue , int iMarker , char *szMarkerRequest )
{
	STD_ERROR				StdError				=	{0};

//Error: 
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_GetMarker(int hInstrumentHandle, int iChannel , int iMarker , char *szMarkerRequest , double *lfGetMarkerOneValue  , double *lfGetMarkerTwoValue , double *lfGetMarkerThreeValue , double *lfGetMarkerFourValue )
{
	STD_ERROR				StdError				=	{0};

//Error: 
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}


void* DLLEXPORT NetworkAnalyzer_SendFile( int hInstrumentHandle ,char *szLocalFileName ,char *szStateFileName  )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szLoadNetworkState[512]				=	{0},
							szDirectoryPath[512]				=	{0},
							szFileSize[32]						=	{0};
	
	int						iActualSize							=	0;
	
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
	
	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
					{		

							viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );
			
							strcpy( szDirectoryPath , szStateFileName );
			
							pTemp = strrchr( szDirectoryPath , '\\' );
			
							if ( pTemp )
							{
								*pTemp = 0;
							}
			
							if ( strstr( szDirectoryPath , ":\\" ))
							{
								RecursiveMakeDirectory( pLocalHandle->sessionHandle , "\"0.0\"\n" , szDirectoryPath );
							}
			
							viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );
			
							sprintf( szLoadNetworkState , ":MMEM:TRAN \"%s\",#%d%s" , szStateFileName , strlen(szFileSize) , szFileSize );   
	
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szLoadNetworkState ));
			
							CHK_VSA ( viBufWrite( pLocalHandle->sessionHandle , pFileBuffer , iFileSize ,&iWriteCount ));
			
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "\n" )); 
							
					}
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
					{
							viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );
			
							strcpy( szDirectoryPath , szStateFileName );
			
							pTemp = strrchr( szDirectoryPath , '\\' );
			
							if ( pTemp )
							{
								*pTemp = 0;
							}
			
							if ( strstr( szDirectoryPath , ":\\" ))
							{
								RecursiveMakeDirectory( pLocalHandle->sessionHandle , "\"0.0\"\n" , szDirectoryPath );
							}
			
							viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );
			
							sprintf( szLoadNetworkState , ":MMEM:TRAN \"%s\",#%d%s" , szStateFileName , strlen(szFileSize) , szFileSize );   
	
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szLoadNetworkState ));
			
							CHK_VSA ( viBufWrite( pLocalHandle->sessionHandle , pFileBuffer , iFileSize ,&iWriteCount ));
			
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "\n" )); 
							
					}
		}
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	FREE(pFileBuffer);
	
	if ( iFileHandle )
		CloseFile ( iFileHandle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

void* DLLEXPORT Equipment_SendStateFile ( int hInstrumentHandle , char *szLocalFileName ,char *szStateFileName  )
{
	return NetworkAnalyzer_SendFile( hInstrumentHandle , szLocalFileName , szStateFileName );	
}

void* DLLEXPORT NetworkAnalyzer_ReceiveFile( int hInstrumentHandle ,char *szLocalFileName ,char *szStateFileName  )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szLoadNetworkState[512]				=	{0},
							szReadFeedback[LOW_STRING]			=	{0};
	
	int						iActualSize							=	0;
	
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
	
	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
					{
							viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );
			
							sprintf( szLoadNetworkState , ":MMEM:TRAN? \"%s\"\n" , szStateFileName );   
	
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szLoadNetworkState ));
			
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
						
							
					}
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
					{
							viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );
			
							sprintf( szLoadNetworkState , ":MMEM:TRAN? \"%s\"\n" , szStateFileName );   
	
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szLoadNetworkState ));
			
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
						
							
					}
		}
	}
	
Error: 

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	FREE(pReadBuffer);
	
	if ( iFileHandle )
		CloseFile ( iFileHandle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT Equipment_ReceiveStateFile ( int hInstrumentHandle , char *szLocalFileName ,char *szStateFileName  )
{
	return NetworkAnalyzer_ReceiveFile( hInstrumentHandle , szLocalFileName , szStateFileName );	
}

void* DLLEXPORT NetworkAnalyzer_GetFileCatalog ( int hInstrumentHandle , char *szPath , char ***szFileNames , int **pvFileSizes , int *piNumberOfFiles )
{
	STD_ERROR               StdError                  			=   {0};        

	char					szLoadNetworkCatalog[512]			=	{0},
							szPreviousFeedback[512]				=	{0},
							szReadFeedback[LOW_STRING]			=	{0}, 
							szCurrentBuffer[512]				=	{0},
							szLoadPath[512]						=	{0},
							szDirectoryName[64]					=	{0};  
	
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
							
	int						bFirstTime							=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );

	handle = hInstrumentHandle;
	
	bFirstTime = 1;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
					{
							viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );

							if ( szPath )
							{
								strcpy( szLoadPath , szPath );
				
								iLength = strlen(szLoadPath);
				
								if ( szLoadPath[iLength-1] != '\\' )
									strcat( szLoadPath , "\\" );
				
								sprintf( szLoadNetworkCatalog , ":MMEM:CAT? \"%s\"\n" , szLoadPath );   
							}
							else
								sprintf( szLoadNetworkCatalog , ":MMEM:CAT? \"C:\\\"\n" );   
			
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szLoadNetworkCatalog ));
			
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
		
								if ( bFirstTime )
								{
									bFirstTime = 0;
									
									pNext = strchr( szCurrentBuffer , '/"' );  
									
									if ( pNext )
										pNext++;
								}
								else
								{
									pNext = szCurrentBuffer;
								}
		
								do
								{
									strcpy( szPreviousFeedback , pNext );
			
									pCurrent = pNext;
									
									pTemp = strstr( pNext , "," );
			
									if ( pTemp == NULL )
									{
										pTemp = strchr( szCurrentBuffer , '/"' );  
									
										if ( pTemp == NULL ) 
											break;
									}
									
									*pTemp = 0;
									
									pNext = pTemp+1;
			
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
			
									pTemp = strchr( pCurrent , '\\' );
									
									if ( pTemp )
									{
										*pTemp = 0;
										
										sprintf( szDirectoryName , "[%s]" , pCurrent );
										
										CALLOC_COPY_STRING( (*szFileNames)[iFileIndex] , szDirectoryName ); 
										
										if ( pvFileSizes )
											(*pvFileSizes)[iFileIndex] = -1;
									}
									else
									{
										CALLOC_COPY_STRING( (*szFileNames)[iFileIndex] , pCurrent );
										
										if ( pvFileSizes )
											(*pvFileSizes)[iFileIndex] = atoi(pNext);
									}
			
									iFileIndex++;
			
								} while(1);
		
							} while(1);
						}
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
						{
							viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );

							if ( szPath )
							{
								strcpy( szLoadPath , szPath );
				
								iLength = strlen(szLoadPath);
				
								if ( szLoadPath[iLength-1] != '\\' )
									strcat( szLoadPath , "\\" );
				
								sprintf( szLoadNetworkCatalog , ":MMEM:CAT? \"%s\"\n" , szLoadPath );   
							}
							else
								sprintf( szLoadNetworkCatalog , ":MMEM:CAT? \"C:\\\"\n" );   
			
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szLoadNetworkCatalog ));
			
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
		
								if ( bFirstTime )
								{
									bFirstTime = 0;
									
									pNext = strchr( szCurrentBuffer , ',' );  
									
									if ( pNext )
										pNext++;
								}
								else
								{
									pNext = szCurrentBuffer;
								}
		
								do
								{
									strcpy( szPreviousFeedback , pNext );
			
									pTemp = strstr( pNext , "," );
			
									if ( pTemp == NULL )
										break;
			
									pCurrent = pTemp+1;
			
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
			
									pTemp = strchr( pCurrent , '\\' );
									
									if ( pTemp )
									{
										*pTemp = 0;
										
										sprintf( szDirectoryName , "[%s]" , pCurrent );
										
										CALLOC_COPY_STRING( (*szFileNames)[iFileIndex] , szDirectoryName ); 
										
										if ( pvFileSizes )
											(*pvFileSizes)[iFileIndex] = -1;
									}
									else
									{
										CALLOC_COPY_STRING( (*szFileNames)[iFileIndex] , pCurrent );
										
										if ( pvFileSizes )
											(*pvFileSizes)[iFileIndex] = atoi(pNext);
									}
			
									iFileIndex++;
			
								} while(1);
		
							} while(1);
						}
		}
	}
	
Error: 
	
	if ( piNumberOfFiles )
		*piNumberOfFiles = iNumberOfFile;
	
	if ( handle )
	{
		if ( pLocalHandle->sessionHandle )
			viSetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , iTimeout );  
		
		CmtReleaseTSVPtr ( handle );  
	}
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT Equipment_GetFileCatalog( int hInstrumentHandle , char *szPath , char ***szFileNames , int **pvFileSizes , int *piNumberOfFiles )  
{
	return NetworkAnalyzer_GetFileCatalog( hInstrumentHandle , szPath , szFileNames , pvFileSizes , piNumberOfFiles );	
}

void* DLLEXPORT Equipment_MakeFileCatalog( int hInstrumentHandle , char *szPath )  
{
	STD_ERROR               StdError                  			=   {0};        

	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
							
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );

	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
						{
							RecursiveMakeDirectory( hInstrumentHandle , "\"0.0\"\n" , szPath );	
						}
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
						{
							RecursiveMakeDirectory( hInstrumentHandle , "\"0.0\"\n" , szPath );	
						}
		}
	}
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );  
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT NetworkAnalyzer_DeleteFile( int hInstrumentHandle , char *szStateFileName  )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szDeleteNetworkState[512]			=	{0},
							szMessage[LOW_STRING]				=	{0};
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
							
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );

	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
						{
							viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );

							sprintf( szDeleteNetworkState , ":MMEM:DEL \"%s\"\n" , szStateFileName );   
			
							sprintf( szMessage , "Are you sure you want to delete state file\n\"%s\"?" , szStateFileName );
	
							if ( ConfirmPopup ("Deleting Equipment State File", szMessage) )
							{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szDeleteNetworkState ));
							}
						}
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
						{
							viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );

							sprintf( szDeleteNetworkState , ":MMEM:DEL \"%s\"\n" , szStateFileName );   
			
							sprintf( szMessage , "Are you sure you want to delete state file\n\"%s\"?" , szStateFileName );
	
							if ( ConfirmPopup ("Deleting Equipment State File", szMessage) )
							{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szDeleteNetworkState ));
							}
						}
		}
	}
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );  
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT Equipment_DeleteStateFile ( int hInstrumentHandle , char *szStateFileName  )
{
	return NetworkAnalyzer_DeleteFile( hInstrumentHandle , szStateFileName );	
}

void* DLLEXPORT NetworkAnalyzer_DeleteFileCatalog( int hInstrumentHandle , char *szCatalogName  )  
{
	STD_ERROR               StdError                  			=   {0};        

	char					szDeleteNetworkCatalog[512]			=	{0},
							szMessage[LOW_STRING]				=	{0};
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
							
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );

	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
						{
							viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );

							sprintf( szDeleteNetworkCatalog , ":MMEM:RDIR \"%s\"\n" , szCatalogName );   
			
							sprintf( szMessage , "Are you sure you want to delete catalog \n\"%s\"?" , szCatalogName );
	
							if ( ConfirmPopup ("Deleting Equipment State File", szMessage) )
							{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szDeleteNetworkCatalog ));
							}
						}
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
						{
							viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );

							sprintf( szDeleteNetworkCatalog , ":MMEM:RDIR \"%s\"\n" , szCatalogName );   
			
							sprintf( szMessage , "Are you sure you want to delete catalog \n\"%s\"?" , szCatalogName );
	
							if ( ConfirmPopup ("Deleting Equipment State File", szMessage) )
							{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szDeleteNetworkCatalog ));
							}
						}
		}
	}
	
Error: 
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );  
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT Equipment_DeleteFileCatalog ( int hInstrumentHandle , char *szCatalogName  )
{
	return NetworkAnalyzer_DeleteFileCatalog( hInstrumentHandle , szCatalogName );	
}

void* DLLEXPORT   NetworkAnalyzer_ReCallRegister(int hInstrumentHandle , char *szRegister_FileName )
{
	STD_ERROR						StdError										=	{0};
	
	tsHandle						*pLocalHandle									=	{0};

	int								handle											=	0;
	
	char							szReadFeedback[STD_STRING]						=	{0};
	
	int								iTryIndex										=	0;

	tsSTD_CallBackSet				tSTD_CallBackSet								=	{0};
	
	int								count											=	0;

	int								bGetLock										=	0;
	
	int								iError											=	0;
	
	char							*pFileNamePath									=	NULL;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	IF (( szRegister_FileName == NULL ) , "State is empty" ); 
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	bGetLock = 1;
	
	STDF_UPDATE_CALLBACK_DATA(pLocalHandle->ptCallbacks);   
	
	pLocalHandle->iCurrentChannel = -1;
	pLocalHandle->iCurrentTrace = -1;
	pLocalHandle->iCurrentWindow = -1;
	
	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
					{
						for ( iTryIndex = 0; iTryIndex < 3; iTryIndex++ )
						{
							viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );  
							
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":MMEM:LOAD \"%s\"\n" , szRegister_FileName ));
						
							DelayWithEventProcessing(pLocalHandle->lfStateFileDelay);
			
							viPrintf( pLocalHandle->sessionHandle , ":SYST:ERR?\n" );
	
							SetBreakOnLibraryErrors (0);
							viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
							SetBreakOnLibraryErrors (1);
	
							iError = atoi(szReadFeedback);
	
							if ( iError == 0 )
							{
								FREE_CALLOC_COPY_STRING(pLocalHandle->pLastStateFile,szRegister_FileName); 
					
								viPrintf( pLocalHandle->sessionHandle ,":FORM:DATA REAL\n" );  
								DelayWithEventProcessing( pLocalHandle->lfDelay );
							
								WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );
	
								if ( pLocalHandle->bSwapData )
								{
									viPrintf( pLocalHandle->sessionHandle ,":FORM:BORD SWAP\n" ); 	
								}
								else
								{
									viPrintf( pLocalHandle->sessionHandle ,":FORM:BORD NORM\n" ); 	
								}	
							
								DelayWithEventProcessing( pLocalHandle->lfDelay );
								
								break;
							}
				
							STDF_GET_EQUIPMENT_STATE_FILE( szRegister_FileName , pFileNamePath );
				
							CHK_CMT ( CmtReleaseTSVPtr ( handle )); 
							bGetLock = 0;
				
							FREE_STDERR_COPY_ERR( NetworkAnalyzer_SendFile( handle , pFileNamePath , szRegister_FileName ));    
				
							FREE(pFileNamePath);
				
							CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
							bGetLock = 1;
						}
			}
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
					{
						
						for ( iTryIndex = 0; iTryIndex < 3; iTryIndex++ )
						{
							viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );
		
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":MMEM:LOAD \"%s\"\n" , szRegister_FileName ));
	
							DelayWithEventProcessing(pLocalHandle->lfStateFileDelay);
			
							viPrintf( pLocalHandle->sessionHandle , ":SYST:ERR?\n" );
	
							SetBreakOnLibraryErrors (0);
							viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
							SetBreakOnLibraryErrors (1);
	
							iError = atoi(szReadFeedback);
	
							if ( iError != -256 )
							{
								FREE_CALLOC_COPY_STRING(pLocalHandle->pLastStateFile,szRegister_FileName); 
					
								break;
							}
				
							STDF_GET_EQUIPMENT_STATE_FILE( szRegister_FileName , pFileNamePath );
				
							CHK_CMT ( CmtReleaseTSVPtr ( handle )); 
							bGetLock = 0;
				
							FREE_STDERR_COPY_ERR( NetworkAnalyzer_SendFile( handle , pFileNamePath , szRegister_FileName ));    
				
							FREE(pFileNamePath);
				
							CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
							bGetLock = 1;
						}	
			}
		}
	}
	
Error:
	
	if ( handle && bGetLock )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_SaveToRegister(int hInstrumentHandle, char *pszFile , int iRegister )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char			szReadFeedback[STD_STRING]			=	{0},
					szDirectoryPath[512]				=	{0};
	
	char			*pTemp								=	NULL;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	IF (( pszFile == NULL ) , "State is empty" ); 
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
					{
							strcpy( szDirectoryPath , pszFile );
			
							pTemp = strrchr( szDirectoryPath , '\\' );
			
							if ( pTemp )
							{
								*pTemp = 0;
							}
			
							if ( strstr( szDirectoryPath , ":\\" ))
							{
								RecursiveMakeDirectory( pLocalHandle->sessionHandle , "\"0.0\"\n" , szDirectoryPath );
							}
			
							viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );
							
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":MMEM:STOR:CSAR \"%s\"\n" , pszFile ));

							DelayWithEventProcessing(pLocalHandle->lfStateFileDelay);
							
							WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfStateFileTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
							FREE_CALLOC_COPY_STRING(pLocalHandle->pLastStateFile,pszFile); 
					}
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
					{
							strcpy( szDirectoryPath , pszFile );
			
							pTemp = strrchr( szDirectoryPath , '\\' );
			
							if ( pTemp )
							{
								*pTemp = 0;
							}
			
							if ( strstr( szDirectoryPath , ":\\" ))
							{
								RecursiveMakeDirectory( pLocalHandle->sessionHandle , "\"0.0\"\n" , szDirectoryPath );
							}
			
							viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );
							
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":MMEM:STOR \"%s\"\n" , pszFile ));

							DelayWithEventProcessing(pLocalHandle->lfStateFileDelay);
							
							WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfStateFileTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
							FREE_CALLOC_COPY_STRING(pLocalHandle->pLastStateFile,pszFile); 
					}
		}
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_LoadStateAndCalibration(int hInstrumentHandle, char *pszFile )
{
	return NetworkAnalyzer_ReCallRegister( hInstrumentHandle , pszFile );
}

void* DLLEXPORT   NetworkAnalyzer_SaveStateAndCalibration(int hInstrumentHandle, char *pszFile )
{
	return NetworkAnalyzer_SaveToRegister( hInstrumentHandle, pszFile , 0 );
}

void* DLLEXPORT   NetworkAnalyzer_GetLastStateFileName(int hInstrumentHandle, char **pszFileName )
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
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_SetCalibrationName(int hInstrumentHandle, char *pszName ) 
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	IF (( pszName == NULL ) , "Name is empty" ); 
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if ( strlen(pszName))
	{
		FREE_CALLOC_COPY_STRING( pLocalHandle->pCalibrationName , pszName );
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
} 

void* DLLEXPORT   NetworkAnalyzer_SendLowLevel(int hInstrumentHandle, char szCommand[256] )
{
	STD_ERROR				StdError				=	{0};

//Error: 
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_QuaryLowLevel(int hInstrumentHandle , char szCommand[256] , char *pResponse  , int iBufferlen )
{
	STD_ERROR				StdError				=	{0};

//Error: 
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_QuaryCalibration(int hInstrumentHandle , int iChannel  , char *pResponse  )
{
	STD_ERROR				StdError				=	{0};

//Error: 
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_ECAL_GetInformation(int hInstrumentHandle, char **pszCatalog )
{
	STD_ERROR				StdError							=	{0};
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
	
	char					szReadFeedback[STD_STRING]			=	{0};
	
	unsigned int			uiCount								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	IF (( pszCatalog == NULL ) , "Catalog can't be NULL" ); 
	
	FREE(*pszCatalog);
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
					{
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENS:CORR:CKIT:ECAL1:INF?\n" ));

							SetBreakOnLibraryErrors (0);
							viRead ( pLocalHandle->sessionHandle ,(ViPBuf)szReadFeedback , STD_STRING , &uiCount );    
							SetBreakOnLibraryErrors (1);

							CALLOC_COPY_STRING( *pszCatalog ,szReadFeedback ); 
					}
					break; 
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :
					{
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SYST:COMM:ECAL:DEF?\n" ));

							SetBreakOnLibraryErrors (0);
							viRead ( pLocalHandle->sessionHandle ,(ViPBuf)szReadFeedback , STD_STRING , &uiCount );    
							SetBreakOnLibraryErrors (1);

							CALLOC_COPY_STRING( *pszCatalog ,szReadFeedback ); 
					}
					break; 
		}
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_ECAL_Calibrate(int hInstrumentHandle, char *pszPortList , char *pUserCal , double lfTimeout )
{
	STD_ERROR				StdError							=	{0};
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
	
	char					szReadFeedback[STD_STRING]			=	{0},
							szFormattedCommand[LOW_STRING]		=	{0},
							szSearchPattern[LOW_STRING]			=	{0},
							szEcalModuleName[32]				=	{0},
							szSerialNumber[32]					=	{0},  
							szConnectionType[32]				=	{0},
							szPortList[LOW_STRING]				=	{0};
	
	char					*pTemp								=	NULL,
							*pNext								=	NULL;
	
	double					lfStartTime							=	0.0,
							lfCurrentTime						=	0.0;
			
	int						bSorted								=	0,
							bStatus								=	0;
	
	int						iValue								=	0,  
							iChannelNumber						=	0,
							iNumberOfCalPorts					=	0,
							iECAL_Module_Status					=	0,
							iECAL_Module_Number					=	0,  
							iNoECAL_Counter						=	0;
	
	int						iTimeout							=	0;

	int						count								=	0;
	
	int						viPortNumberList[32]				=	{0};
	
	int						iPortIndex							=	0;
	
	char					*pEcalModel							=	NULL, 
							*pSerialNumber						=	NULL,   
							*pConnectionType					=	NULL;
	
	unsigned int			uiCount								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
	viGetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , &iTimeout );  
	//---------------------- Getting Active Channel ---------------------------------------//
	if ( pLocalHandle->iCurrentChannel < 0 )
	{
		if ( pLocalHandle->bEquipmentIsApna )
		{
			//=====****************************** PNA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SYST:ACT:CHAN?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}
		}
		else
		{
			//=====****************************** ENA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SERV:CHAN:ACT?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}		
		}
	
		pLocalHandle->iCurrentChannel = iChannelNumber;
	}
	
	//-------------------------------------------------------------------------------------//	
	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :															  
				{		   
							if ( pszPortList )
								strcpy( szPortList , pszPortList );
							else
								strcpy( szPortList , "1,2" );
								
							pNext = szPortList;
							iNumberOfCalPorts = 1;
						
							do
							{
								pTemp = strchr(pNext,',');	
							
								if ( pTemp )
								{
									viPortNumberList[iNumberOfCalPorts-1] = atoi(pNext);
									
									iNumberOfCalPorts++;
								
									pNext = pTemp + 1;
								}
							
							} while(pTemp); 
							
							viPortNumberList[iNumberOfCalPorts-1] = atoi(pNext);   
							
							GetCurrentDateTime(&lfStartTime);   
					
							do
							{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENS:CORR:CKIT:ECAL:CLIST?\n" ));    
					
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iECAL_Module_Status = atoi(szReadFeedback);
								
								if ( iECAL_Module_Status < 0 )
								{
									MessagePopup( "No ECAL module found" , "Please connect ECAL module." );
									iNoECAL_Counter++;	
								}
								else
								{
									break;
								}
								
								IF (( iNoECAL_Counter > 3 ) , "No ECAL module present in the system." );
								GetCurrentDateTime(&lfCurrentTime);  
								
							} while((lfCurrentTime-lfStartTime) < lfTimeout );
 
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENS:CORR:CKIT:ECAL:LIST?\n" ));    
					
							SetBreakOnLibraryErrors (0); 
							viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
							SetBreakOnLibraryErrors (1); 
					
							iECAL_Module_Number = atoi(szReadFeedback);
							
							if ( pUserCal && strstr(pUserCal, "char" ))
							{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENS:CORR:CKIT:ECAL%d:INF? %s\n" , iECAL_Module_Number , pUserCal ));
							}
							else
							{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENS:CORR:CKIT:ECAL%d:INF?\n" , iECAL_Module_Number ));
							}
							
							SetBreakOnLibraryErrors (0);
							viRead ( pLocalHandle->sessionHandle ,(ViPBuf)szReadFeedback , STD_STRING , &uiCount );    
							SetBreakOnLibraryErrors (1);

							pTemp = strstr( szReadFeedback , "ModelNumber: " );
							
							IF (( pTemp == NULL ) , "Wrong responds" );
							
							pEcalModel = pTemp + strlen("ModelNumber: ");
							
							memcpy( szEcalModuleName , pEcalModel , (sizeof(szEcalModuleName)-1));
									
							pTemp = strchr( szEcalModuleName , ',' );
							
							if ( pTemp )
								*pTemp = 0;
							
							pTemp = strstr( szReadFeedback , "SerialNumber: " );
							
							IF (( pTemp == NULL ) , "Wrong responds" );
							
							pSerialNumber = pTemp + strlen("SerialNumber: ");
							
							memcpy( szSerialNumber , pSerialNumber , (sizeof(szSerialNumber)-1));
									
							pTemp = strchr( szSerialNumber , ',' );
							
							if ( pTemp )
								*pTemp = 0;	   
							
							//CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENS:CORR:CSET:DEL \"%s\";*CLS\n" , pLocalHandle->pCalibrationName ));
							
							//CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENSe:CORRection:CSET:CREate \"%s\"\n" , pLocalHandle->pCalibrationName ));
							
							do
							{
								bSorted = 0;
								
								for( iPortIndex = 1; iPortIndex < iNumberOfCalPorts; iPortIndex++ )
									if ( viPortNumberList[iPortIndex] < viPortNumberList[iPortIndex-1] )
									{
										iValue = viPortNumberList[iPortIndex-1];
										viPortNumberList[iPortIndex-1] = viPortNumberList[iPortIndex];
										viPortNumberList[iPortIndex] = iValue;
										
										bSorted = 1;
									}
								
							} while( bSorted );
							
							for( iPortIndex = 0; iPortIndex < iNumberOfCalPorts; iPortIndex++ )
							{
								sprintf( szSearchPattern , "Port%cConnector: " , ( 'A' + viPortNumberList[iPortIndex] - 1 ));
								pTemp = strstr( szReadFeedback , szSearchPattern );  
									
								pConnectionType = pTemp + strlen(szSearchPattern); 

								memcpy( szConnectionType , pConnectionType , (sizeof(szEcalModuleName)-1));
								
								pTemp = strchr( szConnectionType , ',' );  
								
								if ( pTemp )
									*pTemp = 0;
								
								RemoveSurroundingWhiteSpace (szConnectionType);
								
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENSe:CORRection:COLLect:GUIDed:CONNector:PORT%d:SELect \"%s\"\n" , viPortNumberList[iPortIndex] , szConnectionType ));
								
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENSe:CORRection:COLLect:GUIDed:CKIT:PORT%d:SELect \"%s ECal %s\"\n" , viPortNumberList[iPortIndex] , szEcalModuleName , szSerialNumber ));
							}
					
							viSetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , (int)(lfTimeout*1E3) );  
							
							//CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENSe:CORRection:COLLect:GUIDed:INITiate:IMMediate \"%s\",1\n" , pLocalHandle->pCalibrationName ));
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENSe:CORRection:COLLect:GUIDed:INITiate:IMMediate\n" ));
							
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENSe:CORRection:COLLect:GUIDed:ACQuire STAN1\n"));
							
							WaitForOperationCompletion( pLocalHandle->sessionHandle , lfTimeout , lfTimeout );   
							
							//CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENSe:CORRection:COLLect:GUIDed:SAVE:CSET \"%s\"\n" , pLocalHandle->pCalibrationName ));
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SENSe:CORRection:COLLect:GUIDed:SAVE\n" )); 
							
							DelayWithEventProcessing( pLocalHandle->lfDelay );
							
							WaitForOperationCompletion( pLocalHandle->sessionHandle , lfTimeout , lfTimeout );  

					}
					break; 
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :															  
				{
							if ( pszPortList == NULL )
							{
								sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:ECAL:SOLT2 1,2\n" , pLocalHandle->iCurrentChannel);
							}
							else
							{
								strcpy( szPortList , pszPortList );
							
								pNext = szPortList;
								iNumberOfCalPorts = 1;
							
								do
								{
									pTemp = strchr(pNext,',');	
								
									if ( pTemp )
									{
										iNumberOfCalPorts++;
									
										pNext = pTemp + 1;
									}
								
								} while(pTemp);
							
								sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:ECAL:SOLT%d %s\n" , pLocalHandle->iCurrentChannel , iNumberOfCalPorts , pszPortList );
							}
						
							viSetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , (int)(lfTimeout*1E3) );  
						
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szFormattedCommand ));
							DelayWithEventProcessing( pLocalHandle->lfEcalDelay );
							
							WaitForOperationCompletion( pLocalHandle->sessionHandle , lfTimeout , lfTimeout );  

					}
					break; 
		}
	}
	
Error:
	
	viSetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , iTimeout );  
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_Reponse_OpenShortLoad_Calibrate(int hInstrumentHandle , int iType_OSL , char *pszPortList , double lfTimeout )
{
	STD_ERROR				StdError							=	{0};
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
	
	char					szReadFeedback[STD_STRING]			=	{0},
							szFormattedCommand[LOW_STRING]		=	{0},
							szPortList[LOW_STRING]				=	{0};
	
	char					*pTemp								=	NULL,
							*pNext								=	NULL;
	
	double					lfStartTime							=	0.0,
							lfCurrentTime						=	0.0;
			
	int						bStatus								=	0;
	
	int						iChannelNumber						=	0,
							iCurrentPort						=	0;
	
	int						iTimeout							=	0;

	int						count								=	0;
	
	int						index								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
	viGetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , &iTimeout );  
	//---------------------- Getting Active Channel ---------------------------------------//
	if ( pLocalHandle->iCurrentChannel < 0 )
	{
		if ( pLocalHandle->bEquipmentIsApna )
		{
			//=====****************************** PNA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SYST:ACT:CHAN?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}
		}
		else
		{
			//=====****************************** ENA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SERV:CHAN:ACT?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}		
		}
	
		pLocalHandle->iCurrentChannel = iChannelNumber;
	}
	
	//-------------------------------------------------------------------------------------//	
	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :															  
				{		
							{
								for ( index = 0; index < 2; index++ )
								{
									if ( pszPortList )
										strcpy( szPortList , pszPortList );
							
									pNext = szPortList;
									iCurrentPort = 1;
								
									do
									{
										pTemp = strchr(pNext,',');	
								
										if ( pTemp )
										{
											*pTemp = 0;
										
											iCurrentPort = atoi(pNext);
									
											pNext = pTemp + 1;
										}
								
									} while(pTemp);
							
									switch ( iType_OSL )
									{
										case 0:
											
											if	( index )
												sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:OPEN %d\n" , pLocalHandle->iCurrentChannel , iCurrentPort );  
											else
												sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:METH:OPEN %d\n" , pLocalHandle->iCurrentChannel , iCurrentPort );  
												
											break;
										case 1:
											
											if	( index )
												sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:SHOR %d\n" , pLocalHandle->iCurrentChannel , iCurrentPort ); 
											else
												sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:METH:SHOR %d\n" , pLocalHandle->iCurrentChannel , iCurrentPort ); 
											
											break;
										case 2:	
											
											if	( index )
												sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:LOAD %d\n" , pLocalHandle->iCurrentChannel , iCurrentPort ); 
											else
												sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:METH:LOAD %d\n" , pLocalHandle->iCurrentChannel , iCurrentPort ); 
											
											break;
										
										default:	
											sprintf( szFormattedCommand , ":SENS%d:CORR:CLE\n" , pLocalHandle->iCurrentChannel);
											break;	
									}
		
									viSetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , (int)(lfTimeout*1E3) );  
						
									CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szFormattedCommand ));
									DelayWithEventProcessing( pLocalHandle->lfDelay );

									CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "*WAI;\n" )); 
									DelayWithEventProcessing( pLocalHandle->lfDelay );
							
									WaitForOperationCompletion( pLocalHandle->sessionHandle , lfTimeout , lfTimeout ); 
								}
							}	 
					}
					break; 
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :															  
					{
								for ( index = 0; index < 2; index++ )
								{
									if ( pszPortList )
										strcpy( szPortList , pszPortList );
							
									pNext = szPortList;
									iCurrentPort = 1;
								
									do
									{
										pTemp = strchr(pNext,',');	
								
										if ( pTemp )
										{
											*pTemp = 0;
										
											iCurrentPort = atoi(pNext);
									
											pNext = pTemp + 1;
										}
								
									} while(pTemp);
							
									switch ( iType_OSL )
									{
										case 0:
											
											if	( index )
												sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:OPEN %d\n" , pLocalHandle->iCurrentChannel , iCurrentPort );  
											else
												sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:METH:OPEN %d\n" , pLocalHandle->iCurrentChannel , iCurrentPort );  
												
											break;
										case 1:
											
											if	( index )
												sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:SHOR %d\n" , pLocalHandle->iCurrentChannel , iCurrentPort ); 
											else
												sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:METH:SHOR %d\n" , pLocalHandle->iCurrentChannel , iCurrentPort ); 
											
											break;
										case 2:	
											
											if	( index )
												sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:LOAD %d\n" , pLocalHandle->iCurrentChannel , iCurrentPort ); 
											else
												sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:METH:LOAD %d\n" , pLocalHandle->iCurrentChannel , iCurrentPort ); 
											
											break;
										
										default:	
											sprintf( szFormattedCommand , ":SENS%d:CORR:CLE\n" , pLocalHandle->iCurrentChannel);
											break;	
									}
		
									viSetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , (int)(lfTimeout*1E3) );  
						
									CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szFormattedCommand ));
									DelayWithEventProcessing( pLocalHandle->lfDelay );

									CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "*WAI;\n" )); 
									DelayWithEventProcessing( pLocalHandle->lfDelay );
							
									WaitForOperationCompletion( pLocalHandle->sessionHandle , lfTimeout , lfTimeout ); 
								}
							}
					break; 
		}
	}
	
Error:
	
	viSetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , iTimeout );  
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_Reponse_Thru_Calibrate(int hInstrumentHandle, char *pszPortList , double lfTimeout )
{
	STD_ERROR				StdError							=	{0};
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
	
	char					szReadFeedback[STD_STRING]			=	{0},
							szFormattedCommand[LOW_STRING]		=	{0},
							szPortList[LOW_STRING]				=	{0};
	
	char					*pTemp								=	NULL,
							*pNext								=	NULL;
	
	double					lfStartTime							=	0.0,
							lfCurrentTime						=	0.0;
			
	int						bStatus								=	0;
	
	int						iChannelNumber						=	0,
							iNumberOfCalPorts					=	0;
	
	int						iTimeout							=	0;

	int						count								=	0;

	int						index								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
	viGetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , &iTimeout );  
	//---------------------- Getting Active Channel ---------------------------------------//
	if ( pLocalHandle->iCurrentChannel < 0 )
	{
		if ( pLocalHandle->bEquipmentIsApna )
		{
			//=====****************************** PNA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SYST:ACT:CHAN?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}
		}
		else
		{
			//=====****************************** ENA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SERV:CHAN:ACT?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}		
		}
	
		pLocalHandle->iCurrentChannel = iChannelNumber;
	}
	
	//-------------------------------------------------------------------------------------//	
	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :															  
				{
							for ( index = 0; index < 2; index++ )
							{
								if ( index )
								{
									if ( pszPortList == NULL )
										sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:THRU 1,2\n" , pLocalHandle->iCurrentChannel);
									else
										sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:THRU %s\n" , pLocalHandle->iCurrentChannel , pszPortList );
								}
								else
								{
									if ( pszPortList == NULL )
										sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:METH:THRU 1,2\n" , pLocalHandle->iCurrentChannel);
									else
										sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:METH:THRU %s\n" , pLocalHandle->iCurrentChannel , pszPortList );
								}
								
								viSetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , (int)(lfTimeout*1E3) );  
						
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szFormattedCommand ));
								DelayWithEventProcessing( pLocalHandle->lfDelay );

								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "*WAI;\n" )); 
								DelayWithEventProcessing( pLocalHandle->lfDelay );
							
								WaitForOperationCompletion( pLocalHandle->sessionHandle , lfTimeout , lfTimeout ); 
							}

					}
					break; 
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :															  
				{
							for ( index = 0; index < 2; index++ )
							{
								if ( index )
								{
									if ( pszPortList == NULL )
										sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:THRU 1,2\n" , pLocalHandle->iCurrentChannel);
									else
										sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:THRU %s\n" , pLocalHandle->iCurrentChannel , pszPortList );
								}
								else
								{
									if ( pszPortList == NULL )
										sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:METH:THRU 1,2\n" , pLocalHandle->iCurrentChannel);
									else
										sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:METH:THRU %s\n" , pLocalHandle->iCurrentChannel , pszPortList );
								}
								
								viSetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , (int)(lfTimeout*1E3) );  
						
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szFormattedCommand ));
								DelayWithEventProcessing( pLocalHandle->lfDelay );

								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "*WAI;\n" )); 
								DelayWithEventProcessing( pLocalHandle->lfDelay );
							
								WaitForOperationCompletion( pLocalHandle->sessionHandle , lfTimeout , lfTimeout ); 
							}

					}
					break; 
		}
	}
	
Error:
	
	viSetAttribute ( pLocalHandle->sessionHandle , VI_ATTR_TMO_VALUE , iTimeout );  
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

void* DLLEXPORT   NetworkAnalyzer_Reponse_Done_Calibrate(int hInstrumentHandle )
{
	STD_ERROR				StdError							=	{0};
	
	tsHandle				*pLocalHandle						=	{0};

	int						handle								=	0;
	
	char					szReadFeedback[STD_STRING]			=	{0},
							szFormattedCommand[LOW_STRING]		=	{0};
	
	char					*pTemp								=	NULL,
							*pNext								=	NULL;
	
	double					lfStartTime							=	0.0,
							lfCurrentTime						=	0.0;
			
	int						bStatus								=	0;
	
	int						iChannelNumber						=	0,
							iNumberOfCalPorts					=	0;
	
	int						iTimeout							=	0;

	int						count								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  
	
	//---------------------- Getting Active Channel ---------------------------------------//
	if ( pLocalHandle->iCurrentChannel < 0 )
	{
		if ( pLocalHandle->bEquipmentIsApna )
		{
			//=====****************************** PNA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "SYST:ACT:CHAN?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}
		}
		else
		{
			//=====****************************** ENA ******************************************=====//
			switch ( pLocalHandle->tType )
			{
				case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:
				default :
						{
								CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , ":SERV:CHAN:ACT?\n" ));
	
								SetBreakOnLibraryErrors (0); 
								viRead( pLocalHandle->sessionHandle, szReadFeedback , LOW_STRING , &count );
								SetBreakOnLibraryErrors (1); 
						
								iChannelNumber = atoi(szReadFeedback);
						}
			}		
		}
	
		pLocalHandle->iCurrentChannel = iChannelNumber;
	}
	
	//-------------------------------------------------------------------------------------//	
	if ( pLocalHandle->bEquipmentIsApna )
	{
		//=====****************************** PNA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :															  
				{
							sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:SAVE\n" , pLocalHandle->iCurrentChannel );
							
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szFormattedCommand ));
							DelayWithEventProcessing( pLocalHandle->lfDelay );

							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "*WAI;\n" )); 
							DelayWithEventProcessing( pLocalHandle->lfDelay );
							
							WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  

					}
					break; 
		}
	}
	else
	{
		//=====****************************** ENA ******************************************=====//
		switch ( pLocalHandle->tType )
		{
			case FW_TYPE_A_11_22:case FW_TYPE_A_11_02:case FW_TYPE_A_09_61:case FW_TYPE_A_09_42:case FW_TYPE_A_09_20:case FW_TYPE_A_09_10:case FW_TYPE_A_08_50:case FW_TYPE_A_07_50:case FW_TYPE_A_06_04:case FW_TYPE_A_06_01:case FW_TYPE_A_04_87:

			default :															  
				{
							sprintf( szFormattedCommand , ":SENS%d:CORR:COLL:SAVE\n" , pLocalHandle->iCurrentChannel );

							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , szFormattedCommand ));
							DelayWithEventProcessing( pLocalHandle->lfDelay );
							
							CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "*WAI;\n" )); 
							DelayWithEventProcessing( pLocalHandle->lfDelay );
							
							WaitForOperationCompletion( pLocalHandle->sessionHandle , pLocalHandle->lfTimeout  ,  pLocalHandle->lfOpcLowLevelTimeout );  

					}
					break; 
		}
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return NetworkAnalyzer_GetErrorTextMessage(hInstrumentHandle,StdError.error,NULL);
}

int		WaitForOperationCompletion( int sessionHandle , double lfTimeout , double lfLowLevelTimeout )
{
	double			lfStartTime						=	0.0,
					lfCurrentTime					=	0.0;

	int				iOpcStatus						=	0;
	
	char			szReadFeedback[STD_STRING]		=	{0};
	
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


int		RecursiveMakeDirectory( int sessionHandle , char *pszEmptyDirectory , char *pPath )
{
	char		szCurrentPath[STD_STRING]		=	{0},
				szReadFeedback[STD_STRING]		=	{0},
				*pTemp							=	NULL;
														
	int			count							=	0;
	
	int			status							=	0;
	
	if ( pPath == NULL )
		return -1;
	
	strcpy( szCurrentPath , pPath );
	
	viPrintf( sessionHandle , "MMEM:CAT? \"%s\"\n" , szCurrentPath );
	
	SetBreakOnLibraryErrors (0);
	viRead( sessionHandle, szReadFeedback , (STD_STRING-1) , &count );
	SetBreakOnLibraryErrors (1);
	
	if (( strcmp( szReadFeedback , pszEmptyDirectory ) == 0 ) || ( strstr( szReadFeedback , "NO CATALOG" )))
	{
		pTemp = strrchr( szCurrentPath , '\\' );
		
		if ( pTemp )
		{
			pTemp++;
			*pTemp = 0;
			
			RecursiveMakeDirectory( sessionHandle , pszEmptyDirectory , szCurrentPath );
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
				
				status = strcmp( szReadFeedback , pszEmptyDirectory );
			}
			
			return status;
		}
	}

	return 0;
}

int main (int argc, char *argv[])
{
	int				hInstrumentHandle				=	0,
					iInstrumentConnectStatus		=	0,
					iInstrumentChanelsQuantity		=	0;
	
	double			*plfFirstVector					=	NULL,
					*plfSecondVector				=	NULL;
						
	double			lfStartFrequency				=	0.0,
					lfStopFrequency					=	0.0;
		
	int				iNumberOfPoints					=	0;
	
	if (InitCVIRTE (0, argv, 0) == 0)
        return -1;  /* out of memory */
	
	//TCPIP::10.0.0.19::5025::SOCKET
	//visa://192.168.145.111/GPIB0::17::INSTR
	NetworkAnalyzer_Init( 0 , "visa://BADAS-TEST7/GPIB::16" , &hInstrumentHandle, &iInstrumentConnectStatus, &iInstrumentChanelsQuantity);
	
	NetworkAnalyzer_GetTrace ( hInstrumentHandle , 1 , 0 , 0 , 60, &plfSecondVector , &plfFirstVector , &iNumberOfPoints );  
	
	NetworkAnalyzer_Reset( hInstrumentHandle);   
	NetworkAnalyzer_Calibration_SetFrequencyPlan ( hInstrumentHandle , 1E9 , 2E9 , 1.5 , -3 , 1601 );
	NetworkAnalyzer_SetFrequency( hInstrumentHandle , 1E9 , 2E9 );
	NetworkAnalyzer_GetFrequency( hInstrumentHandle , &lfStartFrequency, &lfStopFrequency );
	NetworkAnalyzer_SetPointsNum( hInstrumentHandle , 1601);
	NetworkAnalyzer_GetPointsNum( hInstrumentHandle, &iNumberOfPoints ); 
	NetworkAnalyzer_SelectChannel ( hInstrumentHandle , 2 ); 
	NetworkAnalyzer_SelectWindow ( hInstrumentHandle , 1 );  
	NetworkAnalyzer_SelectTrace ( hInstrumentHandle , 3 );
	NetworkAnalyzer_SelectMathFunction ( hInstrumentHandle , 1 , '+' );  
	NetworkAnalyzer_SetTraceToMathMemory( hInstrumentHandle , 1 );
	
	NetworkAnalyzer_ECAL_Calibrate( hInstrumentHandle, "1,2,3,4" , NULL ,30.0 );     
	
	NetworkAnalyzer_ReCallRegister( hInstrumentHandle, "1.csa" ); 
	
	NetworkAnalyzer_SaveToRegister( hInstrumentHandle, "d:\\calibration\\2483-02\\spart" , 0 );
	
	NetworkAnalyzer_GetTrace ( hInstrumentHandle ,  2 , 0 , 0 , 60, &plfSecondVector , &plfFirstVector , &iNumberOfPoints );  
	NetworkAnalyzer_GetTrace ( hInstrumentHandle ,  2 , 0 , 0 , 60, &plfSecondVector , &plfFirstVector , &iNumberOfPoints ); 
	NetworkAnalyzer_GetTrace ( hInstrumentHandle ,  3 , 0 , 0 , 60, &plfSecondVector , &plfFirstVector , &iNumberOfPoints );
	NetworkAnalyzer_GetTrace ( hInstrumentHandle ,  4 , 0 , 0 , 60, &plfSecondVector , &plfFirstVector , &iNumberOfPoints ); 
	
	//NetworkAnalyzer_Reset( hInstrumentHandle);
	
	//NetworkAnalyzer_SetFrequency( hInstrumentHandle, 1E9 , 2E9 );
																																												 
	NetworkAnalyzer_SaveToRegister( hInstrumentHandle, "d:\\calibration\\2483-02\\spart" , 0 );  
		
	NetworkAnalyzer_GetPointsNum( hInstrumentHandle , &iNumberOfPoints ); 
	
	//NetworkAnalyzer_ReCallRegister( hInstrumentHandle , "d:\\calibration\\2483-02\\LOW" );

	NetworkAnalyzer_SelectChannel( hInstrumentHandle , 1 );
	NetworkAnalyzer_SelectChannel( hInstrumentHandle , 2 ); 
	NetworkAnalyzer_SelectChannel( hInstrumentHandle , 3 ); 
	NetworkAnalyzer_SelectChannel( hInstrumentHandle , 4 ); 
		
	NetworkAnalyzer_AutoScale( hInstrumentHandle); 
		
	NetworkAnalyzer_GetPointsNum( hInstrumentHandle , &iNumberOfPoints );
	NetworkAnalyzer_GetFrequency( hInstrumentHandle , &lfStartFrequency, &lfStopFrequency );
	
	//NetworkAnalyzer_GetMeasureVectors( hInstrumentHandle, 0 , &plfFirstVector , &plfSecondVector, &plfThirdVector, &plfFourthVector );
	
	NetworkAnalyzer_GetTrace ( hInstrumentHandle ,  1 , 0 , 0 , 60, NULL , &plfFirstVector , &iNumberOfPoints );
	
	NetworkAnalyzer_GetTrace ( hInstrumentHandle ,  2 , 0 , 0 , 60, NULL , &plfFirstVector , &iNumberOfPoints );
	
	NetworkAnalyzer_GetTrace ( hInstrumentHandle ,  3 , 0 , 0 , 60, NULL , &plfFirstVector , &iNumberOfPoints );
	
	//NetworkAnalyzer_GetFrequency( hInstrumentHandle,double *plfStartFrequency, double *plfStopFrequency);
	//NetworkAnalyzer_SetPointsNum( hInstrumentHandle,int points);
	//NetworkAnalyzer_GetPointsNum( hInstrumentHandle,int *points);
	//NetworkAnalyzer_GetMeasureVectors( hInstrumentHandle,int iGetDataType, double **plfFirstVector,double **plfSecondVector, double **plfThirdVector,double **plfFourthVector);
	//NetworkAnalyzer_SetMeasureType( hInstrumentHandle,int iChannel, int iInSparam, int iConversation);
	//NetworkAnalyzer_DisplaySelect( hInstrumentHandle,int iChannel, int iDataMode, int iDataFormat);
	//NetworkAnalyzer_AutoScale( hInstrumentHandle);
	//NetworkAnalyzer_AvarageSmooth( hInstrumentHandle, iChannel, char *szSmoothRequest, double lfSetSmooth,double lfCurrentSmooth);
	//NetworkAnalyzer_Smooth( hInstrumentHandle, iChannel, int iState, double lfSetSmooth,int iNumberPoints);
	//NetworkAnalyzer_SetBandWidth( hInstrumentHandle, iChannel, double lfBW_Value, char *szBW_Request,double *lfGetBW_Value);
	//NetworkAnalyzer_SetFrequencySweepType( hInstrumentHandle,int iChannel, int iFrequencySweepType);
	//NetworkAnalyzer_SetPower( hInstrumentHandle, iChannel, double lfPower, char *szPowerRequest,int iPortNum);
	//NetworkAnalyzer_GetPower( hInstrumentHandle, iChannel, double *lfPower, int iPortNum);
	//NetworkAnalyzer_SetPowerRange( hInstrumentHandle,int iChannel, int iRange, char *szRangeRequest,int *iCurrentRange);
	//NetworkAnalyzer_SetPowerSweep( hInstrumentHandle,int iChannel, double lfStartPower, double lfStopPower,char *szSweepRequest, double *lfGetStartPower,double *lfGetStopPower);
	//NetworkAnalyzer_SetSweepTime( hInstrumentHandle,int iChannel, double lfSweepTime, char *szSweepTimeRequest,double *lfGetSweepTime);
	//NetworkAnalyzer_SetMarker( hInstrumentHandle,int iChannel, double lfSetValue, int iMarker,char *szMarkerRequest);
	//NetworkAnalyzer_GetMarker( hInstrumentHandle,int iChannel, int iMarker, char *szMarkerRequest,double *lfGetMarkerOneValue, double *lfGetMarkerTwoValue,double *lfGetMarkerThreeValue, double *lfGetMarkerFourValue);
	//NetworkAnalyzer_ReCallRegister( hInstrumentHandle,char *szRegister_FileName);
	//NetworkAnalyzer_SaveToRegister( hInstrumentHandle,char *szTitle, int iRegister);
	//NetworkAnalyzer_SendLowLevel( hInstrumentHandle,char *szCommand);
	//NetworkAnalyzer_QuaryLowLevel( hInstrumentHandle,char *szCommand, char *pResponse, int iBufferlen);
	//NetworkAnalyzer_QuaryCalibration( hInstrumentHandle,int iChannel, char *pResponse);
	//
	//NetworkAnalyzer_GetErrorTextMessage( hInstrumentHandle,int iError, char pErrorMessage); 
	
	NetworkAnalyzer_Close( &hInstrumentHandle );


	return 0;
}
