//==============================================================================
// Include files
#include "Windows.h" 
#include <utility.h>
#include <ansi_c.h>
#include "GlobalDefines.h"
#include "toolbox.h"
#include "formatio.h"

#include "DaqMx_Driver.h"
#include "DaqMx_Driver_wrapper.h"
#include "DaqMx_Driver_Standard.h" 
//==============================================================================
// Constants

//==============================================================================
// Types

#define			DEVICE_ID			STANDARD_DAQMX_DRIVER_DEVICE_ID

typedef enum { DEV_VALUE_TYPE_NONE , DEV_VALUE_TYPE_CHAR , DEV_VALUE_TYPE_SHORT , DEV_VALUE_TYPE_INT , DEV_VALUE_TYPE_LONG , DEV_VALUE_TYPE_FLOAT , DEV_VALUE_TYPE_DOUBLE , DEV_VALUE_TYPE_BUFFER , DEV_VALUE_TYPE_SENTINEL } teValueTypes;

typedef enum { DEV_TYPE_NOT_DEF , DEV_TYPE_ANALOG_CURRENT_IN , DEV_TYPE_ANALOG_VOL_IN , DEV_TYPE_ANALOG_VOL_OUT , DEV_TYPE_DIGITAL ,  DEV_TYPE_COUNTER , DEV_TYPE_SENTINEL } teDevTypes;

typedef struct
{
	union
	{
		char							max_size[STD_BUFFER_SIZE];   
		
		struct					
		{
			char						szCommandName[LOW_STRING];

			int							iConfigType,
										iMeasureType;
	
			int							pPortNumbersArray[32],
										pBaudRateArray[32], 
										iPortsQuantity;
	
			unsigned __int64			UseBitsMap,
										BitDirectionMask,
										DefValue;
	
			char						bPowerUpState,
										bPullingState;
	
			double						dPowerUpLevel;
				
			char 						szDeviceName[64]; 
					
			teValueTypes				valueType;
			int							valueCount;
	
			double						lfSetDelay,
										lfSetTimeout;
	
			int							iSPI_ClockBitIndex,
										iSPI_ClockBitPolarity,
										iSPI_DataBitIndex,
										iSPI_EnableBitIndex,
										iSPI_EnableBitPolarity,
										iSPI_ReadBitIndex,
										iSPI_ReadBitPort;
			
			
			int							iAlalogMeasureNumberOfPoints,
										iAlalogMeasureSimpleRate,
										iAlalogMeasureFunction;	
			
			int							iInterfaceType;
			
			int							iSPI_WriteBitsCount,
										iSPI_ReadBitsCount,
										iSPI_ClockBitPhase;
										
			//------------ Signature --------------------------------//
			unsigned __int64			ulSignatureID;
			//-------------------------------------------------------//	
			
			double						lfMinValue,
										lfMaxValue;  
			
			//---------- end of configuration sector ----------//
			int							hDriverHandle;  
		};
	};
	
} tsSingleDeviceItem;

typedef struct
{
	tsSingleDeviceItem					*pDAQMX_CommandList;
	int									listSize;
	
	int									bInitSuccess;
	
	char								*pszConnectionName,
										*pszAddress;
	
	tsSTD_CallBackSet					*ptCallbacks;

	void								*pCallbackFunction,
										*pCallbackData;
	
	STD_ERROR							*pLastError; 
	
}tsLocalStorage;


//==============================================================================
// Static global variables

//==============================================================================
// Static functions
void*   DLLEXPORT   DAQMX_Driver_SetDefaultValue( tsLocalStorage *pLocalStorage , tsSingleDeviceItem command );

//==============================================================================
// Global variables

//==============================================================================
// Global functions

 //==============================================================================
// DLL main entry-point functions

void* DLLEXPORT DAQMX_STD_Driver_GetErrorTextMessage ( int vhInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR				StdError					=	{0};    

	StdError.error = DAQMX_GetErrorMessage( iError , pErrorMessage ); 
	
Error: 
	
	RETURN_STDERR_POINTER;
}
	 
void*	DLLEXPORT	DAQMX_STD_Driver_Config_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								handle										=	0;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );
	
	if ( *pHandle == 0 )
	{
		CHK_CMT(CmtNewTSV ( sizeof(tsLocalStorage) , pHandle ));
	}
	
	handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	if ( pLocalStorage->ptCallbacks == NULL )
	{
		CALLOC_ERR( pLocalStorage->ptCallbacks , 1 , sizeof(tsSTD_CallBackSet));  
	}
	
	memcpy( pLocalStorage->ptCallbacks , pCallBackSet , sizeof(tsSTD_CallBackSet));  

Error:
	
	if ( handle )
		CmtReleaseTSVPtr (handle);
	
	RETURN_STDERR_POINTER;
}

void*   DLLEXPORT		DAQMX_Driver_Config_Install_CommentCallback( int *pHandle , pfCommentCallback fFunction , void *pData , int type) 
{
 	STD_ERROR        				StdError											=	{0};
	
	tsLocalStorage					*pLocalStorage										=	NULL;			
	
	int								handle												=	0;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL."  );
	
	if ( *pHandle == 0 )
	{
		CHK_CMT(CmtNewTSV ( sizeof(tsLocalStorage) , pHandle ));
	}
	
	handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	if ( pLocalStorage->ptCallbacks == NULL )
	{
		CALLOC_ERR( pLocalStorage->ptCallbacks , 1 , sizeof(tsSTD_CallBackSet));  
	}
	
	pLocalStorage->ptCallbacks->fCommentCallback = fFunction;
	pLocalStorage->ptCallbacks->pCommentCallbackData = pData;
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr (handle);
	
	RETURN_STDERR_POINTER;	
}

void*   DLLEXPORT		DAQMX_Driver_Config_Install_ConfigValueCallback( int *pHandle , pfConfigValueCallback fFunction , void *pData , int type) 
{
 	STD_ERROR        					StdError											=	{0};
	
	tsLocalStorage						*pLocalStorage										=	NULL;			
								
	int									handle												=	0;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL."  );
	
	if ( *pHandle == 0 )
	{
		CHK_CMT(CmtNewTSV ( sizeof(tsLocalStorage) , pHandle ));
	}
	
	handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	if ( pLocalStorage->ptCallbacks == NULL )
	{
		CALLOC_ERR( pLocalStorage->ptCallbacks , 1 , sizeof(tsSTD_CallBackSet));  
	}
	
	pLocalStorage->ptCallbacks->fConfigValueCallback = fFunction;
	pLocalStorage->ptCallbacks->pConfigValueCallbackData = pData;

Error:
	
	if ( handle )
		CmtReleaseTSVPtr (handle);
	
	RETURN_STDERR_POINTER;	
}																	   		

void*   DLLEXPORT		DAQMX_Driver_Config_Install_CheckForBreakCallback( int *pHandle , pfCheckForBreakCallback fFunction , void *pData , int type) 
{
 	STD_ERROR        					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage								=	NULL;			
	
	int								handle										=	0;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL."  );
	
	if ( *pHandle == 0 )
	{
		CHK_CMT(CmtNewTSV ( sizeof(tsLocalStorage) , pHandle ));
	}
	
	handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	if ( pLocalStorage->ptCallbacks == NULL )
	{
		CALLOC_ERR( pLocalStorage->ptCallbacks , 1 , sizeof(tsSTD_CallBackSet));  
	}
	
	pLocalStorage->ptCallbacks->fCheckForBreakCallback = fFunction;
	pLocalStorage->ptCallbacks->pCheckForBreakCallbackData = pData;

Error:
	
	if ( handle )
		CmtReleaseTSVPtr (handle);
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT	DAQMX_Driver_Config_LOG_SetAllowState( int iHandle , int bAllowLog ) 
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	IF (( iHandle == 0 ) , "Driver has not initialized" );

	CHK_CMT( CmtGetTSVPtr ( iHandle , &pLocalStorage ));
	
	pLocalStorage->ptCallbacks->bDisableLog = !bAllowLog;

Error:
	
	if ( iHandle )
		CmtReleaseTSVPtr (iHandle);
	
	RETURN_STDERR_POINTER;
}

void*   DLLEXPORT       DAQMX_Driver_GetConnectionAddress( int hHandle , char **szAddress )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	IF (( hHandle == 0 ) , "Pointer to handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
Error:
	
	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	RETURN_STDERR_POINTER;
}

//==============================================================================


int		DAQMX_Driver_GetValueType( int hDriverHandle )
{
	STD_ERROR							StdError					=	{0};
	
	int									type						=	DEV_VALUE_TYPE_CHAR;
	
	teDigitalConnectionSizeType			sizeType					=	0; 
	
	FREE_STDERR_COPY_ERR( DAQMX_GetValueType( hDriverHandle , &sizeType ));
	
	switch( sizeType )
	{
		case DIGITAL_CONNECT_1_BIT:
			type = DEV_VALUE_TYPE_CHAR;
			break;
		case DIGITAL_CONNECT_8_BIT:
			type = DEV_VALUE_TYPE_CHAR;
			break;
		case DIGITAL_CONNECT_16_BIT:
			type = DEV_VALUE_TYPE_INT;
			break;
		case DIGITAL_CONNECT_32_BIT:
			type = DEV_VALUE_TYPE_INT;
			break;
		case DIGITAL_CONNECT_40_BIT:
			type = DEV_VALUE_TYPE_LONG; 
			break;
		case DIGITAL_CONNECT_48_BIT:
			type = DEV_VALUE_TYPE_LONG;
			break;
		case DIGITAL_CONNECT_64_BIT:
			type = DEV_VALUE_TYPE_LONG;
			break;
			
		default:
			
			FORCE_ERR(-1,"Wrong Value Type");
	}
	
Error:
	
	return type;	
}

void* DLLEXPORT DAQMX_Driver_Close( int *pHandle )
{
	STD_ERROR					StdError					=	{0};
	
	tsLocalStorage				*pLocalStorage				=	NULL;			
	
	int 						Handle						=	0;
	
	int							iIndex						=	0;
	
	IF (( pHandle == NULL ) , "Handle can't be NULL" );
	
	Handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr (Handle, &pLocalStorage));
	
	if ( pLocalStorage->pDAQMX_CommandList )
		for ( iIndex = 0; iIndex < pLocalStorage->listSize; iIndex++ )
		{
			FREE_STDERR_COPY_ERR( DAQMX_Close( &(pLocalStorage->pDAQMX_CommandList[iIndex].hDriverHandle) ));
		}
	
Error:
	
	if (Handle)
	{
		FREE(pLocalStorage->pszConnectionName);
		FREE(pLocalStorage->pszAddress);	
		FREE(pLocalStorage->pLastError);
		
		CmtReleaseTSVPtr (Handle);
		CmtDiscardTSV (Handle);
	}
	
	*pHandle = 0;
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT DAQMX_Driver_Init( char *pszConnectionName , char *pszAddress , int *pHandle )
{									
	STD_ERROR					StdError						=	{0};
	
	CmtTSVHandle 				handle							=	0;
	
	tsLocalStorage				*pLocalStorage					=	NULL;			
	
	tsDaqmxPortConfigItem		DAQmxtConfigItem 				=	{0};
	
	int							iIndex							=	0,
								iPortIndex						=	0;
	
	IF (( pHandle == NULL ) , "Handle can't be NULL" ); 
	
	handle = *pHandle;
	
	if ( handle == 0 )
	{
		CHK_CMT( CmtNewTSV (sizeof(tsLocalStorage), &handle));
	
		*pHandle = handle;
	}
	
	CHK_CMT( CmtGetTSVPtr (handle, &pLocalStorage));
	
	if ( pLocalStorage->ptCallbacks == NULL )
	{
		CALLOC_ERR( pLocalStorage->ptCallbacks , 1 , sizeof(tsSTD_CallBackSet));  
	}
	
	pLocalStorage->ptCallbacks->iInternalHandle = handle;
	
	strcpy( pLocalStorage->ptCallbacks->szName , pszConnectionName ); 
	CALLOC_COPY_STRING( pLocalStorage->pszConnectionName , pszConnectionName );
	CALLOC_COPY_STRING( pLocalStorage->pszAddress , pszAddress );
	
	if (( pLocalStorage->pDAQMX_CommandList ) && ( pLocalStorage->listSize ))
	{
		for ( iIndex = 0; iIndex < pLocalStorage->listSize; iIndex++ )
		{
			DAQmxtConfigItem.UseBitsMap = pLocalStorage->pDAQMX_CommandList[iIndex].UseBitsMap;
			DAQmxtConfigItem.BitDirectionMask = pLocalStorage->pDAQMX_CommandList[iIndex].BitDirectionMask;

			DAQmxtConfigItem.bPowerUpState = pLocalStorage->pDAQMX_CommandList[iIndex].bPowerUpState;
			DAQmxtConfigItem.bPullingState = pLocalStorage->pDAQMX_CommandList[iIndex].bPullingState;

			DAQmxtConfigItem.dPowerUpLevel = pLocalStorage->pDAQMX_CommandList[iIndex].dPowerUpLevel;
			
			DAQmxtConfigItem.DefValue = pLocalStorage->pDAQMX_CommandList[iIndex].DefValue;
			
			DAQmxtConfigItem.lfMinValue = pLocalStorage->pDAQMX_CommandList[iIndex].lfMinValue;
			DAQmxtConfigItem.lfMaxValue = pLocalStorage->pDAQMX_CommandList[iIndex].lfMaxValue;

			DAQmxtConfigItem.iMeasureType = pLocalStorage->pDAQMX_CommandList[iIndex].iMeasureType;

			CALLOC_COPY_ERR( DAQmxtConfigItem.pPortNumbersArray , pLocalStorage->pDAQMX_CommandList[iIndex].iPortsQuantity , sizeof(int) , pLocalStorage->pDAQMX_CommandList[iIndex].pPortNumbersArray , ( pLocalStorage->pDAQMX_CommandList[iIndex].iPortsQuantity * sizeof(int)));
	
			DAQmxtConfigItem.iPortArraySize = pLocalStorage->pDAQMX_CommandList[iIndex].iPortsQuantity;

			for ( iPortIndex = 0; iPortIndex < DAQmxtConfigItem.iPortArraySize; iPortIndex++ )
			{
				DAQmxtConfigItem.pPortNumbersArray[iPortIndex]--;	
				
				if ( DAQmxtConfigItem.pPortNumbersArray[iPortIndex] < 0 )
					DAQmxtConfigItem.pPortNumbersArray[iPortIndex] = 0;
			}
			
			FREE_STDERR_COPY_ERR( DAQMX_Init( pLocalStorage->pDAQMX_CommandList[iIndex].iConfigType , pLocalStorage->pDAQMX_CommandList[iIndex].szDeviceName , DAQmxtConfigItem , &(pLocalStorage->pDAQMX_CommandList[iIndex].hDriverHandle) ));
		
			pLocalStorage->bInitSuccess = 1;
			
			FREE_STDERR_COPY_ERR( DAQMX_Driver_SetDefaultValue( pLocalStorage , pLocalStorage->pDAQMX_CommandList[iIndex] ));
		}
	}
	
Error:
	
	if (handle)  
	{
		CmtReleaseTSVPtr (handle); 
	}
	
	RETURN_STDERR_POINTER;
}


void*   DLLEXPORT   DAQMX_Driver_SetDefaultValue( tsLocalStorage *pLocalStorage , tsSingleDeviceItem command )
{
	STD_ERROR					StdError					=	{0};
	
	if ( pLocalStorage->bInitSuccess == 0 )
	{
		RETURN;
	}
		
	if( command.iConfigType == DEV_TYPE_DIGITAL )
	{   
		FREE_STDERR_COPY_ERR( DAQMX_SetUniversal( command.hDriverHandle , command.DefValue )); 
	}
	
	if ( command.iConfigType == DEV_TYPE_ANALOG_VOL_OUT ) 
	{
		FREE_STDERR_COPY_ERR( DAQMX_SetAnalogLine( command.hDriverHandle , 0.0 ));
	}   
	
Error:

	RETURN_STDERR_POINTER;
}

void*   DLLEXPORT   DAQMX_Driver_SetValue( int hHandle , char *pCommandName , void *pValue , int iValueLength , int iValueSize , ...  )
{
	STD_ERROR					StdError					=	{0};
	
	tsLocalStorage				*pLocalStorage				=	NULL;			
	
	unsigned __int64			lValue						=	0;
		
	double						lfValue						=	0;
	
	unsigned char				iSingleByte					=	0;
	
	unsigned int				iSingleWWord				=	0; 
	
	int							iCommandIndex				=	0;
	
	int							bCommandFound				=	0;
	
	CHK_CMT( CmtGetTSVPtr ( hHandle, &pLocalStorage));

	IF (( pValue == NULL ) ,"Value pointer is NULL" );
	
	for ( iCommandIndex = 0 ; iCommandIndex < pLocalStorage->listSize ; iCommandIndex++ )
		if ((strlen(pLocalStorage->pDAQMX_CommandList[iCommandIndex].szCommandName)) && ( strcmp( pCommandName , pLocalStorage->pDAQMX_CommandList[iCommandIndex].szCommandName ) == 0 ))
		{
			bCommandFound = 1;
			break;
		}
		
	IF (( bCommandFound == 0 ) , "Command was not found.");   
		
	if ( pLocalStorage->bInitSuccess == 0 )
	{
		FREE_STDERR_COPY_ERR( DAQMX_Driver_Init( pLocalStorage->pszConnectionName , pLocalStorage->pszAddress , &hHandle ));
	}
		
	if( pLocalStorage->pDAQMX_CommandList[iCommandIndex].iConfigType == DEV_TYPE_DIGITAL )
	{
		switch( DAQMX_Driver_GetValueType( pLocalStorage->pDAQMX_CommandList[iCommandIndex].hDriverHandle )) 
		{
			case DEV_VALUE_TYPE_CHAR:
			
				switch(iValueSize)
				{
					case 1:
							iSingleByte = *((char*)pValue);
							break;
					case 2:
							iSingleByte = *((short*)pValue);
							break;
					case 4:
							iSingleByte = *((int*)pValue);
							break;						
					case 8:
							iSingleByte = (char)*((__int64*)pValue);
							break;
				
					default:
							break;
				}
			
				lValue = iSingleByte;
			
				break;
			
			case DEV_VALUE_TYPE_INT:
			
				switch(iValueSize)
				{
					case 1:
							iSingleWWord = *((char*)pValue);
							break;
					case 2:
							iSingleWWord = *((short*)pValue);
							break;
					case 4:
							iSingleWWord = *((int*)pValue);
							break;						
					case 8:
							iSingleWWord = (int) *((__int64*)pValue);
							break;
				
					default:
							break;
				}

				lValue = iSingleWWord;
			
				break;
			
			case DEV_VALUE_TYPE_LONG:
			
				switch(iValueSize)
				{
					case 1:
							lValue = *((char*)pValue);
							break;
					case 2:
							lValue = *((short*)pValue);
							break;
					case 4:
							lValue = *((int*)pValue);
							break;						
					case 8:
							lValue = *((__int64*)pValue);
							break;
				
					default:
							break;
				}
			
				break;
			
			default:
			
				FORCE_ERR(-1,"Wrong Value Type");
		}
	
		FREE_STDERR_COPY_ERR( DAQMX_SetUniversal( pLocalStorage->pDAQMX_CommandList[iCommandIndex].hDriverHandle , lValue )); 
	}
	
	if ( pLocalStorage->pDAQMX_CommandList[iCommandIndex].iConfigType == DEV_TYPE_ANALOG_VOL_OUT ) 
	{
		if ( iValueSize == 8 )
			lfValue = *((double*)pValue);
		
		FREE_STDERR_COPY_ERR( DAQMX_SetAnalogLine( pLocalStorage->pDAQMX_CommandList[iCommandIndex].hDriverHandle , lfValue ));
	}   
	
Error:
	
	if (hHandle)
		CmtReleaseTSVPtr (hHandle);
	
	RETURN_STDERR_POINTER;
}

void*   DLLEXPORT   DAQMX_Driver_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize, ...  )    
{
	STD_ERROR					StdError					=	{0};
	
	tsLocalStorage				*pLocalStorage				=	NULL;
	
	unsigned __int64			lValue						=	0,
								*pLongValue					=	NULL;
		
	unsigned char				*piSingleByte				=	NULL;
	
	unsigned int				*piSingleWWord				=	NULL; 
	
	double						*plfArray					=	NULL;
		
	double						lfValueDouble				=	0.0;
	
	int							type						=	0;
	
	int							iCommandIndex				=	0;
	
	int							bCommandFound				=	0;
	
	int							bSkipType					=	0;
	
	IF (( pGetValue == NULL ), "Get Value pointer is NULL" );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle, &pLocalStorage));

	for ( iCommandIndex = 0 ; iCommandIndex < pLocalStorage->listSize ; iCommandIndex++ )
		if ((strlen(pLocalStorage->pDAQMX_CommandList[iCommandIndex].szCommandName)) && ( strcmp( *pGetCommandName , pLocalStorage->pDAQMX_CommandList[iCommandIndex].szCommandName ) == 0 ))
		{
			bCommandFound = 1;
			break;
		}
		
	IF (( bCommandFound == 0 ) , "Command was not found.");   
		
	if ( pLocalStorage->bInitSuccess == 0 )
	{
		FREE_STDERR_COPY_ERR( DAQMX_Driver_Init( pLocalStorage->pszConnectionName , pLocalStorage->pszAddress , &hHandle ));
	}
	
	type = DAQMX_Driver_GetValueType( pLocalStorage->pDAQMX_CommandList[iCommandIndex].hDriverHandle );
	
	if( pLocalStorage->pDAQMX_CommandList[iCommandIndex].iConfigType == DEV_TYPE_DIGITAL )
	{
		FREE_STDERR_COPY_ERR( DAQMX_GetUniversal( pLocalStorage->pDAQMX_CommandList[iCommandIndex].hDriverHandle , &lValue )); 
	}
	
	if (( pLocalStorage->pDAQMX_CommandList[iCommandIndex].iConfigType == DEV_TYPE_ANALOG_CURRENT_IN ) || ( pLocalStorage->pDAQMX_CommandList[iCommandIndex].iConfigType == DEV_TYPE_ANALOG_VOL_IN ))
	{
		switch( pLocalStorage->pDAQMX_CommandList[iCommandIndex].iAlalogMeasureFunction )
		{
			case 0:		// Single Value
				
					FREE_STDERR_COPY_ERR( DAQMX_GetAnalogLine( pLocalStorage->pDAQMX_CommandList[iCommandIndex].hDriverHandle , &lfValueDouble )); 
					
					CALLOC_COPY_ERR( *((double**)pGetValue) , 1 , sizeof(double) , (double*)&lfValueDouble ,( 1 * sizeof(double)));
						
					if ( piValueSize )
						*piValueSize = sizeof(double);
			
					if ( piValueLength )
						*piValueLength = 1;
					
					bSkipType = 1;
					
					break;
			
			case 1:		// Average Value
					break;
					
			case 2:		// Frequency
					break;
					
			case 3:		// Amplitude
					break;
					
			case 4:		// Pulse Param ( Rise Time )
					break;
			
			case 5:		// Pulse Param ( Fall Time )
					break;
					
			case 6:		// Pulse Param ( Width )
					break;
					
			case 7:		// Pulse Param ( Amplitude )
					break;
					
			case 8:		// Wave Form
				
					FREE_STDERR_COPY_ERR( DAQMX_GetAnalogLineWave( pLocalStorage->pDAQMX_CommandList[iCommandIndex].hDriverHandle, pLocalStorage->pDAQMX_CommandList[iCommandIndex].lfSetTimeout , pLocalStorage->pDAQMX_CommandList[iCommandIndex].iAlalogMeasureNumberOfPoints , pLocalStorage->pDAQMX_CommandList[iCommandIndex].iAlalogMeasureSimpleRate , &plfArray ));   
					
					*pGetValue = plfArray;
						
					if ( piValueSize )
						*piValueSize = sizeof(double);
			
					if ( piValueLength )
						*piValueLength = pLocalStorage->pDAQMX_CommandList[iCommandIndex].iAlalogMeasureNumberOfPoints;
					
					bSkipType = 1;
					
					break;
					
			default:   // Single Value
					break;
		}
	}
		
	if ( bSkipType == 0 )
	{
		switch( type ) 
		{
			case DEV_VALUE_TYPE_CHAR:
		
				CALLOC_COPY_ERR( *((char**)pGetValue) , 1 , sizeof(char) , (char*)&lValue ,( 1 * sizeof(char)));
		
				if ( piValueSize )
					*piValueSize = sizeof(char);
		
				if ( piValueLength )
					*piValueLength = 1;
		
				break;
		
			case DEV_VALUE_TYPE_INT:
		
				CALLOC_COPY_ERR( *((int**)pGetValue) , 1 , sizeof(int) , (int*)&lValue ,( 1 * sizeof(int)));
		
				if ( piValueSize )
					*piValueSize = sizeof(int);
		
				if ( piValueLength )
					*piValueLength = 1;
		
				break;
		
			case DEV_VALUE_TYPE_LONG:
		
				CALLOC_COPY_ERR( *((__int64**)pGetValue) , 1 , sizeof(__int64) , &lValue ,( 1 * sizeof(__int64)));
		
				if ( piValueSize )
					*piValueSize = sizeof(__int64);
		
				if ( piValueLength )
					*piValueLength = 1;
		
				break;
		
			default:
		
				FORCE_ERR(-1,"Wrong Value Type");
		}
	}
	
Error:
	
	if (hHandle)
		CmtReleaseTSVPtr (hHandle);
	
	RETURN_STDERR_POINTER;	
}
 
void*   DLLEXPORT   DAQMX_Driver_GetValue_Ex( int hHandle , char *pCommandName , void **pGetValue , int *piValueLength , int *piValueSize, ...  )   
{
	STD_ERROR					StdError					=	{0};

	IF (( pCommandName == NULL ) , "Command Name can't be NULL" );
	
	FREE_STDERR_COPY_ERR( DAQMX_Driver_GetValue( hHandle , &pCommandName , pGetValue , piValueLength , piValueSize  )); 
	
Error:
	
	RETURN_STDERR_POINTER;
}

void*   DLLEXPORT   DAQMX_Driver_Unplug( int hHandle , double lfTimeOut )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}


void*   DLLEXPORT   DAQMX_Driver_PlugIn( int hHandle , double lfTimeOut )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

void*   DLLEXPORT   DAQMX_Driver_EnablePlugInAutomatically( int hHandle , int bEnable )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

void*   DLLEXPORT   DAQMX_Driver_InstallReceiveCallbackFunction( int hHandle , tfStdReceiveCallback pfFunction , void *pData )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	pLocalStorage->pCallbackFunction = (void*) pfFunction;
	pLocalStorage->pCallbackData = pData;

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}


void*   DLLEXPORT   DAQMX_Driver_LoadConfigFile( int hHandle , char *pFileName )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			

	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};
	
	int							hFileHandle											=	0;
				
	int							iIndex												=	0,  
								iCount												=	0;
	
	char						szFormatedLog[LOW_STRING]							=	{0};

	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	IF (( pFileName == NULL ) , "File name is empty." );
	IF (( FileExists( pFileName , NULL ) == 0 )  , "File was not found." );  
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));  

	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks); 
	
	sprintf( szFormatedLog , "%s :: Loading configuration %%s" ,  pLocalStorage->ptCallbacks->szName );
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	
	hFileHandle = OpenFile ( pFileName , VAL_READ_ONLY, VAL_TRUNCATE, VAL_BINARY);
	
	FREE_CALLOC_ERR( pLocalStorage->pDAQMX_CommandList , 256 , sizeof(tsSingleDeviceItem));
	
	for ( iIndex = 0 ; iIndex < 256; iIndex++ )
	{
		iCount = ReadFile ( hFileHandle , pLocalStorage->pDAQMX_CommandList[iIndex].max_size , sizeof(tsSingleDeviceItem) );
		
		if ( iCount != sizeof(tsSingleDeviceItem) )
			break;
		
		if ( pLocalStorage->pDAQMX_CommandList[iIndex].ulSignatureID != 0 ) 
			if ( pLocalStorage->pDAQMX_CommandList[iIndex].ulSignatureID != DEVICE_ID )
				break;
	}
	
	pLocalStorage->listSize = iIndex;

Error:

	if ( hFileHandle )
		CloseFile (hFileHandle);
	
	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

void*   DLLEXPORT   DAQMX_Driver_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			

	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};

	int							iIndex												=	0,  
								iBaseSize											=	0, 
								iCount												=	0;

	char						szFormatedLog[LOW_STRING]							=	{0};
	
	IF (( hHandle == 0 ) , "handle can't be zero." );
	IF (( pBuffer == NULL ) , "Buffer is empty." );
		
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks); 
	
	sprintf( szFormatedLog , "%s :: Loading configuration %%s" ,  pLocalStorage->ptCallbacks->szName );
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
		
	FREE_CALLOC_ERR( pLocalStorage->pDAQMX_CommandList , 256 , sizeof(tsSingleDeviceItem));
	
	iCount = iBufferSize;
	iBaseSize = sizeof(tsSingleDeviceItem); 
	
	for ( iIndex = 0 ; iIndex < 256; iIndex++ )
	{
		if ( iCount < iBaseSize )   
			break;
		
		memcpy( pLocalStorage->pDAQMX_CommandList[iIndex].max_size , pBuffer , iBaseSize );
		
		if ( pLocalStorage->pDAQMX_CommandList[iIndex].ulSignatureID != 0 ) 
			if ( pLocalStorage->pDAQMX_CommandList[iIndex].ulSignatureID != DEVICE_ID )
				break;
				
		pBuffer += iBaseSize;
		iCount -= iBaseSize; 
	}
	
	pLocalStorage->listSize = iIndex;

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

void*   DLLEXPORT   DAQMX_Driver_Check_Connection( int hHandle , char *pCommandName , int *piStatus )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	int							iCommandIndex										=	0,
								bCommandFound										=	0; 
									
	IF (( hHandle == 0 ) , "handle can't be zero." );
	IF (( piStatus == 0 ) , "Status parameter can't be NULL." );
	
	*piStatus = 0;
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	for ( iCommandIndex = 0 ; iCommandIndex < pLocalStorage->listSize ; iCommandIndex++ )
		if ((strlen(pLocalStorage->pDAQMX_CommandList[iCommandIndex].szCommandName)) && ( strcmp( pCommandName , pLocalStorage->pDAQMX_CommandList[iCommandIndex].szCommandName ) == 0 ))
		{
			bCommandFound = 1;
			break;
		}
		
	IF (( bCommandFound == 0 ) , "Command was not found.");   
		
	if ( pLocalStorage->bInitSuccess == 0 )
	{
		FREE_STDERR_COPY_ERR( DAQMX_Driver_Init( pLocalStorage->pszConnectionName , pLocalStorage->pszAddress , &hHandle ));
	}
	
	if ( pLocalStorage->pDAQMX_CommandList[iCommandIndex].hDriverHandle )
		*piStatus = 1;
		
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

void*	DLLEXPORT		DAQMX_Driver_Get_Commands_List( int hHandle , char ***pCommandsList , int *piNumberOfCommands )     
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			

	int							iIndex												=	0;

	IF (( hHandle == 0 ) , "handle can't be zero." );
	IF (( pCommandsList == NULL ) , "Pointer to List is empty." );
		
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	CALLOC_ERR( (*pCommandsList) , pLocalStorage->listSize+1 , sizeof(char*));
	
	for ( iIndex = 0 ; iIndex < pLocalStorage->listSize; iIndex++ )
	{
		CALLOC_COPY_STRING_ERR( (*pCommandsList)[iIndex] , pLocalStorage->pDAQMX_CommandList[iIndex].max_size );
		
		if ( piNumberOfCommands )
			(*piNumberOfCommands)++;
	}

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}
