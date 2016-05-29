#ifndef __Communication_Low_Level_H__
#define __Communication_Low_Level_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files
#include <windows.h>
#include <visa.h>
		
#include "cvidef.h"

#include "GlobalDefines.h" 

//==============================================================================
// Constants

//==============================================================================
// Types

typedef void (*pfReceiveCallBack) ( char *buffer , unsigned int count , void *data ); 

typedef int (*pfAttributeCallBack) ( unsigned long hCommHandle , void *data ); 

typedef void (*pfCloseCallBack) ( unsigned long hCommHandle , int error , void *data );

typedef void (*pfErrorCallBack) ( unsigned long hCommHandle , void *pErrEv , void *data );  

//==============================================================================
// External variables

//==============================================================================
// Global functions

void*	DLLEXPORT	Communication_Init( char *pszConnectionName , char *pszAddress , int *phHandle , ... );

void*	DLLEXPORT	Communication_Close( int hHandle );
void*	DLLEXPORT	Communication_GetLastError(int hHandle);

void* 	DLLEXPORT	Communication_UpdateLastChanges( int hHandle );
void* 	DLLEXPORT	Communication_GetSendStatus ( int hHandle , int *pbSentSuccessfully );

void* 	DLLEXPORT	Communication_Send( int hHandle , unsigned char *pBuffer , unsigned int iSendCount , unsigned int  iReceiveBreakCount );

void*	DLLEXPORT	Communication_Receive(int hHandle,double lfTimeOut, unsigned char **pBuffer,int *piCount);

void*	DLLEXPORT	Communication_EnableReceive(int hHandle,int bState);
void*	DLLEXPORT	Communication_ClearErrors(int hHandle);
void*	DLLEXPORT	Communication_SetReceiveByteTimeOut(int hHandle,double lfTimeout);
void*	DLLEXPORT	Communication_SetReceiveBufferSize(int hHandle,unsigned int size);
void*	DLLEXPORT	Communication_NextTimeReturnFullBuffer(int hHandle,int bState);

void*	DLLEXPORT	Communication_InstallReceiveCallBack(int hHandle,pfReceiveCallBack pFunction, void *pData);
void*	DLLEXPORT	Communication_InstallAttributeSetCallBack( int hHandle , pfAttributeCallBack pFunction , void *pData );
	
void*	DLLEXPORT	Communication_Direct_WriteString(int hHandle,char *szString, int *pSentCount);
void*	DLLEXPORT	Communication_Direct_ReceiveString(int hHandle,double lfTimeout,unsigned char **pReceive);
void*	DLLEXPORT	Communication_Direct_QueryString(int handle,char *szString, double lfTimeout,unsigned char **pReceive);

void*	DLLEXPORT	Communication_GetConnectionAddress( int hHandle , char **szAddress );

void*	DLLEXPORT	Communication_EnableLoging( int hHandle , int bState , char *pLogFileNamePath , int bLogFormatIsAscii , int bLogContinues );

void*	DLLEXPORT	Communication_Direct_WriteBuffer( int hHandle , char *pBuffer , int iBufferSize , int *pSentCount ); 
void*	DLLEXPORT	Communication_Direct_ReceiveBuffer( int hHandle , double lfTimeout , int count , unsigned char **pReceive );

void*	DLLEXPORT	Communication_InstallCloseCallBack( int hHandle , pfCloseCallBack pFunction , void *pData );
void* 	DLLEXPORT	Communication_InstallErrorCallBack( int hHandle , pfErrorCallBack pFunction , void *pData );

void* 	DLLEXPORT	Communication_Unplug( int hHandle , double lfTimeOut );  
void* 	DLLEXPORT	Communication_PlugIn( int hHandle , double lfTimeOut ); 
void* 	DLLEXPORT	Communication_GetPluggedInStatus( int hHandle , int *piStatus );
void* 	DLLEXPORT	Communication_SetPluggedInInitFailDelay( int hHandle , double lfDelay );

void* 	DLLEXPORT	Communication_GetGlobalLock( int hHandle , int *pbGlobalLocked );   
void* 	DLLEXPORT	Communication_ReleaseGlobalLock( int hHandle , int *pbGlobalLocked ); 

void*	DLLEXPORT	Communication_SetLowLevelConfig( int hHandle , int iReadBlockSize , double lfLowLevelReceiveTimeout , double lfLowLevelSendTimeout , int bFlushReceiveBuffer , int bDontWaitForSendStatus );

void*	DLLEXPORT	Communication_SetTerminatingPattern( int hHandle , char *pszTerminatingPattern , int iLength );

void* 	DLLEXPORT	Communication_FlushReceiveBuffer( int hHandle , int bFlushReceiveBuffer ); 
	
void*	DLLEXPORT	Communication_SetTypingMode( int hHandle , int bState , double lfDelay );

void* 	DLLEXPORT	Communication_CloseLogFile( int hHandle );

void* 	DLLEXPORT	Communication_SetReadBlockSize( int hHandle , int iReadBlockSize );

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __Communication_Low_Level_H__ */
