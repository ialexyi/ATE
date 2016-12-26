#include "Windows.h"
#include <userint.h>
#include <ansi_c.h>

#include <utility.h>
#include "GlobalDefines.h"
#include "toolbox.h"
#include "MAIN_DriversManager.h"
#include "MathLibrary.h"

//==============================================================================
// Include files

//==============================================================================
// Static global variables

CmtTSVHandle			gGlobalDriverCommonHandle		=	0;

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions
void		DRIVER_MANAGER_RecursiveDestroy( tsSingleListItem	*pCurrentItem );

//==============================================================================
// DLL main entry-point functions

int __stdcall DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	tsGlobalHandle				*pGlobalStore					=	NULL;
	int							iIndex							=	0;
	
	switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            if (InitCVIRTE (hinstDLL, 0, 0) == 0)
                return 0;     /* out of memory */
			
			CmtNewTSV ( sizeof(tsGlobalHandle) , &gGlobalDriverCommonHandle );
			
            break;
			
        case DLL_PROCESS_DETACH:
			
			if ( gGlobalDriverCommonHandle )
			{
				if ( CmtGetTSVPtr ( gGlobalDriverCommonHandle , &pGlobalStore ) == 0 )
				{
					if ( pGlobalStore->pSTD_CallBackSet )
					{
						for( iIndex = 0; iIndex < pGlobalStore->iNumberOfCallBackSet; iIndex++ )
						{
							FREE( pGlobalStore->pSTD_CallBackSet[iIndex].pCheckForBreakCallbackData );
							FREE( pGlobalStore->pSTD_CallBackSet[iIndex].pCommentCallbackData );
							FREE( pGlobalStore->pSTD_CallBackSet[iIndex].pConfigValueCallbackData );
							FREE( pGlobalStore->pSTD_CallBackSet[iIndex].pFileCallbackData );
						}
					} 

					FREE( pGlobalStore->pSTD_CallBackSet ); 
					
					DRIVER_MANAGER_RecursiveDestroy( pGlobalStore->pItemsList );
				}
				
				CmtReleaseTSVPtr ( gGlobalDriverCommonHandle );
				CmtDiscardTSV ( gGlobalDriverCommonHandle );
			}
			
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

int	DLLEXPORT	DRIVER_MANAGER_SETUP_UpdateCallbacksStructure( void *pSTD_SetCallBackSet , int iNumberOfParams , ... )  
{
	STD_ERROR                   StdError						=	{0};
	
	CmtTSVHandle         		globalHandle					=	0;
	
	tsGlobalHandle				*pGlobalStore					=	NULL;
	
	tsSTD_CallBackSet			*pSTD_CallBackSet				=	NULL;
	
	int							iNumberOfCallBackSet			=	0;
	
	va_list						argumentsList;
	
	char						*pAddress						=	NULL;
	
	unsigned long long 			ulEquipmentID					=	0;
	
	int							bFound							=	0;
	
	int							iIndex							=	0;
	
	va_start( argumentsList , iNumberOfParams );
	
	if ( iNumberOfParams > 0 )
		pAddress = va_arg(argumentsList, char* );
	
	if ( iNumberOfParams > 1 )
		ulEquipmentID = va_arg(argumentsList, unsigned long long );
	
	pSTD_CallBackSet = pSTD_SetCallBackSet;

	globalHandle = gGlobalDriverCommonHandle;
	
	CHK_CMT( CmtGetTSVPtr ( globalHandle , &pGlobalStore ));
	
	//--------------- Check if exists --------------------------//
	if ( ulEquipmentID > 0 )
	{
		for ( iIndex = 0; iIndex < pGlobalStore->iNumberOfCallBackSet; iIndex++ )
			if ( pGlobalStore->pSTD_CallBackSet[iIndex].ullDataBaseLinkID == ulEquipmentID )
			{
				bFound = 1;
				break;
			}		
	}
	else
	{
		if ( pAddress && strlen(pAddress) )
		{
			for ( iIndex = 0; iIndex < pGlobalStore->iNumberOfCallBackSet; iIndex++ )
			{
				if ( strlen( pGlobalStore->pSTD_CallBackSet[iIndex].szAddress ) && ( strcmp( pAddress , pGlobalStore->pSTD_CallBackSet[iIndex].szAddress ) == 0 ))
				{
					bFound = 1;
					break;
				}
			}
		}	
	}
	
	if ( bFound )
	{
		memcpy( &(pGlobalStore->pSTD_CallBackSet[iIndex]) , pSTD_SetCallBackSet , sizeof(tsSTD_CallBackSet));
	}
	else
	{
		iNumberOfCallBackSet = pGlobalStore->iNumberOfCallBackSet++;
	
		pGlobalStore->pSTD_CallBackSet = realloc( pGlobalStore->pSTD_CallBackSet , ( pGlobalStore->iNumberOfCallBackSet * sizeof(tsSTD_CallBackSet)));
	
		memcpy( &(pGlobalStore->pSTD_CallBackSet[iNumberOfCallBackSet]) , pSTD_SetCallBackSet , sizeof(tsSTD_CallBackSet));
		
		if ( pAddress && ( strlen(pAddress)))
		{
			strcpy( pGlobalStore->pSTD_CallBackSet[iNumberOfCallBackSet].szAddress ,	pAddress ); 
		}
	}
	
Error:
	
	if ( globalHandle )
		CmtReleaseTSVPtr (globalHandle);

	RETURN_ERROR_VALUE;
}

int		DRIVER_MANAGER_GetCopyCallbacksStructure( int hDeviceHandle , tsSTD_CallBackSet **pSTD_GetCallBackSet , int iNumberOfParams , ... )  
{
	STD_ERROR                   StdError						=	{0};
	
	CmtTSVHandle         		globalHandle					=	0;
	
	tsGlobalHandle				*pGlobalStore					=	NULL;
	
	int							*pIntPointer					=	NULL,
								*pNewPointer					=	NULL;
	
	tsSTD_CallBackSet			*pSTD_CallBackSet				=	NULL;
	
	int 						iIndex							=	0;
	
	char						*pAddress						=	NULL;
	
	int							bFound							=	0;

	unsigned long long 			ulEquipmentID					=	0;
	
	va_list						argumentsList;
	
	va_start( argumentsList , iNumberOfParams );
	
	if ( iNumberOfParams > 0 )
		pAddress = va_arg(argumentsList, char* );
	
	if ( iNumberOfParams > 1 )
		ulEquipmentID = va_arg(argumentsList, unsigned long long );
	
	IF( ( pSTD_GetCallBackSet == NULL ) , "NULL Pointer" );
	
	globalHandle = gGlobalDriverCommonHandle;
	
	CHK_CMT( CmtGetTSVPtr ( globalHandle , &pGlobalStore ));
	
	IF ( (pGlobalStore->pSTD_CallBackSet == NULL ) , "NULL Pointer" );   
		
	CALLOC_ERR(pSTD_CallBackSet , 1 , sizeof(tsSTD_CallBackSet));
	
	//--------------- Check if exists --------------------------//
	if ( ulEquipmentID > 0 )
	{
		for ( iIndex = 0; iIndex < pGlobalStore->iNumberOfCallBackSet; iIndex++ )
			if ( pGlobalStore->pSTD_CallBackSet[iIndex].ullDataBaseLinkID == ulEquipmentID )
			{
				bFound = 1;
				break;
			}		
	}
	else
	{
		if ( pAddress && strlen(pAddress) )
		{
			for ( iIndex = 0; iIndex < pGlobalStore->iNumberOfCallBackSet; iIndex++ )
			{
				if ( strlen( pGlobalStore->pSTD_CallBackSet[iIndex].szAddress ) && ( strcmp( pAddress , pGlobalStore->pSTD_CallBackSet[iIndex].szAddress ) == 0 ))
				{
					bFound = 1;
					break;
				}
			}
		}
	}
	
	IF (( bFound == 0 ) , "CallBackSet has not found." );
		
	memcpy( pSTD_CallBackSet , &(pGlobalStore->pSTD_CallBackSet[iIndex]) , sizeof(tsSTD_CallBackSet));

	pSTD_CallBackSet->iEquipmentHandle = hDeviceHandle; 
	
	*pSTD_GetCallBackSet = pSTD_CallBackSet;
	
	if ( pGlobalStore->pSTD_CallBackSet->pCheckForBreakCallbackData )
	{
		CALLOC( pNewPointer , 1 , sizeof(int));
		
		pSTD_CallBackSet->pCheckForBreakCallbackData = pNewPointer;
		
		pIntPointer = (int*)pGlobalStore->pSTD_CallBackSet->pCheckForBreakCallbackData;
		
		*pNewPointer = *pIntPointer;
	}
	
	if ( pGlobalStore->pSTD_CallBackSet->pCommentCallbackData )
	{
		CALLOC( pNewPointer , 1 , sizeof(int));  
		
		pSTD_CallBackSet->pCommentCallbackData = pNewPointer;       
		
		pIntPointer = (int*)pGlobalStore->pSTD_CallBackSet->pCommentCallbackData;
		
		*pNewPointer = *pIntPointer;  
	}
	
	if ( pGlobalStore->pSTD_CallBackSet->pConfigValueCallbackData )
	{
		CALLOC( pNewPointer , 1 , sizeof(int));  
		
		pSTD_CallBackSet->pConfigValueCallbackData = pNewPointer;       
		
		pIntPointer = (int*)pGlobalStore->pSTD_CallBackSet->pConfigValueCallbackData;
		
		*pNewPointer = *pIntPointer;  
	}
	
	if ( pGlobalStore->pSTD_CallBackSet->pFileCallbackData )
	{
		CALLOC( pNewPointer , 1 , sizeof(int));  
		
		pSTD_CallBackSet->pFileCallbackData = pNewPointer;       
		
		pIntPointer = (int*)pGlobalStore->pSTD_CallBackSet->pFileCallbackData;
		
		*pNewPointer = *pIntPointer;  
	}
	
	strcpy( pSTD_CallBackSet->szAddress , pGlobalStore->pSTD_CallBackSet->szAddress );
	
Error:
	
	va_end(argumentsList);
	
	if ( globalHandle )
		CmtReleaseTSVPtr (globalHandle);

	RETURN_ERROR_VALUE;
}


int		DRIVER_MANAGER_FreeCallbacksStructure( void **pSTD_GetCallBackSet , int iNumberOfParams , ... )  
{
	STD_ERROR                   StdError						=	{0};
	
	tsSTD_CallBackSet			*pSTD_CallBackSet				=	NULL;
	
	int							iIndex							=	0;
	
	va_list						argumentsList;
	
	int							iNumberOfCallBackSet			=	0;
	
	va_start( argumentsList , iNumberOfParams );
	
	if ( iNumberOfParams > 0 )
		iNumberOfCallBackSet = va_arg(argumentsList, int);
	
	IF( ( pSTD_GetCallBackSet == NULL ) , "NULL Pointer" );
	
	pSTD_CallBackSet = *pSTD_GetCallBackSet;
	
	*pSTD_GetCallBackSet = 0;
	
	if ( pSTD_CallBackSet )
	{
		for( iIndex = 0; iIndex < iNumberOfCallBackSet; iIndex++ )
		{
			FREE( pSTD_CallBackSet[iIndex].pCheckForBreakCallbackData );
			FREE( pSTD_CallBackSet[iIndex].pCommentCallbackData );
			FREE( pSTD_CallBackSet[iIndex].pConfigValueCallbackData );
			FREE( pSTD_CallBackSet[iIndex].pFileCallbackData );
		}
	} 

	FREE( pSTD_CallBackSet );
	
	
Error:
	
	va_end(argumentsList); 
	
	RETURN_ERROR_VALUE;
}

int		DRIVER_MANAGER_IsConnectionExists( char *pszAddress , int *phHandle , int *phLock )
{
	STD_ERROR                   StdError						=	{0};
	
	CmtTSVHandle         		globalHandle			=	0;
	
	tsGlobalHandle				*pGlobalStore			=	NULL;
	
	tsSingleListItem			*pCurrentItem			=	NULL;
	
	int							status					=	0;
	
	if ( pszAddress == NULL )
		return 0;
	
	globalHandle = gGlobalDriverCommonHandle;
	
	CHK_CMT( CmtGetTSVPtr ( globalHandle , &pGlobalStore ));
	
	pCurrentItem = pGlobalStore->pItemsList;

	//------------ Check if resourse exists -------------//
		
	if ( pCurrentItem )
	{
		do
		{
			if ( pCurrentItem->pszConnectString )
				if ( strcmp( pCurrentItem->pszConnectString , pszAddress ) == 0 )
				{
					if ( pCurrentItem->ignore_dup_address )
						if ( phHandle != NULL )
							if ( *phHandle != pCurrentItem->handle ) 
							{
								pCurrentItem = pCurrentItem->pNextItem;   
								continue;
							}
						
					if ( phHandle )
						*phHandle = pCurrentItem->handle;
					
					if ( phLock )
						*phLock = pCurrentItem->lock;
					
					status = 1;
					
					RETURN;
				}
				
				if ( pCurrentItem->pNextItem )
					pCurrentItem = pCurrentItem->pNextItem;
				else
					break;
				
		} while( pCurrentItem );
	}
	
Error:
	
	CmtReleaseTSVPtr (globalHandle);

	return status;
}

int		DRIVER_MANAGER_IsConnectionExistsByType( teDriversType tInstrumentType )
{
	STD_ERROR                   StdError						=	{0};
	
	CmtTSVHandle         		globalHandle			=	0;
	
	tsGlobalHandle				*pGlobalStore			=	NULL;
	
	tsSingleListItem			*pCurrentItem			=	NULL;
	
	int							status					=	0;
	
	globalHandle = gGlobalDriverCommonHandle;
	
	CHK_CMT( CmtGetTSVPtr ( globalHandle , &pGlobalStore ));
	
	pCurrentItem = pGlobalStore->pItemsList;

	//------------ Check if resourse exists -------------//
		
	if ( pCurrentItem )
	{
		do
		{
			if ( pCurrentItem->pszConnectString )
				if ( pCurrentItem->InstrumentType == tInstrumentType )
				{   
					status = 1;
					
					RETURN;
				}
				
				if ( pCurrentItem->pNextItem )
					pCurrentItem = pCurrentItem->pNextItem;
				else
					break;
				
		} while( pCurrentItem );
	}
	
Error:
	
	CmtReleaseTSVPtr (globalHandle);

	return status;
}

int		DRIVER_MANAGER_AddConnection( char *pszAddress, int *phHandle , teDriversType tInstrumentType , int *phLock )
{
	STD_ERROR                   StdError						=	{0};
	
	CmtTSVHandle         		globalHandle			=	0;
	
	tsGlobalHandle				*pGlobalStore			=	NULL;
	
	tsSingleListItem			*pCurrentItem			=	NULL;
	
	int							bExists					=	0,
								status					=	0;
	
	if ( pszAddress == NULL )
		return 0;
	
	globalHandle = gGlobalDriverCommonHandle;
	
	CHK_CMT( CmtGetTSVPtr ( globalHandle , &pGlobalStore ));
	
	pCurrentItem = pGlobalStore->pItemsList;

	//------------ Check if resurse exists -------------//
		
	if ( pCurrentItem )
	{
		do
		{
			if ( pCurrentItem->pszConnectString )
				if ( strcmp( pCurrentItem->pszConnectString , pszAddress ) == 0 )
				{
					if ( pCurrentItem->ignore_dup_address )
						if ( phHandle != NULL )
							if ( *phHandle != pCurrentItem->handle ) 
							{
								if ( pCurrentItem->pNextItem )
								{
									pCurrentItem = pCurrentItem->pNextItem;   
								
									continue;
								}
								else
									break;
							}
										
					bExists = 1;
					break;
				}
				
				if ( pCurrentItem->pNextItem )
					pCurrentItem = pCurrentItem->pNextItem;
				else
					break;
				
		} while( pCurrentItem );
		
		if ( bExists == 0 ) 
		{
			FREE_CALLOC_ERR ( pCurrentItem->pNextItem , 1 , sizeof(tsSingleListItem)); 
			pCurrentItem = pCurrentItem->pNextItem;
		}
		
	}
	else
	{
		FREE_CALLOC_ERR( pGlobalStore->pItemsList , 1 , sizeof(tsSingleListItem));
		pCurrentItem = pGlobalStore->pItemsList;
	}
	
	if ( pCurrentItem ) 
		if ( bExists == 0 )
		{   
			if ( phHandle )
				pCurrentItem->handle = *phHandle;	
	
			FREE_CALLOC_COPY( pCurrentItem->pszConnectString , strlen(pszAddress)+1 , sizeof(char) , pszAddress , strlen(pszAddress));
	 
			pCurrentItem->InstrumentType = tInstrumentType;
			
			CmtNewLock ( NULL , OPT_TL_PROCESS_EVENTS_WHILE_WAITING , &(pCurrentItem->lock) );
	
			if ( phLock )
				*phLock = pCurrentItem->lock;
	
			status = 1;
		}
		else
		{
			if ( phHandle && pCurrentItem->handle )
				*phHandle = pCurrentItem->handle;
			
			if ( phLock && pCurrentItem->lock )
				*phLock = pCurrentItem->lock;
		}
	
Error:
	
	CmtReleaseTSVPtr (globalHandle);

	return status;
}


int		DRIVER_MANAGER_UpdateConnection( char *pszAddress, int hHandle , int *phLock )
{
	STD_ERROR                   StdError						=	{0};
	
	CmtTSVHandle         		globalHandle			=	0;
	
	tsGlobalHandle				*pGlobalStore			=	NULL;
	
	tsSingleListItem			*pCurrentItem			=	NULL;
	
	int							bExists					=	0,
								status					=	0;
	
	if ( pszAddress == NULL )
		return 0;
	
	globalHandle = gGlobalDriverCommonHandle;
	
	CHK_CMT( CmtGetTSVPtr ( globalHandle , &pGlobalStore ));
	
	pCurrentItem = pGlobalStore->pItemsList;

	//------------ Check if resurse exists -------------//
		
	if ( pCurrentItem )
	{
		do
		{
			if ( pCurrentItem->pszConnectString )
				if ( strcmp( pCurrentItem->pszConnectString , pszAddress ) == 0 )
				{
					if ( pCurrentItem->ignore_dup_address )
						if (( pCurrentItem->handle > 0 ) && ( hHandle != pCurrentItem->handle )) 
						{
					
							pCurrentItem = pCurrentItem->pNextItem;   
							continue;
						}
												
					bExists = 1;
					break;
				}
				
				if ( pCurrentItem->pNextItem )
					pCurrentItem = pCurrentItem->pNextItem;
				else
					break;
				
		} while( pCurrentItem );
		
	}
	
	if ( pCurrentItem && bExists )
	{
		pCurrentItem->handle = hHandle;	
		
		FREE_CALLOC_COPY( pCurrentItem->pszConnectString , strlen(pszAddress)+1 , sizeof(char) , pszAddress , strlen(pszAddress) );
		 
		if ( phLock )
			pCurrentItem->lock = *phLock;
		
		status = 1;
		
		RETURN;
	}

	
Error:
	
	CmtReleaseTSVPtr (globalHandle);

	return status;
}

int		DRIVER_MANAGER_UpdateAddressByConnection( int hHandle , char *pszAddress )
{
	STD_ERROR                   StdError						=	{0};
	
	CmtTSVHandle         		globalHandle			=	0;
	
	tsGlobalHandle				*pGlobalStore			=	NULL;
	
	tsSingleListItem			*pCurrentItem			=	NULL;
	
	int							bExists					=	0,
								status					=	0;
	
	if ( pszAddress == NULL )
		return 0;
	
	globalHandle = gGlobalDriverCommonHandle;
	
	CHK_CMT( CmtGetTSVPtr ( globalHandle , &pGlobalStore ));
	
	pCurrentItem = pGlobalStore->pItemsList;

	//------------ Check if resurse exists -------------//
		
	if ( pCurrentItem )
	{
		do
		{
				if ( pCurrentItem->handle == hHandle )
				{
					bExists = 1;
					break;
				}
				
				if ( pCurrentItem->pNextItem )
					pCurrentItem = pCurrentItem->pNextItem;
				else
					break;
				
		} while( pCurrentItem );
		
	}
	
	if ( pCurrentItem && bExists )
	{
		FREE_CALLOC_COPY( pCurrentItem->pszConnectString , strlen(pszAddress)+1 , sizeof(char) , pszAddress , strlen(pszAddress) );
		
		status = 1;
		
		RETURN;
	}

	
Error:
	
	CmtReleaseTSVPtr (globalHandle);

	return status;
}

int		DRIVER_MANAGER_UpdateIgnoreDupAddresses( char *pszAddress , int hHandle , int bIgnoreDupAddresses )
{
	STD_ERROR                   StdError						=	{0};
	
	CmtTSVHandle         		globalHandle			=	0;
	
	tsGlobalHandle				*pGlobalStore			=	NULL;
	
	tsSingleListItem			*pCurrentItem			=	NULL;
	
	int							bExists					=	0,
								status					=	0;
	
	if ( pszAddress == NULL )
		return 0;
	
	globalHandle = gGlobalDriverCommonHandle;
	
	CHK_CMT( CmtGetTSVPtr ( globalHandle , &pGlobalStore ));
	
	pCurrentItem = pGlobalStore->pItemsList;

	//------------ Check if resurse exists -------------//
		
	if ( pCurrentItem )
	{
		do
		{
			if ( pCurrentItem->pszConnectString )
				if ( strcmp( pCurrentItem->pszConnectString , pszAddress ) == 0 )
				{
					if ( pCurrentItem->ignore_dup_address )
						if (( pCurrentItem->handle > 0 ) && ( hHandle != pCurrentItem->handle )) 
						{
							pCurrentItem = pCurrentItem->pNextItem;   
							continue;
						}	
					
					bExists = 1;
					break;
				}
				
				if ( pCurrentItem->pNextItem )
					pCurrentItem = pCurrentItem->pNextItem;
				else
					break;
				
		} while( pCurrentItem );
		
	}
	
	if ( pCurrentItem && bExists )
	{
		pCurrentItem->ignore_dup_address = bIgnoreDupAddresses;	
		
		status = 1;
		
		RETURN;
	}

	
Error:
	
	CmtReleaseTSVPtr (globalHandle);

	return status;
}



int		DRIVER_MANAGER_RemoveConnectionExists( char *pszAddress , int hHandle )
{
	STD_ERROR                   StdError				=	{0};
	
	CmtTSVHandle         		globalHandle			=	0;
	
	tsGlobalHandle				*pGlobalStore			=	NULL;
	
	tsSingleListItem			*pCurrentItem			=	NULL,
								*pPreviousItem			=	NULL;
	
	double						lfStartTime				=	0.0,
								lfCurrentTime			=	0.0;
	
	int							bGetLock				=	0;
	
	if ( pszAddress == NULL )
		return 0;
	
	globalHandle = gGlobalDriverCommonHandle;
	
	CHK_CMT( CmtGetTSVPtr ( globalHandle , &pGlobalStore ));
	
	pCurrentItem = pGlobalStore->pItemsList;

	//------------ Check if resourse exists -------------//
		
	if ( pCurrentItem )
	{
		do
		{
			if ( pCurrentItem->pszConnectString )
				if ( strcmp( pCurrentItem->pszConnectString , pszAddress ) == 0 )
				{
					if ( pCurrentItem->ignore_dup_address )
						if ( hHandle != pCurrentItem->handle ) 
						{
							pPreviousItem = pCurrentItem;
							pCurrentItem = pCurrentItem->pNextItem;   
							continue;
						}
					
					if ( pPreviousItem )
					{
						pPreviousItem->pNextItem = pCurrentItem->pNextItem;
					}
					else
					{
						pGlobalStore->pItemsList = pCurrentItem->pNextItem;	
					}
					
					GetCurrentDateTime(&lfStartTime);
					
					FREE(pCurrentItem->pszConnectString);
					if ( pCurrentItem->lock )
					{
						do
						{
							if ( CmtTryToGetLock ( pCurrentItem->lock , &bGetLock ))
								break;
							
							if ( bGetLock )
							{
								CmtReleaseLock (pCurrentItem->lock);
								CmtDiscardLock ( pCurrentItem->lock );  
								break;
							}
							
							DelayWithEventProcessing(0.5);
							
							GetCurrentDateTime(&lfCurrentTime);
						} while( (lfCurrentTime-lfStartTime) < 5.0 );
						
						
						
					}
					FREE(pCurrentItem);
					
					RETURN;
				}
				
				if ( pCurrentItem->pNextItem )
				{
					pPreviousItem = pCurrentItem;
					pCurrentItem = pCurrentItem->pNextItem;
				}
				else
					break;
				
		} while( pCurrentItem );
		
		
	}
	
Error:
	
	CmtReleaseTSVPtr (globalHandle);

	return 0;
}

void		DRIVER_MANAGER_RecursiveDestroy( tsSingleListItem	*pCurrentItem )
{
	if ( pCurrentItem == NULL )
		return;
	
	if ( pCurrentItem->pNextItem )
		DRIVER_MANAGER_RecursiveDestroy( pCurrentItem->pNextItem );

	/****************************************************/
	SetBreakOnLibraryErrors (0);
	if ( pCurrentItem->lock )
		CmtDiscardLock ( pCurrentItem->lock ); 
	SetBreakOnLibraryErrors (1);
	
	FREE(pCurrentItem->pszConnectString);
	
	free(pCurrentItem);
	
	return;
}


STD_ERROR		DRIVER_MANAGER_CALIBRATION_FillCalibrationInfo( int Handle , double *plfFrequency , double *plfLoss , int iNumberOfPoints , double lfStartFrequnecy , double lfStopFrequency )
{
	STD_ERROR									StdError								=	{0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( iNumberOfPoints <= 0 )
		{STD_ERR (DRV_ERROR_WRONG_PARAMETER);}
	
	if ( pDriverInfo->pCalibration == NULL )
	{
		CALLOC_ERR( pDriverInfo->pCalibration ,1 ,sizeof(tsCalibrationItem));
	}
	
	if ( plfFrequency )
	{
		FREE_CALLOC_COPY_ERR( pDriverInfo->pCalibration->vlfFrequency , iNumberOfPoints , sizeof(double) , plfFrequency , (iNumberOfPoints * sizeof(double)));
	}
	else
	{
		pDriverInfo->pCalibration->lfStartFrequency = lfStartFrequnecy;
		pDriverInfo->pCalibration->lfStopFrequency = lfStopFrequency;
	}
	
	if ( plfLoss )
	{
		FREE_CALLOC_COPY_ERR( pDriverInfo->pCalibration->vlfLoss , iNumberOfPoints , sizeof(double) , plfLoss , (iNumberOfPoints * sizeof(double)));
	}
	
	pDriverInfo->pCalibration->iNumberOfPoints = iNumberOfPoints;
	
Error:	
	
	if ( Handle )
		CmtReleaseTSVPtr ( Handle ); 
	
	return StdError;			
}

STD_ERROR		DRIVER_MANAGER_CALIBRATION_Fill_AdditionalPower_CalibrationInfo( int Handle , double lfPower , double *plfLoss , int iNumberOfPoints )
{
	STD_ERROR									StdError								=	{0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;
	
	int											iIndex									=	0,
												iPowerIndex					 			=	0;  
												
	int											iPreviousNumberOfPowers					=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( iNumberOfPoints <= 0 )
		{STD_ERR (DRV_ERROR_WRONG_PARAMETER);}
	
	if ( pDriverInfo->pCalibration == NULL )
	{
		CALLOC_ERR( pDriverInfo->pCalibration ,1 ,sizeof(tsCalibrationItem));
	
		if ( plfLoss )
		{
			FREE_CALLOC_COPY_ERR( pDriverInfo->pCalibration->vlfLoss , iNumberOfPoints , sizeof(double) , plfLoss , (iNumberOfPoints * sizeof(double)));
		}
		
		pDriverInfo->pCalibration->iNumberOfPoints = iNumberOfPoints;
	}
	else
	{
		IF (( pDriverInfo->pCalibration->iNumberOfPoints != iNumberOfPoints ) , "Wrong Number of points." );	
	}
	
	iPreviousNumberOfPowers = pDriverInfo->pCalibration->iNumberOfPowers;
	
	iPowerIndex = pDriverInfo->pCalibration->iNumberOfPowers++;
	
	pDriverInfo->pCalibration->vlfPowerList = realloc( pDriverInfo->pCalibration->vlfPowerList , ( pDriverInfo->pCalibration->iNumberOfPowers * sizeof(double)));
	
	if ( pDriverInfo->pCalibration->vlfPowerList )
	{
		pDriverInfo->pCalibration->vlfPowerList[iPowerIndex] = lfPower;
	}
	
	pDriverInfo->pCalibration->vlfLossList = realloc( pDriverInfo->pCalibration->vlfLossList , ( pDriverInfo->pCalibration->iNumberOfPowers * sizeof(double*)));
	
	for ( iIndex = iPreviousNumberOfPowers; iIndex < pDriverInfo->pCalibration->iNumberOfPowers; iIndex++ ) 
		(pDriverInfo->pCalibration->vlfLossList)[iIndex] = NULL;
	
	if (( pDriverInfo->pCalibration->vlfLossList ) && ( plfLoss ))
	{
		CALLOC_COPY_ERR( (pDriverInfo->pCalibration->vlfLossList)[iPowerIndex] , iNumberOfPoints , sizeof(double) , plfLoss , (iNumberOfPoints * sizeof(double))); 
	}
	
Error:	
	
	if ( Handle )
		CmtReleaseTSVPtr ( Handle ); 
	
	return StdError;			
}

STD_ERROR		DRIVER_MANAGER_CALIBRATION_GetCalibrationInfo( int Handle , double **plfFrequency , double **plfLoss , int *piNumberOfPoints , double *plfStartFrequnecy , double *plfStopFrequency )
{
	STD_ERROR									StdError								=	{0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( pDriverInfo->pCalibration == NULL )
	{
		CALLOC_ERR( pDriverInfo->pCalibration ,1 ,sizeof(tsCalibrationItem));
	}
	
	if ( pDriverInfo->pCalibration->iNumberOfPoints <= 0 )
		{STD_ERR (DRV_ERROR_WRONG_PARAMETER);}	   
	
	if ( plfFrequency )
	{
		FREE_CALLOC_COPY_ERR( *plfFrequency , pDriverInfo->pCalibration->iNumberOfPoints , sizeof(double) , pDriverInfo->pCalibration->vlfFrequency , (pDriverInfo->pCalibration->iNumberOfPoints * sizeof(double)));
	}

	if ( plfLoss )
	{
		FREE_CALLOC_COPY_ERR( *plfLoss , pDriverInfo->pCalibration->iNumberOfPoints , sizeof(double) , pDriverInfo->pCalibration->vlfLoss , (pDriverInfo->pCalibration->iNumberOfPoints * sizeof(double)));
	}
	
Error:	
	
	if ( plfStartFrequnecy )
		*plfStartFrequnecy = pDriverInfo->pCalibration->lfStartFrequency;
	
	if ( plfStopFrequency )
		*plfStopFrequency = pDriverInfo->pCalibration->lfStopFrequency;
	
	if ( piNumberOfPoints )
		*piNumberOfPoints = pDriverInfo->pCalibration->iNumberOfPoints;
	
	CmtReleaseTSVPtr ( Handle ); 
	
	return StdError;			
}

STD_ERROR		DRIVER_MANAGER_CALIBRATION_Get_AdditionalPower_CalibrationInfo( int Handle , int iPowerIndex , double **plfLoss , int *piNumberOfPoints , double *plfPower )
{
	STD_ERROR									StdError								=	{0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;
	
	int											iNumberOfPoints							=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( pDriverInfo->pCalibration == NULL )
	{
		CALLOC_ERR( pDriverInfo->pCalibration ,1 ,sizeof(tsCalibrationItem));
	}
	
	if ( pDriverInfo->pCalibration->iNumberOfPoints <= 0 )
		{STD_ERR (DRV_ERROR_WRONG_PARAMETER);}	   

	iNumberOfPoints = pDriverInfo->pCalibration->iNumberOfPoints;
	
	if ( piNumberOfPoints )
		*piNumberOfPoints = iNumberOfPoints;

	IF (( iPowerIndex >= pDriverInfo->pCalibration->iNumberOfPowers ) , "No Power more." );
	
	if ( plfPower )
	{
		*plfPower = pDriverInfo->pCalibration->vlfPowerList[iPowerIndex];
	}
	
	if ( pDriverInfo->pCalibration->vlfLossList )
	{
		if ( plfLoss )
		{
			FREE_CALLOC_COPY_ERR( *plfLoss , iNumberOfPoints , sizeof(double) , pDriverInfo->pCalibration->vlfLossList[iPowerIndex] , (iNumberOfPoints * sizeof(double))); 
		}
	}
	
Error:	
	
	CmtReleaseTSVPtr ( Handle ); 
	
	return StdError;			
}

STD_ERROR		DRIVER_MANAGER_CALIBRATION_FillCalibrationRanges( int Handle , char *pInputRangesString ) // sample "1E9,2E9-8E9:50"
{
	STD_ERROR									StdError								=	{0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;
	
	char										*pNext									=	NULL,
												*pTemp									=	NULL,
												*pCurrnet								=	NULL;
	
	double										lfStart									=	0.0,
												lfStop									=	0.0,
												lfCurrent								=	0.0,
												lfStep									=	0.0;
	
	int											iRangeLength							=	0,
												iNumberOfPoints							=	0;
												
	int											iCurrentIndex							=	0,
												iIndex									=	0;
	
	char										*pRangesString							=	NULL;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( pDriverInfo->pCalibration == NULL )
	{
		CALLOC_ERR( pDriverInfo->pCalibration ,1 ,sizeof(tsCalibrationItem));
	}
	
	CALLOC_COPY_STRING_ERR( pRangesString , pInputRangesString );
	
	FREE(pDriverInfo->pCalibration->vlfFrequency);
	
	pNext = pRangesString;
	
	do
	{ 
		iNumberOfPoints++;	
		
		pDriverInfo->pCalibration->vlfFrequency = realloc(pDriverInfo->pCalibration->vlfFrequency , ( iNumberOfPoints * sizeof(double)));
		
		pTemp = strchr( pNext , ',');
		
		pCurrnet = pNext;
		pNext = pTemp; 
		
		if ( pTemp )
		{
			*pTemp = 0;	
			pNext++;
		}
		
		pTemp = strchr( pCurrnet , '-');
		
		if ( pTemp )
		{   //-------- Range --------//
			*pTemp = 0;
			
			lfStart = atof(pCurrnet); 
			
			pCurrnet = pTemp+1;  
			
			pTemp = strchr( pCurrnet , ':');  
			
			if ( pTemp == NULL )
				continue;
			
			*pTemp = 0; 
			
			lfStop = atof(pCurrnet);
			
			pCurrnet = pTemp+1; 
			
			iRangeLength = atoi(pCurrnet); 

			iNumberOfPoints += iRangeLength;
			
			pDriverInfo->pCalibration->vlfFrequency = realloc(pDriverInfo->pCalibration->vlfFrequency , ( iNumberOfPoints * sizeof(double)));
			
			lfStep = ( lfStop - lfStart ) / ( iRangeLength - 1 );
			lfCurrent = lfStart;
			
			for ( iIndex = 0; iIndex < iRangeLength; iIndex++ )
			{
				pDriverInfo->pCalibration->vlfFrequency[iCurrentIndex++] = lfCurrent;
				lfCurrent += lfStep;	
			}
		}
		else
		{
			pDriverInfo->pCalibration->vlfFrequency[iCurrentIndex++] = atof(pCurrnet);  	
		}
		
	} while(pNext);
	
	pDriverInfo->pCalibration->iNumberOfPoints = iCurrentIndex;
	
Error:	
	
	if ( pDriverInfo->pCalibration->vlfFrequency )
	{
		pDriverInfo->pCalibration->lfStartFrequency = pDriverInfo->pCalibration->vlfFrequency[0];
		pDriverInfo->pCalibration->lfStopFrequency = pDriverInfo->pCalibration->vlfFrequency[iCurrentIndex-1]; 
	}
	
	CmtReleaseTSVPtr ( Handle ); 
	
	FREE(pRangesString);
	
	return StdError;			
}

STD_ERROR		DRIVER_MANAGER_CALIBRATION_Set_NumberOf_AdditionalPower( int Handle , double *plfPowerList , int iNumberOfPowers )
{
	STD_ERROR									StdError								=	{0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;
	
	int											iPowerIndex								=	0;
	
	int											iPreviousNumberOfPowers					=	0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( iNumberOfPowers <= 0 )
		{STD_ERR (DRV_ERROR_WRONG_PARAMETER);}
	
	if ( plfPowerList == NULL )
		{STD_ERR (DRV_ERROR_WRONG_PARAMETER);}
		
	if ( pDriverInfo->pCalibration == NULL )
	{
		CALLOC_ERR( pDriverInfo->pCalibration ,1 ,sizeof(tsCalibrationItem));
	}
	
	iPreviousNumberOfPowers = pDriverInfo->pCalibration->iNumberOfPowers;
	
	pDriverInfo->pCalibration->iNumberOfPowers = iNumberOfPowers;
	
	pDriverInfo->pCalibration->vlfPowerList = realloc( pDriverInfo->pCalibration->vlfPowerList , ( iNumberOfPowers * sizeof(double)));
	pDriverInfo->pCalibration->vlfLossList = realloc( pDriverInfo->pCalibration->vlfLossList , ( iNumberOfPowers * sizeof(double*)));
	
	for ( iPowerIndex = iPreviousNumberOfPowers; iPowerIndex < iNumberOfPowers; iPowerIndex++ ) 
		(pDriverInfo->pCalibration->vlfLossList)[iPowerIndex] = NULL;
		
	if (( pDriverInfo->pCalibration->vlfPowerList ) && ( pDriverInfo->pCalibration->vlfLossList ))
		for ( iPowerIndex = 0; iPowerIndex < iNumberOfPowers; iPowerIndex++ )
			pDriverInfo->pCalibration->vlfPowerList[iPowerIndex] = plfPowerList[iPowerIndex];
	
Error:	
	
	if ( Handle )
		CmtReleaseTSVPtr ( Handle ); 
	
	return StdError;			
}


STD_ERROR		DRIVER_MANAGER_CALIBRATION_EnableUseCalibration( int Handle , int bAllow )
{
	STD_ERROR									StdError								=	{0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}

	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( pDriverInfo->pCalibration == NULL )
	{
		CALLOC_ERR( pDriverInfo->pCalibration ,1 ,sizeof(tsCalibrationItem));
	}
	
	pDriverInfo->pCalibration->bAllow = bAllow;
	
Error:	
	
	CmtReleaseTSVPtr ( Handle ); 
	
	return StdError;			
}

STD_ERROR		DRIVER_MANAGER_CALIBRATION_FillStoreInformation( int Handle , char *pStoreName , int clbConfigType , int clbLinkID )
{
	STD_ERROR									StdError								=	{0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}

	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( pDriverInfo->pCalibration == NULL )
	{
		CALLOC_ERR( pDriverInfo->pCalibration ,1 ,sizeof(tsCalibrationItem));
	}
	
	FREE_CALLOC_COPY_STRING( pDriverInfo->pCalibration->pCalibrationRefName , pStoreName );
	
	pDriverInfo->pCalibration->clbConfigType = clbConfigType;
	pDriverInfo->pCalibration->clbLinkID = clbLinkID;
	
Error:	
	
	CmtReleaseTSVPtr ( Handle ); 
	
	return StdError;			
}

STD_ERROR		DRIVER_MANAGER_CALIBRATION_GetStoreInformation( int Handle , char **pStoreName , int *pclbConfigType , int *pclbLinkID )
{
	STD_ERROR									StdError								=	{0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}

	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( pDriverInfo->pCalibration == NULL )
	{
		CALLOC_ERR(	pDriverInfo->pCalibration ,1 ,sizeof(tsCalibrationItem));
	}
	
	if ( pStoreName )
	{
		FREE_CALLOC_COPY_STRING( *pStoreName , pDriverInfo->pCalibration->pCalibrationRefName );
	}
	
	if ( pclbConfigType )
		*pclbConfigType = pDriverInfo->pCalibration->clbConfigType;
	
	if ( pclbLinkID )
		*pclbLinkID = pDriverInfo->pCalibration->clbLinkID;
	
Error:	
	
	CmtReleaseTSVPtr ( Handle ); 
	
	return StdError;			
}

STD_ERROR		DRIVER_MANAGER_CALIBRATION_FillMessageInfo( int Handle , char *pConnectionDescription , char *pConnectionImage )
{
	STD_ERROR									StdError								=	{0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;

	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}

	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( pDriverInfo->pCalibration == NULL )
	{
		CALLOC_ERR(	pDriverInfo->pCalibration ,1 ,sizeof(tsCalibrationItem));
	}
	
	FREE_CALLOC_COPY_STRING( pDriverInfo->pCalibration->pConnectionDescription , pConnectionDescription );
	FREE_CALLOC_COPY_STRING( pDriverInfo->pCalibration->pConnectionImage , pConnectionImage );
	
Error:	
	
	CmtReleaseTSVPtr ( Handle ); 
	
	return StdError;			
}

STD_ERROR		DRIVER_MANAGER_CALIBRATION_GelFactor( int Handle , double lfFrequency , double *plfFactor )
{
	STD_ERROR									StdError								=	{0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;

	if ( plfFactor == NULL )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
		
	*plfFactor = 0.0;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}

	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( pDriverInfo->pCalibration == NULL )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	MTH_InterpolationSingle( 0 , pDriverInfo->pCalibration->vlfFrequency , pDriverInfo->pCalibration->vlfLoss , pDriverInfo->pCalibration->iNumberOfPoints , lfFrequency , plfFactor );
	
Error:	
	
	if ( Handle )
		CmtReleaseTSVPtr ( Handle ); 
	
	return StdError;			
}

STD_ERROR		DRIVER_MANAGER_CALIBRATION_Gel_3D_Factor( int Handle , double lfPower , double lfFrequency , double *plfFactor )
{
	STD_ERROR									StdError								=	{0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;

	double										*plfLossByPower							=	NULL,
												*plfPowerList							=	NULL;
	
	int											iPowerIndex								=	0,
												iNumberOfPowers							=	0;  
	
	double										lfFactorValue							=	0;
	
	if ( plfFactor == NULL )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
		
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}

	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( pDriverInfo->pCalibration == NULL )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	iNumberOfPowers = pDriverInfo->pCalibration->iNumberOfPowers;
	
	if ( iNumberOfPowers )
	{
		CALLOC_ERR( plfLossByPower , iNumberOfPowers + 1 , sizeof(double));
		CALLOC_ERR( plfPowerList , iNumberOfPowers + 1 , sizeof(double));  
	
		for ( iPowerIndex = 0; iPowerIndex < iNumberOfPowers; iPowerIndex++ )
		{
			MTH_InterpolationSingle( 0 , pDriverInfo->pCalibration->vlfFrequency , pDriverInfo->pCalibration->vlfLossList[iPowerIndex] , pDriverInfo->pCalibration->iNumberOfPoints , lfFrequency , &(plfLossByPower[iPowerIndex]) );
		
			plfPowerList[iPowerIndex] = pDriverInfo->pCalibration->vlfPowerList[iPowerIndex];
		}

		plfPowerList[iPowerIndex] = pDriverInfo->pCalibration->lfSetPower;
		MTH_InterpolationSingle( 0 , pDriverInfo->pCalibration->vlfFrequency , pDriverInfo->pCalibration->vlfLoss , pDriverInfo->pCalibration->iNumberOfPoints , lfFrequency , &(plfLossByPower[iPowerIndex]) );
		
		iNumberOfPowers = iPowerIndex + 1;
	
		MTH_InterpolationSingle( 0 , plfPowerList , plfLossByPower , iNumberOfPowers , lfPower , &lfFactorValue );
	}
	else
	{
		MTH_InterpolationSingle( 0 , pDriverInfo->pCalibration->vlfFrequency , pDriverInfo->pCalibration->vlfLoss , pDriverInfo->pCalibration->iNumberOfPoints , lfFrequency , &lfFactorValue );
	}
	
	IF (( IsNotANumber (lfFactorValue) ) , "Result Is Not A Number" );
	IF (( IsInfinity (lfFactorValue) ) , "Result Is Infinity" );
	
Error:	
	
	if (( IsNotANumber (lfFactorValue) ) || ( IsInfinity (lfFactorValue) ))
		lfFactorValue = 0.0;
	
	*plfFactor = lfFactorValue; 
	
	if ( Handle )
		CmtReleaseTSVPtr ( Handle ); 
	
	FREE(plfLossByPower);
	FREE(plfPowerList); 
	
	return StdError;			
}

STD_ERROR		DRIVER_MANAGER_CALIBRATION_Run( int StimulusHandle , int MeasureHandle , int StoreHandle , int iProgressPanel , int iProgressControl )
{
	STD_ERROR									StdError									=	{0};
	
	tsSTD_CallBackSet							tSTD_CallBackSet							=	{0};

	tsDriverInfo								*pStimulusDriverInfo						=	NULL,
												*pMeasureDriverInfo							=	NULL,
												*pStoreDriverInfo							=	NULL;
	
	char										szFormatedString[STD_STRING]				=	{0};
	
	int											hSignalGenerator							=	0,
												hSpectrumAnalyzer							=	0, 
												hPowerMeter									=	0; 
													
	int											iActualyPointsNumber						=	0;
	
	double										lfStartFrequency							=	0.0,
												lfStopFrequency								=	0.0,
												lfStimulusStopFrequency						=	0.0,  
												lfStimulusStartFrequency					=	0.0,  
												lfSignalGeneratorLossFactor					=	0.0,  
												lfSpectrumAnalyzerLossFactor				=	0.0, 
												lfPowerMeterLossFactor     					=	0.0,  
												lfCurrentFrequency							=	0.0, 
												lfFrequencyStep								=	0.0; 
												
	double										lfGetMarker									=	0.0,
												lfGetPower									=	0.0;  
													
	int											iIndex										=	0,
												iTryIndex									=	0, 
												iPowerIndex									=	0;
												
	double										*pTraceData									=	NULL,
												*pFrequencyData								=	NULL;
												
	int											iNumberOfPowers								=	0,
												iProgressCounter							=	0;
												
	double										lfCurrentPower								=	0.0;
	
	if (( StimulusHandle == 0 ) || ( StoreHandle == 0 ))
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( StimulusHandle , &pStimulusDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if (( MeasureHandle != StimulusHandle ) && ( MeasureHandle != 0 ))
	{
		if ( CmtGetTSVPtr ( MeasureHandle , &pMeasureDriverInfo ) < 0 )
			{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	}
	else
	{
		pMeasureDriverInfo = pStimulusDriverInfo;
	}
	
	if (( StoreHandle != StimulusHandle ) && ( StoreHandle != MeasureHandle ) && ( StoreHandle != 0 ))
	{
		if ( CmtGetTSVPtr ( StoreHandle , &pStoreDriverInfo ) < 0 )
			{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	}
	else
	{
		if ( StoreHandle == StimulusHandle )
			pStoreDriverInfo = pStimulusDriverInfo;
		
		if ( StoreHandle == MeasureHandle )
			pStoreDriverInfo = pMeasureDriverInfo;
	}
	
	STDF_UPDATE_CALLBACK_DATA(pStoreDriverInfo->ptCallbacks);
	
	if (( pStoreDriverInfo->pCalibration == NULL ) || ( pStoreDriverInfo->pCalibration->iNumberOfPoints <= 0 ) || (( pStoreDriverInfo->pCalibration->vlfFrequency == NULL ) && ( pStoreDriverInfo->pCalibration->lfStartFrequency <= 0.0 ) && ( pStoreDriverInfo->pCalibration->lfStopFrequency <= 0.0 )))
	{
		ShowMessage ( INSTR_TYPE_CONTINUE , "Calibration Error","Please fill calibration information.", NULL );
		STD_ERR (DRV_ERROR_PASSED_NULL);
	}

	if ( pStoreDriverInfo->pCalibration == NULL )
	{
		CALLOC_ERR(	pStoreDriverInfo->pCalibration ,1 ,sizeof(tsCalibrationItem));
	}
	
	iActualyPointsNumber = pStoreDriverInfo->pCalibration->iNumberOfPoints;
	
	if ( pStoreDriverInfo->pCalibration->lfStartFrequency == 0.0 )
	{
		pStoreDriverInfo->pCalibration->lfStartFrequency = pStoreDriverInfo->pCalibration->vlfFrequency[0];
	}
	
	if ( pStoreDriverInfo->pCalibration->lfStopFrequency == 0.0 )
	{
		pStoreDriverInfo->pCalibration->lfStopFrequency	= pStoreDriverInfo->pCalibration->vlfFrequency[iActualyPointsNumber-1];
	}
		
	iNumberOfPowers = pStoreDriverInfo->pCalibration->iNumberOfPowers;

	iNumberOfPowers++;
	
	lfStartFrequency = pStoreDriverInfo->pCalibration->lfStartFrequency;
	lfStopFrequency = pStoreDriverInfo->pCalibration->lfStopFrequency;
	
	lfCurrentPower = pStoreDriverInfo->pCalibration->lfSetPower; 
	
	if ( pStimulusDriverInfo->pCalibration->lfStartFrequency != 0.0 ) 
		lfStimulusStartFrequency = pStimulusDriverInfo->pCalibration->lfStartFrequency;
	else
		lfStimulusStartFrequency = lfStartFrequency;
	
	if ( pStimulusDriverInfo->pCalibration->lfStopFrequency != 0.0 ) 
		lfStimulusStopFrequency = pStimulusDriverInfo->pCalibration->lfStopFrequency;
	else
		lfStimulusStopFrequency = lfStopFrequency;
	
	if ( iProgressPanel && iProgressControl )
	{
		SetCtrlAttribute( iProgressPanel , iProgressControl , ATTR_VISIBLE , 1 );
		
		if ( pStoreDriverInfo->pCalibration->vlfPowerList && ( lfCurrentPower != pStoreDriverInfo->pCalibration->vlfPowerList[0] )) 
			SetCtrlAttribute( iProgressPanel , iProgressControl , ATTR_MAX_VALUE , (iActualyPointsNumber*iNumberOfPowers) );
		else
			SetCtrlAttribute( iProgressPanel , iProgressControl , ATTR_MAX_VALUE , iActualyPointsNumber );
		
		SetCtrlVal( iProgressPanel , iProgressControl , 0 );
	}
	
	switch(pMeasureDriverInfo->InstrumentType)
	{
		case DRIVER_TYPE_SPECTRUM_ANALYZER:
			{
				//================== Spectrum Analyzer vs Signal Generator =====================//	
				if ( pStimulusDriverInfo->InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
				{
					ShowMessage ( INSTR_TYPE_CONTINUE , "Calibration Error","Stimulus must be signal generator.", NULL );
					{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
				}
				
				sprintf( szFormatedString , "%%s== Calibrating Signal Generator Factors with Spectrum Analyzer ==" );  
				STDF_COMMENT(0,szFormatedString,"%d");
				
				hSignalGenerator = StimulusHandle;
				hSpectrumAnalyzer = MeasureHandle;
				
				CHK_STDERR( DRV_SignalGenerator_Reset ( hSignalGenerator ));   
				CHK_STDERR( DRV_SpectrumAnalyzer_Reset ( hSpectrumAnalyzer ));
				
				if ( pMeasureDriverInfo->pCalibration->pStateFileName == NULL )
				{
					sprintf( szFormatedString , "Spectrum Analyzer %%s ::Loading state (%s)" , "0" );  
					STDF_COMMENT(0,szFormatedString,"%d");

					UPDATERR( DRV_SpectrumAnalyzer_Recall ( hSpectrumAnalyzer , "0" )); 
				}
				else
				{
					sprintf( szFormatedString , "Spectrum Analyzer %%s ::Loading state (%s)" , pMeasureDriverInfo->pCalibration->pStateFileName );  
					STDF_COMMENT(0,szFormatedString,"%d");

					UPDATERR( DRV_SpectrumAnalyzer_Recall ( hSpectrumAnalyzer , pMeasureDriverInfo->pCalibration->pStateFileName )); 
				}
				
				if ( IS_NOT_OK )
				{
					ShowMessage ( INSTR_TYPE_CONTINUE , "Calibration State","Press \"Local\" button on spectrum analyzer.\nPlease setup calibration configuration\n(span,reference level,..)", NULL );
					
					if ( pMeasureDriverInfo->pCalibration->pStateFileName == NULL )
						DRV_SpectrumAnalyzer_SaveState ( hSpectrumAnalyzer , "0" ); 
					else
						DRV_SpectrumAnalyzer_SaveState ( hSpectrumAnalyzer , pMeasureDriverInfo->pCalibration->pStateFileName ); 
				}
			
				if ( pStimulusDriverInfo->pCalibration->pStateFileName && ( strlen(pStimulusDriverInfo->pCalibration->pStateFileName)))
				{
					sprintf( szFormatedString , "Signal Generator %%s ::Loading state (%s)" , pStimulusDriverInfo->pCalibration->pStateFileName );  
					STDF_COMMENT(0,szFormatedString,"%d");

					UPDATERR( DRV_SignalGenerator_RecallState ( hSignalGenerator , pStimulusDriverInfo->pCalibration->pStateFileName )); 
				}
				
				for ( iPowerIndex = 0; iPowerIndex < iNumberOfPowers; iPowerIndex++ )
				{   
					if ( iPowerIndex )
					{
						if ( pStoreDriverInfo->pCalibration->vlfPowerList == NULL )
							break;
						
						if ( iPowerIndex > pStoreDriverInfo->pCalibration->iNumberOfPowers )
							break;
						
						if ( lfCurrentPower == pStoreDriverInfo->pCalibration->vlfPowerList[iPowerIndex-1] )
							break;
						
						lfCurrentPower = pStoreDriverInfo->pCalibration->vlfPowerList[iPowerIndex-1];	
					}
					
					sprintf( szFormatedString , "Signal Generator %%s :: Set power to %0.3lf dBm" , lfCurrentPower );  
					STDF_COMMENT(0,szFormatedString,"%d");
		
					CHK_STDERR_BREAK( DRV_SignalGenerator_SetPower ( hSignalGenerator , 0 , lfCurrentPower )); 
			
					CHK_STDERR_BREAK( DRV_SignalGenerator_SetFrequency ( hSignalGenerator , 0 , lfStimulusStartFrequency )); 
		
					if ( pStoreDriverInfo->pCalibration->lfAttenuator >= 0.0 )
					{
						CHK_STDERR( DRV_SpectrumAnalyzer_Attenuator( hSpectrumAnalyzer , ( pStoreDriverInfo->pCalibration->lfAttenuator )));
					}
						
					if (( iPowerIndex == 0 ) || ( hSpectrumAnalyzer != StoreHandle ))  
					{
						if ( pStoreDriverInfo->pCalibration->lfReferenceLevel > 20.0 )
						{
							CHK_STDERR( DRV_SpectrumAnalyzer_SetReferenceLevel( hSpectrumAnalyzer , ( lfCurrentPower )));
						}
						else
						{
							CHK_STDERR( DRV_SpectrumAnalyzer_SetReferenceLevel( hSpectrumAnalyzer , ( pStoreDriverInfo->pCalibration->lfReferenceLevel )));
						}
					}
					else
					{
						CHK_STDERR( DRV_SpectrumAnalyzer_Attenuator( hSpectrumAnalyzer , ( -1 ))); 
						CHK_STDERR( DRV_SpectrumAnalyzer_SetReferenceLevel( hSpectrumAnalyzer , ( lfCurrentPower )));
					}
					
					sprintf( szFormatedString , "Signal Generator %%s :: RF on" );  
		
					STDF_COMMENT(0,szFormatedString,"%d");
			
					CHK_STDERR( DRV_SignalGenerator_RF_On( hSignalGenerator , 0 ));
			
					CALLOC_ERR( pTraceData , iActualyPointsNumber , sizeof(double));

					lfFrequencyStep = (( lfStopFrequency - lfStartFrequency ) / ( iActualyPointsNumber - 1 ));
	
					lfCurrentFrequency = lfStartFrequency;
		
					for ( iIndex=0; iIndex < iActualyPointsNumber; iIndex++ )
					{
						if ( iProgressPanel && iProgressControl )
							SetCtrlVal( iProgressPanel , iProgressControl , ++iProgressCounter );
						
						if ( pStoreDriverInfo->pCalibration->vlfFrequency )
							lfCurrentFrequency = pStoreDriverInfo->pCalibration->vlfFrequency[iIndex];
				
						sprintf( szFormatedString , "Signal Generator %%s :: Set frequency %0.3lf MHz" , ( lfCurrentFrequency / 1E6 ));  
						STDF_COMMENT(0,szFormatedString,"%d");
		
						if ( lfCurrentFrequency < lfStimulusStartFrequency )
							continue;
						
						if ( lfCurrentFrequency > lfStimulusStopFrequency )
							break;
						
						CHK_STDERR_BREAK( DRV_SignalGenerator_SetFrequency ( hSignalGenerator , 0 , lfCurrentFrequency ));     
		
						sprintf( szFormatedString , "Spectrum Analyzer %%s :: Set frequency %0.3lf MHz" , ( lfCurrentFrequency / 1E6 ));  
						STDF_COMMENT(0,szFormatedString,"%d");
		
						CHK_STDERR_BREAK( DRV_SpectrumAnalyzer_SetFrequency ( hSpectrumAnalyzer , lfCurrentFrequency ));  
			
						for ( iTryIndex = 0; iTryIndex < 10; iTryIndex++ )
						{
							DelayWithEventProcessing(pStoreDriverInfo->pCalibration->lfMeasureDelay); 
							
							DRV_SpectrumAnalyzer_WaitForIntrumentDone( hSpectrumAnalyzer , 20.0 ); 
							
							DRV_SpectrumAnalyzer_InitiateMeasure( hSpectrumAnalyzer ); 

							DRV_SpectrumAnalyzer_WaitForIntrumentDone( hSpectrumAnalyzer , 20.0 );  
							
							CHK_STDERR( DRV_SpectrumAnalyzer_SearchMarkerPeak( hSpectrumAnalyzer , 1 ));  
			
							CHK_STDERR( DRV_SpectrumAnalyzer_GetMarkerAmplitude( hSpectrumAnalyzer , 1 , &lfGetMarker )); 

							if (( lfGetMarker < ( + 100.0 )) && ( lfGetMarker > ( - 200.0 )))
								break;
						}
			
						pTraceData[iIndex] = lfGetMarker - lfCurrentPower;
			
						if ( hSignalGenerator != StoreHandle )
						{
							DRIVER_MANAGER_CALIBRATION_Gel_3D_Factor( hSignalGenerator , lfCurrentPower , lfCurrentFrequency , &lfSignalGeneratorLossFactor );  
			
							if ( lfSignalGeneratorLossFactor != 0.0 )
							{
								sprintf( szFormatedString , "%%sSignal Generator Factor = %0.3lf dBm" , lfSignalGeneratorLossFactor );  
								STDF_COMMENT(0,szFormatedString,"%d");
							}																													  
			
							pTraceData[iIndex] -= lfSignalGeneratorLossFactor;
						}
					
						if ( hSpectrumAnalyzer != StoreHandle )  
						{
							DRIVER_MANAGER_CALIBRATION_Gel_3D_Factor( hSpectrumAnalyzer, lfCurrentPower , lfCurrentFrequency , &lfSpectrumAnalyzerLossFactor );  
			
							if ( lfSpectrumAnalyzerLossFactor != 0.0 )
							{
								sprintf( szFormatedString , "%%sSpectrum Analyzer Factor = %0.3lf dBm" , lfSpectrumAnalyzerLossFactor );  
								STDF_COMMENT(0,szFormatedString,"%d");
							}
			
							pTraceData[iIndex] -= lfSpectrumAnalyzerLossFactor;
						}	   
					
						sprintf( szFormatedString , "%%sMeasure Loss = %0.3lf dBm" , pTraceData[iIndex] );  
						STDF_COMMENT(0,szFormatedString,"%d");
					
						if ( pStoreDriverInfo->pCalibration->vlfFrequency == NULL ) 
							lfCurrentFrequency += lfFrequencyStep;
					}
	
					if ( iPowerIndex == 0 )
					{
						FREE(pStoreDriverInfo->pCalibration->vlfLoss);
						pStoreDriverInfo->pCalibration->vlfLoss = pTraceData;
			
						if ( pStoreDriverInfo->pCalibration->vlfFrequency == NULL ) 
						{
							FREE_CALLOC_ERR( pStoreDriverInfo->pCalibration->vlfFrequency , iActualyPointsNumber , sizeof(double));

							lfFrequencyStep = (( lfStopFrequency - lfStartFrequency ) / ( iActualyPointsNumber - 1 ));
	
							lfCurrentFrequency = lfStartFrequency;

							for ( iIndex=0; iIndex < iActualyPointsNumber; iIndex++ )
							{
								pStoreDriverInfo->pCalibration->vlfFrequency[iIndex] = lfCurrentFrequency;	
								lfCurrentFrequency += lfFrequencyStep;
							}
						}
				
						pStoreDriverInfo->pCalibration->iNumberOfPoints = iActualyPointsNumber;
						pStoreDriverInfo->pCalibration->lfStartFrequency = lfStartFrequency;
						pStoreDriverInfo->pCalibration->lfStopFrequency = lfStopFrequency;
					}
					else
					{
						pStoreDriverInfo->pCalibration->vlfLossList[iPowerIndex-1]= pTraceData;
						
					}
				}
			}
			break;
			
		case DRIVER_TYPE_POWER_METER:
			{
				//================== Power Metere vs Signal Generator =====================//	
				if ( pStimulusDriverInfo->InstrumentType != DRIVER_TYPE_SIGNAL_GENERATOR )
				{
					ShowMessage ( INSTR_TYPE_CONTINUE , "Calibration Error","Stimulus must be signal generator.", NULL );
					{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
				}
				
				sprintf( szFormatedString , "%%s== Calibrating Signal Generator Factors with Power Meter ==" );  
				STDF_COMMENT(0,szFormatedString,"%d");
				
				hSignalGenerator = StimulusHandle;
				hPowerMeter = MeasureHandle;
				
				CHK_STDERR( DRV_SignalGenerator_Reset ( hSignalGenerator ));   
				CHK_STDERR( DRV_PowerMeter_Reset ( hPowerMeter ));
				
				CHK_STDERR( DRV_PowerMeter_SetActiveChannel( hPowerMeter , 1 ));  
		
				CHK_STDERR( DRV_PowerMeter_SetMode ( hPowerMeter , 1 , 0 ));     
			
				CHK_STDERR( DRV_PowerMeter_SetActivePort( hPowerMeter , pMeasureDriverInfo->pCalibration->iCalibrationPortNumber ));  
			
				if ( pStimulusDriverInfo->pCalibration->pStateFileName )
				{
					sprintf( szFormatedString , "Signal Generator %%s ::Loading state (%s)" , pStimulusDriverInfo->pCalibration->pStateFileName );  
					STDF_COMMENT(0,szFormatedString,"%d");

					UPDATERR( DRV_SignalGenerator_RecallState ( hSignalGenerator , pStimulusDriverInfo->pCalibration->pStateFileName )); 
				}
				
				for ( iPowerIndex = 0; iPowerIndex < iNumberOfPowers; iPowerIndex++ )
				{
					if ( iPowerIndex )
					{
						if ( pStoreDriverInfo->pCalibration->vlfPowerList == NULL )
							break;
						
						if ( iPowerIndex > pStoreDriverInfo->pCalibration->iNumberOfPowers )
							break;
						
						if ( lfCurrentPower == pStoreDriverInfo->pCalibration->vlfPowerList[iPowerIndex-1] )
							break;
						
						lfCurrentPower = pStoreDriverInfo->pCalibration->vlfPowerList[iPowerIndex-1];	
					}
					
					sprintf( szFormatedString , "Signal Generator %%s :: Set  power to %0.3lf dBm" , lfCurrentPower );  
					STDF_COMMENT(0,szFormatedString,"%d");
				
					CHK_STDERR_BREAK( DRV_SignalGenerator_SetPower ( hSignalGenerator , 0 , lfCurrentPower )); 
			
					CHK_STDERR_BREAK( DRV_SignalGenerator_SetFrequency ( hSignalGenerator , 0 , pStoreDriverInfo->pCalibration->lfStartFrequency )); 
		
					sprintf( szFormatedString , "Signal Generator %%s :: RF on" );  
		
					STDF_COMMENT(0,szFormatedString,"%d");
			
					CHK_STDERR( DRV_SignalGenerator_RF_On( hSignalGenerator , 0 ));
			
					CALLOC_ERR( pTraceData , iActualyPointsNumber , sizeof(double));

					lfFrequencyStep = (( lfStopFrequency - lfStartFrequency ) / ( iActualyPointsNumber - 1 ));
	
					lfCurrentFrequency = lfStartFrequency;
		
					for ( iIndex=0; iIndex < iActualyPointsNumber; iIndex++ )
					{	 
						if ( iProgressPanel && iProgressControl )
							SetCtrlVal( iProgressPanel , iProgressControl , ++iProgressCounter );
						
						if ( pStoreDriverInfo->pCalibration->vlfFrequency )
							lfCurrentFrequency = pStoreDriverInfo->pCalibration->vlfFrequency[iIndex];
				
						sprintf( szFormatedString , "Signal Generator %%s :: Set frequency %0.3lf MHz" , ( lfCurrentFrequency / 1E6 ));  
						STDF_COMMENT(0,szFormatedString,"%d");
		
						CHK_STDERR_BREAK( DRV_SignalGenerator_SetFrequency ( hSignalGenerator , 0 , lfCurrentFrequency ));     
		
						sprintf( szFormatedString , "Power Meter %%s :: Set frequency %0.3lf MHz" , ( lfCurrentFrequency / 1E6 ));  
						STDF_COMMENT(0,szFormatedString,"%d");
		
						CHK_STDERR_BREAK( DRV_PowerMeter_SetFrequency ( hPowerMeter , pMeasureDriverInfo->pCalibration->iCalibrationPortNumber , lfCurrentFrequency )); 
					
						for ( iTryIndex = 0; iTryIndex < 10; iTryIndex++ )
						{
							DelayWithEventProcessing(pStoreDriverInfo->pCalibration->lfMeasureDelay);  
							
							CHK_STDERR( DRV_PowerMeter_InitSweep( hPowerMeter , 1 )); 
							
							CHK_STDERR( DRV_PowerMeter_ReadCWpower ( hPowerMeter , 1 , 10.0 , &lfGetPower ));      
			
							if (( lfGetPower < ( + 100.0 )) && ( lfGetPower > ( - 200.0 )))
								break;
						}
			
						pTraceData[iIndex] = lfGetPower - lfCurrentPower;   
					
						if ( hSignalGenerator != StoreHandle )
						{
							DRIVER_MANAGER_CALIBRATION_Gel_3D_Factor( hSignalGenerator , lfCurrentPower , lfCurrentFrequency , &lfSignalGeneratorLossFactor );  
			
							if ( lfSignalGeneratorLossFactor != 0.0 )
							{
								sprintf( szFormatedString , "%%sSignal Generator Factor = %0.3lf dBm" , lfSignalGeneratorLossFactor );  
								STDF_COMMENT(0,szFormatedString,"%d");
							}																													  
			
							pTraceData[iIndex] -= lfSignalGeneratorLossFactor;
						}
					
						if ( hPowerMeter != StoreHandle )  
						{
							DRIVER_MANAGER_CALIBRATION_Gel_3D_Factor( hPowerMeter , lfCurrentPower , lfCurrentFrequency , &lfPowerMeterLossFactor );  
			
							if ( lfPowerMeterLossFactor != 0.0 )
							{
								sprintf( szFormatedString , "%%sPower Meter Factor = %0.3lf dBm" , lfPowerMeterLossFactor );  
								STDF_COMMENT(0,szFormatedString,"%d");
							}
			
							pTraceData[iIndex] -= lfPowerMeterLossFactor;
						}
					
						sprintf( szFormatedString , "%%sMeasure Loss = %0.3lf dBm" , pTraceData[iIndex] );  
						STDF_COMMENT(0,szFormatedString,"%d");
					
						if ( pStoreDriverInfo->pCalibration->vlfFrequency == NULL ) 
							lfCurrentFrequency += lfFrequencyStep;
					}
	
					if ( iPowerIndex == 0 )
					{
						FREE(pStoreDriverInfo->pCalibration->vlfLoss);
						pStoreDriverInfo->pCalibration->vlfLoss = pTraceData;
			
						if ( pStoreDriverInfo->pCalibration->vlfFrequency == NULL ) 
						{
							FREE_CALLOC_ERR( pStoreDriverInfo->pCalibration->vlfFrequency , iActualyPointsNumber , sizeof(double));

							lfFrequencyStep = (( lfStopFrequency - lfStartFrequency ) / ( iActualyPointsNumber - 1 ));
	
							lfCurrentFrequency = lfStartFrequency;

							for ( iIndex=0; iIndex < iActualyPointsNumber; iIndex++ )
							{
								pStoreDriverInfo->pCalibration->vlfFrequency[iIndex] = lfCurrentFrequency;	
								lfCurrentFrequency += lfFrequencyStep;
							}
						}
				
						pStoreDriverInfo->pCalibration->iNumberOfPoints = iActualyPointsNumber;
						pStoreDriverInfo->pCalibration->lfStartFrequency = lfStartFrequency;
						pStoreDriverInfo->pCalibration->lfStopFrequency = lfStopFrequency;
					}
					else
					{
						pStoreDriverInfo->pCalibration->vlfLossList[iPowerIndex-1]= pTraceData;
						
					}
				}
			}
			
		break;
		
		case DRIVER_TYPE_NETWORK_ANALYZER:
			
			{
				//================== Network Analyzer =====================//	
				if ( pStimulusDriverInfo->InstrumentType != DRIVER_TYPE_NETWORK_ANALYZER )
				{
					ShowMessage ( INSTR_TYPE_CONTINUE , "Calibration Error","Stimulus must be from Network Analyzer.", NULL );
					{STD_ERR (DRV_ERROR_INCORRECT_DRIVER_TYPE);}
				}
				
				if ( pMeasureDriverInfo->pCalibration->pStateFileName )
				{
					sprintf( szFormatedString , "Network Analyzer %%s :: Loading state (%s)" , pMeasureDriverInfo->pCalibration->pStateFileName );   
					STDF_COMMENT(0,szFormatedString,"%d");  
	
					CHK_STDERR( DRV_NetworkAnalyzer_ReCallRegister( StimulusHandle , pMeasureDriverInfo->pCalibration->pStateFileName ));   
				}
				
				CHK_STDERR( DRV_NetworkAnalyzer_GetPointsNum( StimulusHandle , &iActualyPointsNumber ));  
		
				CHK_STDERR( DRV_NetworkAnalyzer_GetFrequency( StimulusHandle , &lfStartFrequency, &lfStopFrequency )); 
	
				DelayWithEventProcessing(pStoreDriverInfo->pCalibration->lfMeasureDelay); 
	
				sprintf( szFormatedString , "Network Analyzer %%s :: Reading Trace..." );   
				STDF_COMMENT(0,szFormatedString,"%d"); 
	
				CHK_STDERR( DRV_NetworkAnalyzer_GetTrace ( StimulusHandle , pMeasureDriverInfo->pCalibration->iCalibrationPortNumber , 0 , 0 , pMeasureDriverInfo->pCalibration->lfMeasureTimeout , &pFrequencyData , &pTraceData , &iActualyPointsNumber ));
				
				FREE(pStoreDriverInfo->pCalibration->vlfFrequency);
				pStoreDriverInfo->pCalibration->vlfFrequency = pFrequencyData;
				
				FREE(pStoreDriverInfo->pCalibration->vlfLoss);
				pStoreDriverInfo->pCalibration->vlfLoss = pTraceData;
	
				pStoreDriverInfo->pCalibration->iNumberOfPoints = iActualyPointsNumber;
			}
				
		break; 	
	}
	
Error:	
	
	if ( hSignalGenerator )
		DRV_SignalGenerator_RF_Off( hSignalGenerator , 0 );

	if ( StoreHandle )
		CmtReleaseTSVPtr ( StoreHandle ); 
	
	if ( MeasureHandle && ( MeasureHandle != StoreHandle))
		CmtReleaseTSVPtr ( MeasureHandle ); 
	
	if ( StimulusHandle && ( StimulusHandle != StoreHandle))
		CmtReleaseTSVPtr ( StimulusHandle ); 
	
	return StdError;			
}


int	DLLEXPORT	DRIVER_MANAGER_Get_Number_Of_Channels( int Handle , int *piNumberOfChannels )
{
	STD_ERROR									StdError									=	{0};

	tsDriverInfo								*pDriverInfo								=	NULL;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( piNumberOfChannels )
		*piNumberOfChannels = pDriverInfo->ChannelQuantity;

Error:	
	if ( Handle )
		CmtReleaseTSVPtr ( Handle ); 
	
	RETURN_ERROR_VALUE;			
}

int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_Port_Number( int Handle , int iCalibrationPortNumber )
{
	STD_ERROR									StdError									=	{0};

	tsDriverInfo								*pDriverInfo								=	NULL;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( pDriverInfo->pCalibration == NULL )
	{
		CALLOC_ERR(	pDriverInfo->pCalibration ,1 ,sizeof(tsCalibrationItem));
	}
	
	pDriverInfo->pCalibration->iCalibrationPortNumber = iCalibrationPortNumber;

Error:	
	if ( Handle )
		CmtReleaseTSVPtr ( Handle ); 
	
	RETURN_ERROR_VALUE;			
}

int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_Power( int Handle , double lfSetPower )
{
	STD_ERROR									StdError									=	{0};

	tsDriverInfo								*pDriverInfo								=	NULL;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( pDriverInfo->pCalibration == NULL )
	{
		CALLOC_ERR(	pDriverInfo->pCalibration ,1 ,sizeof(tsCalibrationItem));
	}
	
	pDriverInfo->pCalibration->lfSetPower = lfSetPower;
	
Error:	
	if ( Handle )
		CmtReleaseTSVPtr ( Handle ); 
	
	RETURN_ERROR_VALUE;			
}

int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_ReferenceLevel( int Handle , double lfReferenceLevel )
{
	STD_ERROR									StdError									=	{0};

	tsDriverInfo								*pDriverInfo								=	NULL;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( pDriverInfo->pCalibration == NULL )
	{
		CALLOC_ERR(	pDriverInfo->pCalibration ,1 ,sizeof(tsCalibrationItem));
	}
	
	pDriverInfo->pCalibration->lfReferenceLevel = lfReferenceLevel;

Error:	
	if ( Handle )
		CmtReleaseTSVPtr ( Handle ); 
	
	RETURN_ERROR_VALUE;			
}

int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_Attenuator( int Handle , double lfAttenuator )
{
	STD_ERROR									StdError									=	{0};

	tsDriverInfo								*pDriverInfo								=	NULL;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( pDriverInfo->pCalibration == NULL )
	{
		CALLOC_ERR(	pDriverInfo->pCalibration ,1 ,sizeof(tsCalibrationItem));
	}
	
	pDriverInfo->pCalibration->lfAttenuator = lfAttenuator;

Error:	
	if ( Handle )
		CmtReleaseTSVPtr ( Handle ); 
	
	RETURN_ERROR_VALUE;			
}

int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_MeasureDelay( int Handle , double lfMeasureDelay )
{
	STD_ERROR									StdError									=	{0};

	tsDriverInfo								*pDriverInfo								=	NULL;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( pDriverInfo->pCalibration == NULL )
	{
		CALLOC_ERR(	pDriverInfo->pCalibration ,1 ,sizeof(tsCalibrationItem));
	}
	
	pDriverInfo->pCalibration->lfMeasureDelay = lfMeasureDelay;

Error:	
	if ( Handle )
		CmtReleaseTSVPtr ( Handle ); 
	
	RETURN_ERROR_VALUE;			
}

int	DLLEXPORT	DRIVER_MANAGER_Set_Calibration_StateFileName( int Handle , char *pStateFileName )
{
	STD_ERROR									StdError									=	{0};

	tsDriverInfo								*pDriverInfo								=	NULL;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( pDriverInfo->pCalibration == NULL )
	{
		CALLOC_ERR(	pDriverInfo->pCalibration ,1 ,sizeof(tsCalibrationItem));
	}
	
	if ( pStateFileName )
	{
		CALLOC_COPY_STRING( pDriverInfo->pCalibration->pStateFileName , pStateFileName );
	}

Error:	
	if ( Handle )
		CmtReleaseTSVPtr ( Handle ); 
	
	RETURN_ERROR_VALUE;			
}


int	DLLEXPORT	DRIVER_MANAGER_Get_SimulationMode( int Handle , int *piSimulationMode )
{
	STD_ERROR									StdError									=	{0};

	tsDriverInfo								*pDriverInfo								=	NULL;
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( piSimulationMode )
	{
		*piSimulationMode = pDriverInfo->bDemoMode;
	}

Error:	
	if ( Handle )
		CmtReleaseTSVPtr ( Handle ); 
	
	RETURN_ERROR_VALUE;			
}
 
STD_ERROR		DRIVER_MANAGER_CALIBRATION_GetCalibrationRangeInfo( int Handle , double *plfStart , double *plfStop , int *piNumberOfPoints ) 
{
	STD_ERROR									StdError								=	{0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;

	int											iNumberOfPoints							=	0;
												
	
	if ( Handle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	if ( CmtGetTSVPtr ( Handle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	if ( pDriverInfo->pCalibration )	
	{
		iNumberOfPoints = pDriverInfo->pCalibration->iNumberOfPoints;
	
		if ( pDriverInfo->pCalibration->vlfFrequency )
		{
			if ( plfStart )
				*plfStart = pDriverInfo->pCalibration->vlfFrequency[0];
																	   
			if ( plfStop )
				*plfStop = pDriverInfo->pCalibration->vlfFrequency[iNumberOfPoints-1];
		}
		
		if ( piNumberOfPoints )
			*piNumberOfPoints = iNumberOfPoints;
	}
	
Error:	
	
	CmtReleaseTSVPtr ( Handle ); 
	
	return StdError;			
}


STD_ERROR		DRIVER_MANAGER_CALIBRATION_CreateEmptyHandle( int *pHandle ) 
{
	STD_ERROR                                   StdError                                =   {0};
	
	tsDriverInfo								*pDriverInfo							=	NULL;
	
	CmtTSVHandle 								VariableHandle							=	0;

	if ( CmtNewTSV ( sizeof(tsDriverInfo) , &VariableHandle ) != 0 )
		{STD_ERR (DRV_ERROR_CREATE_TSV_HANDLE);}

	if ( VariableHandle == 0 )
		{STD_ERR (DRV_ERROR_CREATE_TSV_HANDLE);}
	
	if ( pHandle )
		*pHandle = VariableHandle;
	
	IF (( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 ) , "Can't create handle." );

Error:	
	
	if ( VariableHandle )
		CmtReleaseTSVPtr ( VariableHandle ); 
	
	return StdError;			
}

STD_ERROR		DRIVER_MANAGER_CALIBRATION_DiscardHandle( int *pHandle ) 
{							
	STD_ERROR									StdError								=	{0};
								
	tsDriverInfo								*pDriverInfo							=	NULL,
												tDriverInfo								=	{0};
	
	CmtTSVHandle 								VariableHandle							=	0;
	
	CmtThreadLockHandle 						LockHandle								=	0;
										
	pfSignalGenerator_Close						pWrapperFunction						=	NULL;
	
	int											bHandleExists							=	0;
	
	int											bLocked									=	0;
	
	if ( pHandle == NULL )
		{STD_ERR (DRV_ERROR_PASSED_NULL);}
	
	VariableHandle = *pHandle;
	*pHandle = 0;
	
	if ( VariableHandle == 0 )
		{STD_ERR (DRV_ERROR_PASSED_NULL);} 
	
	if ( CmtGetTSVPtr ( VariableHandle , &pDriverInfo ) < 0 )
		{STD_ERR (DRV_ERROR_GET_TSV_POINTER);}
	
	memcpy( &tDriverInfo , pDriverInfo , sizeof(tsDriverInfo));

	FREE( pDriverInfo->pLastStateFileName );

	CmtReleaseTSVPtr ( VariableHandle ); 
	
	LockHandle = tDriverInfo.InstrumentLockHandle;
	
	pWrapperFunction = tDriverInfo.tInstrDB.signalGeneratorDriverFunctions.SignalGenerator_Close;
	
	bHandleExists = DRIVER_MANAGER_IsConnectionExists( tDriverInfo.pInstrumentAddress , NULL , NULL );

	if ( pWrapperFunction && bHandleExists )
	{
		CHK_PROCCESS_GET_LOCK ( LockHandle );   
		
		if ( pDriverInfo->bDemoMode == 0 )
		{
			FREE_STDERR_COPY( pWrapperFunction( &tDriverInfo.InstrumentHandle )); 
		}
	
		CmtReleaseLock ( LockHandle ); 
		LockHandle=0;
		bLocked=0;
		
		DRIVER_MANAGER_RemoveConnectionExists( tDriverInfo.pInstrumentAddress , tDriverInfo.InstrumentHandle );
	}
	
	FREE(tDriverInfo.pInstrumentAddress); 
	FREE(tDriverInfo.pDriverFileName);
	
	if ( tDriverInfo.pCalibration )
	{
		FREE(tDriverInfo.pCalibration->pStateFileName);
		FREE(tDriverInfo.pCalibration->pCalibrationRefName);
		FREE(tDriverInfo.pCalibration->pConnectionDescription); 
		FREE(tDriverInfo.pCalibration->pConnectionImage); 
		FREE(tDriverInfo.pCalibration->vlfFrequency); 
		FREE(tDriverInfo.pCalibration->vlfLoss); 
		
		FREE(tDriverInfo.pCalibration->vlfPowerList);
		FREE_LIST(tDriverInfo.pCalibration->vlfLossList,tDriverInfo.pCalibration->iNumberOfPowers);
		
		FREE(tDriverInfo.pCalibration);
	}  
	
	if ( tDriverInfo.ptCallbacks )
	{
		FREE( (tDriverInfo.ptCallbacks)->pCommentCallbackData );
		FREE( (tDriverInfo.ptCallbacks)->pConfigValueCallbackData );
		FREE( (tDriverInfo.ptCallbacks)->pCheckForBreakCallbackData ); 
		FREE( (tDriverInfo.ptCallbacks)->pFileCallbackData );
		
		FREE(tDriverInfo.ptCallbacks);
	}
	
Error:
	
	if ( LockHandle && bLocked )
		CmtReleaseLock ( LockHandle ); 

	if ( VariableHandle )
		CmtDiscardTSV ( VariableHandle ); 
	
	FreeLibrary( tDriverInfo.LibraryHandle );
	
	*pHandle = 0;
	
	if ( StdError.error )
	{FREE_CALLOC_COPY_STRING (StdError.pszErrorDescription, "Error while closing instrument driver.");}
	
	return StdError;
}
