#ifndef __Communication_Thread_H__
#define __Communication_Thread_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"

//==============================================================================
// Constants

//==============================================================================
// Types
typedef void (*pfReceiveCallBack) ( char *buffer , unsigned int count , void *data ); 

typedef struct
{
	pfReceiveCallBack		pCallBackFunction;
	
	void					*pReceiveCallBackData;
	
	unsigned int			receiveCount;
	
	unsigned char			*pBuffer;
		
} tsReceiveDataItem;

//==============================================================================
// External variables

//==============================================================================
// Global functions

int						COM_Thread( void *pData );
void CVICALLBACK 		COM_FinishThread (CmtThreadPoolHandle poolHandle, CmtThreadFunctionID functionID, unsigned int event, int value, void *callbackData);
	
#ifdef __cplusplus
    }
#endif

#endif  /* ndef __Communication_Thread_H__ */
