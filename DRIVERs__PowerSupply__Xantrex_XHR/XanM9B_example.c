/*=============================================================================
Copyright © 2002 Xantrex Technology, Inc. All rights reserved.

FILE NAME: XanM9B_example.c

PURPOSE:
   This sample program initializes the power supply, configures a channel to 
   supply power, and displays a message pop-up dialog.
   To run this sample program, do the following:
      1) Create a new project in LabWindows/CVI.
      2) Add this file to the project.  To add files to the project, select the
         Add Files To Project from the Edit menu of the Project window.
      3) Add one of the following files to the project:
         XANM9B.fp, XANM9B.c, or XANM9B.lib.
      4) Check that the resource name used in XANM9B_InitWithOptions()
         is correct. If simulating, change flag to simulate.
      5) Run the project.                             
   
NOTES: 
   Developed with LabWindows/CVI 6.0; Release Date: 29/11/2002 
=============================================================================*/ 

/******************************************************************** 
 * If you want to run this sample program and the instrument is not *
 * present, set the Simulate flag to 1.  (Example:  "Simulate = 1") * 
 *******************************************************************/

/*********************************************************************
 * Also, you can choose a model to use for simulation purposes.      *
 * This is done by adding "DriverSetup=Model: XFR20-60" or another   *
 * model of your choice to the Option String, of the                 *
 * XanM9B_InitWithOptions function. Include this string right        *
 * after 'Cache=1', inside the existing quotes for the option string.*
 * The default model used in simulation is an XFR12-100.             *
 ********************************************************************/

#include <stdio.h>
#include "XanM9B.h"

#ifdef _CVI_
#include <userint.h>
#endif

int main (void)
                 
{
   ViSession instr1;  // Instrument Handle
   ViStatus  error = VI_SUCCESS;
   ViReal64  measurement   = 0;
   ViReal64  currentLimit  = 0;
   ViReal64  voltageLevel  = 0;
   ViInt32   faultregister = 0;
   ViInt32   condition     = 0;
   ViInt32   outputMode    = 0;
   ViInt32   foldSetting   = 0;
   ViInt32   errCode       = 0;
   ViChar    errMessage[ 256 ];
   ViChar    driverRev[ 256 ];
   ViChar    popUpMsg[ 256 ];
   ViChar    instrRev[ 512 ];
   ViChar    instrInfo[ 512 ];
  
   checkErr( XanM9B_InitWithOptions( "GPIB::2::INSTR", VI_TRUE, VI_TRUE,
             "Simulate=0,RangeCheck=1,QueryInstrStatus=0,Cache=1",&instr1 ) ); 
 
  /* Un-Comment the following block of code if you are using RS232, and */
  /* comment out the preceding call to XanM9B_InitWithOptions           */
  /****************************************************************************
   * checkErr( XanM9B_InitWithOptions("ASRL1::INSTR", VI_TRUE, VI_TRUE,         
   *          "Simulate=0,RangeCheck=1,QueryInstrStatus=1,Cache=1",&instr1 ) );
   *                                                                            
   * checkErr( XanM9B_SetRemoteMode (instr1, XANM9B_VAL_REM ) );                
   ****************************************************************************/
   
   /**************************
    * Power Supply setpoints *
    *************************/
   checkErr( XanM9B_ConfigureOutputEnabled( instr1, "1", VI_TRUE ) );
   
   /* If necessary, change this setting to match your instruments range */
   checkErr( XanM9B_MaxVoltage( instr1,"1", 12 ) );
   /* If necessary, change this setting to match your instruments range */
   checkErr( XanM9B_MaxCurrentLimit( instr1, "1", 100 ) );
   /* If necessary, change this setting to match your instruments range */ 
   checkErr( XanM9B_ConfigureVoltageLevel( instr1, "1", 10 ) );
   /* If necessary, change this setting to match your instruments range */
   checkErr( XanM9B_ConfigureCurrentLimit( instr1, "1",
                                           XANM9B_VAL_CURRENT_REGULATE, 20 ) ); 
   
   /**********************************
    * Miscellaneous Driver Functions *
    *********************************/
   checkErr( XanM9B_UnmaskSetting( instr1,"1","CV,CC" ) );         
   checkErr( XanM9B_UnmaskCondition( instr1,"1",&condition ) );
   checkErr( XanM9B_MaskSetting( instr1, "1", "CV,CC" ) );

   checkErr( XanM9B_QueryAccumulatedState( instr1,"1", &faultregister ) );
   checkErr( XanM9B_revision_query( instr1, driverRev,instrRev ) );
   checkErr( XanM9B_Measure( instr1, "1", XANM9B_VAL_MEASURE_VOLTAGE, 
                             &measurement ) );
   
   checkErr( XanM9B_QuerySetpoint( instr1, "1", XANM9B_ATTR_CURRENT_LIMIT, 
                                   &currentLimit ) );
   checkErr( XanM9B_QuerySetpoint( instr1, "1", XANM9B_ATTR_VOLTAGE_LEVEL, 
                                   &voltageLevel ) );
   checkErr( XanM9B_QueryMode( instr1, "1", XANM9B_ATTR_OUTPUT_ENABLED, 
                               &outputMode ) );
   checkErr( XanM9B_QueryMode( instr1, "1", XANM9B_ATTR_FOLDBACK_SETTING, 
                               &foldSetting ) );
                               
   checkErr( XanM9B_GetAttributeViString( instr1, VI_NULL,
                                          XANM9B_ATTR_INSTRUMENT_MODEL, 512,
                                          instrInfo ) );
   checkErr( XanM9B_GetAttributeViString( instr1, VI_NULL,
                                          XANM9B_ATTR_INSTRUMENT_MANUFACTURER, 512,
                                          instrInfo ) );
   checkErr( XanM9B_GetAttributeViString( instr1, VI_NULL,
                                          XANM9B_ATTR_INSTRUMENT_FIRMWARE_REVISION, 512,
                                          instrInfo ) );                                

   sprintf( popUpMsg, "If you are not simulating, the power supply is now\n"
                      "configured to the specified settings." );
      
#ifdef _CVI_
   MessagePopup( "Message", popUpMsg );
#else
   printf( "Message:\n%s", popUpMsg );
#endif

Error:
   if( error != VI_SUCCESS)
   {
      ViChar errStr[ 2048 ];

      XanM9B_GetError( instr1, &error, 2048, errStr );
      
#ifdef _CVI_
      MessagePopup( "Error!", errStr );
#else
      printf( "Message:\n%s", popUpMsg );
#endif
   }
        
   if( instr1 )
   {
      XanM9B_close( instr1 );
   }
   
   return( 0 );
}
