//==============================================================================
// Include files

#include "STD_RIO_Driver_Commands_Define.h"

//==============================================================================

// Global variables
//============================================================================== 
extern tsSTD_RIO_CommandItem	tSTD_RIO_CommandList[] = 
{
	/***** ***** ***** ***** ***** ***** Get AI Line Value Commands ***** ***** ***** ***** ***** ***** *****/ 
	
	{
		/*	szCommandName				*/	{"GET_AI_LINE_0_VALUE"}, 
		
		/*	usCommandId				    */  0,
		/*	usPortNumber     			*/	0,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
	
		/*	ucReceiveDataType  	 		*/  0x06,
		/*	usReceiveDataLength			*/  0x08
	},
	{
		/*	szCommandName				*/	{"GET_AI_LINE_1_VALUE"}, 
		
		/*	usCommandId				    */  1,
		/*	usPortNumber     			*/	1,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
	
		/*	ucReceiveDataType  	 		*/  0x06,
		/*	usReceiveDataLength			*/  0x08
	},
	{
		/*	szCommandName				*/	{"GET_AI_LINE_2_VALUE"}, 
		
		/*	usCommandId				    */  2,
		/*	usPortNumber     			*/	2,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
	
		/*	ucReceiveDataType  	 		*/  0x06,
		/*	usReceiveDataLength			*/  0x08
	},
	{
		/*	szCommandName				*/	{"GET_AI_LINE_3_VALUE"}, 
		
		/*	usCommandId				    */  3,
		/*	usPortNumber     			*/	3,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
	
		/*	ucReceiveDataType  	 		*/  0x06,
		/*	usReceiveDataLength			*/  0x08
	},
	{
		/*	szCommandName				*/	{"GET_AI_LINE_4_VALUE"}, 
		
		/*	usCommandId				    */  4,
		/*	usPortNumber     			*/	4,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
	
		/*	ucReceiveDataType  	 		*/  0x06,
		/*	usReceiveDataLength			*/  0x08
	},
	{
		/*	szCommandName				*/	{"GET_AI_LINE_5_VALUE"}, 
		
		/*	usCommandId				    */  5,
		/*	usPortNumber     			*/	5,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
	
		/*	ucReceiveDataType  	 		*/  0x06,
		/*	usReceiveDataLength			*/  0x08
	}, 
	{
		/*	szCommandName				*/	{"GET_AI_LINE_6_VALUE"}, 
		
		/*	usCommandId				    */  6,
		/*	usPortNumber     			*/	6,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
	
		/*	ucReceiveDataType  	 		*/  0x06,
		/*	usReceiveDataLength			*/  0x08
	},
	{
		/*	szCommandName				*/	{"GET_AI_LINE_7_VALUE"}, 
		
		/*	usCommandId				    */  7,
		/*	usPortNumber     			*/	7,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
	
		/*	ucReceiveDataType  	 		*/  0x06,
		/*	usReceiveDataLength			*/  0x08
	},
	
	/***** ***** ***** ***** ***** ***** Get AO Line Value Commands ***** ***** ***** ***** ***** ***** *****/ 
	
	{
		/*	szCommandName				*/	{"GET_AO_LINE_0_VALUE"},
		
		/*	usCommandId				    */  100, 
		/*	usPortNumber     			*/	0,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,  
	
		/*	ucReceiveDataType  	 		*/  0x06,
		/*	usReceiveDataLength			*/  0x08
	},
	{
		/*	szCommandName				*/	{"GET_AO_LINE_1_VALUE"},
		
		/*	usCommandId				    */  101, 
		/*	usPortNumber     			*/	1,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,  
	
		/*	ucReceiveDataType  	 		*/  0x06,
		/*	usReceiveDataLength			*/  0x08
	},
	{
		/*	szCommandName				*/	{"GET_AO_LINE_2_VALUE"},
		
		/*	usCommandId				    */  102, 
		/*	usPortNumber     			*/	2,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,  
	
		/*	ucReceiveDataType  	 		*/  0x06,
		/*	usReceiveDataLength			*/  0x08
	},
	{
		/*	szCommandName				*/	{"GET_AO_LINE_3_VALUE"},
		
		/*	usCommandId				    */  103, 
		/*	usPortNumber     			*/	3,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,  
	
		/*	ucReceiveDataType  	 		*/  0x06,
		/*	usReceiveDataLength			*/  0x08
	},
	{
		/*	szCommandName				*/	{"GET_AO_LINE_4_VALUE"},
		
		/*	usCommandId				    */  104, 
		/*	usPortNumber     			*/	4,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,  
	
		/*	ucReceiveDataType  	 		*/  0x06,
		/*	usReceiveDataLength			*/  0x08
	},
	{
		/*	szCommandName				*/	{"GET_AO_LINE_5_VALUE"},
		
		/*	usCommandId				    */  105, 
		/*	usPortNumber     			*/	5,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,  
	
		/*	ucReceiveDataType  	 		*/  0x06,
		/*	usReceiveDataLength			*/  0x08
	},
	{
		/*	szCommandName				*/	{"GET_AO_LINE_6_VALUE"},
		
		/*	usCommandId				    */  106, 
		/*	usPortNumber     			*/	6,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,  
	
		/*	ucReceiveDataType  	 		*/  0x06,
		/*	usReceiveDataLength			*/  0x08
	},
	{
		/*	szCommandName				*/	{"GET_AO_LINE_7_VALUE"},
		
		/*	usCommandId				    */  107, 
		/*	usPortNumber     			*/	7,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,  
	
		/*	ucReceiveDataType  	 		*/  0x06,
		/*	usReceiveDataLength			*/  0x08
	},
	
	/***** ***** ***** ***** ***** ***** Get DI Line Value Commands ***** ***** ***** ***** ***** ***** *****/  
	
	{
		/*	szCommandName				*/	{"GET_DI_LINE_0_VALUE"},
		
		/*	usCommandId				    */  200, 
		/*	usPortNumber     			*/	0,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_1_VALUE"},
		
		/*	usCommandId				    */  201, 
		/*	usPortNumber     			*/	1,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_2_VALUE"},
		
		/*	usCommandId				    */  202, 
		/*	usPortNumber     			*/	2,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_3_VALUE"},
		
		/*	usCommandId				    */  203, 
		/*	usPortNumber     			*/	3,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_4_VALUE"},
		
		/*	usCommandId				    */  204, 
		/*	usPortNumber     			*/	4,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_5_VALUE"},
		
		/*	usCommandId				    */  205, 
		/*	usPortNumber     			*/	5,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_6_VALUE"},
		
		/*	usCommandId				    */  206, 
		/*	usPortNumber     			*/	6,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_7_VALUE"},
		
		/*	usCommandId				    */  207, 
		/*	usPortNumber     			*/	7,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	}, 
	{
		/*	szCommandName				*/	{"GET_DI_LINE_8_VALUE"},
		
		/*	usCommandId				    */  208, 
		/*	usPortNumber     			*/	8,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_9_VALUE"},
		
		/*	usCommandId				    */  209, 
		/*	usPortNumber     			*/	9,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_10_VALUE"},
		
		/*	usCommandId				    */  210, 
		/*	usPortNumber     			*/	10,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_11_VALUE"},
		
		/*	usCommandId				    */  211, 
		/*	usPortNumber     			*/	11,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_12_VALUE"},
		
		/*	usCommandId				    */  212, 
		/*	usPortNumber     			*/	12,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_13_VALUE"},
		
		/*	usCommandId				    */  213, 
		/*	usPortNumber     			*/	13,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_14_VALUE"},
		
		/*	usCommandId				    */  214, 
		/*	usPortNumber     			*/	14,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_15_VALUE"},
		
		/*	usCommandId				    */  215, 
		/*	usPortNumber     			*/	0,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	
	/***** ***** ***** ***** ***** ***** Get DO Line Value Commands ***** ***** ***** ***** ***** ***** *****/  	
	
	{
		/*	szCommandName				*/	{"GET_D0_LINE_0_VALUE"},
		
		/*	usCommandId				    */  300, 
		/*	usPortNumber     			*/	0,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00, 
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_D0_LINE_1_VALUE"},
		
		/*	usCommandId				    */  301, 
		/*	usPortNumber     			*/	1,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00, 
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_D0_LINE_2_VALUE"},
		
		/*	usCommandId				    */  302, 
		/*	usPortNumber     			*/	2,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00, 
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_D0_LINE_3_VALUE"},
		
		/*	usCommandId				    */  303, 
		/*	usPortNumber     			*/	3,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00, 
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_D0_LINE_4_VALUE"},
		
		/*	usCommandId				    */  304, 
		/*	usPortNumber     			*/	4,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00, 
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_D0_LINE_5_VALUE"},
		
		/*	usCommandId				    */  305, 
		/*	usPortNumber     			*/	5,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00, 
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_D0_LINE_6_VALUE"},
		
		/*	usCommandId				    */  306, 
		/*	usPortNumber     			*/	6,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00, 
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_D0_LINE_7_VALUE"},
		
		/*	usCommandId				    */  307, 
		/*	usPortNumber     			*/	7,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00, 
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_D0_LINE_8_VALUE"},
		
		/*	usCommandId				    */  308, 
		/*	usPortNumber     			*/	8,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00, 
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_D0_LINE_9_VALUE"},
		
		/*	usCommandId				    */  309, 
		/*	usPortNumber     			*/	9,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00, 
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_D0_LINE_10_VALUE"},
		
		/*	usCommandId				    */  310, 
		/*	usPortNumber     			*/	10,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00, 
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_D0_LINE_11_VALUE"},
		
		/*	usCommandId				    */  311, 
		/*	usPortNumber     			*/	11,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00, 
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_D0_LINE_12_VALUE"},
		
		/*	usCommandId				    */  312, 
		/*	usPortNumber     			*/	12,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00, 
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_D0_LINE_13_VALUE"},
		
		/*	usCommandId				    */  313, 
		/*	usPortNumber     			*/	13,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00, 
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_D0_LINE_14_VALUE"},
		
		/*	usCommandId				    */  314, 
		/*	usPortNumber     			*/	14,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00, 
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_D0_LINE_15_VALUE"},
		
		/*	usCommandId				    */  315, 
		/*	usPortNumber     			*/	15,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00, 
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},

	/***** ***** ***** ***** ***** ***** Set AO Line Value Commands ***** ***** ***** ***** ***** ***** *****/  	
	
	{
		/*	szCommandName				*/	{"SET_AO_LINE_0_VALUE"},
		
		/*	usCommandId				    */  400, 
		/*	usPortNumber     			*/	0,
		
		/*	ucSendDataType	     		*/  0x06,
		/*	usSendDataLength   			*/  0x08, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_AO_LINE_1_VALUE"},
		
		/*	usCommandId				    */  401, 
		/*	usPortNumber     			*/	1,
		
		/*	ucSendDataType	     		*/  0x06,
		/*	usSendDataLength   			*/  0x08, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_AO_LINE_2_VALUE"},
		
		/*	usCommandId				    */  402, 
		/*	usPortNumber     			*/	2,
		
		/*	ucSendDataType	     		*/  0x06,
		/*	usSendDataLength   			*/  0x08, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_AO_LINE_3_VALUE"},
		
		/*	usCommandId				    */  403, 
		/*	usPortNumber     			*/	3,
		
		/*	ucSendDataType	     		*/  0x06,
		/*	usSendDataLength   			*/  0x08, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_AO_LINE_4_VALUE"},
		
		/*	usCommandId				    */  404, 
		/*	usPortNumber     			*/	4,
		
		/*	ucSendDataType	     		*/  0x06,
		/*	usSendDataLength   			*/  0x08, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_AO_LINE_5_VALUE"},
		
		/*	usCommandId				    */  405, 
		/*	usPortNumber     			*/	5,
		
		/*	ucSendDataType	     		*/  0x06,
		/*	usSendDataLength   			*/  0x08, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_AO_LINE_6_VALUE"},
		
		/*	usCommandId				    */  406, 
		/*	usPortNumber     			*/	6,
		
		/*	ucSendDataType	     		*/  0x06,
		/*	usSendDataLength   			*/  0x08, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_AO_LINE_7_VALUE"},
		
		/*	usCommandId				    */  407, 
		/*	usPortNumber     			*/	7,
		
		/*	ucSendDataType	     		*/  0x06,
		/*	usSendDataLength   			*/  0x08, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	
	/***** ***** ***** ***** ***** ***** Set DO Line Value Commands ***** ***** ***** ***** ***** ***** *****/
	
	{
		/*	szCommandName				*/	{"SET_DO_LINE_0_VALUE"},
		
		/*	usCommandId				    */  500, 
		/*	usPortNumber     			*/	0,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DO_LINE_1_VALUE"},
		
		/*	usCommandId				    */  501, 
		/*	usPortNumber     			*/	1,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DO_LINE_2_VALUE"},
		
		/*	usCommandId				    */  502, 
		/*	usPortNumber     			*/	2,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DO_LINE_3_VALUE"},
		
		/*	usCommandId				    */  503, 
		/*	usPortNumber     			*/	3,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DO_LINE_4_VALUE"},
		
		/*	usCommandId				    */  504, 
		/*	usPortNumber     			*/	4,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DO_LINE_5_VALUE"},
		
		/*	usCommandId				    */  505, 
		/*	usPortNumber     			*/	5,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DO_LINE_6_VALUE"},
		
		/*	usCommandId				    */  506, 
		/*	usPortNumber     			*/	6,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DO_LINE_7_VALUE"},
		
		/*	usCommandId				    */  507, 
		/*	usPortNumber     			*/	7,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DO_LINE_8_VALUE"},
		
		/*	usCommandId				    */  508, 
		/*	usPortNumber     			*/	8,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DO_LINE_9_VALUE"},
		
		/*	usCommandId				    */  509, 
		/*	usPortNumber     			*/	9,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DO_LINE_10_VALUE"},
		
		/*	usCommandId				    */  510, 
		/*	usPortNumber     			*/	10,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DO_LINE_11_VALUE"},
		
		/*	usCommandId				    */  511, 
		/*	usPortNumber     			*/	11,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DO_LINE_12_VALUE"},
		
		/*	usCommandId				    */  512, 
		/*	usPortNumber     			*/	12,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DO_LINE_13_VALUE"},
		
		/*	usCommandId				    */  513, 
		/*	usPortNumber     			*/	13,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DO_LINE_14_VALUE"},
		
		/*	usCommandId				    */  514, 
		/*	usPortNumber     			*/	14,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DO_LINE_15_VALUE"},
		
		/*	usCommandId				    */  515, 
		/*	usPortNumber     			*/	15,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	
	/***** ***** ***** ***** ***** ***** Get AI Line Config Commands ***** ***** ***** ***** ***** ***** *****/
	
	{
		/*	szCommandName				*/	{"GET_AI_LINE_0_CONFIG"},
		
		/*	usCommandId				    */  600, 
		/*	usPortNumber     			*/	0,

		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_AI_LINE_1_CONFIG"},
		
		/*	usCommandId				    */  601, 
		/*	usPortNumber     			*/	1,

		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_AI_LINE_2_CONFIG"},
		
		/*	usCommandId				    */  602, 
		/*	usPortNumber     			*/	2,

		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_AI_LINE_3_CONFIG"},
		
		/*	usCommandId				    */  603, 
		/*	usPortNumber     			*/	3,

		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_AI_LINE_4_CONFIG"},
		
		/*	usCommandId				    */  604, 
		/*	usPortNumber     			*/	4,

		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_AI_LINE_5_CONFIG"},
		
		/*	usCommandId				    */  605, 
		/*	usPortNumber     			*/	5,

		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_AI_LINE_6_CONFIG"},
		
		/*	usCommandId				    */  606, 
		/*	usPortNumber     			*/	6,

		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_AI_LINE_7_CONFIG"},
		
		/*	usCommandId				    */  607, 
		/*	usPortNumber     			*/	7,

		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
	
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	}, 
	
	/***** ***** ***** ***** ***** ***** Get DI Line Config Commands ***** ***** ***** ***** ***** ***** *****/
	
	{
		/*	szCommandName				*/	{"GET_DI_LINE_0_CONFIG"},
		
		/*	usCommandId				    */  700, 
		/*	usPortNumber     			*/	0,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_1_CONFIG"},
		
		/*	usCommandId				    */  701, 
		/*	usPortNumber     			*/	1,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_2_CONFIG"},
		
		/*	usCommandId				    */  702, 
		/*	usPortNumber     			*/	2,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_3_CONFIG"},
		
		/*	usCommandId				    */  703, 
		/*	usPortNumber     			*/	3,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_4_CONFIG"},
		
		/*	usCommandId				    */  704, 
		/*	usPortNumber     			*/	4,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_5_CONFIG"},
		
		/*	usCommandId				    */  705, 
		/*	usPortNumber     			*/	5,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_6_CONFIG"},
		
		/*	usCommandId				    */  706, 
		/*	usPortNumber     			*/	6,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_7_CONFIG"},
		
		/*	usCommandId				    */  707, 
		/*	usPortNumber     			*/	7,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_8_CONFIG"},
		
		/*	usCommandId				    */  708, 
		/*	usPortNumber     			*/	8,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_9_CONFIG"},
		
		/*	usCommandId				    */  709, 
		/*	usPortNumber     			*/	9,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_10_CONFIG"},
		
		/*	usCommandId				    */  710, 
		/*	usPortNumber     			*/	10,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_11_CONFIG"},
		
		/*	usCommandId				    */  711, 
		/*	usPortNumber     			*/	11,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_12_CONFIG"},
		
		/*	usCommandId				    */  712, 
		/*	usPortNumber     			*/	12,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_13_CONFIG"},
		
		/*	usCommandId				    */  713, 
		/*	usPortNumber     			*/	13,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_14_CONFIG"},
		
		/*	usCommandId				    */  714, 
		/*	usPortNumber     			*/	14,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},
	{
		/*	szCommandName				*/	{"GET_DI_LINE_15_CONFIG"},
		
		/*	usCommandId				    */  715, 
		/*	usPortNumber     			*/	15,
		
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00,
		
		/*	ucReceiveDataType  	 		*/  0x01,
		/*	usReceiveDataLength			*/  0x01
	},

	/***** ***** ***** ***** ***** ***** Set AI Line Config Commands ***** ***** ***** ***** ***** ***** *****/ 

	{
		/*	szCommandName				*/	{"SET_AI_LINE_0_CONFIG"},
		
		/*	usCommandId				    */  800, 
		/*	usPortNumber     			*/	0,
		
		/*	ucSendDataType	     		*/  0x07,
		/*	usSendDataLength   			*/  0x02, 
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_AI_LINE_1_CONFIG"},
		
		/*	usCommandId				    */  801, 
		/*	usPortNumber     			*/	1,
		
		/*	ucSendDataType	     		*/  0x07,
		/*	usSendDataLength   			*/  0x02, 
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_AI_LINE_2_CONFIG"},
		
		/*	usCommandId				    */  802, 
		/*	usPortNumber     			*/	2,
		
		/*	ucSendDataType	     		*/  0x07,
		/*	usSendDataLength   			*/  0x02, 
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_AI_LINE_3_CONFIG"},
		
		/*	usCommandId				    */  803, 
		/*	usPortNumber     			*/	3,
		
		/*	ucSendDataType	     		*/  0x07,
		/*	usSendDataLength   			*/  0x02, 
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_AI_LINE_4_CONFIG"},
		
		/*	usCommandId				    */  804, 
		/*	usPortNumber     			*/	4,
		
		/*	ucSendDataType	     		*/  0x07,
		/*	usSendDataLength   			*/  0x02, 
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_AI_LINE_5_CONFIG"},
		
		/*	usCommandId				    */  805, 
		/*	usPortNumber     			*/	5,
		
		/*	ucSendDataType	     		*/  0x07,
		/*	usSendDataLength   			*/  0x02, 
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_AI_LINE_6_CONFIG"},
		
		/*	usCommandId				    */  806, 
		/*	usPortNumber     			*/	6,
		
		/*	ucSendDataType	     		*/  0x07,
		/*	usSendDataLength   			*/  0x02, 
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_AI_LINE_7_CONFIG"},
		
		/*	usCommandId				    */  807, 
		/*	usPortNumber     			*/	7,
		
		/*	ucSendDataType	     		*/  0x07,
		/*	usSendDataLength   			*/  0x02, 
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},

	/***** ***** ***** ***** ***** ***** Set DI Line Config Commands ***** ***** ***** ***** ***** ***** *****/ 

	{
		/*	szCommandName				*/	{"SET_DI_LINE_0_CONFIG"},
		
		/*	usCommandId				    */  900, 
		/*	usPortNumber     			*/	0,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01,
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DI_LINE_1_CONFIG"},
		
		/*	usCommandId				    */  901, 
		/*	usPortNumber     			*/	1,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01,
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DI_LINE_2_CONFIG"},
		
		/*	usCommandId				    */  902, 
		/*	usPortNumber     			*/	2,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01,
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DI_LINE_3_CONFIG"},
		
		/*	usCommandId				    */  903, 
		/*	usPortNumber     			*/	3,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01,
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DI_LINE_4_CONFIG"},
		
		/*	usCommandId				    */  904, 
		/*	usPortNumber     			*/	4,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01,
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DI_LINE_5_CONFIG"},
		
		/*	usCommandId				    */  905, 
		/*	usPortNumber     			*/	5,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01,
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DI_LINE_6_CONFIG"},
		
		/*	usCommandId				    */  906, 
		/*	usPortNumber     			*/	6,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01,
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DI_LINE_7_CONFIG"},
		
		/*	usCommandId				    */  907, 
		/*	usPortNumber     			*/	7,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01,
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DI_LINE_8_CONFIG"},
		
		/*	usCommandId				    */  908, 
		/*	usPortNumber     			*/	8,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01,
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DI_LINE_9_CONFIG"},
		
		/*	usCommandId				    */  909, 
		/*	usPortNumber     			*/	9,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01,
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DI_LINE_10_CONFIG"},
		
		/*	usCommandId				    */  910, 
		/*	usPortNumber     			*/	10,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01,
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DI_LINE_11_CONFIG"},
		
		/*	usCommandId				    */  910, 
		/*	usPortNumber     			*/	11,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01,
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DI_LINE_12_CONFIG"},
		
		/*	usCommandId				    */  912, 
		/*	usPortNumber     			*/	12,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01,
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DI_LINE_13_CONFIG"},
		
		/*	usCommandId				    */  913, 
		/*	usPortNumber     			*/	13,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01,
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DI_LINE_14_CONFIG"},
		
		/*	usCommandId				    */  914, 
		/*	usPortNumber     			*/	14,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01,
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},
	{
		/*	szCommandName				*/	{"SET_DI_LINE_15_CONFIG"},
		
		/*	usCommandId				    */  915, 
		/*	usPortNumber     			*/	15,
		
		/*	ucSendDataType	     		*/  0x01,
		/*	usSendDataLength   			*/  0x01,
		
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	},

/***** ***** ***** ***** ***** ***** General Commands ***** ***** ***** ***** ***** ***** *****/ 
	
	{
		/*	szCommandName				*/	{"RESET"},
		
		/*	usCommandId				    */  1000,
		/*	usPortNumber     			*/	-1,
	
		/*	ucSendDataType	     		*/  0x00,
		/*	usSendDataLength   			*/  0x00, 
	
		/*	ucReceiveDataType  	 		*/  0x00,
		/*	usReceiveDataLength			*/  0x00
	}
};

//==============================================================================

// Global functions
//==============================================================================   
int	GetCommandItemListSize(void) 
{
	return (sizeof(tSTD_RIO_CommandList) / sizeof(tsSTD_RIO_CommandItem));	
}

//==============================================================================   
