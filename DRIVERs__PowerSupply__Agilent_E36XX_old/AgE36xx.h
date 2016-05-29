/******************************************************************************
 *                                                                         
 *               Agilent Technologies
 *               All rights reserved.
 *
 *****************************************************************************/

#ifndef __AGE36XX_HEADER
#define __AGE36XX_HEADER

#include <IviVisaType.h>
#include <unknwn.h>

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

/**************************************************************************** 
 *---------------------------- Attribute Defines ---------------------------* 
 ****************************************************************************/
#ifndef IVI_ATTR_BASE
#define IVI_ATTR_BASE                 1000000
#endif

#ifndef IVI_INHERENT_ATTR_BASE		        
#define IVI_INHERENT_ATTR_BASE		  (IVI_ATTR_BASE +  50000)   /* base for inherent capability attributes */
#endif

#ifndef IVI_CLASS_ATTR_BASE           
#define IVI_CLASS_ATTR_BASE           (IVI_ATTR_BASE + 250000)   /* base for IVI-defined class attributes */
#endif

#ifndef IVI_LXISYNC_ATTR_BASE         
#define IVI_LXISYNC_ATTR_BASE         (IVI_ATTR_BASE + 950000)   /* base for IviLxiSync attributes */
#endif

#ifndef IVI_SPECIFIC_ATTR_BASE        
#define IVI_SPECIFIC_ATTR_BASE        (IVI_ATTR_BASE + 150000)   /* base for attributes of specific drivers */
#endif


/*===== IVI Inherent Instrument Attributes ==============================*/    

    /*- Driver Identification */
#define	AGE36XX_ATTR_SPECIFIC_DRIVER_DESCRIPTION                  (IVI_INHERENT_ATTR_BASE + 514L)  /* ViString, read-only */
#define	AGE36XX_ATTR_SPECIFIC_DRIVER_PREFIX                       (IVI_INHERENT_ATTR_BASE + 302L)  /* ViString, read-only */
#define	AGE36XX_ATTR_SPECIFIC_DRIVER_VENDOR                       (IVI_INHERENT_ATTR_BASE + 513L)  /* ViString, read-only */
#define	AGE36XX_ATTR_SPECIFIC_DRIVER_REVISION                     (IVI_INHERENT_ATTR_BASE + 551L)  /* ViString, read-only */
#define	AGE36XX_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION     (IVI_INHERENT_ATTR_BASE + 515L)  /* ViInt32, read-only */
#define	AGE36XX_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION     (IVI_INHERENT_ATTR_BASE + 516L)  /* ViInt32, read-only */

    /*- Advanced Session Information */
#define	AGE36XX_ATTR_LOGICAL_NAME                       (IVI_INHERENT_ATTR_BASE + 305L)  /* ViString, read-only */
#define	AGE36XX_ATTR_IO_RESOURCE_DESCRIPTOR             (IVI_INHERENT_ATTR_BASE + 304L)  /* ViString, read-only */
#define	AGE36XX_ATTR_DRIVER_SETUP                       (IVI_INHERENT_ATTR_BASE + 7L)    /* ViString, read-only */

    /*- User Options */
#define	AGE36XX_ATTR_CACHE                              (IVI_INHERENT_ATTR_BASE + 4L)   /* ViBoolean, read-write */
#define	AGE36XX_ATTR_INTERCHANGE_CHECK                  (IVI_INHERENT_ATTR_BASE + 21L)  /* ViBoolean, read-write */
#define	AGE36XX_ATTR_QUERY_INSTRUMENT_STATUS            (IVI_INHERENT_ATTR_BASE + 3L)   /* ViBoolean, read-write */
#define	AGE36XX_ATTR_RANGE_CHECK                        (IVI_INHERENT_ATTR_BASE + 2L)   /* ViBoolean, read-write */
#define	AGE36XX_ATTR_RECORD_COERCIONS                   (IVI_INHERENT_ATTR_BASE + 6L)   /* ViBoolean, read-write */
#define	AGE36XX_ATTR_SIMULATE                           (IVI_INHERENT_ATTR_BASE + 5L)   /* ViBoolean, read-write */

    /*- Instrument Identification */
#define	AGE36XX_ATTR_INSTRUMENT_MANUFACTURER            (IVI_INHERENT_ATTR_BASE + 511L)  /* ViString, read-only */
#define	AGE36XX_ATTR_INSTRUMENT_MODEL                   (IVI_INHERENT_ATTR_BASE + 512L)  /* ViString, read-only */
#define	AGE36XX_ATTR_INSTRUMENT_FIRMWARE_REVISION       (IVI_INHERENT_ATTR_BASE + 510L)  /* ViString, read-only */

    /*- Driver Capabilities */
#define	AGE36XX_ATTR_SUPPORTED_INSTRUMENT_MODELS        (IVI_INHERENT_ATTR_BASE + 327L)  /* ViString, read-only */
#define	AGE36XX_ATTR_GROUP_CAPABILITIES                 (IVI_INHERENT_ATTR_BASE + 401L)  /* ViString, read-only */


/*===== Instrument-Specific Attributes =====================================*/

    /*- Calibrations */
#define	AGE36XX_ATTR_CALIBRATION_COUNT                  (IVI_SPECIFIC_ATTR_BASE + 104L)  /* ViInt32, read-only */
#define	AGE36XX_ATTR_CALIBRATIONS_ENABLED               (IVI_SPECIFIC_ATTR_BASE + 1L)    /* ViBoolean, read-only */
#define	AGE36XX_ATTR_CALIBRATIONS_PASSWORD              (IVI_SPECIFIC_ATTR_BASE + 2L)    /* ViString, write-only */
#define	AGE36XX_ATTR_CALIBRATIONS_STRING                (IVI_SPECIFIC_ATTR_BASE + 3L)    /* ViString, read-write */
#define	AGE36XX_ATTR_CALIBRATIONS_TIMES                 (IVI_SPECIFIC_ATTR_BASE + 4L)    /* ViInt32, read-only */

    /*- Outputs */
#define	AGE36XX_ATTR_OUTPUT_COUNT                       (IVI_SPECIFIC_ATTR_BASE + 105L)  /* ViInt32, read-only */
#define	AGE36XX_ATTR_OUTPUTS_CHANNEL                    (IVI_SPECIFIC_ATTR_BASE + 8L)    /* ViInt32, read-write */
#define	AGE36XX_ATTR_OUTPUTS_ENABLED                    (IVI_SPECIFIC_ATTR_BASE + 9L)    /* ViBoolean, read-write */
#define	AGE36XX_ATTR_OUTPUTS_RELAY_ENABLED              (IVI_SPECIFIC_ATTR_BASE + 10L)   /* ViBoolean, read-write */
#define	AGE36XX_ATTR_OUTPUTS_TRACKING_ENABLED           (IVI_SPECIFIC_ATTR_BASE + 11L)   /* ViBoolean, read-write */

    /*- Triggers */
#define	AGE36XX_ATTR_TRIGGER_COUNT                      (IVI_SPECIFIC_ATTR_BASE + 106L)  /* ViInt32, read-only */
#define	AGE36XX_ATTR_TRIGGERS_COUPLING                  (IVI_SPECIFIC_ATTR_BASE + 27L)   /* ViInt32, read-write */

    /*- Output */
#define	AGE36XX_ATTR_CHANNEL_COUNT                      (IVI_INHERENT_ATTR_BASE + 203L)  /* ViInt32, read-only */
#define	AGE36XX_ATTR_OUTPUT_VOLTAGE_RANGE               (IVI_SPECIFIC_ATTR_BASE + 15L)   /* ViInt32, read-write */
#define	AGE36XX_ATTR_OUTPUT_ENABLED                     (IVI_CLASS_ATTR_BASE + 6L)       /* ViBoolean, read-write */
#define	AGE36XX_ATTR_VOLTAGE_LEVEL                      (IVI_CLASS_ATTR_BASE + 1L)       /* ViReal64, read-write */
#define	AGE36XX_ATTR_CURRENT_LIMIT                      (IVI_CLASS_ATTR_BASE + 5L)       /* ViReal64, read-write */
#define	AGE36XX_ATTR_CURRENT_LIMIT_BEHAVIOR             (IVI_CLASS_ATTR_BASE + 4L)       /* ViInt32, read-write */
#define	AGE36XX_ATTR_OVP_LIMIT                          (IVI_CLASS_ATTR_BASE + 3L)       /* ViReal64, read-write */
#define	AGE36XX_ATTR_OVP_ENABLED                        (IVI_CLASS_ATTR_BASE + 2L)       /* ViBoolean, read-write */

    /*- Protection */
#define	AGE36XX_ATTR_OUTPUT_PROTECTION_OCP_ENABLED      (IVI_SPECIFIC_ATTR_BASE + 16L)  /* ViBoolean, read-write */
#define	AGE36XX_ATTR_OUTPUT_PROTECTION_OCP_LIMIT        (IVI_SPECIFIC_ATTR_BASE + 17L)  /* ViReal64, read-write */
#define	AGE36XX_ATTR_OUTPUT_PROTECTION_OCP_TRIPPED      (IVI_SPECIFIC_ATTR_BASE + 18L)  /* ViBoolean, read-only */
#define	AGE36XX_ATTR_OUTPUT_PROTECTION_OVP_ENABLED      (IVI_SPECIFIC_ATTR_BASE + 19L)  /* ViBoolean, read-write */
#define	AGE36XX_ATTR_OUTPUT_PROTECTION_OVP_LIMIT        (IVI_SPECIFIC_ATTR_BASE + 20L)  /* ViReal64, read-write */
#define	AGE36XX_ATTR_OUTPUT_PROTECTION_OVP_TRIPPED      (IVI_SPECIFIC_ATTR_BASE + 21L)  /* ViBoolean, read-only */

    /*- Display */
#define	AGE36XX_ATTR_DISPLAY_ENABLED                    (IVI_SPECIFIC_ATTR_BASE + 5L)  /* ViBoolean, read-write */
#define	AGE36XX_ATTR_DISPLAY_MODE                       (IVI_SPECIFIC_ATTR_BASE + 6L)  /* ViInt32, read-write */
#define	AGE36XX_ATTR_DISPLAY_TEXT                       (IVI_SPECIFIC_ATTR_BASE + 7L)  /* ViString, read-write */

    /*- Status */
#define	AGE36XX_ATTR_STATUS_SERIAL_POLL                 (IVI_SPECIFIC_ATTR_BASE + 22L)  /* ViInt32, read-only */

    /*- System */
#define	AGE36XX_ATTR_SYSTEM_MODEL                       (IVI_SPECIFIC_ATTR_BASE + 24L)  /* ViInt32, read-write */
#define	AGE36XX_ATTR_SYSTEM_TIMEOUT_MILLISECONDS        (IVI_SPECIFIC_ATTR_BASE + 25L)  /* ViInt32, read-write */
#define	AGE36XX_ATTR_SYSTEM_TRACE_ENABLED               (IVI_SPECIFIC_ATTR_BASE + 26L)  /* ViBoolean, read-write */

    /*- Trigger */
#define	AGE36XX_ATTR_TRIGGER_SOURCE                     (IVI_CLASS_ATTR_BASE + 101L)    /* ViInt32, read-write */
#define	AGE36XX_ATTR_TRIGGERED_CURRENT_LIMIT            (IVI_CLASS_ATTR_BASE + 102L)    /* ViReal64, read-write */
#define	AGE36XX_ATTR_TRIGGERED_VOLTAGE_LEVEL            (IVI_CLASS_ATTR_BASE + 103L)    /* ViReal64, read-write */
#define	AGE36XX_ATTR_TRIGGER_CURRENT_LIMIT              (IVI_SPECIFIC_ATTR_BASE + 28L)  /* ViReal64, read-write */
#define	AGE36XX_ATTR_TRIGGER_DELAY                      (IVI_SPECIFIC_ATTR_BASE + 29L)  /* ViReal64, read-write */
#define	AGE36XX_ATTR_TRIGGER_VOLTAGE_LEVEL              (IVI_SPECIFIC_ATTR_BASE + 31L)  /* ViReal64, read-write */
		


/**************************************************************************** 
 *------------------------ Attribute Value Defines -------------------------* 
 ****************************************************************************/

    /*- Defined values for attribute AGE36XX_ATTR_TRIGGER_SOURCE */
#define	AGE36XX_VAL_TRIG_IMMEDIATE                      0
#define	AGE36XX_VAL_TRIG_EXTERNAL                       1
#define	AGE36XX_VAL_SOFTWARE_TRIG                       2
#define	AGE36XX_VAL_TRIG_TTL0                           3
#define	AGE36XX_VAL_TRIG_TTL1                           4
#define	AGE36XX_VAL_TRIG_TTL2                           5
#define	AGE36XX_VAL_TRIG_TTL3                           6
#define	AGE36XX_VAL_TRIG_TTL4                           7
#define	AGE36XX_VAL_TRIG_TTL5                           8
#define	AGE36XX_VAL_TRIG_TTL6                           9
#define	AGE36XX_VAL_TRIG_TTL7                           10
#define	AGE36XX_VAL_TRIG_ECL0                           11
#define	AGE36XX_VAL_TRIG_ECL1                           12
#define	AGE36XX_VAL_TRIG_PXI_STAR                       13
#define	AGE36XX_VAL_TRIG_RTSI_0                         14
#define	AGE36XX_VAL_TRIG_RTSI_1                         15
#define	AGE36XX_VAL_TRIG_RTSI_2                         16
#define	AGE36XX_VAL_TRIG_RTSI_3                         17
#define	AGE36XX_VAL_TRIG_RTSI_4                         18
#define	AGE36XX_VAL_TRIG_RTSI_5                         19
#define	AGE36XX_VAL_TRIG_RTSI_6                         20

    /*- Defined values for attribute AGE36XX_ATTR_CURRENT_LIMIT_BEHAVIOR, parameter Behavior in function AgE36xx_ConfigureCurrentLimit */
#define	AGE36XX_VAL_CURRENT_REGULATE                    0
#define	AGE36XX_VAL_CURRENT_TRIP                        1

    /*- Defined values for parameter OutputState in function AgE36xx_QueryOutputState */
#define	AGE36XX_VAL_OUTPUT_CONSTANT_VOLTAGE             0
#define	AGE36XX_VAL_OUTPUT_CONSTANT_CURRENT             1
#define	AGE36XX_VAL_OUTPUT_OVER_VOLTAGE                 2
#define	AGE36XX_VAL_OUTPUT_OVER_CURRENT                 3
#define	AGE36XX_VAL_OUTPUT_UNREGULATED                  4

    /*- Defined values for parameter MeasurementType in function AgE36xx_Measure */
#define	AGE36XX_VAL_MEASURE_CURRENT                     0
#define	AGE36XX_VAL_MEASURE_VOLTAGE                     1

    /*- Defined values for parameter RangeType in function AgE36xx_ConfigureOutputRange */
#define	AGE36XX_VAL_RANGE_CURRENT                       0
#define	AGE36XX_VAL_RANGE_VOLTAGE                       1

    /*- Defined values for parameter Level in function AgE36xx_CalibrationCurrentLevel, 
        parameter Level in function AgE36xx_CalibrationVoltageLevel */
#define	AGE36XX_VAL_AGILENT_E36XX_CALIBRATION_LEVEL_MIN 0
#define	AGE36XX_VAL_AGILENT_E36XX_CALIBRATION_LEVEL_MID 1
#define	AGE36XX_VAL_AGILENT_E36XX_CALIBRATION_LEVEL_MAX 2

    /*- Defined values for attribute AGE36XX_ATTR_OUTPUTS_CHANNEL */
#define	AGE36XX_VAL_AGILENT_E36XX_OUTPUT1               1
#define	AGE36XX_VAL_AGILENT_E36XX_OUTPUT2               2
#define	AGE36XX_VAL_AGILENT_E36XX_OUTPUT3               3

    /*- Defined values for attribute AGE36XX_ATTR_DISPLAY_MODE */
#define	AGE36XX_VAL_AGILENT_E36XX_DISPLAY_MODE_VOLT_CURRENT 0
#define	AGE36XX_VAL_AGILENT_E36XX_DISPLAY_MODE_VOLT_VOLT 1
#define	AGE36XX_VAL_AGILENT_E36XX_DISPLAY_MODE_CURRENT_CURRENT 2

    /*- Defined values for attribute AGE36XX_ATTR_SYSTEM_MODEL */
#define	AGE36XX_VAL_AGILENT_E36XX_NOT_ASSIGNED          0
#define	AGE36XX_VAL_AGILENT_E36XX_MODEL3631             1
#define	AGE36XX_VAL_AGILENT_E36XX_MODEL3632             2
#define	AGE36XX_VAL_AGILENT_E36XX_MODEL3633             3
#define	AGE36XX_VAL_AGILENT_E36XX_MODEL3634             4
#define	AGE36XX_VAL_AGILENT_E36XX_MODEL3640             5
#define	AGE36XX_VAL_AGILENT_E36XX_MODEL3641             6
#define	AGE36XX_VAL_AGILENT_E36XX_MODEL3642             7
#define	AGE36XX_VAL_AGILENT_E36XX_MODEL3643             8
#define	AGE36XX_VAL_AGILENT_E36XX_MODEL3644             9
#define	AGE36XX_VAL_AGILENT_E36XX_MODEL3645             10
#define	AGE36XX_VAL_AGILENT_E36XX_MODEL3646             11
#define	AGE36XX_VAL_AGILENT_E36XX_MODEL3647             12
#define	AGE36XX_VAL_AGILENT_E36XX_MODEL3648             13
#define	AGE36XX_VAL_AGILENT_E36XX_MODEL3649             14
#define	AGE36XX_VAL_AGILENT_E36XX_MODEL_NONE_SUPPORT    99

    /*- Defined values for attribute AGE36XX_ATTR_TRIGGERS_COUPLING */
#define	AGE36XX_VAL_AGILENT_E36XX_COUPLING_ON           0
#define	AGE36XX_VAL_AGILENT_E36XX_COUPLING_OFF          1
#define	AGE36XX_VAL_AGILENT_E36XX_COUPLING_ALL          2
#define	AGE36XX_VAL_AGILENT_E36XX_COUPLING_NONE         3
#define	AGE36XX_VAL_AGILENT_E36XX_COUPLING_P6P25        4
#define	AGE36XX_VAL_AGILENT_E36XX_COUPLING_P6N25        5
#define	AGE36XX_VAL_AGILENT_E36XX_COUPLING_P25N25       6

    /*- Defined values for attribute AGE36XX_ATTR_OUTPUT_VOLTAGE_RANGE */
#define	AGE36XX_VAL_AGILENT_E36XX_VOLTAGE_RANGE_LOW     0
#define	AGE36XX_VAL_AGILENT_E36XX_VOLTAGE_RANGE_HIGH    1

    /*- Defined values for parameter Register in function AgE36xx_StatusQueryRegister, 
        parameter Register in function AgE36xx_StatusConfigureRegister, parameter Register in function AgE36xx_StatusGetRegister, 
        parameter Register in function AgE36xx_StatusSetRegister */
#define	AGE36XX_VAL_AGILENT_E36XX_STATUS_REGISTER_STATUS_BYTE 0
#define	AGE36XX_VAL_AGILENT_E36XX_STATUS_REGISTER_STANDARD_EVENT 1
#define	AGE36XX_VAL_AGILENT_E36XX_STATUS_REGISTER_QUESTIONABLE 2
#define	AGE36XX_VAL_AGILENT_E36XX_STATUS_REGISTER_INSTRUMENT 3
#define	AGE36XX_VAL_AGILENT_E36XX_STATUS_REGISTER_INSTRUMENT1 4
#define	AGE36XX_VAL_AGILENT_E36XX_STATUS_REGISTER_INSTRUMENT2 5
#define	AGE36XX_VAL_AGILENT_E36XX_STATUS_REGISTER_INSTRUMENT3 6

    /*- Defined values for parameter SubRegister in function AgE36xx_StatusQueryRegister, 
        parameter SubRegister in function AgE36xx_StatusConfigureRegister, parameter SubRegister in function AgE36xx_StatusGetRegister, 
        parameter SubRegister in function AgE36xx_StatusSetRegister */
#define	AGE36XX_VAL_AGILENT_E36XX_STATUS_SUB_REGISTER_EVENT 0
#define	AGE36XX_VAL_AGILENT_E36XX_STATUS_SUB_REGISTER_ENABLE 1
#define	AGE36XX_VAL_AGILENT_E36XX_STATUS_SUB_REGISTER_SUMMARY 2

    /*- Defined values for parameter OutputState in function AgE36xx_OutputQueryState */
#define	AGE36XX_VAL_AGILENT_E36XX_OUTPUT_CONSTANT_VOLTAGE 0
#define	AGE36XX_VAL_AGILENT_E36XX_OUTPUT_CONSTANT_CURRENT 1
#define	AGE36XX_VAL_AGILENT_E36XX_OUTPUT_OVER_VOLTAGE   2
#define	AGE36XX_VAL_AGILENT_E36XX_OUTPUT_OVER_CURRENT   3
#define	AGE36XX_VAL_AGILENT_E36XX_OUTPUT_UNREGULATED    4
#define	AGE36XX_VAL_AGILENT_E36XX_OUTPUT_STATE_OUTPUT_OFF 5

    /*- Defined values for parameter Direction in function AgE36xx_OutputRunStep */
#define	AGE36XX_VAL_AGILENT_E36XX_VOLTAGE_STEP_DIRECTION_UP 0
#define	AGE36XX_VAL_AGILENT_E36XX_VOLTAGE_STEP_DIRECTION_DOWN 1
#define	AGE36XX_VAL_AGILENT_E36XX_CURRENT_STEP_DIRECTION_UP 2
#define	AGE36XX_VAL_AGILENT_E36XX_CURRENT_STEP_DIRECTION_DOWN 3

    /*- Defined values for parameter StepType in function AgE36xx_OutputGetStep, parameter StepType in function AgE36xx_OutputSetStep */
#define	AGE36XX_VAL_AGILENT_E36XX_VOLTAGE_STEP          0
#define	AGE36XX_VAL_AGILENT_E36XX_DEFAULT_VOLTAGE_STEP  1
#define	AGE36XX_VAL_AGILENT_E36XX_CURRENT_STEP          2
#define	AGE36XX_VAL_AGILENT_E36XX_DEFAULT_CURRENT_STEP  3



/**************************************************************************** 
 *---------------- Instrument Driver Function Declarations -----------------* 
 ****************************************************************************/

    /*- AgilentE36xx */

ViStatus _VI_FUNC  AgE36xx_init ( ViRsrc ResourceName, ViBoolean IdQuery, ViBoolean Reset, ViSession* Vi );

ViStatus _VI_FUNC  AgE36xx_GetNativeIUnknownPtr ( ViSession Vi, IUnknown** NativeIUnknownPtr );

ViStatus _VI_FUNC  AgE36xx_GetChannelName ( ViSession Vi, ViInt32 Index, ViInt32 NameBufferSize, ViChar Name[] );

ViStatus _VI_FUNC  AgE36xx_close ( ViSession Vi );

ViStatus _VI_FUNC  AgE36xx_InitWithOptions ( ViRsrc ResourceName, ViBoolean IdQuery, ViBoolean Reset, ViConstString OptionsString, ViSession* Vi );

    /*- System */

ViStatus _VI_FUNC  AgE36xx_SystemRead ( ViSession Vi, ViInt32 Size, ViChar Value[], ViInt32* ActualSize );

ViStatus _VI_FUNC  AgE36xx_SystemWrite ( ViSession Vi, ViConstString CommandString );

ViStatus _VI_FUNC  AgE36xx_SystemWaitForOperationComplete ( ViSession Vi, ViInt32 MaxTimeMilliseconds );

ViStatus _VI_FUNC  AgE36xx_SystemSaveState ( ViSession Vi, ViConstString Identifier );

ViStatus _VI_FUNC  AgE36xx_SystemRecallState ( ViSession Vi, ViConstString Identifier );

ViStatus _VI_FUNC  AgE36xx_SystemEnableLocalControls ( ViSession Vi );

ViStatus _VI_FUNC  AgE36xx_SystemDisableLocalControls ( ViSession Vi );

ViStatus _VI_FUNC  AgE36xx_SystemBeep ( ViSession Vi );

ViStatus _VI_FUNC  AgE36xx_SystemGetMemoryName ( ViSession Vi, ViInt32 Index, ViInt32 retValBufferSize, ViChar retVal[] );

ViStatus _VI_FUNC  AgE36xx_SystemSetMemoryName ( ViSession Vi, ViInt32 Index, ViConstString val );

    /*- Calibrations */

ViStatus _VI_FUNC  AgE36xx_GetCalibrationName ( ViSession Vi, ViInt32 Index, ViInt32 NameBufferSize, ViChar Name[] );

ViStatus _VI_FUNC  AgE36xx_CalibrationsDACError ( ViSession Vi );

ViStatus _VI_FUNC  AgE36xx_CalibrationsSetState ( ViSession Vi, ViBoolean Enabled, ViConstString Password );

    /*- Outputs */

ViStatus _VI_FUNC  AgE36xx_GetOutputName ( ViSession Vi, ViInt32 Index, ViInt32 NameBufferSize, ViChar Name[] );

    /*- Triggers */

ViStatus _VI_FUNC  AgE36xx_GetTriggerName ( ViSession Vi, ViInt32 Index, ViInt32 NameBufferSize, ViChar Name[] );

    /*- Utility */

ViStatus _VI_FUNC  AgE36xx_revision_query ( ViSession Vi, ViChar DriverRev[], ViChar InstrRev[] );

ViStatus _VI_FUNC  AgE36xx_error_message ( ViSession Vi, ViStatus ErrorCode, ViChar ErrorMessage[] );

ViStatus _VI_FUNC  AgE36xx_AttachToExistingCOMSession ( IUnknown* ExistingIUnknownPtr, ViSession* Vi );

ViStatus _VI_FUNC  AgE36xx_GetError ( ViSession Vi, ViStatus* ErrorCode, ViInt32 ErrorDescriptionBufferSize, ViChar ErrorDescription[] );

ViStatus _VI_FUNC  AgE36xx_ClearError ( ViSession Vi );

ViStatus _VI_FUNC  AgE36xx_GetNextCoercionRecord ( ViSession Vi, ViInt32 CoercionRecordBufferSize, ViChar CoercionRecord[] );

ViStatus _VI_FUNC  AgE36xx_ResetInterchangeCheck ( ViSession Vi );

ViStatus _VI_FUNC  AgE36xx_GetNextInterchangeWarning ( ViSession Vi, ViInt32 InterchangeWarningBufferSize, ViChar InterchangeWarning[] );

ViStatus _VI_FUNC  AgE36xx_ClearInterchangeWarnings ( ViSession Vi );

ViStatus _VI_FUNC  AgE36xx_InvalidateAllAttributes ( ViSession Vi );

ViStatus _VI_FUNC  AgE36xx_UnlockSession ( ViSession Vi, ViBoolean* CallerHasLock );

ViStatus _VI_FUNC  AgE36xx_LockSession ( ViSession Vi, ViBoolean* CallerHasLock );

ViStatus _VI_FUNC  AgE36xx_error_query ( ViSession Vi, ViInt32* ErrorCode, ViChar ErrorMessage[] );

ViStatus _VI_FUNC  AgE36xx_self_test ( ViSession Vi, ViInt16* TestResult, ViChar TestMessage[] );

ViStatus _VI_FUNC  AgE36xx_Disable ( ViSession Vi );

ViStatus _VI_FUNC  AgE36xx_ResetWithDefaults ( ViSession Vi );

ViStatus _VI_FUNC  AgE36xx_reset ( ViSession Vi );

    /*- Attribute Accessors */

ViStatus _VI_FUNC  AgE36xx_GetAttributeViInt32 ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViInt32* AttributeValue );

ViStatus _VI_FUNC  AgE36xx_GetAttributeViReal64 ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViReal64* AttributeValue );

ViStatus _VI_FUNC  AgE36xx_GetAttributeViBoolean ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViBoolean* AttributeValue );

ViStatus _VI_FUNC  AgE36xx_GetAttributeViSession ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViSession* AttributeValue );

ViStatus _VI_FUNC  AgE36xx_GetAttributeViString ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViInt32 AttributeValueBufferSize, ViChar AttributeValue[] );

ViStatus _VI_FUNC  AgE36xx_SetAttributeViInt32 ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViInt32 AttributeValue );

ViStatus _VI_FUNC  AgE36xx_SetAttributeViReal64 ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViReal64 AttributeValue );

ViStatus _VI_FUNC  AgE36xx_SetAttributeViBoolean ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViBoolean AttributeValue );

ViStatus _VI_FUNC  AgE36xx_SetAttributeViSession ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViSession AttributeValue );

ViStatus _VI_FUNC  AgE36xx_SetAttributeViString ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViConstString AttributeValue );

    /*- Configuration */

ViStatus _VI_FUNC  AgE36xx_ConfigureOutputEnabled ( ViSession Vi, ViConstString ChannelName, ViBoolean Enabled );

ViStatus _VI_FUNC  AgE36xx_ConfigureVoltageLevel ( ViSession Vi, ViConstString ChannelName, ViReal64 Level );

ViStatus _VI_FUNC  AgE36xx_QueryMaxVoltageLevel ( ViSession Vi, ViConstString ChannelName, ViReal64 CurrentLimit, ViReal64* MaxVoltageLevel );

ViStatus _VI_FUNC  AgE36xx_QueryMaxCurrentLimit ( ViSession Vi, ViConstString ChannelName, ViReal64 VoltageLevel, ViReal64* MaxCurrentLimit );

ViStatus _VI_FUNC  AgE36xx_ConfigureOVP ( ViSession Vi, ViConstString ChannelName, ViBoolean Enabled, ViReal64 Limit );

ViStatus _VI_FUNC  AgE36xx_ConfigureOutputRange ( ViSession Vi, ViConstString ChannelName, ViInt32 RangeType, ViReal64 Range );

ViStatus _VI_FUNC  AgE36xx_ConfigureCurrentLimit ( ViSession Vi, ViConstString ChannelName, ViInt32 Behavior, ViReal64 Limit );

    /*- Triggering */

ViStatus _VI_FUNC  AgE36xx_ConfigureTriggeredCurrentLimit ( ViSession Vi, ViConstString ChannelName, ViReal64 Limit );

ViStatus _VI_FUNC  AgE36xx_ConfigureTriggeredVoltageLevel ( ViSession Vi, ViConstString ChannelName, ViReal64 Level );

ViStatus _VI_FUNC  AgE36xx_ConfigureTriggerSource ( ViSession Vi, ViConstString ChannelName, ViInt32 Source );

    /*- Action */

ViStatus _VI_FUNC  AgE36xx_SendSoftwareTrigger ( ViSession Vi );

ViStatus _VI_FUNC  AgE36xx_Initiate ( ViSession Vi );

ViStatus _VI_FUNC  AgE36xx_Abort ( ViSession Vi );

ViStatus _VI_FUNC  AgE36xx_ResetOutputProtection ( ViSession Vi, ViConstString ChannelName );

ViStatus _VI_FUNC  AgE36xx_QueryOutputState ( ViSession Vi, ViConstString ChannelName, ViInt32 OutputState, ViBoolean* InState );

ViStatus _VI_FUNC  AgE36xx_Measure ( ViSession Vi, ViConstString ChannelName, ViInt32 MeasurementType, ViReal64* Measurement );

    /*- Display */

ViStatus _VI_FUNC  AgE36xx_DisplayClear ( ViSession Vi );

    /*- Status */

ViStatus _VI_FUNC  AgE36xx_StatusQueryRegister ( ViSession Vi, ViInt32 Register, ViInt32 SubRegister, ViInt32* retVal );

ViStatus _VI_FUNC  AgE36xx_StatusConfigureRegister ( ViSession Vi, ViInt32 Register, ViInt32 SubRegister, ViInt32 Data );

ViStatus _VI_FUNC  AgE36xx_StatusClear ( ViSession Vi );

ViStatus _VI_FUNC  AgE36xx_StatusGetRegister ( ViSession Vi, ViInt32 Register, ViInt32 SubRegister, ViInt32* retVal );

ViStatus _VI_FUNC  AgE36xx_StatusSetRegister ( ViSession Vi, ViInt32 Register, ViInt32 SubRegister, ViInt32 val );

    /*- Output */

ViStatus _VI_FUNC  AgE36xx_OutputApplyVoltageCurrent ( ViSession Vi, ViConstString repCapIdentifier, ViReal64 VoltageLevel, ViReal64 CurrentLimit );

ViStatus _VI_FUNC  AgE36xx_OutputQueryState ( ViSession Vi, ViConstString repCapIdentifier, ViInt32 OutputState, ViBoolean* retVal );

ViStatus _VI_FUNC  AgE36xx_OutputRunStep ( ViSession Vi, ViConstString repCapIdentifier, ViInt32 Direction );

ViStatus _VI_FUNC  AgE36xx_OutputGetStep ( ViSession Vi, ViConstString repCapIdentifier, ViInt32 StepType, ViReal64* retVal );

ViStatus _VI_FUNC  AgE36xx_OutputSetStep ( ViSession Vi, ViConstString repCapIdentifier, ViInt32 StepType, ViReal64 val );

    /*- Protection */

ViStatus _VI_FUNC  AgE36xx_OutputProtectionResetOVP ( ViSession Vi, ViConstString repCapIdentifier );

ViStatus _VI_FUNC  AgE36xx_OutputProtectionResetOutputProtection ( ViSession Vi, ViConstString repCapIdentifier );

ViStatus _VI_FUNC  AgE36xx_OutputProtectionResetOCP ( ViSession Vi, ViConstString repCapIdentifier );

ViStatus _VI_FUNC  AgE36xx_OutputProtectionQueryOVPLimitMax ( ViSession Vi, ViConstString repCapIdentifier, ViReal64* retVal );

ViStatus _VI_FUNC  AgE36xx_OutputProtectionQueryOCPLimitMax ( ViSession Vi, ViConstString repCapIdentifier, ViReal64* retVal );

ViStatus _VI_FUNC  AgE36xx_OutputProtectionConfigureOVP ( ViSession Vi, ViConstString repCapIdentifier, ViBoolean Enabled, ViReal64 Limit );

ViStatus _VI_FUNC  AgE36xx_OutputProtectionConfigureOCP ( ViSession Vi, ViConstString repCapIdentifier, ViBoolean Enabled, ViReal64 Limit );

    /*- Calibration */

ViStatus _VI_FUNC  AgE36xx_CalibrationCurrentData ( ViSession Vi, ViConstString repCapIdentifier, ViReal64 Current );

ViStatus _VI_FUNC  AgE36xx_CalibrationCurrentLevel ( ViSession Vi, ViConstString repCapIdentifier, ViInt32 Level );

ViStatus _VI_FUNC  AgE36xx_CalibrationCurrentProtection ( ViSession Vi, ViConstString repCapIdentifier );

ViStatus _VI_FUNC  AgE36xx_CalibrationVoltageData ( ViSession Vi, ViConstString repCapIdentifier, ViReal64 Voltage );

ViStatus _VI_FUNC  AgE36xx_CalibrationVoltageLevel ( ViSession Vi, ViConstString repCapIdentifier, ViInt32 Level );

ViStatus _VI_FUNC  AgE36xx_CalibrationVoltageProtection ( ViSession Vi, ViConstString repCapIdentifier );


/**************************************************************************** 
 *----------------- Instrument Error And Completion Codes ------------------* 
 ****************************************************************************/
#ifndef _IVIC_ERROR_BASE_DEFINES_
#define _IVIC_ERROR_BASE_DEFINES_

#define IVIC_WARN_BASE                           (0x3FFA0000L)
#define IVIC_CROSS_CLASS_WARN_BASE               (IVIC_WARN_BASE + 0x1000)
#define IVIC_CLASS_WARN_BASE                     (IVIC_WARN_BASE + 0x2000)
#define IVIC_SPECIFIC_WARN_BASE                  (IVIC_WARN_BASE + 0x4000)

#define IVIC_ERROR_BASE                          (0xBFFA0000L)
#define IVIC_CROSS_CLASS_ERROR_BASE              (IVIC_ERROR_BASE + 0x1000)
#define IVIC_CLASS_ERROR_BASE                    (IVIC_ERROR_BASE + 0x2000)
#define IVIC_SPECIFIC_ERROR_BASE                 (IVIC_ERROR_BASE + 0x4000)
#define IVIC_LXISYNC_ERROR_BASE                  (IVIC_ERROR_BASE + 0x3000)

#define IVIC_ERROR_INVALID_ATTRIBUTE             (IVIC_ERROR_BASE + 0x0C)
#define IVIC_ERROR_TYPES_DO_NOT_MATCH            (IVIC_ERROR_BASE + 0x15)
#define IVIC_ERROR_IVI_ATTR_NOT_WRITABLE         (IVIC_ERROR_BASE + 0x0D)
#define IVIC_ERROR_IVI_ATTR_NOT_READABLE         (IVIC_ERROR_BASE + 0x0E)

#endif



#define AGE36XX_SUCCESS                                   0
#define AGE36XX_WARN_NSUP_ID_QUERY                        (IVIC_WARN_BASE + 0x65)
#define AGE36XX_WARN_NSUP_RESET                           (IVIC_WARN_BASE + 0x66)
#define AGE36XX_WARN_NSUP_SELF_TEST                       (IVIC_WARN_BASE + 0x67)
#define AGE36XX_WARN_NSUP_ERROR_QUERY                     (IVIC_WARN_BASE + 0x68)
#define AGE36XX_WARN_NSUP_REV_QUERY                       (IVIC_WARN_BASE + 0x69)
#define AGE36XX_ERROR_CANNOT_RECOVER                      (IVIC_ERROR_BASE + 0x00)
#define AGE36XX_ERROR_INSTRUMENT_STATUS                   (IVIC_ERROR_BASE + 0x01)
#define AGE36XX_ERROR_CANNOT_OPEN_FILE                    (IVIC_ERROR_BASE + 0x02)
#define AGE36XX_ERROR_READING_FILE                        (IVIC_ERROR_BASE + 0x03)
#define AGE36XX_ERROR_WRITING_FILE                        (IVIC_ERROR_BASE + 0x04)
#define AGE36XX_ERROR_INVALID_PATHNAME                    (IVIC_ERROR_BASE + 0x0B)
#define AGE36XX_ERROR_INVALID_VALUE                       (IVIC_ERROR_BASE + 0x10)
#define AGE36XX_ERROR_FUNCTION_NOT_SUPPORTED              (IVIC_ERROR_BASE + 0x11)
#define AGE36XX_ERROR_ATTRIBUTE_NOT_SUPPORTED             (IVIC_ERROR_BASE + 0x12)
#define AGE36XX_ERROR_VALUE_NOT_SUPPORTED                 (IVIC_ERROR_BASE + 0x13)
#define AGE36XX_ERROR_NOT_INITIALIZED                     (IVIC_ERROR_BASE + 0x1D)
#define AGE36XX_ERROR_UNKNOWN_CHANNEL_NAME                (IVIC_ERROR_BASE + 0x20)
#define AGE36XX_ERROR_TOO_MANY_OPEN_FILES                 (IVIC_ERROR_BASE + 0x23)
#define AGE36XX_ERROR_CHANNEL_NAME_REQUIRED               (IVIC_ERROR_BASE + 0x44)
#define AGE36XX_ERROR_MISSING_OPTION_NAME                 (IVIC_ERROR_BASE + 0x49)
#define AGE36XX_ERROR_MISSING_OPTION_VALUE                (IVIC_ERROR_BASE + 0x4A)
#define AGE36XX_ERROR_BAD_OPTION_NAME                     (IVIC_ERROR_BASE + 0x4B)
#define AGE36XX_ERROR_BAD_OPTION_VALUE                    (IVIC_ERROR_BASE + 0x4C)
#define AGE36XX_ERROR_OUT_OF_MEMORY                       (IVIC_ERROR_BASE + 0x56)
#define AGE36XX_ERROR_OPERATION_PENDING                   (IVIC_ERROR_BASE + 0x57)
#define AGE36XX_ERROR_NULL_POINTER                        (IVIC_ERROR_BASE + 0x58)
#define AGE36XX_ERROR_UNEXPECTED_RESPONSE                 (IVIC_ERROR_BASE + 0x59)
#define AGE36XX_ERROR_FILE_NOT_FOUND                      (IVIC_ERROR_BASE + 0x5B)
#define AGE36XX_ERROR_INVALID_FILE_FORMAT                 (IVIC_ERROR_BASE + 0x5C)
#define AGE36XX_ERROR_STATUS_NOT_AVAILABLE                (IVIC_ERROR_BASE + 0x5D)
#define AGE36XX_ERROR_ID_QUERY_FAILED                     (IVIC_ERROR_BASE + 0x5E)
#define AGE36XX_ERROR_RESET_FAILED                        (IVIC_ERROR_BASE + 0x5F)
#define AGE36XX_ERROR_RESOURCE_UNKNOWN                    (IVIC_ERROR_BASE + 0x60)
#define AGE36XX_ERROR_ALREADY_INITIALIZED                 (IVIC_ERROR_BASE + 0x61)
#define AGE36XX_ERROR_CANNOT_CHANGE_SIMULATION_STATE      (IVIC_ERROR_BASE + 0x62)
#define AGE36XX_ERROR_INVALID_NUMBER_OF_LEVELS_IN_SELECTOR (IVIC_ERROR_BASE + 0x63)
#define AGE36XX_ERROR_INVALID_RANGE_IN_SELECTOR           (IVIC_ERROR_BASE + 0x64)
#define AGE36XX_ERROR_UNKOWN_NAME_IN_SELECTOR             (IVIC_ERROR_BASE + 0x65)
#define AGE36XX_ERROR_BADLY_FORMED_SELECTOR               (IVIC_ERROR_BASE + 0x66)
#define AGE36XX_ERROR_UNKNOWN_PHYSICAL_IDENTIFIER         (IVIC_ERROR_BASE + 0x67)
#define AGE36XX_IVIDCPWR_ERROR_TRIGGER_NOT_SOFTWARE       (IVIC_CROSS_CLASS_ERROR_BASE + 1)


#define AGE36XX_ERROR_PERSONALITY_NOT_ACTIVE              (IVIC_SPECIFIC_ERROR_BASE + 529L)
#define AGE36XX_ERROR_PERSONALITY_NOT_INSTALLED           (IVIC_SPECIFIC_ERROR_BASE + 530L)
#define AGE36XX_ERROR_PERSONALITY_NOT_LICENSED            (IVIC_SPECIFIC_ERROR_BASE + 531L)
#define AGE36XX_ERROR_IO_GENERAL                          (IVIC_SPECIFIC_ERROR_BASE + 532L)
#define AGE36XX_ERROR_IO_TIMEOUT                          (IVIC_SPECIFIC_ERROR_BASE + 533L)
#define AGE36XX_ERROR_MODEL_NOT_SUPPORTED                 (IVIC_SPECIFIC_ERROR_BASE + 534L)
#define AGE36XX_ERROR_WRAPPED_DRIVER_ERROR                (IVIC_SPECIFIC_ERROR_BASE + 536L)
#define AGE36XX_ERROR_MAX_TIME_EXCEEDED                   (IVIC_SPECIFIC_ERROR_BASE + 535L)




/**************************************************************************** 
 *---------------------------- End Include File ----------------------------* 
 ****************************************************************************/
#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif
#endif // __AGE36XX_HEADER
