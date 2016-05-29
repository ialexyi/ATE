#ifndef __INI_STD_Storage_H__
#define __INI_STD_Storage_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

void*	DLLEXPORT       INI_STD_Storage_Init(char *pszFileNamePath, int *pHandle);
void*	DLLEXPORT       INI_STD_Storage_Close(int *pHandle);

int		DLLEXPORT       INI_STD_Storage_FillCallbacksDataStructure(int handle, void **pCallBackStructure);

void* DLLEXPORT INI_STD_Storage_Save ( int handle );
void* DLLEXPORT INI_STD_Storage_Store_Integer ( int handle , char *pszVeriableName , int value );
void* DLLEXPORT INI_STD_Storage_Restore_Integer ( int handle , char *pszVeriableName , int *pValue );
void* DLLEXPORT INI_STD_Storage_Store_Double ( int handle , char *pszVeriableName , double value );
void* DLLEXPORT INI_STD_Storage_Restore_Double ( int handle , char *pszVeriableName , double *pValue );
void* DLLEXPORT INI_STD_Storage_Store_String ( int handle , char *pszVeriableName , char *pString );
void* DLLEXPORT INI_STD_Storage_Restore_String ( int handle , char *pszVeriableName , char **pString );

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __INI_STD_Storage_H__ */
