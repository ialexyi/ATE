#ifndef __STD_Wrapper_H__
#define __STD_Wrapper_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"
#include "GlobalDefines.h"

//==============================================================================
// Constants

//==============================================================================
// Types


//==============================================================================
// External variables

//==============================================================================
// Global functions
void* DLLEXPORT Config_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet );

void* DLLEXPORT Config_Install_CommentCallback(int *pHandle,pfCommentCallback fFunction, void *pData, int type);
void* DLLEXPORT Config_Install_ConfigValueCallback(int *pHandle,pfConfigValueCallback fFunction, void *pData, int type);
void* DLLEXPORT Config_Install_CheckForBreakCallback(int *pHandle,pfCheckForBreakCallback fFunction, void *pData, int type);

void* DLLEXPORT DEV_Init( int iDeviceID , char *pszConnectionName , char *pszAddress , int *pHandle , ... );
void* DLLEXPORT DEV_GetConnectionAddress(int hHandle,char **szAddress);
void* DLLEXPORT DEV_Close(int *pHandle);

void* DLLEXPORT DEV_SetValue( int hHandle , char *pCommandName , void *pValue , int iValueLength , int iValueSize , ...  );
void* DLLEXPORT DEV_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize , ...  );
	
void* DLLEXPORT DEV_InstallReceiveCallbackFunction( int hHandle , void *pfFunction , void *pData );

void* DLLEXPORT DEV_LoadConfigFile( int hHandle , char *pFileName );
void* DLLEXPORT DEV_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize );
	
void* DLLEXPORT DEV_Unplug( int hHandle , double lfTimeOut );  
void* DLLEXPORT DEV_PlugIn( int hHandle , double lfTimeOut );
void* DLLEXPORT	DEV_EnablePlugInAutomatically( int hHandle , int bEnable );

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __STD_Wrapper_H__ */
