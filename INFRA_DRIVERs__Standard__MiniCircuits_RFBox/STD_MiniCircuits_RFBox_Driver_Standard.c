//==============================================================================
// Include files 
#include "Windows.h"
#include <visa.h>

#include "cvidef.h"  
#include <userint.h>
#include <ansi_c.h>
#include <utility.h>
#include "GlobalDefines.h"
#include <formatio.h>
#include "toolbox.h"
#include "ModZT.h"
#include "STD_MiniCircuits_RFBox_Driver_Standard.h"

//==============================================================================
// Constants
#define			DEVICE_ID							STANDARD_MINI_CIRCUITS_RFBOX_DEVICE_ID

typedef enum { VALUE_MODE_DEFAULT , VALUE_MODE_ON_OFF , VALUE_MODE_SENTINEL } teValueModes;

typedef struct
{
	union
	{
		char							max_size[STD_BUFFER_SIZE];   
		
		struct					
		{
			char						szCommandName[LOW_STRING];
			
			char						szSubDeviceName[32], 
										szSubDeviceLocation[32];
			
			double						lfSetDelay,
										lfSetTimeout;  
			
			int							iValueMode;
			
			int							StateOnValue, 
							  			StateOffValue;
			//------------ Signature --------------------------------//
			unsigned __int64			ulSignatureID;
			//-------------------------------------------------------//			
			
			
			//---------- end of configuration sector ----------//
		};
	};
	
} tsSingleDeviceItem;

typedef struct
{
	CAObjHandle							hDriverHandle;  

	tsSingleDeviceItem					*pMINI_CIRCUITS_CommandList;
	int									listSize;
	
	int									iLastIndex;
	
	char								*pszAddress;
	
	double								lfLastSendCommandTime;
	
	tsSTD_CallBackSet					*ptCallbacks;

	STD_ERROR							*pLastError; 
	
}tsLocalStorage;


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

void*  	MINI_CIRCUITS_STD_Driver_GetErrorTextMessage ( int hHandle , int iError , char *pErrorMessage )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	IF (( hHandle == 0 ) , "Handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));  
	
	//MINI_CIRCUITS_GetErrorTextMessage( pLocalStorage->hDriverHandle , iError , pErrorMessage );
	
Error:
	
	if ( hHandle )
		CmtReleaseTSVPtr (hHandle);
	
	RETURN_STDERR_POINTER;
}
	 
void*			MINI_CIRCUITS_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								handle										=	0;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );
	
	if ( *pHandle == 0 )
	{
		CHK_CMT(CmtNewTSV ( sizeof(tsLocalStorage) , pHandle ));
	}
	
	handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStorage ));      
	
	if ( pLocalStorage->ptCallbacks  == NULL )
	{
		CALLOC( pLocalStorage->ptCallbacks , 1 , sizeof(tsSTD_CallBackSet)); 
	}
	
	memcpy( pLocalStorage->ptCallbacks , pCallBackSet , sizeof(tsSTD_CallBackSet));  

Error:
	
	if ( handle )
		CmtReleaseTSVPtr (handle);
	
	RETURN_STDERR_POINTER;
}

void*		MINI_CIRCUITS_STD_Driver_GetLowLevelHandle( int hHandle , int *phLowLevelHandle )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage						*pLocalStorage								=	NULL;			
	
	IF (( hHandle == 0 ) , "Handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));   
	
	
Error:
	
	if ( hHandle )
		CmtReleaseTSVPtr (hHandle);
	
	RETURN_STDERR_POINTER;
}

void*		MINI_CIRCUITS_STD_Driver_GetConnectionAddress( int hHandle , char **pszAddress )
{
	STD_ERROR							StdError									=	{0};

	tsLocalStorage						*pLocalStorage								=	NULL;			

	IF (( hHandle == 0 ) , "Handle can't be zero." );
	
	CHK_CMT ( CmtGetTSVPtr ( hHandle , &pLocalStorage ));

	if ( pszAddress )
	{
		CALLOC_COPY_STRING( *pszAddress , pLocalStorage->pszAddress );
	}
	
Error:
	
	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );

	RETURN_STDERR_POINTER;
}

void*			MINI_CIRCUITS_STD_Driver_Equipment_Info ( int hLowLevelHandle , char *pAddress , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware )
{
	STD_ERROR						StdError									=	{0};
	
    CAObjHandle 					handle										=	0;

	short							value										=	0;
	
	ERRORINFO						errorInfo									=	{0};
	
	STD_ERR( ModularZT_New_USB_Control(NULL,1,LOCALE_NEUTRAL,0,&handle));
	
	STD_ERR( ModularZT__USB_ControlConnect( handle , NULL , pAddress , &value ));
	
	if ( pModel )
	{
		STD_ERR(  ModularZT__USB_ControlRead_ModelName ( handle , &errorInfo , &pModel , &value ));
	}
		
	if ( pFirmware )
	{
		STD_ERR(  ModularZT__USB_ControlGetFirmware ( handle , &errorInfo , &value ));
		
		CALLOC( *pFirmware , 128 , sizeof(char));
		
		sprintf( *pFirmware , "%d" , value );
	}
	
	if ( pSerialNumber )
	{
		STD_ERR(  ModularZT__USB_ControlRead_SN ( handle , &errorInfo , &pSerialNumber , &value ));
	}

	if ( pVendor )
	{
		CALLOC_COPY_STRING( *pVendor , "Mini-Circuits" );  
	}
		
Error:
		
	
	RETURN_STDERR_POINTER;
}

void*			MINI_CIRCUITS_STD_Driver_Equipment_IsSupported ( int hLowLevelHandle , char *pAddress , char *pID_String , char *pIdentificationText , int *pbSupporting , void **pLocalData )
{
	STD_ERROR						StdError										=	{0};

	if (pbSupporting)
		*pbSupporting = 0;
	
	RETURN_STDERR_POINTER;
}

//==============================================================================

void*  MINI_CIRCUITS_STD_Driver_Close( int *pHandle )
{
	STD_ERROR					StdError					=	{0};
	
	tsLocalStorage				*pLocalStorage				=	NULL;			
	
	int 						Handle						=	0;
									
	IF (( pHandle == NULL ) , "Handle can't be NULL" );
	
	Handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr (Handle, &pLocalStorage));
	
	if ( pLocalStorage->hDriverHandle )
	{
		ModularZT__USB_ControlDisconnect( pLocalStorage->hDriverHandle , NULL );
		
		pLocalStorage->hDriverHandle = 0;
	}
	
Error:
	
	if (Handle)
	{
		FREE(pLocalStorage->pLastError);
		FREE(pLocalStorage->pMINI_CIRCUITS_CommandList);
		
		CmtReleaseTSVPtr (Handle);
		CmtDiscardTSV (Handle);
	}
	
	*pHandle = 0;
	
	RETURN_STDERR_POINTER;
}


void*  MINI_CIRCUITS_STD_Driver_Init( char* szRsrcAddress , int *phInstrumentHandle )
{
	STD_ERROR						StdError										=	{0};
	
	tsLocalStorage					*pLocalStorage									=	NULL;
	
	CmtTSVHandle 					handle											=	0;
	
	tsSTD_CallBackSet				tSTD_CallBackSet								=	{0};

	ERRORINFO						errorInfo										=	{0};

	short							value											=	0;
	
	IF (( szRsrcAddress == NULL ) , "Address string is empty" );
	IF (( phInstrumentHandle == NULL ) , "Handle can't be NULL" );  
	
	handle = *phInstrumentHandle;
	
	if ( handle == 0 )
	{
		CHK_CMT ( CmtNewTSV ( sizeof(tsLocalStorage) , &handle ));
		
		if ( phInstrumentHandle )
		{
			*(int*)phInstrumentHandle = handle;
		}
	}
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalStorage ));

	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks); 
	
	//STDF_CONFIG_VALUE("Timeout", VAL_DOUBLE , 1 , lfTimeout , lfDefaultTimeout );	
	
	STD_ERR( ModularZT_New_USB_Control(NULL,1,LOCALE_NEUTRAL,0,&(pLocalStorage->hDriverHandle)));
	
	STD_ERR( ModularZT__USB_ControlConnect ( pLocalStorage->hDriverHandle , NULL , szRsrcAddress , &value ));
	
	IF (( value == 0 ) , "Can't connect to the RF Box" );
	
	if ( strlen(szRsrcAddress) == 0 )
	{
		STD_ERR(  ModularZT__USB_ControlRead_SN ( pLocalStorage->hDriverHandle , &errorInfo , &pLocalStorage->pszAddress , &value ));    
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER;
}

void*      MINI_CIRCUITS_STD_Driver_SetValue( int hHandle , char *pCommandName , void *pValue , int iValueLength , int iValueSize , ...  )
{
	STD_ERROR					StdError						=	{0};
	
	tsLocalStorage				*pLocalStorage					=	NULL;			
	
	int							iCommandIndex					=	0,
								bCommandFound					=	0;

	char						szCommand[LOW_STRING]			=	{0};

	unsigned int				iValue							=	0;
	
	short						value							=	0;
	
	char						*retStr							=	NULL;	
	
	char						*pError							=	szCommand,
								*pCommand						=	szCommand;
	
	double						lfDelay							=	0.0,
								lfCurrentTime					=	0.0;
	
	IF (( hHandle == 0 ) , "Handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle, &pLocalStorage));

	IF (( pValue == NULL ) ,"Value pointer is NULL" );  
	
	for ( iCommandIndex = 0 ; iCommandIndex < pLocalStorage->listSize ; iCommandIndex++ )
		if ((strlen(pLocalStorage->pMINI_CIRCUITS_CommandList[iCommandIndex].szCommandName)) && ( strcmp( pCommandName , pLocalStorage->pMINI_CIRCUITS_CommandList[iCommandIndex].szCommandName ) == 0 ))
		{
			bCommandFound = 1;
			break;
		}

	IF (( bCommandFound == 0 ) , "Command was not found.");   
	
	pLocalStorage->iLastIndex = iCommandIndex;
	
	switch(iValueSize)
	{
		case 1:
				if ( iValueLength == 1 )
					iValue = *((char*)pValue);
				
				if ( iValueLength > 1 )
					iValue = atoi((char*)pValue);  
					
				break;
		case 2:
				iValue = *((short*)pValue);
				break;
		case 4:
				iValue = *((int*)pValue);
				break;						
		case 8:
				iValue = (char)*((__int64*)pValue);
				break;

		default:
				break;
	}
			
	sprintf( szCommand , ":%s:%s:STATE:%d" , pLocalStorage->pMINI_CIRCUITS_CommandList[iCommandIndex].szSubDeviceName , pLocalStorage->pMINI_CIRCUITS_CommandList[iCommandIndex].szSubDeviceLocation , iValue );

	GetCurrentDateTime (&lfCurrentTime);
	
	lfDelay = pLocalStorage->pMINI_CIRCUITS_CommandList[iCommandIndex].lfSetDelay - ( lfCurrentTime - pLocalStorage->lfLastSendCommandTime );
	
	if ( lfDelay > 0.0 )
		DelayWithEventProcessing(lfDelay);
	
	STD_ERR( ModularZT__USB_ControlSend_SCPI ( pLocalStorage->hDriverHandle , NULL , &pCommand ,&retStr, &value ));
	
	pLocalStorage->lfLastSendCommandTime = lfCurrentTime;
	
	IF (( value == 0 ) , "Not successfully." );
	
	if ( retStr )
	{
		StdError.error = atoi(retStr);
		
		if ( StdError.error < 0 )
		{
			pError = strchr( retStr , ' ' );
			
			if ( pError )
			{
				FORCE_ERR( StdError.error , pError )
			}
		}
	}
	
Error:
	
	if (hHandle)
		CmtReleaseTSVPtr (hHandle);
	
	RETURN_STDERR_POINTER;
}

void*      MINI_CIRCUITS_STD_Driver_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize, ...  )    
{
	STD_ERROR					StdError						=	{0};
	
	tsLocalStorage				*pLocalStorage					=	NULL;
	
	short						value							=	0;
	
	char						*retStr							=	NULL;	

	char						szCommand[LOW_STRING]			=	{0};
	
	int							iCommandIndex					=	0;

	char						*pError							=	szCommand,
								*pCommand						=	szCommand;

	IF (( hHandle == 0 ) , "Handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle, &pLocalStorage));

	IF (( pGetValue == NULL ), "Get Value pointer is NULL" ); 
	
	iCommandIndex = pLocalStorage->iLastIndex;

	sprintf( szCommand , ":%s:%s:STATE?" , pLocalStorage->pMINI_CIRCUITS_CommandList[iCommandIndex].szSubDeviceName , pLocalStorage->pMINI_CIRCUITS_CommandList[iCommandIndex].szSubDeviceLocation );
	
	STD_ERR( ModularZT__USB_ControlSend_SCPI ( pLocalStorage->hDriverHandle , NULL , &pCommand ,&retStr, &value ));
	
	IF (( value == 0 ) , "Not successfully." );
	
	if ( retStr )
	{
		StdError.error = atoi(retStr);
		
		if ( IS_NOT_OK )
		{
			pError = strchr( retStr , ' ' );
			
			if ( pError )
			{
				FORCE_ERR( StdError.error , pError )
			}
		}
	}
	
Error:
	
	if (hHandle)
		CmtReleaseTSVPtr (hHandle);
	
	RETURN_STDERR_POINTER;	
}
 
void*      MINI_CIRCUITS_STD_Driver_GetValue_Ex( int hHandle , char *pCommandName , void **pGetValue , int *piValueLength , int *piValueSize, ...  )   
{
	STD_ERROR					StdError					=	{0};
	
	tsLocalStorage				*pLocalStorage				=	NULL;			
	
	int							iCommandIndex				=	0;
	
	int							bCommandFound				=	0;
							
	short						value							=	0;
	
	char						*retStr							=	NULL;	

	char						szCommand[LOW_STRING]			=	{0};

	char						*pError							=	szCommand,
								*pCommand						=	szCommand;	
	
	IF (( hHandle == 0 ) , "Handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle, &pLocalStorage));

	IF (( pGetValue == NULL ) ,"Value pointer is NULL" ); 
	
	for ( iCommandIndex = 0 ; iCommandIndex < pLocalStorage->listSize ; iCommandIndex++ )
		if ((strlen(pLocalStorage->pMINI_CIRCUITS_CommandList[iCommandIndex].szCommandName)) && ( strcmp( pCommandName , pLocalStorage->pMINI_CIRCUITS_CommandList[iCommandIndex].szCommandName ) == 0 ))
		{
			bCommandFound = 1;
			break;
		}

	IF (( bCommandFound == 0 ) , "Command was not found.");   
	
	sprintf( szCommand , ":%s:%s:STATE?" , pLocalStorage->pMINI_CIRCUITS_CommandList[iCommandIndex].szSubDeviceName , pLocalStorage->pMINI_CIRCUITS_CommandList[iCommandIndex].szSubDeviceLocation );

	STD_ERR( ModularZT__USB_ControlSend_SCPI ( pLocalStorage->hDriverHandle , NULL , &pCommand ,&retStr, &value ));
	
	IF (( value == 0 ) , "Not successfully." );
	
	if ( retStr )
	{
		StdError.error = atoi(retStr);
		
		if ( IS_NOT_OK )
		{
			pError = strchr( retStr , ' ' );
			
			if ( pError )
			{
				FORCE_ERR( StdError.error , pError )
			}
		}
	}
	
Error:
	
	if (hHandle)
		CmtReleaseTSVPtr (hHandle);
	
	RETURN_STDERR_POINTER;
}

void*      MINI_CIRCUITS_STD_Driver_Check_Connection( int hHandle , char *pCommandName , int *piStatus )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	int							iCommandIndex										=	0,
								bCommandFound										=	0; 
									
	
	IF (( hHandle == 0 ) , "Handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));  
	
	IF (( piStatus == 0 ) , "Status parameter can't be NULL." );
	IF (( pCommandName == NULL ) , "Command can't be NULL." );
	
	*piStatus = 0;
	
	for ( iCommandIndex = 0 ; iCommandIndex < pLocalStorage->listSize ; iCommandIndex++ )
		if ((strlen(pLocalStorage->pMINI_CIRCUITS_CommandList[iCommandIndex].szCommandName)) && ( strcmp( pCommandName , pLocalStorage->pMINI_CIRCUITS_CommandList[iCommandIndex].szCommandName ) == 0 ))
		{
			bCommandFound = 1;
			break;
		}
		
	IF (( bCommandFound == 0 ) , "Command was not found.");   
		
Error:

	if ( piStatus )
		*piStatus = bCommandFound;
	
	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

void*      MINI_CIRCUITS_STD_Driver_Unplug( int hHandle , double lfTimeOut )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
	
	IF (( hHandle == 0 ) , "Handle can't be zero." );

	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage )); 
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}


void*      MINI_CIRCUITS_STD_Driver_PlugIn( int hHandle , double lfTimeOut )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
	
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));  
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

void*      MINI_CIRCUITS_STD_Driver_EnablePlugInAutomatically( int hHandle , int bEnable )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
	
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));  
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

void*      MINI_CIRCUITS_STD_Driver_InstallReceiveCallbackFunction( int hHandle , tfStdReceiveCallback pfFunction , void *pData )
{
	STD_ERROR					StdError											=	{0};
			
	RETURN_STDERR_POINTER;
}


void*      MINI_CIRCUITS_STD_Driver_LoadConfigFile( int hHandle , char *pFileName )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			

	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};
	
	int							hFileHandle											=	0;
				
	int							iIndex												=	0,  
								iCount												=	0;
	
	char						szFormatedLog[LOW_STRING]							=	{0};

	int							iFileSize											=	0;
	
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage )); 
	
	IF (( pFileName == NULL ) , "File name is empty." );
	IF (( FileExists( pFileName , &iFileSize ) == 0 )  , "File was not found." );  
	
	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks); 
	
	if ( pLocalStorage->ptCallbacks )
	{
		sprintf( szFormatedLog , "%s :: Loading configuration %%s" ,  pLocalStorage->ptCallbacks->szName );
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	}
	
	hFileHandle = OpenFile ( pFileName , VAL_READ_ONLY, VAL_TRUNCATE, VAL_BINARY);
	
	FREE_CALLOC_ERR( pLocalStorage->pMINI_CIRCUITS_CommandList , ((iFileSize / sizeof(tsSingleDeviceItem))+1) , sizeof(tsSingleDeviceItem));
	
	do
	{
		iCount = ReadFile ( hFileHandle , pLocalStorage->pMINI_CIRCUITS_CommandList[iIndex].max_size , sizeof(tsSingleDeviceItem) );
		
		if ( iCount != sizeof(tsSingleDeviceItem) )
			break;
		
		if ( pLocalStorage->pMINI_CIRCUITS_CommandList[iIndex].ulSignatureID != 0 ) 
			if ( pLocalStorage->pMINI_CIRCUITS_CommandList[iIndex].ulSignatureID != DEVICE_ID )
				break;
		
		iIndex++;
		
	} while(1);
	
	pLocalStorage->listSize = iIndex;

Error:

	if ( hFileHandle )
		CloseFile (hFileHandle);
	
	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

void*      MINI_CIRCUITS_STD_Driver_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			

	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};

	int							iIndex												=	0,  
								iBaseSize											=	0, 
								iCount												=	0;

	char						szFormatedLog[LOW_STRING]							=	{0};
	
	IF (( hHandle == 0 ) , "handle can't be zero." ); 
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));   
	
	IF (( pBuffer == NULL ) , "Buffer is empty." );
	
	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks); 
	
	sprintf( szFormatedLog , "%s :: Loading configuration %%s" ,  pLocalStorage->ptCallbacks->szName );
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
		
	FREE_CALLOC_ERR( pLocalStorage->pMINI_CIRCUITS_CommandList , 256 , sizeof(tsSingleDeviceItem));
	
	iCount = iBufferSize;
	iBaseSize = sizeof(tsSingleDeviceItem); 
	
	for ( iIndex = 0 ; iIndex < 256; iIndex++ )
	{
		if ( iCount < iBaseSize )   
			break;
		
		memcpy( pLocalStorage->pMINI_CIRCUITS_CommandList[iIndex].max_size , pBuffer , iBaseSize );
		
		if ( pLocalStorage->pMINI_CIRCUITS_CommandList[iIndex].ulSignatureID != 0 ) 
			if ( pLocalStorage->pMINI_CIRCUITS_CommandList[iIndex].ulSignatureID != DEVICE_ID )
				break;
				
		pBuffer += iBaseSize;
		iCount -= iBaseSize; 
	}
	
	pLocalStorage->listSize = iIndex;

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}



void*			MINI_CIRCUITS_STD_Driver_Get_Commands_List( int hHandle , char ***pCommandsList , int *piNumberOfCommands )     
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			

	int							iIndex												=	0;

	int							iNumberOfCommands									=	0;

	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));    
	
	IF (( pCommandsList == NULL ) , "Pointer to List is empty." );
	
	CALLOC_ERR( (*pCommandsList) , pLocalStorage->listSize+1 , sizeof(char*));
	
	for ( iIndex = 0 ; iIndex < pLocalStorage->listSize; iIndex++ )
	{
		CALLOC_COPY_STRING_ERR( (*pCommandsList)[iIndex] , pLocalStorage->pMINI_CIRCUITS_CommandList[iIndex].szCommandName );
		
		if ( strlen((*pCommandsList)[iIndex]))
			iNumberOfCommands++;
	}

Error:
	
	if ( piNumberOfCommands )
		*piNumberOfCommands = iNumberOfCommands;

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}


