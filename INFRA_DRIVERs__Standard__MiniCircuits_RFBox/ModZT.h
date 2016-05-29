#ifndef _MODULARZT_H
#define _MODULARZT_H

#include <cviauto.h>

#ifdef __cplusplus
    extern "C" {
#endif
extern const IID ModularZT_IID__USB_Control;

HRESULT CVIFUNC ModularZT_New_USB_Control (const char *server,
                                           int supportMultithreading,
                                           LCID locale, int reserved,
                                           CAObjHandle *objectHandle);

HRESULT CVIFUNC ModularZT_Open_USB_Control (const char *fileName,
                                            const char *server,
                                            int supportMultithreading,
                                            LCID locale, int reserved,
                                            CAObjHandle *objectHandle);

HRESULT CVIFUNC ModularZT_Active_USB_Control (const char *server,
                                              int supportMultithreading,
                                              LCID locale, int reserved,
                                              CAObjHandle *objectHandle);

HRESULT CVIFUNC ModularZT__USB_ControlGetExtFirmware (CAObjHandle objectHandle,
                                                      ERRORINFO *errorInfo,
                                                      short *a0, short *a1,
                                                      short *a2, short *a3,
                                                      char **docFirmware,
                                                      short *arg6);

HRESULT CVIFUNC ModularZT__USB_ControlResetCPU (CAObjHandle objectHandle,
                                                ERRORINFO *errorInfo,
                                                unsigned char *arg1);

HRESULT CVIFUNC ModularZT__USB_ControlResetDevice (CAObjHandle objectHandle,
                                                   ERRORINFO *errorInfo,
                                                   unsigned char *arg1);

HRESULT CVIFUNC ModularZT__USB_ControlGetUSBDeviceName (CAObjHandle objectHandle,
                                                        ERRORINFO *errorInfo,
                                                        char **arg1);

HRESULT CVIFUNC ModularZT__USB_ControlGetConnectionStatus (CAObjHandle objectHandle,
                                                           ERRORINFO *errorInfo,
                                                           short *arg1);

HRESULT CVIFUNC ModularZT__USB_ControlGetUSBConnectionStatus (CAObjHandle objectHandle,
                                                              ERRORINFO *errorInfo,
                                                              short *arg1);

HRESULT CVIFUNC ModularZT__USB_ControlGet_Available_SN_List (CAObjHandle objectHandle,
                                                             ERRORINFO *errorInfo,
                                                             char **SN_List,
                                                             short *arg2);

HRESULT CVIFUNC ModularZT__USB_ControlGet_Available_Address_List (CAObjHandle objectHandle,
                                                                  ERRORINFO *errorInfo,
                                                                  char **SN_List,
                                                                  short *arg2);

HRESULT CVIFUNC ModularZT__USB_ControlSaveEthernet_IPAddress (CAObjHandle objectHandle,
                                                              ERRORINFO *errorInfo,
                                                              short b1, short b2,
                                                              short b3, short b4,
                                                              short *arg5);

HRESULT CVIFUNC ModularZT__USB_ControlGetEthernet_IPAddress (CAObjHandle objectHandle,
                                                             ERRORINFO *errorInfo,
                                                             short *b1,
                                                             short *b2,
                                                             short *b3,
                                                             short *b4,
                                                             short *arg5);

HRESULT CVIFUNC ModularZT__USB_ControlGetEthernet_MACAddress (CAObjHandle objectHandle,
                                                              ERRORINFO *errorInfo,
                                                              short *MAC1,
                                                              short *MAC2,
                                                              short *MAC3,
                                                              short *MAC4,
                                                              short *MAC5,
                                                              short *MAC6,
                                                              short *arg7);

HRESULT CVIFUNC ModularZT__USB_ControlGetEthernet_CurrentConfig (CAObjHandle objectHandle,
                                                                 ERRORINFO *errorInfo,
                                                                 short *IP1,
                                                                 short *IP2,
                                                                 short *IP3,
                                                                 short *IP4,
                                                                 short *mask1,
                                                                 short *mask2,
                                                                 short *mask3,
                                                                 short *mask4,
                                                                 short *gateway1,
                                                                 short *gateway2,
                                                                 short *gateway3,
                                                                 short *gateway4,
                                                                 short *arg13);

HRESULT CVIFUNC ModularZT__USB_ControlGetEthernet_SubNetMask (CAObjHandle objectHandle,
                                                              ERRORINFO *errorInfo,
                                                              short *b1,
                                                              short *b2,
                                                              short *b3,
                                                              short *b4,
                                                              short *arg5);

HRESULT CVIFUNC ModularZT__USB_ControlGetEthernet_NetworkGateway (CAObjHandle objectHandle,
                                                                  ERRORINFO *errorInfo,
                                                                  short *b1,
                                                                  short *b2,
                                                                  short *b3,
                                                                  short *b4,
                                                                  short *arg5);

HRESULT CVIFUNC ModularZT__USB_ControlSaveEthernet_TCPIPPort (CAObjHandle objectHandle,
                                                              ERRORINFO *errorInfo,
                                                              short p,
                                                              short *arg2);

HRESULT CVIFUNC ModularZT__USB_ControlGetEthernet_TCPIPPort (CAObjHandle objectHandle,
                                                             ERRORINFO *errorInfo,
                                                             short *p,
                                                             short *arg2);

HRESULT CVIFUNC ModularZT__USB_ControlSaveEthernet_PWD (CAObjHandle objectHandle,
                                                        ERRORINFO *errorInfo,
                                                        const char *PWD,
                                                        short *arg2);

HRESULT CVIFUNC ModularZT__USB_ControlGetEthernet_PWD (CAObjHandle objectHandle,
                                                       ERRORINFO *errorInfo,
                                                       char **PWD, short *arg2);

HRESULT CVIFUNC ModularZT__USB_ControlGetEthernet_UseDHCP (CAObjHandle objectHandle,
                                                           ERRORINFO *errorInfo,
                                                           short *arg1);

HRESULT CVIFUNC ModularZT__USB_ControlSaveEthernet_UsePWD (CAObjHandle objectHandle,
                                                           ERRORINFO *errorInfo,
                                                           short usePwd,
                                                           short *arg2);

HRESULT CVIFUNC ModularZT__USB_ControlSaveEthernet_UseDHCP (CAObjHandle objectHandle,
                                                            ERRORINFO *errorInfo,
                                                            short useDHCP,
                                                            short *arg2);

HRESULT CVIFUNC ModularZT__USB_ControlGetEthernet_UsePWD (CAObjHandle objectHandle,
                                                          ERRORINFO *errorInfo,
                                                          short *arg1);

HRESULT CVIFUNC ModularZT__USB_ControlSaveEthernet_SubnetMask (CAObjHandle objectHandle,
                                                               ERRORINFO *errorInfo,
                                                               short b1,
                                                               short b2,
                                                               short b3,
                                                               short b4,
                                                               short *arg5);

HRESULT CVIFUNC ModularZT__USB_ControlSaveEthernet_NetworkGateway (CAObjHandle objectHandle,
                                                                   ERRORINFO *errorInfo,
                                                                   short b1,
                                                                   short b2,
                                                                   short b3,
                                                                   short b4,
                                                                   short *arg5);

HRESULT CVIFUNC ModularZT__USB_ControlRead_ModelName (CAObjHandle objectHandle,
                                                      ERRORINFO *errorInfo,
                                                      char **modelName,
                                                      short *arg2);

HRESULT CVIFUNC ModularZT__USB_ControlGetFirmware (CAObjHandle objectHandle,
                                                   ERRORINFO *errorInfo,
                                                   short *arg1);

HRESULT CVIFUNC ModularZT__USB_ControlRead_SN (CAObjHandle objectHandle,
                                               ERRORINFO *errorInfo, char **SN,
                                               short *arg2);

HRESULT CVIFUNC ModularZT__USB_ControlSet_Address (CAObjHandle objectHandle,
                                                   ERRORINFO *errorInfo,
                                                   short *address,
                                                   unsigned char *arg2);

HRESULT CVIFUNC ModularZT__USB_ControlGet_Address (CAObjHandle objectHandle,
                                                   ERRORINFO *errorInfo,
                                                   short *arg1);

HRESULT CVIFUNC ModularZT__USB_ControlSend_SCPI (CAObjHandle objectHandle,
                                                 ERRORINFO *errorInfo,
                                                 char **sndSTR, char **retSTR,
                                                 short *arg3);

HRESULT CVIFUNC ModularZT__USB_ControlWriteEEPROM_LastByte (CAObjHandle objectHandle,
                                                            ERRORINFO *errorInfo,
                                                            short val,
                                                            short *arg2);

HRESULT CVIFUNC ModularZT__USB_ControlConnect (CAObjHandle objectHandle,
                                               ERRORINFO *errorInfo,
                                               const char *SN, short *arg2);

HRESULT CVIFUNC ModularZT__USB_ControlConnectByAddress (CAObjHandle objectHandle,
                                                        ERRORINFO *errorInfo,
                                                        short address,
                                                        short *arg2);

HRESULT CVIFUNC ModularZT__USB_ControlDisconnect (CAObjHandle objectHandle,
                                                  ERRORINFO *errorInfo);
#ifdef __cplusplus
    }
#endif
#endif /* _MODULARZT_H */
