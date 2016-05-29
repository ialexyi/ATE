#include <ansi_c.h>
#include <formatio.h>
#include "chr62000.h"
#include "chr62000_example.h"
#include <utility.h>
#include <userint.h> 

int CVICALLBACK abortApplication (ViStatus error);
int CVICALLBACK model_select (void); 

ViSession	vi;
ViInt16 	controlHandle,configHandle; 

int main (int argc, char *argv[])
{
    if (InitCVIRTE (0, argv, 0) == 0)
        return -1; 
    if ((configHandle = LoadPanel (0, "chr62000_example.uir", CONFIG)) <= 0)
        return -1;
    if ((controlHandle = LoadPanel (0, "chr62000_example.uir", CONTROL)) <= 0)
        return -1;

    /* Display the panel and run the UI */
    DisplayPanel (configHandle); 
    RunUserInterface ();

    /* Free resources and return */
    DiscardPanel (controlHandle);
    CloseCVIRTE ();
	    
    return 0;
}

int CVICALLBACK next (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	ViStatus	error = VI_SUCCESS; 
	int 		resource=0,gpib_addr=0,com_port=0,baudrate=0,simulate=0;
	char 		ResName[20]="";
	char 		ID[512]="", Model[512]="";
	
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal (panel, CONFIG_RESOURCE, &resource); 
			GetCtrlVal (panel, CONFIG_GPIB_ADDR, &gpib_addr); 
			GetCtrlVal (panel, CONFIG_COM_PORT, &com_port);
			GetCtrlVal (panel, CONFIG_BAUDRATE, &baudrate);
			GetCtrlVal (panel, CONFIG_SIMULATE, &simulate); 
			if (resource==1)	
				sprintf (ResName,"ASRL%d::INSTR",com_port);
			else
				sprintf (ResName,"GPIB0::%d::INSTR",gpib_addr);
				
			if (simulate)
			{
				if ((chr62000_InitWithOptions (ResName, VI_TRUE, VI_FALSE,
                          "Simulate=1,RangeCheck=1,QueryInstrStatus=1,Cache=1", &vi)) !=0)
				{                
                    MessagePopup ("Simulation Error","Can not simulate CHROMA 62000P DC power supply.");
                    QuitUserInterface (0);
                    break;
                }    
            }              
			else
			{
				if ((chr62000_InitInterface (ResName, VI_TRUE, VI_FALSE, baudrate, ID, Model, &vi))!=0)
				{
					if (ConfirmPopup ("Initial Error", "Initial CHROMA 62000P DC power supply error. Do you want to quit?"))
					{
						QuitUserInterface (0); 
						break;
					}	
					break;
				}
			}				
				
			HidePanel (panel);
			model_select ();
			DisplayPanel (controlHandle); 
			SetCtrlAttribute (controlHandle, CONTROL_TIMER, ATTR_ENABLED, 1);
			break;
		case EVENT_RIGHT_CLICK:
			MessagePopup ("Help", "After finished the above setting, continue with the next control panel.");
			break;
		}

Error: 			
	if (error!=0)
		abortApplication (error); 
	return error;
}

int CVICALLBACK model_select (void)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	current=0,voltage=0,vsr=0,isr=0;
	int			output_state=0;
	char 		ID[512]="", model[512]="",msg[512]="";
	
	checkErr( chr62000_GetAttributeViString (vi, "", CHR62000_ATTR_ID_QUERY_RESPONSE, 512, ID));
	checkErr( chr62000_GetAttributeViString (vi, "", CHR62000_ATTR_INSTRUMENT_MODEL, 512, model)); 
	
	if ( strncmp(model,"62006P-100-25",13) == 0 )							  			
		current=25,voltage=100;
	else if ( strncmp(model,"62012P-30-160",13) == 0 )
		current=160,voltage=30;
	else if ( strncmp(model,"62012P-80-60",12) == 0 )
		current=60,voltage=80; 	
	else if ( strncmp(model,"62012P-100-50",13) == 0 )
		current=50,voltage=100; 	
	else if ( strncmp(model,"62012P-600-8",12) == 0 )
		current=8,voltage=600; 
		
	SetCtrlAttribute (controlHandle, CONTROL_VOLTAGE_KNOB, ATTR_MAX_VALUE, voltage); 
	SetCtrlAttribute (controlHandle, CONTROL_CURRENT_KNOB, ATTR_MAX_VALUE, current); 

	checkErr( chr62000_SetVoltageLimit (vi, VI_TRUE, voltage)); 
	checkErr( chr62000_SetVoltageLimit (vi, VI_FALSE, 0.00)); 
	checkErr( chr62000_SetCurrentLimit (vi, VI_TRUE, current)); 
	checkErr( chr62000_SetCurrentLimit (vi, VI_FALSE, 0.00)); 

	GetCtrlVal (controlHandle, CONTROL_OUTPUT_SWITCH, &output_state); 	
	GetCtrlVal (controlHandle, CONTROL_VOLTAGE_KNOB, &voltage); 
	GetCtrlVal (controlHandle, CONTROL_CURRENT_KNOB, &current);
	GetCtrlVal (controlHandle, CONTROL_VOLT_SR, &vsr);
	GetCtrlVal (controlHandle, CONTROL_CURR_SR, &isr); 
	
	checkErr( chr62000_SetVoltage (vi, voltage));
	checkErr( chr62000_SetCurrent (vi, current));
	checkErr( chr62000_SetOutput (vi, output_state));
	checkErr( chr62000_SetVoltageSlew (vi, vsr));
	checkErr( chr62000_SetCurrentSlew (vi, isr));
	checkErr( chr62000_SetAPGMode (vi, 0, VI_FALSE));
	
	SetCtrlAttribute (controlHandle, CONTROL_VOLT_MEAS, ATTR_TEXT_BGCOLOR, VAL_BLACK);
	SetCtrlAttribute (controlHandle, CONTROL_CURR_MEAS, ATTR_TEXT_BGCOLOR, VAL_BLACK); 
	SetCtrlAttribute (controlHandle, CONTROL_PWR_MEAS, ATTR_TEXT_BGCOLOR, VAL_BLACK); 
	sprintf (msg, "*IDN Response: %s \nModel: %s", ID, model);
	SetCtrlVal (controlHandle, CONTROL_IDN, msg);
	Delay (0.1);
	
Error: 			
	if (error!=0)
		abortApplication (error); 
	return error;
}

int CVICALLBACK ResourceSel (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int resource=0;
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal (panel, CONFIG_RESOURCE, &resource);
			if (resource==1) //if resource is RS232, then dim the GPIB address
			{
				SetCtrlAttribute (panel, CONFIG_GPIB_ADDR, ATTR_DIMMED, 1);
				SetCtrlAttribute (panel, CONFIG_COM_PORT, ATTR_DIMMED, 0);
				SetCtrlAttribute (panel, CONFIG_BAUDRATE, ATTR_DIMMED, 0);
			}
			else if (resource==0) //if resource is GPIB, then dim the COM Port and Baud Rate
			{
				SetCtrlAttribute (panel, CONFIG_COM_PORT, ATTR_DIMMED, 1);
				SetCtrlAttribute (panel, CONFIG_BAUDRATE, ATTR_DIMMED, 1);
				SetCtrlAttribute (panel, CONFIG_GPIB_ADDR, ATTR_DIMMED, 0); 
			}
			break;
		case EVENT_RIGHT_CLICK:
			MessagePopup ("Help", "Select the interface you want to connect the instrument.");
			break;	
		}
	return 0;
}														  

void CVICALLBACK help (int menuBar, int menuItem, void *callbackData, int panel)
{
    if (panel == controlHandle)
        MessagePopup ("Panel Help",
            "This example demonstrates the CHROMA 62000P series DC power\n"
            "supply with chr62000 IVI instrument driver.\n\n"
            "You may control the voltage, current, voltage slew rate, current slew\n"
            "rate, enable or disable output of the instument, and get measurement\n"
            "on the display.\n\n"
            "Right-click on each control for more help.\n");
    if (panel == configHandle) 
        MessagePopup ("Panel Help",
            "This panel configures the instrument to connect,  \n"
            "or simulate whether the instrument exists or not.\n\n"
            "Right-click on each control for more help.\n");
}

int CVICALLBACK QUIT (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{

	switch (event)
		{
		case EVENT_COMMIT:
			if (ConfirmPopup ("Quit?", "Do you really want to quit chr62000 IVI driver example?")) 
			{
				SetCtrlAttribute (controlHandle, CONTROL_TIMER, ATTR_ENABLED, 0); 
				QuitUserInterface (0);
				chr62000_close (vi);
				Delay (0.5);
				break;
			}
			break;
		case EVENT_RIGHT_CLICK:
			MessagePopup ("Help", "Quit chr62000 IVI driver example program.");
			break;					
		}	
		
	return 0;
}

int  CVICALLBACK CONTROL_ACTION(int panel, int event, 
		void *callbackData, int eventData1, int eventData2)
{

	switch (event)
		{
		case EVENT_CLOSE:
			if (ConfirmPopup ("Quit?", "Do you really want to quit chr62000 IVI driver example?")) 
			{
				SetCtrlAttribute (controlHandle, CONTROL_TIMER, ATTR_ENABLED, 0); 
				QuitUserInterface (0);
				chr62000_close (vi);
				Delay (0.5);
				break;
			}
			break;
		}	
		
	return 0;
}

int CVICALLBACK OUTPUT (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	ViStatus	error = VI_SUCCESS; 
	int 		value; 
	
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal (panel, CONTROL_OUTPUT_SWITCH, &value);
			checkErr( chr62000_SetOutput (vi, value));
			break;
		case EVENT_RIGHT_CLICK:
			MessagePopup ("Help", "Enable or disable the power supply's output voltage.");
			break;			
		}
Error: 			
	if (error!=0)
		abortApplication (error);
	return error;
}

int CVICALLBACK abortApplication (ViStatus error)
{
	char		message[200]="";
	
	if (error>=0xbffa4001 && error <=0xbffa4003) // out of range
	{
		if (error==0xbffa4001)
			sprintf (message, "The value you passed is out of range.");
		else if (error==0xbffa4002)			
			sprintf (message, "The value you passed is out of range of voltage limit.");
		else if (error==0xbffa4003)
			sprintf (message, "The value you passed is out of range of current limit.");
		MessagePopup ("Error", message); 
	}
	else if (error >= 3220963328)  //0xBFFC00000
	{
		MessagePopup ("Error", "I/O or System error occured. Program now abort.");  		
		QuitUserInterface (0);
		chr62000_close (vi);
		Delay (0.5); 
	}	
	
	return 0;
}

int CVICALLBACK SETVOLT (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	ViStatus	error = VI_SUCCESS; 
	ViReal64 	value=0;  
	
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal (panel, CONTROL_VOLTAGE_KNOB, &value); 
			checkErr( chr62000_SetVoltage (vi, value));
			break;
		case EVENT_RIGHT_CLICK:
			MessagePopup ("Help", 
			"Pass the voltage you want the power supply to attempt to generate.\n\n"
			"Units: volt\n\n"
			"Valid Range:\n"
			"     Model            |      Voltage\n"
			"-----------------------------------------\n"
			"62006P-100-25  |  0.00 - 100.00\n"
			"62012P-30-160  |  0.00 -   30.00\n"
			"62012P-80-60    |  0.00 -   80.00\n"
			"62012P-100-50  |  0.00 - 100.00\n"
			"62012P-600-8    |  0.00 - 600.00\n");
			break;	
		}
Error: 			
	if (error!=0)
		abortApplication (error); 
	return error;
}

int CVICALLBACK REFRESH_MEAS (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64 	value=0;
	char		str[30];
	switch (event)				  
		{
		case EVENT_TIMER_TICK:
			checkErr( chr62000_Measure (vi, 0, &value));
			sprintf (str, "%.4f", value);
			SetCtrlVal (panel, CONTROL_VOLT_MEAS, str); 
			checkErr( chr62000_Measure (vi, 1, &value));
			sprintf (str, "%.4f", value);
			SetCtrlVal (panel, CONTROL_CURR_MEAS, str);
			checkErr( chr62000_Measure (vi, 2, &value));
			sprintf (str, "%.1f", value);
			SetCtrlVal (panel, CONTROL_PWR_MEAS, str);
			break;
		}
Error: 			
	if (error!=0)
		abortApplication (error); 
	return error;
}

int CVICALLBACK SETCURR (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64 	value=0;
	
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal (panel, CONTROL_CURRENT_KNOB, &value); 
			checkErr( chr62000_SetCurrent (vi, value)); 
			break;
		case EVENT_RIGHT_CLICK:
			MessagePopup ("Help", 
			"Pass the current you want the power supply to attempt to generate.\n\n"
			"Units: amp\n\n" 
			"Valid Range:\n"
			"     Model            |      Current\n"
			"-----------------------------------------\n"
			"62006P-100-25  |  0.00 -   25.00\n"
			"62012P-30-160  |  0.00 - 160.00\n"
			"62012P-80-60    |  0.00 -   60.00\n"
			"62012P-100-50  |  0.00 -   50.00\n"
			"62012P-600-8    |  0.00 -     8.00\n");
			break;			
		}
Error: 			
	if (error!=0)
		abortApplication (error); 
	return error;
}

int CVICALLBACK SetVSR (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64 	value=0;
																	  
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal (panel, CONTROL_VOLT_SR, &value); 
			checkErr( chr62000_SetVoltageSlew (vi, value)); 
			break;
		case EVENT_RIGHT_CLICK:
			MessagePopup ("Help", "Pass the voltage slew rate to the power supply.\n\n"
			"Units: V/ms");
			break;			
		}
Error: 			
	if (error!=0)
		abortApplication (error); 
	return error;
}

int CVICALLBACK SetISR (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64 	value=0;

	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal (panel, CONTROL_CURR_SR, &value); 
			checkErr( chr62000_SetCurrentSlew (vi, value)); 
			break;
		case EVENT_RIGHT_CLICK:
			MessagePopup ("Help", "Pass the current slew rate to the power supply.\n\n"
			"Units: A/ms"); 
			break;			
		}
Error: 			
	if (error!=0)
		abortApplication (error); 
	return error;
}

int CVICALLBACK CLS (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	ViStatus	error = VI_SUCCESS;
	ViChar 		err_message[256]="",show_message[256]="";
	ViInt32		error_code=0;
	
	switch (event)
		{
		case EVENT_COMMIT:
			checkErr( chr62000_error_query (vi, &error_code, err_message));
			sprintf (show_message, "Error Code: %d, Error Message: %s. The instrument status was cleared.", error_code, err_message);
			MessagePopup ("Message",show_message); 
			checkErr( chr62000_SetCLS (vi));
			break;
	case EVENT_RIGHT_CLICK:
			MessagePopup ("Help", "Clear the power supply's status.");
			break;				
		}
Error: 			
	if (error!=0)
		abortApplication (error); 
	return error;

}

int CVICALLBACK QUIT_CONFIG (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			if (ConfirmPopup ("Quit?", "Do you really want to quit connection setting?"))
			{
				QuitUserInterface (0); 
				break;
			}	
			break;
		case EVENT_RIGHT_CLICK:
			MessagePopup ("Help", "Quit connection setting.");
			break;
		}
	return 0;
}

int CVICALLBACK CONFIG_ACTION (int panel, int event, void *callbackData, 
		int eventData1, int eventData2)		
{
	switch (event)
		{
		case EVENT_CLOSE:
			if (ConfirmPopup ("Quit?", "Do you really want to quit connection setting?"))
			{
				QuitUserInterface (0); 
				break;
			}	
			break;
		}
	return 0;
}

int CVICALLBACK GPIBADDR (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_RIGHT_CLICK:
			MessagePopup ("Help", "Select the GPIB address 1-30.");
			break;	
		}
	return 0;
}

int CVICALLBACK COMPORT (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_RIGHT_CLICK:
			MessagePopup ("Help", "Select which COM Port you use for connection.");
			break;
		}
	return 0;
}

int CVICALLBACK SIMU (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_RIGHT_CLICK:
			MessagePopup ("Help", "Select simulation if the instrument does not exist.");
			break;
		}
	return 0;
}

int CVICALLBACK BAUDRATE (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_RIGHT_CLICK:
			MessagePopup ("Help", "Select baud rate the COM Port you used.");
			break;
		}
	return 0;
}
