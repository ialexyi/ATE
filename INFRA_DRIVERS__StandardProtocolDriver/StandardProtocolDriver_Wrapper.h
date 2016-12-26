#ifndef __STD_Wrapper_H__
#define __STD_Wrapper_H__

//==============================================================================
// Include files

#include "cvidef.h"
#include "GlobalDefines.h"  

//==============================================================================
// Global functions

void* DLLEXPORT Config_Install_CommentCallback(int *pHandle,pfCommentCallback fFunction, void *pData, int type);
void* DLLEXPORT Config_Install_ConfigValueCallback(int *pHandle,pfConfigValueCallback fFunction, void *pData, int type);
void* DLLEXPORT Config_Install_CheckForBreakCallback(int *pHandle,pfCheckForBreakCallback fFunction, void *pData, int type);

void* DLLEXPORT StandardDevice_Init( int iDeviceID , char *pszConnectionName , char *pszAddress , int *pHandle , ... );
void* DLLEXPORT StandardDevice_Reset( int hHandle );
void* DLLEXPORT StandardDevice_Close(int *pHandle);
void* DLLEXPORT StandardDevice_GetConnectionAddress(int hHandle,char **szAddress);  

void* DLLEXPORT StandardDevice_SetValue( int hHandle , char *pCommandName , void *pValue , int iValueLength , int iValueSize , ...  );
void* DLLEXPORT StandardDevice_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize , ...  );
	
void* DLLEXPORT StandardDevice_InstallReceiveCallbackFunction( int hHandle , void *pfFunction , void *pData );

void* DLLEXPORT StandardDevice_LoadConfigFile( int hHandle , char *pFileName );
void* DLLEXPORT StandardDevice_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize );
	
void* DLLEXPORT StandardDevice_Unplug( int hHandle , double lfTimeOut );  
void* DLLEXPORT StandardDevice_PlugIn( int hHandle , double lfTimeOut );
void* DLLEXPORT	StandardDevice_EnablePlugInAutomatically( int hHandle , int bEnable ); 

#endif  /* ndef __STD_Wrapper_H__ */
