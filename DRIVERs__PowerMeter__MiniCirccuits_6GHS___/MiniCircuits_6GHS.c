#include "MiniCircuits_6GHS.h"

typedef interface tagUSB_PM__Interface USB_PM__Interface;

typedef struct 
{
	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( USB_PM__Interface __RPC_FAR * This, 
	                                                         REFIID riid, 
	                                                         void __RPC_FAR *__RPC_FAR *ppvObject);

	ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( USB_PM__Interface __RPC_FAR * This);

	ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( USB_PM__Interface __RPC_FAR * This);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( USB_PM__Interface __RPC_FAR * This, 
	                                                           UINT __RPC_FAR *pctinfo);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( USB_PM__Interface __RPC_FAR * This, 
	                                                      UINT iTInfo, 
	                                                      LCID lcid, 
	                                                      ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( USB_PM__Interface __RPC_FAR * This, 
	                                                        REFIID riid, 
	                                                        LPOLESTR __RPC_FAR *rgszNames, 
	                                                        UINT cNames, 
	                                                        LCID lcid, 
	                                                        DISPID __RPC_FAR *rgDispId);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( USB_PM__Interface __RPC_FAR * This, 
	                                                 DISPID dispIdMember, 
	                                                 REFIID riid, 
	                                                 LCID lcid, 
	                                                 WORD wFlags, 
	                                                 DISPPARAMS __RPC_FAR *pDispParams, 
	                                                 VARIANT __RPC_FAR *pVarResult, 
	                                                 EXCEPINFO __RPC_FAR *pExcepInfo, 
	                                                 UINT __RPC_FAR *puArgErr);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetFreq_) (USB_PM__Interface __RPC_FAR *This, 
	                                                  double *freq);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetFreq_) (USB_PM__Interface __RPC_FAR *This, 
	                                                  double freq);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAVG_) (USB_PM__Interface __RPC_FAR *This, 
	                                                 short *AVG);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetAVG_) (USB_PM__Interface __RPC_FAR *This, 
	                                                 short AVG);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAvgCount_) (USB_PM__Interface __RPC_FAR *This, 
	                                                      short *avgCount);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetAvgCount_) (USB_PM__Interface __RPC_FAR *This, 
	                                                      short avgCount);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetFormat_mW_) (USB_PM__Interface __RPC_FAR *This, 
	                                                       VBOOL *format_mW);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetFormat_mW_) (USB_PM__Interface __RPC_FAR *This, 
	                                                       VBOOL format_mW);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Open_AnySensor_) (USB_PM__Interface __RPC_FAR *This, 
	                                                         short *arg1);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetFasterMode_) (USB_PM__Interface __RPC_FAR *This, 
	                                                        short *s_A);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReadPower_AsLinux_) (USB_PM__Interface __RPC_FAR *This, 
	                                                            float *arg1);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetFirmwareVer_) (USB_PM__Interface __RPC_FAR *This, 
	                                                         short *firmwareVer, 
	                                                         long *arg2);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CloseConnection_) (USB_PM__Interface __RPC_FAR *This);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDeviceTemperature_) (USB_PM__Interface __RPC_FAR *This, 
	                                                               BSTR temperatureFormat, 
	                                                               float *arg2);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSensorSN_) (USB_PM__Interface __RPC_FAR *This, 
	                                                      BSTR *arg1);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSensorModelName_) (USB_PM__Interface __RPC_FAR *This, 
	                                                             BSTR *arg1);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Init_PM_) (USB_PM__Interface __RPC_FAR *This);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close_Sensor_) (USB_PM__Interface __RPC_FAR *This);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get_Available_SN_List_) (USB_PM__Interface __RPC_FAR *This, 
	                                                                BSTR *SN_List, 
	                                                                short *arg2);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetStatus_) (USB_PM__Interface __RPC_FAR *This, 
	                                                    short *arg1);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Open_Sensor_) (USB_PM__Interface __RPC_FAR *This, 
	                                                      BSTR SN_Request, 
	                                                      short *arg2);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReadPower_) (USB_PM__Interface __RPC_FAR *This, 
	                                                    float *arg1);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReadImmediatePower_) (USB_PM__Interface __RPC_FAR *This, 
	                                                             float *arg1);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReadVoltage_) (USB_PM__Interface __RPC_FAR *This, 
	                                                      float *arg1);

} USB_PM__VTable;

typedef interface tagUSB_PM__Interface
{
	CONST_VTBL USB_PM__VTable __RPC_FAR *lpVtbl;
} USB_PM__Interface;

HRESULT CVIFUNC USB_PM_LOAD_DRIVER ( const char *server , int supportMultithreading , LCID locale, int reserved , void **Handle )
{
	HRESULT						result			=	S_OK;
	GUID						clsid			=	{0};

	USB_PM__Interface			*pIDispatch		=	NULL; 
	
	CoInitialize(NULL);
		
	result = CLSIDFromProgID(OLESTR("mcl_pm.USB_PM"), &clsid);
		
	if ( result == 0)
	{
		result = CoCreateInstance( &clsid , NULL,CLSCTX_INPROC_SERVER , &IID_IDispatch, (void**)&pIDispatch); 

		if ( Handle )
			*Handle = pIDispatch;
	}
	
	return result;
}


HRESULT CVIFUNC USB_PM_GetFreq ( void *Handle , ERRORINFO *errorInfo, double *freq)
{
	HRESULT					__result			=	S_OK;
	double					freq__Temp			=	0.0;
	USB_PM__Interface		*pIDispatch			=	NULL;  
						
	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (pIDispatch->lpVtbl->GetFreq_ (pIDispatch, &freq__Temp));

	if (freq)
		{
		*freq = freq__Temp;
		}

Error:

	return __result;
}

HRESULT CVIFUNC USB_PM_SetFreq (void *Handle,ERRORINFO *errorInfo, double freq)
{
	HRESULT					__result			=	S_OK;
	USB_PM__Interface		*pIDispatch			=	NULL;  

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (pIDispatch->lpVtbl->SetFreq_ (pIDispatch, freq));

Error:

	return __result;
}

HRESULT CVIFUNC USB_PM_GetAVG (void *Handle,ERRORINFO *errorInfo, short *AVG)
{
	HRESULT					__result			=	S_OK;
	USB_PM__Interface		*pIDispatch			=	NULL;  
	short					AVG__Temp			=	0;

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (pIDispatch->lpVtbl->GetAVG_ (pIDispatch, &AVG__Temp));

	if (AVG)
		{
		*AVG = AVG__Temp;
		}

Error:

	return __result;
}

HRESULT CVIFUNC USB_PM_SetAVG (void *Handle,ERRORINFO *errorInfo, short AVG)
{
	HRESULT					__result			=	S_OK;
	USB_PM__Interface		*pIDispatch			=	NULL;  

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (pIDispatch->lpVtbl->SetAVG_ (pIDispatch, AVG));

Error:

	return __result;
}

HRESULT CVIFUNC USB_PM_GetAvgCount (void *Handle,ERRORINFO *errorInfo, short *avgCount)
{
	HRESULT					__result			=	S_OK;
	USB_PM__Interface		*pIDispatch			=	NULL;  
	short					avgCount__Temp		=	0;

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (pIDispatch->lpVtbl->GetAvgCount_ (pIDispatch,&avgCount__Temp));

	if (avgCount)
		{
		*avgCount = avgCount__Temp;
		}

Error:

	return __result;
}

HRESULT CVIFUNC USB_PM_SetAvgCount (void *Handle,ERRORINFO *errorInfo, short avgCount)
{
	HRESULT					__result			=	S_OK;
	USB_PM__Interface		*pIDispatch			=	NULL;  

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (pIDispatch->lpVtbl->SetAvgCount_ (pIDispatch, avgCount));

Error:

	return __result;
}

HRESULT CVIFUNC USB_PM_GetFormat_mW (void *Handle,ERRORINFO *errorInfo, VBOOL *format_mW)
{
	HRESULT					__result			=	S_OK;
	USB_PM__Interface		*pIDispatch			=	NULL;  
	VBOOL					format_mW__Temp		=	0;

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (pIDispatch->lpVtbl->GetFormat_mW_ (pIDispatch,&format_mW__Temp));

	if (format_mW)
		{
		*format_mW = format_mW__Temp;
		}

Error:

	return __result;
}

HRESULT CVIFUNC USB_PM_SetFormat_mW (void *Handle,ERRORINFO *errorInfo, VBOOL format_mW)
{
	HRESULT					__result			=	S_OK;
	USB_PM__Interface		*pIDispatch			=	NULL;  

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (pIDispatch->lpVtbl->SetFormat_mW_ (pIDispatch,format_mW));

Error:

	return __result;
}

HRESULT CVIFUNC USB_PM_Open_AnySensor (void *Handle,ERRORINFO *errorInfo, short *arg1)
{
	HRESULT					__result			=	S_OK;
	USB_PM__Interface		*pIDispatch			=	NULL;  
	short					arg1__Temp			=	0;

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (pIDispatch->lpVtbl->Open_AnySensor_ (pIDispatch,&arg1__Temp));

	if (arg1)
		{
		*arg1 = arg1__Temp;
		}

Error:

	return __result;
}

HRESULT CVIFUNC USB_PM_SetFasterMode (void *Handle,
                                          ERRORINFO *errorInfo, short *s_A)
{
	HRESULT					__result			=	S_OK;
	USB_PM__Interface		*pIDispatch			=	NULL;  

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (pIDispatch->lpVtbl->SetFasterMode_ (pIDispatch, s_A));

Error:

	return __result;
}

HRESULT CVIFUNC USB_PM_ReadPower_AsLinux (void *Handle,ERRORINFO *errorInfo, float *arg1)
{
	HRESULT					__result			=	S_OK;
	USB_PM__Interface		*pIDispatch			=	NULL;  
	float					arg1__Temp			=	0;

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (pIDispatch->lpVtbl->ReadPower_AsLinux_ (pIDispatch,&arg1__Temp));

	if (arg1)
		{
		*arg1 = arg1__Temp;
		}

Error:

	return __result;
}

HRESULT CVIFUNC USB_PM_GetFirmwareVer (void *Handle,
                                           ERRORINFO *errorInfo,
                                           short *firmwareVer, long *arg2)
{
	HRESULT					__result			=	S_OK;
	USB_PM__Interface		*pIDispatch			=	NULL;  
	long					arg2__Temp			=	0;

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (pIDispatch->lpVtbl->GetFirmwareVer_ (pIDispatch,firmwareVer,&arg2__Temp));

	if (arg2)
		{
		*arg2 = arg2__Temp;
		}

Error:

	return __result;
}

HRESULT CVIFUNC USB_PM_CloseConnection (void *Handle,ERRORINFO *errorInfo)
{
	HRESULT					__result			=	S_OK;
	USB_PM__Interface		*pIDispatch			=	NULL;  

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (pIDispatch->lpVtbl->CloseConnection_ (pIDispatch));

Error:

	return __result;
}

HRESULT CVIFUNC USB_PM_GetDeviceTemperature (void *Handle,ERRORINFO *errorInfo,const char *temperatureFormat,
                                                 float *arg2)
{
	HRESULT					__result						=	S_OK;
	USB_PM__Interface		*pIDispatch						=	NULL;  
	BSTR					temperatureFormat__AutoType 	=	0;
	float					arg2__Temp						=	0;

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (CA_CStringToBSTR (temperatureFormat,
	                              &temperatureFormat__AutoType));

	__caErrChk (pIDispatch->lpVtbl->GetDeviceTemperature_ (pIDispatch,
	                                                           temperatureFormat__AutoType,
	                                                           &arg2__Temp));

	if (arg2)
		{
		*arg2 = arg2__Temp;
		}

Error:
	CA_FreeBSTR (temperatureFormat__AutoType);

	return __result;
}

HRESULT CVIFUNC USB_PM_GetSensorSN (void *Handle,
                                        ERRORINFO *errorInfo, char **arg1)
{
	HRESULT					__result			=	S_OK;
	USB_PM__Interface		*pIDispatch			=	NULL;  
	BSTR					arg1__AutoType		=	0;

	if (arg1)
		*arg1 = 0;

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (pIDispatch->lpVtbl->GetSensorSN_ (pIDispatch,
	                                                  &arg1__AutoType));

	if (arg1)
		__caErrChk (CA_BSTRGetCString (arg1__AutoType, arg1));

Error:
	CA_FreeBSTR (arg1__AutoType);

	return __result;
}

HRESULT CVIFUNC USB_PM_GetSensorModelName (void *Handle,ERRORINFO *errorInfo, char **arg1)
{
	HRESULT					__result			=	S_OK;
	USB_PM__Interface		*pIDispatch			=	NULL;  
	BSTR					arg1__AutoType		=	0;

	if (arg1)
		*arg1 = 0;

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (pIDispatch->lpVtbl->GetSensorModelName_ (pIDispatch,&arg1__AutoType));

	if (arg1)
		__caErrChk (CA_BSTRGetCString (arg1__AutoType, arg1));

Error:
	CA_FreeBSTR (arg1__AutoType);

	if (FAILED(__result))
		{
		if (arg1)
			{
			CA_FreeMemory (*arg1);
			*arg1 = 0;
			}
		}

	return __result;
}

HRESULT CVIFUNC USB_PM_Init_PM (void *Handle,ERRORINFO *errorInfo)
{
	HRESULT					__result			=	S_OK;
	USB_PM__Interface		*pIDispatch			=	NULL;  

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (pIDispatch->lpVtbl->Init_PM_ (pIDispatch));

Error:

	return __result;
}

HRESULT CVIFUNC USB_PM_Close_Sensor (void *Handle,ERRORINFO *errorInfo)
{
	HRESULT					__result			=	S_OK;
	USB_PM__Interface		*pIDispatch			=	NULL;  

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (pIDispatch->lpVtbl->Close_Sensor_ (pIDispatch));

Error:

	return __result;
}

HRESULT CVIFUNC USB_PM_Get_Available_SN_List (void *Handle,ERRORINFO *errorInfo,char **SN_List, short *arg2)
{
	HRESULT					__result			=	S_OK;
	USB_PM__Interface		*pIDispatch			=	NULL;  
	BSTR					SN_List__AutoType	=	0;
	short					arg2__Temp			=	0;

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (CA_CStringToBSTR (*SN_List, &SN_List__AutoType));

	__caErrChk (pIDispatch->lpVtbl->Get_Available_SN_List_ (pIDispatch,
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

	if (FAILED(__result))
		{
		if (SN_List)
			{
			CA_FreeMemory (*SN_List);
			*SN_List = 0;
			}
		}

	return __result;
}

HRESULT CVIFUNC USB_PM_GetStatus (void *Handle,ERRORINFO *errorInfo, short *arg1)
{
	HRESULT					__result			=	S_OK;
	USB_PM__Interface		*pIDispatch			=	NULL;  
	short					arg1__Temp			=	0;

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (pIDispatch->lpVtbl->GetStatus_ (pIDispatch,&arg1__Temp));

	if (arg1)
		{
		*arg1 = arg1__Temp;
		}

Error:

	return __result;
}

HRESULT CVIFUNC USB_PM_Open_Sensor (void *Handle,ERRORINFO *errorInfo,const char *SN_Request, short *arg2)
{
	HRESULT					__result				=	S_OK;
	USB_PM__Interface		*pIDispatch				=	NULL;  
	
	BSTR					SN_Request__AutoType	=	0;
	short					arg2__Temp				=	0;

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (CA_CStringToBSTR (SN_Request, &SN_Request__AutoType));

	__caErrChk (pIDispatch->lpVtbl->Open_Sensor_ (pIDispatch,SN_Request__AutoType,&arg2__Temp));

	if (arg2)
		{
		*arg2 = arg2__Temp;
		}

Error:
	CA_FreeBSTR (SN_Request__AutoType);

	return __result;
}

HRESULT CVIFUNC USB_PM_ReadPower (void *Handle,ERRORINFO *errorInfo, float *arg1)
{
	HRESULT					__result			=	S_OK;
	USB_PM__Interface		*pIDispatch			=	NULL;  
	float					arg1__Temp			=	0;

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (pIDispatch->lpVtbl->ReadPower_ (pIDispatch,&arg1__Temp));

	if (arg1)
		{
		*arg1 = arg1__Temp;
		}

Error:

	return __result;
}

HRESULT CVIFUNC USB_PM_ReadImmediatePower (void *Handle,ERRORINFO *errorInfo, float *arg1)
{
	HRESULT					__result			=	S_OK;
	USB_PM__Interface		*pIDispatch			=	NULL;  
	float					arg1__Temp			=	0;

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (pIDispatch->lpVtbl->ReadImmediatePower_ (pIDispatch,&arg1__Temp));

	if (arg1)
		{
		*arg1 = arg1__Temp;
		}

Error:

	return __result;
}

HRESULT CVIFUNC USB_PM_ReadVoltage (void *Handle,ERRORINFO *errorInfo, float *arg1)
{
	HRESULT					__result			=	S_OK;
	USB_PM__Interface		*pIDispatch			=	NULL;  
	float					arg1__Temp			=	0;

	pIDispatch = (USB_PM__Interface*) Handle;
	
	__caErrChk (pIDispatch->lpVtbl->ReadVoltage_ (pIDispatch,&arg1__Temp));

	if (arg1)
		{
		*arg1 = arg1__Temp;
		}

Error:

	return __result;
}
