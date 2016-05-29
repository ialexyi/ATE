//==============================================================================
// Include files
#include <windows.h>
#include <utility.h>
#include <userint.h>
#include <visa.h>
#include <ansi_c.h>

#include "LambdaZup.h"
#include "LambdaZup_Support.h"
#include "LambdaZup_Types.h"


//==============================================================================
// Constants

#define			MODULE_NAME			"LambdaZup Driver"

//==============================================================================
// Types

//==============================================================================
// Static global variables

extern		tsLambdaTypes			tLambdaZupDefines[];

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

/***** ***** ***** ***** ***** ***** ***** Ge tError Text Message ***** ***** ***** ***** ***** ***** *****/

errEV ZUP_GetErrorTextMessage	( int handle , int iError , char *pErrorMessage )
{
	errEV				ret								=	{0};
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	int					iVisaStatus						=	0;
	
	char				szVisaErrorDescription[256]		=	{0},
						szErrorDescription[STD_STRING]	=	{0},
						*pErrorLocalString				=	NULL;
	
	IF (( pErrorMessage == NULL ) , "Error passed NULL pointer." , MODULE_NAME );
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	iVisaStatus = viStatusDesc ( visaObjHandle , iError , szVisaErrorDescription );
		
	if (iVisaStatus == VI_SUCCESS)
		strcpy (szErrorDescription, szVisaErrorDescription);
			
	else
		strcpy (szErrorDescription, "Unknown error.");	
	
	if ( pErrorMessage && pErrorLocalString )
	{
		strcpy( pErrorMessage , szErrorDescription );
	}
	
Error:
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** ***** ***** Init ***** ***** ***** ***** ***** ***** ***** *****/

errEV ZUP_Init ( ViRsrc szRsrcAddress , int *pHandle , int *pChannelsQuantity )
{
	errEV				ret								=	{0};  	

	ViSession  			viRsrcManagerHandle				=	0,
						visaObjHandle					=	0;
	
	ViStatus			viStatus						=	0;
		
	int					handle							=	0;
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	char				szChannelModelName[RD_BUF+1]		=	{0};
	
	int					iZupChanelIndex					=	0,
						iZupChanelsQuantity				=	0,
						iTypeCount						=	0,
						iTypeQuantity					=	0,
						bModelFound						=	0;
	
	IF (( pHandle == NULL ) , "Error passed NULL pointer." , MODULE_NAME );
	
	CHK_CMT (CmtNewTSV ( sizeof(tsLambdaHandle) , &handle ));
	
	*pHandle = handle;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
		
	viStatus = viOpenDefaultRM( &viRsrcManagerHandle );
	
	IF (( viStatus != VI_SUCCESS ), "Error open default RM." , MODULE_NAME);
	
	viStatus = viOpen ( viRsrcManagerHandle, szRsrcAddress, VI_NULL, OPEN_TIMEOUT, &visaObjHandle );
	
	IF (( viStatus != VI_SUCCESS ), "Can't establish communication." , MODULE_NAME);
	
	ptLambdaVariable->InstrumentHandle = visaObjHandle;
	
	if ( strstr( szRsrcAddress, "ASRL" ) != NULL )
	{
		CHK_VSA( viSetAttribute( visaObjHandle, VI_ATTR_ASRL_BAUD, VI_ASRL_BAUD_9600 ));
		
		CHK_VSA( viSetAttribute( visaObjHandle, VI_ATTR_ASRL_DATA_BITS, VI_ASRL_BITS_8 ));
		
		CHK_VSA( viSetAttribute( visaObjHandle, VI_ATTR_ASRL_PARITY, VI_ASRL_PAR_NONE ));
		
		CHK_VSA( viSetAttribute( visaObjHandle, VI_ATTR_ASRL_STOP_BITS, VI_ASRL_STOP_ONE ));
		
		CHK_VSA( viSetAttribute( visaObjHandle, VI_ATTR_ASRL_FLOW_CNTRL, VI_ASRL_FLOW_XON_XOFF ));
	}
	
	iTypeQuantity = LambdaZupTypesQuantity();
	
	CHK_VSA( viSetAttribute( visaObjHandle, VI_ATTR_TMO_VALUE, 100 ));
	
	do
	{
		iZupChanelIndex++;
		bModelFound = 0;
			
		ret = ZUP_GetPowerSupplyModel ( handle , iZupChanelIndex , szChannelModelName );						//Get Channel model (type)

		if ((ret.error == 0) && (strstr(szChannelModelName , "Lambda ZUP") != NULL))							
		{
			for (iTypeCount = 0; iTypeCount < iTypeQuantity ; iTypeCount++)										//loop over known types
			{
				if (strstr (szChannelModelName, tLambdaZupDefines[iTypeCount].szPowerSupplyModel) != NULL)		//if known types 
				{
					bModelFound = 1;
					break;//for loop
				}
			}
			
			if ( bModelFound )
			{
				iZupChanelsQuantity++;
				continue;//do-while loop
			}
		}
		
		ret.error = 0;
		break;
		
	}while(1);
	
	IF ((iZupChanelsQuantity < 1), "No Power Supply supported model channels found." , MODULE_NAME);
	
	ptLambdaVariable->iChanelsQuantity = iZupChanelsQuantity;										//Store channels quantity
	CALLOC_ERR (ptLambdaVariable->pLamdaChannel, iZupChanelsQuantity, sizeof(tsLambdaChannel));		//Allocate memory for channels structs
	
	for (iZupChanelIndex = 0; iZupChanelIndex < iZupChanelsQuantity; iZupChanelIndex++)
	{
		CHK_RET (ZUP_GetPowerSupplyModel ( handle , iZupChanelIndex+1, szChannelModelName ));					//Get Channel model (type)
		
		for (iTypeCount = 0; iTypeCount < iTypeQuantity ; iTypeCount++)											//loop over known types
		{
			if (strstr (szChannelModelName, tLambdaZupDefines[iTypeCount].szPowerSupplyModel) != NULL)			//if known types 
			{
				strcpy (ptLambdaVariable->pLamdaChannel[iZupChanelIndex].szChModelName, tLambdaZupDefines[iTypeCount].szPowerSupplyModel);
				CHK_RET (IntToStringAddress ( iZupChanelIndex+1 , ptLambdaVariable->pLamdaChannel[iZupChanelIndex].szChAddress , &(ptLambdaVariable->pLamdaChannel[iZupChanelIndex].iAddrBufLen) ));
				
				ptLambdaVariable->pLamdaChannel[iZupChanelIndex].currentDefine.lfMaxValue = tLambdaZupDefines[iTypeCount].currentDefine.lfMaxValue;
				ptLambdaVariable->pLamdaChannel[iZupChanelIndex].currentDefine.lfMinValue = tLambdaZupDefines[iTypeCount].currentDefine.lfMinValue;
				ptLambdaVariable->pLamdaChannel[iZupChanelIndex].currentDefine.lfAccuracy = tLambdaZupDefines[iTypeCount].currentDefine.lfAccuracy;
				
				ptLambdaVariable->pLamdaChannel[iZupChanelIndex].voltageDefine.lfMaxValue = tLambdaZupDefines[iTypeCount].voltageDefine.lfMaxValue;
				ptLambdaVariable->pLamdaChannel[iZupChanelIndex].voltageDefine.lfMinValue = tLambdaZupDefines[iTypeCount].voltageDefine.lfMinValue;
				ptLambdaVariable->pLamdaChannel[iZupChanelIndex].voltageDefine.lfAccuracy = tLambdaZupDefines[iTypeCount].voltageDefine.lfAccuracy;
				
				ptLambdaVariable->pLamdaChannel[iZupChanelIndex].overVoltageDefine.lfMaxValue = tLambdaZupDefines[iTypeCount].overVoltageDefine.lfMaxValue;
				ptLambdaVariable->pLamdaChannel[iZupChanelIndex].overVoltageDefine.lfMinValue = tLambdaZupDefines[iTypeCount].overVoltageDefine.lfMinValue;
				ptLambdaVariable->pLamdaChannel[iZupChanelIndex].overVoltageDefine.lfAccuracy = tLambdaZupDefines[iTypeCount].overVoltageDefine.lfAccuracy;
				
				ptLambdaVariable->pLamdaChannel[iZupChanelIndex].underVoltageDefine.lfMaxValue = tLambdaZupDefines[iTypeCount].underVoltageDefine.lfMaxValue;
				ptLambdaVariable->pLamdaChannel[iZupChanelIndex].underVoltageDefine.lfMinValue = tLambdaZupDefines[iTypeCount].underVoltageDefine.lfMinValue;
				ptLambdaVariable->pLamdaChannel[iZupChanelIndex].underVoltageDefine.lfAccuracy = tLambdaZupDefines[iTypeCount].underVoltageDefine.lfAccuracy;

				break;
			}
		}
	}
	
	*pChannelsQuantity = iZupChanelsQuantity;
	
Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** ***** ***** Close ***** ***** ***** ***** ***** ***** ***** *****/

errEV ZUP_Close ( int *pHandle )
{
	errEV				ret								=	{0};
	
	int					handle							=	0; 
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
																			
	IF (( pHandle == NULL ) , "Error passed NULL pointer." , MODULE_NAME ); 
	
	handle = *pHandle;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	if (ptLambdaVariable->pLamdaChannel)
		FREE(ptLambdaVariable->pLamdaChannel);
	
	CHK_VSA( viClose (visaObjHandle));

Error:
	
	if (handle)
	{
		CmtReleaseTSVPtr (handle);
		CmtDiscardTSV (handle);
		*pHandle = 0;
	}
	
	return ret;
}

/***** ***** ***** ***** ***** ***** Clear Comm Buffer And Registers ***** ***** ***** ***** ***** *****/

errEV ZUP_ClearCommBufferAndRegisters ( int handle , int iPSAddress )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0};
	
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0;
	
	unsigned int		iChIndex						=	0;
	
	iChIndex = iPSAddress-1;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	IF (((iPSAddress < 1) || (iPSAddress > ptLambdaVariable->iChanelsQuantity)), "Illegal parameter.", MODULE_NAME);
	
	iWriteBufLen = ptLambdaVariable->pLamdaChannel[iChIndex].iAddrBufLen;
		
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)ptLambdaVariable->pLamdaChannel[iChIndex].szChAddress, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);

	iWriteBufLen = sprintf ( szWriteBuffer , ":DCL;" );
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** Get Power Supply Model ***** ***** ***** ***** ***** ***** *****/

errEV ZUP_GetPowerSupplyModel ( int handle , int iPSAddress , char pReturnString[RD_BUF] )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0},
						szReadBuffer[RD_BUF+1]			=	{0};
	
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0,
						iActReceivedLen					=	0;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle; 
	
	CHK_RET ( IntToStringAddress ( iPSAddress , szWriteBuffer , &iWriteBufLen ));
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
		
	Delay (PS_COM_DELAY);

	memset (szWriteBuffer, 0, sizeof(szWriteBuffer));
	
	iWriteBufLen = sprintf ( szWriteBuffer , ":MDL?;" );
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);

	SetBreakOnLibraryErrors (0);
	
	CHK_VSA ( viRead ( visaObjHandle, (ViBuf)szReadBuffer, 127, &iActReceivedLen ));
	
	SetBreakOnLibraryErrors (1);
	
	IF ((iActReceivedLen < 1) , "Error receiving data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	strcpy ( pReturnString , szReadBuffer );
	
Error:
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** Get Power Supply Revision ***** ***** ***** ***** ***** ***** *****/

errEV ZUP_GetPowerSupplyRevision ( int handle , int iPSAddress , char pReturnString[RD_BUF] )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0},
						szReadBuffer[RD_BUF+1]			=	{0};
	
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0,
						iActReceivedLen					=	0;
	
	unsigned int		iChIndex						=	0;
	
	iChIndex = iPSAddress-1;
		
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	IF (((iPSAddress < 1) || (iPSAddress > ptLambdaVariable->iChanelsQuantity)), "Illegal parameter.", MODULE_NAME);
	
	iWriteBufLen = ptLambdaVariable->pLamdaChannel[iChIndex].iAddrBufLen;
		
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)ptLambdaVariable->pLamdaChannel[iChIndex].szChAddress, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);

	iWriteBufLen = sprintf ( szWriteBuffer , ":REV?;" );
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	CHK_VSA ( viRead ( visaObjHandle, (ViBuf)szReadBuffer, 127 , &iActReceivedLen ));
	
	IF ((iActReceivedLen < 1) , "Error receiving data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	strcpy ( pReturnString , szReadBuffer );

Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** ***** Set Output Voltage **** ****** ***** ***** ***** ***** *****/

errEV ZUP_SetOutputVoltage ( int handle , int iPSAddress , double lfVolts )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0};
	
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0;
	
	unsigned int		iChIndex						=	0;
	
	iChIndex = iPSAddress-1;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	IF (((iPSAddress < 1) || (iPSAddress > ptLambdaVariable->iChanelsQuantity)), "Illegal parameter.", MODULE_NAME);
	
	iWriteBufLen = ptLambdaVariable->pLamdaChannel[iChIndex].iAddrBufLen;
		
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)ptLambdaVariable->pLamdaChannel[iChIndex].szChAddress, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);

	CHK_RET ( DoubleToStringVoltage ( ptLambdaVariable->pLamdaChannel[iPSAddress-1] , lfVolts , szWriteBuffer , &iWriteBufLen ));
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** ***** Get Output Voltage Defined **** ****** ***** ***** ***** ***** *****/ 

errEV ZUP_GetOutputVoltageDefined ( int handle , int iPSAddress , double *plfVolts )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0},
						szReadBuffer[RD_BUF+1]			=	{0};
	
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0,
						iActReceivedLen					=	0;
	
	double				lfReceivedValue					=	0.0;
	
	unsigned int		iChIndex						=	0;
	
	iChIndex = iPSAddress-1;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	IF (((iPSAddress < 1) || (iPSAddress > ptLambdaVariable->iChanelsQuantity)), "Illegal parameter.", MODULE_NAME);
	
	iWriteBufLen = ptLambdaVariable->pLamdaChannel[iChIndex].iAddrBufLen;
		
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)ptLambdaVariable->pLamdaChannel[iChIndex].szChAddress, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	iWriteBufLen = sprintf ( szWriteBuffer , ":VOL!;" );
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	CHK_VSA ( viRead ( visaObjHandle, (ViBuf)szReadBuffer, 127, &iActReceivedLen ));
	
	IF ((iActReceivedLen < 1) , "Error receiving data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	sscanf ( szReadBuffer , "SV%lf" , &lfReceivedValue );
	
	*plfVolts = lfReceivedValue; 
	
Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** ***** Get Output Voltage Actual **** ****** ***** ***** ***** ***** *****/ 

errEV ZUP_GetOutputVoltageActual ( int handle , int iPSAddress , double *plfVolts )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0},
						szReadBuffer[RD_BUF+1]			=	{0};
	
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0,
						iActReceivedLen					=	0;
	
	double				lfReceivedValue					=	0.0;
	
	unsigned int		iChIndex						=	0;
	
	iChIndex = iPSAddress-1;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	IF (((iPSAddress < 1) || (iPSAddress > ptLambdaVariable->iChanelsQuantity)), "Illegal parameter.", MODULE_NAME);
	
	iWriteBufLen = ptLambdaVariable->pLamdaChannel[iChIndex].iAddrBufLen;
		
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)ptLambdaVariable->pLamdaChannel[iChIndex].szChAddress, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	iWriteBufLen = sprintf ( szWriteBuffer , ":VOL?;" );
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	CHK_VSA ( viRead ( visaObjHandle, (ViBuf)szReadBuffer, 127 , &iActReceivedLen ));
	
	IF ((iActReceivedLen < 1) , "Error receiving data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	sscanf ( szReadBuffer , "AV%lf" , &lfReceivedValue );
	
	*plfVolts = lfReceivedValue; 
	
Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** ***** Set Output Current **** ****** ***** ***** ***** ***** *****/ 

errEV ZUP_SetOutputCurrent ( int handle , int iPSAddress , double lfAmpers )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0};
														
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0;
	
	unsigned int		iChIndex						=	0;
	
	iChIndex = iPSAddress-1;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	IF (((iPSAddress < 1) || (iPSAddress > ptLambdaVariable->iChanelsQuantity)), "Illegal parameter.", MODULE_NAME);
	
	iWriteBufLen = ptLambdaVariable->pLamdaChannel[iChIndex].iAddrBufLen;
		
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)ptLambdaVariable->pLamdaChannel[iChIndex].szChAddress, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
		
	CHK_RET ( DoubleToStringCurrent ( ptLambdaVariable->pLamdaChannel[iPSAddress-1] , lfAmpers , szWriteBuffer , &iWriteBufLen ));
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** ***** Get Output Current Defined **** ****** ***** ***** ***** ***** *****/ 

errEV ZUP_GetOutputCurrentDefined ( int handle , int iPSAddress , double *plfAmpers )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0},
						szReadBuffer[RD_BUF+1]			=	{0};
	
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0,
						iActReceivedLen					=	0;
	
	double				lfReceivedValue					=	0.0;
	
	unsigned int		iChIndex						=	0;
	
	iChIndex = iPSAddress-1;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	IF (((iPSAddress < 1) || (iPSAddress > ptLambdaVariable->iChanelsQuantity)), "Illegal parameter.", MODULE_NAME);
	
	iWriteBufLen = ptLambdaVariable->pLamdaChannel[iChIndex].iAddrBufLen;
		
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)ptLambdaVariable->pLamdaChannel[iChIndex].szChAddress, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	iWriteBufLen = sprintf ( szWriteBuffer , ":CUR!;" );
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	CHK_VSA ( viRead ( visaObjHandle, (ViBuf)szReadBuffer, 127, &iActReceivedLen ));
	
	IF ((iActReceivedLen < 1) , "Error receiving data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	sscanf ( szReadBuffer , "SA%lf" , &lfReceivedValue );
	
	*plfAmpers = lfReceivedValue; 
	
Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** ***** Get Output Current Actual **** ****** ***** ***** ***** ***** *****/

errEV ZUP_GetOutputCurrentActual ( int handle , int iPSAddress , double *plfAmpers )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0},
						szReadBuffer[RD_BUF+1]			=	{0};
	
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0,
						iActReceivedLen					=	0;
	
	double				lfReceivedValue					=	0.0;
	
	unsigned int		iChIndex						=	0;
	
	iChIndex = iPSAddress-1;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	IF (((iPSAddress < 1) || (iPSAddress > ptLambdaVariable->iChanelsQuantity)), "Illegal parameter.", MODULE_NAME);
	
	iWriteBufLen = ptLambdaVariable->pLamdaChannel[iChIndex].iAddrBufLen;
		
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)ptLambdaVariable->pLamdaChannel[iChIndex].szChAddress, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	iWriteBufLen = sprintf ( szWriteBuffer , ":CUR?;" );
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	CHK_VSA ( viRead ( visaObjHandle, (ViBuf)szReadBuffer, 127, &iActReceivedLen ));
	
	IF ((iActReceivedLen < 1) , "Error receiving data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	sscanf ( szReadBuffer , "AA%lf" , &lfReceivedValue );
	
	*plfAmpers = lfReceivedValue; 
	
Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** ***** Set Voltage Protection Over **** ****** ***** ***** ***** ***** *****/

errEV ZUP_SetVoltageProtectionOver ( int handle , int iPSAddress , double lfVolts )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0};
	
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0;
	
	unsigned int		iChIndex						=	0;
	
	iChIndex = iPSAddress-1;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	IF (((iPSAddress < 1) || (iPSAddress > ptLambdaVariable->iChanelsQuantity)), "Illegal parameter.", MODULE_NAME);
	
	iWriteBufLen = ptLambdaVariable->pLamdaChannel[iChIndex].iAddrBufLen;
		
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)ptLambdaVariable->pLamdaChannel[iChIndex].szChAddress, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);

	CHK_RET ( DoubleToStringOverProtection ( ptLambdaVariable->pLamdaChannel[iPSAddress-1] , lfVolts , szWriteBuffer , &iWriteBufLen ));
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** ***** Get Voltage Protection Over **** ****** ***** ***** ***** ***** *****/

errEV ZUP_GetVoltageProtectionOver ( int handle , int iPSAddress , double *plfVolts )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0},
						szReadBuffer[RD_BUF+1]			=	{0};
	
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0,
						iActReceivedLen					=	0;
	
	double				lfReceivedValue					=	0.0;
	
	unsigned int		iChIndex						=	0;
	
	iChIndex = iPSAddress-1;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	IF (((iPSAddress < 1) || (iPSAddress > ptLambdaVariable->iChanelsQuantity)), "Illegal parameter.", MODULE_NAME);
	
	iWriteBufLen = ptLambdaVariable->pLamdaChannel[iChIndex].iAddrBufLen;
		
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)ptLambdaVariable->pLamdaChannel[iChIndex].szChAddress, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	iWriteBufLen = sprintf ( szWriteBuffer , ":OVP?;" );
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	CHK_VSA ( viRead ( visaObjHandle, (ViBuf)szReadBuffer, 127, &iActReceivedLen ));
	
	IF ((iActReceivedLen < 1) , "Error receiving data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	sscanf ( szReadBuffer , "OP%lf" , &lfReceivedValue );
	
	*plfVolts = lfReceivedValue; 
	
Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** ***** Set Voltage Protection Under **** ****** ***** ***** ***** ***** *****/

errEV ZUP_SetVoltageProtectionUnder ( int handle , int iPSAddress , double lfVolts )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0};
	
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0;
	
	unsigned int		iChIndex						=	0;
	
	iChIndex = iPSAddress-1;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	IF (((iPSAddress < 1) || (iPSAddress > ptLambdaVariable->iChanelsQuantity)), "Illegal parameter.", MODULE_NAME);
	
	iWriteBufLen = ptLambdaVariable->pLamdaChannel[iChIndex].iAddrBufLen;
		
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)ptLambdaVariable->pLamdaChannel[iChIndex].szChAddress, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
		
	CHK_RET ( DoubleToStringUnderProtection ( ptLambdaVariable->pLamdaChannel[iPSAddress-1] , lfVolts , szWriteBuffer , &iWriteBufLen ));
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** ***** Get Voltage Protection Under **** ****** ***** ***** ***** ***** *****/

errEV ZUP_GetVoltageProtectionUnder ( int handle , int iPSAddress , double *plfVolts )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0},
						szReadBuffer[RD_BUF+1]			=	{0};
	
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0,
						iActReceivedLen					=	0;
	
	double				lfReceivedValue					=	0.0;
	
	unsigned int		iChIndex						=	0;
	
	iChIndex = iPSAddress-1;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	IF (((iPSAddress < 1) || (iPSAddress > ptLambdaVariable->iChanelsQuantity)), "Illegal parameter.", MODULE_NAME);
	
	iWriteBufLen = ptLambdaVariable->pLamdaChannel[iChIndex].iAddrBufLen;
		
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)ptLambdaVariable->pLamdaChannel[iChIndex].szChAddress, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	iWriteBufLen = sprintf ( szWriteBuffer , ":UVP?;" );
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	CHK_VSA ( viRead ( visaObjHandle, (ViBuf)szReadBuffer, 127, &iActReceivedLen ));
	
	IF ((iActReceivedLen < 1) , "Error receiving data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	sscanf ( szReadBuffer , "UP%lf" , &lfReceivedValue );
	
	*plfVolts = lfReceivedValue; 
	
Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** ***** Set Output State **** ****** ***** ***** ***** ***** *****/

errEV ZUP_SetOutputState ( int handle , int iPSAddress , int iState )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0};
	
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0;
	
	unsigned int		iChIndex						=	0;
	
	iChIndex = iPSAddress-1;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	IF (((iPSAddress < 1) || (iPSAddress > ptLambdaVariable->iChanelsQuantity)), "Illegal parameter.", MODULE_NAME);
	
	iWriteBufLen = ptLambdaVariable->pLamdaChannel[iChIndex].iAddrBufLen;
		
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)ptLambdaVariable->pLamdaChannel[iChIndex].szChAddress, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	if ( iState )
		
		iWriteBufLen = sprintf ( szWriteBuffer , ":OUT1;" );
	
	else
		
		iWriteBufLen = sprintf ( szWriteBuffer , ":OUT0;" );
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** ***** Get Output State **** ****** ***** ***** ***** ***** *****/

errEV ZUP_GetOutputState ( int handle , int iPSAddress , int *piState )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0},
						szReadBuffer[RD_BUF+1]			=	{0};
	
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0,
						iActReceivedLen					=	0;
	
	int					iReceivedValue					=	0;
	
	unsigned int		iChIndex						=	0;
	
	iChIndex = iPSAddress-1;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	IF (((iPSAddress < 1) || (iPSAddress > ptLambdaVariable->iChanelsQuantity)), "Illegal parameter.", MODULE_NAME);
	
	iWriteBufLen = ptLambdaVariable->pLamdaChannel[iChIndex].iAddrBufLen;
		
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)ptLambdaVariable->pLamdaChannel[iChIndex].szChAddress, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	iWriteBufLen = sprintf ( szWriteBuffer , ":OUT?;" );
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	CHK_VSA ( viRead ( visaObjHandle, (ViBuf)szReadBuffer, 127, &iActReceivedLen ));
	
	IF ((iActReceivedLen < 1) , "Error receiving data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	sscanf ( szReadBuffer , "OT%d" , &iReceivedValue );
	
	*piState = iReceivedValue; 
	
Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** ***** Set Foldback Protection Mode **** ****** ***** ***** ***** ***** *****/

errEV ZUP_SetFoldbackProtectionMode ( int handle , int iPSAddress , int iMode )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0};
	
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0;
	
	unsigned int		iChIndex						=	0;
	
	iChIndex = iPSAddress-1;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	IF (((iPSAddress < 1) || (iPSAddress > ptLambdaVariable->iChanelsQuantity)), "Illegal parameter.", MODULE_NAME);
	
	iWriteBufLen = ptLambdaVariable->pLamdaChannel[iChIndex].iAddrBufLen;
		
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)ptLambdaVariable->pLamdaChannel[iChIndex].szChAddress, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	IF (((iMode != FLD_PROTECT_ARM) && ( iMode != FLD_PROTECT_RELEASE ) && ( iMode != FLD_PROTECT_CANCEL )), "Unknown faldback protection." , MODULE_NAME);
	
	iWriteBufLen = sprintf ( szWriteBuffer , ":FLD%d;" , iMode );
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** ***** Get Foldback Protection Mode **** ****** ***** ***** ***** ***** *****/

errEV ZUP_GetFoldbackProtectionMode ( int handle , int iPSAddress , int *piMode )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0},
						szReadBuffer[RD_BUF+1]			=	{0};
	
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0,
						iActReceivedLen					=	0;
	
	int					iReceivedValue					=	0;
	
	unsigned int		iChIndex						=	0;
	
	iChIndex = iPSAddress-1;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	IF (((iPSAddress < 1) || (iPSAddress > ptLambdaVariable->iChanelsQuantity)), "Illegal parameter.", MODULE_NAME);
	
	iWriteBufLen = ptLambdaVariable->pLamdaChannel[iChIndex].iAddrBufLen;
		
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)ptLambdaVariable->pLamdaChannel[iChIndex].szChAddress, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	iWriteBufLen = sprintf ( szWriteBuffer , ":FLD?;" );
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	CHK_VSA ( viRead ( visaObjHandle, (ViBuf)szReadBuffer, 127, &iActReceivedLen ));
	
	IF ((iActReceivedLen < 1) , "Error receiving data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	sscanf ( szReadBuffer , "FD%d" , &iReceivedValue );
	
	*piMode = iReceivedValue; 
	
Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** ***** Set Auto Restart Mode **** ****** ***** ***** ***** ***** *****/

errEV ZUP_SetAutoRestartMode ( int handle , int iPSAddress , int iMode )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0};
	
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0;
	
	unsigned int		iChIndex						=	0;
	
	iChIndex = iPSAddress-1;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	IF (((iPSAddress < 1) || (iPSAddress > ptLambdaVariable->iChanelsQuantity)), "Illegal parameter.", MODULE_NAME);
	
	iWriteBufLen = ptLambdaVariable->pLamdaChannel[iChIndex].iAddrBufLen;
		
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)ptLambdaVariable->pLamdaChannel[iChIndex].szChAddress, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	if ( iMode )
		
		iWriteBufLen = sprintf ( szWriteBuffer , ":AST1;" );
	
	else
		
		iWriteBufLen = sprintf ( szWriteBuffer , ":AST0;" );
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** ***** Get Auto Restart Mode **** ****** ***** ***** ***** ***** *****/

errEV ZUP_GetAutoRestartMode ( int handle , int iPSAddress , int *piMode )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0},
						szReadBuffer[RD_BUF+1]			=	{0};
	
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0,
						iActReceivedLen					=	0;
	
	int					iReceivedValue					=	0;
	
	unsigned int		iChIndex						=	0;
	
	iChIndex = iPSAddress-1;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	IF (((iPSAddress < 1) || (iPSAddress > ptLambdaVariable->iChanelsQuantity)), "Illegal parameter.", MODULE_NAME);
	
	iWriteBufLen = ptLambdaVariable->pLamdaChannel[iChIndex].iAddrBufLen;
		
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)ptLambdaVariable->pLamdaChannel[iChIndex].szChAddress, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	iWriteBufLen = sprintf ( szWriteBuffer , ":AST?;" );
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	CHK_VSA ( viRead ( visaObjHandle, (ViBuf)szReadBuffer, 127, &iActReceivedLen ));
	
	IF ((iActReceivedLen < 1) , "Error receiving data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	sscanf ( szReadBuffer , "AS%d" , &iReceivedValue );
	
	*piMode = iReceivedValue; 
	
Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** Get Operational Status Register ***** ***** ***** ***** ***** ***** *****/

errEV ZUP_GetOperationalStatusRegister ( int handle , int iPSAddress , char *pReturnString )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0},
						szReadBuffer[RD_BUF+1]			=	{0};
	
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0,
						iActReceivedLen					=	0;
	
	char				szReceivedValue[RD_BUF+1]			=	{0};
	
	unsigned int		iChIndex						=	0;
	
	iChIndex = iPSAddress-1;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	IF (((iPSAddress < 1) || (iPSAddress > ptLambdaVariable->iChanelsQuantity)), "Illegal parameter.", MODULE_NAME);
	
	iWriteBufLen = ptLambdaVariable->pLamdaChannel[iChIndex].iAddrBufLen;
		
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)ptLambdaVariable->pLamdaChannel[iChIndex].szChAddress, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	iWriteBufLen = sprintf ( szWriteBuffer , ":STA?;" );
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	CHK_VSA ( viRead ( visaObjHandle, (ViBuf)szReadBuffer, 127, &iActReceivedLen ));
	
	IF ((iActReceivedLen < 1) , "Error receiving data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	sscanf ( szReadBuffer , "OS%s" , szReceivedValue );
	
	strcpy ( pReturnString , szReceivedValue );
	
Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** Get Alarm Status Register ***** ***** ***** ***** ***** ***** *****/

errEV ZUP_GetAlarmStatusRegister ( int handle , int iPSAddress , char *pReturnString )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0},
						szReadBuffer[RD_BUF+1]			=	{0};
	
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0,
						iActReceivedLen					=	0;
	
	char				szReceivedValue[RD_BUF+1]			=	{0};
	
	unsigned int		iChIndex						=	0;
	
	iChIndex = iPSAddress-1;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	IF (((iPSAddress < 1) || (iPSAddress > ptLambdaVariable->iChanelsQuantity)), "Illegal parameter.", MODULE_NAME);
	
	iWriteBufLen = ptLambdaVariable->pLamdaChannel[iChIndex].iAddrBufLen;
		
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)ptLambdaVariable->pLamdaChannel[iChIndex].szChAddress, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	iWriteBufLen = sprintf ( szWriteBuffer , ":ALM?;" );
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	CHK_VSA ( viRead ( visaObjHandle, (ViBuf)szReadBuffer, 127, &iActReceivedLen ));
	
	IF ((iActReceivedLen < 1) , "Error receiving data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	sscanf ( szReadBuffer , "AL%s" , szReceivedValue );
	
	strcpy ( pReturnString , szReceivedValue );
	
Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** Get Error Codes Register ***** ***** ***** ***** ***** ***** *****/

errEV ZUP_GetErrorCodesRegister ( int handle , int iPSAddress , char *pReturnString )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0},
						szReadBuffer[RD_BUF+1]			=	{0};
	
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0,
						iActReceivedLen					=	0;
	
	char				szReceivedValue[RD_BUF+1]			=	{0};
	
	unsigned int		iChIndex						=	0;
	
	iChIndex = iPSAddress-1;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	IF (((iPSAddress < 1) || (iPSAddress > ptLambdaVariable->iChanelsQuantity)), "Illegal parameter.", MODULE_NAME);
	
	iWriteBufLen = ptLambdaVariable->pLamdaChannel[iChIndex].iAddrBufLen;
		
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)ptLambdaVariable->pLamdaChannel[iChIndex].szChAddress, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	iWriteBufLen = sprintf ( szWriteBuffer , ":STP?;" );
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	CHK_VSA ( viRead ( visaObjHandle, (ViBuf)szReadBuffer, 127, &iActReceivedLen ));
	
	IF ((iActReceivedLen < 1) , "Error receiving data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	sscanf ( szReadBuffer , "PS%s" , szReceivedValue );
	
	strcpy ( pReturnString , szReceivedValue );
	
Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/***** ***** ***** ***** ***** ***** Get Complete Status ***** ***** ***** ***** ***** ***** *****/

errEV ZUP_GetCompleteStatus ( int handle , int iPSAddress , char *pReturnString )
{
	errEV				ret								=	{0};  	
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	ViSession  			visaObjHandle					=	0;
	
	char				szWriteBuffer[WR_BUF]			=	{0},
						szReadBuffer[RD_BUF+1]			=	{0};
	
	ViUInt32			iWriteBufLen					=	0,
						iActSentLen						=	0,
						iActReceivedLen					=	0;
	
	unsigned int		iChIndex						=	0;
	
	iChIndex = iPSAddress-1;
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	visaObjHandle = ptLambdaVariable->InstrumentHandle;
	
	IF (((iPSAddress < 1) || (iPSAddress > ptLambdaVariable->iChanelsQuantity)), "Illegal parameter.", MODULE_NAME);
	
	iWriteBufLen = ptLambdaVariable->pLamdaChannel[iChIndex].iAddrBufLen;
		
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)ptLambdaVariable->pLamdaChannel[iChIndex].szChAddress, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	iWriteBufLen = sprintf ( szWriteBuffer , ":STT?;" );
	
	CHK_VSA ( viWrite ( visaObjHandle, (ViBuf)szWriteBuffer, iWriteBufLen, &iActSentLen ));
	
	IF ((iWriteBufLen != iActSentLen) , "Error sending data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	CHK_VSA ( viRead ( visaObjHandle, (ViBuf)szReadBuffer, 127, &iActReceivedLen ));
	
	IF ((iActReceivedLen < 1) , "Error receiving data." , MODULE_NAME);
	
	Delay (PS_COM_DELAY);
	
	strcpy ( pReturnString , szReadBuffer );
	
Error:	
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/**********************************************************************************/

