#include <cvirte.h>		
#include <userint.h>
#include "ModBus_ProtocolDefines.h"
#include "GlobalDefines.h"
#include "toolbox.h"
#include <formatio.h>
#include <ansi_c.h>
#include "ModBus_OvenControl_Editor.h"

#define						MAX_LIST_NUMBER							1024
#define						DEVICE_ID								STANDARD_OVEN_CONTROL_MODBUS_DEVICE_ID

tsMODBUS_CommandItem		gvtListOfCommands[MAX_LIST_NUMBER]	=	{0};
	
int							giCurrentItemIndex					=	0,
							giNumberOfItems						=	0;

char						vbFieldEnabled[32][32]				=	{{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},	   // TYPE A
																	 {1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},	   // TYPE B
																	 {1,1,1,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0},	   // TYPE C
																	 {1,1,0,0,1,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0},	   // TYPE D 
																	 {1,1,0,0,1,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0},	   // TYPE E
																	 {1,1,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0},	   // TYPE F  
																	 {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};   // TYPE G
																	 

char						szCurrentFilePath[STD_STRING]		=	{0};

int							bSaveChanges						=	0;

int		UpdateList( int panelHandle )
{
	int			iIndex		=	0;
	
	DeleteListItem ( panelHandle, PANEL_COMMAND_NAME_LIST , 0 , -1 );
	
	for ( iIndex = 0 ; iIndex < giNumberOfItems; iIndex++ )
	{
		if ( strlen(gvtListOfCommands[iIndex].szCommandName) == 0 )
			break;
		
		InsertListItem( panelHandle , PANEL_COMMAND_NAME_LIST ,  -1 , gvtListOfCommands[iIndex].szCommandName , iIndex );   
	}	 
	
	giNumberOfItems = iIndex;
	
	InsertListItem( panelHandle , PANEL_COMMAND_NAME_LIST ,  -1 , "..." , iIndex );   
	
	return 0;
}


int		UpdateCurrentItem( int panelHandle )
{
	int						iIndex								=	giCurrentItemIndex,
							iFieldIndex							=	0,
							iTypeIndex							=	0;
								
	char					szStringValue[LOW_STRING]			=	{0};
	
	tuSweepConvertWord		tConvert							=	{0};
	
	int						iType								=	0;
	
	int						vSendSideControoList[32]			=	{PANEL_SEND_DEVICE,PANEL_SEND_FUNC,PANEL_SEND_ADDRESS,PANEL_SEND_NUM_OF,PANEL_SEND_DATA_LEN,PANEL_SEND_DATA_SIZE,PANEL_SEND_DATA,PANEL_SEND_DATA_MUL,PANEL_SEND_VAL_TYPE,PANEL_SEND_SWAP,PANEL_SEND_BYTE_ORDER},
							vReceiveSideControoList[32]			=	{PANEL_RECEIVE_DEVICE,PANEL_RECEIVE_FUNC,PANEL_RECEIVE_ADDRESS,PANEL_RECEIVE_NUM_OF,PANEL_RECEIVE_DATA_LEN,PANEL_RECEIVE_DATA_SIZE,PANEL_RECEIVE_DATA,PANEL_RECEIVE_DATA_MUL,PANEL_RECEIVE_VAL_TYPE,PANEL_RES_SWAP,PANEL_RES_BYTE_ORDER};

	SetCtrlVal( panelHandle , PANEL_COMMAND_NAME_STRING , gvtListOfCommands[iIndex].szCommandName );
	
	//-------------------------- Send Side ----------------------------------------------//
	iType = gvtListOfCommands[iIndex].iSendType;
	
	SetCtrlVal( panelHandle , PANEL_SEND_CRC , gvtListOfCommands[iIndex].bSendCrcBigEndian );
	
	SetCtrlVal( panelHandle , PANEL_SEND_VAL_TYPE , gvtListOfCommands[iIndex].tSendValueType ); 

	SetCtrlVal( panelHandle , PANEL_SEND_PREFIX , gvtListOfCommands[iIndex].bSendPrefix );
	SetCtrlVal( panelHandle , PANEL_RECEIVE_PREFIX , gvtListOfCommands[iIndex].bResponsePrefix ); 
	
	SetCtrlAttribute( panelHandle , PANEL_SEND_PREFIX_ID , ATTR_DIMMED , (!(gvtListOfCommands[iIndex].bSendPrefix)) );
	SetCtrlAttribute( panelHandle , PANEL_REC_PREFIX_ID , ATTR_DIMMED , (!(gvtListOfCommands[iIndex].bResponsePrefix)) );	
		
	SetCtrlVal( panelHandle , PANEL_SEND_TYPE , iType); 
	GetCtrlIndex( panelHandle , PANEL_SEND_TYPE , &iTypeIndex );
	
	SetCtrlVal( panelHandle , PANEL_SEND_PIC , iType );
	
	SetCtrlVal( panelHandle , PANEL_SEND_SWAP , gvtListOfCommands[iIndex].bSendSwap ); 
	
	SetCtrlVal( panelHandle , PANEL_SEND_BYTE_ORDER , gvtListOfCommands[iIndex].bSendDataByteBigEndian );  
			
	if ( gvtListOfCommands[iIndex].bSendPrefix )
	{
		sprintf( szStringValue , "%d" , gvtListOfCommands[iIndex].iSendPrefix_ID ); 
		SetCtrlVal( panelHandle , PANEL_SEND_PREFIX_ID , szStringValue );
	}
	
	if ( gvtListOfCommands[iIndex].bResponsePrefix )
	{
		sprintf( szStringValue , "%d" , gvtListOfCommands[iIndex].iResponsePrefix_ID ); 
		SetCtrlVal( panelHandle , PANEL_REC_PREFIX_ID , szStringValue );
	}
	
	for( iFieldIndex = 0; iFieldIndex < 32; iFieldIndex++ )
		if ( vSendSideControoList[iFieldIndex] )
			SetCtrlAttribute( panelHandle , vSendSideControoList[iFieldIndex] , ATTR_DIMMED , (!(vbFieldEnabled[iTypeIndex][iFieldIndex])) );
	
	switch( iType )
	{
		case PROTOCOL_TYPE_A:
			
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].iSendDataSize ); 
								SetCtrlVal( panelHandle , PANEL_SEND_DATA_SIZE , szStringValue );
			
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].send_side.tTypeA.dev_addr ); 
								SetCtrlVal( panelHandle , PANEL_SEND_DEVICE , szStringValue );
								
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].send_side.tTypeA.func_code ); 
								SetCtrlVal( panelHandle , PANEL_SEND_FUNC , szStringValue );   
	
								tConvert.byte_hi = gvtListOfCommands[iIndex].send_side.tTypeA.start_addr_hi;
								tConvert.byte_lo = gvtListOfCommands[iIndex].send_side.tTypeA.start_addr_lo;
								
								sprintf( szStringValue , "%d" , tConvert.Word ); 
								SetCtrlVal( panelHandle , PANEL_SEND_ADDRESS , szStringValue );  
	
								tConvert.byte_hi = gvtListOfCommands[iIndex].send_side.tTypeA.num_of_hi;
								tConvert.byte_lo = gvtListOfCommands[iIndex].send_side.tTypeA.num_of_lo;
	
								sprintf( szStringValue , "%d" , tConvert.Word );
								SetCtrlVal( panelHandle , PANEL_SEND_NUM_OF , szStringValue );
	
								sprintf( szStringValue , "%d" , gvtListOfCommands[iIndex].send_side.tTypeA.data_length );
								SetCtrlVal( panelHandle , PANEL_SEND_DATA_LEN , szStringValue );  	
	
								tConvert.Word = atoi( gvtListOfCommands[iIndex].szSendData );
	
								gvtListOfCommands[iIndex].send_side.tTypeA.single_data_hi = tConvert.byte_hi;
								gvtListOfCommands[iIndex].send_side.tTypeA.single_data_lo = tConvert.byte_lo;
	
								SetCtrlVal( panelHandle , PANEL_SEND_DATA , gvtListOfCommands[iIndex].szSendData ); 	
	
								sprintf( szStringValue , "%0.3lf" , gvtListOfCommands[iIndex].lfSendValueMultiply );
								SetCtrlVal( panelHandle , PANEL_SEND_DATA_MUL , szStringValue );  
								
								break;
								
		case PROTOCOL_TYPE_B:
								gvtListOfCommands[iIndex].iSendDataSize = 0;
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].iSendDataSize ); 
								SetCtrlVal( panelHandle , PANEL_SEND_DATA_SIZE , szStringValue );
			
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].send_side.tTypeB.dev_addr ); 
								SetCtrlVal( panelHandle , PANEL_SEND_DEVICE , szStringValue );
								
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].send_side.tTypeB.func_code ); 
								SetCtrlVal( panelHandle , PANEL_SEND_FUNC , szStringValue );   
	
								tConvert.byte_hi = gvtListOfCommands[iIndex].send_side.tTypeB.start_addr_hi;
								tConvert.byte_lo = gvtListOfCommands[iIndex].send_side.tTypeB.start_addr_lo;
								
								sprintf( szStringValue , "%d" , tConvert.Word ); 
								SetCtrlVal( panelHandle , PANEL_SEND_ADDRESS , szStringValue );  
	
								tConvert.byte_hi = gvtListOfCommands[iIndex].send_side.tTypeB.num_of_hi;
								tConvert.byte_lo = gvtListOfCommands[iIndex].send_side.tTypeB.num_of_lo;
	
								sprintf( szStringValue , "%d" , tConvert.Word ); 
								SetCtrlVal( panelHandle , PANEL_SEND_NUM_OF , szStringValue );  
								strcpy( gvtListOfCommands[iIndex].szSendData , szStringValue );
								
								SetCtrlVal( panelHandle , PANEL_SEND_DATA_LEN , "" ); 
								SetCtrlVal( panelHandle , PANEL_SEND_DATA , "" ); 
								SetCtrlVal( panelHandle , PANEL_SEND_DATA_MUL , "" ); 
								
								break;
								
		case PROTOCOL_TYPE_C:
			
								gvtListOfCommands[iIndex].iSendDataSize = 2;
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].iSendDataSize ); 
								SetCtrlVal( panelHandle , PANEL_SEND_DATA_SIZE , szStringValue );
								
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].send_side.tTypeC.dev_addr ); 
								SetCtrlVal( panelHandle , PANEL_SEND_DEVICE , szStringValue );
								
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].send_side.tTypeC.func_code ); 
								SetCtrlVal( panelHandle , PANEL_SEND_FUNC , szStringValue );   
	
								tConvert.byte_hi = gvtListOfCommands[iIndex].send_side.tTypeC.start_addr_hi;
								tConvert.byte_lo = gvtListOfCommands[iIndex].send_side.tTypeC.start_addr_lo;
								
								sprintf( szStringValue , "%d" , tConvert.Word ); 
								SetCtrlVal( panelHandle , PANEL_SEND_ADDRESS , szStringValue );  

								tConvert.Word = atoi( gvtListOfCommands[iIndex].szSendData );
	
								gvtListOfCommands[iIndex].send_side.tTypeC.single_data_hi = tConvert.byte_hi;
								gvtListOfCommands[iIndex].send_side.tTypeC.single_data_lo = tConvert.byte_lo;
	
								SetCtrlVal( panelHandle , PANEL_SEND_DATA , gvtListOfCommands[iIndex].szSendData ); 	
	
								sprintf( szStringValue , "%0.3lf" , gvtListOfCommands[iIndex].lfSendValueMultiply );
								SetCtrlVal( panelHandle , PANEL_SEND_DATA_MUL , szStringValue );  
								
								SetCtrlVal( panelHandle , PANEL_SEND_NUM_OF , "" );
								SetCtrlVal( panelHandle , PANEL_SEND_DATA_LEN , "" );  
								
								break;
								
		case PROTOCOL_TYPE_D:
								gvtListOfCommands[iIndex].iSendDataSize = 1;
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].iSendDataSize ); 
								SetCtrlVal( panelHandle , PANEL_SEND_DATA_SIZE , szStringValue ); 
			
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].send_side.tTypeD.dev_addr ); 
								SetCtrlVal( panelHandle , PANEL_SEND_DEVICE , szStringValue );
								
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].send_side.tTypeD.func_code ); 
								SetCtrlVal( panelHandle , PANEL_SEND_FUNC , szStringValue );   
	
								sprintf( szStringValue , "%d" , gvtListOfCommands[iIndex].send_side.tTypeD.data_length );
								SetCtrlVal( panelHandle , PANEL_SEND_DATA_LEN , szStringValue );  	
	
								tConvert.Word = atoi( gvtListOfCommands[iIndex].szSendData );
	
								gvtListOfCommands[iIndex].send_side.tTypeD.single_data_lo = tConvert.byte_lo;
	
								SetCtrlVal( panelHandle , PANEL_SEND_DATA , gvtListOfCommands[iIndex].szSendData ); 	
	
								sprintf( szStringValue , "%0.3lf" , gvtListOfCommands[iIndex].lfSendValueMultiply );
								SetCtrlVal( panelHandle , PANEL_SEND_DATA_MUL , szStringValue );  
								
								SetCtrlVal( panelHandle , PANEL_SEND_ADDRESS , "" ); 
								SetCtrlVal( panelHandle , PANEL_SEND_NUM_OF , "" );
								
								break;
								
		case PROTOCOL_TYPE_E:
								gvtListOfCommands[iIndex].iSendDataSize = 2;
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].iSendDataSize ); 
								SetCtrlVal( panelHandle , PANEL_SEND_DATA_SIZE , szStringValue );
			
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].send_side.tTypeE.dev_addr ); 
								SetCtrlVal( panelHandle , PANEL_SEND_DEVICE , szStringValue );
								
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].send_side.tTypeE.func_code ); 
								SetCtrlVal( panelHandle , PANEL_SEND_FUNC , szStringValue );   
	
								sprintf( szStringValue , "%d" , gvtListOfCommands[iIndex].send_side.tTypeE.data_length );
								SetCtrlVal( panelHandle , PANEL_SEND_DATA_LEN , szStringValue );  	
	
								tConvert.Word = atoi( gvtListOfCommands[iIndex].szSendData );
	
								gvtListOfCommands[iIndex].send_side.tTypeE.single_data_hi = tConvert.byte_hi;
								gvtListOfCommands[iIndex].send_side.tTypeE.single_data_lo = tConvert.byte_lo;
	
								SetCtrlVal( panelHandle , PANEL_SEND_DATA , gvtListOfCommands[iIndex].szSendData ); 	
	
								sprintf( szStringValue , "%0.3lf" , gvtListOfCommands[iIndex].lfSendValueMultiply );
								SetCtrlVal( panelHandle , PANEL_SEND_DATA_MUL , szStringValue );  
								
								SetCtrlVal( panelHandle , PANEL_SEND_ADDRESS , "" ); 
								SetCtrlVal( panelHandle , PANEL_SEND_NUM_OF , "" );
								
								break;
								
		case PROTOCOL_TYPE_F:
								gvtListOfCommands[iIndex].iSendDataSize = 1;
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].iSendDataSize ); 
								SetCtrlVal( panelHandle , PANEL_SEND_DATA_SIZE , szStringValue ); 
								
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].send_side.tTypeF.dev_addr ); 
								SetCtrlVal( panelHandle , PANEL_SEND_DEVICE , szStringValue );
								
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].send_side.tTypeF.func_code ); 
								SetCtrlVal( panelHandle , PANEL_SEND_FUNC , szStringValue );   
	
								tConvert.Word = atoi( gvtListOfCommands[iIndex].szSendData );
	
								gvtListOfCommands[iIndex].send_side.tTypeF.single_data_lo = tConvert.byte_lo;
	
								SetCtrlVal( panelHandle , PANEL_SEND_DATA , gvtListOfCommands[iIndex].szSendData ); 	
	
								sprintf( szStringValue , "%0.3lf" , gvtListOfCommands[iIndex].lfSendValueMultiply );
								SetCtrlVal( panelHandle , PANEL_SEND_DATA_MUL , szStringValue );  
								
								SetCtrlVal( panelHandle , PANEL_SEND_ADDRESS , "" ); 
								SetCtrlVal( panelHandle , PANEL_SEND_NUM_OF , "" );
								SetCtrlVal( panelHandle , PANEL_SEND_DATA_LEN , "" ); 
								
								break;
								
		case PROTOCOL_TYPE_G:
								gvtListOfCommands[iIndex].iSendDataSize = 0;
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].iSendDataSize ); 
								SetCtrlVal( panelHandle , PANEL_SEND_DATA_SIZE , szStringValue );
								
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].send_side.tTypeG.dev_addr ); 
								SetCtrlVal( panelHandle , PANEL_SEND_DEVICE , szStringValue );
								
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].send_side.tTypeG.func_code ); 
								SetCtrlVal( panelHandle , PANEL_SEND_FUNC , szStringValue );   
	
								SetCtrlVal( panelHandle , PANEL_SEND_ADDRESS , "" ); 
								SetCtrlVal( panelHandle , PANEL_SEND_NUM_OF , "" );
								SetCtrlVal( panelHandle , PANEL_SEND_DATA_LEN , "" ); 
								SetCtrlVal( panelHandle , PANEL_SEND_DATA , "" ); 
								SetCtrlVal( panelHandle , PANEL_SEND_DATA_MUL , "" ); 
								
								break;
		default:
								break;
	}
								
	
	//-------------------------- Receive Side ----------------------------------------------//
	iType = gvtListOfCommands[iIndex].iResponseType;
	
	SetCtrlVal( panelHandle , PANEL_RECEIVE_CRC, gvtListOfCommands[iIndex].bResponseCrcBigEndian );  
	
	SetCtrlVal( panelHandle , PANEL_RECEIVE_VAL_TYPE , gvtListOfCommands[iIndex].tResponseValueType ); 
	
	SetCtrlVal( panelHandle , PANEL_RECEIVE_TYPE , iType); 
	GetCtrlIndex( panelHandle , PANEL_RECEIVE_TYPE , &iTypeIndex );
	
	SetCtrlVal( panelHandle , PANEL_RES_PIC , iType );  
	
	SetCtrlVal( panelHandle , PANEL_RES_SWAP , gvtListOfCommands[iIndex].bResponseSwap ); 
	
	SetCtrlVal( panelHandle , PANEL_RES_BYTE_ORDER , gvtListOfCommands[iIndex].bResponseDataByteBigEndian );
		
	for( iFieldIndex = 0; iFieldIndex < 32; iFieldIndex++ )
		if ( vReceiveSideControoList[iFieldIndex] )
			SetCtrlAttribute( panelHandle , vReceiveSideControoList[iFieldIndex] , ATTR_DIMMED , (!(vbFieldEnabled[iTypeIndex][iFieldIndex])) );
	
	switch( iType )
	{
		case PROTOCOL_TYPE_A:
			
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].iResponseDataSize ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA_SIZE , szStringValue );
			
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].receive_side.tTypeA.dev_addr ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DEVICE , szStringValue );
								
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].receive_side.tTypeA.func_code ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_FUNC , szStringValue );   
	
								tConvert.byte_hi = gvtListOfCommands[iIndex].receive_side.tTypeA.start_addr_hi;
								tConvert.byte_lo = gvtListOfCommands[iIndex].receive_side.tTypeA.start_addr_lo;
								
								sprintf( szStringValue , "%d" , tConvert.Word ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_ADDRESS , szStringValue );  
	
								tConvert.byte_hi = gvtListOfCommands[iIndex].receive_side.tTypeA.num_of_hi;
								tConvert.byte_lo = gvtListOfCommands[iIndex].receive_side.tTypeA.num_of_lo;
	
								sprintf( szStringValue , "%d" , tConvert.Word );
								SetCtrlVal( panelHandle , PANEL_RECEIVE_NUM_OF , szStringValue );
	
								sprintf( szStringValue , "%d" , gvtListOfCommands[iIndex].receive_side.tTypeA.data_length );
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA_LEN , szStringValue );  	
	
								tConvert.Word = atoi( gvtListOfCommands[iIndex].szResponseData );
	
								gvtListOfCommands[iIndex].receive_side.tTypeA.single_data_hi = tConvert.byte_hi;
								gvtListOfCommands[iIndex].receive_side.tTypeA.single_data_lo = tConvert.byte_lo;
	
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA , gvtListOfCommands[iIndex].szResponseData ); 	
	
								sprintf( szStringValue , "%0.3lf" , gvtListOfCommands[iIndex].lfResponseValueMultiply );
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA_MUL , szStringValue ); 
								
								break;
								
		case PROTOCOL_TYPE_B:
								gvtListOfCommands[iIndex].iResponseDataSize = 0;
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].iResponseDataSize ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA_SIZE , szStringValue );
			
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].receive_side.tTypeB.dev_addr ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DEVICE , szStringValue );
								
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].receive_side.tTypeB.func_code ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_FUNC , szStringValue );   
	
								tConvert.byte_hi = gvtListOfCommands[iIndex].receive_side.tTypeB.start_addr_hi;
								tConvert.byte_lo = gvtListOfCommands[iIndex].receive_side.tTypeB.start_addr_lo;
								
								sprintf( szStringValue , "%d" , tConvert.Word ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_ADDRESS , szStringValue );  
	
								tConvert.byte_hi = gvtListOfCommands[iIndex].receive_side.tTypeB.num_of_hi;
								tConvert.byte_lo = gvtListOfCommands[iIndex].receive_side.tTypeB.num_of_lo;
	
								sprintf( szStringValue , "%d" , tConvert.Word ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_NUM_OF , szStringValue );  
								strcpy( gvtListOfCommands[iIndex].szResponseData, szStringValue );
								
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA_LEN , "" ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA , "" ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA_MUL , "" );  
								
								break;
								
		case PROTOCOL_TYPE_C:
								gvtListOfCommands[iIndex].iResponseDataSize = 2;
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].iResponseDataSize ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA_SIZE , szStringValue );
			
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].receive_side.tTypeC.dev_addr ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DEVICE , szStringValue );
								
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].receive_side.tTypeC.func_code ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_FUNC , szStringValue );   
	
								tConvert.byte_hi = gvtListOfCommands[iIndex].receive_side.tTypeC.start_addr_hi;
								tConvert.byte_lo = gvtListOfCommands[iIndex].receive_side.tTypeC.start_addr_lo;
								
								sprintf( szStringValue , "%d" , tConvert.Word ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_ADDRESS , szStringValue );  

								tConvert.Word = atoi( gvtListOfCommands[iIndex].szResponseData );
	
								gvtListOfCommands[iIndex].receive_side.tTypeC.single_data_hi = tConvert.byte_hi;
								gvtListOfCommands[iIndex].receive_side.tTypeC.single_data_lo = tConvert.byte_lo;
	
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA , gvtListOfCommands[iIndex].szResponseData ); 	
	
								sprintf( szStringValue , "%0.3lf" , gvtListOfCommands[iIndex].lfResponseValueMultiply );
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA_MUL , szStringValue ); 
								
								SetCtrlVal( panelHandle , PANEL_RECEIVE_NUM_OF , "" );
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA_LEN , "" );  
								
								break;
								
		case PROTOCOL_TYPE_D:
								gvtListOfCommands[iIndex].iResponseDataSize = 1;
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].iResponseDataSize ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA_SIZE , szStringValue );
			
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].receive_side.tTypeD.dev_addr ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DEVICE , szStringValue );
								
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].receive_side.tTypeD.func_code ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_FUNC , szStringValue );   
	
								sprintf( szStringValue , "%d" , gvtListOfCommands[iIndex].receive_side.tTypeD.data_length );
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA_LEN , szStringValue );  	
	
								tConvert.Word = atoi( gvtListOfCommands[iIndex].szResponseData );
	
								gvtListOfCommands[iIndex].receive_side.tTypeD.single_data_lo = tConvert.byte_lo;
	
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA , gvtListOfCommands[iIndex].szResponseData ); 	
	
								sprintf( szStringValue , "%0.3lf" , gvtListOfCommands[iIndex].lfResponseValueMultiply );
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA_MUL , szStringValue ); 
								
								SetCtrlVal( panelHandle , PANEL_RECEIVE_ADDRESS , "" ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_NUM_OF , "" );
								
								break;
								
		case PROTOCOL_TYPE_E:
								gvtListOfCommands[iIndex].iResponseDataSize = 2;
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].iResponseDataSize ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA_SIZE , szStringValue );
			
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].receive_side.tTypeE.dev_addr ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DEVICE , szStringValue );
								
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].receive_side.tTypeE.func_code ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_FUNC , szStringValue );   
	
								sprintf( szStringValue , "%d" , gvtListOfCommands[iIndex].receive_side.tTypeE.data_length );
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA_LEN , szStringValue );  	
	
								tConvert.Word = atoi( gvtListOfCommands[iIndex].szResponseData );
	
								gvtListOfCommands[iIndex].receive_side.tTypeE.single_data_hi = tConvert.byte_hi;
								gvtListOfCommands[iIndex].receive_side.tTypeE.single_data_lo = tConvert.byte_lo;
	
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA , gvtListOfCommands[iIndex].szResponseData ); 	
	
								sprintf( szStringValue , "%0.3lf" , gvtListOfCommands[iIndex].lfResponseValueMultiply );
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA_MUL , szStringValue ); 
								
								SetCtrlVal( panelHandle , PANEL_RECEIVE_ADDRESS , "" ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_NUM_OF , "" );
								
								break;
								
		case PROTOCOL_TYPE_F:
								gvtListOfCommands[iIndex].iResponseDataSize = 1;
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].iResponseDataSize ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA_SIZE , szStringValue );
								
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].receive_side.tTypeF.dev_addr ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DEVICE , szStringValue );
								
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].receive_side.tTypeF.func_code ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_FUNC , szStringValue );   
	
								tConvert.Word = atoi( gvtListOfCommands[iIndex].szResponseData );
	
								gvtListOfCommands[iIndex].receive_side.tTypeF.single_data_lo = tConvert.byte_lo;
	
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA , gvtListOfCommands[iIndex].szResponseData ); 	
	
								sprintf( szStringValue , "%0.3lf" , gvtListOfCommands[iIndex].lfResponseValueMultiply );
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA_MUL , szStringValue ); 
								
								SetCtrlVal( panelHandle , PANEL_RECEIVE_ADDRESS , "" ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_NUM_OF , "" );
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA_LEN , "" ); 
								
								break;
								
		case PROTOCOL_TYPE_G:
								gvtListOfCommands[iIndex].iResponseDataSize = 0;
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].iResponseDataSize ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA_SIZE , szStringValue );
								
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].receive_side.tTypeG.dev_addr ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DEVICE , szStringValue );
								
								sprintf( szStringValue , "%d" , (long)gvtListOfCommands[iIndex].receive_side.tTypeG.func_code ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_FUNC , szStringValue );   
	
								SetCtrlVal( panelHandle , PANEL_RECEIVE_ADDRESS , "" ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_NUM_OF , "" );
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA_LEN , "" ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA , "" ); 
								SetCtrlVal( panelHandle , PANEL_RECEIVE_DATA_MUL , "" );  

								break;
		default:
								break;
	}
	
	return 0;
}


int		OpenConfigurationFile( char *pszFilePath )
{
	int			iIndex								=	0;

	int			hFileHandle							=	0;
	
	int			iCount								=	0;

	int			bForceChanges						=	0;
	
	if ( FileExists( pszFilePath , NULL ))
		if ( strstr( pszFilePath , ".ovncnf" ))
		{
			strcpy( szCurrentFilePath , pszFilePath );
			
			hFileHandle = OpenFile (pszFilePath, VAL_READ_ONLY, VAL_TRUNCATE, VAL_BINARY);

			memset( gvtListOfCommands , 0 , sizeof(gvtListOfCommands));

			for ( iIndex = 0 ; iIndex < MAX_LIST_NUMBER; iIndex++ )
			{
				iCount = ReadFile ( hFileHandle , gvtListOfCommands[iIndex].max_size , sizeof(tsMODBUS_CommandItem) );
			
				if ( iCount != sizeof(tsMODBUS_CommandItem) )
					break;
				
				if ( gvtListOfCommands[iIndex].ulSignatureID != 0 ) 
					if ( gvtListOfCommands[iIndex].ulSignatureID != DEVICE_ID )
					{
						if ( bForceChanges == 0 )
						{
							if ( ConfirmPopup("Open file error." , "Wrong file format.\nAre you sure you want to adapt format?" ))
								bForceChanges = 1;
							else
								break;
						}
					}
			}
		
			giNumberOfItems = iIndex;
		
			giCurrentItemIndex = 0; 
		
			CloseFile (hFileHandle);
		}
	
	return iIndex;
}

int main (int argc, char *argv[])
{
	int 		panelHandle		=	0; 
	
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	
	if ((panelHandle = LoadPanel (0, "ModBus_OvenControl_Editor.uir", PANEL)) < 0)
		return -1;
	
	DisplayPanel (panelHandle);
	
	if ( argc > 1 )
		OpenConfigurationFile( argv[1] ); 
	
	UpdateList( panelHandle );
	UpdateCurrentItem( panelHandle ); 
	
	RunUserInterface ();
	
	DiscardPanel (panelHandle);

	return 0;
}

int CVICALLBACK panelCB (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:

			break;
	}
	return 0;
}

void		SaveFileAs( int bAskUser , int bAskForSaveChanges )
{
	char		szFilePath[STD_STRING]				=	{0},
				szPath[STD_STRING]					=	{0},
				szFileName[LOW_STRING]				=	{0};

	char		*pTemp								=	NULL;
	
	int			hFileHandle							=	0;
	
	int			iIndex								=	0;
	
	if ( bSaveChanges && (( bAskUser == 0 ) || ( bAskForSaveChanges == 0 ) || ConfirmPopup ("Save Changes", "Do you want to save last changes?")))
	{
		strcpy( szFilePath , szCurrentFilePath );
		strcpy( szPath , szCurrentFilePath );    
	
		pTemp = strrchr( szPath , '\\' );
	
		if ( pTemp )
		{
			*pTemp++ = 0;

			strcpy( szFileName , pTemp );
		}
	
		if (( bAskUser == 0 ) || ( FileSelectPopup (szPath, szFileName , "*.ovncnf", "Save", VAL_SAVE_BUTTON, 0, 1, 1, 1, szFilePath )))
		{
			hFileHandle = OpenFile (szFilePath, VAL_WRITE_ONLY, VAL_TRUNCATE, VAL_BINARY);

			for ( iIndex = 0 ; iIndex < giNumberOfItems; iIndex++ )
			{
				gvtListOfCommands[iIndex].ulSignatureID = DEVICE_ID;

				WriteFile ( hFileHandle , gvtListOfCommands[iIndex].max_size , sizeof(tsSTD_Extended_CommandItem) );
			}
	
			CloseFile (hFileHandle);
			
			bSaveChanges = 0;
		}
	}
	
	return;
}


int CVICALLBACK clbExit (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			SaveFileAs( 1 , 1 );  
			
			QuitUserInterface (0); 
			
			break;
	}
	return 0;
}

void CVICALLBACK clbMenuNewFile (int menuBar, int menuItem, void *callbackData, int panel)
{
	memset( gvtListOfCommands , 0 , sizeof(gvtListOfCommands));
	
	bSaveChanges = 1;
	
	giCurrentItemIndex = 0;
	
	UpdateList( panel );
	UpdateCurrentItem( panel );
}

void CVICALLBACK clbMenuOpenFile (int menuBar, int menuItem, void *callbackData, int panel)
{
	char		szFilePath[STD_STRING]				=	{0};
	
	SaveFileAs( 1 , 1 ); 
	
	if ( FileSelectPopup ("", "*.ovncnf", "*.ovncnf", "Load", VAL_LOAD_BUTTON, 0, 1, 1, 1, szFilePath ))
	{
		bSaveChanges = 0;
		
		OpenConfigurationFile( szFilePath );
	
		UpdateList( panel );
		UpdateCurrentItem( panel );
	}
}

void CVICALLBACK clbMenuSaveFile (int menuBar, int menuItem, void *callbackData, int panel)
{
	SaveFileAs( 0 , 0 );
}

void CVICALLBACK clbMenuSaveAsFile (int menuBar, int menuItem, void *callbackData, int panel)
{
	int				bLastSaveChanges = bSaveChanges;
	
	bSaveChanges = 1;
	
	SaveFileAs( 1 , 0 );
	
	if ( bSaveChanges )
		bSaveChanges = bLastSaveChanges;  
}

void CVICALLBACK clbMenuExit (int menuBar, int menuItem, void *callbackData, int panel)
{
	SaveFileAs( 1 , 1 ); 
	
	QuitUserInterface (0);
}

int CVICALLBACK clbValueChanged (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int						iIndex								=	giCurrentItemIndex;
	
	char					szStringValue[LOW_STRING]			=	{0};
	
	int						iValue								=	0; 
	
	long long				lValue								=	0;

	int						iSendType							=	0,
							iResponseType						=	0;
								
	double					lfMultiply							=	0.0;
	
	tuSweepConvertWord		tConvert							=	{0};

	int						iControlType						=	0;
	
	switch (event)
	{
		case EVENT_COMMIT:

			bSaveChanges = 1;
			
			GetCtrlAttribute (panel, control, ATTR_CTRL_STYLE, &iControlType);
			
			if (( iControlType == CTRL_STRING ) || ( iControlType == CTRL_STRING_LS ))
			{
				GetCtrlVal( panel , control , szStringValue );  
			
				if ( strchr( szStringValue , 'x' ) || strchr( szStringValue , 'X' ))
					sscanf( szStringValue , "%x" , &iValue );
				else
				{
					if ( strlen(szStringValue) > 9 )
						sscanf( szStringValue , "%lld" , &iValue );
					else
						sscanf( szStringValue , "%d" , &iValue );
				}
			
				lValue = iValue;
			
				tConvert.Word = (unsigned short) lValue; 
				
				if ( strchr( szStringValue , '.' )) 
					lfMultiply = atof(szStringValue);
				else
					lfMultiply = iValue;
			}
			
			GetCtrlVal( panel , PANEL_SEND_TYPE , &iSendType ); 
					
			gvtListOfCommands[iIndex].iSendType = iSendType;
				
			GetCtrlVal( panel , PANEL_RECEIVE_TYPE , &iResponseType ); 
					
			gvtListOfCommands[iIndex].iResponseType = iResponseType;
			
			GetCtrlVal( panel , PANEL_SEND_CRC , &(gvtListOfCommands[iIndex].bSendCrcBigEndian) ); 
			GetCtrlVal( panel , PANEL_RECEIVE_CRC , &(gvtListOfCommands[iIndex].bResponseCrcBigEndian) ); 

			GetCtrlVal( panel , PANEL_SEND_VAL_TYPE , (int*)(&(gvtListOfCommands[iIndex].tSendValueType))); 
			GetCtrlVal( panel , PANEL_RECEIVE_VAL_TYPE , (int*)(&(gvtListOfCommands[iIndex].tResponseValueType))); 
			
			SetCtrlVal( panel , PANEL_SEND_PIC , iSendType );
			SetCtrlVal( panel , PANEL_RES_PIC , iResponseType );  
			
			GetCtrlVal( panel , PANEL_RES_SWAP , &(gvtListOfCommands[iIndex].bResponseSwap));
			GetCtrlVal( panel , PANEL_SEND_SWAP , &(gvtListOfCommands[iIndex].bSendSwap));  
			
			GetCtrlVal( panel , PANEL_RECEIVE_PREFIX , &(gvtListOfCommands[iIndex].bResponsePrefix));
			GetCtrlVal( panel , PANEL_SEND_PREFIX , &(gvtListOfCommands[iIndex].bSendPrefix));  
			
			GetCtrlVal( panel , PANEL_RES_BYTE_ORDER , &(gvtListOfCommands[iIndex].bResponseDataByteBigEndian));
			GetCtrlVal( panel , PANEL_SEND_BYTE_ORDER , &(gvtListOfCommands[iIndex].bSendDataByteBigEndian));  
			
			switch (control)
			{
				
				//--------------------------------- Send Data Multiply ----------------------------------------//
				case PANEL_SEND_DATA_MUL:
				
							gvtListOfCommands[iIndex].lfSendValueMultiply = lfMultiply;   

							break;
							
				//-------------------------------------- Send Side ----------------------------------------//
				case PANEL_SEND_DATA_SIZE:
				
							gvtListOfCommands[iIndex].iSendDataSize = iValue; 
							
							break;
							
				case PANEL_SEND_PREFIX_ID:			
				
							gvtListOfCommands[iIndex].iSendPrefix_ID = iValue;
						
							break;			

				case PANEL_SEND_DEVICE:
					
					switch( iSendType )
					{
						case PROTOCOL_TYPE_A:
							
							gvtListOfCommands[iIndex].send_side.tTypeA.dev_addr = (unsigned char)lValue;  
												break;
								
						case PROTOCOL_TYPE_B:
							
							gvtListOfCommands[iIndex].send_side.tTypeB.dev_addr = (unsigned char)lValue; 
												break;
								
						case PROTOCOL_TYPE_C:
							
							gvtListOfCommands[iIndex].send_side.tTypeC.dev_addr = (unsigned char)lValue; 
												break;
								
						case PROTOCOL_TYPE_D:
							
							gvtListOfCommands[iIndex].send_side.tTypeD.dev_addr = (unsigned char)lValue; 
												break;
								
						case PROTOCOL_TYPE_E:
							
							gvtListOfCommands[iIndex].send_side.tTypeE.dev_addr = (unsigned char)lValue; 
												break;
								
						case PROTOCOL_TYPE_F:
							
							gvtListOfCommands[iIndex].send_side.tTypeF.dev_addr = (unsigned char)lValue; 
												break;
																										 
						case PROTOCOL_TYPE_G:
							
							gvtListOfCommands[iIndex].send_side.tTypeG.dev_addr = (unsigned char)lValue; 
												break;
						default:
												break;
					}
					
					break;
					
				case PANEL_SEND_FUNC:
					
					switch( iSendType )
					{
						case PROTOCOL_TYPE_A:
							
							gvtListOfCommands[iIndex].send_side.tTypeA.func_code = (unsigned char)lValue;  
												break;
								
						case PROTOCOL_TYPE_B:
							
							gvtListOfCommands[iIndex].send_side.tTypeB.func_code = (unsigned char)lValue; 
												break;
								
						case PROTOCOL_TYPE_C:
							
							gvtListOfCommands[iIndex].send_side.tTypeC.func_code = (unsigned char)lValue; 
												break;
								
						case PROTOCOL_TYPE_D:
							
							gvtListOfCommands[iIndex].send_side.tTypeD.func_code = (unsigned char)lValue; 
												break;
								
						case PROTOCOL_TYPE_E:
							
							gvtListOfCommands[iIndex].send_side.tTypeE.func_code = (unsigned char)lValue; 
												break;
								
						case PROTOCOL_TYPE_F:
							
							gvtListOfCommands[iIndex].send_side.tTypeF.func_code = (unsigned char)lValue; 
												break;
																										 
						case PROTOCOL_TYPE_G:
							
							gvtListOfCommands[iIndex].send_side.tTypeG.func_code = (unsigned char)lValue; 
												break;
						default:
												break;
					}
					break;
					
				case PANEL_SEND_ADDRESS:
					
					switch( iSendType )
					{
						case PROTOCOL_TYPE_A:
							
							gvtListOfCommands[iIndex].send_side.tTypeA.start_addr_hi = tConvert.byte_hi;
							gvtListOfCommands[iIndex].send_side.tTypeA.start_addr_lo = tConvert.byte_lo;
												break;
								
						case PROTOCOL_TYPE_B:
							
							gvtListOfCommands[iIndex].send_side.tTypeB.start_addr_hi = tConvert.byte_hi;
							gvtListOfCommands[iIndex].send_side.tTypeB.start_addr_lo = tConvert.byte_lo;
												break;
								
						case PROTOCOL_TYPE_C:
							
							gvtListOfCommands[iIndex].send_side.tTypeC.start_addr_hi = tConvert.byte_hi;
							gvtListOfCommands[iIndex].send_side.tTypeC.start_addr_lo = tConvert.byte_lo;
												break;
								
						case PROTOCOL_TYPE_D:
												break;
								
						case PROTOCOL_TYPE_E:
												break;
								
						case PROTOCOL_TYPE_F:
												break;
																										 
						case PROTOCOL_TYPE_G:
												break;
						default:
												break;
					}
	
					break;
					
				case PANEL_SEND_NUM_OF:
					
					
					switch( iSendType )
					{
						case PROTOCOL_TYPE_A:
							
							gvtListOfCommands[iIndex].send_side.tTypeA.num_of_hi = tConvert.byte_hi;
							gvtListOfCommands[iIndex].send_side.tTypeA.num_of_lo = tConvert.byte_lo;
												break;
								
						case PROTOCOL_TYPE_B:
							
							gvtListOfCommands[iIndex].send_side.tTypeB.num_of_hi = tConvert.byte_hi;
							gvtListOfCommands[iIndex].send_side.tTypeB.num_of_lo = tConvert.byte_lo;
												break;
								
						case PROTOCOL_TYPE_C:
												break;
								
						case PROTOCOL_TYPE_D:
												break;
								
						case PROTOCOL_TYPE_E:
												break;
								
						case PROTOCOL_TYPE_F:
												break;
																										 
						case PROTOCOL_TYPE_G:
												break;
						default:
												break;
					}
					
					break;
					
				case PANEL_SEND_DATA_LEN:
					
					switch( iSendType )
					{
						case PROTOCOL_TYPE_A:
							
							gvtListOfCommands[iIndex].send_side.tTypeA.data_length = (unsigned char)lValue;  
												break;
								
						case PROTOCOL_TYPE_B:
												break;
								
						case PROTOCOL_TYPE_C:
												break;
								
						case PROTOCOL_TYPE_D:
							
							gvtListOfCommands[iIndex].send_side.tTypeD.data_length = (unsigned char)lValue; 
												break;
								
						case PROTOCOL_TYPE_E:
							
							gvtListOfCommands[iIndex].send_side.tTypeE.data_length = (unsigned char)lValue; 
												break;
								
						case PROTOCOL_TYPE_F:
												break;
																										 
						case PROTOCOL_TYPE_G:
												break;
						default:
												break;
					}
					break;
					
				case PANEL_SEND_DATA:
					
					strcpy( gvtListOfCommands[iIndex].szSendData , szStringValue ); 
					
					switch( iSendType )
					{
						case PROTOCOL_TYPE_A:
							
							gvtListOfCommands[iIndex].send_side.tTypeA.single_data_hi = tConvert.byte_hi;
							gvtListOfCommands[iIndex].send_side.tTypeA.single_data_lo = tConvert.byte_lo;
												break;
								
						case PROTOCOL_TYPE_B:
												break;
								
						case PROTOCOL_TYPE_C:
							
							gvtListOfCommands[iIndex].send_side.tTypeC.single_data_hi = tConvert.byte_hi;
							gvtListOfCommands[iIndex].send_side.tTypeC.single_data_lo = tConvert.byte_lo;
												break;
								
						case PROTOCOL_TYPE_D:
												break;
								
						case PROTOCOL_TYPE_E:
							
							gvtListOfCommands[iIndex].send_side.tTypeE.single_data_hi = tConvert.byte_hi;
							gvtListOfCommands[iIndex].send_side.tTypeE.single_data_lo = tConvert.byte_lo;
												break;
								
						case PROTOCOL_TYPE_F:
												break;
																										 
						case PROTOCOL_TYPE_G:
												break;
						default:
												break;
					}
					
					break;

				//-------------------------------------- Receive Side ----------------------------------------//
				
				//--------------------------------- Send Data Multiply ----------------------------------------//
				case PANEL_RECEIVE_DATA_MUL:
				
							gvtListOfCommands[iIndex].lfResponseValueMultiply = lfMultiply;   

							break;
							
				case PANEL_RECEIVE_DATA_SIZE:
				
							gvtListOfCommands[iIndex].iResponseDataSize = iValue; 
							
							break;
							
				case PANEL_REC_PREFIX_ID:			
				
							gvtListOfCommands[iIndex].iResponsePrefix_ID = iValue;
						
							break;
	
				case PANEL_RECEIVE_DEVICE:
					
					switch( iResponseType )
					{
						case PROTOCOL_TYPE_A:
							
							gvtListOfCommands[iIndex].receive_side.tTypeA.dev_addr = (unsigned char)lValue;  
												break;
								
						case PROTOCOL_TYPE_B:
							
							gvtListOfCommands[iIndex].receive_side.tTypeB.dev_addr = (unsigned char)lValue; 
												break;
								
						case PROTOCOL_TYPE_C:
							
							gvtListOfCommands[iIndex].receive_side.tTypeC.dev_addr = (unsigned char)lValue; 
												break;
								
						case PROTOCOL_TYPE_D:
							
							gvtListOfCommands[iIndex].receive_side.tTypeD.dev_addr = (unsigned char)lValue; 
												break;
								
						case PROTOCOL_TYPE_E:
							
							gvtListOfCommands[iIndex].receive_side.tTypeE.dev_addr = (unsigned char)lValue; 
												break;
								
						case PROTOCOL_TYPE_F:
							
							gvtListOfCommands[iIndex].receive_side.tTypeF.dev_addr = (unsigned char)lValue; 
												break;
																										 
						case PROTOCOL_TYPE_G:
							
							gvtListOfCommands[iIndex].receive_side.tTypeG.dev_addr = (unsigned char)lValue; 
												break;
						default:
												break;
					}
					
					break;
					
				case PANEL_RECEIVE_FUNC:
					
					switch( iResponseType )
					{
						case PROTOCOL_TYPE_A:
							
							gvtListOfCommands[iIndex].receive_side.tTypeA.func_code = (unsigned char)lValue;  
												break;
								
						case PROTOCOL_TYPE_B:
							
							gvtListOfCommands[iIndex].receive_side.tTypeB.func_code = (unsigned char)lValue; 
												break;
								
						case PROTOCOL_TYPE_C:
							
							gvtListOfCommands[iIndex].receive_side.tTypeC.func_code = (unsigned char)lValue; 
												break;
								
						case PROTOCOL_TYPE_D:
							
							gvtListOfCommands[iIndex].receive_side.tTypeD.func_code = (unsigned char)lValue; 
												break;
								
						case PROTOCOL_TYPE_E:
							
							gvtListOfCommands[iIndex].receive_side.tTypeE.func_code = (unsigned char)lValue; 
												break;
								
						case PROTOCOL_TYPE_F:
							
							gvtListOfCommands[iIndex].receive_side.tTypeF.func_code = (unsigned char)lValue; 
												break;
																										 
						case PROTOCOL_TYPE_G:
							
							gvtListOfCommands[iIndex].receive_side.tTypeG.func_code = (unsigned char)lValue; 
												break;
						default:
												break;
					}
					break;
					
				case PANEL_RECEIVE_ADDRESS:
					
					switch( iResponseType )
					{
						case PROTOCOL_TYPE_A:
							
							gvtListOfCommands[iIndex].receive_side.tTypeA.start_addr_hi = tConvert.byte_hi;
							gvtListOfCommands[iIndex].receive_side.tTypeA.start_addr_lo = tConvert.byte_lo;
												break;
								
						case PROTOCOL_TYPE_B:
							
							gvtListOfCommands[iIndex].receive_side.tTypeB.start_addr_hi = tConvert.byte_hi;
							gvtListOfCommands[iIndex].receive_side.tTypeB.start_addr_lo = tConvert.byte_lo;
												break;
								
						case PROTOCOL_TYPE_C:
							
							gvtListOfCommands[iIndex].receive_side.tTypeC.start_addr_hi = tConvert.byte_hi;
							gvtListOfCommands[iIndex].receive_side.tTypeC.start_addr_lo = tConvert.byte_lo;
												break;
								
						case PROTOCOL_TYPE_D:
												break;
								
						case PROTOCOL_TYPE_E:
												break;
								
						case PROTOCOL_TYPE_F:
												break;
																										 
						case PROTOCOL_TYPE_G:
												break;
						default:
												break;
					}
	
					break;
					
				case PANEL_RECEIVE_NUM_OF:
					
					
					switch( iResponseType )
					{
						case PROTOCOL_TYPE_A:
							
							gvtListOfCommands[iIndex].receive_side.tTypeA.num_of_hi = tConvert.byte_hi;
							gvtListOfCommands[iIndex].receive_side.tTypeA.num_of_lo = tConvert.byte_lo;
												break;
								
						case PROTOCOL_TYPE_B:
							
							gvtListOfCommands[iIndex].receive_side.tTypeB.num_of_hi = tConvert.byte_hi;
							gvtListOfCommands[iIndex].receive_side.tTypeB.num_of_lo = tConvert.byte_lo;
												break;
								
						case PROTOCOL_TYPE_C:
												break;
								
						case PROTOCOL_TYPE_D:
												break;
								
						case PROTOCOL_TYPE_E:
												break;
								
						case PROTOCOL_TYPE_F:
												break;
																										 
						case PROTOCOL_TYPE_G:
												break;
						default:
												break;
					}
					
					break;
					
				case PANEL_RECEIVE_DATA_LEN:
					
					switch( iResponseType )
					{
						case PROTOCOL_TYPE_A:
							
							gvtListOfCommands[iIndex].receive_side.tTypeA.data_length = (unsigned char)lValue;  
												break;
								
						case PROTOCOL_TYPE_B:
												break;
								
						case PROTOCOL_TYPE_C:
												break;
								
						case PROTOCOL_TYPE_D:
							
							gvtListOfCommands[iIndex].receive_side.tTypeD.data_length = (unsigned char)lValue; 
												break;
								
						case PROTOCOL_TYPE_E:
							
							gvtListOfCommands[iIndex].receive_side.tTypeE.data_length = (unsigned char)lValue; 
												break;
								
						case PROTOCOL_TYPE_F:
												break;
																										 
						case PROTOCOL_TYPE_G:
												break;
						default:
												break;
					}
					break;
					
				case PANEL_RECEIVE_DATA:
					
					strcpy( gvtListOfCommands[iIndex].szResponseData , szStringValue ); 
					
					switch( iResponseType )
					{
						case PROTOCOL_TYPE_A:
							
							gvtListOfCommands[iIndex].receive_side.tTypeA.single_data_hi = tConvert.byte_hi;
							gvtListOfCommands[iIndex].receive_side.tTypeA.single_data_lo = tConvert.byte_lo;
												break;
								
						case PROTOCOL_TYPE_B:
												break;
								
						case PROTOCOL_TYPE_C:
							
							gvtListOfCommands[iIndex].receive_side.tTypeC.single_data_hi = tConvert.byte_hi;
							gvtListOfCommands[iIndex].receive_side.tTypeC.single_data_lo = tConvert.byte_lo;
												break;
								
						case PROTOCOL_TYPE_D:
												break;
								
						case PROTOCOL_TYPE_E:
							
							gvtListOfCommands[iIndex].receive_side.tTypeE.single_data_hi = tConvert.byte_hi;
							gvtListOfCommands[iIndex].receive_side.tTypeE.single_data_lo = tConvert.byte_lo;
												break;
								
						case PROTOCOL_TYPE_F:
												break;
																										 
						case PROTOCOL_TYPE_G:
												break;
						default:
												break;
					}
					
					break;
			}
			
			UpdateCurrentItem( panel );  
	}
	
	return 0;
}

int CVICALLBACK clbCommandsList (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_VAL_CHANGED:

			GetCtrlIndex ( panel , control , &giCurrentItemIndex );
			
			UpdateCurrentItem( panel );
				
			break;
			
		case EVENT_KEYPRESS:
			
			if ( GetKeyPressEventVirtualKey (eventData2) == VAL_FWD_DELETE_VKEY )
				clbMenuDeleteItem ( 0 , 0 , 0 , panel );
			
			break;			
	}
	return 0;
}

int CVICALLBACK clbCommandNameChanged (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int			iIndex								=	giCurrentItemIndex;
	
	char		szStringValue[LOW_STRING]			=	{0};

	char		*pTemp								=	NULL;
	
	switch (event)
	{
		case EVENT_COMMIT:

			bSaveChanges = 1;
			
			GetCtrlVal( panel , control , szStringValue );  
			
			StringUpperCase (szStringValue);
			RemoveSurroundingWhiteSpace (szStringValue);

			do
			{
				pTemp = strchr( szStringValue , ' ' );
				
				if ( pTemp == NULL )
					break;
				
				*pTemp = '_';
				
			}while(1);
			
			strcpy( gvtListOfCommands[iIndex].szCommandName , szStringValue );
			
			ReplaceListItem ( panel , PANEL_COMMAND_NAME_LIST , iIndex , szStringValue , iIndex );
			
			if ( iIndex == giNumberOfItems )
			{
				InsertListItem( panel , PANEL_COMMAND_NAME_LIST ,  -1 , "..." , iIndex ); 
				giNumberOfItems++;
			}
				
			break;
	}
	return 0;
}

void CVICALLBACK clbMenuDeleteItem (int menuBar, int menuItem, void *callbackData, int panel)
{
	int							iIndex				=	0,
								iCurrentIndex		=	0;
	
	GetCtrlIndex ( panel , PANEL_COMMAND_NAME_LIST , &iCurrentIndex );
			
	bSaveChanges = 1;
	
	if ( iCurrentIndex < giNumberOfItems )
	{
		giNumberOfItems--;
	
		for ( iIndex = giCurrentItemIndex; iIndex < giNumberOfItems; iIndex++ )
			gvtListOfCommands[iIndex] = gvtListOfCommands[iIndex+1];
	
		UpdateList( panel );
		
		if ( iCurrentIndex >= giNumberOfItems )
			iCurrentIndex = giNumberOfItems - 1;
		
		if ( iCurrentIndex < 0 )
			iCurrentIndex = 0;
		
		giCurrentItemIndex = iCurrentIndex;
		
		SetCtrlIndex ( panel , PANEL_COMMAND_NAME_LIST , iCurrentIndex );
		
		UpdateCurrentItem( panel );
	}

	return;
}
