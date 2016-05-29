/******************************************************************************
 *                                                                         
 *               Agilent Technologies
 *               All rights reserved.
 *
 *****************************************************************************/

#ifndef __AGRFSIGGEN_HEADER
#define __AGRFSIGGEN_HEADER

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
#define	AGRFSIGGEN_ATTR_SPECIFIC_DRIVER_DESCRIPTION                  (IVI_INHERENT_ATTR_BASE + 514L)  /* ViString, read-only */
#define	AGRFSIGGEN_ATTR_SPECIFIC_DRIVER_PREFIX                       (IVI_INHERENT_ATTR_BASE + 302L)  /* ViString, read-only */
#define	AGRFSIGGEN_ATTR_SPECIFIC_DRIVER_VENDOR                       (IVI_INHERENT_ATTR_BASE + 513L)  /* ViString, read-only */
#define	AGRFSIGGEN_ATTR_SPECIFIC_DRIVER_REVISION                     (IVI_INHERENT_ATTR_BASE + 551L)  /* ViString, read-only */
#define	AGRFSIGGEN_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION     (IVI_INHERENT_ATTR_BASE + 515L)  /* ViInt32, read-only */
#define	AGRFSIGGEN_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION     (IVI_INHERENT_ATTR_BASE + 516L)  /* ViInt32, read-only */

    /*- Advanced Session Information */
#define	AGRFSIGGEN_ATTR_LOGICAL_NAME                    (IVI_INHERENT_ATTR_BASE + 305L)  /* ViString, read-only */
#define	AGRFSIGGEN_ATTR_IO_RESOURCE_DESCRIPTOR          (IVI_INHERENT_ATTR_BASE + 304L)  /* ViString, read-only */
#define	AGRFSIGGEN_ATTR_DRIVER_SETUP                    (IVI_INHERENT_ATTR_BASE + 7L)    /* ViString, read-only */

    /*- User Options */
#define	AGRFSIGGEN_ATTR_CACHE                           (IVI_INHERENT_ATTR_BASE + 4L)   /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_INTERCHANGE_CHECK               (IVI_INHERENT_ATTR_BASE + 21L)  /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_QUERY_INSTRUMENT_STATUS         (IVI_INHERENT_ATTR_BASE + 3L)   /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_RANGE_CHECK                     (IVI_INHERENT_ATTR_BASE + 2L)   /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_RECORD_COERCIONS                (IVI_INHERENT_ATTR_BASE + 6L)   /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_SIMULATE                        (IVI_INHERENT_ATTR_BASE + 5L)   /* ViBoolean, read-write */

    /*- Instrument Identification */
#define	AGRFSIGGEN_ATTR_INSTRUMENT_MANUFACTURER          (IVI_INHERENT_ATTR_BASE + 511L)  /* ViString, read-only */
#define	AGRFSIGGEN_ATTR_INSTRUMENT_MODEL                 (IVI_INHERENT_ATTR_BASE + 512L)  /* ViString, read-only */
#define	AGRFSIGGEN_ATTR_INSTRUMENT_FIRMWARE_REVISION     (IVI_INHERENT_ATTR_BASE + 510L)  /* ViString, read-only */

    /*- Driver Capabilities */
#define	AGRFSIGGEN_ATTR_SUPPORTED_INSTRUMENT_MODELS     (IVI_INHERENT_ATTR_BASE + 327L)  /* ViString, read-only */
#define	AGRFSIGGEN_ATTR_GROUP_CAPABILITIES              (IVI_INHERENT_ATTR_BASE + 401L)  /* ViString, read-only */


/*===== Instrument-Specific Attributes =====================================*/

    /*- Top-level Attributes */
#define	AGRFSIGGEN_ATTR_FRIENDLY_NAME                   (IVI_SPECIFIC_ATTR_BASE + 119L)  /* ViString, read-only */

    /*- ArmAlarm */
#define	AGRFSIGGEN_ATTR_ARMALARM_COUNT                  (IVI_SPECIFIC_ATTR_BASE + 710L)  /* ViInt32, read-only */

    /*- ArmSource */
#define	AGRFSIGGEN_ATTR_ARMSOURCE_COUNT                 (IVI_SPECIFIC_ATTR_BASE + 159L)  /* ViInt32, read-only */

    /*- Event */
#define	AGRFSIGGEN_ATTR_EVENT_COUNT                     (IVI_SPECIFIC_ATTR_BASE + 160L)  /* ViInt32, read-only */

    /*- TriggerAlarm */
#define	AGRFSIGGEN_ATTR_TRIGGERALARM_COUNT              (IVI_SPECIFIC_ATTR_BASE + 161L)  /* ViInt32, read-only */

    /*- TriggerSource */
#define	AGRFSIGGEN_ATTR_TRIGGERSOURCE_COUNT             (IVI_SPECIFIC_ATTR_BASE + 162L)  /* ViInt32, read-only */

    /*- RF */
#define	AGRFSIGGEN_ATTR_FREQUENCY                       (IVI_CLASS_ATTR_BASE + 1L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_POWER_LEVEL                     (IVI_CLASS_ATTR_BASE + 2L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_OUTPUT_ENABLED                  (IVI_CLASS_ATTR_BASE + 4L)  /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_ALC_ENABLED                     (IVI_CLASS_ATTR_BASE + 3L)  /* ViBoolean, read-write */

    /*- ALC */
#define	AGRFSIGGEN_ATTR_ALC_SOURCE                      (IVI_CLASS_ATTR_BASE + 301L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_ALC_BANDWIDTH                   (IVI_CLASS_ATTR_BASE + 302L)  /* ViReal64, read-write */

    /*- LF Generator */
#define	AGRFSIGGEN_ATTR_LF_GENERATOR_FREQUENCY          (IVI_CLASS_ATTR_BASE + 102L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_LF_GENERATOR_WAVEFORM           (IVI_CLASS_ATTR_BASE + 103L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_ACTIVE_LF_GENERATOR             (IVI_CLASS_ATTR_BASE + 101L)  /* ViString, read-write */
#define	AGRFSIGGEN_ATTR_LF_GENERATOR_COUNT              (IVI_CLASS_ATTR_BASE + 104L)  /* ViInt32, read-only */

    /*- LF Generator Outputs */
#define	AGRFSIGGEN_ATTR_LF_GENERATOR_OUTPUT_ENABLED       (IVI_CLASS_ATTR_BASE + 112L)  /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_LF_GENERATOR_OUTPUT_AMPLITUDE     (IVI_CLASS_ATTR_BASE + 111L)  /* ViReal64, read-write */

    /*- Pulse Generator */
#define	AGRFSIGGEN_ATTR_PULSE_INTERNAL_TRIGGER_PERIOD     (IVI_CLASS_ATTR_BASE + 121L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_PULSE_WIDTH                       (IVI_CLASS_ATTR_BASE + 122L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_PULSE_GATING_ENABLED              (IVI_CLASS_ATTR_BASE + 123L)  /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_PULSE_TRIGGER_SOURCE              (IVI_CLASS_ATTR_BASE + 124L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_PULSE_EXTERNAL_TRIGGER_SLOPE      (IVI_CLASS_ATTR_BASE + 125L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_PULSE_EXTERNAL_TRIGGER_DELAY      (IVI_CLASS_ATTR_BASE + 126L)  /* ViReal64, read-write */

    /*- Double Pulse Generators */
#define	AGRFSIGGEN_ATTR_PULSE_DOUBLE_ENABLED            (IVI_CLASS_ATTR_BASE + 131L)  /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_PULSE_DOUBLE_DELAY              (IVI_CLASS_ATTR_BASE + 132L)  /* ViReal64, read-write */

    /*- Pulse Generator Output */
#define	AGRFSIGGEN_ATTR_PULSE_OUTPUT_POLARITY           (IVI_CLASS_ATTR_BASE + 141L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_PULSE_OUTPUT_ENABLED            (IVI_CLASS_ATTR_BASE + 142L)  /* ViBoolean, read-write */

    /*- Sweep */
#define	AGRFSIGGEN_ATTR_SWEEP_MODE                      (IVI_CLASS_ATTR_BASE + 201L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_SWEEP_TRIGGER_SOURCE            (IVI_CLASS_ATTR_BASE + 202L)  /* ViInt32, read-write */

    /*- Frequency Sweep */
#define	AGRFSIGGEN_ATTR_FREQUENCY_SWEEP_START           (IVI_CLASS_ATTR_BASE + 211L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_FREQUENCY_SWEEP_STOP            (IVI_CLASS_ATTR_BASE + 212L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_FREQUENCY_SWEEP_TIME            (IVI_CLASS_ATTR_BASE + 213L)  /* ViReal64, read-write */

    /*- Power Sweep */
#define	AGRFSIGGEN_ATTR_POWER_SWEEP_START               (IVI_CLASS_ATTR_BASE + 221L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_POWER_SWEEP_STOP                (IVI_CLASS_ATTR_BASE + 222L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_POWER_SWEEP_TIME                (IVI_CLASS_ATTR_BASE + 223L)  /* ViReal64, read-write */

    /*- Frequency Step */
#define	AGRFSIGGEN_ATTR_FREQUENCY_STEP_START                   (IVI_CLASS_ATTR_BASE + 241L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_FREQUENCY_STEP_STOP                    (IVI_CLASS_ATTR_BASE + 242L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_FREQUENCY_STEP_SCALING                 (IVI_CLASS_ATTR_BASE + 243L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_FREQUENCY_STEP_SIZE                    (IVI_CLASS_ATTR_BASE + 244L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_FREQUENCY_STEP_SINGLE_STEP_ENABLED     (IVI_CLASS_ATTR_BASE + 245L)  /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_FREQUENCY_STEP_DWELL                   (IVI_CLASS_ATTR_BASE + 246L)  /* ViReal64, read-write */

    /*- Power Step */
#define	AGRFSIGGEN_ATTR_POWER_STEP_START                   (IVI_CLASS_ATTR_BASE + 261L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_POWER_STEP_STOP                    (IVI_CLASS_ATTR_BASE + 262L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_POWER_STEP_SIZE                    (IVI_CLASS_ATTR_BASE + 263L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_POWER_STEP_SINGLE_STEP_ENABLED     (IVI_CLASS_ATTR_BASE + 264L)  /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_POWER_STEP_DWELL                   (IVI_CLASS_ATTR_BASE + 265L)  /* ViReal64, read-write */

    /*- List */
#define	AGRFSIGGEN_ATTR_LIST_SINGLE_STEP_ENABLED          (IVI_CLASS_ATTR_BASE + 282L)     /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_LIST_DWELL                        (IVI_CLASS_ATTR_BASE + 283L)     /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_LIST_SELECTED_NAME                (IVI_CLASS_ATTR_BASE + 281L)     /* ViString, read-write */
#define	AGRFSIGGEN_ATTR_SWEEP_LIST_SINGLE_STEP_SOURCE     (IVI_SPECIFIC_ATTR_BASE + 104L)  /* ViInt32, read-write */

    /*- FrequencyStep */
#define	AGRFSIGGEN_ATTR_SWEEP_FREQUENCYSTEP_POINTS                 (IVI_SPECIFIC_ATTR_BASE + 91L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_SWEEP_FREQUENCYSTEP_SINGLE_STEP_SOURCE     (IVI_SPECIFIC_ATTR_BASE + 94L)  /* ViInt32, read-write */

    /*- PowerStep */
#define	AGRFSIGGEN_ATTR_SWEEP_POWERSTEP_POINTS                 (IVI_SPECIFIC_ATTR_BASE + 106L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_SWEEP_POWERSTEP_SINGLE_STEP_SOURCE     (IVI_SPECIFIC_ATTR_BASE + 108L)  /* ViInt32, read-write */

    /*- Reference Oscillator */
#define	AGRFSIGGEN_ATTR_REFERENCE_OSCILLATOR_SOURCE                 (IVI_CLASS_ATTR_BASE + 321L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_REFERENCE_OSCILLATOR_EXTERNAL_FREQUENCY     (IVI_CLASS_ATTR_BASE + 322L)  /* ViReal64, read-write */

    /*- AM */
#define	AGRFSIGGEN_ATTR_AM_SOURCE                       (IVI_CLASS_ATTR_BASE + 12L)  /* ViString, read-write */
#define	AGRFSIGGEN_ATTR_AM_SCALING                      (IVI_CLASS_ATTR_BASE + 13L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_AM_DEPTH                        (IVI_CLASS_ATTR_BASE + 16L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_AM_NOMINAL_VOLTAGE              (IVI_CLASS_ATTR_BASE + 15L)  /* ViReal64, read-only */
#define	AGRFSIGGEN_ATTR_AM_ENABLED                      (IVI_CLASS_ATTR_BASE + 11L)  /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_AM_EXTERNAL_COUPLING            (IVI_CLASS_ATTR_BASE + 14L)  /* ViInt32, read-write */

    /*- FM */
#define	AGRFSIGGEN_ATTR_FM_SOURCE                       (IVI_CLASS_ATTR_BASE + 22L)  /* ViString, read-write */
#define	AGRFSIGGEN_ATTR_FM_DEVIATION                    (IVI_CLASS_ATTR_BASE + 25L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_FM_NOMINAL_VOLTAGE              (IVI_CLASS_ATTR_BASE + 24L)  /* ViReal64, read-only */
#define	AGRFSIGGEN_ATTR_FM_ENABLED                      (IVI_CLASS_ATTR_BASE + 21L)  /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_FM_EXTERNAL_COUPLING            (IVI_CLASS_ATTR_BASE + 23L)  /* ViInt32, read-write */

    /*- PM */
#define	AGRFSIGGEN_ATTR_PM_SOURCE                       (IVI_CLASS_ATTR_BASE + 32L)  /* ViString, read-write */
#define	AGRFSIGGEN_ATTR_PM_DEVIATION                    (IVI_CLASS_ATTR_BASE + 35L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_PM_NOMINAL_VOLTAGE              (IVI_CLASS_ATTR_BASE + 34L)  /* ViReal64, read-only */
#define	AGRFSIGGEN_ATTR_PM_ENABLED                      (IVI_CLASS_ATTR_BASE + 31L)  /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_PM_EXTERNAL_COUPLING            (IVI_CLASS_ATTR_BASE + 33L)  /* ViInt32, read-write */

    /*- Source */
#define	AGRFSIGGEN_ATTR_ANALOG_MODULATION_SOURCE_COUNT     (IVI_CLASS_ATTR_BASE + 41L)  /* ViInt32, read-only */

    /*- Pulse Modulation */
#define	AGRFSIGGEN_ATTR_PULSE_MODULATION_ENABLED               (IVI_CLASS_ATTR_BASE + 51L)  /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_PULSE_MODULATION_SOURCE                (IVI_CLASS_ATTR_BASE + 52L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_PULSE_MODULATION_EXTERNAL_POLARITY     (IVI_CLASS_ATTR_BASE + 53L)  /* ViInt32, read-write */

    /*- ARB Generator */
#define	AGRFSIGGEN_ATTR_ARB_CLOCK_FREQUENCY             (IVI_CLASS_ATTR_BASE + 452L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_ARB_FILTER_FREQUENCY            (IVI_CLASS_ATTR_BASE + 453L)  /* ViReal64, read-write */

    /*- Waveform */
#define	AGRFSIGGEN_ATTR_ARB_SELECTED_WAVEFORM           (IVI_CLASS_ATTR_BASE + 451L)  /* ViString, read-write */
#define	AGRFSIGGEN_ATTR_ARB_MAX_NUMBER_WAVEFORMS        (IVI_CLASS_ATTR_BASE + 454L)  /* ViInt32, read-only */
#define	AGRFSIGGEN_ATTR_ARB_WAVEFORM_QUANTUM            (IVI_CLASS_ATTR_BASE + 455L)  /* ViInt32, read-only */
#define	AGRFSIGGEN_ATTR_ARB_WAVEFORM_SIZE_MIN           (IVI_CLASS_ATTR_BASE + 456L)  /* ViInt32, read-only */
#define	AGRFSIGGEN_ATTR_ARB_WAVEFORM_SIZE_MAX           (IVI_CLASS_ATTR_BASE + 457L)  /* ViInt32, read-only */

    /*- Trigger */
#define	AGRFSIGGEN_ATTR_ARB_TRIGGER_SOURCE              (IVI_CLASS_ATTR_BASE + 458L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_ARB_EXTERNAL_TRIGGER_SLOPE      (IVI_CLASS_ATTR_BASE + 459L)  /* ViInt32, read-write */

    /*- Digital Modulation */
#define	AGRFSIGGEN_ATTR_DIGITAL_MODULATION_BASE_STANDARD_COUNT            (IVI_CLASS_ATTR_BASE + 501L)  /* ViInt32, read-only */
#define	AGRFSIGGEN_ATTR_DIGITAL_MODULATION_BASE_SELECTED_STANDARD         (IVI_CLASS_ATTR_BASE + 502L)  /* ViString, read-write */
#define	AGRFSIGGEN_ATTR_DIGITAL_MODULATION_BASE_DATA_SOURCE               (IVI_CLASS_ATTR_BASE + 503L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_DIGITAL_MODULATION_BASE_PRBS_TYPE                 (IVI_CLASS_ATTR_BASE + 504L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_DIGITAL_MODULATION_BASE_SELECTED_BIT_SEQUENCE     (IVI_CLASS_ATTR_BASE + 505L)  /* ViString, read-write */
#define	AGRFSIGGEN_ATTR_DIGITAL_MODULATION_BASE_CLOCK_SOURCE              (IVI_CLASS_ATTR_BASE + 506L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_DIGITAL_MODULATION_BASE_EXTERNAL_CLOCK_TYPE       (IVI_CLASS_ATTR_BASE + 507L)  /* ViInt32, read-write */

    /*- TDMA */
#define	AGRFSIGGEN_ATTR_TDMA_STANDARD_COUNT             (IVI_CLASS_ATTR_BASE + 701L)  /* ViInt32, read-only */
#define	AGRFSIGGEN_ATTR_TDMA_SELECTED_STANDARD          (IVI_CLASS_ATTR_BASE + 702L)  /* ViString, read-write */
#define	AGRFSIGGEN_ATTR_TDMA_FRAME_COUNT                (IVI_CLASS_ATTR_BASE + 705L)  /* ViInt32, read-only */
#define	AGRFSIGGEN_ATTR_TDMA_SELECTED_FRAME             (IVI_CLASS_ATTR_BASE + 706L)  /* ViString, read-write */
#define	AGRFSIGGEN_ATTR_TDMA_CLOCK_SOURCE               (IVI_CLASS_ATTR_BASE + 707L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_TDMA_EXTERNAL_CLOCK_TYPE        (IVI_CLASS_ATTR_BASE + 708L)  /* ViInt32, read-write */

    /*- Trigger */
#define	AGRFSIGGEN_ATTR_TDMA_TRIGGER_SOURCE             (IVI_CLASS_ATTR_BASE + 703L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_TDMA_EXTERNAL_TRIGGER_SLOPE     (IVI_CLASS_ATTR_BASE + 704L)  /* ViInt32, read-write */

    /*- CDMA */
#define	AGRFSIGGEN_ATTR_CDMA_STANDARD_COUNT             (IVI_CLASS_ATTR_BASE + 601L)  /* ViInt32, read-only */
#define	AGRFSIGGEN_ATTR_CDMA_SELECTED_STANDARD          (IVI_CLASS_ATTR_BASE + 602L)  /* ViString, read-write */
#define	AGRFSIGGEN_ATTR_CDMA_TEST_MODEL_COUNT           (IVI_CLASS_ATTR_BASE + 605L)  /* ViInt32, read-only */
#define	AGRFSIGGEN_ATTR_CDMA_SELECTED_TEST_MODEL        (IVI_CLASS_ATTR_BASE + 606L)  /* ViString, read-write */
#define	AGRFSIGGEN_ATTR_CDMA_CLOCK_SOURCE               (IVI_CLASS_ATTR_BASE + 607L)  /* ViInt32, read-write */

    /*- Trigger */
#define	AGRFSIGGEN_ATTR_CDMA_TRIGGER_SOURCE             (IVI_CLASS_ATTR_BASE + 603L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_CDMA_EXTERNAL_TRIGGER_SLOPE     (IVI_CLASS_ATTR_BASE + 604L)  /* ViInt32, read-write */

    /*- IQ */
#define	AGRFSIGGEN_ATTR_IQ_SOURCE                       (IVI_CLASS_ATTR_BASE + 403L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_IQ_SWAP_ENABLED                 (IVI_CLASS_ATTR_BASE + 404L)  /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_IQ_NOMINAL_VOLTAGE              (IVI_CLASS_ATTR_BASE + 402L)  /* ViReal64, read-only */
#define	AGRFSIGGEN_ATTR_IQ_ENABLED                      (IVI_CLASS_ATTR_BASE + 401L)  /* ViBoolean, read-write */

    /*- IQ Impairment */
#define	AGRFSIGGEN_ATTR_IQ_IMPAIRMENT_ENABLED           (IVI_CLASS_ATTR_BASE + 421L)  /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_IQ_I_OFFSET                     (IVI_CLASS_ATTR_BASE + 422L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_IQ_Q_OFFSET                     (IVI_CLASS_ATTR_BASE + 423L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_IQ_RATIO                        (IVI_CLASS_ATTR_BASE + 424L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_IQ_SKEW                         (IVI_CLASS_ATTR_BASE + 425L)  /* ViReal64, read-write */

    /*- Arm */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_ARM_COUNT            (IVI_LXISYNC_ATTR_BASE + 7L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_ARM_DELAY            (IVI_LXISYNC_ATTR_BASE + 8L)  /* ViReal64, read-write */

    /*- Source */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_ARM_SOURCE_COUNT          (IVI_INHERENT_ATTR_BASE + 9L)  /* ViInt32, read-only */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_ARM_SOURCE_OR_ENABLED     (IVI_LXISYNC_ATTR_BASE + 14L)  /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_ARM_SOURCE_ENABLED        (IVI_LXISYNC_ATTR_BASE + 11L)  /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_ARM_SOURCE_DETECTION      (IVI_LXISYNC_ATTR_BASE + 10L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_ARM_SOURCE_FILTER         (IVI_LXISYNC_ATTR_BASE + 13L)  /* ViString, read-write */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_ARM_SOURCE_EVENTID        (IVI_LXISYNC_ATTR_BASE + 12L)  /* ViString, read-write */

    /*- Alarm */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_ARM_ALARM_COUNT             (IVI_INHERENT_ATTR_BASE + 1L)  /* ViInt32, read-only */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_ARM_ALARM_TIME_SECONDS      (IVI_LXISYNC_ATTR_BASE + 6L)   /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_ARM_ALARM_TIME_FRACTION     (IVI_LXISYNC_ATTR_BASE + 5L)   /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_ARM_ALARM_PERIOD            (IVI_LXISYNC_ATTR_BASE + 3L)   /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_ARM_ALARM_REPEAT_COUNT      (IVI_LXISYNC_ATTR_BASE + 4L)   /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_ARM_ALARM_ENABLED           (IVI_LXISYNC_ATTR_BASE + 2L)   /* ViBoolean, read-write */

    /*- Event Log */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_EVENT_LOG_ENTRY_COUNT     (IVI_LXISYNC_ATTR_BASE + 35L)  /* ViInt32, read-only */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_EVENT_LOG_ENABLED         (IVI_LXISYNC_ATTR_BASE + 34L)  /* ViBoolean, read-write */

    /*- Event */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_EVENT_COUNT                  (IVI_INHERENT_ATTR_BASE + 28L)  /* ViInt32, read-only */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_EVENT_WIRED_OR_BIAS_MODE     (IVI_LXISYNC_ATTR_BASE + 33L)   /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_EVENT_SOURCE                 (IVI_LXISYNC_ATTR_BASE + 32L)   /* ViString, read-write */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_EVENT_DESTINATION_PATH       (IVI_LXISYNC_ATTR_BASE + 29L)   /* ViString, read-write */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_EVENT_SLOPE                  (IVI_LXISYNC_ATTR_BASE + 31L)   /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_EVENT_DRIVE_MODE             (IVI_LXISYNC_ATTR_BASE + 30L)   /* ViInt32, read-write */

    /*- Time */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_IS_TIME_MASTER           (IVI_LXISYNC_ATTR_BASE + 36L)  /* ViBoolean, read-only */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_IS_TIME_SYNCHRONIZED     (IVI_LXISYNC_ATTR_BASE + 37L)  /* ViBoolean, read-only */

    /*- Trigger */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_TRIGGER_COUNT        (IVI_LXISYNC_ATTR_BASE + 21L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_TRIGGER_SOURCE       (IVI_LXISYNC_ATTR_BASE + 22L)  /* ViString, read-write */

    /*- Source */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_TRIGGER_SOURCE_COUNT         (IVI_INHERENT_ATTR_BASE + 23L)  /* ViInt32, read-only */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_TRIGGER_SOURCE_DELAY         (IVI_LXISYNC_ATTR_BASE + 24L)   /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_TRIGGER_SOURCE_DETECTION     (IVI_LXISYNC_ATTR_BASE + 25L)   /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_TRIGGER_SOURCE_EVENTID       (IVI_LXISYNC_ATTR_BASE + 26L)   /* ViString, read-write */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_TRIGGER_SOURCE_FILTER        (IVI_LXISYNC_ATTR_BASE + 27L)   /* ViString, read-write */

    /*- Alarm */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_TRIGGER_ALARM_COUNT             (IVI_INHERENT_ATTR_BASE + 15L)  /* ViInt32, read-only */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_TRIGGER_ALARM_PERIOD            (IVI_LXISYNC_ATTR_BASE + 17L)   /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_TRIGGER_ALARM_REPEAT_COUNT      (IVI_LXISYNC_ATTR_BASE + 18L)   /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_TRIGGER_ALARM_TIME_FRACTION     (IVI_LXISYNC_ATTR_BASE + 19L)   /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_TRIGGER_ALARM_TIME_SECONDS      (IVI_LXISYNC_ATTR_BASE + 20L)   /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_IVILXISYNC_TRIGGER_ALARM_ENABLED           (IVI_LXISYNC_ATTR_BASE + 16L)   /* ViBoolean, read-write */

    /*- Arb */
#define	AGRFSIGGEN_ATTR_DIGITALMODULATION_ARB_COPY_WAVEFORM_FROM_NV_MEMORY     (IVI_SPECIFIC_ATTR_BASE + 22L)  /* ViString, read-write */

    /*- Display */
#define	AGRFSIGGEN_ATTR_DISPLAY_DISPLAY_ENABLED         (IVI_SPECIFIC_ATTR_BASE + 53L)  /* ViBoolean, read-write */

    /*- Status */
#define	AGRFSIGGEN_ATTR_STATUS_SERIAL_POLL              (IVI_SPECIFIC_ATTR_BASE + 87L)  /* ViInt32, read-only */

    /*- System */
#define	AGRFSIGGEN_ATTR_SYSTEM_FW_VERSION               (IVI_SPECIFIC_ATTR_BASE + 140L)  /* ViString, read-only */
#define	AGRFSIGGEN_ATTR_SYSTEM_SERIAL_NUMBER            (IVI_SPECIFIC_ATTR_BASE + 141L)  /* ViString, read-only */
#define	AGRFSIGGEN_ATTR_SYSTEM_TIMEOUT_MILLISECONDS     (IVI_SPECIFIC_ATTR_BASE + 142L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_SYSTEM_TRACE_ENABLED            (IVI_SPECIFIC_ATTR_BASE + 143L)  /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_SYSTEM_LXI_DOMAIN               (IVI_SPECIFIC_ATTR_BASE + 157L)  /* ViInt32, read-write */

    /*- Configuration */
#define	AGRFSIGGEN_ATTR_PTP_CONFIGURATION_ACCURACY                     (IVI_SPECIFIC_ATTR_BASE + 800L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_PTP_CONFIGURATION_DEVIATION                    (IVI_SPECIFIC_ATTR_BASE + 804L)  /* ViReal64, read-only */
#define	AGRFSIGGEN_ATTR_PTP_CONFIGURATION_DOMAIN                       (IVI_SPECIFIC_ATTR_BASE + 805L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_PTP_CONFIGURATION_OFFSET                       (IVI_SPECIFIC_ATTR_BASE + 810L)  /* ViReal64, read-only */
#define	AGRFSIGGEN_ATTR_PTP_CONFIGURATION_PRIORITY1                    (IVI_SPECIFIC_ATTR_BASE + 811L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_PTP_CONFIGURATION_PRIORITY2                    (IVI_SPECIFIC_ATTR_BASE + 812L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_PTP_CONFIGURATION_SYNC_INTERVAL                (IVI_SPECIFIC_ATTR_BASE + 814L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_PTP_CONFIGURATION_STATE                        (IVI_SPECIFIC_ATTR_BASE + 813L)  /* ViInt32, read-only */
#define	AGRFSIGGEN_ATTR_PTP_CONFIGURATION_TRACEABILITY                 (IVI_SPECIFIC_ATTR_BASE + 815L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_PTP_CONFIGURATION_VARIANCE                     (IVI_SPECIFIC_ATTR_BASE + 816L)  /* ViReal64, read-only */
#define	AGRFSIGGEN_ATTR_PTP_CONFIGURATION_ANNOUNCE_INTERVAL            (IVI_SPECIFIC_ATTR_BASE + 801L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_PTP_CONFIGURATION_ANNOUNC_RECEIPT_TIME_OUT     (IVI_SPECIFIC_ATTR_BASE + 802L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_PTP_CONFIGURATION_DELAY_REQUEST_INTERVAL       (IVI_SPECIFIC_ATTR_BASE + 803L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_PTP_CONFIGURATION_GRANDMASTER_ACCURACY         (IVI_SPECIFIC_ATTR_BASE + 806L)  /* ViInt32, read-only */
#define	AGRFSIGGEN_ATTR_PTP_CONFIGURATION_GRANDMASTER_MAC_ADDRESS      (IVI_SPECIFIC_ATTR_BASE + 807L)  /* ViString, read-only */
#define	AGRFSIGGEN_ATTR_PTP_CONFIGURATION_GRANDMASTER_TRACEABILITY     (IVI_SPECIFIC_ATTR_BASE + 808L)  /* ViInt32, read-only */
#define	AGRFSIGGEN_ATTR_PTP_CONFIGURATION_MASTER_MAC_ADDRESS           (IVI_SPECIFIC_ATTR_BASE + 809L)  /* ViString, read-only */

    /*- ServoAlgorithm */
#define	AGRFSIGGEN_ATTR_PTP_SERVOALGORITHM_ASYMMETRY                         (IVI_SPECIFIC_ATTR_BASE + 817L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_PTP_SERVOALGORITHM_COARSE_FINE_THRESHOLD             (IVI_SPECIFIC_ATTR_BASE + 818L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_PTP_SERVOALGORITHM_COARSE_INTEGRAL_CONSTANT          (IVI_SPECIFIC_ATTR_BASE + 819L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_PTP_SERVOALGORITHM_COARSE_PROPORTIONAL_CONSTANT      (IVI_SPECIFIC_ATTR_BASE + 820L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_PTP_SERVOALGORITHM_FINE_INTEGRAL_CONSTANT            (IVI_SPECIFIC_ATTR_BASE + 821L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_PTP_SERVOALGORITHM_FINE_PROPORTIONAL_CONSTANT        (IVI_SPECIFIC_ATTR_BASE + 822L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_PTP_SERVOALGORITHM_OUTLIER_MAX_DISCARD_COUNT         (IVI_SPECIFIC_ATTR_BASE + 823L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_PTP_SERVOALGORITHM_OUTLIER_MIN_DISCARD_THRESHOLD     (IVI_SPECIFIC_ATTR_BASE + 824L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_PTP_SERVOALGORITHM_OUTLIER_THRESHOLD_ENABLED         (IVI_SPECIFIC_ATTR_BASE + 826L)  /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_PTP_SERVOALGORITHM_OUTLIER_THRESHOLD                 (IVI_SPECIFIC_ATTR_BASE + 825L)  /* ViReal64, read-write */
#define	AGRFSIGGEN_ATTR_PTP_SERVOALGORITHM_STEERING_THRESHOLD                (IVI_SPECIFIC_ATTR_BASE + 827L)  /* ViReal64, read-write */

    /*- Log */
#define	AGRFSIGGEN_ATTR_PTP_SERVOALGORITHM_LOG_CIRCULAR_MODE_ENABLED     (IVI_SPECIFIC_ATTR_BASE + 829L)  /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_PTP_SERVOALGORITHM_LOG_COUNT                     (IVI_SPECIFIC_ATTR_BASE + 830L)  /* ViInt32, read-only */
#define	AGRFSIGGEN_ATTR_PTP_SERVOALGORITHM_LOG_ENABLED                   (IVI_SPECIFIC_ATTR_BASE + 831L)  /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_PTP_SERVOALGORITHM_LOG_SIZE                      (IVI_SPECIFIC_ATTR_BASE + 832L)  /* ViInt32, read-write */

    /*- Synchronization */
#define	AGRFSIGGEN_ATTR_PTP_SYNCHRONIZATION_IS_MASTER     (IVI_SPECIFIC_ATTR_BASE + 833L)  /* ViBoolean, read-only */

    /*- Time */
#define	AGRFSIGGEN_ATTR_PTP_TIME_DAY_LIGHT_SAVINGS       (IVI_SPECIFIC_ATTR_BASE + 834L)  /* ViBoolean, read-write */
#define	AGRFSIGGEN_ATTR_PTP_TIME_LOCAL_TIME              (IVI_SPECIFIC_ATTR_BASE + 836L)  /* ViString, read-only */
#define	AGRFSIGGEN_ATTR_PTP_TIME_LEAP_SECONDS_OFFSET     (IVI_SPECIFIC_ATTR_BASE + 835L)  /* ViInt32, read-write */
#define	AGRFSIGGEN_ATTR_PTP_TIME_TAI                     (IVI_SPECIFIC_ATTR_BASE + 837L)  /* ViString, read-only */
#define	AGRFSIGGEN_ATTR_PTP_TIME_TIME_ZONE               (IVI_SPECIFIC_ATTR_BASE + 838L)  /* ViString, read-write */
#define	AGRFSIGGEN_ATTR_PTP_TIME_UTC                     (IVI_SPECIFIC_ATTR_BASE + 839L)  /* ViString, read-only */
		


/**************************************************************************** 
 *------------------------ Attribute Value Defines -------------------------* 
 ****************************************************************************/

    /*- Defined values for attribute AGRFSIGGEN_ATTR_SWEEP_MODE, parameter Mode in function AgRfSigGen_ConfigureSweep */
#define	AGRFSIGGEN_VAL_SWEEP_MODE_NONE                  0
#define	AGRFSIGGEN_VAL_SWEEP_MODE_FREQUENCY_SWEEP       1
#define	AGRFSIGGEN_VAL_SWEEP_MODE_POWER_SWEEP           2
#define	AGRFSIGGEN_VAL_SWEEP_MODE_FREQUENCY_STEP        3
#define	AGRFSIGGEN_VAL_SWEEP_MODE_POWER_STEP            4
#define	AGRFSIGGEN_VAL_SWEEP_MODE_LIST                  5

    /*- Defined values for attribute AGRFSIGGEN_ATTR_PULSE_EXTERNAL_TRIGGER_SLOPE, 
        parameter Slope in function AgRfSigGen_ConfigurePulseExternalTrigger */
#define	AGRFSIGGEN_VAL_PULSE_EXTERNAL_TRIGGER_SLOPE_POSITIVE 0
#define	AGRFSIGGEN_VAL_PULSE_EXTERNAL_TRIGGER_SLOPE_NEGATIVE 1

    /*- Defined values for attribute AGRFSIGGEN_ATTR_PULSE_TRIGGER_SOURCE, 
        parameter PulseTriggerSource in function AgRfSigGen_ConfigurePulse */
#define	AGRFSIGGEN_VAL_PULSE_TRIGGER_SOURCE_INTERNAL    0
#define	AGRFSIGGEN_VAL_PULSE_TRIGGER_SOURCE_EXTERNAL    1

    /*- Defined values for attribute AGRFSIGGEN_ATTR_ALC_SOURCE, parameter Source in function AgRfSigGen_ConfigureALC */
#define	AGRFSIGGEN_VAL_ALC_SOURCE_INTERNAL              0
#define	AGRFSIGGEN_VAL_ALC_SOURCE_EXTERNAL              1

    /*- Defined values for attribute AGRFSIGGEN_ATTR_LF_GENERATOR_WAVEFORM, 
        parameter Waveform in function AgRfSigGen_ConfigureLFGenerator */
#define	AGRFSIGGEN_VAL_LF_GENERATOR_WAVEFORM_SINE       0
#define	AGRFSIGGEN_VAL_LF_GENERATOR_WAVEFORM_SQUARE     1
#define	AGRFSIGGEN_VAL_LF_GENERATOR_WAVEFORM_TRIANGLE   2
#define	AGRFSIGGEN_VAL_LF_GENERATOR_WAVEFORM_RAMP_UP    3
#define	AGRFSIGGEN_VAL_LF_GENERATOR_WAVEFORM_RAMP_DOWN  4

    /*- Defined values for attribute AGRFSIGGEN_ATTR_PULSE_OUTPUT_POLARITY, 
        parameter Polarity in function AgRfSigGen_ConfigurePulseOutput */
#define	AGRFSIGGEN_VAL_PULSE_OUTPUT_POLARITY_NORMAL     0
#define	AGRFSIGGEN_VAL_PULSE_OUTPUT_POLARITY_INVERSE    1

    /*- Defined values for attribute AGRFSIGGEN_ATTR_FREQUENCY_STEP_SCALING, 
        parameter Scaling in function AgRfSigGen_ConfigureFrequencyStepStartStop */
#define	AGRFSIGGEN_VAL_FREQUENCY_STEP_SCALING_LINEAR    0
#define	AGRFSIGGEN_VAL_FREQUENCY_STEP_SCALING_LOGARITHMIC 1

    /*- Defined values for attribute AGRFSIGGEN_ATTR_SWEEP_TRIGGER_SOURCE, 
        parameter TriggerSource in function AgRfSigGen_ConfigureSweep */
#define	AGRFSIGGEN_VAL_SWEEP_TRIGGER_SOURCE_IMMEDIATE   0
#define	AGRFSIGGEN_VAL_SWEEP_TRIGGER_SOURCE_EXTERNAL    1
#define	AGRFSIGGEN_VAL_SWEEP_TRIGGER_SOURCE_SOFTWARE    2

    /*- Defined values for attribute AGRFSIGGEN_ATTR_REFERENCE_OSCILLATOR_SOURCE, 
        parameter Source in function AgRfSigGen_ConfigureReferenceOscillator */
#define	AGRFSIGGEN_VAL_REFERENCE_OSCILLATOR_SOURCE_INTERNAL 0
#define	AGRFSIGGEN_VAL_REFERENCE_OSCILLATOR_SOURCE_EXTERNAL 1

    /*- Defined values for attribute AGRFSIGGEN_ATTR_AM_SCALING, parameter Scaling in function AgRfSigGen_ConfigureAM */
#define	AGRFSIGGEN_VAL_AM_SCALING_LINEAR                0
#define	AGRFSIGGEN_VAL_AM_SCALING_LOGARITHMIC           1

    /*- Defined values for attribute AGRFSIGGEN_ATTR_AM_EXTERNAL_COUPLING */
#define	AGRFSIGGEN_VAL_AM_EXTERNAL_COUPLING_AC          0
#define	AGRFSIGGEN_VAL_AM_EXTERNAL_COUPLING_DC          1

    /*- Defined values for attribute AGRFSIGGEN_ATTR_FM_EXTERNAL_COUPLING */
#define	AGRFSIGGEN_VAL_FM_EXTERNAL_COUPLING_AC          0
#define	AGRFSIGGEN_VAL_FM_EXTERNAL_COUPLING_DC          1

    /*- Defined values for attribute AGRFSIGGEN_ATTR_PM_EXTERNAL_COUPLING */
#define	AGRFSIGGEN_VAL_PM_EXTERNAL_COUPLING_AC          0
#define	AGRFSIGGEN_VAL_PM_EXTERNAL_COUPLING_DC          1

    /*- Defined values for attribute AGRFSIGGEN_ATTR_PULSE_MODULATION_EXTERNAL_POLARITY */
#define	AGRFSIGGEN_VAL_PULSE_MODULATION_SOURCE_INTERNAL 0
#define	AGRFSIGGEN_VAL_PULSE_MODULATION_SOURCE_EXTERNAL 1

    /*- Defined values for attribute AGRFSIGGEN_ATTR_ARB_TRIGGER_SOURCE */
#define	AGRFSIGGEN_VAL_ARB_TRIGGER_SOURCE_IMMEDIATE     0
#define	AGRFSIGGEN_VAL_ARB_TRIGGER_SOURCE_EXTERNAL      1
#define	AGRFSIGGEN_VAL_ARB_TRIGGER_SOURCE_SOFTWARE      2

    /*- Defined values for attribute AGRFSIGGEN_ATTR_ARB_EXTERNAL_TRIGGER_SLOPE */
#define	AGRFSIGGEN_VAL_ARB_EXTERNAL_TRIGGER_SLOPE_POSITIVE 0
#define	AGRFSIGGEN_VAL_ARB_EXTERNAL_TRIGGER_SLOPE_NEGATIVE 1

    /*- Defined values for attribute AGRFSIGGEN_ATTR_DIGITAL_MODULATION_BASE_DATA_SOURCE */
#define	AGRFSIGGEN_VAL_DIGITAL_MODULATION_BASE_DATA_SOURCE_EXTERNAL 0
#define	AGRFSIGGEN_VAL_DIGITAL_MODULATION_BASE_DATA_SOURCE_PRBS 1
#define	AGRFSIGGEN_VAL_DIGITAL_MODULATION_BASE_DATA_SOURCE_BIT_SEQUENCE 2

    /*- Defined values for attribute AGRFSIGGEN_ATTR_DIGITAL_MODULATION_BASE_PRBS_TYPE */
#define	AGRFSIGGEN_VAL_DIGITAL_MODULATION_BASE_PRBS_TYPE_PRBS9 0
#define	AGRFSIGGEN_VAL_DIGITAL_MODULATION_BASE_PRBS_TYPE_PRBS11 1
#define	AGRFSIGGEN_VAL_DIGITAL_MODULATION_BASE_PRBS_TYPE_PRBS15 2
#define	AGRFSIGGEN_VAL_DIGITAL_MODULATION_BASE_PRBS_TYPE_PRBS16 3
#define	AGRFSIGGEN_VAL_DIGITAL_MODULATION_BASE_PRBS_TYPE_PRBS20 4
#define	AGRFSIGGEN_VAL_DIGITAL_MODULATION_BASE_PRBS_TYPE_PRBS21 5
#define	AGRFSIGGEN_VAL_DIGITAL_MODULATION_BASE_PRBS_TYPE_PRBS23 6

    /*- Defined values for attribute AGRFSIGGEN_ATTR_DIGITAL_MODULATION_BASE_CLOCK_SOURCE, 
        parameter Source in function AgRfSigGen_ConfigureDigitalModulationBaseClockSource */
#define	AGRFSIGGEN_VAL_DIGITAL_MODULATION_BASE_CLOCK_SOURCE_INTERNAL 0
#define	AGRFSIGGEN_VAL_DIGITAL_MODULATION_BASE_CLOCK_SOURCE_EXTERNAL 1

    /*- Defined values for attribute AGRFSIGGEN_ATTR_DIGITAL_MODULATION_BASE_EXTERNAL_CLOCK_TYPE, 
        parameter Type in function AgRfSigGen_ConfigureDigitalModulationBaseClockSource */
#define	AGRFSIGGEN_VAL_DIGITAL_MODULATION_BASE_EXTERNAL_CLOCK_TYPE_BIT 0
#define	AGRFSIGGEN_VAL_DIGITAL_MODULATION_BASE_EXTERNAL_CLOCK_TYPE_SYMBOL 1

    /*- Defined values for attribute AGRFSIGGEN_ATTR_TDMA_TRIGGER_SOURCE */
#define	AGRFSIGGEN_VAL_TDMA_TRIGGER_SOURCE_IMMEDIATE    0
#define	AGRFSIGGEN_VAL_TDMA_TRIGGER_SOURCE_EXTERNAL     1
#define	AGRFSIGGEN_VAL_TDMA_TRIGGER_SOURCE_SOFTWARE     2

    /*- Defined values for attribute AGRFSIGGEN_ATTR_TDMA_EXTERNAL_TRIGGER_SLOPE */
#define	AGRFSIGGEN_VAL_TDMA_EXTERNAL_TRIGGER_SLOPE_POSITIVE 0
#define	AGRFSIGGEN_VAL_TDMA_EXTERNAL_TRIGGER_SLOPE_NEGATIVE 1

    /*- Defined values for attribute AGRFSIGGEN_ATTR_TDMA_CLOCK_SOURCE, 
        parameter Source in function AgRfSigGen_ConfigureTDMAClockSource */
#define	AGRFSIGGEN_VAL_TDMA_CLOCK_SOURCE_INTERNAL       0
#define	AGRFSIGGEN_VAL_TDMA_CLOCK_SOURCE_EXTERNAL       1

    /*- Defined values for attribute AGRFSIGGEN_ATTR_TDMA_EXTERNAL_CLOCK_TYPE, 
        parameter Type in function AgRfSigGen_ConfigureTDMAClockSource */
#define	AGRFSIGGEN_VAL_TDMA_EXTERNAL_CLOCK_TYPE_BIT     0
#define	AGRFSIGGEN_VAL_TDMA_EXTERNAL_CLOCK_TYPE_SYMBOL  1

    /*- Defined values for attribute AGRFSIGGEN_ATTR_CDMA_TRIGGER_SOURCE */
#define	AGRFSIGGEN_VAL_CDMA_TRIGGER_SOURCE_IMMEDIATE    0
#define	AGRFSIGGEN_VAL_CDMA_TRIGGER_SOURCE_EXTERNAL     1
#define	AGRFSIGGEN_VAL_CDMA_TRIGGER_SOURCE_SOFTWARE     2

    /*- Defined values for attribute AGRFSIGGEN_ATTR_CDMA_EXTERNAL_TRIGGER_SLOPE */
#define	AGRFSIGGEN_VAL_CDMA_EXTERNAL_TRIGGER_SLOPE_POSITIVE 0
#define	AGRFSIGGEN_VAL_CDMA_EXTERNAL_TRIGGER_SLOPE_NEGATIVE 1

    /*- Defined values for attribute AGRFSIGGEN_ATTR_CDMA_CLOCK_SOURCE */
#define	AGRFSIGGEN_VAL_CDMA_CLOCK_SOURCE_INTERNAL       0
#define	AGRFSIGGEN_VAL_CDMA_CLOCK_SOURCE_EXTERNAL       1

    /*- Defined values for attribute AGRFSIGGEN_ATTR_IQ_SOURCE, parameter Source in function AgRfSigGen_ConfigureIQ */
#define	AGRFSIGGEN_VAL_IQ_SOURCE_DIGITAL_MODULATION_BASE 0
#define	AGRFSIGGEN_VAL_IQ_SOURCE_CDMA_BASE              1
#define	AGRFSIGGEN_VAL_IQ_SOURCE_TDMA_BASE              2
#define	AGRFSIGGEN_VAL_IQ_SOURCE_ARB_GENERATOR          3
#define	AGRFSIGGEN_VAL_IQ_SOURCE_EXTERNAL               4

    /*- Defined values for attribute AGRFSIGGEN_ATTR_IVILXISYNC_ARM_SOURCE_DETECTION, 
        attribute AGRFSIGGEN_ATTR_IVILXISYNC_TRIGGER_SOURCE_DETECTION, 
        parameter Detection in function AgRfSigGen_IviLxiSync_ConfigureArmSource, 
        parameter Detection in function AgRfSigGen_IviLxiSync_ConfigureTriggerSource */
#define	AGRFSIGGEN_VAL_IVILXISYNC_DETECTION_RISE        0
#define	AGRFSIGGEN_VAL_IVILXISYNC_DETECTION_FALL        1
#define	AGRFSIGGEN_VAL_IVILXISYNC_DETECTION_HIGH        2
#define	AGRFSIGGEN_VAL_IVILXISYNC_DETECTION_LOW         3

    /*- Defined values for attribute AGRFSIGGEN_ATTR_IVILXISYNC_EVENT_SLOPE, 
        parameter Slope in function AgRfSigGen_IviLxiSync_ConfigureEvent */
#define	AGRFSIGGEN_VAL_IVILXISYNC_SLOPE_RISE            0
#define	AGRFSIGGEN_VAL_IVILXISYNC_SLOPE_FALL            1

    /*- Defined values for attribute AGRFSIGGEN_ATTR_IVILXISYNC_EVENT_DRIVE_MODE, 
        parameter DriveMode in function AgRfSigGen_IviLxiSync_ConfigureEvent */
#define	AGRFSIGGEN_VAL_IVILXISYNC_EVENT_DRIVEN          0
#define	AGRFSIGGEN_VAL_IVILXISYNC_EVENT_OFF             1
#define	AGRFSIGGEN_VAL_IVILXISYNC_EVENT_WIREDOR         2

    /*- Defined values for attribute AGRFSIGGEN_ATTR_SWEEP_FREQUENCYSTEP_SINGLE_STEP_SOURCE, 
        attribute AGRFSIGGEN_ATTR_SWEEP_LIST_SINGLE_STEP_SOURCE, attribute AGRFSIGGEN_ATTR_SWEEP_POWERSTEP_SINGLE_STEP_SOURCE */
#define	AGRFSIGGEN_VAL_SWEEP_TRIGGER_SOURCE_IMMEDIATE   0
#define	AGRFSIGGEN_VAL_SWEEP_TRIGGER_SOURCE_EXTERNAL    1
#define	AGRFSIGGEN_VAL_SWEEP_TRIGGER_SOURCE_SOFTWARE    2
#define	AGRFSIGGEN_VAL_SWEEP_TRIGGER_SOURCE_KEY         3
#define	AGRFSIGGEN_VAL_SWEEP_TRIGGER_SOURCE_LAN         4

    /*- Defined values for attribute AGRFSIGGEN_ATTR_PTP_CONFIGURATION_ACCURACY, 
        attribute AGRFSIGGEN_ATTR_PTP_CONFIGURATION_GRANDMASTER_ACCURACY */
#define	AGRFSIGGEN_VAL_PTP_ACCURACY_NS25                0
#define	AGRFSIGGEN_VAL_PTP_ACCURACY_NS100               1
#define	AGRFSIGGEN_VAL_PTP_ACCURACY_NS250               2
#define	AGRFSIGGEN_VAL_PTP_ACCURACY_NS1000              3
#define	AGRFSIGGEN_VAL_PTP_ACCURACY_NS2500              4
#define	AGRFSIGGEN_VAL_PTP_ACCURACY_US10                5
#define	AGRFSIGGEN_VAL_PTP_ACCURACY_US25                6
#define	AGRFSIGGEN_VAL_PTP_ACCURACY_US100               7
#define	AGRFSIGGEN_VAL_PTP_ACCURACY_US250               8
#define	AGRFSIGGEN_VAL_PTP_ACCURACY_US1000              9
#define	AGRFSIGGEN_VAL_PTP_ACCURACY_US2500              10
#define	AGRFSIGGEN_VAL_PTP_ACCURACY_MS10                11
#define	AGRFSIGGEN_VAL_PTP_ACCURACY_MS25                12
#define	AGRFSIGGEN_VAL_PTP_ACCURACY100                  13
#define	AGRFSIGGEN_VAL_PTP_ACCURACY_MS250               14
#define	AGRFSIGGEN_VAL_PTP_ACCURACY_S10                 16
#define	AGRFSIGGEN_VAL_PTP_ACCURACY_GT10S               17
#define	AGRFSIGGEN_VAL_PTP_ACCURACY_UNKNOWN             18

    /*- Defined values for attribute AGRFSIGGEN_ATTR_PTP_CONFIGURATION_STATE */
#define	AGRFSIGGEN_VAL_PTP_STATE_INITIALIZING           0
#define	AGRFSIGGEN_VAL_PTP_STATE_FAULTY                 1
#define	AGRFSIGGEN_VAL_PTP_STATE_DISABLED               2
#define	AGRFSIGGEN_VAL_PTP_STATE_LISTENING              3
#define	AGRFSIGGEN_VAL_PTP_STATE_PREMASTER              4
#define	AGRFSIGGEN_VAL_PTP_STATE_MASTER                 5
#define	AGRFSIGGEN_VAL_PTP_STATE_PASSIVE                6
#define	AGRFSIGGEN_VAL_PTP_STATE_UNCALIBRATED           7
#define	AGRFSIGGEN_VAL_PTP_STATE_SLAVE                  8

    /*- Defined values for attribute AGRFSIGGEN_ATTR_PTP_CONFIGURATION_TRACEABILITY, 
        attribute AGRFSIGGEN_ATTR_PTP_CONFIGURATION_GRANDMASTER_TRACEABILITY */
#define	AGRFSIGGEN_VAL_PTP_TRACEABILITY_ATOMIC          0
#define	AGRFSIGGEN_VAL_PTP_TRACEABILITY_GPS             1
#define	AGRFSIGGEN_VAL_PTP_TRACEABILITY_RADIO           2
#define	AGRFSIGGEN_VAL_PTP_TRACEABILITY_PTP             3
#define	AGRFSIGGEN_VAL_PTP_TRACEABILITY_NTP             4
#define	AGRFSIGGEN_VAL_PTP_TRACEABILITY_HANDSET         5
#define	AGRFSIGGEN_VAL_PTP_TRACEABILITY_OTHER           6
#define	AGRFSIGGEN_VAL_PTP_TRACEABILITY_OSCILLATOR      7

    /*- Defined values for parameter Reason in function AgRfSigGen_StatusConfigureServiceRequest */
#define	AGRFSIGGEN_VAL_SRQ_REASON_STB_MAV               1
#define	AGRFSIGGEN_VAL_SRQ_REASON_STB_ERROR_QUE         2
#define	AGRFSIGGEN_VAL_SRQ_REASON_ESR_OPC               4
#define	AGRFSIGGEN_VAL_SRQ_REASON_ESR_QUERY_ERROR       8
#define	AGRFSIGGEN_VAL_SRQ_REASON_ESR_DEVICE_ERROR      16
#define	AGRFSIGGEN_VAL_SRQ_REASON_ESR_EXECUTION_ERROR   32
#define	AGRFSIGGEN_VAL_SRQ_REASON_ESR_COMMAND_ERROR     64
#define	AGRFSIGGEN_VAL_SRQ_REASON_ESR_USER_REQUEST      128
#define	AGRFSIGGEN_VAL_SRQ_REASON_QUES_VOLTAGE          256
#define	AGRFSIGGEN_VAL_SRQ_REASON_QUES_CURRENT          512
#define	AGRFSIGGEN_VAL_SRQ_REASON_QUES_TIME             1024
#define	AGRFSIGGEN_VAL_SRQ_REASON_QUES_POWER            2048
#define	AGRFSIGGEN_VAL_SRQ_REASON_QUES_TEMPERATURE      4096
#define	AGRFSIGGEN_VAL_SRQ_REASON_QUES_FREQUENCY        8192
#define	AGRFSIGGEN_VAL_SRQ_REASON_QUES_PHASE            16384
#define	AGRFSIGGEN_VAL_SRQ_REASON_QUES_MODULATION       32768
#define	AGRFSIGGEN_VAL_SRQ_REASON_QUES_CALIBRATION      65536
#define	AGRFSIGGEN_VAL_SRQ_REASON_QUES_INSTRUMENT       131072

    /*- Defined values for parameter Register in function AgRfSigGen_StatusGetRegister, 
        parameter Register in function AgRfSigGen_StatusSetRegister */
#define	AGRFSIGGEN_VAL_STATUS_REGISTER_STATUS_BYTE      0
#define	AGRFSIGGEN_VAL_STATUS_REGISTER_STANDARD_EVENT   1
#define	AGRFSIGGEN_VAL_STATUS_REGISTER_OPERATION        2
#define	AGRFSIGGEN_VAL_STATUS_REGISTER_QUESTIONABLE     3

    /*- Defined values for parameter SubRegister in function AgRfSigGen_StatusGetRegister, 
        parameter SubRegister in function AgRfSigGen_StatusSetRegister */
#define	AGRFSIGGEN_VAL_STATUS_SUB_REGISTER_CONDITION    0
#define	AGRFSIGGEN_VAL_STATUS_SUB_REGISTER_NEGATIVE_TRANSITION 1
#define	AGRFSIGGEN_VAL_STATUS_SUB_REGISTER_POSITIVE_TRANSITION 2
#define	AGRFSIGGEN_VAL_STATUS_SUB_REGISTER_EVENT        3
#define	AGRFSIGGEN_VAL_STATUS_SUB_REGISTER_ENABLE       4



/**************************************************************************** 
 *---------------- Instrument Driver Function Declarations -----------------* 
 ****************************************************************************/

    /*- AgilentRfSigGen */

ViStatus _VI_FUNC  AgRfSigGen_init ( ViRsrc ResourceName, ViBoolean IdQuery, ViBoolean Reset, ViSession* Vi );

ViStatus _VI_FUNC  AgRfSigGen_GetNativeIUnknownPtr ( ViSession Vi, IUnknown** NativeIUnknownPtr );

ViStatus _VI_FUNC  AgRfSigGen_close ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_InitWithOptions ( ViRsrc ResourceName, ViBoolean IdQuery, ViBoolean Reset, ViConstString OptionsString, ViSession* Vi );

    /*- System */

ViStatus _VI_FUNC  AgRfSigGen_SystemRead ( ViSession Vi, ViInt32 Size, ViChar Value[], ViInt32* ActualSize );

ViStatus _VI_FUNC  AgRfSigGen_SystemWrite ( ViSession Vi, ViConstString CommandString );

ViStatus _VI_FUNC  AgRfSigGen_SystemWaitForOperationComplete ( ViSession Vi, ViInt32 MaxTimeMilliseconds );

ViStatus _VI_FUNC  AgRfSigGen_SystemSaveState ( ViSession Vi, ViConstString Identifier );

ViStatus _VI_FUNC  AgRfSigGen_SystemRecallState ( ViSession Vi, ViConstString Identifier );

ViStatus _VI_FUNC  AgRfSigGen_SystemEnableLocalControls ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_SystemDisableLocalControls ( ViSession Vi );

    /*- ArmAlarm */

ViStatus _VI_FUNC  AgRfSigGen_GetArmAlarmName ( ViSession Vi, ViInt32 Index, ViInt32 NameBufferSize, ViChar Name[] );

    /*- ArmSource */

ViStatus _VI_FUNC  AgRfSigGen_GetArmSourceName ( ViSession Vi, ViInt32 Index, ViInt32 NameBufferSize, ViChar Name[] );

    /*- Event */

ViStatus _VI_FUNC  AgRfSigGen_GetEventName ( ViSession Vi, ViInt32 Index, ViInt32 NameBufferSize, ViChar Name[] );

    /*- TriggerAlarm */

ViStatus _VI_FUNC  AgRfSigGen_GetTriggerAlarmName ( ViSession Vi, ViInt32 Index, ViInt32 NameBufferSize, ViChar Name[] );

    /*- TriggerSource */

ViStatus _VI_FUNC  AgRfSigGen_GetTriggerSourceName ( ViSession Vi, ViInt32 Index, ViInt32 NameBufferSize, ViChar Name[] );

    /*- RF */

ViStatus _VI_FUNC  AgRfSigGen_ConfigureOutputEnabled ( ViSession Vi, ViBoolean OutputEnabled );

    /*- Pulse Generator */

ViStatus _VI_FUNC  AgRfSigGen_ConfigurePulseInternalTrigger ( ViSession Vi, ViReal64 Period );

    /*- Digital Modulation Base */

ViStatus _VI_FUNC  AgRfSigGen_WriteDigitalModulationBaseBitSequence ( ViSession Vi, ViConstString Name, ViInt32 BitCount, ViChar Sequence[], ViBoolean MoreDataPending );

ViStatus _VI_FUNC  AgRfSigGen_GetDigitalModulationBaseStandardName ( ViSession Vi, ViInt32 Index, ViInt32 NameBufferSize, ViChar Name[] );

ViStatus _VI_FUNC  AgRfSigGen_SelectDigitalModulationBaseStandard ( ViSession Vi, ViConstString Name );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureDigitalModulationBaseDataSource ( ViSession Vi, ViInt32 Source );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureDigitalModulationBasePRBSType ( ViSession Vi, ViInt32 Type );

ViStatus _VI_FUNC  AgRfSigGen_SelectDigitalModulationBaseBitSequence ( ViSession Vi, ViConstString Name );

ViStatus _VI_FUNC  AgRfSigGen_ClearAllDigitalModulationBaseBitSequences ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureDigitalModulationBaseClockSource ( ViSession Vi, ViInt32 Source, ViInt32 Type );

    /*- RF */

ViStatus _VI_FUNC  AgRfSigGen_ConfigureALCEnabled ( ViSession Vi, ViBoolean ALCEnabled );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureRF ( ViSession Vi, ViReal64 Frequency, ViReal64 PowerLevel );

    /*- Analog Modulation */

ViStatus _VI_FUNC  AgRfSigGen_ConfigureAMEnabled ( ViSession Vi, ViBoolean Enabled );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureAMExternalCoupling ( ViSession Vi, ViInt32 Coupling );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureFMEnabled ( ViSession Vi, ViBoolean Enabled );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureFMExternalCoupling ( ViSession Vi, ViInt32 Coupling );

ViStatus _VI_FUNC  AgRfSigGen_ConfigurePMEnabled ( ViSession Vi, ViBoolean Enabled );

ViStatus _VI_FUNC  AgRfSigGen_ConfigurePMExternalCoupling ( ViSession Vi, ViInt32 Coupling );

ViStatus _VI_FUNC  AgRfSigGen_GetAnalogModulationSourceName ( ViSession Vi, ViInt32 Index, ViInt32 NameBufferSize, ViChar Name[] );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureAM ( ViSession Vi, ViConstString Source, ViInt32 Scaling, ViReal64 Depth );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureFM ( ViSession Vi, ViConstString Source, ViReal64 Deviation );

ViStatus _VI_FUNC  AgRfSigGen_ConfigurePM ( ViSession Vi, ViConstString Source, ViReal64 Deviation );

    /*- Pulse Modulation */

ViStatus _VI_FUNC  AgRfSigGen_ConfigurePulseModulationEnabled ( ViSession Vi, ViBoolean Enabled );

ViStatus _VI_FUNC  AgRfSigGen_ConfigurePulseModulationSource ( ViSession Vi, ViInt32 Source );

ViStatus _VI_FUNC  AgRfSigGen_ConfigurePulseModulationExternalPolarity ( ViSession Vi, ViInt32 Polarity );

    /*- LF Generator */

ViStatus _VI_FUNC  AgRfSigGen_GetLFGeneratorName ( ViSession Vi, ViInt32 Index, ViInt32 NameBufferSize, ViChar Name[] );

ViStatus _VI_FUNC  AgRfSigGen_SetActiveLFGenerator ( ViSession Vi, ViConstString ActiveLFGenerator );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureLFGeneratorOutput ( ViSession Vi, ViReal64 Amplitude, ViBoolean Enabled );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureLFGenerator ( ViSession Vi, ViReal64 Frequency, ViInt32 Waveform );

    /*- Sweep */

ViStatus _VI_FUNC  AgRfSigGen_ConfigureFrequencySweepTime ( ViSession Vi, ViReal64 Time );

ViStatus _VI_FUNC  AgRfSigGen_ConfigurePowerSweepTime ( ViSession Vi, ViReal64 Time );

ViStatus _VI_FUNC  AgRfSigGen_SelectList ( ViSession Vi, ViConstString Name );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureFrequencySweepStartStop ( ViSession Vi, ViReal64 Start, ViReal64 Stop );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureFrequencySweepCenterSpan ( ViSession Vi, ViReal64 Center, ViReal64 Span );

ViStatus _VI_FUNC  AgRfSigGen_ConfigurePowerSweepStartStop ( ViSession Vi, ViReal64 Start, ViReal64 Stop );

ViStatus _VI_FUNC  AgRfSigGen_ResetFrequencyStep ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureFrequencyStepDwell ( ViSession Vi, ViBoolean SingleStepEnabled, ViReal64 Dwell );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureFrequencyStepStartStop ( ViSession Vi, ViReal64 Start, ViReal64 Stop, ViInt32 Scaling, ViReal64 StepSize );

ViStatus _VI_FUNC  AgRfSigGen_ResetPowerStep ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_ConfigurePowerStepDwell ( ViSession Vi, ViBoolean SingleStepEnabled, ViReal64 Dwell );

ViStatus _VI_FUNC  AgRfSigGen_ConfigurePowerStepStartStop ( ViSession Vi, ViReal64 Start, ViReal64 Stop, ViReal64 StepSize );

ViStatus _VI_FUNC  AgRfSigGen_ResetList ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_CreateFrequencyPowerList ( ViSession Vi, ViConstString Name, ViInt32 Length, ViReal64 Frequency[], ViReal64 Power[] );

ViStatus _VI_FUNC  AgRfSigGen_CreatePowerList ( ViSession Vi, ViConstString Name, ViInt32 Length, ViReal64 Power[] );

ViStatus _VI_FUNC  AgRfSigGen_CreateFrequencyList ( ViSession Vi, ViConstString Name, ViInt32 Length, ViReal64 Frequency[] );

ViStatus _VI_FUNC  AgRfSigGen_ClearAllLists ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureListDwell ( ViSession Vi, ViBoolean SingleStepEnabled, ViReal64 Dwell );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureSweep ( ViSession Vi, ViInt32 Mode, ViInt32 TriggerSource );

    /*- IQ */

ViStatus _VI_FUNC  AgRfSigGen_ConfigureIQEnabled ( ViSession Vi, ViBoolean Enabled );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureIQImpairmentEnabled ( ViSession Vi, ViBoolean Enabled );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureIQImpairment ( ViSession Vi, ViReal64 IOffset, ViReal64 QOffset, ViReal64 Ratio, ViReal64 Skew );

ViStatus _VI_FUNC  AgRfSigGen_CalibrateIQ ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureIQ ( ViSession Vi, ViInt32 Source, ViBoolean SwapEnabled );

    /*- ARB Generator */

ViStatus _VI_FUNC  AgRfSigGen_SelectArbWaveform ( ViSession Vi, ViConstString Name );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureArbTriggerSource ( ViSession Vi, ViInt32 Source );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureArbExternalTriggerSlope ( ViSession Vi, ViInt32 Slope );

ViStatus _VI_FUNC  AgRfSigGen_QueryArbWaveformCapabilities ( ViSession Vi, ViInt32* MaxNumberWaveforms, ViInt32* WaveformQuantum, ViInt32* MinWaveformSize, ViInt32* MaxWaveformSize );

ViStatus _VI_FUNC  AgRfSigGen_ClearAllArbWaveforms ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_WriteArbWaveform ( ViSession Vi, ViConstString Name, ViInt32 NumberOfSamples, ViReal64 IData[], ViReal64 QData[], ViBoolean MoreDataPending );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureArb ( ViSession Vi, ViReal64 ClockFrequency, ViReal64 FilterFrequency );

    /*- CDMA */

ViStatus _VI_FUNC  AgRfSigGen_GetCDMAStandardName ( ViSession Vi, ViInt32 Index, ViInt32 NameBufferSize, ViChar Name[] );

ViStatus _VI_FUNC  AgRfSigGen_SelectCDMAStandard ( ViSession Vi, ViConstString Name );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureCDMAClockSource ( ViSession Vi, ViInt32 Source );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureCDMATriggerSource ( ViSession Vi, ViInt32 Source );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureCDMAExternalTriggerSlope ( ViSession Vi, ViInt32 Slope );

ViStatus _VI_FUNC  AgRfSigGen_GetCDMATestModelName ( ViSession Vi, ViInt32 Index, ViInt32 NameBufferSize, ViChar Name[] );

ViStatus _VI_FUNC  AgRfSigGen_SelectCDMATestModel ( ViSession Vi, ViConstString Name );

    /*- TDMA */

ViStatus _VI_FUNC  AgRfSigGen_GetTDMAStandardName ( ViSession Vi, ViInt32 Index, ViInt32 NameBufferSize, ViChar Name[] );

ViStatus _VI_FUNC  AgRfSigGen_SelectTDMAStandard ( ViSession Vi, ViConstString Name );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureTDMATriggerSource ( ViSession Vi, ViInt32 Source );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureTDMAExternalTriggerSlope ( ViSession Vi, ViInt32 Slope );

ViStatus _VI_FUNC  AgRfSigGen_GetTDMAFrameName ( ViSession Vi, ViInt32 Index, ViInt32 NameBufferSize, ViChar Name[] );

ViStatus _VI_FUNC  AgRfSigGen_SelectTDMAFrame ( ViSession Vi, ViConstString Name );

ViStatus _VI_FUNC  AgRfSigGen_ConfigureTDMAClockSource ( ViSession Vi, ViInt32 Source, ViInt32 Type );

    /*- ALC */

ViStatus _VI_FUNC  AgRfSigGen_ConfigureALC ( ViSession Vi, ViInt32 Source, ViReal64 Bandwidth );

    /*- Pulse Generator */

ViStatus _VI_FUNC  AgRfSigGen_ConfigurePulseDouble ( ViSession Vi, ViBoolean Enabled, ViReal64 Delay );

ViStatus _VI_FUNC  AgRfSigGen_ConfigurePulseOutput ( ViSession Vi, ViInt32 Polarity, ViBoolean Enabled );

ViStatus _VI_FUNC  AgRfSigGen_ConfigurePulseExternalTrigger ( ViSession Vi, ViInt32 Slope, ViReal64 Delay );

ViStatus _VI_FUNC  AgRfSigGen_ConfigurePulse ( ViSession Vi, ViInt32 PulseTriggerSource, ViReal64 PulseWidth, ViBoolean GatingEnabled );

    /*- Reference Oscillator */

ViStatus _VI_FUNC  AgRfSigGen_ConfigureReferenceOscillator ( ViSession Vi, ViInt32 Source, ViReal64 Frequency );

    /*- Source */

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_RemoveAllCustomArmSources ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_DisableAllArmSources ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_RemoveArmSource ( ViSession Vi, ViConstString SourceName );

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_AddArmSource ( ViSession Vi, ViConstString SourceName );

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_ConfigureArmSource ( ViSession Vi, ViConstString SourceName, ViBoolean Enabled, ViInt32 Detection );

    /*- Alarm */

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_RemoveAllCustomArmAlarms ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_RemoveArmAlarm ( ViSession Vi, ViConstString AlarmName );

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_AddArmAlarm ( ViSession Vi, ViConstString AlarmName );

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_DisableAllArmAlarms ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_ConfigureArmAlarm ( ViSession Vi, ViConstString AlarmName, ViBoolean Enabled, ViReal64 TimeSeconds, ViReal64 TimeFraction, ViReal64 Period, ViInt32 RepeatCount );

    /*- Event */

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_RemoveAllCustomEvents ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_RemoveEvent ( ViSession Vi, ViConstString EventName );

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_AddEvent ( ViSession Vi, ViConstString EventName );

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_DisableAllEvents ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_ConfigureEvent ( ViSession Vi, ViConstString EventName, ViInt32 DriveMode, ViConstString Source, ViConstString DestinationPath, ViInt32 Slope );

    /*- Source */

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_RemoveAllCustomTriggerSources ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_RemoveTriggerSource ( ViSession Vi, ViConstString SourceName );

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_AddTriggerSource ( ViSession Vi, ViConstString SourceName );

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_ConfigureTriggerSource ( ViSession Vi, ViConstString SourceName, ViReal64 Delay, ViInt32 Detection );

    /*- Alarm */

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_RemoveAllTriggerAlarms ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_RemoveTriggerAlarm ( ViSession Vi, ViConstString AlarmName );

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_DisableAllTriggerAlarms ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_AddTriggerAlarm ( ViSession Vi, ViConstString AlarmName );

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_ConfigureTriggerAlarm ( ViSession Vi, ViConstString AlarmName, ViReal64 TimeSeconds, ViReal64 TimeFraction, ViReal64 Period, ViInt32 RepeatCount );

    /*- Utility */

ViStatus _VI_FUNC  AgRfSigGen_revision_query ( ViSession Vi, ViChar DriverRev[], ViChar InstrRev[] );

ViStatus _VI_FUNC  AgRfSigGen_error_message ( ViSession Vi, ViStatus ErrorCode, ViChar ErrorMessage[] );

ViStatus _VI_FUNC  AgRfSigGen_AttachToExistingCOMSession ( IUnknown* ExistingIUnknownPtr, ViSession* Vi );

ViStatus _VI_FUNC  AgRfSigGen_GetError ( ViSession Vi, ViStatus* ErrorCode, ViInt32 ErrorDescriptionBufferSize, ViChar ErrorDescription[] );

ViStatus _VI_FUNC  AgRfSigGen_ClearError ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_GetNextCoercionRecord ( ViSession Vi, ViInt32 CoercionRecordBufferSize, ViChar CoercionRecord[] );

ViStatus _VI_FUNC  AgRfSigGen_ResetInterchangeCheck ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_GetNextInterchangeWarning ( ViSession Vi, ViInt32 InterchangeWarningBufferSize, ViChar InterchangeWarning[] );

ViStatus _VI_FUNC  AgRfSigGen_ClearInterchangeWarnings ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_InvalidateAllAttributes ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_UnlockSession ( ViSession Vi, ViBoolean* CallerHasLock );

ViStatus _VI_FUNC  AgRfSigGen_LockSession ( ViSession Vi, ViBoolean* CallerHasLock );

ViStatus _VI_FUNC  AgRfSigGen_error_query ( ViSession Vi, ViInt32* ErrorCode, ViChar ErrorMessage[] );

ViStatus _VI_FUNC  AgRfSigGen_self_test ( ViSession Vi, ViInt16* TestResult, ViChar TestMessage[] );

ViStatus _VI_FUNC  AgRfSigGen_Disable ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_ResetWithDefaults ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_reset ( ViSession Vi );

    /*- Attribute Accessors */

ViStatus _VI_FUNC  AgRfSigGen_GetAttributeViInt32 ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViInt32* AttributeValue );

ViStatus _VI_FUNC  AgRfSigGen_GetAttributeViReal64 ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViReal64* AttributeValue );

ViStatus _VI_FUNC  AgRfSigGen_GetAttributeViBoolean ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViBoolean* AttributeValue );

ViStatus _VI_FUNC  AgRfSigGen_GetAttributeViSession ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViSession* AttributeValue );

ViStatus _VI_FUNC  AgRfSigGen_GetAttributeViString ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViInt32 AttributeValueBufferSize, ViChar AttributeValue[] );

ViStatus _VI_FUNC  AgRfSigGen_SetAttributeViInt32 ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViInt32 AttributeValue );

ViStatus _VI_FUNC  AgRfSigGen_SetAttributeViReal64 ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViReal64 AttributeValue );

ViStatus _VI_FUNC  AgRfSigGen_SetAttributeViBoolean ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViBoolean AttributeValue );

ViStatus _VI_FUNC  AgRfSigGen_SetAttributeViSession ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViSession AttributeValue );

ViStatus _VI_FUNC  AgRfSigGen_SetAttributeViString ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViConstString AttributeValue );

    /*- Action */

ViStatus _VI_FUNC  AgRfSigGen_DisableAllModulation ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_WaitUntilSettled ( ViSession Vi, ViInt32 MaxTimeMilliseconds );

ViStatus _VI_FUNC  AgRfSigGen_IsSettled ( ViSession Vi, ViBoolean* Done );

ViStatus _VI_FUNC  AgRfSigGen_SendSoftwareTrigger ( ViSession Vi );

    /*- Event Log */

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_GetNextEventLogEntry ( ViSession Vi, ViInt32 LogEntryBufferSize, ViChar LogEntry[] );

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_ClearEventLog ( ViSession Vi );

    /*- Time */

ViStatus _VI_FUNC  AgRfSigGen_IviLxiSync_GetSystemTime ( ViSession Vi, ViReal64* TimeSeconds, ViReal64* TimeFractional );

    /*- AnalogModulation */

ViStatus _VI_FUNC  AgRfSigGen_AnalogModulationDisableAll ( ViSession Vi );

    /*- Arb */

ViStatus _VI_FUNC  AgRfSigGen_DigitalModulationArbWriteWaveformToNVMemory ( ViSession Vi, ViConstString Name, ViInt32 IDataBufferSize, ViReal64 IData[], ViInt32* IDataActualSize, ViInt32 QDataBufferSize, ViReal64 QData[], ViInt32* QDataActualSize, ViBoolean MoreDataPending );

    /*- Status */

ViStatus _VI_FUNC  AgRfSigGen_StatusPreset ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_StatusConfigureServiceRequest ( ViSession Vi, ViInt32 Reason );

ViStatus _VI_FUNC  AgRfSigGen_StatusClear ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_StatusGetRegister ( ViSession Vi, ViInt32 Register, ViInt32 SubRegister, ViInt32* retVal );

ViStatus _VI_FUNC  AgRfSigGen_StatusSetRegister ( ViSession Vi, ViInt32 Register, ViInt32 SubRegister, ViInt32 val );

    /*- ServoAlgorithm */

ViStatus _VI_FUNC  AgRfSigGen_PtpServoAlgorithmConfigure ( ViSession Vi, ViReal64 Asymmetry, ViReal64 CoarseFineThreshold, ViReal64 CoarseProportionalConstant, ViReal64 CoarseIntegralConstant, ViReal64 FineProportionalConstant, ViReal64 FineIntegralConstant, ViInt32 OutlierMaxDiscardCount, ViReal64 OutlierThreshold, ViReal64 SteeringThreshold );

    /*- Log */

ViStatus _VI_FUNC  AgRfSigGen_PtpServoAlgorithmLogReadAll ( ViSession Vi, ViInt32 retValBufferSize, ViChar retVal[] );

ViStatus _VI_FUNC  AgRfSigGen_PtpServoAlgorithmLogSetCircularReferenceIndex ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_PtpServoAlgorithmLogClearAll ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_PtpServoAlgorithmLogGetIndexedEntry ( ViSession Vi, ViInt32 Index, ViInt32 retValBufferSize, ViChar retVal[] );

ViStatus _VI_FUNC  AgRfSigGen_PtpServoAlgorithmLogGetNextEntry ( ViSession Vi, ViInt32 retValBufferSize, ViChar retVal[] );

ViStatus _VI_FUNC  AgRfSigGen_PtpServoAlgorithmLogGetStatistics ( ViSession Vi, ViInt32* Samples, ViReal64* MeanOffset, ViReal64* StdDevOffset, ViReal64* MaxOffset, ViReal64* MinOffset );

ViStatus _VI_FUNC  AgRfSigGen_PtpServoAlgorithmLogClearStatistics ( ViSession Vi );

    /*- Time */

ViStatus _VI_FUNC  AgRfSigGen_PtpTimeClearMeasurementTime ( ViSession Vi );

ViStatus _VI_FUNC  AgRfSigGen_PtpTimeGetMeasurementTime ( ViSession Vi, ViReal64* DeltaSeconds, ViReal64* BeginSeconds, ViReal64* BeginFraction, ViReal64* EndSeconds, ViReal64* EndFraction );

ViStatus _VI_FUNC  AgRfSigGen_PtpTimeSetCurrentTime ( ViSession Vi, ViReal64 TimeSeconds, ViReal64 TimeFraction );

ViStatus _VI_FUNC  AgRfSigGen_PtpTimeFetchCurrentTime ( ViSession Vi, ViReal64* TimeSeconds, ViReal64* TimeFraction );

    /*- Marker */

ViStatus _VI_FUNC  AgRfSigGen_PtpTimeMarkerClearMarker ( ViSession Vi, ViInt32 Marker );

ViStatus _VI_FUNC  AgRfSigGen_PtpTimeMarkerGetMarkerTime ( ViSession Vi, ViInt32 Marker, ViReal64* DeltaSeconds, ViReal64* MarkerSeconds, ViReal64* MarkerFraction, ViReal64* CurrentSeconds, ViReal64* CurrentFraction );

ViStatus _VI_FUNC  AgRfSigGen_PtpTimeMarkerSetMarker ( ViSession Vi, ViInt32 Marker );


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



#define AGRFSIGGEN_SUCCESS                                0
#define AGRFSIGGEN_WARN_NSUP_ID_QUERY                     (IVIC_WARN_BASE + 0x65)
#define AGRFSIGGEN_WARN_NSUP_RESET                        (IVIC_WARN_BASE + 0x66)
#define AGRFSIGGEN_WARN_NSUP_SELF_TEST                    (IVIC_WARN_BASE + 0x67)
#define AGRFSIGGEN_WARN_NSUP_ERROR_QUERY                  (IVIC_WARN_BASE + 0x68)
#define AGRFSIGGEN_WARN_NSUP_REV_QUERY                    (IVIC_WARN_BASE + 0x69)
#define AGRFSIGGEN_ERROR_CANNOT_RECOVER                   (IVIC_ERROR_BASE + 0x00)
#define AGRFSIGGEN_ERROR_INSTRUMENT_STATUS                (IVIC_ERROR_BASE + 0x01)
#define AGRFSIGGEN_ERROR_CANNOT_OPEN_FILE                 (IVIC_ERROR_BASE + 0x02)
#define AGRFSIGGEN_ERROR_READING_FILE                     (IVIC_ERROR_BASE + 0x03)
#define AGRFSIGGEN_ERROR_WRITING_FILE                     (IVIC_ERROR_BASE + 0x04)
#define AGRFSIGGEN_ERROR_INVALID_PATHNAME                 (IVIC_ERROR_BASE + 0x0B)
#define AGRFSIGGEN_ERROR_INVALID_VALUE                    (IVIC_ERROR_BASE + 0x10)
#define AGRFSIGGEN_ERROR_FUNCTION_NOT_SUPPORTED           (IVIC_ERROR_BASE + 0x11)
#define AGRFSIGGEN_ERROR_ATTRIBUTE_NOT_SUPPORTED          (IVIC_ERROR_BASE + 0x12)
#define AGRFSIGGEN_ERROR_VALUE_NOT_SUPPORTED              (IVIC_ERROR_BASE + 0x13)
#define AGRFSIGGEN_ERROR_NOT_INITIALIZED                  (IVIC_ERROR_BASE + 0x1D)
#define AGRFSIGGEN_ERROR_UNKNOWN_CHANNEL_NAME             (IVIC_ERROR_BASE + 0x20)
#define AGRFSIGGEN_ERROR_TOO_MANY_OPEN_FILES              (IVIC_ERROR_BASE + 0x23)
#define AGRFSIGGEN_ERROR_CHANNEL_NAME_REQUIRED            (IVIC_ERROR_BASE + 0x44)
#define AGRFSIGGEN_ERROR_MISSING_OPTION_NAME              (IVIC_ERROR_BASE + 0x49)
#define AGRFSIGGEN_ERROR_MISSING_OPTION_VALUE             (IVIC_ERROR_BASE + 0x4A)
#define AGRFSIGGEN_ERROR_BAD_OPTION_NAME                  (IVIC_ERROR_BASE + 0x4B)
#define AGRFSIGGEN_ERROR_BAD_OPTION_VALUE                 (IVIC_ERROR_BASE + 0x4C)
#define AGRFSIGGEN_ERROR_OUT_OF_MEMORY                    (IVIC_ERROR_BASE + 0x56)
#define AGRFSIGGEN_ERROR_OPERATION_PENDING                (IVIC_ERROR_BASE + 0x57)
#define AGRFSIGGEN_ERROR_NULL_POINTER                     (IVIC_ERROR_BASE + 0x58)
#define AGRFSIGGEN_ERROR_UNEXPECTED_RESPONSE              (IVIC_ERROR_BASE + 0x59)
#define AGRFSIGGEN_ERROR_FILE_NOT_FOUND                   (IVIC_ERROR_BASE + 0x5B)
#define AGRFSIGGEN_ERROR_INVALID_FILE_FORMAT              (IVIC_ERROR_BASE + 0x5C)
#define AGRFSIGGEN_ERROR_STATUS_NOT_AVAILABLE             (IVIC_ERROR_BASE + 0x5D)
#define AGRFSIGGEN_ERROR_ID_QUERY_FAILED                  (IVIC_ERROR_BASE + 0x5E)
#define AGRFSIGGEN_ERROR_RESET_FAILED                     (IVIC_ERROR_BASE + 0x5F)
#define AGRFSIGGEN_ERROR_RESOURCE_UNKNOWN                 (IVIC_ERROR_BASE + 0x60)
#define AGRFSIGGEN_ERROR_ALREADY_INITIALIZED              (IVIC_ERROR_BASE + 0x61)
#define AGRFSIGGEN_ERROR_CANNOT_CHANGE_SIMULATION_STATE   (IVIC_ERROR_BASE + 0x62)
#define AGRFSIGGEN_ERROR_INVALID_NUMBER_OF_LEVELS_IN_SELECTOR (IVIC_ERROR_BASE + 0x63)
#define AGRFSIGGEN_ERROR_INVALID_RANGE_IN_SELECTOR        (IVIC_ERROR_BASE + 0x64)
#define AGRFSIGGEN_ERROR_UNKOWN_NAME_IN_SELECTOR          (IVIC_ERROR_BASE + 0x65)
#define AGRFSIGGEN_ERROR_BADLY_FORMED_SELECTOR            (IVIC_ERROR_BASE + 0x66)
#define AGRFSIGGEN_ERROR_UNKNOWN_PHYSICAL_IDENTIFIER      (IVIC_ERROR_BASE + 0x67)
#define AGRFSIGGEN_IVIRFSIGGEN_ERROR_TRIGGER_NOT_SOFTWARE (IVIC_CLASS_ERROR_BASE + 0x0001L)
#define AGRFSIGGEN_IVIRFSIGGEN_ERROR_MAX_TIME_EXCEEDED    (IVIC_CLASS_ERROR_BASE + 0x0003L)
#define AGRFSIGGEN_IVIRFSIGGEN_ERROR_LIST_UNKNOWN         (IVIC_CLASS_ERROR_BASE + 0x000AL)
#define AGRFSIGGEN_IVILXISYNC_ERROR_ALARM_TIME_INVALID    (IVIC_LXISYNC_ERROR_BASE + 1L)
#define AGRFSIGGEN_IVILXISYNC_ERROR_EVENT_SOURCE_EXISTS   (IVIC_LXISYNC_ERROR_BASE + 2L)
#define AGRFSIGGEN_IVILXISYNC_ERROR_OUT_OF_EVENT_RESOURCES (IVIC_LXISYNC_ERROR_BASE + 3L)
#define AGRFSIGGEN_IVILXISYNC_ERROR_EVENT_SOURCE_DOES_NOT_EXIST (IVIC_LXISYNC_ERROR_BASE + 4L)
#define AGRFSIGGEN_IVILXISYNC_ERROR_EVENT_SOURCE_NOT_SET  (IVIC_LXISYNC_ERROR_BASE + 5L)
#define AGRFSIGGEN_IVILXISYNC_ERROR_INVALID_EVENT_SOURCE  (IVIC_LXISYNC_ERROR_BASE + 6L)
#define AGRFSIGGEN_IVILXISYNC_ERROR_ALARM_EXISTS          (IVIC_LXISYNC_ERROR_BASE + 7L)
#define AGRFSIGGEN_IVILXISYNC_ERROR_ALARM_DOES_NOT_EXIST  (IVIC_LXISYNC_ERROR_BASE + 8L)
#define AGRFSIGGEN_IVILXISYNC_ERROR_WIRED_OR_MODE_INVALID (IVIC_LXISYNC_ERROR_BASE + 9L)
#define AGRFSIGGEN_IVILXISYNC_ERROR_CANT_REMOVE_RESERVED_REPEATED_CAPABILITY (IVIC_LXISYNC_ERROR_BASE + 10L)


#define AGRFSIGGEN_ERROR_PERSONALITY_NOT_ACTIVE           (IVIC_SPECIFIC_ERROR_BASE + 529L)
#define AGRFSIGGEN_ERROR_PERSONALITY_NOT_INSTALLED        (IVIC_SPECIFIC_ERROR_BASE + 530L)
#define AGRFSIGGEN_ERROR_PERSONALITY_NOT_LICENSED         (IVIC_SPECIFIC_ERROR_BASE + 531L)
#define AGRFSIGGEN_ERROR_IO_GENERAL                       (IVIC_SPECIFIC_ERROR_BASE + 532L)
#define AGRFSIGGEN_ERROR_IO_TIMEOUT                       (IVIC_SPECIFIC_ERROR_BASE + 533L)
#define AGRFSIGGEN_ERROR_MODEL_NOT_SUPPORTED              (IVIC_SPECIFIC_ERROR_BASE + 534L)
#define AGRFSIGGEN_ERROR_WRAPPED_DRIVER_ERROR             (IVIC_SPECIFIC_ERROR_BASE + 536L)
#define AGRFSIGGEN_ERROR_METHOD_NOT_SUPPORTED             (IVIC_SPECIFIC_ERROR_BASE + 1L)
#define AGRFSIGGEN_ERROR_PROPERTY_NOT_SUPPORTED           (IVIC_SPECIFIC_ERROR_BASE + 2L)
#define AGRFSIGGEN_ERROR_MAX_TIME_EXCEEDED                (IVIC_SPECIFIC_ERROR_BASE + 3L)
#define AGRFSIGGEN_ERROR_LIST_UNKNOWN                     (IVIC_SPECIFIC_ERROR_BASE + 4L)
#define AGRFSIGGEN_ERROR_INVALID_SESSION_HANDLE           (IVIC_SPECIFIC_ERROR_BASE + 5L)
#define AGRFSIGGEN_ERROR_ALARM_TIME_INVALID               (IVIC_SPECIFIC_ERROR_BASE + 6L)
#define AGRFSIGGEN_ERROR_EVENT_SOURCE_EXISTS              (IVIC_SPECIFIC_ERROR_BASE + 7L)
#define AGRFSIGGEN_ERROR_OUT_OF_EVENT_RESOURCES           (IVIC_SPECIFIC_ERROR_BASE + 8L)
#define AGRFSIGGEN_ERROR_EVENT_SOURCE_DOES_NOT_EXIST      (IVIC_SPECIFIC_ERROR_BASE + 9L)
#define AGRFSIGGEN_ERROR_EVENT_SOURCE_NOT_SET             (IVIC_SPECIFIC_ERROR_BASE + 10L)
#define AGRFSIGGEN_ERROR_INVALID_EVENT_SOURCE             (IVIC_SPECIFIC_ERROR_BASE + 11L)
#define AGRFSIGGEN_ERROR_ALARM_EXISTS                     (IVIC_SPECIFIC_ERROR_BASE + 12L)
#define AGRFSIGGEN_ERROR_ALARM_DOES_NOT_EXIST             (IVIC_SPECIFIC_ERROR_BASE + 13L)
#define AGRFSIGGEN_ERROR_WIRED_OR_MODE_INVALID            (IVIC_SPECIFIC_ERROR_BASE + 14L)
#define AGRFSIGGEN_ERROR_CANT_REMOVE_RESERVED_REPEATED_CAPABILITY (IVIC_SPECIFIC_ERROR_BASE + 15L)
#define AGRFSIGGEN_ERROR_INVALID_SOURCE                   (IVIC_SPECIFIC_ERROR_BASE + 16L)
#define AGRFSIGGEN_ERROR_INVALID_ALARM                    (IVIC_SPECIFIC_ERROR_BASE + 17L)
#define AGRFSIGGEN_ERROR_OUT_OF_TRIGGER_SOURCE_RESOURCES  (IVIC_SPECIFIC_ERROR_BASE + 18L)
#define AGRFSIGGEN_ERROR_OUT_OF_TRIGGER_ALARM_RESOURCES   (IVIC_SPECIFIC_ERROR_BASE + 19L)
#define AGRFSIGGEN_ERROR_SOURCE_EXISTS                    (IVIC_SPECIFIC_ERROR_BASE + 20L)
#define AGRFSIGGEN_ERROR_APP_NOT_INSTALLED                (IVIC_SPECIFIC_ERROR_BASE + 21L)
#define AGRFSIGGEN_ERROR_SOURCE_DOES_NOT_EXISTS           (IVIC_SPECIFIC_ERROR_BASE + 22L)
#define AGRFSIGGEN_ERROR_CUSTOM_EVENT_SOURCE_NOT_EXISTS   (IVIC_SPECIFIC_ERROR_BASE + 23L)




/**************************************************************************** 
 *---------------------------- End Include File ----------------------------* 
 ****************************************************************************/
#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif
#endif // __AGRFSIGGEN_HEADER
