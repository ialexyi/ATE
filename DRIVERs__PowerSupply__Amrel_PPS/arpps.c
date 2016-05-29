/***************************************************************************** 
 *  (c)1999-2013, National Instruments, Corporation.  All Rights Reserved.    * 
 *****************************************************************************/
/*****************************************************************************
 *  American Reliance PPS Power Supply Instrument Driver                               
 *  LabWindows/CVI 5.0 Instrument Driver                                     
 *  Original Release: 9.12.1999                                  
 *  By: Zdenek Rykala, ELCOM, a.s.                              
 *      PH. +420 69 6996146   Fax +420 69 6996147                              
 *                                                                           
 *  Modification History:                                                    
 *                                                                           
 *       9.12.1999 - [ZR] Instrument Driver Created.                  
 *       3.06.2000 - [pp] Removed code in error_query, fixed some typos,
 *                   modified arpps_CheckStatusCallback
 *                   and OutputEnabledReadCallback
 *       3.4.2001  - [ZR] Driver changed to accord with IVI-3.2 specification
 *                      It's include following changes:
 *                      Added functions:
 *                         arpps_InvalidateAllAttributes
 *                         arpps_ResetWithDefaults
 *                         arpps_Disable
 *                         arpps_GetNextInterchangeWarning
 *                         arpps_ResetInterchangeCheck
 *                         arpps_ClearInterchangeWarning
 *                         arpps_GetError
 *                         arpps_ClearError
 *                      Added attributes:
 *                         ARPPS_ATTR_DRIVER_SETUP
 *                         ARPPS_ATTR_INTERCHANGE_CHECK
 *                      Renamed
 *                         ARPPS_ATTR_QUERY_INSTR_STATUS to ARPPS_ATTR_QUERY_INSTRUMENT_STATUS
 *                         ARPPS_ATTR_RESOURCE_DESCRIPTOR to ARPPS_ATTR_IO_RESOURCE_DESCRIPTOR
 *                         ARPPS_ATTR_NUM_CHANNELS to ARPPS_ATTR_CHANNEL_COUNT
 *                      Fixed
 *                         arpps_revision_query
 *                           (changed value for buffer size form -1 to 256)
 *                      Changed:
 *                         arpps_InitWithOptions changed type of optionsString
 *                                                parameter to ViConstString
 *       5.28.2003 - [AL] Set attribute IVI_ATTR_IO_RESOURCE_DESCRIPTOR 
 *                        in initialize time.
 *		 6.13.2003 - [VJ] Fixed the uninitialized error variable.
 *       8.06.2013 - [XY] Porting driver to 64bit.
 *****************************************************************************/

#include <string.h>
#include <stdio.h>  
#include <ctype.h>
#include "arpps.h"

/*****************************************************************************
 *--------------------- Hidden Attribute Declarations -----------------------*
 *****************************************************************************/
#define ARPPS_ATTR_MEAS_IOUT                (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 1L)
#define ARPPS_ATTR_MEAS_VOUT                (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 2L)

/*****************************************************************************
 *---------------------------- Useful Macros --------------------------------*
 *****************************************************************************/

    /*- I/O buffer size -----------------------------------------------------*/
#define BUFFER_SIZE                             512L

    /*- DriverSetup Options -------------------------------------------------*/
#define SETUP_MODEL_TAG                         "Model"
#define ARPPS_VAL_INSTRUMENT_MANUFACTURER       "American Reliance"

    /*- Status Register Bits ------------------------------------------------*/
#define ARPPS_CH1_OUTPUT_OFF_BIT          (1L << 1)
#define ARPPS_CH1_OCP_MODE_BIT            (1L << 2)
#define ARPPS_CH1_OVER_CURRENT_BIT        (1L << 3)
#define ARPPS_CH1_OVER_VOLTAGE_BIT        (1L << 4)
#define ARPPS_CH1_CONSTANT_CURRENT_BIT    (1L << 5)

    /*- arpps Family Member Capabilites -------------------------------------*/
/* Family members */
enum
{
    PPS8_10 = 0,        /* single-output, single-range */
    PPS18_4,            /* single-output, single-range */
    PPS30_25,           /* single-output, single-range */
    PPS35_2,            /* single-output, single-range */
    PPS60_10,           /* single-output, single-range */
    PPS128_5,           /* single-output, single-range */
    PPS250_2,           /* single-output, single-range */
    PPS5_30,            /* single-output, single-range */
    PPS35_3R,           /* single-output, dual-range   */
    PPS60_15R,          /* single-output, dual-range   */
    PPS8_6D,            /* dual-output,   single-range */
    PPS18_4D,           /* dual-output,   single-range */
    PPS35_2D,           /* dual-output,   single-range */
    PPS30_3D,           /* dual-output,   single-range */
    PPS60_1D,           /* dual-output,   single-range */
    PPS12805D,          /* dual-output,   single-range */
    PPS30_6,            /* single-output, single-range */
    PPS35_5,            /* single-output, single-range */
    PPS8_20,            /* single-output, single-range */
    PPS18_10,           /* single-output, single-range */
    PPS60_3,            /* single-output, single-range */
    PPS128_15,          /* single-output, single-range */
    PPS250_08,          /* single-output, single-range */
	PPS1202,   			/* dual-output,   single-range */   
    NUMBER_OF_MODELS
};  

#define ARPPS_MODEL_NOT_FOUND           -1

static IviStringValueTable arpps_ModelTable =
{
    {PPS8_10,   "PPS8-10"   },      
    {PPS18_4,   "PPS18-4"   },
    {PPS30_25,  "PPS30-2.5" },
    {PPS35_2,   "PPS35-2"   },
    {PPS60_10,  "PPS60-10"  },
    {PPS128_5,  "PPS128-0.5"},
    {PPS250_2,  "PPS250-0.2"},
    {PPS5_30,   "PPS5-30"   },
    {PPS35_3R,  "PPS35-3R"  },
    {PPS60_15R, "PPS60-1.5R"},
    {PPS8_6D,   "PPS8-6D"   },
    {PPS18_4D,  "PPS18-4D"  },
    {PPS35_2D,  "PPS35-2D"  },
    {PPS30_3D,  "PPS30-3D"  },
    {PPS60_1D,  "PPS60-1D"  },
    {PPS12805D, "PPS128.05D"},
    {PPS30_6,   "PPS30-6"   },
    {PPS35_5,   "PPS35-5"   },
    {PPS8_20,   "PPS8-20"   },
    {PPS18_10,  "PPS18-10"  },
    {PPS60_3,   "PPS60-3"   },
    {PPS128_15, "PPS128-1.5"},
    {PPS250_08, "PPS250-0.8"},
	{PPS1202,	"PPS-1202"  },
    {VI_NULL,   VI_NULL}
};

static ViReal64 arpps_VoltageLimits[NUMBER_OF_MODELS] =
{
    8.0,    /* PPS8-10    */
    18.0,   /* PPS18-4    */
    30.0,   /* PPS30-2.5  */
    35.0,   /* PPS35-2    */
    60.0,   /* PPS60-10   */
    128.0,  /* PPS128-0.5 */
    250.0,  /* PPS250-0.2 */
    5.0,    /* PPS5-30    */
    35.0,   /* PPS35-3R   */
    60.0,   /* PPS60-1.5R */
    8.0,    /* PPS8-6D    */
    18.0,   /* PPS18-4D   */
    35.0,   /* PPS35-2D   */
    30.0,   /* PPS30-3D   */
    60.0,   /* PPS60-1D   */
    128.0,  /* PPS128.05D */
    30.0,   /* PPS30-6    */
    35.0,   /* PPS35-5    */
    8.0,    /* PPS8-20    */
    18.0,   /* PPS18-10   */
    60.0,   /* PPS60-3    */
    128.0,  /* PPS128-1.5 */
    250.0,  /* PPS250-0.8 */
	15.0, 	/* PPS-1202	  */ 
};

static ViReal64 arpps_CurrentLimits[NUMBER_OF_MODELS] =
{
    10.0,   /* PPS8-10    */
    4.0,    /* PPS18-4    */
    2.5,    /* PPS30-2.5  */
    2.0,    /* PPS35-2    */
    1.0,    /* PPS60-10   */
    0.5,    /* PPS128-0.5 */
    0.2,    /* PPS250-0.2 */
    30.0,   /* PPS5-30    */
    6.0,    /* PPS35-3R   */
    3.0,    /* PPS60-1.5R */
    6.0,    /* PPS8-6D    */
    4.0,    /* PPS18-4D   */
    2.0,    /* PPS35-2D   */
    3.0,    /* PPS30-3D   */
    1.0,    /* PPS60-1D   */
    0.5,    /* PPS128.05D */
    6.0,    /* PPS30-6    */
    5.0,    /* PPS35-5    */
    20.0,   /* PPS8-20    */
    10.0,   /* PPS18-10   */
    3.0,    /* PPS60-3    */
    1.5,    /* PPS128-1.5 */
    0.8,    /* PPS250-0.8 */
	4.0,    /* PPS-1202   */ 
};

    /*- List of channels passed to the Ivi_BuildChannelTable function -------*/ 
#define SINGLE_CHANNEL_LIST      "1"
#define DUAL_CHANNEL_LIST        "1,2"

/*****************************************************************************
 *-------------- Utility Function Declarations (Non-Exported) ---------------*
 *****************************************************************************/
static ViStatus arpps_GetDriverSetupOption (ViSession vi, ViInt32 *modelType);
static void RemoveSurroundingWhiteSpace(ViChar string[]);
static ViStatus arpps_QueryID (ViSession vi, ViSession io, ViInt32 bufferSize,
                               ViChar buffer[]);
static ViStatus arpps_CheckStatusBit (ViSession vi, ViSession io,
                                      ViConstString ch, ViInt32 mask, 
                                      ViBoolean* value);
static ViStatus arpps_InitAttributes (ViSession vi, ViInt32 modelType, ViBoolean isDual);
static ViStatus arpps_SetSpecificCallbacks (ViSession vi, ViBoolean isDual);
static ViStatus arpps_DefaultInstrSetup (ViSession openInstrSession);
static ViStatus arpps_CheckStatus (ViSession vi);
static ViStatus arpps_WriteSingleInt (ViSession vi, ViSession io,
                                      ViConstString cmd,
                                      ViInt32 value);
static ViStatus arpps_WriteDualInt (ViSession vi, ViSession io,
                                    ViConstString channelName, 
                                    ViConstString cmd,
                                    ViInt32 value);
static ViStatus arpps_WriteSingleDouble (ViSession vi, ViSession io,
                                      ViConstString cmd,
                                      ViReal64 value);
static ViStatus arpps_WriteDualDouble (ViSession vi, ViSession io,
                                      ViConstString channelName, 
                                      ViConstString cmd,
                                      ViReal64 value);
static ViStatus arpps_ReadSingleDouble (ViSession vi, ViSession io,
                                      ViConstString cmd,
                                      ViReal64* value);
static ViStatus arpps_ReadDualDouble (ViSession vi, ViSession io,
                                      ViConstString channelName, 
                                      ViConstString cmd,
                                      ViReal64* value);
static ViStatus arpps_GetSimData(ViSession vi, ViConstString channelName,
                                 ViAttr attrId, ViReal64 *value);
/*****************************************************************************
 *----------------- Callback Declarations (Non-Exported) --------------------*
 *****************************************************************************/

    /*- Global Session Callbacks --------------------------------------------*/
static ViStatus _VI_FUNC arpps_CheckStatusCallback (ViSession vi, ViSession io);

    /*- Attribute callbacks -------------------------------------------------*/
static ViStatus _VI_FUNC arppsAttrSpecificDriverRevision_ReadCallback (ViSession vi,
                                                                  ViSession io, 
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId, 
                                                                  const ViConstString cacheValue);
static ViStatus _VI_FUNC arppsAttrIdQueryResponse_ReadCallback (ViSession vi,
                                                                   ViSession io, 
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId, 
                                                                   const ViConstString cacheValue);
static ViStatus _VI_FUNC arppsSingleOutputEnabled_WriteCallback (ViSession vi, 
                                                                 ViSession io,
                                                                 ViConstString channelName, 
                                                                 ViAttr attributeId,
                                                                 ViBoolean value);
static ViStatus _VI_FUNC arppsDualOutputEnabled_WriteCallback (ViSession vi, 
                                                               ViSession io,
                                                               ViConstString channelName, 
                                                               ViAttr attributeId,
                                                               ViBoolean value);
static ViStatus _VI_FUNC arppsAttrOutputEnabled_ReadCallback (ViSession vi, 
                                                              ViSession io,
                                                              ViConstString channelName, 
                                                              ViAttr attributeId,
                                                              ViBoolean* value);
static ViStatus _VI_FUNC arppsSingleVoltageLevel_WriteCallback (ViSession vi, 
                                                                ViSession io,
                                                                ViConstString channelName, 
                                                                ViAttr attributeId,
                                                                ViReal64 value);
static ViStatus _VI_FUNC arppsDualVoltageLevel_WriteCallback (ViSession vi, 
                                                                ViSession io,
                                                                ViConstString channelName, 
                                                                ViAttr attributeId,
                                                                ViReal64 value);
static ViStatus _VI_FUNC arppsSingleVoltageLevel_ReadCallback (ViSession vi, 
                                                               ViSession io,
                                                               ViConstString channelName, 
                                                               ViAttr attributeId,
                                                               ViReal64* value);
static ViStatus _VI_FUNC arppsDualVoltageLevel_ReadCallback (ViSession vi, 
                                                               ViSession io,
                                                               ViConstString channelName, 
                                                               ViAttr attributeId,
                                                               ViReal64* value);
static ViStatus _VI_FUNC arppsSingleOVPEnabled_WriteCallback (ViSession vi, 
                                                              ViSession io,
                                                              ViConstString channelName, 
                                                              ViAttr attributeId,
                                                              ViBoolean value);
static ViStatus _VI_FUNC arppsDualOVPEnabled_WriteCallback (ViSession vi, 
                                                              ViSession io,
                                                              ViConstString channelName, 
                                                              ViAttr attributeId,
                                                              ViBoolean value);
static ViStatus _VI_FUNC arppsSingleOVPLimit_WriteCallback (ViSession vi, 
                                                            ViSession io,
                                                            ViConstString channelName, 
                                                            ViAttr attributeId,
                                                            ViReal64 value);
static ViStatus _VI_FUNC arppsDualOVPLimit_WriteCallback (ViSession vi, 
                                                            ViSession io,
                                                            ViConstString channelName, 
                                                            ViAttr attributeId,
                                                            ViReal64 value);
static ViStatus _VI_FUNC arppsSingleOVPLimit_ReadCallback (ViSession vi, 
                                                           ViSession io,
                                                           ViConstString channelName, 
                                                           ViAttr attributeId,
                                                           ViReal64* value);
static ViStatus _VI_FUNC arppsDualOVPLimit_ReadCallback (ViSession vi, 
                                                           ViSession io,
                                                           ViConstString channelName, 
                                                           ViAttr attributeId,
                                                           ViReal64* value);
static ViStatus _VI_FUNC arppsSingleCurrentLimitBehavior_WriteCallback (ViSession vi, 
                                                                        ViSession io,
                                                                        ViConstString channelName, 
                                                                        ViAttr attributeId,
                                                                        ViInt32 value);
static ViStatus _VI_FUNC arppsDualCurrentLimitBehavior_WriteCallback (ViSession vi, 
                                                                        ViSession io,
                                                                        ViConstString channelName, 
                                                                        ViAttr attributeId,
                                                                        ViInt32 value);
static ViStatus _VI_FUNC arppsAttrCurrentLimitBehavior_ReadCallback (ViSession vi, 
                                                                       ViSession io,
                                                                       ViConstString channelName, 
                                                                       ViAttr attributeId,
                                                                       ViInt32* value);
static ViStatus _VI_FUNC arppsSingleCurrentLimit_WriteCallback (ViSession vi, 
                                                                ViSession io,
                                                                ViConstString channelName, 
                                                                ViAttr attributeId,
                                                                ViReal64 value);
static ViStatus _VI_FUNC arppsDualCurrentLimit_WriteCallback (ViSession vi, 
                                                                ViSession io,
                                                                ViConstString channelName, 
                                                                ViAttr attributeId,
                                                                ViReal64 value);
static ViStatus _VI_FUNC arppsSingleCurrentLimit_ReadCallback (ViSession vi, 
                                                               ViSession io,
                                                               ViConstString channelName, 
                                                               ViAttr attributeId,
                                                               ViReal64* value);
static ViStatus _VI_FUNC arppsSingleMeasIout_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViReal64 *value);
static ViStatus _VI_FUNC arppsDualMeasIout_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViReal64 *value);
static ViStatus _VI_FUNC arppsSingleMeasVout_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViReal64 *value);
static ViStatus _VI_FUNC arppsDualMeasVout_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViReal64 *value);
static ViStatus _VI_FUNC arppsDualCurrentLimit_ReadCallback (ViSession vi, 
                                                               ViSession io,
                                                               ViConstString channelName, 
                                                               ViAttr attributeId,
                                                               ViReal64* value);

/*****************************************************************************
 *------------ User-Callable Functions (Exportable Functions) ---------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: arpps_init   
 * Purpose:  VXIplug&play required function.  Calls the   
 *           arpps_InitWithOptions function.   
 *****************************************************************************/
ViStatus _VI_FUNC arpps_init (ViRsrc resourceName, ViBoolean IDQuery,
                                 ViBoolean resetDevice, ViSession *newVi)
{   
    ViStatus    error = VI_SUCCESS;

    if (newVi == VI_NULL)
        {
        Ivi_SetErrorInfo (VI_NULL, VI_FALSE, IVI_ERROR_INVALID_PARAMETER, 
                          VI_ERROR_PARAMETER4, "Null address for Instrument Handle");
        checkErr( IVI_ERROR_INVALID_PARAMETER);
        }

    checkWarn( arpps_InitWithOptions (resourceName, IDQuery, resetDevice, 
                                        "", newVi));

Error:  
    return error;
}

/*****************************************************************************
 * Function: arpps_InitWithOptions                                       
 * Purpose:  This function creates a new IVI session and calls the 
 *           IviInit function.                                     
 *****************************************************************************/
ViStatus _VI_FUNC arpps_InitWithOptions (ViRsrc resourceName, ViBoolean IDQuery,
                                            ViBoolean resetDevice, ViConstString optionString, 
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
    
    checkErr( Ivi_GetInfoFromResourceName (resourceName, (ViString )optionString, newResourceName,
                                           newOptionString, &isLogicalName));
    
        /* create a new interchangeable driver */
    checkErr( Ivi_SpecificDriverNew ("arpps", newOptionString, &vi));  
    if (!isLogicalName)
    {
        ViInt32 oldFlag = 0;
        
        checkErr (Ivi_GetAttributeFlags (vi, IVI_ATTR_IO_RESOURCE_DESCRIPTOR, &oldFlag));
        checkErr (Ivi_SetAttributeFlags (vi, IVI_ATTR_IO_RESOURCE_DESCRIPTOR, oldFlag & 0xfffb | 0x0010));
        checkErr (Ivi_SetAttributeViString (vi, "", IVI_ATTR_IO_RESOURCE_DESCRIPTOR, 0, newResourceName));
    }
        /* init the driver */
    checkWarn( arpps_IviInit (newResourceName, IDQuery, resetDevice, vi)); 
    if (isLogicalName)
        checkWarn( Ivi_ApplyDefaultSetup (vi));
    *newVi = vi;
    
Error:
    if (error < VI_SUCCESS) 
        Ivi_Dispose (vi);
        
    return error;
}

/*****************************************************************************
 * Function: arpps_IviInit                                                       
 * Purpose:  This function is called by arpps_InitWithOptions  
 *           or by an IVI class driver.  This function initializes the I/O 
 *           interface, optionally resets the device, optionally performs an
 *           ID query, and sends a default setup to the instrument.                
 *****************************************************************************/
ViStatus _VI_FUNC arpps_IviInit (ViRsrc resourceName, ViBoolean IDQuery,
                                    ViBoolean reset, ViSession vi)
{
    ViStatus    error = VI_SUCCESS;
    ViSession   io = VI_NULL;
    ViChar      rdBuffer[BUFFER_SIZE];
    ViInt32     modelType;
    ViBoolean   isDual = VI_FALSE;

    rdBuffer[0] = '\0';

    checkErr( arpps_GetDriverSetupOption (vi, &modelType));

    if (!Ivi_Simulating(vi))
        {
        ViSession   rmSession = VI_NULL;

        /* Open instrument session */
        checkErr( Ivi_GetAttributeViSession (vi, VI_NULL, IVI_ATTR_VISA_RM_SESSION, 0,
                                             &rmSession)); 
        viCheckErr( viOpen (rmSession, resourceName, VI_NULL, VI_NULL, &io));
        /* io session owned by driver now */
        checkErr( Ivi_SetAttributeViSession (vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, io));  

        /* Configure VISA Formatted I/O */
        viCheckErr( viSetAttribute (io, VI_ATTR_TMO_VALUE, 5000 ));
        viCheckErr( viSetBuf (io, VI_READ_BUF | VI_WRITE_BUF, 4000));
        viCheckErr( viSetAttribute (io, VI_ATTR_WR_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS));
        viCheckErr( viSetAttribute (io, VI_ATTR_RD_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS));
        if (IDQuery && modelType == ARPPS_MODEL_NOT_FOUND)
            {
            checkErr( arpps_QueryID ( vi, io, BUFFER_SIZE, rdBuffer));
            viCheckErr( Ivi_GetValueFromTable (arpps_ModelTable, rdBuffer, &modelType));
            }
        }
    if (modelType == ARPPS_MODEL_NOT_FOUND)
        modelType = PPS35_2;

    if ((modelType==PPS8_6D) || (modelType==PPS18_4D) || (modelType==PPS35_2D) || (modelType==PPS30_3D) || 
        (modelType==PPS60_1D) || (modelType==PPS12805D) || (modelType==PPS1202))
        {
        isDual = VI_TRUE;
        }

    checkErr( Ivi_BuildChannelTable (vi, (isDual) ? DUAL_CHANNEL_LIST : SINGLE_CHANNEL_LIST,
                                     VI_FALSE, VI_NULL));
         /* Add attributes */
    checkErr( arpps_InitAttributes (vi, modelType, isDual));
    /*- Reset instrument ----------------------------------------------------*/
    if (reset) 
        error = arpps_reset (vi);
    else  /*- Send Default Instrument Setup ---------------------------------*/
        error = arpps_DefaultInstrSetup (vi);
    checkErr( error);

    /*- Identification Query ------------------------------------------------*/
    if (!Ivi_Simulating(vi) && IDQuery)
        {
        ViInt32 actModel = modelType;

        if (!rdBuffer[0])
            {
            checkErr( Ivi_GetAttributeViString (vi, VI_NULL, ARPPS_ATTR_ID_QUERY_RESPONSE, 0, BUFFER_SIZE, rdBuffer));
            viCheckErr( Ivi_GetValueFromTable (arpps_ModelTable, rdBuffer, &actModel));
            }
        else
            {
            checkErr( Ivi_SetAttributeViString (vi, VI_NULL, ARPPS_ATTR_ID_QUERY_RESPONSE, 
                                                IVI_VAL_DONT_MARK_AS_SET_BY_USER, rdBuffer));
            }
        if (actModel != modelType)
            viCheckErr( VI_ERROR_FAIL_ID_QUERY);
        }

    checkErr( arpps_CheckStatus (vi));
    
    if (reset && error == VI_SUCCESS )
        checkWarn( VI_WARN_NSUP_RESET);

Error:
    if (error < VI_SUCCESS)
        {
        if (!Ivi_Simulating (vi) && io)
            viClose (io);
        }
    return error;
}

/*****************************************************************************
 * Function: arpps_close                                                           
 * Purpose:  This function closes the instrument.                            
 *
 *           Note:  This function must unlock the session before calling
 *           Ivi_Dispose.
 *****************************************************************************/
ViStatus _VI_FUNC arpps_close (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));
    checkErr( arpps_IviClose (vi));

Error:    
    Ivi_UnlockSession (vi, VI_NULL);
    Ivi_Dispose (vi);  

    return error;
}

/*****************************************************************************
 * Function: arpps_IviClose                                                        
 * Purpose:  This function performs all of the drivers clean-up operations   
 *           except for closing the IVI session.  This function is called by 
 *           arpps_close or by an IVI class driver. 
 *
 *           Note:  This function must close the I/O session and set 
 *           IVI_ATTR_IO_SESSION to 0.
 *****************************************************************************/
ViStatus _VI_FUNC arpps_IviClose (ViSession vi)
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
 * Function: arpps_reset                                                         
 * Purpose:  This function resets the instrument.                          
 *****************************************************************************/
ViStatus _VI_FUNC arpps_reset (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    checkErr( arpps_DefaultInstrSetup (vi));
    if (error == VI_SUCCESS)
        viCheckWarn( VI_WARN_NSUP_RESET);

Error:
    Ivi_UnlockSession (vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: arpps_InvalidateAllAttributes
 * Purpose:  This function invalidates the cached value of all attributes.
 *****************************************************************************/
ViStatus _VI_FUNC arpps_InvalidateAllAttributes (ViSession vi)
{
    return Ivi_InvalidateAllAttributes(vi);
}

/**************************************************************************** 
 *  Function: arpps_GetChannelName
 *  Purpose:  This function returns the highest-level channel name that
 *            corresponds to the specific driver channel string that is in
 *            the channel table at an index you specify.
 ****************************************************************************/
ViStatus _VI_FUNC arpps_GetChannelName (ViSession vi, ViInt32 index,
                                           ViInt32 bufferSize,
                                           ViChar name[])
{
   ViStatus         error = VI_SUCCESS;
   ViConstString    channelName;
   ViInt32          actualSize = 0;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    
    if (bufferSize != 0)
        {
        if (name == VI_NULL)
            {
            viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Channel Name");
            }
        }
    
    /* Get channel name */
    viCheckParm( Ivi_GetNthChannelString (vi, index, &channelName), 2, "Index");
    
    actualSize = (ViInt32)strlen(channelName) + 1;
    
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
 * Function: arpps_ResetWithDefaults
 * Purpose:  This function resets the instrument and applies default settings
 *           from the IVI Configuration Store based on the logical name
 *           from which the session was created.
 *****************************************************************************/
ViStatus _VI_FUNC arpps_ResetWithDefaults (ViSession vi)
{
    ViStatus error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    checkErr( arpps_reset(vi));
    checkErr( Ivi_ApplyDefaultSetup(vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/**************************************************************************** 
 *  Function: arpps_Disable
 *  Purpose:  This function places the instrument in a quiescent state as 
 *            quickly as possible.
 ****************************************************************************/
ViStatus _VI_FUNC arpps_Disable (ViSession vi)
{
    return VI_SUCCESS;
}

/*****************************************************************************
 * Function: arpps_self_test                                                       
 * Purpose:  This function executes the instrument self-test and returns the 
 *           result.                                                         
 *****************************************************************************/
ViStatus _VI_FUNC arpps_self_test (ViSession vi, ViInt16 *testResult, 
                                      ViChar testMessage[])
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckWarn( VI_WARN_NSUP_SELF_TEST);

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: arpps_error_query                                                     
 * Purpose:  This function queries the instrument error queue and returns   
 *           the result.                                                     
 *****************************************************************************/
ViStatus _VI_FUNC arpps_error_query (ViSession vi, ViInt32 *errCode, 
                                        ViChar errMessage[])
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckWarn( VI_WARN_NSUP_ERROR_QUERY);

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: arpps_error_message                                                  
 * Purpose:  This function translates the error codes returned by this       
 *           instrument driver into user-readable strings.  
 *
 *           Note:  The caller can pass VI_NULL for the vi parameter.  This 
 *           is useful if one of the init functions fail.
 *****************************************************************************/
ViStatus _VI_FUNC arpps_error_message (ViSession vi, ViStatus errorCode,
                                          ViChar errorMessage[256])
{
    ViStatus    error = VI_SUCCESS;

    static      IviStringValueTable errorTable = 
        {
            {ARPPS_ERROR_RESET_PROTECTION_NOT_SUPPORTED, "Reset Output Protection is not supported."},
            IVIDCPWR_ERROR_CODES_AND_MSGS,
            {VI_NULL,                               VI_NULL}
        };
        
    if (vi)
        Ivi_LockSession(vi, VI_NULL);

        /* all VISA and IVI error codes are handled as well as codes in the table */
    if (errorMessage == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Error Message");

    checkErr( Ivi_GetSpecificDriverStatusDesc(vi, errorCode, errorMessage, errorTable));

Error:
    if (vi)
        Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: arpps_revision_query                                                  
 * Purpose:  This function returns the driver and instrument revisions.      
 *****************************************************************************/
ViStatus _VI_FUNC arpps_revision_query (ViSession vi, ViChar driverRev[], 
                                           ViChar instrRev[])
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (driverRev == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Driver Revision");

    checkErr( Ivi_GetAttributeViString (vi, VI_NULL, IVI_ATTR_SPECIFIC_DRIVER_REVISION, 
                                        0, 256, driverRev));

    viCheckWarn( VI_WARN_NSUP_REV_QUERY);

Error:    
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}
/*****************************************************************************
 * Function: arpps_ConfigureOutputEnabled                                   
 * Purpose:  This function controls whether or not the signal the power supply
 *           generates is present at the output terminal.
 *****************************************************************************/
ViStatus _VI_FUNC arpps_ConfigureOutputEnabled (ViSession vi,
                                                   ViConstString channelName,
                                                   ViBoolean enabled)
{
    return Ivi_SetAttributeViBoolean (vi, channelName, ARPPS_ATTR_OUTPUT_ENABLED,
                                      IVI_VAL_DIRECT_USER_CALL, enabled);
}

/*****************************************************************************
 * Function: arpps_ConfigureOutputRange                                   
 * Purpose:  This function determines the output range in which the power
 *           supply operates.
 *****************************************************************************/
ViStatus _VI_FUNC arpps_ConfigureOutputRange (ViSession vi,
                                              ViConstString channelName,
                                              ViInt32 rangeType,
                                              ViReal64 range)
{
    ViStatus    error = VI_SUCCESS;
    ViAttr      attrId = 0;

    checkErr( Ivi_LockSession(vi, VI_NULL));

    if (rangeType == ARPPS_VAL_RANGE_CURRENT)
        attrId = ARPPS_ATTR_CURRENT_LIMIT;
    else if (rangeType == ARPPS_VAL_RANGE_VOLTAGE)
        attrId = ARPPS_ATTR_VOLTAGE_LEVEL;
    else
        viCheckParm( IVI_ERROR_INVALID_VALUE, 3, "Range Type");

    viCheckParm( Ivi_CheckAttributeViReal64 (vi, channelName, attrId, 0, range),
                                             4, "Output Range");
    checkErr( arpps_CheckStatus (vi));

Error:    
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
            
}

/*****************************************************************************
 * Function: arpps_ConfigureCurrentLimit                                   
 * Purpose:  This function configures the behavior and level of the
 *           current limit.
 *****************************************************************************/
ViStatus _VI_FUNC arpps_ConfigureCurrentLimit (ViSession vi,
                                                  ViConstString channelName,
                                                  ViInt32 behavior,
                                                  ViReal64 limit)
{
    ViStatus error = VI_SUCCESS;
    ViBoolean behaviorFirst = VI_FALSE;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));
    
    //If you are setting the instrument to trip, set the limit and then the behavior
    //to prevent a premature over-current condition.
    
    behaviorFirst = (behavior != ARPPS_VAL_CURRENT_TRIP);
    
    if (behaviorFirst)
        {
        viCheckParm( Ivi_SetAttributeViInt32 (vi, channelName, 
                                              ARPPS_ATTR_CURRENT_LIMIT_BEHAVIOR,
                                              0, behavior), 3,
                                              "Current Limit Behavior");
        }
    viCheckParm( Ivi_SetAttributeViReal64 (vi, channelName, 
                                               ARPPS_ATTR_CURRENT_LIMIT,
                                               0, limit), 4,
                                               "Current Limit");
    if (!behaviorFirst)        
        {
        viCheckParm( Ivi_SetAttributeViInt32 (vi, channelName, 
                                              ARPPS_ATTR_CURRENT_LIMIT_BEHAVIOR,
                                              0, behavior), 3,
                                              "Current Limit Behavior");
        }
Error:    
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: arpps_ConfigureOVP                                   
 * Purpose:  This function enables or disables over-voltage protection and
 *           configures the over-voltage limit.
 *****************************************************************************/
ViStatus _VI_FUNC arpps_ConfigureOVP (ViSession vi,
                                         ViConstString channelName,
                                         ViBoolean enabled,
                                         ViReal64 limit)
{
    ViStatus error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));


    //If you are enabling over-voltage protection, set the limit first
    //to avoid a premature over-voltage condition.
    if (enabled)
        {
        viCheckParm( Ivi_SetAttributeViReal64 (vi, channelName, 
                                           ARPPS_ATTR_OVP_LIMIT,
                                           0, limit), 4, "OVP Limit");
        }
    viCheckParm( Ivi_SetAttributeViBoolean (vi, channelName, 
                                            ARPPS_ATTR_OVP_ENABLED,
                                            0, enabled), 3, "OVP Enabled");
Error:    
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: arpps_ConfigureVoltageLevel                                   
 * Purpose:  This function configures the voltage level the power supply
 *           attempts to generate.
 *****************************************************************************/
ViStatus _VI_FUNC arpps_ConfigureVoltageLevel (ViSession vi,
                                                  ViConstString channelName,
                                                  ViReal64 level)
{
    return Ivi_SetAttributeViReal64 (vi, channelName, ARPPS_ATTR_VOLTAGE_LEVEL,
                                     IVI_VAL_DIRECT_USER_CALL, level);
}

/*****************************************************************************
 * Function: arpps_QueryOutputState                                   
 * Purpose:  This function returns a value indicating if the power supply
 *           is in a particular output state.
 *****************************************************************************/
static IviRangeTableEntry outputStateRangeTableEntries[] =
    {
        {ARPPS_VAL_OUTPUT_CONSTANT_VOLTAGE, 0, 0, "", ARPPS_CH1_CONSTANT_CURRENT_BIT},
        {ARPPS_VAL_OUTPUT_CONSTANT_CURRENT, 0, 0, "", ARPPS_CH1_CONSTANT_CURRENT_BIT},
        {ARPPS_VAL_OUTPUT_OVER_VOLTAGE,     0, 0, "", ARPPS_CH1_OVER_VOLTAGE_BIT},
        {ARPPS_VAL_OUTPUT_OVER_CURRENT,     0, 0, "", ARPPS_CH1_OVER_CURRENT_BIT},
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

ViStatus _VI_FUNC arpps_QueryOutputState (ViSession vi,
                                          ViConstString channelName,
                                          ViInt32 outputState,
                                          ViBoolean* inState)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     mask = 0;
    ViBoolean   status = VI_FALSE;
    
    checkErr( Ivi_LockSession(vi, VI_NULL));

    if (Ivi_RangeChecking(vi))
        {
        viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL),
                                            2, "Channel Name");
        }

    viCheckParm( Ivi_GetViInt32EntryFromValue (outputState, &outputStateRangeTable,
                                               VI_NULL, VI_NULL, VI_NULL, VI_NULL,
                                               VI_NULL, &mask),
                                               3, "Output State");

    if (!Ivi_Simulating (vi))               /* call only when locked */
        {
        ViSession io = Ivi_IOSession (vi);  /* call only when locked */
        
        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
        checkErr( arpps_CheckStatusBit ( vi, io, channelName, mask, &status));
        }              
    /* If not constant current, then in constant voltage. */
    if (outputState == ARPPS_VAL_OUTPUT_CONSTANT_VOLTAGE)
        status = !status;
    checkErr( arpps_CheckStatus (vi));

Error:    
    if (inState) *inState = status;
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: arpps_QueryMaxCurrentLimit                  
 * Purpose:  This function returns the maximum programmable current limit
 *           that the power supply accepts for a particular voltage level on
 *           a channel for the output range to which the power supply is
 *           currently configured.
 *****************************************************************************/
ViStatus _VI_FUNC arpps_QueryMaxCurrentLimit (ViSession vi,
                                                 ViConstString channelName,
                                                 ViReal64 voltageLevel,
                                                 ViReal64* maxCurrentLimit)
{
    ViStatus    error = VI_SUCCESS;
    ViReal64    value;
    
    checkErr( Ivi_LockSession(vi, VI_NULL));
    
    if (maxCurrentLimit == VI_NULL)
        viCheckParm (IVI_ERROR_INVALID_PARAMETER, 4, "Null value for maxCurrentLimit.");
    
    if (Ivi_RangeChecking(vi))
        {
        viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL), 2, "Channel Name");
        viCheckParm( Ivi_CheckAttributeViReal64 (vi, channelName, ARPPS_ATTR_VOLTAGE_LEVEL, 0, voltageLevel), 3, "Voltage Level");
        }
    checkErr( Ivi_GetAttrMinMaxViReal64 (vi, channelName, ARPPS_ATTR_CURRENT_LIMIT, VI_NULL, &value, VI_NULL, VI_NULL));
    checkErr( arpps_CheckStatus (vi));

Error:    
    if (maxCurrentLimit) *maxCurrentLimit = value;
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: arpps_QueryMaxVoltageLevel                  
 * Purpose:  This function returns the maximum programmable voltage level
 *           that the power supply accepts for a particular current limit on
 *           a channel for the output range to which the power supply is
 *           currently configured.
 *****************************************************************************/
ViStatus _VI_FUNC arpps_QueryMaxVoltageLevel (ViSession vi,
                                                 ViConstString channelName,
                                                 ViReal64 currentLimit,
                                                 ViReal64* maxVoltageLevel)
{
    ViStatus error = VI_SUCCESS;
    ViReal64 value;
    
    checkErr( Ivi_LockSession(vi, VI_NULL));
    
    if (maxVoltageLevel == VI_NULL)
        viCheckParm (IVI_ERROR_INVALID_PARAMETER, 4, "Null value for maxVoltageLevel.");
    
    if (Ivi_RangeChecking(vi))
        {
        viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL), 2, "Channel Name");
        viCheckParm( Ivi_CheckAttributeViReal64 (vi, channelName, ARPPS_ATTR_CURRENT_LIMIT, 0, currentLimit), 3, "Current Limit");
        }

    checkErr( Ivi_GetAttrMinMaxViReal64 (vi, channelName, ARPPS_ATTR_VOLTAGE_LEVEL, VI_NULL, &value, VI_NULL, VI_NULL));
    checkErr( arpps_CheckStatus (vi));

Error:    
    if (maxVoltageLevel) *maxVoltageLevel = value;
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: arpps_ResetOutputProtection                                   
 * Purpose:  This function resets the power supply's output protection.
 *****************************************************************************/
ViStatus _VI_FUNC arpps_ResetOutputProtection (ViSession vi,
                                                  ViConstString channelName)
{
    ViStatus    error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession(vi, VI_NULL));

    viCheckErr( ARPPS_ERROR_RESET_PROTECTION_NOT_SUPPORTED);

Error:    
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
            
}

/*****************************************************************************
 * Function: arpps_Measure                                   
 * Purpose:  This function measures either the current or voltage that the
 *           power supply is generating.
 *****************************************************************************/
ViStatus _VI_FUNC arpps_Measure (ViSession vi,
                                 ViConstString channelName,
                                 ViInt32 measurementType,
                                 ViReal64* measurement)
{
    ViStatus    error = VI_SUCCESS;
    ViReal64    value = 0.0;
    ViAttr      attrId = 0;

    checkErr( Ivi_LockSession(vi, VI_NULL));
    
    IviDCPwr_InterchangeCheck (vi, "arpps_Measure");
    
    if (measurement == VI_NULL)
        viCheckParm (IVI_ERROR_INVALID_PARAMETER, 4, "Null value for measurement.");

    if (Ivi_RangeChecking(vi))
        viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL), 2, "Channel Name");

    if (measurementType == ARPPS_VAL_MEASURE_CURRENT)
        attrId = ARPPS_ATTR_MEAS_IOUT;
    else if (measurementType == ARPPS_VAL_MEASURE_VOLTAGE)
        attrId = ARPPS_ATTR_MEAS_VOUT;
    else
        viCheckParm( IVI_ERROR_INVALID_VALUE, 3, "Measurement Type");
        
    checkErr( Ivi_GetAttributeViReal64(vi, channelName, attrId, 0, &value));
    checkErr( arpps_CheckStatus (vi));

Error:    
    if (measurement) *measurement = value;
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
            
}

/*****************************************************************************
 * Function: arpps_GetAttribute<type> Functions                                    
 * Purpose:  These functions enable the instrument driver user to get 
 *           attribute values directly.  There are typesafe versions for 
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession attributes.                                         
 *****************************************************************************/
ViStatus _VI_FUNC arpps_GetAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViInt32 *value)
{                                                                                                           
    return Ivi_GetAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                    value);
}                                                                                                           
ViStatus _VI_FUNC arpps_GetAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViReal64 *value)
{                                                                                                           
    return Ivi_GetAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}                                                                                                           
ViStatus _VI_FUNC arpps_GetAttributeViString (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViInt32 bufSize, 
                                                 ViChar value[]) 
{   
    return Ivi_GetAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     bufSize, value);
}   
ViStatus _VI_FUNC arpps_GetAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViBoolean *value)
{                                                                                                           
    return Ivi_GetAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           
ViStatus _VI_FUNC arpps_GetAttributeViSession (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViSession *value)
{                                                                                                           
    return Ivi_GetAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           

/*****************************************************************************
 * Function: arpps_SetAttribute<type> Functions                                    
 * Purpose:  These functions enable the instrument driver user to set 
 *           attribute values directly.  There are typesafe versions for 
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession datatypes.                                         
 *****************************************************************************/
ViStatus _VI_FUNC arpps_SetAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViInt32 value)
{                                                                                                           
    return Ivi_SetAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                    value);
}                                                                                                           
ViStatus _VI_FUNC arpps_SetAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViReal64 value)
{                                                                                                           
    return Ivi_SetAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}                                                                                                           
ViStatus _VI_FUNC arpps_SetAttributeViString (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViConstString value) 
{   
    return Ivi_SetAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}   
ViStatus _VI_FUNC arpps_SetAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViBoolean value)
{                                                                                                           
    return Ivi_SetAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           
ViStatus _VI_FUNC arpps_SetAttributeViSession (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViSession value)
{                                                                                                           
    return Ivi_SetAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           

/*****************************************************************************
 * Function: arpps_CheckAttribute<type> Functions                                  
 * Purpose:  These functions enable the instrument driver user to check  
 *           attribute values directly.  These functions check the value you
 *           specify even if you set the ARPPS_ATTR_RANGE_CHECK 
 *           attribute to VI_FALSE.  There are typesafe versions for ViInt32, 
 *           ViReal64, ViString, ViBoolean, and ViSession datatypes.                         
 *****************************************************************************/
ViStatus _VI_FUNC arpps_CheckAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViInt32 value)
{                                                                                                           
    return Ivi_CheckAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}
ViStatus _VI_FUNC arpps_CheckAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                   ViAttr attributeId, ViReal64 value)
{                                                                                                           
    return Ivi_CheckAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                       value);
}                                                                                                           
ViStatus _VI_FUNC arpps_CheckAttributeViString (ViSession vi, ViConstString channelName, 
                                                   ViAttr attributeId, ViConstString value)  
{   
    return Ivi_CheckAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                       value);
}   
ViStatus _VI_FUNC arpps_CheckAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                    ViAttr attributeId, ViBoolean value)
{                                                                                                           
    return Ivi_CheckAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                        value);
}                                                                                                           
ViStatus _VI_FUNC arpps_CheckAttributeViSession (ViSession vi, ViConstString channelName, 
                                                    ViAttr attributeId, ViSession value)
{                                                                                                           
    return Ivi_CheckAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                        value);
}                                                                                                           

/*****************************************************************************
 * Function: arpps_GetError and arpps_ClearError Functions                       
 * Purpose:  These functions enable the instrument driver user to  
 *           get or clear the error information the driver associates with the
 *           IVI session.                                                        
 *****************************************************************************/
ViStatus _VI_FUNC arpps_GetError (ViSession vi, 
                                     ViStatus *errorCode, 
                                     ViInt32 bufferSize,
                                     ViChar description[])
{
    ViStatus error = VI_SUCCESS;
    ViStatus primary = VI_SUCCESS,
             secondary = VI_SUCCESS;
    ViChar   elaboration[256] = "";
    ViChar   errorMessage[1024] = "";
    ViChar  *appendPoint = errorMessage;
    ViInt32  actualSize = 0;
    ViBoolean locked = VI_FALSE;

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
            viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Error");
            }
        if (description == VI_NULL)
            {
            viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Description");
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
        checkErr( arpps_error_message (vi, primary, msg));
        appendPoint += sprintf(appendPoint, "Primary Error: (Hex 0x%08X) %s", primary, msg);
        }
    
    if (secondary != VI_SUCCESS)
        {
        ViChar msg[256] = "";
        checkErr( arpps_error_message (vi, secondary, msg));
        appendPoint += sprintf(appendPoint, "\nSecondary Error: (Hex 0x%08X) %s", secondary, msg);
        }
    
    if (elaboration[0])
        {
        sprintf(appendPoint, "\nElaboration: %s", elaboration);
        }
    
    actualSize = (ViInt32)strlen(errorMessage) + 1;
    
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

ViStatus _VI_FUNC arpps_ClearError (ViSession vi)                                                        
{                                                                                                           
    return Ivi_ClearErrorInfo (vi);                                                                             
}

/*****************************************************************************
 * Function: arpps_GetNextCoercionRecord                        
 * Purpose:  This function enables the instrument driver user to obtain
 *           the coercion information associated with the IVI session.                                                              
 *           This function retrieves and clears the oldest instance in which 
 *           the instrument driver coerced a value the instrument driver user
 *           specified to another value.                     
 *****************************************************************************/
ViStatus _VI_FUNC arpps_GetNextCoercionRecord (ViSession vi,
                                                  ViInt32 bufferSize,
                                                  ViChar  record[])
{
    return Ivi_GetNextCoercionString (vi, bufferSize, record);
}

/*****************************************************************************
 * Function: arpps_LockSession and arpps_UnlockSession Functions                        
 * Purpose:  These functions enable the instrument driver user to lock the 
 *           session around a sequence of driver calls during which other
 *           execution threads must not disturb the instrument state.
 *                                                                          
 *           NOTE:  The callerHasLock parameter must be a local variable 
 *           initialized to VI_FALSE and passed by reference, or you can pass 
 *           VI_NULL.                     
 *****************************************************************************/
ViStatus _VI_FUNC arpps_LockSession (ViSession vi, ViBoolean *callerHasLock)  
{                                              
    return Ivi_LockSession(vi,callerHasLock);      
}                                              
ViStatus _VI_FUNC arpps_UnlockSession (ViSession vi, ViBoolean *callerHasLock) 
{                                              
    return Ivi_UnlockSession(vi,callerHasLock);    
}   

/**************************************************************************** 
 *  Function: arpps_GetNextInterchangeWarning,
 *            arpps_ResetInterchangeCheck, and
 *            arpps_ClearInterchangeWarnings
 *  Purpose:  These functions allow the user to retrieve interchangeability
 *            warnings, reset the driver's interchangeability checking 
 *            state, and clear all previously logged interchangeability warnings.
 ****************************************************************************/
ViStatus _VI_FUNC arpps_GetNextInterchangeWarning (ViSession vi, 
                                                      ViInt32 bufferSize, 
                                                      ViChar warnString[])
{
    return Ivi_GetNextInterchangeCheckString (vi, bufferSize, warnString);
}

ViStatus _VI_FUNC arpps_ResetInterchangeCheck (ViSession vi)
{
    return Ivi_ResetInterchangeCheck (vi);
}

ViStatus _VI_FUNC arpps_ClearInterchangeWarnings (ViSession vi)
{
    return Ivi_ClearInterchangeWarnings (vi);
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
ViStatus _VI_FUNC arpps_WriteInstrData (ViSession vi, ViConstString writeBuffer)   
{   
    return Ivi_WriteInstrData (vi, writeBuffer);    
}   
ViStatus _VI_FUNC arpps_ReadInstrData (ViSession vi, ViInt32 numBytes, 
                                          ViChar rdBuf[], ViInt32 *bytesRead)  
{   
    return Ivi_ReadInstrData (vi, numBytes, rdBuf, bytesRead);   
} 

/****************************************************************************
 *--------- Obsolete Inherent functions ------------------------------------*
 ****************************************************************************/
/*****************************************************************************
 * Function: arpps_GetErrorInfo and arpps_ClearErrorInfo Functions                       
 * Purpose:  These functions enable the instrument driver user to  
 *           get or clear the error information the driver associates with the
 *           IVI session.                                                        
 *****************************************************************************/
ViStatus _VI_FUNC arpps_GetErrorInfo (ViSession vi, ViStatus *primaryError, 
                                         ViStatus *secondaryError, ViChar errorElaboration[256])  
{                                                                                                           
    return Ivi_GetErrorInfo(vi, primaryError, secondaryError, errorElaboration);                                
}                                                                                                           
ViStatus _VI_FUNC arpps_ClearErrorInfo (ViSession vi)                                                        
{                                                                                                           
    return Ivi_ClearErrorInfo (vi);                                                                             
}

/*****************************************************************************
 *-------------------- Utility Functions (Not Exported) ---------------------*
 *****************************************************************************/
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
            start++;

        lengthWithoutLeadingWS = (ViInt32)strlen( start);
        memmove( string, start, lengthWithoutLeadingWS);
        end = string + lengthWithoutLeadingWS;
        while (end > string && isspace((unsigned char)*(end-1)))
            end--;
        
        *end = '\0';
        }
}

/*****************************************************************************
 *  Function: arpps_GetDriverSetupOption
 *  Purpose:  Get the value of a DriverSetup option. The driver setup string
 *            is part of the option string the user passes to the
 *            hp662xa_InitWithOptions function. Specifically it is the part
 *            that follows "DriverSetup=". This function assumes the 
 *            DriverSetup string has the following format:
 *              "Model : OptionValue"
 *****************************************************************************/
static ViStatus arpps_GetDriverSetupOption (ViSession vi, ViInt32 *modelType)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      *tag,
                setup[BUFFER_SIZE],
                model[BUFFER_SIZE];

    *modelType = ARPPS_MODEL_NOT_FOUND;
    checkErr( Ivi_GetAttributeViString (vi, VI_NULL, IVI_ATTR_DRIVER_SETUP, 0, BUFFER_SIZE, setup));
    
    if ((tag = strstr (setup, SETUP_MODEL_TAG)) != VI_NULL)
        {
        sscanf(tag, "%*[^:]:%[^;]", model);
        RemoveSurroundingWhiteSpace (model);

        if (Ivi_GetValueFromTable (arpps_ModelTable, model, modelType) < VI_SUCCESS)
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
 *  Function: arpps_QueryID
 *  Purpose:  Query the instrument for its id string.
 *
 *  Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus arpps_QueryID (ViSession vi, ViSession io,
                                 ViInt32 bufferSize,
                                 ViChar buffer[])
{
    ViStatus  error = VI_SUCCESS;
    ViUInt32  readCount;

    viCheckErr( viPrintf (io, "MODEL?"));
    viCheckErr( viRead (io, buffer, bufferSize-1, &readCount));
    buffer[readCount] = '\0';
    sscanf (buffer, "%s[t32]", buffer);

Error:
    return error;
}

/*****************************************************************************
 *  Function: arpps_CheckStatusBit
 *  Purpose:  Query and check the status from instrument
 *
 *  Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus arpps_CheckStatusBit (ViSession vi, ViSession io,
                                      ViConstString ch, 
                                      ViInt32 mask, 
                                      ViBoolean* value)
{
    ViStatus error = VI_SUCCESS;
    ViInt32  i = 0,
             status = 0;

    checkErr( Ivi_GetChannelIndex (vi, ch, &i));
    if (i == 2) mask <<= 8;

    viCheckErr (viQueryf (io, "STATUS?", "%ld", &status));
    *value = ((status & mask) != 0);
Error:
    return error;
}

/*****************************************************************************
 * Function: arpps_WriteSingleInt
 * Purpose:  This function writes int value to single channel instrument.
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus arpps_WriteSingleInt (ViSession vi, ViSession io,
                                      ViConstString cmd,
                                      ViInt32 value)
{
    ViStatus error;

    viCheckErr( viPrintf (io, "%s %ld", cmd, value));

Error:
    return error;
}

/*****************************************************************************
 * Function: arpps_WriteDualInt
 * Purpose:  This function writes int value to dual channel instrument.
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus arpps_WriteDualInt (ViSession vi, ViSession io,
                                    ViConstString channelName, 
                                    ViConstString cmd,
                                    ViInt32 value)
{
    ViStatus error;

    viCheckErr( viPrintf (io, "%s%s %ld", cmd, channelName, value));

Error:
    return error;
}

/*****************************************************************************
 * Function: arpps_WriteSingleDouble
 * Purpose:  This function writes double value to single channel instrument.
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus arpps_WriteSingleDouble (ViSession vi, ViSession io,
                                      ViConstString cmd,
                                      ViReal64 value)
{
    ViStatus error;

    viCheckErr( viPrintf (io, "%s %Lf", cmd, value));

Error:
    return error;
}

/*****************************************************************************
 * Function: arpps_WriteDualDouble
 * Purpose:  This function writes double value to dual channel instrument.
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus arpps_WriteDualDouble (ViSession vi, ViSession io,
                                       ViConstString channelName, 
                                       ViConstString cmd,
                                       ViReal64 value)
{
    ViStatus error;

    viCheckErr( viPrintf (io, "%s%s %Lf", cmd, channelName, value));

Error:
    return error;
}

/*****************************************************************************
 * Function: arpps_ReadSingleDouble
 * Purpose:  This function reads double value to single channel instrument.
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus arpps_ReadSingleDouble (ViSession vi, ViSession io,
                                      ViConstString cmd,
                                      ViReal64* value)
{
    ViStatus error;

    viCheckErr( viQueryf (io, "%s?", "%Le", cmd, value));

Error:
    return error;
}

/*****************************************************************************
 * Function: arpps_ReadDualDouble
 * Purpose:  This function reads double value to dual channel instrument.
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus arpps_ReadDualDouble (ViSession vi, ViSession io,
                                       ViConstString channelName, 
                                       ViConstString cmd,
                                       ViReal64* value)
{
    ViStatus error;

    viCheckErr( viQueryf (io, "%s%s?", "%Le", cmd, channelName, value));

Error:
    return error;
}

/*****************************************************************************
 * Function: arpps_GetSimData
 * Purpose:  This function returns measurement value in simulation.
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus arpps_GetSimData(ViSession vi, ViConstString channelName,
                                 ViAttr attrId, ViReal64 *value)
{
    ViStatus error;

    checkErr( Ivi_GetAttributeViReal64( vi, channelName, attrId, 0, value));
    *value *= 0.95;

Error:
    return error;
}

/*****************************************************************************
 * Function: arpps_CheckStatus                                                 
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
static ViStatus arpps_CheckStatus (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;

    if (Ivi_QueryInstrStatus (vi) && Ivi_NeedToCheckStatus (vi) && !Ivi_Simulating (vi))
        {
        checkErr( arpps_CheckStatusCallback (vi, Ivi_IOSession(vi)));
        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_FALSE));
        }
        
Error:
    return error;
}

/*****************************************************************************
 * Function: arpps_DefaultInstrSetup                                               
 * Purpose:  This function sends a default setup to the instrument.  The    
 *           arpps_reset function calls this function.  The 
 *           arpps_IviInit function calls this function when the
 *           user passes VI_FALSE for the reset parameter.  This function is 
 *           useful for configuring settings that other instrument driver 
 *           functions require.    
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus arpps_DefaultInstrSetup (ViSession vi)
{
    return Ivi_InvalidateAllAttributes (vi);
}

/*****************************************************************************
 *------------------------ Global Session Callbacks -------------------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: arpps_CheckStatusCallback                                               
 * Purpose:  This function queries the instrument to determine if it has 
 *           encountered an error.  If the instrument has encountered an 
 *           error, this function returns the IVI_ERROR_INSTRUMENT_SPECIFIC 
 *           error code.  This function is called by the 
 *           arpps_CheckStatus utility function.  The 
 *           Ivi_SetAttribute and Ivi_GetAttribute functions invoke this 
 *           function when the optionFlags parameter includes the
 *           IVI_VAL_DIRECT_USER_CALL flag.
 *           
 *           The user can disable calls to this function by setting the 
 *           IVI_ATTR_QUERY_INSTR_STATUS attribute to VI_FALSE.  The driver can 
 *           disable the check status callback for a particular attribute by 
 *           setting the IVI_VAL_DONT_CHECK_STATUS flag.
 *****************************************************************************/
static ViStatus _VI_FUNC arpps_CheckStatusCallback (ViSession vi, ViSession io)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     errorCode;
    
    /* Query instrument status */
    viCheckErr( viQueryf(io, "ERROR?", "%ld", &errorCode));
    if (errorCode != 0)
        {
        checkErr( Ivi_SetErrorInfo (vi, VI_FALSE, IVI_ERROR_INSTR_SPECIFIC, errorCode, VI_NULL));
        }
Error:
    return error;
}

/*****************************************************************************
 *----------------- Attribute Range Tables and Callbacks --------------------*
 *****************************************************************************/

/*- ARPPS_ATTR_ID_QUERY_RESPONSE -*/
static ViStatus _VI_FUNC arppsAttrIdQueryResponse_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    
    checkErr( arpps_QueryID ( vi, io, BUFFER_SIZE, rdBuffer));
    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
    
Error:
    return error;
}
    
/*- ARPPS_ATTR_SPECIFIC_DRIVER_REVISION -*/
static ViStatus _VI_FUNC arppsAttrSpecificDriverRevision_ReadCallback (ViSession vi, 
                                                                  ViSession io,
                                                                  ViConstString channelName, 
                                                                  ViAttr attributeId,
                                                                  const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      driverRevision[256];
    
    
    sprintf (driverRevision, 
             "Driver: arpps %d.%d, Compiler: %s %3.2f, "
             "Components: IVIEngine %.2f, VISA-Spec %.2f",
             ARPPS_MAJOR_VERSION , ARPPS_MINOR_VERSION, 
             IVI_COMPILER_NAME, IVI_COMPILER_VER_NUM, 
             IVI_ENGINE_MAJOR_VERSION + IVI_ENGINE_MINOR_VERSION/1000.0, 
             Ivi_ConvertVISAVer(VI_SPEC_VERSION));

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, driverRevision));    
Error:
    return error;
}

/*- ARPPS_ATTR_OUTPUT_ENABLED -*/
static ViStatus _VI_FUNC arppsSingleOutputEnabled_WriteCallback (ViSession vi, 
                                                                 ViSession io,
                                                                 ViConstString channelName, 
                                                                 ViAttr attributeId,
                                                                 ViBoolean value)
{
    return arpps_WriteSingleInt (vi, io, "OUT", (value) ? 1 : 0);
}

static ViStatus _VI_FUNC arppsDualOutputEnabled_WriteCallback (ViSession vi, 
                                                                 ViSession io,
                                                                 ViConstString channelName, 
                                                                 ViAttr attributeId,
                                                                 ViBoolean value)
{
    return arpps_WriteDualInt (vi, io, channelName, "OUT", (value) ? 1 : 0);
}

static ViStatus _VI_FUNC arppsAttrOutputEnabled_ReadCallback (ViSession vi, 
                                                              ViSession io,
                                                              ViConstString channelName, 
                                                              ViAttr attributeId,
                                                              ViBoolean* value)
{
    ViStatus error = VI_SUCCESS;
    
    checkErr( arpps_CheckStatusBit (vi, io, channelName, ARPPS_CH1_OUTPUT_OFF_BIT, value));
    *value = !(*value);
    
Error:
    return error;
}

/*- ARPPS_ATTR_VOLTAGE_LEVEL -*/
static ViStatus _VI_FUNC arppsSingleVoltageLevel_WriteCallback (ViSession vi, 
                                                                ViSession io,
                                                                ViConstString channelName, 
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
    return arpps_WriteSingleDouble ( vi, io, "VSET", value);
}

static ViStatus _VI_FUNC arppsDualVoltageLevel_WriteCallback (ViSession vi, 
                                                                ViSession io,
                                                                ViConstString channelName, 
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
    return arpps_WriteDualDouble ( vi, io, channelName, "VSET", value);
}

static ViStatus _VI_FUNC arppsSingleVoltageLevel_ReadCallback (ViSession vi, 
                                                               ViSession io,
                                                               ViConstString channelName, 
                                                               ViAttr attributeId,
                                                               ViReal64* value)
{
    return arpps_ReadSingleDouble (vi, io, "VSET", value);
}

static ViStatus _VI_FUNC arppsDualVoltageLevel_ReadCallback (ViSession vi, 
                                                               ViSession io,
                                                               ViConstString channelName, 
                                                               ViAttr attributeId,
                                                               ViReal64* value)
{
    return arpps_ReadDualDouble (vi, io, channelName, "VSET", value);
}

/*- ARPPS_ATTR_OVP_ENABLED -*/
static ViStatus _VI_FUNC arppsSingleOVPEnabled_WriteCallback (ViSession vi, 
                                                            ViSession io,
                                                            ViConstString channelName, 
                                                            ViAttr attributeId,
                                                            ViBoolean value)
{
    return arpps_WriteSingleInt (vi, io, "OVP", (value) ? 1 : 0);
}

static ViStatus _VI_FUNC arppsDualOVPEnabled_WriteCallback (ViSession vi, 
                                                            ViSession io,
                                                            ViConstString channelName, 
                                                            ViAttr attributeId,
                                                            ViBoolean value)
{
    return arpps_WriteDualInt (vi, io, channelName, "OVP", (value) ? 1 : 0);
}

/*- ARPPS_ATTR_OVP_LIMIT -*/
static ViStatus _VI_FUNC arppsSingleOVPLimit_WriteCallback (ViSession vi, 
                                                            ViSession io,
                                                            ViConstString channelName, 
                                                            ViAttr attributeId,
                                                            ViReal64 value)
{
    return arpps_WriteSingleDouble ( vi, io, "OVSET", value);
}

static ViStatus _VI_FUNC arppsDualOVPLimit_WriteCallback (ViSession vi, 
                                                            ViSession io,
                                                            ViConstString channelName, 
                                                            ViAttr attributeId,
                                                            ViReal64 value)
{
    return arpps_WriteDualDouble ( vi, io, channelName, "OVSET", value);
}

static ViStatus _VI_FUNC arppsSingleOVPLimit_ReadCallback (ViSession vi, 
                                                           ViSession io,
                                                           ViConstString channelName, 
                                                           ViAttr attributeId,
                                                           ViReal64* value)
{
    return arpps_ReadSingleDouble (vi, io, "OVSET", value);
}

static ViStatus _VI_FUNC arppsDualOVPLimit_ReadCallback (ViSession vi, 
                                                           ViSession io,
                                                           ViConstString channelName, 
                                                           ViAttr attributeId,
                                                           ViReal64* value)
{
    return arpps_ReadDualDouble (vi, io, channelName, "OVSET", value);
}

/*- ARPPS_ATTR_CURRENT_LIMIT_BEHAVIOR -*/
static IviRangeTableEntry attrCurrentLimitBehaviorRangeTableEntries[] =
    {
        {ARPPS_VAL_CURRENT_REGULATE,  0, 0, "", 0},
        {ARPPS_VAL_CURRENT_TRIP,      0, 0, "", 0},        
        {IVI_RANGE_TABLE_LAST_ENTRY},
    };
static IviRangeTable attrCurrentLimitBehaviorRangeTable = 
    {
        IVI_VAL_DISCRETE,
        VI_FALSE,
        VI_FALSE,
        VI_NULL,
        attrCurrentLimitBehaviorRangeTableEntries,
    };
    
static ViStatus _VI_FUNC arppsSingleCurrentLimitBehavior_WriteCallback (ViSession vi, 
                                                                        ViSession io,
                                                                        ViConstString channelName, 
                                                                        ViAttr attributeId,
                                                                        ViInt32 value)
{
    return arpps_WriteSingleInt (vi, io, "OCP", (value == ARPPS_VAL_CURRENT_TRIP) ? 1 : 0);
}

static ViStatus _VI_FUNC arppsDualCurrentLimitBehavior_WriteCallback (ViSession vi, 
                                                                        ViSession io,
                                                                        ViConstString channelName, 
                                                                        ViAttr attributeId,
                                                                        ViInt32 value)
{
    return arpps_WriteDualInt (vi, io, channelName, "OCP", (value == ARPPS_VAL_CURRENT_TRIP) ? 1 : 0);
}

static ViStatus _VI_FUNC arppsAttrCurrentLimitBehavior_ReadCallback (ViSession vi, 
                                                                       ViSession io,
                                                                       ViConstString channelName, 
                                                                       ViAttr attributeId,
                                                                       ViInt32* value)
{
    ViStatus    error = VI_SUCCESS;
    ViBoolean   state;

    checkErr( arpps_CheckStatusBit (vi, io, channelName, ARPPS_CH1_OCP_MODE_BIT, &state));
    *value = (state) ? ARPPS_VAL_CURRENT_TRIP : ARPPS_VAL_CURRENT_REGULATE;

Error:
    return error;
}

/*- ARPPS_ATTR_CURRENT_LIMIT -*/
static ViStatus _VI_FUNC arppsSingleCurrentLimit_WriteCallback (ViSession vi, 
                                                                ViSession io,
                                                                ViConstString channelName, 
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
    return arpps_WriteSingleDouble ( vi, io, "ISET", value);
}

static ViStatus _VI_FUNC arppsDualCurrentLimit_WriteCallback (ViSession vi, 
                                                              ViSession io,
                                                              ViConstString channelName, 
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
    return arpps_WriteDualDouble ( vi, io, channelName, "ISET", value);
}

static ViStatus _VI_FUNC arppsSingleCurrentLimit_ReadCallback (ViSession vi, 
                                                               ViSession io,
                                                               ViConstString channelName, 
                                                               ViAttr attributeId,
                                                               ViReal64* value)
{
    return arpps_ReadSingleDouble (vi, io, "ISET", value);
}

static ViStatus _VI_FUNC arppsDualCurrentLimit_ReadCallback (ViSession vi, 
                                                               ViSession io,
                                                               ViConstString channelName, 
                                                               ViAttr attributeId,
                                                               ViReal64* value)
{

    return arpps_ReadDualDouble (vi, io, channelName, "ISET", value);
}

    /*- (Hidden) ARPPS_ATTR_MEAS_IOUT -*/
static ViStatus _VI_FUNC arppsSingleMeasIout_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViReal64 *value)
{
    ViStatus    error = VI_SUCCESS;

    if (!Ivi_Simulating(vi))
        error = arpps_ReadSingleDouble (vi, io, "IOUT", value);
    else
        error = arpps_GetSimData(vi, channelName, ARPPS_ATTR_CURRENT_LIMIT, value);

    checkErr( error);

Error:
    return error;
}

static ViStatus _VI_FUNC arppsDualMeasIout_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViReal64 *value)
{
    ViStatus    error = VI_SUCCESS;

    if (!Ivi_Simulating(vi))
        error = arpps_ReadDualDouble (vi, io, channelName, "IOUT", value);
    else
        error = arpps_GetSimData(vi, channelName, ARPPS_ATTR_CURRENT_LIMIT, value);
    
    checkErr( error);

Error:
    return error;
}

    /*- (Hidden) ARPPS_ATTR_MEAS_VOUT -*/
static ViStatus _VI_FUNC arppsSingleMeasVout_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViReal64 *value)
{
    ViStatus    error = VI_SUCCESS;

    if (!Ivi_Simulating(vi))
        error = arpps_ReadSingleDouble (vi, io, "VOUT", value);
    else
        error = arpps_GetSimData(vi, channelName, ARPPS_ATTR_VOLTAGE_LEVEL, value);
    
    checkErr( error);

Error:
    return error;
}

static ViStatus _VI_FUNC arppsDualMeasVout_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViReal64 *value)
{
    ViStatus    error = VI_SUCCESS;

    if (!Ivi_Simulating(vi))
        error = arpps_ReadDualDouble (vi, io, channelName, "VOUT", value);
    else
        error = arpps_GetSimData(vi, channelName, ARPPS_ATTR_VOLTAGE_LEVEL, value);

    checkErr( error);

Error:
    return error;
}

/*****************************************************************************
 * Function: arpps_InitAttributes                                                  
 * Purpose:  This function adds attributes to the IVI session, initializes   
 *           instrument attributes, and sets attribute invalidation          
 *           dependencies.                                                   
 *****************************************************************************/
static ViStatus arpps_InitAttributes (ViSession vi, ViInt32 modelType,
                                      ViBoolean isDual)
{
    ViStatus            error = VI_SUCCESS;
    IviRangeTablePtr    attrVoltageSetting,
                        attrCurrentSetting;
    ViString            arpps_models[] = {"PPS8-10","PPS18-4","PPS30-2.5","PPS35-2","PPS60-10",
                                          "PPS128-0.5","PPS250-0.2","PPS5-30","PPS35-3R","PPS60-1.5R",
                                          "PPS8-6D","PPS18-4D","PPS35-2D","PPS30-3D","PPS60-1D",
                                          "PPS128-0.5D","PPS30-6","PPS35-5","PPS8-20","PPS18-10","PPS60-3",
                                          "PPS128-1.5","PPS250-0.8","PPS-1202"};

    /*- Range Table for attribute ARPPS_ATTR_VOLTAGE_LEVEL ------------------*/
    /*-                           ARPPS_ATTR_OVP_LIMIT ----------------------*/
    checkErr( Ivi_RangeTableNew (vi, 2, 1, VI_TRUE, VI_TRUE, &attrVoltageSetting));
    viCheckErr( Ivi_SetRangeTableEntry (attrVoltageSetting, 0, 0.0,
                                        arpps_VoltageLimits[modelType], 0.0, "", 0));

    /*- Range Table for attribute ARPPS_ATTR_CURRENT_LIMIT ----------*/
    checkErr( Ivi_RangeTableNew (vi, 2, 1, VI_TRUE, VI_TRUE, &attrCurrentSetting));
    viCheckErr( Ivi_SetRangeTableEntry (attrCurrentSetting, 0, 0.0,
                                        arpps_CurrentLimits[modelType], 0.0, "", 0));

    /*- Initialize instrument attributes --------------------------------*/
    checkErr( Ivi_SetAttributeViInt32 (vi, "", ARPPS_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION,
                                       0, ARPPS_MAJOR_VERSION));
    checkErr( Ivi_SetAttributeViInt32 (vi, "", ARPPS_ATTR_SPECIFIC_DRIVER_MINOR_VERSION, 0,
                                       ARPPS_MINOR_VERSION));
    checkErr( Ivi_SetAttrReadCallbackViString (vi, ARPPS_ATTR_SPECIFIC_DRIVER_REVISION,
                                               arppsAttrSpecificDriverRevision_ReadCallback));
    checkErr( Ivi_SetAttributeViInt32 (vi, "", ARPPS_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION,
                                       0, ARPPS_CLASS_SPEC_MAJOR_VERSION));
    checkErr( Ivi_SetAttributeViInt32 (vi, "", ARPPS_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION,
                                       0, ARPPS_CLASS_SPEC_MINOR_VERSION));
    checkErr( Ivi_SetAttributeViString (vi, "", ARPPS_ATTR_IO_SESSION_TYPE,
                                        0, ARPPS_IO_SESSION_TYPE));
    checkErr( Ivi_SetAttributeViString (vi, "", ARPPS_ATTR_SUPPORTED_INSTRUMENT_MODELS,
                                        0, ARPPS_SUPPORTED_INSTRUMENT_MODELS));
    checkErr( Ivi_SetAttributeViString (vi, "", ARPPS_ATTR_INSTRUMENT_MODEL,
                                        0, arpps_models[modelType]));
    checkErr( Ivi_SetAttributeViString (vi, "", ARPPS_ATTR_INSTRUMENT_MANUFACTURER,
                                        0, ARPPS_VAL_INSTRUMENT_MANUFACTURER));
    checkErr( Ivi_SetAttributeViString (vi, "", ARPPS_ATTR_SPECIFIC_DRIVER_VENDOR,
                                        0, ARPPS_DRIVER_VENDOR));
    checkErr( Ivi_SetAttributeViString (vi, "", ARPPS_ATTR_SPECIFIC_DRIVER_DESCRIPTION,
                                        0, ARPPS_DRIVER_DESCRIPTION));
    checkErr( Ivi_SetAttributeViAddr (vi, VI_NULL, IVI_ATTR_OPC_CALLBACK, 0,
                                      VI_NULL));
    checkErr( Ivi_SetAttributeViAddr (vi, VI_NULL, IVI_ATTR_CHECK_STATUS_CALLBACK, 0,
                                      arpps_CheckStatusCallback));
    checkErr( Ivi_SetAttributeViBoolean (vi, VI_NULL, IVI_ATTR_SUPPORTS_WR_BUF_OPER_MODE, 
                                         0, VI_FALSE));
    checkErr( Ivi_SetAttributeViString (vi, "", ARPPS_ATTR_GROUP_CAPABILITIES, 0,
                                        "IviDCPwrBase, IviDCPwrMeasurement"));
                                                      
    /*- Add instrument-specific attributes ------------------------------*/            
    checkErr( Ivi_AddAttributeViBoolean (vi, ARPPS_ATTR_OUTPUT_ENABLED,
                                         "ARPPS_ATTR_OUTPUT_ENABLED",
                                         VI_FALSE, 
                                         IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
                                         arppsAttrOutputEnabled_ReadCallback,
                                         arppsSingleOutputEnabled_WriteCallback));

    checkErr (Ivi_AddAttributeViReal64 (vi, ARPPS_ATTR_VOLTAGE_LEVEL,
                                        "ARPPS_ATTR_VOLTAGE_LEVEL", 0.0,
                                        IVI_VAL_MULTI_CHANNEL,
                                        arppsSingleVoltageLevel_ReadCallback,
                                        arppsSingleVoltageLevel_WriteCallback,
                                        attrVoltageSetting, 0));

    checkErr (Ivi_AddAttributeViBoolean (vi, ARPPS_ATTR_OVP_ENABLED,
                                         "ARPPS_ATTR_OVP_ENABLED", VI_TRUE,
                                         IVI_VAL_MULTI_CHANNEL, VI_NULL,
                                         arppsSingleOVPEnabled_WriteCallback));

    checkErr (Ivi_AddAttributeViReal64 (vi, ARPPS_ATTR_OVP_LIMIT,
                                        "ARPPS_ATTR_OVP_LIMIT", 20.0,
                                        IVI_VAL_MULTI_CHANNEL,
                                        arppsSingleOVPLimit_ReadCallback,
                                        arppsSingleOVPLimit_WriteCallback,
                                        attrVoltageSetting, 0));

    checkErr( Ivi_AddAttributeViInt32 (vi, ARPPS_ATTR_CURRENT_LIMIT_BEHAVIOR,
                                         "ARPPS_ATTR_CURRENT_LIMIT_BEHAVIOR",
                                         ARPPS_VAL_CURRENT_REGULATE, 
                                         IVI_VAL_MULTI_CHANNEL,
                                         arppsAttrCurrentLimitBehavior_ReadCallback,
                                         arppsSingleCurrentLimitBehavior_WriteCallback,
                                         &attrCurrentLimitBehaviorRangeTable));

    checkErr (Ivi_AddAttributeViReal64 (vi, ARPPS_ATTR_CURRENT_LIMIT,
                                        "ARPPS_ATTR_CURRENT_LIMIT", 0.08,
                                        IVI_VAL_MULTI_CHANNEL,
                                        arppsSingleCurrentLimit_ReadCallback,
                                        arppsSingleCurrentLimit_WriteCallback,
                                        attrCurrentSetting, 0));

    checkErr( Ivi_AddAttributeViString (vi, ARPPS_ATTR_ID_QUERY_RESPONSE,
                                        "ARPPS_ATTR_ID_QUERY_RESPONSE", 
                                        "PPS35-2",
                                        IVI_VAL_NOT_USER_WRITABLE,
                                        arppsAttrIdQueryResponse_ReadCallback,
                                        VI_NULL));

    checkErr (Ivi_AddAttributeViReal64 (vi, ARPPS_ATTR_MEAS_IOUT,
                                        "ARPPS_ATTR_MEAS_IOUT", 0.0,
                                        IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION | IVI_VAL_HIDDEN,
                                        arppsSingleMeasIout_ReadCallback, VI_NULL,
                                        VI_NULL, 0));

    checkErr (Ivi_AddAttributeViReal64 (vi, ARPPS_ATTR_MEAS_VOUT,
                                        "ARPPS_ATTR_MEAS_VOUT", 0.0,
                                        IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION | IVI_VAL_HIDDEN,
                                        arppsSingleMeasVout_ReadCallback, VI_NULL,
                                        VI_NULL, 0));

    /*- Setup attribute invalidations -----------------------------------*/
    checkErr( arpps_SetSpecificCallbacks (vi, isDual));
    
     
Error:
    return error;
}


/*****************************************************************************
 * Function: arpps_SetSpecificCallbacks                                                  
 * Purpose:  This function sets callbacks for dual channel models.
 *****************************************************************************/
static ViStatus arpps_SetSpecificCallbacks (ViSession vi, ViBoolean isDual)
{
    ViStatus   error = VI_SUCCESS;

    if (isDual)
        {
        checkErr( Ivi_SetAttrWriteCallbackViBoolean (vi, ARPPS_ATTR_OUTPUT_ENABLED, arppsDualOutputEnabled_WriteCallback));
    
        checkErr( Ivi_SetAttrWriteCallbackViReal64 (vi, ARPPS_ATTR_VOLTAGE_LEVEL, arppsDualVoltageLevel_WriteCallback));
        checkErr( Ivi_SetAttrReadCallbackViReal64 (vi, ARPPS_ATTR_VOLTAGE_LEVEL, arppsDualVoltageLevel_ReadCallback));
    
        checkErr( Ivi_SetAttrWriteCallbackViBoolean (vi, ARPPS_ATTR_OVP_ENABLED, arppsDualOVPEnabled_WriteCallback));
    
        checkErr( Ivi_SetAttrWriteCallbackViReal64 (vi, ARPPS_ATTR_OVP_LIMIT, arppsDualOVPLimit_WriteCallback));
        checkErr( Ivi_SetAttrReadCallbackViReal64 (vi, ARPPS_ATTR_OVP_LIMIT, arppsDualOVPLimit_ReadCallback));
    
        checkErr( Ivi_SetAttrWriteCallbackViInt32 (vi, ARPPS_ATTR_CURRENT_LIMIT_BEHAVIOR, arppsDualCurrentLimitBehavior_WriteCallback));
    
        checkErr( Ivi_SetAttrWriteCallbackViReal64 (vi, ARPPS_ATTR_CURRENT_LIMIT, arppsDualCurrentLimit_WriteCallback));
        checkErr( Ivi_SetAttrReadCallbackViReal64 (vi, ARPPS_ATTR_CURRENT_LIMIT, arppsDualCurrentLimit_ReadCallback));
    
        checkErr( Ivi_SetAttrReadCallbackViReal64 (vi, ARPPS_ATTR_MEAS_IOUT, arppsDualMeasIout_ReadCallback));

        checkErr( Ivi_SetAttrReadCallbackViReal64 (vi, ARPPS_ATTR_MEAS_VOUT, arppsDualMeasVout_ReadCallback));
        }

    if (!Ivi_Simulating(vi))
        checkErr( Ivi_SetAttrReadCallbackViString (vi, ARPPS_ATTR_INSTRUMENT_MODEL, arppsAttrIdQueryResponse_ReadCallback));

Error:
    return error;
}

/*****************************************************************************
 *------------------- End Instrument Driver Source Code ---------------------*
 *****************************************************************************/
