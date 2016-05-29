//==============================================================================
// Include files

#include "Helper.h"

#include <utility.h>
#include <ansi_c.h>
#include <userint.h>
#include <cvirte.h>
#include "LambdaZup.h"
//==============================================================================
// Constants

#define		ZUP_ADDR	1

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

int main (int argc, char *argv[])
{
	/*	
	int 		iStatus				=	0;
	
	ViSession   viInstrumentHandle	=	0;
	
	char		szReturnString[256]	=	{0};
	
	int			iReturnInt			=	0;
	
	double		lfReturnDouble		=	0.0;
	
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;    
	
	SetBreakOnLibraryErrors (0);
	
	CHK_ZUP ( ZUP_Init ( "ASRL4" , &viInstrumentHandle ));
	CHK_ZUP ( ZUP_ClearCommBufferAndRegisters ( viInstrumentHandle , ZUP_ADDR )) ;

	CHK_ZUP ( ZUP_GetPowerSupplyModel ( viInstrumentHandle , ZUP_ADDR , szReturnString ));
	
	MessagePopup("ZUP_GetPowerSupplyModel:",szReturnString); 
	
	CHK_ZUP ( ZUP_GetPowerSupplyRevision ( viInstrumentHandle , ZUP_ADDR , szReturnString ));
	
	MessagePopup("ZUP_GetPowerSupplyRevision:",szReturnString);
		
	CHK_ZUP ( ZUP_SetOutputVoltage ( viInstrumentHandle , ZUP_ADDR , 18.0 ));
	
	CHK_ZUP ( ZUP_SetOutputCurrent ( viInstrumentHandle , ZUP_ADDR , 1.5 ));
	
	CHK_ZUP ( ZUP_SetOutputState ( viInstrumentHandle , ZUP_ADDR , 1 ));

	CHK_ZUP ( ZUP_GetOutputState ( viInstrumentHandle , ZUP_ADDR , &iReturnInt ));
	
	sprintf(szReturnString, "%d" , iReturnInt);
	MessagePopup("ZUP_GetOutputState:",szReturnString); 
		
	CHK_ZUP ( ZUP_GetOutputVoltageDefined ( viInstrumentHandle , ZUP_ADDR , &lfReturnDouble ));
	
	sprintf(szReturnString, "%lf" , lfReturnDouble);
	MessagePopup("ZUP_GetOutputVoltageDefined:",szReturnString);
	
	CHK_ZUP ( ZUP_GetOutputVoltageActual ( viInstrumentHandle , ZUP_ADDR , &lfReturnDouble ));
	
	sprintf(szReturnString, "%lf" , lfReturnDouble);
	MessagePopup("ZUP_GetOutputVoltageActual:",szReturnString);
	
	CHK_ZUP ( ZUP_GetOutputCurrentDefined ( viInstrumentHandle , ZUP_ADDR , &lfReturnDouble ));
	
	sprintf(szReturnString, "%lf" , lfReturnDouble);
	MessagePopup("ZUP_GetOutputCurrentDefined:",szReturnString);
	
	CHK_ZUP ( ZUP_GetOutputCurrentActual ( viInstrumentHandle , ZUP_ADDR , &lfReturnDouble ));
	
	sprintf(szReturnString, "%lf" , lfReturnDouble);
	MessagePopup("ZUP_GetOutputCurrentActual:",szReturnString);
	
	CHK_ZUP ( ZUP_SetOutputState ( viInstrumentHandle , ZUP_ADDR , 0 ));

	CHK_ZUP ( ZUP_GetOutputState ( viInstrumentHandle , ZUP_ADDR , &iReturnInt ));
	
	sprintf(szReturnString, "%d" , iReturnInt);
	MessagePopup("ZUP_GetOutputState:",szReturnString); 

	CHK_ZUP ( ZUP_SetVoltageProtectionOver ( viInstrumentHandle , ZUP_ADDR , 22.0 ));
	
	CHK_ZUP ( ZUP_GetVoltageProtectionOver ( viInstrumentHandle , ZUP_ADDR , &lfReturnDouble ));
	
	sprintf(szReturnString, "%lf" , lfReturnDouble);
	MessagePopup("ZUP_GetVoltageProtectionOver:",szReturnString);
		
	CHK_ZUP ( ZUP_SetVoltageProtectionUnder ( viInstrumentHandle , ZUP_ADDR , 0.0 ));
	
	CHK_ZUP ( ZUP_GetVoltageProtectionUnder ( viInstrumentHandle , ZUP_ADDR , &lfReturnDouble ));
	
	sprintf(szReturnString, "%lf" , lfReturnDouble);
	MessagePopup("ZUP_GetVoltageProtectionUnder:",szReturnString);
		
	CHK_ZUP ( ZUP_SetFoldbackProtectionMode ( viInstrumentHandle , ZUP_ADDR , 2 ));
	
	CHK_ZUP ( ZUP_GetFoldbackProtectionMode ( viInstrumentHandle , ZUP_ADDR , &iReturnInt ));
	
	sprintf(szReturnString, "%d" , iReturnInt);
	MessagePopup("ZUP_GetFoldbackProtectionMode:",szReturnString);
		
	CHK_ZUP ( ZUP_GetAutoRestartMode ( viInstrumentHandle , ZUP_ADDR , &iReturnInt ));
	
	sprintf(szReturnString, "%d" , iReturnInt);
	MessagePopup("ZUP_GetAutoRestartMode:",szReturnString);
		
	CHK_ZUP ( ZUP_GetOperationalStatusRegister ( viInstrumentHandle , ZUP_ADDR , szReturnString ));
	
	MessagePopup("ZUP_GetOperationalStatusRegister:",szReturnString);
		
	CHK_ZUP ( ZUP_GetAlarmStatusRegister ( viInstrumentHandle , ZUP_ADDR , szReturnString ));
		
	MessagePopup("ZUP_GetAlarmStatusRegister:",szReturnString);
	
	CHK_ZUP ( ZUP_GetErrorCodesRegister ( viInstrumentHandle , ZUP_ADDR , szReturnString ));

	MessagePopup("ZUP_GetErrorCodesRegister:",szReturnString);
		
	CHK_ZUP ( ZUP_GetCompleteStatus ( viInstrumentHandle , ZUP_ADDR , szReturnString ));
	
	MessagePopup("ZUP_GetCompleteStatus:",szReturnString);

Error:
	
	SetBreakOnLibraryErrors (1);
	
	if (iStatus)
	{
		iStatus = ZUP_GetErrorTextMessage ( viInstrumentHandle , iStatus , szReturnString );
		MessagePopup("Error!",szReturnString);	
	}
	
	ZUP_Close ( viInstrumentHandle );
	*/		
	return 0;
}
