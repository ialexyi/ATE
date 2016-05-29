//==============================================================================
// Include files
#include "Communication_Headers.h" 
#include "Communication_Thread.h"
#include "GlobalDefines.h"
#include "Communication_Low_Level.h"
#include <formatio.h>


//==============================================================================
// Constants
#define							LOCAL_BUFFER_SIZE							1024

//==============================================================================
// Types

typedef struct
{
	int							hFileHandle,
								hLockHandle;
	
	int							bWrite_read;
	
	char						*pBuffer;
	
	int							iBufferSize;
	
	int							iOperationStatus;				
	
	int							bAsciiPrint;
	
	char						szProtect[64];
	
} tsLogPrintItem;


typedef struct
{
	void						*pCloseFunction,
								*pData;
	
	int							iError,
								iSessionHandle;
	
	char						szProtect[64];
	
} tsCloseDataItem;

typedef struct
{
	void						*pErrorFunction,
								*pData;
	
	int							iSessionHandle;
	
	STD_ERROR						ret;
	
	char						szProtect[64];
	
} tsErrorDataItem;

//==============================================================================
// Static global variables

//==============================================================================
// Static functions
void		COM_ReceiveCallBack( void *pData );
void		COM_AttributeCallBack( void *pData );
void		COM_LogPrintCallBack( void *pData );
void		COM_CloseCallBack( void *pData );
void		COM_ErrorCallBack( void *pData );

void		COM_Support_Copy_Store( tsSingleHandle *pTargetStore , tsSingleHandle *pSourceStore );

void CVICALLBACK COM_DiscardLocalVarStore (void *threadLocalPtr, int event, void *callbackData, unsigned int threadID);
void CVICALLBACK COM_MessageQueue (CmtTSQHandle queueHandle, unsigned int event, int value, void *callbackData);


char		*StringReplaceAll( char *szSourceString , char *szFindWhat , char *szReplaceWith , int *pCount );

//==============================================================================
// Global variables

//==============================================================================
// Global functions


int			COM_Thread( void *pData )
{
	STD_ERROR			StdError								=	{0};
	
	tsSingleHandle		*pLocalVarStore							=	NULL,
						*pThreadSafeStore						=	NULL;
	
	CmtTLVHandle 		hLocalHandle							=	0;
	
	CmtTSVHandle		hThreadHandle							=	0;
	
	CmtTSQHandle		hQueueHandle							=	0;
	
	ViSession			visaObjHandle							=	0,
						defaultRM								=	0; 
							
	unsigned char		*pDynamicBuffer							=	NULL;
					  
	unsigned int		bytesReaded								=	0,
						bufferCounter							=	0,
						lastStartIndex							=	0, 
						iReceiveCounter							=	0; 
							
	char				szLogFormated[1024]						=	{0},
						szHardwareName[1024]					=	{0},  
						szComString[128]						=	{0},
						szVisaAddress[1024]						=	{0};
						
	unsigned int		receiveBufferSize						=	0,
						receiveBufferIndex						=	0, 
						receiveCount							=	0; 
							
	int					iCurrentThreadPriority					=	0;
	
	double				lfLogCurrentTime						=	0;
	
	int					bFirstTime								=	0;

	ViFindList 			hFindHandle								=	0;

	ViUInt32			iReturn_Count							=	0;					
	
	int					iComNumber								=	0,
						iTryIndex								=	0, 
						iCount									=	0,
						iCounter								=	0,
						iTryReceiveCounter						=	0,
						iRotateCarry							=	0,
						status									=	0,  
						bTryReceiveAgain						=	0,
						bComMismatch							=	0;
	
	unsigned __int64	iCurrentBufferSize						=	0;
	
	double				lfPrevTime								=	0.0;
	
	unsigned char		*pLocalReceiveBuffer					=	NULL,
						*pLocalSendBuffer						=	NULL;

	unsigned short		interfaceType							=	0;
	
	char				bInitPass								=	0,
						bReading								=	0;
							
	tsLogPrintItem		*ptLogPrintItem							=	NULL;
	
	int					hLogLockHandle							=	0;
	
	tsCloseDataItem		*pCloseDataItem							=	NULL; 
		
	unsigned char		*pReceiveBuffer							=	NULL;
	
	int					bRelativeAddress						=	0,
						bReceiveDataPresent						=	0;
	
	char				*pSerialConnection						=	0;
	
	tsReceiveDataItem	*pReceiveDataItem						=	NULL;

	int					hThreadInitiated						=	0;
	
	if ( pData == NULL )
		return -1;
	
	hThreadHandle = *((int*) pData);  
		
	CHK_CMT( CmtNewTSQ ( 10 , sizeof(tsSingleHandle) , OPT_TSQ_DYNAMIC_SIZE , &hQueueHandle )); 
	
	//--------------- Initialyzing --------------------------//
	CHK_CMT( CmtNewThreadLocalVar ( sizeof(tsSingleHandle) , NULL, COM_DiscardLocalVarStore, NULL, &hLocalHandle ));
	
	CHK_CMT( CmtInstallTSQCallback (hQueueHandle, EVENT_TSQ_ITEMS_IN_QUEUE, 1 , COM_MessageQueue, &hLocalHandle, CmtGetCurrentThreadID(), NULL));
	
	CHK_CMT( CmtGetThreadLocalVar ( hLocalHandle , &pLocalVarStore ));
	
	if ( CmtGetTSVPtr ( hThreadHandle , &pThreadSafeStore ) == 0 )
	{
		hThreadInitiated = pThreadSafeStore->hThreadInitiated;
		pThreadSafeStore->thread_handle = hThreadHandle;  
		pThreadSafeStore->queue_handle = hQueueHandle;
		
		memset( pLocalVarStore->szMax_Buffer , 0 , sizeof( pLocalVarStore->szMax_Buffer));
		
		COM_Support_Copy_Store( pLocalVarStore , pThreadSafeStore );
		
		CmtReleaseTSVPtr (hThreadHandle);
	}
	
	CmtFlushTSQ ( hQueueHandle , TSQ_FLUSH_ALL, NULL);
	
	CHK_VSA( viOpenDefaultRM (&defaultRM));
	
	bFirstTime = 1;
	
	strcpy( szVisaAddress , pLocalVarStore->pszConnectString );
	
	SetBreakOnLibraryErrors (0);
	
	strcpy( szComString , szVisaAddress);  
	
	StringUpperCase ( szComString );
	
	if ( strstr( szComString , "COM" ))
	{
		sscanf( szComString , "COM%d" , &iComNumber );  
		
		if ( iComNumber > 0 )
		{
			sprintf( szVisaAddress , "ASRL%d" , iComNumber );  
		
			bRelativeAddress = 1;
		}
	}
		
	do
	{			
		
		for( iTryIndex = 0; iTryIndex < 3; iTryIndex++ )
		{
			STDERR( viOpen ( defaultRM , szVisaAddress , VI_NULL, VI_NULL, &visaObjHandle ));
			
			if ( IS_OK )
				break;
		}
		
		pSerialConnection = strstr( szVisaAddress , "ASRL" ); 
		
		if ( bComMismatch == 0 )
		{
			if ( pSerialConnection == NULL )
				break;
		}
	
		if ( bRelativeAddress == 0 )
			break;
		
		if ( pSerialConnection )
		{
			if ( bFirstTime )
				sscanf( pSerialConnection , "ASRL%d" , &iComNumber );
			
			sprintf( szComString , "COM%d" , iComNumber );
		}
		else
		{
			break;
		}
		//-------------- check com numbering --------------------//
		STDERR( viGetAttribute ( visaObjHandle , VI_ATTR_INTF_INST_NAME , szHardwareName ));
				
		if ( IS_OK )
		{
			if ( strstr( szHardwareName , szComString ))
				break;
		}
		
		CHK_VSA( viClose ( visaObjHandle ));
		visaObjHandle = 0;
			
		bComMismatch = 1; 
		
		if ( bFirstTime )
		{
			CHK_VSA( viFindRsrc ( defaultRM , "?*" , &hFindHandle , &iReturn_Count , szVisaAddress )); 
			
			bFirstTime = 0; 
			
			if ( strstr( szVisaAddress , "ASRL") )
				continue;
		}
		
		do
		{
			CHK_VSA( viFindNext ( hFindHandle , szVisaAddress )); 	
			
		}while( strstr( szVisaAddress , "ASRL") == NULL );
		 
	} while((iReturn_Count--) > 0 );
		
	CHK_VSA_ERROR; 
	
	IF (( visaObjHandle == 0 ) , "Can't find address");
	
	DelayWithEventProcessing( 0.1 );
	
	CHK_VSA( viGetAttribute ( visaObjHandle , VI_ATTR_INTF_TYPE , &interfaceType )); 
	
	CHK_CMT( CmtNewLock ( NULL , 0 , &pLocalVarStore->hSessionLock ));
		
	pLocalVarStore->sessionHandle = visaObjHandle;
	pLocalVarStore->initStatus = 1; 
	pLocalVarStore->bNeededComConfig = 1;
	CALLOC_COPY_STRING( pLocalVarStore->pszVisaAddress , szVisaAddress );
	
	if ( interfaceType == VI_INTF_ASRL )
		viSetAttribute (visaObjHandle, VI_ATTR_ASRL_END_IN, VI_ASRL_END_NONE);
	
	//-------------- Default Values ------------------//
	if ( pLocalVarStore->receiveBufferSize == 0 )
		pLocalVarStore->receiveBufferSize = LOCAL_BUFFER_SIZE;  
	
	if ( pLocalVarStore->MaxThreadPriority == 0 )
		pLocalVarStore->MaxThreadPriority = THREAD_PRIORITY_HIGHEST;  
	
	if (pLocalVarStore->lfReceiveSingleByteTimeOut  == 0.0 )
		pLocalVarStore->lfReceiveSingleByteTimeOut = 0.3;  
		
	
	bInitPass = 1;
	
	CHK_CMT( CmtNewLock ( NULL ,  OPT_TL_PROCESS_EVENTS_WHILE_WAITING  , &hLogLockHandle ));	
	
	pLocalVarStore->hLogLockHandle = hLogLockHandle;

	//------------------------------------------------//
	if ( CmtGetTSVPtr ( hThreadHandle , &pThreadSafeStore ) == 0 )
	{
		COM_Support_Copy_Store( pThreadSafeStore , pLocalVarStore );
		
		pLocalVarStore->initStatus = 1; 
		
		CmtReleaseTSVPtr (hThreadHandle);
	} 

	//-------------------------------------------------------------------//
	
	CALLOC_ERR( pDynamicBuffer , (int)pLocalVarStore->receiveBufferSize , sizeof(char));
	receiveBufferSize = pLocalVarStore->receiveBufferSize;
	
	iCurrentBufferSize = receiveBufferSize;
	CALLOC_ERR( pLocalReceiveBuffer , (int)iCurrentBufferSize , sizeof(char) ); 
		
	bufferCounter = 0;
	lastStartIndex = 0;
	
	SetBreakOnLibraryErrors (1);
	
	do
	{  
		ProcessSystemEvents();  
		
		//===========================================================================================//
		if ( pLocalVarStore->bReceiveEnable == 0 ) 		
		{   
			if ( iCurrentThreadPriority != THREAD_PRIORITY_LOWEST )
			{
				CmtSetCurrentThreadPriority (THREAD_PRIORITY_LOWEST);
				iCurrentThreadPriority = THREAD_PRIORITY_LOWEST;
			}
				
			continue;
		}
		else
		{   
			if ( iCurrentThreadPriority != pLocalVarStore->MaxThreadPriority )
			{
				CmtSetCurrentThreadPriority (pLocalVarStore->MaxThreadPriority);
				iCurrentThreadPriority = pLocalVarStore->MaxThreadPriority;
			}
		}
		
		//===========================================================================================//
		if (( pLocalVarStore->bReceiveEnable ) && ( pLocalVarStore->bNeededComConfig == 0 ) && ( pLocalVarStore->bUnpluged == 0 ))
		{
			bytesReaded = 0;   
			
			memset( pLocalReceiveBuffer , 0 , (size_t)iCurrentBufferSize );
			
			SetBreakOnLibraryErrors (0);
			
			iCounter = 0;
			
			lfPrevTime = 0.0;
			
			CmtGetLock (pLocalVarStore->hSessionLock);
			
			switch( interfaceType )
			{
				case 4:
					
					viGetAttribute ( visaObjHandle , VI_ATTR_ASRL_AVAIL_NUM , &iCount );
					
					if ( iCount > iCurrentBufferSize )
					{
						iCurrentBufferSize = iCount;

						FREE_CALLOC( pLocalReceiveBuffer , (int)iCurrentBufferSize , sizeof(char) );  
						
						if ( pLocalReceiveBuffer == NULL )
						{
							CmtReleaseLock (pLocalVarStore->hSessionLock);
							FORCE_ERR(-999,"Can't allocate memory");
						}
					}
					
					if ( iCount )
						status = viRead ( visaObjHandle , pLocalReceiveBuffer , iCount , (ViPUInt32)&iCounter );
						
					break;
					
				default:
						if ( pLocalVarStore->iReadBlockSize > iCurrentBufferSize )
						{
							iCurrentBufferSize = pLocalVarStore->iReadBlockSize;

							FREE_CALLOC( pLocalReceiveBuffer , (int)iCurrentBufferSize , sizeof(char) );  
						
							if ( pLocalReceiveBuffer == NULL )
							{
								CmtReleaseLock (pLocalVarStore->hSessionLock);
								FORCE_ERR(-999,"Can't allocate memory");
							}
						}
					
						status = viRead ( visaObjHandle , pLocalReceiveBuffer , pLocalVarStore->iReadBlockSize , (ViPUInt32)&iCounter);
							
						break;  
			};
			
			//viStatusDesc ( visaObjHandle , status , ret.szErrorDescription );
			
			CmtReleaseLock (pLocalVarStore->hSessionLock);

			bytesReaded = iCounter;
				
			SetBreakOnLibraryErrors (1); 
		}
		
		do
		{
			if ( bytesReaded )
			{   
				if ( bReading == 0 )
				{
					bReading = 1;
					
					lastStartIndex = bufferCounter; 
				}
				
				if (( bufferCounter + bytesReaded ) >= receiveBufferSize )  
				{
					receiveBufferSize = (( bufferCounter + bytesReaded ) * 2 );
					pDynamicBuffer = realloc( pDynamicBuffer , receiveBufferSize * sizeof(char));
					
					IF (( pDynamicBuffer == NULL ) , "Can't allocate memory for receive buffer." );
				}
				
				memcpy( pDynamicBuffer+bufferCounter , pLocalReceiveBuffer , bytesReaded );
					
				bufferCounter += bytesReaded; 
			
				if ( bufferCounter >= receiveBufferSize )
					bufferCounter = 0;
				
				iReceiveCounter = 0; 
			}
			
			bTryReceiveAgain = 0;
			//--- Check retry condition -------------//
			if ( bytesReaded ) 
				bTryReceiveAgain = 1;
			else
				if ( status == VI_ERROR_TMO )
					if ( iTryReceiveCounter > 0 )
					{
						iTryReceiveCounter--;
						bTryReceiveAgain = 1;
					}
				
			if (( pLocalVarStore->pszTerminatingPattern ) && ( strstr((char*)pLocalReceiveBuffer,pLocalVarStore->pszTerminatingPattern)))
				bTryReceiveAgain = 0;
		
			if (( bufferCounter >= pLocalVarStore->receiveBreakCount ) && ( pLocalVarStore->receiveBreakCount > 0 ))
			{
				pLocalVarStore->receiveBreakCount = 0;
				bTryReceiveAgain = 0;
			}
			
			if ( bTryReceiveAgain )
				break;
			
			if ( bReading )
			{
				iTryReceiveCounter = 1;
				bReading = 0;
				
				FREE(pReceiveBuffer);
			
				if ( pLocalVarStore->bUpdateReceiveAll == 0 )
				{
					if (( lastStartIndex + bufferCounter ) >= receiveBufferSize ) 
						iRotateCarry = receiveBufferSize - lastStartIndex;
					else
						iRotateCarry = 0;
					
					pReceiveBuffer = calloc ( bufferCounter + iRotateCarry + 128 , 1 );
			
					if ( pReceiveBuffer  )
					{
						if (( lastStartIndex + bufferCounter ) < receiveBufferSize )
							memcpy( pReceiveBuffer , pDynamicBuffer + lastStartIndex , bufferCounter );
						else
						{
							memcpy( pReceiveBuffer , pDynamicBuffer + lastStartIndex , iRotateCarry );
							memcpy( pReceiveBuffer + iRotateCarry , pDynamicBuffer , ( bufferCounter - iRotateCarry ) ); 
						}
					
						bReceiveDataPresent = 1;
					}
			
					if (( bufferCounter - lastStartIndex ) > 0 )
						receiveCount = bufferCounter - lastStartIndex;  
				
					receiveBufferIndex = lastStartIndex;
				}
				else	
				{
					CALLOC( pReceiveBuffer , bufferCounter + 100 , sizeof(char) ); 
				
					if ( pReceiveBuffer )
					{
						memcpy( pReceiveBuffer , pDynamicBuffer , (size_t)bufferCounter );
						bReceiveDataPresent = 1;
					}
					else
					{
						FORCE_ERR(-999,"Can't allocate memory");
					}
				
					receiveCount = bufferCounter; 
					receiveBufferIndex = 0;
				
				}
			
				//-------------------------------------------------------------//
			
				//-------- Loging --------------------//
				if ( pLocalVarStore->hLogFileHandle > 0 )
				{
					CALLOC( ptLogPrintItem , 1 , sizeof(tsLogPrintItem));
				
					if ( ptLogPrintItem )
					{
						ptLogPrintItem->bWrite_read = 0;
						ptLogPrintItem->iBufferSize = bufferCounter;
						CALLOC( ptLogPrintItem->pBuffer , bufferCounter , 1 );
						ptLogPrintItem->hLockHandle = hLogLockHandle;
						ptLogPrintItem->hFileHandle = pLocalVarStore->hLogFileHandle;
						ptLogPrintItem->bAsciiPrint = pLocalVarStore->bLogFormatIsAscii; 
					
						if ( ptLogPrintItem->pBuffer )
						{
							memcpy( ptLogPrintItem->pBuffer , pReceiveBuffer , bufferCounter );
					
							PostDeferredCall( COM_LogPrintCallBack , ptLogPrintItem );
							ProcessSystemEvents();
							
						}
						else
							FREE( ptLogPrintItem );
					}
				
				}
			
				if (( pLocalVarStore->pReceiveCallBack ) && bReceiveDataPresent )
				{
					CALLOC( pReceiveDataItem , 1 , sizeof(tsReceiveDataItem));

					if ( pReceiveDataItem )
					{
						pReceiveDataItem->pCallBackFunction = pLocalVarStore->pReceiveCallBack;
						pReceiveDataItem->pReceiveCallBackData= pLocalVarStore->pReceiveCallBackData;
						
						pReceiveDataItem->receiveCount = bufferCounter;
						pReceiveDataItem->pBuffer = pReceiveBuffer;
						pReceiveBuffer = NULL;
						
						PostDeferredCallToThread ( COM_ReceiveCallBack , (void*)pReceiveDataItem , hThreadInitiated );
						
						ProcessSystemEvents();
						bReceiveDataPresent = 0;
					}
				}
				
				bufferCounter = 0;  
			}
		
		} while(0);
		
	} while(pLocalVarStore->bFinishThread==0);
	
Error:
	
	if ( IS_NOT_OK )
	{
		if ( CmtGetTSVPtr ( hThreadHandle , &pThreadSafeStore ) == 0 )
		{
			if ( pThreadSafeStore->pLastError )
			{
				FREE_STDERR_POINTER(pThreadSafeStore->pLastError);
				FREE(pThreadSafeStore->pLastError);
			}
			
			GET_STDERR_CLEAR(pThreadSafeStore->pLastError);  
			CmtReleaseTSVPtr (hThreadHandle);
		}
	}
	
	//-------------- Execute Close Callback ------------------//
	CALLOC( pCloseDataItem , 1 , sizeof(tsCloseDataItem));
	
	pCloseDataItem->iError = StdError.error;
	pCloseDataItem->iSessionHandle = visaObjHandle;
	
	if ( pLocalVarStore )
	{
		pCloseDataItem->pCloseFunction = pLocalVarStore->pCloseCallBack;
		pCloseDataItem->pData = pLocalVarStore->pCloseCallBackData; 
	}
	
	PostDeferredCallToThread ( COM_CloseCallBack , pCloseDataItem , hThreadInitiated ); 
	ProcessSystemEvents();
	//----------------------------------------------------------//
	
	if ( pLocalVarStore && ( pLocalVarStore->hLogFileHandle > 0 ))
	{
		GetCurrentDateTime(&lfLogCurrentTime);
		FormatDateTimeString ( lfLogCurrentTime , "\r\n%H:%M:%S.%3f    Close Com.\r\n" , szLogFormated , 1024 );
		WriteFile ( pLocalVarStore->hLogFileHandle , szLogFormated , strlen(szLogFormated) ); 
		
		CloseFile ( pLocalVarStore->hLogFileHandle );
		pLocalVarStore->hLogFileHandle = 0;
	}
	
	if ( hLogLockHandle )
		CmtDiscardLock ( hLogLockHandle );
	
	if ( visaObjHandle )
		viClose( visaObjHandle );
	
	FREE(pLocalSendBuffer);
	FREE(pLocalReceiveBuffer);
	FREE(pDynamicBuffer);
		
	if ( hQueueHandle )
		CmtDiscardTSQ(hQueueHandle);
	
	if ( hLocalHandle )
		CmtDiscardThreadLocalVar ( hLocalHandle ); 
	
	RETURN_ERROR_VALUE;
}

void CVICALLBACK COM_DiscardLocalVarStore (void *threadLocalPtr, int event, void *callbackData, unsigned int threadID)
{
	tsSingleHandle		*pLocalVarStore							=	NULL;  	
	
	pLocalVarStore = (tsSingleHandle*)threadLocalPtr;
	
	FREE( pLocalVarStore->pszConnectString);        
	FREE( pLocalVarStore->pSendBuffer);              
	FREE( pLocalVarStore->pszTerminatingPattern);    
	FREE( pLocalVarStore->pszLogFile);               
	FREE( pLocalVarStore->pszVisaAddress); 
	
}

void		COM_Support_Copy_Store( tsSingleHandle *pTargetStore , tsSingleHandle *pSourceStore )
{
	if (( pTargetStore == NULL ) || ( pSourceStore == NULL ))
		return;
	
	FREE( pTargetStore->pszConnectString);        
	FREE( pTargetStore->pSendBuffer);              
	FREE( pTargetStore->pszTerminatingPattern);    
	FREE( pTargetStore->pszLogFile);               
	FREE( pTargetStore->pszVisaAddress); 
	
	memcpy( pTargetStore , pSourceStore , sizeof(tsSingleHandle)); 
   
	pTargetStore->pszConnectString = NULL;      
	pTargetStore->pSendBuffer = NULL; 
	pTargetStore->pszTerminatingPattern = NULL;
	pTargetStore->pszLogFile = NULL;
	pTargetStore->pszVisaAddress = NULL;
	
	CALLOC_COPY_STRING(( pTargetStore->pszConnectString) ,( pSourceStore->pszConnectString ));      
	CALLOC_COPY_STRING(( pTargetStore->pszLogFile),( pSourceStore->pszLogFile ));             
	CALLOC_COPY_STRING(( pTargetStore->pszVisaAddress ),( pSourceStore->pszVisaAddress ));             
	
	if ( pSourceStore->iTerminatingPatternLength )
	{
		CALLOC(( pTargetStore->pszTerminatingPattern) , pSourceStore->iTerminatingPatternLength , sizeof(char));  

		if ( pTargetStore->pszTerminatingPattern)
			memcpy( ( pTargetStore->pszTerminatingPattern) , ( pSourceStore->pszTerminatingPattern) , (pSourceStore->iTerminatingPatternLength * sizeof(char)));
	}
	
	if ( pTargetStore->sendBufferSize )
	{
		CALLOC( pTargetStore->pSendBuffer , pTargetStore->sendBufferSize , sizeof(char));

		if ( pTargetStore->pSendBuffer && pSourceStore->pSendBuffer )
			memcpy( pTargetStore->pSendBuffer , pSourceStore->pSendBuffer , pTargetStore->sendBufferSize ); 
	}
	
	return;
}

void CVICALLBACK COM_MessageQueue (CmtTSQHandle queueHandle, unsigned int event, int value, void *callbackData)
{
	STD_ERROR			StdError								=	{0};   
                                                                       	
	tsSingleHandle		*pLocalVarStore							=	NULL,  	
						*pThreadSafeStore						=	NULL,
						*pGetVarStore							=	NULL;  	

	tsSingleHandle		tGetVarStore							=	{0};  	
		
	CmtTLVHandle 		hLocalHandle							=	0;     	
                                                                       	
	CmtTSVHandle		hThreadHandle							=	0;     	
                                                                       	
	ViSession			visaObjHandle							=	0,     	
						defaultRM								=	0;     	
						                                               	
	unsigned int		iSendCount								=	0,     	
						iSendSingleCount						=	0, 
						iLocalSendSize							=	0;     	

	int					iNumberOfItems							=	0; 

	char				szLogFileName[1024]						=	{0},   	
						szLogFormated[1024]						=	{0};   	
					                                                   	
	double				lfLogCurrentTime						=	0;     	
                                                                       	
	int					bPlugInRequest							=	0, 
						bFlushReceive							=	0, 
						bSendRequest							=	0,
						bUnplugRequest							=	0;     	
                                                                       	
	unsigned char		*pLocalSendBuffer						=	NULL;  	
                                                                       	
	unsigned short		interfaceType							=	0;     	
                                                                       	
	char				bSending								=	0,     	
						bReading								=	0;     	
						                                               	
	tsLogPrintItem		*ptLogPrintItem							=	NULL;  	
                                                                       	
	int					hLogLockHandle							=	0;     	
                                                                       	
	tsErrorDataItem		*pErrorDataItem							=	NULL;  	
	
	int					*pThreadHandle							=	NULL;

	int					bNeededComConfig						=	0;
	
	int					bSentSuccessfully						=	0;
							
	double				lfCurrentTime							=	0.0,
						lfStartTime								=	0.0;
	
	int					iCharacterIndex							=	0;
	
	hLocalHandle = *(int*)callbackData;
	CHK_CMT( CmtGetThreadLocalVar ( hLocalHandle , &pLocalVarStore ));
	
	hThreadHandle = pLocalVarStore->thread_handle;
	visaObjHandle = pLocalVarStore->sessionHandle;
	hLogLockHandle = pLocalVarStore->hLogLockHandle;
	
	if ( CmtGetTSQReadPtr ( queueHandle , &pGetVarStore , &iNumberOfItems ) == 0 )
	{
		if ( iNumberOfItems )
		{
			if ( pLocalVarStore->bNeededComConfig && pGetVarStore->pAttributeCallBack )
			{
				CALLOC_ERR( pThreadHandle , 1 , sizeof(int));
				*pThreadHandle = hThreadHandle;
				
				STDERR( PostDeferredCallToThread ( COM_AttributeCallBack , (void*)pThreadHandle , pGetVarStore->hThreadInitiated ));
		
				ProcessSystemEvents();
			}
		
			bUnplugRequest = pGetVarStore->bUnplugRequest;
			bPlugInRequest = pGetVarStore->bPlugInRequest;
			bFlushReceive = pGetVarStore->bFlushReceive;
			bSendRequest = pGetVarStore->bSendRequest; 
			
			memcpy( &tGetVarStore , pGetVarStore , sizeof(tsSingleHandle));
			COM_Support_Copy_Store( pLocalVarStore , pGetVarStore ); 
		}
		
		CmtReleaseTSQReadPtr (queueHandle,1); 
	}

	if ( iNumberOfItems == 0 )
		return;

	GetCurrentDateTime(&lfStartTime);   
	
	do
	{
		GetCurrentDateTime(&lfCurrentTime); 
	
		ProcessSystemEvents();  
		
		if ( CmtGetTSVPtr ( hThreadHandle , &pThreadSafeStore ) == 0 )     
		{
			bNeededComConfig = pThreadSafeStore->bNeededComConfig;
		
			CmtReleaseTSVPtr ( hThreadHandle );
		}
	
		if ( bNeededComConfig == 0 )
			break;
		
	} while((lfCurrentTime-lfStartTime) < 1.0 );
	
	pLocalVarStore->bNeededComConfig = bNeededComConfig;
	
	IF (( bNeededComConfig == 1 ) , "Com Configuration is not finished yet." );
	
	//================ Plug and Unplug =========================================//
	if ( bUnplugRequest )
	{
		if ( visaObjHandle )
		{
			SetBreakOnLibraryErrors(0);
			if ( CmtGetLock (pLocalVarStore->hSessionLock) == 0 ) 
			{
				StdError.error = viClose( visaObjHandle );
				CmtReleaseLock (pLocalVarStore->hSessionLock);
			}
			SetBreakOnLibraryErrors(1);
			visaObjHandle = 0;
			
			pLocalVarStore->bUnpluged = 1;
			
			if (( pLocalVarStore->hLogFileHandle > 0 ) && hLogLockHandle )
			{
				CmtGetLock ( hLogLockHandle );
		
				GetCurrentDateTime( &lfLogCurrentTime );
			
				FormatDateTimeString ( lfLogCurrentTime , "\r\n%H:%M:%S.%3f    Com Unplugged" , szLogFormated , 1024 );
				WriteFile ( pLocalVarStore->hLogFileHandle , szLogFormated , strlen(szLogFormated) ); 
				sprintf( szLogFormated , "     Thread ID = %d\r\n" , CurrThreadId() );
				WriteFile ( pLocalVarStore->hLogFileHandle , szLogFormated , strlen(szLogFormated) );
				
				CmtReleaseLock ( hLogLockHandle );
			}
		}
	}
		
	if ( bPlugInRequest )
	{
		if (( pLocalVarStore->pszVisaAddress ) && ( CmtGetLock (pLocalVarStore->hSessionLock) == 0 )) 
		{
			viOpenDefaultRM (&defaultRM);
			visaObjHandle = 0;
			STDERR( viOpen ( defaultRM , pLocalVarStore->pszVisaAddress , VI_NULL, VI_NULL, &visaObjHandle )); 
			CmtReleaseLock (pLocalVarStore->hSessionLock);
		}
		
		if ( visaObjHandle > 0 )
		{
			if ( IS_OK )
			{
				pLocalVarStore->sessionHandle = visaObjHandle;
				
				pLocalVarStore->bUnpluged = 0;
				pLocalVarStore->bNeededComConfig = 1; 
				
				if (( pLocalVarStore->hLogFileHandle > 0 ) && hLogLockHandle )
				{
					CmtGetLock ( hLogLockHandle );
			
					GetCurrentDateTime( &lfLogCurrentTime );
				
					FormatDateTimeString ( lfLogCurrentTime , "\r\n%H:%M:%S.%3f    Com Plugged In" , szLogFormated , 1024 );
					WriteFile ( pLocalVarStore->hLogFileHandle , szLogFormated , strlen(szLogFormated) ); 
					sprintf( szLogFormated , "     Thread ID = %d\r\n" , CurrThreadId() );
					WriteFile ( pLocalVarStore->hLogFileHandle , szLogFormated , strlen(szLogFormated) );
					
					CmtReleaseLock ( hLogLockHandle );
				}
				
				//---------------------- Reconfig Session -------------------------------//
				viGetAttribute ( visaObjHandle , VI_ATTR_INTF_TYPE , &interfaceType );
					
				if ( interfaceType == VI_INTF_ASRL )
					viSetAttribute (visaObjHandle, VI_ATTR_ASRL_END_IN, VI_ASRL_END_NONE);
				
			}
			
			if ( CmtGetLock (pLocalVarStore->hSessionLock) == 0 ) 
			{
				viClose( visaObjHandle );
				CmtReleaseLock (pLocalVarStore->hSessionLock);
			}
		}
		
		//--------------- Init Failded --------------------------------//
		if (( pLocalVarStore->hLogFileHandle > 0 ) && hLogLockHandle )
		{
			if ( CmtGetLock (pLocalVarStore->hSessionLock) == 0 ) 
			{
				FREE_CALLOC( StdError.pszErrorDescription , STD_STRING , sizeof(char*));
				
				if ( StdError.pszErrorDescription )
					viStatusDesc ( visaObjHandle , StdError.error , (char*)StdError.pszErrorDescription );
				
				CmtReleaseLock (pLocalVarStore->hSessionLock);
			}
			
			CmtGetLock ( hLogLockHandle );
	
			GetCurrentDateTime( &lfLogCurrentTime );
		
			FormatDateTimeString ( lfLogCurrentTime , "\r\n%H:%M:%S.%3f    Plugged In Fail" , szLogFormated , 1024 );
			WriteFile ( pLocalVarStore->hLogFileHandle , szLogFormated , strlen(szLogFormated) ); 
			sprintf( szLogFormated , "     Thread ID = %d\r\n" , CurrThreadId() );
			WriteFile ( pLocalVarStore->hLogFileHandle , szLogFormated , strlen(szLogFormated) );
			sprintf( szLogFormated , "                                 Handle Nr. = %d\r\n" , visaObjHandle );
			WriteFile ( pLocalVarStore->hLogFileHandle , szLogFormated , strlen(szLogFormated) );	
			sprintf( szLogFormated , "                                 Error Nr. = %d\r\n" , StdError.error );
			WriteFile ( pLocalVarStore->hLogFileHandle , szLogFormated , strlen(szLogFormated) );
			
			if ( StdError.pszErrorDescription )
				sprintf( szLogFormated , "                                 Error Ds. = %s\r\n" , StdError.pszErrorDescription );
			
			WriteFile ( pLocalVarStore->hLogFileHandle , szLogFormated , strlen(szLogFormated) );
			
			CmtReleaseLock ( hLogLockHandle );
		}
		
		DelayWithEventProcessing(pLocalVarStore->lfPlugInInitFailDelay);
	}
	
	if ( pLocalVarStore->bUnpluged == 0 )
	{
		//=============================================================================================//
		if ( pLocalVarStore->bEnableLoging && ( pLocalVarStore->hLogFileHandle <= 0 ) && ( pLocalVarStore->bFinishThread == 0 ) && ( pLocalVarStore->bBreakAll == 0 ))
		{
			if (( pLocalVarStore->pszLogFile == NULL ) || ( strlen(pLocalVarStore->pszLogFile) == 0 ))
			{
				if ( pLocalVarStore->pszConnectString && (strlen(pLocalVarStore->pszConnectString)))
					sprintf( szLogFileName , "LOG_%s.txt" ,  pLocalVarStore->pszConnectString );
				else
					sprintf( szLogFileName , "LOG_%d.txt" ,  pLocalVarStore->sessionHandle );	
			}
			else
				strcpy( szLogFileName , pLocalVarStore->pszLogFile );					 
				
			pLocalVarStore->hLogFileHandle = OpenFile ( szLogFileName , VAL_WRITE_ONLY, pLocalVarStore->bLogContinues , VAL_BINARY);
			
			FREE_CALLOC_COPY_STRING( pLocalVarStore->pszLogFile , szLogFileName );
			
			//-------- Loging --------------------//
			if (( pLocalVarStore->hLogFileHandle > 0 ) && hLogLockHandle )
			{
				CmtGetLock ( hLogLockHandle );
				
				GetCurrentDateTime( &lfLogCurrentTime );
					
				FormatDateTimeString ( lfLogCurrentTime , "\r\n%H:%M:%S.%3f    Open Com    :    " , szLogFormated , 1024 );
				WriteFile ( pLocalVarStore->hLogFileHandle , szLogFormated , strlen(szLogFormated) ); 
				if ( pLocalVarStore->pszConnectString )
				{
					sprintf( szLogFormated , "Address = %s\r\n" , pLocalVarStore->pszConnectString );
					WriteFile ( pLocalVarStore->hLogFileHandle , szLogFormated , strlen(szLogFormated) ); 
				}
				sprintf( szLogFormated , "                                 Thread ID = %d\r\n" , CurrThreadId() );
				WriteFile ( pLocalVarStore->hLogFileHandle , szLogFormated , strlen(szLogFormated) );
				
				CmtReleaseLock ( hLogLockHandle );
			}
		}
							
		if (( pLocalVarStore->bEnableLoging == 0 ) && ( pLocalVarStore->hLogFileHandle > 0 )) 
		{
			CloseFile ( pLocalVarStore->hLogFileHandle );
			pLocalVarStore->hLogFileHandle = 0;
			
			if ( hLogLockHandle )
			{
				CmtDiscardLock ( hLogLockHandle );
				hLogLockHandle = 0;
			}
		}
			
		if ( bFlushReceive && ( bReading == 0 ))
		{
			CmtGetLock (pLocalVarStore->hSessionLock);
			
			SetBreakOnLibraryErrors (0);
			viFlush ( visaObjHandle , VI_WRITE_BUF );
			viFlush ( visaObjHandle , VI_READ_BUF );
			SetBreakOnLibraryErrors (1);
			CmtReleaseLock (pLocalVarStore->hSessionLock);
			
			bFlushReceive = 0;

			//-------------------------------------------------------------// 	
		}
		
		if ( bSendRequest )
		{  
			//---------------------------------- Send --------------------------------------------------//
			//-------- Update info ---------------------------------//
		
			if (( tGetVarStore.pSendBuffer ) && ( tGetVarStore.sendBufferSize ))
			{
				if ( tGetVarStore.sendBufferSize )
				{
					FREE( pLocalSendBuffer );
			
					iLocalSendSize = (unsigned int)tGetVarStore.sendBufferSize;
				
					CALLOC( pLocalSendBuffer , iLocalSendSize+1 , sizeof(char));
					
					IF (( pLocalSendBuffer == NULL ) , "Can't allocate memory" );
				
					memcpy( pLocalSendBuffer , tGetVarStore.pSendBuffer , (size_t)iLocalSendSize ); 
					bSending = 1;   
				}
			
			}
			else
			{
				pLocalVarStore->sendStatus = -1;	
				StdError.error = -1;
				FREE_CALLOC_COPY_STRING((char*)StdError.pszErrorDescription , "No data to send" );
				bSending = 0;
			}
			
			//-------------------------------------------------------------//
			//--------------- Sending -------------------------------------//
			if ( bSendRequest && bSending )
			{
				if ( CmtGetLock (pLocalVarStore->hSessionLock) == 0 ) 
				{
					viSetAttribute ( visaObjHandle , VI_ATTR_TMO_VALUE , pLocalVarStore->iLowLevelSendTimeOut );
					
					if ( pLocalVarStore->bTypingMode == 0 )
					{
						StdError.error = viWrite ( visaObjHandle , pLocalSendBuffer , (ViUInt32)iLocalSendSize , &iSendCount );
					}
					else
					{
						for ( iCharacterIndex = 0; iCharacterIndex < iLocalSendSize; iCharacterIndex++ )
						{
							StdError.error = viWrite ( visaObjHandle , ((char*)&(pLocalSendBuffer[iCharacterIndex])) , 1 , &iSendSingleCount ); 	
							
							if ( iSendSingleCount )
								iSendCount++;
							
							if ( IS_NOT_OK )
								break;
							
							if ( pLocalVarStore->lfTypingModeDelay > 0.0 )
								DelayWithEventProcessing( pLocalVarStore->lfTypingModeDelay );
						}
					}
			
					viSetAttribute ( visaObjHandle , VI_ATTR_TMO_VALUE , pLocalVarStore->iLowLevelReceiveTimeOut );
					
					bSentSuccessfully = !(StdError.error);
						
					CmtReleaseLock (pLocalVarStore->hSessionLock);
				}
				
				//-------- Loging --------------------//
				if ( pLocalVarStore->hLogFileHandle > 0 )
				{
					CALLOC( ptLogPrintItem , 1 , sizeof(tsLogPrintItem));
					
					if ( ptLogPrintItem )
					{
						ptLogPrintItem->bWrite_read = 1;
						ptLogPrintItem->iBufferSize = iSendCount;
						CALLOC( ptLogPrintItem->pBuffer , iSendCount+1 , 1 );
						ptLogPrintItem->hLockHandle = hLogLockHandle;
						ptLogPrintItem->hFileHandle = pLocalVarStore->hLogFileHandle;
						ptLogPrintItem->bAsciiPrint = pLocalVarStore->bLogFormatIsAscii; 
						
						if ( ptLogPrintItem->pBuffer )
						{  
							if ( iSendCount )
								memcpy( ptLogPrintItem->pBuffer , pLocalSendBuffer , iSendCount );
						
							PostDeferredCall( COM_LogPrintCallBack , ptLogPrintItem );
							ProcessSystemEvents();
								
						}
						else
							FREE( ptLogPrintItem );
					}
					
				}
				
				FREE( pLocalSendBuffer );  
				
				if ( StdError.error )
				{
					FREE_CALLOC( StdError.pszErrorDescription , STD_STRING , sizeof(char*));
				
					if ( StdError.pszErrorDescription )
						viStatusDesc ( visaObjHandle , StdError.error , (char*)StdError.pszErrorDescription );
				}
				else
					if ( iLocalSendSize == iSendCount )
						pLocalVarStore->sendStatus = 0;
					else
					{
						pLocalVarStore->sendStatus = -2;	
						StdError.error = -2;
						FREE_CALLOC_COPY_STRING((char*)StdError.pszErrorDescription , "sent data is corrupt" );	
					}
			}
		}
	}

Error:
	
	if ( StdError.error ) 
	{
		FREE_CALLOC_COPY( pThreadSafeStore->pLastError , 1 , sizeof(STD_ERROR) , &StdError , sizeof(STD_ERROR) );

		//-------- Loging --------------------//
		if (( pLocalVarStore->hLogFileHandle > 0 ) && hLogLockHandle )
		{
			CmtGetLock ( hLogLockHandle );
	
			GetCurrentDateTime( &lfLogCurrentTime );
		
			FormatDateTimeString ( lfLogCurrentTime , "\r\n%H:%M:%S.%3f    Error Com    :    " , szLogFormated , 1024 );
			WriteFile ( pLocalVarStore->hLogFileHandle , szLogFormated , strlen(szLogFormated) ); 
			sprintf( szLogFormated , "                                 Thread ID = %d\r\n" , CurrThreadId() );
			WriteFile ( pLocalVarStore->hLogFileHandle , szLogFormated , strlen(szLogFormated) );
			sprintf( szLogFormated , "                                 Error Nr. = %d\r\n" , StdError.error );
			WriteFile ( pLocalVarStore->hLogFileHandle , szLogFormated , strlen(szLogFormated) );
			
			if ( StdError.pszErrorDescription )
				sprintf( szLogFormated , "                                 Error Ds. = %s\r\n" , StdError.pszErrorDescription );
			
			WriteFile ( pLocalVarStore->hLogFileHandle , szLogFormated , strlen(szLogFormated) );
			
			CmtReleaseLock ( hLogLockHandle );
		}
		//-------------- Execute Error Callback ------------------//
		CALLOC( pErrorDataItem , 1 , sizeof(tsErrorDataItem));

		memcpy ( &(pErrorDataItem->ret) , &( StdError ) , sizeof(STD_ERROR));
	
		pErrorDataItem->iSessionHandle = visaObjHandle;
		pErrorDataItem->pErrorFunction = pLocalVarStore->pErrorCallBack;
		pErrorDataItem->pData = pLocalVarStore->pErrorCallBackData; 

		PostDeferredCallToThread ( COM_ErrorCallBack , pErrorDataItem , pLocalVarStore->hThreadInitiated ); 
		ProcessSystemEvents();
		//----------------------------------------------------------//
	}

	//-------- Update info ---------------------------------//  
	if ( CmtGetTSVPtr ( hThreadHandle , &pThreadSafeStore ) == 0 )
	{
		if ( pThreadSafeStore->bUnplugRequest && pLocalVarStore->bUnpluged )
			pThreadSafeStore->bUnplugRequest = 0;
			
		if ( pThreadSafeStore->bPlugInRequest && ( pLocalVarStore->bUnpluged == 0 ))
		{
			pThreadSafeStore->sessionHandle = visaObjHandle;
			pThreadSafeStore->bNeededComConfig = pLocalVarStore->bNeededComConfig;
			pThreadSafeStore->bPlugInRequest = 0;
		}
		
		if ( pThreadSafeStore->bFlushReceive )
			pThreadSafeStore->bFlushReceive = bFlushReceive;
		
		if ( pThreadSafeStore->hLogFileHandle == 0 )
			pThreadSafeStore->hLogFileHandle = pLocalVarStore->hLogFileHandle; 
			
		pThreadSafeStore->receiveBreakCount = 0;
		pThreadSafeStore->bUnpluggedStatus = pLocalVarStore->bUnpluged;
		pThreadSafeStore->bSendRequest = 0;
		pThreadSafeStore->bSentSuccessfully = bSentSuccessfully;
		pThreadSafeStore->sendStatus = pLocalVarStore->sendStatus;
		GetCurrentDateTime(&pThreadSafeStore->lfSendTime );
		
		CmtReleaseTSVPtr (hThreadHandle);
	}
	//-------------------------------------------------------------// 
	
	FREE( tGetVarStore.pszConnectString);        
	FREE( tGetVarStore.pSendBuffer);              
	FREE( tGetVarStore.pszTerminatingPattern);    
	FREE( tGetVarStore.pszLogFile);               
	FREE( tGetVarStore.pszVisaAddress); 
	
	return;
}
//==================================== API Interface Callbacks =====================================================//

void		COM_LogPrintCallBack( void *pData )
{
	STD_ERROR					StdError					=	{0};
	
	tsLogPrintItem				*ptLogPrintItem				=	NULL;
	
	char						*pszLogFormated				=	NULL;
	
	double						lfLogCurrentTime			=	0.0;
	
	char						*pReplaced					=	NULL, 
								*pTempReplaced				=	NULL, 
								*pLocalBuffer				=	NULL,
								*pToPrint					=	NULL,  
								*pNext						=	NULL,
								*pTemp						=	NULL;
							
	int							index						=	0,
								bFoundAnyAscii				=	0,  
								bFoundAnyWhite				=	0, 
								bFirstTime					=	0;
	
	unsigned char				character					=	0;
	
	char						szTermination[8]			=	{0};
	
	ptLogPrintItem = (tsLogPrintItem*) pData;
	
	if ( ptLogPrintItem == NULL )
		return;
	
	bFirstTime = 1;
	
	if ( ptLogPrintItem->hFileHandle && ptLogPrintItem->hLockHandle )
	{
		GetCurrentDateTime( &lfLogCurrentTime ); 
		
		CHK_CMT( CmtGetLock ( ptLogPrintItem->hLockHandle ));
		
		CALLOC_ERR( pszLogFormated , (ptLogPrintItem->iBufferSize + 1024) , sizeof(char) ); 
		
		if ( ptLogPrintItem->bWrite_read ) 
			FormatDateTimeString ( lfLogCurrentTime , "\r\n%H:%M:%S.%3f    Sent        :    " , pszLogFormated , 1023 );
		else
			FormatDateTimeString ( lfLogCurrentTime , "\r\n%H:%M:%S.%3f    Received    :    " , pszLogFormated , 1023 );
			
		WriteFile ( ptLogPrintItem->hFileHandle , pszLogFormated , strlen(pszLogFormated) ); 

		pLocalBuffer = ptLogPrintItem->pBuffer;
		
		if ( pLocalBuffer )
		{
			for( index = 0; index < ptLogPrintItem->iBufferSize; index++ ) 
			{
				if ( pLocalBuffer[index] >= ' ' )
					bFoundAnyAscii = 1;
				
				if ( pLocalBuffer[index] < 20 )
					bFoundAnyWhite = 1;
				
				if ( ptLogPrintItem->bAsciiPrint && bFoundAnyAscii && ( pLocalBuffer[index] == 0 ))
					pLocalBuffer[index] = ' ';
			}
				
			if ( ptLogPrintItem->bAsciiPrint && bFoundAnyAscii )
			{
				pNext = pLocalBuffer;
				
				while(pNext)
				{
					if ( strlen(pNext) == 0 )
						break;
					
					szTermination[0] = 0;
					
					pTemp = strchr( pNext , '\r' );
					
					if ( pTemp )
					{
						if ( pTemp[1] == '\n' )
						{
							strcpy( szTermination , "\\n" );
							pTemp++;
						}
						else
						{
							strcpy( szTermination , "\\r" );	
						}
						
						*pTemp = 0;
					}
					else
					{
						pTemp = strchr( pNext , '\n' );	
						
						if ( pTemp )
						{
							strcpy( szTermination , "\\n" );
							*pTemp = 0;  
						}
					}
					
					if ( bFirstTime )
					{
						bFirstTime = 0;
						
						pToPrint =  pNext;
						
						if ( bFoundAnyWhite )
						{
							pReplaced = StringReplaceAll( pNext , "\r" , "\\r" , NULL );
						
							pTempReplaced = pReplaced;
						
							pReplaced = StringReplaceAll( pTempReplaced , "\n" , "\\n" , NULL ); 
							
							pToPrint = pReplaced;
						}
						
						sprintf( pszLogFormated , "%s%s\r\n" , pToPrint , szTermination );
						
						FREE( pTempReplaced );
						FREE( pReplaced );
					}
					else
					{
						pToPrint =  pNext;
						
						if ( bFoundAnyWhite )
						{
							pReplaced = StringReplaceAll( pNext , "\r" , "\\r" , NULL );
						
							pTempReplaced = pReplaced;
						
							pReplaced = StringReplaceAll( pTempReplaced , "\n" , "\\n" , NULL ); 
							
							pToPrint = pReplaced;
						}
						
						sprintf( pszLogFormated , "                                 %s%s\r\n" , pToPrint , szTermination );
						
						FREE( pTempReplaced );
						FREE( pReplaced );
					}
					
					WriteFile ( ptLogPrintItem->hFileHandle , pszLogFormated , strlen(pszLogFormated) );
					
					if ( pTemp )
					{
						pTemp++;
						
						if ( *pTemp == '\n' )
							pTemp++; 
							
						pNext = pTemp;
					}
					else
						pNext = NULL;
				}
			}
			else
			{
				for( index = 0; index < ptLogPrintItem->iBufferSize; index++ )
				{
					character = (char)ptLogPrintItem->pBuffer[index];
					sprintf( pszLogFormated , "0x%02X " , character );
					WriteFile ( ptLogPrintItem->hFileHandle , pszLogFormated , strlen(pszLogFormated) );
				}
				
				sprintf( pszLogFormated , " ( %d Bytes )" , ptLogPrintItem->iBufferSize );
				WriteFile ( ptLogPrintItem->hFileHandle , pszLogFormated , strlen(pszLogFormated) );
			}
		}
		
		CmtReleaseLock ( ptLogPrintItem->hLockHandle );
	}

Error:
	
	FREE( pszLogFormated );
	FREE( ptLogPrintItem->pBuffer );
	FREE( ptLogPrintItem );
	
	return;
}
			
			
void		COM_ReceiveCallBack( void *pData )
{
	tsReceiveDataItem		*pReceiveDataItem						=	NULL;

	pfReceiveCallBack		pCallBackFunction						=	NULL;
	
	char					*pBuffer								=	NULL;
	
	pReceiveDataItem = (tsReceiveDataItem*)pData;
	
	if ( pReceiveDataItem == NULL )
		return;
	
	pCallBackFunction = (pfReceiveCallBack)pReceiveDataItem->pCallBackFunction;
	
	if ( pCallBackFunction && pReceiveDataItem->pBuffer )
	{
		pCallBackFunction( pReceiveDataItem->pBuffer , pReceiveDataItem->receiveCount , pReceiveDataItem->pReceiveCallBackData );  
	}
	
	return;
}


void		COM_AttributeCallBack( void *pData )
{
	tsSingleHandle			*pThreadSafeStore							=	NULL;
	
	CmtTSVHandle 			handle										=	0;

	int						*pHandle									=	NULL;

	pfAttributeCallBack		pCallBackFunction							=	NULL;
	
	int						iError										=	0;
	
	pHandle = (int*) pData;
	
	if ( pHandle == NULL )
		return;
	
	handle = *pHandle;
	
	FREE(pHandle);
	
	//--------------- Initialyzing --------------------------//
	if ( CmtGetTSVPtr ( handle , &pThreadSafeStore ) != 0 )
		return;
	
	if ( pThreadSafeStore->bFinishThread == 0 )
	{
		pCallBackFunction = (pfAttributeCallBack)pThreadSafeStore->pAttributeCallBack;
	
		if ( pCallBackFunction )
			iError = pCallBackFunction( pThreadSafeStore->sessionHandle , pThreadSafeStore->pAttributeCallBackData );
	}
	
	if ( iError == 0 )
		pThreadSafeStore->bNeededComConfig = 0;
	
	CmtReleaseTSVPtr (handle); 

	return;
}

void		COM_CloseCallBack( void *pData )
{
	tsCloseDataItem			*pCloseDataItem							=	NULL;

	pfCloseCallBack			pCallBackFunction						=	NULL;
	
	pCloseDataItem = (tsCloseDataItem*) pData;
	
	if ( pCloseDataItem == NULL )
		return;

	pCallBackFunction = (pfCloseCallBack)pCloseDataItem->pCloseFunction;
	
	if ( pCallBackFunction )
		pCallBackFunction((int) pCloseDataItem->iSessionHandle , (int) pCloseDataItem->iError , (void*)pCloseDataItem->pData );
		
	FREE(pCloseDataItem);
	
	return;
}

void		COM_ErrorCallBack( void *pData )
{
	STD_ERROR					*pRet									=	NULL;
	
	tsErrorDataItem			*pErrorDataItem							=	NULL;

	pfErrorCallBack			pCallBackFunction						=	NULL;
	
	pErrorDataItem = (tsErrorDataItem*) pData;
	
	if ( pErrorDataItem == NULL )
		return;

	pCallBackFunction = (pfErrorCallBack)pErrorDataItem->pErrorFunction;
	
	CALLOC( pRet , 1 , sizeof(STD_ERROR));
	
	if ( pRet )
	{
		memcpy( pRet , &pErrorDataItem->ret , sizeof(STD_ERROR));
		
		if ( pCallBackFunction )
			pCallBackFunction((int) pErrorDataItem->iSessionHandle , (void*)pRet , (void*)pErrorDataItem->pData );
		else
			FREE(pRet); 
	}
	
	FREE(pErrorDataItem);
	
	return;
}

void CVICALLBACK COM_FinishThread (CmtThreadPoolHandle poolHandle, CmtThreadFunctionID functionID, unsigned int event, int value, void *callbackData)
{
	STD_ERROR			StdError								=	{0};
	
	tsSingleHandle		*pThreadSafeStore						=	{0},
						tLocalStore								=	{0};
	
	CmtTSVHandle 		handle									=	0;
	
	int					*pHandle								=	NULL;

	int					iActiveThreads							=	0;

	pHandle = (int*) callbackData;
	
	if ( pHandle == NULL )
		return;
	
	handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pThreadSafeStore ));  
	memcpy( &tLocalStore , pThreadSafeStore , sizeof(tsSingleHandle));
	
Error:
	
	if ( handle )
	{
		CmtReleaseTSVPtr (handle);
		CmtDiscardTSV (handle);  
		
		CmtDiscardLock (tLocalStore.hSessionLock);
		
		FREE(tLocalStore.pszLogFile);
		FREE(tLocalStore.pSendBuffer);
		FREE(tLocalStore.pszConnectString); 
		FREE(tLocalStore.pszVisaAddress);
	}
	
	FREE(pHandle); 
		
	return;
}


char		*StringReplaceAll( char *szSourceString , char *szFindWhat , char *szReplaceWith , int *pCount )
{
	STD_ERROR				StdError							=	{0};
	
	char				*pOutputString				=	NULL,
						*pInputString				=	NULL,
						*pPointer					=	NULL;
	
	int					count						=	0,
						index						=	0,
						iCountIndex					=	0,
						next_index					=	0,
						newLength					=	0,
						dynamicSize					=	0;
	
	int					*piLocationList				=	NULL,
						*piTemp						=	NULL;
	
	IF (( szSourceString == NULL ) , "Pointer can't be null.");
	IF (( szFindWhat == NULL ) , "Pointer can't be null.");
	IF (( szReplaceWith == NULL ) , "Pointer can't be null.");
	
	FREE_CALLOC_COPY_STRING_ERR( pInputString , szSourceString );
	dynamicSize = 1024;
	CALLOC_ERR( piLocationList , dynamicSize + 1 , sizeof(int));
	
	do 
	{
		pPointer = strstr( pInputString + index , szFindWhat ); 
		
		if ( pPointer )
		{
			index = pPointer - pInputString + strlen(szFindWhat);
			count++;
			*pPointer = 0;
			
			if ( count > dynamicSize )
			{
				dynamicSize *= 2;
				
				if ( dynamicSize < count)
					dynamicSize += count;
				
				CALLOC_COPY_ERR( piTemp , dynamicSize + 1 , sizeof(int) , piLocationList , count ); 
				FREE( piLocationList );
				piLocationList = piTemp;
			}
			
			piLocationList[count] = index;
		}
		
	}while(pPointer);
	
	if ( pCount )
		*pCount = count;
	
	newLength = strlen(szSourceString) + (( count + 1 ) * strlen(szReplaceWith));
	
	if ( count )
	{
		CALLOC_ERR( pOutputString , newLength , sizeof(char));
	
		for ( iCountIndex = 0 ; iCountIndex < count ; iCountIndex++ )
		{
			index = piLocationList[iCountIndex];
			next_index += sprintf ( pOutputString + next_index , "%s%s" , pInputString + index , szReplaceWith );
		}

		index = piLocationList[iCountIndex];
		sprintf ( pOutputString + next_index , "%s" , pInputString + index );   
	}
	
Error:
	
	if ( pOutputString == NULL )
	{
		CALLOC_COPY_STRING(pOutputString,szSourceString);
	}
	
	FREE(pInputString);
	FREE(piLocationList);
	
	return pOutputString;
}
