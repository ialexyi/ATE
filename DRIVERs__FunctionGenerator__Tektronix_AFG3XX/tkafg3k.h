/*****************************************************************************
 *    Copyright 2012, National Instruments, Corporation.  All Rights Reserved.
 *****************************************************************************/

/******************************************************************************
 *                       Tektronix AFG3000 Series Arbitrary Function Generator
 *
 * Title:    tkafg3k.h
 * Purpose:  Tektronix AFG3000 & AFG2000 Series Arbitrary Function Generator
 *           instrument driver declarations.
 *
 *****************************************************************************/

#ifndef __tkafg3k_HEADER
#define __tkafg3k_HEADER

#include <ivi.h>
#include <ivifgen.h>

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

/****************************************************************************
 *----------------- Instrument Driver Revision Information -----------------*
 ****************************************************************************/
#define TKAFG3K_MAJOR_VERSION                3      /* Instrument driver major version   */
#define TKAFG3K_MINOR_VERSION                4      /* Instrument driver minor version   */

#define TKAFG3K_CLASS_SPEC_MAJOR_VERSION     3     /* Class specification major version */
#define TKAFG3K_CLASS_SPEC_MINOR_VERSION     0     /* Class specification minor version */

#define TKAFG3K_SUPPORTED_INSTRUMENT_MODELS  "AFG3011, AFG3011C, AFG3021, AFG3021B, AFG3021C, AFG3022, AFG3022B, AFG3022C, AFG3101, AFG3101C, AFG3102, AFG3102C, AFG3051C, AFG3052C, AFG3251, AFG3251C, AFG3252, AFG3252C, AFG2021"

#define TKAFG3K_DRIVER_VENDOR                "National Instruments"

#ifdef _IVI_mswin64_
#define TKAFG3K_DRIVER_DESCRIPTION           "Tektronix AFG3000 Series Arbitrary Funtion Generator [Compiled for 64-bit]"
#else
#define TKAFG3K_DRIVER_DESCRIPTION           "Tektronix AFG3000 Series Arbitrary Funtion Generator"
#endif

/****************************************************************************
 *---------------------------- Attribute Defines ---------------------------*
 ****************************************************************************/

    /*- IVI Inherent Instrument Attributes ---------------------------------*/

        /* User Options */
#define TKAFG3K_ATTR_RANGE_CHECK                                IVI_ATTR_RANGE_CHECK                            /* ViBoolean */
#define TKAFG3K_ATTR_QUERY_INSTRUMENT_STATUS                    IVI_ATTR_QUERY_INSTRUMENT_STATUS                /* ViBoolean */
#define TKAFG3K_ATTR_CACHE                                      IVI_ATTR_CACHE                                  /* ViBoolean */
#define TKAFG3K_ATTR_SIMULATE                                   IVI_ATTR_SIMULATE                               /* ViBoolean */
#define TKAFG3K_ATTR_RECORD_COERCIONS                           IVI_ATTR_RECORD_COERCIONS                       /* ViBoolean */
#define TKAFG3K_ATTR_INTERCHANGE_CHECK                          IVI_ATTR_INTERCHANGE_CHECK                      /* ViBoolean */

        /* Driver Information  */
#define TKAFG3K_ATTR_SPECIFIC_DRIVER_PREFIX                     IVI_ATTR_SPECIFIC_DRIVER_PREFIX                 /* ViString, read-only  */
#define TKAFG3K_ATTR_SUPPORTED_INSTRUMENT_MODELS                IVI_ATTR_SUPPORTED_INSTRUMENT_MODELS            /* ViString, read-only  */
#define TKAFG3K_ATTR_GROUP_CAPABILITIES                         IVI_ATTR_GROUP_CAPABILITIES                     /* ViString, read-only  */
#define TKAFG3K_ATTR_INSTRUMENT_MANUFACTURER                    IVI_ATTR_INSTRUMENT_MANUFACTURER                /* ViString, read-only  */
#define TKAFG3K_ATTR_INSTRUMENT_MODEL                           IVI_ATTR_INSTRUMENT_MODEL                       /* ViString, read-only  */
#define TKAFG3K_ATTR_INSTRUMENT_FIRMWARE_REVISION               IVI_ATTR_INSTRUMENT_FIRMWARE_REVISION           /* ViString, read-only  */
#define TKAFG3K_ATTR_SPECIFIC_DRIVER_REVISION                   IVI_ATTR_SPECIFIC_DRIVER_REVISION               /* ViString, read-only  */
#define TKAFG3K_ATTR_SPECIFIC_DRIVER_VENDOR                     IVI_ATTR_SPECIFIC_DRIVER_VENDOR                 /* ViString, read-only  */
#define TKAFG3K_ATTR_SPECIFIC_DRIVER_DESCRIPTION                IVI_ATTR_SPECIFIC_DRIVER_DESCRIPTION            /* ViString, read-only  */
#define TKAFG3K_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION   IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION /* ViInt32, read-only */
#define TKAFG3K_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION   IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION /* ViInt32, read-only */

        /* Advanced Session Information */
#define TKAFG3K_ATTR_LOGICAL_NAME                               IVI_ATTR_LOGICAL_NAME                           /* ViString, read-only  */
#define TKAFG3K_ATTR_IO_RESOURCE_DESCRIPTOR                     IVI_ATTR_IO_RESOURCE_DESCRIPTOR                 /* ViString, read-only  */
#define TKAFG3K_ATTR_DRIVER_SETUP                               IVI_ATTR_DRIVER_SETUP                           /* ViString, read-only  */

    /*- Instrument-Specific Attributes -------------------------------------*/

    /*- Basic Instrument Operation -----------------------------------------*/
#define TKAFG3K_ATTR_CHANNEL_COUNT                              IVI_ATTR_CHANNEL_COUNT                          /* ViInt32,  read-only  */
#define TKAFG3K_ATTR_OUTPUT_MODE                                IVIFGEN_ATTR_OUTPUT_MODE                        /* ViInt32                        */
#define TKAFG3K_ATTR_OUTPUT_MODE_BY_CHANNEL                     (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 2L)            /* ViInt32,   multi-channel       */
#define TKAFG3K_ATTR_REF_CLOCK_SOURCE                           IVIFGEN_ATTR_REF_CLOCK_SOURCE                   /* ViInt32                        */
#define TKAFG3K_ATTR_OUTPUT_ENABLED                             IVIFGEN_ATTR_OUTPUT_ENABLED                     /* ViBoolean, multi-channel       */
#define TKAFG3K_ATTR_OUTPUT_IMPEDANCE                           IVIFGEN_ATTR_OUTPUT_IMPEDANCE                   /* ViReal64,  multi-channel, ohms */
#define TKAFG3K_ATTR_OPERATION_MODE                             IVIFGEN_ATTR_OPERATION_MODE                     /* ViInt32,   multi-channel       */

    /*- Standard Waveform Generation ---------------------------------------*/
#define TKAFG3K_ATTR_FUNC_WAVEFORM                              IVIFGEN_ATTR_FUNC_WAVEFORM                      /* ViInt32,  multi-channel           */
#define TKAFG3K_ATTR_FUNC_AMPLITUDE                             IVIFGEN_ATTR_FUNC_AMPLITUDE                     /* ViReal64, multi-channel, volts    */
#define TKAFG3K_ATTR_FUNC_DC_OFFSET                             IVIFGEN_ATTR_FUNC_DC_OFFSET                     /* ViReal64, multi-channel, volts    */
#define TKAFG3K_ATTR_FUNC_FREQUENCY                             IVIFGEN_ATTR_FUNC_FREQUENCY                     /* ViReal64, multi-channel, hertz    */
#define TKAFG3K_ATTR_FUNC_START_PHASE                           IVIFGEN_ATTR_FUNC_START_PHASE                   /* ViReal64, multi-channel, degrees  */
#define TKAFG3K_ATTR_FUNC_DUTY_CYCLE_HIGH                       IVIFGEN_ATTR_FUNC_DUTY_CYCLE_HIGH               /* ViReal64, multi-channel, percent  */
#define TKAFG3K_ATTR_FUNC_RAMP_SYMMETRY                         (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 3L)            /* ViReal64, multi-channel, percent  */

#define TKAFG3K_ATTR_PULSE_DUTY_CYCLE                           (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 42L)
#define TKAFG3K_ATTR_PULSE_LEAD_DELAY                           (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 4L)            /* ViReal64, multi-channel, s   */
#define TKAFG3K_ATTR_PULSE_PERIOD                               (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 5L)            /* ViReal64, multi-channel, s   */
#define TKAFG3K_ATTR_PULSE_LEADING_TIME                         (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 6L)            /* ViReal64, multi-channel, s   */
#define TKAFG3K_ATTR_PULSE_TRAILING_TIME                        (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 7L)            /* ViReal64, multi-channel, s   */

    /*- Arbitrary Waveform Generation --------------------------------------*/
#define TKAFG3K_ATTR_ARB_WAVEFORM_HANDLE                        IVIFGEN_ATTR_ARB_WAVEFORM_HANDLE                /* ViInt32,  multi-channel         */
#define TKAFG3K_ATTR_ARB_GAIN                                   IVIFGEN_ATTR_ARB_GAIN                           /* ViReal64, multi-channel         */
#define TKAFG3K_ATTR_ARB_OFFSET                                 IVIFGEN_ATTR_ARB_OFFSET                         /* ViReal64, multi-channel, volts  */
#define TKAFG3K_ATTR_ARB_SAMPLE_RATE                            IVIFGEN_ATTR_ARB_SAMPLE_RATE                    /* ViReal64, samples-per-second    */

#define TKAFG3K_ATTR_MAX_NUM_WAVEFORMS                          IVIFGEN_ATTR_MAX_NUM_WAVEFORMS                  /* ViInt32,  read-only             */
#define TKAFG3K_ATTR_WAVEFORM_QUANTUM                           IVIFGEN_ATTR_WAVEFORM_QUANTUM                   /* ViInt32,  read-only             */
#define TKAFG3K_ATTR_MIN_WAVEFORM_SIZE                          IVIFGEN_ATTR_MIN_WAVEFORM_SIZE                  /* ViInt32,  read-only             */
#define TKAFG3K_ATTR_MAX_WAVEFORM_SIZE                          IVIFGEN_ATTR_MAX_WAVEFORM_SIZE                  /* ViInt32,  read-only             */

#define TKAFG3K_ATTR_ARB_FREQUENCY                              IVIFGEN_ATTR_ARB_FREQUENCY                      /* ViReal64, multi-channel         */

    /*- Trigger Configuration ----------------------------------------------*/
#define TKAFG3K_ATTR_TRIGGER_SOURCE                             IVIFGEN_ATTR_TRIGGER_SOURCE                     /* ViInt32, multi-channel */
#define TKAFG3K_ATTR_TRIGGER_MODE                               (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 8L)            /* ViInt32, multi-channel */
#define TKAFG3K_ATTR_TRIGGER_SLOPE                              (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 9L)            /* ViInt32, multi-channel */
#define TKAFG3K_ATTR_INTERNAL_TRIGGER_RATE                      IVIFGEN_ATTR_INTERNAL_TRIGGER_RATE              /* ViReal64               */

    /*- Burst Configuration ------------------------------------------------*/
#define TKAFG3K_ATTR_BURST_COUNT                                IVIFGEN_ATTR_BURST_COUNT                        /* ViInt32, multi-channel */
#define TKAFG3K_ATTR_BURST_MODE                                 (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 41L)

    /*- Sweep Configuration ------------------------------------------------*/
#define TKAFG3K_ATTR_SWEEP_TYPE                                 (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 11L)           /* ViInt32,   multi-channel         */
#define TKAFG3K_ATTR_SWEEP_MODE                                 (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 43L)           /* ViInt32,   multi-channel         */
#define TKAFG3K_ATTR_SWEEP_TIME                                 (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 12L)           /* ViReal64,  multi-channel, s      */
#define TKAFG3K_ATTR_SWEEP_START_FREQUENCY                      (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 13L)           /* ViReal64,  multi-channel, hertz  */
#define TKAFG3K_ATTR_SWEEP_STOP_FREQUENCY                       (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 14L)           /* ViReal64,  multi-channel, hertz  */
#define TKAFG3K_ATTR_SWEEP_HOLD_TIME                            (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 15L)           /* ViReal64,  multi-channel, s      */
#define TKAFG3K_ATTR_SWEEP_RETURN_TIME                          (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 16L)           /* ViReal64,  multi-channel, s      */

    /*- Waveform Appendant -------------------------------------------------*/
#define TKAFG3K_ATTR_APPEND_NOISE_ENABLED                       (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 17L)           /* ViBoolean, multi-channel          */
#define TKAFG3K_ATTR_APPEND_NOISE_LEVEL                         (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 18L)           /* ViReal64, multi-channel, percent  */
#define TKAFG3K_ATTR_APPEND_EXTERNAL_ENABLED                    (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 19L)           /* ViBoolean, multi-channel          */

    /*- Ampitude Modulation Generation -------------------------------------*/
#define TKAFG3K_ATTR_AM_ENABLED                                 IVIFGEN_ATTR_AM_ENABLED                         /* ViBoolean, multi-channel             */
#define TKAFG3K_ATTR_AM_SOURCE                                  IVIFGEN_ATTR_AM_SOURCE                          /* ViInt32,   multi-channel             */
#define TKAFG3K_ATTR_AM_INTERNAL_DEPTH                          IVIFGEN_ATTR_AM_INTERNAL_DEPTH                  /* ViReal64,  percent                   */
#define TKAFG3K_ATTR_AM_INTERNAL_WAVEFORM                       IVIFGEN_ATTR_AM_INTERNAL_WAVEFORM               /* ViInt32                              */
#define TKAFG3K_ATTR_AM_INTERNAL_FREQUENCY                      IVIFGEN_ATTR_AM_INTERNAL_FREQUENCY              /* ViReal64,  hertz                     */
#define TKAFG3K_ATTR_AM_DEPTH_BY_CHANNEL                        (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 20L)           /* ViReal64,  multi-channel, percent    */
#define TKAFG3K_ATTR_AM_INTERNAL_WAVEFORM_BY_CHANNEL            (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 21L)           /* ViInt32,   multi-channel             */
#define TKAFG3K_ATTR_AM_INTERNAL_FREQUENCY_BY_CHANNEL           (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 22L)           /* ViReal64,  multi-channel, hertz      */

    /*- Frequency Modulation Generation ------------------------------------*/
#define TKAFG3K_ATTR_FM_ENABLED                                 IVIFGEN_ATTR_FM_ENABLED                         /* ViBoolean, multi-channel             */
#define TKAFG3K_ATTR_FM_SOURCE                                  IVIFGEN_ATTR_FM_SOURCE                          /* ViInt32,   multi-channel             */
#define TKAFG3K_ATTR_FM_INTERNAL_DEVIATION                      IVIFGEN_ATTR_FM_INTERNAL_DEVIATION              /* ViReal64,  hertz                     */
#define TKAFG3K_ATTR_FM_INTERNAL_WAVEFORM                       IVIFGEN_ATTR_FM_INTERNAL_WAVEFORM               /* ViInt32                              */
#define TKAFG3K_ATTR_FM_INTERNAL_FREQUENCY                      IVIFGEN_ATTR_FM_INTERNAL_FREQUENCY              /* ViReal64,  hertz                     */
#define TKAFG3K_ATTR_FM_DEVIATION_BY_CHANNEL                    (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 23L)           /* ViReal64,  multi-channel, hertz      */
#define TKAFG3K_ATTR_FM_INTERNAL_WAVEFORM_BY_CHANNEL            (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 24L)           /* ViInt32,   multi-channel             */
#define TKAFG3K_ATTR_FM_INTERNAL_FREQUENCY_BY_CHANNEL           (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 25L)           /* ViReal64,  multi-channel, hertz      */

    /*- FSK Modulation Generation ------------------------------------------*/
#define TKAFG3K_ATTR_FSK_ENABLED                                (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 26L)           /* ViBoolean, multi-channel             */
#define TKAFG3K_ATTR_FSK_HOP_FREQUENCY                          (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 27L)           /* ViReal64,  multi-channel, hertz      */
#define TKAFG3K_ATTR_FSK_INTERNAL_RATE                          (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 28L)           /* ViReal64, multi-channel, hertz       */
#define TKAFG3K_ATTR_FSK_SOURCE                                 (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 29L)           /* ViInt32,  multi-channel              */

    /*- Phase Modulation Generation ----------------------------------------*/
#define TKAFG3K_ATTR_PM_ENABLED                                 (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 30L)           /* ViBoolean, multi-channel             */
#define TKAFG3K_ATTR_PM_SOURCE                                  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 31L)           /* ViInt32,   multi-channel             */
#define TKAFG3K_ATTR_PM_DEVIATION_BY_CHANNEL                    (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 32L)           /* ViReal64, multi-channel, deg         */
#define TKAFG3K_ATTR_PM_INTERNAL_FREQUENCY_BY_CHANNEL           (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 33L)           /* ViReal64, multi-channel, hertz       */
#define TKAFG3K_ATTR_PM_INTERNAL_WAVEFORM_BY_CHANNEL            (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 34L)           /* ViInt32,  multi-channel              */

    /*- Pulse Width Modulation Generation ----------------------------------*/
#define TKAFG3K_ATTR_PWM_ENABLED                                (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 35L)           /* ViBoolean, multi-channel             */
#define TKAFG3K_ATTR_PWM_SOURCE                                 (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 36L)           /* ViInt32,   multi-channel             */
#define TKAFG3K_ATTR_PWM_DEVIATION                              (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 37L)           /* ViReal64, multi-channel, percent     */
#define TKAFG3K_ATTR_PWM_INTERNAL_FREQUENCY                     (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 38L)           /* ViReal64, multi-channel, hertz       */
#define TKAFG3K_ATTR_PWM_INTERNAL_WAVEFORM                      (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 39L)           /* ViInt32,  multi-channel              */

    /*- Miscellaneous Attributes -------------------------------------------*/
#define TKAFG3K_ATTR_ID_QUERY_RESPONSE                          (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 1L)            /* ViString(Read Only) */
#define TKAFG3K_ATTR_BURST_DELAY                                (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 44L)


/****************************************************************************
 *------------------------ Attribute Value Defines -------------------------*
 ****************************************************************************/
     /*- Defined Value for Instrument models -------------------------------*/
#define TKAFG3K_VAL_MODEL_AFG3021                               0
#define TKAFG3K_VAL_MODEL_AFG3022                               1
#define TKAFG3K_VAL_MODEL_AFG3101                               2
#define TKAFG3K_VAL_MODEL_AFG3102                               3
#define TKAFG3K_VAL_MODEL_AFG3251                               4
#define TKAFG3K_VAL_MODEL_AFG3252                               5
#define TKAFG3K_VAL_MODEL_AFG3021B                              6
#define TKAFG3K_VAL_MODEL_AFG3022B                              7
#define TKAFG3K_VAL_MODEL_AFG3011                               8
#define TKAFG3K_VAL_MODEL_AFG2021                               9
#define TKAFG3K_VAL_MODEL_AFG3011C                              10
#define TKAFG3K_VAL_MODEL_AFG3021C                              11
#define TKAFG3K_VAL_MODEL_AFG3022C                              12
#define TKAFG3K_VAL_MODEL_AFG3101C                              13
#define TKAFG3K_VAL_MODEL_AFG3102C                              14
#define TKAFG3K_VAL_MODEL_AFG3251C                              15
#define TKAFG3K_VAL_MODEL_AFG3252C                              16
#define TKAFG3K_VAL_MODEL_AFG3051C                              17
#define TKAFG3K_VAL_MODEL_AFG3052C                              18

    /*- Defined values for attribute TKAFG3K_ATTR_OPERATION_MODE --------------*/
#define TKAFG3K_VAL_OPERATE_CONTINUOUS                          IVIFGEN_VAL_OPERATE_CONTINUOUS
#define TKAFG3K_VAL_OPERATE_MODULATION                          (IVIFGEN_VAL_OP_MODE_SPECIFIC_EXT_BASE + 1)
#define TKAFG3K_VAL_OPERATE_SWEEP                               (IVIFGEN_VAL_OP_MODE_SPECIFIC_EXT_BASE + 2)
#define TKAFG3K_VAL_OPERATE_BURST                               IVIFGEN_VAL_OPERATE_BURST

    /*- Defined values for attribute TKAFG3K_ATTR_OUTPUT_MODE --------------*/
#define TKAFG3K_VAL_OUTPUT_FUNC                                 IVIFGEN_VAL_OUTPUT_FUNC
#define TKAFG3K_VAL_OUTPUT_ARB                                  IVIFGEN_VAL_OUTPUT_ARB

    /*- Defined values for attribute TKAFG3K_ATTR_OUTPUT_IMPEDANCE ------*/
#define TKAFG3K_VAL_IMPEDANCE_INFINITY                          0

    /*- Defined values for attribute TKAFG3K_ATTR_REF_CLOCK_SOURCE ------*/
#define TKAFG3K_VAL_REF_CLOCK_INTERNAL                          IVIFGEN_VAL_REF_CLOCK_INTERNAL
#define TKAFG3K_VAL_REF_CLOCK_EXTERNAL                          IVIFGEN_VAL_REF_CLOCK_EXTERNAL

    /*- Defined values for attribute TKAFG3K_ATTR_FUNC_WAVEFORM ---------*/
#define TKAFG3K_VAL_WFM_SINE                                    IVIFGEN_VAL_WFM_SINE
#define TKAFG3K_VAL_WFM_SQUARE                                  IVIFGEN_VAL_WFM_SQUARE
#define TKAFG3K_VAL_WFM_DC                                      IVIFGEN_VAL_WFM_DC
    /*- Extended standard waveform -*/
#define TKAFG3K_VAL_WFM_RAMP                                    (IVIFGEN_VAL_WFM_SPECIFIC_EXT_BASE + 1)
#define TKAFG3K_VAL_WFM_PULS                                    (IVIFGEN_VAL_WFM_SPECIFIC_EXT_BASE + 2)
#define TKAFG3K_VAL_WFM_PRN                                     (IVIFGEN_VAL_WFM_SPECIFIC_EXT_BASE + 3)
#define TKAFG3K_VAL_WFM_SINC                                    (IVIFGEN_VAL_WFM_SPECIFIC_EXT_BASE + 4)
#define TKAFG3K_VAL_WFM_GAUS                                    (IVIFGEN_VAL_WFM_SPECIFIC_EXT_BASE + 5)
#define TKAFG3K_VAL_WFM_LOR                                     (IVIFGEN_VAL_WFM_SPECIFIC_EXT_BASE + 6)
#define TKAFG3K_VAL_WFM_ERIS                                    (IVIFGEN_VAL_WFM_SPECIFIC_EXT_BASE + 7)
#define TKAFG3K_VAL_WFM_EDEC                                    (IVIFGEN_VAL_WFM_SPECIFIC_EXT_BASE + 8)
#define TKAFG3K_VAL_WFM_HAV                                     (IVIFGEN_VAL_WFM_SPECIFIC_EXT_BASE + 9)

#define TKAFG3K_VAL_FIRST_WAVEFORM_HANDLE                       (10000L)
    /*- Extended arbitrary waveform -*/
#define TKAFG3K_VAL_WFM_USER1                                   (TKAFG3K_VAL_FIRST_WAVEFORM_HANDLE)
#define TKAFG3K_VAL_WFM_USER2                                   (TKAFG3K_VAL_FIRST_WAVEFORM_HANDLE + 1)
#define TKAFG3K_VAL_WFM_USER3                                   (TKAFG3K_VAL_FIRST_WAVEFORM_HANDLE + 2)
#define TKAFG3K_VAL_WFM_USER4                                   (TKAFG3K_VAL_FIRST_WAVEFORM_HANDLE + 3)
#define TKAFG3K_VAL_WFM_EDIT                                    (TKAFG3K_VAL_FIRST_WAVEFORM_HANDLE + 11)

    /*- Memory slot for the arbitrary waveform -*/
#define TKAFG3K_VAL_WFM_SLOT1                                   (0L)
#define TKAFG3K_VAL_WFM_SLOT2                                   (1L)
#define TKAFG3K_VAL_WFM_SLOT3                                   (2L)
#define TKAFG3K_VAL_WFM_SLOT4                                   (3L)

    /*- Defined values for Arbitrary Waveform Handles ---------------------------------*/
#define TKAFG3K_VAL_ALL_WAVEFORMS                               IVIFGEN_VAL_ALL_WAVEFORMS

    /*- Defined values for standard shape arbitrary waveform --------------------------*/
#define TKAFG3K_VAL_ARB_WFM_SINE                                (1L)
#define TKAFG3K_VAL_ARB_WFM_SQUARE                              (2L)
#define TKAFG3K_VAL_ARB_WFM_PULS                                (3L)
#define TKAFG3K_VAL_ARB_WFM_RAMP                                (4L)
#define TKAFG3K_VAL_ARB_WFM_PRN                                 (5L)

    /*- Defined values for attribute TKAFG3K_ATTR_TRIGGER_SOURCE ----------------------*/
#define TKAFG3K_VAL_EXTERNAL                                    IVIFGEN_VAL_EXTERNAL
#define TKAFG3K_VAL_INTERNAL_TRIGGER                            IVIFGEN_VAL_INTERNAL_TRIGGER

    /*- Defined values for attribute TKAFG3K_ATTR_TRIGGER_MODE ------------------------*/
#define TKAFG3K_VAL_TRIGGER_TRIGGER                             1
#define TKAFG3K_VAL_TRIGGER_SYNC                                2

    /*- Defined values for attribute TKAFG3K_ATTR_TRIGGER_SLOPE -----------------------*/
#define TKAFG3K_VAL_TRIGGER_POSITIVE                            1
#define TKAFG3K_VAL_TRIGGER_NEGATIVE                            2

    /*- Deifined value for attribute TKAFG3K_ATTR_BURST_COUNT -------------------------*/
#define TKAFG3K_VAL_BURST_INFINITY                              0

    /*- Defined value for attribute TKAFG3K_ATTR_BURST_MODE ---------------------------*/
#define TKAFG3K_VAL_BURST_TRIGGER                               1
#define TKAFG3K_VAL_BURST_GATE                                  2

    /*- Defined value for attribute TKAFG3K_ATTR_SWEEP_TYPE ---------------------------*/
#define TKAFG3K_VAL_SWEEP_LINEAR                                1
#define TKAFG3K_VAL_SWEEP_LOGARITHMIC                           2

    /*- Defined value for attribute TKAFG3K_ATTR_SWEEP_MODE ---------------------------*/
#define TKAFG3K_VAL_SWEEP_AUTO                                  1
#define TKAFG3K_VAL_SWEEP_MANUAL                                2

    /*- Defined values for attribute TKAFG3K_ATTR_AM_SOURCE -*/
#define TKAFG3K_VAL_AM_INTERNAL                                 IVIFGEN_VAL_AM_INTERNAL
#define TKAFG3K_VAL_AM_EXTERNAL                                 IVIFGEN_VAL_AM_EXTERNAL

   /*- Defined values for attribute TKAFG3K_ATTR_AM_INTERNAL_WAVEFORM -*/
#define TKAFG3K_VAL_AM_INTERNAL_SINE                            IVIFGEN_VAL_AM_INTERNAL_SINE
#define TKAFG3K_VAL_AM_INTERNAL_SQUARE                          IVIFGEN_VAL_AM_INTERNAL_SQUARE
#define TKAFG3K_VAL_AM_INTERNAL_TRIANGLE                        IVIFGEN_VAL_AM_INTERNAL_TRIANGLE
#define TKAFG3K_VAL_AM_INTERNAL_RAMP_UP                         IVIFGEN_VAL_AM_INTERNAL_RAMP_UP
#define TKAFG3K_VAL_AM_INTERNAL_RAMP_DOWN                       IVIFGEN_VAL_AM_INTERNAL_RAMP_DOWN
#define TKAFG3K_VAL_AM_INTERNAL_PRN                             (IVIFGEN_VAL_AM_INTERNAL_WFM_SPECIFIC_EXT_BASE + 1)
#define TKAFG3K_VAL_AM_INTERNAL_USER1                           (IVIFGEN_VAL_AM_INTERNAL_WFM_SPECIFIC_EXT_BASE + 2)
#define TKAFG3K_VAL_AM_INTERNAL_USER2                           (IVIFGEN_VAL_AM_INTERNAL_WFM_SPECIFIC_EXT_BASE + 3)
#define TKAFG3K_VAL_AM_INTERNAL_USER3                           (IVIFGEN_VAL_AM_INTERNAL_WFM_SPECIFIC_EXT_BASE + 4)
#define TKAFG3K_VAL_AM_INTERNAL_USER4                           (IVIFGEN_VAL_AM_INTERNAL_WFM_SPECIFIC_EXT_BASE + 5)

    /*- Defined values for attribute TKAFG3K_ATTR_FSK_SOURCE -*/
#define TKAFG3K_VAL_FSK_INTERNAL                                (0L)
#define TKAFG3K_VAL_FSK_EXTERNAL                                (1L)
     
    /*- Defined values for attribute TKAFG3K_ATTR_FM_SOURCE -*/
#define TKAFG3K_VAL_FM_INTERNAL                                 IVIFGEN_VAL_FM_INTERNAL
#define TKAFG3K_VAL_FM_EXTERNAL                                 IVIFGEN_VAL_FM_EXTERNAL

   /*- Defined values for attribute TKAFG3K_ATTR_FM_INTERNAL_WAVEFORM -*/
#define TKAFG3K_VAL_FM_INTERNAL_SINE                            IVIFGEN_VAL_FM_INTERNAL_SINE
#define TKAFG3K_VAL_FM_INTERNAL_SQUARE                          IVIFGEN_VAL_FM_INTERNAL_SQUARE
#define TKAFG3K_VAL_FM_INTERNAL_TRIANGLE                        IVIFGEN_VAL_FM_INTERNAL_TRIANGLE
#define TKAFG3K_VAL_FM_INTERNAL_RAMP_UP                         IVIFGEN_VAL_FM_INTERNAL_RAMP_UP
#define TKAFG3K_VAL_FM_INTERNAL_RAMP_DOWN                       IVIFGEN_VAL_FM_INTERNAL_RAMP_DOWN
#define TKAFG3K_VAL_FM_INTERNAL_PRN                             (IVIFGEN_VAL_FM_INTERNAL_WFM_SPECIFIC_EXT_BASE + 1)
#define TKAFG3K_VAL_FM_INTERNAL_USER1                           (IVIFGEN_VAL_FM_INTERNAL_WFM_SPECIFIC_EXT_BASE + 2)
#define TKAFG3K_VAL_FM_INTERNAL_USER2                           (IVIFGEN_VAL_FM_INTERNAL_WFM_SPECIFIC_EXT_BASE + 3)
#define TKAFG3K_VAL_FM_INTERNAL_USER3                           (IVIFGEN_VAL_FM_INTERNAL_WFM_SPECIFIC_EXT_BASE + 4)
#define TKAFG3K_VAL_FM_INTERNAL_USER4                           (IVIFGEN_VAL_FM_INTERNAL_WFM_SPECIFIC_EXT_BASE + 5)

    /*- Defined values for attribute TKAFG3K_ATTR_PM_SOURCE -*/
#define TKAFG3K_VAL_PM_INTERNAL                                 (0L)
#define TKAFG3K_VAL_PM_EXTERNAL                                 (1L)

    /*- Defined values for attribute TKAFG3K_ATTR_PM_INTERNAL_WAVEFORM_BY_CHANNEL -*/
#define TKAFG3K_VAL_PM_INTERNAL_SINE                            (1L)
#define TKAFG3K_VAL_PM_INTERNAL_SQUARE                          (2L)
#define TKAFG3K_VAL_PM_INTERNAL_TRIANGLE                        (3L)
#define TKAFG3K_VAL_PM_INTERNAL_RAMP_UP                         (4L)
#define TKAFG3K_VAL_PM_INTERNAL_RAMP_DOWN                       (5L)
#define TKAFG3K_VAL_PM_INTERNAL_PRN                             (100L)
#define TKAFG3K_VAL_PM_INTERNAL_USER1                           (200L)
#define TKAFG3K_VAL_PM_INTERNAL_USER2                           (201L)
#define TKAFG3K_VAL_PM_INTERNAL_USER3                           (202L)
#define TKAFG3K_VAL_PM_INTERNAL_USER4                           (203L)

    /*- Defined values for attribute TKAFG3K_ATTR_PWM_SOURCE -*/
#define TKAFG3K_VAL_PWM_INTERNAL                                (0L)
#define TKAFG3K_VAL_PWM_EXTERNAL                                (1L)

    /*- Defined values for attribute TKAFG3K_ATTR_PWM_INTERNAL_WAVEFORM -*/
#define TKAFG3K_VAL_PWM_INTERNAL_SINE                           (1L)
#define TKAFG3K_VAL_PWM_INTERNAL_SQUARE                         (2L)
#define TKAFG3K_VAL_PWM_INTERNAL_TRIANGLE                       (3L)
#define TKAFG3K_VAL_PWM_INTERNAL_RAMP_UP                        (4L)
#define TKAFG3K_VAL_PWM_INTERNAL_RAMP_DOWN                      (5L)
#define TKAFG3K_VAL_PWM_INTERNAL_PRN                            (100L)
#define TKAFG3K_VAL_PWM_INTERNAL_USER1                          (200L)
#define TKAFG3K_VAL_PWM_INTERNAL_USER2                          (201L)
#define TKAFG3K_VAL_PWM_INTERNAL_USER3                          (202L)
#define TKAFG3K_VAL_PWM_INTERNAL_USER4                          (203L)

#define TKAFG3K_VAL_MODULATION_AM                               0
#define TKAFG3K_VAL_MODULATION_FM                               1
#define TKAFG3K_VAL_MODULATION_PM                               2
#define TKAFG3K_VAL_MODULATION_FSK                              3
#define TKAFG3K_VAL_MODULATION_PWM                              4
	
	/*- Defined values for function tkafg3k_SaveLoadInstrumentSettings -*/
#define TKAFG3K_VAL_OPERATION_SAVE								0
#define TKAFG3K_VAL_OPERATION_LOAD								1

/****************************************************************************
 *------------------------ Error And Completion Codes ----------------------*
 ****************************************************************************/
#define TKAFG3K_ERROR_NO_WFMS_AVAILABLE                 IVIFGEN_ERROR_NO_WFMS_AVAILABLE

#define TKAFG3K_ERROR_INVALID_SPECIFIC_MODEL            (IVI_SPECIFIC_ERROR_BASE + 0x11)    // 0xBFFA4011

#define TKAFG3K_ERROR_ABORT_GENERATION_UNSUPPORTED      (IVI_SPECIFIC_ERROR_BASE + 0x21)    // 0xBFFA4021
#define TKAFG3K_ERROR_DISABLE_UNSUPPORTED               (IVI_SPECIFIC_ERROR_BASE + 0x22)    // 0xBFFA4022

#define TKAFG3K_ERROR_CHANNELS_DIFFERENT                (IVI_SPECIFIC_ERROR_BASE + 0x31)    // 0xBFFA4031
#define TKAFG3K_ERROR_COMBINATION_UNSUPPORTED           (IVI_SPECIFIC_ERROR_BASE + 0x32)    // 0xBFFA4032
#define TKAFG3K_ERROR_INVALID_GROUP_VALUE               (IVI_SPECIFIC_ERROR_BASE + 0x33)    // 0xBFFA4033
#define TKAFG3K_ERROR_INIT_FAIL                         (IVI_SPECIFIC_ERROR_BASE + 0x34)    // 0xBFFA4034

/****************************************************************************
 *---------------- Instrument Driver Function Declarations -----------------*
 ****************************************************************************/

    /*- Init and Close Functions -------------------------------------------*/
    ViStatus _VI_FUNC  tkafg3k_init(ViRsrc resourceName, ViBoolean IDQuery,
                                    ViBoolean resetDevice, ViSession *vi);
    ViStatus _VI_FUNC  tkafg3k_InitWithOptions(ViRsrc resourceName,
                                               ViBoolean IDQuery,
                                               ViBoolean resetDevice,
                                               ViConstString optionString,
                                               ViSession *newVi);
    ViStatus _VI_FUNC  tkafg3k_close(ViSession vi);

    /*- Coercion Info Functions --------------------------------------------*/
    ViStatus _VI_FUNC  tkafg3k_GetNextCoercionRecord(ViSession vi,
                                                     ViInt32 bufferSize,
                                                     ViChar record[]);

    /*- Locking Functions --------------------------------------------------*/
    ViStatus _VI_FUNC  tkafg3k_LockSession(ViSession vi,
                                           ViBoolean *callerHasLock);
    ViStatus _VI_FUNC  tkafg3k_UnlockSession(ViSession vi,
                                             ViBoolean *callerHasLock);

    /*- Channel Info Functions ---------------------------------------------*/
    ViStatus _VI_FUNC tkafg3k_GetChannelName(ViSession vi, ViInt32 index,
                                             ViInt32 bufferSize, ViChar name[]);

    /*- Basic Instrument Operation -----------------------------------------*/

    ViStatus _VI_FUNC tkafg3k_ConfigureOperationMode(ViSession vi,
                                                     ViConstString channelName,
                                                     ViInt32 outputMode);

    ViStatus _VI_FUNC tkafg3k_ConfigureOutputMode(ViSession vi,
                                                  ViInt32 outputMode);

    ViStatus _VI_FUNC tkafg3k_ConfigureOutputModeByChannel(
        ViSession vi,
        ViConstString channelName,
        ViInt32 outputMode );

    ViStatus _VI_FUNC tkafg3k_ConfigureRefClockSource(ViSession vi,
                                                      ViInt32 refClockSource);

    ViStatus _VI_FUNC tkafg3k_ConfigureOutputImpedance(ViSession vi,
                                                       ViConstString channelName,
                                                       ViReal64 outputImpedance);

    ViStatus _VI_FUNC tkafg3k_ConfigureOutputEnabled(ViSession vi,
                                                     ViConstString channelName,
                                                     ViBoolean enabled);

    ViStatus _VI_FUNC tkafg3k_InitiateGeneration(ViSession vi);

    ViStatus _VI_FUNC tkafg3k_AbortGeneration(ViSession vi);

    /*- Standard Function Output --------------------------------------------*/
    ViStatus _VI_FUNC tkafg3k_ConfigureStandardWaveform(
        ViSession vi,
        ViConstString channelName,
        ViInt32 waveform,
        ViReal64 amplitude,
        ViReal64 dcOffset,
        ViReal64 frequency,
        ViReal64 startPhase);

    ViStatus _VI_FUNC tkafg3k_ConfigureRampSymmetry(ViSession vi,
                                                    ViConstString channelName,
                                                    ViReal64 rampSymmetry);

    ViStatus _VI_FUNC tkafg3k_ConfigurePulseWaveform(ViSession vi,
                                                     ViConstString channelName,
                                                     ViReal64 dutyCycle,
                                                     ViReal64 leadingEdgeTime,
                                                     ViReal64 trailingEdgeTime);

    ViStatus _VI_FUNC tkafg3k_ConfigurePulseDelay(ViSession vi,
                                                  ViConstString channelName,
                                                  ViReal64 delay);

    /*- Arbitrary Waveform Output -------------------------------------------*/
    ViStatus _VI_FUNC  tkafg3k_ConfigureSampleRate(ViSession vi,
                                                   ViReal64 sampleRate);

    ViStatus _VI_FUNC  tkafg3k_QueryArbWfmCapabilities(ViSession vi,
                                                       ViInt32 *maxNumWfms,
                                                       ViInt32 *wfmQuantum,
                                                       ViInt32 *minWfmSize,
                                                       ViInt32 *maxWfmSize);

    ViStatus _VI_FUNC  tkafg3k_CreateArbWaveform(ViSession vi,
                                                 ViInt32 wfmSize,
                                                 ViReal64 wfmData[],
                                                 ViInt32 *wfmHandle);

    ViStatus _VI_FUNC  tkafg3k_CreateLineInterpolateArbWfm(
        ViSession vi,
        ViInt32 wfmSize,
        ViReal64 startPointData,
        ViReal64 endPointData,
        ViInt32* wfmHandle);

    ViStatus _VI_FUNC  tkafg3k_CreateStandardShapeArbWfm(ViSession vi,
                                                         ViInt32 wfmSize,
                                                         ViInt32 wfmType,
                                                         ViInt32 *wfmHandle);

    ViStatus _VI_FUNC  tkafg3k_GetArbitraryWaveformBySlot(ViSession vi,
                                                          ViInt32 slot,
                                                          ViInt32 *wfmHandle);

    ViStatus _VI_FUNC  tkafg3k_CreateArbWaveformBySlot(ViSession vi,
                                                       ViInt32 slot,
                                                       ViInt32 wfmSize,
                                                       ViReal64 wfmData[],
                                                       ViBoolean overwrite,
                                                       ViInt32 *wfmHandle);

    ViStatus _VI_FUNC  tkafg3k_CreateLineInterpolateArbWfmBySlot(
        ViSession vi,
        ViInt32 slot,
        ViInt32 wfmSize,
        ViReal64 startPointData,
        ViReal64 endPointData,
        ViBoolean overwrite,
        ViInt32* wfmHandle);

    ViStatus _VI_FUNC  tkafg3k_CreateStandardShapeArbWfmBySlot(
        ViSession vi,
        ViInt32 slot,
        ViInt32 wfmSize,
        ViInt32 wfmType,
        ViBoolean overwrite,
        ViInt32 *wfmHandle);

    ViStatus _VI_FUNC  tkafg3k_ClearArbWaveform(ViSession vi, ViInt32 wfmHandle);

    ViStatus _VI_FUNC  tkafg3k_ClearArbWaveformBySlot(ViSession vi,
                                                      ViInt32 slot);

    ViStatus _VI_FUNC  tkafg3k_ConfigureArbWaveform(ViSession vi,
                                                    ViConstString channelName,
                                                    ViInt32 wfmHandle,
                                                    ViReal64 arbGain,
                                                    ViReal64 arbOffset);

    /*- Arbitrary Frequency -------------------------------------------------*/
    ViStatus _VI_FUNC  tkafg3k_ConfigureArbFrequency(ViSession vi,
                                                     ViConstString channelName,
                                                     ViReal64 arbFrequency);

    /*- Triggering ----------------------------------------------------------*/
    ViStatus _VI_FUNC  tkafg3k_ConfigureTriggerSource(ViSession vi,
                                                      ViConstString channelName,
                                                      ViInt32 trigSource);

    ViStatus _VI_FUNC tkafg3k_ForceTrigger(ViSession vi);

    ViStatus _VI_FUNC tkafg3k_ConfigureTriggerOutputMode(ViSession vi,
                                                         ViInt32 outputMode);

    /*- Internal Triggering -------------------------------------------------*/
    ViStatus _VI_FUNC tkafg3k_ConfigureInternalTriggerRate(ViSession vi,
                                                           ViReal64 rate);

    /*- External Triggering -------------------------------------------------*/
    ViStatus _VI_FUNC tkafg3k_ConfigureExternalTriggerSlope(
        ViSession vi,
        ViInt32 triggerSlope);

    /*- Burst Configuration -------------------------------------------------*/
    ViStatus _VI_FUNC tkafg3k_ConfigureBurstEnabled(ViSession vi,
                                                    ViConstString channelName,
                                                    ViBoolean enabled);

    ViStatus _VI_FUNC tkafg3k_ConfigureBurstCount(ViSession vi,
                                                  ViConstString channelName,
                                                  ViInt32 burstCount);

    ViStatus _VI_FUNC tkafg3k_ConfigureBurstMode(ViSession vi,
                                                 ViConstString channelName,
                                                 ViInt32 burstMode);

    ViStatus _VI_FUNC tkafg3k_ConfigureBurstDelay(ViSession vi,
                                                  ViConstString channelName,
                                                  ViReal64 burstDelay);

    /*- Sweep Configuration -------------------------------------------------*/
    ViStatus _VI_FUNC tkafg3k_ConfigureSweepEnabled(ViSession vi,
                                                    ViConstString channelName,
                                                    ViBoolean enabled);

    ViStatus _VI_FUNC tkafg3k_ConfigureSweep(ViSession vi,
                                             ViConstString channelName,
                                             ViReal64 startFrequency,
                                             ViReal64 stopFrequency,
                                             ViReal64 time,
                                             ViInt32 type);

    ViStatus _VI_FUNC tkafg3k_ConfigureSweepEx(ViSession vi,
                                               ViConstString channelName,
                                               ViReal64 startFrequency,
                                               ViReal64 stopFrequency,
                                               ViReal64 time,
                                               ViReal64 holdTime,
                                               ViReal64 returnTime,
                                               ViInt32 type);

    ViStatus _VI_FUNC tkafg3k_ConfigureSweepMode(ViSession vi,
                                                 ViConstString channelName,
                                                 ViInt32 sweepMode);

    /*- Waveform Appendant --------------------------------------------------*/
    ViStatus _VI_FUNC tkafg3k_ConfigureNoiseAppendantEnabled(
        ViSession vi,
        ViConstString channelName,
        ViBoolean enabled);

    ViStatus _VI_FUNC tkafg3k_ConfigureNoiseLevel(ViSession vi,
                                                  ViConstString channelName,
                                                  ViReal64 level);

    ViStatus _VI_FUNC tkafg3k_ConfigureExternalAppendEnabled(
        ViSession vi,
        ViConstString channelName,
        ViBoolean enabled);

    /*- Amplitude Modulation ------------------------------------------------*/

    ViStatus _VI_FUNC  tkafg3k_ConfigureAMEnabled(ViSession vi,
                                                  ViConstString channelName,
                                                  ViBoolean enabled);

    ViStatus _VI_FUNC  tkafg3k_ConfigureAMSource(ViSession vi,
                                                 ViConstString channelName,
                                                 ViInt32 source);

    ViStatus _VI_FUNC  tkafg3k_ConfigureAMInternal(ViSession vi,
                                                   ViReal64 amDepth,
                                                   ViInt32 amWaveform,
                                                   ViReal64 amFrequency);

    ViStatus _VI_FUNC tkafg3k_ConfigureAMInternalByChannel(
        ViSession vi,
        ViConstString channelName,
        ViReal64 amDepth,
        ViInt32 amWaveform,
        ViReal64 amFrequency);

    ViStatus _VI_FUNC tkafg3k_ConfigureAMExternalByChannel(
        ViSession vi,
        ViConstString channelName,
        ViReal64 amDepth);

    /*- Frequency Modulation ------------------------------------------------*/

    ViStatus _VI_FUNC  tkafg3k_ConfigureFMEnabled(ViSession vi,
                                                  ViConstString channelName,
                                                  ViBoolean enabled);

    ViStatus _VI_FUNC  tkafg3k_ConfigureFMSource(ViSession vi,
                                                 ViConstString channelName,
                                                 ViInt32 source);

    ViStatus _VI_FUNC  tkafg3k_ConfigureFMInternal(ViSession vi,
                                                   ViReal64 fmDeviation,
                                                   ViInt32 fmWaveform,
                                                   ViReal64 fmFrequency);

    ViStatus _VI_FUNC  tkafg3k_ConfigureFMInternalByChannel(
        ViSession vi,
        ViConstString channelName,
        ViReal64 fmDeviance,
        ViInt32 fmWaveform,
        ViReal64 fmFrequency);

    ViStatus _VI_FUNC tkafg3k_ConfigureFMExternalByChannel(
        ViSession vi,
        ViConstString channelName,
        ViReal64 fmDeviance);

    /*- Phase Modulation ----------------------------------------------------*/
    ViStatus _VI_FUNC tkafg3k_ConfigurePMEnabled(ViSession vi,
                                                 ViConstString channelName,
                                                 ViBoolean enabled);

    ViStatus _VI_FUNC tkafg3k_ConfigurePMSource(ViSession vi,
                                                ViConstString channelName,
                                                ViInt32 source);

    ViStatus _VI_FUNC tkafg3k_ConfigurePMInternalByChannel(
        ViSession vi,
        ViConstString channelName,
        ViReal64 pmDeviation,
        ViInt32 pmWaveform,
        ViReal64 pmFrequency);

    ViStatus _VI_FUNC tkafg3k_ConfigurePMExternalByChannel(
        ViSession vi,
        ViConstString channelName,
        ViReal64 pmDeviation);

    /*- FSK Modulation -----------------------------------------------------*/
    ViStatus _VI_FUNC tkafg3k_ConfigureFSKEnabled(ViSession vi,
                                                  ViConstString channelName,
                                                  ViBoolean enabled);

    ViStatus _VI_FUNC tkafg3k_ConfigureFSKSource(ViSession vi,
                                                 ViConstString channelName,
                                                 ViInt32 source);

    ViStatus _VI_FUNC tkafg3k_ConfigureFSKInternalByChannel(
        ViSession vi,
        ViConstString channelName,
        ViReal64 hopFrequency,
        ViReal64 rate);

    ViStatus _VI_FUNC tkafg3k_ConfigureFSKExternalByChannel(
        ViSession vi,
        ViConstString channelName,
        ViReal64 hopFrequency);

    /*- PWM Modulation -----------------------------------------------------*/
    ViStatus _VI_FUNC tkafg3k_ConfigurePWMEnabled(ViSession vi,
                                                  ViConstString channelName,
                                                  ViBoolean enabled);

    ViStatus _VI_FUNC tkafg3k_ConfigurePWMSource(ViSession vi,
                                                 ViConstString channelName,
                                                 ViInt32 source);

    ViStatus _VI_FUNC tkafg3k_ConfigurePWMInternalByChannel(
        ViSession vi,
        ViConstString channelName,
        ViReal64 pwmDeviation,
        ViInt32 pwmWaveform,
        ViReal64 pwmFrequency);

    ViStatus _VI_FUNC tkafg3k_ConfigurePWMExternalByChannel(
        ViSession vi,
        ViConstString channelName,
        ViReal64 pwmDeviation);

    /*- Error Functions ----------------------------------------------------*/
    ViStatus _VI_FUNC  tkafg3k_error_query(ViSession vi, ViInt32 *errorCode,
                                           ViChar errorMessage[]);

    ViStatus _VI_FUNC  tkafg3k_GetError(ViSession vi, ViStatus *code,
                                        ViInt32 bufferSize,
                                        ViChar description[]);
    ViStatus _VI_FUNC  tkafg3k_ClearError(ViSession vi);

    ViStatus _VI_FUNC  tkafg3k_error_message(ViSession vi, ViStatus errorCode,
                                             ViChar errorMessage[256]);

    /*- Interchangeability Checking Functions ------------------------------*/
    ViStatus _VI_FUNC tkafg3k_GetNextInterchangeWarning(ViSession vi,
                                                        ViInt32 bufferSize,
                                                        ViChar warnString[]);
    ViStatus _VI_FUNC tkafg3k_ResetInterchangeCheck(ViSession vi);
    ViStatus _VI_FUNC tkafg3k_ClearInterchangeWarnings(ViSession vi);

    /*- Utility Functions --------------------------------------------------*/
    ViStatus _VI_FUNC   tkafg3k_InvalidateAllAttributes(ViSession vi);
    ViStatus _VI_FUNC   tkafg3k_reset(ViSession vi);
    ViStatus _VI_FUNC   tkafg3k_ResetWithDefaults(ViSession vi);
    ViStatus _VI_FUNC   tkafg3k_self_test(ViSession vi, ViInt16 *selfTestResult,
                                          ViChar selfTestMessage[]);
    ViStatus _VI_FUNC   tkafg3k_revision_query(ViSession vi,
                                               ViChar instrumentDriverRevision[],
                                               ViChar firmwareRevision[]);
    ViStatus _VI_FUNC   tkafg3k_Disable(ViSession vi);
    ViStatus _VI_FUNC   tkafg3k_WriteInstrData(ViSession vi,
                                               ViConstString writeBuffer);
    ViStatus _VI_FUNC   tkafg3k_ReadInstrData (ViSession vi, ViInt32 numBytes,
                                               ViChar rdBuf[],
                                               ViInt32 *bytesRead);
	ViStatus _VI_FUNC   tkafg3k_SaveLoadInstrumentSettings (ViSession vi, 
		                                                    ViInt32 operationSelect,
															ViConstString fileName);

    /*- Set, Get, and Check Attribute Functions ----------------------------*/
    ViStatus _VI_FUNC  tkafg3k_GetAttributeViInt32(ViSession vi,
                                                   ViConstString channelName,
                                                   ViAttr attribute,
                                                   ViInt32 *value);
    ViStatus _VI_FUNC  tkafg3k_GetAttributeViReal64(ViSession vi,
                                                    ViConstString channelName,
                                                    ViAttr attribute,
                                                    ViReal64 *value);
    ViStatus _VI_FUNC  tkafg3k_GetAttributeViString(ViSession vi,
                                                    ViConstString channelName,
                                                    ViAttr attribute,
                                                    ViInt32 bufSize,
                                                    ViChar value[]);
    ViStatus _VI_FUNC  tkafg3k_GetAttributeViSession(ViSession vi,
                                                     ViConstString channelName,
                                                     ViAttr attribute,
                                                     ViSession *value);
    ViStatus _VI_FUNC  tkafg3k_GetAttributeViBoolean(ViSession vi,
                                                     ViConstString channelName,
                                                     ViAttr attribute,
                                                     ViBoolean *value);

    ViStatus _VI_FUNC  tkafg3k_SetAttributeViInt32(ViSession vi,
                                                   ViConstString channelName,
                                                   ViAttr attribute,
                                                   ViInt32 value);
    ViStatus _VI_FUNC  tkafg3k_SetAttributeViReal64(ViSession vi,
                                                    ViConstString channelName,
                                                    ViAttr attribute,
                                                    ViReal64 value);
    ViStatus _VI_FUNC  tkafg3k_SetAttributeViString(ViSession vi,
                                                    ViConstString channelName,
                                                    ViAttr attribute,
                                                    ViConstString value);
    ViStatus _VI_FUNC  tkafg3k_SetAttributeViSession(ViSession vi,
                                                     ViConstString channelName,
                                                     ViAttr attribute,
                                                     ViSession value);
    ViStatus _VI_FUNC  tkafg3k_SetAttributeViBoolean(ViSession vi,
                                                     ViConstString channelName,
                                                     ViAttr attribute,
                                                     ViBoolean value);

    ViStatus _VI_FUNC  tkafg3k_CheckAttributeViInt32(ViSession vi,
                                                     ViConstString channelName,
                                                     ViAttr attribute,
                                                     ViInt32 value);
    ViStatus _VI_FUNC  tkafg3k_CheckAttributeViReal64(ViSession vi,
                                                      ViConstString channelName,
                                                      ViAttr attribute,
                                                      ViReal64 value);
    ViStatus _VI_FUNC  tkafg3k_CheckAttributeViString(ViSession vi,
                                                      ViConstString channelName,
                                                      ViAttr attribute,
                                                      ViConstString value);
    ViStatus _VI_FUNC  tkafg3k_CheckAttributeViSession(ViSession vi,
                                                       ViConstString channelName,
                                                       ViAttr attribute,
                                                       ViSession value);
    ViStatus _VI_FUNC  tkafg3k_CheckAttributeViBoolean(ViSession vi,
                                                       ViConstString channelName,
                                                       ViAttr attribute,
                                                       ViBoolean value);

/****************************************************************************
 *---------------------------- End Include File ----------------------------*
 ****************************************************************************/
#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif
#endif /* __tkafg3k_HEADER */
