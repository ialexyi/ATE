//==============================================================================
// Include files
#include <windows.h>
#include <ansi_c.h>
#include <utility.h>
#include "INI_STD_Storage.h"
#include "inifile.h"
#include "GlobalDefines.h"

//==============================================================================
// Constants

//==============================================================================
// Types
typedef enum { VALUE_TYPE_INT , VALUE_TYPE_DOUBLE , VALUE_TYPE_STRING , VALUE_TYPE_IMAGE , VALUE_TYPE_INT_ARRAY , VALUE_TYPE_DOUBLE_ARRAY , VALUE_TYPE_SENTINEL } teValueType;

typedef struct
{	
	IniText		iniFileHandle;

	char		*pszFileNamePath;
	
} tsHandle;

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
				return 0;	  /* out of memory */
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



void* DLLEXPORT INI_STD_Storage_Init ( char *pszFileNamePath , int *pHandle )
{
	STD_ERROR						StdError										=	{0};
	
	tsHandle						*pLocalHandle									=	{0};
	
	CmtTSVHandle 					handle											=	0;


	CHK_CMT ( CmtNewTSV ( sizeof(tsHandle) , &handle ));

	if ( pHandle )
		*pHandle = handle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));

	if ( pszFileNamePath == NULL )
	{
		CALLOC_COPY_STRING_ERR( pLocalHandle->pszFileNamePath , "local_storage.ini" ); 
	}
	else
	{
		CALLOC_COPY_STRING_ERR( pLocalHandle->pszFileNamePath , pszFileNamePath ); 
	}
	
	pLocalHandle->iniFileHandle = Ini_New (TRUE); // TRUE for automatic sorting
	
	IF (( pLocalHandle->iniFileHandle == NULL ) , "Can't open storage file." );
	
	if( FileExists( pLocalHandle->pszFileNamePath , NULL ))
		Ini_ReadFromFile ( pLocalHandle->iniFileHandle , pLocalHandle->pszFileNamePath ); 
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER;
}


void* DLLEXPORT INI_STD_Storage_Close ( int *pHandle )
{
	STD_ERROR		StdError						=	{0};
	
	tsHandle		*pLocalHandle					=	{0};

	int				handle							=	0;
	
	IF (( pHandle == NULL ) , "Handle is empty" );
	
	handle = *pHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if ( pLocalHandle->iniFileHandle )
	{
		if ( pLocalHandle->pszFileNamePath && ( strlen(pLocalHandle->pszFileNamePath)))
		{
			Ini_WriteToFile( pLocalHandle->iniFileHandle , pLocalHandle->pszFileNamePath );
		}
		
		Ini_Dispose ( pLocalHandle->iniFileHandle );
	}
	
	CmtReleaseTSVPtr ( handle ); 
	
	CmtDiscardTSV (handle);
	
	*pHandle = 0;
	
Error:

	RETURN_STDERR_POINTER;
}


void DLLEXPORT clbCommentCallback( int iMessageType ,  char *pszText, void *pData )
{
	if ( pszText == NULL )
		return;

	OutputDebugString( pszText );
	OutputDebugString( "\n" ); 
	
	return;
}

void  DLLEXPORT clbConfigValueCallback( unsigned long long ullDataBaseLinkID , unsigned long long ullEquipmentHandle , int iMultiIndex , char *szModuleName , char *szValueName , int ValueType , int iValueSize , void *pValue , void *pDefaultValue , void *pData )
{
	STD_ERROR						StdError											=	{0};

	int								*pHandle											=	NULL;

	int								handle												=	0;
	
	teValueType						valueType;

	tsHandle						*pLocalHandle										=	{0};
	
	char							szVeriableName[STD_STRING]							=	{0},
									szSectionName[LOW_STRING]							=	{0};
									
	int								iGetIntVariable										=	0;
	
	double							lfGetDoubleVariable									=	0.0;
	
	char							*pGetStringVariable									=	NULL;
	
	if ( pData == NULL )
		return;
	
	if ( szValueName == NULL )
		return;
	
	if (( ValueType < 0 ) || ( ValueType >= VALUE_TYPE_SENTINEL ))
		return;
	
	sprintf( szVeriableName , "Config_%s" , szValueName );
		
	valueType = ValueType;
	
	pHandle = pData; 
	handle = *pHandle;
	
	if ( handle == 0 )
		return;
	
	if ( szModuleName && strlen(szModuleName) )
	{
		strcpy( szSectionName , szModuleName );
	}
	else
	{
		if ( ullEquipmentHandle > 0 )
			sprintf( szSectionName , "%ld" , ullEquipmentHandle );
		else
			sprintf( szSectionName , "default" );
	}
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));

	if ( Ini_SectionExists ( pLocalHandle->iniFileHandle , szSectionName ) == 0 )
	{
		Ini_PutString ( pLocalHandle->iniFileHandle , szSectionName , "default" , "" );	
	}
			 
	switch( valueType )
	{
		case VAL_INTEGER:
			
			if ( Ini_ItemExists ( pLocalHandle->iniFileHandle , szSectionName , szVeriableName ) == 0 )
			{
				if ( pDefaultValue )
					Ini_PutInt ( pLocalHandle->iniFileHandle , szSectionName , szVeriableName , *(int*)pDefaultValue );
			}   
			else
			{
				Ini_GetInt ( pLocalHandle->iniFileHandle , szSectionName , szVeriableName , &iGetIntVariable ); 
				
				memcpy( pValue , &iGetIntVariable , (iValueSize * sizeof(int))); 
			}
			
				
			break;
		case VAL_DOUBLE:
			
			if ( Ini_ItemExists ( pLocalHandle->iniFileHandle , szSectionName , szVeriableName ) == 0 )
			{
				if ( pDefaultValue )
					Ini_PutDouble ( pLocalHandle->iniFileHandle , szSectionName , szVeriableName , *(double*)pDefaultValue );
			}   
			else
			{
				Ini_GetDouble ( pLocalHandle->iniFileHandle , szSectionName , szVeriableName , &lfGetDoubleVariable ); 
				
				memcpy( pValue , &lfGetDoubleVariable , (iValueSize * sizeof(double))); 
			}
			
			break;
		case VAL_STRING:
			
			if ( Ini_ItemExists ( pLocalHandle->iniFileHandle , szSectionName , szVeriableName ) == 0 )
			{
				if ( pDefaultValue )
					Ini_PutString ( pLocalHandle->iniFileHandle , szSectionName , szVeriableName , (char*)pDefaultValue );
			}   
			else
			{
				Ini_GetPointerToString ( pLocalHandle->iniFileHandle , szSectionName , szVeriableName , &pGetStringVariable ); 
				
				if ( pGetStringVariable )
				{
					memcpy( pValue , pGetStringVariable , strlen(pGetStringVariable)); 
					//FREE(pGetIntVariable);
				}
			}
			break;
			
		default:
			break;
	}
	
Error:  
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return;
}

	  
int	 DLLEXPORT INI_STD_Storage_FillCallbacksDataStructure( int handle , void	**pCallBackStructure )
{
	STD_ERROR						StdError											=	{0};
		
	int								*pSharedPointer										=	NULL;
	
	tsSTD_CallBackSet				*pSTD_CallBackSet									=	NULL;
	
	IF (( pCallBackStructure == NULL ) , "NULL Pointer"); 
  
	if ( *pCallBackStructure == NULL )
	{
		CALLOC_ERR( pSTD_CallBackSet , 1 , sizeof(tsSTD_CallBackSet));
	
		*pCallBackStructure = pSTD_CallBackSet;
	}
	else
		pSTD_CallBackSet = *pCallBackStructure;
	
	CALLOC_ERR( pSharedPointer , 1 , sizeof(int));
	*pSharedPointer = handle;
	
	pSTD_CallBackSet->fCommentCallback = clbCommentCallback;
	pSTD_CallBackSet->pCommentCallbackData = pSharedPointer;

	CALLOC_ERR( pSharedPointer , 1 , sizeof(int));
	*pSharedPointer = handle;
	
	pSTD_CallBackSet->fConfigValueCallback = clbConfigValueCallback;
	pSTD_CallBackSet->pConfigValueCallbackData = pSharedPointer;
 	pSTD_CallBackSet->configType = 0;
	pSTD_CallBackSet->iEquipmentHandle = 0;
	
	CALLOC_ERR( pSharedPointer , 1 , sizeof(int));
	*pSharedPointer = handle;
	
Error:
		
	return 0;  
}   


///--------------------------------- Local Interface ----------------------------------------------//

void* DLLEXPORT INI_STD_Storage_Save ( int handle )
{
	STD_ERROR		StdError						=	{0};
	
	tsHandle		*pLocalHandle					=	{0};
	
	if ( handle == 0 )
		return NULL;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if ( pLocalHandle->iniFileHandle )
	{
		if ( pLocalHandle->pszFileNamePath && ( strlen(pLocalHandle->pszFileNamePath)))
		{
			Ini_WriteToFile( pLocalHandle->iniFileHandle , pLocalHandle->pszFileNamePath );
		}
	}
	
Error:  
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER;
}


void* DLLEXPORT INI_STD_Storage_Store_Integer ( int handle , char *pszVeriableName , int value )
{
	STD_ERROR		StdError						=	{0};
	
	tsHandle		*pLocalHandle					=	{0};
	
	if ( handle == 0 )
		return NULL;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if (( pLocalHandle->iniFileHandle ) && pszVeriableName )
	{
		Ini_PutInt ( pLocalHandle->iniFileHandle , "Global" , pszVeriableName , value );
	}
	
Error:  
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT INI_STD_Storage_Restore_Integer ( int handle , char *pszVeriableName , int *pValue )
{
	STD_ERROR		StdError						=	{0};
	
	tsHandle		*pLocalHandle					=	{0};
	
	if ( handle == 0 )
		return NULL;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if (( pLocalHandle->iniFileHandle ) && pszVeriableName && pValue )
	{
		Ini_GetInt ( pLocalHandle->iniFileHandle , "Global" , pszVeriableName , pValue ); 
	}
	
Error:  
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER;
}


void* DLLEXPORT INI_STD_Storage_Store_Double ( int handle , char *pszVeriableName , double value )
{
	STD_ERROR		StdError						=	{0};
	
	tsHandle		*pLocalHandle					=	{0};
	
	if ( handle == 0 )
		return NULL;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if (( pLocalHandle->iniFileHandle ) && pszVeriableName )
	{
		Ini_PutDouble ( pLocalHandle->iniFileHandle , "Global" , pszVeriableName , value );
	}
	
Error:  
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT INI_STD_Storage_Restore_Double ( int handle , char *pszVeriableName , double *pValue )
{
	STD_ERROR		StdError						=	{0};
	
	tsHandle		*pLocalHandle					=	{0};
	
	if ( handle == 0 )
		return NULL;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if (( pLocalHandle->iniFileHandle ) && pszVeriableName && pValue )
	{
		Ini_GetDouble ( pLocalHandle->iniFileHandle , "Global" , pszVeriableName , pValue ); 
	}
	
Error:  
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT INI_STD_Storage_Store_String ( int handle , char *pszVeriableName , char *pString )
{
	STD_ERROR		StdError						=	{0};
	
	tsHandle		*pLocalHandle					=	{0};
	
	if ( handle == 0 )
		return NULL;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if (( pLocalHandle->iniFileHandle ) && pszVeriableName && pString )
	{
		Ini_PutString ( pLocalHandle->iniFileHandle , "Global" , pszVeriableName , pString );
	}
	
Error:  
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER;
}

void* DLLEXPORT INI_STD_Storage_Restore_String ( int handle , char *pszVeriableName , char **pString )
{
	STD_ERROR		StdError						=	{0};
	
	tsHandle		*pLocalHandle					=	{0};
	
	if ( handle == 0 )
		return NULL;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if (( pLocalHandle->iniFileHandle ) && pszVeriableName && pString )
	{
		Ini_GetStringCopy ( pLocalHandle->iniFileHandle , "Global" , pszVeriableName , pString ); 
	}
	
Error:  
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER;
}


