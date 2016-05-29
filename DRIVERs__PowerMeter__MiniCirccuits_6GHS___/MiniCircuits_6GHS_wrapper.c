//==============================================================================
// Include files

#include <windows.h>
#include "MiniCircuits_6GHS.h"
#include <ansi_c.h>
#include <cvirte.h>
#include <userint.h>
#include <visa.h>
#include <utility.h>
#include "GlobalDefines.h"
#include "MiniCircuits_6GHS_wrapper.h"

//==============================================================================
// Constants

#define			MODULE_NAME			"MiniCircuits_6GHS_wrapper"

//==============================================================================
// Types

typedef struct
{
	void		*DriverHandle;
	
} tsHandleItem;
//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions
void* DLLEXPORT PowerMeter_Close( int *phInstrumentHandle );


void* DLLEXPORT PowerMeter_GetLastErrorTextMessage( int handle , int iError , char **pErrorMessage )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
//	CA_GetAutomationErrorString (iError, pErrorMessage, sizeof(pErrorMessage));
	
//Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void*	DLLEXPORT		Equipment_Info ( int hLowLevelHandle , char *pAddress , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware )
{
	STD_ERROR						StdError									=	{0};
	
	int								iCount										=	0, 
									status										=	0, 
									iIndex										=	0, 
									bSupport									=	0, 
									defaultRM									=	0,
									hConnectionHandle							=	0;
	
	char							szIdentificationText[LOW_STRING]			=	{0},
									*pTemp										=	NULL;

	if ( hLowLevelHandle == 0 )
	{
		viOpenDefaultRM (&defaultRM);

		SetBreakOnLibraryErrors (0);
	
		status = viOpen ( defaultRM , pAddress , NULL, NULL, &hConnectionHandle );
	}
	else
	{
		hConnectionHandle = hLowLevelHandle;	
	}
		
	if ( status == 0 )
	{	
		status = viPrintf ( hConnectionHandle , "*IDN?\n" );
	
		if ( status == 0 )
		{
			viRead ( hConnectionHandle , szIdentificationText , (LOW_STRING-1) , &iCount );
			RemoveSurroundingWhiteSpace (szIdentificationText); 
		}
	}
		
	if (( hLowLevelHandle == 0 ) && hConnectionHandle )
		viClose(hConnectionHandle); 
		
	SetBreakOnLibraryErrors (1);

	do
	{
		IF (( strstr( szIdentificationText , "Agilent Technologies" ) == NULL ) , "Is not supported" );
	
		pTemp = strrchr( szIdentificationText , ',' );

		if ( pTemp )
		{
			*pTemp = 0;
			pTemp++;
	
			if ( pFirmware )
			{
				CALLOC_COPY_STRING( *pFirmware , pTemp );
			}
		}
		else
			break;
	
		pTemp = strrchr( szIdentificationText , ',' );

		if ( pTemp )
		{
			*pTemp = 0;
			pTemp++;
	
			if ( pSerialNumber )
			{
				CALLOC_COPY_STRING( *pSerialNumber , pTemp );
			}
		}
		else
			break;
	
		pTemp = strrchr( szIdentificationText , ',' );

		if ( pTemp )
		{
			*pTemp = 0;
			pTemp++;
	
			if ( pModel )
			{
				CALLOC_COPY_STRING( *pModel , pTemp );
			}
		}
		else
			break;
	
		if ( pVendor )
		{
			CALLOC_COPY_STRING( *pVendor , szIdentificationText );  
		}
		
	} while(0);
		
Error:
		
	RETURN_STDERR_POINTER;
}

void*	DLLEXPORT		Equipment_GetLowLevelHandle( int hHandle , int *phLowLevelHandle )
{
	STD_ERROR						StdError									=	{0};
	
	if ( phLowLevelHandle )
		*phLowLevelHandle = hHandle;
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		Equipment_IsSupported ( int hLowLevelHandle , char *pAddress , char *pID_String , char *pIdentificationText , int *pbSupporting , void **pLocalData )
{
	STD_ERROR						StdError									=	{0};
	
	int								iCount										=	0, 
									status										=	0, 
									bSupport									=	0, 
									defaultRM									=	0,
									hConnectionHandle							=	0;
	
	char							szIdentificationText[LOW_STRING]			=	{0};
	
	if (( pIdentificationText == NULL ) || ( strlen(pIdentificationText) < 10 ))
	{
		SetBreakOnLibraryErrors (0);   
		
		if ( hLowLevelHandle == 0 )
		{
			viOpenDefaultRM (&defaultRM);

			SetBreakOnLibraryErrors (0);
	
			status = viOpen ( defaultRM , pAddress , NULL, NULL, &hConnectionHandle );
		}
		else
		{
			hConnectionHandle = hLowLevelHandle;	
		}
		
		if ( status == 0 )
		{	
			status = viPrintf ( hConnectionHandle , "*IDN?\n" );
		
			if ( status == 0 )
			{
				viRead ( hConnectionHandle , szIdentificationText , (LOW_STRING-1) , &iCount );
				RemoveSurroundingWhiteSpace (szIdentificationText); 
			}
		}
		
		if (( hLowLevelHandle == 0 ) && hConnectionHandle )
			viClose(hConnectionHandle); 
		
		SetBreakOnLibraryErrors (1);  	
	}
	else
	{
		if ( strlen(pIdentificationText) < LOW_STRING )
			strcpy( szIdentificationText , pIdentificationText );
		else
			memcpy( szIdentificationText , pIdentificationText , (LOW_STRING-1) );  
	}
		
	do
	{
		if (( strstr( szIdentificationText , "Agilent Technologies" ) == NULL ) && ( strstr( szIdentificationText ,"Hewlett-Packard") == NULL ))
			break;
	
		if ( strstr( szIdentificationText , "8719d" ))
		{
			bSupport = 1;
			break;
		}
					
	}while(0);
	
	if (pbSupporting)
		*pbSupporting = bSupport;
	
	RETURN_STDERR_POINTER;
}

void*	DLLEXPORT		Config_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet )
{
	STD_ERROR						StdError									=	{0};
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT PowerMeter_Init( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *pInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChannelsQuantity )
{			
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	//ERRORINFO				errorInfo			=	{0};
	
	int						hPMObject			=	0;	
	
	tsHandleItem			*pHandle			=	NULL;
	
	void					*pTempHandle		=	NULL;
	
	char					*pDevName			=	NULL;
	
	int						bGetLock			=	0;
	
	*InstrumentChannelsQuantity = 0;
	*InstrumentConnectStatus = 0;
	
	if ( hParentInstrumentHandle == 0 )
	{
		STD_ERR ( USB_PM_LOAD_DRIVER (NULL, 1, LOCALE_NEUTRAL, 0, &pTempHandle )); 
		
		CHK_CMT( CmtNewTSV (sizeof(tsHandleItem) , &hPMObject)); 
		
		CHK_CMT( CmtGetTSVPtr ( hPMObject , &pHandle ));
		bGetLock=1;
		
		pHandle->DriverHandle = pTempHandle;
		//STD_ERR ( Delay_LastComplete(0));
		
		STD_ERR ( USB_PM_Init_PM ( pHandle->DriverHandle , NULL));
		STD_ERR ( Delay_LastComplete(0));
		
		CHK_CMT( CmtReleaseTSVPtr ( hPMObject ));
		bGetLock=0;
	}
	else
		hPMObject = hParentInstrumentHandle;
	
	CHK_CMT( CmtGetTSVPtr ( hPMObject , &pHandle ));
	bGetLock=1;
	
	STD_ERR ( USB_PM_GetSensorModelName ( pHandle->DriverHandle , NULL, &pDevName));  
	STD_ERR ( Delay_LastComplete(0)); 
	
	CHK_CMT( CmtReleaseTSVPtr ( hPMObject ));
	bGetLock=0;
		
	if (strcmp (pDevName , SUPPORTED_DEVICE_NAME) == 0)
	{
		if ( pInstrumentHandle  ) 
			*pInstrumentHandle = hPMObject;
	
		if ( InstrumentChannelsQuantity )
			*InstrumentChannelsQuantity = 1; 
	
		if ( InstrumentConnectStatus )
			*InstrumentConnectStatus = 1;
	}
	else 
		if (hPMObject)	
			PowerMeter_Close( &hPMObject );

Error:
	
	if ( hPMObject && bGetLock )
		CmtReleaseTSVPtr ( hPMObject ); 
		
	if (pDevName)
		CA_FreeMemory(pDevName);
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}


void* DLLEXPORT PowerMeter_Close( int *phInstrumentHandle )
{	
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	int						hPMObject			=	0;	
	
	tsHandleItem			*pHandle			=	NULL;

	int						bGetLock			=	0;
	
	IF ( (phInstrumentHandle == NULL) , "Pointer passed NULL." , MODULE_NAME);
	
	hPMObject = *phInstrumentHandle;
	
	CHK_CMT( CmtGetTSVPtr ( hPMObject , &pHandle )); 
	bGetLock = 1;
	
	USB_PM_CloseConnection ( pHandle->DriverHandle , NULL );
	
	USB_PM_Close_Sensor ( pHandle->DriverHandle, NULL );
	
	*phInstrumentHandle=0;
	
Error:
	
	if ( hPMObject )
	{
		if ( bGetLock )
			CmtReleaseTSVPtr ( hPMObject ); 	
	
		CmtDiscardTSV ( hPMObject );
	}
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_Reset( int hInstrumentHandle )
{	
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	tsHandleItem			*pHandle			=	NULL;
	
	int						bGetLock			=	0; 
	
	CHK_CMT( CmtGetTSVPtr ( hInstrumentHandle , &pHandle )); 
	bGetLock = 1;
	
	STD_ERR ( USB_PM_Close_Sensor (pHandle->DriverHandle, NULL));
	STD_ERR ( Delay_LastComplete( 0 ));
	
	STD_ERR ( USB_PM_Open_Sensor (pHandle->DriverHandle, NULL, "", NULL));
	STD_ERR ( Delay_LastComplete( 0 ));
	
Error:
	
	if ( hInstrumentHandle && bGetLock )
		CmtReleaseTSVPtr ( hInstrumentHandle ); 
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_SetSingleSweep( int hInstrumentHandle , int channel )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
//Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_SetContinuesSweep( int hInstrumentHandle , int channel )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
//Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_InitSweep( int hInstrumentHandle , int channel ) 
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
//Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_SetAuto( int hInstrumentHandle , int channel )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
//Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_SetChanelState( int hInstrumentHandle , int channel , int state )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	tsHandleItem			*pHandle			=	NULL;
	
	int						bGetLock			=	0; 
	
	CHK_CMT( CmtGetTSVPtr ( hInstrumentHandle , &pHandle )); 
	bGetLock = 1;
	
	if (state)
		{STD_ERR ( USB_PM_Open_Sensor (pHandle->DriverHandle, NULL, "", NULL));}
	else
		{STD_ERR ( USB_PM_Close_Sensor (pHandle->DriverHandle, NULL));}
		
	STD_ERR ( Delay_LastComplete( 0 ));
		
Error:
	
	if ( hInstrumentHandle && bGetLock )
		CmtReleaseTSVPtr ( hInstrumentHandle ); 	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_SetVerticalScale( int hInstrumentHandle , int channel , double scale )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
//Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_SetVerticalCenter( int hInstrumentHandle , int channel , double center )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
//Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_Calibrate( int hInstrumentHandle , int channel )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
//Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_Zero( int hInstrumentHandle , int channel )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
//Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_SetUnits( int hInstrumentHandle , int channel , unsigned int units )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	tsHandleItem			*pHandle			=	NULL;
	
	int						bGetLock			=	0; 
	
	CHK_CMT( CmtGetTSVPtr ( hInstrumentHandle , &pHandle )); 
	bGetLock = 1;
	
	IF ((((int)units < 0) || (units >= PM_UNIT_SENTINEL)) , "Bad parameneter." , MODULE_NAME);
	
	switch (units)
	{
		case PM_UNIT_WATT:
			STD_ERR ( USB_PM_SetFormat_mW (pHandle->DriverHandle, NULL, VFALSE));
			break;
		case PM_UNIT_MILI_WATT:
			STD_ERR ( USB_PM_SetFormat_mW (pHandle->DriverHandle, NULL, VTRUE));
			break;
	}
	
Error:
	
	if ( hInstrumentHandle && bGetLock )
		CmtReleaseTSVPtr ( hInstrumentHandle ); 	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

void* DLLEXPORT PowerMeter_SetOffset( int hInstrumentHandle , int channel , double Offset )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
//Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_SetCalFactor( int hInstrumentHandle , int channel , double CalFactor )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
//Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_SetVideoBW( int hInstrumentHandle , int channel ,  unsigned int VideoBW )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
//Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_SetPeakHold( int hInstrumentHandle , int channel , int state )
{
	return 0;
}

void* DLLEXPORT PowerMeter_SetFrequency( int hInstrumentHandle , int iChannel , double dFreq )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	double					dFreq_MHz			=	0.0;
	
	tsHandleItem			*pHandle			=	NULL;
	
	int						bGetLock			=	0; 
	
	CHK_CMT( CmtGetTSVPtr ( hInstrumentHandle , &pHandle )); 
	bGetLock = 1;
	
	dFreq_MHz = dFreq/1E6;
    
	STD_ERR ( USB_PM_SetFreq (pHandle->DriverHandle, NULL, dFreq_MHz));
	STD_ERR ( Delay_LastComplete( 0 ));  
    
Error:
	
	if ( hInstrumentHandle && bGetLock )
		CmtReleaseTSVPtr ( hInstrumentHandle ); 
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_SetMode( int hInstrumentHandle , unsigned int Mode )
{
	return 0;
}

void* DLLEXPORT PowerMeter_OPC( int hInstrumentHandle , double timeout , int *state )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
//Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_FetchCWpower( int hInstrumentHandle , int channel , double timeout , double *value )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	float 					fPower				=	0;
	
	tsHandleItem			*pHandle			=	NULL;
	
	int						bGetLock			=	0; 
	
	CHK_CMT( CmtGetTSVPtr ( hInstrumentHandle , &pHandle )); 
	bGetLock = 1;
	
   	Delay( timeout ); 
   
	STD_ERR ( USB_PM_ReadPower (pHandle->DriverHandle, NULL, &fPower)); 
	STD_ERR ( Delay_LastComplete( 0 ));
	
	*value = (double)fPower;
		
Error:
	
	if ( hInstrumentHandle && bGetLock )
		CmtReleaseTSVPtr ( hInstrumentHandle ); 
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_MeasureModulatedPower( int hInstrumentHandle , int iChannel , double timeout , double *value )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	float 					fPower				=	0;
	
	tsHandleItem			*pHandle			=	NULL;
	
	int						bGetLock			=	0; 
	
	CHK_CMT( CmtGetTSVPtr ( hInstrumentHandle , &pHandle )); 
	bGetLock = 1;
	
   	Delay( timeout ); 
   						   
	STD_ERR ( USB_PM_ReadImmediatePower (pHandle->DriverHandle, NULL, &fPower)); 
	STD_ERR ( Delay_LastComplete( 0 ));
	
	*value = (double)fPower;
		
Error:
	
	if ( hInstrumentHandle && bGetLock )
		CmtReleaseTSVPtr ( hInstrumentHandle ); 
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);     
}

void* DLLEXPORT PowerMeter_FetchMarkerAverage( int hInstrumentHandle , int channel , int marker , double timeout , double *value )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
//Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_FetchMarkerMax( int hInstrumentHandle , int channel , int marker , double timeout , double *value )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
//Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

void* DLLEXPORT PowerMeter_FetchMarkerMin( int hInstrumentHandle , int channel , int marker , double timeout , double *value )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
//Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

int Delay_LastComplete( int hInstrumentHandle )
{
	Delay (GLOBAL_DELAY);
	
	return 0;
}

void main ( void )
{
	int					pInstrumentHandle			=	0,
						InstrumentConnectStatus		=	0,
						InstrumentChannelsQuantity	=	0;
		
	double				value						=	0.0;
		
	PowerMeter_Init( 0 , "spart" , &pInstrumentHandle , &InstrumentConnectStatus , &InstrumentChannelsQuantity );   
	
	PowerMeter_SetFrequency( pInstrumentHandle , 0 , 10E6 );   
	
	PowerMeter_FetchCWpower( pInstrumentHandle , 0 , 0 , &value );  
	
	PowerMeter_Close( &pInstrumentHandle );
		 
	PowerMeter_Init( 0 , "spart" , &pInstrumentHandle , &InstrumentConnectStatus , &InstrumentChannelsQuantity );   
	
	PowerMeter_SetFrequency( pInstrumentHandle , 0 , 100E6 );   
	
	PowerMeter_FetchCWpower( pInstrumentHandle , 0 , 0 , &value );  
	
	PowerMeter_Close( &pInstrumentHandle );
	
	return;
}
