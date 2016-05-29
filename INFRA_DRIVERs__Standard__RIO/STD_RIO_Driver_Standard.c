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
#include "RIO_Driver.h"
#include "STD_RIO_Driver_Standard.h"

//==============================================================================
// Constants

#define							DEVICE_ID								        STANDARD_RIO_DRIVER_DEVICE_ID  
#define							TEMPERATURE_TRIGGER_COMMAND_NAME				"GET_START_TEST_TEMP_TRIGGER"
#define							VIBRATION_TRIGGER_COMMAND_NAME					"GET_START_TEST_VIBR_TRIGGER"

//==============================================================================
// Types

typedef enum 							 
{
	
 	DEV_VALUE_TYPE_NONE,
    DEV_VALUE_TYPE_CHAR,
    DEV_VALUE_TYPE_SHORT,
    DEV_VALUE_TYPE_INT,
    DEV_VALUE_TYPE_LONG,
    DEV_VALUE_TYPE_FLOAT,
    DEV_VALUE_TYPE_DOUBLE,
    DEV_VALUE_TYPE_BUFFER,
    DEV_VALUE_TYPE_SENTINEL
				
} teValueTypes; 

typedef union
{
	 char						buffer[8];
	 
	 char						charValue;
	 short						shortValue;
	 int						intValue;
	 float						floatValue;
	 double						doubleValue;
	
}tsData;


typedef struct
{
	int									hDriverHandle;  

	tsSTD_RIO_CommandItem				*pRIO_CommandList;
	int									listSize;
	
	tsSTD_RIO_CommandItem				LastCommand;
	
	tsData 								LastInput;
	
	tsData 								LastOutput;
	
	int									iLastCommandIndex;
	
	char								*pszAddress;
	
	tsSTD_CallBackSet					*ptCallbacks;

	STD_ERROR							*pLastError; 
	
}tsLocalStorage; 

extern tsSTD_RIO_CommandItem	tSTD_RIO_CommandList[]; 

//==============================================================================
// Static global variables

static int						g_Temperature_Trigger_Command_ID					=	0;
static int						g_Vibration_Trigger_Command_ID						=	0;  

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

/***** ***** ***** ***** ***** ***** ***** ***** RIO STD Driver Get Error Text Message   ***** ***** ***** ***** ***** ***** ***** *****/   
void*  	RIO_STD_Driver_GetErrorTextMessage ( int hHandle , int iError , char *pErrorMessage )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalHandle								=	NULL;			
	
	IF (( hHandle == 0 ) , "Handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalHandle ));  
	
	RIO_Get_Error_Text_Message_Instrument_Handle( pLocalHandle->hDriverHandle , iError , pErrorMessage );
	
Error:
	
	if ( hHandle )
		CmtReleaseTSVPtr (hHandle);
	
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** RIO Copy STD Call Back Set   ***** ***** ***** ***** ***** ***** ***** *****/ 
void*	RIO_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet )
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

/***** ***** ***** ***** ***** ***** ***** ***** RIO STD Driver Get Low Level Handle   ***** ***** ***** ***** ***** ***** ***** *****/ 
void*	RIO_STD_Driver_GetLowLevelHandle( int hHandle , int *phLowLevelHandle )
{
	STD_ERROR						    StdError									=	{0};
	
	tsLocalStorage						*pLocalHandle								=	NULL;			
	
	IF (( hHandle == 0 ) , "Handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalHandle ));   
	
	FREE_STDERR_COPY_ERR( RIO_Get_Low_Level_Handle( pLocalHandle->hDriverHandle , phLowLevelHandle ));	
	
Error:
	
	if ( hHandle )
		CmtReleaseTSVPtr (hHandle);
	
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** RIO STD Driver Get Connection Address   ***** ***** ***** ***** ***** ***** ***** *****/  
void*	RIO_STD_Driver_GetConnectionAddress( int hHandle , char **pszAddress )
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

/***** ***** ***** ***** ***** ***** ***** ***** RIO STD Driver Equipment Info   ***** ***** ***** ***** ***** ***** ***** *****/  
void* RIO_STD_Driver_Equipment_Info ( int hLowLevelHandle , char *pAddress , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware )
{
	STD_ERROR				StdError								=	{0};
	
	int						hInstrumentHandle						=	0;
	
	int						hConnectionHandle					    =	0; 
	
	
	if ( hLowLevelHandle < 0 )
	{
		 hInstrumentHandle = -1;
		
		 FREE_STDERR_COPY_ERR( RIO_Init ( 0 , pAddress , &hInstrumentHandle )); 
		 
		 IF (( hInstrumentHandle == 0 ) , "Handle can't be zero." ); 
		 
		 FREE_STDERR_COPY_ERR( RIO_Get_Low_Level_Handle( hInstrumentHandle , &hConnectionHandle )); 
		 
		 IF (( hInstrumentHandle < 0 ) , "Handle can't be negative." ); 
		 
	}
	else
	{
		hConnectionHandle = hLowLevelHandle;
	}
	
	FREE_STDERR_COPY_ERR( RIO_Get_Device_Info_Low_Level_Handle ( hLowLevelHandle, pModel , pFirmware ));
	
	
	Error:
	
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** RIO STD Driver Equipment IsSupported   ***** ***** ***** ***** ***** ***** ***** *****/ 
void*	RIO_STD_Driver_Equipment_IsSupported ( int hLowLevelHandle , char *pAddress , char *pID_String , char *pIdentificationText , int *pbSupporting , void **pLocalData )
{
	STD_ERROR				StdError								=	{0};  
	
	int						hInstrumentHandle						=	0;
	
	int						hConnectionHandle					    =	0; 
    
	int						iIndex									=	0;
							
	int						bSupport								=	0; 
	
	char                    *pModel     							=   NULL;
	
	char                    *pFirmware						     	=   NULL; 
	
	tsLocalStorage			*pLocalHandle							=	{0}; 
	
	char                    vszSupportdedModels[][16]				=	{{"RIO47202"}}; 
	
	
	
	if ( hLowLevelHandle < 0 )
	{
		 hInstrumentHandle = -1;
		
		 FREE_STDERR_COPY_ERR( RIO_Init ( 0 , pAddress , &hInstrumentHandle )); 
		 
		 IF (( hInstrumentHandle == 0 ) , "Handle can't be zero." ); 
		 
		 FREE_STDERR_COPY_ERR( RIO_Get_Low_Level_Handle( hInstrumentHandle , &hConnectionHandle )); 
		 
		 IF (( hConnectionHandle < 0 ) , "Handle can't be negative." ); 
		 
		 hLowLevelHandle  = hConnectionHandle;
		 
	}
	else
	{
		hConnectionHandle = hLowLevelHandle;
	}
	
	FREE_STDERR_COPY_ERR( RIO_Get_Device_Info_Low_Level_Handle ( hLowLevelHandle, &pModel , &pFirmware ));
	
	if ( pLocalData )
	{
		pLocalHandle = (tsLocalStorage*)(*pLocalData); 
		 
	}
	
	do
	{
		if ( strstr( pModel , "RIO" ) == NULL )
		{
			break;
		}
	
		for ( iIndex = 0; iIndex < (sizeof(vszSupportdedModels) / sizeof(vszSupportdedModels[0])); iIndex++ )
		{	
			if ( strstr( pModel , vszSupportdedModels[iIndex]) )
			{		
				if ( pLocalHandle )
				{
					pLocalHandle->pRIO_CommandList = malloc( GetCommandItemListSize() * sizeof(tsSTD_RIO_CommandItem));
					
					memcpy( pLocalHandle->pRIO_CommandList, tSTD_RIO_CommandList, GetCommandItemListSize() * sizeof(tsSTD_RIO_CommandItem));
					
					pLocalHandle->listSize = GetCommandItemListSize();
				}
				
				bSupport = 1; 
				
				break;
			}
		}
					
	} while(0);
	
	if (pbSupporting)
	{
		*pbSupporting = bSupport;
	}
	
	
	Error:
	
	RETURN_STDERR_POINTER;		
}

/***** ***** ***** ***** ***** ***** ***** ***** RIO STD Driver Init   ***** ***** ***** ***** ***** ***** ***** *****/  
void*  RIO_STD_Driver_Init( char* szRsrcAddress , int *phInstrumentHandle )
{
	STD_ERROR						StdError										=	{0};
	
	tsLocalStorage					*pLocalHandle									=	NULL;
	
	CmtTSVHandle 					handle											=	0;
	
	int								supported										=	0;
	
	tsSTD_CallBackSet				tSTD_CallBackSet								=	{0};
	
	int                             hLowLevelHandle									=	0; 
	
	int								iTempTrigLineNumber								=	-1;
	
	int								iVibTrigLineNumber								=	-1;  
	
	int								iMatchIndex										=	-1;
	
	int								iPortNumber										=	-1; 
	
	tsSTD_RIO_CommandItem           sCommandItem            						=	{0};
	
	
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
	

	STDF_UPDATE_CALLBACK_DATA(pLocalHandle->ptCallbacks); 
	
	FREE_STDERR_COPY_ERR( RIO_Init ( 0 , szRsrcAddress , &(pLocalHandle->hDriverHandle) ));  
	
	CALLOC_COPY_STRING( pLocalHandle->pszAddress , szRsrcAddress );
	
	FREE_STDERR_COPY_ERR( RIO_Get_Low_Level_Handle( pLocalHandle->hDriverHandle , &hLowLevelHandle ));  
	
	FREE_STDERR_COPY_ERR( RIO_STD_Driver_Equipment_IsSupported ( hLowLevelHandle , szRsrcAddress , NULL , NULL , &supported , &pLocalHandle )); 

	IF (( supported == 0 ) , "This device has not supported." );
	
	STDF_CONFIG_VALUE("Temperature_Trigger_Line", VAL_INTEGER , 1 , iTempTrigLineNumber , iTempTrigLineNumber );  
	
	STDF_CONFIG_VALUE("Vibration_Trigger_Line", VAL_INTEGER , 1 , iVibTrigLineNumber , iVibTrigLineNumber );    
	
	if( iTempTrigLineNumber > -1)
	{
		for(int iIndex = 0; iIndex < pLocalHandle->listSize; iIndex++)
		{
			sCommandItem = pLocalHandle->pRIO_CommandList[iIndex];
			
			iMatchIndex = FindPattern (sCommandItem.szCommandName, 0, -1, "GET_DI_LINE", 1, 0);
			iPortNumber = sCommandItem.usPortNumber;
		
			if( iMatchIndex > -1 &&  iPortNumber == iTempTrigLineNumber )
			{
				g_Temperature_Trigger_Command_ID = sCommandItem.usCommandId;
				
				break;
			}
		}
	}
	
	if( iVibTrigLineNumber > -1)
	{
		for(int iIndex = 0; iIndex < pLocalHandle->listSize; iIndex++)
		{
			sCommandItem = pLocalHandle->pRIO_CommandList[iIndex];
			
			iMatchIndex = FindPattern (sCommandItem.szCommandName, 0, -1, "GET_DI_LINE", 1, 0);
			iPortNumber = sCommandItem.usPortNumber;
		
			if( iMatchIndex > -1 && iPortNumber == iVibTrigLineNumber )
			{
				g_Vibration_Trigger_Command_ID = sCommandItem.usCommandId;
				
				break;
			}
		}
	}
	
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** ***** ***** RIO STD Driver Close   ***** ***** ***** ***** ***** ***** ***** *****/ 
void*  RIO_STD_Driver_Close( int *pHandle )
{
	STD_ERROR					StdError					=	{0};
	
	tsLocalStorage				*pLocalStorage				=	NULL;			
	
	int 						Handle						=	0;
									
	IF (( pHandle == NULL ) , "Handle can't be NULL" );
	
	Handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr (Handle, &pLocalStorage));
	
	if ( pLocalStorage->hDriverHandle )
	{
		FREE_STDERR_COPY_ERR( RIO_Close( &(pLocalStorage->hDriverHandle) ));
	}
	
Error:
	
	if (Handle)
	{
		FREE(pLocalStorage->pszAddress);	
		FREE(pLocalStorage->pLastError);
		FREE(pLocalStorage->pRIO_CommandList);
		
		CmtReleaseTSVPtr (Handle);
		CmtDiscardTSV (Handle);
	}
	
	*pHandle = 0;
	
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** RIO STD_Driver Reset   ***** ***** ***** ***** ***** ***** ***** *****/  
void* RIO_STD_Driver_Reset(int hHandle)
{
	STD_ERROR					StdError						=	{0};
	
	tsLocalStorage				*pLocalStorage					=	NULL;   
	
	
	
	IF (( hHandle == 0 ) , "Handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle, &pLocalStorage));
	
	
	FREE_STDERR_COPY_ERR( RIO_Reset ( pLocalStorage->hDriverHandle ));
	
	
Error:
	
	if (hHandle)
		CmtReleaseTSVPtr (hHandle);
	
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** RIO STD Driver SetValue Internal   ***** ***** ***** ***** ***** ***** ***** *****/ 
void*	RIO_STD_Driver_SetValue_Internal( tsLocalStorage *pLocalStorage , char *pCommandName , void *pValue , int iValueLength , int iValueSize ,unsigned char *pIsCommandSuccessfull) 
{
	STD_ERROR					StdError					=	{0}; 
	
	int							iCommandIndex					=	0;
	
	int							iCommandFound					=	0; 
	
	char						szSplitCommand[LOW_STRING]		=	{0}; 
	
	
	
	
	for ( iCommandIndex = 0 ; iCommandIndex < pLocalStorage->listSize ; iCommandIndex++ )
	{
		if ((strlen(pLocalStorage->pRIO_CommandList[iCommandIndex].szCommandName)) && ( strcmp( pCommandName , pLocalStorage->pRIO_CommandList[iCommandIndex].szCommandName ) == 0 ))
		{
			iCommandFound = 1;
			
			break;
		}
		else
		{
			if((strcmp( TEMPERATURE_TRIGGER_COMMAND_NAME , pCommandName ) == 0 ))
			{
				if(pLocalStorage->pRIO_CommandList[iCommandIndex].usCommandId == g_Temperature_Trigger_Command_ID)
				{
					iCommandFound = 1;
			
					break;
				}
			}
			else
			{
				if( strcmp( VIBRATION_TRIGGER_COMMAND_NAME , pCommandName ) == 0 )
				{
					if(pLocalStorage->pRIO_CommandList[iCommandIndex].usCommandId == g_Vibration_Trigger_Command_ID)
					{
						iCommandFound = 1;
			
						break;
					}
				}
			}
		}
	}
		
	if ( iCommandFound == 0 )
	{
		sprintf( szSplitCommand , "%s_" , pCommandName );
		
		for ( iCommandIndex = 0 ; iCommandIndex < pLocalStorage->listSize ; iCommandIndex++ )
		{
			if ((strlen(pLocalStorage->pRIO_CommandList[iCommandIndex].szCommandName)) && ( strstr( pLocalStorage->pRIO_CommandList[iCommandIndex].szCommandName , szSplitCommand )))
			{
				iCommandFound++;
			}
		}
	}
	
	IF (( iCommandFound == 0 ) , "Command was not found.");   
		
	pLocalStorage->iLastCommandIndex = iCommandIndex;
	
	pLocalStorage->LastCommand = pLocalStorage->pRIO_CommandList[pLocalStorage->iLastCommandIndex]; 
	
	calloc(&pLocalStorage->LastInput.buffer, sizeof(double)); 
	
	memcpy(&pLocalStorage->LastInput.buffer, pValue, iValueSize);
	
	calloc(pLocalStorage->LastOutput.buffer, sizeof(double));
	
	if( pLocalStorage->LastCommand.usCommandId >= 0 &&  pLocalStorage->LastCommand.usCommandId <= 99 )// Get AI Line Value Commands
	{
		FREE_STDERR_COPY_ERR( RIO_Get_AI_Line_Value ( pLocalStorage->hDriverHandle, pLocalStorage->LastCommand.usPortNumber, &pLocalStorage->LastOutput.doubleValue ));
	}
	else
	{
		if( pLocalStorage->LastCommand.usCommandId >= 100 &&  pLocalStorage->LastCommand.usCommandId <= 199 )// Get AO Line Value Commands
		{
		 	FREE_STDERR_COPY_ERR( RIO_Get_AO_Line_Value  ( pLocalStorage->hDriverHandle, pLocalStorage->LastCommand.usPortNumber, &pLocalStorage->LastOutput.doubleValue ));
		}
		else
		{
			if( pLocalStorage->LastCommand.usCommandId >= 200 &&  pLocalStorage->LastCommand.usCommandId <= 299 )// Get DI Line Value Commands
			{
			 	FREE_STDERR_COPY_ERR( RIO_Get_DI_Line_Value  ( pLocalStorage->hDriverHandle, pLocalStorage->LastCommand.usPortNumber, &pLocalStorage->LastOutput.charValue));
			}
			else
			{
				if( pLocalStorage->LastCommand.usCommandId >= 300 &&  pLocalStorage->LastCommand.usCommandId <= 399 )// Get DO Line Value Commands
				{
					FREE_STDERR_COPY_ERR( RIO_Get_DO_Line_Value  ( pLocalStorage->hDriverHandle, pLocalStorage->LastCommand.usPortNumber, &pLocalStorage->LastOutput.charValue));	
				}
				else
				{
					if( pLocalStorage->LastCommand.usCommandId >= 400 &&  pLocalStorage->LastCommand.usCommandId <= 499 )// Set AO Line Value Commands
					{
						FREE_STDERR_COPY_ERR( RIO_Set_AO_Line_Value  ( pLocalStorage->hDriverHandle, pLocalStorage->LastCommand.usPortNumber, pLocalStorage->LastInput.charValue ));	
					}
					else
					{
						if( pLocalStorage->LastCommand.usCommandId >= 500 &&  pLocalStorage->LastCommand.usCommandId <= 599 )	// Set DO Line Value Commands
						{
							FREE_STDERR_COPY_ERR( RIO_Set_DO_Line_Value  ( pLocalStorage->hDriverHandle, pLocalStorage->LastCommand.usPortNumber, pLocalStorage->LastInput.charValue ));		
						}
					    else
						{
							if( pLocalStorage->LastCommand.usCommandId >= 600 &&  pLocalStorage->LastCommand.usCommandId <= 699 )// Get AO Line Config Commands
							{
								FREE_STDERR_COPY_ERR( RIO_Get_AI_Line_Config ( pLocalStorage->hDriverHandle, pLocalStorage->LastCommand.usPortNumber, &pLocalStorage->LastOutput.charValue ));	
							}
							else
							{
								if( pLocalStorage->LastCommand.usCommandId >= 700 &&  pLocalStorage->LastCommand.usCommandId <= 799 )// Get DO Line Config Commands
								{
									FREE_STDERR_COPY_ERR( RIO_Get_DI_Line_Config ( pLocalStorage->hDriverHandle, pLocalStorage->LastCommand.usPortNumber, &pLocalStorage->LastOutput.charValue ));	
								}
								else
								{
									if( pLocalStorage->LastCommand.usCommandId >= 800 &&  pLocalStorage->LastCommand.usCommandId <= 899 )// Set AO Line Config Commands
									{
										FREE_STDERR_COPY_ERR( RIO_Set_AI_Line_Config ( pLocalStorage->hDriverHandle, pLocalStorage->LastCommand.usPortNumber, pLocalStorage->LastInput.buffer[0], pLocalStorage->LastInput.buffer[1] )); 
									}
									else
									{
										if( pLocalStorage->LastCommand.usCommandId >= 900 &&  pLocalStorage->LastCommand.usCommandId <= 999 )// Set DO Line Config Commands
										{
											FREE_STDERR_COPY_ERR( RIO_Set_DI_Line_Config ( pLocalStorage->hDriverHandle, pLocalStorage->LastCommand.usPortNumber, pLocalStorage->LastInput.charValue ));
										}
										else
										{
											if( pLocalStorage->LastCommand.usCommandId == 1000 )// Reset
											{
												FREE_STDERR_COPY_ERR( RIO_Reset ( pLocalStorage->hDriverHandle ));	
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	} 
	
	*pIsCommandSuccessfull = IsCommandSuccessful();
	
	
Error:
	
	
	RETURN_STDERR_POINTER
}

/***** ***** ***** ***** ***** ***** ***** ***** RIO STD Driver Set Value   ***** ***** ***** ***** ***** ***** ***** *****/  
void*	RIO_STD_Driver_SetValue( int hHandle , char *pCommandName , void *pValue , int iValueLength , int iValueSize , ...  )
{
	STD_ERROR					StdError					    	=	{0};
	
	tsLocalStorage				*pLocalStorage					    =	NULL; 
	
	unsigned char               ucIsCommandSuccessful				=	0;
	
	
	
	IF (( hHandle == 0 ) , "Handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle, &pLocalStorage));

	IF (( pValue == NULL ) ,"Value pointer is NULL" );  
	
	FREE_STDERR_COPY_ERR( RIO_STD_Driver_SetValue_Internal( pLocalStorage, pCommandName, pValue, iValueLength, iValueSize, &ucIsCommandSuccessful )); 
	
	
	
	Error:
	
	if (hHandle)
		CmtReleaseTSVPtr (hHandle);
	
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** RIO STD Driver GetValue Internal   ***** ***** ***** ***** ***** ***** ***** *****/    
void*	RIO_STD_Driver_GetValue_Internal( tsLocalStorage	*pLocalStorage, void **pGetValue , int *piValueLength , int *piValueSize, ...  )    
{
	STD_ERROR					StdError					=	{0};
	
	int							iValueSize					=	0;  
	
	

	if( pLocalStorage->LastCommand.usCommandId >= 0 &&  pLocalStorage->LastCommand.usCommandId <= 99 )// Get AI Line Value Commands
	{
		CALLOC_COPY_ERR( ((double**)*pGetValue) , 1 , sizeof(double) , &pLocalStorage->LastOutput.charValue ,( 1 * sizeof(double)));  
	
		iValueSize = sizeof(double);
	}
	else
	{
		if( pLocalStorage->LastCommand.usCommandId >= 100 &&  pLocalStorage->LastCommand.usCommandId <= 199 )// Get AO Line Value Commands
		{
		 	CALLOC_COPY_ERR( ((double**)*pGetValue) , 1 , sizeof(double) , &pLocalStorage->LastOutput.charValue ,( 1 * sizeof(double))); 
			
			iValueSize = sizeof(double);
		}
		else
		{
			if( pLocalStorage->LastCommand.usCommandId >= 200 &&  pLocalStorage->LastCommand.usCommandId <= 299 )// Get DI Line Value Commands
			{
			 	CALLOC_COPY_ERR( ((char**)*pGetValue) , 1 , sizeof(char) , &pLocalStorage->LastOutput.charValue ,( 1 * sizeof(char))); 
				
				iValueSize = sizeof(char);	
			}
			else
			{
				if( pLocalStorage->LastCommand.usCommandId >= 300 &&  pLocalStorage->LastCommand.usCommandId <= 399 )// Get DO Line Value Commands
				{
					CALLOC_COPY_ERR( ((char**)*pGetValue) , 1 , sizeof(char) , &pLocalStorage->LastOutput.charValue ,( 1 * sizeof(char)));
			
					iValueSize = sizeof(char);	
				}
				else
				{
					if( pLocalStorage->LastCommand.usCommandId >= 600 &&  pLocalStorage->LastCommand.usCommandId <= 699 )// Get AI Line Config Commands
					{
						CALLOC_COPY_ERR( ((char**)*pGetValue) , 1 , sizeof(char) , &pLocalStorage->LastOutput.charValue ,( 1 * sizeof(char))); 
				
						iValueSize = sizeof(char);	
					} 
					else
					{
						if( pLocalStorage->LastCommand.usCommandId >= 700 &&  pLocalStorage->LastCommand.usCommandId <= 799 )// Get DI Line Config Commands
						{
							CALLOC_COPY_ERR( ((char**)*pGetValue) , 1 , sizeof(char) , &pLocalStorage->LastOutput.charValue ,( 1 * sizeof(char))); 
				
							iValueSize = sizeof(char);	
						} 
					}
				}
			}
		}
	} 
	
	
	if ( piValueLength )
		*piValueLength = 1;
	
	if ( piValueSize )
		*piValueSize = iValueSize;
	
Error:
	
	
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** RIO STD Driver Get Value   ***** ***** ***** ***** ***** ***** ***** *****/  
void*	RIO_STD_Driver_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize, ...  )    
{
	STD_ERROR					StdError					=	{0};
	
	tsLocalStorage				*pLocalStorage				=	NULL; 
		
	int							iCommandIndex				=	0;
	
	
	
	IF (( hHandle == 0 ) , "Handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle, &pLocalStorage));

	IF (( pGetValue == NULL ), "Get Value pointer is NULL" ); 
	
	iCommandIndex = pLocalStorage->iLastCommandIndex;
	
	IF ((( iCommandIndex < 0 ) || ( iCommandIndex > pLocalStorage->listSize )) , "Last index is missmatch" );
	
	strcpy(pGetCommandName, &pLocalStorage->LastCommand.szCommandName);
	
	
	FREE_STDERR_COPY_ERR( RIO_STD_Driver_GetValue_Internal( pLocalStorage, pGetValue , piValueLength , piValueSize )); 
	
	
	Error:
	
	if (hHandle)
		CmtReleaseTSVPtr (hHandle);
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** ***** ***** RIO STD Driver Get Value Ex  ***** ***** ***** ***** ***** ***** ***** *****/  
void*	RIO_STD_Driver_GetValue_Ex( int hHandle , char *pCommandName , void **pGetValue , int *piValueLength , int *piValueSize, ...  )    
{
	STD_ERROR					StdError					        =	{0};
	
	tsLocalStorage				*pLocalStorage				        =	NULL; 
		
	int							iValueLength			        	=	0;
	
	int							iValueSize				            =	0;
	
	unsigned char               ucIsCommandSuccessful				=	0;  
	
	void                        *pValue								= 	NULL; 
	
	
	
	IF (( hHandle == 0 ) , "Handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle, &pLocalStorage));

	IF (( pGetValue == NULL ), "Get Value pointer is NULL" ); 
	
	iValueLength = *piValueLength;
	iValueSize = *piValueSize;
	pValue = calloc(1, sizeof(tsData));
	
	FREE_STDERR_COPY_ERR( RIO_STD_Driver_SetValue_Internal( pLocalStorage, pCommandName, pValue, iValueLength, iValueSize, &ucIsCommandSuccessful )); 
	
	IF (( ucIsCommandSuccessful != 1 ), "Error to write command" ); 
	
	FREE_STDERR_COPY_ERR( RIO_STD_Driver_GetValue_Internal( pLocalStorage, pGetValue , piValueLength , piValueSize ));  
	
	
	Error:
	
	if (hHandle)
		CmtReleaseTSVPtr (hHandle);
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** ***** ***** RIO STD Driver Check Connection  ***** ***** ***** ***** ***** ***** ***** *****/    
void*	RIO_STD_Driver_Check_Connection( int hHandle , char *pCommandName , int *piStatus )
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
		if ((strlen(pLocalStorage->pRIO_CommandList[iCommandIndex].szCommandName)) && ( strcmp( pCommandName , pLocalStorage->pRIO_CommandList[iCommandIndex].szCommandName ) == 0 ))
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

/***** ***** ***** ***** ***** ***** ***** ***** RIO STD Driver Unplug  ***** ***** ***** ***** ***** ***** ***** *****/    
void*	RIO_STD_Driver_Unplug( int hHandle , double lfTimeOut )
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

/***** ***** ***** ***** ***** ***** ***** ***** RIO STD Driver Plug In  ***** ***** ***** ***** ***** ***** ***** *****/ 
void*	RIO_STD_Driver_PlugIn( int hHandle , double lfTimeOut )
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

/***** ***** ***** ***** ***** ***** ***** ***** RIO STD Driver EnablePlugInAutomatically ***** ***** ***** ***** ***** ***** ***** *****/  
void*	RIO_STD_Driver_EnablePlugInAutomatically( int hHandle , int bEnable )
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

/***** ***** ***** ***** ***** ***** ***** ***** RIO STD Driver Install Receive Callback Function ***** ***** ***** ***** ***** ***** ***** *****/ 
void*	RIO_STD_Driver_InstallReceiveCallbackFunction( int hHandle , tfStdReceiveCallback pfFunction , void *pData )
{
	STD_ERROR					StdError											=	{0};
			
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** RIO STD Driver Load Config File ***** ***** ***** ***** ***** ***** ***** *****/  
void*	RIO_STD_Driver_LoadConfigFile( int hHandle , char *pFileName )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;
	
	int							hFileHandle											=	0;  

	int							iFileSize											=	0;
	
	
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage )); 
	
	IF (( pFileName == NULL ) , "File name is empty." );
	IF (( FileExists( pFileName , &iFileSize ) == 0 )  , "File was not found." ); 

Error:

	if ( hFileHandle )
		CloseFile (hFileHandle);
	
	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** RIO STD Driver LoadConfigBuffer ***** ***** ***** ***** ***** ***** ***** *****/ 
void*	RIO_STD_Driver_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;
	 
	
	
	IF (( hHandle == 0 ) , "handle can't be zero." ); 
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));   
	
	IF (( pBuffer == NULL ) , "Buffer is empty." );  
	

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** RIO STD Driver Get Commands List ***** ***** ***** ***** ***** ***** ***** *****/ 
void*	RIO_STD_Driver_Get_Commands_List( int hHandle , char ***pCommandsList , int *piNumberOfCommands )     
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
		CALLOC_COPY_STRING_ERR( (*pCommandsList)[iIndex] , pLocalStorage->pRIO_CommandList[iIndex].szCommandName );
		
		if ( piNumberOfCommands )
			(*piNumberOfCommands)++;
	}

Error: 

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}
