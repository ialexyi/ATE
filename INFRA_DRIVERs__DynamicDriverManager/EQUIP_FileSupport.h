#ifndef __EQUIP_FileSupport_H__
#define __EQUIP_FileSupport_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"

//==============================================================================
// Constants
#include "GlobalDefines.h"

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

STD_ERROR   DLLEXPORT	DRIVER_MANAGER_Equipment_SendStateFile ( int hHandle , char *szLocalFileName ,char *szStateFileName );
STD_ERROR   DLLEXPORT	DRIVER_MANAGER_Equipment_ReceiveStateFile ( int hHandle , char *szLocalFileName ,char *szStateFileName );
STD_ERROR   DLLEXPORT	DRIVER_MANAGER_Equipment_GetFileCatalog ( int hHandle , char *szPath , char ***szFileNames , int **pvFileSizes , int *piNumberOfFiles );
STD_ERROR   DLLEXPORT	DRIVER_MANAGER_Equipment_MakeFileCatalog ( int hHandle , char *szPath );
STD_ERROR   DLLEXPORT	DRIVER_MANAGER_Equipment_DeleteFileCatalog ( int hHandle , char *szCatalogName );
STD_ERROR   DLLEXPORT	DRIVER_MANAGER_Equipment_DeleteStateFile ( int hHandle , char *szStateFileName );

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __EQUIP_FileSupport_H__ */
