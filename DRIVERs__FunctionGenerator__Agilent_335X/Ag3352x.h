/******************************************************************************
 *                                                                         
 * Copyright 2010-2012 Agilent Technologies, Inc.
 *
 *****************************************************************************/

#ifndef __AG3352X_HEADER
#define __AG3352X_HEADER

#include <IviVisaType.h>

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
#define IVI_INHERENT_ATTR_BASE        (IVI_ATTR_BASE +  50000)   /* base for inherent capability attributes */
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

#define AG3352X_ATTR_SPECIFIC_DRIVER_DESCRIPTION              (IVI_INHERENT_ATTR_BASE + 514L)  /* ViString, read-only */
#define AG3352X_ATTR_SPECIFIC_DRIVER_PREFIX                   (IVI_INHERENT_ATTR_BASE + 302L)  /* ViString, read-only */
#define AG3352X_ATTR_SPECIFIC_DRIVER_VENDOR                   (IVI_INHERENT_ATTR_BASE + 513L)  /* ViString, read-only */
#define AG3352X_ATTR_SPECIFIC_DRIVER_REVISION                 (IVI_INHERENT_ATTR_BASE + 551L)  /* ViString, read-only */
#define AG3352X_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION (IVI_INHERENT_ATTR_BASE + 515L)  /* ViInt32, read-only */
#define AG3352X_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION (IVI_INHERENT_ATTR_BASE + 516L)  /* ViInt32, read-only */

/*- User Options */

#define AG3352X_ATTR_RANGE_CHECK                            (IVI_INHERENT_ATTR_BASE + 2L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_QUERY_INSTRUMENT_STATUS                (IVI_INHERENT_ATTR_BASE + 3L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_CACHE                                  (IVI_INHERENT_ATTR_BASE + 4L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_SIMULATE                               (IVI_INHERENT_ATTR_BASE + 5L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_RECORD_COERCIONS                       (IVI_INHERENT_ATTR_BASE + 6L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_INTERCHANGE_CHECK                      (IVI_INHERENT_ATTR_BASE + 21L)  /* ViBoolean, read-write */

/*- Advanced Session Information */

#define AG3352X_ATTR_LOGICAL_NAME                           (IVI_INHERENT_ATTR_BASE + 305L)  /* ViString, read-only */
#define AG3352X_ATTR_IO_RESOURCE_DESCRIPTOR                 (IVI_INHERENT_ATTR_BASE + 304L)  /* ViString, read-only */
#define AG3352X_ATTR_DRIVER_SETUP                           (IVI_INHERENT_ATTR_BASE + 7L)  /* ViString, read-only */

/*- Driver Capabilities */

#define AG3352X_ATTR_GROUP_CAPABILITIES                     (IVI_INHERENT_ATTR_BASE + 401L)  /* ViString, read-only */
#define AG3352X_ATTR_SUPPORTED_INSTRUMENT_MODELS            (IVI_INHERENT_ATTR_BASE + 327L)  /* ViString, read-only */

/*- Instrument Identification */

#define AG3352X_ATTR_INSTRUMENT_FIRMWARE_REVISION           (IVI_INHERENT_ATTR_BASE + 510L)  /* ViString, read-only */
#define AG3352X_ATTR_INSTRUMENT_MANUFACTURER                (IVI_INHERENT_ATTR_BASE + 511L)  /* ViString, read-only */
#define AG3352X_ATTR_INSTRUMENT_MODEL                       (IVI_INHERENT_ATTR_BASE + 512L)  /* ViString, read-only */


/*===== Instrument-Specific Attributes =====================================*/

/*- Arbitrary */

#define AG3352X_ATTR_ARB_GAIN                               (IVI_CLASS_ATTR_BASE + 202L)  /* ViReal64, read-write */
#define AG3352X_ATTR_ARB_OFFSET                             (IVI_CLASS_ATTR_BASE + 203L)  /* ViReal64, read-write */
#define AG3352X_ATTR_ARB_SAMPLE_RATE                        (IVI_CLASS_ATTR_BASE + 204L)  /* ViReal64, read-write */

/*- Waveform */

#define AG3352X_ATTR_ARB_FREQUENCY                          (IVI_CLASS_ATTR_BASE + 209L)  /* ViReal64, read-write */
#define AG3352X_ATTR_MAX_NUM_WAVEFORMS                      (IVI_CLASS_ATTR_BASE + 205L)  /* ViInt32, read-only */
#define AG3352X_ATTR_WAVEFORM_QUANTUM                       (IVI_CLASS_ATTR_BASE + 206L)  /* ViInt32, read-only */
#define AG3352X_ATTR_MAX_WAVEFORM_SIZE                      (IVI_CLASS_ATTR_BASE + 208L)  /* ViInt32, read-only */
#define AG3352X_ATTR_MIN_WAVEFORM_SIZE                      (IVI_CLASS_ATTR_BASE + 207L)  /* ViInt32, read-only */

/*- Sequence */

#define AG3352X_ATTR_MAX_SEQUENCE_LENGTH                    (IVI_CLASS_ATTR_BASE + 214L)  /* ViInt32, read-only */
#define AG3352X_ATTR_MIN_SEQUENCE_LENGTH                    (IVI_CLASS_ATTR_BASE + 213L)  /* ViInt32, read-only */
#define AG3352X_ATTR_MAX_LOOP_COUNT                         (IVI_CLASS_ATTR_BASE + 215L)  /* ViInt32, read-only */
#define AG3352X_ATTR_MAX_NUM_SEQUENCES                      (IVI_CLASS_ATTR_BASE + 212L)  /* ViInt32, read-only */

/*- AM */

#define AG3352X_ATTR_AM_ENABLED                             (IVI_CLASS_ATTR_BASE + 401L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_AM_INTERNAL_DEPTH                      (IVI_CLASS_ATTR_BASE + 403L)  /* ViReal64, read-write */
#define AG3352X_ATTR_AM_INTERNAL_FREQUENCY                  (IVI_CLASS_ATTR_BASE + 405L)  /* ViReal64, read-write */
#define AG3352X_ATTR_AM_INTERNAL_WAVEFORM                   (IVI_CLASS_ATTR_BASE + 404L)  /* ViInt32, read-write */
#define AG3352X_ATTR_AM_SOURCE                              (IVI_CLASS_ATTR_BASE + 402L)  /* ViInt32, read-write */

/*- FM */

#define AG3352X_ATTR_FM_ENABLED                             (IVI_CLASS_ATTR_BASE + 501L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_FM_INTERNAL_DEVIATION                  (IVI_CLASS_ATTR_BASE + 503L)  /* ViReal64, read-write */
#define AG3352X_ATTR_FM_INTERNAL_FREQUENCY                  (IVI_CLASS_ATTR_BASE + 505L)  /* ViReal64, read-write */
#define AG3352X_ATTR_FM_INTERNAL_WAVEFORM                   (IVI_CLASS_ATTR_BASE + 504L)  /* ViInt32, read-write */
#define AG3352X_ATTR_FM_SOURCE                              (IVI_CLASS_ATTR_BASE + 502L)  /* ViInt32, read-write */

/*- Output */

#define AG3352X_ATTR_CHANNEL_COUNT                          (IVI_INHERENT_ATTR_BASE + 203L)  /* ViInt32, read-only */
#define AG3352X_ATTR_OUTPUT_ENABLED                         (IVI_CLASS_ATTR_BASE + 3L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_OUTPUT_IMPEDANCE                       (IVI_CLASS_ATTR_BASE + 4L)  /* ViReal64, read-write */
#define AG3352X_ATTR_OPERATION_MODE                         (IVI_CLASS_ATTR_BASE + 5L)  /* ViInt32, read-write */
#define AG3352X_ATTR_OUTPUT_MODE                            (IVI_CLASS_ATTR_BASE + 1L)  /* ViInt32, read-write */
#define AG3352X_ATTR_REF_CLOCK_SOURCE                       (IVI_CLASS_ATTR_BASE + 2L)  /* ViInt32, read-write */

/*- StandardWaveform */

#define AG3352X_ATTR_FUNC_AMPLITUDE                         (IVI_CLASS_ATTR_BASE + 102L)  /* ViReal64, read-write */
#define AG3352X_ATTR_FUNC_DC_OFFSET                         (IVI_CLASS_ATTR_BASE + 103L)  /* ViReal64, read-write */
#define AG3352X_ATTR_FUNC_DUTY_CYCLE_HIGH                   (IVI_CLASS_ATTR_BASE + 106L)  /* ViReal64, read-write */
#define AG3352X_ATTR_FUNC_FREQUENCY                         (IVI_CLASS_ATTR_BASE + 104L)  /* ViReal64, read-write */
#define AG3352X_ATTR_FUNC_START_PHASE                       (IVI_CLASS_ATTR_BASE + 105L)  /* ViReal64, read-write */
#define AG3352X_ATTR_FUNC_WAVEFORM                          (IVI_CLASS_ATTR_BASE + 101L)  /* ViInt32, read-write */

/*- Trigger */

#define AG3352X_ATTR_BURST_COUNT                            (IVI_CLASS_ATTR_BASE + 350L)  /* ViInt32, read-write */
#define AG3352X_ATTR_INTERNAL_TRIGGER_RATE                  (IVI_CLASS_ATTR_BASE + 310L)  /* ViReal64, read-write */
#define AG3352X_ATTR_TRIGGER_SOURCE                         (IVI_CLASS_ATTR_BASE + 302L)  /* ViInt32, read-write */

/*- Channel */

#define AG3352X_ATTR_CHANNEL_TRACKING                       (IVI_SPECIFIC_ATTR_BASE + 43L)  /* ViInt32, write-only */

/*- Output */

#define AG3352X_ATTR_CHANNEL_OUTPUT_ENABLED                 (IVI_SPECIFIC_ATTR_BASE + 1L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUT_LOAD                    (IVI_SPECIFIC_ATTR_BASE + 2L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUT_MODE                    (IVI_SPECIFIC_ATTR_BASE + 3L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUT_POLARITY                (IVI_SPECIFIC_ATTR_BASE + 4L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUT_QUERY_LOAD_MIN          (IVI_SPECIFIC_ATTR_BASE + 223L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUT_QUERY_LOAD_MAX          (IVI_SPECIFIC_ATTR_BASE + 224L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUT_FREQUENCY               (IVI_SPECIFIC_ATTR_BASE + 72L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUT_FREQUENCY_MAX           (IVI_SPECIFIC_ATTR_BASE + 77L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUT_FREQUENCY_MIN           (IVI_SPECIFIC_ATTR_BASE + 79L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUT_FREQUENCY_MODE          (IVI_SPECIFIC_ATTR_BASE + 81L)  /* ViInt32, read-write */

/*- Function */

#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_FUNCTION        (IVI_SPECIFIC_ATTR_BASE + 53L)  /* ViInt32, read-write */

/*- List */

#define AG3352X_ATTR_CHANNEL_OUTPUT_LIST_FREQUENCY_POINTS     (IVI_SPECIFIC_ATTR_BASE + 89L)  /* ViInt32, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUT_LIST_FREQUENCY_POINTS_MAX (IVI_SPECIFIC_ATTR_BASE + 255L)  /* ViInt32, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUT_LIST_FREQUENCY_POINTS_MIN (IVI_SPECIFIC_ATTR_BASE + 256L)  /* ViInt32, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUT_LIST_DWELL_TIME           (IVI_SPECIFIC_ATTR_BASE + 82L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUT_LIST_DWELL_TIME_MAX       (IVI_SPECIFIC_ATTR_BASE + 88L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUT_LIST_DWELL_TIME_MIN       (IVI_SPECIFIC_ATTR_BASE + 90L)  /* ViReal64, read-only */

/*- Voltage */

#define AG3352X_ATTR_CHANNEL_OUTPUT_VOLTAGE_AMPLITUDE            (IVI_SPECIFIC_ATTR_BASE + 128L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUT_VOLTAGE_HIGH                 (IVI_SPECIFIC_ATTR_BASE + 130L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUT_VOLTAGE_LOW                  (IVI_SPECIFIC_ATTR_BASE + 135L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUT_VOLTAGE_AUTO_RANGING_ENABLED (IVI_SPECIFIC_ATTR_BASE + 139L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUT_VOLTAGE_UNITS                (IVI_SPECIFIC_ATTR_BASE + 140L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUT_VOLTAGE_HIGH_MAX             (IVI_SPECIFIC_ATTR_BASE + 257L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUT_VOLTAGE_HIGH_MIN             (IVI_SPECIFIC_ATTR_BASE + 258L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUT_VOLTAGE_LOW_MAX              (IVI_SPECIFIC_ATTR_BASE + 259L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUT_VOLTAGE_LOW_MIN              (IVI_SPECIFIC_ATTR_BASE + 260L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUT_VOLTAGE_AMPLITUDE_MAX        (IVI_SPECIFIC_ATTR_BASE + 261L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUT_VOLTAGE_AMPLITUDE_MIN        (IVI_SPECIFIC_ATTR_BASE + 262L)  /* ViReal64, read-only */
#define AG3352X_ATTR_VOLTAGE_COUPLING_ENABLED                    (IVI_SPECIFIC_ATTR_BASE + 333L)  /* ViBoolean, read-write */

/*- Limit */

#define AG3352X_ATTR_CHANNEL_OUTPUT_VOLTAGE_LIMIT_HIGH      (IVI_SPECIFIC_ATTR_BASE + 131L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUT_VOLTAGE_LIMIT_LOW       (IVI_SPECIFIC_ATTR_BASE + 132L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUT_VOLTAGE_LIMIT_ENABLED   (IVI_SPECIFIC_ATTR_BASE + 133L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUT_VOLTAGE_LIMIT_HIGH_MAX  (IVI_SPECIFIC_ATTR_BASE + 263L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUT_VOLTAGE_LIMIT_HIGH_MIN  (IVI_SPECIFIC_ATTR_BASE + 264L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUT_VOLTAGE_LIMIT_LOW_MAX   (IVI_SPECIFIC_ATTR_BASE + 265L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUT_VOLTAGE_LIMIT_LOW_MIN   (IVI_SPECIFIC_ATTR_BASE + 266L)  /* ViReal64, read-only */

/*- Offset */

#define AG3352X_ATTR_CHANNEL_OUTPUT_VOLTAGE_OFFSET_DCOFFSET     (IVI_SPECIFIC_ATTR_BASE + 136L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUT_VOLTAGE_OFFSET_DCOFFSET_MAX (IVI_SPECIFIC_ATTR_BASE + 267L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUT_VOLTAGE_OFFSET_DCOFFSET_MIN (IVI_SPECIFIC_ATTR_BASE + 268L)  /* ViReal64, read-only */

/*- FrequencyCoupling */

#define AG3352X_ATTR_CHANNEL_OUTPUT_FREQUENCY_COUPLING_MODE    (IVI_SPECIFIC_ATTR_BASE + 74L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUT_FREQUENCY_COUPLING_OFFSET  (IVI_SPECIFIC_ATTR_BASE + 75L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUT_FREQUENCY_COUPLING_RATIO   (IVI_SPECIFIC_ATTR_BASE + 76L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUT_FREQUENCY_COUPLING_ENABLED (IVI_SPECIFIC_ATTR_BASE + 78L)  /* ViBoolean, read-write */

/*- Sync */

#define AG3352X_ATTR_CHANNEL_OUTPUT_SYNC_ENABLED            (IVI_SPECIFIC_ATTR_BASE + 5L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUT_SYNC_MODE               (IVI_SPECIFIC_ATTR_BASE + 6L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUT_SYNC_POLARITY           (IVI_SPECIFIC_ATTR_BASE + 22L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUT_SYNC_SOURCE             (IVI_SPECIFIC_ATTR_BASE + 23L)  /* ViInt32, read-write */

/*- Trigger */

#define AG3352X_ATTR_CHANNEL_OUTPUT_TRIGGER_ENABLED         (IVI_SPECIFIC_ATTR_BASE + 26L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUT_TRIGGER_SLOPE           (IVI_SPECIFIC_ATTR_BASE + 27L)  /* ViInt32, read-write */

/*- Trigger */

#define AG3352X_ATTR_CHANNEL_TRIGGER_COUNT                       (IVI_SPECIFIC_ATTR_BASE + 7L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_TRIGGER_SOURCE                      (IVI_SPECIFIC_ATTR_BASE + 9L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_TRIGGER_DELAY                       (IVI_SPECIFIC_ATTR_BASE + 127L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_TRIGGER_SLOPE                       (IVI_SPECIFIC_ATTR_BASE + 8L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_TRIGGER_INITIATE_CONTINUOUS_ENABLED (IVI_SPECIFIC_ATTR_BASE + 12L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_CHANNEL_TRIGGER_TIMER                       (IVI_SPECIFIC_ATTR_BASE + 318L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_TRIGGER_TIMER_MAX                   (IVI_SPECIFIC_ATTR_BASE + 319L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_TRIGGER_TIMER_MIN                   (IVI_SPECIFIC_ATTR_BASE + 320L)  /* ViReal64, read-only */

/*- AM */

#define AG3352X_ATTR_CHANNEL_AM_ENABLED                     (IVI_SPECIFIC_ATTR_BASE + 30L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_CHANNEL_AM_SOURCE                      (IVI_SPECIFIC_ATTR_BASE + 31L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_AM_INTERNAL_DEPTH              (IVI_SPECIFIC_ATTR_BASE + 32L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_AM_INTERNAL_FREQUENCY          (IVI_SPECIFIC_ATTR_BASE + 33L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_AM_INTERNAL_FUNCTION           (IVI_SPECIFIC_ATTR_BASE + 34L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_AM_MODE                        (IVI_SPECIFIC_ATTR_BASE + 46L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_AM_DEPTH_MAX                   (IVI_SPECIFIC_ATTR_BASE + 93L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_AM_DEPTH_MIN                   (IVI_SPECIFIC_ATTR_BASE + 184L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_AM_INTERNAL_FREQUENCY_MAX      (IVI_SPECIFIC_ATTR_BASE + 187L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_AM_INTERNAL_FREQUENCY_MIN      (IVI_SPECIFIC_ATTR_BASE + 188L)  /* ViReal64, read-only */

/*- FM */

#define AG3352X_ATTR_CHANNEL_FM_ENABLED                     (IVI_SPECIFIC_ATTR_BASE + 35L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_CHANNEL_FM_SOURCE                      (IVI_SPECIFIC_ATTR_BASE + 36L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_FM_INTERNAL_DEVIATION          (IVI_SPECIFIC_ATTR_BASE + 37L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_FM_INTERNAL_FREQUENCY          (IVI_SPECIFIC_ATTR_BASE + 38L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_FM_INTERNAL_FUNCTION           (IVI_SPECIFIC_ATTR_BASE + 39L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_FM_DEVIATION_MAX               (IVI_SPECIFIC_ATTR_BASE + 213L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_FM_DEVIATION_MIN               (IVI_SPECIFIC_ATTR_BASE + 214L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_FM_INTERNAL_FREQUENCY_MAX      (IVI_SPECIFIC_ATTR_BASE + 215L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_FM_INTERNAL_FREQUENCY_MIN      (IVI_SPECIFIC_ATTR_BASE + 216L)  /* ViReal64, read-only */

/*- BPSK */

#define AG3352X_ATTR_CHANNEL_BPSK_PHASE                     (IVI_SPECIFIC_ATTR_BASE + 55L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_BPSK_INTERNAL_RATE             (IVI_SPECIFIC_ATTR_BASE + 56L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_BPSK_SOURCE                    (IVI_SPECIFIC_ATTR_BASE + 57L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_BPSK_ENABLED                   (IVI_SPECIFIC_ATTR_BASE + 58L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_CHANNEL_BPSK_INTERNAL_RATE_MAX         (IVI_SPECIFIC_ATTR_BASE + 203L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_BPSK_INTERNAL_RATE_MIN         (IVI_SPECIFIC_ATTR_BASE + 204L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_BPSK_PHASE_MAX                 (IVI_SPECIFIC_ATTR_BASE + 205L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_BPSK_PHASE_MIN                 (IVI_SPECIFIC_ATTR_BASE + 206L)  /* ViReal64, read-only */

/*- Burst */

#define AG3352X_ATTR_CHANNEL_BURST_PHASE                    (IVI_SPECIFIC_ATTR_BASE + 59L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_BURST_GATE_POLARITY            (IVI_SPECIFIC_ATTR_BASE + 60L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_BURST_INTERNAL_PERIOD          (IVI_SPECIFIC_ATTR_BASE + 61L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_BURST_MODE                     (IVI_SPECIFIC_ATTR_BASE + 62L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_BURST_NUMBER_OF_CYCLES         (IVI_SPECIFIC_ATTR_BASE + 63L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_BURST_ENABLED                  (IVI_SPECIFIC_ATTR_BASE + 64L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_CHANNEL_BURST_PHASE_UNITS              (IVI_SPECIFIC_ATTR_BASE + 11L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_BURST_INTERNAL_PERIOD_MAX      (IVI_SPECIFIC_ATTR_BASE + 207L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_BURST_INTERNAL_PERIOD_MIN      (IVI_SPECIFIC_ATTR_BASE + 208L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_BURST_NUMBER_OF_CYCLES_MAX     (IVI_SPECIFIC_ATTR_BASE + 209L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_BURST_NUMBER_OF_CYCLES_MIN     (IVI_SPECIFIC_ATTR_BASE + 210L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_BURST_PHASE_MAX                (IVI_SPECIFIC_ATTR_BASE + 211L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_BURST_PHASE_MIN                (IVI_SPECIFIC_ATTR_BASE + 212L)  /* ViReal64, read-only */

/*- FSK */

#define AG3352X_ATTR_CHANNEL_FSK_FREQUENCY                  (IVI_SPECIFIC_ATTR_BASE + 83L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_FSK_INTERNAL_RATE              (IVI_SPECIFIC_ATTR_BASE + 84L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_FSK_SOURCE                     (IVI_SPECIFIC_ATTR_BASE + 85L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_FSK_ENABLED                    (IVI_SPECIFIC_ATTR_BASE + 86L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_CHANNEL_FSK_FREQUENCY_MAX              (IVI_SPECIFIC_ATTR_BASE + 219L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_FSK_FREQUENCY_MIN              (IVI_SPECIFIC_ATTR_BASE + 220L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_FSK_INTERNAL_RATE_MAX          (IVI_SPECIFIC_ATTR_BASE + 221L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_FSK_INTERNAL_RATE_MIN          (IVI_SPECIFIC_ATTR_BASE + 222L)  /* ViReal64, read-only */

/*- Marker */

#define AG3352X_ATTR_CHANNEL_MARKER_FREQUENCY               (IVI_SPECIFIC_ATTR_BASE + 92L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_MARKER_FREQUENCY_MAX           (IVI_SPECIFIC_ATTR_BASE + 309L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_MARKER_FREQUENCY_MIN           (IVI_SPECIFIC_ATTR_BASE + 310L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_MARKER_POINT_MAX               (IVI_SPECIFIC_ATTR_BASE + 311L)  /* ViInt32, read-only */
#define AG3352X_ATTR_CHANNEL_MARKER_POINT_MIN               (IVI_SPECIFIC_ATTR_BASE + 312L)  /* ViInt32, read-only */
#define AG3352X_ATTR_CHANNEL_MARKER_POINT                   (IVI_SPECIFIC_ATTR_BASE + 126L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_MARKER_CYCLE                   (IVI_SPECIFIC_ATTR_BASE + 102L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_MARKER_CYCLE_MAX               (IVI_SPECIFIC_ATTR_BASE + 129L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_MARKER_CYCLE_MIN               (IVI_SPECIFIC_ATTR_BASE + 137L)  /* ViReal64, read-only */

/*- PhaseLock */

#define AG3352X_ATTR_CHANNEL_PHASELOCK_PHASE                (IVI_SPECIFIC_ATTR_BASE + 94L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_PHASELOCK_UNLOCK_ERROR_ENABLED (IVI_SPECIFIC_ATTR_BASE + 95L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_CHANNEL_PHASELOCK_PHASE_UNITS          (IVI_SPECIFIC_ATTR_BASE + 15L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_PHASELOCK_PHASE_MAX            (IVI_SPECIFIC_ATTR_BASE + 269L)  /* ViInt32, read-only */
#define AG3352X_ATTR_CHANNEL_PHASELOCK_PHASE_MIN            (IVI_SPECIFIC_ATTR_BASE + 270L)  /* ViInt32, read-only */

/*- ReferenceOscillator */

#define AG3352X_ATTR_CHANNEL_PHASELOCK_REFERENCE_OSCILLATOR_SOURCE         (IVI_SPECIFIC_ATTR_BASE + 112L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_PHASELOCK_REFERENCE_OSCILLATOR_AUTO_ENABLED   (IVI_SPECIFIC_ATTR_BASE + 113L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_CHANNEL_PHASELOCK_REFERENCE_OSCILLATOR_CURRENT_SOURCE (IVI_SPECIFIC_ATTR_BASE + 114L)  /* ViInt32, read-only */

/*- PM */

#define AG3352X_ATTR_CHANNEL_PM_DEVIATION                   (IVI_SPECIFIC_ATTR_BASE + 96L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_PM_ENABLED                     (IVI_SPECIFIC_ATTR_BASE + 97L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_CHANNEL_PM_INTERNAL_FREQUENCY          (IVI_SPECIFIC_ATTR_BASE + 98L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_PM_INTERNAL_FUNCTION           (IVI_SPECIFIC_ATTR_BASE + 99L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_PM_SOURCE                      (IVI_SPECIFIC_ATTR_BASE + 100L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_PM_DEVIATION_MAX               (IVI_SPECIFIC_ATTR_BASE + 271L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_PM_DEVIATION_MIN               (IVI_SPECIFIC_ATTR_BASE + 272L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_PM_INTERNAL_FREQUENCY_MAX      (IVI_SPECIFIC_ATTR_BASE + 273L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_PM_INTERNAL_FREQUENCY_MIN      (IVI_SPECIFIC_ATTR_BASE + 274L)  /* ViReal64, read-only */

/*- PWM */

#define AG3352X_ATTR_CHANNEL_PWM_DEVIATION                  (IVI_SPECIFIC_ATTR_BASE + 109L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_PWM_DUTY_CYCLE_DEVIATION       (IVI_SPECIFIC_ATTR_BASE + 110L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_PWM_INTERNAL_FUNCTION          (IVI_SPECIFIC_ATTR_BASE + 111L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_PWM_SOURCE                     (IVI_SPECIFIC_ATTR_BASE + 70L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_PWM_INTERNAL_FREQUENCY         (IVI_SPECIFIC_ATTR_BASE + 176L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_PWM_ENABLED                    (IVI_SPECIFIC_ATTR_BASE + 178L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_CHANNEL_PWM_DEVIATION_MAX              (IVI_SPECIFIC_ATTR_BASE + 287L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_PWM_DEVIATION_MIN              (IVI_SPECIFIC_ATTR_BASE + 288L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_PWM_DUTY_CYCLE_DEVIATION_MAX   (IVI_SPECIFIC_ATTR_BASE + 289L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_PWM_DUTY_CYCLE_DEVIATION_MIN   (IVI_SPECIFIC_ATTR_BASE + 290L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_PWM_INTERNAL_FREQUENCY_MAX     (IVI_SPECIFIC_ATTR_BASE + 291L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_PWM_INTERNAL_FREQUENCY_MIN     (IVI_SPECIFIC_ATTR_BASE + 292L)  /* ViReal64, read-only */

/*- Sweep */

#define AG3352X_ATTR_CHANNEL_SWEEP_HOLD_TIME                (IVI_SPECIFIC_ATTR_BASE + 122L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_SWEEP_RETURN_TIME              (IVI_SPECIFIC_ATTR_BASE + 123L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_SWEEP_SPACING                  (IVI_SPECIFIC_ATTR_BASE + 124L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_SWEEP_ENABLED                  (IVI_SPECIFIC_ATTR_BASE + 125L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_CHANNEL_SWEEP_HOLD_TIME_MAX            (IVI_SPECIFIC_ATTR_BASE + 295L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_SWEEP_HOLD_TIME_MIN            (IVI_SPECIFIC_ATTR_BASE + 296L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_SWEEP_RETURN_TIME_MAX          (IVI_SPECIFIC_ATTR_BASE + 297L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_SWEEP_RETURN_TIME_MIN          (IVI_SPECIFIC_ATTR_BASE + 298L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_SWEEP_TIME_MAX                 (IVI_SPECIFIC_ATTR_BASE + 299L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_SWEEP_TIME_MIN                 (IVI_SPECIFIC_ATTR_BASE + 300L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_SWEEP_TIME                     (IVI_SPECIFIC_ATTR_BASE + 315L)  /* ViReal64, read-write */

/*- Frequency */

#define AG3352X_ATTR_CHANNEL_SWEEP_FREQUENCY_CENTER         (IVI_SPECIFIC_ATTR_BASE + 45L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_SWEEP_FREQUENCY_SPAN           (IVI_SPECIFIC_ATTR_BASE + 54L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_SWEEP_FREQUENCY_START          (IVI_SPECIFIC_ATTR_BASE + 73L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_SWEEP_FREQUENCY_STOP           (IVI_SPECIFIC_ATTR_BASE + 80L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_SWEEP_FREQUENCY_CENTER_MAX     (IVI_SPECIFIC_ATTR_BASE + 301L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_SWEEP_FREQUENCY_CENTER_MIN     (IVI_SPECIFIC_ATTR_BASE + 302L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_SWEEP_FREQUENCY_SPAN_MAX       (IVI_SPECIFIC_ATTR_BASE + 303L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_SWEEP_FREQUENCY_SPAN_MIN       (IVI_SPECIFIC_ATTR_BASE + 304L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_SWEEP_FREQUENCY_START_MAX      (IVI_SPECIFIC_ATTR_BASE + 305L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_SWEEP_FREQUENCY_START_MIN      (IVI_SPECIFIC_ATTR_BASE + 306L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_SWEEP_FREQUENCY_STOP_MAX       (IVI_SPECIFIC_ATTR_BASE + 307L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_SWEEP_FREQUENCY_STOP_MIN       (IVI_SPECIFIC_ATTR_BASE + 308L)  /* ViReal64, read-only */

/*- SumModulation */

#define AG3352X_ATTR_CHANNEL_SUMMODULATION_INTERNAL_DEPTH         (IVI_SPECIFIC_ATTR_BASE + 115L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_SUMMODULATION_INTERNAL_FREQUENCY     (IVI_SPECIFIC_ATTR_BASE + 118L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_SUMMODULATION_INTERNAL_FUNCTION      (IVI_SPECIFIC_ATTR_BASE + 119L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_SUMMODULATION_SOURCE                 (IVI_SPECIFIC_ATTR_BASE + 120L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_SUMMODULATION_ENABLED                (IVI_SPECIFIC_ATTR_BASE + 121L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_CHANNEL_SUMMODULATION_COMBINE                (IVI_SPECIFIC_ATTR_BASE + 29L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_SUMMODULATION_INTERNAL_FREQUENCY_MAX (IVI_SPECIFIC_ATTR_BASE + 293L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_SUMMODULATION_INTERNAL_FREQUENCY_MIN (IVI_SPECIFIC_ATTR_BASE + 294L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_SUMMODULATION_INTERNAL_DEPTH_MAX     (IVI_SPECIFIC_ATTR_BASE + 313L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_SUMMODULATION_INTERNAL_DEPTH_MIN     (IVI_SPECIFIC_ATTR_BASE + 314L)  /* ViReal64, read-only */

/*- ArbitraryWaveform */

#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_ARBITRARYWAVEFORM_ADVANCE_CLOCK_SOURCE  (IVI_SPECIFIC_ATTR_BASE + 167L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_ARBITRARYWAVEFORM_INTERPOLATION_ENABLED (IVI_SPECIFIC_ATTR_BASE + 168L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_ARBITRARYWAVEFORM_FORMAT_BORDER         (IVI_SPECIFIC_ATTR_BASE + 25L)  /* ViInt32, read-write */
#define AG3352X_ATTR_ARBITRARY_WAVEFORM_SYMMETRIC_AMPLITUDE                         (IVI_SPECIFIC_ATTR_BASE + 331L)  /* ViReal64, read-write */
#define AG3352X_ATTR_ARBITRARY_WAVEFORM_SELECTED_WAVEFORM                           (IVI_SPECIFIC_ATTR_BASE + 332L)  /* ViString, read-only */

/*- SRate */

#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_ARBITRARYWAVEFORM_SAMPLE_RATE (IVI_SPECIFIC_ATTR_BASE + 19L)  /* ViReal64, read-write */

/*- Coupling */

#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_ARBITRARYWAVEFORM_SRATE_COUPLING_COUPLE_ENABLED (IVI_SPECIFIC_ATTR_BASE + 42L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_ARBITRARYWAVEFORM_SRATE_COUPLING_RATIO          (IVI_SPECIFIC_ATTR_BASE + 49L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_ARBITRARYWAVEFORM_SRATE_COUPLING_OFFSET         (IVI_SPECIFIC_ATTR_BASE + 50L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_ARBITRARYWAVEFORM_SRATE_COUPLING_MODE           (IVI_SPECIFIC_ATTR_BASE + 51L)  /* ViInt32, read-write */

/*- DualArbCorrections */

#define AG3352X_ATTR_DUAL_ARB_CORRECTIONS_BALANCE_ENABLED   (IVI_SPECIFIC_ATTR_BASE + 326L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_DUAL_ARB_CORRECTIONS_SKEW_ENABLED      (IVI_SPECIFIC_ATTR_BASE + 327L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_DUAL_ARB_CORRECTIONS_SKEW_TIME         (IVI_SPECIFIC_ATTR_BASE + 328L)  /* ViReal64, read-write */
#define AG3352X_ATTR_DUAL_ARB_CORRECTIONS_BALANCE_OFFSET    (IVI_SPECIFIC_ATTR_BASE + 329L)  /* ViReal64, read-write */
#define AG3352X_ATTR_DUAL_ARB_CORRECTIONS_BALANCE_GAIN      (IVI_SPECIFIC_ATTR_BASE + 330L)  /* ViReal64, read-write */

/*- Pulse */

#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PULSE_DUTY_CYCLE     (IVI_SPECIFIC_ATTR_BASE + 150L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PULSE_PERIOD         (IVI_SPECIFIC_ATTR_BASE + 152L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PULSE_WIDTH          (IVI_SPECIFIC_ATTR_BASE + 153L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PULSE_DUTY_CYCLE_MAX (IVI_SPECIFIC_ATTR_BASE + 229L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PULSE_DUTY_CYCLE_MIN (IVI_SPECIFIC_ATTR_BASE + 230L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PULSE_PERIOD_MAX     (IVI_SPECIFIC_ATTR_BASE + 231L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PULSE_PERIOD_MIN     (IVI_SPECIFIC_ATTR_BASE + 232L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PULSE_WIDTH_MAX      (IVI_SPECIFIC_ATTR_BASE + 233L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PULSE_WIDTH_MIN      (IVI_SPECIFIC_ATTR_BASE + 234L)  /* ViReal64, read-only */

/*- Transition */

#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PULSE_TRANSITION_EDGE_TIME              (IVI_SPECIFIC_ATTR_BASE + 154L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PULSE_TRANSITION_LEADING_EDGE_TIME      (IVI_SPECIFIC_ATTR_BASE + 155L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PULSE_TRANSITION_TRAILING_EDGE_TIME     (IVI_SPECIFIC_ATTR_BASE + 156L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PULSE_TRANSITION_EDGE_TIME_MAX          (IVI_SPECIFIC_ATTR_BASE + 235L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PULSE_TRANSITION_EDGE_TIME_MIN          (IVI_SPECIFIC_ATTR_BASE + 236L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PULSE_TRANSITION_LEADING_EDGE_TIME_MAX  (IVI_SPECIFIC_ATTR_BASE + 237L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PULSE_TRANSITION_LEADING_EDGE_TIME_MIN  (IVI_SPECIFIC_ATTR_BASE + 238L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PULSE_TRANSITION_TRAILING_EDGE_TIME_MAX (IVI_SPECIFIC_ATTR_BASE + 239L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PULSE_TRANSITION_TRAILING_EDGE_TIME_MIN (IVI_SPECIFIC_ATTR_BASE + 240L)  /* ViReal64, read-only */

/*- Noise */

#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_NOISE_BANDWIDTH     (IVI_SPECIFIC_ATTR_BASE + 147L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_NOISE_BANDWIDTH_MAX (IVI_SPECIFIC_ATTR_BASE + 225L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_NOISE_BANDWIDTH_MIN (IVI_SPECIFIC_ATTR_BASE + 226L)  /* ViReal64, read-only */

/*- PRBS */

#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PRBS_BIT_RATE     (IVI_SPECIFIC_ATTR_BASE + 148L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PRBS_DATA_FORMAT  (IVI_SPECIFIC_ATTR_BASE + 149L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PRBS_BIT_RATE_MAX (IVI_SPECIFIC_ATTR_BASE + 227L)  /* ViInt32, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PRBS_BIT_RATE_MIN (IVI_SPECIFIC_ATTR_BASE + 228L)  /* ViReal64, read-only */

/*- Transition */

#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PRBS_TRANSITION_EDGE_TIME     (IVI_SPECIFIC_ATTR_BASE + 163L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PRBS_TRANSITION_EDGE_TIME_MAX (IVI_SPECIFIC_ATTR_BASE + 164L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PRBS_TRANSITION_EDGE_TIME_MIN (IVI_SPECIFIC_ATTR_BASE + 165L)  /* ViReal64, read-only */

/*- Ramp */

#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_RAMP_SYMMETRY     (IVI_SPECIFIC_ATTR_BASE + 158L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_RAMP_SYMMETRY_MAX (IVI_SPECIFIC_ATTR_BASE + 241L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_RAMP_SYMMETRY_MIN (IVI_SPECIFIC_ATTR_BASE + 242L)  /* ViReal64, read-only */

/*- Square */

#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_SQUARE_DUTY_CYCLE     (IVI_SPECIFIC_ATTR_BASE + 159L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_SQUARE_PERIOD         (IVI_SPECIFIC_ATTR_BASE + 161L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_SQUARE_DUTY_CYCLE_MAX (IVI_SPECIFIC_ATTR_BASE + 243L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_SQUARE_DUTY_CYCLE_MIN (IVI_SPECIFIC_ATTR_BASE + 244L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_SQUARE_PERIOD_MAX     (IVI_SPECIFIC_ATTR_BASE + 245L)  /* ViReal64, read-only */
#define AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_SQUARE_PERIOD_MIN     (IVI_SPECIFIC_ATTR_BASE + 246L)  /* ViReal64, read-only */

/*- Display */

#define AG3352X_ATTR_DISPLAY_TEXT                           (IVI_SPECIFIC_ATTR_BASE + 71L)  /* ViString, read-write */
#define AG3352X_ATTR_DISPLAY_ENABLED                        (IVI_SPECIFIC_ATTR_BASE + 170L)  /* ViBoolean, read-write */

/*- HCopy */

#define AG3352X_ATTR_DISPLAY_HCOPY_DATA_FORMAT              (IVI_SPECIFIC_ATTR_BASE + 87L)  /* ViInt32, read-write */

/*- StateStorage */

#define AG3352X_ATTR_STATE_STORAGE_AUTO_RECALL_ENABLED      (IVI_SPECIFIC_ATTR_BASE + 173L)  /* ViBoolean, read-write */
#define AG3352X_ATTR_STATE_STORAGE_NUMBER_OF_STATES         (IVI_SPECIFIC_ATTR_BASE + 174L)  /* ViInt32, read-only */

/*- Memory */

#define AG3352X_ATTR_MEMORY_CURRENT_DIRECTORY               (IVI_SPECIFIC_ATTR_BASE + 317L)  /* ViString, read-write */

/*- Calibration */

#define AG3352X_ATTR_CALIBRATION_COUNT                      (IVI_SPECIFIC_ATTR_BASE + 65L)  /* ViInt32, read-only */
#define AG3352X_ATTR_CALIBRATION_SECURE_CODE                (IVI_SPECIFIC_ATTR_BASE + 66L)  /* ViString, write-only */
#define AG3352X_ATTR_CALIBRATION_SETUP                      (IVI_SPECIFIC_ATTR_BASE + 67L)  /* ViInt32, read-write */
#define AG3352X_ATTR_CALIBRATION_STORE_MESSAGE              (IVI_SPECIFIC_ATTR_BASE + 68L)  /* ViString, read-write */
#define AG3352X_ATTR_CALIBRATION_VALUE                      (IVI_SPECIFIC_ATTR_BASE + 69L)  /* ViReal64, read-write */
#define AG3352X_ATTR_CALIBRATION_SECURE_STATE               (IVI_SPECIFIC_ATTR_BASE + 117L)  /* ViBoolean, read-only */

/*- System */

#define AG3352X_ATTR_DATE                                   (IVI_SPECIFIC_ATTR_BASE + 195L)  /* ViString, read-write */
#define AG3352X_ATTR_SERIAL_NUMBER                          (IVI_SPECIFIC_ATTR_BASE + 196L)  /* ViString, read-only */
#define AG3352X_ATTR_TIME                                   (IVI_SPECIFIC_ATTR_BASE + 197L)  /* ViString, read-write */
#define AG3352X_ATTR_TIMEOUT_MILLISECONDS                   (IVI_SPECIFIC_ATTR_BASE + 198L)  /* ViInt32, read-write */
#define AG3352X_ATTR_TRACE_ENABLED                          (IVI_SPECIFIC_ATTR_BASE + 199L)  /* ViBoolean, read-write */

/*- Channel */

#define AG3352X_ATTR_CHANNEL_REPCAP_COUNT                   (IVI_SPECIFIC_ATTR_BASE + 103L)  /* ViInt32, read-only */


/**************************************************************************** 
 *------------------------ Attribute Value Defines -------------------------* 
 ****************************************************************************/

/*- Defined values for 
	attribute AG3352X_ATTR_OUTPUT_MODE
	parameter OutputMode in function Ag3352x_ConfigureOutputMode */

#define AG3352X_VAL_OUTPUT_FUNC                             0
#define AG3352X_VAL_OUTPUT_ARB                              1
#define AG3352X_VAL_OUTPUT_SEQ                              2

/*- Defined values for 
	attribute AG3352X_ATTR_OPERATION_MODE
	parameter OperationMode in function Ag3352x_ConfigureOperationMode */

#define AG3352X_VAL_OPERATE_CONTINUOUS                      0
#define AG3352X_VAL_OPERATE_BURST                           1

/*- Defined values for 
	attribute AG3352X_ATTR_REF_CLOCK_SOURCE
	parameter Source in function Ag3352x_ConfigureRefClockSource */

#define AG3352X_VAL_REF_CLOCK_INTERNAL                      0
#define AG3352X_VAL_REF_CLOCK_EXTERNAL                      1

/*- Defined values for 
	attribute AG3352X_ATTR_TRIGGER_SOURCE
	parameter Source in function Ag3352x_ConfigureTriggerSource */

#define AG3352X_VAL_EXTERNAL                                1
#define AG3352X_VAL_SOFTWARE_TRIG                           2
#define AG3352X_VAL_INTERNAL_TRIGGER                        3

/*- Defined values for 
	attribute AG3352X_ATTR_FUNC_WAVEFORM
	parameter Waveform in function Ag3352x_ConfigureStandardWaveform */

#define AG3352X_VAL_WFM_SINE                                1
#define AG3352X_VAL_WFM_SQUARE                              2
#define AG3352X_VAL_WFM_TRIANGLE                            3
#define AG3352X_VAL_WFM_RAMP_UP                             4
#define AG3352X_VAL_WFM_RAMP_DOWN                           5
#define AG3352X_VAL_WFM_DC                                  6

/*- Defined values for 
	attribute AG3352X_ATTR_AM_INTERNAL_WAVEFORM
	parameter Waveform in function Ag3352x_ConfigureAMInternal */

#define AG3352X_VAL_AM_INTERNAL_SINE                        1
#define AG3352X_VAL_AM_INTERNAL_SQUARE                      2
#define AG3352X_VAL_AM_INTERNAL_TRIANGLE                    3
#define AG3352X_VAL_AM_INTERNAL_RAMP_UP                     4
#define AG3352X_VAL_AM_INTERNAL_RAMP_DOWN                   5

/*- Defined values for 
	attribute AG3352X_ATTR_AM_SOURCE
	parameter Source in function Ag3352x_ConfigureAMSource */

#define AG3352X_VAL_AM_INTERNAL                             0
#define AG3352X_VAL_AM_EXTERNAL                             1

/*- Defined values for 
	attribute AG3352X_ATTR_FM_INTERNAL_WAVEFORM
	parameter Waveform in function Ag3352x_ConfigureFMInternal */

#define AG3352X_VAL_FM_INTERNAL_SINE                        1
#define AG3352X_VAL_FM_INTERNAL_SQUARE                      2
#define AG3352X_VAL_FM_INTERNAL_TRIANGLE                    3
#define AG3352X_VAL_FM_INTERNAL_RAMP_UP                     4
#define AG3352X_VAL_FM_INTERNAL_RAMP_DOWN                   5

/*- Defined values for 
	attribute AG3352X_ATTR_FM_SOURCE
	parameter Source in function Ag3352x_ConfigureFMSource */

#define AG3352X_VAL_FM_INTERNAL                             0
#define AG3352X_VAL_FM_EXTERNAL                             1

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_OUTPUT_MODE */

#define AG3352X_VAL_OUTPUT_MODE_FUNCTION                    0
#define AG3352X_VAL_OUTPUT_MODE_ARBITRARY                   1

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_AM_INTERNAL_FUNCTION
	attribute AG3352X_ATTR_CHANNEL_FM_INTERNAL_FUNCTION
	attribute AG3352X_ATTR_CHANNEL_PM_INTERNAL_FUNCTION
	attribute AG3352X_ATTR_CHANNEL_PWM_INTERNAL_FUNCTION
	attribute AG3352X_ATTR_CHANNEL_SUMMODULATION_INTERNAL_FUNCTION */

#define AG3352X_VAL_AM_INTERNAL_WAVEFORM_SINE               0
#define AG3352X_VAL_AM_INTERNAL_WAVEFORM_SQUARE             1
#define AG3352X_VAL_AM_INTERNAL_WAVEFORM_RAMP_UP            2
#define AG3352X_VAL_AM_INTERNAL_FUNCTION_NEGATIVE_RAMP      3
#define AG3352X_VAL_AM_INTERNAL_WAVEFORM_TRIANGLE           4
#define AG3352X_VAL_AM_INTERNAL_FUNCTION_NOISE              5
#define AG3352X_VAL_AM_INTERNAL_FUNCTION_PR_NOISE           6
#define AG3352X_VAL_AM_INTERNAL_FUNCTION_USER               7

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_AM_SOURCE
	attribute AG3352X_ATTR_CHANNEL_FM_SOURCE
	attribute AG3352X_ATTR_CHANNEL_BPSK_SOURCE
	attribute AG3352X_ATTR_CHANNEL_FSK_SOURCE
	attribute AG3352X_ATTR_CHANNEL_PM_SOURCE
	attribute AG3352X_ATTR_CHANNEL_PWM_SOURCE
	attribute AG3352X_ATTR_CHANNEL_SUMMODULATION_SOURCE */

#define AG3352X_VAL_AM_SOURCE_INTERNAL                      0
#define AG3352X_VAL_AM_SOURCE_EXTERNAL                      1
#define AG3352X_VAL_AM_SOURCE_CHANNEL1                      2
#define AG3352X_VAL_AM_SOURCE_CHANNEL2                      3

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_AM_MODE */

#define AG3352X_VAL_AM_AMPLITUDE_MODULATION_MODE_ON         0
#define AG3352X_VAL_AM_AMPLITUDE_MODULATION_MODE_OFF        1

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_BURST_GATE_POLARITY */

#define AG3352X_VAL_BURST_GATE_POLARITY_NORMAL              0
#define AG3352X_VAL_BURST_GATE_POLARITY_INVERTED            1

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_BURST_MODE */

#define AG3352X_VAL_BURST_MODE_TRIGGERED                    0
#define AG3352X_VAL_BURST_MODE_GATED                        1

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_OUTPUT_FREQUENCY_COUPLING_MODE */

#define AG3352X_VAL_FREQUENCY_COUPLE_MODE_OFFSET            0
#define AG3352X_VAL_FREQUENCY_COUPLE_MODE_RATIO             1

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_OUTPUT_FREQUENCY_MODE */

#define AG3352X_VAL_FREQUENCY_MODE_CW                       0
#define AG3352X_VAL_FREQUENCY_MODE_FIXED                    1
#define AG3352X_VAL_OUTPUT_FREQUENCY_MODE_SWEEP             3
#define AG3352X_VAL_FREQUENCY_MODE_LIST                     2

/*- Defined values for 
	attribute AG3352X_ATTR_DISPLAY_HCOPY_DATA_FORMAT */

#define AG3352X_VAL_H_COPY_DATA_FORMAT_PNG                  0
#define AG3352X_VAL_H_COPY_DATA_FORMAT_BMP                  1

/*- Defined values for */

#define AG3352X_VAL_LIST_ADVANCE_TRIGGER_BUS                0
#define AG3352X_VAL_LIST_ADVANCE_TRIGGER_EXTERNAL           1

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_PHASELOCK_REFERENCE_OSCILLATOR_SOURCE
	attribute AG3352X_ATTR_CHANNEL_PHASELOCK_REFERENCE_OSCILLATOR_CURRENT_SOURCE */

#define AG3352X_VAL_REFERENCE_OSCILLATOR_SOURCE_INTERNAL    0
#define AG3352X_VAL_REFERENCE_OSCILLATOR_SOURCE_EXTERNAL    1

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_SWEEP_SPACING */

#define AG3352X_VAL_SWEEP_SPACING_LINEAR                    0
#define AG3352X_VAL_SWEEP_SPACING_LOGARITHMIC               1

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_TRIGGER_SLOPE */

#define AG3352X_VAL_TRIGGER_SLOPE_POSITIVE                  0
#define AG3352X_VAL_TRIGGER_SLOPE_NEGATIVE                  1

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_OUTPUT_VOLTAGE_UNITS */

#define AG3352X_VAL_VOLTAGE_UNIT_VPP                        0
#define AG3352X_VAL_VOLTAGE_UNIT_VRMS                       1
#define AG3352X_VAL_VOLTAGE_UNIT_DBM                        2

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_PRBS_DATA_FORMAT */

#define AG3352X_VAL_OUTPUT_FUNCTION_PRBS_SEQUENCE_TYPE_PN7  5
#define AG3352X_VAL_WAVEFORM_PR_NOISE_SEQUENCE_LENGTH_PN9   0
#define AG3352X_VAL_WAVEFORM_PR_NOISE_SEQUENCE_LENGTH_PN11  1
#define AG3352X_VAL_WAVEFORM_PR_NOISE_SEQUENCE_LENGTH_PN15  2
#define AG3352X_VAL_WAVEFORM_PR_NOISE_SEQUENCE_LENGTH_PN20  3
#define AG3352X_VAL_WAVEFORM_PR_NOISE_SEQUENCE_LENGTH23     4

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_ARBITRARYWAVEFORM_ADVANCE_CLOCK_SOURCE */

#define AG3352X_VAL_WAVEFORM_USER_ADVANCE_CLOCK_SOURCE_TRIGGER 0
#define AG3352X_VAL_WAVEFORM_USER_ADVANCE_CLOCK_SOURCE_CLOCK   1

/*- Defined values for 
	parameter Reason in function Ag3352x_StatusConfigureServiceRequest */

#define AG3352X_VAL_SRQ_REASON_STB_MAV                                1
#define AG3352X_VAL_SRQ_REASON_STB_ERROR_QUEUE                        2
#define AG3352X_VAL_SRQ_REASON_ESR_OPC                                4
#define AG3352X_VAL_SRQ_REASON_ESR_QUERY_ERROR                        8
#define AG3352X_VAL_SRQ_REASON_ESR_DEVICE_ERROR                       16
#define AG3352X_VAL_SRQ_REASON_ESR_EXECUTION_ERROR                    32
#define AG3352X_VAL_SRQ_REASON_ESR_COMMAND_ERROR                      64
#define AG3352X_VAL_SRQ_REASON_ESR_USER_REQUEST                       128
#define AG3352X_VAL_SRQ_REASON_QUES_VOLTAGE                           256
#define AG3352X_VAL_SRQ_REASON_QUES_POWER                             512
#define AG3352X_VAL_SRQ_REASON_QUES_TEMPERATURE                       1024
#define AG3352X_VAL_SRQ_REASON_QUES_FREQUENCY                         2048
#define AG3352X_VAL_STATUS_SRQ_REASON_OPER_CALIBRATING                4096
#define AG3352X_VAL_STATUS_SRQ_REASON_OPER_SELF_TEST                  8192
#define AG3352X_VAL_STATUS_SRQ_REASON_OPER_INITIATED                  16384
#define AG3352X_VAL_STATUS_SRQ_REASON_OPER_WAITFOR_TRIGGER            32768
#define AG3352X_VAL_STATUS_SRQ_REASON_OPER_CONFIG_CHANGED             65536
#define AG3352X_VAL_STATUS_SRQ_REASON_INSTR_LOCKED                    262144
#define AG3352X_VAL_STATUS_SRQ_REASON_OPER_GLOB_ERR_SUMM              1048576
#define AG3352X_VAL_STATUS_SRQ_REASON_QUES_VOLTAGE_OVLD_CHANNEL_2     2097152
#define AG3352X_VAL_STATUS_SRQ_REASON_OPER_INITIATED_CHANNEL_2        4194304
#define AG3352X_VAL_STATUS_SRQ_REASON_OPER_WAIT_FOR_TRIGGER_CHANNEL_2 8388608

/*- Defined values for 
	parameter Register in function Ag3352x_StatusGetRegister
	parameter Register in function Ag3352x_StatusSetRegister */

#define AG3352X_VAL_STATUS_REGISTER_STATUS_BYTE             0
#define AG3352X_VAL_STATUS_REGISTER_STANDARD_EVENT          1
#define AG3352X_VAL_STATUS_REGISTER_OPERATION               2
#define AG3352X_VAL_STATUS_REGISTER_QUESTIONABLE            3

/*- Defined values for 
	parameter SubRegister in function Ag3352x_StatusGetRegister
	parameter SubRegister in function Ag3352x_StatusSetRegister */

#define AG3352X_VAL_STATUS_SUB_REGISTER_CONDITION           0
#define AG3352X_VAL_STATUS_SUB_REGISTER_EVENT               3
#define AG3352X_VAL_STATUS_SUB_REGISTER_ENABLE              4

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_FUNCTION
	parameter Function in function Ag3352x_ChannelApplyQueryOutputConfiguration */

#define AG3352X_VAL_OUTPUT_FUNCTION_SINE                    0
#define AG3352X_VAL_OUTPUT_FUNCTION_SQUARE                  1
#define AG3352X_VAL_OUTPUT_FUNCTION_RAMP                    2
#define AG3352X_VAL_OUTPUT_FUNCTION_PULSE                   3
#define AG3352X_VAL_OUTPUT_FUNCTION_PR_NOISE                7
#define AG3352X_VAL_OUTPUT_FUNCTION_NOISE                   4
#define AG3352X_VAL_OUTPUT_FUNCTION_DC                      5
#define AG3352X_VAL_OUTPUT_FUNCTION_ARB                     6
#define AG3352X_VAL_OUTPUT_FUNCTION_TRIANGLE                9

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_OUTPUT_POLARITY */

#define AG3352X_VAL_OUTPUT_POLARITY_NORMAL                  0
#define AG3352X_VAL_OUTPUT_POLARITY_INVERTED                1

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_OUTPUT_SYNC_MODE */

#define AG3352X_VAL_OUTPUT_SYNC_MODE_NORMAL                 0
#define AG3352X_VAL_OUTPUT_SYNC_MODE_CARRIER                1
#define AG3352X_VAL_OUTPUT_SYNC_MODE_MARKER                 2

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_OUTPUT_SYNC_POLARITY */

#define AG3352X_VAL_OUTPUT_SYNC_POLARITY_NORMAL             0
#define AG3352X_VAL_OUTPUT_SYNC_POLARITY_INVERTED           1

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_OUTPUT_SYNC_SOURCE */

#define AG3352X_VAL_OUTPUT_SYNC_SOURCE_CHANNEL1             0
#define AG3352X_VAL_OUTPUT_SYNC_SOURCE_CHANNEL2             1

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_OUTPUT_TRIGGER_SLOPE */

#define AG3352X_VAL_OUTPUT_TRIGGER_SLOPE_POSITIVE           0
#define AG3352X_VAL_OUTPUT_TRIGGER_SLOPE_NEGATIVE           1

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_BURST_PHASE_UNITS
	attribute AG3352X_ATTR_CHANNEL_PHASELOCK_PHASE_UNITS */

#define AG3352X_VAL_PHASE_UNIT_DEGREE                       0
#define AG3352X_VAL_PHASE_UNIT_RADIAN                       1

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_SUMMODULATION_COMBINE */

#define AG3352X_VAL_COMBINE_CHANNEL1                        0
#define AG3352X_VAL_COMBINE_CHANNEL2                        1
#define AG3352X_VAL_COMBINE_NONE                            2

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_TRACKING */

#define AG3352X_VAL_TRACK_ON                                0
#define AG3352X_VAL_TRACK_OFF                               1
#define AG3352X_VAL_TRACK_INVERTED                          2

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_ARBITRARYWAVEFORM_FORMAT_BORDER */

#define AG3352X_VAL_ARBITRARY_WAVEFORM_FORMAT_BORDER_NORMAL  0
#define AG3352X_VAL_ARBITRARY_WAVEFORM_FORMAT_BORDER_SWAPPED 1

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_TRIGGER_SOURCE */

#define AG3352X_VAL_TRIGGER_SOURCE_IMMEDIATE                0
#define AG3352X_VAL_TRIGGER_SOURCE_EXTERNAL                 1
#define AG3352X_VAL_TRIGGER_SOURCE_TIMER                    3
#define AG3352X_VAL_TRIGGER_SOURCE_BUS                      2

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_ARBITRARYWAVEFORM_INTERPOLATION_ENABLED */

#define AG3352X_VAL_OUTPUT_FUNCTION_ARBITRARY_INTERPOLATION_NORMAL 0
#define AG3352X_VAL_OUTPUT_FUNCTION_ARBITRARY_INTERPOLATION_STEP   1
#define AG3352X_VAL_OUTPUT_FUNCTION_ARBITRARY_INTERPOLATION_OFF    2

/*- Defined values for 
	attribute AG3352X_ATTR_CHANNEL_OUTPUTFUNCTION_ARBITRARYWAVEFORM_SRATE_COUPLING_MODE */

#define AG3352X_VAL_OUTPUT_FUNCTION_ARBITRARY_WAVEFORM_S_RATE_COUPLING_MODE_OFFSET 0
#define AG3352X_VAL_OUTPUT_FUNCTION_ARBITRARY_WAVEFORM_S_RATE_COUPLING_MODE_RATIO  1

/*- Defined values for 
	parameter Format in function Ag3352x_ArbitraryWaveformLoadDualArbWaveform
	parameter Format in function Ag3352x_ArbitraryWaveformLoadDualArbWaveformDAC */

#define AG3352X_VAL_ARBITRARY_WAVEFORM_DUAL_ARB_FORMAT_INTERLEAVED 0
#define AG3352X_VAL_ARBITRARY_WAVEFORM_DUAL_ARB_FORMAT_SEQUENTIAL  1


/**************************************************************************** 
 *---------------- Instrument Driver Function Declarations -----------------* 
 ****************************************************************************/

/*- Ag3352x */

ViStatus _VI_FUNC Ag3352x_init ( ViRsrc ResourceName, ViBoolean IdQuery, ViBoolean Reset, ViSession* Vi );
ViStatus _VI_FUNC Ag3352x_close ( ViSession Vi );
ViStatus _VI_FUNC Ag3352x_InitWithOptions ( ViRsrc ResourceName, ViBoolean IdQuery, ViBoolean Reset, ViConstString OptionsString, ViSession* Vi );

/*- Utility */

ViStatus _VI_FUNC Ag3352x_revision_query ( ViSession Vi, ViChar DriverRev[], ViChar InstrRev[] );
ViStatus _VI_FUNC Ag3352x_error_message ( ViSession Vi, ViStatus ErrorCode, ViChar ErrorMessage[] );
ViStatus _VI_FUNC Ag3352x_GetError ( ViSession Vi, ViStatus* ErrorCode, ViInt32 ErrorDescriptionBufferSize, ViChar ErrorDescription[] );
ViStatus _VI_FUNC Ag3352x_ClearError ( ViSession Vi );
ViStatus _VI_FUNC Ag3352x_ClearInterchangeWarnings ( ViSession Vi );
ViStatus _VI_FUNC Ag3352x_GetNextCoercionRecord ( ViSession Vi, ViInt32 CoercionRecordBufferSize, ViChar CoercionRecord[] );
ViStatus _VI_FUNC Ag3352x_GetNextInterchangeWarning ( ViSession Vi, ViInt32 InterchangeWarningBufferSize, ViChar InterchangeWarning[] );
ViStatus _VI_FUNC Ag3352x_InvalidateAllAttributes ( ViSession Vi );
ViStatus _VI_FUNC Ag3352x_ResetInterchangeCheck ( ViSession Vi );
ViStatus _VI_FUNC Ag3352x_Disable ( ViSession Vi );
ViStatus _VI_FUNC Ag3352x_error_query ( ViSession Vi, ViInt32* ErrorCode, ViChar ErrorMessage[] );
ViStatus _VI_FUNC Ag3352x_LockSession ( ViSession Vi, ViBoolean* CallerHasLock );
ViStatus _VI_FUNC Ag3352x_reset ( ViSession Vi );
ViStatus _VI_FUNC Ag3352x_ResetWithDefaults ( ViSession Vi );
ViStatus _VI_FUNC Ag3352x_self_test ( ViSession Vi, ViInt16* TestResult, ViChar TestMessage[] );
ViStatus _VI_FUNC Ag3352x_UnlockSession ( ViSession Vi, ViBoolean* CallerHasLock );
ViStatus _VI_FUNC Ag3352x_GetChannelName ( ViSession Vi, ViInt32 Index, ViInt32 NameBufferSize, ViChar Name[] );

/*- Attribute Accessors */

ViStatus _VI_FUNC Ag3352x_GetAttributeViInt32 ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViInt32* AttributeValue );
ViStatus _VI_FUNC Ag3352x_GetAttributeViReal64 ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViReal64* AttributeValue );
ViStatus _VI_FUNC Ag3352x_GetAttributeViBoolean ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViBoolean* AttributeValue );
ViStatus _VI_FUNC Ag3352x_GetAttributeViSession ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViSession* AttributeValue );
ViStatus _VI_FUNC Ag3352x_GetAttributeViString ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViInt32 AttributeValueBufferSize, ViChar AttributeValue[] );
ViStatus _VI_FUNC Ag3352x_SetAttributeViInt32 ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViInt32 AttributeValue );
ViStatus _VI_FUNC Ag3352x_SetAttributeViReal64 ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViReal64 AttributeValue );
ViStatus _VI_FUNC Ag3352x_SetAttributeViBoolean ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViBoolean AttributeValue );
ViStatus _VI_FUNC Ag3352x_SetAttributeViSession ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViSession AttributeValue );
ViStatus _VI_FUNC Ag3352x_SetAttributeViString ( ViSession Vi, ViConstString RepCapIdentifier, ViAttr AttributeID, ViConstString AttributeValue );

/*- General */

ViStatus _VI_FUNC Ag3352x_ConfigureOperationMode ( ViSession Vi, ViConstString ChannelName, ViInt32 OperationMode );
ViStatus _VI_FUNC Ag3352x_ConfigureOutputEnabled ( ViSession Vi, ViConstString ChannelName, ViBoolean Enabled );
ViStatus _VI_FUNC Ag3352x_ConfigureOutputImpedance ( ViSession Vi, ViConstString ChannelName, ViReal64 Impedance );
ViStatus _VI_FUNC Ag3352x_ConfigureOutputMode ( ViSession Vi, ViInt32 OutputMode );
ViStatus _VI_FUNC Ag3352x_ConfigureRefClockSource ( ViSession Vi, ViInt32 Source );

/*- Arbitrary Waveform */

ViStatus _VI_FUNC Ag3352x_ConfigureSampleRate ( ViSession Vi, ViReal64 SampleRate );
ViStatus _VI_FUNC Ag3352x_QueryArbWfmCapabilities ( ViSession Vi, ViInt32* MaxNumWfms, ViInt32* WfmQuantum, ViInt32* MinWfmSize, ViInt32* MaxWfmSize );
ViStatus _VI_FUNC Ag3352x_ClearArbWaveform ( ViSession Vi, ViInt32 Handle );
ViStatus _VI_FUNC Ag3352x_ConfigureArbWaveform ( ViSession Vi, ViConstString ChannelName, ViInt32 Handle, ViReal64 Gain, ViReal64 Offset );
ViStatus _VI_FUNC Ag3352x_CreateArbWaveform ( ViSession Vi, ViInt32 Size, ViReal64 Data[], ViInt32* Handle );

/*- Arbitrary Waveform Frequency */

ViStatus _VI_FUNC Ag3352x_ConfigureArbFrequency ( ViSession Vi, ViConstString ChannelName, ViReal64 Frequency );

/*- Arbitrary Sequence */

ViStatus _VI_FUNC Ag3352x_QueryArbSeqCapabilities ( ViSession Vi, ViInt32* MaxNumSeqs, ViInt32* MinSeqLength, ViInt32* MaxSeqLength, ViInt32* MaxLoopCount );
ViStatus _VI_FUNC Ag3352x_ClearArbMemory ( ViSession Vi );
ViStatus _VI_FUNC Ag3352x_ClearArbSequence ( ViSession Vi, ViInt32 Handle );
ViStatus _VI_FUNC Ag3352x_ConfigureArbSequence ( ViSession Vi, ViConstString ChannelName, ViInt32 Handle, ViReal64 Gain, ViReal64 Offset );
ViStatus _VI_FUNC Ag3352x_CreateArbSequence ( ViSession Vi, ViInt32 Length, ViInt32 WfmHandle[], ViInt32 LoopCount[], ViInt32* Handle );

/*- Trigger */

ViStatus _VI_FUNC Ag3352x_ConfigureTriggerSource ( ViSession Vi, ViConstString ChannelName, ViInt32 Source );

/*- Internal Trigger */

ViStatus _VI_FUNC Ag3352x_ConfigureInternalTriggerRate ( ViSession Vi, ViReal64 Rate );

/*- Burst */

ViStatus _VI_FUNC Ag3352x_ConfigureBurstCount ( ViSession Vi, ViConstString ChannelName, ViInt32 Count );

/*- Amplitude Modulation */

ViStatus _VI_FUNC Ag3352x_ConfigureAMEnabled ( ViSession Vi, ViConstString ChannelName, ViBoolean Enabled );
ViStatus _VI_FUNC Ag3352x_ConfigureAMSource ( ViSession Vi, ViConstString ChannelName, ViInt32 Source );
ViStatus _VI_FUNC Ag3352x_ConfigureAMInternal ( ViSession Vi, ViReal64 Depth, ViInt32 Waveform, ViReal64 Frequency );

/*- Frequency Modulation */

ViStatus _VI_FUNC Ag3352x_ConfigureFMEnabled ( ViSession Vi, ViConstString ChannelName, ViBoolean Enabled );
ViStatus _VI_FUNC Ag3352x_ConfigureFMSource ( ViSession Vi, ViConstString ChannelName, ViInt32 Source );
ViStatus _VI_FUNC Ag3352x_ConfigureFMInternal ( ViSession Vi, ViReal64 Deviation, ViInt32 Waveform, ViReal64 Frequency );

/*- Standard Waveform */

ViStatus _VI_FUNC Ag3352x_ConfigureStandardWaveform ( ViSession Vi, ViConstString ChannelName, ViInt32 Waveform, ViReal64 Amplitude, ViReal64 DCOffset, ViReal64 Frequency, ViReal64 StartPhase );

/*- Action */

ViStatus _VI_FUNC Ag3352x_AbortGeneration ( ViSession Vi );
ViStatus _VI_FUNC Ag3352x_InitiateGeneration ( ViSession Vi );
ViStatus _VI_FUNC Ag3352x_SendSoftwareTrigger ( ViSession Vi );

/*- Output */

ViStatus _VI_FUNC Ag3352x_ChannelOutputSetLoadInfinity ( ViSession Vi, ViConstString RepCapIdentifier );

/*- List */

ViStatus _VI_FUNC Ag3352x_ChannelOutputListSetFrequencyList ( ViSession Vi, ViConstString RepCapIdentifier, ViInt32 FrequencyBufferSize, ViReal64 Frequency[] );
ViStatus _VI_FUNC Ag3352x_ChannelOutputListGetFreqencyList ( ViSession Vi, ViConstString RepCapIdentifier, ViInt32 FreqListBufferSize, ViReal64 FreqList[], ViInt32* FreqListActualSize );

/*- Voltage */

ViStatus _VI_FUNC Ag3352x_ChannelOutputVoltageAutoRangingOnce ( ViSession Vi, ViConstString RepCapIdentifier );

/*- Trigger */

ViStatus _VI_FUNC Ag3352x_ChannelTriggerSoftwareTrigger ( ViSession Vi, ViConstString RepCapIdentifier );
ViStatus _VI_FUNC Ag3352x_ChannelTriggerAbort ( ViSession Vi, ViConstString RepCapIdentifier );
ViStatus _VI_FUNC Ag3352x_ChannelTrigger ( ViSession Vi, ViConstString RepCapIdentifier );
ViStatus _VI_FUNC Ag3352x_ChannelTriggerInitiate ( ViSession Vi, ViConstString RepCapIdentifier );

/*- Apply */

ViStatus _VI_FUNC Ag3352x_ChannelApplySetDC ( ViSession Vi, ViConstString RepCapIdentifier, ViReal64 Offset );
ViStatus _VI_FUNC Ag3352x_ChannelApplySetNoise ( ViSession Vi, ViConstString RepCapIdentifier, ViReal64 Amplitude, ViReal64 Offset );
ViStatus _VI_FUNC Ag3352x_ChannelApplySetPulse ( ViSession Vi, ViConstString RepCapIdentifier, ViReal64 Frequency, ViReal64 Amplitude, ViReal64 Offset );
ViStatus _VI_FUNC Ag3352x_ChannelApplySetRamp ( ViSession Vi, ViConstString RepCapIdentifier, ViReal64 Frequency, ViReal64 Amplitude, ViReal64 Offset );
ViStatus _VI_FUNC Ag3352x_ChannelApplySetSinusoid ( ViSession Vi, ViConstString RepCapIdentifier, ViReal64 Frequency, ViReal64 Amplitude, ViReal64 Offset );
ViStatus _VI_FUNC Ag3352x_ChannelApplySetSquare ( ViSession Vi, ViConstString RepCapIdentifier, ViReal64 Frequency, ViReal64 Amplitude, ViReal64 Offset );
ViStatus _VI_FUNC Ag3352x_ChannelApplySetArbWaveform ( ViSession Vi, ViConstString RepCapIdentifier, ViReal64 SampleRate, ViReal64 Amplitude, ViReal64 Offset );
ViStatus _VI_FUNC Ag3352x_ChannelApplySetPRBS ( ViSession Vi, ViConstString RepCapIdentifier, ViReal64 Frequency, ViReal64 Amplitude, ViReal64 Offset );
ViStatus _VI_FUNC Ag3352x_ChannelApplyQueryOutputConfiguration ( ViSession Vi, ViConstString RepCapIdentifier, ViInt32* Function, ViReal64* Frequency, ViReal64* Amplitude, ViReal64* Offset );
ViStatus _VI_FUNC Ag3352x_ChannelApplySetTriangle ( ViSession Vi, ViConstString Channel, ViReal64 Frequency, ViReal64 Amplitude, ViReal64 Offset );

/*- Burst */

ViStatus _VI_FUNC Ag3352x_ChannelBurstSetNumberOfCyclesInfinity ( ViSession Vi, ViConstString RepCapIdentifier );

/*- PhaseLock */

ViStatus _VI_FUNC Ag3352x_ChannelPhaseLockSetReference ( ViSession Vi, ViConstString RepCapIdentifier );
ViStatus _VI_FUNC Ag3352x_ChannelPhaseLockSynchronizeChannels ( ViSession Vi, ViConstString RepCapIdentifier );

/*- ArbitraryWaveform */

ViStatus _VI_FUNC Ag3352x_ChannelOutputFunctionArbitraryWaveformSelectArbWaveform ( ViSession Vi, ViConstString RepCapIdentifier, ViConstString Name );
ViStatus _VI_FUNC Ag3352x_ChannelOutputFunctionArbitraryWaveformClear ( ViSession Vi, ViConstString RepCapIdentifier );
ViStatus _VI_FUNC Ag3352x_ChannelOutputFunctionArbitraryWaveformLoadArbWaveform ( ViSession Vi, ViConstString RepCapIdentifier, ViConstString Name, ViInt32 DataBufferSize, ViReal64 Data[], ViInt32* DataActualSize );
ViStatus _VI_FUNC Ag3352x_ChannelOutputFunctionArbitraryWaveformOpenArbWaveform ( ViSession Vi, ViConstString RepCapIdentifier, ViConstString FileName );
ViStatus _VI_FUNC Ag3352x_ChannelOutputFunctionArbitraryWaveformStoreArbWaveform ( ViSession Vi, ViConstString RepCapIdentifier, ViConstString FileName );
ViStatus _VI_FUNC Ag3352x_ChannelOutputFunctionArbitraryWaveformCreateArbSequence ( ViSession Vi, ViConstString repCapIdentifier, ViConstString SequenceName, ViInt32 DataBufferSize, ViChar ArbName[], ViInt32 RepeatCount[], ViChar PlayControl[], ViChar MarkerMode[], ViInt32 MarkerPoint[] );
ViStatus _VI_FUNC Ag3352x_ChannelOutputFunctionArbitraryWaveformGetCatalog ( ViSession Vi, ViConstString repCapIdentifier, ViInt32 retValBufferSize, ViChar retVal[] );
ViStatus _VI_FUNC Ag3352x_ArbitraryWaveformLoadDualArbWaveform ( ViSession Vi, ViConstString Channel, ViInt32 Format, ViConstString Name, ViInt32 DataBufferSize, ViReal32 Data[] );
ViStatus _VI_FUNC Ag3352x_ArbitraryWaveformSynchronizeArbs ( ViSession Vi, ViConstString Channel );
ViStatus _VI_FUNC Ag3352x_ArbitraryWaveformLoadArbWaveformDAC ( ViSession Vi, ViConstString Channel, ViConstString Name, ViInt32 DataBufferSize, ViInt16 Data[] );
ViStatus _VI_FUNC Ag3352x_ArbitraryWaveformLoadDualArbWaveformDAC ( ViSession Vi, ViConstString Channel, ViInt32 Format, ViConstString Name, ViInt32 DataBufferSize, ViInt16 Data[] );

/*- Attribute */

ViStatus _VI_FUNC Ag3352x_ChannelOutputFunctionArbitraryWaveformAttributeGetAverage ( ViSession Vi, ViConstString RepCapIdentifier, ViConstString ArbName, ViReal64* RetVal );
ViStatus _VI_FUNC Ag3352x_ChannelOutputFunctionArbitraryWaveformAttributeGetCrestFactor ( ViSession Vi, ViConstString RepCapIdentifier, ViConstString ArbName, ViReal64* RetVal );
ViStatus _VI_FUNC Ag3352x_ChannelOutputFunctionArbitraryWaveformAttributeGetDataPoints ( ViSession Vi, ViConstString RepCapIdentifier, ViConstString ArbName, ViInt32* RetVal );
ViStatus _VI_FUNC Ag3352x_ChannelOutputFunctionArbitraryWaveformAttributeGetPeakToPeak ( ViSession Vi, ViConstString RepCapIdentifier, ViConstString ArbName, ViReal64* RetVal );

/*- Display */

ViStatus _VI_FUNC Ag3352x_DisplayTextClear ( ViSession Vi );

/*- HCopy */

ViStatus _VI_FUNC Ag3352x_DisplayHCopyGetScreenDump ( ViSession Vi, ViInt32 RetValBufferSize, ViChar RetVal[], ViInt32* RetValActualSize );

/*- StateStorage */

ViStatus _VI_FUNC Ag3352x_StateStorageDelete ( ViSession Vi, ViInt32 State );
ViStatus _VI_FUNC Ag3352x_StateStorageSave ( ViSession Vi, ViInt32 State );
ViStatus _VI_FUNC Ag3352x_StateStorageRecall ( ViSession Vi, ViInt32 State );
ViStatus _VI_FUNC Ag3352x_StateStorageGetName ( ViSession Vi, ViInt32 State, ViInt32 RetValBufferSize, ViChar RetVal[] );
ViStatus _VI_FUNC Ag3352x_StateStorageSetName ( ViSession Vi, ViInt32 State, ViConstString Val );
ViStatus _VI_FUNC Ag3352x_StateStorageGetValid ( ViSession Vi, ViInt32 Location, ViBoolean* RetVal );
ViStatus _VI_FUNC Ag3352x_StateStorageGetCatalog ( ViSession Vi, ViInt32 retValBufferSize, ViChar retVal[] );

/*- Memory */

ViStatus _VI_FUNC Ag3352x_MemoryDelete ( ViSession Vi, ViConstString FileName );
ViStatus _VI_FUNC Ag3352x_MemoryLoad ( ViSession Vi, ViConstString FileName );
ViStatus _VI_FUNC Ag3352x_MemoryMakeDirectory ( ViSession Vi, ViConstString DirectoryName );
ViStatus _VI_FUNC Ag3352x_MemoryMoveFiles ( ViSession Vi, ViConstString SourceFile, ViConstString DestinationFile );
ViStatus _VI_FUNC Ag3352x_MemoryStore ( ViSession Vi, ViConstString FileName );
ViStatus _VI_FUNC Ag3352x_MemoryLoadState ( ViSession Vi, ViConstString FileName );
ViStatus _VI_FUNC Ag3352x_MemoryRemoveFolder ( ViSession Vi, ViConstString Foldername );
ViStatus _VI_FUNC Ag3352x_MemoryCopyFiles ( ViSession Vi, ViConstString SourceFile, ViConstString DestinationFile );
ViStatus _VI_FUNC Ag3352x_MemoryCopySeqFiles ( ViSession Vi, ViConstString Source, ViConstString Destination );
ViStatus _VI_FUNC Ag3352x_MemoryStoreState ( ViSession Vi, ViConstString FileName );
ViStatus _VI_FUNC Ag3352x_MemoryUploadFile ( ViSession Vi, ViConstString Source, ViConstString Destination );
ViStatus _VI_FUNC Ag3352x_MemoryLoadList ( ViSession Vi, ViConstString FileName );
ViStatus _VI_FUNC Ag3352x_MemoryStoreList ( ViSession Vi, ViConstString FileName );
ViStatus _VI_FUNC Ag3352x_MemoryDownloadFile ( ViSession Vi, ViConstString Source, ViConstString Destination );
ViStatus _VI_FUNC Ag3352x_MemoryGetCatalog ( ViSession Vi, ViConstString FolderName, ViReal64* MemoryUsed, ViReal64* MemoryFree, ViInt32 RetValBufferSize, ViChar RetVal[] );
ViStatus _VI_FUNC Ag3352x_MemoryGetCatalogDataFiles ( ViSession Vi, ViConstString FolderName, ViReal64* MemoryUsed, ViReal64* MemoryFree, ViInt32 RetValBufferSize, ViChar RetVal[] );
ViStatus _VI_FUNC Ag3352x_MemoryGetCatalogStateFiles ( ViSession Vi, ViConstString FolderName, ViReal64* MemoryUsed, ViReal64* MemoryFree, ViInt32 RetValBufferSize, ViChar RetVal[] );

/*- Calibration */

ViStatus _VI_FUNC Ag3352x_SetSecureState ( ViSession Vi, ViBoolean Mode, ViConstString Code );
ViStatus _VI_FUNC Ag3352x_Calibrate ( ViSession Vi, ViInt32* RetVal );

/*- Status */

ViStatus _VI_FUNC Ag3352x_StatusPreset ( ViSession Vi );
ViStatus _VI_FUNC Ag3352x_StatusConfigureServiceRequest ( ViSession Vi, ViInt32 Reason );
ViStatus _VI_FUNC Ag3352x_StatusClear ( ViSession Vi );
ViStatus _VI_FUNC Ag3352x_StatusGetRegister ( ViSession Vi, ViInt32 Register, ViInt32 SubRegister, ViInt32* RetVal );
ViStatus _VI_FUNC Ag3352x_StatusSetRegister ( ViSession Vi, ViInt32 Register, ViInt32 SubRegister, ViInt32 Val );
ViStatus _VI_FUNC Ag3352x_StatusGetSerialPoll ( ViSession Vi, ViInt32* RetVal );

/*- System */

ViStatus _VI_FUNC Ag3352x_DisableLocalControls ( ViSession Vi );
ViStatus _VI_FUNC Ag3352x_EnableLocalControls ( ViSession Vi );
ViStatus _VI_FUNC Ag3352x_GetState ( ViSession Vi, ViInt32 DataBufferSize, ViChar Data[], ViInt32* DataActualSize );
ViStatus _VI_FUNC Ag3352x_PutState ( ViSession Vi, ViInt32 DataBufferSize, ViChar Data[] );
ViStatus _VI_FUNC Ag3352x_WaitForOperationComplete ( ViSession Vi, ViInt32 MaxTimeMilliseconds );
ViStatus _VI_FUNC Ag3352x_IoRead ( ViSession Vi, ViInt32 ValueBufferSize, ViChar Value[], ViInt32* ActualSize );
ViStatus _VI_FUNC Ag3352x_IoWrite ( ViSession Vi, ViConstString Value );


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
#define IVIC_LXISYNC_ERROR_BASE                  (IVIC_ERROR_BASE + 0x2000)

#define IVIC_ERROR_INVALID_ATTRIBUTE             (IVIC_ERROR_BASE + 0x000C)
#define IVIC_ERROR_TYPES_DO_NOT_MATCH            (IVIC_ERROR_BASE + 0x0015)
#define IVIC_ERROR_IVI_ATTR_NOT_WRITABLE         (IVIC_ERROR_BASE + 0x000D)
#define IVIC_ERROR_IVI_ATTR_NOT_READABLE         (IVIC_ERROR_BASE + 0x000E)
#define IVIC_ERROR_INVALID_SESSION_HANDLE        (IVIC_ERROR_BASE + 0x1190)

#endif


#define AG3352X_ERROR_CANNOT_RECOVER                        (IVIC_ERROR_BASE + 0x0000)
#define AG3352X_ERROR_INSTRUMENT_STATUS                     (IVIC_ERROR_BASE + 0x0001)
#define AG3352X_ERROR_CANNOT_OPEN_FILE                      (IVIC_ERROR_BASE + 0x0002)
#define AG3352X_ERROR_READING_FILE                          (IVIC_ERROR_BASE + 0x0003)
#define AG3352X_ERROR_WRITING_FILE                          (IVIC_ERROR_BASE + 0x0004)
#define AG3352X_ERROR_INVALID_PATHNAME                      (IVIC_ERROR_BASE + 0x000B)
#define AG3352X_ERROR_INVALID_VALUE                         (IVIC_ERROR_BASE + 0x0010)
#define AG3352X_ERROR_FUNCTION_NOT_SUPPORTED                (IVIC_ERROR_BASE + 0x0011)
#define AG3352X_ERROR_ATTRIBUTE_NOT_SUPPORTED               (IVIC_ERROR_BASE + 0x0012)
#define AG3352X_ERROR_VALUE_NOT_SUPPORTED                   (IVIC_ERROR_BASE + 0x0013)
#define AG3352X_ERROR_NOT_INITIALIZED                       (IVIC_ERROR_BASE + 0x001D)
#define AG3352X_ERROR_UNKNOWN_CHANNEL_NAME                  (IVIC_ERROR_BASE + 0x0020)
#define AG3352X_ERROR_TOO_MANY_OPEN_FILES                   (IVIC_ERROR_BASE + 0x0023)
#define AG3352X_ERROR_CHANNEL_NAME_REQUIRED                 (IVIC_ERROR_BASE + 0x0044)
#define AG3352X_ERROR_MISSING_OPTION_NAME                   (IVIC_ERROR_BASE + 0x0049)
#define AG3352X_ERROR_MISSING_OPTION_VALUE                  (IVIC_ERROR_BASE + 0x004A)
#define AG3352X_ERROR_BAD_OPTION_NAME                       (IVIC_ERROR_BASE + 0x004B)
#define AG3352X_ERROR_BAD_OPTION_VALUE                      (IVIC_ERROR_BASE + 0x004C)
#define AG3352X_ERROR_OUT_OF_MEMORY                         (IVIC_ERROR_BASE + 0x0056)
#define AG3352X_ERROR_OPERATION_PENDING                     (IVIC_ERROR_BASE + 0x0057)
#define AG3352X_ERROR_NULL_POINTER                          (IVIC_ERROR_BASE + 0x0058)
#define AG3352X_ERROR_UNEXPECTED_RESPONSE                   (IVIC_ERROR_BASE + 0x0059)
#define AG3352X_ERROR_FILE_NOT_FOUND                        (IVIC_ERROR_BASE + 0x005B)
#define AG3352X_ERROR_INVALID_FILE_FORMAT                   (IVIC_ERROR_BASE + 0x005C)
#define AG3352X_ERROR_STATUS_NOT_AVAILABLE                  (IVIC_ERROR_BASE + 0x005D)
#define AG3352X_ERROR_ID_QUERY_FAILED                       (IVIC_ERROR_BASE + 0x005E)
#define AG3352X_ERROR_RESET_FAILED                          (IVIC_ERROR_BASE + 0x005F)
#define AG3352X_ERROR_RESOURCE_UNKNOWN                      (IVIC_ERROR_BASE + 0x0060)
#define AG3352X_ERROR_ALREADY_INITIALIZED                   (IVIC_ERROR_BASE + 0x0061)
#define AG3352X_ERROR_CANNOT_CHANGE_SIMULATION_STATE        (IVIC_ERROR_BASE + 0x0062)
#define AG3352X_ERROR_INVALID_NUMBER_OF_LEVELS_IN_SELECTOR  (IVIC_ERROR_BASE + 0x0063)
#define AG3352X_ERROR_INVALID_RANGE_IN_SELECTOR             (IVIC_ERROR_BASE + 0x0064)
#define AG3352X_ERROR_UNKOWN_NAME_IN_SELECTOR               (IVIC_ERROR_BASE + 0x0065)
#define AG3352X_ERROR_BADLY_FORMED_SELECTOR                 (IVIC_ERROR_BASE + 0x0066)
#define AG3352X_ERROR_UNKNOWN_PHYSICAL_IDENTIFIER           (IVIC_ERROR_BASE + 0x0067)



#define AG3352X_SUCCESS                                     0
#define AG3352X_WARN_NSUP_ID_QUERY                          (IVIC_WARN_BASE + 0x0065)
#define AG3352X_WARN_NSUP_RESET                             (IVIC_WARN_BASE + 0x0066)
#define AG3352X_WARN_NSUP_SELF_TEST                         (IVIC_WARN_BASE + 0x0067)
#define AG3352X_WARN_NSUP_ERROR_QUERY                       (IVIC_WARN_BASE + 0x0068)
#define AG3352X_WARN_NSUP_REV_QUERY                         (IVIC_WARN_BASE + 0x0069)



#define AG3352X_ERROR_TRIGGER_NOT_SOFTWARE                  (IVIC_CROSS_CLASS_ERROR_BASE + 0x0001)
#define AG3352X_ERROR_NO_WFMS_AVAILABLE                     (IVIC_CLASS_ERROR_BASE + 0x0004)
#define AG3352X_ERROR_WFM_IN_USE                            (IVIC_CLASS_ERROR_BASE + 0x0008)
#define AG3352X_ERROR_NO_SEQS_AVAILABLE                     (IVIC_CLASS_ERROR_BASE + 0x0009)
#define AG3352X_ERROR_SEQ_IN_USE                            (IVIC_CLASS_ERROR_BASE + 0x000D)
#define AG3352X_ERROR_PERSONALITY_NOT_ACTIVE                (IVIC_SPECIFIC_ERROR_BASE + 0x0211)
#define AG3352X_ERROR_PERSONALITY_NOT_INSTALLED             (IVIC_SPECIFIC_ERROR_BASE + 0x0212)
#define AG3352X_ERROR_PERSONALITY_NOT_LICENSED              (IVIC_SPECIFIC_ERROR_BASE + 0x0213)
#define AG3352X_ERROR_IO_GENERAL                            (IVIC_SPECIFIC_ERROR_BASE + 0x0214)
#define AG3352X_ERROR_IO_TIMEOUT                            (IVIC_SPECIFIC_ERROR_BASE + 0x0215)
#define AG3352X_ERROR_MODEL_NOT_SUPPORTED                   (IVIC_SPECIFIC_ERROR_BASE + 0x0216)
#define AG3352X_ERROR_WRAPPED_DRIVER_ERROR                  (IVIC_SPECIFIC_ERROR_BASE + 0x0218)
#define AG3352X_ERROR_MAX_TIME_EXCEEDED                     (IVIC_SPECIFIC_ERROR_BASE + 0x0217)
#define AG3352X_ERROR_PROPERTY_NOT_SUPPORTED                (IVIC_SPECIFIC_ERROR_BASE + 0x0219)
#define AG3352X_ERROR_LICENSE_NOT_INSTALLED                 (IVIC_SPECIFIC_ERROR_BASE + 0x0219)
#define AG3352X_ERROR_NOT_SUPPORTED                         (IVIC_SPECIFIC_ERROR_BASE + 0x021A)




/**************************************************************************** 
 *---------------------------- End Include File ----------------------------* 
 ****************************************************************************/
#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif
#endif // __AG3352X_HEADER
