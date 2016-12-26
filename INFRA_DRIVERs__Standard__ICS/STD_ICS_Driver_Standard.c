//==============================================================================
// Include files 
#include "Windows.h"
#include <visa.h>

#include "cvidef.h"  
#include <userint.h>
#include <ansi_c.h>
#include <utility.h>
#include "GlobalDefines.h"
#include <formatio.h>
#include "toolbox.h"
#include "ICS_Driver.h"
#include "STD_ICS_Driver_Standard.h"

//==============================================================================
// Constants
#define			DEVICE_ID							STANDARD_ICS_DRIVER_DEVICE_ID

typedef enum { DEV_VALUE_TYPE_NONE , DEV_VALUE_TYPE_CHAR , DEV_VALUE_TYPE_SHORT , DEV_VALUE_TYPE_INT , DEV_VALUE_TYPE_LONG , DEV_VALUE_TYPE_FLOAT , DEV_VALUE_TYPE_DOUBLE , DEV_VALUE_TYPE_BUFFER , DEV_VALUE_TYPE_SENTINEL } teValueTypes;

typedef enum { VALUE_MODE_DEFAULT , VALUE_MODE_BIT_POSITION , VALUE_MODE_ON_OFF , VALUE_MODE_SENTINEL } teValueModes;

typedef struct
{
	union
	{
		char							max_size[STD_BUFFER_SIZE];   
		
		struct					
		{
			char						szCommandName[LOW_STRING];

			int							pPortNumbersArray[32],
										iPortsQuantity;
	
			unsigned __int64			UseBitsMap,
										BitDirectionMask,
										PolarityConfiguration,
										DefValue;
					
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
										iSPI_ReadBitPort,
										iSPI_WriteBitsCount,
										iSPI_ReadBitsCount,
										iSPI_ClockBitPhase;
			
			int							iSetDataShiftIndex,
										iGetDataShiftIndex;
			
			int							iInterfaceType;
			
			int							pFeedbackPortNumbersArray[32],
										iFeedbackPortsQuantity;
			
			unsigned __int64			FeedbackBitsMap;

			int							bFeedbackSwaped,
										bDataSwaped;
			
			teValueModes				valueMode,
										feedbackMode;

			//------------ Signature --------------------------------//
			unsigned __int64			ulSignatureID;
			//-------------------------------------------------------//			
			
			unsigned __int64			StateOnValue, 
							  			StateOffValue;
			
			int							SetValueShiftIndex,
										FeedbackValueShiftIndex;
			
			//---------- end of configuration sector ----------//
			
			unsigned __int64			ullLastSetValue,
										ullLastGetValue;  
		};
	};
	
} tsSingleDeviceItem;

typedef struct
{
	int									hDriverHandle;  

	tsSingleDeviceItem					*pICS_CommandList;
	int									listSize;
	
	int									iNumberOfPorts;
	
	char								viPortsDefaults[32];
	
	int									iLastCommandIndex;
	
	char								*pszAddress;
	
	tsSTD_CallBackSet					*ptCallbacks;

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

void*  	ICS_STD_Driver_GetErrorTextMessage ( int hHandle , int iError , char *pErrorMessage )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalHandle								=	NULL;			
	
	IF (( hHandle == 0 ) , "Handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalHandle ));  
	
	ICS_GetErrorTextMessage( pLocalHandle->hDriverHandle , iError , pErrorMessage );
	
Error:
	
	if ( hHandle )
		CmtReleaseTSVPtr (hHandle);
	
	RETURN_STDERR_POINTER;
}
	 
void*			ICS_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalHandle								=	NULL;			
	
	int								handle										=	0;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );
	
	if ( *pHandle == 0 )
	{
		CHK_CMT(CmtNewTSV ( sizeof(tsLocalStorage) , pHandle ));
	}
	
	handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalHandle ));      
	
	if ( pLocalHandle->ptCallbacks  == NULL )
	{
		CALLOC( pLocalHandle->ptCallbacks , 1 , sizeof(tsSTD_CallBackSet)); 
	}
	
	memcpy( pLocalHandle->ptCallbacks , pCallBackSet , sizeof(tsSTD_CallBackSet));  

Error:
	
	if ( handle )
		CmtReleaseTSVPtr (handle);
	
	RETURN_STDERR_POINTER;
}

void*		ICS_STD_Driver_GetLowLevelHandle( int hHandle , int *phLowLevelHandle )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage						*pLocalHandle								=	NULL;			
	
	IF (( hHandle == 0 ) , "Handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalHandle ));   
	
	FREE_STDERR_COPY_ERR( ICS_GetLowLevelHandle( pLocalHandle->hDriverHandle , phLowLevelHandle ));	
	
Error:
	
	if ( hHandle )
		CmtReleaseTSVPtr (hHandle);
	
	RETURN_STDERR_POINTER;
}

void*		ICS_STD_Driver_GetConnectionAddress( int hHandle , char **pszAddress )
{
	STD_ERROR							StdError									=	{0};

	tsLocalStorage						*pLocalHandle								=	NULL;			

	IF (( hHandle == 0 ) , "Handle can't be zero." );
	
	CHK_CMT ( CmtGetTSVPtr ( hHandle , &pLocalHandle ));

	if ( pszAddress )
	{
		CALLOC_COPY_STRING( pszAddress , pLocalHandle->pszAddress );
	}
	
Error:
	
	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );

	RETURN_STDERR_POINTER;
}

void*			ICS_STD_Driver_Equipment_Info ( int hLowLevelHandle , char *pAddress , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware )
{
	STD_ERROR						StdError									=	{0};
	
	int								iCount										=	0, 
									status										=	0, 
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
		IF (( strstr( szIdentificationText , "ICS Electronics" ) == NULL ) , "Is not supported" );
	
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

void*			ICS_STD_Driver_Equipment_IsSupported ( int hLowLevelHandle , char *pAddress , char *pID_String , char *pIdentificationText , int *pbSupporting , void **pLocalData )
{
	STD_ERROR						StdError										=	{0};
	
	int								iCount											=	0, 
									status											=	0, 
									iIndex											=	0, 
									bSupport										=	0, 
									defaultRM										=	0,
									hConnectionHandle								=	0;
	
	char							szIdentificationText[LOW_STRING]				=	{0},
									vszSupportdedModels[][16]						=	{{"8003"},{"8013"}};

	int								viPortQuantityList[]							=	{5,16};
	
	tsLocalStorage					*pLocalHandle									=	{0};
									
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
	
	if ( pLocalData )
	{
		pLocalHandle = (tsLocalStorage*)(*pLocalData);
		pLocalHandle->iNumberOfPorts = 0;
	}
	
	do
	{
		if ( strstr( szIdentificationText , "ICS Electronics" ) == NULL ) 
			break;
	
		for ( iIndex = 0; iIndex < (sizeof(vszSupportdedModels) / sizeof(vszSupportdedModels[0])); iIndex++ )
			if ( strstr( szIdentificationText , vszSupportdedModels[iIndex]) )
			{		
				if ( pLocalHandle )
					pLocalHandle->iNumberOfPorts = viPortQuantityList[iIndex]; 
				
				bSupport = 1; 
				break;
			}	
					
	}while(0);
	
	if (pbSupporting)
		*pbSupporting = bSupport;
	
	RETURN_STDERR_POINTER;
}

//==============================================================================

void*  ICS_STD_Driver_Close( int *pHandle )
{
	STD_ERROR					StdError					=	{0};
	
	tsLocalStorage				*pLocalStorage				=	NULL;			
	
	int 						Handle						=	0;
									
	IF (( pHandle == NULL ) , "Handle can't be NULL" );
	
	Handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr (Handle, &pLocalStorage));
	
	if ( pLocalStorage->hDriverHandle )
	{
		FREE_STDERR_COPY_ERR( ICS_Close( &(pLocalStorage->hDriverHandle) ));
	}
	
Error:
	
	if (Handle)
	{
		FREE(pLocalStorage->pszAddress);	
		FREE(pLocalStorage->pLastError);
		FREE(pLocalStorage->pICS_CommandList);
		
		CmtReleaseTSVPtr (Handle);
		CmtDiscardTSV (Handle);
	}
	
	*pHandle = 0;
	
	RETURN_STDERR_POINTER;
}


void*  ICS_STD_Driver_Init( char* szRsrcAddress , int *phInstrumentHandle )
{
	STD_ERROR						StdError										=	{0};
	
	tsLocalStorage					*pLocalHandle									=	NULL;
	
	CmtTSVHandle 					handle											=	0;
	
	int								supported										=	0;
	
	tsSTD_CallBackSet				tSTD_CallBackSet								=	{0};
	
	int								iPortNumber										=	0;
	
	IF (( szRsrcAddress == NULL ) , "Address string is empty" );
	IF (( phInstrumentHandle == NULL ) , "Handle can't be NULL" );  
	
	handle = *phInstrumentHandle;
	
	if ( handle == 0 )
	{
		CHK_CMT ( CmtNewTSV ( sizeof(tsLocalStorage) , &handle ));
		
		if ( phInstrumentHandle )
		{
			*(int*)phInstrumentHandle = handle;
		}
	}
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));

	FREE_STDERR_COPY_ERR( ICS_STD_Driver_Equipment_IsSupported ( NULL , szRsrcAddress , NULL , NULL , &supported , &pLocalHandle )); 

	IF (( supported == 0 ) , "This device has not supported." );

	STDF_UPDATE_CALLBACK_DATA(pLocalHandle->ptCallbacks); 
	
	//STDF_CONFIG_VALUE("Timeout", VAL_DOUBLE , 1 , lfTimeout , lfDefaultTimeout );	
	
	FREE_STDERR_COPY_ERR( ICS_Init ( 0 , szRsrcAddress , &(pLocalHandle->hDriverHandle) ));
	
	CALLOC_COPY_STRING( pLocalHandle->pszAddress , szRsrcAddress );
	
	for ( iPortNumber = 0; iPortNumber < 11; iPortNumber++ )
		pLocalHandle->viPortsDefaults[iPortNumber] = 0x0FF;
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER;
}

void*      ICS_STD_Driver_SetValue( int hHandle , char *pCommandName , void *pValue , int iValueLength , int iValueSize , ...  )
{
	STD_ERROR					StdError						=	{0};
	
	tsLocalStorage				*pLocalStorage					=	NULL;			
	
	unsigned char				ucSingleByte					=	0;
	
	unsigned long long			ullSPIValue						=	0;
	
	int							iIndex							=	0, 
								iSpitIndex						=	0,
								iBitCounter						=	0,
								iCommandIndex					=	0;
	
	int							iCommandFound					=	0,
								bCommandFound					=	0; 
								
	char						szSplitCommand[LOW_STRING]		=	{0},
								szSplitCommandFull[LOW_STRING]	=	{0};
	
	int							iPortNumber						=	0;
	
	unsigned __int64			ullUseBitsMap					=	0;
	
	__int64						llValue							=	0;
	
	IF (( hHandle == 0 ) , "Handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle, &pLocalStorage));

	IF (( pValue == NULL ) ,"Value pointer is NULL" );  
	
	for ( iCommandIndex = 0 ; iCommandIndex < pLocalStorage->listSize ; iCommandIndex++ )
		if ((strlen(pLocalStorage->pICS_CommandList[iCommandIndex].szCommandName)) && ( strcmp( pCommandName , pLocalStorage->pICS_CommandList[iCommandIndex].szCommandName ) == 0 ))
		{
			iCommandFound = 1;
			break;
		}
		
	if ( iCommandFound == 0 )
	{
		sprintf( szSplitCommand , "%s_" , pCommandName );
		
		for ( iCommandIndex = 0 ; iCommandIndex < pLocalStorage->listSize ; iCommandIndex++ )
			if ((strlen(pLocalStorage->pICS_CommandList[iCommandIndex].szCommandName)) && ( strstr( pLocalStorage->pICS_CommandList[iCommandIndex].szCommandName , szSplitCommand )))
				iCommandFound++;		
	}
	
	
	if (( iCommandFound == 0 ) && ( strcmp( pCommandName , "SAVE_DEF" ) == 0 ))
	{
		FREE_STDERR_COPY_ERR( ICS_SaveDefaultState( pLocalStorage->hDriverHandle )); 
	}
	
	IF (( iCommandFound == 0 ) , "Command was not found.");   
		
	pLocalStorage->iLastCommandIndex = iCommandIndex;
	
	switch( pLocalStorage->pICS_CommandList[iCommandIndex].iInterfaceType )
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
			
			if ( pLocalStorage->pICS_CommandList[iCommandIndex].iSetDataShiftIndex )
			{
				if ( pLocalStorage->pICS_CommandList[iCommandIndex].iSetDataShiftIndex > 0 ) 
				{
					ullSPIValue <<= pLocalStorage->pICS_CommandList[iCommandIndex].iSetDataShiftIndex;	
				}
				else
				{
					ullSPIValue >>= (0 - pLocalStorage->pICS_CommandList[iCommandIndex].iSetDataShiftIndex);	
				}
			}
			
			FREE_STDERR_COPY_ERR( ICS_SPI( pLocalStorage->hDriverHandle , ( pLocalStorage->pICS_CommandList[iCommandIndex].pPortNumbersArray[0] ), pLocalStorage->pICS_CommandList[iCommandIndex].iSPI_EnableBitIndex , pLocalStorage->pICS_CommandList[iCommandIndex].iSPI_EnableBitPolarity , pLocalStorage->pICS_CommandList[iCommandIndex].iSPI_ClockBitIndex , pLocalStorage->pICS_CommandList[iCommandIndex].iSPI_ClockBitPolarity , pLocalStorage->pICS_CommandList[iCommandIndex].iSPI_ClockBitPhase , pLocalStorage->pICS_CommandList[iCommandIndex].iSPI_DataBitIndex , ullSPIValue , pLocalStorage->pICS_CommandList[iCommandIndex].iSPI_WriteBitsCount , pLocalStorage->pICS_CommandList[iCommandIndex].iSPI_ReadBitPort , pLocalStorage->pICS_CommandList[iCommandIndex].iSPI_ReadBitIndex , &(pLocalStorage->pICS_CommandList[iCommandIndex].ullLastGetValue) , pLocalStorage->pICS_CommandList[iCommandIndex].iSPI_ReadBitsCount ));
			break;
			
		default:	// I/O
		
			for ( iSpitIndex = 0; iSpitIndex < iCommandFound; iSpitIndex++ )
			{
				switch(iValueSize)
				{
					case 1:
							llValue = *((char*)pValue);
							break;
					case 2:
							llValue = *((short*)pValue);
							break;
					case 4:
							llValue = *((int*)pValue);
							break;						
					case 8:
							llValue = (char)*((__int64*)pValue);
							break;
		
					default:
							break;
				}

				if ( iCommandFound > 1 )
				{
					sprintf( szSplitCommandFull , "%s%d" , szSplitCommand , (iSpitIndex+1) );
		
					for ( iCommandIndex = 0 ; iCommandIndex < pLocalStorage->listSize ; iCommandIndex++ )
						if ((strlen(pLocalStorage->pICS_CommandList[iCommandIndex].szCommandName)) && ( strcmp( pLocalStorage->pICS_CommandList[iCommandIndex].szCommandName , szSplitCommandFull ) == 0 ))
						{
							bCommandFound = 1;
							break;
						}
					
					if ( bCommandFound == 0 )
						continue;
				}
				
				switch ( pLocalStorage->pICS_CommandList[iCommandIndex].valueMode ) 
				{
					case VALUE_MODE_BIT_POSITION:
					
							llValue--;
				
							if ( llValue < 0 )
								llValue = 0;
				
							llValue = 1 << llValue;
							break;
						
					case VALUE_MODE_ON_OFF:
					
							if ( llValue )
								llValue = pLocalStorage->pICS_CommandList[iCommandIndex].StateOnValue;
							else
								llValue = pLocalStorage->pICS_CommandList[iCommandIndex].StateOffValue;
						
						
							break;
					
					default:
						break;
				}
				
				if ( iCommandFound > 1 )
				{
					ullUseBitsMap = pLocalStorage->pICS_CommandList[iCommandIndex].UseBitsMap; 
					
					for ( iIndex = 0; iIndex < sizeof(ullUseBitsMap); iIndex++ )  
						if ( ullUseBitsMap & 0x1 )
							break;
						else
							ullUseBitsMap >>= 1;
					
					llValue &= ullUseBitsMap << iBitCounter; 
					
					for ( iIndex = 0; iIndex < sizeof(llValue); iIndex++ )  
						if ( llValue & 0x1 )
							break;
						else
							llValue >>= 1;
					
					ullUseBitsMap = pLocalStorage->pICS_CommandList[iCommandIndex].UseBitsMap;
					
					for ( iIndex = 0; iIndex < sizeof(ullUseBitsMap); iIndex++ )
						if (( ullUseBitsMap >> iIndex ) & 1 )
							iBitCounter++;
				}	  
			
				if ( pLocalStorage->pICS_CommandList[iCommandIndex].bDataSwaped )
				{
					llValue = ~llValue;	
				}
			
				if ( pLocalStorage->pICS_CommandList[iCommandIndex].SetValueShiftIndex )
				{
					if ( pLocalStorage->pICS_CommandList[iCommandIndex].SetValueShiftIndex > 0 ) 
					{
						llValue <<= pLocalStorage->pICS_CommandList[iCommandIndex].SetValueShiftIndex;	
					}
					else
					{
						llValue >>= (0 - pLocalStorage->pICS_CommandList[iCommandIndex].SetValueShiftIndex);	
					}
				}

				iPortNumber = pLocalStorage->pICS_CommandList[iCommandIndex].pPortNumbersArray[0];
				
				ucSingleByte = (char)llValue;   
				
				FREE_STDERR_COPY_ERR( ICS_SetPort( pLocalStorage->hDriverHandle , iPortNumber , (char)pLocalStorage->pICS_CommandList[iCommandIndex].UseBitsMap , ucSingleByte , pLocalStorage->viPortsDefaults[iPortNumber] ));
			}
			
			DelayWithEventProcessing( pLocalStorage->pICS_CommandList[iCommandIndex].lfSetDelay );
			
			break;	
	}
		
Error:
	
	if (hHandle)
		CmtReleaseTSVPtr (hHandle);
	
	RETURN_STDERR_POINTER;
}

void*      ICS_STD_Driver_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize, ...  )    
{
	STD_ERROR					StdError					=	{0};
	
	tsLocalStorage				*pLocalStorage				=	NULL;
	
	unsigned char				ucSingleByte				=	0;
		
	int							iCommandIndex				=	0;
	
	int							iBitPositionIndex			=	0,
								iBitRealPosition			=	0;
	
	unsigned char 				ucTempMask					=	0;
	
	int							index						=	0;
	
	IF (( hHandle == 0 ) , "Handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle, &pLocalStorage));

	IF (( pGetValue == NULL ), "Get Value pointer is NULL" ); 
	
	iCommandIndex = pLocalStorage->iLastCommandIndex;
	
	IF ((( iCommandIndex < 0 ) || ( iCommandIndex > pLocalStorage->listSize )) , "Last index is missmatch" );					
	
	switch( pLocalStorage->pICS_CommandList[iCommandIndex].iInterfaceType )
	{
		case 1: //SPI	
			
			CALLOC_COPY_ERR( ((__int64**)*pGetValue) , 1 , sizeof(__int64) , &(pLocalStorage->pICS_CommandList[iCommandIndex].ullLastGetValue) ,( 1 * sizeof(__int64)));

			if ( piValueSize )
				*piValueSize = sizeof(__int64);

			if ( piValueLength )
				*piValueLength = 1;
			
			break;
			
		default:	// I/O   
		
			FREE_STDERR_COPY_ERR( ICS_GetPort( pLocalStorage->hDriverHandle , ( pLocalStorage->pICS_CommandList[iCommandIndex].pFeedbackPortNumbersArray[0] ) , &ucSingleByte));
		
			if ( pLocalStorage->pICS_CommandList[iCommandIndex].bFeedbackSwaped )
			{
				ucSingleByte = ~ucSingleByte;
			}
			
			ucSingleByte &= pLocalStorage->pICS_CommandList[iCommandIndex].FeedbackBitsMap;   
			
			switch ( pLocalStorage->pICS_CommandList[iCommandIndex].feedbackMode )    
			{
				case VALUE_MODE_BIT_POSITION :
								
									for( iBitPositionIndex = 0; iBitPositionIndex < 8; iBitPositionIndex++ )
									{
										if ( pLocalStorage->pICS_CommandList[iCommandIndex].FeedbackBitsMap & ( 1 << iBitPositionIndex ) )
											break;
									}
				
									for( ; iBitPositionIndex < ucSingleByte; iBitPositionIndex++ )
									{
										iBitRealPosition++; 
					
										if ( ucSingleByte & ( 1 << iBitPositionIndex ) )
											break;
									}
				
									ucSingleByte = iBitRealPosition;
									
									break;
									
				default:
					
									if (( pLocalStorage->pICS_CommandList[iCommandIndex].FeedbackBitsMap & 0x01 ) == 0 ) 
									{
										ucTempMask = pLocalStorage->pICS_CommandList[iCommandIndex].FeedbackBitsMap;

										// value normalization
										for ( index = 0 ; index < 8 ; index++ )
											if (( ucTempMask & 0x01 ) == 0 )
											{
												ucTempMask >>= 1;
												ucSingleByte >>= 1;
											}
											else
												break;
									}
								
									break;
			}
			
			if ( pLocalStorage->pICS_CommandList[iCommandIndex].FeedbackValueShiftIndex )
			{
				if ( pLocalStorage->pICS_CommandList[iCommandIndex].FeedbackValueShiftIndex > 0 ) 
				{
					ucSingleByte <<= pLocalStorage->pICS_CommandList[iCommandIndex].FeedbackValueShiftIndex;	
				}
				else
				{
					ucSingleByte >>= (0 - pLocalStorage->pICS_CommandList[iCommandIndex].FeedbackValueShiftIndex);	
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
 
void*      ICS_STD_Driver_GetValue_Ex( int hHandle , char *pCommandName , void **pGetValue , int *piValueLength , int *piValueSize, ...  )   
{
	STD_ERROR					StdError					=	{0};
	
	tsLocalStorage				*pLocalStorage				=	NULL;			
	
	unsigned char				ucSingleByte				=	0;
	
	unsigned long long			ullSPIValue					=	0;
	
	int							iCommandIndex				=	0;
	
	int							bCommandFound				=	0;
							
	int							iBitPositionIndex			=	0,
								iBitRealPosition			=	0;
	
	unsigned char 				ucTempMask					=	0;
	
	int							index						=	0;
	
	IF (( hHandle == 0 ) , "Handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle, &pLocalStorage));

	IF (( pGetValue == NULL ) ,"Value pointer is NULL" ); 
	
	for ( iCommandIndex = 0 ; iCommandIndex < pLocalStorage->listSize ; iCommandIndex++ )
		if ((strlen(pLocalStorage->pICS_CommandList[iCommandIndex].szCommandName)) && ( strcmp( pCommandName , pLocalStorage->pICS_CommandList[iCommandIndex].szCommandName ) == 0 ))
		{
			bCommandFound = 1;
			break;
		}
		
	IF (( bCommandFound == 0 ) , "Command was not found.");   
		
	switch( pLocalStorage->pICS_CommandList[iCommandIndex].iInterfaceType )
	{
		case 1: //SPI	

			FREE_STDERR_COPY_ERR( ICS_SPI( pLocalStorage->hDriverHandle , ( pLocalStorage->pICS_CommandList[iCommandIndex].pPortNumbersArray[0] ), pLocalStorage->pICS_CommandList[iCommandIndex].iSPI_EnableBitIndex , pLocalStorage->pICS_CommandList[iCommandIndex].iSPI_EnableBitPolarity , pLocalStorage->pICS_CommandList[iCommandIndex].iSPI_ClockBitIndex , pLocalStorage->pICS_CommandList[iCommandIndex].iSPI_ClockBitPolarity , pLocalStorage->pICS_CommandList[iCommandIndex].iSPI_ClockBitPhase , pLocalStorage->pICS_CommandList[iCommandIndex].iSPI_DataBitIndex , ullSPIValue , pLocalStorage->pICS_CommandList[iCommandIndex].iSPI_WriteBitsCount , pLocalStorage->pICS_CommandList[iCommandIndex].iSPI_ReadBitPort , pLocalStorage->pICS_CommandList[iCommandIndex].iSPI_ReadBitIndex , &(pLocalStorage->pICS_CommandList[iCommandIndex].ullLastGetValue) , pLocalStorage->pICS_CommandList[iCommandIndex].iSPI_ReadBitsCount ));
			
			if ( pLocalStorage->pICS_CommandList[iCommandIndex].iGetDataShiftIndex )
			{
				if ( pLocalStorage->pICS_CommandList[iCommandIndex].iGetDataShiftIndex > 0 ) 
				{
					pLocalStorage->pICS_CommandList[iCommandIndex].ullLastGetValue <<= pLocalStorage->pICS_CommandList[iCommandIndex].iGetDataShiftIndex;	
				}
				else
				{
					pLocalStorage->pICS_CommandList[iCommandIndex].ullLastGetValue >>= (0 - pLocalStorage->pICS_CommandList[iCommandIndex].iGetDataShiftIndex);	
				}
			}
			
			CALLOC_COPY_ERR( ((__int64**)*pGetValue) , 1 , sizeof(__int64) , &(pLocalStorage->pICS_CommandList[iCommandIndex].ullLastGetValue) ,( 1 * sizeof(__int64)));

			if ( piValueSize )
				*piValueSize = sizeof(__int64);

			if ( piValueLength )
				*piValueLength = 1;
			
			break;
			
		default:	// I/O   
		
			FREE_STDERR_COPY_ERR( ICS_GetPort( pLocalStorage->hDriverHandle , ( pLocalStorage->pICS_CommandList[iCommandIndex].pFeedbackPortNumbersArray[0] ) , &ucSingleByte));
		
			if ( pLocalStorage->pICS_CommandList[iCommandIndex].bFeedbackSwaped )
			{
				ucSingleByte = ~ucSingleByte;
			}

			ucSingleByte &= pLocalStorage->pICS_CommandList[iCommandIndex].FeedbackBitsMap;  
			
			switch ( pLocalStorage->pICS_CommandList[iCommandIndex].feedbackMode )    
			{
				case VALUE_MODE_BIT_POSITION :
								
									for( iBitPositionIndex = 0; iBitPositionIndex < 8; iBitPositionIndex++ )
									{
										if ( pLocalStorage->pICS_CommandList[iCommandIndex].FeedbackBitsMap & ( 1 << iBitPositionIndex ) )
											break;
									}
				
									for( ; iBitPositionIndex < ucSingleByte; iBitPositionIndex++ )
									{
										iBitRealPosition++; 
					
										if ( ucSingleByte & ( 1 << iBitPositionIndex ) )
											break;
									}
				
									ucSingleByte = iBitRealPosition;
									
									break;
									
				default:
					
									if (( pLocalStorage->pICS_CommandList[iCommandIndex].FeedbackBitsMap & 0x01 ) == 0 ) 
									{
										ucTempMask = pLocalStorage->pICS_CommandList[iCommandIndex].FeedbackBitsMap;

										// value normalization
										for ( index = 0 ; index < 8 ; index++ )
											if (( ucTempMask & 0x01 ) == 0 )
											{
												ucTempMask >>= 1;
												ucSingleByte >>= 1;
											}
											else
												break;
									}
								
									break;
			}
			
			if ( pLocalStorage->pICS_CommandList[iCommandIndex].iGetDataShiftIndex )
			{
				if ( pLocalStorage->pICS_CommandList[iCommandIndex].iGetDataShiftIndex > 0 ) 
				{
					ucSingleByte <<= pLocalStorage->pICS_CommandList[iCommandIndex].iGetDataShiftIndex;	
				}
				else
				{
					ucSingleByte >>= (0 - pLocalStorage->pICS_CommandList[iCommandIndex].iGetDataShiftIndex);	
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

void*      ICS_STD_Driver_Check_Connection( int hHandle , char *pCommandName , int *piStatus )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	int							iCommandIndex										=	0,
								bCommandFound										=	0; 
									
	
	IF (( hHandle == 0 ) , "Handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));  
	
	IF (( piStatus == 0 ) , "Status parameter can't be NULL." );
	IF (( pCommandName == NULL ) , "Command can't be NULL." );
	
	*piStatus = 0;
	
	for ( iCommandIndex = 0 ; iCommandIndex < pLocalStorage->listSize ; iCommandIndex++ )
		if ((strlen(pLocalStorage->pICS_CommandList[iCommandIndex].szCommandName)) && ( strcmp( pCommandName , pLocalStorage->pICS_CommandList[iCommandIndex].szCommandName ) == 0 ))
		{
			bCommandFound = 1;
			break;
		}
		
	IF (( bCommandFound == 0 ) , "Command was not found.");   
	
Error:

	if ( piStatus )
		*piStatus = bCommandFound;
	
	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

void*      ICS_STD_Driver_Unplug( int hHandle , double lfTimeOut )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
	
	IF (( hHandle == 0 ) , "Handle can't be zero." );

	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage )); 
	
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}


void*      ICS_STD_Driver_PlugIn( int hHandle , double lfTimeOut )
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

void*      ICS_STD_Driver_EnablePlugInAutomatically( int hHandle , int bEnable )
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

void*      ICS_STD_Driver_InstallReceiveCallbackFunction( int hHandle , tfStdReceiveCallback pfFunction , void *pData )
{
	STD_ERROR					StdError											=	{0};
			
	RETURN_STDERR_POINTER;
}


void*      ICS_STD_Driver_LoadConfigFile( int hHandle , char *pFileName )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			

	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};
	
	int							hFileHandle											=	0;
				
	int							iIndex												=	0,  
								iCount												=	0;
	
	char						szFormatedLog[LOW_STRING]							=	{0};

	int							iFileSize											=	0;
	
	int							iPortNumber											=	0;
	
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage )); 
	
	IF (( pFileName == NULL ) , "File name is empty." );
	IF (( FileExists( pFileName , &iFileSize ) == 0 )  , "File was not found." );  
	
	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks); 
	
	if ( pLocalStorage->ptCallbacks )
	{
		sprintf( szFormatedLog , "%s :: Loading configuration %%s" ,  pLocalStorage->ptCallbacks->szName );
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	}
	
	hFileHandle = OpenFile ( pFileName , VAL_READ_ONLY, VAL_TRUNCATE, VAL_BINARY);
	
	FREE_CALLOC_ERR( pLocalStorage->pICS_CommandList , ((iFileSize / sizeof(tsSingleDeviceItem))+1) , sizeof(tsSingleDeviceItem));
	
	do
	{
		iCount = ReadFile ( hFileHandle , pLocalStorage->pICS_CommandList[iIndex].max_size , sizeof(tsSingleDeviceItem) );
		
		if ( iCount != sizeof(tsSingleDeviceItem) )
			break;
		
		if ( pLocalStorage->pICS_CommandList[iIndex].ulSignatureID != 0 ) 
			if ( pLocalStorage->pICS_CommandList[iIndex].ulSignatureID != DEVICE_ID )
				break;
		
		iPortNumber = pLocalStorage->pICS_CommandList[iIndex].pPortNumbersArray[0];
		
		pLocalStorage->viPortsDefaults[iPortNumber] |= ( ~pLocalStorage->pICS_CommandList[iIndex].UseBitsMap | pLocalStorage->pICS_CommandList[iIndex].DefValue );
		
		iIndex++;
		
	} while(1);
	
	pLocalStorage->listSize = iIndex;

Error:

	if ( hFileHandle )
		CloseFile (hFileHandle);
	
	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

void*      ICS_STD_Driver_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			

	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};

	int							iIndex												=	0,  
								iBaseSize											=	0, 
								iCount												=	0;

	char						szFormatedLog[LOW_STRING]							=	{0};
	
	int							iPortNumber											=	0;
	
	IF (( hHandle == 0 ) , "handle can't be zero." ); 
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));   
	
	IF (( pBuffer == NULL ) , "Buffer is empty." );
	
	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks); 
	
	sprintf( szFormatedLog , "%s :: Loading configuration %%s" ,  pLocalStorage->ptCallbacks->szName );
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
		
	FREE_CALLOC_ERR( pLocalStorage->pICS_CommandList , 256 , sizeof(tsSingleDeviceItem));
	
	iCount = iBufferSize;
	iBaseSize = sizeof(tsSingleDeviceItem); 
	
	for ( iIndex = 0 ; iIndex < 256; iIndex++ )
	{
		if ( iCount < iBaseSize )   
			break;
		
		memcpy( pLocalStorage->pICS_CommandList[iIndex].max_size , pBuffer , iBaseSize );
		
		if ( pLocalStorage->pICS_CommandList[iIndex].ulSignatureID != 0 ) 
			if ( pLocalStorage->pICS_CommandList[iIndex].ulSignatureID != DEVICE_ID )
				break;
				
		iPortNumber = pLocalStorage->pICS_CommandList[iIndex].pPortNumbersArray[0];
		
		pLocalStorage->viPortsDefaults[iPortNumber] |= ( ~pLocalStorage->pICS_CommandList[iIndex].UseBitsMap | pLocalStorage->pICS_CommandList[iIndex].DefValue );
		
		pBuffer += iBaseSize;
		iCount -= iBaseSize; 
	}
	
	pLocalStorage->listSize = iIndex;

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}



void*			ICS_STD_Driver_Get_Commands_List( int hHandle , char ***pCommandsList , int *piNumberOfCommands )     
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			

	int							iIndex												=	0;

	int							iNumberOfCommands									=	0;

	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));    
	
	IF (( pCommandsList == NULL ) , "Pointer to List is empty." );
	
	CALLOC_ERR( (*pCommandsList) , pLocalStorage->listSize+1 , sizeof(char*));
	
	for ( iIndex = 0 ; iIndex < pLocalStorage->listSize; iIndex++ )
	{
		CALLOC_COPY_STRING_ERR( (*pCommandsList)[iIndex] , pLocalStorage->pICS_CommandList[iIndex].szCommandName );
		
		if ( strlen((*pCommandsList)[iIndex]))
			iNumberOfCommands++;
	}

Error:
	
	if ( piNumberOfCommands )
		*piNumberOfCommands = iNumberOfCommands;

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}


