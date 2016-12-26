//==============================================================================
// Include files

#include "Windows.h" 
#include "cvidef.h"  
#include <userint.h>
#include <ansi_c.h>
#include <utility.h>
#include "GlobalDefines.h"
#include <formatio.h>
#include "toolbox.h"
#include "StandardProtocolDriver_Protocol.h"   
#include <visa.h>

//==============================================================================
// Constants

#define				PACKET_HEAD_LENGTH								4
#define				PACKET_MIN_LENGTH								5
#define				PACKET_CRC_LENGTH								1

#define				ERROR_HEADER									0xF0
#define				ERROR_COMMAND									0xF1
#define				ERROR_CHECKSUM									0xF2
#define				ERROR_DATA										0xF3
#define				ERROR_EXECUTION									0xF4
#define				ERROR_TIMEOUT									0xF5
#define				ERROR_MESSAGE_LENGTH							0xF8
#define				ERROR_DATA_LENGTH								0xF9

//==============================================================================
// Types																		 				   
																							
typedef struct PACKET
{
	unsigned char						header,
										opCode;

	unsigned short						length;

	unsigned char						checksum;

} tsPacket;

typedef struct STD_PROTOCOL_PACKET
{
	union
	{
		tsPacket						packet;

		unsigned char					buffer[128];
	};

} tsStdProtocolPacket;


typedef struct
{
	int							hCommunicationHandle;
	
	int							viOpenDefaultRM;
	
	int							hFileHandle;
	//------------- Communication ---------------//
	unsigned char 				*pBuffer;
	unsigned int				iBufferLength;

	unsigned char 				*pSourceBuffer;
	unsigned int				iSourceBufferLength;
	
	char						*pszAddress;
	
	char						*pLastSentCommand;
	
	int							bDataUpdated, 
								bDataUpdateFailed,
								iNumberOfCommunicationTry;

	double						lfLastCommandTime;
	
	int							iLastSentCommandIndex;
	
	int							iReceiveLowLevelTryNumber;
	
	double						lfLowLevelSendTimeOut,
								lfLowLevelReceiveTimeOut,
								lfLowLevelReceiveTimeOutHigh;
								
	double						lfSendTimeOut,
								lfReceiveTimeOut;
								
	tsSTD_CallBackSet			*ptCallbacks;
	
	tsSTD_CommandItem			*pSTD_CommandList;
	int							listSize;

	int							bEnableAutoPlugIn,
								iAutoPlugInRetryNum;	
	double						lfAutoPlugInRetryDelay; 
	
	int							bSeparateSendReceive; 
	
	int							bClosed;
	
	int							bSerialPort;
	
	void						*pCallbackFunction,
								*pCallbackData;
	
	STD_ERROR					tLastError; 
	
} tsLocalStorage;  


//==============================================================================
// Static functions

void	STD_Driver_ReceiveCallBack( void *pData );

void*   DLLEXPORT       STD_Driver_Send_Patern( int handle, tsSTD_CommandItem tCommandItem , unsigned char *pData , int iSize , *pbSentSuccessfully );

void*   AutoReconnect( int hHandle );

//==============================================================================
// Global functions	
void*	DLLEXPORT	STD_Driver_Config_Copy_STD_CallBackSet ( int *pHandle , void *pCallBackSet )
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

/***** ***** ***** ***** ***** ***** ***** ***** STD Driver Config LOG Set Allow State    ***** ***** ***** ***** ***** ***** ***** *****/    
void*	DLLEXPORT	STD_Driver_Config_LOG_SetAllowState( int iHandle , int bAllowLog )
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

/***** ***** ***** ***** ***** ***** ***** ***** STD Driver Config Install Comment Callback   ***** ***** ***** ***** ***** ***** ***** *****/   
void*	DLLEXPORT	STD_Driver_Config_Install_CommentCallback( CmtTSVHandle *pHandle , pfCommentCallback fFunction , void *pData , int type) 
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

/***** ***** ***** ***** ***** ***** ***** ***** STD Driver Config Install Config Value Callback   ***** ***** ***** ***** ***** ***** ***** *****/  
void*	DLLEXPORT	STD_Driver_Config_Install_ConfigValueCallback( CmtTSVHandle *pHandle , pfConfigValueCallback fFunction , void *pData , int type) 
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

/***** ***** ***** ***** ***** ***** ***** ***** STD Driver Config Install Check For Break Callback   ***** ***** ***** ***** ***** ***** ***** *****/	 
void*	DLLEXPORT	STD_Driver_Config_Install_CheckForBreakCallback( CmtTSVHandle *pHandle , pfCheckForBreakCallback fFunction , void *pData , int type)	
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


//****************************************************************************
//Receive Data


void	DLLEXPORT	STD_Control_Driver_Receive( int handle )
{
	STD_ERROR						StdError									=	{0};
	
	tsStdProtocolPacket				receivePacket								=	{0};

	unsigned int					receiveDataLength							=	0;

	unsigned char					receiveHeader								=	0,
									receiveOpCode								=	0;

	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								iLastSentCommandIndex						=	0,
									iReceiveLowLevelTryNumber					=	0;

	unsigned char					*pBuffer									=	NULL; 
	
	char							szLogFormated[LOW_STRING]					=	{0};
	
	char 							szGarbage[LOW_STRING]						=	{0};
	
	int								index										=	0, 
									iIndex										=	0,
									iIndexCopy									=	0,							
									iTryIndex									=	0; 
	
	int								iLowLevelTimeout							=	0,
									iLowLevelTimeoutLow							=	0;

	int								iReceiveCount								=	0,
									iToReceive									=	0,
									iCount										=	0;  			   
	
	int								hFileHandle									=	0,
									hCommunicationHandle						=	0;

	int								*pHandle									=	0;
	
	unsigned char					calcCheckSum								=	0;
	
	void							*pData 										=	NULL;

	double							lfLogCurrentTime							=	0;
	
	ViStatus						ConnectionStatus							=	0;
	
	int								bPacketReceivedSuccess						=	0,
									bSerialPort									=	0,
									bGetLock									=	0;
			
	FREE(pBuffer);
	
	SetBreakOnLibraryErrors (0);	//Terminate Non Fatal Error popup
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStorage ));					  
	bGetLock = 1;
	
	if ( pLocalStorage->tLastError.error )
		 pLocalStorage->tLastError = StdError;
	
	bSerialPort = pLocalStorage->bSerialPort;
	
	iLastSentCommandIndex = pLocalStorage->iLastSentCommandIndex;   
	
	hCommunicationHandle = pLocalStorage->hCommunicationHandle;  
	
	hFileHandle = pLocalStorage->hFileHandle;
	
	iReceiveLowLevelTryNumber = pLocalStorage->iReceiveLowLevelTryNumber;       //number of tries.
	
	iLowLevelTimeout = pLocalStorage->lfLowLevelReceiveTimeOutHigh * 1E3;  
	
	iLowLevelTimeoutLow = pLocalStorage->lfLowLevelReceiveTimeOut * 1E3;   

	iToReceive = PACKET_HEAD_LENGTH;
	
	viSetAttribute ( hCommunicationHandle , VI_ATTR_TMO_VALUE , iLowLevelTimeout );
	
	//get header packet 8 bytes
	for ( iTryIndex = 0; iTryIndex < iReceiveLowLevelTryNumber; iTryIndex++ )
	{
		ConnectionStatus = viRead ( hCommunicationHandle , (char*)(receivePacket.buffer + iReceiveCount) , iToReceive , &iCount );

		viSetAttribute ( hCommunicationHandle , VI_ATTR_TMO_VALUE , iLowLevelTimeoutLow );     
		
		iToReceive -= iCount;
		iReceiveCount += iCount;
		
		if ( iToReceive <= 0 )
			break;
		
		if (ConnectionStatus != VI_ERROR_TMO   &&   ConnectionStatus < VI_SUCCESS ) RETURN; //goto Error 
	}

	IF (( iReceiveCount <= 0 ) , "No data received." );   //return error "No data received" and goes to exit
	
	
	//if received noise in first byte  
	if  (receivePacket.packet.header != pLocalStorage->pSTD_CommandList[iLastSentCommandIndex].responseHeader) 
	{
		for (iIndex = 0; iIndex < iReceiveCount; iIndex++)   //Find where is bad Byte
		{
			
			iToReceive = iIndex; //How much bytes to receive after a bad Byte
			
			 	if (receivePacket.buffer[iIndex] == pLocalStorage->pSTD_CommandList[iLastSentCommandIndex].responseHeader) //Found a Header
				{
					for (iIndexCopy = 0 ; iIndex < iReceiveCount; iIndex++, iIndexCopy++) //Shift buffer
					receivePacket.buffer[iIndexCopy] = receivePacket.buffer[iIndex];
				
					ConnectionStatus = viRead ( hCommunicationHandle , (char*)(receivePacket.buffer + iIndexCopy) , iToReceive , &iCount ); //Receive other bytes 
						
					if (ConnectionStatus != VI_ERROR_TMO   &&   ConnectionStatus < VI_SUCCESS ) RETURN; //goto Error  
					
					break;
				}
		}
	}

	
	//create log file
	GetCurrentDateTime( &lfLogCurrentTime ); 
	
	FormatDateTimeString ( lfLogCurrentTime , "\r\n%H:%M:%S.%3f    Received    :    " , szLogFormated , (LOW_STRING-1) );
		
	if ( hFileHandle )
	{
		WriteFile ( hFileHandle , szLogFormated , strlen(szLogFormated) ); 
		
		for ( iIndex = 0; iIndex < PACKET_HEAD_LENGTH; iIndex++ )  
		{
			sprintf( szLogFormated , "%02X " , receivePacket.buffer[iIndex] );
			WriteFile ( hFileHandle , szLogFormated , strlen(szLogFormated) ); 
		}
	}
	
	IF (( iReceiveCount < PACKET_HEAD_LENGTH ) , "broken package." );
	
 
	receiveHeader = receivePacket.packet.header;
	receiveDataLength = receivePacket.packet.length;
	receiveOpCode = receivePacket.packet.opCode;

	
	//check for errors (header and OpCode)
	if (( iLastSentCommandIndex >= 0) && ( receiveHeader != pLocalStorage->pSTD_CommandList[iLastSentCommandIndex].responseHeader ))
	{   
		FORCE_ERR( -1 , "Receive Header Error");	
	}
	
	if (( iLastSentCommandIndex >= 0) && ( receiveOpCode != pLocalStorage->pSTD_CommandList[iLastSentCommandIndex].responseOpCode ))
	{   
		IF(( receiveOpCode == 0xF0 ) , "Header error");
		IF(( receiveOpCode == 0xF1 ) , "Command error"); 
		IF(( receiveOpCode == 0xF2 ) , "Checksum error"); 
		IF(( receiveOpCode == 0xF3 ) , "Data error"); 
		IF(( receiveOpCode == 0xF4 ) , "Execution Error"); 
		IF(( receiveOpCode == 0xF5 ) , "Time-out Error type 1"); 
		IF(( receiveOpCode == 0xF6 ) , "Time-out Error type 2");
		IF(( receiveOpCode == 0xF7 ) , "Time-out Error type 3"); 

		FORCE_ERR( -1 , "Receive OpCode Error");	
	}
	
	
	CALLOC_ERR( pBuffer , receiveDataLength , sizeof(char));     //reserve memory - size of DataLength 
	
	iReceiveCount = 0;
	
	iToReceive = receiveDataLength; 
	
	//receive data
	for ( iTryIndex = 0; iTryIndex < iReceiveLowLevelTryNumber; iTryIndex++ )
	{
		while (1)
		{
			ConnectionStatus	=	viRead ( hCommunicationHandle , (pBuffer+iReceiveCount) , iToReceive , &iCount );

			iToReceive -= iCount;
			iReceiveCount += iCount;
		
			if (ConnectionStatus != VI_ERROR_TMO   &&   ConnectionStatus < VI_SUCCESS ) RETURN; //goto Error 	
			
			if (iCount == 0)
				break;
		}
		
		if ( iReceiveCount >= receiveDataLength )
				break;
	}
	
	
	IF (( iReceiveCount != receiveDataLength ) , "broken package." ); 
	
	if ( hFileHandle )
	{
		if ( receiveDataLength < 32 ) 
		{
			for ( iIndex = 0; iIndex < receiveDataLength; iIndex++ )  
			{
				sprintf( szLogFormated , "%02X " , pBuffer[iIndex] );		  
				WriteFile ( hFileHandle , szLogFormated , strlen(szLogFormated) ); 
			}
		}
		else
		{
			sprintf( szLogFormated , "..." );
			WriteFile ( hFileHandle , szLogFormated , strlen(szLogFormated) ); 
		}
	}
	
	//receive checksum
	ConnectionStatus = viRead ( hCommunicationHandle , receivePacket.buffer + PACKET_HEAD_LENGTH , PACKET_CRC_LENGTH , &iReceiveCount );
	
	if (ConnectionStatus != VI_ERROR_TMO   &&   ConnectionStatus < VI_SUCCESS ) RETURN; //goto Error 

	IF (( iReceiveCount != PACKET_CRC_LENGTH ) , "broken package." ); 
	
	if ( hFileHandle )
	{   
		for ( iIndex = PACKET_HEAD_LENGTH; iIndex < PACKET_MIN_LENGTH; iIndex++ )  		   
		{
			sprintf( szLogFormated , "%02X " , receivePacket.buffer[iIndex] );  
			WriteFile ( hFileHandle , szLogFormated , strlen(szLogFormated) ); 
		}
		
	}
	
	if  ( receiveDataLength < 512 ) 
	{
		for ( index = 0 ; index < PACKET_HEAD_LENGTH ; index++ )    
			calcCheckSum += receivePacket.buffer[index]; 
	
	
		for (index = 0 ; index < receiveDataLength ; index++ )
			calcCheckSum += pBuffer[index];

	
		IF (( receivePacket.packet.checksum != calcCheckSum ) , "Receive Wrong CheckSum");
	}
	
	bPacketReceivedSuccess = 1;
	
	FREE(pLocalStorage->pBuffer);
	
	pLocalStorage->pBuffer = pBuffer;

	pLocalStorage->iBufferLength = receiveDataLength;

	pLocalStorage->bDataUpdated = 1;  
	
	if ( pLocalStorage->pCallbackFunction )
	{
		CALLOC_ERR( pHandle , 1 , sizeof(int));
		*pHandle = handle;
		pData = pHandle;

		PostDeferredCallToThread ( STD_Driver_ReceiveCallBack , pData , CmtGetCurrentThreadID() );  
	}
	
Error:
	
/*	while (1)	
	{
		if ( bSerialPort == 0 )
		{
			viRead ( hCommunicationHandle , (char*)(szGarbage) , PACKET_HEAD_LENGTH , &iCount );
		}
		else
		{
			viGetAttribute ( hCommunicationHandle , VI_ATTR_ASRL_AVAIL_NUM, &iCount );
			viSetAttribute ( hCommunicationHandle , VI_ATTR_TMO_VALUE , iLowLevelTimeout/6 );
			
			if ( iCount > 0 )
			viRead ( hCommunicationHandle , (char*)(szGarbage) , PACKET_HEAD_LENGTH , &iCount );   
		}
		
		if (iCount == 0)
		{
			viSetAttribute ( hCommunicationHandle , VI_ATTR_TMO_VALUE , iLowLevelTimeout ); 
			break;	
		}
	}   */
	
	if ( bPacketReceivedSuccess == 0 )
	{
		if ( bSerialPort == 0 ) 
		{
			while (1)
			{
				viRead ( hCommunicationHandle , (char*)(szGarbage) , PACKET_HEAD_LENGTH , &iCount );
					if (iCount == 0) 
						break;
			}
		}
		else
		{
			viGetAttribute ( hCommunicationHandle , VI_ATTR_ASRL_AVAIL_NUM, &iCount );  
	
			if ( iCount > 0 ) 
			{
				while(1)
				{
					viGetAttribute ( hCommunicationHandle , VI_ATTR_ASRL_AVAIL_NUM, &iCount );
					viSetAttribute ( hCommunicationHandle , VI_ATTR_TMO_VALUE , iLowLevelTimeoutLow/6 );
			
					if ( iCount > 0 )
						viRead ( hCommunicationHandle , (char*)(szGarbage) , PACKET_HEAD_LENGTH , &iCount );
				
					else if ( iCount == 0 )
					{
						viSetAttribute ( hCommunicationHandle , VI_ATTR_TMO_VALUE , iLowLevelTimeoutLow ); 
						
						break;	
					}
				}
			}
		
		}
	}
	
	if ( handle && IS_NOT_OK && bGetLock )
		pLocalStorage->tLastError = StdError;  	
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );  
	
	if (( ConnectionStatus != VI_ERROR_TMO ) && ( ConnectionStatus < VI_SUCCESS ))
		AutoReconnect ( handle );
						
	SetBreakOnLibraryErrors (1);	// Enable Non Fatal Error popup 
	
	return;
}
 

//*********************************************************************************************

void	STD_Driver_ReceiveCallBack( void *pData )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			

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
		
		CALLOC_COPY_STRING( pCommand , pLocalStorage->pSTD_CommandList[iLastSentCommandIndex].szCommandName );

		CALLOC_COPY_ERR( pValue , (pLocalStorage->iBufferLength + 1) , 1 , pLocalStorage->pBuffer , pLocalStorage->iBufferLength );
		
		iValueSize = pLocalStorage->iBufferLength;
		
		CALLOC_COPY_ERR( pReceiveBuffer , (pLocalStorage->iSourceBufferLength + 1) , 1 , pLocalStorage->pSourceBuffer , pLocalStorage->iSourceBufferLength );
		
		iBufferSize = pLocalStorage->iSourceBufferLength;
		
		receiveFunction = pLocalStorage->pCallbackFunction;
		
		pData = pLocalStorage->pCallbackData;
		
		CmtReleaseTSVPtr ( handle ); 
		handle = 0;
	}	
		
	if ( receiveFunction )
		receiveFunction( pData , pCommand , pValue , iValueSize , pReceiveBuffer , iBufferSize ); 
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle ); 
	
	return;
}

/***** ***** ***** ***** ***** ***** ***** ***** STD Driver Init  ***** ***** ***** ***** ***** ***** ***** *****/	
void*	DLLEXPORT		STD_Driver_Init( char *pszConnectionName , char *pszAddress , int *pHandle , ... )
{
	STD_ERROR						StdError									=	{0};
	
	tsSTD_CallBackSet				tSTD_CallBackSet							=	{0};

	tsLocalStorage					*pLocalStorage								=	NULL;	
	
	ViUInt32						iDB_BaudRate							  	=	0,
									iDB_DataBits							  	=	0,
									iDB_Parity								  	=	0,
									iDB_StopBits							  	=	0,
									iDB_FlowCtrl							  	=	0,
									iDB_Default								  	=	0;
	
	int								handle										=	0;
	
	int								hCommunicationHandle						=	0;
	
	double							lfLowLevelReceiveTimeOutHigh				=	5,   //0.05	
									lfLowLevelReceiveTimeOut			   		=   0.3,
									lfLowLevelSendTimeOut						=	0.1;   //0.1   		 
									
	double							lfReceiveTimeOut							=	1.0,   //1.0
									lfSendTimeOut								=	1.0;   //1.0
	
	int								iNumberOfReTry								=	3;		
											 
	int								*pShareHandle								=	NULL;
	
	char							szFormatedLog[STD_STRING]					=	{0},
									szLogFormated[STD_STRING]					=	{0},  
									szLogFileName[STD_STRING]					=	{0};  

	int								iLowLevelTimeout							=	0.2,		//0.2	   //wait for read function
									iReceiveLowLevelTryNumber					=	2; 
	
	int 							bEnableAutoPlugIn							=	1,
									bSeparateSendReceive						=	0, 
									iAutoPlugInRetryNum							=	3;  //Number of tries
	
	double							lfAutoPlugInRetryDelay						=	20; // sec
									
	double							lfLogCurrentTime							=	0.0;

	IF ((( pHandle == NULL ) || ( pszAddress == NULL )  || ( pszConnectionName == NULL )), "Pointer to handle can't be NULL." ); //return handle , protection     
	
	if ( *pHandle == 0 )
	{
		CHK_CMT(CmtNewTSV ( sizeof(tsLocalStorage) , pHandle ));   //  open new tread
	}
	
	handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &pLocalStorage ));     //use the tread
	
	pShareHandle = calloc(1,sizeof(int));
	
	IF (( pShareHandle == NULL ) , "Can't allocate memory." );
	
	*pShareHandle = handle;									  //handle to call callback functions
	
	if ( pLocalStorage->ptCallbacks == NULL )
	{
		CALLOC_ERR( pLocalStorage->ptCallbacks , 1 , sizeof(tsSTD_CallBackSet));  
	}
	
	CALLOC_COPY_STRING(pLocalStorage->pszAddress , pszAddress );  //save address on Local storage
	
	pLocalStorage->ptCallbacks->iInternalHandle = handle;
	
	strcpy( pLocalStorage->ptCallbacks->szName , pszConnectionName );
	
	pLocalStorage->iLastSentCommandIndex = -1;
	
	STDF_SET_MODULE_NAME(pLocalStorage->ptCallbacks,pszConnectionName);
	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks);
	
	if ( pszConnectionName )
	{
		sprintf( szFormatedLog , "Initializing \"%s\" Driver %%s" ,  pszConnectionName );
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	}
																	  //get parameters from database or set default in DB
	STDF_CONFIG_VALUE("LowLevelReceiveTimeOut", VAL_DOUBLE , 1 , lfLowLevelReceiveTimeOut , lfLowLevelReceiveTimeOut );
	STDF_CONFIG_VALUE("LowLevelReceiveTimeOutHigh", VAL_DOUBLE , 1 , lfLowLevelReceiveTimeOutHigh , lfLowLevelReceiveTimeOutHigh ); 
	STDF_CONFIG_VALUE("LowLevelSendTimeOut", VAL_DOUBLE , 1 , lfLowLevelSendTimeOut ,lfLowLevelSendTimeOut );   
	
	STDF_CONFIG_VALUE("ReceiveTimeOut", VAL_DOUBLE , 1 , lfReceiveTimeOut , lfReceiveTimeOut );
	STDF_CONFIG_VALUE("SendTimeOut", VAL_DOUBLE , 1 , lfSendTimeOut , lfSendTimeOut );   
	
	STDF_CONFIG_VALUE("NumberOfReTry", VAL_INTEGER , 1 , iNumberOfReTry , iNumberOfReTry );

	STDF_CONFIG_VALUE("ReceiveLowLevelTryNumber", VAL_INTEGER , 1 , iReceiveLowLevelTryNumber , iReceiveLowLevelTryNumber );
	
	STDF_CONFIG_VALUE("EnableAutoPlugIn", VAL_INTEGER , 1 , bEnableAutoPlugIn , bEnableAutoPlugIn ); 
	STDF_CONFIG_VALUE("AutoPlugInRetryNum", VAL_INTEGER , 1 , iAutoPlugInRetryNum , iAutoPlugInRetryNum );
	STDF_CONFIG_VALUE("AutoPlugInRetryDelay", VAL_INTEGER , 1 , lfAutoPlugInRetryDelay , lfAutoPlugInRetryDelay );	
	STDF_CONFIG_VALUE("SeparateSendReceive", VAL_INTEGER , 1 , bSeparateSendReceive , bSeparateSendReceive );	
	
	pLocalStorage->bSeparateSendReceive = bSeparateSendReceive;
	pLocalStorage->iReceiveLowLevelTryNumber = iReceiveLowLevelTryNumber;  
	pLocalStorage->iNumberOfCommunicationTry = iNumberOfReTry;
	pLocalStorage->lfLowLevelReceiveTimeOut = lfLowLevelReceiveTimeOut;
	pLocalStorage->lfLowLevelReceiveTimeOutHigh = lfLowLevelReceiveTimeOutHigh; 
	pLocalStorage->lfLowLevelSendTimeOut = lfLowLevelSendTimeOut;
	pLocalStorage->lfReceiveTimeOut = lfReceiveTimeOut;
	pLocalStorage->lfSendTimeOut = lfSendTimeOut;
	pLocalStorage->bEnableAutoPlugIn = bEnableAutoPlugIn;
	pLocalStorage->iAutoPlugInRetryNum = iAutoPlugInRetryNum;
	pLocalStorage->lfAutoPlugInRetryDelay = lfAutoPlugInRetryDelay; 

	CHK_VSA( viOpenDefaultRM (&pLocalStorage->viOpenDefaultRM));
	
	CHK_VSA( viOpen ( pLocalStorage->viOpenDefaultRM , pszAddress , VI_NULL, VI_NULL, &hCommunicationHandle ));
	
	pLocalStorage->hCommunicationHandle = hCommunicationHandle;

	iLowLevelTimeout = lfLowLevelReceiveTimeOut * 1E3;
	
	if ( iLowLevelTimeout > 0 )
	{
		CHK_VSA( viSetAttribute ( hCommunicationHandle , VI_ATTR_TMO_VALUE , iLowLevelTimeout )); 
	}
	
	if ( pszConnectionName )
	{
		sprintf( szLogFileName , "COM_%s.log" , pszConnectionName );
	}
	else
	{
		if ( pszAddress )
			sprintf( szLogFileName , "COM_%s.log" , pszAddress );
	}
	
	pLocalStorage->hFileHandle = OpenFile ( szLogFileName , VAL_READ_WRITE, VAL_TRUNCATE , VAL_BINARY);
	
	GetCurrentDateTime(&lfLogCurrentTime);
	
	FormatDateTimeString ( lfLogCurrentTime , "\r\n%H:%M:%S.%3f    Open        :    " , szLogFormated , 1024 );
	
	if ( pLocalStorage->hFileHandle )
		WriteFile ( pLocalStorage->hFileHandle , szLogFormated , strlen(szLogFormated) ); 
				
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
	{
		CmtReleaseTSVPtr ( handle );
	
		STD_Config_Attributes( handle );
	}
	
	RETURN_STDERR_POINTER;
} 


void* 	DLLEXPORT	STD_Config_Attributes( int hHandle )
{
	STD_ERROR						StdError									=	{0};
	
	tsSTD_CallBackSet				tSTD_CallBackSet							=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	ViUInt32						iDB_BaudRate							  	=	0,
									iDB_DataBits							  	=	0,
									iDB_Parity								  	=	0,
									iDB_StopBits							  	=	0,
									iDB_FlowCtrl							  	=	0,
									iDB_Default								  	=	0;

	int								*pHandle									=	0,
									handle										=	0;
		
	char							szFormatedLog[LOW_STRING]					=	{0};

	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks);
	
	if( strstr(pLocalStorage->pszAddress,"TCP") != NULL )
	{
	

	}	
	
	if( strstr(pLocalStorage->pszAddress,"ASRL") != NULL )  
	{
		pLocalStorage->bSerialPort = 1;
		
		sprintf( szFormatedLog , "%s :: Configurating communication %%s" ,  pLocalStorage->ptCallbacks->szName );
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	
		iDB_BaudRate = 115200;
	
		STDF_CONFIG_VALUE("BaudRate", VAL_INTEGER , 1 , iDB_BaudRate , iDB_BaudRate );	
	
		CHK_VSA( viSetAttribute ( pLocalStorage->hCommunicationHandle  , VI_ATTR_ASRL_BAUD , iDB_BaudRate ));
	
		iDB_DataBits = 8;
	
		STDF_CONFIG_VALUE("DataBits", VAL_INTEGER , 1 , iDB_DataBits , iDB_DataBits );
		
		CHK_VSA( viSetAttribute (pLocalStorage->hCommunicationHandle , VI_ATTR_ASRL_DATA_BITS, iDB_DataBits));
	
		iDB_Parity = 0;
	
		STDF_CONFIG_VALUE("Parity", VAL_INTEGER , 1 , iDB_Parity , iDB_Parity );
		
		CHK_VSA( viSetAttribute (pLocalStorage->hCommunicationHandle , VI_ATTR_ASRL_PARITY, iDB_Parity));
	
		iDB_StopBits = 10;
	
		STDF_CONFIG_VALUE("StopBits", VAL_INTEGER , 1 , iDB_StopBits , iDB_StopBits );
		
		CHK_VSA( viSetAttribute (pLocalStorage->hCommunicationHandle , VI_ATTR_ASRL_STOP_BITS, iDB_StopBits));
	
		iDB_FlowCtrl = 0;
	
		STDF_CONFIG_VALUE("FlowCtrl", VAL_INTEGER , 1 , iDB_FlowCtrl , iDB_FlowCtrl );
		
		CHK_VSA( viSetAttribute (pLocalStorage->hCommunicationHandle , VI_ATTR_ASRL_FLOW_CNTRL, iDB_FlowCtrl));
	
		CHK_VSA( viSetAttribute ( pLocalStorage->hCommunicationHandle  , VI_ATTR_ASRL_DISCARD_NULL , VI_FALSE ));
	}
	
Error:
	
	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	RETURN_STDERR_POINTER;
}




void* 	DLLEXPORT	STD_Config_SetAttribute( int hHandle , char *pAttributeName , char *pAttributeValue )
{
	STD_ERROR						StdError									=	{0};

	tsLocalStorage					*pLocalStorage								=	NULL;

	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	if (( pAttributeName == NULL ) || ( pAttributeValue == NULL ))
	{
		RETURN;
	}
	if ( strcmp( pAttributeName , "SendTimeOut" ) == 0 )
	{
		pLocalStorage->lfSendTimeOut = atof(pAttributeValue);
		RETURN;
	}
	if ( strcmp( pAttributeName , "ReceiveTimeOut" ) == 0 )
	{
		pLocalStorage->lfReceiveTimeOut = atof(pAttributeValue);
		RETURN;
	}
	if ( strcmp( pAttributeName , "NumberOfReTry" ) == 0 )
	{
		pLocalStorage->iNumberOfCommunicationTry = atoi(pAttributeValue);
		RETURN;
	}
	
Error:
	
	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	RETURN_STDERR_POINTER;
}

	
/***** ***** ***** ***** ***** ***** ***** ***** STD Driver Close  ***** ***** ***** ***** ***** ***** ***** *****/	 
void*	DLLEXPORT		STD_Driver_Close(int *pHandle)
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	int								handle										=	0;
	
	char							szFormatedLog[LOW_STRING]					=	{0},
									szLogFormated[STD_STRING]					=	{0}; 
									
	tsSTD_CallBackSet				tSTD_CallBackSet							=	{0};
	
	double							lfLogCurrentTime							=	0.0;
	
	IF (( pHandle == NULL ) , "Pointer to handle can't be NULL." );
	
	handle = *pHandle;  

	if ( handle && ( CmtGetTSVPtr ( handle , &pLocalStorage ) == 0 ))
	{
		STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks); 
	
		sprintf( szFormatedLog , "%s :: Close communication %%s" ,  pLocalStorage->ptCallbacks->szName );
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	
		if ( pLocalStorage->hCommunicationHandle )
			viClose ( pLocalStorage->hCommunicationHandle );

		CmtReleaseTSVPtr ( handle ); 
	}
	
Error:

	if ( handle && ( CmtGetTSVPtr ( handle , &pLocalStorage ) == 0 ))
	{
		*pHandle = 0;

		FREE(pLocalStorage->pszAddress); 
	
		FREE(pLocalStorage->pSourceBuffer);
			
		FREE(pLocalStorage->pBuffer);
		
		FREE(pLocalStorage->pLastSentCommand);
		
		FREE(pLocalStorage->ptCallbacks);

		GetCurrentDateTime(&lfLogCurrentTime);
		FormatDateTimeString ( lfLogCurrentTime , "\r\n\r\n%H:%M:%S.%3f    Close.\r\n\r\n" , szLogFormated , 1023 );
		
		if ( pLocalStorage->hFileHandle )
			WriteFile ( pLocalStorage->hFileHandle , szLogFormated , strlen(szLogFormated) ); 
		
		CloseFile ( pLocalStorage->hFileHandle );
		
		CmtReleaseTSVPtr ( handle );
	
		CmtDiscardTSV ( handle );
	}
	
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** STD Driver TCP Reset ***** ***** ***** ***** ***** ***** ***** *****/    
void*	DLLEXPORT		STD_Driver_Reset(int hHandle)
{
	STD_ERROR						StdError									=	{0};

	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** ***** ***** STD Driver Get Connection Address  ***** ***** ***** ***** ***** ***** ***** *****/	
void*   DLLEXPORT       STD_Driver_GetConnectionAddress( int hHandle , char	**szAddress )
{
	STD_ERROR						StdError									=	{0};
	
	tsLocalStorage					*pLocalStorage								=	NULL;			
	
	IF (( hHandle == 0 ) , "Pointer to handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	strcpy( szAddress , pLocalStorage->pszAddress );  
	
Error:
	
	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
	
	RETURN_STDERR_POINTER;
} 



void*   DLLEXPORT       STD_Driver_Send_Patern( int hHandle, tsSTD_CommandItem tCommandItem , unsigned char *pData , int iSize , *pbSentSuccessfully) 
{
	STD_ERROR				StdError											=	{0};
	
	unsigned char			*pSendBuffer										=	NULL;

	unsigned short			*pBuffer											=	NULL;
	
	tsLocalStorage			*pLocalStorage										=	NULL;
	
	unsigned int			iSendLenth											=	0,
							index												=	0,
							iCount												=	0;
	
	unsigned char			checkSumCalc										=	0;
	
	int						hFileHandle											=	0,
							hCommunicationHandle								=	0;
	
	double					lfLogCurrentTime									=	0.0,
							lfLowLevelSendTimeOut								=	0.0;						   
	
	char					szLogFormated[LOW_STRING]							=	{0}; 
	
	ViStatus				ConnectionStatus									=	0; 
																							
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	hCommunicationHandle = pLocalStorage->hCommunicationHandle;  
	lfLowLevelSendTimeOut = pLocalStorage->lfLowLevelSendTimeOut;
	
	hFileHandle = pLocalStorage->hFileHandle;	

	if ( iSize == 0 )
		iSize = tCommandItem.dataSize;

	if ( tCommandItem.iSendValue )
		pData = (char*)&(tCommandItem.iSendValue);

	CALLOC_ERR( pSendBuffer , (iSize + PACKET_HEAD_LENGTH + PACKET_CRC_LENGTH ) , sizeof(char));

	pSendBuffer[iSendLenth++] = tCommandItem.sendHeader;
	pSendBuffer[iSendLenth++] = tCommandItem.opCode;
	pSendBuffer[iSendLenth++] = (char)iSize;
	pSendBuffer[iSendLenth++] = (char)( iSize >> 8 );
	
    //calculate  checksum
	for ( index = 0 ; index < PACKET_HEAD_LENGTH; index++ )
		checkSumCalc += pSendBuffer[index];

	for ( index = 0 ; index < iSize ; index++)
		checkSumCalc += pSendBuffer[index+PACKET_HEAD_LENGTH] = pData[index];									  
	
	iSendLenth = iSize + PACKET_HEAD_LENGTH;

	pSendBuffer[iSendLenth++] = checkSumCalc % 256;

	//send
	*pbSentSuccessfully=0;
	
	ConnectionStatus = viWrite ( hCommunicationHandle , pSendBuffer , iSendLenth , &iCount );
	
	if (  ConnectionStatus	<	VI_SUCCESS )	RETURN;
	
	if (  ConnectionStatus	>=	VI_SUCCESS) *pbSentSuccessfully = 1;

	IF (( iCount!=iSendLenth ),"Unable to send entire data bytes");

	GetCurrentDateTime( &lfLogCurrentTime ); 

	FormatDateTimeString ( lfLogCurrentTime , "\r\n\r\n%H:%M:%S.%3f    Sent        :    " , szLogFormated , (LOW_STRING-1) );

	if ( hFileHandle )
	{
		WriteFile ( hFileHandle , szLogFormated , strlen(szLogFormated) ); 
	
		for ( index = 0; index < PACKET_HEAD_LENGTH ; index++ )  
		{
			sprintf( szLogFormated , "%02X " ,pSendBuffer[index] );
			
			WriteFile ( hFileHandle , szLogFormated , strlen(szLogFormated) ); 
		}
		
		if (  iSize < 32 ) 
		{
			for ( ; index <  iSize + PACKET_HEAD_LENGTH ; index++ ) 
			{
				sprintf( szLogFormated , "%02X " ,pSendBuffer[index] );
			
				WriteFile ( hFileHandle , szLogFormated , strlen(szLogFormated) ); 
			}
		}
		else
		{
			sprintf( szLogFormated , "..." );
			WriteFile ( hFileHandle , szLogFormated , strlen(szLogFormated) ); 
		}
		
		for (  index = 0; index < PACKET_CRC_LENGTH ; index++ )  
		{
			sprintf( szLogFormated , "%02X " ,pSendBuffer[PACKET_HEAD_LENGTH + iSize + index] );
			
			WriteFile ( hFileHandle , szLogFormated , strlen(szLogFormated) ); 
		}
		
//			WriteFile ( hFileHandle , "\r\n" , 2); 
	}

Error:
	
	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );
		
	if ( ConnectionStatus  <  VI_SUCCESS )	   
		AutoReconnect ( hHandle );
	
	FREE(pSendBuffer);
	
	RETURN_STDERR_POINTER;
}

///--------------------- API ------------------------------//

void*   DLLEXPORT   STD_Driver_SetValue( int hHandle , char *pCommandName , void *pValue , int iValueLength , int iValueSize , ...  )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL,			
								tLocalStorage										=	{0};
	
	int							bCommandFound										=	0,
								iCommandIndex										=	0,
								iSize												=	0, 
								iDataLen											=	0;
	
	unsigned char 				*pData												=	NULL;
	
	char						szFormatedLog[LOW_STRING]							=	{0};
	
	double						lfStartTime											=	0.0,
								lfCurrentTime										=	0.0;
	
	int							iTryCounter											=	0;
	
	char						szLog[1024]											=	{0};
	
	int							bDataUpdated										=	0,
								bSentSuccessfully									=	0; 
									
	double						lfWaitForConnectionTimeout							=	0.0;
	
	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};

	IF (( hHandle == 0 ) , "handle can't be zero." );
	IF ((( iValueLength > 0 ) && ( pValue == NULL )) , "Value can't be NULL." ); 
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	memcpy( &tLocalStorage , pLocalStorage , sizeof(tsLocalStorage));
	CHK_CMT(CmtReleaseTSVPtr ( hHandle ));   
	
	STDF_UPDATE_CALLBACK_DATA(tLocalStorage.ptCallbacks);
	
	GetCurrentDateTime(&lfStartTime); 
	
	lfWaitForConnectionTimeout = (double)tLocalStorage.iNumberOfCommunicationTry * tLocalStorage.lfReceiveTimeOut;

	for ( iCommandIndex = 0 ; iCommandIndex < tLocalStorage.listSize ; iCommandIndex++ )
		if ( strcmp( pCommandName , tLocalStorage.pSTD_CommandList[iCommandIndex].szCommandName ) == 0 )
		{
			bCommandFound = 1;
			break;
		}
		
	IF (( bCommandFound == 0 ) , "Command was not found.");   
		
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));  
	pLocalStorage->iLastSentCommandIndex = iCommandIndex;
	pLocalStorage->bDataUpdated = 0;
	CHK_CMT(CmtReleaseTSVPtr ( hHandle )); 
	
	iDataLen = tLocalStorage.pSTD_CommandList[iCommandIndex].dataSize;
	iSize = (iValueSize * iValueLength);
	
	CALLOC_ERR( pData , ( iDataLen + iSize ) , sizeof(char));
	
	switch ( tLocalStorage.pSTD_CommandList[iCommandIndex].sendingSizeType )
	{
		case 0:
				// Constant Size 
				if (( pValue ) && ( iSize >= iDataLen ))
					memcpy( pData , pValue	, iSize ); 
				
				iSize = 0;
				
				break;
				
		case 1:
		default:
				if (( pValue ) && ( iSize ))
					memcpy( pData , pValue	, iSize );  
				
				break;
	
	}
	
	for ( iTryCounter = 0 ; iTryCounter < tLocalStorage.iNumberOfCommunicationTry ; iTryCounter++ )
	{
		FREE_STDERR_COPY_ERR( STD_Driver_Send_Patern( hHandle, tLocalStorage.pSTD_CommandList[iCommandIndex] , pData , iSize , &bSentSuccessfully)); 
	
		GetCurrentDateTime(&lfStartTime);
		
		if ( IS_NOT_OK )
		{
			OutputDebugString("\t\tUUT Driver Error : Can't send command.");   
			DelayWithEventProcessing(5.0);
			sprintf( szLog , "\t\tUUT Driver       : Try to send command again. (%d/%d)" , iTryCounter+1 , tLocalStorage.iNumberOfCommunicationTry );
			OutputDebugString(szLog);  
			continue;
		}
		
		if (  IS_NOT_OK || ( bSentSuccessfully == 0 ))
			continue;
		
		if ( tLocalStorage.bSeparateSendReceive == 0 )
		{
			DelayWithEventProcessing((double)tLocalStorage.pSTD_CommandList[iCommandIndex].sendReceiveDelay_Msec / 1E3 );
		
			do
			{
				STD_Control_Driver_Receive( hHandle );

				CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage )); 
		
				bDataUpdated = pLocalStorage->bDataUpdated;
			
				if ( pLocalStorage->tLastError.error )
					StdError = pLocalStorage->tLastError;
			
				CmtReleaseTSVPtr ( hHandle );            
			
				if ( bDataUpdated )
					break;
			
				if ( IS_NOT_OK )			
					break;
			
				GetCurrentDateTime(&lfCurrentTime);
			
			} while((lfCurrentTime-lfStartTime) < tLocalStorage.lfSendTimeOut );
		
			if ((IS_OK) && bDataUpdated ) 			
				break;
		}
		else
		{
			break;
		}
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



void*   DLLEXPORT   STD_Driver_GetValue( int hHandle , char **pGetCommandName , void **pGetValue , int *piValueLength , int *piValueSize, ...  )    
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL,			
								tLocalStorage										=	{0};			

	char						szFormatedLog[LOW_STRING]							=	{0};
								
	double						lfStartTime											=	0.0,
								lfCurrentTime										=	0.0;

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
		FREE_CALLOC_COPY_STRING( *pGetCommandName , tLocalStorage.pSTD_CommandList[iLastSentCommandIndex].szCommandName );
	}
		
	GetCurrentDateTime(&lfStartTime);

	do
	{
		if ( tLocalStorage.bSeparateSendReceive )
			STD_Control_Driver_Receive( hHandle );
		
		CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
		memcpy( &tLocalStorage , pLocalStorage , sizeof(tsLocalStorage));
		CHK_CMT(CmtReleaseTSVPtr ( hHandle ));   
	
		if ( tLocalStorage.bDataUpdated )
			break;
		
		if ( tLocalStorage.tLastError.error ) 
			break;
		
		GetCurrentDateTime(&lfCurrentTime);
		
	} while((lfCurrentTime-lfStartTime) < tLocalStorage.lfReceiveTimeOut );
	
	if ((lfCurrentTime-lfStartTime) >= tLocalStorage.lfReceiveTimeOut )
		OutputDebugString("\t\tUUT Driver Error : Receive timeout."); 
		
	if ( tLocalStorage.bDataUpdated )
	{
		if ( pGetValue )
		{
			FREE_CALLOC_COPY_ERR(((char**)*pGetValue) , (tLocalStorage.iBufferLength + 1) , 1 , tLocalStorage.pBuffer , tLocalStorage.iBufferLength );	  
		}
		
		if ( piValueLength )
			*piValueLength = tLocalStorage.iBufferLength;
		
		if ( piValueSize )
			*piValueSize = 1;
		
		CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage )); 
		pLocalStorage->bDataUpdated = 0;
		CHK_CMT(CmtReleaseTSVPtr ( hHandle ));   
		
		sprintf( szFormatedLog , "%s :: Has Received respond of command [%s] %%s" ,  tLocalStorage.ptCallbacks->szName , tLocalStorage.pSTD_CommandList[iLastSentCommandIndex].szCommandName );
	
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d");  
	
	}
	else
	{
	
		sprintf( szFormatedLog , "%s :: Hasn't Received respond of command [%s] %%s" ,  tLocalStorage.ptCallbacks->szName , tLocalStorage.pSTD_CommandList[iLastSentCommandIndex].szCommandName );
	
		STDF_COMMENT(0,szFormatedLog,"On Slot N%d");  
	}
	
Error:
  
	if ( StdError.error == 0 )
	{
		if ( CmtGetTSVPtr ( hHandle , &pLocalStorage ) == 0 )
		{
			if ( pLocalStorage->tLastError.error )
				StdError = pLocalStorage->tLastError;
	
			CmtReleaseTSVPtr ( hHandle );
		}
	}
	   
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** STD Driver Unplug ***** ***** ***** ***** ***** ***** ***** *****/	
void*   DLLEXPORT   STD_Driver_Unplug( int hHandle , double lfTimeOut )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));   

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** STD Driver Plug In ***** ***** ***** ***** ***** ***** ***** *****/	
void*   DLLEXPORT   STD_Driver_PlugIn( int hHandle , double lfTimeOut )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));	 

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** STD Drive Enable Plug In Automatically ***** ***** ***** ***** ***** ***** ***** *****/	 
void*   DLLEXPORT   STD_Driver_EnablePlugInAutomatically( int hHandle , int bEnable )
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

/***** ***** ***** ***** ***** ***** ***** ***** STD Driver Install Receive Callback Function ***** ***** ***** ***** ***** ***** ***** *****/	     
void*   DLLEXPORT   STD_Driver_InstallReceiveCallbackFunction( int hHandle , tfStdReceiveCallback pfFunction , void *pData )
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

/***** ***** ***** ***** ***** ***** ***** ***** STD Driver Load Config File ***** ***** ***** ***** ***** ***** ***** *****/	
void*   DLLEXPORT   STD_Driver_LoadConfigFile( int hHandle , char *pFileName )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			

	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};
	
	int							hFileHandle											=	0;
				
	int							iIndex												=	0,  
								iCount												=	0;
	
	char						szFormatedLog[LOW_STRING]							=	{0};

	int							bGetSafeVariable									=	0;
	
	IF (( hHandle == 0 ) , "handle can't be zero." );
	
	IF (( pFileName == NULL ) , "File name is empty." );
	IF (( FileExists( pFileName , NULL ) == 0 )  , "File was not found." );  
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));  
	bGetSafeVariable = 1;
	
	STDF_UPDATE_CALLBACK_DATA(pLocalStorage->ptCallbacks); 
	
	sprintf( szFormatedLog , "%s :: Loading configuration %%s" ,  pLocalStorage->ptCallbacks->szName );
	STDF_COMMENT(0,szFormatedLog,"On Slot N%d");
	
	hFileHandle = OpenFile ( pFileName , VAL_READ_ONLY, VAL_TRUNCATE, VAL_BINARY);
	
	FREE_CALLOC_ERR( pLocalStorage->pSTD_CommandList , 2048 , sizeof(tsSTD_CommandItem));
	
	for ( iIndex = 0 ; iIndex < 2048; iIndex++ )
	{
		iCount = ReadFile ( hFileHandle , pLocalStorage->pSTD_CommandList[iIndex].max_size , sizeof(tsSTD_CommandItem) );
		
		if ( iCount != sizeof(tsSTD_CommandItem) )
			break;
	}
	
	pLocalStorage->listSize = iIndex;

Error:

	if ( hFileHandle )
		CloseFile (hFileHandle);
	
	if ( hHandle && bGetSafeVariable )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** STD Driver Load Config Buffer ***** ***** ***** ***** ***** ***** ***** *****/	 
void*   DLLEXPORT   STD_Driver_LoadConfigBuffer( int hHandle , char *pBuffer , int iBufferSize )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			

	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0};

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
		
	FREE_CALLOC_ERR( pLocalStorage->pSTD_CommandList , 2048 , sizeof(tsSTD_CommandItem));
	
	iCount = iBufferSize;
	iBaseSize = sizeof(tsSTD_CommandItem); 
	
	for ( iIndex = 0 ; iIndex < 2048; iIndex++ )
	{
		if ( iCount < iBaseSize )   
			break;
		
		memcpy( pLocalStorage->pSTD_CommandList[iIndex].max_size , pBuffer , iBaseSize );
		pBuffer += iBaseSize;
		iCount -= iBaseSize; 
	}
	
	pLocalStorage->listSize = iIndex;

Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** STD Driver Check Connection ***** ***** ***** ***** ***** ***** ***** *****/	 
void*   DLLEXPORT   STD_Driver_Check_Connection( int hHandle , char *pCommandName , int *piStatus )
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			
																			
	int							iCommandIndex										=	0,
								bCommandFound										=	0; 
									
	IF (( hHandle == 0 ) , "handle can't be zero." );
	IF (( piStatus == 0 ) , "Status parameter can't be NULL." );
	
	*piStatus = 0;
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	for ( iCommandIndex = 0 ; iCommandIndex < pLocalStorage->listSize ; iCommandIndex++ )
		if ((strlen(pLocalStorage->pSTD_CommandList[iCommandIndex].szCommandName)) && ( strcmp( pCommandName , pLocalStorage->pSTD_CommandList[iCommandIndex].szCommandName ) == 0 ))
		{
			bCommandFound = 1;
			break;
		}
		
	IF (( bCommandFound == 0 ) , "Command was not found.");   

Error:

	if ( piStatus )
	{
		*piStatus = bCommandFound;	
	}
	
	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}

/***** ***** ***** ***** ***** ***** ***** ***** STD Driver Get Commands_List ***** ***** ***** ***** ***** ***** ***** *****/	
void*	DLLEXPORT		STD_Driver_Get_Commands_List( int hHandle , char ***pCommandsList , int *piNumberOfCommands )     
{
	STD_ERROR					StdError											=	{0};
	
	tsLocalStorage				*pLocalStorage										=	NULL;			

	int							iIndex												=	0;

	int							iNumberOfCommands									=	0;
	
	IF (( hHandle == 0 ) , "handle can't be zero." );
	IF (( pCommandsList == NULL ) , "Pointer to List is empty." );
		
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));
	
	CALLOC_ERR( (*pCommandsList) , pLocalStorage->listSize+1 , sizeof(char*));
	
	for ( iIndex = 0 ; iIndex < pLocalStorage->listSize; iIndex++ )
	{
		CALLOC_COPY_STRING_ERR( (*pCommandsList)[iIndex] , pLocalStorage->pSTD_CommandList[iIndex].szCommandName );
		 
		iNumberOfCommands++;
	}

Error:

	if ( piNumberOfCommands )
		*piNumberOfCommands = iNumberOfCommands;
		
	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}



//*******************************************************
//Auto reconnect

void*  AutoReconnect( int hHandle )
{
	STD_ERROR					StdError											=	{0};
	tsSTD_CallBackSet			tSTD_CallBackSet									=	{0}; 
	
	tsLocalStorage				*pLocalStorage										=	NULL;
	
	char						szLogFormated[LOW_STRING]							=	{0};
	
	double						lfAutoPlugInRetryDelay								=	0.0;
	
	int 						hFileHandle											=	0;
	
	double						lfLogCurrentTime									=	0;  
										

	
	IF (( hHandle == 0 ) , "handle can't be zero." );		//handle for tread
	
	CHK_CMT( CmtGetTSVPtr ( hHandle , &pLocalStorage ));	// get LocalStorage for this tread
	
	lfAutoPlugInRetryDelay	=	pLocalStorage->lfAutoPlugInRetryDelay;
	
	hFileHandle   =  pLocalStorage->hFileHandle;	
	
	//log
	GetCurrentDateTime( &lfLogCurrentTime ); 
	
	FormatDateTimeString ( lfLogCurrentTime , "\r\n\r\n%H:%M:%S.%3f    Auto Reconnection        :    \r\n" , szLogFormated , (LOW_STRING-1) );

	if (pLocalStorage->bEnableAutoPlugIn)
	{
		
		WriteFile ( hFileHandle , szLogFormated , strlen(szLogFormated) ); 
		
		if (viClose ( pLocalStorage->hCommunicationHandle )==VI_SUCCESS) //Close session
		{	
			sprintf( szLogFormated , "%s :: Close communication %%s" ,  pLocalStorage->ptCallbacks->szName ); //log
			STDF_COMMENT(0,szLogFormated,"On Slot N%d");   //print in logger window
			strcpy(szLogFormated  ,  "Close communication \r\n");
			WriteFile ( hFileHandle , szLogFormated  ,  strlen(szLogFormated) );
		}
		
		else
		{
			sprintf( szLogFormated , "%s :: Failed to close communication  %%s" ,  pLocalStorage->ptCallbacks->szName ); //log
			STDF_COMMENT(0,szLogFormated,"On Slot N%d");   //print in logger window	
			strcpy(szLogFormated  ,  "Failed to close communication \r\n");  
			WriteFile ( hFileHandle , szLogFormated  ,  strlen(szLogFormated) );
		}
		
		
		for (int i=0; i<pLocalStorage->iAutoPlugInRetryNum; i++)
		{
			DelayWithEventProcessing ( lfAutoPlugInRetryDelay );	//when power down we need delay  
		
			if(viOpen ( pLocalStorage->viOpenDefaultRM , pLocalStorage->pszAddress , VI_NULL, VI_NULL, &pLocalStorage->hCommunicationHandle )==VI_SUCCESS)   //Open session
			{
				sprintf( szLogFormated , "Initializing \"%s\" Success %%s" ,  pLocalStorage->ptCallbacks->szName );
				STDF_COMMENT(0,szLogFormated,"On Slot N%d");
				strcpy(szLogFormated  ,  "Initializing  Success \r\n");  
				WriteFile ( hFileHandle , szLogFormated  ,  strlen(szLogFormated) ); 
				break;
			}
			
			else
			{
				sprintf( szLogFormated , "Initializing \"%s\" Fail %%s" ,   pLocalStorage->ptCallbacks->szName );
				STDF_COMMENT(0,szLogFormated,"On Slot N%d");
				strcpy(szLogFormated  ,  "Initializing  Fail \r\n");
				WriteFile ( hFileHandle , szLogFormated  ,  strlen(szLogFormated) );
			}
		}
		
	}
	else
	{
		WriteFile ( hFileHandle , szLogFormated , strlen(szLogFormated) ); 
	 	sprintf( szLogFormated , "Communication \"%s\" Lost %%s" ,  pLocalStorage->ptCallbacks->szName ); 		  
		STDF_COMMENT(0,szLogFormated,"On Slot N%d");  
		WriteFile ( hFileHandle , szLogFormated , strlen(szLogFormated) ); 
	}
  
  Error:

	if ( hHandle )
		CmtReleaseTSVPtr ( hHandle );  
			
	RETURN_STDERR_POINTER;
}









