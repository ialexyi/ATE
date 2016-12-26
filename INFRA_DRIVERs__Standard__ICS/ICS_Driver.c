//==============================================================================
// Include files
#include "Windows.h"
#include <visa.h>  

#include "cvidef.h"  
#include "toolbox.h"
#include <userint.h>
#include <ansi_c.h>
#include <utility.h>
#include "GlobalDefines.h"

//==============================================================================
// Types

#define							MAX_NUMBER_OF_PORTS							32

typedef struct
{
	ViSession			defaultRM,
						sessionHandle;
	
	unsigned char		PortState[MAX_NUMBER_OF_PORTS];
	
	unsigned char		FirstRunning[MAX_NUMBER_OF_PORTS];  
	
} tsHandle;


//==============================================================================   
/// Local function variables

//==============================================================================
// Constants

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

STD_ERROR   ICS_Write( int sessionHandle , unsigned char ucPortNumber , unsigned char *pvByteBuffer , int iSize ,int *pBytesWritten );

//====================================================================================================//

void*			ICS_GetLowLevelHandle( int hHandle , int *phLowLevelHandle )
{
	STD_ERROR						StdError										=	{0};

	tsHandle						*pLocalHandle									=	{0};
	
	if ( phLowLevelHandle )
	{
		if ( CmtGetTSVPtr ( hHandle , &pLocalHandle ) == 0 )
		{
			*phLowLevelHandle = pLocalHandle->sessionHandle; 
			
			CmtReleaseTSVPtr ( hHandle ); 
		}	
	}
	
	RETURN_STDERR_POINTER;	
}

void*  ICS_GetErrorTextMessage ( int hInstrumentHandle , int iError , char *pErrorMessage )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	char			szCommand[LOW_STRING]				=	{0},
					szReadBuffer[LOW_STRING]			=	{0},
					szErrorDescription[LOW_STRING]		=	{0};
	
	int				count								=	0;
	
	char			*pTemp								=	NULL;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	strcpy( szCommand , ":SYST:ERR?\n" );
	
	CHK_VSA( viPrintf( pLocalHandle->sessionHandle , szCommand ));
	
	SetBreakOnLibraryErrors (0);
	viRead( pLocalHandle->sessionHandle, szReadBuffer , LOW_STRING , &count );
	SetBreakOnLibraryErrors (1);
	
	iError = atoi(szReadBuffer);
	
	pTemp = strchr( szReadBuffer , '"' );
	
	if ( pTemp )
	{
		pTemp++;
		
		strcpy( szErrorDescription , pTemp );  
		
		pTemp = strchr( szErrorDescription , '"' );
		
		if ( pTemp )
			*pTemp = 0;
	}
	
	viPrintf( pLocalHandle->sessionHandle , "*CLS\n" );
	
	if ( pErrorMessage )
	{
		strcpy( pErrorMessage , szErrorDescription ); 
	}

	StdError.error = iError;
	
	if ( iError )
	{
		CALLOC_COPY_STRING( StdError.pszErrorDescription , szErrorDescription );
	}
	
Error:

	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	RETURN_STDERR_POINTER;	
}

/***** ***** ***** ***** ***** ***** ***** ***** Init ***** ***** ***** ***** ***** ***** ***** *****/

void*  ICS_Init ( int hParentInstrumentHandle , ViRsrc szRsrcAddress , int *vhInstrumentHandle )
{
	STD_ERROR						StdError										=	{0};
	
	tsHandle						*pLocalHandle									=	{0};
	
	CmtTSVHandle 					handle											=	0;
	
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

	if ( pLocalHandle->defaultRM == 0 )
	{
		CHK_VSA ( viOpenDefaultRM (&(pLocalHandle->defaultRM)));
	}
	
	if ( pLocalHandle->sessionHandle == 0 )
	{
		CHK_VSA ( viOpen ( pLocalHandle->defaultRM , szRsrcAddress , VI_NULL, VI_NULL, &(pLocalHandle->sessionHandle)));
	}
	
Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return ICS_GetErrorTextMessage(handle,StdError.error,NULL);
}

/***** ***** ***** ***** ***** ***** ***** ***** Close ***** ***** ***** ***** ***** ***** ***** *****/

void*  ICS_Close ( int *vhInstrumentHandle )
{
	STD_ERROR		StdError						=	{0};
	
	tsHandle		*pLocalHandle					=	{0};

	int				handle							=	0;
	
	IF (( vhInstrumentHandle == NULL ) , "Handle is empty" );
	
	handle = *vhInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));
	
	viClose (pLocalHandle->sessionHandle);  
	
	viClose (pLocalHandle->defaultRM);
	
	CmtReleaseTSVPtr ( handle ); 
	
	CmtDiscardTSV (handle);
	
	*vhInstrumentHandle = 0;
	
Error:

	RETURN_STDERR_POINTER;	
}

void*  ICS_Reset ( int hInstrumentHandle )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	{0};

	int				handle								=	0;
	
	IF (( hInstrumentHandle == 0 ) , "Handle is empty" );
	
	handle = hInstrumentHandle;
	
	CHK_CMT ( CmtGetTSVPtr ( handle , &pLocalHandle ));

Error:
	
	if ( handle )
		CmtReleaseTSVPtr ( handle );
	
	return ICS_GetErrorTextMessage(handle,StdError.error,NULL);
}


/**
 * @brief ICS_SetPortConfiguration
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortNumber (unsigned char) - the specific port
 * @param[in] ucPortPolarity (unsigned char) - the configuration pins to set polarity.
 */
void*    ICS_SetPortConfiguration( int Handle , unsigned char ucPortNumber , unsigned char ucPortPolarity )
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	NULL; 

	char			szString[128]						=	{0},
					szReadFeedback[LOW_STRING]			=	{0};
	
	unsigned char 	ucPortPolarityRead					=	0;
	
	int				iActualSize							=	0;
	
	IF (( Handle == 0 ) , "Empty Handle" );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pLocalHandle  ));
	
	sprintf( szString , "OP%d %d\n" , ucPortNumber , ucPortPolarity );
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,szString ));
	
	sprintf( szString , "P%d\n" , ucPortNumber );   
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,szString ));
	
	SetBreakOnLibraryErrors (0);
	viRead( pLocalHandle->sessionHandle, szReadFeedback , (LOW_STRING-1) , &iActualSize );
	SetBreakOnLibraryErrors (1);
		
	ucPortPolarityRead = atoi(szReadFeedback);
	
	IF (( ucPortPolarityRead != ucPortPolarity ) , "Can't set polarity of the port." );
	
Error:	
	
	if( Handle ) 
		CmtReleaseTSVPtr ( Handle );
	
	return ICS_GetErrorTextMessage(Handle,StdError.error,NULL);
}

/**
 * @brief ICS_SetPort
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortNumber (unsigned char) - the specific port
 * @param[in] ucSetByte (unsigned char) - the byte (0-15) to set (write to port)
 * @param[in] ucPortDefaultValue (unsigned char) - First time preset
 */
void*    ICS_SetPort( int Handle , unsigned char ucPortNumber , unsigned char ucMask , unsigned char ucSetByte , unsigned char ucPortDefaultValue )
{
	STD_ERROR				StdError											=	{0};
	
	unsigned char   		ucWritePortData										=	0;
	
	tsHandle				*pLocalHandle										=	NULL; 
	
	int						index												=	0;
	
	unsigned char 			ucTempMask											=	0;

	char					szString[128]										=	{0},
							szReadFeedback[LOW_STRING]							=	{0};
	
	int						iActualSize											=	0;
	
	int						iValue												=	0;	
	
	IF (( Handle == 0 ) , "Empty Handle" );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pLocalHandle  ));
	
	if (( ucMask & 0x01 ) == 0 ) 
	{
		ucTempMask = ucMask;
	
		// value normalization
		for ( index = 0 ; index < 8 ; index++ )
			if (( ucTempMask & 0x01 ) == 0 )
			{
				ucTempMask >>= 1;
				ucSetByte <<= 1;
			}
			else
				break;
	}
	
	if ( pLocalHandle->FirstRunning[ucPortNumber] == 0 )
	{
		sprintf( szString , "BO%d?\n" , ucPortNumber );   
	
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,szString ));

		SetBreakOnLibraryErrors (0);
		viRead( pLocalHandle->sessionHandle, szReadFeedback , (LOW_STRING-1) , &iActualSize );
		SetBreakOnLibraryErrors (1);
	
		sscanf(szReadFeedback , "%d" , &iValue );
		
		ucWritePortData = iValue;
		
		pLocalHandle->FirstRunning[ucPortNumber] = 1;
		pLocalHandle->PortState[ucPortNumber] = ucWritePortData;
	}
		
	ucWritePortData = pLocalHandle->PortState[ucPortNumber];
	
	ucWritePortData = ((ucWritePortData & (~ucMask)) | (ucMask & ucSetByte));
	
	sprintf( szString , "BO%d %d\n" , ucPortNumber , ucWritePortData );
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,szString ));
	
	pLocalHandle->PortState[ucPortNumber] = ucWritePortData;
	
Error:	
	
	if( Handle ) 
		CmtReleaseTSVPtr ( Handle );
	
	return ICS_GetErrorTextMessage(Handle,StdError.error,NULL);	
}

/**
 * @brief ICS_GetPort
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortNumber (unsigned char) - the specific port
 * @param[in] pGetByte pointer to the geted byte (outputs from ICS_SetPort and inputs from Read command)
 */
void*    ICS_GetPort( int Handle , unsigned char ucPortNumber , unsigned char *pGetByte)
{
	STD_ERROR		StdError							=	{0};
	
	tsHandle		*pLocalHandle						=	NULL; 
	
	char			szString[128]						=	{0},
					szReadFeedback[LOW_STRING]			=	{0};
	
	int				iActualSize							=	0;
	
	int				iValue								=	0;
	
	IF (( Handle == 0 ) , "Empty Handle" );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pLocalHandle  ));
	
	sprintf( szString , "BI%d?\n" , ucPortNumber );   
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,szString ));
	
	SetBreakOnLibraryErrors (0);
	viRead( pLocalHandle->sessionHandle, szReadFeedback , (LOW_STRING-1) , &iActualSize );
	SetBreakOnLibraryErrors (1);
		
	sscanf(szReadFeedback , "%x" , &iValue );
	
	if ( pGetByte )
		*pGetByte = ( unsigned char)iValue;

Error:	
	
	if( Handle ) 
		CmtReleaseTSVPtr ( Handle );
	
	return ICS_GetErrorTextMessage(Handle,StdError.error,NULL);	
}

/**
 * @brief ICS_SetPortByBit
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortNumber (unsigned char) - the specific port
 * @param[in] ucSetByte (unsigned char) - the byte (0-15) to set (write to port)
 */
void*    ICS_SetPortByBit( int Handle , unsigned char ucPortNumber , unsigned char ucMask , unsigned char ucSetByte )
{
	STD_ERROR		StdError							=	{0};
	
	unsigned char	ucBitMask							=	0,
					ucBitIndex							=	0;
	
	char			szString[128]						=	{0};
	
	tsHandle		*pLocalHandle						=	NULL; 
	
	IF (( Handle == 0 ) , "Empty Handle" );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pLocalHandle  ));
	
	for ( ucBitIndex = 0; ucBitIndex < 8; ucBitIndex++ )
	{
		ucBitMask = 1 << ucBitIndex;
		
		if (( ucMask & ucBitMask ) == 0 )
			continue;
		
		if ( ucSetByte & ucBitMask )
			sprintf( szString , "CLOSE %d,%d\n" , ucPortNumber , ucBitIndex );   
		else
			sprintf( szString , "OPEN %d,%d\n" , ucPortNumber , ucBitIndex );   
	
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,szString ));
	}
	
Error:	
	
	if( Handle ) { CmtReleaseTSVPtr ( Handle );}
	
	return ICS_GetErrorTextMessage(Handle,StdError.error,NULL);	
}
	
/**
 * @brief ICS_GetPortByBit
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortNumber (unsigned char) - the specific port
 * @param[in] pGetByte pointer to the geted byte (outputs from ICS_SetPort and inputs from Read command)
 */
void*    ICS_GetPortByBit( int Handle , unsigned char ucPortNumber , unsigned char ucMask , unsigned char *pGetByte )
{
	STD_ERROR		StdError							=	{0};

	unsigned char	ucBitMask							=	0,
					ucBitIndex							=	0, 
					ucPortBitRead						=	0;
	
	tsHandle		*pLocalHandle						=	NULL; 
	
	char			szString[128]						=	{0},
					szReadFeedback[LOW_STRING]			=	{0};
	
	int				iActualSize							=	0;
	
	int				iValue								=	0;
	
	IF (( Handle == 0 ) , "Empty Handle" );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pLocalHandle  ));
	
	for ( ucBitIndex = 0; ucBitIndex < 8; ucBitIndex++ )
	{
		ucBitMask = 1 << ucBitIndex;
		
		if (( ucMask & ucBitMask ) == 0 )
			continue;
		
		sprintf( szString , "READ? %d,%d\n" , ucPortNumber , ucBitIndex );   
	
		CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,szString ));
	
		SetBreakOnLibraryErrors (0);
		viRead( pLocalHandle->sessionHandle, szReadFeedback , (LOW_STRING-1) , &iActualSize );
		SetBreakOnLibraryErrors (1);
		
		ucPortBitRead = atoi(szReadFeedback);
		
		if ( ucPortBitRead )
			iValue |= ucBitMask;
	}
	
	if ( pGetByte )
		*pGetByte = ( unsigned char)iValue;
	
Error:	
	
	if( Handle ) 
		CmtReleaseTSVPtr ( Handle );
	
	return ICS_GetErrorTextMessage(Handle,StdError.error,NULL);	
}

/**
 * @brief ICS_SetBit
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortNumber (unsigned char) - the specific port
 * @param[in] ucBitIndex (unsigned char) - the specific bit (0-7)
 * @param[in] ucSetBit (unsigned char) - the bit to set (0 - low, otherwise - hi)
 */
void*    ICS_SetBit( int Handle , unsigned char ucPortNumber , unsigned char ucBitIndex , unsigned char ucSetBit )
{
	STD_ERROR		StdError							=	{0};
	
	char			szString[128]						=	{0};
	
	tsHandle		*pLocalHandle						=	NULL; 
	
	IF (( Handle == 0 ) , "Empty Handle" );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pLocalHandle  ));
	
	if ( ucSetBit )
		sprintf( szString , "CLOSE %d,%d\n" , ucPortNumber , ucBitIndex );   
	else
		sprintf( szString , "OPEN %d,%d\n" , ucPortNumber , ucBitIndex );   
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,szString ));
	
Error:	
	
	if( Handle ) { CmtReleaseTSVPtr ( Handle );}
	
	return ICS_GetErrorTextMessage(Handle,StdError.error,NULL);	
}

/**
 * @brief ICS_GetBit
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortNumber (unsigned char) - the specific port
 * @param[in] ucBitIndex (unsigned char) - the specific bit (0-7)
 * @param[in] pGetBit pointer to the geted bit (output from ICS_SetPort or input from Read command)
 */
void*    ICS_GetBit( int Handle , unsigned char ucPortNumber , unsigned char ucBitIndex , unsigned char *pGetBit)
{
	STD_ERROR		StdError							=	{0};

	unsigned char	ucPortBitRead						=	0;
	
	tsHandle		*pLocalHandle						=	NULL; 
	
	char			szString[128]						=	{0},
					szReadFeedback[LOW_STRING]			=	{0};
	
	int				iActualSize							=	0;
	
	IF (( Handle == 0 ) , "Empty Handle" );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pLocalHandle  ));
	
	sprintf( szString , "READ? %d,%d\n" , ucPortNumber , ucBitIndex );   
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,szString ));
	
	SetBreakOnLibraryErrors (0);
	viRead( pLocalHandle->sessionHandle, szReadFeedback , (LOW_STRING-1) , &iActualSize );
	SetBreakOnLibraryErrors (1);
		
	ucPortBitRead = atoi(szReadFeedback);
	
	if ( pGetBit )
	{
		*pGetBit = ucPortBitRead;
	}
	
Error:	
	
	if( Handle ) 
		CmtReleaseTSVPtr ( Handle );
	
	return ICS_GetErrorTextMessage(Handle,StdError.error,NULL);	
}

/**
 * @brief ICS_SPI
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortNumber (unsigned char) - the specific port
 * @param[in] ucEnableBitIndex (unsigned char) - Enable bit ( to disable set to 8 )
 * @param[in] ucEnableBitPolarity (unsigned char) - If Enable bit enabled used polarity to enable (1 or 0)
 * @param[in] ucClockBitIndex (unsigned char) - Clock Bit Index
 * @param[in] ucClockBitPolarity (unsigned char) - Clock Bit Polarity 
 * @param[in] ucDataBitIndex (unsigned char) - Data in Bit Index
 * @param[in] ullSetData (unsigned long long) - data bits to send
 * @param[in] ucSetNumberOfBits (unsigned char) - number bits to send
 * @param[in] ucReadBitPortIndex (unsigned char) - Data out Bit Port Index
 * @param[in] ucReadBitIndex (unsigned char) - Data out Bit Index    
 * @param[out] ullGetData (unsigned long long) - pointer to data will be received.
 * @param[in] ucGetNumberOfBits (unsigned char) - number bits to send
 */
void*    ICS_SPI( int Handle , unsigned char ucPortNumber , unsigned char ucEnableBitIndex , unsigned char ucEnableBitPolarity , unsigned char ucClockBitIndex , unsigned char ucClockBitPolarity , unsigned char ucClockBitPhase  , unsigned char ucDataBitIndex , unsigned __int64 ullSetData , unsigned char ucSetNumberOfBits , unsigned char ucReadBitPortIndex , unsigned char ucReadBitIndex , unsigned __int64 *ullGetData , unsigned char ucGetNumberOfBits )
{
	STD_ERROR				StdError											=	{0};
	
	DWORD  					BytesNum											=	0,
							BytesWritten										=	0;
	
	char   					ucWritePortData[512]								=	{0};
	
	tsHandle				*pLocalHandle										=	NULL; 
	
	int						index												=	0;
	
	unsigned char 			ucClockBitMask										=	0,
							ucDataBitMask										=	0,
							ucEnableBitMask										=	0,  
							ucReadBitMask										=	0,
							ucReadByte											=	0, 
							ucTempByte											=	0; 
								
	unsigned long long		ullOutputByte										=	0;
	
	char					szString[128]										=	{0},
							szReadFeedback[LOW_STRING]							=	{0};
	
	int						iActualSize											=	0;
	
	IF (( Handle == 0 ) , "Empty Handle" );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pLocalHandle  ));

	IF(( ucClockBitIndex == ucDataBitIndex ) , "Clock Bit can't be same bit as Data Bit." ); 
	
	IF(( ucClockBitIndex > 7 ) , "Wrong parameter (ucClockBitIndex)" );  
	IF(( ucDataBitIndex > 7 ) , "Wrong parameter (ucDataBitIndex)" );  
	
	ucClockBitMask = 1 << ucClockBitIndex;
	ucDataBitMask = 1 << ucDataBitIndex;
	ucEnableBitMask = 1 << ucEnableBitIndex; 
	ucReadBitMask = 1 << ucReadBitIndex;
	
	if ( ucSetNumberOfBits )
	{
		BytesNum = 0;
	
		if ( ucClockBitPolarity ) 
			ucTempByte = 0xFF;
		else
			ucTempByte = 0;
	
		// Clear Clock 
		ucWritePortData[BytesNum++] = (pLocalHandle->PortState[ucPortNumber]) = (((pLocalHandle->PortState[ucPortNumber]) & (~ucClockBitMask)) | (ucClockBitMask & ucTempByte ));

		if ( ucEnableBitIndex < 8 )
		{
			if ( ucEnableBitPolarity )
				ucTempByte = 0xFF;
			else
				ucTempByte = 0;	
		
			// Enable SPI
			ucWritePortData[BytesNum++] = (pLocalHandle->PortState[ucPortNumber]) = (((pLocalHandle->PortState[ucPortNumber]) & (~ucEnableBitMask)) | (ucEnableBitMask & ucTempByte ));

		}
	
		if ( ucClockBitPolarity ) 
			ucTempByte = 0xFF;
		else
			ucTempByte = 0;
	
		// Clear Clock 
		ucWritePortData[BytesNum++] = (pLocalHandle->PortState[ucPortNumber]) = (((pLocalHandle->PortState[ucPortNumber]) & (~ucClockBitMask)) | (ucClockBitMask & ucTempByte ));

		
		for( index = 0 ; index < ucSetNumberOfBits ; index++ )
		{
			if ( ucClockBitPhase == 0 )
			{
				if ( ullSetData & ( 0x1 << ( ucSetNumberOfBits - index - 1 )))
					ucTempByte = 0xFF;
				else
					ucTempByte = 0;
					
				// Set Data
				ucWritePortData[BytesNum++] = (pLocalHandle->PortState[ucPortNumber]) = (((pLocalHandle->PortState[ucPortNumber]) & (~ucDataBitMask)) | (ucDataBitMask & ucTempByte ));
			}
			
			if ( !ucClockBitPolarity ) 
				ucTempByte = 0xFF;
			else
				ucTempByte = 0;
		
			// Clear Clock
			ucWritePortData[BytesNum++] = (pLocalHandle->PortState[ucPortNumber]) = (((pLocalHandle->PortState[ucPortNumber]) & (~ucClockBitMask)) | (ucClockBitMask & ucTempByte ));
		
			if ( ucClockBitPhase == 1 )
			{
				if ( ullSetData & ( 0x1 << ( ucSetNumberOfBits - index - 1 )))
					ucTempByte = 0xFF;
				else
					ucTempByte = 0;
					
				// Set Data
				ucWritePortData[BytesNum++] = (pLocalHandle->PortState[ucPortNumber]) = (((pLocalHandle->PortState[ucPortNumber]) & (~ucDataBitMask)) | (ucDataBitMask & ucTempByte ));
			}
			
			if ( ucClockBitPolarity ) 
				ucTempByte = 0xFF;
			else
				ucTempByte = 0;
		
			// Set Clock
			ucWritePortData[BytesNum++] = (pLocalHandle->PortState[ucPortNumber]) = (((pLocalHandle->PortState[ucPortNumber]) & (~ucClockBitMask)) | (ucClockBitMask & ucTempByte ));
		}
	
		if ( ucClockBitPolarity ) 
			ucTempByte = 0xFF;
		else
			ucTempByte = 0;

		// Clear Clock 
		ucWritePortData[BytesNum++] = (pLocalHandle->PortState[ucPortNumber]) = (((pLocalHandle->PortState[ucPortNumber]) & (~ucClockBitMask)) | (ucClockBitMask & ucTempByte ));
		
		if (( ucEnableBitIndex < 8 ) && ( ucGetNumberOfBits == 0 )) 
		{
			if ( !ucEnableBitPolarity )
				ucTempByte = 0xFF;
			else
				ucTempByte = 0;	
		
			// Disable SPI
			ucWritePortData[BytesNum++] = (pLocalHandle->PortState[ucPortNumber]) = (((pLocalHandle->PortState[ucPortNumber]) & (~ucEnableBitMask)) | (ucEnableBitMask & ucTempByte ));
		}
	
		ucTempByte = 0;
		
		// Clear Data
		ucWritePortData[BytesNum++] = (pLocalHandle->PortState[ucPortNumber]) = (((pLocalHandle->PortState[ucPortNumber]) & (~ucDataBitMask)) | (ucDataBitMask & ucTempByte ));
	
		CHK_STDERR(ICS_Write( pLocalHandle->sessionHandle , ucPortNumber , ucWritePortData , BytesNum , &BytesWritten));
		
		IF(( BytesNum != BytesWritten ) , "SPI send fail." );   
	}
	
	if ( ucGetNumberOfBits )
	{
		//---------------------------------------------//
		if ( ucClockBitPolarity ) 
			ucTempByte = 0xFF;
		else
			ucTempByte = 0;
	
		// Clear Clock 
		ucWritePortData[0] = (pLocalHandle->PortState[ucPortNumber]) = (((pLocalHandle->PortState[ucPortNumber]) & (~ucClockBitMask)) | (ucClockBitMask & ucTempByte ));

		CHK_STDERR(ICS_Write( pLocalHandle->sessionHandle , ucPortNumber, ucWritePortData, 1, &BytesWritten) );
	
		IF(( BytesWritten != 1 ) , "SPI send fail." ); 
		//---------------------------------------------//  
		
		if (( ucEnableBitIndex < 8 ) && ( ucSetNumberOfBits == 0 ))
		{
			if ( ucEnableBitPolarity )
				ucTempByte = 0xFF;
			else
				ucTempByte = 0;	
		
			// Enable SPI
			ucWritePortData[0] = (pLocalHandle->PortState[ucPortNumber]) = (((pLocalHandle->PortState[ucPortNumber]) & (~ucEnableBitMask)) | (ucEnableBitMask & ucTempByte ));
		
			CHK_STDERR(ICS_Write( pLocalHandle->sessionHandle , ucPortNumber, ucWritePortData, 1, &BytesWritten) );
			
			IF(( BytesWritten != 1 ) , "SPI send fail." ); 
		}
	
		//---------------------------------------------//
		if ( ucClockBitPolarity ) 
			ucTempByte = 0xFF;
		else
			ucTempByte = 0;
	
		// Clear Clock 
		ucWritePortData[0] = (pLocalHandle->PortState[ucPortNumber]) = (((pLocalHandle->PortState[ucPortNumber]) & (~ucClockBitMask)) | (ucClockBitMask & ucTempByte ));

		CHK_STDERR(ICS_Write( pLocalHandle->sessionHandle , ucPortNumber, ucWritePortData, 1, &BytesWritten) );
	
		IF(( BytesWritten != 1 ) , "SPI send fail." ); 
		//---------------------------------------------// 
		
		for( index = 0 ; index < ucGetNumberOfBits ; index++ )
		{
			//================================= Clock ============================================//
			if ( !ucClockBitPolarity ) 
				ucTempByte = 0xFF;
			else
				ucTempByte = 0;
		
			// Clear Clock
			ucWritePortData[0] = (pLocalHandle->PortState[ucPortNumber]) = (((pLocalHandle->PortState[ucPortNumber]) & (~ucClockBitMask)) | (ucClockBitMask & ucTempByte ));
		
			CHK_STDERR(ICS_Write( pLocalHandle->sessionHandle , ucPortNumber, ucWritePortData, 1, &BytesWritten) );
			
			IF(( BytesWritten != 1 ) , "SPI send fail." );
			//====================================================================================//
			//================================= Data ============================================//
			// Get Data
			
			if ( ucClockBitPhase == 0 )
			{
				sprintf( szString , "BI%d?\n" , ucPortNumber );   
	
				CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,szString ));
	
				SetBreakOnLibraryErrors (0);
				viRead( pLocalHandle->sessionHandle, szReadFeedback , (LOW_STRING-1) , &iActualSize );
				SetBreakOnLibraryErrors (1);
	
				ucReadByte = atoi(szReadFeedback);    

				if (( 0x01 << ucReadBitIndex ) & ucReadByte )
					ullOutputByte |= 0x01 << ( ucGetNumberOfBits - index - 1 );
			}
			//====================================================================================//
			//================================= Clock ============================================//
			
			if ( ucClockBitPolarity ) 
				ucTempByte = 0xFF;
			else
				ucTempByte = 0;
			
			// Set Clock
			ucWritePortData[0] = (pLocalHandle->PortState[ucPortNumber]) = (((pLocalHandle->PortState[ucPortNumber]) & (~ucClockBitMask)) | (ucClockBitMask & ucTempByte ));
		
			CHK_STDERR(ICS_Write( pLocalHandle->sessionHandle , ucPortNumber, ucWritePortData, 1, &BytesWritten) );
			
			IF(( BytesWritten != 1 ) , "SPI send fail." );
			//====================================================================================//
			//================================= Data ============================================//
			// Get Data
			
			if ( ucClockBitPhase == 1 )
			{
				sprintf( szString , "BI%d?\n" , ucPortNumber );   
	
				CHK_VSA ( viPrintf( pLocalHandle->sessionHandle ,szString ));
	
				SetBreakOnLibraryErrors (0);
				viRead( pLocalHandle->sessionHandle, szReadFeedback , (LOW_STRING-1) , &iActualSize );
				SetBreakOnLibraryErrors (1);
	
				ucReadByte = atoi(szReadFeedback);    

				if (( 0x01 << ucReadBitIndex ) & ucReadByte )
					ullOutputByte |= 0x01 << ( ucGetNumberOfBits - index - 1 );
			}
			//====================================================================================//
		}
	
		if ( ucClockBitPolarity ) 
			ucTempByte = 0xFF;
		else
			ucTempByte = 0;
	
		// Clear Clock 
		ucWritePortData[0] = (pLocalHandle->PortState[ucPortNumber]) = (((pLocalHandle->PortState[ucPortNumber]) & (~ucClockBitMask)) | (ucClockBitMask & ucTempByte ));
		
		CHK_STDERR(ICS_Write( pLocalHandle->sessionHandle , ucPortNumber, ucWritePortData, 1, &BytesWritten) );
	
		IF(( BytesWritten != 1 ) , "SPI send fail." );
		
		if ( ucEnableBitIndex < 8 )
		{
			if ( !ucEnableBitPolarity )
				ucTempByte = 0xFF;
			else
				ucTempByte = 0;	
		
			// Enable SPI
			ucWritePortData[0] = (pLocalHandle->PortState[ucPortNumber]) = (((pLocalHandle->PortState[ucPortNumber]) & (~ucEnableBitMask)) | (ucEnableBitMask & ucTempByte ));
		
			CHK_STDERR(ICS_Write( pLocalHandle->sessionHandle , ucPortNumber, ucWritePortData, 1, &BytesWritten) );
			
			IF(( BytesWritten != 1 ) , "SPI send fail." );
		}
	}
	
Error:	
	
	if ( ullGetData )
		*ullGetData = ullOutputByte;
	
	if( Handle ) 
		CmtReleaseTSVPtr ( Handle );
	
	return ICS_GetErrorTextMessage(Handle,StdError.error,NULL);	
}


STD_ERROR   ICS_Write( int sessionHandle , unsigned char ucPortNumber , unsigned char *pvByteBuffer , int iSize ,int *pBytesWritten )
{
	STD_ERROR				StdError											=	{0};
	
	int						index												=	0;
	
	char					szString[128]										=	{0};
	
	for( index = 0; index < iSize; index++ )
	{
		sprintf( szString , "BO%d %d\n" , ucPortNumber , pvByteBuffer[index] );
	
		CHK_VSA ( viPrintf( sessionHandle ,szString ));
	}
	
Error:	
	
	if ( pBytesWritten )
		*pBytesWritten = index;
	
	RETURN_STDERR;	
}


void*	   ICS_SaveDefaultState( int Handle )
{
	STD_ERROR				StdError											=	{0};

	tsHandle				*pLocalHandle										=	NULL; 

	IF (( Handle == 0 ) , "Empty Handle" );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pLocalHandle  ));
	
	CHK_VSA ( viPrintf( pLocalHandle->sessionHandle , "*SAV 0\n" ));
	
Error:	
	
	if( Handle ) 
		CmtReleaseTSVPtr ( Handle );
		
	return ICS_GetErrorTextMessage(Handle,StdError.error,NULL);	
}
