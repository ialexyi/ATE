README.TXT
Rohde&Schwarz NRP-Z series of Power Sensors
Instrument Driver Driver Revision Rev 02.31, 01/2013.

Contents:

1    Introduction

2    NRP-Z Specific Information
2.1    Instrument Addresses
2.2    Sensor Identification and Logical Names
2.3    Callbacks and Window Messages

3    Using the NRP-Z Driver in Application Development Environments
3.1    Microsoft Visual C++ and Borland C++
3.2    Microsoft Visual Basic
3.3    HP VEE
3.4    National Instruments LabWindows/CVI

4.0  Known Defects

5.0  Driver Limitation

6.0  Revision History

7.0  Files Installed

- --------------------------------------------------------------------------

1.   Introduction

This driver is completely or partially compatible with entire family of
Rohde&Schwarz NRP-Z Power Sensor.

The help file included with the Rohde&Schwarz NRP-Z driver contains
programming information. This REAME file contains additional information
for the user/installer.

The Rohde&Schwarz NRP-Z Windows 98(or ME) and Windows 2000/XP drivers are
actually a single 32-bit driver.  All references which follow assume Windows 98.
For Windows 2000 or XP, change win95 references to winnt.

1.  Conformance with the VXIplug&play standard. The only exceptions are that
    it does not have a soft front panel.

2.  It includes a "Function Panel" (.fp) file which allows it to be used
    with visual programming environments such as HP-VEE, LabWindows, and
    LabVIEW.

3.  It includes a comprehensive on-line help file which complements the
    instrument manual. The help file presents detailed documentation
    of each function.

4.  Source is included so that the driver can be modified if desired.

5.  It includes a Visual Basic include file (.bas) which contains the function
    calls in Visual Basic syntax, so that driver functions can be called from
    Visual Basic. If you use Visual Basic with this driver, you should be
    familiar with C/C++ function declarations. In particular, care must be
    taken when working with C/C++ pointers.

- --------------------------------------------------------------------------

2.    NRP-Z Specific Information

The driver required NrpToolkit and VISA installed on your computer. Driver does
not use VISA library, but it is installed to VXIpnp directory and can be used
as a standard VXIpnp driver.

2.1    Instrument Addresses

The syntax for the Instrument Descriptor is following:
USB::<vendor Id>::<product Id>::<serial number>

where <vendor Id> is 0aad for Rohde&Schwarz
      <product Id> depends on the sensor
                       NRP-Z21  : 0x0003
                       NRP-FU   : 0x0004
                       FSH-Z1   : 0x000b
                       NRP-Z11  : 0x000c
                       NRP-Z22  : 0x0013
                       NRP-Z23  : 0x0014
                       NRP-Z24  : 0x0015
                       NRP-Z51  : 0x0016
                       NRP-Z52  : 0x0017
                       NRP-Z55  : 0x0018
                       NRP-Z56  : 0x0019
                       FSH-Z18  : 0x001a
                       NRP-Z91  : 0x0021
                       NRP-Z81  : 0x0023
                       NRP-Z31  : 0x002c
                       NRP-Z37  : 0x002d
                       NRP-Z96  : 0x002e
                       NRP-Z27  : 0x002f
                       NRP-Z28  : 0x0051
                       NRP-Z98  : 0x0052
                       NRP-Z92  : 0x0062
                       NRP-Z57  : 0x0070
                       NRP-Z85  : 0x0083
                       NRPC40   : 0x008f
                       NRPC50   : 0x0090
                       NRP-Z86  : 0x0095
                       NRP-Z211 : 0x00a6
                       NRP-Z221 : 0x00a7
                       NRP-Z58  : 0x00a8
                       NRPC33   : 0x00b6
                       NRPC18   : 0x00bf
      <serial number>  you can find on your sensor. Serial number is number
                       with 6 digits. For exampel 9000003

Examples: "USB::0x0aad::0x000b::100000"

You can use '*' for product id or serial number. We suggest full resource descriptor,
when you have connected more sensor. You can use star, when one sensor is connected.

Examples: "USB::0x0aad::0x000b::*" - Opens first FSH-Z1 sensor on the BUS.
          "USB::0x0aad::*"         - Opens first R&S sensor on the BUS.
          "*"                      - Opens first R&S sensor on the BUS.

Note:
   One instance of the driver is allowed for one process. If you initialize two
   sessions you close only one session successfully and rsnrpz_close for second
   session make memory leak. If you want work with more sensors use function
   rsnrpz_AddSensor which attach sensor to specified channel.
   rsnrpz_init attach sensor to channel 1.

2.2   Sensor Identification and Logical Names

For easy identification of sensors on the USB bus use the
ChannelAssigment.exe application,
which is distributed with the driver and found in the nrpz driver
directory.

The driver supports logical names. You can pass logical name instead of
instrument
descriptor. For example: "sensor1" instead of "USB::0x0aad::0x000b::100000".
Logical names can be configured with the Channel Assignment application.


2.3   Callbacks and Window Messages

The R&S NRP-Zxx series USB sensors support hot-plugging. Therefore this driver
also offers a method to recognize hot-plugging events in an application
program. For this you can register a pointer to a callback function
which will be called every time a sensor is attached or unplugged.

The application's function for handling hot-plugging events must have
the following signature:
    void MyDeviceChangedCallback( NRP_USERARG userArgument );

Use
    NRP_USERARG someArg = 0;   /* 0 or whatever is appropriate... */
    rsnrpz_status_setDeviceChangedCallback( MyDeviceChangedCallback, someArg );

to register your application's callback function with the driver.
After this, whenever a hot-plugging event occurs, the driver will call
your  'MyDeviceChangedCallback' with an argument of 'someArg'


Another notification mechanism exists for events which lead to SRQ. You can
register a window message, which will be sent in that case. This mechanism is
intended for advanced users only and it is not recommended to utilize this
driver-feature by unexperienced users

Use functions
    rsnrpz_status_registerWindowMessage()
    rsnrpz_status_enableEventNotification()
    rsnrpz_status_disableEventNotification()
for setting SRQ.

For example, if you would like to detect an event when sensor assigned to
channel 2 starts measuring, you should call following functions.

    rsnrpz_status_registerWindowMessage( vi, hWindow, MY_WM_MESSAGE );
    rsnrpz_status_checkEvent( vi, RSNRPZ_STATCLASS_O_MEAS, RSNRPZ_SENSOR_02 );

When sensor 2 starts measurement, driver will send the specified message
(MY_WM_MESSAGE) to specified window (hWindow).

-----------------------------------------------------------------------------

3.   Using the Rohde&Schwarz NRP-Z Driver in Application Development Environments

This section offers suggestions on using the rsnrpz_32.dll within
various application development environments.

3.1    Microsoft Visual C++ 4.0 (or higher) and Borland C++ 4.5 (or higher)

Refer to your Microsoft Visual C++ or Borland C++ manuals for information
on linking and calling DLLs.

A.) The driver uses Pascal calling conventions.

B.) Rebuilding the driver DLL should be done in a different directory
    than the one the driver in the one which the driver was installed, in order
    to observe the changes.

3.2   Microsoft Visual Basic 6.0 (or higher)

Refer to the Microsoft Visual BASIC manual for information on calling DLLs.
The BASIC include file is rsnrpz.bas, which is contained in the directory
~vxipnp\win95\include. The ~ refers to the directory in the VXIPNP variable.
By default this is set to C:\.

3.3   HP VEE Version 6.0 Professional (or higher)

Your copy of HP VEE for WINDOWS contains a document titled "Using
VXIplug&play Drivers with HP VEE for Windows." This document contains
the detailed information you need for HP VEE applications.

3.4   National Instruments LabWindows/CVI(R) 5.5 (or higher)

The rsnrpz driver is supplied as both a source code file and as a dynamic
link library file (dll). There are several advantages to using the dll
form of the driver. These include:

1. Transportability across different computer platforms

2. Faster load time for your project

LabWindows/CVI (R) by default will attempt to load the source version
of the instrument driver. To load the dll you must include the file
rsnrpz.fp in your project. This file can be found in the vxipnp\win95\rsnrpz
directory. Do not include rsnrpz.c in your project. You must also provide
an include path for rsnrpz.h. This is done by adding the directory
~vxipnp\win95\include to the include paths (CVI Project Option menu) if you
have not already done so. The ~ refers to the directory in the VXIPNP
variable. By default this is set to C:\.

- --------------------------------------------------------------------------

4.0 Known Defects

    If the driver is initialized during boot phase of the NRP-Z instrument,
    it may not work correctly. Plug the device prior to initializing the driver
    and wait for finishing the boot phase (approx. 2 to 3 seconds).

- --------------------------------------------------------------------------

5.0  Limitation

1) Driver has been designed as remote control driver and it is not fully
   prepared for hot plug and unplug. Connect all sensors before you initialize
   the driver and do not disconnect sensor during comunication.

- ---------------------------------------------------------------------------

6.0  Revision History

  Rev 2.3.1.1 (2.31), 22-JAN-2013 Juergen D. Geltinger
    - fixed potential string problems with VEE
    - fixed rsnrpz_initZ5()for VEE
    - removed unused code
    - added recognition of NRPC series Power Sensors
    
  Rev 2.3.0.0 (2.30), 17-OCT-2012 Juergen D. Geltinger
    - Added functions for supporting R&S USB Hub NRP-Z5
      rsnrpz_GetDeviceStatusZ5()
      rsnrpz_GetDeviceInfoZ5()
      rsnrpz_initZ5()

  Rev 2.2.1.0 (2.21), 04-SEP-2012 Juergen D. Geltinger
    - rsnrpz_meass_fetchBufferMeasurement() and
      rsnrpz_meass_fetchBufferMeasurementAux() can be called now with
      'arraySize' and 'measurementArray' (and 'auxNArray') parameters
      set to 0 to get internal number of data-points in the expected
      result array(s)

  Rev 2.2.0.0 (2.20), 20-MAY-2012 T. Roeder
    - fixed dead lock situation in rsnrpz_chan_isMeasurementComplete()
    - fixed handling of CAL:ZERO:AUTO in rsnrpz_WriteCommand()

  Rev 2.1.9.0 (2.19), 26-MAR-2012 Juergen D. Geltinger
    - added function rsnrpz_fw_version_check() to get current and
      required minimal version of sensor firmware
    - changed signature of rsnrpz_init_long_distance() to keep
      MeasurementStudio happy

  Rev 2.1.8.0 (2.18), 18-OCT-2011 T. Roeder
    - fixed compatibility issues on 64-bit OS when VISA is included
    - rsnrpz_GetSensorInfo, channel index starts at 1 for first sensor
    - rsnrpz_chan_isMeasurementComplete checks if sensor is still alive

  Rev 2.1.7.0 (2.17), 10-JUN-2011 Juergen D. Geltinger
    - added function rsnrpz_corr_getSParamDevList()
    - clearing errors in rsnrpz_defaultInstrSetup()
    - clear session in case of errors during rsnrpz_init()

  Rev 2.1.6.0 (2.16), 08-JUN-2011 Juergen D. Geltinger
    - exporting functions rsnrpz_status_setDeviceChangedCallback() and
      rsnrpz_GetSessionControlHandle() only when not running under VEE

  Rev 2.1.5.0 (2.15), 17-MAY-2011 Juergen D. Geltinger
    - fix situation where rsnrpz_close() will be called by a user
      with a session which was already closed before
      (now returning VI_ERROR_PARAMETER1 in this case)

  Rev 2.1.4.0 (2.14), 20-APR-2011 Juergen D. Geltinger
    - better compatibility with CVI below V9.0
    - fixed CVI linkage warning

  Rev 2.1.3.0 (2.13), 08-MAR-2011 Juergen D. Geltinger
    - fixed LFR & UFR parameter of rsnrpz_chan_zeroAdvanced()

  Rev 2.1.2.0 (2.12), 26-JAN-2011 T. Roeder
    - fixed problem when unplugging sensor during measurement
    - bugfix in rsnrpz_NotifyCommand where LeaveCriticalSection may have
      been called without EnterCriticalSection
    - improved trace functionality

  Rev 2.1.1.0 (2.11), 06-DEC-2010 Juergen D. Geltinger
    - removed unused/old code fragments left over from 2.10 change

  Rev 2.1.0.0 (2.10), 03-DEC-2010 Juergen D. Geltinger
    - 'external' interface information (ViSession) always
      is a 32-bit entity now, even with the 64-bit version
      of the driver
    - Fixed define of PEAK algorithm in trace measurement

  Rev 2.0.0.0 (2.00), 04-NOV-2010 Juergen D. Geltinger
    - first official 32/64 bit release

  Rev 01.96, 29-JUN-2010 T. Roeder, J. Geltinger
    - modifications for compiling as 64 bit driver

  Rev 01.95, 16-APR-2010 Thomas Roeder
    - added NrpDeviceChangedCallback
    - added rsnrpz_status_setDeviceChangedCallback
    - changed some defines to constants

  Rev 01.94, 06-APR-2010 Thomas Roeder
    - using #if defined(__linux__) instead of LINUX
    - added support for __APPLE__
    - exported rsnrpz_service_getDetectorTemperature

  Rev 01.93, 15-MAR-2010 Juergen D. Geltinger
    - Fixed determination of PulseHighestParamNo for support of
      varying firmware versions of NRP-Z81 type sensors
      (currently 25 or 26 parameters possible; this gets
       dynamically evaluated now)

  Rev 01.92, 4-MAR-2010 Martin Krcmar
    - New functions:
        rsnrpz_chan_setCorrectionFrequencyStep
        rsnrpz_chan_getCorrectionFrequencyStep
        rsnrpz_chan_setCorrectionFrequencySpacing
        rsnrpz_chan_getCorrectionFrequencySpacing
        rsnrpz_chan_getContAvBufferCount
        rsnrpz_chan_getContAvBufferInfo
        rsnrpz_scope_setEquivalentSampling
        rsnrpz_scope_meas_setTime
        rsnrpz_scope_meas_getTime
        rsnrpz_scope_meas_setEquivalentSampling
        rsnrpz_scope_meas_getSamplePeriod
        rsnrpz_calib_test
        rsnrpz_calib_getTestDeviation
        rsnrpz_calib_getTestReference
    - Modified functions:
        rsnrpz_SetBitInRegisters
        rsnrpz_SetRegister

  Rev 01.91, 01-MAR-2010 Juergen D. Geltinger
    - added rsnrpz_chan_getCacheRange()
      (for internal use by PowerViewerPlus)
    - bugfix in rsnrpz_FormatCommand() (C locale)

  Rev 01.90, 09-SEP-2009 Juergen D. Geltinger
    - changed recognition of supported commands (not using sensor type
      anymore, but feature-bits from sensor). "Old" behaviour still
      available by defining USE_CHECKINSTR

  Rev 01.80, 09-DEC-2008 Juergen D. Geltinger
    - fixed handling of automatic pulse parameter measurement results
      in conjunction with isMeasurementComplete()

  Rev 01.79, 12-NOV-2008 Juergen D. Geltinger
    - dropout time handling no longer limited to NRP-Z81 only (relies on
      sensor features now)
    - reading pulse measurement's scalar results from cache instead of
      requesting them from the device again

  Rev 01.78, 04-NOV-2008 Juergen D. Geltinger
    - revision query no longer needs an open session

  Rev 01.77, 09-SEP-2008 Juergen D. Geltinger
    - suppressed SYST:INIT in initialization because of side effect of
      resetting zero cal values etc.
    - fix for identification of special sensor version NRP-Z311

  Rev 01.76, 07-JUL-2008 Juergen D. Geltinger
    - added missing (char*) cast for clean LINUX compilation

  Rev 01.75, 20-JUN-2008 Juergen D. Geltinger
    - added support for command 'SYSTEM:INFO? "Uptime"'
    - fixed rsnrpz_service_getDetectorTemperature()
    - fixed rsnrpz_chan_info() with empty item
    - added product-IDs of new sensors to list in chapter 2.1 of this document

  Rev 01.74, 09-JUN-2008 Jiri Kominek/Thomas Roeder/JDG
    - Added Pulse Analysis scope measurement (NRP-Z81 only)
    - changed sequence of error handling in ReadValue()

  Rev 01.73, 21-MAY-2008 Thomas Roeder
    - fixed memory leak in NrpDataAvailableCallback for aux channels
    - added (char*) type casts to get rid of gcc -Wwrite-string warnings:
      'deprecated conversion from string constant...'

  Rev 01.72, 16-MAY-2008 Juergen D. Geltinger
    - added error check for correctly sent command in WriteCommand
      for not setting up a wait event in case of an error
    - fixed evaluation of registry key for initialization

  Rev 01.71, 26-MAR-2008 Thomas Roeder
    - thread-safe initialization and fixed some LINUX compiler warnings

  Rev 01.70, 13-MAR-2008 Juergen D. Geltinger
    - fixed deadlock situation in error handling

  Rev 01.69, 04-MAR-2008 Juergen D. Geltinger
    - fixed compilation under LINUX (stricmp() was missing) 

  Rev 01.68, 22-FEB-2008 Juergen D. Geltinger
    - cleared LabWindows/CVI compiler/linker warnings

  Rev 01.67, 08-FEB-2008 Juergen D. Geltinger
    - fixed compilation problem with #included "visa.h"

  Rev 01.66, 15-JAN-2008 Juergen D. Geltinger
    - integrated latest version of NrpControl2.dll

  Rev 01.65, 09-JAN-2008 Juergen D. Geltinger
    - added SYST:INFO item 'FPGA Build', and made item check
      case insensitive

  Rev 01.64, 28-NOV-2007 Juergen D. Geltinger
    - added conditional defines for LINUX compilation
        HINT:  There is currently no notification mechanism
               for service requests (SRQ) under LINUX !
    - added rsnrpz_GetSensorCount() and rsnrpz_GetSensorInfo()
    - fixed rsnrpz_chan[s]_isMeasurementComplete checks in Statistics modes

  Rev 01.63, 10-AUG-2007 Juergen D. Geltinger
   - added rsnrpz_GetAvailableSensors() to retrieve a list of all
     actually connected sensors

  Rev 01.62, 19-JUL-2007 Juergen D. Geltinger
   - fixed usage of the same event-handles for both read and write
     operations due to Rev 1.60 changes (rsnrpz_meass_readMeasurement()
     did not work any more)

   Rev 01.61, 15-JUN-2007 Juergen D. Geltinger
   - fixed CriticalSection issue

   Rev 01.60, 06/2007 Juergen D. Geltinger
   - fixed usage of more than one session at a time. In that case
       the rsnrpz_close() unloaded/closed the NrpControl driver as
       soon as the first session was closed. This was wrong!
       NrpCloseDriver() is only allowed to be called after the
       LAST session was closed!

    Rev 01.51, 11-MAY-2007 Jiri Kominek
    - New functions:
      rsnrpz_trigger_setMasterState
      rsnrpz_trigger_getMasterState
      rsnrpz_trigger_setSyncState
      rsnrpz_trigger_getSyncState

    Rev 01.50, 02/2007 Juergen D. Geltinger
    - enhanced version of rsnrpz_init(); now queries the registry
      to find out whether a long distance connection should be
      established (for 'AnywhereUSB' remote hub)
    - New functions:
        rsnrpz_init_long_distance()
          to support connection of sensors via 'AnywhereUSB' remote hub

    Rev 01.40, 10/2006 Juergen D. Geltinger
    - New functions:
      rsnrpz_get_timeout
      rsnrpz_set_timeout
        Fixes a timeout-terminated hang in USB Write Functions
        on Windows XP Embedded

    Rev 01.31, 08/2006 by J. Kominek
    - New functions:
      rsnrpz_bandwidth_setBw
      rsnrpz_bandwidth_getBw
      rsnrpz_bandwidth_getBwList
      rsnrpz_chan_setBurstChopperEnabled
      rsnrpz_chan_getBurstChopperEnabled
      Timegates subsystem
      rsnrpz_tslot_setTimeSlotMidOffset
      rsnrpz_tslot_getTimeSlotMidOffset
      rsnrpz_tslot_setTimeSlotMidLength
      rsnrpz_tslot_getTimeSlotMidLength
      rsnrpz_tslot_setTimeSlotChopperEnabled
      rsnrpz_tslot_getTimeSlotChopperEnabled
      rsnrpz_trigger_setDropoutTime
      rsnrpz_trigger_getDropoutTime
      rsnrpz_chan_setAuxiliary
      rsnrpz_chan_getAuxiliary
      rsnrpz_meass_readMeasurementAux
      rsnrpz_meass_fetchMeasurementAux
      rsnrpz_meass_readBufferMeasurementAux
      rsnrpz_meass_fetchBufferMeasurementAux
      rsnrpz_stat_confTimegate
      rsnrpz_stat_confScale
      rsnrpz_stat_setOffsetTime
      rsnrpz_stat_getOffsetTime
      rsnrpz_stat_setMidOffset
      rsnrpz_stat_getMidOffset
      rsnrpz_stat_setMidLength
      rsnrpz_stat_getMidLength
      rsnrpz_stat_setScaleRefLevel
      rsnrpz_stat_getScaleRefLevel
      rsnrpz_stat_setScaleRange
      rsnrpz_stat_getScaleRange
      rsnrpz_stat_setScalePoints
      rsnrpz_stat_getScalePoints
      rsnrpz_stat_getScaleWidth
      rsnrpz_scope_fastZero
      rsnrpz_corr_setSParamDevice
      rsnrpz_corr_getSParamDevice
      rsnrpz_chan_setReflectionGammaUncertainty
      rsnrpz_chan_getReflectionGammaUncertainty
      rsnrpz_chan_zeroAdvanced

  Rev 01.30, 11/2005 by J. Kominek
  - FPHelp fixed - command Index Added into CHM
  - Added support of NRP-Z37, 27, 81
  - Modified functions:
      rsnrpz_chan_mode
  - New functions:
      rsnrpz_chan_configureReflectGammaCorr
      rsnrpz_chan_setReflectionGammaMagn
      rsnrpz_chan_getReflectionGammaMagn
      rsnrpz_chan_setReflectionGammaPhase
      rsnrpz_chan_getReflectionGammaPhase

  Rev 01.22, 04/2005 by J. Kominek
  - FP Help fixed

  Rev 01.21, 04/2005 by K.P.
  - Fixed rsnrpz_CloseSensor - forces freeing of user data

  Rev 01.20, 11/2004 by Z.R.
  - Fixed NrpContol2.dll
  - Fixed rsnrpz_AddSensor

  Rev 01.19, 08/2004 by Z.R.
  - Fixed NrpControl2.dll

  Rev 01.18, 03/2004 by Z.R.
  - Fixed NrpControl2.dll

  Rev 01.17, 12/2003 by Z.R.
  - Removed debug lines with MessageBeep function (now it's 10x faster)

  Rev 01.16, 10/2003 by Z.R., J.K.
  - Added new NrpControl2.dll (revision 01.21)
  - Float parameters are formated with %e instead of %f
  - Minor changes in FP file
  - Fixed rsnrpz_status_checkEvent function
  - Changed default value of transition from PTR to NONE
    - changed function rsnrpz_InitializeRegisters
    - changed FP of rsnrpz_status_catchEvent
  - Fixed rsnrpz_chan_infosCount

 Rev 01.15, 09/2003 by Z.R.
  - Added new NrpControl2.dll (revision 01.21)
  - Minor changes in FP file

 Rev 01.14, 08/2003 by Z.R.
  - Added new Nrpcontrol2.dll (revision 01.20)

 Rev 01.13, 08/2003 by Z.R.
  - Added VISA constants and types into header file. VISA is not required.

 Rev 01.12, 07/2003 by Z.R.
  - Fixed bug in the error callback

 Rev 01.11, 07/2003 by Z.R.
  - ignored Sample Error in release version.
  - used macro constant for device errors instead of magic numbers
  - isMeasComplete functions don't reflect measurement bits and they return
    TRUE when data are available. When functions return TRUE and new data
    has not been received, all other isMeasurementComplete funvtion calls
    return FALSE.

 Rev 01.10, 07/2003 by Zdenek Rykala
  - Fixeds rsnrpz_chans_isMeasurementComplete and
    rsnrpz_chan_isMeasurementComplete. Funxtions do not return VI_TRUE when
    sensor is in wait for trigger state
  - Fixed functions rsnrpz_chans_zero and rsnrpz_chan_zero
    These function do not wait for acknowledge.
  - Added better checking for invalid session handle
  - Fixed functions rsnrpz_avg_setAutoEnabled, rsnrpz_avg_getAutoEnabled
    rsnrpz_scope_setAutoEnabled, rsnrpz_scope_getAutoEnabled.
  - If direction is RSNRPZ_DIRECTION_PTR, and is then set to
    RSNRPZ_DIRECTION_NTR, the PTR is removed
  - rsnrpz_close and rsnrpz_CloseSensor functions send *RST.
  - Installer distribute NrpControl2.dll v01.10

 Rev 01.01, 06/2003 by Zdenek Rykala
  - Cosmetic changes in the help
  - Fixed rsnrpz_error_message function (translate NrpConrol2 errors)
  - Fetch function returns an error when data are not available
  - Fixed funxtions rsnrpz_chans_isZeroingComplete and
    rsnrpz_chans_isMeasurementComplete

 Rev 01.00, 05/2003 by Zdenek Rykala
  - Driver created

- ---------------------------------------------------------------------------

7.0  Files Installed

The Install program will place the following files on the hard drive
under the destination folder. (Depeding on the operating system version,
the destination folder may vary. For examples, under Win7-64, the files
will normally go to "c:/Program Files/IVI Foundation/VISA/Win64", the
32-bit files go to "c:/Program Files (x86)/IVI Foundation/VISA/WinNT".
Hence, bear in mind, that the phrase <DEST> below may constitude different
directories!)

<DEST>/Include/rsnrpz.h             - Header file for use with C/C++, HP VEE
                                      and LabView/LabWindows
<DEST>/Include/rsnrpz.cs            - Module file for C#
<DEST>/Include/rsnrpz.vb            - Module file for use with Visual BASIC .NET
<DEST>/Include/rsnrpz.bas           - Module file for use with Visual BASIC
<DEST>/Include/NrpControl2.h        - Header file for use with C/C++ and LabWindows
<DEST>/Bin/rsnrpz_32.dll            - Dynamic Link Library file for use with
                                      all languages/platforms (32 bit)
<DEST>/Bin/rsnrpz_64.dll            - Dynamic Link Library file for use with
                                      all languages/platforms (64 bit)
<DEST>/lib/msc/NrpControl2.lib      - Library file for use with C/C++ (32 bit)
<DEST>/lib/msc/NrpControl2_64.lib   - Library file for use with C/C++ (64 bit)
<DEST>/lib/msc/rsnrpz.lib           - Library file for use with C/C++ (32 bit)
<DEST>/lib/msc/rsnrpz_64.lib        - Library file for use with C/C++ (64 bit)
<DEST>/rsnrpz/rsnrpz.c              - Source code for use with C/C++
<DEST>/rsnrpz/rsnrpz.def            - Definition file for use with C++ when
                                      building the 32-bit .dll
<DEST>/rsnrpz/rsnrpz64.def          - Definition file for use with C++ when
                                      building the 64-bit .dll
<DEST>/rsnrpz/rsnrpz.fp             - Function Panel file for use with HP VEE
                                      and LabVIEW/LabWindows
<DEST>/rsnrpz/rsnrpz_vxi.chm        - Help file for use during application
                                      programming
<DEST>/rsnrpz/readme.txt            - THIS file containing general information
<DEST>/rsnrpz/ChannelAssignment.exe - Application for configuring Logical Names

- ---------------------------------------------------------------------------

Should you have any technical questions please contact
the hotline of Rohde & Schwarz Vertriebs-GmbH
Rohde & Schwarz Support Center
e-mail: CustomerSupport@rsd.rohde-schwarz.com

- ---------------------------------------------------------------------------

LabWindows/CVI & LabVIEW are registered trademarks of National Instruments.
