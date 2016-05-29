#include "ModZT.h"

typedef interface tagModularZT__USB_Control_Interface ModularZT__USB_Control_Interface;

typedef struct tagModularZT__USB_Control_VTable
{
	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( ModularZT__USB_Control_Interface __RPC_FAR * This, 
	                                                         REFIID riid, 
	                                                         void __RPC_FAR *__RPC_FAR *ppvObject);

	ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( ModularZT__USB_Control_Interface __RPC_FAR * This);

	ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( ModularZT__USB_Control_Interface __RPC_FAR * This);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( ModularZT__USB_Control_Interface __RPC_FAR * This, 
	                                                           UINT __RPC_FAR *pctinfo);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( ModularZT__USB_Control_Interface __RPC_FAR * This, 
	                                                      UINT iTInfo, 
	                                                      LCID lcid, 
	                                                      ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( ModularZT__USB_Control_Interface __RPC_FAR * This, 
	                                                        REFIID riid, 
	                                                        LPOLESTR __RPC_FAR *rgszNames, 
	                                                        UINT cNames, 
	                                                        LCID lcid, 
	                                                        DISPID __RPC_FAR *rgDispId);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( ModularZT__USB_Control_Interface __RPC_FAR * This, 
	                                                 DISPID dispIdMember, 
	                                                 REFIID riid, 
	                                                 LCID lcid, 
	                                                 WORD wFlags, 
	                                                 DISPPARAMS __RPC_FAR *pDispParams, 
	                                                 VARIANT __RPC_FAR *pVarResult, 
	                                                 EXCEPINFO __RPC_FAR *pExcepInfo, 
	                                                 UINT __RPC_FAR *puArgErr);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetExtFirmware_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                         short *a0, 
	                                                         short *a1, 
	                                                         short *a2, 
	                                                         short *a3, 
	                                                         BSTR *docFirmware, 
	                                                         short *arg6);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ResetCPU_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                   unsigned char *arg1);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ResetDevice_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                      unsigned char *arg1);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetUSBDeviceName_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                           BSTR *arg1);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetConnectionStatus_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                              short *arg1);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetUSBConnectionStatus_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                                 short *arg1);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get_Available_SN_List_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                                BSTR *SN_List, 
	                                                                short *arg2);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get_Available_Address_List_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                                     BSTR *SN_List, 
	                                                                     short *arg2);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveEthernet_IPAddress_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                                 short b1, 
	                                                                 short b2, 
	                                                                 short b3, 
	                                                                 short b4, 
	                                                                 short *arg5);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEthernet_IPAddress_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                                short *b1, 
	                                                                short *b2, 
	                                                                short *b3, 
	                                                                short *b4, 
	                                                                short *arg5);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEthernet_MACAddress_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                                 short *MAC1, 
	                                                                 short *MAC2, 
	                                                                 short *MAC3, 
	                                                                 short *MAC4, 
	                                                                 short *MAC5, 
	                                                                 short *MAC6, 
	                                                                 short *arg7);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEthernet_CurrentConfig_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
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

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEthernet_SubNetMask_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                                 short *b1, 
	                                                                 short *b2, 
	                                                                 short *b3, 
	                                                                 short *b4, 
	                                                                 short *arg5);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEthernet_NetworkGateway_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                                     short *b1, 
	                                                                     short *b2, 
	                                                                     short *b3, 
	                                                                     short *b4, 
	                                                                     short *arg5);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveEthernet_TCPIPPort_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                                 short p, 
	                                                                 short *arg2);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEthernet_TCPIPPort_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                                short *p, 
	                                                                short *arg2);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveEthernet_PWD_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                           BSTR PWD, 
	                                                           short *arg2);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEthernet_PWD_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                          BSTR *PWD, 
	                                                          short *arg2);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEthernet_UseDHCP_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                              short *arg1);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveEthernet_UsePWD_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                              short usePwd, 
	                                                              short *arg2);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveEthernet_UseDHCP_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                               short useDHCP, 
	                                                               short *arg2);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEthernet_UsePWD_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                             short *arg1);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveEthernet_SubnetMask_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                                  short b1, 
	                                                                  short b2, 
	                                                                  short b3, 
	                                                                  short b4, 
	                                                                  short *arg5);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveEthernet_NetworkGateway_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                                      short b1, 
	                                                                      short b2, 
	                                                                      short b3, 
	                                                                      short b4, 
	                                                                      short *arg5);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Read_ModelName_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                         BSTR *modelName, 
	                                                         short *arg2);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetFirmware_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                      short *arg1);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Read_SN_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                  BSTR *SN, 
	                                                  short *arg2);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Set_Address_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                      short *address, 
	                                                      unsigned char *arg2);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get_Address_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                      short *arg1);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Send_SCPI_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                    BSTR *sndSTR, 
	                                                    BSTR *retSTR, 
	                                                    short *arg3);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WriteEEPROM_LastByte_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                               short val, 
	                                                               short *arg2);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Connect_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                  BSTR SN, 
	                                                  short *arg2);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConnectByAddress_) (ModularZT__USB_Control_Interface __RPC_FAR *This, 
	                                                           short address, 
	                                                           short *arg2);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Disconnect_) (ModularZT__USB_Control_Interface __RPC_FAR *This);

} ModularZT__USB_Control_VTable;

typedef interface tagModularZT__USB_Control_Interface
{
	CONST_VTBL ModularZT__USB_Control_VTable __RPC_FAR *lpVtbl;
} ModularZT__USB_Control_Interface;

const IID ModularZT_IID__USB_Control =
	{
		0xEDCE2C73, 0x3BED, 0x4B1B, 0xB4, 0xCA, 0xE5, 0x99, 0xF, 0x75, 0x86, 0xF0
	};

HRESULT CVIFUNC ModularZT_New_USB_Control (const char *server,
                                           int supportMultithreading,
                                           LCID locale, int reserved,
                                           CAObjHandle *objectHandle)
{
	HRESULT __result = S_OK;
	GUID clsid = {0xADD2241C, 0x225A, 0x43FD, 0xAE, 0x6B, 0x2D, 0x29, 0x4B,
	              0xF7, 0x51, 0x39};

	__result = CA_CreateObjectByClassIdEx (&clsid, server,
	                                       &ModularZT_IID__USB_Control,
	                                       supportMultithreading, locale,
	                                       reserved, objectHandle);

	return __result;
}

HRESULT CVIFUNC ModularZT_Open_USB_Control (const char *fileName,
                                            const char *server,
                                            int supportMultithreading,
                                            LCID locale, int reserved,
                                            CAObjHandle *objectHandle)
{
	HRESULT __result = S_OK;
	GUID clsid = {0xADD2241C, 0x225A, 0x43FD, 0xAE, 0x6B, 0x2D, 0x29, 0x4B,
	              0xF7, 0x51, 0x39};

	__result = CA_LoadObjectFromFileByClassIdEx (fileName, &clsid, server,
	                                             &ModularZT_IID__USB_Control,
	                                             supportMultithreading, locale,
	                                             reserved, objectHandle);

	return __result;
}

HRESULT CVIFUNC ModularZT_Active_USB_Control (const char *server,
                                              int supportMultithreading,
                                              LCID locale, int reserved,
                                              CAObjHandle *objectHandle)
{
	HRESULT __result = S_OK;
	GUID clsid = {0xADD2241C, 0x225A, 0x43FD, 0xAE, 0x6B, 0x2D, 0x29, 0x4B,
	              0xF7, 0x51, 0x39};

	__result = CA_GetActiveObjectByClassIdEx (&clsid, server,
	                                          &ModularZT_IID__USB_Control,
	                                          supportMultithreading, locale,
	                                          reserved, objectHandle);

	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlGetExtFirmware (CAObjHandle objectHandle,
                                                      ERRORINFO *errorInfo,
                                                      short *a0, short *a1,
                                                      short *a2, short *a3,
                                                      char **docFirmware,
                                                      short *arg6)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	BSTR docFirmware__AutoType = 0;
	short arg6__Temp;

	__caErrChk (CA_CStringToBSTR (*docFirmware, &docFirmware__AutoType));

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->GetExtFirmware_ (__vtblIFacePtr, a0,
	                                                     a1, a2, a3,
	                                                     &docFirmware__AutoType,
	                                                     &arg6__Temp));

	if (docFirmware)
		{
		CA_FreeMemory (*docFirmware);
		*docFirmware = 0;
		}
	if (docFirmware)
		__caErrChk (CA_BSTRGetCString (docFirmware__AutoType, docFirmware));
	if (arg6)
		{
		*arg6 = arg6__Temp;
		}

Error:
	CA_FreeBSTR (docFirmware__AutoType);
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	if (FAILED(__result))
		{
		if (docFirmware)
			{
			CA_FreeMemory (*docFirmware);
			*docFirmware = 0;
			}
		}
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlResetCPU (CAObjHandle objectHandle,
                                                ERRORINFO *errorInfo,
                                                unsigned char *arg1)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	unsigned char arg1__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->ResetCPU_ (__vtblIFacePtr, &arg1__Temp));

	if (arg1)
		{
		*arg1 = arg1__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlResetDevice (CAObjHandle objectHandle,
                                                   ERRORINFO *errorInfo,
                                                   unsigned char *arg1)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	unsigned char arg1__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->ResetDevice_ (__vtblIFacePtr,
	                                                  &arg1__Temp));

	if (arg1)
		{
		*arg1 = arg1__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlGetUSBDeviceName (CAObjHandle objectHandle,
                                                        ERRORINFO *errorInfo,
                                                        char **arg1)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	BSTR arg1__AutoType = 0;

	if (arg1)
		*arg1 = 0;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->GetUSBDeviceName_ (__vtblIFacePtr,
	                                                       &arg1__AutoType));

	if (arg1)
		__caErrChk (CA_BSTRGetCString (arg1__AutoType, arg1));

Error:
	CA_FreeBSTR (arg1__AutoType);
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	if (FAILED(__result))
		{
		if (arg1)
			{
			CA_FreeMemory (*arg1);
			*arg1 = 0;
			}
		}
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlGetConnectionStatus (CAObjHandle objectHandle,
                                                           ERRORINFO *errorInfo,
                                                           short *arg1)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	short arg1__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->GetConnectionStatus_ (__vtblIFacePtr,
	                                                          &arg1__Temp));

	if (arg1)
		{
		*arg1 = arg1__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlGetUSBConnectionStatus (CAObjHandle objectHandle,
                                                              ERRORINFO *errorInfo,
                                                              short *arg1)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	short arg1__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->GetUSBConnectionStatus_ (__vtblIFacePtr,
	                                                             &arg1__Temp));

	if (arg1)
		{
		*arg1 = arg1__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlGet_Available_SN_List (CAObjHandle objectHandle,
                                                             ERRORINFO *errorInfo,
                                                             char **SN_List,
                                                             short *arg2)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	BSTR SN_List__AutoType = 0;
	short arg2__Temp;

	__caErrChk (CA_CStringToBSTR (*SN_List, &SN_List__AutoType));

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->Get_Available_SN_List_ (__vtblIFacePtr,
	                                                            &SN_List__AutoType,
	                                                            &arg2__Temp));

	if (SN_List)
		{
		CA_FreeMemory (*SN_List);
		*SN_List = 0;
		}
	if (SN_List)
		__caErrChk (CA_BSTRGetCString (SN_List__AutoType, SN_List));
	if (arg2)
		{
		*arg2 = arg2__Temp;
		}

Error:
	CA_FreeBSTR (SN_List__AutoType);
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	if (FAILED(__result))
		{
		if (SN_List)
			{
			CA_FreeMemory (*SN_List);
			*SN_List = 0;
			}
		}
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlGet_Available_Address_List (CAObjHandle objectHandle,
                                                                  ERRORINFO *errorInfo,
                                                                  char **SN_List,
                                                                  short *arg2)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	BSTR SN_List__AutoType = 0;
	short arg2__Temp;

	__caErrChk (CA_CStringToBSTR (*SN_List, &SN_List__AutoType));

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->Get_Available_Address_List_ (__vtblIFacePtr,
	                                                                 &SN_List__AutoType,
	                                                                 &arg2__Temp));

	if (SN_List)
		{
		CA_FreeMemory (*SN_List);
		*SN_List = 0;
		}
	if (SN_List)
		__caErrChk (CA_BSTRGetCString (SN_List__AutoType, SN_List));
	if (arg2)
		{
		*arg2 = arg2__Temp;
		}

Error:
	CA_FreeBSTR (SN_List__AutoType);
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	if (FAILED(__result))
		{
		if (SN_List)
			{
			CA_FreeMemory (*SN_List);
			*SN_List = 0;
			}
		}
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlSaveEthernet_IPAddress (CAObjHandle objectHandle,
                                                              ERRORINFO *errorInfo,
                                                              short b1, short b2,
                                                              short b3, short b4,
                                                              short *arg5)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	short arg5__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->SaveEthernet_IPAddress_ (__vtblIFacePtr,
	                                                             b1, b2, b3,
	                                                             b4,
	                                                             &arg5__Temp));

	if (arg5)
		{
		*arg5 = arg5__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlGetEthernet_IPAddress (CAObjHandle objectHandle,
                                                             ERRORINFO *errorInfo,
                                                             short *b1,
                                                             short *b2,
                                                             short *b3,
                                                             short *b4,
                                                             short *arg5)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	short arg5__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->GetEthernet_IPAddress_ (__vtblIFacePtr,
	                                                            b1, b2, b3, b4,
	                                                            &arg5__Temp));

	if (arg5)
		{
		*arg5 = arg5__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlGetEthernet_MACAddress (CAObjHandle objectHandle,
                                                              ERRORINFO *errorInfo,
                                                              short *MAC1,
                                                              short *MAC2,
                                                              short *MAC3,
                                                              short *MAC4,
                                                              short *MAC5,
                                                              short *MAC6,
                                                              short *arg7)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	short arg7__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->GetEthernet_MACAddress_ (__vtblIFacePtr,
	                                                             MAC1, MAC2,
	                                                             MAC3, MAC4,
	                                                             MAC5, MAC6,
	                                                             &arg7__Temp));

	if (arg7)
		{
		*arg7 = arg7__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

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
                                                                 short *arg13)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	short arg13__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->GetEthernet_CurrentConfig_ (__vtblIFacePtr,
	                                                                IP1, IP2,
	                                                                IP3, IP4,
	                                                                mask1,
	                                                                mask2,
	                                                                mask3,
	                                                                mask4,
	                                                                gateway1,
	                                                                gateway2,
	                                                                gateway3,
	                                                                gateway4,
	                                                                &arg13__Temp));

	if (arg13)
		{
		*arg13 = arg13__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlGetEthernet_SubNetMask (CAObjHandle objectHandle,
                                                              ERRORINFO *errorInfo,
                                                              short *b1,
                                                              short *b2,
                                                              short *b3,
                                                              short *b4,
                                                              short *arg5)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	short arg5__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->GetEthernet_SubNetMask_ (__vtblIFacePtr,
	                                                             b1, b2, b3,
	                                                             b4,
	                                                             &arg5__Temp));

	if (arg5)
		{
		*arg5 = arg5__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlGetEthernet_NetworkGateway (CAObjHandle objectHandle,
                                                                  ERRORINFO *errorInfo,
                                                                  short *b1,
                                                                  short *b2,
                                                                  short *b3,
                                                                  short *b4,
                                                                  short *arg5)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	short arg5__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->GetEthernet_NetworkGateway_ (__vtblIFacePtr,
	                                                                 b1, b2,
	                                                                 b3, b4,
	                                                                 &arg5__Temp));

	if (arg5)
		{
		*arg5 = arg5__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlSaveEthernet_TCPIPPort (CAObjHandle objectHandle,
                                                              ERRORINFO *errorInfo,
                                                              short p,
                                                              short *arg2)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	short arg2__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->SaveEthernet_TCPIPPort_ (__vtblIFacePtr,
	                                                             p,
	                                                             &arg2__Temp));

	if (arg2)
		{
		*arg2 = arg2__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlGetEthernet_TCPIPPort (CAObjHandle objectHandle,
                                                             ERRORINFO *errorInfo,
                                                             short *p,
                                                             short *arg2)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	short arg2__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->GetEthernet_TCPIPPort_ (__vtblIFacePtr,
	                                                            p, &arg2__Temp));

	if (arg2)
		{
		*arg2 = arg2__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlSaveEthernet_PWD (CAObjHandle objectHandle,
                                                        ERRORINFO *errorInfo,
                                                        const char *PWD,
                                                        short *arg2)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	BSTR PWD__AutoType = 0;
	short arg2__Temp;

	__caErrChk (CA_CStringToBSTR (PWD, &PWD__AutoType));

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->SaveEthernet_PWD_ (__vtblIFacePtr,
	                                                       PWD__AutoType,
	                                                       &arg2__Temp));

	if (arg2)
		{
		*arg2 = arg2__Temp;
		}

Error:
	CA_FreeBSTR (PWD__AutoType);
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlGetEthernet_PWD (CAObjHandle objectHandle,
                                                       ERRORINFO *errorInfo,
                                                       char **PWD, short *arg2)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	BSTR PWD__AutoType = 0;
	short arg2__Temp;

	__caErrChk (CA_CStringToBSTR (*PWD, &PWD__AutoType));

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->GetEthernet_PWD_ (__vtblIFacePtr,
	                                                      &PWD__AutoType,
	                                                      &arg2__Temp));

	if (PWD)
		{
		CA_FreeMemory (*PWD);
		*PWD = 0;
		}
	if (PWD)
		__caErrChk (CA_BSTRGetCString (PWD__AutoType, PWD));
	if (arg2)
		{
		*arg2 = arg2__Temp;
		}

Error:
	CA_FreeBSTR (PWD__AutoType);
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	if (FAILED(__result))
		{
		if (PWD)
			{
			CA_FreeMemory (*PWD);
			*PWD = 0;
			}
		}
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlGetEthernet_UseDHCP (CAObjHandle objectHandle,
                                                           ERRORINFO *errorInfo,
                                                           short *arg1)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	short arg1__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->GetEthernet_UseDHCP_ (__vtblIFacePtr,
	                                                          &arg1__Temp));

	if (arg1)
		{
		*arg1 = arg1__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlSaveEthernet_UsePWD (CAObjHandle objectHandle,
                                                           ERRORINFO *errorInfo,
                                                           short usePwd,
                                                           short *arg2)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	short arg2__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->SaveEthernet_UsePWD_ (__vtblIFacePtr,
	                                                          usePwd,
	                                                          &arg2__Temp));

	if (arg2)
		{
		*arg2 = arg2__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlSaveEthernet_UseDHCP (CAObjHandle objectHandle,
                                                            ERRORINFO *errorInfo,
                                                            short useDHCP,
                                                            short *arg2)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	short arg2__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->SaveEthernet_UseDHCP_ (__vtblIFacePtr,
	                                                           useDHCP,
	                                                           &arg2__Temp));

	if (arg2)
		{
		*arg2 = arg2__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlGetEthernet_UsePWD (CAObjHandle objectHandle,
                                                          ERRORINFO *errorInfo,
                                                          short *arg1)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	short arg1__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->GetEthernet_UsePWD_ (__vtblIFacePtr,
	                                                         &arg1__Temp));

	if (arg1)
		{
		*arg1 = arg1__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlSaveEthernet_SubnetMask (CAObjHandle objectHandle,
                                                               ERRORINFO *errorInfo,
                                                               short b1,
                                                               short b2,
                                                               short b3,
                                                               short b4,
                                                               short *arg5)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	short arg5__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->SaveEthernet_SubnetMask_ (__vtblIFacePtr,
	                                                              b1, b2, b3,
	                                                              b4,
	                                                              &arg5__Temp));

	if (arg5)
		{
		*arg5 = arg5__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlSaveEthernet_NetworkGateway (CAObjHandle objectHandle,
                                                                   ERRORINFO *errorInfo,
                                                                   short b1,
                                                                   short b2,
                                                                   short b3,
                                                                   short b4,
                                                                   short *arg5)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	short arg5__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->SaveEthernet_NetworkGateway_ (__vtblIFacePtr,
	                                                                  b1, b2,
	                                                                  b3, b4,
	                                                                  &arg5__Temp));

	if (arg5)
		{
		*arg5 = arg5__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlRead_ModelName (CAObjHandle objectHandle,
                                                      ERRORINFO *errorInfo,
                                                      char **modelName,
                                                      short *arg2)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	BSTR modelName__AutoType = 0;
	short arg2__Temp;

	__caErrChk (CA_CStringToBSTR (*modelName, &modelName__AutoType));

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->Read_ModelName_ (__vtblIFacePtr,
	                                                     &modelName__AutoType,
	                                                     &arg2__Temp));

	if (modelName)
		{
		CA_FreeMemory (*modelName);
		*modelName = 0;
		}
	if (modelName)
		__caErrChk (CA_BSTRGetCString (modelName__AutoType, modelName));
	if (arg2)
		{
		*arg2 = arg2__Temp;
		}

Error:
	CA_FreeBSTR (modelName__AutoType);
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	if (FAILED(__result))
		{
		if (modelName)
			{
			CA_FreeMemory (*modelName);
			*modelName = 0;
			}
		}
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlGetFirmware (CAObjHandle objectHandle,
                                                   ERRORINFO *errorInfo,
                                                   short *arg1)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	short arg1__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->GetFirmware_ (__vtblIFacePtr,
	                                                  &arg1__Temp));

	if (arg1)
		{
		*arg1 = arg1__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlRead_SN (CAObjHandle objectHandle,
                                               ERRORINFO *errorInfo, char **SN,
                                               short *arg2)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	BSTR SN__AutoType = 0;
	short arg2__Temp;

	__caErrChk (CA_CStringToBSTR (*SN, &SN__AutoType));

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->Read_SN_ (__vtblIFacePtr,
	                                              &SN__AutoType, &arg2__Temp));

	if (SN)
		{
		CA_FreeMemory (*SN);
		*SN = 0;
		}
	if (SN)
		__caErrChk (CA_BSTRGetCString (SN__AutoType, SN));
	if (arg2)
		{
		*arg2 = arg2__Temp;
		}

Error:
	CA_FreeBSTR (SN__AutoType);
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	if (FAILED(__result))
		{
		if (SN)
			{
			CA_FreeMemory (*SN);
			*SN = 0;
			}
		}
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlSet_Address (CAObjHandle objectHandle,
                                                   ERRORINFO *errorInfo,
                                                   short *address,
                                                   unsigned char *arg2)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	unsigned char arg2__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->Set_Address_ (__vtblIFacePtr, address,
	                                                  &arg2__Temp));

	if (arg2)
		{
		*arg2 = arg2__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlGet_Address (CAObjHandle objectHandle,
                                                   ERRORINFO *errorInfo,
                                                   short *arg1)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	short arg1__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->Get_Address_ (__vtblIFacePtr,
	                                                  &arg1__Temp));

	if (arg1)
		{
		*arg1 = arg1__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlSend_SCPI (CAObjHandle objectHandle,
                                                 ERRORINFO *errorInfo,
                                                 char **sndSTR, char **retSTR,
                                                 short *arg3)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	BSTR sndSTR__AutoType = 0;
	BSTR retSTR__AutoType = 0;
	short arg3__Temp;

	__caErrChk (CA_CStringToBSTR (*sndSTR, &sndSTR__AutoType));
	__caErrChk (CA_CStringToBSTR (*retSTR, &retSTR__AutoType));

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->Send_SCPI_ (__vtblIFacePtr,
	                                                &sndSTR__AutoType,
	                                                &retSTR__AutoType,
	                                                &arg3__Temp));

	if (sndSTR)
		{
		CA_FreeMemory (*sndSTR);
		*sndSTR = 0;
		}
	if (sndSTR)
		__caErrChk (CA_BSTRGetCString (sndSTR__AutoType, sndSTR));
	if (retSTR)
		{
		CA_FreeMemory (*retSTR);
		*retSTR = 0;
		}
	if (retSTR)
		__caErrChk (CA_BSTRGetCString (retSTR__AutoType, retSTR));
	if (arg3)
		{
		*arg3 = arg3__Temp;
		}

Error:
	CA_FreeBSTR (sndSTR__AutoType);
	CA_FreeBSTR (retSTR__AutoType);
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	if (FAILED(__result))
		{
		if (sndSTR)
			{
			CA_FreeMemory (*sndSTR);
			*sndSTR = 0;
			}
		if (retSTR)
			{
			CA_FreeMemory (*retSTR);
			*retSTR = 0;
			}
		}
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlWriteEEPROM_LastByte (CAObjHandle objectHandle,
                                                            ERRORINFO *errorInfo,
                                                            short val,
                                                            short *arg2)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	short arg2__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->WriteEEPROM_LastByte_ (__vtblIFacePtr,
	                                                           val,
	                                                           &arg2__Temp));

	if (arg2)
		{
		*arg2 = arg2__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlConnect (CAObjHandle objectHandle,
                                               ERRORINFO *errorInfo,
                                               const char *SN, short *arg2)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	BSTR SN__AutoType = 0;
	short arg2__Temp;

	__caErrChk (CA_CStringToBSTR (SN, &SN__AutoType));

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->Connect_ (__vtblIFacePtr, SN__AutoType,
	                                              &arg2__Temp));

	if (arg2)
		{
		*arg2 = arg2__Temp;
		}

Error:
	CA_FreeBSTR (SN__AutoType);
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlConnectByAddress (CAObjHandle objectHandle,
                                                        ERRORINFO *errorInfo,
                                                        short address,
                                                        short *arg2)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;
	short arg2__Temp;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->ConnectByAddress_ (__vtblIFacePtr,
	                                                       address,
	                                                       &arg2__Temp));

	if (arg2)
		{
		*arg2 = arg2__Temp;
		}

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}

HRESULT CVIFUNC ModularZT__USB_ControlDisconnect (CAObjHandle objectHandle,
                                                  ERRORINFO *errorInfo)
{
	HRESULT __result = S_OK;
	ModularZT__USB_Control_Interface * __vtblIFacePtr = 0;
	int __didAddRef;
	int __errorInfoPresent = 0;

	__caErrChk (CA_GetInterfaceFromObjHandle (objectHandle,
	                                          &ModularZT_IID__USB_Control, 0,
	                                          &__vtblIFacePtr, &__didAddRef));
	__caErrChk (__vtblIFacePtr->lpVtbl->Disconnect_ (__vtblIFacePtr));

Error:
	if (__vtblIFacePtr && __didAddRef)
		__vtblIFacePtr->lpVtbl->Release (__vtblIFacePtr);
	CA_FillErrorInfoEx (objectHandle, &ModularZT_IID__USB_Control, __result,
	                    errorInfo, &__errorInfoPresent);
	if (__errorInfoPresent)
		__result = DISP_E_EXCEPTION;
	return __result;
}
