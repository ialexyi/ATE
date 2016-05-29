#ifndef __Communication_Haeders_H__
#define __Communication_Haeders_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files
#include <windows.h>
#include <ansi_c.h>
#include <cvirte.h>     
#include <userint.h>
#include <utility.h>
#include "toolbox.h"
#include "cvidef.h"
#include <visa.h>
#include "GlobalDefines.h"
#include "Communication_Low_Level.h"

//==============================================================================
// Constants

//==============================================================================
// Types

typedef struct 
{ 
	union
	{
		char									szMax_Buffer[512];
	
		struct
		{
		
			CmtTSVHandle						thread_handle;
	
			CmtTSQHandle						queue_handle;
		
			ViSession							sessionHandle;
	
			char								*pszConnectString,
												*pszVisaAddress;
	
			int									initStatus;
	
			int									hSessionLock,
												hLogLockHandle;
	
			int									hThreadInitiated;
	
			unsigned char						*pSendBuffer;
	
			unsigned int						sendBufferSize;

			unsigned int						receiveBreakCount,
												receiveBufferSize;

			double								lfSendTime,
												lfReceiveTime;
	
			double								lfSendTimeout,
												lfReceiveTimeout,
												lfReceiveSingleByteTimeOut;
	
			double								lfPlugInInitFailDelay;
	
			int									bTypingMode;
			double								lfTypingModeDelay;
			
			int									sendStatus,
												receiveStatus;
	
			char								*pszTerminatingPattern;
			int 								iTerminatingPatternLength;
			//--------- Cycling ------------------//
			unsigned int						sendCycleBufferSize;
	
			int									iReadBlockSize;
	
			int									iLowLevelReceiveTimeOut,
												iLowLevelSendTimeOut;
	
			double								lfSendFrequency;
			//------ Controls --------///
			char								bSendRequest,
												bReceiveEnable,
												bUpdateReceive,
												bUpdateReceiveAll,
												bFlushReceive,
												bUnplugRequest,
												bPlugInRequest,
												bUnpluged,
												bUnpluggedStatus,
												bEnableLoging,
												bNeededComConfig;
	
			int									hLogFileHandle;

			char								*pszLogFile;
	
			int									bLogContinues;
	
			int									bLogFormatIsAscii;
	
			pfReceiveCallBack					pReceiveCallBack; 
			void								*pReceiveCallBackData;
	
			pfAttributeCallBack					pAttributeCallBack; 
			void								*pAttributeCallBackData;
	
			pfCloseCallBack						pCloseCallBack; 
			void								*pCloseCallBackData;
	
			pfErrorCallBack						pErrorCallBack; 
			void								*pErrorCallBackData;
	
			char								bBreakAll,
												bFinishThread;
	
			int									MaxThreadPriority;
	
			int									bDontWaitForSendStatus;
	
			//------ FeedBack ---------------------------------//
			char								bSentSuccessfully;
	
			STD_ERROR							*pLastError;
			
			//-------------- 1.1.0.0 ----------------------------------------//
			
		};
	};
	
} tsSingleHandle;
/*
typedef struct  SINGLE_LIST_ITEM
{ 
	union
	{
		char									szMax_Buffer[32];
	
		struct
		{
			CmtTSVHandle						handle;
	
			int									initStatus; 
	
			char								*pszConnectString;
	
			struct SINGLE_LIST_ITEM				*pNextItem;
	
			STD_ERROR							*pLastError; 
			
			//-------------- 1.1.0.0 ----------------------------------------//
		};
	};
	
} tsSingleListItem;

typedef struct 
{ 
	union
	{
		char									szMax_Buffer[32];
	
		struct
		{
			ViSession							defaultRM;
	
			CmtThreadPoolHandle					hThreadPoolHandle;
	
			int									hDatabaseConnectionHandle;
	
			tsSingleListItem					*pItemsList;
			
			//-------------- 1.1.0.0 ----------------------------------------//
		};
	};
	
} tsGlobalHandle;
*/
//==============================================================================
// External variables

//==============================================================================
// Global functions

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __Communication_Haeders_H__ */
