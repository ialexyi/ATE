#include <Ivi.h>
#include <userint.h>
#include "agesg.h"
#include <ansi_c.h>
//==============================================================================
// Include files

//#include "agesg_wrapper.h"
#include "GlobalDefines.h"
#include <formatio.h>
#include "toolbox.h"
#include <utility.h>

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

/***** ***** ***** ***** ***** ***** Get Last Error Text Message ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT SignalGenerator_GetLastErrorTextMessage ( int hInstrumentHandle , char **pErrorMessage )
{
	STD_ERROR				StdError					=	{0};    

	//agesg_GetError( pErrorMessage);
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SignalGenerator_GetErrorTextMessage ( int hInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR				StdError					=	{0};    

	iError = agesg_error_message	( hInstrumentHandle , iError , pErrorMessage );
	
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
									szIdentificationUpperText[LOW_STRING]		=	{0}; 
									
	char							vszSupportdedModels[][12]					=	{{"E4400B"},{"E4420B"},{"E4421B"},{"E4422B"},{"E4423B"},{"E4424B"},{"E4425B"},{"E4426B"},{"E4430B"},{"E4431B"},{"E4432B"},{"E4433B"},{"E4434B"},{"E4435B"},{"E4436B"},{"E4437B"},{"ESG-D1000B"},{"ESG-D2000B"},{"ESG-D3000B"},{"ESG-D4000B"},{"E4438C"},{"E4428C"}};
									
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
		
		if (( strstr( szIdentificationUpperText , "AGILENT TECHNOLOGIES" ) == NULL ) && ( strstr( szIdentificationUpperText ,"HEWLETT-PACKARD") == NULL ))
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

void*		DLLEXPORT	SignalGenerator_Init( int hParentInstrumentHandle , char* szRsrcAddress , int *phInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChannelsQuantity )
{	
	STD_ERROR           StdError                    =   {0};
						
	int					iChannelCount				=	1;
	
	char				szErrorTextMessage[256]		=	{0};
	
	ViSession			handle						=	0;
	
	int					hVisaOpenLockHandle			=	0;
	
	int					bGetVisaLock				=	0;
	
	if ( hParentInstrumentHandle == 0 )
	{
		CHK_CMT( CmtNewLock ( VISA_OPEN_LOCK , OPT_TL_PROCESS_EVENTS_WHILE_WAITING , &hVisaOpenLockHandle ));   
		
		if ( hVisaOpenLockHandle )
		{
			CHK_CMT( CmtGetLock ( hVisaOpenLockHandle ));
			bGetVisaLock = 1;
		}
		
		STD_ERR ( agesg_init ( szRsrcAddress, VI_TRUE, VI_FALSE, &handle));
	}
	else
		handle = hParentInstrumentHandle;
		
	if ( phInstrumentHandle  ) 
		*phInstrumentHandle = handle;
	
	STD_ERR ( agesg_ClearError (handle));
	
	if ( InstrumentChannelsQuantity )
		*InstrumentChannelsQuantity = iChannelCount;

Error:
	
	if ( hVisaOpenLockHandle && bGetVisaLock ) 
	{
		CmtReleaseLock (hVisaOpenLockHandle);
		CmtDiscardLock (hVisaOpenLockHandle); 
	}
	
	if (( IS_NOT_OK ) || (iChannelCount < 1))
	{ 
		FREE_CALLOC( StdError.pszErrorDescription , STD_STRING , sizeof(char*));
				
		if ( StdError.pszErrorDescription )
			agesg_error_message (handle, StdError.error , StdError.pszErrorDescription );
		
		MessagePopup("Power Supply Initializing Error!",szErrorTextMessage);
		
		agesg_close (handle);
	}
	else
		if ( InstrumentConnectStatus )
			*InstrumentConnectStatus = 1;	
	
	RETURN_STDERR_POINTER;
}


void*	DLLEXPORT	SignalGenerator_Close( int *pviInstrumentHandle )
{
	STD_ERROR				StdError					=	{0};    

	int 					iStatus						=	0;  
	
	if (pviInstrumentHandle)
	{
		iStatus = agesg_close( *pviInstrumentHandle );
		*pviInstrumentHandle = 0;
	}
		
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT SignalGenerator_Reset ( int hInstrumentHandle )   
{   
	STD_ERROR           StdError                    =   {0};

	STD_ERR( agesg_reset ( hInstrumentHandle ));
	
	STD_ERR( agesg_ConfigureOutputModulationEnabled ( hInstrumentHandle , VI_FALSE ));

Error:
	
	RETURN_STDERR_POINTER; 
}


void* DLLEXPORT 	SignalGenerator_RF_Off ( int InstrumentHandle , int channel )
{
	STD_ERROR           StdError                    =   {0};

	STD_ERR( agesg_ConfigureOutputEnabled ( InstrumentHandle , VI_FALSE ));
	
	STD_ERR( agesg_WaitUntilSettled ( InstrumentHandle , 10000 ));

Error:
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_RF_On ( int InstrumentHandle , int channel )
{
	STD_ERROR           StdError                    =   {0};

	STD_ERR( agesg_ConfigureOutputEnabled ( InstrumentHandle , VI_TRUE));
	
	STD_ERR( agesg_WaitUntilSettled ( InstrumentHandle , 10000 ));

Error:
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_Modulation_Off ( int InstrumentHandle , int channel )
{
	STD_ERROR           StdError                    =   {0};

	STD_ERR( agesg_ConfigureOutputModulationEnabled ( InstrumentHandle , VI_FALSE ));
	
	STD_ERR( agesg_WaitUntilSettled ( InstrumentHandle , 10000 ));

Error:
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_Modulation_On( int InstrumentHandle , int channel )
{
	STD_ERROR           StdError                    =   {0};

	STD_ERR( agesg_ConfigureOutputModulationEnabled ( InstrumentHandle , VI_TRUE ));
	
	STD_ERR( agesg_WaitUntilSettled ( InstrumentHandle , 10000 ));

Error:
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_Config ( int InstrumentHandle , int channel , double lfFrequency , double lfAmplitude )
{
	STD_ERROR           StdError                    =   {0};

	STD_ERR( agesg_ConfigureRF ( InstrumentHandle , lfFrequency , lfAmplitude ));
	
	STD_ERR( agesg_WaitUntilSettled ( InstrumentHandle , 10000 ));

Error:
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_SetFrequency ( int InstrumentHandle , int channel , double lfFrequency )
{
	STD_ERROR           StdError                    =   {0};

	STD_ERR( agesg_SetAttributeViReal64 ( InstrumentHandle , VI_NULL , AGESG_ATTR_FREQUENCY , lfFrequency ));
	
	STD_ERR( agesg_WaitUntilSettled ( InstrumentHandle , 10000 ));

Error:
	
	RETURN_STDERR_POINTER; 
}


void* DLLEXPORT 	SignalGenerator_SetPower ( int InstrumentHandle , int channel , double lfAmplitude )
{
	STD_ERROR           StdError                    =   {0};

	STD_ERR( agesg_SetAttributeViReal64 ( InstrumentHandle , VI_NULL , AGESG_ATTR_POWER_LEVEL , lfAmplitude ));
	
	STD_ERR( agesg_WaitUntilSettled ( InstrumentHandle , 10000 ));

Error:
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_SetModulation ( int InstrumentHandle , int channel , int source , int iPulseMode , double dWidth , double dPeriod )
{
	STD_ERROR           StdError                    =   {0};

	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_SetModulationState ( int InstrumentHandle , int channel , int iState )
{
	STD_ERROR           StdError                    =   {0};

	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	SignalGenerator_RecallState ( int InstrumentHandle,char *szFileName )
{
	STD_ERROR           StdError                    =   {0};

	char				szCommand[LOW_STRING]		=	{0};
	
	int					iRegisterNumber				=	0;
	
	iRegisterNumber = atoi(szFileName);
	
	sprintf( szCommand , "*RCL %d,0" , iRegisterNumber );
	
	STD_ERR( agesg_WriteInstrData ( InstrumentHandle , szCommand ));
	
	STD_ERR( agesg_WaitUntilSettled ( InstrumentHandle , 10000 ));
	
Error:

	RETURN_STDERR_POINTER; 
}


void* DLLEXPORT   SignalGenerator_GetLastStateFileName(int hInstrumentHandle, char **pszFileName )
{
	STD_ERROR		StdError							=	{0};
	
	RETURN_STDERR_POINTER; 
}
/*
void main ( void)
{
	int							hInstrumentHandle									=	0,
								InstrumentConnectStatus								=	0,
								InstrumentChannelsQuantity							=	0;
		
	int							status												=	0;
	
	SignalGenerator_Init( 0 , "GPIB::19::INSTR" , &hInstrumentHandle , &InstrumentConnectStatus , &InstrumentChannelsQuantity );
	                                                
	SignalGenerator_Reset ( hInstrumentHandle );
	
	SignalGenerator_Close ( &hInstrumentHandle ); 
	
	return;
}

*/
