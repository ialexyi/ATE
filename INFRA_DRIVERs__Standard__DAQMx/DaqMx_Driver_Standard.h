#ifndef __DaqMx_Driver_Standard_H__
#define __DaqMx_Driver_Standard_H__

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
typedef void* (*tfStdReceiveCallback) ( void *pHandle , char *pCommand , int *pValue , int iValueSize , char *pReceiveBuffer , int iBufferSize );
//==============================================================================
// Global functions
void* 	DLLEXPORT 	DAQMX_STD_Driver_GetErrorTextMessage ( int vhInstrumentHandle , int iError , char *pErrorMessage );

void*	DLLEXPORT	DAQMX_Driver_Config_Install_CommentCallback( int *pHandle , pfCommentCallback fFunction , void *pData , int type);
void*	DLLEXPORT	DAQMX_Driver_Config_Install_ConfigValueCallback( int *pHandle , pfConfigValueCallback fFunction , void *pData , int type);
void*	DLLEXPORT	DAQMX_Driver_Config_Install_CheckForBreakCallback( int *pHandle , pfCheckForBreakCallback fFunction , void *pData , int type);

void*   DLLEXPORT   DAQMX_Driver_InstallReceiveCallbackFunction( int hHandle , tfStdReceiveCallback pfFunction , void *pData );

void*   DLLEXPORT   DAQMX_Driver_LoadConfigFile( int hHandle , char *pFileName );
void*   DLLEXPORT   DAQMX_Driver_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize );
	
void*   DLLEXPORT   DAQMX_Driver_GetConnectionAddress( int hHandle , char **szAddress );
void*   DLLEXPORT   DAQMX_Driver_Unplug( int hHandle , double lfTimeOut ); 
void*   DLLEXPORT   DAQMX_Driver_PlugIn( int hHandle , double lfTimeOut );
void*   DLLEXPORT   DAQMX_Driver_EnablePlugInAutomatically( int hHandle , int bEnable );

void*	DLLEXPORT	DAQMX_Driver_Config_LOG_SetAllowState( int iHandle , int bAllowLog );

void* 	DLLEXPORT 	DAQMX_Driver_Init( char *pszConnectionName , char *pszAddress , int *pHandle );
void* 	DLLEXPORT 	DAQMX_Driver_Close( int *pHandle );

void*   DLLEXPORT   DAQMX_Driver_SetValue( int hHandle , char *pCommandName , void *pValue , int iValueLength , int iValueSize , ...  );
void*   DLLEXPORT   DAQMX_Driver_GetValue_Ex( int hHandle , char *pCommandName , void **pGetValue , int *piValueLength , int *piValueSize, ...  );

void*   DLLEXPORT   DAQMX_Driver_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize, ...  );

void*   DLLEXPORT   DAQMX_Driver_Check_Connection( int hHandle , char *pCommandName , int *piStatus );
void*	DLLEXPORT	DAQMX_Driver_Get_Commands_List( int hHandle , char ***pCommandsList , int *piNumberOfCommands );     

#endif  /* ndef __DaqMx_Driver_Standard_H__ */
