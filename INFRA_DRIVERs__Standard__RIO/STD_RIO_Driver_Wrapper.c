//==============================================================================
// Include files 

#include <tcpsupp.h>
#include <cvirte.h> 
#include "windows.h"
#include "cvidef.h"
#include "GlobalDefines.h"
#include "toolbox.h"
#include "STD_RIO_Driver_Standard.h"

//==============================================================================
// Constants

#define			DEVICE_ID								STANDARD_RIO_DRIVER_DEVICE_ID

//==============================================================================  


// DLL main entry-point functions
//============================================================================== 

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

unsigned __int64	DLLEXPORT	StandardDevice_Get_IdentificationNumber( void )
{
	return (unsigned __int64)DEVICE_ID;
}

//==============================================================================    

// Global functions 
//==============================================================================  

/***** ***** ***** ***** ***** ***** Standard Device Get Error Text Message ***** ***** ***** ***** ***** ***** *****/ 
void* DLLEXPORT	StandardDevice_GetErrorTextMessage ( int vhInstrumentHandle , int iError , char *pErrorMessage )
{
	return RIO_STD_Driver_GetErrorTextMessage( vhInstrumentHandle , iError , pErrorMessage );
}

/***** ***** ***** ***** ***** ***** Config Copy STD Call Back Set ***** ***** ***** ***** ***** ***** *****/  
void*	DLLEXPORT	Config_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet )
{
	return RIO_Copy_STD_CallBackSet( pHandle , pCallBackSet );
}

/***** ***** ***** ***** ***** ***** Equipment Get Low Level Handle ***** ***** ***** ***** ***** ***** *****/   
void*	DLLEXPORT	Equipment_GetLowLevelHandle( int hHandle , int *phLowLevelHandle )
{
	return RIO_STD_Driver_GetLowLevelHandle( hHandle , phLowLevelHandle );	
}

/***** ***** ***** ***** ***** ***** Equipment Info ***** ***** ***** ***** ***** ***** *****/ 
void*	DLLEXPORT	Equipment_Info ( int hLowLevelHandle , char *pAddress , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware )
{
	return RIO_STD_Driver_Equipment_Info( hLowLevelHandle , pAddress , pVendor , pSerialNumber , pModel , pFirmware );
}

/***** ***** ***** ***** ***** ***** Equipment Is Supported ***** ***** ***** ***** ***** ***** *****/    
void*	DLLEXPORT		Equipment_IsSupported ( int hLowLevelHandle , char *pAddress , char *pID_String , char *pIdentificationText , int *pbSupporting , void **pLocalData )
{
	return RIO_STD_Driver_Equipment_IsSupported( hLowLevelHandle , pAddress , pID_String , pIdentificationText , pbSupporting , pLocalData );
}																																		  

/***** ***** ***** ***** ***** ***** Standard Device Init ***** ***** ***** ***** ***** ***** *****/ 
void*	DLLEXPORT	StandardDevice_Init( int iDeviceID , char *pszConnectionName , char *pszAddress , int *pHandle , ... )
{
	STD_ERROR							StdError											=	{0};
	
	int									iLocalDeviceID										=	0;
	
	iLocalDeviceID = DEVICE_ID;
	
	IF (( iDeviceID != iLocalDeviceID ) , "Illegal Device ID");
	
	FREE_STDERR_COPY_ERR( RIO_STD_Driver_Init( pszAddress , pHandle )); 

Error:
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** Standard Device Get Connection Address ***** ***** ***** ***** ***** ***** *****/    
void*	DLLEXPORT	StandardDevice_GetConnectionAddress( int hHandle , char **pszAddress )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");
	
	FREE_STDERR_COPY_ERR( RIO_STD_Driver_GetConnectionAddress( hHandle , pszAddress ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** Standard Device Reset ***** ***** ***** ***** ***** ***** *****/
void*	DLLEXPORT	StandardDevice_Reset( int hHandle )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");
	
	FREE_STDERR_COPY_ERR( RIO_STD_Driver_Reset ( hHandle )); 
		
Error:
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** Standard Device Set Value ***** ***** ***** ***** ***** ***** *****/
void*	DLLEXPORT	StandardDevice_SetValue( int hHandle , char *pCommandName , void *pValue , int iValueLength , int iValueSize , ...  )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");
	
	FREE_STDERR_COPY_ERR( RIO_STD_Driver_SetValue( hHandle , pCommandName , pValue , iValueLength , iValueSize ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** Standard Device Get Value ***** ***** ***** ***** ***** ***** *****/   
void*	DLLEXPORT	StandardDevice_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize , ...  )            
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");
		
	FREE_STDERR_COPY_ERR( RIO_STD_Driver_GetValue( hHandle , pGetCommandName , pGetValue , piValueLength , piValueSize ));
		
Error:
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** Standard Device Get Value Ex ***** ***** ***** ***** ***** ***** *****/  
void*	DLLEXPORT	StandardDevice_GetValue_Ex ( int hHandle , char *pCommandName , void **pGetValue , int *piValueLength , int *piValueSize , ...  )            
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");
		
	FREE_STDERR_COPY_ERR( RIO_STD_Driver_GetValue_Ex( hHandle , pCommandName , pGetValue , piValueLength , piValueSize ));
		
Error:
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** Standard Device Close ***** ***** ***** ***** ***** ***** *****/ 
void*	DLLEXPORT	StandardDevice_Close( int *pHandle )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL.");

	FREE_STDERR_COPY_ERR( RIO_STD_Driver_Close( pHandle )); 
	
Error:
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** Standard Device Check Connection ***** ***** ***** ***** ***** ***** *****/ 
void*	DLLEXPORT	StandardDevice_Check_Connection( int hHandle , char *pCommandName , int *piStatus )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( RIO_STD_Driver_Check_Connection( hHandle , pCommandName , piStatus ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** Standard Device Unplug ***** ***** ***** ***** ***** ***** *****/  
void*	DLLEXPORT	StandardDevice_Unplug( int hHandle , double lfTimeOut )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( RIO_STD_Driver_Unplug( hHandle , lfTimeOut ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** Standard Device Plug In ***** ***** ***** ***** ***** ***** *****/ 
void*	DLLEXPORT	StandardDevice_PlugIn( int hHandle , double lfTimeOut )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( RIO_STD_Driver_PlugIn( hHandle , lfTimeOut ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** Standard Device Enable Plug In Automatically ***** ***** ***** ***** ***** ***** *****/
void*	DLLEXPORT	StandardDevice_EnablePlugInAutomatically( int hHandle , int bEnable )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( RIO_STD_Driver_EnablePlugInAutomatically( hHandle , bEnable ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** Standard Device Install Receive Callback Function ***** ***** ***** ***** ***** ***** *****/ 
void*	DLLEXPORT	StandardDevice_InstallReceiveCallbackFunction( int hHandle , void *pfFunction , void *pData )  
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( RIO_STD_Driver_InstallReceiveCallbackFunction( hHandle , pfFunction , pData ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** Standard Device Load Config File ***** ***** ***** ***** ***** ***** *****/ 
void*	DLLEXPORT	StandardDevice_LoadConfigFile( int hHandle , char *pFileName )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( RIO_STD_Driver_LoadConfigFile( hHandle , pFileName ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** Standard Device Load Config Buffer ***** ***** ***** ***** ***** ***** *****/  
void*	DLLEXPORT	StandardDevice_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( RIO_STD_Driver_LoadConfigBuffer( hHandle , pBuffer , iBufferSize ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** Standard Device Get Commands List ***** ***** ***** ***** ***** ***** *****/ 
void*	DLLEXPORT	StandardDevice_Get_Commands_List( int hHandle , char ***pCommandsList , int *piNumberOfCommands )     
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( RIO_STD_Driver_Get_Commands_List( hHandle , pCommandsList , piNumberOfCommands ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

//==============================================================================    


// For Debug only
//============================================================================== 
void main( void )
{
	int 					iStatus									=   0;
	int                     hParentInstrumentHandle					=   0;
	int						hInstrumentHandle						=   0;
	char					szRsrcAddress[LOW_STRING]	        	=	"10.0.1.108:14564"; 
	char					szCommandName[LOW_STRING]				=   {0}; 
	int                     iIsCommandSuccessful 					=   0; 
	double                  dResult							        =   0.0;
	unsigned char           ucResult							    =   0;
	int						iValueLength							=   0;
	int						iValueSize								=   0; 
	char                    *pReplay 								=   NULL;
	char                    *pModel     							=   NULL;
	char                    *pFirmware						     	=   NULL;
	
	RIO_STD_Driver_Init( szRsrcAddress , &hParentInstrumentHandle ); 
	
	RIO_STD_Driver_SetValue( hParentInstrumentHandle , "GET_AI_LINE_0_VALUE"   , &iIsCommandSuccessful , 1 , sizeof(iIsCommandSuccessful));
	RIO_STD_Driver_GetValue( hParentInstrumentHandle , &szCommandName , (void**)&pReplay , &iValueLength , &iValueSize ); 
	
	if (( pReplay ) || (iValueLength * iValueSize == sizeof (double)))
	{
		memcpy (&dResult, pReplay, sizeof (double));
	}
	
	
	RIO_STD_Driver_Close( &hParentInstrumentHandle ); 
	
	
	
	iStatus = GetCommandItemListSize();
																 
	iStatus =  RIO_Init ( hParentInstrumentHandle , szRsrcAddress , &hInstrumentHandle );
	iStatus =  RIO_Reset( hInstrumentHandle );
	iStatus =  RIO_Get_Device_Info_Instrument_Handle ( hInstrumentHandle, &pModel, &pFirmware ); 
	
	iStatus =  RIO_Set_AO_Line_Value  ( hInstrumentHandle, 1, 1.5 );
	iStatus =  RIO_Set_DO_Line_Value  ( hInstrumentHandle, 1, 0 ); 
	iStatus =  RIO_Set_DO_Line_Value  ( hInstrumentHandle, 1, 1 ); 
	
	iStatus =  RIO_Get_AI_Line_Value  ( hInstrumentHandle, 1, &dResult );
    iStatus =  RIO_Get_AO_Line_Value  ( hInstrumentHandle, 1, &dResult ); 
    iStatus =  RIO_Get_DI_Line_Value  ( hInstrumentHandle, 1, &ucResult ); 
    iStatus =  RIO_Get_DO_Line_Value  ( hInstrumentHandle, 1, &ucResult ); 
	
	iStatus =  RIO_Get_AI_Line_Config ( hInstrumentHandle, 1, &ucResult ); 
	iStatus =  RIO_Get_DI_Line_Config ( hInstrumentHandle, 1, &ucResult );
	
	iStatus =  RIO_Set_AI_Line_Config ( hInstrumentHandle, 2, 4, 1); 
	iStatus =  RIO_Set_DI_Line_Config ( hInstrumentHandle, 10, 1 ); 
	
	iStatus =  RIO_Close ( &hInstrumentHandle ); 
	
	return;  
}

//==============================================================================    
