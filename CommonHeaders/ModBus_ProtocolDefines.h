#ifndef __ModBus_ProtocolDefines_H__
#define __ModBus_ProtocolDefines_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"

#define									MAX_MODBUS_PROTOCOL_LENGTH					512			
#define									MAX_MODBUS_COMMAND_DEFINE_LENGTH			2048
//==============================================================================
// Constants
// dev_addr func_code(16) start_addr(hi,lo) num_regs(hi,lo) num_bytes data_1(hi,lo) ... data_n(hi,lo) crc16(lo,hi)
//==============================================================================
// Types
typedef enum { PROTOCOL_TYPE_NONE , PROTOCOL_TYPE_A , PROTOCOL_TYPE_B , PROTOCOL_TYPE_C , PROTOCOL_TYPE_D , PROTOCOL_TYPE_E , PROTOCOL_TYPE_F , PROTOCOL_TYPE_G , PROTOCOL_TYPE_SENTINEL } teModBusProtocolType;
typedef enum { VALUE_TYPE_NONE , VALUE_TYPE_CHAR , VALUE_TYPE_SHORT , VALUE_TYPE_INT , VALUE_TYPE_LONG , VALUE_TYPE_FLOAT , VALUE_TYPE_DOUBLE , VALUE_TYPE_ARRAY , VALUE_TYPE_SENTINEL } teModBusProtocolValueType;

typedef union
{
	unsigned short						Word;   
	
	struct
	{   
		unsigned char					byte_lo,
										byte_hi;
	};
	
} tuSweepConvertWord;
		
typedef struct
{ 
	union
	{
		char							max_size[MAX_MODBUS_PROTOCOL_LENGTH];   
	
		struct
		{   
			unsigned char				dev_addr,
										func_code;
	
			unsigned char				start_addr_hi,
										start_addr_lo;
	
			unsigned char				num_of_hi,
										num_of_lo;
	
			unsigned char				data_length;
		
			unsigned char				single_data_hi, 
										single_data_lo; 
		
			unsigned char				crc16_hi, 
										crc16_lo; 
		} tTypeA;
		
		struct
		{   
			unsigned char				dev_addr,
										func_code;
	
			unsigned char				start_addr_hi,
										start_addr_lo;
	
			unsigned char				num_of_hi,
										num_of_lo;
		
			unsigned char				crc16_hi, 
										crc16_lo; 
		} tTypeB;
		
		struct
		{   
			unsigned char				dev_addr,
										func_code;
	
			unsigned char				start_addr_hi,
										start_addr_lo;
	
			unsigned char				single_data_hi, 
										single_data_lo; 
		
			unsigned char				crc16_hi, 
										crc16_lo; 
		} tTypeC;
		
		struct
		{   
			unsigned char				dev_addr,
										func_code;
	
			unsigned char				data_length;
			
			unsigned char				single_data_lo; 
			
			unsigned char				crc16_hi, 
										crc16_lo; 
		} tTypeD;
		
		struct
		{   
			unsigned char				dev_addr,
										func_code;
	
			unsigned char				data_length;
			
			unsigned char				single_data_hi, 
										single_data_lo; 
			
			unsigned char				crc16_hi, 
										crc16_lo; 
		} tTypeE;
		
		struct
		{   
			unsigned char				dev_addr,
										func_code;

			unsigned char				single_data_lo; 
				
			unsigned char				crc16_hi, 
										crc16_lo; 
		} tTypeF;
		
		struct
		{   
			unsigned char				dev_addr,
										func_code;

			unsigned char				crc16_hi, 
										crc16_lo; 
		} tTypeG;
	};
	
} tsMODBUS_ProtocolItem;

typedef union
{
	char							max_size[MAX_MODBUS_COMMAND_DEFINE_LENGTH];   
	
	struct
	{   
		char						szCommandName[128]; 
		
		tsMODBUS_ProtocolItem		send_side,
									receive_side;
		
		char						szSendData[128],
									szResponseData[128];
		
		int							iSendType,
									iResponseType;
		
		int							iSendDataSize,
									iResponseDataSize;
		
		int							bSendCrcBigEndian,
									bResponseCrcBigEndian;
		
		double						lfSendValueMultiply,
									lfResponseValueMultiply;
		
		teModBusProtocolValueType	tSendValueType,
									tResponseValueType;
		
		int							bSendSwap,
									bResponseSwap;
		
		int							bSendDataByteBigEndian,
									bResponseDataByteBigEndian;
									
		//------------ Signature --------------------------------//
		unsigned __int64			ulSignatureID;
		//-------------------------------------------------------//	
		
		unsigned int				bSendPrefix,
									bResponsePrefix;
		
		unsigned int				iSendPrefix_ID,
									iResponsePrefix_ID;
		
	};
	
} tsMODBUS_CommandItem;

//-------------- Oven Standards ------------------------------//
#ifndef _OVEN_STD_COMMANDS_
#define  _OVEN_STD_COMMANDS_
char  gvszOVEN_STANDARD_CMD_NAMES_LIST[][64] = {{"SET_TEMPERATURE"},{"GET_TEMPERATURE"},{"READ_TEMPERATURE"},{"START_OVEN"},{"STOP_OVEN"},{"SET_SLOPE"},{"INIT_CONTROL"},{"GET_CHAMBER_STATUS"},{"CLOSE_CONTROL"},{"GET_SLOPE"}};
#endif

typedef enum { OVEN_STD_CMD_SET_TEMPERATURE , OVEN_STD_CMD_GET_TEMPERATURE , OVEN_STD_CMD_READ_TEMPERATURE , OVEN_STD_CMD_START_OVEN , OVEN_STD_CMD_STOP_OVEN , OVEN_STD_CMD_SET_SLOPE , OVEN_STD_CMD_INIT_CONTROL , OVEN_STD_CMD_GET_CHAMBER_STATUS , OVEN_STD_CMD_CLOSE_CONTROL ,OVEN_STD_CMD_GET_SLOPE , OVEN_STD_CMD_SENTINEL } teOvenStandardCmd;
//---------------------------------------------//

//==============================================================================
// External variables

//==============================================================================
// Global functions


#ifdef __cplusplus
    }
#endif

#endif  /* ndef __ModBus_ProtocolDefines_H__ */
