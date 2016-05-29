//==============================================================================
// Include files

#include "Windows.h"
#include "toolbox.h"
#include <ansi_c.h>
#include <userint.h>
#include <utility.h>
#include "GlobalDefines.h"
#include "Modbus_OvenController_Standard.h"

//==============================================================================
// Constants

#define			DEVICE_ID					STANDARD_OVEN_CONTROL_MODBUS_DEVICE_ID

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

void*	DLLEXPORT		Config_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet )
{
	return MODBUS_Driver_Config_Copy_STD_CallBackSet( pHandle , pCallBackSet );
}

void* DLLEXPORT		StandardDevice_GetErrorTextMessage ( int vhInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR				StdError					=	{0};    

Error: 
	
	RETURN_STDERR_POINTER;
}

unsigned __int64	DLLEXPORT		StandardDevice_Get_IdentificationNumber( void )
{
	return (unsigned __int64)DEVICE_ID;
}

void*	DLLEXPORT		Equipment_Info ( int hLowLevelHandle , char *pAddress , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware )
{
	STD_ERROR						StdError									=	{0};
	RETURN_STDERR_POINTER;
}

void*	DLLEXPORT		Equipment_GetLowLevelHandle( int hHandle , int *phLowLevelHandle )
{
	STD_ERROR						StdError									=	{0};
	
	if ( phLowLevelHandle )
		*phLowLevelHandle = hHandle;
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		Equipment_IsSupported ( int hLowLevelHandle , char *pAddress , char *pID_String , char *pIdentificationText , int *pbSupporting , void **pLocalData )
{
	STD_ERROR						StdError									=	{0};
	RETURN_STDERR_POINTER;
}

void*	DLLEXPORT		Config_Install_CommentCallback( int *pHandle , pfCommentCallback fFunction , void *pData , int type) 
{
	STD_ERROR				StdError						=	{0};
						
	FREE_STDERR_COPY_ERR( MODBUS_Driver_Config_Install_CommentCallback( pHandle , fFunction , pData , type));

Error: 	
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		Config_Install_ConfigValueCallback( int *pHandle , pfConfigValueCallback fFunction , void *pData , int type) 
{
	STD_ERROR				StdError						=	{0};
						
	
	FREE_STDERR_COPY_ERR( MODBUS_Driver_Config_Install_ConfigValueCallback( pHandle , fFunction , pData , type));
	
Error: 
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		Config_Install_CheckForBreakCallback( int *pHandle , pfCheckForBreakCallback fFunction , void *pData , int type) 
{
	STD_ERROR				StdError						=	{0};
						
	
	FREE_STDERR_COPY_ERR( MODBUS_Driver_Config_Install_CheckForBreakCallback( pHandle , fFunction , pData , type));
	
Error: 
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		Config_LOG_SetAllowState( int iHandle , int bAllowLog ) 
{
	STD_ERROR				StdError						=	{0};
	
	FREE_STDERR_COPY_ERR( MODBUS_Driver_Config_LOG_SetAllowState( iHandle , bAllowLog ));
	
Error: 
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_Init( int iDeviceID , char *pszConnectionName , char *pszAddress , int *pHandle , ... )
{
	STD_ERROR							StdError											=	{0};
	
	int									handle												=	0,
										iLocalDeviceID										=	0;
	
	iLocalDeviceID = DEVICE_ID;
	
	IF (( iDeviceID != iLocalDeviceID ) , "Illegal Device ID");
	
	FREE_STDERR_COPY_ERR( MODBUS_Driver_Init( pszConnectionName , pszAddress , pHandle )); 

Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_GetConnectionAddress( int hHandle , char **pszAddress )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");
	
	FREE_STDERR_COPY_ERR( MODBUS_Driver_GetConnectionAddress( hHandle , pszAddress ));
	
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
	
	FREE_STDERR_COPY_ERR( MODBUS_Driver_SetValue( hHandle , pCommandName , pValue , iValueLength , iValueSize ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize , ...  )            
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");
		
	FREE_STDERR_COPY_ERR( MODBUS_Driver_GetValue( hHandle , pGetCommandName , pGetValue , piValueLength , piValueSize ));
		
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_Close( int *pHandle )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL.");

	FREE_STDERR_COPY_ERR( MODBUS_Driver_Close( pHandle )); 
	
Error:
	
	RETURN_STDERR_POINTER;	
}


void*	DLLEXPORT		StandardDevice_Unplug( int hHandle , double lfTimeOut )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( MODBUS_Driver_Unplug( hHandle , lfTimeOut ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}


void*	DLLEXPORT		StandardDevice_PlugIn( int hHandle , double lfTimeOut )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( MODBUS_Driver_PlugIn( hHandle , lfTimeOut ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}


void*	DLLEXPORT		StandardDevice_EnablePlugInAutomatically( int hHandle , int bEnable )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( MODBUS_Driver_EnablePlugInAutomatically( hHandle , bEnable ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_InstallReceiveCallbackFunction( int hHandle , void *pfFunction , void *pData )  
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( MODBUS_Driver_InstallReceiveCallbackFunction( hHandle , pfFunction , pData ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_LoadConfigFile( int hHandle , char *pFileName )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( MODBUS_Driver_LoadConfigFile( hHandle , pFileName ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}


void*	DLLEXPORT		StandardDevice_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( MODBUS_Driver_LoadConfigBuffer( hHandle , pBuffer , iBufferSize ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_Check_Connection( int hHandle , char *pCommandName , int *piStatus )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( MODBUS_Driver_Check_Connection( hHandle , pCommandName , piStatus ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_Get_Commands_List( int hHandle , char ***pCommandsList , int *piNumberOfCommands )     
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( MODBUS_Driver_Get_Commands_List( hHandle , pCommandsList , piNumberOfCommands ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

/*
void main( void )
{
	int					hHandle							=	0;
	
	char				*pGetCommandName				=	0;
	
	short				*pGetValue						=	0;
	
	int					iDataSize						=	0,
						iDataLength						=	0;
	
	char				pSetCommandValue[LOW_STRING]	=	{0};
	
	int					iSlope							=	0,
						iTemperature					=	0;
		  
	StandardDevice_Init( DEVICE_ID , "OVEN" , "ASRL11" , &hHandle ) ;
	
	StandardDevice_LoadConfigFile( hHandle , "C:\\emi\\WorkSpace\\Angelantoni.ovncnf" ); 

	StandardDevice_SetValue( hHandle , "INIT_CONTROL" , NULL , 0 , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "READ_TEMPERATURE" , NULL , 0 , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_TEMPERATURE" , NULL , 0 , 0 );                                 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize ); 
	
	
	
	
	StandardDevice_SetValue( hHandle , "GET_SLOPE" , NULL , 0 , 0 );                                 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize ); 
		
	iSlope = 0.5 * 100;   
	StandardDevice_SetValue( hHandle , "SET_SLOPE" , &iSlope , 1 , sizeof(iSlope) ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_SLOPE" , NULL , 0 , 0 );                                 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize ); 
	
	iSlope = 1 * 100;   
	StandardDevice_SetValue( hHandle , "SET_SLOPE" , &iSlope , 1 , sizeof(iSlope) ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_SLOPE" , NULL , 0 , 0 );                                 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_TEMPERATURE" , NULL , 0 , 0 );                                 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "READ_TEMPERATURE" , NULL , 0 , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_CHAMBER_STATUS" , NULL , 0 , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize ); 
	
	iTemperature = -10 * 100;
	StandardDevice_SetValue( hHandle , "SET_TEMPERATURE" , &iTemperature , 1 , sizeof(iTemperature) ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize );  
	
	StandardDevice_SetValue( hHandle , "START_OVEN" , NULL , 0 , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_CHAMBER_STATUS" , NULL , 0 , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_TEMPERATURE" , NULL , 0 , 0 );
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize );  
	
	StandardDevice_SetValue( hHandle , "READ_TEMPERATURE" , NULL , 0 , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "READ_TEMPERATURE" , NULL , 0 , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "READ_TEMPERATURE" , NULL , 0 , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize ); 
	
	iTemperature = 30 * 100;
	StandardDevice_SetValue( hHandle , "SET_TEMPERATURE" , &iTemperature , 1 , sizeof(iTemperature) );            
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize );   
	
	StandardDevice_SetValue( hHandle , "GET_TEMPERATURE" , NULL , 0 , 0 );
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize );  
	
	StandardDevice_SetValue( hHandle , "READ_TEMPERATURE" , NULL , 0 , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "READ_TEMPERATURE" , NULL , 0 , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "READ_TEMPERATURE" , NULL , 0 , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "STOP_OVEN" , NULL , 0 , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "CLOSE_CONTROL" , NULL , 0 , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void*)&pGetValue , &iDataLength , &iDataSize ); 
		
	DelayWithEventProcessing(1000.0);
	
	StandardDevice_Close( &hHandle );    
		
	return;
}
	*/
