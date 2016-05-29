//==============================================================================
// Include files
#include "windows.h"
#include "cvidef.h"    
#include "toolbox.h"
#include <utility.h>
#include "GlobalDefines.h"
#include "StandardProtocolDriver_Protocol.h"
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

	unsigned char 				*pSourceBuffer;
	unsigned int				iSourceBufferLength;
	
	int							bDataUpdated,
								iNumberOfCommunicationTry;

	int							iLastSentCommandIndex;
	
	double						lfSendTimeOut,
								lfReceiveTimeOut;
								
	tsSTD_CallBackSet			*ptCallbacks;
	
	tsSTD_CommandItem			*pSTD_CommandList;
	int							listSize;

	int							bEnableAutoPlugIn;							
	
	int							bClosed;
	
	void						*pCallbackFunction,
								*pCallbackData;
	
	STD_ERROR					*pLastError; 
	
} tsLocalStorage;


//==============================================================================
// Static global variables

//==============================================================================
// Static functions
void*   DLLEXPORT       STD_Driver_Send_Patern( int hCommunicationHandle , tsSTD_CommandItem tCommandItem , unsigned char *pData , int iSize );

void	STD_Driver_ReceiveCallBack( void *pData );
//==============================================================================
// Global variables

//==============================================================================
// Global functions

void*	DLLEXPORT	STD_Driver_Config_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet )
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

void*	DLLEXPORT	STD_Driver_Config_LOG_SetAllowState( int iHandle , int bAllowLog ) 
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
void*	DLLEXPORT	STD_Driver_Config_Install_CommentCallback( CmtTSVHandle *pHandle , pfCommentCallback fFunction , void *pData , int type) 
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

void*	DLLEXPORT	STD_Driver_Config_Install_ConfigValueCallback( CmtTSVHandle *pHandle , pfConfigValueCallback fFunction , void *pData , int type) 
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

void*	DLLEXPORT	STD_Driver_Config_Install_CheckForBreakCallback( CmtTSVHandle *pHandle , pfCheckForBreakCallback fFunction , void *pData , int type) 
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
void   DLLEXPORT   STD_Driver_Close_CallBack ( int hCommHandle , int error , void *pData )
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
void   DLLEXPORT   STD_Driver_Error_CallBack ( int hCommHandle , void *pStdError , void *pData )
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
void	DLLEXPORT	STD_Driver_Receive_CallBack( unsigned char *pBuffer , unsigned int iBufferLen , void *pData )
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
		FREE_CALLOC_COPY( pLocalStorage->pSourceBuffer , iBufferLen + 100 , sizeof(char) , pBuffer , iBufferLen );   
		pLocalStorage->iSourceBufferLength = iBufferLen;
		
		pLocalStorage->iBufferLength = 0;
		GET_STDERR_CLEAR( pLocalStorage->pLastError );
	
		IF (( iBufferLen < MIN_MESSAGE_LEN ) , "Receive Wrong Packet");
		IF (( pBuffer[0] != pLocalStorage->pSTD_CommandList[iLastSentCommandIndex].responseHeader ) , "Receive Wrong Header");
	
		opCode = pBuffer[1]; 
		dataLength = pBuffer[2] + ( pBuffer[3] << 8 );
		
		IF (( iBufferLen < ( dataLength + MIN_MESSAGE_LEN )), "Receive Wrong Packet"); 
	
		for ( index = 0 ; index < (dataLength+MIN_MESSAGE_LEN-1) ; index++ )
			calcCheckSum += pBuffer[index];
	
		checkSum = pBuffer[index];
		
		IF (( checkSum != (calcCheckSum%256)) , "Receive Wrong CheckSum");
		
		FREE_CALLOC_ERR( pLocalStorage->pBuffer , dataLength + 100 , sizeof(char));
	
		for ( index = 0 ; index < dataLength ; index++ )  
			pLocalStorage->pBuffer[index] = pBuffer[index+MIN_MESSAGE_LEN-1];	
	
		pLocalStorage->iBufferLength = dataLength;
		
		iLastSentCommandIndex = pLocalStorage->iLastSentCommandIndex;
		
		if ( opCode != pLocalStorage->pSTD_CommandList[iLastSentCommandIndex].responseOpCode )
		{
			IF(( opCode == 0xF0 ) , "Header error");
			IF(( opCode == 0xF1 ) , "Command error"); 
			IF(( opCode == 0xF2 ) , "Checksum error"); 
			IF(( opCode == 0xF3 ) , "Data error"); 
			IF(( opCode == 0xF4 ) , "Execution Error"); 
			IF(( opCode == 0xF5 ) , "Time-out Error type 1"); 
			IF(( opCode == 0xF6 ) , "Time-out Error type 2");
			IF(( opCode == 0xF7 ) , "Time-out Error type 3"); 
		}
		
		if ( pLocalStorage->pCallbackFunction )
		{
			CALLOC_ERR( pHandle , 1 , sizeof(int));
			*pHandle = handle;
			pData = pHandle;
		
			PostDeferredCallToThread ( STD_Driver_ReceiveCallBack , pData , CmtGetCurrentThreadID() );  
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

void	STD_Driver_ReceiveCallBack( void *pData )
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
		
		CALLOC_COPY_STRING( pCommand , pLocalStorage->pSTD_CommandList[iLastSentCommandIndex].szCommandName );

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
int DLLEXPORT			STD_Driver_Set_Attributes_CallBack( ViSession hCommHandle , void *pData )
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


void*	DLLEXPORT		STD_Driver_Init( char *pszConnectionName , char *pszAddress , int *pHandle , ... )
{
	STD_ERROR						StdError									=	{0};
	
	tsSTD_CallBackSet				tSTD_CallBackSet							=	{0};

	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								handle										=	0;
	
	int								hCommunicationHandle						=	0;
	
	double							lfReceiveTimeOut							=	5.0,
									lfSendTimeOut								=	5.0; 
										
	int								iNumberOfReTry								=	3;
											 
	int								iLowLevel_ReadBlockSize						=	4096;
	
	int								bLowLevel_FlushReceiveBuffer				=	0,
									bLowLevel_DontWaitForSendStatus				=	1;
									
	double							lfLowLevelReceiveTimeout					=	0.1,
									lfLowLevelSendTimeout						=	0.1;
									
	int								*pShareHandle								=	NULL;
	
	unsigned int					index										=	0,
									listSize									=	0;
	
	char							szFormatedLog[STD_STRING]					=	{0};
	
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
		
	FREE_STDERR_COPY_ERR( Communication_InstallAttributeSetCallBack( hCommunicationHandle , (pfAttributeCallBack) STD_Driver_Set_Attributes_CallBack , (void *)pShareHandle ));
	
	FREE_STDERR_COPY_ERR( Communication_InstallReceiveCallBack( hCommunicationHandle , (pfReceiveCallBack)STD_Driver_Receive_CallBack , (void *)pShareHandle ));
	
	FREE_STDERR_COPY_ERR( Communication_InstallCloseCallBack( hCommunicationHandle , (pfCloseCallBack)STD_Driver_Close_CallBack , (void *)pShareHandle ));
	FREE_STDERR_COPY_ERR( Communication_InstallErrorCallBack( hCommunicationHandle , (pfErrorCallBack)STD_Driver_Error_CallBack , (void *)pShareHandle ));

	FREE_STDERR_COPY_ERR( Communication_SetPluggedInInitFailDelay( hCommunicationHandle , 1.0 )); //tConfig.lfInitFailWaitForDelay ));
		
	STDF_CONFIG_VALUE("LowLevel_ReadBlockSize", VAL_INTEGER , 1 , iLowLevel_ReadBlockSize , iLowLevel_ReadBlockSize );
	STDF_CONFIG_VALUE("LowLevel_FlushReceiveBuffer", VAL_INTEGER , 1 , bLowLevel_FlushReceiveBuffer , bLowLevel_FlushReceiveBuffer );
	STDF_CONFIG_VALUE("LowLevel_DontWaitForSendStatus", VAL_INTEGER , 1 , bLowLevel_DontWaitForSendStatus , bLowLevel_DontWaitForSendStatus );
	STDF_CONFIG_VALUE("LowLevel_ReceiveTimeOut", VAL_DOUBLE , 1 , lfLowLevelReceiveTimeout , lfLowLevelReceiveTimeout );
	STDF_CONFIG_VALUE("LowLevel_ReceiveTimeOut", VAL_DOUBLE , 1 , lfLowLevelSendTimeout , lfLowLevelSendTimeout );		
		
	FREE_STDERR_COPY_ERR( Communication_SetLowLevelConfig( hCommunicationHandle , iLowLevel_ReadBlockSize , lfLowLevelReceiveTimeout , lfLowLevelSendTimeout , bLowLevel_FlushReceiveBuffer , bLowLevel_DontWaitForSendStatus ));
	
	if ( pszConnectionName )
	{
		sprintf( szFormatedLog , "COM_%s.log" , pszConnectionName );
		FREE_STDERR_COPY_ERR( Communication_EnableLoging( hCommunicationHandle , 1 , szFormatedLog , 0 , 0 ));
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


void*	DLLEXPORT		STD_Driver_Close(int *pHandle)
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								handle										=	0;
	
	double							lfCurrentTime								=	0.0,
									lfStartTime									=	0.0;
	
	int								bClosed										=	0;
	
	char							szFormatedLog[LOW_STRING]					=	{0};

	tsSTD_CallBackSet				tSTD_CallBackSet							=	{0};
	
	int								hCommunicationHandle						=	0;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );
	
	handle = *pHandle;  

	IF (( handle == 0 ) , "No handle." );
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks); 
	
	sprintf( szFormatedLog , "%s :: Close communication %%s" ,  pLocalStorage->ptCallbacks->szName );
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	
	hCommunicationHandle = pLocalStorage->hCommunicationHandle;
	
	CHK_CMT( CmtReleaseTSVPtr ( handle ));
	
	FREE_STDERR_COPY_ERR( Communication_Close( hCommunicationHandle ));

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
		
		CmtGetTSVPtr ( handle , &pLocalStorage );
		
		FREE(pLocalStorage->pSTD_CommandList );
		
		FREE(pLocalStorage->pSourceBuffer);
			
		FREE(pLocalStorage->pBuffer);
		
		FREE(pLocalStorage->ptCallbacks);

		CmtReleaseTSVPtr ( handle );
	
		CmtDiscardTSV ( handle );

	}
	
	RETURN_STDERR_POINTER;
}


void*   DLLEXPORT       STD_Driver_GetConnectionAddress( int hHandle , char	**szAddress )
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



void*   DLLEXPORT       STD_Driver_Send_Patern( int hCommunicationHandle , tsSTD_CommandItem tCommandItem , unsigned char *pData , int iSize )
{
	STD_ERROR				StdError											=	{0};
	
	unsigned char			*pSendBuffer										=	NULL;
	
	unsigned int			iSendLenth											=	0,
							index												=	0,
							checkSumCalc										=	0;
	
	if ( iSize == 0 )
		iSize = tCommandItem.dataSize;
	
	CALLOC_ERR( pSendBuffer , (iSize + MIN_MESSAGE_LEN) , sizeof(char));
	
	checkSumCalc += pSendBuffer[iSendLenth++] = tCommandItem.sendHeader;
	checkSumCalc += pSendBuffer[iSendLenth++] = tCommandItem.opCode;
	checkSumCalc += pSendBuffer[iSendLenth++] = (char)( iSize & 0x0FF );
	checkSumCalc += pSendBuffer[iSendLenth++] = (char)( iSize >> 8 );
	
	if ( tCommandItem.iSendValue )
		pData = (char*)&(tCommandItem.iSendValue);
		
	for ( index = 0 ; index < iSize ; index++)
		checkSumCalc += pSendBuffer[iSendLenth++] = pData[index];

	pSendBuffer[iSendLenth++] = checkSumCalc % 256;
	
	FREE_STDERR_COPY_ERR( Communication_Send( hCommunicationHandle , pSendBuffer , iSendLenth , 0 ));         
	
Error:
	
	FREE(pSendBuffer);
	
	RETURN_STDERR_POINTER;
}

///--------------------- API ------------------------------//

void*   DLLEXPORT   STD_Driver_SetValue( int hHandle , char *pCommandName , void *pValue , int iValueLength , int iValueSize , ...  )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL,			
								tLocalStorage										=	{0};
	
	int							bCommandFound										=	0,
								iCommandIndex										=	0,
								iSize												=	0, 
								iDataLen											=	0;
	
	unsigned char 				*pData												=	NULL;
	
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
				
		DelayWithEventProcessing(0.2);
	
		GetCurrentDateTime(&lfCurrentTime);
			
	} while((lfCurrentTime-lfStartTime) < lfWaitForConnectionTimeout );

	IF (( bPluggedInStatus ) , "Communication is not established");
	
	for ( iCommandIndex = 0 ; iCommandIndex < tLocalStorage.listSize ; iCommandIndex++ )
		if ( strcmp( pCommandName , tLocalStorage.pSTD_CommandList[iCommandIndex].szCommandName ) == 0 )
		{
			bCommandFound = 1;
			break;
		}
		
	IF (( bCommandFound == 0 ) , "Command was not found.");   
		
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));  
	pLocalStorage->iLastSentCommandIndex = iCommandIndex;
	CHK_CMT(CmtReleaseTSVPtr ( hHandle )); 
	
	iDataLen = tLocalStorage.pSTD_CommandList[iCommandIndex].dataSize;
	iSize = (iValueSize * iValueLength);
	
	CALLOC_ERR( pData , ( iDataLen + iSize ) , sizeof(char));
	
	switch ( tLocalStorage.pSTD_CommandList[iCommandIndex].sendingSizeType )
	{
		case 0:
				// Constant Size 
				if (( pValue ) && ( iSize >= iDataLen ))
					memcpy( pData , pValue	, iDataLen ); 
				
				iSize = 0;
				
				break;
				
		case 1:
		default:
				if (( pValue ) && ( iSize ))
					memcpy( pData , pValue	, iSize );  
				
				break;
	
	}
	
	for ( iTryCounter = 0 ; iTryCounter < tLocalStorage.iNumberOfCommunicationTry ; iTryCounter++ )
	{
		FREE_STDERR_COPY_ERR( STD_Driver_Send_Patern( tLocalStorage.hCommunicationHandle , tLocalStorage.pSTD_CommandList[iCommandIndex] , pData , iSize )); 
	
		GetCurrentDateTime(&lfStartTime);
	
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
	
Error:

	FREE(pData);
	
	if ((IS_OK) && bSentSuccessfully ) 
		sprintf( szFormatedLog , "%s :: Sent command [%s] %%s" ,  tLocalStorage.ptCallbacks->szName , pCommandName );
	else
		sprintf( szFormatedLog , "%s :: Error sending command [%s] %%s" ,  tLocalStorage.ptCallbacks->szName , pCommandName );
	
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");  
	
	RETURN_STDERR_POINTER;
}



void*   DLLEXPORT   STD_Driver_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize, ...  )    
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL,			
								tLocalStorage										=	{0};			
																			
	int							bCommandFound										=	0,
								iCommandIndex										=	0,
								iDataLen											=	0;
	
	unsigned char 				*pData												=	NULL;
	
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
	
	iLastSentCommandIndex = tLocalStorage.iLastSentCommandIndex;
	
	if ( pGetCommandName )
	{
		FREE_CALLOC_COPY_STRING( *pGetCommandName , tLocalStorage.pSTD_CommandList[iLastSentCommandIndex].szCommandName );
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
		
		if ( tLocalStorage.pLastError && tLocalStorage.pLastError->error ) 
			break;
		
		GetCurrentDateTime(&lfCurrentTime);
		
	} while((lfCurrentTime-lfStartTime) < tLocalStorage.lfReceiveTimeOut );
	
	if ((lfCurrentTime-lfStartTime) >= tLocalStorage.lfReceiveTimeOut )
		OutputDebugString("\t\tUUT Driver Error : Receive timeout."); 
		
	if ( tLocalStorage.bDataUpdated )
	{
		if ( pGetValue )
		{
			FREE_CALLOC_COPY_ERR(((char**)*pGetValue) , (tLocalStorage.iBufferLength + 1) , 1 , tLocalStorage.pBuffer , tLocalStorage.iBufferLength );
		}
		
		if ( piValueLength )
			*piValueLength = tLocalStorage.iBufferLength;
		
		if ( piValueSize )
			*piValueSize = 1;
		
		CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage )); 
		pLocalStorage->bDataUpdated = 0;
		CHK_CMT(CmtReleaseTSVPtr ( hHandle ));   
		
		sprintf( szFormatedLog , "%s :: Has Received respond of command [%s] %%s" ,  tLocalStorage.ptCallbacks->szName , tLocalStorage.pSTD_CommandList[iLastSentCommandIndex].szCommandName );
	
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d");  
	
	}
	else
	{
	
		sprintf( szFormatedLog , "%s :: Hasn't Received respond of command [%s] %%s" ,  tLocalStorage.ptCallbacks->szName , tLocalStorage.pSTD_CommandList[iLastSentCommandIndex].szCommandName );
	
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d");  
	}
	
Error:
   /*
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
	   */
	RETURN_STDERR_POINTER;
}


void*   DLLEXPORT   STD_Driver_Unplug( int hHandle , double lfTimeOut )
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


void*   DLLEXPORT   STD_Driver_PlugIn( int hHandle , double lfTimeOut )
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

void*   DLLEXPORT   STD_Driver_EnablePlugInAutomatically( int hHandle , int bEnable )
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

void*   DLLEXPORT   STD_Driver_InstallReceiveCallbackFunction( int hHandle , tfStdReceiveCallback pfFunction , void *pData )
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


void*   DLLEXPORT   STD_Driver_LoadConfigFile( int hHandle , char *pFileName )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			

	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};
	
	int							hFileHandle											=	0;
				
	int							iIndex												=	0,  
								iCount												=	0;
	
	char						szFormatedLog[LOW_STRING]							=	{0};

	int							bGetSafeVariable									=	0;
	
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	IF (( pFileName == NULL ) , "File name is empty." );
	IF (( FileExists( pFileName , NULL ) == 0 )  , "File was not found." );  
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));  
	bGetSafeVariable = 1;
	
	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks); 
	
	sprintf( szFormatedLog , "%s :: Loading configuration %%s" ,  pLocalStorage->ptCallbacks->szName );
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	
	hFileHandle = OpenFile ( pFileName , VAL_READ_ONLY, VAL_TRUNCATE, VAL_BINARY);
	
	FREE_CALLOC_ERR( pLocalStorage->pSTD_CommandList , 256 , sizeof(tsSTD_CommandItem));
	
	for ( iIndex = 0 ; iIndex < 256; iIndex++ )
	{
		iCount = ReadFile ( hFileHandle , pLocalStorage->pSTD_CommandList[iIndex].max_size , sizeof(tsSTD_CommandItem) );
		
		if ( iCount != sizeof(tsSTD_CommandItem) )
			break;
	}
	
	pLocalStorage->listSize = iIndex;

Error:

	if ( hFileHandle )
		CloseFile (hFileHandle);
	
	if ( hHandle && bGetSafeVariable )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

void*   DLLEXPORT   STD_Driver_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			

	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};

	int							hFileHandle											=	0;
				
	int							iIndex												=	0,  
								iBaseSize											=	0, 
								iCount												=	0;

	char						szFormatedLog[LOW_STRING]							=	{0};
	
	IF (( hHandle == 0 ) , "handle can't be zero." );
	IF (( pBuffer == NULL ) , "Buffer is empty." );
		
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks); 
	
	sprintf( szFormatedLog , "%s :: Loading configuration %%s" ,  pLocalStorage->ptCallbacks->szName );
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
		
	FREE_CALLOC_ERR( pLocalStorage->pSTD_CommandList , 256 , sizeof(tsSTD_CommandItem));
	
	iCount = iBufferSize;
	iBaseSize = sizeof(tsSTD_CommandItem); 
	
	for ( iIndex = 0 ; iIndex < 256; iIndex++ )
	{
		if ( iCount < iBaseSize )   
			break;
		
		memcpy( pLocalStorage->pSTD_CommandList[iIndex].max_size , pBuffer , iBaseSize );
		pBuffer += iBaseSize;
		iCount -= iBaseSize; 
	}
	
	pLocalStorage->listSize = iIndex;

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}


void*   DLLEXPORT   STD_Driver_Check_Connection( int hHandle , char *pCommandName , int *piStatus )
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
	
	for ( iCommandIndex = 0 ; iCommandIndex < pLocalStorage->listSize ; iCommandIndex++ )
		if ((strlen(pLocalStorage->pSTD_CommandList[iCommandIndex].szCommandName)) && ( strcmp( pCommandName , pLocalStorage->pSTD_CommandList[iCommandIndex].szCommandName ) == 0 ))
		{
			bCommandFound = 1;
			break;
		}
		
	IF (( bCommandFound == 0 ) , "Command was not found.");   

Error:

	if ( piStatus )
	{
		*piStatus = bCommandFound;	
	}
	
	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

void*	DLLEXPORT		STD_Driver_Get_Commands_List( int hHandle , char ***pCommandsList , int *piNumberOfCommands )     
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			

	int							iIndex												=	0;

	int							iNumberOfCommands									=	0;
	
	IF (( hHandle == 0 ) , "handle can't be zero." );
	IF (( pCommandsList == NULL ) , "Pointer to List is empty." );
		
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	CALLOC_ERR( (*pCommandsList) , pLocalStorage->listSize+1 , sizeof(char*));
	
	for ( iIndex = 0 ; iIndex < pLocalStorage->listSize; iIndex++ )
	{
		CALLOC_COPY_STRING_ERR( (*pCommandsList)[iIndex] , pLocalStorage->pSTD_CommandList[iIndex].szCommandName );
		 
		iNumberOfCommands++;
	}

Error:

	if ( piNumberOfCommands )
		*piNumberOfCommands = iNumberOfCommands;
		
	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}
