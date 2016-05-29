/****************************************************************************
 *                       Chroma DC Power Supply 62000P Series                           
 *---------------------------------------------------------------------------
 *   Copyright (c) National Instruments 1998.  All Rights Reserved.         
 *---------------------------------------------------------------------------
 *                                                                          
 * Title:    chr62000.h                                        
 * Purpose:  Chroma DC Power Supply 62000P Series                                       
 *           instrument driver declarations.                                
 *                                                                          
 ****************************************************************************/

#ifndef __CHR62000_HEADER
#define __CHR62000_HEADER

#include <ivi.h>

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

/****************************************************************************
 *----------------- Instrument Driver Revision Information -----------------*
 ****************************************************************************/
#define CHR62000_MAJOR_VERSION                1     /* Instrument driver major version   */
#define CHR62000_MINOR_VERSION                0     /* Instrument driver minor version   */
                                                                
#define CHR62000_CLASS_SPEC_MAJOR_VERSION     2     /* Class specification major version */
#define CHR62000_CLASS_SPEC_MINOR_VERSION     0     /* Class specification minor version */

#define CHR62000_SUPPORTED_INSTRUMENT_MODELS  "62006P-100-25,62012P-30-160,62012P-80-60,62012P-100-50,62012P-600-8"
#define CHR62000_DRIVER_VENDOR                "Chroma ATE INC."
#define CHR62000_DRIVER_DESCRIPTION           "DC Power Supply 62000P Series Instrument Driver"
                    

/**************************************************************************** 
 *------------------------------ Useful Macros -----------------------------* 
 ****************************************************************************/

/**************************************************************************** 
 *---------------------------- Attribute Defines ---------------------------* 
 ****************************************************************************/

    /*- IVI Inherent Instrument Attributes ---------------------------------*/    

        /* User Options */
#define CHR62000_ATTR_RANGE_CHECK                   IVI_ATTR_RANGE_CHECK                    /* ViBoolean */
#define CHR62000_ATTR_QUERY_INSTR_STATUS            IVI_ATTR_QUERY_INSTR_STATUS             /* ViBoolean */
#define CHR62000_ATTR_CACHE                         IVI_ATTR_CACHE                          /* ViBoolean */
#define CHR62000_ATTR_SIMULATE                      IVI_ATTR_SIMULATE                       /* ViBoolean */
#define CHR62000_ATTR_RECORD_COERCIONS              IVI_ATTR_RECORD_COERCIONS               /* ViBoolean */

        /* Instrument Capabilities */
#define CHR62000_ATTR_NUM_CHANNELS                  IVI_ATTR_NUM_CHANNELS                   /* ViInt32,  read-only  */

        /* Driver Information  */
#define CHR62000_ATTR_SPECIFIC_DRIVER_PREFIX        IVI_ATTR_SPECIFIC_DRIVER_PREFIX         /* ViString, read-only  */
#define CHR62000_ATTR_SUPPORTED_INSTRUMENT_MODELS   IVI_ATTR_SUPPORTED_INSTRUMENT_MODELS    /* ViString, read-only  */
#define CHR62000_ATTR_GROUP_CAPABILITIES            IVI_ATTR_GROUP_CAPABILITIES             /* ViString, read-only  */
#define CHR62000_ATTR_INSTRUMENT_MANUFACTURER       IVI_ATTR_INSTRUMENT_MANUFACTURER        /* ViString, read-only  */
#define CHR62000_ATTR_INSTRUMENT_MODEL              IVI_ATTR_INSTRUMENT_MODEL               /* ViString, read-only  */
#define CHR62000_ATTR_INSTRUMENT_FIRMWARE_REVISION  IVI_ATTR_INSTRUMENT_FIRMWARE_REVISION              /* ViString, read-only  */
#define CHR62000_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION  /* ViInt32,  read-only  */
#define CHR62000_ATTR_SPECIFIC_DRIVER_MINOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_MINOR_VERSION  /* ViInt32,  read-only  */
#define CHR62000_ATTR_SPECIFIC_DRIVER_REVISION      IVI_ATTR_SPECIFIC_DRIVER_REVISION       /* ViString, read-only  */
#define CHR62000_ATTR_SPECIFIC_DRIVER_VENDOR        IVI_ATTR_SPECIFIC_DRIVER_VENDOR         /* ViString, read-only  */
#define CHR62000_ATTR_SPECIFIC_DRIVER_DESCRIPTION   IVI_ATTR_SPECIFIC_DRIVER_DESCRIPTION    /* ViString, read-only  */
#define CHR62000_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION /* ViInt32, read-only */
#define CHR62000_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION /* ViInt32, read-only */

        /* Error Info */
#define CHR62000_ATTR_PRIMARY_ERROR                 IVI_ATTR_PRIMARY_ERROR                  /* ViInt32   */
#define CHR62000_ATTR_SECONDARY_ERROR               IVI_ATTR_SECONDARY_ERROR                /* ViInt32   */
#define CHR62000_ATTR_ERROR_ELABORATION             IVI_ATTR_ERROR_ELABORATION              /* ViString  */

        /* Advanced Session Information */
#define CHR62000_ATTR_LOGICAL_NAME                  IVI_ATTR_LOGICAL_NAME                   /* ViString, read-only  */
#define CHR62000_ATTR_RESOURCE_DESCRIPTOR           IVI_ATTR_RESOURCE_DESCRIPTOR            /* ViString, read-only  */
#define CHR62000_ATTR_IO_SESSION_TYPE               IVI_ATTR_IO_SESSION_TYPE                /* ViString, read-only  */
#define CHR62000_ATTR_IO_SESSION                    IVI_ATTR_IO_SESSION                     /* ViSession, read-only */
    

    /*- Instrument-Specific Attributes -------------------------------------*/

#define CHR62000_ATTR_ID_QUERY_RESPONSE      (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 1L)     /* ViString (Read Only) */
#define CHR62000_ATTR_VOLTAGE               (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 2L)
#define CHR62000_ATTR_VOLT_LIMIT_HIGH       (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 3L)
#define CHR62000_ATTR_VOLT_LIMIT_LOW        (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 4L)
#define CHR62000_ATTR_VOLT_PROTECT          (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 5L)
#define CHR62000_ATTR_VOLT_SLEW             (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 6L)
#define CHR62000_ATTR_CURRENT               (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 7L)
#define CHR62000_ATTR_CURR_LIMIT_HIGH       (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 8L)
#define CHR62000_ATTR_CURR_LIMIT_LOW        (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 9L)
#define CHR62000_ATTR_CURR_PROTECT          (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 10L)
#define CHR62000_ATTR_CURR_SLEW             (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 11L)
#define CHR62000_ATTR_POWER_PROTECT         (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 12L)
#define CHR62000_ATTR_TTL_PORT              (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 14L)
#define CHR62000_ATTR_REMOTE_MODE           (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 16L)
#define CHR62000_ATTR_FOLDBACK_DELAY_TIME   (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 18L)
#define CHR62000_ATTR_PROG_SELECTED         (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 21L)
#define CHR62000_ATTR_PROG_SEQ_SELECTED     (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 26L)
#define CHR62000_ATTR_STATUS_ESE            (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 27L)
#define CHR62000_ATTR_STATUS_SRE            (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 28L)

    
/**************************************************************************** 
 *------------------------ Attribute Value Defines -------------------------* 
 ****************************************************************************/

        /* Instrument specific attribute value definitions */


/**************************************************************************** 
 *---------------- Instrument Driver Function Declarations -----------------* 
 ****************************************************************************/

    /*- Init and Close Functions -------------------------------------------*/
ViStatus _VI_FUNC  chr62000_init (ViRsrc resourceName, ViBoolean IDQuery,
                                  ViBoolean resetDevice, ViSession *vi);

ViStatus _VI_FUNC chr62000_InitInterface (ViRsrc resourceName, ViBoolean IDQuery,
                                          ViBoolean resetDevice,
                                          ViInt32 baudRate, ViChar IDString[],
                                          ViChar modelNumber[],
                                          ViSession *newVi);

ViStatus _VI_FUNC  chr62000_InitWithOptions (ViRsrc resourceName, ViBoolean IDQuery,
                                             ViBoolean resetDevice, ViString optionString, 
                                             ViSession *newVi);
ViStatus _VI_FUNC  chr62000_close (ViSession vi);   

    /*- Coercion Info Functions --------------------------------------------*/
ViStatus _VI_FUNC  chr62000_GetNextCoercionRecord (ViSession vi,
                                                   ViInt32 bufferSize,
                                                   ViChar record[]);

    /*- Locking Functions --------------------------------------------------*/
ViStatus _VI_FUNC  chr62000_LockSession (ViSession vi, ViBoolean *callerHasLock);   
ViStatus _VI_FUNC  chr62000_UnlockSession (ViSession vi, ViBoolean *callerHasLock);

         
    /*- Error Functions ----------------------------------------------------*/
ViStatus _VI_FUNC  chr62000_error_query (ViSession vi, ViInt32 *errorCode,
                                         ViChar errorMessage[]);
ViStatus _VI_FUNC  chr62000_GetErrorInfo (ViSession vi, ViStatus *primaryError, 
                                          ViStatus *secondaryError, 
                                          ViChar errorElaboration[256]);
ViStatus _VI_FUNC  chr62000_ClearErrorInfo (ViSession vi);
ViStatus _VI_FUNC  chr62000_error_message (ViSession vi, ViStatus errorCode,
                                           ViChar errorMessage[256]);
    

ViStatus _VI_FUNC chr62000_Measure (ViSession vi, ViInt32 measureType,
                                 ViReal64 *readValue);


ViStatus _VI_FUNC chr62000_Fetch (ViSession vi, ViInt32 fetchType,
                                  ViReal64 *readValue);

ViStatus _VI_FUNC chr62000_QueryBeeper (ViSession vi,
                                        ViChar beeper[]);

ViStatus _VI_FUNC chr62000_QueryOutput (ViSession vi,
                                        ViChar output[]);

ViStatus _VI_FUNC chr62000_QueryFoldback (ViSession vi,
                                          ViChar foldback[],
                                          ViReal64 *foldbackDelayTime);

ViStatus _VI_FUNC chr62000_QueryAPGMode (ViSession vi,
                                         ViChar APGMode[],
                                         ViChar APGReferenceVoltage[]);
                                              
ViStatus _VI_FUNC chr62000_QueryProgPara (ViSession vi,
                                          ViInt32 programSelected,
                                          ViInt32 sequenceSelected,
                                          ViInt32 *link, ViInt32 *count,
                                          ViChar type[], ViReal64 *voltage,
                                          ViReal64 *voltageSlew,
                                          ViReal64 *current,
                                          ViReal64 *currentSlew, ViInt32 *TTL,
                                          ViReal64 *time);

ViStatus _VI_FUNC chr62000_QueryProgRunState (ViSession vi,
                                              ViChar programRunningState[]);

ViStatus _VI_FUNC chr62000_QueryESR (ViSession vi,
                                     ViInt32 *statusRegister);

ViStatus _VI_FUNC chr62000_QuerySTB (ViSession vi,
                                     ViInt32 *statusByte);

    /*- Utility Functions --------------------------------------------------*/
ViStatus _VI_FUNC  chr62000_reset (ViSession vi);
ViStatus _VI_FUNC  chr62000_self_test (ViSession vi, ViInt16 *selfTestResult,
                                       ViChar selfTestMessage[]);
ViStatus _VI_FUNC  chr62000_revision_query (ViSession vi, 
                                            ViChar instrumentDriverRevision[],
                                            ViChar firmwareRevision[]);
ViStatus _VI_FUNC  chr62000_WriteInstrData (ViSession vi, ViConstString writeBuffer); 
ViStatus _VI_FUNC  chr62000_ReadInstrData  (ViSession vi, ViInt32 numBytes, 
                                            ViChar rdBuf[], ViInt32 *bytesRead);

    /*- Set, Get, and Check Attribute Functions ----------------------------*/
ViStatus _VI_FUNC  chr62000_GetAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 *value);
ViStatus _VI_FUNC  chr62000_GetAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 *value);
ViStatus _VI_FUNC  chr62000_GetAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 bufSize, ViChar value[]); 
ViStatus _VI_FUNC  chr62000_GetAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession *value);
ViStatus _VI_FUNC  chr62000_GetAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean *value);

ViStatus _VI_FUNC chr62000_SetVoltage (ViSession vi,
                                       ViReal64 voltage);

ViStatus _VI_FUNC chr62000_SetVoltageLimit (ViSession vi,
                                            ViBoolean highLow,
                                            ViReal64 voltageLimit);

ViStatus _VI_FUNC chr62000_SetVoltageProtect (ViSession vi,
                                              ViReal64 voltageProtect);

ViStatus _VI_FUNC chr62000_SetVoltageSlew (ViSession vi,
                                           ViReal64 voltageSlew);

ViStatus _VI_FUNC chr62000_SetCurrent (ViSession vi,
                                       ViReal64 current);

ViStatus _VI_FUNC chr62000_SetCurrentLimit (ViSession vi,
                                            ViBoolean highLow,
                                            ViReal64 currentLimit);

ViStatus _VI_FUNC chr62000_SetCurrentProtect (ViSession vi,
                                              ViReal64 currentProtect);

ViStatus _VI_FUNC chr62000_SetCurrentSlew (ViSession vi,
                                           ViReal64 currentSlew);

ViStatus _VI_FUNC chr62000_SetPowerProtect (ViSession vi,
                                            ViReal64 powerProtect);

ViStatus _VI_FUNC chr62000_SetOutput (ViSession vi,
                                      ViBoolean output);

ViStatus _VI_FUNC chr62000_SetTTLPort (ViSession vi,
                                       ViInt32 TTLValue);

ViStatus _VI_FUNC chr62000_SetBeeper (ViSession vi,
                                      ViBoolean beeper);

ViStatus _VI_FUNC chr62000_SetRmtMode (ViSession vi,
                                       ViBoolean remoteMode);

ViStatus _VI_FUNC chr62000_SetFoldbackProtect (ViSession vi,
                                               ViInt32 foldbackProtect,
                                               ViReal64 foldbackDelayTime);

ViStatus _VI_FUNC chr62000_SetAPGMode (ViSession vi,
                                       ViInt32 APGMode,
                                       ViBoolean APGReferenceVoltage);

ViStatus _VI_FUNC chr62000_ProgSel (ViSession vi,
                                    ViInt32 programSelected, ViInt32 link,
                                    ViInt32 count, ViBoolean programClear);

ViStatus _VI_FUNC chr62000_SetSeqPara (ViSession vi,
                                       ViInt32 sequenceSelected, ViInt32 type,
                                       ViReal64 voltage, ViReal64 voltageSlew,
                                       ViReal64 current, ViReal64 currentSlew,
                                       ViInt32 TTL, ViReal64 time);
														 
ViStatus _VI_FUNC chr62000_ProgRun (ViSession vi, ViBoolean run);

ViStatus _VI_FUNC chr62000_SaveProgram (ViSession vi);

ViStatus _VI_FUNC chr62000_SetCLS (ViSession vi);

ViStatus _VI_FUNC chr62000_SetESE (ViSession vi,
                                   ViInt32 enableRegister);

ViStatus _VI_FUNC chr62000_SetSRE (ViSession vi,
                                   ViInt32 enableRegister);

ViStatus _VI_FUNC chr62000_Save (ViSession vi);

ViStatus _VI_FUNC chr62000_Recall (ViSession vi);

ViStatus _VI_FUNC  chr62000_SetAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 value);
ViStatus _VI_FUNC  chr62000_SetAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 value);
ViStatus _VI_FUNC  chr62000_SetAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViConstString value); 
ViStatus _VI_FUNC  chr62000_SetAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession value);
ViStatus _VI_FUNC  chr62000_SetAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean value);

ViStatus _VI_FUNC  chr62000_CheckAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 value);
ViStatus _VI_FUNC  chr62000_CheckAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 value);
ViStatus _VI_FUNC  chr62000_CheckAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViConstString value); 
ViStatus _VI_FUNC  chr62000_CheckAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession value);
ViStatus _VI_FUNC  chr62000_CheckAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean value);

    /*********************************************************
        Functions reserved for class driver use only.
        End-users should not call these functions.  
     *********************************************************/
ViStatus _VI_FUNC  chr62000_IviInit (ViRsrc resourceName, ViBoolean IDQuery, 
                                     ViBoolean reset, ViSession vi);
ViStatus _VI_FUNC  chr62000_IviClose (ViSession vi);   

/****************************************************************************
 *------------------------ Error And Completion Codes ----------------------*
 ****************************************************************************/
#define CHR62000_ERROR_OUT_OF_RANGE					(IVI_SPECIFIC_ERROR_BASE + 1)
#define CHR62000_ERROR_OUT_OF_VOLT_LIMIT			(IVI_SPECIFIC_ERROR_BASE + 2)
#define CHR62000_ERROR_OUT_OF_CURR_LIMIT			(IVI_SPECIFIC_ERROR_BASE + 3) 
#define	CHR62000_ERROR_OUT_OF_OVP					(IVI_SPECIFIC_ERROR_BASE + 4)
#define CHR62000_ERROR_OUT_OF_OCP					(IVI_SPECIFIC_ERROR_BASE + 5)
#define CHR62000_ERROR_OUT_OF_MAXPOWER				(IVI_SPECIFIC_ERROR_BASE + 6)
#define CHR62000_ERROR_INVALID_INTERFACE			(IVI_SPECIFIC_ERROR_BASE + 7)
#define CHR62000_ERROR_INVALID_PARAMETER			(IVI_SPECIFIC_ERROR_BASE + 8)
#define CHR62000_ERROR_EXECUTION					(IVI_SPECIFIC_ERROR_BASE + 9)

/**************************************************************************** 
 *---------------------------- End Include File ----------------------------* 
 ****************************************************************************/

#define CHR62000_VAL_MEAS_VOLT                                      0
#define CHR62000_VAL_MEAS_CURR                                      1
#define CHR62000_VAL_MEAS_POWER                                     2

#define CHR62000_VAL_FETCH_VOLT                                     0
#define CHR62000_VAL_FETCH_CURR                                     1
#define CHR62000_VAL_FETCH_POWER                                    2

#define CHR62000_VAL_OUTPUT_OFF                                     0
#define CHR62000_VAL_OUTPUT_ON                                      1

#define CHR62000_VAL_AUTO                                           0
#define CHR62000_VAL_MANUAL                                         1 
#define CHR62000_VAL_TRIGGER                                        2

#define CHR62000_VAL_FOLD_DISABLE                                   0
#define CHR62000_VAL_FOLD_CVTOCC                                    1
#define CHR62000_VAL_FOLD_CCTOCV                                    2

#define CHR62000_VAL_APG_NONE                                       0
#define CHR62000_VAL_APG_V                                          1
#define CHR62000_VAL_APG_I                                          2
#define CHR62000_VAL_APG_VI                                         3

#define CHR62000_VAL_62006P_100_25_V                                100.000000
#define CHR62000_VAL_62006P_100_25_I                                25.000000
#define CHR62000_VAL_62006P_100_25_P                                600.000000
#define CHR62000_VAL_62012P_30_160_V                                30.000000
#define CHR62000_VAL_62012P_30_160_I                                160.000000
#define CHR62000_VAL_62012P_30_160_P                                1200.000
#define CHR62000_VAL_62012P_80_60_V                                 80.000000
#define CHR62000_VAL_62012P_80_60_I                                 60.000000
#define CHR62000_VAL_62012P_80_60_P                                 1200.000
#define CHR62000_VAL_62012P_100_50_V                                100.000000
#define CHR62000_VAL_62012P_100_50_I                                50.000000
#define CHR62000_VAL_62012P_100_50_P                                1200.000
#define CHR62000_VAL_62012P_600_8_V                                 600.000000
#define CHR62000_VAL_62012P_600_8_I                                 8.000000
#define CHR62000_VAL_62012P_600_8_P                                 1200.000


#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif
#endif /* __CHR62000_HEADER */




