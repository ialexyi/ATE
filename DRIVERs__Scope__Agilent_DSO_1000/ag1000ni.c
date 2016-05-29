/***************************************************************************** 
 * 2009 - 2010, National Instruments, Corporation.  All Rights Reserved.     * 
 *****************************************************************************/

/*****************************************************************************
 *  Agilent 1000 Series Oscilloscopes Instrument Driver                               
 *  LabWindows/CVI Instrument Driver                                     
 *  Original Release: Monday, September 22, 2008                                  
 *  By: Clark Li, National Instruments                              
 *      PH. (86-21) 5050 9810   Fax (86-21) 6879 5616                              
 *                                                                           
 *  Modification History:                                                    
 *                                                                           
 *       Monday, September 22, 2008 - Instrument Driver Created.   
 *       10/30/2009 - [cli] Fixed CAR 175254. Renamed ConfigureNumberOfAverages 
 *                          to ConfigureNumAverages
 *       
 *       01/18/2010 - [lei liu] Fixed CAR 202336. Remove the delay code from 
 *                              the source file.   
 *
 *       05/05/2010 - [czhou] Porting driver to 64-bits.
 *
 *       05/06/2010 - [czhou] 1. Move AG1000NI_ATTR_PROBE_ATTENUATION setting 
 *                               at the beginning of any other settings in 
 *                               function ag1000ni_ConfigureChannel, to avoid 
 *                               a mismatch between the setting values and 
 *                               setting results of Y Scale.
 *                            2. Update the waveform calculation process in 
 *                               function ag1000ni_FetchWaveform.
 *
 *       05/11/2010 - [czhou] 1. Update the range table of attribute
 *                               AG1000NI_ATTR_HORZ_MIN_NUM_PTS to 1 ~ 10240. 
 *                            2. Update the AG1000NI_NUM_HORZ_DIVS values.
 *****************************************************************************/
#include <string.h>
#include <stdio.h>  
#include <formatio.h>
#include <utility.h>
#include <ansi_c.h>
#include "ag1000ni.h"

/*****************************************************************************
 *--------------------- Hidden Attribute Declarations -----------------------*
 *****************************************************************************/

#define AG1000NI_ATTR_OPC_TIMEOUT                                      (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 1L)  /* ViInt32 */
#define AG1000NI_ATTR_USE_SPECIFIC_SIMULATION                          (IVI_INHERENT_ATTR_BASE + 23L)         /* ViBoolean */
#define AG1000NI_ATTR_VISA_RM_SESSION                                  (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 5L)  /* ViSession */
#define AG1000NI_ATTR_IO_SESSION                                       (IVI_INHERENT_ATTR_BASE + 322L)        /* ViSession */
#define AG1000NI_ATTR_OPC_CALLBACK                                     (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 6L)  /* ViAddr */
#define AG1000NI_ATTR_CHECK_STATUS_CALLBACK                            (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 7L)  /* ViAddr */
#define AG1000NI_ATTR_USER_INTERCHANGE_CHECK_CALLBACK                  (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 8L)  /* ViAddr */
#define AG1000NI_ATTR_INSTR_EDGE_TRIGGER_SOURCE                        (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 9L)  /* ViString */
#define AG1000NI_ATTR_INSTR_EDGE_TRIGGER_MODIFIER                      (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 10L) /* ViInt32 */
#define AG1000NI_ATTR_INSTR_TV_TRIGGER_SOURCE                          (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 11L) /* ViString */
#define AG1000NI_ATTR_INSTR_TV_TRIGGER_MODIFIER                        (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 12L) /* ViInt32 */
#define AG1000NI_ATTR_INSTR_PATTERN_TRIGGER_SOURCE                     (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 13L) /* ViString */
#define AG1000NI_ATTR_INSTR_PATTERN_TRIGGER_MODIFIER                   (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 14L) /* ViInt32 */
#define AG1000NI_ATTR_INSTR_PULSE_TRIGGER_SOURCE                       (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 15L) /* ViString */
#define AG1000NI_ATTR_INSTR_PULSE_TRIGGER_MODIFIER                     (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 16L) /* ViInt32 */
#define AG1000NI_ATTR_INSTR_MODEL_TYPE                                 (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 21L) /* ViInt32 */

/*****************************************************************************
 *---------------------------- Useful Macros --------------------------------*
 *****************************************************************************/

    /*- I/O buffer size -----------------------------------------------------*/
#define BUFFER_SIZE                             512L        

    /*- 488.2 Event Status Register (ESR) Bits ------------------------------*/
#define IEEE_488_2_ERROR_MASK                   0x3C                   

    /*- Lists of channels passed to the Ivi_BuildChannelTable function ------*/
#define TWO_CHANNEL_LIST                            "CHAN1,CHAN2"       
#define FOUR_CHANNEL_LIST                           "CHAN1,CHAN2,CHAN3,CHAN4"       
    /*- Lists of repated capitablities passed to the Ivi_BuildRepCapTable function ------*/ 

#define ALTERNATION_TRIGGER_REP_CAP_NAME                    "ALTERNATION_TRIGGER"
#define ALTERNATION_TRIGGER_LIST                            "SOURA,SOURB"       

    /* Number of horizontal divisions */
#define AG1000NI_NUM_HORZ_DIVS (12)

    /* Number of vertical divisions */
#define AG1000NI_NUM_VER_DIVS (8)

    /*- Default definition of ID Query Response for simulation --------------*/
#define AG1000NI_SIMULATION_ID_QUERY                     

    /*- Macros for access to enum range table -----------------------------------*/
#define ag1000ni_GetCmdFromIntValue(value, table, cmd) \
        Ivi_GetViInt32EntryFromValue (value, table, VI_NULL, VI_NULL,\
                                      VI_NULL, VI_NULL, cmd, VI_NULL)

#define ag1000ni_GetValueFromCmd(buffer, table, value) \
        Ivi_GetViInt32EntryFromString (buffer, table, value, VI_NULL,\
        VI_NULL, VI_NULL, VI_NULL)  

/*- Macros for the wrapper of empty channel name */
#define VALID_CHANNELNAME(channelName) \
        (VI_NULL == channelName ? "" : channelName)



/*****************************************************************************/
/*= Typedefs for string-value-revision tables. ========= */
/*****************************************************************************/
typedef struct { ViInt32       value;        /* Attribute ID */ 
                 ViConstString string;       /* Command */
                 ViConstString repCommand;   /* Extension for Reapted Command */
} ag1000niStringValueEntry;

typedef ag1000niStringValueEntry ag1000niStringValueTable[];

static ag1000niStringValueTable ag1000niEnum_commands = 
{   /* Attribute ID,                                   SCPI command          */
    {AG1000NI_ATTR_ACQUISITION_TYPE                    , ":ACQ:TYPE"           ,      ""},
    {AG1000NI_ATTR_SAMPLE_MODE                         , ":ACQ:MODE"           ,      ""},
    {AG1000NI_ATTR_VERTICAL_COUPLING                   , ":"                   , ":COUP"},
    {AG1000NI_ATTR_TRIGGER_TYPE                        , ":TRIG:MODE"          ,      ""},
    {AG1000NI_ATTR_INSTR_EDGE_TRIGGER_MODIFIER         , ":TRIG:EDGE:SWE"      ,      ""},
    {AG1000NI_ATTR_INSTR_TV_TRIGGER_MODIFIER           , ":TRIG:VIDEO:SWE"     ,      ""},
    {AG1000NI_ATTR_INSTR_PULSE_TRIGGER_MODIFIER        , ":TRIG:PULS:SWE"      ,      ""},
    {AG1000NI_ATTR_INSTR_PATTERN_TRIGGER_MODIFIER      , ":TRIG:PATT:SWE"      ,      ""},
    {AG1000NI_ATTR_TRIGGER_COUPLING                    , ":TRIG:COUP"          ,      ""},
    {AG1000NI_ATTR_TRIGGER_SLOPE                       , ":TRIG:EDGE:SLOP"     ,      ""},
    {AG1000NI_ATTR_TV_TRIGGER_SIGNAL_FORMAT            , ":TRIG:VIDEO:STAN"    ,      ""},
    {AG1000NI_ATTR_TV_TRIGGER_EVENT                    , ":TRIG:VIDEO:MODE"    ,      ""},
    {AG1000NI_ATTR_TV_TRIGGER_POLARITY                 , ":TRIG:VIDEO:POL"     ,      ""},
    {AG1000NI_ATTR_PULSE_TRIGGER_MODE                  , ":TRIG:PULS:MODE"     ,      ""},
    {AG1000NI_ATTR_ALTERNATION_TRIGGER_SOURCE          , ":TRIG:ALT:SOUR"      ,      ""},
    {AG1000NI_ATTR_ALTERNATION_TRIGGER_TYPE            , ":TRIG:ALT:TYPE"      ,      ""},
    {AG1000NI_ATTR_ALTERNATION_TRIGGER_COUPLING        , ":TRIG:ALT:COUP"      ,      ""},
    {AG1000NI_ATTR_ALTERNATION_PULSE_TRIGGER_MODE      , ":TRIG:ALT:PULS:MODE" ,      ""},
    {AG1000NI_ATTR_ALTERNATION_EDGE_TRIGGER_SLOPE      , ":TRIG:ALT:EDGE:SLOP" ,      ""},
    {AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_EVENT        , ":TRIG:ALT:VIDEO:MODE",      ""},
    {AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_POLARITY     , ":TRIG:ALT:VIDEO:POL" ,      ""},
    {AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_SIGNAL_FORMAT, ":TRIG:ALT:VIDEO:STAN",      ""},
    {VI_NULL                                           , VI_NULL               , VI_NULL}
};

static ag1000niStringValueTable ag1000niViString_commands = 
{   /* Attribute ID,                             SCPI command          */
    {AG1000NI_ATTR_INSTR_EDGE_TRIGGER_SOURCE   , ":TRIG:EDGE:SOUR" ,      ""},
    {AG1000NI_ATTR_INSTR_TV_TRIGGER_SOURCE     , ":TRIG:VIDEO:SOUR",      ""},
    {AG1000NI_ATTR_INSTR_PULSE_TRIGGER_SOURCE  , ":TRIG:PULS:SOUR" ,      ""},
    {AG1000NI_ATTR_INSTR_PATTERN_TRIGGER_SOURCE, ":TRIG:PATT:SOUR" ,      ""},
    {AG1000NI_ATTR_PATTERN_TRIGGER_PATTERN     , ":TRIG:PATT:PATT" ,      ""},
    {VI_NULL                                   , VI_NULL           , VI_NULL}
};

static ag1000niStringValueTable ag1000niViBoolean_commands = 
{   /* Attribute ID,                        SCPI command          */
    {AG1000NI_ATTR_CHANNEL_ENABLED          , ":"         , ":DISP"},
    {AG1000NI_ATTR_INPUT_INVERSION_ENABLED  , ":"         ,  ":INV"},
    {AG1000NI_ATTR_DIGITAL_FILTER_ENABLED   , ":"         , ":FILT"},
    {AG1000NI_ATTR_TRIGGER_HF_REJECT_ENABLED, ":TRIG:HFRE",      ""},
    {VI_NULL                                , VI_NULL     , VI_NULL}
};

static ag1000niStringValueTable ag1000niViReal64_commands = 
{   /* Attribute ID,                                         SCPI command          */
    {AG1000NI_ATTR_ALTERNATION_TRIGGER_HORZ_TIME_PER_RECORD  , ":TRIG:ALT:TSCAL"    ,       ""},
    {AG1000NI_ATTR_HORZ_TIME_PER_RECORD                      , ":TIM:SCAL"          ,       ""},
    {AG1000NI_ATTR_ALTERNATION_TRIGGER_ACQUISITION_START_TIME, ":TRIG:ALT:TOFFS"    ,       ""},
    {AG1000NI_ATTR_ACQUISITION_START_TIME                    , ":TIM:OFFS"          ,       ""},
    {AG1000NI_ATTR_HORZ_SAMPLE_RATE                          , ":ACQ:SAMP"          ,       ""},
    {AG1000NI_ATTR_TRIGGER_HOLDOFF                           , ":TRIG:HOLD"         ,       ""},
    {AG1000NI_ATTR_PULSE_TRIGGER_WIDTH                       , ":TRIG:PULS:WIDT"    ,       ""},
    {AG1000NI_ATTR_ALTERNATION_TRIGGER_HOLDOFF               , ":TRIG:ALT:HOLD"     ,       ""},
    {AG1000NI_ATTR_ALTERNATION_PULSE_TRIGGER_WIDTH           , ":TRIG:ALT:PULS:TIME",       ""},
    {AG1000NI_ATTR_VERTICAL_RANGE                            , ":"                  , ":SCALE"},
    {AG1000NI_ATTR_VERTICAL_OFFSET                           , ":"                  ,  ":OFFS"},
    {AG1000NI_ATTR_TRIGGER_LEVEL                             , ":TRIG:EDGE:LEV"     ,       ""},
    {AG1000NI_ATTR_TV_TRIGGER_LEVEL                          , ":TRIG:VIDEO:LEV"    ,       ""},
    {AG1000NI_ATTR_PATTERN_TRIGGER_LEVEL                     , ":TRIG:PATT:LEV"     ,       ""},
    {AG1000NI_ATTR_PULSE_TRIGGER_LEVEL                       , ":TRIG:PULS:LEV"     ,       ""},
    {AG1000NI_ATTR_ALTERNATION_EDGE_TRIGGER_LEVEL            , ":TRIG:ALT:LEV"      ,       ""},
    {AG1000NI_ATTR_TRIGGER_SENSITIVITY                       , ":TRIG:SENS"         ,       ""},
    {AG1000NI_ATTR_ALTERNATION_TRIGGER_SENSITIVITY           , ":TRIG:ALT:SENS"     ,       ""},
    {VI_NULL                                                 , VI_NULL              ,  VI_NULL}
};

static ag1000niStringValueTable ag1000niViInt32_commands = 
{   /* Attribute ID,                                 SCPI command          */
    {AG1000NI_ATTR_NUM_AVERAGES                      , ":ACQ:AVER"           ,      ""},
    {AG1000NI_ATTR_HORZ_MIN_NUM_PTS                  , ":WAV:POIN"           ,      ""},
    {AG1000NI_ATTR_TV_TRIGGER_LINE_NUMBER            , ":TRIG:VIDEO:LINE"    ,      ""},
    {AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_LINE_NUMBER, ":TRIG:ALT:VIDEO:LINE",      ""},
    {VI_NULL                                         , VI_NULL               , VI_NULL}
};

/*****************************************************************************
 *----------------- Value Definition and Range Tables -----------------------*
 *****************************************************************************/
static IviRangeTableEntry attrInstrModelRangeTableEntries[] =
    {
        {AG1000NI_VAL_DSO1002A, 0, 0, "DSO1002A", 0},
        {AG1000NI_VAL_DSO1004A, 0, 0, "DSO1004A", 0},
        {AG1000NI_VAL_DSO1012A, 0, 0, "DSO1012A", 0},
        {AG1000NI_VAL_DSO1014A, 0, 0, "DSO1014A", 0},
        {AG1000NI_VAL_DSO1022A, 0, 0, "DSO1022A", 0},
        {AG1000NI_VAL_DSO1024A, 0, 0, "DSO1024A", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrInstrModelRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrInstrModelRangeTableEntries,
    };

static IviRangeTableEntry attrHorzMinNumPtsRangeTableEntries[] =
    {
        {1, 10240, 20480, "", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrHorzMinNumPtsRangeTable =
    {
        IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrHorzMinNumPtsRangeTableEntries,
    };

static IviRangeTableEntry attrProbeAttenuationRangeTableEntries[] =
    {
        {0.001, 0.001, 0.001, "", 0},
        {0.01, 0.01, 0.01, "", 0},
        {0.1, 0.1, 0.1, "", 0},
        {1.0, 1.0, 1.0, "", 0},
        {10.0, 10.0, 10.0, "", 0},
        {100.0, 100.0, 100.0, "", 0},
        {1000.0, 1000.0, 1000.0, "", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrProbeAttenuationRangeTable =
    {
        IVI_VAL_COERCED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrProbeAttenuationRangeTableEntries,
    };

static IviRangeTableEntry attrAlternationPulseTriggerWidthRangeTableEntries[] =
    {
        {20.0e-9, 10.0, 0, "", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrAlternationPulseTriggerWidthRangeTable =
    {
        IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAlternationPulseTriggerWidthRangeTableEntries,
    };

static IviRangeTableEntry attrAlternationTriggerHoldoffRangeTableEntries[] =
    {
        {100.0e-9, 1.5, 0, "", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrAlternationTriggerHoldoffRangeTable =
    {
        IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAlternationTriggerHoldoffRangeTableEntries,
    };

static IviRangeTableEntry attrPulseTriggerWidthRangeTableEntries[] =
    {
        {20.0e-9, 10.0, 0, "", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrPulseTriggerWidthRangeTable =
    {
        IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrPulseTriggerWidthRangeTableEntries,
    };

static IviRangeTableEntry attrAcquisitionTypeRangeTableEntries[] =
    {
        {AG1000NI_VAL_NORMAL, 0, 0, "NORMAL", 0},
        {AG1000NI_VAL_AVERAGE, 0, 0, "AVERAGE", 0},
        {AG1000NI_VAL_PEAK_DETECT, 0, 0, "PEAK", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrAcquisitionTypeRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAcquisitionTypeRangeTableEntries,
    };

static IviRangeTableEntry attrTriggerCouplingRangeTableEntries[] =
    {
        {AG1000NI_VAL_AC, 0, 0, "AC", 0},
        {AG1000NI_VAL_DC, 0, 0, "DC", 0},
        {AG1000NI_VAL_LF_REJECT, 0, 0, "LF", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrTriggerCouplingRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrTriggerCouplingRangeTableEntries,
    };

static IviRangeTableEntry attrTriggerSlopeRangeTableEntries[] =
    {
        {AG1000NI_VAL_POSITIVE, 0, 0, "POSITIVE", 0},
        {AG1000NI_VAL_NEGATIVE, 0, 0, "NEGATIVE", 0},
        {AG1000NI_VAL_ALTERNATION, 0, 0, "ALTERNATION", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrTriggerSlopeRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrTriggerSlopeRangeTableEntries,
    };

static IviRangeTableEntry attrTriggerTypeRangeTableEntries[] =
    {
        {AG1000NI_VAL_EDGE_TRIGGER, 0, 0, "EDGE", 0},
        {AG1000NI_VAL_TV_TRIGGER, 0, 0, "VIDEO", 0},
        {AG1000NI_VAL_PULSE_TRIGGER, 0, 0, "PULSE", 0},
        {AG1000NI_VAL_PATTERN_TRIGGER, 0, 0, "PATTERN", 0},
        {AG1000NI_VAL_ALTERNATION_TRIGGER, 0, 0, "ALTERNATION", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrTriggerTypeRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrTriggerTypeRangeTableEntries,
    };

static IviRangeTableEntry attrVerticalCouplingRangeTableEntries[] =
    {
        {AG1000NI_VAL_AC, 0, 0, "AC", 0},
        {AG1000NI_VAL_DC, 0, 0, "DC", 0},
        {AG1000NI_VAL_GND, 0, 0, "GND", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrVerticalCouplingRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrVerticalCouplingRangeTableEntries,
    };

static IviRangeTableEntry attrTVTriggerEventRangeTableEntries[] =
    {
        {AG1000NI_VAL_TV_EVENT_LINE_NUMBER, 0, 0, "LINE", 0},
        {AG1000NI_VAL_TV_EVENT_ALL_LINE, 0, 0, "ALL", 0},
        {AG1000NI_VAL_TV_EVENT_ODD_FIELD, 0, 0, "ODD", 0},
        {AG1000NI_VAL_TV_EVENT_EVEN_FIELD, 0, 0, "EVEN", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrTVTriggerEventRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrTVTriggerEventRangeTableEntries,
    };

static IviRangeTableEntry attrTVTriggerEventReadRangeTableEntries[] =
    {
        {AG1000NI_VAL_TV_EVENT_LINE_NUMBER, 0, 0, "LINE", 0},
        {AG1000NI_VAL_TV_EVENT_ALL_LINE, 0, 0, "ALL LINES", 0},
        {AG1000NI_VAL_TV_EVENT_ODD_FIELD, 0, 0, "ODD FIELD", 0},
        {AG1000NI_VAL_TV_EVENT_EVEN_FIELD, 0, 0, "EVEN FIELD", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrTVTriggerEventReadRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrTVTriggerEventReadRangeTableEntries,
    };

static IviRangeTableEntry attrTVTriggerPolarityRangeTableEntries[] =
    {
        {AG1000NI_VAL_TV_POSITIVE, 0, 0, "POSITIVE", 0},
        {AG1000NI_VAL_TV_NEGATIVE, 0, 0, "NEGATIVE", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrTVTriggerPolarityRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrTVTriggerPolarityRangeTableEntries,
    };

static IviRangeTableEntry attrTVTriggerSignalFormatRangeTableEntries[] =
    {
        {AG1000NI_VAL_NTSC, 0, 0, "NTSC", 0},
        {AG1000NI_VAL_PALSECAM, 0, 0, "PALSECAM", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrTVTriggerSignalFormatRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrTVTriggerSignalFormatRangeTableEntries,
    };

static IviRangeTableEntry attrSampleModeRangeTableEntries[] =
    {
        {AG1000NI_VAL_REAL_TIME, 0, 0, "RTIME", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrSampleModeRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSampleModeRangeTableEntries,
    };

static IviRangeTableEntry attrTriggerModifierRangeTableEntries[] =
    {
        {AG1000NI_VAL_NO_TRIGGER_MOD, 0, 0, "NORMAL", 0},
        {AG1000NI_VAL_AUTO, 0, 0, "AUTO", 0},
        {AG1000NI_VAL_SINGLE, 0, 0, "SINGLE", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrTriggerModifierRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrTriggerModifierRangeTableEntries,
    };

static IviRangeTableEntry attrTriggerHoldoffRangeTableEntries[] =
    {
        {100.0e-9, 1.5, 0, "", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrTriggerHoldoffRangeTable = 
    {
        IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrTriggerHoldoffRangeTableEntries,
    };

static IviRangeTableEntry attrNumAveragesRangeTableEntries[] =
    {
        {2, 2, 2, "", 0},
        {4, 4, 4, "", 0},
        {8, 8, 8, "", 0},
        {16, 16, 16, "", 0},
        {32, 32, 32, "", 0},
        {64, 64, 64, "", 0},
        {128, 128, 128, "", 0},
        {256, 256, 256, "", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrNumAveragesRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrNumAveragesRangeTableEntries,
    };

static IviRangeTableEntry attrPulseTriggerModeRangeTableEntries[] =
    {
        {AG1000NI_VAL_PULSE_TRIGGER_GT_POSITIVE, 0, 0, "+GRE", 0},
        {AG1000NI_VAL_PULSE_TRIGGER_LT_POSITIVE, 0, 0, "+LESS", 0},
        {AG1000NI_VAL_PULSE_TRIGGER_GT_NEGATIVE, 0, 0, "-GRE", 0},
        {AG1000NI_VAL_PULSE_TRIGGER_LT_NEGATIVE, 0, 0, "-LESS", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrPulseTriggerModeRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrPulseTriggerModeRangeTableEntries,
    };

static IviRangeTableEntry attrPulseTriggerModeReadRangeTableEntries[] =
    {
        {AG1000NI_VAL_PULSE_TRIGGER_GT_POSITIVE, 0, 0, "+GREATER THAN", 0},
        {AG1000NI_VAL_PULSE_TRIGGER_LT_POSITIVE, 0, 0, "+LESS THAN", 0},
        {AG1000NI_VAL_PULSE_TRIGGER_GT_NEGATIVE, 0, 0, "-GREATER THAN", 0},
        {AG1000NI_VAL_PULSE_TRIGGER_LT_NEGATIVE, 0, 0, "-LESS THAN", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrPulseTriggerModeReadRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrPulseTriggerModeReadRangeTableEntries,
    };

static IviRangeTableEntry attrAlternationTriggerCouplingRangeTableEntries[] =
    {
        {AG1000NI_VAL_AC, 0, 0, "AC", 0},
        {AG1000NI_VAL_DC, 0, 0, "DC", 0},
        {AG1000NI_VAL_LF_REJECT, 0, 0, "LF", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrAlternationTriggerCouplingRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAlternationTriggerCouplingRangeTableEntries,
    };

static IviRangeTableEntry attrAlternationTriggerSourceRangeTableEntries[] =
    {
        {AG1000NI_VAL_CH1CH2, 0, 0, "CH1CH2", 0},
        {AG1000NI_VAL_CH1CH3, 0, 0, "CH1CH3", 0},
        {AG1000NI_VAL_CH1CH4, 0, 0, "CH1CH4", 0},
        {AG1000NI_VAL_CH2CH3, 0, 0, "CH2CH3", 0},
        {AG1000NI_VAL_CH2CH4, 0, 0, "CH2CH4", 0},
        {AG1000NI_VAL_CH3CH4, 0, 0, "CH3CH4", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrAlternationTriggerSourceRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAlternationTriggerSourceRangeTableEntries,
    };

static IviRangeTableEntry attrAlternationTriggerTypeRangeTableEntries[] =
    {
        {AG1000NI_VAL_ALTERNATION_EDGE_TRIGGER, 0, 0, "EDGE", 0},
        {AG1000NI_VAL_ALTERNATION_TV_TRIGGER, 0, 0, "VIDEO", 0},
        {AG1000NI_VAL_ALTERNATION_PULSE_TRIGGER, 0, 0, "PULSE", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrAlternationTriggerTypeRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAlternationTriggerTypeRangeTableEntries,
    };

static IviRangeTableEntry attrAlternationEdgeTriggerSlopeRangeTableEntries[] =
    {
        {AG1000NI_VAL_POSITIVE, 0, 0, "POSITIVE", 0},
        {AG1000NI_VAL_NEGATIVE, 0, 0, "NEGATIVE", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrAlternationEdgeTriggerSlopeRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAlternationEdgeTriggerSlopeRangeTableEntries,
    };

static IviRangeTableEntry attrAlternationPulseTriggerModeRangeTableEntries[] =
    {
        {AG1000NI_VAL_PULSE_TRIGGER_GT_POSITIVE, 0, 0, "+GRE", 0},
        {AG1000NI_VAL_PULSE_TRIGGER_LT_POSITIVE, 0, 0, "+LESS", 0},
        {AG1000NI_VAL_PULSE_TRIGGER_GT_NEGATIVE, 0, 0, "-GRE", 0},
        {AG1000NI_VAL_PULSE_TRIGGER_LT_NEGATIVE, 0, 0, "-LESS", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrAlternationPulseTriggerModeRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAlternationPulseTriggerModeRangeTableEntries,
    };

static IviRangeTableEntry attrAlternationPulseTriggerModeReadRangeTableEntries[] =
    {
        {AG1000NI_VAL_PULSE_TRIGGER_GT_POSITIVE, 0, 0, "+GREATER THAN", 0},
        {AG1000NI_VAL_PULSE_TRIGGER_LT_POSITIVE, 0, 0, "+LESS THAN", 0},
        {AG1000NI_VAL_PULSE_TRIGGER_GT_NEGATIVE, 0, 0, "-GREATER THAN", 0},
        {AG1000NI_VAL_PULSE_TRIGGER_LT_NEGATIVE, 0, 0, "-LESS THAN", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrAlternationPulseTriggerModeReadRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAlternationPulseTriggerModeReadRangeTableEntries,
    };

static IviRangeTableEntry attrAlternationTVTriggerEventRangeTableEntries[] =
    {
        {AG1000NI_VAL_TV_EVENT_LINE_NUMBER, 0, 0, "LINE", 0},
        {AG1000NI_VAL_TV_EVENT_ALL_LINE, 0, 0, "ALL", 0},
        {AG1000NI_VAL_TV_EVENT_ODD_FIELD, 0, 0, "ODD", 0},
        {AG1000NI_VAL_TV_EVENT_EVEN_FIELD, 0, 0, "EVEN", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrAlternationTVTriggerEventRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAlternationTVTriggerEventRangeTableEntries,
    };

static IviRangeTableEntry attrAlternationTVTriggerEventReadRangeTableEntries[] =
    {
        {AG1000NI_VAL_TV_EVENT_LINE_NUMBER, 0, 0, "LINE", 0},
        {AG1000NI_VAL_TV_EVENT_ALL_LINE, 0, 0, "ALL LINES", 0},
        {AG1000NI_VAL_TV_EVENT_ODD_FIELD, 0, 0, "ODD FIELD", 0},
        {AG1000NI_VAL_TV_EVENT_EVEN_FIELD, 0, 0, "EVEN FIELD", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrAlternationTVTriggerEventReadRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAlternationTVTriggerEventReadRangeTableEntries,
    };

static IviRangeTableEntry attrAlternationTVTriggerPolarityRangeTableEntries[] =
    {
        {AG1000NI_VAL_TV_POSITIVE, 0, 0, "POSITIVE", 0},
        {AG1000NI_VAL_TV_NEGATIVE, 0, 0, "NEGATIVE", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrAlternationTVTriggerPolarityRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAlternationTVTriggerPolarityRangeTableEntries,
    };

static IviRangeTableEntry attrAlternationTVTriggerSignalFormatRangeTableEntries[] =
    {
        {AG1000NI_VAL_NTSC, 0, 0, "NTSC", 0},
        {AG1000NI_VAL_PALSECAM, 0, 0, "PALSECAM", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrAlternationTVTriggerSignalFormatRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAlternationTVTriggerSignalFormatRangeTableEntries,
    };

static IviRangeTableEntry attrInputImpedanceRangeTableEntries[] =
    {
        {1.0e6, 1.0e6, 0, "", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrInputImpedanceRangeTable =
    {
        IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrInputImpedanceRangeTableEntries,
    };

/*****************************************************************************
 *-------------- Utility Function Declarations (Non-Exported) ---------------*
 *****************************************************************************/

static ViStatus ag1000ni_IviInit (ViRsrc resourceName, ViBoolean IDQuery, 
                                     ViBoolean reset, ViSession vi);
static ViStatus ag1000ni_IviClose (ViSession vi);   

static ViStatus ag1000ni_InitAttributes (ViSession vi);
static ViStatus ag1000ni_DefaultInstrSetup (ViSession openInstrSession);
static ViStatus ag1000ni_CheckStatus (ViSession vi);
static ViStatus ag1000ni_GetDriverSetupOption (ViSession vi, ViInt32 *model);
static ViStatus ag1000ni_SetupInstrumentInfo (ViSession vi, ViInt32 *model);

    /*- File I/O Utility Functions -*/
static ViStatus ag1000ni_ReadToFile (ViSession vi, ViConstString filename, 
                                     ViInt32 maxBytesToRead, ViInt32 fileAction, ViInt32 *totalBytesWritten);
static ViStatus ag1000ni_WriteFromFile (ViSession vi, ViConstString filename, 
                                        ViInt32 maxBytesToWrite, ViInt32 byteOffset, 
                                        ViInt32 *totalBytesWritten);

static ViStatus ag1000ni_GetStringFromTable( ag1000niStringValueTable table, ViInt32 value,
        ViConstString* string );

static ViStatus ag1000ni_GetValueFromTable( ag1000niStringValueTable table,
        ViConstString string, ViInt32* value );

static ViStatus _VI_FUNC ag1000niAttrTriggerLevel_CommonCheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value);

/*****************************************************************************
 *----------------- Callback Declarations (Non-Exported) --------------------*
 *****************************************************************************/

    /*- Global Session Callbacks --------------------------------------------*/

static ViStatus _VI_FUNC ag1000ni_CheckStatusCallback (ViSession vi, ViSession io);
static ViStatus _VI_FUNC ag1000ni_WaitForOPCCallback (ViSession vi, ViSession io);

    /*- Attribute callbacks -------------------------------------------------*/

static ViStatus _VI_FUNC ag1000niEnum_ReadCallback (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32*        value);
static ViStatus _VI_FUNC ag1000niEnum_WriteCallback (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32         value);

static ViStatus _VI_FUNC ag1000niViBoolean_ReadCallback (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViBoolean*      value);
static ViStatus _VI_FUNC ag1000niViBoolean_WriteCallback (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViBoolean       value);

static ViStatus _VI_FUNC ag1000niViString_WriteCallback (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViConstString   value);
static ViStatus _VI_FUNC ag1000niViString_ReadCallback (
    ViSession           vi,
    ViSession           io,
    ViConstString       channelName,
    ViAttr              attributeId,
    const ViConstString cacheValue);

static ViStatus _VI_FUNC ag1000niViReal64_WriteCallback (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViReal64        value
);
static ViStatus _VI_FUNC ag1000niViReal64_ReadCallback (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViReal64*       value
);
static ViStatus _VI_FUNC ag1000niViInt32_WriteCallback (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32         value
);
static ViStatus _VI_FUNC ag1000niViInt32_ReadCallback (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32*        value
);

static ViStatus _VI_FUNC ag1000niEnum_ReadCallbackWithSpecifiedRangeTable (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,      
    ViAttr          attributeId,
    IviRangeTablePtr    tblPtr,
    ViInt32*        value);

static ViStatus _VI_FUNC ag1000niEnum_ReadCallbackWithSuffixChannel (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32*        value);

static ViStatus _VI_FUNC ag1000niEnum_WriteCallbackWithSuffixChannel (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32         value);

static ViStatus _VI_FUNC ag1000niViReal64_WriteCallbackWithSuffixChannel (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViReal64        value
);
static ViStatus _VI_FUNC ag1000niViReal64_ReadCallbackWithSuffixChannel (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViReal64*       value
);

static ViStatus _VI_FUNC ag1000niViInt32_WriteCallbackWithSuffixChannel (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32         value
);
static ViStatus _VI_FUNC ag1000niViInt32_ReadCallbackWithSuffixChannel (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32*        value
);

static ViStatus _VI_FUNC ag1000niAttrDriverRevision_ReadCallback (ViSession vi,
                                                                  ViSession io, 
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId, 
                                                                  const ViConstString cacheValue);
static ViStatus _VI_FUNC ag1000niAttrInstrumentManufacturer_ReadCallback (ViSession vi, 
                                                                          ViSession io,
                                                                          ViConstString channelName, 
                                                                          ViAttr attributeId,
                                                                          const ViConstString cacheValue);
static ViStatus _VI_FUNC ag1000niAttrInstrumentModel_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue);

static ViStatus _VI_FUNC ag1000niAttrFirmwareRevision_ReadCallback (ViSession vi, 
                                                                    ViSession io,
                                                                    ViConstString channelName, 
                                                                    ViAttr attributeId,
                                                                    const ViConstString cacheValue);
static ViStatus _VI_FUNC ag1000niAttrIdQueryResponse_ReadCallback (ViSession vi,
                                                                   ViSession io, 
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId, 
                                                                   const ViConstString cacheValue);

static ViStatus _VI_FUNC ag1000niAttrInitiateContinuous_WriteCallback (ViSession vi,
                                                                       ViSession io,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViBoolean value);

static ViStatus _VI_FUNC ag1000niAttrHorzTimePerRecord_ReadCallback (ViSession vi,
                                                                     ViSession io,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViReal64 *value);

static ViStatus _VI_FUNC ag1000niAttrHorzTimePerRecord_WriteCallback (ViSession vi,
                                                                      ViSession io,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViReal64 value);     

static ViStatus _VI_FUNC ag1000niAttrProbeAttenuation_ReadCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViReal64 *value);

static ViStatus _VI_FUNC ag1000niAttrProbeAttenuation_WriteCallback (ViSession vi,
                                                                     ViSession io,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViReal64 value);

static ViStatus _VI_FUNC ag1000niAttrMaxInputFrequency_ReadCallback (ViSession vi,
                                                                     ViSession io,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViReal64 *value);

static ViStatus _VI_FUNC ag1000niAttrMaxInputFrequency_WriteCallback (ViSession vi,
                                                                      ViSession io,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViReal64 value);

static ViStatus _VI_FUNC ag1000niAttrTriggerSource_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 const ViConstString cacheValue);

static ViStatus _VI_FUNC ag1000niAttrTriggerSource_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViConstString value);

static ViStatus _VI_FUNC ag1000niAttrInstrEdgeTriggerSource_CheckCallback (ViSession vi,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViConstString value);

static ViStatus _VI_FUNC ag1000niAttrCommonTriggerSource_CheckCallback (ViSession vi,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViConstString value);

static ViStatus _VI_FUNC ag1000niAttrTriggerModifier_ReadCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViInt32 *value);

static ViStatus _VI_FUNC ag1000niAttrTriggerModifier_WriteCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViInt32 value);

static ViStatus _VI_FUNC ag1000niAttrTvTriggerLineNumber_CheckCallback (ViSession vi,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViInt32 value);

static ViStatus _VI_FUNC ag1000niAttrPatternTriggerPattern_CheckCallback (ViSession vi,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViConstString value);

static ViStatus _VI_FUNC ag1000niAttrAcquisitionStartTime_CheckCallback (ViSession vi,
                                                                         ViConstString channelName,
                                                                         ViAttr attributeId,
                                                                         ViReal64 value);

static ViStatus _VI_FUNC ag1000niAttrAlternationTriggerHorzTimePerRecord_ReadCallback (ViSession vi,
                                                                                       ViSession io,
                                                                                       ViConstString channelName,
                                                                                       ViAttr attributeId,
                                                                                       ViReal64 *value);

static ViStatus _VI_FUNC ag1000niAttrAlternationTriggerHorzTimePerRecord_WriteCallback (ViSession vi,
                                                                                        ViSession io,
                                                                                        ViConstString channelName,
                                                                                        ViAttr attributeId,
                                                                                        ViReal64 value);

static ViStatus _VI_FUNC ag1000niAttrHorzTimePerRecord_RangeTableCallback (ViSession vi,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           IviRangeTablePtr *rangeTablePtr);

static ViStatus _VI_FUNC ag1000niAttrVerticalRange_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 *value);

static ViStatus _VI_FUNC ag1000niAttrVerticalRange_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value);

static ViStatus _VI_FUNC ag1000niAttrVerticalRange_CheckCallback (ViSession vi,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value);

static ViStatus _VI_FUNC ag1000niAttrTriggerLevel_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC ag1000niAttrTvTriggerLevel_CheckCallback (ViSession vi,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 value);

static ViStatus _VI_FUNC ag1000niAttrPatternTriggerLevel_CheckCallback (ViSession vi,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViReal64 value);

static ViStatus _VI_FUNC ag1000niAttrPulseTriggerLevel_CheckCallback (ViSession vi,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViReal64 value);

static ViStatus _VI_FUNC ag1000niAttrAlternationEdgeTriggerLevel_CheckCallback (ViSession vi,
                                                                                ViConstString channelName,
                                                                                ViAttr attributeId,
                                                                                ViReal64 value);

static ViStatus _VI_FUNC ag1000niAttrTriggerSensitivity_CheckCallback (ViSession vi,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViReal64 value);

static ViStatus _VI_FUNC ag1000niAttrAlternationTriggerSensitivity_CheckCallback (ViSession vi,
                                                                                  ViConstString channelName,
                                                                                  ViAttr attributeId,
                                                                                  ViReal64 value);

static ViStatus _VI_FUNC ag1000niAttrTriggerSource_CommonReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          const ViConstString cacheValue);

static ViStatus _VI_FUNC ag1000niAttrHorzRecordLength_ReadCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViInt32 *value);

static ViStatus _VI_FUNC ag1000niAttrTvTriggerEvent_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 *value);

static ViStatus _VI_FUNC ag1000niAttrPulseTriggerMode_ReadCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViInt32 *value);

static ViStatus _VI_FUNC ag1000niAttrAlternationPulseTriggerMode_ReadCallback (ViSession vi,
                                                                               ViSession io,
                                                                               ViConstString channelName,
                                                                               ViAttr attributeId,
                                                                               ViInt32 *value);

static ViStatus _VI_FUNC ag1000niAttrAlternationTvTriggerEvent_ReadCallback (ViSession vi,
                                                                             ViSession io,
                                                                             ViConstString channelName,
                                                                             ViAttr attributeId,
                                                                             ViInt32 *value);

static ViStatus _VI_FUNC ag1000niAttrAlternationTvTriggerLineNumber_CheckCallback (ViSession vi,
                                                                                   ViConstString channelName,
                                                                                   ViAttr attributeId,
                                                                                   ViInt32 value);


/*****************************************************************************
 *------------ User-Callable Functions (Exportable Functions) ---------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: ag1000ni_init   
 * Purpose:  VXIplug&play required function.  Calls the   
 *           ag1000ni_InitWithOptions function.   
 *****************************************************************************/
ViStatus _VI_FUNC ag1000ni_init (ViRsrc resourceName, ViBoolean IDQuery,
                                 ViBoolean resetDevice, ViSession *newVi)
{   
    ViStatus    error = VI_SUCCESS;

    if (newVi == VI_NULL)
    {
        Ivi_SetErrorInfo (VI_NULL, VI_FALSE, IVI_ERROR_INVALID_PARAMETER, 
                          VI_ERROR_PARAMETER4, "Null address for Instrument Handle");
        checkErr( IVI_ERROR_INVALID_PARAMETER);
    }

    checkErr( ag1000ni_InitWithOptions (resourceName, IDQuery, resetDevice, 
                                        "", newVi));

Error:  
    return error;
}

/*****************************************************************************
 * Function: ag1000ni_InitWithOptions                                       
 * Purpose:  This function creates a new IVI session and calls the 
 *           IviInit function.                                     
 *****************************************************************************/
ViStatus _VI_FUNC ag1000ni_InitWithOptions(
    ViRsrc          resourceName,
    ViBoolean       IDQuery,
    ViBoolean       resetDevice,
    ViConstString   optionString,
    ViSession       *newVi
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
    checkErr( Ivi_SpecificDriverNew ("ag1000ni", newOptionString, &vi));  
    if (!isLogicalName)
    {
        ViInt32 oldFlag = 0;

        checkErr (Ivi_GetAttributeFlags (vi, IVI_ATTR_IO_RESOURCE_DESCRIPTOR, &oldFlag));
        checkErr (Ivi_SetAttributeFlags (vi, IVI_ATTR_IO_RESOURCE_DESCRIPTOR, oldFlag & 0xfffb | 0x0010));
        checkErr (Ivi_SetAttributeViString (vi, "", IVI_ATTR_IO_RESOURCE_DESCRIPTOR, 0, newResourceName));
    }
        /* init the driver */
    checkErr( ag1000ni_IviInit (newResourceName, IDQuery, resetDevice, vi)); 
    if (isLogicalName)
        checkErr( Ivi_ApplyDefaultSetup (vi));
    *newVi = vi;

Error:
    if (error < VI_SUCCESS) 
        Ivi_Dispose (vi);

    return error;
}

/*****************************************************************************
 * Function: ag1000ni_close                                                           
 * Purpose:  This function closes the instrument.                            
 *
 *           Note:  This function must unlock the session before calling
 *           Ivi_Dispose.
 *****************************************************************************/
ViStatus _VI_FUNC ag1000ni_close (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    checkErr( ag1000ni_IviClose (vi));

Error:    
    Ivi_UnlockSession (vi, VI_NULL);
    Ivi_Dispose (vi);  

    return error;
}

/*****************************************************************************
 * Function: ag1000ni_reset                                                         
 * Purpose:  This function resets the instrument.                          
 *****************************************************************************/
ViStatus _VI_FUNC ag1000ni_reset (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (!Ivi_Simulating(vi))                /* call only when locked */
    {
        ViSession   io = Ivi_IOSession(vi); /* call only when locked */

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
        viCheckErr( viPrintf (io, "*RST"));
    }

    checkErr( ag1000ni_DefaultInstrSetup (vi));                                

    checkErr( ag1000ni_CheckStatus (vi));                                      

Error:
    Ivi_UnlockSession (vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: ag1000ni_ResetWithDefaults
 * Purpose:  This function resets the instrument and applies default settings
 *           from the IVI Configuration Store based on the logical name
 *           from which the session was created.
 *****************************************************************************/
ViStatus _VI_FUNC ag1000ni_ResetWithDefaults (ViSession vi)
{
    ViStatus error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    checkErr( ag1000ni_reset(vi));
    checkErr( Ivi_ApplyDefaultSetup(vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/**************************************************************************** 
 *  Function: ag1000ni_Disable
 *  Purpose:  This function places the instrument in a quiescent state as 
 *            quickly as possible.
 ****************************************************************************/
ViStatus _VI_FUNC ag1000ni_Disable (ViSession vi)
{
    ViStatus error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

Error:
    Ivi_UnlockSession (vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: ag1000ni_self_test                                                       
 * Purpose:  This function executes the instrument self-test and returns the 
 *           result.                                                         
 *****************************************************************************/
ViStatus _VI_FUNC ag1000ni_self_test (ViSession vi, ViInt16 *testResult, 
                                      ViChar testMessage[])
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (testResult == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Test Result");
    if (testMessage == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Test Message");

    /* Not supported by instrument */
    *testResult = 0;
    strcpy (testMessage, "No error.");         

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: ag1000ni_revision_query                                                  
 * Purpose:  This function returns the driver and instrument revisions.      
 *****************************************************************************/
ViStatus _VI_FUNC ag1000ni_revision_query (ViSession vi, ViChar driverRev[], 
                                           ViChar instrRev[])
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (driverRev == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Driver Revision");
    if (instrRev == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Instrument Revision");

    checkErr( Ivi_GetAttributeViString (vi, VI_NULL, AG1000NI_ATTR_SPECIFIC_DRIVER_REVISION, 
                                        0, 256, driverRev));
    checkErr( Ivi_GetAttributeViString (vi, "", AG1000NI_ATTR_INSTRUMENT_FIRMWARE_REVISION, 
                                        0, 256, instrRev));
    checkErr( ag1000ni_CheckStatus (vi));

Error:    
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: ag1000ni_error_query                                                     
 * Purpose:  This function queries the instrument error queue and returns   
 *           the result.                                                     
 *****************************************************************************/
ViStatus _VI_FUNC ag1000ni_error_query (ViSession vi, ViInt32 *errCode, 
                                        ViChar errMessage[])
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (errCode == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Error Code");
    if (errMessage == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Error Message");

    if (!Ivi_Simulating(vi))                /* call only when locked */
    {
        ViSession   io = Ivi_IOSession(vi); /* call only when locked */

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
        viCheckErr( viPrintf(io, ":SYST:ERR?"));

        viCheckErr( viScanf (io, "%ld,%256[^\n]", errCode, errMessage));       
    }
    else
    {
            /* Simulate Error Query */
        *errCode = 0;
        strcpy (errMessage, "No error.");
    }

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: ag1000ni_error_message                                                  
 * Purpose:  This function translates the error codes returned by this       
 *           instrument driver into user-readable strings.  
 *
 *           Note:  The caller can pass VI_NULL for the vi parameter.  This 
 *           is useful if one of the init functions fail.
 *****************************************************************************/
ViStatus _VI_FUNC ag1000ni_error_message (ViSession vi, ViStatus errorCode,
                                          ViChar errorMessage[256])
{
    ViStatus    error = VI_SUCCESS;

    static      IviStringValueTable errorTable = 
    {   
            IVISCOPE_ERROR_CODES_AND_MSGS,
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
 *  Function: ag1000ni_IsInvalidWfmElement
 *  Purpose:  This function takes one of the Waveform Array's element value
 *            that you obtain from the ReadWaveform or FetchWaveform function
 *            and determines if the value is a valid measurement value or a
 *            value indicating that the oscilloscope could not sample a
 *            voltage.
 *****************************************************************************/
ViStatus _VI_FUNC ag1000ni_IsInvalidWfmElement (ViSession vi,
                                       ViReal64 elementValue,
                                       ViBoolean *isInvalid)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     type;

    checkErr( Ivi_LockSession(vi, VI_NULL));

    if (isInvalid == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Is Invalid.");

    viCheckErr( Ivi_GetViReal64Type (elementValue, &type));

    if (type == IVI_VAL_TYPE_NAN)
        *isInvalid = VI_TRUE;
    else
        *isInvalid = VI_FALSE;

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: ag1000ni_InvalidateAllAttributes
 * Purpose:  This function invalidates the cached value of all attributes.
 *****************************************************************************/
ViStatus _VI_FUNC ag1000ni_InvalidateAllAttributes (ViSession vi)
{
    return Ivi_InvalidateAllAttributes(vi);
}  

/*****************************************************************************
 * Function: ag1000ni_GetError and ag1000ni_ClearError Functions                       
 * Purpose:  These functions enable the instrument driver user to  
 *           get or clear the error information the driver associates with the
 *           IVI session.                                                        
 *****************************************************************************/
ViStatus _VI_FUNC ag1000ni_GetError (ViSession vi, 
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
        checkErr( ag1000ni_error_message (vi, primary, msg));
        appendPoint += sprintf(appendPoint, "Primary Error: (Hex 0x%08X) %s\n", primary, msg);
    }

    if (secondary != VI_SUCCESS)
    {
        ViChar msg[256] = "";
        checkErr( ag1000ni_error_message (vi, secondary, msg));
        appendPoint += sprintf(appendPoint, "Secondary Error: (Hex 0x%08X) %s\n", secondary, msg);
    }

    if (elaboration[0])
    {
        sprintf(appendPoint, "Elaboration: %s", elaboration);
    }

    actualSize = (ViInt32)strlen(errorMessage) + 1;

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

/*****************************************************************************
 * Function: ag1000ni_ClearError                        
 * Purpose:  This function clears the error code and error description for
             the IVI session.           
 *****************************************************************************/
ViStatus _VI_FUNC ag1000ni_ClearError (ViSession vi)                                                        
{                                                                                                           
    return Ivi_ClearErrorInfo (vi);                                                                             
}

/*****************************************************************************
 * Function: ag1000ni_GetNextCoercionRecord                        
 * Purpose:  This function enables the instrument driver user to obtain
 *           the coercion information associated with the IVI session.                                                              
 *           This function retrieves and clears the oldest instance in which 
 *           the instrument driver coerced a value the instrument driver user
 *           specified to another value.                     
 *****************************************************************************/
ViStatus _VI_FUNC ag1000ni_GetNextCoercionRecord (ViSession vi,
                                                  ViInt32 bufferSize,
                                                  ViChar  record[])
{
    return Ivi_GetNextCoercionString (vi, bufferSize, record);
}

/**************************************************************************** 
 *  Function: ag1000ni_GetNextInterchangeWarning,
 *            ag1000ni_ResetInterchangeCheck, and
 *            ag1000ni_ClearInterchangeWarnings
 *  Purpose:  These functions allow the user to retrieve interchangeability
 *            warnings, reset the driver's interchangeability checking 
 *            state, and clear all previously logged interchangeability warnings.
 ****************************************************************************/
ViStatus _VI_FUNC ag1000ni_GetNextInterchangeWarning (ViSession vi, 
                                                      ViInt32 bufferSize, 
                                                      ViChar warnString[])
{
    return Ivi_GetNextInterchangeCheckString (vi, bufferSize, warnString);
}        

ViStatus _VI_FUNC ag1000ni_ResetInterchangeCheck (ViSession vi)
{
    return Ivi_ResetInterchangeCheck (vi);
}

ViStatus _VI_FUNC ag1000ni_ClearInterchangeWarnings (ViSession vi)
{
    return Ivi_ClearInterchangeWarnings (vi);
}

/*****************************************************************************
 * Function: ag1000ni_LockSession and ag1000ni_UnlockSession Functions                        
 * Purpose:  These functions enable the instrument driver user to lock the 
 *           session around a sequence of driver calls during which other
 *           execution threads must not disturb the instrument state.
 *                                                                          
 *           NOTE:  The callerHasLock parameter must be a local variable 
 *           initialized to VI_FALSE and passed by reference, or you can pass 
 *           VI_NULL.                     
 *****************************************************************************/
ViStatus _VI_FUNC ag1000ni_LockSession (ViSession vi, ViBoolean *callerHasLock)  
{                                              
    return Ivi_LockSession(vi,callerHasLock);      
}                                              
ViStatus _VI_FUNC ag1000ni_UnlockSession (ViSession vi, ViBoolean *callerHasLock) 
{                                              
    return Ivi_UnlockSession(vi,callerHasLock);    
}

/**************************************************************************** 
 *  Function: ag1000ni_GetChannelName
 *  Purpose:  This function returns the highest-level channel name that
 *            corresponds to the specific driver channel string that is in
 *            the channel table at an index you specify.
 ****************************************************************************/
ViStatus _VI_FUNC ag1000ni_GetChannelName (ViSession vi, ViInt32 index,
                                           ViInt32 bufferSize,
                                           ViChar name[])
{
    ViStatus        error = VI_SUCCESS;
    ViConstString   channelName;
    ViInt32         actualSize = 0;
    ViInt32         channelCount;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    /* Check parameters */
    if (bufferSize != 0)
        {
        if (name == VI_NULL)
            {
            viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Channel Name");
            }
        }

    checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, IVI_ATTR_CHANNEL_COUNT, 0, &channelCount));
    if ( (index<1) || (index>channelCount) )
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Channel Index");

    /* Get channel name */
    viCheckParm( Ivi_GetNthChannelString (vi, index, &channelName), 2, "Index");
    checkErr( Ivi_GetUserChannelName (vi, channelName, &channelName));

    actualSize = (ViInt32)strlen(channelName) + 1;

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
 *= IviScope Configuration -> Acquisition Group Functions =============================*
 *===========================================================================*/         

/*******************************************************************************
 * Function:  ag1000ni_ConfigureAcquisitionRecord
 * Purpose:   This function configures the common attributes of the
              acquisition subsystem.  These attributes are the time per
              record, minimum record length, and acquisition start time. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ConfigureAcquisitionRecord (
                  ViSession vi, ViReal64 TimePerRecord, ViInt32 MinNumPts, 
                  ViReal64 AcquisitionStartTime)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AG1000NI_ATTR_HORZ_TIME_PER_RECORD,
                 0, TimePerRecord), 2, "Time Per Record");
    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_HORZ_MIN_NUM_PTS,
                 0, MinNumPts), 3, "Min Num Pts");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AG1000NI_ATTR_ACQUISITION_START_TIME,
                 0, AcquisitionStartTime), 4, "Acquisition Start Time");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_ConfigureAcquisitionType
 * Purpose:   This function configures how the oscilloscope acquires data and
              fills the waveform record. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ConfigureAcquisitionType (
                  ViSession vi, ViInt32 AcquisitionType)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_ACQUISITION_TYPE,
                 0, AcquisitionType), 2, "Acquisition Type");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_ConfigureInitiateContinuous
 * Purpose:   This function sets the continuous acquisition mode on the
              instrument. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ConfigureInitiateContinuous (
                  ViSession vi, ViBoolean ContinuousAcquisition)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViBoolean (vi, VI_NULL, AG1000NI_ATTR_INITIATE_CONTINUOUS,
                 0, ContinuousAcquisition), 2, "Continuous Acquisition");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_ConfigureNumberOfAverages
 * Purpose:   This function configures the number of waveforms that the
              oscilloscope  acquires and averages.  After the oscilloscope
              acquires as many waveforms as you specify, it returns to the
              idle state. 
*******************************************************************************/
ViStatus _VI_FUNC ag1000ni_ConfigureNumberOfAverages (ViSession vi,
                                        ViInt32 NumAverages)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_NUM_AVERAGES,
                 0, NumAverages), 2, "Num Averages");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*******************************************************************************
 * Function:  ag1000ni_ConfigureNumAverages
 * Purpose:   This function configures the number of waveforms that the
              oscilloscope  acquires and averages.  After the oscilloscope
              acquires as many waveforms as you specify, it returns to the
              idle state. 
*******************************************************************************/
ViStatus _VI_FUNC ag1000ni_ConfigureNumAverages (ViSession vi,
                                        ViInt32 NumAverages)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_NUM_AVERAGES,
                 0, NumAverages), 2, "Num Averages");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}
/*===========================================================================*
 *= IviScope Configuration -> Channel Group Functions =======================*
 *===========================================================================*/         

/*******************************************************************************
 * Function:  ag1000ni_ConfigureChannel
 * Purpose:   This function configures the common attributes of the
              oscilloscope's channel subsystem for a particular channel.
              These attributes are the vertical range, vertical offset,
              coupling, probe attenuation, and channel enabled. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ConfigureChannel (ViSession vi, ViConstString Channel, ViReal64 Range, 
                  ViReal64 Offset, ViInt32 Coupling, ViReal64 ProbeAttenuation, 
                  ViBoolean Enabled)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViReal64 (vi, Channel, AG1000NI_ATTR_PROBE_ATTENUATION,
                 0, ProbeAttenuation), 6, "Probe Attenuation");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, Channel, AG1000NI_ATTR_VERTICAL_RANGE,
                 0, Range), 3, "Range");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, Channel, AG1000NI_ATTR_VERTICAL_OFFSET,
                 0, Offset), 4, "Offset");
    viCheckParm( Ivi_SetAttributeViInt32 (vi, Channel, AG1000NI_ATTR_VERTICAL_COUPLING,
                 0, Coupling), 5, "Coupling");
    viCheckParm( Ivi_SetAttributeViBoolean (vi, Channel, AG1000NI_ATTR_CHANNEL_ENABLED,
                 0, Enabled), 7, "Enabled");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);                             
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_ConfigureChanCharacteristics
 * Purpose:   This function configures the attributes that control the
              electrical characteristics of the channel.  These attributes are
              the input impedance and the bandwidth. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ConfigureChanCharacteristics (ViSession vi, ViConstString Channel, ViReal64 InputImpedance, 
                  ViReal64 MaxInputFrequency)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViReal64 (vi, Channel, AG1000NI_ATTR_INPUT_IMPEDANCE,
                 0, InputImpedance), 3, "Input Impedance");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, Channel, AG1000NI_ATTR_MAX_INPUT_FREQUENCY,
                 0, MaxInputFrequency), 4, "Max Input Frequency");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_ConfigureChanAdjustment
 * Purpose:    
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ConfigureChanAdjustment (
                  ViSession vi, ViConstString Channel, ViBoolean InputInverted, 
                  ViBoolean DigitalFilterEnabled)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViBoolean (vi, Channel, AG1000NI_ATTR_INPUT_INVERSION_ENABLED,
                 0, InputInverted), 3, "Input Inverted");
    viCheckParm( Ivi_SetAttributeViBoolean (vi, Channel, AG1000NI_ATTR_DIGITAL_FILTER_ENABLED,
                 0, DigitalFilterEnabled), 4, "Digital Filter Enabled");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*===========================================================================*
 *= IviScope Configuration -> Configuration Information Group Functions =============================*
 *===========================================================================*/         

/*******************************************************************************
 * Function:  ag1000ni_ActualRecordLength
 * Purpose:   This function returns the actual number of points the
              oscilloscope acquires for each channel.  After you configure the
              oscilloscope for an acquisition, call this function to determine
              the size of the waveforms that the oscilloscope acquires.  The
              value is equal to or greater than the minimum number of points
              you specify in the ag1000ni_ConfigureAcquisitionRecord function. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ActualRecordLength (
                  ViSession vi, ViInt32* ActualRecordLength)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_HORZ_RECORD_LENGTH,
                 0, ActualRecordLength), 2, "Actual Record Length");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*******************************************************************************
 * Function:  ag1000ni_SampleMode
 * Purpose:   This function returns the sample mode the oscilloscope is
              currently using. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_SampleMode (
                  ViSession vi, ViInt32* SampleMode)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_SAMPLE_MODE,
                 0, SampleMode), 2, "Sample Mode");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_SampleRate
 * Purpose:   This function returns the effective sample rate of the acquired
              waveform using the current configuration in samples per second. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_SampleRate (
                  ViSession vi, ViReal64* SampleRate)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_GetAttributeViReal64 (vi, VI_NULL, AG1000NI_ATTR_HORZ_SAMPLE_RATE,
                 0, SampleRate), 2, "Sample Rate");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*===========================================================================*
 *= IviScope Configuration -> Trigger Group Functions =============================*
 *===========================================================================*/         

/*******************************************************************************
 * Function:  ag1000ni_ConfigureTrigger
 * Purpose:   This function configures the common attributes of the trigger
              subsystem.  These attributes are the trigger type and holdoff. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ConfigureTrigger (
                  ViSession vi, ViInt32 TriggerType, ViReal64 Holdoff)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_TYPE,
                 0, TriggerType), 2, "Trigger Type");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_HOLDOFF,
                 0, Holdoff), 3, "Holdoff");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_ConfigureTriggerCoupling
 * Purpose:   Sets the AG1000NI_ATTR_TRIGGER_COUPLING attribute of the driver. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ConfigureTriggerCoupling (
                  ViSession vi, ViInt32 Coupling)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_COUPLING,
                 0, Coupling), 2, "Coupling");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_ConfigureTriggerModifier
 * Purpose:   Sets the AG1000NI_ATTR_TRIGGER_MODIFIER attribute of the driver. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ConfigureTriggerModifier (
                  ViSession vi, ViInt32 TriggerModifier)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_MODIFIER,
                 0, TriggerModifier), 2, "Trigger Modifier");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_ConfigureTriggerCharacteristics
 * Purpose:   Configures additional characteristics for triggering. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ConfigureTriggerCharacteristics (
                  ViSession vi, ViReal64 Sensitivity, ViBoolean HFRejectFilterEnabled)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_SENSITIVITY,
                 0, Sensitivity), 2, "Sensitivity");
    viCheckParm( Ivi_SetAttributeViBoolean (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_HF_REJECT_ENABLED,
                 0, HFRejectFilterEnabled), 3, "HF Reject Filter Enabled");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_ConfigureEdgeTriggerSource
 * Purpose:   This function configures the edge trigger.  An edge trigger
              occurs when the trigger signal passes through the voltage
              threshold that you specify with the Trigger Level parameter and
              has the slope that you specify with the Trigger Slope parameter. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ConfigureEdgeTriggerSource (
                  ViSession vi, ViConstString Source, ViReal64 Level, 
                  ViInt32 Slope)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViString (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_SOURCE,
                 0, Source), 2, "Source");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_LEVEL,
                 0, Level), 3, "Level");
    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_SLOPE,
                 0, Slope), 4, "Slope");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_ConfigureTVTriggerSource
 * Purpose:   This function configures the TV trigger.  You configure the TV
              trigger source, signal format, the event on which to trigger,
              and the signal polarity. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ConfigureTVTriggerSource (
                  ViSession vi, ViConstString Source, ViInt32 TVSignalFormat, 
                  ViInt32 TVEvent, ViInt32 TVPolarity)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViString (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_SOURCE,
                 0, Source), 2, "Source");
    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TV_TRIGGER_SIGNAL_FORMAT,
                 0, TVSignalFormat), 3, "TV Signal Format");
    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TV_TRIGGER_EVENT,
                 0, TVEvent), 4, "TV Event");
    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TV_TRIGGER_POLARITY,
                 0, TVPolarity), 5, "TV Polarity");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_ConfigureTVTriggerLineNumber
 * Purpose:   This function configures the line on which TV trigger is set. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ConfigureTVTriggerLineNumber (
                  ViSession vi, ViInt32 TVLineNumber)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TV_TRIGGER_LINE_NUMBER,
                 0, TVLineNumber), 2, "TV Line Number");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_ConfigureTVTriggerCharacteristics
 * Purpose:   Sets the AG1000NI_ATTR_TV_TRIGGER_LEVEL attribute. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ConfigureTVTriggerCharacteristics (
                  ViSession vi, ViReal64 TVTriggerLevel)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AG1000NI_ATTR_TV_TRIGGER_LEVEL,
                 0, TVTriggerLevel), 2, "TV Trigger Level");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_ConfigurePatternTriggerSource
 * Purpose:   This function configures the Pattern trigger.  You configure the
              Pattern trigger source, trigger level and pattern. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ConfigurePatternTriggerSource (
                  ViSession vi, ViConstString Source, ViReal64 PatternTriggerLevel, 
                  ViConstString PatternTriggerPattern)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViString (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_SOURCE,
                 0, Source), 2, "Source");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AG1000NI_ATTR_PATTERN_TRIGGER_LEVEL,
                 0, PatternTriggerLevel), 3, "Pattern Trigger Level");
    viCheckParm( Ivi_SetAttributeViString (vi, VI_NULL, AG1000NI_ATTR_PATTERN_TRIGGER_PATTERN,
                 0, PatternTriggerPattern), 4, "Pattern Trigger Pattern");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_ConfigurePulseTriggerSource
 * Purpose:   This function configures the pulse trigger.  You configure the
              pulse trigger source, trigger mode, trigger level and trigger
              width. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ConfigurePulseTriggerSource (
                  ViSession vi, ViConstString Source, ViInt32 PulseTriggerMode, 
                  ViReal64 PulseTriggerLevel, ViReal64 PulseTriggerWidth)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViString (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_SOURCE,
                 0, Source), 2, "Source");
    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_PULSE_TRIGGER_MODE,
                 0, PulseTriggerMode), 3, "Pulse Trigger Mode");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AG1000NI_ATTR_PULSE_TRIGGER_LEVEL,
                 0, PulseTriggerLevel), 4, "Pulse Trigger Level");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AG1000NI_ATTR_PULSE_TRIGGER_WIDTH,
                 0, PulseTriggerWidth), 5, "Pulse Trigger Width");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_ConfigureAlternationTrigger
 * Purpose:   This function configures the alternation trigger. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ConfigureAlternationTrigger (
                  ViSession vi, ViInt32 Source, ViConstString CurrentSource, 
                  ViInt32 AltTriggerType, ViInt32 TriggerCoupling, ViReal64 TriggerHoldoff, 
                  ViReal64 TriggerSensitivity)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_ALTERNATION_TRIGGER_SOURCE,
                 0, Source), 2, "Source");
    viCheckParm( Ivi_SetAttributeViInt32 (vi, CurrentSource, AG1000NI_ATTR_ALTERNATION_TRIGGER_TYPE,
                 0, AltTriggerType), 4, "Alt Trigger Type");
    viCheckParm( Ivi_SetAttributeViInt32 (vi, CurrentSource, AG1000NI_ATTR_ALTERNATION_TRIGGER_COUPLING,
                 0, TriggerCoupling), 5, "Trigger Coupling");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, CurrentSource, AG1000NI_ATTR_ALTERNATION_TRIGGER_HOLDOFF,
                 0, TriggerHoldoff), 6, "Trigger Holdoff");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, CurrentSource, AG1000NI_ATTR_ALTERNATION_TRIGGER_SENSITIVITY,
                 0, TriggerSensitivity), 7, "Trigger Sensitivity");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_ConfigureAlternationEdgeTrigger
 * Purpose:   This function configures the alternation edge trigger.  An edge
              trigger occurs when the trigger signal passes through the
              voltage threshold that you specify with the Trigger Level
              parameter and has the slope that you specify with the Trigger
              Slope parameter. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ConfigureAlternationEdgeTrigger (
                  ViSession vi, ViConstString CurrentSource, ViReal64 Level, ViInt32 Slope)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    viCheckParm( Ivi_SetAttributeViReal64 (vi, CurrentSource, AG1000NI_ATTR_ALTERNATION_EDGE_TRIGGER_LEVEL,
                 0, Level), 3, "Level");
    viCheckParm( Ivi_SetAttributeViInt32 (vi, CurrentSource, AG1000NI_ATTR_ALTERNATION_EDGE_TRIGGER_SLOPE,
                 0, Slope), 4, "Slope");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_ConfigureAlternationPulseTrigger
 * Purpose:   This function configures the alternation pulse trigger.  You
              configure the pulse trigger source, trigger mode and trigger
              width.      
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ConfigureAlternationPulseTrigger (
                  ViSession vi, ViConstString CurrentSource, ViInt32 Mode, ViReal64 Width)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViInt32 (vi, CurrentSource, AG1000NI_ATTR_ALTERNATION_PULSE_TRIGGER_MODE,
                 0, Mode), 3, "Mode");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, CurrentSource, AG1000NI_ATTR_ALTERNATION_PULSE_TRIGGER_WIDTH,
                 0, Width), 4, "Width");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_ConfigureAlternationTVTrigger
 * Purpose:   This function configures the alternation TV trigger.  You
              configure the TV trigger source, signal format, the event on
              which to trigger, the signal polarity and line number. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ConfigureAlternationTVTrigger (
                  ViSession vi, ViConstString CurrentSource, ViInt32 SignalFormat, ViInt32 Event, 
                  ViInt32 Polarity, ViInt32 LineNumber)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViInt32 (vi, CurrentSource, AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_SIGNAL_FORMAT,
                 0, SignalFormat), 3, "Signal Format");
    viCheckParm( Ivi_SetAttributeViInt32 (vi, CurrentSource, AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_EVENT,
                 0, Event), 4, "Event");
    viCheckParm( Ivi_SetAttributeViInt32 (vi, CurrentSource, AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_POLARITY,
                 0, Polarity), 5, "Polarity");

    if (AG1000NI_VAL_TV_EVENT_LINE_NUMBER == Event)
        viCheckParm( Ivi_SetAttributeViInt32 (vi, CurrentSource, AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_LINE_NUMBER,
            0, LineNumber), 6, "Line Number");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}           

/*******************************************************************************
 * Function:  ag1000ni_ConfigureRefLevels
 * Purpose:   This function configures the common attributes of the
              acquisition subsystem when alternation trigger mode is enabled.
              These attributes are the time per record, and acquisition start
              time. 
*******************************************************************************/
ViStatus _VI_FUNC ag1000ni_ConfigureAlternationTriggerAcquisitionRecord (ViSession vi,
                                                                         ViReal64 timePerRecordSeconds,
                                                                         ViReal64 acquisitionStartTimeSecond)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AG1000NI_ATTR_ALTERNATION_TRIGGER_HORZ_TIME_PER_RECORD,
                 0, timePerRecordSeconds), 2, "Alternation Trigger Time Per Record");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AG1000NI_ATTR_ALTERNATION_TRIGGER_ACQUISITION_START_TIME,
                 0, acquisitionStartTimeSecond), 3, "Alternation Trigger Acquisition Start Time");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*===========================================================================*
 *= IviScope Configuration -> Measurements Group Functions =============================*
*===========================================================================*/  

/*******************************************************************************
 * Function:  ag1000ni_ConfigureRefLevels
 * Purpose:   This function configures the reference levels for waveform
              measurements. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ConfigureRefLevels (
                  ViSession vi, ViReal64 Low, ViReal64 Mid, 
                  ViReal64 High)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AG1000NI_ATTR_MEAS_LOW_REF,
                 0, Low), 2, "Low");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AG1000NI_ATTR_MEAS_MID_REF,
                 0, Mid), 3, "Mid");
    viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AG1000NI_ATTR_MEAS_HIGH_REF,
                 0, High), 4, "High");                              

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*===========================================================================*
 *= IviScope Configuration Group Functions =============================*
 *===========================================================================*/         

/*******************************************************************************
 * Function:  ag1000ni_AutoSetup
 * Purpose:   This function initiates an acquisition on the channels that you
              enable with the ag1000ni_ConfigureChannel function.  It then
              waits for the acquisition to complete, and returns the waveform
              for the channel you specify. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_AutoSetup (
                  ViSession vi)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (!Ivi_Simulating (vi))               /* call only when locked */
    {
        ViSession   io = Ivi_IOSession (vi);    /* call only when locked */

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
        viCheckErr( viPrintf (io, ":AUT;"));       

        Delay(0.2);
        checkErr( ag1000ni_DefaultInstrSetup(vi));
        error = ag1000ni_WaitForOPC (vi, 5000);
    }

    checkErr( Ivi_InvalidateAllAttributes (vi));

    checkErr( ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*===========================================================================*
 *= IviScope Waveform Acquisition Group Functions =============================*
 *===========================================================================*/         

/*******************************************************************************
 * Function:  ag1000ni_ReadWaveform
 * Purpose:   This function initiates an acquisition on the channels that you
              enable with the ag1000ni_ConfigureChannel function.  
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ReadWaveform (
                  ViSession vi, ViConstString Channel, ViInt32 WaveformSize, 
                  ViInt32 MaxTimeMilliseconds, ViReal64 WaveformArray[], ViInt32* ActualPoints, 
                  ViReal64* InitialX, ViReal64* XIncrement)
{
    ViStatus    error = VI_SUCCESS;
    ViBoolean   channelEnabled;
    ViInt32     acquisitionType;    

    checkErr( Ivi_LockSession (vi, VI_NULL));
    
    IviScope_InterchangeCheck (vi, "ag1000ni_ReadWaveform");

    /* Parameter Checking */
    /* Note: Not checking the following parameters: waveformSize, maxTime */

    viCheckParm(Ivi_CoerceChannelName (vi, Channel, &Channel), 2, "Channel");

    checkErr( Ivi_GetAttributeViBoolean (vi, Channel, AG1000NI_ATTR_CHANNEL_ENABLED, 0, &channelEnabled));
    if (!channelEnabled)
        viCheckErrElab (AG1000NI_ERROR_CHANNEL_NOT_ENABLED, "Cannot read waveform from disabled channel.");

    checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_ACQUISITION_TYPE, 0, &acquisitionType));

    if (AG1000NI_VAL_PEAK_DETECT == acquisitionType) {
        viCheckErrElab (IVI_ERROR_INVALID_CONFIGURATION, "Use ReadMinMaxWaveform for acquisition types Peak Detect.");
    }

    if (VI_NULL == WaveformArray)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 5, "Null address for Waveform");
    if (VI_NULL == ActualPoints)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 6, "Null address for Actual Points");
    if (VI_NULL == InitialX)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 7, "Null address for Initial X");
    if (VI_NULL == XIncrement)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 8, "Null address for X Increment");

    /* Read Waveform */
    checkErr( ag1000ni_InitiateAcquisition (vi)); 
    error = ag1000ni_WaitForOPC (vi, MaxTimeMilliseconds);

    if (VI_ERROR_TMO == error)
        error = AG1000NI_ERROR_MAX_TIME_EXCEEDED;
    viCheckErr( error);

    checkErr( ag1000ni_FetchWaveform (vi, Channel, WaveformSize, WaveformArray, ActualPoints, InitialX, XIncrement));
    checkErr( ag1000ni_CheckStatus (vi));                                      
Error:
    Ivi_UnlockSession (vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_ReadMinMaxWaveform
 * Purpose:   This function initiates an acquisition on the channels that you
              enable with the ag1000ni_ConfigureChannel function.  It then
              waits for the acquisition to complete, and returns the min/max
              waveforms for the channel you specify.
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ReadMinMaxWaveform (
                  ViSession vi, ViConstString Channel, ViInt32 WaveformSize, 
                  ViInt32 MaxTimeMilliseconds, ViReal64 MinWaveform[], ViReal64 MaxWaveform[], 
                  ViInt32* ActualPoints, ViReal64* InitialX, ViReal64* XIncrement)
{
    ViStatus    error = VI_SUCCESS;
    ViBoolean   channelEnabled;
    ViInt32     acquisitionType;    

    checkErr (Ivi_LockSession (vi, VI_NULL));

    IviScope_InterchangeCheck (vi, "ag1000ni_ReadMinMaxWaveform"); 
    /* Parameter Checking */
    /* Note: Not checking the following parameters: waveformSize, maxTime */

    viCheckParm (Ivi_CoerceChannelName (vi, Channel, &Channel), 2, "Channel");

    checkErr (Ivi_GetAttributeViBoolean (vi, Channel, AG1000NI_ATTR_CHANNEL_ENABLED, 0, &channelEnabled));
    if (!channelEnabled)
        viCheckErrElab (AG1000NI_ERROR_CHANNEL_NOT_ENABLED, "Cannot read waveform from disabled channel.");

    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_ACQUISITION_TYPE, 0, &acquisitionType));

    if (AG1000NI_VAL_PEAK_DETECT != acquisitionType) {
        viCheckErrElab (IVI_ERROR_INVALID_CONFIGURATION, "Use ReadMinMaxWaveform for acquisition types Peak Detect.");
    }

    if (VI_NULL == MinWaveform)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 5, "Null address for Min Waveform");
    if (VI_NULL == MaxWaveform)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 6, "Null address for Max Waveform");
    if (VI_NULL == ActualPoints)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 7, "Null address for Actual Points");
    if (VI_NULL == InitialX)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 8, "Null address for Initial X");
    if (VI_NULL == XIncrement)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 9, "Null address for X Increment");

    /* Read MinMax Waveform */
    checkErr( ag1000ni_InitiateAcquisition (vi)); 
    error = ag1000ni_WaitForOPC (vi, MaxTimeMilliseconds);

    if (VI_ERROR_TMO == error)
        error = AG1000NI_ERROR_MAX_TIME_EXCEEDED;
    viCheckErr( error);

    checkErr( ag1000ni_FetchMinMaxWaveform (vi, Channel, WaveformSize, MinWaveform, MaxWaveform, ActualPoints, InitialX, XIncrement));
    checkErr( ag1000ni_CheckStatus (vi));                                      
Error:
    Ivi_UnlockSession (vi, VI_NULL);
    return error;
}               

static IviRangeTableEntry measFuncTableEntries[] =       
    { /* Discrete Value,                   Not Used, Not Used,   CmdStr CmdValue*/
        {AG1000NI_VAL_VOLTAGE_PEAK_TO_PEAK,    0,        0,      "VPP",      0}, 
        {AG1000NI_VAL_VOLTAGE_MAX,             0,        0,      "VMAX",     0}, 
        {AG1000NI_VAL_VOLTAGE_MIN,             0,        0,      "VMIN",     0},         
        {AG1000NI_VAL_AMPLITUDE,               0,        0,      "VAMP",     0}, 
        {AG1000NI_VAL_VOLTAGE_HIGH,            0,        0,      "VTOP",     0}, 
        {AG1000NI_VAL_VOLTAGE_LOW,             0,        0,      "VBAS",     0}, 
        {AG1000NI_VAL_VOLTAGE_AVERAGE,         0,        0,      "VAV",      0}, 
        {AG1000NI_VAL_VOLTAGE_RMS,             0,        0,      "VRMS",     0}, 
        {AG1000NI_VAL_OVERSHOOT,               0,        0,      "OVER",     0},
        {AG1000NI_VAL_PRESHOOT,                0,        0,      "PRES",     0},
        {AG1000NI_VAL_FREQUENCY,               0,        0,      "FREQ",     0}, 
        {AG1000NI_VAL_RISE_TIME,               0,        0,      "RIS",      0}, 
        {AG1000NI_VAL_FALL_TIME,               0,        0,      "FALL",     0}, 
        {AG1000NI_VAL_PERIOD,                  0,        0,      "PER",      0}, 
        {AG1000NI_VAL_WIDTH_POS,               0,        0,      "PWID",     0}, 
        {AG1000NI_VAL_WIDTH_NEG,               0,        0,      "NWID",     0}, 
        {AG1000NI_VAL_DUTY_CYCLE_POS,          0,        0,      "PDUT",     0}, 
        {AG1000NI_VAL_DUTY_CYCLE_NEG,          0,        0,      "NDUT",     0}, 
        {IVI_RANGE_TABLE_LAST_ENTRY}  /* end of table */
    };   
static IviRangeTable measFuncTable =       
    {
        IVI_VAL_DISCRETE, VI_FALSE, VI_FALSE, VI_NULL, measFuncTableEntries
    };

/*******************************************************************************
 * Function:  ag1000ni_ReadWaveformMeasurement
 * Purpose:   This function initiates an acquisition on the channels that you
              enable with the ag1000ni_ConfigureChannel function.  It then
              waits for the acquisition to complete, obtains a waveform
              measurement on the channel you specify, and returns the
              measurement value. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ReadWaveformMeasurement (
                  ViSession vi, ViConstString Channel, ViInt32 MeasFunction, 
                  ViInt32 MaxTimeMilliseconds, ViReal64* Measurement)
{
    ViStatus    error = VI_SUCCESS;
    ViBoolean   channelEnabled;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    IviScope_InterchangeCheck (vi, "ag1000ni_ReadWaveformMeasurement");
    
    /* Parameter Checking */
    /* Note: Not checking the following parameters: maxTime */

    viCheckParm( Ivi_CoerceChannelName (vi, Channel, &Channel), 2, "Channel");
    checkErr( Ivi_GetAttributeViBoolean (vi, Channel, AG1000NI_ATTR_CHANNEL_ENABLED, 0, &channelEnabled));
    if (!channelEnabled)
        viCheckErrElab (AG1000NI_ERROR_CHANNEL_NOT_ENABLED, "Cannot read waveform from disabled channel.");

    viCheckParm( Ivi_GetViInt32EntryFromValue (MeasFunction, &measFuncTable, VI_NULL, VI_NULL, VI_NULL, VI_NULL, VI_NULL, VI_NULL), 
                                                3, "Measurement Function");
    if (VI_NULL == Measurement)
            viCheckParm (IVI_ERROR_INVALID_PARAMETER, 5, "Null address for Measurement");

    /* Read Waveform Measurement */
    checkErr (ag1000ni_InitiateAcquisition (vi));

    error = ag1000ni_WaitForOPC (vi, MaxTimeMilliseconds);

    if (VI_ERROR_TMO == error)
        error = AG1000NI_ERROR_MAX_TIME_EXCEEDED;
    viCheckErr( error);

    checkErr (ag1000ni_FetchWaveformMeasurement (vi, Channel, MeasFunction, Measurement));
    checkErr (ag1000ni_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_ReadDelayMeasurement
 * Purpose:   This function measures the phase in degrees between two
              edges. If two sources are specified, the phase from the
              specified edge of the first source to the specified edge of the
              second source is measured. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ReadDelayMeasurement (
                  ViSession vi, ViConstString Source1, ViConstString Source2, 
                  ViInt32 Direction, ViInt32 MaxTimeMilliSeconds, ViReal64* Measurement)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    /* Read Measurement */
    checkErr (ag1000ni_InitiateAcquisition (vi));

    error = ag1000ni_WaitForOPC (vi, MaxTimeMilliSeconds);

    if (VI_ERROR_TMO == error)
        error = AG1000NI_ERROR_MAX_TIME_EXCEEDED;
    viCheckErr( error);

    checkErr (ag1000ni_FetchDelayMeasurement (vi, Source1, Source2, Direction, Measurement));
    checkErr (ag1000ni_CheckStatus (vi));  
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_ReadPhaseMeasurement
 * Purpose:   This function measures the phase in degrees between two
              edges. If two sources are specified, the phase from the
              specified edge of the first source to the specified edge of the
              second source is measured. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_ReadPhaseMeasurement (
                  ViSession vi, ViConstString Source1, ViConstString Source2, 
                  ViInt32 Direction, ViInt32 MaxTimeMilliSeconds, ViReal64* Measurement)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    /* Read Measurement */
    checkErr (ag1000ni_InitiateAcquisition (vi));

    error = ag1000ni_WaitForOPC (vi, MaxTimeMilliSeconds);

    if (VI_ERROR_TMO == error)
        error = AG1000NI_ERROR_MAX_TIME_EXCEEDED;
    viCheckErr( error);

    checkErr (ag1000ni_FetchPhaseMeasurement (vi, Source1, Source2, Direction, Measurement));
    checkErr (ag1000ni_CheckStatus (vi)); 
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*===========================================================================*
 *= IviScope Waveform Acquisition -> Low-level Acquisition Group Functions =============================*
 *===========================================================================*/         

/*******************************************************************************
 * Function:  ag1000ni_InitiateAcquisition
 * Purpose:   This function initiates a waveform acquisition.  After you call
              this function, the oscilloscope leaves the Idle state and waits
              for a trigger.  The oscilloscope acquires a waveform for each
              channel you have enabled with the  ag1000ni_ConfigureChannel
              function. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_InitiateAcquisition (
                  ViSession vi)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    IviScope_InterchangeCheck (vi, "ag1000ni_InitiateAcquisition");
    
    if (!Ivi_Simulating (vi))                /* call only when locked */
    {
        ViSession   io = Ivi_IOSession (vi); /* call only when locked */

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

        viCheckErr( viBufWrite (io, ":SINGLE", 7, VI_NULL));
    }                 

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_AcquisitionStatus
 * Purpose:   This function returns whether an acquisition is in progress,
              complete, or if the status is unknown. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_AcquisitionStatus (
                  ViSession vi, ViInt32* Status)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     status = AG1000NI_VAL_ACQ_STATUS_UNKNOWN;
    ViSession   io;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (VI_NULL == Status)
            viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Status");
    
    io = Ivi_IOSession (vi);
    
    if (VI_ERROR_TMO != ag1000ni_WaitForOPCCallback (vi, io))
        status = AG1000NI_VAL_ACQ_COMPLETE;     

Error:
    *Status = status;
    Ivi_UnlockSession (vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_FetchWaveform
 * Purpose:   This function returns the waveform the oscilloscope acquires for
              the channel you specify.  The waveform is from an acquisition
              that you previously initiated. 
               
 * Note:      voltage = [(data value - YREFERENCE) * YINCREMENT] + YORIGIN 
              time = [(data point number - XREFERENCE) * XINCREMENT] + XORIGIN
              
              XINCREMENT - time difference between data points.
              XORIGIN    - always the first data point in memory.
              XREFERENCE - specifies the data point associated with x-origin.
              YINCREMENT - voltage diff between data points.
              YORIGIN    - value is the voltage at center screen.
              YREFERENCE - specifies the data point where y-origin occurs.
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_FetchWaveform (
                  ViSession vi, ViConstString Channel, ViInt32 WaveformSize, 
                  ViReal64 WaveformArray[], ViInt32* ActualPoints, ViReal64* InitialX, 
                  ViReal64* XIncrement)
{
    ViStatus    error = VI_SUCCESS;
    ViBoolean   channelEnabled;
    ViInt32     acquisitionType;    

    ViInt8      *rdBuf = VI_NULL;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    /* Parameter Checking */
    viCheckParm(Ivi_CoerceChannelName (vi, Channel, &Channel), 2, "Channel");

    checkErr( Ivi_GetAttributeViBoolean (vi, Channel, AG1000NI_ATTR_CHANNEL_ENABLED, 0, &channelEnabled));
    if (!channelEnabled)
        viCheckErrElab (AG1000NI_ERROR_CHANNEL_NOT_ENABLED, "Cannot read waveform from disabled channel.");

    checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_ACQUISITION_TYPE, 0, &acquisitionType));

    if (AG1000NI_VAL_PEAK_DETECT == acquisitionType) {
        viCheckErrElab (IVI_ERROR_INVALID_CONFIGURATION, "Use ReadMinMaxWaveform for acquisition types Peak Detect.");
    }

    if (VI_NULL == WaveformArray)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Waveform");
    if (VI_NULL == ActualPoints)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 5, "Null address for Actual Points");
    if (VI_NULL == InitialX)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 6, "Null address for Initial X");
    if (VI_NULL == XIncrement)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 7, "Null address for X Increment");

    /*Fetch Waveform */
    if (!Ivi_Simulating (vi))               /* call only when locked */
    {
        ViSession   io = Ivi_IOSession (vi);    /* call only when locked */
        ViChar      dummy;      
        ViChar      tempString[10]; 
        ViUInt32    length = 0;
        ViUInt32    readed_bytes = 0; 
        ViReal64    yIncrement = 0.0, yOrigin = 0.0, yReference = 0.0; 
        ViInt32     i = 0; 

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

        viCheckErr (viPrintf (io, ":WAV:SOUR %s;", Channel));               
        viCheckErr( viQueryf (io, ":WAV:YINC?", "%Le", &yIncrement));   
        viCheckErr( viQueryf (io, ":WAV:YOR?", "%Le", &yOrigin)); 
        viCheckErr( viQueryf (io, ":WAV:YREF?", "%Le", &yReference));
        viCheckErr( viQueryf (io, ":WAV:XINC?", "%Le", XIncrement));        
        viCheckErr( viQueryf (io, ":WAV:XOR?", "%Le", InitialX));  

        viCheckErr (viPrintf (io, ":WAV:DATA?"));
        do {
            viCheckErr (viRead (io, (ViPBuf)&dummy, 1, VI_NULL));        
        } while ( dummy != '#' );

        // number of characters for length
        viCheckErr (viRead (io, (ViPBuf)&dummy, 1, VI_NULL));       

        // terminate the string
        length = dummy - '0';

        viCheckErr (viRead (io, (ViPBuf)tempString, length, &readed_bytes));
        tempString[readed_bytes]=0;
        *ActualPoints = atoi(tempString);   

        if (*ActualPoints > WaveformSize)
            *ActualPoints = WaveformSize;

        checkErr (Ivi_Alloc (vi, *ActualPoints + 1, (ViAddr) &rdBuf));

        viCheckErr (viRead (io, (ViPBuf)rdBuf, *ActualPoints + 1, VI_NULL));

        //Read all the data in  the buffer
        if(rdBuf[*ActualPoints] != '\n') {      
            viScanf (io, "%*[^\n]");
        }

        for (i = 0; i < *ActualPoints; i++)
            WaveformArray[i] = (yReference - ((ViInt8)rdBuf[i])) * yIncrement + yOrigin;
    }
    else {
        ViInt32     x;
        ViReal64    yRange;
        ViInt32     triggerSlope, coup;
        ViReal64    k, level, theta;
        checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_HORZ_MIN_NUM_PTS, 0, ActualPoints));
        checkErr( Ivi_GetAttributeViReal64 (vi, Channel, AG1000NI_ATTR_VERTICAL_RANGE, 0, &yRange)); 
        checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_SLOPE, 0, &triggerSlope));
        checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_COUPLING, 0, &coup));
        checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_LEVEL, 0, &level));
        checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, AG1000NI_ATTR_HORZ_TIME_PER_RECORD, 0, XIncrement));
        theta = asin (2*level/yRange);
        if (AG1000NI_VAL_POSITIVE == triggerSlope)
            k = 1.0;
        else 
            k = -1.0;

        *InitialX = 0.0;
        if( *ActualPoints>WaveformSize ) 
            *ActualPoints = WaveformSize;  /* Checking number of points to write */
        for (x = 0; x < *ActualPoints; x++)
            WaveformArray[x] = 0.5 * k * yRange * sin (*XIncrement * x + k * theta) + (!(x%20)) * (16384 - rand())/150000.0;
    }                                                                                                                       

Error:
    if (VI_NULL != rdBuf) {
        Ivi_Free (vi, rdBuf);
        rdBuf = NULL;
    }
    Ivi_UnlockSession (vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_FetchMinMaxWaveform
 * Purpose:   This function returns the minimum and maximum waveforms that the
              oscilloscope acquires for the channel you specify.  The
              waveforms are from an acquisition that you previously initiated.
              Use this function to fetch waveforms when you set the
              acquisition type to AG1000NI_VAL_PEAK_DETECT or
              AG1000NI_VAL_ENVELOPE. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_FetchMinMaxWaveform (
                  ViSession vi, ViConstString Channel, ViInt32 WaveformSize, 
                  ViReal64 MinWaveform[], ViReal64 MaxWaveform[], ViInt32* ActualPoints, 
                  ViReal64* InitialX, ViReal64* XIncrement)
{
    ViStatus    error = VI_SUCCESS;
    ViBoolean   channelEnabled;
    ViInt32     acquisitionType;    

    ViInt8      *rdBuf = VI_NULL; 

    checkErr (Ivi_LockSession (vi, VI_NULL));

    /* Parameter Checking */
    /* Note: Not checking the following parameters: waveformSize, maxTime */

    viCheckParm (Ivi_CoerceChannelName (vi, Channel, &Channel), 2, "Channel");

    checkErr (Ivi_GetAttributeViBoolean (vi, Channel, AG1000NI_ATTR_CHANNEL_ENABLED, 0, &channelEnabled));
    if (!channelEnabled)
        viCheckErrElab (AG1000NI_ERROR_CHANNEL_NOT_ENABLED, "Cannot read waveform from disabled channel.");

    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_ACQUISITION_TYPE, 0, &acquisitionType));

    if (AG1000NI_VAL_PEAK_DETECT != acquisitionType) {
        viCheckErrElab (IVI_ERROR_INVALID_CONFIGURATION, "Use ReadMinMaxWaveform for acquisition types Peak Detect.");
    }

    if (VI_NULL == MinWaveform)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Min Waveform");
    if (VI_NULL == MaxWaveform)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 5, "Null address for Max Waveform");
    if (VI_NULL == ActualPoints)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 6, "Null address for Actual Points");
    if (VI_NULL == InitialX)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 7, "Null address for Initial X");
    if (VI_NULL == XIncrement)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 8, "Null address for X Increment");

    /*Fetch MinMax Waveform */
    if (!Ivi_Simulating (vi))               /* call only when locked */
    {
        ViSession   io = Ivi_IOSession (vi);    /* call only when locked */
        ViChar      dummy;      
        ViChar      tempString[10]; 
        ViUInt32    length = 0;
        ViUInt32    readed_bytes = 0; 
        ViReal64    yIncrement = 0.0, yOrigin = 0.0, yReference = 0.0; 
        ViInt32     i = 0; 

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

        viCheckErr (viPrintf (io, ":WAV:SOUR %s;", Channel));               
        viCheckErr( viQueryf (io, ":WAV:YINC?", "%le", &yIncrement));
        viCheckErr( viQueryf (io, ":WAV:YOR?", "%le", &yOrigin));   
        viCheckErr( viQueryf (io, ":WAV:XINC?", "%le", XIncrement));        
        viCheckErr( viQueryf (io, ":WAV:XOR?", "%le", InitialX));   

        viCheckErr (viPrintf (io, ":WAV:DATA?"));
        do {
            viCheckErr (viRead (io, (ViPBuf)&dummy, 1, VI_NULL));        
        } while ( dummy != '#' );

        // number of characters for length
        viCheckErr (viRead (io, (ViPBuf)&dummy, 1, VI_NULL));       

        // terminate the string
        length = dummy - '0';

        viCheckErr (viRead (io, (ViPBuf)tempString, length, &readed_bytes));
        tempString[readed_bytes]=0;
        *ActualPoints = atoi(tempString);   

        if (*ActualPoints > WaveformSize)
            *ActualPoints = WaveformSize;

        checkErr (Ivi_Alloc (vi, *ActualPoints + 1, (ViAddr) &rdBuf));

        viCheckErr (viRead (io, (ViPBuf)rdBuf, *ActualPoints + 1, VI_NULL));

        //Read all the data in  the buffer
        if(rdBuf[*ActualPoints] != '\n') {      
            viScanf (io, "%*[^\n]");
        }

        for (i = 0; i < (*ActualPoints)/2; i++) {
            MinWaveform[i] = (yReference - yOrigin - ((ViInt8)rdBuf[i<<1])) * yIncrement;
            MaxWaveform[i] = (yReference - yOrigin - ((ViInt8)rdBuf[(i<<1)+1])) * yIncrement;
        }
    }
    else {
        ViInt32     x;
        ViReal64    yRange;
        ViInt32     triggerSlope, coup;
        ViReal64    k, level, theta;
        checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_HORZ_MIN_NUM_PTS, 0, ActualPoints));
        checkErr( Ivi_GetAttributeViReal64 (vi, Channel, AG1000NI_ATTR_VERTICAL_RANGE, 0, &yRange)); 
        checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_SLOPE, 0, &triggerSlope));
        checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_COUPLING, 0, &coup));
        checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_LEVEL, 0, &level));
        checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, AG1000NI_ATTR_HORZ_TIME_PER_RECORD, 0, XIncrement));
        theta = asin (2*level/yRange);
        if (AG1000NI_VAL_POSITIVE == triggerSlope)
            k = 1.0;
        else 
            k = -1.0;

        *InitialX = 0.0;
        if( *ActualPoints>WaveformSize ) 
            *ActualPoints = WaveformSize;  /* Checking number of points to write */
        for (x = 0; x < *ActualPoints; x++) {
            /* add glitch to the every 20th element in the waveform array */
            MaxWaveform[x] = k * yRange * .49 * sin (*XIncrement * 12560 * x + k * theta - *InitialX) + (!(x%20)) * rand()/150000.0;
            MinWaveform[x] = k * yRange * .51 * sin (*XIncrement * 12560 * x + k * theta - *InitialX) - (!(x%20)) * rand()/160000.0;
        }
    }               
Error:
    if (VI_NULL != rdBuf) {
        Ivi_Free (vi, rdBuf);
        rdBuf = NULL;
    }
    Ivi_UnlockSession (vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_FetchWaveformMeasurement
 * Purpose:   This function obtains a waveform measurement and returns the
              measurement value.  You specify a particular measurement type,
              such as rise time, frequency, and voltage peak-to-peak.  The
              waveform on which the oscilloscope calculates the waveform
              measurement is from an acquisition that you previously
              initiated. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_FetchWaveformMeasurement (
                  ViSession vi, ViConstString Channel, ViInt32 MeasFunction, 
                  ViReal64* Measurement)
{
    ViStatus    error = VI_SUCCESS;
    ViBoolean   channelEnabled;
    ViString    cmd;     

    checkErr( Ivi_LockSession (vi, VI_NULL));

    /* Parameter Checking */
    /* Note: Not checking the following parameters: maxTime */

    viCheckParm( Ivi_CoerceChannelName (vi, Channel, &Channel), 2, "Channel");
    checkErr( Ivi_GetAttributeViBoolean (vi, Channel, AG1000NI_ATTR_CHANNEL_ENABLED, 0, &channelEnabled));
    if (!channelEnabled)
        viCheckErrElab (AG1000NI_ERROR_CHANNEL_NOT_ENABLED, "Cannot read waveform from disabled channel.");

    viCheckParm( Ivi_GetViInt32EntryFromValue (MeasFunction, &measFuncTable, VI_NULL, VI_NULL, VI_NULL, VI_NULL, &cmd, VI_NULL), 3, "Measurement Function");
    if (VI_NULL == Measurement)
            viCheckParm (IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Measurement");

    /* Fetch Waveform Measurement */
    if (!Ivi_Simulating (vi))               /* call only when locked */
    {  
        ViChar      measBuffer[BUFFER_SIZE];
        ViChar      *pMeasBuffer;

        ViSession   io = Ivi_IOSession (vi);    /* call only when locked */

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

        viCheckErr( viQueryf (io, ":MEAS:%s? %s", "%s", cmd, Channel, measBuffer));             

        pMeasBuffer = measBuffer;   

        if ((pMeasBuffer[0] >= '0' && pMeasBuffer[0] <= '9') || pMeasBuffer[0] == '+' || pMeasBuffer[0] == '-')         
            *Measurement = atof(pMeasBuffer);
        else
            viCheckErr (AG1000NI_ERROR_UNABLE_TO_PERFORM_MEASUREMENT);
    }
    else
    {
        ViReal64    range;

        checkErr (Ivi_GetAttributeViReal64 (vi, Channel, AG1000NI_ATTR_VERTICAL_RANGE, 0, &range));
        switch (MeasFunction) {
            case AG1000NI_VAL_RISE_TIME:
            case AG1000NI_VAL_FALL_TIME:
                *Measurement = 2e-6;
                break;
            case AG1000NI_VAL_FREQUENCY:
                *Measurement = rand ()/10.0 + 10000.0;
                break;
            case AG1000NI_VAL_PERIOD:
                *Measurement = 1.0/(rand ()/10.0 + 10000.0);
                break;
            case AG1000NI_VAL_VOLTAGE_RMS:
                *Measurement = range * .3535;
                break;
            case AG1000NI_VAL_VOLTAGE_PEAK_TO_PEAK:
                *Measurement = range;
                break;
            case AG1000NI_VAL_VOLTAGE_MAX:
            case AG1000NI_VAL_VOLTAGE_HIGH:
                *Measurement = (1 - rand ()/120000.0) * range/2;
                break;
            case AG1000NI_VAL_VOLTAGE_LOW:
            case AG1000NI_VAL_VOLTAGE_MIN:
                *Measurement = (rand ()/120000.0 - 1) * range/2;
                break;
            case AG1000NI_VAL_VOLTAGE_AVERAGE:
                *Measurement = 0;
                break;
            case AG1000NI_VAL_WIDTH_NEG:
            case AG1000NI_VAL_WIDTH_POS:
                *Measurement = 1e-4 * rand () / 30000.0;
                break;
            case AG1000NI_VAL_DUTY_CYCLE_NEG:
                *Measurement = 30.0;
                break;
            case AG1000NI_VAL_DUTY_CYCLE_POS:
                *Measurement = 70.0;
                break;
            case AG1000NI_VAL_AMPLITUDE:
                *Measurement = range * .49;
                break;
            case AG1000NI_VAL_OVERSHOOT:
                *Measurement = 23.3;
                break;
            case AG1000NI_VAL_PRESHOOT:
                *Measurement = 4.66;
                break;
            default:
                viCheckErr (IVI_ERROR_INVALID_VALUE);
        }
    }
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_FetchDelayMeasurement
 * Purpose:   This function measures the phase in degrees between two
              edges. If two sources are specified, the phase from the
              specified edge of the first source to the specified edge of the
              second source is measured. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_FetchDelayMeasurement (
                  ViSession vi, ViConstString Source1, ViConstString Source2, 
                  ViInt32 Direction, ViReal64* Measurement)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (AG1000NI_VAL_RISING != Direction && AG1000NI_VAL_FALLING != Direction)
            viCheckParm (IVI_ERROR_INVALID_PARAMETER, 4, "Invalid value for Direction");

    if (!Ivi_Simulating (vi)) {
        ViSession io = Ivi_IOSession (vi);

        viCheckErr (viQueryf (io, ":MEAS:%s? %s, %s", "%le", AG1000NI_VAL_RISING == Direction? "PDE" : "NDE", Source1, Source2, Measurement));
    }
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_FetchPhaseMeasurement
 * Purpose:   This function measures the phase in degrees between two
              edges. If two sources are specified, the phase from the
              specified edge of the first source to the specified edge of the
              second source is measured. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_FetchPhaseMeasurement (
                  ViSession vi, ViConstString Source1, ViConstString Source2, 
                  ViInt32 Direction, ViReal64* Measurement)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (AG1000NI_VAL_RISING != Direction && AG1000NI_VAL_FALLING != Direction)
            viCheckParm (IVI_ERROR_INVALID_PARAMETER, 4, "Invalid value for Direction");

    if (!Ivi_Simulating (vi)) {
        ViSession io = Ivi_IOSession (vi);

        viCheckErr (viQueryf (io, ":MEAS:%s? %s, %s", "%le", AG1000NI_VAL_RISING == Direction? "PPH" : "NPH", Source1, Source2, Measurement));
    }                              

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*******************************************************************************
 * Function:  ag1000ni_Abort
 * Purpose:   This function aborts an acquisition and returns the oscilloscope
              to the Idle state.  You initiate an acquisition with the
              ag1000ni_InitiateAcquisition function. 
*******************************************************************************/            
ViStatus _VI_FUNC ag1000ni_Abort (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (!Ivi_Simulating (vi))                /* call only when locked */
    {
        ViSession   io = Ivi_IOSession (vi); /* call only when locked */

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

        viCheckErr( viWrite (io, ":STOP;", 6, VI_NULL));
    }      
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}               

/*****************************************************************************
 * Function: ag1000ni_GetAttribute<type> Functions                                    
 * Purpose:  These functions enable the instrument driver user to get 
 *           attribute values directly.  There are typesafe versions for 
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession attributes.                                         
 *****************************************************************************/
ViStatus _VI_FUNC ag1000ni_GetAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViInt32 *value)
{                                                                                                           
    return Ivi_GetAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                    value);
}                                                                                                           
ViStatus _VI_FUNC ag1000ni_GetAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViReal64 *value)
{                                                                                                           
    return Ivi_GetAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}                                                                                                           
ViStatus _VI_FUNC ag1000ni_GetAttributeViString (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViInt32 bufSize, 
                                                 ViChar value[]) 
{   
    return Ivi_GetAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     bufSize, value);
}   
ViStatus _VI_FUNC ag1000ni_GetAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViBoolean *value)
{                                                                                                           
    return Ivi_GetAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           
ViStatus _VI_FUNC ag1000ni_GetAttributeViSession (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViSession *value)
{                                                                                                           
    return Ivi_GetAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           

/*****************************************************************************
 * Function: ag1000ni_SetAttribute<type> Functions                                    
 * Purpose:  These functions enable the instrument driver user to set 
 *           attribute values directly.  There are typesafe versions for 
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession datatypes.                                         
 *****************************************************************************/
ViStatus _VI_FUNC ag1000ni_SetAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViInt32 value)
{                                                                                                           
    return Ivi_SetAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                    value);
}                                                                                                           
ViStatus _VI_FUNC ag1000ni_SetAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViReal64 value)
{                                                                                                           
    return Ivi_SetAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}                                                                                                           
ViStatus _VI_FUNC ag1000ni_SetAttributeViString (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViConstString value) 
{   
    return Ivi_SetAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}   
ViStatus _VI_FUNC ag1000ni_SetAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViBoolean value)
{                                                                                                           
    return Ivi_SetAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           
ViStatus _VI_FUNC ag1000ni_SetAttributeViSession (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViSession value)
{                                                                                                           
    return Ivi_SetAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           

/*****************************************************************************
 * Function: ag1000ni_CheckAttribute<type> Functions                                  
 * Purpose:  These functions enable the instrument driver user to check  
 *           attribute values directly.  These functions check the value you
 *           specify even if you set the AG1000NI_ATTR_RANGE_CHECK 
 *           attribute to VI_FALSE.  There are typesafe versions for ViInt32, 
 *           ViReal64, ViString, ViBoolean, and ViSession datatypes.                         
 *****************************************************************************/
ViStatus _VI_FUNC ag1000ni_CheckAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViInt32 value)
{                                                                                                           
    return Ivi_CheckAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}
ViStatus _VI_FUNC ag1000ni_CheckAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                   ViAttr attributeId, ViReal64 value)
{                                                                                                           
    return Ivi_CheckAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                       value);
}                                                                                                           
ViStatus _VI_FUNC ag1000ni_CheckAttributeViString (ViSession vi, ViConstString channelName, 
                                                   ViAttr attributeId, ViConstString value)  
{   
    return Ivi_CheckAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                       value);
}   
ViStatus _VI_FUNC ag1000ni_CheckAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                    ViAttr attributeId, ViBoolean value)
{                                                                                                           
    return Ivi_CheckAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                        value);
}                                                                                                           
ViStatus _VI_FUNC ag1000ni_CheckAttributeViSession (ViSession vi, ViConstString channelName, 
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
ViStatus _VI_FUNC ag1000ni_WriteInstrData (ViSession vi, ViConstString writeBuffer)   
{   
    return Ivi_WriteInstrData (vi, writeBuffer);    
}   
ViStatus _VI_FUNC ag1000ni_ReadInstrData (ViSession vi, ViInt32 numBytes, 
                                          ViChar rdBuf[], ViInt32 *bytesRead)  
{   
    return Ivi_ReadInstrData (vi, numBytes, rdBuf, bytesRead);   
}           

/*****************************************************************************
 *-------------------- Utility Functions (Not Exported) ---------------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: ag1000ni_IviInit                                                       
 * Purpose:  This function is called by ag1000ni_InitWithOptions  
 *           or by an IVI class driver.  This function initializes the I/O 
 *           interface, optionally resets the device, optionally performs an
 *           ID query, and sends a default setup to the instrument.                
 *****************************************************************************/
static ViStatus ag1000ni_IviInit (ViRsrc resourceName, ViBoolean IDQuery,
                                    ViBoolean reset, ViSession vi)
{
    ViStatus    error = VI_SUCCESS;
    ViSession   io = VI_NULL;   
    ViInt32     model = AG1000NI_VAL_DSO1002A; 

    if (!Ivi_Simulating(vi))
    {
        ViSession   rmSession = VI_NULL;

        /* Open instrument session */
        checkErr( Ivi_GetAttributeViSession (vi, VI_NULL, IVI_ATTR_VISA_RM_SESSION, 0,
                                             &rmSession)); 
        viCheckErr( viOpen (rmSession, resourceName, VI_NULL, VI_NULL, &io));
        /* io session owned by driver now */
        checkErr( Ivi_SetAttributeViSession (vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, io));  

        /* Configure VISA Formatted I/O */
        viCheckErr( viSetAttribute (io, VI_ATTR_TMO_VALUE, 5000 ));
        viCheckErr( viSetBuf (io, VI_READ_BUF | VI_WRITE_BUF, 4000));
        viCheckErr( viSetAttribute (io, VI_ATTR_WR_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS));
        viCheckErr( viSetAttribute (io, VI_ATTR_RD_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS));
    }   

    if (Ivi_Simulating (vi))
        checkErr (ag1000ni_GetDriverSetupOption (vi, &model));
    else
        checkErr (ag1000ni_SetupInstrumentInfo (vi, &model));

    if (AG1000NI_VAL_DSO1002A == model ||
        AG1000NI_VAL_DSO1012A == model ||
        AG1000NI_VAL_DSO1022A == model) {
        checkErr (Ivi_BuildChannelTable (vi, TWO_CHANNEL_LIST, VI_FALSE, VI_NULL));
    } else {
        checkErr (Ivi_BuildChannelTable (vi, FOUR_CHANNEL_LIST, VI_FALSE, VI_NULL));
    }

    checkErr( Ivi_BuildRepCapTable (vi, ALTERNATION_TRIGGER_REP_CAP_NAME, ALTERNATION_TRIGGER_LIST));

     /* Add attributes */
    checkErr( ag1000ni_InitAttributes (vi));                
    checkErr (Ivi_SetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_INSTR_MODEL_TYPE, 0, model));      

    /*- Identification Query ------------------------------------------------*/
    if (IDQuery)                               
    {
        ViChar rdBuffer[BUFFER_SIZE];

        #define VALID_RESPONSE_STRING_START        "Agilent Technologies"

        checkErr( Ivi_GetAttributeViString (vi, VI_NULL, AG1000NI_ATTR_ID_QUERY_RESPONSE, 
                                            0, BUFFER_SIZE, rdBuffer));

        if (strncmp (rdBuffer, VALID_RESPONSE_STRING_START, 
                     strlen(VALID_RESPONSE_STRING_START)) != 0)
        {             
            viCheckErr( VI_ERROR_FAIL_ID_QUERY);
        }
    }

    /*- Reset instrument ----------------------------------------------------*/
    if (reset) 
        checkErr( ag1000ni_reset (vi));
    else  /*- Send Default Instrument Setup ---------------------------------*/
        checkErr( ag1000ni_DefaultInstrSetup (vi));

    //checkErr (Ivi_SetErrorInfo (vi, VI_FALSE, -1, -1, "-1"));
    checkErr (Ivi_SetErrorInfo (vi, VI_FALSE, -2, -2, "-2"));
    checkErr (Ivi_SetErrorInfo (vi, VI_FALSE, -3, -3, "-3"));
    checkErr (Ivi_SetErrorInfo (vi, VI_FALSE, -4, -4, "-4"));
    checkErr( ag1000ni_CheckStatus (vi));  

Error:
    if (error < VI_SUCCESS)
    {
        if (!Ivi_Simulating (vi) && io)
            viClose (io);
    }
    return error;
}

/*****************************************************************************
 * Function: ag1000ni_IviClose                                                        
 * Purpose:  This function performs all of the drivers clean-up operations   
 *           except for closing the IVI session.  This function is called by 
 *           ag1000ni_close or by an IVI class driver. 
 *
 *           Note:  This function must close the I/O session and set 
 *           IVI_ATTR_IO_SESSION to 0.
 *****************************************************************************/
static ViStatus ag1000ni_IviClose (ViSession vi)
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
 * Function: ag1000ni_CheckStatus                                                 
 * Purpose:  This function checks the status of the instrument to detect 
 *           whether the instrument has encountered an error.  This function  
 *           is called at the end of most exported driver functions.  If the    
 *           instrument reports an error, this function returns      
 *           IVI_ERROR_INSTRUMENT_SPECIFIC.  The user can set the 
 *           ag1000ni_ATTR_QUERY_INSTRUMENT_STATUS attribute to VI_FALSE to disable this 
 *           check and increase execution speed.                                   
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus ag1000ni_CheckStatus (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;

    if (Ivi_QueryInstrStatus (vi) && Ivi_NeedToCheckStatus (vi) && !Ivi_Simulating (vi))
    {
        checkErr( ag1000ni_CheckStatusCallback (vi, Ivi_IOSession(vi)));
        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_FALSE));
    }

Error:
    return error;
}

/*****************************************************************************
 * Function: ag1000ni_WaitForOPC                                                  
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
ViStatus ag1000ni_WaitForOPC (ViSession vi, ViInt32 maxTime) 
{
    ViStatus    error = VI_SUCCESS;

    if (!Ivi_Simulating(vi))
    {
        ViInt32  oldOPCTimeout;

        checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_OPC_TIMEOUT, 
                                           0, &oldOPCTimeout));
        Ivi_SetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_OPC_TIMEOUT,        
                                 0, maxTime);

        error = ag1000ni_WaitForOPCCallback (vi, Ivi_IOSession(vi));

        Ivi_SetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_OPC_TIMEOUT, 
                                 0, oldOPCTimeout);
        viCheckErr( error);
    }
Error:
    return error;
}

/*****************************************************************************
 * Function: ag1000ni_DefaultInstrSetup                                               
 * Purpose:  This function sends a default setup to the instrument.  The    
 *           ag1000ni_reset function calls this function.  The 
 *           ag1000ni_IviInit function calls this function when the
 *           user passes VI_FALSE for the reset parameter.  This function is 
 *           useful for configuring settings that other instrument driver 
 *           functions require.    
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus ag1000ni_DefaultInstrSetup (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     triggerType;

    /* Invalidate all attributes */
    checkErr( Ivi_InvalidateAllAttributes (vi));

    if (!Ivi_Simulating(vi))
    {
        ViSession   io = Ivi_IOSession(vi); /* call only when locked */

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

        viCheckErr( viPrintf (io, "*CLS"));
        viCheckErr (viPrintf (io, ":WAV:FORM BYTE"));
        viCheckErr (viPrintf (io, ":WAV:POIN:MODE RAW"));  
        viCheckErr (viPrintf (io, ":TIM:FORM YT"));  
        viCheckErr (viPrintf (io, ":TIM:MODE MAIN"));     
    }

    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_TYPE, 0, &triggerType));
    if (AG1000NI_VAL_ALTERNATION_TRIGGER != triggerType)
        checkErr( Ivi_SetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_MODIFIER,
            IVI_VAL_DONT_MARK_AS_SET_BY_USER, 
            AG1000NI_VAL_NO_TRIGGER_MOD));

    checkErr( Ivi_SetAttributeViBoolean (vi, VI_NULL, AG1000NI_ATTR_INITIATE_CONTINUOUS,
                                         IVI_VAL_DONT_MARK_AS_SET_BY_USER, VI_FALSE));
Error:
    return error;
}

/*****************************************************************************
 *  Function: ag1000ni_GetDriverSetupOption
 *  Purpose:  Get the value of a DriverSetup option.  The driver setup string
 *            is part of the option string the user passes to the
 *            ag1000ni_InitWithOptions function.  Specifically it is the part
 *            that follows "DriverSetup=".  This function assumes the 
 *            DriverSetup string has the following format:
 *              "OptionTag1 : OptionValue1; OptionTag2 : OptionValue2"
 *            Leading and trailing white space is removed from the option
 *            value and both the option tag and option value may contain
 *            white space.  The option tag is case sensitive.
 *
 *            Example:
 *
 *              "DriverSetup= Model : InstrX ; Special Option:Feature Y "
 *
 *              optionTag "Model"          returns optionValue "InstrX"
 *
 *              optionTag "Special Option" returns optionValue "Feature Y"
 *****************************************************************************/
static ViStatus ag1000ni_GetDriverSetupOption (ViSession vi, ViInt32 *model)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuf[BUFFER_SIZE]; 

    IviRangeTablePtr    table;
    IviRangeTableEntry  *entries;
    int                 i;

    ViString firstOccur;

    checkErr (Ivi_GetAttributeViString (vi, VI_NULL, IVI_ATTR_DRIVER_SETUP, 0, BUFFER_SIZE, rdBuf));

    firstOccur = strstr (rdBuf, "Model");
    if (NULL != firstOccur) {
        while (':' != *firstOccur)
            ++ firstOccur;
        ++ firstOccur;
        while (' ' == *firstOccur)
            ++ firstOccur;  /* Now it points to the model name */

        table = &attrInstrModelRangeTable;
        if (VI_NULL == table)
            viCheckErrElab (AG1000NI_ATTR_INSTR_MODEL_TYPE, "Unsupported Model Type");
        entries = table->rangeValues;
        if (VI_NULL == entries)
            viCheckErrElab (AG1000NI_ATTR_INSTR_MODEL_TYPE, "Unsupported Model Type");

        for (i = 0; ; i ++) {
            if (VI_NULL == entries[i].discreteOrMinValue)
                break;
            if (0 == strnicmp (firstOccur, entries[i].cmdString, strlen (entries[i].cmdString))) {
                *model = entries[i].discreteOrMinValue;
                break;
            }
        }

        if (VI_NULL == entries[i].discreteOrMinValue) {
                /* Not found */
            *model = AG1000NI_VAL_DSO1002A; 
        }
    } else {
        *model = AG1000NI_VAL_DSO1002A;
    }    
Error:  
    return error;
}

/*****************************************************************************
 * Function: ag1000ni_SetupInstrumentInfo                                                  
 * Purpose:  This function sends "*IDN?" command to the instrument and parse the
 *           returned string. It will then set the Model type attribute and installed options.
 *****************************************************************************/
static ViStatus ag1000ni_SetupInstrumentInfo (ViSession vi, ViInt32 *model)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuf[BUFFER_SIZE]; 

    IviRangeTablePtr    table;
    IviRangeTableEntry  *entries;
    int                 i;

    if (!Ivi_Simulating (vi)) {
        ViSession io = Ivi_IOSession (vi);
        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

        viCheckErr (viPrintf (io, "*IDN?"));
        viCheckErr (viScanf (io, "%T", rdBuf));

        table = &attrInstrModelRangeTable; 
        entries = table->rangeValues;
        if (VI_NULL == entries)
            viCheckErr (IVI_ERROR_INVALID_VALUE);

        for (i = 0; VI_NULL != entries[i].discreteOrMinValue; i ++) {
            if (VI_NULL != strstr (rdBuf, entries[i].cmdString)) {
                *model = entries[i].discreteOrMinValue;
                break;
            }
        }

        if (VI_NULL == entries[i].discreteOrMinValue) {
            /* Not found */
            *model = AG1000NI_VAL_DSO1002A;
        } 
    }
Error:
    return error;
}

/*****************************************************************************
 * Function: ReadToFile and WriteFromFile Functions                          
 * Purpose:  Functions for instrument driver developers to read/write        
 *           instrument data to/from a file.                                 
 *****************************************************************************/
static ViStatus ag1000ni_ReadToFile (ViSession vi, ViConstString filename, 
                                     ViInt32 maxBytesToRead, ViInt32 fileAction, 
                                     ViInt32 *totalBytesWritten)  
{   
    return Ivi_ReadToFile (vi, filename, maxBytesToRead, fileAction, totalBytesWritten);  
}   
static ViStatus ag1000ni_WriteFromFile (ViSession vi, ViConstString filename, 
                                        ViInt32 maxBytesToWrite, ViInt32 byteOffset, 
                                        ViInt32 *totalBytesWritten) 
{   
    return Ivi_WriteFromFile (vi, filename, maxBytesToWrite, byteOffset, totalBytesWritten); 
}

/*****************************************************************************
 *  Function: ag1000ni_GetStringFromTable
 *  Purpose:  Read the string command from rangetable    
 *****************************************************************************/
static ViStatus ag1000ni_GetStringFromTable(
        ag1000niStringValueTable table,
    ViInt32               value,
    ViConstString*        string
)
{
    ViStatus    error = VI_SUCCESS;
    ViBoolean   found = VI_FALSE;
    ViInt32     i;

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
 *  Function: ag1000ni_GetRepCommandFromTable
 *  Purpose:  Read the repeated command from range table to concat with the 
 *            string command.
 *****************************************************************************/
static ViStatus ag1000ni_GetRepCommandFromTable(
    ag1000niStringValueTable table,
    ViInt32               value,
    ViConstString*        repCommand
)
{
    ViStatus    error = VI_SUCCESS;
    ViBoolean   found = VI_FALSE;
    ViInt32     i;

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
 *  Function: ag1000ni_GetValueFromTable
 *  Purpose:  Read the enum value from table according to its command. 
 *****************************************************************************/
static ViStatus ag1000ni_GetValueFromTable(
        ag1000niStringValueTable    table,
    ViConstString           string,
    ViInt32*                value)
{
    ViStatus    error = VI_SUCCESS;
    ViBoolean   found = VI_FALSE;
    ViInt32     i;

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

/*****************************************************************************
 *------------------------ Global Session Callbacks -------------------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: ag1000ni_CheckStatusCallback                                               
 * Purpose:  This function queries the instrument to determine if it has 
 *           encountered an error.  If the instrument has encountered an 
 *           error, this function returns the IVI_ERROR_INSTRUMENT_SPECIFIC 
 *           error code.  This function is called by the 
 *           ag1000ni_CheckStatus utility function.  The 
 *           Ivi_SetAttribute and Ivi_GetAttribute functions invoke this 
 *           function when the optionFlags parameter includes the
 *           IVI_VAL_DIRECT_USER_CALL flag.
 *           
 *           The user can disable calls to this function by setting the 
 *           AG1000NI_QUERY_INSTRUMENT_STATUS attribute to VI_FALSE.  The driver can 
 *           disable the check status callback for a particular attribute by 
 *           setting the IVI_VAL_DONT_CHECK_STATUS flag.
 *****************************************************************************/
static ViStatus _VI_FUNC ag1000ni_CheckStatusCallback (ViSession vi, ViSession io)
{
    ViStatus    error = VI_SUCCESS;

Error:
    return error;
}

/*****************************************************************************
 * Function: ag1000ni_WaitForOPCCallback                                               
 * Purpose:  This function waits until the instrument has finished processing 
 *           all pending operations.  This function is called by the 
 *           ag1000ni_WaitForOPC utility function.  The IVI engine invokes
 *           this function in the following two cases:
 *           - Before invoking the read callback for attributes for which the 
 *             IVI_VAL_WAIT_FOR_OPC_BEFORE_READS flag is set.
 *           - After invoking the write callback for attributes for which the 
 *             IVI_VAL_WAIT_FOR_OPC_AFTER_WRITES flag is set.
 *****************************************************************************/
static ViStatus _VI_FUNC ag1000ni_WaitForOPCCallback (ViSession vi, ViSession io)
{   
    ViStatus    error = VI_SUCCESS;
    ViInt32     opcTimeout;
    ViInt32     time = 0;
    ViInt32     opc;
    ViBoolean   trigger = VI_FALSE;
    ViReal64    delayTime = 5.0e-3; 
    ViReal64    maxTimeout;

    checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_OPC_TIMEOUT, 
                                       0, &opcTimeout));

    maxTimeout = (opcTimeout/1000.0)/delayTime;
    while(time < maxTimeout) {
        viCheckErr( viQueryf (io, "*OPC?", "%d", &opc));    
        if (1 == opc) {
            trigger = VI_TRUE;
            break;
        }

        Delay(delayTime);
        time++;
    }

    if(!trigger)
        error = VI_ERROR_TMO;
Error:               
    return error;
}   

/*****************************************************************************
 *----------------- Attribute Range Tables and Callbacks --------------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: ag1000niAttrTriggerLevel_CommonCheckCallback                                               
 * Purpose:  This function checks the range for trigger level.
 *****************************************************************************/
static ViStatus _VI_FUNC ag1000niAttrTriggerLevel_CommonCheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;  
    ViChar      triggerSource[BUFFER_SIZE];
    ViReal64    verRange, scale;
    ViInt32     cmp;          

    checkErr (Ivi_GetAttributeViString (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_SOURCE, 0, BUFFER_SIZE, triggerSource));
    checkErr (Ivi_GetAttributeViReal64 (vi, triggerSource, AG1000NI_ATTR_VERTICAL_RANGE, 0, &verRange));

    scale = verRange / AG1000NI_NUM_VER_DIVS;

    viCheckErr (Ivi_CompareWithPrecision (8, value, - scale * 12, &cmp));
    if (cmp < 0)
        viCheckErr (IVI_ERROR_INVALID_VALUE);
    viCheckErr (Ivi_CompareWithPrecision (8, value, scale * 12, &cmp));
    if (cmp > 0)
        viCheckErr (IVI_ERROR_INVALID_VALUE);
Error:
    return error;
}

/*****************************************************************************
 * Function: ag1000niEnum_WriteCallback                                               
 * Purpose:  This function sets the enum value of the ViInt32 attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC ag1000niEnum_WriteCallback (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32         value)
{
    ViStatus            error = VI_SUCCESS;
    ViString            command,
                        commandValue;
    ViString            repCommand;                     
    IviRangeTablePtr    table;

    viCheckErr( ag1000ni_GetStringFromTable (ag1000niEnum_commands, attributeId, &command)); 
    viCheckErr (ag1000ni_GetRepCommandFromTable (ag1000niEnum_commands, attributeId, &repCommand));

    checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));
    viCheckErr( ag1000ni_GetCmdFromIntValue(value, table, &commandValue));

    if (!Ivi_Simulating (vi))
    {
        viCheckErr( viPrintf (io, "%s%s%s %s", command, VALID_CHANNELNAME (channelName), repCommand, commandValue));
    }
Error:
    return error;
}

/*****************************************************************************
 * Function: ag1000niEnum_ReadCallback                                               
 * Purpose:  This function reads the enum value of the ViInt32 attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC ag1000niEnum_ReadCallback (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32*        value
)
{
    ViStatus            error = VI_SUCCESS;
    ViString            command;
    ViString            repCommand; 
    IviRangeTablePtr    table;

    viCheckErr( ag1000ni_GetStringFromTable (ag1000niEnum_commands, attributeId, &command)); 
    viCheckErr (ag1000ni_GetRepCommandFromTable (ag1000niEnum_commands, attributeId, &repCommand));

    checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));

    if (!Ivi_Simulating (vi))
    {
        ViChar  rdBuffer[BUFFER_SIZE];
        ViInt32 rdBufferSize = (ViInt32)sizeof(rdBuffer);

        viCheckErr( viPrintf (io, "%s%s%s?", command, VALID_CHANNELNAME (channelName), repCommand));
        viCheckErr( viScanf(io, "%#s", &rdBufferSize, rdBuffer));
        viCheckErr( ag1000ni_GetValueFromCmd(rdBuffer, table, value));
    }
Error:
    return error;
}

/*****************************************************************************
 * Function: ag1000niEnum_ReadCallbackWithSpecifiedRangeTable                                               
 * Purpose:  This function reads the enum value of the ViInt32 attribute. 
 *           With additional range table.
 *****************************************************************************/
static ViStatus _VI_FUNC ag1000niEnum_ReadCallbackWithSpecifiedRangeTable (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    IviRangeTablePtr   tablePtr,
    ViInt32*        value
)
{
    ViStatus            error = VI_SUCCESS;
    ViString            command;
    ViString            repCommand; 

    viCheckErr( ag1000ni_GetStringFromTable (ag1000niEnum_commands, attributeId, &command)); 
    viCheckErr (ag1000ni_GetRepCommandFromTable (ag1000niEnum_commands, attributeId, &repCommand));

    if (!Ivi_Simulating (vi))
    {
        ViChar  rdBuffer[BUFFER_SIZE];
        size_t  rdBufferSize = sizeof(rdBuffer);

        viCheckErr( viPrintf (io, "%s%s%s?", command, VALID_CHANNELNAME (channelName), repCommand));
        viCheckErr( viScanf(io, "%#[^\n]", &rdBufferSize, rdBuffer));
        viCheckErr( ag1000ni_GetValueFromCmd(rdBuffer, tablePtr, value));
    }
Error:
    return error;
}

/*****************************************************************************
 * Function: ag1000niEnum_ReadCallbackWithSuffixChannelWithSpecifiedRangeTable                                               
 * Purpose:  This function reads the enum value of the ViInt32 attribute. 
 *           With additional range table and a suffix channel.
 *****************************************************************************/
static ViStatus _VI_FUNC ag1000niEnum_ReadCallbackWithSuffixChannelWithSpecifiedRangeTable (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    IviRangeTablePtr   table,
    ViInt32*        value
)
{
    ViStatus            error = VI_SUCCESS;
    ViString            command;
    ViString            repCommand; 

    viCheckErr( ag1000ni_GetStringFromTable (ag1000niEnum_commands, attributeId, &command)); 
    viCheckErr (ag1000ni_GetRepCommandFromTable (ag1000niEnum_commands, attributeId, &repCommand));

    if (!Ivi_Simulating (vi))
    {
        ViChar  rdBuffer[BUFFER_SIZE];
        ViInt32 rdBufferSize = (ViInt32)sizeof(rdBuffer);

        viCheckErr( viPrintf (io, "%s? %s%s", command, VALID_CHANNELNAME (channelName), repCommand));
        viCheckErr( viScanf(io, "%#[^\n]", &rdBufferSize, rdBuffer));
        viCheckErr( ag1000ni_GetValueFromCmd(rdBuffer, table, value));
    }
Error:
    return error;
}

/*****************************************************************************
 * Function: ag1000niEnum_WriteCallbackWithSuffixChannel                                               
 * Purpose:  This function sets the enum value of the ViInt32 attribute.
 *           With a suffix channel name.
 *****************************************************************************/
static ViStatus _VI_FUNC ag1000niEnum_WriteCallbackWithSuffixChannel (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32         value)
{
    ViStatus            error = VI_SUCCESS;
    ViString            command,
                        commandValue;
    ViString            repCommand;                     
    IviRangeTablePtr    table;

    viCheckErr( ag1000ni_GetStringFromTable (ag1000niEnum_commands, attributeId, &command)); 
    viCheckErr (ag1000ni_GetRepCommandFromTable (ag1000niEnum_commands, attributeId, &repCommand));

    checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));
    viCheckErr( ag1000ni_GetCmdFromIntValue(value, table, &commandValue));

    if (!Ivi_Simulating (vi))
    {
        viCheckErr( viPrintf (io, "%s %s,%s%s", command, commandValue, VALID_CHANNELNAME (channelName), repCommand));
    }
Error:
    return error;
}

/*****************************************************************************
 * Function: ag1000niEnum_ReadCallbackWithSuffixChannel                                               
 * Purpose:  This function reads the enum value of the ViInt32 attribute. 
 *           With a suffix channel name.
 *****************************************************************************/
static ViStatus _VI_FUNC ag1000niEnum_ReadCallbackWithSuffixChannel (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32*        value
)
{
    ViStatus            error = VI_SUCCESS;
    ViString            command;
    ViString            repCommand; 
    IviRangeTablePtr    table;

    viCheckErr( ag1000ni_GetStringFromTable (ag1000niEnum_commands, attributeId, &command)); 
    viCheckErr (ag1000ni_GetRepCommandFromTable (ag1000niEnum_commands, attributeId, &repCommand));

    checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));

    if (!Ivi_Simulating (vi))
    {
        ViChar  rdBuffer[BUFFER_SIZE];
        ViInt32 rdBufferSize = (ViInt32)sizeof(rdBuffer);

        viCheckErr( viPrintf (io, "%s? %s%s", command, VALID_CHANNELNAME (channelName), repCommand));
        viCheckErr( viScanf(io, "%#s", &rdBufferSize, rdBuffer));
        viCheckErr( ag1000ni_GetValueFromCmd(rdBuffer, table, value));
    }
Error:
    return error;
}

/*****************************************************************************
 * Function: ag1000niViBoolean_WriteCallback                                               
 * Purpose:  This function sets the bool value of the ViBoolean attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC ag1000niViBoolean_WriteCallback (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViBoolean       value
)
{
    ViStatus        error = VI_SUCCESS;
    ViString        command;
    ViString        repCommand;

    viCheckErr( ag1000ni_GetStringFromTable (ag1000niViBoolean_commands, attributeId, &command)); 
    viCheckErr (ag1000ni_GetRepCommandFromTable (ag1000niViBoolean_commands, attributeId, &repCommand));

    if (!Ivi_Simulating (vi)) {
        viCheckErr( viPrintf (io, "%s%s%s %s", command, VALID_CHANNELNAME (channelName), repCommand, value ? "ON" : "OFF"));
    }

Error:
    return error;
}

/*****************************************************************************
 * Function: ag1000niViBoolean_ReadCallback                                               
 * Purpose:  This function reads the bool value of the ViBoolean attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC ag1000niViBoolean_ReadCallback (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViBoolean*      value)
{
    ViStatus    error = VI_SUCCESS;
    ViString    command;
    ViString    repCommand; 

    viCheckErr( ag1000ni_GetStringFromTable (ag1000niViBoolean_commands, attributeId, &command)); 
    viCheckErr (ag1000ni_GetRepCommandFromTable (ag1000niViBoolean_commands, attributeId, &repCommand));

    if (!Ivi_Simulating (vi))
        viCheckErr( viQueryf (io, "%s%s%s?", "%hd", command, VALID_CHANNELNAME (channelName), repCommand, value));

Error:
    return error;
}

/*****************************************************************************
 * Function: ag1000niViString_WriteCallback                                               
 * Purpose:  This function sets the string value of the ViString attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC ag1000niViString_WriteCallback (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViConstString   value)
{
    ViStatus    error = VI_SUCCESS;
    ViString    command;
    ViString    repCommand;

    viCheckErr( ag1000ni_GetStringFromTable (ag1000niViString_commands, attributeId, &command));
    viCheckErr (ag1000ni_GetRepCommandFromTable (ag1000niViString_commands, attributeId, &repCommand));

    if (!Ivi_Simulating (vi)) {
        viCheckErr( viPrintf (io, "%s%s%s %s", command, VALID_CHANNELNAME (channelName), repCommand, value));

    }

Error:
    return error;
}

/*****************************************************************************
 * Function: ag1000niViString_ReadCallback                                               
 * Purpose:  This function reads the string value of the ViString attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC ag1000niViString_ReadCallback (
    ViSession           vi,
    ViSession           io,
    ViConstString       channelName,
    ViAttr              attributeId,
    const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViString    command;
    ViString    repCommand; 
    ViChar*     pdata = VI_NULL;
    ViChar*     pend  = VI_NULL;

    viCheckErr( ag1000ni_GetStringFromTable (ag1000niViString_commands, attributeId, &command)); 
    viCheckErr (ag1000ni_GetRepCommandFromTable (ag1000niViString_commands, attributeId, &repCommand));

    if (!Ivi_Simulating (vi))
    {
        ViChar      buffer[BUFFER_SIZE];
        ViChar*     pbuffer = VI_NULL;
        ViUInt32    allocated_count = 0;
        ViUInt32    count = 0;
        ViUInt32    readed_count;

        viCheckErr( viPrintf (io, "%s%s%s?", command, VALID_CHANNELNAME (channelName), repCommand));

        while ((error = viRead (io, buffer, BUFFER_SIZE, &readed_count)) == VI_SUCCESS_MAX_CNT)
        {
            if (allocated_count)
            {
                if (allocated_count <= count + readed_count)
                {
                    allocated_count += (allocated_count - count) + BUFFER_SIZE; 
                    pdata = realloc (pdata, allocated_count);
                }

                pend = pdata + count;
            }
            else
            {
                allocated_count = 2 * BUFFER_SIZE;
                pdata = malloc(allocated_count);
                pend  = pdata;
            }

            memcpy(pend, buffer, readed_count);
            count += readed_count;
        }

        if (readed_count > 0)
        {
            /*
             * Check if the last character is alnum.
             * if this character is not alnum we know that it's '\n'.
             * Remove it.
             */
            if (!isalnum(buffer[readed_count-1]))
                readed_count--;
            buffer[readed_count] = '\0';
        }

        /*
         * If function is read more data as BUFFER_SIZE, we must
         * copy last data from static buffer to dynamic buffer
         */
        if (pdata)
        {
            /*
             * For sure, check if the  buffer has enought element.
             * This condition should be FALSE every time.
             */
            if (allocated_count <= count + readed_count)
            {
                allocated_count += (allocated_count - count) + BUFFER_SIZE; 
                pdata = realloc (pdata, allocated_count);
            }
            pend = pdata + count;
            memcpy(pend, buffer, readed_count+1);           
            pbuffer = pdata;
        }
        else
            pbuffer = buffer;

        viCheckErr( error);

        checkErr( Ivi_SetValInStringCallback(vi, attributeId, pbuffer));
    }

Error:
    if (pdata)
        free( pdata);
    return error;
}

/*****************************************************************************
 * Function: ag1000niViReal64_WriteCallback                                               
 * Purpose:  This function sets the bool value of the ViReal64 attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC ag1000niViReal64_WriteCallback (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViReal64        value)
{
    ViStatus    error = VI_SUCCESS;
    ViString    command;
    ViString    repCommand;     

    viCheckErr( ag1000ni_GetStringFromTable (ag1000niViReal64_commands, attributeId, &command));
    viCheckErr (ag1000ni_GetRepCommandFromTable (ag1000niViReal64_commands, attributeId, &repCommand)); 

    if (!Ivi_Simulating (vi))
    {
        if(abs(value) < 1.0)
        {
            viCheckErr( viPrintf (io, "%s%s%s %Le", command, VALID_CHANNELNAME (channelName), repCommand, value));
        }
        else
        {
            viCheckErr( viPrintf (io, "%s%s%s %Lf", command, VALID_CHANNELNAME (channelName), repCommand, value));
        }
    }
Error:
    return error;
}

/*****************************************************************************
 * Function: ag1000niViReal64_ReadCallback                                               
 * Purpose:  This function reads the bool value of the ViReal64 attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC ag1000niViReal64_ReadCallback (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViReal64*       value)
{
    ViStatus    error = VI_SUCCESS;
    ViString    command;
    ViString    repCommand; 

    viCheckErr( ag1000ni_GetStringFromTable (ag1000niViReal64_commands, attributeId, &command)); 
    viCheckErr (ag1000ni_GetRepCommandFromTable (ag1000niViReal64_commands, attributeId, &repCommand)); 

    if (!Ivi_Simulating (vi))
        viCheckErr( viQueryf (io, "%s%s%s?", "%Le", command, VALID_CHANNELNAME (channelName), repCommand, value));

Error:
    return error;
}

/*****************************************************************************
 * Function: ag1000niViReal64_WriteCallbackWithSuffixChannel                                               
 * Purpose:  This function sets the bool value of the ViReal64 attribute.
 *           With a suffix channel name.
 *****************************************************************************/
static ViStatus _VI_FUNC ag1000niViReal64_WriteCallbackWithSuffixChannel (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViReal64        value)
{
    ViStatus    error = VI_SUCCESS;
    ViString    command;
    ViString    repCommand;     

    viCheckErr( ag1000ni_GetStringFromTable (ag1000niViReal64_commands, attributeId, &command));
    viCheckErr (ag1000ni_GetRepCommandFromTable (ag1000niViReal64_commands, attributeId, &repCommand)); 

    if (!Ivi_Simulating (vi))
    {
        if(abs(value) < 1.0)
        {
            viCheckErr( viPrintf (io, "%s %Le,%s%s", command, value, VALID_CHANNELNAME (channelName), repCommand));
        }
        else
        {
            viCheckErr( viPrintf (io, "%s %Lf,%s%s", command, value, VALID_CHANNELNAME (channelName), repCommand));
        }
    }
Error:
    return error;
}

/*****************************************************************************
 * Function: ag1000niViReal64_ReadCallbackWithSuffixChannel                                               
 * Purpose:  This function reads the bool value of the ViReal64 attribute.
 *           With a suffix channel name.
 *****************************************************************************/
static ViStatus _VI_FUNC ag1000niViReal64_ReadCallbackWithSuffixChannel (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViReal64*       value)
{
    ViStatus    error = VI_SUCCESS;
    ViString    command;
    ViString    repCommand; 

    viCheckErr( ag1000ni_GetStringFromTable (ag1000niViReal64_commands, attributeId, &command)); 
    viCheckErr (ag1000ni_GetRepCommandFromTable (ag1000niViReal64_commands, attributeId, &repCommand)); 

    if (!Ivi_Simulating (vi))
        viCheckErr( viQueryf (io, "%s? %s%s", "%Le", command, VALID_CHANNELNAME (channelName), repCommand, value));

Error:
    return error;
}

/*****************************************************************************
 * Function: ag1000niViInt32_WriteCallback                                               
 * Purpose:  This function sets the bool value of the ViInt32 attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC ag1000niViInt32_WriteCallback (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32         value
)
{
    ViStatus    error = VI_SUCCESS;
    ViString    command;
    ViString    repCommand;     

    viCheckErr( ag1000ni_GetStringFromTable (ag1000niViInt32_commands, attributeId, &command)); 
    viCheckErr (ag1000ni_GetRepCommandFromTable (ag1000niViInt32_commands, attributeId, &repCommand));  

    if (!Ivi_Simulating (vi)) {
        viCheckErr( viPrintf (io, "%s%s%s %ld", command, VALID_CHANNELNAME (channelName), repCommand, value));
    }

Error:
    return error;
}

/*****************************************************************************
 * Function: ag1000niViInt32_ReadCallback                                               
 * Purpose:  This function reads the bool value of the ViInt32 attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC ag1000niViInt32_ReadCallback (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32*        value
)
{
    ViStatus    error = VI_SUCCESS;
    ViString    command;
    ViString    repCommand; 

    viCheckErr( ag1000ni_GetStringFromTable (ag1000niViInt32_commands, attributeId, &command)); 
    viCheckErr (ag1000ni_GetRepCommandFromTable (ag1000niViInt32_commands, attributeId, &repCommand));  

    if (!Ivi_Simulating (vi))
        viCheckErr( viQueryf (io, "%s%s%s?", "%ld", command, VALID_CHANNELNAME (channelName), repCommand, value));

Error:
    return error;
}

/*****************************************************************************
 * Function: ag1000niViInt32_WriteCallbackWithSuffixChannel                                               
 * Purpose:  This function sets the bool value of the ViInt32 attribute.
 *           With a suffix channel name.  
 *****************************************************************************/
static ViStatus _VI_FUNC ag1000niViInt32_WriteCallbackWithSuffixChannel (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32         value
)
{
    ViStatus    error = VI_SUCCESS;
    ViString    command;
    ViString    repCommand;     

    viCheckErr( ag1000ni_GetStringFromTable (ag1000niViInt32_commands, attributeId, &command)); 
    viCheckErr (ag1000ni_GetRepCommandFromTable (ag1000niViInt32_commands, attributeId, &repCommand));  

    if (!Ivi_Simulating (vi)) {
        viCheckErr( viPrintf (io, "%s %ld,%s%s", command, value, VALID_CHANNELNAME (channelName), repCommand));
    }

Error:
    return error;
}

/*****************************************************************************
 * Function: ag1000niViInt32_ReadCallbackWithSuffixChannel                                               
 * Purpose:  This function reads the bool value of the ViInt32 attribute.
 *           With a suffix channel name.  
 *****************************************************************************/
static ViStatus _VI_FUNC ag1000niViInt32_ReadCallbackWithSuffixChannel (
    ViSession       vi,
    ViSession       io,
    ViConstString   channelName,
    ViAttr          attributeId,
    ViInt32*        value
)
{
    ViStatus    error = VI_SUCCESS;
    ViString    command;
    ViString    repCommand; 

    viCheckErr( ag1000ni_GetStringFromTable (ag1000niViInt32_commands, attributeId, &command)); 
    viCheckErr (ag1000ni_GetRepCommandFromTable (ag1000niViInt32_commands, attributeId, &repCommand));  

    if (!Ivi_Simulating (vi))
        viCheckErr( viQueryf (io, "%s? %s%s", "%ld", command, VALID_CHANNELNAME (channelName), repCommand, value));

Error:
    return error;
}

/*- AG1000NI_ATTR_ID_QUERY_RESPONSE -*/

static ViStatus _VI_FUNC ag1000niAttrIdQueryResponse_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    ViUInt32    retCnt;

    viCheckErr( viPrintf (io, "*IDN?"));
    viCheckErr( viRead (io, rdBuffer, BUFFER_SIZE-1, &retCnt));
    rdBuffer[retCnt] = 0;

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));

Error:
    return error;
}

/*- AG1000NI_ATTR_SPECIFIC_DRIVER_REVISION -*/

static ViStatus _VI_FUNC ag1000niAttrDriverRevision_ReadCallback (ViSession vi, 
                                                                  ViSession io,
                                                                  ViConstString channelName, 
                                                                  ViAttr attributeId,
                                                                  const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      driverRevision[256];

    sprintf (driverRevision, 
             "Driver: ag1000ni %d.%d, Compiler: %s %3.2f, "
             "Components: IVIEngine %.2f, VISA-Spec %.2f",
             AG1000NI_MAJOR_VERSION, AG1000NI_MINOR_VERSION, 
             IVI_COMPILER_NAME, IVI_COMPILER_VER_NUM, 
             IVI_ENGINE_MAJOR_VERSION + IVI_ENGINE_MINOR_VERSION/1000.0, 
             Ivi_ConvertVISAVer(VI_SPEC_VERSION));

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, driverRevision));    
Error:
    return error;
}

/*- AG1000NI_ATTR_INSTRUMENT_FIRMWARE_REVISION -*/

static ViStatus _VI_FUNC ag1000niAttrFirmwareRevision_ReadCallback (ViSession vi, 
                                                                    ViSession io,
                                                                    ViConstString channelName, 
                                                                    ViAttr attributeId,
                                                                    const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      idQ[BUFFER_SIZE], rdBuffer[BUFFER_SIZE];

    checkErr( Ivi_GetAttributeViString (vi, "", AG1000NI_ATTR_ID_QUERY_RESPONSE,
                                        0, BUFFER_SIZE, idQ));

    sscanf (idQ, "%*[^,],%*[^,],%*[^,],%256[^\n]", rdBuffer);

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));

Error:
    return error;
}

/*- AG1000NI_ATTR_INSTRUMENT_MANUFACTURER -*/

static ViStatus _VI_FUNC ag1000niAttrInstrumentManufacturer_ReadCallback (ViSession vi, 
                                                                          ViSession io,
                                                                          ViConstString channelName, 
                                                                          ViAttr attributeId,
                                                                          const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    ViChar      idQ[BUFFER_SIZE];

    checkErr( Ivi_GetAttributeViString (vi, "", AG1000NI_ATTR_ID_QUERY_RESPONSE,
                                        0, BUFFER_SIZE, idQ));
    sscanf (idQ, "%256[^,]", rdBuffer);

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));

Error:
    return error;
}

/*- AG1000NI_ATTR_INSTRUMENT_MODEL -*/

static ViStatus _VI_FUNC ag1000niAttrInstrumentModel_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    ViChar      idQ[BUFFER_SIZE];

    checkErr( Ivi_GetAttributeViString (vi, "", AG1000NI_ATTR_ID_QUERY_RESPONSE,
                                        0, BUFFER_SIZE, idQ));
    sscanf (idQ, "%*[^,],%256[^,]", rdBuffer);

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));

Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrInitiateContinuous_WriteCallback (ViSession vi,
                                                                       ViSession io,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViBoolean value)
{
    ViStatus    error = VI_SUCCESS;

    viCheckErr( viPrintf (io, "%s", value ? ":RUN" : ":SINGLE")); 
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrHorzTimePerRecord_ReadCallback (ViSession vi,
                                                                     ViSession io,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViReal64 *value)
{
    ViStatus    error = VI_SUCCESS;

    checkErr (ag1000niViReal64_ReadCallback (vi, io, channelName, attributeId, value));

    (*value) *= AG1000NI_NUM_HORZ_DIVS;
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrHorzTimePerRecord_WriteCallback (ViSession vi,
                                                                      ViSession io,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;

    checkErr (ag1000niViReal64_WriteCallback (vi, io, channelName, attributeId, value / AG1000NI_NUM_HORZ_DIVS));   

Error:
    return error;
}

static IviRangeTableEntry attrDSO102XAHorzTimePerRecordRangeTableEntries[] = 
        {
            {   0.0,         1.0e-9,     1e-9,         "",        0    },                         
            {   1.0e-9,      2.0e-9,     2e-9,         "",        0    },             
            {   2.0e-9,      5.0e-9,     5e-9,         "",        0    }, 
            {   5.0e-9,      1.0e-8,     1e-8,         "",        0    }, 
            {   1.0e-8,      2.0e-8,     2e-8,         "",        0    }, 
            {   2.0e-8,      5.0e-8,     5e-8,         "",        0    }, 
            {   5.0e-8,      1.0e-7,     1e-7,         "",        0    }, 
            {   1.0e-7,      2.0e-7,     2e-7,         "",        0    }, 
            {   2.0e-7,      5.0e-7,     5e-7,         "",        0    }, 
            {   5.0e-7,      1.0e-6,     1e-6,         "",        0    }, 
            {   1.0e-6,      2.0e-6,     2e-6,         "",        0    }, 
            {   2.0e-6,      5.0e-6,     5e-6,         "",        0    }, 
            {   5.0e-6,      1.0e-5,     1e-5,         "",        0    }, 
            {   1.0e-5,      2.0e-5,     2e-5,         "",        0    }, 
            {   2.0e-5,      5.0e-5,     5e-5,         "",        0    }, 
            {   5.0e-5,      1.0e-4,     1e-4,         "",        0    }, 
            {   1.0e-4,      2.0e-4,     2e-4,         "",        0    }, 
            {   2.0e-4,      5.0e-4,     5e-4,         "",        0    }, 
            {   5.0e-4,      1.0e-3,     1e-3,         "",        0    }, 
            {   1.0e-3,      2.0e-3,     2e-3,         "",        0    }, 
            {   2.0e-3,      5.0e-3,     5e-3,         "",        0    }, 
            {   5.0e-3,      1.0e-2,     1e-2,         "",        0    }, 
            {   1.0e-2,      2.0e-2,     2e-2,         "",        0    }, 
            {   2.0e-2,      5.0e-2,     5e-2,         "",        0    }, 
            {   5.0e-2,      1.0e-1,     1e-1,         "",        0    }, 
            {   1.0e-1,      2.0e-1,     2e-1,         "",        0    }, 
            {   2.0e-1,      5.0e-1,     5e-1,         "",        0    }, 
            {   5.0e-1,      1.0e+0,     1e+0,         "",        0    }, 
            {   1.0e+0,      2.0e+0,     2e+0,         "",        0    }, 
            {   2.0e+0,      5.0e+0,     5e+0,         "",        0    }, 
            {   5.0e+0,      1.0e+1,     1e+1,         "",        0    }, 
            {   1.0e+1,      2.0e+1,     2e+1,         "",        0    }, 
            {   2.0e+1,      5.0e+1,     5e+1,         "",        0    }, 
            {IVI_RANGE_TABLE_LAST_ENTRY}
        };
static IviRangeTable  attrDSO102XAHorzTimePerRecordRangeTable =
        {
            IVI_VAL_COERCED,                     
            VI_TRUE,                            
            VI_TRUE,                            
            "", 
            attrDSO102XAHorzTimePerRecordRangeTableEntries, 
        };

static IviRangeTableEntry attrDSO101XAHorzTimePerRecordRangeTableEntries[] = 
        {   
            {   0.0e-9,      2.0e-9,     2e-9,         "",        0    },             
            {   2.0e-9,      5.0e-9,     5e-9,         "",        0    }, 
            {   5.0e-9,      1.0e-8,     1e-8,         "",        0    }, 
            {   1.0e-8,      2.0e-8,     2e-8,         "",        0    }, 
            {   2.0e-8,      5.0e-8,     5e-8,         "",        0    }, 
            {   5.0e-8,      1.0e-7,     1e-7,         "",        0    }, 
            {   1.0e-7,      2.0e-7,     2e-7,         "",        0    }, 
            {   2.0e-7,      5.0e-7,     5e-7,         "",        0    }, 
            {   5.0e-7,      1.0e-6,     1e-6,         "",        0    }, 
            {   1.0e-6,      2.0e-6,     2e-6,         "",        0    }, 
            {   2.0e-6,      5.0e-6,     5e-6,         "",        0    }, 
            {   5.0e-6,      1.0e-5,     1e-5,         "",        0    }, 
            {   1.0e-5,      2.0e-5,     2e-5,         "",        0    }, 
            {   2.0e-5,      5.0e-5,     5e-5,         "",        0    }, 
            {   5.0e-5,      1.0e-4,     1e-4,         "",        0    }, 
            {   1.0e-4,      2.0e-4,     2e-4,         "",        0    }, 
            {   2.0e-4,      5.0e-4,     5e-4,         "",        0    }, 
            {   5.0e-4,      1.0e-3,     1e-3,         "",        0    }, 
            {   1.0e-3,      2.0e-3,     2e-3,         "",        0    }, 
            {   2.0e-3,      5.0e-3,     5e-3,         "",        0    }, 
            {   5.0e-3,      1.0e-2,     1e-2,         "",        0    }, 
            {   1.0e-2,      2.0e-2,     2e-2,         "",        0    }, 
            {   2.0e-2,      5.0e-2,     5e-2,         "",        0    }, 
            {   5.0e-2,      1.0e-1,     1e-1,         "",        0    }, 
            {   1.0e-1,      2.0e-1,     2e-1,         "",        0    }, 
            {   2.0e-1,      5.0e-1,     5e-1,         "",        0    }, 
            {   5.0e-1,      1.0e+0,     1e+0,         "",        0    }, 
            {   1.0e+0,      2.0e+0,     2e+0,         "",        0    }, 
            {   2.0e+0,      5.0e+0,     5e+0,         "",        0    }, 
            {   5.0e+0,      1.0e+1,     1e+1,         "",        0    }, 
            {   1.0e+1,      2.0e+1,     2e+1,         "",        0    }, 
            {   2.0e+1,      5.0e+1,     5e+1,         "",        0    }, 
            {IVI_RANGE_TABLE_LAST_ENTRY}
        };
static IviRangeTable  attrDSO101XAHorzTimePerRecordRangeTable =
        {
            IVI_VAL_COERCED,                     
            VI_TRUE,                            
            VI_TRUE,                            
            "", 
            attrDSO101XAHorzTimePerRecordRangeTableEntries, 
        };

static IviRangeTableEntry attrDSO100XAHorzTimePerRecordRangeTableEntries[] = 
        {  
            {   0.0e-9,      5.0e-9,     5e-9,         "",        0    }, 
            {   5.0e-9,      1.0e-8,     1e-8,         "",        0    }, 
            {   1.0e-8,      2.0e-8,     2e-8,         "",        0    }, 
            {   2.0e-8,      5.0e-8,     5e-8,         "",        0    }, 
            {   5.0e-8,      1.0e-7,     1e-7,         "",        0    }, 
            {   1.0e-7,      2.0e-7,     2e-7,         "",        0    }, 
            {   2.0e-7,      5.0e-7,     5e-7,         "",        0    }, 
            {   5.0e-7,      1.0e-6,     1e-6,         "",        0    }, 
            {   1.0e-6,      2.0e-6,     2e-6,         "",        0    }, 
            {   2.0e-6,      5.0e-6,     5e-6,         "",        0    }, 
            {   5.0e-6,      1.0e-5,     1e-5,         "",        0    }, 
            {   1.0e-5,      2.0e-5,     2e-5,         "",        0    }, 
            {   2.0e-5,      5.0e-5,     5e-5,         "",        0    }, 
            {   5.0e-5,      1.0e-4,     1e-4,         "",        0    }, 
            {   1.0e-4,      2.0e-4,     2e-4,         "",        0    }, 
            {   2.0e-4,      5.0e-4,     5e-4,         "",        0    }, 
            {   5.0e-4,      1.0e-3,     1e-3,         "",        0    }, 
            {   1.0e-3,      2.0e-3,     2e-3,         "",        0    }, 
            {   2.0e-3,      5.0e-3,     5e-3,         "",        0    }, 
            {   5.0e-3,      1.0e-2,     1e-2,         "",        0    }, 
            {   1.0e-2,      2.0e-2,     2e-2,         "",        0    }, 
            {   2.0e-2,      5.0e-2,     5e-2,         "",        0    }, 
            {   5.0e-2,      1.0e-1,     1e-1,         "",        0    }, 
            {   1.0e-1,      2.0e-1,     2e-1,         "",        0    }, 
            {   2.0e-1,      5.0e-1,     5e-1,         "",        0    }, 
            {   5.0e-1,      1.0e+0,     1e+0,         "",        0    }, 
            {   1.0e+0,      2.0e+0,     2e+0,         "",        0    }, 
            {   2.0e+0,      5.0e+0,     5e+0,         "",        0    }, 
            {   5.0e+0,      1.0e+1,     1e+1,         "",        0    }, 
            {   1.0e+1,      2.0e+1,     2e+1,         "",        0    }, 
            {   2.0e+1,      5.0e+1,     5e+1,         "",        0    }, 
            {IVI_RANGE_TABLE_LAST_ENTRY}
        };
static IviRangeTable  attrDSO100XAHorzTimePerRecordRangeTable =
        {
            IVI_VAL_COERCED,                     
            VI_TRUE,                            
            VI_TRUE,                            
            "", 
            attrDSO100XAHorzTimePerRecordRangeTableEntries, 
        };

static ViStatus _VI_FUNC ag1000niAttrHorzTimePerRecord_RangeTableCallback (ViSession vi,
                                                                            ViConstString channelName,
                                                                            ViAttr attributeId,
                                                                            IviRangeTablePtr *rangeTablePtr)
{
    ViStatus    error = VI_SUCCESS;
    IviRangeTablePtr    tblPtr = VI_NULL;

    ViInt32     model;

    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_INSTR_MODEL_TYPE, 0, &model));

    switch (model) {
        case AG1000NI_VAL_DSO1022A:
        case AG1000NI_VAL_DSO1024A:
             tblPtr = &attrDSO102XAHorzTimePerRecordRangeTable; 
             break;
        case AG1000NI_VAL_DSO1012A:
        case AG1000NI_VAL_DSO1014A:
             tblPtr = &attrDSO101XAHorzTimePerRecordRangeTable; 
             break;
        case AG1000NI_VAL_DSO1002A:
        case AG1000NI_VAL_DSO1004A:
             tblPtr = &attrDSO100XAHorzTimePerRecordRangeTable; 
             break;
        default:
             viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);
    }           
Error:
    *rangeTablePtr = tblPtr;
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrProbeAttenuation_ReadCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViReal64 *value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      cmd[BUFFER_SIZE];

    viCheckErr (viQueryf (io, ":%s:PROB?", "%s", channelName, cmd));
    cmd[strlen (cmd) - 1] = '\0'; // remove the suffix 'X'.        
    viCheckErr (sscanf (cmd, "%le", value));
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrProbeAttenuation_WriteCallback (ViSession vi,
                                                                     ViSession io,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     cmp;
    ViChar      cmd[BUFFER_SIZE];

    viCheckErr (Ivi_CompareWithPrecision (8, value, 0.001, &cmp));
    if (0 == cmp) {
        strncpy (cmd, "0.001X", BUFFER_SIZE);
        cmd[BUFFER_SIZE - 1] = '\0';
        goto SendCmd;
    }       
    viCheckErr (Ivi_CompareWithPrecision (8, value, 0.01, &cmp));
    if (0 == cmp) {
        strncpy (cmd, "0.01X", BUFFER_SIZE);
        cmd[BUFFER_SIZE - 1] = '\0';
        goto SendCmd;
    }      
    viCheckErr (Ivi_CompareWithPrecision (8, value, 0.1, &cmp));
    if (0 == cmp) {
        strncpy (cmd, "0.1X", BUFFER_SIZE);
        cmd[BUFFER_SIZE - 1] = '\0';
        goto SendCmd;
    }                         
    viCheckErr (Ivi_CompareWithPrecision (8, value, 1.0, &cmp));
    if (0 == cmp) {
        strncpy (cmd, "1X", BUFFER_SIZE);
        cmd[BUFFER_SIZE - 1] = '\0';
        goto SendCmd;
    }             
    viCheckErr (Ivi_CompareWithPrecision (8, value, 10.0, &cmp));
    if (0 == cmp) {
        strncpy (cmd, "10X", BUFFER_SIZE);
        cmd[BUFFER_SIZE - 1] = '\0';
        goto SendCmd;
    }  
    viCheckErr (Ivi_CompareWithPrecision (8, value, 100.0, &cmp));
    if (0 == cmp) {
        strncpy (cmd, "100X", BUFFER_SIZE);
        cmd[BUFFER_SIZE - 1] = '\0';
        goto SendCmd;
    }    
    viCheckErr (Ivi_CompareWithPrecision (8, value, 1000.0, &cmp));
    if (0 == cmp) {
        strncpy (cmd, "1000X", BUFFER_SIZE);
        cmd[BUFFER_SIZE - 1] = '\0';
        goto SendCmd;
    }

SendCmd:
    viCheckErr (viPrintf (io, ":%s:PROB %s", channelName, cmd));
Error:
    return error;
}         

static ViStatus _VI_FUNC ag1000niAttrMaxInputFrequency_ReadCallback (ViSession vi,
                                                                     ViSession io,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViReal64 *value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuf[BUFFER_SIZE];

    viCheckErr (viQueryf (io, ":%s:BWL?", "%s", channelName, rdBuf));

    // If the filter is enabled, max freq is 25MHz.
    if (stricmp (rdBuf, "1") == 0)
        *value = 25.0e6;
    else
        *value = IVI_VIREAL64_MAX;
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrMaxInputFrequency_WriteCallback (ViSession vi,
                                                                      ViSession io,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     cmp;

    viCheckErr (Ivi_CompareWithPrecision (8, value, 25.0e6, &cmp));

    viCheckErr (viPrintf (io, ":%s:BWL %d", channelName, cmp <= 0 ? 1 : 0));
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrTriggerSource_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     triggerType;
    ViAttr      attrId = 0; 
    ViChar      rdBuf[BUFFER_SIZE];

    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_TYPE, 0, &triggerType));

    switch (triggerType) {
        case AG1000NI_VAL_EDGE_TRIGGER:
            attrId = AG1000NI_ATTR_INSTR_EDGE_TRIGGER_SOURCE;
            break;
        case AG1000NI_VAL_TV_TRIGGER:
            attrId = AG1000NI_ATTR_INSTR_TV_TRIGGER_SOURCE;
            break;
        case AG1000NI_VAL_PULSE_TRIGGER:
            attrId = AG1000NI_ATTR_INSTR_PULSE_TRIGGER_SOURCE;
            break;
        case AG1000NI_VAL_PATTERN_TRIGGER:
            attrId = AG1000NI_ATTR_INSTR_PATTERN_TRIGGER_SOURCE;
            break;
        case AG1000NI_VAL_ALTERNATION_TRIGGER:
            break;
        default:
            viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);
    }

    if (0 != attrId) {
        checkErr (Ivi_GetAttributeViString (vi, channelName, attrId, 0, BUFFER_SIZE, rdBuf));
        checkErr (Ivi_SetValInStringCallback (vi, attributeId, rdBuf));
    }
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrTriggerSource_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViConstString value)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     triggerType;
    ViAttr      attrId = 0;

    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_TYPE, 0, &triggerType));

    switch (triggerType) {
        case AG1000NI_VAL_EDGE_TRIGGER:
            attrId = AG1000NI_ATTR_INSTR_EDGE_TRIGGER_SOURCE;
            break;
        case AG1000NI_VAL_TV_TRIGGER:
            attrId = AG1000NI_ATTR_INSTR_TV_TRIGGER_SOURCE;
            break;
        case AG1000NI_VAL_PULSE_TRIGGER:
            attrId = AG1000NI_ATTR_INSTR_PULSE_TRIGGER_SOURCE;
            break;
        case AG1000NI_VAL_PATTERN_TRIGGER:
            attrId = AG1000NI_ATTR_INSTR_PATTERN_TRIGGER_SOURCE;
            break;
        case AG1000NI_VAL_ALTERNATION_TRIGGER:
            break;
        default:
            viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);
    }

    if (0 != attrId)
        checkErr (Ivi_SetAttributeViString (vi, channelName, attrId, 0, value));
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrInstrEdgeTriggerSource_CheckCallback (ViSession vi,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViConstString value)
{
    ViStatus    error = VI_SUCCESS;

    if (stricmp (value, "EXT") != 0 &&
        stricmp (value, "EXT5") != 0 &&
        stricmp (value, "ACLINE") != 0) {
        checkErr (Ivi_CoerceChannelName (vi, channelName, VI_NULL));
    }
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrCommonTriggerSource_CheckCallback (ViSession vi,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViConstString value)
{
    ViStatus    error = VI_SUCCESS;
    if (stricmp (value, "EXT") != 0 &&
        stricmp (value, "EXT5") != 0) {
        checkErr (Ivi_CoerceChannelName (vi, channelName, VI_NULL));
    }
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrTriggerModifier_ReadCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViInt32 *value)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     triggerType;
    ViAttr      attrId;

    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_TYPE, 0, &triggerType));

    switch (triggerType) {
        case AG1000NI_VAL_EDGE_TRIGGER:
            attrId = AG1000NI_ATTR_INSTR_EDGE_TRIGGER_MODIFIER;
            break;
        case AG1000NI_VAL_TV_TRIGGER:
            attrId = AG1000NI_ATTR_INSTR_TV_TRIGGER_MODIFIER;
            break;
        case AG1000NI_VAL_PULSE_TRIGGER:
            attrId = AG1000NI_ATTR_INSTR_PULSE_TRIGGER_MODIFIER;
            break;
        case AG1000NI_VAL_PATTERN_TRIGGER:
            attrId = AG1000NI_ATTR_INSTR_PATTERN_TRIGGER_MODIFIER;
            break;
        case AG1000NI_VAL_ALTERNATION_TRIGGER:
        default:
            viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);
    }

    checkErr (Ivi_GetAttributeViInt32 (vi, channelName, attrId, 0, value));
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrTriggerModifier_WriteCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViInt32 value)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     triggerType;
    ViAttr      attrId;

    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_TYPE, 0, &triggerType));

    switch (triggerType) {
        case AG1000NI_VAL_EDGE_TRIGGER:
            attrId = AG1000NI_ATTR_INSTR_EDGE_TRIGGER_MODIFIER;
            break;
        case AG1000NI_VAL_TV_TRIGGER:
            attrId = AG1000NI_ATTR_INSTR_TV_TRIGGER_MODIFIER;
            break;
        case AG1000NI_VAL_PULSE_TRIGGER:
            attrId = AG1000NI_ATTR_INSTR_PULSE_TRIGGER_MODIFIER;
            break;
        case AG1000NI_VAL_PATTERN_TRIGGER:
            attrId = AG1000NI_ATTR_INSTR_PATTERN_TRIGGER_MODIFIER;
            break;
        case AG1000NI_VAL_ALTERNATION_TRIGGER:
        default:
            viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);
    }

    checkErr (Ivi_SetAttributeViInt32 (vi, channelName, attrId, 0, value));
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrTvTriggerLineNumber_CheckCallback (ViSession vi,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViInt32 value)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     tvTriggerType;
    ViInt32     signalFormat;
    ViInt32     minValue = 1, maxValue;

    checkErr (Ivi_DefaultCheckCallbackViInt32 (vi, channelName, attributeId, value));

    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TV_TRIGGER_EVENT, 0, &tvTriggerType));
    if (AG1000NI_VAL_TV_EVENT_LINE_NUMBER != tvTriggerType)
        viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);

    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TV_TRIGGER_SIGNAL_FORMAT, 0, &signalFormat));

    if (AG1000NI_VAL_NTSC == signalFormat) 
        maxValue = 525;
    else
        maxValue = 625;

    if (value < minValue || value > maxValue)
        viCheckErr (IVI_ERROR_INVALID_VALUE);
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrPatternTriggerPattern_CheckCallback (ViSession vi,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViConstString value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      buf[BUFFER_SIZE];
    ViInt32     val, mask, extSetting, edgeSource, edgeDir;
    ViString    token;
    // value
    strncpy (buf, value, sizeof (buf));
    buf[BUFFER_SIZE-1] = '\0';
    token = strtok (buf, ",");
    if (NULL == token) 
        viCheckErr (IVI_ERROR_INVALID_VALUE);
    val = atoi (token);
    if (val < 0 || val > 0x1F)
        viCheckErr (IVI_ERROR_INVALID_VALUE);       
    // mask 
    token = strtok (NULL, ",");
    if (NULL == token) 
        viCheckErr (IVI_ERROR_INVALID_VALUE);
    mask = atoi (token);
    if (mask < 0 || mask > 0x1F)
        viCheckErr (IVI_ERROR_INVALID_VALUE);
    // ext setting
    token = strtok (NULL, ",");
    if (NULL == token) 
        viCheckErr (IVI_ERROR_INVALID_VALUE);
    extSetting = atoi (token);
    if (extSetting != 0 && extSetting != 1)
        viCheckErr (IVI_ERROR_INVALID_VALUE);
    // edge source
    token = strtok (NULL, ",");
    if (NULL != token) {
        edgeSource = atoi (token);
        if (edgeSource < 0 && edgeSource > 5)
            viCheckErr (IVI_ERROR_INVALID_VALUE);
        // edge dir
        token = strtok (NULL, ",");
        if (NULL == token) 
            viCheckErr (IVI_ERROR_INVALID_VALUE);
        edgeDir = atoi (token);
        if (edgeDir != 0 && edgeDir != 1)
            viCheckErr (IVI_ERROR_INVALID_VALUE);   
    }
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrAcquisitionStartTime_CheckCallback (ViSession vi,
                                                                         ViConstString channelName,
                                                                         ViAttr attributeId,
                                                                         ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViReal64    scale;
    ViReal64    minTime, 
                maxTime;
    ViInt32     cmp;

    checkErr (Ivi_DefaultCheckCallbackViReal64 (vi, channelName, attributeId, value));

    checkErr (Ivi_GetAttributeViReal64 (vi, VI_NULL, AG1000NI_ATTR_HORZ_TIME_PER_RECORD, 0, &scale));
    scale /= AG1000NI_NUM_HORZ_DIVS;

    viCheckErr (Ivi_CompareWithPrecision (8, scale, 50.0e-3, &cmp));

    if (cmp < 0) {
        minTime = -14 * scale;
        maxTime = 1.0;
    }
    else {
        minTime = -6 * scale;
        maxTime =  6 * scale;
    }

    viCheckErr (Ivi_CheckNumericRange (value, minTime, maxTime, IVI_ERROR_INVALID_VALUE));
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrAlternationTriggerHorzTimePerRecord_ReadCallback (ViSession vi,
                                                                                       ViSession io,
                                                                                       ViConstString channelName,
                                                                                       ViAttr attributeId,
                                                                                       ViReal64 *value)
{
    ViStatus    error = VI_SUCCESS;

    checkErr (ag1000niViReal64_ReadCallbackWithSuffixChannel (vi, io, channelName, attributeId, value));

    (*value) *= AG1000NI_NUM_HORZ_DIVS;

Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrAlternationTriggerHorzTimePerRecord_WriteCallback (ViSession vi,
                                                                                        ViSession io,
                                                                                        ViConstString channelName,
                                                                                        ViAttr attributeId,
                                                                                        ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;

    checkErr (ag1000niViReal64_WriteCallbackWithSuffixChannel (vi, io, channelName, attributeId, value / AG1000NI_NUM_HORZ_DIVS));   

Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrVerticalRange_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 *value)
{
    ViStatus    error = VI_SUCCESS;

    checkErr (ag1000niViReal64_ReadCallback (vi, io, channelName, attributeId, value));

    (*value) *= AG1000NI_NUM_VER_DIVS;

Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrVerticalRange_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;

    checkErr (ag1000niViReal64_WriteCallback (vi, io, channelName, attributeId, value / AG1000NI_NUM_VER_DIVS));   

Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrVerticalRange_CheckCallback (ViSession vi,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     cmp   = 0;
    ViReal64    minValue = 2.0e-3;
    ViReal64    maxValue = 10.0;
    ViReal64    probeAttn;

    checkErr (Ivi_GetAttributeViReal64 (vi, channelName, AG1000NI_ATTR_PROBE_ATTENUATION, 0, &probeAttn));

    minValue *= probeAttn;
    maxValue *= probeAttn;

    value /= AG1000NI_NUM_VER_DIVS;

    viCheckErr (Ivi_CompareWithPrecision (8, value, minValue, &cmp));
    if (cmp < 0) 
        viCheckErr (IVI_ERROR_INVALID_VALUE);
    viCheckErr (Ivi_CompareWithPrecision (8, value, maxValue, &cmp));
    if (cmp > 0)
        viCheckErr (IVI_ERROR_INVALID_VALUE);
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrTriggerLevel_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     triggerType;

    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_TYPE, 0, &triggerType));
    if (AG1000NI_VAL_EDGE_TRIGGER != triggerType)
        viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);

    checkErr (ag1000niAttrTriggerLevel_CommonCheckCallback (vi, channelName, attributeId, value));
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrTvTriggerLevel_CheckCallback (ViSession vi,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     triggerType;

    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_TYPE, 0, &triggerType));
    if (AG1000NI_VAL_TV_TRIGGER != triggerType)
        viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);

    checkErr (ag1000niAttrTriggerLevel_CommonCheckCallback (vi, channelName, attributeId, value));
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrPatternTriggerLevel_CheckCallback (ViSession vi,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     triggerType;

    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_TYPE, 0, &triggerType));
    if (AG1000NI_VAL_PATTERN_TRIGGER != triggerType)
        viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);

    checkErr (ag1000niAttrTriggerLevel_CommonCheckCallback (vi, channelName, attributeId, value));
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrPulseTriggerLevel_CheckCallback (ViSession vi,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     triggerType;

    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_TYPE, 0, &triggerType));
    if (AG1000NI_VAL_PULSE_TRIGGER != triggerType)
        viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);

    checkErr (ag1000niAttrTriggerLevel_CommonCheckCallback (vi, channelName, attributeId, value));
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrAlternationEdgeTriggerLevel_CheckCallback (ViSession vi,
                                                                                ViConstString channelName,
                                                                                ViAttr attributeId,
                                                                                ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     triggerSource;
    ViString    triggerSourceBuf;
    ViChar      channel[BUFFER_SIZE];
    IviRangeTablePtr tablePtr;
    ViReal64    verRange, scale;
    ViInt32     cmp;

    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_ALTERNATION_TRIGGER_SOURCE, 0, &triggerSource));  
    checkErr (Ivi_GetAttrRangeTable (vi, VI_NULL, AG1000NI_ATTR_ALTERNATION_TRIGGER_SOURCE, &tablePtr));    
    viCheckErr (ag1000ni_GetCmdFromIntValue (triggerSource, tablePtr, &triggerSourceBuf));

    if (stricmp (channelName, "SOURA") == 0) {
        strncpy (channel, triggerSourceBuf, 3);
    } else {
        strncpy (channel, triggerSourceBuf + 3, 3);
    }

    // Now channel looks like CH1, move the number back
    channel[4] = channel[2];
    // Complete the channel name
    strncpy (channel + 2, "AN", 2);
    channel[5] = '\0';
    // Now channel looks like CHAN1

    checkErr (Ivi_GetAttributeViReal64 (vi, channel, AG1000NI_ATTR_VERTICAL_RANGE, 0, &verRange));

    scale = verRange / AG1000NI_NUM_VER_DIVS;

    viCheckErr (Ivi_CompareWithPrecision (8, value, - scale * 12, &cmp));
    if (cmp < 0)
        viCheckErr (IVI_ERROR_INVALID_VALUE);
    viCheckErr (Ivi_CompareWithPrecision (8, value, scale * 12, &cmp));
    if (cmp > 0)
        viCheckErr (IVI_ERROR_INVALID_VALUE);
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrTriggerSensitivity_CheckCallback (ViSession vi,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;  
    ViChar      triggerSource[BUFFER_SIZE];
    ViReal64    verRange, scale;
    ViInt32     cmp;          

    checkErr (Ivi_GetAttributeViString (vi, VI_NULL, AG1000NI_ATTR_TRIGGER_SOURCE, 0, BUFFER_SIZE, triggerSource));
    checkErr (Ivi_GetAttributeViReal64 (vi, triggerSource, AG1000NI_ATTR_VERTICAL_RANGE, 0, &verRange));

    scale = verRange / AG1000NI_NUM_VER_DIVS;

    viCheckErr (Ivi_CompareWithPrecision (8, value, 0.1, &cmp));
    if (cmp < 0)
        viCheckErr (IVI_ERROR_INVALID_VALUE);
    viCheckErr (Ivi_CompareWithPrecision (8, value, scale, &cmp));
    if (cmp > 0)
        viCheckErr (IVI_ERROR_INVALID_VALUE);
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrAlternationTriggerSensitivity_CheckCallback (ViSession vi,
                                                                                  ViConstString channelName,
                                                                                  ViAttr attributeId,
                                                                                  ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     triggerSource;
    ViString    triggerSourceBuf;
    ViChar      channel[BUFFER_SIZE];
    IviRangeTablePtr tablePtr;
    ViReal64    verRange, scale;
    ViInt32     cmp;

    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AG1000NI_ATTR_ALTERNATION_TRIGGER_SOURCE, 0, &triggerSource));  
    checkErr (Ivi_GetAttrRangeTable (vi, VI_NULL, AG1000NI_ATTR_ALTERNATION_TRIGGER_SOURCE, &tablePtr));    
    viCheckErr (ag1000ni_GetCmdFromIntValue (triggerSource, tablePtr, &triggerSourceBuf));

    if (stricmp (channelName, "SOURA") == 0) {
        strncpy (channel, triggerSourceBuf, 3);
    } else {
        strncpy (channel, triggerSourceBuf + 3, 3);
    }

    // Now channel looks like CH1, move the number back
    channel[4] = channel[2];
    // Complete the channel name
    strncpy (channel + 2, "AN", 2);
    channel[5] = '\0';
    // Now channel looks like CHAN1

    checkErr (Ivi_GetAttributeViReal64 (vi, channel, AG1000NI_ATTR_VERTICAL_RANGE, 0, &verRange));

    scale = verRange / AG1000NI_NUM_VER_DIVS;

    viCheckErr (Ivi_CompareWithPrecision (8, value, 0.1, &cmp));
    if (cmp < 0)
        viCheckErr (IVI_ERROR_INVALID_VALUE);
    viCheckErr (Ivi_CompareWithPrecision (8, value, scale, &cmp));
    if (cmp > 0)
        viCheckErr (IVI_ERROR_INVALID_VALUE);
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrTriggerSource_CommonReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          const ViConstString cacheValue){
    ViStatus    error = VI_SUCCESS;
    ViString    command;
    ViString    repCommand; 
    ViChar*     pdata = VI_NULL;
    ViChar*     pChannelSource = VI_NULL;//for the workaround of trigger sources 
    ViChar*     pend  = VI_NULL;

    viCheckErr( ag1000ni_GetStringFromTable (ag1000niViString_commands, attributeId, &command)); 
    viCheckErr (ag1000ni_GetRepCommandFromTable (ag1000niViString_commands, attributeId, &repCommand));

    if (!Ivi_Simulating (vi))
    {
        ViChar      buffer[BUFFER_SIZE];
        ViChar*     pbuffer = VI_NULL;      
        ViUInt32    allocated_count = 0;
        ViUInt32    count = 0;
        ViUInt32    readed_count;

        viCheckErr( viPrintf (io, "%s%s%s?", command, VALID_CHANNELNAME (channelName), repCommand));

        while ((error = viRead (io, buffer, BUFFER_SIZE, &readed_count)) == VI_SUCCESS_MAX_CNT)
        {
            if (allocated_count)
            {
                if (allocated_count <= count + readed_count)
                {
                    allocated_count += (allocated_count - count) + BUFFER_SIZE; 
                    pdata = realloc (pdata, allocated_count);
                }

                pend = pdata + count;
            }
            else
            {
                allocated_count = 2 * BUFFER_SIZE;
                pdata = malloc(allocated_count);
                pend  = pdata;
            }

            memcpy(pend, buffer, readed_count);
            count += readed_count;
        }

        if (readed_count > 0)
        {
            /*
             * Check if the last character is alnum.
             * if this character is not alnum we know that it's '\n'.
             * Remove it.
             */
            if (!isalnum(buffer[readed_count-1]))
                readed_count--;
            buffer[readed_count] = '\0';
        }

        /*
         * If function is read more data as BUFFER_SIZE, we must
         * copy last data from static buffer to dynamic buffer
         */
        if (pdata)
        {
            /*
             * For sure, check if the  buffer has enought element.
             * This condition should be FALSE every time.
             */
            if (allocated_count <= count + readed_count)
            {
                allocated_count += (allocated_count - count) + BUFFER_SIZE; 
                pdata = realloc (pdata, allocated_count);
            }
            pend = pdata + count;
            memcpy(pend, buffer, readed_count+1);           
            pbuffer = pdata;
        }
        else
            pbuffer = buffer;

        viCheckErr( error);

        if (strnicmp (pbuffer, "CH", strlen ("CH")) == 0) {
            ViInt32 newBufferLen = (ViInt32)strlen (pbuffer) + 3;
            checkErr (Ivi_Alloc (vi, newBufferLen, (ViAddr)&pChannelSource));   
            strncpy (pChannelSource, "CHAN", newBufferLen);
            strncpy (pChannelSource + 4, pbuffer + 2, newBufferLen - 4);
            pChannelSource[newBufferLen - 1] = '\0';

            pbuffer = pChannelSource;
        }

        checkErr( Ivi_SetValInStringCallback(vi, attributeId, pbuffer));
    }

Error:
    if (pdata)
        free( pdata);
    if (pChannelSource)
        Ivi_Free (vi, pChannelSource);
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrHorzRecordLength_ReadCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViInt32 *value)
{
    ViStatus    error = VI_SUCCESS;

    checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AG1000NI_ATTR_HORZ_MIN_NUM_PTS, 0, value));
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrTvTriggerEvent_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 *value)
{
    ViStatus    error = VI_SUCCESS;

    checkErr (ag1000niEnum_ReadCallbackWithSpecifiedRangeTable (vi, io, channelName, attributeId, &attrTVTriggerEventReadRangeTable, value));
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrPulseTriggerMode_ReadCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViInt32 *value)
{
    ViStatus    error = VI_SUCCESS;

    checkErr (ag1000niEnum_ReadCallbackWithSpecifiedRangeTable (vi, io, channelName, attributeId, &attrPulseTriggerModeReadRangeTable, value));
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrAlternationPulseTriggerMode_ReadCallback (ViSession vi,
                                                                               ViSession io,
                                                                               ViConstString channelName,
                                                                               ViAttr attributeId,
                                                                               ViInt32 *value)
{
    ViStatus    error = VI_SUCCESS;

    checkErr (ag1000niEnum_ReadCallbackWithSuffixChannelWithSpecifiedRangeTable (vi, io, channelName, attributeId, &attrAlternationPulseTriggerModeReadRangeTable, value));  
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrAlternationTvTriggerEvent_ReadCallback (ViSession vi,
                                                                             ViSession io,
                                                                             ViConstString channelName,
                                                                             ViAttr attributeId,
                                                                             ViInt32 *value)
{
    ViStatus    error = VI_SUCCESS;   

    checkErr (ag1000niEnum_ReadCallbackWithSuffixChannelWithSpecifiedRangeTable (vi, io, channelName, attributeId, &attrAlternationTVTriggerEventReadRangeTable, value));  
Error:
    return error;
}

static ViStatus _VI_FUNC ag1000niAttrAlternationTvTriggerLineNumber_CheckCallback (ViSession vi,
                                                                                   ViConstString channelName,
                                                                                   ViAttr attributeId,
                                                                                   ViInt32 value)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     tvTriggerType;
    ViInt32     signalFormat;
    ViInt32     minValue = 1, maxValue;

    checkErr (Ivi_DefaultCheckCallbackViInt32 (vi, channelName, attributeId, value));

    checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_EVENT, 0, &tvTriggerType));
    if (AG1000NI_VAL_TV_EVENT_LINE_NUMBER != tvTriggerType)
        viCheckErr (IVI_ERROR_INVALID_CONFIGURATION);

    checkErr (Ivi_GetAttributeViInt32 (vi, channelName, AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_SIGNAL_FORMAT, 0, &signalFormat));

    if (AG1000NI_VAL_NTSC == signalFormat) 
        maxValue = 525;
    else
        maxValue = 625;

    if (value < minValue || value > maxValue)
        viCheckErr (IVI_ERROR_INVALID_VALUE);

Error:
    return error;
}


/*****************************************************************************
 * Function: ag1000ni_InitAttributes                                                  
 * Purpose:  This function adds attributes to the IVI session, initializes   
 *           instrument attributes, and sets attribute invalidation          
 *           dependencies.                                                   
 *****************************************************************************/
static ViStatus ag1000ni_InitAttributes (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     flags = 0;

    checkErr( Ivi_SetAttrReadCallbackViString (vi, AG1000NI_ATTR_SPECIFIC_DRIVER_REVISION,
                                               ag1000niAttrDriverRevision_ReadCallback));
    checkErr( Ivi_SetAttributeViInt32 (vi, "", AG1000NI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION,
                                       0, AG1000NI_CLASS_SPEC_MAJOR_VERSION));
    checkErr( Ivi_SetAttributeViInt32 (vi, "", AG1000NI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION,
                                       0, AG1000NI_CLASS_SPEC_MINOR_VERSION));
    checkErr( Ivi_SetAttributeViString (vi, "", AG1000NI_ATTR_SUPPORTED_INSTRUMENT_MODELS,
                                        0, AG1000NI_SUPPORTED_INSTRUMENT_MODELS));

    checkErr( Ivi_GetAttributeFlags (vi, AG1000NI_ATTR_INSTRUMENT_FIRMWARE_REVISION, &flags));
    checkErr( Ivi_SetAttributeFlags (vi, AG1000NI_ATTR_INSTRUMENT_FIRMWARE_REVISION, 
                                     flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
    checkErr( Ivi_SetAttrReadCallbackViString (vi, AG1000NI_ATTR_INSTRUMENT_FIRMWARE_REVISION,
                                               ag1000niAttrFirmwareRevision_ReadCallback));

    checkErr( Ivi_GetAttributeFlags (vi, AG1000NI_ATTR_INSTRUMENT_MANUFACTURER, &flags));
    checkErr( Ivi_SetAttributeFlags (vi, AG1000NI_ATTR_INSTRUMENT_MANUFACTURER, 
                                     flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
    checkErr( Ivi_SetAttrReadCallbackViString (vi, AG1000NI_ATTR_INSTRUMENT_MANUFACTURER,
                                               ag1000niAttrInstrumentManufacturer_ReadCallback));

    checkErr( Ivi_GetAttributeFlags (vi, AG1000NI_ATTR_INSTRUMENT_MODEL, &flags));
    checkErr( Ivi_SetAttributeFlags (vi, AG1000NI_ATTR_INSTRUMENT_MODEL, 
                                     flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
    checkErr( Ivi_SetAttrReadCallbackViString (vi, AG1000NI_ATTR_INSTRUMENT_MODEL,
                                               ag1000niAttrInstrumentModel_ReadCallback));

    checkErr( Ivi_SetAttributeViString (vi, "", AG1000NI_ATTR_SPECIFIC_DRIVER_VENDOR,
                                        0, AG1000NI_DRIVER_VENDOR));
    checkErr( Ivi_SetAttributeViString (vi, "", AG1000NI_ATTR_SPECIFIC_DRIVER_DESCRIPTION,
                                        0, AG1000NI_DRIVER_DESCRIPTION));
    checkErr( Ivi_SetAttributeViAddr (vi, VI_NULL, IVI_ATTR_OPC_CALLBACK, 0,
                                      ag1000ni_WaitForOPCCallback));
    checkErr( Ivi_SetAttributeViAddr (vi, VI_NULL, IVI_ATTR_CHECK_STATUS_CALLBACK, 0,
                                      ag1000ni_CheckStatusCallback));     

    checkErr( Ivi_SetAttributeViString (vi, "", AG1000NI_ATTR_GROUP_CAPABILITIES, 0,
                                        "IviScopeBase,"
                                        "IviScopeTVTrigger,"
                                        "IviScopeWaveformMeas,"
                                        "IviScopeMinMaxWaveform,"
                                        "IviScopeContinuousAcquisition,"
                                        "IviScopeAverageAcquisition,"
                                        "IviScopeSampleMode,"
                                        "IviScopeTriggerModifier,"
                                        "IviScopeAutoSetup"));

    checkErr (Ivi_AddAttributeViReal64 (vi, AG1000NI_ATTR_ACQUISITION_START_TIME,
                                        "AG1000NI_ATTR_ACQUISITION_START_TIME", 0.0,
                                        IVI_VAL_NEVER_CACHE,
                                        ag1000niViReal64_ReadCallback,
                                        ag1000niViReal64_WriteCallback, VI_NULL, 8));
    checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi,
                                                AG1000NI_ATTR_ACQUISITION_START_TIME,
                                                ag1000niAttrAcquisitionStartTime_CheckCallback));
    checkErr (Ivi_AddAttributeViInt32 (vi, AG1000NI_ATTR_ACQUISITION_TYPE,
                                       "AG1000NI_ATTR_ACQUISITION_TYPE",
                                       AG1000NI_VAL_NORMAL, 0,
                                       ag1000niEnum_ReadCallback,
                                       ag1000niEnum_WriteCallback,
                                       &attrAcquisitionTypeRangeTable));
    checkErr (Ivi_AddAttributeViInt32 (vi, AG1000NI_ATTR_HORZ_MIN_NUM_PTS,
                                       "AG1000NI_ATTR_HORZ_MIN_NUM_PTS", 10240, 0,
                                       ag1000niViInt32_ReadCallback,
                                       ag1000niViInt32_WriteCallback,
                                       &attrHorzMinNumPtsRangeTable));
    checkErr (Ivi_AddAttributeViInt32 (vi, AG1000NI_ATTR_HORZ_RECORD_LENGTH,
                                       "AG1000NI_ATTR_HORZ_RECORD_LENGTH", 1024,
                                       IVI_VAL_NOT_USER_WRITABLE | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION,
                                       ag1000niAttrHorzRecordLength_ReadCallback,
                                       VI_NULL, VI_NULL));
    checkErr (Ivi_AddAttributeViReal64 (vi, AG1000NI_ATTR_HORZ_SAMPLE_RATE,
                                        "AG1000NI_ATTR_HORZ_SAMPLE_RATE", 1.0e6,
                                        IVI_VAL_NOT_USER_WRITABLE,
                                        ag1000niViReal64_ReadCallback, VI_NULL,
                                        VI_NULL, 8));
    checkErr (Ivi_AddAttributeViReal64 (vi, AG1000NI_ATTR_HORZ_TIME_PER_RECORD,
                                        "AG1000NI_ATTR_HORZ_TIME_PER_RECORD", 1.0,
                                        IVI_VAL_NEVER_CACHE,
                                        ag1000niAttrHorzTimePerRecord_ReadCallback,
                                        ag1000niAttrHorzTimePerRecord_WriteCallback,
                                        VI_NULL, 8));
    checkErr (Ivi_SetAttrRangeTableCallback (vi, AG1000NI_ATTR_HORZ_TIME_PER_RECORD,
                                             ag1000niAttrHorzTimePerRecord_RangeTableCallback));
    checkErr (Ivi_AddAttributeViBoolean (vi, AG1000NI_ATTR_INITIATE_CONTINUOUS,
                                         "AG1000NI_ATTR_INITIATE_CONTINUOUS",
                                         VI_TRUE, 0, VI_NULL,
                                         ag1000niAttrInitiateContinuous_WriteCallback));
    checkErr (Ivi_AddAttributeViInt32 (vi, AG1000NI_ATTR_NUM_AVERAGES,
                                       "AG1000NI_ATTR_NUM_AVERAGES", 2, 0,
                                       ag1000niViInt32_ReadCallback,
                                       ag1000niViInt32_WriteCallback,
                                       &attrNumAveragesRangeTable));
    checkErr (Ivi_AddAttributeViInt32 (vi, AG1000NI_ATTR_SAMPLE_MODE,
                                       "AG1000NI_ATTR_SAMPLE_MODE",
                                       AG1000NI_VAL_REAL_TIME,
                                       IVI_VAL_NOT_USER_WRITABLE,
                                       ag1000niEnum_ReadCallback, VI_NULL,
                                       &attrSampleModeRangeTable));
    checkErr (Ivi_AddAttributeViBoolean (vi, AG1000NI_ATTR_CHANNEL_ENABLED,
                                         "AG1000NI_ATTR_CHANNEL_ENABLED", VI_FALSE,
                                         IVI_VAL_MULTI_CHANNEL,
                                         ag1000niViBoolean_ReadCallback,
                                         ag1000niViBoolean_WriteCallback));
    checkErr (Ivi_AddAttributeViReal64 (vi, AG1000NI_ATTR_VERTICAL_RANGE,
                                        "AG1000NI_ATTR_VERTICAL_RANGE", 10.0,
                                        IVI_VAL_MULTI_CHANNEL,
                                        ag1000niAttrVerticalRange_ReadCallback,
                                        ag1000niAttrVerticalRange_WriteCallback,
                                        VI_NULL, 8));
    checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, AG1000NI_ATTR_VERTICAL_RANGE,
                                                ag1000niAttrVerticalRange_CheckCallback));
    checkErr (Ivi_AddAttributeViReal64 (vi, AG1000NI_ATTR_VERTICAL_OFFSET,
                                        "AG1000NI_ATTR_VERTICAL_OFFSET", 0.0,
                                        IVI_VAL_MULTI_CHANNEL,
                                        ag1000niViReal64_ReadCallback,
                                        ag1000niViReal64_WriteCallback, VI_NULL, 8));
    checkErr (Ivi_AddAttributeViInt32 (vi, AG1000NI_ATTR_VERTICAL_COUPLING,
                                       "AG1000NI_ATTR_VERTICAL_COUPLING",
                                       AG1000NI_VAL_DC, IVI_VAL_MULTI_CHANNEL,
                                       ag1000niEnum_ReadCallback,
                                       ag1000niEnum_WriteCallback,
                                       &attrVerticalCouplingRangeTable));
    checkErr (Ivi_AddAttributeViReal64 (vi, AG1000NI_ATTR_INPUT_IMPEDANCE,
                                        "AG1000NI_ATTR_INPUT_IMPEDANCE", 1.0e6,
                                        IVI_VAL_MULTI_CHANNEL, VI_NULL, VI_NULL,
                                        &attrInputImpedanceRangeTable, 8));
    checkErr (Ivi_AddAttributeViReal64 (vi, AG1000NI_ATTR_MAX_INPUT_FREQUENCY,
                                        "AG1000NI_ATTR_MAX_INPUT_FREQUENCY", 25.0e6,
                                        IVI_VAL_MULTI_CHANNEL,
                                        ag1000niAttrMaxInputFrequency_ReadCallback,
                                        ag1000niAttrMaxInputFrequency_WriteCallback,
                                        VI_NULL, 8));
    checkErr (Ivi_AddAttributeViReal64 (vi, AG1000NI_ATTR_PROBE_ATTENUATION,
                                        "AG1000NI_ATTR_PROBE_ATTENUATION", 1.0,
                                        IVI_VAL_MULTI_CHANNEL,
                                        ag1000niAttrProbeAttenuation_ReadCallback,
                                        ag1000niAttrProbeAttenuation_WriteCallback,
                                        &attrProbeAttenuationRangeTable, 8));
    checkErr (Ivi_AddAttributeViBoolean (vi, AG1000NI_ATTR_INPUT_INVERSION_ENABLED,
                                         "AG1000NI_ATTR_INPUT_INVERSION_ENABLED",
                                         VI_FALSE, IVI_VAL_MULTI_CHANNEL,
                                         ag1000niViBoolean_ReadCallback,
                                         ag1000niViBoolean_WriteCallback));
    checkErr (Ivi_AddAttributeViBoolean (vi, AG1000NI_ATTR_DIGITAL_FILTER_ENABLED,
                                         "AG1000NI_ATTR_DIGITAL_FILTER_ENABLED",
                                         VI_FALSE, IVI_VAL_MULTI_CHANNEL,
                                         ag1000niViBoolean_ReadCallback,
                                         ag1000niViBoolean_WriteCallback));
    checkErr (Ivi_AddAttributeViInt32 (vi, AG1000NI_ATTR_TRIGGER_TYPE,
                                       "AG1000NI_ATTR_TRIGGER_TYPE",
                                       AG1000NI_VAL_EDGE_TRIGGER, 0,
                                       ag1000niEnum_ReadCallback,
                                       ag1000niEnum_WriteCallback,
                                       &attrTriggerTypeRangeTable));
    checkErr (Ivi_AddAttributeViString (vi, AG1000NI_ATTR_TRIGGER_SOURCE,
                                        "AG1000NI_ATTR_TRIGGER_SOURCE", "CHAN1",
                                        IVI_VAL_NEVER_CACHE | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION,
                                        ag1000niAttrTriggerSource_ReadCallback,
                                        ag1000niAttrTriggerSource_WriteCallback));
    checkErr (Ivi_AddAttributeViInt32 (vi, AG1000NI_ATTR_TRIGGER_COUPLING,
                                       "AG1000NI_ATTR_TRIGGER_COUPLING",
                                       AG1000NI_VAL_DC, 0, ag1000niEnum_ReadCallback,
                                       ag1000niEnum_WriteCallback,
                                       &attrTriggerCouplingRangeTable));
    checkErr (Ivi_AddAttributeViReal64 (vi, AG1000NI_ATTR_TRIGGER_HOLDOFF,
                                        "AG1000NI_ATTR_TRIGGER_HOLDOFF", 100.0e-9, 0,
                                        ag1000niViReal64_ReadCallback,
                                        ag1000niViReal64_WriteCallback,
                                        &attrTriggerHoldoffRangeTable, 8));
    checkErr (Ivi_AddAttributeViInt32 (vi, AG1000NI_ATTR_TRIGGER_MODIFIER,
                                       "AG1000NI_ATTR_TRIGGER_MODIFIER",
                                       AG1000NI_VAL_NO_TRIGGER_MOD,
                                       IVI_VAL_NEVER_CACHE | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION,
                                       ag1000niAttrTriggerModifier_ReadCallback,
                                       ag1000niAttrTriggerModifier_WriteCallback,
                                       &attrTriggerModifierRangeTable));
    checkErr (Ivi_AddAttributeViReal64 (vi, AG1000NI_ATTR_TRIGGER_SENSITIVITY,
                                        "AG1000NI_ATTR_TRIGGER_SENSITIVITY", 1.0, 0,
                                        ag1000niViReal64_ReadCallback,
                                        ag1000niViReal64_WriteCallback, VI_NULL, 8));
    checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi,
                                                AG1000NI_ATTR_TRIGGER_SENSITIVITY,
                                                ag1000niAttrTriggerSensitivity_CheckCallback));
    checkErr (Ivi_AddAttributeViBoolean (vi, AG1000NI_ATTR_TRIGGER_HF_REJECT_ENABLED,
                                         "AG1000NI_ATTR_TRIGGER_HF_REJECT_ENABLED",
                                         VI_FALSE, 0, ag1000niViBoolean_ReadCallback,
                                         ag1000niViBoolean_WriteCallback));
    checkErr (Ivi_AddAttributeViReal64 (vi, AG1000NI_ATTR_TRIGGER_LEVEL,
                                        "AG1000NI_ATTR_TRIGGER_LEVEL", 0.0,
                                        IVI_VAL_NEVER_CACHE,
                                        ag1000niViReal64_ReadCallback,
                                        ag1000niViReal64_WriteCallback, VI_NULL, 8));
    checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, AG1000NI_ATTR_TRIGGER_LEVEL,
                                                ag1000niAttrTriggerLevel_CheckCallback));
    checkErr (Ivi_AddAttributeViInt32 (vi, AG1000NI_ATTR_TRIGGER_SLOPE,
                                       "AG1000NI_ATTR_TRIGGER_SLOPE",
                                       AG1000NI_VAL_POSITIVE, 0,
                                       ag1000niEnum_ReadCallback,
                                       ag1000niEnum_WriteCallback,
                                       &attrTriggerSlopeRangeTable));
    checkErr (Ivi_AddAttributeViInt32 (vi, AG1000NI_ATTR_TV_TRIGGER_EVENT,
                                       "AG1000NI_ATTR_TV_TRIGGER_EVENT",
                                       AG1000NI_VAL_TV_EVENT_LINE_NUMBER, 0,
                                       ag1000niAttrTvTriggerEvent_ReadCallback,
                                       ag1000niEnum_WriteCallback,
                                       &attrTVTriggerEventRangeTable));
    checkErr (Ivi_AddAttributeViInt32 (vi, AG1000NI_ATTR_TV_TRIGGER_LINE_NUMBER,
                                       "AG1000NI_ATTR_TV_TRIGGER_LINE_NUMBER", 1, 0,
                                       ag1000niViInt32_ReadCallback,
                                       ag1000niViInt32_WriteCallback, VI_NULL));
    checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi,
                                               AG1000NI_ATTR_TV_TRIGGER_LINE_NUMBER,
                                               ag1000niAttrTvTriggerLineNumber_CheckCallback));
    checkErr (Ivi_AddAttributeViInt32 (vi, AG1000NI_ATTR_TV_TRIGGER_POLARITY,
                                       "AG1000NI_ATTR_TV_TRIGGER_POLARITY",
                                       AG1000NI_VAL_TV_POSITIVE, 0,
                                       ag1000niEnum_ReadCallback,
                                       ag1000niEnum_WriteCallback,
                                       &attrTVTriggerPolarityRangeTable));
    checkErr (Ivi_AddAttributeViInt32 (vi, AG1000NI_ATTR_TV_TRIGGER_SIGNAL_FORMAT,
                                       "AG1000NI_ATTR_TV_TRIGGER_SIGNAL_FORMAT",
                                       AG1000NI_VAL_NTSC, 0,
                                       ag1000niEnum_ReadCallback,
                                       ag1000niEnum_WriteCallback,
                                       &attrTVTriggerSignalFormatRangeTable));
    checkErr (Ivi_AddAttributeViReal64 (vi, AG1000NI_ATTR_TV_TRIGGER_LEVEL,
                                        "AG1000NI_ATTR_TV_TRIGGER_LEVEL", 0.0,
                                        IVI_VAL_NEVER_CACHE,
                                        ag1000niViReal64_ReadCallback,
                                        ag1000niViReal64_WriteCallback, VI_NULL, 8));
    checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, AG1000NI_ATTR_TV_TRIGGER_LEVEL,
                                                ag1000niAttrTvTriggerLevel_CheckCallback));
    checkErr (Ivi_AddAttributeViReal64 (vi, AG1000NI_ATTR_PATTERN_TRIGGER_LEVEL,
                                        "AG1000NI_ATTR_PATTERN_TRIGGER_LEVEL", 0.0,
                                        IVI_VAL_NEVER_CACHE,
                                        ag1000niViReal64_ReadCallback,
                                        ag1000niViReal64_WriteCallback, VI_NULL, 8));
    checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi,
                                                AG1000NI_ATTR_PATTERN_TRIGGER_LEVEL,
                                                ag1000niAttrPatternTriggerLevel_CheckCallback));
    checkErr (Ivi_AddAttributeViString (vi, AG1000NI_ATTR_PATTERN_TRIGGER_PATTERN,
                                        "AG1000NI_ATTR_PATTERN_TRIGGER_PATTERN",
                                        "0,0,0", IVI_VAL_NEVER_CACHE,
                                        ag1000niViString_ReadCallback,
                                        ag1000niViString_WriteCallback));
    checkErr (Ivi_SetAttrCheckCallbackViString (vi,
                                                AG1000NI_ATTR_PATTERN_TRIGGER_PATTERN,
                                                ag1000niAttrPatternTriggerPattern_CheckCallback));
    checkErr (Ivi_AddAttributeViInt32 (vi, AG1000NI_ATTR_PULSE_TRIGGER_MODE,
                                       "AG1000NI_ATTR_PULSE_TRIGGER_MODE",
                                       AG1000NI_VAL_PULSE_TRIGGER_GT_POSITIVE, 0,
                                       ag1000niAttrPulseTriggerMode_ReadCallback,
                                       ag1000niEnum_WriteCallback,
                                       &attrPulseTriggerModeRangeTable));
    checkErr (Ivi_AddAttributeViReal64 (vi, AG1000NI_ATTR_PULSE_TRIGGER_LEVEL,
                                        "AG1000NI_ATTR_PULSE_TRIGGER_LEVEL", 0.0,
                                        IVI_VAL_NEVER_CACHE,
                                        ag1000niViReal64_ReadCallback,
                                        ag1000niViReal64_WriteCallback, VI_NULL, 8));
    checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi,
                                                AG1000NI_ATTR_PULSE_TRIGGER_LEVEL,
                                                ag1000niAttrPulseTriggerLevel_CheckCallback));
    checkErr (Ivi_AddAttributeViReal64 (vi, AG1000NI_ATTR_PULSE_TRIGGER_WIDTH,
                                        "AG1000NI_ATTR_PULSE_TRIGGER_WIDTH", 20.0e-9,
                                        0, ag1000niViReal64_ReadCallback,
                                        ag1000niViReal64_WriteCallback,
                                        &attrPulseTriggerWidthRangeTable, 8));
    checkErr (Ivi_AddRepeatedAttributeViInt32 (vi, ALTERNATION_TRIGGER_REP_CAP_NAME,
                                               AG1000NI_ATTR_ALTERNATION_TRIGGER_COUPLING,
                                               "AG1000NI_ATTR_ALTERNATION_TRIGGER_COUPLING",
                                               AG1000NI_VAL_DC, 0,
                                               ag1000niEnum_ReadCallbackWithSuffixChannel,
                                               ag1000niEnum_WriteCallbackWithSuffixChannel,
                                               &attrAlternationTriggerCouplingRangeTable));
    checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, ALTERNATION_TRIGGER_REP_CAP_NAME,
                                                AG1000NI_ATTR_ALTERNATION_TRIGGER_HOLDOFF,
                                                "AG1000NI_ATTR_ALTERNATION_TRIGGER_HOLDOFF",
                                                100.0e-9, 0,
                                                ag1000niViReal64_ReadCallbackWithSuffixChannel,
                                                ag1000niViReal64_WriteCallbackWithSuffixChannel,
                                                &attrAlternationTriggerHoldoffRangeTable,
                                                0));
    checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, ALTERNATION_TRIGGER_REP_CAP_NAME,
                                                AG1000NI_ATTR_ALTERNATION_TRIGGER_SENSITIVITY,
                                                "AG1000NI_ATTR_ALTERNATION_TRIGGER_SENSITIVITY",
                                                1.0, 0,
                                                ag1000niViReal64_ReadCallbackWithSuffixChannel,
                                                ag1000niViReal64_WriteCallbackWithSuffixChannel,
                                                VI_NULL, 0));
    checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi,
                                                AG1000NI_ATTR_ALTERNATION_TRIGGER_SENSITIVITY,
                                                ag1000niAttrAlternationTriggerSensitivity_CheckCallback));
    checkErr (Ivi_AddAttributeViInt32 (vi, AG1000NI_ATTR_ALTERNATION_TRIGGER_SOURCE,
                                       "AG1000NI_ATTR_ALTERNATION_TRIGGER_SOURCE",
                                       AG1000NI_VAL_CH1CH2, 0,
                                       ag1000niEnum_ReadCallback,
                                       ag1000niEnum_WriteCallback,
                                       &attrAlternationTriggerSourceRangeTable));
    checkErr (Ivi_AddRepeatedAttributeViInt32 (vi, ALTERNATION_TRIGGER_REP_CAP_NAME,
                                               AG1000NI_ATTR_ALTERNATION_TRIGGER_TYPE,
                                               "AG1000NI_ATTR_ALTERNATION_TRIGGER_TYPE",
                                               AG1000NI_VAL_ALTERNATION_EDGE_TRIGGER,
                                               0, ag1000niEnum_ReadCallbackWithSuffixChannel,
                                               ag1000niEnum_WriteCallbackWithSuffixChannel,
                                               &attrAlternationTriggerTypeRangeTable));
    checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, ALTERNATION_TRIGGER_REP_CAP_NAME,
                                                AG1000NI_ATTR_ALTERNATION_EDGE_TRIGGER_LEVEL,
                                                "AG1000NI_ATTR_ALTERNATION_EDGE_TRIGGER_LEVEL",
                                                0.0, IVI_VAL_NEVER_CACHE,
                                                ag1000niViReal64_ReadCallbackWithSuffixChannel,
                                                ag1000niViReal64_WriteCallbackWithSuffixChannel,
                                                VI_NULL, 0));
    checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi,
                                                AG1000NI_ATTR_ALTERNATION_EDGE_TRIGGER_LEVEL,
                                                ag1000niAttrAlternationEdgeTriggerLevel_CheckCallback));
    checkErr (Ivi_AddRepeatedAttributeViInt32 (vi, ALTERNATION_TRIGGER_REP_CAP_NAME,
                                               AG1000NI_ATTR_ALTERNATION_EDGE_TRIGGER_SLOPE,
                                               "AG1000NI_ATTR_ALTERNATION_EDGE_TRIGGER_SLOPE",
                                               AG1000NI_VAL_POSITIVE, 0,
                                               ag1000niEnum_ReadCallbackWithSuffixChannel,
                                               ag1000niEnum_WriteCallbackWithSuffixChannel,
                                               &attrAlternationEdgeTriggerSlopeRangeTable));
    checkErr (Ivi_AddRepeatedAttributeViInt32 (vi, ALTERNATION_TRIGGER_REP_CAP_NAME,
                                               AG1000NI_ATTR_ALTERNATION_PULSE_TRIGGER_MODE,
                                               "AG1000NI_ATTR_ALTERNATION_PULSE_TRIGGER_MODE",
                                               AG1000NI_VAL_PULSE_TRIGGER_GT_POSITIVE,
                                               0,
                                               ag1000niAttrAlternationPulseTriggerMode_ReadCallback,
                                               ag1000niEnum_WriteCallbackWithSuffixChannel,
                                               &attrAlternationPulseTriggerModeRangeTable));
    checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, ALTERNATION_TRIGGER_REP_CAP_NAME,
                                                AG1000NI_ATTR_ALTERNATION_PULSE_TRIGGER_WIDTH,
                                                "AG1000NI_ATTR_ALTERNATION_PULSE_TRIGGER_WIDTH",
                                                20.0e-9, 0,
                                                ag1000niViReal64_ReadCallbackWithSuffixChannel,
                                                ag1000niViReal64_WriteCallbackWithSuffixChannel,
                                                &attrAlternationPulseTriggerWidthRangeTable,
                                                0));
    checkErr (Ivi_AddRepeatedAttributeViInt32 (vi, ALTERNATION_TRIGGER_REP_CAP_NAME,
                                               AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_LINE_NUMBER,
                                               "AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_LINE_NUMBER",
                                               1, 0, ag1000niViInt32_ReadCallbackWithSuffixChannel,
                                               ag1000niViInt32_WriteCallbackWithSuffixChannel,
                                               VI_NULL));
    checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi,
                                               AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_LINE_NUMBER,
                                               ag1000niAttrAlternationTvTriggerLineNumber_CheckCallback));
    checkErr (Ivi_AddRepeatedAttributeViInt32 (vi, ALTERNATION_TRIGGER_REP_CAP_NAME,
                                               AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_EVENT,
                                               "AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_EVENT",
                                               AG1000NI_VAL_TV_EVENT_LINE_NUMBER, 0,
                                               ag1000niAttrAlternationTvTriggerEvent_ReadCallback,
                                               ag1000niEnum_WriteCallbackWithSuffixChannel,
                                               &attrAlternationTVTriggerEventRangeTable));
    checkErr (Ivi_AddRepeatedAttributeViInt32 (vi, ALTERNATION_TRIGGER_REP_CAP_NAME,
                                               AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_POLARITY,
                                               "AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_POLARITY",
                                               AG1000NI_VAL_TV_POSITIVE, 0,
                                               ag1000niEnum_ReadCallbackWithSuffixChannel,
                                               ag1000niEnum_WriteCallbackWithSuffixChannel,
                                               &attrAlternationTVTriggerPolarityRangeTable));
    checkErr (Ivi_AddRepeatedAttributeViInt32 (vi, ALTERNATION_TRIGGER_REP_CAP_NAME,
                                               AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_SIGNAL_FORMAT,
                                               "AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_SIGNAL_FORMAT",
                                               AG1000NI_VAL_NTSC, 0,
                                               ag1000niEnum_ReadCallbackWithSuffixChannel,
                                               ag1000niEnum_WriteCallbackWithSuffixChannel,
                                               &attrAlternationTVTriggerSignalFormatRangeTable));
    checkErr (Ivi_AddAttributeViReal64 (vi, AG1000NI_ATTR_MEAS_HIGH_REF,
                                        "AG1000NI_ATTR_MEAS_HIGH_REF", 90.0, 0,
                                        VI_NULL, VI_NULL, VI_NULL, 8));
    checkErr (Ivi_AddAttributeViReal64 (vi, AG1000NI_ATTR_MEAS_MID_REF,
                                        "AG1000NI_ATTR_MEAS_MID_REF", 50.0, 0,
                                        VI_NULL, VI_NULL, VI_NULL, 8));
    checkErr (Ivi_AddAttributeViReal64 (vi, AG1000NI_ATTR_MEAS_LOW_REF,
                                        "AG1000NI_ATTR_MEAS_LOW_REF", 10.0, 0,
                                        VI_NULL, VI_NULL, VI_NULL, 8));
    checkErr (Ivi_AddAttributeViString (vi, AG1000NI_ATTR_INSTR_EDGE_TRIGGER_SOURCE,
                                        "AG1000NI_ATTR_INSTR_EDGE_TRIGGER_SOURCE",
                                        "CHAN1", IVI_VAL_HIDDEN,
                                        ag1000niAttrTriggerSource_CommonReadCallback,
                                        ag1000niViString_WriteCallback));
    checkErr (Ivi_SetAttrCheckCallbackViString (vi,
                                                AG1000NI_ATTR_INSTR_EDGE_TRIGGER_SOURCE,
                                                ag1000niAttrInstrEdgeTriggerSource_CheckCallback));
    checkErr (Ivi_AddAttributeViInt32 (vi, AG1000NI_ATTR_INSTR_EDGE_TRIGGER_MODIFIER,
                                       "AG1000NI_ATTR_INSTR_EDGE_TRIGGER_MODIFIER",
                                       AG1000NI_VAL_NO_TRIGGER_MOD, IVI_VAL_HIDDEN,
                                       ag1000niEnum_ReadCallback,
                                       ag1000niEnum_WriteCallback,
                                       &attrTriggerModifierRangeTable));
    checkErr (Ivi_AddAttributeViString (vi, AG1000NI_ATTR_INSTR_TV_TRIGGER_SOURCE,
                                        "AG1000NI_ATTR_INSTR_TV_TRIGGER_SOURCE",
                                        "CHAN1", IVI_VAL_HIDDEN,
                                        ag1000niAttrTriggerSource_CommonReadCallback,
                                        ag1000niViString_WriteCallback));
    checkErr (Ivi_SetAttrCheckCallbackViString (vi,
                                                AG1000NI_ATTR_INSTR_TV_TRIGGER_SOURCE,
                                                ag1000niAttrCommonTriggerSource_CheckCallback));
    checkErr (Ivi_AddAttributeViInt32 (vi, AG1000NI_ATTR_INSTR_TV_TRIGGER_MODIFIER,
                                       "AG1000NI_ATTR_INSTR_TV_TRIGGER_MODIFIER",
                                       AG1000NI_VAL_NO_TRIGGER_MOD, IVI_VAL_HIDDEN,
                                       ag1000niEnum_ReadCallback,
                                       ag1000niEnum_WriteCallback,
                                       &attrTriggerModifierRangeTable));
    checkErr (Ivi_AddAttributeViString (vi,
                                        AG1000NI_ATTR_INSTR_PATTERN_TRIGGER_SOURCE,
                                        "AG1000NI_ATTR_INSTR_PATTERN_TRIGGER_SOURCE",
                                        "CHAN1", IVI_VAL_HIDDEN,
                                        ag1000niAttrTriggerSource_CommonReadCallback,
                                        ag1000niViString_WriteCallback));
    checkErr (Ivi_SetAttrCheckCallbackViString (vi,
                                                AG1000NI_ATTR_INSTR_PATTERN_TRIGGER_SOURCE,
                                                ag1000niAttrCommonTriggerSource_CheckCallback));
    checkErr (Ivi_AddAttributeViInt32 (vi,
                                       AG1000NI_ATTR_INSTR_PATTERN_TRIGGER_MODIFIER,
                                       "AG1000NI_ATTR_INSTR_PATTERN_TRIGGER_MODIFIER",
                                       AG1000NI_VAL_NO_TRIGGER_MOD, IVI_VAL_HIDDEN,
                                       ag1000niEnum_ReadCallback,
                                       ag1000niEnum_WriteCallback,
                                       &attrTriggerModifierRangeTable));
    checkErr (Ivi_AddAttributeViString (vi, AG1000NI_ATTR_INSTR_PULSE_TRIGGER_SOURCE,
                                        "AG1000NI_ATTR_INSTR_PULSE_TRIGGER_SOURCE",
                                        "CHAN1", IVI_VAL_HIDDEN,
                                        ag1000niAttrTriggerSource_CommonReadCallback,
                                        ag1000niViString_WriteCallback));
    checkErr (Ivi_SetAttrCheckCallbackViString (vi,
                                                AG1000NI_ATTR_INSTR_PULSE_TRIGGER_SOURCE,
                                                ag1000niAttrCommonTriggerSource_CheckCallback));
    checkErr (Ivi_AddAttributeViInt32 (vi,
                                       AG1000NI_ATTR_INSTR_PULSE_TRIGGER_MODIFIER,
                                       "AG1000NI_ATTR_INSTR_PULSE_TRIGGER_MODIFIER",
                                       AG1000NI_VAL_NO_TRIGGER_MOD, IVI_VAL_HIDDEN,
                                       ag1000niEnum_ReadCallback,
                                       ag1000niEnum_WriteCallback,
                                       &attrTriggerModifierRangeTable));
    checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, ALTERNATION_TRIGGER_REP_CAP_NAME,
                                                AG1000NI_ATTR_ALTERNATION_TRIGGER_ACQUISITION_START_TIME,
                                                "AG1000NI_ATTR_ALTERNATION_TRIGGER_ACQUISITION_START_TIME",
                                                0.0, 0,
                                                ag1000niViReal64_ReadCallbackWithSuffixChannel,
                                                ag1000niViReal64_WriteCallbackWithSuffixChannel,
                                                VI_NULL, 0));
    checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi,
                                                AG1000NI_ATTR_ALTERNATION_TRIGGER_ACQUISITION_START_TIME,
                                                ag1000niAttrAcquisitionStartTime_CheckCallback));
    checkErr (Ivi_AddRepeatedAttributeViReal64 (vi, ALTERNATION_TRIGGER_REP_CAP_NAME,
                                                AG1000NI_ATTR_ALTERNATION_TRIGGER_HORZ_TIME_PER_RECORD,
                                                "AG1000NI_ATTR_ALTERNATION_TRIGGER_HORZ_TIME_PER_RECORD",
                                                1.0, IVI_VAL_NEVER_CACHE,
                                                ag1000niAttrAlternationTriggerHorzTimePerRecord_ReadCallback,
                                                ag1000niAttrAlternationTriggerHorzTimePerRecord_WriteCallback,
                                                VI_NULL, 0));
    checkErr (Ivi_SetAttrRangeTableCallback (vi,
                                             AG1000NI_ATTR_ALTERNATION_TRIGGER_HORZ_TIME_PER_RECORD,
                                             ag1000niAttrHorzTimePerRecord_RangeTableCallback));

        /*- End of Instrument Specific Attributes -------------------------------*/

    checkErr (Ivi_AddAttributeViString (vi, AG1000NI_ATTR_ID_QUERY_RESPONSE,
                                        "AG1000NI_ATTR_ID_QUERY_RESPONSE",
                                        "Agilent Technologies",
                                        IVI_VAL_NOT_USER_WRITABLE,
                                        ag1000niAttrIdQueryResponse_ReadCallback,
                                        VI_NULL));

    checkErr( Ivi_AddAttributeViInt32 (vi, AG1000NI_ATTR_OPC_TIMEOUT,
                                       "AG1000NI_ATTR_OPC_TIMEOUT",
                                       5000, IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL)); 
    checkErr( Ivi_AddAttributeViInt32 (vi,
                                       AG1000NI_ATTR_VISA_RM_SESSION,
                                       "AG1000NI_ATTR_VISA_RM_SESSION",
                                       VI_NULL,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL));
    checkErr( Ivi_AddAttributeViInt32 (vi, AG1000NI_ATTR_OPC_CALLBACK,
                                       "AG1000NI_ATTR_OPC_CALLBACK",
                                       VI_NULL,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL));
    checkErr( Ivi_AddAttributeViInt32 (vi,
                                       AG1000NI_ATTR_CHECK_STATUS_CALLBACK,
                                       "AG1000NI_ATTR_CHECK_STATUS_CALLBACK",
                                       VI_NULL,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL));
    checkErr( Ivi_AddAttributeViInt32 (vi,
                                       AG1000NI_ATTR_USER_INTERCHANGE_CHECK_CALLBACK,
                                       "AG1000NI_ATTR_USER_INTERCHANGE_CHECK_CALLBACK",
                                       VI_NULL,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL));
    checkErr (Ivi_AddAttributeViInt32 (vi, AG1000NI_ATTR_INSTR_MODEL_TYPE,
                                       "AG1000NI_ATTR_INSTR_MODEL_TYPE",
                                       AG1000NI_VAL_DSO1002A, IVI_VAL_HIDDEN,
                                       VI_NULL, VI_NULL, &attrInstrModelRangeTable));  
    checkErr (Ivi_AddAttributeInvalidation (vi, AG1000NI_ATTR_ACQUISITION_TYPE,
                                            AG1000NI_ATTR_HORZ_RECORD_LENGTH,
                                            VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AG1000NI_ATTR_PROBE_ATTENUATION,
                                            AG1000NI_ATTR_VERTICAL_RANGE, VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AG1000NI_ATTR_PROBE_ATTENUATION,
                                            AG1000NI_ATTR_VERTICAL_OFFSET, VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AG1000NI_ATTR_PROBE_ATTENUATION,
                                            AG1000NI_ATTR_TRIGGER_LEVEL, VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AG1000NI_ATTR_PROBE_ATTENUATION,
                                            AG1000NI_ATTR_TV_TRIGGER_LEVEL, VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AG1000NI_ATTR_PROBE_ATTENUATION,
                                            AG1000NI_ATTR_PATTERN_TRIGGER_LEVEL,
                                            VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AG1000NI_ATTR_PROBE_ATTENUATION,
                                            AG1000NI_ATTR_PULSE_TRIGGER_LEVEL,
                                            VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AG1000NI_ATTR_PROBE_ATTENUATION,
                                            AG1000NI_ATTR_ALTERNATION_EDGE_TRIGGER_LEVEL,
                                            VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AG1000NI_ATTR_VERTICAL_RANGE,
                                            AG1000NI_ATTR_VERTICAL_OFFSET, VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AG1000NI_ATTR_VERTICAL_RANGE,
                                            AG1000NI_ATTR_TRIGGER_LEVEL, VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AG1000NI_ATTR_VERTICAL_RANGE,
                                            AG1000NI_ATTR_TV_TRIGGER_LEVEL, VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AG1000NI_ATTR_VERTICAL_RANGE,
                                            AG1000NI_ATTR_PATTERN_TRIGGER_LEVEL,
                                            VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AG1000NI_ATTR_VERTICAL_RANGE,
                                            AG1000NI_ATTR_PULSE_TRIGGER_LEVEL,
                                            VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AG1000NI_ATTR_VERTICAL_RANGE,
                                            AG1000NI_ATTR_ALTERNATION_EDGE_TRIGGER_LEVEL,
                                            VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AG1000NI_ATTR_VERTICAL_OFFSET,
                                            AG1000NI_ATTR_TRIGGER_LEVEL, VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AG1000NI_ATTR_VERTICAL_OFFSET,
                                            AG1000NI_ATTR_TV_TRIGGER_LEVEL, VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AG1000NI_ATTR_VERTICAL_OFFSET,
                                            AG1000NI_ATTR_PATTERN_TRIGGER_LEVEL,
                                            VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AG1000NI_ATTR_VERTICAL_OFFSET,
                                            AG1000NI_ATTR_PULSE_TRIGGER_LEVEL,
                                            VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AG1000NI_ATTR_VERTICAL_OFFSET,
                                            AG1000NI_ATTR_ALTERNATION_EDGE_TRIGGER_LEVEL,
                                            VI_TRUE));
Error:
    return error;
}               
               
               
               
               
