/*=============================================================================
Copyright © 2003 Xantrex Technology, Inc. All rights reserved.

FILE NAME: XanM9B.c

PURPOSE:
   Instrument Driver for Xantrex XFR-XHR-XPD-HPD-XT Series DC 
   Power Supplies equipped with an M9B (RS232 or GPIB) Interface card.
 
NOTES: 
   Developed using LabWindows/CVI 6.0; Original Release April 14, 2002                                  
   
CHANGE HISTORY:
   Date: Jul-23-2002  By: Jeff Fieldhouse
   Revision: 2.0
   - Added support for RS-232 (Serial).
   - Added support for Xantrex XFR 1.2kW and XFR 2.8kW power supplies.
   - Added support for Xantrex XPD series power supplies.
   - Added high level function "XanM9B_QuerySetpoint" to allow
     querying of Voltage and Current setpoints, as well as Over
     Voltage Protection.
   - Added high level function "XanM9B_QueryMode" to allow querying of
     Output and Hold state, Auxilary A and B lines, and Foldback Mode. 
   - Removed OPF bit from IEEE_488_2_ERROR_BITS definition as it is 
     prone to cause false errors on the XFR.
   - Changed Ivi_AddAttributeViReal64 funtions to specify 
     four digits of precision (used to compare cached values).
     
   Date: Jul-29-2002  By: Jeff Fieldhouse
   Revision: 3.0
   - Added support for Xantrex HPD series power supplies.
   - Added support for Xantrex XT series power supplies.
   - Added Fault Delay Query to user-callable function "XanM9B_QuerySetpoint()"
   - Deleted utility function "XanM9B_GetDriverSetupOption()" as it 
     was never called.
   - Changed OVPMinVoltage from 5% of max output, to 0%
     Note: setting OVP to 0 volts will disable the output.
   - Re-organized the Function Tree for easier navigation.
 
   Date: Sep-12-2002  By: Jeff Fieldhouse 
   Revision:4.0
   - Put XanM9B_GetDriverSetupOption() back in to driver to comply with 
     IVI specs.
   - Improved Range Checking on select funcitons.
   - Added the 'Behavior' control to XanM9B_ConfigureCurrentLimit.
   - Added the following functions to comply with IVI specifications
     XanM9B_ConfigureOutputRange          
     XanM9B_ConfigureTriggerSource
     XanM9B_Initiate
     XanM9B_Abort
     XanM9B_SendSoftwareTrigger
     XanM9B_QueryOutputState
     XanM9B_ClearStatus
     XanM9B_QueryMaxVoltageLevel
     XanM9B_QueryMaxCurrentLimit 
     XanM9B_SelfTest
     XanM9B_ResetWithDefaults
     XanM9B_Disable
     XanM9B_InvalidateAllAttributes
     XanM9B_GetChannelName
     XanM9B_GetError
     XanM9B_ClearError
     XanM9B_GetNextInterchangeWarning
     XanM9B_ClearInterchangeWarnings
     XanM9B_ResetInterchangeCheck
   - Deprecated the following functions to comply with IVI-3.2: specs
     XanM9B_GetErrorInfo
     XanM9B_ClearErrorInfo
   - Deprecated the following attributes to comply with IVI-3.2: specs 
     XANM9B_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION
     XANM9B_ATTR_SPECIFIC_DRIVER_MINOR_VERSION
     XANM9B_ATTR_PRIMARY_ERROR                     
     XANM9B_ATTR_SECONDARY_ERROR                           
     XANM9B_ATTR_ERROR_ELABORATION
     XANM9B_ATTR_IO_SESSION_TYPE
     XANM9B_ATTR_IO_SESSION
   - Renamed the following attributes to comply with IVI-3.2: specs
     XANM9B_ATTR_NUM_CHANNELS        
            changed to XANM9B_ATTR_CHANNEL_COUNT
     XANM9B_ATTR_QUERY_INSTR_STATUS  
            changed to XANM9B_ATTR_QUERY_INSTRUMENT_STATUS
     XANM9B_ATTR_RESOURCE_DESCRIPTOR 
            changed to XANM9B_ATTR_IO_RESOURCE_DESCRIPTOR
   - Added the following attributes to comply with the IVI-3.2: specs
     XANM9B_ATTR_DRIVER_SETUP
     XANM9B_ATTR_INTERCHANGE_CHECK
 
   Date: Nov-29-2002  By: Jeff Fieldhouse 
   Revision:4.01
   - Added the parameter ViChar errorMessage to the XanM9B_error_query function.
     It was missing prior to this version. Since the M9B products do not return
     a message with the error code, the messages are generated within the 
     function itself.
   - Changed the voltage range for all units to be -maxVoltage to +maxVoltage,
     instead of zero to +maxVoltage.
     Note: Sending a negative voltage to the unit will assert the polarity 
     signal at the back of the power supply. True negative voltages are not 
     generated.
   - Changed XanM9B_ConfigureCurrentLimit so that it accepts the value
     XANM9B_VAL_CURRENT_TRIP. If this value is selected the driver will first 
     set the current limit behavior of the power supply by setting the Foldback 
     mode to Constant Current (CC). It will also set the fault delay time to 
     3 seconds. This will avoid nuisance faults during configuration. It will 
     then set the current limit. If, at any time after the 3 seconds has elapsed,
     the instrument switches into CC mode, the output will Trip.
   - Added the function XanM9B_SetRemoteMode, to allow users to put the unit
     into local or remote state.
   - Added the file variable 'ViReal64 configureableDelay,' which defaults to 
     zero. If the user is communicating via RS232, this delay is set to 75ms.
     
   Date: Dec-31-2002  By: Jeff Fieldhouse 
   Revision:4.02 
   - Fixed some text errors in the function panels.
   
   Date: Jan-03-2003   By: Jeff Fieldhouse
   Revision: 4.03 
   - Fixed bug that caused XanM9BAttrRemoteMode_ReadCallback to generate an 
     error when viScanf() was called ('&' was missing from the parameter)  	   

 *  5/28/2003 - [AL] Set attribute IVI_ATTR_IO_RESOURCE_DESCRIPTOR 
 *                        in initialize time.   
=============================================================================*/

#include <string.h>
#include <stdio.h>  
#include <formatio.h>
#include <ctype.h>
#include <utility.h> 
#include "XanM9B.h"


/*****************************************************************************
 *----------------------------- File Variables ------------------------------*
 *****************************************************************************/
ViReal64 configureableDelay = 0; // Time delay after a write to the instrument

/*****************************************************************************
 *--------------------- Hidden Attribute Declarations -----------------------*
 *****************************************************************************/

#define XANM9B_ATTR_OPC_TIMEOUT       (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 1L)   
#define XANM9B_ATTR_INSTR_STATUS_REG  (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 2L)

/*****************************************************************************
 *---------------------------- Useful Macros --------------------------------*
 *****************************************************************************/

    /*- I/O buffer size -----------------------------------------------------*/
#define BUFFER_SIZE                 512L        
#define XANM9B_IO_SESSION_TYPE      IVI_VAL_VISA_SESSION_TYPE

    /*- 488.2 Event Status Register (ESR) Bits ------------------------------*/
#define IEEE_488_2_ERROR_BITS       0x1498   /* OV,OT,ERR,ACF,SNSP */

#define XANM9B_MODEL_NOT_FOUND      -1L  
#define SETUP_MODEL_TAG             "Model"

    /*- Mask bits for XanM9B_QueryOutputState function-----------------------*/
#define XANM9B_MASK_CV              0x0001
#define XANM9B_MASK_CC              0x0002
#define XANM9B_MASK_OV              0x0008
#define XANM9B_MASK_UNR             0x0003
#define XANM9B_MASK_OT              0x0010
#define XANM9B_MASK_FOLD            0x0040

/* Family members single-output, single-range, triggered */
enum
{
   /* XFR 2.8kW Models */
   XFR7_5300,
   XFR12220,
   XFR20130,
   XFR3385,
   XFR4070,
   XFR5060,
   XFR6046,
   XFR8037,
   XFR10028,
   XFR15018,
   XFR3009,
   XFR4207,
   XFR6004,
   /* XFR 1.2kW Models */
   XFR6200,
   XFR7_5140,
   XFR12100,
   XFR2060,
   XFR3535,
   XFR4030,
   XFR4530,
   XFR6020,
   XFR10012,
   XFR1508,
   XFR1607_5,
   XFR3004,
   XFR6002,
   /* XHR 1kW Models */
   XHR7_5130,
   XHR2050,
   XHR3333,
   XHR4025,
   XHR6018,
   XHR10010,
   XHR1507,
   XHR3003_5,
   XHR6001_7,
   /* XHR 600W Models */
   XHR7_580,
   XHR2030,
   XHR3318,
   XHR4015,
   XHR6010,
   XHR1006,
   XHR1504,
   XHR3002,
   XHR6001,
   /* XPD 500W Models */
   XPD7_567,
   XPD1830,
   XPD3316,
   XPD609,
   XPD1204_5,
   /* XanHPD Models */
   HPD1520,
   HPD3010,
   HPD605,
   /* XanXT Models */
   XT76,
   XT154,
   XT203,
   XT302,
   XT601,
   XT1200_5,
   XT2500_25
   };

static IviStringValueTable XanM9B_ModelTable =
{
   /* XFR 2.8kW Models */
   {XFR7_5300, "XFR7.5-300"},
   {XFR12220,  "XFR12-220"},
   {XFR20130,  "XFR20-130"},
   {XFR3385,   "XFR33-85"},
   {XFR4070,   "XFR40-70"},
   {XFR5060,   "XFR50-60"},
   {XFR6046,   "XFR60-46"},
   {XFR8037,   "XFR80-37"},
   {XFR10028,  "XFR100-28"},
   {XFR15018,  "XFR150-18"},
   {XFR3009,   "XFR300-9"},
   {XFR4207,   "XFR420-7"},
   {XFR6004,   "XFR600-4"},
   /* XFR 1.2kW Models */
   {XFR6200,   "XFR6-200"},   
   {XFR7_5140, "XFR7.5-140"},
   {XFR12100,  "XFR12-100"},
   {XFR2060,   "XFR20-60"},
   {XFR3535,   "XFR35-35"},
   {XFR4030,   "XFR40-30"},
   {XFR4530,   "XFR45-30"},
   {XFR6020,   "XFR60-20"},
   {XFR10012,  "XFR100-12"},
   {XFR1508,   "XFR150-8"},
   {XFR1607_5, "XFR160-7.5"},
   {XFR3004,   "XFR300-4"},
   {XFR6002,   "XFR600-2"},
   /* XHR 1kW Models */
   {XHR7_5130, "XHR7.5-130"},
   {XHR2050,   "XHR20-50"},
   {XHR3333,   "XHR33-33"},
   {XHR4025,   "XHR40-25"},
   {XHR6018,   "XHR60-18"},
   {XHR10010,  "XHR100-10"},
   {XHR1507,   "XHR150-7"},
   {XHR3003_5, "XHR300-3.5"},
   {XHR6001_7, "XHR600-1.7"},
   /* XHR 600W Models */
   {XHR7_580,  "XHR7.5-80"},
   {XHR2030,   "XHR20-30"},
   {XHR3318,   "XHR33-18"},
   {XHR4015,   "XHR40-15"},
   {XHR6010,   "XHR60-10"},
   {XHR1006,   "XHR100-6"},
   {XHR1504,   "XHR150-4"},
   {XHR3002,   "XHR300-2"},
   {XHR6001,   "XHR600-1"},
   /* XPD 500W Models */
   {XPD7_567,  "XPD7.5-67"},
   {XPD1830,   "XPD18-30"},
   {XPD3316,   "XPD33-16"},
   {XPD609,    "XPD60-9"},
   {XPD1204_5, "XPD120-4.5"},
   /* XanHPD Models */
   {HPD1520,   "HPD15-20"},
   {HPD3010,   "HPD30-10"},
   {HPD605,    "HPD60-5"},
   /* XanXT Models */
   {XT76,      "XT7-6"},
   {XT154,     "XT15-4"},
   {XT203,     "XT20-3"},
   {XT302,     "XT30-2"},
   {XT601,     "XT60-1"},
   {XT1200_5,  "XT120-0.5"},
   {XT2500_25, "XT250-0.25"},
   {VI_NULL,   VI_NULL}
};

   
/*****************************************************************************
 *-------------- Utility Function Declarations (Non-Exported) ---------------*
 *****************************************************************************/
static ViStatus XanM9B_InitAttributes (ViSession vi,ViInt32 modelType);
static ViStatus XanM9B_DefaultInstrSetup (ViSession openInstrSession);
static ViStatus XanM9B_CheckStatus (ViSession vi);
static ViStatus XanM9B_GetDriverSetupOption (ViSession vi, ViInt32 *modelType);
static ViStatus XanM9B_WaitForOPC (ViSession vi, ViInt32 maxTime);
static ViStatus XanM9B_QueryID (ViSession vi, ViSession io, ViInt32 bufferSize,
                                 ViChar buffer[]);
static ViStatus XanM9B_GetModelFromBuffer(ViSession vi, ViConstString buffer,
                                 ViInt32 *modelType);    
static void RemoveSurroundingWhiteSpace(ViChar string[]);

/*****************************************************************************
 *----------------- Callback Declarations (Non-Exported) --------------------*
 *****************************************************************************/

    /*- Global Session Callbacks --------------------------------------------*/
    
static ViStatus _VI_FUNC XanM9B_CheckStatusCallback (ViSession vi, ViSession io);

    /*- Attribute callbacks -------------------------------------------------*/
static ViStatus _VI_FUNC XanM9BAttrDriverRevision_ReadCallback (ViSession vi,
                                                                ViSession io, 
                                                                ViConstString channelName,
                                                                ViAttr attributeId, 
                                                                const ViConstString cacheValue);
                                                                  
static ViStatus _VI_FUNC XanM9BAttrInstrumentManufacturer_ReadCallback (ViSession vi, 
                                                                        ViSession io,
                                                                        ViConstString channelName, 
                                                                        ViAttr attributeId,
                                                                        const ViConstString cacheValue);
                                                                        
static ViStatus _VI_FUNC XanM9BAttrInstrumentModel_ReadCallback (ViSession vi, 
                                                                 ViSession io,
                                                                 ViConstString channelName, 
                                                                 ViAttr attributeId,
                                                                 const ViConstString cacheValue);

static ViStatus _VI_FUNC XanM9BAttrInstrumentFirmwareRevision_ReadCallback (ViSession vi, 
                                                                            ViSession io,
                                                                            ViConstString channelName, 
                                                                            ViAttr attributeId,
                                                                            const ViConstString cacheValue);
                                                                    
static ViStatus _VI_FUNC XanM9BAttrOutputEnabled_WriteCallback (ViSession vi, 
                                                                ViSession io,
                                                                ViConstString channelName, 
                                                                ViAttr attributeId,
                                                                ViBoolean value);
                                                                 
static ViStatus _VI_FUNC XanM9BAttrOutputEnabled_ReadCallback (ViSession vi, 
                                                               ViSession io,
                                                               ViConstString channelName, 
                                                               ViAttr attributeId,
                                                               ViBoolean *value);
                                                                 
static ViStatus _VI_FUNC XanM9BAttrAuxaEnabled_WriteCallback (ViSession vi, 
                                                              ViSession io,
                                                              ViConstString channelName, 
                                                              ViAttr attributeId,
                                                              ViBoolean value);
                                                                 
static ViStatus _VI_FUNC XanM9BAttrAuxaEnabled_ReadCallback (ViSession vi, 
                                                             ViSession io,
                                                             ViConstString channelName, 
                                                             ViAttr attributeId,
                                                             ViBoolean *value);

static ViStatus _VI_FUNC XanM9BAttrAuxbEnabled_WriteCallback (ViSession vi, 
                                                              ViSession io,
                                                              ViConstString channelName, 
                                                              ViAttr attributeId,
                                                              ViBoolean value);
                                                                 
static ViStatus _VI_FUNC XanM9BAttrAuxbEnabled_ReadCallback (ViSession vi, 
                                                             ViSession io,
                                                             ViConstString channelName, 
                                                             ViAttr attributeId,
                                                             ViBoolean *value);

static ViStatus _VI_FUNC XanM9BAttrCmodeEnabled_WriteCallback (ViSession vi, 
                                                               ViSession io,
                                                               ViConstString channelName, 
                                                               ViAttr attributeId,
                                                               ViBoolean value);
                                                                 
static ViStatus _VI_FUNC XanM9BAttrCmodeEnabled_ReadCallback (ViSession vi, 
                                                              ViSession io,
                                                              ViConstString channelName, 
                                                              ViAttr attributeId,
                                                              ViBoolean *value);
                                                                                                                                
static ViStatus _VI_FUNC XanM9BAttrIdQueryResponse_ReadCallback (ViSession vi,
                                                                 ViSession io, 
                                                                 ViConstString channelName, 
                                                                 ViAttr attributeId, 
                                                                 const ViConstString cacheValue);

static ViStatus _VI_FUNC XanM9BAttrFaultDelay_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViReal64 *value);

static ViStatus _VI_FUNC XanM9BAttrFaultDelay_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 value);
                                                              
static ViStatus _VI_FUNC XanM9BAttrVoltageLevel_WriteCallback (ViSession vi, 
                                                               ViSession io,
                                                               ViConstString channelName, 
                                                               ViAttr attributeId,
                                                               ViReal64 value);
                                                                
static ViStatus _VI_FUNC XanM9BAttrVoltageLevel_ReadCallback (ViSession vi, 
                                                              ViSession io,
                                                              ViConstString channelName, 
                                                              ViAttr attributeId,
                                                              ViReal64* value);

static ViStatus _VI_FUNC XanM9BAttrOVPLimit_WriteCallback (ViSession vi, 
                                                           ViSession io,
                                                           ViConstString channelName, 
                                                           ViAttr attributeId,
                                                           ViReal64 value);
                                                            
static ViStatus _VI_FUNC XanM9BAttrOVPLimit_ReadCallback (ViSession vi, 
                                                          ViSession io,
                                                          ViConstString channelName, 
                                                          ViAttr attributeId,
                                                          ViReal64* value);
                                                
static ViStatus _VI_FUNC XanM9BAttrCurrentLimit_WriteCallback (ViSession vi, 
                                                               ViSession io,
                                                               ViConstString channelName, 
                                                               ViAttr attributeId,
                                                               ViReal64 value);
                                                                
static ViStatus _VI_FUNC XanM9BAttrCurrentLimit_ReadCallback (ViSession vi, 
                                                              ViSession io,
                                                              ViConstString channelName, 
                                                              ViAttr attributeId,
                                                              ViReal64* value);
                                                               
static ViStatus _VI_FUNC XanM9BAttrMaximumCurrentLimit_ReadCallback (ViSession vi,
                                                                     ViSession io,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViReal64 *value);

static ViStatus _VI_FUNC XanM9BAttrMaximumCurrentLimit_WriteCallback (ViSession vi,
                                                                      ViSession io,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViReal64 value);
                                                              
 static ViStatus _VI_FUNC XanM9BAttrMaximumVoltage_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 *value);

static ViStatus _VI_FUNC XanM9BAttrMaximumVoltage_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);
                                                              
static ViStatus _VI_FUNC XanM9BAttrTrigVoltageLevel_WriteCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   ViReal64 value);
                                                                    
static ViStatus _VI_FUNC XanM9BAttrTrigVoltageLevel_ReadCallback (ViSession vi, 
                                                                  ViSession io,
                                                                  ViConstString channelName, 
                                                                  ViAttr attributeId,
                                                                  ViReal64* value);
                                                                 
static ViStatus _VI_FUNC XanM9BAttrTrigCurrentLimit_WriteCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   ViReal64 value);
                                                                    
static ViStatus _VI_FUNC XanM9BAttrTrigCurrentLimit_ReadCallback (ViSession vi, 
                                                                  ViSession io,
                                                                  ViConstString channelName, 
                                                                  ViAttr attributeId,
                                                                  ViReal64* value);
                   
static ViStatus _VI_FUNC XanM9BAttrSrqEnabled_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViBoolean *value);

static ViStatus _VI_FUNC XanM9BAttrSrqEnabled_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViBoolean value);

static ViStatus _VI_FUNC XanM9BAttrFoldbackSetting_WriteCallback (ViSession vi, 
                                                                  ViSession io,
                                                                  ViConstString channelName,   
                                                                  ViAttr attributeId,
                                                                  ViInt32 value);
                                                                
static ViStatus _VI_FUNC XanM9BAttrFoldbackSetting_ReadCallback (ViSession vi, 
                                                                 ViSession io,
                                                                 ViConstString channelName, 
                                                                 ViAttr attributeId,
                                                                 ViInt32* value);

static ViStatus _VI_FUNC XanM9BAttrHoldEnabled_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViBoolean *value);

static ViStatus _VI_FUNC XanM9BAttrHoldEnabled_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViBoolean value);

static ViStatus _VI_FUNC XanM9BAttrUnmaskSetting_WriteCallback (ViSession vi, 
                                                                ViSession io,
                                                                ViConstString channelName,   
                                                                ViAttr attributeId,
                                                                ViConstString value);
                                                                
static ViStatus _VI_FUNC XanM9BAttrMaskSetting_WriteCallback (ViSession vi, 
                                                              ViSession io,
                                                              ViConstString channelName,   
                                                              ViAttr attributeId,
                                                              ViConstString value);

static ViStatus _VI_FUNC XanM9BAttrUnmaskCondition_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value);

static ViStatus _VI_FUNC XanM9BAttrInstrStatusReg_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value);

static ViStatus _VI_FUNC XanM9BAttrCurrentLimitBehavior_ReadCallback (ViSession vi,
                                                                      ViSession io,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViInt32 *value);

static ViStatus _VI_FUNC XanM9BAttrCurrentLimitBehavior_WriteCallback (ViSession vi,
                                                                       ViSession io,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViInt32 value);

static ViStatus _VI_FUNC XanM9BAttrOVPEnabled_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViBoolean *value);

static ViStatus _VI_FUNC XanM9BAttrOVPEnabled_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViBoolean value);

static ViStatus _VI_FUNC XanM9BAttrTriggerSource_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value);

static ViStatus _VI_FUNC XanM9BAttrTriggerSource_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value);

static ViStatus _VI_FUNC XanM9BAttrRemoteMode_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 *value);

static ViStatus _VI_FUNC XanM9BAttrRemoteMode_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 value);

static ViStatus XanM9B_QueryACCRegister(ViSession vi, ViConstString channelName,
                                        ViInt32 *reg);
                                    
static ViStatus XanM9B_QueryFaultRegister(ViSession vi, ViConstString channelName,
                                          ViInt32 *reg);
                                      
                                      
/*****************************************************************************
 *------------ User-Callable Functions (Exportable Functions) ---------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: XanM9B_init   
 * Purpose:  VXIplug&play required function.  Calls the   
 *           XanM9B_InitWithOptions function.   
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_init (ViRsrc resourceName, ViBoolean IDQuery,
                               ViBoolean resetDevice, ViSession *newVi)
{   
   ViStatus    error = VI_SUCCESS;

   if (newVi == VI_NULL)
      {
      Ivi_SetErrorInfo (VI_NULL, VI_FALSE, IVI_ERROR_INVALID_PARAMETER, 
                        VI_ERROR_PARAMETER4, "Null address for Instrument Handle");
      checkErr( IVI_ERROR_INVALID_PARAMETER);
      }

   checkErr( XanM9B_InitWithOptions (resourceName, IDQuery, resetDevice, 
                                     "", newVi));

Error:  
   return error;
}

/*****************************************************************************
 * Function: XanM9B_InitWithOptions                                       
 * Purpose:  This function creates a new IVI session and calls the 
 *           IviInit function.                                     
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_InitWithOptions (ViRsrc resourceName, ViBoolean IDQuery,
                                          ViBoolean resetDevice, ViString optionString, 
                                          ViSession *newVi)
{   
   ViStatus    error = VI_SUCCESS;
   ViSession   vi = VI_NULL;
   ViChar      newResourceName[IVI_MAX_MESSAGE_BUF_SIZE];
   ViChar      newOptionString[IVI_MAX_MESSAGE_BUF_SIZE];
   ViBoolean   isLogicalName;
    
   if (newVi == VI_NULL)
      {
      Ivi_SetErrorInfo (VI_NULL, VI_FALSE, IVI_ERROR_INVALID_PARAMETER, 
                        VI_ERROR_PARAMETER5, "Null address for Instrument Handle");
      checkErr( IVI_ERROR_INVALID_PARAMETER);
      }

   *newVi = VI_NULL;
    
   checkErr( Ivi_GetInfoFromResourceName (resourceName, optionString, newResourceName,
                                          newOptionString, &isLogicalName));
    
   /* create a new interchangeable driver */
   checkErr( Ivi_SpecificDriverNew ("XanM9B", newOptionString, &vi));  
   if (!isLogicalName)
    {
        ViInt32 oldFlag = 0;
    
        checkErr (Ivi_GetAttributeFlags (vi, IVI_ATTR_IO_RESOURCE_DESCRIPTOR, &oldFlag));
        checkErr (Ivi_SetAttributeFlags (vi, IVI_ATTR_IO_RESOURCE_DESCRIPTOR, oldFlag & 0xfffb | 0x0010));
        checkErr (Ivi_SetAttributeViString (vi, "", IVI_ATTR_IO_RESOURCE_DESCRIPTOR, 0, newResourceName));
    } 
   /* init the driver */
   checkErr( XanM9B_IviInit (newResourceName, IDQuery, resetDevice, vi)); 
    
   if (isLogicalName)
      {
      checkErr( Ivi_ApplyDefaultSetup (vi));
      }
   *newVi = vi;
    
Error:
   if (error < VI_SUCCESS) 
      Ivi_Dispose (vi);
        
   return error;
}

/*****************************************************************************
 * Function: XanM9B_IviInit                                                       
 * Purpose:  This function is called by XanM9B_InitWithOptions  
 *           or by an IVI class driver.  This function initializes the I/O 
 *           interface, optionally resets the device, optionally performs an
 *           ID query, and sends a default setup to the instrument.                
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_IviInit (ViRsrc resourceName, ViBoolean IDQuery,
                                  ViBoolean reset, ViSession vi)
{
   ViStatus    error = VI_SUCCESS;
   ViSession   io = VI_NULL;
   ViSession   rmSession = VI_NULL;
    
   ViChar      rdBuffer[BUFFER_SIZE];
   ViInt32     modelType = XANM9B_MODEL_NOT_FOUND;
   ViUInt16    interface = VI_INTF_GPIB;
   ViInt16     instrErr = 0;
   
   rdBuffer[0] = '\0';
   checkErr( XanM9B_GetDriverSetupOption (vi, &modelType));
   checkErr( Ivi_BuildChannelTable (vi, "1", VI_FALSE, VI_NULL));
    
   if (!Ivi_Simulating(vi))
      {
      /* Open instrument session */
      checkErr( Ivi_GetAttributeViSession (vi, VI_NULL, IVI_ATTR_VISA_RM_SESSION, 0,
                                           &rmSession)); 
      viCheckErr( viOpen (rmSession, resourceName, VI_NULL, 100, &io));
      /* io session owned by driver now */
       
      viCheckErr( viGetAttribute (io, VI_ATTR_INTF_TYPE, &interface));
      if (interface == VI_INTF_ASRL)  /* Serial Port */
         {
         viCheckErr( viSetAttribute (io, VI_ATTR_TERMCHAR, '\r'));
         viCheckErr( viSetAttribute (io, VI_ATTR_TERMCHAR_EN, VI_TRUE));
         viCheckErr( viSetAttribute (io, VI_ATTR_ASRL_END_IN, VI_ASRL_END_TERMCHAR));
         viCheckErr( viSetAttribute (io, VI_ATTR_ASRL_END_OUT, VI_ASRL_END_TERMCHAR));
         
         /* Delay for 75ms after each write to the instrument, if using RS232 */
         configureableDelay = 0.075;
         }
        
      checkErr( Ivi_SetAttributeViSession (vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, io));  

      /* Configure VISA Formatted I/O */
      viCheckErr( viSetAttribute (io, VI_ATTR_TMO_VALUE, 5000 ));
      viCheckErr( viSetBuf (io, VI_READ_BUF | VI_WRITE_BUF, 4000));
      viCheckErr( viSetAttribute (io, VI_ATTR_WR_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS));
      viCheckErr( viSetAttribute (io, VI_ATTR_RD_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS));
   
      /* Clear any existing errors on the instrument */
      viPrintf( io, "ERR?" );
      Delay( configureableDelay );
      viScanf( io, "ERR %hd", &instrErr );
      
      if (IDQuery && (modelType == XANM9B_MODEL_NOT_FOUND))
         {
         checkErr( XanM9B_QueryID( vi, io, BUFFER_SIZE, rdBuffer));
         checkErr( XanM9B_GetModelFromBuffer(vi, rdBuffer, &modelType));
         }
      }
         
   if (modelType == XANM9B_MODEL_NOT_FOUND)
      {  
      modelType = XFR12100;
      }
    
   /* Add attributes */
   checkErr( XanM9B_InitAttributes (vi, modelType));

   if( Ivi_Simulating(vi) )
      {
      checkErr( Ivi_SetAttributeViString( vi, VI_NULL,  XANM9B_ATTR_ID_QUERY_RESPONSE, 
                                          0, "ID XFR12-100 Version 1.14TR" ));
      }
    /*- Reset instrument ----------------------------------------------------*/
   if( ( reset ) && ( interface == VI_INTF_ASRL ) )
      {
      /* put the unit into remote mode, before resetting it */
      checkErr( XanM9B_SetRemoteMode( vi, XANM9B_VAL_REM ) );
      error = ( XanM9B_reset( vi ) );
      }
   else if( reset ) 
      {
      error = ( XanM9B_reset( vi ) );
      }
   else /*- Send Default Instrument Setup ---------------------------------*/  
      {
      error = ( XanM9B_DefaultInstrSetup( vi ) );
      }
      
   checkErr( error );
    
   /*- Identification Query ------------------------------------------------*/
   if (!Ivi_Simulating(vi) && IDQuery)                               
      {
      ViInt32 actModel = modelType;
      
      if (!rdBuffer[0])
         {  
         checkErr( Ivi_GetAttributeViString (vi, VI_NULL, XANM9B_ATTR_ID_QUERY_RESPONSE, 
                                             0, BUFFER_SIZE, rdBuffer));
         checkErr( XanM9B_GetModelFromBuffer(vi, rdBuffer, &actModel));
         }
      else
         {
         checkErr( Ivi_SetAttributeViString (vi, VI_NULL,  XANM9B_ATTR_ID_QUERY_RESPONSE, 
                                             IVI_VAL_DONT_MARK_AS_SET_BY_USER, rdBuffer));
         }

      if (modelType != actModel)
         {
         viCheckErr( VI_ERROR_FAIL_ID_QUERY);
         }
      }
      
    checkErr( XanM9B_CheckStatus (vi));

Error:
   if (error < VI_SUCCESS)
      {
      if (!Ivi_Simulating (vi) && io)
          viClose (io);
      }
   return error;
}

/*****************************************************************************
 * Function: XanM9B_close                                                           
 * Purpose:  This function closes the instrument.                            
 *
 *           Note:  This function must unlock the session before calling
 *           Ivi_Dispose.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_close(ViSession vi)
{
   ViStatus    error = VI_SUCCESS;
    
   checkErr( Ivi_LockSession (vi, VI_NULL));
    
   checkErr( XanM9B_IviClose (vi));

Error:    
   Ivi_UnlockSession (vi, VI_NULL);
   Ivi_Dispose (vi);  

   return error;
}

/*****************************************************************************
 * Function: XanM9B_IviClose                                                        
 * Purpose:  This function performs all of the drivers clean-up operations   
 *           except for closing the IVI session.  This function is called by 
 *           XanM9B_close or by an IVI class driver. 
 *
 *           Note:  This function must close the I/O session and set 
 *           IVI_ATTR_IO_SESSION to 0.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_IviClose (ViSession vi)
{
   ViStatus error = VI_SUCCESS;
   ViSession io = VI_NULL;

   /* Do not lock here.  The caller manages the lock. */

   checkErr( Ivi_GetAttributeViSession (vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, &io));

Error:
   Ivi_SetAttributeViSession (vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, VI_NULL);
   if(io)                                                      
      {
      viClose (io);
      }
   return error;   
}

/*****************************************************************************
 * Function: XanM9B_ConfigureOutputEnabled                                   
 * Purpose:  This function controls whether or not the signal the power supply
 *           generates is present at the output terminal.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_ConfigureOutputEnabled (ViSession vi,
                                                 ViConstString channelName,
                                                 ViBoolean enabled)
{
   return Ivi_SetAttributeViBoolean (vi, channelName, XANM9B_ATTR_OUTPUT_ENABLED,
                                     0, enabled);
}  

/*****************************************************************************
 * Function: XanM9B_ConfigureCmodeEnabled                                   
 * Purpose:  This function enables/disables calibration mode.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_ConfigureCmodeEnabled (ViSession vi,
                                                ViConstString channelName,
                                                ViBoolean enabled)
{
   return Ivi_SetAttributeViBoolean (vi, channelName, XANM9B_ATTR_CMODE_ENABLED,
                                     0, enabled);
}  

/*****************************************************************************
 * Function: XanM9B_ConfigureAuxaEnabled                                   
 * Purpose:  This function controls the AUXA output signal.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_ConfigureAuxaEnabled (ViSession vi,
                                               ViConstString channelName,
                                               ViBoolean enabled)
{
   return Ivi_SetAttributeViBoolean (vi, channelName, XANM9B_ATTR_AUXA_ENABLED,
                                     0, enabled);
}  

/*****************************************************************************
 * Function: XanM9B_ConfigureAuxaEnabled                                   
 * Purpose:  This function controls the AUXB output signal.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_ConfigureAuxbEnabled (ViSession vi,
                                               ViConstString channelName,
                                               ViBoolean enabled)
{
   return Ivi_SetAttributeViBoolean (vi, channelName, XANM9B_ATTR_AUXB_ENABLED,
                                     0, enabled);
}  

/******************************************************************************
 * Function:  XanM9B_ConfigureFoldbackSetting
 * Purpose:   This function disables the power supply output when the output 
 *            enters the fold condition
 ******************************************************************************/
ViStatus _VI_FUNC XanM9B_ConfigureFoldbackSetting (ViSession vi, 
                                                   ViConstString channelName,
                                                   ViInt32 value)
{
   ViStatus error = VI_SUCCESS;
    
   checkErr( Ivi_LockSession (vi, VI_NULL));
    
   if (Ivi_RangeChecking(vi))
      {
      viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL),
                                          2, "Incorrect Channel Name");
      }
  
   checkErr( Ivi_SetAttributeViInt32 (vi, channelName, XANM9B_ATTR_FOLDBACK_SETTING,
                                      0, value)); 
   
   checkErr( XanM9B_CheckStatus(vi));

Error:    
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}

/*****************************************************************************
 * Function: XanM9B_ConfigureCurrentLimit                                   
 * Purpose:  This function configures the level of the
 *           current limit.
 * NOTE:     The Current limit must be set before sending a voltage level to the
 *           power supply upon initialization.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_ConfigureCurrentLimit (ViSession vi,
                                                ViConstString channelName,
                                                ViInt32 behavior,
                                                ViReal64 limit)
{
   ViStatus error = VI_SUCCESS;
    
   checkErr( Ivi_LockSession (vi, VI_NULL));
    
   if (Ivi_RangeChecking(vi))
      {
      viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL), 2, "Channel Name");
      }
      
   viCheckParm( Ivi_SetAttributeViInt32 (vi, channelName,
                                         XANM9B_ATTR_CURRENT_LIMIT_BEHAVIOR,
                                         0, behavior), 3,
                                         "Behavior parameter invalid for XanM9B_ConfigureCurrentLimit.");
   
   viCheckParm( Ivi_SetAttributeViReal64 (vi, channelName, 
                                          XANM9B_ATTR_CURRENT_LIMIT,
                                          0, limit), 4,
                                          "Limit parameter out of instrument range for XanM9B_ConfigureCurrentLimit.");

   checkErr( XanM9B_CheckStatus(vi));

Error:    
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}

/*****************************************************************************
 * Function: XanM9B_ConfigureOVP                                   
 * Purpose:  This function configures the over-voltage protection limit
 *        
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_ConfigureOVP (ViSession vi,
                                       ViConstString channelName,
                                       ViBoolean enabled,
                                       ViReal64 limit)
{
   ViStatus error = VI_SUCCESS;
   
   checkErr( Ivi_LockSession (vi, VI_NULL));

   if (enabled == VI_FALSE)
      {
      Ivi_SetErrorInfo (vi, VI_FALSE, XANM9B_WARNING_OVP_CANNOT_BE_DISABLED,
                        VI_ERROR_PARAMETER3, 
                        "These instruments cannot disable their OVP.");
      
      viCheckWarn(XANM9B_WARNING_OVP_CANNOT_BE_DISABLED);
      }
      
   checkErr( Ivi_SetAttributeViBoolean (vi, channelName, 
                                        XANM9B_ATTR_OVP_ENABLED,
                                        0, VI_TRUE));
                                        
   viCheckParm( Ivi_SetAttributeViReal64 (vi, channelName, 
                                          XANM9B_ATTR_OVP_LIMIT,
                                          0, limit), 4, "Limit parameter is outside instrument's range for XanM9B_ConfigureOVP.");
        
   checkErr( XanM9B_CheckStatus (vi));

Error:    
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}

/*****************************************************************************
 * Function: XanM9B_ConfigureVoltageLevel                                   
 * Purpose:  This function configures the voltage level the power supply
 *           attempts to generate.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_ConfigureVoltageLevel (ViSession vi,
                                                ViConstString channelName,
                                                ViReal64 level)
{
   return Ivi_SetAttributeViReal64 (vi, channelName,XANM9B_ATTR_VOLTAGE_LEVEL,
                                    0, level);
}

/*****************************************************************************
 * Function: XanM9B_MaxCurrentLimit                  
 * Purpose:  This function sets the maximum programmable current limit
 *           
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_MaxCurrentLimit (ViSession vi,
                                          ViConstString channelName,
                                          ViReal64 CurrentLevel)
{
   return Ivi_SetAttributeViReal64 (vi,channelName,XANM9B_ATTR_MAXIMUM_CURRENT_LIMIT,
                                    0,CurrentLevel);
}                                     
 
/*****************************************************************************
 * Function: XanM9B_ConfigureOutputRange                                   
 * Purpose:  This function determines the output range in which the power
 *           supply operates.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_ConfigureOutputRange (ViSession vi,
                                               ViConstString channelName, 
                                               ViInt32 rangeType,
                                               ViReal64 range)
{
   ViStatus    error = VI_SUCCESS;
    
   checkErr( Ivi_LockSession(vi, VI_NULL));
    
   if (Ivi_RangeChecking(vi))
      {
      viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL), 2, "Incorrect Channel Name");
        
      if ((rangeType != XANM9B_VAL_RANGE_CURRENT) && (rangeType != XANM9B_VAL_RANGE_VOLTAGE))
           viCheckParm( IVI_ERROR_INVALID_VALUE, 3, "'Range Type' parameter is invalid for XanM9B_ConfigureOutputRange.");
      }

   if (rangeType == XANM9B_VAL_RANGE_CURRENT)
      {
      viCheckParm( Ivi_CheckAttributeViReal64(vi, channelName, XANM9B_ATTR_CURRENT_LIMIT, 0, range),
                                              4, "Invalid Current value for 'Range' parameter in XanM9B_ConfigureOutputRange.");
      }
   else
      {
      viCheckParm( Ivi_CheckAttributeViReal64(vi, channelName, XANM9B_ATTR_VOLTAGE_LEVEL, 0, range),
                                              4, "Invalid Voltage value for 'Range' parameter in XanM9B_ConfigureOutputRange.");
      }
    
   checkErr( XanM9B_CheckStatus (vi));

Error:    
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}

/*****************************************************************************
 * Function: XanM9B_MaxVoltage                 
 * Purpose:  This function sets the maximum programmable voltage limit
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_MaxVoltage (ViSession vi, ViConstString channelName,
                                     ViReal64 VoltageLevel)
{
   return Ivi_SetAttributeViReal64 (vi,channelName,XANM9B_ATTR_MAXIMUM_VOLTAGE,
                                    0,VoltageLevel);
}                                     
  
/*****************************************************************************
 * Function: XanM9B_ConfigureSrqEnabled                                   
 * Purpose:  This function sets the supply's ability to generate service requests
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_ConfigureSrqEnabled(ViSession vi, ViConstString channelName,
                                             ViBoolean value)
{
   return  (Ivi_SetAttributeViBoolean (vi,channelName,XANM9B_ATTR_SRQ_ENABLED,
                                         0,value));
}  
 
/*****************************************************************************
 * Function: XanM9B_ConfigureFaultDelay                                   
 * Purpose:  This function sets a time delay before reporting fault conditions
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_ConfigureFaultDelay (ViSession vi, ViConstString channelName,
                                              ViReal64 value)
{
   return  (Ivi_SetAttributeViReal64 (vi,channelName,XANM9B_ATTR_FAULT_DELAY,
                                      0, value));
}  

/*****************************************************************************
 * Function: XanM9B_ClearStatus                                   
 * Purpose:  This function will clears the accumulated status data structures.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_ClearStatus (ViSession vi)
{
   ViStatus error = VI_SUCCESS;

   checkErr( Ivi_LockSession(vi, VI_NULL));

   if (!Ivi_Simulating(vi))              
      {
      ViSession io = Ivi_IOSession(vi); 
      checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
      viCheckErr(viPrintf(io, "ASTS?"));
      Delay( configureableDelay );
      viCheckErr(viScanf (io, "ASTS %*hd"));
      }

   checkErr( XanM9B_CheckStatus (vi));

Error:
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}

/*****************************************************************************
 * Function: XanM9B_ResetOutputProtection                                   
 * Purpose:  This function resets the power supply after an  output protection.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_ResetOutputProtection (ViSession vi,
                                                ViConstString channelName)
{
   ViStatus error = VI_SUCCESS;
    
   checkErr( Ivi_LockSession(vi, VI_NULL));
      
   if (!Ivi_Simulating (vi))
      {
      ViSession io = Ivi_IOSession(vi);
        
      checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
        
      viCheckErr( viPrintf (io, "RST"));
      Delay( configureableDelay );
      }
    
   checkErr( XanM9B_CheckStatus (vi));
    
Error:    
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}

/*****************************************************************************
 * Function: XanM9B_ConfigureHoldEnabled                                   
 * Purpose:  This function controls whether the commands sent to the power supply
 *           are held until a trigger or generated.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_ConfigureHoldEnabled (ViSession vi,
                                               ViConstString channelName,
                                               ViBoolean enabled)
{
   return Ivi_SetAttributeViBoolean (vi, channelName, XANM9B_ATTR_HOLD_ENABLED,
                                     0, enabled);
}

/*****************************************************************************
 * Function: XanM9B_ConfigureTriggerSource                                   
 * Purpose:  This function configures the trigger source used by the power supply
 *        to implement the triggered setpoints.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_ConfigureTriggerSource (ViSession vi,
                                                 ViConstString channelName, 
                                                 ViInt32 source)
{
   ViStatus error = VI_SUCCESS;

   checkErr( Ivi_LockSession(vi, VI_NULL));
    
   Ivi_SetErrorInfo (vi, VI_FALSE, XANM9B_WARNING_FUNCTION_NOT_SUPPORTED, VI_SUCCESS,
                    "The XanM9B_ConfigureTriggerSource function is not Supported.");
    
   viCheckWarn(XANM9B_WARNING_FUNCTION_NOT_SUPPORTED);

Error:    
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}

/*****************************************************************************
 * Function: XanM9B_ConfigureTriggeredVoltageLevel                                   
 * Purpose:  This function configures the voltage level the power supply
 *           attempts to generate after it receives a trigger.
 * NOTE: This function should only be used when HOLD is enabled. 
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_ConfigureTriggeredVoltageLevel (ViSession vi,
                                                         ViConstString channelName,
                                                         ViReal64 level)
{
   return Ivi_SetAttributeViReal64 (vi, channelName,
                                    XANM9B_ATTR_TRIGGERED_VOLTAGE_LEVEL,
                                    0, level);
}

/*****************************************************************************
 * Function: XanM9B_ConfigureTriggeredCurrentLimit                                   
 * Purpose:  This function configures the current limit the power supply
 *           attempts to generate after it receives a trigger.
 * NOTE: This function should only be used when HOLD is enabled.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_ConfigureTriggeredCurrentLimit (ViSession vi,
                                                         ViConstString channelName,
                                                         ViReal64 limit)
{
   return Ivi_SetAttributeViReal64 (vi, channelName,
                                    XANM9B_ATTR_TRIGGERED_CURRENT_LIMIT,
                                    0, limit);
}

/*****************************************************************************
 * Function: XanM9B_MaskSetting                                   
 * Purpose:  This function sets the MASK settings for the power supply       
 *****************************************************************************/
static IviRangeTableEntry attrMaskSettingRangeTableEntries[] =
   {
      {XANM9B_VAL_MASK_CV, 0, 0, "CV", 0},
      {XANM9B_VAL_MASK_CC, 0, 0, "CC", 0},
      {XANM9B_VAL_MASK_OV, 0, 0, "OV", 0},
      {XANM9B_VAL_MASK_OT, 0, 0, "OT", 0},
      {XANM9B_VAL_MASK_SD, 0, 0, "SD", 0},
      {XANM9B_VAL_MASK_FOLD, 0, 0, "FOLD", 0},
      {XANM9B_VAL_MASK_ERR, 0, 0, "ERR", 0},
      {XANM9B_VAL_MASK_PON, 0, 0, "PON", 0},
      {XANM9B_VAL_MASK_REM, 0, 0, "REM", 0},
      {XANM9B_VAL_MASK_ACF, 0, 0, "ACF", 0},
      {XANM9B_VAL_MASK_OPF, 0, 0, "OPF", 0},
      {XANM9B_VAL_MASK_SNSP, 0, 0, "SNSP", 0},
      {XANM9B_VAL_MASK_NONE, 0, 0, "NONE", 0},
      {XANM9B_VAL_MASK_ALL, 0, 0, "ALL", 0},
      {IVI_RANGE_TABLE_LAST_ENTRY}
   };

static IviRangeTable attrMaskSettingRangeTable =
   {
      IVI_VAL_DISCRETE,
      VI_TRUE,
      VI_TRUE,
      VI_NULL,
      attrMaskSettingRangeTableEntries,
   };

ViStatus _VI_FUNC XanM9B_MaskSetting (ViSession vi, ViConstString channelName,
                                      ViChar mnemonics[])
{
   ViStatus    error = VI_SUCCESS;
   ViChar      settings[BUFFER_SIZE];
   ViChar      *mnemonicToken[12];
   ViInt16     i = 0;
   
   checkErr( Ivi_LockSession (vi, VI_NULL));
    
   if (Ivi_RangeChecking(vi))
      {
      viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL),
                                          2, "Incorrect Channel Name");
        
      strcpy(settings, mnemonics); /* copy mnemonics to preserve the original string */
      mnemonicToken[i] = strtok(settings, ",");
      do
         {        /* break mnemonics string apart so range checking can be done */
         i++;
         mnemonicToken[i] = strtok(NULL, ",");
         }
         while (mnemonicToken[i] != NULL);
        
      while (i > 0)
         {
         i--;
         viCheckParm(Ivi_GetViInt32EntryFromString (mnemonicToken[i], 
                                                    &attrMaskSettingRangeTable, 
                                                    VI_NULL, VI_NULL, VI_NULL, VI_NULL, 
                                                    VI_NULL), 3, 
                                                    "Invalid Mnemonic for XanM9B_MaskSetting");
         }                                            
      }
    
   checkErr( Ivi_SetAttributeViString (vi, channelName,
                                       XANM9B_ATTR_MASK_SETTING,
                                       0, mnemonics));

   checkErr( XanM9B_CheckStatus (vi));
   
Error:
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}

/*****************************************************************************
 * Function: XanM9B_UnmaskSetting                                   
 * Purpose:  This function sets the UNMASK settings for the power supply         
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_UnmaskSetting (ViSession vi,ViConstString channelName,
                                        ViChar mnemonics[])
{
   ViStatus    error = VI_SUCCESS;
   ViChar      settings[BUFFER_SIZE];
   ViChar      *mnemonicToken[12];
   ViInt16     i = 0;
   
   checkErr( Ivi_LockSession (vi, VI_NULL));
    
   if (Ivi_RangeChecking(vi))
      {
      viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL),
                                          2, "Incorrect Channel Name");
        
      strcpy(settings, mnemonics);  /* copy mnemonics to preserve the original string */
      mnemonicToken[i] = strtok(settings, ",");
      do
         {      /* break mnemonics string apart so range checking can be done */
         i++;
         mnemonicToken[i] = strtok(NULL, ",");
         }
         while (mnemonicToken[i] != NULL);
        
      while (i > 0)
         {
         i--;
         viCheckParm(Ivi_GetViInt32EntryFromString (mnemonicToken[i], 
                                                    &attrMaskSettingRangeTable, 
                                                    VI_NULL, VI_NULL, VI_NULL, VI_NULL, 
                                                    VI_NULL), 3, 
                                                    "Invalid Mnemonic for XanM9B_UnmaskSetting");
         }                                            
      }
    
   checkErr( Ivi_SetAttributeViString (vi, channelName,
                                       XANM9B_ATTR_UNMASK_SETTING,
                                       0, mnemonics));
                                       
   checkErr( XanM9B_CheckStatus (vi));                                        
                                       
Error:
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}

/*****************************************************************************
 * Function: XanM9B_QueryMaxCurrentLimit                  
 * Purpose:  This function returns the maximum programmable current limit
 *           that the power supply accepts.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_QueryMaxCurrentLimit (ViSession vi,
                                               ViConstString channelName,
                                               ViReal64 voltageLevel,
                                               ViReal64 *maxCurrentLimit)
{
   ViStatus          error = VI_SUCCESS;
   IviRangeTablePtr  currentLimitRangeTable = VI_NULL;
   
   checkErr( Ivi_LockSession(vi, VI_NULL)); 
   
   if (Ivi_RangeChecking(vi))
      {
      viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL), 2, "Incorrect Channel Name");
      viCheckParm( Ivi_CheckAttributeViReal64(vi, channelName, 
                                              XANM9B_ATTR_VOLTAGE_LEVEL, 0, 
                                              voltageLevel), 3, 
                                              "Voltage Level is beyond instrument range for XanM9B_QueryMaxCurrentLimit");
      }

   checkErr( Ivi_GetStoredRangeTablePtr (vi, XANM9B_ATTR_CURRENT_LIMIT, 
                                         &currentLimitRangeTable));
                                
   viCheckErr( Ivi_GetViReal64EntryFromIndex (0, currentLimitRangeTable, VI_NULL, 
                                              maxCurrentLimit, VI_NULL, VI_NULL, 
                                              VI_NULL));
                                     
   checkErr( XanM9B_CheckStatus (vi));                                   
   
Error:
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}

/*****************************************************************************
 * Function: XanM9B_QueryMaxVoltageLevel                  
 * Purpose:  This function returns the maximum programmable voltage level
 *           that the power supply accepts.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_QueryMaxVoltageLevel (ViSession vi,
                                               ViConstString channelName,
                                               ViReal64 currentLimit,
                                               ViReal64 *maxVoltageLevel)
{
   ViStatus          error = VI_SUCCESS;
   IviRangeTablePtr  voltageLevelRangeTable = VI_NULL;
   
   checkErr( Ivi_LockSession (vi, VI_NULL));
   
   if (Ivi_RangeChecking(vi))
      {
      viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL), 2, "Incorrect Channel Name");
      viCheckParm( Ivi_CheckAttributeViReal64(vi, channelName, 
                                              XANM9B_ATTR_CURRENT_LIMIT, 0, 
                                              currentLimit), 3, 
                                              "Current Limit is beyond instrument range for XanM9B_QueryMaxVoltageLevel");
      }
   
   checkErr( Ivi_GetStoredRangeTablePtr (vi, XANM9B_ATTR_VOLTAGE_LEVEL, 
                                         &voltageLevelRangeTable));
                                
   viCheckErr( Ivi_GetViReal64EntryFromIndex (0, voltageLevelRangeTable, VI_NULL, 
                                              maxVoltageLevel, VI_NULL, VI_NULL, 
                                              VI_NULL));
                                     
   checkErr( XanM9B_CheckStatus (vi));                                   

Error:
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}

/*****************************************************************************
 * Function: XanM9B_UnmaskCondition                                   
 * Purpose:  This function gets the UNMASK condition for the power supply        
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_UnmaskCondition (ViSession vi, ViConstString channelName,
                                          ViInt32 *condition)
{
   ViStatus error = VI_SUCCESS;
   
   checkErr( Ivi_LockSession (vi, VI_NULL));
   
   if (Ivi_RangeChecking(vi))
      {
      viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL), 2, "Incorrect Channel Name");
      }
        
   checkErr( Ivi_GetAttributeViInt32 (vi, channelName,
                                      XANM9B_ATTR_UNMASK_CONDITION,
                                      0, condition));
                                       
   checkErr( XanM9B_CheckStatus (vi));                                       
                                     
Error:
   Ivi_UnlockSession(vi, VI_NULL);
   return error;                                     
}

/*****************************************************************************
 * Function: XanM9B_Initiate                                   
 * Purpose:  The Voltage Level, and Current Limit setpoints
 *        are invalidated so that when the trigger event is received,
 *        the setpoints can be updated.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_Initiate (ViSession vi)
{
   ViStatus error = VI_SUCCESS;
   
   checkErr( Ivi_LockSession(vi, VI_NULL));
   
   IviDCPwr_InterchangeCheck(vi, "XanM9B_Initiate");
   
   checkErr( Ivi_InvalidateAttribute (vi, IVI_VAL_ALL_CHANNELS,
                                      XANM9B_ATTR_VOLTAGE_LEVEL));
                                       
   checkErr( Ivi_InvalidateAttribute (vi, IVI_VAL_ALL_CHANNELS,
                                      XANM9B_ATTR_CURRENT_LIMIT));
                                       
Error:
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}

/*****************************************************************************
 * Function: XanM9B_Abort                                   
 * Purpose:  This function causes the instrument to no longer wait for a
 *           trigger.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_Abort (ViSession vi)
{
   ViStatus error = VI_SUCCESS;
   ViReal64 voltage = 0;
   ViReal64 current = 0;
   
   checkErr( Ivi_LockSession(vi, VI_NULL));
   
   checkErr( Ivi_InvalidateAttribute (vi, IVI_VAL_ALL_CHANNELS,
                                      XANM9B_ATTR_TRIGGERED_VOLTAGE_LEVEL));
                                       
   checkErr( Ivi_InvalidateAttribute (vi, IVI_VAL_ALL_CHANNELS,
                                      XANM9B_ATTR_TRIGGERED_CURRENT_LIMIT));
    
   checkErr( Ivi_SetAttributeViBoolean (vi, "1", XANM9B_ATTR_HOLD_ENABLED,
                                        0, VI_FALSE));   /* Disable HOLD */
    
   checkErr( Ivi_GetAttributeViReal64 (vi, "1", XANM9B_ATTR_VOLTAGE_LEVEL, 0, &voltage));
   checkErr( Ivi_GetAttributeViReal64 (vi, "1", XANM9B_ATTR_CURRENT_LIMIT, 0, &current));
   
   if (!Ivi_Simulating (vi))
      {
      ViSession io = Ivi_IOSession(vi);
      checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
        
      viCheckErr( viPrintf (io, "VSET %lf", voltage)); /* Erases any trigger voltage setting held in instrument */
      Delay( configureableDelay );
      viCheckErr( viPrintf (io, "ISET %lf", current)); /* Erases any trigger current setting held in instrument */
      Delay( configureableDelay );
      }
   
   checkErr( XanM9B_CheckStatus (vi));

Error:    
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: XanM9B_SendSoftwareTrigger                                   
 * Purpose:  This function sends a trigger to enable all held commands. 
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_SendSoftwareTrigger (ViSession vi)
{
    
   ViStatus    error = VI_SUCCESS;
   ViReal64    voltage = 0;
   ViReal64    current = 0;
    
   checkErr( Ivi_LockSession(vi, VI_NULL));
    
   if (!Ivi_Simulating (vi))
      {
      ViSession io = Ivi_IOSession(vi);
        
      checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
        
      viCheckErr( viPrintf (io, "TRG"));
      Delay( configureableDelay );
      }
   else
      {
      checkErr(Ivi_GetAttributeViReal64(vi, "1", XANM9B_ATTR_TRIGGERED_VOLTAGE_LEVEL,
                                        0, &voltage));
   
      checkErr(Ivi_GetAttributeViReal64(vi, "1", XANM9B_ATTR_TRIGGERED_CURRENT_LIMIT,
                                        0, &current));                                  
   
      checkErr(Ivi_SetAttributeViReal64(vi, "1", XANM9B_ATTR_VOLTAGE_LEVEL,
                                        0, voltage));
   
      checkErr(Ivi_SetAttributeViReal64(vi, "1", XANM9B_ATTR_CURRENT_LIMIT,
                                        0, current));                 
      }
      
   checkErr( XanM9B_CheckStatus (vi));
      
Error:    
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}               

/****************************************************************************
 * Function: XanM9B_Measure
 * Purpose:  This function measures either the current or voltage that the
 *           power supply is generating.
 *****************************************************************************/
static IviRangeTableEntry measurementTypeRangeTableEntries[] = 
   {   
      {XANM9B_VAL_MEASURE_CURRENT, 0, 0, "IOUT?", XANM9B_ATTR_CURRENT_LIMIT},
      {XANM9B_VAL_MEASURE_VOLTAGE, 0, 0, "VOUT?", XANM9B_ATTR_VOLTAGE_LEVEL},
      {IVI_RANGE_TABLE_LAST_ENTRY}
   };
static IviRangeTable measurementTypeRangeTable = 
   {
      IVI_VAL_DISCRETE,
      VI_FALSE,
      VI_FALSE,
      VI_NULL,
      measurementTypeRangeTableEntries
   };

ViStatus _VI_FUNC XanM9B_Measure (ViSession vi,
                                  ViConstString channelName,
                                  ViInt32 measurementType,
                                  ViReal64 *measurement)
{
                               
   ViStatus    error = VI_SUCCESS;
   ViReal64    value = 0.0;
   ViAttr      attrId;
   ViString    cmd = 0;
    
   checkErr( Ivi_LockSession(vi, VI_NULL));
    
   IviDCPwr_InterchangeCheck(vi, "XanM9B_Measure");
    
   viCheckParm( Ivi_GetViInt32EntryFromValue (measurementType,
                                              &measurementTypeRangeTable, 
                                              VI_NULL, VI_NULL, VI_NULL,
                                              VI_NULL, &cmd, &attrId), 3, "Measurement Type invalid for XanM9B_Measure.");
   if (!Ivi_Simulating (vi))
      {
      ViSession io = Ivi_IOSession(vi);
        
      checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
      viCheckErr(viPrintf(io, "%s",cmd));
      Delay( configureableDelay );
         if (measurementType == 1)
            {
            viCheckErr(viScanf(io,"VOUT %lf",&value));
            }
         else
            {
            viCheckErr(viScanf(io,"IOUT %lf",&value));  
            }
      }
   else
      { 
      checkErr( Ivi_GetAttributeViReal64 (vi, channelName, attrId, 0, &value));
      value *= 0.95; 
      }
       
   checkErr( XanM9B_CheckStatus (vi));

Error:    
   if (measurement) *measurement = value;
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
            
}

/*****************************************************************************
 * Function: XanM9B_QueryOutputState                                   
 * Purpose:  This function returns a value indicating if the power supply
 *           is in a particular output state.
 *****************************************************************************/
static IviRangeTableEntry outputStateRangeTableEntries[] =
   {
      {XANM9B_VAL_OUTPUT_CONSTANT_VOLTAGE, 0, 0, "", XANM9B_MASK_CV},
      {XANM9B_VAL_OUTPUT_CONSTANT_CURRENT, 0, 0, "", XANM9B_MASK_CC},
      {XANM9B_VAL_OUTPUT_OVER_VOLTAGE,     0, 0, "", XANM9B_MASK_OV},
      {XANM9B_VAL_OUTPUT_UNREGULATED,      0, 0, "", XANM9B_MASK_UNR},
      {XANM9B_VAL_OUTPUT_OVER_TEMPERATURE, 0, 0, "", XANM9B_MASK_OT},
      {XANM9B_VAL_OUTPUT_FOLD_TRIPPED,     0, 0, "", XANM9B_MASK_FOLD},
      {IVI_RANGE_TABLE_LAST_ENTRY}
   };
static IviRangeTable outputStateRangeTable =
   {
      IVI_VAL_DISCRETE,
      VI_FALSE,
      VI_FALSE,
      VI_NULL,
      outputStateRangeTableEntries,
   };
ViStatus _VI_FUNC XanM9B_QueryOutputState (ViSession vi,
                                          ViConstString channelName, 
                                          ViInt32 outputState,
                                          ViBoolean *inState)
{
   ViStatus    error = VI_SUCCESS;
   ViInt32     mask, reg;
   ViBoolean   value = VI_FALSE;

   checkErr( Ivi_LockSession(vi, VI_NULL));

   if (inState == VI_NULL)
       viCheckParm (IVI_ERROR_INVALID_PARAMETER, 2, 
       "Null value for 'In State' parameter of XanM9B_QueryOutputState.");

   if (Ivi_RangeChecking(vi))
      {
      viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL),
                                          2, "Incorrect Channel Name");
      }

   viCheckParm( Ivi_GetViInt32EntryFromValue (outputState,
                                              &outputStateRangeTable,
                                              VI_NULL, VI_NULL, VI_NULL, 
                                              VI_NULL, VI_NULL, &mask),
                                              3, "'Output State' parameter of XanM9B_QueryOutputState");

   checkErr( Ivi_GetAttributeViInt32(vi, channelName, XANM9B_ATTR_INSTR_STATUS_REG, 0, &reg));
    
   reg &= mask;
   value = (reg != 0);
   if (mask == XANM9B_MASK_UNR)
      value = (value == 0);
    
   checkErr( XanM9B_CheckStatus (vi));

Error:    
   if (inState) *inState = value;
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}

/*****************************************************************************
 * Function: XanM9B_QueryFaultState                                   
 * Purpose:  This function returns fault register
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_QueryFaultState (ViSession vi,
                                          ViConstString channelName,
                                          ViInt32 *faultRegister)
{
   return XanM9B_QueryFaultRegister( vi, channelName, faultRegister);
}

/*****************************************************************************
 * Function: XanM9B_QuerySetpoint                                   
 * Purpose:  This function returns the setpoint value for a particular
 *           output parameter. ie. Voltage, Current, OVP, Voltage Soft Limit,
 *           Current Soft Limit, and the Fault Delay setting (seconds)
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_QuerySetpoint (ViSession vi, ViConstString channelName,
                                        ViAttr attrId, ViReal64 *setpointLevel)
{
   ViStatus error = VI_SUCCESS;
   ViReal64 value = 0.0;
    
   checkErr( Ivi_LockSession(vi, VI_NULL)); 
   checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
   checkErr( Ivi_GetAttributeViReal64 (vi, channelName, attrId, 0, &value));
   checkErr( XanM9B_CheckStatus (vi));
   
Error:
   if (setpointLevel) 
      *setpointLevel = value;
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}
 
/*****************************************************************************
 * Function: XanM9B_QueryMode                                   
 * Purpose:  This function returns the mode value for a particular
 *           setting. The valid modes are Output: Disabled/Enabled, 
 *           AuxA: Disabled/Enabled, AuxB: Disabled/Enabled,
 *           Fold: Disabled/CV/CC, Hold: Disabled/Enabled
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_QueryMode (ViSession vi, ViConstString channelName,
                                    ViAttr attrId, ViInt32 *modeValue)
{
   ViStatus error = VI_SUCCESS;
   ViInt32     value = 0;
    
   checkErr( Ivi_LockSession(vi, VI_NULL)); 
   checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
   
   if (attrId == XANM9B_ATTR_FOLDBACK_SETTING)
      checkErr( Ivi_GetAttributeViInt32 (vi, channelName, attrId, 0, &value));
      
   else 
      checkErr( Ivi_GetAttributeViBoolean (vi, channelName, attrId, 0, (ViBoolean *) (&value)));
      
   checkErr( XanM9B_CheckStatus (vi));

Error:
   if (modeValue) 
      *modeValue = value;
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}
 
/*****************************************************************************
 * Function: XanM9B_QueryAccumulatedRegister                                   
 * Purpose:  This function returns accumulated status register
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_QueryAccumulatedState (ViSession vi,
                                                ViConstString channelName,
                                                ViInt32 *accRegister)
{
    return XanM9B_QueryACCRegister( vi, channelName,accRegister);
}

/*****************************************************************************
 * Function: WriteInstrData and ReadInstrData Functions                      
 * Purpose:  These functions enable the instrument driver user to  
 *           write and read commands directly to and from the instrument.            
 *                                                                           
 *           Note:  These functions bypass the IVI attribute state caching.  
 *                  WriteInstrData invalidates the cached values for all 
 *                  attributes.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_WriteInstrData (ViSession vi, ViConstString writeBuffer)   
{   
   return Ivi_WriteInstrData (vi, writeBuffer);    
}   

ViStatus _VI_FUNC XanM9B_ReadInstrData (ViSession vi, ViInt32 numBytes, 
                                          ViChar rdBuf[], ViInt32 *bytesRead)  
{   
   return Ivi_ReadInstrData (vi, numBytes, rdBuf, bytesRead);   
} 

/*****************************************************************************
 * Function: XanM9B_GetAttribute<type> Functions                                    
 * Purpose:  These functions enable the instrument driver user to get 
 *           attribute values directly.  There are typesafe versions for 
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession attributes.                                         
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_GetAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViInt32 *value)
{                                                                                                           
   return Ivi_GetAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                   value);
}                                                                                                           
ViStatus _VI_FUNC XanM9B_GetAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                               ViAttr attributeId, ViReal64 *value)
{                                                                                                           
   return Ivi_GetAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                    value);
}                                                                                                           
ViStatus _VI_FUNC XanM9B_GetAttributeViString (ViSession vi, ViConstString channelName, 
                                               ViAttr attributeId, ViInt32 bufSize, 
                                               ViChar value[]) 
{   
   return Ivi_GetAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                    bufSize, value);
}   
ViStatus _VI_FUNC XanM9B_GetAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViBoolean *value)
{                                                                                                           
   return Ivi_GetAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}                                                                                                           
ViStatus _VI_FUNC XanM9B_GetAttributeViSession (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViSession *value)
{                                                                                                           
    return Ivi_GetAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           

/*****************************************************************************
 * Function: XanM9B_SetAttribute<type> Functions                                    
 * Purpose:  These functions enable the instrument driver user to set 
 *           attribute values directly.  There are typesafe versions for 
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession datatypes.                                         
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_SetAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                              ViAttr attributeId, ViInt32 value)
{                                                                                                           
   return Ivi_SetAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                   value);
}                                                                                                           
ViStatus _VI_FUNC XanM9B_SetAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                               ViAttr attributeId, ViReal64 value)
{                                                                                                           
   return Ivi_SetAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                    value);
}                                                                                                           
ViStatus _VI_FUNC XanM9B_SetAttributeViString (ViSession vi, ViConstString channelName, 
                                               ViAttr attributeId, ViConstString value) 
{   
   return Ivi_SetAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                    value);
}   
ViStatus _VI_FUNC XanM9B_SetAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViBoolean value)
{                                                                                                           
   return Ivi_SetAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}                                                                                                           
ViStatus _VI_FUNC XanM9B_SetAttributeViSession (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViSession value)
{                                                                                                           
   return Ivi_SetAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}                                                                                                           

/*****************************************************************************
 * Function: XanM9B_CheckAttribute<type> Functions                                  
 * Purpose:  These functions enable the instrument driver user to check  
 *           attribute values directly.  These functions check the value you
 *           specify even if you set the XANM9B_ATTR_RANGE_CHECK 
 *           attribute to VI_FALSE.  There are typesafe versions for ViInt32, 
 *           ViReal64, ViString, ViBoolean, and ViSession datatypes.                         
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_CheckAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViInt32 value)
{                                                                                                           
   return Ivi_CheckAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}
ViStatus _VI_FUNC XanM9B_CheckAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViReal64 value)
{                                                                                                           
   return Ivi_CheckAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           
ViStatus _VI_FUNC XanM9B_CheckAttributeViString (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViConstString value)  
{   
   return Ivi_CheckAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}   
ViStatus _VI_FUNC XanM9B_CheckAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViBoolean value)
{                                                                                                           
   return Ivi_CheckAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                       value);
}                                                                                                           
ViStatus _VI_FUNC XanM9B_CheckAttributeViSession (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViSession value)
{                                                                                                           
   return Ivi_CheckAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                       value);
}                                                                                                           

/*****************************************************************************
 * Function: XanM9B_self_test                                  
 * Purpose:  This function causes the power supply to run a self diagnostic test.
 * Note:  This function is not supported and returns a warning.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_self_test (ViSession vi, ViInt16 *testResult,
                                    ViChar testMessage[])
{
   ViStatus    error = VI_SUCCESS;

   checkErr( Ivi_LockSession (vi, VI_NULL));
   if (testResult == VI_NULL)
       viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for 'Test Result' parameter in XanM9B_self_test");
   if (testMessage == VI_NULL)
       viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for 'Test Message' parameter in XanM9B_self_test");

   strcpy(testMessage, "Self-Test is not supported by these instruments.");
      viCheckWarn( VI_WARN_NSUP_SELF_TEST);

Error:
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}

/*****************************************************************************
 * Function: XanM9B_reset                                                         
 * Purpose:  This function resets the instrument.                          
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_reset (ViSession vi)
{
   ViStatus    error = VI_SUCCESS;

   checkErr( Ivi_LockSession (vi, VI_NULL));

   if (!Ivi_Simulating(vi))                /* call only when locked */
      {
      ViSession   io = Ivi_IOSession(vi); /* call only when locked */
       
      checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
      viCheckErr( viPrintf (io, "CLR"));
      Delay( configureableDelay );
      }
       
   checkErr( Ivi_InvalidateAttribute (vi, IVI_VAL_ALL_CHANNELS,
                                      XANM9B_ATTR_VOLTAGE_LEVEL));
   checkErr( Ivi_InvalidateAttribute (vi, IVI_VAL_ALL_CHANNELS,
                                      XANM9B_ATTR_CURRENT_LIMIT));
   checkErr( Ivi_InvalidateAttribute (vi, IVI_VAL_ALL_CHANNELS,
                                      XANM9B_ATTR_OVP_LIMIT));
                                      
   checkErr( XanM9B_DefaultInstrSetup (vi));                                
   
   checkErr( XanM9B_CheckStatus (vi));                                      

Error:
   Ivi_UnlockSession (vi, VI_NULL);
   return error;
}

/*****************************************************************************
 * Function: XanM9B_SetRemoteMode
 * Purpose:  This function sets the power supply into remote or local mode
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_SetRemoteMode( ViSession vi, ViInt32 state )
{
   ViStatus error = VI_SUCCESS;
   
   checkErr( Ivi_LockSession( vi, VI_NULL ) );
   
   checkErr( Ivi_SetAttributeViInt32( vi, VI_NULL, XANM9B_ATTR_REMOTE_MODE,
                                      0, state ) );
                                      
   checkErr( XanM9B_CheckStatus( vi ) );                                       
                                       
Error:
   Ivi_UnlockSession( vi, VI_NULL );
   return( error );
}

/*****************************************************************************
 * Function: XanM9B_InvalidateAllAttributes
 * Purpose:  This function invalidates the cached value of all attributes.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_InvalidateAllAttributes (ViSession vi)
{
   return Ivi_InvalidateAllAttributes(vi);
}

/**************************************************************************** 
 *  Function: XanM9B_GetChannelName
 *  Purpose:  This function returns the name of a channel based on a 1-based
 *            index.
 ****************************************************************************/
/**************************************************************************** 
 *  Function: XanM9B_GetChannelName
 *  Purpose:  This function returns the highest-level channel name that
 *            corresponds to the specific driver channel string that is in
 *            the channel table at an index you specify.
 ****************************************************************************/
ViStatus _VI_FUNC XanM9B_GetChannelName (ViSession vi, ViInt32 index,
                                         ViInt32 bufferSize,
                                         ViChar name[])
{
   ViStatus          error 			= VI_SUCCESS;
   ViConstString  	channelName 	= {0};
   ViInt32        	actualSize 		= 0;

   checkErr( Ivi_LockSession (vi, VI_NULL));
    
   if (bufferSize != 0)
      {  
      if (name == VI_NULL)
         {
         viCheckParm( XANM9B_ERROR_INVALID_PARAMETER, 4, "Null address for 'Channel Name' parameter in XanM9B_GetChannelName.");
         }
      }
    
   /* Get channel name */
   viCheckParm( Ivi_GetNthChannelString (vi, index, &channelName), 2, "Index");
    
   if ( channelName == NULL )
   {
	   error = -1;
	   goto Error;
   }
   

	actualSize = strlen(channelName) + 1;
    
   /* Prepare return values */
   if (bufferSize == 0)
      {
      error = actualSize;
      }
   else 
      {
      if (bufferSize > 0)
         {
         if (actualSize > bufferSize)
            {
            error = actualSize;
            actualSize = bufferSize;
            }
         }
      memcpy(name, channelName, actualSize-1);
      name[actualSize-1] = '\0';
      }

Error:    
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}

/*****************************************************************************
 * Function: XanM9B_error_query                                                     
 * Purpose:  This function queries the instrument error queue and returns   
 *           the result.                                                     
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_error_query (ViSession vi,ViInt32 *errCode, 
                                      ViChar errorMessage[])
{
   
   ViStatus    error = VI_SUCCESS;
   
   checkErr( Ivi_LockSession (vi, VI_NULL));     

   if( errCode == VI_NULL )
   {   
      viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, 
                   "Null value for Error Code parameter." );
   }
   
   if( errorMessage == VI_NULL )
   {   
      viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, 
                   "Null value for Error Message parameter." );
   }
   
   if (!Ivi_Simulating(vi))               /* call only when locked */
      {
      ViSession   io = Ivi_IOSession(vi); /* call only when locked */
      checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
      
      viCheckErr( viPrintf(io, "ERR?"));
      Delay( configureableDelay );
      viCheckErr(viScanf(io, "ERR %ld",errCode));
      
      if (*errCode == 0) 
         {
            strcpy( errorMessage, "No Errors" );
         }
      else if (*errCode == 4) 
         {
            strcpy( errorMessage, "Unrecognized Character or String, or Syntax Error" );
         }
      else if (*errCode == 5) 
         {
            strcpy( errorMessage, "Number Out of Range" );
         }
      else if (*errCode == 6) 
         {
            strcpy( errorMessage, "Attempt to exceed Soft Limits" );
         }
      else if (*errCode == 7) 
         {
            strcpy( errorMessage, "Improper Soft Limit" );
         }
      else if (*errCode == 8) 
         {
            strcpy( errorMessage, "Data Requested without a Query Being Sent" );
         }
      else if (*errCode == 9) 
         {
            strcpy( errorMessage, "OVP Set Below Output" );
         }
      else if (*errCode == 10) 
         {
            strcpy( errorMessage, "Slave Processor Not Responding" );
         }
      else if (*errCode == 12) 
         {
            strcpy( errorMessage, "Illegal Calibration" );
         }
      else 
         {
            strcpy( errorMessage, "Unknown Error Code" );
         }
      }
   else
      {
          /* Simulate Error Query */
      *errCode = 0;
      strcpy( errorMessage, "No Errors" );
      }
      
Error:
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}

/*****************************************************************************
 * Function: XanM9B_error_message                                                  
 * Purpose:  This function translates the error codes returned by this       
 *           instrument driver into user-readable strings.  
 *       
 *           Note:  The caller can pass VI_NULL for the vi parameter.  This 
 *           is useful if one of the init functions fail.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_error_message (ViSession vi, ViStatus errorCode,
                                        ViChar errorMessage[256])
{
   ViStatus    error = VI_SUCCESS;
    
   static IviStringValueTable errorTable = 
      {
      IVIDCPWR_ERROR_CODES_AND_MSGS,
      {VI_NULL,                               VI_NULL},
      };
        
   if (vi)
      Ivi_LockSession(vi, VI_NULL);

      /* all VISA and IVI error codes are handled as well as codes in the table */
   if (errorMessage == VI_NULL)
      viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, 
                   "Null address for 'Error Message' parameter in XanM9B_error_message.");

   checkErr( Ivi_GetSpecificDriverStatusDesc(vi, errorCode, errorMessage, errorTable)); 
    
Error:
   if (vi)
      Ivi_UnlockSession(vi, VI_NULL);
   return error;
}

/*****************************************************************************
 * Function: XanM9B_ResetWithDefaults
 * Purpose:  This function resets the instrument and applies default settings
 *           from the IVI Configuration Store based on the logical name
 *           from which the session was created.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_ResetWithDefaults (ViSession vi)
{
   ViStatus error = VI_SUCCESS;
    
   checkErr( Ivi_LockSession (vi, VI_NULL));
   checkErr( XanM9B_reset(vi));
   checkErr( Ivi_ApplyDefaultSetup(vi));
   
Error:
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}

/**************************************************************************** 
 *  Function: XanM9B_Disable
 *  Purpose:  This function places the instrument in a quiescent state as 
 *            quickly as possible.
 ****************************************************************************/
ViStatus _VI_FUNC XanM9B_Disable (ViSession vi)
{
   ViStatus error = VI_SUCCESS;
    
   checkErr( Ivi_LockSession (vi, VI_NULL));
   if (!Ivi_Simulating(vi)) 
      {
      ViSession   io = Ivi_IOSession(vi); /* call only when locked */
       
      checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
      viCheckErr( viPrintf (io, "CLR"));
      Delay( configureableDelay );
      }

Error:
   Ivi_UnlockSession (vi, VI_NULL);
   return error;
}

/*****************************************************************************
 * Function: XanM9B_revision_query                                                  
 * Purpose:  This function returns the driver and instrument revisions.      
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_revision_query (ViSession vi, ViChar driverRev[], 
                                         ViChar instrRev[])
{
   ViStatus    error = VI_SUCCESS;
    
   checkErr( Ivi_LockSession (vi, VI_NULL));

   if (driverRev == VI_NULL)
      {
      viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, 
                   "Null address for 'Driver Revision' parameter in XanM9B_revision_query.");
      }
    
   if (instrRev == VI_NULL)
      {
      viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, 
                   "Null address for 'Instrument Revision' parameter in XanM9B_revision_query.");
      }                    

   checkErr( Ivi_GetAttributeViString (vi, VI_NULL, XANM9B_ATTR_SPECIFIC_DRIVER_REVISION, 
                                       0, 256, driverRev));  
   checkErr( Ivi_GetAttributeViString (vi, "", XANM9B_ATTR_INSTRUMENT_FIRMWARE_REVISION, 
                                       0, 256, instrRev));
   checkErr( XanM9B_CheckStatus (vi));

Error:    
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}

/*****************************************************************************
 * Function: XanM9B_GetNextCoercionRecord                        
 * Purpose:  This function enables the instrument driver user to obtain
 *           the coercion information associated with the IVI session.                                                              
 *           This function retrieves and clears the oldest instance in which 
 *           the instrument driver coerced a value the instrument driver user
 *           specified to another value.                     
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_GetNextCoercionRecord (ViSession vi,
                                                ViInt32 bufferSize,
                                                ViChar  record[])
{
   return Ivi_GetNextCoercionString (vi, bufferSize, record);
}

/**************************************************************************** 
 *  Function: XanM9B_GetNextInterchangeWarning,
 *            XanM9B_ResetInterchangeCheck, and
 *            XanM9B_ClearInterchangeWarnings
 *  Purpose:  These functions allow the user to retrieve interchangeability
 *            warnings, reset the driver's interchangeability checking 
 *            state, and clear all previously logged interchangeability warnings.
 ****************************************************************************/
ViStatus _VI_FUNC XanM9B_GetNextInterchangeWarning (ViSession vi,
                                                    ViInt32 bufferSize,
                                                    ViChar warnString[])
{
   return Ivi_GetNextInterchangeCheckString (vi, bufferSize, warnString);
}

ViStatus _VI_FUNC XanM9B_ClearInterchangeWarnings (ViSession vi)
{
   return Ivi_ClearInterchangeWarnings (vi);
}

ViStatus _VI_FUNC XanM9B_ResetInterchangeCheck (ViSession vi)
{
   return Ivi_ResetInterchangeCheck (vi);
}

/*****************************************************************************
 * Function: XanM9B_LockSession and XanM9B_UnlockSession Functions                        
 * Purpose:  These functions enable the instrument driver user to lock the 
 *           session around a sequence of driver calls during which other
 *           execution threads must not disturb the instrument state.
 *                                                                          
 *           NOTE:  The callerHasLock parameter must be a local variable 
 *           initialized to VI_FALSE and passed by reference, or you can pass 
 *           VI_NULL.                     
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_LockSession (ViSession vi, ViBoolean *callerHasLock)  
{                                              
   return Ivi_LockSession(vi,callerHasLock);      
}                                              

ViStatus _VI_FUNC XanM9B_UnlockSession (ViSession vi, ViBoolean *callerHasLock) 
{                                              
   return Ivi_UnlockSession(vi,callerHasLock);    
}   

/*****************************************************************************
 * Function: XanM9B_GetError and XanM9B_ClearError Functions                       
 * Purpose:  These functions enable the instrument driver user to  
 *           get or clear the error information the driver associates with the
 *           IVI session.                                                        
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_GetError (ViSession vi, ViStatus *errorCode,
                                   ViInt32 bufferSize,
                                   ViChar description[])
{
   ViStatus    error = VI_SUCCESS;
   ViStatus    primary = VI_SUCCESS,
               secondary = VI_SUCCESS;
   ViChar      elaboration[256] = "";
   ViChar      errorMessage[1024] = "";
   ViChar      *appendPoint = errorMessage;
   ViInt32     actualSize = 0;
   ViBoolean   locked = VI_FALSE;

    /* Lock Session */      
   if (vi != 0)
      {
      checkErr( Ivi_LockSession(vi, &locked));
      }

   /* Test for nulls and acquire error data */
   if (bufferSize != 0)
      {
      if (errorCode == VI_NULL)
         {
         viCheckParm( XANM9B_ERROR_INVALID_PARAMETER, 3, 
                      "Null address for 'Error Code' parameter for XanM9B_GetError.");
         }
         
      if (description == VI_NULL)
         {
         viCheckParm( XANM9B_ERROR_INVALID_PARAMETER, 4, 
                      "Null address for 'Description' parameter for XanM9B_GetError.");
         }
      
      checkErr( Ivi_GetErrorInfo (vi, &primary, &secondary, elaboration));
      }
   else
      {
      checkErr( Ivi_GetAttributeViString(vi, VI_NULL, IVI_ATTR_ERROR_ELABORATION, 0, 256, elaboration));
      checkErr( Ivi_GetAttributeViInt32(vi, VI_NULL, IVI_ATTR_SECONDARY_ERROR, 0, &secondary));
      checkErr( Ivi_GetAttributeViInt32(vi, VI_NULL, IVI_ATTR_PRIMARY_ERROR, 0, &primary));
      }
      
   /* Format data */
   if (primary != VI_SUCCESS)
      {
      ViChar msg[256] = "";
      checkErr( XanM9B_error_message (vi, primary, msg));
      appendPoint += sprintf(appendPoint, "Primary Error: (Hex 0x%08X) %s", primary, msg);
      }
   
   if (secondary != VI_SUCCESS)
      {
      ViChar msg[256] = "";
      checkErr( XanM9B_error_message (vi, secondary, msg));
      appendPoint += sprintf(appendPoint, "\nSecondary Error: (Hex 0x%08X) %s", secondary, msg);
      }
   
   if (elaboration[0])
      {
      sprintf(appendPoint, "\nElaboration: %s", elaboration);
      }
   
   actualSize = strlen(errorMessage) + 1;
   
   /* Prepare return values */
   if (bufferSize == 0)
      {
      error = actualSize;
      }
   else 
      {
      if (bufferSize > 0)
         {
         if (actualSize > bufferSize)
            {
            error = actualSize;
            actualSize = bufferSize;
            }
         }
      
      memcpy(description, errorMessage, actualSize-1);
      description[actualSize-1] = '\0';
      }
   
   if (errorCode) 
      {
      *errorCode = primary;
      }
   
Error:
    /* Unlock Session */
   Ivi_UnlockSession (vi, &locked);
   return error;
}

ViStatus _VI_FUNC XanM9B_ClearError (ViSession vi)
{                                                                                                           
   return Ivi_ClearErrorInfo (vi);                                                                             
}


/*****************************************************************************
 *-------------------- Utility Functions (Not Exported) ---------------------*
 *****************************************************************************/
             
/**************************************************************************
 *  Function: XanM9B_QueryID
 *  Purpose:  Query the instrument for its id string.
 *
 *  Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus XanM9B_QueryID (ViSession vi, ViSession io,
                                ViInt32 bufferSize,
                                ViChar buffer[])
{
   ViStatus  error = VI_SUCCESS;
   ViUInt32  readCount;

   viCheckErr( viPrintf(io, "ID?"));
   Delay( configureableDelay );
   viCheckErr( viRead (io, buffer, bufferSize-1, &readCount));
   buffer[readCount] = '\0';
   
Error:                                                    
   return error;
}

/*****************************************************************************
 *  Function: XanM9B_GetDriverSetupOption
 *  Purpose:  Get the value of a DriverSetup option. The driver setup string
 *            is part of the option string the user passes to the
 *            XanM9B_InitWithOptions function. Specifically it is the part
 *            that follows "DriverSetup=". This function assumes the 
 *            DriverSetup string has the following format:
 *            "Model : Option Value" 
 *
 *  Example   DriverSetup = Model: XFR12-100  (within existing quotes of 
 *         the Option String)
 *****************************************************************************/
static ViStatus XanM9B_GetDriverSetupOption (ViSession vi, ViInt32 *modelType)
{
   ViStatus    error = VI_SUCCESS;
   ViChar      *tag,
               setup[BUFFER_SIZE],
               model[BUFFER_SIZE];

   *modelType = XANM9B_MODEL_NOT_FOUND;
   checkErr( Ivi_GetAttributeViString (vi, VI_NULL, IVI_ATTR_DRIVER_SETUP, 0, BUFFER_SIZE, setup));
    
   if ((tag = strstr (setup, SETUP_MODEL_TAG)) != VI_NULL)
      {
      sscanf(tag, "%*[^:]:%[^\n]", model);
      RemoveSurroundingWhiteSpace (model);

      if (Ivi_GetValueFromTable (XanM9B_ModelTable, model, modelType) < VI_SUCCESS)
         {
         ViChar buffer[BUFFER_SIZE];
         sprintf (buffer, "Bad value for Model option in DriverSetup: %s", model);
         viCheckErrElab (IVI_ERROR_BAD_OPTION_VALUE, buffer);
         }
      }

Error:
   return error;
}

/*****************************************************************************
 * Function: RemoveSurroundingWhiteSpace
 * Purpose:  This function removes surrounding white space around a string
 *****************************************************************************/
static void RemoveSurroundingWhiteSpace(ViChar string[])
{
   ViChar  *start = string,
           *end;
   ViInt32 lengthWithoutLeadingWS;

   if (start)
      {
      while (*start && isspace((unsigned char)*start))
          {
          start++;
          }
          
      lengthWithoutLeadingWS = strlen( start);
      memmove( string, start, lengthWithoutLeadingWS);
      end = string + lengthWithoutLeadingWS;
      
      while (end > string && isspace((unsigned char)*(end-1)))
         {
         end--;
         }
        
      *end = '\0';
      }
}

/*****************************************************************************
 * Function: XanM9B_GetModelFromBuffer
 * Purpose:  This function returns model number from buffer. If table 
 *           XanM9B_ModelTable does not contains this model, function
 *           returns VI_ERROR_FAIL_ID_QUERY;
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus XanM9B_GetModelFromBuffer(ViSession vi, ViConstString buffer,
                                          ViInt32 *modelType)
{
   ViStatus error = VI_SUCCESS;
   ViChar   tmpBuffer[BUFFER_SIZE];
 
   sscanf(buffer,"ID %s",tmpBuffer);
   
   if (Ivi_GetValueFromTable (XanM9B_ModelTable, tmpBuffer, modelType) < VI_SUCCESS)
   viCheckErr( VI_ERROR_FAIL_ID_QUERY);

Error:
   return error;
}

/*****************************************************************************
 * Function: XanM9B_CheckStatus                                                 
 * Purpose:  This function checks the status of the instrument to detect 
 *           whether the instrument has encountered an error.  This function  
 *           is called at the end of most exported driver functions.  If the    
 *           instrument reports an error, this function returns      
 *           IVI_ERROR_INSTR_SPECIFIC.  The user can set the 
 *           IVI_ATTR_QUERY_INSTR_STATUS attribute to VI_FALSE to disable this 
 *           check and increase execution speed.                                   
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus XanM9B_CheckStatus (ViSession vi)
{
   ViStatus    error = VI_SUCCESS;

   if (Ivi_QueryInstrStatus (vi)&& Ivi_NeedToCheckStatus (vi) && !Ivi_Simulating (vi))
      {
      checkErr( XanM9B_CheckStatusCallback (vi, Ivi_IOSession(vi)));
      checkErr( Ivi_SetNeedToCheckStatus (vi, VI_FALSE));
      }
        
Error:
   return error;
}

/*****************************************************************************
 * Function: XanM9B_DefaultInstrSetup                                               
 * Purpose:  This function sends a default setup to the instrument.  The    
 *           XanM9B_reset function calls this function.  The 
 *           XanM9B_IviInit function calls this function when the
 *           user passes VI_FALSE for the reset parameter.  This function is 
 *           useful for configuring settings that other instrument driver 
 *           functions require.    
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus XanM9B_DefaultInstrSetup (ViSession vi)
{
   ViStatus    error = VI_SUCCESS;
   ViReal64    max=0;    
    
   checkErr( Ivi_LockSession(vi, VI_NULL));
   /* Invalidate all attributes */
   checkErr( Ivi_InvalidateAllAttributes (vi));
   
   if (!Ivi_Simulating(vi))
      {
      ViSession   io = Ivi_IOSession(vi); /* call only when locked */
      checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
      }

Error:
   Ivi_UnlockSession (vi, VI_NULL);
   return error;
}

/*****************************************************************************
 * Function: XanM9B_GetErrorInfo and XanM9B_ClearErrorInfo Functions                       
 * Purpose:  These functions enable the instrument driver user to  
 *           get or clear the error information the driver associates with the
 *           IVI session.
 * Note:  These functions have been deprecated.
 *****************************************************************************/
ViStatus _VI_FUNC XanM9B_GetErrorInfo (ViSession vi, ViStatus *primaryError, 
                                       ViStatus *secondaryError, ViChar errorElaboration[256])  
{                                                                                                           
   return Ivi_GetErrorInfo(vi, primaryError, secondaryError, errorElaboration);                                
}                                                                                                           

ViStatus _VI_FUNC XanM9B_ClearErrorInfo (ViSession vi)                                                        
{                                                                                                           
   return Ivi_ClearErrorInfo (vi);                                                                             
}


/*****************************************************************************
 *------------------------ Global Session Callbacks -------------------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: XanM9B_CheckStatusCallback                                               
 * Purpose:  This function queries the instrument to determine if it has 
 *           encountered an error.  If the instrument has encountered an 
 *           error, this function returns the IVI_ERROR_INSTRUMENT_SPECIFIC 
 *           error code.  This function is called by the 
 *           XanM9B_CheckStatus utility function.  The 
 *           Ivi_SetAttribute and Ivi_GetAttribute functions invoke this 
 *           function when the optionFlags parameter includes the
 *           IVI_VAL_DIRECT_USER_CALL flag.
 *           
 *           The user can disable calls to this function by setting the 
 *           IVI_ATTR_QUERY_INSTR_STATUS attribute to VI_FALSE.  The driver can 
 *           disable the check status callback for a particular attribute by 
 *           setting the IVI_VAL_DONT_CHECK_STATUS flag.
 *****************************************************************************/
static ViStatus _VI_FUNC XanM9B_CheckStatusCallback (ViSession vi, ViSession io)
{
   ViStatus    error = VI_SUCCESS;
   ViInt32     esr = 0; 

   viCheckErr(viPrintf (io,"STS?"));
   Delay( configureableDelay );
   viCheckErr(viScanf (io,"STS %hd",&esr)); 
       
   if (IEEE_488_2_ERROR_BITS & esr)
      {
      viCheckErr( IVI_ERROR_INSTR_SPECIFIC);   
      }
            
Error:
   return error;
}


/*****************************************************************************
 *----------------- Attribute Range Tables and Callbacks --------------------*
 *****************************************************************************/

/*- XANM9B_ATTR_ID_QUERY_RESPONSE -*/
static ViStatus _VI_FUNC XanM9BAttrIdQueryResponse_ReadCallback (ViSession vi, 
                                                                 ViSession io,
                                                                 ViConstString channelName, 
                                                                 ViAttr attributeId,
                                                                 const ViConstString cacheValue)
{
   ViStatus    error = VI_SUCCESS;
   ViChar      rdBuffer[BUFFER_SIZE];

   checkErr( XanM9B_QueryID (vi, io, BUFFER_SIZE, rdBuffer));
   checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
 
Error:
   return error;
}

/*- XANM9B_ATTR_SPECIFIC_DRIVER_REVISION -*/
static ViStatus _VI_FUNC XanM9BAttrDriverRevision_ReadCallback (ViSession vi, 
                                                                ViSession io,
                                                                ViConstString channelName, 
                                                                ViAttr attributeId,
                                                                const ViConstString cacheValue)
{
   ViStatus    error = VI_SUCCESS;
   ViChar      driverRev[256];
    
   sprintf (driverRev, 
            "Driver: XanM9B %d.%d, Compiler: %s %3.2f, "
            "Components: IVIEngine %.2f, VISA-Spec %.2f",
            XANM9B_MAJOR_VERSION , XANM9B_MINOR_VERSION, 
            IVI_COMPILER_NAME, IVI_COMPILER_VER_NUM, 
            IVI_ENGINE_MAJOR_VERSION + IVI_ENGINE_MINOR_VERSION/1000.0, 
            Ivi_ConvertVISAVer(VI_SPEC_VERSION));

   checkErr( Ivi_SetValInStringCallback (vi, attributeId, driverRev));    
Error:
   return error;
}

/*- XANM9B_ATTR_INSTRUMENT_FIRMWARE_REVISION -*/
static ViStatus _VI_FUNC XanM9BAttrInstrumentFirmwareRevision_ReadCallback (ViSession vi, 
                                                                            ViSession io,
                                                                            ViConstString channelName, 
                                                                            ViAttr attributeId,
                                                                            const ViConstString cacheValue)
{
   ViStatus    error = VI_SUCCESS;
   ViChar      instrRev[BUFFER_SIZE];
    
   viCheckErr( viPrintf (io, "ROM?"));
   Delay( configureableDelay );
   viCheckErr( viScanf (io, "%256[^\r]", instrRev));
   
   checkErr( Ivi_SetValInStringCallback (vi, attributeId, instrRev));
    
Error:
   return error;
}

/*- XANM9B_ATTR_INSTRUMENT_MANUFACTURER -*/
static ViStatus _VI_FUNC XanM9BAttrInstrumentManufacturer_ReadCallback (ViSession vi, 
                                                                        ViSession io,
                                                                        ViConstString channelName, 
                                                                        ViAttr attributeId,
                                                                        const ViConstString cacheValue)
{
   ViStatus    error = VI_SUCCESS;
   ViChar      rdBuffer[BUFFER_SIZE];

   checkErr( Ivi_SetValInStringCallback (vi, attributeId, "Xantrex"));
    
Error:
   return error;
}

/*- XANM9B_ATTR_INSTRUMENT_MODEL -*/
static ViStatus _VI_FUNC XanM9BAttrInstrumentModel_ReadCallback (ViSession vi, 
                                                                 ViSession io,
                                                                 ViConstString channelName, 
                                                                 ViAttr attributeId,
                                                                 const ViConstString cacheValue)
{
   ViStatus    error = VI_SUCCESS;
   ViChar      rdBuffer[BUFFER_SIZE];
   ViChar      idQ[BUFFER_SIZE];
    
   checkErr( Ivi_GetAttributeViString (vi, VI_NULL, XANM9B_ATTR_ID_QUERY_RESPONSE,
                                       0, BUFFER_SIZE, idQ));
   sscanf (idQ, "ID %s", rdBuffer);

   checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
    
Error:
   return error;
}

/*- XANM9B_ATTR_OUTPUT_ENABLED -*/
static ViStatus _VI_FUNC XanM9BAttrOutputEnabled_WriteCallback (ViSession vi, 
                                                                ViSession io,
                                                                ViConstString channelName, 
                                                                ViAttr attributeId,
                                                                ViBoolean value)
{
   ViStatus error = VI_SUCCESS;

   viCheckErr( viPrintf (io, "OUT %hd",  (value) ? 1 : 0));
   Delay( configureableDelay );

Error:
   return error;
}

static ViStatus _VI_FUNC XanM9BAttrOutputEnabled_ReadCallback (ViSession vi, 
                                                               ViSession io,
                                                               ViConstString channelName, 
                                                               ViAttr attributeId,
                                                               ViBoolean* value)
{
   ViStatus error = VI_SUCCESS;

   viCheckErr(viPrintf(io,"OUT?"));
   Delay( configureableDelay );
   viCheckErr( viScanf (io, "OUT %hd",value));

Error:
   return error;
}

/*- XANM9B_ATTR_AUXA_ENABLED -*/
static ViStatus _VI_FUNC XanM9BAttrAuxaEnabled_WriteCallback (ViSession vi, 
                                                              ViSession io,
                                                              ViConstString channelName, 
                                                              ViAttr attributeId,
                                                              ViBoolean value)
{
   ViStatus error = VI_SUCCESS;

   viCheckErr( viPrintf (io, "AUXA %hd",  (value) ? 1 : 0));
   Delay( configureableDelay );

Error:
   return error;
}

static ViStatus _VI_FUNC XanM9BAttrAuxaEnabled_ReadCallback (ViSession vi, 
                                                             ViSession io,
                                                             ViConstString channelName, 
                                                             ViAttr attributeId,
                                                             ViBoolean* value)
{
   ViStatus error = VI_SUCCESS;
    
   viCheckErr(viPrintf(io,"AUXA?")); 
   Delay( configureableDelay );
   viCheckErr(viScanf (io, "AUXA %hd",value));

Error:
   return error;
}

/*- XANM9B_ATTR_AUXB_ENABLED -*/
static ViStatus _VI_FUNC XanM9BAttrAuxbEnabled_WriteCallback (ViSession vi, 
                                                              ViSession io,
                                                              ViConstString channelName, 
                                                              ViAttr attributeId,
                                                              ViBoolean value)
{
   ViStatus error = VI_SUCCESS;

   viCheckErr( viPrintf (io, "AUXB %hd",  (value) ? 1 : 0));
   Delay( configureableDelay );

Error:
   return error;
}

static ViStatus _VI_FUNC XanM9BAttrAuxbEnabled_ReadCallback (ViSession vi, 
                                                             ViSession io,
                                                             ViConstString channelName, 
                                                             ViAttr attributeId,
                                                             ViBoolean* value)
{
   ViStatus error = VI_SUCCESS;
    
   viCheckErr(viPrintf(io,"AUXB?")); 
   Delay( configureableDelay );
   viCheckErr(viScanf (io, "AUXB %hd",value));

Error:
   return error;
}

/*- XANM9B_ATTR_CMODE_ENABLED -*/
static ViStatus _VI_FUNC XanM9BAttrCmodeEnabled_ReadCallback (ViSession vi, 
                                                              ViSession io,
                                                              ViConstString channelName, 
                                                              ViAttr attributeId,
                                                              ViBoolean* value)
{
   ViStatus error = VI_SUCCESS;

   viCheckErr(viPrintf(io,"CMODE?")); 
   Delay( configureableDelay );
   viCheckErr(viScanf (io, "CMODE %hd",value));

Error:
   return error;
}

static ViStatus _VI_FUNC XanM9BAttrCmodeEnabled_WriteCallback (ViSession vi, 
                                                               ViSession io,
                                                               ViConstString channelName, 
                                                               ViAttr attributeId,
                                                               ViBoolean value)
{
   ViStatus error = VI_SUCCESS;

   viCheckErr( viPrintf (io, "CMODE %ld",  (value) ? 1 : 0));
   Delay( configureableDelay );

Error:
   return error;
}

/*- XANM9B_ATTR_VOLTAGE_LEVEL -*/
static ViStatus _VI_FUNC XanM9BAttrVoltageLevel_WriteCallback (ViSession vi, 
                                                               ViSession io,
                                                               ViConstString channelName, 
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
   ViStatus error = VI_SUCCESS;

   viCheckErr( viPrintf (io, "VSET %lf", value));
   Delay( configureableDelay );
 
Error:
   return error;
}

static ViStatus _VI_FUNC XanM9BAttrVoltageLevel_ReadCallback (ViSession vi, 
                                                              ViSession io,
                                                              ViConstString channelName, 
                                                              ViAttr attributeId,
                                                              ViReal64* value)
{
   ViStatus error = VI_SUCCESS;
   
   viCheckErr( viPrintf (io, "VSET?"));
   Delay( configureableDelay );
   viCheckErr( viScanf(io, "VSET %lf",value));
   
Error:
   return error;
}

/*- XANM9B_ATTR_OVP_LIMIT -*/
static ViStatus _VI_FUNC XanM9BAttrOVPLimit_WriteCallback (ViSession vi, 
                                                           ViSession io,
                                                           ViConstString channelName, 
                                                           ViAttr attributeId,
                                                           ViReal64 value)
{
   ViStatus error = VI_SUCCESS;
  
   viCheckErr( viPrintf (io, "OVSET %lf", value));
   Delay( configureableDelay );
     
Error:
   return error;
}

static ViStatus _VI_FUNC XanM9BAttrOVPLimit_ReadCallback (ViSession vi, 
                                                          ViSession io,
                                                          ViConstString channelName, 
                                                          ViAttr attributeId,
                                                          ViReal64* value)
{
   ViStatus error = VI_SUCCESS;
    
   viCheckErr(viPrintf(io,"OVSET?"));
   Delay( configureableDelay );
   viCheckErr( viScanf (io, "OVSET %lf", value));
   
Error:
   return error;
}


/*- XANM9B_ATTR_CURRENT_LIMIT-*/
static ViStatus _VI_FUNC XanM9BAttrCurrentLimit_WriteCallback (ViSession vi, 
                                                               ViSession io,
                                                               ViConstString channelName, 
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
   ViStatus error = VI_SUCCESS;
      
   viCheckErr( viPrintf (io, "ISET %lf", value));
   Delay( configureableDelay );
    
Error: 
   return error;
}

static ViStatus _VI_FUNC XanM9BAttrCurrentLimit_ReadCallback (ViSession vi, 
                                                              ViSession io,
                                                              ViConstString channelName, 
                                                              ViAttr attributeId,
                                                              ViReal64* value)
{
   ViStatus error = VI_SUCCESS;
    
   viCheckErr( viPrintf(io,"ISET?"));
   Delay( configureableDelay );
   viCheckErr(viScanf(io, "ISET %lf", value));
     
Error:
   return error;
}

/*- XANM9B_ATTR_HOLD_ENABLED -*/
static ViStatus _VI_FUNC XanM9BAttrHoldEnabled_ReadCallback (ViSession vi, 
                                                             ViSession io,
                                                             ViConstString channelName, 
                                                             ViAttr attributeId,
                                                             ViBoolean* value)
{
   ViStatus error = VI_SUCCESS;

   viCheckErr( viPrintf (io, "HOLD?"));
   Delay( configureableDelay );
   viCheckErr( viScanf(io, "HOLD %hd",value));

Error:
   return error;
}

static ViStatus _VI_FUNC XanM9BAttrHoldEnabled_WriteCallback (ViSession vi, 
                                                              ViSession io,
                                                              ViConstString channelName, 
                                                              ViAttr attributeId,
                                                              ViBoolean value)
{
   ViStatus error = VI_SUCCESS;

   viCheckErr( viPrintf (io, "HOLD %ld",  (value) ? 1 : 0));
   Delay( configureableDelay );

Error:
   return error;
}   

/*- XANM9B_ATTR_TRIGGERED_VOLTAGE_LEVEL -*/
static ViStatus _VI_FUNC XanM9BAttrTrigVoltageLevel_WriteCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   ViReal64 value)
{
   ViStatus error = VI_SUCCESS;
   
   viCheckErr( viPrintf (io, "VSET %lf", value));
   Delay( configureableDelay );
    
Error:
   return error;
}

static ViStatus _VI_FUNC XanM9BAttrTrigVoltageLevel_ReadCallback (ViSession vi, 
                                                                  ViSession io,
                                                                  ViConstString channelName, 
                                                                  ViAttr attributeId,
                                                                  ViReal64* value)
{
   ViStatus error = VI_SUCCESS;
    
   viCheckErr( viPrintf(io,"VSET?"));
   Delay( configureableDelay );
   viCheckErr( viScanf(io, "VSET %lf", value));
    
Error:
   return error;
}

/*- XANM9B_ATTR_TRIGGERED_CURRENT_LIMIT -*/
static ViStatus _VI_FUNC XanM9BAttrTrigCurrentLimit_WriteCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   ViReal64 value)
{
   ViStatus error = VI_SUCCESS;

   viCheckErr( viPrintf (io, "ISET %lf", value));
   Delay( configureableDelay );
    
Error:
   return error;
}

static ViStatus _VI_FUNC XanM9BAttrTrigCurrentLimit_ReadCallback (ViSession vi, 
                                                                  ViSession io,
                                                                  ViConstString channelName, 
                                                                  ViAttr attributeId,
                                                                  ViReal64* value)
{
   ViStatus error = VI_SUCCESS;
    
   viCheckErr( viPrintf (io,"ISET?"));
   Delay( configureableDelay );
   viCheckErr( viScanf (io, "ISET %lf", value));

Error:
   return error;
}

/*- XANM9B_ATTR_FAULT_DELAY -*/
static IviRangeTableEntry attrFaultDelayRangeTableEntries[] =
   {
      {0.0, 32.0, 0, "", 0},
      {IVI_RANGE_TABLE_LAST_ENTRY}
   };

static IviRangeTable attrFaultDelayRangeTable =
   {
      IVI_VAL_RANGED,
      VI_TRUE,
      VI_TRUE,
      VI_NULL,
      attrFaultDelayRangeTableEntries,
   };

static ViStatus _VI_FUNC XanM9BAttrFaultDelay_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViReal64 *value)
{
   ViStatus    error = VI_SUCCESS;
   ViString    word=0;
    
   viCheckErr (viPrintf (io, "DLY?"));
   Delay( configureableDelay );
   viCheckErr (viScanf(io,"DLY %lf",value));

Error:
   return error;              
}

static ViStatus _VI_FUNC XanM9BAttrFaultDelay_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 value)
{
   ViStatus    error = VI_SUCCESS;
   viCheckErr (viPrintf (io, "DLY %Lf",  value));
   Delay( configureableDelay );
   
Error:
   return error;
}

/*- XANM9B_ATTR_MAXIMUM_CURRENT_LIMIT -*/
static ViStatus _VI_FUNC XanM9BAttrMaximumCurrentLimit_ReadCallback (ViSession vi,
                                                                     ViSession io,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViReal64 *value)
{
   ViStatus    error = VI_SUCCESS;
    
   viCheckErr (viPrintf (io, "IMAX?"));
   Delay( configureableDelay );
   viCheckErr (viScanf(io,"IMAX %lf",value));

Error:
   return error;              
}

static ViStatus _VI_FUNC XanM9BAttrMaximumCurrentLimit_WriteCallback (ViSession vi,
                                                                      ViSession io,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViReal64 value)
{
   ViStatus    error = VI_SUCCESS;
   viCheckErr (viPrintf (io, "IMAX %Lf",  value));
   Delay( configureableDelay );

Error:
   return error;
}

/*- XANM9B_ATTR_MAXIMUM_VOLTAGE -*/ 
static ViStatus _VI_FUNC XanM9BAttrMaximumVoltage_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
   ViStatus    error = VI_SUCCESS;
    
   viCheckErr (viPrintf (io, "VMAX?"));
   Delay( configureableDelay );
   viCheckErr (viScanf(io,"VMAX %lf",value));

Error:
   return error;              
}

static ViStatus _VI_FUNC XanM9BAttrMaximumVoltage_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
   ViStatus    error = VI_SUCCESS;
   viCheckErr (viPrintf (io, "VMAX %Lf",  value));
   Delay( configureableDelay );
    
Error:
   return error;
}

/*- XANM9B_ATTR_SRQ_ENABLED -*/
static ViStatus _VI_FUNC XanM9BAttrSrqEnabled_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViBoolean *value)
{
   ViStatus    error = VI_SUCCESS;
    
   viCheckErr (viPrintf(io,"SRQ?"));
   Delay( configureableDelay );
   viCheckErr (viScanf (io, "SRQ %hd", value));

Error:
   return error;
}

static ViStatus _VI_FUNC XanM9BAttrSrqEnabled_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViBoolean value)
{
   ViStatus    error = VI_SUCCESS;
   viCheckErr (viPrintf (io, "SRQ %hd", value));
   Delay( configureableDelay );

Error:
   return error;
}
 
static IviRangeTableEntry attrFoldbackSettingRangeTableEntries[] =
   {
      {XANM9B_VAL_FOLD_OFF, 0, 0, "OFF", 0},
      {XANM9B_VAL_FOLD_CV, 0, 0, "CV", 0},
      {XANM9B_VAL_FOLD_CC, 0, 0, "CC", 0},
      {IVI_RANGE_TABLE_LAST_ENTRY}
   };

static IviRangeTable attrFoldbackSettingRangeTable =
   {
      IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrFoldbackSettingRangeTableEntries,
   };

/*- XANM9B_ATTR_FOLDBACK_SETTING -*/ 
static ViStatus _VI_FUNC XanM9BAttrFoldbackSetting_WriteCallback (ViSession vi, 
                                                                  ViSession io,
                                                                  ViConstString channelName,   
                                                                  ViAttr attributeId,
                                                                  ViInt32 value)
{
   ViStatus    error = VI_SUCCESS;
   viCheckErr (viPrintf (io, "FOLD %hd", value));
   Delay( configureableDelay );

Error:
   return error;
}

static ViStatus _VI_FUNC XanM9BAttrFoldbackSetting_ReadCallback (ViSession vi, 
                                                                 ViSession io,
                                                                 ViConstString channelName, 
                                                                 ViAttr attributeId,
                                                                 ViInt32* value)
{
   ViStatus    error = VI_SUCCESS;
   ViString    cmd=0;
    
   viCheckErr (viPrintf(io,"FOLD?"));
   Delay( configureableDelay );
   viCheckErr (viScanf (io, "FOLD %hd",value));

Error:
   return error;
} 

/*- XANM9B_ATTR_MASK_SETTING -*/
static ViStatus _VI_FUNC XanM9BAttrMaskSetting_WriteCallback (ViSession vi, 
                                                              ViSession io,
                                                              ViConstString channelName,   
                                                              ViAttr attributeId,
                                                              ViConstString value)
{
   ViStatus    error = VI_SUCCESS;
   viCheckErr (viPrintf (io, "MASK %s", value));
   Delay( configureableDelay );

Error:
   return error;
}

/*- XANM9B_ATTR_UNMASK_SETTING -*/
static ViStatus _VI_FUNC XanM9BAttrUnmaskSetting_WriteCallback (ViSession vi, 
                                                                ViSession io,
                                                                ViConstString channelName,   
                                                                ViAttr attributeId,
                                                                ViConstString value)
{
   ViStatus    error = VI_SUCCESS;
   viCheckErr (viPrintf (io, "UNMASK %s", value));
   Delay( configureableDelay );

Error:
   return error;
} 
 
/*- XANM9B_ATTR_UNMASK_CONDITION -*/ 
static ViStatus _VI_FUNC XanM9BAttrUnmaskCondition_ReadCallback (ViSession vi, 
                                                                 ViSession io,
                                                                 ViConstString channelName,   
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value)
{
   ViStatus    error = VI_SUCCESS;
   
   viCheckErr (viPrintf (io, "UNMASK?" ));
   Delay( configureableDelay );
   viCheckErr (viScanf (io,"UNMASK %hd",value));

Error:
   return error;
}

/*- XANM9B_ATTR_INSTR_STATUS_REG -*/                       
static ViStatus _VI_FUNC XanM9BAttrInstrStatusReg_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value)
{
   ViStatus error = VI_SUCCESS;

   viCheckErr (viPrintf (io, "STS?" ));
   Delay( configureableDelay );
   viCheckErr (viScanf (io,"STS %hd",value));

Error:
   return error;
}

/*- XANM9B_ATTR_CURRENT_LIMIT_BEHAVIOR -*/
static IviRangeTableEntry attrCurrentLimitBehaviorRangeTableEntries[] =
   {
      {XANM9B_VAL_CURRENT_REGULATE, 0, 0, "REG", 0},
      {XANM9B_VAL_CURRENT_TRIP, 0, 0, "TRP", 0},
      {IVI_RANGE_TABLE_LAST_ENTRY}
   };

static IviRangeTable attrCurrentLimitBehaviorRangeTable =
   {
      IVI_VAL_DISCRETE,
      VI_FALSE,
      VI_FALSE,
      VI_NULL,
      attrCurrentLimitBehaviorRangeTableEntries,
   };
   
static ViStatus _VI_FUNC XanM9BAttrCurrentLimitBehavior_ReadCallback (ViSession vi,
                                                                      ViSession io,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViInt32 *value)
{
   ViStatus error = VI_SUCCESS;
   ViInt32  foldMode = 0;
   
   checkErr( XanM9B_QueryMode( vi, "1", XANM9B_ATTR_FOLDBACK_SETTING, &foldMode ) );
         
   if( foldMode == XANM9B_VAL_FOLD_CC )
   {
      *value = XANM9B_VAL_CURRENT_TRIP;      
   }
   else
   {
      *value = XANM9B_VAL_CURRENT_REGULATE;
   }
   
Error:
   return error;
}

static ViStatus _VI_FUNC XanM9BAttrCurrentLimitBehavior_WriteCallback (ViSession vi,
                                                                       ViSession io,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViInt32 value)
{
   ViStatus error = VI_SUCCESS;
   ViInt32  foldMode = 0;
   
   if( value == XANM9B_VAL_CURRENT_TRIP )
   {
      /* The user has selected to have the unit trip in an over current situation, since these
         units do not have an OCP setpoint, set the Foldback to Constant Current, and fault delay
         to 3 seconds, to avoid any nuisance faults due to changes in the setpoint. */
      checkErr( XanM9B_ConfigureFaultDelay( vi, "1", 3 ) );
      checkErr( XanM9B_ConfigureFoldbackSetting( vi, "1", XANM9B_VAL_FOLD_CC ) );
   }
   else
   {
      /* The user has selected to have the unit regulate current therefore, check the fold mode 
         of the unit. If it is set to fold in Constant Current, turn the fold protection off. 
         Otherwise, do nothing, as the user might have selected to fold in Constant Voltage. */
      checkErr( XanM9B_QueryMode( vi, "1", XANM9B_ATTR_FOLDBACK_SETTING, &foldMode ) );
      
      if( foldMode == XANM9B_VAL_FOLD_CC )
      {
         checkErr( XanM9B_ConfigureFoldbackSetting( vi, "1", XANM9B_VAL_FOLD_OFF ) );
      }
   }
   
Error:
   return error;
}

/*- XANM9B_ATTR_OVP_ENABLED -*/
static ViStatus _VI_FUNC XanM9BAttrOVPEnabled_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViBoolean *value)
{
   ViStatus error = VI_SUCCESS;

   *value = VI_TRUE;
   
Error:
   return error;
}

static ViStatus _VI_FUNC XanM9BAttrOVPEnabled_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViBoolean value)
{
   ViStatus error = VI_SUCCESS;
   
   /* This callback does not do anything */

Error:
   return error;
}

/*- XANM9B_ATTR_TRIGGER_SOURCE -*/
static ViStatus _VI_FUNC XanM9BAttrTriggerSource_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value)
{
   ViStatus error = VI_SUCCESS;
   
   *value = XANM9B_VAL_TRIG_IMMEDIATE;

Error:
   return error;
}

static ViStatus _VI_FUNC XanM9BAttrTriggerSource_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value)
{
   ViStatus error = VI_SUCCESS;
   
   /* This callback does not do anything */

Error:
   return error;
}
                       
 /*****************************************************************************
 *  Function: XanM9B_QueryACCRegister
 *  Purpose:  Query Acc Register.
 *****************************************************************************/
static ViStatus XanM9B_QueryACCRegister(ViSession vi, ViConstString channelName,
                                        ViInt32 *reg)
                                    
{
   ViStatus        error = VI_SUCCESS;
    
   checkErr( Ivi_LockSession(vi, VI_NULL));
   
   if (!Ivi_Simulating(vi))              /* call only when locked */
      {
      ViSession io = Ivi_IOSession(vi); /* call only when locked */
      checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
      viCheckErr(viPrintf(io,"ASTS?"));
      Delay( configureableDelay );
      viCheckErr(viScanf(io,"ASTS %ld",reg));
        
      }

   checkErr( XanM9B_CheckStatus (vi));

Error:
    
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}
 
/*****************************************************************************
 *  Function: XanM9B_QueryFaultRegister
 *  Purpose:  Query Fault Register.
 *****************************************************************************/
static ViStatus XanM9B_QueryFaultRegister(ViSession vi, ViConstString channelName,
                                          ViInt32 *reg)
                                    
{
   ViStatus        error = VI_SUCCESS;
    
   checkErr( Ivi_LockSession(vi, VI_NULL));
   
   if (!Ivi_Simulating(vi))              /* call only when locked */
      {
      ViSession io = Ivi_IOSession(vi); /* call only when locked */
      checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
      viCheckErr(viPrintf(io,"FAULT?"));
      Delay( configureableDelay );
      viCheckErr(viScanf(io,"FAULT %ld",reg));
      }

   checkErr( XanM9B_CheckStatus (vi));

Error:
   Ivi_UnlockSession(vi, VI_NULL);
   return error;
}


static IviRangeTableEntry attrRemoteModeRangeTableXTEntries[] =
   {
      {XANM9B_VAL_LOC, 0, 0, "LOC ON", 0},
      {XANM9B_VAL_REM, 0, 0, "LOC OFF", 0},
      {IVI_RANGE_TABLE_LAST_ENTRY}
   };

static IviRangeTable attrRemoteModeRangeTableXT =
   {
      IVI_VAL_DISCRETE,
      VI_TRUE,
      VI_TRUE,
      VI_NULL,
      attrRemoteModeRangeTableXTEntries,
   };
   
static IviRangeTableEntry attrRemoteModeRangeTableXFREntries[] =
   {
      {XANM9B_VAL_LOC, 0, 0, "REM OFF", 0},
      {XANM9B_VAL_REM, 0, 0, "REM ON", 0},
      {IVI_RANGE_TABLE_LAST_ENTRY}
   };

static IviRangeTable attrRemoteModeRangeTableXFR =
   {
      IVI_VAL_DISCRETE,
      VI_TRUE,
      VI_TRUE,
      VI_NULL,
      attrRemoteModeRangeTableXFREntries,
   };
   
/* XANM9B_ATTR_REMOTE_MODE */   
static ViStatus _VI_FUNC XanM9BAttrRemoteMode_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 *value)
{
   ViStatus error = VI_SUCCESS;
   ViInt16  statusReg = 0;
   
   viCheckErr( viPrintf( io, "STS?" ) ) ;
   Delay( configureableDelay );
   viCheckErr( viScanf( io,"STS %hd", &statusReg ) ); 
   
   statusReg = statusReg & 0x0200;
   
   if( statusReg )
   {
      *value = XANM9B_VAL_REM;
   }
   else
   {
      *value = XANM9B_VAL_LOC;
   }

Error:
   return error;
}

static ViStatus _VI_FUNC XanM9BAttrRemoteMode_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 value)
{
   ViStatus error = VI_SUCCESS;
   ViChar modelType[ BUFFER_SIZE ]; 
   ViString cmd;
   
   checkErr( Ivi_GetAttributeViString( vi, VI_NULL, XANM9B_ATTR_INSTRUMENT_MODEL, 0, BUFFER_SIZE, 
                                       modelType ) );
                             
   if( ( strstr( modelType, "XT" ) != NULL ) || ( strstr( modelType, "HPD" ) != NULL ) )
   {
      /* Get the command string from the range table */
      viCheckParm( Ivi_GetViInt32EntryFromValue( value, &attrRemoteModeRangeTableXT,
                                                 VI_NULL, VI_NULL, VI_NULL, 
                                                 VI_NULL, &cmd, VI_NULL ), 2, 
                                                 "Remote State" );      
   }
   else
   {
      /* Get the command string from the range table */
      viCheckParm( Ivi_GetViInt32EntryFromValue( value, &attrRemoteModeRangeTableXFR,
                                                 VI_NULL, VI_NULL, VI_NULL, 
                                                 VI_NULL, &cmd, VI_NULL ), 2, 
                                                 "Remote State" );   
   }
   
   viCheckErr( viPrintf( io, cmd ) );
   Delay( configureableDelay );

Error:
   return error;
}

/*****************************************************************************
 * Function: XanM9B_InitAttributes                                                  
 * Purpose:  This function adds attributes to the IVI session, initializes   
 *           instrument attributes, and sets attribute invalidation          
 *           dependencies.                                                   
 *****************************************************************************/

static ViStatus XanM9B_InitAttributes (ViSession vi, ViInt32 modelType)
{
   ViStatus    error = VI_SUCCESS;
   ViInt32     flags = 0;
    
   ViReal64    maxVoltage, OVPMaxVoltage,OVPMinVoltage = 0,currentLimit;
   ViChar      idnBuffer[BUFFER_SIZE];
   ViInt32     modelNmb;
    
   IviRangeTablePtr  voltageRangeTablePtr = VI_NULL,
                     ovpRangeTablePtr = VI_NULL,
                     currentRangeTablePtr = VI_NULL;
    
   switch (modelType)
      {
      /* XFR 2.8kW Models*/
      case XFR7_5300:
         maxVoltage = 7.5;
         OVPMaxVoltage = 8.25;
         currentLimit = 300;
         break;
      case XFR12220:
         maxVoltage = 12;
         OVPMaxVoltage = 13.2;
         currentLimit = 220;
         break;
      case XFR20130:
         maxVoltage = 20;
         OVPMaxVoltage = 22;
         currentLimit = 130;
         break;
      case XFR3385:
         maxVoltage = 33;
         OVPMaxVoltage = 36.3;
         currentLimit = 85;
         break;
      case XFR4070:
         maxVoltage = 40;
         OVPMaxVoltage = 44;
         currentLimit = 70;
         break;
      case XFR5060:
         maxVoltage = 50;
         OVPMaxVoltage = 55;
         currentLimit = 60;
         break;
      case XFR6046:
         maxVoltage = 60;
         OVPMaxVoltage = 66;
         currentLimit = 46;
         break;
      case XFR8037:
         maxVoltage = 80;
         OVPMaxVoltage = 88;
         currentLimit = 37;
         break;
      case XFR10028:
         maxVoltage = 100;
         OVPMaxVoltage = 110;
         currentLimit = 28;
         break;
      case XFR15018:
         maxVoltage = 150;
         OVPMaxVoltage = 165;
         currentLimit = 18;
         break;
      case XFR3009:
         maxVoltage = 300;
         OVPMaxVoltage = 330;
         currentLimit = 9;
         break;
      case XFR4207:
         maxVoltage = 420;
         OVPMaxVoltage = 462;
         currentLimit = 7;
         break;
      case XFR6004:
         maxVoltage = 600;
         OVPMaxVoltage = 660;
         currentLimit = 4;
         break;
      /* XFR 1.2kW Models*/
      case XFR6200:
         maxVoltage = 6;
         OVPMaxVoltage = 6.6;
         currentLimit = 200;
         break;
      case XFR7_5140:
         maxVoltage = 7.5;
         OVPMaxVoltage = 8.25;
         currentLimit = 140;
         break;
      case XFR12100:
         maxVoltage = 12;
         OVPMaxVoltage = 13.2;
         currentLimit = 100;
         break;
      case XFR2060:
         maxVoltage = 20;
         OVPMaxVoltage = 22;
         currentLimit = 60;
         break;
      case XFR3535:
         maxVoltage = 35;
         OVPMaxVoltage = 38.5;
         currentLimit = 35;
         break;
      case XFR4030:
         maxVoltage = 40;
         OVPMaxVoltage = 44;
         currentLimit = 30;
         break;
      case XFR4530:
         maxVoltage = 45;
         OVPMaxVoltage = 49.5;
         currentLimit = 30;
         break;
      case XFR6020:
         maxVoltage = 60;
         OVPMaxVoltage = 66;
         currentLimit = 20;
         break;
      case XFR10012:
         maxVoltage = 100;
         OVPMaxVoltage = 110;
         currentLimit = 12;
         break;
      case XFR1508:
         maxVoltage = 150;
         OVPMaxVoltage = 165;
         currentLimit = 8;
         break;
      case XFR1607_5:
         maxVoltage = 160;
         OVPMaxVoltage = 176;
         currentLimit = 7.5;
         break;
      case XFR3004:
         maxVoltage = 300;
         OVPMaxVoltage = 330;
         currentLimit = 4;
         break;
      case XFR6002:
         maxVoltage = 600;
         OVPMaxVoltage = 660;
         currentLimit = 2;
         break;
      /* XHR 1kW Models */
      case XHR7_5130:       
         maxVoltage = 7.5;
         OVPMaxVoltage = 8.25;
         currentLimit = 130;
         break;
      case XHR2050:
         maxVoltage = 20;
         OVPMaxVoltage = 22;
         currentLimit = 50;
         break;
      case XHR3333:
         maxVoltage = 33;
         OVPMaxVoltage = 36.3;
         currentLimit = 33;
         break;
      case XHR4025:
         maxVoltage = 40;
         OVPMaxVoltage = 44;
         currentLimit = 25;
         break;
      case XHR6018:
         maxVoltage = 60;
         OVPMaxVoltage = 66;
         currentLimit = 18;
         break;
      case XHR10010:
         maxVoltage = 100;
         OVPMaxVoltage = 110;
         currentLimit = 10;
         break;
      case XHR1507:
         maxVoltage = 150;
         OVPMaxVoltage = 165;
         currentLimit = 7;
         break;
      case XHR3003_5:
         maxVoltage = 300;
          OVPMaxVoltage = 330;
         currentLimit = 3.5;
         break;
      case XHR6001_7:
         maxVoltage = 600;
          OVPMaxVoltage = 660;
         currentLimit = 1.7;
         break;
      /* XHR 600W Models */
      case XHR7_580:
         maxVoltage = 7.5;
         OVPMaxVoltage = 8.25;
         currentLimit = 80;
         break;
      case XHR2030:
         maxVoltage = 20;
         OVPMaxVoltage = 22;
         currentLimit = 30;
         break;
      case XHR3318:
         maxVoltage = 33;
         OVPMaxVoltage = 36.3;
         currentLimit = 18;
         break;
      case XHR4015:
         maxVoltage = 40;
         OVPMaxVoltage = 44;
         currentLimit = 15;
         break;
      case XHR6010:
         maxVoltage = 60;
         OVPMaxVoltage = 66;
         currentLimit = 10;
         break;
      case XHR1006:
         maxVoltage = 100;
         OVPMaxVoltage = 110;
         currentLimit = 6;
         break;
      case XHR1504:
         maxVoltage = 150;
         OVPMaxVoltage = 165;
         currentLimit = 4;
         break;
      case XHR3002:
         maxVoltage = 300;
         OVPMaxVoltage = 330;
         currentLimit = 2;
         break;
      case XHR6001:
         maxVoltage = 600;
         OVPMaxVoltage = 660;
         currentLimit = 1;
         break;
      /* XPD 500W Models */
      case XPD7_567:
         maxVoltage = 7.5;
         OVPMaxVoltage = 8.25;
         currentLimit = 67;
         break;
      case XPD1830:
         maxVoltage = 18;
         OVPMaxVoltage = 19.8;
         currentLimit = 30;
         break;
      case XPD3316:
         maxVoltage = 33;
         OVPMaxVoltage = 36.3;
         currentLimit = 16;
         break;
      case XPD609:
         maxVoltage = 60;
         OVPMaxVoltage = 66;
         currentLimit = 9;
         break;
      case XPD1204_5:
         maxVoltage = 120;
         OVPMaxVoltage = 132;
         currentLimit = 4.5;
         break;
      /* HPD 300W  Models */  
      case HPD1520:
         maxVoltage = 15;
         OVPMaxVoltage = 16.5;
         currentLimit = 20;
         break;
      case HPD3010:
         maxVoltage = 30;
         OVPMaxVoltage = 33;
         currentLimit = 10;
         break;
      case HPD605:
         maxVoltage = 60;
         OVPMaxVoltage = 66;
         currentLimit = 5;
         break;
      /* XT 60W Models */
      case XT76:
         maxVoltage = 7;
         OVPMaxVoltage = 7.7;
         currentLimit = 6;
         break;
      case XT154:
         maxVoltage = 15;
         OVPMaxVoltage = 16.5;
         currentLimit = 4;
         break;
      case XT203:
         maxVoltage = 20;
         OVPMaxVoltage = 22;
         currentLimit = 3;
         break;
      case XT302:
         maxVoltage = 30;
         OVPMaxVoltage = 33;
         currentLimit = 2;
         break;
      case XT601:
         maxVoltage = 60;
         OVPMaxVoltage = 66;
         currentLimit = 1;
         break;
      case XT1200_5:
         maxVoltage = 120;
         OVPMaxVoltage = 132;
         currentLimit = 0.5;
         break;
      case XT2500_25:
         maxVoltage = 250;
         OVPMaxVoltage = 275;
         currentLimit = 0.25;
         break;
      default:
         Ivi_SetErrorInfo (vi, VI_FALSE, XANM9B_MODEL_NOT_FOUND, VI_SUCCESS,
                       "The instrument you are trying to Initialize has not been defined for this Instrument Driver");
         viCheckErr(XANM9B_MODEL_NOT_FOUND);
         break;
      }
 
   checkErr( Ivi_RangeTableNew (vi, 2, 1, VI_TRUE, VI_TRUE, &voltageRangeTablePtr));
   viCheckErr( Ivi_SetRangeTableEntry (voltageRangeTablePtr, 0, -maxVoltage, maxVoltage, 0.0, VI_NULL, 0));

   checkErr( Ivi_RangeTableNew (vi, 2, 1, VI_TRUE, VI_TRUE, &ovpRangeTablePtr));
   viCheckErr( Ivi_SetRangeTableEntry (ovpRangeTablePtr, 0, OVPMinVoltage, OVPMaxVoltage, 0.0, VI_NULL, 0));

   checkErr( Ivi_RangeTableNew (vi, 2, 1, VI_TRUE, VI_TRUE, &currentRangeTablePtr));
   viCheckErr( Ivi_SetRangeTableEntry (currentRangeTablePtr, 0, 0.0, currentLimit, 0.0, VI_NULL, 0));
    
   /*- Initialize instrument attributes --------------------------------*/            
   checkErr( Ivi_SetAttributeViInt32 (vi, "", XANM9B_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION,
                                      0, XANM9B_MAJOR_VERSION));
                                       
   checkErr( Ivi_SetAttributeViInt32 (vi, "", XANM9B_ATTR_SPECIFIC_DRIVER_MINOR_VERSION,
                                      0, XANM9B_MINOR_VERSION));
                                       
   checkErr( Ivi_SetAttrReadCallbackViString (vi, XANM9B_ATTR_SPECIFIC_DRIVER_REVISION,
                                               XanM9BAttrDriverRevision_ReadCallback));
                                               
   checkErr( Ivi_SetAttributeViInt32 (vi, "", XANM9B_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION,
                                      0, XANM9B_CLASS_SPEC_MAJOR_VERSION));
                                       
   checkErr( Ivi_SetAttributeViInt32 (vi, "", XANM9B_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION,
                                      0, XANM9B_CLASS_SPEC_MINOR_VERSION));
                                       
   checkErr( Ivi_SetAttributeViString (vi, "", XANM9B_ATTR_IO_SESSION_TYPE,
                                       0, XANM9B_IO_SESSION_TYPE));
                                        
   checkErr( Ivi_SetAttributeViString (vi, "", XANM9B_ATTR_SUPPORTED_INSTRUMENT_MODELS,
                                       0, XANM9B_SUPPORTED_INSTRUMENT_MODELS));
                                        
   checkErr( Ivi_SetAttrReadCallbackViString (vi, XANM9B_ATTR_INSTRUMENT_FIRMWARE_REVISION,
                                              XanM9BAttrInstrumentFirmwareRevision_ReadCallback));
                                               
   checkErr( Ivi_GetAttributeFlags (vi, XANM9B_ATTR_INSTRUMENT_MANUFACTURER, &flags));
     
   checkErr( Ivi_SetAttributeFlags (vi, XANM9B_ATTR_INSTRUMENT_MANUFACTURER, 
                                    flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
                                    
   checkErr( Ivi_SetAttrReadCallbackViString (vi, XANM9B_ATTR_INSTRUMENT_MANUFACTURER,
                                              XanM9BAttrInstrumentManufacturer_ReadCallback));
                                               
   checkErr( Ivi_GetAttributeFlags (vi, XANM9B_ATTR_INSTRUMENT_MODEL, &flags));
     
   checkErr( Ivi_SetAttributeFlags (vi, XANM9B_ATTR_INSTRUMENT_MODEL, 
                                    flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
                                     
   checkErr( Ivi_SetAttrReadCallbackViString (vi, XANM9B_ATTR_INSTRUMENT_MODEL,
                                              XanM9BAttrInstrumentModel_ReadCallback));
                                               
   checkErr( Ivi_SetAttributeViString (vi, "", XANM9B_ATTR_SPECIFIC_DRIVER_VENDOR,
                                       0, XANM9B_DRIVER_VENDOR));
                                        
   checkErr( Ivi_SetAttributeViString (vi, "", XANM9B_ATTR_SPECIFIC_DRIVER_DESCRIPTION,
                                       0, XANM9B_DRIVER_DESCRIPTION));
                                        
   checkErr( Ivi_SetAttributeViAddr (vi, VI_NULL, IVI_ATTR_CHECK_STATUS_CALLBACK, 0,
                                     XanM9B_CheckStatusCallback));
                                      
   checkErr( Ivi_SetAttributeViBoolean (vi, VI_NULL, IVI_ATTR_SUPPORTS_WR_BUF_OPER_MODE, 
                                        0, VI_TRUE));
                                         
   checkErr( Ivi_SetAttributeViString (vi, "", XANM9B_ATTR_GROUP_CAPABILITIES, 0,
                                       "IviDCPwrBase,"
                                       "IviDCPwrTrigger,"
                                       "IviDCPwrSoftwareTrigger,"
                                       "IviDCPwrMeasurement"));

   /*- Instrument-specific attributes ------------------------------*/            
   checkErr (Ivi_AddAttributeViBoolean (vi, XANM9B_ATTR_OUTPUT_ENABLED,
                                        "XANM9B_ATTR_OUTPUT_ENABLED", VI_TRUE,
                                        IVI_VAL_MULTI_CHANNEL,
                                        XanM9BAttrOutputEnabled_ReadCallback,
                                        XanM9BAttrOutputEnabled_WriteCallback));

   checkErr (Ivi_AddAttributeViBoolean (vi, XANM9B_ATTR_AUXA_ENABLED,
                                        "XANM9B_ATTR_AUXA_ENABLED", VI_TRUE,
                                        IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
                                        XanM9BAttrAuxaEnabled_ReadCallback,
                                        XanM9BAttrAuxaEnabled_WriteCallback));
                                      
   checkErr (Ivi_AddAttributeViBoolean (vi, XANM9B_ATTR_SRQ_ENABLED,
                                        "XANM9B_ATTR_SRQ_ENABLED", VI_TRUE,
                                        IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
                                        XanM9BAttrSrqEnabled_ReadCallback,
                                        XanM9BAttrSrqEnabled_WriteCallback));
                                        
   checkErr (Ivi_AddAttributeViBoolean (vi, XANM9B_ATTR_AUXB_ENABLED,
                                        "XANM9B_ATTR_AUXB_ENABLED", VI_TRUE,
                                        IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
                                        XanM9BAttrAuxbEnabled_ReadCallback,
                                        XanM9BAttrAuxbEnabled_WriteCallback));
                                                  
   checkErr( Ivi_AddAttributeViBoolean(vi,XANM9B_ATTR_CMODE_ENABLED,
                                       "XANM9B_ATTR_CMODE_ENABLED",VI_TRUE,
                                       IVI_VAL_MULTI_CHANNEL|IVI_VAL_NEVER_CACHE,
                                       XanM9BAttrCmodeEnabled_ReadCallback,
                                       XanM9BAttrCmodeEnabled_WriteCallback));
    
   checkErr( Ivi_AddAttributeViBoolean(vi,XANM9B_ATTR_HOLD_ENABLED,
                                       "XANM9B_ATTR_HOLD_ENABLED",VI_TRUE,
                                       IVI_VAL_MULTI_CHANNEL|IVI_VAL_NEVER_CACHE,
                                       XanM9BAttrHoldEnabled_ReadCallback,
                                       XanM9BAttrHoldEnabled_WriteCallback));
   
   checkErr( Ivi_AddAttributeViReal64 (vi, XANM9B_ATTR_VOLTAGE_LEVEL,
                                       "XANM9B_ATTR_VOLTAGE_LEVEL", 0.0,
                                       IVI_VAL_MULTI_CHANNEL,
                                       XanM9BAttrVoltageLevel_ReadCallback,
                                       XanM9BAttrVoltageLevel_WriteCallback,
                                       voltageRangeTablePtr, 4));
    
   checkErr (Ivi_AddAttributeViReal64 (vi, XANM9B_ATTR_OVP_LIMIT,
                                       "XANM9B_ATTR_OVP_LIMIT", 0,
                                       IVI_VAL_MULTI_CHANNEL,
                                       XanM9BAttrOVPLimit_ReadCallback,
                                       XanM9BAttrOVPLimit_WriteCallback,
                                       ovpRangeTablePtr, 4));
                              
   checkErr (Ivi_AddAttributeViReal64 (vi, XANM9B_ATTR_CURRENT_LIMIT,
                                       "XANM9B_ATTR_CURRENT_LIMIT", 0.0,
                                       IVI_VAL_MULTI_CHANNEL,
                                       XanM9BAttrCurrentLimit_ReadCallback,
                                       XanM9BAttrCurrentLimit_WriteCallback,
                                       currentRangeTablePtr, 4));
    
   checkErr (Ivi_AddAttributeViReal64 (vi, XANM9B_ATTR_MAXIMUM_CURRENT_LIMIT,
                                       "XANM9B_ATTR_MAXIMUM_CURRENT_LIMIT", 0.0,
                                       IVI_VAL_MULTI_CHANNEL,
                                       XanM9BAttrMaximumCurrentLimit_ReadCallback,
                                       XanM9BAttrMaximumCurrentLimit_WriteCallback,
                                       currentRangeTablePtr, 4));

   checkErr (Ivi_AddAttributeViReal64 (vi, XANM9B_ATTR_MAXIMUM_VOLTAGE,
                                       "XANM9B_ATTR_MAXIMUM_VOLTAGE", 0.0,
                                       IVI_VAL_MULTI_CHANNEL,
                                       XanM9BAttrMaximumVoltage_ReadCallback,
                                       XanM9BAttrMaximumVoltage_WriteCallback,
                                       voltageRangeTablePtr, 4));
      
   checkErr (Ivi_AddAttributeViReal64 (vi, XANM9B_ATTR_TRIGGERED_VOLTAGE_LEVEL,
                                       "XANM9B_ATTR_TRIGGERED_VOLTAGE_LEVEL", 0.0,
                                       IVI_VAL_MULTI_CHANNEL,
                                       XanM9BAttrTrigVoltageLevel_ReadCallback,
                                       XanM9BAttrTrigVoltageLevel_WriteCallback,
                                       voltageRangeTablePtr, 4));
            
   checkErr( Ivi_AddAttributeViReal64 (vi, XANM9B_ATTR_TRIGGERED_CURRENT_LIMIT,
                                       "XANM9B_ATTR_TRIGGERED_CURRENT_LIMIT", 0.1,
                                       IVI_VAL_MULTI_CHANNEL,
                                       XanM9BAttrTrigCurrentLimit_ReadCallback,
                                       XanM9BAttrTrigCurrentLimit_WriteCallback,
                                       currentRangeTablePtr, 4));

   checkErr (Ivi_AddAttributeViString (vi, XANM9B_ATTR_ID_QUERY_RESPONSE,
                                       "XANM9B_ATTR_ID_QUERY_RESPONSE", "I",
                                       IVI_VAL_NOT_USER_WRITABLE,
                                       XanM9BAttrIdQueryResponse_ReadCallback,
                                       VI_NULL));
                                                    
   checkErr( Ivi_AddAttributeViInt32 (vi, XANM9B_ATTR_OPC_TIMEOUT,
                                      "XANM9B_ATTR_OPC_TIMEOUT",
                                      5000, IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                      VI_NULL, VI_NULL, VI_NULL)); 
                                   
   checkErr (Ivi_AddAttributeViReal64 (vi, XANM9B_ATTR_FAULT_DELAY,
                                       "XANM9B_ATTR_FAULT_DELAY", 0.0,
                                       IVI_VAL_MULTI_CHANNEL,
                                       XanM9BAttrFaultDelay_ReadCallback,
                                       XanM9BAttrFaultDelay_WriteCallback,
                                       &attrFaultDelayRangeTable, 0));   
    
   checkErr (Ivi_AddAttributeViInt32 (vi, XANM9B_ATTR_FOLDBACK_SETTING,
                                      "XANM9B_ATTR_FOLDBACK_SETTING", 0,
                                      IVI_VAL_MULTI_CHANNEL,
                                      XanM9BAttrFoldbackSetting_ReadCallback,
                                      XanM9BAttrFoldbackSetting_WriteCallback,
                                      &attrFoldbackSettingRangeTable));
                   
   checkErr (Ivi_AddAttributeViInt32 (vi, XANM9B_ATTR_UNMASK_CONDITION,
                                      "XANM9B_ATTR_UNMASK_CONDITION", 0,
                                      IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
                                      XanM9BAttrUnmaskCondition_ReadCallback,
                                      VI_NULL, VI_NULL));
  
   checkErr (Ivi_AddAttributeViString (vi, XANM9B_ATTR_MASK_SETTING,
                                       "XANM9B_ATTR_MASK_SETTING", "NONE",
                                       IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
                                       VI_NULL, XanM9BAttrMaskSetting_WriteCallback));
                                        
   checkErr (Ivi_AddAttributeViString (vi, XANM9B_ATTR_UNMASK_SETTING,
                                       "XANM9B_ATTR_UNMASK_SETTING", "NONE",
                                       IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
                                       VI_NULL,
                                       XanM9BAttrUnmaskSetting_WriteCallback));
                                         
   checkErr (Ivi_AddAttributeViInt32 (vi, XANM9B_ATTR_INSTR_STATUS_REG,
                                      "XANM9B_ATTR_INSTR_STATUS_REG", 0,
                                      IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE | IVI_VAL_HIDDEN,
                                      XanM9BAttrInstrStatusReg_ReadCallback,
                                      VI_NULL, VI_NULL));
                                      
   checkErr (Ivi_AddAttributeViInt32 (vi, XANM9B_ATTR_CURRENT_LIMIT_BEHAVIOR,
                                      "XANM9B_ATTR_CURRENT_LIMIT_BEHAVIOR",
                                      XANM9B_VAL_CURRENT_REGULATE,
                                      IVI_VAL_MULTI_CHANNEL | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION | IVI_VAL_DONT_CHECK_STATUS,
                                      XanM9BAttrCurrentLimitBehavior_ReadCallback,
                                      XanM9BAttrCurrentLimitBehavior_WriteCallback,
                                      &attrCurrentLimitBehaviorRangeTable));
                                      
   checkErr (Ivi_AddAttributeViBoolean (vi, XANM9B_ATTR_OVP_ENABLED,
                                        "XANM9B_ATTR_OVP_ENABLED", VI_TRUE,
                                        IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION | IVI_VAL_DONT_CHECK_STATUS,
                                        XanM9BAttrOVPEnabled_ReadCallback,
                                        XanM9BAttrOVPEnabled_WriteCallback));
                                        
   checkErr (Ivi_AddAttributeViInt32 (vi, XANM9B_ATTR_TRIGGER_SOURCE,
                                      "XANM9B_ATTR_TRIGGER_SOURCE",
                                      XANM9B_VAL_TRIG_IMMEDIATE,
                                      IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION | IVI_VAL_DONT_CHECK_STATUS,
                                      XanM9BAttrTriggerSource_ReadCallback,
                                      XanM9BAttrTriggerSource_WriteCallback,
                                      VI_NULL));
                                      
   checkErr (Ivi_AddAttributeViInt32 (vi, XANM9B_ATTR_REMOTE_MODE,
                                      "XANM9B_ATTR_REMOTE_MODE", XANM9B_VAL_REM, 0,
                                      XanM9BAttrRemoteMode_ReadCallback,
                                      XanM9BAttrRemoteMode_WriteCallback,
                                      &attrRemoteModeRangeTableXFR));
    
   checkErr (Ivi_AddAttributeInvalidation (vi, XANM9B_ATTR_MASK_SETTING,
                                           XANM9B_ATTR_UNMASK_CONDITION, VI_TRUE));
                                           
   checkErr (Ivi_AddAttributeInvalidation (vi, XANM9B_ATTR_MASK_SETTING,
                                           XANM9B_ATTR_UNMASK_SETTING, VI_TRUE));
                                           
   checkErr (Ivi_AddAttributeInvalidation (vi, XANM9B_ATTR_UNMASK_SETTING,
                                           XANM9B_ATTR_UNMASK_CONDITION, VI_TRUE));
                                           
   checkErr (Ivi_AddAttributeInvalidation (vi, XANM9B_ATTR_UNMASK_SETTING,
                                           XANM9B_ATTR_MASK_SETTING, VI_TRUE));
Error:
    return error;
}

/*****************************************************************************
 *------------------- End Instrument Driver Source Code ---------------------*
 *****************************************************************************/
