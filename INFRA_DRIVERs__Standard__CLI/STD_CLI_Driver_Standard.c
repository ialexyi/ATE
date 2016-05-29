//==============================================================================
// Include files
#include "windows.h"
#include "cvidef.h"    
#include "toolbox.h"
#include <utility.h>
#include "GlobalDefines.h"
#include <visa.h>
#include <formatio.h>
#include <userint.h>
#include <ansi_c.h>
#include "STD_CLI_Driver_Standard.h"

//==============================================================================
// Constants

#define				MIN_MESSAGE_LEN				5
#define				DEVICE_ID					STANDARD_CLI_DRIVER_DEVICE_ID
//==============================================================================
// Types


typedef union
{
	char				max_size[64];   
	
	struct
	{  
		int				iParametersType;								// 1 - int , 2 - long , 3 - double , 4 - string       
		
		char			szParamName[12];
		
		char			szDefaultValue[16],
						szFormatValue[16]; 
	};
	
} tsSEND_PARAM_ITEM;

typedef union
{
	char				max_size[64];   
	
	struct
	{  
		int				iParametersType;								// 1 - int , 2 - long , 3 - double , 4 - string  

		char			szParamName[12];
		
		char			szStartPatern[16],
						szStopPatern[16]; 
	};
	
} tsRECEIVE_PARAM_ITEM;

typedef union
{
	char				max_size[STD_DOUBLE_BUFFER_SIZE];   
	
	struct
	{   
		//---------- Send Side -----------------//
		char							szSendCommandFormated[64];
		int								iSendNumberOfParameters; 
		tsSEND_PARAM_ITEM				tSendParameters[8];
	
		//---------- Receive Side -----------------//
		char							szRespondsFormated[64];
		int								iReceiveNumberOfParameters;
		tsRECEIVE_PARAM_ITEM			tReceiveParameters[8];
		
		char							szCommandName[64];
		
		//------------ Signature --------------------------------//
		unsigned __int64				ulSignatureID;
		//-------------------------------------------------------//			
	};
	
} tsSTD_CLI_CommandItem;

typedef struct
{
	int							hCommunicationHandle;
	
	//------------- Communication ---------------//
	unsigned char 				*pBuffer;
	unsigned int				iBufferLength;

	unsigned char 				*pSourceBuffer;
	unsigned int				iSourceBufferLength;
	
	int							bDataUpdated,
								bDataUpdateFailed,
								iNumberOfCommunicationTry;

	int							iLastSentCommandIndex;
	
	double						lfSendTimeOut,
								lfReceiveTimeOut;
								
	tsSTD_CallBackSet			*ptCallbacks;
	
	tsSTD_CLI_CommandItem		*pSTD_CLI_CommandList;
	int							listSize;

	int							bEnableAutoPlugIn;							
	
	int							bClosed;
	
	void						*pCallbackFunction,
								*pCallbackData;
	
	int							hCallbackFunctionThreadId;
	
	STD_ERROR					*pLastError; 
	
} tsLocalStorage;


//==============================================================================
// Static global variables

//==============================================================================
// Static functions
void*   DLLEXPORT       STD_CLI_Driver_Send_Patern( int hCommunicationHandle , tsSTD_CLI_CommandItem tCommandItem , void *pValue , int iValueLength , int iValueSize );

void	STD_CLI_Driver_ReceiveCallBack( void *pData );
//==============================================================================
// Global variables

//==============================================================================
// Global functions

void*	DLLEXPORT	STD_CLI_Driver_Config_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet )
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
	
	if ( pLocalStorage->ptCallbacks == NULL )
	{
		CALLOC_ERR( pLocalStorage->ptCallbacks , 1 , sizeof(tsSTD_CallBackSet));  
	}
	
	memcpy( pLocalStorage->ptCallbacks , pCallBackSet , sizeof(tsSTD_CallBackSet));  

Error:
	
	if ( handle )
		CmtReleaseTSVPtr (handle);
	
	RETURN_STDERR_POINTER;
}

void*	DLLEXPORT	STD_CLI_Driver_Config_LOG_SetAllowState( int iHandle , int bAllowLog ) 
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	IF (( iHandle == 0 ) , "Driver has not initialized" );

	CHK_CMT( CmtGetTSVPtr ( iHandle , &pLocalStorage ));
	
	pLocalStorage->ptCallbacks->bDisableLog = !bAllowLog;

Error:
	
	if ( iHandle )
		CmtReleaseTSVPtr (iHandle);
	
	RETURN_STDERR_POINTER;
}

//------------------- Standard UUT Driver Protocol --------------------------------------------------//
void*	DLLEXPORT	STD_CLI_Driver_Config_Install_CommentCallback( CmtTSVHandle *pHandle , pfCommentCallback fFunction , void *pData , int type) 
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
	
	if ( pLocalStorage->ptCallbacks == NULL )
	{
		CALLOC_ERR( pLocalStorage->ptCallbacks , 1 , sizeof(tsSTD_CallBackSet));  
	}
	
	pLocalStorage->ptCallbacks->fCommentCallback = fFunction;
	pLocalStorage->ptCallbacks->pCommentCallbackData = pData;
	pLocalStorage->ptCallbacks->commentType = type;
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr (handle);
	
	RETURN_STDERR_POINTER;
}

void*	DLLEXPORT	STD_CLI_Driver_Config_Install_ConfigValueCallback( CmtTSVHandle *pHandle , pfConfigValueCallback fFunction , void *pData , int type) 
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
	
	if ( pLocalStorage->ptCallbacks == NULL )
	{
		CALLOC_ERR( pLocalStorage->ptCallbacks , 1 , sizeof(tsSTD_CallBackSet));  
	}
	
	pLocalStorage->ptCallbacks->fConfigValueCallback = fFunction;
	pLocalStorage->ptCallbacks->pConfigValueCallbackData = pData;
	pLocalStorage->ptCallbacks->configType = type;
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr (handle);
	
	
	RETURN_STDERR_POINTER;
}																	   		

void*	DLLEXPORT	STD_CLI_Driver_Config_Install_CheckForBreakCallback( CmtTSVHandle *pHandle , pfCheckForBreakCallback fFunction , void *pData , int type) 
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
	
	if ( pLocalStorage->ptCallbacks == NULL )
	{
		CALLOC_ERR( pLocalStorage->ptCallbacks , 1 , sizeof(tsSTD_CallBackSet));  
	}
	
	pLocalStorage->ptCallbacks->fCheckForBreakCallback = fFunction;
	pLocalStorage->ptCallbacks->pCheckForBreakCallbackData = pData;
	pLocalStorage->ptCallbacks->breakType = type;
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr (handle);
	
	
	RETURN_STDERR_POINTER;
}

//typedef void (*pfCloseCallBack) ( unsigned long hCommHandle , int error , void *data );
void   DLLEXPORT   STD_CLI_Driver_Close_CallBack ( int hCommHandle , int error , void *pData )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								*pHandle									=	0,
									handle										=	0;
	pHandle = pData;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );

	handle = *pHandle;
	
	FREE(pHandle);
	
	if ( CmtGetTSVPtr ( handle , &pLocalStorage ) == 0 )
	{
		CALLOC( pLocalStorage->pLastError , 1 , sizeof(STD_ERROR));
		
		pLocalStorage->pLastError->error = error;
		pLocalStorage->bClosed = 1;
	}
	else
		handle = 0;
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return;
}

//typedef void (*pfErrorCallBack) ( unsigned long hCommHandle , void *pErrEv , void *data );  
void   DLLEXPORT   STD_CLI_Driver_Error_CallBack ( int hCommHandle , void *pStdError , void *pData )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								*pHandle									=	0,
									handle										=	0;
	
	pHandle = pData;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );

	handle = *pHandle;
	
	if ( CmtGetTSVPtr ( handle , &pLocalStorage ) == 0 )
	{
		do
		{
			if ( pStdError )
			{
				FREE_STDERR_COPY(pStdError);
				pStdError = NULL;
				
				if ( StdError.error ) //== -1073807360 )
				{
					OutputDebugString("\t\tUUT Driver Error : Unplugging communication.");   
					FREE_STDERR_COPY_ERR( Communication_Unplug( pLocalStorage->hCommunicationHandle , 10.0 ));
					
					if ( pLocalStorage->bEnableAutoPlugIn )
					{
						OutputDebugString("\t\tUUT Driver       : Plugging back communication.");   
						FREE_STDERR_COPY_ERR( Communication_PlugIn( pLocalStorage->hCommunicationHandle , 10.0 )); 
					}
					else
						OutputDebugString("\t\tUUT Driver       : Plugging back is disable.");   
						
					break;
				}
	
				FREE_STDERR_COPY_ERR(pStdError);
			}
			
		} while(0);
	}
	else
		handle = 0;
Error:
	
	FREE(pStdError);
	
	if ( handle )
	{
		//GET_RET( pLocalStorage->pLastError ); 
		CmtReleaseTSVPtr ( handle );
	}
	
	return;
}

//====================================================================================================//
//typedef void (*pfReceiveCallBack) ( char *buffer , unsigned int count , void *data ); 
void	DLLEXPORT	STD_CLI_Driver_Receive_CallBack( unsigned char *pBuffer , unsigned int iBufferLen , void *pData )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			

	int								*pHandle									=	0,
									handle										=	0;
	
	int								iLastSentCommandIndex						=	0;
	
	int								bDataUpdateFailed							=	0;
	
	char							*pReceiveCheck								=	NULL,
									*pTemp										=	NULL;
	pHandle = pData;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );
	IF (( pBuffer == NULL ) , "Receive NULL pointer buffer." );   
	
	handle = *pHandle;
	
	if ( CmtGetTSVPtr ( handle , &pLocalStorage ) == 0 )
	{
		do
		{
			iLastSentCommandIndex = pLocalStorage->iLastSentCommandIndex;
		
			if ( iLastSentCommandIndex < 0 )
				break;
		
			pLocalStorage->iBufferLength = 0;
			GET_STDERR_CLEAR( pLocalStorage->pLastError );
	
			iLastSentCommandIndex = pLocalStorage->iLastSentCommandIndex;
	
			pReceiveCheck = StringReplaceAll( pBuffer  , "\n" , "\\n" , NULL );
	
			pTemp = pReceiveCheck;
		
			pReceiveCheck = StringReplaceAll( pTemp , "\r" , "\\r" , NULL ); 
	
			do  
			{ 
				if ( pReceiveCheck && ( strstr( pReceiveCheck , pLocalStorage->pSTD_CLI_CommandList[iLastSentCommandIndex].szRespondsFormated ) == NULL ))  
				{
					bDataUpdateFailed = 1;
					
					break;
				}
				
				FREE_CALLOC_COPY( pLocalStorage->pSourceBuffer , iBufferLen + 100 , sizeof(char) , pBuffer , iBufferLen );   
				pLocalStorage->iSourceBufferLength = iBufferLen;
			
			}while(0);
		
			if ( pLocalStorage->pCallbackFunction )
			{
				CALLOC_ERR( pHandle , 1 , sizeof(int));
				*pHandle = handle;
				pData = pHandle;
		
				PostDeferredCallToThread ( STD_CLI_Driver_ReceiveCallBack , pData , pLocalStorage->hCallbackFunctionThreadId );  
			}
			
		}while(0); 	
	}
	else
		handle = 0;
		
Error:
	
	if ( handle )
	{
		pLocalStorage->bDataUpdated = 1;
		pLocalStorage->bDataUpdateFailed = bDataUpdateFailed;
		GET_STDERR_CLEAR( pLocalStorage->pLastError );
		CmtReleaseTSVPtr ( handle );
	}
	
	FREE(pBuffer);
	FREE(pReceiveCheck); 
	FREE(pTemp); 
	
	return;
}

void	STD_CLI_Driver_ReceiveCallBack( void *pData )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			

	int								*pHandle									=	0,
									handle										=	0;
	
	int								iLastSentCommandIndex						=	0;
	
	char							*pCommand									=	NULL,
									*pReceiveBuffer								=	NULL;
									
	int								*pValue										=	NULL;

	int								iValueSize									=	0,
									iBufferSize									=	0; 
		
	tfStdReceiveCallback			receiveFunction								=	NULL;
	
	pHandle = pData;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );

	handle = *pHandle;
	
	FREE(pHandle);
	
	if ( CmtGetTSVPtr ( handle , &pLocalStorage ) == 0 )
	{
		iLastSentCommandIndex = pLocalStorage->iLastSentCommandIndex;  
		
		CALLOC_COPY_STRING( pCommand , pLocalStorage->pSTD_CLI_CommandList[iLastSentCommandIndex].szCommandName );

		CALLOC_COPY_ERR( pValue , (pLocalStorage->iBufferLength + 1) , 1 , pLocalStorage->pBuffer , pLocalStorage->iBufferLength );
		
		iValueSize = pLocalStorage->iBufferLength;
		
		CALLOC_COPY_ERR( pReceiveBuffer , (pLocalStorage->iSourceBufferLength + 1) , 1 , pLocalStorage->pSourceBuffer , pLocalStorage->iSourceBufferLength );
		
		iBufferSize = pLocalStorage->iSourceBufferLength;
		
		receiveFunction = pLocalStorage->pCallbackFunction;
		
		pData = pLocalStorage->pCallbackData;
		
		CmtReleaseTSVPtr ( handle ); 
	}	
		
	if ( receiveFunction )
		receiveFunction( pData , pCommand , pValue , iValueSize , pReceiveBuffer , iBufferSize ); 
Error:
	
	return;
}

//typedef int (*pfAttributeCallBack) ( unsigned long hCommHandle , void *data ); 
int DLLEXPORT			STD_CLI_Driver_Set_Attributes_CallBack( ViSession hCommHandle , void *pData )
{
	STD_ERROR						StdError									=	{0};
	
	tsSTD_CallBackSet				tSTD_CallBackSet							=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	ViUInt32						iDB_BaudRate							  	=	0,
									iDB_DataBits							  	=	0,
									iDB_Parity								  	=	0,
									iDB_StopBits							  	=	0,
									iDB_FlowCtrl							  	=	0;

	int								*pHandle									=	0,
									handle										=	0;
		
	char							szFormatedLog[LOW_STRING]					=	{0};
		
	pHandle = pData;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );

	handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStorage )); 
	
	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks);
	
	sprintf( szFormatedLog , "%s :: Configurating communication %%s" ,  pLocalStorage->ptCallbacks->szName );
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	
	iDB_BaudRate = 115200;
	
	STDF_CONFIG_VALUE("BaudRate", VAL_INTEGER , 1 , iDB_BaudRate , iDB_BaudRate );	
	
	CHK_VSA( viSetAttribute ( hCommHandle , VI_ATTR_ASRL_BAUD , iDB_BaudRate ));
	
	iDB_DataBits = 8;
	
	STDF_CONFIG_VALUE("DataBits", VAL_INTEGER , 1 , iDB_DataBits , iDB_DataBits );
		
	CHK_VSA( viSetAttribute (hCommHandle, VI_ATTR_ASRL_DATA_BITS, iDB_DataBits));
	
	iDB_Parity = 0;
	
	STDF_CONFIG_VALUE("Parity", VAL_INTEGER , 1 , iDB_Parity , iDB_Parity );
		
	CHK_VSA( viSetAttribute (hCommHandle, VI_ATTR_ASRL_PARITY, iDB_Parity));
	
	iDB_StopBits = 10;
	
	STDF_CONFIG_VALUE("StopBits", VAL_INTEGER , 1 , iDB_StopBits , iDB_StopBits );
		
	CHK_VSA( viSetAttribute (hCommHandle, VI_ATTR_ASRL_STOP_BITS, iDB_StopBits));
	
	iDB_FlowCtrl = 0;
	
	STDF_CONFIG_VALUE("FlowCtrl", VAL_INTEGER , 1 , iDB_FlowCtrl , iDB_FlowCtrl );
		
	CHK_VSA( viSetAttribute (hCommHandle, VI_ATTR_ASRL_FLOW_CNTRL, iDB_FlowCtrl));
	
	CHK_VSA( viSetAttribute ( hCommHandle , VI_ATTR_ASRL_DISCARD_NULL , VI_FALSE ));

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_ERROR_VALUE;
}


void*	DLLEXPORT		STD_CLI_Driver_Init( char *pszConnectionName , char *pszAddress , int *pHandle , ... )
{
	STD_ERROR						StdError									=	{0};
	
	tsSTD_CallBackSet				tSTD_CallBackSet							=	{0};

	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								handle										=	0;
	
	int								hCommunicationHandle						=	0;
	
	double							lfReceiveTimeOut							=	5.0,
									lfSendTimeOut								=	5.0; 
										
	int								iNumberOfReTry								=	3;
											 
	int								*pShareHandle								=	NULL;
	
	char							szFormatedLog[STD_STRING]					=	{0};
	
	int								bDB_Ascii									=	1;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );
	
	if ( *pHandle == 0 )
	{
		CHK_CMT(CmtNewTSV ( sizeof(tsLocalStorage) , pHandle ));
	}
	
	handle = *pHandle;
	
	pShareHandle = calloc(1,sizeof(int));
	
	IF (( pShareHandle == NULL ) , "Can't allocate memory." );
	
	*pShareHandle = handle;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	if ( pLocalStorage->ptCallbacks == NULL )
	{
		CALLOC_ERR( pLocalStorage->ptCallbacks , 1 , sizeof(tsSTD_CallBackSet));  
	}
		
	pLocalStorage->ptCallbacks->iInternalHandle = handle;
	
	strcpy( pLocalStorage->ptCallbacks->szName , pszConnectionName );
	
	pLocalStorage->iLastSentCommandIndex = -1;
	
	//listSize = GetCommandItemListSize(); 
		
	//CALLOC_ERR( pLocalStorage->pSTD_CLI_CommandList, listSize , sizeof(tsSTD_CLI_CommandItem));
	
	//IF (( pLocalStorage->pSTD_CLI_CommandList== NULL ) , "Can't allocate memory." ); 
	
	//for ( index = 0 ; index < ; index++ , listSize++ )
	//	memcpy( &(pLocalStorage->pSTD_CLI_CommandList[index]) , &(tSTD_CommandList[index]) , sizeof(tsSTD_CLI_CommandItem) ); 	
	
	//pLocalStorage->listSize = listSize;
	
	//pLocalStorage->ptCallbacks->iMultiUsingIndex = tConfig.iSlotNumber;   
	
	STDF_SET_MODULE_NAME(pLocalStorage->ptCallbacks,pszConnectionName);
	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks);
	
	if ( pszConnectionName )
	{
		sprintf( szFormatedLog , "Initializing \"%s\" Driver %%s" ,  pszConnectionName );
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	}
	
	//if ( strlen(tConfig.szConnectionAddress) == 0 )
	//{
	//	STDF_CONFIG_VALUE("CommunicationAddress", VALUE_TYPE_STRING , CONFIG_COM_STRUCT_LEN , tConfig.szConnectionAddress , tConfig.szConnectionAddress );  
	//}
													 

	STDF_CONFIG_VALUE("ReceiveTimeOut", VAL_DOUBLE , 1 , lfReceiveTimeOut , lfReceiveTimeOut );
	STDF_CONFIG_VALUE("NumberOfReTry", VAL_INTEGER , 1 , iNumberOfReTry , iNumberOfReTry );
	
	pLocalStorage->iNumberOfCommunicationTry = iNumberOfReTry;
	pLocalStorage->lfReceiveTimeOut = lfReceiveTimeOut;
	pLocalStorage->lfSendTimeOut = lfSendTimeOut;
	
	//if ( tConfig.lfTimeout > 0 )
	//	pLocalStorage->lfReceiveTimeOut = tConfig.lfTimeout;
	
	//if ( tConfig.iNumberOfRetries > 0 )
	//	pLocalStorage->iNumberOfCommunicationTry = tConfig.iNumberOfRetries;
		
	//strcpy( (char*)tConfigCommunication.szAddress , tConfig.szConnectionAddress );
	//tConfigCommunication.iType = tConfig.iConfigType;
	
	//memcpy( szConfigBuffer , &tConfigCommunication , sizeof(tsConfigCommunication));
	
	FREE_STDERR_COPY_ERR( Communication_Init( pszConnectionName , pszAddress , &hCommunicationHandle ));
	
	pLocalStorage->hCommunicationHandle = hCommunicationHandle;
		
	FREE_STDERR_COPY_ERR( Communication_InstallAttributeSetCallBack( hCommunicationHandle , (pfAttributeCallBack) STD_CLI_Driver_Set_Attributes_CallBack , (void *)pShareHandle ));
	
	FREE_STDERR_COPY_ERR( Communication_InstallReceiveCallBack( hCommunicationHandle , (pfReceiveCallBack)STD_CLI_Driver_Receive_CallBack , (void *)pShareHandle ));
	
	FREE_STDERR_COPY_ERR( Communication_InstallCloseCallBack( hCommunicationHandle , (pfCloseCallBack)STD_CLI_Driver_Close_CallBack , (void *)pShareHandle ));
	FREE_STDERR_COPY_ERR( Communication_InstallErrorCallBack( hCommunicationHandle , (pfErrorCallBack)STD_CLI_Driver_Error_CallBack , (void *)pShareHandle ));

	FREE_STDERR_COPY_ERR( Communication_SetPluggedInInitFailDelay( hCommunicationHandle , 1.0 )); //tConfig.lfInitFailWaitForDelay ));
		
	if ( pszConnectionName )
	{
		STDF_CONFIG_VALUE("Ascii", VAL_INTEGER , 1 , bDB_Ascii , bDB_Ascii ); 
		
		sprintf( szFormatedLog , "COM_%s.log" , pszConnectionName );
		FREE_STDERR_COPY_ERR( Communication_EnableLoging( hCommunicationHandle , 1 , szFormatedLog , bDB_Ascii , 0 ));
	}
	
	FREE_STDERR_COPY_ERR( Communication_EnableReceive( pLocalStorage->hCommunicationHandle , 1 )); 	
		
	FREE_STDERR_COPY_ERR( Communication_FlushReceiveBuffer( pLocalStorage->hCommunicationHandle , 1 )); 
	
	pLocalStorage->bEnableAutoPlugIn = 1; 
		
	if ( pszConnectionName )
	{
		sprintf( szFormatedLog , "Initializing \"%s\" Success %%s" ,  pszConnectionName );
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	}
	
Error:
	
	if (( IS_NOT_OK) && pszConnectionName )
	{
		sprintf( szFormatedLog , "Initializing \"%s\" Fail %%s" ,  pszConnectionName );
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	}
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER;
}


void*	DLLEXPORT		STD_CLI_Driver_Close(int *pHandle)
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								handle										=	0;
	
	double							lfCurrentTime								=	0.0,
									lfStartTime									=	0.0;
	
	int								bClosed										=	0;
	
	char							szFormatedLog[LOW_STRING]					=	{0};

	tsSTD_CallBackSet				tSTD_CallBackSet							=	{0};
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );
	
	handle = *pHandle;  

	IF (( handle == 0 ) , "No handle." );
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks); 
	
	sprintf( szFormatedLog , "%s :: Close communication %%s" ,  pLocalStorage->ptCallbacks->szName );
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	
	FREE_STDERR_COPY_ERR( Communication_Close( pLocalStorage->hCommunicationHandle ));

	GetCurrentDateTime(&lfStartTime);
	
	do
	{
		GetCurrentDateTime(&lfCurrentTime);
		
		if (( lfCurrentTime - lfStartTime ) > 10.0 )
			break;
		
		if ( CmtGetTSVPtr ( handle , &pLocalStorage ) == 0 )
		{
			bClosed = pLocalStorage->bClosed;
		
			CmtReleaseTSVPtr ( handle );
		}
		
		if ( bClosed )
			break;
		
		DelayWithEventProcessing(1.0);
		
	} while(!bClosed);
	
Error:
	
	if ( handle )
	{
		*pHandle = 0;
		
		FREE(pLocalStorage->pSTD_CLI_CommandList);
		
		FREE(pLocalStorage->pSourceBuffer);
			
		FREE(pLocalStorage->pBuffer);
		
		FREE(pLocalStorage->ptCallbacks);

		CmtReleaseTSVPtr ( handle );
	
		CmtDiscardTSV ( handle );

	}
	
	RETURN_STDERR_POINTER;
}


void*   DLLEXPORT       STD_CLI_Driver_GetConnectionAddress( int hHandle , char	**szAddress )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	IF (( hHandle == 0 ) , "Pointer to handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	FREE_STDERR_COPY_ERR( Communication_GetConnectionAddress( pLocalStorage->hCommunicationHandle , szAddress ));
	
Error:
	
	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	RETURN_STDERR_POINTER;
}



void*   DLLEXPORT       STD_CLI_Driver_Send_Patern( int hCommunicationHandle , tsSTD_CLI_CommandItem tCommandItem , void *pValue , int iValueLength , int iValueSize )
{
	STD_ERROR				StdError											=	{0};

	char					*pPrepereSendBuffer									=	NULL,
							*pSendBuffer										=	NULL;
							
	unsigned int			iMaxAllocationSize									=	0,
							iCurrentAllocationSize								=	0,
							index												=	0,
							copy_index											=	0,
							iContinuesCount										=	0, 
							bParameterExists									=	0;
	
	char					**pValuesArray										=	NULL;
	
	char					*pTempString										=	NULL,
							*pStartParam										=	NULL, 
							*pNextParam											=	NULL; 
	
	tuConvert				tConvertValue;
	
	char					*pStringData										=	NULL,
							*pStringDataNext									=	NULL,
							*pRespondsFormated     								=	NULL,
							*pResponds     										=	NULL; 
							
	int						iRespondsLength										=	0;
	
	//--- Calculating Size ----------------//
	iMaxAllocationSize = strlen( tCommandItem.szSendCommandFormated );
	
	CALLOC_ERR( pValuesArray , tCommandItem.iSendNumberOfParameters + 1 , sizeof(char*) );   
		
	pStringDataNext = pValue;
	
	for ( index = 0; index < tCommandItem.iSendNumberOfParameters ; index++ )
	{
		iCurrentAllocationSize = strlen( tCommandItem.tSendParameters[index].szDefaultValue);
		
		tConvertValue.uqWord = atol( tCommandItem.tSendParameters[index].szDefaultValue );
		
		if ( pValue && iValueLength && iValueSize )
		{
			if ( iValueSize == sizeof(char))
			{
				iCurrentAllocationSize += iValueLength;
				
				pStringData = pStringDataNext;
				pStringDataNext = strchr( pStringData , ',' ) + 1;
			}
			else
			{
				if ( iValueSize <= 8 )
				{
					if ( iValueLength == 1 )
					{
						memcpy( &(tConvertValue) , pValue , iValueSize );
					}
					else
					{
						memcpy( &(tConvertValue) , ((char*)pValue + (index*(iValueSize))) , iValueSize );
					}
				}
			}
			
			bParameterExists = 1;
		}
		else
			bParameterExists = 0;
		
		CALLOC_COPY_ERR( pValuesArray[index] , iCurrentAllocationSize + 100 , sizeof(char) , tCommandItem.tSendParameters[index].szDefaultValue , strlen(tCommandItem.tSendParameters[index].szDefaultValue)); 
		
		if ( bParameterExists )
		{
			switch( tCommandItem.tSendParameters[index].iParametersType ) // 1 - int , 2 - long , 3 - double , 4 - string    
			{
				case 1:
					switch( iValueSize )
					{
						case 2:
							sprintf( pValuesArray[index] , tCommandItem.tSendParameters[index].szFormatValue , tConvertValue.sWord );    
							break;
						
						case 4:
							sprintf( pValuesArray[index] , tCommandItem.tSendParameters[index].szFormatValue , tConvertValue.sdWord );    
							break; 
						
						case 8:
							sprintf( pValuesArray[index] , tCommandItem.tSendParameters[index].szFormatValue , tConvertValue.sqWord );    
							break;
							
						default:
							break;
					}
					break;
					
				case 2:
					switch( iValueSize )
					{
						case 2:
							sprintf( pValuesArray[index] , tCommandItem.tSendParameters[index].szFormatValue , tConvertValue.sWord );    
							break;
						
						case 4:
							sprintf( pValuesArray[index] , tCommandItem.tSendParameters[index].szFormatValue , tConvertValue.sdWord );    
							break; 
						
						case 8:
							sprintf( pValuesArray[index] , tCommandItem.tSendParameters[index].szFormatValue , tConvertValue.sqWord );    
							break;
							
						default:
							break;
					}
					break;
				case 3:
					switch( iValueSize )
					{
						case 8:
							sprintf( pValuesArray[index] , tCommandItem.tSendParameters[index].szFormatValue , tConvertValue.lfWord );    
							break;
						default:
							break;
					}
					break;
					
				case 4:
					switch( iValueSize )
					{
						case 1:
							
							if ( pStringData && strlen( pStringData ))
								sprintf( pValuesArray[index] , tCommandItem.tSendParameters[index].szFormatValue , pStringData );    
							
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		}
		
		iMaxAllocationSize += iCurrentAllocationSize + strlen(pValuesArray[index]);
	}
	
	CALLOC_ERR( pPrepereSendBuffer , iMaxAllocationSize + 10 , 1 );
	
	strcpy((char*)pPrepereSendBuffer , tCommandItem.szSendCommandFormated );
	
	for ( index = 0; index < tCommandItem.iSendNumberOfParameters ; index++ ) 
	{
		iValueLength = strlen(pValuesArray[index]);
			
		pStartParam = strstr((char*) pPrepereSendBuffer , "%s" );
		
		if ( pStartParam )
		{
			pNextParam = pStartParam+2;
			
			if ( pNextParam )
			{
				iContinuesCount = strlen(pNextParam);
					
				FREE_CALLOC_COPY_ERR( pTempString , iContinuesCount + 1 , 1 , pNextParam , iContinuesCount ); 
				
				for ( copy_index = 0; copy_index < iValueLength; copy_index++ ) 
					pStartParam[copy_index] = pValuesArray[index][copy_index];
				
				for ( copy_index = 0; copy_index < iContinuesCount; copy_index++ )
					pStartParam[copy_index+iValueLength] = pTempString[copy_index];
				
				pStartParam[copy_index+iValueLength] = 0;
			}
		}
	}
	
	FREE_STDERR_COPY_ERR( Communication_SetTerminatingPattern( hCommunicationHandle, "\r" , 1 ));  
	
	pRespondsFormated = StringReplaceAll( tCommandItem.szRespondsFormated , "\\r" , "\r" , NULL ); 							   
																															   
	pResponds = StringReplaceAll( pRespondsFormated , "\\n" , "\n" , NULL );
																															   
	if ( pResponds )
		iRespondsLength = strlen( pResponds );
		
	pSendBuffer = StringReplaceAll( pPrepereSendBuffer , "\\n" , "\n" , NULL );
	
	FREE( pPrepereSendBuffer );
	
	pPrepereSendBuffer = pSendBuffer;
	
	pSendBuffer = StringReplaceAll( pPrepereSendBuffer , "\\r" , "\r" , NULL ); 
	
	FREE_STDERR_COPY_ERR( Communication_Send( hCommunicationHandle , pSendBuffer , strlen(pSendBuffer) , iRespondsLength ));         
	
Error:
	
	FREE( pRespondsFormated );
	
	FREE( pResponds );
	
	FREE( pPrepereSendBuffer );
	
	FREE( pSendBuffer );
	
	FREE( pTempString );
	
	FREE_LIST( pValuesArray , tCommandItem.iSendNumberOfParameters );  
	
	RETURN_STDERR_POINTER;
}

///--------------------- API ------------------------------//

void*   DLLEXPORT   STD_CLI_Driver_SetValue( int hHandle , char *pCommandName , void *pValue , int iValueLength , int iValueSize , ...  )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL,			
								tLocalStorage										=	{0};
	
	int							bCommandFound										=	0,
								iCommandIndex										=	0;
	
	char						szFormatedLog[LOW_STRING]							=	{0};
	
	double						lfStartTime											=	0.0,
								lfCurrentTime										=	0.0;
	
	int							iTryCounter											=	0;
	
	char						szLog[1024]											=	{0};
	
	int							bDataUpdated										=	0, 
								bDataUpdateFailed									=	0, 
								bPluggedInStatus									=	0,  
								bSentSuccessfully									=	0; 
									
	double						lfWaitForConnectionTimeout							=	0.0;
	
	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};

	IF (( hHandle == 0 ) , "handle can't be zero." );
	IF ((( iValueLength > 0 ) && ( pValue == NULL )) , "Value can't be NULL." ); 
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	memcpy( &tLocalStorage , pLocalStorage , sizeof(tsLocalStorage));
	CHK_CMT(CmtReleaseTSVPtr ( hHandle ));   
	
	STDF_UPDATE_CALLBACK_DATA(tLocalStorage.ptCallbacks);
	
	GetCurrentDateTime(&lfStartTime); 
	
	lfWaitForConnectionTimeout = (double)tLocalStorage.iNumberOfCommunicationTry * tLocalStorage.lfReceiveTimeOut;
		
	do
	{
		FREE_STDERR_COPY( Communication_GetPluggedInStatus( tLocalStorage.hCommunicationHandle , &bPluggedInStatus ));  

		if ( bPluggedInStatus == 0 )
			break;
		
		CHK_CMT( CmtReleaseTSVPtr ( hHandle ));
					
		DelayWithEventProcessing(0.2);
	
		CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
			
		GetCurrentDateTime(&lfCurrentTime);
			
	} while((lfCurrentTime-lfStartTime) < lfWaitForConnectionTimeout );

	IF (( bPluggedInStatus ) , "Communication is not established");
	
	//STDF_UPDATE_CALLBACK_DATA(tLocalStorage.tCallbacks); 
	
	for ( iCommandIndex = 0 ; iCommandIndex < tLocalStorage.listSize ; iCommandIndex++ )
		if ( strcmp( pCommandName , tLocalStorage.pSTD_CLI_CommandList[iCommandIndex].szCommandName ) == 0 )
		{
			bCommandFound = 1;
			break;
		}
		
	IF (( bCommandFound == 0 ) , "Command was not found.");   
		
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));  
	pLocalStorage->iLastSentCommandIndex = iCommandIndex;
	pLocalStorage->bDataUpdated = 0;
	CHK_CMT(CmtReleaseTSVPtr ( hHandle )); 
	
	for ( iTryCounter = 0 ; iTryCounter < tLocalStorage.iNumberOfCommunicationTry ; iTryCounter++ )
	{
		FREE_STDERR_COPY_ERR( STD_CLI_Driver_Send_Patern( tLocalStorage.hCommunicationHandle , tLocalStorage.pSTD_CLI_CommandList[iCommandIndex] , pValue , iValueLength , iValueSize )); 
		
		FREE_STDERR_COPY( Communication_GetSendStatus ( tLocalStorage.hCommunicationHandle , &bSentSuccessfully ));   
		
		if ( bSentSuccessfully == 0 )
			continue;
		
		if ( IS_NOT_OK )
		{
			OutputDebugString("\t\tUUT Driver Error : Can't send command.");   
			DelayWithEventProcessing(5.0);
			sprintf( szLog , "\t\tUUT Driver       : Try to send command again. (%d/%d)" , iTryCounter+1 , tLocalStorage.iNumberOfCommunicationTry );
			OutputDebugString(szLog);  
			continue;
		}
		
		if (  IS_NOT_OK )
			continue;
		
		GetCurrentDateTime(&lfStartTime); 
		
		do
		{
			CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage )); 
		
			if ( pLocalStorage->pLastError )
			{
				UPDATERR( *(pLocalStorage->pLastError) ); 
			}
			
			bDataUpdated = pLocalStorage->bDataUpdated;
			bDataUpdateFailed = pLocalStorage->bDataUpdated;
			
			CHK_CMT(CmtReleaseTSVPtr ( hHandle ));            
			
			if ((IS_NOT_OK) || bDataUpdated ) 
				break;
			
			GetCurrentDateTime(&lfCurrentTime);
			
		} while((lfCurrentTime-lfStartTime) < tLocalStorage.lfReceiveTimeOut );
		
		if ((IS_OK) && bSentSuccessfully && ( bDataUpdateFailed == 0 ) ) 
			break;
	}
	
Error:

	if ((IS_OK) && bSentSuccessfully ) 
		sprintf( szFormatedLog , "%s :: Sent command [%s] %%s" ,  tLocalStorage.ptCallbacks->szName , pCommandName );
	else
		sprintf( szFormatedLog , "%s :: Error sending command [%s] %%s" ,  tLocalStorage.ptCallbacks->szName , pCommandName );
	
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");  
	
	RETURN_STDERR_POINTER;
}



void*   DLLEXPORT   STD_CLI_Driver_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize, ...  )    
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL,			
								tLocalStorage										=	{0};			

	char						szFormatedLog[LOW_STRING]							=	{0};
								
	double						lfStartTime											=	0.0,
								lfCurrentTime										=	0.0;
	
	int							iLastSentCommandIndex								=	0;
	
	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};

	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	memcpy( &tLocalStorage , pLocalStorage , sizeof(tsLocalStorage));
	CHK_CMT(CmtReleaseTSVPtr ( hHandle ));   
	
	STDF_UPDATE_CALLBACK_DATA(tLocalStorage.ptCallbacks);
	
	iLastSentCommandIndex = tLocalStorage.iLastSentCommandIndex;
	
	if ( pGetCommandName )
	{
		FREE_CALLOC_COPY_STRING( *pGetCommandName , tLocalStorage.pSTD_CLI_CommandList[iLastSentCommandIndex].szCommandName );
	}
		
	GetCurrentDateTime(&lfStartTime);

	do
	{
		ProcessSystemEvents();
		
		CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
		memcpy( &tLocalStorage , pLocalStorage , sizeof(tsLocalStorage));
		CHK_CMT(CmtReleaseTSVPtr ( hHandle ));   
	
		if ( tLocalStorage.bDataUpdated )
			break;
		
		if ( tLocalStorage.pLastError->error ) 
			break;
		
		GetCurrentDateTime(&lfCurrentTime);
		
	} while((lfCurrentTime-lfStartTime) < tLocalStorage.lfReceiveTimeOut );
	
	if ((lfCurrentTime-lfStartTime) >= tLocalStorage.lfReceiveTimeOut )
		OutputDebugString("\t\tUUT Driver Error : Receive timeout."); 
		
	if ( tLocalStorage.bDataUpdated )
	{
		if ( pGetValue )
		{
			FREE_CALLOC_COPY_ERR( ((char*)(*pGetValue)), (tLocalStorage.iBufferLength + 1) , 1 , tLocalStorage.pBuffer , tLocalStorage.iBufferLength );
		}
		
		if ( piValueLength )
			*piValueLength = tLocalStorage.iBufferLength;
		
		if ( piValueSize )
			*piValueSize = 1;
		
		CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage )); 
		pLocalStorage->bDataUpdated = 0;
		CHK_CMT(CmtReleaseTSVPtr ( hHandle ));   
		
		sprintf( szFormatedLog , "%s :: Has Received respond of command [%s] %%s" ,  tLocalStorage.ptCallbacks->szName , tLocalStorage.pSTD_CLI_CommandList[iLastSentCommandIndex].szCommandName );
	
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d");  
	
	}
	else
	{
	
		sprintf( szFormatedLog , "%s :: Hasn't Received respond of command [%s] %%s" ,  tLocalStorage.ptCallbacks->szName , tLocalStorage.pSTD_CLI_CommandList[iLastSentCommandIndex].szCommandName );
	
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d");  
	}
	
Error:

	if ( StdError.error == 0 )
	{
		if ( CmtGetTSVPtr ( hHandle , &pLocalStorage ) == 0 )
		{
			if ( pLocalStorage->pLastError )
			{
				UPDATERR( *pLocalStorage->pLastError );
			}
	
			memset( &(pLocalStorage->pLastError) , 0 , sizeof(STD_ERROR)); 
			
			CmtReleaseTSVPtr ( hHandle );
		}
	}
	
	RETURN_STDERR_POINTER;
}

void*   DLLEXPORT   STD_CLI_Driver_GetValue_Ex( int hHandle , char *pCommandName , void **pGetValue , int *piValueLength , int *piValueSize, ...  )    
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL,			
								tLocalStorage										=	{0};			

	int							bCommandFound										=	0,
								iCommandIndex										=	0,
								iValueCounter										=	0,
								index												=	0; 
									
	double						lfStartTime											=	0.0,
								lfCurrentTime										=	0.0,
								lfTimeout											=	0.0;
									
	int							iTryCounter											=	0,
								bNotFound											=	0;
	
	char						szLog[1024]											=	{0};
	
	int							bPluggedInStatus									=	0,  
								bSentSuccessfully									=	0; 
									
	double						lfWaitForConnectionTimeout							=	0.0;
	
	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};
	
	char						*pNext												=	NULL,
								*pValue												=	NULL,
								*pSentCommand										=	NULL,
								*pTemp												=	NULL;
									
	IF (( hHandle == 0 ) , "handle can't be zero." );
	IF (( pCommandName == NULL ) , "CommandName can't be NULL." ); 
	IF (( pGetValue == NULL ) , "GetValue can't be NULL." ); 
	IF (( piValueLength == NULL ) , "ValueLength can't be NULL." );  
	IF (( piValueSize == NULL ) , "ValueSize can't be NULL." );  
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	memcpy( &tLocalStorage , pLocalStorage , sizeof(tsLocalStorage));
	CHK_CMT(CmtReleaseTSVPtr ( hHandle ));   
	
	STDF_UPDATE_CALLBACK_DATA(tLocalStorage.ptCallbacks);
	
	GetCurrentDateTime(&lfStartTime); 
	
	lfWaitForConnectionTimeout = (double)tLocalStorage.iNumberOfCommunicationTry * tLocalStorage.lfReceiveTimeOut;
		
	do
	{
		FREE_STDERR_COPY( Communication_GetPluggedInStatus( tLocalStorage.hCommunicationHandle , &bPluggedInStatus ));  

		if ( bPluggedInStatus == 0 )
			break;
		
		CHK_CMT( CmtReleaseTSVPtr ( hHandle ));
					
		DelayWithEventProcessing(0.2);
	
		CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
			
		GetCurrentDateTime(&lfCurrentTime);
			
	} while((lfCurrentTime-lfStartTime) < lfWaitForConnectionTimeout );

	IF (( bPluggedInStatus ) , "Communication is not established");
	
	for ( iCommandIndex = 0 ; iCommandIndex < tLocalStorage.listSize ; iCommandIndex++ )
		if ( strcmp( pCommandName , tLocalStorage.pSTD_CLI_CommandList[iCommandIndex].szCommandName ) == 0 )
		{
			bCommandFound = 1;
			break;
		}
		
	IF (( bCommandFound == 0 ) , "Command was not found.");   
		
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));  
	pLocalStorage->iLastSentCommandIndex = iCommandIndex;
	CHK_CMT(CmtReleaseTSVPtr ( hHandle )); 
	
	GetCurrentDateTime(&lfStartTime);
		
	for ( iTryCounter = 0 ; iTryCounter < tLocalStorage.iNumberOfCommunicationTry ; iTryCounter++ )
	{
		FREE_STDERR_COPY_ERR( STD_CLI_Driver_Send_Patern( tLocalStorage.hCommunicationHandle , tLocalStorage.pSTD_CLI_CommandList[iCommandIndex] , NULL , 0 , 0 )); 
		
		if ( IS_NOT_OK )
		{
			OutputDebugString("\t\tUUT Driver Error : Can't send command.");   
			DelayWithEventProcessing(5.0);
			sprintf( szLog , "\t\tUUT Driver       : Try to send command again. (%d/%d)" , iTryCounter+1 , tLocalStorage.iNumberOfCommunicationTry );
			OutputDebugString(szLog);  
			continue;
		}
		
		if (  IS_NOT_OK )
			continue;
		
		do
		{
			ProcessSystemEvents();
			
			FREE_STDERR_COPY( Communication_GetSendStatus ( tLocalStorage.hCommunicationHandle , &bSentSuccessfully ));
			
			if ( bSentSuccessfully )
				break;
			
			CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage )); 
		
			if ( pLocalStorage->pLastError )
			{
				UPDATERR( *(pLocalStorage->pLastError) ); 
			}
			
			CHK_CMT(CmtReleaseTSVPtr ( hHandle ));            
			
			if ( IS_NOT_OK )
				break;
			
			GetCurrentDateTime(&lfCurrentTime);
			
		} while((lfCurrentTime-lfStartTime) < tLocalStorage.lfSendTimeOut );
		
		if ((IS_OK) && bSentSuccessfully ) 
			break;
	}
	
	IF (( tLocalStorage.pSourceBuffer == NULL ) , "No responds" );
	
	if ( tLocalStorage.pSTD_CLI_CommandList[bCommandFound].iReceiveNumberOfParameters )
	{
		iValueCounter = 0;
		
		pNext = tLocalStorage.pSourceBuffer;
		
		/*
		CALLOC_ERR( tReceiveValue.tParametersList.pList , tLocalStorage.pSTD_CLI_CommandList[bCommandFound].iReceiveNumberOfParameters , sizeof(tsSimpleParameterItem));
		tReceiveValue.tParametersList.iNumberOfParameters = tLocalStorage.pSTD_CLI_CommandList[bCommandFound].iReceiveNumberOfParameters;
		
		for ( index = 0; index < tLocalStorage.pSTD_CLI_CommandList[bCommandFound].iReceiveNumberOfParameters; index++ )
		{
			FREE(pValue);
			
			pTemp = strstr( tLocalStorage.pSTD_CLI_CommandList[bCommandFound].tReceiveParameters[index].szStopPatern , "\n" );
			
			if ( pTemp ) 
			{
				*pTemp = '\n';
				pTemp++;
				*pTemp = 0;
			}
			
			pTemp = strstr( tLocalStorage.pSTD_CLI_CommandList[bCommandFound].tReceiveParameters[index].szStopPatern , "\r" );
			
			if ( pTemp ) 
			{
				*pTemp = '\r';
				pTemp++;
				*pTemp = 0;
			}
			
			pValue = String_Parsing ( pNext , tLocalStorage.pSTD_CLI_CommandList[bCommandFound].tReceiveParameters[index].szStartPatern , tLocalStorage.pSTD_CLI_CommandList[bCommandFound].tReceiveParameters[index].szStopPatern , &pNext ); 
			
			if ( pValue )
			{
				RemoveSurroundingWhiteSpace ( pValue ); 
				iValueCounter++;
				
				switch( tLocalStorage.pSTD_CLI_CommandList[bCommandFound].tReceiveParameters[index].iParametersType )
				{
					case RGGCOM_TYPE_INT:
					
						tReceiveValue.tParametersList.pList[index].type = CONVERT_TYPE_INT;
						
						if ( strchr( pValue , 'x' ) || strchr( pValue , 'X' ) )
							sscanf( pValue , "%x" , &tReceiveValue.tParametersList.pList[index].tValue.udWord );
						else
							tReceiveValue.tParametersList.pList[index].tValue.sdWord = atoi(pValue);
						
						break;
					case RGGCOM_TYPE_LONG:
						
						tReceiveValue.tParametersList.pList[index].type = CONVERT_TYPE_LONG;
						
						if ( strchr( pValue , 'x' ) || strchr( pValue , 'X' ) )
							sscanf( pValue , "%x" , &tReceiveValue.tParametersList.pList[index].tValue.uqWord );
						else
							tReceiveValue.tParametersList.pList[index].tValue.sqWord = atol(pValue);
						
						break;
						
					case RGGCOM_TYPE_DOUBLE:
						
						tReceiveValue.tParametersList.pList[index].type = CONVERT_TYPE_DOUBLE;
						sscanf( pValue , "%lf" , &tReceiveValue.tParametersList.pList[index].tValue.lfWord );
						
						break;
						
					case RGGCOM_TYPE_STRING:
						
						tReceiveValue.tParametersList.pList[index].type = CONVERT_TYPE_STRING;
						
						memcpy( tReceiveValue.tParametersList.pList[index].szString , pValue , 1024 );
						
						break;
						
					default:
						break;
				}
			}
		}
		
		*/
	}
	
	//IF (( iValueCounter != tLocalStorage.pSTD_CLI_CommandList[bCommandFound].iReceiveNumberOfParameters ) , "Wrong Responds" , MODULE_NAME );
	
	//sprintf( szLog , "CBS Unit %cs : %s" , '%' , ptValue->szParameter );
	
	//STDF_COMMENT(0,szLog,"N%d"); 

Error:


	RETURN_STDERR_POINTER;
}


void*   DLLEXPORT   STD_CLI_Driver_Unplug( int hHandle , double lfTimeOut )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	FREE_STDERR_COPY_ERR( Communication_Unplug( pLocalStorage->hCommunicationHandle , lfTimeOut )); 

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}


void*   DLLEXPORT   STD_CLI_Driver_PlugIn( int hHandle , double lfTimeOut )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	FREE_STDERR_COPY_ERR( Communication_PlugIn( pLocalStorage->hCommunicationHandle , lfTimeOut )); 

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

void*   DLLEXPORT   STD_CLI_Driver_EnablePlugInAutomatically( int hHandle , int bEnable )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	pLocalStorage->bEnableAutoPlugIn = bEnable;

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

void*   DLLEXPORT   STD_CLI_Driver_InstallReceiveCallbackFunction( int hHandle , tfStdReceiveCallback pfFunction , void *pData )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	pLocalStorage->pCallbackFunction = (void*) pfFunction;
	pLocalStorage->pCallbackData = pData;
	pLocalStorage->hCallbackFunctionThreadId = GetCurrentThreadId();
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}


void*   DLLEXPORT   STD_CLI_Driver_LoadConfigFile( int hHandle , char *pFileName )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			

	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};
	
	int							hFileHandle											=	0;
				
	int							iIndex												=	0,  
								iCount												=	0;
	
	char						szFormatedLog[LOW_STRING]							=	{0};

	int							iFileSize											=	0,
								iNumberOfItems										=	0;
	
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	IF (( pFileName == NULL ) , "File name is empty." );
	IF (( FileExists( pFileName , &iFileSize ) == 0 )  , "File was not found." );  
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));  

	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks); 
	
	sprintf( szFormatedLog , "%s :: Loading configuration %%s" ,  pLocalStorage->ptCallbacks->szName );
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	
	hFileHandle = OpenFile ( pFileName , VAL_READ_ONLY, VAL_TRUNCATE, VAL_BINARY);
	
	iNumberOfItems = ( iFileSize / sizeof(tsSTD_CLI_CommandItem) );
	
	FREE_CALLOC_ERR( pLocalStorage->pSTD_CLI_CommandList, iNumberOfItems+1 , sizeof(tsSTD_CLI_CommandItem));
	
	do
	{
		iCount = ReadFile ( hFileHandle , pLocalStorage->pSTD_CLI_CommandList[iIndex].max_size , sizeof(tsSTD_CLI_CommandItem) );
	
		if ( iCount != sizeof(tsSTD_CLI_CommandItem) )
			break;
		
		if ( pLocalStorage->pSTD_CLI_CommandList[iIndex].ulSignatureID != 0 ) 
				if ( pLocalStorage->pSTD_CLI_CommandList[iIndex].ulSignatureID != DEVICE_ID )
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

void*   DLLEXPORT   STD_CLI_Driver_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			

	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};

	int							iIndex												=	0,  
								iBaseSize											=	0, 
								iCount												=	0;

	char						szFormatedLog[LOW_STRING]							=	{0};

	int							iNumberOfItems										=	0;
	
	IF (( hHandle == 0 ) , "handle can't be zero." );
	IF (( pBuffer == NULL ) , "Buffer is empty." );
		
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks); 
	
	sprintf( szFormatedLog , "%s :: Loading configuration %%s" ,  pLocalStorage->ptCallbacks->szName );
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");

	iNumberOfItems = ( iBufferSize / sizeof(tsSTD_CLI_CommandItem) );
	
	FREE_CALLOC_ERR( pLocalStorage->pSTD_CLI_CommandList, iNumberOfItems+1 , sizeof(tsSTD_CLI_CommandItem));
	
	iCount = iBufferSize;
	iBaseSize = sizeof(tsSTD_CLI_CommandItem); 
	
	for ( iIndex = 0 ; iIndex < iNumberOfItems; iIndex++ )
	{
		if ( iCount < iBaseSize )   
			break;
		
		memcpy( pLocalStorage->pSTD_CLI_CommandList[iIndex].max_size , pBuffer , iBaseSize );
		
		if ( pLocalStorage->pSTD_CLI_CommandList[iIndex].ulSignatureID != 0 ) 
			if ( pLocalStorage->pSTD_CLI_CommandList[iIndex].ulSignatureID != DEVICE_ID )
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


void*   DLLEXPORT   STD_CLI_Driver_Check_Connection( int hHandle , char *pCommandName , int *piStatus )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	int							iCommandIndex										=	0,
								bCommandFound										=	0; 
									
	IF (( hHandle == 0 ) , "handle can't be zero." );
	IF (( piStatus == 0 ) , "Status parameter can't be NULL." );
	
	*piStatus = 0;
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	for ( iCommandIndex = 0 ; iCommandIndex < pLocalStorage->listSize ; iCommandIndex++ )
		if ((strlen(pLocalStorage->pSTD_CLI_CommandList[iCommandIndex].szCommandName)) && ( strcmp( pCommandName , pLocalStorage->pSTD_CLI_CommandList[iCommandIndex].szCommandName ) == 0 ))
		{
			bCommandFound = 1;
			break;
		}
		
	IF (( bCommandFound == 0 ) , "Command was not found.");   
		
	//*piStatus = 1;
		
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

void*	DLLEXPORT		STD_CLI_Driver_Get_Commands_List( int hHandle , char ***pCommandsList , int *piNumberOfCommands )     
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			

	int							iIndex												=	0;

	IF (( hHandle == 0 ) , "handle can't be zero." );
	IF (( pCommandsList == NULL ) , "Pointer to List is empty." );
		
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	CALLOC_ERR( (*pCommandsList) , pLocalStorage->listSize+1 , sizeof(char*));
	
	for ( iIndex = 0 ; iIndex < pLocalStorage->listSize; iIndex++ )
	{
		CALLOC_COPY_STRING_ERR( (*pCommandsList)[iIndex] , pLocalStorage->pSTD_CLI_CommandList[iIndex].max_size );
		
		if ( piNumberOfCommands )
			(*piNumberOfCommands)++;
	}

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}
