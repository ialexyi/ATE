//==============================================================================
// Include files

#include "windows.h"
#include "cvidef.h" 
#include <utility.h>
#include "GlobalDefines.h"
#include "toolbox.h"
#include "STD_CLI_Driver_Standard.h"

//==============================================================================
// Constants

#define			DEVICE_ID			STANDARD_CLI_DRIVER_DEVICE_ID

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

unsigned __int64	DLLEXPORT		StandardDevice_Get_IdentificationNumber( void )
{
	return (unsigned __int64)DEVICE_ID;
}

void* DLLEXPORT StandardDevice_GetErrorTextMessage ( int vhInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR				StdError					=	{0};    

Error: 
	
	RETURN_STDERR_POINTER;
}


void*	DLLEXPORT		Equipment_Info ( int hLowLevelHandle , char *pAddress , char **pVendor , char **pSerialNumber , char **pModel , char **pFirmware )
{
	STD_ERROR						StdError									=	{0};
	RETURN_STDERR_POINTER;
}

void*	DLLEXPORT		Equipment_GetLowLevelHandle( int hHandle , int *phLowLevelHandle )
{
	STD_ERROR						StdError									=	{0};
	
	FREE_STDERR_COPY_ERR( STD_CLI_GetLowLevelHandle( hHandle , phLowLevelHandle ));

Error:
	
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
	
	FREE_STDERR_COPY_ERR( STD_CLI_Driver_Config_Copy_STD_CallBackSet( pHandle , pCallBackSet ));

Error:
	
	RETURN_STDERR_POINTER;
}

//typedef void* (*pfSTD_Config_Install_CommentCallback) ( int *pHandle , pfCommentCallback fFunction , void *pData , int type);
void*	DLLEXPORT		Config_Install_CommentCallback( int *pHandle , pfCommentCallback fFunction , void *pData , int type) 
{
	STD_ERROR				StdError						=	{0};
						
	FREE_STDERR_COPY_ERR( STD_CLI_Driver_Config_Install_CommentCallback( pHandle , fFunction , pData , type));

Error: 	
	
	RETURN_STDERR_POINTER;	
}

//typedef void* (*pfSTD_Config_Install_ConfigValueCallback) ( int *pHandle , pfConfigValueCallback fFunction , void *pData , int type);
void*	DLLEXPORT		Config_Install_ConfigValueCallback( int *pHandle , pfConfigValueCallback fFunction , void *pData , int type) 
{
	STD_ERROR				StdError						=	{0};
						
	
	FREE_STDERR_COPY_ERR( STD_CLI_Driver_Config_Install_ConfigValueCallback( pHandle , fFunction , pData , type));
	
Error: 
	
	RETURN_STDERR_POINTER;	
}

//typedef void* (*pfSTD_Config_Install_CheckForBreakCallback) ( int *pHandle , pfCheckForBreakCallback fFunction , void *pData , int type);
void*	DLLEXPORT		Config_Install_CheckForBreakCallback( int *pHandle , pfCheckForBreakCallback fFunction , void *pData , int type) 
{
	STD_ERROR				StdError						=	{0};
						
	
	FREE_STDERR_COPY_ERR( STD_CLI_Driver_Config_Install_CheckForBreakCallback( pHandle , fFunction , pData , type));
	
Error: 
	
	RETURN_STDERR_POINTER;	
}

//typedef void* (*pfSTD_Config_LOG_SetAllowState) ( int iHandle , int bAllowLog ); 
void*	DLLEXPORT		Config_LOG_SetAllowState( int iHandle , int bAllowLog ) 
{
	STD_ERROR				StdError						=	{0};
	
	FREE_STDERR_COPY_ERR( STD_CLI_Driver_Config_LOG_SetAllowState( iHandle , bAllowLog ));
	
Error: 
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_Init( int iDeviceID , char *pszConnectionName , char *pszAddress , int *pHandle , ... )
{
	STD_ERROR							StdError											=	{0};
	
	int									iLocalDeviceID										=	0;
	
	iLocalDeviceID = DEVICE_ID;
	
	IF (( iDeviceID != iLocalDeviceID ) , "Illegal Device ID");
	
	FREE_STDERR_COPY_ERR( STD_CLI_Driver_Init( pszConnectionName , pszAddress , pHandle )); 

Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_GetConnectionAddress( int hHandle , char **pszAddress )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");
	
	FREE_STDERR_COPY_ERR( STD_CLI_Driver_GetConnectionAddress( hHandle , pszAddress ));
	
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
	
	FREE_STDERR_COPY_ERR( STD_CLI_Driver_SetValue( hHandle , pCommandName , pValue , iValueLength , iValueSize ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize , ...  )            
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");
		
	FREE_STDERR_COPY_ERR( STD_CLI_Driver_GetValue( hHandle , pGetCommandName , pGetValue , piValueLength , piValueSize ));
		
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_GetValue_Ex ( int hHandle , char *pCommandName , void **pGetValue , int *piValueLength , int *piValueSize , ...  )            
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");
		
	FREE_STDERR_COPY_ERR( STD_CLI_Driver_GetValue_Ex( hHandle , pCommandName , pGetValue , piValueLength , piValueSize ));
		
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_Close( int *pHandle )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL.");

	FREE_STDERR_COPY_ERR( STD_CLI_Driver_Close( pHandle )); 
	
Error:
	
	RETURN_STDERR_POINTER;	
}


void*	DLLEXPORT		StandardDevice_Unplug( int hHandle , double lfTimeOut )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( STD_CLI_Driver_Unplug( hHandle , lfTimeOut ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}


void*	DLLEXPORT		StandardDevice_PlugIn( int hHandle , double lfTimeOut )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( STD_CLI_Driver_PlugIn( hHandle , lfTimeOut ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}


void*	DLLEXPORT		StandardDevice_EnablePlugInAutomatically( int hHandle , int bEnable )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( STD_CLI_Driver_EnablePlugInAutomatically( hHandle , bEnable ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_InstallReceiveCallbackFunction( int hHandle , void *pfFunction , void *pData )  
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( STD_CLI_Driver_InstallReceiveCallbackFunction( hHandle , pfFunction , pData ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_LoadConfigFile( int hHandle , char *pFileName )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( STD_CLI_Driver_LoadConfigFile( hHandle , pFileName ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}


void*	DLLEXPORT		StandardDevice_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( STD_CLI_Driver_LoadConfigBuffer( hHandle , pBuffer , iBufferSize ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_Check_Connection( int hHandle , char *pCommandName , int *piStatus )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( STD_CLI_Driver_Check_Connection( hHandle , pCommandName , piStatus ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_Get_Commands_List( int hHandle , char ***pCommandsList , int *piNumberOfCommands )     
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( STD_CLI_Driver_Get_Commands_List( hHandle , pCommandsList , piNumberOfCommands ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}


void main( void )
{
 	int					hHandle				=	0;
	
	void				*pGetValue			=	0;
	
	int					iDataLength			=	0,
						iDataSize			=	0;																						 
	
	int					iFrequencyIndex		=	0,
						iPowerIndex			=	0;
	
	char				szMessage[64]		=	{"ABCD"};
	
	int 				iValueLength		=	0,
						iValueSize	  		=	0;
	
	char				szTextValue[512]	=  {0};  	 //for test
	char				szTextValue2[512]	=  {0}; 	 // for test
	
	char                *pszGetCommandName  =	NULL;
	
	
	
	
	
/*	
	StandardDevice_Init( DEVICE_ID , "HPS" , "ASRL4" , &hHandle ) ;
	StandardDevice_LoadConfigFile( hHandle , "C:\\emi\\WorkSpace\\B3I_MCU_TerminalControl.clicnf" ); 
	
	StandardDevice_SetValue( hHandle , "WAKE_UP_SIGNAL" , NULL , 0 , 0 ); 
	
	StandardDevice_SetValue( hHandle , "TEST_WAKE_UP_SIGNAL" , NULL , 0 , 0 );
	StandardDevice_GetValue( hHandle , &pszGetCommandName , (void**)&pGetValue , &iValueLength , &iValueSize ); 
*/	
	
	
	
	
	
	StandardDevice_Init( DEVICE_ID , "HPS" , "ASRL16" , &hHandle ) ;
	
	StandardDevice_LoadConfigFile( hHandle , "C:\\emi\\WorkSpace\\B3I_HPS_TerminalControl.clicnf" ); 
	
	StandardDevice_SetValue( hHandle , "UART_CLEAN" , NULL , 0 , 0 );
	
	StandardDevice_SetValue( hHandle , "START_RECEIVE_UART" , NULL , 0 , 0 );
	
	StandardDevice_SetValue( hHandle , "SEND_UART" , szMessage , strlen(szMessage) , sizeof(char) ); 
	
	StandardDevice_SetValue( hHandle , "UART_GET_DATA" , NULL , 0 , 0 );   
	StandardDevice_GetValue( hHandle , &pszGetCommandName , (void**)&pGetValue , &iValueLength , &iValueSize ); 
	
	StandardDevice_SetValue( hHandle , "UART_CLEAN" , NULL , 0 , 0 );
	
	//StandardDevice_SetValue( hHandle , "ROOT" , NULL , 0 , 0 ); 
	
	//StandardDevice_SetValue( hHandle , "USB_DEVICE_LIST" , NULL , 0, 0 );											 //for test
	//StandardDevice_GetValue( hHandle , &pszGetCommandName , (void**)&pGetValue , &iValueLength , &iValueSize ); 	 //for test
	
	//memset( szTextValue , 0 , sizeof(szTextValue) );
	//memcpy( szTextValue , pGetValue , ( iValueLength*iValueSize ));
	
//	StandardDevice_SetValue( hHandle , "PING" , NULL , 0, 0 );														 //for test
//	StandardDevice_GetValue( hHandle , &pszGetCommandName , (void**)&pGetValue , &iValueLength , &iValueSize );		 //for test
	
//	memset( szTextValue2 , 0 , sizeof(szTextValue2) );
//	memcpy( szTextValue2 , pGetValue , ( iValueLength*iValueSize ));
	
/*	
	StandardDevice_SetValue( hHandle , "START_RECEIVE_UART" , NULL , 0 , 0 );
	
	StandardDevice_SetValue( hHandle , "SEND_UART" , szMessage , strlen(szMessage) , sizeof(char) ); 
	
	StandardDevice_SetValue( hHandle , "UART_GET_DATA" , NULL , 0 , 0 );   
	StandardDevice_GetValue( hHandle , &pszGetCommandName , (void**)&pGetValue , &iValueLength , &iValueSize ); 
	
	StandardDevice_SetValue( hHandle , "UART_CLEAN" , NULL , 0 , 0 );    */


//  Another commands

//	StandardDevice_SetValue( hHandle , "REBOOT" , NULL , 0 , 0 );  
//	StandardDevice_GetValue_Ex ( hHandle , "USB_DEVICE_LIST" , (void**)&pGetValue , &iValueLength , &iValueSize );  //for test
//	StandardDevice_SetValue( hHandle , "DUMMY" , NULL , 0, 0 );											 //for test   
	StandardDevice_Close( &hHandle );   
	
	
	StandardDevice_Init( DEVICE_ID , "HPS" , "ASRL3" , &hHandle ) ;
	
	StandardDevice_LoadConfigFile( hHandle , "C:\\emi\\WorkSpace\\B3I_HPS_TerminalControl.clicnf" ); 
	
	StandardDevice_SetValue( hHandle , "ROOT" , NULL , 0 , 0 ); 
	
	StandardDevice_SetValue( hHandle , "USB_DEVICE_LIST" , NULL , 0, 0 );											 //for test
	StandardDevice_GetValue( hHandle , &pszGetCommandName , (void**)&pGetValue , &iValueLength , &iValueSize );
	
	StandardDevice_Close( &hHandle ); 
	
	return;
}

