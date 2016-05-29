/*===========================================================================*/
/*                                                                           */
/* Rohde & Schwarz NRP-Z Instrument Driver                                   */
/*                                                                           */
/* Should you have any technical questions please contact the hotline of     */
/* Rohde & Schwarz Vertriebs-GmbH Rohde & Schwarz Support Center             */
/*                                                                           */
/* e-mail: customersupport@rohde-schwarz.com                                 */
/*                                                                           */
/* Modification History:                                                     */
/*                                                                           */
/*  Rev 2.3.1.1 (2.31), 22-JAN-2013 Juergen D. Geltinger                     */
/*  - fixed potential string problems with VEE                               */
/*  - fixed rsnrpz_initZ5()for VEE                                           */
/*  - removed unused code                                                    */
/*  - added recognition of NRPC series Power Sensors                         */
/*                                                                           */
/*  Rev 2.3.0.0 (2.30), 17-OCT-2012 Juergen D. Geltinger                     */
/*  - Added functions for supporting R&S USB Hub NRP-Z5                      */
/*    rsnrpz_GetDeviceStatusZ5()                                             */
/*    rsnrpz_GetDeviceInfoZ5()                                               */
/*    rsnrpz_initZ5()                                                        */
/*                                                                           */
/*  Rev 2.2.1.0 (2.21), 04-SEP-2012 Juergen D. Geltinger                     */
/*  - rsnrpz_meass_fetchBufferMeasurement() and                              */
/*    rsnrpz_meass_fetchBufferMeasurementAux() can be called now with        */
/*    'arraySize' and 'measurementArray' (and 'auxNArray') parameters        */
/*    set to 0 to get internal number of data-points in the expected         */
/*    result array(s)                                                        */
/*                                                                           */
/*  Rev 2.2.0.0 (2.20), 20-MAY-2012 T. Roeder                                */
/*  - fixed dead lock situation in rsnrpz_chan_isMeasurementComplete()       */
/*  - fixed handling of CAL:ZERO:AUTO in rsnrpz_WriteCommand()               */
/*                                                                           */
/*  Rev 2.1.9.0 (2.19), 26-MAR-2012 Juergen D. Geltinger                     */
/*  - added function rsnrpz_fw_version_check() to get current and            */
/*    required minimal version of sensor firmware                            */
/*  - changed signature of rsnrpz_init_long_distance() to keep               */
/*    MeasurementStudio happy                                                */
/*                                                                           */
/*  Rev 2.1.8.0 (2.18), 18-OCT-2011 T. Roeder                                */
/*  - fixed compatibility issues on 64-bit OS when VISA is included          */
/*  - rsnrpz_GetSensorInfo, channel index starts at 1 for first sensor       */
/*  - rsnrpz_chan_isMeasurementComplete checks if sensor is still alive      */
/*                                                                           */
/*  Rev 2.1.7.0 (2.17), 10-JUN-2011 Juergen D. Geltinger                     */
/*  - added function rsnrpz_corr_getSParamDevList()                          */
/*  - clearing errors in rsnrpz_defaultInstrSetup()                          */
/*  - clear session in case of errors during rsnrpz_init()                   */
/*                                                                           */
/*  Rev 2.1.6.0 (2.16), 08-JUN-2011 Juergen D. Geltinger                     */
/*  - exporting functions rsnrpz_status_setDeviceChangedCallback() and       */
/*    rsnrpz_GetSessionControlHandle() only when not running under VEE       */
/*                                                                           */
/*  Rev 2.1.5.0 (2.15), 17-MAY-2011 Juergen D. Geltinger                     */
/*  - fix situation where rsnrpz_close() will be called by a user            */
/*    with a session which was already closed before                         */
/*    (now returning VI_ERROR_PARAMETER1 in this case)                       */
/*                                                                           */
/*  Rev 2.1.4.0 (2.14), 20-APR-2011 Juergen D. Geltinger                     */
/*  - better compatibility with CVI below V9.0                               */
/*  - fixed CVI linkage warning                                              */
/*                                                                           */
/*  Rev 2.1.3.0 (2.13), 08-MAR-2011 Juergen D. Geltinger                     */
/*  - fixed LFR & UFR parameter of rsnrpz_chan_zeroAdvanced()                */
/*                                                                           */
/*  Rev 2.1.2.0 (2.12), 26-JAN-2011 T. Roeder                                */
/*  - fixed problem when unplugging sensor during measurement                */
/*  - cleaned up                                                             */
/*  - bugfix in rsnrpz_NotifyCommand where LeaveCriticalSection may have     */
/*    been called without EnterCriticalSection                               */
/*  - improved trace functionality                                           */
/*                                                                           */
/*  Rev 2.1.1.0 (2.11), 06-DEC-2010 Juergen D. Geltinger                     */
/*  - removed unused/old code fragments left over from 2.10 change           */
/*                                                                           */
/*  Rev 2.1.0.0 (2.10), 03-DEC-2010 Juergen D. Geltinger                     */
/*  - 'external' interface information (ViSession) always                    */
/*    is a 32-bit entity now, even with the 64-bit version                   */
/*    of the driver                                                          */
/*  - Fixed define of PEAK algorithm in trace measurement                    */
/*                                                                           */
/*  Rev 2.0.0.0 (2.00), 04-NOV-2010 Juergen D. Geltinger                     */
/*  - first official 32/64 bit release                                       */
/*                                                                           */
/*  Rev 01.96, 29-JUN-2010 T. Roeder, J. Geltinger                           */
/*  - modifications for compiling as 64 bit driver                           */
/*                                                                           */
/*  Rev 01.95, 16-APR-2010 T. Roeder                                         */
/*  - added NrpDeviceChangedCallback                                         */
/*  - added rsnrpz_status_setDeviceChangedCallback                           */
/*  - changed some defines to constants                                      */
/*                                                                           */
/*  Rev 01.94, 06-APR-2010 T. Roeder                                         */
/*  - using #if defined(__linux__) instead of LINUX                          */
/*  - added support for __APPLE__                                            */
/*  - exported rsnrpz_service_getDetectorTemperature                         */
/*                                                                           */
/*  Rev 01.93, 15-MAR-2010 Juergen D. Geltinger                              */
/*  - Fixed determination of PulseHighestParamNo for support of              */
/*    varying firmware versions of NRP-Z81 type sensors                      */
/*    (currently 25 or 26 parameters possible; this gets                     */
/*     dynamically evaluated now)                                            */
/*                                                                           */
/*  Rev 01.92, 4-MAR-2010 Martin Krcmar                                      */
/*  - New functions:                                                         */
/*      rsnrpz_chan_setCorrectionFrequencyStep                               */
/*      rsnrpz_chan_getCorrectionFrequencyStep                               */
/*      rsnrpz_chan_setCorrectionFrequencySpacing                            */
/*      rsnrpz_chan_getCorrectionFrequencySpacing                            */
/*      rsnrpz_chan_getContAvBufferCount                                     */
/*      rsnrpz_chan_getContAvBufferInfo                                      */
/*      rsnrpz_scope_setEquivalentSampling                                   */
/*      rsnrpz_scope_getEquivalentSampling                                   */
/*      rsnrpz_scope_meas_setTime                                            */
/*      rsnrpz_scope_meas_getTime                                            */
/*      rsnrpz_scope_meas_setEquivalentSampling                              */
/*      rsnrpz_scope_meas_getEquivalentSampling                              */
/*      rsnrpz_scope_meas_getSamplePeriod                                    */
/*      rsnrpz_calib_test                                                    */
/*      rsnrpz_calib_getTestDeviation                                        */
/*      rsnrpz_calib_getTestReference                                        */
/*  - Modified functions:                                                    */
/*      rsnrpz_SetBitInRegisters                                             */
/*      rsnrpz_SetRegister                                                   */
/*                                                                           */
/*  Rev 01.91, 01-MAR-2010 Juergen D. Geltinger                              */
/*  - added rsnrpz_chan_getCacheRange()                                      */
/*    (for internal use by PowerViewerPlus)                                  */
/*  - bugfix in rsnrpz_FormatCommand() (C locale)                            */
/*                                                                           */
/*  Rev 01.90, 09-SEP-2009 Juergen D. Geltinger                              */
/*  - changed recognition of supported commands (not using sensor type       */
/*    anymore, but feature-bits from sensor). "Old" behaviour still          */
/*    available by defining USE_CHECKINSTR                                   */
/*                                                                           */
/*  Rev 01.80, 09-DEC-2008 Juergen D. Geltinger                              */
/*  - fixed handling of automatic pulse parameter measurement results        */
/*    in conjunction with isMeasurementComplete()                            */
/*                                                                           */
/*  Rev 01.79, 11-NOV-2008 Juergen D. Geltinger                              */
/*  - dropout time handling no longer limited to NRP-Z81 only (relies on     */
/*    sensor features now)                                                   */
/*                                                                           */
/*  Rev 01.78, 04-NOV-2008 Juergen D. Geltinger                              */
/*  - revision query no longer needs an open session                         */
/*                                                                           */
/*  Rev 01.77, 09-SEP-2008 Juergen D. Geltinger                              */
/*  - suppressed SYST:INIT in initialization because of side effect of       */
/*    resetting zero cal values etc.                                         */
/*  - fix for identification of special sensor version NRP-Z311              */
/*                                                                           */
/*  Rev 01.76, 07-JUL-2008 Juergen D. Geltinger                              */
/*  - added missing (char*) cast for clean LINUX compilation                 */
/*                                                                           */
/*  Rev 01.75, 20-JUN-2008 Juergen D. Geltinger                              */
/*  - added support for command 'SYSTEM:INFO? "Uptime"'                      */
/*  - fixed rsnrpz_service_getDetectorTemperature()                          */
/*  - fixed rsnrpz_chan_info() with empty item                               */
/*                                                                           */
/*  Rev 01.74, 09-JUN-2008 Jiri Kominek/Thomas Roeder/JDG                    */
/*  - Added Pulse Analysis scope measurement (NRP-Z81 only)                  */
/*  - changed sequence of error handling in ReadValue()                      */
/*                                                                           */
/*  Rev 01.73, 21-MAY-2008 Thomas Roeder                                     */
/*  - fixed memory leak in NrpDataAvailableCallback for aux channels         */
/*  - added (char*) type casts to get rid of gcc -Wwrite-string warnings:    */
/*    'deprecated conversion from string constant...'                        */
/*                                                                           */
/*  Rev 01.72, 16-MAY-2008 Juergen D. Geltinger                              */
/*  - added error check for correctly sent command in WriteCommand           */
/*    for not setting up a wait event in case of an error                    */
/*  - fixed evaluation of registry key for initialization                    */
/*                                                                           */
/*  Rev 01.71, 26-MAR-2008 Thomas Roeder                                     */
/*  - thread-safe initialization                                             */
/*                                                                           */
/*  Rev 01.70, 13-MAR-2008 Juergen D. Geltinger                              */
/*  - fixed deadlock situation in error handling                             */
/*                                                                           */
/*  Rev 01.69, 04-MAR-2008 Juergen D. Geltinger                              */
/*  - fixed compilation under LINUX (stricmp() was missing)                  */
/*                                                                           */
/*  Rev 01.68, 22-FEB-2008 Juergen D. Geltinger                              */
/*  - cleared LabWindows/CVI compiler/linker warnings                        */
/*                                                                           */
/*  Rev 01.67, 08-FEB-2008 Juergen D. Geltinger                              */
/*  - fixed compilation problem with #included "visa.h"                      */
/*                                                                           */
/*  Rev 01.66, 15-JAN-2008 Juergen D. Geltinger                              */
/*  - integrated latest version of NrpControl2.dll                           */
/*                                                                           */
/*  Rev 01.65, 09-JAN-2008 Juergen D. Geltinger                              */
/*  - added SYST:INFO item 'FPGA Build', and made item check                 */
/*    case insensitive                                                       */
/*                                                                           */
/*  Rev 01.64, 28-NOV-2007 Juergen D. Geltinger                              */
/*  - added conditional defines for LINUX compilation                        */
/*      HINT:  There is currently no notification mechanism                  */
/*             for service requests (SRQ) under LINUX !                      */
/*  - added rsnrpz_GetSensorCount() and rsnrpz_GetSensorInfo()               */
/*  - fixed rsnrpz_chan[s]_isMeasurementComplete checks in Statistics modes  */
/*                                                                           */
/*  Rev 01.63, 10-AUG-2007 Juergen D. Geltinger                              */
/*  - added rsnrpz_GetAvailableSensors                                       */
/*                                                                           */
/*  Rev 01.62, 19-JUL-2007 Juergen D. Geltinger                              */
/*  - fixed usage of the same event-handles for both read and write          */
/*    operations due to Rev 1.60 changes (rsnrpz_meass_readMeasurement()     */
/*    did not work any more)                                                 */
/*                                                                           */
/*  Rev 01.61, 15-JUN-2007 Juergen D. Geltinger                              */
/*  - fixed CriticalSection issue                                            */
/*                                                                           */
/*  Rev 01.60, 06-JUN-2007 Juergen D. Geltinger                              */
/*  - fixed usage of more than one session at a time. In that case           */
/*      the rsnrpz_close() unloaded/closed the NrpControl driver as          */
/*      soon as the first session was closed. This was wrong!                */
/*      NrpCloseDriver() is only allowed to be called after the              */
/*      LAST session was closed!                                             */
/*                                                                           */
/*  Rev 01.51, 11-MAY-2007 Jiri Kominek                                      */
/*  - New functions:                                                         */
/*      rsnrpz_trigger_setMasterState                                        */
/*      rsnrpz_trigger_getMasterState                                        */
/*      rsnrpz_trigger_setSyncState                                          */
/*      rsnrpz_trigger_getSyncState                                          */
/*                                                                           */
/*  Rev 01.50, 14-FEB-2007 Juergen D. Geltinger                              */
/*  - enhanced version of rsnrpz_init(); now queries the registry            */
/*      to find out whether a long distance connection should be             */
/*      established (for 'AnywhereUSB' remote hub)                           */
/*  - New functions:                                                         */
/*      rsnrpz_init_long_distance()                                          */
/*        to support connection of sensors via 'AnywhereUSB' remote hub      */
/*                                                                           */
/*  Rev 01.40, 11-OCT-2006 Juergen D. Geltinger                              */
/*  - New functions:                                                         */
/*      rsnrpz_get_timeout                                                   */
/*      rsnrpz_set_timeout                                                   */
/*        Fixes a timeout-terminated hang in USB Write Functions             */
/*        on Windows XP Embedded                                             */
/*                                                                           */
/*  Rev 01.31, 08/2006 by J. Kominek                                         */
/*  - New functions:                                                         */
/*      rsnrpz_bandwidth_setBw                                               */
/*      rsnrpz_bandwidth_getBw                                               */
/*      rsnrpz_bandwidth_getBwList                                           */
/*      rsnrpz_chan_setBurstChopperEnabled                                   */
/*      rsnrpz_chan_getBurstChopperEnabled                                   */
/*      Time-gates subsystem                                                 */
/*      rsnrpz_tslot_setTimeSlotMidOffset                                    */
/*      rsnrpz_tslot_getTimeSlotMidOffset                                    */
/*      rsnrpz_tslot_setTimeSlotMidLength                                    */
/*      rsnrpz_tslot_getTimeSlotMidLength                                    */
/*      rsnrpz_tslot_setTimeSlotChopperEnabled                               */
/*      rsnrpz_tslot_getTimeSlotChopperEnabled                               */
/*      rsnrpz_trigger_setDropoutTime                                        */
/*      rsnrpz_trigger_getDropoutTime                                        */
/*      rsnrpz_chan_setAuxiliary                                             */
/*      rsnrpz_chan_getAuxiliary                                             */
/*      rsnrpz_meass_readMeasurementAux                                      */
/*      rsnrpz_meass_fetchMeasurementAux                                     */
/*      rsnrpz_meass_readBufferMeasurementAux                                */
/*      rsnrpz_meass_fetchBufferMeasurementAux                               */
/*      rsnrpz_stat_confTimegate                                             */
/*      rsnrpz_stat_confScale                                                */
/*      rsnrpz_stat_setOffsetTime                                            */
/*      rsnrpz_stat_getOffsetTime                                            */
/*      rsnrpz_stat_setMidOffset                                             */
/*      rsnrpz_stat_getMidOffset                                             */
/*      rsnrpz_stat_setMidLength                                             */
/*      rsnrpz_stat_getMidLength                                             */
/*      rsnrpz_stat_setScaleRefLevel                                         */
/*      rsnrpz_stat_getScaleRefLevel                                         */
/*      rsnrpz_stat_setScaleRange                                            */
/*      rsnrpz_stat_getScaleRange                                            */
/*      rsnrpz_stat_setScalePoints                                           */
/*      rsnrpz_stat_getScalePoints                                           */
/*      rsnrpz_stat_getScaleWidth                                            */
/*      rsnrpz_scope_fastZero                                                */
/*      rsnrpz_corr_setSParamDevice                                          */
/*      rsnrpz_corr_getSParamDevice                                          */
/*      rsnrpz_chan_setReflectionGammaUncertainty                            */
/*      rsnrpz_chan_getReflectionGammaUncertainty                            */
/*      rsnrpz_chan_zeroAdvanced                                             */
/*                                                                           */
/*  Rev 01.30, 11/2005 by J. Kominek                                         */
/*  - FP Help fixed - command Index Added into CHM                           */
/*  - Added support of NRP-Z37, 27, 81                                       */
/*  - Modified functions:                                                    */
/*      rsnrpz_chan_mode                                                     */
/*  - New functions:                                                         */
/*      rsnrpz_chan_configureReflectGammaCorr                                */
/*      rsnrpz_chan_setReflectionGammaMagn                                   */
/*      rsnrpz_chan_getReflectionGammaMagn                                   */
/*      rsnrpz_chan_setReflectionGammaPhase                                  */
/*      rsnrpz_chan_getReflectionGammaPhase                                  */
/*                                                                           */
/*  Rev 01.22, 04/2005 by J. Kominek                                         */
/*  - FP Help fixed                                                          */
/*                                                                           */
/*  Rev 01.21, 04/2005 by K.P.                                               */
/*  - Fixed rsnrpz_CloseSensor - forces freeing of user data                 */
/*                                                                           */
/*  Rev 01.20, 11/2004 by Z.R.                                               */
/*  - Fixed NrpControl2.dll                                                  */
/*  - Fixed rsnrpz_AddSensor                                                 */
/*                                                                           */
/*  Rev 01.19, 08/2004 by Z.R.                                               */
/*  - Fixed NrpControl2.dll                                                  */
/*                                                                           */
/*  Rev 01.18, 03/2004 by Z.R.                                               */
/*  - Fixed NrpControl2.dll                                                  */
/*                                                                           */
/*  Rev 01.17, 12/2003 by Z.R.                                               */
/*  - Removed debug lines with MessageBeep function (now it's 10x faster)    */
/*                                                                           */
/*  Rev 01.16, 10/2003 by Z.R., J.K.                                         */
/*  - Added new NrpControl2.dll (revision 01.21)                             */
/*  - Float parameters are formated with %e instead of %f                    */
/*  - Minor changes in FP file                                               */
/*  - Fixed rsnrpz_status_checkEvent function                                */
/*  - Fixed rsnrpz_chan_infosCount                                           */
/*                                                                           */
/*  Rev 01.15, 09/2003 by Z.R.                                               */
/*  - Added new NrpControl2.dll (revision 01.21)                             */
/*  - Minor changes in FP file                                               */
/*                                                                           */
/*  Rev 01.14, 08/2003 by Z.R.                                               */
/*  - Added new NrpControl2.dll (revision 01.20)                             */
/*                                                                           */
/*  Rev 01.13, 08/2003 by Z.R.                                               */
/*  - Added VISA constants and types into header file. VISA is not required. */
/*                                                                           */
/*  Rev 01.12, 07/2003 by Z.R.                                               */
/*  - fixed bug in the error callback                                        */
/*                                                                           */
/*  Rev 01.11, 07/2003 by Z.R.                                               */
/*  - ignored Sample Error in release version.                               */
/*  - used macro constant for device errors instead of magic numbers         */
/*  - isMeasComplete functions don't reflect measurement bits and they return*/
/*    TRUE when data are available. When functions return TRUE and new data  */
/*    has not been received, all other isMeasurementComplete function calls  */
/*    return FALSE.                                                          */
/*                                                                           */
/*  Rev 01.10, 07/2003 by Z.R.                                               */
/*  - Fixed  rsnrpz_chans_isMeasurementComplete and                          */
/*    rsnrpz_chan_isMeasurementComplete. Functions do not return VI_TRUE when*/
/*    sensor is in wait for trigger state                                    */
/*  - Fixed functions rsnrpz_chans_zero and rsnrpz_chan_zero                 */
/*    These function do not wait for acknowledge.                            */
/*  - Added better checking for invalid session handle                       */
/*  - Fixed functions rsnrpz_avg_setAutoEnabled, rsnrpz_avg_getAutoEnabled   */
/*    rsnrpz_scope_setAutoEnabled, rsnrpz_scope_getAutoEnabled.              */
/*  - If direction is RSNRPZ_DIRECTION_PTR, and is then set to               */
/*    RSNRPZ_DIRECTION_NTR, the PTR is removed                               */
/*  - rsnrpz_close and rsnrpz_CloseSensor functions send *RST .              */
/*                                                                           */
/*  Rev 01.01, 06/2003 by Z.R.                                               */
/*  - Cosmetic changes in the help                                           */
/*  - Fixed rsnrpz_error_message function (translate NrpConrol2 errors)      */
/*  - Fetch function returns an error when data are not available            */
/*  - Fixed functions rsnrpz_chans_isZeroingComplete and                     */
/*    rsnrpz_chans_isMeasurementComplete                                     */
/*                                                                           */
/*  Rev 01.00, 05/2003 by Z.R.                                               */
/*  - Driver created                                                         */
/*                                                                           */
/*===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include "rsnrpz.h"

#if defined(__linux__) || defined(__APPLE__)
  #include <unistd.h>
  #include <dirent.h>
  #include "winEmul.h"
  #define  _NRP_FUNC
  #define  MAX_PATH  MAXNAMLEN
#else
  #include <windows.h>
#endif

#if defined(__APPLE__)
  #include <mach/mach.h>
  #include <mach/mach_time.h>
#endif

#if defined(__linux__)
  #include <sys/sysinfo.h>
  #include <stdarg.h>
#endif

/*= CONSTANTS ======================================================================*/

static const int ENABLE_trace_log               = 0;                        /* Enable tracing                             */

static const char* const RSNRPZ_REVISION        = "Rev 02.31, 01/2013";     /* Instrument driver revision                 */

#define BUFFER_SIZE                             1024L                       /* File I/O buffer size                       */
#define RSNRPZ_MAX_CHANNELS                     32                          /* Maximum number of channels                 */
#define RSNRPZ_MAX_MAIN_SESSIONS                32                          /* Maximum number of main sessions            */

static const int IEEE_ERROR_BITS                = 0x3C;                     /* AND mask for ESR checking                                    */
static const int ALL_CHANNELS                   = -1;                       /* Used for functions,  which set all sensors                   */
static const int NRP_SIZE_OF_REGISTER           = 1;                        /* size of register. 1 = 32 bits, 2 = 64 bits 3 = 96 bits etc.  */

#define MAX_REGISTER_EVENTS                     128
#define MAX_ERROR_QUEUE_SIZE                    16
#define MAX_LENGTH_OF_ERROR_MESSGAE             256

static const char* const RSNRPZ_IDN_QUERY       = "ROHDE&SCHWARZ,NRP-Z";
static const char* const RSNRPZ_IDN_QUERY_311   = "ROHDE&SCHWARZ,NRPZ311";
static const char* const RSFSH_IDN_QUERY        = "ROHDE&SCHWARZ,FSH-Z";
static const char* const RSNRPC_IDN_QUERY       = "ROHDE&SCHWARZ,NRPC";

static const char* const SUBKEY                 = "Software\\Rohde&Schwarz\\NrpDll";
static const char* const LONGDISTKEY            = "LongDistanceConnection";

static const char* const trace_log_FILE         = "rsnrpz.log";

/*= Macros ===========================================================*/

/* Used in range table table definition (last entry) */
#define LAST_RANGE_TABLE_ENTRY          NULL, 0, 0

/* general error check */
#define CHECKERR(fCal) \
  if (rsnrpz_status = (fCal), rsnrpz_status < VI_SUCCESS) \
    return rsnrpz_status;

/* error check (only used in init function) */
#define CHECKERR_CU(fCal) \
  { if (rsnrpz_status = (fCal), rsnrpz_status < VI_SUCCESS) \
      goto Error; }

ViBoolean CheckSession( ViSession vi );
ViBoolean CheckChannel( ViSession vi, int channel );

#define CHECKSESSION(vi)              \
  if( !CheckSession(vi) )             \
    return VI_ERROR_PARAMETER1;

#define CHECKCHANNEL(vi, channel)     \
  if( !CheckChannel( vi, channel ) )  \
    return VI_ERROR_PARAMETER2;

#define CHECKGATE(vi, gate)           \
  if( gate < 1 || gate > 16 )         \
    return VI_ERROR_PARAMETER3

/* constants for registers (ESR register bits) */
static const int NRP_ESR_REGISTER_QUERY_ERROR           = 2; /* (1L << 2) */
static const int NRP_ESR_REGISTER_DEV_DEPENDENT_ERROR   = 3; /* (1L << 3) */
static const int NRP_ESR_REGISTER_EXECUTION_ERROR       = 4; /* (1L << 4) */
static const int NRP_ESR_REGISTER_COMMAND_ERROR         = 5; /* (1L << 5) */

/* trace functionality */
static FILE* gs_fTrace = NULL;

void trace_log( const char* msg, ... )
{
  va_list ap;

  if( ENABLE_trace_log!=1 )
    return;

  if( gs_fTrace == 0 )
    gs_fTrace = fopen( trace_log_FILE, "w" );

  if( gs_fTrace == 0 )
    return;

#if defined(__APPLE__)
  // from mach/mach_time.h
  fprintf( gs_fTrace, "%ld\t: ", (long int)mach_absolute_time() );
#else
  #if defined(__linux__)
    // from sys/sysinfo.h
    struct sysinfo si;
    if( sysinfo(&si) == 0 )
      fprintf( gs_fTrace, "%ld\t: ", (long int)si.uptime );
  #else
      fprintf( gs_fTrace, "%ld\t: ", GetTickCount() );
  #endif
#endif

  va_start( ap, msg );
  vfprintf( gs_fTrace, msg, ap );
  va_end( ap );

  fprintf( gs_fTrace, "\n" );
  fflush( gs_fTrace );

  // alternative: OutputDebugStringA( msg );
}

/*****************************************************************************/
/*= Typedefs for string value tables. =======================================*/
/*****************************************************************************/
/* rsnrpz_stringValPair is used in the rsnrpz_error_message function         */
/* rsnrpz_statusDataRanges is used to track session dependent status & ranges*/
/*===========================================================================*/
typedef enum tag_ERegistryTypes
{
  CONDITION_REGISTER = 0,
  PTRANSITION_REGISTER,
  NTRANSITION_REGISTER,
  EVENT_REGISTER,
  ENABLE_REGISTER,
  NB_OF_REGISTER_TYPES
} ERegistryTypes;

typedef enum tag_ENrpRegistrs
{
  DEVICE_STATUS_REGISTER_CONNECTED = 0,
  DEVICE_STATUS_REGISTER_ERROR,
  QUESTIONABLE_STATUS_REGISTER,
  STANDARD_EVENT_STATUS_REGISTER,
  STATUS_BYTE_REGISTER,
  OPERATION_STATUS_REGISTER,
  OPERATION_CALIBRATING_STATUS_REGISTER,
  OPERATION_MEASURING_STATUS_REGISTER,
  OPERATION_TRIGGER_STATUS_REGISTER,
  OPERATION_SENSE_STATUS_REGISTER,
  OPERATION_LOWER_LIMIT_FAIL_STATUS_REGISTER,
  OPERATION_UPPER_LIMIT_FAIL_STATUS_REGISTER,
  QUESTIONABLE_POWER_STATUS_REGISTER,
  QUESTIONABLE_WINDOW_STATUS_REGISTER,
  QUESTIONABLE_CALIBRATING_STATUS_REGISTER,
  QUESTIONABLE_ZERO_STATUS_REGISTER,
  NB_OF_NRP_REGISTERS
} ENrpRegistrs;


/**
 * Specifies element in error queue. every error has error
 * code and associated error message.
 * Error queue is part of main session and size is defined by
 * MAX_ERROR_QUEUE_SIZE. If error queue is full and new error
 * has been occured, the oldes error is removed.
 */
typedef struct
{
  ViInt32     ErrorCode;
  ViChar      ErrorMessage[MAX_LENGTH_OF_ERROR_MESSGAE];
} SErrorEntry;

/**
 * Defines element with value, which has associated string
 */
typedef struct  rsnrpz_stringValPair
{
   ViStatus         stringVal;
   const char*      stringName;
}  rsnrpz_tStringValPair;

/**
 * Defines register type. In our case is register 32 bit value.
 * if you want support bigger register change size of array.
 */
typedef ViUInt32    TRegister[1];

typedef TRegister   TNrpRegister[NB_OF_REGISTER_TYPES];

/**
 * Defines element of discrete range table
 * - parameter: string value
 * - bit: instrument specific value associated to string parameter
 * - value: instrument driver specific value associated to string parameter
 */
typedef struct
{
  const char*   parameter;
  ViInt32       bit;
  ViInt32       value;
} SRangeTableEntry;


/**
 * Defines element of discrete range table
 * - parameter: string value
 * - bit: instrument specific value associated to string parameter
 * - value: instrument driver specific value associated to string parameter
 */
typedef struct
{
  char*         parameter;
  ViInt32       bit;
  ViInt32       value;
} SpecRangeTableEntry;

/**
 * Defines element of command table.
 * - command: sensor command
 * - groupNb: group number associated to instrument command
 * - paramNb: parameter number associated to instrument command
 * - RangeTable: pointer to discrete range table, associated to instrument command
 */
typedef struct
{
    const char*         command;
    ViInt16             groupNb;
    ViInt16             paramNb;
    SRangeTableEntry*   RangeTable;
} SCommandEntry;

typedef SCommandEntry NrpCommandTable[];

typedef union
{
    ViInt32 lvalue;
    float   fvalue;
    char*   pstring;
} UValue;


typedef struct
{
    NRP_SESSION     session;
    ViInt16         groupNb;
    ViInt16         paramNb;
    HANDLE          event;
} SEventEntry;

/**
 * defines element in the cache table
 */
typedef struct
{
    UValue  current;
    UValue  minimum;
    UValue  maximum;
    short   type;
    short   IsAvailable;
} SValueEntry;

typedef SValueEntry NrpValueTable[];

/**
 * Defines the data structure which is associated to a driver session (main session)
 * Each main session may be used with RSNRPZ_MAX_CHANNELS channels. These sensors
 * use individual session numbers.
 */
struct tag_SMainSession
{
    CRITICAL_SECTION            cs;
    HANDLE                        event;
#if ! defined(__linux__) && ! defined(__APPLE__)
    HWND                        hMessageWindow;
#endif
    UINT                        MessageID;
    NRP_SESSION                 pSessions[RSNRPZ_MAX_CHANNELS];        /* session numbers used with the channels        */
    struct tag_SSessionInfo*    pSInfos[RSNRPZ_MAX_CHANNELS];            /* info structure for each channel                */
    TNrpRegister                NrpRegisters[NB_OF_NRP_REGISTERS];
    SEventEntry                 EventTable[MAX_REGISTER_EVENTS];
    ViBoolean                   ErrorChecking;
    SErrorEntry                 NrpErrors[MAX_ERROR_QUEUE_SIZE];
    HANDLE                      hHandlesRd[RSNRPZ_MAX_CHANNELS];
    HANDLE                      hHandlesWr[RSNRPZ_MAX_CHANNELS];
};

/**
 * Defines data structure which is associated for every sensor session (channels)
 */
struct tag_SSessionInfo
{
    struct tag_SMainSession *MainSession;
    CRITICAL_SECTION        cs;
    SValueEntry*            CacheTable;
    ViBoolean               IsSingleMeasValid;
    ViBoolean               IsArrayMeasValid;
    ViBoolean               IsArrayMeasAuxValid;
    ViBoolean               IsMeasurementComplete;
    ViReal64                SingleMeas;
    ViReal64                SingleMeasAux1;
    ViReal64                SingleMeasAux2;
    ViReal64*               ArrayMeas;
    ViReal64*               ArrayMeasAux1;
    ViReal64*               ArrayMeasAux2;
    ViInt32                 ArrayMeasSize;
    ViChar                  SensorType[10];
    ViBoolean               IsInStatMode;
    ViBoolean               IsPulseMeasurementOn;
    ViBoolean               IsPulseScalarsComplete;
    ViInt32                 PulseHighestParamNo;
};

typedef struct tag_SMainSession SMainSession;
typedef struct tag_SSessionInfo SSessionInfo;

typedef struct tag_mainSess_Node
{
    struct tag_SMainSession     *pMainSession;
    struct tag_mainSess_Node    *pNext;
} SMainSessionNode;

typedef struct tag_SensorDetail
{
    char    szLogicalName[128];
    char    szSensorType[128];
    char    szSerial[128];
} SensorDetail;

typedef struct
{
  const ViChar *szType;
  const ViChar *szMinFwRequired;
} SMinimalFirmware;


typedef void ApplicationCallbackFunc( NRP_SESSION session );

// Only for C/C++ users and for internal use by rsnrpz_GetSensorCount / rsnrpz_GetSensorInfo
static ViStatus rsnrpz_GetAvailableSensors( SensorDetail  sensor[], ViInt32 *size );

static CRITICAL_SECTION csAllSessions;

static void AddSessionNode( SMainSession* pMainSession );
static void RemoveSessionNode( SMainSession* pMainSession );
static int  NumSessionNodes( void );

/* This is the table that contains all main session numbers        */
/* The table MUST be initialized properly with zeros                */
static SMainSession* V2NTable[ RSNRPZ_MAX_MAIN_SESSIONS ] =
        { 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0 };

static SensorDetail        *pAvailableSensor     = 0;
static ViInt32              iAvailableSensors    = 0;
static ViBoolean            boDriverIsOpen       = VI_FALSE;
static ViBoolean            bCsInitialized       = VI_FALSE;
static CRITICAL_SECTION     cs;

// pointer for device changed call back function
static ApplicationCallbackFunc *pCallbackFn                = 0;
static NRP_USERARG              lDeviceChangedUserArgument = 0;

/**************************************************************************/
/*= Global data ==========================================================*/
/**************************************************************************/
/*
 * Boolean Range Table
 */
static SRangeTableEntry gs_BooleanRangeTable[] =
{
    { "OFF",  (1L << 0), VI_FALSE    },
    { "ON",   (1L << 1), VI_TRUE     },
    { "ONCE", (1L << 2), RSNRPZ_ONCE },
    { LAST_RANGE_TABLE_ENTRY         }
};

static SRangeTableEntry gs_ZeroingRangeTable[] =
{
    { "OFF",  (1L << 0), VI_FALSE        },
    { "ON",   (1L << 1), VI_TRUE         },
    { "ONCE", (1L << 2), RSNRPZ_ONCE     },
    { "LFR",  (1L << 3), RSNRPZ_ZERO_LFR },
    { "UFR",  (1L << 4), RSNRPZ_ZERO_UFR },
    { LAST_RANGE_TABLE_ENTRY             }
};

/*
 * Trigger slope and arm slope commands
 */
static SRangeTableEntry gs_SlopeRangeTable[] =
{
    {"POS",  (1L << 0), RSNRPZ_SLOPE_POSITIVE   },
    {"NEG",  (1L << 1), RSNRPZ_SLOPE_NEGATIVE   },
    { LAST_RANGE_TABLE_ENTRY                    }
};

/*
 * SENSe:AVERage:COUNt:AUTo:TYPe
 */
static SRangeTableEntry gs_AverCounAutoTypeRangeTable[] =
{
    {"RES",  (1L << 0), RSNRPZ_AUTO_COUNT_TYPE_RESOLUTION   },
    {"NSR",  (1L << 1), RSNRPZ_AUTO_COUNT_TYPE_NSR          },
    { LAST_RANGE_TABLE_ENTRY                                }
};

/*
 * SENSe:AVERage:TCONtrol
 */
static SRangeTableEntry gs_AverTconRangeTable[] =
{
    {"MOV",  (1L << 0), RSNRPZ_TERMINAL_CONTROL_MOVING  },
    {"REP",  (1L << 1), RSNRPZ_TERMINAL_CONTROL_REPEAT  },
    { LAST_RANGE_TABLE_ENTRY                            }
};

/*
 * SENSe:FUNCtion
 */
static SRangeTableEntry gs_FunctionRangeTable[] =
{
  {"\"POWer:AVG\"",         (1L << 0), RSNRPZ_SENSOR_MODE_CONTAV    },
  {"\"POWer:TSLot:AVG\"",   (1L << 1), RSNRPZ_SENSOR_MODE_TIMESLOT  },
  {"\"POWer:BURSt:AVG\"",   (1L << 2), RSNRPZ_SENSOR_MODE_BURST     },
  {"\"XTIMe:POWer\"",       (1L << 3), RSNRPZ_SENSOR_MODE_SCOPE     },
  {"\"XPOWer:CCDFunction\"",(1L << 5), RSNRPZ_SENSOR_MODE_CCDF      },
  {"\"XPOWer:PDFunction\"", (1L << 6), RSNRPZ_SENSOR_MODE_PDF       },
  {LAST_RANGE_TABLE_ENTRY                                           }
};

/*
 * TRIGger:SOURce
 */
static SRangeTableEntry gs_TriggerSourceRangeTable[] =
{
    {"HOLD",(1L << 0), RSNRPZ_TRIGGER_SOURCE_HOLD       },
    {"IMM", (1L << 1), RSNRPZ_TRIGGER_SOURCE_IMMEDIATE  },
    {"INT", (1L << 2), RSNRPZ_TRIGGER_SOURCE_INTERNAL   },
    {"BUS", (1L << 3), RSNRPZ_TRIGGER_SOURCE_BUS        },
    {"EXT", (1L << 4), RSNRPZ_TRIGGER_SOURCE_EXTERNAL   },
    {LAST_RANGE_TABLE_ENTRY                             }
};

/*
 * SENSe:SAMPle
 */
static SRangeTableEntry gs_SensSampRangeTable[] =
{
    {"FREQ1", (1L << 0), RSNRPZ_SAMPLING_FREQUENCY1 },
    {"FREQ2", (1L << 1), RSNRPZ_SAMPLING_FREQUENCY2 },
    {LAST_RANGE_TABLE_ENTRY                         }
};


/*
 * SENSe:AUXiliary
 */
static SRangeTableEntry gs_AuxValueTypeTable[] =
{
    {"NONE",   (1L << 0), RSNRPZ_AUX_NONE   },
    {"MINMAX", (1L << 1), RSNRPZ_AUX_MINMAX },
    {"RNDMAX", (1L << 2), RSNRPZ_AUX_RNDMAX },
    {LAST_RANGE_TABLE_ENTRY                 }
};

/*
 * SENSe:TRACe:MEASurement:ALGOrithm
 */
static SRangeTableEntry gs_AlgorithmRangeTable[] =
{
    {"HIST", (1L << 0), RSNRPZ_SCOPE_MEAS_ALG_HIST },
    {"INT",  (1L << 1), RSNRPZ_SCOPE_MEAS_ALG_INT  },
    {"PEAK", (1L << 3), RSNRPZ_SCOPE_MEAS_ALG_PEAK },
    {LAST_RANGE_TABLE_ENTRY                        }
};

static SRangeTableEntry gs_AlgorithmReadRangeTable[] =
{
    {"HISTOGRAM",   (1L << 0), RSNRPZ_SCOPE_MEAS_ALG_HIST },
    {"INTEGRATION", (1L << 1), RSNRPZ_SCOPE_MEAS_ALG_INT  },
    {"PEAK",        (1L << 3), RSNRPZ_SCOPE_MEAS_ALG_PEAK },
    {LAST_RANGE_TABLE_ENTRY                         }
};

static SRangeTableEntry gs_FreqSpacRangeTable[] =
{
    {"LIN", (1L << 0), RSNRPZ_SPACING_LINEAR },
    {"LOG", (1L << 1), RSNRPZ_SPACING_LOG },
    {LAST_RANGE_TABLE_ENTRY                         }
};

/* Simplification of command table definition */
#if defined(CMD)
    #undef CMD
#endif
#define CMD(command, groupNb, paramNb, table)  { command, groupNb, paramNb, table },

/*
 * States of the sensor trigger system into readable text
 * Used in NrpGetTriggerStateText() function.
 */
static NrpCommandTable gs_NrpCommands =
{
    CMD("CALibration:DATA",                 1, 1,  NULL)   /* Binary Block */
    CMD("CALibration:DATA:LENGth",          1, 3,  NULL)   /* Long */
    CMD("CALibration:ZERO:AUTO",            1, 2,  gs_ZeroingRangeTable)   /* Bitfield */
    CMD("CALibration:ZERO:FAST:AUTO",       1, 4,  gs_BooleanRangeTable)   /* Bitfield */
    CMD("CALibration:TEST",                 1, 5,  NULL)   /* Float */
    CMD("CALibration:TEST:DEVice",          1, 6,  NULL)   /* Float */
    CMD("CALibration:TEST:REFerence",       1, 7,  NULL)   /* Float */

    CMD("SENSe:BWIDth:VIDeo",               2, 1,  NULL)   /* String */
    CMD("SENSe:BWIDth:VIDeo:LIST",          2, 2,  NULL)   /* String read only*/
    CMD("SENSe:RGAMma:MAGNitude",           2, 3,  NULL)   /* Float */
    CMD("SENSe:RGAMma:PHASe",               2, 4,  NULL)   /* Float */
    CMD("SENSe:RGAMma:EUNC",                2, 6,  NULL)   /* Float */
    CMD("SENSe:FREQuency:STEP",             2, 8,  NULL)   /* Float */
    CMD("SENSe:FREQuency:SPACing",          2, 10, gs_FreqSpacRangeTable)   /* Bitfield */
    CMD("SENSe:POWer:AVG:BUFFer:COUNt",     2, 9,  NULL)   /* Long */
    CMD("SENSe:POWer:AVG:BUFFer:INFO",      2, 12, NULL)   /* String read only */
    CMD("SENSe:AVERage:STATe",              3, 4,  gs_BooleanRangeTable)   /* Bitfield */
    CMD("SENSe:AVERage:COUNt",              3, 1,  NULL)   /* Long */
    CMD("SENSe:AVERage:COUN:AUTO",          3, 2,  gs_BooleanRangeTable)   /* Bitfield */
    CMD("SENSe:AVERage:COUN:AUTO:MTIMe",    3, 18, NULL)   /* Float */
    CMD("SENSe:AVERage:COUN:AUTO:RESolution", 3, 19, NULL)   /* Long */
    CMD("SENSe:AVERage:COUN:AUTO:SLOT",     3, 20, NULL)   /* Long */
    CMD("SENSe:AVERage:COUN:AUTO:NSRatio",  3, 21, NULL)   /* Float */
    CMD("SENSe:AVERage:COUN:AUTO:TYPE",     3, 22, gs_AverCounAutoTypeRangeTable)   /* Bitfield */
    CMD("SENSe:AVERage:RESolution",         3, 29, NULL)   /* ---, write only */
    CMD("SENSe:AVERage:TCONtrol",           3, 5,  gs_AverTconRangeTable)   /* Bitfield */
    CMD("SENSe:CORRection:OFFSet",          3, 6,  NULL)   /* Float */
    CMD("SENSe:CORRection:OFFSet:STATe",    3, 7,  gs_BooleanRangeTable)   /* Bitfield */
    CMD("SENSe:CORRection:DCYCle",          3, 3,  NULL)   /* Float */
    CMD("SENSe:CORRection:DCYCle:STATe",    3, 8,  gs_BooleanRangeTable)   /* Bitfield */
    CMD("SENSe:CORRection:SPDevice:STATe",  3, 23, gs_BooleanRangeTable)   /* Bitfield */
    CMD("SENSe:CORRection:SPDevice:SELect", 3, 31, NULL)   /* Long */
    CMD("SENSe:CORRection:SPDevice:LIST",   2,  7, NULL)   /* String read only */
    CMD("SENSe:SGAMma:MAGNitude",           3, 24, NULL)   /* Float */
    CMD("SENSe:SGAMma:PHASe",               3, 25, NULL)   /* Float */
    CMD("SENSe:SGAMma:CORRection:STATe",    3, 26, gs_BooleanRangeTable)   /* Bitfield */
    CMD("SENSe:SGAMma:EUNC",                3, 27, NULL)   /* Float */
    CMD("SENSe:FREQuency",                  3, 9,  NULL)   /* Float */
    CMD("SENSe:FUNCtion",                   3, 10, gs_FunctionRangeTable)  /* Bitfield !!! value returned as string !!! */
    CMD("SENSe:EUNC:STATe",                 3, 11, gs_BooleanRangeTable)   /* Bitfield */
    CMD("SENSe:EUNC:SGAMma:STATe",          3, 28, gs_BooleanRangeTable)   /* Bitfield */
    CMD("SENSe:AUXiliary",                  3, 30, gs_AuxValueTypeTable)   /* Bitfield */
    CMD("SENSe:POWer:AVG:APERture",         4, 1,  NULL)   /* Float */
    CMD("SENSe:POWer:AVG:BUFFer:STATe",     4, 3,  gs_BooleanRangeTable)   /* Bitfield */
    CMD("SENSe:POWer:AVG:BUFFer:SIZE",      4, 2,  NULL)   /* Long */
    CMD("SENSe:POWer:AVG:SMO:STATe",        4, 4,  gs_BooleanRangeTable)   /* Bitfield */
    CMD("SENSe:POWer:TSL:AVG:COUNt",        5, 1,  NULL)   /* Long */
    CMD("SENSe:POWer:TSL:AVG:WIDTh",        5, 2,  NULL)   /* Float */
    CMD("SENSe:POWer:TSL:CHOPper:STATe",    5, 3,  gs_BooleanRangeTable)   /* Bitfield */
    CMD("SENSe:POWer:TSL:MID:OFFSet",       5, 4,  NULL)   /* Float */
    CMD("SENSe:POWer:TSL:MID:TIME",         5, 5,  NULL)   /* Float */
    CMD("SENSe:POWer:BURSt:DTOLerance",     6, 1,  NULL)   /* Float */
    CMD("SENSe:POWer:BURSt:CHOP:STATe",     6, 2,  gs_BooleanRangeTable)   /* Bitfield */
    CMD("SENSe:RANGe",                      3, 12, NULL)   /* Long */
    CMD("SENSe:RANGe:AUTO",                 3, 13, gs_BooleanRangeTable)   /* Bitfield */
    CMD("SENSe:RANGe:CLEVel",               3, 14, NULL)   /* Float */
    CMD("SENSe:SAMP",                       3, 17, gs_SensSampRangeTable)  /* Bitfield */
    CMD("SENSe:TRACe:OFFSet:TIME",          7, 2,  NULL)   /* Float */
    CMD("SENSe:TRACe:POINts",               7, 3,  NULL)   /* Long */
    CMD("SENSe:TRACe:REAL",                 7, 4,  gs_BooleanRangeTable)   /* Bitfield */
    CMD("SENSe:TRACe:TIME",                 7, 5,  NULL)   /* Float */
    CMD("SENSe:TRACe:MPW",                  7, 14, NULL)   /* Float */     /* TODO: NOT FUNCTION */
    CMD("SENSe:TRACe:AVERage:STATe",             7, 1,  gs_BooleanRangeTable)   /* Bitfield */
    CMD("SENSe:TRACe:AVERage:COUNt",             7, 6,  NULL)   /* Long  */
    CMD("SENSe:TRACe:AVERage:COUNt:AUTO",        7, 7,  gs_BooleanRangeTable)   /* Bitfield */
    CMD("SENSe:TRACe:AVERage:COUNt:AUTO:MTIMe",  7, 8,  NULL)   /* Float */
    CMD("SENSe:TRACe:AVERage:COUNt:AUTO:RES",    7, 9,  NULL)   /* Long  */
    CMD("SENSe:TRACe:AVERage:COUNt:AUTO:POIN",   7, 10, NULL)   /* Long  */     /* TODO: NOT FUNCTION */
    CMD("SENSe:TRACe:AVERage:COUNt:AUTO:NSRatio",7, 11, NULL)   /* Float */
    CMD("SENSe:TRACe:AVERage:COUNt:AUTO:TYPE",   7, 12, gs_AverCounAutoTypeRangeTable)   /* Bitfield */
    CMD("SENSe:TRACe:AVERage:TCONtrol",          7, 13, gs_AverTconRangeTable)   /* Bitfield */
    CMD("SENSe:TRACe:ESAM:AUTO",            7, 18, gs_BooleanRangeTable)   /* Bitfield  */
    CMD("SENSe:TIMe:EXCLude:STARt",         3, 15, NULL)   /* Float */
    CMD("SENSe:TIMe:EXCLude:STOP",          3, 16, NULL)   /* Float */

    CMD("SYSTem:INFO",                      8, 1,  NULL)   /* String */
    CMD("SYSTem:MINPower",                  8, 5,  NULL)   /* Float */
    CMD("SYSTem:INIT",                      8, 2,  NULL)   /* ---, write only */
    CMD("SYSTem:TRANsaction:BEGin",         8, 3,  NULL)   /* ---, write only */
    CMD("SYSTem:TRANsaction:END",           8, 4,  NULL)   /* ---, write only */
    CMD("SYSTem:RUTime",                    8, 8,  NULL)   /* Float */
    CMD("SYSTem:SUTime",                    8, 9,  NULL)   /* Float */
    CMD("SYSTem:FEAT?",                     8, 10, NULL)
    CMD("SYSTem:LIM?",                      8, 11, NULL)

    CMD("ABORt",                            9, 1,  NULL)   /* ---, write only */
    CMD("INIT:IMMediate",                   9, 3,  NULL)   /* ---, write only */
    CMD("INIT:CONTinuous",                  9, 2,  gs_BooleanRangeTable)   /* Bitfield */
    CMD("TRIGger:ATRigger:STATe",           9, 6,  gs_BooleanRangeTable)   /* Bitfield */
    CMD("TRIGger:COUNt",                    9, 7,  NULL)   /* Long */
    CMD("TRIGger:DELay",                    9, 8,  NULL)   /* Float */
    CMD("TRIGger:DELay:AUTO",               9, 9,  gs_BooleanRangeTable)   /* Bitfield */
    CMD("TRIGger:HOLDoff",                  9, 10, NULL)   /* Float */
    CMD("TRIGger:IMMediate",                9, 11, NULL)   /* ---, write only */
    CMD("TRIGger:LEVel",                    9, 12, NULL)   /* Float */
    CMD("TRIGger:SLOPe",                    9, 13, gs_SlopeRangeTable)   /* Bitfield */
    CMD("TRIGger:SOURce",                   9, 14, gs_TriggerSourceRangeTable) /* Bitfield */
    CMD("TRIGger:HYSTeresis",               9, 15, NULL)   /* Float */
    CMD("TRIGger:DTIMe",                    9, 16, NULL)   /* Float */
    CMD("TRIGger:MASTime:STATe",            9, 17, gs_BooleanRangeTable)   /* Bitfield */
    CMD("TRIGger:SYNC:STATe",               9, 19, gs_BooleanRangeTable)   /* Bitfield */

    CMD("*RST",                            11, 1, NULL)   /* ---, write only   */
    CMD("*TRG",                            11, 2, NULL)   /* ---, write only   */
    CMD("*IDN",                            11, 3, NULL)   /* String, read only */
    CMD("*TST",                            11, 4, NULL)   /* Long, read only   */
    CMD("TEST:SENSe",                      12, 1, NULL)   /* String, read only */
    CMD("SERVice:UNL 1234",                10, 4, NULL)   /* ---, write only   */
    CMD("SERVice:CAL:TEMP",                10,13, gs_BooleanRangeTable)   /* ---, write only */
    CMD("SERVice:CAL:TEMP:DATA",           10,14, NULL)   /* Float */

    CMD("SENSe:STATe:OFFSet:TIME",         14, 1, NULL)   /* Float */
    CMD("SENSe:STATe:TIME",                14, 2, NULL)   /* Float */
    CMD("SENSe:STATe:MID:OFFSet",          14, 3, NULL)   /* Float */
    CMD("SENSe:STATe:MID:TIME",            14, 4, NULL)   /* Float */
    CMD("SENSe:STATe:SCALE:X:RLEVel",      14, 5, NULL)   /* Float */
    CMD("SENSe:STATe:SCALE:X:RANGe",       14, 6, NULL)   /* Float */
    CMD("SENSe:STATe:SCALE:X:POINts",      14, 7, NULL)   /* Long  */
    CMD("SENSe:STATe:SCALE:X:MPW",         14, 8, NULL)   /* Float */

    CMD("SENSe:TRACe:MEASurement:STATe",                            16, 1, gs_BooleanRangeTable)   /* Bitfield */
    CMD("SENSe:TRACe:MEASurement:ALGorithm",                        16, 2, gs_AlgorithmRangeTable) /* Bitfield */
    CMD("SENSe:TRACe:MEASurement:AUTO",                             16, 3, gs_BooleanRangeTable)   /* Bitfield */
    CMD("SENSe:TRACe:MEASurement:DEFine:DURation:REFerence",        16, 4, NULL)   /* Float */
    CMD("SENSe:TRACe:MEASurement:DEFine:TRANsition:LREFerence",     16, 5, NULL)   /* Float */
    CMD("SENSe:TRACe:MEASurement:DEFine:TRANsition:HREFerence",     16, 6, NULL)   /* Float */
    CMD("SENSe:TRACe:MEASurement:OFFSet:TIME",                      16, 7, NULL)   /* Float */
    CMD("SENSe:TRACe:MEASurement:PULSe:DCYCle",                     16, 8, NULL)   /* Float */
    CMD("SENSe:TRACe:MEASurement:PULSe:DURation",                   16, 9, NULL)   /* Float */
    CMD("SENSe:TRACe:MEASurement:PULSe:PERiod",                     16, 10, NULL)   /* Float */
    CMD("SENSe:TRACe:MEASurement:PULSe:SEParation",                 16, 11, NULL)   /* Float */
    CMD("SENSe:TRACe:MEASurement:TRANsition:NEGative:DURation",     16, 12, NULL)   /* Float */
    CMD("SENSe:TRACe:MEASurement:TRANsition:NEGative:OCCurrence",   16, 13, NULL)   /* Float */
    CMD("SENSe:TRACe:MEASurement:TRANsition:NEGative:OVERshot",     16, 14, NULL)   /* Float */
    CMD("SENSe:TRACe:MEASurement:TRANsition:POSitive:DURation",     16, 15, NULL)   /* Float */
    CMD("SENSe:TRACe:MEASurement:TRANsition:POSitive:OCCurrence",   16, 16, NULL)   /* Float */
    CMD("SENSe:TRACe:MEASurement:TRANsition:POSitive:OVERshot",     16, 17, NULL)   /* Float */
    CMD("SENSe:TRACe:MEASurement:POWer:MIN",                        16, 18, NULL)   /* Float */
    CMD("SENSe:TRACe:MEASurement:POWer:MAX",                        16, 19, NULL)   /* Float */
    CMD("SENSe:TRACe:MEASurement:POWer:AVG",                        16, 20, NULL)   /* Float */
    CMD("SENSe:TRACe:MEASurement:POWer:PULSe:TOP",                  16, 21, NULL)   /* Float */
    CMD("SENSe:TRACe:MEASurement:POWer:PULSe:BASE",                 16, 22, NULL)   /* Float */
    CMD("SENSe:TRACe:MEASurement:POWer:REFerence",                  16, 23, NULL)   /* Float */
    CMD("SENSe:TRACe:MEASurement:POWer:LREFerence",                 16, 24, NULL)   /* Float */
    CMD("SENSe:TRACe:MEASurement:POWer:HREFerence",                 16, 25, NULL)   /* Float */
    CMD("SENSe:TRACe:MEASurement:TRANsition:SPER",                  16, 26, NULL)   /* Float */

    CMD("SENSe:TRACe:MEASurement:TRANsition:ESAM:AUTO",   17, 1,  gs_BooleanRangeTable)   /* Bitfield */
    CMD("SENSe:TRACe:MEASurement:TIME",                   17, 2,  NULL)   /* Float */

    CMD("SENSe:POWer:TGAT:CHOP:STATe",          100, 1, gs_BooleanRangeTable) /* Bitfield */
    CMD("SENSe:POWer:TGAT1:OFFS:TIME",          101, 1, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT1:TIME",               101, 2, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT1:FREQuency",          101, 3, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT1:EXCLude:MID:OFFSet", 101, 4, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT1:EXCLude:MID:TIME",   101, 5, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT2:OFFSet:TIME",        102, 1, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT2:TIME",               102, 2, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT2:FREQuency",          102, 3, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT2:EXCLude:MID:OFFSet", 102, 4, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT2:EXCLude:MID:TIME",   102, 5, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT3:OFFSet:TIME",        103, 1, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT3:TIME",               103, 2, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT3:FREQuency",          103, 3, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT3:EXCLude:MID:OFFSet", 103, 4, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT3:EXCLude:MID:TIME",   103, 5, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT4:OFFSet:TIME",        104, 1, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT4:TIME",               104, 2, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT4:FREQuency",          104, 3, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT4:EXCLude:MID:OFFSet", 104, 4, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT4:EXCLude:MID:TIME",   104, 5, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT5:OFFSet:TIME",        105, 1, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT5:TIME",               105, 2, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT5:FREQuency",          105, 3, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT5:EXCLude:MID:OFFSet", 105, 4, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT5:EXCLude:MID:TIME",   105, 5, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT6:OFFSet:TIME",        106, 1, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT6:TIME",               106, 2, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT6:FREQuency",          106, 3, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT6:EXCLude:MID:OFFSet", 106, 4, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT6:EXCLude:MID:TIME",   106, 5, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT7:OFFSet:TIME",        107, 1, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT7:TIME",               107, 2, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT7:FREQuency",          107, 3, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT7:EXCLude:MID:OFFSet", 107, 4, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT7:EXCLude:MID:TIME",   107, 5, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT8:OFFSet:TIME",        108, 1, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT8:TIME",               108, 2, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT8:FREQuency",          108, 3, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT8:EXCLude:MID:OFFSet", 108, 4, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT8:EXCLude:MID:TIME",   108, 5, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT9:OFFSet:TIME",        109, 1, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT9:TIME",               109, 2, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT9:FREQuency",          109, 3, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT9:EXCLude:MID:OFFSet", 109, 4, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT9:EXCLude:MID:TIME",   109, 5, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT10:OFFSet:TIME",       110, 1, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT10:TIME",              110, 2, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT10:FREQuency",         110, 3, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT10:EXCLude:MID:OFFSet",110, 4, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT10:EXCLude:MID:TIME",  110, 5, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT11:OFFSet:TIME",       111, 1, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT11:TIME",              111, 2, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT11:FREQuency",         111, 3, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT11:EXCLude:MID:OFFSet",111, 4, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT11:EXCLude:MID:TIME",  111, 5, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT12:OFFSet:TIME",       112, 1, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT12:TIME",              112, 2, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT12:FREQuency",         112, 3, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT12:EXCLude:MID:OFFSet",112, 4, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT12:EXCLude:MID:TIME",  112, 5, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT13:OFFSet:TIME",       113, 1, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT13:TIME",              113, 2, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT13:FREQuency",         113, 3, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT13:EXCLude:MID:OFFSet",113, 4, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT13:EXCLude:MID:TIME",  113, 5, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT14:OFFSet:TIME",       114, 1, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT14:TIME",              114, 2, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT14:FREQuency",         114, 3, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT14:EXCLude:MID:OFFSet",114, 4, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT14:EXCLude:MID:TIME",  114, 5, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT15:OFFSet:TIME",       115, 1, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT15:TIME",              115, 2, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT15:FREQuency",         115, 3, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT15:EXCLude:MID:OFFSet",115, 4, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT15:EXCLude:MID:TIME",  115, 5, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT16:OFFSet:TIME",       116, 1, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT16:TIME",              116, 2, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT16:FREQuency",         116, 3, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT16:EXCLude:MID:OFFSet",116, 4, NULL)   /* Float */
    CMD("SENSe:POWer:TGAT16:EXCLude:MID:TIME",  116, 5, NULL)   /* Float */

    /* add more entries here */

    {   NULL,                                       0, 0, NULL}
};


static ViInt32  gs_class_registers[] = {
        RSNRPZ_STATCLASS_D_CONN,
        RSNRPZ_STATCLASS_D_ERR,
        RSNRPZ_STATCLASS_O_CAL,
        RSNRPZ_STATCLASS_O_MEAS,
        RSNRPZ_STATCLASS_O_TRIGGER,
        RSNRPZ_STATCLASS_O_SENSE,
        RSNRPZ_STATCLASS_O_LOWER,
        RSNRPZ_STATCLASS_O_UPPER,
        RSNRPZ_STATCLASS_Q_POWER,
        RSNRPZ_STATCLASS_Q_WINDOW,
        RSNRPZ_STATCLASS_Q_CAL,
        RSNRPZ_STATCLASS_Q_ZER,
};

static ViInt32  gs_driver_registers[] = {
    DEVICE_STATUS_REGISTER_CONNECTED,
    DEVICE_STATUS_REGISTER_ERROR,
    OPERATION_CALIBRATING_STATUS_REGISTER,
    OPERATION_MEASURING_STATUS_REGISTER,
    OPERATION_TRIGGER_STATUS_REGISTER,
    OPERATION_SENSE_STATUS_REGISTER,
    OPERATION_LOWER_LIMIT_FAIL_STATUS_REGISTER,
    OPERATION_UPPER_LIMIT_FAIL_STATUS_REGISTER,
    QUESTIONABLE_POWER_STATUS_REGISTER,
    QUESTIONABLE_WINDOW_STATUS_REGISTER,
    QUESTIONABLE_CALIBRATING_STATUS_REGISTER,
    QUESTIONABLE_ZERO_STATUS_REGISTER
};

static int      gs_class_registers_count = sizeof(gs_class_registers)/sizeof(*gs_class_registers);
static ViUInt32 gs_ulWriteTimeout        = 25000;


/*****************************************************************************/
/*= NRP Control Callbacks ===================================================*/
/*****************************************************************************/
static void _NRP_FUNC NrpCommandAcceptedCallback(
    NRP_SESSION session,
    long        groupParam,
    NRP_USERARG userArgument
);

static void _NRP_FUNC NrpDeviceChangedCallback(
    NRP_USERARG userArgument
);

static void _NRP_FUNC NrpDataAvailableCallback(
    NRP_SESSION session,
    long        dataType,
    NRP_USERARG userArgument
);

static void _NRP_FUNC NrpErrorOccurredCallback(
    NRP_SESSION session,
    long        errorCode,
    NRP_USERARG userArgument
);

static void _NRP_FUNC NrpStateChangedCallback(
    NRP_SESSION session,
    long        sensorTriggerState,
    NRP_USERARG userArgument
);

/*****************************************************************************/
/*= UTILITY ROUTINE DECLARATIONS (Non-Exportable Functions) =================*/
/*****************************************************************************/
//static ViInt32 rsnrpz_CheckInstr (ViSession vi, ViInt32 channel, ViString instr);
static ViInt32 rsnrpz_GetNbOfCommands(void);

static ViStatus rsnrpz_GetIndexFromCommand(
    const ViString  command,
    ViInt32*        index
);

static ViStatus rsnrpz_GetIndexFromPacket(
    ViInt32     groupNb,
    ViInt32     paramNb,
    ViInt32*    index
);

static ViStatus rsnrpz_GetBitFromValue(
    const SRangeTableEntry* table,
    ViInt32     value,
    ViInt32*    pBit
);

static ViStatus rsnrpz_GetBitFromString(
    const SRangeTableEntry* table,
    const ViString  string,
    ViInt32*        pBit
);

static ViStatus rsnrpz_GetStringFromValue(
    const SRangeTableEntry* table,
    ViInt32             value,
    const ViChar**      pParameter
);

static ViStatus rsnrpz_GetValueFromBit(
    const SRangeTableEntry* table,
    ViInt32             bit,
    ViInt32*            pValue
);

static ViStatus rsnrpz_CheckRange(
    NrpValueTable   table,
    ViInt32         index,
    void*           value
);

static ViStatus rsnrpz_FormatCommand(
    NrpValueTable   table,
    ViChar          buffer[],
    ViInt32         index,
    void*           value
);

static ViStatus rsnrpz_GetValueFromCache(
    NrpValueTable   table,
    ViInt32         index,
    void*           value,
    ViInt32         parameterSize
);

static ViStatus rsnrpz_SetGroupFeature(
    NRP_SESSION session,
    ViInt32     groupNb,
    long        feature[],
    ViInt32     count
);

static ViStatus rsnrpz_WriteCommand(
    ViSession       vi,
    ViInt32         channel,
    const ViString  command,
    void*           parameter
);

static ViStatus rsnrpz_ReadValue(
    ViSession       vi,
    ViInt32         channel,
    const ViString  command,
    void*           parameter,
    int             parameterSize
);

static ViStatus rsnrpz_InitializeRegisters(
    TNrpRegister    pRegisters[]
);

static ViStatus rsnrpz_UpdateRegisters(
    SMainSession*   pMainSession,
    int             nrpRegister,
    int             registerType,
    TRegister       mask
);
static ViStatus rsnrpz_SetRegister(
    TNrpRegister*   pNrpRegister,
    int             registerType,
    TRegister       mask,
    ViBoolean*      pSummary
);


static ViStatus rsnrpz_SetBitInRegisters(
    SMainSession*   pMainSession,
    int             nrpRegister,
    int             registerType,
    ViInt32         bit,
    ViBoolean       state
);

static ViStatus rsnrpz_QueryRegister(
    ViSession       vi,
    int             nrpRegister,
    int             registerType,
    TRegister*      pRegister
);

static ViStatus rsnrpz_defaultInstrSetup(
    ViSession   vi,
    ViInt32     channel
);

static ViStatus rsnrpz_CreateEvents(
    ViSession   vi,
    ViInt32     channel,
    long        groupNb,
    long        paramNb,
    HANDLE*     pEventHandleArray,
    ViUInt32*   pCount
);

static ViStatus rsnrpz_DestroyEvents(
    ViSession   vi,
    HANDLE*     pEventHandleArray,
    ViInt32     count
);

static ViStatus rsnrpz_CreateEvent(
//  ViSession session,
    NRP_SESSION session,
    long        groupNb,
    long        paramNb,
    HANDLE*     pEventHandle
);

static ViStatus rsnrpz_SetEvent(
//  ViSession   session,
    NRP_SESSION session,
    long        groupNb,
    long        paramNb
);

static ViStatus rsnrpz_PutError(
    ViSession   vi,
    ViInt32     errorCode,
    ViString    errorMessage
);

static ViInt32 GetNbOfErrors(
    SErrorEntry     errorQueue[]
);

static ViInt32 RemoveFirstError(
    ViSession       vi
);

static ViStatus rsnrpz_checkStatus (
    ViSession vi
);

static ViStatus rsnrpz_NotifyCommand(
    ViSession   vi,
    ViInt32     channel,
    ViInt32     groupNb,
    ViInt32     paramNb,
    ViInt32     parameter,
    ViBoolean   isCommandAccepted
);


#if ! defined (USE_CHECKINSTR)
static ViBoolean rsnrpz_IsSupported(
    ViSession vi,
    ViInt32   channel,
    ViInt32   groupNb,
    ViInt32   paramNb
);
#endif

/*===========================================================================*/
/* Function: C_dblstr                                                        */
/* Purpose:  This function formats a double into a string in C locale        */
/*===========================================================================*/
const char *C_dblstr( double d, const char *cpFmt, char *cpBuf, int iMaxLen )
{
  int i;

  memset( cpBuf, 0x00, iMaxLen );

#if defined (_CVI_)
  #if _CVI_ < 900

    /* CVI up to 8.0 does not have 'snprintf' */
  {
    char szDbl[1024];   // big enough to hold a formatted double

    memset( szDbl, 0x00, sizeof( szDbl ) );
    sprintf( szDbl, cpFmt, d );
    if ( iMaxLen >= (int)sizeof( szDbl ) )
      iMaxLen = (int)sizeof( szDbl ) - 1;
    szDbl[ iMaxLen ] = 0x00;
    strcpy( cpBuf, szDbl );
  }
  #else

    /* CVI 9.0 and higher has 'snprintf' */

    snprintf( cpBuf, iMaxLen-1, cpFmt, d );

  #endif
#else
  /* not CVI */
  #if defined(__linux__) || defined(__APPLE__)
      snprintf( cpBuf, iMaxLen-1, cpFmt, d );
  #else
      _snprintf( cpBuf, iMaxLen-1, cpFmt, d );
  #endif
#endif

  // Sensor always needs decimal points as in C locale
    iMaxLen = (int)strlen( cpBuf );
    for ( i = 0; i < iMaxLen; i++ )
        if ( cpBuf[i] == ',' )
            cpBuf[i] = '.';

    return cpBuf;
}

/*===========================================================================*/
/* Function: stristr                                                         */
/* Purpose:  This function implements a case insensitive 'strstr()'          */
/*===========================================================================*/
const char *stristr( const char *cpHaystack, const char *cpNeedle )
{
    if ( !*cpNeedle )
        return cpHaystack;

    for ( ; *cpHaystack; ++cpHaystack )
    {
        if ( toupper( *cpHaystack ) == toupper( *cpNeedle ) )
        {
            // First char matched -- loop through the remaining chars...
            const char *cpHs, *cpNdl;

            for ( cpHs = cpHaystack, cpNdl = cpNeedle; *cpHs && *cpNdl; ++cpHs, ++cpNdl )
            {
                if ( toupper( *cpHs ) != toupper( *cpNdl ) )
                    break;
            }

            if ( ! *cpNdl )         // all of 'cpNeedle' chars matched
            {                       //        up to null termination!
                return cpHaystack;  // return the cpHaystack
            }
        }
    }
    return 0;
}

/*===========================================================================*/
/* Function: STRICMP                                                         */
/* Purpose:  This function implements a case insensitive 'strcmp()'          */
/*===========================================================================*/
int STRICMP( const char *cpFirst, const char *cpSecond )
{
#if defined(__linux__) || defined(__APPLE__)

    if ( cpFirst == cpSecond )  // even works if both are NULL pointers
        return 0;

    if ( cpFirst == 0 )
        return -1;

    if ( cpSecond == 0 )
        return 1;

    // Well, now BOTH are valid pointers
    --cpFirst;
    --cpSecond;

    do
    {
        ++cpFirst;
        ++cpSecond;

        if ( toupper( *cpFirst ) < toupper( *cpSecond ) )
            return -1;

        if ( toupper( *cpFirst ) > toupper( *cpSecond ) )
            return 1;
    }
    while ( *cpFirst && *cpSecond );

    // if we arrive here, BOTH pointers point to 0x00
    return 0;

#else

#if defined (_CVI_)
    return stricmp( cpFirst, cpSecond );
#else
    // stricmp is deprecated; better using _stricmp()
    return _stricmp( cpFirst, cpSecond );
#endif

#endif
}



/* --------------------------------------------------------------------------------
 * Comparator for SCPI commands strings (mandatory / long form)
 * --------------------------------------------------------------------------------
 *
 * int StrCmpMandatorySCPI( const char *cpLongForm, const char *cpUserVersion );
 *
 *   'cpLongForm'    shall contain the mandatory part in UPPERCASE,
 *                   (for example, XTIMe:POWer)
 *   'cpUserVersion' is not case sensitive
 *
 * Returns    == 0   SCPI strings are "equal"
 *            != 0   SCPI strings are NOT "equal"
 *
 * Tests
 *   StrCmpMandatorySCPI( "\"XTIMe:POWer\"",           "XTIM:POW"      ) ); // == 0  (equal)
 *   StrCmpMandatorySCPI( "XTIMe:POWer",               "XTIME:POWER"   ) ); // == 0  (equal)
 *   StrCmpMandatorySCPI( "\"XTIMe:POWer\"",           "xtim:POw"      ) ); // == 0  (equal)
 *   StrCmpMandatorySCPI( "XTIMe:POWer",               "xTiMe:PoWeR"   ) ); // == 0  (equal)
 *   StrCmpMandatorySCPI( "XTIMe:POWer",               "\"xTiMe:PoW\"" ) ); // == 0  (equal)
 *   StrCmpMandatorySCPI( "XTIMe:POWer",               "XTi:POW"       ) ); // != 0  (not equal)
 *   StrCmpMandatorySCPI( "SENSe:POWer:AVERage:FIXed", "SENS:POW:AVER" ) ); // != 0  (not equal)
 *
 * -------------------------------------------------------------------------------- */

typedef struct tag_strToken
{
  char  *cpStart;
  char  *cpOrig;
  char   cDelimiter;
  size_t sizOrig;
} tStrToken;

tStrToken *Tokenizer( const char *cpData, char cDelim )
{
  tStrToken *pTok = (tStrToken *)calloc( 1, sizeof( tStrToken ) );
  if ( ! cpData )
    return pTok;

  pTok->sizOrig = strlen( cpData ) + 1;  /* size includes 0x00 terminator */
  pTok->cpOrig  = (char *)calloc( pTok->sizOrig, sizeof( char ) );

  /* Be tolerant on strings with apostrophes ( "xyz" ) */
  if ( (pTok->sizOrig > 2) && (*cpData == '"') && (*(cpData + pTok->sizOrig - 2) == '"') )
  {
    memcpy( pTok->cpOrig, cpData + 1, pTok->sizOrig - 3 );
    pTok->sizOrig -= 2;
  }
  else
    memcpy( pTok->cpOrig, cpData, pTok->sizOrig );
  
  pTok->cpStart    = pTok->cpOrig;
  pTok->cDelimiter = cDelim;

  return pTok;
}

const char *GetNextToken( tStrToken *pTok )
{
  char *cpDelim = 0;
  const char *cpToken = pTok->cpStart;

  if ( ! pTok )
    return 0;

  cpDelim = strchr( pTok->cpStart, pTok->cDelimiter );
  if ( ! cpDelim )
  {
    pTok->cpStart = &pTok->cpOrig[ pTok->sizOrig - 1 ];
    return cpToken;
  }

  *cpDelim = 0x00;
  cpDelim++;
  pTok->cpStart = cpDelim;

  return cpToken;
}

int HasMoreTokens( tStrToken *pTok )
{
  if ( ! pTok )
    return 0;

  if ( ! pTok->cpStart )
    return 0;

  if ( ! *(pTok->cpStart) )
    return 0;

  return 1;
}

tStrToken *FinishTokenizer( tStrToken *pTok )
{
  if ( ! pTok )
    return pTok;

  free( pTok->cpOrig );
  pTok->cDelimiter = 0x00;
  pTok->sizOrig    = 0;
  pTok->cpOrig     = 0;
  pTok->cpStart    = 0;

  free( pTok );
  pTok = 0;

  return pTok;
}


int StrCmpMandatorySCPI( const char *cpLongForm, const char *cpUserVersion )
{
  tStrToken *pTokenizerA = 0;
  tStrToken *pTokenizerB = 0;
  int        iCmp        = 0;
  size_t     nLen        = 0;

  if ( cpLongForm == cpUserVersion )  /* even works if both are NULL pointers */
    return 0;

  if ( cpLongForm == 0 )
    return -1;

  if ( cpUserVersion == 0 )
    return 1;

  /* BOTH are valid pointers now */
  pTokenizerA = Tokenizer( cpLongForm,    ':' );
  pTokenizerB = Tokenizer( cpUserVersion, ':' );

  while ( HasMoreTokens( pTokenizerA ) && HasMoreTokens( pTokenizerB ) )
  {
    const char *cpA = GetNextToken( pTokenizerA );
    const char *cpB = GetNextToken( pTokenizerB );
    char       *cpMand = 0;
    char       *cpDest = 0;

/*
    printf( "A = %s\n", cpA );
    printf( "B = %s\n", cpB );
*/

    iCmp = STRICMP( cpA, cpB );
    if ( iCmp == 0 )
      continue;

    /* Both strings are NOT identical;  */
    /* compare *mandatory* parts        */
    cpMand = (char *)calloc( 1, strlen( cpA ) + 1 );
    cpDest = cpMand;
    for ( nLen = 0; nLen < strlen( cpA ); nLen++ )
    {
      if ( isupper( *(cpA + nLen) ) )
        *(cpDest++) = *(cpA + nLen);
    }

    iCmp = STRICMP( cpMand, cpB );
    free( cpMand );
    cpMand = 0;

    if ( iCmp != 0 )
      break;

  }

  /* Are there any tokens left...? */
  if (   HasMoreTokens( pTokenizerA ) && ! HasMoreTokens( pTokenizerB ) )
    iCmp = 1;
  if ( ! HasMoreTokens( pTokenizerA ) &&   HasMoreTokens( pTokenizerB ) )
    iCmp = -1;


  pTokenizerA = FinishTokenizer( pTokenizerA );
  pTokenizerB = FinishTokenizer( pTokenizerB );

  return iCmp;
}
/* --------------------------------------------------------------------------------
 * -------------------------------------------------------------------------------- */





/* Check for a valid session number */
ViBoolean CheckSession( ViSession vi )
{
    if( (vi<1) || (vi>RSNRPZ_MAX_MAIN_SESSIONS) )
        return FALSE;
    if(  V2NTable[ vi-1 ]==0 )
        return FALSE;
    return TRUE;
}

/* Check for a valid session and channel number */
ViBoolean CheckChannel( ViSession vi, int channel )
{
    if( !CheckSession( vi ) )
        return FALSE;
    if (channel < 1 || channel > RSNRPZ_MAX_CHANNELS)
        return FALSE;
    if ( V2NTable[vi-1]->pSessions[channel-1] == 0 )
        return FALSE;
    return TRUE;
}

// retrieves the SMainSession pointer from a ViSession (index)
// The function returns zero is no valid pointer was found
static SMainSession* V2N( ViSession lSess )
{
    if ( (lSess<1) || (lSess>RSNRPZ_MAX_MAIN_SESSIONS) )
    {
        trace_log( "V2N() called with invalid session number." );
        return 0;
    }

    return V2NTable[ lSess-1 ];
}

static ViSession N2V( SMainSession *pSession )
{
    int iIdx;

    for ( iIdx = 0; iIdx < RSNRPZ_MAX_MAIN_SESSIONS; iIdx++ )
    {
        if ( V2NTable[ iIdx ] == pSession )
            return (ViSession)(iIdx+1);
    }

    trace_log( "N2V() did not find a valid session number." );
    return 0;
}

void MakeSureCSInitialized( void )
{
    if ( bCsInitialized )
        return;

    trace_log( "Initializing critical sections." );

    bCsInitialized = VI_TRUE;
    InitializeCriticalSection( &cs );
    InitializeCriticalSection( &csAllSessions );
}

// get the session info structure from a given session number
static SSessionInfo* GetSesInfoFromSession( NRP_SESSION pSession )
{
    int iIdx, j;

    /* search the main sessions */
    for ( iIdx = 0; iIdx < RSNRPZ_MAX_MAIN_SESSIONS; iIdx++ )
    {
        if( V2NTable[iIdx]!=0 )
        {
            /* search channel sessions */
            for ( j = 0; j<RSNRPZ_MAX_CHANNELS; j++ )
            {
                // this is a match
                if( V2NTable[iIdx]->pSessions[j] == pSession )
                    return V2NTable[iIdx]->pSInfos[j];
            }
        }
    }

    trace_log( "GetSesInfoFromSession() did not find valid session information." );
    return 0;
}

/*===========================================================================*/
/* Function: DiscardAllSessions                                              */
/* Purpose:  This function removes all session information                   */
/*===========================================================================*/
void DiscardAllSessions( void )
{
    int iIdx = 0;

    trace_log( "Cleaning up all session information." );

    MakeSureCSInitialized();

    EnterCriticalSection( &csAllSessions );

    for ( iIdx = 0; iIdx < RSNRPZ_MAX_MAIN_SESSIONS; iIdx++ )
        V2NTable[ iIdx ] = 0;

    LeaveCriticalSection( &csAllSessions );

}


/*===========================================================================*/
/* Function: AddSessionNode                                                  */
/* Purpose:                                                                  */
/*===========================================================================*/
static void AddSessionNode( SMainSession* pMainSession )
{
    int iIdx  = 0;
    int iFree = -1;

    trace_log( "Adding a main session node." );

    MakeSureCSInitialized();

    EnterCriticalSection( &csAllSessions );

    /* check if the main session number is already there */
    for ( iIdx = 0; iIdx < RSNRPZ_MAX_MAIN_SESSIONS; iIdx++ )
        if ( V2NTable[ iIdx ] == pMainSession )
        {
            iFree = iIdx;
            break;
        }

    /* check for empty space */
    if ( iFree < 0 )
    {
        for ( iIdx = 0; iIdx < RSNRPZ_MAX_MAIN_SESSIONS; iIdx++ )
            if ( V2NTable[ iIdx ] == 0 )
            {
                iFree = iIdx;
                break;
            }
    }

    if ( iFree >= 0 )
        V2NTable[ iFree ] = pMainSession;

    LeaveCriticalSection( &csAllSessions );
}

/*===========================================================================*/
/* Function: RemoveSessionNode                                               */
/* Purpose:  Searches the given session in our session-list and              */
/*           removes the corresponding entry from the list                   */
/*===========================================================================*/
static void RemoveSessionNode( SMainSession* pMainSession )
{
    int iIdx = 0;

    trace_log( "Removing the main session %d.", pMainSession );

    MakeSureCSInitialized();
    EnterCriticalSection( &csAllSessions );

    for ( iIdx = 0; iIdx < RSNRPZ_MAX_MAIN_SESSIONS; iIdx++ )
        if ( V2NTable[ iIdx ] == pMainSession )
            V2NTable[ iIdx ] = 0;

    LeaveCriticalSection( &csAllSessions );
}

/*===========================================================================*/
/* Function: NumSessionNodes                                                 */
/* Purpose:  Returns the number of sessions which are stored in our list     */
/*===========================================================================*/
static int NumSessionNodes( void )
{
    int iRet = 0;
    int iIdx = 0;

    MakeSureCSInitialized();
    EnterCriticalSection( &csAllSessions );

    for ( iIdx = 0; iIdx < RSNRPZ_MAX_MAIN_SESSIONS; iIdx++ )
        if ( V2NTable[ iIdx ] != 0 )
            iRet++;

    LeaveCriticalSection( &csAllSessions );

    return iRet;

}

/*===========================================================================*/
/*                                                                           */
/*       USER-CALLABLE FUNCTIONS (Exported Functions)                        */
/*                                                                           */
/*===========================================================================*/


/*===========================================================================*/
/* Function: set_timeout                                                     */
/* Purpose:  This function sets the write-timeout in millisec.               */
/*           On Windows Embedded an USB-Write could block for a long         */
/*             amount of time.                                                   */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_setTimeout"
ViStatus _VI_FUNC rsnrpz_setTimeout( ViUInt32 ulNewTimo )
{
    gs_ulWriteTimeout = ulNewTimo;

    return VI_SUCCESS;
}

/*===========================================================================*/
/* Function: get_timeout                                                     */
/* Purpose:  This function returns the current write-timeout value in millis.*/
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_getTimeout"
ViStatus _VI_FUNC rsnrpz_getTimeout( ViUInt32 *pulNewTimo )
{
    if ( pulNewTimo )
        *pulNewTimo = gs_ulWriteTimeout;

    return VI_SUCCESS;
}

/*===========================================================================*/
/* Function: Internal Initialize                                             */
/* Purpose:  This function opens the instrument, queries the instrument      */
/*           for its ID, and initializes the instrument to a known state.    */
/*           (It takes care about selecting default or long distance         */
/*            connections in the NrpControl2 DLL)                            */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_private_setup"
ViStatus _VI_FUNC rsnrpz_private_setup(
    ViRsrc      resourceName,
    ViBoolean   IDQuery,
    ViBoolean   resetDevice,
    ViSession*  pInstrSession,
    ViBoolean   bLongDistance
)
{
    ViStatus        rsnrpz_status = NRP_SUCCESS;
    NRP_SESSION     session;
    SMainSession*   pmain_session = 0;
    ViInt32         index, nb_of_commands;
    SSessionInfo*   psession_info = 0;
    char            event_name[MAX_PATH];
    ViBoolean        bLocked       = VI_FALSE;

    trace_log( "%s(Rsc=%s, ID=%d, Reset=%d, LD=%d )", __FUNC__, resourceName, IDQuery, resetDevice, bLongDistance );

    // The very first call will initialize the critical section
    MakeSureCSInitialized();

    /*- Check input parameter ranges ----------------------------------------*/
    if (IDQuery != VI_TRUE && IDQuery != VI_FALSE)
        return VI_ERROR_PARAMETER2;
    if (resetDevice != VI_TRUE && resetDevice != VI_FALSE)
        return VI_ERROR_PARAMETER3;
    if ( pInstrSession == NULL )
        return VI_ERROR_PARAMETER4;

    /*- Open instrument session ---------------------------------------------*/

    // OpenDriver() MUST NOT be called from more than one thread simultaneously
    // We need a variable to know if the unlock is necessary in case of an error.
    EnterCriticalSection(&cs);
    bLocked=VI_TRUE;

    if ( boDriverIsOpen == VI_FALSE )
    {
        CHECKERR_CU( NrpOpenDriver());
        boDriverIsOpen = VI_TRUE;
    }

    if ( bLongDistance == VI_TRUE )
        NrpSetAttribute( 0, NRP_ATTR_DISTANCE_LONG, 0 );
    else
        NrpSetAttribute( 0, NRP_ATTR_DISTANCE_DEFAULT, 0 );

    CHECKERR_CU( NrpOpenSensor( resourceName, &session));

    pmain_session = (SMainSession*)malloc( sizeof(SMainSession));
    memset( pmain_session, 0, sizeof(SMainSession));
    pmain_session->ErrorChecking = VI_TRUE;
    InitializeCriticalSection(&(pmain_session->cs));

    sprintf(event_name, "NrpDriverSession_%p", (void*)pmain_session);
    pmain_session->event = CreateEventA( NULL, TRUE, TRUE, event_name);
    SetEvent(pmain_session->event);
    /* TODO: check error */

    /* set all sessions that are associated to this main session to zero */
    memset( pmain_session->pSessions, 0, sizeof(NRP_SESSION) * RSNRPZ_MAX_CHANNELS );
    /* the first index contains the main session */
    pmain_session->pSessions[0] = session;

    psession_info = (SSessionInfo*)malloc(sizeof(SSessionInfo));
    memset(psession_info, 0, sizeof(SSessionInfo));
    psession_info->MainSession = (SMainSession *)pmain_session;

    /*
     * Initialize critical section
     */
    InitializeCriticalSection(&(psession_info->cs));
    nb_of_commands = rsnrpz_GetNbOfCommands();
    psession_info->CacheTable = (SValueEntry*) malloc(sizeof(SValueEntry) * nb_of_commands);
    memset(psession_info->CacheTable, 0, sizeof(SValueEntry) * nb_of_commands);

    *pInstrSession = 0;
    AddSessionNode( pmain_session );
    *pInstrSession = N2V( pmain_session );

    /*
     * Set flag IsAvailable for commands:
     *  - SYST:INIT    [ 8/ 2]
     *  - SYST:FEAT?   [ 8/10]
     *  - SYST:LIM?    [ 8/11]
     *  - *IDN         [11/ 3]
     *  - *RST         [11/ 1]
     */
    CHECKERR_CU( rsnrpz_GetIndexFromPacket(8, 2, &index));
    psession_info->CacheTable[index].IsAvailable = 1;
    CHECKERR_CU( rsnrpz_GetIndexFromPacket(8, 10, &index));
    psession_info->CacheTable[index].IsAvailable = 1;
    CHECKERR_CU( rsnrpz_GetIndexFromPacket(8, 11, &index));
    psession_info->CacheTable[index].IsAvailable = 1;
    CHECKERR_CU( rsnrpz_GetIndexFromPacket(11, 3, &index));
    psession_info->CacheTable[index].IsAvailable = 1;
    CHECKERR_CU( rsnrpz_GetIndexFromPacket(11, 1, &index));
    psession_info->CacheTable[index].IsAvailable = 1;

    psession_info->IsPulseMeasurementOn = VI_FALSE;
    psession_info->PulseHighestParamNo  = 0;

    CHECKERR_CU( rsnrpz_InitializeRegisters( pmain_session->NrpRegisters));

    pmain_session->pSInfos[0] = psession_info;

#if !(defined(__APPLE__) || defined(__linux__))
  #pragma warning (push)
  #pragma warning (disable: 4028 )
#endif

    CHECKERR_CU( NrpSetNotifyCallbackDataAvailable(   NrpDataAvailableCallback,  0));
    CHECKERR_CU( NrpSetNotifyCallbackErrorOccurred(   NrpErrorOccurredCallback,  0));
    CHECKERR_CU( NrpSetNotifyCallbackStateChanged(    NrpStateChangedCallback,   0));
    CHECKERR_CU( NrpSetNotifyCallbackCommandAccepted( NrpCommandAcceptedCallback,0));

#if !(defined(__APPLE__) || defined(__linux__))
  #pragma warning (pop)
#endif

    // From here, initialization can continue
    // in parallel threads if necessary
    bLocked = VI_FALSE;
    LeaveCriticalSection( &cs );



    /*- Identification Query ------------------------------------------------*/
    {
        ViChar  buffer[BUFFER_SIZE] = "";

        trace_log( "Sending *IDN" );
        CHECKERR_CU( rsnrpz_ReadValue( *pInstrSession, 1,
                                       (char*)"*IDN",
                                       buffer, sizeof(buffer) ) );

        if ( strncmp( buffer, "??????????", 10) == 0 )
        {
            /* DELL bug, first packet loss */
            CHECKERR_CU( rsnrpz_ReadValue( *pInstrSession, 1,
                                            (char*)"*IDN",
                                            buffer, sizeof(buffer)));

        }

        if( IDQuery )
        {
            if ( !(    strstr(buffer, RSNRPZ_IDN_QUERY)
                    || strstr(buffer, RSNRPZ_IDN_QUERY_311)
                    || strstr(buffer, RSFSH_IDN_QUERY)
                    || strstr(buffer, RSNRPC_IDN_QUERY)
                  )
               )
            {
                CHECKERR_CU( VI_ERROR_FAIL_ID_QUERY );
            }
        }
        sscanf (buffer,"%*[^,],%[^,]",psession_info->SensorType);
    }

    /*- Reset instrument ----------------------------------------------------*/
    if (resetDevice)
        CHECKERR_CU( rsnrpz_chan_reset( *pInstrSession, 1 ) )
    else  /*- Send Default Instrument Setup ---------------------------------*/
        CHECKERR_CU( rsnrpz_defaultInstrSetup( *pInstrSession, 1 ) )

    CHECKERR_CU( rsnrpz_checkStatus( *pInstrSession ) );

    CHECKERR_CU( rsnrpz_SetBitInRegisters( pmain_session,
                                           DEVICE_STATUS_REGISTER_CONNECTED,
                                           CONDITION_REGISTER,
                                           1,
                                           VI_TRUE));
Error:
    trace_log( "%s : Error handling", __FUNC__ );

    if( bLocked )
        LeaveCriticalSection( &cs );

    if ( rsnrpz_status < (ViStatus) NRP_SUCCESS )
    {
        trace_log( "Resetting events..." );
        if ( (pmain_session != 0 ) && pmain_session->event )
            ResetEvent( pmain_session->event );

        trace_log( "Calling NrpSetNotifyCallbackDataAvailable(0,0)" );
        NrpSetNotifyCallbackDataAvailable(   NULL, 0);
        trace_log( "Calling NrpSetNotifyCallbackErrorOccurred(0,0)" );
        NrpSetNotifyCallbackErrorOccurred(   NULL, 0);
        trace_log( "Calling NrpSetNotifyCallbackStateChanged(0,0)" );
        NrpSetNotifyCallbackStateChanged(    NULL, 0);
        trace_log( "Calling NrpSetNotifyCallbackCommandAccepted(0,0)" );
        NrpSetNotifyCallbackCommandAccepted( NULL, 0);

        trace_log( "Clearing session information..." );
        if ( psession_info != 0 )
        {
            if ( psession_info->MainSession )
            {
                DeleteCriticalSection(&(psession_info->cs));
            }

            if ( psession_info->CacheTable != 0 )
                free( psession_info->CacheTable);
            free( psession_info );
        }

        trace_log( "Clearing main session..." );
        if ( pmain_session != 0 )
        {
            if ( pmain_session->pSessions[0] )
                NrpCloseSensor( pmain_session->pSessions[0] );

            memset( pmain_session->pSessions, 0, sizeof(NRP_SESSION) * RSNRPZ_MAX_CHANNELS );

            CloseHandle( pmain_session->event );
            free( pmain_session );
        }
        pmain_session = 0;
        *pInstrSession = 0;

        return rsnrpz_status;
    }

    if ( pmain_session )
    {
        // we must remember the new session in a
        // private array of sessions, because closing
        // the driver MUST NOT be done as long as
        // at least ONE session is still open!!!
        AddSessionNode( pmain_session );
    }

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Initialize                                                      */
/* Purpose:  This function opens the instrument, queries the instrument      */
/*           for its ID, and initializes the instrument to a known state.    */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_init"
ViStatus _VI_FUNC rsnrpz_init(
    ViRsrc      resourceName,
    ViBoolean   IDQuery,
    ViBoolean   resetDevice,
    ViSession*  pInstrSession
)
{
    ViBoolean bLongDist = VI_FALSE;

#if ! defined(__linux__) && ! defined(__APPLE__)

    HKEY      hKey;
    long      lError;

    lError = RegOpenKeyExA( HKEY_LOCAL_MACHINE, SUBKEY, 0, KEY_QUERY_VALUE, &hKey);
    if ( lError == ERROR_SUCCESS )
    {
        long lDist          = 0L;
        unsigned long lSize = sizeof( lDist );

        lError = RegQueryValueExA( hKey, LONGDISTKEY, 0, 0, (unsigned char *)&lDist, &lSize );

        if ( lError == ERROR_SUCCESS )
        {
            bLongDist = (ViBoolean)( lDist != 0L );
        }

        RegCloseKey( hKey );
    }

#endif
    return rsnrpz_private_setup( resourceName, IDQuery, resetDevice, pInstrSession, bLongDist );
}


/*===========================================================================*/
/* Function: Initialize (Long Distance)                                      */
/* Purpose:  This function opens the instrument, queries the instrument      */
/*           for its ID, and initializes the instrument to a known state.    */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_init_long_distance"
ViStatus _VI_FUNC rsnrpz_init_long_distance(
    ViBoolean   IDQuery,
    ViBoolean   resetDevice,
  ViString    resourceName,
  ViSession*  pInstrSession
)
{
    return rsnrpz_private_setup( (ViRsrc)resourceName, IDQuery, resetDevice, pInstrSession, VI_TRUE );
}


/*===========================================================================*/
/* Function: Add Sensor                                                      */
/* Purpose:  This function opens the instrument, queries the instrument      */
/*           for its ID, and initializes the instrument to a known state.    */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_AddSensor"
ViStatus _VI_FUNC rsnrpz_AddSensor(
    ViSession   vi,
    ViInt32     channel,
    ViRsrc      resourceName,
    ViBoolean   IDQuery,
    ViBoolean   resetDevice
)
{
    ViStatus            rsnrpz_status = NRP_SUCCESS;
    NRP_SESSION         session;
    SMainSession*       pmain_session = NULL;
    ViInt32             index, nb_of_commands;
    SSessionInfo*       psession_info = NULL;
    ViBoolean           is_open       = VI_FALSE;
    ViBoolean           is_locked     = VI_FALSE;
    int                 i;

    CHECKSESSION(vi);

    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    /*- Check input parameter ranges ----------------------------------------*/
    if (channel < 2 || channel > (RSNRPZ_MAX_CHANNELS-1) )
        return VI_ERROR_PARAMETER2;
    if (IDQuery != VI_TRUE && IDQuery != VI_FALSE)
        return VI_ERROR_PARAMETER4;
    if (resetDevice != VI_TRUE && resetDevice != VI_FALSE)
        return VI_ERROR_PARAMETER5;

    if ( pmain_session->pSessions[channel - 1] )
        return RSNRPZ_ERROR_SENSOR_ASSIGNED_TO_CHANNEL;

    CHECKERR_CU( NrpOpenSensor( resourceName, &session));

    /*==============================================================*
     * Sensor can be already assigned to channel
     * check if the sensor is assigned. If yes return an error.
     *==============================================================*/
    for (i = 0; i < RSNRPZ_MAX_CHANNELS; i++)
    {
        if ( pmain_session->pSessions[i] == session )
            return RSNRPZ_ERROR_SENSOR_ASSIGNED_TO_CHANNEL;
    }

    /*==============================================================*
     * Sensor is open and we should be assigned to a channel
     *==============================================================*/
    is_open = VI_TRUE;
    pmain_session->pSessions[channel-1] = session;

    psession_info = (SSessionInfo*)malloc(sizeof(SSessionInfo));
    memset(psession_info, 0, sizeof(SSessionInfo));
    psession_info->MainSession = (SMainSession *)V2N(vi);
    /*
     * Initialize critical section
     */
    InitializeCriticalSection(&(psession_info->cs));

    nb_of_commands = rsnrpz_GetNbOfCommands();
    psession_info->CacheTable = (SValueEntry*) malloc(sizeof(SValueEntry) * nb_of_commands);
    memset(psession_info->CacheTable, 0, sizeof(SValueEntry) * nb_of_commands);

    /*
     * Set flag IsAvailable for commands:
     *  - SYST:INIT    [ 8/ 2]
     *  - SYST:FEAT?   [ 8/10]
     *  - SYST:LIM?    [ 8/11]
     *  - *IDN         [11/ 3]
     *  - *RST         [11/ 1]
     */
    CHECKERR_CU( rsnrpz_GetIndexFromPacket(8, 2, &index));
    psession_info->CacheTable[index].IsAvailable = 1;
    CHECKERR_CU( rsnrpz_GetIndexFromPacket(8, 10, &index));
    psession_info->CacheTable[index].IsAvailable = 1;
    CHECKERR_CU( rsnrpz_GetIndexFromPacket(8, 11, &index));
    psession_info->CacheTable[index].IsAvailable = 1;
    CHECKERR_CU( rsnrpz_GetIndexFromPacket(11, 3, &index));
    psession_info->CacheTable[index].IsAvailable = 1;
    CHECKERR_CU( rsnrpz_GetIndexFromPacket(11, 1, &index));
    psession_info->CacheTable[index].IsAvailable = 1;

    psession_info->IsPulseMeasurementOn = VI_FALSE;
    psession_info->PulseHighestParamNo  = 0;

    pmain_session->pSInfos[channel-1] = psession_info;
    //CHECKERR_CU( NrpSetAttribute( session,
    //                              NRP_ATTR_USER_DATA,
    //                              (NRP_SESSION)psession_info));


    /*- Identification Query ------------------------------------------------*/
    {
        ViChar  buffer[BUFFER_SIZE] = "";

        CHECKERR_CU( rsnrpz_ReadValue( vi,
                                       1,
                                       (char*)"*IDN",
                                       buffer,
                                       sizeof( buffer ) ) );

        if ( strncmp( buffer, "??????????", 10) == 0 )
        {
            /* DELL bug, first packet loss */
            CHECKERR_CU( rsnrpz_ReadValue( vi,
                                           1,
                                           (char*)"*IDN",
                                           buffer,
                                           sizeof( buffer ) ) );

        }

        if (IDQuery)
        {
            if ( !(    strstr(buffer, RSNRPZ_IDN_QUERY)
                    || strstr(buffer, RSNRPZ_IDN_QUERY_311)
                    || strstr(buffer, RSFSH_IDN_QUERY)
                    || strstr(buffer, RSNRPC_IDN_QUERY)
                  )
               )
            {
                CHECKERR_CU( VI_ERROR_FAIL_ID_QUERY);
            }
        }
        sscanf (buffer,"%*[^,],%[^,]",psession_info->SensorType);
    }
#if 0
    /*- Reset instrument ----------------------------------------------------*/
    if (resetDevice)
        CHECKERR_CU( rsnrpz_chan_reset( vi /*(ViSession)pmain_session*/, channel));
    else  /*- Send Default Instrument Setup ---------------------------------*/
#endif
        CHECKERR_CU( rsnrpz_defaultInstrSetup( vi, channel));

    CHECKERR_CU( rsnrpz_checkStatus( vi ) );

    is_locked = VI_TRUE;
    trace_log( "%s : EnterCriticalSection()", __FUNC__ );
    EnterCriticalSection(&(pmain_session->cs));

    CHECKERR_CU( rsnrpz_SetBitInRegisters( pmain_session,
                                           DEVICE_STATUS_REGISTER_CONNECTED,
                                           CONDITION_REGISTER,
                                           channel,
                                           VI_TRUE));

Error:
    if ( is_locked )
    {
        trace_log( "%s : LeaveCriticalSection()", __FUNC__ );
        LeaveCriticalSection(&(pmain_session->cs));
    }
    if ( rsnrpz_status < (ViStatus) NRP_SUCCESS )
    {
        if ( psession_info != NULL )
        {
            DeleteCriticalSection(&(psession_info->cs));

            if ( psession_info->CacheTable != NULL )
                free( psession_info->CacheTable);
            free(psession_info);
        }
        if ( is_open )
        {
            NrpCloseSensor(pmain_session->pSessions[channel-1]);
            pmain_session->pSessions[channel-1] = 0;
        }
    }

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Abort                                                           */
/* Purpose:  This function immediately sets all the sensors to the IDLE      */
/*           state.                                                          */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chans_abort"
ViStatus _VI_FUNC rsnrpz_chans_abort(
    ViSession vi
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKSESSION(vi);

    CHECKERR( rsnrpz_WriteCommand( vi, ALL_CHANNELS, (char*)"ABOR", NULL) );
    CHECKERR( rsnrpz_checkStatus ( vi));
    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Count                                                           */
/* Purpose:  This function returns number of available channels.             */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chans_getCount"
ViStatus _VI_FUNC rsnrpz_chans_getCount(
    ViSession   vi,
    ViInt32*    count
)
{
    ViInt32      i;
    ViInt32      sensor_count     = 0;
    SMainSession *pmain_session   = 0;

    CHECKSESSION(vi);

    pmain_session = V2N( vi );
    if ( pmain_session )
    {
        for ( i=0; i < RSNRPZ_MAX_CHANNELS; i++ )
        {
            if ( pmain_session->pSessions[i] != 0 )
                sensor_count++;
        }
    }

    if ( count )
        *count = sensor_count;

    return VI_SUCCESS;
}

/*===========================================================================*/
/* Function: Initiate                                                        */
/* Purpose:  This function starts a single-shot measurement on all channels. */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chans_initiate"
ViStatus _VI_FUNC rsnrpz_chans_initiate(
    ViSession vi
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKSESSION(vi);

    CHECKERR( rsnrpz_WriteCommand( vi, ALL_CHANNELS, (char*)"INIT:IMM", NULL));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Zero                                                            */
/* Purpose:  This function performs zeroing using the signal at the sensor   */
/*           input.                                                          */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chans_zero"
ViStatus _VI_FUNC rsnrpz_chans_zero(
    ViSession vi
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value         = RSNRPZ_ONCE;

    CHECKSESSION(vi);

    CHECKERR( rsnrpz_WriteCommand( vi, ALL_CHANNELS, (char*)"CAL:ZERO:AUTO", &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Zeroing Complete State                                      */
/* Purpose:  This function returns the state of the measurement.             */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chans_isZeroingComplete"
ViStatus _VI_FUNC rsnrpz_chans_isZeroingComplete(
    ViSession   vi,
    ViBoolean*  pIsZeroComplete
)
{
    ViBoolean       is_zero_complete = VI_TRUE;
    ViStatus        rsnrpz_status    = VI_SUCCESS;
    ViInt32         i;
    TRegister       cal_register;
    ViUInt32*       pregister_value;
    SMainSession*   pmain_session = 0;

    CHECKSESSION(vi);

    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    CHECKERR( rsnrpz_QueryRegister( vi,
                                    OPERATION_CALIBRATING_STATUS_REGISTER,
                                    CONDITION_REGISTER,
                                    &cal_register));
    pregister_value = cal_register;

    for (i = 1; i <= RSNRPZ_MAX_CHANNELS && is_zero_complete; i++)
    {
        if ( pmain_session->pSessions[i-1] )
            is_zero_complete = (ViBoolean)(((*pregister_value) & (1L << i)) == 0);
    }

    if ( pIsZeroComplete )
        *pIsZeroComplete = is_zero_complete;

//    return rsnrpz_status;
    return rsnrpz_checkStatus(vi);
}

/*===========================================================================*/
/* Function: Get Measurement Complete State                                  */
/* Purpose:  This function returns the state of the measurement.             */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chans_isMeasurementComplete"
ViStatus _VI_FUNC rsnrpz_chans_isMeasurementComplete(
    ViSession   vi,
    ViBoolean*  pIsMeasurementComplete
)
#if 1
{
    ViStatus        rsnrpz_status    = VI_SUCCESS;
    ViBoolean       is_meas_complete = VI_TRUE;
    ViInt32         i                = 0;
    SMainSession*   pmain_session    = 0;

    CHECKSESSION(vi);

    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    /*
     * Check all connected sensors if data are available
     */
    for (i = 1; i <= RSNRPZ_MAX_CHANNELS && is_meas_complete; i++)
    {
        if (pmain_session->pSessions[i-1])
        {
            SSessionInfo* psession_info;
            psession_info = pmain_session->pSInfos[i-1];

            EnterCriticalSection( &psession_info->cs );
            if ( psession_info->IsInStatMode == VI_TRUE )
            {
                if ( (psession_info->IsMeasurementComplete != VI_TRUE)
                  || (psession_info->IsArrayMeasValid      != VI_TRUE)
                   )
                    is_meas_complete = VI_FALSE;
            }
            else if ( psession_info->IsPulseMeasurementOn == VI_TRUE )
            {
                if ( (psession_info->IsMeasurementComplete  != VI_TRUE)
                  || (psession_info->IsPulseScalarsComplete != VI_TRUE)
                   )
                    is_meas_complete = VI_FALSE;
            }
            else
            {
                if ( psession_info->IsMeasurementComplete != VI_TRUE )
                    is_meas_complete = VI_FALSE;
            }

            LeaveCriticalSection( &psession_info->cs );
        }
    }

    if ( is_meas_complete )
    {
        /*
         * Clear isMeasComplete for all sensors
         */
        trace_log( "chans_IsMeasurementComplete = TRUE" );

        if ( pmain_session->pSessions[0] )
        {
            SSessionInfo* psession_info;
            psession_info = pmain_session->pSInfos[0];

            EnterCriticalSection(&psession_info->cs);
            psession_info->IsMeasurementComplete  = VI_FALSE;
            psession_info->IsPulseScalarsComplete = VI_FALSE;
            LeaveCriticalSection(&psession_info->cs);
        }
    }

    if ( pIsMeasurementComplete )
        *pIsMeasurementComplete = is_meas_complete;

//Error:
    if (rsnrpz_status < VI_SUCCESS && pIsMeasurementComplete)
        *pIsMeasurementComplete = VI_FALSE;
    return rsnrpz_status;
}

#else
{
    ViStatus        rsnrpz_status    = VI_SUCCESS;
    ViBoolean       is_meas_complete = VI_TRUE;
    ViBoolean       is_locked        = VI_FALSE;
    ViInt32         i;
    SMainSession*   pmain_session    = 0;
    TRegister       meas_register;
    TRegister       trigger_register;
    ViUInt32*       pregister_value;

    CHECKSESSION(vi);

    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    is_locked = VI_TRUE;
    trace_log( "%s : EnterCriticalSection()", __FUNC__ );
    EnterCriticalSection(&pmain_session->cs);

    CHECKERR_CU( rsnrpz_QueryRegister( vi,
                                       OPERATION_TRIGGER_STATUS_REGISTER,
                                       CONDITION_REGISTER,
                                       &trigger_register));
    CHECKERR_CU( rsnrpz_QueryRegister( vi,
                                       OPERATION_MEASURING_STATUS_REGISTER,
                                       CONDITION_REGISTER,
                                       &meas_register));
    trace_log( "%s : LeaveCriticalSection()", __FUNC__ );
    LeaveCriticalSection(&pmain_session->cs);
    is_locked = VI_FALSE;

    /*
     * Try check if any measurement bit is set
     */
    pregister_value = meas_register;

    for (i = 1; i <= RSNRPZ_MAX_CHANNELS && is_meas_complete; i++)
    {
        if ( pmain_session->pSessions[i-1] )
            is_meas_complete = (ViBoolean)( ((*pregister_value) & (1L << i)) == 0 );
    }

    if ( is_meas_complete )
    {
        /*
         * Try check if any trigger bit is set
         */
        pregister_value = trigger_register;

        for (i = 1; i <= RSNRPZ_MAX_CHANNELS && is_meas_complete; i++)
        {
            if (pmain_session->pSessions[i-1])
                is_meas_complete = (ViBoolean)( ((*pregister_value) & (1L << i)) == 0 );
        }
    }

    if ( pIsMeasurementComplete )
        *pIsMeasurementComplete = is_meas_complete;

Error:
    if ( is_locked )
    {
        trace_log( "%s : LeaveCriticalSection()", __FUNC__ );
        LeaveCriticalSection(&pmain_session->cs);
    }
    if (rsnrpz_status < VI_SUCCESS && pIsMeasurementComplete)
        *pIsMeasurementComplete = VI_FALSE;

    return rsnrpz_status;
}
#endif

/*===========================================================================*/
/* Function: Measurement Mode                                                */
/* Purpose:  This function sets the sensor to one of the measurement modes.  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_mode"
ViStatus _VI_FUNC rsnrpz_chan_mode(
    ViSession   vi,
    ViInt32     channel,
    ViInt32     measurementMode
)
{
    ViStatus     rsnrpz_status  = VI_SUCCESS;
    ViInt32      meas_function  = measurementMode;
    ViInt32      buffer_state   = VI_FALSE;
    SMainSession *pmain_session = 0;

    CHECKCHANNEL(vi, channel);

    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    {
        SSessionInfo* psession_info;
        psession_info = pmain_session->pSInfos[channel-1];

        if ( (measurementMode == RSNRPZ_SENSOR_MODE_CCDF)
          || (measurementMode == RSNRPZ_SENSOR_MODE_PDF) )
            psession_info->IsInStatMode = VI_TRUE;
        else
            psession_info->IsInStatMode = VI_FALSE;

        if ( measurementMode == RSNRPZ_SENSOR_MODE_BUF_CONTAV )
        {
            buffer_state  = VI_TRUE;
            meas_function = RSNRPZ_SENSOR_MODE_CONTAV ;

            CHECKERR( rsnrpz_WriteCommand( vi,
                                           channel,
                                           (char*)"SENS:FUNC",
                                           &meas_function));
            CHECKERR( rsnrpz_WriteCommand( vi,
                                           channel,
                                           (char*)"SENS:POW:AVG:BUFF:STAT",
                                           &buffer_state));
        }
        else
        {
            CHECKERR( rsnrpz_WriteCommand( vi,
                                           channel,
                                           (char*)"SENS:POW:AVG:BUFF:STAT",
                                           &buffer_state));
            CHECKERR( rsnrpz_WriteCommand( vi,
                                           channel,
                                           (char*)"SENS:FUNC",
                                           &meas_function));
        }
    }
    CHECKERR( rsnrpz_checkStatus ( vi));

//Error:
    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Dropout Time                                                */
/* Purpose:  This function defines the dropout time value.                   */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_setDropoutTime"
ViStatus _VI_FUNC rsnrpz_trigger_setDropoutTime (ViSession vi,
                                                 ViInt32 channel,
                                                 ViReal64 dropoutTime)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value         = (float)dropoutTime;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"TRIG:DTIM",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Dropout Time                                                */
/* Purpose:  This function queries the dropout time value.                   */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_getDropoutTime"
ViStatus _VI_FUNC rsnrpz_trigger_getDropoutTime (ViSession vi,
                                                 ViInt32 channel,
                                                 ViReal64 *dropoutTime)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value         = 0.0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"TRIG:DTIM",
                                &value, sizeof(value)));

    if ( dropoutTime )
        *dropoutTime = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Trigger Master State                                        */
/* Purpose:  This function                                                   */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_setMasterState"
ViStatus _VI_FUNC rsnrpz_trigger_setMasterState (ViSession vi,
                                        ViInt32 channel, ViBoolean state)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = (ViInt32)state;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"TRIG:MAST:STAT",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Trigger Master State                                        */
/* Purpose:  This function                                                   */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_getMasterState"
ViStatus _VI_FUNC rsnrpz_trigger_getMasterState (ViSession vi,
                                        ViInt32 channel, ViBoolean *state)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"TRIG:MAST:STAT",
                                &value, sizeof(value)));

    if ( state )
        *state = (ViBoolean)value;
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Trigger Synchronization State                               */
/* Purpose:  This function                                                   */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_setSyncState"
ViStatus _VI_FUNC rsnrpz_trigger_setSyncState (ViSession vi,
                                      ViInt32 channel, ViBoolean state)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = (ViInt32)state;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"TRIG:SYNC:STAT",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Trigger Synchronization State                               */
/* Purpose:  This function                                                   */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_getSyncState"
ViStatus _VI_FUNC rsnrpz_trigger_getSyncState (ViSession vi,
                                      ViInt32 channel, ViBoolean *state)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"TRIG:SYNC:STAT",
                                &value, sizeof(value)));

    if ( state )
        *state = (ViBoolean)value;
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Sensor Info                                                     */
/* Purpose:  This function returns selected information on a sensor.         */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_info"
ViStatus _VI_FUNC rsnrpz_chan_info(
    ViSession       vi,
    ViInt32         channel,
    ViString        infoType,
    ViInt32         bufferSize,
    ViChar _VI_FAR  data[]
)
{
    ViInt32     i;
    ViStatus    rsnrpz_status            = VI_SUCCESS;
    ViChar      buffer[BUFFER_SIZE]      = "";
    ViChar*     pbuffer                  = buffer;
    ViChar*     pdynamic_buffer          = NULL;
    const char*  s_valid_info_types[] = {
                                            "Manufacturer",
                                            "Type",
                                            "Stock Number",
                                            "Serial",
                                            "HWVersion",
                                            "HWVariant",
                                            "SW Build",
                                            "FPGA Build",
                                            "Technology",
                                            "Function",
                                            "MinPower",
                                            "MaxPower" ,
                                            "MinFreq",
                                            "MaxFreq",
                                            "Resolution",
                                            "Impedance",
                                            "Coupling",
                                            "Cal. Abs.",
                                            "Cal. Refl.",
                                            "Cal. S-Para.",
                                            "Cal. Misc.",
                                            "Cal. Temp.",
                                            "Cal. Lin.",
                                            "SPD Mnemonic",
                                            "Uptime",
                                            "",
                                            NULL
                                        };

    CHECKCHANNEL(vi, channel);

    for ( i = 0; infoType && s_valid_info_types[i]; i++)
    {
        if (STRICMP(infoType, s_valid_info_types[i]) == 0)
            break;
    }
    if ( infoType == NULL || (!s_valid_info_types[i])  )
        return VI_ERROR_PARAMETER2;

    // special case for sensor uptime
    // ------------------------------
    if ( s_valid_info_types[i] && ! strcmp( s_valid_info_types[i], "Uptime" ) )
        strcpy( buffer, s_valid_info_types[i] );

    rsnrpz_status = rsnrpz_ReadValue( vi,
                                      channel,
                                      (char*)"SYST:INFO",
                                      buffer, sizeof(buffer));

    if ( s_valid_info_types[i] && ! strcmp( s_valid_info_types[i], "Uptime" ) && (rsnrpz_status == VI_SUCCESS) )
    {
        strncpy( data, buffer, bufferSize );
        data[ bufferSize-1 ] = 0x00;
        return VI_SUCCESS;
    }

    if ( rsnrpz_status > 0 )
    {
        pdynamic_buffer = (ViChar*) malloc(rsnrpz_status);
        memset(pdynamic_buffer, 0, rsnrpz_status);
        pbuffer = pdynamic_buffer;

        rsnrpz_status = rsnrpz_ReadValue( vi,
                                          channel,
                                          (char*)"SYST:INFO",
                                          pdynamic_buffer,
                                          rsnrpz_status);

    }
    CHECKERR_CU( rsnrpz_status );

    if (strlen(infoType)==0)
    {
        /* return entire answer if infoType is the empty string */
        strncpy( data, pbuffer, bufferSize - 1);
    }
    else
    {
        pbuffer = (ViChar *)stristr( pbuffer, infoType );
        if ( pbuffer )
        {
            ViChar*     pend = NULL;

            pbuffer += strlen(infoType) + 1;

            pend = strchr(pbuffer, '\n');
            if (pend)
            {
                while (pbuffer < pend && isspace(*(pend-1)) )
                    pend--;
                *pend = '\0';
            }

            if ( bufferSize < 0 )
                strcpy( data, pbuffer);
            else if ( bufferSize < (int)(strlen( pbuffer) + 1) )
            {
                rsnrpz_status = (int)strlen(pbuffer) + 1;

                if ( (bufferSize - 1) > 0 && data)
                {
                    strncpy( data, pbuffer, bufferSize - 1);
                }
            }
            else
            {
                strcpy( data, pbuffer);
            }
        }
        else
        {
            return VI_ERROR_PARAMETER2;
        }
    }

    if ( rsnrpz_status == VI_SUCCESS )
    {
        /*
         * Don't overwrite required buffer size
         */
        CHECKERR( rsnrpz_checkStatus ( vi));
    }

Error:
    if ( pdynamic_buffer != NULL )
        free(pdynamic_buffer);

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Sensors Info Header                                             */
/* Purpose:  This function returns specified parameter header which can be   */
/*           retrieved from selected sensor.                                 */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_infoHeader"
ViStatus _VI_FUNC rsnrpz_chan_infoHeader(
    ViSession       vi,
    ViInt32         channel,
    ViInt32         parameterNb,
    ViInt32         bufferSize,
    ViChar _VI_FAR  data[]
)
{
    ViStatus    rsnrpz_status       = VI_SUCCESS;
    ViChar      buffer[BUFFER_SIZE] = "";
    ViChar*     pbuffer             = buffer;
    ViChar*     pend                = buffer;
    ViChar*     pdynamic_buffer     = NULL;
    ViInt32     index;

    CHECKCHANNEL(vi, channel);

    if ( parameterNb < 0 )
        return VI_ERROR_PARAMETER3;


    rsnrpz_status = rsnrpz_ReadValue( vi,
                                      channel,
                                      (char*)"SYST:INFO",
                                      buffer, sizeof(buffer));

    if ( rsnrpz_status > 0 )
    {
        pdynamic_buffer = (ViChar*) malloc(rsnrpz_status);
        memset(pdynamic_buffer, 0, rsnrpz_status);
        pbuffer = pdynamic_buffer;

        rsnrpz_status = rsnrpz_ReadValue( vi,
                                          channel,
                                          (char*)"SYST:INFO",
                                          pdynamic_buffer,
                                          rsnrpz_status);

    }
    CHECKERR_CU( rsnrpz_status );

    index = 0;
    do
    {
        pend = strchr(pbuffer, '\n');
        if ( parameterNb == index )
        {
            while (pbuffer < pend && isspace(*(pend-1)))
                pend--;
            *pend = '\0';

            if ( bufferSize < 0 )
                strcpy(data, pbuffer);
            else if ( bufferSize < (int)(strlen(pbuffer) + 1) )
            {
                rsnrpz_status = (int)strlen(pbuffer) + 1;

                if ( (bufferSize - 1) > 0 && data)
                {
                    strncpy( data, pbuffer, bufferSize - 1);
                }
            }
            else
                strcpy(data, pbuffer);

            break;
        }

        pbuffer = pend;
        if ( pbuffer )
        {
            pbuffer++;
            index++;
        }
    } while( pbuffer && *pbuffer );

    if ( pbuffer == NULL )
        return VI_ERROR_PARAMETER3;

    if ( rsnrpz_status == VI_SUCCESS )
    {
        /*
         * Don't overwrite required buffer size
         */
        CHECKERR( rsnrpz_checkStatus ( vi));
    }

Error:
    if ( pdynamic_buffer != NULL )
        free(pdynamic_buffer);

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Sensors Info Count                                              */
/* Purpose:  This function returns the number of info headers for selected   */
/*           channel.                                                        */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_infosCount"
ViStatus _VI_FUNC rsnrpz_chan_infosCount(
    ViSession       vi,
    ViInt32         channel,
    ViInt32*        pCount
)
{
    ViStatus    rsnrpz_status       = VI_SUCCESS;
    ViChar      buffer[BUFFER_SIZE] = "";
    ViChar*     pbuffer             = buffer;
    ViChar*     pdynamic_buffer     = NULL;
    ViInt32     index               = 0;

    CHECKCHANNEL(vi, channel);

    rsnrpz_status = rsnrpz_ReadValue( vi,
                                      channel,
                                      (char*)"SYST:INFO",
                                      buffer, sizeof(buffer));

    if ( rsnrpz_status > 0 )
    {
        pdynamic_buffer = (ViChar*)malloc( rsnrpz_status );
        memset(pdynamic_buffer, 0, rsnrpz_status);
        pbuffer = pdynamic_buffer;

        rsnrpz_status = rsnrpz_ReadValue( vi,
                                          channel,
                                          (char*)"SYST:INFO",
                                          pdynamic_buffer,
                                          rsnrpz_status);

    }
    CHECKERR_CU( rsnrpz_status );

    index = 0;
    do
    {
        pbuffer = strchr(pbuffer, '\n');
        if ( pbuffer )
        {
            pbuffer++;
            index++;
        }
    } while( pbuffer && *pbuffer );

/* Changed by J.K.
 *  index++;
 */

    CHECKERR( rsnrpz_checkStatus ( vi));

Error:
    if ( pdynamic_buffer != NULL )
        free(pdynamic_buffer);
    if ( pCount )
        *pCount = index;

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Configure Exclude                                               */
/* Purpose:  This function configures times that is to be excluded at the    */
/*           beginning and at the end of the integration.                    */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_timing_configureExclude"
ViStatus _VI_FUNC rsnrpz_timing_configureExclude(
    ViSession   vi,
    ViInt32     channel,
    ViReal64    excludeStart,
    ViReal64    excludeStop
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_timing_setTimingExcludeStart( vi,
                                                   channel,
                                                   excludeStart));
    CHECKERR( rsnrpz_timing_setTimingExcludeStop( vi,
                                                  channel,
                                                  excludeStop));

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Timing Exclude Start                                        */
/* Purpose:  This function sets a time that is to be excluded at the         */
/*           beginning of the integration.                                   */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_timing_setTimingExcludeStart "
ViStatus _VI_FUNC rsnrpz_timing_setTimingExcludeStart (
    ViSession       vi,
    ViInt32         channel,
    ViReal64        excludeStart
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value         = (float)excludeStart;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 3, 15 ) )  //  3/15  =  SENS:TIM:EXCL:STAR
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_WriteCommand( vi, channel, (char*)"SENS:TIM:EXCL:STAR", &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Timing Exclude Start                                        */
/* Purpose:  This function reads a time that is to be excluded at the        */
/*           beginning of the integration.                                   */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_timing_getTimingExcludeStart"
ViStatus _VI_FUNC rsnrpz_timing_getTimingExcludeStart (
    ViSession   vi,
    ViInt32     channel,
    ViReal64*   excludeStart
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value         = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 3, 15 ) )  //  3/15  =  SENS:TIM:EXCL:STAR
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TIM:EXCL:STAR",
                                &value, sizeof(value)));

    if ( excludeStart )
        *excludeStart = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Timing Exclude Stop                                         */
/* Purpose:  This function sets a time that is to be excluded at the end of  */
/*           the integration.                                                */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_timing_setTimingExcludeStop"
ViStatus _VI_FUNC rsnrpz_timing_setTimingExcludeStop (
    ViSession   vi,
    ViInt32     channel,
    ViReal64    excludeStop
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value         = (float)excludeStop;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 3, 16 ) )  //  3/16  =  SENS:TIM:EXCL:STOP
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_WriteCommand( vi, channel, (char*)"SENS:TIM:EXCL:STOP", &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Timing Exclude Stop                                         */
/* Purpose:  This function reads a time that is to be excluded at the end of */
/*           the integration.                                                */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_timing_getTimingExcludeStop"
ViStatus _VI_FUNC rsnrpz_timing_getTimingExcludeStop(
    ViSession   vi,
    ViInt32     channel,
    ViReal64*   excludeStop
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value         = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 3, 16 ) )  //  3/16  =  SENS:TIM:EXCL:STOP
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TIM:EXCL:STOP",
                                &value, sizeof(value)));

    if ( excludeStop )
        *excludeStop = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Video Bandwidth                                             */
/* Purpose:  This function sets the video bandwidth according to a list of   */
/*           available bandwidths.                                           */
/*===========================================================================*/
#if !defined (__GNUC__)
#pragma warning( push )
#pragma warning( disable : 4706 )
#endif

#undef  __FUNC__
#define __FUNC__ "rsnrpz_bandwidth_setBw"
ViStatus _VI_FUNC rsnrpz_bandwidth_setBw (ViSession vi,
                                          ViInt32 channel, ViInt32 bandwidth)
{
    ViChar      buffer[BUFFER_SIZE];
    ViChar*     p2buf         = buffer;
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     i;
    ViInt32     index, count  = 0;
    SpecRangeTableEntry* local_table;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr( vi, channel, (char*)"NRP-Z81" ) )
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 2, 1 ) )  //  2/1  =  SENS:BWID:VID
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR (rsnrpz_bandwidth_getBwList (vi, channel, BUFFER_SIZE, buffer));
    CHECKERR (rsnrpz_GetIndexFromCommand ((char*)"SENS:BWID:VID", &index));

    if ( strlen( buffer ) )
    {
        count = 1;
        p2buf = strchr( p2buf, ',' );

        while ( p2buf )
        {
            count++;
            p2buf++;
            p2buf = strchr( p2buf, ',' );
        }
    }
    else
        return RSNRPZ_ERROR_INTERPRETING_RESPONSE;

    local_table = (SpecRangeTableEntry*) malloc (sizeof(SpecRangeTableEntry) * (count + 1));

    p2buf = strtok (buffer,",");
    for (i = 0; i < count; i++)
    {
        local_table[i].bit = (1L << i);
        local_table[i].parameter = (ViChar *) malloc (strlen (p2buf) + 5);
//        strcpy (local_table[i].parameter,p2buf);
        sprintf (local_table[i].parameter, "\"%s\"", p2buf);
        local_table[i].value = i;
        p2buf = strtok (NULL,",");
    }

    local_table[i].bit = 0;
    local_table[i].parameter = NULL;
    local_table[i].value = 0;

    gs_NrpCommands[index].RangeTable = (SRangeTableEntry*) local_table;

    i = bandwidth;

    rsnrpz_status = rsnrpz_WriteCommand( vi, channel, (char*)"SENS:BWID:VID", &i);

    for (i = 0; i < count; i++)
    {
        free (local_table[i].parameter);
    }

    free (local_table);

    gs_NrpCommands[index].RangeTable = VI_NULL;

    CHECKERR( rsnrpz_status );
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}
#if !defined (__GNUC__)
#pragma warning( pop )
#endif

/*===========================================================================*/
/* Function: Get Video Bandwidth                                             */
/* Purpose:  This function queries the video bandwidth according to a list of*/
/*           available bandwidths.                                           */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_bandwidth_getBw"
ViStatus _VI_FUNC rsnrpz_bandwidth_getBw (ViSession vi,
                                          ViInt32 channel, ViInt32 *bandwidth)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViChar      buffer[BUFFER_SIZE];
    ViChar*     p2buf = buffer;
    ViInt32     i, value;
    ViInt32     index, count = 0;
    SpecRangeTableEntry* local_table;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr( vi, channel, (char*)"NRP-Z81" ) )
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 2, 1 ) )  //  2/1  =  SENS:BWID:VID
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR (rsnrpz_bandwidth_getBwList (vi, channel, BUFFER_SIZE, buffer));
    CHECKERR (rsnrpz_GetIndexFromCommand ((char*)"SENS:BWID:VID", &index));

    if (strlen (buffer))
    {
        count = 1;
        p2buf = strchr( p2buf, ',' );

        while ( p2buf )
        {
            count++;
            p2buf++;
            p2buf = strchr( p2buf, ',' );
        }
    }
    else
        return RSNRPZ_ERROR_INTERPRETING_RESPONSE;

    local_table = (SpecRangeTableEntry*) malloc (sizeof(SpecRangeTableEntry) * (count + 1));

    p2buf = strtok (buffer,",");
    for (i = 0; i < count; i++)
    {
        local_table[i].bit = (1L << i);
        local_table[i].parameter = (ViChar *) malloc (strlen (p2buf) + 1);
        strcpy (local_table[i].parameter,p2buf);
        local_table[i].value = i;
        p2buf = strtok (NULL,",");
    }

    local_table[i].bit = 0;
    local_table[i].parameter = NULL;
    local_table[i].value = 0;

    gs_NrpCommands[index].RangeTable = (SRangeTableEntry*) local_table;

    rsnrpz_status = rsnrpz_ReadValue( vi, channel, (char*)"SENS:BWID:VID", &value, sizeof (value));

    for (i = 0; i < count; i++)
    {
        free (local_table[i].parameter);
    }

    free (local_table);
    gs_NrpCommands[index].RangeTable = VI_NULL;

    CHECKERR( rsnrpz_status );
    CHECKERR( rsnrpz_checkStatus ( vi));

    *bandwidth = value;

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Video Bandwidth List                                        */
/* Purpose:  This function queries the list of possible video bandwidths.    */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_bandwidth_getBwList"
ViStatus _VI_FUNC rsnrpz_bandwidth_getBwList (ViSession vi,
                                              ViInt32 channel,
                                              ViInt32 bufferSize,
                                              ViChar bandwidthList[])
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViChar      buffer [BUFFER_SIZE];
    ViChar      buffer2[BUFFER_SIZE];
    ViChar      buffer3[BUFFER_SIZE] = "";
    ViChar      *p2buf;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:BWID:VID:LIST",
                                buffer,
                                BUFFER_SIZE));


    CHECKERR( rsnrpz_checkStatus ( vi));

    p2buf = buffer;
    while (p2buf && *p2buf)
    {
        sscanf (p2buf, "%[^\r\n]", buffer2);
        strcat (buffer3, buffer2);
        strcat (buffer3, ",");
        p2buf = strstr (p2buf,"\r\n") + 2;
    }

    buffer3[strlen(buffer3) -1] = '\0';

    strncpy (bandwidthList, buffer3, bufferSize);

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Configure Averaging Auto                                        */
/* Purpose:  This function configures all parameters necessary for automatic */
/*           detection of filter bandwidth.                                  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_avg_configureAvgAuto"
ViStatus _VI_FUNC rsnrpz_avg_configureAvgAuto(
    ViSession   vi,
    ViInt32     channel,
    ViInt32     resolution
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR(rsnrpz_avg_setAutoResolution(vi, channel, resolution));

    CHECKERR(rsnrpz_avg_setAutoType (vi, channel,
                                    RSNRPZ_AUTO_COUNT_TYPE_RESOLUTION));

    CHECKERR(rsnrpz_avg_setTerminalControl (vi,
                                           channel,
                                           RSNRPZ_TERMINAL_CONTROL_REPEAT));

    CHECKERR(rsnrpz_avg_setAutoEnabled(vi,channel,VI_TRUE));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Configure Averaging NS Ratio                                    */
/* Purpose:  This function configures all parameters necessary for setting   */
/*           the noise ratio in the measurement result and automatic         */
/*           detection of filter bandwidth.                                  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_avg_configureAvgNSRatio"
ViStatus _VI_FUNC rsnrpz_avg_configureAvgNSRatio(
    ViSession   vi,
    ViInt32     channel,
    ViReal64    maximumNoiseRatio,
    ViReal64    upperTimeLimit
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_avg_setAutoNoiseSignalRatio( vi,
                                                 channel,
                                                 maximumNoiseRatio));

    CHECKERR( rsnrpz_avg_setAutoMaxMeasuringTime( vi,
                                                 channel,
                                                 upperTimeLimit));

    CHECKERR(rsnrpz_avg_setAutoType (vi, channel,
                                    RSNRPZ_AUTO_COUNT_TYPE_NSR));

    CHECKERR(rsnrpz_avg_setTerminalControl (vi,
                                           channel,
                                           RSNRPZ_TERMINAL_CONTROL_REPEAT));

    CHECKERR(rsnrpz_avg_setAutoEnabled(vi,channel,VI_TRUE));

    return rsnrpz_status;
}




/*===========================================================================*/
/* Function: Configure Averaging Manual                                      */
/* Purpose:  This function configures all parameters necessary for manual    */
/*           setting of filter bandwidth.                                    */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_avg_configureAvgManual"
ViStatus _VI_FUNC rsnrpz_avg_configureAvgManual(
    ViSession   vi,
    ViInt32     channel,
    ViInt32     count
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_avg_setAutoEnabled( vi,
                                         channel,
                                         VI_FALSE));
    CHECKERR( rsnrpz_avg_setCount( vi,channel,count));

    CHECKERR( rsnrpz_avg_setTerminalControl( vi,
                                             channel,
                                             RSNRPZ_TERMINAL_CONTROL_REPEAT));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Auto Averaging State                                        */
/* Purpose:  This function can be used to automatically determine a value for*/
/*           filter bandwidth.                                               */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_avg_setAutoEnabled"
ViStatus _VI_FUNC rsnrpz_avg_setAutoEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean   autoEnabled
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value         = autoEnabled;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi, channel, (char*)"SENS:AVER:COUN:AUTO", &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Auto Averaging State                                        */
/* Purpose:  This function queries the setting of automatic switchover of    */
/*           filter bandwidth.                                               */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_avg_getAutoEnabled"
ViStatus _VI_FUNC rsnrpz_avg_getAutoEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean*  pAutoEnabled
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:AVER:COUN:AUTO",
                                &value, sizeof(value)));

    if ( pAutoEnabled )
        *pAutoEnabled = (ViBoolean)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Auto Max Measuring Time                                     */
/* Purpose:  This function sets an upper time limit can be set.              */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_avg_setAutoMaxMeasuringTime"
ViStatus _VI_FUNC rsnrpz_avg_setAutoMaxMeasuringTime(
    ViSession   vi,
    ViInt32     channel,
    ViReal64    upperTimeLimit
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)upperTimeLimit;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:AVER:COUN:AUTO:MTIM",
                                   &value));

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Auto Max Measuring Time                                     */
/* Purpose:  This function queries an upper time limit.                      */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_avg_getAutoMaxMeasuringTime"
ViStatus _VI_FUNC rsnrpz_avg_getAutoMaxMeasuringTime(
    ViSession   vi,
    ViInt32     channel,
    ViReal64*   upperTimeLimit
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:AVER:COUN:AUTO:MTIM",
                                &value, sizeof(value)));

    if ( upperTimeLimit )
        *upperTimeLimit = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Auto Noise Signal Ratio                                     */
/* Purpose:  This function sets the maximum noise ratio in the measurement   */
/*           result.                                                         */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_avg_setAutoNoiseSignalRatio"
ViStatus _VI_FUNC rsnrpz_avg_setAutoNoiseSignalRatio(
    ViSession   vi,
    ViInt32     channel,
    ViReal64    maximumNoiseRatio
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)maximumNoiseRatio;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:AVER:COUN:AUTO:NSR",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Auto Noise Signal Ratio                                     */
/* Purpose:  This function queries the maximum noise signal ratio value.     */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_avg_getAutoNoiseSignalRatio"
ViStatus _VI_FUNC rsnrpz_avg_getAutoNoiseSignalRatio(
    ViSession   vi,
    ViInt32     channel,
    ViReal64*   maximumNoiseRatio
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:AVER:COUN:AUTO:NSR",
                                &value, sizeof(value)));

    if ( maximumNoiseRatio )
        *maximumNoiseRatio = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Auto Resolution                                             */
/* Purpose:  This function defines the number of significant places          */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_avg_setAutoResolution"
ViStatus _VI_FUNC rsnrpz_avg_setAutoResolution(
    ViSession   vi,
    ViInt32     channel,
    ViInt32     resolution
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:AVER:COUN:AUTO:RES",
                                   &resolution));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Auto Resolution                                             */
/* Purpose:  This function reads the number of significant places            */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_avg_getAutoResolution"
ViStatus _VI_FUNC rsnrpz_avg_getAutoResolution(
    ViSession   vi,
    ViInt32     channel,
    ViInt32*    resolution
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:AVER:COUN:AUTO:RES",
                                &value, sizeof(value)));

    if ( resolution )
        *resolution = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Auto Type                                                   */
/* Purpose:  This function selects a method by which the automatic filter    */
/*           length switchover can operate.                                  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_avg_setAutoType"
ViStatus _VI_FUNC rsnrpz_avg_setAutoType(
    ViSession   vi,
    ViInt32     channel,
    ViInt32     method
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:AVER:COUN:AUTO:TYPE",
                                   &method));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Auto Type                                                   */
/* Purpose:  This function returns a method by which the automatic filter    */
/*           length switchover can operate.                                  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_avg_getAutoType"
ViStatus _VI_FUNC rsnrpz_avg_getAutoType(
    ViSession   vi,
    ViInt32     channel,
    ViInt32*    pMethod
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:AVER:COUN:AUTO:TYPE",
                                &value, sizeof(value)));

    if ( pMethod )
        *pMethod = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Set Averaging Count                                             */
/* Purpose:  This function sets the filter bandwidth.                        */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_avg_setCount"
ViStatus _VI_FUNC rsnrpz_avg_setCount(
    ViSession   vi,
    ViInt32     channel,
    ViInt32     count
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:AVER:COUN",
                                   &count));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Averaging Count                                             */
/* Purpose:  This function gets the filter bandwidth.                        */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_avg_getCount"
ViStatus _VI_FUNC rsnrpz_avg_getCount(
    ViSession   vi,
    ViInt32     channel,
    ViInt32*    pCount
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:AVER:COUN",
                                &value, sizeof(value)));

    if ( pCount )
        *pCount = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Averaging State                                             */
/* Purpose:  This function switches the filter function of a sensor on or    */
/*           off.                                                            */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_avg_setEnabled"
ViStatus _VI_FUNC rsnrpz_avg_setEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean   averaging
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = (ViInt32)averaging;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:AVER:STAT",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Averaging State                                             */
/* Purpose:  This function returns the state of the filter function of a     */
/*           sensor.                                                         */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_avg_getEnabled"
ViStatus _VI_FUNC rsnrpz_avg_getEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean*  pAveraging
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:AVER:STAT",
                                &value, sizeof(value)));

    if ( pAveraging )
        *pAveraging = (ViBoolean)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Timeslot                                                    */
/* Purpose:  This function sets a timeslot whose measured value is used to   */
/*           automatically determine the filter length.                      */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_avg_setSlot"
ViStatus _VI_FUNC rsnrpz_avg_setSlot(
    ViSession   vi,
    ViInt32     channel,
    ViInt32     timeslot
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:AVER:COUN:AUTO:SLOT",
                                   &timeslot));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Timeslot                                                    */
/* Purpose:  This function returns a timeslot whose measured value is used to*/
/*           automatically determine the filter length.                      */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_avg_getSlot"
ViStatus _VI_FUNC rsnrpz_avg_getSlot(
    ViSession   vi,
    ViInt32     channel,
    ViInt32*    pTimeslot
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:AVER:COUN:AUTO:SLOT",
                                &value, sizeof(value)));

    if ( pTimeslot )
        *pTimeslot = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Terminal Control                                            */
/* Purpose:  This function determines whether a new result is calculated     */
/*           immediately after a new measured value is available or only     */
/*           after an entire range of new values is available for the filter.*/
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_avg_setTerminalControl "
ViStatus _VI_FUNC rsnrpz_avg_setTerminalControl (
    ViSession   vi,
    ViInt32     channel,
    ViInt32     terminalControl
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:AVER:TCON",
                                   &terminalControl));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Terminal Control                                            */
/* Purpose:  This function returns the type of terminal control.             */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_avg_getTerminalControl"
ViStatus _VI_FUNC rsnrpz_avg_getTerminalControl(
    ViSession   vi,
    ViInt32     channel,
    ViInt32*    pTerminalControl
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:AVER:TCON",
                                &value, sizeof(value)));

    if ( pTerminalControl )
        *pTerminalControl = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Reset                                                           */
/* Purpose:  This function initializes the digital filter by deleting the    */
/*           stored measured values.                                         */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_avg_reset"
ViStatus _VI_FUNC rsnrpz_avg_reset(
    ViSession   vi,
    ViInt32     channel
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:AVER:RES",
                                   NULL));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Auto Range State                                            */
/* Purpose:  This function sets the automatic selection of a measurement     */
/*           range to ON or OFF.                                             */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_range_setAutoEnabled"
ViStatus _VI_FUNC rsnrpz_range_setAutoEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean   autoRange
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = (ViInt32)autoRange;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81") || rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 3, 13 ) )  //  3/13  =  SENS:RANG:AUTO
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:RANG:AUTO",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Auto Range State                                            */
/* Purpose:  This function returns the state of automatic selection of a     */
/*           measurement range.                                              */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_range_getAutoEnabled"
ViStatus _VI_FUNC rsnrpz_range_getAutoEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean*  pAutoRange
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81") || rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 3, 13 ) )  //  3/13  =  SENS:RANG:AUTO
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:RANG:AUTO",
                                &value, sizeof(value)));

    if ( pAutoRange )
        *pAutoRange = (ViBoolean)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Crossover Level                                             */
/* Purpose:  This function sets the cross-over level.                        */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_range_setCrossoverLevel"
ViStatus _VI_FUNC rsnrpz_range_setCrossoverLevel(
    ViSession   vi,
    ViInt32     channel,
    ViReal64    crossoverLevel
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)crossoverLevel;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81") || rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 3, 14 ) )  //  3/14  =  SENS:RANG:CLEV
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:RANG:CLEV",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Crossover Level                                             */
/* Purpose:  This function reads the cross-over level.                       */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_range_getCrossoverLevel"
ViStatus _VI_FUNC rsnrpz_range_getCrossoverLevel(
    ViSession   vi,
    ViInt32     channel,
    ViReal64*   pCrossoverLevel
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81") || rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 3, 14 ) )  //  3/14  =  SENS:RANG:CLEV
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:RANG:CLEV",
                                &value, sizeof(value)));

    if ( pCrossoverLevel )
        *pCrossoverLevel = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Range                                                       */
/* Purpose:  This function selects a measurement range in which the          */
/*           corresponding sensor is to perform a measurement.               */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_range_setRange "
ViStatus _VI_FUNC rsnrpz_range_setRange (
    ViSession   vi,
    ViInt32     channel,
    ViInt32     range
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81") || rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 3, 12 ) )  //  3/12  =  SENS:RANG
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:RANG",
                                   &range));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Range                                                       */
/* Purpose:  This function returns a measurement range in which the          */
/*           corresponding sensor is to perform a measurement.               */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_range_getRange "
ViStatus _VI_FUNC rsnrpz_range_getRange (
    ViSession   vi,
    ViInt32     channel,
    ViInt32*    pRange
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

#if defined (USE_CHECKINSTR)
    if (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81") || rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 3, 12 ) )  //  3/12  =  SENS:RANG
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:RANG",
                                &value, sizeof(value)));

    if ( pRange )
        *pRange = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Configure Reflection Gamma Corr                                 */
/* Purpose:  This function sets the parameters of the compensation of the    */
/*           load distortion at the signal output.                           */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_configureReflectGammaCorr"
ViStatus _VI_FUNC rsnrpz_chan_configureReflectGammaCorr (ViSession vi,
                                                          ViInt32 channel,
                                                          ViReal64 magnitude,
                                                          ViReal64 phase)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR(rsnrpz_chan_setReflectionGammaMagn(vi, channel, magnitude));
    CHECKERR(rsnrpz_chan_setReflectionGammaPhase(vi, channel, phase));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Reflection Gamma Magnitude                                  */
/* Purpose:  This function sets the magnitude of the reflection coefficient  */
/*           of the load for distortion compensation.                        */
/*           To deactivate distortion compensation, set Magnitude to 0.      */
/*           Distortion compensation should remain deactivated in the case of*/
/*           questionable measurement values for the reflection coefficient  */
/*           of the load.                                                    */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_setReflectionGammaMagn"
ViStatus _VI_FUNC rsnrpz_chan_setReflectionGammaMagn (ViSession vi,
                                                      ViInt32 channel,
                                                      ViReal64 magnitude)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)magnitude;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:RGAM:MAGN",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Reflection Gamma Phase                                      */
/* Purpose:  This function defines the phase angle (in degrees) of the       */
/*           complex reflection factor of the load at the signal output.     */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_getReflectionGammaMagn"
ViStatus _VI_FUNC rsnrpz_chan_getReflectionGammaMagn (ViSession vi,
                                                      ViInt32 channel,
                                                      ViReal64 *magnitude)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:RGAM:MAGN",
                                &value, sizeof(value)));

    if ( magnitude )
        *magnitude = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Reflection Gamma Phase                                      */
/* Purpose:  This function defines the phase angle (in degrees) of the       */
/*           complex reflection factor of the load at the signal output.     */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_setReflectionGammaPhase"
ViStatus _VI_FUNC rsnrpz_chan_setReflectionGammaPhase (ViSession vi,
                                                       ViInt32 channel,
                                                       ViReal64 phase)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)phase;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:RGAM:PHAS",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Reflection Gamma Phase                                      */
/* Purpose:  This function reads the phase angle (in degrees) of the complex */
/*           reflection factor of the load at the signal output.             */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_getReflectionGammaPhase"
ViStatus _VI_FUNC rsnrpz_chan_getReflectionGammaPhase (ViSession vi,
                                                       ViInt32 channel,
                                                       ViReal64 *phase)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:RGAM:PHAS",
                                &value, sizeof(value)));

    if ( phase )
        *phase = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Set Reflection Gamma Uncertainty                                */
/* Purpose:  This function defines reflection gamma uncertainty.             */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_setReflectionGammaUncertainty"
ViStatus _VI_FUNC rsnrpz_chan_setReflectionGammaUncertainty (ViSession vi,
                                                    ViInt32 channel,
                                                    ViReal64 uncertainty)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)uncertainty;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:RGAM:EUNC",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Reflection Gamma Uncertainty                                */
/* Purpose:  This function queries reflection gamma uncertainty.             */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_getReflectionGammaUncertainty"
ViStatus _VI_FUNC rsnrpz_chan_getReflectionGammaUncertainty (ViSession vi,
                                                    ViInt32 channel,
                                                    ViReal64 *uncertainty)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:RGAM:EUNC",
                                &value, sizeof(value)));

    if ( uncertainty )
        *uncertainty = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Configure Duty Cycle                                            */
/* Purpose:  This function configures all duty cycle parameters.             */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_corr_configureDutyCycle"
ViStatus _VI_FUNC rsnrpz_corr_configureDutyCycle(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean   dutyCycleState,
    ViReal64    dutyCycle
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    if ( dutyCycleState )
    {
        CHECKERR(rsnrpz_corr_setDutyCycle(vi, channel, dutyCycle));
    }

    CHECKERR(rsnrpz_corr_setDutyCycleEnabled(vi, channel, dutyCycleState));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Duty Cycle                                                  */
/* Purpose:  This function informs the R&S NRP about the duty cycle of the   */
/*           power to be measured.                                           */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_corr_setDutyCycle "
ViStatus _VI_FUNC rsnrpz_corr_setDutyCycle (
    ViSession   vi,
    ViInt32     channel,
    ViReal64    dutyCycle
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)dutyCycle;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:CORR:DCYC",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Duty Cycle                                                  */
/* Purpose:  This function gets the size of duty cycle of the power to be    */
/*           measured.                                                       */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_corr_getDutyCycle "
ViStatus _VI_FUNC rsnrpz_corr_getDutyCycle (
    ViSession   vi,
    ViInt32     channel,
    ViReal64*   pDutyCycle
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:CORR:DCYC",
                                &value, sizeof(value)));

    if ( pDutyCycle )
        *pDutyCycle = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Duty Cycle State                                            */
/* Purpose:  This function switches measured-value correction for a specific */
/*           duty cycle on or off.                                           */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_corr_setDutyCycleEnabled"
ViStatus _VI_FUNC rsnrpz_corr_setDutyCycleEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean   dutyCycleState
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = dutyCycleState;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:CORR:DCYC:STAT",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Duty Cycle State                                            */
/* Purpose:  This function gets the setting of duty cycle.                   */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_corr_getDutyCycleEnabled"
ViStatus _VI_FUNC rsnrpz_corr_getDutyCycleEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean*  pDutyCycleState
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:CORR:DCYC:STAT",
                                &value, sizeof(value)));

    if ( pDutyCycleState )
        *pDutyCycleState = (ViBoolean)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Configure Corrections                                           */
/* Purpose:  This function configures all correction parameters.             */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_corr_configureCorrections "
ViStatus _VI_FUNC rsnrpz_corr_configureCorrections (
    ViSession   vi,
    ViInt32     channel,
    ViBoolean   offsetState,
    ViReal64    offset,
    ViBoolean   offsetTableState,
    ViString    tableName,
    ViBoolean   sParameterEnable
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR (rsnrpz_corr_setOffset (vi, channel, offset));
    CHECKERR (rsnrpz_corr_setOffsetEnabled (vi, channel, offsetState));
#if 0
    CHECKERR (rsnrpz_corr_setOffsetTableEnabled (vi, channel, offsetTableState));
    CHECKERR (rsnrpz_corr_setOffsetTableSelect (vi, channel, tableName));
#else
    if ( tableName == 0 || offsetTableState == 0 )
        /* no warnings please */ { }
#endif
    return rsnrpz_corr_setSParamDeviceEnabled (vi, channel, sParameterEnable);
}

/*===========================================================================*/
/* Function: Set Correction Frequency                                        */
/* Purpose:  This function informs the R&S NRP about the frequency of the    */
/*           power to be measured since this frequency is not automatically  */
/*           determined.                                                     */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_setCorrectionFrequency "
ViStatus _VI_FUNC rsnrpz_chan_setCorrectionFrequency (
    ViSession   vi,
    ViInt32     channel,
    ViReal64    frequency
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)frequency;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:FREQ",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Correction Frequency                                        */
/* Purpose:  This function queries the instrument for the frequency of the   */
/*           power to be measured since this frequency is not automatically  */
/*           determined.                                                     */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_getCorrectionFrequency"
ViStatus _VI_FUNC rsnrpz_chan_getCorrectionFrequency(
    ViSession   vi,
    ViInt32     channel,
    ViReal64*   frequency
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:FREQ",
                                &value, sizeof(value)));

    if ( frequency )
        *frequency = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Correction Frequency Step                                   */
/* Purpose:  If the frequency step parameter is set to a value greater than  */
/*           0.0 the sensor does a internal frequency increment if buffered  */
/*           mode is enabled                                                 */
/*                                                                           */
/*           Depending on the parameter "frequency spacing" the sensor adds  */
/*           this value to the current frequency or it multiplies this value */
/*           with the current frequency.                                     */
/*                                                                           */
/*           This function is used to do a simple scalar network nalysis.    */
/*           To enable this automativally frequency stepping you have to    */
/*           configure CONTAV sensor mode, enable buffered measurements and  */
/*           set frequency stepping to a value greater than 0.               */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_setCorrectionFrequencyStep"
ViStatus _VI_FUNC rsnrpz_chan_setCorrectionFrequencyStep(
    ViSession vi,
    ViInt32 channel,
    ViReal64 frequencyStep
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)frequencyStep;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:FREQ:STEP",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Get Correction Frequency Step                                   */
/* Purpose:  This function returns the frequency step value.                 */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_getCorrectionFrequencyStep"
ViStatus _VI_FUNC rsnrpz_chan_getCorrectionFrequencyStep(
    ViSession vi,
    ViInt32 channel,
    ViReal64 *frequencyStep
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:FREQ:STEP",
                                &value, sizeof(value)));

    if ( frequencyStep )
        *frequencyStep = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Set Correction Frequency Spacing                                */
/* Purpose:  If scalar network analysis is enabled this parameter defines    */
/*           how the frequency is incremented.                               */
/*                                                                           */
/*           Linear spacing means that the frequency step value is added to  */
/*           the current frequency after each buffered measurement.          */
/*           Logarithmic spacing means that the frequency step value is      */
/*           multiplied with the current frequency after each buffered       */
/*           measurement.                                                    */
/*                                                                           */
/*           This command is used to do a simple scalar network nalysis.     */
/*           To enable this automativally frequency stepping you have to     */
/*           configure CONTAV sensor mode, enable buffered measurements and  */
/*           set frequency stepping to a value greater than 0.               */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_setCorrectionFrequencySpacing"
ViStatus _VI_FUNC rsnrpz_chan_setCorrectionFrequencySpacing(
    ViSession vi,
    ViInt32 channel,
    ViInt32 frequencySpacing
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:FREQ:SPAC",
                                   &frequencySpacing));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Get Correction Frequency Spacing                                */
/* Purpose:  This function returns the frequency spacing value.              */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_getCorrectionFrequencySpacing"
ViStatus _VI_FUNC rsnrpz_chan_getCorrectionFrequencySpacing(
    ViSession vi,
    ViInt32 channel,
    ViInt32 *frequencySpacing
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:FREQ:SPAC",
                                &value, sizeof(value)));

    if ( frequencySpacing )
        *frequencySpacing = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Offset                                                      */
/* Purpose:  With this function a fixed offset value can be defined for      */
/*           multiplying (logarithmically adding) the measured value of a    */
/*           sensor.                                                         */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_corr_setOffset"
ViStatus _VI_FUNC rsnrpz_corr_setOffset(
    ViSession   vi,
    ViInt32     channel,
    ViReal64    offset
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)offset;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:CORR:OFFS",
                                   &value));


    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Offset                                                      */
/* Purpose:  This function gets a fixed offset value defined for multiplying */
/*           (logarithmically adding) the measured value of a sensor.        */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_corr_getOffset"
ViStatus _VI_FUNC rsnrpz_corr_getOffset(
    ViSession   vi,
    ViInt32     channel,
    ViReal64*   pOffset
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:CORR:OFFS",
                                &value, sizeof(value)));

    if ( pOffset )
        *pOffset = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Offset State                                                */
/* Purpose:  This function switches the offset correction on or off.         */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_corr_setOffsetEnabled"
ViStatus _VI_FUNC rsnrpz_corr_setOffsetEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean   offsetState
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = offsetState;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:CORR:OFFS:STAT",
                                   &value));

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Offset State                                                */
/* Purpose:  This function returns the offset correction on or off.          */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_corr_getOffsetEnabled"
ViStatus _VI_FUNC rsnrpz_corr_getOffsetEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean*  pOffsetState
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:CORR:OFFS:STAT",
                                &value, sizeof(value)));

    if ( pOffsetState )
        *pOffsetState = (ViBoolean)value;
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set S-Parameter Device State                                    */
/* Purpose:  This function instructs the sensor to perform a measured-value  */
/*           correction by means of the stored s-parameter device.           */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_corr_setSParamDeviceEnabled"
ViStatus _VI_FUNC rsnrpz_corr_setSParamDeviceEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean   sParameterEnable
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = sParameterEnable;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:CORR:SPD:STAT",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get S-Parameter Device State                                    */
/* Purpose:  This function returns the state of a measured-value correction  */
/*           by means of the stored s-parameter device.                      */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_corr_getSParamDeviceEnabled"
ViStatus _VI_FUNC rsnrpz_corr_getSParamDeviceEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean*  psParameterCorrection
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:CORR:SPD:STAT",
                                &value, sizeof(value)));

    if ( psParameterCorrection )
        *psParameterCorrection = (ViBoolean)value;
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Set S-Parameter Device                                          */
/* Purpose:  This function stores s-parameter device.                        */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_corr_setSParamDevice"
ViStatus _VI_FUNC rsnrpz_corr_setSParamDevice (ViSession vi,
                                      ViInt32 channel, ViInt32 sParameter)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:CORR:SPD:SEL",
                                   &sParameter));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get S-Parameter Device                                          */
/* Purpose:  This function gets s-parameter device.                          */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_corr_getSParamDevice"
ViStatus _VI_FUNC rsnrpz_corr_getSParamDevice (ViSession vi,
                                      ViInt32 channel, ViInt32 *sParameter)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:CORR:SPD:SEL",
                                &value, sizeof(value)));

    if ( sParameter != NULL )
        *sParameter = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Get S-Parameter Device List                                     */
/* Purpose:  This function queries the list of possible S-Parameter Devices  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_corr_getSParamDevList"
ViStatus _VI_FUNC rsnrpz_corr_getSParamDevList( ViSession vi,
                                                ViInt32   channel,
                                                ViInt32   iSpdListSize,
                                                ViChar    spdList[] )
{
  ViStatus    rsnrpz_status = VI_SUCCESS;
  ViChar      szBuffer[ 2 * BUFFER_SIZE ];
  ViChar      *cpCRLF;
  ViInt32     iLen = 0;

  CHECKCHANNEL(vi, channel);

  memset( szBuffer, 0x00, 2 * BUFFER_SIZE );

  CHECKERR( rsnrpz_ReadValue( vi,
                              channel,
                              (char*)"SENS:CORR:SPD:LIST",
                              szBuffer,
                              2 * BUFFER_SIZE - 1 ) );


  CHECKERR( rsnrpz_checkStatus( vi ) );

  iLen = (ViInt32)strlen( szBuffer );
  cpCRLF = szBuffer;

  for ( ; ; )
  {
    if ( *cpCRLF == 0 )   // end of list
      break;

    cpCRLF = strstr( cpCRLF, "\r\n" );

    if ( ! cpCRLF )       // no more entries
      break;

    *cpCRLF = ',';        // CR becomes ','
    cpCRLF++;             // LF will be removed
    memmove( cpCRLF, cpCRLF + 1, iLen - (cpCRLF - szBuffer) );
    iLen--;               // string is one byte shorter now
  }

  strncpy( spdList, szBuffer, iSpdListSize );

  return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Configure Source Gamma Corr                                     */
/* Purpose:  This function sets the parameters of the reflection coefficient */
/*           for measured-value correction.                                  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_configureSourceGammaCorr"
ViStatus _VI_FUNC rsnrpz_chan_configureSourceGammaCorr(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean   sourceGammaCorrection,
    ViReal64    magnitude,
    ViReal64    phase
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_chan_setSourceGammaMagnitude(vi, channel, magnitude));
    CHECKERR( rsnrpz_chan_setSourceGammaPhase(vi, channel, phase));
    CHECKERR( rsnrpz_chan_setSourceGammaCorrEnabled(vi, channel, sourceGammaCorrection)) ;
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Source Gamma Magnitude                                      */
/* Purpose:  This function sets the magnitude of the reflection coefficient  */
/*           for measured-value correction.                                  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_setSourceGammaMagnitude"
ViStatus _VI_FUNC rsnrpz_chan_setSourceGammaMagnitude(
    ViSession   vi,
    ViInt32     channel,
    ViReal64    magnitude
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)magnitude;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:SGAM:MAGN",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Source Gamma Magnitude                                      */
/* Purpose:  This function reads the magnitude of the reflection coefficient */
/*           for measured-value correction.                                  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_getSourceGammaMagnitude"
ViStatus _VI_FUNC rsnrpz_chan_getSourceGammaMagnitude(
    ViSession   vi,
    ViInt32     channel,
    ViReal64*   pMagnitude
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:SGAM:MAGN",
                                &value, sizeof(value)));

    if ( pMagnitude )
        *pMagnitude = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Source Gamma Phase                                          */
/* Purpose:  This function sets the phase angle of the reflection coefficient*/
/*           for measured-value correction.                                  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_setSourceGammaPhase "
ViStatus _VI_FUNC rsnrpz_chan_setSourceGammaPhase (
    ViSession   vi,
    ViInt32     channel,
    ViReal64    phase
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)phase;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:SGAM:PHAS",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Source Gamma Phase                                          */
/* Purpose:  This function reads the phase angle of the reflection           */
/*           coefficient for measured-value correction.                      */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_getSourceGammaPhase"
ViStatus _VI_FUNC rsnrpz_chan_getSourceGammaPhase(
    ViSession   vi,
    ViInt32     channel,
    ViReal64*   pPhase
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:SGAM:PHAS",
                                &value, sizeof(value)));

    if ( pPhase )
        *pPhase = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Source Gamma Corr State                                     */
/* Purpose:  This function switches the measured-value correction of the     */
/*           reflection coefficient effect of the source gamma ON or OFF.    */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_setSourceGammaCorrEnabled"
ViStatus _VI_FUNC rsnrpz_chan_setSourceGammaCorrEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean   sourceGammaCorrection
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = sourceGammaCorrection;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:SGAM:CORR:STAT",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Source Gamma Corr State                                     */
/* Purpose:  This function reads the state of source gamma correction.       */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_getSourceGammaCorrEnabled"
ViStatus _VI_FUNC rsnrpz_chan_getSourceGammaCorrEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean*  pSourceGammaCorrection
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:SGAM:CORR:STAT",
                                &value, sizeof(value)));

    if ( pSourceGammaCorrection )
        *pSourceGammaCorrection = (ViBoolean)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set ContAv Aperture                                             */
/* Purpose:  This function determines the integration time for a single      */
/*           measurement in the ContAv mode.                                 */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_setContAvAperture"
ViStatus _VI_FUNC rsnrpz_chan_setContAvAperture(
    ViSession   vi,
    ViInt32     channel,
    ViReal64    contAvAperture
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)contAvAperture;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:POW:AVG:APER",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get ContAv Aperture                                             */
/* Purpose:  This function queries the value of ContAv mode aperture size.   */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_getContAvAperture"
ViStatus _VI_FUNC rsnrpz_chan_getContAvAperture(
    ViSession   vi,
    ViInt32     channel,
    ViReal64*   pContAvAperture
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:POW:AVG:APER",
                                &value, sizeof(value)));

    if ( pContAvAperture )
        *pContAvAperture = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set ContAv Smoothing State                                      */
/* Purpose:  This function activates digital lowpass filtering of the sampled*/
/*           video signal.                                                   */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_setContAvSmoothingEnabled"
ViStatus _VI_FUNC rsnrpz_chan_setContAvSmoothingEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean   contAvSmoothing
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = contAvSmoothing;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:POW:AVG:SMO:STAT",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get ContAv Smoothing State                                      */
/* Purpose:  This function gets the state of digital lowpass filtering of the*/
/*           sampled video signal.                                           */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_getContAvSmoothingEnabled"
ViStatus _VI_FUNC rsnrpz_chan_getContAvSmoothingEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean*  pContAvSmoothing
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:POW:AVG:SMO:STAT",
                                &value, sizeof(value)));

    if ( pContAvSmoothing )
        *pContAvSmoothing = (ViBoolean)value;
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Buffered ContAv State                                       */
/* Purpose:  This function switches on the buffered ContAv mode, after which */
/*           data blocks rather than single measured values are then         */
/*           returned.                                                       */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_setContAvBufferedEnabled"
ViStatus _VI_FUNC rsnrpz_chan_setContAvBufferedEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean   contAvBufferedMode
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = contAvBufferedMode;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:POW:AVG:BUFF:STAT",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Buffered ContAv State                                       */
/* Purpose:  This function returns the state of ContAv Buffered Measurement  */
/*           Mode.                                                           */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_getContAvBufferedEnabled"
ViStatus _VI_FUNC rsnrpz_chan_getContAvBufferedEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean*  pContAvBufferedMode
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:POW:AVG:BUFF:STAT",
                                &value, sizeof(value)));

    if ( pContAvBufferedMode )
        *pContAvBufferedMode = (ViBoolean)value;
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set ContAv Buffer Size                                          */
/* Purpose:  This function sets the number of desired values for the buffered*/
/*           ContAv mode.                                                    */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_setContAvBufferSize"
ViStatus _VI_FUNC rsnrpz_chan_setContAvBufferSize(
    ViSession   vi,
    ViInt32     channel,
    ViInt32     bufferSize
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = bufferSize;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:POW:AVG:BUFF:SIZE",
                                   &value));

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get ContAv Buffer Size                                          */
/* Purpose:  This function returns the number of desired values for the      */
/*           buffered ContAv mode.                                           */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_getContAvBufferSize"
ViStatus _VI_FUNC rsnrpz_chan_getContAvBufferSize(
    ViSession   vi,
    ViInt32     channel,
    ViInt32*    pBufferSize
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:POW:AVG:BUFF:SIZE",
                                &value, sizeof(value)));

    if ( pBufferSize )
        *pBufferSize = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get ContAv Buffer Count                                         */
/* Purpose:  This function returns the number of measurement values          */
/*           currently stored in the sensor buffer while the buffered        */
/*           measurement is running.                                         */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_getContAvBufferCount"
ViStatus _VI_FUNC rsnrpz_chan_getContAvBufferCount(
    ViSession vi,
    ViInt32 channel,
    ViInt32 *bufferCount
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:POW:AVG:BUFF:COUN",
                                &value, sizeof(value)));

    if ( bufferCount )
        *bufferCount = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Get ContAv Buffer Info                                          */
/* Purpose:  This function returns some important settings for the scalar    */
/*           network analysis.                                               */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_getContAvBufferInfo"
ViStatus _VI_FUNC rsnrpz_chan_getContAvBufferInfo (ViSession vi,
                                                   ViInt32 channel,
                                                   ViString infoType,
                                                   ViInt32 bufferSize,
                                                   ViChar data[])
{
    ViInt32     i;
    ViStatus    rsnrpz_status            = VI_SUCCESS;
    ViChar      buffer[BUFFER_SIZE]      = "";
    ViChar*     pbuffer                  = buffer;
    ViChar*     pdynamic_buffer          = NULL;
    const char*  s_valid_info_types[] = {
                                            "Fast",
                                            "Normal",
                                            "HighPrecision",
                                            "Tracemode",
                                            "",
                                            NULL
                                        };

    CHECKCHANNEL(vi, channel);

    for ( i = 0; infoType && s_valid_info_types[i]; i++)
    {
        if (STRICMP(infoType, s_valid_info_types[i]) == 0)
            break;
    }
    if ( infoType == NULL || (!s_valid_info_types[i])  )
        return VI_ERROR_PARAMETER3;

    rsnrpz_status = rsnrpz_ReadValue( vi,
                                      channel,
                                      (char*)"SENS:POW:AVG:BUFF:INFO",
                                      buffer, sizeof(buffer));
    CHECKERR_CU( rsnrpz_status );

    if (strlen(infoType)==0)
    {
        // return entire answer if infoType is the empty string
        strncpy( data, pbuffer, bufferSize - 1);
    }
    else
    {
        pbuffer = (ViChar *)stristr( pbuffer, infoType );
        if ( pbuffer )
        {
            ViChar*     pend = NULL;

            pbuffer += strlen(infoType) + 1;

            pend = strchr(pbuffer, '\n');
            if (pend)
            {
                while (pbuffer < pend && isspace(*(pend-1)) )
                    pend--;
                *pend = '\0';
            }

            if ( bufferSize < 0 )
                strcpy( data, pbuffer);
            else if ( bufferSize < (int)(strlen( pbuffer) + 1) )
            {
                rsnrpz_status = (int)strlen(pbuffer) + 1;

                if ( (bufferSize - 1) > 0 && data)
                {
                    strncpy( data, pbuffer, bufferSize - 1);
                }
            }
            else
            {
                strcpy( data, pbuffer);
            }
        }
        else
        {
            return VI_ERROR_PARAMETER2;
        }
    }

    if ( rsnrpz_status == VI_SUCCESS )
    {
        /*
         * Don't overwrite required buffer size
         */
        CHECKERR( rsnrpz_checkStatus ( vi));
    }

Error:
    if ( pdynamic_buffer != NULL )
        free(pdynamic_buffer);

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Burst Drop-Out Tolerance                                    */
/* Purpose:  This function sets the drop-out tolerance parameter.            */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_setBurstDropoutTolerance"
ViStatus _VI_FUNC rsnrpz_chan_setBurstDropoutTolerance(
    ViSession   vi,
    ViInt32     channel,
    ViReal64    dropoutTolerance
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)dropoutTolerance;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 6, 1 ) )  //  6/1  =  SENS:POW:BURS:DTOL
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:POW:BURS:DTOL",
                                   &value));

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Burst Drop-Out Tolerance                                    */
/* Purpose:  This function returns the drop-out tolerance parameter.         */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_getBurstDropoutTolerance"
ViStatus _VI_FUNC rsnrpz_chan_getBurstDropoutTolerance(
    ViSession   vi,
    ViInt32     channel,
    ViReal64*   pDropoutTolerance
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 6, 1 ) )  //  6/1  =  SENS:POW:BURS:DTOL
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:POW:BURS:DTOL",
                                &value, sizeof(value)));

    if ( pDropoutTolerance )
        *pDropoutTolerance = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Set Burst Chopper State                                         */
/* Purpose:  This function enables or disables the chopper in BurstAv mode.  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_setBurstChopperEnabled"
ViStatus _VI_FUNC rsnrpz_chan_setBurstChopperEnabled (ViSession vi,
                                                      ViInt32 channel,
                                                      ViBoolean burstAvChopper)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = burstAvChopper;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 6, 2 ) )  //  6/2  =  SENS:POW:BURS:CHOP:STAT
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:POW:BURS:CHOP:STAT",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Burst Chopper State                                         */
/* Purpose:  This function queries the state of the chopper in BurstAv mode. */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_getBurstChopperEnabled"
ViStatus _VI_FUNC rsnrpz_chan_getBurstChopperEnabled (ViSession vi,
                                                      ViInt32 channel,
                                                      ViBoolean *burstAvChopper)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 6, 2 ) )  //  6/2  =  SENS:POW:BURS:CHOP:STAT
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:POW:BURS:CHOP:STAT",
                                &value, sizeof(value)));

    if ( burstAvChopper )
        *burstAvChopper = (ViBoolean)value;
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Configure Statistics Timegate                                   */
/* Purpose:  This function configures the timegate (depends on trigger event)*/
/*           in which the sensor is doing statistic measurements.            */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_stat_confTimegate"

ViStatus _VI_FUNC rsnrpz_stat_confTimegate (ViSession vi,
                                            ViInt32 channel, ViReal64 offset,
                                            ViReal64 time,
                                            ViReal64 midambleOffset,
                                            ViReal64 midambleLength)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_stat_setOffsetTime(vi, channel, offset));
    CHECKERR( rsnrpz_stat_setTime(vi, channel, time));
    CHECKERR( rsnrpz_stat_setMidOffset(vi,channel,midambleOffset));
    CHECKERR( rsnrpz_stat_setMidLength(vi,channel,midambleLength));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Configure Statistics Scale                                      */
/* Purpose:  This function sets the X-Axis of statistical measurement.       */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_stat_confScale"

ViStatus _VI_FUNC rsnrpz_stat_confScale (ViSession vi,
                                         ViInt32 channel,
                                         ViReal64 referenceLevel, ViReal64 range,
                                         ViInt32 points)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_stat_setScaleRefLevel(vi, channel, referenceLevel));
    CHECKERR( rsnrpz_stat_setScaleRange(vi, channel, range));
    CHECKERR( rsnrpz_stat_setScalePoints(vi,channel, points));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Statistics Offset Time                                      */
/* Purpose:  This function sets the start after the trigger of the timegate  */
/*           in which the sensor is doing statistic measurements.            */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_stat_setOffsetTime"

ViStatus _VI_FUNC rsnrpz_stat_setOffsetTime (ViSession vi,
                                             ViInt32 channel, ViReal64 offset)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float) offset;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 14, 1 ) )  //  14/1  =  SENS:STAT:OFFS:TIME
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:STAT:OFFS:TIME",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Statistics Offset Time                                      */
/* Purpose:  This function queries the start after the trigger of the        */
/*           timegate in which the sensor is doing statistic measurements.   */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_stat_getOffsetTime"

ViStatus _VI_FUNC rsnrpz_stat_getOffsetTime (ViSession vi,
                                             ViInt32 channel, ViReal64 *offset)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 14, 1 ) )  //  14/1  =  SENS:STAT:OFFS:TIME
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:STAT:OFFS:TIME",
                                &value, sizeof(value)));

    if ( offset )
        *offset = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Statistics Time                                             */
/* Purpose:  This function sets the length of the timegate in which the      */
/*           sensor is doing statistic measurements.                         */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_stat_setTime"

ViStatus _VI_FUNC rsnrpz_stat_setTime (ViSession vi,
                                       ViInt32 channel, ViReal64 time)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float) time;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 14, 2 ) )  //  14/2  =  SENS:STAT:TIME
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:STAT:TIME",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Statistics Time                                             */
/* Purpose:  This function gets the length of the timegate in which the      */
/*           sensor is doing statistic measurements.                         */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_stat_getTime"

ViStatus _VI_FUNC rsnrpz_stat_getTime (ViSession vi,
                                       ViInt32 channel, ViReal64 *time)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 14, 2 ) )  //  14/2  =  SENS:STAT:TIME
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:STAT:TIME",
                                &value, sizeof(value)));

    if ( time )
        *time = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Statistics Mid Offset                                       */
/* Purpose:  This function sets the midamble offset after timeslot start in  */
/*           seconds in the timegate in which the sensor is doing statistic  */
/*           measurements.                                                   */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_stat_setMidOffset"

ViStatus _VI_FUNC rsnrpz_stat_setMidOffset (ViSession vi,
                                            ViInt32 channel, ViReal64 offset)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float) offset;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 14, 3 ) )  //  14/3  =  SENS:STAT:MID:OFFS
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:STAT:MID:OFFS",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Statistics Mid Offset                                       */
/* Purpose:  This function gets the midamble offset after timeslot start in  */
/*           seconds in the timegate in which the sensor is doing statistic  */
/*           measurements.                                                   */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_stat_getMidOffset"

ViStatus _VI_FUNC rsnrpz_stat_getMidOffset (ViSession vi,
                                            ViInt32 channel, ViReal64 *offset)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 14, 3 ) )  //  14/3  =  SENS:STAT:MID:OFFS
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:STAT:MID:OFFS",
                                &value, sizeof(value)));

    if ( offset )
        *offset = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Statistics Mid Length                                       */
/* Purpose:  This function sets the midamble length in seconds.              */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_stat_setMidLength"

ViStatus _VI_FUNC rsnrpz_stat_setMidLength (ViSession vi,
                                            ViInt32 channel, ViReal64 length)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float) length;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 14, 4 ) )  //  14/4  =  SENS:STAT:MID:TIME
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:STAT:MID:TIME",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Statistics Mid Length                                       */
/* Purpose:  This function gets the midamble length in seconds.              */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_stat_getMidLength"

ViStatus _VI_FUNC rsnrpz_stat_getMidLength (ViSession vi,
                                            ViInt32 channel, ViReal64 *length)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 14, 4 ) )  //  14/4  =  SENS:STAT:MID:TIME
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:STAT:MID:TIME",
                                &value, sizeof(value)));

    if ( length )
        *length = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Statistics Scale Ref Level                                  */
/* Purpose:  This function sets the X-Axis reference level.                  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_stat_setScaleRefLevel"

ViStatus _VI_FUNC rsnrpz_stat_setScaleRefLevel (ViSession vi,
                                                ViInt32 channel,
                                                ViReal64 referenceLevel)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float) referenceLevel;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 14, 5 ) )  //  14/5  =  SENS:STAT:SCALE:X:RLEV
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:STAT:SCALE:X:RLEV",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Statistics Scale Ref Level                                  */
/* Purpose:  This function gets the X-Axis reference level.                  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_stat_getScaleRefLevel"

ViStatus _VI_FUNC rsnrpz_stat_getScaleRefLevel (ViSession vi,
                                                ViInt32 channel,
                                                ViReal64 *referenceLevel)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 14, 5 ) )  //  14/5  =  SENS:STAT:SCALE:X:RLEV
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:STAT:SCALE:X:RLEV",
                                &value, sizeof(value)));

    if ( referenceLevel )
        *referenceLevel = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Statistics Scale Range                                      */
/* Purpose:  This function sets the X-Axis range.                            */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_stat_setScaleRange"

ViStatus _VI_FUNC rsnrpz_stat_setScaleRange (ViSession vi,
                                             ViInt32 channel, ViReal64 range)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float) range;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 14, 6 ) )  //  14/6  =  SENS:STAT:SCALE:X:RANG
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:STAT:SCALE:X:RANG",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Statistics Scale Range                                      */
/* Purpose:  This function gets the X-Axis range.                            */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_stat_getScaleRange"

ViStatus _VI_FUNC rsnrpz_stat_getScaleRange (ViSession vi,
                                             ViInt32 channel, ViReal64 *range)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 14, 6 ) )  //  14/6  =  SENS:STAT:SCALE:X:RANG
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:STAT:SCALE:X:RANG",
                                &value, sizeof(value)));

    if ( range )
        *range = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Statistics Scale Points                                     */
/* Purpose:  This function sets the number of power classes the sensor is    */
/*           calculating and returning.                                      */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_stat_setScalePoints"

ViStatus _VI_FUNC rsnrpz_stat_setScalePoints (ViSession vi,
                                              ViInt32 channel, ViInt32 points)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = points;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 14, 7 ) )  //  14/7  =  SENS:STAT:SCALE:X:POIN
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:STAT:SCALE:X:POIN",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Statistics Scale Points                                     */
/* Purpose:  This function gets the number of power classes the sensor is    */
/*           calculating and returning.                                      */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_stat_getScalePoints"

ViStatus _VI_FUNC rsnrpz_stat_getScalePoints (ViSession vi,
                                              ViInt32 channel, ViInt32 *points)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 14, 7 ) )  //  14/7  =  SENS:STAT:SCALE:X:POIN
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:STAT:SCALE:X:POIN",
                                &value, sizeof(value)));

    if ( points )
        *points = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Statistics Scale Width                                      */
/* Purpose:  This function queries the X-Axis MP width.                      */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_stat_getScaleWidth"

ViStatus _VI_FUNC rsnrpz_stat_getScaleWidth (ViSession vi,
                                             ViInt32 channel, ViReal64 *width)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 14, 8 ) )  //  14/8  =  SENS:STAT:SCALE:X:MPW
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:STAT:SCALE:X:MPW",
                                &value, sizeof(value)));

    if ( width )
        *width = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Configure Timeslot                                              */
/* Purpose:  This function configures the parameters of Timeslot measurement */
/*           mode. Both exclude start and stop are set to 10% of timeslot    */
/*           width each.                                                     */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_tslot_configureTimeSlot"

ViStatus _VI_FUNC rsnrpz_tslot_configureTimeSlot(
    ViSession       vi,
    ViInt32         channel,
    ViInt32         timeSlotCount,
    ViReal64        width
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_tslot_setTimeSlotCount(vi, channel, timeSlotCount));
    CHECKERR( rsnrpz_tslot_setTimeSlotWidth(vi, channel, width));

    CHECKERR( rsnrpz_timing_setTimingExcludeStart(vi,channel,0.1*width));
    CHECKERR( rsnrpz_timing_setTimingExcludeStop(vi,channel,1e-6*width));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Time Slot Count                                             */
/* Purpose:  This function sets the number of simultaneously measured        */
/*           timeslots in the Timeslot mode.                                 */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_tslot_setTimeSlotCount"
ViStatus _VI_FUNC rsnrpz_tslot_setTimeSlotCount(
    ViSession   vi,
    ViInt32     channel,
    ViInt32     timeSlotCount
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = timeSlotCount;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 5, 1 ) )  //  5/1  =  SENS:POW:TSL:AVG:COUN
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:POW:TSL:AVG:COUN",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Time Slot Count                                             */
/* Purpose:  This function reads the number of simultaneously measured       */
/*           timeslots in the Timeslot mode.                                 */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_tslot_getTimeSlotCount"
ViStatus _VI_FUNC rsnrpz_tslot_getTimeSlotCount(
    ViSession   vi,
    ViInt32     channel,
    ViInt32*    pTimeSlotCount
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 5, 1 ) )  //  5/1  =  SENS:POW:TSL:AVG:COUN
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:POW:TSL:AVG:COUN",
                                &value, sizeof(value)));

    if ( pTimeSlotCount )
        *pTimeSlotCount = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Time Slot Width                                             */
/* Purpose:  This function sets the length of the timeslot in the Timeslot   */
/*           mode.                                                           */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_tslot_setTimeSlotWidth"
ViStatus _VI_FUNC rsnrpz_tslot_setTimeSlotWidth(
    ViSession   vi,
    ViInt32     channel,
    ViReal64    width
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)width;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 5, 2 ) )  //  5/2  =  SENS:POW:TSL:AVG:WIDT
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:POW:TSL:AVG:WIDT",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Time Slot Width                                             */
/* Purpose:  This function reads the length of the timeslot in the Timeslot  */
/*           mode.                                                           */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_tslot_getTimeSlotWidth"
ViStatus _VI_FUNC rsnrpz_tslot_getTimeSlotWidth(
    ViSession   vi,
    ViInt32     channel,
    ViReal64*   pWidth
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 5, 2 ) )  //  5/2  =  SENS:POW:TSL:AVG:WIDT
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:POW:TSL:AVG:WIDT",
                                &value, sizeof(value)));

    if ( pWidth )
        *pWidth = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Time Slot Mid Offset                                        */
/* Purpose:  This function sets the midamble offset after timeslot start in  */
/*           seconds in the Timeslot mode.                                   */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_tslot_setTimeSlotMidOffset"
ViStatus _VI_FUNC rsnrpz_tslot_setTimeSlotMidOffset (ViSession vi,
                                                     ViInt32 channel,
                                                     ViReal64 offset)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)offset;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 5, 4 ) )  //  5/4  =  SENS:POW:TSL:MID:OFFS
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:POW:TSL:MID:OFFS",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Time Slot Mid Offset                                        */
/* Purpose:  This function gets the midamble offset after timeslot start in  */
/*           seconds in the Timeslot mode.                                   */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_tslot_getTimeSlotMidOffset"
ViStatus _VI_FUNC rsnrpz_tslot_getTimeSlotMidOffset (ViSession vi,
                                                     ViInt32 channel,
                                                     ViReal64 *offset)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 5, 4 ) )  //  5/4  =  SENS:POW:TSL:MID:OFFS
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:POW:TSL:MID:OFFS",
                                &value, sizeof(value)));

    if ( offset )
        *offset = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Time Slot Mid Length                                        */
/* Purpose:  This function sets the midamble length in seconds.              */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_tslot_setTimeSlotMidLength"
ViStatus _VI_FUNC rsnrpz_tslot_setTimeSlotMidLength (ViSession vi,
                                                     ViInt32 channel,
                                                     ViReal64 length)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)length;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 5, 5 ) )  //  5/5  =  SENS:POW:TSL:MID:TIME
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:POW:TSL:MID:TIME",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Time Slot Mid Length                                        */
/* Purpose:  This function gets the midamble length in seconds.              */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_tslot_getTimeSlotMidLength"
ViStatus _VI_FUNC rsnrpz_tslot_getTimeSlotMidLength (ViSession vi,
                                                     ViInt32 channel,
                                                     ViReal64 *length)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 5, 5 ) )  //  5/5  =  SENS:POW:TSL:MID:TIME
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:POW:TSL:MID:TIME",
                                &value, sizeof(value)));

    if ( length )
        *length = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Time Slot Chopper State                                     */
/* Purpose:  This function enables or disables the chopper in Timeslot mode. */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_tslot_setTimeSlotChopperEnabled"
ViStatus _VI_FUNC rsnrpz_tslot_setTimeSlotChopperEnabled (ViSession vi,
                                                  ViInt32 channel,
                                                  ViBoolean timeSlotChopper)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = timeSlotChopper;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 5, 3 ) )  //  5/3  =  SENS:POW:TSL:CHOP:STAT
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:POW:TSL:CHOP:STAT",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Time Slot Chopper State                                     */
/* Purpose:  This function queries the state of the chopper in Time Slot     */
/*           mode.                                                           */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_tslot_getTimeSlotChopperEnabled"
ViStatus _VI_FUNC rsnrpz_tslot_getTimeSlotChopperEnabled (ViSession vi,
                                                  ViInt32 channel,
                                                  ViBoolean *timeSlotChopper)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 5, 3 ) )  //  5/3  =  SENS:POW:TSL:CHOP:STAT
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:POW:TSL:CHOP:STAT",
                                &value, sizeof(value)));

    if ( timeSlotChopper )
        *timeSlotChopper = (ViBoolean)value;
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Configure Scope                                                 */
/* Purpose:  This function sets parameters of the Scope mode.                */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_configureScope"
ViStatus _VI_FUNC rsnrpz_scope_configureScope(
    ViSession   vi,
    ViInt32     channel,
    ViInt32     scopePoints,
    ViReal64    scopeTime,
    ViReal64    offsetTime,
    ViBoolean   realtime
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_scope_setAverageEnabled(vi, channel, VI_FALSE));
    CHECKERR( rsnrpz_scope_setAverageTerminalControl(vi,
                                                     channel,
                                                    RSNRPZ_TERMINAL_CONTROL_REPEAT));
    CHECKERR( rsnrpz_scope_setPoints(vi,channel,scopePoints));
    CHECKERR( rsnrpz_scope_setTime(vi, channel, scopeTime));
    CHECKERR( rsnrpz_scope_setOffsetTime(vi, channel, offsetTime));

#if defined (USE_CHECKINSTR)
    if (!(rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5")) && !(rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81")))
#else
    if ( rsnrpz_IsSupported( vi, channel, 7, 4 ) )  //  7/4  =  SENS:TRAC:REAL
#endif
    {
        CHECKERR( rsnrpz_scope_setRealtimeEnabled(vi, channel, realtime));
    }
    CHECKERR( rsnrpz_checkStatus ( vi));


    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Fast Zero                                                       */
/* Purpose:  This function performs zeroing using the signal at the sensor   */
/*           input.                                                          */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_fastZero"

ViStatus _VI_FUNC rsnrpz_scope_fastZero (ViSession vi)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = RSNRPZ_ONCE;

    CHECKSESSION(vi);

    CHECKERR( rsnrpz_WriteCommand( vi, ALL_CHANNELS, (char*)"CAL:ZERO:FAST:AUTO", &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Scope Averaging State                                       */
/* Purpose:  This function switches the filter function of a sensor on or    */
/*           off.                                                            */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_setAverageEnabled"
ViStatus _VI_FUNC rsnrpz_scope_setAverageEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean   scopeAveraging
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = ((scopeAveraging) ? VI_TRUE : VI_FALSE);

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 1 ) )  //  7/1  =  SENS:TRAC:AVER:STAT
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:TRAC:AVER:STAT",
                                   &value));

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Averaging State                                       */
/* Purpose:  This function reads the state of filter function of a sensor.   */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_getAverageEnabled"
ViStatus _VI_FUNC rsnrpz_scope_getAverageEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean*  pScopeAveraging
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 1 ) )  //  7/1  =  SENS:TRAC:AVER:STAT
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:AVER:STAT",
                                &value, sizeof(value)));

    if ( pScopeAveraging )
        *pScopeAveraging = (ViBoolean)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Scope Average Count                                         */
/* Purpose:  This function sets the length of the filter for the Scope mode. */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_setAverageCount"
ViStatus _VI_FUNC rsnrpz_scope_setAverageCount(
    ViSession   vi,
    ViInt32     channel,
    ViInt32     count
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = count;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 6 ) )  //  7/6  =  SENS:TRAC:AVER:COUN
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:TRAC:AVER:COUN",
                                   &value));

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Average Count                                         */
/* Purpose:  This function returns the length of the filter                  */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_getAverageCount"
ViStatus _VI_FUNC rsnrpz_scope_getAverageCount(
    ViSession   vi,
    ViInt32     channel,
    ViInt32*    pCount
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 6 ) )  //  7/6  =  SENS:TRAC:AVER:COUN
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:AVER:COUN",
                                &value, sizeof(value)));

    if ( pCount )
        *pCount = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Scope Terminal Control                                      */
/* Purpose:  This function determines whether a new result will be calculated*/
/*           immediately after a new measured value is available or only     */
/*           after an entire range of new values is available for the filter.*/
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_setAverageTerminalControl"
ViStatus _VI_FUNC rsnrpz_scope_setAverageTerminalControl(
    ViSession   vi,
    ViInt32     channel,
    ViInt32     terminalControl
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 13 ) )  //  7/13  =  SENS:TRAC:AVER:TCON
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:TRAC:AVER:TCON",
                                   &terminalControl));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Terminal Control                                      */
/* Purpose:  This function returns selected terminal control type.           */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_getAverageTerminalControl"
ViStatus _VI_FUNC rsnrpz_scope_getAverageTerminalControl(
    ViSession   vi,
    ViInt32     channel,
    ViInt32*    pTerminalControl
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 13 ) )  //  7/13  =  SENS:TRAC:AVER:TCON
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:AVER:TCON",
                                &value, sizeof(value)));

    if ( pTerminalControl )
        *pTerminalControl = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Scope Offset Time                                           */
/* Purpose:  This function determines the relative position of the trigger   */
/*           event in relation to the beginning of the Scope measurement     */
/*           sequence.                                                       */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_setOffsetTime"
ViStatus _VI_FUNC rsnrpz_scope_setOffsetTime(
    ViSession   vi,
    ViInt32     channel,
    ViReal64    offsetTime
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)offsetTime;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 2 ) )  //  7/2  =  SENS:TRAC:OFFS:TIME
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:TRAC:OFFS:TIME",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Offset Time                                           */
/* Purpose:  This function reads the relative position of the trigger event  */
/*           in relation to the beginning of the Scope measurement sequence. */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_getOffsetTime"
ViStatus _VI_FUNC rsnrpz_scope_getOffsetTime(
    ViSession   vi,
    ViInt32     channel,
    ViReal64*   pOffsetTime
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 2 ) )  //  7/2  =  SENS:TRAC:OFFS:TIME
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:OFFS:TIME",
                                &value, sizeof(value)));

    if ( pOffsetTime )
        *pOffsetTime = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Scope Points                                                */
/* Purpose:  This function sets the number of desired values per Scope       */
/*           sequence.                                                       */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_setPoints "
ViStatus _VI_FUNC rsnrpz_scope_setPoints (
    ViSession   vi,
    ViInt32     channel,
    ViInt32     scopePoints
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 3 ) )  //  7/3  =  SENS:TRAC:POIN
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:TRAC:POIN",
                                   &scopePoints));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Points                                                */
/* Purpose:  This function reads the number of desired values per Scope      */
/*           sequence.                                                       */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_getPoints"
ViStatus _VI_FUNC rsnrpz_scope_getPoints(
    ViSession   vi,
    ViInt32     channel,
    ViInt32*    pScopePoints
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 3 ) )  //  7/3  =  SENS:TRAC:POIN
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:POIN",
                                &value, sizeof(value)));

    if ( pScopePoints )
        *pScopePoints = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Scope Realtime State                                        */
/* Purpose:  This function sets the state of real-time measurement setting.  */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_setRealtimeEnabled"
ViStatus _VI_FUNC rsnrpz_scope_setRealtimeEnabled(
    ViSession       vi,
    ViInt32         channel,
    ViBoolean       realtime
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = realtime;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 4 ) )  //  7/4  =  SENS:TRAC:REAL
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:TRAC:REAL",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Realtime State                                        */
/* Purpose:  This function reads the state of real-time measurement setting. */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_getRealtimeEnabled"
ViStatus _VI_FUNC rsnrpz_scope_getRealtimeEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean*  pRealTime
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 4 ) )  //  7/4  =  SENS:TRAC:REAL
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:REAL",
                                &value, sizeof(value)));

    if ( pRealTime )
        *pRealTime = (ViBoolean)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Scope Time                                                  */
/* Purpose:  This function sets the time to be covered by the Scope sequence.*/
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_setTime"
ViStatus _VI_FUNC rsnrpz_scope_setTime(
    ViSession   vi,
    ViInt32     channel,
    ViReal64    scopeTime
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)scopeTime;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 5 ) )  //  7/5  =  SENS:TRAC:TIME
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:TRAC:TIME",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Time                                                  */
/* Purpose:  This function reads the value of the time to be covered by the  */
/*           Scope sequence.                                                 */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_getTime"
ViStatus _VI_FUNC rsnrpz_scope_getTime(
    ViSession   vi,
    ViInt32     channel,
    ViReal64*   pScopeTime
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 5 ) )  //  7/5  =  SENS:TRAC:TIME
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:TIME",
                                &value, sizeof(value)));

    if ( pScopeTime )
        *pScopeTime = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Scope Auto Averaging State                                  */
/* Purpose:  This function can be used to automatically determine a value for*/
/*           filter bandwidth.                                               */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_setAutoEnabled"
ViStatus _VI_FUNC rsnrpz_scope_setAutoEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean   autoEnabled
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = autoEnabled;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81") ||
        rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 7 ) )  //  7/7  =  SENS:TRAC:AVER:COUN:AUTO
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi, channel, (char*)"SENS:TRAC:AVER:COUN:AUTO", &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Auto Averaging State                                  */
/* Purpose:  This function queries the setting of automatic switchover of    */
/*           filter bandwidth.                                               */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_getAutoEnabled"
ViStatus _VI_FUNC rsnrpz_scope_getAutoEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean*  pAutoEnabled
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81") ||
        rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 7 ) )  //  7/7  =  SENS:TRAC:AVER:COUN:AUTO
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:AVER:COUN:AUTO",
                                &value, sizeof(value)));

    if ( pAutoEnabled )
        *pAutoEnabled = (ViBoolean)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Scope Equivalent Sampling                                   */
/* Purpose:  This function activates or deactivates the automatic equivalent */
/*           sampling if the resolution of the trace measurement is          */
/*           configured lower than a sample period.                          */
/*                                                                           */
/*           Note:                                                           */
/*           1) This function is only available for NRP-Z81.                 */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_setEquivalentSampling"
ViStatus _VI_FUNC rsnrpz_scope_setEquivalentSampling (ViSession vi,
                                                      ViInt32 channel,
                                                      ViBoolean scopeEquivalentSampling)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = scopeEquivalentSampling;

    CHECKCHANNEL(vi, channel);

    if ( ! rsnrpz_IsSupported( vi, channel, 7, 18 ) )  //  7/18  =  SENS:TRAC:ESAM:AUTO
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:TRAC:ESAM:AUTO",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Equivalent Sampling                                   */
/* Purpose:  This function returns the state of the automatic equivalent     */
/*           sampling if the resolution of the trace measurement is          */
/*           configured lower than a sample period.                          */
/*                                                                           */
/*           Note:                                                           */
/*           1) This function is only available for NRP-Z81.                 */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_getEquivalentSampling"
ViStatus _VI_FUNC rsnrpz_scope_getEquivalentSampling (ViSession vi,
                                                      ViInt32 channel,
                                                      ViBoolean *scopeEquivalentSampling)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    if ( ! rsnrpz_IsSupported( vi, channel, 7, 18 ) )  //  7/18  =  SENS:TRAC:ESAM:AUTO
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
        channel,
        (char*)"SENS:TRAC:ESAM:AUTO",
        &value, sizeof(value)));

    if ( scopeEquivalentSampling )
        *scopeEquivalentSampling = (ViBoolean)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Scope Measurements State                                    */
/* Purpose:  This function turns on or off the automatic pulse measurement   */
/*           feature.                                                        */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_meas_setMeasEnabled"
ViStatus _VI_FUNC rsnrpz_scope_meas_setMeasEnabled (ViSession vi,
                                           ViInt32 channel,
                                           ViBoolean traceMeasurements)
{
    SSessionInfo  *psession_info;
    ViStatus       rsnrpz_status = VI_SUCCESS;
    ViInt32        value         = ((traceMeasurements) ? VI_TRUE : VI_FALSE);
    int            i             = 0;
    //NRP_SESSION*   plSession     = 0;
    //NRP_SESSION    lNrpSession   = 0;
    SMainSession  *pmain_session = 0;

    CHECKCHANNEL(vi, channel);

    // get associated session and check
    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    //plSession     = pmain_session->pSessions;
    //lNrpSession   = pmain_session->pSessions[channel-1];
    psession_info = pmain_session->pSInfos[channel-1];

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr( vi, channel, (char*)"NRP-Z81" ) )
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 16, 1 ) )  //  16/1  =  SENS:TRAC:MEAS:STAT
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    psession_info->IsPulseMeasurementOn = traceMeasurements;
    psession_info->PulseHighestParamNo  = 0;
    if ( psession_info->IsPulseMeasurementOn != VI_FALSE )
    {
        // Find highest parameter number of pulse measurements
        // in order to wait for ALL pulse parameter in isMeasurementComplete()
        for ( i = 0; gs_NrpCommands[i].command; i++ )
        {
            if ( gs_NrpCommands[i].groupNb == 16 )
            {
                long lIndex = i;

                // indexes of 'gs_NrpCommands[]' and 'psession_info->CacheTable[]'
                // directly correspond to each other. Therefore we don't need
                // to search the index from Group/Param

/*
                ViStatus error;
                error = rsnrpz_GetIndexFromPacket( gs_NrpCommands[i].groupNb,
                                                   gs_NrpCommands[i].paramNb,
                                                   &lIndex );
*/
                if ( rsnrpz_CheckRange( psession_info->CacheTable,
                                        lIndex,
                                        0 ) != RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED )
                {
                    if ( gs_NrpCommands[i].paramNb > psession_info->PulseHighestParamNo )
                        psession_info->PulseHighestParamNo = gs_NrpCommands[i].paramNb;
                }
            }
        }
    }

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:TRAC:MEAS:STAT",
                                   &value));

    // Do NOT remove the activation of AUTOMATIC sending of pulse parameter
    // because otherwise we later can't get the values from cache !!!
    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:TRAC:MEAS:AUTO",
                                   &value));

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Measurements State                                    */
/* Purpose:  This function reads the state of filter function of a sensor.   */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_meas_getMeasEnabled"
ViStatus _VI_FUNC rsnrpz_scope_meas_getMeasEnabled (ViSession vi,
                                           ViInt32 channel,
                                           ViBoolean *pTraceMeasurements)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr( vi, channel, (char*)"NRP-Z81" ) )
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 16, 1 ) )  //  16/1  =  SENS:TRAC:MEAS:STAT
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:MEAS:STAT",
                                &value, sizeof(value)));

    if ( pTraceMeasurements )
        *pTraceMeasurements = (ViBoolean)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Scope Measurements Algorithm                                */
/* Purpose:  This function selects the algorithm for detecting the top and   */
/*           the base level of the pulsed signal.                            */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_meas_setMeasAlgorithm"
ViStatus _VI_FUNC rsnrpz_scope_meas_setMeasAlgorithm (ViSession vi,
                                             ViInt32 channel, ViInt32 algorithm)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr( vi, channel, (char*)"NRP-Z81" ) )
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 16, 2 ) )  //  16/2  =  SENS:TRAC:MEAS:ALG
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:TRAC:MEAS:ALG",
                                   &algorithm));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Measurements Algorithm                                */
/* Purpose:  This function queries selected algorithm for detecting the top  */
/*           and the base level of the pulsed signal.                        */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_meas_getMeasAlgorithm"
ViStatus _VI_FUNC rsnrpz_scope_meas_getMeasAlgorithm (ViSession vi,
                                             ViInt32 channel, ViInt32 *pAlgorithm)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;
    ViInt32     index;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr( vi, channel, (char*)"NRP-Z81" ) )
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 16, 2 ) )  //  16/2  =  SENS:TRAC:MEAS:ALG
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR (rsnrpz_GetIndexFromCommand ((char*)"SENS:TRAC:MEAS:ALG", &index));

    gs_NrpCommands[index].RangeTable = gs_AlgorithmReadRangeTable;

    rsnrpz_status = rsnrpz_ReadValue( vi, channel, (char*)"SENS:TRAC:MEAS:ALG", &value, sizeof (value));

    gs_NrpCommands[index].RangeTable = gs_AlgorithmRangeTable;

    CHECKERR( rsnrpz_status );
    CHECKERR( rsnrpz_checkStatus ( vi));

    *pAlgorithm = value;

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Scope Measurements Level Thresholds                         */
/* Purpose:  This function defines the thresholds of the reference and       */
/*           transition levels that are used for the calculation  of the     */
/*           pulse's time parameter.                                         */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_meas_setLevelThresholds"
ViStatus _VI_FUNC rsnrpz_scope_meas_setLevelThresholds (ViSession vi,
                                                   ViInt32 channel,
                                                   ViReal64 durationRef,
                                                   ViReal64 transitionLowRef,
                                                   ViReal64 transitionHighRef)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)durationRef;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr( vi, channel, (char*)"NRP-Z81" ) )
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 16, 4 ) )  //  16/4  =  SENS:TRAC:MEAS:DUR:REF
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:TRAC:MEAS:DEF:DUR:REF",
                                   &value));
    value = (float)transitionLowRef;
    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:TRAC:MEAS:DEF:TRAN:LREF",
                                   &value));
    value = (float)transitionHighRef;
    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:TRAC:MEAS:DEF:TRAN:HREF",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Measurements Level Thresholds                         */
/* Purpose:  This function queries the thresholds of the reference and       */
/*           transition levels that are used for the calculation  of the     */
/*           pulse's time parameter.                                         */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_meas_getLevelThresholds"
ViStatus _VI_FUNC rsnrpz_scope_meas_getLevelThresholds (ViSession vi,
                                                   ViInt32 channel,
                                                   ViReal64 *pDurationRef,
                                                   ViReal64 *pTransitionLowRef,
                                                   ViReal64 *pTransitionHighRef)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr( vi, channel, (char*)"NRP-Z81" ) )
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 16, 4 ) )  //  16/4  =  SENS:TRAC:MEAS:DUR:REF
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:MEAS:DEF:DUR:REF",
                                &value, sizeof(value)));

    if ( pDurationRef )
        *pDurationRef = (ViReal64)value;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:MEAS:DEF:TRAN:LREF",
                                &value, sizeof(value)));

    if ( pTransitionLowRef )
        *pTransitionLowRef = (ViReal64)value;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:MEAS:DEF:TRAN:HREF",
                                &value, sizeof(value)));

    if ( pTransitionHighRef )
        *pTransitionHighRef = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Set Scope Measurements Time                                     */
/* Purpose:  This function defines measurement time which sets the duration  */
/*           of analysing the current trace for the pulse parameters. The    */
/*           measurement time could be used together with the measurement    */
/*           offset time to select the second (or any other) pulse in the    */
/*           trace and not the whole trace.                                  */
/*                                                                           */
/*           Note:                                                           */
/*           1) This function is only available for NRP-Z81.                 */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_meas_setTime"
ViStatus _VI_FUNC rsnrpz_scope_meas_setTime (ViSession vi,
                                             ViInt32 channel, ViReal64 measTime)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)measTime;

    CHECKCHANNEL(vi, channel);

    if ( ! rsnrpz_IsSupported( vi, channel, 17, 2 ) )  //  17/2  =  SENS:TRAC:MEAS:TIME
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:TRAC:MEAS:TIME",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Get Scope Measurements Time                                     */
/* Purpose:  This function returns the measurement time.                     */
/*                                                                           */
/*           Note:                                                           */
/*           1) This function is only available for NRP-Z81.                 */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_meas_getTime"
ViStatus _VI_FUNC rsnrpz_scope_meas_getTime (ViSession vi,
                                             ViInt32 channel, ViReal64 *measTime)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    if ( ! rsnrpz_IsSupported( vi, channel, 17, 2 ) )  //  17/2  =  SENS:TRAC:MEAS:TIME
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:MEAS:TIME",
                                &value, sizeof(value)));

    if ( measTime )
        *measTime = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Scope Measurements Offset Time                              */
/* Purpose:  This function defines offset time used to set the start point of*/
/*           analysing the current trace for the pulse parameters.           */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_meas_setOffsetTime"
ViStatus _VI_FUNC rsnrpz_scope_meas_setOffsetTime (ViSession vi,
                                              ViInt32 channel,
                                              ViReal64 offsetTime)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)offsetTime;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr( vi, channel, (char*)"NRP-Z81" ) )
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 16, 7 ) )  //  16/7  =  SENS:TRAC:MEAS:OFFS:TIME
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:TRAC:MEAS:OFFS:TIME",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Measurements Offset Time                              */
/* Purpose:  This function queries offset time used to set the start point of*/
/*           analysing the current trace for the pulse parameters.           */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_meas_getOffsetTime"
ViStatus _VI_FUNC rsnrpz_scope_meas_getOffsetTime (ViSession vi,
                                              ViInt32 channel,
                                              ViReal64 *pOffsetTime)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr( vi, channel, (char*)"NRP-Z81" ) )
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 16, 7 ) )  //  16/7  =  SENS:TRAC:MEAS:OFFS:TIME
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:MEAS:OFFS:TIME",
                                &value, sizeof(value)));

    if ( pOffsetTime )
        *pOffsetTime = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Measurements Pulse Times                              */
/* Purpose:  This function returns the calculated pulse time parameters of   */
/*           the last recorded trace.                                        */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_meas_getPulseTimes"
ViStatus _VI_FUNC rsnrpz_scope_meas_getPulseTimes (ViSession vi,
                                              ViInt32 channel,
                                              ViReal64 *pDutyCycle,
                                              ViReal64 *pPulseDuration,
                                              ViReal64 *pPulsePeriod)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr( vi, channel, (char*)"NRP-Z81" ) )
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 16, 8 ) )  //  16/8  =  SENS:TRAC:MEAS:PULS:DCYC
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:MEAS:PULS:DCYC",
                                &value, sizeof(value)));

    if ( pDutyCycle )
        *pDutyCycle = (ViReal64)value;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:MEAS:PULS:DUR",
                                &value, sizeof(value)));

    if ( pPulseDuration )
        *pPulseDuration = (ViReal64)value;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:MEAS:PULS:PER",
                                &value, sizeof(value)));

    if ( pPulsePeriod )
        *pPulsePeriod = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Measurements Pulse Transition                         */
/* Purpose:  This function returns the transition parameters of the last     */
/*           examined trace data.                                            */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_meas_getPulseTransition"
ViStatus _VI_FUNC rsnrpz_scope_meas_getPulseTransition (ViSession vi,
                                                   ViInt32 channel,
                                                   ViInt32 slope,
                                                   ViReal64 *pDuration,
                                                   ViReal64 *pOccurence,
                                                   ViReal64 *pOvershoot)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;
    ViChar      buffer [50];
    const char *transition_arr [] = {"POS","NEG"};

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr( vi, channel, (char*)"NRP-Z81" ) )
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 16, 12 ) )  //  16/12  =  SENS:TRAC:MEAS:TRAN:NEG:DUR
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    if ( (slope < 0) || (slope > 1) )
        return RSNRPZ_ERROR_INVALID_VALUE;

    sprintf (buffer, "SENS:TRAC:MEAS:TRAN:%s:DUR", transition_arr[slope]);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                buffer,
                                &value, sizeof(value)));

    if ( pDuration )
        *pDuration = (ViReal64)value;

    sprintf (buffer, "SENS:TRAC:MEAS:TRAN:%s:OCC", transition_arr[slope]);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                buffer,
                                &value, sizeof(value)));

    if ( pOccurence )
        *pOccurence = (ViReal64)value;

    sprintf (buffer, "SENS:TRAC:MEAS:TRAN:%s:OVER", transition_arr[slope]);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                buffer,
                                &value, sizeof(value)));

    if ( pOvershoot )
        *pOvershoot = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Measurements Pulse Power                              */
/* Purpose:  This function returns the average power, the minimum level and  */
/*           the maximum level of the analysed trace in Watt.                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_meas_getPulsePower"
ViStatus _VI_FUNC rsnrpz_scope_meas_getPulsePower (ViSession vi,
                                              ViInt32 channel, ViReal64 *pAverage,
                                              ViReal64 *pMinPeak,
                                              ViReal64 *pMaxPeak)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr( vi, channel, (char*)"NRP-Z81" ) )
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 16, 20 ) )  //  16/20  =  SENS:TRAC:MEAS:POW:AVG
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:MEAS:POW:AVG",
                                &value, sizeof(value)));

    if ( pAverage )
        *pAverage = (ViReal64)value;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:MEAS:POW:MIN",
                                &value, sizeof(value)));

    if ( pMinPeak )
        *pMinPeak = (ViReal64)value;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:MEAS:POW:MAX",
                                &value, sizeof(value)));

    if ( pMaxPeak )
        *pMaxPeak = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Measurements Pulse Levels                             */
/* Purpose:  This function returns the pulse top level and the pulse base    */
/*           level in watt.                                                  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_meas_getPulseLevels"
ViStatus _VI_FUNC rsnrpz_scope_meas_getPulseLevels (ViSession vi,
                                               ViInt32 channel,
                                               ViReal64 *pTopLevel,
                                               ViReal64 *pBaseLevel)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr( vi, channel, (char*)"NRP-Z81" ) )
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 16, 21 ) )  //  16/21  =  SENS:TRAC:MEAS:POW:PULS:TOP
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:MEAS:POW:PULS:TOP",
                                &value, sizeof(value)));

    if ( pTopLevel )
        *pTopLevel = (ViReal64)value;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:MEAS:POW:PULS:BASE",
                                &value, sizeof(value)));

    if ( pBaseLevel )
        *pBaseLevel = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Measurements Pulse Reference Levels                   */
/* Purpose:  This function returns the calculated reference level in Watt at */
/*           the defined thresholds of the last recorded trace.              */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_meas_getPulseReferenceLevels"
ViStatus _VI_FUNC rsnrpz_scope_meas_getPulseReferenceLevels (ViSession vi,
                                                        ViInt32 channel,
                                                        ViReal64 *pLowRefLevel,
                                                        ViReal64 *pHighRefLevel,
                                                        ViReal64 *pDurationRefLevel)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr( vi, channel, (char*)"NRP-Z81" ) )
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 16, 23 ) )  //  16/23  =  SENS:TRAC:MEAS:POW:REF
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:MEAS:POW:LREF",
                                &value, sizeof(value)));

    if ( pLowRefLevel )
        *pLowRefLevel = (ViReal64)value;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:MEAS:POW:HREF",
                                &value, sizeof(value)));

    if ( pHighRefLevel )
        *pHighRefLevel = (ViReal64)value;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:MEAS:POW:REF",
                                &value, sizeof(value)));

    if ( pDurationRefLevel )
        *pDurationRefLevel = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Set Scope Measurement Equivalent Sampling                       */
/* Purpose:  This function activates or deactivates the automatic equivalent */
/*           sampling mode during automatic pulse analysis.                  */
/*           If equivalent sampling is enabled, the sensor tries to measure  */
/*           the time parameters (mainly rise/fall times) of the pulse with  */
/*           a high resolution by doing equivalent sampling.                 */
/*           To do the equivalent sampling a periodic signal is mandatory.   */
/*           The sensor decides automatically if equivalent  sampling is     */
/*           possible. To get the resolution of the measured time parameter  */
/*           the function "rsnrpz_scope_meas_getSamplePeriod" could be used. */
/*                                                                           */
/*           Note:                                                           */
/*           1) This function is only available for NRP-Z81.                 */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_meas_setEquivalentSampling"
ViStatus _VI_FUNC rsnrpz_scope_meas_setEquivalentSampling (ViSession vi,
                                                           ViInt32 channel,
                                                           ViBoolean scopeMeasEquivSampling)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = scopeMeasEquivSampling;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:TRAC:MEAS:TRAN:ESAM:AUTO",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Measurement Equivalent Sampling                       */
/* Purpose:  This function returns the state of the automatic equivalent     */
/*           sampling mode during automatic pulse analysis.                  */
/*                                                                           */
/*           Note:                                                           */
/*           1) This function is only available for NRP-Z81.                 */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_meas_getEquivalentSampling"
ViStatus _VI_FUNC rsnrpz_scope_meas_getEquivalentSampling (ViSession vi,
                                                           ViInt32 channel,
                                                           ViBoolean *scopeMeasEquivSampling)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
        channel,
        (char*)"SENS:TRAC:MEAS:TRAN:ESAM:AUTO",
        &value, sizeof(value)));

    if ( scopeMeasEquivSampling )
        *scopeMeasEquivSampling = (ViBoolean)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Measurement Sample Period                             */
/* Purpose:  This function returns the sample period (in s) of the last      */
/*           pulse analysis. This parameter is a good indicator if the       */
/*           equivalent sampling mode of measuring the rise and fall times   */
/*           was successful or not.                                          */
/*                                                                           */
/*           Note:                                                           */
/*           1) This function is only available for NRP-Z81.                 */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_meas_getSamplePeriod"
ViStatus _VI_FUNC rsnrpz_scope_meas_getSamplePeriod (ViSession vi,
                                                     ViInt32 channel,
                                                     ViReal64 *samplePeriod)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    if ( ! rsnrpz_IsSupported( vi, channel, 16, 26 ) )  //  16/26  =  SENS:TRAC:MEAS:TRAN:SPER
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:MEAS:TRAN:SPER",
                                &value, sizeof(value)));

    if ( samplePeriod )
        *samplePeriod = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Configure Internal                                              */
/* Purpose:  This function configures the parameters of internal trigger     */
/*           system.                                                         */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_configureInternal"
ViStatus _VI_FUNC rsnrpz_trigger_configureInternal(
    ViSession   vi,
    ViInt32     channel,
    ViReal64    triggerLevel,
    ViInt32     triggerSlope
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR(rsnrpz_trigger_setSource (vi, channel,
                                      RSNRPZ_TRIGGER_SOURCE_INTERNAL));

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5") )
#else
    if ( rsnrpz_IsSupported( vi, channel, 9, 6 ) )  //  9/6  =  TRIG:ATR:STAT
#endif
    {
        CHECKERR(rsnrpz_trigger_setAutoTriggerEnabled (vi, channel, VI_FALSE));
    }

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81") )
#else
    if ( rsnrpz_IsSupported( vi, channel, 9, 9 ) )  //  9/9  =  TRIG:DEL:AUTO
#endif
    {
        CHECKERR(rsnrpz_trigger_setAutoDelayEnabled (vi, channel, VI_TRUE));
    }

    CHECKERR(rsnrpz_trigger_setDelay (vi, channel, 0.0));
    CHECKERR(rsnrpz_trigger_setCount (vi, channel, 1));
    CHECKERR(rsnrpz_trigger_setHoldoff (vi, channel, 0.0));
    CHECKERR(rsnrpz_trigger_setHysteresis (vi, channel, 3.0));
    CHECKERR(rsnrpz_trigger_setLevel (vi, channel, triggerLevel));

    CHECKERR( rsnrpz_trigger_setSlope(vi, channel, triggerSlope));

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Set Scope Auto Max Measuring Time                               */
/* Purpose:  This function sets an upper time limit can be set.              */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_setAutoMaxMeasuringTime"
ViStatus _VI_FUNC rsnrpz_scope_setAutoMaxMeasuringTime(
    ViSession   vi,
    ViInt32     channel,
    ViReal64    upperTimeLimit
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)upperTimeLimit;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81") ||
        rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 8 ) )  //  7/8  =  SENS:TRAC:AVER:COUN:AUTO:MTIM
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:TRAC:AVER:COUN:AUTO:MTIM",
                                   &value));

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Auto Max Measuring Time                               */
/* Purpose:  This function queries an upper time limit.                      */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_getAutoMaxMeasuringTime"
ViStatus _VI_FUNC rsnrpz_scope_getAutoMaxMeasuringTime(
    ViSession   vi,
    ViInt32     channel,
    ViReal64*   upperTimeLimit
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81") ||
        rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 8 ) )  //  7/8  =  SENS:TRAC:AVER:COUN:AUTO:MTIM
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:AVER:COUN:AUTO:MTIM",
                                &value, sizeof(value)));

    if ( upperTimeLimit )
        *upperTimeLimit = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Scope Auto Noise Signal Ratio                               */
/* Purpose:  This function sets the maximum noise ratio in the measurement   */
/*           result.                                                         */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_setAutoNoiseSignalRatio"
ViStatus _VI_FUNC rsnrpz_scope_setAutoNoiseSignalRatio(
    ViSession   vi,
    ViInt32     channel,
    ViReal64    maximumNoiseRatio
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)maximumNoiseRatio;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81") ||
        rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 11 ) )  //  7/11  =  SENS:TRAC:AVER:COUN:AUTO:NSR
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:TRAC:AVER:COUN:AUTO:NSR",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Auto Noise Signal Ratio                               */
/* Purpose:  This function queries the maximum noise signal ratio value.     */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_getAutoNoiseSignalRatio"
ViStatus _VI_FUNC rsnrpz_scope_getAutoNoiseSignalRatio(
    ViSession   vi,
    ViInt32     channel,
    ViReal64*   maximumNoiseRatio
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81") ||
        rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 11 ) )  //  7/11  =  SENS:TRAC:AVER:COUN:AUTO:NSR
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:AVER:COUN:AUTO:NSR",
                                &value, sizeof(value)));

    if ( maximumNoiseRatio )
        *maximumNoiseRatio = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Scope Auto Resolution                                       */
/* Purpose:  This function defines the number of significant places          */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_setAutoResolution"
ViStatus _VI_FUNC rsnrpz_scope_setAutoResolution(
    ViSession   vi,
    ViInt32     channel,
    ViInt32     resolution
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81") ||
        rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 9 ) )  //  7/9  =  SENS:TRAC:AVER:COUN:AUTO:RES
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:TRAC:AVER:COUN:AUTO:RES",
                                   &resolution));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Auto Resolution                                       */
/* Purpose:  This function reads the number of significant places            */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_getAutoResolution"
ViStatus _VI_FUNC rsnrpz_scope_getAutoResolution(
    ViSession   vi,
    ViInt32     channel,
    ViInt32*    resolution
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81") ||
        rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 9 ) )  //  7/9  =  SENS:TRAC:AVER:COUN:AUTO:RES
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:AVER:COUN:AUTO:RES",
                                &value, sizeof(value)));

    if ( resolution )
        *resolution = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Scope Auto Type                                                   */
/* Purpose:  This function selects a method by which the automatic filter    */
/*           length switchover can operate.                                  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_setAutoType"
ViStatus _VI_FUNC rsnrpz_scope_setAutoType(
    ViSession   vi,
    ViInt32     channel,
    ViInt32     method
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81") ||
        rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 12 ) )  //  7/12  =  SENS:TRAC:AVER:COUN:AUTO:TYPE
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:TRAC:AVER:COUN:AUTO:TYPE",
                                   &method));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Scope Auto Type                                                   */
/* Purpose:  This function returns a method by which the automatic filter    */
/*           length switchover can operate.                                  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_scope_getAutoType"
ViStatus _VI_FUNC rsnrpz_scope_getAutoType(
    ViSession   vi,
    ViInt32     channel,
    ViInt32*    pMethod
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81") ||
        rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 7, 12 ) )  //  7/12  =  SENS:TRAC:AVER:COUN:AUTO:TYPE
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:TRAC:AVER:COUN:AUTO:TYPE",
                                &value, sizeof(value)));

    if ( pMethod )
        *pMethod = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Configure External                                              */
/* Purpose:  This function configures the parameters of external trigger     */
/*           system.                                                         */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_configureExternal"
ViStatus _VI_FUNC rsnrpz_trigger_configureExternal(
    ViSession   vi,
    ViInt32     channel,
    ViReal64    triggerDelay
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_trigger_setSource (vi, channel,
                                      RSNRPZ_TRIGGER_SOURCE_EXTERNAL));

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"))
#else
    if ( rsnrpz_IsSupported( vi, channel, 9, 6 ) )  //  9/6  =  TRIG:ATR:STAT
#endif
    {
        CHECKERR( rsnrpz_trigger_setAutoTriggerEnabled (vi, channel, VI_FALSE));
    }

#if defined (USE_CHECKINSTR)
    if ( ! rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81") != VI_SUCCESS)
#else
    if ( rsnrpz_IsSupported( vi, channel, 9, 9 ) )  //  9/9  =  TRIG:DEL:AUTO
#endif
    {
        CHECKERR( rsnrpz_trigger_setAutoDelayEnabled (vi, channel, VI_TRUE));
    }

    CHECKERR( rsnrpz_trigger_setDelay (vi, channel, triggerDelay));
    CHECKERR( rsnrpz_trigger_setCount (vi, channel, 1));
    CHECKERR( rsnrpz_trigger_setHoldoff (vi, channel, 0.0));

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Immediate Trigger                                               */
/* Purpose:  This function performs triggering                               */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_immediate"
ViStatus _VI_FUNC rsnrpz_trigger_immediate(
    ViSession   vi,
    ViInt32     channel
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi, channel, (char*)"TRIG:IMM", NULL));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Trigger Auto Delay State                                    */
/* Purpose:  This function sets the setting of auto delay feature.           */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_setAutoDelayEnabled"
ViStatus _VI_FUNC rsnrpz_trigger_setAutoDelayEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean   autoDelay
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = autoDelay;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"TRIG:DEL:AUTO",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Trigger Auto Delay State                                    */
/* Purpose:  This function reads the setting of auto delay feature.          */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_getAutoDelayEnabled"
ViStatus _VI_FUNC rsnrpz_trigger_getAutoDelayEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean*  pAutoDelay
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"TRIG:DEL:AUTO",
                                &value, sizeof(value)));

    if ( pAutoDelay )
        *pAutoDelay = (ViBoolean)value;
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Auto Trigger State                                          */
/* Purpose:  This function turns On or Off the auto trigger feature.         */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_setAutoTriggerEnabled"
ViStatus _VI_FUNC rsnrpz_trigger_setAutoTriggerEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean   autoTrigger
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = autoTrigger;

    CHECKCHANNEL(vi, channel);


#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 9, 6 ) )  //  9/6  =  TRIG:ATR:STAT
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"TRIG:ATR:STAT",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Auto Trigger State                                          */
/* Purpose:  This function reads the state of Auto Trigger.                  */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_getAutoTriggerEnabled"
ViStatus _VI_FUNC rsnrpz_trigger_getAutoTriggerEnabled(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean*  pAutoTrigger
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 9, 6 ) )  //  9/6  =  TRIG:ATR:STAT
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"TRIG:ATR:STAT",
                                &value, sizeof(value)));

    if ( pAutoTrigger )
        *pAutoTrigger = (ViBoolean)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Trigger Count                                               */
/* Purpose:  This function sets the number of measurement cycles to be       */
/*           performed when the measurement is started with INIT.            */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_setCount"
ViStatus _VI_FUNC rsnrpz_trigger_setCount(
    ViSession   vi,
    ViInt32     channel,
    ViInt32     triggerCount
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"TRIG:COUN",
                                   &triggerCount));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Trigger Count                                               */
/* Purpose:  This function reads the number of measurement cycles to be      */
/*           performed when the measurement is started with INIT.            */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_getCount"
ViStatus _VI_FUNC rsnrpz_trigger_getCount(
    ViSession   vi,
    ViInt32     channel,
    ViInt32*    pTriggerCount
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"TRIG:COUN",
                                &value, sizeof(value)));

    if ( pTriggerCount )
        *pTriggerCount = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Trigger Delay                                               */
/* Purpose:  This function defines the delay between the trigger event and   */
/*           the beginning of the actual measurement (integration).          */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_setDelay"
ViStatus _VI_FUNC rsnrpz_trigger_setDelay(
    ViSession   vi,
    ViInt32     channel,
    ViReal64    triggerDelay
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)triggerDelay;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"TRIG:DEL",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Trigger Delay                                               */
/* Purpose:  This function reads value of the delay (in seconds) between the */
/*           trigger event and the beginning of the actual measurement       */
/*           (integration).                                                  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_getDelay"
ViStatus _VI_FUNC rsnrpz_trigger_getDelay(
    ViSession   vi,
    ViInt32     channel,
    ViReal64*   pTriggerDelay
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"TRIG:DEL",
                                &value, sizeof(value)));

    if ( pTriggerDelay )
        *pTriggerDelay = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Trigger Holdoff                                             */
/* Purpose:  This function defines a period after a trigger event within     */
/*           which all further trigger events are ignored.                   */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_setHoldoff"
ViStatus _VI_FUNC rsnrpz_trigger_setHoldoff(
    ViSession   vi,
    ViInt32     channel,
    ViReal64    triggerHoldoff
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)triggerHoldoff;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"TRIG:HOLD",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Trigger Holdoff                                             */
/* Purpose:  This function returns the value of a period after a trigger     */
/*           event within which all further trigger events are ignored.      */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_getHoldoff"
ViStatus _VI_FUNC rsnrpz_trigger_getHoldoff(
    ViSession   vi,
    ViInt32     channel,
    ViReal64*   pTriggerHoldoff
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"TRIG:HOLD",
                                &value, sizeof(value)));

    if ( pTriggerHoldoff )
        *pTriggerHoldoff = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Trigger Hysteresis                                          */
/* Purpose:  This function is used to specify how far the signal level has   */
/*           to drop below the trigger level before a new signal edge can be */
/*           detected as a trigger event.                                    */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_setHysteresis"
ViStatus _VI_FUNC rsnrpz_trigger_setHysteresis(
    ViSession   vi,
    ViInt32     channel,
    ViReal64    triggerHysteresis
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)triggerHysteresis;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"TRIG:HYST",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Trigger Hysteresis                                          */
/* Purpose:  This function reads the value of trigger hysteresis.            */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_getHysteresis"
ViStatus _VI_FUNC rsnrpz_trigger_getHysteresis(
    ViSession   vi,
    ViInt32     channel,
    ViReal64*   pTriggerHysteresis
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"TRIG:HYST",
                                &value, sizeof(value)));

    if ( pTriggerHysteresis )
        *pTriggerHysteresis = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Trigger Level                                               */
/* Purpose:  This function determines the power a trigger signal must exceed */
/*           before a trigger event is detected. This setting is only used   */
/*           for internal trigger signal source.                             */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_setLevel"
ViStatus _VI_FUNC rsnrpz_trigger_setLevel(
    ViSession   vi,
    ViInt32     channel,
    ViReal64    triggerLevel
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)triggerLevel;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"TRIG:LEV",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Trigger Level                                               */
/* Purpose:  This function reads the power a trigger signal must exceed      */
/*           before a trigger event is detected.                             */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_getLevel"
ViStatus _VI_FUNC rsnrpz_trigger_getLevel(
    ViSession   vi,
    ViInt32     channel,
    ViReal64*   pTriggerLevel
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"TRIG:LEV",
                                &value, sizeof(value)));

    if ( pTriggerLevel )
        *pTriggerLevel = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Trigger Slope                                               */
/* Purpose:  This function determines whether the rising (POSitive) or the   */
/*           falling (NEGative) edge of the signal is used for triggering.   */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_setSlope"
ViStatus _VI_FUNC rsnrpz_trigger_setSlope(
    ViSession   vi,
    ViInt32     channel,
    ViInt32     triggerSlope
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"TRIG:SLOP",
                                   &triggerSlope));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Trigger Slope                                               */
/* Purpose:  This function reads whether the rising (POSitive) or the falling*/
/*           (NEGative) edge of the signal is used for triggering.           */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_getSlope"
ViStatus _VI_FUNC rsnrpz_trigger_getSlope(
    ViSession   vi,
    ViInt32     channel,
    ViInt32*    pTriggerSlope
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"TRIG:SLOP",
                                &value, sizeof(value)));

    if ( pTriggerSlope )
        *pTriggerSlope = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Trigger Source                                              */
/* Purpose:  This function sets the trigger signal source for the            */
/*           WAIT_FOR_TRG state.                                             */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_setSource"
ViStatus _VI_FUNC rsnrpz_trigger_setSource(
    ViSession   vi,
    ViInt32     channel,
    ViInt32     triggerSource
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"TRIG:SOUR",
                                   &triggerSource));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Trigger Source                                              */
/* Purpose:  This function gets the trigger signal source for the            */
/*           WAIT_FOR_TRG state.                                             */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_trigger_getSource"
ViStatus _VI_FUNC rsnrpz_trigger_getSource(
    ViSession   vi,
    ViInt32     channel,
    ViInt32*    pTriggerSource
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"TRIG:SOUR",
                                &value, sizeof(value)));

    if ( pTriggerSource )
        *pTriggerSource = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Status Update Time                                          */
/* Purpose:  This function sets status update time, which influences USB     */
/*           traffic during sensor's waiting for trigger.                    */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_system_setStatusUpdateTime"
ViStatus _VI_FUNC rsnrpz_system_setStatusUpdateTime (ViSession vi,
                                            ViInt32 channel,
                                            ViReal64 statusUpdateTime)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)statusUpdateTime;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi, channel, (char*)"SYST:SUT", &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Status Update Time                                          */
/* Purpose:  This function gets status update time.                          */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_system_getStatusUpdateTime"
ViStatus _VI_FUNC rsnrpz_system_getStatusUpdateTime (ViSession vi,
                                            ViInt32 channel,
                                            ViReal64 *statusUpdateTime)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SYST:SUT",
                                &value, sizeof(value)));

    if ( statusUpdateTime )
        *statusUpdateTime = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Result Update Time                                          */
/* Purpose:  This function sets result update time, which influences USB     */
/*           traffic if sensor is in continuous sweep mode.                  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_system_setResultUpdateTime"
ViStatus _VI_FUNC rsnrpz_system_setResultUpdateTime (ViSession vi,
                                            ViInt32 channel,
                                            ViReal64 resultUpdateTime)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = (float)resultUpdateTime;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi, channel, (char*)"SYST:RUT", &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Result Update Time                                          */
/* Purpose:  This function gets result update time.                          */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_system_getResultUpdateTime"
ViStatus _VI_FUNC rsnrpz_system_getResultUpdateTime (ViSession vi,
                                            ViInt32 channel,
                                            ViReal64 *resultUpdateTime)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SYST:RUT",
                                &value, sizeof(value)));

    if ( resultUpdateTime )
        *resultUpdateTime = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Test                                                            */
/* Purpose:  This function does internal test measurements with enabled and  */
/*           disabled heater and returns the power difference between both   */
/*           measurements.                                                   */
/*           The result of this test is used to determine the long time      */
/*           drift of the power sensor.                                      */
/*                                                                           */
/*           Note:                                                           */
/*           1) This function is available only for NRP-Z56 and NRP-Z57.     */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_calib_test"
ViStatus _VI_FUNC rsnrpz_calib_test (ViSession vi, ViInt32 channel,
                                     ViReal64 *calibTest)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    if ( ! rsnrpz_IsSupported( vi, channel, 1, 5 ) )  //  1/5  =  CAL:TEST
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"CAL:TEST",
                                &value, sizeof(value)));

    if ( calibTest )
        *calibTest = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Get Test Deviation                                              */
/* Purpose:  This function  first does an internal heater test with CAL:TEST */
/*           and returns the relative deviation between the test result and  */
/*           the result that was measured in the calibration lab during      */
/*           sensor calibration.                                             */
/*                                                                           */
/*           Note:                                                           */
/*           1) This function is available only for NRP-Z56 and NRP-Z57.     */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_calib_getTestDeviation"
ViStatus _VI_FUNC rsnrpz_calib_getTestDeviation (ViSession vi,
                                                 ViInt32 channel,
                                                 ViReal64 *testDeviation)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    if ( ! rsnrpz_IsSupported( vi, channel, 1, 6 ) )  //  1/6  =  CAL:TEST:DEV
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"CAL:TEST:DEV",
                                &value, sizeof(value)));

    if ( testDeviation )
        *testDeviation = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Get Test Reference                                              */
/* Purpose:  This function returns the heater test result that was measured  */
/*           in the calibration lab during sensor calibration                */
/*                                                                           */
/*           Note:                                                           */
/*           1) This function is available only for NRP-Z56 and NRP-Z57.     */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_calib_getTestReference"
ViStatus _VI_FUNC rsnrpz_calib_getTestReference (ViSession vi,
                                                 ViInt32 channel,
                                                 ViReal64 *testReference)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       value = 0.0;

    CHECKCHANNEL(vi, channel);

    if ( ! rsnrpz_IsSupported( vi, channel, 1, 7 ) )  //  1/7  =  CAL:TEST:REF
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"CAL:TEST:REF",
                                &value, sizeof(value)));

    if ( testReference )
        *testReference = (ViReal64)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Abort                                                           */
/* Purpose:  This function immediately sets selected sensor to the IDLE      */
/*           state. Measurements in progress are interrupted.                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_abort"
ViStatus _VI_FUNC rsnrpz_chan_abort(
    ViSession   vi,
    ViInt32     channel
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi, channel, (char*)"ABOR", NULL));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Initiate                                                        */
/* Purpose:  This function starts a single-shot measurement on selected      */
/*           channel.                                                        */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_initiate"
ViStatus _VI_FUNC rsnrpz_chan_initiate(
    ViSession   vi,
    ViInt32     channel
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi, channel, (char*)"INIT:IMM", NULL));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Set Initiate Continuous State                                   */
/* Purpose:  This function selects either single-shot or continuous          */
/*           (free-running) measurement cycles.                              */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_setInitContinuousEnabled"
ViStatus _VI_FUNC rsnrpz_chan_setInitContinuousEnabled(
    ViSession       vi,
    ViInt32         channel,
    ViBoolean       continuousInitiate
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = continuousInitiate;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"INIT:CONT",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Initiate Continuous State                                   */
/* Purpose:  This function returns whether single-shot or continuous         */
/*           (free-running) measurement is selected.                         */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_getInitContinuousEnabled"
ViStatus _VI_FUNC rsnrpz_chan_getInitContinuousEnabled(
    ViSession       vi,
    ViInt32         channel,
    ViBoolean*      pContinuousInitiate
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"INIT:CONT",
                                &value, sizeof(value)));

    if ( pContinuousInitiate )
        *pContinuousInitiate = (ViBoolean)value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Reset                                                           */
/* Purpose:  This function prompts the basic unit to send an *RST to the     */
/*           respective sensor. Measurements in progress are interrupted.    */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_reset"
ViStatus _VI_FUNC rsnrpz_chan_reset(
    ViSession   vi,
    ViInt32     channel
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi, channel, (char*)"*RST", NULL));
    CHECKERR( rsnrpz_defaultInstrSetup( vi, channel));

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Sampling Frequency                                          */
/* Purpose:  This function selects the sampling frequency.                   */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_setSamplingFrequency"
ViStatus _VI_FUNC rsnrpz_chan_setSamplingFrequency(
    ViSession       vi,
    ViInt32         channel,
    ViInt32         samplingFrequency
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81") ||
        rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5")  ||
        rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z9"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 3, 17 ) )  //  3/17  =  SENS:SAMP
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:SAMP",
                                   &samplingFrequency));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Sampling Frequency                                          */
/* Purpose:  This function returns the selected sampling frequency.          */
/* Not supported for: NRP-Z51                                                */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_getSamplingFrequency"
ViStatus _VI_FUNC rsnrpz_chan_getSamplingFrequency(
    ViSession   vi,
    ViInt32     channel,
    ViInt32*    pSamplingFrequency
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

#if defined (USE_CHECKINSTR)
    if (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z81") ||
        rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5")  ||
        rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z9"))
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 3, 17 ) )  //  3/17  =  SENS:SAMP
#endif
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:SAMP",
                                &value, sizeof(value)));

    if ( pSamplingFrequency )
        *pSamplingFrequency = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Zero                                                            */
/* Purpose:  This function performs zeroing of selected sensor using the     */
/*           signal at the sensor input.                                     */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_zero"
ViStatus _VI_FUNC rsnrpz_chan_zero(
    ViSession   vi,
    ViInt32     channel
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = RSNRPZ_ONCE;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi, channel, (char*)"CAL:ZERO:AUTO", &value));

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Zero Advanced                                                   */
/* Purpose:  This function performs zeroing using the signal at the sensor   */
/*           input.                                                          */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_zeroAdvanced"
ViStatus _VI_FUNC rsnrpz_chan_zeroAdvanced( ViSession vi,
                                            ViInt32   channel,
                                            ViInt32   zeroing )
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = zeroing;

    if ( (zeroing != RSNRPZ_ZERO_LFR) && (zeroing != RSNRPZ_ZERO_UFR))
        return VI_ERROR_PARAMETER2;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi, channel, (char*)"CAL:ZERO:AUTO", &value));

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Zeroing Complete State                                      */
/* Purpose:  This function returns the state of zeroing of the sensor.       */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_isZeroComplete"
ViStatus _VI_FUNC rsnrpz_chan_isZeroComplete(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean*  pIsZeroingComplete
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    TRegister       meas_register;
    ViUInt32*        pregister_value;
    ViBoolean       is_zero_complete = VI_FALSE;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_QueryRegister( vi,
                                    OPERATION_CALIBRATING_STATUS_REGISTER,
                                    CONDITION_REGISTER,
                                    &meas_register));
    pregister_value = meas_register;

    if (((*pregister_value) & (1L << channel)) == 0)
        is_zero_complete = TRUE;

    if ( pIsZeroingComplete )
        *pIsZeroingComplete = is_zero_complete;

//    return rsnrpz_status;
    return rsnrpz_checkStatus(vi);
}

/*===========================================================================*/
/* Function: Get Measurement Complete State                                  */
/* Purpose:  This function returns the state of the measurement.             */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_isMeasurementComplete"
ViStatus _VI_FUNC rsnrpz_chan_isMeasurementComplete(
    ViSession   vi,
    ViInt32     channel,
    ViBoolean*  pIsMeasurementComplete
)
{
    ViStatus      rsnrpz_status    = VI_SUCCESS;
    ViBoolean     is_meas_complete = VI_FALSE;
    SMainSession *pmain_session    = 0;
    long          ErrorCode;

    CHECKCHANNEL(vi, channel);

    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    // check if sensor is still alive in low level driver
    if( ( rsnrpz_status = NrpGetError( pmain_session->pSessions[channel-1], &ErrorCode ) ) !=VI_SUCCESS )
        return rsnrpz_status;

    {
        SSessionInfo* psession_info;
        psession_info = pmain_session->pSInfos[channel-1];

        EnterCriticalSection( &psession_info->cs );

        if ( psession_info->IsInStatMode == VI_TRUE )
        {
            if ( (psession_info->IsMeasurementComplete != VI_TRUE)
              || (psession_info->IsArrayMeasValid      != VI_TRUE)
               )
                is_meas_complete = VI_FALSE;
            else
                is_meas_complete = VI_TRUE;
        }
        else if ( psession_info->IsPulseMeasurementOn == VI_TRUE )
        {
            if ( (psession_info->IsMeasurementComplete  != VI_TRUE)
              || (psession_info->IsPulseScalarsComplete != VI_TRUE)
               )
                is_meas_complete = VI_FALSE;
            else
                is_meas_complete = VI_TRUE;
        }
        else
        {
            if ( psession_info->IsMeasurementComplete != VI_TRUE )
                is_meas_complete = VI_FALSE;
            else
                is_meas_complete = VI_TRUE;
        }

        if ( is_meas_complete )
        {
            trace_log( "chan_IsMeasurementComplete = TRUE" );
            psession_info->IsMeasurementComplete  = VI_FALSE;
            psession_info->IsPulseScalarsComplete = VI_FALSE;
        }

        LeaveCriticalSection(&psession_info->cs);
    }

//Error:
    if ( pIsMeasurementComplete )
        *pIsMeasurementComplete = is_meas_complete;
    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Sensor Self Test                                                */
/* Purpose:  This function performs a sensor test and returns a list of      */
/*           strings separated by commas. The contents of this test protocol */
/*           is sensor-specific.                                             */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_selfTest"
ViStatus _VI_FUNC rsnrpz_chan_selfTest(
    ViSession       vi,
    ViInt32         channel,
    ViChar _VI_FAR  result[]
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViChar      buffer[BUFFER_SIZE] = "";
    ViChar*     pbuffer = buffer;
    ViChar*     pdynamic_buffer = NULL;

    CHECKCHANNEL(vi, channel);

    rsnrpz_status = rsnrpz_ReadValue( vi,
                                      channel,
                                      (char*)"TEST:SENS",
                                      buffer, sizeof(buffer));

    if ( rsnrpz_status > 0 )
    {
        pdynamic_buffer = (ViChar*) malloc(rsnrpz_status);
        memset(pdynamic_buffer, 0, rsnrpz_status);
        pbuffer = pdynamic_buffer;

        rsnrpz_status = rsnrpz_ReadValue( vi,
                                          channel,
                                          (char*)"TEST:SENS",
                                          pdynamic_buffer,
                                          rsnrpz_status);
    }
    CHECKERR_CU( rsnrpz_status );

    if ( result )
        strcpy( result, pbuffer);
    else
        rsnrpz_status = (int)strlen(pbuffer) + 1;

Error:
    if ( pdynamic_buffer != NULL )
        free(pdynamic_buffer);

    return rsnrpz_status;
}



/*===========================================================================*/
/* Function: Set Auxiliary Value                                             */
/* Purpose:  This function selects auxiliary values transfered with FETCh    */
/*           commands.                                                       */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_setAuxiliary"
ViStatus _VI_FUNC rsnrpz_chan_setAuxiliary (ViSession vi,
                                            ViInt32 channel,
                                            ViInt32 auxiliaryValue)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = auxiliaryValue;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi,
                                   channel,
                                   (char*)"SENS:AUX",
                                   &value));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Auxiliary Value                                             */
/* Purpose:  This function queries auxiliary values transfered with FETCh    */
/*           commands.                                                       */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_chan_getAuxiliary"
ViStatus _VI_FUNC rsnrpz_chan_getAuxiliary (ViSession vi,
                                            ViInt32 channel,
                                            ViInt32 *auxiliaryValue)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     value = 0;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SENS:AUX",
                                &value, sizeof(value)));

    if ( auxiliaryValue )
        *auxiliaryValue = value;

    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Read Measurement                                                */
/* Purpose:  This function initiates an acquisition on the channels that you */
/*           specifies in channel parameter.  It then waits for the          */
/*           acquisition to complete, and returns the measurement for the    */
/*           channel you specify.                                            */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_meass_readMeasurement"
ViStatus _VI_FUNC rsnrpz_meass_readMeasurement(
    ViSession       vi,
    ViInt32         channel,
    ViInt32         timeout,
    ViReal64*       pMeasurement
)
{
    ViStatus      rsnrpz_status = VI_SUCCESS;
    ViUInt32      handle_count  = 0;
    SSessionInfo *psession_info = 0;
    DWORD         dw_error;
    SMainSession *pmain_session = 0;

    CHECKCHANNEL(vi, channel);

    // get associated session and check
    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    trace_log("CreateEvents in "__FUNC__);

    CHECKERR_CU( rsnrpz_CreateEvents( vi,
                                   channel,
                                   -1,
                                   -1,
                                   pmain_session->hHandlesRd,
                                   &handle_count));

    CHECKERR_CU( rsnrpz_chan_initiate (vi, channel));

    dw_error = WaitForSingleObject( pmain_session->hHandlesRd[0],
                                    timeout);

    trace_log("DestroyEvents in "__FUNC__);
    rsnrpz_status = rsnrpz_DestroyEvents( vi,
                                          pmain_session->hHandlesRd,
                                          handle_count);
    handle_count = 0;

    if ( dw_error == WAIT_TIMEOUT )
    {
        rsnrpz_status = RSNRPZ_ERROR_MAX_TIME_EXCEEDED;
    }
    CHECKERR_CU( rsnrpz_status);


    { /* inner block */
        psession_info = pmain_session->pSInfos[channel-1];

        EnterCriticalSection(&(psession_info->cs));

        /*
         * we don't need check IsSingleMeasValid, because
         * value has been received, before event was set.
         */
        if ( pMeasurement )
            *pMeasurement = psession_info->SingleMeas;
    }
    rsnrpz_status = rsnrpz_checkStatus ( vi);

Error:
    if (handle_count)
    {
        trace_log("DestroyEvents in "__FUNC__);
        rsnrpz_DestroyEvents( vi,
                              pmain_session->hHandlesRd,
                              handle_count );
    }

    if ( psession_info )
    {
        LeaveCriticalSection(&(psession_info->cs));
    }

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Fetch Measurement                                               */
/* Purpose:  This function returns the measurement the sensor acquires for   */
/*           the channel you specify.  The measurement is from an acquisition*/
/*           that you previously initiated.                                  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_meass_fetchMeasurement"
ViStatus _VI_FUNC rsnrpz_meass_fetchMeasurement(
    ViSession       vi,
    ViInt32         channel,
    ViReal64*       pMeasurement
)
{
    ViStatus        rsnrpz_status  = VI_SUCCESS;
    SSessionInfo*    psession_info  = 0;
    SMainSession*    pmain_sessions = 0;

    CHECKCHANNEL(vi, channel);

    pmain_sessions = V2N( vi );
    if( pmain_sessions==0 )
        return VI_ERROR_PARAMETER1;

    psession_info = pmain_sessions->pSInfos[channel-1];

    trace_log( "fetchMeasurement()" );

    EnterCriticalSection(&(psession_info->cs));

    if ( psession_info->IsSingleMeasValid )
    {
        if (pMeasurement)
            *pMeasurement = psession_info->SingleMeas;
    }
    else
    {
        /*
         * Measurement is not available => return an error.
         */
         CHECKERR_CU( RSNRPZ_ERROR_MEAS_NOT_AVAILABLE);
    }
    rsnrpz_status = rsnrpz_checkStatus ( vi);

Error:
    if ( psession_info )
        LeaveCriticalSection(&(psession_info->cs));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Read Buffer Measurement                                         */
/* Purpose:  This function initiates an acquisition on the channels that you */
/*           specifies in channel parameter.  It then waits for the          */
/*           acquisition to complete, and returns the measurement for the    */
/*           channel you specify.                                            */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_meass_readBufferMeasurement"
ViStatus _VI_FUNC rsnrpz_meass_readBufferMeasurement(
    ViSession   vi,
    ViInt32     channel,
    ViInt32     timeout,
    ViInt32     arraySize,
    ViReal64    measurementArray[],
    ViInt32*    pCount
)
{
    ViStatus      rsnrpz_status = VI_SUCCESS;
    ViUInt32      handle_count  = 0;
    SSessionInfo *psession_info = 0;
    DWORD         dw_error;
    SMainSession *pmain_session = 0;

    CHECKCHANNEL(vi, channel);

    // get associated session and check
    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    if (pCount)
        *pCount = 0;

    trace_log("CreateEvents in "__FUNC__);

    CHECKERR_CU( rsnrpz_CreateEvents( vi,
                                   channel,
                                   -2,
                                   -2,
                                   pmain_session->hHandlesRd,
                                   &handle_count));

    CHECKERR_CU( rsnrpz_chan_initiate (vi, channel));

    dw_error = WaitForSingleObject( pmain_session->hHandlesRd[0],
                                    timeout);

    trace_log("DestroyEvents in "__FUNC__);
    rsnrpz_status = rsnrpz_DestroyEvents( vi,
                                          pmain_session->hHandlesRd,
                                          handle_count );
    handle_count = 0;

    if ( dw_error == WAIT_TIMEOUT )
    {
        rsnrpz_status = RSNRPZ_ERROR_MAX_TIME_EXCEEDED;
    }
    CHECKERR_CU( rsnrpz_status);


    { /* inner block */
        psession_info = pmain_session->pSInfos[channel-1];

        EnterCriticalSection(&(psession_info->cs));

        if ( arraySize > 0 && measurementArray)
        {
            if ( psession_info->ArrayMeas != NULL && psession_info->ArrayMeasSize > 0 )
            {
                int count = ( psession_info->ArrayMeasSize > arraySize ) ? arraySize : psession_info->ArrayMeasSize;
                memcpy( measurementArray,
                        psession_info->ArrayMeas,
                        sizeof(ViReal64) * count);
                if (pCount)
                    *pCount = count;
            }
        }
    }
    rsnrpz_status = rsnrpz_checkStatus ( vi);

Error:
    if (handle_count)
    {
        trace_log("DestroyEvents 2 in "__FUNC__);
        rsnrpz_DestroyEvents( vi,
                              pmain_session->hHandlesRd,
                              handle_count );
    }

    if ( psession_info )
    {
        LeaveCriticalSection(&(psession_info->cs));
    }

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Fetch Buffer Measurement                                        */
/* Purpose:  This function returns the measurement the sensor acquires for   */
/*           the channel you specify.  The measurement is from an acquisition*/
/*           that you previously initiated.                                  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_meass_fetchBufferMeasurement"
ViStatus _VI_FUNC rsnrpz_meass_fetchBufferMeasurement(
    ViSession   vi,
    ViInt32     channel,
    ViInt32     arraySize,
    ViReal64    measurementArray[],
    ViInt32*    pCount
)
{
    ViStatus        rsnrpz_status  = VI_SUCCESS;
    SSessionInfo*   psession_info  = 0;
    SMainSession*   pmain_sessions = 0;

    CHECKCHANNEL(vi, channel);

    pmain_sessions = V2N( vi );
    if( pmain_sessions==0 )
        return VI_ERROR_PARAMETER1;

    if (pCount)
        *pCount = 0;

    psession_info = pmain_sessions->pSInfos[channel-1];

    trace_log( __FUNC__ );

    EnterCriticalSection(&(psession_info->cs));

    if ( psession_info->IsArrayMeasValid == VI_FALSE )
    {
        /*
         * Measurement is not available => return an error.
         */
         CHECKERR_CU( RSNRPZ_ERROR_MEAS_NOT_AVAILABLE);
    }

    if ( (arraySize        == 0)
      && (measurementArray == 0)
      && (psession_info->ArrayMeas != 0)
      && (pCount                   != 0)
       )
      *pCount = psession_info->ArrayMeasSize;

    if ( arraySize > 0 && measurementArray)
    {
        if ( psession_info->ArrayMeas != NULL && psession_info->ArrayMeasSize > 0 )
        {
            int count = ( psession_info->ArrayMeasSize > arraySize ) ? arraySize : psession_info->ArrayMeasSize;
            memcpy( measurementArray,
                    psession_info->ArrayMeas,
                    sizeof(ViReal64) * count);
            if (pCount)
                *pCount = count;
        }
    }
    rsnrpz_status = rsnrpz_checkStatus ( vi);

Error:
    if ( psession_info )
        LeaveCriticalSection(&(psession_info->cs));

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Send Software Trigger                                           */
/* Purpose:  Triggers a BUS event. If the sensor is in the WAIT_FOR_TRG      */
/*           state and the source for the trigger source is set to BUS,      */
/*           the sensor enters the MEASURING state.                          */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_meass_sendSoftwareTrigger"
ViStatus _VI_FUNC rsnrpz_meass_sendSoftwareTrigger(
    ViSession   vi,
    ViInt32     channel
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi, channel, (char*)"*TRG", NULL));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Get Detector Temperature                                        */
/* Purpose:  This function initiates a temperature measurement of the sensor */
/*           and reads the temperature value from the instrument.            */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_service_getDetectorTemperature"
ViStatus _VI_FUNC rsnrpz_service_getDetectorTemperature(
    ViSession   vi,
    ViInt32     channel,
    ViReal64*   pTemperature)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    float       temperature   = 0.0;
    ViInt32     lOnce         = RSNRPZ_ONCE;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi, channel, (char*)"SERV:UNL 1234", NULL ) );
    CHECKERR( rsnrpz_WriteCommand( vi, channel, (char*)"SERV:CAL:TEMP", &lOnce ) );
    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SERV:CAL:TEMP:DATA",
                                &temperature, sizeof(temperature)));
    rsnrpz_WriteCommand( vi, channel, (char*)"SERV:UNL 0", NULL );

    if ( pTemperature )
        *pTemperature = temperature;

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Start Simulation                                                */
/* Purpose:  This function sets the number of simulation pairs count-value.  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_service_startSimulation"
ViStatus _VI_FUNC rsnrpz_service_startSimulation(
    ViSession       vi,
    ViInt32         channel,
    ViInt32         blockCount
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;

    CHECKCHANNEL(vi, channel);

    CHECKERR( rsnrpz_WriteCommand( vi, channel, (char*)"SERV:UNL 1234", NULL));
    CHECKERR( rsnrpz_WriteCommand( vi, channel, (char*)"SERV:RCO", &blockCount));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Simulation Values                                           */
/* Purpose:  This function sets the values which will be simulated. Right    */
/*           before calling this function must be called function            */
/*           rsnrpz_service_startSimulation!                                     */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_service_setSimulationValues"
ViStatus _VI_FUNC rsnrpz_service_setSimulationValues(
    ViSession   vi,
    ViInt32     channel,
    ViInt32     valueCount[],
    ViReal64    values[]
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViInt32     i;
    ViInt32        simulation_count = 0;
    float       fvalue;
    ViInt32     lvalue;

    CHECKCHANNEL(vi, channel);

    if (!valueCount)
        return VI_ERROR_PARAMETER3;

    if (!values)
        return VI_ERROR_PARAMETER4;

    CHECKERR( rsnrpz_WriteCommand( vi, channel, (char*)"SERV:UNL 1234", NULL));
    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"SERV:RCO",
                                &simulation_count, sizeof(simulation_count)));

    for (i = 0; i < simulation_count; i++)
    {
        fvalue = (float)values[i];
        lvalue = valueCount[i];

        /*
         * TODO: fix it
         */
        CHECKERR( rsnrpz_WriteCommand(vi,
                                      channel,
                                      (char*)"SERV:RES", &fvalue));
        CHECKERR( rsnrpz_WriteCommand(vi,
                                      channel,
                                      (char*)"SERV:RES", &lvalue));
    }

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Stop Simulation                                                 */
/* Purpose:  This function stops the simulation by setting the count-value   */
/*           pairs to zero.                                                  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_service_stopSimulation"
ViStatus _VI_FUNC rsnrpz_service_stopSimulation(
    ViSession   vi,
    ViInt32     channel
)
{
    return rsnrpz_service_startSimulation(vi, channel, 0);
}

/*===========================================================================*/
/* Function: Check Condition                                                 */
/* Purpose:  This function checks selected status register for bits defined  */
/*           in Bitmask and returns a logical OR of all defined bits.        */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_status_checkCondition"
ViStatus _VI_FUNC rsnrpz_status_checkCondition(
    ViSession   vi,
    ViInt32     statusClass,
    ViUInt32    mask,
    ViBoolean*  pState
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    TRegister   nrp_register;
    int         i = 0;
    ViUInt32*   pnrp_register;

    CHECKSESSION(vi);

    for (i = 0; i < gs_class_registers_count; i++)
    {
        if ( gs_class_registers[i] == statusClass )
            break;
    }

    CHECKERR( ( i == gs_class_registers_count ) ? VI_ERROR_PARAMETER3 : VI_SUCCESS );

    CHECKERR( rsnrpz_QueryRegister( vi,
                                    gs_driver_registers[i],
                                    CONDITION_REGISTER,
                                    &nrp_register ));

    pnrp_register = (ViUInt32*)nrp_register;

    if ( pState )
        *pState = (ViBoolean)(((*pnrp_register) & mask) != 0);

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Catch Event                                                     */
/* Purpose:  This function sets the PTransition and NTransition register of  */
/*           selected status register according to bitmask.                  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_status_catchEvent"
ViStatus _VI_FUNC rsnrpz_status_catchEvent(
    ViSession   vi,
    ViInt32     statusClass,
    ViUInt32    mask,
    ViInt32     direction
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    SMainSession*   pmain_session = NULL;
    TRegister       nrp_register;
    ViUInt32*       pnrp_register;
    int             i;

    CHECKSESSION(vi);

    for (i = 0; i < gs_class_registers_count; i++)
    {
        if ( gs_class_registers[i] == statusClass )
            break;
    }

    CHECKERR( ( i == gs_class_registers_count ) ? VI_ERROR_PARAMETER3 : VI_SUCCESS );

    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    trace_log( "%s : EnterCriticalSection()", __FUNC__ );
    EnterCriticalSection(&(pmain_session->cs));

    switch ( direction )
    {
        case RSNRPZ_DIRECTION_PTR:
            CHECKERR_CU( rsnrpz_QueryRegister( vi,
                                               gs_driver_registers[i],
                                               PTRANSITION_REGISTER,
                                               &nrp_register ));
            pnrp_register = (ViUInt32*)nrp_register;
            *pnrp_register |= mask;

            CHECKERR_CU( rsnrpz_UpdateRegisters( pmain_session,
                                                 gs_driver_registers[i],
                                                 PTRANSITION_REGISTER,
                                                 nrp_register));

            CHECKERR_CU( rsnrpz_QueryRegister( vi,
                                               gs_driver_registers[i],
                                               NTRANSITION_REGISTER,
                                               &nrp_register ));
            pnrp_register = (ViUInt32*)nrp_register;
            *pnrp_register &= ~mask;
            CHECKERR_CU( rsnrpz_UpdateRegisters( pmain_session,
                                                 gs_driver_registers[i],
                                                 NTRANSITION_REGISTER,
                                                 nrp_register));
            break;
        case RSNRPZ_DIRECTION_NTR:
            CHECKERR_CU( rsnrpz_QueryRegister( vi,
                                               gs_driver_registers[i],
                                               NTRANSITION_REGISTER,
                                               &nrp_register ));
            pnrp_register = (ViUInt32*)nrp_register;
            *pnrp_register |= mask;

            CHECKERR_CU( rsnrpz_UpdateRegisters( pmain_session,
                                                 gs_driver_registers[i],
                                                 NTRANSITION_REGISTER,
                                                 nrp_register));
            CHECKERR_CU( rsnrpz_QueryRegister( vi,
                                               gs_driver_registers[i],
                                               PTRANSITION_REGISTER,
                                               &nrp_register ));
            pnrp_register = (ViUInt32*)nrp_register;
            *pnrp_register &= ~mask;

            CHECKERR_CU( rsnrpz_UpdateRegisters( pmain_session,
                                                 gs_driver_registers[i],
                                                 PTRANSITION_REGISTER,
                                                 nrp_register));
            break;
        case RSNRPZ_DIRECTION_BOTH:
            CHECKERR_CU( rsnrpz_QueryRegister( vi,
                                    gs_driver_registers[i],
                                    PTRANSITION_REGISTER,
                                    &nrp_register ));
            pnrp_register = (ViUInt32*)nrp_register;
            *pnrp_register |= mask;

            CHECKERR_CU( rsnrpz_UpdateRegisters( pmain_session,
                                                 gs_driver_registers[i],
                                                 PTRANSITION_REGISTER,
                                                 nrp_register));

            CHECKERR_CU( rsnrpz_QueryRegister( vi,
                                               gs_driver_registers[i],
                                               NTRANSITION_REGISTER,
                                               &nrp_register ));
            pnrp_register = (ViUInt32*)nrp_register;
            *pnrp_register |= mask;

            CHECKERR_CU( rsnrpz_UpdateRegisters( pmain_session,
                                                 gs_driver_registers[i],
                                                 NTRANSITION_REGISTER,
                                                 nrp_register));
            break;
        case RSNRPZ_DIRECTION_NONE:
            CHECKERR_CU( rsnrpz_QueryRegister( vi,
                                    gs_driver_registers[i],
                                    PTRANSITION_REGISTER,
                                    &nrp_register ));
            pnrp_register = (ViUInt32*)nrp_register;
            *pnrp_register &= ~mask;
            CHECKERR_CU( rsnrpz_UpdateRegisters( pmain_session,
                                                 gs_driver_registers[i],
                                                 PTRANSITION_REGISTER,
                                                 nrp_register));

            CHECKERR_CU( rsnrpz_QueryRegister( vi,
                                    gs_driver_registers[i],
                                    NTRANSITION_REGISTER,
                                    &nrp_register ));
            pnrp_register = (ViUInt32*)nrp_register;
            *pnrp_register &= ~mask;
            CHECKERR_CU( rsnrpz_UpdateRegisters( pmain_session,
                                                 gs_driver_registers[i],
                                                 NTRANSITION_REGISTER,
                                                 nrp_register));
            break;
        default:
            CHECKERR_CU( VI_ERROR_PARAMETER4);
            break;
    }

Error:
    if ( pmain_session )
    {
        trace_log( "%s : LeaveCriticalSection()", __FUNC__ );
        LeaveCriticalSection(&(pmain_session->cs));
    }
    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Check Event                                                     */
/* Purpose:  This function checks the selected status register for events    */
/*           specified by bitmask and sets returns their states.             */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_status_checkEvent"
ViStatus _VI_FUNC rsnrpz_status_checkEvent(
    ViSession   vi,
    ViInt32     statusClass,
    ViUInt32    mask,
    ViUInt32    resetMask,
    ViBoolean*  pEvents
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    TRegister       nrp_register;
    int             i = 0;
    ViUInt32*       pnrp_register;
    SMainSession*   pmain_session = NULL;

    CHECKSESSION(vi);

    for (i = 0; i < gs_class_registers_count; i++)
    {
        if ( gs_class_registers[i] == statusClass )
            break;
    }

    CHECKERR( ( i == gs_class_registers_count ) ? VI_ERROR_PARAMETER3 : VI_SUCCESS );


    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    trace_log( "%s : EnterCriticalSection()", __FUNC__ );
    EnterCriticalSection(&(pmain_session->cs));

    CHECKERR_CU( rsnrpz_QueryRegister( vi,
                                       gs_driver_registers[i],
                                       EVENT_REGISTER,
                                       &nrp_register ));

    pnrp_register = (ViUInt32*)nrp_register;

    if ( pEvents )
        *pEvents = (ViBoolean)(((*pnrp_register) & mask) != 0);

    *pnrp_register &= ~resetMask;

#if 0
    memcpy(pmain_session->NrpRegisters[gs_driver_registers[i]][EVENT_REGISTER], pnrp_register, sizeof(ViUInt32));
#else
    CHECKERR( rsnrpz_UpdateRegisters( pmain_session,
                                      gs_driver_registers[i],
                                      EVENT_REGISTER,
                                      nrp_register
                                    ));
#endif

Error:
    if ( pmain_session )
    {
        trace_log( "%s : LeaveCriticalSection()", __FUNC__ );
        LeaveCriticalSection(&(pmain_session->cs));
    }
    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Enable Event Notification                                       */
/* Purpose:  This function enables events defined by Bitmask in enable       */
/*           register respective to the selected status register.            */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_status_enableEventNotification"
ViStatus _VI_FUNC rsnrpz_status_enableEventNotification(
    ViSession   vi,
    ViInt32     statusClass,
    ViUInt32    mask
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    TRegister       nrp_register;
    int             i = 0;
    ViUInt32*       pnrp_register;
    SMainSession*   pmain_session = NULL;

    CHECKSESSION(vi);

    for (i = 0; i < gs_class_registers_count; i++)
    {
        if ( gs_class_registers[i] == statusClass )
            break;
    }

    CHECKERR( ( i == gs_class_registers_count ) ? VI_ERROR_PARAMETER3 : VI_SUCCESS );

    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    trace_log( "%s : EnterCriticalSection()", __FUNC__ );
    EnterCriticalSection(&(pmain_session->cs));

    CHECKERR_CU( rsnrpz_QueryRegister( vi,
                                       gs_driver_registers[i],
                                       ENABLE_REGISTER,
                                       &nrp_register ));
    pnrp_register = (ViUInt32*)nrp_register;
    *pnrp_register |= mask;

    CHECKERR_CU( rsnrpz_UpdateRegisters( pmain_session,
                                         gs_driver_registers[i],
                                         ENABLE_REGISTER,
                                         nrp_register));

Error:
    if ( pmain_session )
    {
        trace_log( "%s : LeaveCriticalSection()", __FUNC__ );
        LeaveCriticalSection(&(pmain_session->cs));
    }
    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Disable Event Notification                                      */
/* Purpose:  This function disables events defined by Bitmask in enable      */
/*           register respective to the selected status register.            */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_status_disableEventNotification"
ViStatus _VI_FUNC rsnrpz_status_disableEventNotification(
    ViSession   vi,
    ViInt32     statusClass,
    ViUInt32    mask
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    TRegister       nrp_register;
    int             i = 0;
    ViUInt32*       pnrp_register;
    SMainSession*   pmain_session = NULL;

    CHECKSESSION(vi);

    for (i = 0; i < gs_class_registers_count; i++)
    {
        if ( gs_class_registers[i] == statusClass )
            break;
    }

    CHECKERR( ( i == gs_class_registers_count ) ? VI_ERROR_PARAMETER3 : VI_SUCCESS );

    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    trace_log( "%s : EnterCriticalSection()", __FUNC__ );
    EnterCriticalSection(&(pmain_session->cs));

    CHECKERR_CU( rsnrpz_QueryRegister( vi,
                                       gs_driver_registers[i],
                                       ENABLE_REGISTER,
                                       &nrp_register ));
    pnrp_register = (ViUInt32*)nrp_register;
    *pnrp_register &= ~mask;

    CHECKERR_CU( rsnrpz_UpdateRegisters( pmain_session,
                                         gs_driver_registers[i],
                                         ENABLE_REGISTER,
                                         nrp_register));

Error:
    if ( pmain_session )
    {
        trace_log( "%s : LeaveCriticalSection()", __FUNC__ );
        LeaveCriticalSection(&(pmain_session->cs));
    }
    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Register Window Message                                         */
/* Purpose:  This function registers message, which will be send to specified*/
/*           window, when SRQ is occured.                                    */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_status_registerWindowMessage"
ViStatus _VI_FUNC rsnrpz_status_registerWindowMessage(
    ViSession   vi,
//  ViUInt32    handle,
    ViUInt32   *handle,
    ViUInt32    messageId
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    SMainSession*   pmain_session = NULL;

    CHECKSESSION(vi);

    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    trace_log( "%s : EnterCriticalSection()", __FUNC__ );
    EnterCriticalSection(&(pmain_session->cs));

#if ! defined(__linux__) && ! defined(__APPLE__)
    pmain_session->hMessageWindow = (HWND)handle;
#else
    handle = handle;   // otherwise 'unused' warning
#endif
    pmain_session->MessageID = (UINT)messageId;

    trace_log( "%s : LeaveCriticalSection()", __FUNC__ );
    LeaveCriticalSection(&(pmain_session->cs));

    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Read Measurement Aux                                            */
/* Purpose:  This function initiates an acquisition on the channels that you */
/*           specifies in channel parameter. It then waits for the           */
/*           acquisition to complete, and returns the auxiliary measurement  */
/*           for the channel you specify.                                    */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_meass_readMeasurementAux"
ViStatus _VI_FUNC rsnrpz_meass_readMeasurementAux (ViSession vi,
                                                   ViInt32 channel,
                                                   ViInt32 timeout,
                                                   ViReal64 *measurement,
                                                   ViReal64 *aux1,
                                                   ViReal64 *aux2)
{
    ViStatus      rsnrpz_status = VI_SUCCESS;
    ViUInt32      handle_count  = 0;
    SSessionInfo *psession_info = 0;
    SMainSession *pmain_session = 0;
    DWORD         dw_error;

    CHECKCHANNEL(vi, channel);

    // get associated session and check
    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    trace_log("CreateEvents in "__FUNC__);

    CHECKERR_CU( rsnrpz_CreateEvents( vi,
                                   channel,
                                   -1,
                                   -1,
                                   pmain_session->hHandlesRd,
                                   &handle_count));

    CHECKERR_CU( rsnrpz_chan_initiate (vi, channel));

    dw_error = WaitForSingleObject( pmain_session->hHandlesRd[0],
                                    timeout );

    trace_log("DestroyEvents in "__FUNC__);
    rsnrpz_status = rsnrpz_DestroyEvents( vi,
                                          pmain_session->hHandlesRd,
                                          handle_count );
    handle_count = 0;

    if ( dw_error == WAIT_TIMEOUT )
    {
        rsnrpz_status = RSNRPZ_ERROR_MAX_TIME_EXCEEDED;
    }
    CHECKERR_CU( rsnrpz_status);


    { /* inner block */
        psession_info = pmain_session->pSInfos[channel-1];

        EnterCriticalSection(&(psession_info->cs));

        /*
         * we don't need check IsSingleMeasValid, because
         * value has been received, before event was set.
         */
        if (measurement)
            *measurement = psession_info->SingleMeas;
        if (aux1)
            *aux1 = psession_info->SingleMeasAux1;
        if (aux2)
            *aux2 = psession_info->SingleMeasAux2;
    }
    rsnrpz_status = rsnrpz_checkStatus( vi );

Error:
    if (handle_count)
    {
        trace_log("DestroyEvents in "__FUNC__);
        rsnrpz_DestroyEvents( vi,
                             pmain_session->hHandlesRd,
                              handle_count );
    }

    if ( psession_info )
    {
        LeaveCriticalSection(&(psession_info->cs));
    }

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Fetch Measurement Aux                                           */
/* Purpose:  This function returns the measurement the sensor acquires for   */
/*           the channel you specify.  The measurement is from an acquisition*/
/*           that you previously initiated.                                  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_meass_fetchMeasurementAux"
ViStatus _VI_FUNC rsnrpz_meass_fetchMeasurementAux (ViSession vi,
                                                    ViInt32 channel,
                                                    ViInt32 timeout_ms,
                                                    ViReal64 *measurement,
                                                    ViReal64 *aux1,
                                                    ViReal64 *aux2)
{
    ViStatus        rsnrpz_status  = VI_SUCCESS;
    SSessionInfo*   psession_info  = 0;
    SMainSession*   pmain_session  = 0;

    CHECKCHANNEL(vi, channel);

    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    psession_info = pmain_session->pSInfos[channel-1];

    timeout_ms = timeout_ms;    // suppress 'unused' warning

    trace_log( __FUNC__ );

    EnterCriticalSection(&(psession_info->cs));

    if ( psession_info->IsSingleMeasValid )
    {
        if (measurement)
            *measurement = psession_info->SingleMeas;
        if (aux1)
            *aux1 = psession_info->SingleMeasAux1;
        if (aux2)
            *aux2 = psession_info->SingleMeasAux2;
    }
    else
    {
        /*
         * Measurement is not available => return an error.
         */
         CHECKERR_CU( RSNRPZ_ERROR_MEAS_NOT_AVAILABLE);
    }
    rsnrpz_status = rsnrpz_checkStatus ( vi);

Error:
    if ( psession_info )
        LeaveCriticalSection(&(psession_info->cs));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Read Buffer Data Aux                                            */
/* Purpose:  This function initiates an acquisition on the channels that you */
/*           specifies in channel parameter.  It then waits for the          */
/*           acquisition to complete, and returns the measurement for the    */
/*           channel you specify.                                            */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_meass_readBufferMeasurementAux"
ViStatus _VI_FUNC rsnrpz_meass_readBufferMeasurementAux (ViSession vi,
                                                         ViInt32 channel,
                                                         ViInt32 timeout,
                                                         ViInt32 arraySize,
                                                         ViReal64 measurementArray[],
                                                         ViReal64 aux1Array[],
                                                         ViReal64 aux2Array[],
                                                         ViInt32 *pCount)
{
    ViStatus      rsnrpz_status = VI_SUCCESS;
    ViUInt32      handle_count  = 0;
    SSessionInfo *psession_info = 0;
    SMainSession *pmain_session = 0;
    DWORD         dw_error;

    CHECKCHANNEL(vi, channel);

    // get associated session and check
    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 4, 2 ) )  //  4/2  =  SENS:POW:AVG:BUFF:SIZE
        return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif

    if (pCount)
        *pCount = 0;

    trace_log("CreateEvents in "__FUNC__);
    CHECKERR_CU( rsnrpz_CreateEvents( vi,
                                   channel,
                                   -2,
                                   -2,
                                   pmain_session->hHandlesRd,
                                   &handle_count));

    CHECKERR_CU( rsnrpz_chan_initiate (vi, channel));

    dw_error = WaitForSingleObject( pmain_session->hHandlesRd[0],
                                    timeout );

    trace_log("DestroyEvents in "__FUNC__);
    rsnrpz_status = rsnrpz_DestroyEvents( vi,
                                          pmain_session->hHandlesRd,
                                          handle_count );
    handle_count = 0;

    if ( dw_error == WAIT_TIMEOUT )
    {
        rsnrpz_status = RSNRPZ_ERROR_MAX_TIME_EXCEEDED;
    }
    CHECKERR_CU( rsnrpz_status);


    { /* inner block */

        psession_info = pmain_session->pSInfos[channel-1];

        EnterCriticalSection(&(psession_info->cs));

        if ( arraySize > 0)
        {
            int count = ( psession_info->ArrayMeasSize > arraySize ) ? arraySize : psession_info->ArrayMeasSize;
            if (measurementArray)
            {
                if ( psession_info->ArrayMeas != NULL && psession_info->ArrayMeasSize > 0 )
                {
                    memcpy( measurementArray,
                            psession_info->ArrayMeas,
                            sizeof(ViReal64) * count);
                    if (pCount)
                        *pCount = count;
                }
            }

            if (aux1Array)
            {
                if ( psession_info->ArrayMeasAux1 != NULL && psession_info->ArrayMeasSize > 0 )
                {
                    memcpy( aux1Array,
                            psession_info->ArrayMeasAux1,
                            sizeof(ViReal64) * count);
                    if (pCount)
                        *pCount = count;
                }
            }

            if (aux2Array)
            {
                if ( psession_info->ArrayMeasAux2 != NULL && psession_info->ArrayMeasSize > 0 )
                {
                    memcpy( aux2Array,
                            psession_info->ArrayMeasAux2,
                            sizeof(ViReal64) * count);
                    if (pCount)
                        *pCount = count;
                }
            }
        }
    }
    rsnrpz_status = rsnrpz_checkStatus ( vi);

Error:
    if (handle_count)
    {
        trace_log("DestroyEvents 2 in "__FUNC__);
        rsnrpz_DestroyEvents( vi,
                              pmain_session->hHandlesRd,
                              handle_count );
    }

    if ( psession_info )
    {
        LeaveCriticalSection(&(psession_info->cs));
    }

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Fetch Buffer Data Aux                                           */
/* Purpose:  This function returns the buffer measurement the sensor acquires*/
/*           for the channel you specify.  The measurement is from an        */
/*           acquisition that you previously initiated.                      */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_meass_fetchBufferMeasurementAux"
ViStatus _VI_FUNC rsnrpz_meass_fetchBufferMeasurementAux (ViSession vi,
                                                          ViInt32 channel,
                                                          ViInt32 timeout,
                                                          ViInt32 arraySize,
                                                          ViReal64 measurementArray[],
                                                          ViReal64 aux1Array[],
                                                          ViReal64 aux2Array[],
                                                          ViInt32 *pCount)
{
    ViStatus        rsnrpz_status  = VI_SUCCESS;
    SSessionInfo*   psession_info  = 0;
    SMainSession*   pmain_session  = 0;

    CHECKCHANNEL(vi, channel);

    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

#if defined (USE_CHECKINSTR)
    CHECKERR (rsnrpz_CheckInstr (vi, channel, (char*)"NRP-Z5"));
#else
    if ( ! rsnrpz_IsSupported( vi, channel, 4, 2 ) )  //  4/2  =  SENS:POW:AVG:BUFF:SIZE
      return RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
#endif


    timeout = timeout;      // suppress 'unused' warning

    if (pCount)
        *pCount = 0;

    psession_info = pmain_session->pSInfos[channel-1];

    trace_log( __FUNC__ );

    EnterCriticalSection(&(psession_info->cs));

    if ( psession_info->IsArrayMeasAuxValid == VI_FALSE )
    {
        /*
         * Measurement is not available => return an error.
         */
         CHECKERR_CU( RSNRPZ_ERROR_MEAS_NOT_AVAILABLE);
    }

    if ( (arraySize        == 0)
      && (measurementArray == 0)
      && (aux1Array        == 0)
      && (aux2Array        == 0)
      && (psession_info->ArrayMeas != 0)
      && (pCount                   != 0)
       )
      *pCount = psession_info->ArrayMeasSize;


    if ( arraySize > 0)
    {
      int count = ( psession_info->ArrayMeasSize > arraySize ) ? arraySize : psession_info->ArrayMeasSize;
      if (measurementArray)
      {
        if ( psession_info->ArrayMeas != NULL && psession_info->ArrayMeasSize > 0 )
        {
          memcpy( measurementArray,
                  psession_info->ArrayMeas,
                  sizeof(ViReal64) * count);
          if ( pCount )
            *pCount = count;
        }
      }

      if (aux1Array)
      {
        if ( psession_info->ArrayMeasAux1 != NULL && psession_info->ArrayMeasSize > 0 )
        {
          memcpy( aux1Array,
                  psession_info->ArrayMeasAux1,
                  sizeof(ViReal64) * count);
          if ( pCount )
            *pCount = count;
        }
      }

      if (aux2Array)
      {
        if ( psession_info->ArrayMeasAux2 != NULL && psession_info->ArrayMeasSize > 0 )
        {
          memcpy( aux2Array,
                  psession_info->ArrayMeasAux2,
                  sizeof(ViReal64) * count);
          if (pCount)
            *pCount = count;
        }
      }
    }

    rsnrpz_status = rsnrpz_checkStatus ( vi);

Error:
    if ( psession_info )
        LeaveCriticalSection(&(psession_info->cs));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Preset                                                          */
/* Purpose:  This function resets the R&S NRP to default values.             */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_status_preset"
ViStatus _VI_FUNC rsnrpz_status_preset(
    ViSession vi
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    SMainSession*   pmain_session = NULL;

    CHECKSESSION(vi);

    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    trace_log( "%s : EnterCriticalSection()", __FUNC__ );
    EnterCriticalSection(&(pmain_session->cs));

    CHECKERR_CU( rsnrpz_InitializeRegisters(pmain_session->NrpRegisters));

Error:
    if ( pmain_session )
    {
        trace_log( "%s : LeaveCriticalSection()", __FUNC__ );
        LeaveCriticalSection(&(pmain_session->cs));
    }
    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: State Checking                                                  */
/* Purpose:  This function switches ON/OFF state checking of the instrument  */
/*           (reading of the Standard Event Register and checking it for     */
/*           error). Nearly all driver function are using state checking.    */
/*           Switch this function to ON when debug your application. For     */
/*           better bus throughput and instruments performance switch it OFF.*/
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_errorCheckState"
ViStatus _VI_FUNC rsnrpz_errorCheckState (
    ViSession   vi,
    ViBoolean   state
)
{
    ViStatus      rsnrpz_status = VI_SUCCESS;
    SMainSession *pmain_session = 0;

    CHECKSESSION(vi);

    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    pmain_session->ErrorChecking = state;

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Reset                                                           */
/* Purpose:  This function resets the instrument.  If the reset function     */
/*           is not supported by the instrument, this function returns       */
/*           the warning VI_WARN_NSUP_RESET.                                 */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_reset"
ViStatus _VI_FUNC rsnrpz_reset(
    ViSession vi
)
{
    ViStatus rsnrpz_status = VI_SUCCESS;

    /*  Initialize the instrument to a known state.  */

    CHECKSESSION(vi);

      CHECKERR( rsnrpz_WriteCommand( vi, ALL_CHANNELS, (char*)"*RST", NULL));
    CHECKERR( rsnrpz_defaultInstrSetup (vi, ALL_CHANNELS));
    CHECKERR( rsnrpz_checkStatus ( vi));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Self-Test                                                       */
/* Purpose:  This function executes the instrument self-test and returns     */
/*           the result. If the self test function is not supported by the   */
/*           instrument, this function returns the warning                   */
/*           VI_WARN_NSUP_SELF_TEST.                                         */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_self_test"
ViStatus _VI_FUNC rsnrpz_self_test(
    ViSession       vi,
    ViInt16*        pTestResult,
    ViChar _VI_FAR testMessage[]
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    ViInt32         index, count;
    int             i;
    ViInt32         values[RSNRPZ_MAX_CHANNELS];
    SMainSession*   pmain_session = 0;

    CHECKSESSION(vi);

    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    if ( testMessage == 0 )
    {
        // suppress 'unused' warning
    }

    memset( values, 0, sizeof(values) );

    CHECKERR_CU( rsnrpz_ReadValue( vi,
                                   ALL_CHANNELS,
                                  (char*)"*TST",
                                   &values[0],
                                   sizeof(values) ) );

      CHECKERR_CU( rsnrpz_GetIndexFromCommand( (char*)"*TST", &index));
    count = 0;
    for ( i = 0; i < RSNRPZ_MAX_CHANNELS; i++)
    {
        if ( pmain_session->pSessions[i] )
        {
            SSessionInfo* psession_info;
            psession_info = pmain_session->pSInfos[i];

            EnterCriticalSection(&(psession_info->cs));
            rsnrpz_status = rsnrpz_GetValueFromCache( psession_info->CacheTable,
                                                      index,
                                                      &values[count++],
                                                      sizeof(values[0]));
            LeaveCriticalSection(&(psession_info->cs));
        }
    }

    CHECKERR_CU( rsnrpz_status);

    if ( pTestResult )
    {
        *pTestResult = 0;
        for ( i= 0; i < count; i++)
        {
            if ( values[i] )
            {
                 *pTestResult = (ViInt16)values[i];
                 break;
            }

        }
    }

    CHECKERR_CU( rsnrpz_checkStatus(vi));

Error:
    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Error Query                                                     */
/* Purpose:  This function queries the instrument error queue, and returns   */
/*           the result. If the error query function is not supported by the */
/*           instrument, this function returns the warning                   */
/*           VI_WARN_NSUP_ERROR_QUERY.                                       */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_error_query"
ViStatus _VI_FUNC rsnrpz_error_query(
    ViSession       vi,
    ViInt32*        pErrorCode,
    ViChar _VI_FAR errorMessage[]
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    SMainSession*   pmain_session = 0;

    CHECKSESSION(vi);

    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    trace_log( "%s : EnterCriticalSection()", __FUNC__ );
    EnterCriticalSection(&(pmain_session->cs));

    if ( pmain_session->NrpErrors[0].ErrorCode )
    {
        if ( pErrorCode )
            *pErrorCode = pmain_session->NrpErrors[0].ErrorCode;
        if ( errorMessage )
            strncpy(errorMessage, pmain_session->NrpErrors[0].ErrorMessage, 255);

        // RemoveFirstError() can only be called from WITHIN a locked
        //                    critical section 'pmain_session->cs' !!!
        CHECKERR_CU( RemoveFirstError(vi));
    }
    else
    {
        if ( pErrorCode )
            *pErrorCode = 0;
        if ( errorMessage )
            strcpy(errorMessage, "No Error");
    }

Error:
    trace_log( "%s : LeaveCriticalSection()", __FUNC__ );
    LeaveCriticalSection(&(pmain_session->cs));

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Error Message                                                   */
/* Purpose:  This function translates the error return value from the        */
/*           instrument driver into a user-readable string.                  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_error_message"
ViStatus _VI_FUNC rsnrpz_error_message(
    ViSession       vi,
    ViStatus        statusCode,
    ViChar _VI_FAR  errMessage[]
)
{
    ViStatus rsnrpz_status = VI_SUCCESS;
    ViInt16 i;

    static rsnrpz_tStringValPair statusDescArray[] = {
        {VI_WARN_NSUP_ID_QUERY,                  "WARNING: ID Query not supported"},
        {VI_WARN_NSUP_RESET,                     "WARNING: Reset not supported"},
        {VI_WARN_NSUP_SELF_TEST,                 "WARNING: Self-test not supported"},
        {VI_WARN_NSUP_ERROR_QUERY,               "WARNING: Error Query not supported"},
        {VI_WARN_NSUP_REV_QUERY,                 "WARNING: Revision Query not supported"},
        {VI_ERROR_PARAMETER1,                    "ERROR: Parameter 1 out of range"},
        {VI_ERROR_PARAMETER2,                    "ERROR: Parameter 2 out of range"},
        {VI_ERROR_PARAMETER3,                    "ERROR: Parameter 3 out of range"},
        {VI_ERROR_PARAMETER4,                    "ERROR: Parameter 4 out of range"},
        {VI_ERROR_PARAMETER5,                    "ERROR: Parameter 5 out of range"},
        {VI_ERROR_PARAMETER6,                    "ERROR: Parameter 6 out of range"},
        {VI_ERROR_PARAMETER7,                    "ERROR: Parameter 7 out of range"},
        {VI_ERROR_PARAMETER8,                    "ERROR: Parameter 8 out of range"},
        {VI_ERROR_FAIL_ID_QUERY,                 "ERROR: Identification query failed"},
        {VI_ERROR_INV_RESPONSE,                  "ERROR: Interpreting instrument response"},
        {VI_ERROR_INSTR_INTERPRETING_RESPONSE,   "ERROR: Interpreting the instrument's response"},
        {VI_ERROR_INSTR_PARAMETER9 ,             "ERROR: Parameter 9 out of range"},
        {VI_ERROR_INSTR_PARAMETER10,             "ERROR: Parameter 10 out of range"},
        {VI_ERROR_INSTR_PARAMETER11,             "ERROR: Parameter 11 out of range"},
        {VI_ERROR_INSTR_PARAMETER12,             "ERROR: Parameter 12 out of range"},
        {VI_ERROR_INSTR_PARAMETER13,             "ERROR: Parameter 13 out of range"},
        {VI_ERROR_INSTR_PARAMETER14,             "ERROR: Parameter 14 out of range"},
        {VI_ERROR_INSTR_PARAMETER15,             "ERROR: Parameter 15 out of range"},
        {RSNRPZ_ERROR_INSTRUMENT_ERROR,          "ERROR: Instrument status error"},
        {RSNRPZ_ERROR_INVALID_CONFIGURATION,     "ERROR: Instrument configuration error"},
        {RSNRPZ_ERROR_INSTRUMENT_OPTION,         "ERROR: Option not installed"},
        {RSNRPZ_ERROR_INSTRUMENT_NSUP_MODEL,     "ERROR: Required Instrument model is not available."},
        {RSNRPZ_ERROR_SETTINGS_CONFLICT,         "ERROR: Instrument settings conflict."},
        {RSNRPZ_ERROR_INTERPRETING_RESPONSE,     "ERROR: Unrecognized response from instrument."},
        {RSNRPZ_ERROR_TABLE_NOT_FOUND,           "ERROR: Specified offset table is not available."},
        {RSNRPZ_ERROR_COMMAND_NOT_FOUND,         "ERROR: Command is not supported by the driver."},
        {RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED,     "ERROR: Command is not supported by the sensor."},
        {RSNRPZ_ERROR_INVALID_VALUE,             "ERROR: Invalid value. (Value out of range.)"},
        {RSNRPZ_ERROR_INCORRECT_CHANNEL,         "ERROR: Invalid channel."},
        {RSNRPZ_ERROR_TIMEOUT,                   "ERROR: Time-out expired"},
        {RSNRPZ_ERROR_MAX_REGISTER_EVENTS,       "ERROR: Maximum registered eventes exceed."},
        {RSNRPZ_ERROR_REGISTER_EVENT,            "ERROR: Register event failed."},
        {RSNRPZ_ERROR_SENSOR_ASSIGNED_TO_CHANNEL,"ERROR: Channel has already assigned sensor."},
        {RSNRPZ_ERROR_SMALL_BUFFER,              "ERROR: Internal error, small buffer."},
        {RSNRPZ_ERROR_MEAS_NOT_AVAILABLE,        "ERROR: Measurement is not available."},
        {RSNRPZ_ERROR_MAX_TIME_EXCEEDED,         "ERROR: Maximum time exceeded before the operation completed."},
        {VI_NULL, VI_NULL}
    };

    if ( vi == 0 )
    {
        // suppress 'unused' warning
    }


    if (statusCode != VI_SUCCESS)
    {
        for (i = 0; statusDescArray[i].stringName; i++)
            {
            if (statusDescArray[i].stringVal == statusCode)
                {
                sprintf (errMessage, "%s", statusDescArray[i].stringName);
                return (VI_SUCCESS);
                }
            }

        if ( errMessage )
        {
            NrpGetErrorText( statusCode, errMessage, 256);
            if ( strstr(errMessage, "Unknown Error Code (0x") )
                return (VI_WARN_UNKNOWN_STATUS);
        }
    }
    else
    {
        if ( errMessage )
            *errMessage = '\0';

    }

    rsnrpz_status = VI_SUCCESS;
    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Revision Query                                                  */
/* Purpose:  This function returns the driver and instrument revisions.      */
/*           If the revision query function is not supported by the          */
/*           instrument, this function returns the warning                   */
/*           VI_WARN_NSUP_REV_QUERY.                                         */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_revision_query"
ViStatus _VI_FUNC rsnrpz_revision_query(
    ViSession       vi,
    ViChar _VI_FAR driverRev[],
    ViChar _VI_FAR instrRev[]
)
{
//  CHECKSESSION(vi);
    vi = vi;

    if ( instrRev )
    {
        instrRev[0] = '\0';
    }

    if ( driverRev )
        sprintf (driverRev, "%s", RSNRPZ_REVISION);

    return VI_WARN_NSUP_REV_QUERY;
}

/*===========================================================================*/
/* Function: Close Sensor                                                    */
/* Purpose:  This function closes the instrument.                            */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_CloseSensor"
ViStatus _VI_FUNC rsnrpz_CloseSensor(
    ViSession   vi,
    ViInt32     channel
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    ViStatus        reset_status  = VI_SUCCESS;
    SMainSession*   pmain_session = 0;
    int             j;

    CHECKCHANNEL(vi, channel);

    pmain_session = V2N( vi );
    if( pmain_session == 0 )
        return VI_SUCCESS;

    reset_status = rsnrpz_WriteCommand( vi, channel, (char*)"*RST", NULL);

    trace_log( "%s : EnterCriticalSection()", __FUNC__ );
    EnterCriticalSection(&(pmain_session->cs));

    { /* inner block */
        ViInt32         command_count = rsnrpz_GetNbOfCommands();

        SSessionInfo* psession_info;
        psession_info = pmain_session->pSInfos[channel-1];

        // close sensor and set session to zero
        NrpCloseSensor( pmain_session->pSessions[channel-1] );
        pmain_session->pSessions[channel-1] = 0;

        if ( psession_info != 0 )
        {
            DeleteCriticalSection(&(psession_info->cs));
            if ( psession_info->CacheTable != NULL )
            {
                for ( j = 0; j < command_count; j++)
                {
                    if (( psession_info->CacheTable[j].type == DATA_STRING ) &&
                        ( psession_info->CacheTable[j].current.pstring != NULL ))
                    {
                        free(psession_info->CacheTable[j].current.pstring);
                        psession_info->CacheTable[j].current.pstring = NULL;
                    }
                }

                free( psession_info->CacheTable);
                psession_info->CacheTable = NULL;

                if ( psession_info->ArrayMeas )
                {
                    free(psession_info->ArrayMeas);
                    psession_info->ArrayMeas = NULL;
                    psession_info->ArrayMeasSize = 0;
                }

                if ( psession_info->ArrayMeasAux1 )
                {
                    free(psession_info->ArrayMeasAux1);
                    psession_info->ArrayMeasAux1 = NULL;
                }

                if ( psession_info->ArrayMeasAux2 )
                {
                    free(psession_info->ArrayMeasAux2);
                    psession_info->ArrayMeasAux2 = NULL;
                }

                free(psession_info);
                psession_info = NULL;
            }
        }
    } /* end of inner block */


    CHECKERR_CU( rsnrpz_SetBitInRegisters( pmain_session,
                                           DEVICE_STATUS_REGISTER_CONNECTED,
                                           CONDITION_REGISTER,
                                           channel,
                                           VI_FALSE));

Error:
    if ( pmain_session != NULL )
    {
        trace_log( "%s : LeaveCriticalSection()", __FUNC__ );
        LeaveCriticalSection(&(pmain_session->cs));
    }
    if (reset_status == (ViStatus)NRP_ERROR_DEVICE_DISCONNECTED)
        rsnrpz_status = NRP_ERROR_DEVICE_DISCONNECTED;
    rsnrpz_status = (rsnrpz_status == VI_SUCCESS) ? rsnrpz_status : reset_status;
    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Close                                                           */
/* Purpose:  This function closes the instrument.                            */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_close"
ViStatus _VI_FUNC rsnrpz_close (ViSession vi)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    ViStatus        reset_status  = VI_SUCCESS;
    SMainSession*   pmain_session = NULL;
    SSessionInfo*   psession_infos[RSNRPZ_MAX_CHANNELS];
    ViInt32         i,j,count;
    ViInt32         command_count = rsnrpz_GetNbOfCommands();
    SSessionInfo*   psession_info  = 0;

    pmain_session = V2N( vi );  // Might be 0

  if ( (pmain_session == 0) && (vi != 0) )
  {
    // This can happen if somebody tries
    // to close a session twice...
    return VI_ERROR_PARAMETER1;
  }


    if ( pmain_session == 0 )
    {
        DiscardAllSessions();  // This is a BRUTE FORCE close (without a session)!!!
                               // We don't synchronize with a critical section here
                               // because the corresponding critical section may
                               // not even have been initialized

        if ( pAvailableSensor )
            free( pAvailableSensor );
        pAvailableSensor = 0;

        if ( boDriverIsOpen == VI_TRUE )
        {
                // close driver if no device change call back is registered
                if( pCallbackFn == 0 )
                {
                    NrpCloseDriver();
                    boDriverIsOpen = VI_FALSE;
                }
        }

        return VI_SUCCESS;
    }

    memset( psession_infos, 0, sizeof( psession_infos ) );

    reset_status = rsnrpz_WriteCommand( vi, ALL_CHANNELS, (char*)"*RST", NULL );

    /*=====================================================================*
     * Don't accept any callback
     *=====================================================================*/
    if ( NumSessionNodes() < 2 )
    {
        // This is the LAST session [which will be discarded later
        // by a call to RemoveSessionNode() below]! Therefore we
        // don't need the callbacks any longer...!
        NrpSetNotifyCallbackDataAvailable(   NULL, 0 );
        NrpSetNotifyCallbackErrorOccurred(   NULL, 0 );
        NrpSetNotifyCallbackStateChanged(    NULL, 0 );
        NrpSetNotifyCallbackCommandAccepted( NULL, 0 );
    }

    if ( pmain_session->event )
    {
        ResetEvent(pmain_session->event);
#if ! defined(__linux__) && ! defined(__APPLE__)
        Sleep(100);
#else
        usleep(100*1000);
#endif
    }

    /*=====================================================================*
     * Close all sensors that belong to this main session (channels)
     *=====================================================================*/
    trace_log( "%s : EnterCriticalSection()", __FUNC__ );
    EnterCriticalSection( &(pmain_session->cs) );

    count = 0;
    for ( i = 0; i < RSNRPZ_MAX_CHANNELS; i++ )
    {
        if ( pmain_session->pSessions[i] != 0 )
        {
            // get psession_info as void*
            psession_info = pmain_session->pSInfos[i];

            // close this one and set session to invalid
            NrpCloseSensor( pmain_session->pSessions[i] );
            pmain_session->pSessions[i] = 0;

            psession_infos[count] = psession_info;

            // next session info
            count++;
        }
    }

    /*=====================================================================*
     * Delete all cache tables
     *=====================================================================*/
    for ( i = 0; psession_infos[i]; i++ )
    {
        DeleteCriticalSection(&(psession_infos[i]->cs));
        if ( psession_infos[i]->CacheTable != NULL )
        {
            for ( j = 0; j < command_count; j++)
            {
                if (( psession_infos[i]->CacheTable[j].type == DATA_STRING ) &&
                    ( psession_infos[i]->CacheTable[j].current.pstring != NULL ))
                {
                    free(psession_infos[i]->CacheTable[j].current.pstring);
                    psession_infos[i]->CacheTable[j].current.pstring = NULL;
                }
            }

            free( psession_infos[i]->CacheTable);
            psession_infos[i]->CacheTable = NULL;

            if ( psession_infos[i]->ArrayMeas )
            {
                free(psession_infos[i]->ArrayMeas);
                psession_infos[i]->ArrayMeas = NULL;
                psession_infos[i]->ArrayMeasSize = 0;
            }


            free(psession_infos[i]);
            psession_infos[i] = NULL;
        }
    }

    memset( pmain_session->pSessions, 0, sizeof(NRP_SESSION) * RSNRPZ_MAX_CHANNELS );

    trace_log( "%s : LeaveCriticalSection()", __FUNC__ );
    LeaveCriticalSection(&(pmain_session->cs));

    DeleteCriticalSection(&(pmain_session->cs));

    /*
     * Delete events
     */
    for (i = 0; i < MAX_REGISTER_EVENTS; i++ )
    {
        if ( pmain_session->EventTable[i].event )
        {
            CloseHandle(pmain_session->EventTable[i].event);
            /* TODO: check error code*/
            pmain_session->EventTable[i].event = NULL;
            pmain_session->EventTable[i].session = 0;
        }
    }

    if ( pmain_session->event )
        CloseHandle(pmain_session->event);

    free(pmain_session);

    RemoveSessionNode( pmain_session );

//Error:

//  if ( ! pAllSessions )
    if ( NumSessionNodes() < 1 )
    {
        if ( pAvailableSensor )
            free( pAvailableSensor );
        pAvailableSensor = 0;

        EnterCriticalSection(&cs);

        if ( boDriverIsOpen == VI_TRUE )
        {
            // close driver if no device change call back is registered
            if( pCallbackFn == 0 )
            {
                NrpCloseDriver();
                 boDriverIsOpen = VI_FALSE;
            }
        }

        LeaveCriticalSection( &cs );

        DeleteCriticalSection( &cs );
        DeleteCriticalSection( &csAllSessions );
        bCsInitialized = VI_FALSE;
    }

    rsnrpz_status = (rsnrpz_status == VI_SUCCESS) ? rsnrpz_status : reset_status;
    return rsnrpz_status;
}

/*****************************************************************************/
/*----------- INSERT INSTRUMENT-DEPENDENT UTILITY ROUTINES HERE -------------*/
/*****************************************************************************/
/*===========================================================================*/
/* Function: Check Instrument                                                */
/* Purpose:  This function fails, if connected instrument is the same as     */
/*           specified by argument                                           */
/*===========================================================================*/
/*
static ViInt32 rsnrpz_CheckInstr (ViSession vi, ViInt32 channel, ViString instr)
{
    SSessionInfo*   psession_info  = 0;

    ViStatus    rsnrpz_status = VI_SUCCESS;

    SMainSession*   pmain_session = V2N(vi);
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    psession_info = pmain_session->pSInfos[channel-1];

    if (strstr (psession_info->SensorType, instr))
        rsnrpz_status = RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;

//Error:
    return rsnrpz_status;
}
*/

/*===========================================================================*/
/* Function: Get Number of commands                                          */
/* Purpose:  This function returns number of commands in static global table.*/
/*           This value is used for memory allocation of chache table.       */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsrpz_GetNbOfCommands"
static ViInt32 rsnrpz_GetNbOfCommands(void)
{
    ViInt32 i;
    for (i = 0; gs_NrpCommands[i].command; i++)
        ; /* Do nothing */
    return i;
}

/*===========================================================================*/
/* Function: Get Index from command                                          */
/* Purpose:  This function returns index position in static global table for */
/*           specified command.                                              */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_GetIndexFromCommand"
static ViStatus rsnrpz_GetIndexFromCommand(
    const ViString  command,
    ViInt32*        index
)
{
    ViStatus    error       = VI_SUCCESS;
    int         i           = 0;
    int         bOK;        // comparison result
    int         iIndex;        // index in reference command
    int         iIndexCmd;    // index in inbound command

    if ( index )
        *index = -1;

    for (i = 0; gs_NrpCommands[i].command; i++)
    {
        /*
        // -> no good - we need a smarter approach here !
        if ( strcmp( gs_NrpCommands[i].command, command) == 0 )
        {
            if ( index ) *index = i;
            break;
        }
        */

        // compare AAAaa:BBBbb:CCCcc against the following inbound commands
        //  - AAA:BBB:CCC        OK
        //  - AA:BB:CC            FAIL
        //  - AAAa:BBBb:CCCc    OK
        //  - AAAx:BBBy:CCCz    FAIL
        //  - AAA:BBB            FAIL
        //  - AAA:BBB:CCC:DDD    FAIL

        bOK         = TRUE;     // comparison result
        iIndex      = 0;        // index in reference command
        iIndexCmd   = 0;        // index in inbound command

#if !(defined(__APPLE__) || defined(__linux__))
  #pragma warning (push)
  #pragma warning (disable: 4127 )  // conditional expression is constant
#endif

        while( TRUE )

#if !(defined(__APPLE__) || defined(__linux__))
  #pragma warning (pop)
#endif
        {
            // the reference command ends but the inbound command may still
            // contain characters (beyond the optional ones)
            if( gs_NrpCommands[i].command[iIndex]==0 )
            {
                // the inbound command also ends -> this is OK
                if( command[iIndexCmd]==0 )
                    break;

                bOK = FALSE;
                break;
            }

            // the inbound command ends
            if( command[iIndexCmd]==0 )
            {
                // check if there are still mandatory characters left in the reference string
                while( gs_NrpCommands[i].command[iIndex]!=0 )
                {
                    if( (gs_NrpCommands[i].command[iIndex]>='0' && gs_NrpCommands[i].command[iIndex]<='9') ||
                        (gs_NrpCommands[i].command[iIndex]>='A' && gs_NrpCommands[i].command[iIndex]<='Z') ||
                         gs_NrpCommands[i].command[iIndex]=='?' || gs_NrpCommands[i].command[iIndex]=='*'
                      )
                    {
                        bOK = FALSE;
                        break;
                    }

                    iIndex++;
                }
                break;
            }

            // mandatory, capital letters or numbers, or ?, or *
            if( (gs_NrpCommands[i].command[iIndex]>='0' && gs_NrpCommands[i].command[iIndex]<='9') ||
                (gs_NrpCommands[i].command[iIndex]>='A' && gs_NrpCommands[i].command[iIndex]<='Z') ||
                 gs_NrpCommands[i].command[iIndex]=='?' || gs_NrpCommands[i].command[iIndex]=='*'
              )
            {
                if( gs_NrpCommands[i].command[iIndex]==toupper(command[iIndexCmd] ) )
                {
                    iIndex++;
                    iIndexCmd++;
                    continue;
                }
                else
                {
                    bOK = FALSE;
                    break;
                }
            }

            // next section
            if( command[iIndexCmd]==':' )
            {
                // position both pointers to the character following the :
                iIndexCmd++;
                while( gs_NrpCommands[i].command[iIndex]!=0 && gs_NrpCommands[i].command[iIndex]!=':' )
                {
                    // is there still mandatory stuff remaining in the reference command ?
                    if( (gs_NrpCommands[i].command[iIndex]>='0' && gs_NrpCommands[i].command[iIndex]<='9') ||
                        (gs_NrpCommands[i].command[iIndex]>='A' && gs_NrpCommands[i].command[iIndex]<='Z') ||
                         gs_NrpCommands[i].command[iIndex]=='?' || gs_NrpCommands[i].command[iIndex]=='*'
                      )
                    {
                        bOK = FALSE;
                        break;
                    }
                    iIndex++;
                }
                if( gs_NrpCommands[i].command[iIndex]!=0 )
                    iIndex++;
                continue;
            }

            // optional lower case letters in reference command
            if( (gs_NrpCommands[i].command[iIndex]>='a' && gs_NrpCommands[i].command[iIndex]<='z')
              )
            {
                if( toupper(gs_NrpCommands[i].command[iIndex])==toupper(command[iIndexCmd] ) )
                {
                    iIndex++;
                    iIndexCmd++;
                    continue;
                }
                else
                {
                    bOK = FALSE;
                    break;
                }
            }

            // unknown letter in reference command -> this should not happen -> ignore it for now
            iIndex++;

        }

        if( bOK )
        {
            if ( index )
                *index = i;
            break;
        }

    }

    if ( !gs_NrpCommands[i].command )
    {
        error = RSNRPZ_ERROR_COMMAND_NOT_FOUND;
    }

    return error;
}

/*===========================================================================*/
/* Function: Get Bit from value                                              */
/* Purpose:  This function returns bit in bitfield, which is associated to   */
/*           value is specified table. If the value is not found in the      */
/*           table an error is returned.                                     */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_GetBitFromValue"
static ViStatus rsnrpz_GetBitFromValue(
    const SRangeTableEntry* table,
    ViInt32     value,
    ViInt32*    pBit
)
{
    ViStatus    error = VI_SUCCESS;

    int i = 0;

    if ( pBit) *pBit = 0;

    for (i = 0; table[i].parameter; i++)
    {
        if (table[i].value == value)
        {
            if ( pBit) *pBit = table[i].bit;
            break;
        }
    }

    if ( !table[i].parameter )
    {
        error = RSNRPZ_ERROR_INVALID_VALUE;
    }

    return error;
}

/*===========================================================================*/
/* Function: Get Bit from string                                             */
/* Purpose:  This function returns bit in bitfield, which is associated to   */
/*           string is specified table. If the string is not found in the    */
/*           table an error is returned.                                     */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_GetBitFromString"
static ViStatus rsnrpz_GetBitFromString(
    const SRangeTableEntry* table,
    const ViString          cpCmd,
    ViInt32*                pBit
)
{
  ViStatus    error = VI_SUCCESS;

  int i = 0;

  if ( pBit) *pBit = 0;

  for (i = 0; table[i].parameter; i++)
  {
    if ( StrCmpMandatorySCPI( table[i].parameter, cpCmd ) != 0 )
      continue;

    if ( pBit) *pBit = table[i].bit;
    break;
  }

  if ( ! table[i].parameter )
  {
    error = RSNRPZ_ERROR_INVALID_VALUE;
  }

  return error;
}



/*===========================================================================*/
/* Function: Get String From Value                                           */
/* Purpose:  This function returns string, which is associated to            */
/*           value is specified table. If the value is not found in the      */
/*           table an error is returned.                                     */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_GetStringFromValue"
static ViStatus rsnrpz_GetStringFromValue(
    const SRangeTableEntry* table,
    ViInt32                 value,
    const ViChar**          pParameter
)
{
    ViStatus    error = VI_SUCCESS;

    int i = 0;

    if ( pParameter) *pParameter = NULL;

    for (i = 0; table[i].parameter; i++)
    {
        if (table[i].value == value)
        {
            if ( pParameter) *pParameter = table[i].parameter;
            break;
        }
    }

    if ( !table[i].parameter )
    {
        error = RSNRPZ_ERROR_INVALID_VALUE;
    }

    return error;
}

/*===========================================================================*/
/* Function: Get Value From Bit                                              */
/* Purpose:  This function returns value, which is associated to bitMask     */
/*           is specified table. If the bit is not found in the              */
/*           table an error is returned.                                     */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_GetValueFromBit"
static ViStatus rsnrpz_GetValueFromBit(
    const SRangeTableEntry* table,
    ViInt32             bitMask,
    ViInt32*            pValue
)
{
    ViStatus    error = VI_SUCCESS;

    int i = 0;

    if ( pValue) *pValue = 0;

    for (i = 0; table[i].parameter; i++)
    {
        if (table[i].bit == bitMask)
        {
            if ( pValue) *pValue = table[i].value;
            break;
        }
    }

    if ( !table[i].parameter )
    {
        error = RSNRPZ_ERROR_INVALID_VALUE;
    }

    return error;
}

/*===========================================================================*/
/* Function: Get Index from packet                                           */
/* Purpose:  This function returns index position in static global table for */
/*           specified command (specified by group and parameter numbers).   */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_GetIndexFromPacket"
static ViStatus rsnrpz_GetIndexFromPacket(
    ViInt32     groupNb,
    ViInt32     paramNb,
    ViInt32*    index
)
{
    ViStatus    error = VI_SUCCESS;
    int i = 0;

    if ( index ) *index = -1;
    for (i = 0; gs_NrpCommands[i].command; i++)
    {
        if ( ( gs_NrpCommands[i].groupNb == (ViInt16)groupNb ) &&
             ( gs_NrpCommands[i].paramNb == (ViInt16)paramNb ) )
        {
            if ( index ) *index = i;
            break;
        }
    }

    if ( !gs_NrpCommands[i].command )
    {
        error = RSNRPZ_ERROR_COMMAND_NOT_FOUND;
    }

    return error;
}


#if ! defined (USE_CHECKINSTR)
/*===========================================================================*/
/* Function: IsSupported                                                     */
/* Purpose:  This function checks if a command of a specified group and      */
/*           parameter number is supported                                   */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_IsSupported"
static ViBoolean rsnrpz_IsSupported( ViSession vi,
                                     ViInt32   channel,
                                     ViInt32   groupNb,
                                     ViInt32   paramNb )
{
    ViStatus      err           = VI_SUCCESS;
    ViInt32       index         = -1;
    SSessionInfo *pSessInfo     = 0;
    SMainSession *pmain_session = 0;

    err = rsnrpz_GetIndexFromPacket( groupNb, paramNb, &index );
    if ( err != VI_SUCCESS )
        return VI_FALSE;            // Command not found

    if ( index < 0 )
        return VI_FALSE;            // Command not found

    if ( channel == ALL_CHANNELS )  // works only for a single channel
        return VI_FALSE;

    if ( channel < 1 )
        return VI_FALSE;            // channel invalid...

    if ( channel > RSNRPZ_MAX_CHANNELS )
        return VI_FALSE;

    // get associated session and check
    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_FALSE;

    if ( pmain_session->pSessions[channel-1] == 0)
        return VI_FALSE;            // session invalid

    pSessInfo = pmain_session->pSInfos[channel-1];
    if ( pSessInfo->CacheTable[index].IsAvailable == 0 )
        return VI_FALSE;

    return VI_TRUE;
}
#endif


/*===========================================================================*/
/* Function: Check Range                                                     */
/* Purpose:  This function checks if the command on the specified index      */
/*           position is supported. If the command is supported, function    */
/*           check correct range of the value.                               */
/*           If the value is NULL pointer function is checking if command is */
/*           supported.                                                      */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_CheckRange"
static ViStatus rsnrpz_CheckRange(
    NrpValueTable   table,
    ViInt32         index,
    void*           value
)
{
    ViStatus    rsnrpz_status = NRP_SUCCESS;

    if ( ! table[index].IsAvailable )
    {
        CHECKERR_CU( RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED);
    }

    if ( value != NULL )
    {
        switch ( table[index].type )
        {
        case DATA_BITFIELDPARAM:
            {
            long new_value = *(long*)value;
            ViInt32 bit;
            long mask = table[index].maximum.lvalue;

            CHECKERR( rsnrpz_GetBitFromValue(gs_NrpCommands[index].RangeTable,
                                             new_value,
                                             &bit));
            if (( mask & bit ) == 0 )
            {
                CHECKERR_CU( RSNRPZ_ERROR_INVALID_VALUE);
            }
            }
            break;

        // DATA_LONGPARAM is always 32 bit value
        case DATA_LONGPARAM:
            {
                long new_value = (long)( *(ViInt32*)value );

                if (( table[index].minimum.lvalue > new_value ) ||
                    ( table[index].maximum.lvalue < new_value ))
                {
                    CHECKERR_CU( RSNRPZ_ERROR_INVALID_VALUE);
                }
            }
            break;

        case DATA_FLOATPARAM:
            {
                float new_value = *(float*)value;

                if (( table[index].minimum.fvalue > new_value ) ||
                    ( table[index].maximum.fvalue < new_value ))
                {
                    CHECKERR_CU( RSNRPZ_ERROR_INVALID_VALUE);
                }
            }
            break;
        default:
            rsnrpz_status = NRP_SUCCESS;
            break;
        }
    }

Error:
    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Format Command                                                  */
/* Purpose:  This function formats command to buffer.                        */
/*                                                                           */
/* Warning:  Function does not check size of the buffer.                     */
/*           Suggested size of the buffer is >256 bytes.                     */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_FormatCommand"
static ViStatus rsnrpz_FormatCommand(
    NrpValueTable   table,
    ViChar          buffer[],
    ViInt32         index,
    void*           value
)
{
    ViStatus    rsnrpz_status = VI_SUCCESS;
    ViChar*        pCmd;
    ViChar        szDstCmd[1024];
    ViInt32        i;

    // do not copy the non-capital letters
    pCmd = (ViChar*)gs_NrpCommands[index].command;
    i = 0;
    while( *pCmd!=0 && (i<(ViInt32)sizeof(szDstCmd)-1) )
    {
        if( *pCmd>='a' && *pCmd<='z' )
        {
            pCmd++;
            continue;
        }

        szDstCmd[i]=*pCmd;
        pCmd++;
        i++;
    }
    szDstCmd[i]=0;

    if ( value != NULL )
    {
        switch ( table[index].type )
        {

        // DATA_BITFIELDPARAM is always 32-bit
        case DATA_BITFIELDPARAM:
            {
            const char*  pparameter = NULL;

            if ( gs_NrpCommands[index].RangeTable )
            {
                CHECKERR( rsnrpz_GetStringFromValue(
                                        gs_NrpCommands[index].RangeTable,
                                        *(ViInt32*)value,
                                        &pparameter));

                sprintf( buffer, "%s %s", szDstCmd, pparameter);
            }
            else
            {
                CHECKERR( RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED);
            }
            }
            break;

        // DATA_LONGPARAM is always 32 bit value
        case DATA_LONGPARAM:
            sprintf( buffer, "%s %ld", szDstCmd, (long)( *(ViInt32*)value ) );
            break;

        case DATA_FLOATPARAM:
        {
            char szBuf[80];
            C_dblstr( (double)(*(float*)value), "%e", szBuf, sizeof( szBuf ) );
            sprintf( buffer, "%s %s", szDstCmd, szBuf );
        }   break;
        case DATA_STRING:
            sprintf( buffer, "%s %s", szDstCmd, (char*)value);
            break;
        }
    }
    else
    {
        strcpy( buffer, szDstCmd );
    }

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Value From Cache                                            */
/* Purpose:  This function returns a value from the cache.                   */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_GetValueFromCache"
static ViStatus rsnrpz_GetValueFromCache(
    NrpValueTable   table,
    ViInt32         index,
    void*           value,
    ViInt32         parameterSize
)
{
    ViStatus    rsnrpz_status = NRP_SUCCESS;

    if ( ! table[index].IsAvailable )
    {
        CHECKERR( RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED);
    }

    if ( value != NULL )
    {
        switch ( table[index].type )
        {

        // DATA_BITFIELDPARAM is always 32 bit value
        case DATA_BITFIELDPARAM:
            {
                ViInt32    read_value;
                if ( parameterSize != 4 )
                    return RSNRPZ_ERROR_SMALL_BUFFER;

                CHECKERR( rsnrpz_GetValueFromBit( gs_NrpCommands[index].RangeTable,
                                                  table[index].current.lvalue,
                                                  &read_value));
                *((ViInt32*)value) = read_value;
            }
            break;

        // DATA_LONGPARAM is always 32 bit value
        case DATA_LONGPARAM:
            {
                if ( parameterSize != 4 )
                    return RSNRPZ_ERROR_SMALL_BUFFER;
                *((ViInt32*)value) = (ViInt32) table[index].current.lvalue;
            }
            break;

        case DATA_FLOATPARAM:
            {
                if ( parameterSize < (ViInt32)sizeof(float) )
                    return RSNRPZ_ERROR_SMALL_BUFFER;

                *((float*)value) = table[index].current.fvalue;
            }
            break;

        case DATA_STRING:
            {
                int length = 0;
                int copy_count = 0;

                if ( table[index].current.pstring != NULL )
                {
                    length = (int)strlen(table[index].current.pstring);
                }

                if ( parameterSize < (length + 1) )
                {
                    copy_count = (parameterSize > 0) ? parameterSize-1 : 0;
                    rsnrpz_status = (length + 1 );
                }
                else
                    copy_count = length;

                if ( value )
                {
                    memcpy( value, table[index].current.pstring, copy_count );
                    *(((char*)value) + copy_count) = '\0';
                }

            }
            break;
        default:
            rsnrpz_status = NRP_SUCCESS;
            break;
        }
    }

    return rsnrpz_status;
}

#define IS_SET_BIT(feature, bit) \
    (*(feature + (bit/32) ) & (1L << (bit-((bit/32) * 32))))

/*===========================================================================*/
/* Function: Set Group feature                                               */
/* Purpose:  This function parse group feature and sets IsAvailable flag     */
/*           in cache for all commands which are supported for specified     */
/*           sensor.                                                         */
/* Warning:  Function sets only flag for available commands, and it assumes  */
/*           that all IsAvailable flags are set ti zero. This is done in init*/
/*           when driver call memset for cache zeroing                       */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_SetGroupFeature"
static ViStatus rsnrpz_SetGroupFeature(
    NRP_SESSION session,
    ViInt32     groupNb,
    long        feature[],
    ViInt32     count
)
{
    long        rsnrpz_status = NRP_SUCCESS;
    ViUInt32    i = 0;
    ViInt32        index;

    SSessionInfo *psession_info = GetSesInfoFromSession(session);
    if( psession_info==0 )
        return RSNRPZ_ERROR_INVALID_VALUE;

    for (; i < count * sizeof(ViInt32) * 8; i++)
    {
        if ( IS_SET_BIT(feature, i) )
        {
            rsnrpz_GetIndexFromPacket(groupNb, (ViInt32)(i+1), &index);
            if ( index >= 0 )
            {
               psession_info->CacheTable[index].IsAvailable = 1;
            }
        }
    }

//Error:
    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Write Command                                                   */
/* Purpose:  This function write specifies command to sensor. If the channel */
/*           is set to ALL_CHANNELS, command is sent to all sensors          */
/*           which are attached to the session.                              */
/*           Function is checking correct range.                             */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_WriteCommand"
static ViStatus rsnrpz_WriteCommand(
    ViSession       vi,
    ViInt32         channel,
    const ViString  command,
    void*           parameter
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    ViStatus        error = VI_SUCCESS;
    char            buffer[BUFFER_SIZE];
    char*            pend = buffer;
    SSessionInfo*   psession_info  = 0;
    int                i, command_length = (int)strlen(command);
    ViInt32            index = 0;
    ViUInt32        count = 0;
    DWORD            dw_error;
    ViBoolean        is_ack_required = VI_TRUE;

    // get associated session and check
    SMainSession *pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    memset( pmain_session->hHandlesWr,
            0,
            sizeof( pmain_session->hHandlesWr ) );

    for (i = 0; i < command_length; i++, pend++)
        *pend = (char)toupper(command[i]);
    *pend = '\0';

    rsnrpz_status = rsnrpz_GetIndexFromCommand( buffer, &index );

    if ( index < 0 )
        rsnrpz_status = RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED;
    CHECKERR( rsnrpz_status);

    trace_log( "WriteCmd( '%s' )", command );

    /*
     * CAL:ZERO ONCE does not wait for 'R' packet
     */
    if ((gs_NrpCommands[index].groupNb == 1) &&
        (gs_NrpCommands[index].paramNb == 2))
    {
        is_ack_required = (ViBoolean)( *((ViInt32*)parameter) != RSNRPZ_ONCE );
        trace_log( "CAL:ZERO:AUTO - ACK required : %d ", (int)is_ack_required );
    }

    if ( channel == ALL_CHANNELS )
    {
        ViInt32     channels[RSNRPZ_MAX_CHANNELS];
        ViInt32     valid_channels;

        memset(channels, 0, sizeof(channels));

        /*============================================================*
         *  Check range for all connected sensors
         *============================================================*/
        valid_channels = 0;
        for (i = 0 ; i < RSNRPZ_MAX_CHANNELS; i++ )
        {
            if ( pmain_session->pSessions[i] )
            {
                psession_info = pmain_session->pSInfos[i];

                rsnrpz_status = rsnrpz_CheckRange( psession_info->CacheTable,
                                                   index,
                                                   parameter);
                if ( rsnrpz_status == VI_SUCCESS )
                    channels[valid_channels++] = i+1;
                else
                    error = rsnrpz_status;
            }
        }

        if ( valid_channels == 0 && error != VI_SUCCESS )
        {
            CHECKERR_CU( error);
        } // if ( count == 0 && error != VI_SUCCESS )

        /*
         * Sensors are not connected
         */
        if ( valid_channels == 0 )
            return VI_SUCCESS;

        /*=========================================================*
         * Format command
         *=========================================================*/
        { /* inner block */

        psession_info = pmain_session->pSInfos[channels[0]-1];

        CHECKERR_CU( rsnrpz_FormatCommand( psession_info->CacheTable,
                              buffer,
                              index,
                              parameter
                            ));
        }

        /*=========================================================*
         * Create events for every sensor associated with command
         *=========================================================*/
        if ( is_ack_required )
        {
            if ( error == VI_SUCCESS )
            {
                trace_log("CreateEvents in "__FUNC__);
                CHECKERR_CU( rsnrpz_CreateEvents( vi,
                                                  channel,
                                                  gs_NrpCommands[index].groupNb,
                                                  gs_NrpCommands[index].paramNb,
                                                  pmain_session->hHandlesWr,
                                                  &count));
            }
            else
            {
                HANDLE* phandle = pmain_session->hHandlesWr;

                ViInt32 total_count = 0;
                for (i = 0 ; channels[i]; i++ )
                {
                    trace_log("CreateEvents 2 in "__FUNC__);
                    rsnrpz_status = rsnrpz_CreateEvents( vi,
                                                         channels[i],
                                                         gs_NrpCommands[index].groupNb,
                                                         gs_NrpCommands[index].paramNb,
                                                         phandle,
                                                         &count);
                    if ( rsnrpz_status < VI_SUCCESS )
                        break;
                    phandle     += count;
                    total_count += count;
                }

                if ( rsnrpz_status < VI_SUCCESS )
                {
                    trace_log("DestroyEvents in "__FUNC__);
                    error = rsnrpz_DestroyEvents( vi,
                                                  pmain_session->hHandlesWr,
                                                  total_count );
                    CHECKERR_CU( rsnrpz_status);
                }

                count = total_count;
            }
        }

        /*================================================================*
         * Send command to all channels, which has correct range checking
         *================================================================*/
        for (i = 0 ; channels[i]; i++ )
        {
            if ( pmain_session->pSessions[channels[i] - 1] )
            {
                 rsnrpz_NotifyCommand( vi,
                                       channels[i],
                                       gs_NrpCommands[index].groupNb,
                                       gs_NrpCommands[index].paramNb,
                                       0, VI_FALSE);

                 error = NrpSendCommand( pmain_session->pSessions[channels[i] - 1],
                                           buffer,
                                           0 /* gs_ulWriteTimeout */
                                       );
            }
        }

        if ( is_ack_required )
        {
            dw_error = WaitForMultipleObjects( count,
                                               pmain_session->hHandlesWr,
                                               TRUE,
                                               gs_ulWriteTimeout
                                             );

            trace_log("DestroyEvents 2 in "__FUNC__);
            error = rsnrpz_DestroyEvents( vi,
                                          pmain_session->hHandlesWr,
                                          count );

            if ( dw_error == WAIT_TIMEOUT )
            {
                CHECKERR_CU( RSNRPZ_ERROR_TIMEOUT);
            }
        }
        CHECKERR_CU( error);
    }
    else
    if ( channel > 0 && channel <= RSNRPZ_MAX_CHANNELS )
    {
        /*
         * Send command to specified sensor
         */
        if ( pmain_session->pSessions[channel-1] )
        {
            psession_info = pmain_session->pSInfos[channel-1];

            CHECKERR( rsnrpz_CheckRange( psession_info->CacheTable,
                                         index,
                                         parameter));

            CHECKERR( rsnrpz_FormatCommand( psession_info->CacheTable,
                                            buffer,
                                            index,
                                            parameter
                                          ));

            if ( is_ack_required )
            {
                trace_log("CreateEvents 3 in "__FUNC__);
                CHECKERR( rsnrpz_CreateEvents( vi,
                                               channel,
                                               gs_NrpCommands[index].groupNb,
                                               gs_NrpCommands[index].paramNb,
                                               pmain_session->hHandlesWr,
                                               &count));
            }

            rsnrpz_NotifyCommand( vi,
                                  channel,
                                  gs_NrpCommands[index].groupNb,
                                  gs_NrpCommands[index].paramNb,
                                  0, VI_FALSE);

            rsnrpz_status = NrpSendCommand( pmain_session->pSessions[channel-1],
                                            buffer,
                                            0);

            if ( rsnrpz_status != VI_SUCCESS )
            {
                error = rsnrpz_DestroyEvents( vi,
                                              pmain_session->hHandlesWr,
                                              count );

                CHECKERR( rsnrpz_status);
//              CHECKERR( error);

            }

            if ( is_ack_required )
            {
                dw_error = WaitForSingleObject( pmain_session->hHandlesWr[0],
                                                gs_ulWriteTimeout );

                trace_log("DestroyEvents 3 in "__FUNC__);
                error = rsnrpz_DestroyEvents( vi,
                                              pmain_session->hHandlesWr,
                                              count );

                if ( dw_error == WAIT_TIMEOUT )
                {
                    CHECKERR_CU( RSNRPZ_ERROR_TIMEOUT);
                }
            }
            CHECKERR( rsnrpz_status);
            CHECKERR( error);
        }
        else
            CHECKERR( RSNRPZ_ERROR_INCORRECT_CHANNEL);
    }
    else
        CHECKERR( RSNRPZ_ERROR_INCORRECT_CHANNEL);

Error:
    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Read Value                                                      */
/* Purpose:  This function queries specifies value from sensor.              */
/*           If the channel is set to ALL_CHANNELS, command is set to all    */
/*           sensors attached to session and waits up to value is stored in  */
/*           cahce for all channels. Function does not query value from cache*/
/*           when ALL_CHANNELS is passed in channel parameter.               */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_ReadValue"
static ViStatus rsnrpz_ReadValue(
    ViSession       vi,
    ViInt32         channel,
    const ViString  command,
    void*           parameter,
    int             parameterSize
)
{
    ViStatus    rsnrpz_status        = VI_SUCCESS;
    ViStatus    error                = VI_SUCCESS;
    char        buffer[BUFFER_SIZE];
    char*       pend                = buffer;
    int         i, command_length    = (int)strlen(command);
    ViInt32     index                = 0;
    ViUInt32    count                = 0;
    DWORD       dw_error            = 0;

    // get associated session and check
    SMainSession *pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    trace_log( "%s(vi=%d, channel=%d, command=%s)", __FUNC__, vi, channel, command );

    for (i = 0; i < command_length; i++, pend++)
        *pend = (char)toupper(command[i]);
    *pend = '\0';

    rsnrpz_status = rsnrpz_GetIndexFromCommand( buffer, &index );

    *pend++ = '?';
    if ( parameter && ! strcmp( (char *)parameter, "Uptime" ) )
        strcpy( pend, " \"UPTIME\"" );
    else
        *pend = '\0';

    if ( index < VI_SUCCESS )
    {
        CHECKERR( RSNRPZ_ERROR_COMMAND_NOT_SUPPORTED);
    }
    if ( channel == ALL_CHANNELS )
    {
        trace_log( "Calling rsnrpz_CreateEvents()" );
        CHECKERR( rsnrpz_CreateEvents( vi,
                                       channel,
                                       gs_NrpCommands[index].groupNb,
                                       gs_NrpCommands[index].paramNb,
                                       pmain_session->hHandlesRd,
                                       &count));

        /*================================================================*
         * Send command to all channels, which has correct range checking
         *================================================================*/
        for (i = 0; i < RSNRPZ_MAX_CHANNELS; i++ )
        {
            if ( pmain_session->pSessions[i] )
            {
                 error = NrpSendCommand( pmain_session->pSessions[i],
                                           buffer,
                                           0 /* gs_ulWriteTimeout */
                                       );
                 if (error!=VI_SUCCESS)
                     break;
            }
        }

        if (error==VI_SUCCESS)
            dw_error = WaitForMultipleObjects( count,
                                            pmain_session->hHandlesRd,
                                            TRUE,
                                            gs_ulWriteTimeout
                                            );

        trace_log( "Calling rsnrpz_DestroyEvents()" );
        error = rsnrpz_DestroyEvents( vi,
                                      pmain_session->hHandlesRd,
                                      count );

        CHECKERR_CU( error);
        if ( dw_error == WAIT_TIMEOUT )
        {
            CHECKERR_CU( RSNRPZ_ERROR_TIMEOUT);
        }
    }
    else
    if ( channel > 0 && channel <= RSNRPZ_MAX_CHANNELS )
    {
        /*
         * Send command to a specified sensor
         */
        if ( pmain_session->pSessions[channel-1] )
        {
            SSessionInfo *psession_info = pmain_session->pSInfos[channel-1];

            CHECKERR( rsnrpz_CheckRange( psession_info->CacheTable,
                             index, NULL));

            // ----------------------------------------------------------
            // If pulse-measurement is ON  AND  the command-group IS 16
            // we can get the results from the internal cache. Otherwise
            // we must request them from the device....
            // ----------------------------------------------------------
            if ( (psession_info->IsPulseMeasurementOn == VI_FALSE)
              || ( gs_NrpCommands[index].groupNb != 0x10)
              || ((gs_NrpCommands[index].groupNb == 0x10) && (gs_NrpCommands[index].paramNb < 8))
               )
            {
                // create the event for the group and parameter number and
                // return the event handles
                CHECKERR( rsnrpz_CreateEvents( vi,
                                               channel,
                                               gs_NrpCommands[index].groupNb,
                                               gs_NrpCommands[index].paramNb,
                                               pmain_session->hHandlesRd,
                                               &count ));

                rsnrpz_status = NrpSendCommand( pmain_session->pSessions[channel-1],
                                                buffer, 0);

                if( rsnrpz_status==VI_SUCCESS )
                {
                    trace_log( "Calling WaitForSingleObject(TIMO=%d ms)", gs_ulWriteTimeout );
                    dw_error = WaitForSingleObject( pmain_session->hHandlesRd[0],
                                                    gs_ulWriteTimeout );
                }

                error = rsnrpz_DestroyEvents( vi,
                                              pmain_session->hHandlesRd,
                                              count );

                trace_log( "NrpSendCommand() returned %d", rsnrpz_status );
                CHECKERR( rsnrpz_status);

                if ( dw_error == WAIT_TIMEOUT )
                {
                    trace_log( "WaitForSingleObject() returned TIMEOUT error." );
                    CHECKERR_CU( RSNRPZ_ERROR_TIMEOUT );
                }
                CHECKERR( error );
            }

            trace_log("Reading cached values for channel %d in "__FUNC__, channel);
            EnterCriticalSection(&(psession_info->cs));
            rsnrpz_status = rsnrpz_GetValueFromCache( psession_info->CacheTable,
                                                      index,
                                                      parameter,
                                                      parameterSize);
            LeaveCriticalSection(&(psession_info->cs));
            CHECKERR(rsnrpz_status);

        }
        else
            CHECKERR( RSNRPZ_ERROR_INCORRECT_CHANNEL);
    }
    else
    {
        CHECKERR( RSNRPZ_ERROR_INCORRECT_CHANNEL);
    }

Error:
    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Initialize Register                                             */
/* Purpose:  This function initialize register to default state.             */
/*           Default state is following:                                     */
/*              all bits in condition registers are set to 0                 */
/*              all bits in positive transition registers are set to 1       */
/*              all bits in negative transition registers are set to 0       */
/*              all bits in event registers are set to 0                     */
/*              all bits in enabled registers are set to 0, but following    */
/*              registers have exception:                                    */
/*              - QUESTIONABLE_STATUS_REGISTER bits are set to 1             */
/*              - OPERATION_STATUS_REGISTER bits are set to 1                */
/*              - STATUS_BYTE_REGISTER bits(1,4,7) are set to 1              */
/*                                                                           */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_InitializeRegisters"
static ViStatus rsnrpz_InitializeRegisters(
    TNrpRegister    pRegisters[]
)
{
    int i = 0;

    memset(pRegisters, 0, sizeof(TNrpRegister) * NB_OF_NRP_REGISTERS);

    /*
     * Set positive transition of all registers to 111...111
     */
    for ( i = 0; i < NB_OF_NRP_REGISTERS; i++ )
    {

        memset( &pRegisters[i][PTRANSITION_REGISTER],
            0xFF,
            sizeof(TRegister)
          );
    }

    memset( &pRegisters[QUESTIONABLE_STATUS_REGISTER][ENABLE_REGISTER],
            0xFF,
            sizeof(TRegister)
          );

    memset( &pRegisters[OPERATION_STATUS_REGISTER][ENABLE_REGISTER],
            0xFF,
            sizeof(TRegister)
          );

    memset( &pRegisters[STATUS_BYTE_REGISTER][ENABLE_REGISTER],
            0x8A,
            1
          );

    return VI_SUCCESS;
}

/*===========================================================================*/
/* Function: Update Registers                                                */
/* Purpose:  This function updates specifies register                        */
/*           If condition register is updated, function changes event        */
/*           register up to status byte STB.                                 */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_UpdateRegisters"
static ViStatus rsnrpz_UpdateRegisters(
    SMainSession*   pMainSession,
    int             nrpRegister,
    int             registerType,
    TRegister       mask
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    ViBoolean       summary_bit;
    TNrpRegister*   pregister = (pMainSession->NrpRegisters) + nrpRegister;

    CHECKERR( rsnrpz_SetRegister( pregister, registerType, mask, &summary_bit));

    if (( registerType == CONDITION_REGISTER ) ||
        ( registerType == EVENT_REGISTER )
        )

    {
        switch ( nrpRegister )
        {
        case DEVICE_STATUS_REGISTER_ERROR:
        case DEVICE_STATUS_REGISTER_CONNECTED:
            CHECKERR( rsnrpz_SetBitInRegisters( pMainSession, STATUS_BYTE_REGISTER, CONDITION_REGISTER, 1, summary_bit));
            break;
        case QUESTIONABLE_STATUS_REGISTER:
            CHECKERR( rsnrpz_SetBitInRegisters( pMainSession, STATUS_BYTE_REGISTER, CONDITION_REGISTER, 3, summary_bit));
            break;
        case STANDARD_EVENT_STATUS_REGISTER:
            CHECKERR( rsnrpz_SetBitInRegisters( pMainSession, STATUS_BYTE_REGISTER, CONDITION_REGISTER, 5, summary_bit));
            break;
        case STATUS_BYTE_REGISTER:
            if ( summary_bit == VI_TRUE )
            {
#if ! defined(__linux__) && ! defined(__APPLE__)
                if (pMainSession->MessageID > 0 && pMainSession->hMessageWindow != NULL )
                    PostMessage(pMainSession->hMessageWindow, pMainSession->MessageID, 0, 0);
#endif
            }
            break;
        case OPERATION_STATUS_REGISTER:
            CHECKERR( rsnrpz_SetBitInRegisters( pMainSession, STATUS_BYTE_REGISTER, CONDITION_REGISTER, 7, summary_bit));

            break;
        case OPERATION_CALIBRATING_STATUS_REGISTER:
            CHECKERR( rsnrpz_SetBitInRegisters( pMainSession, OPERATION_STATUS_REGISTER, CONDITION_REGISTER, 0, summary_bit));
            break;
        case OPERATION_MEASURING_STATUS_REGISTER:
            CHECKERR( rsnrpz_SetBitInRegisters( pMainSession, OPERATION_STATUS_REGISTER, CONDITION_REGISTER, 4, summary_bit));
            break;
        case OPERATION_TRIGGER_STATUS_REGISTER:
            CHECKERR( rsnrpz_SetBitInRegisters( pMainSession, OPERATION_STATUS_REGISTER, CONDITION_REGISTER, 5, summary_bit));
            break;
        case OPERATION_SENSE_STATUS_REGISTER:
            CHECKERR( rsnrpz_SetBitInRegisters( pMainSession, OPERATION_STATUS_REGISTER, CONDITION_REGISTER, 10, summary_bit));
            break;
        case OPERATION_LOWER_LIMIT_FAIL_STATUS_REGISTER:
            CHECKERR( rsnrpz_SetBitInRegisters( pMainSession, OPERATION_STATUS_REGISTER, CONDITION_REGISTER, 11, summary_bit));
            break;
        case OPERATION_UPPER_LIMIT_FAIL_STATUS_REGISTER:
            CHECKERR( rsnrpz_SetBitInRegisters( pMainSession, OPERATION_STATUS_REGISTER, CONDITION_REGISTER, 12, summary_bit));
            break;
        case QUESTIONABLE_POWER_STATUS_REGISTER:
        case QUESTIONABLE_ZERO_STATUS_REGISTER:
            CHECKERR( rsnrpz_SetBitInRegisters( pMainSession, QUESTIONABLE_STATUS_REGISTER, CONDITION_REGISTER, 3, summary_bit));
            break;
        case QUESTIONABLE_WINDOW_STATUS_REGISTER:
            CHECKERR( rsnrpz_SetBitInRegisters( pMainSession, QUESTIONABLE_STATUS_REGISTER, CONDITION_REGISTER, 4, summary_bit));
            break;
        case QUESTIONABLE_CALIBRATING_STATUS_REGISTER:
            CHECKERR( rsnrpz_SetBitInRegisters( pMainSession, QUESTIONABLE_STATUS_REGISTER, CONDITION_REGISTER, 8, summary_bit));
            break;
        }
    }

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set bit In Registers                                            */
/* Purpose:  This function set specifies bit in specified register           */
/*           If condition register is updated, function changes event        */
/*           register up to status byte STB.                                 */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_SetBitInRegisters"
static ViStatus rsnrpz_SetBitInRegisters(
    SMainSession*   pMainSession,
    int             nrpRegister,
    int             registerType,
    ViInt32         bit,
    ViBoolean       state
)
{
    ViStatus        rsnrpz_status   = VI_SUCCESS;
    TRegister       old_state;
    ViUInt32        uiTmp;

    ViUInt32        new_bit = bit - ((bit / 8) * 8);
    unsigned char*  pregistry_element = ((unsigned char*)&old_state) + (bit / 8);


    memset( &old_state, 0, sizeof(TRegister) );
    memcpy( &old_state, &(pMainSession->NrpRegisters[nrpRegister][registerType]), sizeof(TRegister));

    uiTmp = *pregistry_element;
    if ( state )
        uiTmp |= (1 << new_bit);
    else
        uiTmp &= (~(1 << new_bit));

    *pregistry_element = (unsigned char)uiTmp;

    CHECKERR( rsnrpz_UpdateRegisters( pMainSession,
                                      nrpRegister,
                                      registerType,
                                      old_state
                                    ));
    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Set Register                                                    */
/* Purpose:  This function change sprcified part of register                 */
/*           every register has condition, positive transition, negative     */
/*           transition, event and ebable part.                              */
/*           When condition part is updated, function will set changed bit to*/
/*           event register and when binary AND of event and enable part is  */
/*           not 0, pSummary is set to TRUE. pSummary bit should be set to   */
/*           next register.                                                  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_SetRegister"
static ViStatus rsnrpz_SetRegister(
    TNrpRegister*   pNrpRegister,
    int             registerType,
    TRegister       mask,
    ViBoolean*      pSummary
)
{
    ViInt32         summary = 0;
    unsigned char*  pmask            = (unsigned char*)mask;
    unsigned char*  ppositive_tran   = (unsigned char*)&((*pNrpRegister)[PTRANSITION_REGISTER]);
    unsigned char*  pnegative_tran   = (unsigned char*)&((*pNrpRegister)[NTRANSITION_REGISTER]);
    unsigned char*  pevent_register  = (unsigned char*)&((*pNrpRegister)[EVENT_REGISTER]);
    unsigned char*  penable_register = (unsigned char*)&((*pNrpRegister)[ENABLE_REGISTER]);
    int         i;


    if ( registerType == CONDITION_REGISTER )
    {
        TRegister   old_state;
        TRegister   event;
        unsigned char*  pold_state;
        unsigned char*  pevent;

        memcpy( &old_state,
                *pNrpRegister[CONDITION_REGISTER],
                sizeof(TRegister)
              );

        pold_state = (unsigned char*)old_state;
        pevent     = (unsigned char*)event;

        for (i = 0; i < (int)sizeof(TRegister); i++)
        {
            ViUInt32 uiTmp;

            /* positive transition */
            *pevent  = (unsigned char)((*pold_state)^(*pmask));
            *pevent  = (unsigned char)((*pmask)^(*pold_state));
            *pevent  = (unsigned char)((((*pold_state)^(*pmask))&(*pmask)) & (*ppositive_tran));

            /* negative transition */
            uiTmp = *pevent;
            uiTmp |= (((*pold_state)^(*pmask))&(*pold_state)) & (*pnegative_tran);
            *pevent = (unsigned char)uiTmp;
            *pevent_register = (unsigned char)( (ViUInt32)*pevent_register | uiTmp );
            summary |= (*pevent) & (*penable_register);

            penable_register++;
            pevent_register++;
            pevent++;
            pold_state++;
            pmask++;
            ppositive_tran++;
            pnegative_tran++;
        }
        memcpy( (*pNrpRegister)[CONDITION_REGISTER],
                mask,
                sizeof(TRegister)
              );
    }
    else
    {
        memcpy( (*pNrpRegister)[registerType],
                mask,
                sizeof(TRegister)
              );

        if ( registerType == EVENT_REGISTER )
        {
            for (i = 0; i < (int)sizeof(TRegister); i++)
            {
                summary |= (*pevent_register) & (*penable_register);
            }
        }
    }

    if ( pSummary )
        *pSummary = (ViBoolean)((summary) ? VI_TRUE : VI_FALSE);

    return VI_SUCCESS;
}

/*===========================================================================*/
/* Function: Default Instrument Setup                                        */
/* Purpose:  This function sends a default setup to the instrument.  This    */
/*           function is called by the rsnrpz_reset operation and by the     */
/*           rsnrpz_init function if the reset option has not been           */
/*           selected.  This function is useful for configuring any          */
/*           instrument settings that are required by the rest of the        */
/*           instrument driver functions such as turning headers ON or OFF   */
/*           or using the long or short form for commands, queries, and data.*/
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_defaultInstrSetup "
static ViStatus rsnrpz_defaultInstrSetup (
    ViSession   vi,
    ViInt32     channel
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    ViChar          buffer[BUFFER_SIZE] = "";

    CHECKERR( rsnrpz_ReadValue( vi,
                                channel,
                                (char*)"*IDN",
                                buffer,
                                sizeof(buffer)));

    if (rsnrpz_WriteCommand( vi, channel, (char*)"SYST:FEAT?", NULL) != VI_SUCCESS)
    {
        // remove instrument errors
        rsnrpz_checkStatus( vi );

        // fall back to request the features
        CHECKERR( rsnrpz_WriteCommand( vi, channel, (char*)"SYST:INIT", NULL));
    }
    else
    {
        CHECKERR( rsnrpz_WriteCommand( vi, channel, (char*)"SYST:LIM?", NULL));
    }

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Create Events                                                   */
/* Purpose:  This function creates event in event queue for specified        */
/*           channel(s), group number and parameter number and returns       */
/*           handles tof these events                                        */
/*           Driver waits up to events are set. Events are set in            */
/*           CommandAccepted callback.                                       */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_CreateEvents"
static ViStatus rsnrpz_CreateEvents(
    ViSession   vi,
    ViInt32     channel,
    long        groupNb,
    long        paramNb,
    HANDLE*     pEventHandleArray,
    ViUInt32*   pCount
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    SMainSession*   pmain_session = V2N( vi );
    ViInt32         i,count = 0;

    trace_log( "%s(vi=%d, channel=%d, group=%ld, param=%ld)", __FUNC__, vi, channel, groupNb, paramNb  );

    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    trace_log( "%s : EnterCriticalSection()", __FUNC__ );
    EnterCriticalSection(&(pmain_session->cs));

    if ( channel == ALL_CHANNELS )
    {
        for (i = 0 ; i < RSNRPZ_MAX_CHANNELS; i++ )
        {
            if ( pmain_session->pSessions[i] )
            {
                CHECKERR_CU( rsnrpz_CreateEvent( pmain_session->pSessions[i],
                                                 groupNb,
                                                 paramNb,
                                                 &pEventHandleArray[count++]
                                               ) );
            }
        }
    }
    else if (pmain_session->pSessions[channel-1])
    {

        CHECKERR_CU( rsnrpz_CreateEvent( pmain_session->pSessions[channel-1],
                                         groupNb,
                                         paramNb,
                                         &pEventHandleArray[count++]
                                       ) );
    }

Error:
    if ( rsnrpz_status < VI_SUCCESS )
    {
        if ( --count > 0 )
        {
            trace_log("DestroyEvents in "__FUNC__);
            rsnrpz_DestroyEvents( vi, pEventHandleArray, count);
            count = 0;
        }
    }

    if ( pCount )
        *pCount = count;
    if ( pmain_session )
    {
        trace_log( "%s : LeaveCriticalSection()", __FUNC__ );
        LeaveCriticalSection(&(pmain_session->cs));
    }
    return rsnrpz_status;
}


/*===========================================================================*/
/* Function: Destroy Events                                                  */
/* Purpose:  This function closes all specied handles of events in event     */
/*           queue.                                                          */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_DestroyEvents"
static ViStatus rsnrpz_DestroyEvents(
    ViSession   vi,
    HANDLE*     pEventHandleArray,
    ViInt32     count
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    SMainSession*   pmain_session = V2N( vi );
    ViInt32         j,i = 0;

    trace_log( "%s(vi=%d)", __FUNC__, vi );

    if( pmain_session==0 )
    {
        trace_log( "Invalid viSession." );
        return VI_ERROR_PARAMETER1;
    }

    trace_log( "%s : EnterCriticalSection()", __FUNC__ );
    EnterCriticalSection( &(pmain_session->cs) );

    for (i = 0 ; i < count; i++ )
    {
        for (j = 0; j < MAX_REGISTER_EVENTS; j++ )
        {
            if ( pmain_session->EventTable[j].event == pEventHandleArray[i] )
            {
                trace_log( "Deleting event at index pos. %d", i );

                CloseHandle( pmain_session->EventTable[j].event );
                /* TODO: check error code*/

                pmain_session->EventTable[j].event   = 0;
                pmain_session->EventTable[j].session = 0;
            }
        }
    }

    trace_log( "%s : LeaveCriticalSection()", __FUNC__ );
    LeaveCriticalSection( &(pmain_session->cs) );

    trace_log( "Events cleaned up." );

    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Create Event                                                    */
/* Purpose:  This function creates event in event queue for specified        */
/*           sensor session, group number and parameter number and returns   */
/*           handle to this event                                            */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_CreateEvent"
static ViStatus rsnrpz_CreateEvent(
    NRP_SESSION session,
    long        groupNb,
    long        paramNb,
    HANDLE     *pEventHandle
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    SMainSession*   pmain_session  = VI_NULL;
    char            event_name[MAX_PATH];
    ViInt32         i;
    HANDLE*         new_handle = NULL;

    SSessionInfo *psession_info = GetSesInfoFromSession(session);

    trace_log( "%s(session=%ld, group=%ld, param=%ld)", __FUNC__, session, groupNb, paramNb  );

    if( psession_info==0 )
        return RSNRPZ_ERROR_INVALID_VALUE;

    pmain_session = (SMainSession*)(psession_info->MainSession);
    trace_log( "Using main session %ld", pmain_session );

    for (i = 0; i < MAX_REGISTER_EVENTS; i++ )
    {
        if ( pmain_session->EventTable[i].event == 0 )
            break;
    }

    if ( i == MAX_REGISTER_EVENTS )
        CHECKERR_CU( RSNRPZ_ERROR_MAX_REGISTER_EVENTS);

    sprintf( event_name, "event_%ld_%d_%d", session, (int)groupNb, (int)paramNb );
    pmain_session->EventTable[i].session = session;
    pmain_session->EventTable[i].groupNb = (short)groupNb;
    pmain_session->EventTable[i].paramNb = (short)paramNb;
    pmain_session->EventTable[i].event   = CreateEventA( 0, TRUE, FALSE, event_name );
    trace_log( "Event created at event table index %d", i );

#if !defined(__linux__) && !defined(__APPLE__)
    new_handle = pmain_session->EventTable[i].event;
#else
    new_handle = &pmain_session->EventTable[i].event;
#endif

    if ( pmain_session->EventTable[i].event == 0 )
        CHECKERR_CU( RSNRPZ_ERROR_REGISTER_EVENT);

Error:
    if ( pEventHandle )
    {
#if ! defined(__linux__) && ! defined(__APPLE__)
        *pEventHandle = new_handle;
#else
        *pEventHandle = *new_handle;
#endif
    }
    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Event                                                       */
/* Purpose:  This function find event in event queue assicated to specified  */
/*           sensor session, group number and parameter number and set       */
/*           this event.                                                     */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_SetEvent"
static ViStatus rsnrpz_SetEvent(
    NRP_SESSION session,
    long        groupNb,
    long        paramNb
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    SMainSession*   pmain_session;
    ViInt32         i;
    BOOL            error;

    SSessionInfo *psession_info = GetSesInfoFromSession(session);

    trace_log( "%s(session=%ld, group=%ld, param=%ld)", __FUNC__, session, groupNb, paramNb );

    // check for valid session information
    if( psession_info==0 )
    {
        trace_log( "Cannot determine session information." );
        return RSNRPZ_ERROR_INVALID_VALUE;
    }

    // get main session
    pmain_session = (SMainSession*)(psession_info->MainSession);
    trace_log( "Using main session %ld", pmain_session );

    trace_log( "%s : EnterCriticalSection()", __FUNC__ );
    EnterCriticalSection( &(pmain_session->cs) );

    // go through the list of events
    for( i = 0; i < MAX_REGISTER_EVENTS; i++ )
    {
        // search for the event
        if (( pmain_session->EventTable[i].session == session        ) &&
            ( pmain_session->EventTable[i].groupNb == (short)groupNb ) &&
            ( pmain_session->EventTable[i].paramNb == (short)paramNb ) &&
            ( pmain_session->EventTable[i].event   != 0              )
           )
        {
            // matching event found
            trace_log( "Setting event at event table index pos. %d", i );

            error = SetEvent( pmain_session->EventTable[i].event );
            if ( !error )
            {
                trace_log( "Error setting event!" );

                ; /* TODO: an error and set handle to 0 */
            }
        }
    }

//Error:
    if ( pmain_session )
    {
        trace_log( "%s : LeaveCriticalSection()", __FUNC__ );
        LeaveCriticalSection(&(pmain_session->cs));
    }
    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Event                                                       */
/* Purpose:  This function find event in event queue assicated to specified  */
/*           sensor session, group number and parameter number and set       */
/*           this event.                                                     */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_PutError"
static ViStatus rsnrpz_PutError(
    ViSession   vi,
    ViInt32     errorCode,
    ViString    errorMessage
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    SMainSession*   pmain_session = VI_NULL;
    ViInt32         size;

    pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    trace_log( "%s : EnterCriticalSection()", __FUNC__ );
    EnterCriticalSection(&(pmain_session->cs));

    size = GetNbOfErrors( pmain_session->NrpErrors);
    if ( size == MAX_ERROR_QUEUE_SIZE )
    {
        // RemoveFirstError() can only be called from WITHIN a locked
        //                    critical section 'pmain_session->cs' !!!
        RemoveFirstError( vi);
        size--;
    }
    pmain_session->NrpErrors[size].ErrorCode = errorCode;
    if ( errorMessage )
    {
        strncpy( pmain_session->NrpErrors[size].ErrorMessage,
                 errorMessage,
                 MAX_LENGTH_OF_ERROR_MESSGAE-1);
        pmain_session->NrpErrors[size].ErrorMessage[MAX_LENGTH_OF_ERROR_MESSGAE-1] = '\0';
    }
    else
    {
        pmain_session->NrpErrors[size].ErrorMessage[0] = '\0';
    }

    CHECKERR_CU( rsnrpz_SetBitInRegisters(  pmain_session,
                                            STATUS_BYTE_REGISTER,
                                            CONDITION_REGISTER,
                                            2,
                                            VI_TRUE
                                          ));

Error:
    if ( pmain_session )
    {
        trace_log( "%s : LeaveCriticalSection()", __FUNC__ );
        LeaveCriticalSection(&(pmain_session->cs));
    }
    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Get Nuber Of Errors                                             */
/* Purpose:  This function returns number of errors in error queue.          */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "GetNbOfErrors"
static ViInt32 GetNbOfErrors(
    SErrorEntry     errorQueue[]
)
{
    ViInt32 i;

    for (i = 0; i < MAX_ERROR_QUEUE_SIZE && errorQueue[i].ErrorCode; i++)
        ; /* Do nothing */

    return i;
}

/*===========================================================================*/
/* Function:  Remove First Errors                                            */
/* Purpose:   This function deletes the oldest error from error queue.       */
/* Important: RemoveFirstError() can only be called from WITHIN a locked     */
/*                               critical section 'pmain_session->cs' !!!    */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "RemoveFirstError"
static ViInt32 RemoveFirstError(
    ViSession       vi
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    ViInt32         size;

    SMainSession*   pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

//  Leads to a deadlock under LINUX !!!
//  EnterCriticalSection(&(pmain_session->cs));

     size = GetNbOfErrors( pmain_session->NrpErrors);
    if ( size > 1 )
    {
        memmove( pmain_session->NrpErrors,
                 pmain_session->NrpErrors + 1,
                 sizeof(SErrorEntry)*(size-1));
        memset( pmain_session->NrpErrors + (size-1),
                0, sizeof(SErrorEntry)*( MAX_ERROR_QUEUE_SIZE - (size-1) ));
    }
    else if (size == 1)
    {
        memset( pmain_session->NrpErrors,
                0, sizeof(SErrorEntry)*MAX_ERROR_QUEUE_SIZE);

        CHECKERR_CU( rsnrpz_SetBitInRegisters(  pmain_session,
                                                STATUS_BYTE_REGISTER,
                                                CONDITION_REGISTER,
                                                2,
                                                VI_FALSE
                                             ));
    }

Error:
//  if ( pmain_session )
//      LeaveCriticalSection(&(pmain_session->cs));

    return rsnrpz_status;
}

static ViStatus rsnrpz_QueryRegister(
    ViSession       vi,
    int             nrpRegister,
    int             registerType,
    TRegister*      pRegister
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;

    SMainSession*   pmain_session = V2N( vi );
    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    trace_log( "%s : EnterCriticalSection()", __FUNC__ );
    EnterCriticalSection(&(pmain_session->cs));

    memcpy(*pRegister, pmain_session->NrpRegisters[nrpRegister][registerType], sizeof(TRegister));

    if ( pmain_session )
    {
        trace_log( "%s : LeaveCriticalSection()", __FUNC__ );
        LeaveCriticalSection(&(pmain_session->cs));
    }
    return rsnrpz_status;
}

/*=========================================================================*/
/* Function: Check Status                                                  */
/* Purpose:  This function reads Event Status Register to check for        */
/*           possible errors.                                              */
/*=========================================================================*/
static ViStatus rsnrpz_checkStatus (
    ViSession vi
)
{
    ViStatus            rsnrpz_status = VI_SUCCESS;
    SMainSession*       pmain_session = V2N( vi );
    TRegister           event_register;
    ViUInt32*           pregister;

    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    if (pmain_session->ErrorChecking)
    {
        CHECKERR_CU( rsnrpz_QueryRegister( vi,
                                           STANDARD_EVENT_STATUS_REGISTER,
                                           EVENT_REGISTER,
                                           &event_register));
        pregister = event_register;

        trace_log( "%s : EnterCriticalSection()", __FUNC__ );
        EnterCriticalSection(&(pmain_session->cs));

        memset( pmain_session->NrpRegisters[STANDARD_EVENT_STATUS_REGISTER][EVENT_REGISTER],
                0,
                sizeof(TRegister)
                );

        trace_log( "%s : LeaveCriticalSection()", __FUNC__ );
        LeaveCriticalSection(&(pmain_session->cs));

        rsnrpz_status = ((*pregister) & IEEE_ERROR_BITS) ? RSNRPZ_ERROR_INSTRUMENT_ERROR : VI_SUCCESS;
    }

Error:
    return rsnrpz_status;
}

/*===========================================================================*/
/* Function: Set Register Notification on command                            */
/* Purpose:  This function set registry depends on the sent or accpeted      */
/*           command.                                                        */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_NotifyCommand"
static ViStatus rsnrpz_NotifyCommand(
    ViSession   vi,
    ViInt32     channel,
    ViInt32     groupNb,
    ViInt32     paramNb,
    ViInt32     parameter,
    ViBoolean   isCommandAccepted
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    SMainSession*   pmain_session = V2N( vi );
    ViInt32         bit  = -1,
                    bit2 = -1;
    ViBoolean       state = VI_TRUE,
                    state2= VI_TRUE;
    ViInt32         nrp_register  = -1,
                    nrp_register2 = -1;
    ViBoolean        locked = VI_FALSE;

    trace_log( "%s(vi=%d, channel=%d, grp=%d, par=%d)", __FUNC__, vi, channel, groupNb, paramNb );

    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    if ( parameter == 0 )
    {
        // suppress 'unused' warning
    }

    if (( groupNb == 1 ) &&
        ( paramNb == 2 ))
    {
        /****************************************************
         * Zero ONCE
         ****************************************************/
        state        = (ViBoolean)(( isCommandAccepted ) ? VI_FALSE : VI_TRUE);
        bit          = channel;
        nrp_register = OPERATION_CALIBRATING_STATUS_REGISTER;

        if ( isCommandAccepted )
        {
            nrp_register2    = QUESTIONABLE_ZERO_STATUS_REGISTER;
            bit2             = channel;
            state2           = VI_FALSE;
        }
    }
    else
    if (( groupNb == 8 ) &&
        ( paramNb == 2 ))
    {
        /****************************************************
         * SYST:INIT
         ****************************************************/
        state        = (ViBoolean)(( isCommandAccepted ) ? VI_FALSE : VI_TRUE);
        bit          = channel;
        nrp_register = OPERATION_SENSE_STATUS_REGISTER;
    }
    else
    if (( groupNb == 9 ) &&
        ( paramNb == 3 ))
    {
        /****************************************************
         * INIT:IMM
         ****************************************************/
        if ( isCommandAccepted )
        {
            SSessionInfo *psession_info = pmain_session->pSInfos[channel-1];

            EnterCriticalSection(&(psession_info->cs));

            psession_info->IsSingleMeasValid      = VI_FALSE;
            psession_info->IsArrayMeasValid       = VI_FALSE;
            psession_info->IsMeasurementComplete  = VI_FALSE;
            psession_info->IsPulseScalarsComplete = VI_FALSE;

            LeaveCriticalSection(&(psession_info->cs));
        }
    }

    if ( bit > 0 )
    {
        locked = VI_TRUE;
        trace_log( "%s : EnterCriticalSection()", __FUNC__ );
        EnterCriticalSection(&(pmain_session->cs));

        CHECKERR_CU( rsnrpz_SetBitInRegisters(  pmain_session,
                                                nrp_register,
                                                CONDITION_REGISTER,
                                                bit,
                                                state
                                              ));

        if ( bit2 > 0 )
        {
            CHECKERR_CU( rsnrpz_SetBitInRegisters(  pmain_session,
                                                    nrp_register2,
                                                    CONDITION_REGISTER,
                                                    bit2,
                                                    state2
                                                  ));
        }
    }

Error:
    if ( locked )
    {
        trace_log( "%s : LeaveCriticalSection()", __FUNC__ );
        LeaveCriticalSection(&(pmain_session->cs));
    }

    return rsnrpz_status;
}

/*****************************************************************************/
/*= NRP Control Callbacks ===================================================*/
/*****************************************************************************/
/*===========================================================================*/
/* Function: Nrp Command Accepted Callback                                   */
/* Purpose:  This function is called when the sensor accpets a command.      */
/*           The function set an event for the specified sensor and command. */
/*           When some commands are accepted (SYST:INT, CAL:ZERO:AUTO),      */
/*           should be updated. More information about registry subsystem you*/
/*           can find in NRP manual.                                         */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "NrpCommandAcceptedCallback"
static void _NRP_FUNC NrpCommandAcceptedCallback(
    NRP_SESSION sensorHandle,
    long        groupParam,
    NRP_USERARG userArgument
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    SMainSession*   pmain_session = NULL;
    int             channel;
  DWORD           dwWObj = 0;

    // get session info structure from handle
    SSessionInfo *psession_info = GetSesInfoFromSession( sensorHandle );

    trace_log( "%s(handle=%ld, grpprm=%ld)", __FUNC__, sensorHandle, groupParam );

    if( psession_info==0 )
    {
        trace_log( "Invalid sensor handle." );
        return;
    }

    // suppress 'unused' warning
    userArgument = userArgument;

    // get main session from session info
    pmain_session = (SMainSession*)(psession_info->MainSession);

    // determine the channel number
    for ( channel = 0; channel < RSNRPZ_MAX_CHANNELS &&
                       sensorHandle != pmain_session->pSessions[channel];
          channel++)
        ; /* Do nothing */

    if( channel == RSNRPZ_MAX_CHANNELS )
    {
        trace_log( "Cannot determine channel." );
        return;
    }

    // make 1 based
    channel++;

  dwWObj = WaitForSingleObject( pmain_session->event, 0 );

    if ( dwWObj == WAIT_OBJECT_0 )
    {
        short   groupNb = (short)( groupParam & 0xFF  );
        short   paramNb = (short)((groupParam & 0xFF00) >> 8);

        CHECKERR_CU( rsnrpz_NotifyCommand( N2V( pmain_session ),
                                           channel,
                                           groupNb,
                                           paramNb,
                                           0, VI_TRUE));

        rsnrpz_SetEvent(sensorHandle, groupNb, paramNb);
    }

Error:
    if (rsnrpz_status < VI_SUCCESS )
        rsnrpz_status = rsnrpz_status;
}

/*===========================================================================*/
/* Function: Nrp Device Changed Callback                                     */
/* Purpose:  This function is called when a sensor gets connected or         */
/*           disconnected.                                                     */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "NrpDeviceChangedCallback"
static void _NRP_FUNC NrpDeviceChangedCallback(
//long userArgument
  NRP_USERARG userArgument
    )
{
trace_log( __FUNC__ );

    userArgument = userArgument;

    // call the device change call back function
    if ( pCallbackFn!=0 )
    {
/**
        void(*cb)(void *);
    cb = (void(*)(void *)) pCallbackFn;
        cb( lDeviceChangedUserArgument );
**/
    pCallbackFn( lDeviceChangedUserArgument );
    }
}

/*===========================================================================*/
/* Function: Nrp Data Available Callback                                     */
/* Purpose:  This function is called when data is available.                 */
/*           The function reads the sensor data and stores it in the cache.  */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "NrpDataAvailableCallback"
static void _NRP_FUNC NrpDataAvailableCallback(
    NRP_SESSION  session,
    long         dataType,
    NRP_USERARG userArgument
)
{
    long            rsnrpz_status = NRP_SUCCESS;
    void*           pdynamic_buffer = NULL;
    void*           pdynamic_buffer_aux1 = NULL;
    void*           pdynamic_buffer_aux2 = NULL;
    //ViStatus        error = VI_SUCCESS;
    long            data_type,
                    paramNb,
                    groupNb;
    ViInt32         index = -1;
    SMainSession*   pmain_session = 0;

    SSessionInfo *psession_info = GetSesInfoFromSession(session);
    if( psession_info==0 )
        return;

    if ( userArgument == 0 )
    {
        // suppress 'unused' warning
    }

    trace_log( "%s(session=%ld, type=%ld)", __FUNC__, session, dataType );

    pmain_session = (SMainSession*)(psession_info->MainSession);

    if (WaitForSingleObject(pmain_session->event, 0) != WAIT_OBJECT_0 )
    {
        return;
    }

    EnterCriticalSection(&(psession_info->cs));

    CHECKERR_CU( NrpGetData( session,
                             &data_type,
                             &groupNb,
                             &paramNb));

    rsnrpz_GetIndexFromPacket( groupNb,
                               paramNb,
                              &index);

    switch ( data_type )
    {
    case DATA_BITFIELDFEATURE:
        {
            long    feature[3];

trace_log( "DATA_BITFIELDFEATURE" );

            memset( feature, 0, sizeof(feature));
            CHECKERR_CU( NrpGetBitfieldResult( session,
                                               &feature[0],
                                               &feature[1],
                                               &feature[2]));
            rsnrpz_SetGroupFeature( session,
                                    groupNb,
                                    feature,
                                    3);

            // Evaluation of highest available 'Pulse Measurement' parameter
            if ( groupNb == 16 )
            {
                unsigned long ulFeat = (unsigned long)feature[0];
                if ( ulFeat != 0 )
                {
                    int iBit = 31;
                    while ( ! (ulFeat & 0x80000000) )
                    {
                        iBit--;
                        ulFeat = ulFeat << 1;
                    }

                    psession_info->PulseHighestParamNo = iBit + 1;
                }
            }
        }
        break;
    case DATA_BITFIELDLIMIT:
    case DATA_BITFIELDPARAM:
        {
            long value1, value2, value3;

trace_log( "DATA_BITFIELDLIMIT/PARAM" );

            CHECKERR_CU( NrpGetBitfieldResult( session,
                                               &value1,
                                               &value2,
                                               &value3));
            if ( index >= 0 )
            {
                // sensor always returns 32-bit data
                psession_info->CacheTable[index].current.lvalue     = (ViInt32)value1;
                psession_info->CacheTable[index].type               = DATA_BITFIELDPARAM;
                if ( dataType == DATA_BITFIELDLIMIT )
                {
                    psession_info->CacheTable[index].minimum.lvalue = (ViInt32)value3;
                    psession_info->CacheTable[index].maximum.lvalue = (ViInt32)value2;
                } // if ( dataType == DATA_LONGLIMIT )
            } // if ( index >= 0 )
        }
        break;
    case DATA_FLOATLIMIT:
    case DATA_FLOATPARAM:
        {
            float value1, value2, value3;

trace_log( "DATA_FLOATLIMIT/PARAM" );

            CHECKERR_CU( NrpGetFloatResult( session,
                                            &value1,
                                            &value2,
                                            &value3));
            if ( index >= 0 )
            {
                psession_info->CacheTable[index].current.fvalue     = value1;
                psession_info->CacheTable[index].type               = DATA_FLOATPARAM;
                if ( dataType == DATA_FLOATLIMIT )
                {
                    psession_info->CacheTable[index].minimum.fvalue = value3;
                    psession_info->CacheTable[index].maximum.fvalue = value2;
                } // if ( dataType == DATA_LONGLIMIT )

                if ( (data_type == DATA_FLOATPARAM) && (psession_info->IsPulseMeasurementOn == VI_TRUE) )
                {
                    if ( (groupNb == 16) && (paramNb == psession_info->PulseHighestParamNo) )
                        psession_info->IsPulseScalarsComplete = VI_TRUE;
                }

            } // if ( index >= 0 )
        }
        break;
    case DATA_BINARYBLOCK:
        {
            long size = 0,
                 read_count;

trace_log( "DATA_BINARYBLOCK" );

            CHECKERR_CU( NrpGetBinaryResultLength( session, &size));

            pdynamic_buffer = malloc(sizeof(float)*size);

            CHECKERR_CU( NrpGetBinaryResult( session,
                                           (unsigned char*)pdynamic_buffer,
                                            size,
                                            &read_count));
            /*
             * TODO: Read binary data?
             */

        }
        break;
    case DATA_FLOATARRAY:
        {
            long size = 0,
                 read_count;
            int i;
            float*  psource;
            ViReal64*   pdestination;

trace_log( "DATA_FLOATARRAY" );

            CHECKERR_CU( NrpGetFloatArrayLength( session, &size));

            pdynamic_buffer = malloc(sizeof(float)*size);
            CHECKERR_CU( NrpGetFloatArray( session,
                                           (float*)pdynamic_buffer,
                                            size,
                                            &read_count));

            if ( psession_info->ArrayMeasSize != read_count )
            {
                if (psession_info->ArrayMeas)
                {
                    free( psession_info->ArrayMeas);
                    psession_info->ArrayMeas = NULL;
                    psession_info->ArrayMeasSize = 0;
                }
                psession_info->ArrayMeas = (ViReal64*)malloc(sizeof(ViReal64) * size);
                psession_info->ArrayMeasSize = size;
            }


            pdestination = psession_info->ArrayMeas;
            psource = (float*)pdynamic_buffer;
            for (i = 0; i < size; i++)
                *pdestination++ = (ViReal64)*psource++;
            psession_info->IsArrayMeasValid      = VI_TRUE;
            psession_info->IsMeasurementComplete = VI_TRUE;

            rsnrpz_SetEvent(session, -2, -2);
        }
        break;
    case DATA_AUXFLOATARRAY:
        {
            long size = 0,
                 read_count;
            int i;
            float*  psource;
            ViReal64*   pdestination;

trace_log( "DATA_AUXFLOATARRAY" );

            CHECKERR_CU( NrpGetAuxFloatArrayLength( session, &size));

            pdynamic_buffer = malloc(sizeof(float)*size);
            pdynamic_buffer_aux1 = malloc(sizeof(float)*size);
            pdynamic_buffer_aux2 = malloc(sizeof(float)*size);

            CHECKERR_CU( NrpGetAuxFloatArray( session,
                                           (float*)pdynamic_buffer,
                                           (float*)pdynamic_buffer_aux1,
                                           (float*)pdynamic_buffer_aux2,
                                            size,
                                            &read_count));

            if ( psession_info->ArrayMeasSize != read_count )
            {
                if (psession_info->ArrayMeas)
                {
                    free( psession_info->ArrayMeas);
                    psession_info->ArrayMeas = NULL;
                    psession_info->ArrayMeasSize = 0;
                }
                psession_info->ArrayMeas = (ViReal64*)malloc(sizeof(ViReal64) * size);
                psession_info->ArrayMeasSize = size;

                if (psession_info->ArrayMeasAux1)
                {
                    free( psession_info->ArrayMeasAux1);
                    psession_info->ArrayMeasAux1 = NULL;
                }

                psession_info->ArrayMeasAux1 = (ViReal64*)malloc(sizeof(ViReal64) * size);

                if (psession_info->ArrayMeasAux2)
                {
                    free( psession_info->ArrayMeasAux2);
                    psession_info->ArrayMeasAux2 = NULL;
                }
                psession_info->ArrayMeasAux2 = (ViReal64*)malloc(sizeof(ViReal64) * size);
            }

            pdestination = psession_info->ArrayMeas;
            psource = (float*)pdynamic_buffer;
            for (i = 0; i < size; i++)
                *pdestination++ = (ViReal64)*psource++;

            pdestination = psession_info->ArrayMeasAux1;
            psource = (float*)pdynamic_buffer_aux1;
            for (i = 0; i < size; i++)
                *pdestination++ = (ViReal64)*psource++;

            pdestination = psession_info->ArrayMeasAux2;
            psource = (float*)pdynamic_buffer_aux2;
            for (i = 0; i < size; i++)
                *pdestination++ = (ViReal64)*psource++;
            psession_info->IsArrayMeasAuxValid   = VI_TRUE;
            psession_info->IsArrayMeasValid      = VI_TRUE;
            psession_info->IsMeasurementComplete = VI_TRUE;

            rsnrpz_SetEvent(session, -2, -2);
        }
        break;
    case DATA_FLOATRESULT:
        {
            float value1, value2, value3;

trace_log( "DATA_FLOATRESULT" );

            CHECKERR_CU( NrpGetFloatResult( session,
                                            &value1,
                                            &value2,
                                            &value3));

            psession_info->SingleMeas     = (ViReal64)value1;
            psession_info->SingleMeasAux1 = (ViReal64)value2;
            psession_info->SingleMeasAux2 = (ViReal64)value3;
            psession_info->IsSingleMeasValid     = VI_TRUE;
            psession_info->IsMeasurementComplete = VI_TRUE;

            rsnrpz_SetEvent(session, -1, -1);
        }
        break;

    // DATA_LONGPARAM is always 32 bit value
    case DATA_LONGLIMIT:
    case DATA_LONGPARAM:
        {
            long value1, value2, value3;

trace_log( "DATA_LONGLIMIT/PARAM" );

            CHECKERR_CU( NrpGetLongResult( session,
                                           &value1,
                                           &value2,
                                           &value3));
            if ( index >= 0 )
            {
                psession_info->CacheTable[index].current.lvalue     = (ViInt32)value1;
                if ( dataType == DATA_LONGLIMIT )
                {
                    psession_info->CacheTable[index].type           = DATA_LONGPARAM;
                    psession_info->CacheTable[index].minimum.lvalue = (ViInt32)value3;
                    psession_info->CacheTable[index].maximum.lvalue = (ViInt32)value2;
                } // if ( dataType == DATA_LONGLIMIT )
            } // if ( index >= 0 )
        }
        break;
    case DATA_STRING:

trace_log( "DATA_STRING" );

        rsnrpz_status = NrpGetStringResult( session, NULL, 0);
        if ( rsnrpz_status > 0 )
        {
            ViChar  buffer[BUFFER_SIZE];
            ViChar* pbuffer = buffer;
            int     buffer_size = sizeof( buffer);

            if ( rsnrpz_status > buffer_size )
            {
                pdynamic_buffer = malloc( rsnrpz_status );
                /*
                 * TODO: Check memory allocation
                 */
                pbuffer = (ViChar*)pdynamic_buffer;
                buffer_size = rsnrpz_status;
            }

            rsnrpz_status = NrpGetStringResult( session, pbuffer, buffer_size);

            if ( index >= 0 )
            {
                if ( psession_info->CacheTable[index].type == DATA_BITFIELDPARAM )
                {
                    ViInt32 bit;
                    CHECKERR_CU( rsnrpz_GetBitFromString( gs_NrpCommands[index].RangeTable,
                                                          pbuffer,
                                                          &bit));
                    psession_info->CacheTable[index].current.lvalue = bit;

                }
                else
                {
                    psession_info->CacheTable[index].type = DATA_STRING;
                    if (psession_info->CacheTable[index].current.pstring != NULL)
                        free(psession_info->CacheTable[index].current.pstring);
                    psession_info->CacheTable[index].current.pstring = (char *)malloc(strlen(pbuffer)+1);
                    strcpy(psession_info->CacheTable[index].current.pstring, pbuffer);
                }
            } // if ( index >= 0 )
        }
        CHECKERR_CU( rsnrpz_status);
        break;
    } // switch ( dataType )

Error:
    if ( psession_info != NULL )
        LeaveCriticalSection(&(psession_info->cs));

    if ( pdynamic_buffer )
        free(pdynamic_buffer);
    if ( pdynamic_buffer_aux1 )
        free(pdynamic_buffer_aux1);
    if ( pdynamic_buffer_aux2 )
        free(pdynamic_buffer_aux2);
}

/*===========================================================================*/
/* Function: Nrp Error Occurred Callback                                     */
/* Purpose:  This function is called when sensor reports an error.           */
/*           When an error is occurred, registry should be updated.          */
/*           More information about registry subsystem you can find in NRP   */
/*           manual.                                                         */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "NrpErrorOccurredCallback"
static void _NRP_FUNC NrpErrorOccurredCallback(
//  ViSession   sensorHandle,
    NRP_SESSION sensorHandle,
    long        errorCode,
    NRP_USERARG userArgument
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    SMainSession*   pmain_session = NULL;
    int             bit = 0;
    int             mapped_error = 0;
    char            error_message[256];
    int             channel;
    ViBoolean       is_locked = VI_FALSE;
    ViBoolean       set_sensor_power        = VI_FALSE;
    ViBoolean       set_sensor_calibartion  = VI_FALSE;
    ViBoolean       set_sensor_zero         = VI_FALSE;

    if ( userArgument == 0 )
    {
        // suppress 'unused' warning
    }

trace_log( "CB ErrOcc code=0x%08x", errorCode );

    if ( sensorHandle == 0 )
    {
        /**
         * USB Layer error
         */
        return;
    }
    else
    {

        SSessionInfo *psession_info = GetSesInfoFromSession(sensorHandle);
        if( psession_info==0 )
            return;

        pmain_session = (SMainSession*)(psession_info->MainSession);

        for (channel = 0; channel < RSNRPZ_MAX_CHANNELS &&
                          sensorHandle != pmain_session->pSessions[channel];
             channel++)
            ; /* Do nothing */

        if (channel == RSNRPZ_MAX_CHANNELS)
            return;
        else
            channel++;

        if (WaitForSingleObject(pmain_session->event, 0) != WAIT_OBJECT_0 )
            return;

        switch ( (unsigned long) errorCode )
        {
            case NRP_ERROR_DEVICE_CALDATA_FORMAT:
            case NRP_ERROR_DEVICE_CHECKSUM:
            case NRP_ERROR_DEVICE_ILLEGALSERIAL:
            case NRP_ERROR_DEVICE_FLASHPROG:
            case NRP_ERROR_DEVICE_RESULT_QUESTIONABLE:
                mapped_error = -200;
                break;
            case NRP_ERROR_DEVICE_OVERRANGE:
                mapped_error = 52;
                set_sensor_power = VI_TRUE;
                break;
            case NRP_ERROR_DEVICE_NOTINSERVICEMODE:
                mapped_error = -203;
                break;
            case NRP_ERROR_DEVICE_CALZERO:
                mapped_error = -341;
                set_sensor_zero = VI_TRUE;
                break;
            case NRP_ERROR_DEVICE_TRIGGERQUEUEFULL:
            case NRP_ERROR_DEVICE_EVENTQUEUEFULL:
                set_sensor_power = VI_TRUE;
            case NRP_ERROR_DEVICE_BURST_TOO_SHORT:
                set_sensor_power = VI_TRUE;
            case NRP_ERROR_DEVICE_OUTBUFFERFULL:
                mapped_error = -230;
                break;
    #if defined(_DEBUG)
            case NRP_ERROR_DEVICE_SAMPLEERROR:
                mapped_error = 54;
                break;
    #endif
            case NRP_ERROR_DEVICE_OVERLOAD:
                mapped_error = 51;
                set_sensor_power = VI_TRUE;
                break;
            case NRP_ERROR_DEVICE_HARDWARE:
                mapped_error = 55;
                set_sensor_power = VI_TRUE;
                break;
            case NRP_ERROR_DEVICE_FILTERTRUNCATED:
                mapped_error = 56;
                break;
            case NRP_ERROR_DEVICE_GENERIC:
                mapped_error = -300;
                break;
            case NRP_ERROR_DEVICE_OVERMAX:
            case NRP_ERROR_DEVICE_UNDERMIN:
                mapped_error = -220;
                break;
            case NRP_ERROR_DEVICE_VOLTAGE:
                mapped_error = -310;
                break;
            case NRP_ERROR_DEVICE_SYNTAX:
                mapped_error = -102;
                break;
            case NRP_ERROR_DEVICE_MEMORY:
                mapped_error = -311;
                break;
            case NRP_ERROR_DEVICE_PARAMETER:
            case NRP_ERROR_DEVICE_UNKNOWNCOMMAND:
                mapped_error = -100;
                break;
            case NRP_ERROR_DEVICE_NOTIDLE:
                mapped_error = 28;
                break;
            case NRP_ERROR_DEVICE_CALDATANOTPRESENT:
                set_sensor_calibartion = VI_TRUE;
                mapped_error = -340;
                break;
            case NRP_ERROR_DEVICE_TIMING:
            default:
                mapped_error = 0;
                break;
        }

        if ( mapped_error )
        {
            if ((mapped_error <= -100 ) && (mapped_error >= -199))
                bit = NRP_ESR_REGISTER_COMMAND_ERROR;
            else
            if ((mapped_error <= -200 ) && (mapped_error >= -299))
                bit = NRP_ESR_REGISTER_EXECUTION_ERROR;
            else
            if ((mapped_error <= -300 ) && (mapped_error >= -399))
                bit = NRP_ESR_REGISTER_DEV_DEPENDENT_ERROR;
            else
                bit = NRP_ESR_REGISTER_DEV_DEPENDENT_ERROR;

            CHECKERR_CU( NrpGetErrorText(errorCode, error_message, sizeof(error_message)));

            if ( errorCode != (long)NRP_ERROR_DEVICE_RESULT_QUESTIONABLE )
            {
                CHECKERR_CU( rsnrpz_PutError( N2V( pmain_session ), mapped_error, error_message));
            }
            else
            {
                CHECKERR_CU( rsnrpz_PutError( N2V( pmain_session ), errorCode, error_message));
            }

            is_locked = VI_TRUE;
            trace_log( "%s : EnterCriticalSection()", __FUNC__ );
            EnterCriticalSection( &(pmain_session->cs) );

            CHECKERR_CU( rsnrpz_SetBitInRegisters( pmain_session,
                                                   STANDARD_EVENT_STATUS_REGISTER,
                                                   CONDITION_REGISTER,
                                                   bit,
                                                   VI_TRUE
                                                 ));
            CHECKERR_CU( rsnrpz_SetBitInRegisters( pmain_session,
                                                   STANDARD_EVENT_STATUS_REGISTER,
                                                   CONDITION_REGISTER,
                                                   bit,
                                                   VI_FALSE
                                                 ));

            CHECKERR_CU( rsnrpz_SetBitInRegisters( pmain_session,
                                                   DEVICE_STATUS_REGISTER_ERROR,
                                                   CONDITION_REGISTER,
                                                   channel,
                                                   VI_TRUE
                                                 ));

            CHECKERR_CU( rsnrpz_SetBitInRegisters( pmain_session,
                                                   DEVICE_STATUS_REGISTER_ERROR,
                                                   CONDITION_REGISTER,
                                                   channel,
                                                   VI_FALSE
                                                 ));

            if ( set_sensor_power )
            {
                CHECKERR_CU( rsnrpz_SetBitInRegisters( pmain_session,
                                                   QUESTIONABLE_POWER_STATUS_REGISTER,
                                                   CONDITION_REGISTER,
                                                   channel,
                                                   VI_TRUE
                                                 ));
                CHECKERR_CU( rsnrpz_SetBitInRegisters( pmain_session,
                                                   QUESTIONABLE_POWER_STATUS_REGISTER,
                                                   CONDITION_REGISTER,
                                                   channel,
                                                   VI_FALSE
                                                 ));
            }

            if ( set_sensor_calibartion )
            {
                CHECKERR_CU( rsnrpz_SetBitInRegisters( pmain_session,
                                                   QUESTIONABLE_CALIBRATING_STATUS_REGISTER,
                                                   CONDITION_REGISTER,
                                                   channel,
                                                   VI_TRUE
                                                 ));
                CHECKERR_CU( rsnrpz_SetBitInRegisters( pmain_session,
                                                   QUESTIONABLE_CALIBRATING_STATUS_REGISTER,
                                                   CONDITION_REGISTER,
                                                   channel,
                                                   VI_FALSE
                                                 ));
            }

            if ( set_sensor_zero )
            {
                CHECKERR_CU( rsnrpz_SetBitInRegisters( pmain_session,
                                                       QUESTIONABLE_ZERO_STATUS_REGISTER,
                                                       CONDITION_REGISTER,
                                                       channel,
                                                       VI_TRUE
                                                     ));
            }
        }
    }
Error:
    if ( is_locked && pmain_session )
    {
        trace_log( "%s : LeaveCriticalSection()", __FUNC__ );
        LeaveCriticalSection(&(pmain_session->cs));
    }
}

/*===========================================================================*/
/* Function: Nrp State Changed Callback                                      */
/* Purpose:  This function is called when sensor change trigger state.       */
/*           All these changes (idle - wait for trigger - measuring) should  */
/*           change registry subsystem.                                       */
/*           More information about registry subsystem you can find in NRP   */
/*           manual.                                                         */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "NrpStateChangedCallback"
static void _NRP_FUNC NrpStateChangedCallback(
    NRP_SESSION sensorHandle,
    long        sensorTriggerState,
    NRP_USERARG userArgument
)
{
    ViStatus        rsnrpz_status = VI_SUCCESS;
    SMainSession*   pmain_session;
    int             index;
    ViBoolean       is_locked = VI_FALSE;

    SSessionInfo *psession_info = GetSesInfoFromSession(sensorHandle);
    if( psession_info==0 )
        return;

    if ( userArgument == 0 )
    {
        // suppress 'unused' warning
    }

trace_log( "CB StateChange state=%d", sensorTriggerState );

    pmain_session = (SMainSession*)(psession_info->MainSession);

    if (WaitForSingleObject(pmain_session->event, 0) != WAIT_OBJECT_0 )
        return;

    for (index = 0; index < RSNRPZ_MAX_CHANNELS; index++)
    {
        if ( pmain_session->pSessions[index] == sensorHandle )
            break;
    }

    if ( index < RSNRPZ_MAX_CHANNELS)
    {
        ViBoolean   measure_state = VI_FALSE;
        ViBoolean   trigger_state = VI_FALSE;

        index++;  /* channels are is 1-based */

        switch ( sensorTriggerState )
        {
        case NRP_TRIGGER_WAIT_FOR_TRIGGER:
            trigger_state = VI_TRUE;
            break;
        case NRP_TRIGGER_MEASURING:
            measure_state = VI_TRUE;
            break;
        case NRP_TRIGGER_IDLE:
        default:  /* NRP_TRIGGER_UNKNOWN  */
                  /* NRP_TRIGGER_RESERVED */
            measure_state = VI_FALSE;
            break;
        }

        is_locked = VI_TRUE;
        EnterCriticalSection(&(psession_info->cs));

        CHECKERR_CU( rsnrpz_SetBitInRegisters(  pmain_session,
                                                OPERATION_MEASURING_STATUS_REGISTER,
                                                CONDITION_REGISTER,
                                                index,
                                                measure_state
                                              ));
        CHECKERR_CU( rsnrpz_SetBitInRegisters(  pmain_session,
                                                OPERATION_TRIGGER_STATUS_REGISTER,
                                                CONDITION_REGISTER,
                                                index,
                                                trigger_state
                                              ));
    }

Error:
    if ( is_locked )
    {
        LeaveCriticalSection(&(psession_info->cs));
    }
}



/*===========================================================================*/
/* Function: rsnrpz_GetAvailableSensors                                      */
/* Purpose:  This function returns the currently connected sensors.          */
/*===========================================================================*/
#undef  __FUNC__
#define __FUNC__ "rsnrpz_GetAvailableSensors"
static ViStatus rsnrpz_GetAvailableSensors(
    SensorDetail sensor[],
    ViInt32 *size )
{
    long            lNumDevices = 0;
    long            lNrpStatus  = NRP_SUCCESS;
    long            lCnt, lDest;
    long            lConnected;
    SensorDetail    sd;
    ViBoolean       boAlreadyOpen;

    boAlreadyOpen = boDriverIsOpen;

    if ( boDriverIsOpen == VI_FALSE )
    {
        lNrpStatus = NrpOpenDriver();
        boDriverIsOpen = VI_TRUE;
    }

    lNrpStatus = NrpLockDeviceList();
    if ( lNrpStatus != NRP_SUCCESS )
    {
        if ( boAlreadyOpen == VI_FALSE )
        {
            // close driver if no device change call back is registered
            if( pCallbackFn == 0 )
            {
                NrpCloseDriver();
                boDriverIsOpen = VI_FALSE;
            }
        }
        return lNrpStatus;
    }

    lNrpStatus = NrpGetDeviceListLength( &lNumDevices );
    if ( lNrpStatus != NRP_SUCCESS )
    {
        if ( boAlreadyOpen == VI_FALSE )
        {
            // close driver if no device change call back is registered
            if( pCallbackFn == 0 )
            {
                NrpCloseDriver();
                boDriverIsOpen = VI_FALSE;
            }
        }
        return lNrpStatus;
    }

    lDest = 0;
    for ( lCnt = 0; lCnt < lNumDevices; lCnt++ )
    {
        memset( &sd, 0x00, sizeof( SensorDetail ) );
        NrpGetDeviceInfo( lCnt,
                          sd.szLogicalName,
                          sd.szSensorType,
                          sd.szSerial,
                         &lConnected
                        );

        if ( lConnected && (lDest < *size) )
        {
            memcpy( &sensor[ lDest ], &sd, sizeof( SensorDetail ) );
            lDest++;
        }
    }

    *size = lDest;

    if ( boAlreadyOpen == VI_FALSE )
    {
        // close driver if no device change call back is registered
        if( pCallbackFn == 0 )
        {
            NrpCloseDriver();
            boDriverIsOpen = VI_FALSE;
        }
    }
    return lNrpStatus;
}


/*===========================================================================*/
/* Function: rsnrpz_GetSensorCount                                           */
/* Purpose:  This function returns the number of currently connected sensors */
/*===========================================================================*/
ViStatus _VI_FUNC rsnrpz_GetSensorCount( ViSession iDummyHandle, ViInt32 *piCount )
{
    iDummyHandle = iDummyHandle;    // suppress compiler warnings

    if ( pAvailableSensor )
        free( pAvailableSensor );
    pAvailableSensor = 0;


    pAvailableSensor = (SensorDetail *)calloc( RSNRPZ_MAX_CHANNELS, sizeof( SensorDetail) );

    if ( ! pAvailableSensor )
    {
        if ( piCount )
            *piCount = 0;

        return RSNRPZ_ERROR_INVALID_VALUE;
    }

    iAvailableSensors = RSNRPZ_MAX_CHANNELS;
    rsnrpz_GetAvailableSensors( pAvailableSensor, &iAvailableSensors );

    if ( piCount )
        *piCount = iAvailableSensors;

    return NRP_SUCCESS;

}


/*===========================================================================*/
/* Function: rsnrpz_GetSensorInfo                                            */
/* Purpose:  This function returns the name/descriptor of a connected sensor */
/*                                                                           */
/*           The char arrays pointed to by pszSensorName, pszSensorType and  */
/*           pszSensorSerial should provide room for 128 characters          */
/*===========================================================================*/
ViStatus _VI_FUNC rsnrpz_GetSensorInfo(
                      ViSession      iDummyHandle,
                      ViInt32        iChannel,
                      ViChar _VI_FAR pszSensorName[],
                      ViChar _VI_FAR pszSensorType[],
                      ViChar _VI_FAR pszSensorSerial[]
                      )
{
    if ( ! pAvailableSensor )
        rsnrpz_GetSensorCount( iDummyHandle, 0 );   // sets pAvailableSensor
                                                    //      iAvailableSensors

    if ( ! pAvailableSensor )
        return RSNRPZ_ERROR_INVALID_VALUE;

    if ( (1 > iChannel) || (iChannel > iAvailableSensors) )
        return VI_ERROR_PARAMETER2;

    if ( pszSensorName )
        strcpy( pszSensorName, pAvailableSensor[ iChannel-1 ].szLogicalName );

    if ( pszSensorType )
        strcpy( pszSensorType, pAvailableSensor[ iChannel-1 ].szSensorType );

    if ( pszSensorSerial )
        strcpy( pszSensorSerial, pAvailableSensor[ iChannel-1 ].szSerial );

    return NRP_SUCCESS;
}


/*****************************************************************************/
/* Support of R&S USB Hub NRP-Z5                                             */
/*****************************************************************************/

/*===========================================================================*/
/* Function: rsnrpz_GetDeviceStatusZ5                                        */
/* Purpose:  This functions provides information about whether an  NRP-Z5    */
/*           is connected to the PC.                                         */
/* Returns:  NRP_SUCCESS                    if an NRP-Z5 was found           */
/*           NRP_ERROR_DEVICE_DISCONNECTED  No NRP-Z5 is connected           */
/*===========================================================================*/
ViStatus _VI_FUNC rsnrpz_GetDeviceStatusZ5( ViSession iDummyHandle, ViInt32 *piAvail )
{
#if defined(__linux__)
  ViBoolean       boAlreadyOpen;
#endif

  ViStatus lStatus = NRP_SUCCESS;

  iDummyHandle = iDummyHandle;  // suppress warning

  if ( piAvail )
    *piAvail = 0;


#if defined(__linux__)
  boAlreadyOpen = boDriverIsOpen;

  if ( boDriverIsOpen == VI_FALSE )
  {
    NrpOpenDriver();
    boDriverIsOpen = VI_TRUE;
  }
#endif


  lStatus = NrpGetDeviceStatusZ5();


#if defined(__linux__)

  if ( boAlreadyOpen == VI_FALSE )
  {
    // close driver if no device change call back is registered
    if( pCallbackFn == 0 )
    {
      NrpCloseDriver();
      boDriverIsOpen = VI_FALSE;
    }
  }
#endif


  if ( lStatus == NRP_SUCCESS )
    if ( piAvail )
      *piAvail = 1;

  return lStatus;
}


/*===========================================================================*/
/* Function: rsnrpz_GetDeviceInfoZ5                                          */
/* Purpose:  If the above status function (= NrpGetDeviceStatusZ5() )        */
/*           indicates that there is an NRP-Z5, this function supplies       */
/*           information about the connected devices at its ports A...D      */
/*           (using 'iPortIdx' = 0...3)                                      */
/* Returns:  NRP_SUCCESS                 if an NRP-Z5 was found and          */
/*                                       'iPortIdx' is in the range [0..3]   */
/*           NRP_ERROR_DESCRIPTOR_INDEX  if no NRP-Z5 is connected or        */
/*                                       'iPortIdx' is not within [0..3]     */
/*                                                                           */
/*           The char arrays pointed to by pszSensorName, pszSensorType and  */
/*           pszSensorSerial should provide room for 128 characters          */
/*===========================================================================*/
ViStatus _VI_FUNC rsnrpz_GetDeviceInfoZ5( ViSession         iDummyHandle,
                                          ViInt32           iPortIdx,
                                          ViChar _VI_FAR    pszSensorName[],
                                          ViChar _VI_FAR    pszSensorType[],
                                          ViChar _VI_FAR    pszSensorSerial[],
                                          ViBoolean _VI_FAR *pbConnected )
{
  long      lConnected    = 0;
  ViStatus  lStatus       = NRP_SUCCESS;
  
#if defined(__linux__)
  ViBoolean boAlreadyOpen = boDriverIsOpen;
#endif

  if ( pszSensorName )
    *pszSensorName = 0;
  if ( pszSensorType )
    *pszSensorType = 0;
  if ( pszSensorSerial )
    *pszSensorSerial = 0;
  if ( pbConnected )
    *pbConnected = VI_FALSE;

  lStatus = rsnrpz_GetDeviceStatusZ5( iDummyHandle, 0 );
  if ( lStatus != NRP_SUCCESS )
  {
    // No NRP-Z5
    return lStatus;
  }

    
#if defined(__linux__)
  if ( boDriverIsOpen == VI_FALSE )
  {
    NrpOpenDriver();
    boDriverIsOpen = VI_TRUE;
  }
#endif

  lStatus = NrpGetDeviceInfoZ5( iPortIdx, pszSensorName, pszSensorType, pszSensorSerial, &lConnected );
  if ( (lStatus == NRP_SUCCESS) && pbConnected )
    *pbConnected = (lConnected == 0) ? VI_FALSE : VI_TRUE;

#if defined(__linux__)

  if ( boAlreadyOpen == VI_FALSE )
  {
    // close driver if no device change call back is registered
    if( pCallbackFn == 0 )
    {
      NrpCloseDriver();
      boDriverIsOpen = VI_FALSE;
    }
  }
#endif
    
    
  return lStatus;
}

/*===========================================================================*/
/* Function: rsnrpz_initZ5                                                   */
/* Purpose:  Open a dedicated sensor on Port 'A', 'B', 'C' or 'D' of         */
/*           an NRP-Z5. Port can be given as [0...3] or ['A'...'D']          */
/* Returns:  NRP_SUCCESS                 if an NRP-Z5 was found and 'cPort'  */
/*                                       is in the range [0..3]/['A'..'D']   */
/*                                       and a power sensor is physically    */
/*                                       connected to the selected channel   */
/*                                       and could have been opened.         */
/*                                       The variable pointed to by          */
/*                                       'pInstrSession' then contains the   */
/*                                       session ID of the opened sensor     */
/*           NRP_ERROR_INVALID_RESOURCE  if at least one of the above pre-   */
/*                                       requisites is not met               */
/*===========================================================================*/
ViStatus _VI_FUNC rsnrpz_initZ5( ViInt32     cPort,
                                 ViBoolean   IDQuery,
                                 ViBoolean   resetDevice,
                                 ViSession*  pInstrSession )
{
  ViChar    szResource[128];
  ViBoolean bConnected = VI_FALSE;
  ViStatus  lStatus    = NRP_SUCCESS;

  if ( (0x00 <= cPort) && (cPort <= 3) )
    cPort += 'A';

  cPort = toupper( cPort );

  if ( (cPort < 'A') || ('D' < cPort) )
    return NRP_ERROR_INVALID_RESOURCE;

  lStatus = rsnrpz_GetDeviceInfoZ5( 0,              // dummy
                                    (cPort - 'A'),  // [0..3]
                                    szResource,     // Resource Name
                                    0,              // not requesting Type
                                    0,              // nor requesting S/N
                                    &bConnected );

  if ( lStatus != NRP_SUCCESS )
    return lStatus;

  if ( bConnected != VI_TRUE )
    return NRP_ERROR_DEVICE_DISCONNECTED;

  return rsnrpz_private_setup( (ViRsrc)szResource, IDQuery, resetDevice, pInstrSession, VI_FALSE );
}




/*===========================================================================*/
/* Function: rsnrpz_status_driverOpenState                                   */
/* Purpose:  This function returns the Nrp low level driver state            */
/*===========================================================================*/
ViStatus _VI_FUNC rsnrpz_status_driverOpenState( ViBoolean* driverState )
{
  if ( driverState )
        *driverState = boDriverIsOpen;

    return NRP_SUCCESS;
}



/*===========================================================================*/
/* Function: Get parameter value and its range from driver cache             */
/* Purpose:  This function returns the current value of a parameter and its  */
/*           limits from the driver cache. The command must be provided as   */
/*           SCPI command, e.g. 'TRIG:LEV'.                                  */
/* Not supported for:                                                        */
/*===========================================================================*/
ViStatus _VI_FUNC rsnrpz_chan_getCacheRange( ViSession   vi,
                                             ViInt32     channel,
                                             const char *pszCommand,
                                             ViReal64   *pdCurrent,
                                             ViReal64   *pdMin,
                                             ViReal64   *pdMax )
{
    ViInt32         index;
    ViStatus        rsnrpz_status = NRP_SUCCESS;
    SMainSession*   pmain_session = V2N( vi );
    int                iValidData    = 0;

    if( pmain_session==0 )
        return VI_ERROR_PARAMETER1;

    // check if channel is in the correct range
    CHECKCHANNEL( vi, channel );

    {
        // get session_info object for channel
        SSessionInfo *psession_info = pmain_session->pSInfos[channel-1];

        // check input pointers
        if( pszCommand==0 || pdCurrent==0 || pdMin==0 || pdMax==0 )
            return VI_ERROR_PARAMETER3;

        // check if the cache table is valid
        if( psession_info->CacheTable == 0 )
            return _VI_ERROR;

        // get index for command
        CHECKERR_CU( rsnrpz_GetIndexFromCommand( (char*)pszCommand, &index) );

        EnterCriticalSection( &( psession_info->cs ) );

        // is the value available ?
        if( psession_info->CacheTable[index].IsAvailable!=1 )
        {
            LeaveCriticalSection( &( psession_info->cs ) );
            return _VI_ERROR;
        }

        // see NrpDataAvailableCallback() in this file
        if( psession_info->CacheTable[index].type==DATA_FLOATPARAM )
        {
            *pdCurrent = (double)(psession_info->CacheTable[index].current.fvalue);
            *pdMin     = (double)(psession_info->CacheTable[index].minimum.fvalue);
            *pdMax     = (double)(psession_info->CacheTable[index].maximum.fvalue);
            iValidData = 1;
        }

        // DATA_LONGPARAM is always 32 bit value
        if( psession_info->CacheTable[index].type==DATA_LONGPARAM )
        {
            *pdCurrent = (double)(psession_info->CacheTable[index].current.lvalue);
            *pdMin     = (double)(psession_info->CacheTable[index].minimum.lvalue);
            *pdMax     = (double)(psession_info->CacheTable[index].maximum.lvalue);
            iValidData = 1;
        }

        LeaveCriticalSection( &( psession_info->cs ) );

        // no valid data format
        if( iValidData==0 )
            return _VI_ERROR;

    }

Error:
    return rsnrpz_status;
}


#if defined (_OUTSIDEVEE_)
/*===========================================================================*/
/* Function: rsnrpz_GetSessionControlHandle                                  */
/* Purpose:  This function returns the corresponding session ID of the       */
/* underlying NrpControl2                                                    */
/* (This is for internal use in NrpTestEngine only)                          */
/*===========================================================================*/
ViStatus _VI_FUNC rsnrpz_GetSessionControlHandle(
  ViSession      iHandle,
  ViInt32        iChannel,
  ViSession    **pCtrlSession
  )
{
  SMainSession *pmain_session;

  if ( ! iHandle )
    return VI_ERROR_PARAMETER1;

  if ( iChannel <= 0 || iChannel > RSNRPZ_MAX_CHANNELS )
    return RSNRPZ_ERROR_INCORRECT_CHANNEL;

  // get associated session and check
  pmain_session = V2N( iHandle );
  if( pmain_session==0 )
    return VI_ERROR_PARAMETER1;

  if ( pCtrlSession )
    *pCtrlSession = (ViSession *)(pmain_session->pSessions[iChannel-1]);

  return NRP_SUCCESS;
}


/*===========================================================================*/
/* Function: Set parameters for device changed callback                      */
/* Purpose:  This function sets the parameters for the device change callback*/
/* Not supported for: VEE                                                    */
/*===========================================================================*/
ViStatus _VI_FUNC rsnrpz_status_setDeviceChangedCallback(
            void (*pFn)(NRP_SESSION),
            NRP_USERARG lUserArgument
            )
{
  ViStatus  rsnrpz_status = NRP_SUCCESS;
  ViBoolean bLocked       = VI_FALSE;

  // The very first call initializes the critical section
  MakeSureCSInitialized();

  // OpenDriver() MUST NOT be called from more than one thread simultaneously
  // We need a variable to know if unlock is necessary in case of error
  EnterCriticalSection( &cs );
  bLocked = VI_TRUE;

  // open driver if this is required
  if ( ( boDriverIsOpen == VI_FALSE ) && ( pFn != 0 ) )
  {
    CHECKERR_CU( NrpOpenDriver());
    boDriverIsOpen = VI_TRUE;
  }

  // we either delete the internal call back or set it
  if( pFn == 0 )
  {
    CHECKERR_CU( NrpSetNotifyCallbackDeviceChanged( 0,  0 ) );
  }
  else
  {
    CHECKERR_CU( NrpSetNotifyCallbackDeviceChanged( NrpDeviceChangedCallback,  0 ) );
  }

  // memorize the application callback
//pCallbackFn = (void*) pFn;
  pCallbackFn = pFn;
  lDeviceChangedUserArgument = lUserArgument;

  // no open sessions left
//if ( ( pAllSessions == 0 ) && ( pFn == 0 ) )
  if ( ( NumSessionNodes() == 0 ) && ( pFn == 0 ) )
  {
    NrpCloseDriver();
    boDriverIsOpen = VI_FALSE;
  }

  // From now on the initialization can continue
  // in parallel threads
  bLocked = VI_FALSE;
  LeaveCriticalSection( &cs );

  return NRP_SUCCESS;

Error:
  if ( bLocked )
    LeaveCriticalSection( &cs );

  return rsnrpz_status;
}
#endif



/*===========================================================================*/
/* Function: Returns information about the current and the                   */
/*           required minimal revision of the instrument firmware            */
/* Purpose:                                                                  */
/* Not supported for: VEE                                                    */
/*===========================================================================*/
static SMinimalFirmware MinFw[] =
{
  { "FSH-Z1",    "0.0"  },
  { "FSH-Z18",   "0.0"  },
  { "NRP-Z11",   "4.08" },
  { "NRP-Z21",   "4.08" },
  { "NRP-Z22",   "4.08" },
  { "NRP-Z23",   "4.08" },
  { "NRP-Z24",   "4.08" },
  { "NRP-Z51",   "4.08" },
  { "NRP-Z52",   "4.08" },
  { "NRP-Z55",   "4.08" },
  { "NRP-Z56",   "4.08" },
  { "NRP-Z57",   "4.08" },
  { "NRP-Z58",   "4.08" },
  { "NRP-Z91",   "4.08" },
  { "NRP-Z92",   "4.08" },
  { "NRP-Z96",   "4.08" },
  { "NRP-Z98",   "4.08" },
  { "NRP-Z31",   "4.08" },
  { "NRPZ311",   "4.08" },
  { "NRP-Z37",   "4.08" },
  { "NRP-Z27",   "4.08" },
  { "NRP-Z28",   "4.08" },
  { "NRP-Z81",   "1.18" },
  { "NRP-Z85",   "1.20" },
  { "NRP-Z86",   "1.20" },
  { "NRP-Z211",  "4.08" },
  { "NRP-Z221",  "4.08" },
  { "NRPC18",    "4.08" },
  { "NRPC33",    "4.08" },
  { "NRPC40",    "4.08" },
  { "NRPC50",    "4.08" },
  { "NRPC18-B1", "4.08" },
  { "NRPC33-B1", "4.08" },
  { "NRPC40-B1", "4.08" },
  { "NRPC50-B1", "4.08" },
  { 0,           0,     }
};

static void SplitVersion( const char *cpVersion, int *piMajor, int *piMinor, int *piSub )
{
  const char *cpPart = 0;

  if ( piMajor )
    *piMajor = 0;

  if ( piMinor )
    *piMinor = 0;

  if ( piSub )
    *piSub = 0;

  if ( ! cpVersion || ! *cpVersion )
    return;

  cpPart = cpVersion;
  if ( piMajor )
    *piMajor = atoi( cpPart );

  cpPart = strchr( cpPart, '.' );
  if ( cpPart )
  {
    cpPart++;
    if ( piMinor )
      *piMinor = atoi( cpPart );

    cpPart = strchr( cpPart, '.' );
    if ( cpPart )
    {
      cpPart++;
      if ( piSub )
        *piSub = atoi( cpPart );
    }
  }
}

ViStatus _VI_FUNC rsnrpz_fw_version_check( ViSession      vi,
                                           ViInt32        iBufSize,
                                           ViChar _VI_FAR firmwareCurrent[],
                                           ViChar _VI_FAR firmwareRequiredMinimum[],
                                           ViBoolean     *pbFirmwareOkay )
{
  ViStatus          rsnrpz_status = NRP_SUCCESS;
  SMinimalFirmware *pMinFw = MinFw;
  ViChar            bufferType[20];
  ViChar            bufferSwBuild[20];
  int               iCurrMajor;
  int               iCurrMinor;
  int               iCurrSub;
  int               iRequiredMajor;
  int               iRequiredMinor;
  int               iRequiredSub;

  if ( iBufSize < 16 )
    return VI_ERROR_PARAMETER2;

/* Test...
  SplitVersion( "04.08",     &iCurrMajor, &iCurrMinor, &iCurrSub );
  SplitVersion( "1.32",      &iCurrMajor, &iCurrMinor, &iCurrSub );
  SplitVersion( "1.32a",     &iCurrMajor, &iCurrMinor, &iCurrSub );
  SplitVersion( "1.32a.114", &iCurrMajor, &iCurrMinor, &iCurrSub );
  SplitVersion( "1.20.2",    &iCurrMajor, &iCurrMinor, &iCurrSub );
  SplitVersion( "3.96.22",   &iCurrMajor, &iCurrMinor, &iCurrSub );
*/

  memset( bufferType,              0, sizeof( bufferType    ) );
  memset( bufferSwBuild,           0, sizeof( bufferSwBuild ) );
  memset( firmwareCurrent,         0, iBufSize );
  memset( firmwareRequiredMinimum, 0, iBufSize );
  if ( pbFirmwareOkay )
    *pbFirmwareOkay = 0;

  rsnrpz_status = rsnrpz_chan_info( vi, 1, (ViString)"Type", sizeof(bufferType), bufferType );
  if ( rsnrpz_status != NRP_SUCCESS )
    return VI_ERROR_FAIL_ID_QUERY;

  if ( ! strlen( bufferType ) )
    return VI_ERROR_FAIL_ID_QUERY;  /* NRP_ERROR_INVALID_SENSOR_TYPE; */


  rsnrpz_status = rsnrpz_chan_info( vi, 1, (ViString)"SW Build", sizeof(bufferSwBuild), bufferSwBuild );
  if ( rsnrpz_status != NRP_SUCCESS )
    return rsnrpz_status;


  if ( iBufSize < (int)strlen( bufferType ) )
    bufferType[ iBufSize - 1 ] = 0x00;

  if ( iBufSize < (int)strlen( bufferSwBuild ) )
    bufferSwBuild[ iBufSize - 1 ] = 0x00;

  strcpy( firmwareCurrent, bufferSwBuild );
  SplitVersion( bufferSwBuild, &iCurrMajor, &iCurrMinor, &iCurrSub );

  // Search Sensor-Type in our local list and check versions...
  while ( pMinFw && pMinFw->szType && pMinFw->szMinFwRequired )
  {
    if ( ! STRICMP( pMinFw->szType, bufferType ) )
    {
      strcpy( firmwareRequiredMinimum, pMinFw->szMinFwRequired );
      SplitVersion( firmwareRequiredMinimum, &iRequiredMajor, &iRequiredMinor, &iRequiredSub );

      if ( pbFirmwareOkay )
      {
        // Check Major Versions...
        if ( iCurrMajor > iRequiredMajor )
        {
          *pbFirmwareOkay = 1;
          break;
        }

        if ( iCurrMajor < iRequiredMajor )
        {
          *pbFirmwareOkay = 0;
          break;
        }

        // Majors are equal! --> Check Minor Versions...
        if ( iCurrMinor > iRequiredMinor )
        {
          *pbFirmwareOkay = 1;
          break;
        }

        if ( iCurrMinor < iRequiredMinor )
        {
          *pbFirmwareOkay = 0;
          break;
        }

        // Majors & Minors are equal! --> Check Sub Versions...
        if ( iCurrSub >= iRequiredSub )
        {
          *pbFirmwareOkay = 1;
          break;
        }

        *pbFirmwareOkay = 0;

      } // pFirmwareOkay != NULL

      break;

    }

    pMinFw++;
  }

  if ( ! pMinFw || ! pMinFw->szType )
    return VI_ERROR_FAIL_ID_QUERY;  // Unknown sensor type; not in our list

  return NRP_SUCCESS;
}



/*****************************************************************************/
/*=== END INSTRUMENT DRIVER SOURCE CODE =====================================*/
/*****************************************************************************/
