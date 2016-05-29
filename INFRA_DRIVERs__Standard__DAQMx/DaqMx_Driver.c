//==============================================================================
// Include files
#include "cvidef.h" 
#include "Windows.h"
#include "toolbox.h"
#include <ansi_c.h>
#include <userint.h>
#include "GlobalDefines.h" 
#include <formatio.h>
#include <analysis.h>
#include <utility.h>
#include "DaqMx_Driver.h"
#include <NIDAQmx.h>

//==============================================================================
// Constants

//==============================================================================
// Types

typedef struct
{
	TaskHandle						hTaskHandle_IN,
									hTaskHandle_OUT,
									hTaskHandle_OUT_IN;
	
	teDigitalConnectionSizeType		connect_type;
	
	tsDaqmxPortConfigItem			config;
	
	teDAQMX_ConfigTypes				type;
	
	int								bBroakenData,
									bMultiPortUsing;
	
} tsDaqMX_StoreMemory;


//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

int DLLEXPORT DAQMX_GetErrorMessage( int error , char *pszDescription )
{
	STD_ERROR					StdError					=	{0};

	if ( pszDescription == NULL )
	{
		CALLOC_ERR( pszDescription , LOW_STRING , sizeof(char));	
		
		CHK_DAQMX( DAQmxGetErrorString ( error , pszDescription , LOW_STRING ));
	}
	
Error:
	
	return StdError.error;
}


void*   DLLEXPORT		DAQMX_Close( int *pHandle ) 
{
 	STD_ERROR        			StdError							=	{0};
	
	tsDaqMX_StoreMemory		*pDaqMXstore				=	NULL;
	
	int						Handle						=	0;
	
	IF (( pHandle == NULL ) , "Function DAQMX_Close receive empty handle."  ); 
	
	Handle = *pHandle;
	
	IF (( Handle == 0 ) , "Function DAQMX_Close receive empty handle."  );   
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pDaqMXstore ))
		
	if ( pDaqMXstore->hTaskHandle_OUT ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(pDaqMXstore->hTaskHandle_OUT);
		DAQmxClearTask(pDaqMXstore->hTaskHandle_OUT);
	}

	if ( pDaqMXstore->hTaskHandle_IN ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(pDaqMXstore->hTaskHandle_IN);
		DAQmxClearTask(pDaqMXstore->hTaskHandle_IN);
	}
	
	if ( pDaqMXstore->hTaskHandle_OUT_IN ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(pDaqMXstore->hTaskHandle_OUT_IN);
		DAQmxClearTask(pDaqMXstore->hTaskHandle_OUT_IN);
	}
		
Error:

	if ( Handle )
	{
		CmtReleaseTSVPtr (Handle);
		CmtDiscardTSV (Handle);
		*pHandle=0;
	}
	
	RETURN_STDERR_POINTER;	
}

#define			LIST_BUFFER_SIZE			10e3	

void*   DLLEXPORT		DAQMX_Init( teDAQMX_ConfigTypes type , char *pszDeviceName , tsDaqmxPortConfigItem ConfigBuffer , int *pHandle )
{
	STD_ERROR        			StdError															=	{0};
	
	CmtTSVHandle 			VariableHandle												=	0;
	
	tsDaqMX_StoreMemory		*pDaqMXstore												=	NULL;
	
	char					szDevicesList[STD_STRING]									=	{0},
							*pPortsList													=	NULL,
							*pLinesList													=	NULL,
							szDeviceName[128]											=	{0}, 
							szCurrentPort[128]											=	{0}, 
							szCurrentLine[128]											=	{0},
							*pCurrentValue												=	NULL,
							*pNextValue													=	NULL,
							*pSeparator													=	NULL;


	unsigned int			portIndex													=	0,
							iLineIndex													=	0, 
							iPortIndex													=	0;
								
	int						iMeasureType												=	0,
							iBitsCount													=	0,
							iBitIndex													=	0,
							iByteIndex													=	0;
							
	int						bBroakenData												=	0,
							bMultiPortUsing												=	0;
	
	unsigned char			UseBitsMap[8]												=	{0},
							BitDirectionMask[8]											=	{0};
	
	char					szConnectionString[2048]									=	{0};
	
	IF (( ConfigBuffer.pPortNumbersArray == NULL ) , "No any port defined"  );     
	IF (( ConfigBuffer.iPortArraySize == 0 ) , "No any port defined"  );   
		 
	IF (( pHandle == NULL ) , "Function DAQMX_Init receive NULL poiner of handle."  );
						  
	if ( *pHandle == 0 )
	{
		CHK_CMT( CmtNewTSV ( sizeof(tsDaqMX_StoreMemory) , &VariableHandle ));
	}
	else
		VariableHandle = *pHandle; 
	
	*pHandle = VariableHandle;
	
	CHK_CMT( CmtGetTSVPtr ( VariableHandle , &pDaqMXstore ));
	
	memcpy( &pDaqMXstore->config , &ConfigBuffer , sizeof(tsDaqmxPortConfigItem));

	CHK_DAQMX ( DAQmxGetSystemInfoAttribute ( DAQmx_Sys_DevNames , szDevicesList , STD_STRING ));
	
	StringUpperCase (szDevicesList);
	StringUpperCase (pszDeviceName); 
	
	pSeparator = strchr(szDevicesList,',');
	
	if ( pszDeviceName )
	{
		if ( strstr( szDevicesList , pszDeviceName ) )
			strcpy( szDeviceName , pszDeviceName );
	}
	else
	{
		if ( pSeparator )
			*pSeparator = 0;
			
		strcpy( szDeviceName , szDevicesList );
	}
	
	IF (( strlen( szDeviceName ) == 0 ) , "No DAQmx device found."  ); 
	
	pDaqMXstore->type = type;
		
	iMeasureType = ConfigBuffer.iMeasureType;
	
	bBroakenData = 0;
	bMultiPortUsing = 0;
	
	for( iBitIndex = 0 ; iBitIndex < 64 ; iBitIndex++ ) 
		if (( ConfigBuffer.UseBitsMap >> iBitIndex ) & 0x01 )
		{
			iBitsCount++;
			
			if ( bBroakenData )
				bBroakenData = 1;
		}
		else
			if ( !bBroakenData )
				bBroakenData = -1;
				
	if ( bBroakenData < 0 )
		bBroakenData = 0;
	
	pDaqMXstore->bBroakenData = bBroakenData;
	
	//---------- Analyzing Bits Map ------------------//
	for( iByteIndex = 0 ; iByteIndex < 8 ; iByteIndex++ )
	{
		UseBitsMap[iByteIndex] = (char) ( ConfigBuffer.UseBitsMap >> ( iByteIndex * 8 ));	
		BitDirectionMask[iByteIndex] = (char) ( ConfigBuffer.BitDirectionMask >> ( iByteIndex * 8 ));
		
		if ( UseBitsMap[iByteIndex] )
			bMultiPortUsing++;
	}
	
	if ( bMultiPortUsing > 1 )
		bMultiPortUsing = 1;
	else
		bMultiPortUsing = 0;
	
	pDaqMXstore->bMultiPortUsing = bMultiPortUsing;
		
	switch(type)
	{
		case DAQMX_CONFIG_TYPE_ANALOG_CURRENT_IN:
		/*********************************************/
		// DAQmx Configure Analog Input Code
		/*********************************************/
		CHK_DAQMX (DAQmxCreateTask("",&(pDaqMXstore->hTaskHandle_IN)));
		
		CALLOC_ERR( pPortsList , LIST_BUFFER_SIZE , 1 );  
		
		CHK_DAQMX ( DAQmxGetDeviceAttribute ( szDeviceName , DAQmx_Dev_AI_PhysicalChans , pPortsList , (ssize_t)LIST_BUFFER_SIZE ));
		
		//------------ Find Needed port -------------------------//
		pNextValue = strstr( pPortsList, szDeviceName );
			
		for ( portIndex = 0 ; portIndex <= ConfigBuffer.pPortNumbersArray[0] ; portIndex++ )
		{
			if ( pNextValue == NULL )
				break;
			
			pSeparator = strchr(pNextValue,',');
			
			if ( pSeparator )
				*pSeparator = 0; 
			
			pCurrentValue = pNextValue;
			
			if ( pSeparator )
				pNextValue = pSeparator+1;
			else
				pNextValue = NULL;	
						
			if ( pNextValue )
				if ( strstr( pNextValue , szDeviceName ) == NULL )
				{
					SET_ERR(-1,"port is not exists");
					break;
				}
		}
	
		if ( pCurrentValue )
			strcpy ( szCurrentPort , pCurrentValue );
		
		IF ( ( strlen( szCurrentPort ) == 0 ) , "No channels found."  );   	
		
		CHK_DAQMX (DAQmxCreateAICurrentChan (pDaqMXstore->hTaskHandle_IN, szCurrentPort,"", iMeasureType , 0.0 , 1.0 ,DAQmx_Val_Amps , DAQmx_Val_External , 1.0 , "" ));
		
		break;
		
		case DAQMX_CONFIG_TYPE_ANALOG_VOLT_IN:
		/*********************************************/
		// DAQmx Configure Analog Input Code
		/*********************************************/
		CHK_DAQMX (DAQmxCreateTask("",&(pDaqMXstore->hTaskHandle_IN )));
		
		CALLOC_ERR( pPortsList , LIST_BUFFER_SIZE , 1 );  
		
		CHK_DAQMX ( DAQmxGetDeviceAttribute ( szDeviceName , DAQmx_Dev_AI_PhysicalChans , pPortsList , (ssize_t)LIST_BUFFER_SIZE ));
		
		//------------ Find Needed port -------------------------//
		pNextValue = strstr( pPortsList, szDeviceName );
			
		for ( portIndex = 0 ; portIndex <= ConfigBuffer.pPortNumbersArray[0] ; portIndex++ )
		{
			if ( pNextValue == NULL )
				break;
			
			pSeparator = strchr(pNextValue,',');
			
			if ( pSeparator )
				*pSeparator = 0; 
			
			pCurrentValue = pNextValue;
			
			if ( pSeparator )
				pNextValue = pSeparator+1;
			else
				pNextValue = NULL;	
						
			if ( pNextValue )
				if ( strstr( pNextValue , szDeviceName ) == NULL )
				{
					SET_ERR(-1,"port is not exists");
					break;
				}
		}
	
		if ( pCurrentValue )
			strcpy ( szCurrentPort , pCurrentValue );
		
		IF ( ( strlen( szCurrentPort ) == 0 ) , "No channels found."  );   	
		
		CHK_DAQMX (DAQmxCreateAIVoltageChan(pDaqMXstore->hTaskHandle_IN, szCurrentPort,"", iMeasureType , pDaqMXstore->config.lfMinValue , pDaqMXstore->config.lfMaxValue , DAQmx_Val_Volts , ""));   
		
		break;
		
		case DAQMX_CONFIG_TYPE_ANALOG_VOLT_OUT:
		
		/*********************************************/
		// DAQmx Configure Analog Output Code
		/*********************************************/
		CHK_DAQMX (DAQmxCreateTask("",&(pDaqMXstore->hTaskHandle_OUT)));
		
		CALLOC_ERR( pPortsList , LIST_BUFFER_SIZE , 1 );
		
		CHK_DAQMX ( DAQmxGetDeviceAttribute ( szDeviceName , DAQmx_Dev_AO_PhysicalChans , pPortsList , (ssize_t)LIST_BUFFER_SIZE ));
		
		//------------ Find Needed port -------------------------//
		pNextValue = strstr( pPortsList, szDeviceName );
			
		for ( portIndex = 0 ; portIndex <= ConfigBuffer.pPortNumbersArray[0] ; portIndex++ )
		{
			if ( pNextValue == NULL )
				break;
			
			pSeparator = strchr(pNextValue,',');
			
			if ( pSeparator )
				*pSeparator = 0; 
			
			pCurrentValue = pNextValue;
			
			if ( pSeparator )
				pNextValue = pSeparator+1;
			else
				pNextValue = NULL;	
			
			if ( pNextValue )
				if ( strstr( pNextValue , szDeviceName ) == NULL )
				{
					SET_ERR(-1,"port is not exists");
					break;
				}
		}
	
		if ( pCurrentValue )
			strcpy ( szCurrentPort , pCurrentValue );
		
		IF ( ( strlen( szCurrentPort ) == 0 ) , "No channels found."  );   
		
		SetBreakOnLibraryErrors (0);
		
		if ( ConfigBuffer.bPowerUpState )
		{
			DAQmxSetAnalogPowerUpStates ( szDeviceName , szCurrentPort , ConfigBuffer.dPowerUpLevel , DAQmx_Val_ChannelVoltage ,  NULL );
		}
		
		SetBreakOnLibraryErrors (1);  
		
		CHK_DAQMX (DAQmxCreateAOVoltageChan( pDaqMXstore->hTaskHandle_OUT , szCurrentPort , "" , pDaqMXstore->config.lfMinValue , pDaqMXstore->config.lfMaxValue ,DAQmx_Val_Volts , "" ));  
		
		break;
		
		case DAQMX_CONFIG_TYPE_DIGITAL:
		
		/*********************************************/
		// DAQmx Configure Digital Output Code
		/*********************************************/
		CALLOC_ERR( pPortsList , LIST_BUFFER_SIZE , 1 );
		CALLOC_ERR( pLinesList , LIST_BUFFER_SIZE , 1 );
			
		CHK_DAQMX ( DAQmxGetDeviceAttribute ( szDeviceName , DAQmx_Dev_DO_Ports , pPortsList , (ssize_t)LIST_BUFFER_SIZE ));
		
		CHK_DAQMX ( DAQmxGetDeviceAttribute ( szDeviceName , DAQmx_Dev_DO_Lines , pLinesList , (ssize_t)LIST_BUFFER_SIZE ));
				
		for ( iPortIndex = 0 ; iPortIndex < ConfigBuffer.iPortArraySize; iPortIndex++ )
			if( UseBitsMap[iPortIndex] && BitDirectionMask[iPortIndex] )
			{
				if ( pDaqMXstore->hTaskHandle_OUT == NULL )
				{
					CHK_DAQMX (DAQmxCreateTask("",&(pDaqMXstore->hTaskHandle_OUT)));  
					CHK_DAQMX (DAQmxCreateTask("",&(pDaqMXstore->hTaskHandle_OUT_IN)));
				}
				
				sprintf( szCurrentPort , "%s/port%d" , szDeviceName , ConfigBuffer.pPortNumbersArray[iPortIndex] );
				
				if ( strstr( pPortsList , szCurrentPort ) == NULL )
				{
					SET_ERR(-1,"port is not exists");
					break;
				}
				
				IF ( ( strlen( szCurrentPort ) == 0 ) , "No channels found."  );   

				//if (( iBitsCount != 1 ) && (( bBroakenData && ( bMultiPortUsing == 0 )) || (( UseBitsMap[iPortIndex] == 0x0FF ) && ( BitDirectionMask[iPortIndex] == 0x0FF )))) 
				if (( UseBitsMap[iPortIndex] == 0x0FF ) && ( BitDirectionMask[iPortIndex] == 0x0FF )) 
				{
					if ( strlen(szConnectionString))
						strcat( szConnectionString , ", ");
				
					strcat( szConnectionString , szCurrentPort);
				
					switch(iPortIndex)
					{
						case 0:
							pDaqMXstore->connect_type = DIGITAL_CONNECT_8_BIT;
							break;
						case 1:
							pDaqMXstore->connect_type = DIGITAL_CONNECT_16_BIT;
							break;
						case 2:
						case 3:
							pDaqMXstore->connect_type = DIGITAL_CONNECT_32_BIT;
							break;
						case 4:
							pDaqMXstore->connect_type = DIGITAL_CONNECT_40_BIT;
							break;
						case 5:
							pDaqMXstore->connect_type = DIGITAL_CONNECT_48_BIT;
							break;
						case 6:
						case 7:
							pDaqMXstore->connect_type = DIGITAL_CONNECT_64_BIT;
							break;
					}
				
					continue;
				}
				
				for ( iLineIndex = 0 ; iLineIndex < 8 ; iLineIndex++ )  
					if (( UseBitsMap[iPortIndex] >> iLineIndex ) & 0x01 )
						//if (( BitDirectionMask[iPortIndex] >> iLineIndex ) & 0x01 )   	
						{
							sprintf( szCurrentLine , "%s/line%d" , szCurrentPort , iLineIndex ); 
					
							if ( strstr( pLinesList , szCurrentLine ) == NULL )
								break;
						
							if ( strlen(szConnectionString))
								strcat( szConnectionString , ", ");
						
							strcat( szConnectionString , szCurrentLine);
						
							switch(iPortIndex)
							{
								case 0:
									pDaqMXstore->connect_type = DIGITAL_CONNECT_8_BIT;
									break;
								case 1:
									pDaqMXstore->connect_type = DIGITAL_CONNECT_16_BIT;
									break;
								case 2:
								case 3:
									pDaqMXstore->connect_type = DIGITAL_CONNECT_32_BIT;
									break;
								case 4:
									pDaqMXstore->connect_type = DIGITAL_CONNECT_40_BIT;
									break;
								case 5:
									pDaqMXstore->connect_type = DIGITAL_CONNECT_48_BIT;
									break;
								case 6:
								case 7:
									pDaqMXstore->connect_type = DIGITAL_CONNECT_64_BIT;
									break;
							}
						}
				
				if ( strstr( pLinesList , szCurrentLine ) == NULL )
				{
					SET_ERR(-1,"line is not exists");
					break;
				}
			}
			
			
		IF (( strlen( szConnectionString ) == 0 ) , "No Connections found"  );
			
		if ( iBitsCount == 1 )
		{			
			pDaqMXstore->connect_type = DIGITAL_CONNECT_1_BIT;
				
			CHK_DAQMX (DAQmxCreateDOChan( pDaqMXstore->hTaskHandle_OUT , szConnectionString , "" , DAQmx_Val_ChanPerLine ));   
			CHK_DAQMX (DAQmxCreateDIChan( pDaqMXstore->hTaskHandle_OUT_IN , szConnectionString , "" , DAQmx_Val_ChanPerLine ));   
		}
		else
		{
			CHK_DAQMX (DAQmxCreateDOChan( pDaqMXstore->hTaskHandle_OUT , szConnectionString , "" , DAQmx_Val_ChanForAllLines ));   
			CHK_DAQMX (DAQmxCreateDIChan( pDaqMXstore->hTaskHandle_OUT_IN , szConnectionString , "" , DAQmx_Val_ChanForAllLines ));   
			
		}

		SetBreakOnLibraryErrors (0);

		if ( ConfigBuffer.bPullingState )
		{
			DAQmxSetDigitalPullUpPullDownStates ( szDeviceName , szConnectionString , DAQmx_Val_PullUp , NULL );
		}
		else
		{
			DAQmxSetDigitalPullUpPullDownStates ( szDeviceName , szConnectionString , DAQmx_Val_PullDown , NULL );
		}

		if ( ConfigBuffer.bPowerUpState )
		{
			DAQmxSetDigitalPowerUpStates ( szDeviceName , szConnectionString , DAQmx_Val_High , NULL );
	
			if ( ConfigBuffer.dPowerUpLevel > 3.5 )
				DAQmxSetDigitalLogicFamilyPowerUpState ( szDeviceName , DAQmx_Val_5V ); 
			else
				if ( ConfigBuffer.dPowerUpLevel < 3.0 )
					DAQmxSetDigitalLogicFamilyPowerUpState ( szDeviceName , DAQmx_Val_2point5V ); 
				else
					DAQmxSetDigitalLogicFamilyPowerUpState ( szDeviceName , DAQmx_Val_3point3V );  
			
		}
		else
		{
			DAQmxSetDigitalPowerUpStates ( szDeviceName , szConnectionString , DAQmx_Val_Low , NULL );
		}

		SetBreakOnLibraryErrors (1);

		
		/*********************************************/
		// DAQmx Configure Digital Input Code
		/*********************************************/
		
		CHK_DAQMX ( DAQmxGetDeviceAttribute ( szDeviceName , DAQmx_Dev_DI_Ports , pPortsList , (ssize_t)LIST_BUFFER_SIZE ));
		
		CHK_DAQMX ( DAQmxGetDeviceAttribute ( szDeviceName , DAQmx_Dev_DI_Lines , pLinesList , (ssize_t)LIST_BUFFER_SIZE ));  
		
		memset( szConnectionString , 0 , 2048 );
		
		for ( iPortIndex = 0 ; iPortIndex < ConfigBuffer.iPortArraySize; iPortIndex++ )
			if( UseBitsMap[iPortIndex] && ((~BitDirectionMask[iPortIndex]) & UseBitsMap[iPortIndex]))
			{
				if ( pDaqMXstore->hTaskHandle_IN == NULL )
				{
					CHK_DAQMX (DAQmxCreateTask("",&(pDaqMXstore->hTaskHandle_IN)));
				}
				
				sprintf( szCurrentPort , "%s/port%d" , szDeviceName , ConfigBuffer.pPortNumbersArray[iPortIndex] );
				
				if ( strstr( pPortsList , szCurrentPort ) == NULL )
				{
					SET_ERR(-1,"port is not exists");
					break;
				}
				
				IF ( ( strlen( szCurrentPort ) == 0 ) , "No channels found."  );   
		
				//if (( iBitsCount != 1 ) && (( bBroakenData && ( bMultiPortUsing == 0 )) || (( UseBitsMap[iPortIndex] == 0x0FF ) && ( BitDirectionMask[iPortIndex] == 0x0FF )))) 
				if (( UseBitsMap[iPortIndex] == 0x0FF ) && ( BitDirectionMask[iPortIndex] == 0 )) 
				{
					if ( strlen(szConnectionString))
						strcat( szConnectionString , ", ");
				
					strcat( szConnectionString , szCurrentPort);
				
					switch(iPortIndex)
					{
						case 0:
							pDaqMXstore->connect_type = DIGITAL_CONNECT_8_BIT;
							break;
						case 1:
							pDaqMXstore->connect_type = DIGITAL_CONNECT_16_BIT;
							break;
						case 2:
						case 3:
							pDaqMXstore->connect_type = DIGITAL_CONNECT_32_BIT;
							break;
						case 4:
							pDaqMXstore->connect_type = DIGITAL_CONNECT_40_BIT;
							break;
						case 5:
							pDaqMXstore->connect_type = DIGITAL_CONNECT_48_BIT;
							break;
						case 6:
						case 7:
							pDaqMXstore->connect_type = DIGITAL_CONNECT_64_BIT;
							break;
					}
				
					continue;
				}
				
				for ( iLineIndex = 0 ; iLineIndex < 8 ; iLineIndex++ )  
					if (( UseBitsMap[iPortIndex] >> iLineIndex ) & 0x01 )
						if ((( BitDirectionMask[iPortIndex] >> iLineIndex ) & 0x01 ) == 0 )  	
						{
							sprintf( szCurrentLine , "%s/line%d" , szCurrentPort , iLineIndex ); 
					
							if ( strstr( pLinesList , szCurrentLine ) == NULL )
								break;
						
							if ( strlen(szConnectionString))
								strcat( szConnectionString , ", ");
						
							strcat( szConnectionString , szCurrentLine);
						
							switch(iPortIndex)
							{
								case 0:
									pDaqMXstore->connect_type = DIGITAL_CONNECT_8_BIT;
									break;
								case 1:
									pDaqMXstore->connect_type = DIGITAL_CONNECT_16_BIT;
									break;
								case 2:
								case 3:
									pDaqMXstore->connect_type = DIGITAL_CONNECT_32_BIT;
									break;
								case 4:
									pDaqMXstore->connect_type = DIGITAL_CONNECT_40_BIT;
									break;
								case 5:
									pDaqMXstore->connect_type = DIGITAL_CONNECT_48_BIT;
									break;
								case 6:
								case 7:
									pDaqMXstore->connect_type = DIGITAL_CONNECT_64_BIT;
									break;
							}
						}
				
				if ( strstr( pLinesList , szCurrentLine ) == NULL )
				{
					SET_ERR(-1,"line is not exists");
					break;
				}
			}
			
			if ( strlen( szConnectionString ))
			{
				if ( iBitsCount == 1 )
				{			
					pDaqMXstore->connect_type = DIGITAL_CONNECT_1_BIT;
					
					CHK_DAQMX (DAQmxCreateDIChan( pDaqMXstore->hTaskHandle_IN , szConnectionString , "" , DAQmx_Val_ChanPerLine ));   
				}
				else
				{
					CHK_DAQMX (DAQmxCreateDIChan( pDaqMXstore->hTaskHandle_IN , szConnectionString , "" , DAQmx_Val_ChanForAllLines ));   
				}

				SetBreakOnLibraryErrors (0);
	
				if ( ConfigBuffer.bPullingState )
				{
					DAQmxSetDigitalPullUpPullDownStates ( szDeviceName , szConnectionString , DAQmx_Val_PullUp , NULL );
				}
				else
				{
					DAQmxSetDigitalPullUpPullDownStates ( szDeviceName , szConnectionString , DAQmx_Val_PullDown , NULL );
				}
	
				if ( ConfigBuffer.bPowerUpState )
				{
					DAQmxSetDigitalPowerUpStates ( szDeviceName , szConnectionString , DAQmx_Val_High , NULL );
		
					if ( ConfigBuffer.dPowerUpLevel > 3.5 )
						DAQmxSetDigitalLogicFamilyPowerUpState ( szDeviceName , DAQmx_Val_5V ); 
					else
						if ( ConfigBuffer.dPowerUpLevel < 3.0 )
							DAQmxSetDigitalLogicFamilyPowerUpState ( szDeviceName , DAQmx_Val_2point5V ); 
						else
							DAQmxSetDigitalLogicFamilyPowerUpState ( szDeviceName , DAQmx_Val_3point3V );  
				
				}
				else
				{
					DAQmxSetDigitalPowerUpStates ( szDeviceName , szConnectionString , DAQmx_Val_Low , NULL );
				}
			}
			
			SetBreakOnLibraryErrors (1);
		
			break;
		
		case DAQMX_CONFIG_TYPE_COUNTER:
		
		break;
		
		default:
			break;

	};

Error:

	if (pPortsList)
		free(pPortsList);
	
	if (pLinesList)
		free(pLinesList);
	
	if ( VariableHandle )
		CmtReleaseTSVPtr (VariableHandle);

	FREE( ConfigBuffer.pPortNumbersArray );
	
	RETURN_STDERR_POINTER;	
}

void*   DLLEXPORT		DAQMX_SetUniversal( int Handle , unsigned __int64 iValue ) 
{
 	STD_ERROR        			StdError							=	{0};
	
	tsDaqMX_StoreMemory		*pDaqMXstore				=	NULL;
	
	int						count						=	0,
							index						=	0,
							iBitCounter					=	0;
	
	unsigned char			singleByte					=	0,
							setByte						=	0;
	
	unsigned short			singleWByte					=	0,
							setWByte					=	0; 
		
	unsigned int			singleWWord					=	0,
							setWWord					=	0; 
	
	unsigned __int64 		iNewValue					=	0;
	
	IF (( Handle == 0 ) , "Function DAQMX_SetPort receive empty handle."  ); 
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pDaqMXstore ))
		
	IF (( pDaqMXstore->type != DAQMX_CONFIG_TYPE_DIGITAL )  , "Function DAQMX_SetPort receive wrong handle."  ); 
		
	//----------- convert value to bitmap -----------------------//
	if ( pDaqMXstore->bBroakenData )
		for ( index = 0; index < ( sizeof(__int64) * 8 ); index++ )
			if (( pDaqMXstore->config.UseBitsMap >> index ) & 0x01 )
				if (( iValue >> iBitCounter++ ) & 0x01 )
					iNewValue |= 0x01 << index;
	
	if ( iNewValue )
		iValue = iNewValue;
					
	switch( pDaqMXstore->connect_type )
	{
		case DIGITAL_CONNECT_1_BIT:
			
			if ( pDaqMXstore->hTaskHandle_OUT ) 
			{
				if ( iValue )
					setByte = 1;
				
				CHK_DAQMX ( DAQmxWriteDigitalLines ( pDaqMXstore->hTaskHandle_OUT , 1 , 1 , 10.0 , DAQmx_Val_GroupByChannel , &setByte , &count , 0));

				CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_OUT )); 
			}
			break;
		case DIGITAL_CONNECT_8_BIT:
			
			//if ( pDaqMXstore->hTaskHandle_OUT_IN ) 
			//{
			//	CHK_DAQMX ( DAQmxStartTask ( pDaqMXstore->hTaskHandle_OUT_IN ));  
			//
			//	CHK_DAQMX ( DAQmxReadDigitalU8 ( pDaqMXstore->hTaskHandle_OUT_IN  , DAQmx_Val_Auto, 10.0, DAQmx_Val_GroupByChannel , &singleByte , 1 , &count , 0 )); 
			//
			//	CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_OUT_IN )); 
			//
			//}
	
			setByte = ((~(((char)pDaqMXstore->config.UseBitsMap) & (~((char)pDaqMXstore->config.BitDirectionMask))) & singleByte ) | (char)iValue );
	
			if ( pDaqMXstore->hTaskHandle_OUT ) 
			{
				CHK_DAQMX ( DAQmxWriteDigitalU8 (pDaqMXstore->hTaskHandle_OUT, 1, 1, 10.0,DAQmx_Val_GroupByChannel, &setByte, &count, 0));

				CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_OUT )); 
	
			}
			break;
			
		case DIGITAL_CONNECT_16_BIT:
			
			//if ( pDaqMXstore->hTaskHandle_OUT_IN ) 
			//{
			//	CHK_DAQMX ( DAQmxStartTask ( pDaqMXstore->hTaskHandle_OUT_IN ));  
			//
			//	CHK_DAQMX ( DAQmxReadDigitalU16 ( pDaqMXstore->hTaskHandle_OUT_IN  , DAQmx_Val_Auto, 10.0, DAQmx_Val_GroupByChannel , &singleWByte , 1 , &count , 0 )); 
			//
			//	CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_OUT_IN )); 
			//
			//}
	
			setWByte = ((~(((short)pDaqMXstore->config.UseBitsMap) & (~((short)pDaqMXstore->config.BitDirectionMask))) & singleWByte ) | (short)iValue );
	
			if ( pDaqMXstore->hTaskHandle_OUT ) 
			{
				CHK_DAQMX ( DAQmxWriteDigitalU16 (pDaqMXstore->hTaskHandle_OUT, 1, 1, 10.0,DAQmx_Val_GroupByChannel, &setWByte, &count, 0));

				CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_OUT )); 
	
			}
			break;
		case DIGITAL_CONNECT_32_BIT:
			
			//if ( pDaqMXstore->hTaskHandle_OUT_IN ) 
			//{
			//	CHK_DAQMX ( DAQmxStartTask ( pDaqMXstore->hTaskHandle_OUT_IN ));  
			//
			//	CHK_DAQMX ( DAQmxReadDigitalU32 ( pDaqMXstore->hTaskHandle_OUT_IN  , DAQmx_Val_Auto, 10.0, DAQmx_Val_GroupByChannel , &singleWWord , 1 , &count , 0 )); 
			//
			//	CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_OUT_IN )); 
			//
			//}
	
			setWWord = ((~(((int)pDaqMXstore->config.UseBitsMap) & (~((int)pDaqMXstore->config.BitDirectionMask))) & singleWWord ) | (int)iValue );
	
			if ( pDaqMXstore->hTaskHandle_OUT ) 
			{
				CHK_DAQMX ( DAQmxWriteDigitalU32 (pDaqMXstore->hTaskHandle_OUT, 1, 1, 10.0,DAQmx_Val_GroupByChannel, &setWWord, &count, 0));

				CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_OUT )); 
	
			}
			break;
		case DIGITAL_CONNECT_40_BIT:
			break;
		case DIGITAL_CONNECT_48_BIT:
			break;
		case DIGITAL_CONNECT_64_BIT:
			break;
			
		default:
			
			FORCE_ERR(-1,"Wrong Digital Connection Type");
	}
		
Error:

	if ( Handle )
		CmtReleaseTSVPtr (Handle);
	
	RETURN_STDERR_POINTER;	
}


void*   DLLEXPORT		DAQMX_GetUniversal( int Handle , unsigned __int64 *piValue ) 
{
 	STD_ERROR        			StdError							=	{0};
	
	tsDaqMX_StoreMemory		*pDaqMXstore				=	NULL;
	
	int						count						=	0,
							quantity					=	0, 
							index						=	0,
							iBitCounter					=	0;
	
	unsigned char			singleByte					=	0;
	
	unsigned short			singleWByte					=	0; 
		
	unsigned int			singleWWord					=	0; 
	
	unsigned __int64 		iGetValue					=	0,
							iNewValue					=	0;

	unsigned __int64		UseBitsMap					=	0;
					  
	IF (( Handle == 0 ) , "Function DAQMX_SetPort receive empty handle."  ); 
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pDaqMXstore ))
		
	IF (( pDaqMXstore->type != DAQMX_CONFIG_TYPE_DIGITAL )  , "Function DAQMX_SetPort receive wrong handle."  ); 
		
	switch( pDaqMXstore->connect_type )
	{
		case DIGITAL_CONNECT_1_BIT:
			
			if ( pDaqMXstore->hTaskHandle_IN ) 
			{
				CHK_DAQMX ( DAQmxReadDigitalLines (pDaqMXstore->hTaskHandle_IN, 1, 10.0, DAQmx_Val_GroupByChannel , &singleByte , 1 , &count, &quantity , 0));

				CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_IN )); 
				
				iGetValue = singleByte;
				
			}
			
			break;
		case DIGITAL_CONNECT_8_BIT:
			
			if ( pDaqMXstore->hTaskHandle_IN ) 
			{
				CHK_DAQMX ( DAQmxStartTask ( pDaqMXstore->hTaskHandle_IN ));  
		
				CHK_DAQMX ( DAQmxReadDigitalU8 ( pDaqMXstore->hTaskHandle_IN  , DAQmx_Val_Auto, 10.0, DAQmx_Val_GroupByChannel , &singleByte , 1 , &count , 0 )); 
		
				CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_IN )); 
		
				iGetValue = singleByte;
			}

			break;
			
		case DIGITAL_CONNECT_16_BIT:
			
			if ( pDaqMXstore->hTaskHandle_IN ) 
			{
				CHK_DAQMX ( DAQmxStartTask ( pDaqMXstore->hTaskHandle_IN ));  
		
				CHK_DAQMX ( DAQmxReadDigitalU16 ( pDaqMXstore->hTaskHandle_IN  , DAQmx_Val_Auto, 10.0, DAQmx_Val_GroupByChannel , &singleWByte , 1 , &count , 0 )); 
		
				CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_IN )); 
		
				iGetValue = singleWByte;
			}
			break;
		case DIGITAL_CONNECT_32_BIT:
			
			if ( pDaqMXstore->hTaskHandle_IN ) 
			{
				CHK_DAQMX ( DAQmxStartTask ( pDaqMXstore->hTaskHandle_IN ));  
		
				CHK_DAQMX ( DAQmxReadDigitalU32 ( pDaqMXstore->hTaskHandle_IN  , DAQmx_Val_Auto, 10.0, DAQmx_Val_GroupByChannel , &singleWWord , 1 , &count , 0 )); 
		
				CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_IN )); 
		
				iGetValue = singleWWord; 
			}
			
			break;
		case DIGITAL_CONNECT_40_BIT:
			break;
		case DIGITAL_CONNECT_48_BIT:
			break;
		case DIGITAL_CONNECT_64_BIT:
			break;
			
		default:
			
			FORCE_ERR(-1,"Wrong Digital Connection Type");
	}
	
	UseBitsMap = pDaqMXstore->config.UseBitsMap;
	
		//----------- convert value to bitmap -----------------------//
	if ( pDaqMXstore->bBroakenData )
		for ( index = 0; index < ( sizeof(__int64) * 8 ); index++ )
			if (( UseBitsMap >> index ) & 0x01 )
				if (( iGetValue >> index ) & 0x01 )
					iNewValue |= 0x01 << iBitCounter++;
	
	if ( iNewValue )
		iGetValue = iNewValue;
					
Error:
	
	if ( piValue )
		*piValue = iGetValue;
	
	if ( Handle )
		CmtReleaseTSVPtr (Handle);
	
	RETURN_STDERR_POINTER;	
}


void*   DLLEXPORT		DAQMX_SetDigitalBit( int Handle , unsigned char bState ) 
{
 	STD_ERROR        			StdError							=	{0};
	
	tsDaqMX_StoreMemory		*pDaqMXstore				=	NULL;
	
	int						count						=	0;
	
	IF (( Handle == 0 ) , "Function DAQMX_SetDigitalBit receive empty handle."  ); 
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pDaqMXstore ))
		
	IF (( pDaqMXstore->type != DAQMX_CONFIG_TYPE_DIGITAL )  , "Function DAQMX_SetDigitalBit receive wrong handle."  ); 
		
	if ( pDaqMXstore->hTaskHandle_OUT ) 
	{
		CHK_DAQMX ( DAQmxWriteDigitalLines ( pDaqMXstore->hTaskHandle_OUT , 1 , 1 , 10.0 , DAQmx_Val_GroupByChannel , &bState , &count , 0));

		CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_OUT )); 
	}
	
Error:

	if ( Handle )
		CmtReleaseTSVPtr (Handle);
	
	RETURN_STDERR_POINTER;	
}


void*   DLLEXPORT		DAQMX_GetDigitalBit( int Handle , unsigned char *pbState ) 
{
 	STD_ERROR        			StdError							=	{0};
	
	tsDaqMX_StoreMemory		*pDaqMXstore				=	NULL;
	
	int						count						=	0,
							quantity					=	0;
	
	IF (( Handle == 0 ) , "Function DAQMX_GetDigitalBit receive empty handle."  ); 
	IF (( pbState == NULL ) , "Function DAQMX_GetDigitalBit receive NULL Pointer."  );  
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pDaqMXstore ))
	
	IF (( pDaqMXstore->type != DAQMX_CONFIG_TYPE_DIGITAL )  , "Function DAQMX_GetDigitalBit receive wrong handle."  ); 

	if ( pDaqMXstore->hTaskHandle_IN ) 
	{

		CHK_DAQMX ( DAQmxReadDigitalLines (pDaqMXstore->hTaskHandle_IN, 1, 10.0, DAQmx_Val_GroupByChannel , pbState , 1 , &count, &quantity , 0));

		CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_IN )); 
	
	}

	
Error:
	
	if ( Handle )
		CmtReleaseTSVPtr (Handle);

	RETURN_STDERR_POINTER;	
}


void*   DLLEXPORT		DAQMX_SetPort( int Handle , unsigned char byte ) 
{
 	STD_ERROR        			StdError							=	{0};
	
	tsDaqMX_StoreMemory		*pDaqMXstore				=	NULL;
	
	int						count						=	0;
	
	unsigned char			singleByte					=	0,
							setByte						=	0;
	
	IF (( Handle == 0 ) , "Function DAQMX_SetPort receive empty handle."  ); 
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pDaqMXstore ))
		
	IF (( pDaqMXstore->type != DAQMX_CONFIG_TYPE_DIGITAL )  , "Function DAQMX_SetPort receive wrong handle."  ); 
		
	//if ( pDaqMXstore->hTaskHandle_OUT_IN ) 
	//{
	//	CHK_DAQMX ( DAQmxStartTask ( pDaqMXstore->hTaskHandle_OUT_IN ));  
	//	
	//	CHK_DAQMX ( DAQmxReadDigitalU8 ( pDaqMXstore->hTaskHandle_OUT_IN  , DAQmx_Val_Auto, 10.0, DAQmx_Val_GroupByChannel , &singleByte , 1 , &count , 0 )); 
	//	
	//	CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_OUT_IN )); 
	//	
	//}
	
	setByte = ((~(((char)pDaqMXstore->config.UseBitsMap) & (~((char)pDaqMXstore->config.BitDirectionMask))) & singleByte ) | byte );
	
	if ( pDaqMXstore->hTaskHandle_OUT ) 
	{
		CHK_DAQMX ( DAQmxWriteDigitalU8 (pDaqMXstore->hTaskHandle_OUT, 1, 1, 10.0,DAQmx_Val_GroupByChannel, &setByte, &count, 0));

		CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_OUT )); 
	
	}

	
Error:

	if ( Handle )
		CmtReleaseTSVPtr (Handle);
	
	RETURN_STDERR_POINTER;	
}

void*   DLLEXPORT	DAQMX_GetPort( int Handle , unsigned char *pByte ) 
{
 	STD_ERROR        			StdError							=	{0};
	
	tsDaqMX_StoreMemory		*pDaqMXstore				=	NULL;
	
	int						count						=	0;
	
	unsigned char			singleByte					=	0;
	
	IF (( Handle == 0 ) , "Function DAQMX_SetPort receive empty handle."  ); 
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pDaqMXstore ))
		
	IF (( pDaqMXstore->type != DAQMX_CONFIG_TYPE_DIGITAL )  , "Function DAQMX_SetPort receive wrong handle."  ); 
		
	if ( pDaqMXstore->hTaskHandle_IN ) 
	{
		CHK_DAQMX ( DAQmxStartTask ( pDaqMXstore->hTaskHandle_IN ));  
		
		CHK_DAQMX ( DAQmxReadDigitalU8 ( pDaqMXstore->hTaskHandle_IN  , DAQmx_Val_Auto, 10.0, DAQmx_Val_GroupByChannel , &singleByte , 1 , &count , 0 )); 
		
		CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_IN )); 
		
	}

	if ( pByte )
		*pByte = ( singleByte & ( (char)pDaqMXstore->config.UseBitsMap & (char)pDaqMXstore->config.BitDirectionMask ));
	
Error:

	if ( Handle )
		CmtReleaseTSVPtr (Handle);
	
	RETURN_STDERR_POINTER;	
}

void*   DLLEXPORT	DAQMX_SetAnalogLine( int Handle , double value ) 
{
 	STD_ERROR        			StdError							=	{0};
	
	tsDaqMX_StoreMemory		*pDaqMXstore				=	NULL;
	
	unsigned long			bTaskDone					=	0; 

	
	IF (( Handle == 0 ) , "Function DAQMX_SetAnalogLine receive empty handle."  ); 
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pDaqMXstore ))
		
	IF (( pDaqMXstore->type != DAQMX_CONFIG_TYPE_ANALOG_VOLT_OUT )  , "Function DAQMX_SetPort receive wrong handle."  ); 

	if ( pDaqMXstore->hTaskHandle_OUT ) 
	{
		DAQmxIsTaskDone ( pDaqMXstore->hTaskHandle_OUT , &bTaskDone ); 
		
		if ( bTaskDone == 0 ) 
			DAQmxStopTask ( pDaqMXstore->hTaskHandle_OUT ); 
		
		CHK_DAQMX ( DAQmxWriteAnalogScalarF64 (pDaqMXstore->hTaskHandle_OUT , 1 , 2.0 , value , 0 ));
		
		CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_OUT )); 
	
	}

	
Error:

	if ( Handle )
		CmtReleaseTSVPtr (Handle);
	
	RETURN_STDERR_POINTER;	
}

void*   DLLEXPORT	DAQMX_GetAnalogLine( int Handle , double *pGetValue ) 
{
 	STD_ERROR        			StdError							=	{0};
	
	tsDaqMX_StoreMemory		*pDaqMXstore				=	NULL;
	
	double 					GetValue					=	0;
	
	unsigned long			bTaskDone					=	0; 
	
	IF (( Handle == 0 ) , "Function DAQMX_SetAnalogLine receive empty handle."  ); 
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pDaqMXstore ))
		
	IF ((( pDaqMXstore->type != DAQMX_CONFIG_TYPE_ANALOG_CURRENT_IN ) && ( pDaqMXstore->type != DAQMX_CONFIG_TYPE_ANALOG_VOLT_IN ))  , "Function DAQMX_SetPort receive wrong handle."  ); 

	if ( pDaqMXstore->hTaskHandle_IN ) 
	{
		DAQmxIsTaskDone ( pDaqMXstore->hTaskHandle_IN , &bTaskDone ); 
		
		if ( bTaskDone == 0 ) 
			DAQmxStopTask ( pDaqMXstore->hTaskHandle_IN ); 
		
		CHK_DAQMX ( DAQmxStartTask ( pDaqMXstore->hTaskHandle_IN ));
		
		CHK_DAQMX ( DAQmxReadAnalogScalarF64 (pDaqMXstore->hTaskHandle_IN , 10.0 , &GetValue , 0));
		
		CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_IN )); 
	
	}

	if ( pGetValue )
		*pGetValue = GetValue;
	
Error:

	if ( Handle )
		CmtReleaseTSVPtr (Handle);
	
	RETURN_STDERR_POINTER;	
}


void*   DLLEXPORT	DAQMX_GetAnalogLineAvarage( int Handle , unsigned int number , double *pGetValue ) 
{
 	STD_ERROR        			StdError							=	{0};
	
	tsDaqMX_StoreMemory		*pDaqMXstore				=	NULL;
	
	int						count						=	0,
							index						=	0; 
								
	double 					*pGetArrayValue				=	NULL,
							lfAverage					=	0.0,
							lfValue						=	0.0;

	unsigned long			bTaskDone					=	0; 
	
	IF (( Handle == 0 ) , "Function DAQMX_GetAnalogLineAvarage receive empty handle."  ); 
	IF (( number == 0 ) , "Function DAQMX_GetAnalogLineAvarage receive empty number."  );  
	
	pGetArrayValue = calloc(number,sizeof(double));
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pDaqMXstore ));
		
	IF ( (( pDaqMXstore->type != DAQMX_CONFIG_TYPE_ANALOG_CURRENT_IN ) && ( pDaqMXstore->type != DAQMX_CONFIG_TYPE_ANALOG_VOLT_IN ))  , "Function DAQMX_SetPort receive wrong handle."  ); 

	if ( pDaqMXstore->hTaskHandle_IN ) 
	{
		DAQmxIsTaskDone ( pDaqMXstore->hTaskHandle_IN , &bTaskDone ); 
		
		if ( bTaskDone == 0 ) 
			DAQmxStopTask ( pDaqMXstore->hTaskHandle_IN ); 
		
		CHK_DAQMX ( DAQmxStartTask ( pDaqMXstore->hTaskHandle_IN ));
		
		CHK_DAQMX ( DAQmxReadAnalogF64 ( pDaqMXstore->hTaskHandle_IN , number , 10.0 , DAQmx_Val_GroupByChannel , pGetArrayValue ,  number , &count , 0 ));
			
		lfAverage = 0.0;
			
		for ( index=0 ; index < count ; index++ )
			lfAverage += pGetArrayValue[index];
		
		if ( count )
			lfValue = lfAverage/count;
			
		CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_IN )); 
	
	}

	if ( pGetValue )
		*pGetValue = lfValue;
	
Error:

	if ( pGetArrayValue )
		free(pGetArrayValue);
	
	if ( Handle )
		CmtReleaseTSVPtr (Handle);
	
	RETURN_STDERR_POINTER;	
}


void*   DLLEXPORT	DAQMX_GetDigitalPulseCharacters( int Handle , int numberOfSimples , double lfTimeout , double *plfAmplitude , double *plfWidth , double *plfStart ) 
{
 	STD_ERROR        			StdError							=	{0};
	
	tsDaqMX_StoreMemory		*pDaqMXstore				=	NULL;
	
	int						count						=	0,
							index						=	0; 
								
	uInt8 					*pArrayValue				=	NULL;
	
	int						numBytesPerSamp				=	0;
	
	double 					*pPulseArray				=	NULL, 
							lfAmplitude					=	0.0,  
							lfTemp						=	0.0;
	
	ssize_t					Width						=	0,
							Start						=	0,
							temp						=	0;
				
	double					lfStartTime					=	0.0,
							lfStopTime					=	0.0;

	IF (( Handle == 0 ) , "Function DAQMX_GetAnalogLineAvarage receive empty handle."  ); 
	
	pArrayValue = calloc(numberOfSimples,sizeof(uInt8));
	pPulseArray = calloc(numberOfSimples,sizeof(double));  
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pDaqMXstore ))
		
	IF (( pDaqMXstore->type != DAQMX_CONFIG_TYPE_DIGITAL )  , "Function DAQMX_SetPort receive wrong handle."  ); 

	if ( pDaqMXstore->hTaskHandle_IN ) 
	{
		CHK_DAQMX ( DAQmxStartTask ( pDaqMXstore->hTaskHandle_IN ));
		
		SetBreakOnLibraryErrors (0);
		
		GetCurrentDateTime(&lfStartTime);
		DAQmxReadDigitalLines( pDaqMXstore->hTaskHandle_IN, numberOfSimples , lfTimeout , DAQmx_Val_GroupByChannel, pArrayValue , numberOfSimples , &count , &numBytesPerSamp , 0 );
		GetCurrentDateTime(&lfStopTime); 
		
		for ( index=0 ; index < count ; index++ )  
			pPulseArray[index] = (double)pArrayValue[index];
			
		PulseParam (pPulseArray, count, &lfTemp, &lfTemp, &lfTemp, &lfTemp, &lfAmplitude, &lfTemp, &lfTemp, &lfTemp, &Start , &Width,&temp, &temp, &lfTemp);
		
		SetBreakOnLibraryErrors (1);             
		
		if ( plfAmplitude )
			*plfAmplitude = lfAmplitude;
			
		if ( plfWidth )
			*plfWidth = (Width * ((lfStopTime-lfStartTime)/(double)count));
			
		if ( plfStart )
			*plfStart = (Start * ((lfStopTime-lfStartTime)/(double)count));    
		
			
		CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_IN )); 
	
	}

Error:

	if ( pArrayValue )
		free(pArrayValue);
	
	if ( pPulseArray )
		free(pPulseArray);
	
	if ( Handle )
		CmtReleaseTSVPtr (Handle);
	
	RETURN_STDERR_POINTER;	
}



void*   DLLEXPORT	DAQMX_GetDigitalBitArray( int Handle , int numberOfSimples , double lfTimeout , unsigned char **pArrayValue , int *pCount ) 
{
 	STD_ERROR        			StdError							=	{0};
	
	tsDaqMX_StoreMemory		*pDaqMXstore				=	NULL;
	
	int						numBytesPerSamp				=	0;
	
	IF (( Handle == 0 ) , "Function DAQMX_GetDigitalBitArray receive empty handle."  ); 
	IF (( pArrayValue  == NULL ) , "Function DAQMX_GetDigitalBitArray receive NULL pointer."  ); 
	IF (( pCount == NULL ) , "Function DAQMX_GetDigitalBitArray receive NULL pointer."  );  
	
	if ( *pArrayValue )
	{
		free(*pArrayValue);
	}
		
	*pArrayValue = calloc(numberOfSimples,sizeof(uInt8));
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pDaqMXstore ))
		
	IF (( pDaqMXstore->type != DAQMX_CONFIG_TYPE_DIGITAL )  , "Function DAQMX_SetPort receive wrong handle."  ); 

	if ( pDaqMXstore->hTaskHandle_IN ) 
	{
		CHK_DAQMX ( DAQmxStartTask ( pDaqMXstore->hTaskHandle_IN ));

		DAQmxReadDigitalLines( pDaqMXstore->hTaskHandle_IN, numberOfSimples , lfTimeout , DAQmx_Val_GroupByChannel, *pArrayValue , numberOfSimples , pCount , &numBytesPerSamp , 0 );

		CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_IN )); 
	
	}

Error:
	
	if ( Handle )
		CmtReleaseTSVPtr (Handle);
	
	RETURN_STDERR_POINTER;	
}



void*   DLLEXPORT	DAQMX_GetAnalogLineWave( int Handle , double lfTimeOut , unsigned int number , unsigned long long SampsPerChannel , double **plfArray ) 
{
 	STD_ERROR        			StdError							=	{0};
	
	tsDaqMX_StoreMemory		*pDaqMXstore				=	NULL;
	
	int						count						=	0;
	
	unsigned long			bTaskDone					=	0; 
	
	double					lfRate						=	0,
							lfSamleRate					=	0; 
							
	IF (( Handle == 0 ) , "Function DAQMX_GetAnalogLineWave receive empty handle."  ); 
	IF (( number == 0 ) , "Function DAQMX_GetAnalogLineWave receive empty number."  );  
	IF (( plfArray == NULL ) , "Function DAQMX_GetAnalogLineWave receive NULL Pointer of Array."  ); 
	
	if ( *plfArray )
		free(*plfArray);
	
	*plfArray = calloc(number,sizeof(double));
	
	IF (( *plfArray == NULL ) , "Can't allocate memory."  );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pDaqMXstore ));
		
	IF ( (( pDaqMXstore->type != DAQMX_CONFIG_TYPE_ANALOG_CURRENT_IN ) && ( pDaqMXstore->type != DAQMX_CONFIG_TYPE_ANALOG_VOLT_IN ))  , "Function DAQMX_SetPort receive wrong handle."  ); 

	if ( pDaqMXstore->hTaskHandle_IN ) 
	{
		DAQmxIsTaskDone ( pDaqMXstore->hTaskHandle_IN , &bTaskDone ); 
		
		if ( bTaskDone == 0 ) 
			DAQmxStopTask ( pDaqMXstore->hTaskHandle_IN ); 
		
		if ( SampsPerChannel > 0.0 ) 
		{
			lfSamleRate = SampsPerChannel;
			
			CHK_DAQMX ( DAQmxSetTimingAttribute( pDaqMXstore->hTaskHandle_IN , DAQmx_SampClk_Rate , lfSamleRate ));
		}
		
		CHK_DAQMX ( DAQmxGetTimingAttribute( pDaqMXstore->hTaskHandle_IN , DAQmx_SampClk_Rate , &lfRate ));
		
		CHK_DAQMX ( DAQmxCfgSampClkTiming (pDaqMXstore->hTaskHandle_IN, "", lfRate, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, number));
		
		CHK_DAQMX ( DAQmxStartTask ( pDaqMXstore->hTaskHandle_IN ));
		
		CHK_DAQMX ( DAQmxReadAnalogF64 ( pDaqMXstore->hTaskHandle_IN , number , lfTimeOut , DAQmx_Val_GroupByChannel , *plfArray ,  number , &count , 0 ));
			
		CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_IN )); 
	
	}

Error:

	if ( Handle )
		CmtReleaseTSVPtr (Handle);
	
	RETURN_STDERR_POINTER;	
}


void*   DLLEXPORT	DAQMX_SetAnalogLineWave( int Handle , int bContinues , double lfFrequency , unsigned int number , double *plfArray ) 
{
 	STD_ERROR        			StdError							=	{0};
	
	tsDaqMX_StoreMemory		*pDaqMXstore				=	NULL;
	
	int						count						=	0;
	
	double					Rate						=	0;
	
	unsigned long			bTaskDone					=	0; 
	
	IF (( Handle == 0 ) , "Function DAQMX_GetAnalogLineWave receive empty handle."  ); 
	IF (( number == 0 ) , "Function DAQMX_GetAnalogLineWave receive empty number."  );  
	IF (( plfArray == NULL ) , "Function DAQMX_GetAnalogLineWave receive NULL Pointer of Array."  ); 
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pDaqMXstore ));
		
	IF ( (( pDaqMXstore->type != DAQMX_CONFIG_TYPE_ANALOG_VOLT_OUT ))  , "Function DAQMX_SetPort receive wrong handle."  ); 

	if ( pDaqMXstore->hTaskHandle_OUT ) 
	{  
		DAQmxIsTaskDone ( pDaqMXstore->hTaskHandle_OUT , &bTaskDone ); 
		
		if ( bTaskDone == 0 ) 
			DAQmxStopTask ( pDaqMXstore->hTaskHandle_OUT ); 
		
		Rate = number * lfFrequency;
		
		CHK_DAQMX (DAQmxSetTimingAttribute( pDaqMXstore->hTaskHandle_OUT , DAQmx_SampClk_Rate , Rate ));
		CHK_DAQMX (DAQmxGetTimingAttribute( pDaqMXstore->hTaskHandle_OUT , DAQmx_SampClk_Rate , &Rate ));

		if ( bContinues )
		{
			CHK_DAQMX ( DAQmxCfgSampClkTiming( pDaqMXstore->hTaskHandle_OUT , "" , Rate , DAQmx_Val_Rising , DAQmx_Val_ContSamps , 1000 ));
		}
		else
		{
			CHK_DAQMX ( DAQmxCfgSampClkTiming( pDaqMXstore->hTaskHandle_OUT , "" , Rate , DAQmx_Val_Rising , DAQmx_Val_FiniteSamps , 1000 ));  
		}
		
		CHK_DAQMX ( DAQmxWriteAnalogF64 (pDaqMXstore->hTaskHandle_OUT, number, 0, 10.0 , DAQmx_Val_GroupByChannel, plfArray, &count, 0));
		
		CHK_DAQMX ( DAQmxStartTask ( pDaqMXstore->hTaskHandle_OUT )); 
	}

Error:

	if ( Handle )
		CmtReleaseTSVPtr (Handle);
	
	RETURN_STDERR_POINTER;	
}

void*   DLLEXPORT	DAQMX_DoneTask( int Handle ) 
{
 	STD_ERROR        			StdError							=	{0};
	
	tsDaqMX_StoreMemory		*pDaqMXstore				=	NULL;
	
	unsigned long			bTaskDone					=	0; 
	
	IF (( Handle == 0 ) , "Function DAQMX_GetAnalogLineWave receive empty handle."  ); 
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pDaqMXstore ));
	
	if ( pDaqMXstore->hTaskHandle_OUT )
	{
		DAQmxIsTaskDone ( pDaqMXstore->hTaskHandle_OUT , &bTaskDone ); 
		
		if ( bTaskDone == 0 ) 
			DAQmxStopTask ( pDaqMXstore->hTaskHandle_OUT ); 
	}
	
	if ( pDaqMXstore->hTaskHandle_OUT_IN )
	{
		DAQmxIsTaskDone ( pDaqMXstore->hTaskHandle_OUT_IN , &bTaskDone ); 
		
		if ( bTaskDone == 0 ) 
			DAQmxStopTask ( pDaqMXstore->hTaskHandle_OUT_IN ); 
	}
	
	if ( pDaqMXstore->hTaskHandle_IN ) 
	{
		DAQmxIsTaskDone ( pDaqMXstore->hTaskHandle_IN , &bTaskDone ); 
		
		if ( bTaskDone == 0 ) 
			DAQmxStopTask ( pDaqMXstore->hTaskHandle_IN ); 
	}
		

Error:

	if ( Handle )
		CmtReleaseTSVPtr (Handle);
	
	RETURN_STDERR_POINTER;	
}

void*   DLLEXPORT	DAQMX_ConfigSampleRate( int Handle , double lfSampleRate ) 
{
 	STD_ERROR        			StdError							=	{0};
	
	tsDaqMX_StoreMemory		*pDaqMXstore				=	NULL;
	
	unsigned long			bTaskDone					=	0;
	
	IF (( Handle == 0 ) , "Function DAQMX_GetAnalogLineWave receive empty handle."  ); 
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pDaqMXstore ));
	
	if ( pDaqMXstore->hTaskHandle_OUT )
	{
		DAQmxIsTaskDone ( pDaqMXstore->hTaskHandle_OUT , &bTaskDone ); 
		
		if ( bTaskDone == 0 ) 
			DAQmxStopTask ( pDaqMXstore->hTaskHandle_OUT ); 
		
		CHK_DAQMX ( DAQmxSetTimingAttribute( pDaqMXstore->hTaskHandle_OUT , DAQmx_SampClk_Rate , lfSampleRate )); 
	}
	
	if ( pDaqMXstore->hTaskHandle_OUT_IN ) 
	{
		DAQmxIsTaskDone ( pDaqMXstore->hTaskHandle_OUT_IN , &bTaskDone ); 
		
		if ( bTaskDone == 0 ) 
			DAQmxStopTask ( pDaqMXstore->hTaskHandle_OUT_IN ); 
		
		CHK_DAQMX ( DAQmxSetTimingAttribute( pDaqMXstore->hTaskHandle_OUT_IN , DAQmx_SampClk_Rate , lfSampleRate ));
	}
	
	if ( pDaqMXstore->hTaskHandle_IN ) 	
	{
		DAQmxIsTaskDone ( pDaqMXstore->hTaskHandle_IN , &bTaskDone ); 
		
		if ( bTaskDone == 0 ) 
			DAQmxStopTask ( pDaqMXstore->hTaskHandle_IN ); 
		
		CHK_DAQMX ( DAQmxSetTimingAttribute( pDaqMXstore->hTaskHandle_IN , DAQmx_SampClk_Rate , lfSampleRate ));
	}	
		

Error:

	if ( Handle )
		CmtReleaseTSVPtr (Handle);
	
	RETURN_STDERR_POINTER;	
}


void*   DLLEXPORT	DAQMX_CalculateTHD( int Handle , double lfTimeOut , unsigned int number , double lfSampleRate , double lfHarmonicFrequency , double *plfPercent_THD , double *plfPercent_THDNoise ) 
{
 	STD_ERROR        			StdError							=	{0};
	
	tsDaqMX_StoreMemory		*pDaqMXstore				=	NULL;
	
	int						count						=	0;
	
	double					*plfArray					=	NULL;
	
	double					Harmonic_Amplitude[10]		=	{0},
							Harmonic_Frequency[10]		=	{0}; 
									
	double					lfRate						=	0.0;
	
	unsigned long			bTaskDone					=	0; 
	
	IF (( Handle == 0 ) , "Function DAQMX_CalculateTHD receive empty handle."  ); 
	IF (( number == 0 ) , "Function DAQMX_CalculateTHD receive empty number."  );  
	
	plfArray = calloc(number,sizeof(double));
	
	IF (( plfArray == NULL ) , "Can't allocate memory."  );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pDaqMXstore ));
		
	IF ( (( pDaqMXstore->type != DAQMX_CONFIG_TYPE_ANALOG_CURRENT_IN ) && ( pDaqMXstore->type != DAQMX_CONFIG_TYPE_ANALOG_VOLT_IN ))  , "Function DAQMX_SetPort receive wrong handle."  ); 

	if ( pDaqMXstore->hTaskHandle_IN ) 
	{
		DAQmxIsTaskDone ( pDaqMXstore->hTaskHandle_IN , &bTaskDone ); 
		
		if ( bTaskDone == 0 ) 
			DAQmxStopTask ( pDaqMXstore->hTaskHandle_IN ); 
		
		if ( lfSampleRate > 0.0 )
		{
			CHK_DAQMX ( DAQmxSetTimingAttribute( pDaqMXstore->hTaskHandle_IN , DAQmx_SampClk_Rate , lfSampleRate ));
			
			CHK_DAQMX ( DAQmxGetTimingAttribute( pDaqMXstore->hTaskHandle_IN , DAQmx_SampClk_Rate , &lfRate ));
		
			CHK_DAQMX ( DAQmxCfgSampClkTiming( pDaqMXstore->hTaskHandle_IN , "" , lfRate , DAQmx_Val_Rising , DAQmx_Val_ContSamps , 1000 )); 
		}
		
		CHK_DAQMX ( DAQmxStartTask ( pDaqMXstore->hTaskHandle_IN ));
		
		CHK_DAQMX ( DAQmxReadAnalogF64 ( pDaqMXstore->hTaskHandle_IN , number , lfTimeOut , DAQmx_Val_GroupByChannel  , plfArray ,  number , &count , 0 ));
			
		CHK_DAQMX ( DAQmxStopTask ( pDaqMXstore->hTaskHandle_IN )); 
	
	}

	HarmonicAnalyzerUsingSignal (plfArray, number, 10 , ( lfRate * number ) , lfHarmonicFrequency , Harmonic_Amplitude , Harmonic_Frequency , plfPercent_THD , plfPercent_THDNoise ); 
	
Error:

	if ( plfArray )
		free(plfArray);
	
	if ( Handle )
		CmtReleaseTSVPtr (Handle);
	
	RETURN_STDERR_POINTER;	
}



void*   DLLEXPORT		DAQMX_GetValueType( int Handle , teDigitalConnectionSizeType *pSizeType ) 
{
 	STD_ERROR        			StdError							=	{0};
	
	tsDaqMX_StoreMemory		*pDaqMXstore				=	NULL;
	
	IF (( Handle == 0 ) , "Function DAQMX_SetPort receive empty handle."  ); 
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &pDaqMXstore ))
		
	if ( pSizeType ) 
		*pSizeType = pDaqMXstore->connect_type;

Error:

	if ( Handle )
		CmtReleaseTSVPtr (Handle);
	
	RETURN_STDERR_POINTER;	
}
/*	
int main (int argc, char *argv[])
{
	STD_ERROR							StdError								=	{0};  

	int								hOut							=	0,
									hIn								=	0;
	
	tsDaqmxPortConfigItem 			ConfigBuffer					=	{0};
		
	double							plfWaveForm[10001]				=	{0},
									phase							=	0.0;
	
	double							//Harmonic_Amplitude[1024]		=	{0},
									//Harmonic_Frequency[1024]		=	{0}, 
									Percent_THD						=	0.0,
									Percent_THDNoise				=	0.0;
		
	//double							*plfArray						=	NULL;
	
    // initialize and load resources //
	CHK_UIL ( InitCVIRTE (0, argv, 0 )); 

	ConfigBuffer.UseBitsMap = 0x0ff;
	CALLOC_COPY( ConfigBuffer.pPortNumbersArray , 10 , 1 , "1" , 1 );
	ConfigBuffer.iPortArraySize = 1;
	
	CHK_STDERR( DAQMX_Init( DAQMX_CONFIG_TYPE_DIGITAL , NULL , ConfigBuffer , &hOut ));     
	
	SineWave (1000, 1.0, 0.006 , &phase , plfWaveForm);  
	
	DAQMX_SetAnalogLineWave( hOut , 1 , 100.0 , 1000 , plfWaveForm );
		
	//DAQMX_GetAnalogLineWave( hIn , 100.0 , 1000 , &plfArray );
		
	//HarmonicAnalyzerUsingSignal (plfArray, 1000, 10 , 1000*1000 , 1000 ,Harmonic_Amplitude, Harmonic_Frequency, &Percent_THD, &Percent_THDNoise); 
	
	DAQMX_CalculateTHD( hIn , 100.0 , 1000 , 400E3 , 600 , &Percent_THD, &Percent_THDNoise ); 

		
Error:

	DAQMX_DoneTask( hOut ); 
		
	DAQMX_Close(&hOut);
	DAQMX_Close(&hIn);  
	
	return 0;
}
 */
