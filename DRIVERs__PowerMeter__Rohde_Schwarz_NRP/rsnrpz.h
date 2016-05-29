/*= R&S NRP-Z Power Meter Include File ========================================*/
#ifndef __RSNRPZ_HEADER
#define __RSNRPZ_HEADER

#if defined(__linux__)
  #include <nrplib.h>
#else
  #if defined(__APPLE__)
    #include <RsNrpLib/nrplib.h>
  #else
    #include "NrpControl2.h"
  #endif
#endif

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

#if defined(_MSC_VER) || defined(_CVI_) || defined(__linux__) || defined(__APPLE__)
  #define _OUTSIDEVEE_
#endif 

#if !defined(__VISATYPE_HEADER__)

/*****************************************************************************/
/*= VISA constant and type definitions ======================================*/
/*****************************************************************************/
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)) && !defined(_NI_mswin16_)
#define _VI_FAR
#define _VI_FUNC            __stdcall
#define _VI_FUNCC           __cdecl
#define _VI_FUNCH           __stdcall
#define _VI_SIGNED          signed

#elif defined(_CVI_) && defined(_NI_i386_)

#define _VI_FAR
#define _VI_FUNC            _pascal
#define _VI_FUNCC
#define _VI_FUNCH           _pascal
#define _VI_SIGNED          signed
#endif

#if defined(__linux__) || defined(__APPLE__)
#define _VI_FAR
#define _VI_FUNC
#define _VI_FUNCC           cdecl
#define _VI_FUNCH
#define _VI_SIGNED
#endif


#define _VI_ERROR           (-2147483647L-1)  /* 0x80000000 */

#define VI_WARN_NSUP_ID_QUERY     (          0x3FFC0101L)
#define VI_WARN_NSUP_RESET        (          0x3FFC0102L)
#define VI_WARN_NSUP_SELF_TEST    (          0x3FFC0103L)
#define VI_WARN_NSUP_ERROR_QUERY  (          0x3FFC0104L)
#define VI_WARN_NSUP_REV_QUERY    (          0x3FFC0105L)
#define VI_WARN_UNKNOWN_STATUS    (          0x3FFF0085L)

#define VI_ERROR_PARAMETER1       (_VI_ERROR+0x3FFC0001L)
#define VI_ERROR_PARAMETER2       (_VI_ERROR+0x3FFC0002L)
#define VI_ERROR_PARAMETER3       (_VI_ERROR+0x3FFC0003L)
#define VI_ERROR_PARAMETER4       (_VI_ERROR+0x3FFC0004L)
#define VI_ERROR_PARAMETER5       (_VI_ERROR+0x3FFC0005L)
#define VI_ERROR_PARAMETER6       (_VI_ERROR+0x3FFC0006L)
#define VI_ERROR_PARAMETER7       (_VI_ERROR+0x3FFC0007L)
#define VI_ERROR_PARAMETER8       (_VI_ERROR+0x3FFC0008L)
#define VI_ERROR_FAIL_ID_QUERY    (_VI_ERROR+0x3FFC0011L)
#define VI_ERROR_INV_RESPONSE     (_VI_ERROR+0x3FFC0012L)

#if defined(_WIN64) || ((defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)) && !defined(_NI_mswin16_))
  // windows
#elif defined(__GNUC__) && (__GNUC__ >= 3)
  #include <limits.h>
  #include <sys/types.h>
#endif

#if defined(LONG_MAX) && (LONG_MAX > 0x7FFFFFFFL)
typedef unsigned int        ViUInt32;
typedef _VI_SIGNED int      ViInt32;
#else
typedef unsigned long       ViUInt32;
typedef _VI_SIGNED long     ViInt32;
#endif

typedef unsigned short      ViUInt16;
typedef _VI_SIGNED short    ViInt16;
typedef char                ViChar;
typedef ViChar*             ViString;
typedef ViString            ViRsrc;
typedef double              ViReal64;
typedef ViUInt16            ViBoolean;

typedef ViInt32             ViStatus;
typedef ViUInt32            ViSession;

/*- Completion and Error Codes ----------------------------------------------*/

#define VI_SUCCESS          (0L)

/*- Other VISA Definitions --------------------------------------------------*/

#define VI_NULL             (0)

#define VI_TRUE             (1)
#define VI_FALSE            (0)


#endif



/*****************************************************************************/
/*= Instrument Driver Specific Error/Warning Codes ==========================*/
/*****************************************************************************/
#define VI_ERROR_INSTR_INTERPRETING_RESPONSE    (_VI_ERROR+0x3FFC0803L)
#define VI_ERROR_INSTR_PARAMETER9               (_VI_ERROR+0x3FFC0809L)
#define VI_ERROR_INSTR_PARAMETER10              (_VI_ERROR+0x3FFC080AL)
#define VI_ERROR_INSTR_PARAMETER11              (_VI_ERROR+0x3FFC080BL)
#define VI_ERROR_INSTR_PARAMETER12              (_VI_ERROR+0x3FFC080CL)
#define VI_ERROR_INSTR_PARAMETER13              (_VI_ERROR+0x3FFC080DL)
#define VI_ERROR_INSTR_PARAMETER14              (_VI_ERROR+0x3FFC080EL)
#define VI_ERROR_INSTR_PARAMETER15              (_VI_ERROR+0x3FFC080FL)

/*****************************************************************************/
/*= Define Instrument Specific Error/Warning Codes Here =====================*/
/*****************************************************************************/
#define VI_WARNING_INSTR_OFFSET                         (0x3FFC0900L)
#define VI_ERROR_INSTR_OFFSET                   (_VI_ERROR+0x3FFC0900L)

#define RSNRPZ_ERROR_INSTRUMENT_ERROR           (VI_ERROR_INSTR_OFFSET + 0xF0L)

    /* Add instrument driver specific error codes here */
#define RSNRPZ_ERROR_INSTRUMENT_ERROR           (VI_ERROR_INSTR_OFFSET + 0xF0L)
#define RSNRPZ_ERROR_INVALID_CONFIGURATION      (VI_ERROR_INSTR_OFFSET + 0xF1L)
#define RSNRPZ_ERROR_INSTRUMENT_OPTION          (VI_ERROR_INSTR_OFFSET + 0xF2L)
#define RSNRPZ_ERROR_INSTRUMENT_NSUP_MODEL      (VI_ERROR_INSTR_OFFSET + 0xF3L)
#define RSNRPZ_ERROR_SETTINGS_CONFLICT          (VI_ERROR_INSTR_OFFSET + 0xF4L)
#define RSNRPZ_ERROR_INTERPRETING_RESPONSE      (VI_ERROR_INSTR_OFFSET + 0xF5L)
#define RSNRPZ_ERROR_TABLE_NOT_FOUND            (VI_ERROR_INSTR_OFFSET + 0xF6L)
#define RSNRPZ_ERROR_COMMAND_NOT_FOUND          (VI_ERROR_INSTR_OFFSET + 0xF7L)
#define RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED      (VI_ERROR_INSTR_OFFSET + 0xF8L)
#define RSNRPZ_ERROR_INVALID_VALUE              (VI_ERROR_INSTR_OFFSET + 0xF9L)
#define RSNRPZ_ERROR_INCORRECT_CHANNEL          (VI_ERROR_INSTR_OFFSET + 0xFAL)
#define RSNRPZ_ERROR_TIMEOUT                    (VI_ERROR_INSTR_OFFSET + 0xFBL)
#define RSNRPZ_ERROR_MAX_REGISTER_EVENTS        (VI_ERROR_INSTR_OFFSET + 0xFCL)
#define RSNRPZ_ERROR_REGISTER_EVENT             (VI_ERROR_INSTR_OFFSET + 0xFDL)
#define RSNRPZ_ERROR_SENSOR_ASSIGNED_TO_CHANNEL (VI_ERROR_INSTR_OFFSET + 0xFEL)
#define RSNRPZ_ERROR_SMALL_BUFFER               (VI_ERROR_INSTR_OFFSET + 0xFFL)
#define RSNRPZ_ERROR_MEAS_NOT_AVAILABLE         (VI_ERROR_INSTR_OFFSET + 0x100L)
#define RSNRPZ_ERROR_MAX_TIME_EXCEEDED          (VI_ERROR_INSTR_OFFSET + 0x101L)

#define RSNRPZ_WARNING_NO_CHANNEL               (VI_WARNING_INSTR_OFFSET + 0x01L)

/*****************************************************************************/
/*= Instrument specific defines =============================================*/
/*****************************************************************************/
#define RSNRPZ_ONCE                                 3
#define RSNRPZ_ZERO_LFR                             4
#define RSNRPZ_ZERO_UFR                             5

#define RSNRPZ_SLOPE_POSITIVE                       0
#define RSNRPZ_SLOPE_NEGATIVE                       1

#define RSNRPZ_AUTO_COUNT_TYPE_RESOLUTION           0
#define RSNRPZ_AUTO_COUNT_TYPE_NSR                  1

#define RSNRPZ_TERMINAL_CONTROL_MOVING              0
#define RSNRPZ_TERMINAL_CONTROL_REPEAT              1

#define RSNRPZ_SENSOR_MODE_CONTAV                   0
#define RSNRPZ_SENSOR_MODE_BUF_CONTAV               1
#define RSNRPZ_SENSOR_MODE_TIMESLOT                 2
#define RSNRPZ_SENSOR_MODE_BURST                    3
#define RSNRPZ_SENSOR_MODE_SCOPE                    4
#define RSNRPZ_SENSOR_MODE_CCDF                     6
#define RSNRPZ_SENSOR_MODE_PDF                      7

#define RSNRPZ_TRIGGER_SOURCE_BUS                   0
#define RSNRPZ_TRIGGER_SOURCE_EXTERNAL              1
#define RSNRPZ_TRIGGER_SOURCE_HOLD                  2
#define RSNRPZ_TRIGGER_SOURCE_IMMEDIATE             3
#define RSNRPZ_TRIGGER_SOURCE_INTERNAL              4

#define RSNRPZ_SAMPLING_FREQUENCY1                  1
#define RSNRPZ_SAMPLING_FREQUENCY2                  2

#define RSNRPZ_AUX_NONE                             0
#define RSNRPZ_AUX_MINMAX                           1
#define RSNRPZ_AUX_RNDMAX                           2


#define RSNRPZ_VAL_MAX_TIME_INFINITE                0xFFFFFFFF
#define RSNRPZ_VAL_MAX_TIME_IMMEDIATE               0

#define RSNRPZ_MEASUREMENT_SINGLE     0
#define RSNRPZ_MEASUREMENT_SINGLE_REL 1
#define RSNRPZ_MEASUREMENT_DIFF       2
#define RSNRPZ_MEASUREMENT_DIFF_REL   3
#define RSNRPZ_MEASUREMENT_SUM        4
#define RSNRPZ_MEASUREMENT_SUM_REL    5
#define RSNRPZ_MEASUREMENT_RATIO      6
#define RSNRPZ_MEASUREMENT_RATIO_REL  7
#define RSNRPZ_MEASUREMENT_SWR        8
#define RSNRPZ_MEASUREMENT_REFL       9
#define RSNRPZ_MEASUREMENT_RLOS       10

/*- Register bits -*/
#define RSNRPZ_STATCLASS_D_CONN         (1)
#define RSNRPZ_STATCLASS_D_ERR          (2)
#define RSNRPZ_STATCLASS_O_CAL          (3)
#define RSNRPZ_STATCLASS_O_MEAS         (4)
#define RSNRPZ_STATCLASS_O_TRIGGER      (5)
#define RSNRPZ_STATCLASS_O_SENSE        (6)
#define RSNRPZ_STATCLASS_O_LOWER        (7)
#define RSNRPZ_STATCLASS_O_UPPER        (8)
#define RSNRPZ_STATCLASS_Q_POWER        (9)
#define RSNRPZ_STATCLASS_Q_WINDOW       (10)
#define RSNRPZ_STATCLASS_Q_CAL          (11)
#define RSNRPZ_STATCLASS_Q_ZER          (12)

#define RSNRPZ_DIRECTION_NONE       0
#define RSNRPZ_DIRECTION_PTR        1
#define RSNRPZ_DIRECTION_NTR        2
#define RSNRPZ_DIRECTION_BOTH       3

#define RSNRPZ_ALL_SENSORS    0xFFFFFFFE
#define RSNRPZ_SENSOR_01      0x00000002 /* (1L << 1) */
#define RSNRPZ_SENSOR_02      0x00000004 /* (1L << 2) */
#define RSNRPZ_SENSOR_03      0x00000008 /* (1L << 3) */
#define RSNRPZ_SENSOR_04      0x00000010 /* (1L << 4) */
#define RSNRPZ_SENSOR_05      0x00000020 /* (1L << 5) */
#define RSNRPZ_SENSOR_06      0x00000040 /* (1L << 6) */
#define RSNRPZ_SENSOR_07      0x00000080 /* (1L << 7) */
#define RSNRPZ_SENSOR_08      0x00000100 /* (1L << 8) */
#define RSNRPZ_SENSOR_09      0x00000200 /* (1L << 9) */
#define RSNRPZ_SENSOR_10      0x00000400 /* (1L << 10)*/
#define RSNRPZ_SENSOR_11      0x00000800 /* (1L << 11)*/
#define RSNRPZ_SENSOR_12      0x00001000 /* (1L << 12)*/
#define RSNRPZ_SENSOR_13      0x00002000 /* (1L << 13)*/
#define RSNRPZ_SENSOR_14      0x00004000 /* (1L << 14)*/
#define RSNRPZ_SENSOR_15      0x00008000 /* (1L << 15)*/
#define RSNRPZ_SENSOR_16      0x00010000 /* (1L << 16)*/
#define RSNRPZ_SENSOR_17      0x00020000 /* (1L << 17)*/
#define RSNRPZ_SENSOR_18      0x00040000 /* (1L << 18)*/
#define RSNRPZ_SENSOR_19      0x00080000 /* (1L << 19)*/
#define RSNRPZ_SENSOR_20      0x00100000 /* (1L << 20)*/
#define RSNRPZ_SENSOR_21      0x00200000 /* (1L << 21)*/
#define RSNRPZ_SENSOR_22      0x00400000 /* (1L << 22)*/
#define RSNRPZ_SENSOR_23      0x00800000 /* (1L << 23)*/
#define RSNRPZ_SENSOR_24      0x01000000 /* (1L << 24)*/
#define RSNRPZ_SENSOR_25      0x02000000 /* (1L << 25)*/
#define RSNRPZ_SENSOR_26      0x04000000 /* (1L << 26)*/
#define RSNRPZ_SENSOR_27      0x08000000 /* (1L << 27)*/
#define RSNRPZ_SENSOR_28      0x10000000 /* (1L << 28)*/
#define RSNRPZ_SENSOR_29      0x20000000 /* (1L << 29)*/
#define RSNRPZ_SENSOR_30      0x40000000 /* (1L << 30)*/
#define RSNRPZ_SENSOR_31      0x80000000 /* (1L << 31)*/

#define RSNRPZ_SCOPE_MEAS_ALG_HIST 0
#define RSNRPZ_SCOPE_MEAS_ALG_INT  1
#define RSNRPZ_SCOPE_MEAS_ALG_PEAK 3

#define RSNRPZ_SPACING_LINEAR      0
#define RSNRPZ_SPACING_LOG         1

#define RSNRPZ_Z5_PORT_A           0
#define RSNRPZ_Z5_PORT_B           1
#define RSNRPZ_Z5_PORT_C           2
#define RSNRPZ_Z5_PORT_D           3

/*****************************************************************************/
/*= GLOBAL USER-CALLABLE FUNCTION DECLARATIONS (Exportable Functions) =======*/
/*****************************************************************************/
ViStatus _VI_FUNC rsnrpz_chan_getCacheRange( ViSession instrumentHandle,
                                             ViInt32 channel, 
                                             const char *pszCommand, 
                                             ViReal64 *pdCurrent, 
                                             ViReal64 *pdMin, 
                                             ViReal64 *pdMax );
ViStatus _VI_FUNC rsnrpz_setTimeout(     ViUInt32 ulNewTimo );
ViStatus _VI_FUNC rsnrpz_getTimeout(     ViUInt32 *pulNewTimo );
ViStatus _VI_FUNC rsnrpz_init(           ViRsrc resourceName,
                                         ViBoolean IDQuery,
                                         ViBoolean resetDevice,
                                         ViSession *instrumentHandle );
ViStatus _VI_FUNC rsnrpz_long_distance_setup( 
                                         ViBoolean IDQuery,
                                         ViBoolean resetDevice,
                                         ViString  resourceName,
                                         ViSession *instrumentHandle );
ViStatus _VI_FUNC rsnrpz_AddSensor(      ViSession instrumentHandle,
                                         ViInt32 channel,
                                         ViRsrc resourceName,
                                         ViBoolean IDQuery,
                                         ViBoolean resetDevice );
ViStatus _VI_FUNC rsnrpz_chans_abort(    ViSession instrumentHandle);
ViStatus _VI_FUNC rsnrpz_chans_getCount( ViSession instrumentHandle,
                                         ViInt32* count );
ViStatus _VI_FUNC rsnrpz_chans_initiate( ViSession instrumentHandle );
ViStatus _VI_FUNC rsnrpz_chans_zero(     ViSession instrumentHandle );
ViStatus _VI_FUNC rsnrpz_chan_zeroAdvanced(ViSession instrumentHandle,
                                         ViInt32 channel,
                                         ViInt32 zeroing );
ViStatus _VI_FUNC rsnrpz_chans_isZeroingComplete (ViSession instrumentHandle,
                                                  ViBoolean* zeroingCompleted);
ViStatus _VI_FUNC rsnrpz_chans_isMeasurementComplete (ViSession instrumentHandle,
                                                      ViBoolean* measurementCompleted);
ViStatus _VI_FUNC rsnrpz_chan_mode (ViSession instrumentHandle, ViInt32 channel,
                                    ViInt32 measurementMode);
ViStatus _VI_FUNC rsnrpz_timing_configureExclude (ViSession instrumentHandle,
                                                  ViInt32 channel,
                                                  ViReal64 excludeStart,
                                                  ViReal64 excludeStop);
ViStatus _VI_FUNC rsnrpz_timing_setTimingExcludeStart (ViSession instrumentHandle,
                                                       ViInt32 channel,
                                                       ViReal64 excludeStart);
ViStatus _VI_FUNC rsnrpz_timing_getTimingExcludeStart (ViSession instrumentHandle,
                                                       ViInt32 channel,
                                                       ViReal64* excludeStart);
ViStatus _VI_FUNC rsnrpz_timing_setTimingExcludeStop (ViSession instrumentHandle,
                                                      ViInt32 channel,
                                                      ViReal64 excludeStop);
ViStatus _VI_FUNC rsnrpz_timing_getTimingExcludeStop (ViSession instrumentHandle,
                                                      ViInt32 channel,
                                                      ViReal64* excludeStop);
ViStatus _VI_FUNC rsnrpz_bandwidth_setBw (ViSession instrumentHandle,
                                          ViInt32 channel, ViInt32 bandwidth);
ViStatus _VI_FUNC rsnrpz_bandwidth_getBw (ViSession instrumentHandle,
                                          ViInt32 channel, ViInt32* bandwidth);
ViStatus _VI_FUNC rsnrpz_bandwidth_getBwList (ViSession instrumentHandle,
                                              ViInt32 channel, ViInt32 bufferSize,
                                              ViChar _VI_FAR bandwidthList[]);
ViStatus _VI_FUNC rsnrpz_avg_configureAvgAuto (ViSession instrumentHandle,
                                               ViInt32 channel, ViInt32 resolution);
ViStatus _VI_FUNC rsnrpz_avg_configureAvgNSRatio (ViSession instrumentHandle,
                                                  ViInt32 channel,
                                                  ViReal64 maximumNoiseRatio,
                                                  ViReal64 upperTimeLimit);
ViStatus _VI_FUNC rsnrpz_avg_configureAvgManual (ViSession instrumentHandle,
                                                 ViInt32 channel, ViInt32 count);
ViStatus _VI_FUNC rsnrpz_avg_setAutoEnabled (ViSession instrumentHandle,
                                             ViInt32 channel,
                                             ViBoolean autoEnabled);
ViStatus _VI_FUNC rsnrpz_avg_getAutoEnabled (ViSession instrumentHandle,
                                             ViInt32 channel,
                                             ViBoolean* autoEnabled);
ViStatus _VI_FUNC rsnrpz_avg_setAutoMaxMeasuringTime (ViSession instrumentHandle,
                                                      ViInt32 channel,
                                                      ViReal64 upperTimeLimit);
ViStatus _VI_FUNC rsnrpz_avg_getAutoMaxMeasuringTime (ViSession instrumentHandle,
                                                      ViInt32 channel,
                                                      ViReal64* upperTimeLimit);
ViStatus _VI_FUNC rsnrpz_avg_setAutoNoiseSignalRatio (ViSession instrumentHandle,
                                                      ViInt32 channel,
                                                      ViReal64 maximumNoiseRatio);
ViStatus _VI_FUNC rsnrpz_avg_getAutoNoiseSignalRatio (ViSession instrumentHandle,
                                                      ViInt32 channel,
                                                      ViReal64* maximumNoiseRatio);
ViStatus _VI_FUNC rsnrpz_avg_setAutoResolution (ViSession instrumentHandle,
                                                ViInt32 channel,
                                                ViInt32 resolution);
ViStatus _VI_FUNC rsnrpz_avg_getAutoResolution (ViSession instrumentHandle,
                                                ViInt32 channel,
                                                ViInt32* resolution);
ViStatus _VI_FUNC rsnrpz_avg_setAutoType (ViSession instrumentHandle,
                                          ViInt32 channel, ViInt32 method);
ViStatus _VI_FUNC rsnrpz_avg_getAutoType (ViSession instrumentHandle,
                                          ViInt32 channel, ViInt32* method);
ViStatus _VI_FUNC rsnrpz_avg_setCount (ViSession instrumentHandle, ViInt32 channel,
                                       ViInt32 count);
ViStatus _VI_FUNC rsnrpz_avg_getCount (ViSession instrumentHandle, ViInt32 channel,
                                       ViInt32* count);
ViStatus _VI_FUNC rsnrpz_avg_setEnabled (ViSession instrumentHandle,
                                         ViInt32 channel, ViBoolean averaging);
ViStatus _VI_FUNC rsnrpz_avg_getEnabled (ViSession instrumentHandle,
                                         ViInt32 channel, ViBoolean* averaging);
ViStatus _VI_FUNC rsnrpz_avg_setSlot (ViSession instrumentHandle, ViInt32 channel,
                                      ViInt32 timeslot);
ViStatus _VI_FUNC rsnrpz_avg_getSlot (ViSession instrumentHandle, ViInt32 channel,
                                      ViInt32* timeslot);
ViStatus _VI_FUNC rsnrpz_avg_setTerminalControl (ViSession instrumentHandle,
                                                 ViInt32 channel,
                                                 ViInt32 terminalControl);
ViStatus _VI_FUNC rsnrpz_avg_getTerminalControl (ViSession instrumentHandle,
                                                 ViInt32 channel,
                                                 ViInt32* terminalControl);
ViStatus _VI_FUNC rsnrpz_avg_reset (ViSession instrumentHandle, ViInt32 channel);
ViStatus _VI_FUNC rsnrpz_range_setAutoEnabled (ViSession instrumentHandle,
                                               ViInt32 channel,
                                               ViBoolean autoRange);
ViStatus _VI_FUNC rsnrpz_range_getAutoEnabled (ViSession instrumentHandle,
                                               ViInt32 channel,
                                               ViBoolean* autoRange);
ViStatus _VI_FUNC rsnrpz_range_setCrossoverLevel (ViSession instrumentHandle,
                                                  ViInt32 channel,
                                                  ViReal64 crossoverLevel);
ViStatus _VI_FUNC rsnrpz_range_getCrossoverLevel (ViSession instrumentHandle,
                                                  ViInt32 channel,
                                                  ViReal64* crossoverLevel);
ViStatus _VI_FUNC rsnrpz_range_setRange (ViSession instrumentHandle,
                                         ViInt32 channel, ViInt32 range);
ViStatus _VI_FUNC rsnrpz_range_getRange (ViSession instrumentHandle,
                                         ViInt32 channel, ViInt32* range);
ViStatus _VI_FUNC rsnrpz_corr_configureCorrections (ViSession instrumentHandle,
                                                    ViInt32 channel,
                                                    ViBoolean offsetState,
                                                    ViReal64 offset,
                                                    ViBoolean reserved1,
                                                    ViString reserved2,
                                                    ViBoolean sParameterEnable);
ViStatus _VI_FUNC rsnrpz_chan_setCorrectionFrequency (ViSession instrumentHandle,
                                                      ViInt32 channel,
                                                      ViReal64 frequency);
ViStatus _VI_FUNC rsnrpz_chan_getCorrectionFrequency (ViSession instrumentHandle,
                                                      ViInt32 channel,
                                                      ViReal64* frequency);
ViStatus _VI_FUNC rsnrpz_chan_setCorrectionFrequencyStep (ViSession instrumentHandle,
                                                          ViInt32 channel,
                                                          ViReal64 frequencyStep);
ViStatus _VI_FUNC rsnrpz_chan_getCorrectionFrequencyStep (ViSession instrumentHandle,
                                                          ViInt32 channel,
                                                          ViReal64 *frequencyStep);
ViStatus _VI_FUNC rsnrpz_chan_setCorrectionFrequencySpacing (ViSession instrumentHandle,
                                                             ViInt32 channel,
                                                             ViInt32 frequencySpacing);
ViStatus _VI_FUNC rsnrpz_chan_getCorrectionFrequencySpacing (ViSession instrumentHandle,
                                                             ViInt32 channel,
                                                             ViInt32 *frequencySpacing);
ViStatus _VI_FUNC rsnrpz_corr_setOffset (ViSession instrumentHandle,
                                         ViInt32 channel, ViReal64 offset);
ViStatus _VI_FUNC rsnrpz_corr_getOffset (ViSession instrumentHandle,
                                         ViInt32 channel, ViReal64* offset);
ViStatus _VI_FUNC rsnrpz_corr_setOffsetEnabled (ViSession instrumentHandle,
                                                ViInt32 channel,
                                                ViBoolean offsetState);
ViStatus _VI_FUNC rsnrpz_corr_getOffsetEnabled (ViSession instrumentHandle,
                                                ViInt32 channel,
                                                ViBoolean* offsetState);
ViStatus _VI_FUNC rsnrpz_corr_setSParamDeviceEnabled (ViSession instrumentHandle,
                                                      ViInt32 channel,
                                                      ViBoolean sParameterEnable);
ViStatus _VI_FUNC rsnrpz_corr_getSParamDeviceEnabled (ViSession instrumentHandle,
                                                      ViInt32 channel,
                                                      ViBoolean* sParameterCorrection);
ViStatus _VI_FUNC rsnrpz_corr_setSParamDevice (ViSession instrumentHandle,
                                      ViInt32 channel, ViInt32 sParameter);
ViStatus _VI_FUNC rsnrpz_corr_getSParamDevice (ViSession instrumentHandle,
                                      ViInt32 channel, ViInt32 *sParameter);
ViStatus _VI_FUNC rsnrpz_corr_getSParamDevList( ViSession vi,
                                                ViInt32   channel,
                                                ViInt32   iSpdListSize,
                                                ViChar    spdList[] );
ViStatus _VI_FUNC rsnrpz_chan_configureSourceGammaCorr (ViSession instrumentHandle,
                                                        ViInt32 channel,
                                                        ViBoolean sourceGammaCorrection,
                                                        ViReal64 magnitude,
                                                        ViReal64 phase);
ViStatus _VI_FUNC rsnrpz_chan_setSourceGammaMagnitude (ViSession instrumentHandle,
                                                       ViInt32 channel,
                                                       ViReal64 magnitude);
ViStatus _VI_FUNC rsnrpz_chan_getSourceGammaMagnitude (ViSession instrumentHandle,
                                                       ViInt32 channel,
                                                       ViReal64* magnitude);
ViStatus _VI_FUNC rsnrpz_chan_setSourceGammaPhase (ViSession instrumentHandle,
                                                   ViInt32 channel, ViReal64 phase);
ViStatus _VI_FUNC rsnrpz_chan_getSourceGammaPhase (ViSession instrumentHandle,
                                                   ViInt32 channel,
                                                   ViReal64* phase);
ViStatus _VI_FUNC rsnrpz_chan_setSourceGammaCorrEnabled (ViSession instrumentHandle,
                                                         ViInt32 channel,
                                                         ViBoolean sourceGammaCorrection);
ViStatus _VI_FUNC rsnrpz_chan_getSourceGammaCorrEnabled (ViSession instrumentHandle,
                                                         ViInt32 channel,
                                                         ViBoolean* sourceGammaCorrection);
ViStatus _VI_FUNC rsnrpz_chan_configureReflectGammaCorr (ViSession instrumentHandle,
                                                         ViInt32 channel,
                                                         ViReal64 magnitude,
                                                         ViReal64 phase);
ViStatus _VI_FUNC rsnrpz_chan_setReflectionGammaMagn (ViSession instrumentHandle,
                                                      ViInt32 channel,
                                                      ViReal64 magnitude);
ViStatus _VI_FUNC rsnrpz_chan_getReflectionGammaMagn (ViSession instrumentHandle,
                                                      ViInt32 channel,
                                                      ViReal64* magnitude);
ViStatus _VI_FUNC rsnrpz_chan_setReflectionGammaPhase (ViSession instrumentHandle,
                                                       ViInt32 channel,
                                                       ViReal64 phase);
ViStatus _VI_FUNC rsnrpz_chan_getReflectionGammaPhase (ViSession instrumentHandle,
                                                       ViInt32 channel,
                                                       ViReal64* phase);
ViStatus _VI_FUNC rsnrpz_chan_setReflectionGammaUncertainty (ViSession instrumentHandle,
                                                    ViInt32 channel,
                                                    ViReal64 uncertainty);
ViStatus _VI_FUNC rsnrpz_chan_getReflectionGammaUncertainty (ViSession instrumentHandle,
                                                    ViInt32 channel,
                                                    ViReal64 *uncertainty);
ViStatus _VI_FUNC rsnrpz_corr_configureDutyCycle (ViSession instrumentHandle,
                                                  ViInt32 channel,
                                                  ViBoolean dutyCycleState,
                                                  ViReal64 dutyCycle);
ViStatus _VI_FUNC rsnrpz_corr_setDutyCycle (ViSession instrumentHandle,
                                            ViInt32 channel, ViReal64 dutyCycle);
ViStatus _VI_FUNC rsnrpz_corr_getDutyCycle (ViSession instrumentHandle,
                                            ViInt32 channel, ViReal64* dutyCycle);
ViStatus _VI_FUNC rsnrpz_corr_setDutyCycleEnabled (ViSession instrumentHandle,
                                                   ViInt32 channel,
                                                   ViBoolean dutyCycleState);
ViStatus _VI_FUNC rsnrpz_corr_getDutyCycleEnabled (ViSession instrumentHandle,
                                                   ViInt32 channel,
                                                   ViBoolean* dutyCycleState);
ViStatus _VI_FUNC rsnrpz_chan_setContAvAperture (ViSession instrumentHandle,
                                                 ViInt32 channel,
                                                 ViReal64 contAvAperture);
ViStatus _VI_FUNC rsnrpz_chan_getContAvAperture (ViSession instrumentHandle,
                                                 ViInt32 channel,
                                                 ViReal64* contAvAperture);
ViStatus _VI_FUNC rsnrpz_chan_setContAvSmoothingEnabled (ViSession instrumentHandle,
                                                         ViInt32 channel,
                                                         ViBoolean contAvSmoothing);
ViStatus _VI_FUNC rsnrpz_chan_getContAvSmoothingEnabled (ViSession instrumentHandle,
                                                         ViInt32 channel,
                                                         ViBoolean* contAvSmoothing);
ViStatus _VI_FUNC rsnrpz_chan_setContAvBufferedEnabled (ViSession instrumentHandle,
                                                        ViInt32 channel,
                                                        ViBoolean contAvBufferedMode);
ViStatus _VI_FUNC rsnrpz_chan_getContAvBufferedEnabled (ViSession instrumentHandle,
                                                        ViInt32 channel,
                                                        ViBoolean* contAvBufferedMode);
ViStatus _VI_FUNC rsnrpz_chan_setContAvBufferSize (ViSession instrumentHandle,
                                                   ViInt32 channel,
                                                   ViInt32 bufferSize);
ViStatus _VI_FUNC rsnrpz_chan_getContAvBufferSize (ViSession instrumentHandle,
                                                   ViInt32 channel,
                                                   ViInt32* bufferSize);
ViStatus _VI_FUNC rsnrpz_chan_getContAvBufferCount (ViSession instrumentHandle,
                                                    ViInt32 channel,
                                                    ViInt32 *bufferCount);
ViStatus _VI_FUNC rsnrpz_chan_getContAvBufferInfo (ViSession instrumentHandle,
                                                   ViInt32 channel,
                                                   ViString infoType,
                                                   ViInt32 arraySize,
                                                   ViChar info[]);
ViStatus _VI_FUNC rsnrpz_chan_setBurstDropoutTolerance (ViSession instrumentHandle,
                                                        ViInt32 channel,
                                                        ViReal64 dropoutTolerance);
ViStatus _VI_FUNC rsnrpz_chan_getBurstDropoutTolerance (ViSession instrumentHandle,
                                                        ViInt32 channel,
                                                        ViReal64* dropoutTolerance);
ViStatus _VI_FUNC rsnrpz_chan_setBurstChopperEnabled (ViSession instrumentHandle,
                                                      ViInt32 channel,
                                                      ViBoolean burstAvChopper);
ViStatus _VI_FUNC rsnrpz_chan_getBurstChopperEnabled (ViSession instrumentHandle,
                                                      ViInt32 channel,
                                                      ViBoolean* burstAvChopper);
ViStatus _VI_FUNC rsnrpz_stat_confTimegate (ViSession instrumentHandle,
                                            ViInt32 channel, ViReal64 offset,
                                            ViReal64 time, ViReal64 midambleOffset,
                                            ViReal64 midambleLength);
ViStatus _VI_FUNC rsnrpz_stat_confScale (ViSession instrumentHandle,
                                         ViInt32 channel, ViReal64 referenceLevel,
                                         ViReal64 range, ViInt32 points);
ViStatus _VI_FUNC rsnrpz_stat_setOffsetTime (ViSession instrumentHandle,
                                             ViInt32 channel, ViReal64 offset);
ViStatus _VI_FUNC rsnrpz_stat_getOffsetTime (ViSession instrumentHandle,
                                             ViInt32 channel, ViReal64* offset);
ViStatus _VI_FUNC rsnrpz_stat_setTime (ViSession instrumentHandle, ViInt32 channel,
                                       ViReal64 time);
ViStatus _VI_FUNC rsnrpz_stat_getTime (ViSession instrumentHandle, ViInt32 channel,
                                       ViReal64* time);
ViStatus _VI_FUNC rsnrpz_stat_setMidOffset (ViSession instrumentHandle,
                                            ViInt32 channel, ViReal64 offset);
ViStatus _VI_FUNC rsnrpz_stat_getMidOffset (ViSession instrumentHandle,
                                            ViInt32 channel, ViReal64* offset);
ViStatus _VI_FUNC rsnrpz_stat_setMidLength (ViSession instrumentHandle,
                                            ViInt32 channel, ViReal64 length);
ViStatus _VI_FUNC rsnrpz_stat_getMidLength (ViSession instrumentHandle,
                                            ViInt32 channel, ViReal64* length);
ViStatus _VI_FUNC rsnrpz_stat_setScaleRefLevel (ViSession instrumentHandle,
                                                ViInt32 channel,
                                                ViReal64 referenceLevel);
ViStatus _VI_FUNC rsnrpz_stat_getScaleRefLevel (ViSession instrumentHandle,
                                                ViInt32 channel,
                                                ViReal64* referenceLevel);
ViStatus _VI_FUNC rsnrpz_stat_setScaleRange (ViSession instrumentHandle,
                                             ViInt32 channel, ViReal64 range);
ViStatus _VI_FUNC rsnrpz_stat_getScaleRange (ViSession instrumentHandle,
                                             ViInt32 channel, ViReal64* range);
ViStatus _VI_FUNC rsnrpz_stat_setScalePoints (ViSession instrumentHandle,
                                              ViInt32 channel, ViInt32 points);
ViStatus _VI_FUNC rsnrpz_stat_getScalePoints (ViSession instrumentHandle,
                                              ViInt32 channel, ViInt32* points);
ViStatus _VI_FUNC rsnrpz_stat_getScaleWidth (ViSession instrumentHandle,
                                             ViInt32 channel, ViReal64* width);
ViStatus _VI_FUNC rsnrpz_tslot_configureTimeSlot (ViSession instrumentHandle,
                                                  ViInt32 channel,
                                                  ViInt32 timeSlotCount,
                                                  ViReal64 width);
ViStatus _VI_FUNC rsnrpz_tslot_setTimeSlotCount (ViSession instrumentHandle,
                                                 ViInt32 channel,
                                                 ViInt32 timeSlotCount);
ViStatus _VI_FUNC rsnrpz_tslot_getTimeSlotCount (ViSession instrumentHandle,
                                                 ViInt32 channel,
                                                 ViInt32* timeSlotCount);
ViStatus _VI_FUNC rsnrpz_tslot_setTimeSlotWidth (ViSession instrumentHandle,
                                                 ViInt32 channel, ViReal64 width);
ViStatus _VI_FUNC rsnrpz_tslot_getTimeSlotWidth (ViSession instrumentHandle,
                                                 ViInt32 channel, ViReal64* width);
ViStatus _VI_FUNC rsnrpz_tslot_setTimeSlotMidOffset (ViSession instrumentHandle,
                                                     ViInt32 channel,
                                                     ViReal64 offset);
ViStatus _VI_FUNC rsnrpz_tslot_getTimeSlotMidOffset (ViSession instrumentHandle,
                                                     ViInt32 channel,
                                                     ViReal64* offset);
ViStatus _VI_FUNC rsnrpz_tslot_setTimeSlotMidLength (ViSession instrumentHandle,
                                                     ViInt32 channel,
                                                     ViReal64 length);
ViStatus _VI_FUNC rsnrpz_tslot_getTimeSlotMidLength (ViSession instrumentHandle,
                                                     ViInt32 channel,
                                                     ViReal64* length);
ViStatus _VI_FUNC rsnrpz_tslot_setTimeSlotChopperEnabled (ViSession instrumentHandle,
                                                          ViInt32 channel,
                                                          ViBoolean timeSlotChopper);
ViStatus _VI_FUNC rsnrpz_tslot_getTimeSlotChopperEnabled (ViSession instrumentHandle,
                                                          ViInt32 channel,
                                                          ViBoolean* timeSlotChopper);
ViStatus _VI_FUNC rsnrpz_scope_configureScope (ViSession instrumentHandle,
                                               ViInt32 channel, ViInt32 scopePoints,
                                               ViReal64 scopeTime,
                                               ViReal64 offsetTime,
                                               ViBoolean realtime);
ViStatus _VI_FUNC rsnrpz_scope_fastZero (ViSession instrumentHandle);
ViStatus _VI_FUNC rsnrpz_scope_setAverageEnabled (ViSession instrumentHandle,
                                                  ViInt32 channel,
                                                  ViBoolean scopeAveraging);
ViStatus _VI_FUNC rsnrpz_scope_getAverageEnabled (ViSession instrumentHandle,
                                                  ViInt32 channel,
                                                  ViBoolean* scopeAveraging);
ViStatus _VI_FUNC rsnrpz_scope_setAverageCount (ViSession instrumentHandle,
                                                ViInt32 channel, ViInt32 count);
ViStatus _VI_FUNC rsnrpz_scope_getAverageCount (ViSession instrumentHandle,
                                                ViInt32 channel, ViInt32* count);
ViStatus _VI_FUNC rsnrpz_scope_setAverageTerminalControl (ViSession instrumentHandle,
                                                          ViInt32 channel,
                                                          ViInt32 terminalControl);
ViStatus _VI_FUNC rsnrpz_scope_getAverageTerminalControl (ViSession instrumentHandle,
                                                          ViInt32 channel,
                                                          ViInt32* terminalControl);
ViStatus _VI_FUNC rsnrpz_scope_setOffsetTime (ViSession instrumentHandle,
                                              ViInt32 channel, ViReal64 offsetTime);
ViStatus _VI_FUNC rsnrpz_scope_getOffsetTime (ViSession instrumentHandle,
                                              ViInt32 channel,
                                              ViReal64* offsetTime);
ViStatus _VI_FUNC rsnrpz_scope_setPoints (ViSession instrumentHandle,
                                          ViInt32 channel, ViInt32 scopePoints);
ViStatus _VI_FUNC rsnrpz_scope_getPoints (ViSession instrumentHandle,
                                          ViInt32 channel, ViInt32* scopePoints);
ViStatus _VI_FUNC rsnrpz_scope_setRealtimeEnabled (ViSession instrumentHandle,
                                                   ViInt32 channel,
                                                   ViBoolean realtime);
ViStatus _VI_FUNC rsnrpz_scope_getRealtimeEnabled (ViSession instrumentHandle,
                                                   ViInt32 channel,
                                                   ViBoolean* realtime);
ViStatus _VI_FUNC rsnrpz_scope_setTime (ViSession instrumentHandle, ViInt32 channel,
                                        ViReal64 scopeTime);
ViStatus _VI_FUNC rsnrpz_scope_getTime (ViSession instrumentHandle, ViInt32 channel,
                                        ViReal64* scopeTime);
ViStatus _VI_FUNC rsnrpz_scope_setAutoEnabled (ViSession instrumentHandle,
                                               ViInt32 channel,
                                               ViBoolean autoEnabled);
ViStatus _VI_FUNC rsnrpz_scope_getAutoEnabled (ViSession instrumentHandle,
                                               ViInt32 channel,
                                               ViBoolean* autoEnabled);
ViStatus _VI_FUNC rsnrpz_scope_setAutoMaxMeasuringTime (ViSession instrumentHandle,
                                                        ViInt32 channel,
                                                        ViReal64 upperTimeLimit);
ViStatus _VI_FUNC rsnrpz_scope_getAutoMaxMeasuringTime (ViSession instrumentHandle,
                                                        ViInt32 channel,
                                                        ViReal64* upperTimeLimit);
ViStatus _VI_FUNC rsnrpz_scope_setAutoNoiseSignalRatio (ViSession instrumentHandle,
                                                        ViInt32 channel,
                                                        ViReal64 maximumNoiseRatio);
ViStatus _VI_FUNC rsnrpz_scope_getAutoNoiseSignalRatio (ViSession instrumentHandle,
                                                        ViInt32 channel,
                                                        ViReal64* maximumNoiseRatio);
ViStatus _VI_FUNC rsnrpz_scope_setAutoResolution (ViSession instrumentHandle,
                                                  ViInt32 channel,
                                                  ViInt32 resolution);
ViStatus _VI_FUNC rsnrpz_scope_getAutoResolution (ViSession instrumentHandle,
                                                  ViInt32 channel,
                                                  ViInt32* resolution);
ViStatus _VI_FUNC rsnrpz_scope_setAutoType (ViSession instrumentHandle,
                                            ViInt32 channel, ViInt32 method);
ViStatus _VI_FUNC rsnrpz_scope_getAutoType (ViSession instrumentHandle,
                                            ViInt32 channel, ViInt32* method);
ViStatus _VI_FUNC rsnrpz_scope_setEquivalentSampling (ViSession instrumentHandle,
                                                      ViInt32 channel,
                                                      ViBoolean scopeEquivalentSampling);
ViStatus _VI_FUNC rsnrpz_scope_getEquivalentSampling (ViSession instrumentHandle,
                                                      ViInt32 channel,
                                                      ViBoolean *scopeEquivalentSampling);
ViStatus _VI_FUNC rsnrpz_scope_meas_setMeasEnabled (ViSession instrumentHandle,
                                           ViInt32 channel,
                                           ViBoolean traceMeasurements);
ViStatus _VI_FUNC rsnrpz_scope_meas_getMeasEnabled (ViSession instrumentHandle,
                                           ViInt32 channel,
                                           ViBoolean *traceMeasurements);
ViStatus _VI_FUNC rsnrpz_scope_meas_setMeasAlgorithm (ViSession instrumentHandle,
                                             ViInt32 channel, ViInt32 algorithm);
ViStatus _VI_FUNC rsnrpz_scope_meas_getMeasAlgorithm (ViSession instrumentHandle,
                                             ViInt32 channel, ViInt32 *algorithm);
ViStatus _VI_FUNC rsnrpz_scope_meas_setLevelThresholds (ViSession instrumentHandle,
                                                   ViInt32 channel,
                                                   ViReal64 durationRef,
                                                   ViReal64 transitionLowRef,
                                                   ViReal64 transitionHighRef);
ViStatus _VI_FUNC rsnrpz_scope_meas_getLevelThresholds (ViSession instrumentHandle,
                                                   ViInt32 channel,
                                                   ViReal64 *durationRef,
                                                   ViReal64 *transitionLowRef,
                                                   ViReal64 *transitionHighRef);
ViStatus _VI_FUNC rsnrpz_scope_meas_setTime (ViSession instrumentHandle,
                                             ViInt32 channel, ViReal64 measTime);
ViStatus _VI_FUNC rsnrpz_scope_meas_getTime (ViSession instrumentHandle,
                                             ViInt32 channel, ViReal64 *measTime);
ViStatus _VI_FUNC rsnrpz_scope_meas_setOffsetTime (ViSession instrumentHandle,
                                              ViInt32 channel,
                                              ViReal64 offsetTime);
ViStatus _VI_FUNC rsnrpz_scope_meas_getOffsetTime (ViSession instrumentHandle,
                                              ViInt32 channel,
                                              ViReal64 *offsetTime);
ViStatus _VI_FUNC rsnrpz_scope_meas_getPulseTimes (ViSession instrumentHandle,
                                              ViInt32 channel,
                                              ViReal64 *dutyCycle,
                                              ViReal64 *pulseDuration,
                                              ViReal64 *pulsePeriod);
ViStatus _VI_FUNC rsnrpz_scope_meas_getPulseTransition (ViSession instrumentHandle,
                                                   ViInt32 channel,
                                                   ViInt32 slope,
                                                   ViReal64 *duration,
                                                   ViReal64 *occurence,
                                                   ViReal64 *overshoot);
ViStatus _VI_FUNC rsnrpz_scope_meas_getPulsePower (ViSession instrumentHandle,
                                              ViInt32 channel, ViReal64 *average,
                                              ViReal64 *minPeak,
                                              ViReal64 *maxPeak);
ViStatus _VI_FUNC rsnrpz_scope_meas_getPulseLevels (ViSession instrumentHandle,
                                               ViInt32 channel,
                                               ViReal64 *topLevel,
                                               ViReal64 *baseLevel);
ViStatus _VI_FUNC rsnrpz_scope_meas_getPulseReferenceLevels (ViSession instrumentHandle,
                                                        ViInt32 channel,
                                                        ViReal64 *lowRefLevel,
                                                        ViReal64 *highRefLevel,
                                                        ViReal64 *durationRefLevel);
ViStatus _VI_FUNC rsnrpz_scope_meas_setEquivalentSampling (ViSession instrumentHandle,
                                                           ViInt32 channel,
                                                           ViBoolean scopeMeasEquivSampling);
ViStatus _VI_FUNC rsnrpz_scope_meas_getEquivalentSampling (ViSession instrumentHandle,
                                                           ViInt32 channel,
                                                           ViBoolean *scopeMeasEquivSampling);
ViStatus _VI_FUNC rsnrpz_scope_meas_getSamplePeriod (ViSession instrumentHandle,
                                                     ViInt32 channel,
                                                     ViReal64 *samplePeriod);
ViStatus _VI_FUNC rsnrpz_trigger_configureInternal (ViSession instrumentHandle,
                                                    ViInt32 channel,
                                                    ViReal64 triggerLevel,
                                                    ViInt32 triggerSlope);
ViStatus _VI_FUNC rsnrpz_trigger_configureExternal (ViSession instrumentHandle,
                                                    ViInt32 channel,
                                                    ViReal64 triggerDelay);
ViStatus _VI_FUNC rsnrpz_trigger_immediate (ViSession instrumentHandle,
                                            ViInt32 channel);
ViStatus _VI_FUNC rsnrpz_trigger_setAutoDelayEnabled (ViSession instrumentHandle,
                                                      ViInt32 channel,
                                                      ViBoolean autoDelay);
ViStatus _VI_FUNC rsnrpz_trigger_getAutoDelayEnabled (ViSession instrumentHandle,
                                                      ViInt32 channel,
                                                      ViBoolean* autoDelay);
ViStatus _VI_FUNC rsnrpz_trigger_setAutoTriggerEnabled (ViSession instrumentHandle,
                                                        ViInt32 channel,
                                                        ViBoolean autoTrigger);
ViStatus _VI_FUNC rsnrpz_trigger_getAutoTriggerEnabled (ViSession instrumentHandle,
                                                        ViInt32 channel,
                                                        ViBoolean* autoTrigger);
ViStatus _VI_FUNC rsnrpz_trigger_setCount (ViSession instrumentHandle,
                                           ViInt32 channel, ViInt32 triggerCount);
ViStatus _VI_FUNC rsnrpz_trigger_getCount (ViSession instrumentHandle,
                                           ViInt32 channel, ViInt32* triggerCount);
ViStatus _VI_FUNC rsnrpz_trigger_setDelay (ViSession instrumentHandle,
                                           ViInt32 channel, ViReal64 triggerDelay);
ViStatus _VI_FUNC rsnrpz_trigger_getDelay (ViSession instrumentHandle,
                                           ViInt32 channel, ViReal64* triggerDelay);
ViStatus _VI_FUNC rsnrpz_trigger_setHoldoff (ViSession instrumentHandle,
                                             ViInt32 channel,
                                             ViReal64 triggerHoldoff);
ViStatus _VI_FUNC rsnrpz_trigger_getHoldoff (ViSession instrumentHandle,
                                             ViInt32 channel,
                                             ViReal64* triggerHoldoff);
ViStatus _VI_FUNC rsnrpz_trigger_setHysteresis (ViSession instrumentHandle,
                                                ViInt32 channel,
                                                ViReal64 triggerHysteresis);
ViStatus _VI_FUNC rsnrpz_trigger_getHysteresis (ViSession instrumentHandle,
                                                ViInt32 channel,
                                                ViReal64* triggerHysteresis);
ViStatus _VI_FUNC rsnrpz_trigger_setLevel (ViSession instrumentHandle,
                                           ViInt32 channel, ViReal64 triggerLevel);
ViStatus _VI_FUNC rsnrpz_trigger_getLevel (ViSession instrumentHandle,
                                           ViInt32 channel, ViReal64* triggerLevel);
ViStatus _VI_FUNC rsnrpz_trigger_setSlope (ViSession instrumentHandle,
                                           ViInt32 channel, ViInt32 triggerSlope);
ViStatus _VI_FUNC rsnrpz_trigger_getSlope (ViSession instrumentHandle,
                                           ViInt32 channel, ViInt32* triggerSlope);
ViStatus _VI_FUNC rsnrpz_trigger_setSource (ViSession instrumentHandle,
                                            ViInt32 channel, ViInt32 triggerSource);
ViStatus _VI_FUNC rsnrpz_trigger_getSource (ViSession instrumentHandle,
                                            ViInt32 channel,
                                            ViInt32* triggerSource);
ViStatus _VI_FUNC rsnrpz_trigger_setDropoutTime (ViSession instrumentHandle,
                                                 ViInt32 channel,
                                                 ViReal64 dropoutTime);
ViStatus _VI_FUNC rsnrpz_trigger_getDropoutTime (ViSession instrumentHandle,
                                                 ViInt32 channel,
                                                 ViReal64* dropoutTime);
ViStatus _VI_FUNC rsnrpz_trigger_setMasterState (ViSession instrumentHandle,
                                        ViInt32 channel, ViBoolean state);
ViStatus _VI_FUNC rsnrpz_trigger_getMasterState (ViSession instrumentHandle,
                                        ViInt32 channel, ViBoolean *state);
ViStatus _VI_FUNC rsnrpz_trigger_setSyncState (ViSession instrumentHandle,
                                      ViInt32 channel, ViBoolean state);
ViStatus _VI_FUNC rsnrpz_trigger_getSyncState (ViSession instrumentHandle,
                                      ViInt32 channel, ViBoolean *state);
ViStatus _VI_FUNC rsnrpz_chan_info (ViSession instrumentHandle, ViInt32 channel,
                                    ViString infoType, ViInt32 arraySize,
                                    ViChar _VI_FAR info[]);
ViStatus _VI_FUNC rsnrpz_chan_infoHeader (ViSession instrumentHandle,
                                          ViInt32 channel, ViInt32 parameterNumber,
                                          ViInt32 arraySize,
                                          ViChar _VI_FAR header[]);
ViStatus _VI_FUNC rsnrpz_chan_infosCount (ViSession instrumentHandle,
                                          ViInt32 channel, ViInt32* count);
ViStatus _VI_FUNC rsnrpz_system_setStatusUpdateTime (ViSession instrumentHandle,
                                            ViInt32 channel,
                                            ViReal64 statusUpdateTime);
ViStatus _VI_FUNC rsnrpz_system_getStatusUpdateTime (ViSession instrumentHandle,
                                            ViInt32 channel,
                                            ViReal64 *statusUpdateTime);
ViStatus _VI_FUNC rsnrpz_system_setResultUpdateTime (ViSession instrumentHandle,
                                            ViInt32 channel,
                                            ViReal64 resultUpdateTime);
ViStatus _VI_FUNC rsnrpz_system_getResultUpdateTime (ViSession instrumentHandle,
                                            ViInt32 channel,
                                            ViReal64 *resultUpdateTime);
ViStatus _VI_FUNC rsnrpz_calib_test (ViSession instrumentHandle, ViInt32 channel,
                                     ViReal64 *calibTest);
ViStatus _VI_FUNC rsnrpz_calib_getTestDeviation (ViSession instrumentHandle,
                                                 ViInt32 channel,
                                                 ViReal64 *testDeviation);
ViStatus _VI_FUNC rsnrpz_calib_getTestReference (ViSession instrumentHandle,
                                                 ViInt32 channel,
                                                 ViReal64 *testReference);
ViStatus _VI_FUNC rsnrpz_chan_abort (ViSession instrumentHandle, ViInt32 channel);
ViStatus _VI_FUNC rsnrpz_chan_initiate (ViSession instrumentHandle,
                                        ViInt32 channel);
ViStatus _VI_FUNC rsnrpz_chan_setInitContinuousEnabled (ViSession instrumentHandle,
                                                        ViInt32 channel,
                                                        ViBoolean continuousInitiate);
ViStatus _VI_FUNC rsnrpz_chan_getInitContinuousEnabled (ViSession instrumentHandle,
                                                        ViInt32 channel,
                                                        ViBoolean* continuousInitiate);
ViStatus _VI_FUNC rsnrpz_chan_reset (ViSession instrumentHandle, ViInt32 channel);
ViStatus _VI_FUNC rsnrpz_chan_setSamplingFrequency (ViSession instrumentHandle,
                                                    ViInt32 channel,
                                                    ViInt32 samplingFrequency);
ViStatus _VI_FUNC rsnrpz_chan_getSamplingFrequency (ViSession instrumentHandle,
                                                    ViInt32 channel,
                                                    ViInt32* samplingFrequency);
ViStatus _VI_FUNC rsnrpz_chan_zero (ViSession instrumentHandle, ViInt32 channel);
ViStatus _VI_FUNC rsnrpz_chan_isZeroComplete (ViSession instrumentHandle,
                                              ViInt32 channel,
                                              ViBoolean* zeroingComplete);
ViStatus _VI_FUNC rsnrpz_chan_isMeasurementComplete (ViSession instrumentHandle,
                                                     ViInt32 channel,
                                                     ViBoolean* measurementComplete);
ViStatus _VI_FUNC rsnrpz_chan_selfTest (ViSession instrumentHandle, ViInt32 channel,
                                        ViChar _VI_FAR result[]);
ViStatus _VI_FUNC rsnrpz_chan_setAuxiliary (ViSession instrumentHandle,
                                            ViInt32 channel,
                                            ViInt32 auxiliaryValue);
ViStatus _VI_FUNC rsnrpz_chan_getAuxiliary (ViSession instrumentHandle,
                                            ViInt32 channel,
                                            ViInt32* auxiliaryValue);
ViStatus _VI_FUNC rsnrpz_meass_readMeasurement (ViSession instrumentHandle,
                                                ViInt32 channel, ViInt32 timeout_ms,
                                                ViReal64* measurement);
ViStatus _VI_FUNC rsnrpz_meass_fetchMeasurement (ViSession instrumentHandle,
                                                 ViInt32 channel,
                                                 ViReal64* measurement);
ViStatus _VI_FUNC rsnrpz_meass_readBufferMeasurement (ViSession instrumentHandle,
                                                      ViInt32 channel,
                                                      ViInt32 maximumTime_ms,
                                                      ViInt32 bufferSize,
                                                      ViReal64 _VI_FAR measurementArray[],
                                                      ViInt32* readCount);
ViStatus _VI_FUNC rsnrpz_meass_fetchBufferMeasurement (ViSession instrumentHandle,
                                                       ViInt32 channel,
                                                       ViInt32 arraySize,
                                                       ViReal64 _VI_FAR measurementArray[],
                                                       ViInt32* readCount);
ViStatus _VI_FUNC rsnrpz_meass_sendSoftwareTrigger (ViSession instrumentHandle,
                                                    ViInt32 channel);
ViStatus _VI_FUNC rsnrpz_meass_readMeasurementAux (ViSession instrumentHandle,
                                                   ViInt32 channel,
                                                   ViInt32 timeout_ms,
                                                   ViReal64* measurement,
                                                   ViReal64* aux1, ViReal64* aux2);
ViStatus _VI_FUNC rsnrpz_meass_fetchMeasurementAux (ViSession instrumentHandle,
                                                    ViInt32 channel,
                                                    ViInt32 timeout_ms,
                                                    ViReal64* measurement,
                                                    ViReal64* aux1, ViReal64* aux2);
ViStatus _VI_FUNC rsnrpz_meass_readBufferMeasurementAux (ViSession instrumentHandle,
                                                         ViInt32 channel,
                                                         ViInt32 maximumTime_ms,
                                                         ViInt32 bufferSize,
                                                         ViReal64 _VI_FAR measurementArray[],
                                                         ViReal64 _VI_FAR aux1Array[],
                                                         ViReal64 _VI_FAR aux2Array[],
                                                         ViInt32* readCount);
ViStatus _VI_FUNC rsnrpz_meass_fetchBufferMeasurementAux (ViSession instrumentHandle,
                                                          ViInt32 channel,
                                                          ViInt32 maximumTime_ms,
                                                          ViInt32 bufferSize,
                                                          ViReal64 _VI_FAR measurementArray[],
                                                          ViReal64 _VI_FAR aux1Array[],
                                                          ViReal64 _VI_FAR aux2Array[],
                                                          ViInt32* readCount);
ViStatus _VI_FUNC rsnrpz_status_preset (ViSession instrumentHandle);
ViStatus _VI_FUNC rsnrpz_status_checkCondition (ViSession instrumentHandle,
                                                ViInt32 statusClass, ViUInt32 mask,
                                                ViBoolean* state);
ViStatus _VI_FUNC rsnrpz_status_catchEvent (ViSession instrumentHandle,
                                            ViInt32 statusClass, ViUInt32 mask,
                                            ViInt32 direction);
ViStatus _VI_FUNC rsnrpz_status_checkEvent (ViSession instrumentHandle,
                                            ViInt32 statusClass, ViUInt32 mask,
                                            ViUInt32 resetMask, ViBoolean* events);
ViStatus _VI_FUNC rsnrpz_status_enableEventNotification (ViSession instrumentHandle,
                                                         ViInt32 statusClass,
                                                         ViUInt32 mask);
ViStatus _VI_FUNC rsnrpz_status_disableEventNotification (ViSession instrumentHandle,
                                                          ViInt32 statusClass,
                                                          ViUInt32 mask);
ViStatus _VI_FUNC rsnrpz_status_registerWindowMessage (ViSession instrumentHandle,
                                                       ViUInt32 *windowHandle,
                                                       ViUInt32 messageID);
ViStatus _VI_FUNC rsnrpz_errorCheckState (ViSession instrumentHandle,
                                          ViBoolean stateChecking);
ViStatus _VI_FUNC rsnrpz_reset (ViSession instrumentHandle);
ViStatus _VI_FUNC rsnrpz_self_test (ViSession instrumentHandle,
                                    ViInt16* selfTestResult,
                                    ViChar _VI_FAR selfTestMessage[]);
ViStatus _VI_FUNC rsnrpz_error_query (ViSession instrumentHandle,
                                      ViInt32* errorCode,
                                      ViChar _VI_FAR errorMessage[]);
ViStatus _VI_FUNC rsnrpz_error_message (ViSession instrumentHandle,
                                        ViStatus statusCode,
                                        ViChar _VI_FAR message[]);
ViStatus _VI_FUNC rsnrpz_revision_query (ViSession instrumentHandle,
                                         ViChar _VI_FAR instrumentDriverRevision[],
                                         ViChar _VI_FAR firmwareRevision[]);
ViStatus _VI_FUNC rsnrpz_CloseSensor (ViSession instrumentHandle, ViInt32 channel);
ViStatus _VI_FUNC rsnrpz_close (ViSession instrumentHandle);

ViStatus _VI_FUNC rsnrpz_GetSensorCount( ViSession iDummyHandle, ViInt32 *piCount );
ViStatus _VI_FUNC rsnrpz_GetSensorInfo( 
                      ViSession      iDummyHandle,
                      ViInt32        iChannel,
                      ViChar _VI_FAR pszSensorName[],
                      ViChar _VI_FAR pszSensorType[],
                      ViChar _VI_FAR pszSensorSerial[] );

/*****************************************************************************/
/* Support of R&S USB Hub NRP-Z5                                             */
/*****************************************************************************/
ViStatus _VI_FUNC rsnrpz_GetDeviceStatusZ5( ViSession iDummyHandle, ViInt32 *piAvail );
ViStatus _VI_FUNC rsnrpz_GetDeviceInfoZ5(   ViSession         iDummyHandle,
                                            ViInt32           iPortIdx,
                                            ViChar _VI_FAR    pszSensorName[],
                                            ViChar _VI_FAR    pszSensorType[],
                                            ViChar _VI_FAR    pszSensorSerial[],
                                            ViBoolean _VI_FAR *pbConnected );
ViStatus _VI_FUNC rsnrpz_initZ5(            ViInt32     cPort,
                                            ViBoolean   IDQuery,
                                            ViBoolean   resetDevice,
                                            ViSession*  pInstrSession );



ViStatus _VI_FUNC rsnrpz_status_driverOpenState( ViBoolean* driverState );

ViStatus _VI_FUNC rsnrpz_service_getDetectorTemperature( ViSession vi, ViInt32 channel, ViReal64* pTemperature );

ViStatus _VI_FUNC rsnrpz_fw_version_check( ViSession      vi,
                                           ViInt32        iBufSize,
                                           ViChar _VI_FAR firmwareCurrent[],
                                           ViChar _VI_FAR firmwareRequiredMinimum[],
                                           ViBoolean     *pbFirmwareOkay );

#if defined(_OUTSIDEVEE_)
ViStatus _VI_FUNC rsnrpz_GetSessionControlHandle(       // internal use only
                       ViSession      iHandle,
                       ViInt32        iChannel,
                       ViSession    **pCtrlSession );

ViStatus _VI_FUNC rsnrpz_status_setDeviceChangedCallback( void (*pFn)(NRP_SESSION), NRP_USERARG lUserArgument );
#endif

#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

/*****************************************************************************/
/*=== END INCLUDE FILE ======================================================*/
/*****************************************************************************/

#endif
