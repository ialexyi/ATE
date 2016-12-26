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
		int				iParametersType;								// 1 - int , 2 - long , 3 - double , 4 - string , 5 - Binary       
		
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
		int				iParametersType;								// 1 - int , 2 - long , 3 - double , 4 - string  , 5 - Binary     

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
	
	int							viOpenDefaultRM;
	
	int							hFileHandle;
	
	//------------- Communication ---------------//
	unsigned char 				*pBuffer;
	unsigned int				iBufferLength;

	unsigned char 				*pSourceBuffer;
	unsigned int				iSourceBufferLength;
	
	char						*pszAddress;
	
	int							bDataUpdated,
								bDataUpdateFailed,
								iNumberOfCommunicationTry;

	int							iLastSentCommandIndex;
	
	double						lfLowLevelSendTimeOut,
								lfLowLevelReceiveTimeOut;
	
	int 						iReceiveLowLevelTryNumber;
	
	double						lfDelaySendingCommands;	
	
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
	
	char						*pszTerminatingPattern;
	int                         iTerminatingPatternLength;
	
	STD_ERROR					*pLastError; 
	
} tsLocalStorage;


//==============================================================================
// Static global variables

//==============================================================================
// Static functions
void*   DLLEXPORT       STD_CLI_Driver_Send_Patern( int hCommunicationHandle , int hFileHandle, tsSTD_CLI_CommandItem tCommandItem , void *pValue , int iValueLength , int iValueSize , double lfDelaySendingCommands ,  *pbSentStatus);

int		Send( int hCommunicationHandle , int hFileHandle , char *pBuffer , int iCount , double lfDelaySendingCommands ,  *pbSentSuccessfully);

char*		Receive( int hCommunicationHandle , int iTryNumber , int hFileHandle , int iExactSize , int *piCount );

void	STD_CLI_Driver_ReceiveCallBack( void *pData );

void* DLLEXPORT	SetTerminatingPattern( int hHandle , char *pszTerminatingPattern , int iLength );
//==============================================================================
// Global variables

//==============================================================================
// Global functions

void*	DLLEXPORT	STD_CLI_GetLowLevelHandle( int hHandle , int *phLowLevelHandle ) 
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));

	*phLowLevelHandle = pLocalStorage->hCommunicationHandle;
	
Error:
	
	if ( hHandle )
		CmtReleaseTSVPtr (hHandle);
	
	RETURN_STDERR_POINTER;
}

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
			//		FREE_STDERR_COPY_ERR( Communication_Unplug( pLocalStorage->hCommunicationHandle , 10.0 ));					//by Max
					
					if ( pLocalStorage->bEnableAutoPlugIn )
					{
						OutputDebugString("\t\tUUT Driver       : Plugging back communication.");   
				//		FREE_STDERR_COPY_ERR( Communication_PlugIn( pLocalStorage->hCommunicationHandle , 10.0 )); 			//by Max
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

/***********************************************************************************
 *(CLI Driver) Driver_Receive                                                      *
 ***********************************************************************************/

void	DLLEXPORT	STD_CLI_Driver_Receive_CallBack( int handle )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								iLastSentCommandIndex						=	0,
									iReceiveMinimumLength						=	0;
	
	unsigned char					*pBuffer									=	NULL;
	
	int								*pHandle									=	0;
	
	int								iBufferLen									=	0;
	
	int								bDataUpdateFailed							=	0;
	
	char							*pReceiveCheck								=	NULL,
									*pTemp										=	NULL;
	
	void							*pData 										=	NULL;
	
	char							*pExpectedRespond							=	NULL,
									*pStatus									=	NULL;
	
	
	IF (( handle == NULL ) , "Pointer to handle can't be NULL." );
	
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
			
			//receive
			pBuffer = Receive( pLocalStorage->hCommunicationHandle , pLocalStorage->iReceiveLowLevelTryNumber , pLocalStorage->hFileHandle , iReceiveMinimumLength ,  &iBufferLen ); 
	
			pReceiveCheck = StringReplaceAll( pBuffer  , "\n" , "\\n" , NULL );
	
			pTemp = pReceiveCheck;
		
			pReceiveCheck = StringReplaceAll( pTemp , "\r" , "\\r" , NULL ); 
			
			//CALLOC_COPY_STRING( pExpectedRespond , pLocalStorage->pSTD_CLI_CommandList[iLastSentCommandIndex].szRespondsFormated ); 	   //added by max
			//pStatus = strstr(pReceiveCheck, pExpectedRespond);																					//added by max
	
			//compare expected command with receive command
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
		FREE(pLocalStorage->pBuffer);
		pLocalStorage->iBufferLength = iBufferLen; 
		pLocalStorage->pBuffer = pBuffer; 
		pLocalStorage->bDataUpdated = 1;
		pLocalStorage->bDataUpdateFailed = bDataUpdateFailed;
		GET_STDERR_CLEAR( pLocalStorage->pLastError );
		CmtReleaseTSVPtr ( handle );
	}
	
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



/***********************************************************************************
 *(CLI Driver) INIT                                                                *
 ***********************************************************************************/
void*	DLLEXPORT		STD_CLI_Driver_Init( char *pszConnectionName , char *pszAddress , int *pHandle , ... )
{
	STD_ERROR						StdError									=	{0};
	
	tsSTD_CallBackSet				tSTD_CallBackSet							=	{0};

	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								handle										=	0;
	
	int								hCommunicationHandle						=	0;
	
	double							lfLowLevelReceiveTimeOut					=	0.3,   //0.05		 
									lfLowLevelSendTimeOut						=	0.1;   //0.1  
	
	double							lfReceiveTimeOut							=	5.0,
									lfSendTimeOut								=	5.0;
	
	double							lfDelaySendingCommands						=	0.05;
	
	int								iLowLevelTimeout							=	0.2,
									iReceiveLowLevelTryNumber					=	2;
										
	int								iNumberOfReTry								=	3;
											 
	char							szFormatedLog[STD_STRING]					=	{0},
									szLogFormated[STD_STRING]					=	{0},  
									szLogFileName[STD_STRING]					=	{0};
	
	ViUInt32						iDB_BaudRate							  	=	115200,
									iDB_DataBits							  	=	8,
									iDB_Parity								  	=	0,
									iDB_StopBits							  	=	10,
									iDB_FlowCtrl							  	=	0;
	
	double							lfLogCurrentTime							=	0.0; 
	
	
	IF ((( pHandle == NULL ) || ( pszAddress == NULL )  || ( pszConnectionName == NULL )), "Pointer to handle can't be NULL." );
	
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
		
	CALLOC_COPY_STRING(pLocalStorage->pszAddress , pszAddress );  //save address on Local storage 
	
	pLocalStorage->ptCallbacks->iInternalHandle = handle;
	
	strcpy( pLocalStorage->ptCallbacks->szName , pszConnectionName );
	
	pLocalStorage->iLastSentCommandIndex = -1;
	
	STDF_SET_MODULE_NAME(pLocalStorage->ptCallbacks,pszConnectionName);			   //what this
	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks);						   //what this  
	
	if ( pszConnectionName )
	{
		sprintf( szFormatedLog , "Initializing \"%s\" Driver %%s" ,  pszConnectionName );
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	}
	

	STDF_CONFIG_VALUE("LowLevelReceiveTimeOut", VAL_DOUBLE , 1 , lfLowLevelReceiveTimeOut , lfLowLevelReceiveTimeOut );
	STDF_CONFIG_VALUE("LowLevelSendTimeOut", VAL_DOUBLE , 1 , lfLowLevelSendTimeOut ,lfLowLevelSendTimeOut );   
	STDF_CONFIG_VALUE("ReceiveTimeOut", VAL_DOUBLE , 1 , lfReceiveTimeOut , lfReceiveTimeOut );
	STDF_CONFIG_VALUE("SendTimeOut", VAL_DOUBLE , 1 , lfSendTimeOut , lfSendTimeOut );   
	STDF_CONFIG_VALUE("NumberOfReTry", VAL_INTEGER , 1 , iNumberOfReTry , iNumberOfReTry );
	STDF_CONFIG_VALUE("DelaySendingCommands", VAL_DOUBLE , 1 , lfDelaySendingCommands , lfDelaySendingCommands );
	STDF_CONFIG_VALUE("ReceiveLowLevelTryNumber", VAL_INTEGER , 1 , iReceiveLowLevelTryNumber , iReceiveLowLevelTryNumber );
	
	pLocalStorage->iReceiveLowLevelTryNumber = iReceiveLowLevelTryNumber;  
	pLocalStorage->iNumberOfCommunicationTry = iNumberOfReTry;
	pLocalStorage->lfLowLevelReceiveTimeOut = lfLowLevelReceiveTimeOut;
	pLocalStorage->lfLowLevelSendTimeOut = lfLowLevelSendTimeOut;
	pLocalStorage->lfDelaySendingCommands = lfDelaySendingCommands;
	pLocalStorage->lfReceiveTimeOut = lfReceiveTimeOut;
	pLocalStorage->lfSendTimeOut = lfSendTimeOut;
	
	CHK_VSA( viOpenDefaultRM (&pLocalStorage->viOpenDefaultRM));
	CHK_VSA( viOpen ( pLocalStorage->viOpenDefaultRM , pszAddress , VI_NULL, VI_NULL, &hCommunicationHandle ));
	
	pLocalStorage->hCommunicationHandle = hCommunicationHandle;
		
	IF (( STD_CLI_Driver_Set_Attributes_CallBack( hCommunicationHandle , pHandle )) , "Failed to configurate attributes" );
	
	iLowLevelTimeout = lfLowLevelReceiveTimeOut * 1E3;
	
	if ( iLowLevelTimeout > 0 )
	{
		CHK_VSA( viSetAttribute ( hCommunicationHandle , VI_ATTR_TMO_VALUE , iLowLevelTimeout )); 
	}
	
	
	
	if ( pszConnectionName )
	{
		sprintf( szLogFileName , "COM_%s.log" , pszConnectionName );
	}
	else
	{
		if ( pszAddress )
			sprintf( szLogFileName , "COM_%s.log" , pszAddress );
	}
	
	pLocalStorage->hFileHandle = OpenFile ( szLogFileName , VAL_READ_WRITE, VAL_TRUNCATE , VAL_BINARY);
	
	GetCurrentDateTime(&lfLogCurrentTime);
	
	FormatDateTimeString ( lfLogCurrentTime , "\r\n%H:%M:%S.%3f    Open        :    \r\n" , szLogFormated , 1024 );
	
	if ( pLocalStorage->hFileHandle )
		WriteFile ( pLocalStorage->hFileHandle , szLogFormated , strlen(szLogFormated) ); 
				
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



/***********************************************************************************
 * (CLI Driver) Close                                                              *
 ***********************************************************************************/

void*	DLLEXPORT		STD_CLI_Driver_Close(int *pHandle)
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								handle										=	0;
	
	char							szFormatedLog[LOW_STRING]					=	{0},
									szLogFormated[STD_STRING]					=	{0}; 
	
	double							lfLogCurrentTime							=	0.0; 

	tsSTD_CallBackSet				tSTD_CallBackSet							=	{0};
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );
	
	handle = *pHandle;  

	IF (( handle == 0 ) , "No handle." );
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks); 
	
	sprintf( szFormatedLog , "%s :: Close communication %%s" ,  pLocalStorage->ptCallbacks->szName );
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	
	if ( pLocalStorage->hCommunicationHandle )
		viClose ( pLocalStorage->hCommunicationHandle );

	CHK_CMT( CmtReleaseTSVPtr ( handle ));
	
Error:
	
	if ( handle && ( CmtGetTSVPtr ( handle , &pLocalStorage ) == 0 ))
	{
		*pHandle = 0;
		
		FREE(pLocalStorage->pszAddress);
		
		FREE(pLocalStorage->pSTD_CLI_CommandList);
		
		FREE(pLocalStorage->pSourceBuffer);
			
		FREE(pLocalStorage->pBuffer);
		
		FREE(pLocalStorage->ptCallbacks);
		
		if ( pLocalStorage->hFileHandle )   
		{
			GetCurrentDateTime(&lfLogCurrentTime);
			FormatDateTimeString ( lfLogCurrentTime , "\r\n\r\n%H:%M:%S.%3f    Close.\r\n\r\n" , szLogFormated , 1023 );
		
			WriteFile ( pLocalStorage->hFileHandle , szLogFormated , strlen(szLogFormated) );

			CloseFile( pLocalStorage->hFileHandle ); 
		}
		
		CmtReleaseTSVPtr ( handle );	    
	
		CmtDiscardTSV ( handle );
	}
	
	RETURN_STDERR_POINTER;
}



/***********************************************************************************
 * (CLI Driver) GetConnectionAddress                                               *
 ***********************************************************************************/

void*   DLLEXPORT       STD_CLI_Driver_GetConnectionAddress( int hHandle , char	**szAddress )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	IF (( hHandle == 0 ) , "Pointer to handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
//	FREE_STDERR_COPY_ERR( Communication_GetConnectionAddress( pLocalStorage->hCommunicationHandle , szAddress ));
	
Error:
	
	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	RETURN_STDERR_POINTER;
}


/***********************************************************************************
 *(CLI Driver) Send_Patern                                                         *
 ***********************************************************************************/

void*   DLLEXPORT       STD_CLI_Driver_Send_Patern( int hCommunicationHandle , int hFileHandle , tsSTD_CLI_CommandItem tCommandItem , void *pValue , int iValueLength , int iValueSize ,double lfDelaySendingCommands ,  *pbSentStatus )
{
	STD_ERROR				StdError											=	{0};

	char					*pPrepareSendBuffer									=	NULL,
							*pSendBuffer										=	NULL;
							
	unsigned int			iMaxAllocationSize									=	0,
							iCurrentAllocationSize								=	0,
							index												=	0,
							copy_index											=	0,
							iContinuesCount										=	0;
	
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
			
		}
		else
		{
			switch( tCommandItem.tSendParameters[index].iParametersType )
			{
				case 1:
					sscanf( tCommandItem.tSendParameters[index].szDefaultValue , "%d" , &(tConvertValue.sdWord) );  
					iValueSize = sizeof(int);
					break;
				case 2:
					sscanf( tCommandItem.tSendParameters[index].szDefaultValue , "%lld" , &(tConvertValue.sqWord) );  
					iValueSize = sizeof(long long);
					break;
				case 3:
					sscanf( tCommandItem.tSendParameters[index].szDefaultValue , "%lf" , &(tConvertValue.lfWord) );  
					iValueSize = sizeof(double);
					break;
				case 4:
					pStringData = tCommandItem.tSendParameters[index].szDefaultValue;
					iValueSize = iCurrentAllocationSize;
					break;	
				case 5:
					sscanf( tCommandItem.tSendParameters[index].szDefaultValue , "%lld" , &(tConvertValue.sqWord) );  
					iValueSize = sizeof(long long);
					break;	
					
				default:
					pStringData = tCommandItem.tSendParameters[index].szDefaultValue;
					iValueSize = iCurrentAllocationSize;
					break;						
					
			}
		}
			
		CALLOC_COPY_ERR( pValuesArray[index] , iCurrentAllocationSize + 100 , sizeof(char) , tCommandItem.tSendParameters[index].szDefaultValue , strlen(tCommandItem.tSendParameters[index].szDefaultValue)); 
		
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
			case 5:
				
				memcpy( pValuesArray[index] , &(tConvertValue) , iValueSize );
				break;
				
			default:
				break;
		}
		
	//	iMaxAllocationSize += iCurrentAllocationSize + strlen(pValuesArray[index]);
	}
	
	CALLOC_ERR( pPrepareSendBuffer , iMaxAllocationSize + 10 , 1 );
	
	strcpy((char*)pPrepareSendBuffer , tCommandItem.szSendCommandFormated );
	
	for ( index = 0; index < tCommandItem.iSendNumberOfParameters ; index++ ) 
	{
		iValueLength = strlen(pValuesArray[index]);
			
		pStartParam = strstr((char*) pPrepareSendBuffer , "%s" );
		
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
	
//	FREE_STDERR_COPY_ERR( SetTerminatingPattern( hCommunicationHandle, "\r" , 1 ));  		 
																										   
	pRespondsFormated = StringReplaceAll( tCommandItem.szRespondsFormated , "\\r" , "\r" , NULL ); 		// (how much bytes i expect to receive) may be not actual					   
																															   
	pResponds = StringReplaceAll( pRespondsFormated , "\\n" , "\n" , NULL );
																															   
	if ( pResponds )
		iRespondsLength = strlen( pResponds );
		
	pSendBuffer = StringReplaceAll( pPrepareSendBuffer , "\\n" , "\n" , NULL );
	
	FREE( pPrepareSendBuffer );
	
	pPrepareSendBuffer = pSendBuffer;
	
	pSendBuffer = StringReplaceAll( pPrepareSendBuffer , "\\r" , "\r" , NULL ); 
	
	Send( hCommunicationHandle ,  hFileHandle , pSendBuffer , strlen(pSendBuffer) , lfDelaySendingCommands , pbSentStatus);
	
	
Error:
	
	FREE( pRespondsFormated );
	
	FREE( pResponds );
	
	FREE( pPrepareSendBuffer );
	
	FREE( pSendBuffer );
	
	FREE( pTempString );
	
	FREE_LIST( pValuesArray , tCommandItem.iSendNumberOfParameters );  
	
	RETURN_STDERR_POINTER;
}

///--------------------- API ------------------------------//

/***********************************************************************************
 *(CLI Driver) Set Value                                                           *
 ***********************************************************************************/

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
	
	int							iTryCounter											=	0,
							    iCommandLength										=	0;
	
	char						szLog[1024]											=	{0};
	
	int							bDataUpdated										=	0, 
								bDataUpdateFailed									=	0, 
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
		
	
	for ( iCommandIndex = 0 ; iCommandIndex < tLocalStorage.listSize ; iCommandIndex++ )								//find command
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
		iCommandLength = strlen ( tLocalStorage.pSTD_CLI_CommandList[iCommandIndex].szSendCommandFormated);
		if ( strlen ( tLocalStorage.pSTD_CLI_CommandList[iCommandIndex].szSendCommandFormated) !=0 )
		{
		FREE_STDERR_COPY_ERR( STD_CLI_Driver_Send_Patern( tLocalStorage.hCommunicationHandle , tLocalStorage.hFileHandle, tLocalStorage.pSTD_CLI_CommandList[iCommandIndex] , pValue , iValueLength , iValueSize ,tLocalStorage.lfDelaySendingCommands , &bSentSuccessfully)); 
		}
		
		else
		{
			bSentSuccessfully = 1; 	
		}
		
		GetCurrentDateTime(&lfStartTime);  
		
		if ( IS_NOT_OK )	  //check the error
		{
			OutputDebugString("\t\tUUT Driver Error : Can't send command.");   
			DelayWithEventProcessing(5.0);
			sprintf( szLog , "\t\tUUT Driver       : Try to send command again. (%d/%d)" , iTryCounter+1 , tLocalStorage.iNumberOfCommunicationTry );
			OutputDebugString(szLog);  
			continue;
		}
		
		if (  IS_NOT_OK || ( bSentSuccessfully == 0 ))
			continue;
		
		//may be need to place delay 
		
		do
		{
			STD_CLI_Driver_Receive_CallBack( hHandle ); 	
			
			CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage )); 
		
			if ( pLocalStorage->pLastError )
			{
				UPDATERR( *(pLocalStorage->pLastError) ); 
			}
			
			bDataUpdated = pLocalStorage->bDataUpdated;
			bDataUpdateFailed = pLocalStorage->bDataUpdateFailed;
			
			CHK_CMT(CmtReleaseTSVPtr ( hHandle ));            
			
			if ((IS_NOT_OK) || bDataUpdated ) 
				break;
			
			GetCurrentDateTime(&lfCurrentTime);
			
		} while((lfCurrentTime-lfStartTime) < tLocalStorage.lfReceiveTimeOut );
		
		if ((IS_OK) && bSentSuccessfully && ( bDataUpdateFailed == 0 )) 
			break;
	}
	
	IF ( bDataUpdateFailed , "Uncorrect answer received" );
Error:

	if ((IS_OK) && bSentSuccessfully ) 
		sprintf( szFormatedLog , "%s :: Sent command [%s] %%s" ,  tLocalStorage.ptCallbacks->szName , pCommandName );
	else
		sprintf( szFormatedLog , "%s :: Error sending command [%s] %%s" ,  tLocalStorage.ptCallbacks->szName , pCommandName );
	
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");  
	
	RETURN_STDERR_POINTER;
}


/***********************************************************************************
 * (CLI Driver) GetValue			                                               *
 ***********************************************************************************/
/*
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

/*	if ( StdError.error == 0 )
	{
		if ( CmtGetTSVPtr ( hHandle , &pLocalStorage ) == 0 )
		{
			if ( pLocalStorage->pLastError )
			{
				UPDATERR( *pLocalStorage->pLastError );
			}
	
			memset( &(pLocalStorage->pLastError) , 0 , sizeof(STD_ERROR));   //kills tread
			
			CmtReleaseTSVPtr ( hHandle );
		}
	}	 */
	
/*	RETURN_STDERR_POINTER;
}

 */



/***********************************************************************************
 * (CLI Driver) GetValue			                                               *
 ***********************************************************************************/

void*   DLLEXPORT   STD_CLI_Driver_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize, ...  )    
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL,			
								tLocalStorage										=	{0};
	
	tsSTD_CLI_CommandItem		tCommandItem										=   {0};	 

	tuConvert					tConvertValue;

	char						szFormatedLog[LOW_STRING]							=	{0};
								
	double						lfStartTime											=	0.0,
								lfCurrentTime										=	0.0;
	
	int							index												=	0;
	
	int							bSameType											=	0;
	
	int							iFirstType											=	0,
								iValueSize											=	0,  
								iLastSentCommandIndex								=	0;

	int							*pGetIntValue										=   NULL;
	
	double						*pGetDoubleValue									=	NULL;
	
	long long					*pGetLongValue										=	NULL;
	
	char						*pGetStringValue									=	NULL;
					
	char						*pSourceBuffer										=	NULL; 
		
	void						*pData												=	NULL;
	
	void						**pParametersArray									=	NULL;
	
	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};

	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	memcpy( &tLocalStorage , pLocalStorage , sizeof(tsLocalStorage));
	CHK_CMT(CmtReleaseTSVPtr ( hHandle ));   
	
	if ( piValueLength )
		*piValueLength = 0;
						
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
		tCommandItem = tLocalStorage.pSTD_CLI_CommandList[iLastSentCommandIndex]; 
		
		if	( tCommandItem.iReceiveNumberOfParameters > 0 )
		{
			pSourceBuffer = tLocalStorage.pBuffer;
		
			bSameType = 1;
			iFirstType = tCommandItem.tReceiveParameters[0].iParametersType;
		
			for ( index = 0; index <  tCommandItem.iReceiveNumberOfParameters; index++)  
				if ( iFirstType != tCommandItem.tReceiveParameters[index].iParametersType )
					bSameType = 0;
		
			if ( bSameType )
			{   
				switch( iFirstType )
				{
					case 1:
						iValueSize = sizeof(int);
						break;
					case 2:
						iValueSize = sizeof(long long);
						break;
					case 3:
						iValueSize = sizeof(double);
						break;
					case 4:
						iValueSize = tLocalStorage.iBufferLength;
						break;	
					case 5:
						iValueSize = tLocalStorage.iBufferLength;
						break;	
					
					default:
						iValueSize = tLocalStorage.iBufferLength;
						break;						
					
				}
			
				CALLOC_ERR( pParametersArray , tCommandItem.iReceiveNumberOfParameters , iValueSize );
				
				if ( piValueSize )
					*piValueSize = iValueSize;
				
				if ( piValueLength )
					*piValueLength = tCommandItem.iReceiveNumberOfParameters;
				
				if ( tCommandItem.iReceiveNumberOfParameters == 1 )
				{
					if ( iFirstType > 3 )
					{
						if ( piValueLength )
							*piValueLength = iValueSize;	
						
						if ( piValueSize )
							*piValueSize = 1;
					}
				}
			}
			else
			{
				CALLOC_ERR( pParametersArray , tCommandItem.iReceiveNumberOfParameters , sizeof(void*) );
				
				*piValueSize = 11;
			}
			
			for ( index = 0; index <  tCommandItem.iReceiveNumberOfParameters; index++)
			{
				FREE(pData);
				
				pData = String_Parsing ( pSourceBuffer , tCommandItem.tReceiveParameters[index].szStartPatern , tCommandItem.tReceiveParameters[index].szStopPatern , &pSourceBuffer ); 
				
				if ( pData == NULL )
					break;
				
				if ( tCommandItem.iReceiveNumberOfParameters > 1 )
					if ( piValueLength )
						*piValueLength = index + 1;
								
				switch( tCommandItem.tReceiveParameters[index].iParametersType ) // 1 - int , 2 - long , 3 - double , 4 - string 5 - Binary   
				{
					case 1:
					
						sscanf( pData , "%d" , &tConvertValue.udWord );    
					
						if ( bSameType )
						{
							((int*)pParametersArray)[index] = tConvertValue.udWord;
						}
						else
						{
							CALLOC_ERR( pGetIntValue , 1 , sizeof(int) );  	
							pParametersArray[index] = pGetIntValue;
							*pGetIntValue = tConvertValue.udWord;
						}
						
						break;
				
					case 2:
						sscanf( pData , "%lld" , &tConvertValue.sqWord );    
					
						if ( bSameType )
						{
							((long long*)pParametersArray)[index] = tConvertValue.sqWord;
						}
						else
						{
							CALLOC_ERR( pGetLongValue , 1 , sizeof(long long) );  	
							pParametersArray[index] = pGetLongValue;
							*pGetLongValue = tConvertValue.sqWord;
						}
						
						break;
						
					case 3:
						sscanf( pData , "%lf" , &tConvertValue.lfWord );    
					
						if ( bSameType )
						{
							((double*)pParametersArray)[index] = tConvertValue.lfWord;
						}
						else
						{
							CALLOC_ERR( pGetDoubleValue , 1 , sizeof(double) );  	
							pParametersArray[index] = pGetDoubleValue;
							*pGetDoubleValue = tConvertValue.lfWord;
						}

						break;
				
					case 4:
						
						if ( tCommandItem.iReceiveNumberOfParameters == 1 )
						{
							memcpy((char*)pParametersArray , pData, strlen(pData));
						}
						else
						{
							CALLOC_COPY_ERR( pGetStringValue , 1 , strlen(pData) , pData , strlen(pData) );  	
							pParametersArray[index] = pGetStringValue;
						}
					
						break;
						
					case 5:
				
						if ( tCommandItem.iReceiveNumberOfParameters == 1 )
						{
							memcpy ((char*)pParametersArray , pData, strlen(pData));
						}
						else
						{
							CALLOC_COPY_ERR( pGetStringValue , 1 , strlen(pData) , pData , strlen(pData) );  	
							pParametersArray[index] = pGetStringValue;
						}
					
						break;				
					default:
						break;
				}		   
			
				
			}
		
			*pGetValue = (void**)pParametersArray;
		
		}

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

	
	FREE(pData);
	
	RETURN_STDERR_POINTER;
}



/***********************************************************************************
 * (CLI Driver) GetValue Extended			                                       *
 ***********************************************************************************/

void*   DLLEXPORT   STD_CLI_Driver_GetValue_Ex( int hHandle , char *pCommandName , void **pGetValue , int *piValueLength , int *piValueSize, ...  )    
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL,			
								tLocalStorage										=	{0};			

	int							bCommandFound										=	0,
								iCommandIndex										=	0; 
									
	double						lfStartTime											=	0.0,
								lfCurrentTime										=	0.0;
									
	int							iTryCounter											=	0;
	
	char						szLog[1024]											=	{0};
	char						szFormatedLog[LOW_STRING]							=	{0};
	
	int							bDataUpdated										=	0, 
								bDataUpdateFailed									=	0, 
								bSentSuccessfully									=	0;  
									
	double						lfWaitForConnectionTimeout							=	0.0;
	int							iLastSentCommandIndex								=	0;
	
	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};

									
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
	
	GetCurrentDateTime(&lfStartTime);
		
	for ( iTryCounter = 0 ; iTryCounter < tLocalStorage.iNumberOfCommunicationTry ; iTryCounter++ )
	{
	FREE_STDERR_COPY_ERR( STD_CLI_Driver_Send_Patern( tLocalStorage.hCommunicationHandle , tLocalStorage.hFileHandle, tLocalStorage.pSTD_CLI_CommandList[iCommandIndex] , NULL , 0 , 0 ,tLocalStorage.lfDelaySendingCommands , &bSentSuccessfully));
		
		if ( IS_NOT_OK )
		{
			OutputDebugString("\t\tUUT Driver Error : Can't send command.");   
			DelayWithEventProcessing(5.0);
			sprintf( szLog , "\t\tUUT Driver       : Try to send command again. (%d/%d)" , iTryCounter+1 , tLocalStorage.iNumberOfCommunicationTry );
			OutputDebugString(szLog);  
			continue;
		}
		
		if (  IS_NOT_OK || ( bSentSuccessfully == 0 ))
			continue;
		
		do
		{
//			ProcessSystemEvents();
			
			STD_CLI_Driver_Receive_CallBack( hHandle ); 
			
			CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage )); 
		
			if ( pLocalStorage->pLastError )
			{
				UPDATERR( *(pLocalStorage->pLastError) ); 
			}
			
			bDataUpdated = pLocalStorage->bDataUpdated;
			bDataUpdateFailed = pLocalStorage->bDataUpdateFailed;
			
			CHK_CMT(CmtReleaseTSVPtr ( hHandle ));            
			
			if ((IS_NOT_OK) || bDataUpdated ) 
				break;
			
			GetCurrentDateTime(&lfCurrentTime);
			
		} while((lfCurrentTime-lfStartTime) < tLocalStorage.lfSendTimeOut );
		
		if ((IS_OK) && bSentSuccessfully && ( bDataUpdateFailed == 0 ) ) 
			break;
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
		
		iLastSentCommandIndex =  tLocalStorage.iLastSentCommandIndex;
		sprintf( szFormatedLog , "%s :: Has Received respond of command [%s] %%s" ,  tLocalStorage.ptCallbacks->szName , tLocalStorage.pSTD_CLI_CommandList[iLastSentCommandIndex].szCommandName );
	
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d"); 

Error:

	if ((IS_OK) && bSentSuccessfully ) 
		sprintf( szFormatedLog , "%s :: Sent command [%s] %%s" ,  tLocalStorage.ptCallbacks->szName , pCommandName );
	else
		sprintf( szFormatedLog , "%s :: Error sending command [%s] %%s" ,  tLocalStorage.ptCallbacks->szName , pCommandName );
	
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");  
	
	
	RETURN_STDERR_POINTER;

}
}


void*   DLLEXPORT   STD_CLI_Driver_Unplug( int hHandle , double lfTimeOut )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
//	FREE_STDERR_COPY_ERR( Communication_Unplug( pLocalStorage->hCommunicationHandle , lfTimeOut )); 

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
	
//	FREE_STDERR_COPY_ERR( Communication_PlugIn( pLocalStorage->hCommunicationHandle , lfTimeOut )); 

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



/***********************************************************************************
 *(CLI Driver) LoadConfigFile                                                      *
 ***********************************************************************************/

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


/***********************************************************************************
 *(CLI Driver) LoadConfigBuffer                                                    *
 ***********************************************************************************/

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
		
Error:

	if ( piStatus )
		*piStatus = bCommandFound;
	
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
		CALLOC_COPY_STRING_ERR( (*pCommandsList)[iIndex] , pLocalStorage->pSTD_CLI_CommandList[iIndex].szCommandName );
		
		if ( piNumberOfCommands )
			(*piNumberOfCommands)++;
	}

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}



/***********************************************************************************
 *(CLI Driver) Receive                                                             *
 ***********************************************************************************/

char*		Receive( int hCommunicationHandle , int iTryNumber , int hFileHandle , int iExactSize , int *piCount )
{
	int							iIndex												=	0, 
								iToRead												=	0, 
								iReceiveCount										=	0,
								iBufferLen											=	0;
	
	char						szBuffer[10000]										=	{0},
								*pBuffer											=	NULL;
	
	double						lfLogCurrentTime									=	0.0;
	
	char						szLogFormated[LOW_STRING]							=	{0};
		
	int							iTryIndex											=	0;
	
	SetBreakOnLibraryErrors (0);
	
	if ( iExactSize > 10000 )
		iExactSize = 9999;
	
	if ( iExactSize == 0 )
		viGetAttribute ( hCommunicationHandle , VI_ATTR_ASRL_AVAIL_NUM, &iExactSize ); 
	
	iToRead = iExactSize;  
	
	iTryIndex = iTryNumber;
	
	do
	{
		if ( iToRead <= 0 )  
			iToRead = 16;
		
		viRead ( hCommunicationHandle , szBuffer+iBufferLen , iToRead , &iReceiveCount );
		
		iBufferLen += iReceiveCount;
		
		if ( iExactSize > iBufferLen )
			iToRead = (iExactSize-iBufferLen) ;
		
		if ( iReceiveCount == 0 )
			iTryIndex--;
		else	
			iTryIndex = iTryNumber;	
		
	} while(( iTryIndex > 0 ) || ( iReceiveCount > 0 ));

	SetBreakOnLibraryErrors (1);
	
	for( iIndex = 0; iIndex < iBufferLen; iIndex++ )
		if ( szBuffer[iIndex] == 0 )
			szBuffer[iIndex] = ' ';
	
	if ( iBufferLen )
	{
		GetCurrentDateTime( &lfLogCurrentTime ); 
		
		FormatDateTimeString ( lfLogCurrentTime , "\r\n%H:%M:%S.%3f    Received    :    \r\n" , szLogFormated , (LOW_STRING-1) );
			
		if ( hFileHandle )
		{
			WriteFile ( hFileHandle , szLogFormated , strlen(szLogFormated) ); 
			WriteFile ( hFileHandle , szBuffer , iBufferLen );
		}
	}
	
Error:
		
	ProcessSystemEvents();
	
	if ( piCount )
		*piCount = iBufferLen;
	
	pBuffer = calloc( iBufferLen+1 , sizeof(char));
	
	if (( pBuffer ) && ( iBufferLen > 0 ))
		memcpy( pBuffer , szBuffer , iBufferLen ); 
	
	return pBuffer;
}


/***********************************************************************************
 *(CLI Driver) Send                                                                 *
 ***********************************************************************************/

int		Send( int hCommunicationHandle , int hFileHandle , char *pBuffer , int iCount , double lfDelaySendingCommands ,  *pbSentSuccessfully )
{
	int							iToSend												=	0,
								iOffset												=	0,
								iTempCount											=	0,
								iSentCount											=	0;

	double						lfLogCurrentTime									=	0.0;
	
	char						szLogFormated[LOW_STRING]							=	{0};
	
	char						*pTemp 												=	NULL,
								*pStart												=	NULL;
	
	int							bCommandDelay										=	0;
	
	ViStatus					ConnectionStatus									=	0; 
	
//	SetBreakOnLibraryErrors (0);
	
		do
		{
			iToSend = iCount - iSentCount;
			
			pStart = pBuffer+iOffset;
			
			pTemp = strchr( pStart , '\r' );
			
			if ( pTemp )
			{
				iToSend = pTemp - pStart;
				
				if (( iToSend == 0 ) && ( pStart[0] == '\r' ))
				{
					iToSend = 1;
					bCommandDelay = 1;
				}
			}
			
			
			ConnectionStatus = viWrite ( hCommunicationHandle , pBuffer+iOffset , iToSend , &iTempCount );
			
			if (  ConnectionStatus	>=	VI_SUCCESS) *pbSentSuccessfully = 1;
			else  *pbSentSuccessfully = 0;
			
			
			if ( bCommandDelay )
			{
				if ( lfDelaySendingCommands > 0.0 )
					DelayWithEventProcessing(lfDelaySendingCommands);	  
				
				bCommandDelay = 0;
			}
			
			iSentCount += iTempCount;
			
			iOffset = iSentCount;
			
			if ( iTempCount == 0 )
				break;
			
		} while( iSentCount < iCount );
	


//	SetBreakOnLibraryErrors (1);
	
	GetCurrentDateTime( &lfLogCurrentTime ); 
	
	FormatDateTimeString ( lfLogCurrentTime , "\r\n\r\n%H:%M:%S.%3f    Sent        :    \r\n" , szLogFormated , (LOW_STRING-1) );
			
	if ( hFileHandle )
	{
		WriteFile ( hFileHandle , szLogFormated , strlen(szLogFormated) ); 
		WriteFile ( hFileHandle , pBuffer , iSentCount );  	
	}
	
Error:
	
	ProcessSystemEvents();
	
	return iSentCount;
}



/***********************************************************************************
 *(CLI Driver) SetTerminatingPattern                                               *	Not in use meantime
 ***********************************************************************************/

void* DLLEXPORT	SetTerminatingPattern( int hHandle , char *pszTerminatingPattern , int iLength )
{
	
	STD_ERROR						StdError						=	{0};

	tsLocalStorage					*pLocalStorage					=	NULL;
	
	IF (( hHandle == 0 ) , "handle can't be zero." );   

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	if ( pszTerminatingPattern && iLength )
	{
		CALLOC_ERR( pLocalStorage->pszTerminatingPattern , iLength+1 , sizeof(char));
		
		memcpy( pLocalStorage->pszTerminatingPattern , pszTerminatingPattern , (iLength*sizeof(char)));
		
		pLocalStorage->iTerminatingPatternLength = iLength;
	}
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}
