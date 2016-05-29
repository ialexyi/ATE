//==============================================================================
// Include files

#include <windows.h>
#include <userint.h>
#include <ansi_c.h>
#include <utility.h>
#include <visa.h>
#include "LambdaZup_Support.h"
#include "LambdaZup.h"

//==============================================================================
// Constants

#define			MODULE_NAME		"LambdaZup wrapper"

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

/***** ***** ***** ***** ***** ***** Get Error Text Message ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetErrorTextMessage ( int iInstrumentHandle , int iError , char *pErrorMessage )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	CHK_RET ( ZUP_GetErrorTextMessage	( iInstrumentHandle , iError , pErrorMessage ));
	
Error:		
		
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

/***** ***** ***** ***** ***** ***** ***** ***** Init ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_Init ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *phInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChanelsQuantity )
{			
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	int			iZupChanelsQuantity			=	0;
	
	int			handle						=	0;
	
	IF (((phInstrumentHandle == NULL) || (InstrumentConnectStatus == NULL) || (InstrumentChanelsQuantity == NULL)), "Error passed NULL pointer." , MODULE_NAME);
	
	if ( hParentInstrumentHandle == 0 )
	{
		CHK_RET ( ZUP_Init ( szRsrcAddress , &handle , &iZupChanelsQuantity));       
	}
	else
	{
		handle = hParentInstrumentHandle;
		
		GetZupChannelsQuantity ( handle , &iZupChanelsQuantity );
	}
	
	if (iZupChanelsQuantity < 1)
		*InstrumentConnectStatus = 0;
	
	else
	{
		*phInstrumentHandle = handle;
		*InstrumentConnectStatus = 1;
		*InstrumentChanelsQuantity = iZupChanelsQuantity;
	}
	
Error:
		
	if (ret.error)
		ZUP_Close ( &handle );
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

/***** ***** ***** ***** ***** ***** ***** ***** Close ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_Close ( int *phInstrumentHandle )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL; 
	
	IF ( (phInstrumentHandle == NULL), "Handle can't be NULL." , MODULE_NAME );
	
	CHK_RET ( ZUP_Close ( phInstrumentHandle ));
	
	*phInstrumentHandle = 0;
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

/***** ***** ***** ***** ***** ***** ***** ***** Reset ***** ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_Reset ( int iInstrumentHandle ) 
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}

/***** ***** ***** ***** ***** ***** ***** ***** Get Model ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetModel ( int iInstrumentHandle , int channel , char *pReturnString )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	CHK_RET ( ZUP_GetPowerSupplyModel ( iInstrumentHandle , channel , pReturnString ));
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** ***** Get Revision ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetRevision ( int iInstrumentHandle , int channel , char *pReturnString )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	CHK_RET ( ZUP_GetPowerSupplyRevision ( iInstrumentHandle , channel , pReturnString ));
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** Set Output Voltage ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_SetOutputVoltage ( int iInstrumentHandle , int channel , double lfVoltage )
{
	errEV					ret						=	{0};
	errEV					*pRet					=	NULL;
	
	double					lfVoltageDefined		=	0.0,
							lfVoltageDelta			=	0.0,
							lfOffsetVoltage			=	0.0;
	
	CHK_RET ( ZUP_SetOutputVoltage ( iInstrumentHandle , channel , lfVoltage ));
	
	CHK_RET ( ZUP_GetOutputVoltageDefined ( iInstrumentHandle , channel , &lfVoltageDefined ));  
	
	lfVoltageDelta = lfVoltage - lfVoltageDefined;
		
	if ( fabs( lfVoltageDelta ) < 0.5 )
	{
		lfOffsetVoltage = lfVoltage + lfVoltageDelta;
			
		CHK_RET ( ZUP_SetOutputVoltage ( iInstrumentHandle , channel , lfOffsetVoltage ));
	
		CHK_RET ( ZUP_GetOutputVoltageDefined ( iInstrumentHandle , channel , &lfVoltageDefined ));
		
		lfVoltageDelta = lfVoltage - lfVoltageDefined; 
		
		IF ((( fabs( lfVoltageDelta ) > 0.05 ) && ( fabs( lfVoltageDelta ) < 0.5 )) , "Can't set need value" , MODULE_NAME );
	}
	
Error:
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** Get Output Voltage Defined ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetOutputVoltageDefined ( int iInstrumentHandle , int channel , double *plfVoltage )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	CHK_RET ( ZUP_GetOutputVoltageDefined ( iInstrumentHandle , channel , plfVoltage ));
	
Error:
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** Get Output Voltage Actual ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetOutputVoltageActual ( int iInstrumentHandle , int channel , double *plfVoltage )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	CHK_RET ( ZUP_GetOutputVoltageActual ( iInstrumentHandle , channel , plfVoltage ));
	
Error:
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** Set Output Current ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_SetOutputCurrent ( int iInstrumentHandle , int channel , double lfCurrent )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	double					lfCurrentDefined		=	0.0,
							lfCurrentDelta			=	0.0,
							lfOffsetCurrent			=	0.0;
	
	CHK_RET ( ZUP_SetOutputCurrent ( iInstrumentHandle , channel , lfCurrent ));
	
	CHK_RET ( ZUP_GetOutputCurrentDefined ( iInstrumentHandle , channel , &lfCurrentDefined ));  
	
	lfCurrentDelta = lfCurrent - lfCurrentDefined;
		
	if ( fabs( lfCurrentDelta ) < 0.5 )
	{
		lfOffsetCurrent = lfCurrent + lfCurrentDelta;
			
		CHK_RET ( ZUP_SetOutputCurrent ( iInstrumentHandle , channel , lfOffsetCurrent ));
	
		CHK_RET ( ZUP_GetOutputCurrentDefined ( iInstrumentHandle , channel , &lfCurrentDefined ));
		
		lfCurrentDelta = lfCurrent - lfCurrentDefined;
		
		IF ((( fabs( lfCurrentDelta ) > 0.05 ) && ( fabs( lfCurrentDelta ) < 0.5 )) , "Can't set need value" , MODULE_NAME );
	}
	
	
Error:
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** Get Output Current Defined ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetOutputCurrentDefined ( int iInstrumentHandle , int channel , double *plfCurrent )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	CHK_RET ( ZUP_GetOutputCurrentDefined ( iInstrumentHandle , channel , plfCurrent ));
	
Error:
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** Get Output Current Actual ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetOutputCurrentActual ( int iInstrumentHandle , int channel , double *plfCurrent )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	CHK_RET ( ZUP_GetOutputCurrentActual ( iInstrumentHandle , channel , plfCurrent ));
	
Error:
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** Get Output Power Actual ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetOutputPowerActual ( int iInstrumentHandle , int channel , double *plfPower )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	double		lfCurrent		=	0.0,
				lfVoltage		=	0.0;
	
	CHK_RET ( ZUP_GetOutputCurrentActual ( iInstrumentHandle , channel , &lfCurrent ));

	CHK_RET ( ZUP_GetOutputVoltageActual ( iInstrumentHandle , channel , &lfVoltage ));
	
	if ( plfPower )
		*plfPower = lfCurrent * lfVoltage;
	
Error:	
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);
}
/***** ***** ***** ***** ***** ***** ***** Set Voltage Protection Over ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_SetVoltageProtectionOver ( int iInstrumentHandle , int channel , double lfVoltage )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	double					lfVoltageDefined		=	0.0,
							lfVoltageDelta			=	0.0,
							lfOffsetVoltage			=	0.0;
	
	CHK_RET ( ZUP_SetVoltageProtectionOver ( iInstrumentHandle , channel , lfVoltage ));
	
	CHK_RET ( ZUP_GetVoltageProtectionOver ( iInstrumentHandle , channel , &lfVoltageDefined ));  
	
	lfVoltageDelta = lfVoltage - lfVoltageDefined;
		
	if ( fabs( lfVoltageDelta ) < 0.5 )
	{
		lfOffsetVoltage = lfVoltage + lfVoltageDelta;
			
		CHK_RET ( ZUP_SetVoltageProtectionOver ( iInstrumentHandle , channel , lfOffsetVoltage ));
	
		CHK_RET ( ZUP_GetVoltageProtectionOver ( iInstrumentHandle , channel , &lfVoltageDefined ));
		
		lfVoltageDelta = lfVoltage - lfVoltageDefined; 
		
		IF ((( fabs( lfVoltageDelta ) > 0.05 ) && ( fabs( lfVoltageDelta ) < 0.5 )) , "Can't set need value" , MODULE_NAME );
	}
	
Error:
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** Get Voltage Protection Over ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetVoltageProtectionOver ( int iInstrumentHandle , int channel , double *plfVoltage )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	CHK_RET ( ZUP_GetVoltageProtectionOver ( iInstrumentHandle , channel , plfVoltage ));
	
Error:
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** Set Voltage Protection Under ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_SetVoltageProtectionUnder ( int iInstrumentHandle , int channel , double lfVoltage )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	double					lfVoltageDefined		=	0.0,
							lfVoltageDelta			=	0.0,
							lfOffsetVoltage			=	0.0;
	
	CHK_RET ( ZUP_SetVoltageProtectionUnder ( iInstrumentHandle , channel , lfVoltage ));
	
	CHK_RET ( ZUP_GetVoltageProtectionUnder ( iInstrumentHandle , channel , &lfVoltageDefined ));  
	
	lfVoltageDelta = lfVoltage - lfVoltageDefined;
		
	if ( fabs( lfVoltageDelta ) < 0.5 )
	{
		lfOffsetVoltage = lfVoltage + lfVoltageDelta;
			
		CHK_RET ( ZUP_SetVoltageProtectionUnder ( iInstrumentHandle , channel , lfOffsetVoltage ));
	
		CHK_RET ( ZUP_GetVoltageProtectionUnder ( iInstrumentHandle , channel , &lfVoltageDefined ));
		
		lfVoltageDelta = lfVoltage - lfVoltageDefined; 
		
		IF ((( fabs( lfVoltageDelta ) > 0.05 ) && ( fabs( lfVoltageDelta ) < 0.5 )) , "Can't set need value" , MODULE_NAME );
	}
	
Error:
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** Get Voltage Protection Under ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetVoltageProtectionUnder ( int iInstrumentHandle , int channel , double *plfVoltage )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	CHK_RET ( ZUP_GetVoltageProtectionUnder ( iInstrumentHandle , channel , plfVoltage ));
	
Error:
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** ***** Set Output State ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_SetOutputState ( int iInstrumentHandle , int channel , int iState )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	int						iGetState			=	0;
	
	CHK_RET ( ZUP_SetOutputState ( iInstrumentHandle , channel , iState ));
	
	CHK_RET ( ZUP_GetOutputState ( iInstrumentHandle , channel , &iGetState )); 
	
	IF (( iGetState != iState ) , "Can't sat needed state" , MODULE_NAME );
	
Error:
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** ***** Get Output State ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetOutputState ( int iInstrumentHandle , int channel , int *piState )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	CHK_RET ( ZUP_GetOutputState ( iInstrumentHandle , channel , piState ));
	
Error:
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** Set Foldback Protection Mode ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_SetFoldbackProtectionMode ( int iInstrumentHandle , int channel , int iMode )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	CHK_RET ( ZUP_SetFoldbackProtectionMode ( iInstrumentHandle , channel , iMode ));
	
Error:
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** Get Foldback Protection Mode ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetFoldbackProtectionMode ( int iInstrumentHandle , int channel , int *piMode )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	CHK_RET ( ZUP_GetFoldbackProtectionMode ( iInstrumentHandle , channel , piMode ));
	
Error:
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** Set Auto Restart Mode ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_SetAutoRestartMode ( int iInstrumentHandle , int channel , int iMode )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	CHK_RET ( ZUP_SetAutoRestartMode ( iInstrumentHandle , channel , iMode ));
	
Error:
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** ***** Get Auto Restart Mode ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetAutoRestartMode ( int iInstrumentHandle , int channel , int *piMode )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	CHK_RET ( ZUP_GetAutoRestartMode ( iInstrumentHandle , channel , piMode ));
	
Error:
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** Get Operational Status Register ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetOperationalStatusRegister ( int iInstrumentHandle , int channel , char *pReturnString )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	CHK_RET ( ZUP_GetOperationalStatusRegister ( iInstrumentHandle , channel , pReturnString ));
	
Error:
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** Get Alarm Status Register ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetAlarmStatusRegister ( int iInstrumentHandle , int channel , char *pReturnString )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	CHK_RET ( ZUP_GetAlarmStatusRegister ( iInstrumentHandle , channel , pReturnString ));
	
Error:
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** Get Error Codes Register ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetErrorCodesRegister ( int iInstrumentHandle , int channel , char *pReturnString )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	CHK_RET ( ZUP_GetErrorCodesRegister ( iInstrumentHandle , channel , pReturnString ));
	
Error:
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** Get Complete Status ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_GetCompleteStatus ( int iInstrumentHandle , int channel , char *pReturnString )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	CHK_RET ( ZUP_GetCompleteStatus ( iInstrumentHandle , channel , pReturnString ));
	
Error:
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}

/***** ***** ***** ***** ***** ***** Clear Comm Buffer And Registers ***** ***** ***** ***** ***** *****/

void* DLLEXPORT PowerSupply_ClearCommBufferAndRegisters ( int iInstrumentHandle , int channel )
{
	errEV					ret					=	{0};
	errEV					*pRet				=	NULL;
	
	CHK_RET ( ZUP_ClearCommBufferAndRegisters ( iInstrumentHandle , channel ));
	
Error:
	
	CALLOC_COPY(pRet,1,sizeof(errEV),&ret,sizeof(errEV));
	
	return ((void*)pRet);	
}
