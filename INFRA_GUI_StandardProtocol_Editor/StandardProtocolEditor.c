#include <cvirte.h>		
#include <userint.h>
#include "StandardProtocolEditor.h"
#include "GlobalDefines.h"
#include "toolbox.h"
#include <formatio.h>
#include <ansi_c.h>

#define						MAX_LIST_NUMBER							0xFF

#define			DEVICE_ID				STANDARD_PROTOCOL_DRIVER_DEVICE_ID

tsSTD_CommandItem			gvtListOfCommands[MAX_LIST_NUMBER]	=	{0};
	
int							giCurrentItemIndex					=	0,
							giNumberOfItems						=	0;

int		UpdateList( int panelHandle )
{
	int			iIndex		=	0;
	
	DeleteListItem ( panelHandle, PANEL_COMMAND_NAME_LIST , 0 , -1 );
	
	for ( iIndex = 0 ; iIndex < MAX_LIST_NUMBER; iIndex++ )
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
	int			iIndex								=	giCurrentItemIndex;
	
	char		szStringValue[LOW_STRING]			=	{0};
	
	SetCtrlVal( panelHandle , PANEL_COMMAND_NAME_STRING , gvtListOfCommands[iIndex].szCommandName );
	
	sprintf( szStringValue , "0x%02X" , (long)gvtListOfCommands[iIndex].iResponseValue ); 
	SetCtrlVal( panelHandle , PANEL_REC_DATA_VAL_STR , szStringValue );
	sprintf( szStringValue , "0x%02X" , gvtListOfCommands[iIndex].responseDataSize ); 
	SetCtrlVal( panelHandle , PANEL_REC_DATA_SIZE_STR , szStringValue );    	
	sprintf( szStringValue , "0x%02X" , gvtListOfCommands[iIndex].responseHeader ); 
	SetCtrlVal( panelHandle , PANEL_REC_HEADER_STR , szStringValue );     	
	sprintf( szStringValue , "0x%02X" , gvtListOfCommands[iIndex].responseOpCode ); 
	SetCtrlVal( panelHandle , PANEL_REC_OP_CODE_STR , szStringValue );
	sprintf( szStringValue , "0x%02X" , (long)gvtListOfCommands[iIndex].iSendValue ); 
	SetCtrlVal( panelHandle , PANEL_SEND_DATA_VAL_STR , szStringValue );  	
	sprintf( szStringValue , "0x%02X" , gvtListOfCommands[iIndex].dataSize ); 
	SetCtrlVal( panelHandle , PANEL_SEND_DATA_SIZE_STR , szStringValue ); 	
	sprintf( szStringValue , "0x%02X" , gvtListOfCommands[iIndex].sendHeader ); 
	SetCtrlVal( panelHandle , PANEL_SEND_HEADER_STR , szStringValue );    	
	sprintf( szStringValue , "0x%02X" , gvtListOfCommands[iIndex].opCode ); 
	SetCtrlVal( panelHandle , PANEL_SEND_OP_CODE_STR , szStringValue );
	
	SetCtrlVal( panelHandle , PANEL_SEND_VAR_SIZE , gvtListOfCommands[iIndex].sendingSizeType );
	SetCtrlAttribute( panelHandle , PANEL_SEND_DATA_SIZE_STR , ATTR_DIMMED , gvtListOfCommands[iIndex].sendingSizeType );
	SetCtrlAttribute( panelHandle , PANEL_SEND_DATA_VAL_STR , ATTR_DIMMED , gvtListOfCommands[iIndex].sendingSizeType ); 
	
	SetCtrlVal( panelHandle , PANEL_REC_VAR_SIZE , gvtListOfCommands[iIndex].responseSizeType );
	SetCtrlAttribute( panelHandle , PANEL_REC_DATA_SIZE_STR , ATTR_DIMMED , gvtListOfCommands[iIndex].responseSizeType );
	SetCtrlAttribute( panelHandle , PANEL_REC_DATA_VAL_STR , ATTR_DIMMED , gvtListOfCommands[iIndex].sendingSizeType ); 
	
	return 0;
}										

int		OpenConfigurationFile( char *pszFilePath )
{
	int			iIndex								=	0;

	int			hFileHandle							=	0;
	
	int			iCount								=	0;

	int			bForceChanges						=	0;
	
	if ( FileExists( pszFilePath , NULL ))
		if ( strstr( pszFilePath , ".spcnf" ))
		{
			hFileHandle = OpenFile (pszFilePath, VAL_READ_ONLY, VAL_TRUNCATE, VAL_BINARY);

			memset( gvtListOfCommands , 0 , sizeof(gvtListOfCommands));

			for ( iIndex = 0 ; iIndex < MAX_LIST_NUMBER; iIndex++ )
			{
				iCount = ReadFile ( hFileHandle , gvtListOfCommands[iIndex].max_size , sizeof(tsSTD_CommandItem) );
			
				if ( iCount != sizeof(tsSTD_CommandItem) )
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
	
	if ((panelHandle = LoadPanel (0, "StandardProtocolEditor.uir", PANEL)) < 0)
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

int CVICALLBACK clbExit (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			QuitUserInterface (0); 
			
			break;
	}
	return 0;
}

void CVICALLBACK clbMenuNewFile (int menuBar, int menuItem, void *callbackData, int panel)
{
	memset( gvtListOfCommands , 0 , sizeof(gvtListOfCommands));
	
	giCurrentItemIndex = 0;
	
	UpdateList( panel );
	UpdateCurrentItem( panel );
}

void CVICALLBACK clbMenuOpenFile (int menuBar, int menuItem, void *callbackData, int panel)
{
	char		szFilePath[STD_STRING]				=	{0};
	
	if ( FileSelectPopup ("", "*.spcnf", "*.spcnf", "Load", VAL_LOAD_BUTTON, 0, 1, 1, 1, szFilePath ))
	{
		OpenConfigurationFile( szFilePath );
	
		UpdateList( panel );
		UpdateCurrentItem( panel );
	}
}

void CVICALLBACK clbMenuSaveFile (int menuBar, int menuItem, void *callbackData, int panel)
{
	int			iIndex								=	0;

	char		szFilePath[STD_STRING]				=	{0};

	int			hFileHandle							=	0;
	
	if ( FileSelectPopup ("", "*.spcnf", "*.spcnf", "Save", VAL_SAVE_BUTTON, 0, 1, 1, 1, szFilePath ))
	{
		hFileHandle = OpenFile (szFilePath, VAL_WRITE_ONLY, VAL_TRUNCATE, VAL_BINARY);
		
		for ( iIndex = 0 ; iIndex < giNumberOfItems; iIndex++ )
		{
			gvtListOfCommands[iIndex].ulSignatureID = DEVICE_ID;

			WriteFile ( hFileHandle , gvtListOfCommands[iIndex].max_size , sizeof(tsSTD_CommandItem) );
		}
			
		CloseFile (hFileHandle);
	}
}

void CVICALLBACK clbMenuExit (int menuBar, int menuItem, void *callbackData, int panel)
{
	QuitUserInterface (0);
}

int CVICALLBACK clbValueChanged (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int			iIndex								=	giCurrentItemIndex;
	
	char		szStringValue[LOW_STRING]			=	{0};
	
	long		lValue								=	0;

	switch (event)
	{
		case EVENT_COMMIT:

			GetCtrlVal( panel , control , szStringValue );  
			
			if ( strchr( szStringValue , 'x' ) || strchr( szStringValue , 'X' ))
				sscanf( szStringValue , "%x" , &lValue );
			else
				sscanf( szStringValue , "%d" , &lValue );
			
			switch (control)
			{
				case PANEL_REC_DATA_VAL_STR:
					 
					gvtListOfCommands[iIndex].iResponseValue = lValue;
					break;
					
				case PANEL_REC_DATA_SIZE_STR:
					gvtListOfCommands[iIndex].responseDataSize = lValue; 
					break;
					
				case PANEL_REC_HEADER_STR:
					gvtListOfCommands[iIndex].responseHeader  = lValue; 
					
					break;
					
				case PANEL_REC_OP_CODE_STR:
					gvtListOfCommands[iIndex].responseOpCode  = lValue; 
					break;
					
				case PANEL_SEND_DATA_VAL_STR:
					gvtListOfCommands[iIndex].iSendValue = lValue; 
					break;
					
				case PANEL_SEND_DATA_SIZE_STR:
					gvtListOfCommands[iIndex].dataSize = lValue;  
					break;
					
				case PANEL_SEND_HEADER_STR:
					gvtListOfCommands[iIndex].sendHeader = lValue;  
					break;
					
				case PANEL_SEND_OP_CODE_STR:
					gvtListOfCommands[iIndex].opCode = lValue;  
					break;	
			}
	}
	
	return 0;
}


int CVICALLBACK clbValueNumericChanged (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int			iIndex								=	giCurrentItemIndex;

	int			iValue								=	0;

	switch (event)
	{
		case EVENT_COMMIT:

			GetCtrlVal( panel , control , &iValue );  
			
			switch (control)
			{
				case PANEL_SEND_VAR_SIZE:
					gvtListOfCommands[iIndex].sendingSizeType = iValue;   
					break;
					
				case PANEL_REC_VAR_SIZE:
					gvtListOfCommands[iIndex].responseSizeType = iValue;   
					break;
			}
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

void	SortCommandListByOpCode()
{
	tsSTD_CommandItem			tCopyItem		=	{0},
								*pCheckItem		=	NULL,
								*pPilotItem		=	NULL;
	
	int							index			=	0;
	
	giCurrentItemIndex = 0;
	
	for ( index = 1; index < giNumberOfItems; index++ )
	{
		pCheckItem = &(gvtListOfCommands[index]);
		pPilotItem = pCheckItem;
		
		while( pCheckItem > &(gvtListOfCommands[0]))
		{
			pCheckItem--;
			
			if ( pPilotItem->opCode >= pCheckItem->opCode )
				break;
			
			memcpy( &tCopyItem , pCheckItem , sizeof(tsSTD_CommandItem));
			memcpy( pCheckItem , pPilotItem , sizeof(tsSTD_CommandItem));
			memcpy( pPilotItem , &tCopyItem , sizeof(tsSTD_CommandItem));
			pPilotItem = pCheckItem; 
		};
	}
		   
	return;
}

void CVICALLBACK clbSortByOpCode (int menuBar, int menuItem, void *callbackData, int panel)
{
	SortCommandListByOpCode();
	
	UpdateList( panel ); 
}
