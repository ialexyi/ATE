//==============================================================================
// Include files 


//==============================================================================
// Constants

//==============================================================================
// Types

typedef struct
{
	char			szCommandName[128];
	
	signed   short	usCommandId; 
	signed   short	usPortNumber;
	
	unsigned char	ucSendDataType;
	unsigned short	usSendDataLength; 
	
	unsigned char	ucReceiveDataType;
	unsigned short	usReceiveDataLength; 
	
	
} tsSTD_RIO_CommandItem;

//==============================================================================
// External variables

//==============================================================================
// Global functions

int	GetCommandItemListSize(void);

