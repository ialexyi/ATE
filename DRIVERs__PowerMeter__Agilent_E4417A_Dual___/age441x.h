/*= EPM Series Power Meters =================================================*/

/*===========================================================================*/
/*  Please do not use global variables or arrays in the include file of      */
/*  instrument drivers that will be submitted for inclusion into the         */
/*  LabWindows Instrument Driver Library.                                    */
/*===========================================================================*/
     
#ifndef __age441x_HEADER
#define __age441x_HEADER

#include <vpptype.h>

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

/*****************************************************************************/
/*= Instrument Driver Specific Error/Warning Codes ==========================*/
/*****************************************************************************/
#define VI_ERROR_INSTR_FILE_OPEN                (_VI_ERROR+0x3FFC0800L)
#define VI_ERROR_INSTR_FILE_WRITE               (_VI_ERROR+0x3FFC0801L)
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
#define VI_WARNING_INSTR_OFFSET                           (0x3FFC0900L)
#define VI_ERROR_INSTR_OFFSET                   (_VI_ERROR+0x3FFC0900L)

#define AGE441X_ERROR_INSTRUMENT_ERROR        (VI_ERROR_INSTR_OFFSET + 0xF0L)
#define AGE441X_ERROR_INSTRUMENT_NUMBER       (VI_ERROR_INSTR_OFFSET + 0xF1L)
#define AGE441X_ERROR_INVALID_INSTRUMENT	  (VI_ERROR_INSTR_OFFSET + 0xF2L) 	
#define AGE441X_ERROR_BAD_SENSOR			  (VI_ERROR_INSTR_OFFSET + 0xF3L) 	
#define AGE441X_ERROR_NAME_NOT_FOUND		  (VI_ERROR_INSTR_OFFSET + 0xF4L) 
#define AGE441X_ERROR_INTERFACE				  (VI_ERROR_INSTR_OFFSET + 0xF5L) 	

/*****************************************************************************/
/*= Instrument specific defines =============================================*/
/*****************************************************************************/
#define AGE441X_SENSOR_NONE					0
#define AGE441X_SENSOR_8480					1
#define AGE441X_SENSOR_E					2
#define AGE441X_SENSOR_E9320				3

#define AGE441X_STATUS_SRE					0
#define AGE441X_STATUS_ESE					1
#define AGE441X_STATUS_DEVICE				2 
#define AGE441X_STATUS_OPERATION			3    
#define AGE441X_STATUS_QUESTIONABLE			4 
#define AGE441X_STATUS_O_CAL				5 
#define AGE441X_STATUS_O_MEAS				6	
#define AGE441X_STATUS_O_TRIGGER			7 
#define AGE441X_STATUS_O_SENSE				8 
#define AGE441X_STATUS_O_LFAIL				9 
#define AGE441X_STATUS_O_UFAIL				10
#define AGE441X_STATUS_Q_POWER				11					
#define AGE441X_STATUS_Q_CAL				12
#define AGE441X_STATUS_ESR					13 
#define AGE441X_STATUS_STB					14 

#define AGE441X_STATUS_NEG					0
#define AGE441X_STATUS_POS					1
#define AGE441X_STATUS_ENABLE				2
#define AGE441X_STATUS_EVEN					3  
#define AGE441X_STATUS_COND					4  

#define AGE441X_STATE_OFF					0
#define AGE441X_STATE_ON					1
#define AGE441X_STATE_ONCE					2

#define AGE441X_CHANNEL_A					0
#define AGE441X_CHANNEL_B					1

#define AGE441X_MATH_VALUE 					0
#define AGE441X_MATH_DIFF  					1
#define AGE441X_MATH_RATIO 					2

#define AGE441X_FEED_AVER 					0
#define AGE441X_FEED_PTAV 					1
#define AGE441X_FEED_PEAK 					2

#define AGE441X_MEAS_NORMAL 				0
#define AGE441X_MEAS_DOUBLE 				1
#define AGE441X_MEAS_FAST   				2

#define AGE441X_TABLE_CAL  					0
#define AGE441X_TABLE_OFFS 					1

#define AGE441X_LINCORR_ATYP 				0
#define AGE441X_LINCORR_DTYP 				1

#define AGE441X_MEAS_SINGLE     			0
#define AGE441X_MEAS_SINGLE_REL 			1
#define AGE441X_MEAS_DIFF       			2
#define AGE441X_MEAS_DIFF_REL   			3
#define AGE441X_MEAS_RATIO      			4
#define AGE441X_MEAS_RATIO_REL  			5

#define AGE411x_UNIT_DBM 					0
#define AGE411x_UNIT_W   					1

#define AGE441X_TTL_LOW  					0
#define AGE441X_TTL_HIGH 					1
#define AGE441X_TTL_BOTH					2

#define AGE441X_TRIG_IMM  					0
#define AGE441X_TRIG_BUS  					1
#define AGE441X_TRIG_HOLD 					2
#define AGE441X_TRIG_EXT  					3
#define AGE441X_TRIG_INTA 					4
#define AGE441X_TRIG_INTB 					5

#define AGE441X_SLOPE_NEG 					0
#define AGE441X_SLOPE_POS 					1

#define AGE441_POWRANG_LOW 					0
#define AGE441_POWRANG_UPP 					1

#define AGE441X_BAND_OFF  					0
#define AGE441X_BAND_LOW  					1
#define AGE441X_BAND_MED  					2
#define AGE441X_BAND_HIGH 					3

#define AGE441X_E9320_MODE_NORM 			0
#define AGE441X_E9320_MODE_AVER 			1

#define AGE441X_MEM_TABLE 					0
#define AGE441X_MEM_SRREG 					1
#define AGE441X_MEM_ALL   					2

#define AGE441X_MEM_SEL   					0
#define AGE441X_MEM_CLEAR 					1
#define AGE441X_MEM_MOVE  					2


#define AGE441X_CAL_ALL  					0
#define AGE441X_CAL_CAL  					1
#define AGE441X_CAL_ZERO 					2

#define AGE441X_UNIT_DBM					0
#define	AGE441X_UNIT_W						1

#define AGE441X_TRACE_RES_LOW  				0
#define AGE441X_TRACE_RES_MED  				1
#define AGE441X_TRACE_RES_HIGH				2

#define AGE441X_POWRANG_LOW					0
#define AGE441X_POWRANG_UPP					1
/*****************************************************************************/
/*= GLOBAL USER-CALLABLE FUNCTION DECLARATIONS (Exportable Functions) =======*/
/*****************************************************************************/
ViStatus _VI_FUNC age441x_init (ViRsrc resourceName, ViBoolean IDQuery,
                    ViBoolean resetDevice, ViPSession instrSession);

ViStatus _VI_FUNC age441x_powerMeasurement (ViSession instrumentHandle,
                                            ViInt32 channel, ViInt32 unit,
                                            ViReal64 lowerLimit,
                                            ViReal64 upperLimit,
                                            ViBoolean *limitFail,
                                            ViReal64 *value);

ViStatus _VI_FUNC age441x_configureInput (ViSession instrumentHandle,
                                          ViInt32 channel, ViReal64 frequency,
                                          ViInt32 measurementSpeed);


ViStatus _VI_FUNC age441x_configureFrequency (ViSession instrumentHandle,
                                          ViInt32 channel, 
                                          ViReal64 frequency);
										  
ViStatus _VI_FUNC age441x_configureChannelOffset (ViSession instrumentHandle,
                                                  ViInt32 channel,
                                                  ViBoolean channelOffsetState,
                                                  ViReal64 channelOffset);

ViStatus _VI_FUNC age441x_configureAveragingAuto (ViSession instrumentHandle,
                                                  ViInt32 channel,
                                                  ViBoolean stepDetection);

ViStatus _VI_FUNC age441x_configureAveraging (ViSession instrumentHandle,
                                              ViInt32 channel, ViBoolean state,
                                              ViInt32 filterLength);

ViStatus _VI_FUNC age441x_linearityCorrection (ViSession instrumentHandle,
                                               ViInt32 channel, ViInt32 type);

ViStatus _VI_FUNC age441x_getFrequencyDependentOffset (ViSession instrumentHandle,
                                                       ViInt32 channel,
                                                       ViReal64 *offset);

ViStatus _VI_FUNC age441x_configureLimits (ViSession instrumentHandle,
                                           ViInt32 measurement, 
                                           ViBoolean state,
                                           ViInt32 autoClear,
                                           ViReal64 lowerLimit,
                                           ViReal64 upperLimit);

ViStatus _VI_FUNC age441x_configureMath (ViSession instrumentHandle,
                                         ViInt32 measurement, ViInt32 sensor1,
                                         ViInt32 operation, ViInt32 sensor2);

ViStatus _VI_FUNC age441x_getMathExpressions (ViSession instrumentHandle,
                                              ViInt32 measurement,
                                              ViInt32 arraySize,
                                              ViInt32 sensor1[],
                                              ViInt32 expression[],
                                              ViInt32 sensor2[],
                                              ViInt32 *returnedValues);

ViStatus _VI_FUNC age441x_configureGain (ViSession instrumentHandle,
                                         ViInt32 measurement, ViBoolean state,
                                         ViReal64 value);

ViStatus _VI_FUNC age441x_resetRefValue (ViSession instrumentHandle,
                                         ViInt32 measurement);

ViStatus _VI_FUNC age441x_setRefValueState (ViSession instrumentHandle,
                                            ViInt32 measurement, ViBoolean state);

ViStatus _VI_FUNC age441x_feed (ViSession instrumentHandle, ViInt32 measurement,
                                ViInt32 feed, ViInt32 type);


ViStatus _VI_FUNC age441x_configureMeasurement (ViSession instrumentHandle,
                                                ViInt32 measurement,
                                                ViInt32 measuringFunction,
                                                ViInt32 sensor1, ViInt32 sensor2,
                                                ViInt32 resolution, ViInt32 unit);

ViStatus _VI_FUNC age441x_setTTLCalibrationState (ViSession instrumentHandle,
                                                  ViBoolean state);

ViStatus _VI_FUNC age441x_calibrationLockout (ViSession instrumentHandle,
                                              ViInt32 channel, ViBoolean state);


ViStatus _VI_FUNC age441x_configureRecorder (ViSession instrumentHandle,
                                             ViInt32 recorder,
                                             ViReal64 limitLower,
                                             ViReal64 limitUpper,
                                             ViBoolean state);

ViStatus _VI_FUNC age441x_recorderMeasurement (ViSession instrumentHandle,
                                               ViInt32 recorder,
                                               ViInt32 measurement);

ViStatus _VI_FUNC age441x_referenceOscillatorState (ViSession instrumentHandle,
                                                    ViBoolean state);

ViStatus _VI_FUNC age441x_configureTTL (ViSession instrumentHandle,
                                        ViInt32 TTLOutput, ViBoolean state,
                                        ViInt32 activeMode, ViInt32 measurement,
                                        ViInt32 limit);

ViStatus _VI_FUNC age441x_setInitiateContinuousState (ViSession instrumentHandle,
                                                      ViInt32 channel,
                                                      ViBoolean state);

ViStatus _VI_FUNC age441x_configureTrigger (ViSession instrumentHandle,
                                            ViInt32 channel,
                                            ViBoolean delayAutoState,
                                            ViInt32 source);

ViStatus _VI_FUNC age441x_configureChannelATrigger (ViSession instrumentHandle,
                                                    ViReal64 delay,
                                                    ViReal64 holdoff,
                                                    ViReal64 hysteresis,
                                                    ViReal64 powerLevel,
                                                    ViInt32 levelAuto,
                                                    ViInt32 slope);

ViStatus _VI_FUNC age441x_triggerEventCount (ViSession instrumentHandle,
                                             ViInt32 channel, ViInt32 count);

ViStatus _VI_FUNC age441x_eSeriesOffsetTable (ViSession instrumentHandle,
                                              ViInt32 channel,
                                              ViBoolean tableState,
                                              ViString tableName);

ViStatus _VI_FUNC age441x_eSeriesConfigureInput (ViSession instrumentHandle,
                                                 ViInt32 channel,
                                                 ViBoolean autoranging,
                                                 ViInt32 powerRange);

ViStatus _VI_FUNC age441x_eSeriesConfigureMeasurement (ViSession instrumentHandle,
                                                       ViInt32 measurement,
                                                       ViInt32 measuringFunction,
                                                       ViInt32 sensor1,
                                                       ViInt32 sensor2,
                                                       ViInt32 resolution,
                                                       ViReal64 expectedValue,
                                                       ViInt32 unit);

ViStatus _VI_FUNC age441x_e9320VideoAveraging (ViSession instrumentHandle,
                                          ViInt32 channel, ViBoolean state,
                                          ViInt32 filterLength);


ViStatus _VI_FUNC age441x_e9320SensorParameters (ViSession instrumentHandle,
                                           ViInt32 channel, ViInt32 mode,
                                           ViInt32 bandwidth);
                                           
ViStatus _VI_FUNC age441x_e9320ConfigureTrace (ViSession instrumentHandle,
                                            ViInt32 channel,
                                            ViBoolean traceState,
                                            ViReal64 traceOffsetTime,
                                            ViReal64 traceTime,
                                            ViReal64 traceLowLimit,
                                            ViReal64 traceUpperLimit,
                                            ViInt32 traceUnit);



ViStatus _VI_FUNC age441x_e9320ConfigureGate (ViSession instrumentHandle,
                                              ViInt32 channel, ViInt32 gate,
                                              ViReal64 offsetTime, ViReal64 time);

ViStatus _VI_FUNC age441x_e9320OutputTriggerState (ViSession instrumentHandle,
                                              ViBoolean state);
                                              
ViStatus _VI_FUNC age441x_configureDutyCycle (ViSession instrumentHandle,
                                              ViInt32 channel,
                                              ViBoolean dutyCycleState,
                                              ViReal64 dutyCycle);

ViStatus _VI_FUNC age441x_setCorrections(ViSession instrumentHandle,
                                           ViInt32 channel,
                                           ViBoolean tableState,
                                           ViString tableName,
                                           ViReal64 calibrationFactorGain);
                                           
ViStatus _VI_FUNC age441x_setCalibrationFactor (ViSession instrumentHandle,
                                                ViInt32 channel, ViReal64 factor);

ViStatus _VI_FUNC age441x_memoryContents (ViSession instrumentHandle,
                                          ViInt32 type, ViInt32 stringLength,
                                          ViChar catalog[], ViInt32 *retLength);

ViStatus _VI_FUNC age441x_sizeOfMemoryBlock (ViSession instrumentHandle,
                                             ViChar name[], ViInt32 *size);

ViStatus _VI_FUNC age441x_activeTableValues (ViSession instrumentHandle,
                                             ViInt32 frequencyArraySize,
                                             ViReal64 frequencyArray[],
                                             ViInt32 *returnedFreqValues,
                                             ViInt32 factorArraySize,
                                             ViReal64 factorOffset[],
                                             ViInt32 *returnedOffsetValues);

ViStatus _VI_FUNC age441x_activeTableInformation (ViSession instrumentHandle,
                                                  ViChar name[],
                                                  ViInt32 *freqPoints,
                                                  ViInt32 *offsetPoints);

ViStatus _VI_FUNC age441x_tableOperation (ViSession instrumentHandle,
                                          ViInt32 function, ViChar table1[],
                                          ViChar table2[]);

ViStatus _VI_FUNC age441x_defineCalibrationTable (ViSession instrumentHandle,
                                                  ViInt32 arraySize,
                                                  ViReal64 frequencyArray[],
                                                  ViReal64 calibrationFactors[]);

ViStatus _VI_FUNC age441x_defineOffsetTable (ViSession instrumentHandle,
                                             ViInt32 arraySize,
                                             ViReal64 frequencyArray[],
                                             ViReal64 offsets[]);

ViStatus _VI_FUNC age441x_registerMapName (ViSession instrumentHandle,
                                           ViInt32 registerNumber, ViChar name[]);

ViStatus _VI_FUNC age441x_registerCount (ViSession instrumentHandle,
                                         ViInt32 *count);

ViStatus _VI_FUNC age441x_loadFromRegister (ViSession instrumentHandle,
                                            ViInt32 registerNumber);

ViStatus _VI_FUNC age441x_saveToRegister (ViSession instrumentHandle,
                                          ViInt32 registerNumber);
                                          
ViStatus _VI_FUNC age441x_getLimitsFailure (ViSession instrumentHandle,
                                            ViInt32 measurement,
                                            ViBoolean *limitExceeded,
                                            ViInt32 *numberOfFailures);

ViStatus _VI_FUNC age441x_clearFailCount (ViSession instrumentHandle,
                                          ViInt32 measurement);

ViStatus _VI_FUNC age441x_calibration (ViSession instrumentHandle,
                                       ViInt32 channel, ViInt32 action,
                                       ViBoolean *fail);

ViStatus _VI_FUNC age441x_e9320ClearPeakHold (ViSession instrumentHandle,
                                              ViInt32 measurement);

ViStatus _VI_FUNC age441x_e9320NormalPathZeroing (ViSession instrumentHandle,
                                                  ViInt32 channel);

ViStatus _VI_FUNC age441x_availableMemory (ViSession instrumentHandle,
                                           ViInt32 type, ViInt32 *free,
                                           ViInt32 *used);

ViStatus _VI_FUNC age441x_abort (ViSession instrumentHandle, ViInt32 channel);

ViStatus _VI_FUNC age441x_initiate (ViSession instrumentHandle, ViInt32 channel);

ViStatus _VI_FUNC age441x_triggerImmediate (ViSession instrumentHandle,
                                            ViInt32 channel);

ViStatus _VI_FUNC age441x_setStatusRegisters (ViSession instrumentHandle,
                                              ViInt32 reg,
                                              ViInt32 regPart,
                                              ViUInt16 state);

ViStatus _VI_FUNC age441x_getStatusRegisters (ViSession instrumentHandle,
                                              ViInt32 register,
                                              ViInt32 registerPart,
                                              ViUInt16 *state);

ViStatus _VI_FUNC age441x_fetch (ViSession instrumentHandle, ViInt32 measurement,
                                 ViInt32 measuringFunction,
                                 ViReal64 *measuredValue);

ViStatus _VI_FUNC age441x_read (ViSession instrumentHandle, ViInt32 measurement,
                                ViInt32 measuringFunction,
                                ViReal64 *measuredValue);

ViStatus _VI_FUNC age441x_measure (ViSession instrumentHandle,
                                   ViInt32 measurement,
                                   ViInt32 measuringFunction, ViInt32 sensor1,
                                   ViInt32 sensor2, ViInt32 resolution,
                                   ViInt32 unit, ViReal64 *measuredValue);
ViStatus _VI_FUNC age441x_eSeriesMeasure (ViSession instrumentHandle,
                                          ViInt32 measurement,
                                          ViInt32 measuringFunction,
                                          ViInt32 sensor1, ViInt32 sensor2,
                                          ViInt32 resolution,
                                          ViReal64 expectedValue, ViInt32 unit,
                                          ViReal64 *measuredValue);

ViStatus _VI_FUNC age441x_e9320TraceData (ViSession instrumentHandle,
                                          ViInt32 channel, ViInt32 resolution,
                                          ViInt32 arraySize, ViReal64 data[],
                                          ViInt32 *returnedValues);

/** INSERT USER-CALLABLE INSTRUMENT-DEPENDENT FUNCTION DECLARATIONS HERE **/ 

ViStatus _VI_FUNC age441x_writeInstrData (ViSession instrSession, ViString writeBuffer);
ViStatus _VI_FUNC age441x_readInstrData (ViSession instrSession,
                    ViInt32 numberBytesToRead, ViChar _VI_FAR readBuffer[], ViPInt32 numBytesRead);
ViStatus _VI_FUNC age441x_reset (ViSession instrSession);
ViStatus _VI_FUNC age441x_self_test (ViSession instrSession, ViPInt16 selfTestResult,
                    ViChar _VI_FAR selfTestMessage[]);
ViStatus _VI_FUNC age441x_error_query (ViSession instrSession, ViPInt32 errorCode,
                    ViChar _VI_FAR errorMessage[]);
ViStatus _VI_FUNC age441x_error_message (ViSession instrSession, ViStatus statusCode,
                    ViChar _VI_FAR message[]);
ViStatus _VI_FUNC age441x_revision_query (ViSession instrSession,
                    ViChar _VI_FAR instrumentDriverRevision[], 
                    ViChar _VI_FAR firmwareRevision[]);

ViStatus _VI_FUNC age441x_instrumentType (ViSession instrumentHandle,
                                          ViInt32 *type);

ViStatus _VI_FUNC age441x_getSensorType (ViSession instrumentHandle,
                                         ViInt32 channel, ViBoolean *state,
                                         ViChar sensorType[]);
                                         
ViStatus _VI_FUNC age441x_setCheckStatus (ViSession instrumentHandle,
                                          ViBoolean state);

ViStatus _VI_FUNC age441x_getCheckStatus (ViSession instrumentHandle,
                                          ViBoolean *state);

ViStatus _VI_FUNC age441x_close (ViSession instrSession);

#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

/*****************************************************************************/
/*=== END INCLUDE FILE ======================================================*/
/*****************************************************************************/

#endif
