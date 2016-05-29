/*****************************************************************************/
/*  ag875x.h                                                                 */
/*  Driver Version: A.01.01                                                  */
/*  Copyright (C) 02000 - 2001 Agilent Technologies                          */
/*---------------------------------------------------------------------------*/
/*  Instrument Driver for the Agilent 8753 and 8720 series Network Analyzers */
/*****************************************************************************/

#ifndef ag875x_INCLUDE 
#define ag875x_INCLUDE
#include "vpptype.h"

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

/*****************************************************************************/
/*  STANDARD SECTION                                                         */
/*  Constants and function prototypes for Agilent standard functions.        */
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
/* DEVELOPER: Remove what you don't need from this standard function         */
/*              section, with the exception that VPP required functions      */
/*              may not be removed.                                          */
/*            Don't add to this section - add to the instrument specific     */
/*              section below.                                               */
/*            Don't change section - if you need to "change" the prototype   */
/*              of a standard function, delete it from this section and      */
/*              add a new function (named differently) in the instrument     */
/*              specific section.                                            */
/*---------------------------------------------------------------------------*/

        /***************************************************/
        /*  Standard constant error conditions returned    */
        /*  by driver functions.                           */
	    /*                                                 */
        /*  Agilent Common Error numbers start at BFFC0D00 */
        /*  The parameter errors extend the number of      */
        /*  errors over the eight defined in VPP 3.4       */
        /***************************************************/

#define INSTR_REAL32
#define ag875x_INSTR_ERROR_NULL_PTR      (_VI_ERROR+0x3FFC0D02L) /* 0xBFFC0D02 */
#define ag875x_INSTR_ERROR_RESET_FAILED  (_VI_ERROR+0x3FFC0D03L) /* 0xBFFC0D03 */
#define ag875x_INSTR_ERROR_UNEXPECTED    (_VI_ERROR+0x3FFC0D04L) /* 0xBFFC0D04 */
#define ag875x_INSTR_ERROR_INV_SESSION   (_VI_ERROR+0x3FFC0D05L) /* 0xBFFC0D05 */
#define ag875x_INSTR_ERROR_LOOKUP        (_VI_ERROR+0x3FFC0D06L) /* 0xBFFC0D06 */
#define ag875x_INSTR_ERROR_DETECTED      (_VI_ERROR+0x3FFC0D07L) /* 0xBFFC0D07 */

#define ag875x_INSTR_ERROR_PARAMETER9    (_VI_ERROR+0x3FFC0D20L) /* 0xBFFC0D20 */
#define ag875x_INSTR_ERROR_PARAMETER10   (_VI_ERROR+0x3FFC0D21L) /* 0xBFFC0D21 */
#define ag875x_INSTR_ERROR_PARAMETER11   (_VI_ERROR+0x3FFC0D22L) /* 0xBFFC0D22 */
#define ag875x_INSTR_ERROR_PARAMETER12   (_VI_ERROR+0x3FFC0D23L) /* 0xBFFC0D23 */
#define ag875x_INSTR_ERROR_PARAMETER13   (_VI_ERROR+0x3FFC0D24L) /* 0xBFFC0D24 */
#define ag875x_INSTR_ERROR_PARAMETER14   (_VI_ERROR+0x3FFC0D25L) /* 0xBFFC0D25 */
#define ag875x_INSTR_ERROR_PARAMETER15   (_VI_ERROR+0x3FFC0D26L) /* 0xBFFC0D26 */
#define ag875x_INSTR_ERROR_PARAMETER16   (_VI_ERROR+0x3FFC0D27L) /* 0xBFFC0D27 */
#define ag875x_INSTR_ERROR_PARAMETER17   (_VI_ERROR+0x3FFC0D28L) /* 0xBFFC0D28 */
#define ag875x_INSTR_ERROR_PARAMETER18   (_VI_ERROR+0x3FFC0D29L) /* 0xBFFC0D29 */

        /***************************************************/
        /*  Constants used by system status functions      */
        /*    These defines are bit numbers which define   */
        /*    the operation and questionable registers.    */
        /*    They are instrument specific.                */
        /***************************************************/


        /***************************************************/
        /*  Constants used by function ag875x_timeOut      */
        /***************************************************/

#define ag875x_TIMEOUT_MAX         2147483647L
#define ag875x_TIMEOUT_MIN         1L

        /***************************************************/
        /*  Required plug and play functions from VPP-3.1  */
        /***************************************************/

ViStatus _VI_FUNC ag875x_init
					(ViRsrc InstrDesc, 
					ViBoolean id_query, 
					ViBoolean do_reset, 
					ViPSession vi);

ViStatus _VI_FUNC ag875x_close
					(ViSession vi);

ViStatus _VI_FUNC ag875x_reset
					(ViSession vi);

ViStatus _VI_FUNC ag875x_self_test
					(ViSession vi, 
					ViPInt16 test_result, 
					ViChar _VI_FAR test_message[]);

ViStatus _VI_FUNC ag875x_error_query
					(ViSession vi, 
					ViPInt32 inst_error_number, 
					ViChar _VI_FAR inst_error_message[]);

ViStatus _VI_FUNC ag875x_error_message
					(ViSession vi, 
					ViStatus driver_error_number, 
					ViChar _VI_FAR driver_error_message[]);

ViStatus _VI_FUNC ag875x_revision_query
					(ViSession vi, 
					ViChar _VI_FAR driver_rev[], 
					ViChar _VI_FAR instr_rev[]);

        /***************************************************/
        /*  Agilent standard utility functions             */
        /***************************************************/

ViStatus _VI_FUNC ag875x_timeOut
					(ViSession vi, 
					ViInt32 time_out);

ViStatus _VI_FUNC ag875x_timeOut_Q
					(ViSession vi, 
					ViPInt32 q_time_out);

ViStatus _VI_FUNC ag875x_errorQueryDetect
					(ViSession vi, 
					ViBoolean eq_detect);

ViStatus _VI_FUNC ag875x_errorQueryDetect_Q
					(ViSession vi, 
					ViPBoolean q_eq_detect);

ViStatus _VI_FUNC ag875x_dcl
					(ViSession vi);

        /***************************************************/
        /*  Agilent standard status functions              */
        /***************************************************/
 
ViStatus _VI_FUNC ag875x_readStatusByte_Q
					(ViSession vi, 
					ViPInt16 status_byte);

        /***************************************************/
        /*  Agilent standard command passthrough functions */
        /***************************************************/

ViStatus _VI_FUNC ag875x_cmd
					( ViSession vi, 
					ViString cmd_string);

ViStatus _VI_FUNC ag875x_cmdString_Q
					(ViSession vi, 
					ViString sq_cmd_string, 
					ViInt32 sq_size,ViChar _VI_FAR sq_result[]);

/*ViStatus _VI_FUNC ag875x_cmdData_Q
					(ViSession vi, 
					ViString Dq_cmd_string, 
					ViInt32 Dq_size,
					ViChar _VI_FAR Dq_result[]);
*/
	
ViStatus _VI_FUNC ag875x_cmdInt
					( ViSession vi, 
					ViString i_cmd_string, 
					ViInt32 i_val);

ViStatus _VI_FUNC ag875x_cmdInt16_Q
					(ViSession vi, 
					ViString iq_cmd_string,	
					ViPInt16 iq_result);

ViStatus _VI_FUNC ag875x_cmdInt32_Q
					(ViSession vi, 
					ViString lq_cmd_string,	
					ViPInt32 lq_result);

ViStatus _VI_FUNC ag875x_cmdReal
					(ViSession vi, 
					ViString r_cmd_string, 
					ViReal64 r_val);

#ifdef INSTR_REAL32

ViStatus _VI_FUNC ag875x_cmdReal32_Q
					(ViSession vi, 
					ViString rq_cmd_string, 
					ViPReal32 rq_result);
#endif

ViStatus _VI_FUNC ag875x_cmdReal64_Q
					(ViSession vi,
					ViString dq_cmd_string, 
					ViPReal64 dq_result);

/* End of Agilent standard declarations */
/*---------------------------------------------------------------------------*/




/*****************************************************************************/
/*  INSTRUMENT SPECIFIC SECTION                                              */
/*  Constants and function prototypes for instrument specific functions.     */
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
/* DEVELOPER: Add constants and function prototypes here.                    */
/*            As a metter of style, add the constant #define's first,        */
/*              followed by function prototypes.                             */
/*            Remember that function prototypes must be consistent with      */
/*              the driver's function panel prototypes.                      */
/*---------------------------------------------------------------------------*/

        /***************************************************/
        /*  Instrument specific constants                  */
        /***************************************************/

#define ag875x_FILE_OPEN_ERROR     		(_VI_ERROR+0x3FFC0D30L) /* 0xBFFC0D30 */
#define ag875x_FILE_READ_ERROR    		(_VI_ERROR+0x3FFC0D31L) /* 0xBFFC0D31 */
#define ag875x_FILE_WRITE_ERROR    		(_VI_ERROR+0x3FFC0D32L) /* 0xBFFC0D32 */
#define ag875x_REC_HEADER_MISMATCH   	(_VI_ERROR+0x3FFC0D34L) /* 0xBFFC0D34 */
#define ag875x_NOT_SUPPORTED_WITH_872X	(_VI_ERROR+0x3FFC0D36L) /* 0xBFFC0D36 */
#define ag875x_INVALID_ARRAY_NUMBER		(_VI_ERROR+0x3FFC0D37L) /* 0xBFFC0D37 */
#define ag875x_NO_MATCH_ERROR     		(_VI_ERROR+0x3FFC0D38L) /* 0xBFFC0D38 */
#define ag875x_REQUIRES_4_PARM_FW  		(_VI_ERROR+0x3FFC0D39L) /* 0xBFFC0D39 */
#define ag875x_REQUIRES_FW_612  		(_VI_ERROR+0x3FFC0D40L) /* 0xBFFC0D40 */
#define ag875x_REQUIRES_2_PORT_CAL 		(_VI_ERROR+0x3FFC0D41L) /* 0xBFFC0D41 */
#define ag875x_ARRAY_TOO_SMALL  		(_VI_ERROR+0x3FFC0D42L) /* 0xBFFC0D42 */
#define ag875x_LRN_8753_TO_872X       	(_VI_ERROR+0x3FFC0D43L) /* 0xBFFC0D43 */
#define ag875x_LRN_872X_TO_8753       	(_VI_ERROR+0x3FFC0D44L) /* 0xBFFC0D44 */
#define ag875x_LRN_NEWER_THAN_INST    	(_VI_ERROR+0x3FFC0D45L) /* 0xBFFC0D45 */
#define ag875x_LIMITYPE_ERROR       	(_VI_ERROR+0x3FFC0D46L) /* 0xBFFC0D46 */
#define ag875x_INVALID_8753_RANGE      	(_VI_ERROR+0x3FFC0D47L) /* 0xBFFC0D47 */
#define ag875x_READ_MATCH_ERROR      	(_VI_ERROR+0x3FFC0D48L) /* 0xBFFC0D48 */
#define ag875x_NOT_SUPPORTED_WITH_8753	(_VI_ERROR+0x3FFC0D49L) /* 0xBFFC0D49 */
#define ag875x_OPTION_REQUIRED      	(_VI_ERROR+0x3FFC0D50L) /* 0xBFFC0D50 */
#define ag875x_REQUIRES_FW_710      	(_VI_ERROR+0x3FFC0D51L) /* 0xBFFC0D51 */
#define ag875x_CALKIT_NOT_SUPPORTED    	(_VI_ERROR+0x3FFC0D52L) /* 0xBFFC0D52 */
#define ag875x_NO_SUCH_CAL_STANDARD    	(_VI_ERROR+0x3FFC0D53L) /* 0xBFFC0D53 */
#define ag875x_OPTION_INVALIDATES_FUNC 	(_VI_ERROR+0x3FFC0D54L) /* 0xBFFC0D54 */
#define ag875x_REQUIRES_S_PARM_ANALYZER	(_VI_ERROR+0x3FFC0D55L) /* 0xBFFC0D55 */
#define ag875x_REQUIRES_FW_760      	(_VI_ERROR+0x3FFC0D56L) /* 0xBFFC0D56 */
#define ag875x_LIMIT_TABLE_EMPTY		(_VI_ERROR+0x3FFC0D57L) /* 0xBFFC0D57 */
#define ag875x_MISSING_FILE_NAME		(_VI_ERROR+0x3FFC0D58L) /* 0xBFFC0D58 */
#define ag875x_NO_TITLE_FOUND           (_VI_ERROR+0x3FFC0D59L) /* 0xBFFC0D59 */
#define ag875x_POWERMETER_CAL_INACTIVE  (_VI_ERROR+0x3FFC0D60L) /* 0xBFFC0D60 */
#define ag875x_LIMIT_TEST_OFF           (_VI_ERROR+0x3FFC0D61L) /* 0xBFFC0D61 */


        /***************************************************/
        /*  Instrument specific enumerations               */
        /***************************************************/


/******   Used by Function ag875x_analogPort   *****/
#define  ag875x_ANLOG_PORT_OFF  0
#define  ag875x_ANLOG_PORT_ON  1

#define  ag875x_ANALOG_MAX  31
#define  ag875x_ANALOG_MIN  1


/******   Used by Function ag875x_calCorrection   *****/
#define  ag875x_CALIB_OFF  0
#define  ag875x_CALIB_ON  1


/******   Used by Function ag875x_channelSelect   *****/
#define ag875x_CH1	0
#define ag875x_CH2	1	
#define ag875x_CH3	2
#define ag875x_CH4	3


/******   Used by Function ag875x_displaySelect   *****/
// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3

#define  ag875x_DISP_DATA  0
#define  ag875x_DISP_MEMO  1
#define  ag875x_DISP_DATAM 2
#define  ag875x_DISP_DDM   3
#define  ag875x_DISP_DMM   4
#define  ag875x_DISP_DATI  5

#define  ag875x_DISP_LOGM  0
#define  ag875x_DISP_PHAS  1
#define  ag875x_DISP_DELA  2
#define  ag875x_DISP_SMIC  3
#define  ag875x_DISP_POLA  4
#define  ag875x_DISP_LINM  5
#define  ag875x_DISP_REAL  6
#define  ag875x_DISP_IMAG  7
#define  ag875x_DISP_SWR   8


/******   Used by Function ag875x_dualSplit   *****/
#define  ag875x_DUAL_CHAN_OFF  0
#define  ag875x_DUAL_CHAN_ON   1

#define  ag875x_SPLIT_CHAN_OFF 0
#define  ag875x_SPLIT_CHAN_ON  1


/******   Used by Function ag875x_frequency   *****/
#define  ag875x_FREQ_STRT_STOP    "START/STOP"
#define  ag875x_FREQ_CENT_SPAN    "CENTER/SPAN"
#define  ag875x_FREQ_STRT_STOP_Q  "START/STOP?"
#define  ag875x_FREQ_CENT_SPAN_Q  "CENTER/SPAN?"


/******   Used by Function ag875x_loadOrPurgeFromDisk   *****/
#define  ag875x_LP_LOAD_FILE   0
#define  ag875x_LP_PURGE_FILE  1


/******   Used by Function ag875x_measType   *****/
#define  ag875x_IN_AR    0
#define  ag875x_IN_BR    1
#define  ag875x_IN_AB    2
#define  ag875x_IN_MA    3
#define  ag875x_IN_MB    4
#define  ag875x_IN_MR    5
#define  ag875x_S_PAR11  6
#define  ag875x_S_PAR21  7
#define  ag875x_S_PAR12  8
#define  ag875x_S_PAR22  9
#define  ag875x_TRANS   10
#define  ag875x_REFL    11

#define  ag875x_CONV_OFF   0
#define  ag875x_CONV_ZREF  1
#define  ag875x_CONV_ZTRA  2
#define  ag875x_CONV_YREF  3
#define  ag875x_CONV_YTRA  4
#define  ag875x_CONV_IDS   5


/******   Used by Function ag875x_portAttenuation   *****/
#define  ag875x_PORT_NUM1  0
#define  ag875x_PORT_NUM2  1

#define  ag875x_ATTEN_00  0
#define  ag875x_ATTEN_05  1
#define  ag875x_ATTEN_10  2
#define  ag875x_ATTEN_15  3
#define  ag875x_ATTEN_20  4
#define  ag875x_ATTEN_25  5
#define  ag875x_ATTEN_30  6
#define  ag875x_ATTEN_35  7
#define  ag875x_ATTEN_40  8
#define  ag875x_ATTEN_45  9
#define  ag875x_ATTEN_50  10
#define  ag875x_ATTEN_60  11
#define  ag875x_ATTEN_70  12


/******   Used by Function ag875x_readTraceOrCalDataDos   *****/
#define  ag875x_CALCnn_OUTPUT     0
#define  ag875x_ICALnn_OUTPUT     1
#define  ag875x_DATA_OUTPUT       2
#define  ag875x_DATF_OUTPUT       3
#define  ag875x_FORM_OUTPUT       4
#define  ag875x_FORF_OUTPUT       5
#define  ag875x_MEMO_OUTPUT       6
#define  ag875x_MEMF_OUTPUT       7
#define  ag875x_PREn_OUTPUT       8
#define  ag875x_RAWn_OUTPUT       9
#define  ag875x_RAFn_OUTPUT      10

#define  ag875x_ARRAY_01          0
#define  ag875x_ARRAY_02          1
#define  ag875x_ARRAY_03          2
#define  ag875x_ARRAY_04          3
#define  ag875x_ARRAY_05          4
#define  ag875x_ARRAY_06          5
#define  ag875x_ARRAY_07          6
#define  ag875x_ARRAY_08          7
#define  ag875x_ARRAY_09          8
#define  ag875x_ARRAY_10          9 
#define  ag875x_ARRAY_11         10
#define  ag875x_ARRAY_12         11

#define  ag875x_ARRAY_MAX_S_PARM 12
#define  ag875x_ARRAY_MAX_ET      6
#define  ag875x_ARRAY_MIN         1

#define  ag875x_RAW_MAX_S_PARM    4
#define  ag875x_RAW_MAX_ET        1


/******   Used by Function ag875x_recallOrClearReg   *****/
#define  ag875x_RC_RECALL_REG     0
#define  ag875x_RC_CLEAR_REG      1
#define  ag875x_RC_CLEAR_ALL_REG  2

#define  ag875x_REG_NUM_MAX  31
#define  ag875x_REG_NUM_MIN   1


/******   Used by Function ag875x_saveToReg   *****/
// #define  ag875x_REG_NUM_MAX  31
// #define  ag875x_REG_NUM_MIN   1



/******   Used by Function ag875x_powerCoupling   *****/
// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3

#define  ag875x_CHAN_POWER_UNCPLD  "CHAN POWER UNCPLD"
#define  ag875x_CHAN_POWER_CPLD    "CHAN POWER CPLD"
#define  ag875x_CHAN_POWER_CPLD_Q  "CHAN POWER CPLD?"
#define  ag875x_PORT_POWER_UNCPLD  "PORT POWER UNCPLD"
#define  ag875x_PORT_POWER_CPLD    "PORT POWER CPLD"
#define  ag875x_PORT_POWER_CPLD_Q  "PORT POWER CPLD?"


/******   Used by Function ag875x_storeToDisk   *****/
#define  ag875x_SDISK_ASCII   0
#define  ag875x_SDISK_BINARY  1

#define  ag875x_CORR_DATA_OFF  0
#define  ag875x_CORR_DATA_ON   1

#define  ag875x_DATA_ONLY_OFF  0
#define  ag875x_DATA_ONLY_ON   1

#define  ag875x_TRACE_DATA_OFF 0
#define  ag875x_TRACE_DATA_ON  1

#define  ag875x_GRAPH_DATA_OFF  0
#define  ag875x_GRAPH_DATA_ON   1

#define  ag875x_RAW_ARRAY_DATA_OFF  0
#define  ag875x_RAW_ARRAY_DATA_ON   1


/******   Used by Function ag875x_freqListSweepSeg   *****/
#define  ag875x_SEGMENT_NUM_MAX  31
#define  ag875x_SEGMENT_NUM_MIN  1


/******   Used by Function ag875x_freqSweepType   *****/
#define  ag875x_SWP_LINF    0
#define  ag875x_SWP_LOGF    1
#define  ag875x_SWP_CWTIME  2
#define  ag875x_SWP_LISTF   3


/******   Used by Function ag875x_writeTraceOrCalDataDos   *****/
#define  ag875x_CALCnn_INPUT   0
#define  ag875x_DATA_INPUT     1
#define  ag875x_FORM_INPUT     2
#define  ag875x_MEMO_INPUT     3
#define  ag875x_RAWn_INPUT     4

// #define  ag875x_ARRAY_01          0
// #define  ag875x_ARRAY_02          1
// #define  ag875x_ARRAY_03          2
// #define  ag875x_ARRAY_04          3
// #define  ag875x_ARRAY_05          4
// #define  ag875x_ARRAY_06          5
// #define  ag875x_ARRAY_07          6
// #define  ag875x_ARRAY_08          7
// #define  ag875x_ARRAY_09          8
// #define  ag875x_ARRAY_10          9
// #define  ag875x_ARRAY_11         10
// #define  ag875x_ARRAY_12         11

// #define  ag875x_ARRAY_MAX_S_PARM 12
// #define  ag875x_ARRAY_MAX_ET      6
// #define  ag875x_ARRAY_MIN         1

// #define  ag875x_RAW_MAX_S_PARM    4
// #define  ag875x_RAW_MAX_ET        1

#define  ag875x_PNTS_03		0
#define  ag875x_PNTS_11		1
#define  ag875x_PNTS_21		2
#define  ag875x_PNTS_26		3
#define  ag875x_PNTS_51		4
#define  ag875x_PNTS_101	5
#define  ag875x_PNTS_201	6
#define  ag875x_PNTS_401	7
#define  ag875x_PNTS_801	8
#define  ag875x_PNTS_1601	9


/******   Used by Function ag875x_calType_Q   *****/
// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3

#define  ag875x_CAL_NONE            "NONE"
#define  ag875x_CAL_RESPONSE        "RESPONSE"
#define  ag875x_CAL_RESP_ISOL       "RESP & ISOL"
#define  ag875x_CAL_ENH_RESP        "ENHANCED RESPONSE"
#define  ag875x_CAL_REV_ENH_RESP    "REVERSE ENHANCED RESPONSE"
#define  ag875x_CAL_S11_1_PORT      "S11 1-PORT"
#define  ag875x_CAL_S22_1_PORT      "S22 1-PORT"
#define  ag875x_CAL_FULL_2_PORT     "FULL 2-PORT"
#define  ag875x_CAL_1_PATH_2_PORT   "ONE PATH 2-PORT"
#define  ag875x_CAL_TRL_LRM         "TRL/LRM 2-PORT"
#define  ag875x_CHANNEL_NOT_ENABLED "CHANNEL NOT ENABLED"

/* These enums used only internally by ag875x_calType_Q . */
#define  ag875x_NUM_CAL_ARRAYS_NONE            0
#define  ag875x_NUM_CAL_ARRAYS_RESPONSE        1
#define  ag875x_NUM_CAL_ARRAYS_RESP_ISOL       2
#define  ag875x_NUM_CAL_ARRAYS_ENH_RESP        6
#define  ag875x_NUM_CAL_ARRAYS_REV_ENH_RESP    6
#define  ag875x_NUM_CAL_ARRAYS_S11_1_PORT      3
#define  ag875x_NUM_CAL_ARRAYS_S22_1_PORT      3
#define  ag875x_NUM_CAL_ARRAYS_FULL_2_PORT     12
#define  ag875x_NUM_CAL_ARRAYS_1_PATH_2_PORT   12
#define  ag875x_NUM_CAL_ARRAYS_TRL_LRM         12

/******   Used by Function ag875x_readID   *****/
#define  ag875x_MODEL_Q         "MODEL?"
#define  ag875x_SERIAL_Q        "SERIAL?"
#define  ag875x_REVISION_Q      "REVISION?"
#define  ag875x_REV_VALUE_Q     "REV_VALUE?"
#define  ag875x_MAXFREQ_Q	    "MAXFREQ?"
#define  ag875x_MINFREQ_Q       "MINFREQ?"

#define  ag875x_OPT_002_Q		"002?"
#define  ag875x_OPT_004_Q		"004?"
#define  ag875x_OPT_006_Q		"006?"
#define  ag875x_OPT_007_Q		"007?"
#define  ag875x_OPT_010_Q		"010?"
#define  ag875x_OPT_011_Q		"011?"
#define  ag875x_OPT_012_Q		"012?"
#define  ag875x_OPT_014_Q		"014?"
#define  ag875x_OPT_075_Q		"075?"
#define  ag875x_OPT_085_Q		"085?"
#define  ag875x_OPT_089_Q		"089?"
#define  ag875x_OPT_1D5_Q		"1D5?"
#define  ag875x_OPT_400_Q		"400?"


/******   Used by Function ag875x_dualSplit4Parm   *****/
// #define  ag875x_DUAL_CHAN_OFF  0 
// #define  ag875x_DUAL_CHAN_ON   1

#define  ag875x_DISP_1_GRAT       0
#define  ag875x_DISP_2_GRAT       1
#define  ag875x_DISP_4_GRAT       2

#define  ag875x_DISP_2_CHAN3_TOP  0
#define  ag875x_DISP_2_CHAN2_TOP  1

#define  ag875x_DISP_4_CHAN3_UPR  0
#define  ag875x_DISP_4_CHAN2_UPR  1


/******   Used by Function ag875x_readLongLrnString   *****/
#define  ag875x_NO_DATA_TRACE_SAVE  0
#define  ag875x_DATA_TRACE_SAVE     1


/******   Used by Function ag875x_writePowerMeterCal   *****/
// #define ag875x_CH1	0
// #define ag875x_CH2	1	

// #define  ag875x_PNTS_03		0
// #define  ag875x_PNTS_11		1
// #define  ag875x_PNTS_21		2
// #define  ag875x_PNTS_26		3
// #define  ag875x_PNTS_51		4
// #define  ag875x_PNTS_101		5
// #define  ag875x_PNTS_201		6
// #define  ag875x_PNTS_401		7
// #define  ag875x_PNTS_801		8
// #define  ag875x_PNTS_1601	9


/******   Used by Function ag875x_clock   *****/
#define ag875x_CLK_DATE           "DATE"
#define ag875x_CLK_DATE_Q         "DATE?"
#define ag875x_CLK_TIME           "TIME"
#define ag875x_CLK_TIME_Q         "TIME?"
#define ag875x_CLK_SYNC           "SYNC"


/******   Used by Function ag875x_points   *****/
// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3

// #define  ag875x_PNTS_03		0
// #define  ag875x_PNTS_11		1
// #define  ag875x_PNTS_21		2
// #define  ag875x_PNTS_26		3
// #define  ag875x_PNTS_51		4
// #define  ag875x_PNTS_101		5
// #define  ag875x_PNTS_201		6
// #define  ag875x_PNTS_401		7
// #define  ag875x_PNTS_801		8
// #define  ag875x_PNTS_1601	9

#define ag875x_SET           "SET"
#define ag875x_Q             "?"


/******   Used by Function ag875x_powerSweep   *****/
// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3

#define ag875x_ON                  "ON"  
#define ag875x_OFF                 "OFF" 
#define ag875x_PWRSWP_START_STOP   "START/STOP"
#define ag875x_PWRSWP_START_STOP_Q "S/S?"


/******   Used by Function ag875x_markerSet   *****/
// #define ag875x_ON                    "ON"
#define ag875x_MKR_STIM                 "STIMULUS"
#define ag875x_MKR_POINT                "POINT"
#define ag875x_MKR_MIN                  "MINIMUM" 
#define ag875x_MKR_MAX                  "MAXIMUM"
#define ag875x_MKR_SEARCH_LEFT          "SEARCH LEFT"
#define ag875x_MKR_SEARCH_RIGHT         "SEARCH RIGHT"
#define ag875x_MKR_SEARCH_OFF           "SEARCH OFF"
#define ag875x_MKR_WIDTHS               "WIDTHS"
#define ag875x_MKR_WIDTHS_OFF           "WIDTHS OFF"
#define ag875x_MKR_TRACK_ON             "TRACK ON"
#define ag875x_MKR_TRACK_OFF            "TRACK OFF"
#define ag875x_MKR_DELTA_TO_MKR         "DELTA TO MKR"
#define ag875x_MKR_DELTA_TO_FIXED       "DELTA TO FIXED"
#define ag875x_MKR_DELTA_ZERO           "DELTA ZERO"
#define ag875x_MKR_DELTA_OFF            "DELTA OFF"
#define ag875x_MKR_STATS_ON             "STATS ON"
#define ag875x_MKR_STATS_OFF            "STATS OFF"
#define ag875x_MKR_CONTINUOUS           "CONTINUOUS"
#define ag875x_MKR_DISCRETE             "DISCRETE"
#define ag875x_MKR_COUPLED              "COUPLED"
#define ag875x_MKR_UNCOUPLED            "UNCOUPLED"
#define ag875x_MKR_START_TO_MKR         "START TO MKR"
#define ag875x_MKR_STOP_TO_MKR          "STOP TO MKR"
#define ag875x_MKR_CENTER_TO_MKR        "CENTER TO MKR"
#define ag875x_MKR_SPAN_TO_MKR          "SPAN TO MKR"
#define ag875x_MKR_CW_TO_MKR            "CW TO MKR"
#define ag875x_MKR_REF_TO_MKR           "REF TO MKR"
#define ag875x_MKR_DELAY_TO_MKR         "DELAY TO MKR"
#define ag875x_MKR_FIXED_MKR_STIM       "FIXED MKR STIM"
#define ag875x_MKR_FIXED_MKR_VALUE      "FIXED MKR VALUE"
#define ag875x_MKR_FIXED_MKR_AUX_VAL    "FIXED MKR AUX VALUE"
#define ag875x_MKR_ANNOTATE_ALL_MKRS    "ANNOTATE ALL MKRS"
#define ag875x_MKR_ANNOTATE_ONE_MKR     "ANNOTATE ONE MKR"
#define ag875x_MKR_POLAR_LIN_MAG        "POLAR LIN MAG"
#define ag875x_MKR_POLAR_LOG_MAG        "POLAR LOG MAG"
#define ag875x_MKR_POLAR_RI             "POLAR RI"
#define ag875x_MKR_SMITH_LIN_MAG        "SMITH LIN MAG"
#define ag875x_MKR_SMITH_LOG_MAG        "SMITH LOG MAG"
#define ag875x_MKR_SMITH_RI             "SMITH RI"
#define ag875x_MKR_SMITH_IMPEDANCE      "SMITH IMPEDANCE"
#define ag875x_MKR_SMITH_ADMITTANCE     "SMITH ADMITTANCE"
#define ag875x_MKR_ALL_OFF              "ALL OFF"

#define ag875x_MKR_1		0      
#define ag875x_MKR_2		1      
#define ag875x_MKR_3		2      
#define ag875x_MKR_4		3      
#define ag875x_MKR_5		4    
  
#define ag875x_MKR_NUM_MAX	5      
#define ag875x_MKR_NUM_MIN	1      

// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3


/******   Used by Function ag875x_markerQuery   *****/
#define ag875x_MKR_VALUE_STIM_Q         "VALUE STIM?"
#define ag875x_MKR_POINT_Q              "POINT?"
#define ag875x_MKR_SEARCH_MODE_Q        "SEARCH MODE?" 
#define ag875x_MKR_TARGET_Q             "TARGET?"
#define ag875x_MKR_SEARCH_WIDTH_Q       "SEARCH WIDTH?"
#define ag875x_MKR_WIDTHS_RESULTS_Q     "WIDTHS RESULTS?"
#define ag875x_MKR_TRACK_ON_Q           "TRACK ON?"
#define ag875x_MKR_DELTA_MODE_Q         "DELTA MODE?"
#define ag875x_MKR_STATS_ON_Q           "STATS ON?"
#define ag875x_MKR_STATS_RESULTS_Q      "STATS RESULTS?"
#define ag875x_MKR_CONTINUOUS_Q         "CONTINUOUS?"
#define ag875x_MKR_COUPLED_Q            "COUPLED?"
#define ag875x_MKR_FIXED_MKR_STIM_Q     "FIXED MKR STIM?"
#define ag875x_MKR_FIXED_MKR_VALUE_Q    "FIXED MKR VALUE?"
#define ag875x_MKR_FIXED_MKR_AUX_VAL_Q  "FIXED MKR AUX VALUE?"
#define ag875x_MKR_ANNOTATE_ALL_MKRS_Q  "ANNOTATE ALL MKRS?"
#define ag875x_MKR_POLAR_FORMAT_Q       "POLAR FORMAT?"
#define ag875x_MKR_SMITH_FORMAT_Q       "SMITH FORMAT?"
#define ag875x_MKR_ALL_OFF_Q            "ALL OFF?"

#define ag875x_MKR_SEARCH_IS_MAX        1
#define ag875x_MKR_SEARCH_IS_MIN        2
#define ag875x_MKR_SEARCH_IS_TARGET     3

#define ag875x_MKR_DELTA_IS_OFF         0
#define ag875x_MKR_DELTA_IS_MKR_1       1
#define ag875x_MKR_DELTA_IS_MKR_2       2
#define ag875x_MKR_DELTA_IS_MKR_3       3
#define ag875x_MKR_DELTA_IS_MKR_4       4
#define ag875x_MKR_DELTA_IS_MKR_5       5
#define ag875x_MKR_DELTA_IS_FIXED       6

#define ag875x_MKR_ANNOTATE_ONE         0
#define ag875x_MKR_ANNOTATE_ALL         1

#define ag875x_MKR_POLAR_IS_LIN_MAG     1
#define ag875x_MKR_POLAR_IS_LOG_MAG     2
#define ag875x_MKR_POLAR_IS_RI          3

#define ag875x_MKR_SMITH_IS_LIN_MAG     1
#define ag875x_MKR_SMITH_IS_LOG_MAG     2
#define ag875x_MKR_SMITH_IS_RI          3
#define ag875x_MKR_SMITH_IS_IMPEDANCE   4
#define ag875x_MKR_SMITH_IS_ADMITTANCE  5

#define ag875x_MKR_AT_LEAST_ONE_ON      0
#define ag875x_MKR_ALL_ARE_OFF          1

// #define ag875x_MKR_1		0      
// #define ag875x_MKR_2		1      
// #define ag875x_MKR_3		2      
// #define ag875x_MKR_4		3      
// #define ag875x_MKR_5		4     
   
// #define ag875x_MKR_NUM_MAX	5  
// #define ag875x_MKR_NUM_MIN	1
 
// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3


/******   Used by Function ag875x_limitTableSet   *****/
#define ag875x_LIM_TABLE_MAX_SEGS       18
#define ag875x_LIM_TABLE_MIN_SEGS       1

#define ag875x_LIM_TABLE_UPPER_LOWER    "UPPER/LOWER"
#define ag875x_LIM_TABLE_MIDDLE_DELTA   "MIDDLE/DELTA"

#define ag875x_LIM_SLOPED_LINE          0
#define ag875x_LIM_FLAT_LINE            1
#define ag875x_LIM_SINGLE_POINT         2

#define ag875x_LIM_STIM_MKR_STIM        9999
#define ag875x_LIM_MIDDLE_MKR_AMPL      9999

// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3

#define ag875x_APPEND_TO_EXISTING_TABLE  VI_TRUE
#define ag875x_CLEAR_EXISTING_TABLE      VI_FALSE


/******   Used by Function ag875x_limitStatusSet   *****/
#define ag875x_LIM_STATUS_TEST_ON           "TEST ON"
#define ag875x_LIM_STATUS_TEST_OFF          "TEST OFF"
#define ag875x_LIM_STATUS_DISPLAY_ON        "DISPLAY ON"
#define ag875x_LIM_STATUS_DISPLAY_OFF       "DISPLAY OFF"
#define ag875x_LIM_STATUS_STIM_OFFSET       "STIM OFFSET"
#define ag875x_LIM_STATUS_AMPL_OFFSET       "AMPL OFFSET"
#define ag875x_LIM_STATUS_MKR_AMPL_OFFSET   "MKR AMPL OFFSET"

// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3

/******   Used by Function ag875x_limitStatusQuery   *****/
#define ag875x_LIM_STATUS_TEST_Q            "TEST?"
#define ag875x_LIM_STATUS_DISPLAY_Q         "DISPLAY?"
#define ag875x_LIM_STATUS_STIM_OFFSET_Q     "STIM OFFSET?"
#define ag875x_LIM_STATUS_AMPL_OFFSET_Q     "AMPL OFFSET?"
#define ag875x_LIM_STATUS_CHANNEL_Q         "CHANNEL STATUS?"
#define ag875x_LIM_STATUS_SEG_NUM_Q         "SEG STATUS?"
#define ag875x_LIM_STATUS_ALL_SEGS_Q        "ALL SEGS STATUS?"
#define ag875x_LIM_STATUS_FAIL_POINT_Q      "FAIL POINT RESULTS?"
#define ag875x_LIM_STATUS_ALL_POINT_Q       "ALL POINT RESULTS?"
#define ag875x_LIM_STATUS_MKR_Q             "MKR RESULTS?"
#define ag875x_LIM_STATUS_ALL_SEGS_MIN_Q    "ALL SEGS MIN VAL?"
#define ag875x_LIM_STATUS_ALL_SEGS_MAX_Q    "ALL SEGS MAX VAL?"
#define ag875x_LIM_STATUS_SEG_MIN_MAX_Q     "SEG MIN MAX?"
#define ag875x_LIM_STATUS_ALL_MIN_MAX_Q     "ALL MIN MAX?"

// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3

/* #define ag875x_LIM_TABLE_MAX_SEGS       18 */
/* #define ag875x_LIM_TABLE_MIN_SEGS       1  */


/******   Used by Function ag875x_freqListSet   *****/
#define ag875x_FREQ_LIST_MAX_SEGS             30 
#define ag875x_FREQ_LIST_MIN_SEGS             1  

#define ag875x_FREQ_LIST_STEPPED              "STEPPED LIST"
#define ag875x_FREQ_LIST_SWEPT                "SWEPT LIST"
#define ag875x_FREQ_LIST_SWEPT_IF             "SWEPT LIST IF"
#define ag875x_FREQ_LIST_SWEPT_CPLD_PWR       "SWEPT LIST CPLD PWR"
#define ag875x_FREQ_LIST_SWEPT_CPLD_PWR_IF    "SWEPT LIST CPLD PWR IF"
#define ag875x_FREQ_LIST_SWEPT_UNCPLD_PWR     "SWEPT LIST UNCPLD PWR"
#define ag875x_FREQ_LIST_SWEPT_UNCPLD_PWR_IF  "SWEPT LIST UNCPLD PWR IF"

// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3

// #define ag875x_APPEND_TO_EXISTING_TABLE    VI_TRUE
// #define ag875x_CLEAR_EXISTING_TABLE        VI_FALSE

 
/******   Used by Function ag875x_freqListQuery   *****/
// #define ag875x_FREQ_LIST_STEPPED              "STEPPED LIST"
// #define ag875x_FREQ_LIST_SWEPT                "SWEPT LIST"
// #define ag875x_FREQ_LIST_SWEPT_IF             "SWEPT LIST IF"
// #define ag875x_FREQ_LIST_SWEPT_CPLD_PWR       "SWEPT LIST CPLD PWR"
// #define ag875x_FREQ_LIST_SWEPT_CPLD_PWR_IF    "SWEPT LIST CPLD PWR IF"
// #define ag875x_FREQ_LIST_SWEPT_UNCPLD_PWR     "SWEPT LIST UNCPLD PWR"
// #define ag875x_FREQ_LIST_SWEPT_UNCPLD_PWR_IF  "SWEPT LIST UNCPLD PWR IF"

// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3

 
/******   Used by Function ag875x_powerRange   *****/
#define ag875x_PWR_RANGE_0       0
#define ag875x_PWR_RANGE_1       1
#define ag875x_PWR_RANGE_2       2
#define ag875x_PWR_RANGE_3       3
#define ag875x_PWR_RANGE_4       4
#define ag875x_PWR_RANGE_5       5
#define ag875x_PWR_RANGE_6       6
#define ag875x_PWR_RANGE_7       7
#define ag875x_PWR_RANGE_8       8
#define ag875x_PWR_RANGE_9       9
#define ag875x_PWR_RANGE_10     10
#define ag875x_PWR_RANGE_11     11
#define ag875x_PWR_RANGE_AUTO   12

// #define ag875x_SET	"SET"
// #define ag875x_Q		"?"

// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3


/******   Used by Function ag875x_powerSet   *****/
#define ag875x_PWR_LEVEL        "LEVEL"
#define ag875x_PWR_SOURCE_ON    "ON"
#define ag875x_PWR_SOURCE_OFF   "OFF"

// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3

// #define  ag875x_PORT_NUM1  0
// #define  ag875x_PORT_NUM2  1


/******   Used by Function ag875x_powerQuery   *****/
#define ag875x_PWR_LEVEL_Q      "LEVEL?"
#define ag875x_PWR_SOURCE_ON_Q  "ON?"

// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3

// #define  ag875x_PORT_NUM1  0
// #define  ag875x_PORT_NUM2  1


/******   Used by Function ag875x_softkey   *****/
#define ag875x_ERASE_SOFTKEYS     "ERASE SOFTKEYS"

#define ag875x_SOFTKEY_MAX        8
#define ag875x_SOFTKEY_MIN        0


/******   Used by Function ag875x_sweepAltOrChop   *****/
#define ag875x_SWEEP_ALT          "ALT"  
#define ag875x_SWEEP_CHOP         "CHOP" 
// #define ag875x_Q               "?"


/******   Used by Function ag875x_sweepStep   *****/
// #define ag875x_ON              "ON"
// #define ag875x_OFF             "OFF"
// #define ag875x_Q               "?"

// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3


/******   Used by Function ag875x_sweepTime   *****/
// #define ag875x_SET              "SET" 
#define ag875x_SWEEP_TIME_AUTO     "AUTO" 
#define ag875x_SWEEP_TIME_MANUAL   "MANUAL" 
// #define ag875x_Q                "?"

// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3


/******   Used by Function ag875x_averageSmooth   *****/
#define  ag875x_AVG_FACTOR_MAX  999
#define  ag875x_AVG_FACTOR_MIN  0

#define  ag875x_SMOOTH_APERTURE_MAX  20.0
#define  ag875x_SMOOTH_APERTURE_MIN  0

#define ag875x_AVG_FACTOR         "FACTOR"
#define ag875x_SMOOTH_APERTURE    "APERTURE"
#define ag875x_AVG_RESTART        "RESTART"
#define ag875x_AVG_FACTOR_Q       "FACTOR?"
#define ag875x_SMOOTH_APERTURE_Q  "APERTURE?"
#define ag875x_AVG_STATUS_Q       "AVERAGING?"
#define ag875x_SMOOTH_STATUS_Q    "SMOOTHING?"

// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3


/******   Used by Function ag875x_bandwidth   *****/
#define  ag875x_IF_BW_10	0
#define  ag875x_IF_BW_30	1
#define  ag875x_IF_BW_100	2
#define  ag875x_IF_BW_300	3
#define  ag875x_IF_BW_1000	4
#define  ag875x_IF_BW_3000	5
#define  ag875x_IF_BW_3700	6
#define  ag875x_IF_BW_6000	7

// #define ag875x_SET		"SET"
// #define ag875x_Q			"?"

// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3


/******   Used by Function ag875x_title   *****/
#define ag875x_TITLE_SET_PADDED    "SET PADDED" 
#define ag875x_TITLE_SET_NO_PAD    "SET NO PAD" 
// #define ag875x_Q                "?"

// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3


/******   Used by Function ag875x_print   *****/
#define ag875x_PRINT_NOW                "PRINT"
#define ag875x_PRINT_DEST               "DESTINATION"
#define ag875x_PRINT_PORT_Q             "PORT?"
#define ag875x_PRINT_COLOR              "COLOR"
#define ag875x_PRINT_COLOR_Q            "COLOR?"
#define ag875x_PRINT_FEATURE_COLOR      "FEATURE COLOR"
#define ag875x_PRINT_MODE               "MODE"
#define ag875x_PRINT_QUADRANT           "QUADRANT"
#define ag875x_PRINT_QUADRANT_Q         "QUADRANT?"
#define ag875x_PRINT_FORMFEED           "FORMFEED"
#define ag875x_PRINT_FORMFEED_Q         "FORMFEED?"
#define ag875x_PRINT_SOFTKEYS           "SOFTKEYS"
#define ag875x_PRINT_BAUD_RATE          "BAUD RATE"
#define ag875x_PRINT_BAUD_RATE_Q        "BAUD RATE?"
#define ag875x_PRINT_HANDSHAKE          "HANDSHAKE"
#define ag875x_PRINT_HANDSHAKE_Q        "HANDSHAKE?"
#define ag875x_PRINT_DEFAULT            "DEFAULT"
#define ag875x_PRINT_SINGLE_PAGE        "SINGLE PAGE"
#define ag875x_PRINT_MULTI_PAGE         "MULTI PAGE"
#define ag875x_PRINT_DJ_540_OR_850C     "DJ 540 OR 850C"
#define ag875x_PRINT_DESKJET            "DESKJET"
#define ag875x_PRINT_EPSON              "EPSON"
#define ag875x_PRINT_LASERJET           "LASERJET"
#define ag875x_PRINT_PORT_PARALLEL      "PARALLEL"
#define ag875x_PRINT_PORT_SERIAL        "SERIAL"
#define ag875x_PRINT_PORT_GPIB          "GPIB"
#define ag875x_PRINT_FEATURE_CH1_DATA   "CH 1 DATA"
#define ag875x_PRINT_FEATURE_CH2_DATA   "CH 2 DATA"
#define ag875x_PRINT_FEATURE_CH3_DATA   "CH 3 DATA"
#define ag875x_PRINT_FEATURE_CH4_DATA   "CH 4 DATA"
#define ag875x_PRINT_FEATURE_CH1_MEM    "CH 1 MEMORY"
#define ag875x_PRINT_FEATURE_CH2_MEM    "CH 2 MEMORY"
#define ag875x_PRINT_FEATURE_CH3_MEM    "CH 3 MEMORY"
#define ag875x_PRINT_FEATURE_CH4_MEM    "CH 4 MEMORY"
#define ag875x_PRINT_FEATURE_GRATICULE  "GRATICULE"
#define ag875x_PRINT_FEATURE_REF_LINE   "REF LINE"
#define ag875x_PRINT_FEATURE_WARNING    "WARNING"
#define ag875x_PRINT_FEATURE_TEXT       "TEXT"
#define ag875x_PRINT_COLOR_WHITE        "WHITE"
#define ag875x_PRINT_COLOR_CYAN         "CYAN"
#define ag875x_PRINT_COLOR_MAGENTA      "MAGENTA"
#define ag875x_PRINT_COLOR_BLUE         "BLUE"
#define ag875x_PRINT_COLOR_YELLOW       "YELLOW"
#define ag875x_PRINT_COLOR_GREEN        "GREEN"
#define ag875x_PRINT_COLOR_RED          "RED"
#define ag875x_PRINT_COLOR_BLACK        "BLACK"
#define ag875x_PRINT_MODE_LIST          "LIST"
#define ag875x_PRINT_MODE_OP_PARMS      "OP PARMS"
#define ag875x_PRINT_MODE_GRAPH         "GRAPH"
#define ag875x_PRINT_QUAD_LEFT_LOWER    "LEFT LOWER"
#define ag875x_PRINT_QUAD_LEFT_UPPER    "LEFT UPPER"
#define ag875x_PRINT_QUAD_RIGHT_LOWER   "RIGHT LOWER"
#define ag875x_PRINT_QUAD_RIGHT_UPPER   "RIGHT UPPER"
#define ag875x_PRINT_QUAD_FULL_PAGE     "FULL PAGE"
#define ag875x_PRINT_BAUD_RATE_1200     "1200"
#define ag875x_PRINT_BAUD_RATE_2400     "2400"
#define ag875x_PRINT_BAUD_RATE_4800     "4800"
#define ag875x_PRINT_BAUD_RATE_9600     "9600"
#define ag875x_PRINT_BAUD_RATE_19200    "19200"
#define ag875x_PRINT_HANDSHAKE_DTR_DSR  "DTR-DSR"
#define ag875x_PRINT_HANDSHAKE_XON_XOFF "XON-XOFF"

// #define ag875x_ON              "ON"
// #define ag875x_OFF             "OFF"


/******   Used by Function ag875x_calkit   *****/
#define ag875x_CALKIT_2_4_MM       "2.4MM"
#define ag875x_CALKIT_2_92_MM      "2.92MM"
#define ag875x_CALKIT_2_92_MM_STAR "2.92MM STAR"
#define ag875x_CALKIT_3_5_MM_TRL   "3.5MM TRL"
#define ag875x_CALKIT_3_5_MM_C     "3.5MM C"
#define ag875x_CALKIT_3_5_MM_D     "3.5MM D"
#define ag875x_CALKIT_716          "7-16"
#define ag875x_CALKIT_7_MM         "7MM"
#define ag875x_CALKIT_N50          "N50"
#define ag875x_CALKIT_N75          "N75"
#define ag875x_CALKIT_USER_DEFINED "USER DEFINED"
// #define ag875x_Q                "?" 


/******   Used by Function ag875x_measType_Q   *****/
// #define  ag875x_IN_AR    0
// #define  ag875x_IN_BR    1
// #define  ag875x_IN_AB    2
// #define  ag875x_IN_MA    3
// #define  ag875x_IN_MB    4
// #define  ag875x_IN_MR    5
// #define  ag875x_S_PAR11  6
// #define  ag875x_S_PAR21  7
// #define  ag875x_S_PAR12  8
// #define  ag875x_S_PAR22  9
// #define  ag875x_TRANS   10
// #define  ag875x_REFL    11

// #define  ag875x_CONV_OFF   0
// #define  ag875x_CONV_ZREF  1
// #define  ag875x_CONV_ZTRA  2
// #define  ag875x_CONV_YREF  3
// #define  ag875x_CONV_YTRA  4
// #define  ag875x_CONV_IDS   5


/******   Used by Function ag875x_sweepType_Q   *****/
// #define  ag875x_SWP_LINF		0
// #define  ag875x_SWP_LOGF		1
// #define  ag875x_SWP_CWTIME	2
// #define  ag875x_SWP_LISTF	3
#define  ag875x_SWP_POWER		4 


/******   Used by Function ag875x_trigger   *****/
#define ag875x_TRIG_HOLD	"HOLD"
#define ag875x_TRIG_CONT	"CONTINUOUS"
// #define ag875x_Q			"?"

// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3


/******   Used by Function ag875x_display_Q   *****/
// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3

// #define  ag875x_DISP_DATA  0
// #define  ag875x_DISP_MEMO  1
// #define  ag875x_DISP_DATAM 2
// #define  ag875x_DISP_DDM   3
// #define  ag875x_DISP_DMM   4
// #define  ag875x_DISP_DATI  5

// #define  ag875x_DISP_LOGM  0
// #define  ag875x_DISP_PHAS  1
// #define  ag875x_DISP_DELA  2
// #define  ag875x_DISP_SMIC  3
// #define  ag875x_DISP_POLA  4
// #define  ag875x_DISP_LINM  5
// #define  ag875x_DISP_REAL  6
// #define  ag875x_DISP_IMAG  7
// #define  ag875x_DISP_SWR   8


/******   Used by Function ag875x_auxChannel   *****/
// #define ag875x_CH1	0
// #define ag875x_CH2	1	

// #define ag875x_ON              "ON"
// #define ag875x_OFF             "OFF"
// #define ag875x_Q               "?"


/******   Used by Function ag875x_readPowerMeterCal   *****/
// #define ag875x_CH1	0
// #define ag875x_CH2	1	

// #define  ag875x_PNTS_03		0
// #define  ag875x_PNTS_11		1
// #define  ag875x_PNTS_21		2
// #define  ag875x_PNTS_26		3
// #define  ag875x_PNTS_51		4
// #define  ag875x_PNTS_101		5
// #define  ag875x_PNTS_201		6
// #define  ag875x_PNTS_401		7
// #define  ag875x_PNTS_801		8
// #define  ag875x_PNTS_1601	9

#define  ag875x_READ_UNINTERP_CAL  0
#define  ag875x_READ_INTERP_CAL    1


/******   Used by Function ag875x_esr_Q   *****/
#define  ag875x_ESR_EMPTY    			  "ESR_EMPTY" 
#define  ag875x_ESR_OPERATION_COMPLETE    "ESR_OPC"
#define  ag875x_ESR_REQUEST_CONTROL       "ESR_RQL"
#define  ag875x_ESR_QUERY_ERROR           "ESR_QYE"
#define  ag875x_ESR_SEQUENCE_BIT          "ESR_SQB"
#define  ag875x_ESR_EXECUTION_ERROR       "ESR_EXE"
#define  ag875x_ESR_COMMAND_SYNTAX_ERROR  "ESR_CME"
#define  ag875x_ESR_USER_REQUEST          "ESR_URQ"
#define  ag875x_ESR_POWER_ON              "ESR_PON"


/******   Used by Function ag875x_esb_Q   *****/
#define  ag875x_ESB_EMPTY    			  "ESB_EMPTY" 
#define  ag875x_ESB_SING_NUMG_CALSTD      "ESB_SNC"
#define  ag875x_ESB_SERVICE_ROUTINE       "ESB_SRW"
#define  ag875x_ESB_DATA_ENTRY_COMPLETE   "ESB_DEC"
#define  ag875x_ESB_LIMIT_FAILED_CH2      "ESB_LF2"
#define  ag875x_ESB_LIMIT_FAILED_CH1      "ESB_LF1"
#define  ag875x_ESB_SEARCH_FAILED_CH2     "ESB_SF2"
#define  ag875x_ESB_SEARCH_FAILED_CH1     "ESB_SF1"
#define  ag875x_ESB_COPY_COMPLETE         "ESB_CPC"
#define  ag875x_ESB_LIMIT_FAILED_CH4      "ESB_LF4"
#define  ag875x_ESB_LIMIT_FAILED_CH3      "ESB_LF3"
#define  ag875x_ESB_SEARCH_FAILED_CH4     "ESB_SF4"
#define  ag875x_ESB_SEARCH_FAILED_CH3     "ESB_SF3"


/******   Used by Function ag875x_srq_enable   *****/
// #define  ag875x_ESR_OPERATION_COMPLETE    "ESR_OPC"
// #define  ag875x_ESR_REQUEST_CONTROL       "ESR_RQL"
// #define  ag875x_ESR_QUERY_ERROR           "ESR_QYE"
// #define  ag875x_ESR_SEQUENCE_BIT          "ESR_SQB"
// #define  ag875x_ESR_EXECUTION_ERROR       "ESR_EXE"
// #define  ag875x_ESR_COMMAND_SYNTAX_ERROR  "ESR_CME"
// #define  ag875x_ESR_USER_REQUEST          "ESR_URQ"
// #define  ag875x_ESR_POWER_ON              "ESR_PON"
// #define  ag875x_ESB_SING_NUMG_CALSTD      "ESB_SNC"
// #define  ag875x_ESB_SERVICE_ROUTINE       "ESB_SRW"
// #define  ag875x_ESB_DATA_ENTRY_COMPLETE   "ESB_DEC"
// #define  ag875x_ESB_LIMIT_FAILED_CH2      "ESB_LF2"
// #define  ag875x_ESB_LIMIT_FAILED_CH1      "ESB_LF1"
// #define  ag875x_ESB_SEARCH_FAILED_CH2     "ESB_SF2"
// #define  ag875x_ESB_SEARCH_FAILED_CH1     "ESB_SF1"
// #define  ag875x_ESB_COPY_COMPLETE         "ESB_CPC"
// #define  ag875x_ESB_LIMIT_FAILED_CH4      "ESB_LF4"
// #define  ag875x_ESB_LIMIT_FAILED_CH3      "ESB_LF3"
// #define  ag875x_ESB_SEARCH_FAILED_CH4     "ESB_SF4"
// #define  ag875x_ESB_SEARCH_FAILED_CH3     "ESB_SF3"
#define  ag875x_STB_CHECK_ERROR_QUEUE        "STB_CEQ"
#define  ag875x_STB_MESSAGE_IN_OUTPUT_QUEUE  "STB_MOQ"
#define  ag875x_STB_PRESET                   "STB_PRS"


/******   Used by Function ag875x_calInterpolation   *****/
// #define ag875x_SET			"SET"
// #define ag875x_Q				"?"

#define  ag875x_CAL_INTERP_OFF  0
#define  ag875x_CAL_INTERP_ON   1

// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3


/******   Used by Function ag875x_limitTableQuery   *****/
#define ag875x_LIM_TABLE_SIZE_MAX         72 

// #define ag875x_LIM_TABLE_UPPER_LOWER    "UPPER/LOWER"
// #define ag875x_LIM_TABLE_MIDDLE_DELTA   "MIDDLE/DELTA"

// #define ag875x_LIM_SLOPED_LINE          0
// #define ag875x_LIM_FLAT_LINE            1
// #define ag875x_LIM_SINGLE_POINT         2

// #define ag875x_CH1	0
// #define ag875x_CH2	1	
// #define ag875x_CH3	2
// #define ag875x_CH4	3



        /***************************************************/
        /*  Instrument specific functions                  */
        /***************************************************/


ViStatus _VI_FUNC ag875x_analogPort
					(ViSession vi, 
					ViBoolean analogState, 
					ViInt16 analogPortNum);

ViStatus _VI_FUNC ag875x_autoscale
					(ViSession vi);

ViStatus _VI_FUNC ag875x_calCorrection
					(ViSession vi, 
					ViBoolean calibState);

ViStatus _VI_FUNC ag875x_channelSelect
					(ViSession vi, 
					ViInt16 channel);

ViStatus _VI_FUNC ag875x_displaySelect
					(ViSession vi, 
					ViInt16 displaySelectChannel, 
					ViInt16 displaySelectMode, 
					ViInt16 displaySelectFormat);

ViStatus _VI_FUNC ag875x_dualSplit  
					(ViSession vi, 
					ViBoolean dualChannelState, 
					ViBoolean splitChannelMode);

ViStatus _VI_FUNC ag875x_esb_Q  
					(ViSession vi, 
					ViChar _VI_FAR statstr[]);

ViStatus _VI_FUNC ag875x_esr_Q  
					(ViSession vi, 
					ViChar _VI_FAR errstr[]);

ViStatus _VI_FUNC ag875x_frequency  
					(ViSession vi, 
					ViString frequencyRequest, 
					ViReal64 setStartOrCenter, 
					ViReal64 setStopOrSpan, 
					ViPReal64 getStartOrCenter, 
					ViPReal64 getStopOrSpan);

ViStatus _VI_FUNC ag875x_loadOrPurgeFromDisk  
					(ViSession vi, 
					ViInt16 loadOrPurgeMode, 
					ViChar _VI_FAR loadOrPurgeFileTitle[]);

ViStatus _VI_FUNC ag875x_measType  
					(ViSession vi, 
					ViInt16 inputSParam, 
					ViInt16 conversionParam);

ViStatus _VI_FUNC ag875x_opc_Q  
					(ViSession vi, 
					ViString oqCmdString, 
					ViPBoolean instrReady);

ViStatus _VI_FUNC ag875x_portAttenuation  
					(ViSession vi, 
					ViInt16 PortAttenuationPortNum, 
					ViInt16 attenuation);

ViStatus _VI_FUNC ag875x_readTraceOrCalDataDos  
					(ViSession vi, 
					ViInt16 readTraceOrCalType, 
					ViReal64 _VI_FAR readTraceOrCalArray[], 
					ViInt16 readTraceOrCalArrayNum);

ViStatus _VI_FUNC ag875x_recallOrClearReg  
					(ViSession vi,
					ViInt16 recallOrClearRegMode, 
					ViInt16 recallOrClearRegNum);

ViStatus _VI_FUNC ag875x_saveToReg  
					(ViSession vi, 
					ViChar _VI_FAR saveRegTitle[], 
					ViInt16 saveRegNum);

ViStatus _VI_FUNC ag875x_powerCoupling  
					(ViSession vi, 
					ViString powerCouplingRequest, 
					ViInt16 powerCouplingChannel, 
					ViPBoolean powerCouplingState);

ViStatus _VI_FUNC ag875x_storeToDisk  
					(ViSession vi, 
					ViInt16 saveUsingFormat, 
					ViBoolean dataArray, 
					ViBoolean dataOnly, 
					ViBoolean formatArray, 
					ViBoolean graphics, 
					ViBoolean rawArray, 
					ViChar _VI_FAR storeFileTitle[]);

ViStatus _VI_FUNC ag875x_freqListSweepSeg  
					(ViSession vi, 
					ViInt16 freqListSweepSegNum);

ViStatus _VI_FUNC ag875x_freqSweepType  
					(ViSession vi, 
					ViInt16 freqSweepType);

ViStatus _VI_FUNC ag875x_writeTraceOrCalDataDos  
					(ViSession vi, 
					ViInt16 writeTraceOrCalType, 
					ViReal64 _VI_FAR writeTraceOrCalArray[], 
					ViInt16 writeTraceOrCalArrayNum, 
					ViInt16 writeTraceOrCalPoints);

ViStatus _VI_FUNC ag875x_calType_Q  
					(ViSession vi, 
					ViInt16 calTypeQChannel, 
					ViPString currentCalTypeQ);

ViStatus _VI_FUNC ag875x_stringParse  
					(ViSession vi, 
					ViChar _VI_FAR stringToParse[], 
					ViPString parsedString, 
					ViPInt32 parseNotDone, 
					ViString delimiter);

ViStatus _VI_FUNC ag875x_readID  
					(ViSession vi, 
					ViPString currentID, 
					ViString IDRequest, 
					ViPReal64 IDStatus);

ViStatus _VI_FUNC ag875x_dualSplit4Parm  
					(ViSession vi, 
					ViBoolean dualChannel4ParmState, 
					ViInt16 splitDisplay4ParmMode, 
					ViInt16 channelPosition2XDisplay, 
					ViInt16 channelPosition4XDisplay);

ViStatus _VI_FUNC ag875x_readLongLrnString
					(ViSession vi, 
					ViString readLongLrnFile, 
					ViReal64 lrnStringRevisionNum, 
					ViBoolean dataTraceState);

ViStatus _VI_FUNC ag875x_writeLongLrnString
					(ViSession vi, 
					ViString writeLongLrnFile);

ViStatus _VI_FUNC ag875x_writePowerMeterCal
					(ViSession vi, 
					ViInt16 writePowerMeterCalChannel, 
					ViReal64 _VI_FAR writePowerMeterCalArray[], 
					ViInt16 writePowerMeterCalPoints);

ViStatus _VI_FUNC ag875x_clock
					(ViSession vi, 
					ViString clockRequest, 
					ViString clockData, 
					ViPString currentClock);

ViStatus _VI_FUNC ag875x_points
					(ViSession vi, 
					ViInt16 points, 
					ViString pointsRequest, 
					ViInt16 pointsChannel, 
					ViPInt16 currentPoints);

ViStatus _VI_FUNC ag875x_powerSweep
					(ViSession vi, 
					ViString powerSweepRequest, 
					ViReal64 setStartPower, 
					ViReal64 setStopPower, 
					ViInt16 powerSweepChannel, 
					ViPReal64 getStartPower, 
					ViPReal64 getStopPower);

ViStatus _VI_FUNC ag875x_markerSet
					(ViSession vi, 
					ViString markerRequest, 
					ViInt16 setMarker, 
					ViReal64 setMarkerValue, 
					ViInt16 markerChannel);

ViStatus _VI_FUNC ag875x_markerQuery
					(ViSession vi, 
					ViPReal64 markerQueryResult1, 
					ViPReal64 markerQueryResult2, 
					ViPReal64 markerQueryResult3, 
					ViString markerQueryRequest, 
					ViInt16 getMarker, 
					ViInt16 markerQueryChannel, 
					ViPReal64 markerQueryResult4);

ViStatus _VI_FUNC ag875x_limitTableSet
					(ViSession vi, 
					ViInt16 limitTableSetSegments, 
					ViReal64 _VI_FAR limitTableSetLimits[], 
					ViString limitTableSetRequest, 
					ViInt16 limitTableSetChannel, 
					ViBoolean limitTableSetAppendMode);

ViStatus _VI_FUNC ag875x_limitStatusSet
					(ViSession vi, 
					ViString limitStatusSetRequest, 
					ViInt16 limitStatusSetChannel, 
					ViReal64 limitStatusSetOffset);

ViStatus _VI_FUNC ag875x_limitStatusQuery
					(ViSession vi, 
					ViInt16 limitStatusQueryArrSize, 
					ViReal64 _VI_FAR limitStatusQueryResult[], 
					ViPInt16 limitStatusQueryNumValues, 
					ViString limitStatusQueryRequest, 
					ViInt16 limitStatusQueryChannel, 
					ViInt16 limitStatusQuerySegNum);

ViStatus _VI_FUNC ag875x_freqListSet
					(ViSession vi, 
					ViInt16 freqListSetSegments, 
					ViReal64 _VI_FAR freqListSetList[], 
					ViString freqListSetRequest, 
					ViInt16 freqListSetChannel, 
					ViBoolean freqListSetAppendMode);

ViStatus _VI_FUNC ag875x_freqListQuery
					(ViSession vi, 
					ViPInt16 freqListQuerySegments, 
					ViReal64 _VI_FAR freqListQueryList[],
					ViPString freqListQueryListType, 
					ViInt16 freqListQueryChannel);

ViStatus _VI_FUNC ag875x_powerRange
					(ViSession vi, 
					ViInt16 range, 
					ViString powerRangeRequest, 
					ViInt16 powerRangeChannel, 
					ViPInt16 currentPowerRange);

ViStatus _VI_FUNC ag875x_powerSet
					(ViSession vi, 
					ViReal64 power, 
					ViString powerSetRequest, 
					ViInt16 powerSetChannel, 
					ViInt16 powerSetPortNum);

ViStatus _VI_FUNC ag875x_powerQuery
					(ViSession vi, 
					ViPReal64 currentPowerQuery,
					ViString powerQueryRequest, 
					ViInt16 powerQueryChannel,
					ViInt16 powerQueryPortNum);

ViStatus _VI_FUNC ag875x_softkey
					(ViSession vi, 
					ViString softkey, 
					ViInt16 softkeyNumber);

ViStatus _VI_FUNC ag875x_sweepAltOrChop
					(ViSession vi, 
					ViString sweepAltOrChopRequest, 
					ViPInt16 sweepAltOrChopMode);

ViStatus _VI_FUNC ag875x_sweepStep
					(ViSession vi, 
					ViString sweepStepRequest, 
					ViInt16 sweepStepChannel, 
					ViPInt16 sweepStepState);

ViStatus _VI_FUNC ag875x_sweepTime
					(ViSession vi, 
					ViReal64 sweepTime, 
					ViString sweepTimeRequest, 
					ViInt16 sweepTimeChannel, 
					ViPReal64 currentSweepTime);

ViStatus _VI_FUNC ag875x_takeSweep
					(ViSession vi, ViInt32 sweeps);

ViStatus _VI_FUNC ag875x_averageSmooth
					(ViSession vi, 
					ViReal64 averageSmoothSet, 
					ViString averageSmoothRequest, 
					ViInt16 averageSmoothChannel, 
					ViPReal64 currentAverageSmooth);

ViStatus _VI_FUNC ag875x_bandwidth
					(ViSession vi, 
					ViReal64 bandwidth, 
					ViString bandwidthRequest, 
					ViInt16 bandwidthChannel, 
					ViPReal64 currentBandwidth);

ViStatus _VI_FUNC ag875x_title
					(ViSession vi, 
					ViString title, 
					ViString titleRequest, 
					ViInt16 titleChannel, 
					ViPString currentTitle);

ViStatus _VI_FUNC ag875x_print
					(ViSession vi, 
					ViString printRequest, 
					ViString printModeA, 
					ViString printModeB, 
					ViPString queryPrintMode);

ViStatus _VI_FUNC ag875x_calkit
					(ViSession vi, 
					ViString calkitRequest, 
					ViPString currentCalkit);

ViStatus _VI_FUNC ag875x_message
					(ViSession vi, 
					ViString message);

ViStatus _VI_FUNC ag875x_measType_Q
					(ViSession vi, 
					ViPInt16 currentMeasType, 
					ViPInt16 currentConversion);

ViStatus _VI_FUNC ag875x_sweepType_Q
					(ViSession vi, 
					ViPInt16 currentSweepType);

ViStatus _VI_FUNC ag875x_trigger
					(ViSession vi, 
					ViString triggerRequest, 
					ViInt16 triggerChannel,
					ViPBoolean triggerMode);

ViStatus _VI_FUNC ag875x_display_Q
					(ViSession vi, 
					ViInt16 displayQChannel, 
					ViPInt16 displayQMode, 
					ViPInt16 displayQFormat);

ViStatus _VI_FUNC ag875x_auxChannel
					(ViSession vi, 
					ViInt16 auxChannel, 
					ViString auxChannelRequext, 
					ViPBoolean auxChannelState);

ViStatus _VI_FUNC ag875x_readPowerMeterCal
					(ViSession vi, 
					ViInt16 readPowerMeterCalChannel, 
					ViReal64 _VI_FAR readPowerMeterCalArray[], 
					ViInt16 readPowerMeterCalPoints, 
					ViBoolean interpolatedMode);

ViStatus _VI_FUNC ag875x_srq_enable
					(ViSession vi, 
					ViChar _VI_FAR statstr[]);

ViStatus _VI_FUNC ag875x_plotToFile
					(ViSession vi, 
					ViString plotFile, 
					ViBoolean includeSoftkeys);

ViStatus _VI_FUNC ag875x_calInterpolation
					(ViSession vi, 
					ViString calInterpolationRequest, 
					ViBoolean setCalInterpolationState, 
					ViInt16 calInterpolationChannel, 
					ViPBoolean getCalInterpolationState);

ViStatus _VI_FUNC ag875x_limitTableQuery
					(ViSession vi, 
					ViPInt16 limitTableQuerySegments, 
					ViReal64 _VI_FAR limitTableQueryLimits[], 
					ViString limitTableQueryMode, 
					ViInt16 limitTableQueryChannel);


/* Used for "C" externs in C++ */
#if defined(__cplusplus) || defined(__cplusplus__)
}    /* end of "C" externs for C++ */
#endif 

#endif /* ag875x_INCLUDE */

