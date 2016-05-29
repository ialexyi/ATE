#ifndef __FTDI_Driver_H__
#define __FTDI_Driver_H__

//==============================================================================
// Include files

#include "cvidef.h"
#include "GlobalDefines.h"

//==============================================================================
// Constants
		
#define		 DBUS_PORT		0
#define		 CBUS_PORT		1		
		
//==============================================================================
// Types

//typedef enum { ETYPE_CURRENT_MODULE , 
//			   ETYPE_CMT_MULTITREADING , 
//			   ETYPE_FTDI_LOW_DRIVER , 
//			   ETYPE_SENTINEL 			
//	} teErrorModuleType;									// Error Types		
		

typedef enum { CTYPE_DESCRIPTION,
			   CTYPE_SN_NUMBER ,  
			   CTYPE_LOCATION , 
			   CTYPE_SINGLE_DEVICE,
			   CTYPE_SENTINEL 
	} teConnectionTypes;									// Connection Types

typedef enum { DTYPE_FLAG_VALUE , 
			   DTYPE_DEVICE_TYPE , 
			   DTYPE_DEVICE_ID , 
			   DTYPE_DEVICE_LOCATION_ID , 
			   DTYPE_DEVICE_SN , 
			   DTYPE_DEVICE_DESCRIPTION , 
			   DTYPE_DEVICE_FT_HANDLE , 
			   DTYPE_SENTINEL 
	} teDetailTypes;										// Device detail Types

typedef enum { ITYPE_FT_DEVICE , 
			   ITYPE_DEVICE_ID , 
			   ITYPE_DEVICE_SN , 
			   ITYPE_DEVICE_DESCRIPTION , 
			   ITYPE_SENTINEL 
	} teInfoTypes;											// Device info Types

typedef enum { DEVICE_BM , 
			   DEVICE_AM , 
			   DEVICE_100AX , 
			   DEVICE_UNKNOWN , 
			   DEVICE_2232C , 
			   DEVICE_232R , 
			   DEVICE_2232H , 
			   DEVICE_4232H , 
			   DEVICE_SENTINEL 
	} teDevicePackages;										// Device packages

typedef enum { BMODE_RESET , 
			   BMODE_ASYNCHRONOUS_BIT_BANG , 
			   BMODE_MPSSE , 
			   BMODE_SYNCHRONOUS_BIT_BANG , 
			   BMODE_MCU_HOST_BUS_EMULATION , 
			   BMODE_FAST_OPTO_ISOLATED_SERIAL , 
			   BMODE_CBUS_BIT_BANG , 
			   BMODE_SINGLE_CHANNEL_SYNCHRONOUS_245_FIFO , 
			   BMODE_SENTINEL 
	} teBitModes;											// Device bit modes


//==============================================================================
// External variables

//==============================================================================
// Global functions

/**
 * @brief FTDI_NumberOfDevices
 *
 * @param[out] piNumberOfDevices pointer to int that contains the number of connected devices
 */
void*   DLLEXPORT FTDI_NumberOfDevices(unsigned long *piNumberOfDevices);

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
void*   DLLEXPORT FTDI_GetDeviceDetail( unsigned int DeviceNumber , teDetailTypes DetailType , char *pReturnDetail );

/**
 * @brief FTDI_Init
 *
 * @param[in] ConnectByType (define int) - the connection type
 * @param[in] szConnectName (define int) - the cpesific value of a connection type
 * @param[out] pHandle pointer to the opened Handle
 */
void*   DLLEXPORT FTDI_Init(teConnectionTypes ConnectByType , char *szConnectName , int *pHandle);

/**
 * @brief FTDI_Close
 *
 * @param[in] Handle (int) - handle
 */
void*   DLLEXPORT FTDI_Close(int *pHandle);

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
void*   DLLEXPORT FTDI_GetDeviceInfo( int Handle , teInfoTypes InfoType , char *pReturnInfo );

/**
 * @brief FTDI_SetModeConfiguration
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucBitMode (unsigned char) - the mode to set
 * @param[in] ucBitConfig (unsigned char) - the configuration of pins to mode
 */
void*   DLLEXPORT FTDI_SetModeConfiguration( int Handle , unsigned char ucPortIndex , teBitModes ucBitMode , unsigned char ucBitConfig);

/**
 * @brief FTDI_SetModeConfiguration
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortIndex (unsigned char) - the specific port
 * @param[in] ucBitConfig (unsigned char) - the configuration of pins to set (I/O)
 */
void*   DLLEXPORT FTDI_SetPortConfiguration( int Handle , unsigned char ucPortIndex , unsigned char ucBitConfig);

/**
 * @brief FTDI_GetModeConfiguration
 *
 * @param[in] Handle (int) - handle 
 * @param[out] pReturnMode pointer to the geted mode
 * @param[out] pReturnConfiguration pointer to the geted configuration of pins (I/0)
 */
void*   DLLEXPORT FTDI_GetModeConfiguration( int Handle , unsigned char ucPortIndex , char *pReturnMode ,  char *pReturnConfiguration);

/**
 * @brief FTDI_SetPort
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortIndex (unsigned char) - the specific port
 * @param[in] ucSetByte (unsigned char) - the byte (0-15) to set (write to port)
 */
void*   DLLEXPORT FTDI_SetPort( int Handle , unsigned char ucPortIndex , unsigned char ucMask , unsigned char ucSetByte);							// usefull in Asynchronous Bit Bang Mode ( BMODE_ASYNCHRONOUS_BIT_BANG );

/**
 * @brief FTDI_GetPort
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortIndex (unsigned char) - the specific port
 * @param[in] pGetByte pointer to the geted byte (outputs from FTDI_SetPort and inputs from Read command)
 */
void*   DLLEXPORT FTDI_GetPort( int Handle , unsigned char ucPortIndex , unsigned char *pGetByte);										// usefull in Asynchronous Bit Bang Mode ( BMODE_ASYNCHRONOUS_BIT_BANG ); 

/**
 * @brief FTDI_SetBit
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortIndex (unsigned char) - the specific port
 * @param[in] ucBitIndex (unsigned char) - the specific bit (0-7)
 * @param[in] ucSetBit (unsigned char) - the bit to set (0 - low, otherwise - hi)
 */
//void*   DLLEXPORT FTDI_SetBit( int Handle , unsigned char ucPortIndex , unsigned char ucBitIndex , unsigned char ucSetBit); 	// usefull in Asynchronous Bit Bang Mode ( BMODE_ASYNCHRONOUS_BIT_BANG );

/**
 * @brief FTDI_GetBit
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortIndex (unsigned char) - the specific port
 * @param[in] ucBitIndex (unsigned char) - the specific bit (0-7)
 * @param[in] pGetBit pointer to the geted bit (output from FTDI_SetPort or input from Read command)
 */
//void*   DLLEXPORT FTDI_GetBit( int Handle , unsigned char ucPortIndex , unsigned char ucBitIndex , unsigned char *pGetBit);			// usefull in Asynchronous Bit Bang Mode ( BMODE_ASYNCHRONOUS_BIT_BANG );


/**
 * @brief FTDI_GetPortQuantity
 *
 * @param[in] Handle (int) - handle 
 * @param[in] pNumberOfPorts pointer to the number of ports
 */
void*   DLLEXPORT FTDI_GetPortQuantity( int Handle , int *pNumberOfPorts );

/**
 * @brief FTDI_SetSpeed
 *
 * @param[in] Handle (int) - handle 
 * @param[in] ucPortIndex (unsigned char) - the specific port
 * @param[in] iBaudRate (unsigned char) - the configuration of pins to set (I/O)
 */
void*   DLLEXPORT FTDI_SetSpeed( int Handle , unsigned char ucPortIndex , int iBaudRate );

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
 * @param[in] bDoubleEnablePulse (unsigned char) - Different enable pulse for send and receive data.
 */
void*   DLLEXPORT FTDI_SPI( int Handle , unsigned char ucPortIndex , unsigned char ucEnableBitIndex , unsigned char ucEnableBitPolarity , unsigned char ucClockBitIndex , unsigned char ucClockBitPolarity , unsigned char ucClockBitPhase  , unsigned char ucDataBitIndex , unsigned __int64 ullSetData , unsigned char ucSetNumberOfBits , unsigned char ucReadBitPortIndex , unsigned char ucReadBitIndex , unsigned __int64 *ullGetData , unsigned char ucGetNumberOfBits , unsigned char bDoubleEnablePulse );

 
/**
 * @brief FTDI_Get_CBUS_PortNumber
 *
 * @param[in] Handle (int) - handle 
 * @param[in] pCBUS_Port_Number pointer to the CBUS port number
 */
void*   DLLEXPORT FTDI_Get_CBUS_PortNumber( int Handle , int *pCBUS_Port_Number );

int		DLLEXPORT FTDI_GetErrorMessage( int error , char *pszDescription );


#endif  /* ndef __FTDI_Driver_H__ */
