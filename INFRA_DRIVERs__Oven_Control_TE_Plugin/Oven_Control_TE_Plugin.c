//==============================================================================
// Include files
#include "Windows.h"
#include "toolbox.h"
#include <ansi_c.h>
#include <userint.h>
#include <utility.h>
#include "Oven_Control_TE_Plugin.h"
#include "STD_DriversManager.h"
#include "GlobalDefines.h"

//==============================================================================
// Constants
#define			SET_TEMPERATURE_COMMAND_NAME		"SET_TEMPERATURE"
#define			GET_TEMPERATURE_COMMAND_NAME		"GET_TEMPERATURE"
#define			READ_TEMPERATURE_COMMAND_NAME		"READ_TEMPERATURE"
#define			START_OVEN_COMMAND_NAME				"START_OVEN"
#define			STOP_OVEN_COMMAND_NAME				"STOP_OVEN"
#define			SET_SLOPE_COMMAND_NAME				"SET_SLOPE"
#define			INIT_CONTROL_COMMAND_NAME			"INIT_CONTROL"
#define			GET_CHAMBER_STATUS_COMMAND_NAME		"GET_CHAMBER_STATUS"
#define			CLOSE_CONTROL_COMMAND_NAME			"CLOSE_CONTROL"
#define			GET_SLOPE_COMMAND_NAME				"GET_SLOPE"


//==============================================================================
// Types
typedef struct
{				
	int					bDriverLoadedSubDriver;	
	
	int					handle;
	
} tsHandle;

typedef struct
{
	tsHandle			vHandles[1024];
	
	int					count;
	
} tsHandleList;

//==============================================================================
// Static global variables
int		volatile		gHandleListHandle									=	0;
//==============================================================================
// Static functions

//==============================================================================
// Global variables


//==============================================================================
// Global functions
int	DLLEXPORT	EXT_TRIG_TemperatureModuleClose( int *pHandle );

//==============================================================================
// DLL main entry-point functions

int __stdcall DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	tsHandleList			*ptHandleList				=	NULL;
	
	int						index						=	0;

	tsHandle				vHandles[1024]				=	{0};
	
	int						count						=	0;
	
    switch (fdwReason) 
	{
        case DLL_PROCESS_ATTACH:
           
			if (InitCVIRTE (hinstDLL, 0, 0) == 0)
                return 0;     /* out of memory */
			
			CmtNewTSV ( sizeof(tsHandleList) , &gHandleListHandle );
			
            break;
        case DLL_PROCESS_DETACH:
			
			if ( CmtGetTSVPtr ( gHandleListHandle , &ptHandleList ) == 0 )
			{
				count = ptHandleList->count;
				
				memcpy( vHandles , ptHandleList->vHandles , sizeof(vHandles));
				
				CmtReleaseTSVPtr ( gHandleListHandle ); 
			}
			
			for ( index = 0; index < count; index++ )
			{
				if ( vHandles[index].handle )
					EXT_TRIG_TemperatureModuleClose ( &(vHandles[index].handle) );
			}
			
			CmtDiscardTSV (gHandleListHandle);
			
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

int	DLLEXPORT	EXT_TRIG_TemperatureModuleInit( int lDriverDeviceID , char *pszDriverLocation , char *pszConfigFile , char *pszAddress , int *pHandle )
{
	STD_ERROR						StdError											=	{0};

	char							*pGetCommandName									=	0;
	
	short							*pGetValue											=	0;
	
	int								iDataSize											=	0,
									iDataLength											=	0;

	int								bCommandExists										=	0,
									bDriverLoadedSubDriver								=	0; 
								
	int								handle												=	0;

	tsHandleList					*ptHandleList										=	NULL;

	int								index												=	0;
	
	int								bFound												=	0;
	
	int								iHandleIndex										=	0;
	
	if ( pszDriverLocation )
	{
		CHK_STDERR( DRV_StandardDevice_Init( pszDriverLocation , lDriverDeviceID , "OVEN" , pszAddress , pHandle ));
	
		if ( pszConfigFile )
		{
			CHK_STDERR( DRV_StandardDevice_LoadConfigFile( *pHandle , pszConfigFile ));
	
			DRV_StandardDevice_Check_Connection( *pHandle , CLOSE_CONTROL_COMMAND_NAME , &bCommandExists );

			if ( bCommandExists )
			{
				CHK_STDERR( DRV_StandardDevice_SetValue( *pHandle , INIT_CONTROL_COMMAND_NAME , NULL , 0 , 0 )); 
				CHK_STDERR( DRV_StandardDevice_GetValue( *pHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize )); 
			}
		
			bDriverLoadedSubDriver = 1;
		}
	}
	
	if ( pHandle )
		handle = *pHandle;
	
	if ( CmtGetTSVPtr ( gHandleListHandle , &ptHandleList ) == 0 )
	{
		for ( index = 0; index < ptHandleList->count; index++ )
			if ( ptHandleList->vHandles[index].handle == handle )
				bFound = 1;	
		
		if ( bFound == 0 )
		{
			iHandleIndex = ptHandleList->count++;
			
			ptHandleList->vHandles[iHandleIndex].handle = handle;
		
			ptHandleList->vHandles[iHandleIndex].bDriverLoadedSubDriver = bDriverLoadedSubDriver; 
		}
		
		CmtReleaseTSVPtr ( gHandleListHandle ); 
	}
	
Error:
		
	FREE(pGetCommandName);
	FREE(pGetValue);
	
	RETURN_ERROR_VALUE;
}

double	DLLEXPORT	EXT_TRIG_TemperatureModuleSet( int iHandle , double lfTemperature , double lfTemperatureStep , double lfTimeDuration )
{
	STD_ERROR					StdError											=	{0};

 	char						*pGetCommandName									=	0;
	
	short						*pGetValue											=	0;
	
	int							iDataSize											=	0,
								iDataLength											=	0;
	
	double						lfGetTemperature									=	0.0,
								lfGetSlope											=	0.0;
								 
	static double				lfReadTemperature 									=	0.0;

	int							iSlope												=	0,
								iTemperature										=	0;

	int							bCommandExists										=	0;
	
	DRV_StandardDevice_Check_Connection( iHandle , GET_SLOPE_COMMAND_NAME , &bCommandExists );

	if ( bCommandExists )
	{
		CHK_STDERR( DRV_StandardDevice_SetValue( iHandle , GET_SLOPE_COMMAND_NAME , NULL , 0 , 0 ));                                 
		CHK_STDERR( DRV_StandardDevice_GetValue( iHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize )); 
	
		IF ((( pGetValue == NULL ) || ( iDataLength == 0 )) , "No responds" );
	
		lfGetSlope = (double)pGetValue[0] / 100.0;
	
		if ( lfGetSlope != lfTemperatureStep )
		{
			iSlope = lfTemperatureStep * 100;   
			CHK_STDERR( DRV_StandardDevice_SetValue( iHandle , SET_SLOPE_COMMAND_NAME , &iSlope , 1 , sizeof(iSlope) )); 
			CHK_STDERR( DRV_StandardDevice_GetValue( iHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize )); 
		}
	}
	
	CHK_STDERR( DRV_StandardDevice_SetValue( iHandle , GET_TEMPERATURE_COMMAND_NAME , NULL , 0 , 0 ));                                 
	CHK_STDERR( DRV_StandardDevice_GetValue( iHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize )); 
	
	IF ((( pGetValue == NULL ) || ( iDataLength == 0 )) , "No responds" );
	
	lfGetTemperature = (double)pGetValue[0] / 100.0;
	
	if ( lfGetTemperature != lfTemperature )
	{
		iTemperature = lfTemperature * 100;   
		CHK_STDERR( DRV_StandardDevice_SetValue( iHandle , SET_TEMPERATURE_COMMAND_NAME , &iTemperature , 1 , sizeof(iTemperature) )); 
		CHK_STDERR( DRV_StandardDevice_GetValue( iHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize )); 
	}
	
	
	CHK_STDERR( DRV_StandardDevice_SetValue( iHandle , READ_TEMPERATURE_COMMAND_NAME , NULL , 0 , 0 ));                                 
	CHK_STDERR( DRV_StandardDevice_GetValue( iHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize )); 
	
	IF ((( pGetValue == NULL ) || ( iDataLength == 0 )) , "No responds" );
	
	lfReadTemperature = (double)pGetValue[0] / 100.0;
	
	CHK_STDERR( DRV_StandardDevice_SetValue( iHandle , GET_CHAMBER_STATUS_COMMAND_NAME , NULL , 0 , 0 ));                                 
	CHK_STDERR( DRV_StandardDevice_GetValue( iHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize )); 
	
	IF ((( pGetValue == NULL ) || ( iDataLength == 0 )) , "No responds" );
	
	if ( pGetValue[0] == 0 )
	{
		CHK_STDERR( DRV_StandardDevice_SetValue( iHandle , START_OVEN_COMMAND_NAME , NULL , 0 , 0 ));                                 
		CHK_STDERR( DRV_StandardDevice_GetValue( iHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize )); 
	}
	
Error:
		
	FREE(pGetCommandName);
	FREE(pGetValue);
	
	RETURN_VALUE( lfReadTemperature );
}

double	DLLEXPORT	EXT_TRIG_VibrationModuleSet( int iHandle , double lfVibration , double lfVibrationStep , double lfTimeDuration )
{
	STD_ERROR					StdError											=	{0};

	char						*pGetValue											=	0;
	
	int							iDataSize											=	0,
								iDataLength											=	0;

	static double				lfReadVibration 									=	0.0;

	int							bStartTrigger										=	0;
	
Error:
		
	FREE(pGetValue);
	
	RETURN_VALUE( lfReadVibration );
}

int	DLLEXPORT	EXT_TRIG_TemperatureModuleClose( int *pHandle )
{
	STD_ERROR						StdError											=	{0};

	char							*pGetCommandName									=	0;
	
	short							*pGetValue											=	0;
	
	int								iDataSize											=	0,
									iDataLength											=	0;
	
	int								bCommandExists										=	0,
									bDriverLoadedSubDriver								=	0; 
									
	int								index												=	0;
	
	int								handle												=	0;

	tsHandleList					*ptHandleList										=	NULL;
	
	DRV_StandardDevice_Check_Connection( *pHandle , STOP_OVEN_COMMAND_NAME , &bCommandExists );

	if ( bCommandExists )
	{
		CHK_STDERR( DRV_StandardDevice_SetValue( *pHandle , STOP_OVEN_COMMAND_NAME , NULL , 0 , 0 )); 
	
		CHK_STDERR( DRV_StandardDevice_GetValue( *pHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize )); 
	}
	
	DRV_StandardDevice_Check_Connection( *pHandle , CLOSE_CONTROL_COMMAND_NAME , &bCommandExists );

	if ( bCommandExists )
	{
		CHK_STDERR( DRV_StandardDevice_SetValue( *pHandle , CLOSE_CONTROL_COMMAND_NAME , NULL , 0 , 0 )); 
	
		CHK_STDERR( DRV_StandardDevice_GetValue( *pHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize )); 
	}

	if ( pHandle )
		handle = *pHandle;
	
	if ( CmtGetTSVPtr ( gHandleListHandle , &ptHandleList ) == 0 )
	{
		for ( index = 0; index < ptHandleList->count; index++ )
			if ( ptHandleList->vHandles[index].handle == handle )
			{
				ptHandleList->vHandles[index].handle = 0;	
				bDriverLoadedSubDriver = ptHandleList->vHandles[index].bDriverLoadedSubDriver;
			}
		
		ptHandleList->count--;
		
		CmtReleaseTSVPtr ( gHandleListHandle ); 
	}
	
	if ( bDriverLoadedSubDriver )
	{
		CHK_STDERR( DRV_StandardDevice_Close( pHandle ));
	}
	
Error:
		
	FREE(pGetCommandName);
	FREE(pGetValue);
	
	RETURN_ERROR_VALUE;
}

void main()
{
	int			iHandle						=	0;
	

	return;
}
