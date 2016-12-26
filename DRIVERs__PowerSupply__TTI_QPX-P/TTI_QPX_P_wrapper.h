#ifndef __TTI_PL303P_wrapper_H__
#define __TTI_PL303P_wrapper_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"

//==============================================================================
// Constants

#define		CH_NAME_LEN		128
//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

void*   DLLEXPORT       PowerSupply_GetErrorTextMessage(int viInstrumentHandle, int iError, char *pErrorMessage);
void*   DLLEXPORT       Equipment_Info(int hLowLevelHandle, char *pAddress, char **pVendor, char **pSerialNumber, char **pModel, char **pFirmware);
void*   DLLEXPORT       Equipment_GetLowLevelHandle(int hHandle, int *phLowLevelHandle);
void*   DLLEXPORT       Equipment_IsSupported(int hLowLevelHandle, char *pAddress, char *pID_String, char *pIdentificationText, int *pbSupporting, void **pLocalData);
void*   DLLEXPORT       Config_Copy_STD_CallBackSet(int *pHandle, void *pCallBackSet);
void*   DLLEXPORT       PowerSupply_Init(int hParentInstrumentHandle, char* szRsrcAddress, int *viInstrumentHandle, int *InstrumentConnectStatus, int *InstrumentChanelsQuantity);
void*   DLLEXPORT       PowerSupply_Close(int *phInstrumentHandle);
void*   DLLEXPORT       PowerSupply_Reset(int viInstrumentHandle);

void*   DLLEXPORT       PowerSupply_WaitForOperationComplete(int viInstrumentHandle, double lfTimeout);

void*   DLLEXPORT       PowerSupply_GetModel(int viInstrumentHandle, int channel, char szReturnString[256]);

void*   DLLEXPORT       PowerSupply_GetRevision(int viInstrumentHandle, int channel, char szReturnString[256]);

void*   DLLEXPORT       PowerSupply_SetOutputVoltage(int viInstrumentHandle, int channel, double lfVoltage);

void*   DLLEXPORT       PowerSupply_GetOutputVoltageDefined(int viInstrumentHandle, int channel, double *plfVoltage);

void*   DLLEXPORT       PowerSupply_GetOutputVoltageActual(int viInstrumentHandle, int channel, double *plfVoltage);

void*   DLLEXPORT       PowerSupply_SetOutputCurrent(int viInstrumentHandle, int channel, double lfCurrent);

void*   DLLEXPORT       PowerSupply_GetOutputCurrentDefined(int viInstrumentHandle, int channel, double *plfCurrent);

void*   DLLEXPORT       PowerSupply_GetOutputCurrentActual(int viInstrumentHandle, int channel, double *plfCurrent);

void*   DLLEXPORT       PowerSupply_GetOutputPowerActual(int viInstrumentHandle, int channel, double *plfPower);

void*   DLLEXPORT       PowerSupply_SetVoltageProtectionOver(int viInstrumentHandle, int channel, double lfVoltage);

void*   DLLEXPORT       PowerSupply_GetVoltageProtectionOver(int viInstrumentHandle, int channel, double *plfVoltage);

void*   DLLEXPORT       PowerSupply_SetVoltageProtectionUnder(int viInstrumentHandle, int channel, double lfVoltage);

void*   DLLEXPORT       PowerSupply_GetVoltageProtectionUnder(int viInstrumentHandle, int channel, double *plfVoltage);

void*   DLLEXPORT       PowerSupply_SetOutputState(int viInstrumentHandle, int channel, int iState);

void*   DLLEXPORT       PowerSupply_GetOutputState(int viInstrumentHandle, int channel, int *piState);
void*   DLLEXPORT       PowerSupply_SetFoldbackProtectionMode(int viInstrumentHandle, int channel, int iMode);
void*   DLLEXPORT       PowerSupply_GetFoldbackProtectionMode(int viInstrumentHandle, int channel, int *piMode);
void*   DLLEXPORT       PowerSupply_SetAutoRestartMode(int viInstrumentHandle, int channel, int iMode);
void*   DLLEXPORT       PowerSupply_GetAutoRestartMode(int viInstrumentHandle, int channel, int *piMode);
void*   DLLEXPORT       PowerSupply_GetOperationalStatusRegister(int viInstrumentHandle, int channel, char *pReturnString);
void*   DLLEXPORT       PowerSupply_GetAlarmStatusRegister(int viInstrumentHandle, int channel, char *pReturnString);
void*   DLLEXPORT       PowerSupply_GetErrorCodesRegister(int viInstrumentHandle, int channel, char *pReturnString);
void*   DLLEXPORT       PowerSupply_GetCompleteStatus(int viInstrumentHandle, int channel, char *pReturnString);
void*   DLLEXPORT       PowerSupply_SetTrackingMode(int viInstrumentHandle, int channel, int iMode);
void*   DLLEXPORT       PowerSupply_GetTrackingMode(int viInstrumentHandle, int channel, int *piMode);
void*   DLLEXPORT       PowerSupply_SetVoltageRange(int viInstrumentHandle, int channel, double lfVoltage);
void*   DLLEXPORT       PowerSupply_SetCurrentRange(int viInstrumentHandle, int channel, double lfCurrent);
void*   DLLEXPORT       PowerSupply_GetRange(int viInstrumentHandle, int channel, int *piRange);
void*   DLLEXPORT       PowerSupply_SetRange(int viInstrumentHandle, int channel, int iRange);
void*   DLLEXPORT       PowerSupply_SetRemoteSense(int viInstrumentHandle, int channel, int bState);
void*   DLLEXPORT       PowerSupply_GetRemoteSense(int viInstrumentHandle, int channel, int *pbState);
void*   DLLEXPORT       PowerSupply_GetVoltageRange(int viInstrumentHandle, int channel, double *plfVoltage);
void*   DLLEXPORT       PowerSupply_GetCurrentRange(int viInstrumentHandle, int channel, double *plfCurrent);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __TTI_PL303P_wrapper_H__ */
