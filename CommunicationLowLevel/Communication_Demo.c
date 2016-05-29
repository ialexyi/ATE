//==============================================================================
// Include files

#include <ansi_c.h>
#include "Communication_Low_Level.h"
#include "toolbox.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables
void receive ( char *buffer , unsigned int count , void *data );
//==============================================================================
// Global functions

int main (int argc, char *argv[])
{
	errEV							ret												=	{0}; 
	
	int								hHandle											=	0; 
		
	tsConfigCommunication			tConfig											=	{0};
	
	unsigned char 					*pBuffer										=	NULL;
	
	int								iCount											=	0;
	
	unsigned char					szConfigBuffer[CONFIG_COM_STRUCT_LEN]			=	{0};
		
	InitCVIRTE (0, argv, 0 ); 

	strcpy( (char*)tConfig.szAddress , "Silicon Labs CP210x" );
	tConfig.iType = 1;
		
	memcpy( szConfigBuffer , &tConfig , sizeof(tsConfigCommunication));
	
	ret = Communication_Init( szConfigBuffer , &hHandle );
	
	Communication_EnableReceive( hHandle, 1 ); 
	
	Communication_InstallCallBack( hHandle , receive , NULL );
	
	Communication_Send( hHandle , (unsigned char*)"Dima Super Krut" , 15 , 1 , 1 );

	Communication_Receive( hHandle , 100.0, &pBuffer , &iCount );


	Communication_Close(hHandle);
	
	DelayWithEventProcessing(50.0);
	
	return 0;
	
}


void receive ( char *buffer , unsigned int count , void *data )
{
	

	return;
}
