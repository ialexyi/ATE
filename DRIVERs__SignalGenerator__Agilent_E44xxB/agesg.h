/****************************************************************************
 *                       ESG Family Signal Generators
 *---------------------------------------------------------------------------
 *    Copyright (c) National Instruments 2002-2009.  All Rights Reserved.
 *---------------------------------------------------------------------------
 *
 * Title:       agesg.h
 * Purpose:     ESG Family Signal Generators instrument driver
 *              declarations for the Base and Extended.
 ****************************************************************************/

#ifndef AGESG_HEADER
#define AGESG_HEADER

#include <ivirfsiggen.h>

#ifdef _CVI_
#pragma EnableLibraryRuntimeChecking
#endif

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

/****************************************************************************
 *----------------- Instrument Driver Revision Information -----------------*
 ****************************************************************************/

#define AGESG_MAJOR_VERSION                2     /* Instrument driver major version   */
#define AGESG_MINOR_VERSION                2     /* Instrument driver minor version   */

#define AGESG_CLASS_SPEC_MAJOR_VERSION     1     /* Class specification major version */
#define AGESG_CLASS_SPEC_MINOR_VERSION     0     /* Class specification minor version */

#define AGESG_DRIVER_VENDOR              "National Instruments"
#define AGESG_DRIVER_DESCRIPTION         "ESG Family Signal Generators Instrument Driver"

#define AGESG_SUPPORTED_INSTRUMENT_MODELS "E4400B,E4420B,E4421B,E4422B,E4423B,E4424B,E4425B,E4426B,E4430B,E4431B,E4432B,E4433B,E4434B,E4435B,E4436B,E4437B,E4438C,E4428C"

/****************************************************************************
 *------------------------------ Useful Macros -----------------------------*
 ****************************************************************************/


/****************************************************************************
 *------------------ IviRFSigGen Class Attribute Defines ----------------------*
 ****************************************************************************/

    /*- IVI Inherent Attributes -*/
        /*- User Options -*/
#define AGESG_ATTR_CACHE                     IVI_ATTR_CACHE                         /* ViBoolean */
#define AGESG_ATTR_RANGE_CHECK               IVI_ATTR_RANGE_CHECK                   /* ViBoolean */
#define AGESG_ATTR_QUERY_INSTRUMENT_STATUS   IVI_ATTR_QUERY_INSTR_STATUS            /* ViBoolean */
#define AGESG_ATTR_RECORD_COERCIONS          IVI_ATTR_RECORD_COERCIONS              /* ViBoolean */
#define AGESG_ATTR_SIMULATE                  IVI_ATTR_SIMULATE                      /* ViBoolean */
#define AGESG_ATTR_INTERCHANGE_CHECK         IVI_ATTR_INTERCHANGE_CHECK             /* ViBoolean */
#define AGESG_ATTR_SPY                       IVI_ATTR_SPY                           /* ViBoolean */

        /*- Instrument Capabilities -*/
#define AGESG_ATTR_CHANNEL_COUNT             IVI_ATTR_NUM_CHANNELS                  /* ViInt32,  read-only */
#define AGESG_ATTR_GROUP_CAPABILITIES        IVI_ATTR_GROUP_CAPABILITIES            /* ViString, read-only */

        /*- Class Driver Information -*/
#define AGESG_ATTR_CLASS_DRIVER_PREFIX                         IVI_ATTR_CLASS_DRIVER_PREFIX                       /* ViString, read-only */
#define AGESG_ATTR_CLASS_DRIVER_VENDOR                         IVI_ATTR_CLASS_DRIVER_VENDOR                       /* ViString, read-only */
#define AGESG_ATTR_CLASS_DRIVER_DESCRIPTION                    IVI_ATTR_CLASS_DRIVER_DESCRIPTION                  /* ViString, read-only */
#define AGESG_ATTR_CLASS_DRIVER_CLASS_SPEC_MAJOR_VERSION       IVI_ATTR_CLASS_DRIVER_CLASS_SPEC_MAJOR_VERSION     /* ViInt32,  read-only */
#define AGESG_ATTR_CLASS_DRIVER_CLASS_SPEC_MINOR_VERSION       IVI_ATTR_CLASS_DRIVER_CLASS_SPEC_MINOR_VERSION     /* ViInt32,  read-only */

        /*- Specific Driver Information -*/
#define AGESG_ATTR_SPECIFIC_DRIVER_PREFIX                      IVI_ATTR_SPECIFIC_DRIVER_PREFIX                    /* ViString, read-only */
#define AGESG_ATTR_SPECIFIC_DRIVER_LOCATOR                     IVI_ATTR_SPECIFIC_DRIVER_LOCATOR                   /* ViString, read-only */
#define AGESG_ATTR_IO_RESOURCE_DESCRIPTOR                      IVI_ATTR_RESOURCE_DESCRIPTOR                       /* ViString, read-only */
#define AGESG_ATTR_LOGICAL_NAME                                IVI_ATTR_LOGICAL_NAME                              /* ViString, read-only */
#define AGESG_ATTR_SPECIFIC_DRIVER_VENDOR                      IVI_ATTR_SPECIFIC_DRIVER_VENDOR                    /* ViString, read-only */
#define AGESG_ATTR_SPECIFIC_DRIVER_DESCRIPTION                 IVI_ATTR_SPECIFIC_DRIVER_DESCRIPTION               /* ViString, read-only */
#define AGESG_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION    IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION  /* ViInt32,  read-only */
#define AGESG_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION    IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION  /* ViInt32,  read-only */

        /*- Instrument Information -*/
#define AGESG_ATTR_INSTRUMENT_FIRMWARE_REVISION     IVI_ATTR_INSTRUMENT_FIRMWARE_REVISION   /* ViString, read-only */
#define AGESG_ATTR_INSTRUMENT_MANUFACTURER          IVI_ATTR_INSTRUMENT_MANUFACTURER        /* ViString, read-only */
#define AGESG_ATTR_INSTRUMENT_MODEL                 IVI_ATTR_INSTRUMENT_MODEL               /* ViString, read-only */
#define AGESG_ATTR_SUPPORTED_INSTRUMENT_MODELS      IVI_ATTR_SUPPORTED_INSTRUMENT_MODELS    /* ViString, read-only */
#define AGESG_ATTR_IO_SESSION_TYPE                  IVI_ATTR_IO_SESSION_TYPE                /* ViString, read-only */
#define AGESG_ATTR_IO_SESSION                       IVI_ATTR_IO_SESSION                     /* ViString, read-only */

        /*- Version Information -*/
#define AGESG_ATTR_CLASS_DRIVER_MAJOR_VERSION       IVI_ATTR_CLASS_DRIVER_MAJOR_VERSION     /* ViInt32,  read-only */
#define AGESG_ATTR_CLASS_DRIVER_MINOR_VERSION       IVI_ATTR_CLASS_DRIVER_MINOR_VERSION     /* ViInt32,  read-only */
#define AGESG_ATTR_CLASS_DRIVER_REVISION            IVI_ATTR_CLASS_DRIVER_REVISION          /* ViString, read-only */

#define AGESG_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION    IVI_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION  /* ViInt32,  read-only */
#define AGESG_ATTR_SPECIFIC_DRIVER_MINOR_VERSION    IVI_ATTR_SPECIFIC_DRIVER_MINOR_VERSION  /* ViInt32,  read-only */
#define AGESG_ATTR_SPECIFIC_DRIVER_REVISION         IVI_ATTR_SPECIFIC_DRIVER_REVISION       /* ViString, read-only */

        /*- Error Info -*/
#define AGESG_ATTR_PRIMARY_ERROR                    IVI_ATTR_PRIMARY_ERROR                  /* ViInt32  */
#define AGESG_ATTR_SECONDARY_ERROR                  IVI_ATTR_SECONDARY_ERROR                /* ViInt32  */
#define AGESG_ATTR_ERROR_ELABORATION                IVI_ATTR_ERROR_ELABORATION              /* ViString */

        /*- Driver Setup information -*/
#define AGESG_ATTR_DRIVER_SETUP                     IVI_ATTR_DRIVER_SETUP                   /* ViString */

    /*- IviRFSigGen Fundamental Attributes -*/
#define AGESG_ATTR_FREQUENCY                                  IVIRFSIGGEN_ATTR_FREQUENCY         /* ViReal64 */
#define AGESG_ATTR_POWER_LEVEL                                IVIRFSIGGEN_ATTR_POWER_LEVEL       /* ViReal64 */
#define AGESG_ATTR_ALC_ENABLED                                IVIRFSIGGEN_ATTR_ALC_ENABLED       /* ViBoolean */
#define AGESG_ATTR_OUTPUT_ENABLED                             IVIRFSIGGEN_ATTR_OUTPUT_ENABLED    /* ViBoolean */
#define AGESG_ATTR_FREQUENCY_MULTIPLIER_CW					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 368L)/* ViReal64 */
#define AGESG_ATTR_FREQUENCY_OFFSET							  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 31L)	/* ViReal64 */
#define AGESG_ATTR_FREQUENCY_OFFSET_ENABLED					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 383L)/* ViBoolean */
#define AGESG_ATTR_FREQUENCY_OPTIMIZATION					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 32L)	/* ViInt32 */
#define AGESG_ATTR_FREQUENCY_REFERENCE						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 33L)	/* ViReal64 */
#define AGESG_ATTR_FREQUENCY_REFERENCE_ENABLED				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 34L)	/* ViBoolean */
#define AGESG_ATTR_PHASE_ADJUSTMENT							  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 35L)	/* ViReal64 */
#define AGESG_ATTR_OUTPUT_MODULATION_ENABLED				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 40L)	/* ViBoolean */

#define AGESG_ATTR_ALTERNATE_AMPLITUDE_ENABLED				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 41L) /* ViBoolean */
#define AGESG_ATTR_ALTERNATE_AMPLITUDE						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 42L) /* ViReal64 */
#define AGESG_ATTR_ALTERNATE_MANUAL_TRIGGER					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 43L) /* ViInt32 */
#define AGESG_ATTR_ALTERNATE_TRIGGER_SOURCE					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 44L) /* ViInt32 */

#define AGESG_ATTR_AUTOMATIC_ATTENUATION_ENABLED			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 45L) /* ViBoolean */
#define AGESG_ATTR_POWER_OFFSET								  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 48L)	/* ViReal64 */
#define AGESG_ATTR_POWER_REFERENCE							  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 49L)	/* ViReal64 */
#define AGESG_ATTR_POWER_REFERENCE_ENABLED					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 50L)	/* ViBoolean */

#define AGESG_ATTR_POWER_ALC_SEARCH_SPAN_START				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 384L)	/* ViReal64 */
#define AGESG_ATTR_POWER_ALC_SEARCH_SPAN_STOP				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 385L)	/* ViReal64 */
#define AGESG_ATTR_POWER_ALC_SEARCH_SPAN_TYPE				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 386L)	/* ViInt32 */
#define AGESG_ATTR_POWER_ALC_SEARCH_SPAN_ENABLED			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 387L)	/* ViBoolean */


    /*- IviRFSigGen Extended Attributes -*/
        /*- IviRFSigGenModulateAM Extension Group -*/
#define AGESG_ATTR_ACTIVE_EXTERNAL_SOURCE					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 369L)  /* ViString */

#define AGESG_ATTR_AM_ENABLED                                 IVIRFSIGGEN_ATTR_AM_ENABLED             /* ViBoolean */
#define AGESG_ATTR_AM2_ENABLED                                (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 8L)    /* ViBoolean */

#define AGESG_ATTR_AM_SOURCE                                  IVIRFSIGGEN_ATTR_AM_SOURCE              /* ViString */
#define AGESG_ATTR_AM2_SOURCE                                 (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 10L)   /* ViString */

#define AGESG_ATTR_AM_SCALING                                 IVIRFSIGGEN_ATTR_AM_SCALING             /* ViInt32 */
#define AGESG_ATTR_AM2_SCALING                                (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 11L)   /* ViInt32 */

#define AGESG_ATTR_AM_EXTERNAL_COUPLING                       IVIRFSIGGEN_ATTR_AM_EXTERNAL_COUPLING   /* ViInt32 */
#define AGESG_ATTR_AM2_EXTERNAL_COUPLING                      (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 9L)    /* ViInt32 */

#define AGESG_ATTR_AM_NOMINAL_VOLTAGE                         IVIRFSIGGEN_ATTR_AM_NOMINAL_VOLTAGE     /* ViReal64 */

#define AGESG_ATTR_AM_DEPTH                                   IVIRFSIGGEN_ATTR_AM_DEPTH               /* ViReal64 */
#define AGESG_ATTR_AM2_DEPTH                                  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 12L)   /* ViReal64 */

#define AGESG_ATTR_AM_DEPTH_COUPLING						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 13L)   /* ViBoolean */
#define AGESG_ATTR_AM_WIDEBAND_ENABLED						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 39L)   /* ViBoolean */

        /*- IviRFSigGenModulateFM Extension Group -*/
#define AGESG_ATTR_FM_ENABLED                                 IVIRFSIGGEN_ATTR_FM_ENABLED             /* ViBoolean */
#define AGESG_ATTR_FM2_ENABLED                                (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 24L)   /* ViBoolean */

#define AGESG_ATTR_FM_SOURCE                                  IVIRFSIGGEN_ATTR_FM_SOURCE              /* ViString */
#define AGESG_ATTR_FM2_SOURCE                                 (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 22L)   /* ViString */

#define AGESG_ATTR_FM_EXTERNAL_COUPLING                       IVIRFSIGGEN_ATTR_FM_EXTERNAL_COUPLING   /* ViInt32 */
#define AGESG_ATTR_FM2_EXTERNAL_COUPLING                      (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 21L)   /* ViInt32 */

#define AGESG_ATTR_FM_NOMINAL_VOLTAGE                         IVIRFSIGGEN_ATTR_FM_NOMINAL_VOLTAGE     /* ViReal64 */
#define AGESG_ATTR_FM_DEVIATION                               IVIRFSIGGEN_ATTR_FM_DEVIATION           /* ViReal64 */
#define AGESG_ATTR_FM2_DEVIATION                              (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 23L)   /* ViReal64 */

#define AGESG_ATTR_FM_DEVIATION_COUPLING					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 25L)   /* ViBoolean */

        /*- IviRFSigGenModulatePM Extension Group -*/
#define AGESG_ATTR_PM_ENABLED                                 IVIRFSIGGEN_ATTR_PM_ENABLED             /* ViBoolean */
#define AGESG_ATTR_PM2_ENABLED                                (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 14L)   /* ViBoolean */

#define AGESG_ATTR_PM_SOURCE                                  IVIRFSIGGEN_ATTR_PM_SOURCE              /* ViString */
#define AGESG_ATTR_PM2_SOURCE                                 (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 26L)   /* ViString */

#define AGESG_ATTR_PM_EXTERNAL_COUPLING                       IVIRFSIGGEN_ATTR_PM_EXTERNAL_COUPLING   /* ViInt32 */
#define AGESG_ATTR_PM2_EXTERNAL_COUPLING                      (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 29L)   /* ViInt32 */

#define AGESG_ATTR_PM_NOMINAL_VOLTAGE                         IVIRFSIGGEN_ATTR_PM_NOMINAL_VOLTAGE     /* ViReal64 */

#define AGESG_ATTR_PM_DEVIATION                               IVIRFSIGGEN_ATTR_PM_DEVIATION           /* ViReal64 */
#define AGESG_ATTR_PM2_DEVIATION                              (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 27L)   /* ViReal64 */

#define AGESG_ATTR_PM_BANDWIDTH                               (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 1L)    /* ViInt32 */
#define AGESG_ATTR_PM_DEVIATION_COUPLING					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 28L)   /* ViBoolean */

        /*- IviRFSigGenAnalogModulationSource Extension Group -*/
#define AGESG_ATTR_ANALOG_MODULATION_SOURCE_COUNT             IVIRFSIGGEN_ATTR_ANALOG_MODULATION_SOURCE_COUNT    /* ViInt32 */

        /*- IviRFSigGenModulatePulse Extension Group -*/
#define AGESG_ATTR_PULSE_MODULATION_ENABLED                   IVIRFSIGGEN_ATTR_PULSE_MODULATION_ENABLED             /* ViBoolean */
#define AGESG_ATTR_PULSE_MODULATION_SOURCE                    IVIRFSIGGEN_ATTR_PULSE_MODULATION_SOURCE              /* ViString */
#define AGESG_ATTR_PULSE_MODULATION_EXTERNAL_POLARITY         IVIRFSIGGEN_ATTR_PULSE_MODULATION_EXTERNAL_POLARITY   /* ViInt32 */
#define AGESG_ATTR_FAST_PULSE_MODULATION_ENABLED              (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 38L)		            /* ViBoolean */

#define AGESG_ATTR_PULSE_WAVEFORM							  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 56L)		            /* ViInt32 */
#define AGESG_ATTR_PULSE_FREQUENCY							  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 57L)		            /* ViReal64 */

        /*- IviRFSigGenLFGenerator Extension Group -*/
#define AGESG_ATTR_ACTIVE_LF_GENERATOR                        IVIRFSIGGEN_ATTR_ACTIVE_LF_GENERATOR     /* ViString */
#define AGESG_ATTR_LF_GENERATOR_COUNT                         IVIRFSIGGEN_ATTR_LF_GENERATOR_COUNT      /* ViInt32 */
#define AGESG_ATTR_LF_GENERATOR_FREQUENCY                     IVIRFSIGGEN_ATTR_LF_GENERATOR_FREQUENCY  /* ViReal64 */
#define AGESG_ATTR_LF_GENERATOR_WAVEFORM                      IVIRFSIGGEN_ATTR_LF_GENERATOR_WAVEFORM   /* ViInt32 */
#define AGESG_ATTR_LF_ALTERNATE_FREQUENCY      				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 15L)	   /* ViReal64 */
#define AGESG_ATTR_LF_ALTERNATE_AMPLITUDE      				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 16L)	   /* ViReal64 */
#define AGESG_ATTR_LF_SWEEP_TIME							  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 17L)	   /* ViReal64 */
#define AGESG_ATTR_LF_SWEEP_TRIGGER_SOURCE					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 18L)	   /* Viint32 */
#define AGESG_ATTR_LF_GENERATOR_WIDTH						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 37L)	   /* ViReal64 */
#define AGESG_ATTR_LF_GENERATOR_PERIOD						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 36L)	   /* ViReal64 */

        /*- IviRFSigGenLFGeneratorOutput Extension Group -*/
#define AGESG_ATTR_LF_GENERATOR_OUTPUT_AMPLITUDE              IVIRFSIGGEN_ATTR_LF_GENERATOR_OUTPUT_AMPLITUDE  /* ViReal64 */
#define AGESG_ATTR_LF_GENERATOR_OUTPUT_ENABLED                IVIRFSIGGEN_ATTR_LF_GENERATOR_OUTPUT_ENABLED    /* ViBoolean */

        /*- IviRFSigGenPulseGenerator Extension Group -*/
#define AGESG_ATTR_PULSE_INTERNAL_TRIGGER_PERIOD              IVIRFSIGGEN_ATTR_PULSE_INTERNAL_TRIGGER_PERIOD  /* ViReal64 */
#define AGESG_ATTR_PULSE_WIDTH                                IVIRFSIGGEN_ATTR_PULSE_WIDTH                    /* ViReal64 */
#define AGESG_ATTR_PULSE_GATING_ENABLED                       IVIRFSIGGEN_ATTR_PULSE_GATING_ENABLED           /* ViBoolean */
#define AGESG_ATTR_PULSE_TRIGGER_SOURCE                       IVIRFSIGGEN_ATTR_PULSE_TRIGGER_SOURCE           /* ViInt32 */
#define AGESG_ATTR_PULSE_EXTERNAL_TRIGGER_SLOPE               IVIRFSIGGEN_ATTR_PULSE_EXTERNAL_TRIGGER_SLOPE   /* ViInt32 */
#define AGESG_ATTR_PULSE_EXTERNAL_TRIGGER_DELAY               IVIRFSIGGEN_ATTR_PULSE_EXTERNAL_TRIGGER_DELAY   /* ViReal64 */

        /*- IviRFSigGenPulseDoubleGenerator Extension Group -*/

        /*- IviRFSigGenPulseGeneratorOutput Extension Group -*/
#define AGESG_ATTR_PULSE_OUTPUT_POLARITY                      IVIRFSIGGEN_ATTR_PULSE_OUTPUT_POLARITY  /* ViInt32 */
#define AGESG_ATTR_PULSE_OUTPUT_ENABLED                       IVIRFSIGGEN_ATTR_PULSE_OUTPUT_ENABLED   /* ViBoolean */

        /*- IviRFSigGenSweep Extension Group -*/
#define AGESG_ATTR_SWEEP_MODE                                 IVIRFSIGGEN_ATTR_SWEEP_MODE            /* ViInt32 */
#define AGESG_ATTR_SWEEP_TRIGGER_SOURCE                       IVIRFSIGGEN_ATTR_SWEEP_TRIGGER_SOURCE  /* ViInt32 */

        /*- IviRFSigGenFrequencySweep Extension Group -*/

        /*- IviRFSigGenPowerSweep Extension Group -*/

        /*- IviRFSigGenFrequencyStep Extension Group -*/
#define AGESG_ATTR_FREQUENCY_STEP_START                       IVIRFSIGGEN_ATTR_FREQUENCY_STEP_START                /* ViReal64 */
#define AGESG_ATTR_FREQUENCY_STEP_STOP                        IVIRFSIGGEN_ATTR_FREQUENCY_STEP_STOP                 /* ViReal64 */
#define AGESG_ATTR_FREQUENCY_STEP_SCALING                     IVIRFSIGGEN_ATTR_FREQUENCY_STEP_SCALING              /* ViInt32 */
#define AGESG_ATTR_FREQUENCY_STEP_SIZE                        IVIRFSIGGEN_ATTR_FREQUENCY_STEP_SIZE                 /* ViReal64 */
#define AGESG_ATTR_FREQUENCY_STEP_SINGLE_STEP_ENABLED         IVIRFSIGGEN_ATTR_FREQUENCY_STEP_SINGLE_STEP_ENABLED  /* ViBoolean */
#define AGESG_ATTR_FREQUENCY_STEP_DWELL                       IVIRFSIGGEN_ATTR_FREQUENCY_STEP_DWELL                /* ViReal64 */

        /*- IviRFSigGenPowerStep Extension Group -*/
#define AGESG_ATTR_POWER_STEP_START                           IVIRFSIGGEN_ATTR_POWER_STEP_START                /* ViReal64 */
#define AGESG_ATTR_POWER_STEP_STOP                            IVIRFSIGGEN_ATTR_POWER_STEP_STOP                 /* ViReal64 */
#define AGESG_ATTR_POWER_STEP_SIZE                            IVIRFSIGGEN_ATTR_POWER_STEP_SIZE                 /* ViReal64 */
#define AGESG_ATTR_POWER_STEP_SINGLE_STEP_ENABLED             IVIRFSIGGEN_ATTR_POWER_STEP_SINGLE_STEP_ENABLED  /* ViBoolean */
#define AGESG_ATTR_POWER_STEP_DWELL                           IVIRFSIGGEN_ATTR_POWER_STEP_DWELL                /* ViReal64 */

        /*- IviRFSigGenList Extension Group -*/
#define AGESG_ATTR_LIST_SELECTED_NAME                         IVIRFSIGGEN_ATTR_LIST_SELECTED_NAME        /* ViString */
#define AGESG_ATTR_LIST_SINGLE_STEP_ENABLED                   IVIRFSIGGEN_ATTR_LIST_SINGLE_STEP_ENABLED  /* ViBoolean */
#define AGESG_ATTR_LIST_DWELL                                 IVIRFSIGGEN_ATTR_LIST_DWELL                /* ViReal64 */

#define AGESG_ATTR_SWEEP_DIRECTION							  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 51L)		/* ViInt32 */
#define AGESG_ATTR_LIST_MODE								  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 52L)		/* ViInt32 */
#define AGESG_ATTR_LIST_MANUAL_POINT						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 53L)		/* ViInt32 */

#define AGESG_ATTR_LIST_FREQUENCY_NUM_POINTS				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 54L)		/* ViInt32, read-only */
#define AGESG_ATTR_LIST_POWER_NUM_POINTS					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 55L)		/* ViInt32, read-only */

        /*- IviRFSigGenALC Extension Group -*/
#define AGESG_ATTR_ALC_SOURCE                                 IVIRFSIGGEN_ATTR_ALC_SOURCE     			/* ViInt32 */
#define AGESG_ATTR_ALC_BANDWIDTH                              IVIRFSIGGEN_ATTR_ALC_BANDWIDTH  			/* ViReal64 */
#define AGESG_ATTR_ALC_SEARCH_ENABLED						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 46L)		/* ViBoolean */
#define AGESG_ATTR_ALC_SEARCH_REFERENCE						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 47L)		/* ViInt32 */

        /*- IviRFSigGenReferenceOscillator Extension Group -*/
#define AGESG_ATTR_REFERENCE_OSCILLATOR_SOURCE                IVIRFSIGGEN_ATTR_REFERENCE_OSCILLATOR_SOURCE              /* ViInt32 */
#define AGESG_ATTR_REFERENCE_OSCILLATOR_EXTERNAL_FREQUENCY    IVIRFSIGGEN_ATTR_REFERENCE_OSCILLATOR_EXTERNAL_FREQUENCY  /* ViReal64 */

        /*- IviRFSigGenModulateIQ Extension Group -*/
#define AGESG_ATTR_IQ_ENABLED                                 IVIRFSIGGEN_ATTR_IQ_ENABLED          /* ViBoolean */
#define AGESG_ATTR_IQ_NOMINAL_VOLTAGE                         IVIRFSIGGEN_ATTR_IQ_NOMINAL_VOLTAGE  /* ViReal64 */
#define AGESG_ATTR_IQ_SOURCE                                  IVIRFSIGGEN_ATTR_IQ_SOURCE           /* ViInt32 */
#define AGESG_ATTR_IQ_SWAP_ENABLED                            IVIRFSIGGEN_ATTR_IQ_SWAP_ENABLED     /* ViBoolean */
#define AGESG_ATTR_IQ_CALIBRATION_START						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 19L)	/* ViReal64 */
#define AGESG_ATTR_IQ_CALIBRATION_STOP						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 20L)	/* ViReal64 */

#define AGESG_ATTR_IQ_MODULATION_SOURCE						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 58L)	/* ViInt32 */
#define AGESG_ATTR_HIGH_CREST_ENABLED						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 59L)	/* ViBoolean */

        /*- IviRFSigGenIQImpairment Extension Group -*/
#define AGESG_ATTR_IQ_IMPAIRMENT_ENABLED                      IVIRFSIGGEN_ATTR_IQ_IMPAIRMENT_ENABLED  /* ViBoolean */
#define AGESG_ATTR_IQ_I_OFFSET                                IVIRFSIGGEN_ATTR_IQ_I_OFFSET            /* ViReal64 */
#define AGESG_ATTR_IQ_Q_OFFSET                                IVIRFSIGGEN_ATTR_IQ_Q_OFFSET            /* ViReal64 */
#define AGESG_ATTR_IQ_RATIO                                   IVIRFSIGGEN_ATTR_IQ_RATIO               /* ViReal64 */
#define AGESG_ATTR_IQ_SKEW                                    IVIRFSIGGEN_ATTR_IQ_SKEW                /* ViReal64 */

        /*- IviRFSigGenArbGenerator Extension Group -*/
#define AGESG_ATTR_ARB_SELECTED_WAVEFORM                      IVIRFSIGGEN_ATTR_ARB_SELECTED_WAVEFORM       /* ViString */
#define AGESG_ATTR_ARB_CLOCK_FREQUENCY                        IVIRFSIGGEN_ATTR_ARB_CLOCK_FREQUENCY         /* ViReal64 */
#define AGESG_ATTR_ARB_FILTER_FREQUENCY                       IVIRFSIGGEN_ATTR_ARB_FILTER_FREQUENCY        /* ViReal64 */
#define AGESG_ATTR_ARB_MAX_NUMBER_WAVEFORMS                   IVIRFSIGGEN_ATTR_ARB_MAX_NUMBER_WAVEFORMS    /* ViInt32 */
#define AGESG_ATTR_ARB_WAVEFORM_QUANTUM                       IVIRFSIGGEN_ATTR_ARB_WAVEFORM_QUANTUM        /* ViInt32 */
#define AGESG_ATTR_ARB_WAVEFORM_SIZE_MIN                      IVIRFSIGGEN_ATTR_ARB_WAVEFORM_SIZE_MIN       /* ViInt32 */
#define AGESG_ATTR_ARB_WAVEFORM_SIZE_MAX                      IVIRFSIGGEN_ATTR_ARB_WAVEFORM_SIZE_MAX       /* ViInt32 */
#define AGESG_ATTR_ARB_TRIGGER_SOURCE                         IVIRFSIGGEN_ATTR_ARB_TRIGGER_SOURCE          /* ViInt32 */
#define AGESG_ATTR_ARB_EXTERNAL_TRIGGER_SLOPE                 IVIRFSIGGEN_ATTR_ARB_EXTERNAL_TRIGGER_SLOPE  /* ViInt32 */

#define AGESG_ATTR_ARB_REFERENCE_SOURCE						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 329L)		/* ViInt32 */
#define AGESG_ATTR_ARB_REFERENCE_FREQUENCY					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 330L)		/* ViInt32 */

        /*- IviRFSigGenCDMABase Extension Group -*/
#define AGESG_ATTR_CDMA_STANDARD_COUNT                        IVIRFSIGGEN_ATTR_CDMA_STANDARD_COUNT          /* ViInt32 */
#define AGESG_ATTR_CDMA_SELECTED_STANDARD                     IVIRFSIGGEN_ATTR_CDMA_SELECTED_STANDARD       /* ViString */
#define AGESG_ATTR_CDMA_TRIGGER_SOURCE                        IVIRFSIGGEN_ATTR_CDMA_TRIGGER_SOURCE          /* ViInt32 */
#define AGESG_ATTR_CDMA_EXTERNAL_TRIGGER_SLOPE                IVIRFSIGGEN_ATTR_CDMA_EXTERNAL_TRIGGER_SLOPE  /* ViInt32 */
#define AGESG_ATTR_CDMA_TEST_MODEL_COUNT                      IVIRFSIGGEN_ATTR_CDMA_TEST_MODEL_COUNT        /* ViInt32 */
#define AGESG_ATTR_CDMA_SELECTED_TEST_MODEL                   IVIRFSIGGEN_ATTR_CDMA_SELECTED_TEST_MODEL     /* ViString */
#define AGESG_ATTR_CDMA_CLOCK_SOURCE                          IVIRFSIGGEN_ATTR_CDMA_CLOCK_SOURCE            /* ViInt32 */

#define AGESG_ATTR_CDMA_STANDARD_FILTER_USER_FILE			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 314L)		/* ViString */
#define AGESG_ATTR_CDMA_STANDARD_FILTER						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 315L)		/* ViInt32 */
#define AGESG_ATTR_CDMA_FILTER_ALPHA						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 316L)		/* ViReal64 */
#define AGESG_ATTR_CDMA_FILTER_BBT							  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 317L)		/* ViReal64 */
#define AGESG_ATTR_CDMA_OPTIMIZE_FIR						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 318L)		/* ViInt32 */
#define AGESG_ATTR_CDMA_EXTERNAL_DELAY_ENABLED				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 319L)		/* ViBoolean */
#define AGESG_ATTR_CDMA_EXTERNAL_DELAY_TIME					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 320L)		/* ViReal64 */
#define AGESG_ATTR_CDMA_RETRIGGER_MODE_ENABLED				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 321L)		/* ViBoolean */
#define AGESG_ATTR_CDMA_TRIGGER_GATE_ACTIVE					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 322L)		/* ViInt32 */

#define AGESG_ATTR_CDMA_SELECTED_CHANNEL_USER_FILE			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 323L)		/* ViString */
#define AGESG_ATTR_CDMA_SELECTED_CARRIER_USER_FILE			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 324L)		/* ViString */
#define AGESG_ATTR_CDMA_SELECTED_CHANNEL_TYPE				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 325L)		/* ViInt32 */

#define AGESG_ATTR_CDMA_IS95A_CHIP_RATE						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 326L)		/* ViReal64 */
#define AGESG_ATTR_CDMA_IS95A_WAVEFORM_LENGTH				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 327L)		/* ViInt32 */
#define AGESG_ATTR_CDMA_IS95A_OVERSAMPLE_RATIO				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 328L)		/* ViInt32 */

#define AGESG_ATTR_CDMA_CLIPPING_TYPE						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 331L)		/* ViInt32 */
#define AGESG_ATTR_CDMA_CLIPPING_LEVEL_IJQ					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 332L)		/* ViReal64 */
#define AGESG_ATTR_CDMA_CLIPPING_LEVEL_I					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 333L)		/* ViReal64 */
#define AGESG_ATTR_CDMA_CLIPPING_LEVEL_Q					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 334L)		/* ViReal64 */
#define AGESG_ATTR_CDMA_CLIPPING_FIR_FILTER					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 335L)		/* ViInt32 */

#define AGESG_ATTR_WCDMA_SELECTED_FORWARD_USER_FILE			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 336L)		/* ViString */
#define AGESG_ATTR_WCDMA_SELECTED_REVERSE_USER_FILE			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 337L)		/* ViString */
#define AGESG_ATTR_WCDMA_SELECTED_FORWARD_MULTI_USER_FILE	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 338L)		/* ViString */

#define AGESG_ATTR_WCDMA_SELECTED_CHANNEL_TYPE				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 339L)		/* ViInt32 */
#define AGESG_ATTR_WCDMA_STANDARD_CHIP_RATE					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 340L)		/* ViInt32 */
#define AGESG_ATTR_WCDMA_STANDARD_FORWARD_TFCI_ENABLED		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 341L)		/* ViBoolean */
#define AGESG_ATTR_WCDMA_STANDARD_CODE_DOMAIN				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 342L)		/* ViInt32 */

#define AGESG_ATTR_WCDMA_STANDARD_SCRAMBLE_CODE				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 343L)		/* ViString */
#define AGESG_ATTR_WCDMA_STANDARD_SECOND_DPDCH				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 344L)		/* ViInt32 */
#define AGESG_ATTR_WCDMA_STANDARD_IQ_MAPPING				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 345L)		/* ViInt32 */
#define AGESG_ATTR_WCDMA_STANDARD_REVERSE_TFCI_ENABLED		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 346L)		/* ViBoolean */

#define AGESG_ATTR_WCDMA_TGPP_DOWN_USER_FILE				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 347L)		/* ViString */
#define AGESG_ATTR_WCDMA_TGPP_UP_USER_FILE					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 348L)		/* ViString */
#define AGESG_ATTR_WCDMA_TGPP_DOWN_MULTI_USER_FILE			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 349L)		/* ViString */

#define AGESG_ATTR_WCDMA_TGPP_SELECTED_CHANNEL_TYPE			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 350L)		/* ViInt32 */
#define AGESG_ATTR_WCDMA_TGPP_CHIP_RATE						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 351L)		/* ViReal64 */
#define AGESG_ATTR_WCDMA_TGPP_DOWN_TFCI_ENABLED				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 352L)		/* ViBoolean */
#define AGESG_ATTR_WCDMA_TGPP_UP_TFCI_ENABLED				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 353L)		/* ViBoolean */
#define AGESG_ATTR_WCDMA_TGPP_CODE_DOMAIN					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 354L)		/* ViInt32 */
#define AGESG_ATTR_WCDMA_TGPP_SCRAMBLE_CODE					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 355L)		/* ViString */
#define AGESG_ATTR_WCDMA_TGPP_SECOND_DPDCH					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 356L)		/* ViInt32 */
#define AGESG_ATTR_WCDMA_TGPP_GAIN_UNIT						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 357L)		/* ViInt32 */
#define AGESG_ATTR_WCDMA_TGPP_IQ_MAPPING					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 358L)		/* ViInt32 */

#define AGESG_ATTR_CDMA2000_SELECTED_FORWARD_USER_FILE		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 359L)		/* ViString */
#define AGESG_ATTR_CDMA2000_SELECTED_REVERSE_USER_FILE		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 360L)		/* ViString */
#define AGESG_ATTR_CDMA2000_SELECTED_FORWARD_MULTI_USER_FILE  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 361L)		/* ViString */
#define AGESG_ATTR_CDMA2000_SELECTED_CHANNEL_TYPE			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 362L)		/* ViInt32 */
#define AGESG_ATTR_CDMA2000_MULTICARRIER_SPACING			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 363L)		/* ViReal64 */
#define AGESG_ATTR_CDMA2000_IQ_MAPPING						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 364L)		/* ViInt32 */
#define AGESG_ATTR_CDMA2000_FORWARD_CODE_DOMAIN				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 365L)		/* ViInt32 */
#define AGESG_ATTR_CDMA2000_REVERSE_CODE_DOMAIN				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 366L)		/* ViInt32 */


        /*- IviRFSigGenTDMABase Extension Group -*/
#define AGESG_ATTR_TDMA_STANDARD_COUNT                        IVIRFSIGGEN_ATTR_TDMA_STANDARD_COUNT          /* ViInt32 */
#define AGESG_ATTR_TDMA_SELECTED_STANDARD                     IVIRFSIGGEN_ATTR_TDMA_SELECTED_STANDARD       /* ViString */
#define AGESG_ATTR_TDMA_TRIGGER_SOURCE                        IVIRFSIGGEN_ATTR_TDMA_TRIGGER_SOURCE          /* ViInt32 */
#define AGESG_ATTR_TDMA_EXTERNAL_TRIGGER_SLOPE                IVIRFSIGGEN_ATTR_TDMA_EXTERNAL_TRIGGER_SLOPE  /* ViInt32 */
#define AGESG_ATTR_TDMA_FRAME_COUNT                           IVIRFSIGGEN_ATTR_TDMA_FRAME_COUNT             /* ViInt32 */
#define AGESG_ATTR_TDMA_SELECTED_FRAME                        IVIRFSIGGEN_ATTR_TDMA_SELECTED_FRAME          /* ViString */
#define AGESG_ATTR_TDMA_CLOCK_SOURCE                          IVIRFSIGGEN_ATTR_TDMA_CLOCK_SOURCE            /* ViInt32 */
#define AGESG_ATTR_TDMA_EXTERNAL_CLOCK_TYPE                   IVIRFSIGGEN_ATTR_TDMA_EXTERNAL_CLOCK_TYPE     /* ViInt32 */

#define AGESG_ATTR_SWEEP_STEP_NUM_POINTS					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 2L)			/* ViInt32 */
#define AGESG_ATTR_IQ_BURST_ENVELOPE_ENABLED 				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 3L)			/* ViBoolean */
#define AGESG_ATTR_IQ_BURST_SOURCE			 				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 4L)			/* ViInt32 */
#define AGESG_ATTR_ARB_TRIGGER_TYPE         				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 5L)			/* ViInt32 */
#define AGESG_ATTR_INSTRUMENT_OPTION_STRING 				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 6L)			/* ViString */
#define AGESG_ATTR_SWEEP_REPEAT		         				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 7L)			/* ViInt32 */

#define AGESG_ATTR_BURST_GATE_INPUT_POLARITY				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 60L)			/* ViInt32 */
#define AGESG_ATTR_DATA_CLOCK_INPUT_POLARITY				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 61L)			/* ViInt32 */
#define AGESG_ATTR_DATA_CLOCK_OUTPUT_POLARITY				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 62L)			/* ViInt32 */
#define AGESG_ATTR_DATA_INPUT_POLARITY						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 63L)			/* ViInt32 */
#define AGESG_ATTR_DATA_OUTPUT_POLARITY						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 64L)			/* ViInt32 */
#define AGESG_ATTR_EVENT_OUTPUT_POLARITY					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 65L)			/* ViInt32, repeated-capability */
#define AGESG_ATTR_PATTERN_TRIGGER_INPUT_POLARITY			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 66L)			/* ViInt32 */
#define AGESG_ATTR_SYMBOL_SYNC_INPUT_POLARITY				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 67L)			/* ViInt32 */
#define AGESG_ATTR_SYMBOL_SYNC_OUTPUT_POLARITY				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 68L)			/* ViInt32 */
#define AGESG_ATTR_TRIGGER_OUTPUT_POLARITY					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 69L)			/* ViInt32 */
#define AGESG_ATTR_CONNECTORS_ENABLED						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 70L)			/* ViBoolean */

#define AGESG_ATTR_TDMA_BURST_ENABLED						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 139L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_UNFRAMED_DATA						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 135L)		/* ViInt32, repeated-capability */
#define AGESG_ATTR_TDMA_UNFRAMED_DATA_USER_FILE				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 367L)		/* ViString, repeated-capability */
#define AGESG_ATTR_TDMA_UNFRAMED_DATA_FIX4					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 136L)		/* ViInt32, repeated-capability */
#define AGESG_ATTR_TDMA_EXTERNAL_TRIGGER_DELAY_BITS			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 137L)		/* ViInt32, repeated-capability */
#define AGESG_ATTR_TDMA_EXTERNAL_TRIGGER_DELAY_ENABLED		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 138L)		/* ViBoolean, repeated-capability */

#define AGESG_ATTR_TDMA_DECT_PP_TYPE						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 85L)			/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_PP_TIMESLOT_ENABLED			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 71L)			/* ViBoolean */

#define AGESG_ATTR_TDMA_DECT_PP_LC_PREAMBLE					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 72L)			/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_PP_LC_SYNCHRONIZATION_WORD		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 73L)			/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_PP_LC_FILE_NAME				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 74L)			/* ViString */
#define AGESG_ATTR_TDMA_DECT_PP_LC_B_FIELD					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 75L)			/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_PP_LC_FIX4						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 76L)			/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_PP_LC_A_FIELD					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 77L)			/* ViInt32 */

#define AGESG_ATTR_TDMA_DECT_PP_LCZ_PREAMBLE				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 123L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_PP_LCZ_SYNCHRONIZATION_WORD	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 124L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_PP_LCZ_FILE_NAME				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 125L)		/* ViString */
#define AGESG_ATTR_TDMA_DECT_PP_LCZ_B_FIELD					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 126L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_PP_LCZ_FIX4					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 127L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_PP_LCZ_A_FIELD					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 128L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_DECT_PP_TB_PREAMBLE					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 87L)			/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_PP_TB_SYNCHRONIZATION_WORD		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 88L)			/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_PP_TB_FILE_NAME				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 89L)			/* ViString */
#define AGESG_ATTR_TDMA_DECT_PP_TB_B_FIELD					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 90L)			/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_PP_TB_FIX4						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 91L)			/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_PP_TB_A_FIELD					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 92L)			/* ViInt32 */

#define AGESG_ATTR_TDMA_DECT_PP_TBZ_PREAMBLE				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 111L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_PP_TBZ_SYNCHRONIZATION_WORD	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 112L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_PP_TBZ_FILE_NAME				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 113L)		/* ViString */
#define AGESG_ATTR_TDMA_DECT_PP_TBZ_B_FIELD					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 114L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_PP_TBZ_FIX4					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 115L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_PP_TBZ_A_FIELD					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 116L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_DECT_RFP_TYPE						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 86L)			/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_RFP_TIMESLOT_ENABLED			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 78L)			/* ViBoolean */

#define AGESG_ATTR_TDMA_DECT_RFP_LC_PREAMBLE				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 79L)			/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_RFP_LC_SYNCHRONIZATION_WORD	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 80L)			/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_RFP_LC_FILE_NAME				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 81L)			/* ViString */
#define AGESG_ATTR_TDMA_DECT_RFP_LC_B_FIELD					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 82L)			/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_RFP_LC_FIX4					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 83L)			/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_RFP_LC_A_FIELD					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 84L)			/* ViInt32 */

#define AGESG_ATTR_TDMA_DECT_RFP_LCZ_PREAMBLE				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 117L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_RFP_LCZ_SYNCHRONIZATION_WORD	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 118L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_RFP_LCZ_FILE_NAME				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 119L)		/* ViString */
#define AGESG_ATTR_TDMA_DECT_RFP_LCZ_B_FIELD				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 120L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_RFP_LCZ_FIX4					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 121L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_RFP_LCZ_A_FIELD				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 122L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_DECT_RFP_TB_PREAMBLE				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 93L)			/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_RFP_TB_SYNCHRONIZATION_WORD	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 94L)			/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_RFP_TB_FILE_NAME				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 95L)			/* ViString */
#define AGESG_ATTR_TDMA_DECT_RFP_TB_B_FIELD					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 96L)			/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_RFP_TB_FIX4					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 97L)			/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_RFP_TB_A_FIELD					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 98L)			/* ViInt32 */

#define AGESG_ATTR_TDMA_DECT_RFP_TBZ_PREAMBLE				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 105L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_RFP_TBZ_SYNCHRONIZATION_WORD	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 106L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_RFP_TBZ_FILE_NAME				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 107L)		/* ViString */
#define AGESG_ATTR_TDMA_DECT_RFP_TBZ_B_FIELD				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 108L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_RFP_TBZ_FIX4					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 109L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_RFP_TBZ_A_FIELD				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 110L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_DECT_RFP_CUSTOM_FILE_NAME			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 129L)		/* ViString */
#define AGESG_ATTR_TDMA_DECT_RFP_CUSTOM_DATA				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 130L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_RFP_CUSTOM_FIX4				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 131L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_DECT_PP_CUSTOM_FILE_NAME			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 132L)		/* ViString */
#define AGESG_ATTR_TDMA_DECT_PP_CUSTOM_DATA					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 133L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_PP_CUSTOM_FIX4					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 134L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_DECT_RFP_DB1_PREAMBLE				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 99L)			/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_RFP_DB1_SYNCHRONIZATION_WORD	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 100L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_RFP_DB1_A_FIELD				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 101L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_DECT_RFP_DB2_PREAMBLE				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 102L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_RFP_DB2_SYNCHRONIZATION_WORD	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 103L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_DECT_RFP_DB2_A_FIELD				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 104L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_PN9_MODE							  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 140L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_SECONDARY_FRAME_TRIGGER				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 141L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_SECONDARY_FRAME_ENABLED				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 142L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_FREQUENCY_CHANNEL_ENABLED			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 143L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_FREQUENCY_CHANNEL_NUMBER			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 144L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_FREQUENCY_CHANNEL_BAND				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 145L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_SYNC_OUT_TYPE						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 146L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_SYNC_OUT_OFFSET						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 147L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_SYNC_OUT_BEGIN_TIMESLOT				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 148L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_PHASE_POLARITY						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 149L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_SYMBOL_RATE							  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 150L)		/* ViReal64 */
#define AGESG_ATTR_TDMA_BURST_RISE_TIME						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 151L)		/* ViReal64 */
#define AGESG_ATTR_TDMA_BURST_RISE_DELAY					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 152L)		/* ViReal64 */
#define AGESG_ATTR_TDMA_BURST_FALL_TIME						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 153L)		/* ViReal64 */
#define AGESG_ATTR_TDMA_BURST_FALL_DELAY					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 154L)		/* ViReal64 */

#define AGESG_ATTR_TDMA_IQ_SCALING							  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 155L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_FILTER_TYPE							  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 156L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_FILTER_ALPHA						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 157L)		/* ViReal64 */
#define AGESG_ATTR_TDMA_FILTER_BBT							  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 158L)		/* ViReal64 */
#define AGESG_ATTR_TDMA_OPTIMIZE_FIR						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 159L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_MODULATION_USER_FILE_IQ				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 160L)		/* ViString */
#define AGESG_ATTR_TDMA_MODULATION_USER_FILE_FSK			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 161L)		/* ViString */
#define AGESG_ATTR_TDMA_MODULATION_TYPE						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 162L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_MODULATION_FREQUENCY_DEVIATION		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 163L)		/* ViReal64 */
#define AGESG_ATTR_TDMA_MODULATION_PHASE_DEVIATION			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 164L)		/* ViReal64 */

#define AGESG_ATTR_TDMA_TETRA_TIMESLOT_ENABLED				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 182L)		/* ViBoolean */
#define AGESG_ATTR_TDMA_TETRA_TIMESLOT_TYPE					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 165L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_TETRA_UP_NORMAL_DATA				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 166L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_UP_NORMAL_DATA_FIX4			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 167L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_UP_NORMAL_DATA_USER_FILE		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 168L)		/* ViString */
#define AGESG_ATTR_TDMA_TETRA_UP_NORMAL_TS					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 169L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_TETRA_DOWN_NORMAL_CONT_DATA			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 170L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_DOWN_NORMAL_CONT_DATA_FIX4	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 171L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_DOWN_NORMAL_CONT_DATA_USER_FILE (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 172L)		/* ViString */
#define AGESG_ATTR_TDMA_TETRA_DOWN_NORMAL_CONT_TS			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 173L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_DOWN_NORMAL_CONT_B1			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 174L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_DOWN_NORMAL_CONT_B2			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 175L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_TETRA_DOWN_NORMAL_DISCONT_DATA		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 176L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_DOWN_NORMAL_DISCONT_DATA_FIX4	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 177L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_DOWN_NORMAL_DISCONT_DATA_USER_FILE (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 178L)		/* ViString */
#define AGESG_ATTR_TDMA_TETRA_DOWN_NORMAL_DISCONT_TS		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 179L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_DOWN_NORMAL_DISCONT_B1		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 180L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_DOWN_NORMAL_DISCONT_B2		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 181L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_TETRA_UP_CONTROL1_DATA				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 183L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_UP_CONTROL1_DATA_FIX4			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 184L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_UP_CONTROL1_DATA_USER_FILE	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 185L)		/* ViString */
#define AGESG_ATTR_TDMA_TETRA_UP_CONTROL1_TS				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 186L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_TETRA_UP_CONTROL2_DATA				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 187L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_UP_CONTROL2_DATA_FIX4			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 188L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_UP_CONTROL2_DATA_USER_FILE	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 189L)		/* ViString */
#define AGESG_ATTR_TDMA_TETRA_UP_CONTROL2_TS				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 190L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_TETRA_DOWN_SYNC_CONT_DATA			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 191L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_DOWN_SYNC_CONT_DATA_FIX4		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 192L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_DOWN_SYNC_CONT_DATA_USER_FILE	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 193L)		/* ViString */
#define AGESG_ATTR_TDMA_TETRA_DOWN_SYNC_CONT_FCOR			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 194L)		/* ViString */
#define AGESG_ATTR_TDMA_TETRA_DOWN_SYNC_CONT_SSB			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 195L)		/* ViString */
#define AGESG_ATTR_TDMA_TETRA_DOWN_SYNC_CONT_STS			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 196L)		/* ViString */
#define AGESG_ATTR_TDMA_TETRA_DOWN_SYNC_CONT_B_FIELD		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 197L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_TETRA_DOWN_SYNC_DISCONT_DATA		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 198L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_DOWN_SYNC_DISCONT_DATA_FIX4	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 199L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_DOWN_SYNC_DISCONT_DATA_USER_FILE (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 200L)		/* ViString */
#define AGESG_ATTR_TDMA_TETRA_DOWN_SYNC_DISCONT_FCOR		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 201L)		/* ViString */
#define AGESG_ATTR_TDMA_TETRA_DOWN_SYNC_DISCONT_SSB			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 202L)		/* ViString */
#define AGESG_ATTR_TDMA_TETRA_DOWN_SYNC_DISCONT_STS			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 203L)		/* ViString */
#define AGESG_ATTR_TDMA_TETRA_DOWN_SYNC_DISCONT_B_FIELD		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 204L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_TETRA_UP_CUSTOM_DATA				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 205L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_UP_CUSTOM_DATA_FIX4			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 206L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_UP_CUSTOM_DATA_USER_FILE		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 207L)		/* ViString */

#define AGESG_ATTR_TDMA_TETRA_DOWN_CUSTOM_CONT_DATA			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 208L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_DOWN_CUSTOM_CONT_DATA_FIX4	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 209L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_DOWN_CUSTOM_CONT_DATA_USER_FILE (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 210L)		/* ViString */

#define AGESG_ATTR_TDMA_TETRA_DOWN_CUSTOM_DISCONT_DATA		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 211L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_DOWN_CUSTOM_DISCONT_DATA_FIX4   (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 212L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_TETRA_DOWN_CUSTOM_DISCONT_DATA_USER_FILE (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 213L)		/* ViString */

#define AGESG_ATTR_TDMA_TETRA_SCRAMBLE_ENABLED				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 214L)		/* ViBoolean */
#define AGESG_ATTR_TDMA_TETRA_SCRAMBLE_SEED					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 215L)		/* ViString */

#define AGESG_ATTR_TDMA_PHS_SCRAMBLE_ENABLED				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 216L)		/* ViBoolean */
#define AGESG_ATTR_TDMA_PHS_SCRAMBLE_SEED					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 217L)		/* ViString */

#define AGESG_ATTR_TDMA_PHS_UPLINK_TIMESLOT_TYPE			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 218L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PHS_DOWNLINK_TIMESLOT_TYPE			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 219L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PHS_UPLINK_TIMESLOT_ENABLED			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 220L)		/* ViBoolean */
#define AGESG_ATTR_TDMA_PHS_DOWNLINK_TIMESLOT_ENABLED		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 221L)		/* ViBoolean */

#define AGESG_ATTR_TDMA_PHS_UPLINK_TCH_DATA					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 222L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PHS_DOWNLINK_TCH_DATA				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 223L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PHS_UPLINK_TCH_DATA_FIX4			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 224L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PHS_DOWNLINK_TCH_DATA_FIX4			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 225L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PHS_UPLINK_TCH_DATA_USER_FILE		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 226L)		/* ViString */
#define AGESG_ATTR_TDMA_PHS_DOWNLINK_TCH_DATA_USER_FILE		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 227L)		/* ViString */
#define AGESG_ATTR_TDMA_PHS_UPLINK_TCH_UNIQUE_WORD			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 228L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PHS_DOWNLINK_TCH_UNIQUE_WORD		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 229L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PHS_UPLINK_TCH_SACCH				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 230L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PHS_DOWNLINK_TCH_SACCH				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 231L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_PHS_UPLINK_SYNC_UNIQUE_WORD			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 232L)		/* ViString */
#define AGESG_ATTR_TDMA_PHS_DOWNLINK_SYNC_UNIQUE_WORD		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 233L)		/* ViString */
#define AGESG_ATTR_TDMA_PHS_UPLINK_SYNC_CELL_STATION_ID		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 234L)		/* ViString */
#define AGESG_ATTR_TDMA_PHS_DOWNLINK_SYNC_CELL_STATION_ID	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 235L)		/* ViString */
#define AGESG_ATTR_TDMA_PHS_UPLINK_SYNC_PERSONAL_STATION_ID	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 236L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PHS_DOWNLINK_SYNC_PERSONAL_STATION_ID (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 237L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PHS_UPLINK_SYNC_IDLE_FIELD			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 238L)		/* ViString */
#define AGESG_ATTR_TDMA_PHS_DOWNLINK_SYNC_IDLE_FIELD		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 239L)		/* ViString */

#define AGESG_ATTR_TDMA_PHS_UPLINK_CUSTOM_DATA				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 240L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PHS_DOWNLINK_CUSTOM_DATA			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 241L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PHS_UPLINK_CUSTOM_DATA_FIX4			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 242L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PHS_DOWNLINK_CUSTOM_DATA_FIX4		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 243L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PHS_UPLINK_CUSTOM_DATA_USER_FILE	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 244L)		/* ViString */
#define AGESG_ATTR_TDMA_PHS_DOWNLINK_CUSTOM_DATA_USER_FILE	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 245L)		/* ViString */

#define AGESG_ATTR_TDMA_NADC_TIMESLOT_TYPE					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 246L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_NADC_TIMESLOT_ENABLED				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 247L)		/* ViBoolean */

#define AGESG_ATTR_TDMA_NADC_UPLINK_TCH_DATA				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 248L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_NADC_DOWNLINK_TCH_DATA				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 249L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_NADC_UPLINK_TCH_DATA_FIX4			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 250L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_NADC_DOWNLINK_TCH_DATA_FIX4			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 251L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_NADC_UPLINK_TCH_DATA_USER_FILE		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 252L)		/* ViString */
#define AGESG_ATTR_TDMA_NADC_DOWNLINK_TCH_DATA_USER_FILE	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 253L)		/* ViString */
#define AGESG_ATTR_TDMA_NADC_UPLINK_TCH_SYNCHRONIZATION_WORD  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 254L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_NADC_DOWNLINK_TCH_SYNCHRONIZATION_WORD (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 255L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_NADC_UPLINK_TCH_SACCH				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 256L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_NADC_DOWNLINK_TCH_SACCH				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 257L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_NADC_UPLINK_TCH_CDVCC				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 258L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_NADC_DOWNLINK_TCH_CDVCC				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 259L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_NADC_DOWNLINK_TCH_CDL				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 260L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_NADC_UPLINK_CUSTOM_DATA				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 261L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_NADC_DOWNLINK_CUSTOM_DATA			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 262L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_NADC_UPLINK_CUSTOM_DATA_FIX4		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 263L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_NADC_DOWNLINK_CUSTOM_DATA_FIX4		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 264L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_NADC_UPLINK_CUSTOM_DATA_USER_FILE	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 265L)		/* ViString */
#define AGESG_ATTR_TDMA_NADC_DOWNLINK_CUSTOM_DATA_USER_FILE	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 266L)		/* ViString */

#define AGESG_ATTR_TDMA_NADC_FRAME_RATE						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 267L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PDC_FRAME_RATE						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 268L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_PDC_TIMESLOT_TYPE					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 269L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PDC_TIMESLOT_ENABLED				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 270L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PDC_UPLINK_TCH_DATA					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 271L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PDC_DOWNLINK_TCH_DATA				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 272L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PDC_UPLINK_TCH_DATA_FIX4			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 273L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PDC_DOWNLINK_TCH_DATA_FIX4			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 274L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PDC_UPLINK_TCH_DATA_USER_FILE		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 275L)		/* ViString */
#define AGESG_ATTR_TDMA_PDC_DOWNLINK_TCH_DATA_USER_FILE		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 276L)		/* ViString */
#define AGESG_ATTR_TDMA_PDC_UPLINK_TCH_SYNCHRONIZATION_WORD	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 277L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PDC_DOWNLINK_TCH_SYNCHRONIZATION_WORD (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 278L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PDC_UPLINK_TCH_SACCH				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 279L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PDC_DOWNLINK_TCH_SACCH				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 280L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PDC_UPLINK_TCH_COLOR_CODE			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 281L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PDC_DOWNLINK_TCH_COLOR_CODE			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 282L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_PDC_UPLINK_VOX_SYNCHRONIZATION_WORD	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 283L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PDC_UPLINK_VOX_SACCH				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 284L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PDC_UPLINK_VOX_COLOR_CODE			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 285L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_PDC_UPLINK_CUSTOM_DATA				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 286L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PDC_DOWNLINK_CUSTOM_DATA			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 287L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PDC_UPLINK_CUSTOM_DATA_FIX4			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 288L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PDC_DOWNLINK_CUSTOM_DATA_FIX4		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 289L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_PDC_UPLINK_CUSTOM_DATA_USER_FILE	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 290L)		/* ViString */
#define AGESG_ATTR_TDMA_PDC_DOWNLINK_CUSTOM_DATA_USER_FILE	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 291L)		/* ViString */

#define AGESG_ATTR_TDMA_GSM_TIMESLOT_TYPE					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 292L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_GSM_TIMESLOT_ENABLED				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 293L)		/* ViBoolean */

#define AGESG_ATTR_TDMA_GSM_NORMAL_DATA_USER_FILE			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 294L)		/* ViString */
#define AGESG_ATTR_TDMA_GSM_NORMAL_DATA						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 295L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_GSM_NORMAL_DATA_FIX4				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 296L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_GSM_NORMAL_STEAL_BIT				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 297L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_GSM_NORMAL_TRAINING_SEQUENCE		  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 298L)		/* ViString */

#define AGESG_ATTR_TDMA_GSM_SYNC_DATA_USER_FILE				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 299L)		/* ViString */
#define AGESG_ATTR_TDMA_GSM_SYNC_DATA						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 300L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_GSM_SYNC_DATA_FIX4					  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 301L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_GSM_SYNC_TRAINING_SEQUENCE			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 302L)		/* ViString */

#define AGESG_ATTR_TDMA_GSM_DUMMY_TRAINING_SEQUENCE			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 303L)		/* ViString */

#define AGESG_ATTR_TDMA_GSM_ACCESS_DATA_USER_FILE			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 304L)		/* ViString */
#define AGESG_ATTR_TDMA_GSM_ACCESS_DATA						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 305L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_GSM_ACCESS_DATA_FIX4				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 306L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_GSM_ACCESS_EXTENDED_TAIL			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 307L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_GSM_ACCESS_SYNCHRONIZATION_SEQUENCE	  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 308L)		/* ViString */

#define AGESG_ATTR_TDMA_GSM_CUSTOM_DATA_USER_FILE			  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 309L)		/* ViString */
#define AGESG_ATTR_TDMA_GSM_CUSTOM_DATA						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 310L)		/* ViInt32 */
#define AGESG_ATTR_TDMA_GSM_CUSTOM_DATA_FIX4				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 311L)		/* ViInt32 */

#define AGESG_ATTR_TDMA_GSM_DIFFERENTIAL_DATA_ENCODING_ENABLED (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 312L)		/* ViBoolean */
#define AGESG_ATTR_TDMA_CUSTOM_DIFFERENTIAL_DATA_ENCODING_ENABLED (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 313L)	/* ViBoolean */

        /*- Digital Modulation Specific attributes -*/
#define AGESG_ATTR_DM_EXTERNAL_ALC_BANDWIDTH					(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 370L)		/* ViInt32 */
#define AGESG_ATTR_DM_EXTERNAL_POLARITY							(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 371L)		/* ViInt32 */
#define AGESG_ATTR_DM_EXTERNAL_SOURCE							(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 372L)		/* ViInt32 */

#define AGESG_ATTR_DM_EXTERNAL_COMMON_OFFSET					(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 373L)		/* ViReal64 */
#define AGESG_ATTR_DM_EXTERNAL_DIFFERENTIAL_I_OFFSET			(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 374L)		/* ViReal64 */
#define AGESG_ATTR_DM_EXTERNAL_DIFFERENTIAL_Q_OFFSET			(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 375L)		/* ViReal64 */
#define AGESG_ATTR_DM_EXTERNAL_I_OFFSET							(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 376L)		/* ViReal64 */
#define AGESG_ATTR_DM_EXTERNAL_Q_OFFSET							(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 377L)		/* ViReal64 */
#define AGESG_ATTR_DM_EXTERNAL_GAIN								(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 378L)		/* ViReal64 */
#define AGESG_ATTR_DM_EXTERNAL_IQ_ATTENUATION_LEVEL				(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 379L)		/* ViReal64 */
#define AGESG_ATTR_DM_IQ_INPUT_SKEW								(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 382L)		/* ViReal64 */

#define AGESG_ATTR_DM_SKEW_PATH									(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 380L)		/* ViInt32 */
#define AGESG_ATTR_DM_SKEW_ENABLED								(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 381L)		/* ViBoolean */

//#define AGESG_ATTR_NEXT			         				  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 388L)		/* ViInt32 */

/****************************************************************************
 *----------------- IviRFSigGen Class Attribute Value Defines -----------------*
 ****************************************************************************/
/*- Defined values for attribute AGESG_ATTR_AM_SCALE -*/
#define AGESG_VAL_AM_SCALING_LINEAR                                     IVIRFSIGGEN_VAL_AM_SCALING_LINEAR

/*- Defined values for attribute AGESG_ATTR_AM2_SCALE -*/
#define AGESG_VAL_AM2_SCALING_LINEAR                                    IVIRFSIGGEN_VAL_AM_SCALING_LINEAR

/*- Defined values for attribute AGESG_ATTR_AM_EXTERNAL_COUPLING -*/
#define AGESG_VAL_AM_EXTERNAL_COUPLING_AC                             IVIRFSIGGEN_VAL_AM_EXTERNAL_COUPLING_AC
#define AGESG_VAL_AM_EXTERNAL_COUPLING_DC                             IVIRFSIGGEN_VAL_AM_EXTERNAL_COUPLING_DC

/*- Defined values for attribute AGESG_ATTR_AM2_EXTERNAL_COUPLING -*/
#define AGESG_VAL_AM2_EXTERNAL_COUPLING_AC                             IVIRFSIGGEN_VAL_AM_EXTERNAL_COUPLING_AC
#define AGESG_VAL_AM2_EXTERNAL_COUPLING_DC                             IVIRFSIGGEN_VAL_AM_EXTERNAL_COUPLING_DC

/*- Defined values for attribute AGESG_ATTR_FM_EXTERNAL_COUPLING -*/
#define AGESG_VAL_FM_EXTERNAL_COUPLING_AC                             IVIRFSIGGEN_VAL_FM_EXTERNAL_COUPLING_AC
#define AGESG_VAL_FM_EXTERNAL_COUPLING_DC                             IVIRFSIGGEN_VAL_FM_EXTERNAL_COUPLING_DC

/*- Defined values for attribute AGESG_ATTR_FM2_EXTERNAL_COUPLING -*/
#define AGESG_VAL_FM2_EXTERNAL_COUPLING_AC                            IVIRFSIGGEN_VAL_FM_EXTERNAL_COUPLING_AC
#define AGESG_VAL_FM2_EXTERNAL_COUPLING_DC                            IVIRFSIGGEN_VAL_FM_EXTERNAL_COUPLING_DC

/*- Defined values for attribute AGESG_ATTR_PM_EXTERNAL_COUPLING -*/
#define AGESG_VAL_PM_EXTERNAL_COUPLING_AC                             IVIRFSIGGEN_VAL_PM_EXTERNAL_COUPLING_AC
#define AGESG_VAL_PM_EXTERNAL_COUPLING_DC                             IVIRFSIGGEN_VAL_PM_EXTERNAL_COUPLING_DC

/*- Defined values for attribute AGESG_ATTR_PM2_EXTERNAL_COUPLING -*/
#define AGESG_VAL_PM2_EXTERNAL_COUPLING_AC                            IVIRFSIGGEN_VAL_PM_EXTERNAL_COUPLING_AC
#define AGESG_VAL_PM2_EXTERNAL_COUPLING_DC                            IVIRFSIGGEN_VAL_PM_EXTERNAL_COUPLING_DC

/*- Defined values for attribute AGESG_ATTR_PULSE_MODULATION_SOURCE -*/
#define AGESG_VAL_PULSE_MODULATION_SOURCE_INTERNAL                    IVIRFSIGGEN_VAL_PULSE_MODULATION_SOURCE_INTERNAL
#define AGESG_VAL_PULSE_MODULATION_SOURCE_EXTERNAL                    IVIRFSIGGEN_VAL_PULSE_MODULATION_SOURCE_EXTERNAL

/*- Defined values for attribute AGESG_ATTR_PULSE_MODULATION_EXTERNAL_POLARITY -*/
#define AGESG_VAL_PULSE_MODULATION_EXTERNAL_POLARITY_NORMAL           IVIRFSIGGEN_VAL_PULSE_MODULATION_EXTERNAL_POLARITY_NORMAL

/*- Defined values for attribute AGESG_ATTR_LF_GENERATOR_WAVEFORM -*/
#define AGESG_VAL_LF_GENERATOR_WAVEFORM_SINE                          IVIRFSIGGEN_VAL_LF_GENERATOR_WAVEFORM_SINE
#define AGESG_VAL_LF_GENERATOR_WAVEFORM_SQUARE                        IVIRFSIGGEN_VAL_LF_GENERATOR_WAVEFORM_SQUARE
#define AGESG_VAL_LF_GENERATOR_WAVEFORM_TRIANGLE                      IVIRFSIGGEN_VAL_LF_GENERATOR_WAVEFORM_TRIANGLE
#define AGESG_VAL_LF_GENERATOR_WAVEFORM_RAMP_UP                       IVIRFSIGGEN_VAL_LF_GENERATOR_WAVEFORM_RAMP_UP

#define AGESG_VAL_LF_GENERATOR_WAVEFORM_DUAL_SINE                     (IVIRFSIGGEN_VAL_LF_GENERATOR_WAVEFORM_SPECIFIC_EXT_BASE + 1L)
#define AGESG_VAL_LF_GENERATOR_WAVEFORM_SWEPT_SINE                    (IVIRFSIGGEN_VAL_LF_GENERATOR_WAVEFORM_SPECIFIC_EXT_BASE + 2L)
#define AGESG_VAL_LF_GENERATOR_WAVEFORM_PULSE                         (IVIRFSIGGEN_VAL_LF_GENERATOR_WAVEFORM_SPECIFIC_EXT_BASE + 3L)
#define AGESG_VAL_LF_GENERATOR_WAVEFORM_NOISE                         (IVIRFSIGGEN_VAL_LF_GENERATOR_WAVEFORM_SPECIFIC_EXT_BASE + 4L)
#define AGESG_VAL_LF_GENERATOR_WAVEFORM_DC                            (IVIRFSIGGEN_VAL_LF_GENERATOR_WAVEFORM_SPECIFIC_EXT_BASE + 5L)

/*- Defined values for attribute AGESG_ATTR_SWEEP_TRIGGER_SOURCE -*/
#define AGESG_VAL_LF_SWEEP_TRIGGER_SOURCE_IMMEDIATE                   IVIRFSIGGEN_VAL_SWEEP_TRIGGER_SOURCE_IMMEDIATE
#define AGESG_VAL_LF_SWEEP_TRIGGER_SOURCE_EXTERNAL                    IVIRFSIGGEN_VAL_SWEEP_TRIGGER_SOURCE_EXTERNAL
#define AGESG_VAL_LF_SWEEP_TRIGGER_SOURCE_SOFTWARE                    IVIRFSIGGEN_VAL_SWEEP_TRIGGER_SOURCE_SOFTWARE
#define AGESG_VAL_LF_SWEEP_TRIGGER_SOURCE_TRIGGER_KEY                 (IVIRFSIGGEN_VAL_SWEEP_TRIGGER_SOURCE_SPECIFIC_EXT_BASE + 1L)

/*- Defined values for attribute AGESG_ATTR_PULSE_TRIGGER_SOURCE -*/
#define AGESG_VAL_PULSE_TRIGGER_SOURCE_INTERNAL                       IVIRFSIGGEN_VAL_PULSE_TRIGGER_SOURCE_INTERNAL
#define AGESG_VAL_PULSE_TRIGGER_SOURCE_EXTERNAL                       IVIRFSIGGEN_VAL_PULSE_TRIGGER_SOURCE_EXTERNAL

/*- Defined values for attribute AGESG_ATTR_PULSE_EXTERNAL_TRIGGER_SLOPE -*/
#define AGESG_VAL_PULSE_EXTERNAL_TRIGGER_SLOPE_POSITIVE               IVIRFSIGGEN_VAL_PULSE_EXTERNAL_TRIGGER_SLOPE_POSITIVE
#define AGESG_VAL_PULSE_EXTERNAL_TRIGGER_SLOPE_NEGATIVE               IVIRFSIGGEN_VAL_PULSE_EXTERNAL_TRIGGER_SLOPE_NEGATIVE
/* #define AGESG_VAL_PULSE_EXTERNAL_TRIGGER_SLOPE_CLASS_EXT_BASE         IVIRFSIGGEN_VAL_PULSE_EXTERNAL_TRIGGER_SLOPE_CLASS_EXT_BASE    */
/* #define AGESG_VAL_PULSE_EXTERNAL_TRIGGER_SLOPE_SPECIFIC_EXT_BASE      IVIRFSIGGEN_VAL_PULSE_EXTERNAL_TRIGGER_SLOPE_SPECIFIC_EXT_BASE */

/*- Defined values for attribute AGESG_ATTR_PULSE_OUTPUT_POLARITY -*/
#define AGESG_VAL_PULSE_OUTPUT_POLARITY_NORMAL                        IVIRFSIGGEN_VAL_PULSE_OUTPUT_POLARITY_NORMAL

/*- Defined values for attribute AGESG_ATTR_SWEEP_MODE -*/
#define AGESG_VAL_SWEEP_MODE_NONE                                     IVIRFSIGGEN_VAL_SWEEP_MODE_NONE
#define AGESG_VAL_SWEEP_MODE_FREQUENCY_STEP                           IVIRFSIGGEN_VAL_SWEEP_MODE_FREQUENCY_STEP
#define AGESG_VAL_SWEEP_MODE_POWER_STEP                               IVIRFSIGGEN_VAL_SWEEP_MODE_POWER_STEP
#define AGESG_VAL_SWEEP_MODE_LIST                                     IVIRFSIGGEN_VAL_SWEEP_MODE_LIST
#define AGESG_VAL_SWEEP_MODE_FREQUENCY_POWER_STEP					  (IVIRFSIGGEN_VAL_SWEEP_MODE_SPECIFIC_EXT_BASE + 1L)

/*- Defined values for attribute AGESG_ATTR_SWEEP_TRIGGER_SOURCE -*/
#define AGESG_VAL_SWEEP_TRIGGER_SOURCE_IMMEDIATE                      IVIRFSIGGEN_VAL_SWEEP_TRIGGER_SOURCE_IMMEDIATE
#define AGESG_VAL_SWEEP_TRIGGER_SOURCE_EXTERNAL                       IVIRFSIGGEN_VAL_SWEEP_TRIGGER_SOURCE_EXTERNAL
#define AGESG_VAL_SWEEP_TRIGGER_SOURCE_SOFTWARE                       IVIRFSIGGEN_VAL_SWEEP_TRIGGER_SOURCE_SOFTWARE
#define AGESG_VAL_SWEEP_TRIGGER_SOURCE_TRIGGER_KEY                    (IVIRFSIGGEN_VAL_SWEEP_TRIGGER_SOURCE_SPECIFIC_EXT_BASE + 1L)

/*- Defined values for attribute AGESG_ATTR_FREQUENCY_STEP_SCALE -*/
#define AGESG_VAL_FREQUENCY_STEP_SCALING_LINEAR                         IVIRFSIGGEN_VAL_FREQUENCY_STEP_SCALING_LINEAR

/*- Defined values for attribute AGESG_ATTR_ALC_SOURCE -*/
#define AGESG_VAL_ALC_SOURCE_INTERNAL                                 IVIRFSIGGEN_VAL_ALC_SOURCE_INTERNAL

/*- Defined values for attribute AGESG_ATTR_REFERENCE_OSCILLATOR_SOURCE -*/
#define AGESG_VAL_REFERENCE_OSCILLATOR_SOURCE_INTERNAL                IVIRFSIGGEN_VAL_REFERENCE_OSCILLATOR_SOURCE_INTERNAL
#define AGESG_VAL_REFERENCE_OSCILLATOR_SOURCE_EXTERNAL                IVIRFSIGGEN_VAL_REFERENCE_OSCILLATOR_SOURCE_EXTERNAL

/*- Defined values for attribute AGESG_ATTR_IQ_SOURCE -*/
#define AGESG_VAL_IQ_SOURCE_DIGITAL_MODULATION_BASE                   IVIRFSIGGEN_VAL_IQ_SOURCE_DIGITAL_MODULATION_BASE
#define AGESG_VAL_IQ_SOURCE_CDMA_BASE                                 IVIRFSIGGEN_VAL_IQ_SOURCE_CDMA_BASE
#define AGESG_VAL_IQ_SOURCE_TDMA_BASE                                 IVIRFSIGGEN_VAL_IQ_SOURCE_TDMA_BASE
#define AGESG_VAL_IQ_SOURCE_ARB_GENERATOR                             IVIRFSIGGEN_VAL_IQ_SOURCE_ARB_GENERATOR
#define AGESG_VAL_IQ_SOURCE_EXTERNAL                                  IVIRFSIGGEN_VAL_IQ_SOURCE_EXTERNAL
/* #define AGESG_VAL_IQ_SOURCE_CLASS_EXT_BASE                            IVIRFSIGGEN_VAL_IQ_SOURCE_CLASS_EXT_BASE    */
/* #define AGESG_VAL_IQ_SOURCE_SPECIFIC_EXT_BASE                         IVIRFSIGGEN_VAL_IQ_SOURCE_SPECIFIC_EXT_BASE */

/*- Defined values for attribute AGESG_ATTR_ARB_TRIGGER_SOURCE -*/
#define AGESG_VAL_ARB_TRIGGER_SOURCE_IMMEDIATE                        IVIRFSIGGEN_VAL_ARB_TRIGGER_SOURCE_IMMEDIATE
#define AGESG_VAL_ARB_TRIGGER_SOURCE_EXTERNAL                         IVIRFSIGGEN_VAL_ARB_TRIGGER_SOURCE_EXTERNAL
#define AGESG_VAL_ARB_TRIGGER_SOURCE_SOFTWARE                         IVIRFSIGGEN_VAL_ARB_TRIGGER_SOURCE_SOFTWARE
#define AGESG_VAL_ARB_TRIGGER_SOURCE_TRIGGER_KEY                      (IVIRFSIGGEN_VAL_ARB_TRIGGER_SOURCE_SPECIFIC_EXT_BASE + 1L)

/*- Defined values for attribute AGESG_ATTR_ARB_EXTERNAL_TRIGGER_SLOPE -*/
#define AGESG_VAL_ARB_EXTERNAL_TRIGGER_SLOPE_POSITIVE                 IVIRFSIGGEN_VAL_ARB_EXTERNAL_TRIGGER_SLOPE_POSITIVE
#define AGESG_VAL_ARB_EXTERNAL_TRIGGER_SLOPE_NEGATIVE                 IVIRFSIGGEN_VAL_ARB_EXTERNAL_TRIGGER_SLOPE_NEGATIVE

/*- Defined values for attribute AGESG_ATTR_CDMA_TRIGGER_SOURCE -*/
#define AGESG_VAL_CDMA_TRIGGER_SOURCE_IMMEDIATE                       IVIRFSIGGEN_VAL_CDMA_TRIGGER_SOURCE_IMMEDIATE
#define AGESG_VAL_CDMA_TRIGGER_SOURCE_EXTERNAL                        IVIRFSIGGEN_VAL_CDMA_TRIGGER_SOURCE_EXTERNAL
#define AGESG_VAL_CDMA_TRIGGER_SOURCE_SOFTWARE                        IVIRFSIGGEN_VAL_CDMA_TRIGGER_SOURCE_SOFTWARE
#define AGESG_VAL_CDMA_TRIGGER_SOURCE_TRIGGER_KEY					  (IVIRFSIGGEN_VAL_CDMA_TRIGGER_SOURCE_SPECIFIC_EXT_BASE + 1L)

/*- Defined values for attribute AGESG_ATTR_CDMA_EXTERNAL_TRIGGER_SLOPE -*/
#define AGESG_VAL_CDMA_EXTERNAL_TRIGGER_SLOPE_POSITIVE                IVIRFSIGGEN_VAL_CDMA_EXTERNAL_TRIGGER_SLOPE_POSITIVE
#define AGESG_VAL_CDMA_EXTERNAL_TRIGGER_SLOPE_NEGATIVE                IVIRFSIGGEN_VAL_CDMA_EXTERNAL_TRIGGER_SLOPE_NEGATIVE

/*- Defined values for attribute AGESG_ATTR_CDMA_CLOCK_SOURCE -*/
#define AGESG_VAL_CDMA_CLOCK_SOURCE_INTERNAL                          IVIRFSIGGEN_VAL_CDMA_CLOCK_SOURCE_INTERNAL
#define AGESG_VAL_CDMA_CLOCK_SOURCE_EXTERNAL                          IVIRFSIGGEN_VAL_CDMA_CLOCK_SOURCE_EXTERNAL

/*- Defined values for attribute AGESG_ATTR_TDMA_TRIGGER_SOURCE -*/
#define AGESG_VAL_TDMA_TRIGGER_SOURCE_IMMEDIATE                       IVIRFSIGGEN_VAL_TDMA_TRIGGER_SOURCE_IMMEDIATE
#define AGESG_VAL_TDMA_TRIGGER_SOURCE_EXTERNAL                        IVIRFSIGGEN_VAL_TDMA_TRIGGER_SOURCE_EXTERNAL
#define AGESG_VAL_TDMA_TRIGGER_SOURCE_SOFTWARE                        IVIRFSIGGEN_VAL_TDMA_TRIGGER_SOURCE_SOFTWARE
#define AGESG_VAL_TDMA_TRIGGER_SOURCE_TRIGGER_KEY					  (IVIRFSIGGEN_VAL_TDMA_TRIGGER_SOURCE_SPECIFIC_EXT_BASE + 1L)

/*- Defined values for attribute AGESG_ATTR_TDMA_EXTERNAL_TRIGGER_SLOPE -*/
#define AGESG_VAL_TDMA_EXTERNAL_TRIGGER_SLOPE_POSITIVE                IVIRFSIGGEN_VAL_TDMA_EXTERNAL_TRIGGER_SLOPE_POSITIVE
#define AGESG_VAL_TDMA_EXTERNAL_TRIGGER_SLOPE_NEGATIVE                IVIRFSIGGEN_VAL_TDMA_EXTERNAL_TRIGGER_SLOPE_NEGATIVE

/*- Defined values for attribute AGESG_ATTR_TDMA_CLOCK_SOURCE -*/
#define AGESG_VAL_TDMA_CLOCK_SOURCE_INTERNAL                          IVIRFSIGGEN_VAL_TDMA_CLOCK_SOURCE_INTERNAL
#define AGESG_VAL_TDMA_CLOCK_SOURCE_EXTERNAL                          IVIRFSIGGEN_VAL_TDMA_CLOCK_SOURCE_EXTERNAL

/*- Defined values for attribute AGESG_ATTR_TDMA_EXTERNAL_CLOCK_TYPE -*/
#define AGESG_VAL_TDMA_EXTERNAL_CLOCK_TYPE_BIT                        IVIRFSIGGEN_VAL_TDMA_EXTERNAL_CLOCK_TYPE_BIT
#define AGESG_VAL_TDMA_EXTERNAL_CLOCK_TYPE_SYMBOL                     IVIRFSIGGEN_VAL_TDMA_EXTERNAL_CLOCK_TYPE_SYMBOL

/*- Defined values for attribute AGESG_ATTR_PM_BANDWIDTH -*/
#define AGESG_VAL_PM_BANDWIDTH_NORMAL                               0
#define AGESG_VAL_PM_BANDWIDTH_HIGH                                 1

#define AGESG_VAL_FREQUENCY_MODE_CW                                 0
#define AGESG_VAL_FREQUENCY_MODE_FIXED                              1
#define AGESG_VAL_FREQUENCY_MODE_LIST                               2

#define AGESG_VAL_POWER_MODE_FIXED                                  1
#define AGESG_VAL_POWER_MODE_LIST                                   2

#define AGESG_VAL_SWEEP_LIST_TYPE_LIST                              0
#define AGESG_VAL_SWEEP_LIST_TYPE_STEP                              1

#define AGESG_VAL_SWEEP_DWELL_MODE_STEP                             0
#define AGESG_VAL_SWEEP_DWELL_MODE_LIST                             1

#define AGESG_VAL_ARB_FILTER_FREQUENCY_THROUGH                      0.0			/* Through */
#define AGESG_VAL_ARB_FILTER_FREQUENCY_250_KHZ                      250.0e+3	/* 250.0 kHz */
#define AGESG_VAL_ARB_FILTER_FREQUENCY_2_5_MHZ                      2.5e+6		/*   2.5 MHz */
#define AGESG_VAL_ARB_FILTER_FREQUENCY_8_MHZ                        8.0e+6		/*   8.0 MHz */

#define AGESG_VAL_ARB_REFERENCE_INTERNAL							0
#define AGESG_VAL_ARB_REFERENCE_EXTERNAL							1

#define AGESG_VAL_ARB_TRIGGER_TYPE_CONTINUOUS                       0
#define AGESG_VAL_ARB_TRIGGER_TYPE_SINGLE                           1
#define AGESG_VAL_ARB_TRIGGER_TYPE_GATE                             2
#define AGESG_VAL_ARB_TRIGGER_TYPE_SEQ_ADVANCE                      3

#define AGESG_VAL_SWEEP_REPEAT_SINGLE								0
#define AGESG_VAL_SWEEP_REPEAT_CONTINUOUS							1

#define AGESG_VAL_ALC_BANDWIDTH_NORMAL								200.0
#define AGESG_VAL_ALC_BANDWIDTH_NARROW								100.0

#define AGESG_VAL_FREQUENCY_OPTIMIZATION_BELOW_10KHZ				1
#define AGESG_VAL_FREQUENCY_OPTIMIZATION_ABOVE_10KHZ				2

#define AGESG_VAL_ALTERNATE_TRIGGER_SOURCE_INTERNAL					0
#define AGESG_VAL_ALTERNATE_TRIGGER_SOURCE_EXTERNAL					1
#define AGESG_VAL_ALTERNATE_TRIGGER_SOURCE_MANUAL					2

#define AGESG_VAL_ALTERNATE_MANUAL_TRIGGER_MAIN						0
#define AGESG_VAL_ALTERNATE_MANUAL_TRIGGER_DELTA					1

#define AGESG_VAL_ALC_SEARCH_REFERENCE_FIXED						0
#define AGESG_VAL_ALC_SEARCH_REFERENCE_MODULATED					1

#define AGESG_VAL_SWEEP_DIRECTION_UP								0
#define AGESG_VAL_SWEEP_DIRECTION_DOWN								1

#define AGESG_VAL_LIST_MODE_AUTO									0
#define AGESG_VAL_LIST_MODE_MANUAL									1

#define AGESG_VAL_PULSE_WAVEFORM_SQUARE								0
#define AGESG_VAL_PULSE_WAVEFORM_PULSE								1

#define AGESG_VAL_IQ_MODULATION_SOURCE_INTERNAL						0
#define AGESG_VAL_IQ_MODULATION_SOURCE_EXTERNAL						1
#define AGESG_VAL_IQ_MODULATION_SOURCE_BBG1							2
#define AGESG_VAL_IQ_MODULATION_SOURCE_EXT600						3
#define AGESG_VAL_IQ_MODULATION_SOURCE_OFF							4

#define AGESG_VAL_BURST_GATE_INPUT_POLARITY_POSITIVE				0
#define AGESG_VAL_BURST_GATE_INPUT_POLARITY_NEGATIVE				1

#define AGESG_VAL_DATA_CLOCK_INPUT_POLARITY_POSITIVE				0
#define AGESG_VAL_DATA_CLOCK_INPUT_POLARITY_NEGATIVE				1

#define AGESG_VAL_DATA_CLOCK_OUTPUT_POLARITY_POSITIVE				0
#define AGESG_VAL_DATA_CLOCK_OUTPUT_POLARITY_NEGATIVE				1

#define AGESG_VAL_DATA_INPUT_POLARITY_POSITIVE						0
#define AGESG_VAL_DATA_INPUT_POLARITY_NEGATIVE						1

#define AGESG_VAL_DATA_OUTPUT_POLARITY_POSITIVE						0
#define AGESG_VAL_DATA_OUTPUT_POLARITY_NEGATIVE						1

#define AGESG_VAL_EVENT_OUTPUT_POLARITY_POSITIVE					0
#define AGESG_VAL_EVENT_OUTPUT_POLARITY_NEGATIVE					1

#define AGESG_VAL_PATTERN_TRIGGER_INPUT_POLARITY_POSITIVE			0
#define AGESG_VAL_PATTERN_TRIGGER_INPUT_POLARITY_NEGATIVE			1

#define AGESG_VAL_SYMBOL_SYNC_INPUT_POLARITY_POSITIVE				0
#define AGESG_VAL_SYMBOL_SYNC_INPUT_POLARITY_NEGATIVE				1

#define AGESG_VAL_SYMBOL_SYNC_OUTPUT_POLARITY_POSITIVE				0
#define AGESG_VAL_SYMBOL_SYNC_OUTPUT_POLARITY_NEGATIVE				1

#define AGESG_VAL_TRIGGER_OUTPUT_POLARITY_POSITIVE					0
#define AGESG_VAL_TRIGGER_OUTPUT_POLARITY_NEGATIVE					1

#define AGESG_VAL_TDMA_UNFRAMED_DATA_PN9							0
#define AGESG_VAL_TDMA_UNFRAMED_DATA_PN11							1
#define AGESG_VAL_TDMA_UNFRAMED_DATA_PN15							2
#define AGESG_VAL_TDMA_UNFRAMED_DATA_PN20							3
#define AGESG_VAL_TDMA_UNFRAMED_DATA_PN23							4
#define AGESG_VAL_TDMA_UNFRAMED_DATA_FIX4							5
#define AGESG_VAL_TDMA_UNFRAMED_DATA_USERFILE						6
#define AGESG_VAL_TDMA_UNFRAMED_DATA_EXTERNAL						7
#define AGESG_VAL_TDMA_UNFRAMED_DATA_P4								8
#define AGESG_VAL_TDMA_UNFRAMED_DATA_P8								9
#define AGESG_VAL_TDMA_UNFRAMED_DATA_P16							10
#define AGESG_VAL_TDMA_UNFRAMED_DATA_P32							11
#define AGESG_VAL_TDMA_UNFRAMED_DATA_P64							12

#define AGESG_VAL_TDMA_PN9_MODE_NORMAL								0
#define AGESG_VAL_TDMA_PN9_MODE_QUICK								1

#define AGESG_VAL_TDMA_SECONDARY_FRAME_TRIGGER_TRIGGER_KEY			0
#define AGESG_VAL_TDMA_SECONDARY_FRAME_TRIGGER_SOFTWARE				1
#define AGESG_VAL_TDMA_SECONDARY_FRAME_TRIGGER_EXTERNAL				2

#define AGESG_VAL_TDMA_DECT_RFP										0
#define AGESG_VAL_TDMA_DECT_PP										1

#define AGESG_VAL_TDMA_DECT_PP_TYPE_CUSTOM							0
#define AGESG_VAL_TDMA_DECT_PP_TYPE_TRAFFIC_BEARER					1
#define AGESG_VAL_TDMA_DECT_PP_TYPE_LOW_CAPACITY					2
#define AGESG_VAL_TDMA_DECT_PP_TYPE_TRAFFIC_BEARER_Z				3
#define AGESG_VAL_TDMA_DECT_PP_TYPE_LOW_CAPACITY_Z					4

#define AGESG_VAL_TDMA_DECT_LC_B_FIELD_PN9							0
#define AGESG_VAL_TDMA_DECT_LC_B_FIELD_PN15							1
#define AGESG_VAL_TDMA_DECT_LC_B_FIELD_FIX4							2
#define AGESG_VAL_TDMA_DECT_LC_B_FIELD_USERFILE						3
#define AGESG_VAL_TDMA_DECT_LC_B_FIELD_EXT							4
#define AGESG_VAL_TDMA_DECT_LC_B_FIELD_FDEV1_HS						5
#define AGESG_VAL_TDMA_DECT_LC_B_FIELD_FDEV1_FS						6
#define AGESG_VAL_TDMA_DECT_LC_B_FIELD_FDEV2_FS						7
#define AGESG_VAL_TDMA_DECT_LC_B_FIELD_FACC							8
#define AGESG_VAL_TDMA_DECT_LC_B_FIELD_DM0							9
#define AGESG_VAL_TDMA_DECT_LC_B_FIELD_DM1							10
#define AGESG_VAL_TDMA_DECT_LC_B_FIELD_P4							11
#define AGESG_VAL_TDMA_DECT_LC_B_FIELD_P8							12
#define AGESG_VAL_TDMA_DECT_LC_B_FIELD_P16							13
#define AGESG_VAL_TDMA_DECT_LC_B_FIELD_P32							14
#define AGESG_VAL_TDMA_DECT_LC_B_FIELD_P64							15

#define AGESG_VAL_TDMA_DECT_LCZ_B_FIELD_PN9							0
#define AGESG_VAL_TDMA_DECT_LCZ_B_FIELD_PN15						1
#define AGESG_VAL_TDMA_DECT_LCZ_B_FIELD_FIX4						2
#define AGESG_VAL_TDMA_DECT_LCZ_B_FIELD_USERFILE					3
#define AGESG_VAL_TDMA_DECT_LCZ_B_FIELD_EXT							4
#define AGESG_VAL_TDMA_DECT_LCZ_B_FIELD_FDEV1_HS					5
#define AGESG_VAL_TDMA_DECT_LCZ_B_FIELD_FDEV1_FS					6
#define AGESG_VAL_TDMA_DECT_LCZ_B_FIELD_FDEV2_FS					7
#define AGESG_VAL_TDMA_DECT_LCZ_B_FIELD_FACC						8
#define AGESG_VAL_TDMA_DECT_LCZ_B_FIELD_DM0							9
#define AGESG_VAL_TDMA_DECT_LCZ_B_FIELD_DM1							10
#define AGESG_VAL_TDMA_DECT_LCZ_B_FIELD_P4							11
#define AGESG_VAL_TDMA_DECT_LCZ_B_FIELD_P8							12
#define AGESG_VAL_TDMA_DECT_LCZ_B_FIELD_P16							13
#define AGESG_VAL_TDMA_DECT_LCZ_B_FIELD_P32							14
#define AGESG_VAL_TDMA_DECT_LCZ_B_FIELD_P64							15

#define AGESG_VAL_TDMA_DECT_RFP_TYPE_CUSTOM							0
#define AGESG_VAL_TDMA_DECT_RFP_TYPE_TRAFFIC_BEARER					1
#define AGESG_VAL_TDMA_DECT_RFP_TYPE_LOW_CAPACITY					2
#define AGESG_VAL_TDMA_DECT_RFP_TYPE_TRAFFIC_BEARER_Z				3
#define AGESG_VAL_TDMA_DECT_RFP_TYPE_LOW_CAPACITY_Z					4
#define AGESG_VAL_TDMA_DECT_RFP_TYPE_DUMMY_1						5
#define AGESG_VAL_TDMA_DECT_RFP_TYPE_DUMMY_2						6

#define AGESG_VAL_TDMA_DECT_TB_B_FIELD_PN9							0
#define AGESG_VAL_TDMA_DECT_TB_B_FIELD_PN15							1
#define AGESG_VAL_TDMA_DECT_TB_B_FIELD_FIX4							2
#define AGESG_VAL_TDMA_DECT_TB_B_FIELD_USERFILE						3
#define AGESG_VAL_TDMA_DECT_TB_B_FIELD_EXT							4
#define AGESG_VAL_TDMA_DECT_TB_B_FIELD_FDEV1_HS						5
#define AGESG_VAL_TDMA_DECT_TB_B_FIELD_FDEV1_FS						6
#define AGESG_VAL_TDMA_DECT_TB_B_FIELD_FDEV2_FS						7
#define AGESG_VAL_TDMA_DECT_TB_B_FIELD_FACC							8
#define AGESG_VAL_TDMA_DECT_TB_B_FIELD_DM0							9
#define AGESG_VAL_TDMA_DECT_TB_B_FIELD_DM1							10
#define AGESG_VAL_TDMA_DECT_TB_B_FIELD_P4							11
#define AGESG_VAL_TDMA_DECT_TB_B_FIELD_P8							12
#define AGESG_VAL_TDMA_DECT_TB_B_FIELD_P16							13
#define AGESG_VAL_TDMA_DECT_TB_B_FIELD_P32							14
#define AGESG_VAL_TDMA_DECT_TB_B_FIELD_P64							15

#define AGESG_VAL_TDMA_DECT_TBZ_B_FIELD_PN9							0
#define AGESG_VAL_TDMA_DECT_TBZ_B_FIELD_PN15						1
#define AGESG_VAL_TDMA_DECT_TBZ_B_FIELD_FIX4						2
#define AGESG_VAL_TDMA_DECT_TBZ_B_FIELD_USERFILE					3
#define AGESG_VAL_TDMA_DECT_TBZ_B_FIELD_EXT							4
#define AGESG_VAL_TDMA_DECT_TBZ_B_FIELD_FDEV1_HS					5
#define AGESG_VAL_TDMA_DECT_TBZ_B_FIELD_FDEV1_FS					6
#define AGESG_VAL_TDMA_DECT_TBZ_B_FIELD_FDEV2_FS					7
#define AGESG_VAL_TDMA_DECT_TBZ_B_FIELD_FACC						8
#define AGESG_VAL_TDMA_DECT_TBZ_B_FIELD_DM0							9
#define AGESG_VAL_TDMA_DECT_TBZ_B_FIELD_DM1							10
#define AGESG_VAL_TDMA_DECT_TBZ_B_FIELD_P4							11
#define AGESG_VAL_TDMA_DECT_TBZ_B_FIELD_P8							12
#define AGESG_VAL_TDMA_DECT_TBZ_B_FIELD_P16							13
#define AGESG_VAL_TDMA_DECT_TBZ_B_FIELD_P32							14
#define AGESG_VAL_TDMA_DECT_TBZ_B_FIELD_P64							15

#define AGESG_VAL_TDMA_DECT_CUSTOM_DATA_PN9							0
#define AGESG_VAL_TDMA_DECT_CUSTOM_DATA_PN15						1
#define AGESG_VAL_TDMA_DECT_CUSTOM_DATA_FIX4						2
#define AGESG_VAL_TDMA_DECT_CUSTOM_DATA_USERFILE					3
#define AGESG_VAL_TDMA_DECT_CUSTOM_DATA_EXT							4
#define AGESG_VAL_TDMA_DECT_CUSTOM_DATA_FDEV1_HS					5
#define AGESG_VAL_TDMA_DECT_CUSTOM_DATA_FDEV1_FS					6
#define AGESG_VAL_TDMA_DECT_CUSTOM_DATA_FDEV2_FS					7
#define AGESG_VAL_TDMA_DECT_CUSTOM_DATA_FACC						8
#define AGESG_VAL_TDMA_DECT_CUSTOM_DATA_DM0							9
#define AGESG_VAL_TDMA_DECT_CUSTOM_DATA_DM1							10
#define AGESG_VAL_TDMA_DECT_CUSTOM_DATA_P4							11
#define AGESG_VAL_TDMA_DECT_CUSTOM_DATA_P8							12
#define AGESG_VAL_TDMA_DECT_CUSTOM_DATA_P16							13
#define AGESG_VAL_TDMA_DECT_CUSTOM_DATA_P32							14
#define AGESG_VAL_TDMA_DECT_CUSTOM_DATA_P64							15

#define AGESG_VAL_TDMA_DECT_DB_DUMMY_1								0
#define AGESG_VAL_TDMA_DECT_DB_DUMMY_2								1

#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_STANDARD				0
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_P_GSM_BASE			1
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_P_GSM_MOBILE			2
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_E_GSM_BASE			3
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_E_GSM_MOBILE			4
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_R_GSM_BASE			5
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_R_GSM_MOBILE			6
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_DCS_BASE				7
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_DCS_MOBILE			8
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_PCS_BASE				9
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_PCS_MOBILE			10
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_GSM_450_BASE			11
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_GSM_450_MOBILE		12
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_GSM_480_BASE			13
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_GSM_480_MOBILE		14
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_GSM_850_BASE			15
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_GSM_850_MOBILE		16
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_800_MHZ_BASE			17
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_800_MHZ_MOBILE		18
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_1500_MHZ_BASE			19
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_1500_MHZ_MOBILE		20
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_BASE					21
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_MOBILE				22
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_BASE_390_400			23
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_MOBILE_380_390		24
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_BASE_420_430			25
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_MOBILE_410_420		26
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_BASE_460_470			27
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_MOBILE_450_460		28
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_BASE_915_921			29
#define AGESG_VAL_TDMA_FREQUENCY_CHANNEL_BAND_MOBILE_870_876		30

#define AGESG_VAL_TDMA_SYNC_OUT_TYPE_FRAME							0
#define AGESG_VAL_TDMA_SYNC_OUT_TYPE_BEGIN_TIMESLOT					1
#define AGESG_VAL_TDMA_SYNC_OUT_TYPE_ALL_TIMESLOTS					2

#define AGESG_VAL_TDMA_PHASE_POLARITY_NORMAL						0
#define AGESG_VAL_TDMA_PHASE_POLARITY_INVERTED						1

#define AGESG_VAL_TDMA_FILTER_TYPE_ROOT_NYQUIST						0
#define AGESG_VAL_TDMA_FILTER_TYPE_NYQUIST							1
#define AGESG_VAL_TDMA_FILTER_TYPE_GAUSSIAN							2
#define AGESG_VAL_TDMA_FILTER_TYPE_RECTANGLE						3
#define AGESG_VAL_TDMA_FILTER_TYPE_IS95								4
#define AGESG_VAL_TDMA_FILTER_TYPE_IS95_EQ							5
#define AGESG_VAL_TDMA_FILTER_TYPE_IS95_MOD							6
#define AGESG_VAL_TDMA_FILTER_TYPE_IS95_MOD_EQ						7
#define AGESG_VAL_TDMA_FILTER_TYPE_GSM_GAUSSIAN						8
#define AGESG_VAL_TDMA_FILTER_TYPE_APCO_C4FM						9
#define AGESG_VAL_TDMA_FILTER_TYPE_EDGE								10
#define AGESG_VAL_TDMA_FILTER_TYPE_USER_FIR							11

#define AGESG_VAL_TDMA_OPTIMIZE_FIR_EVM								0
#define AGESG_VAL_TDMA_OPTIMIZE_FIR_ACP								1

#define AGESG_VAL_TDMA_MODULATION_TYPE_BPSK							0
#define AGESG_VAL_TDMA_MODULATION_TYPE_QPSK							1
#define AGESG_VAL_TDMA_MODULATION_TYPE_IS95QPSK						2
#define AGESG_VAL_TDMA_MODULATION_TYPE_GRAYQPSK						3
#define AGESG_VAL_TDMA_MODULATION_TYPE_OQPSK						4
#define AGESG_VAL_TDMA_MODULATION_TYPE_IS95OQPSK					5
#define AGESG_VAL_TDMA_MODULATION_TYPE_P4DQPSK						6
#define AGESG_VAL_TDMA_MODULATION_TYPE_PSK8							7
#define AGESG_VAL_TDMA_MODULATION_TYPE_PSK16						8
#define AGESG_VAL_TDMA_MODULATION_TYPE_D8PSK						9
#define AGESG_VAL_TDMA_MODULATION_TYPE_MSK							10
#define AGESG_VAL_TDMA_MODULATION_TYPE_FSK2							11
#define AGESG_VAL_TDMA_MODULATION_TYPE_FSK4							12
#define AGESG_VAL_TDMA_MODULATION_TYPE_FSK8							13
#define AGESG_VAL_TDMA_MODULATION_TYPE_FSK16						14
#define AGESG_VAL_TDMA_MODULATION_TYPE_C4FM							15
#define AGESG_VAL_TDMA_MODULATION_TYPE_QAM4							16
#define AGESG_VAL_TDMA_MODULATION_TYPE_QAM8							17
#define AGESG_VAL_TDMA_MODULATION_TYPE_QAM16						18
#define AGESG_VAL_TDMA_MODULATION_TYPE_QAM32						19
#define AGESG_VAL_TDMA_MODULATION_TYPE_QAM64						20
#define AGESG_VAL_TDMA_MODULATION_TYPE_QAM256						21
#define AGESG_VAL_TDMA_MODULATION_TYPE_EDGE							22
#define AGESG_VAL_TDMA_MODULATION_TYPE_USER_IQ						23
#define AGESG_VAL_TDMA_MODULATION_TYPE_USER_FSK						24

#define AGESG_VAL_TDMA_TETRA_TIMESLOT_TYPE_UP_NORMAL				0
#define AGESG_VAL_TDMA_TETRA_TIMESLOT_TYPE_UP_CONTROL1				1
#define AGESG_VAL_TDMA_TETRA_TIMESLOT_TYPE_UP_CONTROL2				2
#define AGESG_VAL_TDMA_TETRA_TIMESLOT_TYPE_UP_CUSTOM				3
#define AGESG_VAL_TDMA_TETRA_TIMESLOT_TYPE_DOWN_NORMAL_CONT			4
#define AGESG_VAL_TDMA_TETRA_TIMESLOT_TYPE_DOWN_SYNC_CONT			5
#define AGESG_VAL_TDMA_TETRA_TIMESLOT_TYPE_DOWN_CUSTOM_CONT			6
#define AGESG_VAL_TDMA_TETRA_TIMESLOT_TYPE_DOWN_NORMAL_DISCONT		7
#define AGESG_VAL_TDMA_TETRA_TIMESLOT_TYPE_DOWN_SYNC_DISCONT		8
#define AGESG_VAL_TDMA_TETRA_TIMESLOT_TYPE_DOWN_CUSTOM_DISCONT		9

#define AGESG_VAL_TDMA_TETRA_TIMESLOT_DATA_PN9						0
#define AGESG_VAL_TDMA_TETRA_TIMESLOT_DATA_PN15						1
#define AGESG_VAL_TDMA_TETRA_TIMESLOT_DATA_FIX4						2
#define AGESG_VAL_TDMA_TETRA_TIMESLOT_DATA_USER_FILE				3
#define AGESG_VAL_TDMA_TETRA_TIMESLOT_DATA_P4						4
#define AGESG_VAL_TDMA_TETRA_TIMESLOT_DATA_P8						5
#define AGESG_VAL_TDMA_TETRA_TIMESLOT_DATA_P16						6
#define AGESG_VAL_TDMA_TETRA_TIMESLOT_DATA_P32						7
#define AGESG_VAL_TDMA_TETRA_TIMESLOT_DATA_P64						8
#define AGESG_VAL_TDMA_TETRA_TIMESLOT_DATA_EXTERNAL					9

#define AGESG_VAL_TDMA_PHS_TIMESLOT_DATA_PN9						0
#define AGESG_VAL_TDMA_PHS_TIMESLOT_DATA_PN15						1
#define AGESG_VAL_TDMA_PHS_TIMESLOT_DATA_FIX4						2
#define AGESG_VAL_TDMA_PHS_TIMESLOT_DATA_USER_FILE					3
#define AGESG_VAL_TDMA_PHS_TIMESLOT_DATA_P4							4
#define AGESG_VAL_TDMA_PHS_TIMESLOT_DATA_P8							5
#define AGESG_VAL_TDMA_PHS_TIMESLOT_DATA_P16						6
#define AGESG_VAL_TDMA_PHS_TIMESLOT_DATA_P32						7
#define AGESG_VAL_TDMA_PHS_TIMESLOT_DATA_P64						8
#define AGESG_VAL_TDMA_PHS_TIMESLOT_DATA_EXTERNAL					9

#define AGESG_VAL_TDMA_PHS_TIMESLOT_TYPE_TRAFFIC_CHANNEL			0
#define AGESG_VAL_TDMA_PHS_TIMESLOT_TYPE_SYNCHRONIZATION			1
#define AGESG_VAL_TDMA_PHS_TIMESLOT_TYPE_CUSTOM						2

#define AGESG_VAL_TDMA_PHS_CHANNEL_UPLINK							0
#define AGESG_VAL_TDMA_PHS_CHANNEL_DOWNLINK							1
#define AGESG_VAL_TDMA_PHS_CHANNEL_UPLINK_TCH_ALL					2
#define AGESG_VAL_TDMA_PHS_CHANNEL_DOWNLINK_TCH_ALL					3

#define AGESG_VAL_TDMA_NADC_TIMESLOT_DATA_PN9						0
#define AGESG_VAL_TDMA_NADC_TIMESLOT_DATA_PN15						1
#define AGESG_VAL_TDMA_NADC_TIMESLOT_DATA_FIX4						2
#define AGESG_VAL_TDMA_NADC_TIMESLOT_DATA_USER_FILE					3
#define AGESG_VAL_TDMA_NADC_TIMESLOT_DATA_P4						4
#define AGESG_VAL_TDMA_NADC_TIMESLOT_DATA_P8						5
#define AGESG_VAL_TDMA_NADC_TIMESLOT_DATA_P16						6
#define AGESG_VAL_TDMA_NADC_TIMESLOT_DATA_P32						7
#define AGESG_VAL_TDMA_NADC_TIMESLOT_DATA_P64						8
#define AGESG_VAL_TDMA_NADC_TIMESLOT_DATA_EXTERNAL					9

#define AGESG_VAL_TDMA_NADC_CHANNEL_UPLINK							0
#define AGESG_VAL_TDMA_NADC_CHANNEL_DOWNLINK						1
#define AGESG_VAL_TDMA_NADC_CHANNEL_UPLINK_TCH_ALL					2
#define AGESG_VAL_TDMA_NADC_CHANNEL_DOWNLINK_TCH_ALL				3

#define AGESG_VAL_TDMA_NADC_TIMESLOT_TYPE_UPLINK_TRAFFIC_CHANNEL	0
#define AGESG_VAL_TDMA_NADC_TIMESLOT_TYPE_UPLINK_CUSTOM				1
#define AGESG_VAL_TDMA_NADC_TIMESLOT_TYPE_DOWNLINK_TRAFFIC_CHANNEL	2
#define AGESG_VAL_TDMA_NADC_TIMESLOT_TYPE_DOWNLINK_CUSTOM			3

#define AGESG_VAL_TDMA_NADC_FRAME_RATE_FULL							0
#define AGESG_VAL_TDMA_NADC_FRAME_RATE_HALF							1

#define AGESG_VAL_TDMA_PDC_FRAME_RATE_FULL							0
#define AGESG_VAL_TDMA_PDC_FRAME_RATE_HALF							1

#define AGESG_VAL_TDMA_PDC_TIMESLOT_DATA_PN9						0
#define AGESG_VAL_TDMA_PDC_TIMESLOT_DATA_PN15						1
#define AGESG_VAL_TDMA_PDC_TIMESLOT_DATA_FIX4						2
#define AGESG_VAL_TDMA_PDC_TIMESLOT_DATA_USER_FILE					3
#define AGESG_VAL_TDMA_PDC_TIMESLOT_DATA_P4							4
#define AGESG_VAL_TDMA_PDC_TIMESLOT_DATA_P8							5
#define AGESG_VAL_TDMA_PDC_TIMESLOT_DATA_P16						6
#define AGESG_VAL_TDMA_PDC_TIMESLOT_DATA_P32						7
#define AGESG_VAL_TDMA_PDC_TIMESLOT_DATA_P64						8
#define AGESG_VAL_TDMA_PDC_TIMESLOT_DATA_EXTERNAL					9

#define AGESG_VAL_TDMA_PDC_CHANNEL_UPLINK							0
#define AGESG_VAL_TDMA_PDC_CHANNEL_DOWNLINK							1
#define AGESG_VAL_TDMA_PDC_CHANNEL_UPLINK_TCH_ALL					2
#define AGESG_VAL_TDMA_PDC_CHANNEL_DOWNLINK_TCH_ALL					3

#define AGESG_VAL_TDMA_PDC_TIMESLOT_TYPE_UPLINK_TRAFFIC_CHANNEL		0
#define AGESG_VAL_TDMA_PDC_TIMESLOT_TYPE_UPLINK_CUSTOM				1
#define AGESG_VAL_TDMA_PDC_TIMESLOT_TYPE_DOWNLINK_TRAFFIC_CHANNEL	2
#define AGESG_VAL_TDMA_PDC_TIMESLOT_TYPE_DOWNLINK_CUSTOM			3
#define AGESG_VAL_TDMA_PDC_TIMESLOT_TYPE_UPLINK_VOX					4

#define AGESG_VAL_TDMA_GSM_TIMESLOT_DATA_PN9						0
#define AGESG_VAL_TDMA_GSM_TIMESLOT_DATA_PN15						1
#define AGESG_VAL_TDMA_GSM_TIMESLOT_DATA_FIX4						2
#define AGESG_VAL_TDMA_GSM_TIMESLOT_DATA_USER_FILE					3
#define AGESG_VAL_TDMA_GSM_TIMESLOT_DATA_P4							4
#define AGESG_VAL_TDMA_GSM_TIMESLOT_DATA_P8							5
#define AGESG_VAL_TDMA_GSM_TIMESLOT_DATA_P16						6
#define AGESG_VAL_TDMA_GSM_TIMESLOT_DATA_P32						7
#define AGESG_VAL_TDMA_GSM_TIMESLOT_DATA_P64						8
#define AGESG_VAL_TDMA_GSM_TIMESLOT_DATA_EXTERNAL					9
#define AGESG_VAL_TDMA_GSM_TIMESLOT_DATA_TCHFS_PN9					10
#define AGESG_VAL_TDMA_GSM_TIMESLOT_DATA_TCHFS_PN15					11
#define AGESG_VAL_TDMA_GSM_TIMESLOT_DATA_CS1_PN9					12
#define AGESG_VAL_TDMA_GSM_TIMESLOT_DATA_CS1_PN15					13
#define AGESG_VAL_TDMA_GSM_TIMESLOT_DATA_DMCS1_PN9					14
#define AGESG_VAL_TDMA_GSM_TIMESLOT_DATA_DMCS1_PN15					15
#define AGESG_VAL_TDMA_GSM_TIMESLOT_DATA_UMCS1_PN9					16
#define AGESG_VAL_TDMA_GSM_TIMESLOT_DATA_UMCS1_PN15					17

#define AGESG_VAL_TDMA_GSM_TIMESLOT_DATA_TCHFS						20
#define AGESG_VAL_TDMA_GSM_TIMESLOT_DATA_CS1						21
#define AGESG_VAL_TDMA_GSM_TIMESLOT_DATA_DMCS1						22
#define AGESG_VAL_TDMA_GSM_TIMESLOT_DATA_UMCS1						23

#define AGESG_VAL_TDMA_GSM_TIMESLOT_TYPE_NORMAL						0
#define AGESG_VAL_TDMA_GSM_TIMESLOT_TYPE_FREQUENCY_CORRECTION		1
#define AGESG_VAL_TDMA_GSM_TIMESLOT_TYPE_SYNC						2
#define AGESG_VAL_TDMA_GSM_TIMESLOT_TYPE_DUMMY						3
#define AGESG_VAL_TDMA_GSM_TIMESLOT_TYPE_ACCESS						4
#define AGESG_VAL_TDMA_GSM_TIMESLOT_TYPE_CUSTOM						5
#define AGESG_VAL_TDMA_GSM_TIMESLOT_TYPE_NORMAL_ALL					6

#define AGESG_VAL_TDMA_GSM_TRAINING_SEQUENCE_TSC0			"0970897"
#define AGESG_VAL_TDMA_GSM_TRAINING_SEQUENCE_TSC1			"0B778B7"
#define AGESG_VAL_TDMA_GSM_TRAINING_SEQUENCE_TSC2			"10EE90E"
#define AGESG_VAL_TDMA_GSM_TRAINING_SEQUENCE_TSC3			"11ED11E"
#define AGESG_VAL_TDMA_GSM_TRAINING_SEQUENCE_TSC4			"06B906B"
#define AGESG_VAL_TDMA_GSM_TRAINING_SEQUENCE_TSC5			"13AC13A"
#define AGESG_VAL_TDMA_GSM_TRAINING_SEQUENCE_TSC6			"29F629F"
#define AGESG_VAL_TDMA_GSM_TRAINING_SEQUENCE_TSC7			"3BC4BBC"

#define AGESG_VAL_CDMA_OPTIMIZE_FIR_EVM								0
#define AGESG_VAL_CDMA_OPTIMIZE_FIR_ACP								1

#define AGESG_VAL_CDMA_STANDARD_FILTER_ROOT_NYQUIST					0
#define AGESG_VAL_CDMA_STANDARD_FILTER_NYQUIST						1
#define AGESG_VAL_CDMA_STANDARD_FILTER_GAUSSIAN						2
#define AGESG_VAL_CDMA_STANDARD_FILTER_IS95							3
#define AGESG_VAL_CDMA_STANDARD_FILTER_IS95_EQ						4
#define AGESG_VAL_CDMA_STANDARD_FILTER_IS95_MODULATION				5
#define AGESG_VAL_CDMA_STANDARD_FILTER_IS95_MODULATION_EQ			6
#define AGESG_VAL_CDMA_STANDARD_FILTER_IS2000_SR3					7
#define AGESG_VAL_CDMA_STANDARD_FILTER_RECTANGLE					8
#define AGESG_VAL_CDMA_STANDARD_FILTER_WCDMA						9
#define AGESG_VAL_CDMA_STANDARD_FILTER_UN32_GSM						10
#define AGESG_VAL_CDMA_STANDARD_FILTER_APCO25_C4FM					11
#define AGESG_VAL_CDMA_STANDARD_FILTER_USER_FILE					12

#define AGESG_VAL_CDMA_TRIGGER_GATE_ACTIVE_LOW						0
#define AGESG_VAL_CDMA_TRIGGER_GATE_ACTIVE_HIGH						1

#define AGESG_VAL_CDMA_SELECTED_CHANNEL_PILOT						0
#define AGESG_VAL_CDMA_SELECTED_CHANNEL_9CHFWD						1
#define AGESG_VAL_CDMA_SELECTED_CHANNEL_32CHFWD						2
#define AGESG_VAL_CDMA_SELECTED_CHANNEL_64CHFWD						3
#define AGESG_VAL_CDMA_SELECTED_CHANNEL_REVERSE						4
#define AGESG_VAL_CDMA_SELECTED_CHANNEL_CUSTOM_STATE				5
#define AGESG_VAL_CDMA_SELECTED_CHANNEL_3_CARRIERS					6
#define AGESG_VAL_CDMA_SELECTED_CHANNEL_4_CARRIERS					7
#define AGESG_VAL_CDMA_SELECTED_CHANNEL_CUSTOM_CARRIER				8
#define AGESG_VAL_CDMA_SELECTED_CHANNEL_MULTICARRIER				10

#define AGESG_VAL_CDMA_CLIPPING_TYPE_IJQ							0
#define AGESG_VAL_CDMA_CLIPPING_TYPE_IORQ							1

#define AGESG_VAL_CDMA_CLIPPING_FIR_FILTER_PRE						0
#define AGESG_VAL_CDMA_CLIPPING_FIR_FILTER_POST						1

#define AGESG_VAL_WCDMA_SELECT_FORWARD_DPCH1						0
#define AGESG_VAL_WCDMA_SELECT_FORWARD_DPCH3						1
#define AGESG_VAL_WCDMA_SELECT_FORWARD_PERCH						2
#define AGESG_VAL_WCDMA_SELECT_FORWARD_PDPCH1						3
#define AGESG_VAL_WCDMA_SELECT_FORWARD_PDPCH3						4
#define AGESG_VAL_WCDMA_SELECT_FORWARD_PDPCH50						5
#define AGESG_VAL_WCDMA_SELECT_REVERSE_DPCCH						6
#define AGESG_VAL_WCDMA_SELECT_REVERSE_DDPDCH1						7
#define AGESG_VAL_WCDMA_SELECT_REVERSE_DDPDCH2						8
#define AGESG_VAL_WCDMA_SELECT_REVERSE_DDPDCH3						9
#define AGESG_VAL_WCDMA_SELECT_FORWARD_CAR2							10
#define AGESG_VAL_WCDMA_SELECT_FORWARD_CAR3							11
#define AGESG_VAL_WCDMA_SELECT_FORWARD_CAR4							12
#define AGESG_VAL_WCDMA_SELECT_FORWARD_CUSTOM						13
#define AGESG_VAL_WCDMA_SELECT_REVERSE_CUSTOM						14
#define AGESG_VAL_WCDMA_SELECT_FORWARD_MULTI_CUSTOM					15
#define AGESG_VAL_WCDMA_SELECT_FORWARD_MULTI_CARRIER				20

#define AGESG_VAL_WCDMA_STANDARD_CODE_DOMAIN_NOCHANGE				0
#define AGESG_VAL_WCDMA_STANDARD_CODE_DOMAIN_EQUAL					1
#define AGESG_VAL_WCDMA_STANDARD_CODE_DOMAIN_SCALE					2

#define AGESG_VAL_WCDMA_STANDARD_CHIP_RATE_4096						4.096e+6
#define AGESG_VAL_WCDMA_STANDARD_CHIP_RATE_8192						8.192e+6
#define AGESG_VAL_WCDMA_STANDARD_CHIP_RATE_16384					16.384e+6

#define AGESG_VAL_WCDMA_STANDARD_IQ_MAPPING_NORMAL					0
#define AGESG_VAL_WCDMA_STANDARD_IQ_MAPPING_INVERT					1

#define AGESG_VAL_WCDMA_STANDARD_SECOND_DPDCH_I						0
#define AGESG_VAL_WCDMA_STANDARD_SECOND_DPDCH_Q						1

#define AGESG_VAL_WCDMA_SELECT_DOWN_DPCH1							0
#define AGESG_VAL_WCDMA_SELECT_DOWN_DPCH3							1
#define AGESG_VAL_WCDMA_SELECT_DOWN_PSCH							2
#define AGESG_VAL_WCDMA_SELECT_DOWN_PSCHDPCH1						3
#define AGESG_VAL_WCDMA_SELECT_DOWN_PSCHDPCH3						4
#define AGESG_VAL_WCDMA_SELECT_DOWN_MODEL1_DPCH16					5
#define AGESG_VAL_WCDMA_SELECT_DOWN_MODEL1_DPCH32					6
#define AGESG_VAL_WCDMA_SELECT_DOWN_MODEL1_DPCH64					7
#define AGESG_VAL_WCDMA_SELECT_DOWN_MODEL2							8
#define AGESG_VAL_WCDMA_SELECT_DOWN_MODEL3_DPCH16					9
#define AGESG_VAL_WCDMA_SELECT_DOWN_MODEL3_DPCH32					10
#define AGESG_VAL_WCDMA_SELECT_DOWN_MODEL4							11
#define AGESG_VAL_WCDMA_SELECT_UP_DPCCH								12
#define AGESG_VAL_WCDMA_SELECT_UP_DPCCH_DPDCH1						13
#define AGESG_VAL_WCDMA_SELECT_UP_DPCCH_DPDCH2						14
#define AGESG_VAL_WCDMA_SELECT_UP_DPCCH_DPDCH3						15
#define AGESG_VAL_WCDMA_SELECT_UP_DPCCH_DPDCH4						16
#define AGESG_VAL_WCDMA_SELECT_UP_DPCCH_DPDCH5						17
#define AGESG_VAL_WCDMA_SELECT_DOWN_CAR2							18
#define AGESG_VAL_WCDMA_SELECT_DOWN_CAR3							19
#define AGESG_VAL_WCDMA_SELECT_DOWN_CAR4							20
#define AGESG_VAL_WCDMA_SELECT_DOWN_CUSTOM							21
#define AGESG_VAL_WCDMA_SELECT_UP_CUSTOM							22
#define AGESG_VAL_WCDMA_SELECT_DOWN_MULTI_CUSTOM					23
#define AGESG_VAL_WCDMA_SELECT_DOWN_MULTI_CARRIER					24

#define AGESG_VAL_WCDMA_TGPP_CODE_DOMAIN_NOCHANGE					0
#define AGESG_VAL_WCDMA_TGPP_CODE_DOMAIN_EQUAL						1
#define AGESG_VAL_WCDMA_TGPP_CODE_DOMAIN_SCALE						2

#define AGESG_VAL_WCDMA_TGPP_SECOND_DPDCH_I							0
#define AGESG_VAL_WCDMA_TGPP_SECOND_DPDCH_Q							1

#define AGESG_VAL_WCDMA_TGPP_IQ_MAPPING_NORMAL						0
#define AGESG_VAL_WCDMA_TGPP_IQ_MAPPING_INVERT						1

#define AGESG_VAL_WCDMA_TGPP_GAIN_UNIT_DB							0
#define AGESG_VAL_WCDMA_TGPP_GAIN_UNIT_LINEAR						1
#define AGESG_VAL_WCDMA_TGPP_GAIN_UNIT_INDEX						2

/*- Defined values for attribute AGESG_ATTR_CDMA2000_SELECTED_CHANNEL_TYPE -*/
#define AGESG_VAL_CDMA2000_SELECT_FORWARD_SR1_PILOT						0
#define AGESG_VAL_CDMA2000_SELECT_FORWARD_SR3_DIRECT_PILOT				1
#define AGESG_VAL_CDMA2000_SELECT_FORWARD_SR3_MULTICARRIER_PILOT		2
#define AGESG_VAL_CDMA2000_SELECT_FORWARD_SR1_9CHANNELS					3
#define AGESG_VAL_CDMA2000_SELECT_FORWARD_SR3_DIRECT_9CHANNELS			4
#define AGESG_VAL_CDMA2000_SELECT_FORWARD_SR3_MULTICARRIER_9CHANNELS	5
#define AGESG_VAL_CDMA2000_SELECT_REVERSE_SR1_PILOT						6
#define AGESG_VAL_CDMA2000_SELECT_REVERSE_SR3_PILOT						7
#define AGESG_VAL_CDMA2000_SELECT_REVERSE_SR1_5CHANNELS					8
#define AGESG_VAL_CDMA2000_SELECT_REVERSE_SR3_5CHANNELS					9
#define AGESG_VAL_CDMA2000_SELECT_REVERSE_SR1_8CHANNELS					10
#define AGESG_VAL_CDMA2000_SELECT_FORWARD_SR3_2CARRIERS					11
#define AGESG_VAL_CDMA2000_SELECT_FORWARD_3CARRIERS						12
#define AGESG_VAL_CDMA2000_SELECT_FORWARD_4CARRIERS						13
#define AGESG_VAL_CDMA2000_SELECT_FORWARD_CUSTOM						14
#define AGESG_VAL_CDMA2000_SELECT_REVERSE_CUSTOM						15
#define AGESG_VAL_CDMA2000_SELECT_FORWARD_MULTICARRIER_CUSTOM			16
#define AGESG_VAL_CDMA2000_SELECT_FORWARD_MULTI_CARRIER					20

/*- Defined values for attribute AGESG_ATTR_CDMA2000_MULTICARRIER_SPACING -*/
#define AGESG_VAL_CDMA2000_MULTICARIER_SPACING_1_23_MHZ					1.23e+6
#define AGESG_VAL_CDMA2000_MULTICARIER_SPACING_1_25_MHZ					1.25e+6

/*- Defined values for attribute AGESG_ATTR_CDMA2000_IQ_MAPPING -*/
#define AGESG_VAL_CDMA2000_IQ_MAPPING_NORMAL							0
#define AGESG_VAL_CDMA2000_IQ_MAPPING_INVERT							1

/*- Defined values for attribute AGESG_ATTR_CDMA2000_FORWARD_CODE_DOMAIN -*/
/*- Defined values for attribute AGESG_ATTR_CDMA2000_REVERSE_CODE_DOMAIN -*/
#define AGESG_VAL_CDMA2000_CODE_DOMAIN_EQUAL							0
#define AGESG_VAL_CDMA2000_CODE_DOMAIN_SCALE							1

/*- Defined values for attribute AGESG_ATTR_DM_EXTERNAL_ALC_BANDWIDTH -*/
#define AGESG_VAL_DM_EXTERNAL_ALC_BANDWIDTH_NORMAL						0
#define AGESG_VAL_DM_EXTERNAL_ALC_BANDWIDTH_NARROW						1

/*- Defined values for attribute AGESG_ATTR_DM_EXTERNAL_POLARITY -*/
#define AGESG_VAL_DM_EXTERNAL_POLARITY_NORMAL							0
#define AGESG_VAL_DM_EXTERNAL_POLARITY_INVERT							1

/*- Defined values for attribute AGESG_ATTR_DM_EXTERNAL_SOURCE -*/
#define AGESG_VAL_DM_EXTERNAL_SOURCE_EXTERNAL							0
#define AGESG_VAL_DM_EXTERNAL_SOURCE_INTERNAL							1
#define AGESG_VAL_DM_EXTERNAL_SOURCE_BBG1								2
#define AGESG_VAL_DM_EXTERNAL_SOURCE_EXTERNAL_600_OHMS					3
#define AGESG_VAL_DM_EXTERNAL_SOURCE_OFF								4

/*- Defined values for attribute AGESG_ATTR_DM_SKEW_PATH -*/
#define AGESG_VAL_DM_SKEW_PATH_RF										0
#define AGESG_VAL_DM_SKEW_PATH_BB										1

/*- Defined values for attribute AGESG_ATTR_POWER_ALC_SEARCH_SPAN_TYPE -*/
#define AGESG_VAL_POWER_ALC_SEARCH_SPAN_TYPE_FULL						0
#define AGESG_VAL_POWER_ALC_SEARCH_SPAN_TYPE_USER						1

/****************************************************************************
 *--------- IviRFSigGen Class Instrument Driver Function Declarations ---------*
 ****************************************************************************/
    /*- Required VXIplug&play Functions -*/
ViStatus _VI_FUNC agesg_init (
	ViRsrc logicalName,
	ViBoolean idQuery,
	ViBoolean resetDevice,
	ViSession *vi
);

ViStatus _VI_FUNC agesg_WriteInstrData (ViSession vi, ViConstString writeBuffer);

ViStatus _VI_FUNC agesg_ReadInstrData (
	ViSession	vi,
	ViInt32		numBytes,
	ViChar		rdBuf[],
	ViInt32*	bytesRead
);

ViStatus _VI_FUNC agesg_close (ViSession vi);

ViStatus _VI_FUNC agesg_reset (ViSession vi);

ViStatus _VI_FUNC agesg_self_test (
	ViSession vi,
	ViInt16 *selfTestResult,
	ViChar selfTestMessage[]
);

ViStatus _VI_FUNC agesg_error_query (
	ViSession vi,
	ViInt32 *errorCode,
	ViChar errorMessage[]
);

ViStatus _VI_FUNC agesg_error_message (
	ViSession vi,
	ViStatus statusCode,
	ViChar message[]
);

ViStatus _VI_FUNC agesg_revision_query (
	ViSession vi,
	ViChar driverRev[],
	ViChar instrRev[]
);

    /*- Required IVI Functions -*/
ViStatus _VI_FUNC agesg_InitWithOptions (
	ViRsrc logicalName,
	ViBoolean IDQuery,
	ViBoolean resetDevice,
	ViConstString optionString,
	ViSession *vi
);

ViStatus _VI_FUNC agesg_InvalidateAllAttributes (ViSession vi);

    /*- Set, Get, and Check Attribute Functions -*/

ViStatus _VI_FUNC agesg_GetAttributeViInt32 (
	ViSession vi,
	ViConstString channelName,
	ViAttr attributeId,
	ViInt32 *value
);

ViStatus _VI_FUNC agesg_SetAttributeViInt32 (
	ViSession vi,
	ViConstString channelName,
	ViAttr attributeId,
	ViInt32 value
);

ViStatus _VI_FUNC agesg_CheckAttributeViInt32 (
	ViSession vi,
	ViConstString channelName,
	ViAttr attributeId,
	ViInt32 value
);

ViStatus _VI_FUNC agesg_GetAttributeViReal64 (
	ViSession vi,
	ViConstString channelName,
	ViAttr attributeId,
	ViReal64 *value
);

ViStatus _VI_FUNC agesg_SetAttributeViReal64 (
	ViSession vi,
	ViConstString channelName,
	ViAttr attributeId,
	ViReal64 value
);

ViStatus _VI_FUNC agesg_CheckAttributeViReal64 (
	ViSession vi,
	ViConstString channelName,
	ViAttr attributeId,
	ViReal64 value
);

ViStatus _VI_FUNC agesg_GetAttributeViString (
	ViSession vi,
	ViConstString channelName,
	ViAttr attributeId,
	ViInt32 bufferSize,
	ViChar value[]
);

ViStatus _VI_FUNC agesg_SetAttributeViString (
	ViSession vi,
	ViConstString channelName,
	ViAttr attributeId,
	ViConstString value
);

ViStatus _VI_FUNC agesg_CheckAttributeViString (
	ViSession vi,
	ViConstString channelName,
	ViAttr attributeId,
	ViConstString value
);

ViStatus _VI_FUNC agesg_GetAttributeViBoolean (
	ViSession vi,
	ViConstString channelName,
	ViAttr attributeId,
	ViBoolean *value
);

ViStatus _VI_FUNC agesg_SetAttributeViBoolean (
	ViSession vi,
	ViConstString channelName,
	ViAttr attributeId,
	ViBoolean value
);

ViStatus _VI_FUNC agesg_CheckAttributeViBoolean (
	ViSession vi,
	ViConstString channelName,
	ViAttr attributeId,
	ViBoolean value
);

ViStatus _VI_FUNC agesg_GetAttributeViSession (
	ViSession vi,
	ViConstString channelName,
	ViAttr attributeId,
	ViSession *value
);

ViStatus _VI_FUNC agesg_SetAttributeViSession (
	ViSession vi,
	ViConstString channelName,
	ViAttr attributeId,
	ViSession value
);

ViStatus _VI_FUNC agesg_CheckAttributeViSession (
	ViSession vi,
	ViConstString channelName,
	ViAttr attributeId,
	ViSession value
);

    /*- Lock and Unlock Functions -*/
ViStatus _VI_FUNC agesg_LockSession (
	ViSession vi,
	ViBoolean *callerHasLock
);

ViStatus _VI_FUNC agesg_UnlockSession (
	ViSession vi,
	ViBoolean *callerHasLock
);

    /*- Error Information Functions -*/
ViStatus _VI_FUNC agesg_GetError (ViSession vi,
									 ViStatus *errorCode,
                                     ViInt32 bufferSize,
                                     ViChar description[]);

ViStatus _VI_FUNC agesg_ClearError (ViSession vi);

    /*- Interchangeability Checking Functions -*/
ViStatus _VI_FUNC agesg_GetNextInterchangeWarning (
	ViSession vi,
	ViInt32 bufferSize,
	ViChar warning[]
);

ViStatus _VI_FUNC agesg_ClearInterchangeWarnings (ViSession vi);
ViStatus _VI_FUNC agesg_ResetInterchangeCheck (ViSession vi);

ViStatus _VI_FUNC agesg_GetNextCoercionRecord (
	ViSession vi,
	ViInt32 bufferSize,
	ViChar record[]
);

ViStatus _VI_FUNC agesg_ResetWithDefaults (ViSession vi);

ViStatus _VI_FUNC agesg_Disable (ViSession vi);

    /*- IviRFSigGen Fundamental Capabilities -*/
ViStatus _VI_FUNC agesg_ConfigureRF (
	ViSession vi,
	ViReal64 Frequency,
	ViReal64 PowerLevel
);

ViStatus _VI_FUNC agesg_ConfigureALCEnabled (
	ViSession vi,
	ViBoolean ALCEnabled
);

ViStatus _VI_FUNC agesg_ConfigureOutputEnabled (
	ViSession vi,
	ViBoolean Enabled
);

ViStatus _VI_FUNC agesg_ConfigureOutputModulationEnabled (
	ViSession vi,
	ViBoolean OutputEnabled
);

ViStatus _VI_FUNC agesg_ConfigureFrequencyMultiplierCW (
	ViSession	vi,
	ViReal64	Multiplier
);

ViStatus _VI_FUNC agesg_ConfigureFrequencyOffset (
	ViSession vi,
	ViReal64 Offset
);

ViStatus _VI_FUNC agesg_ConfigureFrequencyOptimization (
	ViSession vi,
	ViInt32 Optimization
);

ViStatus _VI_FUNC agesg_ConfigureFrequencyOffsetEnabled (
	ViSession	vi,
	ViBoolean	enabled
);

ViStatus _VI_FUNC agesg_ConfigureFrequencyReference (
	ViSession vi,
	ViReal64 Reference
);

ViStatus _VI_FUNC agesg_ConfigureFrequencyReferenceEnabled (
	ViSession vi,
	ViBoolean Enabled
);

ViStatus _VI_FUNC agesg_ConfigurePhaseAdjustment (
	ViSession vi,
	ViReal64 Adjustment
);

ViStatus _VI_FUNC agesg_SetPhaseReference (ViSession vi);

ViStatus _VI_FUNC agesg_ConfigureAlternateAmplitudeEnabled (
	ViSession vi,
	ViBoolean Enabled
);

ViStatus _VI_FUNC agesg_ConfigureAlternateAmplitude (
	ViSession vi,
	ViReal64 Amplitude
);

ViStatus _VI_FUNC agesg_ConfigureAlternateTriggerSource (
	ViSession vi,
	ViInt32 Source,
	ViInt32 ManualTrigger
);

ViStatus _VI_FUNC agesg_ConfigureAutomaticAttenuationEnabled (
	ViSession vi,
	ViBoolean Enabled
);

ViStatus _VI_FUNC agesg_ConfigurePowerOffset (
	ViSession vi,
	ViReal64 Offset
);

ViStatus _VI_FUNC agesg_ConfigurePowerReference (
	ViSession vi,
	ViReal64 Reference
);

ViStatus _VI_FUNC agesg_ConfigurePowerReferenceEnabled (
	ViSession vi,
	ViBoolean Enabled
);

ViStatus _VI_FUNC agesg_CalibrateDCFM (ViSession vi);

ViStatus _VI_FUNC agesg_DisableAllModulation (ViSession vi);

ViStatus _VI_FUNC agesg_IsSettled (
	ViSession vi,
	ViBoolean *Settled
);

ViStatus _VI_FUNC agesg_WaitUntilSettled (
	ViSession vi,
	ViInt32 MaxTimeMilliseconds
);

ViStatus _VI_FUNC agesg_ConfigureALCSearchSpan (
	ViSession	vi,
	ViReal64	start,
	ViReal64	stop,
	ViInt32		type,
	ViBoolean	enabled
);

    /*- IviRFSigGenModulateAM Extension Group -*/
ViStatus _VI_FUNC agesg_ConfigureAMEnabled (
	ViSession vi,
	ViBoolean Enabled
);
ViStatus _VI_FUNC agesg_ConfigureAM2Enabled (
	ViSession vi,
	ViBoolean Enabled
);

ViStatus _VI_FUNC agesg_ConfigureAMExternalCoupling (
	ViSession vi,
	ViInt32 Coupling
);
ViStatus _VI_FUNC agesg_ConfigureAM2ExternalCoupling (
	ViSession vi,
	ViInt32 Coupling
);

ViStatus _VI_FUNC agesg_ConfigureAM (
	ViSession vi,
	ViConstString Source,
	ViInt32 Scaling,
	ViReal64 Depth
);
ViStatus _VI_FUNC agesg_ConfigureAM2 (
	ViSession vi,
	ViConstString Source,
	ViInt32 Scaling,
	ViReal64 Depth
);

ViStatus _VI_FUNC agesg_ConfigureAMDepthCoupling (
	ViSession vi,
	ViBoolean Enabled
);

ViStatus _VI_FUNC agesg_ConfigureWideAMEnabled (
	ViSession vi,
	ViBoolean Enabled
);


    /*- IviRFSigGenModulateFM Extension Group -*/
ViStatus _VI_FUNC agesg_ConfigureFMEnabled (
	ViSession vi,
	ViBoolean Enabled
);
ViStatus _VI_FUNC agesg_ConfigureFM2Enabled (
	ViSession vi,
	ViBoolean Enabled
);

ViStatus _VI_FUNC agesg_ConfigureFMExternalCoupling (
	ViSession vi,
	ViInt32 Coupling
);
ViStatus _VI_FUNC agesg_ConfigureFM2ExternalCoupling (
	ViSession vi,
	ViInt32 Coupling
);

ViStatus _VI_FUNC agesg_ConfigureFM (
	ViSession vi,
	ViConstString Source,
	ViReal64 Deviation
);
ViStatus _VI_FUNC agesg_ConfigureFM2 (
	ViSession vi,
	ViConstString Source,
	ViReal64 Deviation
);

ViStatus _VI_FUNC agesg_ConfigureFMDeviationCoupling (
	ViSession vi,
	ViBoolean Enabled
);


    /*- IviRFSigGenModulatePM Extension Group -*/
ViStatus _VI_FUNC agesg_ConfigurePMEnabled (
	ViSession vi,
	ViBoolean Enabled
);

ViStatus _VI_FUNC agesg_ConfigurePMBandwidth (
	ViSession	vi,
	ViInt32		bandwidth
);

ViStatus _VI_FUNC agesg_ConfigurePM2Enabled (
	ViSession vi,
	ViBoolean Enabled
);

ViStatus _VI_FUNC agesg_ConfigurePMExternalCoupling (
	ViSession vi,
	ViInt32 Coupling
);
ViStatus _VI_FUNC agesg_ConfigurePM2ExternalCoupling (
	ViSession vi,
	ViInt32 Coupling
);

ViStatus _VI_FUNC agesg_ConfigurePM (
	ViSession vi,
	ViConstString Source,
	ViReal64 Deviation
);
ViStatus _VI_FUNC agesg_ConfigurePM2 (
	ViSession vi,
	ViConstString Source,
	ViReal64 Deviation
);

ViStatus _VI_FUNC agesg_ConfigurePMDeviationCoupling (
	ViSession vi,
	ViBoolean Enabled
);


ViStatus _VI_FUNC agesg_SetActiveExternalSource (
	ViSession		vi,
	ViConstString	activeSource
);

    /*- IviRFSigGenAnalogModulationSource Extension Group -*/
ViStatus _VI_FUNC agesg_GetAnalogModulationSourceName (
	ViSession vi,
	ViInt32 Index,
	ViInt32 NameBufferSize,
	ViChar Name[]
);


    /*- IviRFSigGenModulatePulse Extension Group -*/
ViStatus _VI_FUNC agesg_ConfigurePulseModulationEnabled (
	ViSession vi,
	ViBoolean Enabled
);

ViStatus _VI_FUNC agesg_ConfigurePulseModulationSource (
    ViSession vi,
    ViInt32 Source
);

ViStatus _VI_FUNC agesg_ConfigurePulseModulationExternalPolarity (
	ViSession vi,
	ViInt32 Polarity
);

ViStatus _VI_FUNC agesg_ConfigureFastPulseModulationEnabled (
	ViSession vi,
	ViBoolean Enabled
);

ViStatus _VI_FUNC agesg_ConfigurePulseWaveform (
	ViSession vi,
	ViInt32 Waveform
);

ViStatus _VI_FUNC agesg_ConfigurePulseModulationFrequency (
	ViSession vi,
	ViReal64 Frequency
);


    /*- IviRFSigGenLFGenerator Extension Group -*/
ViStatus _VI_FUNC agesg_GetLFGeneratorName (
	ViSession vi,
	ViInt32 Index,
	ViInt32 NameBufferSize,
	ViChar Name[]
);

ViStatus _VI_FUNC agesg_SetActiveLFGenerator (
	ViSession vi,
	ViConstString ActiveLFGenerator
);

ViStatus _VI_FUNC agesg_ConfigureLFGenerator (
	ViSession vi,
	ViReal64 Frequency,
	ViInt32 Waveform
);
ViStatus _VI_FUNC agesg_ConfigureDualSine (
	ViSession vi,
	ViReal64 Frequency,
	ViReal64 Amplitude
);
ViStatus _VI_FUNC agesg_ConfigureSweptSine (
	ViSession vi,
	ViReal64 StartFrequency,
	ViReal64 StopFrequency,
	ViReal64 SweepTime
);
ViStatus _VI_FUNC agesg_ConfigureSweepTrigger (
	ViSession vi,
	ViInt32 source
);

ViStatus _VI_FUNC agesg_ConfigureFunctionGeneratorPulse (
	ViSession vi,
	ViReal64 Width,
	ViReal64 Period
);

    /*- IviRFSigGenLFGeneratorOutput Extension Group -*/
ViStatus _VI_FUNC agesg_ConfigureLFGeneratorOutput (
	ViSession vi,
	ViReal64 Amplitude,
	ViBoolean Enabled
);


    /*- IviRFSigGenPulseGenerator Extension Group -*/
ViStatus _VI_FUNC agesg_ConfigurePulseExternalTrigger (
	ViSession vi,
	ViInt32 Slope,
	ViReal64 Delay
);

ViStatus _VI_FUNC agesg_ConfigurePulseInternalTrigger (
	ViSession vi,
	ViReal64 Period
);

ViStatus _VI_FUNC agesg_ConfigurePulse (
	ViSession vi,
	ViInt32 PulseTriggerSource,
	ViReal64 PulseWidth,
	ViBoolean GatingEnabled
);


    /*- IviRFSigGenPulseGeneratorOutput Extension Group -*/
ViStatus _VI_FUNC agesg_ConfigurePulseOutput (
	ViSession vi,
	ViInt32 Polarity,
	ViBoolean Enabled
);


    /*- IviRFSigGenSweep Extension Group -*/
ViStatus _VI_FUNC agesg_ConfigureSweep (
	ViSession vi,
	ViInt32 Mode,
	ViInt32 TriggerSource
);


    /*- IviRFSigGenFrequencySweep Extension Group -*/

    /*- IviRFSigGenPowerSweep Extension Group -*/

    /*- IviRFSigGenFrequencyStep Extension Group -*/
ViStatus _VI_FUNC agesg_ConfigureFrequencyStepStartStop (
	ViSession vi,
	ViReal64 Start,
	ViReal64 Stop,
	ViInt32 Scaling,
	ViReal64 StepSize
);

ViStatus _VI_FUNC agesg_ConfigureFrequencyStepDwell (
	ViSession vi,
	ViBoolean SingleStepEnabled,
	ViReal64 Dwell
);

ViStatus _VI_FUNC agesg_ResetFrequencyStep (ViSession vi);


    /*- IviRFSigGenPowerStep Extension Group -*/
ViStatus _VI_FUNC agesg_ConfigurePowerStepStartStop (
	ViSession vi,
	ViReal64 Start,
	ViReal64 Stop,
	ViReal64 StepSize
);

ViStatus _VI_FUNC agesg_ConfigurePowerStepDwell (
	ViSession vi,
	ViBoolean SingleStepEnabled,
	ViReal64 Dwell
);

ViStatus _VI_FUNC agesg_ResetPowerStep (ViSession vi);


    /*- IviRFSigGenList Extension Group -*/
ViStatus _VI_FUNC agesg_CreateFrequencyList (
	ViSession vi,
	ViConstString Name,
	ViInt32 Length,
	ViReal64 Frequency[]
);

ViStatus _VI_FUNC agesg_CreatePowerList (
	ViSession vi,
	ViConstString Name,
	ViInt32 Length,
	ViReal64 Power[]
);

ViStatus _VI_FUNC agesg_CreateFrequencyPowerList (
	ViSession vi,
	ViConstString Name,
	ViInt32 Length,
	ViReal64 Frequency[],
	ViReal64 Power[]
);

ViStatus _VI_FUNC agesg_SelectList (
	ViSession vi,
	ViConstString Name
);

ViStatus _VI_FUNC agesg_ClearAllLists (ViSession vi);

ViStatus _VI_FUNC agesg_ConfigureListDwell (
	ViSession vi,
	ViBoolean SingleStepEnabled,
	ViReal64 Dwell
);

ViStatus _VI_FUNC agesg_ResetList (ViSession vi);

ViStatus _VI_FUNC agesg_ConfigureSweepDirection (
	ViSession vi,
	ViInt32 Direction
);

ViStatus _VI_FUNC agesg_ConfigureListMode (
	ViSession vi,
	ViInt32 Mode
);

ViStatus _VI_FUNC agesg_SelectManualPoint (
	ViSession vi,
	ViInt32 Point
);

ViStatus _VI_FUNC agesg_DoSingleSweep (
	ViSession vi
);

    /*- IviRFSigGenALC Extension Group -*/
ViStatus _VI_FUNC agesg_ConfigureALC (
	ViSession vi,
	ViInt32 Source,
	ViReal64 Bandwidth
);

ViStatus _VI_FUNC agesg_ConfigureAlcSearchEnabled (
	ViSession vi,
	ViBoolean Enabled
);

ViStatus _VI_FUNC agesg_DoPowerSearch (
	ViSession vi
);

ViStatus _VI_FUNC agesg_ConfigureALCSearchReference (
	ViSession vi,
	ViInt32 Reference
);


    /*- IviRFSigGenReferenceOscillator Extension Group -*/
ViStatus _VI_FUNC agesg_ConfigureReferenceOscillator (
	ViSession vi,
	ViInt32 Source,
	ViReal64 Frequency
);


    /*- IviRFSigGenSoftwareTrigger Extension Group -*/
ViStatus _VI_FUNC agesg_SendSoftwareTrigger (ViSession vi);


    /*- IviRFSigGenModulateIQ Extension Group -*/
ViStatus _VI_FUNC agesg_ConfigureIQEnabled (
	ViSession vi,
	ViBoolean Enabled
);

ViStatus _VI_FUNC agesg_ConfigureIQ (
	ViSession vi,
	ViInt32 Source,
	ViBoolean SwapEnabled
);

ViStatus _VI_FUNC agesg_ConfigureIQSkew (
	ViSession	vi,
	ViInt32		skewPath,
	ViBoolean	skewEnabled
);

ViStatus _VI_FUNC agesg_CalibrateIQ (ViSession vi);

ViStatus _VI_FUNC agesg_DefaultCalibrateIQ (ViSession vi);

ViStatus _VI_FUNC agesg_CalibrateIQRange (
	ViSession vi,
	ViReal64 start,
	ViReal64 stop
);

ViStatus _VI_FUNC agesg_ConfigureIQExternalImpairment (
	ViSession	vi,
	ViReal64	commonOffset,
	ViReal64	differentialIOffset,
	ViReal64	differentialQOffset,
	ViReal64	gain,
	ViReal64	iOffset,
	ViReal64	qOffset,
	ViReal64	attenuationLevel
);

ViStatus _VI_FUNC agesg_ConfigureIQInputSkew (
	ViSession	vi,
	ViReal64	skew
);

ViStatus _VI_FUNC agesg_ConfigureHighCrestEnabled (
	ViSession vi,
	ViBoolean Enabled
);


    /*- IviRFSigGenIQImpairment Extension Group -*/
ViStatus _VI_FUNC agesg_ConfigureIQImpairmentEnabled (
	ViSession vi,
	ViBoolean Enabled
);

ViStatus _VI_FUNC agesg_ConfigureIQImpairment (
	ViSession vi,
	ViReal64 IOffset,
	ViReal64 QOffset,
	ViReal64 Ratio,
	ViReal64 Skew
);


    /*- IviRFSigGenArbGenerator Extension Group -*/
ViStatus _VI_FUNC agesg_ConfigureArb (
	ViSession vi,
	ViReal64 ClockFrequency,
	ViReal64 FilterFrequency
);

ViStatus _VI_FUNC agesg_WriteArbWaveform (
	ViSession vi,
	ViConstString Name,
	ViInt32 NumberOfSamples,
	ViReal64 IData[],
	ViReal64 QData[],
	ViBoolean MoreDataPending
);

ViStatus _VI_FUNC agesg_SelectArbWaveform (
	ViSession vi,
	ViConstString Name
);

ViStatus _VI_FUNC agesg_ClearAllArbWaveforms (ViSession vi);

ViStatus _VI_FUNC agesg_QueryArbWaveformCapabilities (
	ViSession vi,
	ViInt32 *MaxNumberWaveforms,
	ViInt32 *WaveformQuantum,
	ViInt32 *MinWaveformSize,
	ViInt32 *MaxWaveformSize
);

ViStatus _VI_FUNC agesg_ConfigureArbTriggerSource (
	ViSession vi,
	ViInt32 Source
);

ViStatus _VI_FUNC agesg_ConfigureArbExternalTriggerSlope (
	ViSession vi,
	ViInt32 Slope
);


    /*- IviRFSigGenCDMABase Extension Group -*/
ViStatus _VI_FUNC agesg_GetCDMAStandardName (
	ViSession vi,
	ViInt32 Index,
	ViInt32 NameBufferSize,
	ViChar Name[]
);

ViStatus _VI_FUNC agesg_SelectCDMAStandard (
	ViSession vi,
	ViConstString Name
);

ViStatus _VI_FUNC agesg_ConfigureCDMAClockSource (
	ViSession vi,
	ViInt32 Source
);

ViStatus _VI_FUNC agesg_ConfigureCDMATriggerSource (
	ViSession vi,
	ViInt32 Source
);

ViStatus _VI_FUNC agesg_ConfigureCDMAExternalTriggerSlope (
	ViSession vi,
	ViInt32 Slope
);

ViStatus _VI_FUNC agesg_GetCDMATestModelName (
	ViSession vi,
	ViInt32 Index,
	ViInt32 NameBufferSize,
	ViChar Name[]
);

ViStatus _VI_FUNC agesg_SelectCDMATestModel (
	ViSession vi,
	ViConstString Name
);

ViStatus _VI_FUNC agesg_ConfigureCDMAFilter (
	ViSession vi,
	ViInt32 Select,
	ViReal64 Alpha,
	ViReal64 BbT,
	ViInt32 OptimizeFIR,
	ViConstString UserFile
);

ViStatus _VI_FUNC agesg_ConfigureCDMATriggerAdvanced (
	ViSession vi,
	ViBoolean ExternalDelayEnabled,
	ViReal64 DelayTime,
	ViBoolean RetriggerEnabled,
	ViInt32 TriggerGate
);

ViStatus _VI_FUNC agesg_SelectCDMAChannel (
	ViSession vi,
	ViInt32 ChannelSelect,
	ViConstString UserFile
);

ViStatus _VI_FUNC agesg_DefineCDMAChannel (
	ViSession vi,
	ViReal64 ChipRate,
	ViReal64 FilterFrequency,
	ViInt32 WaveformLength,
	ViInt32 OversampleRatio,
	ViInt32 ARBReference,
	ViReal64 ReferenceFrequency
);

ViStatus _VI_FUNC agesg_ConfigureCDMAClipping (
	ViSession vi,
	ViInt32 Type,
	ViReal64 LevelIjQ,
	ViReal64 LevelI,
	ViReal64 LevelQ,
	ViInt32 Filter
);

ViStatus _VI_FUNC agesg_SelectWCDMAChannel (
	ViSession vi,
	ViInt32 ChannelSelect,
	ViConstString UserFile
);

ViStatus _VI_FUNC agesg_ConfigureWCDMAForwardChannel (
	ViSession vi,
	ViReal64 ChipRate,
	ViBoolean TFCIEnabled,
	ViInt32 CodeDomain
);

ViStatus _VI_FUNC agesg_ConfigureWCDMAReverseChannel (
	ViSession vi,
	ViReal64 ChipRate,
	ViBoolean TFCIEnabled,
	ViConstString ScrambleCode,
	ViInt32 SecondDPDCH
);

ViStatus _VI_FUNC agesg_DefineWCDMAChannel (
	ViSession vi,
	ViReal64 FilterFrequency,
	ViInt32 ARBReference,
	ViReal64 ReferenceFrequency,
	ViInt32 IQMapping
);

ViStatus _VI_FUNC agesg_SelectWCDMA3GPPChannel (
	ViSession vi,
	ViInt32 ChannelSelect,
	ViConstString UserFile
);

ViStatus _VI_FUNC agesg_ConfigureWCDMA3GPPDownChannel (
	ViSession vi,
	ViReal64 ChipRate,
	ViBoolean TFCIEnabled,
	ViInt32 CodeDomain
);

ViStatus _VI_FUNC agesg_ConfigureWCDMA3GPPUpChannel (
	ViSession vi,
	ViReal64 ChipRate,
	ViBoolean TFCIEnabled,
	ViConstString ScrambleCode,
	ViInt32 SecondDPDCH,
	ViInt32 GainUnit
);

ViStatus _VI_FUNC agesg_DefineWCDMA3GPPChannel (
	ViSession vi,
	ViReal64 FilterFrequency,
	ViInt32 ARBReference,
	ViReal64 ReferenceFrequency,
	ViInt32 IQMapping
);

ViStatus _VI_FUNC agesg_SelectWCDMA2000Channel (
	ViSession vi,
	ViInt32 ChannelSelect,
	ViConstString UserFile
);

ViStatus _VI_FUNC agesg_DefineCDMA2000Channel (
	ViSession vi,
	ViReal64 FilterFrequency,
	ViInt32 ARBReference,
	ViReal64 ReferenceFrequency,
	ViReal64 Spacing,
	ViInt32 IQMapping
);

    /*- IviRFSigGenTDMABase Extension Group -*/
ViStatus _VI_FUNC agesg_GetTDMAStandardName (
	ViSession vi,
	ViInt32 Index,
	ViInt32 NameBufferSize,
	ViChar Name[]
);

ViStatus _VI_FUNC agesg_SelectTDMAStandard (
	ViSession vi,
	ViConstString Name
);

ViStatus _VI_FUNC agesg_ConfigureTDMAClockSource (
	ViSession vi,
	ViInt32 Source,
	ViInt32 Type
);

ViStatus _VI_FUNC agesg_ConfigureTDMATriggerSource (
	ViSession vi,
	ViInt32 Source
);

ViStatus _VI_FUNC agesg_ConfigureTDMAExternalTriggerSlope (
	ViSession vi,
	ViInt32 Slope
);

ViStatus _VI_FUNC agesg_GetTDMAFrameName (
	ViSession vi,
	ViInt32 Index,
	ViInt32 NameBufferSize,
	ViChar Name[]
);

ViStatus _VI_FUNC agesg_SelectTDMAFrame (
	ViSession vi,
	ViConstString Name
);

ViStatus _VI_FUNC agesg_ConfigureTDMABurstEnabled (
	ViSession vi,
	ViBoolean Enabled
);

	/*- DECT standard -*/
ViStatus _VI_FUNC agesg_ConfigureTDMAPattern (
	ViSession vi,
	ViInt32	PatternData,
	ViInt32 PatternDataFix4,
	ViConstString UserFile,
	ViInt32 PatternTrigger,
	ViBoolean DelayEnabled,
	ViInt32 DelayBits
);

ViStatus _VI_FUNC agesg_ConfigureDECTTimeslotLC (
	ViSession vi,
	ViInt32 TransmitLink,
	ViInt32	TimeslotNumber,
	ViBoolean TimeslotEnabled,
	ViInt32 Preamble,
	ViInt32 SynchronizationWord,
	ViString AField,
	ViInt32 BField,
	ViInt32 FIX4Data,
	ViConstString UserFile
);

ViStatus _VI_FUNC agesg_ConfigureDECTTimeslotLCZ (
	ViSession vi,
	ViInt32 TransmitLink,
	ViInt32	TimeslotNumber,
	ViBoolean TimeslotEnabled,
	ViInt32 Preamble,
	ViInt32 SynchronizationWord,
	ViString AField,
	ViInt32 BField,
	ViInt32 FIX4Data,
	ViConstString UserFile
);

ViStatus _VI_FUNC agesg_ConfigureDECTTimeslotTB (
	ViSession vi,
	ViInt32 TransmitLink,
	ViInt32	TimeslotNumber,
	ViBoolean TimeslotEnabled,
	ViInt32 Preamble,
	ViInt32 SynchronizationWord,
	ViString AField,
	ViInt32 BField,
	ViInt32 FIX4Data,
	ViConstString UserFile
);

ViStatus _VI_FUNC agesg_ConfigureDECTTimeslotTBZ (
	ViSession vi,
	ViInt32 TransmitLink,
	ViInt32	TimeslotNumber,
	ViBoolean TimeslotEnabled,
	ViInt32 Preamble,
	ViInt32 SynchronizationWord,
	ViString AField,
	ViInt32 BField,
	ViInt32 FIX4Data,
	ViConstString UserFile
);

ViStatus _VI_FUNC agesg_ConfigureDECTTimeslotDB (
	ViSession vi,
	ViInt32 DummyType,
	ViInt32	TimeslotNumber,
	ViBoolean TimeslotEnabled,
	ViInt32 Preamble,
	ViInt32 SynchronizationWord,
	ViString AField
);

ViStatus _VI_FUNC agesg_ConfigureDECTTimeslotCustom (
	ViSession vi,
	ViInt32 TransmitLink,
	ViInt32	TimeslotNumber,
	ViBoolean TimeslotEnabled,
	ViInt32 CustomData,
	ViInt32 FIX4Data,
	ViConstString UserFile
);

ViStatus _VI_FUNC agesg_ConfigureTDMAFrame (
	ViSession vi,
	ViInt32 PN9Mode,
	ViBoolean	SecondaryFrameEnabled,
	ViInt32 SecondaryFrameTrigger,
	ViInt32 TriggerSource,
	ViBoolean ExternalDelayEnabled,
	ViInt32 ExternalDelayBits
);

ViStatus _VI_FUNC agesg_ConfigureTDMASyncOut (
	ViSession vi,
	ViInt32 SyncOutType,
	ViInt32 SyncOutOffset,
	ViInt32 SyncOutBeginTimeslot
);

ViStatus _VI_FUNC agesg_ConfigureTDMAFrequencyChannels (
	ViSession vi,
	ViBoolean FrequencyChannelEnabled,
	ViInt32 FrequencyChannelNumber,
	ViInt32 FrequencyChannelBand
);

ViStatus _VI_FUNC agesg_ModifyTDMAStandard (
	ViSession vi,
	ViInt32 PhasePolarity,
	ViReal64 SymbolRate,
	ViReal64 BurstRiseTime,
	ViReal64 BurstRiseDelay,
	ViReal64 BurstFallTime,
	ViReal64 BurstFallDelay
);

ViStatus _VI_FUNC agesg_ConfigureTDMAIQScaling (
	ViSession vi,
	ViInt32 IQScaling
);

ViStatus _VI_FUNC agesg_ConfigureTDMAFilter (
	ViSession vi,
	ViInt32 FilterSelect,
	ViConstString UserFile,
	ViReal64 FilterAlpha,
	ViReal64 FilterBbT,
	ViInt32 OptimizeFIR
);

ViStatus _VI_FUNC agesg_ConfigureTDMAModulationType (
	ViSession vi,
	ViInt32 ModulationType,
	ViReal64 PhaseMSK,
	ViReal64 FreqDevFSK,
	ViConstString UserFile
);

ViStatus _VI_FUNC agesg_ConfigureTETRATimeslotNormal (
	ViSession vi,
	ViInt32 Number,
	ViBoolean Enabled,
	ViInt32 Type,
	ViInt32 Data,
	ViInt32 Fix4,
	ViConstString UserFile,
	ViInt32 TrainingSequence,
	ViInt32 B1,
	ViInt32 B2
);

ViStatus _VI_FUNC agesg_ConfigureTETRATimeslotControl (
	ViSession vi,
	ViInt32 Number,
	ViBoolean Enabled,
	ViInt32 Type,
	ViInt32 Data,
	ViInt32 Fix4,
	ViConstString UserFile,
	ViInt32 TrainingSequence
);

ViStatus _VI_FUNC agesg_ConfigureTETRATimeslotSync (
	ViSession vi,
	ViInt32 Number,
	ViBoolean Enabled,
	ViInt32 Type,
	ViInt32 Data,
	ViInt32 Fix4,
	ViConstString UserFile,
	ViConstString FCOR,
	ViConstString SSB,
	ViConstString STS,
	ViInt32 BroadcastBits
);

ViStatus _VI_FUNC agesg_ConfigureTETRATimeslotCustom (
	ViSession vi,
	ViInt32 Number,
	ViBoolean Enabled,
	ViInt32 Type,
	ViInt32 Data,
	ViInt32 Fix4,
	ViConstString UserFile
);

ViStatus _VI_FUNC agesg_ConfigureTETRAScramble (
	ViSession vi,
	ViBoolean Enabled,
	ViConstString Seed
);

ViStatus _VI_FUNC agesg_ConfigurePHSScramble (
	ViSession vi,
	ViBoolean Enabled,
	ViConstString Seed
);

ViStatus _VI_FUNC agesg_ConfigurePHSTimeslotTCH (
	ViSession vi,
	ViInt32 Number,
	ViBoolean Enabled,
	ViInt32 Type,
	ViInt32 Data,
	ViInt32 Fix4,
	ViConstString UserFile,
	ViInt32 UniqueWord,
	ViInt32	Sacch
);

ViStatus _VI_FUNC agesg_ConfigurePHSTimeslotSYNC (
	ViSession vi,
	ViInt32 Number,
	ViBoolean Enabled,
	ViInt32 Type,
	ViConstString UniqueWord,
	ViConstString CellStationId,
	ViInt32 PersonalStationId,
	ViConstString IdleField
);

ViStatus _VI_FUNC agesg_ConfigurePHSTimeslotCustom (
	ViSession vi,
	ViInt32 Number,
	ViBoolean Enabled,
	ViInt32 Type,
	ViInt32 Data,
	ViInt32 Fix4,
	ViConstString UserFile
);

ViStatus _VI_FUNC agesg_ConfigureNADCTimeslotTCH (
	ViSession vi,
	ViInt32 Number,
	ViBoolean Enabled,
	ViInt32 Type,
	ViInt32 Data,
	ViInt32 Fix4,
	ViConstString UserFile,
	ViInt32 SW,
	ViInt32 SACCH,
	ViInt32 CDVCC,
	ViInt32 CDL
);

ViStatus _VI_FUNC agesg_ConfigureNADCTimeslotCustom (
	ViSession vi,
	ViInt32 Number,
	ViBoolean Enabled,
	ViInt32 Type,
	ViInt32 Data,
	ViInt32 Fix4,
	ViConstString UserFile
);

ViStatus _VI_FUNC agesg_ConfigureNADCTransmissionRate (
	ViSession vi,
	ViInt32 Rate
);

ViStatus _VI_FUNC agesg_ConfigurePDCTransmissionRate (
	ViSession vi,
	ViInt32 Rate
);

ViStatus _VI_FUNC agesg_ConfigurePDCTimeslotTCH (
	ViSession vi,
	ViInt32 Number,
	ViBoolean Enabled,
	ViInt32 Type,
	ViInt32 Data,
	ViInt32 Fix4,
	ViConstString UserFile,
	ViInt32 SynchronizationWord,
	ViInt32 SACCH,
	ViInt32 ColorCode
);

ViStatus _VI_FUNC agesg_ConfigurePDCTimeslotVOX (
	ViSession vi,
	ViInt32 Number,
	ViBoolean Enabled,
	ViInt32 Type,
	ViInt32 SynchronizationWord,
	ViInt32 SACCH,
	ViInt32 ColorCode
);

ViStatus _VI_FUNC agesg_ConfigurePDCTimeslotCustom (
	ViSession vi,
	ViInt32 Number,
	ViBoolean Enabled,
	ViInt32 Type,
	ViInt32 Data,
	ViInt32 Fix4,
	ViConstString UserFile
);

ViStatus _VI_FUNC agesg_ConfigureGSMTimeslotNormal (
	ViSession vi,
	ViInt32 Number,
	ViBoolean Enabled,
	ViInt32 Type,
	ViInt32 Data,
	ViInt32 Fix4,
	ViConstString UserFile,
	ViInt32 StealBit,
	ViConstString TrainingSequence
);

ViStatus _VI_FUNC agesg_ConfigureGSMTimeslotFCorr (
	ViSession vi,
	ViInt32 Number,
	ViBoolean Enabled
);

ViStatus _VI_FUNC agesg_ConfigureGSMTimeslotSync (
	ViSession vi,
	ViInt32 Number,
	ViBoolean Enabled,
	ViInt32 Data,
	ViInt32 Fix4,
	ViConstString UserFile,
	ViConstString TrainingSequence
);

ViStatus _VI_FUNC agesg_ConfigureGSMTimeslotDummy (
	ViSession vi,
	ViInt32 Number,
	ViBoolean Enabled,
	ViConstString TrainingSequence
);

ViStatus _VI_FUNC agesg_ConfigureGSMTimeslotAccess (
	ViSession vi,
	ViInt32 Number,
	ViBoolean Enabled,
	ViInt32 Data,
	ViInt32 Fix4,
	ViConstString UserFile,
	ViInt32 ExtendedTail,
	ViConstString SynchronizationSequence
);

ViStatus _VI_FUNC agesg_ConfigureGSMTimeslotCustom (
	ViSession vi,
	ViInt32 Number,
	ViBoolean Enabled,
	ViInt32 Data,
	ViInt32 Fix4,
	ViConstString UserFile
);

ViStatus _VI_FUNC agesg_ConfigureGSMDifferentialData (
	ViSession vi,
	ViBoolean Enabled
);

ViStatus _VI_FUNC agesg_ConfigureCustomDifferentialData (
	ViSession vi,
	ViBoolean Enabled
);

ViStatus _VI_FUNC agesg_ConfigureBurstGateInputPolarity (
	ViSession vi,
	ViInt32 Polarity
);

ViStatus _VI_FUNC agesg_ConfigureDataClockInputPolarity (
	ViSession vi,
	ViInt32 Polarity
);

ViStatus _VI_FUNC agesg_ConfigureDataClockOutputPolarity (
	ViSession vi,
	ViInt32 Polarity
);

ViStatus _VI_FUNC agesg_ConfigureDataInputPolarity (
	ViSession vi,
	ViInt32 Polarity
);

ViStatus _VI_FUNC agesg_ConfigureDataOutputPolarity (
	ViSession vi,
	ViInt32 Polarity
);

ViStatus _VI_FUNC agesg_ConfigureEventOutputPolarity (
	ViSession vi,
	ViConstString Event,
	ViInt32 Polarity
);

ViStatus _VI_FUNC agesg_ConfigurePatternTriggerInputPolarity (
	ViSession vi,
	ViInt32 Polarity
);

ViStatus _VI_FUNC agesg_ConfigureSymbolSyncInputPolarity (
	ViSession vi,
	ViInt32 Polarity
);

ViStatus _VI_FUNC agesg_ConfigureSymbolSyncOutputPolarity (
	ViSession vi,
	ViInt32 Polarity
);

ViStatus _VI_FUNC agesg_ConfigureConnectorsEnabled (
	ViSession vi,
	ViBoolean Enabled
);

ViStatus _VI_FUNC agesg_ConfigureTriggerOutputPolarity (
	ViSession vi,
	ViInt32 Polarity
);

/* Obsolete attributes and functions */
#define AGESG_ATTR_FREQUENCY_MULTIPLIER						  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 30L)	/* ViInt32 */

ViStatus _VI_FUNC agesg_ConfigureFrequencyMultiplier (
	ViSession vi,
	ViInt32 Multiplier
);


    /*********************************************************
        Functions reserved for class driver use only.
        End-users should not call these functions.
     *********************************************************/
ViStatus _VI_FUNC  agesg_IviInit (ViRsrc resourceName, ViBoolean IDQuery,
                                     ViBoolean reset, ViSession vi);
ViStatus _VI_FUNC  agesg_IviClose (ViSession vi);

/****************************************************************************
 *--------------- IviRFSigGen Class Error And Completion Codes ----------------*
 ****************************************************************************/
#define AGESG_ERROR_TRIGGER_NOT_SOFTWARE   IVIRFSIGGEN_ERROR_TRIGGER_NOT_SOFTWARE
#define AGESG_ERROR_MAX_TIME_EXCEEDED      IVIRFSIGGEN_ERROR_MAX_TIME_EXCEEDED
#define AGESG_ERROR_LIST_UNKNOWN           IVIRFSIGGEN_ERROR_LIST_UNKNOWN

/****************************************************************************
 *---------------------------- End Include File ----------------------------*
 ****************************************************************************/
#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

#endif /* IVIRFSIGGEN_HEADER */
