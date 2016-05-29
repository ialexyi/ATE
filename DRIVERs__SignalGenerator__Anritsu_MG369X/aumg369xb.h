/****************************************************************************************
 *
 * (c) 2007, National Instruments, Corporation. All Rights Reserved.         
 *                                                                          
 * Title:    aumg369xb.h
 *
 * Purpose:  ANRITSU MG369XB Synthesized Signal Generator                                     
 *           VXIPNP instrument driver declarations.                                
 *                                                                          
 ***************************************************************************************/

     
#ifndef __AUMU369XB_HEADER
#define __AUMU369XB_HEADER
									   
#include <vpptype.h>

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif
	
/***************************************************************************************/
/*= Instrument Specific Error/Warning Codes ===========================================*/
/***************************************************************************************/
#define VI_WARNING_INSTR_OFFSET								(0x3FFC0900L)
#define VI_ERROR_INSTR_OFFSET								(_VI_ERROR+0x3FFC0900L)

#define AUMG369XB_ERROR_SYNTAX_ERROR						(VI_ERROR_INSTR_OFFSET + 0xF0L)
#define	AUMG369XB_ERROR_PARAMETER_RANGE_ERROR				(VI_ERROR_INSTR_OFFSET + 0xF1L)
#define	AUMG369XB_ERROR_LOCK_ERROR							(VI_ERROR_INSTR_OFFSET + 0xF2L)

/***************************************************************************************/
/*= Instrument Models =================================================================*/
/***************************************************************************************/
#define AUMG369XB_MODEL_MG3691B								1
#define AUMG369XB_MODEL_MG3692B								2
#define AUMG369XB_MODEL_MG3693B								3
#define AUMG369XB_MODEL_MG3694B								4
#define AUMG369XB_MODEL_MG3695B								5
#define AUMG369XB_MODEL_MG3696B								6
	
/***************************************************************************************/
/*= INSTRUMENT CONSTANTS ==============================================================*/
/***************************************************************************************/

/*- for function aumg369xb_configureRF ------------------------------------------------*/
#define AUMG369XB_RAMP_REST_0								0        
#define AUMG369XB_RAMP_REST_10								1		

/*- for function aumg369xb_configureFrequencyEntry ------------------------------------*/
/*- for function aumg369xb_configureCWFrequency ---------------------------------------*/
/*- for function aumg369xb_configureSlaveFrequencyEntry -------------------------------*/
/*- for function aumg369xb_configureFrequencyMarkerEnabled ----------------------------*/
#define AUMG369XB_FREQUENCY_F0								0		
#define AUMG369XB_FREQUENCY_F1								1		
#define AUMG369XB_FREQUENCY_F2								2		
#define AUMG369XB_FREQUENCY_F3								3		
#define AUMG369XB_FREQUENCY_F4								4		
#define AUMG369XB_FREQUENCY_F5								5			
#define AUMG369XB_FREQUENCY_F6								6		
#define AUMG369XB_FREQUENCY_F7								7		
#define AUMG369XB_FREQUENCY_F8								8		
#define AUMG369XB_FREQUENCY_F9								9		
#define AUMG369XB_FREQUENCY_M0								10		
#define AUMG369XB_FREQUENCY_M1								11		
#define AUMG369XB_FREQUENCY_M2								12		
#define AUMG369XB_FREQUENCY_M3								13		
#define AUMG369XB_FREQUENCY_M4								14		
#define AUMG369XB_FREQUENCY_M5								15		
#define AUMG369XB_FREQUENCY_M6								16				
#define AUMG369XB_FREQUENCY_M7								17		
#define AUMG369XB_FREQUENCY_M8								18		
#define AUMG369XB_FREQUENCY_M9								19		

/*- for function aumg369xb_configureAnalogSweepFrequency ------------------------------*/
/*- for function aumg369xb_configureStepSweepFrequency --------------------------------*/
/*- for function aumg369xb_configureListSweepFrequency --------------------------------*/
/*- for function aumg369xb_configurePowerLevelSweep -----------------------------------*/	
#define AUMG369XB_TRIGGER_SOURCE_AUTO						0      
#define AUMG369XB_TRIGGER_SOURCE_EXTERNAL					1       
#define AUMG369XB_TRIGGER_SOURCE_SINGLE						2		
#define AUMG369XB_TRIGGER_SOURCE_MANUAL						3		

/*- for function aumg369xb_configureAnalogSweepFrequency ------------------------------*/
/*- for function aumg369xb_configureStepSweepFrequency --------------------------------*/
/*- for function aumg369xb_configureManualSweepFrequency ------------------------------*/
#define AUMG369XB_SWEEP_RANGE_FULL							0		
#define AUMG369XB_SWEEP_RANGE_F1_F2							1		
#define AUMG369XB_SWEEP_RANGE_F3_F4							2		
#define AUMG369XB_SWEEP_RANGE_F0_DF							3		
#define AUMG369XB_SWEEP_RANGE_F1_DF							4		
#define AUMG369XB_SWEEP_RANGE_F5_DF							5		
#define AUMG369XB_SWEEP_RANGE_F6_DF							6		

/*- for function aumg369xb_configureStepSweepFrequency --------------------------------*/
#define AUMG369XB_STEP_SWEEP_MODE_LOGARITHMIC				0			
#define AUMG369XB_STEP_SWEEP_MODE_LINEAR					1		

/*- for function aumg369xb_configureAlternateSweepFrequency ---------------------------*/
#define AUMG369XB_ALTERNATE_SWEEP_RANGE_FULL				0		
#define AUMG369XB_ALTERNATE_SWEEP_RANGE_F1_F2				1		
#define AUMG369XB_ALTERNATE_SWEEP_RANGE_F3_F4				2		
#define AUMG369XB_ALTERNATE_SWEEP_RANGE_F1_DF				3		
#define AUMG369XB_ALTERNATE_SWEEP_RANGE_F5_DF				4		
#define AUMG369XB_ALTERNATE_SWEEP_RANGE_F6_DF				5		

/*- for function aumg369xb_configureListSweepFrequency --------------------------------*/
/*- for function aumg369xb_configureSweepList -----------------------------------------*/
#define AUMG369XB_LIST_NUMBER_0								0		
#define AUMG369XB_LIST_NUMBER_1								1		
#define AUMG369XB_LIST_NUMBER_2								2		
#define AUMG369XB_LIST_NUMBER_3								3		

/*- for function aumg369xb_configureFrequencyMarkerMode -------------------------------*/
#define AUMG369XB_FREQUENCY_MARKER_INTENSITY				0		
#define AUMG369XB_FREQUENCY_MARKER_VIDEO					1		
#define AUMG369XB_FREQUENCY_MARKER_OFF						2		

/*- for function aumg369xb_configurePowerLevelUnit ------------------------------------*/
#define AUMG369XB_UNIT_LOGARITHMIC_DBM						0		
#define AUMG369XB_UNIT_LOGARITHMIC_DBUV						1		
#define AUMG369XB_UNIT_LINEAR_MV							2		

/*- for function aumg369xb_configurePowerLevelEntry -----------------------------------*/
/*- for function aumg369xb_configureAlternateSweepFrequency ---------------------------*/
/*- for function aumg369xb_configureFixedPowerLevel -----------------------------------*/
#define AUMG369XB_POWER_LEVEL_L0							0		
#define AUMG369XB_POWER_LEVEL_L1							1		
#define AUMG369XB_POWER_LEVEL_L2							2		
#define AUMG369XB_POWER_LEVEL_L3							3		
#define AUMG369XB_POWER_LEVEL_L4							4		
#define AUMG369XB_POWER_LEVEL_L5							5		
#define AUMG369XB_POWER_LEVEL_L6							6		
#define AUMG369XB_POWER_LEVEL_L7							7		
#define AUMG369XB_POWER_LEVEL_L8							8		
#define AUMG369XB_POWER_LEVEL_L9							9		

/*- for function aumg369xb_configurePowerLevelSweep -----------------------------------*/
#define AUMG369X_SWEEP_RANGE_L1_L2							0		

/*- for function aumg369xb_configureALCMode -------------------------------------------*/
#define AUMG369XB_LEVELING_MODE_INTERNAL					0		
#define AUMG369XB_LEVELING_MODE_EXTERNAL_DETECTOR			1		
#define AUMG369XB_LEVELING_MODE_POWER_METER					2		
#define AUMG369XB_LEVELING_MODE_FIXED_GAIN					3		
#define AUMG369XB_LEVELING_MODE_AUTO_FIXED_GAIN				4		

/*- for function aumg369xb_configureInternalAM ----------------------------------------*/
/*- for function aumg369xb_configureInternalFM ----------------------------------------*/
/*- for function aumg369xb_configureInternalPM ----------------------------------------*/
#define AUMG369XB_WAVEFORM_SINE_WAVE						1		
#define AUMG369XB_WAVEFORM_SQUARE_WAVE						2		
#define AUMG369XB_WAVEFORM_SQUARE_POSITIVE_RAMP				3		
#define AUMG369XB_WAVEFORM_SQUARE_NEGATIVE_RAMP				4		
#define AUMG369XB_WAVEFORM_SQUARE_GAUSSIAN_NOISE			5		
#define AUMG369XB_WAVEFORM_SQUARE_UNIFORM_NOISE				6		
#define AUMG369XB_WAVEFORM_SQUARE_TRIANGLE_WAVE				7		

/*- for function aumg369xb_configureInternalAM ----------------------------------------*/
/*- for function aumg369xb_configureExternalAM ----------------------------------------*/
#define AUMG369XB_AM_SCALING_LINEAR							0		
#define AUMG369XB_AM_SCALING_LOGARITHMIC					1		

/*- for function aumg369xb_configureInternalFM ----------------------------------------*/						
/*- for function aumg369xb_configureExternalFM ----------------------------------------*/
#define AUMG369XB_FM_MODE_LOCKED_LOW_NOISE					0		
#define AUMG369XB_FM_MODE_LOCKED							1		
#define AUMG369XB_FM_MODE_UNLOCKED_NARROW					2		
#define AUMG369XB_FM_MODE_UNLOCKED_WIDE						3		

/*- for function aumg369xb_configureInternalPM ----------------------------------------*/
/*- for function aumg369xb_configureExternalPM ----------------------------------------*/
#define AUMG369XB_PM_MODE_NARROW							0		
#define AUMG369XB_PM_MODE_WIDE								1		

/*- for function aumg369xb_configureInternalPulseModulation ---------------------------*/
/*- for function aumg369xb_configureExternalPulseModulation ---------------------------*/
#define AUMG369XB_RF_ON_POLARITY_LOW						0		
#define AUMG369XB_RF_ON_POLARITY_HIGH						1		

/*- for function aumg369xb_configureInternalPulseModulation ---------------------------*/
#define AUMG369XB_INTERNAL_PULSE_MODE_SINGLE				1		
#define AUMG369XB_INTERNAL_PULSE_MODE_DOUBLET				2		
#define AUMG369XB_INTERNAL_PULSE_MODE_TRIPLET				3		
#define AUMG369XB_INTERNAL_PULSE_MODE_QUADRUPLET			4		

#define AUMG369XB_TRIGGER_TYPE_FREE_RUN						1		
#define AUMG369XB_TRIGGER_TYPE_GATED						2		
#define AUMG369XB_TRIGGER_TYPE_DELAYED						3		
#define AUMG369XB_TRIGGER_TYPE_TRIGGERED					4		
#define AUMG369XB_TRIGGER_TYPE_TRIGGERED_WITH_DELAY			5		

#define AUMG369XB_TRIGGER_EDGE_TYPE_RISING					0		
#define AUMG369XB_TRIGGER_EDGE_TYPE_FALLING					1		

#define AUMG369XB_CLOCK_RATE_10MHZ							1		
#define AUMG369XB_CLOCK_RATE_40MHZ_100MHZ					4		

/*- for function aumg369xb_configureInternalPulseWidth --------------------------------*/
#define AUMG369XB_INTERNAL_PULSE_WIDTH_1					1		
#define AUMG369XB_INTERNAL_PULSE_WIDTH_2					2		
#define AUMG369XB_INTERNAL_PULSE_WIDTH_3					3		
#define AUMG369XB_INTERNAL_PULSE_WIDTH_4					4		

/*- for function aumg369xb_configureInternalPulseDelay --------------------------------*/
#define AUMG369XB_INTERNAL_PULSE_DELAY_1					1		
#define AUMG369XB_INTERNAL_PULSE_DELAY_2					2		
#define AUMG369XB_INTERNAL_PULSE_DELAY_3					3		
#define AUMG369XB_INTERNAL_PULSE_DELAY_4					4		

/*- for function aumg369xb_configureInternalSquareWavePulseModulation -----------------*/
#define AUMG369XB_SQUARE_WAVE_FREQUENCY_400_HZ				1		
#define AUMG369XB_SQUARE_WAVE_FREQUENCY_1000_HZ				2		
#define AUMG369XB_SQUARE_WAVE_FREQUENCY_7812_HZ				3		
#define AUMG369XB_SQUARE_WAVE_FREQUENCY_27800_HZ			4		

/*- for function aumg369xb_configureRearPanel -----------------------------------------*/
#define AUMG369XB_BLANKING_POLARITY_POSITIVE				0		
#define AUMG369XB_BLANKING_POLARITY_NEGATIVE				1		

#define AUMG369XB_MARKER_POLARITY_POSITIVE					0		
#define AUMG369XB_MARKER_POLARITY_NEGATIVE					1		

#define AUMG369XB_PENLIFT_RELAY_NORMALLY_OPEN               0       
#define AUMG369XB_PENLIFT_RELAY_NORMALLY_CLOSED             1      

#define AUMG369XB_EXTERNAL_REFERENCE_BANDWIDTH_10_HZ		0		
#define AUMG369XB_EXTERNAL_REFERENCE_BANDWIDTH_30_HZ		1		
#define AUMG369XB_EXTERNAL_REFERENCE_BANDWIDTH_100_HZ		2		
#define AUMG369XB_EXTERNAL_REFERENCE_BANDWIDTH_300_HZ		3		

/*- for function aumg369xb_configureGroupExecuteTrigger -------------------------------*/
#define AUMG369XB_EXECUTE_COMMAND_OFF						0		
#define AUMG369XB_EXECUTE_COMMAND_SQF						1		
#define AUMG369XB_EXECUTE_COMMAND_DN						2		
#define AUMG369XB_EXECUTE_COMMAND_FFSS						3		
#define AUMG369XB_EXECUTE_COMMAND_TSS						4		
#define AUMG369XB_EXECUTE_COMMAND_TRS						5		
#define AUMG369XB_EXECUTE_COMMAND_TST						6		
#define AUMG369XB_EXECUTE_COMMAND_UP						7		

/*- for function aumg369xb_scanPresetCWFrequency --------------------------------------*/
#define AUMG369XB_FREQUENCY_SCAN_DOWN_LOWER					0		
#define AUMG369XB_FREQUENCY_SCAN_UP_HIGHER					1		
#define AUMG369XB_FREQUENCY_SCAN_NEXT_HIGHER				2       

/*- for function aumg369xb_writeInstrumentSetup ---------------------------------------*/
/*- for function aumg369xb_readInstrumentSetup ----------------------------------------*/
#define AUMG369XB_INSTRUMENT_SETUP_CURRENT					0		
#define AUMG369XB_INSTRUMENT_SETUP_ALL						1		

/*- for function aumg369xb_configureUserLevelCalibration ------------------------------*/
#define AUMG369XB_CALIBRATION_TABLE_OFF						0		
#define AUMG369XB_CALIBRATION_TABLE_1						1		
#define AUMG369XB_CALIBRATION_TABLE_2						2		
#define AUMG369XB_CALIBRATION_TABLE_3						3		
#define AUMG369XB_CALIBRATION_TABLE_4						4		
#define AUMG369XB_CALIBRATION_TABLE_5						5		

/***************************************************************************************/
/*= GLOBAL USER-CALLABLE FUNCTION DECLARATIONS (Exportable Functions) =================*/
/***************************************************************************************/

/*-------------------------------------------------------------------------------------*/	
/*- Initialize and Close Functions ----------------------------------------------------*/
/*-------------------------------------------------------------------------------------*/
ViStatus _VI_FUNC aumg369xb_init (ViRsrc resourceName, 
					ViBoolean IDQuery, ViBoolean resetDevice, 
					ViSession *InstrumentHandle);

ViStatus _VI_FUNC aumg369xb_close (ViSession vi);

/*-------------------------------------------------------------------------------------*/
/*- Configuration Functions -----------------------------------------------------------*/
/*-------------------------------------------------------------------------------------*/

	/*- RF Configuration --------------------------------------------------------------*/
ViStatus _VI_FUNC aumg369xb_configureOutputEnabled (ViSession vi,
					ViBoolean outputEnabled);

ViStatus _VI_FUNC aumg369xb_configureRF (ViSession vi, 
					ViBoolean retraceRFState, ViBoolean DeltaFRFState,
					ViInt32 rampRest, ViBoolean resetState, 
					ViReal64 frequencyScaling, ViBoolean terminateRFState);

	/*- Frequency Configuration -------------------------------------------------------*/
ViStatus _VI_FUNC aumg369xb_configureFrequencyList (ViSession vi, 
					ViReal64 frequencyList[20]);

ViStatus _VI_FUNC aumg369xb_configureFrequencyEntry (ViSession vi, 
					ViInt32 selectedFrequency, ViReal64 frequency);

ViStatus _VI_FUNC aumg369xb_configureDeltaFrequency (ViSession vi,
					ViReal64 deltaFrequency);

	/*- CW Frequency Configuration ----------------------------------------------------*/
ViStatus _VI_FUNC aumg369xb_configureCWFrequency (ViSession vi,
					ViInt32 selectedFrequency, ViBoolean phaseOffsetEnabled, 
					ViReal64 phaseOffset, ViBoolean CWRampEnabled);

ViStatus _VI_FUNC aumg369xb_configureFastFrequencySwitching (ViSession vi,
					ViInt32 tableLocation, ViInt32 numberOfFrequency,
					ViReal64 frequencyList[], ViInt32 pointerLocation);

	/*- Sweep Frequency Configuration -------------------------------------------------*/
ViStatus _VI_FUNC aumg369xb_configureAnalogSweepFrequency (ViSession vi, 
					ViInt32 sweepRange, ViReal64 sweepTime, ViInt32 triggerSource);

ViStatus _VI_FUNC aumg369xb_configureStepSweepFrequency (ViSession vi,  
					ViInt32 sweepRange, ViInt32 stepSweepMode, ViReal64 dwellTime, 
					ViInt32 numberOfSteps, ViInt32 triggerSource);

ViStatus _VI_FUNC aumg369xb_configureStepSweepSweepTime (ViSession vi,
					ViReal64 sweepTime);

ViStatus _VI_FUNC aumg369xb_configureAlternateSweepFrequency (ViSession vi, 
					ViInt32 sweepRange, ViInt32 selectedPowerLevel);

ViStatus _VI_FUNC aumg369xb_configureDualStepSweepEnabled (ViSession vi,
					ViBoolean enabled);

ViStatus _VI_FUNC aumg369xb_configureNonEquallySpacedStepSweepEnabled (ViSession vi,
					ViBoolean enabled);

ViStatus _VI_FUNC aumg369xb_configureManualSweepFrequency (ViSession vi, 
					ViInt32 sweepRange, ViInt32 numberOfSteps);

ViStatus _VI_FUNC aumg369xb_configureListSweepFrequency (ViSession vi, 
					ViInt32 listNumber, ViInt32 startIndex, ViInt32 stopIndex, 
					ViReal64 dwellTime, ViInt32 triggerSource);

ViStatus _VI_FUNC aumg369xb_configureSweepList (ViSession vi, 
					ViInt32 listNumber, ViInt32 listIndex, 
					ViInt32 numberOfFrequency, ViReal64 frequencyList[], 
					ViInt32 numberOfPowerLevel, ViReal64 powerLevelList[]);

	/*- Master Slave Configuration ----------------------------------------------------*/
ViStatus _VI_FUNC aumg369xb_configureMasterSlaveOperation (ViSession vi,
					ViBoolean masterSlaveEnabled, ViReal64 slaveDeltaFrequency, 
					ViReal64 slaveMainPowerLevel, ViReal64 slaveAlternateSweepPowerLevel);

ViStatus _VI_FUNC aumg369xb_configureSlaveFrequencyList (ViSession vi,
					ViReal64 frequencyList[20]);

ViStatus _VI_FUNC aumg369xb_configureSlaveFrequencyEntry (ViSession vi,
					ViInt32 selectedFrequency, ViReal64 frequency);

ViStatus  _VI_FUNC aumg369xb_configureVNAMode (ViSession vi,
					ViReal64  VNAOffset);

	/*- Frequency Marker Configuration ------------------------------------------------*/
ViStatus _VI_FUNC aumg369xb_configureFrequencyMarkerEnabled (ViSession vi,
					ViInt32 activeFrequency, ViBoolean frequencyMarkerEnabled);

ViStatus _VI_FUNC aumg369xb_configureFrequencyMarkerMode (ViSession vi,
					ViInt32 frequencyMarkerMode);

	/*- Power Level Configuration -----------------------------------------------------*/
ViStatus _VI_FUNC aumg369xb_configurePowerLevelUnit (ViSession vi,
					ViInt32 powerLevelUnit);

ViStatus _VI_FUNC aumg369xb_configurePowerLevelList (ViSession vi, 
					ViReal64 powerLevelList[10]);

ViStatus _VI_FUNC aumg369xb_configurePowerLevelEntry (ViSession vi, 
					ViInt32 selectedPowerLevel, ViReal64 powerLevel);

ViStatus _VI_FUNC aumg369xb_configureFixedPowerLevel (ViSession vi, 
					ViInt32 selectedPowerLevel, ViBoolean levelOffsetEnabled, 
					ViReal64 levelOffset);

ViStatus _VI_FUNC aumg369xb_configure1dBBelowPeakLeveledPower (ViSession vi);

ViStatus _VI_FUNC aumg369xb_configurePowerLevelSweep (ViSession vi,
					ViReal64 dwellTime, ViInt32 numberOfSteps, ViInt32 triggerSource);

ViStatus _VI_FUNC aumg369xb_configureALCMode (ViSession vi,
					ViInt32 levelingMode, ViBoolean decoupleEnabled, 
					ViInt32 attenuation, ViInt32 referenceLevelDAC);

ViStatus _VI_FUNC aumg369xb_configureALCLoop (ViSession vi, 
					ViReal64 pivot, ViBoolean slopeEnabled, ViInt32 slope);

ViStatus _VI_FUNC aumg369xb_configurePowerOffsetTable (ViSession vi,
					ViBoolean powerOffsetTableEnabled, ViInt32 numberOfPowerOffset, 
					ViReal64 powerOffsetList[]);

	/*- Modulation Configuration ------------------------------------------------------*/
ViStatus _VI_FUNC aumg369xb_configureInternalAM (ViSession vi,
					ViBoolean InternalAMEnabled, ViInt32 scaling, ViReal64 depth, 
					ViReal64 rate, ViInt32 waveform);

ViStatus _VI_FUNC aumg369xb_configureExternalAM (ViSession vi,
					ViBoolean ExternalAMEnabled, ViInt32 scaling, ViReal64 sensitivity);

ViStatus _VI_FUNC aumg369xb_configureInternalFM (ViSession vi,
					ViBoolean InternalFMEnabled, ViInt32 FMMode, ViReal64 deviation, 
					ViReal64 rate, ViInt32 waveform);

ViStatus _VI_FUNC aumg369xb_configureExternalFM (ViSession vi, 
					ViBoolean ExternalFMEnabled, ViInt32 FMMode, ViReal64 sensitivity);

ViStatus _VI_FUNC aumg369xb_configureInternalPM (ViSession vi,
					ViBoolean InternalPMEnabled, ViInt32 PMMode, ViReal64 deviation, 
					ViReal64 rate, ViInt32 waveform);

ViStatus _VI_FUNC aumg369xb_configureExternalPM (ViSession vi,
					ViBoolean ExternalPMEnabled, ViInt32 PMMode, ViReal64 sensitivity);

ViStatus _VI_FUNC aumg369xb_configureInternalPulseModulation (ViSession vi,
					ViBoolean InternalPulseModulationEnabled, ViReal64 period, ViInt32 mode, 
					ViInt32 triggerType, ViInt32 triggerEdgeType, 
					ViInt32 RFOnPolarity, ViInt32 clockRate);

ViStatus _VI_FUNC aumg369xb_configureInternalPulseModulationFrequency (ViSession vi,
					ViReal64 frequency);

ViStatus _VI_FUNC aumg369xb_configureInternalPulseWidth (ViSession vi,
					ViInt32 selectedPulseWidth, ViReal64 pulseWidth);

ViStatus _VI_FUNC aumg369xb_configureInternalPulseDelay (ViSession vi,
					ViInt32 selectedPulseDelay, ViReal64 pulseDelay);

ViStatus _VI_FUNC aumg369xb_configureInternalPulseStepDelay (ViSession vi,
					ViBoolean enabled, ViReal64 startDelay, ViReal64 stopDelay, 
					ViReal64 stepSize, ViReal64 stepTime);

ViStatus _VI_FUNC aumg369xb_configureInternalSquareWavePulseModulation (ViSession vi,
					ViInt32 squareWaveFrequency);

ViStatus _VI_FUNC aumg369xb_configureExternalPulseModulation (ViSession vi,
					ViBoolean ExternalPulseEnabled, ViInt32 RFOnPolarity);

ViStatus _VI_FUNC aumg369xb_configureSCANModulationEnabled (ViSession vi,
					ViBoolean enabled);

	/*- Measurement Configuration -----------------------------------------------------*/
ViStatus _VI_FUNC aumg369xb_configureAMMeasurementEnabled (ViSession vi,
					ViBoolean enabled);

ViStatus _VI_FUNC aumg369xb_configureFMMeasurementEnabled (ViSession vi,
					ViBoolean enabled);

ViStatus _VI_FUNC aumg369xb_configurePowerMeterMeasurementEnabled (ViSession vi,
					ViBoolean enabled);

	/*- System Configuration ----------------------------------------------------------*/
ViStatus _VI_FUNC aumg369xb_configureRearPanel (ViSession vi, 
					ViInt32 blankingPolarity, ViInt32 markerPolarity,
					ViInt32 penliftRelay, ViInt32 externalReferenceBandwidth);

ViStatus _VI_FUNC aumg369xb_configureSecureModeEnabled (ViSession vi, 
					ViBoolean enabled);

ViStatus _VI_FUNC aumg369xb_configureGroupExecuteTrigger (ViSession vi,
					ViInt32 executeCommand);

/*-------------------------------------------------------------------------------------*/
/*- Action/Status Functions -----------------------------------------------------------*/
/*-------------------------------------------------------------------------------------*/
ViStatus _VI_FUNC aumg369xb_scanPresetCWFrequency (ViSession vi, 
					ViInt32 scanFrequencyType);

ViStatus _VI_FUNC aumg369xb_zeroPhaseOffsetDisplay (ViSession vi);

ViStatus _VI_FUNC aumg369xb_calculateSweepList (ViSession vi);

ViStatus _VI_FUNC aumg369xb_setInternalTriggeredPulseWithDelay (ViSession vi);

ViStatus _VI_FUNC aumg369xb_setInternalGatedPulse (ViSession vi);

ViStatus _VI_FUNC aumg369xb_setInternal1kHzSquareWavePulse (ViSession vi);

ViStatus _VI_FUNC aumg369xb_performMasterReset (ViSession vi);

ViStatus _VI_FUNC aumg369xb_triggerSingleSweep (ViSession vi);

ViStatus _VI_FUNC aumg369xb_resetSweep (ViSession vi);

ViStatus _VI_FUNC aumg369xb_stepToNextSweep (ViSession vi);

ViStatus _VI_FUNC aumg369xb_sendGETBusMessage (ViSession vi);

/*-------------------------------------------------------------------------------------*/
/*- Utility Functions -----------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------*/
ViStatus _VI_FUNC aumg369xb_reset (ViSession vi);

ViStatus _VI_FUNC aumg369xb_selfTest (ViSession vi, 
					ViInt32 *testResult, ViByte testResultBytes[6]);

ViStatus _VI_FUNC aumg369xb_errorQuery (ViSession vi,	
					ViChar errorMessage[256]);

ViStatus _VI_FUNC aumg369xb_errorMessage (ViSession vi, 
					ViStatus statusCode, ViChar message[256]);

ViStatus _VI_FUNC aumg369xb_revisionQuery (ViSession vi,
					ViChar driverRev[256], ViChar buildRev[256], 
					ViChar instrRev[256]);

ViStatus _VI_FUNC aumg369xb_queryModelNumber (ViSession vi,
					ViInt32 *modelNumber);

ViStatus _VI_FUNC aumg369xb_configureSerialNumber (ViSession vi,
					ViChar serialNumber[]);

ViStatus _VI_FUNC aumg369xb_returnLocalControl (ViSession vi);

/*-------- Instrument Setup Configuration ------------------------------------------------*/
ViStatus _VI_FUNC aumg369xb_saveInstrumentSetup (ViSession vi,
					ViInt32 setupNumber);

ViStatus _VI_FUNC aumg369xb_recallInstrumentSetup (ViSession vi,
					ViInt32 setupNumber);

ViStatus _VI_FUNC aumg369xb_recallNextInstrumentSetup (ViSession vi);

ViStatus _VI_FUNC aumg369xb_writeInstrumentSetup (ViSession vi,
					ViInt32 instrumentSetupType, ViInt32 arrayLength, 
					ViByte instrumentSetup[]);

ViStatus _VI_FUNC aumg369xb_readInstrumentSetup (ViSession vi,
					ViInt32 instrumentSetupType, ViInt32 arrayLength, 
					ViByte instrumentSetup[], ViInt32 *returnLength);

/*-------- User Level Calibration Configuration ------------------------------------------*/
ViStatus _VI_FUNC aumg369xb_configureUserLevelCalibration (ViSession vi,
					ViInt32 calibrationTableNumber);

ViStatus _VI_FUNC aumg369xb_writeUserLevelCalibrationTable (ViSession vi,
					ViInt32 arrayLength, ViByte calibrationData[]);

ViStatus _VI_FUNC aumg369xb_readUserLevelCalibrationTable (ViSession vi,
					ViInt32 arrayLength,  ViByte calibrationData[], 
					ViInt32 *returnLength);

#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

/***************************************************************************************/
/*=== END INCLUDE FILE ================================================================*/
/***************************************************************************************/

#endif
