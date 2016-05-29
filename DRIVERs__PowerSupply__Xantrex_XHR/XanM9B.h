/*=============================================================================
Copyright © 2002 Xantrex Technology, Inc. All rights reserved.                           
  
FILE NAME:  XanM9B.h                                        

PURPOSE: 
   Instrument Driver declarations for Xantrex XFR-XHR-XPD-HPD-XT
   Series DC Power Supplies equipped with an M9B interface card.
=============================================================================*/

#ifndef __XanM9B_HEADER
#define __XanM9B_HEADER

#include <ivi.h>
#include <ividcpwr.h>

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

/****************************************************************************
 *----------------- Instrument Driver Revision Information -----------------*
 ****************************************************************************/
#define XANM9B_MAJOR_VERSION                4     /* Instrument driver major version   */
#define XANM9B_MINOR_VERSION                30    /* Instrument driver minor version   */
                                                                
#define XANM9B_CLASS_SPEC_MAJOR_VERSION     2     /* Class specification major version */
#define XANM9B_CLASS_SPEC_MINOR_VERSION     0     /* Class specification minor version */

        
#define XANM9B_SUPPORTED_INSTRUMENT_MODELS   "XHR7_580,XHR2030,XHR3318,XHR4015,XHR6010,"\
                                             "XHR1006,XHR1504,XHR3002,XHR6001,"\
                                             "XHR7_5130,XHR2050,XHR3333,XHR4025,XHR6018,"\
                                             "XHR10010,XHR1507,XHR3003_5,XHR6001_7,"\
                                             "XFR6200,XFR7_5140,XFR12100,XFR2060,XFR3535,"\
                                             "XFR4030,XFR4530,XFR6020,XFR10012,XFR1508,"\
                                             "XFR1607_5,XFR3004,XFR6002,"\
                                             "XFR7_5300,XFR12220,XFR20130,XFR3385,XFR4070,"\
                                             "XFR5060,XFR6046,XFR8037,XFR10028,XFR15018,"\
                                             "XFR3009,XFR4207,XFR6004,"\
                                             "XPD7_567,XPD1830,XPD3316,XPD609,XPD1204_5,"\
                                             "HPD1520,HPD3010,HPD605,"\
                                             "XT76,XT154,XT203,XT302,XT601,XT1200_5,XT2500_25"
                                         
#define XANM9B_DRIVER_VENDOR                 "Xantrex Technology Inc."
#define XANM9B_DRIVER_DESCRIPTION            "DC Power Supply Driver"
                    

/**************************************************************************** 
 *------------------------------ Useful Macros -----------------------------* 
 ****************************************************************************/

    /*- Defined values for rangeType parameter of function -----------------*/
    /*- ConfigureOutputRange -----------------------------------------------*/
#define XANM9B_VAL_RANGE_CURRENT             IVIDCPWR_VAL_RANGE_CURRENT              
#define XANM9B_VAL_RANGE_VOLTAGE             IVIDCPWR_VAL_RANGE_VOLTAGE

    /*- Defined values for outputState parameter of function ---------------*/
    /*- QueryOutputState ---------------------------------------------------*/
#define XANM9B_VAL_OUTPUT_CONSTANT_VOLTAGE   IVIDCPWR_VAL_OUTPUT_CONSTANT_VOLTAGE
#define XANM9B_VAL_OUTPUT_CONSTANT_CURRENT   IVIDCPWR_VAL_OUTPUT_CONSTANT_CURRENT
#define XANM9B_VAL_OUTPUT_OVER_VOLTAGE       IVIDCPWR_VAL_OUTPUT_OVER_VOLTAGE       
#define XANM9B_VAL_OUTPUT_UNREGULATED        IVIDCPWR_VAL_OUTPUT_UNREGULATED    
#define XANM9B_VAL_OUTPUT_OVER_TEMPERATURE   (IVIDCPWR_VAL_OUTPUT_STATE_SPECIFIC_EXT_BASE + 1L)
#define XANM9B_VAL_OUTPUT_FOLD_TRIPPED       (IVIDCPWR_VAL_OUTPUT_STATE_SPECIFIC_EXT_BASE + 2L)

    /*- Defined values for measurementType parameter of function -----------*/
    /*- Measure ------------------------------------------------------------*/
#define XANM9B_VAL_MEASURE_CURRENT           IVIDCPWR_VAL_MEASURE_CURRENT
#define XANM9B_VAL_MEASURE_VOLTAGE           IVIDCPWR_VAL_MEASURE_VOLTAGE


/**************************************************************************** 
 *---------------------------- Attribute Defines ---------------------------* 
 ****************************************************************************/

    /*- IVI Inherent Instrument Attributes ---------------------------------*/    

        /* User Options */
#define XANM9B_ATTR_RANGE_CHECK                 IVI_ATTR_RANGE_CHECK                  /* ViBoolean */
#define XANM9B_ATTR_QUERY_INSTRUMENT_STATUS     IVI_ATTR_QUERY_INSTRUMENT_STATUS      /* ViBoolean */
#define XANM9B_ATTR_CACHE                       IVI_ATTR_CACHE                        /* ViBoolean */
#define XANM9B_ATTR_SIMULATE                    IVI_ATTR_SIMULATE                     /* ViBoolean */
#define XANM9B_ATTR_RECORD_COERCIONS            IVI_ATTR_RECORD_COERCIONS             /* ViBoolean */

#define XANM9B_ATTR_DRIVER_SETUP                IVI_ATTR_DRIVER_SETUP                 /* ViString */
#define XANM9B_ATTR_INTERCHANGE_CHECK           IVI_ATTR_INTERCHANGE_CHECK            /* ViBoolean */

        /* Instrument Capabilities */
#define XANM9B_ATTR_CHANNEL_COUNT               IVI_ATTR_CHANNEL_COUNT
#define XANM9B_ATTR_GROUP_CAPABILITIES          IVI_ATTR_GROUP_CAPABILITIES           /* ViString, read-only  */

        /* Driver Information  */
#define XANM9B_ATTR_SPECIFIC_DRIVER_PREFIX         IVI_ATTR_SPECIFIC_DRIVER_PREFIX         /* ViString, read-only  */
#define XANM9B_ATTR_SUPPORTED_INSTRUMENT_MODELS    IVI_ATTR_SUPPORTED_INSTRUMENT_MODELS    /* ViString, read-only  */
#define XANM9B_ATTR_INSTRUMENT_MANUFACTURER        IVI_ATTR_INSTRUMENT_MANUFACTURER        /* ViString, read-only  */
#define XANM9B_ATTR_INSTRUMENT_MODEL               IVI_ATTR_INSTRUMENT_MODEL               /* ViString, read-only  */
#define XANM9B_ATTR_INSTRUMENT_FIRMWARE_REVISION   IVI_ATTR_INSTRUMENT_FIRMWARE_REVISION   /* ViString, read-only  */
#define XANM9B_ATTR_SPECIFIC_DRIVER_REVISION       IVI_ATTR_SPECIFIC_DRIVER_REVISION       /* ViString, read-only  */
#define XANM9B_ATTR_SPECIFIC_DRIVER_VENDOR         IVI_ATTR_SPECIFIC_DRIVER_VENDOR         /* ViString, read-only  */
#define XANM9B_ATTR_SPECIFIC_DRIVER_DESCRIPTION    IVI_ATTR_SPECIFIC_DRIVER_DESCRIPTION    /* ViString, read-only  */
#define XANM9B_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION   IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION /* ViInt32, read-only */
#define XANM9B_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION   IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION /* ViInt32, read-only */

        /* Advanced Session Information */
#define XANM9B_ATTR_LOGICAL_NAME                   IVI_ATTR_LOGICAL_NAME              /* ViString, read-only  */
#define XANM9B_ATTR_IO_RESOURCE_DESCRIPTOR         IVI_ATTR_RESOURCE_DESCRIPTOR       /* ViString, read-only  */

    /*- Instrument-Specific Attributes -------------------------------------*/
    /*- Basic Instrument Operations ----------------------------------------*/
#define XANM9B_ATTR_VOLTAGE_LEVEL               IVIDCPWR_ATTR_VOLTAGE_LEVEL           /* ViReal64,  multi-channel, volts */
#define XANM9B_ATTR_OVP_LIMIT                   IVIDCPWR_ATTR_OVP_LIMIT               /* ViReal64,  multi-channel, volts */
#define XANM9B_ATTR_CURRENT_LIMIT               IVIDCPWR_ATTR_CURRENT_LIMIT           /* ViReal64,  multi-channel, amps  */
#define XANM9B_ATTR_OVP_ENABLED                 IVIDCPWR_ATTR_OVP_ENABLED             /* ViBoolean, multi-channel */
#define XANM9B_ATTR_OUTPUT_ENABLED              IVIDCPWR_ATTR_OUTPUT_ENABLED          /* ViBoolean, multi-channel */
#define XANM9B_ATTR_CURRENT_LIMIT_BEHAVIOR      IVIDCPWR_ATTR_CURRENT_LIMIT_BEHAVIOR  /* ViBoolean, multi-channel */

    /*- Trigger Attributes -------------------------------------------------*/
#define XANM9B_ATTR_TRIGGER_SOURCE              IVIDCPWR_ATTR_TRIGGER_SOURCE
#define XANM9B_ATTR_TRIGGERED_CURRENT_LIMIT     IVIDCPWR_ATTR_TRIGGERED_CURRENT_LIMIT /* ViReal64,  multi-channel, amps  */
#define XANM9B_ATTR_TRIGGERED_VOLTAGE_LEVEL     IVIDCPWR_ATTR_TRIGGERED_VOLTAGE_LEVEL /* ViReal64,  multi-channel, volts */

#define XANM9B_ATTR_ID_QUERY_RESPONSE           (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 1L)  /* ViString (Read Only) */
#define XANM9B_ATTR_AUXA_ENABLED                (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 2L)
#define XANM9B_ATTR_AUXB_ENABLED                (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 3L)
#define XANM9B_ATTR_CMODE_ENABLED               (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 4L)
#define XANM9B_ATTR_SRQ_ENABLED                 (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 5L)
#define XANM9B_ATTR_FOLDBACK_SETTING            (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 6L)
#define XANM9B_ATTR_HOLD_ENABLED                (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 7L)    
#define XANM9B_ATTR_MAXIMUM_CURRENT_LIMIT       (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 8L) 
#define XANM9B_ATTR_MASK_SETTING                (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 9L)
#define XANM9B_ATTR_UNMASK_SETTING              (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 10L)
#define XANM9B_ATTR_UNMASK_CONDITION            (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 11L)
#define XANM9B_ATTR_MAXIMUM_VOLTAGE             (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 12L)
#define XANM9B_ATTR_FAULT_DELAY                 (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 13L)
#define XANM9B_ATTR_REMOTE_MODE                 (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 14L)
    
/**************************************************************************** 
 *------------------------ Attribute Value Defines -------------------------* 
 ****************************************************************************/
    /*- Defined values for attribute XANM9B_ATTR_TRIGGER_SOURCE -----------*/
#define XANM9B_VAL_TRIG_IMMEDIATE            IVIDCPWR_VAL_TRIG_IMMEDIATE 

   /*- Defined values for attribute XANM9B_ATTR_CURRENT_LIMIT_BEHAVIOR ---*/
#define XANM9B_VAL_CURRENT_REGULATE          IVIDCPWR_VAL_CURRENT_REGULATE
#define XANM9B_VAL_CURRENT_TRIP              IVIDCPWR_VAL_CURRENT_TRIP

   /*- Defined values for attribute XANM9B_ATTR_FOLDBACK_SETTING ---------*/
#define XANM9B_VAL_FOLD_OFF                  0L
#define XANM9B_VAL_FOLD_CV                   1L
#define XANM9B_VAL_FOLD_CC                   2L

   /*- Defined values for attribute XANM9B_ATTR_MASK_SETTING ---------*/
   /*- Defined values for attribute XANM9B_ATTR_UNMASK_SETTING ---------*/ 
#define XANM9B_VAL_MASK_CV                   0L
#define XANM9B_VAL_MASK_CC                   1L
#define XANM9B_VAL_MASK_OV                   3L
#define XANM9B_VAL_MASK_OT                   4L
#define XANM9B_VAL_MASK_SD                   5L
#define XANM9B_VAL_MASK_FOLD                 6L
#define XANM9B_VAL_MASK_ERR                  7L
#define XANM9B_VAL_MASK_PON                  8L
#define XANM9B_VAL_MASK_REM                  9L
#define XANM9B_VAL_MASK_ACF                  10L
#define XANM9B_VAL_MASK_OPF                  11L
#define XANM9B_VAL_MASK_SNSP                 12L
#define XANM9B_VAL_MASK_NONE                 13L
#define XANM9B_VAL_MASK_ALL                  14L

   /*- Defined values for attribute XANM9B_ATTR_REMOTE_MODE ---------*/
#define XANM9B_VAL_LOC                       0L
#define XANM9B_VAL_REM                       1L


/**************************************************************************** 
 *---------------- Instrument Driver Function Declarations -----------------* 
 ****************************************************************************/
    /*- Init and Close Functions -------------------------------------------*/
ViStatus _VI_FUNC  XanM9B_init (ViRsrc resourceName, ViBoolean IDQuery,
                                ViBoolean resetDevice, ViSession *vi);
                                
ViStatus _VI_FUNC  XanM9B_InitWithOptions (ViRsrc resourceName, ViBoolean IDQuery,
                                           ViBoolean resetDevice, ViString optionString, 
                                           ViSession *newVi);
                                           
ViStatus _VI_FUNC XanM9B_close(ViSession vi);

    /*- Coercion Info Functions --------------------------------------------*/
ViStatus _VI_FUNC  XanM9B_GetNextCoercionRecord (ViSession vi,
                                                 ViInt32 bufferSize,
                                                 ViChar record[]);

ViStatus _VI_FUNC XanM9B_GetNextInterchangeWarning (ViSession vi,
                                                    ViInt32 bufferSize,
                                                    ViChar warnString[]);

ViStatus _VI_FUNC XanM9B_ClearInterchangeWarnings (ViSession vi);

ViStatus _VI_FUNC XanM9B_ResetInterchangeCheck (ViSession vi);

    /*- Locking Functions --------------------------------------------------*/
ViStatus _VI_FUNC  XanM9B_LockSession (ViSession vi, ViBoolean *callerHasLock);   
ViStatus _VI_FUNC  XanM9B_UnlockSession (ViSession vi, ViBoolean *callerHasLock);


    /*- Basic Instrument Operation -----------------------------------------*/
ViStatus _VI_FUNC XanM9B_ConfigureOutputEnabled (ViSession vi,
                                                 ViConstString channelName,
                                                 ViBoolean enabled);



ViStatus _VI_FUNC XanM9B_ConfigureFaultDelay (ViSession vi, ViConstString channelName,
                                              ViReal64 value);

ViStatus _VI_FUNC XanM9B_ConfigureCurrentLimit (ViSession vi,
                                                ViConstString channelName,
                                                ViInt32 behavior,
                                                ViReal64 limit);
                                                 
ViStatus _VI_FUNC XanM9B_ConfigureOVP (ViSession vi,
                                       ViConstString channelName,
                                       ViBoolean enabled,
                                       ViReal64 limit);

ViStatus _VI_FUNC XanM9B_ConfigureVoltageLevel (ViSession vi,
                                                ViConstString channelName,
                                                ViReal64 level);


ViStatus _VI_FUNC XanM9B_QueryOutputState (ViSession vi,
                                           ViConstString channelName, 
                                           ViInt32 outputState,
                                           ViBoolean *inState);

ViStatus _VI_FUNC XanM9B_MaxCurrentLimit (ViSession vi,
                                          ViConstString channelName,
                                          ViReal64 CurrentLevel);

ViStatus _VI_FUNC XanM9B_ConfigureOutputRange (ViSession vi,
                                               ViConstString channelName, 
                                               ViInt32 rangeType,
                                               ViReal64 range);

ViStatus _VI_FUNC XanM9B_MaxVoltage (ViSession vi,
                                     ViConstString channelName,
                                     ViReal64 VoltageLevel);                                                 

ViStatus _VI_FUNC XanM9B_ClearStatus (ViSession vi);

ViStatus _VI_FUNC XanM9B_ResetOutputProtection (ViSession vi,
                                                ViConstString channelName);

ViStatus _VI_FUNC XanM9B_ConfigureSrqEnabled(ViSession vi, ViConstString channelName,
                                             ViBoolean value);
                                      
ViStatus _VI_FUNC XanM9B_QueryFaultState (ViSession vi,
                                          ViConstString channelName,
                                          ViInt32 *faultRegister);

ViStatus _VI_FUNC XanM9B_QuerySetpoint (ViSession vi, ViConstString channelName,
                                        ViAttr AttrId, ViReal64 *setpointLevel);

ViStatus _VI_FUNC XanM9B_QueryMode (ViSession vi, ViConstString channelName,
                                    ViAttr attrId, ViInt32 *modeValue);

ViStatus _VI_FUNC XanM9B_QueryAccumulatedState (ViSession vi,
                                                ViConstString channelName,
                                                ViInt32 *accRegister);    
    
ViStatus _VI_FUNC XanM9B_ConfigureFoldbackSetting (ViSession vi, 
                                                   ViConstString channelName,
                                                   ViInt32 value);

    /*- Triggering ---------------------------------------------------------*/
ViStatus _VI_FUNC XanM9B_ConfigureHoldEnabled (ViSession vi,
                                               ViConstString channelName,
                                               ViBoolean enabled);

ViStatus _VI_FUNC XanM9B_Initiate (ViSession vi);

ViStatus _VI_FUNC XanM9B_Abort (ViSession vi);


ViStatus _VI_FUNC XanM9B_SendSoftwareTrigger (ViSession vi);    

ViStatus _VI_FUNC XanM9B_ConfigureTriggeredVoltageLevel (ViSession vi,
                                                         ViConstString channelName,
                                                         ViReal64 level);

ViStatus _VI_FUNC XanM9B_ConfigureTriggeredCurrentLimit (ViSession vi,
                                                         ViConstString channelName,
                                                         ViReal64 limit);
                                                          
ViStatus _VI_FUNC XanM9B_ConfigureTriggerSource (ViSession vi,
                                                 ViConstString channelName, 
                                                 ViInt32 source);

ViStatus _VI_FUNC XanM9B_ConfigureAuxaEnabled (ViSession vi,
                                               ViConstString channelName,
                                               ViBoolean enabled);

ViStatus _VI_FUNC XanM9B_ConfigureAuxbEnabled (ViSession vi,
                                               ViConstString channelName,
                                               ViBoolean enabled);
                                                  
ViStatus _VI_FUNC XanM9B_ConfigureCmodeEnabled (ViSession vi,
                                                ViConstString channelName,
                                                ViBoolean enabled);
                                                  
ViStatus _VI_FUNC XanM9B_UnmaskSetting (ViSession vi,ViConstString channelName,
                                        ViChar mnemonics[]);  

ViStatus _VI_FUNC XanM9B_MaskSetting (ViSession vi,ViConstString channelName,
                                      ViChar mnemonics[]);

ViStatus _VI_FUNC XanM9B_QueryMaxCurrentLimit (ViSession vi,
                                               ViConstString channelName,
                                               ViReal64 voltageLevel,
                                               ViReal64 *maxCurrentLimit);

ViStatus _VI_FUNC XanM9B_QueryMaxVoltageLevel (ViSession vi,
                                               ViConstString channelName,
                                               ViReal64 currentLimit,
                                               ViReal64 *maxVoltageLevel);
                                                           
ViStatus _VI_FUNC XanM9B_UnmaskCondition (ViSession vi, ViConstString channelName,
                                          ViInt32 *condition);                                                           
    /*- Measuring ----------------------------------------------------------*/
ViStatus _VI_FUNC XanM9B_Measure (ViSession vi,
                                  ViConstString channelName,
                                  ViInt32 measurementType,
                                  ViReal64 *measurement);

    /*- Error Functions ----------------------------------------------------*/
ViStatus _VI_FUNC  XanM9B_error_query (ViSession vi, ViInt32 *errorCode, 
                                       ViChar errorMessage[]);

ViStatus _VI_FUNC XanM9B_GetError (ViSession vi, ViStatus *errorCode,
                                   ViInt32 bufferSize,
                                   ViChar description[]);

ViStatus _VI_FUNC XanM9B_ClearError (ViSession vi);

ViStatus _VI_FUNC XanM9B_error_message (ViSession vi, ViStatus errorCode,
                                        ViChar errorMessage[256]);
                                        
ViStatus _VI_FUNC XanM9B_WriteInstrData (ViSession vi, ViConstString writeBuffer);

ViStatus _VI_FUNC XanM9B_ReadInstrData (ViSession vi, ViInt32 numBytes, 
                                        ViChar rdBuf[], ViInt32 *bytesRead);  
    
    /*- Utility Functions --------------------------------------------------*/
ViStatus _VI_FUNC XanM9B_reset (ViSession vi);

ViStatus _VI_FUNC XanM9B_self_test (ViSession vi, ViInt16 *testResult,
                                    ViChar testMessage[]);

ViStatus _VI_FUNC XanM9B_ResetWithDefaults (ViSession vi);

ViStatus _VI_FUNC XanM9B_Disable (ViSession vi);
                                    
ViStatus _VI_FUNC XanM9B_revision_query (ViSession vi, 
                                         ViChar instrumentDriverRevision[],
                                         ViChar firmwareRevision[]);

ViStatus _VI_FUNC XanM9B_SetRemoteMode (ViSession vi, ViInt32 state);

  
ViStatus _VI_FUNC XanM9B_InvalidateAllAttributes (ViSession vi); 

ViStatus _VI_FUNC XanM9B_GetChannelName (ViSession vi, ViInt32 index,
                                         ViInt32 bufferSize,
                                         ViChar name[]);

    /*- Set, Get, and Check Attribute Functions ----------------------------*/
ViStatus _VI_FUNC  XanM9B_GetAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 *value);
ViStatus _VI_FUNC  XanM9B_GetAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 *value);
ViStatus _VI_FUNC  XanM9B_GetAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 bufSize, ViChar value[]); 
ViStatus _VI_FUNC  XanM9B_GetAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession *value);
ViStatus _VI_FUNC  XanM9B_GetAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean *value);

ViStatus _VI_FUNC  XanM9B_SetAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 value);
ViStatus _VI_FUNC  XanM9B_SetAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 value);
ViStatus _VI_FUNC  XanM9B_SetAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViConstString value); 
ViStatus _VI_FUNC  XanM9B_SetAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession value);
ViStatus _VI_FUNC  XanM9B_SetAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean value);

ViStatus _VI_FUNC  XanM9B_CheckAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 value);
ViStatus _VI_FUNC  XanM9B_CheckAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 value);
ViStatus _VI_FUNC  XanM9B_CheckAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViConstString value); 
ViStatus _VI_FUNC  XanM9B_CheckAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession value);
ViStatus _VI_FUNC  XanM9B_CheckAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean value);

    /*********************************************************
        Functions reserved for class driver use only.
        End-users should not call these functions.  
     *********************************************************/
ViStatus _VI_FUNC  XanM9B_IviInit (ViRsrc resourceName, ViBoolean IDQuery, 
                                   ViBoolean reset, ViSession vi);
                                   
ViStatus _VI_FUNC  XanM9B_IviClose (ViSession vi);   

/****************************************************************************
 *------------------------ Error And Completion Codes ----------------------*
 ****************************************************************************/
#define XANM9B_WARNING_FUNCTION_NOT_SUPPORTED   (IVI_SPECIFIC_WARN_BASE + 1L)
#define XANM9B_WARNING_OVP_CANNOT_BE_DISABLED   (IVI_SPECIFIC_WARN_BASE + 2L)

#define XANM9B_ERROR_INVALID_PARAMETER          IVI_ERROR_INVALID_PARAMETER


/****************************************************************************
 *--------- Obsolete Inherent Instrument Attributes and functions ----------*
 ****************************************************************************/
     /* These attributes and functions have been deprecated and may not
        be supported in future versions of this driver. */
        
/*------ Driver Information ------------------------------------------------*/
#define XANM9B_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION  IVI_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION  /* ViInt32,  read-only  */
#define XANM9B_ATTR_SPECIFIC_DRIVER_MINOR_VERSION  IVI_ATTR_SPECIFIC_DRIVER_MINOR_VERSION  /* ViInt32,  read-only  */

/*------ Error Info --------------------------------------------------------*/
#define XANM9B_ATTR_PRIMARY_ERROR                  IVI_ATTR_PRIMARY_ERROR                  /* ViInt32   */
#define XANM9B_ATTR_SECONDARY_ERROR                IVI_ATTR_SECONDARY_ERROR                /* ViInt32   */
#define XANM9B_ATTR_ERROR_ELABORATION              IVI_ATTR_ERROR_ELABORATION              /* ViString  */

/*------ Advanced Session Information --------------------------------------*/
#define XANM9B_ATTR_IO_SESSION_TYPE                IVI_ATTR_IO_SESSION_TYPE                /* ViString, read-only  */
#define XANM9B_ATTR_IO_SESSION                     IVI_ATTR_IO_SESSION                     /* ViSession, read-only */

/*------ Deprecated Attribute Identifiers for Renamed Attributes -----------*/ 
#define XANM9B_ATTR_NUM_CHANNELS                   XANM9B_ATTR_CHANNEL_COUNT               /* ViInt32,  read-only  */ 
#define XANM9B_ATTR_QUERY_INSTR_STATUS             XANM9B_ATTR_QUERY_INSTRUMENT_STATUS     /* ViBoolean */
#define XANM9B_ATTR_RESOURCE_DESCRIPTOR            XANM9B_ATTR_IO_RESOURCE_DESCRIPTOR      /* ViString, read-only  */

/*------ Deprecated Error Information functions ----------------------------*/
ViStatus _VI_FUNC  XanM9B_GetErrorInfo (ViSession vi, ViStatus *primaryError, 
                                        ViStatus *secondaryError, 
                                        ViChar errorElaboration[256]);
                                        
ViStatus _VI_FUNC  XanM9B_ClearErrorInfo (ViSession vi);

/**************************************************************************** 
 *---------------------------- End Include File ----------------------------* 
 ****************************************************************************/
#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif
#endif /* __XanM9B_HEADER */




