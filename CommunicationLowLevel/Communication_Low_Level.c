//==============================================================================
// Include files
#include "windows.h"
#include <userint.h>
#include "toolbox.h"
#include <utility.h>
#include <visa.h>
#include "GlobalDefines.h"
#include "Communication_Thread.h"
#include "Communication_Headers.h"
#include "Communication_Low_Level.h"  
#include <formatio.h>

//==============================================================================
// Static global variables

typedef struct
{
	unsigned int				iType;				// 0 - Connection String , 1 - Hardware Device Name
		
	char						szSpare[32];
	
} tsConfigCommunication;

//==============================================================================
// Static functions
void		COM_Support_Copy_Store( tsSingleHandle *pTargetStore , tsSingleHandle *pSourceStore );
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
			
            break;
    }
    
    return 1;
}

int __stdcall DllEntryPoint (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    /* Included for compatibility with Borland */

    return DllMain (hinstDLL, fdwReason, lpvReserved);
}


void* DLLEXPORT	Communication_Init( char *pszConnectionName , char *pszAddress , int *phHandle , ... )
{
	STD_ERROR						StdError								=	{0};

	tsSingleHandle					*pLocalStore							=	{0};
	
	CmtTSVHandle 					handle									=	0;
	
	int								hDatabaseConnectionHandle				=	0;
	
	ViSession						visaObjHandle							=	0;
		
	int								*pHandle								=	NULL;
	
	char							szVisaAddress[STD_STRING]				=	{0},
									szHardwareName[STD_STRING]				=	{0};
	
	char							szLog[LOW_STRING]						=	{0};
	
	//tsConnectionConfig				tDatabaseConnectionConfigure			=	{0};
	
	tsConfigCommunication			config									=	{0};
	
	ViFindList 						hFindHandle								=	0;
		
	ViUInt32						iReturn_Count							=	0,
									iDeviceIndex							=	0;
	
	int								status									=	0;
	
	int								bBreak									=	0;
										
	int								bDeviceFound							=	0,
									bInvalidHandle							=	0,
									bHandleExists							=	0,
									bGetHandle								=	0;
	
	double							lfStartTime								=	0.0,
									lfCurrentTime							=	0.0,
									lfStartThreadTimeout					=	300.0;
	
	va_list							vaExtraParameters						=	{0};
	
	int								iParameterCounter						=	0;
	
	unsigned int					uiValue									=	0;
	
	int								initStatus								=	0;
	
	ViSession						defaultRM								=	0;
	
	va_start(vaExtraParameters,phHandle);   

	IF( ( phHandle == NULL ) , "Handle Pointer can't be NULL." );
	
	*phHandle = 0;

	viOpenDefaultRM (&defaultRM);  
	
	//================= Try connect to Databse =======================================//
	//INFO_Connect( NULL , &tDatabaseConnectionConfigure , &hDatabaseConnectionHandle );

	while((*(long*)vaExtraParameters) != LASTPARAM )
	{
		switch(iParameterCounter)
		{
			case 0:
						uiValue = va_arg( vaExtraParameters , int);				// 0 - Connection String , 1 - Hardware Device Name
						
						if ( uiValue > 1 ) 
						{
							bBreak = 1;
							break;
						}
							
						config.iType = uiValue;
						
						break;
			default:
						bBreak = 1;
						break;
		}

		if ( bBreak )
			break;
		
		iParameterCounter++;
	};
		
	if ( pszConnectionName )
		sprintf( szLog , "No connection's address found for [%s]" , pszConnectionName );
	
	IF ((pszAddress == NULL) , szLog );
	IF (((pszAddress == NULL)&&(pszConnectionName == NULL)) , "No connection's information found." ); 
	 
	//------------ Check if resurse exists -------------//
	
	SetBreakOnLibraryErrors (0); 
	
	switch(config.iType)
	{
		case 0:
			strcpy( szVisaAddress , (const char *)pszAddress );
			break;
			
		case 1: //---- Hardware name ----------------//
			
			status = viFindRsrc ( defaultRM , "?*" , &hFindHandle , &iReturn_Count , szVisaAddress );
			
			for( iDeviceIndex = 0 ; (( iDeviceIndex < iReturn_Count ) && ( status == 0 )) ; iDeviceIndex++ )
			{
				if ( viOpen ( defaultRM , szVisaAddress , VI_NULL, VI_NULL , &visaObjHandle ) == 0 )
				{
					status = viGetAttribute ( visaObjHandle , VI_ATTR_INTF_INST_NAME , szHardwareName );
						
					viClose (visaObjHandle);
					visaObjHandle=0;
				
					//-----------------------------------------------------------------//
					if ( status == 0 )
						if ( strstr( szHardwareName , (const char *)pszAddress ) )
						{
						//	strcpy( (char *)szAddress , (const char *)szVisaAddress );
							
							bDeviceFound = 1;
							
							break;	
						}

				}
				
				status = viFindNext ( hFindHandle , szVisaAddress );
			}
			
			IF (( bDeviceFound == 0 ) ,"No device found" ); 
			IF ( (strlen(szVisaAddress)==0),"No device found" );
			
			break;
			
		default:
			strcpy( szVisaAddress , (const char *)pszAddress );  
			break;
	}
	
	SetBreakOnLibraryErrors (1); 
	
	if (( handle == 0 ) || ( bInvalidHandle ))
	{
		CHK_CMT( CmtNewTSV ( sizeof(tsSingleHandle) , &handle ));
	
		*phHandle = handle;
	}
	else
		bHandleExists = 1;  

	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStore ));
	bGetHandle = 1;
	
	if ( bHandleExists == 0 )
	{
		memset( pLocalStore , 0 , sizeof(tsSingleHandle));
	
		pLocalStore->hThreadInitiated = CurrThreadId();
		pLocalStore->lfSendTimeout = 10.0;
	
		pLocalStore->iLowLevelReceiveTimeOut = 1000;
		pLocalStore->iLowLevelSendTimeOut = 1000;
	
		pLocalStore->iReadBlockSize = 1;

		FREE_CALLOC_COPY_STRING_ERR( pLocalStore->pszConnectString , szVisaAddress );
	}
	
	if ( pLocalStore->sessionHandle )
		viClose( pLocalStore->sessionHandle );
		
	CALLOC_ERR( pHandle , 1 , sizeof(int));
		
	IF( ( pHandle == NULL ) , "Can't allocate Memory." );
		
	*pHandle = handle;
	
	CHK_CMT( CmtReleaseTSVPtr ( handle ));
	bGetHandle = 0;

	CHK_CMT( CmtScheduleThreadPoolFunctionAdv ( DEFAULT_THREAD_POOL_HANDLE , COM_Thread , (void*)pHandle , THREAD_PRIORITY_NORMAL , COM_FinishThread , EVENT_TP_THREAD_FUNCTION_END , (void*)pHandle , CmtGetCurrentThreadID() , NULL ));

	ProcessSystemEvents();
	
	GetCurrentDateTime(&lfStartTime); 
	
	while(( initStatus == 0 ) && ( IS_OK ))
	{
		GetCurrentDateTime(&lfCurrentTime);
		
		if (( lfCurrentTime - lfStartTime ) > lfStartThreadTimeout )
		{
			FORCE_ERR( -1 , "Can't start thread" );
		}
		
		ProcessSystemEvents();
		
		DelayWithEventProcessing(0.5);
		
		if( CmtGetTSVPtr ( handle , &pLocalStore ) == 0 )
		{
			FREE_STDERR_COPY( pLocalStore->pLastError); 
			
			initStatus = pLocalStore->initStatus;
			
			CmtReleaseTSVPtr ( handle ); 
		}
		else
		{
			SET_ERR( -1, "Can't start communication." );
			break;	
		}
	}
	
Error:
	
	va_end(vaExtraParameters);
	
	SetBreakOnLibraryErrors (0);

	if ( handle && bGetHandle )
		CmtReleaseTSVPtr ( handle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}


void* DLLEXPORT	Communication_Close( int hHandle )
{
	STD_ERROR				StdError						=	{0};
	
	tsSingleHandle			*pLocalStore					=	{0},
							*pQueueStore					=	NULL;
	
	CmtTSVHandle 			handle							=	0;
	
	int						iConnectionCounter				=	0, 
							iEmptyItems						=	0;
								
	double					lfStartTime						=	0.0,
							lfCurrentTime					=	0.0;
	
	int						hThreadPoolHandle				=	0;
	
	int						iActiveThreads					=	0;
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );
	
	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	pLocalStore->bBreakAll = 1;
	pLocalStore->bFinishThread = 1;
	
	if( CmtGetTSQWritePtr ( pLocalStore->queue_handle , &pQueueStore , &iEmptyItems ) == 0 )
	{
		memset( pQueueStore , 0 , sizeof(tsSingleHandle));
		COM_Support_Copy_Store( pQueueStore , pLocalStore ); 
		
		CmtReleaseTSQWritePtr ( pLocalStore->queue_handle , 1 );
	}
	
	CHK_CMT( CmtReleaseTSVPtr (hHandle));
		
	SetBreakOnLibraryErrors (1);
	
	ProcessSystemEvents();
	DelayWithEventProcessing(0.5);

Error:
	
	SetBreakOnLibraryErrors (1);
	
	ProcessSystemEvents();
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT	Communication_GetLastError( int hHandle )
{
	STD_ERROR				StdError						=	{0};

	tsSingleHandle			*pLocalStore					=	{0};
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	
	FREE_STDERR_COPY(pLocalStore->pLastError);
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	RETURN_STDERR_POINTER;	
}


void* DLLEXPORT	Communication_Send( int hHandle , unsigned char *pBuffer , unsigned int iSendCount , unsigned int  iReceiveBreakCount )
{
	STD_ERROR				StdError						=	{0},
							last_StdError					=	{0};

	tsSingleHandle			*pLocalStore					=	NULL,
							*pQueueStore					=	NULL;
							
	double					lfStartTime						=	0.0,
							lfCurrentTime					=	0.0;
	
	int						iConnectionCounter				=	0;
		
	char					bBreak							=	0,
							bGetVariable					=	0;
	
	int						initStatus						=	0;
	
	int						iEmptyItems						=	0;
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  
	IF( ( pBuffer == NULL ) , "Buffer Pointer can't be NULL." );
	IF( ( iSendCount == 0 ) , "Count can't be 0." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	bGetVariable = 1; 
	
	initStatus = pLocalStore->initStatus;

	IF (( initStatus != 1 ) , "Communication is not initiated" );  
	
	IF (( pLocalStore->bUnpluggedStatus )  , "Communication is unplugged" );  
		
	FREE_CALLOC_COPY_ERR( pLocalStore->pSendBuffer , iSendCount , 1 , pBuffer , iSendCount );  
	
	pLocalStore->sendBufferSize = iSendCount;
	
	pLocalStore->receiveBreakCount = iReceiveBreakCount;
	
	pLocalStore->bSentSuccessfully = 0;
	pLocalStore->bSendRequest = 1;

	CHK_CMT( CmtGetTSQWritePtr ( pLocalStore->queue_handle , &pQueueStore , &iEmptyItems )); 
	memset( pQueueStore , 0 , sizeof(tsSingleHandle));
	COM_Support_Copy_Store( pQueueStore , pLocalStore ); 
	CHK_CMT( CmtReleaseTSQWritePtr ( pLocalStore->queue_handle , 1 ));
	
	CHK_CMT(CmtReleaseTSVPtr ( hHandle ));
	bGetVariable = 0;
	
	ProcessSystemEvents(); 
	
	if ( pLocalStore->bDontWaitForSendStatus == 0 )
	{
		CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
		bGetVariable = 1;
		
		GetCurrentDateTime(&lfStartTime);
		GetCurrentDateTime(&lfCurrentTime);
	
		while (( pLocalStore->bSentSuccessfully == 0 ) && (( lfCurrentTime - lfStartTime ) < pLocalStore->lfSendTimeout ))
		{
			GetCurrentDateTime(&lfCurrentTime);  
			CHK_CMT(CmtReleaseTSVPtr ( hHandle ));
			bGetVariable = 0;
			ProcessSystemEvents(); 
			DelayWithEventProcessing(0.3);
			ProcessSystemEvents();  
			CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
			bGetVariable = 1;
		}
		
		FREE_STDERR_COPY_ERR( pLocalStore->pLastError );
		
		CHK_CMT(CmtReleaseTSVPtr ( hHandle ));  
		bGetVariable = 0;
	}
		
	hHandle = 0;
	
Error:

	if ( hHandle && bGetVariable )
		CmtReleaseTSVPtr ( hHandle );
	
	RETURN_STDERR_POINTER;	
}
/*
void* DLLEXPORT	Communication_Receive( int hHandle , double lfTimeOut , unsigned char **pBuffer , int *piCount )
{
	STD_ERROR						StdError						=	{0},
									last_StdError					=	{0};

	tsSingleHandle					*pLocalStore					=	{0};
	
	double							lfStartTime						=	0.0,
									lfCurrentTime					=	0.0;
	
	int								iConnectionCounter				=	0;
		
	char							bBreak							=	0,
									bGetVariable					=	0,
									bLocked							=	0;
	
	static int						hReceiveLock					=	0; 
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  
	IF( ( pBuffer == NULL ) , "Buffer Pointer can't be NULL." );
	IF( ( piCount == 0 ) , "Count Pointer can't be NULL." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	bGetVariable = 1;
	
	IF (( pLocalStore->initStatus != 1 ) , "Communication is not initiated" );  
	
	hReceiveLock = pLocalStore->hReceiveLock; 
	
	CHK_CMT(CmtReleaseTSVPtr ( hHandle ));
	bGetVariable = 0;  
	
	CHK_CMT(CmtGetLock (hReceiveLock));
	bLocked = 1;
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	bGetVariable = 1;
	
	if ( pLocalStore->bReceiveEnable == 0 )
	{
		memset( &pLocalStore->lastError , 0 , sizeof(STD_ERROR));  
		pLocalStore->bReceiveEnable = 1;
	}
	
	GetCurrentDateTime(&lfStartTime);
	GetCurrentDateTime(&lfCurrentTime);
	
	while (( pLocalStore->bReceiveDataPresent == 0 ) && ( ( lfCurrentTime - lfStartTime ) < lfTimeOut ))
	{
		CHK_CMT(CmtReleaseTSVPtr ( hHandle ));
		bGetVariable = 0;
		CHK_CMT(CmtReleaseLock ( hReceiveLock ));
		bLocked = 0; 
		
		ProcessSystemEvents(); 
		DelayWithEventProcessing(0.2);
		
		CHK_CMT(CmtGetLock (hReceiveLock));
		bLocked = 1;
		CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
		bGetVariable = 1;
		GetCurrentDateTime(&lfCurrentTime); 
	}

	IF (( pLocalStore->bReceiveDataPresent == 0 ) , "No Data Received." );
	
	CALLOC_ERR( *pBuffer , pLocalStore->receiveCount + 1 , sizeof(char) );
	
	*piCount = pLocalStore->receiveCount;
		
	IF( ( *pBuffer == NULL ) , "Can't allocate memory." ); 
	
	if ( pLocalStore->pReceiveBuffer && pLocalStore->receiveCount )
		memcpy(*pBuffer,pLocalStore->pReceiveBuffer,pLocalStore->receiveCount);
	
	pLocalStore->bReceiveDataPresent = 0; 	
	
	CHK_STDERR(pLocalStore->lastError);
	
	hHandle = 0;
	
Error:

	if ( hHandle && bGetVariable )
		CmtReleaseTSVPtr ( hHandle );
	
	if ( bLocked && hReceiveLock )
		CmtReleaseLock ( hReceiveLock ); 	
		
	RETURN_STDERR_POINTER;	
}
*/
void* DLLEXPORT	Communication_Unplug( int hHandle , double lfTimeOut )
{
	STD_ERROR						StdError						=	{0};

	double							lfStartTime						=	0.0,
									lfCurrentTime					=	0.0;
	
	int								bGetVariable					=	0;
	
	tsSingleHandle					*pLocalStore					=	NULL,
									*pQueueStore					=	NULL;
	
	int								iEmptyItems						=	0;
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	bGetVariable = 1;
	
	IF (( pLocalStore->bUnpluggedStatus ) , "Already Unplugged" );
	
	pLocalStore->bUnplugRequest = 1;  
	
	CHK_CMT( CmtGetTSQWritePtr ( pLocalStore->queue_handle , &pQueueStore , &iEmptyItems )); 
	memset( pQueueStore , 0 , sizeof(tsSingleHandle));
	COM_Support_Copy_Store( pQueueStore , pLocalStore ); 
	CHK_CMT( CmtReleaseTSQWritePtr ( pLocalStore->queue_handle , 1 ));
	
	ProcessSystemEvents(); 
	
	GetCurrentDateTime(&lfStartTime);
	GetCurrentDateTime(&lfCurrentTime);
	
	while (( pLocalStore->bUnpluggedStatus == 0 ) && ( ( lfCurrentTime - lfStartTime ) < lfTimeOut ))
	{
		CHK_CMT(CmtReleaseTSVPtr ( hHandle ));
		bGetVariable = 0;
		
		ProcessSystemEvents(); 
		DelayWithEventProcessing(0.2);
		
		CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
		bGetVariable = 1;
		GetCurrentDateTime(&lfCurrentTime); 
	}

Error:

	if ( hHandle && bGetVariable )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT	Communication_PlugIn( int hHandle , double lfTimeOut )
{
	STD_ERROR						StdError						=	{0};

	double							lfStartTime						=	0.0,
									lfCurrentTime					=	0.0;
	
	int								bGetVariable					=	0;
	
	tsSingleHandle					*pLocalStore					=	NULL,
									*pQueueStore					=	NULL;
	
	int								iEmptyItems						=	0;
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	bGetVariable = 1;
	
	IF (( pLocalStore->bUnpluggedStatus == 0 ) , "Already Plugged In" );
	
	pLocalStore->bPlugInRequest = 1;  
	
	CHK_CMT( CmtGetTSQWritePtr ( pLocalStore->queue_handle , &pQueueStore , &iEmptyItems )); 
	memset( pQueueStore , 0 , sizeof(tsSingleHandle));
	COM_Support_Copy_Store( pQueueStore , pLocalStore ); 
	CHK_CMT( CmtReleaseTSQWritePtr ( pLocalStore->queue_handle , 1 ));
	
	ProcessSystemEvents(); 
	
	GetCurrentDateTime(&lfStartTime);
	GetCurrentDateTime(&lfCurrentTime);
	
	while (( pLocalStore->bUnpluggedStatus == 1 ) && ( ( lfCurrentTime - lfStartTime ) < lfTimeOut ))
	{
		CHK_CMT(CmtReleaseTSVPtr ( hHandle ));
		bGetVariable = 0;
		
		ProcessSystemEvents(); 
		DelayWithEventProcessing(0.2);
		
		CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
		bGetVariable = 1;
		GetCurrentDateTime(&lfCurrentTime); 
	}

Error:

	if ( hHandle && bGetVariable )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT	Communication_GetPluggedInStatus( int hHandle , int *piStatus )
{
	STD_ERROR						StdError						=	{0};

	int								bGetVariable					=	0;
	
	int								status							=	0;
	
	tsSingleHandle		*pLocalStore			=	{0};
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	bGetVariable = 1;
	
	status = pLocalStore->bUnpluggedStatus;

Error:

	if ( hHandle && bGetVariable )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	if ( piStatus )
		*piStatus = status;
	
	RETURN_STDERR_POINTER;	
}


void* DLLEXPORT	Communication_SetPluggedInInitFailDelay( int hHandle , double lfDelay )
{
	STD_ERROR						StdError						=	{0};

	int								bGetVariable					=	0;
	
	tsSingleHandle		*pLocalStore			=	{0};
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	bGetVariable = 1;

	pLocalStore->lfPlugInInitFailDelay = lfDelay;
		
Error:

	if ( hHandle && bGetVariable )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT	Communication_GetSendStatus ( int hHandle , int *pbSentSuccessfully )
{
	STD_ERROR						StdError						=	{0};

	int								bGetVariable					=	0;
	
	tsSingleHandle		*pLocalStore			=	{0};
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	bGetVariable = 1;

	if ( pbSentSuccessfully )
		*pbSentSuccessfully = pLocalStore->bSentSuccessfully;
		
	FREE_STDERR_COPY( pLocalStore->pLastError );
		
Error:

	if ( hHandle && bGetVariable )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT	Communication_SetLowLevelConfig( int hHandle , int iReadBlockSize , double lfLowLevelReceiveTimeout , double lfLowLevelSendTimeout , int bFlushReceiveBuffer , int bDontWaitForSendStatus )
{
	STD_ERROR						StdError						=	{0};

	int								bGetVariable					=	0;
	
	tsSingleHandle		*pLocalStore			=	{0};
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	bGetVariable = 1;
	
	if ( iReadBlockSize < 1 )
		iReadBlockSize = 1;
	
	pLocalStore->iReadBlockSize = iReadBlockSize;
	
	pLocalStore->iLowLevelReceiveTimeOut = (int) ( lfLowLevelReceiveTimeout * 1E3 );
	
	if ( pLocalStore->iLowLevelReceiveTimeOut <= 0 )
		pLocalStore->iLowLevelReceiveTimeOut = 1000;
		
	pLocalStore->iLowLevelSendTimeOut = (int) ( lfLowLevelSendTimeout * 1E3 );
	
	if ( pLocalStore->iLowLevelSendTimeOut <= 0 )
		pLocalStore->iLowLevelSendTimeOut = 1000;
	
	if ( bFlushReceiveBuffer )
		pLocalStore->bFlushReceive = 1;
	
	if ( bDontWaitForSendStatus ) 
		pLocalStore->bDontWaitForSendStatus = 1;
		
Error:

	if ( hHandle && bGetVariable )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}
	

void* DLLEXPORT	Communication_SetReadBlockSize( int hHandle , int iReadBlockSize )
{
	STD_ERROR						StdError						=	{0};

	int								bGetVariable					=	0;
	
	tsSingleHandle		*pLocalStore			=	{0};
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	bGetVariable = 1;
	
	if ( iReadBlockSize < 1 )
		iReadBlockSize = 1;
	
	pLocalStore->iReadBlockSize = iReadBlockSize;
		
Error:

	if ( hHandle && bGetVariable )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT	Communication_FlushReceiveBuffer( int hHandle , int bFlushReceiveBuffer )
{
	STD_ERROR						StdError						=	{0};

	int								bGetVariable					=	0;
	
	tsSingleHandle					*pLocalStore					=	NULL,
									*pQueueStore					=	NULL;
	
	int								iEmptyItems						=	0;
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	bGetVariable = 1;
	
	pLocalStore->bFlushReceive = bFlushReceiveBuffer;
	
	if ( bFlushReceiveBuffer )
	{
		CHK_CMT( CmtGetTSQWritePtr ( pLocalStore->queue_handle , &pQueueStore , &iEmptyItems )); 
		memset( pQueueStore , 0 , sizeof(tsSingleHandle));
		COM_Support_Copy_Store( pQueueStore , pLocalStore ); 
		CHK_CMT( CmtReleaseTSQWritePtr ( pLocalStore->queue_handle , 1 ));
		
		ProcessSystemEvents(); 
	}
	
Error:

	if ( hHandle && bGetVariable )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT	Communication_EnableLoging( int hHandle , int bState , char *pLogFileNamePath , int bLogFormatIsAscii , int bLogContinues )
{
	STD_ERROR						StdError						=	{0};

	tsSingleHandle					*pLocalStore					=	{0};
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	
	if ( bState )
		pLocalStore->bEnableLoging = 1;
	else
		pLocalStore->bEnableLoging = 0;  
	
	if ( bLogContinues )
		pLocalStore->bLogContinues = 1;
	else
		pLocalStore->bLogContinues = 0;
		
	if ( pLogFileNamePath && ( strlen(pLogFileNamePath) >= 5 ))
		FREE_CALLOC_COPY_STRING( pLocalStore->pszLogFile , pLogFileNamePath );	
	
	pLocalStore->bLogFormatIsAscii = bLogFormatIsAscii;
		
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT	Communication_SetTypingMode( int hHandle , int bState , double lfDelay )
{
	STD_ERROR						StdError						=	{0};

	tsSingleHandle					*pLocalStore					=	{0};
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	
	if ( bState )
		pLocalStore->bTypingMode = 1;
	else
		pLocalStore->bTypingMode = 0;  

	pLocalStore->lfTypingModeDelay = lfDelay;
		
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT	Communication_EnableReceive( int hHandle , int bState )
{
	STD_ERROR						StdError						=	{0};

	tsSingleHandle					*pLocalStore					=	{0}; 
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	
	if ( bState )
		pLocalStore->bReceiveEnable = 1;
	else
		pLocalStore->bReceiveEnable = 0; 
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT	Communication_CloseLogFile( int hHandle )
{
	STD_ERROR						StdError						=	{0};

	tsSingleHandle					*pLocalStore					=	{0}; 
	
	double							lfLogCurrentTime				=	0.0;
	
	char							szLogFormated[LOW_STRING]		=	{0};
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	
	pLocalStore->bEnableLoging = 0;  
	
	GetCurrentDateTime(&lfLogCurrentTime);
	FormatDateTimeString ( lfLogCurrentTime , "\r\n%H:%M:%S.%3f    Close Com.\r\n" , szLogFormated , LOW_STRING );
	WriteFile ( pLocalStore->hLogFileHandle , szLogFormated , strlen(szLogFormated) ); 
	
	CloseFile ( pLocalStore->hLogFileHandle );
	pLocalStore->hLogFileHandle = 0;
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT	Communication_UpdateLastChanges( int hHandle )
{
	STD_ERROR						StdError						=	{0};

	tsSingleHandle					*pLocalStore					=	{0},
									*pQueueStore					=	NULL;
	
	int								iEmptyItems						=	0;
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));

	CHK_CMT( CmtGetTSQWritePtr ( pLocalStore->queue_handle , &pQueueStore , &iEmptyItems )); 
	memset( pQueueStore , 0 , sizeof(tsSingleHandle));
	COM_Support_Copy_Store( pQueueStore , pLocalStore ); 
	CHK_CMT( CmtReleaseTSQWritePtr ( pLocalStore->queue_handle , 1 ));
	
	ProcessSystemEvents(); 
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT	Communication_ClearErrors( int hHandle )
{
	STD_ERROR						StdError						=	{0};

	tsSingleHandle					*pLocalStore					=	{0};
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	
	FREE( pLocalStore->pLastError );
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT	Communication_SetReceiveByteTimeOut( int hHandle , double lfTimeout )
{
	STD_ERROR						StdError						=	{0};

	tsSingleHandle					*pLocalStore					=	{0};
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	
	if ( lfTimeout > 0.0 )
		pLocalStore->lfReceiveSingleByteTimeOut = lfTimeout;  
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT	Communication_SetReceiveBufferSize( int hHandle , unsigned int size )
{
	STD_ERROR						StdError						=	{0};

	tsSingleHandle					*pLocalStore					=	{0};
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	
	if ( size > 1024 )
	{	pLocalStore->receiveBufferSize = size;  
		pLocalStore->bFlushReceive = 1; 
	}
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT	Communication_NextTimeReturnFullBuffer( int hHandle , int bState )
{
	STD_ERROR						StdError						=	{0};

	tsSingleHandle					*pLocalStore					=	{0};
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	
	if ( bState )
		pLocalStore->bUpdateReceiveAll = 1;  
	else
		pLocalStore->bUpdateReceiveAll = 0;
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT	Communication_InstallReceiveCallBack( int hHandle , pfReceiveCallBack pFunction , void *pData )
{
	STD_ERROR						StdError						=	{0};

	tsSingleHandle					*pLocalStore					=	{0};
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	
	if ( pLocalStore->initStatus == 1 ) 
	{
		pLocalStore->pReceiveCallBack = pFunction;
		pLocalStore->pReceiveCallBackData = pData;
	}
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT	Communication_InstallAttributeSetCallBack( int hHandle , pfAttributeCallBack pFunction , void *pData )
{
	STD_ERROR						StdError						=	{0};

	tsSingleHandle					*pLocalStore					=	{0};
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	
	if ( pLocalStore->initStatus == 1 )
	{
		pLocalStore->pAttributeCallBack = (pfAttributeCallBack)pFunction;
		pLocalStore->pAttributeCallBackData = pData;
	}
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT	Communication_InstallCloseCallBack( int hHandle , pfCloseCallBack pFunction , void *pData )
{
	STD_ERROR						StdError						=	{0};

	tsSingleHandle					*pLocalStore					=	{0};
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	
	if ( pLocalStore->initStatus == 1 )
	{
		pLocalStore->pCloseCallBack = (pfCloseCallBack)pFunction;
		pLocalStore->pCloseCallBackData = pData;
	}
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT	Communication_InstallErrorCallBack( int hHandle , pfErrorCallBack pFunction , void *pData )
{
	STD_ERROR						StdError						=	{0};

	tsSingleHandle					*pLocalStore					=	{0};
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	
	if ( pLocalStore->initStatus == 1 )
	{
		pLocalStore->pErrorCallBack = (pfErrorCallBack)pFunction;
		pLocalStore->pErrorCallBackData = pData;
	}
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT	Communication_GetConnectionAddress( int hHandle , char	**szAddress )
{
	STD_ERROR						StdError						=	{0};

	tsSingleHandle					*pLocalStore					=	{0};
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0); 
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	
	if ( szAddress && pLocalStore->pszConnectString )
		strcpy( szAddress , pLocalStore->pszConnectString );
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1); 
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT	Communication_SetTerminatingPattern( int hHandle , char *pszTerminatingPattern , int iLength )
{
	STD_ERROR						StdError						=	{0};

	tsSingleHandle					*pLocalStore					=	{0};
	
	IF( ( hHandle == 0 ) , "Handle can't be 0." );  

	SetBreakOnLibraryErrors (0);
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStore ));
	
	if ( pszTerminatingPattern && iLength )
	{
		CALLOC_ERR( pLocalStore->pszTerminatingPattern , iLength+1 , sizeof(char));
		
		memcpy( pLocalStore->pszTerminatingPattern , pszTerminatingPattern , (iLength*sizeof(char)));
		
		pLocalStore->iTerminatingPatternLength = iLength;
	}
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	SetBreakOnLibraryErrors (1);
	
	RETURN_STDERR_POINTER;	
}


void* DLLEXPORT	Communication_GetGlobalLock( int hHandle , int *pbGlobalLocked )
{
	STD_ERROR						StdError						=	{0};	

Error:

	RETURN_STDERR_POINTER;	
}


void* DLLEXPORT	Communication_ReleaseGlobalLock( int hHandle , int *pbGlobalLocked )
{
	STD_ERROR						StdError						=	{0};	

Error:

	RETURN_STDERR_POINTER;	
}

/* 
void main(void)
{
	int							hHandle					=	0;

	Communication_Init( "ASRL1" , &hHandle ); 

	return;
}
*/
