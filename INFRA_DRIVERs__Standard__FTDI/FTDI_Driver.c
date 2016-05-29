//==============================================================================
//
// Title:       FTDI_Driver
// Purpose:     FTDI driver.
//
// Created on:  3/14/2010 at 5:44:09 PM by Dmitry.
// Copyright:   Everest. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files
#include "cvidef.h"  
#include "FTD2XX.H"    
#include "toolbox.h"
#include <userint.h>
#include <ansi_c.h>
#include <utility.h>
#include "FTDI_Driver.h"

#define			MAX_NUMBER_OF_DEVICE_PORTS				16
//==============================================================================
// Types

typedef struct
{
	FT_HANDLE					CommHandle;
	
	teDevicePackages			CommDevice;
	
	teBitModes					CurrentCommMode;

	char						szProtect[64];
	
} tsPort;

typedef struct
{
	tsPort						*Port;
	
	teBitModes					CommMode;   
	
	unsigned char				PortConfig;
	
	unsigned char				PortState;
	
	char						szProtect[64];
	
} tsPortItem;

typedef struct
{
	tsPortItem					vPortList[MAX_NUMBER_OF_DEVICE_PORTS];
	
	tsPort						vRealPortList[MAX_NUMBER_OF_DEVICE_PORTS];   
	
	int							iPortNumber;
	
	int							iDevice_CBUS_Port_Number; 
	
	tsSTD_CallBackSet			tCallbacks;
	
	char						szProtect[64];
	
} tsDriverHandleVar;

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

//====================================================================================================//

int DLLEXPORT FTDI_GetErrorMessage( int error , char *pszDescription )
{
	STD_ERROR					StdError					=	{0};

	if ( pszDescription == NULL )
	{
		CALLOC_ERR( pszDescription , LOW_STRING , sizeof(char));	
	}
	
	switch(error)
	{
		case FT_OK:
			sprintf( pszDescription , "No errors" );
			break;
		case FT_INVALID_HANDLE:
			sprintf( pszDescription , "Invalid handle" );
			break;
		case FT_DEVICE_NOT_FOUND:
			sprintf( pszDescription , "Device not found" );
			break;
		case FT_DEVICE_NOT_OPENED:
			sprintf( pszDescription , "Device not opened" );
			break;
		case FT_IO_ERROR:
			sprintf( pszDescription , "I//O error" );
			break;
		case FT_INSUFFICIENT_RESOURCES:
			sprintf( pszDescription , "Insufficient resources" );
			break;
		case FT_INVALID_PARAMETER:
			sprintf( pszDescription , "Invalid parameter" );
			break;
		case FT_INVALID_BAUD_RATE:
			sprintf( pszDescription , "Invalid baud rate" );
			break;
		case FT_DEVICE_NOT_OPENED_FOR_ERASE:
			sprintf( pszDescription , "Device not opened for erase" );
			break;
		case FT_DEVICE_NOT_OPENED_FOR_WRITE:
			sprintf( pszDescription , "Device not opened for write" );
			break;
		case FT_FAILED_TO_WRITE_DEVICE:
			sprintf( pszDescription , "Failed to write device" );
			break;
		case FT_EEPROM_READ_FAILED:
			sprintf( pszDescription , "EEPROM read failed" );
			break;	
		case FT_EEPROM_WRITE_FAILED:
			sprintf( pszDescription , "EEPROM write failed" );
			break;
		case FT_EEPROM_ERASE_FAILED:
			sprintf( pszDescription , "EEPROM read failed erase failed" );
			break;
		case FT_EEPROM_NOT_PRESENT:
			sprintf( pszDescription , "EEPROM not present" );
			break;
		case FT_EEPROM_NOT_PROGRAMMED:
			sprintf( pszDescription , "EEPROM not programmed" );
			break;
		case FT_INVALID_ARGS:
			sprintf( pszDescription , "Invalid args" );
			break;
		case FT_NOT_SUPPORTED:
			sprintf( pszDescription , "Not supported" );
			break;
		case FT_OTHER_ERROR:
			sprintf( pszDescription , "Other error" );
			break;
		default:
			sprintf( pszDescription , "Undefined error" );
			break;
	}
	
Error:
	
	return StdError.error;
}

/**
 * @brief FTDI_NumberOfDevices
 *
 * @param[out] piNumberOfDevices pointer to int that contains the number of connected devices
 */
void*   DLLEXPORT FTDI_NumberOfDevices( unsigned long *piNumberOfDevices )
{
	STD_ERROR					StdError					=	{0};
	
	CHK_FTDI( FT_CreateDeviceInfoList( piNumberOfDevices ));
	
Error:	
	
	RETURN_STDERR_POINTER;	
}

/**
 * @brief FTDI_GetDeviceDetail
 *
 * Usfull for opened or not devices
 * @see FTDI_GetDeviceInfo
 *
 * @param[in] DeviceNumber (int) - the device number
 * @param[in] DetailType (define int) - the specific detail to get
 * @param[out] pReturnDetail (pointer) - to the geted value
 */
void*   DLLEXPORT FTDI_GetDeviceDetail( unsigned int DeviceNumber , teDetailTypes DetailType , char *pReturnDetail )
{
	STD_ERROR					StdError					=	{0};
	
	unsigned long		numOfDevices			=	0;
	
	DWORD				flagVal					=	0,
						DevType					=	0,
						DevId					=	0,
						DevLocId				=	0;
	char				DevSN[128]				=	{0},
						DevDescription[512]		=	{0};
	FT_HANDLE			DevFtHandle				=	0;
	
	IF( ( pReturnDetail == NULL ) , "Wrong Detail Type" );  
	IF( (( DetailType < 0 ) || ( DetailType >= DTYPE_SENTINEL )) , "Wrong Detail Type" );
	
	// create the device information list 
	
	CHK_FTDI( FT_CreateDeviceInfoList( &numOfDevices ));
	
	IF( ( DeviceNumber > numOfDevices ) , "Wrong Device Number" );
	
	// get the device details
	
	CHK_FTDI( FT_GetDeviceInfoDetail ( DeviceNumber, &flagVal, &DevType, &DevId, &DevLocId, DevSN, DevDescription, &DevFtHandle));
	
	switch ( DetailType )
	{
		case DTYPE_FLAG_VALUE:
			
			sprintf( pReturnDetail , "0x%x" , flagVal ); 
			
			break;	
			
		case DTYPE_DEVICE_TYPE:
			
			sprintf( pReturnDetail , "0x%x" , DevType );
			
			break;
			
		case DTYPE_DEVICE_ID:
			
			sprintf( pReturnDetail , "0x%x" , DevId );
				 
			break;

		case DTYPE_DEVICE_LOCATION_ID:
			
			sprintf( pReturnDetail , "0x%x" , DevLocId );
			
			break;			
			
		case DTYPE_DEVICE_SN:
			
			strcpy ( pReturnDetail , DevSN );
			
			break;	
			
		case DTYPE_DEVICE_DESCRIPTION:
			
			strcpy ( pReturnDetail , DevDescription );
				 
			break;
			
		case DTYPE_DEVICE_FT_HANDLE:
			
			sprintf( pReturnDetail , "0x%x" , DevFtHandle );
			
			break;
	}	

Error:
	
	RETURN_STDERR_POINTER;	
}

/**
 * @brief FTDI_Init
 *
 * @param[in] ConnectByType (define int) - the connection type
 * @param[in] szConnectName (define int) - the cpesific value of a connection type
 * @param[out] pHandle pointer to the opened Handle
 */
void*   DLLEXPORT FTDI_Init( teConnectionTypes ConnectByType , char *szConnectName , int *pHandle )
{
	STD_ERROR					StdError											=	{0};
	
	int							Handle												=	0; 
	
	FT_DEVICE					ftDev												=	0;
	
	FT_HANDLE 					hDeviceHandle										=	0;
	
	FT_HANDLE 					vhDevicePortList[MAX_NUMBER_OF_DEVICE_PORTS]		=	{0};
	
	FT_STATUS					Status												=	0; 
	
	FT_DEVICE_LIST_INFO_NODE 	*pDevicesList										=	NULL;
	
	char						szPortFormatedConnectionName[512]					=	{0},
								szBasicName[512]									=	{0};
	
	tsDriverHandleVar			*psvDriverVar										=	NULL; 
	
	unsigned long				iDeviceQuantity										=	0,
								iDeviceIndex										=	0,
								iDeviceType											=	0,
								iDevicePortCounter									=	0,
								iDevice_CBUS_Port_Number							=	0;
								
	unsigned long				openType											=	0;
	
	int							size												=	0;
	
	long						iLocation											=	0;
	
	FT_PROGRAM_DATA				tProgrammData										=	{0};
	
	char						szManufacturerBuf[32]								=	{0}, 
								szManufacturerIdBuf[16]								=	{0}, 
								szDescriptionBuf[64]								=	{0}, 
								szSerialNumberBuf[16]								=	{0},
								szMessageString[LOW_STRING]							=	{0};

	tsPort						*LastPort											=	NULL;  
	
	//--------- prepare eeprom structure ----------------------//
	tProgrammData.Signature1 = 0x00000000; 
	tProgrammData.Signature2 = 0xffffffff; 
	tProgrammData.Version = 0x00000002;      // EEPROM structure with FT232R extensions 
	tProgrammData.Manufacturer = szManufacturerBuf; 
	tProgrammData.ManufacturerId = szManufacturerIdBuf; 
	tProgrammData.Description = szDescriptionBuf; 
	tProgrammData.SerialNumber = szSerialNumberBuf; 

	IF( ((ConnectByType < 0) || ( ConnectByType >= CTYPE_SENTINEL )) , "Wrong Connection type" );
			
	IF((( ConnectByType != CTYPE_SINGLE_DEVICE )&&( szConnectName == NULL )) , "Wrong input parameter ( ConnectName is NULL )" );   
	
	CHK_FTDI( FT_CreateDeviceInfoList(&iDeviceQuantity));  
	
	CALLOC( pDevicesList , iDeviceQuantity+1 , (sizeof(FT_DEVICE_LIST_INFO_NODE)));
		
	DelayWithEventProcessing(0.5);
	
	CHK_FTDI( FT_GetDeviceInfoList( pDevicesList , &iDeviceQuantity ));
	
	DelayWithEventProcessing(0.5);
	
	IF (( iDeviceQuantity == 0 ) , "No FTDI Devices Found" );
	
	switch ( ConnectByType )
	{
		case CTYPE_SINGLE_DEVICE:
			
			if ( iDeviceQuantity > 1 )
			{
				strcpy( szBasicName , pDevicesList[0].SerialNumber );
				
				size = strlen( szBasicName );
	
				IF (( size == 0 ) , "Can't get devices list" );
				
				if ( szBasicName[size-1] == 'A' )
				{
					szBasicName[size-1] = 0;
					
					do
					{
						sprintf( szPortFormatedConnectionName , "%s%c" , szBasicName , 'A' + iDevicePortCounter );
					
						Status = FT_OpenEx( szPortFormatedConnectionName , FT_OPEN_BY_SERIAL_NUMBER , &(vhDevicePortList[iDevicePortCounter]));  

						if ( Status == FT_OK )
							iDevicePortCounter++;

					}while ( Status == FT_OK );
				
					if ( iDevicePortCounter )
						hDeviceHandle = vhDevicePortList[0];
					else
						CHK_FTDI( FT_DEVICE_NOT_FOUND );
				
					break;
				}
			}
			
			CHK_FTDI( FT_Open( 0 , &hDeviceHandle )); 
			
			break;	
			
		case CTYPE_SN_NUMBER:
			
			openType = FT_OPEN_BY_SERIAL_NUMBER;
			
			if ( FT_OK != FT_OpenEx( szConnectName , FT_OPEN_BY_SERIAL_NUMBER , &hDeviceHandle ))   
			{
				FT_Close(hDeviceHandle);
				
				do
				{
					sprintf( szPortFormatedConnectionName , "%s%c" , szConnectName , 'A' + iDevicePortCounter );
					
					Status = FT_OpenEx( szPortFormatedConnectionName , FT_OPEN_BY_SERIAL_NUMBER , &(vhDevicePortList[iDevicePortCounter]));  

					if ( Status == FT_OK )
						iDevicePortCounter++;

				}while ( Status == FT_OK );
				
				if ( iDevicePortCounter )
					hDeviceHandle = vhDevicePortList[0];
				else
					CHK_FTDI( FT_DEVICE_NOT_FOUND );
					
			}
				 
			break;
			
		case CTYPE_DESCRIPTION:
			
			openType = FT_OPEN_BY_DESCRIPTION;
			
			if ( FT_OK != FT_OpenEx( szConnectName , FT_OPEN_BY_DESCRIPTION , &hDeviceHandle ))   
			{
				FT_Close(hDeviceHandle); 
				
				do
				{
					sprintf( szPortFormatedConnectionName , "%s%c" , szConnectName , 'A' + iDevicePortCounter );
					
					Status = FT_OpenEx( szPortFormatedConnectionName , FT_OPEN_BY_DESCRIPTION , &(vhDevicePortList[iDevicePortCounter]));  

					if ( Status == FT_OK )
						iDevicePortCounter++;

				}while ( Status == FT_OK );
				
				if ( iDevicePortCounter )
					hDeviceHandle = vhDevicePortList[0];
				else
				{
					if ( iDeviceQuantity == 1 )
					{
						CHK_FTDI( FT_Open( 0 , &hDeviceHandle )); 
						
						CHK_FTDI( FT_EE_Read( hDeviceHandle , &tProgrammData ));
						
						if ( strstr( tProgrammData.Description , "FT232" ) || strstr( tProgrammData.Description , "USB UART" ))
						{
							sprintf( szMessageString , "Found device \"%s\"\nDo you want to rename it to :\n \"%s\"?", tProgrammData.Description , szConnectName );
						
							if (ConfirmPopup ("Wrong device", szMessageString))
							{
								strcpy( tProgrammData.Description , szConnectName );
								
								CHK_FTDI( FT_EE_Program( hDeviceHandle , &tProgrammData ));      
							
								MessagePopup("Burn Successful","Please disconnect the device and connect it again\n( Reset device )");
								break;
							}
						}
					}
					
					CHK_FTDI( FT_DEVICE_NOT_FOUND );
				}
			}
			
			break;

		case CTYPE_LOCATION:
			
			openType = FT_OPEN_BY_LOCATION;
			
			iLocation = atoi(szConnectName);
			
			CHK_FTDI( FT_OpenEx((PVOID) iLocation , FT_OPEN_BY_LOCATION , &hDeviceHandle )) ;
				 
			break;			
	}
	
	CHK_FTDI( FT_GetDeviceInfo ( hDeviceHandle, &ftDev, NULL, NULL, NULL, NULL)); 
	
	iDeviceType = -1;
	
	switch ( ftDev )
	{
		case FT_DEVICE_BM:
			
			iDeviceType = DEVICE_BM;
			
			break;
		
		case FT_DEVICE_AM:
			
			iDeviceType = DEVICE_AM;
			
			break;
		
		case FT_DEVICE_100AX:
			
			iDeviceType = DEVICE_100AX;
			
			break;
		
		case FT_DEVICE_UNKNOWN:
			
			iDeviceType = DEVICE_UNKNOWN;
			
			break;
		
		case FT_DEVICE_2232C:
			
			iDeviceType = DEVICE_2232C;
			
			break;
		
		case FT_DEVICE_232R:
			
			iDeviceType = DEVICE_232R;
			
			iDevice_CBUS_Port_Number = iDevicePortCounter + 1;
			
			break;
			
		case FT_DEVICE_2232H:
			
			iDeviceType = DEVICE_2232H;
	
			break; 
			
		case FT_DEVICE_4232H:
			
			iDeviceType = DEVICE_4232H;
				
			break; 
			
		default:
			break; 
	
	}
	
	if ( *pHandle == 0 )
	{
		CHK_CMT( CmtNewTSV ( sizeof(tsDriverHandleVar) , pHandle ));
	}
	
	Handle = *pHandle;
	
	if ( CmtGetTSVPtr ( Handle , &psvDriverVar  ) == 0 )
	{
		if ( iDevicePortCounter ) 
		{
			psvDriverVar->iPortNumber = iDevicePortCounter; 
			psvDriverVar->iDevice_CBUS_Port_Number = iDevice_CBUS_Port_Number;
			
			for( iDeviceIndex=0; iDeviceIndex < iDevicePortCounter ; iDeviceIndex++ ) 
			{
				psvDriverVar->vRealPortList[iDeviceIndex].CommHandle = vhDevicePortList[iDeviceIndex];
				psvDriverVar->vRealPortList[iDeviceIndex].CommDevice = iDeviceType;
				
				psvDriverVar->vPortList[iDeviceIndex].Port = &(psvDriverVar->vRealPortList[iDeviceIndex]);
				
				LastPort = &(psvDriverVar->vRealPortList[iDeviceIndex]);  
			}
			
			hDeviceHandle = vhDevicePortList[0];  
	
		}
		else
		{
			psvDriverVar->iPortNumber = 1;
			psvDriverVar->vRealPortList[0].CommHandle  = hDeviceHandle;
			psvDriverVar->vRealPortList[0].CommDevice = iDeviceType;
			
			psvDriverVar->vPortList[0].Port = &(psvDriverVar->vRealPortList[0]);  
			LastPort = &(psvDriverVar->vRealPortList[0]); 
			iDeviceIndex++;
		}
			
		if ( iDevice_CBUS_Port_Number > 0 )
		{
			psvDriverVar->iPortNumber++; 
			
			psvDriverVar->iDevice_CBUS_Port_Number = psvDriverVar->iPortNumber;
			
			psvDriverVar->vPortList[iDeviceIndex].Port = LastPort;
		}
		
		CmtReleaseTSVPtr ( Handle );
	}
	
Error:	
	
	FREE(pDevicesList);
	
	RETURN_STDERR_POINTER;	
}
  
/**
 * @brief FTDI_Close
 *
 * @param[in] Handle (int) - handle
 */
void*   DLLEXPORT FTDI_Close( int *pHandle )
{
	STD_ERROR					StdError													=	{0};
	
	int						Handle												=	0;
	
	int						iDeviceIndex										=	0;
	
	tsDriverHandleVar		*psvDriverVar										=	NULL; 
	
	IF (( pHandle == NULL ) , "Handle Can't be NULL" );
	
	Handle = *pHandle;
	
	IF (( Handle == 0 ) , "Empty Handle" );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &psvDriverVar  ));
	
	for( iDeviceIndex=0; iDeviceIndex < psvDriverVar->iPortNumber ; iDeviceIndex++ ) 
		if ( psvDriverVar->vRealPortList[iDeviceIndex].CommHandle )
		{
			FT_Close( psvDriverVar->vRealPortList[iDeviceIndex].CommHandle );
			psvDriverVar->vRealPortList[iDeviceIndex].CommHandle = 0;
		}
	
Error:	
	
	if( Handle ) 
	{ 
		CmtReleaseTSVPtr ( Handle );
		CmtDiscardTSV ( Handle );
	}
		
	RETURN_STDERR_POINTER;	
}

/**
 * @brief FTDI_GetDeviceInfo
 *
 * Usfull only for opened devices
 * @see FTDI_GetDeviceDetail
 *
 * @param[in] Handle (int) - handle 
 * @param[in] InfoType (define int) - the specific info to get
 * @param[out] pReturnInfo pointer to the geted value
 */
void*   DLLEXPORT FTDI_GetDeviceInfo( int Handle , teInfoTypes InfoType , char *pReturnInfo )
{
	STD_ERROR					StdError													=	{0};
	
	FT_DEVICE				ftDev												=	0;
	DWORD					DevId												=	0;
	char					DevSN[128]											=	{0},
							DevDescription[512]									=	{0};
	
	int						index												=	0;
	
	tsDriverHandleVar		*psvDriverVar										=	NULL; 
	
	IF (( Handle == 0 ) , "Empty Handle" );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &psvDriverVar  ));
	
	IF( ((InfoType < 0) || ( InfoType >= ITYPE_SENTINEL )) , "Wrong Info Type" );
	
	// get the device info
	
	CHK_FTDI( FT_GetDeviceInfo ( psvDriverVar->vRealPortList[0].CommHandle, &ftDev, &DevId, DevSN, DevDescription, NULL));
	
	switch ( InfoType )
	{
		case ITYPE_FT_DEVICE:
			
			sprintf( pReturnInfo , "0x%x" , ftDev ); 
			
			break;	
			
		case ITYPE_DEVICE_ID:
			
			sprintf( pReturnInfo , "0x%x" , DevId );
			
			break;
			
		case ITYPE_DEVICE_SN:
			
			if ( psvDriverVar->iPortNumber > 1 )
			{
				index = strlen(DevSN);
				
				if ( index )
				{
					index--;
					DevSN[index] = 0;
				}
			}
				
			strcpy ( pReturnInfo , DevSN );
				 
			break;

		case ITYPE_DEVICE_DESCRIPTION:
			
			if ( psvDriverVar->iPortNumber > 1 )
			{
				index = strlen(DevDescription);
				
				if ( index )
				{
					index--;
					DevDescription[index] = 0;
				}
			}
			
			strcpy ( pReturnInfo , DevDescription );
			
			break;			
	}	
	
Error:	
	
	if( Handle ) { CmtReleaseTSVPtr ( Handle );}
	
	RETURN_STDERR_POINTER;	
}

/**
 * @brief FTDI_SetModeConfiguration
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortIndex (unsigned char) - the specific port
 * @param[in] ucBitMode (unsigned char) - the mode to set
 * @param[in] ucBitConfig (unsigned char) - the configuration of pins to mode
 */
void*   DLLEXPORT FTDI_SetModeConfiguration( int Handle , unsigned char ucPortIndex , teBitModes ucBitMode , unsigned char ucBitConfig)
{
	STD_ERROR					StdError													=	{0};
	
	tsDriverHandleVar		*psvDriverVar										=	NULL; 
	
	IF (( Handle == 0 ) , "Empty Handle" );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &psvDriverVar  ));
	
	IF( ((ucBitMode < 0) || ( ucBitMode >= BMODE_SENTINEL )) , "Wrong Bit Mode" );
	
	IF( ( ucPortIndex >= psvDriverVar->iPortNumber ) , "Wrong Port Index" );   
	
	IF( ( psvDriverVar->vPortList[ucPortIndex].Port == NULL ) , "Wrong Port Index" ); 
	
	switch ( ucBitMode )
	{
		case BMODE_RESET:
			
			CHK_FTDI( FT_SetBitMode( psvDriverVar->vPortList[ucPortIndex].Port->CommHandle , ucBitConfig, 0x0));
			
			psvDriverVar->vPortList[ucPortIndex].CommMode = BMODE_RESET;
			psvDriverVar->vPortList[ucPortIndex].Port->CurrentCommMode = BMODE_RESET;
			break;
			
		case BMODE_ASYNCHRONOUS_BIT_BANG:
			
			CHK_FTDI( FT_SetBitMode( psvDriverVar->vPortList[ucPortIndex].Port->CommHandle , ucBitConfig, 0x1));
			
			psvDriverVar->vPortList[ucPortIndex].CommMode = BMODE_ASYNCHRONOUS_BIT_BANG;
			psvDriverVar->vPortList[ucPortIndex].Port->CurrentCommMode = BMODE_ASYNCHRONOUS_BIT_BANG;
			
			break;
			
//		case BMODE_MPSSE:
//			
//			CHK_FTDI( FT_SetBitMode(psvDriverVar->CommHandle, ucSetBits, 0x2));
//			
//			psvDriverVar->CommMode = BMODE_MPSSE;
//			
//			break;
//			
//		case BMODE_SYNCHRONOUS_BIT_BANG:
//			
//			CHK_FTDI( FT_SetBitMode(psvDriverVar->CommHandle, ucSetBits, 0x4));
//			
//			psvDriverVar->CommMode = BMODE_SYNCHRONOUS_BIT_BANG;
//			
//			break;
//			
//		case BMODE_MCU_HOST_BUS_EMULATION:
//			
//			CHK_FTDI( FT_SetBitMode(psvDriverVar->CommHandle, ucSetBits, 0x8));
//			
//			psvDriverVar->CommMode = BMODE_MCU_HOST_BUS_EMULATION;
//			
//			break;
//			
//		case BMODE_FAST_OPTO_ISOLATED_SERIAL:
//			
//			CHK_FTDI( FT_SetBitMode(psvDriverVar->CommHandle, ucSetBits, 0x10));
//			
//			psvDriverVar->CommMode = BMODE_FAST_OPTO_ISOLATED_SERIAL;
//			
//			break;
//			
		case BMODE_CBUS_BIT_BANG:
			
			ucBitConfig <<= 4;
			CHK_FTDI( FT_SetBitMode( psvDriverVar->vPortList[ucPortIndex].Port->CommHandle , ucBitConfig, 0x20));
			
			psvDriverVar->vPortList[ucPortIndex].CommMode = BMODE_CBUS_BIT_BANG;
			psvDriverVar->vPortList[ucPortIndex].Port->CurrentCommMode = BMODE_CBUS_BIT_BANG;
			
			break;
//			
//		case BMODE_SINGLE_CHANNEL_SYNCHRONOUS_245_FIFO:
//			
//			CHK_FTDI( FT_SetBitMode(psvDriverVar->CommHandle, ucSetBits, 0x40));
//			
//			psvDriverVar->CommMode = BMODE_SINGLE_CHANNEL_SYNCHRONOUS_245_FIFO;
//			
//			break;
			
			default:
				break;
	}
	
Error:	
	
	if( Handle ) 
		CmtReleaseTSVPtr ( Handle );
	
	RETURN_STDERR_POINTER;	
}

/**
 * @brief FTDI_SetPortConfiguration
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortIndex (unsigned char) - the specific port
 * @param[in] ucBitConfig (unsigned char) - the configuration of pins to set (I/O)
 */
void*   DLLEXPORT FTDI_SetPortConfiguration( int Handle , unsigned char ucPortIndex , unsigned char ucBitConfig)
{
	STD_ERROR					StdError													=	{0};
	
	tsDriverHandleVar		*psvDriverVar										=	NULL; 
	
	IF (( Handle == 0 ) , "Empty Handle" );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &psvDriverVar  ));
	
	IF( ( ucPortIndex >= psvDriverVar->iPortNumber ) , "Wrong Port Index" );   
	
	psvDriverVar->vPortList[ucPortIndex].PortConfig |= ucBitConfig;
	
Error:	
	
	if( Handle ) 
		CmtReleaseTSVPtr ( Handle );
	
	RETURN_STDERR_POINTER;	
}

/**
 * @brief FTDI_SetSpeed
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortIndex (unsigned char) - the specific port
 * @param[in] iBaudRate (unsigned char) - the configuration of pins to set (I/O)
 */
void*   DLLEXPORT FTDI_SetSpeed( int Handle , unsigned char ucPortIndex , int iBaudRate )
{
	STD_ERROR					StdError													=	{0};
	
	tsDriverHandleVar		*psvDriverVar										=	NULL; 
	
	int						vBaudRateList[]										=	{ FT_BAUD_300 , FT_BAUD_600 , FT_BAUD_1200 , FT_BAUD_2400 , FT_BAUD_4800 , FT_BAUD_9600 , FT_BAUD_14400 , FT_BAUD_19200 , FT_BAUD_38400 , FT_BAUD_57600 , FT_BAUD_115200 , FT_BAUD_230400 , FT_BAUD_460800 , FT_BAUD_921600 , FT_BAUD_1250000 };
	
	int						index												=	0,
							size												=	0;
	
	IF (( Handle == 0 ) , "Empty Handle" );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &psvDriverVar  ));
	
	IF( ( ucPortIndex >= psvDriverVar->iPortNumber ) , "Wrong Port Index" );   
	
	size = sizeof(vBaudRateList) / sizeof(int);
	
	for( index = 0; index < size; index++ )
		if ( iBaudRate < vBaudRateList[index] )
			break;
		
	if ( index >= size )
		index = size - 1;
	
	CHK_FTDI( FT_SetBaudRate( psvDriverVar->vPortList[ucPortIndex].Port->CommHandle , vBaudRateList[index] ));
	
Error:	
	
	if( Handle ) 
		CmtReleaseTSVPtr ( Handle );
	
	RETURN_STDERR_POINTER;	
}
			
			
			
/**
 * @brief FTDI_GetModeConfiguration
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortIndex (unsigned char) - the specific port
 * @param[out] pReturnMode pointer to the geted mode
 * @param[out] pReturnConfiguration pointer to the geted configuration of pins (I/0)
 */
void*   DLLEXPORT FTDI_GetModeConfiguration( int Handle , unsigned char ucPortIndex , char *pReturnMode ,  char *pReturnConfiguration)
{
	STD_ERROR					StdError													=	{0};
	
	tsDriverHandleVar		*psvDriverVar										=	NULL; 
	
	IF (( Handle == 0 ) , "Empty Handle" );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &psvDriverVar  ));
	
	IF( ( ucPortIndex >= psvDriverVar->iPortNumber ) , "Wrong Port Index" );   
	
	switch ( psvDriverVar->vPortList[ucPortIndex].CommMode )
	{
		case BMODE_RESET:
			
			strcpy ( pReturnMode , "Reset" );
			
			break;
			
		case BMODE_ASYNCHRONOUS_BIT_BANG:
			
			strcpy ( pReturnMode , "Asynchronous Bit Bang" );
			
			break;
			
		case BMODE_MPSSE:
			
			strcpy ( pReturnMode , "MPSSE" );
			
			break;
			
		case BMODE_SYNCHRONOUS_BIT_BANG:
			
			strcpy ( pReturnMode , "Synchronous Bit Bang" );
			
			break;
			
		case BMODE_MCU_HOST_BUS_EMULATION:
			
			strcpy ( pReturnMode , "MCU Host Bus Emulation Mode" );
			
			break;
			
		case BMODE_FAST_OPTO_ISOLATED_SERIAL:
			
			strcpy ( pReturnMode , "Fast Opto-Isolated Serial Mode" );
			
			break;
			
		case BMODE_CBUS_BIT_BANG:
			
			strcpy ( pReturnMode , "CBUS Bit Bang Mode" );
			
			break;			
			
		case BMODE_SINGLE_CHANNEL_SYNCHRONOUS_245_FIFO:
			
			strcpy ( pReturnMode , "Single Channel Synchronous 245 FIFO Mode" );
			
			break;	
	}
	
	sprintf( pReturnConfiguration , "0x%x" , psvDriverVar->vPortList[ucPortIndex].PortConfig );

Error:	
	
	if( Handle ) { CmtReleaseTSVPtr ( Handle );}
	
	RETURN_STDERR_POINTER;	
}						

/**
 * @brief FTDI_SetPort
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortIndex (unsigned char) - the specific port
 * @param[in] ucSetByte (unsigned char) - the byte (0-15) to set (write to port)
 */
void*   DLLEXPORT FTDI_SetPort( int Handle , unsigned char ucPortIndex , unsigned char ucMask , unsigned char ucSetByte)
{
	STD_ERROR					StdError													=	{0};
	
	DWORD  					BytesNum											=	1,
		   					BytesWritten										=	0;
	
	char   					szWriteBuffer[512]									=	{0};
	
	tsDriverHandleVar		*psvDriverVar										=	NULL; 
	
	int						index												=	0;
	
	unsigned char 			ucTempMask											=	0,
							ucBitConfig											=	0;
	
	IF (( Handle == 0 ) , "Empty Handle" );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &psvDriverVar  ));
	
	IF( ( ucPortIndex >= psvDriverVar->iPortNumber ) , "Wrong Port Index" ); 

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
	
	szWriteBuffer[0] = (psvDriverVar->vPortList[ucPortIndex].PortState) = (((psvDriverVar->vPortList[ucPortIndex].PortState) & (~ucMask)) | (ucMask & ucSetByte));
	
	if ( psvDriverVar->vPortList[ucPortIndex].CommMode == BMODE_ASYNCHRONOUS_BIT_BANG )
	{
		if ( psvDriverVar->vPortList[ucPortIndex].Port && ( psvDriverVar->vPortList[ucPortIndex].Port->CurrentCommMode != BMODE_ASYNCHRONOUS_BIT_BANG ))
		{
			FT_SetBitMode( psvDriverVar->vPortList[ucPortIndex].Port->CommHandle , 0xFF, 0x1);
			
			psvDriverVar->vPortList[ucPortIndex].Port->CurrentCommMode = BMODE_ASYNCHRONOUS_BIT_BANG;
		}
		
		CHK_FTDI( FT_Write(psvDriverVar->vPortList[ucPortIndex].Port->CommHandle, szWriteBuffer, BytesNum, &BytesWritten) );
	}
	
	if ( psvDriverVar->vPortList[ucPortIndex].CommMode == BMODE_CBUS_BIT_BANG ) 
	{
		ucBitConfig = (( psvDriverVar->vPortList[ucPortIndex].PortConfig << 4 ) | ( szWriteBuffer[0] & 0x0F ));
		
		psvDriverVar->vPortList[ucPortIndex].PortState = ucBitConfig;
		
		psvDriverVar->vPortList[ucPortIndex].Port->CurrentCommMode = BMODE_CBUS_BIT_BANG;
		
		CHK_FTDI( FT_SetBitMode( psvDriverVar->vPortList[ucPortIndex].Port->CommHandle , ucBitConfig, 0x20)); 
	}
	
Error:	
	
	if( Handle ) 
		CmtReleaseTSVPtr ( Handle );
	
	RETURN_STDERR_POINTER;	
}

/**
 * @brief FTDI_GetPort
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortIndex (unsigned char) - the specific port
 * @param[in] pGetByte pointer to the geted byte (outputs from FTDI_SetPort and inputs from Read command)
 */
void*   DLLEXPORT FTDI_GetPort( int Handle , unsigned char ucPortIndex , unsigned char *pGetByte)
{
	STD_ERROR					StdError													=	{0};
	
	unsigned char			ucReadByte											=	0;
	
	tsDriverHandleVar		*psvDriverVar										=	NULL; 
	
	IF (( Handle == 0 ) , "Empty Handle" );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &psvDriverVar  ));
	
	IF( ( ucPortIndex >= psvDriverVar->iPortNumber ) , "Wrong Port Index" );           
	
	if ( psvDriverVar->vPortList[ucPortIndex].CommMode == BMODE_ASYNCHRONOUS_BIT_BANG )
	{
		if ( psvDriverVar->vPortList[ucPortIndex].Port && ( psvDriverVar->vPortList[ucPortIndex].Port->CurrentCommMode != BMODE_ASYNCHRONOUS_BIT_BANG ))
		{	
			psvDriverVar->vPortList[ucPortIndex].Port->CurrentCommMode = BMODE_ASYNCHRONOUS_BIT_BANG;
			
			FT_SetBitMode( psvDriverVar->vPortList[ucPortIndex].Port->CommHandle , 0xFF, 0x1);
		}
	}
	
	if ( psvDriverVar->vPortList[ucPortIndex].CommMode == BMODE_CBUS_BIT_BANG ) 
	{
		psvDriverVar->vPortList[ucPortIndex].Port->CurrentCommMode = BMODE_CBUS_BIT_BANG;
		
		CHK_FTDI( FT_SetBitMode( psvDriverVar->vPortList[ucPortIndex].Port->CommHandle , psvDriverVar->vPortList[ucPortIndex].PortState, 0x20)); 
	}	
		
	CHK_FTDI( FT_GetBitMode(psvDriverVar->vPortList[ucPortIndex].Port->CommHandle, &ucReadByte ));   
	
	// collect current situation: outputs from PortConfig[0] and inputs from the Read command
		
	psvDriverVar->vPortList[ucPortIndex].PortState = (((psvDriverVar->vPortList[ucPortIndex].PortState) & (psvDriverVar->vPortList[ucPortIndex].PortConfig)) | ((~(psvDriverVar->vPortList[ucPortIndex].PortConfig)) & ucReadByte));

	if ( pGetByte )
		*pGetByte = psvDriverVar->vPortList[ucPortIndex].PortState;

Error:	
	
	if( Handle ) 
		CmtReleaseTSVPtr ( Handle );
	
	RETURN_STDERR_POINTER;	
}

/**
 * @brief FTDI_SetBit
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortIndex (unsigned char) - the specific port
 * @param[in] ucBitIndex (unsigned char) - the specific bit (0-7)
 * @param[in] ucSetBit (unsigned char) - the bit to set (0 - low, otherwise - hi)
 */
/*void*   DLLEXPORT FTDI_SetBit( int Handle , unsigned char ucPortIndex , unsigned char ucBitIndex , unsigned char ucSetBit)
{
	STD_ERROR					StdError													=	{0};
	
	DWORD					BytesNum											=	1,
							BytesWritten										=	0;
	
	unsigned char			ucHelper											=	0x01;
	char					szWriteBuffer[512]									=	{0};
	
	tsDriverHandleVar		*psvDriverVar										=	NULL; 
	
	IF (( Handle == 0 ) , "Empty Handle" );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &psvDriverVar  ));
	
	IF( ( ucPortIndex >= psvDriverVar->iPortNumber ) , "Wrong Port Index" );           
	
	IF(( psvDriverVar->vPortList[ucPortIndex].CommMode != BMODE_ASYNCHRONOUS_BIT_BANG ) , "Wrong Bit Mode" ); 
	
	IF(( ucBitIndex > 7 ) , "Wrong parameter (ucBitIndex)" );
	
	ucHelper = ucHelper<<ucBitIndex;
	
	IF((( ucHelper & (psvDriverVar->vPortList[ucPortIndex].PortConfig)) == 0) , "Error, trying to set an input bit" );
	
	if ( ucSetBit ) 									// not 0 - hi	
		(psvDriverVar->vPortList[ucPortIndex].PortState) |= ucHelper;
	else												// 0 - low
		(psvDriverVar->vPortList[ucPortIndex].PortState) &= (~ucHelper);
	
	szWriteBuffer[0] = (psvDriverVar->vPortList[ucPortIndex].PortState);
	
	CHK_FTDI( FT_Write(psvDriverVar->vPortList[ucPortIndex].CommHandle, szWriteBuffer, BytesNum, &BytesWritten) );
	
Error:	
	
	if( Handle ) { CmtReleaseTSVPtr ( Handle );}
	
	RETURN_STDERR_POINTER;	
}
 */
/**
 * @brief FTDI_GetBit
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortIndex (unsigned char) - the specific port
 * @param[in] ucBitIndex (unsigned char) - the specific bit (0-7)
 * @param[in] pGetBit pointer to the geted bit (output from FTDI_SetPort or input from Read command)
 */
/*void*   DLLEXPORT FTDI_GetBit( int Handle , unsigned char ucPortIndex , unsigned char ucBitIndex , unsigned char *pGetBit)
{
	STD_ERROR					StdError													=	{0};

	unsigned char			ucHelper											=	0x01,
							ucReadByte											=	0;
	
	tsDriverHandleVar		*psvDriverVar										=	NULL; 
	
	IF (( Handle == 0 ) , "Empty Handle" );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &psvDriverVar  ));
	
	IF( ( ucPortIndex >= psvDriverVar->iPortNumber ) , "Wrong Port Index" );           
	
	IF(( psvDriverVar->vPortList[ucPortIndex].CommMode != BMODE_ASYNCHRONOUS_BIT_BANG ) , "Wrong Bit Mode" ); 
	
	IF(( ucBitIndex > 7 ) , "Wrong parameter (ucBitIndex)" );
	
	ucHelper = ucHelper<<ucBitIndex;
	
	if ( ucHelper & (psvDriverVar->vPortList[ucPortIndex].PortConfig)) 				// wanted bit configurated as output 
		ucHelper &= psvDriverVar->vPortList[ucPortIndex].PortState;				
	else														// wanted bit configurated as input
	{
		// Get Data
		
		CHK_FTDI( FT_GetBitMode(psvDriverVar->vPortList[ucPortIndex].CommHandle, &ucReadByte ));
		
		ucHelper &= ucReadByte;
	}
	
	if ( pGetBit )
	{
		if (ucHelper)
			*pGetBit = 1;
		else
			*pGetBit = 0;
	}
	
Error:	
	
	if( Handle ) 
		CmtReleaseTSVPtr ( Handle );
	
	RETURN_STDERR_POINTER;	
}
*/


/**
 * @brief FTDI_GetPortQuantity
 *
 * @param[in] Handle (int) - handle 
 * @param[in] pNumberOfPorts pointer to the number of ports
 */
void*   DLLEXPORT FTDI_GetPortQuantity( int Handle , int *pNumberOfPorts )
{
	STD_ERROR					StdError										=	{0};
	
	tsDriverHandleVar		*psvDriverVar										=	NULL; 
	
	IF (( Handle == 0 ) , "Empty Handle" );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &psvDriverVar  ));
	
	if ( pNumberOfPorts ) 
		*pNumberOfPorts = psvDriverVar->iPortNumber;           

Error:	
	
	if( Handle ) 
		CmtReleaseTSVPtr ( Handle );
	
	RETURN_STDERR_POINTER;	
}

 
/**
 * @brief FTDI_Get_CBUS_PortNumber
 *
 * @param[in] Handle (int) - handle 
 * @param[in] pCBUS_Port_Number pointer to the CBUS port number
 */
void*   DLLEXPORT FTDI_Get_CBUS_PortNumber( int Handle , int *pCBUS_Port_Number )
{
	STD_ERROR					StdError										=	{0};
	
	tsDriverHandleVar		*psvDriverVar										=	NULL; 
	
	IF (( Handle == 0 ) , "Empty Handle" );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &psvDriverVar  ));
	
	if ( pCBUS_Port_Number ) 
		*pCBUS_Port_Number = psvDriverVar->iDevice_CBUS_Port_Number;           

Error:	
	
	if( Handle ) 
		CmtReleaseTSVPtr ( Handle );
	
	RETURN_STDERR_POINTER;	
}

/**
 * @brief FTDI_SPI
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortIndex (unsigned char) - the specific port
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
void*   DLLEXPORT FTDI_SPI( int Handle , unsigned char ucPortIndex , unsigned char ucEnableBitIndex , unsigned char ucEnableBitPolarity , unsigned char ucClockBitIndex , unsigned char ucClockBitPolarity , unsigned char ucClockBitPhase  , unsigned char ucDataBitIndex , unsigned __int64 ullSetData , unsigned char ucSetNumberOfBits , unsigned char ucReadBitPortIndex , unsigned char ucReadBitIndex , unsigned __int64 *ullGetData , unsigned char ucGetNumberOfBits  , unsigned char bDoubleEnablePulse )
{
	STD_ERROR					StdError													=	{0};
	
	DWORD  					BytesNum											=	0,
							BytesReaden											=	0,  
							BytesWritten										=	0;
	
	char   					szReadBuffer[512]									=	{0},
							szWriteBuffer[512]									=	{0};
	
	tsDriverHandleVar		*psvDriverVar										=	NULL; 
	
	int						index												=	0;
	
	unsigned char 			ucClockBitMask										=	0,
							ucDataBitMask										=	0,
							ucEnableBitMask										=	0,  
							ucReadBitMask										=	0,
							ucReadByte											=	0, 
							ucTempByte											=	0; 
								
	unsigned long long		ullOutputByte										=	0;
	
	IF (( Handle == 0 ) , "Empty Handle" );
	
	CHK_CMT( CmtGetTSVPtr ( Handle , &psvDriverVar  ));
	
	IF( ( ucPortIndex >= psvDriverVar->iPortNumber ) , "Wrong Port Index" ); 

	IF(( ucClockBitIndex == ucDataBitIndex ) , "Clock Bit can't be same bit as Data Bit." ); 
	
	IF(( ucClockBitIndex > 7 ) , "Wrong parameter (ucClockBitIndex)" );  
	IF(( ucDataBitIndex > 7 ) , "Wrong parameter (ucDataBitIndex)" );  
	
	ucClockBitMask = 1 << ucClockBitIndex;
	ucDataBitMask = 1 << ucDataBitIndex;
	ucEnableBitMask = 1 << ucEnableBitIndex; 
	ucReadBitMask = 1 << ucReadBitIndex;
	
	if ( psvDriverVar->vPortList[ucPortIndex].CommMode == BMODE_ASYNCHRONOUS_BIT_BANG )
	{
		if ( psvDriverVar->vPortList[ucPortIndex].Port && ( psvDriverVar->vPortList[ucPortIndex].Port->CurrentCommMode != BMODE_ASYNCHRONOUS_BIT_BANG ))
		{		
			psvDriverVar->vPortList[ucPortIndex].Port->CurrentCommMode = BMODE_ASYNCHRONOUS_BIT_BANG;
			
			FT_SetBitMode( psvDriverVar->vPortList[ucPortIndex].Port->CommHandle , 0xFF, 0x1);
		}
	}
	
	if ( psvDriverVar->vPortList[ucPortIndex].CommMode == BMODE_CBUS_BIT_BANG ) 
	{
		psvDriverVar->vPortList[ucPortIndex].Port->CurrentCommMode = BMODE_CBUS_BIT_BANG;
		
		CHK_FTDI( FT_SetBitMode( psvDriverVar->vPortList[ucPortIndex].Port->CommHandle , psvDriverVar->vPortList[ucPortIndex].PortState, 0x20)); 
	}
	
	if ( ucSetNumberOfBits )
	{
		BytesNum = 0;
	
		if ( ucClockBitPolarity ) 
			ucTempByte = 0xFF;
		else
			ucTempByte = 0;
	
		// Clear Clock 
		szWriteBuffer[BytesNum++] = (psvDriverVar->vPortList[ucPortIndex].PortState) = (((psvDriverVar->vPortList[ucPortIndex].PortState) & (~ucClockBitMask)) | (ucClockBitMask & ucTempByte ));

		if ( ucEnableBitIndex < 8 )
		{
			if ( ucEnableBitPolarity )
				ucTempByte = 0xFF;
			else
				ucTempByte = 0;	
		
			// Enable SPI
			szWriteBuffer[BytesNum++] = (psvDriverVar->vPortList[ucPortIndex].PortState) = (((psvDriverVar->vPortList[ucPortIndex].PortState) & (~ucEnableBitMask)) | (ucEnableBitMask & ucTempByte ));

		}
	
		if ( ucClockBitPolarity ) 
			ucTempByte = 0xFF;
		else
			ucTempByte = 0;
	
		// Clear Clock 
		szWriteBuffer[BytesNum++] = (psvDriverVar->vPortList[ucPortIndex].PortState) = (((psvDriverVar->vPortList[ucPortIndex].PortState) & (~ucClockBitMask)) | (ucClockBitMask & ucTempByte ));

		for( index = 0 ; index < ucSetNumberOfBits ; index++ )
		{
			if ( ucClockBitPhase == 0 )
			{
				if ( ullSetData & ( 0x1 << ( ucSetNumberOfBits - index - 1 )))
					ucTempByte = 0xFF;
				else
					ucTempByte = 0;
					
				// Set Data
				szWriteBuffer[BytesNum++] = (psvDriverVar->vPortList[ucPortIndex].PortState) = (((psvDriverVar->vPortList[ucPortIndex].PortState) & (~ucDataBitMask)) | (ucDataBitMask & ucTempByte ));
			}
			
			if ( !ucClockBitPolarity ) 
				ucTempByte = 0xFF;
			else
				ucTempByte = 0;
		
			// Set Clock
			szWriteBuffer[BytesNum++] = (psvDriverVar->vPortList[ucPortIndex].PortState) = (((psvDriverVar->vPortList[ucPortIndex].PortState) & (~ucClockBitMask)) | (ucClockBitMask & ucTempByte ));
		
			if ( ucClockBitPhase == 1 )
			{
				if ( ullSetData & ( 0x1 << ( ucSetNumberOfBits - index - 1 )))
					ucTempByte = 0xFF;
				else
					ucTempByte = 0;
					
				// Set Data
				szWriteBuffer[BytesNum++] = (psvDriverVar->vPortList[ucPortIndex].PortState) = (((psvDriverVar->vPortList[ucPortIndex].PortState) & (~ucDataBitMask)) | (ucDataBitMask & ucTempByte ));
			}
			
			if ( ucClockBitPolarity ) 
				ucTempByte = 0xFF;
			else
				ucTempByte = 0;
		
			// Set Clock
			szWriteBuffer[BytesNum++] = (psvDriverVar->vPortList[ucPortIndex].PortState) = (((psvDriverVar->vPortList[ucPortIndex].PortState) & (~ucClockBitMask)) | (ucClockBitMask & ucTempByte ));
		}
	
		if ( ucClockBitPolarity ) 
			ucTempByte = 0xFF;
		else
			ucTempByte = 0;

		// Clear Clock 
		szWriteBuffer[BytesNum++] = (psvDriverVar->vPortList[ucPortIndex].PortState) = (((psvDriverVar->vPortList[ucPortIndex].PortState) & (~ucClockBitMask)) | (ucClockBitMask & ucTempByte ));
		
		if ((( ucEnableBitIndex < 8 ) && ( ucGetNumberOfBits == 0 )) || ( bDoubleEnablePulse == 1 ))
		{
			if ( !ucEnableBitPolarity )
				ucTempByte = 0xFF;
			else
				ucTempByte = 0;	
		
			// Disable SPI
			szWriteBuffer[BytesNum++] = (psvDriverVar->vPortList[ucPortIndex].PortState) = (((psvDriverVar->vPortList[ucPortIndex].PortState) & (~ucEnableBitMask)) | (ucEnableBitMask & ucTempByte ));
		}
	
		ucTempByte = 0;
		
		// Clear Data
		szWriteBuffer[BytesNum++] = (psvDriverVar->vPortList[ucPortIndex].PortState) = (((psvDriverVar->vPortList[ucPortIndex].PortState) & (~ucDataBitMask)) | (ucDataBitMask & ucTempByte ));
	
		CHK_FTDI( FT_Write(psvDriverVar->vPortList[ucPortIndex].Port->CommHandle, szWriteBuffer, BytesNum, &BytesWritten) );
	
		IF(( BytesNum != BytesWritten ) , "SPI send fail." );   
		
		//CHK_FTDI( FT_Read( psvDriverVar->vPortList[ucPortIndex].Port->CommHandle , szReadBuffer , sizeof(szReadBuffer) , &BytesReaden )); 
	}
	
	if ( ucGetNumberOfBits )
	{
		//---------------------------------------------//
		if ( ucClockBitPolarity ) 
			ucTempByte = 0xFF;
		else
			ucTempByte = 0;
	
		// Clear Clock 
		szWriteBuffer[0] = (psvDriverVar->vPortList[ucPortIndex].PortState) = (((psvDriverVar->vPortList[ucPortIndex].PortState) & (~ucClockBitMask)) | (ucClockBitMask & ucTempByte ));

		CHK_FTDI( FT_Write(psvDriverVar->vPortList[ucPortIndex].Port->CommHandle, szWriteBuffer, 1, &BytesWritten) );
	
		IF(( BytesWritten != 1 ) , "SPI send fail." ); 
		//---------------------------------------------//  
		
		if ((( ucEnableBitIndex < 8 ) && ( ucSetNumberOfBits == 0 )) || ( bDoubleEnablePulse == 1 ))  
		{
			if ( ucEnableBitPolarity )
				ucTempByte = 0xFF;
			else
				ucTempByte = 0;	
		
			// Enable SPI
			szWriteBuffer[0] = (psvDriverVar->vPortList[ucPortIndex].PortState) = (((psvDriverVar->vPortList[ucPortIndex].PortState) & (~ucEnableBitMask)) | (ucEnableBitMask & ucTempByte ));
		
			CHK_FTDI( FT_Write(psvDriverVar->vPortList[ucPortIndex].Port->CommHandle, szWriteBuffer, 1, &BytesWritten) );
	
			IF(( BytesWritten != 1 ) , "SPI send fail." ); 
		}
	
		//---------------------------------------------//
		if ( ucClockBitPolarity ) 
			ucTempByte = 0xFF;
		else
			ucTempByte = 0;
	
		// Clear Clock 
		szWriteBuffer[0] = (psvDriverVar->vPortList[ucPortIndex].PortState) = (((psvDriverVar->vPortList[ucPortIndex].PortState) & (~ucClockBitMask)) | (ucClockBitMask & ucTempByte ));

		CHK_FTDI( FT_Write(psvDriverVar->vPortList[ucPortIndex].Port->CommHandle, szWriteBuffer, 1, &BytesWritten) );
	
		IF(( BytesWritten != 1 ) , "SPI send fail." ); 
		//---------------------------------------------// 
		
		for( index = 0 ; index < ucGetNumberOfBits ; index++ )
		{
			//================================= Data ============================================//
			// Set Data
			if ( ucClockBitPhase == 0 )
			{
				if ( ullSetData & ( 0x1 << ( ucGetNumberOfBits - index - 1 )))
					ucTempByte = 0xFF;
				else
					ucTempByte = 0;
					
				// Set Data
				szWriteBuffer[0] = (psvDriverVar->vPortList[ucPortIndex].PortState) = (((psvDriverVar->vPortList[ucPortIndex].PortState) & (~ucDataBitMask)) | (ucDataBitMask & ucTempByte ));
				
				CHK_FTDI( FT_Write(psvDriverVar->vPortList[ucPortIndex].Port->CommHandle, szWriteBuffer, 1, &BytesWritten) ); 
			}
			
			//================================= Data ============================================//
			// Get Data
			
			if ( ucClockBitPhase == 0 )
			{
				CHK_FTDI( FT_GetBitMode(psvDriverVar->vPortList[ucReadBitPortIndex].Port->CommHandle, &ucReadByte ));    

				if (( 0x01 << ucReadBitIndex ) & ucReadByte )
					ullOutputByte |= 0x01 << ( ucGetNumberOfBits - index - 1 );
			}
			//====================================================================================//
			
			//================================= Clock ============================================//
			if ( !ucClockBitPolarity ) 
				ucTempByte = 0xFF;
			else
				ucTempByte = 0;
		
			// Clear Clock
			szWriteBuffer[0] = (psvDriverVar->vPortList[ucPortIndex].PortState) = (((psvDriverVar->vPortList[ucPortIndex].PortState) & (~ucClockBitMask)) | (ucClockBitMask & ucTempByte ));
		
			CHK_FTDI( FT_Write(psvDriverVar->vPortList[ucPortIndex].Port->CommHandle, szWriteBuffer, 1, &BytesWritten) );
	
			IF(( BytesWritten != 1 ) , "SPI send fail." );
			//====================================================================================//
			//================================= Data ============================================//
			// Set Data
			if ( ucClockBitPhase == 1 )
			{
				if ( ullSetData & ( 0x1 << ( ucGetNumberOfBits - index - 1 )))
					ucTempByte = 0xFF;
				else
					ucTempByte = 0;
					
				// Set Data
				szWriteBuffer[0] = (psvDriverVar->vPortList[ucPortIndex].PortState) = (((psvDriverVar->vPortList[ucPortIndex].PortState) & (~ucDataBitMask)) | (ucDataBitMask & ucTempByte ));
				
				CHK_FTDI( FT_Write(psvDriverVar->vPortList[ucPortIndex].Port->CommHandle, szWriteBuffer, 1, &BytesWritten) );  
			}
			//====================================================================================//
			//================================= Clock ============================================//
			
			if ( ucClockBitPolarity ) 
				ucTempByte = 0xFF;
			else
				ucTempByte = 0;
			
			// Set Clock
			szWriteBuffer[0] = (psvDriverVar->vPortList[ucPortIndex].PortState) = (((psvDriverVar->vPortList[ucPortIndex].PortState) & (~ucClockBitMask)) | (ucClockBitMask & ucTempByte ));
		
			CHK_FTDI( FT_Write(psvDriverVar->vPortList[ucPortIndex].Port->CommHandle, szWriteBuffer, 1, &BytesWritten) );
	
			IF(( BytesWritten != 1 ) , "SPI send fail." );
			//====================================================================================//
			//================================= Data ============================================//
			// Get Data
			
			if ( ucClockBitPhase == 1 )
			{
				CHK_FTDI( FT_GetBitMode(psvDriverVar->vPortList[ucReadBitPortIndex].Port->CommHandle, &ucReadByte ));    

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
		szWriteBuffer[0] = (psvDriverVar->vPortList[ucPortIndex].PortState) = (((psvDriverVar->vPortList[ucPortIndex].PortState) & (~ucClockBitMask)) | (ucClockBitMask & ucTempByte ));
		
		CHK_FTDI( FT_Write(psvDriverVar->vPortList[ucPortIndex].Port->CommHandle, szWriteBuffer, 1, &BytesWritten) );
	
		IF(( BytesWritten != 1 ) , "SPI send fail." );
		
		if ( ucEnableBitIndex < 8 )
		{
			if ( !ucEnableBitPolarity )
				ucTempByte = 0xFF;
			else
				ucTempByte = 0;	
		
			// Enable SPI
			szWriteBuffer[0] = (psvDriverVar->vPortList[ucPortIndex].PortState) = (((psvDriverVar->vPortList[ucPortIndex].PortState) & (~ucEnableBitMask)) | (ucEnableBitMask & ucTempByte ));
		
			CHK_FTDI( FT_Write(psvDriverVar->vPortList[ucPortIndex].Port->CommHandle, szWriteBuffer, 1, &BytesWritten) );
	
			IF(( BytesWritten != 1 ) , "SPI send fail." );
		}
	}
	
Error:	
	
	if ( ullGetData )
		*ullGetData = ullOutputByte;
	
	if( Handle ) 
		CmtReleaseTSVPtr ( Handle );
	
	RETURN_STDERR_POINTER;	
}



