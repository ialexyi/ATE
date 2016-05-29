#ifndef __GBL_CommonSection_H__
#define __GBL_CommonSection_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"
#include "GlobalDefines.h"

//==============================================================================
// Constants
#define			INSTR_TYPE_CONTINUE							0
#define			INSTR_TYPE_YES_NO							1	
//==============================================================================
// Types

typedef enum{	EQUIP_TYPE_DV = EQUIPMENT_TYPE_STANDARD_DEVICE,\
				EQUIP_TYPE_PS = EQUIPMENT_TYPE_POWER_SUPPLY,\
				EQUIP_TYPE_NA = EQUIPMENT_TYPE_NETWORK_ANALYZER,\
				EQUIP_TYPE_SG = EQUIPMENT_TYPE_SIGNAL_GENERATOR,\
				EQUIP_TYPE_SA = EQUIPMENT_TYPE_SPECTRUM_ANALYZER,\
				EQUIP_TYPE_SC = EQUIPMENT_TYPE_OSCILLOSCOPE,\
				EQUIP_TYPE_FG = EQUIPMENT_TYPE_FUNCTION_GENERATOR,\
				EQUIP_TYPE_PM = EQUIPMENT_TYPE_POWER_METER,\
				EQUIP_TYPE_NF = EQUIPMENT_TYPE_NOISE_FIGURE,\
				EQUIP_TYPE_MM = EQUIPMENT_TYPE_MULTI_METER,\
				EQUIP_TYPE_WA = EQUIPMENT_TYPE_WAVE_FORM_ANALYZER } teEquipmentTypes;
		
//==============================================================================
// External variables

//==============================================================================
// Global functions

STD_ERROR   DLLEXPORT	DRIVER_MANAGER_GetEquipmentInformation( int hHandle , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware );

int	DLLEXPORT	DRIVER_MANAGER_GetDriversFileList( char *pRootDirectory , char ***pDriversList , int **pDriversTypes , int *pCount );

STD_ERROR	DLLEXPORT	DRIVER_MANAGER_CheckEquipmentDriverCompatibility( char *pszDriverLocation , char *pszAddressString , int *pStatus , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware , char **pIDN , char **pID_Responds );

int  ShowMessage ( int type , char *pTitle , char *pText, char *pPicture );

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __GBL_CommonSection_H__ */
