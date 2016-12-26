#ifndef __STD_CLI_DRIVER_STD_H__
#define __STD_CLI_DRIVER_STD_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "windows.h"
#include "cvidef.h"
#include <userint.h>
#include <ansi_c.h>
#include "GlobalDefines.h"

//==============================================================================
// Constants

//==============================================================================
// Types
typedef void* (*tfStdReceiveCallback) ( void *pHandle , char *pCommand , int *pValue , int iValueSize , char *pReceiveBuffer , int iBufferSize );
//==============================================================================
// External variables

//==============================================================================
// Global functions
void*   DLLEXPORT   STD_CLI_Driver_Config_Install_CommentCallback( int *pHandle , pfCommentCallback fFunction , void *pData , int type); 
void*   DLLEXPORT   STD_CLI_Driver_Config_Install_ConfigValueCallback( int *pHandle , pfConfigValueCallback fFunction , void *pData , int type);
void*   DLLEXPORT   STD_CLI_Driver_Config_Install_CheckForBreakCallback( int *pHandle , pfCheckForBreakCallback fFunction , void *pData , int type);
void*	DLLEXPORT	STD_CLI_Driver_Config_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet );

void*	DLLEXPORT	STD_CLI_Driver_Config_LOG_SetAllowState( int iHandle , int bAllowLog );

void*   DLLEXPORT   STD_CLI_Driver_Init( char *pszConnectionName , char *pszAddress , int *pHandle , ... );

void*   DLLEXPORT   STD_CLI_Driver_Close( int *pHandle );

void*   DLLEXPORT   STD_CLI_Driver_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize , ...  );    
void*   DLLEXPORT   STD_CLI_Driver_SetValue( int hHandle , char *pCommandName , void *pValue , int iValueLength , int iValueSize , ...  );			  

void*   DLLEXPORT   STD_CLI_Driver_GetValue_Ex ( int hHandle , char *pCommandName , void **pGetValue , int *piValueLength , int *piValueSize , ...  );    

void*   DLLEXPORT   STD_CLI_Driver_InstallReceiveCallbackFunction( int hHandle , tfStdReceiveCallback pfFunction , void *pData );
	
void*   DLLEXPORT   STD_CLI_Driver_LoadConfigFile( int hHandle , char *pFileName );
void*   DLLEXPORT   STD_CLI_Driver_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize );
	
void*   DLLEXPORT   STD_CLI_Driver_GetConnectionAddress( int hHandle , char	**szAddress );
void*   DLLEXPORT   STD_CLI_Driver_Unplug( int hHandle , double lfTimeOut ); 
void*   DLLEXPORT   STD_CLI_Driver_PlugIn( int hHandle , double lfTimeOut );
void*   DLLEXPORT   STD_CLI_Driver_EnablePlugInAutomatically( int hHandle , int bEnable );

void*   DLLEXPORT   STD_CLI_Driver_Check_Connection( int hHandle , char *pCommandName , int *piStatus );
void*	DLLEXPORT	STD_CLI_Driver_Get_Commands_List( int hHandle , char ***pCommandsList , int *piNumberOfCommands );     

void*	DLLEXPORT	STD_CLI_GetLowLevelHandle( int hHandle , int *phLowLevelHandle );

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __STD_CLI_DRIVER_STD_H__ */
