
/***************************************************************************** 

/*****************************************************************************
 *  Anritsu ML248x / ML249x IVI Driver Instrument Driver                               
 *  LabWindows/CVI Instrument Driver                                     
 *  Original Release: 2008/07/07
 *  By: Liping Zhao, National Instruments                              
 *      PH. (8621)50509810x3196   Fax (8621)68795616                              
 *                                                                           
 *  Modification History: 
 *       Rev  1.1, 2009/09/16
 *	      Modified appending '\n' character to all commands sent through TCPIP interfaces.
 *        Enabled termination character for commands reading through TCPIP interfaces.
 *                                                                  
 *       2008/07/07 - Instrument Driver Created.                  
 *                                                                           
 *****************************************************************************/

#include "toolbox.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <formatio.h>
#include <ansi_c.h> 

#include "auml24xx.h"

/*****************************************************************************
 *--------------------- Hidden Attribute Declarations -----------------------*
 *****************************************************************************/

#define AUML24XX_ATTR_OPC_TIMEOUT                                                      (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 1L)       /* ViInt32 */
#define AUML24XX_ATTR_USE_SPECIFIC_SIMULATION                                          (IVI_INHERENT_ATTR_BASE + 23L)              /* ViBoolean */
#define AUML24XX_ATTR_VISA_RM_SESSION                                                  (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 5L)       /* ViSession */
#define AUML24XX_ATTR_IO_SESSION                                                       (IVI_INHERENT_ATTR_BASE + 322L)             /* ViSession */
#define AUML24XX_ATTR_OPC_CALLBACK                                                     (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 6L)       /* ViAddr */
#define AUML24XX_ATTR_CHECK_STATUS_CALLBACK                                            (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 7L)       /* ViAddr */							    
#define AUML24XX_ATTR_USER_INTERCHANGE_CHECK_CALLBACK                                  (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 8L)       /* ViAddr */

 /*- Averaging Attributes -----------------------*/ 
#define AUML24XX_ATTR_INSTR_CW_AVERAGING_ENABLED                                      (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 101L)	   /* ViBoolean  */
#define AUML24XX_ATTR_INSTR_CW_AVERAGING_NUMBER                                       (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 102L)	   /* ViInt32  */
#define AUML24XX_ATTR_INSTR_PMOD_AVERAGING_NUMBER                                     (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 103L) 	   /* ViInt32  */
#define AUML24XX_ATTR_INSTR_PMOD_AVERAGING_ENABLED                                    (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 104L) 	   /* ViBoolean  */

/*- Trigger Attributes -----------------------*/  
#define AUML24XX_ATTR_INSTR_PMOD_TRIGGER_CAPTURE_TIME                                 (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 201L)		   /* ViReal64  */ 
#define AUML24XX_ATTR_INSTR_CW_TRIGGER_CAPTURE_TIME                                   (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 202L)	   /* ViReal64  */
#define AUML24XX_ATTR_INSTR_PMOD_TRIGGER_ARMING_MODE                                  (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 203L)	   /* ViInt32  */
#define AUML24XX_ATTR_INSTR_CW_TRIGGER_ARMING_MODE                                    (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 204L)	   /* ViInt32  */
#define AUML24XX_ATTR_INSTR_PMOD_TRIGGER_INTERNAL_SLOPE                               (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 205L)	   /* ViInt32  */
#define AUML24XX_ATTR_INSTR_CW_TRIGGER_INTERNAL_SLOPE                                 (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 206L)	   /* ViInt32  */
#define AUML24XX_ATTR_INSTR_PMOD_TRIGGER_INTERNAL_LEVEL                               (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 207L)	   /* ViReal64  */
#define AUML24XX_ATTR_INSTR_CW_TRIGGER_INTERNAL_LEVEL                                 (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 208L)	   /* ViReal64  */
#define AUML24XX_ATTR_INSTR_PMOD_TRIGGER_DELAY_TIME                                   (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 209L)	   /* ViReal64  */
#define AUML24XX_ATTR_INSTR_CW_TRIGGER_DELAY_TIME                                     (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 210L)	   /* ViReal64  */
#define AUML24XX_ATTR_INSTR_PMOD_TRIGGER_SOURCE                                       (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 211L)	   /* ViInt32  */
#define AUML24XX_ATTR_INSTR_CW_TRIGGER_SOURCE                                         (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 212L)	   /* ViInt32  */
#define AUML24XX_ATTR_INSTR_PMOD_INTERNAL_TRIGGER_EVENT_SOURCE                        (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 213L) 	   /* ViString  */
#define AUML24XX_ATTR_INSTR_CW_INTERNAL_TRIGGER_EVENT_SOURCE                          (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 214L)  	   /* ViString  */

/*- Instrument Model Attributes -----------------------*/  
#define AUML24XX_ATTR_INSTR_MODEL_TYPE                                                (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 301L) 	   /* ViInt32  */
/*****************************************************************************
 *---------------------------- Useful Macros --------------------------------*
 *****************************************************************************/

    /*- I/O buffer size -----------------------------------------------------*/
#define BUFFER_SIZE                             512L 
#define MEAS_BUFFER_SIZE                        4096L    


    /*- 488.2 Event Status Register (ESR) Bits ------------------------------*/
#define IEEE_488_2_ERROR_MASK              0x3C

    /*- List of channels passed to the Ivi_BuildChannelTable function -------*/ 
#define CHANNEL_LIST_DUAL                            "1,2"
#define CHANNEL_LIST_SINGLE                          "1"

#define SENSOR_REP_CAP_NAME                          "Sensor"
#define SENSOR_LIST_DUAL                             "A,B"
#define SENSOR_LIST_SINGLE                           "A"

#define BNC_REP_CAP_NAME                             "BNC"
#define BNC_LIST                                     "1,2" 

static IviRangeTableEntry attrPmodMeasurementDisplayRangeTableEntries[] =
	{
		{AUML24XX_VAL_PMOD_PRF, 0, 0, "PRF", 0},
		{AUML24XX_VAL_PMOD_RDO, 0, 0, "RDO", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrPmodMeasurementDisplayRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrPmodMeasurementDisplayRangeTableEntries,
	};

static IviRangeTableEntry attrChannelCountRangeTableEntries[] =
	{
		{1, 2, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrChannelCountRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrChannelCountRangeTableEntries,
	};

static IviRangeTableEntry attrActiveCalibrationFactorNumberRangeTableEntries[] =
	{
		{1, 20, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrActiveCalibrationFactorNumberRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrActiveCalibrationFactorNumberRangeTableEntries,
	};

static IviRangeTableEntry attrRrsTraceStateRangeTableEntries[] =
	{
		{AUML24XX_VAL_RRS_FALSE, 0, 0, "FALSE", 0},
		{AUML24XX_VAL_RRS_FULL, 0, 0, "FULL", 0},
		{AUML24XX_VAL_RRS_PARTIAL, 0, 0, "PARTIAL", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrRrsTraceStateRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrRrsTraceStateRangeTableEntries,
	};

static IviRangeTableEntry attrActiveChannelRangeTableEntries[] =
	{
		{AUML24XX_VAL_CHAN1, 2, 1, "1", 0},
		{AUML24XX_VAL_CHAN2, 2, 1, "2", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrActiveChannelRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrActiveChannelRangeTableEntries,
	};

static IviRangeTableEntry attrPaeBiasCurrentConversionFactorRangeTableEntries[] =
	{
		{0.001, 100, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrPaeBiasCurrentConversionFactorRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrPaeBiasCurrentConversionFactorRangeTableEntries,
	};

static IviRangeTableEntry attrLimitComplexSpecificationTypeRangeTableEntries[] =
	{
		{AUML24XX_VAL_LIMIT_SPEC_USER, 0, 0, "", 0},
		{AUML24XX_VAL_LIMIT_SPEC_PDEF, 0, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrLimitComplexSpecificationTypeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrLimitComplexSpecificationTypeRangeTableEntries,
	};

static IviRangeTableEntry attrInstrPMODTriggerDelayTimeRangeTableEntries[] =
	{
		{-1, 600, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrInstrPMODTriggerDelayTimeRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrInstrPMODTriggerDelayTimeRangeTableEntries,
	};

static IviRangeTableEntry attrInstrCWTriggerDelayTimeRangeTableEntries[] =
	{
		{0, 0.999, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrInstrCWTriggerDelayTimeRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrInstrCWTriggerDelayTimeRangeTableEntries,
	};

static IviRangeTableEntry attrStatPostProcessingMarkerPositionRangeTableEntries[] =
	{
		{-999.99, 999.99, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrStatPostProcessingMarkerPositionRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrStatPostProcessingMarkerPositionRangeTableEntries,
	};

static IviRangeTableEntry attrCWTriggerArmingModeRangeTableEntries[] =
	{
		{AUML24XX_VAL_TRIGGER_ARMING_MODE_AUTO, 0, 0, "AUTO", 0},
		{AUML24XX_VAL_TRIGGER_ARMING_MODE_SINGLE, 0, 0, "SINGLE", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrCWTriggerArmingModeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrCWTriggerArmingModeRangeTableEntries,
	};

static IviRangeTableEntry attrCaptureTimeRangeTableEntries[] =
	{
		{5e-8, 7, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrCaptureTimeRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrCaptureTimeRangeTableEntries,
	};

static IviRangeTableEntry attrTriggerFrameArmingTimeDurationRangeTableEntries[] =
	{
		{0, 0.55, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrTriggerFrameArmingTimeDurationRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrTriggerFrameArmingTimeDurationRangeTableEntries,
	};



static IviRangeTableEntry attrMarkerSearchUpperTargetRangeTableEntries[] =
	{
		{1, 99, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrMarkerSearchUpperTargetRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrMarkerSearchUpperTargetRangeTableEntries,
	};

static IviRangeTableEntry attrMarkerSearchStartValueSourceRangeTableEntries[] =
	{
		{AUML24XX_VAL_MARKER_SEARCH_SOURCE_MARKER, 0, 0, "MARKER", 0},
		{AUML24XX_VAL_MARKER_SEARCH_SOURCE_GATE, 0, 0, "GATE", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrMarkerSearchStartValueSourceRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrMarkerSearchStartValueSourceRangeTableEntries,
	};

static IviRangeTableEntry attrMarkerSearchLowerTargetRangeTableEntries[] =
	{
		{1, 99, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrMarkerSearchLowerTargetRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrMarkerSearchLowerTargetRangeTableEntries,
	};

static IviRangeTableEntry attrMarkerDeltaMeasurementTypeRangeTableEntries[] =
	{
		{AUML24XX_VAL_DELTA_MARKER_PDIFF, 0, 0, "PDIFF", 0},
		{AUML24XX_VAL_DELTA_MARKER_PAVG, 0, 0, "PAVG", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrMarkerDeltaMeasurementTypeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrMarkerDeltaMeasurementTypeRangeTableEntries,
	};

static IviRangeTableEntry attrMarkerRangeTableEntries[] =
	{
		{1, 4, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrMarkerRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrMarkerRangeTableEntries,
	};

static IviRangeTableEntry attrGatePatternStartTimeRangeTableEntries[] =
	{
		{0, 7, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrGatePatternStartTimeRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrGatePatternStartTimeRangeTableEntries,
	};

static IviRangeTableEntry attrGatePatternStopTimeRangeTableEntries[] =
	{
		{0, 7, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrGatePatternStopTimeRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrGatePatternStopTimeRangeTableEntries,
	};

static IviRangeTableEntry attrFenceStartTimeRangeTableEntries[] =
	{
		{0, 7, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrFenceStartTimeRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrFenceStartTimeRangeTableEntries,
	};

static IviRangeTableEntry attrFenceStopTimeRangeTableEntries[] =
	{
		{0, 7, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrFenceStopTimeRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrFenceStopTimeRangeTableEntries,
	};

static IviRangeTableEntry attrGatePatternActiveNumberRangeTableEntries[] =
	{
		{1, 4, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrGatePatternActiveNumberRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrGatePatternActiveNumberRangeTableEntries,
	};

static IviRangeTableEntry attrGatePattern1RepeatOffsetRangeTableEntries[] =
	{
		{0, 7, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrGatePattern1RepeatOffsetRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrGatePattern1RepeatOffsetRangeTableEntries,
	};

static IviRangeTableEntry attrGatePattern1RepeatNumberRangeTableEntries[] =
	{
		{2, 8, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrGatePattern1RepeatNumberRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrGatePattern1RepeatNumberRangeTableEntries,
	};

static IviRangeTableEntry attrPMODTriggerArmingModeRangeTableEntries[] =
	{
		{AUML24XX_VAL_TRIGGER_ARMING_MODE_AUTO, 0, 0, "AUTO", 0},
		{AUML24XX_VAL_TRIGGER_ARMING_MODE_SINGLE, 0, 0, "SINGLE", 0},
		{AUML24XX_VAL_TRIGGER_ARMING_MODE_FRAME, 0, 0, "FRAME", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrPMODTriggerArmingModeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrPMODTriggerArmingModeRangeTableEntries,
	};


static IviRangeTableEntry attrRangeCalibratorDiagnosticsTestRangeTableEntries[] =
	{
		{AUML24XX_VAL_RANGE_CALIBRATOR_ZERO, 0, 0, "0", 0},
		{AUML24XX_VAL_RANGE_CALIBRATOR_RANGE1_HIGH, 0, 0, "1", 0},
		{AUML24XX_VAL_RANGE_CALIBRATOR_RANGE1_LOW, 0, 0, "2", 0},
		{AUML24XX_VAL_RANGE_CALIBRATOR_RANGE2_HIGH, 0, 0, "3", 0},
		{AUML24XX_VAL_RANGE_CALIBRATOR_RANGE2_LOW, 0, 0, "4", 0},
		{AUML24XX_VAL_RANGE_CALIBRATOR_RANGE3_HIGH, 0, 0, "5", 0},
		{AUML24XX_VAL_RANGE_CALIBRATOR_RANGE3_LOW, 0, 0, "6", 0},
		{AUML24XX_VAL_RANGE_CALIBRATOR_RANGE4_HIGH, 0, 0, "7", 0},
		{AUML24XX_VAL_RANGE_CALIBRATOR_RANGE4_LOW, 0, 0, "8", 0},
		{AUML24XX_VAL_RANGE_CALIBRATOR_RANGE5_HIGH, 0, 0, "9", 0},
		{AUML24XX_VAL_RANGE_CALIBRATOR_RANGE5_LOW, 0, 0, "10", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrRangeCalibratorDiagnosticsTestRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrRangeCalibratorDiagnosticsTestRangeTableEntries,
	};

static IviRangeTableEntry attrBNC1OutputModeRangeTableEntries[] =
	{
		{AUML24XX_VAL_OUTPUT_MODE_OFF, 0, 0, "OFF", 0},
		{AUML24XX_VAL_OUTPUT_MODE_AOUT, 0, 0, "AOUT", 0},
		{AUML24XX_VAL_OUTPUT_MODE_PSFL, 0, 0, "PSFL", 0},
		{AUML24XX_VAL_OUTPUT_MODE_LVLA1, 0, 0, "LVLA1", 0},
		{AUML24XX_VAL_OUTPUT_MODE_LVLA2, 0, 0, "LVLA2", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrBNC1OutputModeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrBNC1OutputModeRangeTableEntries,
	};

static IviRangeTableEntry attrBNC2OutputModeRangeTableEntries[] =
	{
		{AUML24XX_VAL_OUTPUT_MODE_OFF, 0, 0, "OFF", 0},
		{AUML24XX_VAL_OUTPUT_MODE_AOUT, 0, 0, "AOUT", 0},
		{AUML24XX_VAL_OUTPUT_MODE_PSFL, 0, 0, "PSFL", 0},
		{AUML24XX_VAL_OUTPUT_MODE_LVLB1, 0, 0, "LVLB1", 0},
		{AUML24XX_VAL_OUTPUT_MODE_LVLB2, 0, 0, "LVLB2", 0},
		{AUML24XX_VAL_OUTPUT_MODE_TRIG, 0, 0, "TRIG", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrBNC2OutputModeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrBNC2OutputModeRangeTableEntries,
	};

static IviRangeTableEntry attrBNCPassVoltageLevelRangeTableEntries[] =
	{
		{AUML24XX_VAL_BNC_PASS_VOLTAGE_LEVEL_HIGH, 0, 0, "HIGH", 0},
		{AUML24XX_VAL_BNC_PASS_VOLTAGE_LEVEL_LOW, 0, 0, "LOW", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrBNCPassVoltageLevelRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrBNCPassVoltageLevelRangeTableEntries,
	};

static IviRangeTableEntry attrBNCAnalogOutputStartVoltageScaleRangeTableEntries[] =
	{
		{-5, +5, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrBNCAnalogOutputStartVoltageScaleRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrBNCAnalogOutputStartVoltageScaleRangeTableEntries,
	};

static IviRangeTableEntry attrBNCAnalogOutputStopVoltageScaleRangeTableEntries[] =
	{
		{-5, 5, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrBNCAnalogOutputStopVoltageScaleRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrBNCAnalogOutputStopVoltageScaleRangeTableEntries,
	};

static IviRangeTableEntry attrBNCOutputSourceChannelRangeTableEntries[] =
	{
		{AUML24XX_VAL_BNC_OUTPUT_SOURCE_CHANNEL_1, 0, 0, "1", 0},
		{AUML24XX_VAL_BNC_OUTPUT_SOURCE_CHANNEL_2, 0, 0, "2", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrBNCOutputSourceChannelRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrBNCOutputSourceChannelRangeTableEntries,
	};

static IviRangeTableEntry attrBNCDBAnalogOutputRangeTableEntries[] =
	{
		{-270, 260, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrBNCDBAnalogOutputRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrBNCDBAnalogOutputRangeTableEntries,
	};

static IviRangeTableEntry attrBNCDBMAnalogOutputRangeTableEntries[] =
	{
		{-270, 260, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrBNCDBMAnalogOutputRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrBNCDBMAnalogOutputRangeTableEntries,
	};

static IviRangeTableEntry attrBNCDBUVAnalogOutputRangeTableEntries[] =
	{
		{-163, 367, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrBNCDBUVAnalogOutputRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrBNCDBUVAnalogOutputRangeTableEntries,
	};

static IviRangeTableEntry attrBNCDBMVAnalogOutputRangeTableEntries[] =
	{
		{-223, 307, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrBNCDBMVAnalogOutputRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrBNCDBMVAnalogOutputRangeTableEntries,
	};

static IviRangeTableEntry attrBNCWattsAnalogOutputRangeTableEntries[] =
	{
		{0, 9.9999e8, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrBNCWattsAnalogOutputRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrBNCWattsAnalogOutputRangeTableEntries,
	};

static IviRangeTableEntry attrBNCDBWAnalogOutputRangeTableEntries[] =
	{
		{-283, 247, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrBNCDBWAnalogOutputRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrBNCDBWAnalogOutputRangeTableEntries,
	};

static IviRangeTableEntry attrSensorOffsetTableActiveNumberRangeTableEntries[] =
	{
		{1, 5, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrSensorOffsetTableActiveNumberRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSensorOffsetTableActiveNumberRangeTableEntries,
	};

static IviRangeTableEntry attrSensorOffsetTypeRangeTableEntries[] =
	{
		{AUML24XX_VAL_SENSOR_OFFSET_TYPE_OFF, 0, 0, "OFF", 0},
		{AUML24XX_VAL_SENSOR_OFFSET_TYPE_FIXED, 0, 0, "FIXED", 0},
		{AUML24XX_VAL_SENSOR_OFFSET_TYPE_TABLE, 0, 0, "TABLE", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrSensorOffsetTypeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSensorOffsetTypeRangeTableEntries,
	};

static IviRangeTableEntry attrSensorRangeHoldRangeTableEntries[] =
	{
		{AUML24XX_VAL_RANGE_HOLD_AUTO, 0, 0, "AUTO", 0},
		{AUML24XX_VAL_RANGE_HOLD_1, 0, 0, "1", 0},
		{AUML24XX_VAL_RANGE_HOLD_2, 0, 0, "2", 0},
		{AUML24XX_VAL_RANGE_HOLD_3, 0, 0, "3", 0},
		{AUML24XX_VAL_RANGE_HOLD_4, 0, 0, "4", 0},
		{AUML24XX_VAL_RANGE_HOLD_5, 0, 0, "5", 0},
		{AUML24XX_VAL_RANGE_HOLD_6, 0, 0, "6", 0},
		{AUML24XX_VAL_RANGE_HOLD_7, 0, 0, "7", 0},
		{AUML24XX_VAL_RANGE_HOLD_8, 0, 0, "8", 0},
		{AUML24XX_VAL_RANGE_HOLD_9, 0, 0, "9", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrSensorRangeHoldRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSensorRangeHoldRangeTableEntries,
	};

static IviRangeTableEntry attrSensorUniversalOperationModeRangeTableEntries[] =
	{
		{AUML24XX_VAL_UNIVERSAL_SENSOR_TRMS, 0, 0, "TRMS", 0},
		{AUML24XX_VAL_UNIVERSAL_SENSOR_FCW, 0, 0, "FCW", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrSensorUniversalOperationModeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSensorUniversalOperationModeRangeTableEntries,
	};

static IviRangeTableEntry attrPAEInputConfigurationRangeTableEntries[] =
	{
		{AUML24XX_VAL_PAE_INPUT_A_MINUS_B, 0, 0, "A-B", 0},
		{AUML24XX_VAL_PAE_INPUT_B_MINUS_A, 0, 0, "B-A", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrPAEInputConfigurationRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrPAEInputConfigurationRangeTableEntries,
	};

static IviRangeTableEntry attrPAEBiasVoltageRangeTableEntries[] =
	{
		{1e-6, 1e6, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrPAEBiasVoltageRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrPAEBiasVoltageRangeTableEntries,
	};

static IviRangeTableEntry attrPAEBiasCurrentSourceRangeTableEntries[] =
	{
		{AUML24XX_VAL_PAE_BIAS_CURRENT_SOURCE_FIXED, 0, 0, "FIXED", 0},
		{AUML24XX_VAL_PAE_BIAS_CURRENT_SOURCE_PROBE, 0, 0, "PROBE", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrPAEBiasCurrentSourceRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrPAEBiasCurrentSourceRangeTableEntries,
	};

static IviRangeTableEntry attrPAEBiasCurrentValueRangeTableEntries[] =
	{
		{1e-6, 1e3, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrPAEBiasCurrentValueRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrPAEBiasCurrentValueRangeTableEntries,
	};

static IviRangeTableEntry attrStatPostProcessingFunctionTypeRangeTableEntries[] =
	{
		{AUML24XX_VAL_PROCESSING_TYPE_PDF, 0, 0, "PDF", 0},
		{AUML24XX_VAL_PROCESSING_TYPE_CDF, 0, 0, "CDF", 0},
		{AUML24XX_VAL_PROCESSING_TYPE_CCDF, 0, 0, "CCDF", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrStatPostProcessingFunctionTypeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrStatPostProcessingFunctionTypeRangeTableEntries,
	};

static IviRangeTableEntry attrDualChannelPostProcessingFunctionModuleRangeTableEntries[] =
	{
		{AUML24XX_VAL_POST_PROCESSING_STATS, 0, 0, "STATS", 0},
		{AUML24XX_VAL_POST_PROCESSING_PAE, 0, 0, "PAE", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrDualChannelPostProcessingFunctionModuleRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrDualChannelPostProcessingFunctionModuleRangeTableEntries,
	};

static IviRangeTableEntry attrSingleChannelPostProcessingFunctionModuleRangeTableEntries[] =
	{
		{AUML24XX_VAL_POST_PROCESSING_STATS, 0, 0, "STATS", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrSingleChannelPostProcessingFunctionModuleRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSingleChannelPostProcessingFunctionModuleRangeTableEntries,
	};

static IviRangeTableEntry attrPAESourceRangeTableEntries[] =
	{
		{AUML24XX_VAL_PAE_SOURCE_CHANNEL, 0, 0, "CHANNEL", 0},
		{AUML24XX_VAL_PAE_SOURCE_GATE, 0, 0, "GATE", 0},
		{AUML24XX_VAL_PAE_SOURCE_MARKER, 0, 0, "MARKER", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrPAESourceRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrPAESourceRangeTableEntries,
	};

static IviRangeTableEntry attrStatPostProcessingSourceSlelectionRangeTableEntries[] =
	{
		{AUML24XX_VAL_STAT_POST_PROCESSING_SOURCE_CHANNEL, 0, 0, "CHANNEL", 0},
		{AUML24XX_VAL_STAT_POST_PROCESSING_SOURCE_GATE, 0, 0, "GATE", 0},
		{AUML24XX_VAL_STAT_POST_PROCESSING_SOURCE_MARKER, 0, 0, "MARKER", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrStatPostProcessingSourceSlelectionRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrStatPostProcessingSourceSlelectionRangeTableEntries,
	};

static IviRangeTableEntry attrLimitPDEFComplexSpecificationActiveNumberRangeTableEntries[] =
	{
		{1, 20, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrLimitPDEFComplexSpecificationActiveNumberRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrLimitPDEFComplexSpecificationActiveNumberRangeTableEntries,
	};

static IviRangeTableEntry attrLimitUserComplexSpecificationActiveNumberRangeTableEntries[] =
	{
		{1, 30, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrLimitUserComplexSpecificationActiveNumberRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrLimitUserComplexSpecificationActiveNumberRangeTableEntries,
	};

static IviRangeTableEntry attrLimitLineTestTypeRangeTableEntries[] =
	{
		{AUML24XX_VAL_LIMIT_LINE_TEST_BOTH, 0, 0, "BOTH", 0},
		{AUML24XX_VAL_LIMIT_LINE_TEST_LOWER, 0, 0, "LOWER", 0},
		{AUML24XX_VAL_LIMIT_LINE_TEST_UPPER, 0, 0, "UPPER", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrLimitLineTestTypeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrLimitLineTestTypeRangeTableEntries,
	};

static IviRangeTableEntry attrLimitSimpleUpperLineRangeTableEntries[] =
	{
		{-999.99, 9.99e8, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrLimitSimpleUpperLineRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrLimitSimpleUpperLineRangeTableEntries,
	};

static IviRangeTableEntry attrLimitSimpleLowerLineRangeTableEntries[] =
	{
		{-999.99, 9.99e8, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrLimitSimpleLowerLineRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrLimitSimpleLowerLineRangeTableEntries,
	};

static IviRangeTableEntry attrLimitComplexTimeOffsetRangeTableEntries[] =
	{
		{-7.00, 7.00, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrLimitComplexTimeOffsetRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrLimitComplexTimeOffsetRangeTableEntries,
	};

static IviRangeTableEntry attrLimitComplexTimeReplicationOffsetRangeTableEntries[] =
	{
		{0, 7.00, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrLimitComplexTimeReplicationOffsetRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrLimitComplexTimeReplicationOffsetRangeTableEntries,
	};

static IviRangeTableEntry attrLimitComplexPowerReplicationOffsetRangeTableEntries[] =
	{
		{-999.99, 9.9999e8, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrLimitComplexPowerReplicationOffsetRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrLimitComplexPowerReplicationOffsetRangeTableEntries,
	};

static IviRangeTableEntry attrLimitComplexRpeatCountRangeTableEntries[] =
	{
		{2, 8, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrLimitComplexRpeatCountRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrLimitComplexRpeatCountRangeTableEntries,
	};

static IviRangeTableEntry attrLimitComplexPowerOffsetRangeTableEntries[] =
	{
		{-999.99, 9.99e8, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrLimitComplexPowerOffsetRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrLimitComplexPowerOffsetRangeTableEntries,
	};

static IviRangeTableEntry attrLimitCheckingTypeRangeTableEntries[] =
	{
		{AUML24XX_VAL_LIMIT_CHECKING_COMPLEX, 0, 0, "COMPLEX", 0},
		{AUML24XX_VAL_LIMIT_CHECKING_SIMPLE, 0, 0, "SIMPLE", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrLimitCheckingTypeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrLimitCheckingTypeRangeTableEntries,
	};

static IviRangeTableEntry attrSensorCalibrationFactorAdjustPCTRangeTableEntries[] =
	{
		{0.07, 150, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrSensorCalibrationFactorAdjustPCTRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSensorCalibrationFactorAdjustPCTRangeTableEntries,
	};

static IviRangeTableEntry attrSensorCalibrationFactorAdjustDBRangeTableEntries[] =
	{
		{-1.76, 31.55, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrSensorCalibrationFactorAdjustDBRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSensorCalibrationFactorAdjustDBRangeTableEntries,
	};

static IviRangeTableEntry attrSensorCalibrationFactorManualValuePCTRangeTableEntries[] =
	{
		{0.07, 150, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrSensorCalibrationFactorManualValuePCTRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSensorCalibrationFactorManualValuePCTRangeTableEntries,
	};

static IviRangeTableEntry attrSensorCalibrationFactorManualValueDBRangeTableEntries[] =
	{
		{-1.76, 31.55, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrSensorCalibrationFactorManualValueDBRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSensorCalibrationFactorManualValueDBRangeTableEntries,
	};

static IviRangeTableEntry attrSensorCalibrationFactorSourceRangeTableEntries[] =
	{
		{AUML24XX_VAL_CAL_FACTOR_SOURCE_FREQ, 0, 0, "FREQ", 0},
		{AUML24XX_VAL_CAL_FACTOR_SOURCE_MAN, 0, 0, "MAN", 0},
		{AUML24XX_VAL_CAL_FACTOR_SOURCE_VGHZ, 0, 0, "VGHZ", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrSensorCalibrationFactorSourceRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSensorCalibrationFactorSourceRangeTableEntries,
	};

static IviRangeTableEntry attrSensorCalibrationFactorStopFrequencyRangeTableEntries[] =
	{
		{1e5, 4e11, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrSensorCalibrationFactorStopFrequencyRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSensorCalibrationFactorStopFrequencyRangeTableEntries,
	};

static IviRangeTableEntry attrSensorCalibrationFactorUnitsRangeTableEntries[] =
	{
		{AUML24XX_VAL_CAL_FACTOR_UNIT_PCT, 0, 0, "PCT", 0},
		{AUML24XX_VAL_CAL_FACTOR_UNIT_DB, 0, 0, "DB", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrSensorCalibrationFactorUnitsRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSensorCalibrationFactorUnitsRangeTableEntries,
	};

static IviRangeTableEntry attrSensorCalibrationFactorStopVoltageRangeTableEntries[] =
	{
		{0, 20, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrSensorCalibrationFactorStopVoltageRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSensorCalibrationFactorStopVoltageRangeTableEntries,
	};

static IviRangeTableEntry attrSensorCalibrationFactorStartVoltageRangeTableEntries[] =
	{
		{0, 20, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrSensorCalibrationFactorStartVoltageRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSensorCalibrationFactorStartVoltageRangeTableEntries,
	};

static IviRangeTableEntry attrSensorCalibrationFactorStartFrequencyRangeTableEntries[] =
	{
		{1e5, 4e11, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrSensorCalibrationFactorStartFrequencyRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSensorCalibrationFactorStartFrequencyRangeTableEntries,
	};

static IviRangeTableEntry attrRFReferenceCalibratorFrequencyRangeTableEntries[] =
	{
		{AUML24XX_VAL_REF_CALIBRATOR_FREQ_50MHZ, 0, 0, "50MHZ", 0},
		{AUML24XX_VAL_REF_CALIBRATOR_FREQ_1GHZ, 0, 0, "1GHZ", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrRFReferenceCalibratorFrequencyRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrRFReferenceCalibratorFrequencyRangeTableEntries,
	};

static IviRangeTableEntry attrInternalTriggerLevelRangeTableEntries[] =
	{
		{-230, 220, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrInternalTriggerLevelRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrInternalTriggerLevelRangeTableEntries,
	};

static IviRangeTableEntry attrTriggerFrameArmingLevelRangeTableEntries[] =
	{
		{-230, 220, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrTriggerFrameArmingLevelRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrTriggerFrameArmingLevelRangeTableEntries,
	};

static IviRangeTableEntry attrTriggerBandwidthRangeTableEntries[] =
	{
		{AUML24XX_VAL_TRIGGER_BANDWIDTH_20MHZ, 0, 0, "20MHZ", 0},
		{AUML24XX_VAL_TRIGGER_BANDWIDTH_2MHZ, 0, 0, "2MHZ", 0},
		{AUML24XX_VAL_TRIGGER_BANDWIDTH_200KHZ, 0, 0, "200KHZ", 0},
		{AUML24XX_VAL_TRIGGER_BANDWIDTH_20KHZ, 0, 0, "20KHZ", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrTriggerBandwidthRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrTriggerBandwidthRangeTableEntries,
	};

static IviRangeTableEntry attrTriggerHoldoffTimeRangeTableEntries[] =
	{
		{0, 7, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrTriggerHoldoffTimeRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrTriggerHoldoffTimeRangeTableEntries,
	};

static IviRangeTableEntry attrTriggerExternalEdgeRangeTableEntries[] =
	{
		{AUML24XX_VAL_EXT_EDGE_RISE, 0, 0, "RISE", 0},
		{AUML24XX_VAL_EXT_EDGE_FALL, 0, 0, "FALL", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrTriggerExternalEdgeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrTriggerExternalEdgeRangeTableEntries,
	};

static IviRangeTableEntry attrRelativeModeRangeTableEntries[] =
	{
		{AUML24XX_VAL_RELATIVE_MODE_OFF, 0, 0, "0", 0},
		{AUML24XX_VAL_RELATIVE_MODE_REF, 0, 0, "1", 0},
		{AUML24XX_VAL_RELATIVE_MODE_OLD_REF, 0, 0, "2", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrRelativeModeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrRelativeModeRangeTableEntries,
	};

static IviRangeTableEntry attrDualChannelConfigRangeTableEntries[] =
	{
		{AUML24XX_VAL_CHAN_CONFIG_A, 0, 0, "A", 0},
		{AUML24XX_VAL_CHAN_CONFIG_B, 0, 0, "B", 0},
		{AUML24XX_VAL_CHAN_CONFIG_A_MINUS_B, 0, 0, "A-B", 0},
		{AUML24XX_VAL_CHAN_CONFIG_B_MINUS_A, 0, 0, "B-A", 0},
		{AUML24XX_VAL_CHAN_CONFIG_A_DIVIDED_B, 0, 0, "A/B", 0},
		{AUML24XX_VAL_CHAN_CONFIG_B_DIVIDED_A, 0, 0, "B/A", 0},
		{AUML24XX_VAL_CHAN_CONFIG_V, 0, 0, "V", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrDualChannelConfigRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrDualChannelConfigRangeTableEntries,
	};

static IviRangeTableEntry attrSingleChannelConfigRangeTableEntries[] =
	{
		{AUML24XX_VAL_CHAN_CONFIG_A, 0, 0, "A", 0},
		{AUML24XX_VAL_CHAN_CONFIG_V, 0, 0, "V", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrSingleChannelConfigRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSingleChannelConfigRangeTableEntries,
	};

static IviRangeTableEntry attrPMODMeasurementTypeRangeTableEntries[] =
	{
		{AUML24XX_VAL_PMOD_MEAS_TYPE_1, 0, 0, "1", 0},
		{AUML24XX_VAL_PMOD_MEAS_TYPE_2, 0, 0, "2", 0},
		{AUML24XX_VAL_PMOD_MEAS_TYPE_3, 0, 0, "3", 0},
		{AUML24XX_VAL_PMOD_MEAS_TYPE_4, 0, 0, "4", 0},
		{AUML24XX_VAL_PMOD_MEAS_TYPE_5, 0, 0, "5", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrPMODMeasurementTypeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrPMODMeasurementTypeRangeTableEntries,
	};

static IviRangeTableEntry attrChannelMeasurementModeRangeTableEntries[] =
	{
		{AUML24XX_VAL_MEAS_MODE_CW, 0, 0, "CW", 0},
		{AUML24XX_VAL_MEAS_MODE_PMOD, 0, 0, "PMOD", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrChannelMeasurementModeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrChannelMeasurementModeRangeTableEntries,
	};

static IviRangeTableEntry attrCWAveragingModeRangeTableEntries[] =
	{
		{AUML24XX_VAL_AVERAGING_MOV, 0, 0, "MOV", 0},
		{AUML24XX_VAL_AVERAGING_RPT, 0, 0, "RPT", 0},
		{AUML24XX_VAL_AVERAGING_AUTO, 0, 0, "AUTO", 0},
		{AUML24XX_VAL_AVERAGING_OFF, 0, 0, "OFF", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrCWAveragingModeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrCWAveragingModeRangeTableEntries,
	};

static IviRangeTableEntry attrSettlePercentRangeTableEntries[] =
	{
		{0.01, 10, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrSettlePercentRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSettlePercentRangeTableEntries,
	};


static IviRangeTableEntry attrDisplayMeasurementPointsEntries[] =
	{
		{AUML24XX_VAL_P200, 0, 0, "P200", 0},
		{AUML24XX_VAL_P400, 0, 0, "P400", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrDisplayMeasurementPoints =
	{
		IVI_VAL_DISCRETE,
        VI_FALSE,
        VI_FALSE,
        VI_NULL,
        attrDisplayMeasurementPointsEntries,
	};

static IviRangeTableEntry attrInstrModelTypeRangeTableEntries[] =
	{
		{AUML24XX_VAL_MODEL_ML2487A, 0, 0, "ML2487A", 0},
		{AUML24XX_VAL_MODEL_ML2488A, 0, 0, "ML2488A", 0},
		{AUML24XX_VAL_MODEL_ML2495A, 0, 0, "ML2495A", 0},
		{AUML24XX_VAL_MODEL_ML2496A, 0, 0, "ML2496A", 0},
		{AUML24XX_VAL_MODEL_ML2487B, 0, 0, "ML2487B", 0},
		{AUML24XX_VAL_MODEL_ML2488B, 0, 0, "ML2488B", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrInstrModelTypeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_FALSE,
        VI_FALSE,
        VI_NULL,
        attrInstrModelTypeRangeTableEntries,
	};

static IviRangeTableEntry attr248XSampleRateRangeTableEntries[] =
	{
		{AUML24XX_VAL_AUTO, 0, 0, "AUTO", 0},
		{AUML24XX_VAL_31K25, 0, 0, "31K25", 0},
		{AUML24XX_VAL_62K5, 0, 0, "62K5", 0},
		{AUML24XX_VAL_125K, 0, 0, "125K", 0},
		{AUML24XX_VAL_250K, 0, 0, "250K", 0},
		{AUML24XX_VAL_500K, 0, 0, "500K", 0},
		{AUML24XX_VAL_1M, 0, 0, "1M", 0},
		{AUML24XX_VAL_2M, 0, 0, "2M", 0},
		{AUML24XX_VAL_4M, 0, 0, "4M", 0},
		{AUML24XX_VAL_8M, 0, 0, "8M", 0},
		{AUML24XX_VAL_16M, 0, 0, "16M", 0},
		{AUML24XX_VAL_32M, 0, 0, "32M", 0},
		{AUML24XX_VAL_64M, 0, 0, "64M", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attr248XSampleRateRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attr248XSampleRateRangeTableEntries,
	};

static IviRangeTableEntry attr249XSampleRateRangeTableEntries[] =
	{
		{AUML24XX_VAL_AUTO, 0, 0, "AUTO", 0},
		{AUML24XX_VAL_31K25, 0, 0, "31K25", 0},
		{AUML24XX_VAL_62K5, 0, 0, "62K5", 0},
		{AUML24XX_VAL_125K, 0, 0, "125K", 0},
		{AUML24XX_VAL_250K, 0, 0, "250K", 0},
		{AUML24XX_VAL_500K, 0, 0, "500K", 0},
		{AUML24XX_VAL_1M, 0, 0, "1M", 0},
		{AUML24XX_VAL_2M, 0, 0, "2M", 0},
		{AUML24XX_VAL_4M, 0, 0, "4M", 0},
		{AUML24XX_VAL_8M, 0, 0, "8M", 0},
		{AUML24XX_VAL_16M, 0, 0, "16M", 0},
		{AUML24XX_VAL_31M25, 0, 0, "31M25", 0},
		{AUML24XX_VAL_62M5, 0, 0, "62M5", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attr249XSampleRateRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attr249XSampleRateRangeTableEntries,
	};

static IviRangeTableEntry attrCorrectionFrequencyRangeTableEntries[] =
	{
		{100e3, 4e11, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrCorrectionFrequencyRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrCorrectionFrequencyRangeTableEntries,
	};

/*- AUML24XX_ATTR_OFFSET -*/
static IviRangeTableEntry attrOffsetRangeTableEntries[] =
    {
        {-200.0, 200, 0, "", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };
static IviRangeTable attrOffsetRangeTable =
    {
        IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrOffsetRangeTableEntries,
	};
/*- AUML24XX_ATTR_UNITS -*/
static IviRangeTableEntry attrUnitsRangeTableEntries[] =
	{
		{AUML24XX_VAL_DBM, 0, 0, "DBM", 0},
		{AUML24XX_VAL_DBMV, 0, 0, "DBMV", 0},
		{AUML24XX_VAL_DBUV, 0, 0, "DBUV", 0},
		{AUML24XX_VAL_WATTS, 0, 0, "W", 0},
		{AUML24XX_VAL_DBW, 0, 0, "DBW", 0},
		{AUML24XX_VAL_VOLTS, 0, 0, "V", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrUnitsRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_FALSE,
        VI_FALSE,
        VI_NULL,
        attrUnitsRangeTableEntries,

    };


/*- AUML24XX_ATTR_TRIGGER_SOURCE -*/
static IviRangeTableEntry attrTriggerSourceRangeTableEntries[] =
	{
        {AUML24XX_VAL_IMMEDIATE,0, 0, "IMM",    0},
		{AUML24XX_VAL_EXTERNAL, 0, 0, "EXTTL", 0},
        {AUML24XX_VAL_INTERNAL, 0, 0, "INT",    0},
        {AUML24XX_VAL_SOFTWARE_TRIG,   0, 0, "BUS",    0},
		{AUML24XX_VAL_INTA, 0, 0, "INTA", 0},
		{AUML24XX_VAL_INTB, 0, 0, "INTB", 0},
		{AUML24XX_VAL_CONT, 0, 0, "CONT", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrTriggerSourceRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_FALSE,
        VI_FALSE,
        VI_NULL,
        attrTriggerSourceRangeTableEntries,
	};

static IviRangeTableEntry attrInternalTriggerSlopeRangeTableEntries[] =
	{
		{AUML24XX_VAL_POSITIVE, 0, 0, "RISE", 0},
		{AUML24XX_VAL_NEGATIVE, 0, 0, "FALL", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrInternalTriggerSlopeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_FALSE,
        VI_FALSE,
        VI_NULL,
        attrInternalTriggerSlopeRangeTableEntries,
	};

/*- AUML24XX_ATTR_DUTY_CYCLE_CORRECTION -*/
static IviRangeTableEntry attrDutyCycleCorrectionRangeTableEntries[] =
	{
		{0.1, 100.00, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrDutyCycleCorrectionRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrDutyCycleCorrectionRangeTableEntries,
	};

/*- AUML24XX_ATTR_AVERAGING_COUNT -*/
static IviRangeTableEntry attrAveragingCountRangeTableEntries[] =
	{
		{1, 512, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrAveragingCountRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAveragingCountRangeTableEntries,
	};

/*****************************************************************************/
/*= Typedefs for string-value tables.                     ========= */
/*****************************************************************************/
typedef struct 																																				
{
    ViInt32 	     value;			/* Attribute ID */
    ViConstString    string;		/* Command */  
	ViConstString    repCommand;    /* Extension for Repeated Command */    
} auml24xxStringValueEntry;

typedef auml24xxStringValueEntry auml24xxStringValueTable[];

/*- Macros for the wrapper of empty channel name */
#define VALID_CHANNELNAME(channelName) \
		(VI_NULL == channelName ? "" : channelName)

/*- Macros for access to enum range table -----------------------------------*/
#define auml24xx_GetCmdFromIntValue(value, table, cmd) \
    	Ivi_GetViInt32EntryFromValue (value, table, VI_NULL, VI_NULL,\
    								  VI_NULL, VI_NULL, cmd, VI_NULL)

#define auml24xx_GetValueFromCmd(buffer, table, value) \
		Ivi_GetViInt32EntryFromString (buffer, table, value, VI_NULL,\
									   VI_NULL, VI_NULL, VI_NULL)

static auml24xxStringValueTable auml24xxViBoolean_commands =
{

	{AUML24XX_ATTR_MIN_MAX_TRACKING_ENABLED,						"CWMMTKS"					, ","},
	{AUML24XX_ATTR_TRIGGER_LINKING_ENABLED,    						"TRLINKS"					, ""}, 
	{AUML24XX_ATTR_TRIGGER_HOLDOFF_ENABLED,    						"TRHOFS"					, ","},
	{AUML24XX_ATTR_TRIGGER_AUTO_ENABLED,    						"TRAUTOS"					, ","},
	{AUML24XX_ATTR_DUTY_CYCLE_CORRECTION_ENABLED,					"CWDUTYS"					, ","},
	{AUML24XX_ATTR_REF_OSCILLATOR_ENABLED,			        		"SNRFCAL"					, ""},
	{AUML24XX_ATTR_GATE_PATTERN_1_REPEAT_ENABLED,					"GP1REPS"					, ","},
	{AUML24XX_ATTR_MARKER_DELTA_LINK_ENABLED,    					"MKDLINK"					, ","},
	{AUML24XX_ATTR_MARKER_DELTA_ENABLED,    			     		"MKDELTS"					, ","},
	{AUML24XX_ATTR_LIMIT_COMPLEX_RPEAT_ENABLED,    			     	"LMXREPS"					, ","},
	{AUML24XX_ATTR_LIMIT_CHECKING_ENABLED,				   	        "LMSTATE"				    , ","},
	{AUML24XX_ATTR_LIMIT_FAIL_INDICATOR_HOLD_ENABLED,    			"LMFHOLD"					, ","},
	{AUML24XX_ATTR_STAT_POST_PROCESSING_MARKER_ENABLED,    			"TTMKS"					, ","},
	{AUML24XX_ATTR_POST_PROCESSING_ACQUISITION_ENABLED,    			"PPACQS"					, ","},
	{AUML24XX_ATTR_SENSOR_FILTER_ENABLED,               			"SNFILTS"					, ","},
	{AUML24XX_ATTR_GPIB_RESPONSE_BUFFERING_ENABLED,        			"SYBUFS"					, ""},
	{AUML24XX_ATTR_FAST_MODE_ENABLED,                   			"SYFAST"					, ""},
	{AUML24XX_ATTR_INSTR_PMOD_AVERAGING_ENABLED,           			"PMAVGS"					, ","},
	{AUML24XX_ATTR_INSTR_CW_AVERAGING_ENABLED,                      "CWAVG"                     , ","},
	{AUML24XX_ATTR_AVERAGING_AUTO_ENABLED,							"CWAVG"						, ","},
	{AUML24XX_ATTR_GATE_PATTERN_ENABLED,					    	"GPGATS"					, ","},
	{AUML24XX_ATTR_FENCE_ENABLED,					        		"GPFENS"					, ","},
	{AUML24XX_ATTR_MARKER_ENABLED,									"MKSTATE"					, ","},
{VI_NULL													        ,VI_NULL					, VI_NULL}
};	

static auml24xxStringValueTable auml24xxEnum_commands =
{   
	{AUML24XX_ATTR_UNITS,                                            "CHUNIT"					, ","},
	{AUML24XX_ATTR_SAMPLE_RATE,										 "TRSAMPL"					, ","},
	{AUML24XX_ATTR_DISPlAY_MEASUREMENT_POINTS,						 "SYDRES"					, ""},
	{AUML24XX_ATTR_CW_AVERAGING_MODE,								 "CWAVG"					, ","},  
	{AUML24XX_ATTR_PMOD_MEASUREMENT_TYPE,							 "PMMEAS"					, ","}, 
	{AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE,						 "CHMODE"					, ","}, 
	{AUML24XX_ATTR_CHANNEL_INPUT_CONFIG,						     "CHCFG"					, ","},
	{AUML24XX_ATTR_RELATIVE_MODE,        						     "CWREL"					, ","},
	{AUML24XX_ATTR_TRIGGER_EXTERNAL_EDGE,  						     "TRXEDG"					, ""},
	{AUML24XX_ATTR_TRIGGER_BANDWIDTH,  						         "TRBW"					, ","},
    {AUML24XX_ATTR_REF_OSCILLATOR_FREQUENCY,  			      	     "SNCALF"					, ""},
	{AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_UNITS,    	             "SNCFU"				    , ","},
	{AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_SOURCE,   	             "SNCFSRC"				    , ","},
	{AUML24XX_ATTR_MARKER_SEARCH_START_VALUE_SOURCE,   	             "MKPSSV"				    , ","},
	{AUML24XX_ATTR_MARKER_DELTA_MEASUREMENT_TYPE,   	             "MKDMEAS"				    , ","},
	{AUML24XX_ATTR_LIMIT_CHECKING_TYPE,				   	             "LMTYP"				    , ","},
	{AUML24XX_ATTR_LIMIT_LINE_TEST_TYPE,				   	         "LMLINE"				    , ","},
	{AUML24XX_ATTR_STAT_POST_PROCESSING_SOURCE_SELECTION,	         "TTSRC"				    , ","},
	{AUML24XX_ATTR_STAT_POST_PROCESSING_FUNCTION_TYPE,	             "TTFUNC"				    , ","},
	{AUML24XX_ATTR_POST_PROCESSING_FUNCTION_MODULE,	                 "PPFUNC"				    , ","},
	{AUML24XX_ATTR_PAE_SOURCE_SELECTION,        	                 "PAESRC"				    , ","},
	{AUML24XX_ATTR_PAE_INPUT_CONFIGURATION,        	                 "PAECFG"				    , ","},
	{AUML24XX_ATTR_PAE_BIAS_CURRENT_SOURCE,        	                 "PAEBIS"				    , ","},
	{AUML24XX_ATTR_SENSOR_OFFSET_TYPE,	        	                 "SNOFTYP"				    , ","},
	{AUML24XX_ATTR_SENSOR_RANGE_HOLD,	        	                 "SNRGH"				    , ","},
	{AUML24XX_ATTR_SENSOR_UNIVERSAL_OPERATION_MODE,	                 "SNUNIVM"				    , ","},
	{AUML24XX_ATTR_BNC_PASS_VOLTAGE_LEVEL,      	                 "BNPLEV"				    , ","},
	{AUML24XX_ATTR_BNC_OUTPUT_SOURCE_CHANNEL,      	                 "BNOCH"				    , ","},
	{AUML24XX_ATTR_BNC2_OUTPUT_MODE, 		      	                 "BNC2M"				    , ""},
	{AUML24XX_ATTR_BNC1_OUTPUT_MODE, 		      	                 "BNC1M"				    , ""},
	{AUML24XX_ATTR_RANGE_CALIBRATOR_DIAGNOSTICS_TEST,                "RCDIAGT"				    , ","},
	{AUML24XX_ATTR_INSTR_PMOD_TRIGGER_SOURCE,                        "TRSRC"				    , ",PMOD,"},
	{AUML24XX_ATTR_INSTR_CW_TRIGGER_SOURCE,                          "TRSRC"				    , ",CW,"},
	{AUML24XX_ATTR_INSTR_CW_TRIGGER_INTERNAL_SLOPE,                  "TRINEDG"				    , ",CW,"},
	{AUML24XX_ATTR_INSTR_PMOD_TRIGGER_INTERNAL_SLOPE,                "TRINEDG"				    , ",PMOD,"},
	{AUML24XX_ATTR_INSTR_CW_TRIGGER_ARMING_MODE,                     "TRARMD"				    , ",CW,"},
	{AUML24XX_ATTR_INSTR_PMOD_TRIGGER_ARMING_MODE,                   "TRARMD"				    , ",PMOD,"},
	{AUML24XX_ATTR_SENSOR_RANGE_HOLD,                       		 "SNRGH"                    , ","},
	{AUML24XX_ATTR_RRS_TRACE_STATE,                         		 "PMRRS"                    , ""},
	{AUML24XX_ATTR_PMOD_MEASUREMENT_DISPLAY,						 "PMDTYP"					,","},
	{AUML24XX_ATTR_ACTIVE_CHANNEL,                                   "CHACTIV"                  ,""},
	{VI_NULL									                	,VI_NULL					, VI_NULL}
};

static auml24xxStringValueTable auml24xxViReal64_commands =
{
	{AUML24XX_ATTR_CORRECTION_FREQUENCY,							 "SNCFRQ"				, ","}, 
	{AUML24XX_ATTR_SENSOR_CORRECTION_FREQUENCY,						 "SNCFRQ"				, ","}, 
	{AUML24XX_ATTR_OFFSET,					                		 "SNOFIX"				, ","},  
	{AUML24XX_ATTR_SENSOR_OFFSET,	    	                		 "SNOFIX"				, ","},  
	{AUML24XX_ATTR_SETTLE_PERCENT,					        		 "CWSETLP"				, ","},  
	{AUML24XX_ATTR_TRIGGER_HOLDOFF_TIME,    						 "TRHOFT"				, ","},
	{AUML24XX_ATTR_TRIGGER_FRAME_ARMING_TIME_DURATION,				 "TRFTIM"				, ","},
	{AUML24XX_ATTR_TRIGGER_FRAME_ARMING_LEVEL,	        	 		 "TRFLEV"				, ","},
	{AUML24XX_ATTR_DUTY_CYCLE_CORRECTION,  						     "CWDUTY"				, ","},  
	{AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_START_VOLTAGE,	         "SNZSTV"				, ","},
	{AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_START_FREQUENCY,	     "SNZSTF"				, ","},
	{AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_STOP_VOLTAGE,    	     "SNZSPV"				, ","}, 
	{AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_STOP_FREQUENCY,   	     "SNZSPF"				, ","},
	{AUML24XX_ATTR_GATE_PATTERN_1_REPEAT_OFFSET, 			   	     "GP1REPT"				, ","},
	{AUML24XX_ATTR_MARKER_SEARCH_UPPER_TARGET, 			   	         "MKPSUT"				, ","},
	{AUML24XX_ATTR_MARKER_SEARCH_LOWER_TARGET, 			   	         "MKPSLT"				, ","},
	{AUML24XX_ATTR_MARKER_DELTA_POSITION,   			   	         "MKDPOS"				, ","},
	{AUML24XX_ATTR_MARKER_POSITION ,   			   	                  "MKPOS"				, ","},
	{AUML24XX_ATTR_LIMIT_COMPLEX_TIME_OFFSET,   		   	         "LMXTOF"				, ","},
	{AUML24XX_ATTR_LIMIT_COMPLEX_TIME_REPLICATION_OFFSET,			 "LMXROFT"				, ","},
	{AUML24XX_ATTR_LIMIT_COMPLEX_POWER_REPLICATION_OFFSET,			 "LMXROFP"				, ","},
	{AUML24XX_ATTR_LIMIT_COMPLEX_POWER_OFFSET,				         "LMXPOF"				, ","},
	{AUML24XX_ATTR_LIMIT_SIMPLE_UPPER_LINE,				             "LMSUP"				, ","},
	{AUML24XX_ATTR_LIMIT_SIMPLE_LOWER_LINE,				             "LMSLO"				, ","},
	{AUML24XX_ATTR_STAT_POST_PROCESSING_MARKER_POSITION,   			 "TTMKPOS"				, ","},
	{AUML24XX_ATTR_PAE_BIAS_CURRENT_CONVERSION_FACTOR,   			 "PAEBICF"				, ","},
	{AUML24XX_ATTR_PAE_BIAS_VOLTAGE,              		        	 "PAEBV"				, ","},
	{AUML24XX_ATTR_PAE_BIAS_CURRENT,              		        	 "PAEBI"				, ","},
	{AUML24XX_ATTR_BNC_ANALOG_OUTPUT_START_VOLTAGE_SCALE,  			 "BNVOST"				, ","},
	{AUML24XX_ATTR_BNC_ANALOG_OUTPUT_STOP_VOLTAGE_SCALE,  			 "BNVOSP"				, ","},
    {AUML24XX_ATTR_BNC_DB_ANALOG_OUTPUT_START,  			         "BNDST"				, ",DB,"},	
    {AUML24XX_ATTR_BNC_DBM_ANALOG_OUTPUT_START,  		  	         "BNDST"				, ",DBM,"},
    {AUML24XX_ATTR_BNC_DBW_ANALOG_OUTPUT_START,  		     	     "BNDST"				, ",DBW,"},
    {AUML24XX_ATTR_BNC_DBUV_ANALOG_OUTPUT_START,  		        	 "BNDST"				, ",DBUV,"},
    {AUML24XX_ATTR_BNC_DBMV_ANALOG_OUTPUT_START,  		        	 "BNDST"				, ",DBMV,"},
    {AUML24XX_ATTR_BNC_WATTS_ANALOG_OUTPUT_START,  		        	 "BNDST"				, ",W,"},
    {AUML24XX_ATTR_BNC_DB_ANALOG_OUTPUT_STOP,  			             "BNDSP"				, ",DB,"},	
    {AUML24XX_ATTR_BNC_DBM_ANALOG_OUTPUT_STOP,  		     	     "BNDSP"				, ",DBM,"},
    {AUML24XX_ATTR_BNC_DBW_ANALOG_OUTPUT_STOP,  		    	     "BNDSP"				, ",DBW,"},
    {AUML24XX_ATTR_BNC_DBUV_ANALOG_OUTPUT_STOP,  		    	     "BNDSP"				, ",DBUV,"},
    {AUML24XX_ATTR_BNC_DBMV_ANALOG_OUTPUT_STOP,  		        	 "BNDSP"				, ",DBMV,"},
    {AUML24XX_ATTR_BNC_WATTS_ANALOG_OUTPUT_STOP,  		        	 "BNDSP"				, ",W,"},
	{AUML24XX_ATTR_INSTR_CW_TRIGGER_DELAY_TIME,  			         "TRDLYT"				, ",CW,"},
	{AUML24XX_ATTR_INSTR_PMOD_TRIGGER_DELAY_TIME,  			         "TRDLYT"				, ",PMOD,"},
	{AUML24XX_ATTR_INSTR_CW_TRIGGER_INTERNAL_LEVEL,  			     "TRINLEV"				, ",CW,"},
	{AUML24XX_ATTR_INSTR_PMOD_TRIGGER_INTERNAL_LEVEL,  			     "TRINLEV"				, ",PMOD,"},
	{AUML24XX_ATTR_INSTR_CW_TRIGGER_CAPTURE_TIME,  			         "TRCAPT"				, ",CW,"},
	{AUML24XX_ATTR_INSTR_PMOD_TRIGGER_CAPTURE_TIME,  			     "TRCAPT"				, ",PMOD,"},
	{AUML24XX_ATTR_SENSOR_CALIBRATION_MANUAL_FACTOR,    		 	 "SNCFCAL"				, ","},
	{AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_ADJUST, 			     "SNCFADJ"				, ","},
	{AUML24XX_ATTR_GATE_PATTERN_START_TIME,							 "GPTIMST"				, ","},
	{AUML24XX_ATTR_GATE_PATTERN_STOP_TIME,							 "GPTIMSP"				, ","},
	{AUML24XX_ATTR_FENCE_START_TIME,								 "GPFENST"				, ","},
	{AUML24XX_ATTR_FENCE_STOP_TIME,									 "GPFENSP"				, ","},
	{VI_NULL														 ,VI_NULL				, VI_NULL}
};

static auml24xxStringValueTable auml24xxViInt32_commands =
{	 
	{AUML24XX_ATTR_GATE_PATTERN_1_REPEAT_NUMBER,					 "GP1REPN"  		   , ","},
	{AUML24XX_ATTR_LIMIT_COMPLEX_RPEAT_COUNT,		       			 "LMXREPN"  		   , ","},
	{AUML24XX_ATTR_SENSOR_OFFSET_TABLE_ACTIVE_NUMBER,      			 "SNOTSEL"  		   , ","},
	{AUML24XX_ATTR_INSTR_PMOD_AVERAGING_NUMBER,      			     "PMAVGN"  		   , ","},
	{AUML24XX_ATTR_GATE_PATTERN_CONFIGURATION_NUMBER,   			 "GPACTN"		   , ","},
	{AUML24XX_ATTR_LIMIT_COMPLEX_SPECIFICATION_ACTIVE_NUMBER,        "LMXSPEC"		   , ","},
	{VI_NULL											,VI_NULL							, VI_NULL}
};

static auml24xxStringValueTable auml24xxViString_commands =
{
	{AUML24XX_ATTR_INSTR_CW_INTERNAL_TRIGGER_EVENT_SOURCE		    ,"TRSRC"				, ",CW"},
	{AUML24XX_ATTR_INSTR_PMOD_INTERNAL_TRIGGER_EVENT_SOURCE		    ,"TRSRC"				, ",PMOD"},
	{VI_NULL 														,VI_NULL				, VI_NULL}
};

/*****************************************************************************
 *-------------- Utility Function Declarations (Non-Exported) ---------------*
 *****************************************************************************/
static ViStatus auml24xx_IviInit (ViRsrc resourceName, ViBoolean IDQuery, 
                                     ViBoolean reset, ViSession vi);
static ViStatus auml24xx_IviClose (ViSession vi);   

static ViStatus auml24xx_InitAttributes (ViSession vi,ViInt32 model);
static ViStatus auml24xx_DefaultInstrSetup (ViSession openInstrSession);
static ViStatus auml24xx_CheckStatus (ViSession vi);
static ViStatus auml24xx_WaitForOPC (ViSession vi, ViInt32 maxTime);

static ViStatus auml24xx_GetDriverSetupOption (
	ViSession vi,
	ViConstString optionTag,
	ViInt32 optionValueSize,
	ViChar optionValue[] );

static void auml24xx_RemoveSurroundingWhiteSpace(ViChar string[]);

static ViStatus auml24xx_GetModelFromBuffer(
	ViSession       vi,
	ViConstString   buffer,
	ViInt32*        pModelType
	);

    /*- File I/O Utility Functions -*/
static ViStatus auml24xx_ReadToFile (ViSession vi, ViConstString filename, 
                                     ViInt32 maxBytesToRead, ViInt32 fileAction, ViInt32 *totalBytesWritten);
static ViStatus auml24xx_WriteFromFile (ViSession vi, ViConstString filename, 
                                        ViInt32 maxBytesToWrite, ViInt32 byteOffset, 
                                        ViInt32 *totalBytesWritten);

static ViStatus auml24xx_GetStringFromTable(
	auml24xxStringValueTable table,
	ViInt32				  value,
	ViConstString*		  string
);
static ViStatus auml24xx_GetRepCommandFromTable(
	auml24xxStringValueTable table,
	ViInt32				  value,
	ViConstString*		  repCommand
);
static ViStatus auml24xx_GetValueFromTable(
	auml24xxStringValueTable	table,
	ViConstString			string,
	ViInt32*				value
);

static ViStatus	auml24xx_ParseRealArray(
	ViString		buffer,
	ViString        ignoreString,
	ViInt32			count,
	ViReal64		data[],
	ViInt32*		nbOfValues);

static ViStatus	auml24xx_ParseErrorBit(
	ViInt32		errorBit,
	ViInt32     index,
	ViChar      errorInfor[]);

static ViStatus	auml24xx_ViPrintf(
	ViSession vi, 
	ViString writeFmt,
	...); 
static ViStatus	auml24xx_ViScanf(
	ViSession vi, 
	ViString readFmt,
	...); 
static ViStatus	auml24xx_ViQueryf(
	ViSession vi, 
	ViString writeFmt, 
	ViString readFmt,
	...);

static ViStatus auml24xx_GetCount(
	ViInt32         measType,
	ViInt32		    *count);

/*****************************************************************************
 *----------------- Callback Declarations (Non-Exported) --------------------*
 *****************************************************************************/

    /*- Global Session Callbacks --------------------------------------------*/

static ViStatus _VI_FUNC auml24xx_CheckStatusCallback (ViSession vi, ViSession io);
static ViStatus _VI_FUNC auml24xx_WaitForOPCCallback (ViSession vi, ViSession io);

    /*- Attribute callbacks -------------------------------------------------*/

static ViStatus _VI_FUNC auml24xxAttrDriverRevision_ReadCallback (ViSession vi,
                                                                  ViSession io, 
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId, 
                                                                  const ViConstString cacheValue);
static ViStatus _VI_FUNC auml24xxAttrInstrumentManufacturer_ReadCallback (ViSession vi, 
                                                                          ViSession io,
                                                                          ViConstString channelName, 
                                                                          ViAttr attributeId,
                                                                          const ViConstString cacheValue);
static ViStatus _VI_FUNC auml24xxAttrInstrumentModel_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue);

static ViStatus _VI_FUNC auml24xxAttrFirmwareRevision_ReadCallback (ViSession vi, 
                                                                    ViSession io,
                                                                    ViConstString channelName, 
                                                                    ViAttr attributeId,
                                                                    const ViConstString cacheValue);
static ViStatus _VI_FUNC auml24xxAttrIdQueryResponse_ReadCallback (ViSession vi,
                                                                   ViSession io, 
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId, 
                                                                   const ViConstString cacheValue);
static ViStatus _VI_FUNC auml24xxAttrAveragingAutoEnabled_ReadCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViBoolean      *value
);
static ViStatus _VI_FUNC auml24xxAttrAveragingAutoEnabled_WriteCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViBoolean       value
);
static ViStatus _VI_FUNC auml24xxAttrTriggerSource_ReadCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32        *value
);
static ViStatus _VI_FUNC auml24xxAttrTriggerSource_WriteCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32         value
);
static ViStatus _VI_FUNC auml24xxAttrInternalTriggerEventSource_ReadCallback(
    ViSession               vi,
    ViSession               io,
    ViConstString           channelName,
    ViAttr                  attributeId,
    const ViConstString     cacheValue
);
static ViStatus _VI_FUNC auml24xxAttrInternalTriggerEventSource_WriteCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViConstString   value
);
static ViStatus _VI_FUNC auml24xxAttrInternalTriggerLevel_ReadCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViReal64       *value
);
static ViStatus _VI_FUNC auml24xxAttrInternalTriggerLevel_WriteCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViReal64        value
);
static ViStatus _VI_FUNC auml24xxAttrInternalTriggerSlope_ReadCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32        *value
);
static ViStatus _VI_FUNC auml24xxAttrInternalTriggerSlope_WriteCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32         value
);
static ViStatus _VI_FUNC auml24xxAttrAveragingCount_ReadCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32*        value
);
static ViStatus _VI_FUNC auml24xxAttrAveragingCount_WriteCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32         value
);
static ViStatus _VI_FUNC auml24xxAttrRefOscillatorFrequency_ReadCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViReal64       *value
);
static ViStatus _VI_FUNC auml24xxAttrRefOscillatorFrequency_WriteCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViReal64        value
);

static ViStatus _VI_FUNC auml24xxViBoolean_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViBoolean *value);

static ViStatus _VI_FUNC auml24xxViBoolean_WriteCallback (ViSession vi,
													   ViSession io,
													   ViConstString channelName,
													   ViAttr attributeId,
													   ViBoolean value);

static ViStatus _VI_FUNC auml24xxViInt32_ReadCallback (ViSession vi,
                                                       ViSession io,
                                                       ViConstString channelName,
                                                       ViAttr attributeId,
                                                       ViInt32 *value);

static ViStatus _VI_FUNC auml24xxViInt32_WriteCallback (ViSession vi,
                                                        ViSession io,
                                                        ViConstString channelName,
                                                        ViAttr attributeId,
                                                        ViInt32 value);

static ViStatus _VI_FUNC auml24xxViReal64_ReadCallback (ViSession vi,
                                                        ViSession io,
                                                        ViConstString channelName,
                                                        ViAttr attributeId,
                                                        ViReal64 *value);

static ViStatus _VI_FUNC auml24xxViReal64_WriteCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViReal64 value);

static ViStatus _VI_FUNC auml24xxAttrCaptureTime_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrCaptureTime_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC auml24xxAttrTriggerArmingMode_ReadCallback (ViSession vi,
                                                                     ViSession io,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViInt32 *value);

static ViStatus _VI_FUNC auml24xxAttrTriggerArmingMode_WriteCallback (ViSession vi,
                                                                      ViSession io,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViInt32 value);

static ViStatus _VI_FUNC auml24xxAttrTriggerDelayTime_ReadCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrTriggerDelayTime_WriteCallback (ViSession vi,
                                                                     ViSession io,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViReal64 value);

static ViStatus _VI_FUNC auml24xxAttrSensorCalibrationFactorAdjust_ReadCallback (ViSession vi,
                                                                                 ViSession io,
                                                                                 ViConstString channelName,
                                                                                 ViAttr attributeId,
                                                                                 ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrSensorCalibrationFactorAdjust_WriteCallback (ViSession vi,
                                                                                  ViSession io,
                                                                                  ViConstString channelName,
                                                                                  ViAttr attributeId,
                                                                                  ViReal64 value);

static ViStatus _VI_FUNC auml24xxAttrSensorCalibrationManualFactor_ReadCallback (ViSession vi,
                                                                                 ViSession io,
                                                                                 ViConstString channelName,
                                                                                 ViAttr attributeId,
                                                                                 ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrSensorCalibrationManualFactor_WriteCallback (ViSession vi,
                                                                                  ViSession io,
                                                                                  ViConstString channelName,
                                                                                  ViAttr attributeId,
                                                                                  ViReal64 value);

static ViStatus _VI_FUNC auml24xxAttrFenceEnabled_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViBoolean *value);

static ViStatus _VI_FUNC auml24xxAttrFenceEnabled_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViBoolean value);

static ViStatus _VI_FUNC auml24xxAttrFenceStartTime_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrFenceStartTime_WriteCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 value);

static ViStatus _VI_FUNC auml24xxAttrGatePatternEnabled_ReadCallback (ViSession vi,
                                                                      ViSession io,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViBoolean *value);

static ViStatus _VI_FUNC auml24xxAttrGatePatternEnabled_WriteCallback (ViSession vi,
                                                                       ViSession io,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViBoolean value);

static ViStatus _VI_FUNC auml24xxAttrGatePatternStartTime_ReadCallback (ViSession vi,
                                                                        ViSession io,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrGatePatternStartTime_WriteCallback (ViSession vi,
                                                                         ViSession io,
                                                                         ViConstString channelName,
                                                                         ViAttr attributeId,
                                                                         ViReal64 value);

static ViStatus _VI_FUNC auml24xxAttrGatePatternStopTime_ReadCallback (ViSession vi,
                                                                       ViSession io,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrGatePatternStopTime_WriteCallback (ViSession vi,
                                                                        ViSession io,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViReal64 value);

static ViStatus _VI_FUNC auml24xxAttrMarkerPosition_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrMarkerPosition_WriteCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 value);

static ViStatus _VI_FUNC auml24xxAttrMarkerEnabled_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViBoolean *value);

static ViStatus _VI_FUNC auml24xxAttrMarkerEnabled_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViBoolean value);

static ViStatus _VI_FUNC auml24xxAttrBncDbwAnalogOutputStop_ReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrInstrCWTriggerCaptureTime_ReadCallback (ViSession vi,
                                                                             ViSession io,
                                                                             ViConstString channelName,
                                                                             ViAttr attributeId,
                                                                             ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrInstrCWTriggerDelayTime_ReadCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrInstrPMODTriggerCaptureTime_ReadCallback (ViSession vi,
                                                                               ViSession io,
                                                                               ViConstString channelName,
                                                                               ViAttr attributeId,
                                                                               ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrInstrPMODTriggerDelayTime_ReadCallback (ViSession vi,
                                                                             ViSession io,
                                                                             ViConstString channelName,
                                                                             ViAttr attributeId,
                                                                             ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrSampleRate_RangeTableCallback (ViSession vi,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    IviRangeTablePtr *rangeTablePtr);

static ViStatus _VI_FUNC auml24xxAttrChannelInputConfig_RangeTableCallback (ViSession vi,
                                                                            ViConstString channelName,
                                                                            ViAttr attributeId,
                                                                            IviRangeTablePtr *rangeTablePtr);

static ViStatus _VI_FUNC auml24xxAttrTriggerHoldoffTime_CheckCallback (ViSession vi,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViReal64 value);

static ViStatus _VI_FUNC auml24xxAttrTriggerFrameArmingLevel_CheckCallback (ViSession vi,
                                                                            ViConstString channelName,
                                                                            ViAttr attributeId,
                                                                            ViReal64 value);

static ViStatus _VI_FUNC auml24xxAttrTriggerHoldoffEnabled_CheckCallback (ViSession vi,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViBoolean value);

static ViStatus _VI_FUNC auml24xxAttrTriggerLinkingEnabled_CheckCallback (ViSession vi,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViBoolean value);

static ViStatus _VI_FUNC auml24xxAttrTriggerSource_CheckCallback (ViSession vi,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value);

static ViStatus _VI_FUNC auml24xxAttrSensorCalibrationManualFactor_RangeTableCallback (ViSession vi,
                                                                                       ViConstString channelName,
                                                                                       ViAttr attributeId,
                                                                                       IviRangeTablePtr *rangeTablePtr);

static ViStatus _VI_FUNC auml24xxAttrSensorCalibrationFactorAdjust_RangeTableCallback (ViSession vi,
                                                                                       ViConstString channelName,
                                                                                       ViAttr attributeId,
                                                                                       IviRangeTablePtr *rangeTablePtr);

static ViStatus _VI_FUNC auml24xxAttrLimitCheckingType_CheckCallback (ViSession vi,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViInt32 value);

static ViStatus _VI_FUNC auml24xxAttrPostProcessingFunctionModule_RangeTableCallback (ViSession vi,
                                                                                      ViConstString channelName,
                                                                                      ViAttr attributeId,
                                                                                      IviRangeTablePtr *rangeTablePtr);

static ViStatus _VI_FUNC auml24xxAttrBncAnalogOutputStartVoltageScale_CheckCallback (ViSession vi,
                                                                                     ViConstString channelName,
                                                                                     ViAttr attributeId,
                                                                                     ViReal64 value);

static ViStatus _VI_FUNC auml24xxAttrBncAnalogOutputStopVoltageScale_CheckCallback (ViSession vi,
                                                                                    ViConstString channelName,
                                                                                    ViAttr attributeId,
                                                                                    ViReal64 value);

static ViStatus _VI_FUNC auml24xxAttrFenceStopTime_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrFenceStopTime_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value);

static ViStatus _VI_FUNC auml24xxAttrMarkerDeltaPosition_CheckCallback (ViSession vi,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViReal64 value);

static ViStatus _VI_FUNC auml24xxAttrMarkerPosition_CheckCallback (ViSession vi,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 value);

static ViStatus _VI_FUNC auml24xxEnum_ReadCallback (ViSession vi, ViSession io,
                                                    ViConstString channelName,
                                                    ViAttr attributeId,
                                                    ViInt32 *value);

static ViStatus _VI_FUNC auml24xxEnum_WriteCallback (ViSession vi, ViSession io,
                                                     ViConstString channelName,
                                                     ViAttr attributeId,
                                                     ViInt32 value);

static ViStatus _VI_FUNC auml24xxAttrSampleRate_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 *value);

static ViStatus _VI_FUNC auml24xxAttrSampleRate_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 value);

static ViStatus _VI_FUNC auml24xxAttrDisplayMeasurementPoints_ReadCallback (ViSession vi,
                                                                             ViSession io,
                                                                             ViConstString channelName,
                                                                             ViAttr attributeId,
                                                                             ViInt32 *value);

static ViStatus _VI_FUNC auml24xxAttrCWAveragingMode_ReadCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViInt32 *value);

static ViStatus _VI_FUNC auml24xxAttrCWAveragingMode_WriteCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViInt32 value);

static ViStatus _VI_FUNC auml24xxAttrChannelInputConfig_CheckCallback (ViSession vi,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViInt32 value);

static ViStatus _VI_FUNC auml24xxAttrInstrCWAveragingEnabled_ReadCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViBoolean *value);

static ViStatus _VI_FUNC auml24xxAttrInstrCWAveragingEnabled_WriteCallback (ViSession vi,
                                                                            ViSession io,
                                                                            ViConstString channelName,
                                                                            ViAttr attributeId,
                                                                            ViBoolean value);

static ViStatus _VI_FUNC auml24xxAttrTriggerExternalEdge_ReadCallback (ViSession vi,
                                                                       ViSession io,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViInt32 *value);

static ViStatus _VI_FUNC auml24xxAttrTriggerLinkingEnabled_ReadCallback (ViSession vi,
                                                                         ViSession io,
                                                                         ViConstString channelName,
                                                                         ViAttr attributeId,
                                                                         ViBoolean *value);

static ViStatus _VI_FUNC auml24xxAttrTriggerFrameArmingTimeDuration_CheckCallback (ViSession vi,
                                                                                   ViConstString channelName,
                                                                                   ViAttr attributeId,
                                                                                   ViReal64 value);

static ViStatus _VI_FUNC auml24xxAttrInstrCWTriggerArmingMode_ReadCallback (ViSession vi,
                                                                            ViSession io,
                                                                            ViConstString channelName,
                                                                            ViAttr attributeId,
                                                                            ViInt32 *value);

static ViStatus _VI_FUNC auml24xxAttrInstrPMODTriggerArmingMode_ReadCallback (ViSession vi,
                                                                              ViSession io,
                                                                              ViConstString channelName,
                                                                              ViAttr attributeId,
                                                                              ViInt32 *value);

static ViStatus _VI_FUNC auml24xxAttrInstrPMODTriggerInternalSlope_ReadCallback (ViSession vi,
                                                                                 ViSession io,
                                                                                 ViConstString channelName,
                                                                                 ViAttr attributeId,
                                                                                 ViInt32 *value);

static ViStatus _VI_FUNC auml24xxAttrInstrCWTriggerInternalSlope_ReadCallback (ViSession vi,
                                                                               ViSession io,
                                                                               ViConstString channelName,
                                                                               ViAttr attributeId,
                                                                               ViInt32 *value);

static ViStatus _VI_FUNC auml24xxAttrInstrCWTriggerInternalLevel_ReadCallback (ViSession vi,
                                                                               ViSession io,
                                                                               ViConstString channelName,
                                                                               ViAttr attributeId,
                                                                               ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrInstrCWTriggerSource_ReadCallback (ViSession vi,
                                                                        ViSession io,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViInt32 *value);

static ViStatus _VI_FUNC auml24xxAttrInstrPMODTriggerInternalLevel_ReadCallback (ViSession vi,
                                                                                 ViSession io,
                                                                                 ViConstString channelName,
                                                                                 ViAttr attributeId,
                                                                                 ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrInstrPMODTriggerSource_ReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViInt32 *value);

static ViStatus _VI_FUNC auml24xxAttrInstrInternalTriggerEventSource_ReadCallback (ViSession vi,
                                                                                   ViSession io,
                                                                                   ViConstString channelName,
                                                                                   ViAttr attributeId,
                                                                                   const ViConstString cacheValue);

static ViStatus _VI_FUNC auml24xxAttrInstrInternalTriggerEventSource_WriteCallback (ViSession vi,
                                                                                    ViSession io,
                                                                                    ViConstString channelName,
                                                                                    ViAttr attributeId,
                                                                                    ViConstString value);

static ViStatus _VI_FUNC auml24xxAttrInstrInternalTriggerEventSource_CheckCallback (ViSession vi,
                                                                                    ViConstString channelName,
                                                                                    ViAttr attributeId,
                                                                                    ViConstString value);

static ViStatus _VI_FUNC auml24xxAttrInstrCWAveragingNumber_WriteCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViInt32 value);

static ViStatus _VI_FUNC auml24xxAttrInstrCWAveragingNumber_ReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViInt32 *value);

static ViStatus _VI_FUNC auml24xxAttrInstrCWAveragingNumber_CheckCallback (ViSession vi,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViInt32 value);

static ViStatus _VI_FUNC auml24xxAttrAveragingEnabled_ReadCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViBoolean *value);

static ViStatus _VI_FUNC auml24xxAttrAveragingEnabled_WriteCallback (ViSession vi,
                                                                     ViSession io,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViBoolean value);

static ViStatus _VI_FUNC auml24xxAttrLimitComplexSpecificationActiveNumber_WriteCallback (ViSession vi,
                                                                                          ViSession io,
                                                                                          ViConstString channelName,
                                                                                          ViAttr attributeId,
                                                                                          ViInt32 value);

static ViStatus _VI_FUNC auml24xxAttrLimitComplexSpecificationActiveNumber_ReadCallback (ViSession vi,
                                                                                         ViSession io,
                                                                                         ViConstString channelName,
                                                                                         ViAttr attributeId,
                                                                                         ViInt32 *value);

static ViStatus _VI_FUNC auml24xxAttrLimitComplexSpecificationActiveNumber_RangeTableCallback (ViSession vi,
                                                                                               ViConstString channelName,
                                                                                               ViAttr attributeId,
                                                                                               IviRangeTablePtr *rangeTablePtr);

static ViStatus _VI_FUNC auml24xxAttrPAEViReal64_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC auml24xxAttrGpibResponseBufferingEnabled_ReadCallback (ViSession vi,
                                                                                ViSession io,
                                                                                ViConstString channelName,
                                                                                ViAttr attributeId,
                                                                                ViBoolean *value);

static ViStatus _VI_FUNC auml24xxAttrFastModeEnabled_ReadCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViBoolean *value);

static ViStatus _VI_FUNC auml24xxAttrBncDbAnalogOutputStart_ReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrBncDbAnalogOutputStop_ReadCallback (ViSession vi,
                                                                         ViSession io,
                                                                         ViConstString channelName,
                                                                         ViAttr attributeId,
                                                                         ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrBncDbmAnalogOutputStart_ReadCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrBncDbmAnalogOutputStop_ReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrBncDbmvAnalogOutputStart_ReadCallback (ViSession vi,
                                                                            ViSession io,
                                                                            ViConstString channelName,
                                                                            ViAttr attributeId,
                                                                            ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrBncDbmvAnalogOutputStop_ReadCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrBncDbuvAnalogOutputStart_ReadCallback (ViSession vi,
                                                                            ViSession io,
                                                                            ViConstString channelName,
                                                                            ViAttr attributeId,
                                                                            ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrBncDbuvAnalogOutputStop_ReadCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrBncDbwAnalogOutputStart_ReadCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrBncWattsAnalogOutputStart_ReadCallback (ViSession vi,
                                                                             ViSession io,
                                                                             ViConstString channelName,
                                                                             ViAttr attributeId,
                                                                             ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrBncWattsAnalogOutputStop_ReadCallback (ViSession vi,
                                                                            ViSession io,
                                                                            ViConstString channelName,
                                                                            ViAttr attributeId,
                                                                            ViReal64 *value);

static ViStatus _VI_FUNC auml24xxAttrBnc1OutputMode_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 *value);

static ViStatus _VI_FUNC auml24xxAttrBnc2OutputMode_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 *value);

static ViStatus _VI_FUNC auml24xxAttrUnits_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViInt32 *value);

static ViStatus _VI_FUNC auml24xxAttrUnits_WriteCallback (ViSession vi,
                                                          ViSession io,
                                                          ViConstString channelName,
                                                          ViAttr attributeId,
                                                          ViInt32 value);

static ViStatus _VI_FUNC auml24xxAttrActiveChannel_CheckCallback (ViSession vi,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value);

static ViStatus _VI_FUNC auml24xxAttrActiveCalibrationFactorNumber_ReadCallback (ViSession vi,
                                                                                 ViSession io,
                                                                                 ViConstString channelName,
                                                                                 ViAttr attributeId,
                                                                                 ViInt32 *value);

static ViStatus _VI_FUNC auml24xxAttrActiveCalibrationFactorNumber_WriteCallback (ViSession vi,
                                                                                  ViSession io,
                                                                                  ViConstString channelName,
                                                                                  ViAttr attributeId,
                                                                                  ViInt32 value);

static ViStatus _VI_FUNC auml24xxAttrFenceStartTime_CheckCallback (ViSession vi,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 value);

static ViStatus _VI_FUNC auml24xxAttrFenceStopTime_CheckCallback (ViSession vi,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value);

static ViStatus _VI_FUNC auml24xxAttrDualChannelModeEnabled_ReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViBoolean *value);

static ViStatus _VI_FUNC auml24xxAttrDualChannelModeEnabled_WriteCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViBoolean value);

static ViStatus _VI_FUNC auml24xxAttrActiveChannel_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value);

static ViStatus _VI_FUNC auml24xxAttrRefOscillatorEnabled_ReadCallback (ViSession vi,
                                                                        ViSession io,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViBoolean *value);

static ViStatus _VI_FUNC auml24xxAttrPAEViInt32_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 value);


static ViStatus _VI_FUNC auml24xxAttrBncOutputSourceChannel_CheckCallback (ViSession vi,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViInt32 value);

static ViStatus _VI_FUNC auml24xxAttrRangeAutoEnabled_CheckCallback (ViSession vi,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViBoolean value);

static ViStatus _VI_FUNC auml24xxAttrPAEViInt32_CheckCallback (ViSession vi,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViInt32 value);

static ViStatus _VI_FUNC auml24xx_SetupChannelCount (ViSession vi);    

/*****************************************************************************
 *------------ User-Callable Functions (Exportable Functions) ---------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: auml24xx_init   
 * Purpose:  VXIplug&play required function.  Calls the   
 *           auml24xx_InitWithOptions function.   
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_init (ViRsrc resourceName, ViBoolean IDQuery,
                                 ViBoolean resetDevice, ViSession *newVi)
{   
    ViStatus    error = VI_SUCCESS;

    if (newVi == VI_NULL)
        {
        Ivi_SetErrorInfo (VI_NULL, VI_FALSE, IVI_ERROR_INVALID_PARAMETER, 
                          VI_ERROR_PARAMETER4, "Null address for Instrument Handle");
        checkErr( IVI_ERROR_INVALID_PARAMETER);
        }

    checkErr( auml24xx_InitWithOptions (resourceName, IDQuery, resetDevice, 
                                        "", newVi));

Error:  
    return error;
}

/*****************************************************************************
 * Function: auml24xx_InitWithOptions                                       
 * Purpose:  This function creates a new IVI session and calls the 
 *           IviInit function.                                     
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_InitWithOptions(
    ViRsrc          resourceName,
    ViBoolean       IDQuery,
    ViBoolean       resetDevice,
    ViConstString   optionString,
    ViSession      *newVi
)
{   
    ViStatus    error = VI_SUCCESS;
    ViSession   vi = VI_NULL;
    ViChar      newResourceName[IVI_MAX_MESSAGE_BUF_SIZE];
    ViChar      newOptionString[IVI_MAX_MESSAGE_BUF_SIZE];
    ViBoolean   isLogicalName;

    if (newVi == VI_NULL)
        {
        Ivi_SetErrorInfo (VI_NULL, VI_FALSE, IVI_ERROR_INVALID_PARAMETER, 
                          VI_ERROR_PARAMETER5, "Null address for Instrument Handle");
        checkErr( IVI_ERROR_INVALID_PARAMETER);
        }

    *newVi = VI_NULL;

    checkErr( Ivi_GetInfoFromResourceName( resourceName, (ViString)optionString,
                                           newResourceName,
                                           newOptionString,
                                           &isLogicalName));

        /* create a new interchangeable driver */
    checkErr( Ivi_SpecificDriverNew ("auml24xx", newOptionString, &vi));  
    if (!isLogicalName)
    {
        ViInt32 oldFlag = 0;

        checkErr (Ivi_GetAttributeFlags (vi, IVI_ATTR_IO_RESOURCE_DESCRIPTOR, &oldFlag));
        checkErr (Ivi_SetAttributeFlags (vi, IVI_ATTR_IO_RESOURCE_DESCRIPTOR, oldFlag & 0xfffb | 0x0010));
        checkErr (Ivi_SetAttributeViString (vi, "", IVI_ATTR_IO_RESOURCE_DESCRIPTOR, 0, newResourceName));
    }
        /* init the driver */
    checkErr( auml24xx_IviInit (newResourceName, IDQuery, resetDevice, vi)); 
    if (isLogicalName)
        checkErr( Ivi_ApplyDefaultSetup (vi));
    *newVi = vi;

Error:
    if (error < VI_SUCCESS) 
        Ivi_Dispose (vi);

    return error;
}

/*****************************************************************************
 * Function: auml24xx_close                                                           
 * Purpose:  This function closes the instrument.                            
 *
 *           Note:  This function must unlock the session before calling
 *           Ivi_Dispose.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_close (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    checkErr( auml24xx_IviClose (vi));

Error:    
    Ivi_UnlockSession (vi, VI_NULL);
    Ivi_Dispose (vi);  

    return error;
}

/*****************************************************************************
 * Function: auml24xx_reset                                                         
 * Purpose:  This function resets the instrument.                          
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_reset (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	    {
	    ViSession   io = Ivi_IOSession(vi); /* call only when locked */

	    checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
	    viCheckErr( auml24xx_ViPrintf (io, "*RST"));
	    }

	checkErr( auml24xx_DefaultInstrSetup (vi));                                

	checkErr( auml24xx_CheckStatus (vi));                                      

Error:
    Ivi_UnlockSession (vi, VI_NULL);
    return error;
}
					  
/*****************************************************************************
 * Function: auml24xx_ResetWithDefaults
 * Purpose:  This function resets the instrument and applies default settings
 *           from the IVI Configuration Store based on the logical name
 *           from which the session was created.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ResetWithDefaults (ViSession vi)
{
    ViStatus error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    checkErr( auml24xx_reset(vi));
    checkErr( Ivi_ApplyDefaultSetup(vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/**************************************************************************** 
 *  Function: auml24xx_Disable
 *  Purpose:  This function is not supported by the auml24xx instrument.
 ****************************************************************************/
ViStatus _VI_FUNC auml24xx_Disable (ViSession vi)
{
    ViStatus error = VI_SUCCESS;

    return error;
}

/*****************************************************************************
 * Function: auml24xx_self_test                                                       
 * Purpose:  This function executes the instrument self-test and returns the 
 *           result.                                                         
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_self_test (ViSession vi, ViInt16 *testResult, 
                                      ViChar testMessage[])
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (testResult == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Test Result");
    if (testMessage == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Test Message");

	if (!Ivi_Simulating(vi))                /* call only when locked */
	    {
	    ViSession   io = Ivi_IOSession(vi); /* call only when locked */

	    checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
	    viCheckErr( auml24xx_ViPrintf (io, "*TST?"));

		viCheckErr( auml24xx_ViScanf (io, "%s",testMessage));

	    }
	else
	    {
	        /* Simulate Self Test */
	    strcpy (testMessage, "No error.");
	    }
	*testResult = 0; 
	checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_revision_query                                                  
 * Purpose:  This function returns the driver and instrument revisions.      
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_revision_query (ViSession vi, ViChar driverRev[], 
                                           ViChar instrRev[])
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (driverRev == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Driver Revision");
    if (instrRev == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Instrument Revision");

    checkErr( Ivi_GetAttributeViString (vi, VI_NULL, AUML24XX_ATTR_SPECIFIC_DRIVER_REVISION, 
                                        0, 256, driverRev));
    checkErr( Ivi_GetAttributeViString (vi, "", AUML24XX_ATTR_INSTRUMENT_FIRMWARE_REVISION, 
                                        0, 256, instrRev));
    checkErr( auml24xx_CheckStatus (vi));

Error:    
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_error_query                                                     
 * Purpose:  This function queries the instrument error and return the error 
 * message
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_error_query (ViSession vi, ViInt32 *errCode,
	                                    ViChar errMessage[])
{
    ViStatus    error = VI_SUCCESS;
	ViChar      errorInfor[BUFFER_SIZE*2] = "";
 	ViChar      firstSeg[BUFFER_SIZE] = "";
	ViChar      secondSeg[BUFFER_SIZE] = "";
	ViChar      thirdSeg[BUFFER_SIZE] = "";
	ViChar      buffer[BUFFER_SIZE] = "";
	ViInt32     index,errorBit;
                                                                                       	
    checkErr( Ivi_LockSession (vi, VI_NULL));
	strcpy(errMessage,"");
	
    if (errMessage == VI_NULL)                                                         	
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Error Message");	
                                                                                       	
	if (!Ivi_Simulating(vi))                /* call only when locked */                	
	  {                                                                              	
	    ViSession   io = Ivi_IOSession(vi); /* call only when locked */                	
                                                                                       	
	    checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));                             	
	    viCheckErr( auml24xx_ViPrintf(io, "SYERLST"));                                       	
                                                                                       	
		viCheckErr(auml24xx_ViScanf (io, "%s", buffer)); 
		viCheckErr(Scan(buffer,"%s>%s[t33x]%s[t33x]%s[t33x]",firstSeg,secondSeg,thirdSeg));
		for( index = 0; index != 13; index++)
		 {
            errorBit = (firstSeg[index]-'0');
	    	viCheckErr(auml24xx_ParseErrorBit(errorBit,index,errorInfor));
			strcat(errMessage,errorInfor);
	     }
		
		if(strcmp(secondSeg,"")!=0)
		 {
			buffer[0] = '\0';
			Fmt(buffer,"%s<%s%s%s","Command \"",secondSeg,"\" Error\n");
			strcat(errMessage,buffer);
		 }
		if(strcmp(thirdSeg,"")!=0)
		 {
			buffer[0] = '\0';
			Fmt(buffer,"%s<%s%s%s","Execution \"",thirdSeg,"\" Error\n");
			strcat(errMessage,buffer);
		 }
	    if(strcmp(errMessage,"")==0)
		   strcpy(errMessage,"No Error");                                                                                 	
 
	  }
	else                                                                               	
	    {                                                                              	
	        /* Simulate Error Query */                                                 	
                                                           	
	    strcpy (errMessage, "No error.");                                              	
	    }   
	*errCode = 0; 
                                                                                       	
Error:                                                                                 	
    Ivi_UnlockSession(vi, VI_NULL);                                                    
	
    return error;
}

/*****************************************************************************
 * Function: auml24xx_error_message                                                  
 * Purpose:  This function translates the error codes returned by this       
 *           instrument driver into user-readable strings.  
 *
 *           Note:  The caller can pass VI_NULL for the vi parameter.  This 
 *           is useful if one of the init functions fail.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_error_message (ViSession vi, ViStatus errorCode,
                                          ViChar errorMessage[256])
{
    ViStatus    error = VI_SUCCESS;

    static      IviStringValueTable errorTable = 
        {
  			{AUML24XX_ERROR_ATTR_NO_INTERNAL_TRIIGER_SOURCE, "No internal trigger source has been set."},
			{AUML24XX_ERROR_ATTR_AVERAGE_NUMBER_UNAVAILABLE, "auto averaging enabled, manual average number unavailable"},
			IVIPWRMETER_ERROR_CODES_AND_MSGS,
			{VI_NULL,                               VI_NULL}
        };

    if (vi)
        Ivi_LockSession(vi, VI_NULL);

        /* all VISA and IVI error codes are handled as well as codes in the table */
    if (errorMessage == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Error Message");

    checkErr( Ivi_GetSpecificDriverStatusDesc(vi, errorCode, errorMessage, errorTable));

Error:
    if (vi)
        Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_InvalidateAllAttributes
 * Purpose:  This function invalidates the cached value of all attributes.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_InvalidateAllAttributes (ViSession vi)
{
    return Ivi_InvalidateAllAttributes(vi);
}

/*****************************************************************************
 * Function: auml24xx_GetError and auml24xx_ClearError Functions                       
 * Purpose:  These functions enable the instrument driver user to  
 *           get or clear the error information the driver associates with the
 *           IVI session.                                                        
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_GetError (ViSession vi, 
                                     ViStatus *errorCode, 
                                     ViInt32 bufferSize,
                                     ViChar description[])  
{
    ViStatus error = VI_SUCCESS;
    ViStatus primary = VI_SUCCESS,
             secondary = VI_SUCCESS;
    ViChar   elaboration[256] = "";
    ViChar   errorMessage[1024] = "";
    ViChar  *appendPoint = errorMessage;
    ViInt32  actualSize = 0;
    ViBoolean locked = VI_FALSE;

    /* Lock Session */
    if (vi != 0)
        {
        checkErr( Ivi_LockSession(vi, &locked));
        }

    /* Test for nulls and acquire error data */
    if (bufferSize != 0)
        {
        if (errorCode == VI_NULL)
            {
            viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Error");
            }
        if (description == VI_NULL)
            {
            viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Description");
            }
        checkErr( Ivi_GetErrorInfo (vi, &primary, &secondary, elaboration));
        }

    else
        {
        checkErr( Ivi_GetAttributeViString(vi, VI_NULL, IVI_ATTR_ERROR_ELABORATION, 0, 256, elaboration));
        checkErr( Ivi_GetAttributeViInt32(vi, VI_NULL, IVI_ATTR_SECONDARY_ERROR, 0, &secondary));
        checkErr( Ivi_GetAttributeViInt32(vi, VI_NULL, IVI_ATTR_PRIMARY_ERROR, 0, &primary));
        }

    /* Format data */
    if (primary != VI_SUCCESS)
        {
        ViChar msg[256] = "";
        checkErr( auml24xx_error_message (vi, primary, msg));
        appendPoint += sprintf(appendPoint, "Primary Error: (Hex 0x%08X) %s\n", primary, msg);
        }

    if (secondary != VI_SUCCESS)
        {
        ViChar msg[256] = "";
        checkErr( auml24xx_error_message (vi, secondary, msg));
        appendPoint += sprintf(appendPoint, "Secondary Error: (Hex 0x%08X) %s\n", secondary, msg);
        }

    if (elaboration[0])
        {
        sprintf(appendPoint, "Elaboration: %s", elaboration);
        }

    actualSize = strlen(errorMessage) + 1;

    /* Prepare return values */
    if (bufferSize == 0)
        {
        error = actualSize;
        }
    else 
        {
        if (bufferSize > 0)
            {
            if (actualSize > bufferSize)
                {
                error = actualSize;
                actualSize = bufferSize;
                }
            }
        memcpy(description, errorMessage, actualSize-1);
        description[actualSize-1] = '\0';
        }

    if (errorCode) 
        {
        *errorCode = primary;
        }

Error:
    /* Unlock Session */
    Ivi_UnlockSession (vi, &locked);
    return error;
}   

ViStatus _VI_FUNC auml24xx_ClearError (ViSession vi)                                                        
{                                                                                                           
    return Ivi_ClearErrorInfo (vi);                                                                             
}

/*****************************************************************************
 * Function: auml24xx_GetNextCoercionRecord                        
 * Purpose:  This function enables the instrument driver user to obtain
 *           the coercion information associated with the IVI session.                                                              
 *           This function retrieves and clears the oldest instance in which 
 *           the instrument driver coerced a value the instrument driver user
 *           specified to another value.                     
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_GetNextCoercionRecord (ViSession vi,
                                                  ViInt32 bufferSize,
                                                  ViChar  record[])
{
    return Ivi_GetNextCoercionString (vi, bufferSize, record);
}

/**************************************************************************** 
 *  Function: auml24xx_GetNextInterchangeWarning,
 *            auml24xx_ResetInterchangeCheck, and
 *            auml24xx_ClearInterchangeWarnings
 *  Purpose:  These functions allow the user to retrieve interchangeability
 *            warnings, reset the driver's interchangeability checking 
 *            state, and clear all previously logged interchangeability warnings.
 ****************************************************************************/
ViStatus _VI_FUNC auml24xx_GetNextInterchangeWarning (ViSession vi, 
                                                      ViInt32 bufferSize, 
                                                      ViChar warnString[])
{
    return Ivi_GetNextInterchangeCheckString (vi, bufferSize, warnString);
}

ViStatus _VI_FUNC auml24xx_ResetInterchangeCheck (ViSession vi)
{
    return Ivi_ResetInterchangeCheck (vi);
}

ViStatus _VI_FUNC auml24xx_ClearInterchangeWarnings (ViSession vi)
{
    return Ivi_ClearInterchangeWarnings (vi);
}

/*****************************************************************************
 * Function: auml24xx_LockSession and auml24xx_UnlockSession Functions                        
 * Purpose:  These functions enable the instrument driver user to lock the 
 *           session around a sequence of driver calls during which other
 *           execution threads must not disturb the instrument state.
 *                                                                          
 *           NOTE:  The callerHasLock parameter must be a local variable 
 *           initialized to VI_FALSE and passed by reference, or you can pass 
 *           VI_NULL.                     
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_LockSession (ViSession vi, ViBoolean *callerHasLock)  
{                                              
    return Ivi_LockSession(vi,callerHasLock);      
}                                              
ViStatus _VI_FUNC auml24xx_UnlockSession (ViSession vi, ViBoolean *callerHasLock) 
{                                              
    return Ivi_UnlockSession(vi,callerHasLock);    
}   

/**************************************************************************** 
 *  Function: auml24xx_GetChannelName
 *  Purpose:  This function returns the highest-level channel name that
 *            corresponds to the specific driver channel string that is in
 *            the channel table at an index you specify.
 ****************************************************************************/
ViStatus _VI_FUNC auml24xx_GetChannelName (ViSession vi, ViInt32 index,
                                           ViInt32 bufferSize,
                                           ViChar name[])
{
    ViStatus         error = VI_SUCCESS;
    ViConstString    channelName;
    ViInt32          actualSize = 0;
    ViInt32			channelCount;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    /* Check parameters */
    if (bufferSize != 0)
        {
        if (name == VI_NULL)
            {
            viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Channel Name");
            }
        }

    checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_CHANNEL_COUNTS, 0, &channelCount));
    if ( (index<1) || (index>channelCount) )
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Channel Index");

    /* Get channel name */
    viCheckParm( Ivi_GetNthChannelString (vi, index, &channelName), 2, "Index");
    checkErr( Ivi_GetUserChannelName (vi, channelName, &channelName));

    actualSize = strlen(channelName) + 1;

    /* Prepare return values */
    if (bufferSize == 0)
        {
        error = actualSize;
        }
    else 
        {
        if (bufferSize > 0)
            {
            if (actualSize > bufferSize)
                {
                error = actualSize;
                actualSize = bufferSize;
                }
            }
        memcpy(name, channelName, actualSize-1);
        name[actualSize-1] = '\0';
        }

Error:    
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*===========================================================================*
 *= IviPwrMeter Base Configuration Functions ================================*
 *===========================================================================*/

/*****************************************************************************
 * Function: auml24xx_ConfigureMeasurement
 * Purpose:  This function configures the instrument to take single or dual
 * sensor measurements on active channel. Use
 * ConfigureActiveChannel function to set active channel before
 * this configuration.

 * To configure the power meter to take single sensor measurements,
 * pass No Operator as the value of the Operator parameter. To
 * configure the power meter to take simultaneous dual sensor
 * measurements, pass one of the defined math operators as the
 * value of the Operator parameter.

 * If you pass No Operator as the value of the Operator parameter,
 * this function enables the sensor specified by the Sensor Name 1
 * parameter and disables all other sensors. The value of the
 * Sensor Name 2 parameter is ignored.

 * If you do not pass No Operator as the value of the Operator
 * parameter, this function enables the sensors specified by the
 * Sensor Name 1 and Sensor Name 2 parameters.
*****************************************************************************/

ViStatus _VI_FUNC auml24xx_ConfigureMeasurement(
    ViSession       vi,
    ViInt32         mathOperator,
    ViConstString   sensorNameOne,
    ViConstString   sensorNameTwo
)
{
    ViStatus error = VI_SUCCESS;
	ViInt32  activeChannel;
	ViChar   channel[BUFFER_SIZE];

    checkErr( Ivi_LockSession (vi, VI_NULL));


	if (!Ivi_Simulating(vi))                /* call only when locked */
  {
	checkErr( Ivi_CoerceRepCapName(vi, SENSOR_REP_CAP_NAME, sensorNameOne, &sensorNameOne));                  
	checkErr( Ivi_CoerceRepCapName(vi, SENSOR_REP_CAP_NAME, sensorNameTwo, &sensorNameTwo));                  

	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_ACTIVE_CHANNEL, 0, &activeChannel));   
	sprintf(channel,"%d",activeChannel);

	if(mathOperator==AUML24XX_VAL_CHAN_CONFIG_V)
	{
	  Ivi_SetAttributeViInt32 (vi, channel, AUML24XX_ATTR_CHANNEL_INPUT_CONFIG, 0,AUML24XX_VAL_CHAN_CONFIG_V);
	}
	else if(mathOperator== AUML24XX_VAL_NONE)
	{
		 if(!strcmp(sensorNameOne,"A"))
		 {
			 Ivi_SetAttributeViInt32 (vi, channel, AUML24XX_ATTR_CHANNEL_INPUT_CONFIG, 0,AUML24XX_VAL_CHAN_CONFIG_A);
		 }
		 else if(!strcmp(sensorNameOne,"B"))  
		 {
			 Ivi_SetAttributeViInt32 (vi, channel, AUML24XX_ATTR_CHANNEL_INPUT_CONFIG, 0,AUML24XX_VAL_CHAN_CONFIG_B);   
		 }

	}
	else if(strcmp(sensorNameOne,sensorNameTwo)&&!strcmp(sensorNameOne,"A")&&!strcmp(sensorNameTwo,"B"))
	{
		switch(mathOperator)
		{
			case AUML24XX_VAL_DIFFERENCE:
        	     Ivi_SetAttributeViInt32 (vi, channel, AUML24XX_ATTR_CHANNEL_INPUT_CONFIG, 0,AUML24XX_VAL_CHAN_CONFIG_A_MINUS_B);
			     break;
			case AUML24XX_VAL_QUOTIENT:
        	     Ivi_SetAttributeViInt32 (vi, channel, AUML24XX_ATTR_CHANNEL_INPUT_CONFIG, 0,AUML24XX_VAL_CHAN_CONFIG_A_DIVIDED_B);
				 break;
			default:
			     viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);  
				 break;
		}
	}
	else if(strcmp(sensorNameOne,sensorNameTwo)&&!strcmp(sensorNameOne,"B")&&!strcmp(sensorNameTwo,"A"))
	{
		switch(mathOperator)
		{
			case AUML24XX_VAL_DIFFERENCE:
        	     Ivi_SetAttributeViInt32 (vi, channel, AUML24XX_ATTR_CHANNEL_INPUT_CONFIG, 0,AUML24XX_VAL_CHAN_CONFIG_B_MINUS_A);
				 break;
			case AUML24XX_VAL_QUOTIENT:
        	     Ivi_SetAttributeViInt32 (vi, channel, AUML24XX_ATTR_CHANNEL_INPUT_CONFIG, 0,AUML24XX_VAL_CHAN_CONFIG_B_DIVIDED_A);
				 break;
			default:
			     viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);  
				 break;
		}
	}
    else
	{
			     viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);  
	}
  }

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************             
 * Function: auml24xx_ConfigureActiveChannel                                               
 * Purpose:  This function configures the active channel for Units,Trigger                 
 *           Source, Internal Trigger Configuration.                                       
 *****************************************************************************/            
ViStatus _VI_FUNC auml24xx_ConfigureActiveChannel(                                         
	ViSession  vi,                                                                         
	ViConstString Channel
)                                                                                          
{                                                                                          
    ViStatus    error = VI_SUCCESS;                                                        

    checkErr( Ivi_LockSession (vi, VI_NULL));                                              

	viCheckParm( Ivi_CoerceChannelName (vi, Channel, &Channel),2, "Channel");
	if(!strcmp(Channel,"1"))
	{
	   viCheckParm( Ivi_SetAttributeViInt32(vi, VI_NULL,AUML24XX_ATTR_ACTIVE_CHANNEL, 0,
                                          1),  2, "Channel Number");
	}
	else if(!strcmp(Channel,"2") )
	{
	   viCheckParm( Ivi_SetAttributeViInt32(vi, VI_NULL,AUML24XX_ATTR_ACTIVE_CHANNEL, 0,
                                          2),  2, "Channel Number");
	}
 
    checkErr( auml24xx_CheckStatus (vi));                                                  

Error:                                                                                     
    Ivi_UnlockSession(vi, VI_NULL);                                                        
    return error;                                                                          

}                                                                                          

/*****************************************************************************
 * Function: auml24xx_ConfigureUnits
 * Purpose:     This function configures the auto range mode for a given channel
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureUnits(
    ViSession   vi,
    ViInt32     units
)
{
    ViStatus    error = VI_SUCCESS;
	checkErr( Ivi_LockSession (vi, VI_NULL));

	viCheckParm( Ivi_SetAttributeViInt32 (vi, "", AUML24XX_ATTR_UNITS, 0,units), 2, "Units");

    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: auml24xx_ConfigureRangeAutoEnabled
 * Purpose:     This function is not supported by the auml24xx instrument.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureRangeAutoEnabled(
    ViSession       vi,
    ViConstString   channelName,
    ViBoolean       rangeAutoEnabled
)
{
    ViStatus      error = VI_SUCCESS;
    return error;

}

/*****************************************************************************
 * Function: auml24xx_ConfigureAveragingAutoEnabled
 * Purpose:  This function enables or disables the auto-averaging mode
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureAveragingAutoEnabled(
    ViSession       vi,
    ViConstString   channelName,
    ViBoolean       averagingAutoEnabled
)
{
    ViStatus      error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

	viCheckParm(Ivi_SetAttributeViBoolean( vi,
                                      channelName,
                                      AUML24XX_ATTR_AVERAGING_AUTO_ENABLED,
                                      0,
                                      averagingAutoEnabled),3,"Auto Averaging Enabled");

	checkErr( auml24xx_CheckStatus (vi));
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureAveragingEnabled
 * Purpose:  This function enables or disables the auto-averaging mode
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureAveragingEnabled(
	ViSession  vi,
	ViConstString Channel,
	ViBoolean AveragingEnabled
)
{
    ViStatus      error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

	viCheckParm(Ivi_SetAttributeViBoolean( vi,
                                      Channel,
                                      AUML24XX_ATTR_AVERAGING_ENABLED,
                                      0,
                                      AveragingEnabled),3,"Averaging Enabled");

	checkErr( auml24xx_CheckStatus (vi));
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureCorrectionFrequency
 * Purpose: This function is not supported by the auml24xx instrument. Use
 *          auml24xx_ConfigureSensorCorrectionFrequency to configure
 *         correction frequency on sensor.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureCorrectionFrequency(
    ViSession       vi,
    ViConstString   channelName,
    ViReal64        correctionFrequency
)
{
    ViStatus    error = VI_SUCCESS;

    return error;

}

/*****************************************************************************
 * Function: auml24xx_ConfigureMeasurementMode
 * Purpose:   This function configures the measurment mode(CW/PMOD) on the
 *            specified channel.
 *****************************************************************************/
ViStatus _VI_FUNC  auml24xx_ConfigureMeasurementMode(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 MeasurementMode
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViInt32 (vi, Channel, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0,
                                          MeasurementMode), 3, "Measurment Mode");

    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: auml24xx_ConfigureSettlePercentage
 * Purpose:  This function configures the settle percentage of the
 *             instrument.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureSettlePercentage(
	 ViSession  vi,ViConstString Channel,
	 ViReal64 SettlePercentage
)
 {
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViReal64 (vi, Channel, AUML24XX_ATTR_SETTLE_PERCENT, 0,
                                          SettlePercentage), 3, "Settle Percentage");

    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

 }

/*****************************************************************************
 * Function: auml24xx_ConfigureOffset
 * Purpose: This function is not supported by the auml24xx instrument. Use 
 *          auml24xx_ConfigureSensorOffset to configure offset on sensor.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureOffset(
    ViSession       vi,
    ViConstString   channelName,
    ViReal64        offset
)
{
    ViStatus    error = VI_SUCCESS;

    return error;

}

/*****************************************************************************
 * Function: auml24xx_ConfigurePulsedModulatedMeasurement
 * Purpose:  This function configures the settings of the PMOD measurement.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigurePulsedModulatedMeasurement(
	 ViSession  vi,
	 ViConstString Channel,
	 ViInt32 MeasurementType,
	 ViInt32 DisplayType,
	 ViInt32 SampleRate
)
 {
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViInt32 (vi, Channel, AUML24XX_ATTR_PMOD_MEASUREMENT_TYPE, 0,
                                          MeasurementType), 3, "Measurement Type");
    viCheckParm( Ivi_SetAttributeViInt32 (vi, Channel, AUML24XX_ATTR_PMOD_MEASUREMENT_DISPLAY, 0,
                                          DisplayType), 4, "Display Type");
    viCheckParm( Ivi_SetAttributeViInt32 (vi, Channel, AUML24XX_ATTR_SAMPLE_RATE, 0,
                                          SampleRate), 5, "Sample Rate");
	
    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
 }


/*****************************************************************************
 * Function: auml24xx_ConfigureRelativeMeasurement
 * Purpose:  This function configures the relative mode for CW measurements.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureRelativeMeasurement(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 RelativeModeControl
)
 {
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViInt32 (vi, Channel, AUML24XX_ATTR_RELATIVE_MODE, 0,
                                          RelativeModeControl), 3, "Relative ModeControl");

    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
 }


/*===========================================================================*
 *= IviPwrMeter Trigger Source Extension Group ==============================*
 *===========================================================================*/
/*****************************************************************************
 * Function: auml24xx_ConfigureTriggerSource
 * Purpose:     This function sets the trigger source of the power meter
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureTriggerSource(
    ViSession   vi,
    ViInt32     triggerSource
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_TRIGGER_SOURCE, 0,
                                          triggerSource), 2, "Trigger Source");

    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureGET
 * Purpose: This function configures the Group Execute Trigger type.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureGET(
	ViSession  vi,
	ViInt32 GETType
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		switch(GETType)
		{
			case AUML24XX_VAL_GET_0:
				 viCheckErr( auml24xx_ViPrintf (io, "GT0"));  
				 break;
			case AUML24XX_VAL_GET_1:
				 viCheckErr( auml24xx_ViPrintf (io, "GT1"));  
				 break;
			case AUML24XX_VAL_GET_2:
				 viCheckErr( auml24xx_ViPrintf (io, "GT2"));  
				 break;				
			default:
			     viCheckErr (IVI_ERROR_INVALID_VALUE);
			     break;

		}
	}
	  checkErr( auml24xx_CheckStatus (vi));
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}


/*****************************************************************************
 * Function: auml24xx_ConfigureTrigger
 * Purpose: This function configures the trigger arming mode, capture time
 *          and trigger delay time under CW or PMOD measurement mode. Before
 *          calling this function, measurement mode must be configured first.
 *
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureTrigger(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 ArmingMode,
	ViReal64 CaptureTime,
	ViReal64 DelayTime
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViInt32 (vi, Channel, AUML24XX_ATTR_TRIGGER_ARMING_MODE, 0,
                                          ArmingMode), 3, "Arming Mode");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, Channel, AUML24XX_ATTR_CAPTURE_TIME, 0,
                                          CaptureTime), 4, "Capture Time");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, Channel, AUML24XX_ATTR_TRIGGER_DELAY_TIME, 0,
                                          DelayTime), 5, "Delay Time");

    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
 }

/*****************************************************************************
 * Function: auml24xx_ConfigureTriggerFrameArming
 * Purpose: This function configures the frame arming trigger
 *          characteristics.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureTriggerFrameArming(
	ViSession  vi,
	ViConstString Channel,  
	ViReal64 ArmingLevel,
	ViReal64 ArmingTimeDuration
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViReal64 (vi, Channel, AUML24XX_ATTR_TRIGGER_FRAME_ARMING_LEVEL, 0,
                                          ArmingLevel), 3, "Arming Level");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, Channel, AUML24XX_ATTR_TRIGGER_FRAME_ARMING_TIME_DURATION, 0,
                                          ArmingTimeDuration), 4, "Arming Time Duration");

    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
 }

/*****************************************************************************
 * Function: auml24xx_ConfigureTriggerLinkingEnabled
 * Purpose:  Turns the trigger linking on of off.
 * Remarks: This option allows both measurement channels to share
 *          the same measurement triggering set up.
 * Note: Both channels must be set to the same measurement mode. If
 * failing to meet this condition the instrument will produce an
 * execution error.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureTriggerLinkingEnabled(
	ViSession  vi,
	ViBoolean TriggerLinkingEnabled
)
 {
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViBoolean (vi, VI_NULL, AUML24XX_ATTR_TRIGGER_LINKING_ENABLED, 0,
                                          TriggerLinkingEnabled), 2, "Trigger Linking State");
    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
 }

/*****************************************************************************
 * Function: auml24xx_ConfigurePMODTriggerHoldoff
 * Purpose:  This function configures the PMOD trigger hold off.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigurePMODTriggerHoldoff(
	ViSession  vi,
	ViConstString Channel,
    ViBoolean TriggerHoldoffEnabled,
	ViReal64 TriggerHoldoffTime)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViBoolean (vi, Channel, AUML24XX_ATTR_TRIGGER_HOLDOFF_ENABLED, 0,
                                          TriggerHoldoffEnabled),3, "Trigger Holdoff State");
	if(TriggerHoldoffEnabled)
	{

    	viCheckParm( Ivi_SetAttributeViReal64 (vi, Channel, AUML24XX_ATTR_TRIGGER_HOLDOFF_TIME, 0,
                                          TriggerHoldoffTime), 4, "Trigger Holdoff Time");
	}
    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigurePMODTrigger
 * Purpose:  This function configures the auto triggering and trigger
 *           bandwidth.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigurePMODTrigger(
	ViSession  vi,
	ViConstString Channel,
	ViBoolean AutoTriggeringEnabled,
	ViInt32 TriggerBandwidth
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViBoolean (vi, Channel, AUML24XX_ATTR_TRIGGER_AUTO_ENABLED, 0,
                                          AutoTriggeringEnabled), 3, "Auto Triggering Enabled");
    viCheckParm( Ivi_SetAttributeViInt32 (vi, Channel, AUML24XX_ATTR_TRIGGER_BANDWIDTH, 0,
                                          TriggerBandwidth), 4, "Trigger Bandwidth");

    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
 }

/*****************************************************************************
 * Function: auml24xx_ConfigureExternalTriggerEdge
 * Purpose:  This function configures the external trigger edge.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureExternalTriggerEdge(
	ViSession  vi,
	ViInt32 ExternaTriggerEdge
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViInt32 (vi,VI_NULL, AUML24XX_ATTR_TRIGGER_EXTERNAL_EDGE, 0,
                                          ExternaTriggerEdge), 2, "Externa Trigger Edge");
    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
 }

/*****************************************************************************
 * Function: auml24xx_EnterTriggerHoldMode
 * Purpose: This function places the instrument into trigger hold mode.
 *****************************************************************************/

ViStatus _VI_FUNC auml24xx_EnterTriggerHoldMode(
    ViSession   vi
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
        viCheckErr( auml24xx_ViPrintf (io, "TR0"));

	}
	  checkErr( auml24xx_CheckStatus (vi));
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_QueryRRSTraceState
 * Purpose:  This functions queries the state off the RRS trace state.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_QueryRRSTraceState(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 *RRSTraceState
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    checkErr( Ivi_GetAttributeViInt32 (vi,Channel, AUML24XX_ATTR_RRS_TRACE_STATE, 0,RRSTraceState));
    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*===========================================================================*
 *= IviPwrMeter Internal Trigger Extension Group ============================*
 *===========================================================================*/
/*****************************************************************************
 * Function: auml24xx_ConfigureInternalTrigger
 * Purpose:     This function configures the internal trigger event source and
 *              the internal trigger slope of the power meter.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureInternalTrigger(
    ViSession       vi,
    ViConstString   eventSource,
    ViInt32         slope
)
{
    ViStatus error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViString( vi,
                                           VI_NULL,
                                           AUML24XX_ATTR_INTERNAL_TRIGGER_EVENT_SOURCE,
                                           0, eventSource),
                                           2, "Event Source");

    viCheckParm( Ivi_SetAttributeViInt32( vi,
                                          VI_NULL,
                                          AUML24XX_ATTR_INTERNAL_TRIGGER_SLOPE,
                                          0, slope),
                                          3, "Slope");

    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureInternalTriggerLevel
 * Purpose:     This function configures the internal trigger level of the power
 *              meter
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureInternalTriggerLevel(
    ViSession   vi,
    ViReal64    triggerLevel
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AUML24XX_ATTR_INTERNAL_TRIGGER_LEVEL, 0,
                                          triggerLevel), 2, "Trigger Level");

    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;  
}

/*===========================================================================*
 *= IviPwrMeter Averaging Count Extension Group  ============================*
 *===========================================================================*/
/*****************************************************************************
 * Function: auml24xx_ConfigureAveragingCount
 * Purpose:     This function sets the manual average count that the instrument
 *              uses in manual averaging mode. The averaging count specifies
 *              the number of samples that the instrument takes before the
 *              measurement is complete.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureAveragingCount(
    ViSession       vi,
    ViConstString   channelName,
    ViInt32         count
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_AVERAGING_COUNT, 0,
                                          count), 3, "Count");

    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureCWAveragingMode
 * Purpose:  This function configures the averaging mode for CW measurment.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureCWAveragingMode(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 AveragingMode
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViInt32 (vi,Channel, AUML24XX_ATTR_CW_AVERAGING_MODE, 0,
                                         AveragingMode), 3, "Averaging Mode");
    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
 }

/*****************************************************************************
 * Function: auml24xx_ResetPMODProfileAveraging
 * Purpose:  This function resets the Pulsed/Modulated Profile Averaging.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ResetPMODProfileAveraging(
	ViSession  vi,
	ViConstString Channel
)
{
    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceChannelName (vi, Channel, &Channel) );

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "PMAVRST %s",Channel));  
	}
	  checkErr( auml24xx_CheckStatus (vi));
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*===========================================================================*
 *= IviPwrMeter Duty Cycle Correction Extension Group =======================*
 *===========================================================================*/

/*****************************************************************************
 * Function: auml24xx_ConfigureDutyCycleCorrection
 * Purpose:     This function enables or disables the duty cycle correction and
 *              sets the duty cycle correction for pulse power measurements.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureDutyCycleCorrection(
    ViSession       vi,
    ViConstString   channelName,
    ViBoolean       correctionEnabled,
    ViReal64        correction
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViBoolean( vi,
                                            channelName,
                                            AUML24XX_ATTR_DUTY_CYCLE_CORRECTION_ENABLED,
                                            0, correctionEnabled),
                                            3, "Correction Enabled");

     viCheckParm( Ivi_SetAttributeViReal64( vi,
                                            channelName,
                                            AUML24XX_ATTR_DUTY_CYCLE_CORRECTION,
                                            0, correction),
                                            4, "Correction");

    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ResetMinMaxTracking
 * Purpose:  This function resets the min/max values for the CW
             measurement mode if Min/Max tracking state is enabled.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ResetMinMaxTracking(
	ViSession  vi,
	ViConstString Channel
)
{
    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	checkErr( Ivi_CoerceChannelName (vi, Channel, &Channel) );

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "CWMMRST %s",Channel));  
	}
	  checkErr( auml24xx_CheckStatus (vi));
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

                                                                                 
/*****************************************************************************
 * Function: auml24xx_ConfigureMinMaxTrackingEnabled
 * Purpose:  This function turns Min and Max Values Tracking State on or off.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureMinMaxTrackingEnabled(
	ViSession  vi,
	ViConstString Channel,
	ViBoolean TrackingEnabled
)

{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViBoolean (vi, Channel, AUML24XX_ATTR_MIN_MAX_TRACKING_ENABLED, 0,
                                          TrackingEnabled), 3, "Tracking State");

    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureDualChannelEnabled
 * Purpose:  This function turns the dual channel mode on or off.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureDualChannelEnabled(
	ViSession       vi,
	ViBoolean DualChannelEnabled
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViBoolean (vi, VI_NULL, AUML24XX_ATTR_DUAL_CHANNEL_MODE_ENABLED, 0,
                                          DualChannelEnabled), 2, "Test Type");

    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureActiveGatingPattern
 * Purpose:  This function configures the active gating pattern number. Fence
 *           and Gating settings will be configured on the active gating
 *           pattern.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureActiveGatingPattern(
	ViSession  vi,ViConstString Channel,
	ViInt32 ActiveGatingPattern
)
 {
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViInt32 (vi, Channel, AUML24XX_ATTR_GATE_PATTERN_CONFIGURATION_NUMBER, 0,
                                          ActiveGatingPattern), 3, "Active Gating Pattern");
    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
 }

/*****************************************************************************
 * Function: auml24xx_ConfigureRepeateGates
 * Purpose:  This function configures the Repeat Gates.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureRepeateGates(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 GatePattern1RepeatNumber,
	ViBoolean GatePattern1RepeatEnabled,
	ViReal64 GatePattern1RepeatOffset
)
 {
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
   viCheckParm( Ivi_SetAttributeViInt32 (vi, Channel, AUML24XX_ATTR_GATE_PATTERN_1_REPEAT_NUMBER, 0,
                                          GatePattern1RepeatNumber), 3, "Gate Pattern1 Repeat Number");
    viCheckParm( Ivi_SetAttributeViBoolean (vi, Channel, AUML24XX_ATTR_GATE_PATTERN_1_REPEAT_ENABLED, 0,
                                          GatePattern1RepeatEnabled),4, "Gate Pattern1 Repeat State");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, Channel, AUML24XX_ATTR_GATE_PATTERN_1_REPEAT_OFFSET, 0,
                                         GatePattern1RepeatOffset),5, "Gate Pattern1 Repeat Offset");
    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
 }
/*****************************************************************************
 * Function: auml24xx_ConfigureFence
 * Purpose:  This function configures the Fence setting on the current active
 *           gating pattern. Before calling this function, Active Gating
 *           Pattern number must be configured first.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureFence(
	ViSession  vi,
	ViConstString Channel,
	ViBoolean FenceEnabled,
	ViReal64 FenceStopTime,
	ViReal64 FenceStartTime
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
   viCheckParm( Ivi_SetAttributeViBoolean (vi, Channel, AUML24XX_ATTR_FENCE_ENABLED, 0,
                                          FenceEnabled), 3, "Fence Enabled");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, Channel, AUML24XX_ATTR_FENCE_STOP_TIME, 0,
                                          FenceStopTime),4, "Fence Stop Time");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, Channel, AUML24XX_ATTR_FENCE_START_TIME, 0,
                                         FenceStartTime),5, "Fence StartTime");
    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureGating
 * Purpose:  This function configures the Gate setting on the current active
 *           gating pattern. Before calling this function, Active Gating
 *           Pattern number must be configured first.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureGating(
	ViSession  vi,
	ViConstString Channel,
	ViBoolean GateEnabled,
	ViReal64 GateStopTime,
	ViReal64 GateStartTime
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViBoolean (vi, Channel, AUML24XX_ATTR_GATE_PATTERN_ENABLED, 0,
                                          GateEnabled), 3, "Gate Pattern Enabled");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, Channel, AUML24XX_ATTR_GATE_PATTERN_STOP_TIME, 0,
                                          GateStopTime),4, "Gate Pattern Stop Time");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, Channel, AUML24XX_ATTR_GATE_PATTERN_START_TIME, 0,
                                         GateStartTime),5, "Gate Pattern StartTime");
    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_SwithOffGatingPatterns
 * Purpose:  This function turns OFF all enabled gating patterns including
 *           the active gating pattern. This action will end internal
 *           processing of measurements associated to gating patterns. The
 *           gating patterns definitions however remain unchanged.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_SwitchOffGatingPatterns(
	ViSession  vi,
	ViConstString Channel
)
{
    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceChannelName (vi, Channel, &Channel) );

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "GPOFF %s",Channel));  
	}
	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}



/*****************************************************************************
 * Function: auml24xx_ConfigureLimit
 * Purpose:  This function configures the limit characteristics.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureLimit(
	ViSession  vi,
	ViConstString Channel,
	ViBoolean LimitCheckingEnabled,
	ViBoolean FailIndicatorHoldEnabled,
	ViInt32 LimitLineTestType,
	ViInt32 LimitCheckingType	  
)

{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViBoolean (vi, Channel, AUML24XX_ATTR_LIMIT_CHECKING_ENABLED, 0,
                                          LimitCheckingEnabled), 3, "Limit Checking Enabled");
    viCheckParm( Ivi_SetAttributeViBoolean (vi, Channel, AUML24XX_ATTR_LIMIT_FAIL_INDICATOR_HOLD_ENABLED, 0,
                                          FailIndicatorHoldEnabled), 4, "Fail Indicator Hold");
     viCheckParm( Ivi_SetAttributeViInt32(vi, Channel, AUML24XX_ATTR_LIMIT_LINE_TEST_TYPE, 0,
                                          LimitLineTestType), 5, "Limit Line Test Type");
     viCheckParm( Ivi_SetAttributeViInt32(vi, Channel, AUML24XX_ATTR_LIMIT_CHECKING_TYPE, 0,
                                         LimitCheckingType), 6, " Limit Checking Type");
 
	 checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
 }

/*****************************************************************************
 * Function: auml24xx_ConfigureSimpleLimit
 * Purpose:  This function configures the simple limit.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureSimpleLimit(
	ViSession  vi,
	ViConstString Channel,
	ViReal64 LowerLimitLineValue,
	ViReal64 UpperLimitLineValue
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViReal64 (vi, Channel, AUML24XX_ATTR_LIMIT_SIMPLE_LOWER_LINE, 0,
                                          LowerLimitLineValue), 3, "Lower Limit Line Value");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, Channel, AUML24XX_ATTR_LIMIT_SIMPLE_UPPER_LINE, 0,
                                          UpperLimitLineValue), 4, "Upper Limit Line Value");
    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
 }


/*****************************************************************************
 * Function: auml24xx_ConfigureComplexLimit
 * Purpose:  This function configures the complex limit settings.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureComplexLimit(
	ViSession  vi,
	ViConstString Channel,
	ViReal64 ComplexLimitsPowerOffset,
	ViInt32 ComplexLimitsRepeatCount,
	ViBoolean ComplexLimitsRepeatEnabled,
	ViReal64 ComplexLimitsPowerReplicationOffset,
	ViReal64 ComplexLimitsTimeReplicationOffset,
	ViReal64 ComplexLimitsTimeOffset
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViReal64(vi, Channel, AUML24XX_ATTR_LIMIT_COMPLEX_POWER_OFFSET, 0,
                                         ComplexLimitsPowerOffset), 3, "Complex Limits Power Offset");
    viCheckParm( Ivi_SetAttributeViInt32(vi, Channel, AUML24XX_ATTR_LIMIT_COMPLEX_RPEAT_COUNT, 0,
                                         ComplexLimitsRepeatCount), 4, "Complex Limits Repeat Count");
    viCheckParm( Ivi_SetAttributeViBoolean(vi, Channel, AUML24XX_ATTR_LIMIT_COMPLEX_RPEAT_ENABLED, 0,
                                         ComplexLimitsRepeatEnabled), 5, "Complex Limits Repeat Enabled");
    viCheckParm( Ivi_SetAttributeViReal64(vi, Channel, AUML24XX_ATTR_LIMIT_COMPLEX_POWER_REPLICATION_OFFSET, 0,
                                         ComplexLimitsPowerReplicationOffset), 6, "Complex Limits Power Replication Offset");
    viCheckParm( Ivi_SetAttributeViReal64(vi, Channel, AUML24XX_ATTR_LIMIT_COMPLEX_TIME_REPLICATION_OFFSET, 0,
                                         ComplexLimitsTimeReplicationOffset), 7, "Complex Limits Time Replication Offset");    
    viCheckParm( Ivi_SetAttributeViReal64(vi, Channel, AUML24XX_ATTR_LIMIT_COMPLEX_TIME_OFFSET, 0,
                                         ComplexLimitsTimeOffset), 8, "Complex Limits Time Offset");    

	checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureComplexLimitSpecType
 * Purpose:  This function configures the complex limit spec type. The active
 *           specification number will be configures based on this setting.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureComplexLimitSpecType(
	ViSession  vi, 
	ViConstString Channel,
	ViInt32 ComplexLimitsSpecificationType
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViInt32(vi, Channel, AUML24XX_ATTR_LIMIT_COMPLEX_SPECIFICATION_TYPE, 0,
                                         ComplexLimitsSpecificationType), 2, "Complex Limits Specification Type");
    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureComplexLimitSpecActiveNumber
 * Purpose:  This function configures the complex active limits specification
 *           number. Before calling this function, the complex limit spec
 *           type must be configured first.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureComplexLimitSpecActiveNumber(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 ComplexLimitsSpecificationActiveNumber
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViInt32(vi, Channel, AUML24XX_ATTR_LIMIT_COMPLEX_SPECIFICATION_ACTIVE_NUMBER, 0,
                                        ComplexLimitsSpecificationActiveNumber), 2, "Complex Limits Specification Active Number");
    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: QueryComplexLimitMemoryStore
 * Purpose:  This function queries the status of a selected user complex
 *           limits store.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_QueryComplexLimitMemoryStore(
	ViSession  vi,
	ViInt32 StoreNumber,
	ViBoolean * FreeStatus
)
{
    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		ViChar rdBuffer[BUFFER_SIZE];
    	checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
		viCheckErr( auml24xx_ViPrintf (io, "LMXSTQ %ld", StoreNumber));
		viCheckErr( auml24xx_ViScanf(io, "%*[^,],%s", rdBuffer));

		if (!strcmp(rdBuffer,"FREE"))
		{
		   *FreeStatus=VI_TRUE;
		}
		else if(!strcmp(rdBuffer,"USED"))

		{
		   *FreeStatus=VI_FALSE;
		}
		else
		{
			viCheckErr (IVI_ERROR_INVALID_VALUE);
		}
	}

	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}
/*****************************************************************************
 * Function: auml24xx_ConfigureComplexLimitSpecificationIDName
 * Purpose:  Specifies the name string of the limit store.   
 * Remarks: Defines the target store number and name string for the
 *          complex limits specification. Note that this command must be
 *          followed by one or more DefineComplexLimitsSegment function  and
 *          always terminated by the SaveComplexLimitSpecification function
 *          to save the specification to the target non-volatile store.
 *          Failing to use the SaveComplexLimitSpecification function will
 *          result in loss of data if the user subsequently calls a new
 *          ConfigureComplexLimitsSpecificationIDHeader function or the
 *          instrument is turned OFF.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureComplexLimitSpecificationIDName(
	ViSession  vi,
	ViInt32 StoreNumber,
	ViConstString Name 
)
{
    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	if(strlen(Name)>16|| StoreNumber<1 || StoreNumber >30)
	{ 
		viCheckErr (IVI_ERROR_INVALID_VALUE);
	}

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "LMXSID %ld,%s",StoreNumber,Name));  
	}
	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}
/*****************************************************************************
 * Function: auml24xx_ConfigureComplexLimitSegment
 * Purpose:  This function defines the complex limits segment.
 *  Notes: Only call this function following the
 *         ConfigureComplexLimitsSpecificationIDHeader function.  Failing 
 *         to do so will result in an execution error. Also call the
 *         SaveComplexLimitSpecification function at the end of the segment
 *         definition. Failing to do so will result in loss of data if a
 *         subsequent ConfigureComplexLimitsSpecificationIDHeader function
 *         is called, or the instrument is turned OFF.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureComplexLimitSegment(
	ViSession  vi,
	ViReal64 StartTime,
	ViReal64 StopTime,
	ViReal64 UpLimitStart,
	ViReal64 UpLimitStop,
	ViReal64 LowLimitSart,
	ViReal64 LowLimitStop
)
{
    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

 		viCheckErr( auml24xx_ViPrintf (io, "LMXSEG %Lf,%Lf,%Lf,%Lf,%Lf,%Lf"
			                  ,StartTime,StopTime,UpLimitStart,UpLimitStop,LowLimitSart,LowLimitStop));  
	}
	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: auml24xx_SaveComplexLimitSpecification
 * Purpose:  This function saves Specification to Complex Limits Store.
   Remarks: This function saves the complex limits specification
 *          being currently function to complete the function sequence
 *          ConfigureComplexLimitsSpecificationIDHeader,
 *          DefineComplexLimitsSegment, SaveComplexLimitSpecification
 *          required for defining a complex limits specification having one
 *          or more segments.
 *          Failing to issue this command will result in loss of data if a
 *          subsequent ConfigureComplexLimitsSpecificationIDHeader function
 *          is called, or the instrument is turned OFF. An execution error
 *          will be returned if this command is issued without first calling
 *          the ConfigureComplexLimitsSpecificationIDHeader function, or if
 *          this functions called twice or more when saving a
 *          specification.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_SaveComplexLimitSpecification(
	ViSession  vi,
	ViInt32 MaxTime
)
{
    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "LMXSAVE"));
	    error=auml24xx_WaitForOPC (vi, MaxTime);
        if ( error == VI_ERROR_TMO)
          viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);

	    viCheckErr(error); 
 	}
	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    if ( error == VI_ERROR_TMO)
      viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);
    return error;

}

/*****************************************************************************
 * Function: auml24xx_DisableAllMarkers
 * Purpose:  This function disables all markers.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_DisableAllMarkers(
	ViSession  vi,
	ViConstString Channel
)
{
    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceChannelName (vi, Channel, &Channel) );

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "MKAOFF %s",Channel));  
	}
	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: auml24xx_ConfigureMarker
 * Purpose:  This function enables or disables the marker based on the marker
 *           number configuration and configures marker position.
 *           Before calling this function, the marker  number must be configured 
 *           first.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureMarker(
	ViSession  vi,
	ViConstString Channel,
	ViBoolean MarkerEnabled,
	ViReal64 MarkerPosition
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

 	if (!Ivi_Simulating(vi))           
	{
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
	}
    viCheckParm( Ivi_SetAttributeViBoolean(vi, Channel, AUML24XX_ATTR_MARKER_ENABLED, 0,
                                        MarkerEnabled), 3, "Marker Enabled");
	if (MarkerEnabled)
	{
		viCheckParm( Ivi_SetAttributeViReal64(vi, Channel, AUML24XX_ATTR_MARKER_POSITION, 0,
                                        MarkerPosition), 4, "Marker Position");
	}
    
	checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: auml24xx_ConfigureMarkerActive
 * Purpose:  This function makes the marker active based on the current
 *           marker number.  Before calling this function, the marker number
 *           must be configured first.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureMarkerActive(
	ViSession  vi,
	ViConstString Channel
)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     markerNumber;
	checkErr( Ivi_LockSession (vi, VI_NULL));
		checkErr( Ivi_CoerceChannelName (vi, Channel, &Channel) ); 
	checkErr( Ivi_GetAttributeViInt32 (vi, Channel, AUML24XX_ATTR_MARKER_CONFIGURATION_NUMBER,0,&markerNumber));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "MKACTN %s,%ld",Channel,markerNumber));  
	}
	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: auml24xx_PositionAcitveMarkerToMax
 * Purpose:  This function positions the active marker to MAX.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_PositionActiveMarkerToMax(
	ViSession  vi,
	ViConstString Channel
)
{
    ViStatus    error = VI_SUCCESS;

    ViInt32     displayType;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceChannelName (vi, Channel, &Channel) );
    checkErr( Ivi_GetAttributeViInt32( vi,Channel, AUML24XX_ATTR_PMOD_MEASUREMENT_DISPLAY,0, &displayType));

    if ( displayType != AUML24XX_VAL_PMOD_PRF )
        viCheckErr( IVI_ERROR_INVALID_CONFIGURATION);

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "MKTMAX %s",Channel));  
	}
	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function:  auml24xx_PositionAcitveMarkerToMin
 * Purpose:  This function positions the active marker to MIN.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_PositionActiveMarkerToMin(
	ViSession  vi,
	ViConstString Channel
)
{

    ViStatus    error = VI_SUCCESS;
    ViInt32     displayType;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceChannelName (vi, Channel, &Channel) );
    checkErr( Ivi_GetAttributeViInt32( vi,Channel, AUML24XX_ATTR_PMOD_MEASUREMENT_DISPLAY,0, &displayType));

    if ( displayType != AUML24XX_VAL_PMOD_PRF )
        viCheckErr( IVI_ERROR_INVALID_CONFIGURATION);

	checkErr( Ivi_CoerceChannelName (vi, Channel, &Channel) );

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "MKTMIN %s",Channel));  
	}
	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: auml24xx_ConfigureAdvancedMarkerSearch
 * Purpose:  This function configures the advanced marker search settings.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureAdvancedMarkerSearch(
	ViSession  vi,
	ViConstString Channel,
	ViReal64 LowerTarget,
	ViReal64 UpperTarget,
	ViInt32 StartValueSource)

{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViReal64(vi, Channel, AUML24XX_ATTR_MARKER_SEARCH_LOWER_TARGET, 0,
                                         LowerTarget), 3, "Lower Target");
    viCheckParm( Ivi_SetAttributeViReal64(vi, Channel, AUML24XX_ATTR_MARKER_SEARCH_UPPER_TARGET, 0,
                                         UpperTarget), 4, "Upper Target");
    viCheckParm( Ivi_SetAttributeViInt32(vi, Channel, AUML24XX_ATTR_MARKER_SEARCH_START_VALUE_SOURCE, 0,
                                         StartValueSource), 5, "Start Value Source");
 	checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureDeltaMarker
 * Purpose:  This function configures the delta marker.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureDeltaMarker(
	ViSession  vi,
	ViConstString Channel,
	ViBoolean DeltaMarkerEnabled,
	ViBoolean DeltaMarkerLinkingEnabled,
	ViInt32 DeltaMarkerMeasurementType,
	ViReal64 DeltaMarkerPosition
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViBoolean(vi, Channel, AUML24XX_ATTR_MARKER_DELTA_ENABLED, 0,
                                         DeltaMarkerEnabled), 3, "Delta Marker Enabeld");
	viCheckParm( Ivi_SetAttributeViBoolean(vi, Channel, AUML24XX_ATTR_MARKER_DELTA_LINK_ENABLED, 0,
                                         DeltaMarkerLinkingEnabled), 4, "Delta Marker Linking Enabled");
    
	viCheckParm( Ivi_SetAttributeViInt32(vi, Channel, AUML24XX_ATTR_MARKER_DELTA_MEASUREMENT_TYPE, 0,
                                         DeltaMarkerMeasurementType), 5, "Delta Marker Measurement Type");
	if (DeltaMarkerEnabled)
	{
		viCheckParm( Ivi_SetAttributeViReal64(vi, Channel, AUML24XX_ATTR_MARKER_DELTA_POSITION, 0,
                                         DeltaMarkerPosition), 6, "Delta Marker Position");
	}

	checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}


/*****************************************************************************
 * Function: auml24xx_ConfigureMarkerNumber
 * Purpose:  This function configures the marker number for marker
 *           configuration.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureMarkerNumber(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 MarkerNumber
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViInt32(vi, Channel, AUML24XX_ATTR_MARKER_CONFIGURATION_NUMBER, 0,
                                         MarkerNumber), 3, "Active Marker Number");
  	checkErr( auml24xx_CheckStatus (vi));		   

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}



/*****************************************************************************
 * Function: auml24xx_ConfigurePostProcessingAcquisition
 * Purpose:  This function configures the post processing acquisition.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigurePostProcessingAcquisition(
	ViSession  vi,
	ViConstString Channel,
	ViBoolean PostAcqusitionEnabled,
	ViInt32 FunctionModule
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViBoolean(vi, Channel, AUML24XX_ATTR_POST_PROCESSING_ACQUISITION_ENABLED, 0,
                                         PostAcqusitionEnabled), 3, "Post Acqusition State");
    viCheckParm( Ivi_SetAttributeViInt32(vi, Channel, AUML24XX_ATTR_POST_PROCESSING_FUNCTION_MODULE, 0,
                                          FunctionModule), 4, "Post Processing Function Module");
  	checkErr( auml24xx_CheckStatus (vi));		   

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureStatisticalPostProcessing
 * Purpose:  This function configures the Statistical Post Processing.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureStatisticalPostProcessing(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 FunctionType,
	ViReal64 StatisticalPostProcessingMarkerPosition,
	ViBoolean StatisticalPostProcessingMarkerEnabled,
	ViInt32 SourceSelection
)

{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViInt32(vi, Channel, AUML24XX_ATTR_STAT_POST_PROCESSING_FUNCTION_TYPE, 0,
                                         FunctionType), 3, "Function Type");
	viCheckParm( Ivi_SetAttributeViReal64(vi, Channel, AUML24XX_ATTR_STAT_POST_PROCESSING_MARKER_POSITION, 0,
                                         StatisticalPostProcessingMarkerPosition), 4, "Statistical Post Processing Marker Position");
    viCheckParm( Ivi_SetAttributeViBoolean(vi, Channel, AUML24XX_ATTR_STAT_POST_PROCESSING_MARKER_ENABLED, 0,
                                         StatisticalPostProcessingMarkerEnabled), 5, "Statistical Post Processing Marker State");
    viCheckParm( Ivi_SetAttributeViInt32(vi, Channel, AUML24XX_ATTR_STAT_POST_PROCESSING_SOURCE_SELECTION, 0,
                                         SourceSelection), 6, "SourceSelection");
 	checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_RestartPostProcessingAcquisition
 * Purpose:  This function restarts the post processing acquisition.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_RestartPostProcessingAcquisition(
	ViSession  vi,
	ViConstString Channel
)
{
    ViStatus    error = VI_SUCCESS;
    ViBoolean   processingEnabled;
	ViInt32     functionModule;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceChannelName (vi, Channel, &Channel) );

    checkErr( Ivi_GetAttributeViBoolean (vi, Channel, AUML24XX_ATTR_POST_PROCESSING_ACQUISITION_ENABLED,
		                                 0,&processingEnabled));

    checkErr( Ivi_GetAttributeViInt32 (vi, Channel, AUML24XX_ATTR_POST_PROCESSING_FUNCTION_MODULE,
		                                 0,&functionModule));
	if((!processingEnabled) || (functionModule!=AUML24XX_VAL_POST_PROCESSING_STATS))
	{
		viCheckErr (IVI_ERROR_INVALID_CONFIGURATION); 
	}

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "PPACQRT %s",Channel));  
	}
	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: aauml24xx_ConfigurePowerAddedEfficiencyProcessing
 * Purpose:  This function configures the PAE processing.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigurePowerAddedEfficiencyProcessing(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 PAEInputConfiguration,
	ViReal64 PAEBiasCurrentValue,
	ViReal64 PAEBiasCurrentConversionFactor,
	ViInt32 PAEBiasCurrentSource,
	ViReal64 PAEBiasVoltageValue,
	ViInt32 PAESourceSelection
)

{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViInt32(vi, Channel, AUML24XX_ATTR_PAE_INPUT_CONFIGURATION, 0,
                                         PAEInputConfiguration), 3, "PAE Input Configuration");
	viCheckParm( Ivi_SetAttributeViReal64(vi, Channel, AUML24XX_ATTR_PAE_BIAS_CURRENT, 0,
                                         PAEBiasCurrentValue), 4, "PAE Bias Current Value");
    viCheckParm( Ivi_SetAttributeViReal64(vi, Channel, AUML24XX_ATTR_PAE_BIAS_CURRENT_CONVERSION_FACTOR, 0,
                                         PAEBiasCurrentConversionFactor), 5, "PAE Bias Current Conversion Factor");
    viCheckParm( Ivi_SetAttributeViInt32(vi, Channel, AUML24XX_ATTR_PAE_BIAS_CURRENT_SOURCE, 0,
                                         PAEBiasCurrentSource), 6, "PAE Bias Current Source");
    viCheckParm( Ivi_SetAttributeViReal64(vi, Channel, AUML24XX_ATTR_PAE_BIAS_VOLTAGE, 0,
                                         PAEBiasVoltageValue), 7, "PAE Bias Voltage Value");
    viCheckParm( Ivi_SetAttributeViInt32(vi, Channel, AUML24XX_ATTR_PAE_SOURCE_SELECTION, 0,
                                         PAESourceSelection), 8, "PAE Source Selection");
 	checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}


/*****************************************************************************
 * Function: auml24xx_ConfigureSensor
 * Purpose:  This function configures the sensor filter and range hold type.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureSensor(
	ViSession  vi,
	ViConstString Sensor,
	ViBoolean SensorFilterState,
	ViInt32 RangeHoldType
)

{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
	
	checkErr( Ivi_CoerceRepCapName(vi, SENSOR_REP_CAP_NAME, Sensor, &Sensor));          
	
    viCheckParm( Ivi_SetAttributeViBoolean(vi, Sensor, AUML24XX_ATTR_SENSOR_FILTER_ENABLED, 0,SensorFilterState), 3, "Sensor Filter State");
	
    viCheckParm( Ivi_SetAttributeViInt32(vi, Sensor, AUML24XX_ATTR_SENSOR_RANGE_HOLD, 0,RangeHoldType), 4, "Range Hold Type");

  	checkErr( auml24xx_CheckStatus (vi));		   

Error:
	
    Ivi_UnlockSession(vi, VI_NULL);
	
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureSensorOperatingMode
 * Purpose:  This function configures the sensor operating mode.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureSensorOperatingMode(
	ViSession  vi,
	ViConstString Sensor,
	ViInt32 UniversalSensorOperatingMode
)

{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceRepCapName(vi, SENSOR_REP_CAP_NAME, Sensor, &Sensor));                  

    viCheckParm( Ivi_SetAttributeViInt32(vi, Sensor, AUML24XX_ATTR_SENSOR_UNIVERSAL_OPERATION_MODE, 0,
                                          UniversalSensorOperatingMode), 3, "Universal Sensor Operating Mode");
  	checkErr( auml24xx_CheckStatus (vi));		   

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}


/*****************************************************************************
 * Function: auml24xx_ConfigureSensorOffset
 * Purpose:  This function configures the sensor offset
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureSensorOffset(
    ViSession       vi,
    ViConstString   sensorName,
    ViReal64        sensorOffset
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViReal64 (vi, sensorName, AUML24XX_ATTR_SENSOR_OFFSET, 0,
                                          sensorOffset), 3, "Sensor Offset");

    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureActiveOffsetTableNumber
 * Purpose:  This function configures the offset table number applied to the
             sensor.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureActiveOffsetTableNumber(
	ViSession  vi,
	ViConstString Sensor,
	ViInt32 ActiveTableNumber
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

	viCheckParm( Ivi_SetAttributeViInt32(vi, Sensor, AUML24XX_ATTR_SENSOR_OFFSET_TABLE_ACTIVE_NUMBER, 0,
                                          ActiveTableNumber), 2, "Active Table Name");
  	checkErr( auml24xx_CheckStatus (vi));		   

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}	

/*****************************************************************************
 * Function: auml24xx_ConfigureSensorOffsetType
 * Purpose:  TThis function configures the sensor offset type.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureSensorOffsetType(
	ViSession  vi,
	ViConstString Sensor,
	ViInt32 OffsetType
)

{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViInt32(vi, Sensor, AUML24XX_ATTR_SENSOR_OFFSET_TYPE, 0,
                                          OffsetType), 3, "Sensor Offset Type");
  	checkErr( auml24xx_CheckStatus (vi));		   

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_QuerySensorOffsetValue
 * Purpose:  This function queries the sensor offset value.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_QuerySensorOffsetValue(
	ViSession  vi,
	ViConstString Sensor,
	ViReal64 * OffsetValue
)
{
    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceRepCapName(vi, SENSOR_REP_CAP_NAME, Sensor, &Sensor));                  

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "SNOFVO %s",Sensor));  
		viCheckErr( auml24xx_ViScanf(io, "%Le", OffsetValue));
	}
	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}	

/*****************************************************************************
 * Function: auml24xx_ClearOffsetTable
 * Purpose:  This function clears the specified the offset table.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ClearOffsetTable(
	ViSession  vi,
	ViInt32 TableNumber,
	ViInt32 MaxTime
)
{
    ViStatus    error = VI_SUCCESS;
	ViBoolean   valid; 

	checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr(auml24xx_QueryValidOffsetTable(vi,TableNumber,&valid));

	if(!valid)
	{
	  viCheckErr(IVI_ERROR_INVALID_CONFIGURATION);
	}

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
		viCheckErr( auml24xx_ViPrintf (io, "SNOTCLR %ld",TableNumber)); 
	    error=auml24xx_WaitForOPC (vi, MaxTime);
        if ( error == VI_ERROR_TMO)
          viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);

	    viCheckErr(error); 
 	}

	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    if ( error == VI_ERROR_TMO)
        viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureOffsetTableEntry
 * Purpose:  This function adds a frequency/offset data pair to the selected
 *           offset table store number.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureOffsetTableEntry(
	ViSession  vi,
	ViInt32 TableNumber,
	ViReal64 Frequency,
	ViReal64 Offset
)
{
    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	if(TableNumber>5 || TableNumber<1)
	{ 
		viCheckErr (IVI_ERROR_INVALID_VALUE);
	}

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "SNOTADD %ld,%Le,%Le",TableNumber,Frequency,Offset));  
	}
	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: auml24xx_ConfigureOffsetTableIDName
 * Purpose:  This function configures the offset table Identity Name.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureOffsetTableIDName(
	ViSession  vi,
	ViInt32 TableNumber,
	ViConstString IDName
)
{
    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	if(strlen(IDName)>9 || TableNumber>5 || TableNumber<1)
	{ 
		viCheckErr (IVI_ERROR_INVALID_VALUE);
	}

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "SNOTID %ld,%s",TableNumber,IDName));  
	}
	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_QueryValidOffsetTable
 * Purpose:  This function queries the instrument on whether the selected
             offset table is a valid initialized table.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_QueryValidOffsetTable(
	ViSession  vi,
	ViInt32 TableNumber,
	ViBoolean * Valid
)
{
    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	if ( TableNumber<1 || TableNumber>5)
	{
	 		viCheckErr (IVI_ERROR_INVALID_VALUE);
	}

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
        ViChar	rdBuffer[BUFFER_SIZE];

		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "SNOTVLD %ld",TableNumber));  
		viCheckErr( auml24xx_ViScanf(io, "%s", rdBuffer));
		if (!strcmp(rdBuffer,"TRUE"))
		{
		   * Valid = VI_TRUE;
		}
		else if(!strcmp(rdBuffer,"FALSE"))

		{
		   * Valid=VI_FALSE;
		}
		else
		{
			viCheckErr (IVI_ERROR_INVALID_VALUE);
		}

	}

	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}



/*****************************************************************************
 * Function: auml24xx_ConfigureSensorCorrectionFrequency
 * Purpose:  This function configures the sensor correction frequency
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureSensorCorrectionFrequency(
    ViSession       vi,
    ViConstString   sensorName,
    ViReal64        sensorCorrectionFrequency
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViReal64 (vi, sensorName, AUML24XX_ATTR_SENSOR_CORRECTION_FREQUENCY, 0,
                                          sensorCorrectionFrequency), 3, "Sensor Correction Frequency");

    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}
/*****************************************************************************
 * Function: auml24xx_ConfigureCalibrationFactor
 * Purpose:  This function configures the calibration factor on the sensor.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureCalibrationFactor(
	ViSession  vi,
	ViConstString Sensor,
	ViReal64 SensorCalibrationFactorAdjust,
	ViReal64 CalibrationFactorManualValue,
	ViInt32 SensorCalFactorSource,
	ViInt32 SensorCalFactorUnits
)
{
	    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViReal64(vi, Sensor, AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_ADJUST, 0,
                                          SensorCalibrationFactorAdjust), 3, "Sensor Calibration Factor Adjust");
    viCheckParm( Ivi_SetAttributeViReal64(vi, Sensor, AUML24XX_ATTR_SENSOR_CALIBRATION_MANUAL_FACTOR, 0,
                                          CalibrationFactorManualValue), 4, "Calibration Factor Manual Value");
    viCheckParm( Ivi_SetAttributeViInt32(vi, Sensor, AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_SOURCE, 0,
                                          SensorCalFactorSource),5, "Sensor Calibration Factor Source");
    viCheckParm( Ivi_SetAttributeViInt32(vi, Sensor, AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_UNITS, 0,
                                          SensorCalFactorUnits),6, "Sensor Cal Factor Units");

  	checkErr( auml24xx_CheckStatus (vi));		   

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureVGHZCalibrationFactor
 * Purpose:  This function configures the calibration factor range.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureVGHZCalibrationFactor(
	ViSession  vi,
	ViConstString Sensor,
	ViReal64 CalibrationFactorStopFrequency,
	ViReal64 CalibrationFactorStartFrequency,
	ViReal64 CalibrationFactorStopVoltage,
	ViReal64 CalibrationFactorStartVoltage
)
{
	 ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceRepCapName(vi, SENSOR_REP_CAP_NAME, Sensor, &Sensor)); 

    viCheckParm( Ivi_SetAttributeViReal64(vi, Sensor, AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_STOP_FREQUENCY, 0,
                                          CalibrationFactorStopFrequency), 3, "Calibration Factor Stop Frequency");
    viCheckParm( Ivi_SetAttributeViReal64(vi, Sensor, AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_START_FREQUENCY, 0,
                                          CalibrationFactorStartFrequency), 4, "Calibration Factor Start Frequency");
    viCheckParm( Ivi_SetAttributeViReal64(vi, Sensor, AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_STOP_VOLTAGE, 0,
                                          CalibrationFactorStopVoltage), 5, "Calibration Factor Stop Voltage");
    viCheckParm( Ivi_SetAttributeViReal64(vi, Sensor, AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_START_VOLTAGE, 0,
                                          CalibrationFactorStartVoltage), 6, "Calibration Factor Start Voltage");
  	checkErr( auml24xx_CheckStatus (vi));		   

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_QueryCurrentCalibrationFactor
 * Purpose:  This function queries the current calibration factor value.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_QueryCurrentCalibrationFactor(
	ViSession  vi,
	ViConstString Sensor,
	ViReal64 * CalibrationFactorValue
) 
{
    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_CoerceRepCapName(vi, SENSOR_REP_CAP_NAME, Sensor, &Sensor));                  

	checkErr( Ivi_LockSession (vi, VI_NULL));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "SNCFVAL %s",Sensor));
		viCheckErr( auml24xx_ViScanf(io, "%Le", CalibrationFactorValue));
	}
	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureAveragingAutoEnabled
 * Purpose:  This function selects the cal factor table or combination of
 *           tables to be used and automatically updates the sensor. This
 *           command only executes when cal factor source is set to Frequency
 *           or V/GHz. An execution error is returned if set to Manual or if
 *           the selected table store is empty.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureCalibrationFactorActiveTableNumber(
	ViSession  vi,
	ViConstString Sensor,
	ViInt32 TableNumber
)
{
	    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceRepCapName(vi, SENSOR_REP_CAP_NAME, Sensor, &Sensor));
	checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
    viCheckParm( Ivi_SetAttributeViInt32(vi, Sensor, AUML24XX_ATTR_ACTIVE_CALIBRATION_FACTOR_NUMBER, 0,
                                          TableNumber), 3, "Table Number");
  	checkErr( auml24xx_CheckStatus (vi));		   

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_QueryValidCalibrationFactorTable
 * Purpose:  This function queries the instrument on whether the table number
 *           passed is a valid initialized table for the selected sensor.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_QueryValidCalibrationFactorTable(
	ViSession  vi,
	ViConstString Sensor,
	ViInt32 TableNumber,
	ViBoolean * Valid
)
{
    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceRepCapName(vi, SENSOR_REP_CAP_NAME, Sensor,  &Sensor));

	if ( TableNumber<1 || TableNumber>20)
	{
	 		viCheckErr (IVI_ERROR_INVALID_VALUE);
	}

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
        ViChar	rdBuffer[BUFFER_SIZE];

		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "SNCTVAL %s,%ld",Sensor,TableNumber));  
		viCheckErr( auml24xx_ViScanf(io, "%s", rdBuffer));
		if (!strcmp(rdBuffer,"TRUE"))
		{
		   * Valid = VI_TRUE;
		}
		else if(!strcmp(rdBuffer,"FALSE"))

		{
		   * Valid=VI_FALSE;
		}
		else
		{
			viCheckErr (IVI_ERROR_INVALID_VALUE);
		}

	}

	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_SaveCalibrationFactorTable
 * Purpose:  This function saves the calibration factor table currently being
 *           edited to the appropriate sensor.
 *
 *           Uses the following function sequence :
 *           1 ConfigureCalibrationFactorTableEntry
 *             (Run this function more than once if the you want to add more
 *             than one Entry.)
 *           2 SaveCalibrationFactorTable
 *           to define and save the calibration factor table having one or
 *           more factor/frequency data pairs.
 *
 *           Failing to issue this function will result in loss of data. 
*****************************************************************************/
ViStatus _VI_FUNC auml24xx_SaveCalibrationFactorTable(
	ViSession  vi, 
	ViInt32 MaxTime
) 
{
    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "SNCTSAV"));  
	    error=auml24xx_WaitForOPC (vi, MaxTime);
        if ( error == VI_ERROR_TMO)
          viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);

	    viCheckErr(error); 
 	}

	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_PresetCalibrationFactorTable
 * Purpose:  This function presets the cal factor table to the factory
 *           settings. The preset table is automatically saved to the sensor.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_PresetCalibrationFactorTable(
	ViSession  vi, 
	ViConstString Sensor, 
	ViInt32 TableNumber,
	ViInt32 MaxTime
) 
{
    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceRepCapName(vi, SENSOR_REP_CAP_NAME, Sensor,  &Sensor));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "SNCTPRE %s,%ld",Sensor,TableNumber));  
	    error=auml24xx_WaitForOPC (vi, MaxTime);
        if ( error == VI_ERROR_TMO)
          viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);

	    viCheckErr(error); 
 	}

	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_QueryCalibrationFactorActiveTableNumber
 * Purpose:  This function returns a number indicating the cal factor table,
 *           or combination of tables being used by the selected sensor.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_QueryCalibrationFactorActiveTableNumber(
	ViSession  vi,
	ViConstString Sensor,
	ViInt32 * ActiveTable
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

	viCheckParm( Ivi_GetAttributeViInt32(vi, Sensor, AUML24XX_ATTR_ACTIVE_CALIBRATION_FACTOR_NUMBER, 0,
                                          ActiveTable), 2, "Active Table");

  	checkErr( auml24xx_CheckStatus (vi));		   

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: auml24xx_ConfigureCalibrationFactorTableIDName
 * Purpose:  This function updates the seven-character identity string. This
             only affects the copy of the cal factor table stored in the
             memory of the power meter. To take effect and not be lost,
             function SaveCalibrationFactorTable must be called.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureCalibrationFactorTableIDName(
	ViSession  vi,
	ViConstString Sensor,
	ViInt32 TableNumber,
	ViConstString IDName
)
{
    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceRepCapName(vi, SENSOR_REP_CAP_NAME, Sensor,  &Sensor));

	if ( strlen(IDName)>7)
	{
	 		viCheckErr (IVI_ERROR_INVALID_VALUE);
	}

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "SNCTID %s,%ld,%s",Sensor,TableNumber,IDName));  
	}

	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: auml24xx_ClearCalibrationFactorTable
 * Purpose: This function clears the cal factor table to one entry for 50
 *          MHz at 100% and the identity name string from the table. The
 *          cleared table is automatically saved to the sensor.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ClearCalibrationFactorTable(
	ViSession  vi,
	ViConstString Sensor,
	ViInt32 TableNumber,
	ViInt32 MaxTime
)
{
    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceRepCapName(vi, SENSOR_REP_CAP_NAME, Sensor,  &Sensor));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 

		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "SNCTCLR %s,%ld",Sensor,TableNumber));
	    error=auml24xx_WaitForOPC (vi, MaxTime);
        if ( error == VI_ERROR_TMO)
          viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);

	    viCheckErr(error); 
 	}

	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureCalibrationFactorTableEntry
 * Purpose:  This function configures calibration factor table entry.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureCalibrationFactorTableEntry(
	ViSession  vi,ViConstString Sensor,
	ViInt32 TableNumber,
	ViReal64 Frequency,
	ViReal64 CalibrationFactor,
	ViInt32  UnitType
)
{
    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceRepCapName(vi, SENSOR_REP_CAP_NAME, Sensor,  &Sensor));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 

		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
		switch (UnitType)
		{
			case AUML24XX_VAL_CAL_FACTOR_UNIT_PCT:
		       viCheckErr( auml24xx_ViPrintf (io, "SNCTADD %s,%ld,%Le,%Le,PCT",Sensor,TableNumber,Frequency,CalibrationFactor));  
			   break;
			case AUML24XX_VAL_CAL_FACTOR_UNIT_DB:
		       viCheckErr( auml24xx_ViPrintf (io, "SNCTADD %s,%ld,%Le,%Le,DB",Sensor,TableNumber,Frequency,CalibrationFactor));  
			   break;				
			default:
			   viCheckErr (IVI_ERROR_INVALID_CONFIGURATION); 
			   break;
		}

	}

	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureSystem
 * Purpose:   This function configures GPIB Buffering, fast mode and number of
 * measurement points on the instruments.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureSystem(
	ViSession  vi,
	ViBoolean GPIBBufferingEnabled,
    ViBoolean FastModeEnabled,
	ViInt32 NumberOfPoints
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

	viCheckParm( Ivi_SetAttributeViBoolean(vi, VI_NULL, AUML24XX_ATTR_GPIB_RESPONSE_BUFFERING_ENABLED, 0,
                                         GPIBBufferingEnabled), 2, "GPIB Buffering State");
	viCheckParm( Ivi_SetAttributeViBoolean(vi, VI_NULL, AUML24XX_ATTR_FAST_MODE_ENABLED, 0,
                                          FastModeEnabled), 3, "Fast Mode State");
    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_DISPlAY_MEASUREMENT_POINTS, 0,
                                          NumberOfPoints),4, "Number Of Points");
   	checkErr( auml24xx_CheckStatus (vi));		   

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}


/*****************************************************************************
 * Function: auml24xx_ConfigureBNCOutput
 * Purpose:  This function configures the BNC output mode.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureBNCOutput(
	ViSession  vi,
	ViConstString BNCNumber,
	ViInt32 OutputMode,
	ViInt32 PassVoltageLevel,
	ViInt32 ChannelNumber  
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_CoerceRepCapName (vi, BNC_REP_CAP_NAME, BNCNumber,&BNCNumber),
                 2, "BNC Number");
	if(!strcmp(BNCNumber,"1"))
	{
	   viCheckParm( Ivi_SetAttributeViInt32(vi, VI_NULL, AUML24XX_ATTR_BNC1_OUTPUT_MODE, 0,
                                          OutputMode), 3, "BNC Output Mode");
	}
	else if(!strcmp(BNCNumber,"2") )
	{
	  	viCheckParm( Ivi_SetAttributeViInt32(vi, VI_NULL, AUML24XX_ATTR_BNC2_OUTPUT_MODE, 0,
                                          OutputMode), 3, "BNC2 Output Mode");
	}
 
	viCheckParm( Ivi_SetAttributeViInt32(vi, BNCNumber, AUML24XX_ATTR_BNC_PASS_VOLTAGE_LEVEL, 0,
                                         PassVoltageLevel), 4, "Pass Voltage Level");
    viCheckParm( Ivi_SetAttributeViInt32(vi, BNCNumber, AUML24XX_ATTR_BNC_OUTPUT_SOURCE_CHANNEL, 0,
                                          ChannelNumber), 5, "BNC Output Source Channel Number");

	checkErr( auml24xx_CheckStatus (vi));		   

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureBNCOutputCharacteristics
 * Purpose:  This function configures the BNC Output characteristics.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureBNCOutputCharacteristics(
	ViSession  vi,
	ViConstString BNCNumber,
	ViReal64 AnalogOutputDisplayPowerStop,
	ViReal64 AnalogOutputDisplayPowerStart,
	ViReal64 AnalogOutputStopVoltageScale,
	ViReal64 AnalogOutputStartVoltageScale,
	ViInt32 UnitType
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
	switch(UnitType)
		{
			case AUML24XX_VAL_BNC_OUTPUT_DB:

				 viCheckParm( Ivi_SetAttributeViReal64(vi, BNCNumber, AUML24XX_ATTR_BNC_DB_ANALOG_OUTPUT_STOP, 0,
                                                       AnalogOutputDisplayPowerStop),3, "Analog Output Display Power Stop");

	             viCheckParm( Ivi_SetAttributeViReal64(vi, BNCNumber, AUML24XX_ATTR_BNC_DB_ANALOG_OUTPUT_START, 0,
                                                       AnalogOutputDisplayPowerStart),4, "Analog Output Display Power Start");
				 break;
			case AUML24XX_VAL_BNC_OUTPUT_DBM:

				 viCheckParm( Ivi_SetAttributeViReal64(vi, BNCNumber, AUML24XX_ATTR_BNC_DBM_ANALOG_OUTPUT_STOP, 0,
                                                       AnalogOutputDisplayPowerStop),3, "Analog Output Display Power Stop");

	             viCheckParm( Ivi_SetAttributeViReal64(vi, BNCNumber, AUML24XX_ATTR_BNC_DBM_ANALOG_OUTPUT_START, 0,
                                                       AnalogOutputDisplayPowerStart),4, "Analog Output Display Power Start");
				 break;
			case AUML24XX_VAL_BNC_OUTPUT_DBUV:

				 viCheckParm( Ivi_SetAttributeViReal64(vi, BNCNumber, AUML24XX_ATTR_BNC_DBUV_ANALOG_OUTPUT_STOP, 0,
                                                       AnalogOutputDisplayPowerStop),3, "Analog Output Display Power Stop");

	             viCheckParm( Ivi_SetAttributeViReal64(vi, BNCNumber, AUML24XX_ATTR_BNC_DBUV_ANALOG_OUTPUT_START, 0,
                                                       AnalogOutputDisplayPowerStart),4, "Analog Output Display Power Start");
				 break;
			case AUML24XX_VAL_BNC_OUTPUT_DBMV:

				 viCheckParm( Ivi_SetAttributeViReal64(vi, BNCNumber, AUML24XX_ATTR_BNC_DBMV_ANALOG_OUTPUT_STOP, 0,
                                                       AnalogOutputDisplayPowerStop),3, "Analog Output Display Power Stop");

	             viCheckParm( Ivi_SetAttributeViReal64(vi, BNCNumber, AUML24XX_ATTR_BNC_DBMV_ANALOG_OUTPUT_START, 0,
                                                       AnalogOutputDisplayPowerStart),4, "Analog Output Display Power Start");
				 break;
			case AUML24XX_VAL_BNC_OUTPUT_WATTS:

				 viCheckParm( Ivi_SetAttributeViReal64(vi, BNCNumber, AUML24XX_ATTR_BNC_WATTS_ANALOG_OUTPUT_STOP, 0,
                                                       AnalogOutputDisplayPowerStop),3, "Analog Output Display Power Stop");

	             viCheckParm( Ivi_SetAttributeViReal64(vi, BNCNumber, AUML24XX_ATTR_BNC_WATTS_ANALOG_OUTPUT_START, 0,
                                                       AnalogOutputDisplayPowerStart),4, "Analog Output Display Power Start");
				 break;
			case AUML24XX_VAL_BNC_OUTPUT_DBW:

				 viCheckParm( Ivi_SetAttributeViReal64(vi, BNCNumber, AUML24XX_ATTR_BNC_DBW_ANALOG_OUTPUT_STOP, 0,
                                                       AnalogOutputDisplayPowerStop),3, "Analog Output Display Power Stop");

	             viCheckParm( Ivi_SetAttributeViReal64(vi, BNCNumber, AUML24XX_ATTR_BNC_DBW_ANALOG_OUTPUT_START, 0,
                                                       AnalogOutputDisplayPowerStart),4, "Analog Output Display Power Start");
				 break;
		}

    viCheckParm( Ivi_SetAttributeViReal64(vi, BNCNumber, AUML24XX_ATTR_BNC_ANALOG_OUTPUT_STOP_VOLTAGE_SCALE, 0,
                                          AnalogOutputStopVoltageScale), 5, "Calibration Factor Stop Voltage");
    viCheckParm( Ivi_SetAttributeViReal64(vi, BNCNumber, AUML24XX_ATTR_BNC_ANALOG_OUTPUT_START_VOLTAGE_SCALE, 0,
                                          AnalogOutputStartVoltageScale), 6, "Analog Output Start Voltage Scale");
  	checkErr( auml24xx_CheckStatus (vi));		   

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*===========================================================================*
 *= IviPwrMeter Zero Correction Extension Group  ============================*
 *===========================================================================*/

/*****************************************************************************
 * Function: auml24xx_IsZeroComplete
 * Purpose:  This function is not supported by the auml24xx instrument.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_IsZeroComplete(
    ViSession   vi,
    ViInt32    *zeroComplete
)
{
    ViStatus error = VI_SUCCESS;
    return error;
}

/*****************************************************************************
 * Function: auml24xx_Zero
 * Purpose:     This function performs a zero correction on the specified
 *              channel. You can use the IsZeroComplete function to determine
 *              when the zero correction is complete.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_Zero(
    ViSession       vi,
    ViConstString   channelName
)
{
    ViStatus    error = VI_SUCCESS;
 
    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_CoerceRepCapName (vi, SENSOR_REP_CAP_NAME, channelName,&channelName),
                 2, "Channel Name");

    if (!Ivi_Simulating(vi))                /* Call only when locked */
    {

        ViSession   io = Ivi_IOSession(vi);

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

        viCheckErr( auml24xx_ViPrintf (io, "SNZERO %s", channelName));

	    error=auml24xx_WaitForOPC (vi, 50000);
        if ( error == VI_ERROR_TMO)
          viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);

	    viCheckErr(error); 

	}  
	checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ZeroAllChannels
 * Purpose:     This function performs a zero correction on all enabled channels.
 *              You can use the Is Zero Complete function to determine when the
 *              zero correction is complete.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ZeroAllChannels(
    ViSession   vi
)
{
    ViStatus        error = VI_SUCCESS;
    ViConstString   channel_name = VI_NULL;
    ViInt32         channel_count;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    checkErr( Ivi_GetAttributeViInt32( vi,
                                       VI_NULL,
                                       AUML24XX_ATTR_CHANNEL_COUNT,
                                       0, &channel_count));

    if(channel_count==1)
	{
       checkErr( auml24xx_Zero (vi, "A"));
	}
	else if(channel_count==2)
	{
       checkErr( auml24xx_Zero (vi, "A"));
       checkErr( auml24xx_Zero (vi, "B"));
	}
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ZeroBNCInputConnector
 * Purpose:  This function zeros the multipurpose BNC connector used for
 *           Volts per GHz connection (Analogue Input 2). This will calibrate
 *           the units to read zero volts on this BNC.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ZeroBNCInputConnector(
	ViSession  vi, 
	ViInt32 MaxTime
)
{

    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "BNVZERO")); 

	    error=auml24xx_WaitForOPC (vi, MaxTime);
        if ( error == VI_ERROR_TMO)
          viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);

	    viCheckErr(error); 
 	}
	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_AbortRangeCalibratorTest
 * Purpose:  This function ends the Range Calibrator test if a test is
 *           currently running. If a test is not running, this command is
 *           ignored. Partial test results will not be available when
 *           aborting a test sequence prematurely. An execution error is
 *           returned if the Range Calibrator is not connected to the power
 *           meter.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_AbortRangeCalibratorTest(
	ViSession  vi, 
	ViInt32 MaxTime
)
{
    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "RCABORT"));  
	    error=auml24xx_WaitForOPC (vi, MaxTime);
        if ( error == VI_ERROR_TMO)
          viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);

	    viCheckErr(error); 

	}

	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureAveragingAutoEnabled
 * Purpose:  When in Diagnostics Mode, this function performs a Zero on
 *           the selected sensor input. A Zero is always recommended
 *           prior to requesting a reading, when first entering Diagnostics
 *           Mode, or when switching to a new measurement range.
 *           An execution error is returned if this command is sent when
 *           the instrument is not configured in Diagnostics Mode or if the
 *           Range Calibrator is not connected to the power meter.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ZeroRangeCalibratorSensorInput(
	ViSession  vi, 
	ViInt32 MaxTime
)
{
    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "RCZERO"));  
	    error=auml24xx_WaitForOPC (vi, MaxTime);
        if ( error == VI_ERROR_TMO)
          viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);

	   viCheckErr(error); 

	}

	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_InitiateRangeCalibratorFullTest
 * Purpose:  This function initiates a Range Calibrator full test on the
 *           selected sensor input(s) (see below). At the end of the test
 *           sequence the test results can be obtained using the RCD
 *           command. An execution error is returned if the Range
 *           Calibrator is not connected to the power meter. A full test
 *           involves the sequence below for each sensor input. The
 *           sensor input is zeroed before each step is carried out.
 *           Test ZERO LEVEL
 *           Test RANGE 1 HIGH
 *           Test RANGE 1 LOW
 *           Test RANGE 2 HIGH
 *           Test RANGE 2 LOW
 *           Test RANGE 3 HIGH
 *           Test RANGE 3 LOW
 *           Test RANGE 4 HIGH
 *           Test RANGE 4 LOW
 *           Test RANGE 5 HIGH
 *           Test RANGE 5 LOW
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_InitiateRangeCalibratorFullTest(
	ViSession  vi,
	ViConstString Sensor,
	ViInt32 MaxTime
)
{
    ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_CoerceRepCapName (vi, SENSOR_REP_CAP_NAME, Sensor,&Sensor),
                 2, "Sensor");

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); 
		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "RCTEST %s", Sensor));  
		error = auml24xx_WaitForOPC (vi, MaxTime);
        if ( error == VI_ERROR_TMO)
          viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);

	}

	  checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureRangeCalibratorDiagnosticTest
 * Purpose:  This function configures range calibrator diagnostic test type.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureRangeCalibratorDiagnosticTest(
	ViSession  vi,
	ViConstString Sensor,
	ViInt32 TestType
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViInt32 (vi, Sensor, AUML24XX_ATTR_RANGE_CALIBRATOR_DIAGNOSTICS_TEST, 0,
                                          TestType), 3, "Test Type");

    checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}


/*****************************************************************************
 * Function: auml24xx_OutputRangeCalibratorDiagnosticTestData
 * Purpose:  This function outputs Range Calibrator Diagnostic Test Data 
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_OutputRangeCalibratorDiagnosticTestData(
	ViSession  vi,
	ViConstString Sensor,
	ViBoolean *Valid,
	ViInt32 *ActualPoints, 
	ViReal64 Data[],
	ViInt32 Length
)
{
    ViStatus error = VI_SUCCESS;
	ViChar   dataBuf[MEAS_BUFFER_SIZE];
	ViChar   validBuf[BUFFER_SIZE];
	ViInt32  i;
	
    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_CoerceRepCapName (vi, SENSOR_REP_CAP_NAME, Sensor,&Sensor),
                 2, "Sensor");
    if (Valid == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Valid");
	
    if (ActualPoints == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for ActualPoints");

	if (Data == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 5, "Null address for MeasurementData");

	if (!Ivi_Simulating(vi))                /* Call only when locked */
    {
       	ViSession   io = Ivi_IOSession(vi); 	   

       	checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
        viCheckErr( auml24xx_ViPrintf (io, "RCD %s",Sensor));
		viCheckErr( auml24xx_ViScanf(io,"%*[^,],%s,%s",validBuf,dataBuf));
    	if (!strcmp(validBuf,"TRUE"))
	    	{
		       *Valid=VI_TRUE;
		    }
		else if(!strcmp(validBuf,"FALSE"))

		    {
		       *Valid=VI_FALSE;
		    }
		else
		   {
		       viCheckErr (IVI_ERROR_INVALID_VALUE);
		   }

		viCheckErr( auml24xx_ParseRealArray(dataBuf,VI_NULL, Length, Data, ActualPoints));

    }
    else
    {

		*Valid=VI_TRUE;
		for(i=0;i < 11; i++)
		{
			Data[i]=((ViReal64)rand() / (ViReal64)RAND_MAX); 
		}
    }
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*===========================================================================*
 *= IviPwrMeter Calibration Extension Group  ================================*
 *===========================================================================*/

/*****************************************************************************
 * Function: auml24xx_IsCalibrationComplete
 * Purpose:  This function is not supported by the auml24xx instrument.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_IsCalibrationComplete(
    ViSession   vi,
    ViInt32*    calStatus
)
{
    ViStatus    error = VI_SUCCESS;
    return error;			 
}

/*****************************************************************************
 * Function: auml24xx_Calibrate
 * Purpose:     This function performs calibration on the specified sensor.
 *              This functions returns only after the sensor has been calibrated.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_Calibrate(
    ViSession       vi,
    ViConstString   channelName
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_CoerceRepCapName (vi, SENSOR_REP_CAP_NAME, channelName,&channelName), 2, "Channel Name");

    if (!Ivi_Simulating(vi))                /* Call only when locked */
    {

        ViSession   io = Ivi_IOSession(vi);

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

        viCheckErr( auml24xx_ViPrintf (io, "SNCAL %s", channelName));

	    error=auml24xx_WaitForOPC (vi, 50000);
        if ( error == VI_ERROR_TMO)
          viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);

	    viCheckErr(error); 

	}  
	checkErr( auml24xx_CheckStatus (vi));

Error:
	
    Ivi_UnlockSession(vi, VI_NULL);
	
    return error;
}

/*===========================================================================*
 *= IviPwrMeter Reference Oscillator Extension Group  =======================*
 *===========================================================================*/

/*****************************************************************************
 * Function: auml24xx_ConfigureReferenceOscillatorEnabled
 * Purpose:     This function enables or disables the reference oscillator.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureRefOscillatorEnabled(
    ViSession       vi,
    ViBoolean       refOscillatorEnabled
)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

	viCheckParm( Ivi_SetAttributeViBoolean(vi, VI_NULL, AUML24XX_ATTR_REF_OSCILLATOR_ENABLED, 0,
                                          refOscillatorEnabled), 2, "Ref Oscillator Enabled");

	checkErr( auml24xx_CheckStatus (vi));		   

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ConfigureReferenceOscillator
 * Purpose:     This function sets the frequency and power level of the signal
 *              generated by the reference oscillator.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ConfigureRefOscillator(
    ViSession   vi,
    ViReal64    frequency,
    ViReal64    level
)
{
    ViStatus error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViReal64( vi,
                                           VI_NULL,
                                           AUML24XX_ATTR_REF_OSCILLATOR_FREQUENCY,
                                           0, frequency),
                                           2, "Reference Oscillator Frequency");

     checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*===========================================================================*
 *= IviPwrMeter Measurement Extension Group  ================================*
 *===========================================================================*/
/*****************************************************************************
 * Function: auml24xx_Read
 * Purpose:  This function is not supported on the instrument.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_Read (
    ViSession vi,
	ViInt32 maxTime,
	ViReal64 *result
)
{
    ViStatus    error   = VI_SUCCESS;
	ViReal64    headerData;
	int			iStatus=0;
	char		szBuffer[128]={0};
	
	double		lfCurrentTime=0.0,
				lfStartTime=0.0;
	
	ViSession   io = Ivi_IOSession(vi);
	
    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (result == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER,3, "Null address for Actual Points");

	GetCurrentDateTime(&lfStartTime);
	
	do
	{
		viCheckErr( auml24xx_ViPrintf (io, "*OPC?"));  
		viCheckErr( auml24xx_ViScanf(io, "%s", szBuffer ));
	
		iStatus = atoi(szBuffer);
	
		if ( iStatus == 1 )
			break;
		
		GetCurrentDateTime(&lfCurrentTime);
		
	} while((lfCurrentTime-lfStartTime)<5.0);
	
	if ( iStatus != 1 )
		viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED );  

	viCheckErr( auml24xx_ViPrintf(io, "*TRG"));
	viCheckErr( auml24xx_ViScanf(io, "%s", szBuffer )); 
	sscanf(szBuffer, "%lf", result);
		
	checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}


/*****************************************************************************
 * Function: auml24xx_ReadPMODTrace
 * Purpose:  This function reads the PMOD Measurement Trace
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ReadPMODTrace(
	ViSession       vi,
	ViConstString   ChannelName,
	ViInt32 *ActualPoints, 
	ViReal64 TraceArray[],
	ViInt32  Length,
	ViReal64 *InitialX,
	ViReal64 *xIncrement,
	ViInt32         MaxTime
)
{
    ViStatus    error   = VI_SUCCESS;
	ViReal64    headerData;
    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (ActualPoints == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Actual Points");

	if (TraceArray == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 5, "Null address for Trace Array");

	if (InitialX == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 7, "Null address for Initial X");

	if (xIncrement == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 8, "Null address for X Increment");

	checkErr( auml24xx_Initiate (vi));

    error = auml24xx_FetchHeader (vi,ChannelName, &headerData,MaxTime);
    if ( error == VI_ERROR_TMO)
        viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);
	
	checkErr(auml24xx_FetchPMODTrace(vi,ChannelName,ActualPoints,TraceArray,Length,InitialX,xIncrement));
	
	checkErr( auml24xx_CheckStatus (vi));

	

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: auml24xx_ReadPMODReadOut
 * Purpose:  This function reads the PMOD readout measurement data
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ReadPMODReadOut(
	ViSession       vi,
	ViConstString   ChannelName,
	ViInt32 *ActualPoints, 
	ViReal64 MeasurementData[],
	ViInt32         MaxTime
)
{
    ViStatus    error   = VI_SUCCESS;
	ViReal64    headerData;
    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (ActualPoints == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Actual Points");

	if (MeasurementData == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Measurement Data");

	checkErr( auml24xx_Initiate (vi));

    error = auml24xx_FetchHeader (vi,ChannelName, &headerData,MaxTime);

    if ( error == VI_ERROR_TMO)
        viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);
	
	checkErr(auml24xx_FetchPMODReadOut(vi,ChannelName,ActualPoints,MeasurementData));
	
	checkErr( auml24xx_CheckStatus (vi));

	

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ReadPMODMaxTrace
 * Purpose:  This function reads the PMOD max measurement trace
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ReadPMODMaxTrace(
	ViSession       vi,
	ViConstString   ChannelName,
	ViInt32 *ActualPoints, 
	ViReal64 TraceArray[],
	ViInt32 Length,
	ViReal64 *InitialX,
	ViReal64 *xIncrement,
	ViInt32         MaxTime
)
{
    ViStatus    error   = VI_SUCCESS;
	ViReal64    headerData;
    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (ActualPoints == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Actual Points");

	if (TraceArray == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 5, "Null address for Trace Array");

	if (InitialX == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 7, "Null address for Initial X");

	if (xIncrement == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 8, "Null address for X Increment");

	checkErr( auml24xx_Initiate (vi));

    error = auml24xx_FetchHeader (vi,ChannelName, &headerData,MaxTime);

    if ( error == VI_ERROR_TMO)
        viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);
	
	checkErr(auml24xx_FetchPMODMaxTrace(vi,ChannelName,ActualPoints,TraceArray,Length,InitialX,xIncrement));
	
	checkErr( auml24xx_CheckStatus (vi));

	

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: auml24xx_ReadPMODMinTrace
 * Purpose:  This function reads the PMOD min measurement trace
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ReadPMODMinTrace(
	ViSession       vi,
	ViConstString   ChannelName,
	ViInt32 *ActualPoints, 
	ViReal64 TraceArray[],
	ViInt32 Length,
	ViReal64 *InitialX,
	ViReal64 *xIncrement,
	ViInt32         MaxTime
)
{
    ViStatus    error   = VI_SUCCESS;
	ViReal64    headerData;
    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (ActualPoints == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Actual Points");

	if (TraceArray == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 5, "Null address for Trace Array");

	if (InitialX == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 7, "Null address for Initial X");

	if (xIncrement == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 8, "Null address for X Increment");

	checkErr( auml24xx_Initiate (vi));

    error = auml24xx_FetchHeader (vi,ChannelName, &headerData,MaxTime);

    if ( error == VI_ERROR_TMO)
        viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);
	
	checkErr(auml24xx_FetchPMODMinTrace(vi,ChannelName,ActualPoints,TraceArray,Length,InitialX,xIncrement));
	
	checkErr( auml24xx_CheckStatus (vi));

	

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: auml24xx_ReadCWData
 * Purpose:  This function reads the CW measurement Data.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ReadCWData(
	ViSession       vi,
	ViConstString   ChannelName,
	ViReal64 *CWdata,
	ViInt32         MaxTime    	
)
{
    ViStatus    error   = VI_SUCCESS;
	ViReal64    headerData;
    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (CWdata == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for CW Data");


	checkErr( auml24xx_Initiate (vi));

    error = auml24xx_FetchHeader (vi,ChannelName, &headerData,MaxTime);

    if ( error == VI_ERROR_TMO)
        viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);
	
	checkErr(auml24xx_FetchCWData(vi,ChannelName,CWdata));
	
	checkErr( auml24xx_CheckStatus (vi));

	

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}




/*****************************************************************************
 * Function: auml24xx_ReadMarker
 * Purpose:  This function reads the specified marker data.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ReadMarker(
	ViSession       vi,
	ViConstString   ChannelName,
	ViInt32  MarkerNumber,
	ViReal64 *Power,
	ViReal64 *Position,
	ViInt32         MaxTime
)
{
    ViStatus    error   = VI_SUCCESS;
	ViReal64    headerData;
    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (Power == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Power");

	if (Position == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 5, "Null address for Position");
 
	checkErr( auml24xx_Initiate (vi));

    error = auml24xx_FetchHeader (vi,ChannelName, &headerData,MaxTime);

    if ( error == VI_ERROR_TMO)
        viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);
	
	checkErr(auml24xx_FetchMarker(vi,ChannelName,MarkerNumber,Power,Position));
	
	checkErr( auml24xx_CheckStatus (vi));
																			

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: auml24xx_ReadDeltaMarker
 * Purpose:  This function reads the delta marker data.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ReadDeltaMarker(
	ViSession       vi,
	ViConstString   ChannelName,
	ViReal64 *MeasurementData,
	ViReal64 *Position,
	ViInt32         MaxTime
)
{
    ViStatus    error   = VI_SUCCESS;
	ViReal64    headerData;
    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (MeasurementData == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Measurement Data");

	if (Position == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 5, "Null address for Position");
 
	checkErr( auml24xx_Initiate (vi));

    error = auml24xx_FetchHeader (vi,ChannelName, &headerData,MaxTime);

    if ( error == VI_ERROR_TMO)
        viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);
	
	checkErr(auml24xx_FetchDeltaMarker(vi,ChannelName,MeasurementData,Position));
	
	checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: auml24xx_ReadPulseMeasurement
 * Purpose:  This function reads the pulse measurement data.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ReadPulseMeasurement(
	ViSession  vi,
	ViConstString Channel,
	ViReal64 * PulseFallTime,
	ViReal64 * PulseRiseTime,
	ViReal64 * PulseOffTime,
	ViReal64 * PulseWidth,
	ViReal64 * PulseRepetitionInterval,
	ViInt32         MaxTime
)
{
    ViStatus    error   = VI_SUCCESS;
	ViReal64    headerData;
    checkErr( Ivi_LockSession (vi, VI_NULL));

	if (PulseFallTime == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for PulseFallTime");

	if (PulseRiseTime == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for PulseRiseTime");

	if (PulseOffTime == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 5, "Null address for PulseOffTime");

	if (PulseWidth == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 6, "Null address for PulseWidth");

	if (PulseRepetitionInterval == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 7, "Null address for PulseRepetitionInterval");

	checkErr( auml24xx_Initiate (vi));

    error = auml24xx_FetchHeader (vi,Channel, &headerData,MaxTime);

    if ( error == VI_ERROR_TMO)
        viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);
	
	checkErr(auml24xx_FetchPulseMeasurement(vi,Channel, PulseFallTime,PulseRiseTime,PulseOffTime,
		                                                    PulseWidth,PulseRepetitionInterval));
	
	checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: auml24xx_ReadStatisticalCursor
 * Purpose:  This function reads the statisical cursor data.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ReadStatisticalCursor(
	ViSession       vi,
	ViConstString   ChannelName,
	ViReal64 *Percentage,
	ViReal64 *Power,
	ViInt32         MaxTime
)
{
    ViStatus    error   = VI_SUCCESS;
	ViReal64    headerData;
    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (Percentage == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Percentage");

	if (Power == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Power");
 
	checkErr( auml24xx_Initiate (vi));

    error = auml24xx_FetchHeader (vi,ChannelName, &headerData,MaxTime);

    if ( error == VI_ERROR_TMO)
        viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);
	
	checkErr(auml24xx_FetchStatisticalCursor(vi,ChannelName,Percentage,Power));
	
	checkErr( auml24xx_CheckStatus (vi));
																			

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: auml24xx_ReadPAE
 * Purpose:  This function reads the PAE data.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ReadPAE(
	ViSession       vi,
	ViConstString   ChannelName,
	ViReal64 *PAEData,
	ViInt32         MaxTime
)
{
    ViStatus    error   = VI_SUCCESS;
	ViReal64    headerData;
    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (PAEData == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for PAE Data");
 
	checkErr( auml24xx_Initiate (vi));

    error = auml24xx_FetchHeader (vi,ChannelName, &headerData,MaxTime);

    if ( error == VI_ERROR_TMO)
        viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);
	
	checkErr(auml24xx_FetchPAE(vi,ChannelName,PAEData));
	
	checkErr( auml24xx_CheckStatus (vi));
																			

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: auml24xx_ReadStatPostProcessingTrace
 * Purpose:  This function reads the statisical post processing trace.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ReadStatPostProcessingTrace(
	ViSession       vi,
	ViConstString   ChannelName,
	ViInt32 *ActualPoints, 
	ViReal64 TraceArray[],
	ViInt32 Length,
	ViReal64 *InitialX,
	ViReal64 *xIncrement,
	ViInt32         MaxTime
)
{
    ViStatus    error   = VI_SUCCESS;
	ViReal64    headerData;
    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (ActualPoints == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Actual Points");

	if (TraceArray == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Trace Array");

	checkErr( auml24xx_Initiate (vi));

    error = auml24xx_FetchHeader (vi,ChannelName, &headerData,MaxTime);

    if ( error == VI_ERROR_TMO)
        viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);
	
	checkErr(auml24xx_FetchStatisticalPostProcessingTrace(vi,ChannelName,ActualPoints,TraceArray,Length,InitialX,xIncrement));
	
	checkErr( auml24xx_CheckStatus (vi));

	

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_ReadGating
 * Purpose:  This function reads the gating data.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_ReadGating(
	ViSession  vi,
    ViConstString ChannelName,
	ViInt32 GateNumber,
	ViInt32 *ActualPoints, 
	ViReal64 MeasurementData[],
	ViInt32         MaxTime
)
 {
    ViStatus    error   = VI_SUCCESS;
	ViReal64    headerData;
    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (ActualPoints == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Actual Points");

	if (MeasurementData == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 5, "Null address for Measurement Data");

	checkErr( auml24xx_Initiate (vi));

    error = auml24xx_FetchHeader (vi,ChannelName, &headerData,MaxTime);

    if ( error == VI_ERROR_TMO)
        viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED);
	
	checkErr(auml24xx_FetchGating(vi,ChannelName,GateNumber,ActualPoints,MeasurementData));
	
	checkErr( auml24xx_CheckStatus (vi));

	

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*===========================================================================*
 *= IviPwrMeter Low-Level Measurement Functions =============================*
 *===========================================================================*/

/*****************************************************************************
 * Function: auml24xx_Fetch
 * Purpose:  This function is not supported on the instrument.
  *****************************************************************************/
ViStatus _VI_FUNC auml24xx_Fetch(
    ViSession   vi,
    ViReal64   *result
)
{
    ViStatus    error   = VI_SUCCESS;
	ViReal64    headerData;
	int			iStatus=0,
				iChannel=0;
	char		szBuffer[128]={0};
	
	double		lfCurrentTime=0.0,
				lfStartTime=0.0;
	
	ViSession   io = Ivi_IOSession(vi);
	
    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (result == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER,3, "Null address for Actual Points");

	GetCurrentDateTime(&lfStartTime);
	
	do
	{
		viCheckErr( auml24xx_ViPrintf (io, "*OPC?"));  
		viCheckErr( auml24xx_ViScanf(io, "%s", szBuffer ));
	
		iStatus = atoi(szBuffer);
	
		if ( iStatus == 1 )
			break;
		
		GetCurrentDateTime(&lfCurrentTime);
		
	} while((lfCurrentTime-lfStartTime)<5.0);
	
	if ( iStatus != 1 )
		viCheckErr( AUML24XX_ERROR_MAX_TIME_EXCEEDED );  
	
	GetCurrentDateTime(&lfStartTime);
	
	do
	{
		viCheckErr( auml24xx_ViPrintf (io, "CHACTIV?"));  
		viCheckErr( auml24xx_ViScanf(io, "%s %d", szBuffer , &iChannel ));
	
		if ( strcmp( szBuffer , "CHACTIV" ))
			sscanf(szBuffer , "%d" , &iChannel );
	
		if ( iChannel )
			break;
		
		GetCurrentDateTime(&lfCurrentTime);
		
	} while((lfCurrentTime-lfStartTime)<5.0);
	
	if ( iChannel > 0 )
	{
		viCheckErr( auml24xx_ViPrintf(io, "CWO %d" , iChannel ));
		viCheckErr( auml24xx_ViScanf(io, "%s", szBuffer )); 
	}
	
	sscanf(szBuffer, "%lf", result);
		
	checkErr( auml24xx_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_Initiate
 * Purpose:     This function initiates a measurement. The instrument will
 * enter wait for trigger mode.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_Initiate(
    ViSession   vi
)
{
    ViStatus error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (!Ivi_Simulating(vi))                /* Call only when locked */
    {

        ViSession       io = Ivi_IOSession(vi);
        viCheckErr( auml24xx_ViPrintf (io, "*TRG"));

    }

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_IsMeasurementComplete
 * Purpose:  This function is not supported on the instrument.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_IsMeasurementComplete(
    ViSession   vi,
    ViInt32    *measComplete
)
{
    ViStatus    error = VI_SUCCESS;


    return error;
}


/*****************************************************************************
 * Function: auml24xx_Abort
 * Purpose:  This function is not supported on the instrument.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_Abort(
    ViSession   vi
)
{
    ViStatus    error = VI_SUCCESS;


    return error;
}

/*===========================================================================*
 *= IviPwrMeter Software Trigger Extension Group ============================*
 *===========================================================================*/

/*****************************************************************************
 * Function: auml24xx_SendSoftwareTrigger
 * Purpose:  This function is not supported on the instrument. 
 *****************************************************************************/

ViStatus _VI_FUNC auml24xx_SendSoftwareTrigger(
    ViSession   vi
)
{
    ViStatus    error = VI_SUCCESS;


    return error;
}


/*****************************************************************************
 * Function: auml24xx_QueryResultRangeType
 * Purpose:  This function is not supported on the instrument
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_QueryResultRangeType(
    ViSession   vi,
    ViReal64    measurementValue,
    ViInt32    *rangeType
)
{
    ViStatus error = VI_SUCCESS;
    
Error:
    return error;
}

/*****************************************************************************
 * Function: auml24xx_FetchHeader
 * Purpose:  This function returns the header data.
 *           After trigger occurs, the instrument will place one or two
 *           measurement readings in the GPIB output buffer as outlined
 *           below. These readings must be fetched from the instrument first,
 *           before attempting to request additional measurement data.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_FetchHeader(
	ViSession       vi,
	ViConstString   ChannelName,
	ViReal64        *HeaderData,
	ViInt32         MaxTime
)
{
    ViStatus error = VI_SUCCESS;
	ViInt32     oldTime; 
    checkErr( Ivi_LockSession (vi, VI_NULL));
	
	checkErr( Ivi_CoerceChannelName (vi, ChannelName, &ChannelName) );
	
	if (HeaderData == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for HeaderData");

	if (!Ivi_Simulating(vi))                /* Call only when locked */
    {


		ViSession   io = Ivi_IOSession(vi); 
	    viCheckErr( viGetAttribute (io, VI_ATTR_TMO_VALUE, &oldTime ));   
     	viCheckErr( viSetAttribute (io, VI_ATTR_TMO_VALUE, MaxTime ));  

       	checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
  		error =  auml24xx_ViScanf(io, "%le", HeaderData);
    	viCheckErr( viSetAttribute (io, VI_ATTR_TMO_VALUE, oldTime ));
        viCheckErr( error);
    }
    else
    {
		*HeaderData=((ViReal64)rand() / (ViReal64)RAND_MAX); 

    }

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*===========================================================================*
 *= IviPwrMeter Low-Level Measurement Functions =============================*
 *===========================================================================*/

/*****************************************************************************
 * Function: auml24xx_FetchPMODTrace
 * Purpose:  This function returns Pulsed / Modulated Profile Data.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_FetchPMODTrace(
    ViSession       vi,
	ViConstString   ChannelName,
	ViInt32 *ActualPoints, 
	ViReal64 TraceData[],
	ViInt32 Length,
	ViReal64 *InitialX,
	ViReal64 *xIncrement
)
{
    ViStatus error = VI_SUCCESS;
	ViChar   buf[MEAS_BUFFER_SIZE];
	ViReal64 captureTime;
	ViInt32  points;
	ViInt32  i;
	
    checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceChannelName (vi, ChannelName, &ChannelName) ); 
    if (ActualPoints == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for ActualPoints");

	if (TraceData == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for TraceData");

	if (InitialX == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 6, "Null address for Initial X");

	if (xIncrement == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 7, "Null address for X Increment");


	if (!Ivi_Simulating(vi))                /* Call only when locked */
    {
       	ViSession   io = Ivi_IOSession(vi); 	   

       	checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
		
        viCheckErr( auml24xx_ViPrintf (io, "TRCAPT? %s,PMOD",ChannelName));
		viCheckErr( auml24xx_ViScanf(io,"%*[^,],%*[^,],%Le",&captureTime));
		
		viCheckErr( auml24xx_ViPrintf (io, "PMPO %s",ChannelName));
		viCheckErr( auml24xx_ViScanf(io,"%*[^,],%*[^,],%s",buf));  
		viCheckErr( auml24xx_ParseRealArray(buf,VI_NULL, Length, TraceData, ActualPoints));
		
		*InitialX=0;
		*xIncrement= captureTime/(*ActualPoints-1);

    }
    else
    {
        checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, 
                                             AUML24XX_ATTR_DISPlAY_MEASUREMENT_POINTS, 
                                             0, &points));
        switch(points)
		{
			case AUML24XX_VAL_P200:
				*ActualPoints=200;
				break;
			case AUML24XX_VAL_P400:
				*ActualPoints=400;
				break;
			default:
				*ActualPoints=200;
			    break;
		}
		for(i=0;i < *ActualPoints; i++)
		{
			TraceData[i]=((ViReal64)rand() / (ViReal64)RAND_MAX); 
		}
		*InitialX=0;
		*xIncrement= 0.0046/(*ActualPoints-1);
    }

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_FetchPMODReadOut
 * Purpose:  This function returns Readout Measurements over Capture Time.

 *           The sequence of measurement data will be different depending
 *           upon the selected measurement type number.

 *           Measurement Type:
 *           1 Average Power
 *           2 Average Power, Peak Power
 *           3 Average Power, Peak Power, Crest Factor
 *           4 Average Power, Min Power & Time , Max Power & Time
 *           5 Average Power, Held Min Power & Time, Held Max Power &
 *           Time
 *           Data Format  
 *           1 avg_pow
 *           2 Avg_pow,Pk_pow
 *           3 Avg_pow,Pk_pow,Cres_Fact
 *           4 Avg_pow,min_pow,min_time,max_pow,max_time
 *           5 Avg_pow,hmin_pow,hmin_time,hmax_pow,hmax_time
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_FetchPMODReadOut(
	ViSession       vi,
	ViConstString   ChannelName,
	ViInt32 *ActualPoints, 
	ViReal64 MeasurementData[]
)
{
    ViStatus error = VI_SUCCESS;
	ViChar   buf[MEAS_BUFFER_SIZE];
	ViInt32  measType;
	ViInt32  i;
	ViString PModeReadOutIgnore="PA,PK,CF,PN,TN,PX,TX,PHN,PHX,THN,THX";

    checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceChannelName (vi, ChannelName, &ChannelName) ); 	

	if (ActualPoints == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for ActualPoints");

	if (MeasurementData == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for MeasurementData");

	if (!Ivi_Simulating(vi))                /* Call only when locked */
    {
       	ViSession   io = Ivi_IOSession(vi); /* call only when locked  */

       	checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

        viCheckErr( auml24xx_ViPrintf (io, "PMRDO %s",ChannelName));
		viCheckErr( auml24xx_ViScanf(io,"%*[^,],%*[^,],%s",buf));  
		viCheckErr( auml24xx_ParseRealArray(buf,PModeReadOutIgnore,5, MeasurementData, ActualPoints));
    }
    else
    {
        checkErr( Ivi_GetAttributeViInt32 (vi, ChannelName, 
                                             AUML24XX_ATTR_PMOD_MEASUREMENT_TYPE, 
                                             0, &measType));
        switch(measType)
		{
			case AUML24XX_VAL_PMOD_MEAS_TYPE_1:
				*ActualPoints=1;
				break;
			case AUML24XX_VAL_PMOD_MEAS_TYPE_2:
				*ActualPoints=2;
				break;
			case AUML24XX_VAL_PMOD_MEAS_TYPE_3:
				*ActualPoints=3;
				break;
			case AUML24XX_VAL_PMOD_MEAS_TYPE_4:
				*ActualPoints=5;
				break;
			case AUML24XX_VAL_PMOD_MEAS_TYPE_5:
				*ActualPoints=5;
				break;
			default:
				*ActualPoints=1;				
				break;
		}
		for(i=0;i < *ActualPoints; i++)
		{
			MeasurementData[i]=((ViReal64)rand() / (ViReal64)RAND_MAX); 
		}
    }


Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_FetchPMODMaxTrace
 * Purpose:  This function returns Pulsed / Modulated Graph Max Data.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_FetchPMODMaxTrace(
	ViSession       vi,
	ViConstString   ChannelName,
	ViInt32 *ActualPoints, 
	ViReal64 TraceData[],
	ViInt32 Length,
	ViReal64 *InitialX,
	ViReal64 *xIncrement
)
{
    ViStatus error = VI_SUCCESS;
	ViInt32  points;
	ViInt32  i;
	ViChar   buf[MEAS_BUFFER_SIZE];
	ViReal64 captureTime;

    checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceChannelName (vi, ChannelName, &ChannelName) ); 
    if (ActualPoints == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for ActualPoints");

	if (TraceData == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for TraceData");

	if (InitialX == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 6, "Null address for Initial X");

	if (xIncrement == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 7, "Null address for X Increment");


	if (!Ivi_Simulating(vi))                /* Call only when locked */
    {
       	ViSession   io = Ivi_IOSession(vi); 	   

       	checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
        
		viCheckErr( auml24xx_ViPrintf (io, "TRCAPT? %s,PMOD",ChannelName));
		viCheckErr( auml24xx_ViScanf(io,"%*[^,],%*[^,],%Le",&captureTime));
		
        viCheckErr( auml24xx_ViPrintf (io, "PMXPO %s",ChannelName));
		viCheckErr( auml24xx_ViScanf(io,"%*[^,],%*[^,],%s",buf));  
		viCheckErr( auml24xx_ParseRealArray(buf,VI_NULL, Length, TraceData, ActualPoints));

		*InitialX=0;
		*xIncrement= captureTime/(*ActualPoints-1);
    }
    else
    {
        checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, 
                                             AUML24XX_ATTR_DISPlAY_MEASUREMENT_POINTS, 
                                             0, &points));
        switch(points)
		{
			case AUML24XX_VAL_P200:
				*ActualPoints=200;
				break;
			case AUML24XX_VAL_P400:
				*ActualPoints=400;
				break;
			default:
				*ActualPoints=200;
			    break;
		}
		for(i=0;i < *ActualPoints; i++)
		{
			TraceData[i]=((ViReal64)rand() / (ViReal64)RAND_MAX)*2; 
		}
		*InitialX=0;
		*xIncrement= 0.0046/(*ActualPoints-1);
    }

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_FetchPMODMinTrace
 * Purpose:  This function returns Pulsed / Modulated Graph Min Data.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_FetchPMODMinTrace(
    ViSession       vi,
	ViConstString   ChannelName,
	ViInt32 *ActualPoints, 
	ViReal64 TraceData[],
	ViInt32 Length,
	ViReal64 *InitialX,
	ViReal64 *xIncrement
)
{
    ViStatus error = VI_SUCCESS;
	ViChar   buf[MEAS_BUFFER_SIZE];
	ViInt32  points;
	ViInt32  i;
	ViReal64 captureTime;

    checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceChannelName (vi, ChannelName, &ChannelName) ); 
    if (ActualPoints == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for ActualPoints");

	if (TraceData == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for TraceData");

	if (InitialX == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 6, "Null address for Initial X");

	if (xIncrement == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 7, "Null address for X Increment");

	if (!Ivi_Simulating(vi))                /* Call only when locked */
    {
       	ViSession   io = Ivi_IOSession(vi); 	   

       	checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
		
		viCheckErr( auml24xx_ViPrintf (io, "TRCAPT? %s,PMOD",ChannelName));
		viCheckErr( auml24xx_ViScanf(io,"%*[^,],%*[^,],%Le",&captureTime));
		
        viCheckErr( auml24xx_ViPrintf (io, "PMNPO %s",ChannelName));
		viCheckErr( auml24xx_ViScanf(io,"%*[^,],%*[^,],%s",buf));  
		viCheckErr( auml24xx_ParseRealArray(buf,VI_NULL, Length, TraceData, ActualPoints));
		
		*InitialX=0;
		*xIncrement= captureTime/(*ActualPoints-1);
    }
    else
    {
        checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, 
                                             AUML24XX_ATTR_DISPlAY_MEASUREMENT_POINTS, 
                                             0, &points));
        switch(points)
		{
			case AUML24XX_VAL_P200:
				*ActualPoints=200;
				break;
			case AUML24XX_VAL_P400:
				*ActualPoints=400;
				break;
			default:
				*ActualPoints=200;
				break;
		}
		for(i=0;i < *ActualPoints; i++)
		{
			TraceData[i]=((ViReal64)rand() / (ViReal64)RAND_MAX)*0.5; 
		}

		*InitialX=0;
		*xIncrement= 0.0046/(*ActualPoints-1);

	}

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_FetchCWData
 * Purpose:  This function returns a measurement reading for the selected 
 * channel in CW mode.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_FetchCWData(
	ViSession       vi,
	ViConstString   ChannelName,
	ViReal64 *CWData
)
{
    ViStatus error = VI_SUCCESS;
    checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceChannelName (vi, ChannelName, &ChannelName) ); 
	if (CWData == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for CWData");

	if (!Ivi_Simulating(vi))                /* Call only when locked */
    {
       	ViSession   io = Ivi_IOSession(vi); 

       	checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
  		viCheckErr( auml24xx_ViPrintf(io, "CWO %s", ChannelName));
  		viCheckErr( auml24xx_ViScanf(io, "%Le", CWData));

    }
    else
    {
	   *CWData=((ViReal64)rand() / (ViReal64)RAND_MAX);
    }

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}


/*****************************************************************************
 * Function: auml24xx_FetchMarker
 * Purpose:  This function returns specified Marker Reading.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_FetchMarker(
    ViSession       vi,
	ViConstString   ChannelName,
	ViInt32  MarkerNumber,
	ViReal64 *Power,
	ViReal64 *Position
)
{
    ViStatus error = VI_SUCCESS;
	ViChar   buf[BUFFER_SIZE];
	ViReal64 MeasurementData[3];
	ViInt32  ActualPoints;

    checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceChannelName (vi, ChannelName, &ChannelName) ); 
	if (MarkerNumber>4 || MarkerNumber<1)
 		viCheckErr (IVI_ERROR_INVALID_VALUE);

	if (Power == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Power");
	if (Position == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 5, "Null address for Position");

	if (!Ivi_Simulating(vi))                /* Call only when locked */
    {
       	ViSession   io = Ivi_IOSession(vi); 

       	checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
  		viCheckErr( auml24xx_ViPrintf(io, "MKNO %s,%ld", ChannelName,MarkerNumber));
  		viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%s",buf));
		viCheckErr( auml24xx_ParseRealArray(buf,VI_NULL, 3, MeasurementData, &ActualPoints));

		*Power   =  MeasurementData[0];
		*Position=  MeasurementData[2];

    }
    else
    {
		*Power = ((ViReal64)rand() / (ViReal64)RAND_MAX);
		*Position = ((ViReal64)rand() / (ViReal64)RAND_MAX);
    }

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: auml24xx_FetchDeltaMarker
 * Purpose:  This function returns delta Marker Reading.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_FetchDeltaMarker(
	ViSession       vi,
	ViConstString   ChannelName,
	ViReal64 *Power,
	ViReal64 *Position
)
{
    ViStatus error = VI_SUCCESS;
	ViChar   buf[BUFFER_SIZE];
	ViReal64 MeasurementData[3];
	ViInt32  ActualPoints;

    checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceChannelName (vi, ChannelName, &ChannelName) ); 
	if (Power == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Power");
	if (Position == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Position");

	if (!Ivi_Simulating(vi))                /* Call only when locked */
    {
       	ViSession   io = Ivi_IOSession(vi); 

       	checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
  		viCheckErr( auml24xx_ViPrintf(io, "MKDO %s", ChannelName));
  		viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%s",buf));
		viCheckErr( auml24xx_ParseRealArray(buf,VI_NULL, 3, MeasurementData, &ActualPoints));

		*Power   =  MeasurementData[0];
		*Position=  MeasurementData[2];

    }
    else
    {
		*Power = ((ViReal64)rand() / (ViReal64)RAND_MAX);
		*Position = ((ViReal64)rand() / (ViReal64)RAND_MAX);
    }

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: auml24xx_FetchPulseMeasurement
 * Purpose:  This function returns pulse measurement data.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_FetchPulseMeasurement(
	ViSession  vi,ViConstString Channel,
	ViReal64 * PulseFallTime,
	ViReal64 * PulseRiseTime,
	ViReal64 * PulseOffTime,
	ViReal64 * PulseWidth,
	ViReal64 * PulseRepetitionInterval
)
{
    ViStatus error = VI_SUCCESS;
    checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceChannelName (vi, Channel, &Channel) ); 
	if (PulseFallTime == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for PulseFallTime");

	if (PulseRiseTime == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for PulseRiseTime");

	if (PulseOffTime == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 5, "Null address for PulseOffTime");

	if (PulseWidth == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 6, "Null address for PulseWidth");

	if (PulseRepetitionInterval == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 7, "Null address for PulseRepetitionInterval");

	if (!Ivi_Simulating(vi))                /* Call only when locked */
    {
       	ViSession   io = Ivi_IOSession(vi); 

       	checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

  		viCheckErr( auml24xx_ViQueryf(io, "MKPFTO %s","%*[^,],%Le",Channel,PulseFallTime));
  		viCheckErr( auml24xx_ViQueryf(io, "MKPRTO %s","%*[^,],%Le",Channel,PulseRiseTime));
 		viCheckErr( auml24xx_ViQueryf(io, "MKPOTO %s","%*[^,],%Le",Channel,PulseOffTime));
 		viCheckErr( auml24xx_ViQueryf(io, "MKPWTO %s","%*[^,],%Le",Channel,PulseWidth));
 		viCheckErr( auml24xx_ViQueryf(io, "MKPRIO %s","%*[^,],%Le",Channel,PulseRepetitionInterval));

    }
    else
    {
        *PulseFallTime = ((ViReal64)rand() / (ViReal64)RAND_MAX); 
	    *PulseRiseTime = ((ViReal64)rand() / (ViReal64)RAND_MAX); 
		*PulseOffTime = ((ViReal64)rand() / (ViReal64)RAND_MAX); 
		*PulseWidth = ((ViReal64)rand() / (ViReal64)RAND_MAX); 
		*PulseRepetitionInterval = ((ViReal64)rand() / (ViReal64)RAND_MAX); 
	}   
	

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_FetchStatisticalCursor
 * Purpose:  This function returns the Statistics cursor readings.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_FetchStatisticalCursor(
	ViSession       vi,
	ViConstString   ChannelName,
	ViReal64 *Percentage,
	ViReal64 *Power
)
{
    ViStatus error = VI_SUCCESS;
	ViChar   buf[BUFFER_SIZE];
	ViReal64 MeasurementData[2];
	ViInt32  ActualPoints;

    checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceChannelName (vi, ChannelName, &ChannelName) ); 
	if (Percentage == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Percentage");
	if (Power == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Power");

	if (!Ivi_Simulating(vi))                /* Call only when locked */
    {
       	ViSession   io = Ivi_IOSession(vi); 

       	checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
  		viCheckErr( auml24xx_ViPrintf(io, "TTMKRO %s", ChannelName));
  		viCheckErr( auml24xx_ViScanf(io, "%*[^,],%s",buf));
		viCheckErr( auml24xx_ParseRealArray(buf,VI_NULL, 2, MeasurementData, &ActualPoints));

		*Percentage=MeasurementData[0];
		*Power     =MeasurementData[1];

    }
    else
    {
		*Percentage = ((ViReal64)rand() / (ViReal64)RAND_MAX);
		*Power = ((ViReal64)rand() / (ViReal64)RAND_MAX);
    }

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;

}

/*****************************************************************************
 * Function: auml24xx_FetchPAE
 * Purpose:  This function returns the PAE reading.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_FetchPAE(
    ViSession       vi,
	ViConstString   ChannelName,
	ViReal64 *PAEData
)
{
    ViStatus error = VI_SUCCESS;
    checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceChannelName (vi, ChannelName, &ChannelName) ); 
	if (PAEData == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for PAEData");

	if (!Ivi_Simulating(vi))                /* Call only when locked */
    {
       	ViSession   io = Ivi_IOSession(vi); 

       	checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
  		viCheckErr( auml24xx_ViPrintf(io, "PAEO %s", ChannelName));
  		viCheckErr( auml24xx_ViScanf(io, "%*[^,],%Le", PAEData));

    }
    else
    {
	   *PAEData=((ViReal64)rand() / (ViReal64)RAND_MAX);
    }

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_FetchStatisticalPostProcessTrace
 * Purpose:  This function returns Statistical Post-processing Function
             Readings.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_FetchStatisticalPostProcessingTrace(
	ViSession       vi,
	ViConstString   ChannelName,
	ViInt32 *ActualPoints, 
	ViReal64 TraceData[],
	ViInt32 Length,
	ViReal64 *InitialX,
	ViReal64 *xIncrement
)
{
    ViStatus error = VI_SUCCESS;
	ViChar   buf[MEAS_BUFFER_SIZE];
	ViInt32  i;
	ViReal64 startPower;
	ViReal64 stopPower;

    checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceChannelName (vi, ChannelName, &ChannelName) ); 
    if (ActualPoints == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for ActualPoints");

	if (TraceData == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for TraceData");

	if (!Ivi_Simulating(vi))                /* Call only when locked */
    {
       	ViSession   io = Ivi_IOSession(vi); 	   

       	checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
        
		viCheckErr( auml24xx_ViPrintf (io, "TTPST? %s",ChannelName));
		viCheckErr( auml24xx_ViScanf(io,"%*[^,],%Le",&startPower));  
        viCheckErr( auml24xx_ViPrintf (io, "TTPSP? %s",ChannelName));
		viCheckErr( auml24xx_ViScanf(io,"%*[^,],%Le",&stopPower));  

		viCheckErr( auml24xx_ViPrintf (io, "TTFRO %s",ChannelName));
		viCheckErr( auml24xx_ViScanf(io,"%*[^,],%*[^,],%s",buf));  
		viCheckErr( auml24xx_ParseRealArray(buf,VI_NULL, Length, TraceData, ActualPoints));
		*InitialX = startPower;
		*xIncrement=(stopPower-startPower)/(*ActualPoints-1);
    }
    else
    {
		*ActualPoints = 400;
		for(i=0;i < *ActualPoints; i++)
		{
			TraceData[i]=((ViReal64)rand() / (ViReal64)RAND_MAX)*0.5;
		}
		*InitialX = -70;
		*xIncrement= 0.225;
		
    }

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}


/*****************************************************************************
 * Function: auml24xx_FetchGating
 * Purpose:  This function returns gating pattern measurement data.
 *           The sequence of measurement data will be different depending
 *           upon the selected measurement type number.
 *           Measurement Type:
 *           1 Average Power
 *           2 Average Power, Peak Power
 *           3 Average Power, Peak Power, Crest Factor
 *           4 Average Power, Min Power & Time , Max Power & Time
 *           5 Average Power, Held Min Power & Time, Held Max Power &
 *           Time
 *           Data Format
 *           1 avg_pow
 *           2 Avg_pow,Pk_pow
 *           3 Avg_pow,Pk_pow,Cres_Fact
 *           4 Avg_pow,min_pow,min_time,max_pow,max_time
 *           5 Avg_pow,hmin_pow,hmin_time,hmax_pow,hmax_time
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_FetchGating(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 GateNumber,
	ViInt32 *ActualPoints, 
	ViReal64 MeasurementData[]
)
{
    ViStatus error = VI_SUCCESS;
	ViChar   buf[MEAS_BUFFER_SIZE];
	ViInt32  i;
	ViInt32  measType;
	ViString gatingIgnore="PA,PK,CF,PN,TN,PX,TX,PHN,PHX,THN,THX";

	checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_CoerceChannelName (vi, Channel, &Channel) ); 

    if (GateNumber<1 || GateNumber > 8)
	    viCheckErr (IVI_ERROR_INVALID_VALUE);  

	if (ActualPoints == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for ActualPoints");

	if (MeasurementData == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 5, "Null address for MeasurementData");

	if (!Ivi_Simulating(vi))                /* Call only when locked */
    {
       	ViSession   io = Ivi_IOSession(vi); /* call only when locked  */

       	checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
         viCheckErr( auml24xx_ViPrintf (io, "GPNMO %s,%ld",Channel,GateNumber));
		viCheckErr( auml24xx_ViScanf(io,"%*[^,],%*[^,],%*[^,],%s",buf));  
		viCheckErr( auml24xx_ParseRealArray(buf,gatingIgnore,5, MeasurementData, ActualPoints));

    }
    else
    {
        checkErr( Ivi_GetAttributeViInt32 (vi, Channel, 
                                             AUML24XX_ATTR_PMOD_MEASUREMENT_TYPE, 
                                             0, &measType));
        switch(measType)
		{
			case AUML24XX_VAL_PMOD_MEAS_TYPE_1:
				*ActualPoints=1;
				break;
			case AUML24XX_VAL_PMOD_MEAS_TYPE_2:
				*ActualPoints=2;
				break;
			case AUML24XX_VAL_PMOD_MEAS_TYPE_3:
				*ActualPoints=3;
				break;
			case AUML24XX_VAL_PMOD_MEAS_TYPE_4:
				*ActualPoints=5;
				break;
			case AUML24XX_VAL_PMOD_MEAS_TYPE_5:
				*ActualPoints=5;
				break;
			default:
				*ActualPoints=1;				
				break;
		}
		for(i=0;i < *ActualPoints; i++)
		{
			MeasurementData[i]=((ViReal64)rand() / (ViReal64)RAND_MAX); 
		}
    }


Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: auml24xx_GetAttribute<type> Functions                                    
 * Purpose:  These functions enable the instrument driver user to get 
 *           attribute values directly.  There are typesafe versions for 
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession attributes.                                         
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_GetAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViInt32 *value)
{                                                                                                           
    return Ivi_GetAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                    value);
}                                                                                                           
ViStatus _VI_FUNC auml24xx_GetAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViReal64 *value)
{                                                                                                           
    return Ivi_GetAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}                                                                                                           
ViStatus _VI_FUNC auml24xx_GetAttributeViString (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViInt32 bufSize, 
                                                 ViChar value[]) 
{   
    return Ivi_GetAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     bufSize, value);
}   
ViStatus _VI_FUNC auml24xx_GetAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViBoolean *value)
{                                                                                                           
    return Ivi_GetAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           
ViStatus _VI_FUNC auml24xx_GetAttributeViSession (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViSession *value)
{                                                                                                           
    return Ivi_GetAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           

/*****************************************************************************
 * Function: auml24xx_SetAttribute<type> Functions                                    
 * Purpose:  These functions enable the instrument driver user to set 
 *           attribute values directly.  There are typesafe versions for 
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession datatypes.                                         
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_SetAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViInt32 value)
{                                                                                                           
    return Ivi_SetAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                    value);
}                                                                                                           
ViStatus _VI_FUNC auml24xx_SetAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViReal64 value)
{                                                                                                           
    return Ivi_SetAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}                                                                                                           
ViStatus _VI_FUNC auml24xx_SetAttributeViString (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViConstString value) 
{   
    return Ivi_SetAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}   
ViStatus _VI_FUNC auml24xx_SetAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViBoolean value)
{                                                                                                           
    return Ivi_SetAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           
ViStatus _VI_FUNC auml24xx_SetAttributeViSession (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViSession value)
{                                                                                                           
    return Ivi_SetAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           

/*****************************************************************************
 * Function: auml24xx_CheckAttribute<type> Functions                                  
 * Purpose:  These functions enable the instrument driver user to check  
 *           attribute values directly.  These functions check the value you
 *           specify even if you set the AUML24XX_ATTR_RANGE_CHECK 
 *           attribute to VI_FALSE.  There are typesafe versions for ViInt32, 
 *           ViReal64, ViString, ViBoolean, and ViSession datatypes.                         
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_CheckAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViInt32 value)
{                                                                                                           
    return Ivi_CheckAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}
ViStatus _VI_FUNC auml24xx_CheckAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                   ViAttr attributeId, ViReal64 value)
{                                                                                                           
    return Ivi_CheckAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                       value);
}                                                                                                           
ViStatus _VI_FUNC auml24xx_CheckAttributeViString (ViSession vi, ViConstString channelName, 
                                                   ViAttr attributeId, ViConstString value)  
{   
    return Ivi_CheckAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                       value);
}   
ViStatus _VI_FUNC auml24xx_CheckAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                    ViAttr attributeId, ViBoolean value)
{                                                                                                           
    return Ivi_CheckAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                        value);
}                                                                                                           
ViStatus _VI_FUNC auml24xx_CheckAttributeViSession (ViSession vi, ViConstString channelName, 
                                                    ViAttr attributeId, ViSession value)
{                                                                                                           
    return Ivi_CheckAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                        value);
}                                                                                                           

/*****************************************************************************
 * Function: WriteInstrData and ReadInstrData Functions                      
 * Purpose:  These functions enable the instrument driver user to  
 *           write and read commands directly to and from the instrument.            
 *                                                                           
 *           Note:  These functions bypass the IVI attribute state caching.  
 *                  WriteInstrData invalidates the cached values for all 
 *                  attributes.
 *****************************************************************************/
ViStatus _VI_FUNC auml24xx_WriteInstrData (ViSession vi, ViConstString writeBuffer)   
{   
    return Ivi_WriteInstrData (vi, writeBuffer);    
}   
ViStatus _VI_FUNC auml24xx_ReadInstrData (ViSession vi, ViInt32 numBytes, 
                                          ViChar rdBuf[], ViInt32 *bytesRead)  
{   
    return Ivi_ReadInstrData (vi, numBytes, rdBuf, bytesRead);   
} 

/*===========================================================================*
 *= Instrument Specific Group Functions =====================================*
 *===========================================================================*/

/*****************************************************************************
 *-------------------- Utility Functions (Not Exported) ---------------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: auml24xx_IviInit                                                       
 * Purpose:  This function is called by auml24xx_InitWithOptions  
 *           or by an IVI class driver.  This function initializes the I/O 
 *           interface, optionally resets the device, optionally performs an
 *           ID query, and sends a default setup to the instrument.                
 *****************************************************************************/
static ViStatus auml24xx_IviInit (ViRsrc resourceName, ViBoolean IDQuery,
                                    ViBoolean reset, ViSession vi)
{
    ViStatus    error = VI_SUCCESS;
    ViSession   io = VI_NULL;
	ViChar      modelStr[20] = "";
	ViInt32     modelNumber = AUML24XX_VAL_MODEL_ML2496A;
	ViChar      idnString[256] = "ANRITSU,ML2487A";
	ViInt32 	modelStrLen = 1;
	ViUInt16    interface = VI_INTF_GPIB;
	
	char		vModelList[][32]			=	{ "ML2487A" ,"ML2488A", "ML2495A" , "ML2496A" , "ML2487B" , "ML2488B" };
		
	checkErr( Ivi_BuildRepCapTable (vi, BNC_REP_CAP_NAME, BNC_LIST));

	checkErr( auml24xx_GetDriverSetupOption (vi, "Model", 20, modelStr) );

    if (!Ivi_Simulating(vi))
        {
        ViSession   rmSession = VI_NULL;

        /* Open instrument session */
        checkErr( Ivi_GetAttributeViSession (vi, VI_NULL, IVI_ATTR_VISA_RM_SESSION, 0,
                                             &rmSession)); 
        viCheckErr( viOpen (rmSession, resourceName, VI_NULL, VI_NULL, &io));
		
		auml24xx_ViPrintf (io, "*RST"); 
		
        /* io session owned by driver now */
        viCheckErr( viGetAttribute (io, VI_ATTR_INTF_TYPE, &interface));
        if (interface == VI_INTF_ASRL)  /* Serial Port */
            {
            viCheckErr( viSetAttribute (io, VI_ATTR_TERMCHAR, '\n'));
            viCheckErr( viSetAttribute (io, VI_ATTR_TERMCHAR_EN, VI_TRUE));
            viCheckErr( viSetAttribute (io, VI_ATTR_ASRL_END_IN, VI_ASRL_END_TERMCHAR));
            viCheckErr( viSetAttribute (io, VI_ATTR_ASRL_END_OUT, VI_ASRL_END_TERMCHAR));
            }
		else if (interface == VI_INTF_TCPIP)  /* Ethernet Port */         
		    {
		    viCheckErr( viSetAttribute (io, VI_ATTR_TERMCHAR, '\n'));
            viCheckErr( viSetAttribute (io, VI_ATTR_TERMCHAR_EN, VI_TRUE));
			}

        /* io session owned by driver now */
        checkErr( Ivi_SetAttributeViSession (vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, io));  

		/* Configure VISA Formatted I/O */
		viCheckErr( viSetAttribute (io, VI_ATTR_TMO_VALUE, 5000 ));
		viCheckErr( viSetBuf (io, VI_READ_BUF | VI_WRITE_BUF, 4000));
		viCheckErr( viSetAttribute (io, VI_ATTR_WR_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS));
		viCheckErr( viSetAttribute (io, VI_ATTR_RD_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS));
        }

	if (IDQuery && !Ivi_Simulating(vi))                               
    {
	   	ViChar rdBuffer[BUFFER_SIZE];

		viCheckErr( auml24xx_ViQueryf (io, "*IDN?", "%256[^\n]", rdBuffer));

		for ( modelNumber = 0; modelNumber < ( sizeof(vModelList) / sizeof(vModelList[0])); modelNumber++ )
			if ( strstr( rdBuffer , vModelList[modelNumber]	))
			{
				modelNumber++;
				break;
			}
	}
	else
	{
		/* Parse ID query string for model number only if the */
		/* user did not specify the model via option string   */
		if (!*modelStr)
			sscanf (idnString, "%*[^,],%[^,]", modelStr);		

		modelStrLen = strlen(modelStr)-1;

		error = Ivi_GetViInt32EntryFromString (	modelStr, 
			&attrInstrModelTypeRangeTable, 
			&modelNumber, 
			VI_NULL, 
			VI_NULL, 
			VI_NULL, 
			VI_NULL);

		if (error < VI_SUCCESS)
			viCheckErrElab( VI_ERROR_FAIL_ID_QUERY, "Failed to Recognize Model");
	}

	if(modelNumber ==  AUML24XX_VAL_MODEL_ML2487A||modelNumber==AUML24XX_VAL_MODEL_ML2495A||modelNumber==AUML24XX_VAL_MODEL_ML2487B)
    {
	  checkErr( Ivi_BuildChannelTable (vi, CHANNEL_LIST_SINGLE, VI_FALSE, VI_NULL));
      checkErr( Ivi_BuildRepCapTable (vi, SENSOR_REP_CAP_NAME, SENSOR_LIST_SINGLE));
    }
	else if (modelNumber ==  AUML24XX_VAL_MODEL_ML2488A|| modelNumber==AUML24XX_VAL_MODEL_ML2496A||modelNumber==AUML24XX_VAL_MODEL_ML2488B)
	{
      checkErr( Ivi_BuildChannelTable (vi, CHANNEL_LIST_DUAL, VI_FALSE, VI_NULL));
      checkErr( Ivi_BuildRepCapTable (vi, SENSOR_REP_CAP_NAME, SENSOR_LIST_DUAL));
	}

	checkErr( auml24xx_InitAttributes (vi, modelNumber) );

	/*setup channel count and repeat capability table*/
	checkErr(auml24xx_SetupChannelCount(vi)); 

     /* Add attributes */
	

   /*- Reset instrument ----------------------------------------------------*/
    if (reset) 
        checkErr( auml24xx_reset (vi));
    else  /*- Send Default Instrument Setup ---------------------------------*/
        checkErr( auml24xx_DefaultInstrSetup (vi));

    checkErr( auml24xx_CheckStatus (vi));


	
Error:
    if (error < VI_SUCCESS)
        {
        if (!Ivi_Simulating (vi) && io)
            viClose (io);
        }
    return error;
}

/*****************************************************************************
 * Function: auml24xx_IviClose                                                        
 * Purpose:  This function performs all of the drivers clean-up operations   
 *           except for closing the IVI session.  This function is called by 
 *           auml24xx_close or by an IVI class driver. 
 *
 *           Note:  This function must close the I/O session and set 
 *           IVI_ATTR_IO_SESSION to 0.
 *****************************************************************************/
static ViStatus auml24xx_IviClose (ViSession vi)
{
    ViStatus error = VI_SUCCESS;
    ViSession io = VI_NULL;

        /* Do not lock here.  The caller manages the lock. */

    checkErr( Ivi_GetAttributeViSession (vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, &io));

Error:
    Ivi_SetAttributeViSession (vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, VI_NULL);
    if(io)                                                      
        {
        viClose (io);
        }
    return error;   
}

/*****************************************************************************
 * Function: auml24xx_CheckStatus                                                 
 * Purpose:  This function checks the status of the instrument to detect 
 *           whether the instrument has encountered an error.  This function  
 *           is called at the end of most exported driver functions.  If the    
 *           instrument reports an error, this function returns      
 *           IVI_ERROR_INSTRUMENT_SPECIFIC.  The user can set the 
 *           AUML24XX_ATTR_QUERY_INSTRUMENT_STATUS attribute to VI_FALSE to disable this 
 *           check and increase execution speed.                                   
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus auml24xx_CheckStatus (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;

    if (Ivi_QueryInstrStatus (vi) && Ivi_NeedToCheckStatus (vi) && !Ivi_Simulating (vi))
        {
        checkErr( auml24xx_CheckStatusCallback (vi, Ivi_IOSession(vi)));
        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_FALSE));
        }

Error:
    return error;
}

/*****************************************************************************
 * Function: auml24xx_WaitForOPC                                                  
 * Purpose:  This function waits for the instrument to complete the      
 *           execution of all pending operations.  This function is a        
 *           wrapper for the WaitForOPCCallback.  It can be called from 
 *           other instrument driver functions. 
 *
 *           The maxTime parameter specifies the maximum time to wait for
 *           operation complete in milliseconds.
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus auml24xx_WaitForOPC (ViSession vi, ViInt32 maxTime) 
{
    ViStatus    error = VI_SUCCESS;

    if (!Ivi_Simulating(vi))
        {
        ViInt32  oldOPCTimeout; 

        checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_OPC_TIMEOUT, 
                                           0, &oldOPCTimeout));
        Ivi_SetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_OPC_TIMEOUT,        
                                 0, maxTime);

        error = auml24xx_WaitForOPCCallback (vi, Ivi_IOSession(vi));

        Ivi_SetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_OPC_TIMEOUT, 
                                 0, oldOPCTimeout);
        viCheckErr( error);
        }
Error:
    return error;
}

/*****************************************************************************
 * Function: auml24xx_DefaultInstrSetup                                               
 * Purpose:  This function sends a default setup to the instrument.  The    
 *           auml24xx_reset function calls this function.  The 
 *           auml24xx_IviInit function calls this function when the
 *           user passes VI_FALSE for the reset parameter.  This function is 
 *           useful for configuring settings that other instrument driver 
 *           functions require.    
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus auml24xx_DefaultInstrSetup (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;

    /* Invalidate all attributes */
    checkErr( Ivi_InvalidateAllAttributes (vi));

    if (!Ivi_Simulating(vi))
        {
        ViSession   io = Ivi_IOSession(vi); /* call only when locked */

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( auml24xx_ViPrintf (io, "*CLS;*ESE 1;*SRE 32"));

        }
Error:
    return error;
}

/*****************************************************************************
 * Function: ReadToFile and WriteFromFile Functions                          
 * Purpose:  Functions for instrument driver developers to read/write        
 *           instrument data to/from a file.                                 
 *****************************************************************************/
static ViStatus auml24xx_ReadToFile (ViSession vi, ViConstString filename, 
                                     ViInt32 maxBytesToRead, ViInt32 fileAction, 
                                     ViInt32 *totalBytesWritten)  
{   
    return Ivi_ReadToFile (vi, filename, maxBytesToRead, fileAction, totalBytesWritten);  
}   
static ViStatus auml24xx_WriteFromFile (ViSession vi, ViConstString filename, 
                                        ViInt32 maxBytesToWrite, ViInt32 byteOffset, 
                                        ViInt32 *totalBytesWritten) 
{   
    return Ivi_WriteFromFile (vi, filename, maxBytesToWrite, byteOffset, totalBytesWritten); 
}

/*****************************************************************************
 *  Function: auml24xx_GetStringFromTable
 *  Purpose:  
 *****************************************************************************/
static ViStatus auml24xx_GetStringFromTable(
	auml24xxStringValueTable table,
	ViInt32				  value,
	ViConstString*		  string
)
{
	ViStatus	error = VI_SUCCESS;
	ViBoolean	found = VI_FALSE;
	ViInt32		i;

	for( i = 0; table[i].string; i++)
	{
		if ( table[i].value == value )
		{
			*string = table[i].string;
			found = VI_TRUE;
			break;
		}
	}

	if ( !found )
		error = IVI_ERROR_INVALID_VALUE;

	return error;
}

/*****************************************************************************
 *  Function: auml24xx_GetRepCommandFromTable
 *  Purpose:  
 *****************************************************************************/
static ViStatus auml24xx_GetRepCommandFromTable(
	auml24xxStringValueTable table,
	ViInt32				  value,
	ViConstString*		  repCommand
)
{
	ViStatus	error = VI_SUCCESS;
	ViBoolean	found = VI_FALSE;
	ViInt32		i;

	for( i = 0; table[i].repCommand; i++)
	{
		if ( table[i].value == value )
		{
			*repCommand = table[i].repCommand;
			found = VI_TRUE;
			break;
		}
	}

	if ( !found )
		error = IVI_ERROR_INVALID_VALUE;

	return error;
}

/*****************************************************************************
 *  Function: auml24xx_GetValueFromTable
 *  Purpose:  
 *****************************************************************************/
static ViStatus auml24xx_GetValueFromTable(
	auml24xxStringValueTable	table,
	ViConstString			string,
	ViInt32*				value
)
{
	ViStatus	error = VI_SUCCESS;
	ViBoolean	found = VI_FALSE;
	ViInt32		i;

	for( i = 0; table[i].string; i++)
	{
		if ( strcmp(table[i].string, string) == 0 )
		{
			*value = table[i].value;
			found = VI_TRUE;
			break;
		}
	}

	if ( !found )
		error = IVI_ERROR_INVALID_VALUE;

	return error;
}

static ViStatus auml24xx_GetCount(
	ViInt32         measType,
	ViInt32		    *count)
{
   ViStatus	error = VI_SUCCESS;
   switch(measType)
   {
	   case 1: 
		   *count=1;
		   break;
	   case 2:
		   *count=2;
		   break;
	   case 3:
		   *count=3;
		   break;
	   case 4:
	   case 5:
		   *count=5;
		   break;
	   default:
		error = IVI_ERROR_INVALID_VALUE;
		   break;
   }
   return error;		   
}

/*****************************************************************************
 * Function: auml24xx_ParseRealArray
 * Purpose:  This function parses string with real values, which are
 *           separated by comma.
 *****************************************************************************/
static ViStatus	auml24xx_ParseRealArray(
	ViString		buffer,
	ViString        igoreString,
	ViInt32			count,
	ViReal64		data[],
	ViInt32*		nbOfValues)
{
	ViChar*		ptoken;
	ViInt32		index = 0;	

	ptoken = strtok (buffer, ",");
	while (ptoken)
	{
		if(igoreString)
		{	
			if(!strstr(igoreString,ptoken))
		       if (index < count)
			   {
				   data[index] = atof(ptoken);
		           index++;						
			   }
		}
		else
		{	
			if (index < count)
			   data[index] = atof(ptoken);
		       index++;						
		}
		ptoken = strtok (NULL, ",");
	}

	if (nbOfValues)
		*nbOfValues = ( index < count ) ? index : count;

	return ( index > count ) ? index : VI_SUCCESS;
}


/*****************************************************************************
 * Function: auml24xx_ParseRealArray
 * Purpose:  This function parses the errorBit into Error Informaiton.
 *****************************************************************************/
static ViStatus	auml24xx_ParseErrorBit(
	ViInt32		errorBit,
	ViInt32     index,
	ViChar      errorInfor[])
{
   ViStatus	error = VI_SUCCESS;

   switch(index)
   {
	   case 0: 
		   if(errorBit == 1)
		   {
			   strcpy (errorInfor,"Sensor A Zero not done.\n");
		   }
		   else if(errorBit == 2)
		   {
			   strcpy (errorInfor,"Sensor A Zero failed.\n");
		   }   
		   else
		   {
			   strcpy (errorInfor,"");
		   }
		   break;
	   case 1:
		   if(errorBit == 1)
		   {
			   strcpy (errorInfor,"Sensor B Zero not done.\n");
		   }
		   else if(errorBit == 2)
		   {
			   strcpy (errorInfor ,"Sensor B Zero failed.\n");
		   }   
		   else
		   {
			   strcpy (errorInfor,"");
		   }		  
		   break;
	   case 2:
		   if(errorBit == 1)
		   {
			   strcpy (errorInfor,"Sensor A Calibration failed.\n");
		   }
		   else
		   {
			   strcpy (errorInfor,"");
		   }		  
		   break;
	   case 3:
		   if(errorBit == 1)
		   {
			   strcpy (errorInfor,"Sensor B Calibration failed.\n");
		   }
		   else
		   {
			   strcpy (errorInfor,"");
		   }		  
		   break;
	   case 4:
		   if(errorBit == 1)
		   {
			   strcpy (errorInfor,"Sensor A range hold over range.\n");
		   }
		   else if(errorBit == 2)
		   {
			   strcpy (errorInfor,"Sensor A range hold under range.\n");
		   }   
		   else
		   {
			   strcpy (errorInfor,"");
		   }		  
		   break;
	   case 5:
		   if(errorBit == 1)
		   {
			   strcpy (errorInfor ,"Sensor B range hold over range.\n");
		   }
		   else if(errorBit == 2)
		   {
			   strcpy (errorInfor,"Sensor B range hold under range.\n");
		   }   
		   else
		   {
			   strcpy (errorInfor,"");
		   }		  
		   break;
	   case 6:
		   if(errorBit == 1)
		   {
			   strcpy (errorInfor,"Display channel 1 over range.\n");
		   }
		   else if(errorBit == 2)
		   {
			   strcpy (errorInfor,"Display channel 1 under range.\n");
		   }   
		   else
		   {
			   strcpy (errorInfor,"");
		   }		  
		   break;
	   case 7:
		   if(errorBit == 1)
		   {
			   strcpy (errorInfor,"Display channel 2 over range.\n");
		   }
		   else if(errorBit == 2)
		   {
			   strcpy (errorInfor,"Display channel 2 under range.\n");
		   }   
		   else
		   {
			   strcpy (errorInfor,"");
		   }		  
		   break;
	   case 8:
		   if(errorBit == 1)
		   {
			   strcpy (errorInfor,"Display channel 1 illegal log operation.\n");
		   }
		   else
		   {
			   strcpy (errorInfor,"");
		   }		  
		   break;
	   case 9:
		   if(errorBit == 1)
		   {
			   strcpy (errorInfor,"Display channel 2 illegal log operation.\n");
		   }
		   else
		   {
			   strcpy (errorInfor,"");
		   }		  
		   break;
	   case 10:
		   if(errorBit == 1)
		   {
			   strcpy (errorInfor,"Sensor A not fitted or used.\n");
		   }
		   else
		   {
			   strcpy (errorInfor,"");
		   }		  
		   break;
	   case 11:
		   if(errorBit == 1)
		   {
			   strcpy (errorInfor,"Sensor B not fitted or used.\n");
		   }
		   else
		   {
			   strcpy (errorInfor,"");
		   }		  
		   break;
	   case 12:
		   if(errorBit == 1)
		   {
			   strcpy (errorInfor,"Display channel 1 high limit failed.\n");
		   }
		   else if(errorBit == 2)
		   {
			   strcpy (errorInfor,"Display channel 1 low limit failed.\n");
		   }   
		   else
		   {
			   strcpy (errorInfor,"");
		   }		  
		   break;
	   case 13:
		   if(errorBit == 1)
		   {
			   strcpy (errorInfor,"Display channel 2 high limit failed.\n");
		   }
		   else if(errorBit == 2)
		   {
			   strcpy (errorInfor,"Display channel 2 low limit failed.\n");
		   }   
		   else
		   {
			   strcpy (errorInfor,"");
		   }		  
    	   break;

	   default:
		   error = IVI_ERROR_INVALID_VALUE;
		   break;
   }
   return error;		   

}
/*****************************************************************************  
 * Function: auml24xx_ViPrintf
 * Purpose:  This function performs viPrinf function based on different interface            
 *           type(ethernet or others).                                                
 *****************************************************************************/ 
static ViStatus	auml24xx_ViPrintf(ViSession vi, ViString writeFmt,...)
{
	ViStatus	error = VI_SUCCESS; 
    ViUInt16    interface = VI_INTF_GPIB;
	ViChar      fmt[BUFFER_SIZE];
	va_list     ap;
	
	
    checkErr( viGetAttribute (vi, VI_ATTR_INTF_TYPE, &interface));   	

	if (interface ==  VI_INTF_TCPIP)  /* Ethernet Port */        
	{
		strcpy(fmt,writeFmt);
		strcat(fmt,"\n");
	}
	else
	{
		strcpy(fmt,writeFmt);
	}
	
	va_start(ap, writeFmt);
	checkErr(viVPrintf(vi,fmt, ap));
	va_end(ap);

Error:
	return error;
}


/*****************************************************************************  
 * Function: auml24xx_ViScanf                                            
 * Purpose:  This function performs viScanf function based on different interface            
 *           type(serial or others).                                                
 *****************************************************************************/ 
static ViStatus	auml24xx_ViScanf(ViSession vi, ViString readFmt,...)
{
	ViStatus	error = VI_SUCCESS; 
    ViUInt16    interface = VI_INTF_GPIB;
	ViChar      fmt[BUFFER_SIZE];
	va_list     ap;
	
	
    checkErr( viGetAttribute (vi, VI_ATTR_INTF_TYPE, &interface));   	
    if (interface == VI_INTF_ASRL)  /* Serial Port */  
	{
		strcpy(fmt,"R");
		strcat(fmt,readFmt);
	}
	else 
	{
		strcpy(fmt,readFmt); 
	}
	
	va_start(ap, readFmt);
	checkErr(viVScanf(vi,fmt, ap));
	va_end(ap);

Error:
	return error;
}


/*****************************************************************************    
 * Function: auml24xx_ViQueryf                                                     
 * Purpose:  This function performs viQuery function based on different interface 
 *           type(serial or others).                                              
 *****************************************************************************/   
static ViStatus	auml24xx_ViQueryf(ViSession vi, ViString writeFmt, ViString readFmt,...)       
{                                                                          
	ViStatus	error = VI_SUCCESS;                                        
    ViUInt16    interface = VI_INTF_GPIB;                                  
	ViChar      rfmt[BUFFER_SIZE];                                          
	ViChar      wfmt[BUFFER_SIZE];                                          
	va_list     ap;                                                            
	                                                                       
	                                                                       
    checkErr( viGetAttribute (vi, VI_ATTR_INTF_TYPE, &interface));   	   
    if (interface == VI_INTF_ASRL)  /* Serial Port */                      
	{                                                                      
		strcpy(rfmt,"R");                                                   
		strcat(rfmt,readFmt);
		strcpy(wfmt,writeFmt);   
	}
	else if (interface ==  VI_INTF_TCPIP)  /* Ethernet Port */  
	{
		strcpy(rfmt,readFmt);
		strcpy(wfmt,writeFmt);                                                   
		strcat(wfmt,"\n");
	}	
	else                                                                   
	{                                                                      
		strcpy(rfmt,readFmt);
		strcpy(wfmt,writeFmt);
	}                                                                      
	                                                                       
	va_start(ap, readFmt);                                                 
	checkErr(viVQueryf(vi,wfmt,rfmt,ap));                                  
	va_end(ap);                                                            
                                                                           
Error:                                                                     
	return error;                                                          
}                                                                          

/*****************************************************************************
 * Function: auml24xx_SetupChannelCount
 * Purpose:  This function sets the AUML24XX_ATTR_CHANNEL_COUNTS in the init 
 * function.
 *****************************************************************************/
static ViStatus _VI_FUNC auml24xx_SetupChannelCount (ViSession vi)      
{
	ViStatus	error = VI_SUCCESS;
    ViInt32     modelType;
	ViInt32     flags;

	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_INSTR_MODEL_TYPE, 0, &modelType));
	if(modelType ==  AUML24XX_VAL_MODEL_ML2487A|| modelType==AUML24XX_VAL_MODEL_ML2495A||modelType==AUML24XX_VAL_MODEL_ML2487B)
	{
	    checkErr (Ivi_SetAttributeViInt32(vi, VI_NULL, AUML24XX_ATTR_CHANNEL_COUNTS,0,1));

	}
	else if (modelType ==  AUML24XX_VAL_MODEL_ML2488A|| modelType==AUML24XX_VAL_MODEL_ML2496A||modelType==AUML24XX_VAL_MODEL_ML2488B)
	{
    	 checkErr (Ivi_SetAttributeViInt32(vi, VI_NULL, AUML24XX_ATTR_CHANNEL_COUNTS,0,2));
	}
	else
	{
	     viCheckErr (IVI_ERROR_INVALID_VALUE);      
	}

Error:
	return error;
}

/*****************************************************************************
*  Function: auml24xx_GetDriverSetupOption
*  Purpose:  Get the value of a DriverSetup option.  The driver setup string
*            is part of the option string the user passes to the
*            auml24xx_InitWithOptions function.  Specifically it is the part
*            that follows "DriverSetup=".  This function assumes the 
*            DriverSetup string has the following format:
*              "OptionTag1 : OptionValue1; OptionTag2 : OptionValue2"
*            Leading and trailing white space is removed from the option
*            value and both the option tag and option value may contain
*            white space.  The option tag is case sensitive.
*
*            Example:
*
*              "DriverSetup= Model : InstrX ; Special Option : Feature Y "
*
*              optionTag "Model"          returns optionValue "InstrX"
*
*              optionTag "Special Option" returns optionValue "Feature Y"
*****************************************************************************/
static ViStatus auml24xx_GetDriverSetupOption (
	ViSession vi,
	ViConstString optionTag,
	ViInt32 optionValueSize,
	ViChar optionValue[] )
{                   
	ViStatus error = VI_SUCCESS;
	ViChar   setup[BUFFER_SIZE];
	ViChar*  searchOrigin;
	ViChar*  start;
	ViChar*  end;
	ViInt32  n;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	if ((optionValue != VI_NULL) && (optionValueSize > 0))
		optionValue[0] = 0;

	if ((optionTag != VI_NULL) && (optionValue != VI_NULL) && (optionValueSize > 1))
	{
		checkErr( Ivi_GetAttributeViString (vi, "", IVI_ATTR_DRIVER_SETUP, 0, BUFFER_SIZE, setup));
		searchOrigin = setup;
		while (searchOrigin != VI_NULL)
		{
			searchOrigin = strstr(searchOrigin, optionTag);
			if (searchOrigin)
			{
				searchOrigin += strlen(optionTag);
				start = searchOrigin;
				while (isspace(*start)) start++;
				if (*start == ':')
				{
					start++;
					while (isspace(*start)) start++;
					if (*start)
					{
						end = strchr (start, ';');
						if (start != end)
						{
							end = (end) ? --end : &setup[strlen(setup)-1];
							while (isspace(*end)) end--;
							n = end - start + 1;
							if (n > optionValueSize - 1)
								n = optionValueSize - 1;
							strncpy(optionValue, start, n);
							optionValue[n] = 0;
							break;
						}
					}
				}
			}
		}
	}

Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 *------------------------ Global Session Callbacks -------------------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: auml24xx_CheckStatusCallback                                               
 * Purpose:  This function queries the instrument to determine if it has 
 *           encountered an error.  If the instrument has encountered an 
 *           error, this function returns the IVI_ERROR_INSTRUMENT_SPECIFIC 
 *           error code.  This function is called by the 
 *           auml24xx_CheckStatus utility function.  The 
 *           Ivi_SetAttribute and Ivi_GetAttribute functions invoke this 
 *           function when the optionFlags parameter includes the
 *           IVI_VAL_DIRECT_USER_CALL flag.
 *           
 *           The user can disable calls to this function by setting the 
 *           AUML24XX_QUERY_INSTRUMENT_STATUS attribute to VI_FALSE.  The driver can 
 *           disable the check status callback for a particular attribute by 
 *           setting the IVI_VAL_DONT_CHECK_STATUS flag.
 *****************************************************************************/
static ViStatus _VI_FUNC auml24xx_CheckStatusCallback (ViSession vi, ViSession io)
{
    ViStatus    error = VI_SUCCESS;

	    ViInt16     esr = 0; 

	    viCheckErr( auml24xx_ViQueryf (io, "*ESR?", "%hd", &esr));
	    if (esr & IEEE_488_2_ERROR_MASK)
	        {
	        viCheckErr( IVI_ERROR_INSTR_SPECIFIC);
	        }

Error:
    return error;
}

/*****************************************************************************
 * Function: auml24xx_WaitForOPCCallback                                               
 * Purpose:  This function waits until the instrument has finished processing 
 *           all pending operations.  This function is called by the 
 *           auml24xx_WaitForOPC utility function.  The IVI engine invokes
 *           this function in the following two cases:
 *           - Before invoking the read callback for attributes for which the 
 *             IVI_VAL_WAIT_FOR_OPC_BEFORE_READS flag is set.
 *           - After invoking the write callback for attributes for which the 
 *             IVI_VAL_WAIT_FOR_OPC_AFTER_WRITES flag is set.
 *****************************************************************************/
static ViStatus _VI_FUNC auml24xx_WaitForOPCCallback (ViSession vi, ViSession io)
{
    ViStatus    error = VI_SUCCESS;

    ViInt32     opcTimeout;
    ViUInt16    statusByte;
	ViInt32     oldTime;  
	ViBoolean   opcFinish;
	ViUInt16    interface = VI_INTF_GPIB;
	
	checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_OPC_TIMEOUT,   	
                                       0, &opcTimeout));                             	
    viCheckErr( viGetAttribute (io, VI_ATTR_INTF_TYPE, &interface)); 	
	if(interface == VI_INTF_ASRL ||interface == VI_INTF_TCPIP)
	{
    	viCheckErr( viGetAttribute (io, VI_ATTR_TMO_VALUE, &oldTime ));  
		viCheckErr( viSetAttribute (io, VI_ATTR_TMO_VALUE, opcTimeout ));   
		                                                                    
		viCheckErr( auml24xx_ViQueryf (io, "*OPC?", "%hd", &opcFinish));          
			                                                                 
		viCheckErr( viSetAttribute (io, VI_ATTR_TMO_VALUE, oldTime ));
	}
    else
	{
        viCheckErr( viEnableEvent (io, VI_EVENT_SERVICE_REQ, VI_QUEUE, VI_NULL));

        viCheckErr( auml24xx_ViPrintf (io, "*OPC"));

        /*wait for SRQ */
        viCheckErr( viWaitOnEvent (io, VI_EVENT_SERVICE_REQ, opcTimeout, VI_NULL, VI_NULL));
        viCheckErr( viDisableEvent (io, VI_EVENT_SERVICE_REQ, VI_QUEUE));

        /*check status*/
        checkErr( auml24xx_CheckStatus (vi));

        viCheckErr( viBufWrite (io, "*CLS", 4, VI_NULL));
        viCheckErr( viReadSTB (io, &statusByte));
	}

Error:
    if((interface != VI_INTF_ASRL) && (interface != VI_INTF_TCPIP)) 
	{
	 viDiscardEvents (io, VI_EVENT_SERVICE_REQ, VI_QUEUE);
	}

    return error;
}

/*****************************************************************************
* Function: auml24xx_RemoveSurroundingWhiteSpace
* Purpose:  This function removes surrounding white space around a string
*****************************************************************************/
static void auml24xx_RemoveSurroundingWhiteSpace(ViChar string[])
{
	ViChar  *start = string,
		*end;
	ViInt32 lengthWithoutLeadingWS;

	if (start)
	{
		while (*start && isspace((unsigned char)*start))
			start++;

		lengthWithoutLeadingWS = (ViInt32)strlen( start);
		memmove( string, start, lengthWithoutLeadingWS);
		end = string + lengthWithoutLeadingWS;
		while (end > string && isspace((unsigned char)*(end-1)))
			end--;

		*end = '\0';
	}
}
/*****************************************************************************
* Function: auml24xx_GetModelFromBuffer
* Purpose:  This function returns model number from buffer. If the 
*		    instrument model table does not contains this model, function
*           returns VI_ERROR_FAIL_ID_QUERY;
*****************************************************************************/
static ViStatus auml24xx_GetModelFromBuffer(
	ViSession       vi,
	ViConstString   buffer,
	ViInt32*        pModelType
	)
{
	ViStatus   	error = VI_SUCCESS;
	ViChar		tmpBuffer[BUFFER_SIZE];

	if(!pModelType)
		viCheckErr(IVI_ERROR_INVALID_VALUE);

	sscanf(buffer, "%*[^,],%[^,]", tmpBuffer);
	auml24xx_RemoveSurroundingWhiteSpace(tmpBuffer);

	error = Ivi_GetViInt32EntryFromString ( tmpBuffer, &attrInstrModelTypeRangeTable, pModelType, 
											VI_NULL, VI_NULL, VI_NULL, VI_NULL );
	if(error < VI_SUCCESS)
		viCheckErr(VI_ERROR_FAIL_ID_QUERY);

Error:
	return error;
}

/*****************************************************************************
 *----------------- Attribute Range Tables and Callbacks --------------------*
 *****************************************************************************/
/*****************************************************************************
 * Function: auml24xxEnum_WriteCallback                                               
 * Purpose:  This function sets the enum value of the ViInt32 attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC auml24xxEnum_WriteCallback (
	ViSession		vi,
	ViSession		io,
	ViConstString	channelName,
	ViAttr			attributeId,
	ViInt32			value)
{
	ViStatus			error = VI_SUCCESS;
	ViString 			command,
						commandValue;
	ViString            repCommand;
	IviRangeTablePtr 	table;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxEnum_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxEnum_commands, attributeId, &repCommand));

	checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));
	viCheckErr( auml24xx_GetCmdFromIntValue(value, table, &commandValue));

	if (!Ivi_Simulating (vi))
	{
		viCheckErr( auml24xx_ViPrintf (io, "%s %s%s%s", command, VALID_CHANNELNAME (channelName), repCommand, commandValue));
	}
Error:
	return error;
}

/*****************************************************************************
 * Function: auml24xxEnum_ReadCallback                                               
 * Purpose:  This function reads the enum value of the ViInt32 attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC auml24xxEnum_ReadCallback (
	ViSession		vi,
	ViSession		io,
	ViConstString	channelName,
	ViAttr			attributeId,
	ViInt32*		value
)
{
	ViStatus			error = VI_SUCCESS;
	ViString 			command;
	ViString            repCommand;
	IviRangeTablePtr 	table;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxEnum_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxEnum_commands, attributeId, &repCommand));

	checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));

	if (!Ivi_Simulating (vi))
	{
		ViChar	rdBuffer[BUFFER_SIZE];

		viCheckErr( auml24xx_ViPrintf (io, "%s? %s", command, VALID_CHANNELNAME (channelName)));
		viCheckErr( auml24xx_ViScanf(io, "%*[^,],%s", rdBuffer));
		viCheckErr( auml24xx_GetValueFromCmd(rdBuffer, table, value));
	}
Error:
	return error;
}

/*****************************************************************************
 * Function: auml24xxViBoolean_WriteCallback                                               
 * Purpose:  This function sets the bool value of the ViBoolean attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC auml24xxViBoolean_WriteCallback (
	ViSession		vi,
	ViSession		io,
	ViConstString	channelName,
	ViAttr			attributeId,
	ViBoolean		value
)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViBoolean_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViBoolean_commands, attributeId, &repCommand));

	if (!Ivi_Simulating (vi))
		viCheckErr( auml24xx_ViPrintf (io, "%s %s%s%s", command, VALID_CHANNELNAME (channelName), repCommand, value ? "ON" : "OFF"));

Error:
	return error;
}

/*****************************************************************************
 * Function: auml24xxViBoolean_ReadCallback                                               
 * Purpose:  This function reads the bool value of the ViBoolean attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC auml24xxViBoolean_ReadCallback (
	ViSession		vi,
	ViSession		io,
	ViConstString	channelName,
	ViAttr 			attributeId,
	ViBoolean*		value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViBoolean_commands, attributeId, &command)); 

	if (!Ivi_Simulating (vi))
	{
	    ViChar	rdBuffer[BUFFER_SIZE];
		viCheckErr( auml24xx_ViPrintf (io, "%s? %s", command, VALID_CHANNELNAME (channelName)));
		viCheckErr( auml24xx_ViScanf(io, "%*[^,],%s", rdBuffer));
		if (!strcmp(rdBuffer,"OFF"))
		{
		   *value=VI_FALSE;
		}
		else if(!strcmp(rdBuffer,"ON"))

		{
		   *value=VI_TRUE;
		}
		else
		{
			viCheckErr (IVI_ERROR_INVALID_VALUE);
		}

	}
Error:
	return error;
}

/*****************************************************************************
 * Function: auml24xxViString_WriteCallback                                               
 * Purpose:  This function sets the string value of the ViString attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC auml24xxAttrInstrInternalTriggerEventSource_WriteCallback (ViSession vi,
                                                                                    ViSession io,
                                                                                    ViConstString channelName,
                                                                                    ViAttr attributeId,
                                                                                    ViConstString value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViString_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViString_commands, attributeId, &repCommand));

	if (!Ivi_Simulating (vi))
		viCheckErr( auml24xx_ViPrintf (io, "%s %s%s,INT%s", command, VALID_CHANNELNAME (channelName), repCommand, value));

Error:
	return error;
}

/*****************************************************************************
 * Function: auml24xxViString_ReadCallback                                               
 * Purpose:  This function reads the string value of the ViString attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC auml24xxAttrInstrInternalTriggerEventSource_ReadCallback (ViSession vi,
                                                                                   ViSession io,
                                                                                   ViConstString channelName,
                                                                                   ViAttr attributeId,
                                                                                   const ViConstString cacheValue)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;
	viCheckErr( auml24xx_GetStringFromTable (auml24xxViString_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViString_commands, attributeId, &repCommand));

	if (!Ivi_Simulating (vi))
	{
		ViChar		rdBuffer[BUFFER_SIZE];

		viCheckErr( auml24xx_ViPrintf (io, "%s? %s%s", command, VALID_CHANNELNAME (channelName), repCommand));
		viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%s", rdBuffer));
		if (strcmp(rdBuffer,"INTA") && strcmp(rdBuffer,"INTB"))
		{
			viCheckErr(AUML24XX_ERROR_ATTR_NO_INTERNAL_TRIIGER_SOURCE);
		}
		else if(!strcmp(rdBuffer,"INTA"))
		{
			 checkErr( Ivi_SetValInStringCallback(vi, attributeId, "A")); 
		}
		else if(!strcmp(rdBuffer,"INTB"))
		{
			 checkErr( Ivi_SetValInStringCallback(vi, attributeId, "B")); 
		}
	}
Error:
	return error;
}

/*****************************************************************************
 * Function: auml24xxViReal64_WriteCallback                                               
 * Purpose:  This function sets the real value of the ViReal64 attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC auml24xxViReal64_WriteCallback (
	ViSession		vi,
	ViSession		io,
	ViConstString	channelName,
	ViAttr			attributeId,
	ViReal64		value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));

	if (!Ivi_Simulating (vi))
		viCheckErr( auml24xx_ViPrintf (io, "%s %s%s%Le", command, VALID_CHANNELNAME (channelName), repCommand, value));

Error:
	return error;
}

/*****************************************************************************
 * Function: auml24xxViReal64_ReadCallback                                               
 * Purpose:  This function reads the real value of the ViReal64 attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC auml24xxViReal64_ReadCallback (
	ViSession		vi,
	ViSession		io,
	ViConstString	channelName,
	ViAttr 			attributeId,
	ViReal64*		value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));

	if (!Ivi_Simulating (vi))
	{
		viCheckErr( auml24xx_ViPrintf (io, "%s? %s",command, VALID_CHANNELNAME (channelName)));
		viCheckErr( auml24xx_ViScanf(io, "%*[^,],%Le", value));

	}
Error:
	return error;
}

/*****************************************************************************
 * Function: auml24xxViInt32_WriteCallback                                               
 * Purpose:  This function sets the int value of the ViInt32 attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC auml24xxViInt32_WriteCallback (
	ViSession		vi,
	ViSession		io,
	ViConstString	channelName,
	ViAttr			attributeId,
	ViInt32		    value
)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViInt32_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViInt32_commands, attributeId, &repCommand));

	if (!Ivi_Simulating (vi))
		viCheckErr( auml24xx_ViPrintf (io, "%s %s%s%ld", command, VALID_CHANNELNAME (channelName), repCommand, value));

Error:
	return error;
}

/*****************************************************************************
 * Function: auml24xxViInt32_ReadCallback                                               
 * Purpose:  This function reads the int value of the ViInt32 attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC auml24xxViInt32_ReadCallback (
	ViSession		vi,
	ViSession		io,
	ViConstString	channelName,
	ViAttr 			attributeId,
	ViInt32*		value
)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViInt32_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViInt32_commands, attributeId, &repCommand));

	if (!Ivi_Simulating (vi))
	{
			viCheckErr( auml24xx_ViPrintf (io, "%s? %s",command, VALID_CHANNELNAME (channelName)));
	        viCheckErr( auml24xx_ViScanf(io, "%*[^,],%ld", value));
	}

Error:
	return error;
}

/*****************************************************************************
 * Function: auml24xxAttrPAEViInt32_CheckCallback                                              
 * Purpose:  This function checks whether the instrument could do PAE processing
 *           based on the instrument model(channel count).			
 *****************************************************************************/
static ViStatus _VI_FUNC auml24xxAttrPAEViInt32_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32     modelType;

	checkErr (Ivi_DefaultCheckCallbackViInt32 (vi, channelName, attributeId, value));
	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_INSTR_MODEL_TYPE, 0, &modelType));

	if(modelType == AUML24XX_VAL_MODEL_ML2487A || modelType == AUML24XX_VAL_MODEL_ML2495A || modelType == AUML24XX_VAL_MODEL_ML2487B) 
	{
	  viCheckErr (IVI_ERROR_INVALID_CONFIGURATION); 
	}
Error:
	return error;
}

/*****************************************************************************
 * Function: auml24xxAttrPAEViReal64_CheckCallback                                              
 * Purpose:  This function checks whether the instrument could do PAE processing
 *           based on the instrument model(channel count).			
 *****************************************************************************/
static ViStatus _VI_FUNC auml24xxAttrPAEViReal64_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32     modelType;

	checkErr (Ivi_DefaultCheckCallbackViReal64 (vi, channelName, attributeId, value));
	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_INSTR_MODEL_TYPE, 0, &modelType));

	if(modelType == AUML24XX_VAL_MODEL_ML2487A || modelType == AUML24XX_VAL_MODEL_ML2495A || modelType == AUML24XX_VAL_MODEL_ML2487B) 
	{
	  viCheckErr (IVI_ERROR_INVALID_CONFIGURATION); 
	}
Error:
	return error;
}
/*- AUML24XX_ATTR_ID_QUERY_RESPONSE -*/

static ViStatus _VI_FUNC auml24xxAttrIdQueryResponse_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    ViUInt32    retCnt;

    viCheckErr( auml24xx_ViPrintf (io, "*IDN?"));
    viCheckErr( viRead (io, rdBuffer, BUFFER_SIZE-1, &retCnt));
    rdBuffer[retCnt] = 0;

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));

Error:
    return error;
}

/*- AUML24XX_ATTR_SPECIFIC_DRIVER_REVISION -*/

static ViStatus _VI_FUNC auml24xxAttrDriverRevision_ReadCallback (ViSession vi, 
                                                                  ViSession io,
                                                                  ViConstString channelName, 
                                                                  ViAttr attributeId,
                                                                  const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      driverRevision[256];

    sprintf (driverRevision, 
             "Driver: auml24xx %.2f, Compiler: %s %3.2f, "
             "Components: IVIEngine %.2f, VISA-Spec %.2f",
             AUML24XX_MAJOR_VERSION + AUML24XX_MINOR_VERSION/1000.0, 
             IVI_COMPILER_NAME, IVI_COMPILER_VER_NUM, 
             IVI_ENGINE_MAJOR_VERSION + IVI_ENGINE_MINOR_VERSION/1000.0, 
             Ivi_ConvertVISAVer(VI_SPEC_VERSION));

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, driverRevision));    
Error:
    return error;
}

/*- AUML24XX_ATTR_INSTRUMENT_FIRMWARE_REVISION -*/

static ViStatus _VI_FUNC auml24xxAttrFirmwareRevision_ReadCallback (ViSession vi, 
                                                                    ViSession io,
                                                                    ViConstString channelName, 
                                                                    ViAttr attributeId,
                                                                    const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      idQ[BUFFER_SIZE], rdBuffer[BUFFER_SIZE];

    checkErr( Ivi_GetAttributeViString (vi, "", AUML24XX_ATTR_ID_QUERY_RESPONSE,
                                        0, BUFFER_SIZE, idQ));

	sscanf (idQ, "%*[^,],%*[^,],%*[^,],%256[^\n]", rdBuffer);

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));

Error:
    return error;
}

/*- AUML24XX_ATTR_INSTRUMENT_MANUFACTURER -*/

static ViStatus _VI_FUNC auml24xxAttrInstrumentManufacturer_ReadCallback (ViSession vi, 
                                                                          ViSession io,
                                                                          ViConstString channelName, 
                                                                          ViAttr attributeId,
                                                                          const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    ViChar      idQ[BUFFER_SIZE];

    checkErr( Ivi_GetAttributeViString (vi, "", AUML24XX_ATTR_ID_QUERY_RESPONSE,
                                        0, BUFFER_SIZE, idQ));
    sscanf (idQ, "%256[^,]", rdBuffer);

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));

Error:
    return error;
}

/*- AUML24XX_ATTR_INSTRUMENT_MODEL -*/

static ViStatus _VI_FUNC auml24xxAttrInstrumentModel_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    ViChar      idQ[BUFFER_SIZE];

    checkErr( Ivi_GetAttributeViString (vi, "", AUML24XX_ATTR_ID_QUERY_RESPONSE,
                                        0, BUFFER_SIZE, idQ));
    sscanf (idQ, "%*[^,],%256[^,]", rdBuffer);

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));

Error:
    return error;
}

/*- AUML24XX_ATTR_AVERAGING_AUTO_ENABLED -*/
static ViStatus _VI_FUNC auml24xxAttrAveragingAutoEnabled_ReadCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViBoolean      *value
)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
    ViChar	rdBuffer[BUFFER_SIZE]; 
	viCheckErr( auml24xx_GetStringFromTable (auml24xxViBoolean_commands, attributeId, &command)); 

 
	viCheckErr( auml24xx_ViPrintf (io, "%s? %s", command, VALID_CHANNELNAME (channelName)));

	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%[^,],%s ", rdBuffer));
	if (!strcmp(rdBuffer,"AUTO"))
		{
		   *value= VI_TRUE;
		}
	else if( (!strcmp(rdBuffer,"OFF"))||(!strcmp(rdBuffer,"RPT"))||(!strcmp(rdBuffer,"MOV")))

		{
		   *value=VI_FALSE;
		}
	else
		{
			viCheckErr (IVI_ERROR_INVALID_VALUE);
		}

Error:
	return error;
}

/*- AUML24XX_ATTR_AVERAGING_AUTO_ENABLED */
static ViStatus _VI_FUNC auml24xxAttrAveragingAutoEnabled_WriteCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViBoolean       value
)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViBoolean_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViBoolean_commands, attributeId, &repCommand));
	viCheckErr( auml24xx_ViPrintf (io, "%s %s%s%s,", command, VALID_CHANNELNAME (channelName), repCommand, value ? "AUTO" : "MOV"));

Error:
	return error;
}

/*- AUML24XX_ATTR_TRIGGER_SOURCE -*/ 
static ViStatus _VI_FUNC auml24xxAttrTriggerSource_ReadCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32        *value
)
{
    ViStatus    error = VI_SUCCESS;

	ViInt32          channelMeasMode;
	ViAttr           targetAttributeId;
	ViInt32          activeChannel;
	ViChar           channel[BUFFER_SIZE];

	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_ACTIVE_CHANNEL, 0, &activeChannel));   
	sprintf(channel,"%d",activeChannel);

    /* Get the corresponding internal attributes here */  
	checkErr (Ivi_GetAttributeViInt32 (vi, channel, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &channelMeasMode));
		switch (channelMeasMode) 
		{
		  case AUML24XX_VAL_MEAS_MODE_CW:
			{
				targetAttributeId = AUML24XX_ATTR_INSTR_CW_TRIGGER_SOURCE; 
			}
			 break;
	      case AUML24XX_VAL_MEAS_MODE_PMOD:
			 {
			    targetAttributeId = AUML24XX_ATTR_INSTR_PMOD_TRIGGER_SOURCE; 
			 }
			break;

		default:
			break;
	     }

	checkErr (Ivi_GetAttributeViInt32 (vi, channel, targetAttributeId, 0, value));  	

Error:
    return error;
}

/*- AUML24XX_ATTR_TRIGGER_SOURCE -*/  
static ViStatus _VI_FUNC auml24xxAttrTriggerSource_WriteCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32         value
)
{
    ViStatus    error = VI_SUCCESS;

	ViInt32     channelMeasMode;
	ViAttr      targetAttributeId;
	ViInt32          activeChannel;
	ViChar           channel[BUFFER_SIZE];

	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_ACTIVE_CHANNEL, 0, &activeChannel));   
	sprintf(channel,"%d",activeChannel);

	/* Get the corresponding internal attributes here */  
	checkErr (Ivi_GetAttributeViInt32 (vi, channel, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &channelMeasMode));
		switch (channelMeasMode) 
		{
		  case AUML24XX_VAL_MEAS_MODE_CW:
			{
				targetAttributeId = AUML24XX_ATTR_INSTR_CW_TRIGGER_SOURCE; 
			}
			 break;
	      case AUML24XX_VAL_MEAS_MODE_PMOD:
			 {
			    targetAttributeId = AUML24XX_ATTR_INSTR_PMOD_TRIGGER_SOURCE; 
			 }
			break;

		default:
			break;
	     }

	checkErr (Ivi_SetAttributeViInt32 (vi, channel, targetAttributeId, 0, value));  	

Error:
    return error;
}

/*- AUML24XX_ATTR_INTERNAL_TRIGGER_EVENT_SOURCE -*/
static ViStatus _VI_FUNC auml24xxAttrInternalTriggerEventSource_ReadCallback(
    ViSession               vi,
    ViSession               io,
    ViConstString           channelName,
    ViAttr                  attributeId,
    const ViConstString     cacheValue
)
{    
	ViStatus    error = VI_SUCCESS;
	ViChar      buffer[BUFFER_SIZE];
	ViInt32     channelMeasMode;
	ViAttr      targetAttributeId;
	ViInt32     activeChannel;
	ViChar      channel[BUFFER_SIZE];

	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_ACTIVE_CHANNEL, 0, &activeChannel));   
	sprintf(channel,"%d",activeChannel);
	/* Get the corresponding internal attributes here */  
	checkErr (Ivi_GetAttributeViInt32 (vi, channel, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &channelMeasMode));
		switch (channelMeasMode) 
		{
		  case AUML24XX_VAL_MEAS_MODE_CW:
			{
				targetAttributeId = AUML24XX_ATTR_INSTR_CW_INTERNAL_TRIGGER_EVENT_SOURCE; 
			}
			 break;
	      case AUML24XX_VAL_MEAS_MODE_PMOD:
			 {
			    targetAttributeId = AUML24XX_ATTR_INSTR_PMOD_INTERNAL_TRIGGER_EVENT_SOURCE; 
			 }
			break;

		default:
			break;
	     }

	checkErr (Ivi_GetAttributeViString (vi, channel, targetAttributeId, 0, BUFFER_SIZE, buffer));  	
	checkErr( Ivi_SetValInStringCallback(vi, attributeId, buffer));

Error:
    return error;
}

static ViStatus _VI_FUNC auml24xxAttrInternalTriggerEventSource_WriteCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViConstString   value
)
{
    ViStatus    error = VI_SUCCESS;

	ViInt32     channelMeasMode;
	ViAttr      targetAttributeId;
	ViInt32     activeChannel;
	ViChar      channel[BUFFER_SIZE];

	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_ACTIVE_CHANNEL, 0, &activeChannel));   
	sprintf(channel,"%d",activeChannel);
	/* Get the corresponding internal attributes here */  
	checkErr (Ivi_GetAttributeViInt32 (vi, channel, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &channelMeasMode));
		switch (channelMeasMode) 
		{
		  case AUML24XX_VAL_MEAS_MODE_CW:
			{
				targetAttributeId = AUML24XX_ATTR_INSTR_CW_INTERNAL_TRIGGER_EVENT_SOURCE; 
			}
			 break;
	      case AUML24XX_VAL_MEAS_MODE_PMOD:
			 {
			    targetAttributeId = AUML24XX_ATTR_INSTR_PMOD_INTERNAL_TRIGGER_EVENT_SOURCE; 
			 }
			break;

		default:
			break;
	     }

	checkErr (Ivi_SetAttributeViString (vi, channel, targetAttributeId, 0, value));  	

Error:
    return error;
}

/*- AUML24XX_ATTR_INTERNAL_TRIGGER_LEVEL -*/
static ViStatus _VI_FUNC auml24xxAttrInternalTriggerLevel_ReadCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViReal64       *value
)											
{
    ViStatus    error = VI_SUCCESS;

	ViInt32     channelMeasMode;
	ViAttr      targetAttributeId;
	ViInt32     activeChannel;
	ViChar      channel[BUFFER_SIZE];

	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_ACTIVE_CHANNEL, 0, &activeChannel));   
	sprintf(channel,"%d",activeChannel);
	/* Get the corresponding internal attributes here */  
	checkErr (Ivi_GetAttributeViInt32 (vi, channel, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &channelMeasMode));
		switch (channelMeasMode) 
		{
		  case AUML24XX_VAL_MEAS_MODE_CW:
			{
				targetAttributeId = AUML24XX_ATTR_INSTR_CW_TRIGGER_INTERNAL_LEVEL; 
			}
			 break;
	      case AUML24XX_VAL_MEAS_MODE_PMOD:
			 {
			    targetAttributeId = AUML24XX_ATTR_INSTR_PMOD_TRIGGER_INTERNAL_LEVEL; 
			 }
			break;

		default:
			break;
	     }

	checkErr (Ivi_GetAttributeViReal64 (vi, channel, targetAttributeId, 0, value));  	

Error:
    return error;
}

static ViStatus _VI_FUNC auml24xxAttrInternalTriggerLevel_WriteCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViReal64        value
)
{
    ViStatus    error = VI_SUCCESS;
	ViInt32     channelMeasMode;
	ViAttr      targetAttributeId;
	ViInt32     activeChannel;
	ViChar      channel[BUFFER_SIZE];

	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_ACTIVE_CHANNEL, 0, &activeChannel));   
	sprintf(channel,"%d",activeChannel);
	/* Get the corresponding internal attributes here */  
	checkErr (Ivi_GetAttributeViInt32 (vi, channel, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &channelMeasMode));
		switch (channelMeasMode) 
		{
		  case AUML24XX_VAL_MEAS_MODE_CW:
			{
				targetAttributeId = AUML24XX_ATTR_INSTR_CW_TRIGGER_INTERNAL_LEVEL; 
			}
			 break;
	      case AUML24XX_VAL_MEAS_MODE_PMOD:
			 {
			    targetAttributeId = AUML24XX_ATTR_INSTR_PMOD_TRIGGER_INTERNAL_LEVEL; 
			 }
			break;

		default:
			break;
	     }

	checkErr (Ivi_SetAttributeViReal64 (vi, channel, targetAttributeId, 0, value));  	

Error:
    return error;
}

/*- AUML24XX_ATTR_INTERNAL_TRIGGER_SLOPE -*/

static ViStatus _VI_FUNC auml24xxAttrInternalTriggerSlope_ReadCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32        *value
)
{
    ViStatus    error = VI_SUCCESS;

	ViInt32     channelMeasMode;
	ViAttr      targetAttributeId;
	ViInt32     activeChannel;
	ViChar      channel[BUFFER_SIZE];

	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_ACTIVE_CHANNEL, 0, &activeChannel));   
	sprintf(channel,"%d",activeChannel);
	/* Get the corresponding internal attributes here */  
	checkErr (Ivi_GetAttributeViInt32 (vi, channel, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &channelMeasMode));
		switch (channelMeasMode) 
		{
		  case AUML24XX_VAL_MEAS_MODE_CW:
			{
				targetAttributeId = AUML24XX_ATTR_INSTR_CW_TRIGGER_INTERNAL_SLOPE; 
			}
			 break;
	      case AUML24XX_VAL_MEAS_MODE_PMOD:
			 {
			    targetAttributeId = AUML24XX_ATTR_INSTR_PMOD_TRIGGER_INTERNAL_SLOPE; 
			 }
			break;

		default:
			break;
	     }

	checkErr (Ivi_GetAttributeViInt32 (vi, channel, targetAttributeId, 0, value));  	

Error:
    return error;
}

static ViStatus _VI_FUNC auml24xxAttrInternalTriggerSlope_WriteCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32         value
)
{
    ViStatus    error = VI_SUCCESS;
	ViInt32     channelMeasMode;
	ViAttr      targetAttributeId;
	ViInt32     activeChannel;
	ViChar      channel[BUFFER_SIZE];

	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_ACTIVE_CHANNEL, 0, &activeChannel));   
	sprintf(channel,"%d",activeChannel);
	/* Get the corresponding internal attributes here */  
	checkErr (Ivi_GetAttributeViInt32 (vi, channel, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &channelMeasMode));
		switch (channelMeasMode) 
		{
		  case AUML24XX_VAL_MEAS_MODE_CW:
			{
				targetAttributeId = AUML24XX_ATTR_INSTR_CW_TRIGGER_INTERNAL_SLOPE; 
			}
			 break;
	      case AUML24XX_VAL_MEAS_MODE_PMOD:
			 {
			    targetAttributeId = AUML24XX_ATTR_INSTR_PMOD_TRIGGER_INTERNAL_SLOPE; 
			 }
			break;

		default:
			break;
	     }

	checkErr (Ivi_SetAttributeViInt32 (vi, channel, targetAttributeId, 0, value));  	

Error:
    return error;
}

/*- AUML24XX_ATTR_DUTY_CYCLE_CORRECTION_ENABLED -*/

static ViStatus _VI_FUNC auml24xxAttrAveragingCount_ReadCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32*        value
)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32     channelMeasMode;
	ViAttr      targetAttributeId;

	/* Get the corresponding internal attributes here */  
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &channelMeasMode));
		switch (channelMeasMode) 
		{
		  case AUML24XX_VAL_MEAS_MODE_CW:
			{
				targetAttributeId = AUML24XX_ATTR_INSTR_CW_AVERAGING_NUMBER; 
			}
			 break;
	      case AUML24XX_VAL_MEAS_MODE_PMOD:
			 {
			    targetAttributeId = AUML24XX_ATTR_INSTR_PMOD_AVERAGING_NUMBER; 
			 }
			break;

		default:
			break;
	     }

	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, targetAttributeId, 0, value));  	

Error:
	return error;
}

/*- AUML24XX_ATTR_AVERAGING_COUNT -*/  
static ViStatus _VI_FUNC auml24xxAttrAveragingCount_WriteCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32         value
)
{	
	ViStatus	error = VI_SUCCESS;
	ViInt32     channelMeasMode;
	ViAttr      targetAttributeId;

	/* Get the corresponding internal attributes here */  
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &channelMeasMode));
		switch (channelMeasMode) 
		{
		  case AUML24XX_VAL_MEAS_MODE_CW:
			{
				targetAttributeId = AUML24XX_ATTR_INSTR_CW_AVERAGING_NUMBER; 
			}
			 break;
	      case AUML24XX_VAL_MEAS_MODE_PMOD:
			 {
			    targetAttributeId = AUML24XX_ATTR_INSTR_PMOD_AVERAGING_NUMBER; 
			 }
			break;

		default:
			break;
	     }

	checkErr (Ivi_SetAttributeViInt32 (vi, channelName, targetAttributeId, 0, value));  	

Error:
	return error;
}

/*- AUML24XX_ATTR_REF_OSCILLATOR_FREQUENCY -*/
static ViStatus _VI_FUNC auml24xxAttrRefOscillatorFrequency_ReadCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViReal64       *value
)
{
	ViStatus			error = VI_SUCCESS;
	ViString 			command;
	ViString            repCommand;
	IviRangeTablePtr 	table;
	ViInt32             frequency;
	ViChar	rdBuffer[BUFFER_SIZE]; 
	viCheckErr( auml24xx_GetStringFromTable (auml24xxEnum_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxEnum_commands, attributeId, &repCommand));
	checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));
	viCheckErr( auml24xx_ViPrintf (io, "%s? %s", command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^ ] %s", rdBuffer));
	viCheckErr( auml24xx_GetValueFromCmd(rdBuffer, table, &frequency));
	*value=(ViReal64)frequency;
Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrRefOscillatorFrequency_WriteCallback(
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViReal64        value
)
{
	ViStatus			error = VI_SUCCESS;
	ViString 			command,
						commandValue;
	ViString            repCommand;
	IviRangeTablePtr 	table;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxEnum_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxEnum_commands, attributeId, &repCommand));

	checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));
	viCheckErr( auml24xx_GetCmdFromIntValue((ViInt32)value, table, &commandValue));
	viCheckErr( auml24xx_ViPrintf (io, "%s %s%s%s", command, VALID_CHANNELNAME (channelName), repCommand, commandValue));
Error:
	return error;
}

/*- AUML24XX_ATTR_CAPTURE_TIME -*/
static ViStatus _VI_FUNC auml24xxAttrCaptureTime_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32     channelMeasMode;
	ViAttr      targetAttributeId;
	/* Get the corresponding internal attributes here */  
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &channelMeasMode));
		switch (channelMeasMode) 
		{
		  case AUML24XX_VAL_MEAS_MODE_CW:
			{
				targetAttributeId = AUML24XX_ATTR_INSTR_CW_TRIGGER_CAPTURE_TIME; 
			}
			 break;
	      case AUML24XX_VAL_MEAS_MODE_PMOD:
			 {
			    targetAttributeId = AUML24XX_ATTR_INSTR_PMOD_TRIGGER_CAPTURE_TIME; 
			 }
			break;

		default:
			break;
	     }

	checkErr (Ivi_GetAttributeViReal64 (vi, channelName, targetAttributeId, 0, value));  	

Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrCaptureTime_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32     channelMeasMode;
	ViAttr      targetAttributeId;
	/* Get the corresponding internal attributes here */  
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &channelMeasMode));
		switch (channelMeasMode) 
		{
		  case AUML24XX_VAL_MEAS_MODE_CW:
			{
				targetAttributeId = AUML24XX_ATTR_INSTR_CW_TRIGGER_CAPTURE_TIME; 
			}
			 break;
	      case AUML24XX_VAL_MEAS_MODE_PMOD:
			 {
			    targetAttributeId = AUML24XX_ATTR_INSTR_PMOD_TRIGGER_CAPTURE_TIME; 
			 }
			break;

		default:
			break;
	     }

	checkErr (Ivi_SetAttributeViReal64 (vi, channelName, targetAttributeId, 0, value));  	

Error:
	return error;
}

/*- AUML24XX_ATTR_TRIGGER_ARMING_MODE -*/
static ViStatus _VI_FUNC auml24xxAttrTriggerArmingMode_ReadCallback (ViSession vi,
                                                                     ViSession io,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32     channelMeasMode;
	ViAttr      targetAttributeId;
	/* Get the corresponding internal attributes here */  
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &channelMeasMode));
		switch (channelMeasMode) 
		{
		  case AUML24XX_VAL_MEAS_MODE_CW:
			{
				targetAttributeId = AUML24XX_ATTR_INSTR_CW_TRIGGER_ARMING_MODE; 
			}
			 break;
	      case AUML24XX_VAL_MEAS_MODE_PMOD:
			 {
			    targetAttributeId = AUML24XX_ATTR_INSTR_PMOD_TRIGGER_ARMING_MODE; 
			 }
			break;

		default:
			break;
	     }

	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, targetAttributeId, 0, value));  	

Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrTriggerArmingMode_WriteCallback (ViSession vi,
                                                                      ViSession io,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32     channelMeasMode;
	ViAttr      targetAttributeId;
	/* Get the corresponding internal attributes here */  
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &channelMeasMode));
		switch (channelMeasMode) 
		{
		  case AUML24XX_VAL_MEAS_MODE_CW:
			{
				targetAttributeId = AUML24XX_ATTR_INSTR_CW_TRIGGER_ARMING_MODE; 
			}
			 break;
	      case AUML24XX_VAL_MEAS_MODE_PMOD:
			 {
			    targetAttributeId = AUML24XX_ATTR_INSTR_PMOD_TRIGGER_ARMING_MODE; 
			 }
			break;

		default:
			break;
	     }

	checkErr (Ivi_SetAttributeViInt32 (vi, channelName, targetAttributeId, 0, value));  	

Error:
	return error;
}

/*- AUML24XX_ATTR_TRIGGER_DELAY_TIME -*/
static ViStatus _VI_FUNC auml24xxAttrTriggerDelayTime_ReadCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32     channelMeasMode;
	ViAttr      targetAttributeId;
	/* Get the corresponding internal attributes here */  
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &channelMeasMode));
		switch (channelMeasMode) 
		{
		  case AUML24XX_VAL_MEAS_MODE_CW:
			{
				targetAttributeId = AUML24XX_ATTR_INSTR_CW_TRIGGER_DELAY_TIME; 
			}
			 break;
	      case AUML24XX_VAL_MEAS_MODE_PMOD:
			 {
			    targetAttributeId = AUML24XX_ATTR_INSTR_PMOD_TRIGGER_DELAY_TIME; 
			 }
			break;

		default:
			break;
	     }

	checkErr (Ivi_GetAttributeViReal64 (vi, channelName, targetAttributeId, 0, value));  	

Error:
	return error;
}

/*- AUML24XX_ATTR_TRIGGER_DELAY_TIME -*/
static ViStatus _VI_FUNC auml24xxAttrTriggerDelayTime_WriteCallback (ViSession vi,
                                                                     ViSession io,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32     channelMeasMode;
	ViAttr      targetAttributeId;

	/* Get the corresponding internal attributes here */  
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &channelMeasMode));
		switch (channelMeasMode) 
		{
		  case AUML24XX_VAL_MEAS_MODE_CW:
			{
				targetAttributeId = AUML24XX_ATTR_INSTR_CW_TRIGGER_DELAY_TIME; 
			}
			 break;
	      case AUML24XX_VAL_MEAS_MODE_PMOD:
			 {
			    targetAttributeId = AUML24XX_ATTR_INSTR_PMOD_TRIGGER_DELAY_TIME; 
			 }
			break;

		default:
			break;
	     }

	checkErr (Ivi_SetAttributeViReal64 (vi, channelName, targetAttributeId, 0, value));  	

Error:
	return error;
}

/*- AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_ADJUST -*/
static ViStatus _VI_FUNC auml24xxAttrSensorCalibrationFactorAdjust_ReadCallback (ViSession vi,
                                                                                 ViSession io,
                                                                                 ViConstString channelName,
                                                                                 ViAttr attributeId,
                                                                                 ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s",command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));
Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrSensorCalibrationFactorAdjust_WriteCallback (ViSession vi,
                                                                                  ViSession io,
                                                                                  ViConstString channelName,
                                                                                  ViAttr attributeId,
                                                                                  ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;
	ViInt32     factorUnits;

	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_UNITS, 0, &factorUnits));

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));
	if(factorUnits==AUML24XX_VAL_CAL_FACTOR_UNIT_PCT)
		{
		  viCheckErr( auml24xx_ViPrintf (io, "%s %s,PCT%s%Le", command, VALID_CHANNELNAME (channelName), repCommand, value));
		}
	if(factorUnits==AUML24XX_VAL_CAL_FACTOR_UNIT_DB)
		{
		  viCheckErr( auml24xx_ViPrintf (io, "%s %s,DB%s%Le", command, VALID_CHANNELNAME (channelName), repCommand, value));
		}
Error:
	return error;
}

/*- AUML24XX_ATTR_SENSOR_CALIBRATION_MANUAL_FACTOR -*/
static ViStatus _VI_FUNC auml24xxAttrSensorCalibrationManualFactor_ReadCallback (ViSession vi,
                                                                                 ViSession io,
                                                                                 ViConstString channelName,
                                                                                 ViAttr attributeId,
                                                                                 ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));
	viCheckErr( auml24xx_ViPrintf (io, "%s? %s",command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));
Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrSensorCalibrationManualFactor_WriteCallback (ViSession vi,
                                                                                  ViSession io,
                                                                                  ViConstString channelName,
                                                                                  ViAttr attributeId,
                                                                                  ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;
	ViInt32     factorUnits;

	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_UNITS, 0, &factorUnits));

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));

	if(factorUnits==AUML24XX_VAL_CAL_FACTOR_UNIT_PCT)
		{
		  viCheckErr( auml24xx_ViPrintf (io, "%s %s,PCT%s%Le", command, VALID_CHANNELNAME (channelName), repCommand, value));
		}
 	if(factorUnits==AUML24XX_VAL_CAL_FACTOR_UNIT_DB)
		{
		  viCheckErr( auml24xx_ViPrintf (io, "%s %s,DB%s%Le", command, VALID_CHANNELNAME (channelName), repCommand, value));
		}
Error:
	return error;
}

/*- AUML24XX_ATTR_FENCE_ENABLED -*/
static ViStatus _VI_FUNC auml24xxAttrFenceEnabled_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViBoolean *value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViInt32     configurationNumber;
    ViChar	rdBuffer[BUFFER_SIZE];   
	viCheckErr( auml24xx_GetStringFromTable (auml24xxViBoolean_commands, attributeId, &command)); 
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_GATE_PATTERN_CONFIGURATION_NUMBER, 0, &configurationNumber));   

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,%ld", command, VALID_CHANNELNAME (channelName),configurationNumber));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%s", rdBuffer));
	if (!strcmp(rdBuffer,"OFF"))
		{
		   *value=VI_FALSE;
		}
	else if(!strcmp(rdBuffer,"ON"))
		{
		   *value=VI_TRUE;
		}
	else
		{
			viCheckErr (IVI_ERROR_INVALID_VALUE);
		}

Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrFenceEnabled_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViBoolean value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;
	ViInt32     configurationNumber;

	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_GATE_PATTERN_CONFIGURATION_NUMBER, 0, &configurationNumber));   
	viCheckErr( auml24xx_GetStringFromTable (auml24xxViBoolean_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViBoolean_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s %s,%ld%s%s", command, VALID_CHANNELNAME (channelName), configurationNumber,repCommand,  
			                  value ? "ON" : "OFF"));

Error:
	return error;
}

/*- AUML24XX_ATTR_FENCE_START_TIME -*/
static ViStatus _VI_FUNC auml24xxAttrFenceStartTime_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;
	ViInt32     configurationNumber;
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_GATE_PATTERN_CONFIGURATION_NUMBER, 0, &configurationNumber));   

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,%ld",command, VALID_CHANNELNAME (channelName),configurationNumber));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));
Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrFenceStartTime_WriteCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;
	ViInt32     configurationNumber;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_GATE_PATTERN_CONFIGURATION_NUMBER, 0, &configurationNumber));   
	viCheckErr( auml24xx_ViPrintf (io, "%s %s,%ld%s%Le", command, VALID_CHANNELNAME (channelName), configurationNumber,repCommand, value));

Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrFenceStartTime_CheckCallback (ViSession vi,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 value)
{
	ViReal64    gateStart;
	ViReal64    gateStop;
	ViStatus	error = VI_SUCCESS;

	checkErr (Ivi_DefaultCheckCallbackViReal64 (vi, channelName, attributeId, value));
	checkErr (Ivi_GetAttributeViReal64 (vi, channelName, AUML24XX_ATTR_GATE_PATTERN_START_TIME, 0, &gateStart));
	checkErr (Ivi_GetAttributeViReal64 (vi, channelName, AUML24XX_ATTR_GATE_PATTERN_STOP_TIME, 0, &gateStop));
	if( value < gateStart|| value > gateStop)
	{
		viCheckErr (IVI_ERROR_INVALID_CONFIGURATION); 

	}

Error:
	return error;
}

/*- AUML24XX_ATTR_GATE_PATTERN_ENABLED -*/ 
static ViStatus _VI_FUNC auml24xxAttrGatePatternEnabled_ReadCallback (ViSession vi,
                                                                      ViSession io,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViBoolean *value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViInt32     configurationNumber;
    ViChar	rdBuffer[BUFFER_SIZE]; 
	
	viCheckErr( auml24xx_GetStringFromTable (auml24xxViBoolean_commands, attributeId, &command)); 
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_GATE_PATTERN_CONFIGURATION_NUMBER, 0, &configurationNumber));   

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,%ld", command, VALID_CHANNELNAME (channelName),configurationNumber));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%s", rdBuffer));
	if (!strcmp(rdBuffer,"OFF"))
		{
		   *value=VI_FALSE;
		}
	else if(!strcmp(rdBuffer,"ON"))
		{
		   *value=VI_TRUE;
		}
	else
		{
			viCheckErr (IVI_ERROR_INVALID_VALUE);
		}
Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrGatePatternEnabled_WriteCallback (ViSession vi,
                                                                       ViSession io,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViBoolean value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;
	ViInt32     configurationNumber;

	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_GATE_PATTERN_CONFIGURATION_NUMBER, 0, &configurationNumber));   
	viCheckErr( auml24xx_GetStringFromTable (auml24xxViBoolean_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViBoolean_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s %s,%ld%s%s", command, VALID_CHANNELNAME (channelName), configurationNumber,repCommand,  
			                  value ? "ON" : "OFF"));

Error:
	return error;
}

/*- AUML24XX_ATTR_GATE_PATTERN_START_TIME -*/ 
static ViStatus _VI_FUNC auml24xxAttrGatePatternStartTime_ReadCallback (ViSession vi,
                                                                        ViSession io,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;
	ViInt32     configurationNumber;
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_GATE_PATTERN_CONFIGURATION_NUMBER, 0, &configurationNumber));   

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));
	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,%ld",command, VALID_CHANNELNAME (channelName),configurationNumber));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));
Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrGatePatternStartTime_WriteCallback (ViSession vi,
                                                                         ViSession io,
                                                                         ViConstString channelName,
                                                                         ViAttr attributeId,
                                                                         ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;
	ViInt32     configurationNumber;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_GATE_PATTERN_CONFIGURATION_NUMBER, 0, &configurationNumber));   
	viCheckErr( auml24xx_ViPrintf (io, "%s %s,%ld%s%Le", command, VALID_CHANNELNAME (channelName), configurationNumber,repCommand, value));

Error:
	return error;
}

/*- AUML24XX_ATTR_GATE_PATTERN_STOP_TIME -*/ 
static ViStatus _VI_FUNC auml24xxAttrGatePatternStopTime_ReadCallback (ViSession vi,
                                                                       ViSession io,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;
	ViInt32     configurationNumber;
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_GATE_PATTERN_CONFIGURATION_NUMBER, 0, &configurationNumber));   

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,%ld",command, VALID_CHANNELNAME (channelName),configurationNumber));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));
Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrGatePatternStopTime_WriteCallback (ViSession vi,
                                                                        ViSession io,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;
	ViInt32     configurationNumber;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_GATE_PATTERN_CONFIGURATION_NUMBER, 0, &configurationNumber));   
	viCheckErr( auml24xx_ViPrintf (io, "%s %s,%ld%s%Le", command, VALID_CHANNELNAME (channelName), configurationNumber,repCommand, value));

Error:
	return error;
}

/*- AUML24XX_ATTR_MARKER_POSITION -*/ 
static ViStatus _VI_FUNC auml24xxAttrMarkerPosition_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;
	ViInt32     configurationNumber;
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_MARKER_CONFIGURATION_NUMBER, 0, &configurationNumber));   

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,%ld",command, VALID_CHANNELNAME (channelName),configurationNumber));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));

Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrMarkerPosition_WriteCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;
	ViInt32     configurationNumber;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_MARKER_CONFIGURATION_NUMBER, 0, &configurationNumber));   
	viCheckErr( auml24xx_ViPrintf (io, "%s %s,%ld%s%Le", command, VALID_CHANNELNAME (channelName), configurationNumber,repCommand, value));

Error:
	return error;
}

/*- AUML24XX_ATTR_MARKER_ENABLED -*/ 
static ViStatus _VI_FUNC auml24xxAttrMarkerEnabled_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViBoolean *value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViInt32     configurationNumber;
    ViChar	rdBuffer[BUFFER_SIZE]; 
	viCheckErr( auml24xx_GetStringFromTable (auml24xxViBoolean_commands, attributeId, &command)); 
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_MARKER_CONFIGURATION_NUMBER, 0, &configurationNumber));   

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,%ld", command, VALID_CHANNELNAME (channelName),configurationNumber));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%s", rdBuffer));
	if (!strcmp(rdBuffer,"OFF"))
		{
		   *value=VI_FALSE;
		}
	else if(!strcmp(rdBuffer,"ON"))
		{
		   *value=VI_TRUE;
		}
	else
		{
			viCheckErr (IVI_ERROR_INVALID_VALUE);
		}
Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrMarkerEnabled_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViBoolean value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;
	ViInt32     configurationNumber;

	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_MARKER_CONFIGURATION_NUMBER, 0, &configurationNumber));   
	viCheckErr( auml24xx_GetStringFromTable (auml24xxViBoolean_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViBoolean_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s %s,%ld%s%s", command, VALID_CHANNELNAME (channelName), configurationNumber,repCommand,  
			                  value ? "ON" : "OFF"));

Error:
	return error;
}

/*- AUML24XX_ATTR_BNC_DBW_ANALOG_OUTPUT_STOP-*/ 
static ViStatus _VI_FUNC auml24xxAttrBncDbwAnalogOutputStop_ReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));
	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,DBW",command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));

Error:
	return error;
}

/*- AUML24XX_ATTR_INSTR_CW_TRIGGER_CAPTURE_TIME -*/ 
static ViStatus _VI_FUNC auml24xxAttrInstrCWTriggerCaptureTime_ReadCallback (ViSession vi,
                                                                             ViSession io,
                                                                             ViConstString channelName,
                                                                             ViAttr attributeId,
                                                                             ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,CW",command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));
Error:
	return error;
}

/*- AUML24XX_ATTR_INSTR_CW_TRIGGER_DELAY_TIME -*/ 
static ViStatus _VI_FUNC auml24xxAttrInstrCWTriggerDelayTime_ReadCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,CW",command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));
Error:
	return error;
}

/*- AUML24XX_ATTR_INSTR_PMOD_TRIGGER_CAPTURE_TIME -*/ 
static ViStatus _VI_FUNC auml24xxAttrInstrPMODTriggerCaptureTime_ReadCallback (ViSession vi,
                                                                               ViSession io,
                                                                               ViConstString channelName,
                                                                               ViAttr attributeId,
                                                                               ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,PMOD",command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));
Error:
	return error;
}

/*- AUML24XX_ATTR_INSTR_PMOD_TRIGGER_DELAY_TIME -*/ 
static ViStatus _VI_FUNC auml24xxAttrInstrPMODTriggerDelayTime_ReadCallback (ViSession vi,
                                                                             ViSession io,
                                                                             ViConstString channelName,
                                                                             ViAttr attributeId,
                                                                             ViReal64 *value)
{	
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,PMOD",command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));
Error:
	return error;
}

/*- AUML24XX_ATTR_SAMPLE_RATE -*/ 
static ViStatus _VI_FUNC auml24xxAttrSampleRate_RangeTableCallback (ViSession vi,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    IviRangeTablePtr *rangeTablePtr)
{
	ViStatus	error = VI_SUCCESS;
	IviRangeTablePtr	tblPtr = VI_NULL;
	ViInt32     modelType;

	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_INSTR_MODEL_TYPE, 0, &modelType));
	switch (modelType) {
		case AUML24XX_VAL_MODEL_ML2487A:
		case AUML24XX_VAL_MODEL_ML2488A:
			tblPtr = &attr248XSampleRateRangeTable;
			break;
		case AUML24XX_VAL_MODEL_ML2487B:
		case AUML24XX_VAL_MODEL_ML2488B:
		case AUML24XX_VAL_MODEL_ML2495A:
		case AUML24XX_VAL_MODEL_ML2496A:	
			tblPtr = &attr249XSampleRateRangeTable;
			break;
		default:
			viCheckErr (IVI_ERROR_INVALID_VALUE);
			break;
	}

Error:
	*rangeTablePtr = tblPtr;
	return error;
}

/*- AUML24XX_ATTR_CHANNEL_INPUT_CONFIG-*/ 
static ViStatus _VI_FUNC auml24xxAttrChannelInputConfig_RangeTableCallback (ViSession vi,
                                                                            ViConstString channelName,
                                                                            ViAttr attributeId,
                                                                            IviRangeTablePtr *rangeTablePtr)
{
	ViStatus	error = VI_SUCCESS;
	IviRangeTablePtr	tblPtr = VI_NULL;

	ViInt32     modelType;
	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_INSTR_MODEL_TYPE, 0, &modelType));
	switch (modelType) 
	{
		case AUML24XX_VAL_MODEL_ML2487A:
		case AUML24XX_VAL_MODEL_ML2495A:
		case AUML24XX_VAL_MODEL_ML2487B:	
			tblPtr = &attrSingleChannelConfigRangeTable;
			break;
		case AUML24XX_VAL_MODEL_ML2488A:
		case AUML24XX_VAL_MODEL_ML2496A:
		case AUML24XX_VAL_MODEL_ML2488B:	
			tblPtr = &attrDualChannelConfigRangeTable;
			break;

		default:
			viCheckErr (IVI_ERROR_INVALID_VALUE);
			break;
	}

Error:
	*rangeTablePtr = tblPtr;
	return error;
}

/*- AUML24XX_ATTR_TRIGGER_HOLDOFF_TIME-*/ 
static ViStatus _VI_FUNC auml24xxAttrTriggerHoldoffTime_CheckCallback (ViSession vi,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32     measMode;
	ViInt32     modelType;

	checkErr (Ivi_DefaultCheckCallbackViReal64 (vi, channelName, attributeId, value));
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &measMode));  
	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_INSTR_MODEL_TYPE, 0, &modelType)); 

	if ((measMode!=AUML24XX_VAL_MEAS_MODE_PMOD) || 
		 (modelType==AUML24XX_VAL_MODEL_ML2495A) || 
		 (modelType==AUML24XX_VAL_MODEL_ML2496A) ||
		 (modelType==AUML24XX_VAL_MODEL_ML2487B) ||
		 (modelType==AUML24XX_VAL_MODEL_ML2488B) 
		 )

		viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);     

Error:
	return error;
}

/*- AUML24XX_ATTR_TRIGGER_FRAME_ARMING_LEVEL-*/ 
static ViStatus _VI_FUNC auml24xxAttrTriggerFrameArmingLevel_CheckCallback (ViSession vi,
                                                                            ViConstString channelName,
                                                                            ViAttr attributeId,
                                                                            ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32     measMode;

	checkErr (Ivi_DefaultCheckCallbackViReal64 (vi, channelName, attributeId, value));
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &measMode));  

	if (measMode!=AUML24XX_VAL_MEAS_MODE_PMOD)

		viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);     

Error:
	return error;
}

/*- AUML24XX_ATTR_TRIGGER_HOLDOFF_ENABLED-*/ 
static ViStatus _VI_FUNC auml24xxAttrTriggerHoldoffEnabled_CheckCallback (ViSession vi,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViBoolean value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32     measMode;
	ViInt32     modelType;

	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &measMode));  
	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_INSTR_MODEL_TYPE, 0, &modelType)); 

	if ((measMode!=AUML24XX_VAL_MEAS_MODE_PMOD) || 
		 (modelType==AUML24XX_VAL_MODEL_ML2495A) || 
		 (modelType==AUML24XX_VAL_MODEL_ML2496A) ||
		 (modelType==AUML24XX_VAL_MODEL_ML2487B) ||
		 (modelType==AUML24XX_VAL_MODEL_ML2488B) 
		 )

		viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);     

Error:
	return error;
}

/*- AUML24XX_ATTR_TRIGGER_LINKING_ENABLED-*/ 
static ViStatus _VI_FUNC auml24xxAttrTriggerLinkingEnabled_CheckCallback (ViSession vi,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViBoolean value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32     modelType;    
	ViInt32     chan1MeasMode;
	ViInt32     chan2MeasMode;

	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_INSTR_MODEL_TYPE, 0, &modelType));
	checkErr (Ivi_GetAttributeViInt32 (vi, "1", AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &chan1MeasMode));
	checkErr (Ivi_GetAttributeViInt32 (vi, "2", AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &chan2MeasMode));

	switch (modelType) 
	{
		case AUML24XX_VAL_MODEL_ML2487A:
		case AUML24XX_VAL_MODEL_ML2495A:
		case AUML24XX_VAL_MODEL_ML2487B:
			viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);
			break;
		case AUML24XX_VAL_MODEL_ML2488A:
		case AUML24XX_VAL_MODEL_ML2496A:
		case AUML24XX_VAL_MODEL_ML2488B:
	         if (chan1MeasMode!=chan2MeasMode)
		     	viCheckErr (IVI_ERROR_INVALID_CONFIGURATION); 
			break;

		default:
			viCheckErr (IVI_ERROR_INVALID_VALUE);
			break;
	}

Error:												
	return error;
}

/*- AUML24XX_ATTR_TRIGGER_SOURCE-*/ 
static ViStatus _VI_FUNC auml24xxAttrTriggerSource_CheckCallback (ViSession vi,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32     modelType;

	checkErr (Ivi_DefaultCheckCallbackViInt32 (vi, channelName, attributeId, value));
	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_INSTR_MODEL_TYPE, 0, &modelType));

	switch (value) 
	{

        case AUML24XX_VAL_EXTERNAL:
		     break;
        case AUML24XX_VAL_INTA:
		     break;
		case AUML24XX_VAL_INTB:

				if(modelType == AUML24XX_VAL_MODEL_ML2487A|| modelType == AUML24XX_VAL_MODEL_ML2495A || modelType == AUML24XX_VAL_MODEL_ML2487B)
				{
					viCheckErr (IVI_ERROR_INVALID_CONFIGURATION); 
				}
			break;
	    case AUML24XX_VAL_CONT:
			 break;

		default:
			viCheckErr (IVI_ERROR_INVALID_VALUE);
	         break;
	}

Error:
	return error;
}											  

/*- AUML24XX_ATTR_SENSOR_CALIBRATION_MANUAL_FACTOR -*/ 
static ViStatus _VI_FUNC auml24xxAttrSensorCalibrationManualFactor_RangeTableCallback (ViSession vi,
                                                                                       ViConstString channelName,
                                                                                       ViAttr attributeId,
                                                                                       IviRangeTablePtr *rangeTablePtr)
{
	ViStatus	error = VI_SUCCESS;
	IviRangeTablePtr	tblPtr = VI_NULL;
	ViInt32     factorUnits;

	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_UNITS, 0, &factorUnits));
	switch (factorUnits) {
		case AUML24XX_VAL_CAL_FACTOR_UNIT_PCT:
			tblPtr = &attrSensorCalibrationFactorManualValuePCTRangeTable;
			break;
		case AUML24XX_VAL_CAL_FACTOR_UNIT_DB:
			tblPtr = &attrSensorCalibrationFactorManualValueDBRangeTable;
			break;
		default:
			viCheckErr (IVI_ERROR_INVALID_VALUE);
			break;
	}

Error:
	*rangeTablePtr = tblPtr;
	return error;
}

/*- AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_ADJUST -*/ 
static ViStatus _VI_FUNC auml24xxAttrSensorCalibrationFactorAdjust_RangeTableCallback (ViSession vi,
                                                                                       ViConstString channelName,
                                                                                       ViAttr attributeId,
                                                                                       IviRangeTablePtr *rangeTablePtr)
{
	ViStatus	error = VI_SUCCESS;
	IviRangeTablePtr	tblPtr = VI_NULL;
	ViInt32     factorUnits;

	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_UNITS, 0, &factorUnits));
	switch (factorUnits) {
		case AUML24XX_VAL_CAL_FACTOR_UNIT_PCT:
			tblPtr = &attrSensorCalibrationFactorAdjustPCTRangeTable;
			break;
		case AUML24XX_VAL_CAL_FACTOR_UNIT_DB:
			tblPtr = &attrSensorCalibrationFactorAdjustDBRangeTable;
			break;
		default:
			viCheckErr (IVI_ERROR_INVALID_VALUE);
			break;
	}

Error:
	*rangeTablePtr = tblPtr;
	return error;
}

/*- AUML24XX_ATTR_LIMIT_CHECKING_TYPE -*/ 
static ViStatus _VI_FUNC auml24xxAttrLimitCheckingType_CheckCallback (ViSession vi,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32     measMode;

	checkErr (Ivi_DefaultCheckCallbackViInt32 (vi, channelName, attributeId, value));
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &measMode));  

	if ( (measMode == AUML24XX_VAL_MEAS_MODE_CW) && (value == AUML24XX_VAL_LIMIT_CHECKING_COMPLEX))

		viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);     

Error:
	return error;
}

/*- AUML24XX_ATTR_POST_PROCESSING_FUNCTION_MODULE -*/ 
static ViStatus _VI_FUNC auml24xxAttrPostProcessingFunctionModule_RangeTableCallback (ViSession vi,
                                                                                      ViConstString channelName,
                                                                                      ViAttr attributeId,
                                                                                      IviRangeTablePtr *rangeTablePtr)
{
	ViStatus	error = VI_SUCCESS;
	IviRangeTablePtr	tblPtr = VI_NULL;
	ViInt32     modelType;

	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_INSTR_MODEL_TYPE, 0, &modelType));
	switch (modelType) {
		case AUML24XX_VAL_MODEL_ML2487A:
		case AUML24XX_VAL_MODEL_ML2487B:
		case AUML24XX_VAL_MODEL_ML2495A:
			tblPtr = &attrSingleChannelPostProcessingFunctionModuleRangeTable;
			break;
		case AUML24XX_VAL_MODEL_ML2488A:
		case AUML24XX_VAL_MODEL_ML2488B:	
		case AUML24XX_VAL_MODEL_ML2496A:
			tblPtr = &attrDualChannelPostProcessingFunctionModuleRangeTable;
			break;
		default:
			viCheckErr (IVI_ERROR_INVALID_VALUE);
			break;
	}

Error:
	*rangeTablePtr = tblPtr;
	return error;
}

/*- AUML24XX_ATTR_BNC_ANALOG_OUTPUT_START_VOLTAGE_SCALE -*/    
static ViStatus _VI_FUNC auml24xxAttrBncAnalogOutputStartVoltageScale_CheckCallback (ViSession vi,
                                                                                     ViConstString channelName,
                                                                                     ViAttr attributeId,
                                                                                     ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64     stopVoltage;

	checkErr (Ivi_DefaultCheckCallbackViReal64 (vi, channelName, attributeId, value));
	checkErr (Ivi_GetAttributeViReal64 (vi, channelName, AUML24XX_ATTR_BNC_ANALOG_OUTPUT_STOP_VOLTAGE_SCALE, 0, &stopVoltage));  

	if (value > stopVoltage)
		viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);     
Error:
	return error;
}

/*- AUML24XX_ATTR_BNC_ANALOG_OUTPUT_STOP_VOLTAGE_SCALE -*/    
static ViStatus _VI_FUNC auml24xxAttrBncAnalogOutputStopVoltageScale_CheckCallback (ViSession vi,
                                                                                    ViConstString channelName,
                                                                                    ViAttr attributeId,
                                                                                    ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64     startVoltage;

	checkErr (Ivi_DefaultCheckCallbackViReal64 (vi, channelName, attributeId, value));
	checkErr (Ivi_GetAttributeViReal64 (vi, channelName, AUML24XX_ATTR_BNC_ANALOG_OUTPUT_START_VOLTAGE_SCALE, 0, &startVoltage));  

	if (value < startVoltage)
		viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);     

Error:
	return error;
}

/*- AUML24XX_ATTR_FENCE_STOP_TIME -*/    
static ViStatus _VI_FUNC auml24xxAttrFenceStopTime_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;
	ViInt32     configurationNumber;
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_GATE_PATTERN_CONFIGURATION_NUMBER, 0, &configurationNumber));   

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,%ld",command, VALID_CHANNELNAME (channelName),configurationNumber));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));
Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrFenceStopTime_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;
	ViInt32     configurationNumber;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_GATE_PATTERN_CONFIGURATION_NUMBER, 0, &configurationNumber));   
	viCheckErr( auml24xx_ViPrintf (io, "%s %s,%ld%s%Le", command, VALID_CHANNELNAME (channelName), configurationNumber,repCommand, value));

Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrFenceStopTime_CheckCallback (ViSession vi,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value)
{
	ViReal64    gateStart;
	ViReal64    gateStop;
	ViStatus	error = VI_SUCCESS;

	checkErr (Ivi_DefaultCheckCallbackViReal64 (vi, channelName, attributeId, value));
	checkErr (Ivi_GetAttributeViReal64 (vi, channelName, AUML24XX_ATTR_GATE_PATTERN_START_TIME, 0, &gateStart));
	checkErr (Ivi_GetAttributeViReal64 (vi, channelName, AUML24XX_ATTR_GATE_PATTERN_STOP_TIME, 0, &gateStop));
	if( value < gateStart|| value > gateStop)
	{
		viCheckErr (IVI_ERROR_INVALID_CONFIGURATION); 

	}

Error:
	return error;
}

/*- AUML24XX_ATTR_MARKER_DELTA_POSITION -*/    
static ViStatus _VI_FUNC auml24xxAttrMarkerDeltaPosition_CheckCallback (ViSession vi,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViBoolean     deltaMarkerEnabled;

	checkErr (Ivi_DefaultCheckCallbackViReal64 (vi, channelName, attributeId, value));
	checkErr (Ivi_GetAttributeViBoolean (vi, channelName, AUML24XX_ATTR_MARKER_DELTA_ENABLED, 0, &deltaMarkerEnabled));   

	if(!deltaMarkerEnabled)
	    viCheckErr (IVI_ERROR_INVALID_CONFIGURATION); 

Error:
	return error;
}

/*- AUML24XX_ATTR_MARKER_POSITION -*/    
static ViStatus _VI_FUNC auml24xxAttrMarkerPosition_CheckCallback (ViSession vi,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViBoolean     markerEnabled;

	checkErr (Ivi_DefaultCheckCallbackViReal64 (vi, channelName, attributeId, value));
	checkErr (Ivi_GetAttributeViBoolean (vi, channelName, AUML24XX_ATTR_MARKER_ENABLED, 0, &markerEnabled));   

	if(!markerEnabled)
	    viCheckErr (IVI_ERROR_INVALID_CONFIGURATION); 

Error:
	return error;
}

/*- AUML24XX_ATTR_SAMPLE_RATE -*/    
static ViStatus _VI_FUNC auml24xxAttrSampleRate_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 *value){
	ViStatus			error = VI_SUCCESS;
	ViString 			command;
	ViString            repCommand;
	IviRangeTablePtr 	table;
 	ViChar	autoMode[BUFFER_SIZE];         
 	ViChar  sampleRate[BUFFER_SIZE];       	
	viCheckErr( auml24xx_GetStringFromTable (auml24xxEnum_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxEnum_commands, attributeId, &repCommand));

	checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));
 
	viCheckErr( auml24xx_ViPrintf (io, "%s? %s", command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%[^,],%s,%s", autoMode, sampleRate));
	if (!strcmp(autoMode,"AUTO"))
		{
			*value=AUML24XX_VAL_AUTO;
		}
	else if (!strcmp(autoMode,"USER"))
		{
	 if  (!strcmp(sampleRate,"31K25"))
	      *value=AUML24XX_VAL_31K25;
	 else if (!strcmp(sampleRate,"62K5"))
	      *value=AUML24XX_VAL_62K5;
	 else if (!strcmp(sampleRate,"125K"))
	      *value=AUML24XX_VAL_125K;
	 else if (!strcmp(sampleRate,"250K"))
	      *value=AUML24XX_VAL_250K;
	 else if (!strcmp(sampleRate,"500K"))
	      *value=AUML24XX_VAL_500K;
	 else if (!strcmp(sampleRate,"1M"))
	      *value=AUML24XX_VAL_1M;
	 else if (!strcmp(sampleRate,"2M"))
	      *value=AUML24XX_VAL_2M;
	 else if (!strcmp(sampleRate,"4M"))
	      *value=AUML24XX_VAL_4M;
	 else if (!strcmp(sampleRate,"8M"))
	      *value=AUML24XX_VAL_8M;
	 else if (!strcmp(sampleRate,"16M"))
	      *value=AUML24XX_VAL_16M;
	 else if (!strcmp(sampleRate,"32M"))
	      *value=AUML24XX_VAL_32M;
	 else if (!strcmp(sampleRate,"64M"))
	      *value=AUML24XX_VAL_64M;
	 else if (!strcmp(sampleRate,"31.25M"))
	      *value=AUML24XX_VAL_31M25;
	 else if (!strcmp(sampleRate,"62.5M"))
	      *value=AUML24XX_VAL_62M5;
	 else
		 viCheckErr (IVI_ERROR_INVALID_VALUE);

	}
Error:
	return error;
}

/*- AUML24XX_ATTR_SAMPLE_RATE -*/   
static ViStatus _VI_FUNC auml24xxAttrSampleRate_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 value){
	ViStatus			error = VI_SUCCESS;
	ViString 			command,
						commandValue;
	ViString            repCommand;
	IviRangeTablePtr 	table;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxEnum_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxEnum_commands, attributeId, &repCommand));

	checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));
	viCheckErr( auml24xx_GetCmdFromIntValue(value, table, &commandValue));

	viCheckErr( auml24xx_ViPrintf (io, "%s %s%s%s", command, VALID_CHANNELNAME (channelName), repCommand, commandValue));
Error:
	return error;
}

/*- AUML24XX_ATTR_DISPlAY_MEASUREMENT_POINTS -*/   
static ViStatus _VI_FUNC auml24xxAttrDisplayMeasurementPoints_ReadCallback (ViSession vi,
                                                                             ViSession io,
                                                                             ViConstString channelName,
                                                                             ViAttr attributeId,
                                                                             ViInt32 *value){
	ViStatus			error = VI_SUCCESS;
	ViString 			command;
	ViString            repCommand;
	IviRangeTablePtr 	table;
 	ViChar	rdBuffer[BUFFER_SIZE];  
	
	viCheckErr( auml24xx_GetStringFromTable (auml24xxEnum_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxEnum_commands, attributeId, &repCommand));

	checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));


	viCheckErr( auml24xx_ViPrintf (io, "%s? %s", command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^ ] %s", rdBuffer));

    viCheckErr( auml24xx_GetValueFromCmd(rdBuffer, table, value));
Error:
	return error;
}

/*- AUML24XX_ATTR_CW_AVERAGING_MODE -*/   
static ViStatus _VI_FUNC auml24xxAttrCWAveragingMode_ReadCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViInt32 *value)
{
	ViStatus			error = VI_SUCCESS;
	ViString 			command;
	ViString            repCommand;
	IviRangeTablePtr 	table;
 	ViChar	rdBuffer[BUFFER_SIZE]; 
   	viCheckErr( auml24xx_GetStringFromTable (auml24xxEnum_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxEnum_commands, attributeId, &repCommand));

	checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));
	viCheckErr( auml24xx_ViPrintf (io, "%s? %s", command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%[^,],%s", rdBuffer));
	viCheckErr( auml24xx_GetValueFromCmd(rdBuffer, table, value));
Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrCWAveragingMode_WriteCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViInt32 value){
	ViStatus			error = VI_SUCCESS;
	ViString 			command,
						commandValue;
	ViString            repCommand;
	IviRangeTablePtr 	table;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxEnum_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxEnum_commands, attributeId, &repCommand));

	checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));
	viCheckErr( auml24xx_GetCmdFromIntValue(value, table, &commandValue));

	viCheckErr( auml24xx_ViPrintf (io, "%s %s%s%s,", command, VALID_CHANNELNAME (channelName), repCommand, commandValue));
Error:
	return error;
}

/*- AUML24XX_ATTR_CHANNEL_INPUT_CONFIG -*/   
static ViStatus _VI_FUNC auml24xxAttrChannelInputConfig_CheckCallback (ViSession vi,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32     measMode;

	checkErr (Ivi_DefaultCheckCallbackViInt32 (vi, channelName, attributeId, value));
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &measMode));  

	if (measMode!=AUML24XX_VAL_MEAS_MODE_CW)
	{
		if (value==AUML24XX_VAL_CHAN_CONFIG_V)
		{
			viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);     
		}

	}

Error:
	return error;
}

/*- AUML24XX_ATTR_INSTR_CW_AVERAGING_ENABLED -*/ 
static ViStatus _VI_FUNC auml24xxAttrInstrCWAveragingEnabled_ReadCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViBoolean *value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
    ViChar	rdBuffer[BUFFER_SIZE];  
	
	viCheckErr( auml24xx_GetStringFromTable (auml24xxViBoolean_commands, attributeId, &command)); 

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s", command, VALID_CHANNELNAME (channelName)));

	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%[^,],%s ", rdBuffer));
	if (!strcmp(rdBuffer,"OFF"))
		{
		   *value=VI_FALSE;
		}
	else if( (!strcmp(rdBuffer,"AUTO"))||(!strcmp(rdBuffer,"RPT"))||(!strcmp(rdBuffer,"MOV")))

		{
		   *value=VI_TRUE;
		}
	else
		{
			viCheckErr (IVI_ERROR_INVALID_VALUE);
		}
Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrInstrCWAveragingEnabled_WriteCallback (ViSession vi,
                                                                            ViSession io,
                                                                            ViConstString channelName,
                                                                            ViAttr attributeId,
                                                                            ViBoolean value){
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViBoolean_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViBoolean_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s %s%s%s,", command, VALID_CHANNELNAME (channelName), repCommand, value ? "AUTO" : "OFF"));

Error:
	return error;
}

/*- AUML24XX_ATTR_TRIGGER_EXTERNAL_EDGE -*/ 
static ViStatus _VI_FUNC auml24xxAttrTriggerExternalEdge_ReadCallback (ViSession vi,
                                                                       ViSession io,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViInt32 *value){
	ViStatus			error = VI_SUCCESS;
	ViString 			command;
	ViString            repCommand;
	IviRangeTablePtr 	table;
	ViChar	rdBuffer[BUFFER_SIZE];     
    
	viCheckErr( auml24xx_GetStringFromTable (auml24xxEnum_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxEnum_commands, attributeId, &repCommand));

	checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s", command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^ ] %s", rdBuffer));
	viCheckErr( auml24xx_GetValueFromCmd(rdBuffer, table, value));
Error:
	return error;
}

/*- AUML24XX_ATTR_TRIGGER_LINKING_ENABLED -*/ 
static ViStatus _VI_FUNC auml24xxAttrTriggerLinkingEnabled_ReadCallback (ViSession vi,
                                                                         ViSession io,
                                                                         ViConstString channelName,
                                                                         ViAttr attributeId,
                                                                         ViBoolean *value){
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
     ViChar	rdBuffer[BUFFER_SIZE];   
	 
	viCheckErr( auml24xx_GetStringFromTable (auml24xxViBoolean_commands, attributeId, &command)); 
	viCheckErr( auml24xx_ViPrintf (io, "%s? %s", command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^ ] %s", rdBuffer));
	if (!strcmp(rdBuffer,"OFF"))
		{
		   *value=VI_FALSE;
		}
	else if(!strcmp(rdBuffer,"ON"))

		{
		   *value=VI_TRUE;
		}
	else
		{
			viCheckErr (IVI_ERROR_INVALID_VALUE);
		}
Error:
	return error;
}

/*- AUML24XX_ATTR_TRIGGER_FRAME_ARMING_TIME_DURATION -*/ 
static ViStatus _VI_FUNC auml24xxAttrTriggerFrameArmingTimeDuration_CheckCallback (ViSession vi,
                                                                                   ViConstString channelName,
                                                                                   ViAttr attributeId,
                                                                                   ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32     measMode;

	checkErr (Ivi_DefaultCheckCallbackViReal64 (vi, channelName, attributeId, value));
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &measMode));  

	if (measMode!=AUML24XX_VAL_MEAS_MODE_PMOD)

		viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);     

Error:
	return error;
}

/*- AUML24XX_ATTR_INSTR_CW_TRIGGER_ARMING_MODE -*/ 
static ViStatus _VI_FUNC auml24xxAttrInstrCWTriggerArmingMode_ReadCallback (ViSession vi,
                                                                            ViSession io,
                                                                            ViConstString channelName,
                                                                            ViAttr attributeId,
                                                                            ViInt32 *value){
	ViStatus			error = VI_SUCCESS;
	ViString 			command;
	ViString            repCommand;
	IviRangeTablePtr 	table;
	ViChar	rdBuffer[BUFFER_SIZE];  
	
	viCheckErr( auml24xx_GetStringFromTable (auml24xxEnum_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxEnum_commands, attributeId, &repCommand));

	checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));
	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,CW", command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%s", rdBuffer));
	viCheckErr( auml24xx_GetValueFromCmd(rdBuffer, table, value));
Error:
	return error;
}

/*- AUML24XX_ATTR_INSTR_PMOD_TRIGGER_ARMING_MODE -*/ 
static ViStatus _VI_FUNC auml24xxAttrInstrPMODTriggerArmingMode_ReadCallback (ViSession vi,
                                                                              ViSession io,
                                                                              ViConstString channelName,
                                                                              ViAttr attributeId,
                                                                              ViInt32 *value){
	ViStatus			error = VI_SUCCESS;
	ViString 			command;
	ViString            repCommand;
	IviRangeTablePtr 	table;
	ViChar	rdBuffer[BUFFER_SIZE]; 
	
	viCheckErr( auml24xx_GetStringFromTable (auml24xxEnum_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxEnum_commands, attributeId, &repCommand));

	checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,PMOD", command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%s", rdBuffer));
	viCheckErr( auml24xx_GetValueFromCmd(rdBuffer, table, value));
Error:
	return error;
}

/*- AUML24XX_ATTR_INSTR_PMOD_TRIGGER_INTERNAL_SLOPE -*/ 
static ViStatus _VI_FUNC auml24xxAttrInstrPMODTriggerInternalSlope_ReadCallback (ViSession vi,
                                                                                 ViSession io,
                                                                                 ViConstString channelName,
                                                                                 ViAttr attributeId,
                                                                                 ViInt32 *value)
{
	ViStatus			error = VI_SUCCESS;
	ViString 			command;
	ViString            repCommand;
	IviRangeTablePtr 	table;
	ViChar	rdBuffer[BUFFER_SIZE];   
	viCheckErr( auml24xx_GetStringFromTable (auml24xxEnum_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxEnum_commands, attributeId, &repCommand));

	checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));


	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,PMOD", command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%s", rdBuffer));
	viCheckErr( auml24xx_GetValueFromCmd(rdBuffer, table, value));
Error:
	return error;
}

/*- AUML24XX_ATTR_INSTR_CW_TRIGGER_INTERNAL_SLOPE -*/ 
static ViStatus _VI_FUNC auml24xxAttrInstrCWTriggerInternalSlope_ReadCallback (ViSession vi,
                                                                               ViSession io,
                                                                               ViConstString channelName,
                                                                               ViAttr attributeId,
                                                                               ViInt32 *value)
{
	ViStatus			error = VI_SUCCESS;
	ViString 			command;
	ViString            repCommand;
	IviRangeTablePtr 	table;
	ViChar	rdBuffer[BUFFER_SIZE];   
	viCheckErr( auml24xx_GetStringFromTable (auml24xxEnum_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxEnum_commands, attributeId, &repCommand));

	checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));


	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,CW", command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%s", rdBuffer));
	viCheckErr( auml24xx_GetValueFromCmd(rdBuffer, table, value));
Error:
	return error;
}

/*- AUML24XX_ATTR_INSTR_CW_TRIGGER_INTERNAL_LEVEL -*/ 
static ViStatus _VI_FUNC auml24xxAttrInstrCWTriggerInternalLevel_ReadCallback (ViSession vi,
                                                                               ViSession io,
                                                                               ViConstString channelName,
                                                                               ViAttr attributeId,
                                                                               ViReal64 *value){
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));
	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,CW",command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));
Error:
	return error;
}

/*- AUML24XX_ATTR_INSTR_CW_TRIGGER_SOURCE -*/ 
static ViStatus _VI_FUNC auml24xxAttrInstrCWTriggerSource_ReadCallback (ViSession vi,
                                                                        ViSession io,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViInt32 *value){
	ViStatus			error = VI_SUCCESS;
	ViString 			command;
	ViString            repCommand;
	IviRangeTablePtr 	table;
	ViChar	rdBuffer[BUFFER_SIZE]; 
	viCheckErr( auml24xx_GetStringFromTable (auml24xxEnum_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxEnum_commands, attributeId, &repCommand));

	checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,CW", command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%s", rdBuffer));
	viCheckErr( auml24xx_GetValueFromCmd(rdBuffer, table, value));
Error:
	return error;
}

/*- AUML24XX_ATTR_INSTR_PMOD_TRIGGER_INTERNAL_LEVEL -*/ 
static ViStatus _VI_FUNC auml24xxAttrInstrPMODTriggerInternalLevel_ReadCallback (ViSession vi,
                                                                                 ViSession io,
                                                                                 ViConstString channelName,
                                                                                 ViAttr attributeId,
                                                                                 ViReal64 *value){
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,PMOD",command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));
Error:
	return error;
}

/*- AUML24XX_ATTR_INSTR_PMOD_TRIGGER_SOURCE -*/ 
static ViStatus _VI_FUNC auml24xxAttrInstrPMODTriggerSource_ReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViInt32 *value){
	ViStatus			error = VI_SUCCESS;
	ViString 			command;
	ViString            repCommand;
	IviRangeTablePtr 	table;
 	ViChar	rdBuffer[BUFFER_SIZE];  
	viCheckErr( auml24xx_GetStringFromTable (auml24xxEnum_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxEnum_commands, attributeId, &repCommand));

	checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));
	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,PMOD", command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%s", rdBuffer));
	viCheckErr( auml24xx_GetValueFromCmd(rdBuffer, table, value));
Error:
	return error;
}

/*- AUML24XX_ATTR_INSTR_CW_INTERNAL_TRIGGER_EVENT_SOURCE -*/ 
static ViStatus _VI_FUNC auml24xxAttrInstrInternalTriggerEventSource_CheckCallback (ViSession vi,
                                                                                    ViConstString channelName,
                                                                                    ViAttr attributeId,
                                                                                    ViConstString value){
	ViStatus	error = VI_SUCCESS;
	ViInt32     modelType;
	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_INSTR_MODEL_TYPE, 0, &modelType));

	checkErr (Ivi_CoerceRepCapName (vi, SENSOR_REP_CAP_NAME, value, &value));

	if (!strcmp(value,"B") || !strcmp(value,"b") )
	{
		if(modelType==AUML24XX_VAL_MODEL_ML2487A || modelType==AUML24XX_VAL_MODEL_ML2495A ||  modelType==AUML24XX_VAL_MODEL_ML2487B)
		  	viCheckErr (IVI_ERROR_INVALID_VALUE);    

	}
Error:
	return error;
}

/*- AUML24XX_ATTR_INSTR_CW_AVERAGING_NUMBER -*/ 
static ViStatus _VI_FUNC auml24xxAttrInstrCWAveragingNumber_WriteCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	viCheckErr( auml24xx_ViPrintf (io, "CWAVG %s,,%ld",VALID_CHANNELNAME (channelName), value));

Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrInstrCWAveragingNumber_ReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32     cwAveragingMode;

	/* Get the corresponding internal attributes here */  
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_CW_AVERAGING_MODE, 0, &cwAveragingMode));

	if( (cwAveragingMode!=AUML24XX_VAL_AVERAGING_MOV) && (cwAveragingMode!=AUML24XX_VAL_AVERAGING_RPT))
		viCheckErr(AUML24XX_ERROR_ATTR_AVERAGE_NUMBER_UNAVAILABLE);

	viCheckErr( auml24xx_ViPrintf (io, "CWAVG? %s",VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%ld", value));

Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrInstrCWAveragingNumber_CheckCallback (ViSession vi,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32     cwAveragingMode;

	/* Get the corresponding internal attributes here */  
	checkErr (Ivi_DefaultCheckCallbackViInt32 (vi, channelName, attributeId, value));
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_CW_AVERAGING_MODE, 0, &cwAveragingMode));

	if( (cwAveragingMode!=AUML24XX_VAL_AVERAGING_MOV) && (cwAveragingMode!=AUML24XX_VAL_AVERAGING_RPT))
		viCheckErr(AUML24XX_ERROR_ATTR_AVERAGE_NUMBER_UNAVAILABLE);

Error:
	return error;
}

/*- AUML24XX_ATTR_AVERAGING_ENABLED -*/ 
static ViStatus _VI_FUNC auml24xxAttrAveragingEnabled_ReadCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViBoolean *value)
{
    ViStatus    error = VI_SUCCESS;
	ViInt32     channelMeasMode;
	ViAttr      targetAttributeId;
	/* Get the corresponding internal attributes here */  
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &channelMeasMode));
		switch (channelMeasMode) {
		case AUML24XX_VAL_MEAS_MODE_CW:
			{
				targetAttributeId = AUML24XX_ATTR_INSTR_CW_AVERAGING_ENABLED; 
			}
			break;
		case AUML24XX_VAL_MEAS_MODE_PMOD:
			{
				targetAttributeId = AUML24XX_ATTR_INSTR_PMOD_AVERAGING_ENABLED; 
			}
			break;

		default:
			break;
	}

	checkErr (Ivi_GetAttributeViBoolean (vi, channelName, targetAttributeId, 0, value));  	

Error:
    return error;
}

static ViStatus _VI_FUNC auml24xxAttrAveragingEnabled_WriteCallback (ViSession vi,
                                                                     ViSession io,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViBoolean value)
{
    ViStatus    error = VI_SUCCESS;
	ViInt32     channelMeasMode;
	ViAttr      targetAttributeId;
	/* Set the corresponding internal attributes here */       
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE, 0, &channelMeasMode));
		switch (channelMeasMode) {
		case AUML24XX_VAL_MEAS_MODE_CW:
			{
				targetAttributeId = AUML24XX_ATTR_INSTR_CW_AVERAGING_ENABLED; 
			}
			break;
		case AUML24XX_VAL_MEAS_MODE_PMOD:
			{
				targetAttributeId = AUML24XX_ATTR_INSTR_PMOD_AVERAGING_ENABLED; 
			}
			break;

		default:
			break;
	}

	checkErr (Ivi_SetAttributeViBoolean (vi, channelName, targetAttributeId, 0, value));  

Error:
    return error;
}

/*- AUML24XX_ATTR_LIMIT_COMPLEX_SPECIFICATION_ACTIVE_NUMBER -*/ 
static ViStatus _VI_FUNC auml24xxAttrLimitComplexSpecificationActiveNumber_WriteCallback (ViSession vi,
                                                                                          ViSession io,
                                                                                          ViConstString channelName,
                                                                                          ViAttr attributeId,
                                                                                          ViInt32 value)

{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;
	ViInt32     limitSpecType;

	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_LIMIT_COMPLEX_SPECIFICATION_TYPE, 0, &limitSpecType));  

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViInt32_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViInt32_commands, attributeId, &repCommand));

	if(limitSpecType== AUML24XX_VAL_LIMIT_SPEC_USER)
		{
		   viCheckErr( auml24xx_ViPrintf (io, "%s %s,USER%s%ld", command, VALID_CHANNELNAME (channelName), repCommand, value));
		}
	else if( limitSpecType== AUML24XX_VAL_LIMIT_SPEC_PDEF)
		{
		   viCheckErr( auml24xx_ViPrintf (io, "%s %s,PDEF%s%ld", command, VALID_CHANNELNAME (channelName), repCommand, value));
		}
	else 
		{
		   viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);     
		}
Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrLimitComplexSpecificationActiveNumber_ReadCallback (ViSession vi,
                                                                                         ViSession io,
                                                                                         ViConstString channelName,
                                                                                         ViAttr attributeId,
                                                                                         ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViInt32_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViInt32_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s",command, VALID_CHANNELNAME (channelName)));
    viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%ld", value));

Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrLimitComplexSpecificationActiveNumber_RangeTableCallback (ViSession vi,
                                                                                               ViConstString channelName,
                                                                                               ViAttr attributeId,
                                                                                               IviRangeTablePtr *rangeTablePtr)
{
	ViStatus	error = VI_SUCCESS;
	IviRangeTablePtr	tblPtr = VI_NULL;
	ViInt32     limitSpecType;

	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_LIMIT_COMPLEX_SPECIFICATION_TYPE, 0, &limitSpecType));
	switch (limitSpecType) {
		case AUML24XX_VAL_LIMIT_SPEC_USER:
			tblPtr = &attrLimitUserComplexSpecificationActiveNumberRangeTable;
			break;
		case AUML24XX_VAL_LIMIT_SPEC_PDEF:
			tblPtr = &attrLimitPDEFComplexSpecificationActiveNumberRangeTable;
			break;
		default:
			viCheckErr (IVI_ERROR_INVALID_VALUE);
			break;
	}

Error:
	*rangeTablePtr = tblPtr;
	return error;
}

/*- AUML24XX_ATTR_GPIB_RESPONSE_BUFFERING_ENABLED -*/ 
static ViStatus _VI_FUNC auml24xxAttrGpibResponseBufferingEnabled_ReadCallback (ViSession vi,
                                                                                ViSession io,
                                                                                ViConstString channelName,
                                                                                ViAttr attributeId,
                                                                                ViBoolean *value){
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
    ViChar	rdBuffer[BUFFER_SIZE];  
	viCheckErr( auml24xx_GetStringFromTable (auml24xxViBoolean_commands, attributeId, &command)); 

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s", command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^ ] %s", rdBuffer));
	if (!strcmp(rdBuffer,"OFF"))
		{
		   *value=VI_FALSE;
		}
	else if(!strcmp(rdBuffer,"ON"))

		{
		   *value=VI_TRUE;
		}
	else
		{
			viCheckErr (IVI_ERROR_INVALID_VALUE);
		}

Error:
	return error;
}

/*- AUML24XX_ATTR_FAST_MODE_ENABLED -*/ 
static ViStatus _VI_FUNC auml24xxAttrFastModeEnabled_ReadCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViBoolean *value){
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
    ViChar	rdBuffer[BUFFER_SIZE];       
	viCheckErr( auml24xx_GetStringFromTable (auml24xxViBoolean_commands, attributeId, &command)); 


	viCheckErr( auml24xx_ViPrintf (io, "%s? %s", command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^ ] %s", rdBuffer));
	if (!strcmp(rdBuffer,"OFF"))
		{
		   *value=VI_FALSE;
		}
	else if(!strcmp(rdBuffer,"ON"))

		{
		   *value=VI_TRUE;
		}
	else
		{
			viCheckErr (IVI_ERROR_INVALID_VALUE);
		}

Error:
	return error;
}

/*- AUML24XX_ATTR_BNC_DB_ANALOG_OUTPUT_START -*/ 
static ViStatus _VI_FUNC auml24xxAttrBncDbAnalogOutputStart_ReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViReal64 *value){
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,DB",command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));

Error:
	return error;
}

/*- AUML24XX_ATTR_BNC_DB_ANALOG_OUTPUT_STOP -*/ 
static ViStatus _VI_FUNC auml24xxAttrBncDbAnalogOutputStop_ReadCallback (ViSession vi,
                                                                         ViSession io,
                                                                         ViConstString channelName,
                                                                         ViAttr attributeId,
                                                                         ViReal64 *value){
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,DB",command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));

Error:
	return error;
}

/*- AUML24XX_ATTR_BNC_DBM_ANALOG_OUTPUT_START -*/ 
static ViStatus _VI_FUNC auml24xxAttrBncDbmAnalogOutputStart_ReadCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViReal64 *value){
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,DBM",command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));

Error:
	return error;
}

/*- AUML24XX_ATTR_BNC_DBM_ANALOG_OUTPUT_STOP -*/ 
static ViStatus _VI_FUNC auml24xxAttrBncDbmAnalogOutputStop_ReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViReal64 *value){
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,DBM",command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));

Error:
	return error;
}

/*- AUML24XX_ATTR_BNC_DBMV_ANALOG_OUTPUT_START -*/ 
static ViStatus _VI_FUNC auml24xxAttrBncDbmvAnalogOutputStart_ReadCallback (ViSession vi,
                                                                            ViSession io,
                                                                            ViConstString channelName,
                                                                            ViAttr attributeId,
                                                                            ViReal64 *value){
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,DBMV",command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));

Error:
	return error;
}

/*- AUML24XX_ATTR_BNC_DBMV_ANALOG_OUTPUT_STOP -*/ 
static ViStatus _VI_FUNC auml24xxAttrBncDbmvAnalogOutputStop_ReadCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViReal64 *value){
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));


		viCheckErr( auml24xx_ViPrintf (io, "%s? %s,DBMV",command, VALID_CHANNELNAME (channelName)));
		viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));

Error:
	return error;
}

/*- AUML24XX_ATTR_BNC_DBUV_ANALOG_OUTPUT_START -*/ 
static ViStatus _VI_FUNC auml24xxAttrBncDbuvAnalogOutputStart_ReadCallback (ViSession vi,
                                                                            ViSession io,
                                                                            ViConstString channelName,
                                                                            ViAttr attributeId,
                                                                            ViReal64 *value){
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,DBUV",command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));

Error:
	return error;
}

/*- AUML24XX_ATTR_BNC_DBUV_ANALOG_OUTPUT_STOP -*/ 
static ViStatus _VI_FUNC auml24xxAttrBncDbuvAnalogOutputStop_ReadCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViReal64 *value){
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,DBUV",command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));

Error:
	return error;
}

/*- AUML24XX_ATTR_BNC_DBW_ANALOG_OUTPUT_START -*/ 
static ViStatus _VI_FUNC auml24xxAttrBncDbwAnalogOutputStart_ReadCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViReal64 *value){
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,DBW",command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));

Error:
	return error;
}

/*- AUML24XX_ATTR_BNC_WATTS_ANALOG_OUTPUT_START -*/     
static ViStatus _VI_FUNC auml24xxAttrBncWattsAnalogOutputStart_ReadCallback (ViSession vi,
                                                                             ViSession io,
                                                                             ViConstString channelName,
                                                                             ViAttr attributeId,
                                                                             ViReal64 *value){
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));

	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,W",command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));

Error:
	return error;
}

/*- AUML24XX_ATTR_BNC_WATTS_ANALOG_OUTPUT_STOP -*/     
static ViStatus _VI_FUNC auml24xxAttrBncWattsAnalogOutputStop_ReadCallback (ViSession vi,
                                                                            ViSession io,
                                                                            ViConstString channelName,
                                                                            ViAttr attributeId,
                                                                            ViReal64 *value){
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViString    repCommand;

	viCheckErr( auml24xx_GetStringFromTable (auml24xxViReal64_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxViReal64_commands, attributeId, &repCommand));


	viCheckErr( auml24xx_ViPrintf (io, "%s? %s,W",command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%*[^,],%Le", value));

Error:
	return error;
}

/*- AUML24XX_ATTR_BNC1_OUTPUT_MODE -*/     
static ViStatus _VI_FUNC auml24xxAttrBnc1OutputMode_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 *value){
	ViStatus			error = VI_SUCCESS;
	ViString 			command;
	ViString            repCommand;
	IviRangeTablePtr 	table;
	ViChar	rdBuffer[BUFFER_SIZE]; 
	viCheckErr( auml24xx_GetStringFromTable (auml24xxEnum_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxEnum_commands, attributeId, &repCommand));

	checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));



	viCheckErr( auml24xx_ViPrintf (io, "%s? %s", command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^ ] %s", rdBuffer));
	viCheckErr( auml24xx_GetValueFromCmd(rdBuffer, table, value));
Error:
	return error;
}

/*- AUML24XX_ATTR_BNC2_OUTPUT_MODE -*/     
static ViStatus _VI_FUNC auml24xxAttrBnc2OutputMode_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 *value){
	ViStatus			error = VI_SUCCESS;
	ViString 			command;
	ViString            repCommand;
	IviRangeTablePtr 	table;
 	ViChar	rdBuffer[BUFFER_SIZE];
	viCheckErr( auml24xx_GetStringFromTable (auml24xxEnum_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxEnum_commands, attributeId, &repCommand));

	checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));
	viCheckErr( auml24xx_ViPrintf (io, "%s? %s", command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^ ] %s", rdBuffer));
	viCheckErr( auml24xx_GetValueFromCmd(rdBuffer, table, value));
Error:
	return error;
}

/*- AUML24XX_ATTR_UNITS -*/     
static ViStatus _VI_FUNC auml24xxAttrUnits_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViInt32 *value){
	ViStatus			error = VI_SUCCESS;
	ViString 			command;
	IviRangeTablePtr 	table;
	ViInt32             activeChannel;
	ViChar	rdBuffer[BUFFER_SIZE];  
	
	checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AUML24XX_ATTR_ACTIVE_CHANNEL, 0, &activeChannel));   
	viCheckErr( auml24xx_GetStringFromTable (auml24xxEnum_commands, attributeId, &command)); 
	checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));
	viCheckErr( auml24xx_ViPrintf (io, "%s? %ld", command, activeChannel));
	viCheckErr( auml24xx_ViScanf(io, "%*[^,],%s", rdBuffer));
	viCheckErr( auml24xx_GetValueFromCmd(rdBuffer, table, value));

Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrUnits_WriteCallback (ViSession vi,
                                                          ViSession io,
                                                          ViConstString channelName,
                                                          ViAttr attributeId,
                                                          ViInt32 value){
	ViStatus			error = VI_SUCCESS;
	ViString 			command,
						commandValue;
	ViString            repCommand;
	IviRangeTablePtr 	table;
	ViInt32             activeChannel;

	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_ACTIVE_CHANNEL, 0, &activeChannel));   

	viCheckErr( auml24xx_GetStringFromTable (auml24xxEnum_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxEnum_commands, attributeId, &repCommand));

	checkErr( Ivi_GetAttrRangeTable (vi, VI_NULL, attributeId, &table));
	viCheckErr( auml24xx_GetCmdFromIntValue(value, table, &commandValue));


	viCheckErr( auml24xx_ViPrintf (io, "%s %ld%s%s", command, activeChannel, repCommand, commandValue));
Error:
	return error;
}

/*- AUML24XX_ATTR_ACTIVE_CHANNEL -*/     
static ViStatus _VI_FUNC auml24xxAttrActiveChannel_CheckCallback (ViSession vi,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value)
{
	ViInt32     modelType;
	ViStatus	error = VI_SUCCESS;

	checkErr (Ivi_DefaultCheckCallbackViInt32 (vi, channelName, attributeId, value));
	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_INSTR_MODEL_TYPE, 0, &modelType));

	if(modelType == AUML24XX_VAL_MODEL_ML2487A||modelType==AUML24XX_VAL_MODEL_ML2495A|| modelType==AUML24XX_VAL_MODEL_ML2487B)
	{
		if (value==AUML24XX_VAL_CHAN2)
		{
			viCheckErr (IVI_ERROR_INVALID_CONFIGURATION); 
		}
	}

Error:
	return error;
}

/*- AUML24XX_ATTR_ACTIVE_CALIBRATION_FACTOR_NUMBER -*/     
static ViStatus _VI_FUNC auml24xxAttrActiveCalibrationFactorNumber_ReadCallback (ViSession vi,
                                                                                 ViSession io,
                                                                                 ViConstString channelName,
                                                                                 ViAttr attributeId,
                                                                                 ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;


	viCheckErr( auml24xx_ViPrintf (io, "SNCFUSE %s",VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf (io, "%ld",value));

Error:
	return error;
}

/*- AUML24XX_ATTR_ACTIVE_CALIBRATION_FACTOR_NUMBER -*/     
static ViStatus _VI_FUNC auml24xxAttrActiveCalibrationFactorNumber_WriteCallback (ViSession vi,
                                                                                  ViSession io,
                                                                                  ViConstString channelName,
                                                                                  ViAttr attributeId,
                                                                                  ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( auml24xx_ViPrintf (io, "SNCTABN %s,%ld",VALID_CHANNELNAME (channelName),value));

Error:
	return error;
}

/*- AUML24XX_ATTR_DUAL_CHANNEL_MODE_ENABLED -*/     
static ViStatus _VI_FUNC auml24xxAttrDualChannelModeEnabled_ReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViBoolean *value){
    ViInt32 chanNumber;
	ViStatus    error = VI_SUCCESS;
	viCheckErr( auml24xx_ViQueryf (io, "CHDISPN?","%*[^ ] %ld", &chanNumber));
	if(chanNumber==1)
		{ 
			*value=VI_FALSE;
		}
	else
		{
			*value=VI_TRUE;
		}

Error:
	return error;

}

static ViStatus _VI_FUNC auml24xxAttrDualChannelModeEnabled_WriteCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViBoolean value){
    ViStatus    error = VI_SUCCESS;

	viCheckErr( auml24xx_ViPrintf (io, "CHDISPN %ld", value ? 2:1));

Error:
	return error;

}

/*- AUML24XX_ATTR_ACTIVE_CHANNEL -*/     
static ViStatus _VI_FUNC auml24xxAttrActiveChannel_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value){
	ViStatus			error = VI_SUCCESS;
	ViString 			command;
	ViString            repCommand;
	IviRangeTablePtr 	table;
	ViChar	rdBuffer[BUFFER_SIZE];

	viCheckErr( auml24xx_GetStringFromTable (auml24xxEnum_commands, attributeId, &command)); 
	viCheckErr (auml24xx_GetRepCommandFromTable (auml24xxEnum_commands, attributeId, &repCommand));

	checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));

    viCheckErr( auml24xx_ViPrintf (io, "%s? %s", command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^ ] %s", rdBuffer));
	viCheckErr( auml24xx_GetValueFromCmd(rdBuffer, table, value));
	
Error:
	return error;
}

/*- AUML24XX_ATTR_REF_OSCILLATOR_ENABLED -*/     
static ViStatus _VI_FUNC auml24xxAttrRefOscillatorEnabled_ReadCallback (ViSession vi,
                                                                        ViSession io,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViBoolean *value){
	ViStatus	error = VI_SUCCESS;
	ViString 	command;
	ViChar	rdBuffer[BUFFER_SIZE];
	
	viCheckErr( auml24xx_GetStringFromTable (auml24xxViBoolean_commands, attributeId, &command)); 
	viCheckErr( auml24xx_ViPrintf (io, "%s? %s", command, VALID_CHANNELNAME (channelName)));
	viCheckErr( auml24xx_ViScanf(io, "%*[^ ] %s", rdBuffer));
	if (!strcmp(rdBuffer,"OFF"))
		{
		   *value=VI_FALSE;
		}
	else if(!strcmp(rdBuffer,"ON"))

		{
		   *value=VI_TRUE;
		}
	else
		{
			viCheckErr (IVI_ERROR_INVALID_VALUE);
		}

	
Error:
	return error;
}

/*- AUML24XX_ATTR_BNC_OUTPUT_SOURCE_CHANNEL -*/
static ViStatus _VI_FUNC auml24xxAttrBncOutputSourceChannel_CheckCallback (ViSession vi,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32     modelType;

	checkErr (Ivi_DefaultCheckCallbackViInt32 (vi, channelName, attributeId, value));
	checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_INSTR_MODEL_TYPE, 0, &modelType));

	if(modelType == AUML24XX_VAL_MODEL_ML2487A || modelType == AUML24XX_VAL_MODEL_ML2495A || modelType == AUML24XX_VAL_MODEL_ML2487B) 
	{
	    if(value == 2)
	    	viCheckErr (IVI_ERROR_INVALID_CONFIGURATION); 
	}
Error:
	return error;
}

static ViStatus _VI_FUNC auml24xxAttrRangeAutoEnabled_CheckCallback (ViSession vi,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViBoolean value)
{
	ViStatus	error = VI_SUCCESS;

	if(value == VI_FALSE)
		viCheckErr(IVI_ERROR_INVALID_VALUE);
	
Error:
	return error;
}

/*****************************************************************************
 * Function: auml24xx_InitAttributes                                                  
 * Purpose:  This function adds attributes to the IVI session, initializes   
 *           instrument attributes, and sets attribute invalidation          
 *           dependencies.                                                   
 *****************************************************************************/
static ViStatus auml24xx_InitAttributes (ViSession vi,ViInt32 model)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     flags = 0;

    checkErr( Ivi_SetAttrReadCallbackViString (vi, AUML24XX_ATTR_SPECIFIC_DRIVER_REVISION,
                                               auml24xxAttrDriverRevision_ReadCallback));
    checkErr( Ivi_SetAttributeViInt32 (vi, "", AUML24XX_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION,
                                       0, AUML24XX_CLASS_SPEC_MAJOR_VERSION));
    checkErr( Ivi_SetAttributeViInt32 (vi, "", AUML24XX_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION,
                                       0, AUML24XX_CLASS_SPEC_MINOR_VERSION));
    checkErr( Ivi_SetAttributeViString (vi, "", AUML24XX_ATTR_SUPPORTED_INSTRUMENT_MODELS,
                                        0, AUML24XX_SUPPORTED_INSTRUMENT_MODELS));

    checkErr( Ivi_GetAttributeFlags (vi, AUML24XX_ATTR_INSTRUMENT_FIRMWARE_REVISION, &flags));
    checkErr( Ivi_SetAttributeFlags (vi, AUML24XX_ATTR_INSTRUMENT_FIRMWARE_REVISION, 
                                     flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
    checkErr( Ivi_SetAttrReadCallbackViString (vi, AUML24XX_ATTR_INSTRUMENT_FIRMWARE_REVISION,
                                               auml24xxAttrFirmwareRevision_ReadCallback));
	checkErr( Ivi_GetAttributeFlags (vi, AUML24XX_ATTR_INSTRUMENT_MANUFACTURER, &flags));
	checkErr( Ivi_SetAttributeFlags (vi, AUML24XX_ATTR_INSTRUMENT_MANUFACTURER, 
	                                 flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
	checkErr( Ivi_SetAttrReadCallbackViString (vi, AUML24XX_ATTR_INSTRUMENT_MANUFACTURER,
	                                           auml24xxAttrInstrumentManufacturer_ReadCallback));
	checkErr( Ivi_GetAttributeFlags (vi, AUML24XX_ATTR_INSTRUMENT_MODEL, &flags));
	checkErr( Ivi_SetAttributeFlags (vi, AUML24XX_ATTR_INSTRUMENT_MODEL, 
	                                 flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
	checkErr( Ivi_SetAttrReadCallbackViString (vi, AUML24XX_ATTR_INSTRUMENT_MODEL,
	                                           auml24xxAttrInstrumentModel_ReadCallback));

    checkErr( Ivi_SetAttributeViString (vi, "", AUML24XX_ATTR_SPECIFIC_DRIVER_VENDOR,
                                        0, AUML24XX_DRIVER_VENDOR));
    checkErr( Ivi_SetAttributeViString (vi, "", AUML24XX_ATTR_SPECIFIC_DRIVER_DESCRIPTION,
                                        0, AUML24XX_DRIVER_DESCRIPTION));
    checkErr( Ivi_SetAttributeViAddr (vi, VI_NULL, IVI_ATTR_OPC_CALLBACK, 0,
                                      auml24xx_WaitForOPCCallback));
    checkErr( Ivi_SetAttributeViAddr (vi, VI_NULL, IVI_ATTR_CHECK_STATUS_CALLBACK, 0,
                                      auml24xx_CheckStatusCallback));

	checkErr( Ivi_SetAttributeViString (vi, "", AUML24XX_ATTR_GROUP_CAPABILITIES, 0,
	                                    "IviPwrMeterBase,"
	                                    "IviPwrMeterTriggerSource,"
	                                    "IviPwrMeterInternalTrigger,"
	                                    "IviPwrMeterSoftwareTrigger,"
	                                    "IviPwrMeterAveragingCount,"
	                                    "IviPwrMeterZeroCorrection,"
	                                    "IviPwrMeterDutyCycleCorrection,"
	                                    "IviPwrMeterCalibration,"
	                                    "IviPwrMeterReferenceOscillator"));

        /*- Add instrument-specific attributes ------------------------------*/            
	checkErr (Ivi_AddAttributeViBoolean (vi, AUML24XX_ATTR_AVERAGING_AUTO_ENABLED,
	                                     "AUML24XX_ATTR_AVERAGING_AUTO_ENABLED", 0,
	                                     IVI_VAL_MULTI_CHANNEL,
	                                     auml24xxAttrAveragingAutoEnabled_ReadCallback,
	                                     auml24xxAttrAveragingAutoEnabled_WriteCallback));

	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_CORRECTION_FREQUENCY,
	                                    "AUML24XX_ATTR_CORRECTION_FREQUENCY",
	                                    50.00e6, IVI_VAL_MULTI_CHANNEL, VI_NULL,
	                                    VI_NULL, VI_NULL, 0));

	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_OFFSET,
	                                    "AUML24XX_ATTR_OFFSET", 0.0,
	                                    IVI_VAL_MULTI_CHANNEL, VI_NULL, VI_NULL,
	                                    VI_NULL, 0));

	checkErr (Ivi_AddAttributeViBoolean (vi, AUML24XX_ATTR_RANGE_AUTO_ENABLED,
	                                     "AUML24XX_ATTR_RANGE_AUTO_ENABLED", VI_TRUE,
	                                     IVI_VAL_MULTI_CHANNEL, VI_NULL, VI_NULL));
	checkErr (Ivi_SetAttrCheckCallbackViBoolean (vi,
	                                             AUML24XX_ATTR_RANGE_AUTO_ENABLED,
	                                             auml24xxAttrRangeAutoEnabled_CheckCallback));

	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_UNITS,
	                                   "AUML24XX_ATTR_UNITS", AUML24XX_VAL_DBM,
	                                   IVI_VAL_NEVER_CACHE,
	                                   auml24xxAttrUnits_ReadCallback,
	                                   auml24xxAttrUnits_WriteCallback,
	                                   &attrUnitsRangeTable));

	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_TRIGGER_SOURCE,
	                                   "AUML24XX_ATTR_TRIGGER_SOURCE",
	                                   AUML24XX_VAL_IMMEDIATE,
	                                   IVI_VAL_NEVER_CACHE | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION,
	                                   auml24xxAttrTriggerSource_ReadCallback,
	                                   auml24xxAttrTriggerSource_WriteCallback,
	                                   &attrTriggerSourceRangeTable));
	checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi, AUML24XX_ATTR_TRIGGER_SOURCE,
	                                           auml24xxAttrTriggerSource_CheckCallback));

	checkErr (Ivi_AddAttributeViString (vi,
	                                    AUML24XX_ATTR_INTERNAL_TRIGGER_EVENT_SOURCE,
	                                    "AUML24XX_ATTR_INTERNAL_TRIGGER_EVENT_SOURCE",
	                                    "A",
	                                    IVI_VAL_NEVER_CACHE | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION,
	                                    auml24xxAttrInternalTriggerEventSource_ReadCallback,
	                                    auml24xxAttrInternalTriggerEventSource_WriteCallback));

	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_INTERNAL_TRIGGER_LEVEL,
	                                    "AUML24XX_ATTR_INTERNAL_TRIGGER_LEVEL", 0.0,
	                                    IVI_VAL_NEVER_CACHE | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION,
	                                    auml24xxAttrInternalTriggerLevel_ReadCallback,
	                                    auml24xxAttrInternalTriggerLevel_WriteCallback,
	                                    &attrInternalTriggerLevelRangeTable, 0));

	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_INTERNAL_TRIGGER_SLOPE,
	                                   "AUML24XX_ATTR_INTERNAL_TRIGGER_SLOPE",
	                                   AUML24XX_VAL_POSITIVE,
	                                   IVI_VAL_NEVER_CACHE | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION,
	                                   auml24xxAttrInternalTriggerSlope_ReadCallback,
	                                   auml24xxAttrInternalTriggerSlope_WriteCallback,
	                                   &attrInternalTriggerSlopeRangeTable));

	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_DUTY_CYCLE_CORRECTION,
	                                    "AUML24XX_ATTR_DUTY_CYCLE_CORRECTION", 100.0,
	                                    IVI_VAL_MULTI_CHANNEL,
	                                    auml24xxViReal64_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrDutyCycleCorrectionRangeTable, 0));

	checkErr (Ivi_AddAttributeViBoolean (vi,
	                                     AUML24XX_ATTR_DUTY_CYCLE_CORRECTION_ENABLED,
	                                     "AUML24XX_ATTR_DUTY_CYCLE_CORRECTION_ENABLED",
	                                     VI_FALSE, IVI_VAL_MULTI_CHANNEL,
	                                     auml24xxViBoolean_ReadCallback,
	                                     auml24xxViBoolean_WriteCallback));

	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_AVERAGING_COUNT,
	                                   "AUML24XX_ATTR_AVERAGING_COUNT", 1,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION,
	                                   auml24xxAttrAveragingCount_ReadCallback,
	                                   auml24xxAttrAveragingCount_WriteCallback,
	                                   &attrAveragingCountRangeTable));

	checkErr (Ivi_AddAttributeViBoolean (vi, AUML24XX_ATTR_REF_OSCILLATOR_ENABLED,
	                                     "AUML24XX_ATTR_REF_OSCILLATOR_ENABLED",
	                                     VI_FALSE, 0,
	                                     auml24xxAttrRefOscillatorEnabled_ReadCallback,
	                                     auml24xxViBoolean_WriteCallback));

	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_REF_OSCILLATOR_FREQUENCY,
	                                    "AUML24XX_ATTR_REF_OSCILLATOR_FREQUENCY",
	                                    AUML24XX_VAL_REF_CALIBRATOR_FREQ_50MHZ, 0,
	                                    auml24xxAttrRefOscillatorFrequency_ReadCallback,
	                                    auml24xxAttrRefOscillatorFrequency_WriteCallback,
	                                    &attrRFReferenceCalibratorFrequencyRangeTable,
	                                    0));

	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_REF_OSCILLATOR_LEVEL,
	                                    "AUML24XX_ATTR_REF_OSCILLATOR_LEVEL", 0.0, 0,
	                                    VI_NULL, VI_NULL, VI_NULL, 0));

	checkErr (Ivi_AddAttributeViString (vi, AUML24XX_ATTR_ID_QUERY_RESPONSE,
	                                    "AUML24XX_ATTR_ID_QUERY_RESPONSE", "ANRITSU",
	                                    IVI_VAL_NOT_USER_WRITABLE,
	                                    auml24xxAttrIdQueryResponse_ReadCallback,
	                                    VI_NULL));

    checkErr( Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_OPC_TIMEOUT,
                                       "AUML24XX_ATTR_OPC_TIMEOUT",
                                       5000, IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL)); 
    checkErr( Ivi_AddAttributeViInt32 (vi,
                                       AUML24XX_ATTR_VISA_RM_SESSION,
                                       "AUML24XX_ATTR_VISA_RM_SESSION",
                                       VI_NULL,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL));
    checkErr( Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_OPC_CALLBACK,
                                       "AUML24XX_ATTR_OPC_CALLBACK",
                                       VI_NULL,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL));
    checkErr( Ivi_AddAttributeViInt32 (vi,
                                       AUML24XX_ATTR_CHECK_STATUS_CALLBACK,
                                       "AUML24XX_ATTR_CHECK_STATUS_CALLBACK",
                                       VI_NULL,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL));
    checkErr( Ivi_AddAttributeViInt32 (vi,
                                       AUML24XX_ATTR_USER_INTERCHANGE_CHECK_CALLBACK,
                                       "AUML24XX_ATTR_USER_INTERCHANGE_CHECK_CALLBACK",
                                       VI_NULL,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_CHANNEL_INPUT_CONFIG,
	                                   "AUML24XX_ATTR_CHANNEL_INPUT_CONFIG",
	                                   AUML24XX_VAL_CHAN_CONFIG_A,
	                                   IVI_VAL_MULTI_CHANNEL,
	                                   auml24xxEnum_ReadCallback,
	                                   auml24xxEnum_WriteCallback, VI_NULL));
	checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi,
	                                           AUML24XX_ATTR_CHANNEL_INPUT_CONFIG,
	                                           auml24xxAttrChannelInputConfig_CheckCallback));
	checkErr (Ivi_SetAttrRangeTableCallback (vi, AUML24XX_ATTR_CHANNEL_INPUT_CONFIG,
	                                         auml24xxAttrChannelInputConfig_RangeTableCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE,
	                                   "AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE",
	                                   AUML24XX_VAL_MEAS_MODE_PMOD,
	                                   IVI_VAL_MULTI_CHANNEL,
	                                   auml24xxEnum_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrChannelMeasurementModeRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_PMOD_MEASUREMENT_TYPE,
	                                   "AUML24XX_ATTR_PMOD_MEASUREMENT_TYPE",
	                                   AUML24XX_VAL_PMOD_MEAS_TYPE_1,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                   auml24xxEnum_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrPMODMeasurementTypeRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_SETTLE_PERCENT,
	                                    "AUML24XX_ATTR_SETTLE_PERCENT", 0.1,
	                                    IVI_VAL_MULTI_CHANNEL,
	                                    auml24xxViReal64_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrSettlePercentRangeTable, 0));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_RELATIVE_MODE,
	                                   "AUML24XX_ATTR_RELATIVE_MODE",
	                                   AUML24XX_VAL_RELATIVE_MODE_OFF,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                   auml24xxEnum_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrRelativeModeRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_CW_AVERAGING_MODE,
	                                   "AUML24XX_ATTR_CW_AVERAGING_MODE",
	                                   AUML24XX_VAL_AVERAGING_AUTO,
	                                   IVI_VAL_MULTI_CHANNEL,
	                                   auml24xxAttrCWAveragingMode_ReadCallback,
	                                   auml24xxAttrCWAveragingMode_WriteCallback,
	                                   &attrCWAveragingModeRangeTable));
	checkErr (Ivi_AddAttributeViBoolean (vi, AUML24XX_ATTR_MIN_MAX_TRACKING_ENABLED,
	                                     "AUML24XX_ATTR_MIN_MAX_TRACKING_ENABLED",
	                                     VI_FALSE, IVI_VAL_MULTI_CHANNEL,
	                                     auml24xxViBoolean_ReadCallback,
	                                     auml24xxViBoolean_WriteCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_DISPlAY_MEASUREMENT_POINTS,
	                                   "AUML24XX_ATTR_DISPlAY_MEASUREMENT_POINTS",
	                                   AUML24XX_VAL_P200, IVI_VAL_NEVER_CACHE,
	                                   auml24xxAttrDisplayMeasurementPoints_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrDisplayMeasurementPoints));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_TRIGGER_ARMING_MODE,
	                                   "AUML24XX_ATTR_TRIGGER_ARMING_MODE",
	                                   AUML24XX_VAL_TRIGGER_ARMING_MODE_AUTO,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION,
	                                   auml24xxAttrTriggerArmingMode_ReadCallback,
	                                   auml24xxAttrTriggerArmingMode_WriteCallback,
	                                   VI_NULL));
	checkErr (Ivi_AddAttributeViBoolean (vi, AUML24XX_ATTR_TRIGGER_AUTO_ENABLED,
	                                     "AUML24XX_ATTR_TRIGGER_AUTO_ENABLED",
	                                     VI_TRUE, IVI_VAL_MULTI_CHANNEL,
	                                     auml24xxViBoolean_ReadCallback,
	                                     auml24xxViBoolean_WriteCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_TRIGGER_BANDWIDTH,
	                                   "AUML24XX_ATTR_TRIGGER_BANDWIDTH",
	                                   AUML24XX_VAL_TRIGGER_BANDWIDTH_20MHZ,
	                                   IVI_VAL_MULTI_CHANNEL,
	                                   auml24xxEnum_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrTriggerBandwidthRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_CAPTURE_TIME,
	                                    "AUML24XX_ATTR_CAPTURE_TIME", 4.6e-3,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION,
	                                    auml24xxAttrCaptureTime_ReadCallback,
	                                    auml24xxAttrCaptureTime_WriteCallback,
	                                    &attrCaptureTimeRangeTable, 0));
	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_TRIGGER_DELAY_TIME,
	                                    "AUML24XX_ATTR_TRIGGER_DELAY_TIME", 0.0,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION,
	                                    auml24xxAttrTriggerDelayTime_ReadCallback,
	                                    auml24xxAttrTriggerDelayTime_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_TRIGGER_FRAME_ARMING_LEVEL,
	                                    "AUML24XX_ATTR_TRIGGER_FRAME_ARMING_LEVEL",
	                                    0.0, IVI_VAL_MULTI_CHANNEL,
	                                    auml24xxViReal64_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrTriggerFrameArmingLevelRangeTable, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi,
	                                            AUML24XX_ATTR_TRIGGER_FRAME_ARMING_LEVEL,
	                                            auml24xxAttrTriggerFrameArmingLevel_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi,
	                                    AUML24XX_ATTR_TRIGGER_FRAME_ARMING_TIME_DURATION,
	                                    "AUML24XX_ATTR_TRIGGER_FRAME_ARMING_TIME_DURATION",
	                                    4.6e-3,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                    auml24xxViReal64_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrTriggerFrameArmingTimeDurationRangeTable,
	                                    0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi,
	                                            AUML24XX_ATTR_TRIGGER_FRAME_ARMING_TIME_DURATION,
	                                            auml24xxAttrTriggerFrameArmingTimeDuration_CheckCallback));
	checkErr (Ivi_AddAttributeViBoolean (vi, AUML24XX_ATTR_TRIGGER_HOLDOFF_ENABLED,
	                                     "AUML24XX_ATTR_TRIGGER_HOLDOFF_ENABLED",
	                                     VI_FALSE, IVI_VAL_MULTI_CHANNEL,
	                                     auml24xxViBoolean_ReadCallback,
	                                     auml24xxViBoolean_WriteCallback));
	checkErr (Ivi_SetAttrCheckCallbackViBoolean (vi,
	                                             AUML24XX_ATTR_TRIGGER_HOLDOFF_ENABLED,
	                                             auml24xxAttrTriggerHoldoffEnabled_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_TRIGGER_HOLDOFF_TIME,
	                                    "AUML24XX_ATTR_TRIGGER_HOLDOFF_TIME", 0.0,
	                                    IVI_VAL_MULTI_CHANNEL,
	                                    auml24xxViReal64_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrTriggerHoldoffTimeRangeTable, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi,
	                                            AUML24XX_ATTR_TRIGGER_HOLDOFF_TIME,
	                                            auml24xxAttrTriggerHoldoffTime_CheckCallback));
	checkErr (Ivi_AddAttributeViBoolean (vi, AUML24XX_ATTR_TRIGGER_LINKING_ENABLED,
	                                     "AUML24XX_ATTR_TRIGGER_LINKING_ENABLED",
	                                     VI_FALSE, 0,
	                                     auml24xxAttrTriggerLinkingEnabled_ReadCallback,
	                                     auml24xxViBoolean_WriteCallback));
	checkErr (Ivi_SetAttrCheckCallbackViBoolean (vi,
	                                             AUML24XX_ATTR_TRIGGER_LINKING_ENABLED,
	                                             auml24xxAttrTriggerLinkingEnabled_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_SAMPLE_RATE,
	                                   "AUML24XX_ATTR_SAMPLE_RATE",
	                                   AUML24XX_VAL_AUTO, IVI_VAL_MULTI_CHANNEL,
	                                   auml24xxAttrSampleRate_ReadCallback,
	                                   auml24xxAttrSampleRate_WriteCallback, VI_NULL));
	checkErr (Ivi_SetAttrRangeTableCallback (vi, AUML24XX_ATTR_SAMPLE_RATE,
	                                         auml24xxAttrSampleRate_RangeTableCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_TRIGGER_EXTERNAL_EDGE,
	                                   "AUML24XX_ATTR_TRIGGER_EXTERNAL_EDGE",
	                                   AUML24XX_VAL_EXT_EDGE_RISE, 0,
	                                   auml24xxAttrTriggerExternalEdge_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrTriggerExternalEdgeRangeTable));
	checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, SENSOR_REP_CAP_NAME,
	                                            AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_ADJUST,
	                                            "AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_ADJUST",
	                                            100.0, 0,
	                                            auml24xxAttrSensorCalibrationFactorAdjust_ReadCallback,
	                                            auml24xxAttrSensorCalibrationFactorAdjust_WriteCallback,
	                                            VI_NULL, 0));
	checkErr (Ivi_SetAttrRangeTableCallback (vi,
	                                         AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_ADJUST,
	                                         auml24xxAttrSensorCalibrationFactorAdjust_RangeTableCallback));
	checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, SENSOR_REP_CAP_NAME,
	                                            AUML24XX_ATTR_SENSOR_CALIBRATION_MANUAL_FACTOR,
	                                            "AUML24XX_ATTR_SENSOR_CALIBRATION_MANUAL_FACTOR",
	                                            100.0, 0,
	                                            auml24xxAttrSensorCalibrationManualFactor_ReadCallback,
	                                            auml24xxAttrSensorCalibrationManualFactor_WriteCallback,
	                                            VI_NULL, 0));
	checkErr (Ivi_SetAttrRangeTableCallback (vi,
	                                         AUML24XX_ATTR_SENSOR_CALIBRATION_MANUAL_FACTOR,
	                                         auml24xxAttrSensorCalibrationManualFactor_RangeTableCallback));
	checkErr (Ivi_AddRepeatedAttributeViInt32 (vi, SENSOR_REP_CAP_NAME,
	                                           AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_SOURCE,
	                                           "AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_SOURCE",
	                                           AUML24XX_VAL_CAL_FACTOR_SOURCE_FREQ,
	                                           0, auml24xxEnum_ReadCallback,
	                                           auml24xxEnum_WriteCallback,
	                                           &attrSensorCalibrationFactorSourceRangeTable));
	checkErr (Ivi_AddRepeatedAttributeViInt32 (vi, SENSOR_REP_CAP_NAME,
	                                           AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_UNITS,
	                                           "AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_UNITS",
	                                           AUML24XX_VAL_CAL_FACTOR_UNIT_PCT, 0,
	                                           auml24xxEnum_ReadCallback,
	                                           auml24xxEnum_WriteCallback,
	                                           &attrSensorCalibrationFactorUnitsRangeTable));
	checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, SENSOR_REP_CAP_NAME,
	                                            AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_STOP_FREQUENCY,
	                                            "AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_STOP_FREQUENCY",
	                                            2e10, 0,
	                                            auml24xxViReal64_ReadCallback,
	                                            auml24xxViReal64_WriteCallback,
	                                            &attrSensorCalibrationFactorStopFrequencyRangeTable,
	                                            0));
	checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, SENSOR_REP_CAP_NAME,
	                                            AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_STOP_VOLTAGE,
	                                            "AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_STOP_VOLTAGE",
	                                            10.0, 0,
	                                            auml24xxViReal64_ReadCallback,
	                                            auml24xxViReal64_WriteCallback,
	                                            &attrSensorCalibrationFactorStopVoltageRangeTable,
	                                            0));
	checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, SENSOR_REP_CAP_NAME,
	                                            AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_START_FREQUENCY,
	                                            "AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_START_FREQUENCY",
	                                            1e7, 0,
	                                            auml24xxViReal64_ReadCallback,
	                                            auml24xxViReal64_WriteCallback,
	                                            &attrSensorCalibrationFactorStartFrequencyRangeTable,
	                                            0));
	checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, SENSOR_REP_CAP_NAME,
	                                            AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_START_VOLTAGE,
	                                            "AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_START_VOLTAGE",
	                                            0.0, 0,
	                                            auml24xxViReal64_ReadCallback,
	                                            auml24xxViReal64_WriteCallback,
	                                            &attrSensorCalibrationFactorStartVoltageRangeTable,
	                                            0));
	checkErr (Ivi_AddAttributeViInt32 (vi,
	                                   AUML24XX_ATTR_GATE_PATTERN_1_REPEAT_NUMBER,
	                                   "AUML24XX_ATTR_GATE_PATTERN_1_REPEAT_NUMBER",
	                                   2,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                   auml24xxViInt32_ReadCallback,
	                                   auml24xxViInt32_WriteCallback,
	                                   &attrGatePattern1RepeatNumberRangeTable));
	checkErr (Ivi_AddAttributeViBoolean (vi,
	                                     AUML24XX_ATTR_GATE_PATTERN_1_REPEAT_ENABLED,
	                                     "AUML24XX_ATTR_GATE_PATTERN_1_REPEAT_ENABLED",
	                                     VI_FALSE,
	                                     IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                     auml24xxViBoolean_ReadCallback,
	                                     auml24xxViBoolean_WriteCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi,
	                                    AUML24XX_ATTR_GATE_PATTERN_1_REPEAT_OFFSET,
	                                    "AUML24XX_ATTR_GATE_PATTERN_1_REPEAT_OFFSET",
	                                    5.77e-4,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                    auml24xxViReal64_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrGatePattern1RepeatOffsetRangeTable, 0));
	checkErr (Ivi_AddAttributeViInt32 (vi,
	                                   AUML24XX_ATTR_GATE_PATTERN_CONFIGURATION_NUMBER,
	                                   "AUML24XX_ATTR_GATE_PATTERN_CONFIGURATION_NUMBER",
	                                   1,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_ALWAYS_CACHE,
	                                   VI_NULL, VI_NULL,
	                                   &attrGatePatternActiveNumberRangeTable));
	checkErr (Ivi_AddAttributeViBoolean (vi, AUML24XX_ATTR_FENCE_ENABLED,
	                                     "AUML24XX_ATTR_FENCE_ENABLED", VI_FALSE,
	                                     IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                     auml24xxAttrFenceEnabled_ReadCallback,
	                                     auml24xxAttrFenceEnabled_WriteCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_FENCE_STOP_TIME,
	                                    "AUML24XX_ATTR_FENCE_STOP_TIME", 3.21e-4,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                    auml24xxAttrFenceStopTime_ReadCallback,
	                                    auml24xxAttrFenceStopTime_WriteCallback,
	                                    &attrFenceStopTimeRangeTable, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, AUML24XX_ATTR_FENCE_STOP_TIME,
	                                            auml24xxAttrFenceStopTime_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_FENCE_START_TIME,
	                                    "AUML24XX_ATTR_FENCE_START_TIME", 2.25e-4,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                    auml24xxAttrFenceStartTime_ReadCallback,
	                                    auml24xxAttrFenceStartTime_WriteCallback,
	                                    &attrFenceStartTimeRangeTable, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, AUML24XX_ATTR_FENCE_START_TIME,
	                                            auml24xxAttrFenceStartTime_CheckCallback));
	checkErr (Ivi_AddAttributeViBoolean (vi, AUML24XX_ATTR_GATE_PATTERN_ENABLED,
	                                     "AUML24XX_ATTR_GATE_PATTERN_ENABLED",
	                                     VI_FALSE,
	                                     IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                     auml24xxAttrGatePatternEnabled_ReadCallback,
	                                     auml24xxAttrGatePatternEnabled_WriteCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_GATE_PATTERN_STOP_TIME,
	                                    "AUML24XX_ATTR_GATE_PATTERN_STOP_TIME",
	                                    5.77e-4,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                    auml24xxAttrGatePatternStopTime_ReadCallback,
	                                    auml24xxAttrGatePatternStopTime_WriteCallback,
	                                    &attrGatePatternStopTimeRangeTable, 0));
	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_GATE_PATTERN_START_TIME,
	                                    "AUML24XX_ATTR_GATE_PATTERN_START_TIME", 0.0,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                    auml24xxAttrGatePatternStartTime_ReadCallback,
	                                    auml24xxAttrGatePatternStartTime_WriteCallback,
	                                    &attrGatePatternStartTimeRangeTable, 0));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_MARKER_CONFIGURATION_NUMBER,
	                                   "AUML24XX_ATTR_MARKER_CONFIGURATION_NUMBER",
	                                   1,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_ALWAYS_CACHE,
	                                   VI_NULL, VI_NULL, &attrMarkerRangeTable));
	checkErr (Ivi_AddAttributeViBoolean (vi, AUML24XX_ATTR_MARKER_DELTA_ENABLED,
	                                     "AUML24XX_ATTR_MARKER_DELTA_ENABLED",
	                                     VI_FALSE, IVI_VAL_MULTI_CHANNEL,
	                                     auml24xxViBoolean_ReadCallback,
	                                     auml24xxViBoolean_WriteCallback));
	checkErr (Ivi_AddAttributeViBoolean (vi, AUML24XX_ATTR_MARKER_DELTA_LINK_ENABLED,
	                                     "AUML24XX_ATTR_MARKER_DELTA_LINK_ENABLED",
	                                     VI_FALSE, IVI_VAL_MULTI_CHANNEL,
	                                     auml24xxViBoolean_ReadCallback,
	                                     auml24xxViBoolean_WriteCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi,
	                                   AUML24XX_ATTR_MARKER_DELTA_MEASUREMENT_TYPE,
	                                   "AUML24XX_ATTR_MARKER_DELTA_MEASUREMENT_TYPE",
	                                   AUML24XX_VAL_DELTA_MARKER_PDIFF,
	                                   IVI_VAL_MULTI_CHANNEL,
	                                   auml24xxEnum_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrMarkerDeltaMeasurementTypeRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_MARKER_DELTA_POSITION,
	                                    "AUML24XX_ATTR_MARKER_DELTA_POSITION", 0.0,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                    auml24xxViReal64_ReadCallback,
	                                    auml24xxViReal64_WriteCallback, VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi,
	                                            AUML24XX_ATTR_MARKER_DELTA_POSITION,
	                                            auml24xxAttrMarkerDeltaPosition_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_MARKER_POSITION,
	                                    "AUML24XX_ATTR_MARKER_POSITION", 0.0,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                    auml24xxAttrMarkerPosition_ReadCallback,
	                                    auml24xxAttrMarkerPosition_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, AUML24XX_ATTR_MARKER_POSITION,
	                                            auml24xxAttrMarkerPosition_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_MARKER_SEARCH_LOWER_TARGET,
	                                    "AUML24XX_ATTR_MARKER_SEARCH_LOWER_TARGET",
	                                    1.00, IVI_VAL_MULTI_CHANNEL,
	                                    auml24xxViReal64_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrMarkerSearchLowerTargetRangeTable, 0));
	checkErr (Ivi_AddAttributeViInt32 (vi,
	                                   AUML24XX_ATTR_MARKER_SEARCH_START_VALUE_SOURCE,
	                                   "AUML24XX_ATTR_MARKER_SEARCH_START_VALUE_SOURCE",
	                                   AUML24XX_VAL_MARKER_SEARCH_SOURCE_MARKER,
	                                   IVI_VAL_MULTI_CHANNEL,
	                                   auml24xxEnum_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrMarkerSearchStartValueSourceRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_MARKER_SEARCH_UPPER_TARGET,
	                                    "AUML24XX_ATTR_MARKER_SEARCH_UPPER_TARGET",
	                                    90.00, IVI_VAL_MULTI_CHANNEL,
	                                    auml24xxViReal64_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrMarkerSearchUpperTargetRangeTable, 0));
	checkErr (Ivi_AddAttributeViBoolean (vi, AUML24XX_ATTR_MARKER_ENABLED,
	                                     "AUML24XX_ATTR_MARKER_ENABLED", VI_FALSE,
	                                     IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                     auml24xxAttrMarkerEnabled_ReadCallback,
	                                     auml24xxAttrMarkerEnabled_WriteCallback));
	checkErr (Ivi_AddAttributeViBoolean (vi,
	                                     AUML24XX_ATTR_LIMIT_FAIL_INDICATOR_HOLD_ENABLED,
	                                     "AUML24XX_ATTR_LIMIT_FAIL_INDICATOR_HOLD_ENABLED",
	                                     VI_FALSE, IVI_VAL_MULTI_CHANNEL,
	                                     auml24xxViBoolean_ReadCallback,
	                                     auml24xxViBoolean_WriteCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_LIMIT_LINE_TEST_TYPE,
	                                   "AUML24XX_ATTR_LIMIT_LINE_TEST_TYPE",
	                                   AUML24XX_VAL_LIMIT_LINE_TEST_BOTH,
	                                   IVI_VAL_MULTI_CHANNEL,
	                                   auml24xxEnum_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrLimitLineTestTypeRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_LIMIT_SIMPLE_LOWER_LINE,
	                                    "AUML24XX_ATTR_LIMIT_SIMPLE_LOWER_LINE",
	                                    -0.001,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                    auml24xxViReal64_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrLimitSimpleLowerLineRangeTable, 0));
	checkErr (Ivi_AddAttributeViBoolean (vi, AUML24XX_ATTR_LIMIT_CHECKING_ENABLED,
	                                     "AUML24XX_ATTR_LIMIT_CHECKING_ENABLED",
	                                     VI_FALSE, IVI_VAL_MULTI_CHANNEL,
	                                     auml24xxViBoolean_ReadCallback,
	                                     auml24xxViBoolean_WriteCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_LIMIT_SIMPLE_UPPER_LINE,
	                                    "AUML24XX_ATTR_LIMIT_SIMPLE_UPPER_LINE",
	                                    99.99e8,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                    auml24xxViReal64_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrLimitSimpleUpperLineRangeTable, 0));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_LIMIT_CHECKING_TYPE,
	                                   "AUML24XX_ATTR_LIMIT_CHECKING_TYPE",
	                                   AUML24XX_VAL_LIMIT_CHECKING_COMPLEX,
	                                   IVI_VAL_MULTI_CHANNEL,
	                                   auml24xxEnum_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrLimitCheckingTypeRangeTable));
	checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi, AUML24XX_ATTR_LIMIT_CHECKING_TYPE,
	                                           auml24xxAttrLimitCheckingType_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_LIMIT_COMPLEX_POWER_OFFSET,
	                                    "AUML24XX_ATTR_LIMIT_COMPLEX_POWER_OFFSET",
	                                    0.0, IVI_VAL_MULTI_CHANNEL,
	                                    auml24xxViReal64_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrLimitComplexPowerOffsetRangeTable, 0));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_LIMIT_COMPLEX_RPEAT_COUNT,
	                                   "AUML24XX_ATTR_LIMIT_COMPLEX_RPEAT_COUNT", 2,
	                                   IVI_VAL_MULTI_CHANNEL,
	                                   auml24xxViInt32_ReadCallback,
	                                   auml24xxViInt32_WriteCallback,
	                                   &attrLimitComplexRpeatCountRangeTable));
	checkErr (Ivi_AddAttributeViBoolean (vi,
	                                     AUML24XX_ATTR_LIMIT_COMPLEX_RPEAT_ENABLED,
	                                     "AUML24XX_ATTR_LIMIT_COMPLEX_RPEAT_ENABLED",
	                                     VI_FALSE, IVI_VAL_MULTI_CHANNEL,
	                                     auml24xxViBoolean_ReadCallback,
	                                     auml24xxViBoolean_WriteCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi,
	                                    AUML24XX_ATTR_LIMIT_COMPLEX_POWER_REPLICATION_OFFSET,
	                                    "AUML24XX_ATTR_LIMIT_COMPLEX_POWER_REPLICATION_OFFSET",
	                                    0.0, IVI_VAL_MULTI_CHANNEL,
	                                    auml24xxViReal64_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrLimitComplexPowerReplicationOffsetRangeTable,
	                                    0));
	checkErr (Ivi_AddAttributeViReal64 (vi,
	                                    AUML24XX_ATTR_LIMIT_COMPLEX_TIME_REPLICATION_OFFSET,
	                                    "AUML24XX_ATTR_LIMIT_COMPLEX_TIME_REPLICATION_OFFSET",
	                                    0.0, IVI_VAL_MULTI_CHANNEL,
	                                    auml24xxViReal64_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrLimitComplexTimeReplicationOffsetRangeTable,
	                                    0));
	checkErr (Ivi_AddAttributeViInt32 (vi,
	                                   AUML24XX_ATTR_LIMIT_COMPLEX_SPECIFICATION_TYPE,
	                                   "AUML24XX_ATTR_LIMIT_COMPLEX_SPECIFICATION_TYPE",
	                                   AUML24XX_VAL_LIMIT_SPEC_USER,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_ALWAYS_CACHE,
	                                   VI_NULL, VI_NULL,
	                                   &attrLimitComplexSpecificationTypeRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_LIMIT_COMPLEX_TIME_OFFSET,
	                                    "AUML24XX_ATTR_LIMIT_COMPLEX_TIME_OFFSET",
	                                    0.0, IVI_VAL_MULTI_CHANNEL,
	                                    auml24xxViReal64_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrLimitComplexTimeOffsetRangeTable, 0));
	checkErr (Ivi_AddAttributeViBoolean (vi,
	                                     AUML24XX_ATTR_POST_PROCESSING_ACQUISITION_ENABLED,
	                                     "AUML24XX_ATTR_POST_PROCESSING_ACQUISITION_ENABLED",
	                                     VI_FALSE, IVI_VAL_MULTI_CHANNEL,
	                                     auml24xxViBoolean_ReadCallback,
	                                     auml24xxViBoolean_WriteCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi,
	                                   AUML24XX_ATTR_POST_PROCESSING_FUNCTION_MODULE,
	                                   "AUML24XX_ATTR_POST_PROCESSING_FUNCTION_MODULE",
	                                   AUML24XX_VAL_POST_PROCESSING_STATS,
	                                   IVI_VAL_MULTI_CHANNEL,
	                                   auml24xxEnum_ReadCallback,
	                                   auml24xxEnum_WriteCallback, VI_NULL));
	checkErr (Ivi_SetAttrRangeTableCallback (vi,
	                                         AUML24XX_ATTR_POST_PROCESSING_FUNCTION_MODULE,
	                                         auml24xxAttrPostProcessingFunctionModule_RangeTableCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi,
	                                   AUML24XX_ATTR_STAT_POST_PROCESSING_FUNCTION_TYPE,
	                                   "AUML24XX_ATTR_STAT_POST_PROCESSING_FUNCTION_TYPE",
	                                   AUML24XX_VAL_PROCESSING_TYPE_PDF,
	                                   IVI_VAL_MULTI_CHANNEL,
	                                   auml24xxEnum_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrStatPostProcessingFunctionTypeRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi,
	                                    AUML24XX_ATTR_STAT_POST_PROCESSING_MARKER_POSITION,
	                                    "AUML24XX_ATTR_STAT_POST_PROCESSING_MARKER_POSITION",
	                                    -70.0,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                    auml24xxViReal64_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrStatPostProcessingMarkerPositionRangeTable,
	                                    0));
	checkErr (Ivi_AddAttributeViBoolean (vi,
	                                     AUML24XX_ATTR_STAT_POST_PROCESSING_MARKER_ENABLED,
	                                     "AUML24XX_ATTR_STAT_POST_PROCESSING_MARKER_ENABLED",
	                                     VI_FALSE, IVI_VAL_MULTI_CHANNEL,
	                                     auml24xxViBoolean_ReadCallback,
	                                     auml24xxViBoolean_WriteCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi,
	                                   AUML24XX_ATTR_STAT_POST_PROCESSING_SOURCE_SELECTION,
	                                   "AUML24XX_ATTR_STAT_POST_PROCESSING_SOURCE_SELECTION",
	                                   AUML24XX_VAL_STAT_POST_PROCESSING_SOURCE_CHANNEL,
	                                   IVI_VAL_MULTI_CHANNEL,
	                                   auml24xxEnum_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrStatPostProcessingSourceSlelectionRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_PAE_BIAS_CURRENT,
	                                    "AUML24XX_ATTR_PAE_BIAS_CURRENT", 0.01,
	                                    IVI_VAL_MULTI_CHANNEL,
	                                    auml24xxViReal64_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrPAEBiasCurrentValueRangeTable, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, AUML24XX_ATTR_PAE_BIAS_CURRENT,
	                                            auml24xxAttrPAEViReal64_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_PAE_BIAS_CURRENT_SOURCE,
	                                   "AUML24XX_ATTR_PAE_BIAS_CURRENT_SOURCE",
	                                   AUML24XX_VAL_PAE_BIAS_CURRENT_SOURCE_FIXED,
	                                   IVI_VAL_MULTI_CHANNEL,
	                                   auml24xxEnum_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrPAEBiasCurrentSourceRangeTable));
	checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi,
	                                           AUML24XX_ATTR_PAE_BIAS_CURRENT_SOURCE,
	                                           auml24xxAttrPAEViInt32_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, AUML24XX_ATTR_PAE_BIAS_VOLTAGE,
	                                    "AUML24XX_ATTR_PAE_BIAS_VOLTAGE", 10.0,
	                                    IVI_VAL_MULTI_CHANNEL,
	                                    auml24xxViReal64_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrPAEBiasVoltageRangeTable, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, AUML24XX_ATTR_PAE_BIAS_VOLTAGE,
	                                            auml24xxAttrPAEViReal64_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_PAE_INPUT_CONFIGURATION,
	                                   "AUML24XX_ATTR_PAE_INPUT_CONFIGURATION",
	                                   AUML24XX_VAL_PAE_INPUT_A_MINUS_B,
	                                   IVI_VAL_MULTI_CHANNEL,
	                                   auml24xxEnum_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrPAEInputConfigurationRangeTable));
	checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi,
	                                           AUML24XX_ATTR_PAE_INPUT_CONFIGURATION,
	                                           auml24xxAttrPAEViInt32_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_PAE_SOURCE_SELECTION,
	                                   "AUML24XX_ATTR_PAE_SOURCE_SELECTION",
	                                   AUML24XX_VAL_PAE_SOURCE_CHANNEL,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                   auml24xxEnum_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrPAESourceRangeTable));
	checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi,
	                                           AUML24XX_ATTR_PAE_SOURCE_SELECTION,
	                                           auml24xxAttrPAEViInt32_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi,
	                                    AUML24XX_ATTR_PAE_BIAS_CURRENT_CONVERSION_FACTOR,
	                                    "AUML24XX_ATTR_PAE_BIAS_CURRENT_CONVERSION_FACTOR",
	                                    0.01, IVI_VAL_MULTI_CHANNEL,
	                                    auml24xxViReal64_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrPaeBiasCurrentConversionFactorRangeTable,
	                                    0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi,
	                                            AUML24XX_ATTR_PAE_BIAS_CURRENT_CONVERSION_FACTOR,
	                                            auml24xxAttrPAEViReal64_CheckCallback));
	checkErr (Ivi_AddRepeatedAttributeViBoolean (vi, SENSOR_REP_CAP_NAME,
	                                             AUML24XX_ATTR_SENSOR_FILTER_ENABLED,
	                                             "AUML24XX_ATTR_SENSOR_FILTER_ENABLED",
	                                             VI_FALSE, 0,
	                                             auml24xxViBoolean_ReadCallback,
	                                             auml24xxViBoolean_WriteCallback));
	checkErr (Ivi_AddRepeatedAttributeViInt32 (vi, SENSOR_REP_CAP_NAME,
	                                           AUML24XX_ATTR_SENSOR_UNIVERSAL_OPERATION_MODE,
	                                           "AUML24XX_ATTR_SENSOR_UNIVERSAL_OPERATION_MODE",
	                                           AUML24XX_VAL_UNIVERSAL_SENSOR_TRMS,
	                                           IVI_VAL_NEVER_CACHE,
	                                           auml24xxEnum_ReadCallback,
	                                           auml24xxEnum_WriteCallback,
	                                           &attrSensorUniversalOperationModeRangeTable));
	checkErr (Ivi_AddRepeatedAttributeViInt32 (vi, SENSOR_REP_CAP_NAME,
	                                           AUML24XX_ATTR_SENSOR_RANGE_HOLD,
	                                           "AUML24XX_ATTR_SENSOR_RANGE_HOLD",
	                                           AUML24XX_VAL_RANGE_HOLD_AUTO,
	                                           IVI_VAL_NEVER_CACHE,
	                                           auml24xxEnum_ReadCallback,
	                                           auml24xxEnum_WriteCallback,
	                                           &attrSensorRangeHoldRangeTable));
	checkErr (Ivi_AddRepeatedAttributeViInt32 (vi, SENSOR_REP_CAP_NAME,
	                                           AUML24XX_ATTR_SENSOR_OFFSET_TYPE,
	                                           "AUML24XX_ATTR_SENSOR_OFFSET_TYPE",
	                                           AUML24XX_VAL_SENSOR_OFFSET_TYPE_OFF,
	                                           0, auml24xxEnum_ReadCallback,
	                                           auml24xxEnum_WriteCallback,
	                                           &attrSensorOffsetTypeRangeTable));
	checkErr (Ivi_AddRepeatedAttributeViInt32 (vi, SENSOR_REP_CAP_NAME,
	                                           AUML24XX_ATTR_SENSOR_OFFSET_TABLE_ACTIVE_NUMBER,
	                                           "AUML24XX_ATTR_SENSOR_OFFSET_TABLE_ACTIVE_NUMBER",
	                                           1, 0, auml24xxViInt32_ReadCallback,
	                                           auml24xxViInt32_WriteCallback,
	                                           &attrSensorOffsetTableActiveNumberRangeTable));
	checkErr (Ivi_AddAttributeViBoolean (vi, AUML24XX_ATTR_FAST_MODE_ENABLED,
	                                     "AUML24XX_ATTR_FAST_MODE_ENABLED", VI_FALSE,
	                                     0, auml24xxAttrFastModeEnabled_ReadCallback,
	                                     auml24xxViBoolean_WriteCallback));
	checkErr (Ivi_AddAttributeViBoolean (vi,
	                                     AUML24XX_ATTR_GPIB_RESPONSE_BUFFERING_ENABLED,
	                                     "AUML24XX_ATTR_GPIB_RESPONSE_BUFFERING_ENABLED",
	                                     VI_TRUE, 0,
	                                     auml24xxAttrGpibResponseBufferingEnabled_ReadCallback,
	                                     auml24xxViBoolean_WriteCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_BNC1_OUTPUT_MODE,
	                                   "AUML24XX_ATTR_BNC1_OUTPUT_MODE",
	                                   AUML24XX_VAL_OUTPUT_MODE_OFF,
	                                   IVI_VAL_NEVER_CACHE,
	                                   auml24xxAttrBnc1OutputMode_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrBNC1OutputModeRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_BNC2_OUTPUT_MODE,
	                                   "AUML24XX_ATTR_BNC2_OUTPUT_MODE",
	                                   AUML24XX_VAL_OUTPUT_MODE_OFF,
	                                   IVI_VAL_NEVER_CACHE,
	                                   auml24xxAttrBnc2OutputMode_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrBNC2OutputModeRangeTable));
	checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, BNC_REP_CAP_NAME,
	                                            AUML24XX_ATTR_BNC_DBW_ANALOG_OUTPUT_STOP,
	                                            "AUML24XX_ATTR_BNC_DBW_ANALOG_OUTPUT_STOP",
	                                            247.0, 0,
	                                            auml24xxAttrBncDbwAnalogOutputStop_ReadCallback,
	                                            auml24xxViReal64_WriteCallback,
	                                            &attrBNCDBWAnalogOutputRangeTable, 0));
	checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, BNC_REP_CAP_NAME,
	                                            AUML24XX_ATTR_BNC_DB_ANALOG_OUTPUT_START,
	                                            "AUML24XX_ATTR_BNC_DB_ANALOG_OUTPUT_START",
	                                            -270.0, 0,
	                                            auml24xxAttrBncDbAnalogOutputStart_ReadCallback,
	                                            auml24xxViReal64_WriteCallback,
	                                            &attrBNCDBAnalogOutputRangeTable, 0));
	checkErr (Ivi_AddRepeatedAttributeViInt32 (vi, BNC_REP_CAP_NAME,
	                                           AUML24XX_ATTR_BNC_OUTPUT_SOURCE_CHANNEL,
	                                           "AUML24XX_ATTR_BNC_OUTPUT_SOURCE_CHANNEL",
	                                           1, 0, auml24xxEnum_ReadCallback,
	                                           auml24xxEnum_WriteCallback,
	                                           &attrBNCOutputSourceChannelRangeTable));
	checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi,
	                                           AUML24XX_ATTR_BNC_OUTPUT_SOURCE_CHANNEL,
	                                           auml24xxAttrBncOutputSourceChannel_CheckCallback));
	checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, BNC_REP_CAP_NAME,
	                                            AUML24XX_ATTR_BNC_ANALOG_OUTPUT_STOP_VOLTAGE_SCALE,
	                                            "AUML24XX_ATTR_BNC_ANALOG_OUTPUT_STOP_VOLTAGE_SCALE",
	                                            5.0, 0,
	                                            auml24xxViReal64_ReadCallback,
	                                            auml24xxViReal64_WriteCallback,
	                                            &attrBNCAnalogOutputStopVoltageScaleRangeTable,
	                                            0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi,
	                                            AUML24XX_ATTR_BNC_ANALOG_OUTPUT_STOP_VOLTAGE_SCALE,
	                                            auml24xxAttrBncAnalogOutputStopVoltageScale_CheckCallback));
	checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, BNC_REP_CAP_NAME,
	                                            AUML24XX_ATTR_BNC_ANALOG_OUTPUT_START_VOLTAGE_SCALE,
	                                            "AUML24XX_ATTR_BNC_ANALOG_OUTPUT_START_VOLTAGE_SCALE",
	                                            0.0, 0,
	                                            auml24xxViReal64_ReadCallback,
	                                            auml24xxViReal64_WriteCallback,
	                                            &attrBNCAnalogOutputStartVoltageScaleRangeTable,
	                                            0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi,
	                                            AUML24XX_ATTR_BNC_ANALOG_OUTPUT_START_VOLTAGE_SCALE,
	                                            auml24xxAttrBncAnalogOutputStartVoltageScale_CheckCallback));
	checkErr (Ivi_AddRepeatedAttributeViInt32 (vi, BNC_REP_CAP_NAME,
	                                           AUML24XX_ATTR_BNC_PASS_VOLTAGE_LEVEL,
	                                           "AUML24XX_ATTR_BNC_PASS_VOLTAGE_LEVEL",
	                                           AUML24XX_VAL_BNC_PASS_VOLTAGE_LEVEL_HIGH,
	                                           0, auml24xxEnum_ReadCallback,
	                                           auml24xxEnum_WriteCallback,
	                                           &attrBNCPassVoltageLevelRangeTable));
	checkErr (Ivi_AddRepeatedAttributeViInt32 (vi, SENSOR_REP_CAP_NAME,
	                                           AUML24XX_ATTR_RANGE_CALIBRATOR_DIAGNOSTICS_TEST,
	                                           "AUML24XX_ATTR_RANGE_CALIBRATOR_DIAGNOSTICS_TEST",
	                                           AUML24XX_VAL_RANGE_CALIBRATOR_ZERO, 0,
	                                           auml24xxEnum_ReadCallback,
	                                           auml24xxEnum_WriteCallback,
	                                           &attrRangeCalibratorDiagnosticsTestRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi,
	                                    AUML24XX_ATTR_INSTR_PMOD_TRIGGER_CAPTURE_TIME,
	                                    "AUML24XX_ATTR_INSTR_PMOD_TRIGGER_CAPTURE_TIME",
	                                    4.6e-3,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE | IVI_VAL_HIDDEN,
	                                    auml24xxAttrInstrPMODTriggerCaptureTime_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrCaptureTimeRangeTable, 0));
	checkErr (Ivi_AddAttributeViReal64 (vi,
	                                    AUML24XX_ATTR_INSTR_CW_TRIGGER_CAPTURE_TIME,
	                                    "AUML24XX_ATTR_INSTR_CW_TRIGGER_CAPTURE_TIME",
	                                    4.6e-3,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE | IVI_VAL_HIDDEN,
	                                    auml24xxAttrInstrCWTriggerCaptureTime_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrCaptureTimeRangeTable, 0));
	checkErr (Ivi_AddAttributeViInt32 (vi,
	                                   AUML24XX_ATTR_INSTR_PMOD_TRIGGER_ARMING_MODE,
	                                   "AUML24XX_ATTR_INSTR_PMOD_TRIGGER_ARMING_MODE",
	                                   AUML24XX_VAL_TRIGGER_ARMING_MODE_AUTO,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE | IVI_VAL_HIDDEN,
	                                   auml24xxAttrInstrPMODTriggerArmingMode_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrPMODTriggerArmingModeRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi,
	                                   AUML24XX_ATTR_INSTR_CW_TRIGGER_ARMING_MODE,
	                                   "AUML24XX_ATTR_INSTR_CW_TRIGGER_ARMING_MODE",
	                                   AUML24XX_VAL_TRIGGER_ARMING_MODE_AUTO,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE | IVI_VAL_HIDDEN,
	                                   auml24xxAttrInstrCWTriggerArmingMode_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrCWTriggerArmingModeRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi,
	                                   AUML24XX_ATTR_INSTR_PMOD_TRIGGER_INTERNAL_SLOPE,
	                                   "AUML24XX_ATTR_INSTR_PMOD_TRIGGER_INTERNAL_SLOPE",
	                                   AUML24XX_VAL_POSITIVE,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_HIDDEN,
	                                   auml24xxAttrInstrPMODTriggerInternalSlope_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrInternalTriggerSlopeRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi,
	                                   AUML24XX_ATTR_INSTR_CW_TRIGGER_INTERNAL_SLOPE,
	                                   "AUML24XX_ATTR_INSTR_CW_TRIGGER_INTERNAL_SLOPE",
	                                   AUML24XX_VAL_POSITIVE,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_HIDDEN,
	                                   auml24xxAttrInstrCWTriggerInternalSlope_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrInternalTriggerSlopeRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi,
	                                    AUML24XX_ATTR_INSTR_PMOD_TRIGGER_INTERNAL_LEVEL,
	                                    "AUML24XX_ATTR_INSTR_PMOD_TRIGGER_INTERNAL_LEVEL",
	                                    0.0, IVI_VAL_MULTI_CHANNEL | IVI_VAL_HIDDEN,
	                                    auml24xxAttrInstrPMODTriggerInternalLevel_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrInternalTriggerLevelRangeTable, 0));
	checkErr (Ivi_AddAttributeViReal64 (vi,
	                                    AUML24XX_ATTR_INSTR_CW_TRIGGER_INTERNAL_LEVEL,
	                                    "AUML24XX_ATTR_INSTR_CW_TRIGGER_INTERNAL_LEVEL",
	                                    0.0, IVI_VAL_MULTI_CHANNEL | IVI_VAL_HIDDEN,
	                                    auml24xxAttrInstrCWTriggerInternalLevel_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrInternalTriggerLevelRangeTable, 0));
	checkErr (Ivi_AddAttributeViReal64 (vi,
	                                    AUML24XX_ATTR_INSTR_PMOD_TRIGGER_DELAY_TIME,
	                                    "AUML24XX_ATTR_INSTR_PMOD_TRIGGER_DELAY_TIME",
	                                    0.0,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE | IVI_VAL_HIDDEN,
	                                    auml24xxAttrInstrPMODTriggerDelayTime_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrInstrPMODTriggerDelayTimeRangeTable, 0));
	checkErr (Ivi_AddAttributeViReal64 (vi,
	                                    AUML24XX_ATTR_INSTR_CW_TRIGGER_DELAY_TIME,
	                                    "AUML24XX_ATTR_INSTR_CW_TRIGGER_DELAY_TIME",
	                                    0.0,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE | IVI_VAL_HIDDEN,
	                                    auml24xxAttrInstrCWTriggerDelayTime_ReadCallback,
	                                    auml24xxViReal64_WriteCallback,
	                                    &attrInstrCWTriggerDelayTimeRangeTable, 0));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_INSTR_CW_TRIGGER_SOURCE,
	                                   "AUML24XX_ATTR_INSTR_CW_TRIGGER_SOURCE",
	                                   AUML24XX_VAL_IMMEDIATE,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_HIDDEN,
	                                   auml24xxAttrInstrCWTriggerSource_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrTriggerSourceRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_INSTR_PMOD_TRIGGER_SOURCE,
	                                   "AUML24XX_ATTR_INSTR_PMOD_TRIGGER_SOURCE",
	                                   AUML24XX_VAL_IMMEDIATE,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_HIDDEN,
	                                   auml24xxAttrInstrPMODTriggerSource_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrTriggerSourceRangeTable));
	checkErr (Ivi_AddAttributeViBoolean (vi,
	                                     AUML24XX_ATTR_INSTR_CW_AVERAGING_ENABLED,
	                                     "AUML24XX_ATTR_INSTR_CW_AVERAGING_ENABLED",
	                                     VI_TRUE,
	                                     IVI_VAL_MULTI_CHANNEL | IVI_VAL_HIDDEN,
	                                     auml24xxAttrInstrCWAveragingEnabled_ReadCallback,
	                                     auml24xxAttrInstrCWAveragingEnabled_WriteCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_INSTR_CW_AVERAGING_NUMBER,
	                                   "AUML24XX_ATTR_INSTR_CW_AVERAGING_NUMBER", 1,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_HIDDEN,
	                                   auml24xxAttrInstrCWAveragingNumber_ReadCallback,
	                                   auml24xxAttrInstrCWAveragingNumber_WriteCallback,
	                                   &attrAveragingCountRangeTable));
	checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi,
	                                           AUML24XX_ATTR_INSTR_CW_AVERAGING_NUMBER,
	                                           auml24xxAttrInstrCWAveragingNumber_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_INSTR_PMOD_AVERAGING_NUMBER,
	                                   "AUML24XX_ATTR_INSTR_PMOD_AVERAGING_NUMBER",
	                                   64, IVI_VAL_MULTI_CHANNEL | IVI_VAL_HIDDEN,
	                                   auml24xxViInt32_ReadCallback,
	                                   auml24xxViInt32_WriteCallback,
	                                   &attrAveragingCountRangeTable));
	checkErr (Ivi_AddAttributeViBoolean (vi,
	                                     AUML24XX_ATTR_INSTR_PMOD_AVERAGING_ENABLED,
	                                     "AUML24XX_ATTR_INSTR_PMOD_AVERAGING_ENABLED",
	                                     VI_FALSE,
	                                     IVI_VAL_MULTI_CHANNEL | IVI_VAL_HIDDEN,
	                                     auml24xxViBoolean_ReadCallback,
	                                     auml24xxViBoolean_WriteCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_INSTR_MODEL_TYPE,
	                                   "AUML24XX_ATTR_INSTR_MODEL_TYPE",
	                                   AUML24XX_VAL_MODEL_ML2487A, IVI_VAL_HIDDEN,
	                                   VI_NULL, VI_NULL,
	                                   &attrInstrModelTypeRangeTable));
	checkErr( Ivi_SetAttributeViInt32 (vi, VI_NULL, AUML24XX_ATTR_INSTR_MODEL_TYPE, 
	                                   0, model));                         
	checkErr (Ivi_AddAttributeViInt32 (vi,
	                                   AUML24XX_ATTR_LIMIT_COMPLEX_SPECIFICATION_ACTIVE_NUMBER,
	                                   "AUML24XX_ATTR_LIMIT_COMPLEX_SPECIFICATION_ACTIVE_NUMBER",
	                                   1, IVI_VAL_MULTI_CHANNEL,
	                                   auml24xxAttrLimitComplexSpecificationActiveNumber_ReadCallback,
	                                   auml24xxAttrLimitComplexSpecificationActiveNumber_WriteCallback,
	                                   VI_NULL));
	checkErr (Ivi_SetAttrRangeTableCallback (vi,
	                                         AUML24XX_ATTR_LIMIT_COMPLEX_SPECIFICATION_ACTIVE_NUMBER,
	                                         auml24xxAttrLimitComplexSpecificationActiveNumber_RangeTableCallback));
	checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, BNC_REP_CAP_NAME,
	                                            AUML24XX_ATTR_BNC_DBW_ANALOG_OUTPUT_START,
	                                            "AUML24XX_ATTR_BNC_DBW_ANALOG_OUTPUT_START",
	                                            -283.0, 0,
	                                            auml24xxAttrBncDbwAnalogOutputStart_ReadCallback,
	                                            auml24xxViReal64_WriteCallback,
	                                            &attrBNCDBWAnalogOutputRangeTable, 0));
	checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, BNC_REP_CAP_NAME,
	                                            AUML24XX_ATTR_BNC_DBUV_ANALOG_OUTPUT_START,
	                                            "AUML24XX_ATTR_BNC_DBUV_ANALOG_OUTPUT_START",
	                                            -163.0, 0,
	                                            auml24xxAttrBncDbuvAnalogOutputStart_ReadCallback,
	                                            auml24xxViReal64_WriteCallback,
	                                            &attrBNCDBUVAnalogOutputRangeTable,
	                                            0));
	checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, BNC_REP_CAP_NAME,
	                                            AUML24XX_ATTR_BNC_DBMV_ANALOG_OUTPUT_START,
	                                            "AUML24XX_ATTR_BNC_DBMV_ANALOG_OUTPUT_START",
	                                            -223.0, 0,
	                                            auml24xxAttrBncDbmvAnalogOutputStart_ReadCallback,
	                                            auml24xxViReal64_WriteCallback,
	                                            &attrBNCDBMVAnalogOutputRangeTable,
	                                            0));
	checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, BNC_REP_CAP_NAME,
	                                            AUML24XX_ATTR_BNC_WATTS_ANALOG_OUTPUT_START,
	                                            "AUML24XX_ATTR_BNC_WATTS_ANALOG_OUTPUT_START",
	                                            0.0, 0,
	                                            auml24xxAttrBncWattsAnalogOutputStart_ReadCallback,
	                                            auml24xxViReal64_WriteCallback,
	                                            &attrBNCWattsAnalogOutputRangeTable,
	                                            0));
	checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, BNC_REP_CAP_NAME,
	                                            AUML24XX_ATTR_BNC_DBM_ANALOG_OUTPUT_STOP,
	                                            "AUML24XX_ATTR_BNC_DBM_ANALOG_OUTPUT_STOP",
	                                            260.0, 0,
	                                            auml24xxAttrBncDbmAnalogOutputStop_ReadCallback,
	                                            auml24xxViReal64_WriteCallback,
	                                            &attrBNCDBMAnalogOutputRangeTable, 0));
	checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, BNC_REP_CAP_NAME,
	                                            AUML24XX_ATTR_BNC_DBUV_ANALOG_OUTPUT_STOP,
	                                            "AUML24XX_ATTR_BNC_DBUV_ANALOG_OUTPUT_STOP",
	                                            367.0, 0,
	                                            auml24xxAttrBncDbuvAnalogOutputStop_ReadCallback,
	                                            auml24xxViReal64_WriteCallback,
	                                            &attrBNCDBUVAnalogOutputRangeTable,
	                                            0));
	checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, BNC_REP_CAP_NAME,
	                                            AUML24XX_ATTR_BNC_DBMV_ANALOG_OUTPUT_STOP,
	                                            "AUML24XX_ATTR_BNC_DBMV_ANALOG_OUTPUT_STOP",
	                                            307.0, 0,
	                                            auml24xxAttrBncDbmvAnalogOutputStop_ReadCallback,
	                                            auml24xxViReal64_WriteCallback,
	                                            &attrBNCDBMVAnalogOutputRangeTable,
	                                            0));
	checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, BNC_REP_CAP_NAME,
	                                            AUML24XX_ATTR_BNC_WATTS_ANALOG_OUTPUT_STOP,
	                                            "AUML24XX_ATTR_BNC_WATTS_ANALOG_OUTPUT_STOP",
	                                            1e9, 0,
	                                            auml24xxAttrBncWattsAnalogOutputStop_ReadCallback,
	                                            auml24xxViReal64_WriteCallback,
	                                            &attrBNCWattsAnalogOutputRangeTable,
	                                            0));
	checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, BNC_REP_CAP_NAME,
	                                            AUML24XX_ATTR_BNC_DB_ANALOG_OUTPUT_STOP,
	                                            "AUML24XX_ATTR_BNC_DB_ANALOG_OUTPUT_STOP",
	                                            260.0, 0,
	                                            auml24xxAttrBncDbAnalogOutputStop_ReadCallback,
	                                            auml24xxViReal64_WriteCallback,
	                                            &attrBNCDBAnalogOutputRangeTable, 0));
	checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, BNC_REP_CAP_NAME,
	                                            AUML24XX_ATTR_BNC_DBM_ANALOG_OUTPUT_START,
	                                            "AUML24XX_ATTR_BNC_DBM_ANALOG_OUTPUT_START",
	                                            -270.0, 0,
	                                            auml24xxAttrBncDbmAnalogOutputStart_ReadCallback,
	                                            auml24xxViReal64_WriteCallback,
	                                            &attrBNCDBMAnalogOutputRangeTable, 0));
	checkErr (Ivi_AddAttributeViString (vi,
	                                    AUML24XX_ATTR_INSTR_PMOD_INTERNAL_TRIGGER_EVENT_SOURCE,
	                                    "AUML24XX_ATTR_INSTR_PMOD_INTERNAL_TRIGGER_EVENT_SOURCE",
	                                    "A", IVI_VAL_MULTI_CHANNEL | IVI_VAL_HIDDEN,
	                                    auml24xxAttrInstrInternalTriggerEventSource_ReadCallback,
	                                    auml24xxAttrInstrInternalTriggerEventSource_WriteCallback));
	checkErr (Ivi_SetAttrCheckCallbackViString (vi,
	                                            AUML24XX_ATTR_INSTR_PMOD_INTERNAL_TRIGGER_EVENT_SOURCE,
	                                            auml24xxAttrInstrInternalTriggerEventSource_CheckCallback));
	checkErr (Ivi_AddAttributeViString (vi,
	                                    AUML24XX_ATTR_INSTR_CW_INTERNAL_TRIGGER_EVENT_SOURCE,
	                                    "AUML24XX_ATTR_INSTR_CW_INTERNAL_TRIGGER_EVENT_SOURCE",
	                                    "A", IVI_VAL_MULTI_CHANNEL | IVI_VAL_HIDDEN,
	                                    auml24xxAttrInstrInternalTriggerEventSource_ReadCallback,
	                                    auml24xxAttrInstrInternalTriggerEventSource_WriteCallback));
	checkErr (Ivi_AddAttributeViBoolean (vi, AUML24XX_ATTR_AVERAGING_ENABLED,
	                                     "AUML24XX_ATTR_AVERAGING_ENABLED", VI_FALSE,
	                                     IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION,
	                                     auml24xxAttrAveragingEnabled_ReadCallback,
	                                     auml24xxAttrAveragingEnabled_WriteCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_ACTIVE_CHANNEL,
	                                   "AUML24XX_ATTR_ACTIVE_CHANNEL", 1,
	                                   IVI_VAL_NEVER_CACHE,
	                                   auml24xxAttrActiveChannel_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrActiveChannelRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_RRS_TRACE_STATE,
	                                   "AUML24XX_ATTR_RRS_TRACE_STATE",
	                                   AUML24XX_VAL_RRS_FALSE,
	                                   IVI_VAL_NOT_USER_WRITABLE | IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                   auml24xxEnum_ReadCallback, VI_NULL,
	                                   &attrRrsTraceStateRangeTable));
	checkErr (Ivi_AddRepeatedAttributeViInt32 (vi, SENSOR_REP_CAP_NAME,
	                                           AUML24XX_ATTR_ACTIVE_CALIBRATION_FACTOR_NUMBER,
	                                           "AUML24XX_ATTR_ACTIVE_CALIBRATION_FACTOR_NUMBER",
	                                           1, IVI_VAL_NEVER_CACHE,
	                                           auml24xxAttrActiveCalibrationFactorNumber_ReadCallback,
	                                           auml24xxAttrActiveCalibrationFactorNumber_WriteCallback,
	                                           &attrActiveCalibrationFactorNumberRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_CHANNEL_COUNTS,
	                                   "AUML24XX_ATTR_CHANNEL_COUNTS", 1,
	                                   IVI_VAL_NOT_USER_WRITABLE | IVI_VAL_ALWAYS_CACHE,
	                                   VI_NULL, VI_NULL, &attrChannelCountRangeTable));
	checkErr (Ivi_AddAttributeViBoolean (vi, AUML24XX_ATTR_DUAL_CHANNEL_MODE_ENABLED,
	                                     "AUML24XX_ATTR_DUAL_CHANNEL_MODE_ENABLED",
	                                     VI_FALSE, 0,
	                                     auml24xxAttrDualChannelModeEnabled_ReadCallback,
	                                     auml24xxAttrDualChannelModeEnabled_WriteCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, AUML24XX_ATTR_PMOD_MEASUREMENT_DISPLAY,
	                                   "AUML24XX_ATTR_PMOD_MEASUREMENT_DISPLAY",
	                                   AUML24XX_VAL_PMOD_PRF,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                   auml24xxEnum_ReadCallback,
	                                   auml24xxEnum_WriteCallback,
	                                   &attrPmodMeasurementDisplayRangeTable));
	checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, SENSOR_REP_CAP_NAME,
	                                            AUML24XX_ATTR_SENSOR_CORRECTION_FREQUENCY,
	                                            "AUML24XX_ATTR_SENSOR_CORRECTION_FREQUENCY",
	                                            50.00e6, 0,
	                                            auml24xxViReal64_ReadCallback,
	                                            auml24xxViReal64_WriteCallback,
	                                            &attrCorrectionFrequencyRangeTable,
	                                            0));
	checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, SENSOR_REP_CAP_NAME,
	                                            AUML24XX_ATTR_SENSOR_OFFSET,
	                                            "AUML24XX_ATTR_SENSOR_OFFSET", 0.0,
	                                            0, auml24xxViReal64_ReadCallback,
	                                            auml24xxViReal64_WriteCallback,
	                                            &attrOffsetRangeTable, 0));
	checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi, AUML24XX_ATTR_ACTIVE_CHANNEL,
	                                           auml24xxAttrActiveChannel_CheckCallback));
	checkErr (Ivi_SetAttrCheckCallbackViString (vi,
	                                            AUML24XX_ATTR_INSTR_CW_INTERNAL_TRIGGER_EVENT_SOURCE,
	                                            auml24xxAttrInstrInternalTriggerEventSource_CheckCallback));

        /*- Setup attribute invalidations -----------------------------------*/            

	checkErr (Ivi_AddAttributeInvalidation (vi, AUML24XX_ATTR_AVERAGING_AUTO_ENABLED,
	                                        AUML24XX_ATTR_CW_AVERAGING_MODE, VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi, AUML24XX_ATTR_SAMPLE_RATE,
	                                        AUML24XX_ATTR_INSTR_PMOD_TRIGGER_CAPTURE_TIME,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi, AUML24XX_ATTR_SAMPLE_RATE,
	                                        AUML24XX_ATTR_INSTR_CW_TRIGGER_CAPTURE_TIME,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi, AUML24XX_ATTR_TRIGGER_SOURCE,
	                                        AUML24XX_ATTR_INSTR_PMOD_INTERNAL_TRIGGER_EVENT_SOURCE,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi, AUML24XX_ATTR_TRIGGER_SOURCE,
	                                        AUML24XX_ATTR_INSTR_CW_INTERNAL_TRIGGER_EVENT_SOURCE,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi,
	                                        AUML24XX_ATTR_INTERNAL_TRIGGER_EVENT_SOURCE,
	                                        AUML24XX_ATTR_INSTR_CW_TRIGGER_SOURCE,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi,
	                                        AUML24XX_ATTR_INTERNAL_TRIGGER_EVENT_SOURCE,
	                                        AUML24XX_ATTR_INSTR_PMOD_TRIGGER_SOURCE,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi,
	                                        AUML24XX_ATTR_INSTR_PMOD_AVERAGING_ENABLED,
	                                        AUML24XX_ATTR_INSTR_CW_AVERAGING_NUMBER,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi, AUML24XX_ATTR_AVERAGING_ENABLED,
	                                        AUML24XX_ATTR_AVERAGING_AUTO_ENABLED,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi, AUML24XX_ATTR_AVERAGING_ENABLED,
	                                        AUML24XX_ATTR_CW_AVERAGING_MODE, VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi, AUML24XX_ATTR_AVERAGING_AUTO_ENABLED,
	                                        AUML24XX_ATTR_INSTR_CW_AVERAGING_ENABLED,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi, AUML24XX_ATTR_AVERAGING_AUTO_ENABLED,
	                                        AUML24XX_ATTR_INSTR_CW_AVERAGING_NUMBER,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi, AUML24XX_ATTR_AVERAGING_AUTO_ENABLED,
	                                        AUML24XX_ATTR_INSTR_PMOD_AVERAGING_NUMBER,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi, AUML24XX_ATTR_AVERAGING_AUTO_ENABLED,
	                                        AUML24XX_ATTR_INSTR_PMOD_AVERAGING_ENABLED,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi, AUML24XX_ATTR_AVERAGING_ENABLED,
	                                        AUML24XX_ATTR_INSTR_CW_AVERAGING_NUMBER,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi, AUML24XX_ATTR_AVERAGING_ENABLED,
	                                        AUML24XX_ATTR_INSTR_PMOD_AVERAGING_NUMBER,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi,
	                                        AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_UNITS,
	                                        AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_ADJUST,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi,
	                                        AUML24XX_ATTR_LIMIT_COMPLEX_SPECIFICATION_TYPE,
	                                        AUML24XX_ATTR_LIMIT_COMPLEX_SPECIFICATION_ACTIVE_NUMBER,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi,
	                                        AUML24XX_ATTR_LIMIT_COMPLEX_SPECIFICATION_ACTIVE_NUMBER,
	                                        AUML24XX_ATTR_LIMIT_COMPLEX_SPECIFICATION_TYPE,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi,
	                                        AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_UNITS,
	                                        AUML24XX_ATTR_SENSOR_CALIBRATION_MANUAL_FACTOR,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi,
	                                        AUML24XX_ATTR_BNC_DB_ANALOG_OUTPUT_START,
	                                        AUML24XX_ATTR_BNC_DBM_ANALOG_OUTPUT_START,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi,
	                                        AUML24XX_ATTR_BNC_DBM_ANALOG_OUTPUT_STOP,
	                                        AUML24XX_ATTR_BNC_DB_ANALOG_OUTPUT_STOP,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi,
	                                        AUML24XX_ATTR_BNC_DB_ANALOG_OUTPUT_STOP,
	                                        AUML24XX_ATTR_BNC_DBM_ANALOG_OUTPUT_STOP,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi,
	                                        AUML24XX_ATTR_BNC_DBM_ANALOG_OUTPUT_START,
	                                        AUML24XX_ATTR_BNC_DB_ANALOG_OUTPUT_START,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi, AUML24XX_ATTR_CW_AVERAGING_MODE,
	                                        AUML24XX_ATTR_AVERAGING_AUTO_ENABLED,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi, AUML24XX_ATTR_CW_AVERAGING_MODE,
	                                        AUML24XX_ATTR_INSTR_CW_AVERAGING_ENABLED,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi, AUML24XX_ATTR_CW_AVERAGING_MODE,
	                                        AUML24XX_ATTR_INSTR_CW_AVERAGING_NUMBER,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi, AUML24XX_ATTR_CW_AVERAGING_MODE,
	                                        AUML24XX_ATTR_INSTR_PMOD_AVERAGING_NUMBER,
	                                        VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi, AUML24XX_ATTR_CW_AVERAGING_MODE,
	                                        AUML24XX_ATTR_INSTR_PMOD_AVERAGING_ENABLED,
	                                        VI_TRUE));
Error:
    return error;
}

/*****************************************************************************
 *------------------- End Instrument Driver Source Code ---------------------*
 *****************************************************************************/
