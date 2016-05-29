#include <cvidef.h>

/*======= Master signal generator driver header - Version 03.11 ======*/
/* For mi2023, mi2024, mi3002, ri3271, ifr2023A, ifr2023B, ifr2025 */
/* Produced by
    IFR Ltd.
    Longacres House,
    Norton Green Road,
    Stevenage,
    Herts SG1 2BA
    Phone +44-438-742200
    Fax   +44-438-727601

    V2.02   12/03 Modified minimum frequency to 9KHz
                  New constants ifr2025_LOG and ifr2025_LIN
    
    V2.03   05/06/96 Tracks the driver version

    V2.04   05/08/96 Tracks the driver version

    V2.05   03/12/96 Tracks the driver version
    
    V2.06   03/07/98  Added new variants 2023A, 2023B & 2025 (IFR)
                      Includes different max freq limits and Sinad option

    V2.07   14/08/98  Tracks the driver version
    
    V2.08   17/09/98  Add delta level step to ifr2025_sinad and ifr2025_sinadSetupQuery functions
                      Add new function ifr2025_sinadReadyQuery to query new Sinad status bit
    
    V3.00   09/10/98  Tracks the driver version - Modify ifr2025_DRIVER_REVISION

    V3.01   24/02/99  Version change only following patch to distribution kit script                  

    V3.10   22/03/99  Version change only following correction to function panel

    V3.11   15/09/99  Version change only following changes to initialize code

    V3.20	01/05/03  DLL now supports Win2k
                      Removed ifr2025_installAppHandler ifr2025_uninstallAppHandler
                          ifr2025_serviceSRQ to allow DLL to linked to .fp
                          This fixes problem with use in LabView/HP Vee
/
/*=========================================================================*/

#ifndef ifr2025_INCLUDE 
#define ifr2025_INCLUDE

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

#include "vpptype.h"

ViStatus _VI_FUNC ifr2025_init (ViRsrc instrDesc, ViBoolean IDQuery, ViBoolean resetDevice,
                                ViSession *vi);
ViStatus _VI_FUNC ifr2025_rfFreq (ViSession vi, ViReal64 frequency);
ViStatus _VI_FUNC ifr2025_rfLevel (ViSession vi, ViReal64 RFLevel, ViInt16 RFUnit);
ViStatus _VI_FUNC ifr2025_rfPhase (ViSession vi, ViReal64 carrierPhase);
ViStatus _VI_FUNC ifr2025_rfType (ViSession vi, ViInt16 RFType, ViInt16 RFUnit);
ViStatus _VI_FUNC ifr2025_rfState (ViSession vi, ViInt16 RFState);
ViStatus _VI_FUNC ifr2025_rfLimit (ViSession vi, ViReal64 level, ViInt16 unit, ViInt16 state );
ViStatus _VI_FUNC ifr2025_rfOffset (ViSession vi, ViReal64 level, ViInt16 state );
ViStatus _VI_FUNC ifr2025_attenLockUnlock (ViSession vi, ViInt16 attenuator);
ViStatus _VI_FUNC ifr2025_modMode (ViSession vi, ViInt16 modMode, ViInt16 pulse);
ViStatus _VI_FUNC ifr2025_modControl (ViSession vi, ViBoolean globalMod, ViBoolean pulse);
ViStatus _VI_FUNC ifr2025_modPhase (ViSession vi, ViReal64 modPhase);
ViStatus _VI_FUNC ifr2025_modAM (ViSession vi, ViInt16 channel, ViReal64 depth,
                       ViReal64 modRate, ViInt16 waveform, ViInt16 source,
                       ViBoolean state);
ViStatus _VI_FUNC ifr2025_modFM (ViSession vi, ViInt16 channel, ViReal64 deviation,
                       ViReal64 modRate, ViInt16 waveform, ViInt16 source,
                       ViBoolean state);
ViStatus _VI_FUNC ifr2025_modPM (ViSession vi, ViInt16 channel, ViReal64 deviation,
                       ViReal64 modRate, ViInt16 waveform, ViInt16 source,
                       ViBoolean state);
ViStatus _VI_FUNC ifr2025_setFSK (ViSession vi, ViReal64 FM1Deviation, ViBoolean FM1State);
ViStatus _VI_FUNC ifr2025_dcfmNull (ViSession vi);
ViStatus _VI_FUNC ifr2025_memStore (ViSession vi, ViInt16 storeType, ViInt16 storeNumber);
ViStatus _VI_FUNC ifr2025_memRecall (ViSession vi, ViInt16 recallType, ViInt16 recallNumber);
ViStatus _VI_FUNC ifr2025_memErase (ViSession vi, ViInt16 eraseStores);
ViStatus _VI_FUNC ifr2025_memProtection (ViSession vi, ViBoolean state, ViInt16 start,
                               ViInt16 stop);
ViStatus _VI_FUNC ifr2025_memPowerUp (ViSession vi, ViInt16 mode, ViInt16 memoryNo);
ViStatus _VI_FUNC ifr2025_memDownload (ViSession vi, ViBoolean RFOutput);
ViStatus _VI_FUNC ifr2025_sweep (ViSession vi, ViReal64 startFreq, ViReal64 stopFreq,
                       ViReal64 stepFreq, ViReal64 timeperStep, ViReal64 logInc );
ViStatus _VI_FUNC ifr2025_sweepMode (ViSession vi, ViInt16 carrFreq, ViInt16 sweepMode,
                           ViInt16 triggermode, ViInt16 triggerSource, ViInt16 type );
ViStatus _VI_FUNC ifr2025_memSequence (ViSession vi, ViInt16 set, ViInt16 start, ViInt16 stop);
ViStatus _VI_FUNC ifr2025_memSelectSeq (ViSession vi, ViInt16 select, ViInt16 *start,
                              ViInt16 *stop);
ViStatus _VI_FUNC ifr2025_sinad (ViSession vi, int averages, ViInt16 filterWeighting,
                                ViInt16 mode, ViReal64 autoTarget,
                                ViReal64 deltaLevelStep);
ViStatus _VI_FUNC ifr2025_sweepRun (ViSession vi, ViInt16 sweepControl);
ViStatus _VI_FUNC ifr2025_memTrigger (ViSession vi, ViBoolean state);
ViStatus _VI_FUNC ifr2025_memStepSeq (ViSession vi, ViInt16 step, ViInt16 *storeNum);
ViStatus _VI_FUNC ifr2025_autoSinad (ViSession vi, ViInt16 state);
ViStatus _VI_FUNC ifr2025_reset (ViSession vi);
ViStatus _VI_FUNC ifr2025_self_test (ViSession vi, ViInt16 *result, ViChar message []);
ViStatus _VI_FUNC ifr2025_error_query (ViSession vi, ViInt32 *errorCode,
                             ViChar errorMessage []);
ViStatus _VI_FUNC ifr2025_error_message (ViSession vi, ViStatus errorCode,
                               ViChar errorMessage []);

ViStatus _VI_FUNC ifr2025_revision_query (ViSession vi, ViChar driverRevision [],
                                ViChar instrumentRevision []);
ViStatus _VI_FUNC ifr2025_setFSTD (ViSession vi, ViInt16 standard);
ViStatus _VI_FUNC ifr2025_resetElapsedTime (ViSession vi);
ViStatus _VI_FUNC ifr2025_rppReset (ViSession vi);
ViStatus _VI_FUNC ifr2025_carrierQuery (ViSession vi, ViReal64 *frequency, ViReal64 *RFLevel,
                              ViInt16 *RFUnit, ViInt16 *RFState, ViInt16 *RFType, ViInt16 *mode );
ViStatus _VI_FUNC ifr2025_sweepQuery (ViSession vi, ViReal64 *startFrequency,
                            ViReal64 *stopFrequency, ViReal64 *stepFrequency,
                            ViReal64 *stepTime, ViInt16 *mode, ViInt16 *trigger,
                            ViReal64 *logInc, ViInt16 *type );
ViStatus _VI_FUNC ifr2025_modulationQuery (ViSession vi, ViInt16 channel,
                                 ViReal64 *depthDeviation,
                                 ViReal64 *depthDevIncrement, ViReal64 *frequency,
                                 ViReal64 *frequencyIncrement, ViInt16 *source,
                                 ViInt16 *shape, ViInt16 *state);
ViStatus _VI_FUNC ifr2025_modeQuery (ViSession vi, ViInt16 *modulationMode, ViInt16 *pulse,
                           ViInt16 *globalModulation, ViInt16 *globalPulse);
ViStatus _VI_FUNC ifr2025_elapsedTimeQuery (ViSession vi, ViReal64 *elapsedTime);
ViStatus _VI_FUNC ifr2025_operatingTimeQuery (ViSession vi, ViReal64 *operatingHours);
ViStatus _VI_FUNC ifr2025_rppQuery (ViSession vi, ViInt16 *RPPTripped, ViInt16 *RPPCount);
ViStatus _VI_FUNC ifr2025_sinadSetupQuery (ViSession vi, ViPInt16 averages,
                                          ViPInt16 filterWeighting, ViPInt16 mode,
                                          ViPReal64 targetSinad,
                                          ViPReal64 deltaLevelStep);
ViStatus _VI_FUNC ifr2025_sinadReadyQuery (ViSession vi, ViPInt16 statusCode);
ViStatus _VI_FUNC ifr2025_sinadValueQuery (ViSession vi, ViPReal64 sinadValue);
ViStatus _VI_FUNC ifr2025_autoSinadQuery (ViSession vi, ViPInt16 statusCode);
ViStatus _VI_FUNC ifr2025_close (ViSession vi);


/*= GLOBAL CONSTANT DECLARATIONS ==========================================*/

#define ifr2025_DRIVER_REVISION "Version 3.20 01/05/2003"

#define MINONE ( 0 )
#define mi3002 (3002)
#define mi2023 (2023)
#define mi2024 (2024)
#define mi2030 (2030)
#define mi2031 (2031)
#define ri3271 (3271)
#define ifr2023A (20231)
#define ifr2023B (20232)
#define ifr2025 (2025)

#define NI_VXI               1
#define NI_GPIB              2
#define TRUE    1
#define FALSE   0

#define ifr2025_ON (VI_TRUE)
#define ifr2025_OFF (VI_FALSE)

/* ===================*/
/* INSTRUMENT LIMITS  */
/* ===================*/
#define ifr2025_FREQ_MIN ((ViReal64)8999.5)
#define mi3002_FREQ_MAX ((ViReal64)2400000000.5)
#define mi2023_FREQ_MAX ((ViReal64)1200000000.5)
#define ifr2023B_FREQ_MAX ((ViReal64)2050000000.5)
#define ifr2025_FREQ_MAX ((ViReal64)2510000000.5)

#define ifr2025_PHASE_MIN (0.0)
#define ifr2025_PHASE_MAX (999.9)

#define ifr2025_AMDEPTH_MIN (0.0)
#define ifr2025_AMDEPTH_MAX (99.9)

#define ifr2025_AMRATE_MIN (0.01)
#define ifr2025_AMRATE_MAX (20.0e3)

#define ifr2025_PMDEVIATION_MIN (0.0)
#define ifr2025_PMDEVIATION_MAX (10.0)

#define ifr2025_PMRATE_MIN (0.01)
#define ifr2025_PMRATE_MAX (20.0e3)

#define ifr2025_FMDEVIATION_MIN (0.0)
#define ifr2025_FMDEVIATION_MAX (100.0e3)

#define ifr2025_FMRATE_MIN (0.01)
#define ifr2025_FMRATE_MAX (20.0e3)

#define ifr2025_STEP_FREQ_MIN ((ViReal64)1.0)    /* Hz */
#define ifr2025_STEP_TIME_MIN ((ViReal64)20.0)   /* ms */
#define ifr2025_STEP_TIME_MAX ((ViReal64)10.0e3) /* ms */

#define ifr2025_RFOFFSET_MIN ((ViReal64)-5.0)
#define ifr2025_RFOFFSET_MAX ((ViReal64) 5.0)

/* =================================================================*/
/*  definitions below match the function tree prototype constructs  */
/* =================================================================*/
#define ifr2025_RFTYPE_EMF (0)
#define ifr2025_RFTYPE_PD  (1)

#define ifr2025_RFUNIT_DBM  (0)
#define ifr2025_RFUNIT_DBV  (1)
#define ifr2025_RFUNIT_DBMV (2)
#define ifr2025_RFUNIT_DBUV (3)
#define ifr2025_RFUNIT_V    (4)
#define ifr2025_RFUNIT_MV   (5)
#define ifr2025_RFUNIT_UV   (6)

#define ifr2025_PULSE_OFF (0)
#define ifr2025_PULSE_ON  (1)

#define ifr2025_MODE_AM   (0)
#define ifr2025_MODE_FM   (1)
#define ifr2025_MODE_PM   (2)
#define ifr2025_MODE_AMFM (3)
#define ifr2025_MODE_AMPM (4)
#define ifr2025_MODE_2FSK (5)
#define ifr2025_MODE_4FSK (6)
#define ifr2025_MODE_ILS  (7)
#define ifr2025_MODE_VOR  (8)

#define ifr2025_MOD1 (1)
#define ifr2025_MOD2 (2)

#define ifr2025_AM  (0)
#define ifr2025_AM1 (1)
#define ifr2025_AM2 (2)
#define ifr2025_FM  (3)
#define ifr2025_FM1 (4)
#define ifr2025_FM2 (5)
#define ifr2025_PM  (6)
#define ifr2025_PM1 (7)
#define ifr2025_PM2 (8)

#define ifr2025_DISABLE (0)
#define ifr2025_ENABLE  (1)

#define ifr2025_MOD_OFF (0)
#define ifr2025_MOD_ON  (1)

#define ifr2025_SRC_INT    (0)
#define ifr2025_SRC_EXTAC  (1)
#define ifr2025_SRC_EXTALC (2)
#define ifr2025_SRC_EXTDC  (3)

#define ifr2025_MODF_SIN (0)
#define ifr2025_MODF_TRI (1)
#define ifr2025_MODF_SQR (2)

#define ifr2025_MEM_CFRQ (0)
#define ifr2025_MEM_FULL (1)
#define ifr2025_MEM_RAM  (2)
#define ifr2025_MEM_ALL  (3)
#define ifr2025_MEM_UP   (4)
#define ifr2025_MEM_DOWN (5)

#define ifr2025_FACTORY (0)
#define ifr2025_MEMORY  (1)

#define ifr2025_SWEPT (0)
#define ifr2025_FIXED (1)

#define ifr2025_LIN (0)
#define ifr2025_LOG (1)

#define ifr2025_CONTINUOUS (0)
#define ifr2025_SINGLE     (1)

#define ifr2025_TRIG_OFF       (0)
#define ifr2025_TRIG_START     (1)
#define ifr2025_TRIG_STARTSTOP (2)
#define ifr2025_TRIG_STEP      (3)

#define ifr2025_TRIG_HOLD (0)
#define ifr2025_TRIG_TTL0 (1)
#define ifr2025_TRIG_TTL1 (2)
#define ifr2025_TRIG_TTL2 (3)
#define ifr2025_TRIG_TTL3 (4)
#define ifr2025_TRIG_TTL4 (5)
#define ifr2025_TRIG_TTL5 (6)
#define ifr2025_TRIG_TTL6 (7)
#define ifr2025_TRIG_TTL7 (8)
#define ifr2025_TRIG_BUS  (9)
#define ifr2025_TRIG_FRONT (10)

#define ifr2025_SWEEP_GO    (0)
#define ifr2025_SWEEP_HALT  (1)
#define ifr2025_SWEEP_CONT  (2)
#define ifr2025_SWEEP_RESET (3)
#define ifr2025_SWEEP_RETN  (4)
#define ifr2025_SWEEP_XFER  (5)
#define ifr2025_SWEEP_UP    (6)
#define ifr2025_SWEEP_DOWN  (7)

#define ifr2025_ATTEN_UNLOCK (0)
#define ifr2025_ATTEN_LOCK   (1)

#define ifr2025_FSTD_INT (0)
#define ifr2025_FSTD_EXT10DIR (1)
#define ifr2025_FSTD_EXT1IND (2)
#define ifr2025_FSTD_EXT10IND (3)
#define ifr2025_FSTD_INT10OUT (4)

#define ifr2025_SINAD_IDLE (0)
#define ifr2025_SINAD_ACTIVE (1)

#define ifr2025_SINAD_NONE (0)
#define ifr2025_SINAD_READY (1)

#define ifr2025_NONE (0)
#define ifr2025_CCITT (1)
#define ifr2025_CMESS (2)

#define ifr2025_DISABLE (0)
#define ifr2025_ENABLE (1)
#define ifr2025_AUTO (2)

#define ifr2025_AUTO_STOP (0)
#define ifr2025_AUTO_START (1)

/* ================================================================ */
/* ====================== ERROR DEFINITIONS ======================= */
/* VISA instrument defined errors are from 0xBFFC0800 -> 0xBFFC0FFF */
/*                          see VPP3.2 app A                        */
/* ================================================================ */
  #define ifr2025_ERROR_BASE (_VI_ERROR+0x3FFC0800L)
  #define ifr2025_ERR( e ) ((ViStatus) (ifr2025_ERROR_BASE + (e)) )

  #define ifr2025_ERROR_AM2_LIM_AM1     ifr2025_ERR(500)  /* example only */
  #define ifr2025_ERROR_RFUNITS         ifr2025_ERR(501)
  #define ifr2025_ERROR_RFLEVEL         ifr2025_ERR(502)
  #define ifr2025_ERROR_ONOFF           ifr2025_ERR(503)
  #define ifr2025_ERROR_RFTYPE          ifr2025_ERR(504)
  #define ifr2025_ERROR_RFFREQ          ifr2025_ERR(505)
  #define ifr2025_ERROR_PHASE           ifr2025_ERR(506)
  #define ifr2025_ERROR_MODMODE         ifr2025_ERR(507)
  #define ifr2025_ERROR_PULSE           ifr2025_ERR(508)
  #define ifr2025_ERROR_SOURCE          ifr2025_ERR(509)
  #define ifr2025_ERROR_RATE            ifr2025_ERR(510)
  #define ifr2025_ERROR_DEPTH           ifr2025_ERR(511)
  #define ifr2025_ERROR_WAVEFORM        ifr2025_ERR(512)
  #define ifr2025_ERROR_STATE           ifr2025_ERR(513)
  #define ifr2025_ERROR_MODCHAN         ifr2025_ERR(514)
  #define ifr2025_ERROR_STARTFREQ       ifr2025_ERR(515)
  #define ifr2025_ERROR_STOPFREQ        ifr2025_ERR(516)
  #define ifr2025_ERROR_STEPFREQ        ifr2025_ERR(517)
  #define ifr2025_ERROR_STEPTIME        ifr2025_ERR(518)

  #define ifr2025_ERROR_DDM_TYPE        ifr2025_ERR(519)
  #define ifr2025_ERROR_BEARING         ifr2025_ERR(520)

  #define ifr2025_ERROR_RFOFFSET        ifr2025_ERR(521)
  #define ifr2025_ERROR_LOGINC          ifr2025_ERR(522)

  #define ifr2025_ERROR_SINADAV         ifr2025_ERR(523)
  
  #define ifr2025_ERROR_INST_NOT_SUPP   ifr2025_ERR(800)

/* ================================================================ */

#define ifr2025_CHECK_ON_EXIT  (0x01)
#define ifr2025_CHECK_ON_SRQ   (0x02)
#define ifr2025_CHECK_ON_ENTRY (0x04)

/* ==================================================================== */
/*   Marconi GPIB - 2024 extensions                                     */
/* ==================================================================== */
#define ifr2025_DDM90   (90)
#define ifr2025_DDM150 (150)

#define ifr2025_TO   (1)
#define ifr2025_FROM (2)

#define ifr2025_DEPTH_SDM_MIN (0.0)
#define ifr2025_DEPTH_SDM_MAX (99.9)

#define ifr2025_DEPTH_DDM_MIN (0.0)
#define ifr2025_DEPTH_DDM_MAX (99.9)

#define ifr2025_DEPTH_ILS_MIN (0.0)
#define ifr2025_DEPTH_ILS_MAX (99.9)

#define ifr2025_DEPTH_VOR_MIN (0.0)
#define ifr2025_DEPTH_VOR_MAX (49.9)

#define ifr2025_DEPTH_REF_MIN (0.0)
#define ifr2025_DEPTH_REF_MAX (49.9)

#define ifr2025_BEARING_MIN (0.0)
#define ifr2025_BEARING_MAX (359.9)

#define ifr2025_DEPTH_VORID_MIN (0.0)
#define ifr2025_DEPTH_VORID_MAX (49.9)


ViStatus _VI_FUNC ifr2025_depthSDM( ViSession vi, ViReal64 depth );
ViStatus _VI_FUNC ifr2025_depthDDM( ViSession vi, ViInt16 type, ViReal64 depth );
ViStatus _VI_FUNC ifr2025_depthILSID( ViSession vi, ViReal64 depth );
ViStatus _VI_FUNC ifr2025_ilsQuery(  ViSession vi,
                                    ViReal64  *depthSDM,
                                    ViReal64  *depthDDM,
                                    ViReal64  *depthILSID,
                                    ViInt16   *ddmType );
ViStatus _VI_FUNC ifr2025_depthVOR( ViSession vi, ViReal64 depth );
ViStatus _VI_FUNC ifr2025_depthREF( ViSession vi, ViReal64 depth );
ViStatus _VI_FUNC ifr2025_bearingVOR( ViSession vi, ViInt16 direction, ViReal64 bearing );
ViStatus _VI_FUNC ifr2025_depthVORID( ViSession vi, ViReal64 depth );
ViStatus _VI_FUNC ifr2025_vorQuery(  ViSession vi,
                                    ViReal64  *depthVOR,
                                    ViReal64  *depthREF,
                                    ViReal64  *depthVORID,
                                    ViInt16   *direction,
                                    ViReal64  *bearing );



/* Used for "C" externs in C++ */
#if defined(__cplusplus) || defined(__cplusplus__)
}    /* end of "C" externs for C++ */
#endif 

#endif
