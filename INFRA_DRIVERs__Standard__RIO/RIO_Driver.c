//==============================================================================
// Include files 

#include "Windows.h"
#include <visa.h>  
#include <tcpsupp.h>   
#include "cvidef.h"  
#include "toolbox.h"
#include <userint.h>
#include <ansi_c.h>
#include <formatio.h> 
#include <utility.h>
#include "GlobalDefines.h" 

//==============================================================================
// Constants

#define							PORT_NAME									47202
#define							CONNECT_TIMEOUT     					    30000
#define							WRITE_TIMEOUT     					        5000  
#define							READ_TIMEOUT     					        10000 
#define							READ_WRITE_TIMEOUT    					    5000 
#define							MAX_PACKET_SIZE     					    256
#define							SEND_TERMINATION     					    ";"
#define							RECEIVE_LONG_TERMINATION				    "\r\n:" 
#define							RECEIVE_SHORT_TERMINATION				    ":" 

#define							NUMBER_OF_DO_LINES 					        16
#define							NUMBER_OF_DI_LINES							16 
#define							NUMBER_OF_AI_LINES 					        8
#define							NUMBER_OF_AO_LINES							8

#define                         CTRL(x) (#x[0]-'a'+1)
//==============================================================================
// Types
typedef enum 
{
	
	MINUS_5_PLUS_5 = 1,
	MINUS_10_PLUS_10 = 2,
	ZERO_PLUS_5 = 3,
	ZERO_PLUS_10 = 4
				
} teAI_Range; 

typedef union
{
	unsigned char ucPortConfigs[2];
	
	unsigned short usTotalConfig; 
	
}tuDI_Config;

typedef struct
{
	unsigned int		conversationHandle,
				        connected;
	
	unsigned char		DO_State[NUMBER_OF_DO_LINES];
	
	unsigned char		DI_State[NUMBER_OF_DI_LINES]; 
	
	unsigned char		AO_State[NUMBER_OF_AO_LINES];
	
	unsigned char		AI_State[NUMBER_OF_AI_LINES];   
	
} tsHandle;

//==============================================================================
// Static global variables 

static volatile int			                g_IsConnected                    =  	0;  

static volatile int			                g_TCPError                       =  	0;

static volatile unsigned char               g_CanRead                        =  	0; 

static volatile unsigned char               g_IsCommandSuccessful            =  	0; 


//==============================================================================
// Function Protoypes

void*  RIO_Get_Error_Text_Message ( int hConversationHandle , int iError , char *pErrorMessage ); 

void*  RIO_Get_Device_Info ( int hInstrumentHandle, char **pModel , char **pFirmware ); 

int SendReceiveData(unsigned int conversationHandle, char *pSendBuffer, char *pReceiveBuffer);  

int CVICALLBACK TCPCallback (unsigned handle, int event, int error, void *callbackData); 

int VerifyReplay(char *pReadBuffer); 

int ParseReplay(char *pReadBuffer, char *pResultBuffer);  

int SplitStringByDelimiter( char * str, char delim, char ***array, int *length );

void SetBit(unsigned char *ucByte, unsigned char ucBitNumber, unsigned char ucBitValue);

void ToggleBit(unsigned char *ucByte, unsigned char ucBitNumber );

void GetBit(unsigned char ucByte, unsigned char ucBitNumber, unsigned char *ucBitValue); 

//==============================================================================
// Global functions


/***** ***** ***** ***** ***** ***** ***** ***** RIO Get Low Level Handle   ***** ***** ***** ***** ***** ***** ***** *****/ 
void* RIO_Get_Low_Level_Handle( int hHandle , int *phLowLevelHandle )
{
	STD_ERROR						StdError										=	{0};

	tsHandle						*pLocalHandle									=	{0};
	
	if ( phLowLevelHandle )
	{
		if ( CmtGetTSVPtr ( hHandle , &pLocalHandle ) == 0 )
		{
			*phLowLevelHandle = pLocalHandle->conversationHandle; 
			
			CmtReleaseTSVPtr ( hHandle ); 
		}	
	}
	
	RETURN_STDERR_POINTER;	
}


/***** ***** ***** ***** ***** ***** ***** ***** RIO Get Error Text Message Low Level Handle  ***** ***** ***** ***** ***** ***** ***** *****/  
void*  RIO_Get_Error_Text_Message_Low_Level_Handle( int hLowLevelHandle , int iError , char *pErrorMessage )
{
	STD_ERROR		StdError							=	{0}; 

	int				hConversationHandle					=	0;  
	
	
	IF (( hLowLevelHandle < 0 ) , "Handle is empty" );
	
	hConversationHandle =  hLowLevelHandle;
	
Error:
	
	return RIO_Get_Error_Text_Message ( hConversationHandle , StdError.error, NULL );  	
}

/***** ***** ***** ***** ***** ***** ***** ***** RIO Get Error Text Instrument Handle  ***** ***** ***** ***** ***** ***** ***** *****/  
void*  RIO_Get_Error_Text_Message_Instrument_Handle( int hInstrumentHandle , int iError , char *pErrorMessage )
{
	
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0; 
	
	int				hConversationHandle					=   0;
	
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle )); 
	
	hConversationHandle =  pLocalHandle->conversationHandle;
	
	IF (( hConversationHandle < 0 ) , "Conversation Handle is empty" );
	
Error:

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return RIO_Get_Error_Text_Message ( hConversationHandle , StdError.error, NULL );  	
}
 
/***** ***** ***** ***** ***** ***** ***** ***** RIO Get Error TextMessage  ***** ***** ***** ***** ***** ***** ***** *****/  
void*  RIO_Get_Error_Text_Message ( int hConversationHandle , int iError , char *pErrorMessage )
{
	STD_ERROR		StdError							=	{0}; 
	
	char			szErrorDescription[1024]		    =	{0};
	
	char            szWriteBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	char            szReadBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	char            szResultBuffer[MAX_PACKET_SIZE]		=	{0};
	
	char            **pCharArray                        = 	NULL;  
	
	int 			iStatus					            =   0;
	
	int				iCount								=   0; 
	
	int				iIndex								=   0;  
	
	
	if( g_TCPError < 0 )                //  TCP error
	{
		iError = g_TCPError;  
		sprintf(szErrorDescription, "TCP library error message: %s\nSystem error message: %s", GetTCPErrorString (g_TCPError), GetTCPSystemErrorString());
	}
	else				                // No TCP error
	{
		if(g_IsCommandSuccessful < 1)   // Driver Command Error
		{ 
	    	
			sprintf( szWriteBuffer , "TC 1%s", SEND_TERMINATION ); 

			iStatus =  SendReceiveData(hConversationHandle, szWriteBuffer, &szReadBuffer); 
			
			if( iStatus > 0)
			{
				iStatus = ParseReplay(szReadBuffer, szResultBuffer);
				 
				 if( iStatus > 0)
				 {
			     	iStatus = SplitStringByDelimiter (szResultBuffer, ' ', &pCharArray, &iCount);
					   
					if( iCount > 1)
				    {
						RemoveSurroundingWhiteSpace(pCharArray[0]); 
						
						iError = atoi(pCharArray[0]);
						
						for (iIndex = 1; iIndex < iCount; iIndex++)
						{
							if(iIndex == 1)
						 	{
								RemoveSurroundingWhiteSpace(pCharArray[iIndex]);
								strcpy( szErrorDescription, pCharArray[iIndex] ); 
						 	}
							else
							{
								strcat(szErrorDescription , " ");
								strcat(szErrorDescription , pCharArray[iIndex]);
							} 
						} 
					}
					else
					{
						iError = -1;
						strcpy( szErrorDescription , "Unknown Error"); 
					}
				 }
				 else
				 {
					iError = -1;
					strcpy( szErrorDescription , "Unknown Error"); 
				 }
			}
			else
			{
				iError = -1;
				strcpy( szErrorDescription , "Unknown Error"); 
			}
			
		}
		else                            // No Command Error 
		{
			iError = 0;
			strcpy( szErrorDescription , "No Error"); 
		}
	}
	
	if ( pErrorMessage )			   // Up the error
	{
		strcpy( pErrorMessage , szErrorDescription ); 
	} 
	
	StdError.error = iError; 
	
	if ( iError != 0 )                 // Set Error
	{
		CALLOC_COPY_STRING( StdError.pszErrorDescription , szErrorDescription );
	}
	
Error:  
	
	FREE(pCharArray);
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** ***** ***** Get Device Info Low Level Handle ***** ***** ***** ***** ***** ***** ***** *****/		
void*  RIO_Get_Device_Info_Low_Level_Handle ( int hLowLevelHandle, char **pModel , char **pFirmware )
{
	STD_ERROR		StdError							=	{0}; 

	int				hConversationHandle					=	0;  
	
	
	IF (( hLowLevelHandle < 0 ) , "Handle is empty" );
	
	hConversationHandle =  hLowLevelHandle;

Error:
	
	return RIO_Get_Device_Info ( hConversationHandle, pModel, pFirmware );  
}

/***** ***** ***** ***** ***** ***** ***** ***** Get Device Info Instrument Handle ***** ***** ***** ***** ***** ***** ***** *****/		
void*  RIO_Get_Device_Info_Instrument_Handle ( int hInstrumentHandle, char **pModel , char **pFirmware )
{
	
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0; 
	
	int				hConversationHandle					=   0;  
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle )); 
	
	hConversationHandle =  pLocalHandle->conversationHandle;
	
	IF (( hConversationHandle < 0 ) , "Conversation Handle is empty" );

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle ); 
	
	return RIO_Get_Device_Info ( hConversationHandle, pModel, pFirmware );   
} 

/***** ***** ***** ***** ***** ***** ***** ***** Get Device Info ***** ***** ***** ***** ***** ***** ***** *****/		
void*  RIO_Get_Device_Info ( int hConversationHandle, char **pModel , char **pFirmware )
{
	STD_ERROR		StdError							=	{0};
	
	char            szWriteBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	char            szReadBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	char            szResultBuffer[MAX_PACKET_SIZE]		=	{0};
	
	char            **pCharArray                        = 	NULL;  
	
	int 			iStatus					            =   0;
	
	int				iCount								=   0;
	
	
	sprintf( szWriteBuffer , "%c%c\%s", CTRL(r), CTRL(v), SEND_TERMINATION ); 
	
	iStatus =  SendReceiveData(hConversationHandle, szWriteBuffer, &szReadBuffer); 
	
	if(iStatus > 0)
	{
		iStatus = ParseReplay(szReadBuffer, &szResultBuffer); 
		
		if(iStatus > 0)
		{
			
			iStatus = SplitStringByDelimiter (szResultBuffer, ' ', &pCharArray, &iCount); 
			
			if(iCount > 2)
			{
				RemoveSurroundingWhiteSpace(pCharArray[0]);
				iCount = strlen(pCharArray[0]);
				*pModel  = malloc(iCount + 1);
				*pModel = pCharArray[0];  
				
				RemoveSurroundingWhiteSpace(pCharArray[2]);
				iCount = strlen(pCharArray[2]);
				*pFirmware  = malloc(iCount + 1);
				*pFirmware = pCharArray[2];
				
				g_IsCommandSuccessful = 1;  
			}
			
		}
	    else
		{
		   g_IsCommandSuccessful = 0; 
		}
	}

Error: 
	
	FREE(pCharArray);   
	
	return RIO_Get_Error_Text_Message(hConversationHandle, StdError.error, NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Init ***** ***** ***** ***** ***** ***** ***** *****/
void*  RIO_Init ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *vhInstrumentHandle )
{
	STD_ERROR						StdError										=	{0};
	
	tsHandle						*pLocalHandle									=	{0};
	
	CmtTSVHandle 					handle											=	0;
	
	int 					        iStatus							                =   0; 
	
	char                            **pCharArray                                    = 	NULL; 
	
	char			                szErrorDescription[1024]		                =	{0};
	
	
	IF (( szRsrcAddress == NULL ) , "Address string is empty" ); 
	
	
	if ( hParentInstrumentHandle == 0 ) 
	{
		CHK_CMT ( CmtNewTSV ( sizeof(tsHandle) , &handle ));
	}
	else
		handle = hParentInstrumentHandle;
		
	if ( vhInstrumentHandle )
		*vhInstrumentHandle = handle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	g_IsConnected = 0;

	if ( pLocalHandle->connected < 1 )
	{
		// Open Connection
		iStatus = ConnectToTCPServer (&pLocalHandle->conversationHandle, PORT_NAME, szRsrcAddress, TCPCallback, NULL, CONNECT_TIMEOUT);
		
		if(iStatus > -1)
		{
			pLocalHandle->connected = 1;
			
			g_IsConnected = 1;
		}
		else
		{
			g_TCPError = iStatus;
			
			StdError.error = g_TCPError;  
	        sprintf(szErrorDescription, "TCP library error message: %s\nSystem error message: %s", GetTCPErrorString (g_TCPError), GetTCPSystemErrorString());
			CALLOC_COPY_STRING( StdError.pszErrorDescription , szErrorDescription ); 
		} 
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	FREE(pCharArray);   
	
	RETURN_STDERR_POINTER;
}


/***** ***** ***** ***** ***** ***** ***** ***** Close ***** ***** ***** ***** ***** ***** ***** *****/ 	
void*  RIO_Close ( int *vhInstrumentHandle )
{
	STD_ERROR		StdError						=	{0};
	
	tsHandle		*pLocalHandle					=	{0};

	int				handle							=	0;
	
	int				status							=	-1; 
	
	IF (( vhInstrumentHandle == NULL ) , "Handle is empty" );
	
	handle = *vhInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if( g_IsConnected == 1)
	{
		status = DisconnectFromTCPServer(pLocalHandle->conversationHandle);
	
		if(status > -1)
		{
			pLocalHandle->connected = 0;
			
			g_IsConnected = 0;
		}
		else
		{
			g_TCPError = status;	
		}
	}
	
	CmtReleaseTSVPtr ( handle ); 
	
	CmtDiscardTSV (handle);
	
	*vhInstrumentHandle = 0;
	
Error:

	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** ***** ***** Reset ***** ***** ***** ***** ***** ***** ***** *****/		
void*  RIO_Reset ( int hInstrumentHandle )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char            szWriteBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	char            szReadBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	int 			iStatus					            =   0;
	
	
	g_IsCommandSuccessful = 0; 
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szWriteBuffer , "RS 0%s", SEND_TERMINATION ); 
	
	iStatus =  SendReceiveData(pLocalHandle->conversationHandle, szWriteBuffer, &szReadBuffer); 
	
	if(iStatus > 0)
	{
		iStatus = VerifyReplay(szReadBuffer);
	
		if(iStatus == 1)
			g_IsCommandSuccessful = 1;
	}

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return RIO_Get_Error_Text_Message(pLocalHandle->conversationHandle, StdError.error, NULL);
}



/***** ***** ***** ***** ***** ***** ***** ***** Get AI Line Value ***** ***** ***** ***** ***** ***** ***** *****/
void*  RIO_Get_AI_Line_Value ( int hInstrumentHandle, unsigned char ucLineNumber, double *dResult )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char            szWriteBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	char            szReadBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	char            szResultBuffer[MAX_PACKET_SIZE]		=	{0};
	
	int 			iStatus					            =   0; 
	
	
	if(ucLineNumber >= NUMBER_OF_AI_LINES )
					ucLineNumber = NUMBER_OF_AI_LINES - 1;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szWriteBuffer , "MG @AN[%d]%s", ucLineNumber, SEND_TERMINATION ); 
	
	iStatus =  SendReceiveData(pLocalHandle->conversationHandle, szWriteBuffer, &szReadBuffer); 
	
	if(iStatus > 0)
	{
		iStatus = ParseReplay(szReadBuffer, &szResultBuffer); 
		
		if(iStatus > 0)
		{
			RemoveSurroundingWhiteSpace(szResultBuffer); 
			
			*dResult =  atof(szResultBuffer); 
			
			g_IsCommandSuccessful = 1;  
		}
	    else
		{
		   g_IsCommandSuccessful = 0; 
		}
	}

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return RIO_Get_Error_Text_Message(pLocalHandle->conversationHandle, StdError.error, NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Get AO Line Value ***** ***** ***** ***** ***** ***** ***** *****/
void*  RIO_Get_AO_Line_Value ( int hInstrumentHandle, unsigned char ucLineNumber, double *dResult )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char            szWriteBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	char            szReadBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	char            szResultBuffer[MAX_PACKET_SIZE]		=	{0};
	
	int 			iStatus					            =   0; 
	
	
	if(ucLineNumber >= NUMBER_OF_AO_LINES )
					ucLineNumber = NUMBER_OF_AO_LINES - 1;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szWriteBuffer , "MG @AO[%d]%s", ucLineNumber, SEND_TERMINATION ); 
	
	iStatus =  SendReceiveData(pLocalHandle->conversationHandle, szWriteBuffer, &szReadBuffer); 
	
	if(iStatus > 0)
	{
		iStatus = ParseReplay(szReadBuffer, &szResultBuffer); 
		
		if(iStatus > 0)
		{
			RemoveSurroundingWhiteSpace(szResultBuffer); 
			
			*dResult =  atof(szResultBuffer); 
			
			g_IsCommandSuccessful = 1;  
		}
	    else
		{
		   g_IsCommandSuccessful = 0; 
		   
		}
	}

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return RIO_Get_Error_Text_Message(pLocalHandle->conversationHandle, StdError.error, NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Get DI Line Value ***** ***** ***** ***** ***** ***** ***** *****/
void*  RIO_Get_DI_Line_Value ( int hInstrumentHandle, unsigned char ucLineNumber, unsigned char *ucResult )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char            szWriteBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	char            szReadBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	char            szResultBuffer[MAX_PACKET_SIZE]		=	{0};
	
	int 			iStatus					            =   0; 
	
	
	if(ucLineNumber >= NUMBER_OF_DI_LINES )
					ucLineNumber = NUMBER_OF_DI_LINES - 1;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szWriteBuffer , "MG @IN[%d]%s", ucLineNumber, SEND_TERMINATION ); 
	
	iStatus =  SendReceiveData(pLocalHandle->conversationHandle, szWriteBuffer, &szReadBuffer); 
	
	if(iStatus > 0)
	{
		iStatus = ParseReplay(szReadBuffer, &szResultBuffer); 
		
		if(iStatus > 0)
		{
			RemoveSurroundingWhiteSpace(szResultBuffer); 
			
			*ucResult =  atoi(szResultBuffer); 
			
			g_IsCommandSuccessful = 1;  
		}
	    else
		{
		   g_IsCommandSuccessful = 0; 
		   
		}
	}

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return RIO_Get_Error_Text_Message(pLocalHandle->conversationHandle, StdError.error, NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Get DO Line Value ***** ***** ***** ***** ***** ***** ***** *****/
void*  RIO_Get_DO_Line_Value ( int hInstrumentHandle, unsigned char ucLineNumber, unsigned char *ucResult )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char            szWriteBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	char            szReadBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	char            szResultBuffer[MAX_PACKET_SIZE]		=	{0};
	
	int 			iStatus					            =   0; 
	
	
	if(ucLineNumber >= NUMBER_OF_DO_LINES )
					ucLineNumber = NUMBER_OF_DO_LINES - 1;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szWriteBuffer , "MG @OUT[%d]%s", ucLineNumber, SEND_TERMINATION ); 
	
	iStatus =  SendReceiveData(pLocalHandle->conversationHandle, szWriteBuffer, &szReadBuffer); 
	
	if(iStatus > 0)
	{
		iStatus = ParseReplay(szReadBuffer, &szResultBuffer); 
		
		if(iStatus > 0)
		{
			RemoveSurroundingWhiteSpace(szResultBuffer); 
			
			*ucResult =  atoi(szResultBuffer); 
			
			g_IsCommandSuccessful = 1;  
		}
	    else
		{
		   g_IsCommandSuccessful = 0; 
		   
		}
	}

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return RIO_Get_Error_Text_Message(pLocalHandle->conversationHandle, StdError.error, NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Set AO Line Value ***** ***** ***** ***** ***** ***** ***** *****/
void*  RIO_Set_AO_Line_Value ( int hInstrumentHandle, unsigned char ucLineNumber, double dValue )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char            szWriteBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	char            szReadBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	int 			iStatus					            =   0; 
	
	
	if(ucLineNumber >= NUMBER_OF_AO_LINES )
					ucLineNumber = NUMBER_OF_AO_LINES - 1;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szWriteBuffer , "AO %d,%0.3lf%s", ucLineNumber, dValue, SEND_TERMINATION ); 
	
	iStatus =  SendReceiveData(pLocalHandle->conversationHandle, szWriteBuffer, &szReadBuffer); 
	
	if(iStatus > 0)
	{
		iStatus = VerifyReplay(szReadBuffer); 
		
		if(iStatus > 0)
		{
			g_IsCommandSuccessful = 1;  
		}
	    else
		{
		   g_IsCommandSuccessful = 0; 
		   
		}
	}

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return RIO_Get_Error_Text_Message(pLocalHandle->conversationHandle, StdError.error, NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Set DO Line Value ***** ***** ***** ***** ***** ***** ***** *****/
void*  RIO_Set_DO_Line_Value ( int hInstrumentHandle, unsigned char ucLineNumber, unsigned char ucValue )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char            szWriteBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	char            szReadBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	int 			iStatus					            =   0; 
	
	
	if(ucLineNumber >= NUMBER_OF_DO_LINES )
					ucLineNumber = NUMBER_OF_DO_LINES - 1;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	if(ucValue == 0)
	{
		sprintf( szWriteBuffer , "CB %d%s", ucLineNumber, SEND_TERMINATION ); 
	}
	else
	{
		sprintf( szWriteBuffer , "SB %d%s", ucLineNumber, SEND_TERMINATION );  	
	}
	
	iStatus =  SendReceiveData(pLocalHandle->conversationHandle, szWriteBuffer, &szReadBuffer); 
	
	if(iStatus > 0)
	{
		iStatus = VerifyReplay(szReadBuffer); 
		
		if(iStatus > 0)
		{
			g_IsCommandSuccessful = 1;  
		}
	    else
		{
		   g_IsCommandSuccessful = 0; 
		   
		}
	}

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return RIO_Get_Error_Text_Message(pLocalHandle->conversationHandle, StdError.error, NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Get AI Line Config ***** ***** ***** ***** ***** ***** ***** *****/
void*  RIO_Get_AI_Line_Config ( int hInstrumentHandle, unsigned char ucLineNumber, unsigned char *ucResult )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char            szWriteBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	char            szReadBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	char            szResultBuffer[MAX_PACKET_SIZE]		=	{0};
	
	int 			iStatus					            =   0; 
	
	
	if(ucLineNumber >= NUMBER_OF_AI_LINES )
					ucLineNumber = NUMBER_OF_AI_LINES - 1;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf( szWriteBuffer , "MG _AQ%d%s", ucLineNumber, SEND_TERMINATION ); 
	
	iStatus =  SendReceiveData(pLocalHandle->conversationHandle, szWriteBuffer, &szReadBuffer); 
	
	if(iStatus > 0)
	{
		iStatus = ParseReplay(szReadBuffer, &szResultBuffer); 
		
		if(iStatus > 0)
		{
			RemoveSurroundingWhiteSpace(szResultBuffer); 
			
			*ucResult =  atoi(szResultBuffer); 
			
			g_IsCommandSuccessful = 1;  
		}
	    else
		{
		   g_IsCommandSuccessful = 0; 
		}
	}

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return RIO_Get_Error_Text_Message(pLocalHandle->conversationHandle, StdError.error, NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Get DI Line Config ***** ***** ***** ***** ***** ***** ***** *****/
void*  RIO_Get_DI_Line_Config ( int hInstrumentHandle, unsigned char ucLineNumber, unsigned char *ucResult )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char            szWriteBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	char            szReadBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	char            szResultBuffer[MAX_PACKET_SIZE]		=	{0};
	
	int 			iStatus					            =   0;
	
	tuDI_Config		DI_Config 							=	{0}; 
	
	unsigned char	ucLineConfig			            =   0;	 
	
	
	if(ucLineNumber >= NUMBER_OF_DI_LINES )
					ucLineNumber = NUMBER_OF_DI_LINES - 1;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	sprintf ( szWriteBuffer , "MG _IQ0%s", SEND_TERMINATION ); 
	
	iStatus =  SendReceiveData(pLocalHandle->conversationHandle, szWriteBuffer, &szReadBuffer); 
	
	if(iStatus > 0)
	{
		iStatus = ParseReplay(szReadBuffer, &szResultBuffer); 
		
		if(iStatus > 0)
		{
			RemoveSurroundingWhiteSpace(szResultBuffer); 
			
			DI_Config.usTotalConfig =  (unsigned short)atoi(szResultBuffer);
			 
			
			if(ucLineNumber > 7 )
			{
				GetBit(DI_Config.ucPortConfigs[1], ucLineNumber - 8, &ucLineConfig);
			}
			else
			{
				GetBit(DI_Config.ucPortConfigs[0], ucLineNumber, &ucLineConfig);  
			}
			
			*ucResult =  ucLineConfig;
			
			g_IsCommandSuccessful = 1;  
		}
	    else
		{
		   g_IsCommandSuccessful = 0; 
		}
	}

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return RIO_Get_Error_Text_Message(pLocalHandle->conversationHandle, StdError.error, NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Set AI Line Config ***** ***** ***** ***** ***** ***** ***** *****/
void*  RIO_Set_AI_Line_Config ( int hInstrumentHandle, unsigned char ucLineNumber, unsigned char ucRangeType, unsigned char ucIsDifferential)
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char            szWriteBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	char            szReadBuffer[MAX_PACKET_SIZE]		=	{0};
	
	int 			iStatus					            =   0; 
	
	unsigned char   ucUnitRangeType						=   0;
	
	unsigned char   ucIsEvenNumber				    	=   0; 
	
	
	if(ucLineNumber >= NUMBER_OF_AI_LINES )
		ucLineNumber = NUMBER_OF_AI_LINES - 1;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	switch (ucRangeType)
	{
		case MINUS_5_PLUS_5:
		case MINUS_10_PLUS_10:
        case ZERO_PLUS_5:
        case ZERO_PLUS_10:
			
			ucUnitRangeType  = ucRangeType;
			
			break; 
			
		default:
			
			ucUnitRangeType  = (unsigned char)MINUS_10_PLUS_10;
			
			break;
	}
	
	if(ucIsDifferential == 0)
	{
		sprintf( szWriteBuffer , "AQ %d,%d%s", ucLineNumber, ucUnitRangeType, SEND_TERMINATION ); 
	}
	else
	{
		GetBit(ucLineNumber, 0, &ucIsEvenNumber);
		
		if(ucIsEvenNumber == 1)
		{
			 sprintf( szWriteBuffer , "AQ %d,%d%s", ucLineNumber, ucUnitRangeType, SEND_TERMINATION );   
		}
		else
		{
			sprintf( szWriteBuffer , "AQ %d,-%d%s", ucLineNumber, ucUnitRangeType, SEND_TERMINATION ); 
		}
	}
	
	iStatus =  SendReceiveData(pLocalHandle->conversationHandle, szWriteBuffer, &szReadBuffer); 
	
	if(iStatus > 0)
	{
		iStatus = VerifyReplay(szReadBuffer); 
		
		if(iStatus > 0)
		{
			g_IsCommandSuccessful = 1;  
		}
	    else
		{
		   g_IsCommandSuccessful = 0; 
		}
	}

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return RIO_Get_Error_Text_Message(pLocalHandle->conversationHandle, StdError.error, NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Set DI Line Config ***** ***** ***** ***** ***** ***** ***** *****/
void*  RIO_Set_DI_Line_Config ( int hInstrumentHandle, unsigned char ucLineNumber, unsigned char ucIsInvert )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char            szWriteBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	char            szReadBuffer[MAX_PACKET_SIZE]		=	{0}; 
	
	char            szResultBuffer[MAX_PACKET_SIZE]		=	{0};
	
	int 			iStatus					            =   0;
	
	tuDI_Config		DI_Config 							=	{0};
	
	
	g_IsCommandSuccessful = 0;
	
	if(ucLineNumber >= NUMBER_OF_DI_LINES )
					ucLineNumber = NUMBER_OF_DI_LINES - 1;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle )); 
	
	
	sprintf ( szWriteBuffer , "MG _IQ0%s", SEND_TERMINATION ); 
	
	iStatus =  SendReceiveData(pLocalHandle->conversationHandle, szWriteBuffer, &szReadBuffer); 
	
	if(iStatus > 0)
	{
		iStatus = ParseReplay(szReadBuffer, &szResultBuffer); 
		
		if(iStatus > 0)
		{
			RemoveSurroundingWhiteSpace(szResultBuffer); 
			
			DI_Config.usTotalConfig =  (unsigned short)atoi(szResultBuffer); 
			
			
			if(ucLineNumber > 7 )
			{
				SetBit(&DI_Config.ucPortConfigs[1], ucLineNumber - 8, ucIsInvert);
			}
			else
			{
				SetBit(&DI_Config.ucPortConfigs[0], ucLineNumber, ucIsInvert);  
			}
			
			sprintf ( szWriteBuffer , "IQ %d%s", DI_Config.usTotalConfig, SEND_TERMINATION );
			
			memset(szReadBuffer, 0, sizeof(szReadBuffer));
			
			iStatus =  SendReceiveData(pLocalHandle->conversationHandle, szWriteBuffer, &szReadBuffer); 
			
			if(iStatus > 0)
			{
			
			 	iStatus = VerifyReplay(szReadBuffer);
				
				if(iStatus > 0)
				{
					g_IsCommandSuccessful = 1;  
				}
			}
		}
	}
	

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return RIO_Get_Error_Text_Message(pLocalHandle->conversationHandle, StdError.error, NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Send Receive Data ***** ***** ***** ***** ***** ***** ***** *****/  
int SendReceiveData(unsigned int conversationHandle, char *pSendBuffer, char *pReceiveBuffer)
{
	int 					        iStatus							                =   0; 
	
	int 							iStartTickCounter 								=   0;
	
	int								iCurrentTickCounter								=   0; 
	
	int								iCurrentTimeout									=   0;
	
	int								iCounter										=   0; 
	
	
	g_CanRead = 0;
	g_TCPError = 0;
	
	if(g_IsConnected == 1)
	{
	
		iStartTickCounter =  GetTickCount(); 
	
		iCounter = strlen(pSendBuffer);
		iStatus = ClientTCPWrite (conversationHandle, pSendBuffer,  iCounter , WRITE_TIMEOUT); 
	
		if(iStatus > -1)
		{ 
			while (g_CanRead == 0)
			{
				DelayWithEventProcessing(0.1); 
			
				iCurrentTickCounter = GetTickCount();
				iCurrentTimeout =  iCurrentTickCounter - iStartTickCounter;
			
				if(iCurrentTimeout > READ_TIMEOUT)
				{
					break;
				}
			}
		
			 if(g_CanRead)
			 {
				iCounter = strlen(pReceiveBuffer); 
			 
				iStatus =  ClientTCPRead (conversationHandle, pReceiveBuffer, 256, READ_TIMEOUT); 
		 
				 if(iStatus > -1) 
				 {
					 return 1;
				 }
				 else
				 {
					g_TCPError =  iStatus;
		
					return -1;
			     }  
			 }
			 else
			 {
				  	return -1;
			 } 
		}
		else
		{
			g_TCPError =  iStatus;
		
			return -1;
		} 
	}
	else
	{
		g_TCPError =  -1;
	
		return g_TCPError;
	} 
}

/***** ***** ***** ***** ***** ***** ***** ***** TCP Callback    ***** ***** ***** ***** ***** ***** ***** *****/ 
int CVICALLBACK TCPCallback (unsigned handle, int event, int error, void *callbackData)
{ 

    switch (event)
    {
		case TCP_CONNECT:
			
			g_TCPError = 0;
			
			break;
			
			
        case TCP_DATAREADY:
            
			g_CanRead = 1;
			
            break;
			
        case TCP_DISCONNECT:
			
            g_TCPError = error; 
				
            break;
    }
		   

    return 0;
}

/***** ***** ***** ***** ***** ***** ***** ***** Verify Replay ***** ***** ***** ***** ***** ***** ***** *****/ 
int VerifyReplay(char *pReadBuffer)
{
	int								iIndex											=   -1;
	
	iIndex = FindPattern (pReadBuffer, 0, -1, RECEIVE_LONG_TERMINATION, 0, 0);  
	
	if(iIndex > -1)
	{
		return 1;
	}
	else
	{
		iIndex = FindPattern (pReadBuffer, 0, -1, RECEIVE_SHORT_TERMINATION, 0, 0);  
		
		if(iIndex > -1)
		{
			return 1;
		}
		
	 	return -1;
	}
}

/***** ***** ***** ***** ***** ***** ***** ***** Is Command Successful ***** ***** ***** ***** ***** ***** ***** *****/ 
unsigned char IsCommandSuccessful(void)
{
	return g_IsCommandSuccessful;
}

/***** ***** ***** ***** ***** ***** ***** ***** Parse Replay ***** ***** ***** ***** ***** ***** ***** *****/  
int ParseReplay(char *pReadBuffer, char *pResultBuffer)
{
	int								iIndex											=   0;
	
	iIndex = FindPattern (pReadBuffer, 0, -1, RECEIVE_LONG_TERMINATION, 0, 0);
	
	if(iIndex > 0)
	{
		strncpy(pResultBuffer, pReadBuffer, iIndex); 
		
		return 1;
	}
	else
	{
		iIndex = FindPattern (pReadBuffer, 0, -1, RECEIVE_SHORT_TERMINATION, 0, 0);  
		
		if(iIndex > 0)
		{
			strncpy(pResultBuffer, pReadBuffer, iIndex); 
		
			return 1;
		}
		
	 	return -1;
	}
}

/***** ***** ***** ***** ***** ***** ***** ***** Split String By Delimiter ***** ***** ***** ***** ***** ***** ***** *****/  
int SplitStringByDelimiter( char * str, char delim, char ***array, int *length ) 
{
  char *p;
  char **res;
  int count = 1;
  int index = 0;

  p = str;
  
  // Count occurance of delim in string
  while( (p = strchr(p,delim)) != NULL ) 
  {
    *p = 0; // Null terminate the deliminator.
    p++;    // Skip past our new null
    count++;
  }

  // Allocate dynamic array
  res = calloc( 1, count * sizeof(char *));
  
  if( !res ) 
  	return -1;

  p = str;
  
  for( index =0; index < count; index++ )
  {
    if( *p )
		res[index] = p;
	
	// Copy start of string
    p = strchr(p, 0 );    // Look for next null
	
    p++; // Start of next string
  }

  *array = res;
  *length = count; 

  return 0;
} 

/***** ***** ***** ***** ***** ***** ***** ***** Set Bit ***** ***** ***** ***** ***** ***** ***** *****/
void SetBit(unsigned char *ucByte, unsigned char ucBitNumber, unsigned char ucBitValue)
{
	   if(ucBitNumber > 7)
	   	  ucBitNumber = 7;
	
	
	   if(ucBitValue == 0)
	   {
		    *ucByte &= ~(1 << ucBitNumber);
	   }
	   else
	   {
		   *ucByte |= 1 << ucBitNumber;
	   }
}

/***** ***** ***** ***** ***** ***** ***** ***** Toggle Bit ***** ***** ***** ***** ***** ***** ***** *****/
void ToggleBit(unsigned char *ucByte, unsigned char ucBitNumber )
{
	if(ucBitNumber > 7)
	   	  ucBitNumber = 7;
	
	*ucByte ^= 1 << ucBitNumber;
}

/***** ***** ***** ***** ***** ***** ***** ***** Get Bit ***** ***** ***** ***** ***** ***** ***** *****/   
void GetBit(unsigned char ucByte, unsigned char ucBitNumber, unsigned char *ucBitValue)
{
	   if(ucBitNumber > 7)
	   	  ucBitNumber = 7;
	
	   *ucBitValue = ucByte & (1 << ucBitNumber);
}

//==============================================================================
