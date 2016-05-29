//==============================================================================
// Include files
#include <windows.h>
#include <userint.h>
#include <utility.h>
#include <visa.h>
#include <ansi_c.h>
#include "LambdaZup_Support.h"
#include "LambdaZup_Types.h"
#include "GlobalDefines.h"

//==============================================================================
// Constants

#define		MODULE_NAME		"LambdaZup Driver"

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

tsLambdaTypes		tLambdaZupDefines[] = 
{
	{/***************************************************************************************/
		
		/* szPowerSupplyModel */			{"6V-33A"},
		
									/* lfMaxValue */	/* lfMinValue */	/* lfAccuracy */
		
		/* currentDefine */		{		33.0,				0.0,				5.2			},
		
		/* voltageDefine */		{		6.0,				0.0,				5.3			},
		
		/* overVoltageDefine */	{		7.5,				0.2,				4.2			},
		
		/* underVoltageDefine */{		5.98,				0.0,				4.2			}
	},
	{/***************************************************************************************/
		
		/* szPowerSupplyModel */			{"6V-66A"},
		
									/* lfMaxValue */	/* lfMinValue */	/* lfAccuracy */
		
		/* currentDefine */		{		66.0,				0.0,				5.2			},
		
		/* voltageDefine */		{		6.0,				0.0,				5.3			},
		
		/* overVoltageDefine */	{		7.5,				0.2,				4.2			},
		
		/* underVoltageDefine */{		5.98,				0.0,				4.2			}
	},
	{/***************************************************************************************/
		
		/* szPowerSupplyModel */			{"6V-132A"},
		
									/* lfMaxValue */	/* lfMinValue */	/* lfAccuracy */
		
		/* currentDefine */		{		132.0,				0.0,				6.2			},
		
		/* voltageDefine */		{		6.0,				0.0,				5.3			},
		
		/* overVoltageDefine */	{		7.5,				0.2,				4.2			},
		
		/* underVoltageDefine */{		5.98,				0.0,				4.2			}
	},
	{/***************************************************************************************/
		
		/* szPowerSupplyModel */			{"10V-20A"},
		
									/* lfMaxValue */	/* lfMinValue */	/* lfAccuracy */
		
		/* currentDefine */		{		20.0,				0.0,				6.3			},
		
		/* voltageDefine */		{		10.0,				0.0,				6.3			},
		
		/* overVoltageDefine */	{		13.0,				0.5,				4.1			},
		
		/* underVoltageDefine */{		9.97,				0.0,				4.2			}
	},
	{/***************************************************************************************/
		
		/* szPowerSupplyModel */			{"10V-40A"},
		
									/* lfMaxValue */	/* lfMinValue */	/* lfAccuracy */
		
		/* currentDefine */		{		40.0,				0.0,				5.2			},
		
		/* voltageDefine */		{		10.0,				0.0,				6.3			},
		
		/* overVoltageDefine */	{		13.0,				0.5,				4.1			},
		
		/* underVoltageDefine */{		9.97,				0.0,				4.2			}
	},
	{/***************************************************************************************/
		
		/* szPowerSupplyModel */			{"10V-80A"},
		
									/* lfMaxValue */	/* lfMinValue */	/* lfAccuracy */
		
		/* currentDefine */		{		80.0,				0.0,				5.2			},
		
		/* voltageDefine */		{		10.0,				0.0,				6.3			},
		
		/* overVoltageDefine */	{		13.0,				0.5,				4.1			},
		
		/* underVoltageDefine */{		9.97,				0.0,				4.2			}
	},
	{/***************************************************************************************/
		
		/* szPowerSupplyModel */			{"20V-10A"},
		
									/* lfMaxValue */	/* lfMinValue */	/* lfAccuracy */
		
		/* currentDefine */		{		10.0,				0.0,				6.3			},
		
		/* voltageDefine */		{		20.0,				0.0,				6.3			},
		
		/* overVoltageDefine */	{		24.0,				1.0,				4.1			},
		
		/* underVoltageDefine */{		19.9,				0.0,				4.1			}
	},
	{/***************************************************************************************/
		
		/* szPowerSupplyModel */			{"20V-20A"},
		
									/* lfMaxValue */	/* lfMinValue */	/* lfAccuracy */
		
		/* currentDefine */		{		20.0,				0.0,				6.3			},
		
		/* voltageDefine */		{		20.0,				0.0,				6.3			},
		
		/* overVoltageDefine */	{		24.0,				1.0,				4.1			},
		
		/* underVoltageDefine */{		19.9,				0.0,				4.1			}
	},
	{/***************************************************************************************/
		
		/* szPowerSupplyModel */			{"20V-40A"},
		
									/* lfMaxValue */	/* lfMinValue */	/* lfAccuracy */
		
		/* currentDefine */		{		40.0,				0.0,				5.2			},
		
		/* voltageDefine */		{		20.0,				0.0,				6.3			},
		
		/* overVoltageDefine */	{		24.0,				1.0,				4.1			},
		
		/* underVoltageDefine */{		19.9,				0.0,				4.1			}
	},
	{/***************************************************************************************/
		
		/* szPowerSupplyModel */			{"36V-6A"},
		
									/* lfMaxValue */	/* lfMinValue */	/* lfAccuracy */
		
		/* currentDefine */		{		6.0,				0.0,				5.3			},
		
		/* voltageDefine */		{		36.0,				0.0,				5.2			},
		
		/* overVoltageDefine */	{		40.0,				1.8,				4.1			},
		
		/* underVoltageDefine */{		35.9,				0.0,				4.1			}
	},
	{/***************************************************************************************/
		
		/* szPowerSupplyModel */			{"36V-12A"},
		
									/* lfMaxValue */	/* lfMinValue */	/* lfAccuracy */
		
		/* currentDefine */		{		12.0,				0.0,				6.3			},
		
		/* voltageDefine */		{		36.0,				0.0,				5.2			},
		
		/* overVoltageDefine */	{		40.0,				1.8,				4.1			},
		
		/* underVoltageDefine */{		35.9,				0.0,				4.1			}
	},
	{/***************************************************************************************/
		
		/* szPowerSupplyModel */			{"36V-24A"},
		
									/* lfMaxValue */	/* lfMinValue */	/* lfAccuracy */
		
		/* currentDefine */		{		24.0,				0.0,				6.3			},
		
		/* voltageDefine */		{		36.0,				0.0,				5.2			},
		
		/* overVoltageDefine */	{		40.0,				1.8,				4.1			},
		
		/* underVoltageDefine */{		35.9,				0.0,				4.1			}
	},
	{/***************************************************************************************/
		
		/* szPowerSupplyModel */			{"60V-3.5A"},
		
									/* lfMaxValue */	/* lfMinValue */	/* lfAccuracy */
		
		/* currentDefine */		{		3.5,				0.0,				5.3			},
		
		/* voltageDefine */		{		60.0,				0.0,				5.2			},
		
		/* overVoltageDefine */	{		66.0,				3.0,				4.1			},
		
		/* underVoltageDefine */{		59.8,				0.0,				4.1			}
	},
	{/***************************************************************************************/
		
		/* szPowerSupplyModel */			{"60V-7A"},
		
									/* lfMaxValue */	/* lfMinValue */	/* lfAccuracy */
		
		/* currentDefine */		{		7.0,				0.0,				5.3			},
		
		/* voltageDefine */		{		60.0,				0.0,				5.2			},
		
		/* overVoltageDefine */	{		66.0,				3.0,				4.1			},
		
		/* underVoltageDefine */{		59.8,				0.0,				4.1			}
	},
	{/***************************************************************************************/
		
		/* szPowerSupplyModel */			{"60V-14A"},
		
									/* lfMaxValue */	/* lfMinValue */	/* lfAccuracy */
		
		/* currentDefine */		{		14.0,				0.0,				6.3			},
		
		/* voltageDefine */		{		60.0,				0.0,				5.2			},
		
		/* overVoltageDefine */	{		66.0,				3.0,				4.1			},
		
		/* underVoltageDefine */{		59.8,				0.0,				4.1			}
	},
	{/***************************************************************************************/
		
		/* szPowerSupplyModel */			{"80V-2.5A"},
		
									/* lfMaxValue */	/* lfMinValue */	/* lfAccuracy */
		
		/* currentDefine */		{		2.5,				0.0,				6.4			},
		
		/* voltageDefine */		{		80.0,				0.0,				5.2			},
		
		/* overVoltageDefine */	{		88.0,				4.0,				4.1			},
		
		/* underVoltageDefine */{		79.8,				0.0,				4.1			}
	},
	{/***************************************************************************************/
		
		/* szPowerSupplyModel */			{"80V-5A"},
		
									/* lfMaxValue */	/* lfMinValue */	/* lfAccuracy */
		
		/* currentDefine */		{		5.0,				0.0,				5.3			},
		
		/* voltageDefine */		{		80.0,				0.0,				5.2			},
		
		/* overVoltageDefine */	{		88.0,				4.0,				4.1			},
		
		/* underVoltageDefine */{		79.8,				0.0,				4.1			}
	},
	{/***************************************************************************************/
		
		/* szPowerSupplyModel */			{"120V-1.8A"},
		
									/* lfMaxValue */	/* lfMinValue */	/* lfAccuracy */
		
		/* currentDefine */		{		1.8,				0.0,				6.4			},
		
		/* voltageDefine */		{		120.0,				0.0,				6.2			},
		
		/* overVoltageDefine */	{		132.0,				6.0,				5.1			},
		
		/* underVoltageDefine */{		119.8,				0.0,				5.1			}
	},
	{/***************************************************************************************/
		
		/* szPowerSupplyModel */			{"120V-3.6A"},
		
									/* lfMaxValue */	/* lfMinValue */	/* lfAccuracy */
		
		/* currentDefine */		{		3.6,				0.0,				5.3			},
		
		/* voltageDefine */		{		120.0,				0.0,				6.2			},
		
		/* overVoltageDefine */	{		132.0,				6.0,				5.1			},
		
		/* underVoltageDefine */{		119.8,				0.0,				5.1			}
	}
};

//==============================================================================
// Global functions

/**********************************************************************************/

int LambdaZupTypesQuantity( void )
{
	return (sizeof(tLambdaZupDefines) / sizeof( tsLambdaTypes ));			
}

/**********************************************************************************/ 

errEV GetZupChannelsQuantity ( int handle , int *pChannelsQuantity )
{
	errEV				ret								=	{0};
	
	tsLambdaHandle		*ptLambdaVariable				=	NULL;
	
	IF (( pChannelsQuantity == NULL ) , "Error passed NULL pointer." , MODULE_NAME ); 
	
	CHK_CMT( CmtGetTSVPtr ( handle , &ptLambdaVariable ));
	
	*pChannelsQuantity = ptLambdaVariable->iChanelsQuantity;
	
Error:
	
	if (handle)
		CmtReleaseTSVPtr (handle);
	
	return ret;
}

/**********************************************************************************/

errEV IntToStringAddress ( int iPSAddress , char *pPSAddressBuf , ViUInt32 *ipBufLength )
{
	errEV				ret								=	{0};
	
	IF ((( iPSAddress < 1 ) || ( iPSAddress > 31 )) , "Illegal device address.", MODULE_NAME);
	
	*ipBufLength = sprintf ( pPSAddressBuf , ":ADR%02d;" , iPSAddress );
	
Error:	
	
	return ret;
}

/**********************************************************************************/

errEV DoubleToStringVoltage ( tsLambdaChannel type , double lfVolts , char *pVoltsBuf , ViUInt32 *ipBufLength )
{
	errEV				ret								=	{0};
	
	char		szTemp[WR_BUF]		=	{0},
				szSend[WR_BUF]		=	{0};
	
	IF ((( lfVolts < type.voltageDefine.lfMinValue ) || ( lfVolts > type.voltageDefine.lfMaxValue )) , "Illegal parametr.", MODULE_NAME);
	
	sprintf ( szTemp , ":VOL%c0%3.1lflf;" , '%' , type.voltageDefine.lfAccuracy );  

	sprintf ( szSend , szTemp , lfVolts );
	
	*ipBufLength = sprintf ( pVoltsBuf , "%s" , szSend );

Error:	
	
	return ret;
}

/**********************************************************************************/

errEV DoubleToStringCurrent ( tsLambdaChannel type , double lfAmpers , char *pAmpersBuf , ViUInt32 *ipBufLength )
{
	errEV				ret								=	{0};
	
	char		szTemp[WR_BUF]		=	{0},
				szSend[WR_BUF]		=	{0};
	
	IF ((( lfAmpers < type.currentDefine.lfMinValue ) || ( lfAmpers > type.currentDefine.lfMaxValue )) , "Illegal parametr.", MODULE_NAME);
	
	sprintf ( szTemp , ":CUR%c0%3.1lflf;" , '%' , type.currentDefine.lfAccuracy );

	sprintf ( szSend , szTemp , lfAmpers );
	
	*ipBufLength = sprintf ( pAmpersBuf , "%s" , szSend );

Error:	
	
	return ret;
}

/**********************************************************************************/

errEV DoubleToStringOverProtection ( tsLambdaChannel type , double lfVolts , char *pVoltsBuf , ViUInt32 *ipBufLength )
{
	errEV				ret								=	{0};
	
	char		szTemp[WR_BUF]		=	{0},
				szSend[WR_BUF]		=	{0};
	
	IF ((( lfVolts < type.overVoltageDefine.lfMinValue ) || ( lfVolts > type.overVoltageDefine.lfMaxValue )) , "Illegal parametr.", MODULE_NAME);
		
	sprintf ( szTemp , ":OVP%c0%3.1lflf;" , '%' , type.overVoltageDefine.lfAccuracy );

	sprintf ( szSend , szTemp , lfVolts );

	*ipBufLength = sprintf ( pVoltsBuf , "%s" , szSend );

Error:	
	
	return ret;
}

/**********************************************************************************/

errEV DoubleToStringUnderProtection ( tsLambdaChannel type , double lfVolts , char *pVoltsBuf , ViUInt32 *ipBufLength )
{
	errEV				ret								=	{0};
	
	char		szTemp[WR_BUF]		=	{0},
				szSend[WR_BUF]		=	{0};
	
	IF ((( lfVolts < type.underVoltageDefine.lfMinValue ) || ( lfVolts > type.underVoltageDefine.lfMaxValue )) , "Illegal parametr.", MODULE_NAME);
	
	sprintf ( szTemp , ":UVP%c0%3.1lflf;" , '%' , type.underVoltageDefine.lfAccuracy );
		
	sprintf ( szSend , szTemp , lfVolts );
	
	*ipBufLength = sprintf ( pVoltsBuf , "%s" , szSend );

Error:	
	
	return ret;
}

/**********************************************************************************/
