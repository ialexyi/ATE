#ifndef __STD_EXT_Protocol_H__
#define __STD_EXT_Protocol_H__

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
#include "Communication_Low_Level.h"
#include "StandardExtendedProtocolDriver_Protocol.h"

//==============================================================================
// Constants

//==============================================================================
// Types
typedef void* (*tfStdReceiveCallback) ( void *pHandle , char *pCommand , int *pValue , int iValueSize , char *pReceiveBuffer , int iBufferSize );
//==============================================================================
// External variables

//==============================================================================
// Global functions
void*   DLLEXPORT   STD_Driver_Config_Install_CommentCallback( int *pHandle , pfCommentCallback fFunction , void *pData , int type); 
void*   DLLEXPORT   STD_Driver_Config_Install_ConfigValueCallback( int *pHandle , pfConfigValueCallback fFunction , void *pData , int type);
void*   DLLEXPORT   STD_Driver_Config_Install_CheckForBreakCallback( int *pHandle , pfCheckForBreakCallback fFunction , void *pData , int type);
void* 	DLLEXPORT	STD_Config_SetAttribute( int hHandle , char *pAttributeName , char *pAttributeValue );

void*	DLLEXPORT	STD_Driver_Config_LOG_SetAllowState( int iHandle , int bAllowLog );

void*   DLLEXPORT   STD_Driver_Init( char *pszConnectionName , char *pszAddress , int *pHandle , ... );

void*   DLLEXPORT   STD_Driver_Close( int *pHandle );

void*   DLLEXPORT   STD_Driver_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize , ...  );    
void*   DLLEXPORT   STD_Driver_SetValue( int hHandle , char *pCommandName , void *pValue , int iValueLength , int iValueSize , ...  );

void*   DLLEXPORT   STD_Driver_InstallReceiveCallbackFunction( int hHandle , tfStdReceiveCallback pfFunction , void *pData );
	
void*   DLLEXPORT   STD_Driver_LoadConfigFile( int hHandle , char *pFileName );
void*   DLLEXPORT   STD_Driver_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize );
	
void*   DLLEXPORT   STD_Driver_GetConnectionAddress( int hHandle , char	**szAddress );
void*   DLLEXPORT   STD_Driver_Unplug( int hHandle , double lfTimeOut ); 
void*   DLLEXPORT   STD_Driver_PlugIn( int hHandle , double lfTimeOut );
void*   DLLEXPORT   STD_Driver_EnablePlugInAutomatically( int hHandle , int bEnable );

void*   DLLEXPORT   STD_Driver_Check_Connection( int hHandle , char *pCommandName , int *piStatus );
void*	DLLEXPORT	STD_Driver_Get_Commands_List( int hHandle , char ***pCommandsList , int *piNumberOfCommands );     

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __STD_EXT_Protocol_H__ */
