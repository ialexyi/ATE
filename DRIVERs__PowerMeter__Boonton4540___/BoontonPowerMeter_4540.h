#ifndef __BoontonPowerMeter_4540_H__
#define __BoontonPowerMeter_4540_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"  

//==============================================================================
// Constants

#define				NO_ERRORS					0
#define				FUNC_IN_NULL_POINTER		-1
#define				CANNOT_ALLOCATE_MEMORY		-2   
#define				WRONG_HANDLE				-3
#define				VISA_ERROR					-4
#define				CANNOT_CONNECT				-5   
#define				NO_DATA_SENT				-6
		
//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

int		DLLEXPORT	PowerMeter_Initiate( int hParentInstrumentHandle , char* szRsrcAddress , int *vhInstrumentHandle , int *InstrumentConnectStatus , int *InstrumentChannelsQuantity );

int		DLLEXPORT	PowerMeter_Close( int *pHandle );

char				*PowerMeter_GetLastError(int handle);

int		DLLEXPORT	PowerMeter_Reset(int handle);
/*
Set the state of the data acquisition mode for single or free-run measurements.
If Measurement is set to On, the 4540 immediately begins taking measurements (Modulated and Statistical Modes), 
or arms its trigger and takes a measurement each time a trigger occurs (Pulse Mode).
If set to Off, the measurement will begin (or be armed) if the Single Sweep menu is selected,
and will stop once the measurement criteria (averaging) has been satisfied. 
If the measurement mode is set to Modulated or Statistical,
no measurements will be made if measurement is set to off.
*/
int		DLLEXPORT	PowerMeter_SetSingleSweep(int handle,int channel);
int		DLLEXPORT	PowerMeter_SetContinuesSweep(int handle,int channel);

/*
Starts a single measurement cycle in Pulse mode.
Enough trace sweeps must be triggered to satisfy the channel averaging setting
*/
int		DLLEXPORT	PowerMeter_InitSweep(int handle,int channel);

/*
Perform the Auto Setup function to acquire a signal and display the trace in the Pulse Mode. 
Note: Pulse Mode is forced if not already set.
*/
int		DLLEXPORT	PowerMeter_SetAuto(int handle,int channel);

/*
Set or return the measurement state of the selected
channel. When ON, the channel performs
measurements; when OFF, the channel is disabled and
no measurements are performed.
*/
int		DLLEXPORT	PowerMeter_SetChanelState(int handle,int channel, int state);

/*
Set the power or voltage vertical sensitivity of the trace
display in channel units.
*/
int		DLLEXPORT	PowerMeter_SetVericalScale(int handle,int channel, double scale);

/*
Set or return the power or voltage level of the
horizontal centerline of the graph for the specified
channel in channel units. If a change in the vertical
scale causes the center maximum value to be exceeded, the center will be forced to the
maximum value for the new range.
*/
int		DLLEXPORT	PowerMeter_SetVericalCenter(int handle,int channel, double center);

/*
Initiates a multi-point sensor gain calibration of the
selected sensor with the either the internal calibrator, or
an optional external 1 GHz calibrator. This procedure
calibrates the sensorís linearity at a number of points
across its entire dynamic range. A sensor must be
connected to the channel input and the selected
calibrator output.
*/
int		DLLEXPORT	PowerMeter_Calibrate(int handle,int channel);

/*
For a sensor that has been previously auto calibrated
and has valid status, initiates a zero offset null
adjustment. The sensor does not need to be connected
to any calibrator for zeroing ñ the procedure is often performed in-system. However, this
command will turn off the specified calibrator (defaults to internal) prior to zeroing to
avoid the need to perform this step explicitly
*/
int		DLLEXPORT	PowerMeter_Zero(int handle,int channel);

/*
Set the channel units. For power sensors, 
voltage is calculated with reference to the sensor input impedance.
Note that for ratiometric results,
logarithmic units will always return dBr (dB relative) while linear units return percent.

 0- dBm , 1 - Watts, 2 - Volts, 3 - dBV, 4 - dBmV, 5- dBuV  
 
*/
int		DLLEXPORT	PowerMeter_SetUnits(int handle,int channel, unsigned int units);

/*
Set a measurement offset in dB for the selected sensor.
This is used to compensate for external couplers,
attenuators or amplifiers in the RF signal path ahead of the power sensor.
*/
int		DLLEXPORT	PowerMeter_SetOffset(int handle,int channel, double Offset);

/*
Set the frequency calfactor currently in use on the selected channel. 
Note setting a calfactor with this command will override the Å\automatic.
frequency calfactor that was calculated and applied when the operating frequency was set, 
and setting the operating frequency will override this calfactor setting.

*/
int		DLLEXPORT	PowerMeter_SetCalFactor(int handle,int channel, double CalFactor);

/*
Set the sensor video bandwidth for the selected sensor. 
The trigger channel bandwidth is not affected.
High is the normal setting for most measurements.
The actual bandwidth is determined by the peak sensor model used. 
Use low bandwidth for additional noise reduction when measuring CW or signals with very low modulation bandwidth. 
If Low bandwidth is used on signals with fast modulation, 
measurement errors may result because the sensor cannot track the fast changing envelope of the signal.
The sensor connected to the channel must support video bandwidth switching. 
This command is not valid for CW sensors or voltage probes.

0- Low , 1 - High
*/

int		DLLEXPORT	PowerMeter_SetVideoBW(int handle,int channel, unsigned int VideoBW);

/*
Set the operating mode of the selected channelís peak hold function. 
When set to OFF, instantaneous peak readings are held for a short time, 
and then decayed towards the average power at a rate proportional to the filter time.
This is the best setting for most signals,
because the peak will always represent the peak power of the current signal, 
and the resulting peak-to-average ratio will be correct shortly after any signal level changes.
When set to ON, instantaneous peak readings are held until reset by a new INITiate command or cleared manually. 
This setting is used when it is desirable to hold the highest peak over a long measurement interval without any decay
*/
int		DLLEXPORT	PowerMeter_SetPeakHold(int handle,int channel, int state);

/*
Set the RF frequency for the current measurement. The appropriate frequency calfactor
from the sensorÅfs EEPROM table will be interpolated and applied automatically.
Application of this calfactor cancels out the effect of variations in the flatness of the
sensorÅfs frequency response. If an explicit calfactor has been set, either manually or via
the SENSe:CORRection:CALFactor command, entering a new frequency will override
this calfactor and use only the Å\automatic. frequency calfactor.

Range: 0.001 GHz to 110.00 GHz (sensor and video bandwidth dependant)

*/
int		DLLEXPORT	PowerMeter_SetFrequency(int handle,int channel, double Frequency);

/*
Set the trigger mode for synchronizing data acquisition with pulsed signals.
Normal mode will cause a sweep to be triggered each time the power level crosses the preset trigger level in the direction specified by the trigger slope setting. 
If there are no edges that cross this level, no data acquisition will occur.
Auto mode operates in much the same way as Normal mode, 
but will automatically generate a trace if no trigger edges are detected for a period of time (100 to 500 milliseconds, depending on timebase). 
This will keep the trace updating even if the pulse edges stop. 
The Auto PK-PK mode operates the same as AUTO mode,
but will adjust the trigger level to halfway between the highest and lowest power or voltage levels detected. 
This aids in maintaining synchronization with a pulse signal of varying level.
The Freerun mode forces unsynchronized traces at a high rate to assist in locating the signal.

 0 - Auto , 1 - Auto PK-PK , 2 - Normal , 3 - Freerun

*/
int		DLLEXPORT	PowerMeter_SetMode(int handle,unsigned int Mode);

/*
*/
int		DLLEXPORT	PowerMeter_OPC(int handle,double timeout, int *state);

/*
*/
int 	DLLEXPORT	PowerMeter_FetchCWpower(int handle,int channel, double timeout, double *value);

/*
*/
int		DLLEXPORT	PowerMeter_MeasureModulatedPower(int handle,int channel, double timeout, double *value);

/*
*/
int		DLLEXPORT	PowerMeter_FetchMarkerAverage(int handle,int channel, int marker, double timeout,double *value);

/*
*/
int		DLLEXPORT	PowerMeter_FetchMarkerMax(int handle,int channel, int marker, double timeout,double *value);

/*
*/
int		DLLEXPORT	PowerMeter_FetchMarkerMin(int handle,int channel, int marker, double timeout,double *value);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __BoontonPowerMeter_4540_H__ */
