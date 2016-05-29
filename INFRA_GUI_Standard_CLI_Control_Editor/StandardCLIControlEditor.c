#include <cvirte.h>		
#include <userint.h>
#include "StandardCLIControlEditor.h"
#include "GlobalDefines.h"
#include "toolbox.h"
#include <formatio.h>
#include <ansi_c.h>

#define				DEVICE_ID					STANDARD_CLI_DRIVER_DEVICE_ID

typedef union
{
	char				max_size[64];   
	
	struct
	{  
		int				iParametersType;								// 1 - int , 2 - long , 3 - double , 4 - string       
		
		char			szParamName[12];
		
		char			szDefaultValue[16],
						szFormatValue[16]; 
	};
	
} tsSEND_PARAM_ITEM;

typedef union
{
	char				max_size[64];   
	
	struct
	{  
		int				iParametersType;								// 1 - int , 2 - long , 3 - double , 4 - string  

		char			szParamName[12];
		
		char			szStartPatern[16],
						szStopPatern[16]; 
	};
	
} tsRECEIVE_PARAM_ITEM;

typedef union
{
	char				max_size[STD_DOUBLE_BUFFER_SIZE];   
	
	struct
	{   
		//---------- Send Side -----------------//
		char							szSendCommandFormated[64];
		int								iSendNumberOfParameters; 
		tsSEND_PARAM_ITEM				tSendParameters[8];
	
		//---------- Receive Side -----------------//
		char							szRespondsFormated[64];
		int								iReceiveNumberOfParameters;
		tsRECEIVE_PARAM_ITEM			tReceiveParameters[8];
		
		char							szCommandName[64];
		
		//------------ Signature --------------------------------//
		unsigned __int64				ulSignatureID;
		//-------------------------------------------------------//			
	};
	
} tsSTD_CLI_CommandItem;

tsSTD_CLI_CommandItem			*gptListOfCommands					=	NULL;
	
int								giCurrentItemIndex					=	0,
								giNumberOfItems						=	0,
								ghPanelHandle						=	0;
								
int		UpdateList( int panelHandle )
{
	int			iIndex		=	0;
	
	DeleteListItem ( panelHandle, PANEL_COMMAND_NAME_LIST , 0 , -1 );
	
	for ( iIndex = 0 ; iIndex < giNumberOfItems; iIndex++ )
	{
		if ( strlen(gptListOfCommands[iIndex].szCommandName) == 0 )
			break;
		
		InsertListItem( panelHandle , PANEL_COMMAND_NAME_LIST ,  -1 , gptListOfCommands[iIndex].szCommandName , iIndex );   
	}	 
	
	giNumberOfItems = iIndex;
	
	InsertListItem( panelHandle , PANEL_COMMAND_NAME_LIST ,  -1 , "..." , iIndex );   
	
	return 0;
}


int		UpdateCurrentItem( int panelHandle )
{
	int			iIndex								=	giCurrentItemIndex,
				iSendParamIndex						=	0,
				iReceiveParamIndex					=	0;
	
	char		szStringValue[LOW_STRING]			=	{0};
	
	int			hTabPanelHandle						=	0;
	
	if ( iIndex >= giNumberOfItems )
		return 0;
	
	SetCtrlVal( panelHandle , PANEL_COMMAND_NAME_STRING , gptListOfCommands[iIndex].szCommandName );
	
	SetCtrlVal( panelHandle , PANEL_SEND_COMMAND_STR , gptListOfCommands[iIndex].szSendCommandFormated );
	SetCtrlVal( panelHandle , PANEL_RESPONDS , gptListOfCommands[iIndex].szRespondsFormated );
	
	SetCtrlVal( panelHandle , PANEL_SEND_PARAM , gptListOfCommands[iIndex].iSendNumberOfParameters );

	for( iSendParamIndex = 0; iSendParamIndex < gptListOfCommands[iIndex].iSendNumberOfParameters; iSendParamIndex++ )
	{
		SetTabPageAttribute ( panelHandle , PANEL_SEND_TAB , iSendParamIndex , ATTR_VISIBLE , 1 );

		GetPanelHandleFromTabPage ( panelHandle , PANEL_SEND_TAB , iSendParamIndex , &hTabPanelHandle );

		SetCtrlVal( hTabPanelHandle , SEND_PAR_1_TYPE , gptListOfCommands[iIndex].tSendParameters[iSendParamIndex].iParametersType );			// 1 - int , 2 - long , 3 - double , 4 - string       
		SetCtrlVal( hTabPanelHandle , SEND_PAR_1_NAME , gptListOfCommands[iIndex].tSendParameters[iSendParamIndex].szParamName );		
		SetCtrlVal( hTabPanelHandle , SEND_PAR_1_DEFAULT , gptListOfCommands[iIndex].tSendParameters[iSendParamIndex].szDefaultValue );	
		SetCtrlVal( hTabPanelHandle , SEND_PAR_1_FORMAT , gptListOfCommands[iIndex].tSendParameters[iSendParamIndex].szFormatValue );	
	}
	
	for( ; iSendParamIndex < 8; iSendParamIndex++ )     
		SetTabPageAttribute ( panelHandle , PANEL_SEND_TAB , iSendParamIndex , ATTR_VISIBLE , 0 );
	
	//---------- Receive Side -----------------//
	
	for( iReceiveParamIndex = 0; iReceiveParamIndex < gptListOfCommands[iIndex].iReceiveNumberOfParameters; iReceiveParamIndex++ )
	{
		SetTabPageAttribute ( panelHandle , PANEL_RECEIVE_TAB , iReceiveParamIndex , ATTR_VISIBLE , 1 );

		GetPanelHandleFromTabPage ( panelHandle , PANEL_RECEIVE_TAB , iReceiveParamIndex , &hTabPanelHandle );

		SetCtrlVal( hTabPanelHandle , REC_PAR_1_TYPE  , gptListOfCommands[iIndex].tReceiveParameters[iReceiveParamIndex].iParametersType );			// 1 - int , 2 - long , 3 - double , 4 - string       
		SetCtrlVal( hTabPanelHandle , REC_PAR_1_NAME  , gptListOfCommands[iIndex].tReceiveParameters[iReceiveParamIndex].szParamName );	
		SetCtrlVal( hTabPanelHandle , REC_PAR_1_STOP  , gptListOfCommands[iIndex].tReceiveParameters[iReceiveParamIndex].szStopPatern );		
		SetCtrlVal( hTabPanelHandle , REC_PAR_1_START , gptListOfCommands[iIndex].tReceiveParameters[iReceiveParamIndex].szStartPatern );	
	}
	
	for( ; iReceiveParamIndex < 8; iReceiveParamIndex++ )     
		SetTabPageAttribute ( panelHandle , PANEL_RECEIVE_TAB , iReceiveParamIndex , ATTR_VISIBLE , 0 );
		
	return 0;
}


int		OpenConfigurationFile( char *pszFilePath )
{
	int			iIndex								=	0;

	int			hFileHandle							=	0;
	
	int			iCount								=	0;
	
	int			iFileSize							=	0,
				iNumberOfItems						=	0;
	
	if ( FileExists( pszFilePath , &iFileSize ))
		if ( strstr( pszFilePath , ".clicnf" ))
		{
			hFileHandle = OpenFile (pszFilePath, VAL_READ_ONLY, VAL_TRUNCATE, VAL_BINARY);

			iNumberOfItems = ( iFileSize / sizeof(tsSTD_CLI_CommandItem) );
			
			gptListOfCommands = calloc( iNumberOfItems+1 , sizeof(tsSTD_CLI_CommandItem) );

			if ( gptListOfCommands == 0 )
				return 0;
			
			do
			{
				iCount = ReadFile ( hFileHandle , gptListOfCommands[iIndex].max_size , sizeof(tsSTD_CLI_CommandItem) );
			
				if ( iCount != sizeof(tsSTD_CLI_CommandItem) )
					break;
				
				iIndex++;
				
			} while(1);
		
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
	
	if ((panelHandle = LoadPanel (0, "StandardCLIControlEditor.uir", PANEL)) < 0)
		return -1;
	
	ghPanelHandle = panelHandle;
	
	DisplayPanel (panelHandle);
	
	if ( argc > 1 )
		OpenConfigurationFile( argv[1] ); 
	
	UpdateList( panelHandle );
	UpdateCurrentItem( panelHandle ); 
	
	RunUserInterface ();
	
	DiscardPanel (panelHandle);
	
	if ( gptListOfCommands )
		free(gptListOfCommands);
	
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
	if ( gptListOfCommands )
		free( gptListOfCommands );
		
	giCurrentItemIndex = 0;
	
	UpdateList( panel );
	UpdateCurrentItem( panel );
}

void CVICALLBACK clbMenuOpenFile (int menuBar, int menuItem, void *callbackData, int panel)
{
	char		szFilePath[STD_STRING]				=	{0};
	
	if ( FileSelectPopup ("", "*.clicnf", "*.clicnf", "Load", VAL_LOAD_BUTTON, 0, 1, 1, 1, szFilePath ))
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
	
	if ( FileSelectPopup ("", "*.clicnf", "*.clicnf", "Save", VAL_SAVE_BUTTON, 0, 1, 1, 1, szFilePath ))
	{
		hFileHandle = OpenFile (szFilePath, VAL_WRITE_ONLY, VAL_TRUNCATE, VAL_BINARY);
		
		for ( iIndex = 0 ; iIndex < giNumberOfItems; iIndex++ )
		{
		
			WriteFile ( hFileHandle , gptListOfCommands[iIndex].max_size , sizeof(tsSTD_CLI_CommandItem) );
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
	
	int			iSendParamIndex						=	0,
				iReceiveParamIndex					=	0;
	
	int			hTabPanelHandle						=	0;
	
	switch (event)
	{
		case EVENT_COMMIT:
					
			if ( iIndex >= giNumberOfItems )
				return 0;
				
			for( iSendParamIndex = 0; iSendParamIndex < gptListOfCommands[iIndex].iSendNumberOfParameters; iSendParamIndex++ )
			{
				GetPanelHandleFromTabPage ( ghPanelHandle , PANEL_SEND_TAB , iSendParamIndex , &hTabPanelHandle );

				if ( panel != hTabPanelHandle )
					continue;
				
				if ( control == SEND_PAR_1_TYPE )
					GetCtrlVal( hTabPanelHandle , control , &(gptListOfCommands[iIndex].tSendParameters[iSendParamIndex].iParametersType) );			// 1 - int , 2 - long , 3 - double , 4 - string       
					
				if ( control == SEND_PAR_1_NAME )
					GetCtrlVal( hTabPanelHandle , control , gptListOfCommands[iIndex].tSendParameters[iSendParamIndex].szParamName );		
				
				if ( control == SEND_PAR_1_DEFAULT )
					GetCtrlVal( hTabPanelHandle , control , gptListOfCommands[iIndex].tSendParameters[iSendParamIndex].szDefaultValue );	
				
				if ( control == SEND_PAR_1_FORMAT )
					GetCtrlVal( hTabPanelHandle , control , gptListOfCommands[iIndex].tSendParameters[iSendParamIndex].szFormatValue );	
			}
	
			for( iReceiveParamIndex = 0; iReceiveParamIndex < gptListOfCommands[iIndex].iReceiveNumberOfParameters; iReceiveParamIndex++ )
			{
				GetPanelHandleFromTabPage ( ghPanelHandle , PANEL_RECEIVE_TAB , iReceiveParamIndex , &hTabPanelHandle );

				if ( panel != hTabPanelHandle )
					continue;
				
				if ( control == REC_PAR_1_TYPE )
					GetCtrlVal( hTabPanelHandle , control  , &(gptListOfCommands[iIndex].tReceiveParameters[iReceiveParamIndex].iParametersType) );			// 1 - int , 2 - long , 3 - double , 4 - string       
				
				if ( control == REC_PAR_1_NAME )
					GetCtrlVal( hTabPanelHandle , control  , gptListOfCommands[iIndex].tReceiveParameters[iReceiveParamIndex].szParamName );	
				
				if ( control == REC_PAR_1_STOP )
					GetCtrlVal( hTabPanelHandle , control  , gptListOfCommands[iIndex].tReceiveParameters[iReceiveParamIndex].szStopPatern );		
				
				if ( control == REC_PAR_1_START )
					GetCtrlVal( hTabPanelHandle , control , gptListOfCommands[iIndex].tReceiveParameters[iReceiveParamIndex].szStartPatern );	
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
	int					iIndex								=	0;
						
	char				szStringValue[LOW_STRING]			=	{0};

	int					iSendParamIndex						=	0,
						iReceiveParamIndex					=	0;
	int					iSendNumberOfParameters				=	0,
						iReceiveNumberOfParameters			=	0;
	
	switch (event)
	{
		case EVENT_COMMIT:
				
			GetCtrlVal( panel , control , szStringValue );  
			
			GetCtrlIndex ( panel , PANEL_COMMAND_NAME_LIST , &iIndex ); 
			
			if ( iIndex == giNumberOfItems )
			{
				gptListOfCommands = realloc( gptListOfCommands , ((giNumberOfItems+1)*sizeof(tsSTD_CLI_CommandItem)));
			
				if ( gptListOfCommands == NULL )
					return 0;
			
				memset( &(gptListOfCommands[iIndex]) , 0 , sizeof(tsSTD_CLI_CommandItem));
			}
			
			strcpy( gptListOfCommands[iIndex].szCommandName , szStringValue );
			
			ReplaceListItem ( panel , PANEL_COMMAND_NAME_LIST , iIndex , szStringValue , iIndex );
			
			if ( iIndex == giNumberOfItems )
			{
				InsertListItem( panel , PANEL_COMMAND_NAME_LIST ,  -1 , "..." , iIndex ); 
				giNumberOfItems++;
			
				SetCtrlVal( panel , PANEL_SEND_PARAM , iSendNumberOfParameters );    
			
				for( iSendParamIndex = 0; iSendParamIndex < 8; iSendParamIndex++ )     
					SetTabPageAttribute ( panel , PANEL_SEND_TAB , iSendParamIndex , ATTR_VISIBLE , 0 );
			
				SetCtrlVal( panel , PANEL_RECEIVE_PARAM , iSendNumberOfParameters );    
			
				for( iReceiveParamIndex = 0; iReceiveParamIndex < 8; iReceiveParamIndex++ )     
					SetTabPageAttribute ( panel , PANEL_RECEIVE_TAB , iReceiveParamIndex , ATTR_VISIBLE , 0 );
			}
			
			break;
	}
	return 0;
}


int CVICALLBACK clbSendParamsChanged (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int					iSendParamIndex						=	0;
							
	int					iSendNumberOfParameters				=	0;
	
	int					iIndex								=	giCurrentItemIndex;
	
	if ( event != EVENT_COMMIT )
		return 0;
	
	GetCtrlVal( panel , control , &iSendNumberOfParameters );
	
	if ( iIndex >= giNumberOfItems )
		return 0;
	
	gptListOfCommands[iIndex].iSendNumberOfParameters = iSendNumberOfParameters;
	
	for( iSendParamIndex = 0; iSendParamIndex < iSendNumberOfParameters; iSendParamIndex++ )
		SetTabPageAttribute ( panel , PANEL_SEND_TAB , iSendParamIndex , ATTR_VISIBLE , 1 );
	
	for( ; iSendParamIndex < 8; iSendParamIndex++ )     
		SetTabPageAttribute ( panel , PANEL_SEND_TAB , iSendParamIndex , ATTR_VISIBLE , 0 );
	
	return 0;
}

int CVICALLBACK clbReceiveParamsChanged (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int					iReceiveParamIndex					=	0;
							
	int					iReceiveNumberOfParameters			=	0;

	int					iIndex								=	giCurrentItemIndex;
	
	if ( event != EVENT_COMMIT )
		return 0;
			
	GetCtrlVal( panel , control , &iReceiveNumberOfParameters );

	if ( iIndex >= giNumberOfItems )
		return 0;
	
	gptListOfCommands[iIndex].iReceiveNumberOfParameters = iReceiveNumberOfParameters;
	
	for( iReceiveParamIndex = 0; iReceiveParamIndex < iReceiveNumberOfParameters; iReceiveParamIndex++ )
		SetTabPageAttribute ( panel , PANEL_RECEIVE_TAB , iReceiveParamIndex , ATTR_VISIBLE , 1 );
	
	for( ; iReceiveParamIndex < 8; iReceiveParamIndex++ )     
		SetTabPageAttribute ( panel , PANEL_RECEIVE_TAB , iReceiveParamIndex , ATTR_VISIBLE , 0 );
		
	
	return 0;
}

int CVICALLBACK clbCommandChanged (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
 	int			iIndex								=	giCurrentItemIndex;
	
	if ( event != EVENT_COMMIT )
		return 0;
	
	if ( iIndex >= giNumberOfItems )
		return 0;
	
	GetCtrlVal( panel , control , gptListOfCommands[iIndex].szSendCommandFormated );

	return 0;
}

int CVICALLBACK clbRespondsChanged (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
 	int			iIndex								=	giCurrentItemIndex;
	
	if ( event != EVENT_COMMIT )
		return 0;
	
	if ( iIndex >= giNumberOfItems )
		return 0;
	
	GetCtrlVal( panel , control , gptListOfCommands[iIndex].szRespondsFormated );

	return 0;
}
   
