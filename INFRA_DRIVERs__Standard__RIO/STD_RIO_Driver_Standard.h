#ifndef __STD_RIO_Driver_Standard_H__
#define __STD_RIO_Driver_Standard_H__



//==============================================================================
// Include files

#include "cvidef.h"
#include "RIO_Driver.h"  
#include "STD_RIO_Driver_Commands_Define.h"  

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// External variables

typedef void* (*tfStdReceiveCallback) ( void *pHandle , char *pCommand , int *pValue , int iValueSize , char *pReceiveBuffer , int iBufferSize );  

//==============================================================================
// Global functions 

void *RIO_STD_Driver_GetErrorTextMessage(int hHandle, int iError, char *pErrorMessage);
void *RIO_Copy_STD_CallBackSet(int *pHandle, void *pCallBackSet);
void *RIO_STD_Driver_GetLowLevelHandle(int hHandle, int *phLowLevelHandle);
void *RIO_STD_Driver_GetConnectionAddress(int hHandle, char **pszAddress);
void *RIO_STD_Driver_Equipment_Info(int hLowLevelHandle, char *pAddress, char **pVendor, char **pSerialNumber, char **pModel, char **pFirmware);
void *RIO_STD_Driver_Equipment_IsSupported(int hLowLevelHandle, char *pAddress, char *pID_String, char *pIdentificationText, int *pbSupporting, void **pLocalData);
void *RIO_STD_Driver_Reset(int hHandle);  
void *RIO_STD_Driver_Close(int *pHandle);
void *RIO_STD_Driver_Init(char *szRsrcAddress, int *phInstrumentHandle);
void *RIO_STD_Driver_SetValue(int hHandle, char *pCommandName, void *pValue, int iValueLength, int iValueSize, ...);
void *RIO_STD_Driver_GetValue(int hHandle, char **pGetCommandName, void **pGetValue, int *piValueLength, int *piValueSize, ...);
void *RIO_STD_Driver_GetValue_Ex(int hHandle, char *pCommandName, void **pGetValue, int *piValueLength, int *piValueSize, ...);
void *RIO_STD_Driver_Check_Connection(int hHandle, char *pCommandName, int *piStatus);
void *RIO_STD_Driver_Unplug(int hHandle, double lfTimeOut);
void *RIO_STD_Driver_PlugIn(int hHandle, double lfTimeOut);
void *RIO_STD_Driver_EnablePlugInAutomatically(int hHandle, int bEnable);
void *RIO_STD_Driver_InstallReceiveCallbackFunction(int hHandle, tfStdReceiveCallback pfFunction, void *pData);
void *RIO_STD_Driver_LoadConfigFile(int hHandle, char *pFileName);
void *RIO_STD_Driver_LoadConfigBuffer(int hHandle, char *pBuffer, int iBufferSize);
void *RIO_STD_Driver_Get_Commands_List(int hHandle, char ***pCommandsList, int *piNumberOfCommands);


#endif  /* ndef __STD_RIO_Driver_Standard_H__ */
