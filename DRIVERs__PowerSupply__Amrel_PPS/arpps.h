/****************************************************************************
 *                       American Reliance PPS Power Supply                           
 *---------------------------------------------------------------------------
 *   (c)1999-2013, National Instruments, Corporation.  All Rights Reserved.         
 *---------------------------------------------------------------------------
 *                                                                          
 * Title:    arpps.h                                        
 * Purpose:  American Reliance PPS Power Supply                                       
 *           instrument driver declarations.                                
 *                                                                          
 ****************************************************************************/

#ifndef __ARPPS_HEADER
#define __ARPPS_HEADER

#include <ivi.h>
#include <ividcpwr.h>

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

/****************************************************************************
 *--------------------- Instrument Driver Information ----------------------*
 ****************************************************************************/
#define ARPPS_MAJOR_VERSION                2     /* Instrument driver major version   */
#define ARPPS_MINOR_VERSION               101     /* Instrument driver minor version   */

#define ARPPS_CLASS_SPEC_MAJOR_VERSION     2     /* Class specification major version */
#define ARPPS_CLASS_SPEC_MINOR_VERSION     0     /* Class specification minor version */

#define ARPPS_SUPPORTED_INSTRUMENT_MODELS  "PPS8-10, PPS18-4, PPS30-2.5, PPS35-2, "\
                                           "PPS60-10, PPS128-0.5, PPS250-0.2, PPS5-30, "\
                                           "PPS35-3R, PPS60-1.5R, PPS8-6D, PPS18-4D, "\
                                           "PPS35-2D, PPS30-3D, PPS60-1D, PPS128-0.5D, "\
                                           "PPS30-6, PPS35-5, PPS8-20, PPS18-10, PPS60-3, "\
                                           "PPS128-1.5, PPS250-0.8"

#define ARPPS_DRIVER_VENDOR                "National Instruments"
#ifdef  _IVI_mswin64_
#define ARPPS_DRIVER_DESCRIPTION           "PPS Series - Programmable DC Power Supply [Compiled for 64-bit.]"
#else
#define ARPPS_DRIVER_DESCRIPTION           "PPS Series - Programmable DC Power Supply"
#endif

#define ARPPS_IO_SESSION_TYPE              "VISA"

/**************************************************************************** 
 *------------------------------ Useful Macros -----------------------------* 
 ****************************************************************************/

    /*- Defined values for rangeType parameter of function -----------------*/
    /*- ConfigureOutputRange -----------------------------------------------*/
#define ARPPS_VAL_RANGE_CURRENT               IVIDCPWR_VAL_RANGE_CURRENT
#define ARPPS_VAL_RANGE_VOLTAGE               IVIDCPWR_VAL_RANGE_VOLTAGE
                                                                                            
    /*- Defined values for outputState parameter of function ---------------*/
    /*- QueryOutputState ---------------------------------------------------*/
#define ARPPS_VAL_OUTPUT_CONSTANT_VOLTAGE     IVIDCPWR_VAL_OUTPUT_CONSTANT_VOLTAGE
#define ARPPS_VAL_OUTPUT_CONSTANT_CURRENT     IVIDCPWR_VAL_OUTPUT_CONSTANT_CURRENT
#define ARPPS_VAL_OUTPUT_OVER_VOLTAGE         IVIDCPWR_VAL_OUTPUT_OVER_VOLTAGE
#define ARPPS_VAL_OUTPUT_OVER_CURRENT         IVIDCPWR_VAL_OUTPUT_OVER_CURRENT

    /*- Defined values for measurementType parameter of function -----------*/
    /*- Measure ------------------------------------------------------------*/
#define ARPPS_VAL_MEASURE_CURRENT             IVIDCPWR_VAL_MEASURE_CURRENT
#define ARPPS_VAL_MEASURE_VOLTAGE             IVIDCPWR_VAL_MEASURE_VOLTAGE

/****************************************************************************
 *---------------------------- Attribute Defines ---------------------------*
 ****************************************************************************/

    /*- IVI Inherent Instrument Attributes ---------------------------------*/

        /* User Options */
#define ARPPS_ATTR_RANGE_CHECK                   IVI_ATTR_RANGE_CHECK                    /* ViBoolean */
#define ARPPS_ATTR_QUERY_INSTRUMENT_STATUS       IVI_ATTR_QUERY_INSTRUMENT_STATUS        /* ViBoolean */
#define ARPPS_ATTR_CACHE                         IVI_ATTR_CACHE                          /* ViBoolean */
#define ARPPS_ATTR_SIMULATE                      IVI_ATTR_SIMULATE                       /* ViBoolean */
#define ARPPS_ATTR_RECORD_COERCIONS              IVI_ATTR_RECORD_COERCIONS               /* ViBoolean */

        /* Instrument Capabilities */
#define ARPPS_ATTR_CHANNEL_COUNT                 IVI_ATTR_CHANNEL_COUNT                   /* ViInt32,  read-only  */
#define ARPPS_ATTR_GROUP_CAPABILITIES            IVI_ATTR_GROUP_CAPABILITIES             /* ViString, read-only */

        /* Driver Information  */
#define ARPPS_ATTR_SPECIFIC_DRIVER_PREFIX        IVI_ATTR_SPECIFIC_DRIVER_PREFIX         /* ViString, read-only  */
#define ARPPS_ATTR_SUPPORTED_INSTRUMENT_MODELS   IVI_ATTR_SUPPORTED_INSTRUMENT_MODELS    /* ViString, read-only  */
#define ARPPS_ATTR_INSTRUMENT_MANUFACTURER       IVI_ATTR_INSTRUMENT_MANUFACTURER        /* ViString, read-only  */
#define ARPPS_ATTR_INSTRUMENT_MODEL              IVI_ATTR_INSTRUMENT_MODEL               /* ViString, read-only  */
#define ARPPS_ATTR_INSTRUMENT_FIRMWARE_REVISION  IVI_ATTR_INSTRUMENT_FIRMWARE_REVISION   /* ViString, read-only  */
#define ARPPS_ATTR_SPECIFIC_DRIVER_REVISION      IVI_ATTR_SPECIFIC_DRIVER_REVISION       /* ViString, read-only  */
#define ARPPS_ATTR_SPECIFIC_DRIVER_VENDOR        IVI_ATTR_SPECIFIC_DRIVER_VENDOR         /* ViString, read-only  */
#define ARPPS_ATTR_SPECIFIC_DRIVER_DESCRIPTION   IVI_ATTR_SPECIFIC_DRIVER_DESCRIPTION    /* ViString, read-only  */
#define ARPPS_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION /* ViInt32, read-only */
#define ARPPS_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION /* ViInt32, read-only */

        /* Advanced Session Information */
#define ARPPS_ATTR_LOGICAL_NAME              IVI_ATTR_LOGICAL_NAME                   /* ViString, read-only  */
#define ARPPS_ATTR_IO_RESOURCE_DESCRIPTOR    IVI_ATTR_IO_RESOURCE_DESCRIPTOR         /* ViString, read-only  */
   
    /*- Instrument-Specific Attributes -------------------------------------*/
    /*- Basic Instrument Operations ----------------------------------------*/
#define ARPPS_ATTR_VOLTAGE_LEVEL             IVIDCPWR_ATTR_VOLTAGE_LEVEL           /* ViReal64,  multi-channel, volts */
#define ARPPS_ATTR_OVP_ENABLED               IVIDCPWR_ATTR_OVP_ENABLED             /* ViBoolean, multi-channel        */
#define ARPPS_ATTR_OVP_LIMIT                 IVIDCPWR_ATTR_OVP_LIMIT               /* ViReal64,  multi-channel, volts */
#define ARPPS_ATTR_CURRENT_LIMIT_BEHAVIOR    IVIDCPWR_ATTR_CURRENT_LIMIT_BEHAVIOR  /* ViInt32,   multi-channel        */
#define ARPPS_ATTR_CURRENT_LIMIT             IVIDCPWR_ATTR_CURRENT_LIMIT           /* ViReal64,  multi-channel, amps  */
#define ARPPS_ATTR_OUTPUT_ENABLED            IVIDCPWR_ATTR_OUTPUT_ENABLED          /* ViBoolean, multi-channel        */

    /*- Trigger Attributes -------------------------------------------------*/

#define ARPPS_ATTR_ID_QUERY_RESPONSE        (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 1L)     /* ViString (Read Only) */

/**************************************************************************** 
 *------------------------ Attribute Value Defines -------------------------* 
 ****************************************************************************/
    /*- Defined values for attribute ARPPS_ATTR_CURRENT_LIMIT_BEHAVIOR ---*/
#define ARPPS_VAL_CURRENT_REGULATE           IVIDCPWR_VAL_CURRENT_REGULATE
#define ARPPS_VAL_CURRENT_TRIP               IVIDCPWR_VAL_CURRENT_TRIP    

/**************************************************************************** 
 *---------------- Instrument Driver Function Declarations -----------------* 
 ****************************************************************************/

    /*- Init and Close Functions -------------------------------------------*/
ViStatus _VI_FUNC  arpps_init (ViRsrc resourceName, ViBoolean IDQuery,
                                  ViBoolean resetDevice, ViSession *vi);
ViStatus _VI_FUNC  arpps_InitWithOptions (ViRsrc resourceName, ViBoolean IDQuery,
                                             ViBoolean resetDevice, ViConstString optionString, 
                                             ViSession *newVi);
ViStatus _VI_FUNC  arpps_close (ViSession vi);   

    /*- Coercion Info Functions --------------------------------------------*/
ViStatus _VI_FUNC  arpps_GetNextCoercionRecord (ViSession vi,
                                                   ViInt32 bufferSize,
                                                   ViChar record[]);

    /*- Locking Functions --------------------------------------------------*/
ViStatus _VI_FUNC  arpps_LockSession (ViSession vi, ViBoolean *callerHasLock);   
ViStatus _VI_FUNC  arpps_UnlockSession (ViSession vi, ViBoolean *callerHasLock);


    /*- Basic Instrument Operation -----------------------------------------*/
ViStatus _VI_FUNC arpps_ConfigureOutputEnabled (ViSession vi,
                                                  ViConstString channelName,
                                                  ViBoolean enabled);

ViStatus _VI_FUNC arpps_ConfigureOutputRange (ViSession vi,
                                                ViConstString channelName,
                                                ViInt32 rangeType,
                                                ViReal64 range);

ViStatus _VI_FUNC arpps_ConfigureCurrentLimit (ViSession vi,
                                                 ViConstString channelName,
                                                 ViInt32 behavior,
                                                 ViReal64 limit);
ViStatus _VI_FUNC arpps_ConfigureOVP (ViSession vi,
                                        ViConstString channelName,
                                        ViBoolean enabled,
                                        ViReal64 limit);

ViStatus _VI_FUNC arpps_ConfigureVoltageLevel (ViSession vi,
                                                 ViConstString channelName,
                                                 ViReal64 level);

ViStatus _VI_FUNC arpps_QueryOutputState (ViSession vi,
                                            ViConstString channelName,
                                            ViInt32 outputState,
                                            ViBoolean *inState);

ViStatus _VI_FUNC arpps_QueryMaxCurrentLimit (ViSession vi,
                                                 ViConstString channelName,
                                                 ViReal64 voltageLevel,
                                                 ViReal64 *maxCurrentLimit);

ViStatus _VI_FUNC arpps_QueryMaxVoltageLevel (ViSession vi,
                                                 ViConstString channelName,
                                                 ViReal64 currentLimit,
                                                 ViReal64 *maxVoltageLevel);

ViStatus _VI_FUNC arpps_ResetOutputProtection (ViSession vi,
                                                 ViConstString channelName);

    /*- Measuring ----------------------------------------------------------*/
ViStatus _VI_FUNC arpps_Measure (ViSession vi,
                                   ViConstString channelName,
                                   ViInt32 measurementType,
                                   ViReal64 *measurement);
    /*- Error Functions ----------------------------------------------------*/
ViStatus _VI_FUNC  arpps_error_query (ViSession vi, ViInt32 *errorCode,
                                         ViChar errorMessage[]);
ViStatus _VI_FUNC  arpps_error_message (ViSession vi, ViStatus errorCode,
                                           ViChar errorMessage[256]);
ViStatus _VI_FUNC  arpps_GetError (ViSession vi, 
                                     ViStatus *errorCode, 
                                     ViInt32 bufferSize,
                                     ViChar description[]);
ViStatus _VI_FUNC  arpps_ClearError (ViSession vi);

    /*- Channel Info Functions ---------------------------------------------*/
ViStatus _VI_FUNC arpps_GetChannelName (ViSession vi, ViInt32 index,
                                           ViInt32 bufferSize,
                                           ViChar name[]);

    /*- Interchangeability Checking Functions ------------------------------*/
ViStatus _VI_FUNC arpps_GetNextInterchangeWarning (ViSession vi, 
                                                      ViInt32 bufferSize, 
                                                      ViChar warnString[]);
ViStatus _VI_FUNC arpps_ResetInterchangeCheck (ViSession vi);
ViStatus _VI_FUNC arpps_ClearInterchangeWarnings (ViSession vi);

    /*- Utility Functions --------------------------------------------------*/
ViStatus _VI_FUNC  arpps_reset (ViSession vi);
ViStatus _VI_FUNC  arpps_ResetWithDefaults (ViSession vi);
ViStatus _VI_FUNC  arpps_Disable (ViSession vi);
ViStatus _VI_FUNC  arpps_InvalidateAllAttributes (ViSession vi);
ViStatus _VI_FUNC  arpps_self_test (ViSession vi, ViInt16 *selfTestResult,
                                       ViChar selfTestMessage[]);
ViStatus _VI_FUNC  arpps_revision_query (ViSession vi, 
                                            ViChar instrumentDriverRevision[],
                                            ViChar firmwareRevision[]);
ViStatus _VI_FUNC  arpps_WriteInstrData (ViSession vi, ViConstString writeBuffer); 
ViStatus _VI_FUNC  arpps_ReadInstrData  (ViSession vi, ViInt32 numBytes, 
                                            ViChar rdBuf[], ViInt32 *bytesRead);

    /*- Set, Get, and Check Attribute Functions ----------------------------*/
ViStatus _VI_FUNC  arpps_GetAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 *value);
ViStatus _VI_FUNC  arpps_GetAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 *value);
ViStatus _VI_FUNC  arpps_GetAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 bufSize, ViChar value[]); 
ViStatus _VI_FUNC  arpps_GetAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession *value);
ViStatus _VI_FUNC  arpps_GetAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean *value);

ViStatus _VI_FUNC  arpps_SetAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 value);
ViStatus _VI_FUNC  arpps_SetAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 value);
ViStatus _VI_FUNC  arpps_SetAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViConstString value); 
ViStatus _VI_FUNC  arpps_SetAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession value);
ViStatus _VI_FUNC  arpps_SetAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean value);

ViStatus _VI_FUNC  arpps_CheckAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 value);
ViStatus _VI_FUNC  arpps_CheckAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 value);
ViStatus _VI_FUNC  arpps_CheckAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViConstString value); 
ViStatus _VI_FUNC  arpps_CheckAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession value);
ViStatus _VI_FUNC  arpps_CheckAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean value);

    /*********************************************************
        Functions reserved for class driver use only.
        End-users should not call these functions.  
     *********************************************************/
ViStatus _VI_FUNC  arpps_IviInit (ViRsrc resourceName, ViBoolean IDQuery, 
                                     ViBoolean reset, ViSession vi);
ViStatus _VI_FUNC  arpps_IviClose (ViSession vi);   

/****************************************************************************
 *------------------------ Error And Completion Codes ----------------------*
 ****************************************************************************/
#define ARPPS_ERROR_RESET_PROTECTION_NOT_SUPPORTED   (IVI_SPECIFIC_ERROR_BASE + 1L)

/****************************************************************************
 *--------- Obsolete Inherent Instrument Attributes and functions ----------*
 ****************************************************************************/
    /*- These attributes and functions have been deprecated and may not
        be supported in future versions of this driver. -*/
 
         /* Driver Information  */
#define ARPPS_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION  /* ViInt32,  read-only  */
#define ARPPS_ATTR_SPECIFIC_DRIVER_MINOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_MINOR_VERSION  /* ViInt32,  read-only  */

        /* Error Info */
#define ARPPS_ATTR_PRIMARY_ERROR                 IVI_ATTR_PRIMARY_ERROR                  /* ViInt32   */
#define ARPPS_ATTR_SECONDARY_ERROR               IVI_ATTR_SECONDARY_ERROR                /* ViInt32   */
#define ARPPS_ATTR_ERROR_ELABORATION             IVI_ATTR_ERROR_ELABORATION              /* ViString  */

        /* Advanced Session Information */
#define ARPPS_ATTR_IO_SESSION_TYPE               IVI_ATTR_IO_SESSION_TYPE                /* ViString, read-only  */
#define ARPPS_ATTR_IO_SESSION                    IVI_ATTR_IO_SESSION                     /* ViSession, read-only */

        /*- Deprecated Attribute Identifiers for Renamed Attributes -*/
#define ARPPS_ATTR_NUM_CHANNELS                  ARPPS_ATTR_CHANNEL_COUNT             /* ViInt32,  read-only  */
#define ARPPS_ATTR_QUERY_INSTR_STATUS            ARPPS_ATTR_QUERY_INSTRUMENT_STATUS   /* ViBoolean */
#define ARPPS_ATTR_RESOURCE_DESCRIPTOR           ARPPS_ATTR_IO_RESOURCE_DESCRIPTOR    /* ViString, read-only  */


        /*- Deprecated Error Information functions -*/
ViStatus _VI_FUNC  arpps_GetErrorInfo (ViSession vi, ViStatus *primaryError, 
                                          ViStatus *secondaryError, 
                                          ViChar errorElaboration[256]);
ViStatus _VI_FUNC  arpps_ClearErrorInfo (ViSession vi);


/**************************************************************************** 
 *---------------------------- End Include File ----------------------------* 
 ****************************************************************************/
#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif
#endif /* __ARPPS_HEADER */




