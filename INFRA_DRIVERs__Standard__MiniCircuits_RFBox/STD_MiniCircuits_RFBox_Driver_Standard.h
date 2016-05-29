#ifndef __STANDARD_MINI_CIRCUITS_RFBox_Driver_wrapper_H__
#define __STANDARD_MINI_CIRCUITS_RFBox_Driver_wrapper_H__


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
void *MINI_CIRCUITS_STD_Driver_GetErrorTextMessage(int hHandle, int iError, char *pErrorMessage);
void *MINI_CIRCUITS_Copy_STD_CallBackSet(int *pHandle, void *pCallBackSet);
void *MINI_CIRCUITS_STD_Driver_GetLowLevelHandle(int hHandle, int *phLowLevelHandle);
void *MINI_CIRCUITS_STD_Driver_GetConnectionAddress(int hHandle, char **pszAddress);
void *MINI_CIRCUITS_STD_Driver_Equipment_Info(int hLowLevelHandle, char *pAddress, char **pVendor, char **pSerialNumber, char **pModel, char **pFirmware);
void *MINI_CIRCUITS_STD_Driver_Equipment_IsSupported(int hLowLevelHandle, char *pAddress, char *pID_String, char *pIdentificationText, int *pbSupporting, void **pLocalData);
void *MINI_CIRCUITS_STD_Driver_Close(int *pHandle);
void *MINI_CIRCUITS_STD_Driver_Init(char *szRsrcAddress, int *phInstrumentHandle);
void *MINI_CIRCUITS_STD_Driver_SetValue(int hHandle, char *pCommandName, void *pValue, int iValueLength, int iValueSize, ...);
void *MINI_CIRCUITS_STD_Driver_GetValue(int hHandle, char **pGetCommandName, void **pGetValue, int *piValueLength, int *piValueSize, ...);
void *MINI_CIRCUITS_STD_Driver_GetValue_Ex(int hHandle, char *pCommandName, void **pGetValue, int *piValueLength, int *piValueSize, ...);
void *MINI_CIRCUITS_STD_Driver_Check_Connection(int hHandle, char *pCommandName, int *piStatus);
void *MINI_CIRCUITS_STD_Driver_Unplug(int hHandle, double lfTimeOut);
void *MINI_CIRCUITS_STD_Driver_PlugIn(int hHandle, double lfTimeOut);
void *MINI_CIRCUITS_STD_Driver_EnablePlugInAutomatically(int hHandle, int bEnable);
void *MINI_CIRCUITS_STD_Driver_InstallReceiveCallbackFunction(int hHandle, tfStdReceiveCallback pfFunction, void *pData);
void *MINI_CIRCUITS_STD_Driver_LoadConfigFile(int hHandle, char *pFileName);
void *MINI_CIRCUITS_STD_Driver_LoadConfigBuffer(int hHandle, char *pBuffer, int iBufferSize);
void *MINI_CIRCUITS_STD_Driver_Get_Commands_List(int hHandle, char ***pCommandsList, int *piNumberOfCommands);
  

#endif  /* ndef __STANDARD_MINI_CIRCUITS_RFBox_Driver_wrapper_H__ */
