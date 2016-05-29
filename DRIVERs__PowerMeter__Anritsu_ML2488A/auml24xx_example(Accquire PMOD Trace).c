/***************************************************************************** 
* (c) 2007, National Instruments, Corporation. All Rights Reserved.	     * 
*****************************************************************************/

/*****************************************************************************
*  Anritsu 248x and 249x Power Meter
*  Instrument Driver Sample Program                              
*  Developed with LabWindows/CVI 8.1                                     
*  Original Release: 02/28/2008                                  
*
*  Purpose:  This sample program initializes the power meter and configures 
*            it for acquiring measurement.
*
*            To run this sample program, do the following:
*            1) Create a new project in LabWindows/CVI.
*            2) Add this file to the project.  To add files to the project,
*               select the Add Files To Project from the Edit menu of the 
*               Project window.
*            3) Add one of the following files to the project:
*               auml24xx.fp, auml24xx.c, or auml24xx.lib.
*            4) Check that the resource name used in auml24xx_init ()
*               function is correct.
*   		 5) Run the project.
*            VCC or Borland Users:
*            This example uses CVI's WaveformGraphPopup() to display the
*            waveform array.  Replace this WaveformGraphPopup() function with a 
*            similar display function.  Remove the include statement: 
*            #include <userint.h> from the source code.  Follow the 
*            directions for CVI users given above.
******************************************************************************/


#if defined(_CVI_)
	#include <userint.h>
#endif

#include <cvirte.h> 
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <formatio.h>
#include <ansi_c.h> 


#include "auml24xx.h"

int main (void)
{
    ViStatus    error = VI_SUCCESS;
	ViSession   auml24xx;
    ViReal64    initial_x,
				stop,
				increment_x;
    ViInt32     actual_trace_size,
				actual_points;
    ViReal64    trace[200];
	
	/*- Initialize the power meter ----------------------------------------*/ 
	checkErr( auml24xx_InitWithOptions ("PM", VI_TRUE, VI_TRUE,
									 "Simulate=0,RangeCheck=1,QueryInstrStatus=0,Cache=1",
                                      &auml24xx));

	/*- Configure the active channel ----------------------------------------*/ 
	checkErr(auml24xx_ConfigureActiveChannel(auml24xx,"1"));

	/*- Configure the units ----------------------------------------*/ 
	checkErr(auml24xx_ConfigureUnits(auml24xx,AUML24XX_VAL_DBM));

	/*- Configure the measurement mode ----------------------------------------*/ 
	checkErr(auml24xx_ConfigureMeasurementMode(auml24xx,"1",AUML24XX_VAL_MEAS_MODE_PMOD));

	/*- Configure the measurement  ----------------------------------------*/ 
    checkErr(auml24xx_ConfigureMeasurement(auml24xx,AUML24XX_VAL_NONE,"A","B"));

	/*- Enable the averaging ----------------------------------------*/ 
	checkErr(auml24xx_ConfigureAveragingEnabled(auml24xx,"1",VI_TRUE));

	/*- Configure the averaging count ----------------------------------------*/ 
    checkErr(auml24xx_ConfigureAveragingCount(auml24xx,"1",10));	

	/*- Configure the sensor offset ----------------------------------------*/ 
    checkErr(auml24xx_ConfigureSensorOffset(auml24xx,"A",10));

	/*- Configure the sensor correction frequency ----------------------------------------*/ 
    checkErr(auml24xx_ConfigureSensorCorrectionFrequency(auml24xx,"A",5E+7));

	/*- Configure the trigger source ----------------------------------------*/ 
	checkErr(auml24xx_ConfigureTriggerSource(auml24xx,AUML24XX_VAL_CONT));

	/*- Read PMOD trace ----------------------------------------*/ 
    checkErr(auml24xx_ReadPMODTrace(auml24xx,"1",&actual_points,trace,200,&initial_x,&increment_x, 10000));

#if defined(_CVI_)	
	WaveformGraphPopup ("Acquired PMOD Trace", trace, actual_points, VAL_DOUBLE, 1.0, 0.0, initial_x, increment_x);
#endif
	
Error:
	auml24xx_close(auml24xx);
	return 0;
}




