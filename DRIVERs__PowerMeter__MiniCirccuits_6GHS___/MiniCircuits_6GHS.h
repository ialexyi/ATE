#ifndef _MC_H
#define _MC_H

#include <cviauto.h>

#ifdef __cplusplus
    extern "C" {
#endif
extern const IID mc_IID__USB_PM_;

HRESULT CVIFUNC USB_PM_LOAD_DRIVER ( const char *server , int supportMultithreading , LCID locale, int reserved , void **Handle );
HRESULT CVIFUNC USB_PM_GetFreq (void *Handle,ERRORINFO *errorInfo, double *freq);
HRESULT CVIFUNC USB_PM_SetFreq (void *Handle,ERRORINFO *errorInfo, double freq);
HRESULT CVIFUNC USB_PM_GetAVG (void *Handle,ERRORINFO *errorInfo, short *AVG);
HRESULT CVIFUNC USB_PM_SetAVG (void *Handle,ERRORINFO *errorInfo, short AVG);
HRESULT CVIFUNC USB_PM_GetAvgCount (void *Handle,ERRORINFO *errorInfo, short *avgCount);
HRESULT CVIFUNC USB_PM_SetAvgCount (void *Handle,ERRORINFO *errorInfo, short avgCount);
HRESULT CVIFUNC USB_PM_GetFormat_mW (void *Handle,ERRORINFO *errorInfo, VBOOL *format_mW);
HRESULT CVIFUNC USB_PM_SetFormat_mW (void *Handle,ERRORINFO *errorInfo, VBOOL format_mW);
HRESULT CVIFUNC USB_PM_Open_AnySensor (void *Handle,ERRORINFO *errorInfo, short *arg1);
HRESULT CVIFUNC USB_PM_SetFasterMode (void *Handle,ERRORINFO *errorInfo, short *s_A);
HRESULT CVIFUNC USB_PM_ReadPower_AsLinux (void *Handle,ERRORINFO *errorInfo, float *arg1);
HRESULT CVIFUNC USB_PM_GetFirmwareVer (void *Handle,ERRORINFO *errorInfo,short *firmwareVer, long *arg2);
HRESULT CVIFUNC USB_PM_CloseConnection (void *Handle,ERRORINFO *errorInfo);
HRESULT CVIFUNC USB_PM_GetDeviceTemperature (void *Handle,ERRORINFO *errorInfo,const char *temperatureFormat,float *arg2);
HRESULT CVIFUNC USB_PM_GetSensorSN (void *Handle,ERRORINFO *errorInfo, char **arg1);
HRESULT CVIFUNC USB_PM_GetSensorModelName (void *Handle,ERRORINFO *errorInfo, char **arg1);
HRESULT CVIFUNC USB_PM_Init_PM (void *Handle,ERRORINFO *errorInfo);
HRESULT CVIFUNC USB_PM_Close_Sensor (void *Handle,ERRORINFO *errorInfo);
HRESULT CVIFUNC USB_PM_Get_Available_SN_List (void *Handle,ERRORINFO *errorInfo,char **SN_List, short *arg2);
HRESULT CVIFUNC USB_PM_GetStatus (void *Handle,ERRORINFO *errorInfo, short *arg1);
HRESULT CVIFUNC USB_PM_Open_Sensor (void *Handle,ERRORINFO *errorInfo,const char *SN_Request, short *arg2);
HRESULT CVIFUNC USB_PM_ReadPower (void *Handle,ERRORINFO *errorInfo, float *arg1);
HRESULT CVIFUNC USB_PM_ReadImmediatePower (void *Handle,ERRORINFO *errorInfo, float *arg1);
HRESULT CVIFUNC USB_PM_ReadVoltage (void *Handle,ERRORINFO *errorInfo, float *arg1);

#ifdef __cplusplus
    }
#endif
#endif /* _MC_H */
