#include <cvirte.h>		
#include <userint.h>
#include "GlobalDefines.h"
#include <analysis.h>
#include "toolbox.h"
#include <formatio.h>
#include <ansi_c.h>
#include "DAQmx_Control_Editor.h"

#define						MAX_LIST_NUMBER							0xFF
#define						DEVICE_ID								STANDARD_DAQMX_DRIVER_DEVICE_ID

typedef enum { DEV_VALUE_TYPE_NONE , DEV_VALUE_TYPE_CHAR , DEV_VALUE_TYPE_SHORT , DEV_VALUE_TYPE_INT , DEV_VALUE_TYPE_LONG , DEV_VALUE_TYPE_FLOAT , DEV_VALUE_TYPE_DOUBLE , DEV_VALUE_TYPE_BUFFER , DEV_VALUE_TYPE_SENTINEL } teValueTypes;

typedef struct
{
	union
	{
		char							max_size[STD_BUFFER_SIZE];   
		
		struct					
		{
			char						szCommandName[LOW_STRING];

			int							iConfigType,
										iMeasureType;
	
			int							pPortNumbersArray[32],
										pBaudRateArray[32], 
										iPortsQuantity;
	
			unsigned __int64			UseBitsMap,
										BitDirectionMask,
										DefValue;
	
			char						bPowerUpState,
										bPullingState;
	
			double						dPowerUpLevel;
				
			char 						szDeviceName[64]; 
					
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
										iSPI_ReadBitPort;
			
			int							iAlalogMeasureNumberOfPoints,
										iAlalogMeasureSimpleRate,
										iAlalogMeasureFunction;		
										
			int							iInterfaceType;
			
			int							iSPI_WriteBitsCount,
										iSPI_ReadBitsCount,
										iSPI_ClockBitPhase;	

			//------------ Signature --------------------------------//
			unsigned __int64			ulSignatureID;
			//-------------------------------------------------------//	
			
			double						lfMinValue,
										lfMaxValue;    
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
	
	int			iPortIndex							=	0,
				iCurrentTabIndex					=	0;
	
	int			tabPanelHandle						=	0;
	
	GetActiveTabPage (panelHandle , PANEL_TAB , &iCurrentTabIndex );
	
	GetPanelHandleFromTabPage ( panelHandle , PANEL_TAB , iCurrentTabIndex, &tabPanelHandle );
	
	SetCtrlVal( panelHandle , PANEL_COMMAND_NAME_STRING , gvtListOfCommands[iIndex].szCommandName );
	
	if (( gvtListOfCommands[iIndex].iConfigType == 1 ) || ( gvtListOfCommands[iIndex].iConfigType == 2 ))
		SetTabPageAttribute ( panelHandle , PANEL_TAB , 2 , ATTR_VISIBLE, 1 );
	else
		SetTabPageAttribute ( panelHandle , PANEL_TAB , 2 , ATTR_VISIBLE, 0 );
	
	if ( gvtListOfCommands[iIndex].iConfigType == 4 ) 
		SetTabPageAttribute ( panelHandle , PANEL_TAB , 1 , ATTR_VISIBLE, 1 );
	else
		SetTabPageAttribute ( panelHandle , PANEL_TAB , 1 , ATTR_VISIBLE, 0 );
	
	switch(iCurrentTabIndex)
	{
		case 0:
				if ( gvtListOfCommands[iIndex].pBaudRateArray[0] )
					sprintf( szStringValue , "%d" , gvtListOfCommands[iIndex].pBaudRateArray[0] ); 							     
				else
					strcpy( szStringValue , "-" ); 							     
				
				SetCtrlVal( tabPanelHandle , DEFAULT_BOUD_RATE , szStringValue );
				
				sprintf( szStringValue , "0x%02llX" , gvtListOfCommands[iIndex].BitDirectionMask ); 												  
				SetCtrlVal( tabPanelHandle , DEFAULT_BIT_DIR_MASK , szStringValue ); 
				
				sprintf( szStringValue , "%lf" , gvtListOfCommands[iIndex].lfSetTimeout ); 											
				SetCtrlVal( tabPanelHandle , DEFAULT_SET_TIMEOUT , szStringValue );     											  
				
				sprintf( szStringValue , "%lf" , gvtListOfCommands[iIndex].lfSetDelay ); 
				SetCtrlVal( tabPanelHandle , DEFAULT_SET_DELAY , szStringValue );												  	  
				
				sprintf( szStringValue , "%lf" , gvtListOfCommands[iIndex].dPowerUpLevel ); 													  
				SetCtrlVal( tabPanelHandle , DEFAULT_POWER_UP_LEVEL , szStringValue );  	
													
				if ( gvtListOfCommands[iIndex].lfMaxValue == 0.0 )
					gvtListOfCommands[iIndex].lfMaxValue = 5.0;
				
				sprintf( szStringValue , "%lf" , gvtListOfCommands[iIndex].lfMaxValue ); 													  
				SetCtrlVal( tabPanelHandle , DEFAULT_MAX_VALUE , szStringValue ); 
				
				sprintf( szStringValue , "%lf" , gvtListOfCommands[iIndex].lfMinValue ); 													  
				SetCtrlVal( tabPanelHandle , DEFAULT_MIN_VALUE , szStringValue ); 
																				
				sprintf( szStringValue , "%d" , gvtListOfCommands[iIndex].valueCount ); 														  
				SetCtrlVal( tabPanelHandle , DEFAULT_VALUE_COUNT , szStringValue ); 
				
				sprintf( szStringValue , "0x%02llX" , gvtListOfCommands[iIndex].DefValue ); 													  
				SetCtrlVal( tabPanelHandle , DEFAULT_DEFAULT_VALUE , szStringValue );  
				
				sprintf( szStringValue , "0x%02llX" , gvtListOfCommands[iIndex].UseBitsMap ); 															  
				SetCtrlVal( tabPanelHandle , DEFAULT_USE_BITS_MAP , szStringValue );												  
						
				if ( gvtListOfCommands[iIndex].iPortsQuantity == 0)
					gvtListOfCommands[iIndex].iPortsQuantity = 1;
				
				memset( szStringValue , 0 , LOW_STRING );
				
				for ( iPortIndex = 0; iPortIndex < gvtListOfCommands[iIndex].iPortsQuantity; iPortIndex++ )
				{
					if ( iPortIndex > 0 )
						strcat( szStringValue , "," );
					
					sprintf( szStringValue + strlen(szStringValue) , "%d" , gvtListOfCommands[iIndex].pPortNumbersArray[iPortIndex] ); 															  
				}
				
				SetCtrlVal( tabPanelHandle , DEFAULT_PORTS , szStringValue );
				
				SetCtrlVal( tabPanelHandle , DEFAULT_PULLING_TYPE , gvtListOfCommands[iIndex].bPullingState );
				
				SetCtrlVal( tabPanelHandle , DEFAULT_DEVICE_NAME , gvtListOfCommands[iIndex].szDeviceName );
				
				SetCtrlVal( tabPanelHandle , DEFAULT_VALUE_TYPE , gvtListOfCommands[iIndex].valueType );
				
				SetCtrlVal( tabPanelHandle , DEFAULT_PILLING_STATE , gvtListOfCommands[iIndex].bPowerUpState );
	
				if ( gvtListOfCommands[iIndex].iConfigType )
					SetCtrlVal( tabPanelHandle , DEFAULT_CONFIG_TYPE , gvtListOfCommands[iIndex].iConfigType );
				else
					SetCtrlIndex( tabPanelHandle , DEFAULT_CONFIG_TYPE , 0 );
					
				if ( gvtListOfCommands[iIndex].iMeasureType )
					SetCtrlVal( tabPanelHandle , DEFAULT_MEASURE_TYPE , gvtListOfCommands[iIndex].iMeasureType );
				else
					SetCtrlIndex( tabPanelHandle , DEFAULT_MEASURE_TYPE , 0 );
				
				break; 
				
		case 1:
				break;
				
		case 2:
				sprintf( szStringValue , "%d" , gvtListOfCommands[iIndex].iAlalogMeasureNumberOfPoints  ); 
				SetCtrlVal( tabPanelHandle , MEAS_CONF_NUMBER_OF_POINTS , szStringValue );
				
				sprintf( szStringValue , "%d" , gvtListOfCommands[iIndex].iAlalogMeasureSimpleRate ); 
				SetCtrlVal( tabPanelHandle , MEAS_CONF_SIMPLE_RATE , szStringValue );
				
				SetCtrlVal( tabPanelHandle , MEAS_CONF_MEASURE_FUNCTION , gvtListOfCommands[iIndex].iAlalogMeasureFunction );
				
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
		if ( strstr( pszFilePath , ".daqcnf" ))
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
	
	if ((panelHandle = LoadPanel (0, "DAQmx_Control_Editor.uir", PANEL)) < 0)
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
	
	if ( FileSelectPopup ("", "*.daqcnf", "*.daqcnf", "Load", VAL_LOAD_BUTTON, 0, 1, 1, 1, szFilePath ))
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
	
	if ( FileSelectPopup ("", "*.daqcnf", "*.daqcnf", "Save", VAL_SAVE_BUTTON, 0, 1, 1, 1, szFilePath ))
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

int CVICALLBACK clbMeasureConfigValueChanged (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
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
				case MEAS_CONF_NUMBER_OF_POINTS:
				
					gvtListOfCommands[iIndex].iAlalogMeasureNumberOfPoints = lValue; 
					break;
				
				case MEAS_CONF_SIMPLE_RATE:
				
					gvtListOfCommands[iIndex].iAlalogMeasureSimpleRate = lValue; 
					break;
				
				case MEAS_CONF_MEASURE_FUNCTION:
				
					gvtListOfCommands[iIndex].iAlalogMeasureFunction = lValue; 
					break;
				
				default:
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
	
	int			iTabIndex							=	0,
				iPortIndex							=	0;
	
	char		*pTemp								=	NULL,
				*pNext								=	NULL,
				*pCurrent							=	NULL;
				
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
				case DEFAULT_BOUD_RATE:
					
					gvtListOfCommands[iIndex].pBaudRateArray[0] = lValue;				     
					break;
			
				case DEFAULT_BIT_DIR_MASK:
				
					gvtListOfCommands[iIndex].BitDirectionMask = lValue; 												  
					break;
			
				case DEFAULT_SET_TIMEOUT:
					   
					gvtListOfCommands[iIndex].lfSetTimeout = atof(szStringValue); 											
					break;
			
				case DEFAULT_SET_DELAY:
				
					gvtListOfCommands[iIndex].lfSetDelay = atof(szStringValue); 											
					break;
			
				case DEFAULT_POWER_UP_LEVEL:
				
					gvtListOfCommands[iIndex].dPowerUpLevel = atof(szStringValue); 											
					break;
			
				case DEFAULT_MIN_VALUE:
					
					gvtListOfCommands[iIndex].lfMinValue = atof(szStringValue); 											
					break;

				case DEFAULT_MAX_VALUE:
					
					gvtListOfCommands[iIndex].lfMaxValue = atof(szStringValue); 											
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
								 
					if ( strchr( szStringValue , ',' ) == NULL )
					{
						gvtListOfCommands[iIndex].pPortNumbersArray[0] = lValue;   
						gvtListOfCommands[iIndex].iPortsQuantity = 1;
					}
					else
					{
						pNext = szStringValue;
						iPortIndex = 0;
						
						do
						{
							pTemp = strchr( pNext , ',' );
							
							pCurrent = pNext;
							
							if ( pTemp )
							{
								*pTemp = 0;
								pNext = pTemp + 1; 
							}
							else
							{
								pNext = NULL;
							}
							
							gvtListOfCommands[iIndex].pPortNumbersArray[iPortIndex++] = atoi(pCurrent);   
							
						}while( pNext != NULL );
						
						gvtListOfCommands[iIndex].iPortsQuantity = iPortIndex; 
					}
				
					break;
			
				case DEFAULT_PULLING_TYPE:
						   
					gvtListOfCommands[iIndex].bPullingState = lValue; 
					break;
			
				case DEFAULT_DEVICE_NAME:
				
					strcpy( gvtListOfCommands[iIndex].szDeviceName , szStringValue );
					break;
			
				case DEFAULT_VALUE_TYPE:
										  
					gvtListOfCommands[iIndex].valueType = lValue; 
					break;
			
				case DEFAULT_PILLING_STATE:
										
					gvtListOfCommands[iIndex].bPowerUpState = lValue; 							
					break;
				
				case DEFAULT_CONFIG_TYPE:
				
					gvtListOfCommands[iIndex].iConfigType = lValue; 
					break;
				
				case DEFAULT_MEASURE_TYPE:
				
					gvtListOfCommands[iIndex].iMeasureType = lValue; 
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
