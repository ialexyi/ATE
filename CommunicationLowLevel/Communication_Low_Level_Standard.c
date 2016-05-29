//==============================================================================
// Include files
#include "windows.h"
#include "cvidef.h"    
#include "toolbox.h"
#include <utility.h>
#include "GlobalDefines.h"
#include "Communication_Low_Level_Standard.h"
#include <formatio.h>


//==============================================================================
// Constants

#define				MIN_MESSAGE_LEN				5

//==============================================================================
// Types

typedef struct
{
	int							hCommunicationHandle;
	//------------- Communication ---------------//
	unsigned char 				*pBuffer;
	unsigned int				iBufferLength;

	int							bDataUpdated,
								iNumberOfCommunicationTry;

	int							iLastSentCommandIndex;
	
	double						lfSendTimeOut,
								lfReceiveTimeOut;
								
	tsSTD_CallBackSet			*ptCallbacks;
	
	int							bEnableAutoPlugIn;							
	
	int							bClosed,
								bFreed;
	
	void						*pCallbackFunction,
								*pCallbackData;
	
	STD_ERROR					*pLastError; 
	
} tsLocalStorage;


//==============================================================================
// Static global variables

//==============================================================================
// Static functions

void	COM_LL_STD_Driver_ReceiveCallBack( void *pData );

//==============================================================================
// Global variables

//==============================================================================
// Global functions

void*	DLLEXPORT	COM_LL_STD_Driver_Config_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet )
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

void*	DLLEXPORT	COM_LL_STD_Driver_Config_LOG_SetAllowState( int iHandle , int bAllowLog ) 
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
void*	DLLEXPORT	COM_LL_STD_Driver_Config_Install_CommentCallback( CmtTSVHandle *pHandle , pfCommentCallback fFunction , void *pData , int type) 
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

void*	DLLEXPORT	COM_LL_STD_Driver_Config_Install_ConfigValueCallback( CmtTSVHandle *pHandle , pfConfigValueCallback fFunction , void *pData , int type) 
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

void*	DLLEXPORT	COM_LL_STD_Driver_Config_Install_CheckForBreakCallback( CmtTSVHandle *pHandle , pfCheckForBreakCallback fFunction , void *pData , int type) 
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
void   DLLEXPORT   COM_LL_STD_Driver_Close_CallBack ( int hCommHandle , int error , void *pData )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								*pHandle									=	0,
									handle										=	0;
	
	int								bFreed										=	0;
	
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
	{
		bFreed = pLocalStorage->bFreed;
		CmtReleaseTSVPtr ( handle );
		
		if ( bFreed )
			CmtDiscardTSV ( handle ); 
	}
	
	return;
}

//typedef void (*pfErrorCallBack) ( unsigned long hCommHandle , void *pErrEv , void *data );  
void   DLLEXPORT   COM_LL_STD_Driver_Error_CallBack ( int hCommHandle , void *pStdError , void *pData )
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
void	DLLEXPORT	COM_LL_STD_Driver_Receive_CallBack( unsigned char *pBuffer , unsigned int iBufferLen , void *pData )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								index										=	0;
	
	unsigned char					checkSum									=	0,
									header										=	0, 
									opCode										=	0, 
									calcCheckSum								=	0;
	
	unsigned short					dataLength									=	0;
	
	int								*pHandle									=	0,
									handle										=	0;
	
	int								iLastSentCommandIndex						=	0;
	
	pHandle = pData;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );
	IF (( pBuffer == NULL ) , "Receive NULL pointer buffer." );   
	
	handle = *pHandle;
	
	if ( CmtGetTSVPtr ( handle , &pLocalStorage ) == 0 )
	{
		FREE_CALLOC_COPY( pLocalStorage->pBuffer , iBufferLen + 100 , sizeof(char) , pBuffer , iBufferLen );   
		pLocalStorage->iBufferLength = iBufferLen;
		
		if ( pLocalStorage->pCallbackFunction )
		{
			CALLOC_ERR( pHandle , 1 , sizeof(int));
			*pHandle = handle;
			pData = pHandle;
		
			PostDeferredCallToThread ( COM_LL_STD_Driver_ReceiveCallBack , pData , CmtGetCurrentThreadID() );  
		}
	}
	else
		handle = 0;
		
Error:
	
	if ( handle )
	{
		pLocalStorage->bDataUpdated = 1;
		GET_STDERR_CLEAR( pLocalStorage->pLastError );
		CmtReleaseTSVPtr ( handle );
	}
	
	FREE(pBuffer);
	
	return;
}

void	COM_LL_STD_Driver_ReceiveCallBack( void *pData )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								index										=	0;
	
	unsigned char					checkSum									=	0,
									header										=	0, 
									opCode										=	0, 
									calcCheckSum								=	0;
	
	unsigned short					dataLength									=	0;
	
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
		
		CALLOC_COPY_ERR( pValue , (pLocalStorage->iBufferLength + 1) , 1 , pLocalStorage->pBuffer , pLocalStorage->iBufferLength );
		
		iValueSize = pLocalStorage->iBufferLength;
		
		receiveFunction = pLocalStorage->pCallbackFunction;
		
		pData = pLocalStorage->pCallbackData;
		
		CmtReleaseTSVPtr ( handle ); 
	}	
		
	if ( receiveFunction )
		receiveFunction( pData , pCommand , pValue , iValueSize , pValue , iValueSize ); 
Error:
	
	return;
}

//typedef int (*pfAttributeCallBack) ( unsigned long hCommHandle , void *data ); 
int DLLEXPORT			COM_LL_STD_Driver_Set_Attributes_CallBack( ViSession hCommHandle , void *pData )
{
	STD_ERROR						StdError									=	{0};
	
	tsSTD_CallBackSet				tSTD_CallBackSet							=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	ViUInt32						iDB_BaudRate							  	=	0,
									iDB_DataBits							  	=	0,
									iDB_Parity								  	=	0,
									iDB_StopBits							  	=	0,
									iDB_FlowCtrl							  	=	0,
									iDB_Default								  	=	0;

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


void*	DLLEXPORT		COM_LL_STD_Driver_Init( char *pszConnectionName , char *pszAddress , int *pHandle , ... )
{
	STD_ERROR						StdError									=	{0};
	
	tsSTD_CallBackSet				tSTD_CallBackSet							=	{0};

	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								handle										=	0;
	
	int								hCommunicationHandle						=	0;
	
	double							lfReceiveTimeOut							=	5.0,
									lfSendTimeOut								=	5.0; 
										
	int								iNumberOfReTry								=	3;
										
	int								bDB_Ascii									=	0;
	
	int								*pShareHandle								=	NULL;
	
	unsigned int					index										=	0,
									listSize									=	0;
	
	char							szFormatedLog[STD_STRING]					=	{0};
	
	double							lfTypingModeDelay							=	0;
	
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
	
	//listSize = GetCommandItemListSize(); 
		
	//CALLOC_ERR( pLocalStorage->pSTD_CommandList , listSize , sizeof(tsSTD_CommandItem));
	
	//IF (( pLocalStorage->pSTD_CommandList == NULL ) , "Can't allocate memory." ); 
	
	//for ( index = 0 ; index < ; index++ , listSize++ )
	//	memcpy( &(pLocalStorage->pSTD_CommandList[index]) , &(tSTD_CommandList[index]) , sizeof(tsSTD_CommandItem) ); 	
	
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
		
	FREE_STDERR_COPY_ERR( Communication_InstallAttributeSetCallBack( hCommunicationHandle , (pfAttributeCallBack) COM_LL_STD_Driver_Set_Attributes_CallBack , (void *)pShareHandle ));
	
	FREE_STDERR_COPY_ERR( Communication_InstallReceiveCallBack( hCommunicationHandle , (pfReceiveCallBack)COM_LL_STD_Driver_Receive_CallBack , (void *)pShareHandle ));
	
	FREE_STDERR_COPY_ERR( Communication_InstallCloseCallBack( hCommunicationHandle , (pfCloseCallBack)COM_LL_STD_Driver_Close_CallBack , (void *)pShareHandle ));
	FREE_STDERR_COPY_ERR( Communication_InstallErrorCallBack( hCommunicationHandle , (pfErrorCallBack)COM_LL_STD_Driver_Error_CallBack , (void *)pShareHandle ));

	FREE_STDERR_COPY_ERR( Communication_SetPluggedInInitFailDelay( hCommunicationHandle , 1.0 )); //tConfig.lfInitFailWaitForDelay ));
		
	if ( pszConnectionName )
	{
		STDF_CONFIG_VALUE("Ascii", VAL_INTEGER , 1 , bDB_Ascii , bDB_Ascii ); 
		
		sprintf( szFormatedLog , "COM_%s.log" , pszConnectionName );
		FREE_STDERR_COPY_ERR( Communication_EnableLoging( hCommunicationHandle , 1 , szFormatedLog , bDB_Ascii , 0 ));
	}
	
	FREE_STDERR_COPY_ERR( Communication_EnableReceive( pLocalStorage->hCommunicationHandle , 1 )); 	
		
	FREE_STDERR_COPY_ERR( Communication_FlushReceiveBuffer( pLocalStorage->hCommunicationHandle , 1 )); 
	
	STDF_CONFIG_VALUE("TypingModeDelay", VAL_DOUBLE , 1 , lfTypingModeDelay , lfTypingModeDelay ); 
	
	if ( lfTypingModeDelay > 0.0 )
	{
		FREE_STDERR_COPY_ERR( Communication_SetTypingMode( pLocalStorage->hCommunicationHandle , 1 , lfTypingModeDelay ));
	}
	
	pLocalStorage->bEnableAutoPlugIn = 1; 
	
	ProcessSystemEvents();
	
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


void*	DLLEXPORT		COM_LL_STD_Driver_Close(int *pHandle)
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

	CHK_CMT( CmtReleaseTSVPtr ( handle )); 
	
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
	
	if ( handle && ( CmtGetTSVPtr ( handle , &pLocalStorage ) == 0 ))
	{
		*pHandle = 0;
		
		FREE(pLocalStorage->pBuffer);
		
		FREE(pLocalStorage->ptCallbacks);

		bClosed = pLocalStorage->bClosed;
		
		pLocalStorage->bFreed = 1;
		
		CmtReleaseTSVPtr ( handle );
	
		if ( bClosed )
			CmtDiscardTSV ( handle );
	}
	
	RETURN_STDERR_POINTER;
}


void*   DLLEXPORT       COM_LL_STD_Driver_GetConnectionAddress( int hHandle , char	**szAddress )
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

///--------------------- API ------------------------------//

void*   DLLEXPORT   COM_LL_STD_Driver_SetValue( int hHandle , char *pCommandName , void *pValue , int iValueLength , int iValueSize , ...  )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL,			
								tLocalStorage										=	{0};
	
	int							bCommandFound										=	0,
								iCommandIndex										=	0,
								iDataLen											=	0;
	
	unsigned char 				*pCommandBuffer										=	NULL,
								*pSourceCommandName									=	NULL; 
								
	char						szCommand[1024]										=	{0},
								szFormatedLog[LOW_STRING]							=	{0};
	
	double						lfStartTime											=	0.0,
								lfCurrentTime										=	0.0;
	
	int							iTryCounter											=	0;
	
	char						szLog[1024]											=	{0};
	
	int							bPluggedInStatus									=	0,  
								bSentSuccessfully									=	0; 
									
	double						lfWaitForConnectionTimeout							=	0.0;
	
	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};

	tuConvert					tConvert;
		
	int							iIndex												=	0;
	
	int							iCommandBufferLength								=	0,
								iStringLength										=	0; 
								
	unsigned char				ucCharacter											=	0;
	
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
		
		ProcessSystemEvents(); 
		
		GetCurrentDateTime(&lfCurrentTime);
			
	} while((lfCurrentTime-lfStartTime) < lfWaitForConnectionTimeout );

	IF (( bPluggedInStatus ) , "Communication is not established");
	
	//STDF_UPDATE_CALLBACK_DATA(tLocalStorage.tCallbacks); 
	
	if ( pCommandName && strlen(pCommandName))
	{
		CALLOC_COPY_STRING_ERR( pSourceCommandName , pCommandName );
		
		iStringLength = strlen(pSourceCommandName);
		
		CALLOC_ERR( pCommandBuffer , iStringLength , sizeof(char));
		
		for ( iIndex = 0; iIndex < iStringLength; iIndex++ )
		{
			do
			{
				ucCharacter = pSourceCommandName[iIndex];
				
				if ( pSourceCommandName[iIndex] == '\\' )
				{
					iIndex++;
				
					if ( pSourceCommandName[iIndex] == '\\')
						break;
				
					if (( isalpha (pSourceCommandName[iIndex]) ) || ( pSourceCommandName[iIndex] == '0' ))
					{
						switch(pSourceCommandName[iIndex])
						{
							case '0':
									ucCharacter = 0;
									break;
									
							case 'a':
									ucCharacter = 7;
									break;
								
							case 'b':
									ucCharacter = 8;
									break;
									
							case 't':
									ucCharacter = 9;
									break;
									
							case 'n':
									ucCharacter = 10;
									break;
									
							case 'v':
									ucCharacter = 11;
									break;
									
							case 'f':
									ucCharacter = 12;
									break;
									
							case 'r':
									ucCharacter = 13;
									break;
								
							case 'e':
									ucCharacter = 27;
									break;
									
							default:
									break;		
						}
						
						break;
					}
					else
					{
						ucCharacter = atoi(&(pSourceCommandName[iIndex]));
						
						while(isalnum(pSourceCommandName[++iIndex]));
						
						iIndex--;
						
						break;
					}
				}
				
			}while(0);
			
			pCommandBuffer[iCommandBufferLength++] = ucCharacter;
		}
		
	}
	
	GetCurrentDateTime(&lfStartTime);
	
	for ( iTryCounter = 0 ; iTryCounter < tLocalStorage.iNumberOfCommunicationTry ; iTryCounter++ )
	{
		if ( iCommandBufferLength && pCommandBuffer )
		{
			FREE_STDERR_COPY_ERR( Communication_Send( tLocalStorage.hCommunicationHandle , pCommandBuffer , iCommandBufferLength , -1 ));         
		}
																																				   
		if ( pValue && iValueLength && iValueSize )
			do
			{
				if (( iValueSize == 4 ) && (*((int*)pValue) == 0 ))
					break;
				
				if (( iValueSize == 8 ) && (*((double*)pValue) == 0.0 ))
					break;
				
				if (( iValueSize == 1 ) && (*((char*)pValue) == 0 ))
					break;
				
				FREE_STDERR_COPY_ERR( Communication_Send( tLocalStorage.hCommunicationHandle , pValue , ( iValueLength * iValueSize ) , -1 ));         
				
				
			} while(0);
		
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
			
			if( CmtGetTSVPtr ( hHandle , &pLocalStorage ) == 0 )
			{
				if ( pLocalStorage->pLastError )
				{
					UPDATERR( *(pLocalStorage->pLastError) ); 
				}
			
				CmtReleaseTSVPtr ( hHandle );            
			}
			
			if ( IS_NOT_OK )
				break;
			
			GetCurrentDateTime(&lfCurrentTime);
			
		} while((lfCurrentTime-lfStartTime) < tLocalStorage.lfSendTimeOut );
		
		if ((IS_OK) && bSentSuccessfully ) 
			break;
	}
	
Error:

	FREE(pSourceCommandName);
	
	if ((IS_OK) && bSentSuccessfully ) 
		sprintf( szFormatedLog , "The Command was Sent Successfully.%%s" );
	else
		sprintf( szFormatedLog , "An error occurred when sending command.%%s" );
	
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");  
	
	RETURN_STDERR_POINTER;
}



void*   DLLEXPORT   COM_LL_STD_Driver_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize, ...  )    
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL,			
								tLocalStorage										=	{0};			
																			
	int							bCommandFound										=	0,
								iCommandIndex										=	0,
								iDataLen											=	0;
	
	char						szCommand[1024]										=	{0},
								szFormatedLog[LOW_STRING]							=	{0};
								
	double						lfStartTime											=	0.0,
								lfCurrentTime										=	0.0;
	
	int							iTryCounter											=	0;
	
	char						szLog[1024]											=	{0};
	
	int							bPluggedInStatus									=	0;  
	
	double						lfWaitForConnectionTimeout							=	0.0;
	
	tuConvert					tConvert;
		
	int							iLastSentCommandIndex								=	0;
	
	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};

	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	memcpy( &tLocalStorage , pLocalStorage , sizeof(tsLocalStorage));
	CHK_CMT(CmtReleaseTSVPtr ( hHandle ));   
	
	STDF_UPDATE_CALLBACK_DATA(tLocalStorage.ptCallbacks);
	
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
			FREE_CALLOC_COPY_ERR( *((char**)pGetValue) , (tLocalStorage.iBufferLength + 1) , sizeof(char) , tLocalStorage.pBuffer , tLocalStorage.iBufferLength );
		}
		
		if ( piValueLength )
			*piValueLength = tLocalStorage.iBufferLength;
		
		if ( piValueSize )
			*piValueSize = 1;
		
		CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage )); 
		pLocalStorage->bDataUpdated = 0;
		CHK_CMT(CmtReleaseTSVPtr ( hHandle ));   
		
		sprintf( szFormatedLog , "Has Received respond of command.%%s" );
	
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d");  
	
	}
	else
	{
	
		sprintf( szFormatedLog , "Hasn't Received respond of command.%%s" );
	
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


void*   DLLEXPORT   COM_LL_STD_Driver_GetValue_Ex( int hHandle , char *pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize, ...  )    
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL,			
								tLocalStorage										=	{0};			
																			
	int							bCommandFound										=	0,
								iCommandIndex										=	0,
								iDataLen											=	0;
	
	char						szCommand[1024]										=	{0},
								szFormatedLog[LOW_STRING]							=	{0};
								
	double						lfStartTime											=	0.0,
								lfCurrentTime										=	0.0;
	
	int							iTryCounter											=	0;
	
	char						szLog[1024]											=	{0};
	
	int							bPluggedInStatus									=	0;  
	
	double						lfWaitForConnectionTimeout							=	0.0;
	
	tuConvert					tConvert;
		
	int							iLastSentCommandIndex								=	0;
	
	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};

	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	memcpy( &tLocalStorage , pLocalStorage , sizeof(tsLocalStorage));
	CHK_CMT(CmtReleaseTSVPtr ( hHandle ));   
	
	STDF_UPDATE_CALLBACK_DATA(tLocalStorage.ptCallbacks);
	
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
			FREE_CALLOC_COPY_ERR( *((char**)pGetValue) , (tLocalStorage.iBufferLength + 1) , sizeof(char) , tLocalStorage.pBuffer , tLocalStorage.iBufferLength );
		}
		
		if ( piValueLength )
			*piValueLength = tLocalStorage.iBufferLength;
		
		if ( piValueSize )
			*piValueSize = 1;
		
		CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage )); 
		pLocalStorage->bDataUpdated = 0;
		CHK_CMT(CmtReleaseTSVPtr ( hHandle ));   
		
		sprintf( szFormatedLog , "Has Received respond of command.%%s" );
	
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d");  
	
	}
	else
	{
	
		sprintf( szFormatedLog , "Hasn't Received respond of command.%%s" );
	
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

void*   DLLEXPORT   COM_LL_STD_Driver_Unplug( int hHandle , double lfTimeOut )
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


void*   DLLEXPORT   COM_LL_STD_Driver_PlugIn( int hHandle , double lfTimeOut )
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

void*   DLLEXPORT   COM_LL_STD_Driver_EnablePlugInAutomatically( int hHandle , int bEnable )
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

void*   DLLEXPORT   COM_LL_STD_Driver_InstallReceiveCallbackFunction( int hHandle , tfStdReceiveCallback pfFunction , void *pData )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	pLocalStorage->pCallbackFunction = (void*) pfFunction;
	pLocalStorage->pCallbackData = pData;

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}


void*   DLLEXPORT   COM_LL_STD_Driver_LoadConfigFile( int hHandle , char *pFileName )
{
	STD_ERROR					StdError											=	{0};

	RETURN_STDERR_POINTER;
}

void*   DLLEXPORT   COM_LL_STD_Driver_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize )
{
	STD_ERROR					StdError											=	{0};

	RETURN_STDERR_POINTER;
}


void*   DLLEXPORT   COM_LL_STD_Driver_Check_Connection( int hHandle , char *pCommandName , int *piStatus )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	int							iCommandIndex										=	0,
								bCommandFound										=	0,
								iNumberOfPorts										=	0; 
									
	IF (( hHandle == 0 ) , "handle can't be zero." );
	IF (( piStatus == 0 ) , "Status parameter can't be NULL." );
	
	*piStatus = 0;
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	IF (( pLocalStorage->hCommunicationHandle == 0 ) , "Connection is not established.");   
		
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

void*	DLLEXPORT		COM_LL_STD_Driver_Get_Commands_List( int hHandle , char ***pCommandsList , int *piNumberOfCommands )     
{
	STD_ERROR					StdError											=	{0};

	RETURN_STDERR_POINTER;
}

