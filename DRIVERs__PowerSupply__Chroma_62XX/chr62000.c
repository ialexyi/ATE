/***************************************************************************** 
 *  Copyright 1998 National Instruments Corporation.  All Rights Reserved.   * 
 *****************************************************************************/

/*****************************************************************************
 *  Chroma DC Power Supply 62000P Series Instrument Driver                               
 *  LabWindows/CVI 5.0 Instrument Driver                                     
 *  Environment: NI IVI Compliance Package 2.1
 *		         CVI Run-Time engine 7.0.0.395
 *               NI-VISA 3.01					  
 *  Original Release: 2005/08/17                                  
 *  By: Bill Lu, Chroma                              
 *      PH. 886-3-3279999 ext:3533   Fax 886-3-3278700                              
 *                                                                           
 *  Modification History:                                                    
 *                                                                           
 *       2005/08/17 - Instrument Driver Created.                  
 *                                                                           
 *****************************************************************************/

#include <string.h>
#include <ansi_c.h> 
#include <stdio.h>  
#include <formatio.h>
#include <utility.h> 
#include "chr62000.h"

/*****************************************************************************
 *--------------------- Hidden Attribute Declarations -----------------------*
 *****************************************************************************/

#define CHR62000_ATTR_OPC_TIMEOUT      	    (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 1L)   /* ViInt32 */
#define CHR62000_ATTR_BEEPER                (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 2L)
#define CHR62000_ATTR_OUTPUT                (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 3L)
#define CHR62000_ATTR_PROG_SEQ_VOLTAGE      (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 6L)
#define CHR62000_ATTR_PROG_SEQ_VOLT_SLEW    (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 7L)
#define CHR62000_ATTR_PROG_SEQ_CURRENT      (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 8L)
#define CHR62000_ATTR_PROG_SEQ_CURR_SLEW    (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 9L)
#define CHR62000_ATTR_PROG_SEQ_TTL          (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 10L)
#define CHR62000_ATTR_PROG_SEQ_TIME         (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 11L)
#define CHR62000_ATTR_PROG_SEQ_TYPE         (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 12L)
#define CHR62000_ATTR_PROG_LINK             (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 13L)
#define CHR62000_ATTR_PROG_COUNT            (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 14L)
#define CHR62000_ATTR_PROG_RUN              (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 15L)
#define CHR62000_ATTR_FOLDBACK_PROTECT      (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 16L)
#define CHR62000_ATTR_APG_MODE              (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 17L)
#define CHR62000_ATTR_APG_REFERENCE_VOLTAGE (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 18L)
#define CHR62000_ATTR_MAXPOWER              (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 19L)
#define CHR62000_ATTR_SIMU_RESISTOR         (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 20L)

/*****************************************************************************
 *---------------------------- Useful Macros --------------------------------*
 *****************************************************************************/

    /*- I/O buffer size -----------------------------------------------------*/
#define BUFFER_SIZE                             512L        
#define CHR62000_IO_SESSION_TYPE   IVI_VAL_VISA_SESSION_TYPE

    /*- 488.2 Event Status Register (ESR) Bits ------------------------------*/
#define IEEE_488_2_QUERY_ERROR_BIT              0x04
#define IEEE_488_2_DEVICE_DEPENDENT_ERROR_BIT   0x08
#define IEEE_488_2_EXECUTION_ERROR_BIT          0x10
#define IEEE_488_2_COMMAND_ERROR_BIT            0x20


    /*- List of channels passed to the Ivi_BuildChannelTable function -------*/ 
#define CHANNEL_LIST                            "1"

	/*- Internal define value -----------------------------------------------*/ 
#define	accuracy								0.0001	
#define	delay_time								0.00
#define	MaxChannel								100 
#define OVP_RATIO								1.100
#define OCP_RATIO								1.050
#define OPP_RATIO								1.050

static IviRangeTableEntry attrStatusSreRangeTableEntries[] =
	{
		{0, 255, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrStatusSreRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrStatusSreRangeTableEntries,
	};

static IviRangeTableEntry attrStatusEseRangeTableEntries[] =
	{
		{0, 255, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrStatusEseRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrStatusEseRangeTableEntries,
	};

static IviRangeTableEntry ModelRatingEntries[] =
	{
		{CHR62000_VAL_62006P_100_25_V, 0, 0, "", 0},
		{CHR62000_VAL_62006P_100_25_I, 0, 0, "", 0},
		{CHR62000_VAL_62006P_100_25_P, 0, 0, "", 0},
		{CHR62000_VAL_62012P_30_160_V, 0, 0, "", 0},
		{CHR62000_VAL_62012P_30_160_I, 0, 0, "", 0},
		{CHR62000_VAL_62012P_30_160_P, 0, 0, "", 0},
		{CHR62000_VAL_62012P_80_60_V, 0, 0, "", 0},
		{CHR62000_VAL_62012P_80_60_I, 0, 0, "", 0},
		{CHR62000_VAL_62012P_80_60_P, 0, 0, "", 0},
		{CHR62000_VAL_62012P_100_50_V, 0, 0, "", 0},
		{CHR62000_VAL_62012P_100_50_I, 0, 0, "", 0},
		{CHR62000_VAL_62012P_100_50_P, 0, 0, "", 0},
		{CHR62000_VAL_62012P_600_8_V, 0, 0, "", 0},
		{CHR62000_VAL_62012P_600_8_I, 0, 0, "", 0},
		{CHR62000_VAL_62012P_600_8_P, 0, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable ModelRating =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        ModelRatingEntries,
	};

static IviRangeTableEntry attrFoldbackDelayTimeRangeTableEntries[] =
	{
		{0.01, 600.00, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrFoldbackDelayTimeRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrFoldbackDelayTimeRangeTableEntries,
	};

static IviRangeTableEntry attrApgModeRangeTableEntries[] =
	{
		{CHR62000_VAL_APG_NONE, 0, 0, "NONE", 0},
		{CHR62000_VAL_APG_V, 0, 0, "V", 0},
		{CHR62000_VAL_APG_I, 0, 0, "I", 0},
		{CHR62000_VAL_APG_VI, 0, 0, "VI", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrApgModeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrApgModeRangeTableEntries,
	};

static IviRangeTableEntry attrFoldbackProtectRangeTableEntries[] =
	{
		{CHR62000_VAL_FOLD_DISABLE, 0, 0, "DISABLE", 0},
		{CHR62000_VAL_FOLD_CVTOCC, 0, 0, "CVTOCC", 0},
		{CHR62000_VAL_FOLD_CCTOCV, 0, 0, "CCTOCV", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrFoldbackProtectRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrFoldbackProtectRangeTableEntries,
	};

static IviRangeTableEntry attrProgSeqVoltageRangeTableEntries[] =
	{
		{0.00, 80.00, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrProgSeqVoltageRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrProgSeqVoltageRangeTableEntries,
	};

static IviRangeTableEntry attrProgSeqVoltSlewRangeTableEntries[] =
	{
		{0.01, 10.00, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrProgSeqVoltSlewRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrProgSeqVoltSlewRangeTableEntries,
	};

static IviRangeTableEntry attrProgSeqCurrentRangeTableEntries[] =
	{
		{0.00, 60.00, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrProgSeqCurrentRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrProgSeqCurrentRangeTableEntries,
	};

static IviRangeTableEntry attrProgSeqCurrSlewRangeTableEntries[] =
	{
		{0.01, 1.00, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrProgSeqCurrSlewRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrProgSeqCurrSlewRangeTableEntries,
	};

static IviRangeTableEntry attrProgSeqTtlRangeTableEntries[] =
	{
		{0, 255, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrProgSeqTtlRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrProgSeqTtlRangeTableEntries,
	};

static IviRangeTableEntry attrProgSeqTimeRangeTableEntries[] =
	{
		{0.00, 15000.00, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrProgSeqTimeRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrProgSeqTimeRangeTableEntries,
	};

static IviRangeTableEntry attrProgSeqSelectedRangeTableEntries[] =
	{
		{1, 10, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrProgSeqSelectedRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrProgSeqSelectedRangeTableEntries,
	};

static IviRangeTableEntry attrProgLinkRangeTableEntries[] =
	{
		{0, 10, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrProgLinkRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrProgLinkRangeTableEntries,
	};

static IviRangeTableEntry attrProgCountRangeTableEntries[] =
	{
		{1, 15000, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrProgCountRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrProgCountRangeTableEntries,
	};

static IviRangeTableEntry attrProgSelectedRangeTableEntries[] =
	{
		{1, 10, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrProgSelectedRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrProgSelectedRangeTableEntries,
	};

static IviRangeTableEntry attrProgSeqTypeRangeTableEntries[] =
	{
		{CHR62000_VAL_AUTO, 0, 0, "AUTO", 0},
		{CHR62000_VAL_MANUAL, 0, 0, "MANUAL", 0},
		{CHR62000_VAL_TRIGGER, 0, 0, "TRI", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrProgSeqTypeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrProgSeqTypeRangeTableEntries,
	};

static IviRangeTableEntry attrTtlPortRangeTableEntries[] =
	{
		{0, 255, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrTtlPortRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrTtlPortRangeTableEntries,
	};



static IviRangeTableEntry attrVoltLimitHighRangeTableEntries[] =
	{
		{0.0, 80.0, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrVoltLimitHighRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrVoltLimitHighRangeTableEntries,
	};

static IviRangeTableEntry attrVoltLimitLowRangeTableEntries[] =
	{
		{0.0, 80.0, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrVoltLimitLowRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrVoltLimitLowRangeTableEntries,
	};

static IviRangeTableEntry attrVoltProtectRangeTableEntries[] =
	{
		{0.0, 88.0, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrVoltProtectRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrVoltProtectRangeTableEntries,
	};

static IviRangeTableEntry attrVoltSlewRangeTableEntries[] =
	{
		{0.01, 10.00, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrVoltSlewRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrVoltSlewRangeTableEntries,
	};

static IviRangeTableEntry attrCurrentRangeTableEntries[] =
	{
		{0.00, 60.00, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrCurrentRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrCurrentRangeTableEntries,
	};

static IviRangeTableEntry attrCurrLimitHighRangeTableEntries[] =
	{
		{0.0, 60.0, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrCurrLimitHighRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrCurrLimitHighRangeTableEntries,
	};

static IviRangeTableEntry attrCurrLimitLowRangeTableEntries[] =
	{
		{0.0, 60.0, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrCurrLimitLowRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrCurrLimitLowRangeTableEntries,
	};

static IviRangeTableEntry attrCurrProtectRangeTableEntries[] =
	{
		{0.0, 63.0, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrCurrProtectRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrCurrProtectRangeTableEntries,
	};

static IviRangeTableEntry attrCurrSlewRangeTableEntries[] =
	{
		{0.01, 1.00, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrCurrSlewRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrCurrSlewRangeTableEntries,
	};

static IviRangeTableEntry attrPowerProtectRangeTableEntries[] =
	{
		{0, 1260.0, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrPowerProtectRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrPowerProtectRangeTableEntries,
	};

static IviRangeTableEntry attrFetchTypeRangeTableEntries[] =
	{
		{CHR62000_VAL_FETCH_VOLT, 0, 0, "VOLT?", 0},
		{CHR62000_VAL_FETCH_CURR, 0, 0, "CURR?", 0},
		{CHR62000_VAL_FETCH_POWER, 0, 0, "POW?", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrFetchTypeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrFetchTypeRangeTableEntries,
	};

static IviRangeTableEntry attrMeasTypeRangeTableEntries[] =
	{
		{CHR62000_VAL_MEAS_VOLT, 0, 0, "VOLT?", 0},
		{CHR62000_VAL_MEAS_CURR, 0, 0, "CURR?", 0},
		{CHR62000_VAL_MEAS_POWER, 0, 0, "POW?", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrMeasTypeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrMeasTypeRangeTableEntries,
	};

static IviRangeTableEntry attrVoltageRangeTableEntries[] =
	{
		{0.00, 80.00, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrVoltageRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrVoltageRangeTableEntries,
	};

/*****************************************************************************
 *-------------- Utility Function Declarations (Non-Exported) ---------------*
 *****************************************************************************/
static ViStatus chr62000_InitAttributes (ViSession vi);
static ViStatus chr62000_DefaultInstrSetup (ViSession openInstrSession);
static ViStatus chr62000_CheckStatus (ViSession vi);
static ViStatus chr62000_WaitForOPC (ViSession vi, ViInt32 maxTime);

ViStatus _VI_FUNC chr62000_Invalidate_Program (ViSession vi, ViInt32 programSelected);
ViStatus _VI_FUNC chr62000_Simulate_Meas_Fetch (ViSession vi, ViInt32 measureType, ViReal64 *readValue);
ViStatus _VI_FUNC chr62000_SelectModelMaxRating (ViSession vi);
ViStatus _VI_FUNC chr62000_SwapAttributeViReal64Max (ViSession vi, ViAttr attributeId, ViReal64 value);
ViStatus _VI_FUNC chr62000_Digit_boundary (ViReal64 *value, ViInt32 digit);
ViStatus _VI_FUNC chr62000_Detect_error (ViSession vi, ViChar Cmd[]); 

    /*- File I/O Utility Functions -*/
static ViStatus chr62000_ReadToFile (ViSession vi, ViConstString filename, 
                                     ViInt32 maxBytesToRead, ViInt32 fileAction, ViInt32 *totalBytesWritten);
static ViStatus chr62000_WriteFromFile (ViSession vi, ViConstString filename, 
                                        ViInt32 maxBytesToWrite, ViInt32 byteOffset, 
                                        ViInt32 *totalBytesWritten);

/*****************************************************************************
 *----------------- Callback Declarations (Non-Exported) --------------------*
 *****************************************************************************/

    /*- Global Session Callbacks --------------------------------------------*/
    
static ViStatus _VI_FUNC chr62000_CheckStatusCallback (ViSession vi, ViSession io);
static ViStatus _VI_FUNC chr62000_WaitForOPCCallback (ViSession vi, ViSession io);

    /*- Attribute callbacks -------------------------------------------------*/

static ViStatus _VI_FUNC chr62000AttrDriverRevision_ReadCallback (ViSession vi,
                                                                  ViSession io, 
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId, 
                                                                  const ViConstString cacheValue);
static ViStatus _VI_FUNC chr62000AttrInstrumentManufacturer_ReadCallback (ViSession vi, 
                                                                          ViSession io,
                                                                          ViConstString channelName, 
                                                                          ViAttr attributeId,
                                                                          const ViConstString cacheValue);
static ViStatus _VI_FUNC chr62000AttrInstrumentModel_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue);

static ViStatus _VI_FUNC chr62000AttrFirmwareRevision_ReadCallback (ViSession vi, 
                                                                    ViSession io,
                                                                    ViConstString channelName, 
                                                                    ViAttr attributeId,
                                                                    const ViConstString cacheValue);
static ViStatus _VI_FUNC chr62000AttrIdQueryResponse_ReadCallback (ViSession vi,
                                                                   ViSession io, 
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId, 
                                                                   const ViConstString cacheValue);

static ViStatus _VI_FUNC chr62000AttrVoltage_ReadCallback (ViSession vi,
                                                           ViSession io,
                                                           ViConstString channelName,
                                                           ViAttr attributeId,
                                                           ViReal64 *value);

static ViStatus _VI_FUNC chr62000AttrVoltage_WriteCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViReal64 value);

static ViStatus _VI_FUNC chr62000AttrVoltLimitHigh_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 *value);

static ViStatus _VI_FUNC chr62000AttrVoltLimitHigh_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value);

static ViStatus _VI_FUNC chr62000AttrVoltLimitLow_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr62000AttrVoltLimitLow_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr62000AttrVoltProtect_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);

static ViStatus _VI_FUNC chr62000AttrVoltProtect_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr62000AttrVoltSlew_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViReal64 *value);

static ViStatus _VI_FUNC chr62000AttrVoltSlew_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 value);

static ViStatus _VI_FUNC chr62000AttrCurrent_ReadCallback (ViSession vi,
                                                           ViSession io,
                                                           ViConstString channelName,
                                                           ViAttr attributeId,
                                                           ViReal64 *value);

static ViStatus _VI_FUNC chr62000AttrCurrent_WriteCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViReal64 value);

static ViStatus _VI_FUNC chr62000AttrCurrLimitHigh_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 *value);

static ViStatus _VI_FUNC chr62000AttrCurrLimitHigh_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value);

static ViStatus _VI_FUNC chr62000AttrCurrLimitLow_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr62000AttrCurrLimitLow_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr62000AttrCurrProtect_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);

static ViStatus _VI_FUNC chr62000AttrCurrProtect_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr62000AttrCurrSlew_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViReal64 *value);

static ViStatus _VI_FUNC chr62000AttrCurrSlew_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 value);

static ViStatus _VI_FUNC chr62000AttrPowerProtect_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr62000AttrPowerProtect_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);


static ViStatus _VI_FUNC chr62000AttrOutput_WriteCallback (ViSession vi,
                                                           ViSession io,
                                                           ViConstString channelName,
                                                           ViAttr attributeId,
                                                           ViBoolean value);

static ViStatus _VI_FUNC chr62000AttrTtlPort_ReadCallback (ViSession vi,
                                                           ViSession io,
                                                           ViConstString channelName,
                                                           ViAttr attributeId,
                                                           ViInt32 *value);

static ViStatus _VI_FUNC chr62000AttrTtlPort_WriteCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 value);

static ViStatus _VI_FUNC chr62000AttrBeeper_ReadCallback (ViSession vi,
                                                          ViSession io,
                                                          ViConstString channelName,
                                                          ViAttr attributeId,
                                                          ViBoolean *value);

static ViStatus _VI_FUNC chr62000AttrBeeper_WriteCallback (ViSession vi,
                                                           ViSession io,
                                                           ViConstString channelName,
                                                           ViAttr attributeId,
                                                           ViBoolean value);


static ViStatus _VI_FUNC chr62000AttrRemoteMode_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViBoolean value);

static ViStatus _VI_FUNC chr62000AttrFoldbackDelayTime_ReadCallback (ViSession vi,
                                                                     ViSession io,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViReal64 *value);

static ViStatus _VI_FUNC chr62000AttrFoldbackDelayTime_WriteCallback (ViSession vi,
                                                                      ViSession io,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViReal64 value);

static ViStatus _VI_FUNC chr62000AttrFoldbackProtect_ReadCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViInt32 *value);

static ViStatus _VI_FUNC chr62000AttrFoldbackProtect_WriteCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViInt32 value);

static ViStatus _VI_FUNC chr62000AttrApgMode_ReadCallback (ViSession vi,
                                                           ViSession io,
                                                           ViConstString channelName,
                                                           ViAttr attributeId,
                                                           ViInt32 *value);

static ViStatus _VI_FUNC chr62000AttrApgMode_WriteCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 value);

static ViStatus _VI_FUNC chr62000AttrApgReferenceVoltage_ReadCallback (ViSession vi,
                                                                       ViSession io,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViBoolean *value);

static ViStatus _VI_FUNC chr62000AttrApgReferenceVoltage_WriteCallback (ViSession vi,
                                                                        ViSession io,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViBoolean value);

static ViStatus _VI_FUNC chr62000AttrProgSelected_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value);

static ViStatus _VI_FUNC chr62000AttrProgSelected_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value);

static ViStatus _VI_FUNC chr62000AttrProgSeqVoltage_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 *value);

static ViStatus _VI_FUNC chr62000AttrProgSeqVoltage_WriteCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 value);

static ViStatus _VI_FUNC chr62000AttrProgSeqVoltSlew_ReadCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 *value);

static ViStatus _VI_FUNC chr62000AttrProgSeqVoltSlew_WriteCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViReal64 value);

static ViStatus _VI_FUNC chr62000AttrProgSeqCurrent_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 *value);

static ViStatus _VI_FUNC chr62000AttrProgSeqCurrent_WriteCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 value);

static ViStatus _VI_FUNC chr62000AttrProgSeqCurrSlew_ReadCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 *value);

static ViStatus _VI_FUNC chr62000AttrProgSeqCurrSlew_WriteCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViReal64 value);

static ViStatus _VI_FUNC chr62000AttrProgSeqTtl_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 *value);

static ViStatus _VI_FUNC chr62000AttrProgSeqTtl_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 value);

static ViStatus _VI_FUNC chr62000AttrProgSeqTime_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);

static ViStatus _VI_FUNC chr62000AttrProgSeqTime_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr62000AttrProgSeqType_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value);

static ViStatus _VI_FUNC chr62000AttrProgSeqType_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value);

static ViStatus _VI_FUNC chr62000AttrProgLink_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 *value);

static ViStatus _VI_FUNC chr62000AttrProgLink_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 value);

static ViStatus _VI_FUNC chr62000AttrProgCount_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value);

static ViStatus _VI_FUNC chr62000AttrProgCount_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value);

static ViStatus _VI_FUNC chr62000AttrProgSeqSelected_ReadCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViInt32 *value);

static ViStatus _VI_FUNC chr62000AttrProgSeqSelected_WriteCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViInt32 value);

static ViStatus _VI_FUNC chr62000AttrProgRun_ReadCallback (ViSession vi,
                                                           ViSession io,
                                                           ViConstString channelName,
                                                           ViAttr attributeId,
                                                           ViBoolean *value);

static ViStatus _VI_FUNC chr62000AttrProgRun_WriteCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViBoolean value);

static ViStatus _VI_FUNC chr62000AttrStatusEse_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value);

static ViStatus _VI_FUNC chr62000AttrStatusEse_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value);

static ViStatus _VI_FUNC chr62000AttrStatusSre_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value);

static ViStatus _VI_FUNC chr62000AttrStatusSre_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value);
                                                              
ViInt32 g_nBaudRate;
                                                                        
                                                                        
/*****************************************************************************
 *------------ Instrument-specific public Functions (Non-Exported Functions) ---------------*
 *****************************************************************************/
 
/*****************************************************************************
 * Function: chr62000_Simulate_Meas_Fetch   
 * Purpose:  This function simulate the measure and fetch functions when 
 *           Ivi_Simulating
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_Simulate_Meas_Fetch (ViSession vi, ViInt32 measureType, ViReal64 *readValue)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	simulated_value=0,Resistor=0;
	ViBoolean	output_state=VI_FALSE;

	checkErr( Ivi_GetAttributeViBoolean (vi, VI_NULL, CHR62000_ATTR_OUTPUT, 0,
											&output_state));	
	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_SIMU_RESISTOR, 0,
											&Resistor));
												
	if ( output_state == VI_TRUE)
	{
		switch (measureType)
		{
			case 0:	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_VOLTAGE, IVI_VAL_DIRECT_USER_CALL,
											&simulated_value));
					(*readValue)=simulated_value+((ViReal64)(rand()%30)/100000); 
					break;
			case 1:	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_VOLTAGE, IVI_VAL_DIRECT_USER_CALL,
											&simulated_value));
					(*readValue)=(simulated_value+((ViReal64)(rand()%40)/10000))/Resistor;
					break;
			case 2:	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_VOLTAGE, IVI_VAL_DIRECT_USER_CALL,
											&simulated_value));
					(*readValue)=(simulated_value*simulated_value+((ViReal64)(rand()%50)/10000))/Resistor;
					break;
		}
	}
	else
		(*readValue)=0.00;
	
Error:
	return error;
}
 
/*****************************************************************************
 * Function: chr62000_SelectModelMaxRating   
 * Purpose:  Exchange the max. rating of attributes by different 
 *           DC Power Supply model
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SelectModelMaxRating (ViSession vi)
{
	ViStatus    error = VI_SUCCESS; 
	ViChar		rdBuffer[BUFFER_SIZE]; 
	ViInt32		ModelIndex=6;
	ViReal64	volt_max=0,curr_max=0,power_max=0;
	
	checkErr( Ivi_GetAttributeViString (vi, VI_NULL, CHR62000_ATTR_INSTRUMENT_MODEL,
							  			IVI_VAL_DIRECT_USER_CALL, BUFFER_SIZE, rdBuffer));
							  			
	if ( strncmp(rdBuffer,"62006P-100-25",13) == 0 )							  			
		ModelIndex=0;
	else if ( strncmp(rdBuffer,"62012P-30-160",13) == 0 )
		ModelIndex=3;	
	else if ( strncmp(rdBuffer,"62012P-80-60",12) == 0 )
		ModelIndex=6;	
	else if ( strncmp(rdBuffer,"62012P-100-50",13) == 0 )
		ModelIndex=9;	
	else if ( strncmp(rdBuffer,"62012P-600-8",12) == 0 )
		ModelIndex=12;
		
	checkErr( Ivi_GetViReal64EntryFromIndex (ModelIndex, &ModelRating, &volt_max, VI_NULL, VI_NULL, VI_NULL, VI_NULL));
	checkErr( Ivi_GetViReal64EntryFromIndex (ModelIndex+1, &ModelRating, &curr_max, VI_NULL, VI_NULL, VI_NULL, VI_NULL)); 
	checkErr( Ivi_GetViReal64EntryFromIndex (ModelIndex+2, &ModelRating, &power_max, VI_NULL, VI_NULL, VI_NULL, VI_NULL)); 
	
	checkErr( chr62000_SwapAttributeViReal64Max (vi, CHR62000_ATTR_VOLTAGE, volt_max) );
	checkErr( chr62000_SwapAttributeViReal64Max (vi, CHR62000_ATTR_VOLT_LIMIT_HIGH, volt_max) ); 
	checkErr( chr62000_SwapAttributeViReal64Max (vi, CHR62000_ATTR_VOLT_LIMIT_LOW, volt_max) ); 
	checkErr( chr62000_SwapAttributeViReal64Max (vi, CHR62000_ATTR_VOLT_PROTECT, volt_max*OVP_RATIO) ); 
	checkErr( chr62000_SwapAttributeViReal64Max (vi, CHR62000_ATTR_PROG_SEQ_VOLTAGE, volt_max) ); 
	
	checkErr( chr62000_SwapAttributeViReal64Max (vi, CHR62000_ATTR_CURRENT, curr_max) );
	checkErr( chr62000_SwapAttributeViReal64Max (vi, CHR62000_ATTR_CURR_LIMIT_HIGH, curr_max) ); 
	checkErr( chr62000_SwapAttributeViReal64Max (vi, CHR62000_ATTR_CURR_LIMIT_LOW, curr_max) ); 
	checkErr( chr62000_SwapAttributeViReal64Max (vi, CHR62000_ATTR_CURR_PROTECT, curr_max*OCP_RATIO) ); 
	checkErr( chr62000_SwapAttributeViReal64Max (vi, CHR62000_ATTR_PROG_SEQ_CURRENT, curr_max) ); 

	checkErr( Ivi_SetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_MAXPOWER, 0, power_max));
	checkErr( chr62000_SwapAttributeViReal64Max (vi, CHR62000_ATTR_POWER_PROTECT, power_max*OPP_RATIO) ); 
	
	// decide the CHR62000_ATTR_SIMU_RESISTOR,  
	// power_max*0.7 < (volt_max*volt_max*CHR62000_ATTR_SIMU_RESISTOR) < power_max*0.9
	if (1)
	{
		ViReal64	a=0,res_max,res_min,act_ers,mult=1;
		srand( (unsigned) time(NULL) );
		res_max=(volt_max*volt_max)/(power_max*0.7);
		res_min=(volt_max*volt_max)/(power_max*0.9);  
		
		if ( res_max < 100)
			mult=100,res_max*=mult,res_min*=mult;
			
		while ( (a=(rand()%((ViInt32)res_max))) < res_min);

		checkErr( Ivi_SetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_SIMU_RESISTOR, 0, a/mult)); 
	}
	
Error:
	return error;
}

/*****************************************************************************
 * Function: chr62000_SwapAttributeViReal64Max   
 * Purpose:  A subroutine called by chr62000_SelectModelMaxRating
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SwapAttributeViReal64Max (ViSession vi, ViAttr attributeId, ViReal64 value)
{
	ViStatus    error = VI_SUCCESS;   
	IviRangeTablePtr	tblPtr=VI_NULL;
	
	checkErr( Ivi_GetStoredRangeTablePtr (vi, attributeId, &tblPtr));
	checkErr( Ivi_SetRangeTableEntry (tblPtr, 0, VI_NULL, value, VI_NULL, VI_NULL, VI_NULL));
	//checkErr( Ivi_SetStoredRangeTablePtr (vi, attributeId, tblPtr));	

Error:
	return error;
}

/*****************************************************************************
 * Function: chr62000_Digit_boundary   
 * Purpose:  specify valid decimal place for specified value
 * support only -- valid value:+-10730000, valid digit: 1 & 2
 * accuracy to the sixth decimal place
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_Digit_boundary (ViReal64 *value, ViInt32 digit)
{
	ViStatus    error = VI_SUCCESS;   
	ViInt32		intvalue=1;
	
	*value=*value+0.000001; // if *value=0.999999 then *value=1; 
	
	if (*value<10730000 && *value>-10730000 && digit<=2 && digit>=1)
	{
		ViInt32	mult=1;
		if (digit==1)
			mult=10;
		else
			mult=100;
		*value=(*value)*mult;
		intvalue=*value;	   
		*value=((ViReal64)(intvalue))/mult;
	}

Error:
	return error;
}


/*****************************************************************************
 * Function: chr62000_Detect_error   
 * Purpose:  specify valid decimal place for specified value
 * support only -- valid value:+-10730000, valid digit: 1 & 2
 * accuracy to the sixth decimal place
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_Detect_error (ViSession vi, ViChar Cmd[])
{
	ViStatus    error = VI_SUCCESS;   
	ViChar		rdBuffer[BUFFER_SIZE];
	ViInt32		errCode=256; 
	
	checkErr( chr62000_error_query (vi, &errCode, rdBuffer));
	
	if (errCode!=0)
	{
		ViSession   io = Ivi_IOSession(vi); 
		
		viCheckErr( viPrintf (io, "%s", Cmd));
		Delay (delay_time);
		checkErr( error= CHR62000_ERROR_EXECUTION ); 
	}
	
Error:
	return error;
}

/*****************************************************************************
 * Function: chr62000_Invalidate_Program   
 * Purpose:  Invalidate some attributes of program when the user clear program
 *           parameter
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_Invalidate_Program (ViSession vi, ViInt32 programSelected)
{
	ViStatus    error = VI_SUCCESS;
    ViChar		ChannelName[10];
    ViInt32		i=0,nCount=0,index=0;
	
    /* ChannelName counting and invalidate channel-base attribute */ 
    index= (programSelected-1)*10+1;
    if (!Ivi_Simulating(vi)) 
    {
    	Fmt (ChannelName,"%d", programSelected); 
		checkErr( Ivi_InvalidateAttribute (vi, ChannelName, CHR62000_ATTR_PROG_LINK));
		checkErr( Ivi_InvalidateAttribute (vi, ChannelName, CHR62000_ATTR_PROG_COUNT));
    	for(i=index;i<index+10;i++)
		{
			Fmt (ChannelName,"%d",i);
			checkErr( Ivi_InvalidateAttribute (vi, ChannelName, CHR62000_ATTR_PROG_SEQ_TYPE)); 
			checkErr( Ivi_InvalidateAttribute (vi, ChannelName, CHR62000_ATTR_PROG_SEQ_VOLTAGE)); 
			checkErr( Ivi_InvalidateAttribute (vi, ChannelName, CHR62000_ATTR_PROG_SEQ_VOLT_SLEW)); 
			checkErr( Ivi_InvalidateAttribute (vi, ChannelName, CHR62000_ATTR_PROG_SEQ_CURRENT)); 
			checkErr( Ivi_InvalidateAttribute (vi, ChannelName, CHR62000_ATTR_PROG_SEQ_CURR_SLEW)); 
			checkErr( Ivi_InvalidateAttribute (vi, ChannelName, CHR62000_ATTR_PROG_SEQ_TTL)); 
			checkErr( Ivi_InvalidateAttribute (vi, ChannelName, CHR62000_ATTR_PROG_SEQ_TIME)); 
		}
	}	
	else // Simulating
	{
		Fmt (ChannelName,"%d", programSelected);
		checkErr( Ivi_SetAttributeViInt32 (vi, ChannelName, CHR62000_ATTR_PROG_LINK, 
											0, 0));
		checkErr( Ivi_SetAttributeViInt32 (vi, ChannelName, CHR62000_ATTR_PROG_COUNT,
											0, 1));
   		for(i=index;i<index+10;i++)
		{
			Fmt (ChannelName,"%d",i);
			checkErr( Ivi_SetAttributeViInt32 (vi, ChannelName, CHR62000_ATTR_PROG_SEQ_TYPE, 0, 0)); 
			checkErr( Ivi_SetAttributeViReal64 (vi, ChannelName, CHR62000_ATTR_PROG_SEQ_VOLTAGE, 0, 0.00)); 
			checkErr( Ivi_SetAttributeViReal64 (vi, ChannelName, CHR62000_ATTR_PROG_SEQ_VOLT_SLEW, 0, 10.00)); 
			checkErr( Ivi_SetAttributeViReal64 (vi, ChannelName, CHR62000_ATTR_PROG_SEQ_CURRENT, 0, 0.00)); 
			checkErr( Ivi_SetAttributeViReal64 (vi, ChannelName, CHR62000_ATTR_PROG_SEQ_CURR_SLEW, 0, 1.00)); 
			checkErr( Ivi_SetAttributeViInt32 (vi, ChannelName, CHR62000_ATTR_PROG_SEQ_TTL, 0, 0)); 
			checkErr( Ivi_SetAttributeViReal64 (vi, ChannelName, CHR62000_ATTR_PROG_SEQ_TIME, 0, 0.00)); 
		}		
	}
	
Error:
	return error;
}
                                                                       
/*****************************************************************************
 *------------ User-Callable Functions (Exportable Functions) ---------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: chr62000_init   
 * Purpose:  VXIplug&play required function.  Calls the   
 *           chr62000_InitWithOptions function.   
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_init (ViRsrc resourceName, ViBoolean IDQuery,
                                 ViBoolean resetDevice, ViSession *newVi)
{   
    ViStatus    error = VI_SUCCESS;

    if (newVi == VI_NULL)
        {
        Ivi_SetErrorInfo (VI_NULL, VI_FALSE, IVI_ERROR_INVALID_PARAMETER, 
                          VI_ERROR_PARAMETER4, "Null address for Instrument Handle");
        checkErr( IVI_ERROR_INVALID_PARAMETER);
        }

    checkErr( chr62000_InitWithOptions (resourceName, IDQuery, resetDevice, 
                                        "", newVi));

Error:  
    return error;
}

/*****************************************************************************
 * Function: chr62000_InitInterface   
 * Purpose:  A function which can initial the driver with GPIB or RS-232.  
 *           Calls the chr62000_InitWithOptions function.   
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_InitInterface (ViRsrc resourceName, ViBoolean IDQuery,
                                          ViBoolean resetDevice,
                                          ViInt32 baudRate, ViChar IDString[],
                                          ViChar modelNumber[],
                                          ViSession *newVi)
{
  	ViStatus    error = VI_SUCCESS;
    ViSession   vi = VI_NULL;

    g_nBaudRate=baudRate;
    
    if (newVi == VI_NULL)
        {
        Ivi_SetErrorInfo (VI_NULL, VI_FALSE, IVI_ERROR_INVALID_PARAMETER, 
                          VI_ERROR_PARAMETER4, "Null address for Instrument Handle");
        checkErr( IVI_ERROR_INVALID_PARAMETER);
        }
	if (IDString == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 5, "Null address for ID String" );
	if (modelNumber == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 6, "Null address for Model Number" );
	        
    checkErr( chr62000_InitWithOptions (resourceName, IDQuery, resetDevice, 
                                        "", newVi));
    vi=*newVi; 
	checkErr( Ivi_GetAttributeViString (vi, VI_NULL, CHR62000_ATTR_ID_QUERY_RESPONSE, 
                                        0, BUFFER_SIZE, IDString));
	sscanf (IDString, "%*[^,],%256[^,]", modelNumber);
    checkErr( chr62000_CheckStatus (vi));
    
Error:  
    return error;
}

/*****************************************************************************
 * Function: chr62000_InitWithOptions                                       
 * Purpose:  This function creates a new IVI session and calls the 
 *           IviInit function.                                     
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_InitWithOptions (ViRsrc resourceName, ViBoolean IDQuery,
                                            ViBoolean resetDevice, ViString optionString, 
                                            ViSession *newVi)
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
    
    checkErr( Ivi_GetInfoFromResourceName (resourceName, optionString, newResourceName,
                                           newOptionString, &isLogicalName));
    
        /* create a new interchangeable driver */
    checkErr( Ivi_SpecificDriverNew ("chr62000", newOptionString, &vi));  
    
        /* init the driver */
    checkErr( chr62000_IviInit (newResourceName, IDQuery, resetDevice, vi)); 
    if (isLogicalName)	  
        checkErr( Ivi_ApplyDefaultSetup (vi));
    *newVi = vi;
    
Error:
    if (error < VI_SUCCESS) 
        Ivi_Dispose (vi);
        
    return error;
}

/*****************************************************************************
 * Function: chr62000_IviInit                                                       
 * Purpose:  This function is called by chr62000_InitWithOptions  
 *           or by an IVI class driver.  This function initializes the I/O 
 *           interface, optionally resets the device, optionally performs an
 *           ID query, and sends a default setup to the instrument.                
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_IviInit (ViRsrc resourceName, ViBoolean IDQuery,
                                    ViBoolean reset, ViSession vi)
{
    ViStatus    error = VI_SUCCESS;
    ViSession   io = VI_NULL;
    ViChar		Expand_ChannelName[BUFFER_SIZE];
    ViInt32		i=0,nCount=0;
    
    /* Expand Channel to MaxChannel */ 
    for(i=1;i<=MaxChannel;i++)
	{
		if (i==MaxChannel)
			nCount+= sprintf(Expand_ChannelName+nCount,"%d",i);			
		else
			nCount+= sprintf(Expand_ChannelName+nCount,"%d,",i); 
	}
    checkErr( Ivi_BuildChannelTable (vi, Expand_ChannelName, VI_FALSE, VI_NULL));
   
     /* Add attributes */
    checkErr( chr62000_InitAttributes (vi));
    
	if ( !Ivi_Simulating(vi) )		  
	{
	    ViSession   rmSession = VI_NULL;

	        /* Open instrument session */
	    checkErr( Ivi_GetAttributeViSession (vi, VI_NULL, IVI_ATTR_VISA_RM_SESSION, 0,
	                                       		&rmSession)); 
	    viCheckErr( viOpen (rmSession, resourceName, VI_NULL, VI_NULL, &io));
	        /* io session owned by driver now */
	    checkErr( Ivi_SetAttributeViSession (vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, io));  

		if ( strncmp(resourceName,"ASRL",4)==0 )
		{
			//Configure VISA Formatted I/O 
			viCheckErr( viFlush (io, VI_ASRL_IN_BUF|VI_ASRL_OUT_BUF|VI_WRITE_BUF_DISCARD|VI_READ_BUF_DISCARD));
			viCheckErr( viSetBuf (io, VI_ASRL_IN_BUF | VI_ASRL_OUT_BUF, 512));
			viCheckErr( viSetAttribute (io, VI_ATTR_TERMCHAR, 0x0A));
			viCheckErr( viSetAttribute (io, VI_ATTR_ASRL_END_IN, VI_ASRL_END_TERMCHAR)); // VI_ASRL_END_NONE or VI_ASRL_END_TERMCHAR
			viCheckErr( viSetAttribute (io, VI_ATTR_ASRL_END_OUT, VI_ASRL_END_TERMCHAR)); // VI_ASRL_END_NONE or VI_ASRL_END_TERMCHAR
			viCheckErr( viSetAttribute (io, VI_ATTR_TERMCHAR_EN, VI_TRUE));			
			viCheckErr( viSetAttribute (io, VI_ATTR_ASRL_FLOW_CNTRL, VI_ASRL_FLOW_NONE));
			viCheckErr( viSetAttribute (io, VI_ATTR_ASRL_BAUD, g_nBaudRate));
			viCheckErr( viSetAttribute (io, VI_ATTR_ASRL_DATA_BITS, 8));
			viCheckErr( viSetAttribute (io, VI_ATTR_ASRL_PARITY,0));
			viCheckErr( viSetAttribute (io, VI_ATTR_ASRL_STOP_BITS,10));
		}
			/* Configure VISA Formatted I/O */
		viCheckErr( viSetAttribute (io, VI_ATTR_TMO_VALUE, 5000 ));
		viCheckErr( viSetBuf (io, VI_READ_BUF | VI_WRITE_BUF, 10000));
		viCheckErr( viSetAttribute (io, VI_ATTR_WR_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS));
		viCheckErr( viSetAttribute (io, VI_ATTR_RD_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS));
	}

    /*- Reset instrument ----------------------------------------------------*/
    if (reset) 
        checkErr( chr62000_reset (vi));
    else  /*- Send Default Instrument Setup ---------------------------------*/
        checkErr( chr62000_DefaultInstrSetup (vi));
	
	/*- Identification Query ------------------------------------------------*/
	if (IDQuery)                               
	    {
	    ViChar rdBuffer[BUFFER_SIZE];
	
	    #define VALID_RESPONSE_STRING_START        "CHROMA,62"
	
	    checkErr( Ivi_GetAttributeViString (vi, VI_NULL, CHR62000_ATTR_ID_QUERY_RESPONSE, 
	                                        0, BUFFER_SIZE, rdBuffer));
	    
	    if (strncmp (rdBuffer, VALID_RESPONSE_STRING_START, 
	                 strlen(VALID_RESPONSE_STRING_START)) != 0)
	        {             
	        viCheckErr( VI_ERROR_FAIL_ID_QUERY);
	        }
	    }
	
	checkErr( chr62000_SelectModelMaxRating (vi)); // switch the Max. Rating by Model
    checkErr( chr62000_CheckStatus (vi));

Error:
    if (error < VI_SUCCESS)
        {
        if (!Ivi_Simulating (vi) && io)
            viClose (io);
        }
    return error;
}

/*****************************************************************************
 * Function: chr62000_close                                                           
 * Purpose:  This function closes the instrument.                            
 *
 *           Note:  This function must unlock the session before calling
 *           Ivi_Dispose.
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_close (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));
    
    checkErr( chr62000_IviClose (vi));

Error:    
    Ivi_UnlockSession (vi, VI_NULL);
    Ivi_Dispose (vi);  

    return error;
}

/*****************************************************************************
 * Function: chr62000_IviClose                                                        
 * Purpose:  This function performs all of the drivers clean-up operations   
 *           except for closing the IVI session.  This function is called by 
 *           chr62000_close or by an IVI class driver. 
 *
 *           Note:  This function must close the I/O session and set 
 *           IVI_ATTR_IO_SESSION to 0.
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_IviClose (ViSession vi)
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
 * Function: chr62000_Measure   
 * Purpose:  This function measure the voltage | current | power of the 
 *           instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_Measure (ViSession vi, ViInt32 measureType,
                                 ViReal64 *readValue)
{
	ViStatus	error = VI_SUCCESS;
	ViString	MeasCmd;
	
	checkErr( Ivi_LockSession (vi, VI_NULL)); 
	
	if (readValue == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for ReadValue" );
		
	if (!Ivi_Simulating(vi))
    {					  
		ViSession   io = Ivi_IOSession(vi);
		checkErr( Ivi_GetViInt32EntryFromIndex (measureType, &attrMeasTypeRangeTable, VI_NULL, VI_NULL,
												VI_NULL, &MeasCmd, VI_NULL));
		viCheckErr( viPrintf (io, "MEAS:%s\n", MeasCmd));
		Delay (delay_time);
		viCheckErr( viScanf (io, "%Lf", readValue));
		Delay (delay_time);
	}
	else //simulation
		chr62000_Simulate_Meas_Fetch (vi, measureType, readValue);
		
	checkErr( chr62000_CheckStatus (vi));
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_Fetch   
 * Purpose:  This function fetch the voltage | current | power of the 
 *           instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_Fetch (ViSession vi, ViInt32 fetchType,
                                  ViReal64 *readValue)
{
	ViStatus	error = VI_SUCCESS;
	ViString	FetchCmd;
	
	checkErr( Ivi_LockSession (vi, VI_NULL)); 
	
	if (readValue == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for ReadValue" );
		
	if (!Ivi_Simulating(vi))
    {
		ViSession   io = Ivi_IOSession(vi);
		checkErr( Ivi_GetViInt32EntryFromIndex (fetchType, &attrFetchTypeRangeTable, VI_NULL, VI_NULL,
												VI_NULL, &FetchCmd, VI_NULL));
		viCheckErr( viPrintf (io, "FETC:%s\n", FetchCmd));
		Delay (delay_time);
		viCheckErr( viScanf (io, "%Lf", readValue));
		Delay (delay_time);
	}
	else //simulation
		chr62000_Simulate_Meas_Fetch (vi, fetchType, readValue);  

	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
	
}

/*****************************************************************************
 * Function: chr62000_QueryBeeper   
 * Purpose:  This function query the beeper state of the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_QueryBeeper (ViSession vi,
                                        ViChar beeper[])
{
	ViStatus	error = VI_SUCCESS;
	ViBoolean	get_status = VI_TRUE;
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
	if (beeper == VI_NULL)
    	viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Beeper");
	checkErr( Ivi_GetAttributeViBoolean (vi, VI_NULL, CHR62000_ATTR_BEEPER, 0,
											&get_status));
	if (get_status == VI_TRUE)
		Fmt (beeper, "On");
	else if (get_status == VI_FALSE)
		Fmt (beeper, "OFF");
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_QueryOutput   
 * Purpose:  This function query the output state of the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_QueryOutput (ViSession vi,
                                        ViChar output[])
{
	ViStatus	error = VI_SUCCESS;
	ViBoolean	get_status = VI_TRUE;
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
	if (output == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Error Message");

	
	if (!Ivi_Simulating(vi))
	{
		ViSession   io = Ivi_IOSession(vi);
		viCheckErr( viPrintf (io, "CONF:OUTP?\n"));
		Delay (delay_time);
		viCheckErr( viScanf (io, "%s", output)); 
		Delay (delay_time);
	}
	else
	{
		checkErr( Ivi_GetAttributeViBoolean (vi, VI_NULL, CHR62000_ATTR_OUTPUT, 0,
												&get_status));
		if (get_status == VI_TRUE)
			Fmt (output, "On");
		else if (get_status == VI_FALSE)
			Fmt (output, "OFF");
	}
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_QueryFoldback   
 * Purpose:  This function query the foldback state and foldback delay time
 *           of the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_QueryFoldback (ViSession vi,
                                          ViChar foldback[],
                                          ViReal64 *foldbackDelayTime)
{
	ViStatus	error = VI_SUCCESS;
	ViString	Cmd;
	ViInt32		get_status = 0;
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
    if (foldback == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Foldback");
    if (foldbackDelayTime == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Foldback Delay Time");
	checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, CHR62000_ATTR_FOLDBACK_PROTECT, 0,
											&get_status));
											
	checkErr( Ivi_GetViInt32EntryFromIndex (get_status, &attrFoldbackProtectRangeTable, VI_NULL, VI_NULL,
												VI_NULL, &Cmd, VI_NULL));											
	Fmt	(foldback, "%s", Cmd);	
	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_FOLDBACK_DELAY_TIME, IVI_VAL_DIRECT_USER_CALL,
											foldbackDelayTime));
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_QueryAPGMode   
 * Purpose:  This function query the APG state and APG reference voltage
 *           of the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_QueryAPGMode (ViSession vi,
                                         ViChar APGMode[],
                                         ViChar APGReferenceVoltage[])
{
	ViStatus	error = VI_SUCCESS;
	ViString	Cmd;
	ViInt32		get_status = 0;
	ViBoolean	ref_volt = VI_FALSE;
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
    if (APGMode == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for APG Mode");
    if (APGReferenceVoltage == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for APG Reference Voltage");
	checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, CHR62000_ATTR_APG_MODE, 0,
											&get_status));
	checkErr( Ivi_GetViInt32EntryFromIndex (get_status, &attrApgModeRangeTable, VI_NULL, VI_NULL,
												VI_NULL, &Cmd, VI_NULL));											
	Fmt	(APGMode, "%s", Cmd);
	checkErr( Ivi_GetAttributeViBoolean (vi, VI_NULL, CHR62000_ATTR_APG_REFERENCE_VOLTAGE, 0,
											&ref_volt));	
	if (ref_volt == VI_FALSE)
		Fmt	(APGReferenceVoltage, "5V"); 
	else if (ref_volt == VI_TRUE)
		Fmt	(APGReferenceVoltage, "10V"); 		
		
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_QueryProgPara   
 * Purpose:  This function query the whole program parameter of the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_QueryProgPara (ViSession vi,
                                          ViInt32 programSelected,
                                          ViInt32 sequenceSelected,
                                          ViInt32 *link, ViInt32 *count,
                                          ViChar type[], ViReal64 *voltage,
                                          ViReal64 *voltageSlew,
                                          ViReal64 *current,
                                          ViReal64 *currentSlew, ViInt32 *TTL,
                                          ViReal64 *time)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		channel[10];
	ViInt32		value=0;
	
	checkErr( Ivi_LockSession (vi, VI_NULL)); 
	checkErr( Ivi_SetAttributeViInt32 (vi, VI_NULL, CHR62000_ATTR_PROG_SELECTED,
										IVI_VAL_DIRECT_USER_CALL, programSelected));
	checkErr( Ivi_SetAttributeViInt32 (vi, VI_NULL, CHR62000_ATTR_PROG_SEQ_SELECTED,
										IVI_VAL_DIRECT_USER_CALL , sequenceSelected));  
	Fmt (channel,"%d", programSelected); // The channel of the link and count is equal program
	if (link != VI_NULL)
	checkErr( Ivi_GetAttributeViInt32 (vi, channel, CHR62000_ATTR_PROG_LINK, 0,
											link));
	if (count != VI_NULL)
	checkErr( Ivi_GetAttributeViInt32 (vi, channel, CHR62000_ATTR_PROG_COUNT, 0,
											count));											
	Fmt (channel,"%d", (programSelected-1)*10+sequenceSelected); //	calculate channel for specific program and sequence										
	if (type != VI_NULL)									
	{
		checkErr( Ivi_GetAttributeViInt32 (vi, channel, CHR62000_ATTR_PROG_SEQ_TYPE, 0,
												&value));
		if (value == 0)	
			Fmt (type, "AUTO");
		else if (value == 1)	
			Fmt (type, "MANUAL");
		else if (value == 2)	
			Fmt (type, "EXT.TRIGGER");
	}		
	if (voltage != VI_NULL)											
	checkErr( Ivi_GetAttributeViReal64 (vi, channel, CHR62000_ATTR_PROG_SEQ_VOLTAGE, 0,
											voltage));
	if (voltageSlew != VI_NULL)											
	checkErr( Ivi_GetAttributeViReal64 (vi, channel, CHR62000_ATTR_PROG_SEQ_VOLT_SLEW, 0,
											voltageSlew));
	if (current != VI_NULL)												
	checkErr( Ivi_GetAttributeViReal64 (vi, channel, CHR62000_ATTR_PROG_SEQ_CURRENT, 0,
											current));
	if (currentSlew != VI_NULL)											
	checkErr( Ivi_GetAttributeViReal64 (vi, channel, CHR62000_ATTR_PROG_SEQ_CURR_SLEW, 0,
											currentSlew));
	if (TTL != VI_NULL)												
	checkErr( Ivi_GetAttributeViInt32 (vi, channel, CHR62000_ATTR_PROG_SEQ_TTL, 0,
											TTL));
	if (time != VI_NULL)
	{
		if (value == 0)	// if sequence type is auto, then the time is meaningful, else retrun time=-1
			checkErr( Ivi_GetAttributeViReal64 (vi, channel, CHR62000_ATTR_PROG_SEQ_TIME, 0,
													time));
		else
			*time=-1;
	}					   
	checkErr( chr62000_CheckStatus (vi));
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_QueryProgRunState   
 * Purpose:  This function query the program running state of the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_QueryProgRunState (ViSession vi,
                                              ViChar programRunningState[])
{
	ViStatus	error = VI_SUCCESS;
	ViBoolean	get_status = VI_TRUE;
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
    if (programRunningState == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Program Running State");
	if (!Ivi_Simulating(vi))
	{
		ViSession   io = Ivi_IOSession(vi);
		viCheckErr( viPrintf (io, "PROG:RUN?\n"));
		Delay (delay_time);
		viCheckErr( viScanf (io, "%s", programRunningState)); 
		Delay (delay_time);
	}
	else
	{
		checkErr( Ivi_GetAttributeViBoolean (vi, VI_NULL, CHR62000_ATTR_PROG_RUN, 0,
												&get_status));
		if (get_status == VI_TRUE)
			Fmt (programRunningState, "ON");
		else if (get_status == VI_FALSE)
			Fmt (programRunningState, "OFF");
	}
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_QueryESR   
 * Purpose:  This function query the standard event status register of the 
 *           instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_QueryESR (ViSession vi,
                                     ViInt32 *statusRegister)
{
	ViStatus	error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
    if (statusRegister == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Status Register");
	if (!Ivi_Simulating(vi))
	{
		ViSession   io = Ivi_IOSession(vi);
		viCheckErr( viPrintf (io, "*ESR?\n"));
		Delay (delay_time);
		viCheckErr( viScanf (io, "%d", statusRegister)); 
		Delay (delay_time);
	}
	else
		*statusRegister=0;

	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_QuerySTB   
 * Purpose:  This function query the read status byte of the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_QuerySTB (ViSession vi,
                                     ViInt32 *statusByte)
{
	ViStatus	error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
    if (statusByte == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Status Byte");
	if (!Ivi_Simulating(vi))
	{
		ViSession   io = Ivi_IOSession(vi);
		viCheckErr( viPrintf (io, "*STB?\n"));
		Delay (delay_time);
		viCheckErr( viScanf (io, "%d", statusByte)); 
		Delay (delay_time);
	}
	else
		*statusByte=0;

	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_reset                                                         
 * Purpose:  This function resets the instrument.                          
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_reset (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	    {
	    ViSession   io = Ivi_IOSession(vi); /* call only when locked */
	    
	    checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
	    viCheckErr( viPrintf (io, "*RST"));
	    Delay (1);
	    }
	
	checkErr( chr62000_DefaultInstrSetup (vi));                                
	
	checkErr( chr62000_CheckStatus (vi));                                      

Error:
    Ivi_UnlockSession (vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: chr62000_self_test                                                       
 * Purpose:  This function executes the instrument self-test and returns the 
 *           result.                                                         
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_self_test (ViSession vi, ViInt16 *testResult, 
                                      ViChar testMessage[])
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (testResult == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Test Result");
    if (testMessage == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Test Message");

	viCheckWarn( VI_WARN_NSUP_SELF_TEST);

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: chr62000_error_query                                                     
 * Purpose:  This function queries the instrument error queue and returns   
 *           the result.                                                     
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_error_query (ViSession vi, ViInt32 *errCode, 
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
	
		viCheckErr( viScanf (io, "%ld,\"%256[^\"]", errCode, errMessage));
	
	    }
	else
	    {
	        /* Simulate Error Query */
	    *errCode = 0;
	    strcpy (errMessage, "No error");
	    }

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: chr62000_error_message                                                  
 * Purpose:  This function translates the error codes returned by this       
 *           instrument driver into user-readable strings.  
 *
 *           Note:  The caller can pass VI_NULL for the vi parameter.  This 
 *           is useful if one of the init functions fail.
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_error_message (ViSession vi, ViStatus errorCode,
                                          ViChar errorMessage[256])
{
    ViStatus    error = VI_SUCCESS;
    
    static      IviStringValueTable errorTable = 
        {
			{CHR62000_ERROR_OUT_OF_RANGE,		"The value you applied is out of range."},         
			{CHR62000_ERROR_OUT_OF_VOLT_LIMIT,	"The value you applied is out of range of voltage limit."},
			{CHR62000_ERROR_OUT_OF_CURR_LIMIT,	"The value you applied is out of range of current limit."},
			{CHR62000_ERROR_OUT_OF_OVP,			"The value you applied is less than 1.1*(voltage setting)."}, 
			{CHR62000_ERROR_OUT_OF_OCP,			"The value you applied is less than 1.05*(current setting)."},
			{CHR62000_ERROR_OUT_OF_MAXPOWER,	"The value you applied is greater than max. power."}, 
			{CHR62000_ERROR_INVALID_INTERFACE,	"Invalid interface(RS232C only)."},
			{CHR62000_ERROR_INVALID_PARAMETER,  "Invalid parameter value."},
			{CHR62000_ERROR_EXECUTION,			"Execution error! Use Error-Query function to detect error."},
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
 * Function: chr62000_revision_query                                                  
 * Purpose:  This function returns the driver and instrument revisions.      
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_revision_query (ViSession vi, ViChar driverRev[], 
                                           ViChar instrRev[])
{
    ViStatus    error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (driverRev == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Driver Revision");
    if (instrRev == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Instrument Revision");

    checkErr( Ivi_GetAttributeViString (vi, VI_NULL, CHR62000_ATTR_SPECIFIC_DRIVER_REVISION, 
                                        0, 256, driverRev));
    checkErr( Ivi_GetAttributeViString (vi, "", CHR62000_ATTR_INSTRUMENT_FIRMWARE_REVISION, 
                                        0, 256, instrRev));
    checkErr( chr62000_CheckStatus (vi));

Error:    
    Ivi_UnlockSession(vi, VI_NULL);
    return error;							  
}

/*****************************************************************************
 * Function: chr62000_GetAttribute<type> Functions                                    
 * Purpose:  These functions enable the instrument driver user to get 
 *           attribute values directly.  There are typesafe versions for 
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession attributes.                                         
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_GetAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViInt32 *value)
{                                                                                                           
    return Ivi_GetAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                    value);
}                                                                                                           
ViStatus _VI_FUNC chr62000_GetAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViReal64 *value)
{                                                                                                           
    return Ivi_GetAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}                                                                                                           
ViStatus _VI_FUNC chr62000_GetAttributeViString (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViInt32 bufSize, 
                                                 ViChar value[]) 
{   
    return Ivi_GetAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     bufSize, value);
}   
ViStatus _VI_FUNC chr62000_GetAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViBoolean *value)
{                                                                                                           
    return Ivi_GetAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           
ViStatus _VI_FUNC chr62000_GetAttributeViSession (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViSession *value)
{                                                                                                           
    return Ivi_GetAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           

/*****************************************************************************
 * Function: chr62000_SetVoltage                                       
 * Purpose:  This function set the voltage to the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SetVoltage (ViSession vi,
                                       ViReal64 voltage)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	volt_limit_high=0,volt_limit_low=0,curr_main=0,MaxPower=0;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_VOLT_LIMIT_HIGH, IVI_VAL_DIRECT_USER_CALL,
											&volt_limit_high));
	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_VOLT_LIMIT_LOW, IVI_VAL_DIRECT_USER_CALL,
											&volt_limit_low));
	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_CURRENT, IVI_VAL_DIRECT_USER_CALL,
											&curr_main));											
	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_MAXPOWER, 0, &MaxPower));
	
	// if voltage > volt_limit_high or voltage < volt_limit_low, then error
	if ( ((voltage-volt_limit_high) > accuracy) || ((volt_limit_low-voltage) > accuracy) )
		checkErr( error= CHR62000_ERROR_OUT_OF_VOLT_LIMIT );
	/*else if ( (curr_main*voltage-MaxPower) > accuracy) 	// if curr_main*voltage setting bigger than MaxPower, then error 	
		checkErr( error= CHR62000_ERROR_OUT_OF_MAXPOWER );*/
		else	// the voltage setting is in voltage limit, and in MaxPower range, then write voltage setting to instrument	
			viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_VOLTAGE, IVI_VAL_DIRECT_USER_CALL,
													voltage), 2, "Voltage");
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_SetVoltageLimit                                       
 * Purpose:  This function set the voltage high/low limit to the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SetVoltageLimit (ViSession vi,
                                            ViBoolean highLow,
                                            ViReal64 voltageLimit)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	volt_main=0,volt_limit_high=0,volt_limit_low=0;
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
	
	if ( highLow!=VI_TRUE && highLow!=VI_FALSE )
		checkErr( error=CHR62000_ERROR_INVALID_PARAMETER );
		
	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_VOLTAGE, IVI_VAL_DIRECT_USER_CALL,
											&volt_main));
	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_VOLT_LIMIT_HIGH, IVI_VAL_DIRECT_USER_CALL,
											&volt_limit_high));
	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_VOLT_LIMIT_LOW, IVI_VAL_DIRECT_USER_CALL,
											&volt_limit_low));											
											
	if (highLow == VI_TRUE)
	{
		if ( (volt_limit_low-voltageLimit) > accuracy )
			checkErr( error= CHR62000_ERROR_INVALID_PARAMETER );
		else
		{
			viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_VOLT_LIMIT_HIGH, IVI_VAL_DIRECT_USER_CALL,
													voltageLimit), 3, "Voltage Limit");
			if ( (volt_main-voltageLimit) > accuracy )	
				checkErr( Ivi_InvalidateAttribute (vi, VI_NULL, CHR62000_ATTR_VOLTAGE));
		}											
	}	
	
	else if (highLow == VI_FALSE)
	{
		if ( (voltageLimit-volt_limit_high) > accuracy )
			checkErr( error= CHR62000_ERROR_INVALID_PARAMETER );
		else
		{
			viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_VOLT_LIMIT_LOW, IVI_VAL_DIRECT_USER_CALL,
													voltageLimit), 3, "Voltage Limit");
			if ( (voltageLimit-volt_main) > accuracy )
				checkErr( Ivi_InvalidateAttribute (vi, VI_NULL, CHR62000_ATTR_VOLTAGE));
		}
	}	
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_SetVoltageProtect                                       
 * Purpose:  This function set the protect voltage to the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SetVoltageProtect (ViSession vi,
                                              ViReal64 voltageProtect)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	volt_main=0,min_volt_protect=0;
	ViInt32		value=0;
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_VOLTAGE, IVI_VAL_DIRECT_USER_CALL,
											&volt_main));
	value=(ViInt32)(volt_main*OVP_RATIO*1000); //the protect voltage can not less than OVP_RATIO*voltage setting
	if ( (value%100) !=0)
    {
    	value=value+100-(value%100);
    	min_volt_protect=(((ViReal64)(value))/1000);
    }
    else
    	min_volt_protect=(((ViReal64)(value))/1000);
	
	if ( (min_volt_protect-voltageProtect) > accuracy)
		checkErr( error= CHR62000_ERROR_OUT_OF_OVP );
	else		
		viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_VOLT_PROTECT, IVI_VAL_DIRECT_USER_CALL,
												voltageProtect), 2, "Voltage Protect");
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_SetVoltageSlew                                       
 * Purpose:  This function set the voltage slew rate to the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SetVoltageSlew (ViSession vi,
                                           ViReal64 voltageSlew)
{
	ViStatus	error = VI_SUCCESS;
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
	viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_VOLT_SLEW, IVI_VAL_DIRECT_USER_CALL,
											voltageSlew), 2, "Voltage Slew");
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_SetCurrent                                       
 * Purpose:  This function set the current to the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SetCurrent (ViSession vi,
                                       ViReal64 current)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	curr_limit_high=0,curr_limit_low=0,volt_main=0,MaxPower=0;
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_CURR_LIMIT_HIGH, IVI_VAL_DIRECT_USER_CALL,
											&curr_limit_high));
	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_CURR_LIMIT_LOW, IVI_VAL_DIRECT_USER_CALL,
											&curr_limit_low));
	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_VOLTAGE, IVI_VAL_DIRECT_USER_CALL,
											&volt_main));
	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_MAXPOWER, 0, &MaxPower)); 											
		
	// if current > curr_limit_high or current < curr_limit_low, then error
	if ( ((current-curr_limit_high) > accuracy) || ((curr_limit_low-current) > accuracy) )
		checkErr( error= CHR62000_ERROR_OUT_OF_CURR_LIMIT );											
	/*else if ( (volt_main*current-MaxPower) > accuracy) 	// if volt_main*current setting bigger than MaxPower, then error 	
		checkErr( error= CHR62000_ERROR_OUT_OF_MAXPOWER );*/
		else	// the current setting is in current limit, and in MaxPower range, then write current setting to instrument
			viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_CURRENT, IVI_VAL_DIRECT_USER_CALL,
													current), 2, "Current");
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_SetCurrentLimit                                       
 * Purpose:  This function set the current high/low limit to the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SetCurrentLimit (ViSession vi,
                                            ViBoolean highLow,
                                            ViReal64 currentLimit)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	curr_main=0,curr_limit_high=0,curr_limit_low=0;  	
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
	
	if ( highLow!=VI_TRUE && highLow!=VI_FALSE )
		checkErr( error=CHR62000_ERROR_INVALID_PARAMETER );
	
	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_CURRENT, IVI_VAL_DIRECT_USER_CALL,
											&curr_main));
	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_CURR_LIMIT_HIGH, IVI_VAL_DIRECT_USER_CALL,
											&curr_limit_high));
	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_CURR_LIMIT_LOW, IVI_VAL_DIRECT_USER_CALL,
											&curr_limit_low));
	
	if (highLow == VI_TRUE)
	{
		if ( (curr_limit_low-currentLimit) > accuracy )
			checkErr( error= CHR62000_ERROR_INVALID_PARAMETER );
		else
		{
			viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_CURR_LIMIT_HIGH, IVI_VAL_DIRECT_USER_CALL,
													currentLimit), 3, "Current Limit");
			if ( (curr_main-currentLimit) > accuracy )	
				checkErr( Ivi_InvalidateAttribute (vi, VI_NULL, CHR62000_ATTR_CURRENT));
		}													
	}												
	
	else if (highLow == VI_FALSE)
	{
		if ( (currentLimit-curr_limit_high) > accuracy )
			checkErr( error= CHR62000_ERROR_INVALID_PARAMETER );
		else
		{
			viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_CURR_LIMIT_LOW, IVI_VAL_DIRECT_USER_CALL,
													currentLimit), 3, "Current Limit");
			if ( (currentLimit-curr_main) > accuracy )	
				checkErr( Ivi_InvalidateAttribute (vi, VI_NULL, CHR62000_ATTR_CURRENT));													
		}														
	}											
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_SetCurrentProtect                                       
 * Purpose:  This function set the current protect to the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SetCurrentProtect (ViSession vi,
                                              ViReal64 currentProtect)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	curr_main=0,min_curr_protect=0;
	ViInt32		value=0;
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_CURRENT, IVI_VAL_DIRECT_USER_CALL,
											&curr_main));
	value=(ViInt32)(curr_main*OCP_RATIO*1000); //the protect current can not less than OCP_RATIO*current setting
	if ( (value%100) !=0)
    {
    	value=value+100-(value%100);
    	min_curr_protect=(((ViReal64)(value))/1000);
    }
    else
    	min_curr_protect=(((ViReal64)(value))/1000);
	
	if ( (min_curr_protect-currentProtect) > accuracy)
		checkErr( error= CHR62000_ERROR_OUT_OF_OCP );
	else													
		viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_CURR_PROTECT, IVI_VAL_DIRECT_USER_CALL,
												currentProtect), 2, "Current Protect");
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_SetCurrentSlew                                       
 * Purpose:  This function set the current slew rate to the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SetCurrentSlew (ViSession vi,
                                           ViReal64 currentSlew)
{
	ViStatus	error = VI_SUCCESS;
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
	viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_CURR_SLEW, IVI_VAL_DIRECT_USER_CALL,
											currentSlew), 2, "Current Slew");
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_SetPowerProtect                                       
 * Purpose:  This function set the power protect to the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SetPowerProtect (ViSession vi,
                                            ViReal64 powerProtect)
{
	ViStatus	error = VI_SUCCESS;
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
	viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_POWER_PROTECT, IVI_VAL_DIRECT_USER_CALL,
											powerProtect), 2, "Power Protect");
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_SetOutput                                       
 * Purpose:  This function enable the output to the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SetOutput (ViSession vi,
                                      ViBoolean output)
{
	ViStatus	error = VI_SUCCESS;
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
	
	if ( output!=VI_TRUE && output!=VI_FALSE )
		checkErr( error=CHR62000_ERROR_INVALID_PARAMETER );
	
	viCheckParm( Ivi_SetAttributeViBoolean (vi, VI_NULL, CHR62000_ATTR_OUTPUT, 0,
											output), 2, "Output");
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_SetTTLPort                                       
 * Purpose:  This function set the TTL value to the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SetTTLPort (ViSession vi,
                                       ViInt32 TTLValue)
{
	ViStatus	error = VI_SUCCESS;
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
	viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, CHR62000_ATTR_TTL_PORT, IVI_VAL_DIRECT_USER_CALL,
											TTLValue), 2, "TTL Value");
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_SetBeeper                                       
 * Purpose:  This function set the beeper state to the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SetBeeper (ViSession vi,
                                      ViBoolean beeper)
{
	ViStatus	error = VI_SUCCESS;
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
	
	if ( beeper!=VI_TRUE && beeper!=VI_FALSE )
		checkErr( error=CHR62000_ERROR_INVALID_PARAMETER );
		
	viCheckParm( Ivi_SetAttributeViBoolean (vi, VI_NULL, CHR62000_ATTR_BEEPER, 0,
											beeper), 2, "Beeper");
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_SetRmtMode                                       
 * Purpose:  This function set the remote state to the instrument (RS232C only)
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SetRmtMode (ViSession vi,
                                       ViBoolean remoteMode)
{
	ViStatus	error = VI_SUCCESS;
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
	
	if ( remoteMode!=VI_TRUE && remoteMode!=VI_FALSE )
		checkErr( error=CHR62000_ERROR_INVALID_PARAMETER );
		
	viCheckParm( Ivi_SetAttributeViBoolean (vi, VI_NULL, CHR62000_ATTR_REMOTE_MODE, 0,
													remoteMode), 2, "Remote Mode");
	
	if ( remoteMode == VI_FALSE)
		checkErr( Ivi_InvalidateAllAttributes (vi)); 														
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_SetFoldbackProtect                                       
 * Purpose:  This function set the foldback state and foldback delay time 
 *           to the instrument 
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SetFoldbackProtect (ViSession vi,
                                               ViInt32 foldbackProtect,
                                               ViReal64 foldbackDelayTime)
{
	ViStatus	error = VI_SUCCESS;
	
	checkErr( Ivi_LockSession (vi, VI_NULL));

	viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, CHR62000_ATTR_FOLDBACK_PROTECT, 0,
											foldbackProtect), 2, "Foldback Protect");
	viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_FOLDBACK_DELAY_TIME, IVI_VAL_DIRECT_USER_CALL,
											foldbackDelayTime), 3, "Foldback Delay Time");											
												
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_SetAPGMode   
 * Purpose:  This function set the APG state and APG reference voltage
 *           to the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SetAPGMode (ViSession vi,
                                       ViInt32 APGMode,
                                       ViBoolean APGReferenceVoltage)
{
	ViStatus	error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	if ( APGReferenceVoltage!=VI_TRUE && APGReferenceVoltage!=VI_FALSE )
		checkErr( error=CHR62000_ERROR_INVALID_PARAMETER );	
			
	viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, CHR62000_ATTR_APG_MODE, 0,
											APGMode), 2, "APG Mode");
	viCheckParm( Ivi_SetAttributeViBoolean (vi, VI_NULL, CHR62000_ATTR_APG_REFERENCE_VOLTAGE, 0,
											APGReferenceVoltage), 2, "APG Reference Voltage");											
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_ProgSel   
 * Purpose:  This function set the program No., link and count to the
 *           instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_ProgSel (ViSession vi,
                                    ViInt32 programSelected, ViInt32 link,
                                    ViInt32 count, ViBoolean programClear)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		channel[10];
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
	
	if ( programClear!=VI_TRUE && programClear!=VI_FALSE )
		checkErr( error=CHR62000_ERROR_INVALID_PARAMETER );
		
	viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, CHR62000_ATTR_PROG_SELECTED, IVI_VAL_DIRECT_USER_CALL,
										programSelected), 2, "Program Selected");
										
	if (programClear == VI_TRUE)											
	{
		if (!Ivi_Simulating(vi))
		{
			ViSession   io = Ivi_IOSession(vi);
			viCheckErr( viPrintf (io, "PROG:CLEAR\n")); 
			Delay (0.05);
		}	
		checkErr( chr62000_Invalidate_Program (vi, programSelected));
	}
	else
	{
		Fmt (channel, "%d", programSelected);
		viCheckParm( Ivi_SetAttributeViInt32 (vi, channel, CHR62000_ATTR_PROG_LINK, 0,
												link), 3, "Link");
		viCheckParm( Ivi_SetAttributeViInt32 (vi, channel, CHR62000_ATTR_PROG_COUNT, 0,
												count), 4, "Count");
	}
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_SetSeqPara   
 * Purpose:  This function set the sequence parameter to the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SetSeqPara (ViSession vi,
                                       ViInt32 sequenceSelected, ViInt32 type,
                                       ViReal64 voltage, ViReal64 voltageSlew,
                                       ViReal64 current, ViReal64 currentSlew,
                                       ViInt32 TTL, ViReal64 time)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		channel[10]; 
	ViInt32		programSelected=0,ex_type=8;
	ViReal64	volt_limit_high=0,volt_limit_low=0,curr_limit_high=0,curr_limit_low=0; 
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
	
	if ( type<0  || type>2 )
		checkErr( error=CHR62000_ERROR_INVALID_PARAMETER );
		
	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_VOLT_LIMIT_HIGH, IVI_VAL_DIRECT_USER_CALL,
											&volt_limit_high));
	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_VOLT_LIMIT_LOW, IVI_VAL_DIRECT_USER_CALL,
											&volt_limit_low));
	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_CURR_LIMIT_HIGH, IVI_VAL_DIRECT_USER_CALL,
											&curr_limit_high));
	checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, CHR62000_ATTR_CURR_LIMIT_LOW, IVI_VAL_DIRECT_USER_CALL,
											&curr_limit_low));											
	checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, CHR62000_ATTR_PROG_SELECTED,
										IVI_VAL_DIRECT_USER_CALL, &programSelected));
	checkErr( Ivi_SetAttributeViInt32 (vi, VI_NULL, CHR62000_ATTR_PROG_SEQ_SELECTED,
										IVI_VAL_DIRECT_USER_CALL, sequenceSelected));											
	Fmt (channel,"%d", (programSelected-1)*10+sequenceSelected); //	calculate channel for specific program and sequence
	
	// if voltage > volt_limit_high or voltage < volt_limit_low, then error
	if ( ((voltage-volt_limit_high) > accuracy) || ((volt_limit_low-voltage) > accuracy) )
		checkErr( error= CHR62000_ERROR_OUT_OF_VOLT_LIMIT );
	else													
		viCheckParm( Ivi_SetAttributeViReal64 (vi, channel, CHR62000_ATTR_PROG_SEQ_VOLTAGE, 0,
												voltage), 4, "Voltage");											
	// if current > curr_limit_high or current < curr_limit_low, then error
	if ( ((current-curr_limit_high) > accuracy) || ((curr_limit_low-current) > accuracy) )
		checkErr( error= CHR62000_ERROR_OUT_OF_CURR_LIMIT );											
	else													
		viCheckParm( Ivi_SetAttributeViReal64 (vi, channel, CHR62000_ATTR_PROG_SEQ_CURRENT, 0,
												current), 6, "Current");	
												
	viCheckParm( Ivi_SetAttributeViReal64 (vi, channel, CHR62000_ATTR_PROG_SEQ_VOLT_SLEW, 0,
											voltageSlew), 5, "Voltage Slew");											
	viCheckParm( Ivi_SetAttributeViReal64 (vi, channel, CHR62000_ATTR_PROG_SEQ_CURR_SLEW, 0,
											currentSlew), 7, "Current Slew");
	viCheckParm( Ivi_SetAttributeViInt32 (vi, channel, CHR62000_ATTR_PROG_SEQ_TTL, 0,
											TTL), 8, "TTL");
	if ( type == 0 )  // only when type=AUTO, time will write into the attribute
		viCheckParm( Ivi_SetAttributeViReal64 (vi, channel, CHR62000_ATTR_PROG_SEQ_TIME, 0,
												time), 9, "Time");		
	// get ex_type to compare, if ex_type!=type, then invalidate CHR62000_ATTR_PROG_SEQ_TIME
	checkErr( Ivi_GetAttributeViInt32 (vi, channel, CHR62000_ATTR_PROG_SEQ_TYPE, 
                                           0, &ex_type));												
	viCheckParm( Ivi_SetAttributeViInt32 (vi, channel, CHR62000_ATTR_PROG_SEQ_TYPE, 0,
											type), 3, "Type");
	if ( type != ex_type)										
		checkErr( Ivi_InvalidateAttribute (vi, channel, CHR62000_ATTR_PROG_SEQ_TIME));											

	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_ProgRun   
 * Purpose:  This function set the program running state to the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_ProgRun (ViSession vi, ViBoolean run)
{
	ViStatus	error = VI_SUCCESS;
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
	
	if ( run!=VI_TRUE && run!=VI_FALSE )
		checkErr( error=CHR62000_ERROR_INVALID_PARAMETER );
		
	viCheckParm( Ivi_SetAttributeViBoolean (vi, VI_NULL, CHR62000_ATTR_PROG_RUN, 0,
											run), 2, "Run");
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_SaveProgram   
 * Purpose:  This function save the program parameter to the instrument
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SaveProgram (ViSession vi)
{
	ViStatus	error = VI_SUCCESS;
	
	if (!Ivi_Simulating(vi))
	{
		ViSession   io = Ivi_IOSession(vi);
		viCheckErr( viWrite (io, "PROG:SAVE", 9, VI_NULL));		
		Delay (1); 
	}

Error:
	return error;
}
				   
/*****************************************************************************
 * Function: chr62000_SetCLS   
 * Purpose:  This function clear the instrument status
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SetCLS (ViSession vi)
{
	ViStatus	error = VI_SUCCESS;
	
	if (!Ivi_Simulating(vi))
	{
		ViSession   io = Ivi_IOSession(vi);
		viCheckErr( viWrite (io, "*CLS", 4, VI_NULL));		
		Delay (delay_time); 
	}

Error:
	return error;
}

/*****************************************************************************
 * Function: chr62000_SetESE   
 * Purpose:  This function set the standard event status enable register to 
 *           the instrument 
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SetESE (ViSession vi,
                                   ViInt32 enableRegister)		 
{
	ViStatus	error = VI_SUCCESS;
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
	viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, CHR62000_ATTR_STATUS_ESE, IVI_VAL_DIRECT_USER_CALL,
											enableRegister), 2, "Enable Register");
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_SetSRE   
 * Purpose:  This function set service request enable register to the 
 *           instrument 
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SetSRE (ViSession vi,
                                   ViInt32 enableRegister)
{
	ViStatus	error = VI_SUCCESS;
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
	viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, CHR62000_ATTR_STATUS_SRE, IVI_VAL_DIRECT_USER_CALL,
											enableRegister), 2, "Enable Register");
	checkErr( chr62000_CheckStatus (vi)); 
	
Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr62000_Save   
 * Purpose:  This function store the whole parameter to the instrument memory
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_Save (ViSession vi)
{
	ViStatus	error = VI_SUCCESS;
	
	if (!Ivi_Simulating(vi))
	{
		ViSession   io = Ivi_IOSession(vi);
		viCheckErr( viWrite (io, "*SAV", 4, VI_NULL));		
		Delay (1); 
	}

Error:
	return error;
}

/*****************************************************************************
 * Function: chr62000_Recall   
 * Purpose:  This function recall the whole parameter from the instrument 
 *           memory
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_Recall (ViSession vi)
{
	ViStatus	error = VI_SUCCESS;
	
	if (!Ivi_Simulating(vi))
	{
		ViSession   io = Ivi_IOSession(vi);
		viCheckErr( viWrite (io, "*RCL 1", 6, VI_NULL));		
		Delay (1); 
		checkErr( Ivi_InvalidateAllAttributes (vi));
	}

Error:
	return error;
}

/*****************************************************************************
 * Function: chr62000_SetAttribute<type> Functions                                    
 * Purpose:  These functions enable the instrument driver user to set 
 *           attribute values directly.  There are typesafe versions for 
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession datatypes.                                         
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_SetAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViInt32 value)
{                                                                                                           
    return Ivi_SetAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                    value);
}                                                                                                           
ViStatus _VI_FUNC chr62000_SetAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViReal64 value)
{                                                                                                           
    return Ivi_SetAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}                                                                                                           
ViStatus _VI_FUNC chr62000_SetAttributeViString (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViConstString value) 
{   
    return Ivi_SetAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}   
ViStatus _VI_FUNC chr62000_SetAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViBoolean value)
{                                                                                                           
    return Ivi_SetAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           
ViStatus _VI_FUNC chr62000_SetAttributeViSession (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViSession value)
{                                                                                                           
    return Ivi_SetAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           

/*****************************************************************************
 * Function: chr62000_CheckAttribute<type> Functions                                  
 * Purpose:  These functions enable the instrument driver user to check  
 *           attribute values directly.  These functions check the value you
 *           specify even if you set the CHR62000_ATTR_RANGE_CHECK 
 *           attribute to VI_FALSE.  There are typesafe versions for ViInt32, 
 *           ViReal64, ViString, ViBoolean, and ViSession datatypes.                         
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_CheckAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViInt32 value)
{                                                                                                           
    return Ivi_CheckAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}
ViStatus _VI_FUNC chr62000_CheckAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                   ViAttr attributeId, ViReal64 value)
{                                                                                                           
    return Ivi_CheckAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                       value);
}                                                                                                           
ViStatus _VI_FUNC chr62000_CheckAttributeViString (ViSession vi, ViConstString channelName, 
                                                   ViAttr attributeId, ViConstString value)  
{   
    return Ivi_CheckAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                       value);
}   
ViStatus _VI_FUNC chr62000_CheckAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                    ViAttr attributeId, ViBoolean value)
{                                                                                                           
    return Ivi_CheckAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                        value);
}                                                                                                           
ViStatus _VI_FUNC chr62000_CheckAttributeViSession (ViSession vi, ViConstString channelName, 
                                                    ViAttr attributeId, ViSession value)
{                                                                                                           
    return Ivi_CheckAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                        value);
}                                                                                                           

/*****************************************************************************
 * Function: chr62000_GetNextCoercionRecord                        
 * Purpose:  This function enables the instrument driver user to obtain
 *           the coercion information associated with the IVI session.                                                              
 *           This function retrieves and clears the oldest instance in which 
 *           the instrument driver coerced a value the instrument driver user
 *           specified to another value.                     
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_GetNextCoercionRecord (ViSession vi,
                                                  ViInt32 bufferSize,
                                                  ViChar  record[])
{
    return Ivi_GetNextCoercionString (vi, bufferSize, record);
}

/*****************************************************************************
 * Function: chr62000_LockSession and chr62000_UnlockSession Functions                        
 * Purpose:  These functions enable the instrument driver user to lock the 
 *           session around a sequence of driver calls during which other
 *           execution threads must not disturb the instrument state.
 *                                                                          
 *           NOTE:  The callerHasLock parameter must be a local variable 
 *           initialized to VI_FALSE and passed by reference, or you can pass 
 *           VI_NULL.                     
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_LockSession (ViSession vi, ViBoolean *callerHasLock)  
{                                              
    return Ivi_LockSession(vi,callerHasLock);      
}                                              
ViStatus _VI_FUNC chr62000_UnlockSession (ViSession vi, ViBoolean *callerHasLock) 
{                                              
    return Ivi_UnlockSession(vi,callerHasLock);    
}   

/*****************************************************************************
 * Function: chr62000_GetErrorInfo and chr62000_ClearErrorInfo Functions                       
 * Purpose:  These functions enable the instrument driver user to  
 *           get or clear the error information the driver associates with the
 *           IVI session.                                                        
 *****************************************************************************/
ViStatus _VI_FUNC chr62000_GetErrorInfo (ViSession vi, ViStatus *primaryError, 
                                         ViStatus *secondaryError, ViChar errorElaboration[256])  
{                                                                                                           
    return Ivi_GetErrorInfo(vi, primaryError, secondaryError, errorElaboration);                                
}                                                                                                           
ViStatus _VI_FUNC chr62000_ClearErrorInfo (ViSession vi)                                                        
{                                                                                                           
    return Ivi_ClearErrorInfo (vi);                                                                             
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
ViStatus _VI_FUNC chr62000_WriteInstrData (ViSession vi, ViConstString writeBuffer)   
{   
    return Ivi_WriteInstrData (vi, writeBuffer);    
}   
ViStatus _VI_FUNC chr62000_ReadInstrData (ViSession vi, ViInt32 numBytes, 
                                          ViChar rdBuf[], ViInt32 *bytesRead)  
{   
    return Ivi_ReadInstrData (vi, numBytes, rdBuf, bytesRead);   
} 

/*****************************************************************************
 *-------------------- Utility Functions (Not Exported) ---------------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: chr62000_CheckStatus                                                 
 * Purpose:  This function checks the status of the instrument to detect 
 *           whether the instrument has encountered an error.  This function  
 *           is called at the end of most exported driver functions.  If the    
 *           instrument reports an error, this function returns      
 *           IVI_ERROR_INSTR_SPECIFIC.  The user can set the 
 *           IVI_ATTR_QUERY_INSTR_STATUS attribute to VI_FALSE to disable this 
 *           check and increase execution speed.                                   
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus chr62000_CheckStatus (ViSession vi)		  
{
    ViStatus    error = VI_SUCCESS;

    if (Ivi_QueryInstrStatus (vi) && Ivi_NeedToCheckStatus (vi) && !Ivi_Simulating (vi))
        {
        checkErr( chr62000_CheckStatusCallback (vi, Ivi_IOSession(vi)));
        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_FALSE));
        }
        
Error:
    return error;
}

/*****************************************************************************
 * Function: chr62000_WaitForOPC                                                  
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
static ViStatus chr62000_WaitForOPC (ViSession vi, ViInt32 maxTime) 
{
    ViStatus    error = VI_SUCCESS;

    if (!Ivi_Simulating(vi))
        {
        ViInt32  oldOPCTimeout; 
        
        checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, CHR62000_ATTR_OPC_TIMEOUT, 
                                           0, &oldOPCTimeout));
        Ivi_SetAttributeViInt32 (vi, VI_NULL, CHR62000_ATTR_OPC_TIMEOUT,        
                                 0, maxTime);

        error = chr62000_WaitForOPCCallback (vi, Ivi_IOSession(vi));

        Ivi_SetAttributeViInt32 (vi, VI_NULL, CHR62000_ATTR_OPC_TIMEOUT, 
                                 0, oldOPCTimeout);
        viCheckErr( error);
        }
Error:
    return error;
}

/*****************************************************************************
 * Function: chr62000_DefaultInstrSetup                                               
 * Purpose:  This function sends a default setup to the instrument.  The    
 *           chr62000_reset function calls this function.  The 
 *           chr62000_IviInit function calls this function when the
 *           user passes VI_FALSE for the reset parameter.  This function is 
 *           useful for configuring settings that other instrument driver 
 *           functions require.    
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus chr62000_DefaultInstrSetup (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;
        
    /* Invalidate all attributes */
    checkErr( Ivi_InvalidateAllAttributes (vi));
    
    if (!Ivi_Simulating(vi))
        {
        ViSession   io = Ivi_IOSession(vi); /* call only when locked */

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
		
		viCheckErr( viPrintf (io, "*CLS\n"));
		
		// viCheckErr( viPrintf (io, ":HEADERS OFF"));
        }
Error:
    return error;
}

/*****************************************************************************
 * Function: ReadToFile and WriteFromFile Functions                          
 * Purpose:  Functions for instrument driver developers to read/write        
 *           instrument data to/from a file.                                 
 *****************************************************************************/
static ViStatus chr62000_ReadToFile (ViSession vi, ViConstString filename, 
                                     ViInt32 maxBytesToRead, ViInt32 fileAction, 
                                     ViInt32 *totalBytesWritten)  
{   
    return Ivi_ReadToFile (vi, filename, maxBytesToRead, fileAction, totalBytesWritten);  
}   
static ViStatus chr62000_WriteFromFile (ViSession vi, ViConstString filename, 
                                        ViInt32 maxBytesToWrite, ViInt32 byteOffset, 
                                        ViInt32 *totalBytesWritten) 
{   
    return Ivi_WriteFromFile (vi, filename, maxBytesToWrite, byteOffset, totalBytesWritten); 
}

/*****************************************************************************
 *------------------------ Global Session Callbacks -------------------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: chr62000_CheckStatusCallback                                               
 * Purpose:  This function queries the instrument to determine if it has 
 *           encountered an error.  If the instrument has encountered an 
 *           error, this function returns the IVI_ERROR_INSTRUMENT_SPECIFIC 
 *           error code.  This function is called by the 
 *           chr62000_CheckStatus utility function.  The 
 *           Ivi_SetAttribute and Ivi_GetAttribute functions invoke this 
 *           function when the optionFlags parameter includes the
 *           IVI_VAL_DIRECT_USER_CALL flag.
 *           
 *           The user can disable calls to this function by setting the 
 *           IVI_ATTR_QUERY_INSTR_STATUS attribute to VI_FALSE.  The driver can 
 *           disable the check status callback for a particular attribute by 
 *           setting the IVI_VAL_DONT_CHECK_STATUS flag.
 *****************************************************************************/
static ViStatus _VI_FUNC chr62000_CheckStatusCallback (ViSession vi, ViSession io)
{
    ViStatus    error = VI_SUCCESS;
	
	
	/* Query instrument status */
            
Error:
    return error;
}

/*****************************************************************************
 * Function: chr62000_WaitForOPCCallback                                               
 * Purpose:  This function waits until the instrument has finished processing 
 *           all pending operations.  This function is called by the 
 *           chr62000_WaitForOPC utility function.  The IVI engine invokes
 *           this function in the following two cases:
 *           - Before invoking the read callback for attributes for which the 
 *             IVI_VAL_WAIT_FOR_OPC_BEFORE_READS flag is set.
 *           - After invoking the write callback for attributes for which the 
 *             IVI_VAL_WAIT_FOR_OPC_AFTER_WRITES flag is set.
 *****************************************************************************/
static ViStatus _VI_FUNC chr62000_WaitForOPCCallback (ViSession vi, ViSession io)
{
    ViStatus    error = VI_SUCCESS;

    return error;
}

/*****************************************************************************
 *----------------- Attribute Range Tables and Callbacks --------------------*
 *****************************************************************************/

/*- CHR62000_ATTR_ID_QUERY_RESPONSE -*/

static ViStatus _VI_FUNC chr62000AttrIdQueryResponse_ReadCallback (ViSession vi, 
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
    
/*- CHR62000_ATTR_SPECIFIC_DRIVER_REVISION -*/

static ViStatus _VI_FUNC chr62000AttrDriverRevision_ReadCallback (ViSession vi, 
                                                                  ViSession io,
                                                                  ViConstString channelName, 
                                                                  ViAttr attributeId,
                                                                  const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      driverRevision[256];
    
    
    sprintf (driverRevision, 
             "Driver: chr62000 %.2f, Compiler: %s %3.2f, "
             "Components: IVIEngine %.2f, VISA-Spec %.2f",
             CHR62000_MAJOR_VERSION + CHR62000_MINOR_VERSION/1000.0, 
             IVI_COMPILER_NAME, IVI_COMPILER_VER_NUM, 
             IVI_ENGINE_MAJOR_VERSION + IVI_ENGINE_MINOR_VERSION/1000.0, 
             Ivi_ConvertVISAVer(VI_SPEC_VERSION));

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, driverRevision));    
Error:
    return error;
}				  

															  
/*- CHR62000_ATTR_INSTRUMENT_FIRMWARE_REVISION -*/

static ViStatus _VI_FUNC chr62000AttrFirmwareRevision_ReadCallback (ViSession vi, 
                                                                    ViSession io,
                                                                    ViConstString channelName, 
                                                                    ViAttr attributeId,
                                                                    const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      instrRev[BUFFER_SIZE];
    
    viCheckErr( viPrintf (io, "*IDN?"));

	viCheckErr( viScanf (io, "%*[^,],%*[^,],%256[^,]", instrRev));

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, instrRev));
    
Error:
    return error;
}

/*- CHR62000_ATTR_INSTRUMENT_MANUFACTURER -*/

static ViStatus _VI_FUNC chr62000AttrInstrumentManufacturer_ReadCallback (ViSession vi, 
                                                                          ViSession io,
                                                                          ViConstString channelName, 
                                                                          ViAttr attributeId,
                                                                          const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    ViChar      idQ[BUFFER_SIZE];
    
    checkErr( Ivi_GetAttributeViString (vi, "", CHR62000_ATTR_ID_QUERY_RESPONSE,
                                        0, BUFFER_SIZE, idQ));
    sscanf (idQ, "%256[^,]", rdBuffer);

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
    
Error:
    return error;
}

    

/*- CHR62000_ATTR_INSTRUMENT_MODEL -*/

static ViStatus _VI_FUNC chr62000AttrInstrumentModel_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    ViChar      idQ[BUFFER_SIZE];
    
    checkErr( Ivi_GetAttributeViString (vi, "", CHR62000_ATTR_ID_QUERY_RESPONSE,
                                        0, BUFFER_SIZE, idQ));
    sscanf (idQ, "%*[^,],%256[^,]", rdBuffer);

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
    
Error:
    return error;
}
    
static ViStatus _VI_FUNC chr62000AttrVoltage_ReadCallback (ViSession vi,
                                                           ViSession io,
                                                           ViConstString channelName,
                                                           ViAttr attributeId,
                                                           ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "SOUR:VOLT?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%Lf", value)); 
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrVoltage_WriteCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	chr62000_Digit_boundary (&value, 2); 
	viCheckErr( viPrintf (io, "SOUR:VOLT %.2f\n", value));
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrVoltLimitHigh_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "SOUR:VOLT:LIM:HIGH?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%Lf", value)); 
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrVoltLimitHigh_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	chr62000_Digit_boundary (&value, 1); 
	viCheckErr( viPrintf (io, "SOUR:VOLT:LIM:HIGH %.1f\n", value));
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrVoltLimitLow_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "SOUR:VOLT:LIM:LOW?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%Lf", value)); 
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrVoltLimitLow_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	chr62000_Digit_boundary (&value, 1); 
	viCheckErr( viPrintf (io, "SOUR:VOLT:LIM:LOW %.1f\n", value));
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrVoltProtect_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "SOUR:VOLT:PROT:HIGH?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%Lf", value)); 
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrVoltProtect_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	chr62000_Digit_boundary (&value, 1); 
	viCheckErr( viPrintf (io, "SOUR:VOLT:PROT:HIGH %.1f\n", value));
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrVoltSlew_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "SOUR:VOLT:SLEW?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%Lf", value)); 
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrVoltSlew_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	chr62000_Digit_boundary (&value, 2); 
	viCheckErr( viPrintf (io, "SOUR:VOLT:SLEW %.2f\n", value));
	Delay (delay_time);
	
Error:
	return error;
}
                                                             
static ViStatus _VI_FUNC chr62000AttrCurrent_ReadCallback (ViSession vi,
                                                           ViSession io,
                                                           ViConstString channelName,
                                                           ViAttr attributeId,
                                                           ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "SOUR:CURR?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%Lf", value)); 
	Delay (delay_time);
	
Error:
	return error;
}
                                                           
static ViStatus _VI_FUNC chr62000AttrCurrent_WriteCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	chr62000_Digit_boundary (&value, 2); 
	viCheckErr( viPrintf (io, "SOUR:CURR %.2f\n", value));
	Delay (delay_time);
	
Error:
	return error;
}
                                                            
static ViStatus _VI_FUNC chr62000AttrCurrLimitHigh_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "SOUR:CURR:LIM:HIGH?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%Lf", value)); 
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrCurrLimitHigh_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	chr62000_Digit_boundary (&value, 1); 
	viCheckErr( viPrintf (io, "SOUR:CURR:LIM:HIGH %.1f\n", value));
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrCurrLimitLow_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "SOUR:CURR:LIM:LOW?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%Lf", value)); 
	Delay (delay_time);
	
Error:
	return error;
}
                                                                
static ViStatus _VI_FUNC chr62000AttrCurrLimitLow_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	chr62000_Digit_boundary (&value, 1); 
	viCheckErr( viPrintf (io, "SOUR:CURR:LIM:LOW %.1f\n", value));
	Delay (delay_time);
	
Error:
	return error;
}
                                                                 
static ViStatus _VI_FUNC chr62000AttrCurrProtect_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "SOUR:CURR:PROT:HIGH?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%Lf", value)); 
	Delay (delay_time);
	
Error:
	return error;
}
                                                               
static ViStatus _VI_FUNC chr62000AttrCurrProtect_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	chr62000_Digit_boundary (&value, 1); 
	viCheckErr( viPrintf (io, "SOUR:CURR:PROT:HIGH %.1f\n", value));
	Delay (delay_time);
	
Error:
	return error;
}
                                                                
static ViStatus _VI_FUNC chr62000AttrCurrSlew_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "SOUR:CURR:SLEW?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%Lf", value)); 
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrCurrSlew_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	chr62000_Digit_boundary (&value, 2); 
	viCheckErr( viPrintf (io, "SOUR:CURR:SLEW %.2f\n", value));
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrPowerProtect_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "SOUR:POW:PROT:HIGH?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%Lf", value)); 
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrPowerProtect_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	chr62000_Digit_boundary (&value, 1);
	viCheckErr( viPrintf (io, "SOUR:POW:PROT:HIGH %.1f\n", value));
	Delay (delay_time);
	
Error:
	return error;
}


static ViStatus _VI_FUNC chr62000AttrOutput_WriteCallback (ViSession vi,
                                                           ViSession io,
                                                           ViConstString channelName,
                                                           ViAttr attributeId,
                                                           ViBoolean value)
{
	ViStatus	error = VI_SUCCESS;
	
	if ( value == VI_TRUE)
	{
		viCheckErr( viPrintf (io, "CONF:OUTP ON\n"));
		Delay (delay_time);
	}
	else if ( value == VI_FALSE)
	{
		viCheckErr( viPrintf (io, "CONF:OUTP OFF\n"));
		Delay (delay_time);
	}	
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrTtlPort_ReadCallback (ViSession vi,
                                                           ViSession io,
                                                           ViConstString channelName,
                                                           ViAttr attributeId,
                                                           ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "CONF:TTL?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%d", value)); 
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrTtlPort_WriteCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "CONF:TTL %d\n", value));
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrBeeper_ReadCallback (ViSession vi,
                                                          ViSession io,
                                                          ViConstString channelName,
                                                          ViAttr attributeId,
                                                          ViBoolean *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		state[8];
	
	viCheckErr( viPrintf (io, "CONF:BEEP?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%s", state)); 
	Delay (delay_time);
	
	if(strncmp(state,"On",2) == 0)
		*value=VI_TRUE;
	else if(strncmp(state,"OFF",3) == 0)
		*value=VI_FALSE;
			
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrBeeper_WriteCallback (ViSession vi,
                                                           ViSession io,
                                                           ViConstString channelName,
                                                           ViAttr attributeId,
                                                           ViBoolean value)
{
	ViStatus	error = VI_SUCCESS;
	
	if ( value == VI_TRUE)
	{
		viCheckErr( viPrintf (io, "CONF:BEEP ON\n"));
		Delay (delay_time);
	}
	else if ( value == VI_FALSE)
	{
		viCheckErr( viPrintf (io, "CONF:BEEP OFF\n"));
		Delay (delay_time);
	}	
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrRemoteMode_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViBoolean value)
{
	ViStatus	error = VI_SUCCESS;
	ViUInt16	nInterFace=0; 
	
	viCheckErr( viGetAttribute (io,  VI_ATTR_INTF_TYPE, &nInterFace)); 
	if ( nInterFace == VI_INTF_ASRL)  
	{
		if ( value == VI_TRUE)
		{
			viCheckErr( viPrintf (io, "CONF:REM ON\n"));
			Delay (delay_time);
		}
		else if ( value == VI_FALSE)
		{
			viCheckErr( viPrintf (io, "CONF:REM OFF\n"));
			Delay (delay_time);
		}	
	}
	
	else
		checkErr( error= CHR62000_ERROR_INVALID_INTERFACE );
		
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrFoldbackDelayTime_ReadCallback (ViSession vi,
                                                                     ViSession io,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "CONF:FOLDT?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%Lf", value)); 
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrFoldbackDelayTime_WriteCallback (ViSession vi,
                                                                      ViSession io,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	chr62000_Digit_boundary (&value, 2); 
	viCheckErr( viPrintf (io, "CONF:FOLDT %.2f\n", value));
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrFoldbackProtect_ReadCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		state[10];
	
	viCheckErr( viPrintf (io, "CONF:FOLD?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%s", state)); 
	Delay (delay_time);
	
	if(strncmp(state,"DISABLE",7) == 0)
		*value=0;
	else if(strncmp(state,"CVTOCC",6) == 0)
		*value=1;
	else if(strncmp(state,"CCTOCV",6) == 0)
		*value=2;		

Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrFoldbackProtect_WriteCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString	Cmd;
	
	checkErr( Ivi_GetViInt32EntryFromIndex (value, &attrFoldbackProtectRangeTable, VI_NULL, VI_NULL,
												VI_NULL, &Cmd, VI_NULL));
	viCheckErr( viPrintf (io, "CONF:FOLD %s\n", Cmd));
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrApgMode_ReadCallback (ViSession vi,
                                                           ViSession io,
                                                           ViConstString channelName,
                                                           ViAttr attributeId,
                                                           ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		state[8];
	
	viCheckErr( viPrintf (io, "CONF:APG?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%s", state)); 
	Delay (delay_time);
	
	if(strncmp(state,"NONE",4) == 0)
		*value=0;
	else if(strncmp(state,"VI",2) == 0)
		*value=3;		
	else if(strncmp(state,"V",1) == 0)
		*value=1;
	else if(strncmp(state,"I",1) == 0)
		*value=2;	
		
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrApgMode_WriteCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString	Cmd;
	
	checkErr( Ivi_GetViInt32EntryFromIndex (value, &attrApgModeRangeTable, VI_NULL, VI_NULL,
												VI_NULL, &Cmd, VI_NULL));
	viCheckErr( viPrintf (io, "CONF:APG %s\n", Cmd));
	Delay (delay_time);
	
	if (value == 1)
	{
		checkErr ( Ivi_DeleteAttribute (vi, CHR62000_ATTR_VOLTAGE));
		checkErr ( Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_VOLTAGE, "CHR62000_ATTR_VOLTAGE", 0.00,
											IVI_VAL_NEVER_CACHE, chr62000AttrVoltage_ReadCallback,
											chr62000AttrVoltage_WriteCallback, &attrVoltageRangeTable, 0));
		checkErr ( Ivi_DeleteAttribute (vi, CHR62000_ATTR_CURRENT)); 
		checkErr ( Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_CURRENT, "CHR62000_ATTR_CURRENT", 0.00,
											0, chr62000AttrCurrent_ReadCallback,
											chr62000AttrCurrent_WriteCallback, &attrCurrentRangeTable, 0));	
	}											
	else if (value == 2)
	{
	
		checkErr ( Ivi_DeleteAttribute (vi, CHR62000_ATTR_CURRENT));
		checkErr ( Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_CURRENT, "CHR62000_ATTR_CURRENT", 0.00,
											IVI_VAL_NEVER_CACHE, chr62000AttrCurrent_ReadCallback,
											chr62000AttrCurrent_WriteCallback, &attrCurrentRangeTable, 0));
		checkErr ( Ivi_DeleteAttribute (vi, CHR62000_ATTR_VOLTAGE));  
		checkErr ( Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_VOLTAGE, "CHR62000_ATTR_VOLTAGE", 0.00,
											0, chr62000AttrVoltage_ReadCallback,
											chr62000AttrVoltage_WriteCallback, &attrVoltageRangeTable, 0));
	}
	else if (value == 3)
	{
		checkErr ( Ivi_DeleteAttribute (vi, CHR62000_ATTR_VOLTAGE)); 
		checkErr ( Ivi_DeleteAttribute (vi, CHR62000_ATTR_CURRENT));
		checkErr ( Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_VOLTAGE, "CHR62000_ATTR_VOLTAGE", 0.00,
											IVI_VAL_NEVER_CACHE, chr62000AttrVoltage_ReadCallback,
											chr62000AttrVoltage_WriteCallback, &attrVoltageRangeTable, 0));
		checkErr ( Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_CURRENT, "CHR62000_ATTR_CURRENT", 0.00,
											IVI_VAL_NEVER_CACHE, chr62000AttrCurrent_ReadCallback,
											chr62000AttrCurrent_WriteCallback, &attrCurrentRangeTable, 0));											
	}
	else if (value == 0) 
	{
		checkErr ( Ivi_DeleteAttribute (vi, CHR62000_ATTR_VOLTAGE)); 
		checkErr ( Ivi_DeleteAttribute (vi, CHR62000_ATTR_CURRENT));
		checkErr ( Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_VOLTAGE, "CHR62000_ATTR_VOLTAGE", 0.00,
											0, chr62000AttrVoltage_ReadCallback,
											chr62000AttrVoltage_WriteCallback, &attrVoltageRangeTable, 0));
		checkErr ( Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_CURRENT, "CHR62000_ATTR_CURRENT", 0.00,
											0, chr62000AttrCurrent_ReadCallback,
											chr62000AttrCurrent_WriteCallback, &attrCurrentRangeTable, 0));											
	}	
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrApgReferenceVoltage_ReadCallback (ViSession vi,
                                                                       ViSession io,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViBoolean *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		state[5];
	
	viCheckErr( viPrintf (io, "CONF:APGV?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%s", state)); 
	Delay (delay_time);
	
	if(strncmp(state,"5V",2) == 0)
		*value=VI_FALSE;
	else if(strncmp(state,"10V",3) == 0)
		*value=VI_TRUE;		
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrApgReferenceVoltage_WriteCallback (ViSession vi,
                                                                        ViSession io,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViBoolean value)
{                                                                        
	ViStatus	error = VI_SUCCESS;
	
	if (value == VI_FALSE)
	{
		viCheckErr( viPrintf (io, "CONF:APGV FIVE\n"));
		Delay (delay_time);
	}
	else if (value == VI_TRUE)
	{
		viCheckErr( viPrintf (io, "CONF:APGV TEN\n"));
		Delay (delay_time);
	}
	
Error:
	return error;													   
}									  

static ViStatus _VI_FUNC chr62000AttrProgSelected_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "PROG:SEL?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%d", value)); 
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrProgSelected_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "PROG:SEL %d\n", value));
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrProgSeqVoltage_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "PROG:SEQ:VOLT?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%Lf", value)); 
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrProgSeqVoltage_WriteCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	chr62000_Digit_boundary (&value, 2); 
	viCheckErr( viPrintf (io, "PROG:SEQ:VOLT %.2f\n", value));
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrProgSeqVoltSlew_ReadCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "PROG:SEQ:VOLT:SLEW?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%Lf", value)); 
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrProgSeqVoltSlew_WriteCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	chr62000_Digit_boundary (&value, 2); 
	viCheckErr( viPrintf (io, "PROG:SEQ:VOLT:SLEW %.2f\n", value));
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrProgSeqCurrent_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "PROG:SEQ:CURR?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%Lf", value)); 
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrProgSeqCurrent_WriteCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	chr62000_Digit_boundary (&value, 2); 
	viCheckErr( viPrintf (io, "PROG:SEQ:CURR %.2f\n", value));
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrProgSeqCurrSlew_ReadCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "PROG:SEQ:CURR:SLEW?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%Lf", value)); 
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrProgSeqCurrSlew_WriteCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	chr62000_Digit_boundary (&value, 2); 
	viCheckErr( viPrintf (io, "PROG:SEQ:CURR:SLEW %.2f\n", value));
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrProgSeqTtl_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "PROG:SEQ:TTL?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%d", value)); 
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrProgSeqTtl_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "PROG:SEQ:TTL %d\n", value));
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrProgSeqTime_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "PROG:SEQ:TIME?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%Lf", value)); 
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrProgSeqTime_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	chr62000_Digit_boundary (&value, 2); 
	viCheckErr( viPrintf (io, "PROG:SEQ:TIME %.2f\n", value));
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrProgSeqType_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		state[20];
	
	viCheckErr( viPrintf (io, "PROG:SEQ:TYPE?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%s", state)); 
	Delay (delay_time);
	
	if(strncmp(state,"AUTO",4) == 0)
		*value=0;
	else if(strncmp(state,"MANUAL",6) == 0)
		*value=1;
	else if(strncmp(state,"EXT.TRIGGER",11) == 0)
		*value=2;
			
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrProgSeqType_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString	Cmd;
	
	checkErr( Ivi_GetViInt32EntryFromIndex (value, &attrProgSeqTypeRangeTable, VI_NULL, VI_NULL,
												VI_NULL, &Cmd, VI_NULL));
	viCheckErr( viPrintf (io, "PROG:SEQ:TYPE %s\n", Cmd));
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrProgLink_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "PROG:LINK?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%d", value)); 
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrProgLink_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "PROG:LINK %d\n", value));
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrProgCount_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "PROG:COUNT?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%d", value)); 
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrProgCount_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "PROG:COUNT %d\n", value));
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrProgSeqSelected_ReadCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "PROG:SEQ:SEL?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%d", value)); 
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrProgSeqSelected_WriteCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "PROG:SEQ:SEL %d\n", value));
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrProgRun_WriteCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViBoolean value)
{
	ViStatus	error = VI_SUCCESS;
	
	if ( value == VI_TRUE)
	{
		viCheckErr( viPrintf (io, "PROG:RUN ON\n"));
		Delay (delay_time);
		checkErr( chr62000_Detect_error (vi, "PROG:RUN ON\n"));
	}
	else if ( value == VI_FALSE)
	{
		viCheckErr( viPrintf (io, "PROG:RUN OFF\n"));
		Delay (delay_time);
		checkErr( chr62000_Detect_error (vi, "PROG:RUN OFF\n"));
	}	
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrStatusEse_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "*ESE?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%d", value)); 
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrStatusEse_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "*ESE %d\n", value));
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrStatusSre_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "*SRE?\n"));
	Delay (delay_time);
	viCheckErr( viScanf (io, "%d", value)); 
	Delay (delay_time);
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr62000AttrStatusSre_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "*SRE %d\n", value));
	Delay (delay_time);
	
Error:
	return error;
}

/*****************************************************************************
 * Function: chr62000_InitAttributes                                                  
 * Purpose:  This function adds attributes to the IVI session, initializes   
 *           instrument attributes, and sets attribute invalidation          
 *           dependencies.                                                   
 *****************************************************************************/
static ViStatus chr62000_InitAttributes (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     flags = 0;
    
        /*- Initialize instrument attributes --------------------------------*/            

    checkErr( Ivi_SetAttributeViInt32 (vi, "", CHR62000_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION,
                                       0, CHR62000_MAJOR_VERSION));
    checkErr( Ivi_SetAttributeViInt32 (vi, "", CHR62000_ATTR_SPECIFIC_DRIVER_MINOR_VERSION,
                                       0, CHR62000_MINOR_VERSION));
    checkErr( Ivi_SetAttrReadCallbackViString (vi, CHR62000_ATTR_SPECIFIC_DRIVER_REVISION,
                                               chr62000AttrDriverRevision_ReadCallback));
    checkErr( Ivi_SetAttributeViInt32 (vi, "", CHR62000_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION,
                                       0, CHR62000_CLASS_SPEC_MAJOR_VERSION));
    checkErr( Ivi_SetAttributeViInt32 (vi, "", CHR62000_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION,
                                       0, CHR62000_CLASS_SPEC_MINOR_VERSION));
    checkErr( Ivi_SetAttributeViString (vi, "", CHR62000_ATTR_IO_SESSION_TYPE,
                                        0, CHR62000_IO_SESSION_TYPE));
    checkErr( Ivi_SetAttributeViString (vi, "", CHR62000_ATTR_SUPPORTED_INSTRUMENT_MODELS,
                                        0, CHR62000_SUPPORTED_INSTRUMENT_MODELS));


    checkErr( Ivi_SetAttrReadCallbackViString (vi, CHR62000_ATTR_INSTRUMENT_FIRMWARE_REVISION,
                                               chr62000AttrFirmwareRevision_ReadCallback));

	checkErr( Ivi_GetAttributeFlags (vi, CHR62000_ATTR_INSTRUMENT_MANUFACTURER, &flags));
	checkErr( Ivi_SetAttributeFlags (vi, CHR62000_ATTR_INSTRUMENT_MANUFACTURER, 
	                                 flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
	checkErr( Ivi_SetAttrReadCallbackViString (vi, CHR62000_ATTR_INSTRUMENT_MANUFACTURER,
	                                           chr62000AttrInstrumentManufacturer_ReadCallback));

	checkErr( Ivi_GetAttributeFlags (vi, CHR62000_ATTR_INSTRUMENT_MODEL, &flags));
	checkErr( Ivi_SetAttributeFlags (vi, CHR62000_ATTR_INSTRUMENT_MODEL, 
	                                 flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
	checkErr( Ivi_SetAttrReadCallbackViString (vi, CHR62000_ATTR_INSTRUMENT_MODEL,
	                                           chr62000AttrInstrumentModel_ReadCallback));

    checkErr( Ivi_SetAttributeViString (vi, "", CHR62000_ATTR_SPECIFIC_DRIVER_VENDOR,
                                        0, CHR62000_DRIVER_VENDOR));
    checkErr( Ivi_SetAttributeViString (vi, "", CHR62000_ATTR_SPECIFIC_DRIVER_DESCRIPTION,
                                        0, CHR62000_DRIVER_DESCRIPTION));
    checkErr( Ivi_SetAttributeViAddr (vi, VI_NULL, IVI_ATTR_OPC_CALLBACK, 0,
                                      chr62000_WaitForOPCCallback));
    checkErr( Ivi_SetAttributeViAddr (vi, VI_NULL, IVI_ATTR_CHECK_STATUS_CALLBACK, 0,
                                      chr62000_CheckStatusCallback));
	checkErr( Ivi_SetAttributeViBoolean (vi, VI_NULL, IVI_ATTR_SUPPORTS_WR_BUF_OPER_MODE, 
	                                     0, VI_TRUE));

        
	checkErr( Ivi_SetAttributeViString (vi, "", CHR62000_ATTR_GROUP_CAPABILITIES, 0,
	                                    "None"));
    
        /*- Add instrument-specific attributes ------------------------------*/            
	
	checkErr (Ivi_AddAttributeViString (vi, CHR62000_ATTR_ID_QUERY_RESPONSE,
	                                    "CHR62000_ATTR_ID_QUERY_RESPONSE",
	                                    "CHROMA,62012P-80-60,01.00,2005/08/16",
	                                    IVI_VAL_NOT_USER_WRITABLE,
	                                    chr62000AttrIdQueryResponse_ReadCallback,
	                                    VI_NULL));
	                                           
    checkErr( Ivi_AddAttributeViInt32 (vi, CHR62000_ATTR_OPC_TIMEOUT,
                                       "CHR62000_ATTR_OPC_TIMEOUT",
                                       5000, IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL)); 
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_VOLTAGE,
	                                    "CHR62000_ATTR_VOLTAGE", 0.00, 0,
	                                    chr62000AttrVoltage_ReadCallback,
	                                    chr62000AttrVoltage_WriteCallback,
	                                    &attrVoltageRangeTable, 0));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_VOLT_LIMIT_HIGH,
	                                    "CHR62000_ATTR_VOLT_LIMIT_HIGH", 80.0, 0,
	                                    chr62000AttrVoltLimitHigh_ReadCallback,
	                                    chr62000AttrVoltLimitHigh_WriteCallback,
	                                    &attrVoltLimitHighRangeTable, 0));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_VOLT_LIMIT_LOW,
	                                    "CHR62000_ATTR_VOLT_LIMIT_LOW", 0.0, 0,
	                                    chr62000AttrVoltLimitLow_ReadCallback,
	                                    chr62000AttrVoltLimitLow_WriteCallback,
	                                    &attrVoltLimitLowRangeTable, 0));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_VOLT_PROTECT,
	                                    "CHR62000_ATTR_VOLT_PROTECT", 88.0, 0,
	                                    chr62000AttrVoltProtect_ReadCallback,
	                                    chr62000AttrVoltProtect_WriteCallback,
	                                    &attrVoltProtectRangeTable, 0));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_VOLT_SLEW,
	                                    "CHR62000_ATTR_VOLT_SLEW", 1.00, 0,
	                                    chr62000AttrVoltSlew_ReadCallback,
	                                    chr62000AttrVoltSlew_WriteCallback,
	                                    &attrVoltSlewRangeTable, 0));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_CURRENT,
	                                    "CHR62000_ATTR_CURRENT", 0.00, 0,
	                                    chr62000AttrCurrent_ReadCallback,
	                                    chr62000AttrCurrent_WriteCallback,
	                                    &attrCurrentRangeTable, 0));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_CURR_LIMIT_HIGH,
	                                    "CHR62000_ATTR_CURR_LIMIT_HIGH", 60.0, 0,
	                                    chr62000AttrCurrLimitHigh_ReadCallback,
	                                    chr62000AttrCurrLimitHigh_WriteCallback,
	                                    &attrCurrLimitHighRangeTable, 0));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_CURR_LIMIT_LOW,
	                                    "CHR62000_ATTR_CURR_LIMIT_LOW", 0.0, 0,
	                                    chr62000AttrCurrLimitLow_ReadCallback,
	                                    chr62000AttrCurrLimitLow_WriteCallback,
	                                    &attrCurrLimitLowRangeTable, 0));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_CURR_PROTECT,
	                                    "CHR62000_ATTR_CURR_PROTECT", 63.0, 0,
	                                    chr62000AttrCurrProtect_ReadCallback,
	                                    chr62000AttrCurrProtect_WriteCallback,
	                                    &attrCurrProtectRangeTable, 0));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_CURR_SLEW,
	                                    "CHR62000_ATTR_CURR_SLEW", 1.00, 0,
	                                    chr62000AttrCurrSlew_ReadCallback,
	                                    chr62000AttrCurrSlew_WriteCallback,
	                                    &attrCurrSlewRangeTable, 0));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_POWER_PROTECT,
	                                    "CHR62000_ATTR_POWER_PROTECT", 1260.0, 0,
	                                    chr62000AttrPowerProtect_ReadCallback,
	                                    chr62000AttrPowerProtect_WriteCallback,
	                                    &attrPowerProtectRangeTable, 0));
	checkErr (Ivi_AddAttributeViBoolean (vi, CHR62000_ATTR_OUTPUT,
	                                     "CHR62000_ATTR_OUTPUT", VI_FALSE,
	                                     IVI_VAL_HIDDEN, VI_NULL,
	                                     chr62000AttrOutput_WriteCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR62000_ATTR_TTL_PORT,
	                                   "CHR62000_ATTR_TTL_PORT", 0, 0,
	                                   chr62000AttrTtlPort_ReadCallback,
	                                   chr62000AttrTtlPort_WriteCallback,
	                                   &attrTtlPortRangeTable));
	checkErr (Ivi_AddAttributeViBoolean (vi, CHR62000_ATTR_BEEPER,
	                                     "CHR62000_ATTR_BEEPER", VI_TRUE,
	                                     IVI_VAL_HIDDEN,
	                                     chr62000AttrBeeper_ReadCallback,
	                                     chr62000AttrBeeper_WriteCallback));
	checkErr (Ivi_AddAttributeViBoolean (vi, CHR62000_ATTR_REMOTE_MODE,
	                                     "CHR62000_ATTR_REMOTE_MODE", VI_FALSE, 0,
	                                     VI_NULL,
	                                     chr62000AttrRemoteMode_WriteCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR62000_ATTR_FOLDBACK_PROTECT,
	                                   "CHR62000_ATTR_FOLDBACK_PROTECT",
	                                   CHR62000_VAL_FOLD_DISABLE, IVI_VAL_HIDDEN,
	                                   chr62000AttrFoldbackProtect_ReadCallback,
	                                   chr62000AttrFoldbackProtect_WriteCallback,
	                                   &attrFoldbackProtectRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_FOLDBACK_DELAY_TIME,
	                                    "CHR62000_ATTR_FOLDBACK_DELAY_TIME", 0.01, 0,
	                                    chr62000AttrFoldbackDelayTime_ReadCallback,
	                                    chr62000AttrFoldbackDelayTime_WriteCallback,
	                                    &attrFoldbackDelayTimeRangeTable, 0));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR62000_ATTR_APG_MODE,
	                                   "CHR62000_ATTR_APG_MODE",
	                                   CHR62000_VAL_APG_NONE, IVI_VAL_HIDDEN,
	                                   chr62000AttrApgMode_ReadCallback,
	                                   chr62000AttrApgMode_WriteCallback,
	                                   &attrApgModeRangeTable));
	checkErr (Ivi_AddAttributeViBoolean (vi, CHR62000_ATTR_APG_REFERENCE_VOLTAGE,
	                                     "CHR62000_ATTR_APG_REFERENCE_VOLTAGE",
	                                     VI_FALSE, IVI_VAL_HIDDEN,
	                                     chr62000AttrApgReferenceVoltage_ReadCallback,
	                                     chr62000AttrApgReferenceVoltage_WriteCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR62000_ATTR_PROG_SELECTED,
	                                   "CHR62000_ATTR_PROG_SELECTED", 1, 0,
	                                   chr62000AttrProgSelected_ReadCallback,
	                                   chr62000AttrProgSelected_WriteCallback,
	                                   &attrProgSelectedRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_PROG_SEQ_VOLTAGE,
	                                    "CHR62000_ATTR_PROG_SEQ_VOLTAGE", 0.00,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_HIDDEN,
	                                    chr62000AttrProgSeqVoltage_ReadCallback,
	                                    chr62000AttrProgSeqVoltage_WriteCallback,
	                                    &attrProgSeqVoltageRangeTable, 0));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_PROG_SEQ_VOLT_SLEW,
	                                    "CHR62000_ATTR_PROG_SEQ_VOLT_SLEW", 10.00,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_HIDDEN,
	                                    chr62000AttrProgSeqVoltSlew_ReadCallback,
	                                    chr62000AttrProgSeqVoltSlew_WriteCallback,
	                                    &attrProgSeqVoltSlewRangeTable, 0));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_PROG_SEQ_CURRENT,
	                                    "CHR62000_ATTR_PROG_SEQ_CURRENT", 0.00,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_HIDDEN,
	                                    chr62000AttrProgSeqCurrent_ReadCallback,
	                                    chr62000AttrProgSeqCurrent_WriteCallback,
	                                    &attrProgSeqCurrentRangeTable, 0));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_PROG_SEQ_CURR_SLEW,
	                                    "CHR62000_ATTR_PROG_SEQ_CURR_SLEW", 1.00,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_HIDDEN,
	                                    chr62000AttrProgSeqCurrSlew_ReadCallback,
	                                    chr62000AttrProgSeqCurrSlew_WriteCallback,
	                                    &attrProgSeqCurrSlewRangeTable, 0));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR62000_ATTR_PROG_SEQ_TTL,
	                                   "CHR62000_ATTR_PROG_SEQ_TTL", 0,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_HIDDEN,
	                                   chr62000AttrProgSeqTtl_ReadCallback,
	                                   chr62000AttrProgSeqTtl_WriteCallback,
	                                   &attrProgSeqTtlRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_PROG_SEQ_TIME,
	                                    "CHR62000_ATTR_PROG_SEQ_TIME", 0.00,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_HIDDEN,
	                                    chr62000AttrProgSeqTime_ReadCallback,
	                                    chr62000AttrProgSeqTime_WriteCallback,
	                                    &attrProgSeqTimeRangeTable, 0));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR62000_ATTR_PROG_SEQ_TYPE,
	                                   "CHR62000_ATTR_PROG_SEQ_TYPE",
	                                   CHR62000_VAL_AUTO,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_HIDDEN,
	                                   chr62000AttrProgSeqType_ReadCallback,
	                                   chr62000AttrProgSeqType_WriteCallback,
	                                   &attrProgSeqTypeRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR62000_ATTR_PROG_LINK,
	                                   "CHR62000_ATTR_PROG_LINK", 0,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_HIDDEN,
	                                   chr62000AttrProgLink_ReadCallback,
	                                   chr62000AttrProgLink_WriteCallback,
	                                   &attrProgLinkRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR62000_ATTR_PROG_COUNT,
	                                   "CHR62000_ATTR_PROG_COUNT", 1,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_HIDDEN,
	                                   chr62000AttrProgCount_ReadCallback,
	                                   chr62000AttrProgCount_WriteCallback,
	                                   &attrProgCountRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR62000_ATTR_PROG_SEQ_SELECTED,
	                                   "CHR62000_ATTR_PROG_SEQ_SELECTED", 1, 0,
	                                   chr62000AttrProgSeqSelected_ReadCallback,
	                                   chr62000AttrProgSeqSelected_WriteCallback,
	                                   &attrProgSeqSelectedRangeTable));
	checkErr (Ivi_AddAttributeViBoolean (vi, CHR62000_ATTR_PROG_RUN,
	                                     "CHR62000_ATTR_PROG_RUN", VI_FALSE,
	                                     IVI_VAL_NEVER_CACHE | IVI_VAL_HIDDEN,
	                                     VI_NULL, chr62000AttrProgRun_WriteCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_MAXPOWER,
	                                    "CHR62000_ATTR_MAXPOWER", 1200.00,
	                                    IVI_VAL_HIDDEN, VI_NULL, VI_NULL, VI_NULL, 0));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR62000_ATTR_STATUS_ESE,
	                                   "CHR62000_ATTR_STATUS_ESE", 0, 0,
	                                   chr62000AttrStatusEse_ReadCallback,
	                                   chr62000AttrStatusEse_WriteCallback,
	                                   &attrStatusEseRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR62000_ATTR_STATUS_SRE,
	                                   "CHR62000_ATTR_STATUS_SRE", 0, 0,
	                                   chr62000AttrStatusSre_ReadCallback,
	                                   chr62000AttrStatusSre_WriteCallback,
	                                   &attrStatusSreRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR62000_ATTR_SIMU_RESISTOR,
	                                    "CHR62000_ATTR_SIMU_RESISTOR", 1000.00,
	                                    IVI_VAL_HIDDEN, VI_NULL, VI_NULL, VI_NULL, 0));
                                       
        /*- Setup attribute invalidations -----------------------------------*/            


	checkErr (Ivi_AddAttributeInvalidation (vi, CHR62000_ATTR_VOLTAGE,
	                                        CHR62000_ATTR_VOLT_PROTECT, VI_FALSE));
	checkErr (Ivi_AddAttributeInvalidation (vi, CHR62000_ATTR_CURRENT,
	                                        CHR62000_ATTR_CURR_PROTECT, VI_FALSE));
Error:
    return error;
}

/*****************************************************************************
 *------------------- End Instrument Driver Source Code ---------------------*
 *****************************************************************************/

