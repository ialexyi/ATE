#include <cvirte.h>		
#include <userint.h>
#include "GlobalDefines.h"
#include <analysis.h>
#include "toolbox.h"
#include <formatio.h>
#include <ansi_c.h>
#include "ICS_Control_Editor.h"

#define						MAX_LIST_NUMBER							0xFF

#define						DEVICE_ID								STANDARD_ICS_DRIVER_DEVICE_ID

typedef enum { DEV_VALUE_TYPE_NONE , DEV_VALUE_TYPE_CHAR , DEV_VALUE_TYPE_SHORT , DEV_VALUE_TYPE_INT , DEV_VALUE_TYPE_LONG , DEV_VALUE_TYPE_FLOAT , DEV_VALUE_TYPE_DOUBLE , DEV_VALUE_TYPE_BUFFER , DEV_VALUE_TYPE_SENTINEL } teValueTypes;

typedef enum { VALUE_MODE_DEFAULT , VALUE_MODE_BIT_POSITION , VALUE_MODE_ON_OFF , VALUE_MODE_SENTINEL } teValueModes;

typedef struct
{
	union
	{
		char							max_size[1024];   
		
		struct					
		{
			char						szCommandName[LOW_STRING];

			int							pPortNumbersArray[32],
										iPortsQuantity;
	
			unsigned __int64			UseBitsMap,
										BitDirectionMask,
										PolarityConfiguration,
										DefValue;
					
			teValueTypes				valueType;
			int							valueCount;
	
			double						lfSetDelay,
										lfSetTimeout;
	
			int							iSPI_ClockBitIndex,
										iSPI_ClockBitPolarity,
										iSPI_DataBitIndex,
										iSPI_EnableBitIndex,
										iSPI_EnableBitPolarity,
										iSPI_ReadBitIndex,
										iSPI_ReadBitPort,
										iSPI_WriteBitsCount,
										iSPI_ReadBitsCount,
										iSPI_ClockBitPhase;
			
			int							iSetDataShiftIndex,
										iGetDataShiftIndex;
			
			int							iInterfaceType;
			
			int							pFeedbackPortNumbersArray[32],
										iFeedbackPortsQuantity;
			
			unsigned __int64			FeedbackBitsMap;

			int							bFeedbackSwaped,
										bDataSwaped;
			
			teValueModes				valueMode,
										feedbackMode;
			
			//------------ Signature --------------------------------//
			unsigned __int64			ulSignatureID;
			//-------------------------------------------------------//
			
			unsigned __int64			StateOnValue, 
							  			StateOffValue;
			
			int							SetValueShiftIndex,
										FeedbackValueShiftIndex;
			
		};
	};
	
} tsSingleDeviceItem;


tsSingleDeviceItem			gvtListOfCommands[MAX_LIST_NUMBER]	=	{0};
	
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
	
	int			iCurrentTabIndex					=	0;
	
	int			tabPanelHandle						=	0;
	
	GetActiveTabPage (panelHandle , PANEL_TAB , &iCurrentTabIndex );
	
	GetPanelHandleFromTabPage ( panelHandle , PANEL_TAB , iCurrentTabIndex, &tabPanelHandle );
	
	SetCtrlVal( panelHandle , PANEL_COMMAND_NAME_STRING , gvtListOfCommands[iIndex].szCommandName );
	
	if ( gvtListOfCommands[iIndex].iInterfaceType == 1 ) 
		SetTabPageAttribute ( panelHandle , PANEL_TAB , 1 , ATTR_VISIBLE, 1 );
	else
		SetTabPageAttribute ( panelHandle , PANEL_TAB , 1 , ATTR_VISIBLE, 0 );
	
	if ( gvtListOfCommands[iIndex].valueMode == VALUE_MODE_ON_OFF ) // On Off 
	{
		SetCtrlAttribute( tabPanelHandle , DEFAULT_STATE_ON_VAL , ATTR_DIMMED , 0 );
		SetCtrlAttribute( tabPanelHandle , DEFAULT_STATE_OFF_VAL , ATTR_DIMMED , 0 );
	}
	else
	{
		SetCtrlAttribute( tabPanelHandle , DEFAULT_STATE_ON_VAL , ATTR_DIMMED , 1 );
		SetCtrlAttribute( tabPanelHandle , DEFAULT_STATE_OFF_VAL , ATTR_DIMMED , 1 );
	}
	
	switch(iCurrentTabIndex)
	{
		case 0:
				sprintf( szStringValue , "0x%02llX" , gvtListOfCommands[iIndex].BitDirectionMask ); 												  
				SetCtrlVal( tabPanelHandle , DEFAULT_BIT_DIR_MASK , szStringValue ); 
				
				//sprintf( szStringValue , "%lf" , gvtListOfCommands[iIndex].lfSetTimeout ); 											
				//SetCtrlVal( tabPanelHandle , DEFAULT_SET_TIMEOUT , szStringValue );     											  
				
				sprintf( szStringValue , "%lf" , gvtListOfCommands[iIndex].lfSetDelay ); 
				SetCtrlVal( tabPanelHandle , DEFAULT_SET_DELAY , szStringValue );												  	  
				
				sprintf( szStringValue , "%d" , gvtListOfCommands[iIndex].valueCount ); 														  
				SetCtrlVal( tabPanelHandle , DEFAULT_VALUE_COUNT , szStringValue ); 
				
				sprintf( szStringValue , "0x%02llX" , gvtListOfCommands[iIndex].DefValue ); 													  
				SetCtrlVal( tabPanelHandle , DEFAULT_DEFAULT_VALUE , szStringValue );  
				
				sprintf( szStringValue , "0x%02llX" , gvtListOfCommands[iIndex].UseBitsMap ); 															  
				SetCtrlVal( tabPanelHandle , DEFAULT_USE_BITS_MAP , szStringValue );												  
						
				if ( gvtListOfCommands[iIndex].iPortsQuantity == 0)
					gvtListOfCommands[iIndex].iPortsQuantity = 1;
				
				sprintf( szStringValue , "%d" , gvtListOfCommands[iIndex].pPortNumbersArray[0] ); 															  
				SetCtrlVal( tabPanelHandle , DEFAULT_PORTS , szStringValue );
				
				sprintf( szStringValue , "%d" , gvtListOfCommands[iIndex].pFeedbackPortNumbersArray[0] ); 															  
				SetCtrlVal( tabPanelHandle , DEFAULT_PORTS_FEEDBACK , szStringValue );
				
				sprintf( szStringValue , "0x%02llX" , gvtListOfCommands[iIndex].FeedbackBitsMap ); 															  
				SetCtrlVal( tabPanelHandle , DEFAULT_BITS_MAP_FEEDBACK , szStringValue );		   
				
				SetCtrlVal( tabPanelHandle , DEFAULT_VALUE_TYPE , gvtListOfCommands[iIndex].valueType );
				
				SetCtrlVal( tabPanelHandle , DEFAULT_VALUE_MODE , gvtListOfCommands[iIndex].valueMode ); 
				
				SetCtrlVal( tabPanelHandle , DEFAULT_FEEDBACK_MODE , gvtListOfCommands[iIndex].feedbackMode ); 
				
				SetCtrlVal( tabPanelHandle , DEFAULT_FEEDBACK_SWAP , gvtListOfCommands[iIndex].bFeedbackSwaped );   
				
				SetCtrlVal( tabPanelHandle , DEFAULT_DATA_SWAP , gvtListOfCommands[iIndex].bDataSwaped ); 
				
				if ( gvtListOfCommands[iIndex].valueMode == VALUE_MODE_ON_OFF ) 
				{
					sprintf( szStringValue , "0x%02llX" , gvtListOfCommands[iIndex].StateOnValue ); 
					SetCtrlVal( tabPanelHandle , DEFAULT_STATE_ON_VAL , szStringValue );
					
					sprintf( szStringValue , "0x%02llX" , gvtListOfCommands[iIndex].StateOffValue );  
					SetCtrlVal( tabPanelHandle , DEFAULT_STATE_OFF_VAL , szStringValue ); 
				}
				
				sprintf( szStringValue , "%d" , gvtListOfCommands[iIndex].SetValueShiftIndex ); 															  
				SetCtrlVal( tabPanelHandle , DEFAULT_SET_VAL_OFFSET , szStringValue );
				
				break; 
				
		case 1:
				SetCtrlVal( tabPanelHandle , SPI_READ_BIT_PORT , gvtListOfCommands[iIndex].iSPI_ReadBitPort );                               
				SetCtrlVal( tabPanelHandle , SPI_ENABLE_BIT_POLARITY , gvtListOfCommands[iIndex].iSPI_EnableBitPolarity ); 								           
				SetCtrlVal( tabPanelHandle , SPI_READ_BIT_INDEX , gvtListOfCommands[iIndex].iSPI_ReadBitIndex );      								           
				SetCtrlVal( tabPanelHandle , SPI_CLOCK_BIT_POLARITY , gvtListOfCommands[iIndex].iSPI_ClockBitPolarity );  								           
				SetCtrlVal( tabPanelHandle , SPI_ENABLE_BIT_INDEX , gvtListOfCommands[iIndex].iSPI_EnableBitIndex );    								           
				SetCtrlVal( tabPanelHandle , SPI_DATA_BIT_INDEX , gvtListOfCommands[iIndex].iSPI_DataBitIndex );      								           
				SetCtrlVal( tabPanelHandle , SPI_CLOCK_BIT_INDEX , gvtListOfCommands[iIndex].iSPI_ClockBitIndex );  
				SetCtrlVal( tabPanelHandle , SPI_CLOCK_BIT_PHASE , gvtListOfCommands[iIndex].iSPI_ClockBitPhase );  
					
				sprintf( szStringValue , "%d" , gvtListOfCommands[iIndex].iSPI_WriteBitsCount ); 															  
				SetCtrlVal( tabPanelHandle , SPI_WRITE_BITS_COUNT , szStringValue );
				
				sprintf( szStringValue , "%d" , gvtListOfCommands[iIndex].iSPI_ReadBitsCount ); 															  
				SetCtrlVal( tabPanelHandle , SPI_READ_BITS_COUNT , szStringValue );
				
				sprintf( szStringValue , "%d" , gvtListOfCommands[iIndex].iSetDataShiftIndex ); 
				SetCtrlVal( tabPanelHandle , SPI_SET_DATA_SHIFT , szStringValue );
				
				sprintf( szStringValue , "%d" , gvtListOfCommands[iIndex].iGetDataShiftIndex ); 
				SetCtrlVal( tabPanelHandle , SPI_GET_DATA_SHIFT , szStringValue );
				
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
		if ( strstr( pszFilePath , ".icscnf" ))
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
	
	if ((panelHandle = LoadPanel (0, "ICS_Control_Editor.uir", PANEL)) < 0)
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
	
	if ( FileSelectPopup ("", "*.icscnf", "*.icscnf", "Load", VAL_LOAD_BUTTON, 0, 1, 1, 1, szFilePath ))
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
	
	if ( FileSelectPopup ("", "*.icscnf", "*.icscnf", "Save", VAL_SAVE_BUTTON, 0, 1, 1, 1, szFilePath ))
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

int CVICALLBACK clbSpiConfigValueChanged (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int			iIndex								=	giCurrentItemIndex;
	
	char		szStringValue[LOW_STRING]			=	{0};
	
	long		lValue								=	0;

	int			iStyle								=	0;
	
	int			iTabIndex							=	0;
	
	switch (event)
	{
		case EVENT_VAL_CHANGED:

			GetCtrlAttribute ( panel , control , ATTR_CTRL_STYLE, &iStyle );
			
			switch(iStyle)
			{
				case CTRL_STRING_LS:
				case CTRL_STRING:	
					
						GetCtrlVal( panel , control , szStringValue );  
			
						if ( strchr( szStringValue , 'x' ) || strchr( szStringValue , 'X' ))
							sscanf( szStringValue , "%x" , &lValue );
						else
							sscanf( szStringValue , "%d" , &lValue );
			
						break;
						
				default:
					
						GetCtrlVal( panel , control , &lValue ); 
						break;
						
			}
			
			switch (control)
			{  
				case SPI_READ_BIT_PORT:
					gvtListOfCommands[iIndex].iSPI_ReadBitPort = lValue;  
					break;
				case SPI_ENABLE_BIT_POLARITY:
					gvtListOfCommands[iIndex].iSPI_EnableBitPolarity = lValue; 
					break;
				case SPI_READ_BIT_INDEX:
					gvtListOfCommands[iIndex].iSPI_ReadBitIndex = lValue; 
					break;
				case SPI_CLOCK_BIT_POLARITY:
					gvtListOfCommands[iIndex].iSPI_ClockBitPolarity = lValue; 
					break;
				case SPI_ENABLE_BIT_INDEX:
					gvtListOfCommands[iIndex].iSPI_EnableBitIndex = lValue;   
					break;
				case SPI_DATA_BIT_INDEX:
					gvtListOfCommands[iIndex].iSPI_DataBitIndex = lValue;
					break;
				case SPI_CLOCK_BIT_INDEX:
					gvtListOfCommands[iIndex].iSPI_ClockBitIndex = lValue;  
					break;
				case SPI_WRITE_BITS_COUNT:
					gvtListOfCommands[iIndex].iSPI_WriteBitsCount = lValue; 
					break;
				case SPI_READ_BITS_COUNT:
					gvtListOfCommands[iIndex].iSPI_ReadBitsCount = lValue; 									          
					break;
				case SPI_CLOCK_BIT_PHASE:
					gvtListOfCommands[iIndex].iSPI_ClockBitPhase = lValue; 									          
					break;
				case SPI_SET_DATA_SHIFT:
					gvtListOfCommands[iIndex].iSetDataShiftIndex = lValue; 
					break;
				case SPI_GET_DATA_SHIFT:
					gvtListOfCommands[iIndex].iGetDataShiftIndex = lValue; 
					break;
			}
				
	}
	
	return 0;
}
				
int CVICALLBACK clbValueChanged (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int			iIndex								=	giCurrentItemIndex;
	
	char		szStringValue[LOW_STRING]			=	{0};
	
	long		lValue								=	0;

	int			iStyle								=	0;
	
	int			iTabIndex							=	0;
	
	switch (event)
	{
		case EVENT_COMMIT:

			GetCtrlAttribute ( panel , control , ATTR_CTRL_STYLE, &iStyle );
			
			switch(iStyle)
			{
				case CTRL_STRING_LS:
				case CTRL_STRING:	
					
						GetCtrlVal( panel , control , szStringValue );  
			
						if ( strchr( szStringValue , 'x' ) || strchr( szStringValue , 'X' ))
							sscanf( szStringValue , "%x" , &lValue );
						else
							sscanf( szStringValue , "%d" , &lValue );
			
						break;
						
				default:
					
						GetCtrlVal( panel , control , &lValue ); 
						break;
						
			}
			
			switch (control)
			{
				case DEFAULT_BIT_DIR_MASK:
				
					gvtListOfCommands[iIndex].BitDirectionMask = lValue; 												  
					break;
			
				//case DEFAULT_SET_TIMEOUT:
				//	   
				//	gvtListOfCommands[iIndex].lfSetTimeout = atof(szStringValue); 											
				//	break;
			
				case DEFAULT_SET_DELAY:
				
					gvtListOfCommands[iIndex].lfSetDelay = atof(szStringValue); 											
					break;
			
				case DEFAULT_VALUE_COUNT:
				
					gvtListOfCommands[iIndex].valueCount = lValue; 														  
					break;
				
				case DEFAULT_DEFAULT_VALUE:
							
					gvtListOfCommands[iIndex].DefValue = lValue; 													  
					break;
			
				case DEFAULT_USE_BITS_MAP:
				
					gvtListOfCommands[iIndex].UseBitsMap = lValue;  															  
					break;
			
				case DEFAULT_PORTS:
								 
					gvtListOfCommands[iIndex].pPortNumbersArray[0] = lValue;   
					gvtListOfCommands[iIndex].iPortsQuantity = 1;
				
					break;
			
				case DEFAULT_PORTS_FEEDBACK:
								 
					gvtListOfCommands[iIndex].pFeedbackPortNumbersArray[0] = lValue;   
					gvtListOfCommands[iIndex].iFeedbackPortsQuantity = 1;
				
					break;	
					
				case DEFAULT_BITS_MAP_FEEDBACK:
				
					gvtListOfCommands[iIndex].FeedbackBitsMap = lValue;  															  
					break;	
				
				case DEFAULT_VALUE_TYPE:
										  
					gvtListOfCommands[iIndex].valueType = lValue; 
					break;
			
				case DEFAULT_INTERFACE_TYPE:
				
					gvtListOfCommands[iIndex].iInterfaceType = lValue; 
					break;
					
				case DEFAULT_VALUE_MODE:
				
					gvtListOfCommands[iIndex].valueMode = lValue; 
					break;	
					
				case DEFAULT_FEEDBACK_MODE:
				
					gvtListOfCommands[iIndex].feedbackMode = lValue; 
					break;
					
				case DEFAULT_FEEDBACK_SWAP:
				
					gvtListOfCommands[iIndex].bFeedbackSwaped = lValue; 
					break;	
					
				case DEFAULT_DATA_SWAP:
				
					gvtListOfCommands[iIndex].bDataSwaped = lValue; 
					break;	
					
				case DEFAULT_STATE_ON_VAL:
					
					gvtListOfCommands[iIndex].StateOnValue = lValue; 
					break;
					
				case DEFAULT_STATE_OFF_VAL:
					
					gvtListOfCommands[iIndex].StateOffValue = lValue; 
					break;	
					
				case DEFAULT_SET_VAL_OFFSET:
					
					gvtListOfCommands[iIndex].SetValueShiftIndex = lValue; 
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


int CVICALLBACK clbChangeTab (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_ACTIVE_TAB_CHANGE:

			GetCtrlIndex ( panel , PANEL_COMMAND_NAME_LIST , &giCurrentItemIndex );
			
			UpdateCurrentItem( panel );
				
			break;
	}
	return 0;
}
