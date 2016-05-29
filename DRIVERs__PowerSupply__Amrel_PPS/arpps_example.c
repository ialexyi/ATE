/***************************************************************************** 
 * Copyright 2000-2013 National Instruments Corporation.  All Rights Reserved. * 
 *****************************************************************************/

/*****************************************************************************
 *  arpps Instrument Driver Sample Program                              
 *  Developed with LabWindows/CVI 5.1                                     
 *  Original Release: 3/11/2000                             
 *
 *  Purpose:  This sample program initializes the power supply, configures
 *            a channel to supply power, and displays a message pop-up dialog.
 *
 *            To run this sample program, do the following:
 *            1) Create a new project in LabWindows/CVI.
 *            2) Add this file to the project.  To add files to the project,
 *               select the Add Files To Project from the Edit menu of the 
 *               Project window.
 *            3) Add one of the following files to the project:
 *               arpps.fp, arpps.c, or arpps.lib.
 *            4) Check that the resource name used in arpps_InitWithOptions()
 *               is correct. If simulating, change flag to simulate.
 *            5) Run the project.
 *
 *****************************************************************************/

#include <stdio.h>
#include "arpps.h"

#ifdef _CVI_
#include <userint.h>
#endif

int main (void)
{
    ViSession   arpps;
    ViStatus    error = VI_SUCCESS;
    ViBoolean   overVoltage = VI_FALSE, overCurrent = VI_FALSE;
    ViChar      popUpMsg[256];

        /* 
            If you want to run this sample program and the instrument is not
            present, set the Simulate flag to 1.  (Example:  "Simulate = 1")
        */
    checkErr( arpps_InitWithOptions ("GPIB::14::INSTR", VI_TRUE, VI_TRUE,
                                        "Simulate=0,RangeCheck=1,QueryInstrStatus=0,Cache=1",
                                        &arpps));
                                        
    /* After reading values from panel, set voltage  */
    /* to zero first to prevent overvoltage          */
    /* tripping.  Then call ConfigureDCV             */
	checkErr( arpps_ConfigureOutputEnabled (arpps, "1", VI_FALSE));
    checkErr( arpps_ConfigureVoltageLevel (arpps, "1", 0.0));
    checkErr( arpps_ConfigureCurrentLimit (arpps, "1", ARPPS_VAL_CURRENT_TRIP, 1.0));
	checkErr( arpps_ConfigureOVP (arpps, "1", VI_FALSE, 10.0));
	checkErr( arpps_ConfigureVoltageLevel (arpps, "1", 5.0));
	checkErr( arpps_ConfigureOutputEnabled (arpps, "1", VI_TRUE));
    checkErr( arpps_QueryOutputState (arpps, "1", ARPPS_VAL_OUTPUT_OVER_VOLTAGE, &overVoltage));
	checkErr( arpps_QueryOutputState (arpps, "1", ARPPS_VAL_OUTPUT_OVER_CURRENT, &overCurrent));

    if (overVoltage || overCurrent)
        {
        popUpMsg[0] = 0;
        if (overVoltage)
            sprintf (popUpMsg, "An over-voltage condition occurred.\n");
        if (overCurrent)
            sprintf (popUpMsg, "%sAn over-current condition occurred.", popUpMsg);
        }
    else
        {
        sprintf (popUpMsg, "The power supply is generating the power you configured.");
        }

#ifdef _CVI_
	MessagePopup ("Message", popUpMsg);
#else
	printf("Message:\n%s", popUpMsg);
#endif
	
        
Error:
    if (error != VI_SUCCESS)
        {
            ViChar   errStr[2048];

            arpps_GetError (arpps, &error, 2048, errStr);
#ifdef _CVI_
            MessagePopup ("Error!", errStr);
#else
			printf("Message:\n%s", popUpMsg);
#endif
        }

    arpps_ConfigureOutputEnabled (arpps, "1", VI_FALSE);

    if (overVoltage || overCurrent)
		arpps_ResetOutputProtection (arpps, "1");
        
    if (arpps)
        arpps_close (arpps);
	
	return 0;
}
