#ifndef __STANDARD_FTDI_Driver_wrapper_H__
#define __STANDARD_FTDI_Driver_wrapper_H__


//==============================================================================
// Include files

#include "cvidef.h"

//==============================================================================
// Constants

//==============================================================================
// Types
		
//==============================================================================
// External variables

typedef void* (*tfStdReceiveCallback) ( void *pHandle , char *pCommand , int *pValue , int iValueSize , char *pReceiveBuffer , int iBufferSize );

//==============================================================================
// Global functions
void*	DLLEXPORT	FTDI_Driver_GetErrorTextMessage ( int vhInstrumentHandle , int iError , char *pErrorMessage );

void*	DLLEXPORT	FTDI_Driver_Config_Install_CommentCallback( int *pHandle , pfCommentCallback fFunction , void *pData , int type);
void*	DLLEXPORT	FTDI_Driver_Config_Install_ConfigValueCallback( int *pHandle , pfConfigValueCallback fFunction , void *pData , int type);
void*	DLLEXPORT	FTDI_Driver_Config_Install_CheckForBreakCallback( int *pHandle , pfCheckForBreakCallback fFunction , void *pData , int type);

void*   DLLEXPORT   FTDI_Driver_InstallReceiveCallbackFunction( int hHandle , tfStdReceiveCallback pfFunction , void *pData );

void*   DLLEXPORT   FTDI_Driver_LoadConfigFile( int hHandle , char *pFileName );
void*   DLLEXPORT   FTDI_Driver_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize );
	
void*   DLLEXPORT   FTDI_Driver_GetConnectionAddress( int hHandle , char **szAddress );
void*   DLLEXPORT   FTDI_Driver_Unplug( int hHandle , double lfTimeOut ); 
void*   DLLEXPORT   FTDI_Driver_PlugIn( int hHandle , double lfTimeOut );
void*   DLLEXPORT   FTDI_Driver_EnablePlugInAutomatically( int hHandle , int bEnable );

void*	DLLEXPORT	FTDI_Driver_Config_LOG_SetAllowState( int iHandle , int bAllowLog );

void* 	DLLEXPORT 	FTDI_Driver_Init( char *pszConnectionName , char *pszAddress , int *pHandle );
void* 	DLLEXPORT 	FTDI_Driver_Close( int *pHandle );

void*   DLLEXPORT   FTDI_Driver_SetValue( int hHandle , char *pCommandName , void *pValue , int iValueLength , int iValueSize , ...  );
void*   DLLEXPORT   FTDI_Driver_GetValue_Ex( int hHandle , char *pCommandName , void **pGetValue , int *piValueLength , int *piValueSize, ...  );

void*   DLLEXPORT   FTDI_Driver_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize, ...  );

void*   DLLEXPORT   FTDI_Driver_Check_Connection( int hHandle , char *pCommandName , int *piStatus );

void*	DLLEXPORT		FTDI_Driver_Get_Commands_List( int hHandle , char ***pCommandsList , int *piNumberOfCommands );     

#endif  /* ndef __STANDARD_FTDI_Driver_wrapper_H__ */
