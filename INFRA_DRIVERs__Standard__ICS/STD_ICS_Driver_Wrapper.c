//==============================================================================
// Include files

#include "windows.h"
#include "cvidef.h" 
#include <utility.h>
#include "GlobalDefines.h"
#include "toolbox.h"
#include "STD_ICS_Driver_Standard.h"

//==============================================================================
// Constants

#define			DEVICE_ID								STANDARD_ICS_DRIVER_DEVICE_ID

//==============================================================================
// Types

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
                return 0;     /* out of memory */

            break;
			
        case DLL_PROCESS_DETACH:

            break;
    }
    
    return 1;
}

int __stdcall DllEntryPoint (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    /* Included for compatibility with Borland */

    return DllMain (hinstDLL, fdwReason, lpvReserved);
}

unsigned __int64	DLLEXPORT		StandardDevice_Get_IdentificationNumber( void )
{
	return (unsigned __int64)DEVICE_ID;
}

/***** ***** ***** ***** ***** ***** Get Error Text Message ***** ***** ***** ***** ***** ***** *****/

void* DLLEXPORT 		StandardDevice_GetErrorTextMessage ( int vhInstrumentHandle , int iError , char *pErrorMessage )
{
	return ICS_STD_Driver_GetErrorTextMessage( vhInstrumentHandle , iError , pErrorMessage );
}

void*	DLLEXPORT		Config_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet )
{
	return ICS_Copy_STD_CallBackSet( pHandle , pCallBackSet );
}

void*	DLLEXPORT		Equipment_GetLowLevelHandle( int hHandle , int *phLowLevelHandle )
{
	return ICS_STD_Driver_GetLowLevelHandle( hHandle , phLowLevelHandle );	
}

void*	DLLEXPORT		Equipment_Info ( int hLowLevelHandle , char *pAddress , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware )
{
	return ICS_STD_Driver_Equipment_Info( hLowLevelHandle , pAddress , pVendor , pSerialNumber , pModel , pFirmware );
}

void*	DLLEXPORT		Equipment_IsSupported ( int hLowLevelHandle , char *pAddress , char *pID_String , char *pIdentificationText , int *pbSupporting , void **pLocalData )
{
	return ICS_STD_Driver_Equipment_IsSupported( hLowLevelHandle , pAddress , pID_String , pIdentificationText , pbSupporting , pLocalData );
}																																		  

void*	DLLEXPORT		StandardDevice_Init( int iDeviceID , char *pszConnectionName , char *pszAddress , int *pHandle , ... )
{
	STD_ERROR							StdError											=	{0};
	
	int									iLocalDeviceID										=	0;
	
	iLocalDeviceID = DEVICE_ID;
	
	IF (( iDeviceID != iLocalDeviceID ) , "Illegal Device ID");
	
	FREE_STDERR_COPY_ERR( ICS_STD_Driver_Init( pszAddress , pHandle )); 

Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_GetConnectionAddress( int hHandle , char **pszAddress )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");
	
	FREE_STDERR_COPY_ERR( ICS_STD_Driver_GetConnectionAddress( hHandle , pszAddress ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_Reset( int hHandle )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");
		
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_SetValue( int hHandle , char *pCommandName , void *pValue , int iValueLength , int iValueSize , ...  )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");
	
	FREE_STDERR_COPY_ERR( ICS_STD_Driver_SetValue( hHandle , pCommandName , pValue , iValueLength , iValueSize ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize , ...  )            
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");
		
	FREE_STDERR_COPY_ERR( ICS_STD_Driver_GetValue( hHandle , pGetCommandName , pGetValue , piValueLength , piValueSize ));
		
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_GetValue_Ex ( int hHandle , char *pCommandName , void **pGetValue , int *piValueLength , int *piValueSize , ...  )            
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");
		
	FREE_STDERR_COPY_ERR( ICS_STD_Driver_GetValue_Ex( hHandle , pCommandName , pGetValue , piValueLength , piValueSize ));
		
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_Close( int *pHandle )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL.");

	FREE_STDERR_COPY_ERR( ICS_STD_Driver_Close( pHandle )); 
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_Check_Connection( int hHandle , char *pCommandName , int *piStatus )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( ICS_STD_Driver_Check_Connection( hHandle , pCommandName , piStatus ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_Unplug( int hHandle , double lfTimeOut )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( ICS_STD_Driver_Unplug( hHandle , lfTimeOut ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}


void*	DLLEXPORT		StandardDevice_PlugIn( int hHandle , double lfTimeOut )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( ICS_STD_Driver_PlugIn( hHandle , lfTimeOut ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}


void*	DLLEXPORT		StandardDevice_EnablePlugInAutomatically( int hHandle , int bEnable )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( ICS_STD_Driver_EnablePlugInAutomatically( hHandle , bEnable ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_InstallReceiveCallbackFunction( int hHandle , void *pfFunction , void *pData )  
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( ICS_STD_Driver_InstallReceiveCallbackFunction( hHandle , pfFunction , pData ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_LoadConfigFile( int hHandle , char *pFileName )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( ICS_STD_Driver_LoadConfigFile( hHandle , pFileName ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}


void*	DLLEXPORT		StandardDevice_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( ICS_STD_Driver_LoadConfigBuffer( hHandle , pBuffer , iBufferSize ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_Get_Commands_List( int hHandle , char ***pCommandsList , int *piNumberOfCommands )     
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( ICS_STD_Driver_Get_Commands_List( hHandle , pCommandsList , piNumberOfCommands ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void main( void )
{
 	int					hHandle				=	0;

	int					iSwitchValue		=	0;
	
	StandardDevice_Init( DEVICE_ID , "RFM RF BOX" , "TCPIP::10.0.0.101" , &hHandle ) ;
	
	StandardDevice_LoadConfigFile( hHandle , "D:\\src\\CONFIG_STD_DEVICEs__RF_BOX__RFM\\RFM_RF_BOX_Control.icscnf" ); 
	
	iSwitchValue = 3;
	StandardDevice_SetValue( hHandle , "SW1" , &iSwitchValue , 1 , sizeof(iSwitchValue) );
	StandardDevice_SetValue( hHandle , "SW3" , &iSwitchValue , 1 , sizeof(iSwitchValue) );
	StandardDevice_SetValue( hHandle , "SW1" , &iSwitchValue , 1 , sizeof(iSwitchValue) );
	StandardDevice_SetValue( hHandle , "SW3" , &iSwitchValue , 1 , sizeof(iSwitchValue) );
	
	iSwitchValue = 1;
	StandardDevice_SetValue( hHandle , "SW2" , &iSwitchValue , 1 , sizeof(iSwitchValue) ); 
	
	iSwitchValue = 3;
	StandardDevice_SetValue( hHandle , "SW3" , &iSwitchValue , 1 , sizeof(iSwitchValue) ); 
	
	iSwitchValue = 3;
	StandardDevice_SetValue( hHandle , "SW1" , &iSwitchValue , 1 , sizeof(iSwitchValue) ); 
	
	iSwitchValue = 1;
	StandardDevice_SetValue( hHandle , "SW2" , &iSwitchValue , 1 , sizeof(iSwitchValue) ); 
	iSwitchValue = 1;
	
	iSwitchValue = 3;
	StandardDevice_SetValue( hHandle , "SW3" , &iSwitchValue , 1 , sizeof(iSwitchValue) ); 
	
	iSwitchValue = 3;
	StandardDevice_SetValue( hHandle , "SW1" , &iSwitchValue , 1 , sizeof(iSwitchValue) ); 
	iSwitchValue = 3;
	StandardDevice_SetValue( hHandle , "SW1" , &iSwitchValue , 1 , sizeof(iSwitchValue) ); 
	
	iSwitchValue = 1;
	StandardDevice_SetValue( hHandle , "SW2" , &iSwitchValue , 1 , sizeof(iSwitchValue) ); 
	iSwitchValue = 1;
	StandardDevice_SetValue( hHandle , "SW2" , &iSwitchValue , 1 , sizeof(iSwitchValue) ); 
	
	iSwitchValue = 3;
	StandardDevice_SetValue( hHandle , "SW3" , &iSwitchValue , 1 , sizeof(iSwitchValue) ); 
	iSwitchValue = 3;
	StandardDevice_SetValue( hHandle , "SW3" , &iSwitchValue , 1 , sizeof(iSwitchValue) ); 
	iSwitchValue = 3;
	StandardDevice_SetValue( hHandle , "SW3" , &iSwitchValue , 1 , sizeof(iSwitchValue) );
	iSwitchValue = 3;
	StandardDevice_SetValue( hHandle , "SW3" , &iSwitchValue , 1 , sizeof(iSwitchValue) ); 
	
	iSwitchValue = 1; 
	StandardDevice_SetValue( hHandle , "SW4" , &iSwitchValue , 1 , sizeof(iSwitchValue) ); 
	iSwitchValue = 1; 
	StandardDevice_SetValue( hHandle , "SW4" , &iSwitchValue , 1 , sizeof(iSwitchValue) ); 
	iSwitchValue = 1; 
	StandardDevice_SetValue( hHandle , "SW4" , &iSwitchValue , 1 , sizeof(iSwitchValue) );
	iSwitchValue = 1; 
	StandardDevice_SetValue( hHandle , "SW4" , &iSwitchValue , 1 , sizeof(iSwitchValue) ); 
	
	iSwitchValue = 1; 
	StandardDevice_SetValue( hHandle , "SW5" , &iSwitchValue , 1 , sizeof(iSwitchValue) ); 
	iSwitchValue = 0; 
	StandardDevice_SetValue( hHandle , "SW5" , &iSwitchValue , 1 , sizeof(iSwitchValue) ); 
	
	iSwitchValue = 1; 
	StandardDevice_SetValue( hHandle , "SW6" , &iSwitchValue , 1 , sizeof(iSwitchValue) );
	iSwitchValue = 0; 
	StandardDevice_SetValue( hHandle , "SW6" , &iSwitchValue , 1 , sizeof(iSwitchValue) ); 
	
	iSwitchValue = 1; 
	StandardDevice_SetValue( hHandle , "SW7" , &iSwitchValue , 1 , sizeof(iSwitchValue) );
	iSwitchValue = 0; 
	StandardDevice_SetValue( hHandle , "SW7" , &iSwitchValue , 1 , sizeof(iSwitchValue) );
	
	iSwitchValue = 1; 
	StandardDevice_SetValue( hHandle , "SW8" , &iSwitchValue , 1 , sizeof(iSwitchValue) );
	iSwitchValue = 2; 
	StandardDevice_SetValue( hHandle , "SW8" , &iSwitchValue , 1 , sizeof(iSwitchValue) ); 
	iSwitchValue = 4; 
	StandardDevice_SetValue( hHandle , "SW8" , &iSwitchValue , 1 , sizeof(iSwitchValue) ); 
	iSwitchValue = 0; 
	StandardDevice_SetValue( hHandle , "SW8" , &iSwitchValue , 1 , sizeof(iSwitchValue) ); 
	
	StandardDevice_Close( &hHandle );    
		
	return;
}
