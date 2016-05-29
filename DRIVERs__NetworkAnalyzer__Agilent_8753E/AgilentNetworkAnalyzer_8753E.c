//==============================================================================
// Include files
#include "Windows.h"

#include <utility.h>
#include "AgilentNetworkAnalyzer_8753E.h"
#include <visa.h>
#include "GlobalDefines.h"
#include <formatio.h>
#include "toolbox.h"
#include "ag875x.h"
#include <userint.h>
#include <ansi_c.h>

//==============================================================================
// Constants

//==============================================================================
// Types

typedef struct 
{
	int			iCount,
				iType;
	
} tsNumPointsItem;


tsNumPointsItem		gtNumPointsItem	[] = 
{ 
	{03, ag875x_PNTS_03},
	{11, ag875x_PNTS_11},
	{21, ag875x_PNTS_21},
	{26, ag875x_PNTS_26},
	{51, ag875x_PNTS_51},
	{101, ag875x_PNTS_101},
	{201, ag875x_PNTS_201},
	{401, ag875x_PNTS_401},
	{801, ag875x_PNTS_801},
	{1601, ag875x_PNTS_1601},
};

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

//==============================================================================
// DLL main entry-point functions

int __stdcall DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            if (InitCVIRTE (hinstDLL, 0, 0) == 0)
                return 0;     /* out of memory */
            break;
        case DLL_PROCESS_DETACH:
            CloseCVIRTE ();
            break;
    }
    
    return 1;
}

int __stdcall DllEntryPoint (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    /* Included for compatibility with Borland */

    return DllMain (hinstDLL, fdwReason, lpvReserved);
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
									szIdentificationUpperText[LOW_STRING]		=	{0},
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
		strcpy( szIdentificationUpperText , szIdentificationText );
		StringUpperCase (szIdentificationUpperText);
		
		IF ((( strstr( szIdentificationUpperText , "AGILENT TECHNOLOGIES" ) == NULL ) && ( strstr( szIdentificationUpperText ,"HEWLETT-PACKARD") == NULL )) , "Is not supported" );
		
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
									iIndex										=	0, 
									bSupport									=	0, 
									defaultRM									=	0,
									hConnectionHandle							=	0;
	
	char							szIdentificationText[LOW_STRING]			=	{0},
									szIdentificationUpperText[LOW_STRING]		=	{0}; 
	
	char							vszSupportdedModels[][32]					=	{{"8719D"},{"8719ES"},{"8719ET"},{"8720D"},{"8720ES"},{"8720ET"},{"8722D"},{"8722ES"},{"8722ET"},{"8753D"},{"8753E"},{"8753ES"},{"8753ET"}};
									
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
			status = viPrintf ( hConnectionHandle , "*IDN?\n" );   // MODEL?
		
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
		strcpy( szIdentificationUpperText , szIdentificationText );
		StringUpperCase (szIdentificationUpperText);
		
		if (( strstr( szIdentificationUpperText , "AGILENT TECHNOLOGIES" ) == NULL ) && ( strstr( szIdentificationUpperText ,"HEWLETT-PACKARD") == NULL ))
			break;	
	
		for ( iIndex = 0; iIndex < (sizeof(vszSupportdedModels) / sizeof(vszSupportdedModels[0])); iIndex++ )
			if ( strstr( szIdentificationText , vszSupportdedModels[iIndex]) )
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

void* DLLEXPORT   NetworkAnalyzer_Init ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *phInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChanelsQuantity )
{
	STD_ERROR		StdError				=	{0};
	
	int				supported				=	0;
	
	char			szIdentificationText[LOW_STRING]		=	{0};
	
	int				hInstrumentHandle					=	0;
		
	double			lfFirmwareVersion		=	0.0;
	
	if ( szRsrcAddress == NULL )
		STD_ERR( ERROR_CLUSTER_NULL_DATA );
	
	if ( hParentInstrumentHandle == 0 ) 
	{
		CHK_ERR(ag875x_init( szRsrcAddress ,0,0,(ViPSession)&hInstrumentHandle));
	}
	else
		hInstrumentHandle = hParentInstrumentHandle;
		
	if ( phInstrumentHandle )
		*phInstrumentHandle = hInstrumentHandle;
	
	CHK_ERR( ag875x_readID ( hInstrumentHandle , szIdentificationText , ag875x_MODEL_Q, &lfFirmwareVersion ));
	
	supported = 1;
	*InstrumentChanelsQuantity = 2;
	
Error:
	
	if ( InstrumentConnectStatus )
		*InstrumentConnectStatus = supported;
	
	RETURN_STDERR_POINTER;
}


void* DLLEXPORT   NetworkAnalyzer_Close(int *hInstrumentHandle)
{  
	STD_ERROR		StdError				=	{0};

	if (hInstrumentHandle)
	{
		StdError.error = ag875x_close ( *hInstrumentHandle );
		*hInstrumentHandle = 0;
	}
		
//error:
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT NetworkAnalyzer_GetErrorTextMessage ( int hInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR		StdError				=	{0};
	
	ag875x_error_message (hInstrumentHandle , iError , pErrorMessage );
	
Error:

	RETURN_STDERR_POINTER;
}
 
void* DLLEXPORT   NetworkAnalyzer_Reset( int hInstrumentHandle )
{
	STD_ERROR				StdError				=	{0};

	unsigned short			iStatus					=	0;
	
	CHK_ERR(ag875x_reset(hInstrumentHandle));
	
	CHK_ERR(ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus));  
	
Error:
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_SetFrequency( int hInstrumentHandle , double lfStartFrequency ,  double lfStopFrequency )
{
	STD_ERROR				StdError				=	{0};
	
	unsigned short			iStatus					=	0;

	CHK_ERR(ag875x_frequency(hInstrumentHandle, ag875x_FREQ_STRT_STOP, lfStartFrequency , lfStopFrequency , NULL, NULL));

	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_GetFrequency( int hInstrumentHandle , double *plfStartFrequency ,  double *plfStopFrequency )
{
	STD_ERROR				StdError				=	{0};

	CHK_ERR(ag875x_frequency(hInstrumentHandle, ag875x_FREQ_STRT_STOP_Q , 0.0 , 0.0 , plfStartFrequency, plfStopFrequency));

Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_SetPointsNum( int hInstrumentHandle , int points )
{
	STD_ERROR				StdError				=	{0};
	
	int						iIndex					=	0,
							iNumPointsVal			=	0;

	unsigned short			iStatus					=	0;
	
	iNumPointsVal = sizeof(gtNumPointsItem) / sizeof(tsNumPointsItem);
	
	for ( iIndex = 0 ; iIndex < iNumPointsVal; iIndex++)
		if ( gtNumPointsItem[iIndex].iCount > points )
			break;
	
	CHK_ERR(ag875x_points ( hInstrumentHandle, gtNumPointsItem[iIndex].iType , ag875x_SET, ag875x_CH1, VI_NULL));
			
	CHK_ERR(ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus ));  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_GetPointsNum(int hInstrumentHandle, int *points)
{
	STD_ERROR				StdError				=	{0};

	ViInt16 points16;
	CHK_ERR(ag875x_points (hInstrumentHandle, 0, ag875x_Q, ag875x_CH1, &points16));
	*points=points16;
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_GetMeasureVectors(int hInstrumentHandle, int iGetDataType , double **plfFirstVector , double **plfSecondVector , double **plfThirdVector , double **plfFourthVector )
{
	STD_ERROR				StdError				=	{0};

	int 					iIndex					=	0,
							iNumOfPairs				=	0;
		
	ViInt16					iNumOfPoints			=	0;
	
	double					*vlfPairsOfData			=	NULL;

	unsigned short			iStatus					=	0;
	
	if ( iGetDataType >= ag875x_PREn_OUTPUT )
		iNumOfPairs=4;
	else
		iNumOfPairs=2;
		
	CHK_ERR(ag875x_points (hInstrumentHandle, 0, ag875x_Q, ag875x_CH1, &iNumOfPoints)); 
	
	CALLOC_ERR( vlfPairsOfData , (iNumOfPoints*iNumOfPairs) , sizeof(double));
	
	if ( plfFirstVector )
		FREE_CALLOC_ERR( *plfFirstVector , iNumOfPoints , sizeof(double)); 
	
	if ( plfSecondVector )
		FREE_CALLOC_ERR( *plfSecondVector , iNumOfPoints , sizeof(double)); 
	
	if ( iNumOfPairs == 4 )
	{ 
		if ( plfThirdVector ) 
			FREE_CALLOC_ERR( *plfThirdVector , iNumOfPoints , sizeof(double)); 
		
		if ( plfFourthVector ) 
			FREE_CALLOC_ERR( *plfFourthVector , iNumOfPoints , sizeof(double)); 
	}
	
	CHK_ERR(ag875x_trigger(hInstrumentHandle, ag875x_TRIG_HOLD, ag875x_CH1, VI_NULL));
	CHK_ERR(ag875x_timeOut (hInstrumentHandle, 2000));
	CHK_ERR(ag875x_takeSweep(hInstrumentHandle, 1));
	
	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);
		
	CHK_ERR(ag875x_readTraceOrCalDataDos(hInstrumentHandle, iGetDataType , vlfPairsOfData, VI_NULL));

	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
	
	CHK_ERR(ag875x_trigger(hInstrumentHandle, ag875x_TRIG_CONT, ag875x_CH1, VI_NULL));
	
	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
	
	for (iIndex=0; iIndex < iNumOfPoints ; iIndex++)
	{
		(*plfFirstVector)[iIndex] = vlfPairsOfData[iIndex*iNumOfPairs];
		(*plfSecondVector)[iIndex] = vlfPairsOfData[iIndex*iNumOfPairs+1];
		
		if ( iNumOfPairs == 4 )
		{
			(*plfThirdVector)[iIndex] = vlfPairsOfData[iIndex*iNumOfPairs+2]; 
			(*plfFourthVector)[iIndex] = vlfPairsOfData[iIndex*iNumOfPairs+3];
		}
	}
	
Error: 
	
	FREE(vlfPairsOfData); 
	
	RETURN_STDERR_POINTER;
}


void* DLLEXPORT   NetworkAnalyzer_GetTrace ( int hInstrumentHandle , int iChannel , int iMeasurementType , int iMeasurementFormat , double lfTimeout , double **plfFrequency , double **plfTrace , int *piNumberOfPoints )
{
	STD_ERROR				StdError							=	{0};

	int 					iIndex								=	0;
		
	ViInt16					iNumOfPoints						=	0;
	
	double					*vlfPairsOfData						=	NULL;

	unsigned short			iStatus								=	0;

	double					*plfFirstVector						=	NULL;
	
	double					lfStartFrequency					=	0.0, 
							lfStopFrequency						=	0.0, 
							lfCurrentFrequency					=	0.0,
							lfStepFrequency						=	0.0; 
	
	double					averageEnabled						=	0.0,
							currentAverageSmooth				=	0.0;   

	switch(iChannel)
	{
		case 1:
			iChannel = ag875x_CH1;
			break;
			
		case 2:
			iChannel = ag875x_CH2;
			break;
			
		case 3:
			iChannel = ag875x_CH3;
			break;
			
		case 4:
			iChannel = ag875x_CH4;
			break;
			
		default:
			iChannel = ag875x_CH1;
			break;
	}
			
	CHK_ERR(ag875x_channelSelect (hInstrumentHandle, iChannel)); 
	
	CHK_ERR(ag875x_points (hInstrumentHandle, 0, ag875x_Q, ag875x_CH1, &iNumOfPoints)); 
	
	CALLOC_ERR( vlfPairsOfData , (iNumOfPoints*2) , sizeof(double));
	
	FREE_CALLOC_ERR( plfFirstVector , iNumOfPoints , sizeof(double)); 
	
	if ( plfFrequency )
	{
		FREE_CALLOC_ERR( *plfFrequency , iNumOfPoints , sizeof(double)); 
	}

	CHK_ERR(ag875x_trigger(hInstrumentHandle, ag875x_TRIG_HOLD, iChannel, VI_NULL));
	CHK_ERR(ag875x_timeOut (hInstrumentHandle, (lfTimeout*1E3)));
	
	ag875x_averageSmooth (hInstrumentHandle, 0.0, ag875x_AVG_STATUS_Q , iChannel, &averageEnabled);
	ag875x_averageSmooth (hInstrumentHandle, 0.0, ag875x_AVG_FACTOR_Q , iChannel, &currentAverageSmooth);
	
	if ( averageEnabled > 0.0 )
	{
		CHK_ERR(ag875x_takeSweep(hInstrumentHandle, (int)currentAverageSmooth ));
	}
	else
	{
		CHK_ERR(ag875x_takeSweep(hInstrumentHandle, 1 ));
	}
		
	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);
		
	CHK_ERR(ag875x_readTraceOrCalDataDos(hInstrumentHandle, ag875x_FORM_OUTPUT , vlfPairsOfData, VI_NULL));

	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
	
	for (iIndex=0; iIndex < iNumOfPoints ; iIndex++)
		plfFirstVector[iIndex] = vlfPairsOfData[iIndex*2];
	
	if ( plfTrace )
		*plfTrace = plfFirstVector;
	
	if ( piNumberOfPoints )
		*piNumberOfPoints = iNumOfPoints;
	
	if ( plfFrequency )
	{
		CHK_ERR(ag875x_frequency(hInstrumentHandle, ag875x_FREQ_STRT_STOP_Q , 0.0 , 0.0 , &lfStartFrequency, &lfStopFrequency));
		
		lfCurrentFrequency = lfStartFrequency;
		lfStepFrequency = (lfStopFrequency - lfStartFrequency) / (iNumOfPoints - 1);
		
		for (iIndex=0; iIndex < iNumOfPoints ; iIndex++)  
		{
			(*plfFrequency)[iIndex] = lfCurrentFrequency;
			lfCurrentFrequency += lfStepFrequency;
		}
	}
	
Error: 
	
	if ( plfTrace == NULL )
		FREE(plfFirstVector);

	ag875x_trigger(hInstrumentHandle, ag875x_TRIG_CONT, ag875x_CH1, VI_NULL);
	
	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
	
	FREE(vlfPairsOfData); 
	
	RETURN_STDERR_POINTER;
}
	
	

void* DLLEXPORT   NetworkAnalyzer_SetMeasureType(int hInstrumentHandle , int iChannel , int iInSparam, int iConversation)
{			   
	STD_ERROR				StdError				=	{0};

	unsigned short			iStatus					=	0;
	
	CHK_ERR(ag875x_measType (hInstrumentHandle, iInSparam , iConversation ));

	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_DisplaySelect(int hInstrumentHandle, int iChannel, int iDataMode , int iDataFormat )
{			   
	STD_ERROR				StdError				=	{0};
	
	unsigned short			iStatus					=	0;

	switch(iChannel)
	{
		case 1:
			iChannel = ag875x_CH1;
			break;
			
		case 2:
			iChannel = ag875x_CH2;
			break;
			
		case 3:
			iChannel = ag875x_CH3;
			break;
			
		case 4:
			iChannel = ag875x_CH4;
			break;
			
		default:
			iChannel = ag875x_CH1;
			break;
	}
	
	CHK_ERR(ag875x_displaySelect (hInstrumentHandle, iChannel, iDataMode, iDataFormat));

	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
	
Error: 
	
	RETURN_STDERR_POINTER;
}


void* DLLEXPORT   NetworkAnalyzer_SelectChannel ( int hInstrumentHandle , int iChannel )
{
	STD_ERROR				StdError				=	{0};

	unsigned short			iStatus					=	0;

	switch(iChannel)
	{
		case 1:
			iChannel = ag875x_CH1;
			break;
			
		case 2:
			iChannel = ag875x_CH2;
			break;
			
		case 3:
			iChannel = ag875x_CH3;
			break;
			
		case 4:
			iChannel = ag875x_CH4;
			break;
			
		default:
			iChannel = ag875x_CH1;
			break;
	}
			
	CHK_ERR(ag875x_channelSelect (hInstrumentHandle, iChannel)); 
	
	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
	
Error: 
	
	RETURN_STDERR_POINTER;
}	
	
void* DLLEXPORT   NetworkAnalyzer_AutoScale(int hInstrumentHandle )
{			   
	STD_ERROR				StdError				=	{0};

	unsigned short			iStatus					=	0;
	
	CHK_ERR(ag875x_autoscale ( hInstrumentHandle ));

	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_AvarageSmooth(int hInstrumentHandle, int iChannel, char *szSmoothRequest , double lfSetSmooth , double lfCurrentSmooth )
{			   
	STD_ERROR				StdError				=	{0};

	unsigned short			iStatus					=	0;

	CHK_ERR(ag875x_averageSmooth (hInstrumentHandle, lfSetSmooth , szSmoothRequest, iChannel, &lfCurrentSmooth ));
	
	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
		
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_Smooth(int hInstrumentHandle, int iChannel, int iState , double lfSetSmooth , int iNumberPoints )
{		
	STD_ERROR				StdError				=	{0};

	unsigned short			iStatus					=	0;

	double		lfCurrentSmooth;
	//CHK_ERR(ag875x_averageSmooth (hInstrumentHandle, lfSetSmooth , szSmoothRequest, iChannel, &lfCurrentSmooth ));

	CHK_ERR(ag875x_averageSmooth (hInstrumentHandle, lfSetSmooth , ag875x_SMOOTH_APERTURE , iChannel, &lfCurrentSmooth ));
	
	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_SetBandWidth(int hInstrumentHandle, int iChannel , double  lfBW_Value , char *szBW_Request , double  *lfGetBW_Value )
{			   
	STD_ERROR				StdError				=	{0};

	unsigned short			iStatus					=	0;

	CHK_ERR(ag875x_bandwidth (hInstrumentHandle, lfBW_Value, szBW_Request, iChannel , lfGetBW_Value ));

	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_SetFrequencySweepType(int hInstrumentHandle, int iChannel , int iFrequencySweepType )
{			   
	STD_ERROR				StdError				=	{0};

	unsigned short			iStatus					=	0;

	CHK_ERR(ag875x_freqSweepType (hInstrumentHandle, iFrequencySweepType ));

	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_SetPower(int hInstrumentHandle, int iChannel , double lfPower , char *szPowerRequest , int iPortNum )
{			   
	STD_ERROR				StdError				=	{0};

	unsigned short			iStatus					=	0;

	CHK_ERR(ag875x_powerSet (hInstrumentHandle, lfPower , szPowerRequest , iChannel , iPortNum ));

	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_GetPower(int hInstrumentHandle, int iChannel , double *lfPower , int iPortNum )
{			   
	STD_ERROR				StdError				=	{0};

	CHK_ERR( ag875x_powerQuery (hInstrumentHandle, lfPower , ag875x_PWR_LEVEL_Q, iChannel, iPortNum));

Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_SetPowerRange(int hInstrumentHandle, int iChannel , int iRange , char *szRangeRequest , int *iCurrentRange )
{			   
	STD_ERROR				StdError				=	{0};

	unsigned short			iStatus					=	0;

	CHK_ERR(ag875x_powerRange (hInstrumentHandle, iRange , szRangeRequest , iChannel , (short *)iCurrentRange ));

	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_SetPowerSweep(int hInstrumentHandle, int iChannel , double lfStartPower , double lfStopPower , char *szSweepRequest , double *lfGetStartPower , double *lfGetStopPower )
{			   
	STD_ERROR				StdError				=	{0};

	unsigned short			iStatus					=	0;

	CHK_ERR(ag875x_powerSweep (hInstrumentHandle, szSweepRequest , lfStartPower , lfStopPower , iChannel , lfGetStartPower , lfGetStopPower ));

	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_SetSweepTime(int hInstrumentHandle, int iChannel , double lfSweepTime , char *szSweepTimeRequest , double *lfGetSweepTime )
{			   
	STD_ERROR				StdError				=	{0};

	unsigned short			iStatus					=	0;

	CHK_ERR(ag875x_sweepTime (hInstrumentHandle, lfSweepTime , szSweepTimeRequest , iChannel , lfGetSweepTime ));

	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_SetMarker(int hInstrumentHandle, int iChannel , double lfSetValue , int iMarker , char *szMarkerRequest )
{
	STD_ERROR				StdError				=	{0};

	unsigned short			iStatus					=	0;

	CHK_ERR(ag875x_markerSet (hInstrumentHandle, szMarkerRequest, iMarker , lfSetValue , iChannel ));

	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_GetMarker(int hInstrumentHandle, int iChannel , int iMarker , char *szMarkerRequest , double *lfGetMarkerOneValue  , double *lfGetMarkerTwoValue , double *lfGetMarkerThreeValue , double *lfGetMarkerFourValue )
{
	STD_ERROR				StdError				=	{0};

	CHK_ERR(ag875x_markerQuery (hInstrumentHandle, lfGetMarkerOneValue , lfGetMarkerTwoValue , lfGetMarkerThreeValue , szMarkerRequest, iMarker, iChannel, lfGetMarkerFourValue ));

Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_ReCallRegister(int hInstrumentHandle , char *szRegister_FileName )
{
	STD_ERROR				StdError				=	{0};

	unsigned short			iStatus					=	0,
							iRegister				=	0;  
	
	iRegister = atoi( szRegister_FileName );
	
	if (( iRegister == 0 ) && ( strlen(szRegister_FileName) > 1 ))
	{

	}
	else
	{
		CHK_ERR(ag875x_recallOrClearReg (hInstrumentHandle, ag875x_RC_RECALL_REG, iRegister ));
	}
	
	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_SaveToRegister(int hInstrumentHandle, char szTitle[8] , int iRegister )
{
	STD_ERROR				StdError				=	{0};
	
	unsigned short			iStatus					=	0;

	CHK_ERR(ag875x_saveToReg (hInstrumentHandle, szTitle , iRegister));
	
	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_SendLowLevel(int hInstrumentHandle, char szCommand[256] )
{
	STD_ERROR				StdError				=	{0};

	unsigned short			iStatus					=	0;

	CHK_ERR(ag875x_cmd (hInstrumentHandle, szCommand ));
	
	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_QuaryLowLevel(int hInstrumentHandle , char szCommand[256] , char *pResponse  , int iBufferlen )
{
	STD_ERROR				StdError				=	{0};

	unsigned short			iStatus					=	0;

	CHK_ERR(ag875x_cmdString_Q (hInstrumentHandle, szCommand , iBufferlen , pResponse ));
	
	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT   NetworkAnalyzer_QuaryCalibration(int hInstrumentHandle , int iChannel  , char *pResponse  )
{
	STD_ERROR				StdError				=	{0};

	unsigned short			iStatus					=	0;

	CHK_ERR(ag875x_calType_Q (hInstrumentHandle, iChannel , pResponse  ));
	
	ag875x_opc_Q(hInstrumentHandle, "WAIT", &iStatus);  
	
Error: 
	
	RETURN_STDERR_POINTER;
}

int main (int argc, char *argv[])
{
	int				hInstrumentHandle				=	0,
					iInstrumentConnectStatus		=	0,
					iInstrumentChanelsQuantity		=	0;
	
	double			*plfFirstVector					=	NULL;
		
	int				iNumberOfPoints					=	0;
	
	if (InitCVIRTE (0, argv, 0) == 0)
        return -1;  /* out of memory */
	
	NetworkAnalyzer_Init( 0 , "GPIB::16" , &hInstrumentHandle, &iInstrumentConnectStatus, &iInstrumentChanelsQuantity);
	
	NetworkAnalyzer_ReCallRegister( hInstrumentHandle , "MACS_CAL" ); 
		
	NetworkAnalyzer_Reset( hInstrumentHandle);
	
	//NetworkAnalyzer_SetFrequency( hInstrumentHandle, 1E9 , 2E9 );
																																												 
	//NetworkAnalyzer_AutoScale( hInstrumentHandle);  
	
	NetworkAnalyzer_ReCallRegister( hInstrumentHandle , "10" );
	
	NetworkAnalyzer_GetTrace ( hInstrumentHandle ,  2 , 0 , 0 , 2 , NULL ,&plfFirstVector , &iNumberOfPoints );
	NetworkAnalyzer_GetTrace ( hInstrumentHandle ,  1 , 0 , 0 , 2 , NULL , &plfFirstVector , &iNumberOfPoints );
	NetworkAnalyzer_GetTrace ( hInstrumentHandle ,  3 , 0 , 0 , 2 , NULL , &plfFirstVector , &iNumberOfPoints );
	
	//NetworkAnalyzer_GetFrequency( hInstrumentHandle,double *plfStartFrequency, double *plfStopFrequency);
	//NetworkAnalyzer_SetPointsNum( hInstrumentHandle,int points);
	//NetworkAnalyzer_GetPointsNum( hInstrumentHandle,int *points);
	//NetworkAnalyzer_GetMeasureVectors( hInstrumentHandle,int iGetDataType, double **plfFirstVector,double **plfSecondVector, double **plfThirdVector,double **plfFourthVector);
	//NetworkAnalyzer_SetMeasureType( hInstrumentHandle,int iChannel, int iInSparam, int iConversation);
	//NetworkAnalyzer_DisplaySelect( hInstrumentHandle,int iChannel, int iDataMode, int iDataFormat);
	//NetworkAnalyzer_AutoScale( hInstrumentHandle);
	//NetworkAnalyzer_AvarageSmooth( hInstrumentHandle, iChannel, char *szSmoothRequest, double lfSetSmooth,double lfCurrentSmooth);
	//NetworkAnalyzer_Smooth( hInstrumentHandle, iChannel, int iState, double lfSetSmooth,int iNumberPoints);
	//NetworkAnalyzer_SetBandWidth( hInstrumentHandle, iChannel, double lfBW_Value, char *szBW_Request,double *lfGetBW_Value);
	//NetworkAnalyzer_SetFrequencySweepType( hInstrumentHandle,int iChannel, int iFrequencySweepType);
	//NetworkAnalyzer_SetPower( hInstrumentHandle, iChannel, double lfPower, char *szPowerRequest,int iPortNum);
	//NetworkAnalyzer_GetPower( hInstrumentHandle, iChannel, double *lfPower, int iPortNum);
	//NetworkAnalyzer_SetPowerRange( hInstrumentHandle,int iChannel, int iRange, char *szRangeRequest,int *iCurrentRange);
	//NetworkAnalyzer_SetPowerSweep( hInstrumentHandle,int iChannel, double lfStartPower, double lfStopPower,char *szSweepRequest, double *lfGetStartPower,double *lfGetStopPower);
	//NetworkAnalyzer_SetSweepTime( hInstrumentHandle,int iChannel, double lfSweepTime, char *szSweepTimeRequest,double *lfGetSweepTime);
	//NetworkAnalyzer_SetMarker( hInstrumentHandle,int iChannel, double lfSetValue, int iMarker,char *szMarkerRequest);
	//NetworkAnalyzer_GetMarker( hInstrumentHandle,int iChannel, int iMarker, char *szMarkerRequest,double *lfGetMarkerOneValue, double *lfGetMarkerTwoValue,double *lfGetMarkerThreeValue, double *lfGetMarkerFourValue);
	//NetworkAnalyzer_ReCallRegister( hInstrumentHandle,char *szRegister_FileName);
	//NetworkAnalyzer_SaveToRegister( hInstrumentHandle,char *szTitle, int iRegister);
	//NetworkAnalyzer_SendLowLevel( hInstrumentHandle,char *szCommand);
	//NetworkAnalyzer_QuaryLowLevel( hInstrumentHandle,char *szCommand, char *pResponse, int iBufferlen);
	//NetworkAnalyzer_QuaryCalibration( hInstrumentHandle,int iChannel, char *pResponse);
	//
	//NetworkAnalyzer_GetErrorTextMessage( hInstrumentHandle,int iError, char *pErrorMessage); 
	
	NetworkAnalyzer_Close( &hInstrumentHandle );


	return 0;
}
