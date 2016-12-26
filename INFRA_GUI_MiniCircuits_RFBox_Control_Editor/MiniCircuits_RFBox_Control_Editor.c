#include <cvirte.h>		
#include <userint.h>
#include "GlobalDefines.h"
#include <analysis.h>
#include "toolbox.h"
#include <formatio.h>
#include <ansi_c.h>
#include "MiniCircuits_RFBox_Control_Editor.h"

#define			MAX_LIST_NUMBER						1024
#define			DEVICE_ID							STANDARD_MINI_CIRCUITS_RFBOX_DEVICE_ID

typedef enum { VALUE_MODE_DEFAULT , VALUE_MODE_ON_OFF , VALUE_MODE_SENTINEL } teValueModes;

typedef struct
{
	union
	{
		char							max_size[STD_BUFFER_SIZE];   
		
		struct					
		{
			char						szCommandName[LOW_STRING];
			
			char						szSubDeviceName[32], 
										szSubDeviceLocation[32];
			
			double						lfSetDelay,
										lfSetTimeout;  
			
			int							iValueMode;
			
			int							StateOnValue, 
							  			StateOffValue;
			//------------ Signature --------------------------------//
			unsigned __int64			ulSignatureID;
			//-------------------------------------------------------//			
			
			
			//---------- end of configuration sector ----------//
		};
	};
	
} tsSingleDeviceItem;


tsSingleDeviceItem			gvtListOfCommands[MAX_LIST_NUMBER]	=	{0};
	
int							giCurrentItemIndex					=	0,
							giNumberOfItems						=	0;

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
	int			iIndex								=	giCurrentItemIndex;
	
	char		szStringValue[LOW_STRING]			=	{0};
	
	SetCtrlVal( panelHandle , PANEL_COMMAND_NAME_STRING , gvtListOfCommands[iIndex].szCommandName );
	
	if ( gvtListOfCommands[iIndex].iValueMode == VALUE_MODE_ON_OFF ) // On Off 
	{
		SetCtrlAttribute( panelHandle , PANEL_STATE_ON_VAL , ATTR_DIMMED , 0 );
		SetCtrlAttribute( panelHandle , PANEL_STATE_OFF_VAL , ATTR_DIMMED , 0 );
		
		sprintf( szStringValue , "%d" , gvtListOfCommands[iIndex].StateOnValue ); 
		SetCtrlVal( panelHandle , PANEL_STATE_ON_VAL , szStringValue );
		
		sprintf( szStringValue , "%d" , gvtListOfCommands[iIndex].StateOffValue );  
		SetCtrlVal( panelHandle , PANEL_STATE_OFF_VAL , szStringValue ); 
	}
	else
	{
		SetCtrlAttribute( panelHandle , PANEL_STATE_ON_VAL , ATTR_DIMMED , 1 );
		SetCtrlAttribute( panelHandle , PANEL_STATE_OFF_VAL , ATTR_DIMMED , 1 );
	}
	
	if ( strlen( gvtListOfCommands[iIndex].szSubDeviceName ))
		SetCtrlVal( panelHandle , PANEL_SWITCH_TYPE , gvtListOfCommands[iIndex].szSubDeviceName );   
	else
		SetCtrlIndex( panelHandle , PANEL_SWITCH_TYPE , 0 );   
	
	if ( strlen( gvtListOfCommands[iIndex].szSubDeviceLocation ))
		SetCtrlVal( panelHandle , PANEL_SLOT , gvtListOfCommands[iIndex].szSubDeviceLocation );   
	else
		SetCtrlIndex( panelHandle , PANEL_SLOT , 0 );  
	
	sprintf( szStringValue , "%0.1lf" , gvtListOfCommands[iIndex].lfSetDelay );  
	SetCtrlVal( panelHandle , PANEL_DELAY , szStringValue );  
	
	sprintf( szStringValue , "%0.1lf" , gvtListOfCommands[iIndex].lfSetTimeout );  
	SetCtrlVal( panelHandle , PANEL_TIMEOUT , szStringValue );  
		
	return 0;
}


int		OpenConfigurationFile( char *pszFilePath )
{
	int			iIndex								=	0;

	int			hFileHandle							=	0;
	
	int			iCount								=	0;

	int			bForceChanges						=	0;
	
	if ( FileExists( pszFilePath , NULL ))
		if ( strstr( pszFilePath , ".mccnf" ))
		{
			strcpy( szCurrentFilePath , pszFilePath );
			
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
	
	if ((panelHandle = LoadPanel (0, "MiniCircuits_RFBox_Control_Editor.uir", PANEL)) < 0)
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
	
		if (( bAskUser == 0 ) || ( FileSelectPopup (szPath, szFileName , "*.mccnf", "Save", VAL_SAVE_BUTTON, 0, 1, 1, 1, szFilePath )))
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
	
	if ( FileSelectPopup ("", "*.mccnf", "*.mccnf", "Load", VAL_LOAD_BUTTON, 0, 1, 1, 1, szFilePath ))
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
	int			iIndex								=	giCurrentItemIndex;
	
	char		szStringValue[LOW_STRING]			=	{0};
	
	long		lValue								=	0;

	int			iStyle								=	0,
			  	iDataType							=	0;
	
	double		lfValue								=	0;
	
	switch (event)
	{
		case EVENT_COMMIT:

			GetCtrlAttribute ( panel , control , ATTR_CTRL_STYLE, &iStyle );
			
			bSaveChanges = 1;
			
			switch(iStyle)
			{
				case CTRL_STRING_LS:
				case CTRL_STRING:	
					
						GetCtrlVal( panel , control , szStringValue );  
			
						if ( strchr( szStringValue , '.' ) || strchr( szStringValue , 'E' ))  
						{
							sscanf( szStringValue , "%lf" , &lfValue ); 
							
							lValue = lfValue;
						}
						else
						{
							if ( strchr( szStringValue , 'x' ) || strchr( szStringValue , 'X' ))
								sscanf( szStringValue , "%x" , &lValue );
							else
								sscanf( szStringValue , "%d" , &lValue );
							
							lfValue = lValue;
						}
						
						break;
						
				default:
					
					GetCtrlAttribute ( panel , control , ATTR_DATA_TYPE, &iDataType );  
					
					switch(iDataType)
					{
						case VAL_STRING:
							GetCtrlVal( panel , control , szStringValue );   
							break; 
						case VAL_CHAR:                  
							GetCtrlVal( panel , control , (char*)&lValue ); 
							break; 
						case VAL_INTEGER:  
							GetCtrlVal( panel , control , &lValue ); 
							break; 
						case VAL_SHORT_INTEGER:  
							GetCtrlVal( panel , control , (short*)&lValue ); 
							break; 
						case VAL_UNSIGNED_SHORT_INTEGER:
							GetCtrlVal( panel , control , (unsigned short*)&lValue ); 
							break; 
						case VAL_UNSIGNED_INTEGER:      
							GetCtrlVal( panel , control , (unsigned int*)&lValue ); 
							break; 
						case VAL_UNSIGNED_CHAR:         
							GetCtrlVal( panel , control , (unsigned char*)&lValue ); 
							break; 
						default:
							break;
					}
					
					break;
						
			}

			switch (control)
			{  
				case PANEL_DELAY: 
					gvtListOfCommands[iIndex].lfSetDelay = lfValue;    
					break;
				case PANEL_TIMEOUT:         
					gvtListOfCommands[iIndex].lfSetTimeout = lfValue;    
					break;
				case PANEL_STATE_OFF_VAL:   
					gvtListOfCommands[iIndex].StateOffValue = lValue;    
					break;
				case PANEL_STATE_ON_VAL:    
					gvtListOfCommands[iIndex].StateOnValue = lValue;    
					break;
				case PANEL_SWITCH_TYPE: 
					strcpy( gvtListOfCommands[iIndex].szSubDeviceName , szStringValue );
					break;
				case PANEL_VALUE_MODE:      
					gvtListOfCommands[iIndex].iValueMode = lValue;  
					break;
				case PANEL_SLOT:            
					strcpy( gvtListOfCommands[iIndex].szSubDeviceLocation , szStringValue );
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
