#include "Windows.h"
#include <userint.h>
#include "tkafg3k.h"
#include <ansi_c.h>
//==============================================================================
// Include files

#include "Tektronix_AFG3XX_wrapper.h"
#include "GlobalDefines.h"
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
/***** ***** ***** ***** ***** ***** Get Last Error Text Message ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT FunctionGenerator_GetLastErrorTextMessage ( int handle , char **pErrorMessage )
{
	STD_ERROR			StdError					=	{0};
	
	CALLOC_ERR( *pErrorMessage , 1024 , sizeof(char));
	
	tkafg3k_GetError( handle , &StdError.error , 1024 , *pErrorMessage);
	
Error:
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT FunctionGenerator_GetErrorTextMessage ( ViSession viInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR			StdError					=	{0};

	ViChar				errorMessage[256]			=	{0};
		
	StdError.error = tkafg3k_error_message	( viInstrumentHandle , iError , errorMessage );
	
	if ( pErrorMessage )
		strcpy( pErrorMessage , errorMessage );
	
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
		IF (( strstr( szIdentificationText , "TEKTRONIX" ) == NULL ) , "Is not supported" );
	
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
	
	char							szIdentificationText[LOW_STRING]			=	{0};
	
	char							vszSupportdedModels[][32]					=	{{"AFG3011"},{"AFG3011C"},{"AFG3021"},{"AFG3022"},{"AFG3021B"},{"AFG3022B"},{"AFG3021C"},{"AFG3022C"},{"AFG3101"},{"AFG3102"},{"AFG3101C"},{"AFG3102C"},{"AFG3251"},{"AFG3252"},{"AFG3251C"},{"AFG3252C"},{"AFG2021"},{"AFG3051C"},{"AFG3052C"}};
									
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
		if ( strstr( szIdentificationText , "TEKTRONIX" ) == NULL ) 
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

void*   DLLEXPORT 	FunctionGenerator_Init( int hParentInstrumentHandle , char* szRsrcAddress , int *phInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChannelsQuantity )
{	
	STD_ERROR			StdError					=	{0};

	int					iChannelCount				=	1;
	
	char				szErrorTextMessage[256]		=	{0};
	
	ViSession			handle						=	0;
	
	if ( hParentInstrumentHandle == 0 )
	{
		STD_ERR ( tkafg3k_init ( szRsrcAddress, VI_TRUE, VI_TRUE, &handle));
	}
	else
		handle = hParentInstrumentHandle;
		
	if ( phInstrumentHandle  ) 
			*phInstrumentHandle = handle;
	
	STD_ERR ( tkafg3k_ClearError (handle));
	
	if ( InstrumentChannelsQuantity )
		*InstrumentChannelsQuantity = iChannelCount;

Error:
	
	if ((StdError.error < 0) || (iChannelCount < 1))
	{ 
		tkafg3k_error_message (handle, StdError.error, szErrorTextMessage);
		
		MessagePopup("Function Generator Initializing Error!",szErrorTextMessage);
		
		tkafg3k_close (handle);
	}
	else
		if ( InstrumentConnectStatus )
			*InstrumentConnectStatus = 1;	
	
	RETURN_STDERR_POINTER;
}


void*   DLLEXPORT 	FunctionGenerator_Close( int *pHandle )
{
	STD_ERROR			StdError					=	{0};
	
	ViSession			handle						=	0;

	if ( pHandle == NULL )
		return NULL;
	
	handle = *pHandle;
	
	if ( handle == 0 )
		return 0;
	
	STD_ERR ( tkafg3k_close ( handle ));
		
Error:
	
	*pHandle = 0;
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT FunctionGenerator_Reset ( ViSession viInstrumentHandle )   
{   
	STD_ERROR			StdError					=	{0};

	char				szErrorTextMessage[256]		=	{0};
	
	STD_ERR( tkafg3k_reset ( viInstrumentHandle ));
	
	STD_ERR( tkafg3k_ResetWithDefaults ( viInstrumentHandle )); 
	
Error:
	
	RETURN_STDERR_POINTER; 
}


void* DLLEXPORT 	FunctionGenerator_Signel_Off ( int InstrumentHandle , int channel )
{
	STD_ERROR			StdError					=	{0};

	char				szChannel[256]				=	{0};
	
	STD_ERR( tkafg3k_GetChannelName ( InstrumentHandle , channel , 512, szChannel ));
	
	STD_ERR( tkafg3k_ConfigureOutputEnabled ( InstrumentHandle , szChannel , VI_FALSE ));
	
Error:
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	FunctionGenerator_Signel_On ( int InstrumentHandle , int channel )
{
	STD_ERROR			StdError					=	{0};

	char				szChannel[256]				=	{0};
	
	STD_ERR( tkafg3k_GetChannelName ( InstrumentHandle , channel , 512, szChannel ));
	
	STD_ERR( tkafg3k_ConfigureOutputEnabled ( InstrumentHandle , szChannel , VI_TRUE ));
	
Error:
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	FunctionGenerator_Save_State ( int InstrumentHandle , int iStateNumber , char *pszFileName )
{
	STD_ERROR			StdError					=	{0};

	char				szFileName[512]				=	{0},
						szSetCommand[512]			=	{0},
						szDirectory[512]			=	{0},
						szSetDirectoryCommand[512]	=	{0},
						szReadFeedback[512]			=	{0},
						*pDirPresent				=	NULL,
						*pSetDirectory				=	NULL,
						*pFileName					=	NULL,
						*pChangeSlash				=	NULL; 
						
	int					iReadCount					=	0;
	
	if ( pszFileName == NULL )
		return NULL;
	
	strcpy( szDirectory , pszFileName );  
	
	pDirPresent = strrchr(szDirectory,'/');
	
	if ( pDirPresent == NULL )
		pDirPresent = strrchr(szDirectory,'\\');  

	if ( pDirPresent )
	{
		*pDirPresent = 0;
		
		strcpy( szFileName , pDirPresent+1 );
		
		pChangeSlash = szDirectory;
		
		while( pChangeSlash = strrchr(pChangeSlash,'\\'))
			*pChangeSlash = '/';
	
		pSetDirectory = StrDupWithoutSurrWhiteSpace (szDirectory);
		pFileName = StrDupWithoutSurrWhiteSpace (szFileName); 
		
		sprintf( szSetDirectoryCommand , "MMEM:CDIR \"%s\"" , pSetDirectory );
	
		STD_ERR( tkafg3k_WriteInstrData ( InstrumentHandle , szSetDirectoryCommand ));

		STD_ERR( tkafg3k_WriteInstrData ( InstrumentHandle , "MMEM:CDIR?" ));  
		
		tkafg3k_ReadInstrData (InstrumentHandle, 512, szReadFeedback, NULL);   
		
		if ( strstr(szReadFeedback,pSetDirectory) == NULL )
		{
			sprintf( szSetDirectoryCommand , "MMEM:CDIR \"/\"" );
	
			STD_ERR( tkafg3k_WriteInstrData ( InstrumentHandle , szSetDirectoryCommand ));
			
			STD_ERR( tkafg3k_WriteInstrData ( InstrumentHandle , "MMEM:CDIR?" ));  
		
			tkafg3k_ReadInstrData (InstrumentHandle, 512, szReadFeedback, NULL);   
			
			sprintf( szSetDirectoryCommand , "MMEM:MDIR \"%s\"" , pSetDirectory );
	
			STD_ERR( tkafg3k_WriteInstrData ( InstrumentHandle , szSetDirectoryCommand ));
			
			sprintf( szSetDirectoryCommand , "MMEM:CDIR \"%s\"" , pSetDirectory );
	
			STD_ERR( tkafg3k_WriteInstrData ( InstrumentHandle , szSetDirectoryCommand ));

			memset( szReadFeedback , 0 , sizeof(szReadFeedback));
			
			STD_ERR( tkafg3k_WriteInstrData ( InstrumentHandle , "MMEM:CDIR?" ));  
		
			tkafg3k_ReadInstrData (InstrumentHandle, 512, szReadFeedback, NULL);   
		}
				
		IF ((strstr(szReadFeedback,pSetDirectory) == NULL ) , "Can't Set directory" );
	}
		
	sprintf( szSetCommand , "MEM:STAT:LOCK %d , OFF" , iStateNumber );
	STD_ERR( tkafg3k_WriteInstrData ( InstrumentHandle , szSetCommand )); 

	DelayWithEventProcessing(0.3);
	
	sprintf( szSetCommand , "*SAV %d" , iStateNumber );
	STD_ERR( tkafg3k_WriteInstrData ( InstrumentHandle , szSetCommand )); 
	
	DelayWithEventProcessing(0.3);
	
	sprintf( szSetCommand , "MMEM:STOR:STAT %d,\"%s\"" , iStateNumber , pFileName );
	STD_ERR( tkafg3k_WriteInstrData ( InstrumentHandle , szSetCommand ));
	
Error:
	
	FREE( pSetDirectory );
	FREE( pFileName );
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	FunctionGenerator_Load_State ( int InstrumentHandle , int iStateNumber , char *pszFileName )
{
	STD_ERROR			StdError					=	{0};

	char				szFileName[512]				=	{0},
						szSetCommand[512]			=	{0},
						szDirectory[512]			=	{0},
						szSetDirectoryCommand[512]	=	{0},
						szReadFeedback[512]			=	{0},
						*pDirPresent				=	NULL,
						*pFileName					=	NULL,  
						*pSetDirectory				=	NULL, 
						*pChangeSlash				=	NULL; 
						
	int					iReadCount					=	0;
	
	if ( pszFileName )
	{
		strcpy( szDirectory , pszFileName );  
	
		pDirPresent = strrchr(szDirectory,'/');
	
		if ( pDirPresent == NULL )
			pDirPresent = strrchr(szDirectory,'\\');  

		if ( pDirPresent )
		{
			*pDirPresent = 0;
		
			strcpy( szFileName , pDirPresent+1 );
		
			pChangeSlash = szDirectory;
		
			while( pChangeSlash = strrchr(pChangeSlash,'\\'))
				*pChangeSlash = '/';
	
			pSetDirectory = StrDupWithoutSurrWhiteSpace (szDirectory);
			pFileName = StrDupWithoutSurrWhiteSpace (szFileName); 
		
			sprintf( szSetDirectoryCommand , "MMEM:CDIR \"%s\"" , pSetDirectory );
	
			STD_ERR( tkafg3k_WriteInstrData ( InstrumentHandle , szSetDirectoryCommand ));

			STD_ERR( tkafg3k_WriteInstrData ( InstrumentHandle , "MMEM:CDIR?" ));  
		
			tkafg3k_ReadInstrData (InstrumentHandle, 512, szReadFeedback, NULL);   
		
			IF ((strstr(szReadFeedback,pSetDirectory) == NULL ) , "Can't Set directory" );
		}

		sprintf( szSetCommand , "MEM:STAT:LOCK %d , OFF" , iStateNumber );
		STD_ERR( tkafg3k_WriteInstrData ( InstrumentHandle , szSetCommand )); 

		DelayWithEventProcessing(0.3);
	
		if ( pFileName )
		{
			sprintf( szSetCommand , "MMEM:LOAD:STAT %d,\"%s\"" , iStateNumber , pFileName );
			STD_ERR( tkafg3k_WriteInstrData ( InstrumentHandle , szSetCommand ));
		}
	
		DelayWithEventProcessing(0.3);
	}
	
	sprintf( szSetCommand , "*RCL %d" , iStateNumber );
	STD_ERR( tkafg3k_WriteInstrData ( InstrumentHandle , szSetCommand )); 
	
	STD_ERR( tkafg3k_InitiateGeneration ( InstrumentHandle ));
	
Error:
	
	FREE( pSetDirectory );
	FREE( pFileName );    
	
	RETURN_STDERR_POINTER; 
}

void* DLLEXPORT 	FunctionGenerator_Initiate ( int InstrumentHandle )
{
	STD_ERROR			StdError					=	{0};

	STD_ERR( tkafg3k_InitiateGeneration ( InstrumentHandle ));
	
Error:
	
	RETURN_STDERR_POINTER; 
}


void main (void)
{
	int					InstrumentChanelsQuantity				=	0,
						InstrumentConnectStatus					=	0,
						viInstrumentHandle						=	0;
	
	double				lfValue									=	0.0;
	
	FunctionGenerator_Init ( 0 , "GPIB:11" , &viInstrumentHandle , &InstrumentConnectStatus , &InstrumentChanelsQuantity );

	FunctionGenerator_Reset ( viInstrumentHandle ); 
	
	FunctionGenerator_Load_State ( viInstrumentHandle , 1 , "waveform_generator_setup_IFF\\MODE1.TFS" ); 
	
	FunctionGenerator_Signel_On ( viInstrumentHandle , 1 );
	FunctionGenerator_Signel_On ( viInstrumentHandle , 2 );
		
	FunctionGenerator_Initiate ( viInstrumentHandle ); 
		
	FunctionGenerator_Close(&viInstrumentHandle);
	
	return;
}
