//==============================================================================
// Include files
#include "Windows.h"
#include "toolbox.h"
#include <ansi_c.h>
#include <userint.h>
#include <utility.h>
#include "GlobalDefines.h"
#include "ModBus_ProtocolDefines.h"
#include "Communication_Low_Level.h"
#include "Modbus_OvenController_Standard.h"
#include <formatio.h>

//==============================================================================
// Constants

#define				MIN_MESSAGE_LEN				8
#define				DEVICE_ID					STANDARD_OVEN_CONTROL_MODBUS_DEVICE_ID
//==============================================================================
// Types

typedef struct
{
	int							hCommunicationHandle;
	//------------- Communication ---------------//
	unsigned short 				*pData;
	unsigned int				iDataLength;

	unsigned char 				*pSourceBuffer;
	unsigned int				iSourceBufferLength,
						 		iSendPacketCounter;
	
	int							bDataUpdated,
								iNumberOfCommunicationTry;

	int							iLastSentCommandIndex;
	
	int							iConnectionType;
	
	double						lfSendTimeOut,
								lfReceiveTimeOut;
								
	tsSTD_CallBackSet			*ptCallbacks;
	
	tsMODBUS_CommandItem		*pMODBUS_CommandList;
	int							listSize;

	int							bEnableAutoPlugIn;							
	
	int							bClosed;
	
	void						*pCallbackFunction,
								*pCallbackData;
	
	STD_ERROR					*pLastError; 
	
} tsLocalStorage;


//==============================================================================
// Static global variables

//==============================================================================
// Static functions
void*   DLLEXPORT       MODBUS_Driver_Send_Patern( int hCommunicationHandle , tsMODBUS_CommandItem tCommandItem , unsigned short *pData , int iDataLength , unsigned int iSendPacketCounter );

void	MODBUS_Driver_ReceiveCallBack( void *pData );

unsigned short CalculateModBusCRC_16( unsigned char *buffer , unsigned int length );

//==============================================================================
// Global variables

//==============================================================================
// Global functions
 

void*	DLLEXPORT	MODBUS_Driver_Config_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								handle										=	0;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );
	
	if ( *pHandle == 0 )
	{
		CHK_CMT(CmtNewTSV ( sizeof(tsLocalStorage) , pHandle ));
	}
	
	handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	if ( pLocalStorage->ptCallbacks == NULL )
	{
		CALLOC_ERR( pLocalStorage->ptCallbacks , 1 , sizeof(tsSTD_CallBackSet));  
	}
	
	memcpy( pLocalStorage->ptCallbacks , pCallBackSet , sizeof(tsSTD_CallBackSet));  

Error:
	
	if ( handle )
		CmtReleaseTSVPtr (handle);
	
	RETURN_STDERR_POINTER;
}

void*	DLLEXPORT	MODBUS_Driver_Config_LOG_SetAllowState( int iHandle , int bAllowLog ) 
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	IF (( iHandle == 0 ) , "Driver has not initialized" );

	CHK_CMT( CmtGetTSVPtr ( iHandle , &pLocalStorage ));
	
	pLocalStorage->ptCallbacks->bDisableLog = !bAllowLog;

Error:
	
	if ( iHandle )
		CmtReleaseTSVPtr (iHandle);
	
	RETURN_STDERR_POINTER;
}
 
//------------------- Standard UUT Driver Protocol --------------------------------------------------//
void*	DLLEXPORT	MODBUS_Driver_Config_Install_CommentCallback( CmtTSVHandle *pHandle , pfCommentCallback fFunction , void *pData , int type) 
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								handle										=	0;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );
	
	if ( *pHandle == 0 )
	{
		CHK_CMT(CmtNewTSV ( sizeof(tsLocalStorage) , pHandle ));
	}
	
	handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	if ( pLocalStorage->ptCallbacks == NULL )
	{
		CALLOC_ERR( pLocalStorage->ptCallbacks , 1 , sizeof(tsSTD_CallBackSet));  
	}
	
	pLocalStorage->ptCallbacks->fCommentCallback = fFunction;
	pLocalStorage->ptCallbacks->pCommentCallbackData = pData;
	pLocalStorage->ptCallbacks->commentType = type;
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr (handle);
	
	RETURN_STDERR_POINTER;
}

void*	DLLEXPORT	MODBUS_Driver_Config_Install_ConfigValueCallback( CmtTSVHandle *pHandle , pfConfigValueCallback fFunction , void *pData , int type) 
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								handle										=	0;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );
	
	if ( *pHandle == 0 )
	{
		CHK_CMT(CmtNewTSV ( sizeof(tsLocalStorage) , pHandle ));
	}
	
	handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	if ( pLocalStorage->ptCallbacks == NULL )
	{
		CALLOC_ERR( pLocalStorage->ptCallbacks , 1 , sizeof(tsSTD_CallBackSet));  
	}
	
	pLocalStorage->ptCallbacks->fConfigValueCallback = fFunction;
	pLocalStorage->ptCallbacks->pConfigValueCallbackData = pData;
	pLocalStorage->ptCallbacks->configType = type;
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr (handle);
	
	
	RETURN_STDERR_POINTER;
}																	   		
   
void*	DLLEXPORT	MODBUS_Driver_Config_Install_CheckForBreakCallback( CmtTSVHandle *pHandle , pfCheckForBreakCallback fFunction , void *pData , int type) 
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								handle										=	0;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );
	
	if ( *pHandle == 0 )
	{
		CHK_CMT(CmtNewTSV ( sizeof(tsLocalStorage) , pHandle ));
	}
	
	handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	if ( pLocalStorage->ptCallbacks == NULL )
	{
		CALLOC_ERR( pLocalStorage->ptCallbacks , 1 , sizeof(tsSTD_CallBackSet));  
	}
	
	pLocalStorage->ptCallbacks->fCheckForBreakCallback = fFunction;
	pLocalStorage->ptCallbacks->pCheckForBreakCallbackData = pData;
	pLocalStorage->ptCallbacks->breakType = type;
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr (handle);
	
	
	RETURN_STDERR_POINTER;
}

//typedef void (*pfCloseCallBack) ( unsigned long hCommHandle , int error , void *data );
void   DLLEXPORT   MODBUS_Driver_Close_CallBack ( int hCommHandle , int error , void *pData )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								*pHandle									=	0,
									handle										=	0;
	pHandle = pData;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );

	handle = *pHandle;
	
	FREE(pHandle);
	
	if ( CmtGetTSVPtr ( handle , &pLocalStorage ) == 0 )
	{
		CALLOC( pLocalStorage->pLastError , 1 , sizeof(STD_ERROR));
		
		pLocalStorage->pLastError->error = error;
		
		pLocalStorage->bClosed = 1;
	}
	else
		handle = 0;
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return;
}
  
//typedef void (*pfErrorCallBack) ( unsigned long hCommHandle , void *pErrEv , void *data );  
void   DLLEXPORT   MODBUS_Driver_Error_CallBack ( int hCommHandle , void *pStdError , void *pData )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								*pHandle									=	0,
									handle										=	0;
	
	pHandle = pData;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );

	handle = *pHandle;
	
	if ( handle == 0 )
		return;
	
	if ( CmtGetTSVPtr ( handle , &pLocalStorage ) == 0 )
	{
		do
		{
			if ( pStdError )
			{
				FREE_STDERR_COPY(pStdError);
				pStdError = NULL;
				
				if ( StdError.error ) //== -1073807360 )
				{
					OutputDebugString("\t\tUUT Driver Error : Unplugging communication.");   
					FREE_STDERR_COPY_ERR( Communication_Unplug( pLocalStorage->hCommunicationHandle , 10.0 ));
					
					if ( pLocalStorage->bEnableAutoPlugIn )
					{
						OutputDebugString("\t\tUUT Driver       : Plugging back communication.");   
						FREE_STDERR_COPY_ERR( Communication_PlugIn( pLocalStorage->hCommunicationHandle , 10.0 )); 
					}
					else
						OutputDebugString("\t\tUUT Driver       : Plugging back is disable.");   
						
					break;
				}
	
				FREE_STDERR_COPY_ERR(pStdError);
			}
			
		} while(0);
	}
	else
		handle = 0;
Error:
	
	FREE(pStdError);
	
	if ( handle )
	{
		//GET_RET( pLocalStorage->pLastError ); 
		CmtReleaseTSVPtr ( handle );
	}
	
	return;
}

//====================================================================================================//
//typedef void (*pfReceiveCallBack) ( char *buffer , unsigned int count , void *data ); 
void	DLLEXPORT	MODBUS_Driver_Receive_CallBack( unsigned char *pBuffer , unsigned int iBufferLen , void *pData )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								index										=	0;
	
	unsigned short					calculatedCRC								=	0,
									receivedCRC									=	0;
	
	int								iPrefixSize									=	0;
	
	unsigned short					dataLength									=	0;
	
	unsigned short					*pWordItem									=	NULL;
	unsigned char					*pByteItem									=	NULL;

	unsigned short					usSendPacketCounter							=	0;
	
	int								iCalcReference								=	0,
									iCalcDifferent								=	0;
	
	int								*pHandle									=	0,
									handle										=	0;
	
	int								iLastSentCommandIndex						=	0;
	
	tsMODBUS_ProtocolItem			tCurrentPackage								=	{0};
	
	tuSweepConvertWord				tConvert,
									tConvertedData, 
									tCrcLittleEndian;

	tuConvert						tConvertBigSize;
	
	__int64							llLocalValue								=	0;

	unsigned char 					*pOriginalBuffer							=	NULL;
	
	pHandle = pData;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );
	IF (( pBuffer == NULL ) , "Receive NULL pointer buffer." );   
	IF (( iBufferLen > MAX_MODBUS_PROTOCOL_LENGTH ) , "Received package is too big" );
	IF (( iBufferLen < 3 ) , "Received empty package" );

	handle = *pHandle;
	
	pOriginalBuffer = pBuffer;
	
	if ( CmtGetTSVPtr ( handle , &pLocalStorage ) == 0 )
	{
		FREE_CALLOC_COPY( pLocalStorage->pSourceBuffer , iBufferLen + 100 , sizeof(char) , pBuffer , iBufferLen );   
		pLocalStorage->iSourceBufferLength = iBufferLen;
		pLocalStorage->iDataLength = 0;            
		
		GET_STDERR_CLEAR( pLocalStorage->pLastError ); 
		
		iLastSentCommandIndex = pLocalStorage->iLastSentCommandIndex;
		
		switch ( pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].bResponsePrefix )
		{
			case 1:
				iPrefixSize = sizeof(int)+sizeof(short);
				
				tConvertBigSize.buffer[1] = pBuffer[0];
				tConvertBigSize.buffer[0] = pBuffer[1];
			
				usSendPacketCounter = pLocalStorage->iSendPacketCounter;
				
				if ( usSendPacketCounter != tConvertBigSize.uWord )
				{
					RETURN;	
				}
				
				tConvertBigSize.buffer[1] = pBuffer[2];
				tConvertBigSize.buffer[0] = pBuffer[3];
			
				if ( pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].iResponsePrefix_ID != tConvertBigSize.uWord )
				{
					RETURN;	
				}
				
				tConvertBigSize.buffer[1] = pBuffer[4];
				tConvertBigSize.buffer[0] = pBuffer[5];
			
				iBufferLen -= iPrefixSize;
				
				if ( iBufferLen != tConvertBigSize.uWord )
				{
					RETURN;	
				}
				
				pBuffer += iPrefixSize;
					 
			default:
				
				break;
		}
	
		if ( iBufferLen <= 0 )
		{
			RETURN;	
		}
		
		memcpy( tCurrentPackage.max_size , pBuffer , iBufferLen ); 
		
		do
		{
			if ( pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].bResponseCrcBigEndian == 2 )  // without checksum
				break;
		
			memcpy( &(receivedCRC) , pBuffer + iBufferLen - 2 , 2 );
			
			calculatedCRC = CalculateModBusCRC_16( pBuffer , iBufferLen - 2 );
	
			tConvert.Word = calculatedCRC;
			tCrcLittleEndian.byte_lo = tConvert.byte_hi; 
			tCrcLittleEndian.byte_hi = tConvert.byte_lo;
	
			if ( pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].bResponseCrcBigEndian )
			{
				IF(( receivedCRC != calculatedCRC ) , "Checksum error" );
			}
			else
			{
				IF(( receivedCRC != tCrcLittleEndian.Word ) , "Checksum error" );
			}
			
		} while(0);
		
		iCalcReference = &tCurrentPackage; 
		
		switch( pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].iResponseType ) 
		{
			case PROTOCOL_TYPE_A:
				
				IF ((tCurrentPackage.tTypeA.dev_addr != pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].receive_side.tTypeA.dev_addr ) ,"Wrong Device" );
				IF ((tCurrentPackage.tTypeA.func_code != pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].receive_side.tTypeA.func_code ) ,"Wrong Function Code" );
				
				dataLength = tCurrentPackage.tTypeA.data_length;
				
				pLocalStorage->iDataLength = dataLength;
				FREE_CALLOC_ERR( pLocalStorage->pData , dataLength , sizeof(short));
				
				iCalcDifferent = (char*)&(tCurrentPackage.tTypeA.single_data_hi);
				pWordItem = pBuffer + (iCalcDifferent - iCalcReference);
				
				for ( index = 0 ; (( index < ( dataLength / 2 )) || ( index < 1 )) ; index++ ) 
				{
					tConvert.Word = *pWordItem;
					pWordItem++;
					
					if ( pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].bResponseDataByteBigEndian )
					{
						tConvertedData.byte_lo = tConvert.byte_hi;
						tConvertedData.byte_hi = tConvert.byte_lo;
					}
					else
					{
						tConvertedData.byte_lo = tConvert.byte_lo;
						tConvertedData.byte_hi = tConvert.byte_hi;
					}
					
					pLocalStorage->pData[index] = tConvertedData.Word;
				}
				
				break;
				
			case PROTOCOL_TYPE_B:
				
				IF ((tCurrentPackage.tTypeB.dev_addr != pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].receive_side.tTypeB.dev_addr ) ,"Wrong Device" );
				IF ((tCurrentPackage.tTypeB.func_code != pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].receive_side.tTypeB.func_code ) ,"Wrong Function Code" );
				
				dataLength = 1;
				
				pLocalStorage->iDataLength = dataLength;
				FREE_CALLOC_ERR( pLocalStorage->pData , dataLength , sizeof(short));
				
				tConvertedData.byte_hi = tCurrentPackage.tTypeB.num_of_hi;
				tConvertedData.byte_lo = tCurrentPackage.tTypeB.num_of_lo;
				
				pLocalStorage->pData[0] = tConvertedData.Word;
				
				break;
				
			case PROTOCOL_TYPE_C:
				IF ((tCurrentPackage.tTypeC.dev_addr != pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].receive_side.tTypeC.dev_addr ) ,"Wrong Device" );
				IF ((tCurrentPackage.tTypeC.func_code != pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].receive_side.tTypeC.func_code ) ,"Wrong Function Code" );
				
				dataLength = 1;
				
				pLocalStorage->iDataLength = dataLength;
				FREE_CALLOC_ERR( pLocalStorage->pData , dataLength , sizeof(short));
				
				if ( pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].bResponseDataByteBigEndian )
				{
					tConvertedData.byte_hi  = tCurrentPackage.tTypeC.single_data_lo;
					tConvertedData.byte_lo = tCurrentPackage.tTypeC.single_data_hi;
				}
				else
				{
					tConvertedData.byte_hi  = tCurrentPackage.tTypeC.single_data_hi;
					tConvertedData.byte_lo = tCurrentPackage.tTypeC.single_data_lo;
				}
				
				pLocalStorage->pData[0] = tConvertedData.Word;
				
				break;
				
			case PROTOCOL_TYPE_D:
				IF ((tCurrentPackage.tTypeD.dev_addr != pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].receive_side.tTypeD.dev_addr ) ,"Wrong Device" );
				IF ((tCurrentPackage.tTypeD.func_code != pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].receive_side.tTypeD.func_code ) ,"Wrong Function Code" );
			
				dataLength = tCurrentPackage.tTypeD.data_length;
				
				pLocalStorage->iDataLength = dataLength;
				
				FREE_CALLOC_ERR( pLocalStorage->pData , dataLength , sizeof(short));
				
				iCalcDifferent = (char*)&(tCurrentPackage.tTypeD.single_data_lo);
				pByteItem = pBuffer + (iCalcDifferent - iCalcReference);
				
				for ( index = 0 ; index < dataLength ; index++ ) 
				{
					pLocalStorage->pData[index] = *(pByteItem++);
				}
				
				break;
				
			case PROTOCOL_TYPE_E:
				IF ((tCurrentPackage.tTypeE.dev_addr != pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].receive_side.tTypeE.dev_addr ) ,"Wrong Device" );
				IF ((tCurrentPackage.tTypeE.func_code != pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].receive_side.tTypeE.func_code ) ,"Wrong Function Code" );
			
				dataLength = tCurrentPackage.tTypeE.data_length;
				
				pLocalStorage->iDataLength = dataLength;
				FREE_CALLOC_ERR( pLocalStorage->pData , dataLength , sizeof(short));
				
				iCalcDifferent = (char*)&(tCurrentPackage.tTypeE.single_data_hi);
				pWordItem = pBuffer + (iCalcDifferent - iCalcReference);  
				
				for ( index = 0 ; (( index < ( dataLength / 2 )) || ( index < 1 )) ; index++ ) 
				{
					tConvert.Word = *pWordItem;
					pWordItem++;
					
					if ( pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].bResponseDataByteBigEndian )
					{
						tConvertedData.byte_lo = tConvert.byte_hi;
						tConvertedData.byte_hi = tConvert.byte_lo;
					}
					else
					{
						tConvertedData.byte_lo = tConvert.byte_lo;
						tConvertedData.byte_hi = tConvert.byte_hi;
					}
					
					pLocalStorage->pData[index] = tConvertedData.Word;
				}
				
				break;
				
			case PROTOCOL_TYPE_F:
				IF ((tCurrentPackage.tTypeF.dev_addr != pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].receive_side.tTypeF.dev_addr ) ,"Wrong Device" );
				IF ((tCurrentPackage.tTypeF.func_code != pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].receive_side.tTypeF.func_code ) ,"Wrong Function Code" );
				
				dataLength = 1;
				
				pLocalStorage->iDataLength = dataLength;
				FREE_CALLOC_ERR( pLocalStorage->pData , dataLength , sizeof(short));

				pLocalStorage->pData[0] = tCurrentPackage.tTypeF.single_data_lo;
				
				break;
				
			case PROTOCOL_TYPE_G:
				IF ((tCurrentPackage.tTypeG.dev_addr != pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].receive_side.tTypeG.dev_addr ) ,"Wrong Device" );
				IF ((tCurrentPackage.tTypeG.func_code != pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].receive_side.tTypeG.func_code ) ,"Wrong Function Code" );
		
				dataLength = 1;
				
				pLocalStorage->iDataLength = dataLength;
				FREE_CALLOC_ERR( pLocalStorage->pData , dataLength , sizeof(short));

				pLocalStorage->pData[0] = tCurrentPackage.tTypeG.func_code;
				
				break;
				
			default:
				
				FORCE_ERR(-1,"Command has not defined yet"); 
				break;
		}

		if ( pLocalStorage->pCallbackFunction )
		{
			CALLOC_ERR( pHandle , 1 , sizeof(int));
			*pHandle = handle;
			pData = pHandle;
		
			PostDeferredCallToThread ( MODBUS_Driver_ReceiveCallBack , pData , CmtGetCurrentThreadID() );  
		}
	}
	else
		handle = 0;
		
Error:
	
	if ( handle )
	{
		pLocalStorage->bDataUpdated = 1;
		FREE_STDERR_POINTER( pLocalStorage->pLastError );
		GET_STDERR_CLEAR( pLocalStorage->pLastError );
		CmtReleaseTSVPtr ( handle );
	}
	
	FREE(pOriginalBuffer);
	
	return;
}

void	MODBUS_Driver_ReceiveCallBack( void *pData )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								index										=	0;
	
	unsigned char					checkSum									=	0,
									header										=	0, 
									opCode										=	0, 
									calcCheckSum								=	0;
	
	unsigned short					dataLength									=	0;
	
	int								*pHandle									=	0,
									handle										=	0;
	
	int								iLastSentCommandIndex						=	0;
	
	char							*pCommand									=	NULL,
									*pReceiveBuffer								=	NULL;

	int								*pValue										=	NULL;
		
	int								iValueSize									=	0,
									iBufferSize									=	0; 
		
	tfStdReceiveCallback			receiveFunction								=	NULL;
	
	pHandle = pData;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );

	handle = *pHandle;
	
	FREE(pHandle);
	
	if ( CmtGetTSVPtr ( handle , &pLocalStorage ) == 0 )
	{
		iLastSentCommandIndex = pLocalStorage->iLastSentCommandIndex;  
		
		CALLOC_COPY_STRING( pCommand , pLocalStorage->pMODBUS_CommandList[iLastSentCommandIndex].szCommandName );

		CALLOC_ERR( pValue , (pLocalStorage->iDataLength + 1) , sizeof(int));
		
		for ( index = 0 ; index < pLocalStorage->iDataLength ; index++ )   
			pValue[index] = pLocalStorage->pData[index];
		
		iValueSize = pLocalStorage->iDataLength;
		
		CALLOC_COPY_ERR( pReceiveBuffer , (pLocalStorage->iSourceBufferLength + 1) , 1 , pLocalStorage->pSourceBuffer , pLocalStorage->iSourceBufferLength );
		
		iBufferSize = pLocalStorage->iSourceBufferLength;
		
		receiveFunction = pLocalStorage->pCallbackFunction;
		
		pData = pLocalStorage->pCallbackData;
		
		CmtReleaseTSVPtr ( handle ); 
	}	
		
	if ( receiveFunction )
		receiveFunction( pData , pCommand , pValue , iValueSize , pReceiveBuffer , iBufferSize ); 
Error:
	
	return;
}

//typedef int (*pfAttributeCallBack) ( unsigned long hCommHandle , void *data ); 
int DLLEXPORT			MODBUS_Driver_Set_Attributes_CallBack( ViSession hCommHandle , void *pData )
{
	STD_ERROR						StdError									=	{0};
	
	tsSTD_CallBackSet				tSTD_CallBackSet							=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	ViUInt32						iDB_BaudRate							  	=	0,
									iDB_DataBits							  	=	0,
									iDB_Parity								  	=	0,
									iDB_StopBits							  	=	0,
									iDB_FlowCtrl							  	=	0,
									iDB_Default								  	=	0,
									iDB_TCP_IP_NoDelay							=	0,
									iDB_TCP_IP_KeepAlive						=	0;
									
	int								*pHandle									=	0,
									handle										=	0;
		
	char							szFormatedLog[LOW_STRING]					=	{0};
		
	pHandle = pData;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );

	handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStorage )); 
	
	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks);
	
	sprintf( szFormatedLog , "%s :: Configurating communication %%s" ,  pLocalStorage->ptCallbacks->szName );
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	
	if ( pLocalStorage->iConnectionType == 0 )
	{
		iDB_BaudRate = 19200;
	
		STDF_CONFIG_VALUE("BaudRate", VAL_INTEGER , 1 , iDB_BaudRate , iDB_BaudRate );	
	
		CHK_VSA( viSetAttribute ( hCommHandle , VI_ATTR_ASRL_BAUD , iDB_BaudRate ));
	
		iDB_DataBits = 8;
	
		STDF_CONFIG_VALUE("DataBits", VAL_INTEGER , 1 , iDB_DataBits , iDB_DataBits );
		
		CHK_VSA( viSetAttribute (hCommHandle, VI_ATTR_ASRL_DATA_BITS, iDB_DataBits));
	
		iDB_Parity = 0;
	
		STDF_CONFIG_VALUE("Parity", VAL_INTEGER , 1 , iDB_Parity , iDB_Parity );
		
		CHK_VSA( viSetAttribute (hCommHandle, VI_ATTR_ASRL_PARITY, iDB_Parity));
	
		iDB_StopBits = 10;
	
		STDF_CONFIG_VALUE("StopBits", VAL_INTEGER , 1 , iDB_StopBits , iDB_StopBits );
		
		CHK_VSA( viSetAttribute (hCommHandle, VI_ATTR_ASRL_STOP_BITS, iDB_StopBits));
	
		iDB_FlowCtrl = 0;
	
		STDF_CONFIG_VALUE("FlowCtrl", VAL_INTEGER , 1 , iDB_FlowCtrl , iDB_FlowCtrl );
		
		CHK_VSA( viSetAttribute (hCommHandle, VI_ATTR_ASRL_FLOW_CNTRL, iDB_FlowCtrl));
	
		CHK_VSA( viSetAttribute ( hCommHandle , VI_ATTR_ASRL_DISCARD_NULL , VI_FALSE ));
	}
	
	if ( pLocalStorage->iConnectionType == 1 )
	{
		iDB_TCP_IP_KeepAlive = 1;
	
		STDF_CONFIG_VALUE("TCP_IP_KeepAlive", VAL_INTEGER , 1 , iDB_TCP_IP_KeepAlive , iDB_TCP_IP_KeepAlive );	
	
		CHK_VSA( viSetAttribute ( hCommHandle , VI_ATTR_TCPIP_KEEPALIVE , iDB_TCP_IP_KeepAlive ));
		
		iDB_TCP_IP_NoDelay = 1;
		
		STDF_CONFIG_VALUE("TCP_IP_NoDelay", VAL_INTEGER , 1 , iDB_TCP_IP_NoDelay , iDB_TCP_IP_NoDelay );	
	
		CHK_VSA( viSetAttribute ( hCommHandle , VI_ATTR_TCPIP_NODELAY , iDB_TCP_IP_NoDelay ));
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_ERROR_VALUE;
}


void*	DLLEXPORT		MODBUS_Driver_Init( char *pszConnectionName , char *pszAddress , int *pHandle , ... )
{
	STD_ERROR						StdError									=	{0};
	
	tsSTD_CallBackSet				tSTD_CallBackSet							=	{0};

	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								handle										=	0;
	
	int								hCommunicationHandle						=	0;
	
	double							lfReceiveTimeOut							=	5.0,
									lfSendTimeOut								=	5.0; 
										
	int								iNumberOfReTry								=	3;
											 
	int								*pShareHandle								=	NULL;
	
	unsigned int					index										=	0,
									listSize									=	0;
	
	char							szFormatedLog[STD_STRING]					=	{0};
	
	int								iReadBlockSize								=	32,  
									bDontWaitForSendStatus						=	0;
	
	double							lfLowLevelReceiveTimeout					=	0.2,
									lfLowLevelSendTimeout						=	0.5;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );
	
	if ( *pHandle == 0 )
	{
		CHK_CMT(CmtNewTSV ( sizeof(tsLocalStorage) , pHandle ));
	}
	
	handle = *pHandle;
	
	pShareHandle = calloc(1,sizeof(int));
	
	IF (( pShareHandle == NULL ) , "Can't allocate memory." );
	
	*pShareHandle = handle;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	if ( pLocalStorage->ptCallbacks == NULL )
	{
		CALLOC_ERR( pLocalStorage->ptCallbacks , 1 , sizeof(tsSTD_CallBackSet));  
	}
		
	pLocalStorage->ptCallbacks->iInternalHandle = handle;
	
	strcpy( pLocalStorage->ptCallbacks->szName , pszConnectionName ); 
	
	if ( strstr( pszAddress , "TCP" ) )
	{
		pLocalStorage->iConnectionType = 1;	
	}
	
	if ( strstr( pszAddress , "ASRL" ) )
	{
		pLocalStorage->iConnectionType = 0;	
	}
	
	//listSize = GetCommandItemListSize(); 
		
	//CALLOC_ERR( pLocalStorage->pMODBUS_CommandList , listSize , sizeof(tsMODBUS_CommandItem));
	
	//IF (( pLocalStorage->pMODBUS_CommandList == NULL ) , "Can't allocate memory." ); 
	
	//for ( index = 0 ; index < ; index++ , listSize++ )
	//	memcpy( &(pLocalStorage->pMODBUS_CommandList[index]) , &(tSTD_CommandList[index]) , sizeof(tsMODBUS_CommandItem) ); 	
	
	//pLocalStorage->listSize = listSize;
	
	//pLocalStorage->ptCallbacks->iMultiUsingIndex = tConfig.iSlotNumber;   
	
	STDF_SET_MODULE_NAME(pLocalStorage->ptCallbacks,pszConnectionName);
	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks);
	
	if ( pszConnectionName )
	{
		sprintf( szFormatedLog , "Initializing \"%s\" Driver %%s" ,  pszConnectionName );
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	}
	
	//if ( strlen(tConfig.szConnectionAddress) == 0 )
	//{
	//	STDF_CONFIG_VALUE("CommunicationAddress", VALUE_TYPE_STRING , CONFIG_COM_STRUCT_LEN , tConfig.szConnectionAddress , tConfig.szConnectionAddress );  
	//}
													 

	STDF_CONFIG_VALUE("ReceiveTimeOut", VAL_DOUBLE , 1 , lfReceiveTimeOut , lfReceiveTimeOut );
	STDF_CONFIG_VALUE("NumberOfReTry", VAL_INTEGER , 1 , iNumberOfReTry , iNumberOfReTry );
	
	pLocalStorage->iNumberOfCommunicationTry = iNumberOfReTry;
	pLocalStorage->lfReceiveTimeOut = lfReceiveTimeOut;
	pLocalStorage->lfSendTimeOut = lfSendTimeOut;
	
	//if ( tConfig.lfTimeout > 0 )
	//	pLocalStorage->lfReceiveTimeOut = tConfig.lfTimeout;
	
	//if ( tConfig.iNumberOfRetries > 0 )
	//	pLocalStorage->iNumberOfCommunicationTry = tConfig.iNumberOfRetries;
		
	//strcpy( (char*)tConfigCommunication.szAddress , tConfig.szConnectionAddress );
	//tConfigCommunication.iType = tConfig.iConfigType;
	
	//memcpy( szConfigBuffer , &tConfigCommunication , sizeof(tsConfigCommunication));
	
	FREE_STDERR_COPY_ERR( Communication_Init( pszConnectionName , pszAddress , &hCommunicationHandle ));
	
	pLocalStorage->hCommunicationHandle = hCommunicationHandle;
		
	FREE_STDERR_COPY_ERR( Communication_InstallAttributeSetCallBack( hCommunicationHandle , (pfAttributeCallBack) MODBUS_Driver_Set_Attributes_CallBack , (void *)pShareHandle ));
	
	FREE_STDERR_COPY_ERR( Communication_InstallReceiveCallBack( hCommunicationHandle , (pfReceiveCallBack)MODBUS_Driver_Receive_CallBack , (void *)pShareHandle ));
	
	FREE_STDERR_COPY_ERR( Communication_InstallCloseCallBack( hCommunicationHandle , (pfCloseCallBack)MODBUS_Driver_Close_CallBack , (void *)pShareHandle ));
	FREE_STDERR_COPY_ERR( Communication_InstallErrorCallBack( hCommunicationHandle , (pfErrorCallBack)MODBUS_Driver_Error_CallBack , (void *)pShareHandle ));

	STDF_CONFIG_VALUE("LowLevelSendTimeout", VAL_DOUBLE , 1 , lfLowLevelSendTimeout , lfLowLevelSendTimeout );  
	STDF_CONFIG_VALUE("LowLevelReceiveTimeout", VAL_DOUBLE , 1 , lfLowLevelReceiveTimeout , lfLowLevelReceiveTimeout );
	STDF_CONFIG_VALUE("ReadBlockSize", VAL_INTEGER , 1 , iReadBlockSize , iReadBlockSize );
	STDF_CONFIG_VALUE("DontWaitForSendStatus", VAL_INTEGER , 1 , bDontWaitForSendStatus , bDontWaitForSendStatus );
	
	FREE_STDERR_COPY_ERR( Communication_SetLowLevelConfig( hCommunicationHandle , iReadBlockSize , lfLowLevelReceiveTimeout , lfLowLevelSendTimeout , 0 , bDontWaitForSendStatus ));
	
	FREE_STDERR_COPY_ERR( Communication_SetPluggedInInitFailDelay( hCommunicationHandle , 1.0 )); //tConfig.lfInitFailWaitForDelay ));
		
	if ( pszConnectionName )
	{
		sprintf( szFormatedLog , "COM_%s.log" , pszConnectionName );
		FREE_STDERR_COPY_ERR( Communication_EnableLoging( hCommunicationHandle , 1 , szFormatedLog , 0 , 1 ));
	}
	
	FREE_STDERR_COPY_ERR( Communication_EnableReceive( pLocalStorage->hCommunicationHandle , 1 )); 	
		
	FREE_STDERR_COPY_ERR( Communication_FlushReceiveBuffer( pLocalStorage->hCommunicationHandle , 1 )); 
	
	pLocalStorage->bEnableAutoPlugIn = 1; 
	
	ProcessSystemEvents();
	
	if ( pszConnectionName )
	{
		sprintf( szFormatedLog , "Initializing \"%s\" Success %%s" ,  pszConnectionName );
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	}
	
Error:
	
	if (( IS_NOT_OK) && pszConnectionName )
	{
		sprintf( szFormatedLog , "Initializing \"%s\" Fail %%s" ,  pszConnectionName );
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	}
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER;
}


void*	DLLEXPORT		MODBUS_Driver_Close(int *pHandle)
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								handle										=	0;
	
	double							lfCurrentTime								=	0.0,
									lfStartTime									=	0.0;
	
	int								bClosed										=	0;
	
	char							szFormatedLog[LOW_STRING]					=	{0};

	tsSTD_CallBackSet				tSTD_CallBackSet							=	{0};
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );
	
	handle = *pHandle;  
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStorage ));
	
	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks); 
	
	sprintf( szFormatedLog , "%s :: Close communication %%s" ,  pLocalStorage->ptCallbacks->szName );
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	
	FREE_STDERR_COPY_ERR( Communication_Close( pLocalStorage->hCommunicationHandle ));

	CHK_CMT( CmtReleaseTSVPtr ( handle )); 
	
	GetCurrentDateTime(&lfStartTime);
	
	do
	{
		GetCurrentDateTime(&lfCurrentTime);
		
		if (( lfCurrentTime - lfStartTime ) > 10.0 )
			break;
		
		if ( CmtGetTSVPtr ( handle , &pLocalStorage ) == 0 )
		{
			bClosed = pLocalStorage->bClosed;
		
			CmtReleaseTSVPtr ( handle );
		}
		
		if ( bClosed )
			break;
		
		DelayWithEventProcessing(1.0);
		
	} while(!bClosed);
	
Error:
	
	if (( handle ) && ( CmtGetTSVPtr ( handle , &pLocalStorage ) == 0 ))  
	{
		*pHandle = 0;
		
		FREE(pLocalStorage->pMODBUS_CommandList );
		
		FREE(pLocalStorage->pSourceBuffer);
			
		FREE(pLocalStorage->pData);
		
		FREE(pLocalStorage->ptCallbacks);
		
		CmtReleaseTSVPtr ( handle );
	
		CmtDiscardTSV ( handle );

	}
	
	RETURN_STDERR_POINTER;
}


void*   DLLEXPORT       MODBUS_Driver_GetConnectionAddress( int hHandle , char	**szAddress )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	IF (( hHandle == 0 ) , "Pointer to handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	FREE_STDERR_COPY_ERR( Communication_GetConnectionAddress( pLocalStorage->hCommunicationHandle , szAddress ));
	
Error:
	
	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	RETURN_STDERR_POINTER;
}



void*   DLLEXPORT       MODBUS_Driver_Send_Patern( int hCommunicationHandle , tsMODBUS_CommandItem tCommandItem , unsigned short *pData , int iDataLength , unsigned int iSendPacketCounter )
{
	STD_ERROR						StdError											=	{0};
	
	unsigned char					*pFillBuffer										=	NULL,
									*pSendBuffer										=	NULL;
	
	unsigned int					iSendLenth											=	0,
									prefixLength										=	0,
									crcLength											=	0, 
									index												=	0,
									dataLength											=	0;
	
	unsigned short					*pWordItem											=	NULL;
	unsigned char					*pByteItem											=	NULL;
	
	unsigned short					calculatedCRC										=	0;
		
	tsMODBUS_ProtocolItem			tCurrentPackage										=	{0};

	tuSweepConvertWord				tConvert,
									tConvertedData, 
									tCrcBigEndian;
	
	tuConvert						tConvertBigSize;
	
	char							pLocalData[128]										=	{0};
	
	__int64							llLocalValue										=	0;
	
	int								iCalcReference										=	0,
									iCalcDifferent										=	0;  
	if ( pData == NULL )
	{
		if (( strchr( tCommandItem.szSendData , 'x' )) || ( strchr( tCommandItem.szSendData , 'X' )))
			sscanf( tCommandItem.szSendData , "%llx" , &llLocalValue );
		else
			llLocalValue = atol(tCommandItem.szSendData);
		
		memcpy( pLocalData , &llLocalValue , sizeof(llLocalValue));
		
		pData = (unsigned short *) pLocalData;
		
	}
	
	FREE_CALLOC_ERR( pSendBuffer , 512 , sizeof(char));
	iCalcReference = &tCurrentPackage; 
	
	crcLength = 2;
	
	switch ( tCommandItem.bSendPrefix )
	{
		case 1:
			pFillBuffer = pSendBuffer + sizeof(int) + sizeof(short);
			
			tConvertBigSize.uWord = iSendPacketCounter;
			
			pSendBuffer[0] = tConvertBigSize.buffer[1];
			pSendBuffer[1] = tConvertBigSize.buffer[0];
			
			tConvertBigSize.uWord = tCommandItem.iSendPrefix_ID; 
			
			pSendBuffer[2] = tConvertBigSize.buffer[1];
			pSendBuffer[3] = tConvertBigSize.buffer[0];

			prefixLength = sizeof(int) + sizeof(short);
			
			break;
			
		default:	
			pFillBuffer = pSendBuffer;
			break;
	}
	
	switch( tCommandItem.iSendType ) 
	{
		case PROTOCOL_TYPE_A:
			
			dataLength = tCommandItem.send_side.tTypeA.data_length;
			
			iSendLenth = dataLength + sizeof(tCommandItem.send_side.tTypeA) - 2;
			
			memcpy( tCurrentPackage.max_size , tCommandItem.send_side.max_size , sizeof(tCommandItem.send_side.tTypeA));    
			memcpy( pFillBuffer , tCurrentPackage.max_size , iSendLenth );  
			
			iCalcDifferent = (char*)&(tCurrentPackage.tTypeA.single_data_hi);
			pWordItem = pFillBuffer + (iCalcDifferent - iCalcReference);
			
			for ( index = 0 ; index < ( dataLength / 2 ) ; index++ ) 
			{
				tConvert.Word = pData[index];
				
				if ( tCommandItem.bSendDataByteBigEndian )
				{
					tConvertedData.byte_lo = tConvert.byte_hi;
					tConvertedData.byte_hi = tConvert.byte_lo;
				}
				else
				{
					tConvertedData.byte_lo = tConvert.byte_lo;
					tConvertedData.byte_hi = tConvert.byte_hi;
				}
				
				*pWordItem = tConvertedData.Word;
				
				pWordItem++; 
			}
			
			break;
			
		case PROTOCOL_TYPE_B:
			
			dataLength = 1;
			
			iSendLenth = sizeof(tCommandItem.send_side.tTypeB);
			
			memcpy( tCurrentPackage.max_size , tCommandItem.send_side.max_size , sizeof(tCommandItem.send_side.tTypeB));    
			memcpy( pFillBuffer , tCurrentPackage.max_size , iSendLenth );  
			
			iCalcDifferent = (char*)&(tCurrentPackage.tTypeB.num_of_hi);
			pWordItem = pFillBuffer + (iCalcDifferent - iCalcReference);          
			
			for ( index = 0 ; index < dataLength ; index++ ) 
			{
				tConvert.Word = pData[index];
				
				tConvertedData.byte_lo = tConvert.byte_hi;
				tConvertedData.byte_hi = tConvert.byte_lo;
			
				*pWordItem = tConvertedData.Word;
				
				pWordItem++; 
			}
			
			break;
			
		case PROTOCOL_TYPE_C:
			
			dataLength = 1;
			
			iSendLenth = sizeof(tCommandItem.send_side.tTypeC);
			
			memcpy( tCurrentPackage.max_size , tCommandItem.send_side.max_size , sizeof(tCommandItem.send_side.tTypeC));    
			memcpy( pFillBuffer , tCurrentPackage.max_size , iSendLenth );  
			
			iCalcDifferent = (char*)&(tCurrentPackage.tTypeC.single_data_hi);
			pWordItem = pFillBuffer + (iCalcDifferent - iCalcReference);          
			
			for ( index = 0 ; index < dataLength ; index++ ) 
			{
				tConvert.Word = pData[index];
				
				if ( tCommandItem.bSendDataByteBigEndian )
				{
					tConvertedData.byte_lo = tConvert.byte_hi;
					tConvertedData.byte_hi = tConvert.byte_lo;
				}
				else
				{
					tConvertedData.byte_lo = tConvert.byte_lo;
					tConvertedData.byte_hi = tConvert.byte_hi;
				}
				
				*pWordItem = tConvertedData.Word;
				
				pWordItem++; 
			}
			
			break;
			
		case PROTOCOL_TYPE_D:
		
			dataLength = tCommandItem.send_side.tTypeD.data_length;
			
			iSendLenth = dataLength + sizeof(tCommandItem.send_side.tTypeD) - 1;
			
			memcpy( tCurrentPackage.max_size , tCommandItem.send_side.max_size , sizeof(tCommandItem.send_side.tTypeD));    
			memcpy( pFillBuffer , tCurrentPackage.max_size , iSendLenth );  
			
			iCalcDifferent = (char*)&(tCurrentPackage.tTypeD.single_data_lo);
			pByteItem = pFillBuffer + (iCalcDifferent - iCalcReference);          
			
			for ( index = 0 ; index < ( dataLength / 2 ) ; index++ ) 
			{
				*pByteItem  = pData[index];
				
				pByteItem++; 
			}
			
			pWordItem = (short*)pByteItem;
			
			break;
			
		case PROTOCOL_TYPE_E:
		
			dataLength = tCommandItem.send_side.tTypeE.data_length;
			
			iSendLenth = dataLength + sizeof(tCommandItem.send_side.tTypeE) - 2;
			
			memcpy( tCurrentPackage.max_size , tCommandItem.send_side.max_size , sizeof(tCommandItem.send_side.tTypeE));    
			memcpy( pFillBuffer , tCurrentPackage.max_size , iSendLenth );  
			
			iCalcDifferent = (char*)&(tCurrentPackage.tTypeE.single_data_hi);
			pWordItem = pFillBuffer + (iCalcDifferent - iCalcReference);          
			
			for ( index = 0 ; index < ( dataLength / 2 ) ; index++ ) 
			{
				tConvert.Word = pData[index];
				
				if ( tCommandItem.bSendDataByteBigEndian )
				{
					tConvertedData.byte_lo = tConvert.byte_hi;
					tConvertedData.byte_hi = tConvert.byte_lo;
				}
				else
				{
					tConvertedData.byte_lo = tConvert.byte_lo;
					tConvertedData.byte_hi = tConvert.byte_hi;
				}
				
				*pWordItem = tConvertedData.Word;
				
				pWordItem++; 
			}
			
			break;
			
		case PROTOCOL_TYPE_F:
			
			dataLength = 1;
			
			iSendLenth = sizeof(tCommandItem.send_side.tTypeF);
			
			memcpy( tCurrentPackage.max_size , tCommandItem.send_side.max_size , sizeof(tCommandItem.send_side.tTypeF));    
			
			tCurrentPackage.tTypeF.single_data_lo = pData[index];
			
			memcpy( pFillBuffer , tCurrentPackage.max_size , iSendLenth );  
			
			iCalcDifferent = (char*)&(tCurrentPackage.tTypeF.crc16_hi);
			pWordItem = pFillBuffer + ( iCalcDifferent - iCalcReference);           
			
			break;
			
		case PROTOCOL_TYPE_G:
	
			dataLength = 1;
			
			iSendLenth = sizeof(tCommandItem.send_side.tTypeG);
			
			memcpy( tCurrentPackage.max_size , tCommandItem.send_side.max_size , sizeof(tCommandItem.send_side.tTypeG));    
			
			tCurrentPackage.tTypeG.func_code = pData[index];
			
			memcpy( pFillBuffer , tCurrentPackage.max_size , iSendLenth );   
			
			iCalcDifferent = (char*)&(tCurrentPackage.tTypeG.crc16_hi);
			pWordItem = pFillBuffer + ( iCalcDifferent - iCalcReference);            
			
			break;
			
		default:
			
			FORCE_ERR(-1,"Command has not defined yet"); 
			break;
	}
		
	do
	{	 
		if ( tCommandItem.bSendCrcBigEndian == 2 )  // without cheksum
		{
			iSendLenth -= crcLength;
			
			break;
		}

		if ( tCommandItem.bSendCrcBigEndian == 3 )  // zero cheksum
			calculatedCRC = 0;	
		else
			calculatedCRC = CalculateModBusCRC_16( pFillBuffer , ( iSendLenth - 2 )); 
			
		if ( tCommandItem.bSendCrcBigEndian == 0 )
		{
			tConvert.Word = calculatedCRC;  
		
			tCrcBigEndian.byte_lo = tConvert.byte_hi;
			tCrcBigEndian.byte_hi = tConvert.byte_lo;
		
			calculatedCRC = tCrcBigEndian.Word;
		}
		
	}while(0);
	
	switch ( tCommandItem.bSendPrefix )
	{
		case 1:
			
			tConvertBigSize.uWord = iSendLenth; 
			
			pSendBuffer[4] = tConvertBigSize.buffer[1];
			pSendBuffer[5] = tConvertBigSize.buffer[0];
			
			break;
			
		default:	
			break;
	}		
			
	iSendLenth += prefixLength;
	
	*pWordItem = calculatedCRC; 	
			
	FREE_STDERR_COPY_ERR( Communication_Send( hCommunicationHandle , pSendBuffer , iSendLenth , 0 ));         
	
Error:
	
	FREE(pSendBuffer);
	
	RETURN_STDERR_POINTER;
}
	   
///--------------------- API ------------------------------//

void*   DLLEXPORT   MODBUS_Driver_SetValue( int hHandle , char *pCommandName , void *pValue , int iValueLength , int iValueSize , ...  )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL,			
								tLocalStorage										=	{0};
	
	int							bCommandFound										=	0,
								iCommandIndex										=	0,
								iIndex												=	0;
								
	int							iRealSize											=	0,
								iCopyDataSize										=	0,
								iDataLen											=	0;
	
	unsigned char 				*pData												=	NULL;
	char						*pVoidData											=	NULL;
	
	char						szCommand[1024]										=	{0},
								szFormatedLog[LOW_STRING]							=	{0};
	
	double						lfStartTime											=	0.0,
								lfCurrentTime										=	0.0;
	
	int							iTryCounter											=	0;
	
	char						szLog[1024]											=	{0};
	
	int							bPluggedInStatus									=	0,  
								bSentSuccessfully									=	0; 
									
	double						lfWaitForConnectionTimeout							=	0.0;
	
	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};

	tuConvert					tConvert,
								tSwapConvert;
				
	IF (( hHandle == 0 ) , "handle can't be zero." );
	IF ((( iValueLength > 0 ) && ( pValue == NULL )) , "Value can't be NULL." ); 
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	pLocalStorage->iSendPacketCounter++;
	memcpy( &tLocalStorage , pLocalStorage , sizeof(tsLocalStorage));
	CHK_CMT(CmtReleaseTSVPtr ( hHandle ));   
	
	STDF_UPDATE_CALLBACK_DATA(tLocalStorage.ptCallbacks);
	
	GetCurrentDateTime(&lfStartTime); 
	
	lfWaitForConnectionTimeout = (double)tLocalStorage.iNumberOfCommunicationTry * tLocalStorage.lfReceiveTimeOut;
		
	do
	{
		FREE_STDERR_COPY( Communication_GetPluggedInStatus( tLocalStorage.hCommunicationHandle , &bPluggedInStatus ));  

		if ( bPluggedInStatus == 0 )
			break;
		
		CHK_CMT( CmtReleaseTSVPtr ( hHandle ));
			
		ProcessSystemEvents(); 
		
		DelayWithEventProcessing(0.2);
	
		CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
			
		GetCurrentDateTime(&lfCurrentTime);
			
	} while((lfCurrentTime-lfStartTime) < lfWaitForConnectionTimeout );

	IF (( bPluggedInStatus ) , "Communication is not established");
	
	//STDF_UPDATE_CALLBACK_DATA(tLocalStorage.tCallbacks); 
	
	for ( iCommandIndex = 0 ; iCommandIndex < tLocalStorage.listSize ; iCommandIndex++ )
		if ((strlen(tLocalStorage.pMODBUS_CommandList[iCommandIndex].szCommandName)) && ( strcmp( pCommandName , tLocalStorage.pMODBUS_CommandList[iCommandIndex].szCommandName ) == 0 ))
		{
			bCommandFound = 1;
			break;
		}
		
	IF (( bCommandFound == 0 ) , "Command was not found.");   
		
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage )); 
	pLocalStorage->iLastSentCommandIndex = iCommandIndex;
	CHK_CMT(CmtReleaseTSVPtr ( hHandle ));  
	
	iDataLen = tLocalStorage.pMODBUS_CommandList[iCommandIndex].iSendDataSize;
	
	if ( iDataLen && pValue )
	{
		
		CALLOC_ERR( pData , iDataLen+10 , sizeof(char));
	
		if (( pValue ) && ((iValueLength * iValueSize) >= iDataLen ))
			memcpy( pData , pValue	, iDataLen );  
		else
			memcpy( pData , pValue	, (iValueLength * iValueSize) );
		
		iRealSize = iValueLength * iValueSize; 
		
		switch( tLocalStorage.pMODBUS_CommandList[iCommandIndex].tSendValueType )
		{
			case VALUE_TYPE_NONE:
			
				iCopyDataSize = 0; 
				
				FREE_CALLOC_COPY_ERR( ((char*)pVoidData) , (iRealSize + 1) , sizeof(char) , pData , (iRealSize * sizeof(char)));
				
				for ( iIndex = 0; iIndex < iRealSize; iIndex++ )
				{
					iCopyDataSize += 1;
						
					if ( tLocalStorage.pMODBUS_CommandList[iCommandIndex].lfSendValueMultiply != 0.0 )
					{
						*((char*)pVoidData) *=	tLocalStorage.pMODBUS_CommandList[iCommandIndex].lfSendValueMultiply;
					}
				}
				
				break;
			case VALUE_TYPE_CHAR:
				
				iCopyDataSize = 0; 
				
				FREE_CALLOC_COPY_ERR( ((char*)pVoidData) , (iRealSize + 1) , sizeof(char) , pData , (iRealSize * sizeof(char)));
				
				for ( iIndex = 0; iIndex < iRealSize; iIndex++ )
				{
					iCopyDataSize += 1;
					
					if ( tLocalStorage.pMODBUS_CommandList[iCommandIndex].lfSendValueMultiply != 0.0 )
					{
						*((char*)pVoidData) *=	tLocalStorage.pMODBUS_CommandList[iCommandIndex].lfSendValueMultiply;
					}
				}
				break;
				
			 	
			case VALUE_TYPE_SHORT:
				
				iRealSize /= sizeof(short);
				iCopyDataSize = 0;
				
				FREE_CALLOC_ERR( ((short*)pVoidData) , (iValueLength + 1) , sizeof(short));
				
				for ( iIndex = 0; iIndex < iValueLength; iIndex++ )
				{
					iCopyDataSize +=  sizeof(short);   
					
					switch(iValueSize)
					{
						case 1:
								((short*)pVoidData)[iIndex] = (short)((char*)pValue)[iIndex];
								break;
						
						case 2:
								((short*)pVoidData)[iIndex] = (short)((short*)pValue)[iIndex];
								break;
							
						case 4:
								((short*)pVoidData)[iIndex] = (short)((int*)pValue)[iIndex];
								break;
								
						case 8:
								((short*)pVoidData)[iIndex] = (short)((long long*)pValue)[iIndex];
								break;
								
						default:

								FREE_CALLOC_COPY_ERR( ((char*)pVoidData) , (iRealSize + 1) , sizeof(char) , pData , (iRealSize * sizeof(char)));
								break;
					}
					
					if ( tLocalStorage.pMODBUS_CommandList[iCommandIndex].lfSendValueMultiply != 0.0 )
					{
						*((short*)pVoidData) *=	tLocalStorage.pMODBUS_CommandList[iCommandIndex].lfSendValueMultiply;
					}
					
					if ( tLocalStorage.pMODBUS_CommandList[iCommandIndex].bSendSwap )
					{
						tConvert.uqWord = 0;
						tSwapConvert.uqWord = 0;  
						memcpy( tConvert.buffer , pVoidData , sizeof(short) );
						
						tSwapConvert.buffer[0] = tConvert.buffer[1];
						tSwapConvert.buffer[1] = tConvert.buffer[0];
						
						((short*)pVoidData)[iIndex] = tSwapConvert.uWord;
					}
				}
				break;
			case VALUE_TYPE_INT:
				
				iRealSize /= sizeof(int);
				iCopyDataSize = 0;
				
				FREE_CALLOC_ERR( ((int*)pVoidData) , (iValueLength + 1) , sizeof(int));
				
				for ( iIndex = 0; iIndex < iValueLength; iIndex++ )
				{
					iCopyDataSize +=  sizeof(int);
					
					switch(iValueSize)
					{
						case 1:
								((int*)pVoidData)[iIndex] = (int)((char*)pValue)[iIndex];
								break;
						
						case 2:
								((int*)pVoidData)[iIndex] = (int)((short*)pValue)[iIndex];
								break;
							
						case 4:
								((int*)pVoidData)[iIndex] = (int)((int*)pValue)[iIndex];
								break;
								
						case 8:
								((int*)pVoidData)[iIndex] = (int)((long long*)pValue)[iIndex];
								break;
								
						default:

								FREE_CALLOC_COPY_ERR( ((char*)pVoidData) , (iRealSize + 1) , sizeof(char) , pData , (iRealSize * sizeof(char)));
								break;
					}
					
					if ( tLocalStorage.pMODBUS_CommandList[iCommandIndex].lfSendValueMultiply != 0.0 )
					{
						*((int*)pVoidData) *=	tLocalStorage.pMODBUS_CommandList[iCommandIndex].lfSendValueMultiply;
					}
					
					if ( tLocalStorage.pMODBUS_CommandList[iCommandIndex].bSendSwap )
					{
						tConvert.uqWord = 0;
						tSwapConvert.uqWord = 0;  
						memcpy( tConvert.buffer , pVoidData , sizeof(int) );
						
						tSwapConvert.buffer[0] = tConvert.buffer[3];
						tSwapConvert.buffer[1] = tConvert.buffer[2];
						tSwapConvert.buffer[2] = tConvert.buffer[1];
						tSwapConvert.buffer[3] = tConvert.buffer[0];
						
						((int*)pVoidData)[iIndex] = tSwapConvert.udWord;
					}
				}
				break;
			case VALUE_TYPE_LONG:
				
				iRealSize /= sizeof(__int64);
				iCopyDataSize = 0;
				
				FREE_CALLOC_ERR( ((__int64*)pVoidData) , (iValueLength + 1) , sizeof(__int64));
				
				for ( iIndex = 0; iIndex < iValueLength; iIndex++ )
				{
					iCopyDataSize += sizeof(__int64); 
					
					switch(iValueSize)
					{
						case 1:
								((long long*)pVoidData)[iIndex] = (long long)((char*)pValue)[iIndex];
								break;
						
						case 2:
								((long long*)pVoidData)[iIndex] = (long long)((short*)pValue)[iIndex];
								break;
							
						case 4:
								((long long*)pVoidData)[iIndex] = (long long)((int*)pValue)[iIndex];
								break;
								
						case 8:
								((long long*)pVoidData)[iIndex] = (long long)((long long*)pValue)[iIndex];
								break;
								
						default:

								FREE_CALLOC_COPY_ERR( ((char*)pVoidData) , (iRealSize + 1) , sizeof(char) , pData , (iRealSize * sizeof(char)));
								break;
					}
					
					if ( tLocalStorage.pMODBUS_CommandList[iCommandIndex].lfSendValueMultiply != 0.0 )
					{
						*((long long*)pVoidData) *=	(long long)tLocalStorage.pMODBUS_CommandList[iCommandIndex].lfSendValueMultiply;
					}
					
					if ( tLocalStorage.pMODBUS_CommandList[iCommandIndex].bSendSwap )
					{
						tConvert.uqWord = 0;
						tSwapConvert.uqWord = 0;  
						memcpy( tConvert.buffer , pVoidData , sizeof(__int64) );
						
						tSwapConvert.buffer[0] = tConvert.buffer[7];
						tSwapConvert.buffer[1] = tConvert.buffer[6];
						tSwapConvert.buffer[2] = tConvert.buffer[5];
						tSwapConvert.buffer[3] = tConvert.buffer[4];
						tSwapConvert.buffer[4] = tConvert.buffer[3];
						tSwapConvert.buffer[5] = tConvert.buffer[2];
						tSwapConvert.buffer[6] = tConvert.buffer[1];
						tSwapConvert.buffer[7] = tConvert.buffer[0];
						
						((long long*)pVoidData)[iIndex] = tSwapConvert.uqWord;
					}
				}
				
				break;
			case VALUE_TYPE_FLOAT:
				
				iRealSize /=  sizeof(float);  
				iCopyDataSize = 0;
				
				FREE_CALLOC_ERR( ((float*)pVoidData) , (iValueLength + 1) , sizeof(float));
				
				for ( iIndex = 0; iIndex < iValueLength; iIndex++ )
				{
					iCopyDataSize += sizeof(float);
					
					switch(iValueSize)
					{
						case 1:
								((float*)pVoidData)[iIndex] = (float)((char*)pValue)[iIndex];
								break;
						
						case 2:
								((float*)pVoidData)[iIndex] = (float)((short*)pValue)[iIndex];
								break;
							
						case 4:
								((float*)pVoidData)[iIndex] = (float)((int*)pValue)[iIndex];
								break;
							
						case 8:
								((float*)pVoidData)[iIndex] = (float)((long long*)pValue)[iIndex];
								break;
								
						default:

								FREE_CALLOC_COPY_ERR( ((char*)pVoidData) , (iRealSize + 1) , sizeof(char) , pData , (iRealSize * sizeof(char)));
								break;
					}
					
					if ( tLocalStorage.pMODBUS_CommandList[iCommandIndex].lfSendValueMultiply != 0.0 )
					{
						*((float*)pVoidData) *=	tLocalStorage.pMODBUS_CommandList[iCommandIndex].lfSendValueMultiply;
					}
					
					if ( tLocalStorage.pMODBUS_CommandList[iCommandIndex].bSendSwap )
					{
						tConvert.uqWord = 0;
						tSwapConvert.uqWord = 0;  
						memcpy( tConvert.buffer , pVoidData , sizeof(float) );
						
						tSwapConvert.buffer[0] = tConvert.buffer[3];
						tSwapConvert.buffer[1] = tConvert.buffer[2];
						tSwapConvert.buffer[2] = tConvert.buffer[1];
						tSwapConvert.buffer[3] = tConvert.buffer[0];
						
						((float*)pVoidData)[iIndex] = tSwapConvert.fWord;
					}
				}
				
				break;
			case VALUE_TYPE_DOUBLE:
				
				iRealSize /= sizeof(double);
				iCopyDataSize = 0;
				
				FREE_CALLOC_ERR( ((double*)pVoidData) , (iValueLength + 1) , sizeof(double));
				
				for ( iIndex = 0; iIndex < iValueLength; iIndex++ )
				{
					iCopyDataSize += sizeof(double);
					
					switch(iValueSize)
					{
						case 1:
								((double*)pVoidData)[iIndex] = (double)((char*)pValue)[iIndex];
								break;
						
						case 2:
								((double*)pVoidData)[iIndex] = (double)((short*)pValue)[iIndex];
								break;
							
						case 4:
								((double*)pVoidData)[iIndex] = (double)((int*)pValue)[iIndex];
								break;
							
						case 8:
								((double*)pVoidData)[iIndex] = (double)((long long*)pValue)[iIndex];
								break;
								
						default:

								FREE_CALLOC_COPY_ERR( ((char*)pVoidData) , (iRealSize + 1) , sizeof(char) , pData , (iRealSize * sizeof(char)));
								break;
					}
					
					if ( tLocalStorage.pMODBUS_CommandList[iCommandIndex].lfSendValueMultiply != 0.0 )
					{
						*((double*)pVoidData) *= tLocalStorage.pMODBUS_CommandList[iCommandIndex].lfSendValueMultiply;
					}
					
					if ( tLocalStorage.pMODBUS_CommandList[iCommandIndex].bSendSwap )
					{
						tConvert.uqWord = 0;
						tSwapConvert.uqWord = 0;  
						memcpy( tConvert.buffer , pVoidData , sizeof(double) );
						
						tSwapConvert.buffer[0] = tConvert.buffer[7];
						tSwapConvert.buffer[1] = tConvert.buffer[6];
						tSwapConvert.buffer[2] = tConvert.buffer[5];
						tSwapConvert.buffer[3] = tConvert.buffer[4];
						tSwapConvert.buffer[4] = tConvert.buffer[3];
						tSwapConvert.buffer[5] = tConvert.buffer[2];
						tSwapConvert.buffer[6] = tConvert.buffer[1];
						tSwapConvert.buffer[7] = tConvert.buffer[0];
						
						((double*)pVoidData)[iIndex] = tSwapConvert.lfWord;
					}
						
				}    
				break;
			case VALUE_TYPE_ARRAY:
				
				FREE_CALLOC_COPY_ERR( ((char*)pVoidData) , (iRealSize + 1) , sizeof(char) , pData , (iRealSize * sizeof(char)));
						 
				for ( iIndex = 0; iIndex < iRealSize; iIndex++ )
				{
					iCopyDataSize += 1;  
					
					if ( tLocalStorage.pMODBUS_CommandList[iCommandIndex].lfSendValueMultiply != 0.0 )
					{
						*((char*)pVoidData) *=	tLocalStorage.pMODBUS_CommandList[iCommandIndex].lfSendValueMultiply;
					}
				}  
				break;
			 
		}
		
		if ( pVoidData )
		{
			memcpy( pData , pVoidData , iCopyDataSize );
			FREE(pVoidData);
		}
		  
	}
		
	for ( iTryCounter = 0 ; iTryCounter < tLocalStorage.iNumberOfCommunicationTry ; iTryCounter++ )
	{
		FREE_STDERR_COPY_ERR( MODBUS_Driver_Send_Patern( tLocalStorage.hCommunicationHandle , tLocalStorage.pMODBUS_CommandList[iCommandIndex] , (short*)pData , iDataLen , tLocalStorage.iSendPacketCounter )); 
		
		GetCurrentDateTime(&lfStartTime);  
		
		if ( IS_NOT_OK )
		{
			OutputDebugString("\t\tUUT Driver Error : Can't send command.");   
			DelayWithEventProcessing(5.0);
			sprintf( szLog , "\t\tUUT Driver       : Try to send command again. (%d/%d)" , iTryCounter+1 , tLocalStorage.iNumberOfCommunicationTry );
			OutputDebugString(szLog);  
			continue;
		}
		
		if (  IS_NOT_OK )
			continue;
		
		do
		{
			ProcessSystemEvents();
			
			FREE_STDERR_COPY( Communication_GetSendStatus ( tLocalStorage.hCommunicationHandle , &bSentSuccessfully ));
			
			if ( bSentSuccessfully )
				break;
			
			CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage )); 
			
			if ( pLocalStorage->pLastError )
			{
				UPDATERR( *pLocalStorage->pLastError ); 
			}
			
			CHK_CMT(CmtReleaseTSVPtr ( hHandle ));            
			
			if ( IS_NOT_OK )
				break;
			
			GetCurrentDateTime(&lfCurrentTime);
			
		} while((lfCurrentTime-lfStartTime) < tLocalStorage.lfSendTimeOut );
		
		if ((IS_OK) && bSentSuccessfully ) 
			break;
	}
	
Error:

	FREE(pData); 
	
	if ((IS_OK) && bSentSuccessfully ) 
		sprintf( szFormatedLog , "%s :: Sent command [%s] %%s" ,  tLocalStorage.ptCallbacks->szName , pCommandName );
	else
		sprintf( szFormatedLog , "%s :: Error sending command [%s] %%s" ,  tLocalStorage.ptCallbacks->szName , pCommandName );
	
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");  
	
	RETURN_STDERR_POINTER;
}
 


void*   DLLEXPORT   MODBUS_Driver_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize , ...  )    
{
	STD_ERROR					StdError											=	{0};
   	
	tsLocalStorage				*pLocalStorage										=	NULL,			
								tLocalStorage										=	{0};			
																			
	int							bCommandFound										=	0,
								iCommandIndex										=	0,
								iIndex												=	0;
								
	int							iRealSize											=	0,
								iDataLen											=	0;
	
	unsigned char 				*pData												=	NULL;
	
	char						*pReadValue											=	NULL;
		
	char						szCommand[1024]										=	{0},
								szFormatedLog[LOW_STRING]							=	{0};
								
	double						lfStartTime											=	0.0,
								lfCurrentTime										=	0.0;
	
	int							iTryCounter											=	0;
	
	char						szLog[1024]											=	{0};
	
	int							bPluggedInStatus									=	0;  
	
	double						lfWaitForConnectionTimeout							=	0.0;
	
	tuConvert					tConvert,
								tSwapConvert;
		
	int							iLastSentCommandIndex								=	0;
	
	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};

	IF (( hHandle == 0 ) , "handle can't be zero." );
 	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	memcpy( &tLocalStorage , pLocalStorage , sizeof(tsLocalStorage));
	CHK_CMT(CmtReleaseTSVPtr ( hHandle ));   
	
	STDF_UPDATE_CALLBACK_DATA(tLocalStorage.ptCallbacks);
	
	iLastSentCommandIndex = tLocalStorage.iLastSentCommandIndex;
	
	if ( pGetCommandName )
	{
		FREE_CALLOC_COPY_STRING( *pGetCommandName , tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].szCommandName );
	}
		
	GetCurrentDateTime(&lfStartTime);

	do
	{
		ProcessSystemEvents();
		
		CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
		memcpy( &tLocalStorage , pLocalStorage , sizeof(tsLocalStorage));
		CHK_CMT(CmtReleaseTSVPtr ( hHandle ));   
	
		if ( tLocalStorage.bDataUpdated )
			break;
		
		if ( tLocalStorage.pLastError && tLocalStorage.pLastError->error ) 
			break;
		
		GetCurrentDateTime(&lfCurrentTime);
		
	} while((lfCurrentTime-lfStartTime) < tLocalStorage.lfReceiveTimeOut );
	
	if ((lfCurrentTime-lfStartTime) >= tLocalStorage.lfReceiveTimeOut )
		OutputDebugString("\t\tUUT Driver Error : Receive timeout."); 
		
	if ( tLocalStorage.bDataUpdated )
	{   
		if ( pGetValue )
		{									
			FREE_CALLOC_ERR( ((short**)*pGetValue) , (tLocalStorage.iDataLength + 1) , sizeof(short));
			
			switch( tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].tResponseValueType )
			{
				case VALUE_TYPE_NONE:
										
					iRealSize = tLocalStorage.iDataLength;
					
					FREE_CALLOC_COPY_ERR( ((char*)pReadValue) , (iRealSize + 1) , sizeof(char) , tLocalStorage.pData , (iRealSize * sizeof(char)));
					
					for ( iIndex = 0; iIndex < iRealSize; iIndex++ )
					{
						if ( tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].bResponseSwap )
						{
						
						}
						
						if ( tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].lfResponseValueMultiply != 0.0 )
						{
							((char*)pReadValue)[iIndex] *=	tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].lfResponseValueMultiply;
						}
						
						((short*)*pGetValue)[iIndex] = ((char*)pReadValue)[iIndex];
					}
					
					if ( piValueSize )
						*piValueSize = sizeof(char);
					
					break;
				case VALUE_TYPE_CHAR:
					
					iRealSize = tLocalStorage.iDataLength;
					
					FREE_CALLOC_COPY_ERR( ((char*)pReadValue) , (iRealSize + 1) , sizeof(char) , tLocalStorage.pData , (iRealSize * sizeof(char)));
					
					for ( iIndex = 0; iIndex < iRealSize; iIndex++ )
					{
						if ( tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].bResponseSwap )
						{
						
						}
						
						if ( tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].lfResponseValueMultiply != 0.0 )
						{
							((char*)pReadValue)[iIndex] *=	tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].lfResponseValueMultiply;
						}
						
						((short*)*pGetValue)[iIndex] = ((char*)pReadValue)[iIndex];
					}
					
					if ( piValueSize )
						*piValueSize = sizeof(char);
					
					break;
				case VALUE_TYPE_SHORT:
					
					iRealSize = (tLocalStorage.iDataLength / sizeof(short));
						
					FREE_CALLOC_COPY_ERR( ((short*)pReadValue) , (iRealSize + 1) , sizeof(short) , tLocalStorage.pData , (iRealSize * sizeof(short))); 
					
					for ( iIndex = 0; iIndex < iRealSize; iIndex++ )
					{
						if ( tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].bResponseSwap )
						{
							tConvert.uqWord = 0;
							tSwapConvert.uqWord = 0;  
							memcpy( tConvert.buffer , pReadValue , sizeof(short) );
							
							tSwapConvert.buffer[0] = tConvert.buffer[1];
							tSwapConvert.buffer[1] = tConvert.buffer[0];
							
							((short*)pReadValue)[iIndex] = tSwapConvert.uWord;
						}
						
						if ( tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].lfResponseValueMultiply != 0.0 )
						{
							((short*)pReadValue)[iIndex] *= tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].lfResponseValueMultiply;
						}
						
						((short*)*pGetValue)[iIndex] = ((short*)pReadValue)[iIndex];
					}
					
					if ( piValueSize )
						*piValueSize = sizeof(short);
					
					break;
				case VALUE_TYPE_INT:
					
					iRealSize = (tLocalStorage.iDataLength / sizeof(int)); 
					
					FREE_CALLOC_COPY_ERR( ((int*)pReadValue) , (iRealSize + 1) , sizeof(int) , tLocalStorage.pData , (iRealSize * sizeof(int)));
					
					for ( iIndex = 0; iIndex < iRealSize; iIndex++ )
					{
						if ( tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].bResponseSwap )
						{
							tConvert.uqWord = 0;
							tSwapConvert.uqWord = 0;  
							memcpy( tConvert.buffer , pReadValue , sizeof(int) );
							
							tSwapConvert.buffer[0] = tConvert.buffer[3];
							tSwapConvert.buffer[1] = tConvert.buffer[2];
							tSwapConvert.buffer[2] = tConvert.buffer[1];
							tSwapConvert.buffer[3] = tConvert.buffer[0];
							
							((int*)pReadValue)[iIndex] = tSwapConvert.udWord;
						}
						
						if ( tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].lfResponseValueMultiply != 0.0 )
						{
							((int*)pReadValue)[iIndex] *=	tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].lfResponseValueMultiply;
						}
						
						((short*)*pGetValue)[iIndex] = ((int*)pReadValue)[iIndex];
					}
					
					if ( piValueSize )
						*piValueSize = sizeof(int);
					
					break;
				case VALUE_TYPE_LONG: 
					
					iRealSize = (tLocalStorage.iDataLength / sizeof(__int64)); 
				
					FREE_CALLOC_COPY_ERR( ((__int64*)pReadValue) , (iRealSize + 1) , sizeof(__int64) , tLocalStorage.pData , (iRealSize * sizeof(__int64)));
					
					for ( iIndex = 0; iIndex < iRealSize; iIndex++ )
					{
						if ( tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].bResponseSwap )
						{
							tConvert.uqWord = 0;
							tSwapConvert.uqWord = 0;  
							memcpy( tConvert.buffer , pReadValue , sizeof(__int64) );
							
							tSwapConvert.buffer[0] = tConvert.buffer[7];
							tSwapConvert.buffer[1] = tConvert.buffer[6];
							tSwapConvert.buffer[2] = tConvert.buffer[5];
							tSwapConvert.buffer[3] = tConvert.buffer[4];
							tSwapConvert.buffer[4] = tConvert.buffer[3];
							tSwapConvert.buffer[5] = tConvert.buffer[2];
							tSwapConvert.buffer[6] = tConvert.buffer[1];
							tSwapConvert.buffer[7] = tConvert.buffer[0];
							
							((long long*)pReadValue)[iIndex] = tSwapConvert.uqWord;
						}
						
						if ( tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].lfResponseValueMultiply != 0.0 )
						{
							((long long*)pReadValue)[iIndex] *= (long long)tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].lfResponseValueMultiply;
						}
						
						((short*)*pGetValue)[iIndex] = (short)((long long*)pReadValue)[iIndex];
					}
					
					if ( piValueSize )
						*piValueSize = sizeof(__int64);
					
					break;
				case VALUE_TYPE_FLOAT:
					
					iRealSize = (tLocalStorage.iDataLength / sizeof(float)); 
					
					FREE_CALLOC_COPY_ERR( ((float*)pReadValue) , (iRealSize + 1) , sizeof(float) , tLocalStorage.pData , (iRealSize * sizeof(float)));
					
					for ( iIndex = 0; iIndex < iRealSize; iIndex++ )
					{
						if ( tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].bResponseSwap )
						{
							tConvert.uqWord = 0;
							tSwapConvert.uqWord = 0;  
							memcpy( tConvert.buffer , pReadValue , sizeof(float) );
							
							tSwapConvert.buffer[0] = tConvert.buffer[3];
							tSwapConvert.buffer[1] = tConvert.buffer[2];
							tSwapConvert.buffer[2] = tConvert.buffer[1];
							tSwapConvert.buffer[3] = tConvert.buffer[0];
							
							((float*)pReadValue)[iIndex] = tSwapConvert.fWord;
						}
						
						if ( tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].lfResponseValueMultiply != 0.0 )
						{
							((float*)pReadValue)[iIndex] *= tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].lfResponseValueMultiply;
						}
						
						((short*)*pGetValue)[iIndex] = ((float*)pReadValue)[iIndex];
						
					}
					
					if ( piValueSize )
						*piValueSize = sizeof(float);
					
					break;
				case VALUE_TYPE_DOUBLE:
				
					iRealSize = (tLocalStorage.iDataLength / sizeof(double)); 
						
					FREE_CALLOC_COPY_ERR( ((double*)pReadValue) , (iRealSize + 1) , sizeof(double) , tLocalStorage.pData , (iRealSize * sizeof(double)));
					
					for ( iIndex = 0; iIndex < iRealSize; iIndex++ )
					{
						if ( tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].bResponseSwap )
						{
							tConvert.uqWord = 0;
							tSwapConvert.uqWord = 0;  
							memcpy( tConvert.buffer , pReadValue , sizeof(double) );
							
							tSwapConvert.buffer[0] = tConvert.buffer[7];
							tSwapConvert.buffer[1] = tConvert.buffer[6];
							tSwapConvert.buffer[2] = tConvert.buffer[5];
							tSwapConvert.buffer[3] = tConvert.buffer[4];
							tSwapConvert.buffer[4] = tConvert.buffer[3];
							tSwapConvert.buffer[5] = tConvert.buffer[2];
							tSwapConvert.buffer[6] = tConvert.buffer[1];
							tSwapConvert.buffer[7] = tConvert.buffer[0];
							
							((double*)pReadValue)[iIndex] = tSwapConvert.lfWord;
						}
						
						if ( tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].lfResponseValueMultiply != 0.0 )
						{
							((double*)pReadValue)[iIndex] *= tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].lfResponseValueMultiply;
						}
						
						((short*)*pGetValue)[iIndex] = ((double*)pReadValue)[iIndex];
					}
					
					if ( piValueSize )
						*piValueSize = sizeof(double);
					
					break;
				case VALUE_TYPE_ARRAY:
					
					iRealSize = tLocalStorage.iDataLength;
					
					FREE_CALLOC_COPY_ERR( ((char*)pReadValue) , (iRealSize + 1) , sizeof(char) , tLocalStorage.pData , (iRealSize * sizeof(char)));
					
					for ( iIndex = 0; iIndex < iRealSize; iIndex++ )
					{
						if ( tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].bResponseSwap )
						{
						
						}
						
						if ( tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].lfResponseValueMultiply != 0.0 )
						{
							((char*)pReadValue)[iIndex] *=	tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].lfResponseValueMultiply;
						}
						
						((short*)*pGetValue)[iIndex] = ((char*)pReadValue)[iIndex];
					}
					
					if ( piValueSize )
						*piValueSize = sizeof(char);
					
					break;
			}
		}
		 
		if ( piValueLength )
			*piValueLength = tLocalStorage.iDataLength;
		
		if ( piValueSize )
			*piValueSize = sizeof(short);
		
		CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage )); 
		pLocalStorage->bDataUpdated = 0;
		CHK_CMT(CmtReleaseTSVPtr ( hHandle ));   
		
		sprintf( szFormatedLog , "%s :: Has Received respond of command [%s] %%s" ,  tLocalStorage.ptCallbacks->szName , tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].szCommandName );
	
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d");  
	
	}
	else
	{
	
		sprintf( szFormatedLog , "%s :: Hasn't Received respond of command [%s] %%s" ,  tLocalStorage.ptCallbacks->szName , tLocalStorage.pMODBUS_CommandList[iLastSentCommandIndex].szCommandName );
	
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d");  
	}
	
Error:

	if ( StdError.error )
	{
		if ( CmtGetTSVPtr ( hHandle , &pLocalStorage ) == 0 )
		{
			if ( pLocalStorage->pLastError )
			{
				UPDATERR( *pLocalStorage->pLastError );
			}
	
			CALLOC( pLocalStorage->pLastError , 1 , sizeof(STD_ERROR)); 
			
			CmtReleaseTSVPtr ( hHandle );
		}
	}

	RETURN_STDERR_POINTER;
}

   
void*   DLLEXPORT   MODBUS_Driver_Unplug( int hHandle , double lfTimeOut )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	FREE_STDERR_COPY_ERR( Communication_Unplug( pLocalStorage->hCommunicationHandle , lfTimeOut )); 

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}


void*   DLLEXPORT   MODBUS_Driver_PlugIn( int hHandle , double lfTimeOut )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	FREE_STDERR_COPY_ERR( Communication_PlugIn( pLocalStorage->hCommunicationHandle , lfTimeOut )); 

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

void*   DLLEXPORT   MODBUS_Driver_EnablePlugInAutomatically( int hHandle , int bEnable )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	pLocalStorage->bEnableAutoPlugIn = bEnable;

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

void*   DLLEXPORT   MODBUS_Driver_InstallReceiveCallbackFunction( int hHandle , tfStdReceiveCallback pfFunction , void *pData )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	pLocalStorage->pCallbackFunction = (void*) pfFunction;
	pLocalStorage->pCallbackData = pData;

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}


void*   DLLEXPORT   MODBUS_Driver_LoadConfigFile( int hHandle , char *pFileName )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			

	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};
	
	int							hFileHandle											=	0;
				
	int							iIndex												=	0,  
								iCount												=	0;
	
	char						szFormatedLog[LOW_STRING]							=	{0};

	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	IF (( pFileName == NULL ) , "File name is empty." );
	IF (( FileExists( pFileName , NULL ) == 0 )  , "File was not found." );  
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));  

	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks); 
	
	sprintf( szFormatedLog , "%s :: Loading configuration %%s" ,  pLocalStorage->ptCallbacks->szName );
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	
	hFileHandle = OpenFile ( pFileName , VAL_READ_ONLY, VAL_TRUNCATE, VAL_BINARY);
	
	FREE_CALLOC_ERR( pLocalStorage->pMODBUS_CommandList , 256 , sizeof(tsMODBUS_CommandItem));
	
	for ( iIndex = 0 ; iIndex < 256; iIndex++ )
	{
		iCount = ReadFile ( hFileHandle , pLocalStorage->pMODBUS_CommandList[iIndex].max_size , sizeof(tsMODBUS_CommandItem) );
		
		if ( iCount != sizeof(tsMODBUS_CommandItem) )
			break;
		
		if ( pLocalStorage->pMODBUS_CommandList[iIndex].ulSignatureID != 0 ) 
			if ( pLocalStorage->pMODBUS_CommandList[iIndex].ulSignatureID != DEVICE_ID )
				break; 
	}
	
	pLocalStorage->listSize = iIndex;

Error:

	if ( hFileHandle )
		CloseFile (hFileHandle);
	
	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

void*   DLLEXPORT   MODBUS_Driver_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			

	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};

	int							hFileHandle											=	0;
				
	int							iIndex												=	0,  
								iBaseSize											=	0, 
								iCount												=	0;

	char						szFormatedLog[LOW_STRING]							=	{0};
	
	IF (( hHandle == 0 ) , "handle can't be zero." );
	IF (( pBuffer == NULL ) , "Buffer is empty." );
		
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks); 
	
	sprintf( szFormatedLog , "%s :: Loading configuration %%s" ,  pLocalStorage->ptCallbacks->szName );
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
		
	FREE_CALLOC_ERR( pLocalStorage->pMODBUS_CommandList , 256 , sizeof(tsMODBUS_CommandItem));
	
	iCount = iBufferSize;
	iBaseSize = sizeof(tsMODBUS_CommandItem); 
	
	for ( iIndex = 0 ; iIndex < 256; iIndex++ )
	{
		if ( iCount < iBaseSize )   
			break;
		
		memcpy( pLocalStorage->pMODBUS_CommandList[iIndex].max_size , pBuffer , iBaseSize );
		
		if ( pLocalStorage->pMODBUS_CommandList[iIndex].ulSignatureID != 0 ) 
			if ( pLocalStorage->pMODBUS_CommandList[iIndex].ulSignatureID != DEVICE_ID )
				break;
				
		pBuffer += iBaseSize;
		iCount -= iBaseSize; 
	}
	
	pLocalStorage->listSize = iIndex;

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

void*   DLLEXPORT   MODBUS_Driver_Check_Connection( int hHandle , char *pCommandName , int *piStatus )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	int							iCommandIndex										=	0,
								bCommandFound										=	0,
								iNumberOfPorts										=	0; 
									
	IF (( hHandle == 0 ) , "handle can't be zero." );
	IF (( piStatus == 0 ) , "Status parameter can't be NULL." );
	
	*piStatus = 0;
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	for ( iCommandIndex = 0 ; iCommandIndex < pLocalStorage->listSize ; iCommandIndex++ )
		if ((strlen(pLocalStorage->pMODBUS_CommandList[iCommandIndex].szCommandName)) && ( strcmp( pCommandName , pLocalStorage->pMODBUS_CommandList[iCommandIndex].szCommandName ) == 0 ))
		{
			bCommandFound = 1;
			break;
		}
		
	IF (( bCommandFound == 0 ) , "Command was not found.");   
		
	*piStatus = 1;
		
Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}


void*	DLLEXPORT		MODBUS_Driver_Get_Commands_List( int hHandle , char ***pCommandsList , int *piNumberOfCommands )     
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			

	int							iIndex												=	0;

	IF (( hHandle == 0 ) , "handle can't be zero." );
	IF (( pCommandsList == NULL ) , "Pointer to List is empty." );
		
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	CALLOC_ERR( (*pCommandsList) , pLocalStorage->listSize+1 , sizeof(char*));
	
	for ( iIndex = 0 ; iIndex < pLocalStorage->listSize; iIndex++ )
	{
		CALLOC_COPY_STRING_ERR( (*pCommandsList)[iIndex] , pLocalStorage->pMODBUS_CommandList[iIndex].max_size );
		
		if ( piNumberOfCommands )
			(*piNumberOfCommands)++;
	}

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

unsigned short CalculateModBusCRC_16( unsigned char *buffer , unsigned int length )
{
	unsigned char 				iBitIndex		=	0;
	
	unsigned int				iIndex			=	0,
								iMaskIndex		=	0,
								crc				=	0,
								mem				=	0;

	static unsigned char 		Table1[256]		=	{0},
								Table2[256]		=	{0};
	
	unsigned char 				car				=	0;
	
	unsigned char				crc0			=	0xff,
								crc1			=	0xff;

	if (( Table1[1] == 0 ) && ( Table2[1] == 0 ))
	{
		for( iMaskIndex=0; iMaskIndex < 256; iMaskIndex++ ) 
		{
			crc = iMaskIndex;
		
			for( iBitIndex=0; iBitIndex < 8 ; iBitIndex++)
			{
				mem = (unsigned int)(crc & 0x0001) ;
		
				crc /= 2;
			
				if (mem) 
					crc ^= 0xA001;
			}
		
			Table2[iMaskIndex] = (unsigned char)(crc & 0xff); /* lobyte */
			Table1[iMaskIndex] = (unsigned char)(crc >> 8); /* hibyte */
		}
	}
	
	for( iIndex=0; iIndex < length; iIndex++) 
	{
		car = buffer[iIndex];
		car ^= crc0;
		crc0 = (unsigned char)(crc1 ^ Table2[car]);
		crc1 = Table1[car];
	}
	
	return (crc1<<8) + crc0;
}
