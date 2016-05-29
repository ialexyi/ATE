/*= HP 8563E-Series Spectrum Analyzer =======================================*/

/*===========================================================================*/
/*  Please do not use global variables or arrays in the include file of      */
/*  instrument drivers that will be submitted for inclusion into the         */
/*  LabWindows Instrument Driver Library.                                    */
/*===========================================================================*/
     
#ifndef __hp8563e_HEADER
#define __hp8563e_HEADER

#include <vpptype.h>

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

/*****************************************************************************/
/*= Define Instrument Specific Error/Warning Codes Here =====================*/
/*****************************************************************************/
#define VI_ERROR_INSTR_FILE_OPEN                (_VI_ERROR+0x3FFC0800L)
#define VI_ERROR_INSTR_FILE_WRITE               (_VI_ERROR+0x3FFC0801L)
#define VI_ERROR_INSTR_INTERPRETING_RESPONSE    (_VI_ERROR+0x3FFC0803L)
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

#define hp8563e_ERROR_INVALID_CONFIGURATION     (VI_ERROR_INSTR_OFFSET + 0xF0L)
#define hp8563e_ERROR_NO_OUTPUT_AVAILABLE       (VI_ERROR_INSTR_OFFSET + 0x05L)
#define hp8563e_ERROR_TIMEOUT_OCCUR             (VI_ERROR_INSTR_OFFSET + 0x15L)

/*****************************************************************************/
/*= GLOBAL USER-CALLABLE FUNCTION DECLARATIONS (Exportable Functions) =======*/
/*****************************************************************************/
ViStatus _VI_FUNC hp8563e_init (ViRsrc resourceName, ViBoolean IDQuery,
                    ViBoolean resetDevice, ViPSession instrSession);
ViStatus _VI_FUNC hp8563e_makeBasicMeas (ViSession instrumentHandle,
                    ViReal64 centerFrequencyHz, ViReal64 spanHz, 
                    ViBoolean markerRefLvl, ViPReal64 referenceLevel,
                    ViPReal64 markerFrequency, ViPReal64 markerAmplitude,
                    ViReal64 _VI_FAR signalTrace[]);
ViStatus _VI_FUNC hp8563e_configFreq (ViSession instrumentHandle,
                    ViInt16 frequencyType, ViReal64 frequencyValueHz);
ViStatus _VI_FUNC hp8563e_configSpan (ViSession instrumentHandle,
                    ViInt16 frequencySpanType, ViReal64 frequencySpanHz);
ViStatus _VI_FUNC hp8563e_confAmpltde1 (ViSession instrumentHandle,
                    ViInt16 amplitudeScale, ViReal64 referenceLeveldBmmV);
ViStatus _VI_FUNC hp8563e_confAmpltde2 (ViSession instrumentHandle,
                    ViReal64 amplitudeReferenceOffsetdB, ViInt16 mixerLeveldB);
ViStatus _VI_FUNC hp8563e_confAmpltde3 (ViSession instrumentHandle,
                    ViReal64 normalizedRefPositiondB, ViInt16 preselectorDACNumber);
ViStatus _VI_FUNC hp8563e_confCalibratn (ViSession instrumentHandle,
                    ViBoolean automaticIFAdjustment,
                    ViBoolean freqAmplitudeCorrection);
ViStatus _VI_FUNC hp8563e_confRefLvl (ViSession instrumentHandle,
                    ViInt16 referenceLevelCalibration);
ViStatus _VI_FUNC hp8563e_configFreqAmplCorrectn (ViSession instrumentHandle,
                    ViInt16 numberofCorrectionPoints,
                    ViReal64 _VI_FAR frequencyArrayHz[],
                    ViReal64 _VI_FAR amplitudeArraydB[]);
ViStatus _VI_FUNC hp8563e_confTrkGen (ViSession instrumentHandle,
                    ViBoolean sweepCouple);
ViStatus _VI_FUNC hp8563e_confNorm (ViSession instrumentHandle, ViBoolean normalize,
                    ViReal64 normalizedRefLeveldB,
                    ViReal64 normalizedRefPositiondB);
ViStatus _VI_FUNC hp8563e_confVidAvg (ViSession instrumentHandle,
                    ViBoolean videoAverage, ViInt16 videoAverageValue);
ViStatus _VI_FUNC hp8563e_confMix (ViSession instrumentHandle, ViBoolean mixerMode,
                    ViBoolean externalMixingMode);
ViStatus _VI_FUNC hp8563e_confExternMix (ViSession instrumentHandle,
                    ViInt16 fullBand, ViReal64 conversionLossdB,
                    ViReal64 mixerBiasmA);
ViStatus _VI_FUNC hp8563e_confHarmNumLock (ViSession instrumentHandle,
                    ViInt16 harmonicNumberLock);
ViStatus _VI_FUNC hp8563e_confSignIDandPresel (ViSession instrumentHandle,
                    ViInt16 signalIdentification, ViInt16 preselectorDACNumber);
ViStatus _VI_FUNC hp8563e_configDemod (ViSession instrumentHandle,
                    ViInt16 demodulation, ViReal64 demodulationTime, 
                    ViBoolean squelch, ViReal64 squelchLeveldBm,
                    ViBoolean demodAutomaticGainControl);
ViStatus _VI_FUNC hp8563e_confRearPanel (ViSession instrumentHandle,
                    ViBoolean frequencyReference10MHz, ViInt16 sweepOutput);
ViStatus _VI_FUNC hp8563e_confChanPow (ViSession instrumentHandle,
                    ViBoolean powerType, ViReal64 powerValueHz);
ViStatus _VI_FUNC hp8563e_confACPmain (ViSession instrumentHandle, ViInt16 method,
                    ViInt16 accelerator, ViBoolean measurementState,
                    ViBoolean graphMode);
ViStatus _VI_FUNC hp8563e_confACPchan (ViSession instrumentHandle,
                    ViReal64 channelSpacingHz, ViReal64 channelBandwidthHz,
                    ViInt16 alternateChannels);
ViStatus _VI_FUNC hp8563e_confACPburst (ViSession instrumentHandle,
                    ViReal64 burstPeriodms, ViReal64 burstWidthms);
ViStatus _VI_FUNC hp8563e_confACPweigth (ViSession instrumentHandle,
                    ViBoolean frequencyWeighting, ViReal64 TWeightingms,
                    ViReal64 alphaWeighting);
ViStatus _VI_FUNC hp8563e_confMkr (ViSession instrumentHandle,
                    ViBoolean markerNoise, ViBoolean signalTrack);
ViStatus _VI_FUNC hp8563e_confFreqCnt (ViSession instrumentHandle,
                    ViBoolean frequencyCounter, ViInt16 frequencyCounterResolution);
ViStatus _VI_FUNC hp8563e_confPeakSearch (ViSession instrumentHandle,
                    ViReal64 markerThresholddB, ViReal64 peakExcursiondBdiv);
ViStatus _VI_FUNC hp8563e_confSweep (ViSession instrumentHandle,
                    ViBoolean sweepTime, ViReal64 sweepTimeValuems,
                    ViBoolean sweepMode, ViBoolean delaySweep,
                    ViReal64 delaySweepValuems);
ViStatus _VI_FUNC hp8563e_confVideoGate (ViSession instrumentHandle, ViBoolean gate,
                    ViReal64 gateDelayms, ViReal64 gateLengthms,
                    ViBoolean edgeGatePolarity, ViBoolean gateControl);
ViStatus _VI_FUNC hp8563e_configTrig (ViSession instrumentHandle,
                    ViBoolean sweepMode, ViInt16 triggerMode,
                    ViReal64 videoTriggerLeveldBm, ViBoolean triggerPolarity);
ViStatus _VI_FUNC hp8563e_configCouplRat (ViSession instrumentHandle,
                    ViBoolean ratioType, ViReal64 ratio);
ViStatus _VI_FUNC hp8563e_configCoupl (ViSession instrumentHandle, ViInt16 control,
                    ViBoolean autoManual, ViReal64 valueHzmsdB);
ViStatus _VI_FUNC hp8563e_confTrace (ViSession instrumentHandle,
                    ViInt16 detectionMode, ViBoolean ABA, ViBoolean ABDLA);
ViStatus _VI_FUNC hp8563e_confWin (ViSession instrumentHandle, ViInt16 window);
ViStatus _VI_FUNC hp8563e_configDisplay (ViSession instrumentHandle,
                    ViBoolean displayLine, ViReal64 displayLineLeveldBm,
                    ViBoolean threshold, ViReal64 thresholdLeveldBm,
                    ViBoolean graticule, ViBoolean annotation);
ViStatus _VI_FUNC hp8563e_confTitle (ViSession instrumentHandle,
                    ViChar _VI_FAR screenTitle[]);
ViStatus _VI_FUNC hp8563e_peakPresel (ViSession instrumentHandle);
ViStatus _VI_FUNC hp8563e_perfmCalibr (ViSession instrumentHandle,
                    ViInt16 calibration);
ViStatus _VI_FUNC hp8563e_perfmSwp (ViSession instrumentHandle);
ViStatus _VI_FUNC hp8563e_perfmFourierT (ViSession instrumentHandle,
                    ViBoolean destination, ViBoolean source, ViBoolean window);
ViStatus _VI_FUNC hp8563e_perfmACP (ViSession instrumentHandle, ViInt16 command);
ViStatus _VI_FUNC hp8563e_perfmTrace (ViSession instrumentHandle, ViInt16 command,
                    ViBoolean trace);
ViStatus _VI_FUNC hp8563e_placeMarker (ViSession instrumentHandle, ViInt16 marker,
                    ViReal64 atHzms);
ViStatus _VI_FUNC hp8563e_perfmTo (ViSession instrumentHandle,
                    ViInt16 whichToCommand);
ViStatus _VI_FUNC hp8563e_shiftCF (ViSession instrumentHandle, ViBoolean upDown);
ViStatus _VI_FUNC hp8563e_genQueries (ViSession instrumentHandle, ViInt16 queryType,
                    ViPInt32 output);
ViStatus _VI_FUNC hp8563e_queryACP (ViSession instrumentHandle,
                    ViInt16 ACPQueryType, ViPReal64 measurementResult);
ViStatus _VI_FUNC hp8563e_queryACPchannels (ViSession instrumentHandle,
                    ViReal64 _VI_FAR measurementsArray[],
                    ViPInt16 numofValidMeasurements);
ViStatus _VI_FUNC hp8563e_queryAmpl (ViSession instrumentHandle,
                    ViInt16 amplitudeType, ViPReal64 amplitudeMeasurement);
ViStatus _VI_FUNC hp8563e_queryBW (ViSession instrumentHandle, ViInt16 bandwithType,
                    ViInt16 powerLeveldB,
                    ViPReal64 bandwidthMeasurement);
ViStatus _VI_FUNC hp8563e_queryFreq (ViSession instrumentHandle,
                    ViInt16 frequencyType, ViPReal64 frequencyMeasurement);
ViStatus _VI_FUNC hp8563e_querySwpTime (ViSession instrumentHandle,
                    ViPReal64 sweepTime);
ViStatus _VI_FUNC hp8563e_queryTrace (ViSession instrumentHandle, ViBoolean trace,
                    ViReal64 _VI_FAR traceMeasurementArray[]);
ViStatus _VI_FUNC hp8563e_queryCarrierPwr (ViSession instrumentHandle,
                    ViBoolean carrier, ViBoolean trace, ViPReal64 powerMeasurement);
ViStatus _VI_FUNC hp8563e_queryPwr (ViSession instrumentHandle,
                    ViInt16 powerMeasurementType, ViBoolean trace,
                    ViReal64 rangeHzdB, ViPReal64 powerMeasurement);
ViStatus _VI_FUNC hp8563e_saveRecall (ViSession instrumentHandle,
                    ViBoolean saveRecall, ViInt16 typeofRegister,
                    ViInt16 registerNumber);
ViStatus _VI_FUNC hp8563e_storeRecallRef (ViSession instrumentHandle,
                    ViInt16 command);
ViStatus _VI_FUNC hp8563e_waitUntilDone (ViSession instrumentHandle,
                    ViInt32 timeOutms);
ViStatus _VI_FUNC hp8563e_writeInstrData (ViSession instrumentHandle,
                    ViString writeBuffer);
ViStatus _VI_FUNC hp8563e_readInstrData (ViSession instrumentHandle,
                    ViInt32 numberBytesToRead, ViChar _VI_FAR readBuffer[],
                    ViPInt32 numBytesRead);
ViStatus _VI_FUNC hp8563e_reset (ViSession instrumentHandle);
ViStatus _VI_FUNC hp8563e_selfTest (ViSession instrumentHandle,
                    ViPInt16 selfTestResult, ViChar _VI_FAR selfTestMessage[]);
ViStatus _VI_FUNC hp8563e_errorQuery (ViSession instrumentHandle,
                    ViPInt32 errorCode, ViChar _VI_FAR errorMessage[]);
ViStatus _VI_FUNC hp8563e_errorMessage (ViSession instrumentHandle,
                    ViStatus errorCode, ViChar _VI_FAR errorMessage[]);
ViStatus _VI_FUNC hp8563e_revisionQuery (ViSession instrumentHandle,
                    ViChar _VI_FAR instrumentDriverRevision[],
                    ViChar _VI_FAR firmwareRevision[]);
ViStatus _VI_FUNC hp8563e_close (ViSession instrumentHandle);

#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

/*****************************************************************************/
/*=== END INCLUDE FILE ======================================================*/
/*****************************************************************************/

#endif
