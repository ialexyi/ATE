//==============================================================================
// Include files

#include "windows.h"
#include "cvidef.h" 
#include <utility.h>
#include "GlobalDefines.h"
#include "StandardExtendedProtocolDriver_Protocol.h" 
#include "toolbox.h"
//==============================================================================
// Constants

#define			DEVICE_ID						STANDARD_EXTENDED_PROTOCOL_DRIVER_DEVICE_ID

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

void*	DLLEXPORT		Config_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet )
{
	STD_ERROR						StdError									=	{0};
	
	RETURN_STDERR_POINTER;
}

//typedef void* (*pfSTD_Config_Install_CommentCallback) ( int *pHandle , pfCommentCallback fFunction , void *pData , int type);
void*	DLLEXPORT		Config_Install_CommentCallback( int *pHandle , pfCommentCallback fFunction , void *pData , int type) 
{
	STD_ERROR				StdError						=	{0};
						
	FREE_STDERR_COPY_ERR( STD_Driver_Config_Install_CommentCallback( pHandle , fFunction , pData , type));

Error: 	
	
	RETURN_STDERR_POINTER;	
}

//typedef void* (*pfSTD_Config_Install_ConfigValueCallback) ( int *pHandle , pfConfigValueCallback fFunction , void *pData , int type);
void*	DLLEXPORT		Config_Install_ConfigValueCallback( int *pHandle , pfConfigValueCallback fFunction , void *pData , int type) 
{
	STD_ERROR				StdError						=	{0};
						
	
	FREE_STDERR_COPY_ERR( STD_Driver_Config_Install_ConfigValueCallback( pHandle , fFunction , pData , type));
	
Error: 
	
	RETURN_STDERR_POINTER;	
}

//typedef void* (*pfSTD_Config_Install_CheckForBreakCallback) ( int *pHandle , pfCheckForBreakCallback fFunction , void *pData , int type);
void*	DLLEXPORT		Config_Install_CheckForBreakCallback( int *pHandle , pfCheckForBreakCallback fFunction , void *pData , int type) 
{
	STD_ERROR				StdError						=	{0};
						
	
	FREE_STDERR_COPY_ERR( STD_Driver_Config_Install_CheckForBreakCallback( pHandle , fFunction , pData , type));
	
Error: 
	
	RETURN_STDERR_POINTER;	
}

//typedef void* (*pfSTD_Config_LOG_SetAllowState) ( int iHandle , int bAllowLog ); 
void*	DLLEXPORT		Config_LOG_SetAllowState( int iHandle , int bAllowLog ) 
{
	STD_ERROR				StdError						=	{0};
	
	FREE_STDERR_COPY_ERR( STD_Driver_Config_LOG_SetAllowState( iHandle , bAllowLog ));
	
Error: 
	
	RETURN_STDERR_POINTER;	
}

void* DLLEXPORT Config_SetAttribute( int hHandle , char *pAttributeName , char *pAttributeValue )
{
	STD_ERROR				StdError						=	{0};
	
	FREE_STDERR_COPY_ERR( STD_Config_SetAttribute( hHandle , pAttributeName , pAttributeValue ));
	
Error: 
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_Init( int iDeviceID , char *pszConnectionName , char *pszAddress , int *pHandle , ... )
{
	STD_ERROR							StdError											=	{0};
	
	int									iLocalDeviceID										=	0;
	
	iLocalDeviceID = DEVICE_ID;
	
	IF (( iDeviceID != iLocalDeviceID ) , "Illegal Device ID");
	
	FREE_STDERR_COPY_ERR( STD_Driver_Init( pszConnectionName , pszAddress , pHandle )); 

Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_GetConnectionAddress( int hHandle , char **pszAddress )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");
	
	FREE_STDERR_COPY_ERR( STD_Driver_GetConnectionAddress( hHandle , pszAddress ));
	
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
	
	FREE_STDERR_COPY_ERR( STD_Driver_SetValue( hHandle , pCommandName , pValue , iValueLength , iValueSize ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize , ...  )            
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");
		
	FREE_STDERR_COPY_ERR( STD_Driver_GetValue( hHandle , pGetCommandName , pGetValue , piValueLength , piValueSize ));
		
Error:
	
	RETURN_STDERR_POINTER;	
}


void*	DLLEXPORT		StandardDevice_Close( int *pHandle )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL.");

	FREE_STDERR_COPY_ERR( STD_Driver_Close( pHandle )); 
	
Error:
	
	RETURN_STDERR_POINTER;	
}


void*	DLLEXPORT		StandardDevice_Unplug( int hHandle , double lfTimeOut )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( STD_Driver_Unplug( hHandle , lfTimeOut ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}


void*	DLLEXPORT		StandardDevice_PlugIn( int hHandle , double lfTimeOut )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( STD_Driver_PlugIn( hHandle , lfTimeOut ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}


void*	DLLEXPORT		StandardDevice_EnablePlugInAutomatically( int hHandle , int bEnable )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( STD_Driver_EnablePlugInAutomatically( hHandle , bEnable ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_InstallReceiveCallbackFunction( int hHandle , void *pfFunction , void *pData )  
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( STD_Driver_InstallReceiveCallbackFunction( hHandle , pfFunction , pData ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_LoadConfigFile( int hHandle , char *pFileName )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( STD_Driver_LoadConfigFile( hHandle , pFileName ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}


void*	DLLEXPORT		StandardDevice_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( STD_Driver_LoadConfigBuffer( hHandle , pBuffer , iBufferSize ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_Check_Connection( int hHandle , char *pCommandName , int *piStatus )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( STD_Driver_Check_Connection( hHandle , pCommandName , piStatus ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_Get_Commands_List( int hHandle , char ***pCommandsList , int *piNumberOfCommands )     
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( STD_Driver_Get_Commands_List( hHandle , pCommandsList , piNumberOfCommands ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void main( void )
{
	int					hHandle				=	0;
	
	char				*pGetCommandName	=	0,
						*pGetValue			=	0;
	
	int					iDataLength			=	0,
						iDataSize			=	0;
	
	char				pSetValue[10]		=	{0};
		
	StandardDevice_Init( DEVICE_ID , "RF Box" , "ASRL4" , &hHandle ) ;
	
	StandardDevice_LoadConfigFile( hHandle , "C:\\emi\\WorkSpace\\iff_rf_box.spcnf" ); 
	/* 	
	pSetValue[0] = 0;
	pSetValue[1] = 0;	 // off
	pSetValue[2] = 0;
	
	StandardDevice_SetValue( hHandle , "SET_IFF_PULSE_GEN" , pSetValue , 3 , 1 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void**)&pGetValue , &iDataLength , &iDataSize ); 
	

	pSetValue[0] = 1;
		
	StandardDevice_SetValue( hHandle , "SET_IFF_RF_BOX_SW" , pSetValue , 1 , 1 );  
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void**)&pGetValue , &iDataLength , &iDataSize ); 
		
	pSetValue[0] = 1;
	pSetValue[1] = 0;	 // mode B
	pSetValue[2] = 10;
	
	StandardDevice_SetValue( hHandle , "SET_IFF_PULSE_GEN" , pSetValue , 3 , 1 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void**)&pGetValue , &iDataLength , &iDataSize );  
	  
	pSetValue[0] = 1;
	pSetValue[1] = 1;	 // mode A
	pSetValue[2] = 10;
	
	StandardDevice_SetValue( hHandle , "SET_IFF_PULSE_GEN" , pSetValue , 3 , 1 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void**)&pGetValue , &iDataLength , &iDataSize );  
 
	pSetValue[0] = 1;
	pSetValue[1] = 2;	 // mode 1
	pSetValue[2] = 10;
	
	StandardDevice_SetValue( hHandle , "SET_IFF_PULSE_GEN" , pSetValue , 3 , 1 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void**)&pGetValue , &iDataLength , &iDataSize );  
	
	pSetValue[0] = 1;
	pSetValue[1] = 3;	 // mode 2
	pSetValue[2] = 10;
	
	StandardDevice_SetValue( hHandle , "SET_IFF_PULSE_GEN" , pSetValue , 3 , 1 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void**)&pGetValue , &iDataLength , &iDataSize );              
	
	
	pSetValue[0] = 1;
	pSetValue[1] = 4;	 // mode 3
	pSetValue[2] = 10;
	
	StandardDevice_SetValue( hHandle , "SET_IFF_PULSE_GEN" , pSetValue , 3 , 1 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void**)&pGetValue , &iDataLength , &iDataSize );                
	*/  
	
	pSetValue[0] = 1;
	pSetValue[1] = 7;	 // mode TX 1  	
	pSetValue[2] = 10;
	
	StandardDevice_SetValue( hHandle , "SET_IFF_PULSE_GEN" , pSetValue , 3 , 1 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void**)&pGetValue , &iDataLength , &iDataSize );                
	
	StandardDevice_SetValue( hHandle , "GET_IFF_PULSE_GEN" , pSetValue , 3 , 1 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void**)&pGetValue , &iDataLength , &iDataSize );                
	
	pSetValue[0] = 1;
	pSetValue[1] = 8;	 // mode TX 2   	
	pSetValue[2] = 10;
	
	StandardDevice_SetValue( hHandle , "SET_IFF_PULSE_GEN" , pSetValue , 3 , 1 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void**)&pGetValue , &iDataLength , &iDataSize );               
	
	StandardDevice_SetValue( hHandle , "GET_IFF_PULSE_GEN" , pSetValue , 3 , 1 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void**)&pGetValue , &iDataLength , &iDataSize );                
	
	pSetValue[0] = 1;
	pSetValue[1] = 5;	 // mode X  	
	pSetValue[2] = 15;
	
	StandardDevice_SetValue( hHandle , "SET_IFF_PULSE_GEN" , pSetValue , 3 , 1 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void**)&pGetValue , &iDataLength , &iDataSize );               
	/*
	pSetValue[0] = 1;
	pSetValue[1] = 6;	 // mode C  	
	pSetValue[2] = 10;
	
	StandardDevice_SetValue( hHandle , "SET_IFF_PULSE_GEN" , pSetValue , 3 , 1 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , (void**)&pGetValue , &iDataLength , &iDataSize );               
	
	
	/*
	pGetValue[0] = 1;
		
	StandardDevice_SetValue( hHandle , "SET_IFF_RF_BOX_SW" , pGetValue , 1 , 1 );  
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_IFF_RF_BOX_SW" , NULL , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength );
	
	pGetValue[0] = 2;
		
	StandardDevice_SetValue( hHandle , "SET_IFF_RF_BOX_SW" , pGetValue , 1 , 1 );  
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_IFF_RF_BOX_SW" , NULL , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength );
	 
	pGetValue[0] = 3;
		
	StandardDevice_SetValue( hHandle , "SET_IFF_RF_BOX_SW" , pGetValue , 1 , 1 );  
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_IFF_RF_BOX_SW" , NULL , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength );

	pGetValue[0] = 4;
		
	StandardDevice_SetValue( hHandle , "SET_IFF_RF_BOX_SW" , pGetValue , 1 , 1 );  
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_IFF_RF_BOX_SW" , NULL , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength );

	pGetValue[0] = 5;
		
	StandardDevice_SetValue( hHandle , "SET_IFF_RF_BOX_SW" , pGetValue , 1 , 1 );  
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_IFF_RF_BOX_SW" , NULL , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength );
	
	pGetValue[0] = 6;
		
	StandardDevice_SetValue( hHandle , "SET_IFF_RF_BOX_SW" , pGetValue , 1 , 1 );  
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_IFF_RF_BOX_SW" , NULL , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength );
	
	pGetValue[0] = 7;
		
	StandardDevice_SetValue( hHandle , "SET_IFF_RF_BOX_SW" , pGetValue , 1 , 1 );  
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_IFF_RF_BOX_SW" , NULL , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength );
	
	pGetValue[0] = 8;
		
	StandardDevice_SetValue( hHandle , "SET_IFF_RF_BOX_SW" , pGetValue , 1 , 1 );  
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_IFF_RF_BOX_SW" , NULL , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength );
	
	pGetValue[0] = 9;
		
	StandardDevice_SetValue( hHandle , "SET_IFF_RF_BOX_SW" , pGetValue , 1 , 1 );  
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_IFF_RF_BOX_SW" , NULL , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength );
	
	pGetValue[0] = 10;
		
	StandardDevice_SetValue( hHandle , "SET_IFF_RF_BOX_SW" , pGetValue , 1 , 1 );  
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_IFF_RF_BOX_SW" , NULL , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength );
	
	pGetValue[0] = 11;
		
	StandardDevice_SetValue( hHandle , "SET_IFF_RF_BOX_SW" , pGetValue , 1 , 1 );  
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_IFF_RF_BOX_SW" , NULL , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength );
	
	pGetValue[0] = 12;
		
	StandardDevice_SetValue( hHandle , "SET_IFF_RF_BOX_SW" , pGetValue , 1 , 1 );  
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_IFF_RF_BOX_SW" , NULL , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength );
	
	pGetValue[0] = 13;    
		
	StandardDevice_SetValue( hHandle , "SET_IFF_RF_BOX_SW" , pGetValue , 1 , 1 );  
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_IFF_RF_BOX_SW" , NULL , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength );
	
	pGetValue[0] = 14;  
		
	StandardDevice_SetValue( hHandle , "SET_IFF_RF_BOX_SW" , pGetValue , 1 , 1 );  
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_IFF_RF_BOX_SW" , NULL , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength );

	pGetValue[0] = 15;  
		
	StandardDevice_SetValue( hHandle , "SET_IFF_RF_BOX_SW" , pGetValue , 1 , 1 );  
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_IFF_RF_BOX_SW" , NULL , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength );
	
	pGetValue[0] = 16;  
		
	StandardDevice_SetValue( hHandle , "SET_IFF_RF_BOX_SW" , pGetValue , 1 , 1 );  
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_IFF_RF_BOX_SW" , NULL , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength );
	
	pGetValue[0] = 17;  
		
	StandardDevice_SetValue( hHandle , "SET_IFF_RF_BOX_SW" , pGetValue , 1 , 1 );  
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_IFF_RF_BOX_SW" , NULL , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength );
	
	pGetValue[0] = 18;  
		
	StandardDevice_SetValue( hHandle , "SET_IFF_RF_BOX_SW" , pGetValue , 1 , 1 );  
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_IFF_RF_BOX_SW" , NULL , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength );

	pGetValue[0] = 19;  
		
	StandardDevice_SetValue( hHandle , "SET_IFF_RF_BOX_SW" , pGetValue , 1 , 1 );  
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength , &iDataSize ); 
	
	StandardDevice_SetValue( hHandle , "GET_IFF_RF_BOX_SW" , NULL , 0 ); 
	StandardDevice_GetValue( hHandle , &pGetCommandName , &pGetValue , &iDataLength );
*/  	
	StandardDevice_Close( &hHandle );    
		
	return;
}
