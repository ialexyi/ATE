//==============================================================================
// Include files

#include "windows.h"
#include "cvidef.h" 
#include <utility.h>
#include "GlobalDefines.h"
#include "toolbox.h"
#include "Communication_Low_Level_Standard.h"

//==============================================================================
// Constants

#define			DEVICE_ID			STANDARD_COMMUNICATION_LOW_LEVEL_DEVICE_ID

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
	
	FREE_STDERR_COPY_ERR( COM_LL_STD_Driver_Config_Copy_STD_CallBackSet( pHandle , pCallBackSet ));

Error:
	
	RETURN_STDERR_POINTER;
}

//typedef void* (*pfSTD_Config_Install_CommentCallback) ( int *pHandle , pfCommentCallback fFunction , void *pData , int type);
void*	DLLEXPORT		Config_Install_CommentCallback( int *pHandle , pfCommentCallback fFunction , void *pData , int type) 
{
	STD_ERROR				StdError						=	{0};
						
	FREE_STDERR_COPY_ERR( COM_LL_STD_Driver_Config_Install_CommentCallback( pHandle , fFunction , pData , type));

Error: 	
	
	RETURN_STDERR_POINTER;	
}

//typedef void* (*pfSTD_Config_Install_ConfigValueCallback) ( int *pHandle , pfConfigValueCallback fFunction , void *pData , int type);
void*	DLLEXPORT		Config_Install_ConfigValueCallback( int *pHandle , pfConfigValueCallback fFunction , void *pData , int type) 
{
	STD_ERROR				StdError						=	{0};
						
	
	FREE_STDERR_COPY_ERR( COM_LL_STD_Driver_Config_Install_ConfigValueCallback( pHandle , fFunction , pData , type));
	
Error: 
	
	RETURN_STDERR_POINTER;	
}

//typedef void* (*pfSTD_Config_Install_CheckForBreakCallback) ( int *pHandle , pfCheckForBreakCallback fFunction , void *pData , int type);
void*	DLLEXPORT		Config_Install_CheckForBreakCallback( int *pHandle , pfCheckForBreakCallback fFunction , void *pData , int type) 
{
	STD_ERROR				StdError						=	{0};
						
	
	FREE_STDERR_COPY_ERR( COM_LL_STD_Driver_Config_Install_CheckForBreakCallback( pHandle , fFunction , pData , type));
	
Error: 
	
	RETURN_STDERR_POINTER;	
}

//typedef void* (*pfSTD_Config_LOG_SetAllowState) ( int iHandle , int bAllowLog ); 
void*	DLLEXPORT		Config_LOG_SetAllowState( int iHandle , int bAllowLog ) 
{
	STD_ERROR				StdError						=	{0};
	
	FREE_STDERR_COPY_ERR( COM_LL_STD_Driver_Config_LOG_SetAllowState( iHandle , bAllowLog ));
	
Error: 
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_Init( int iDeviceID , char *pszConnectionName , char *pszAddress , int *pHandle , ... )
{
	STD_ERROR							StdError											=	{0};
	
	int									iLocalDeviceID										=	0;
	
	iLocalDeviceID = DEVICE_ID;
	
	IF (( iDeviceID != iLocalDeviceID ) , "Illegal Device ID");
	
	FREE_STDERR_COPY_ERR( COM_LL_STD_Driver_Init( pszConnectionName , pszAddress , pHandle )); 

Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_GetConnectionAddress( int hHandle , char **pszAddress )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");
	
	FREE_STDERR_COPY_ERR( COM_LL_STD_Driver_GetConnectionAddress( hHandle , pszAddress ));
	
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
	
	FREE_STDERR_COPY_ERR( COM_LL_STD_Driver_SetValue( hHandle , pCommandName , pValue , iValueLength , iValueSize ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize , ...  )            
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");
		
	FREE_STDERR_COPY_ERR( COM_LL_STD_Driver_GetValue( hHandle , pGetCommandName , pGetValue , piValueLength , piValueSize ));
		
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_GetValue_Ex ( int hHandle , char *pCommandName , void **pGetValue , int *piValueLength , int *piValueSize , ...  )            
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");
		
	FREE_STDERR_COPY_ERR( COM_LL_STD_Driver_GetValue_Ex( hHandle , pCommandName , pGetValue , piValueLength , piValueSize ));
		
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_Close( int *pHandle )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL.");

	FREE_STDERR_COPY_ERR( COM_LL_STD_Driver_Close( pHandle )); 
	
Error:
	
	RETURN_STDERR_POINTER;	
}


void*	DLLEXPORT		StandardDevice_Unplug( int hHandle , double lfTimeOut )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( COM_LL_STD_Driver_Unplug( hHandle , lfTimeOut ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}


void*	DLLEXPORT		StandardDevice_PlugIn( int hHandle , double lfTimeOut )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( COM_LL_STD_Driver_PlugIn( hHandle , lfTimeOut ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}


void*	DLLEXPORT		StandardDevice_EnablePlugInAutomatically( int hHandle , int bEnable )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( COM_LL_STD_Driver_EnablePlugInAutomatically( hHandle , bEnable ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_InstallReceiveCallbackFunction( int hHandle , void *pfFunction , void *pData )  
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( COM_LL_STD_Driver_InstallReceiveCallbackFunction( hHandle , pfFunction , pData ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_LoadConfigFile( int hHandle , char *pFileName )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( COM_LL_STD_Driver_LoadConfigFile( hHandle , pFileName ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}


void*	DLLEXPORT		StandardDevice_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( COM_LL_STD_Driver_LoadConfigBuffer( hHandle , pBuffer , iBufferSize ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_Check_Connection( int hHandle , char *pCommandName , int *piStatus )
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( COM_LL_STD_Driver_Check_Connection( hHandle , pCommandName , piStatus ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}

void*	DLLEXPORT		StandardDevice_Get_Commands_List( int hHandle , char ***pCommandsList , int *piNumberOfCommands )     
{
	STD_ERROR							StdError											=	{0};
	
	IF (( hHandle == 0 ) , "Empty handle");

	FREE_STDERR_COPY_ERR( COM_LL_STD_Driver_Get_Commands_List( hHandle , pCommandsList , piNumberOfCommands ));
	
Error:
	
	RETURN_STDERR_POINTER;	
}
/*
void main( void )
{
 	int					hHandle				=	0;
	
//	char				*pGetCommandName	=	0;
	
	void				*pGetValue			=	0;
	
	int					iDataLength			=	0,
						iDataSize			=	0;
	
	int					iDcaValue			=	0,
						iPortSelect			=	0,
						iGroupSelect		=	0,
						iOptoSelect			=	0;
	
	StandardDevice_Init( DEVICE_ID , "Gragon TX" , "" , &hHandle ) ;
	
	StandardDevice_LoadConfigFile( hHandle , "C:\\emi\\WorkSpace\\Dragon_TX_DaqMX.daqcnf" ); 
	
	StandardDevice_GetValue_Ex( hHandle , "READ_FORWARD_WAVE" , &pGetValue , &iDataLength , &iDataSize );    
	StandardDevice_GetValue_Ex( hHandle , "READ_REVERSE_WAVE" , &pGetValue , &iDataLength , &iDataSize );    
	
	iDcaValue = 5;
	StandardDevice_SetValue( hHandle , "SET_DCA" , &iDcaValue , 1 , sizeof(iDcaValue) ); 
	
	iPortSelect = 2;
	StandardDevice_SetValue( hHandle , "PORT_SELECT" , &iPortSelect , 1 , sizeof(iDcaValue) ); 
	
	iGroupSelect = 1;
	StandardDevice_SetValue( hHandle , "GROUP_SELECT" , &iGroupSelect , 1 , sizeof(iDcaValue) ); 
	
	iOptoSelect = 2;
	StandardDevice_SetValue( hHandle , "OPTO_SELECT" , &iOptoSelect , 1 , sizeof(iDcaValue) ); 
	

	
	StandardDevice_Close( &hHandle );    
		
	return;
}
*/
