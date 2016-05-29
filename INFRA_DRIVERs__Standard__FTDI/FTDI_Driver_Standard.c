//==============================================================================
// Include files 
#include "cvidef.h"
#include "Windows.h"
#include <userint.h>
#include <ansi_c.h>
#include <utility.h>
#include "GlobalDefines.h"
#include <formatio.h>
#include "toolbox.h"
#include "FTDI_Driver.h"
#include "FTDI_Driver_Standard.h"

#define			DEVICE_ID						STANDARD_FTDI_DRIVER_DEVICE_ID
//==============================================================================
// Constants

typedef enum { DEV_VALUE_TYPE_NONE , DEV_VALUE_TYPE_CHAR , DEV_VALUE_TYPE_SHORT , DEV_VALUE_TYPE_INT , DEV_VALUE_TYPE_LONG , DEV_VALUE_TYPE_FLOAT , DEV_VALUE_TYPE_DOUBLE , DEV_VALUE_TYPE_BUFFER , DEV_VALUE_TYPE_SENTINEL } teValueTypes;

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
			
			int							iSetDataShiftIndex,
										iGetDataShiftIndex;
										
			//------------ Signature --------------------------------//
			unsigned __int64			ulSignatureID;
			//-------------------------------------------------------//												
			
			int							bDoubleEnablePulse;  
			
			//---------- end of configuration sector ----------//
			int							hDriverHandle;  
			
			unsigned __int64			ullLastSetValue,
										ullLastGetValue;   
		};
	};
	
} tsSingleDeviceItem;

typedef struct
{
	tsSingleDeviceItem					*pFTDI_CommandList;
	int									listSize;
	
	int									bInitSuccess;
	
	int									iCommandIndex; 
	
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

//==============================================================================
// Global variables

//==============================================================================
// Global functions

 //==============================================================================
// DLL main entry-point functions

void* DLLEXPORT FTDI_Driver_GetErrorTextMessage ( int vhInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR				StdError					=	{0};    

	StdError.error = FTDI_GetErrorMessage( iError , pErrorMessage ); 
	
Error: 
	
	RETURN_STDERR_POINTER;
}
	 
void*	DLLEXPORT	FTDI_STD_Driver_Config_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet )
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

void*   DLLEXPORT		FTDI_Driver_Config_Install_CommentCallback( int *pHandle , pfCommentCallback fFunction , void *pData , int type) 
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

void*   DLLEXPORT		FTDI_Driver_Config_Install_ConfigValueCallback( int *pHandle , pfConfigValueCallback fFunction , void *pData , int type) 
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

void*   DLLEXPORT		FTDI_Driver_Config_Install_CheckForBreakCallback( int *pHandle , pfCheckForBreakCallback fFunction , void *pData , int type) 
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

void*	DLLEXPORT	FTDI_Driver_Config_LOG_SetAllowState( int iHandle , int bAllowLog ) 
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

void*   DLLEXPORT       FTDI_Driver_GetConnectionAddress( int hHandle , char **szAddress )
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

void* DLLEXPORT FTDI_Driver_Close( int *pHandle )
{
	STD_ERROR					StdError					=	{0};
	
	tsLocalStorage				*pLocalStorage				=	NULL;			
	
	int 						Handle						=	0,
								hLastHandle					=	0;
									
	int							iIndex						=	0,
								iFoundIndex					=	0;
	
	IF (( pHandle == NULL ) , "Handle can't be NULL" );
	
	Handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr (Handle, &pLocalStorage));
	
	if ( pLocalStorage->pFTDI_CommandList )
		for ( iIndex = 0; iIndex < pLocalStorage->listSize; iIndex++ )
		{
			if ( pLocalStorage->pFTDI_CommandList[iIndex].hDriverHandle )
			{
				hLastHandle = pLocalStorage->pFTDI_CommandList[iIndex].hDriverHandle;
			
				FREE_STDERR_COPY_ERR( FTDI_Close( &(pLocalStorage->pFTDI_CommandList[iIndex].hDriverHandle) ));
			
				for ( iFoundIndex = iIndex; iFoundIndex < pLocalStorage->listSize; iFoundIndex++ ) 
					if ( hLastHandle == pLocalStorage->pFTDI_CommandList[iFoundIndex].hDriverHandle )
						pLocalStorage->pFTDI_CommandList[iFoundIndex].hDriverHandle = 0;
			}
		}
	
Error:
	
	if (Handle)
	{
		FREE(pLocalStorage->pszConnectionName);
		FREE(pLocalStorage->pszAddress);	
		FREE(pLocalStorage->pLastError);
		FREE(pLocalStorage->pFTDI_CommandList);
		
		CmtReleaseTSVPtr (Handle);
		CmtDiscardTSV (Handle);
	}
	
	*pHandle = 0;
	
	RETURN_STDERR_POINTER;
}


void* DLLEXPORT FTDI_Driver_Init( char *pszConnectionName , char *pszAddress , int *pHandle )
{									
	STD_ERROR					StdError						=	{0};
	
	CmtTSVHandle 				handle							=	0;
	
	tsLocalStorage				*pLocalStorage					=	NULL;			
	
	unsigned char				ucBitConfig						=	0,
								ucUseBitsMap					=	0,
								ucBitDirectionMask				=	0;
	
	int							iIndex							=	0,
								iCommandIndex					=	0,       
								iBaudRate						=	0, 
								iNumberOfPorts					=	0,
								iPortIndex						=	0,
								iCBUS_Port_Number				=	0;
									
	int							hConnectionHandle				=	0;
	
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
	
	if (( pLocalStorage->pFTDI_CommandList ) && ( pLocalStorage->listSize ))
	{
		hConnectionHandle = pLocalStorage->pFTDI_CommandList[0].hDriverHandle; 
		
		for ( iCommandIndex = 0; iCommandIndex < pLocalStorage->listSize; iCommandIndex++ )
		{
			if ( hConnectionHandle == 0 )
			{
				FREE_STDERR_COPY_ERR( FTDI_Init( pLocalStorage->pFTDI_CommandList[iCommandIndex].iConfigType , pLocalStorage->pFTDI_CommandList[iCommandIndex].szDeviceName , &(pLocalStorage->pFTDI_CommandList[iCommandIndex].hDriverHandle) ));
				
				hConnectionHandle = pLocalStorage->pFTDI_CommandList[iCommandIndex].hDriverHandle;
			}
			else
				pLocalStorage->pFTDI_CommandList[iCommandIndex].hDriverHandle = hConnectionHandle;
				
			
			if ( pLocalStorage->bInitSuccess == 0 )
			{
				FREE_STDERR_COPY_ERR( FTDI_GetPortQuantity( pLocalStorage->pFTDI_CommandList[iCommandIndex].hDriverHandle , &iNumberOfPorts ));
			
				FREE_STDERR_COPY_ERR( FTDI_Get_CBUS_PortNumber( pLocalStorage->pFTDI_CommandList[iCommandIndex].hDriverHandle , &iCBUS_Port_Number )); 
				
				for ( iPortIndex = 0; iPortIndex < iNumberOfPorts; iPortIndex++ )
				{		
					if (( iPortIndex + 1 ) != iCBUS_Port_Number )
					{
						if ( pLocalStorage->pFTDI_CommandList[iCommandIndex].pBaudRateArray[iPortIndex] )
							iBaudRate = pLocalStorage->pFTDI_CommandList[iCommandIndex].pBaudRateArray[iPortIndex];
			
						FREE_STDERR_COPY_ERR( FTDI_SetSpeed( pLocalStorage->pFTDI_CommandList[iCommandIndex].hDriverHandle , iPortIndex , iBaudRate ));
		
						FREE_STDERR_COPY_ERR( FTDI_SetModeConfiguration( pLocalStorage->pFTDI_CommandList[iCommandIndex].hDriverHandle , iPortIndex , BMODE_ASYNCHRONOUS_BIT_BANG , 0x0FF ));
					}
					else
					{
						FREE_STDERR_COPY_ERR( FTDI_SetModeConfiguration( pLocalStorage->pFTDI_CommandList[iCommandIndex].hDriverHandle , iPortIndex , BMODE_CBUS_BIT_BANG , 0x0 ));
					}
				}
			}
			
			for ( iIndex = 0; iIndex < pLocalStorage->pFTDI_CommandList[iIndex].iPortsQuantity; iIndex++ )
			{		
				iPortIndex = pLocalStorage->pFTDI_CommandList[iCommandIndex].pPortNumbersArray[iIndex] - 1;
			
				if ( pLocalStorage->pFTDI_CommandList[iCommandIndex].iPortsQuantity > 1 )
				{
					ucUseBitsMap = (unsigned char)(pLocalStorage->pFTDI_CommandList[iCommandIndex].UseBitsMap >> (iPortIndex*8)) & 0x0FF;
					ucBitDirectionMask = (unsigned char)(pLocalStorage->pFTDI_CommandList[iCommandIndex].BitDirectionMask >> (iPortIndex*8)) & 0x0FF;   
				}
				else
				{
					ucUseBitsMap = (unsigned char)(pLocalStorage->pFTDI_CommandList[iCommandIndex].UseBitsMap);
					ucBitDirectionMask = (unsigned char)(pLocalStorage->pFTDI_CommandList[iCommandIndex].BitDirectionMask); 
				}
			
				ucBitConfig = ucUseBitsMap & ucBitDirectionMask;
	
				FREE_STDERR_COPY_ERR( FTDI_SetPortConfiguration( pLocalStorage->pFTDI_CommandList[iCommandIndex].hDriverHandle , iPortIndex , ucBitConfig));
			}
			
			FREE_STDERR_COPY_ERR( FTDI_SetPort( pLocalStorage->pFTDI_CommandList[iCommandIndex].hDriverHandle , ( pLocalStorage->pFTDI_CommandList[iCommandIndex].pPortNumbersArray[0] - 1 ), (char)pLocalStorage->pFTDI_CommandList[iCommandIndex].UseBitsMap , (char)pLocalStorage->pFTDI_CommandList[iCommandIndex].DefValue ));
			
			pLocalStorage->bInitSuccess = 1;
		}
	}
	
Error:
	
	if (handle)  
	{
		CmtReleaseTSVPtr (handle); 
	}
	
	RETURN_STDERR_POINTER;
}

void*   DLLEXPORT   FTDI_Driver_SetValue( int hHandle , char *pCommandName , void *pValue , int iValueLength , int iValueSize , ...  )
{
	STD_ERROR					StdError					=	{0};
	
	tsLocalStorage				*pLocalStorage				=	NULL;			
	
	unsigned char				ucSingleByte				=	0;
	
	unsigned long long			ullSPIValue					=	0;
	
	int							iCommandIndex				=	0;
	
	int							bCommandFound				=	0;
	
//	unsigned char				ucBitIndex					=	0,
//								ucBitMask					=	0;
							
	IF (( pValue == NULL ) ,"Value pointer is NULL" );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle, &pLocalStorage));

	for ( iCommandIndex = 0 ; iCommandIndex < pLocalStorage->listSize ; iCommandIndex++ )
		if ((strlen(pLocalStorage->pFTDI_CommandList[iCommandIndex].szCommandName)) && ( strcmp( pCommandName , pLocalStorage->pFTDI_CommandList[iCommandIndex].szCommandName ) == 0 ))
		{
			bCommandFound = 1;
			break;
		}
		
	IF (( bCommandFound == 0 ) , "Command was not found.");   
		
	if ( pLocalStorage->bInitSuccess == 0 )
	{
		FREE_STDERR_COPY_ERR( FTDI_Driver_Init( pLocalStorage->pszConnectionName , pLocalStorage->pszAddress , &hHandle ));
	}
		
	pLocalStorage->iCommandIndex = iCommandIndex;
	
	switch( pLocalStorage->pFTDI_CommandList[iCommandIndex].iInterfaceType )
	{
		case 1: //SPI	
			
			switch(iValueSize)
			{
				case 1:
						ullSPIValue = *((char*)pValue);
						break;
				case 2:
						ullSPIValue = *((short*)pValue);
						break;
				case 4:
						ullSPIValue = *((int*)pValue);
						break;						
				case 8:
						ullSPIValue = (char)*((__int64*)pValue);
						break;
		
				default:
						break;
			}
			
			if ( pLocalStorage->pFTDI_CommandList[iCommandIndex].iSetDataShiftIndex )
			{
				if ( pLocalStorage->pFTDI_CommandList[iCommandIndex].iSetDataShiftIndex > 0 ) 
				{
					ullSPIValue <<= pLocalStorage->pFTDI_CommandList[iCommandIndex].iSetDataShiftIndex;	
				}
				else
				{
					ullSPIValue >>= (0 - pLocalStorage->pFTDI_CommandList[iCommandIndex].iSetDataShiftIndex);	
				}
			}
			
			FREE_STDERR_COPY_ERR( FTDI_SPI( pLocalStorage->pFTDI_CommandList[iCommandIndex].hDriverHandle ,\
											( pLocalStorage->pFTDI_CommandList[iCommandIndex].pPortNumbersArray[0] - 1 ),\
											pLocalStorage->pFTDI_CommandList[iCommandIndex].iSPI_EnableBitIndex ,\
											pLocalStorage->pFTDI_CommandList[iCommandIndex].iSPI_EnableBitPolarity ,\
											pLocalStorage->pFTDI_CommandList[iCommandIndex].iSPI_ClockBitIndex ,\
											pLocalStorage->pFTDI_CommandList[iCommandIndex].iSPI_ClockBitPolarity ,\
											pLocalStorage->pFTDI_CommandList[iCommandIndex].iSPI_ClockBitPhase ,\
											pLocalStorage->pFTDI_CommandList[iCommandIndex].iSPI_DataBitIndex ,\
											ullSPIValue , pLocalStorage->pFTDI_CommandList[iCommandIndex].iSPI_WriteBitsCount ,\
											pLocalStorage->pFTDI_CommandList[iCommandIndex].iSPI_ReadBitPort ,\
											pLocalStorage->pFTDI_CommandList[iCommandIndex].iSPI_ReadBitIndex ,\
											&(pLocalStorage->pFTDI_CommandList[iCommandIndex].ullLastGetValue) ,\
											pLocalStorage->pFTDI_CommandList[iCommandIndex].iSPI_ReadBitsCount ,\
											pLocalStorage->pFTDI_CommandList[iCommandIndex].bDoubleEnablePulse ));
			break;
			
		default:	// I/O
		
			switch(iValueSize)
			{
				case 1:
						ucSingleByte = *((char*)pValue);
						break;
				case 2:
						ucSingleByte = *((short*)pValue);
						break;
				case 4:
						ucSingleByte = *((int*)pValue);
						break;						
				case 8:
						ucSingleByte = (char)*((__int64*)pValue);
						break;
		
				default:
						break;
			}
	
					/*
					if (0)
					{
						ucBitMask = (char)pLocalStorage->pFTDI_CommandList[iCommandIndex].UseBitsMap; 
		
						while ( ucBitMask >>= 1 )
							ucBitIndex++;

						FREE_STDERR_COPY_ERR( FTDI_SetBit( pLocalStorage->pFTDI_CommandList[iCommandIndex].hDriverHandle , pLocalStorage->pFTDI_CommandList[iCommandIndex].pPortNumbersArray[0], ucBitIndex , ucSingleByte ));
					}
					else
					*/
					
			if ( pLocalStorage->pFTDI_CommandList[iCommandIndex].iSetDataShiftIndex )
			{
				if ( pLocalStorage->pFTDI_CommandList[iCommandIndex].iSetDataShiftIndex > 0 ) 
				{
					ucSingleByte <<= pLocalStorage->pFTDI_CommandList[iCommandIndex].iSetDataShiftIndex;	
				}
				else
				{
					ucSingleByte >>= (0 - pLocalStorage->pFTDI_CommandList[iCommandIndex].iSetDataShiftIndex);	
				}
			}
			
			FREE_STDERR_COPY_ERR( FTDI_SetPort( pLocalStorage->pFTDI_CommandList[iCommandIndex].hDriverHandle , ( pLocalStorage->pFTDI_CommandList[iCommandIndex].pPortNumbersArray[0] - 1 ), (char)pLocalStorage->pFTDI_CommandList[iCommandIndex].UseBitsMap , ucSingleByte ));
			break;	
	}
		
Error:
	
	if (hHandle)
		CmtReleaseTSVPtr (hHandle);
	
	RETURN_STDERR_POINTER;
}

void*   DLLEXPORT   FTDI_Driver_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize, ...  )    
{
	STD_ERROR					StdError					=	{0};
	
	tsLocalStorage				*pLocalStorage				=	NULL;
	
	unsigned char				ucSingleByte				=	0;
		
	int							iCommandIndex				=	0;
	
	int							bCommandFound				=	0;
	
	IF (( pGetValue == NULL ), "Get Value pointer is NULL" );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle, &pLocalStorage));

	if ( pLocalStorage->bInitSuccess == 0 )
	{
		FREE_STDERR_COPY_ERR( FTDI_Driver_Init( pLocalStorage->pszConnectionName , pLocalStorage->pszAddress , &hHandle ));
	}

	iCommandIndex = pLocalStorage->iCommandIndex;
	
	switch( pLocalStorage->pFTDI_CommandList[iCommandIndex].iInterfaceType )
	{
		case 1: //SPI	
			
			CALLOC_COPY_ERR( ((__int64**)*pGetValue) , 1 , sizeof(__int64) , &(pLocalStorage->pFTDI_CommandList[iCommandIndex].ullLastGetValue) ,( 1 * sizeof(__int64)));

			if ( piValueSize )
				*piValueSize = sizeof(__int64);

			if ( piValueLength )
				*piValueLength = 1;
			
			break;
			
		default:	// I/O   
		
			/*
				
				if (pLocalStorage->pFTDI_CommandList[iCommandIndex].iMeasureType)
				{
					ucBitMask = (char)pLocalStorage->pFTDI_CommandList[iCommandIndex].UseBitsMap; 
		
					while ( ucBitMask >>= 1 )
						ucBitIndex++;

					FREE_STDERR_COPY_ERR( FTDI_GetBit( pLocalStorage->pFTDI_CommandList[iCommandIndex].hDriverHandle , pLocalStorage->pFTDI_CommandList[iCommandIndex].pPortNumbersArray[0], ucBitIndex ,&ucSingleByte));
				}
		
				else
			*/
		
			FREE_STDERR_COPY_ERR( FTDI_GetPort( pLocalStorage->pFTDI_CommandList[iCommandIndex].hDriverHandle , ( pLocalStorage->pFTDI_CommandList[iCommandIndex].pPortNumbersArray[0] - 1 ) , &ucSingleByte));
		
			if ( pLocalStorage->pFTDI_CommandList[iCommandIndex].iGetDataShiftIndex )
			{
				if ( pLocalStorage->pFTDI_CommandList[iCommandIndex].iGetDataShiftIndex > 0 ) 
				{
					ucSingleByte <<= pLocalStorage->pFTDI_CommandList[iCommandIndex].iGetDataShiftIndex;	
				}
				else
				{
					ucSingleByte >>= (0 - pLocalStorage->pFTDI_CommandList[iCommandIndex].iGetDataShiftIndex);	
				}
			}
			
			CALLOC_COPY_ERR( ((char**)*pGetValue) , 1 , sizeof(char) , &ucSingleByte ,( 1 * sizeof(char)));

			if ( piValueSize )
				*piValueSize = sizeof(char);

			if ( piValueLength )
				*piValueLength = 1;
			
			break;
	}
		
Error:
	
	if (hHandle)
		CmtReleaseTSVPtr (hHandle);
	
	RETURN_STDERR_POINTER;	
}
 
void*   DLLEXPORT   FTDI_Driver_GetValue_Ex( int hHandle , char *pCommandName , void **pGetValue , int *piValueLength , int *piValueSize, ...  )   
{
	STD_ERROR					StdError					=	{0};
	
	tsLocalStorage				*pLocalStorage				=	NULL;			
	
	unsigned char				ucSingleByte				=	0;
	
	unsigned long long			ullSPIValue					=	0;
	
	int							iCommandIndex				=	0;
	
	int							bCommandFound				=	0;
	
//	unsigned char				ucBitIndex					=	0,
//								ucBitMask					=	0;
							
	IF (( pGetValue == NULL ) ,"Value pointer is NULL" );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle, &pLocalStorage));

	for ( iCommandIndex = 0 ; iCommandIndex < pLocalStorage->listSize ; iCommandIndex++ )
		if ((strlen(pLocalStorage->pFTDI_CommandList[iCommandIndex].szCommandName)) && ( strcmp( pCommandName , pLocalStorage->pFTDI_CommandList[iCommandIndex].szCommandName ) == 0 ))
		{
			bCommandFound = 1;
			break;
		}
		
	IF (( bCommandFound == 0 ) , "Command was not found.");   
		
	if ( pLocalStorage->bInitSuccess == 0 )
	{
		FREE_STDERR_COPY_ERR( FTDI_Driver_Init( pLocalStorage->pszConnectionName , pLocalStorage->pszAddress , &hHandle ));
	}
		
	switch( pLocalStorage->pFTDI_CommandList[iCommandIndex].iInterfaceType )
	{
		case 1: //SPI	

			if ( pGetValue && piValueLength && piValueSize )
			{ 
				switch( *piValueSize )
				{
					case 1:
							ullSPIValue = *((char*)pGetValue);
							break;
					case 2:
							ullSPIValue = *((short*)pGetValue);
							break;
					case 4:
							ullSPIValue = *((int*)pGetValue);
							break;						
					case 8:
							ullSPIValue = (char)*((__int64*)pGetValue);
							break;
		
					default:
							break;
				}
			
				if ( pLocalStorage->pFTDI_CommandList[iCommandIndex].iSetDataShiftIndex )
				{
					if ( pLocalStorage->pFTDI_CommandList[iCommandIndex].iSetDataShiftIndex > 0 ) 
					{
						ullSPIValue <<= pLocalStorage->pFTDI_CommandList[iCommandIndex].iSetDataShiftIndex;	
					}
					else
					{
						ullSPIValue >>= (0 - pLocalStorage->pFTDI_CommandList[iCommandIndex].iSetDataShiftIndex);	
					}
				}
			}
			
			FREE_STDERR_COPY_ERR( FTDI_SPI( pLocalStorage->pFTDI_CommandList[iCommandIndex].hDriverHandle ,\
											( pLocalStorage->pFTDI_CommandList[iCommandIndex].pPortNumbersArray[0] - 1 ),\
											pLocalStorage->pFTDI_CommandList[iCommandIndex].iSPI_EnableBitIndex ,\
											pLocalStorage->pFTDI_CommandList[iCommandIndex].iSPI_EnableBitPolarity ,\
											pLocalStorage->pFTDI_CommandList[iCommandIndex].iSPI_ClockBitIndex ,\
											pLocalStorage->pFTDI_CommandList[iCommandIndex].iSPI_ClockBitPolarity ,\
											pLocalStorage->pFTDI_CommandList[iCommandIndex].iSPI_ClockBitPhase ,\
											pLocalStorage->pFTDI_CommandList[iCommandIndex].iSPI_DataBitIndex ,\
											ullSPIValue , pLocalStorage->pFTDI_CommandList[iCommandIndex].iSPI_WriteBitsCount ,\
											pLocalStorage->pFTDI_CommandList[iCommandIndex].iSPI_ReadBitPort ,\
											pLocalStorage->pFTDI_CommandList[iCommandIndex].iSPI_ReadBitIndex ,\
											&(pLocalStorage->pFTDI_CommandList[iCommandIndex].ullLastGetValue) ,\
											pLocalStorage->pFTDI_CommandList[iCommandIndex].iSPI_ReadBitsCount,\
											pLocalStorage->pFTDI_CommandList[iCommandIndex].bDoubleEnablePulse ));
			
			if ( pLocalStorage->pFTDI_CommandList[iCommandIndex].iGetDataShiftIndex )
			{
				if ( pLocalStorage->pFTDI_CommandList[iCommandIndex].iGetDataShiftIndex > 0 ) 
				{
					pLocalStorage->pFTDI_CommandList[iCommandIndex].ullLastGetValue <<= pLocalStorage->pFTDI_CommandList[iCommandIndex].iGetDataShiftIndex;	
				}
				else
				{
					pLocalStorage->pFTDI_CommandList[iCommandIndex].ullLastGetValue >>= (0 - pLocalStorage->pFTDI_CommandList[iCommandIndex].iGetDataShiftIndex);	
				}
			}
			
			CALLOC_COPY_ERR( ((__int64**)*pGetValue) , 1 , sizeof(__int64) , &(pLocalStorage->pFTDI_CommandList[iCommandIndex].ullLastGetValue) ,( 1 * sizeof(__int64)));

			if ( piValueSize )
				*piValueSize = sizeof(__int64);

			if ( piValueLength )
				*piValueLength = 1;
			
			break;
			
		default:	// I/O   
		
			/*
				
				if (pLocalStorage->pFTDI_CommandList[iCommandIndex].iMeasureType)
				{
					ucBitMask = (char)pLocalStorage->pFTDI_CommandList[iCommandIndex].UseBitsMap; 
		
					while ( ucBitMask >>= 1 )
						ucBitIndex++;

					FREE_STDERR_COPY_ERR( FTDI_GetBit( pLocalStorage->pFTDI_CommandList[iCommandIndex].hDriverHandle , pLocalStorage->pFTDI_CommandList[iCommandIndex].pPortNumbersArray[0], ucBitIndex ,&ucSingleByte));
				}
		
				else
			*/
		
			FREE_STDERR_COPY_ERR( FTDI_GetPort( pLocalStorage->pFTDI_CommandList[iCommandIndex].hDriverHandle , ( pLocalStorage->pFTDI_CommandList[iCommandIndex].pPortNumbersArray[0] - 1 ) , &ucSingleByte));
		
			if ( pLocalStorage->pFTDI_CommandList[iCommandIndex].iGetDataShiftIndex )
			{
				if ( pLocalStorage->pFTDI_CommandList[iCommandIndex].iGetDataShiftIndex > 0 ) 
				{
					ucSingleByte <<= pLocalStorage->pFTDI_CommandList[iCommandIndex].iGetDataShiftIndex;	
				}
				else
				{
					ucSingleByte >>= (0 - pLocalStorage->pFTDI_CommandList[iCommandIndex].iGetDataShiftIndex);	
				}
			}
			
			CALLOC_COPY_ERR( ((char*)*pGetValue) , 1 , sizeof(char) , &ucSingleByte ,( 1 * sizeof(char)));

			if ( piValueSize )
				*piValueSize = sizeof(char);

			if ( piValueLength )
				*piValueLength = 1;
			
			break;	
	}
		
Error:
	
	if (hHandle)
		CmtReleaseTSVPtr (hHandle);
	
	RETURN_STDERR_POINTER;
}

void*   DLLEXPORT   FTDI_Driver_Check_Connection( int hHandle , char *pCommandName , int *piStatus )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	int							iCommandIndex										=	0,
								bCommandFound										=	0,
								iNumberOfPorts										=	0; 
									
	IF (( hHandle == 0 ) , "handle can't be zero." );
	IF (( piStatus == 0 ) , "Status parameter can't be NULL." );
	
	*piStatus = 0;
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	for ( iCommandIndex = 0 ; iCommandIndex < pLocalStorage->listSize ; iCommandIndex++ )
		if ((strlen(pLocalStorage->pFTDI_CommandList[iCommandIndex].szCommandName)) && ( strcmp( pCommandName , pLocalStorage->pFTDI_CommandList[iCommandIndex].szCommandName ) == 0 ))
		{
			bCommandFound = 1;
			break;
		}
		
	IF (( bCommandFound == 0 ) , "Command was not found.");   
		
	if ( pLocalStorage->bInitSuccess == 0 )
	{
		FREE_STDERR_COPY_ERR( FTDI_Driver_Init( pLocalStorage->pszConnectionName , pLocalStorage->pszAddress , &hHandle ));
	}
	
	FREE_STDERR_COPY_ERR( FTDI_GetPortQuantity( pLocalStorage->pFTDI_CommandList[iCommandIndex].hDriverHandle , &iNumberOfPorts )); 
	
	if ( iNumberOfPorts )
		*piStatus = 1;
		
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

void*   DLLEXPORT   FTDI_Driver_Unplug( int hHandle , double lfTimeOut )
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


void*   DLLEXPORT   FTDI_Driver_PlugIn( int hHandle , double lfTimeOut )
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

void*   DLLEXPORT   FTDI_Driver_EnablePlugInAutomatically( int hHandle , int bEnable )
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

void*   DLLEXPORT   FTDI_Driver_InstallReceiveCallbackFunction( int hHandle , tfStdReceiveCallback pfFunction , void *pData )
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


void*   DLLEXPORT   FTDI_Driver_LoadConfigFile( int hHandle , char *pFileName )
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
	
	FREE_CALLOC_ERR( pLocalStorage->pFTDI_CommandList , 256 , sizeof(tsSingleDeviceItem));
	
	for ( iIndex = 0 ; iIndex < 256; iIndex++ )
	{
		iCount = ReadFile ( hFileHandle , pLocalStorage->pFTDI_CommandList[iIndex].max_size , sizeof(tsSingleDeviceItem) );
		
		if ( iCount != sizeof(tsSingleDeviceItem) )
			break;
		
		if ( pLocalStorage->pFTDI_CommandList[iIndex].ulSignatureID != 0 ) 
			if ( pLocalStorage->pFTDI_CommandList[iIndex].ulSignatureID != DEVICE_ID )
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

void*   DLLEXPORT   FTDI_Driver_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize )
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
		
	FREE_CALLOC_ERR( pLocalStorage->pFTDI_CommandList , 256 , sizeof(tsSingleDeviceItem));
	
	iCount = iBufferSize;
	iBaseSize = sizeof(tsSingleDeviceItem); 
	
	for ( iIndex = 0 ; iIndex < 256; iIndex++ )
	{
		if ( iCount < iBaseSize )   
			break;
		
		memcpy( pLocalStorage->pFTDI_CommandList[iIndex].max_size , pBuffer , iBaseSize );
		
		if ( pLocalStorage->pFTDI_CommandList[iIndex].ulSignatureID != 0 ) 
			if ( pLocalStorage->pFTDI_CommandList[iIndex].ulSignatureID != DEVICE_ID )
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



void*	DLLEXPORT		FTDI_Driver_Get_Commands_List( int hHandle , char ***pCommandsList , int *piNumberOfCommands )     
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
		CALLOC_COPY_STRING_ERR( (*pCommandsList)[iIndex] , pLocalStorage->pFTDI_CommandList[iIndex].max_size );
		
		if ( piNumberOfCommands && strlen((*pCommandsList)[iIndex]))
			(*piNumberOfCommands)++;
	}

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}


