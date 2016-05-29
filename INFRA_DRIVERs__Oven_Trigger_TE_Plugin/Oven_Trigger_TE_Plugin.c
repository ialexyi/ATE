//==============================================================================
// Include files
#include "Windows.h"
#include "toolbox.h"
#include <ansi_c.h>
#include <userint.h>
#include <utility.h>
#include "Oven_Trigger_TE_Plugin.h"
#include "STD_DriversManager.h"
#include "GlobalDefines.h"

//==============================================================================
// Constants

#define						GET_TRIGGER_TEMP_COMMAND_NAME	"GET_START_TEST_TEMP_TRIGGER"
#define						GET_TRIGGER_VIBR_COMMAND_NAME	"GET_START_TEST_VIBR_TRIGGER"  

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables
static int					gDriverLoadedSubDriver						=		0;
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

int	DLLEXPORT	EXT_TRIG_TemperatureModuleInit( int lDriverDeviceID , char *pszDriverLocation , char *pszConfigFile , char *pszAddress , int *pHandle )
{
	STD_ERROR					StdError											=	{0};
	
	if ( pszDriverLocation )
	{
		CHK_STDERR( DRV_StandardDevice_Init( pszDriverLocation , lDriverDeviceID , "OVEN" , pszAddress , pHandle ));
	
		if ( pszConfigFile )
		{
			CHK_STDERR( DRV_StandardDevice_LoadConfigFile( *pHandle , pszConfigFile ));
		}
		
		gDriverLoadedSubDriver = 1;
	}
	
Error:
		
	RETURN_ERROR_VALUE;
}

double	DLLEXPORT	EXT_TRIG_TemperatureModuleSet( int iHandle , double lfTemperature , double lfTemperatureStep , double lfTimeDuration )
{
	STD_ERROR					StdError											=	{0};

	char						*pGetValue											=	0;
	
	int							iDataSize											=	0,
								iDataLength											=	0;

	static double				lfReadTemperature 									=	0.0;

	int							bStartTrigger										=	0;
	
	CHK_STDERR( DRV_StandardDevice_GetValue_Ex( iHandle , GET_TRIGGER_TEMP_COMMAND_NAME , (void*)&pGetValue , &iDataLength , &iDataSize )); 
	
	IF ((( pGetValue == NULL ) || ( iDataLength == 0 )) , "No responds" );
	
	bStartTrigger = *pGetValue;
	
	if ( bStartTrigger == 0 ) 
		lfReadTemperature = lfTemperature;

Error:
		
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
	
	CHK_STDERR( DRV_StandardDevice_GetValue_Ex( iHandle , GET_TRIGGER_VIBR_COMMAND_NAME , (void*)&pGetValue , &iDataLength , &iDataSize )); 
	
	IF ((( pGetValue == NULL ) || ( iDataLength == 0 )) , "No responds" );
	
	bStartTrigger = *pGetValue;
	
	if ( bStartTrigger == 0 ) 
		lfReadVibration = lfVibration;

Error:
		
	FREE(pGetValue);
	
	RETURN_VALUE( lfReadVibration );
}


int	DLLEXPORT	EXT_TRIG_TemperatureModuleClose( int *pHandle )
{
	STD_ERROR					StdError											=	{0};

	if ( gDriverLoadedSubDriver )
	{
		CHK_STDERR( DRV_StandardDevice_Close( pHandle ));
	}
	
Error:
		
	RETURN_ERROR_VALUE;
}

void main()
{
	int			iHandle						=	0;
	

	return;
}
