/*************************************************************************************** 
 * (c) 2007, National Instruments, Corporation. All Rights Reserved.	     	       * 
 ***************************************************************************************/
 
/***************************************************************************************
 *  Anritsu Ltd, MG369XB Synthesized Signal Generator   
 *  VXIPNP, LabWindows/CVI 8.0 Instrument Driver
 *
 *  Original Release: 10/20/2007                                  
 *  By: Xiangchao Xie, National Instruments, Shanghai China                              
 *      PH: (86.21)50509810	Fax: (86.21)65556244	
 *		Email: xiangchao.xie@ni.com                            
 *                                                                           
 *  Modification History:                                                    
 *                                                                           
 *      07/16/2007  - Instrument Driver Created.
 *		10/20/2007  - Instrument Driver Released. 
 ***************************************************************************************/

#include <visa.h>
#include <ansi_c.h>
#include "aumg369xb.h"


/*= DEFINES ===========================================================================*/
#define AUMG369XB_REVISION			"Rev 1.0, 10/2007, CVI 8.1" /*  Instrument driver revision */
#define AUMG369XB_BUILD_REVISION	"00001"						/*  Driver build revision      */
#define AUMG369XB_BUFFER_SIZE		512L         				/*  File I/O buffer size 	   */

/*= Macros ============================================================================*/ 
#ifndef CHECKERR
#define CHECKERR(fCall) \
	if (status = (fCall), (status = (status < 0) ? status : VI_SUCCESS)) \
		{ \
		goto Error; \
		} \
		else
#endif

#ifndef CHECKPARAM
#define CHECKPARAM(fCall, errorCode) \
	if (fCall) \
		{ \
		status = (errorCode); \
		goto Error; \
		} \
		else
#endif


/*= Static Parameter ==================================================================*/
static ViInt32 gModelNumber						=	AUMG369XB_MODEL_MG3691B;
static ViInt32 gPowerLevelUnit					=	0;


/***************************************************************************************/
/*= INSTRUMENT-DEPENDENT COMMAND ARRAYS ===============================================*/
/***************************************************************************************/
static ViString triggerSourceTable[]			=	{"AUT", "TEX", "EXT", "MAN"};

static ViString alternateSweepRangeTable[]		=	{"AFU", "AF1", "AF3", "AD1", "AD5", "AD6"};

static ViString stepSweepModeTable[]			=	{"LGS", "LIS"};

static ViString sweepRangeTable[]				=	{"FUL", "SF1", "SF3", "DF0", "DF1", "DF5", "DF6"};

static ViString scancwFrequencyTable[]			=	{"SQD", "SQU", "SQF"};

static ViString	 frequencyMarkerModeTable[]		=	{"IM1", "VM1", "MK0"};

static ViString levelingModeTable[]				=	{"IL1", "DL1", "PL1", "LV0", "LV1"};

static ViString internalFMModeTable[]			=	{"FMN", "FM9", "FM7", "FM8"};

static ViString externalFMModeTable[]			=	{"FM2", "FML", "FM1", "FMW"};

static ViString internalPMModeTable[]			=	{"PH7", "PH8"};
	
static ViString externalPMModeTable[]			=	{"PH1", "PH2"};

static ViString triggerEdgeTypeTable[]			=	{"PTR", "PTF"};

static ViString blankingPolarityTable[]			=	{"BPP", "BPN"};	 

static ViString markerPolarityTable[]			=	{"MPP", "MPN"};

static ViString penliftRelayTable[]				=	{"PPO", "PPC"};

static ViString executeCommandTable[]			=	{"GTO", "GTC", "GTD", "GTF", "GTL", "GTS", "GTT", "GTU"};

static ViString powerLevelUnitTable[]			=	{"LOG, PU0", "LOG, PU2", "LIN, PU1"};


/***************************************************************************************/
/*= INSTRUMENT-DEPENDENT STATUS/RANGE STRUCTURE  ======================================*/
/***************************************************************************************/
/* aumg369xb_stringValPair is used in the aumg369xb_errorMessage function			   */
/*=====================================================================================*/
typedef struct  aumg369xb_stringValPair
{
   ViStatus stringVal;
   ViString stringName;
}  aumg369xb_tStringValPair;


/**********************************************************************8888888888*******/
/*= UTILITY ROUTINE DECLARATIONS (Non-Exportable Functions) ===========================*/
/*************************************************************************8888888888****/
ViBoolean	aumg369xb_invalidPointer (void *value);


/***************************************************************************************/
/*= INSTRUMENT SPECIFIC UTILITY ROUTINE DECLARATIONS (Non-Exportable Functions) =======*/
/***************************************************************************************/
ViStatus	aumg369xb_defaultInstrSetup (ViSession vi);
ViStatus	aumg369xb_checkStatus (ViSession vi);


/***************************************************************************************/
/*====== USER-CALLABLE FUNCTIONS (Exportable Functions) ===============================*/
/***************************************************************************************/

/*=====================================================================================*/
/* Function: Initialize														 		   */
/*																			 		   */
/* Purpose:  This function opens the instrument, queries the instrument for its ID,    */
/*           and initializes the instrument to a known state.					       */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_init (ViRsrc resourceName, 
					ViBoolean IDQuery, ViBoolean resetDevice, 
					ViSession *vi)
{
    ViStatus 	status = VI_SUCCESS;
    ViSession 	rmSession = 0;
	ViChar      rdBuffer[AUMG369XB_BUFFER_SIZE];

    /*- Check input parameter ranges --------------------------------------------------*/
    CHECKPARAM (aumg369xb_invalidPointer (vi), VI_ERROR_PARAMETER4);
	/*---------------------------------------------------------------------------------*/

    /*- Open instrument session -------------------------------------------------------*/
    if ((status = viOpenDefaultRM (&rmSession)) < 0)
        return status;

    if ((status = viOpen (rmSession, resourceName, VI_NULL, VI_NULL, vi)) < 0) 
	{
        viClose (rmSession);
        return status;
    }

    /*- Configure VISA Formatted I/O --------------------------------------------------*/
    CHECKERR (viSetAttribute (*vi, VI_ATTR_TMO_VALUE, 10000));
    CHECKERR (viSetBuf (*vi, VI_READ_BUF|VI_WRITE_BUF, 4000));
    CHECKERR (viSetAttribute (*vi, VI_ATTR_WR_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS));
    CHECKERR (viSetAttribute (*vi, VI_ATTR_RD_BUF_OPER_MODE,VI_FLUSH_ON_ACCESS));
	
    /*- Identification Query ----------------------------------------------------------*/
    if (IDQuery) 
	{
		ViChar *queryResponse1 = "ANRITSU,MG3691B";
		ViChar *queryResponse2 = "ANRITSU,MG3692B";
		ViChar *queryResponse3 = "ANRITSU,MG3693B";
		ViChar *queryResponse4 = "ANRITSU,MG3694B";
		ViChar *queryResponse5 = "ANRITSU,MG3695B";
		ViChar *queryResponse6 = "ANRITSU,MG3696B";
		
		CHECKERR (viQueryf (*vi, "*IDN?", "%256[^\n]", rdBuffer));													
		
		if(strncmp (rdBuffer, queryResponse1, strlen(queryResponse1)) == 0)
			gModelNumber = AUMG369XB_MODEL_MG3691B;
		else if (strncmp (rdBuffer, queryResponse2, strlen(queryResponse2)) == 0)
			gModelNumber = AUMG369XB_MODEL_MG3692B;
		else if (strncmp (rdBuffer, queryResponse3, strlen(queryResponse3)) == 0)
			gModelNumber = AUMG369XB_MODEL_MG3693B;
		else if (strncmp (rdBuffer, queryResponse4, strlen(queryResponse4)) == 0)
			gModelNumber = AUMG369XB_MODEL_MG3694B;
		else if (strncmp (rdBuffer, queryResponse5, strlen(queryResponse5)) == 0)
			gModelNumber = AUMG369XB_MODEL_MG3695B;
		else if (strncmp (rdBuffer, queryResponse6, strlen(queryResponse6)) == 0)
			gModelNumber = AUMG369XB_MODEL_MG3696B;
		else
			CHECKERR (VI_ERROR_FAIL_ID_QUERY);
    }
	
	/*- Reset instrument --------------------------------------------------------------*/
    if (resetDevice) 
	{
        CHECKERR (aumg369xb_reset (*vi));
	}
	
	/*- Send Default Instrument Setup -------------------------------------------------*/	
    else
	{
        CHECKERR (aumg369xb_defaultInstrSetup (*vi));
	}
    
	return VI_SUCCESS;  	

Error:
        
    if (*vi != VI_NULL)
    {
        aumg369xb_close(*vi);
        *vi = VI_NULL;
    }
    
    return status;	
}


/*-------------------------------------------------------------------------------------*/
/*- Configuration Functions -----------------------------------------------------------*/
/*-------------------------------------------------------------------------------------*/

/*- RF Configuration ------------------------------------------------------------------*/

/*=====================================================================================*/
/* Function: Configure Output Enabled												   */	
/*																			           */
/* Purpose:  This function turns on or turns off the RF output.						   */															   
/*=====================================================================================*/
/*
Specifies whether to turn on the RF output.

Valid Values:
VI_FALSE (0) - Turns off the RF output.
VI_TRUE  (1) - Turns on the RF output. (Default Value)

*/
ViStatus _VI_FUNC aumg369xb_configureOutputEnabled (ViSession vi,
					ViBoolean outputEnabled)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "RF%d;", outputEnabled ? 1 : 0));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure RF															   */	
/*																			           */
/* Purpose:  This function configures the RF parameters, including the RF state during */
/*           retrace, the RF state during frequency switching, the ramp rest point for */
/*           sweeps that are triggered by a single trigger or external trigger, the RF */
/*           state at reset, the frequency scaling reference multiplier and the        */
/*           attenuation when RF is switched off.			                           */												   
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureRF (ViSession vi, 
					ViBoolean retraceRFState, ViBoolean DeltaFRFState,
					ViInt32 rampRest, ViBoolean resetState, 
					ViReal64 frequencyScaling, ViBoolean terminateRFState)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "RT%d, RC%d, RR%d, RO%d;", retraceRFState ? 1 : 0, 
				DeltaFRFState ? 1 : 0, rampRest, resetState ? 0 : 1));

	CHECKERR (viPrintf (vi, "FRS %f TMS, TR%d;", frequencyScaling, terminateRFState ? 1 : 0));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*- Frequency Configuration -----------------------------------------------------------*/

/*=====================================================================================*/
/* Function: Configure Frequency List												   */	
/*																			           */
/* Purpose:  This function sets the frequency list, including the F0 to F9 and M0 to M9*/
/*           parameters.                                                               */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureFrequencyList (ViSession vi, 
					ViReal64 frequencyList[20])
{
	ViStatus	status = VI_SUCCESS;
	ViInt32		i;

	/*- Check input parameter ranges --------------------------------------------------*/	
	CHECKPARAM (aumg369xb_invalidPointer (frequencyList), VI_ERROR_PARAMETER2);
	/*---------------------------------------------------------------------------------*/

	for (i = 0; i < 10; i++)
	{
		CHECKERR (viPrintf (vi, "F%d %Lf HZ;", i, frequencyList[i]));
		CHECKERR (viPrintf (vi, "M%d %Lf HZ;", i, frequencyList[i + 10]));
	}
	
	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;	
}


/*=====================================================================================*/
/* Function: Configure Frequency Entry												   */	
/*																			           */
/* Purpose:  This function sets the selected frequency.                                */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureFrequencyEntry (ViSession vi, 
					ViInt32 selectedFrequency, ViReal64 frequency)
{
	ViStatus	status = VI_SUCCESS;

	if (selectedFrequency < 10)
	{
		CHECKERR (viPrintf (vi, "F%d %Lf HZ;", selectedFrequency, frequency));
	}
	else
	{
		CHECKERR (viPrintf (vi, "M%d %Lf HZ;", selectedFrequency - 10, frequency));
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;	
}


/*=====================================================================================*/
/* Function: Configure Delta Frequency												   */	
/*																			           */
/* Purpose:  This function sets the delta frequency.	           			           */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureDeltaFrequency (ViSession vi,
					ViReal64 deltaFrequency)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "DFM %Lf HZ;", deltaFrequency));	

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;	
}


/*=====================================================================================*/
/* Function: Configure CW Frequency													   */	
/*																			           */
/* Purpose:  This function places the MG369XB in CW mode and configures the CW         */
/*           frequency parameters, including the current frequency, the phase offset   */
/*           and the CW ramp.
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureCWFrequency (ViSession vi,
					ViInt32 selectedFrequency, ViBoolean phaseOffsetEnabled, 
					ViReal64 phaseOffset, ViBoolean CWRampEnabled)
{
	ViStatus	status = VI_SUCCESS;
	
	if (selectedFrequency < 10)
	{
		CHECKERR (viPrintf (vi, "CF%d;", selectedFrequency));
	}
	else
	{
		CHECKERR (viPrintf (vi, "CM%d;", selectedFrequency - 10));
	}

	CHECKERR (viPrintf (vi, "PS%d;", phaseOffsetEnabled ? 1 : 0));

	if (phaseOffsetEnabled)
	{
		CHECKERR (viPrintf (vi, "PSO %f DG;", phaseOffset));
	}

	CHECKERR (viPrintf (vi, "CS%d;", CWRampEnabled ? 1 : 0));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;	
}


/*=====================================================================================*/
/* Function: Configure Fast Frequency Switching										   */
/*																			           */
/* Purpose:  This function configures the fast-frequency-switching parameters,         */
/*           including the table location, the frequency list, and the table pointer.  */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureFastFrequencySwitching (ViSession vi,
					ViInt32 tableLocation, ViInt32 numberOfFrequency,
					ViReal64 frequencyList[], ViInt32 pointerLocation)
{
	ViStatus	status = VI_SUCCESS;

	ViChar		command_str[100000] = "";
	ViInt32		i;

	/*- Check input parameter ranges --------------------------------------------------*/	
	CHECKPARAM (aumg369xb_invalidPointer (frequencyList), VI_ERROR_PARAMETER3);
	/*---------------------------------------------------------------------------------*/

	sprintf(command_str, "ZL%03d", tableLocation);

	for (i = 0; i < numberOfFrequency; i++)
	{
		sprintf(command_str, "%s %Lf HZ", command_str, frequencyList[i]);
	}

	sprintf(command_str + strlen(command_str), "ZEL");

	CHECKERR (viPrintf (vi, command_str));

	CHECKERR (viPrintf (vi, "ZS%03d", pointerLocation));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;	
}


/*=====================================================================================*/
/* Function: Configure Analog Sweep Frequency										   */
/*																			           */
/* Purpose:  This function sets up the MG369XB for a analog frequency sweep and        */
/*			 configures the analog sweep frequency parameters, including the sweep     */
/*			 range, the sweep time and the trigger source.					           */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureAnalogSweepFrequency (ViSession vi, 
					ViInt32 sweepRange, ViReal64 sweepTime, ViInt32 triggerSource)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "SWP, %s, SWT %f SEC, %s;", sweepRangeTable[sweepRange], 
				sweepTime, triggerSourceTable[triggerSource]));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;	
}


/*=====================================================================================*/
/* Function: Configure Step Sweep Frequency											   */
/*																			           */
/* Purpose:  This function sets up the MG369XB for a step frequency sweep and          */
/*			 configures the step sweep frequency parameters, including the step sweep  */
/*           range, the sweep mode, the dwell-time-per-step, the number of steps and   */
/*           the trigger source.                                                       */        
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureStepSweepFrequency (ViSession vi,  
					ViInt32 sweepRange, ViInt32 stepSweepMode, ViReal64 dwellTime, 
					ViInt32 numberOfSteps, ViInt32 triggerSource)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf(vi, "%s, %s, SDT %f SEC, SNS %d SPS, %s;", stepSweepModeTable[stepSweepMode], 
				sweepRangeTable[sweepRange], dwellTime, numberOfSteps, 
				triggerSourceTable[triggerSource]));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;	
}


/*=====================================================================================*/
/* Function: Configure Step Sweep Sweep Time										   */
/*																			           */
/* Purpose:  This function configures the time for the step sweep.					   */								                   
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureStepSweepSweepTime (ViSession vi,
					ViReal64 sweepTime)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "SWT %f SEC;", sweepTime));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;	
}


/*=====================================================================================*/
/* Function: Configure Alternate Sweep Frequency									   */
/*																			           */
/* Purpose:  This function configures the alternate sweep frequency parameters,        */
/*			 including the sweep range and the power level.						       */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureAlternateSweepFrequency (ViSession vi, 
					ViInt32 sweepRange, ViInt32 powerLevel)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "%s, AL%d;", alternateSweepRangeTable[sweepRange], powerLevel));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure Dual Step Sweep Enabled										   */
/*																			           */
/* Purpose:  This function enables or disables the dual step sweep mode.               */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureDualStepSweepEnabled (ViSession vi,
					ViBoolean enabled)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "DU%d;", enabled ? 1 : 0));
	
	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure Non Equally Spaced Step Sweep Enabled						   */
/*																			           */
/* Purpose:  This function enables or disables the non-equally spaced step sweep.      */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureNonEquallySpacedStepSweepEnabled (ViSession vi,
					ViBoolean enabled)

{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "SP%d;", enabled ? 1 : 0));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure Manual Sweep Frequency										   */
/*																			           */
/* Purpose:  This function sets up the MG369XB for a manual sweep frequency and        */
/*           configures the manual sweep parameters, including the sweep range and the */
/*           number of steps.                                                          */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureManualSweepFrequency (ViSession vi,
					ViInt32 sweepRange, ViInt32 numberOfSteps)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "MAN, %s, SNS %d SPS, RL;", sweepRangeTable[sweepRange], 
				numberOfSteps));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure List Sweep Frequency											   */
/*																			           */
/* Purpose:  This function places the MG369XB in list sweep mode and configures the    */
/*           list sweep parameters, including the list number, the start index, the    */ 
/*           stop index, the dwell time and the trigger source.						   */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureListSweepFrequency (ViSession vi, 
					ViInt32 listNumber, ViInt32 startIndex, ViInt32 stopIndex, 
					ViReal64 dwellTime, ViInt32 triggerSource)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "LST, ELN%d, LIB%04d, LIE%04d, LDT %f SEC, %s;", listNumber, 
				startIndex, stopIndex, dwellTime, triggerSourceTable[triggerSource]));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure Sweep List											           */
/*																			           */
/* Purpose:  This function sets the frequencies and power levels of the selected list  */
/*			 starting at the list index.                                               */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureSweepList (ViSession vi, 
					ViInt32 listNumber, ViInt32 listIndex, 
					ViInt32 numberOfFrequency, ViReal64 frequencyList[], 
					ViInt32 numberOfPowerLevel, ViReal64 powerLevelList[])
{
	ViStatus	status = VI_SUCCESS;
	ViInt32		i;

	/*- Check input parameter ranges --------------------------------------------------*/	
	CHECKPARAM (aumg369xb_invalidPointer (frequencyList), VI_ERROR_PARAMETER4);
	CHECKPARAM (aumg369xb_invalidPointer (powerLevelList), VI_ERROR_PARAMETER6);
	/*---------------------------------------------------------------------------------*/

	CHECKERR (viPrintf (vi, "LST, ELN%d, ELI%04d;", listNumber, listIndex));

	CHECKERR (viPrintf (vi, "LF"));
	for (i = 0; i < numberOfFrequency; i++)
	{
		CHECKERR (viPrintf (vi, "%Lf HZ", frequencyList[i]));
	}

	CHECKERR (viPrintf (vi, "LP"));
	for (i = 0; i < numberOfPowerLevel; i++)
	{
		CHECKERR (viPrintf (vi, "%f DM", powerLevelList[i]));
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*- Master Slave Configuration --------------------------------------------------------*/

/*=====================================================================================*/
/* Function: Configure Master Slave Operation								           */
/*																			           */
/* Purpose:  This function turns on or turns off the master-slave mode of operation.   */
/*           If the master-slave mode is turned on, this function configures the slave */
/*           delta frequency, the slave main power level and the slave alternate sweep */
/*           power level.															   */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureMasterSlaveOperation (ViSession vi,
					ViBoolean masterSlaveEnabled, ViReal64 slaveDeltaFrequency, 
					ViReal64 slaveMainPowerLevel, ViReal64 slaveAlternateSweepPowerLevel)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "S%d;", masterSlaveEnabled ? 1 : 0));

	if (masterSlaveEnabled)
	{
		CHECKERR (viPrintf (vi, "SLDF %Lf HZ;", slaveDeltaFrequency));

		if (gPowerLevelUnit == AUMG369XB_UNIT_LINEAR_MV)
		{
			CHECKERR (viPrintf (vi, "SLL1 %f VT, SLL2 %f VT;", slaveMainPowerLevel,
						slaveAlternateSweepPowerLevel));
		}
		else
		{
			CHECKERR (viPrintf (vi, "SLL1 %f DM, SLL2 %f DM;", slaveMainPowerLevel,
						slaveAlternateSweepPowerLevel));
		}
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure Slave Frequency List									           */
/*																			           */
/* Purpose:  This function configures the frequency list of the slave unit.			   */                                               
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureSlaveFrequencyList (ViSession vi,
					ViReal64 frequencyList[20])
{
	ViStatus	status = VI_SUCCESS;
	ViInt32		i;

	/*- Check input parameter ranges --------------------------------------------------*/	
	CHECKPARAM (aumg369xb_invalidPointer (frequencyList), VI_ERROR_PARAMETER2);
	/*---------------------------------------------------------------------------------*/

	for (i = 0; i < 10; i++)
	{
		CHECKERR (viPrintf (vi, "SLF%d %Lf HZ;", i, frequencyList[i]));
		CHECKERR (viPrintf (vi, "SLM%d %Lf HZ;", i, frequencyList[i + 10]));
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure Slave Frequency Entry								           */
/*																			           */
/* Purpose:  This function configures the selected slave frequency.			           */                                               
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureSlaveFrequencyEntry (ViSession vi,
					ViInt32 selectedFrequency, ViReal64 frequency)
{
	ViStatus	status = VI_SUCCESS;

	if (selectedFrequency < 10)
	{
		CHECKERR (viPrintf (vi, "SLF%d %Lf HZ;", selectedFrequency, frequency));
	}
	else
	{
		CHECKERR (viPrintf (vi, "SLM%d %Lf HZ;", selectedFrequency - 10, frequency));
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure VNA Mode												           */
/*																			           */
/* Purpose:  This function configures the frequency offset for the Slave instrument in */
/*           the VNA mode.                                                             */
/*=====================================================================================*/
ViStatus  _VI_FUNC aumg369xb_configureVNAMode (ViSession vi,
					ViReal64  VNAOffset)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "SOF %Lf HZ;", VNAOffset));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure Frequency Marker Enabled								           */
/*																			           */
/* Purpose:  This function enables or disables the marker at the active frequency.	   */                                               
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureFrequencyMarkerEnabled (ViSession vi,
					ViInt32 activeFrequency, ViBoolean frequencyMarkerEnabled)

{
	ViStatus	status = VI_SUCCESS;
	
	if (activeFrequency < 10)
	{
		CHECKERR (viPrintf (vi, "F%d ME%d;", activeFrequency, 
					frequencyMarkerEnabled ? 1 : 0));
	}
	else
	{
		CHECKERR (viPrintf (vi, "M%d ME%d;", activeFrequency - 10, 
					frequencyMarkerEnabled ? 1 : 0));
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure Frequency Marker Mode								           */
/*																			           */
/* Purpose:  This function configure the frequency marker mode.                        */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureFrequencyMarkerMode (ViSession vi,
					ViInt32 frequencyMarkerMode)

{
	ViStatus	status = VI_SUCCESS;
	
	CHECKERR (viPrintf (vi, "%s;", frequencyMarkerModeTable[frequencyMarkerMode]));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*- Power Level Configuration ---------------------------------------------------------*/

/*=====================================================================================*/
/* Function: Configure Power Level Units											   */	
/*																			           */
/* Purpose:  This function configures the power level unit.		       		           */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configurePowerLevelUnit (ViSession vi,
					ViInt32 powerLevelUnit)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "%s", powerLevelUnitTable[powerLevelUnit]));

	gPowerLevelUnit = powerLevelUnit;	

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure Power Level List												   */	
/*																			           */
/* Purpose:  This function configures the power level list.       					   */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configurePowerLevelList (ViSession vi, 
					ViReal64 powerLevelList[10])
{
	ViStatus	status = VI_SUCCESS;
	ViInt32		i;

	/*- Check input parameter ranges --------------------------------------------------*/	
	CHECKPARAM (aumg369xb_invalidPointer (powerLevelList), VI_ERROR_PARAMETER2);
	/*---------------------------------------------------------------------------------*/

	if (gPowerLevelUnit == AUMG369XB_UNIT_LINEAR_MV)
	{
		for (i = 0; i < 10; i++)
		{
			CHECKERR (viPrintf (vi, "XL%d %f VT;", i, powerLevelList[i]));
		}
	}
	else
	{
		for (i = 0; i < 10; i++)
		{
			CHECKERR (viPrintf (vi, "XL%d %f DM;", i, powerLevelList[i]));
		}
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure Power Level Entry											   */	
/*																			           */
/* Purpose:  This function sets the selected power level.           		           */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configurePowerLevelEntry (ViSession vi, 
					ViInt32 selectedPowerLevel, ViReal64 powerLevel)
{
	ViStatus	status = VI_SUCCESS;
	
	if (gPowerLevelUnit == AUMG369XB_UNIT_LINEAR_MV)
	{
		CHECKERR (viPrintf (vi, "XL%d %f VT;", selectedPowerLevel, powerLevel));
	}
	else
	{
		CHECKERR (viPrintf (vi, "XL%d %f DM;", selectedPowerLevel, powerLevel));
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure Fixed Power Level											   */	
/*																			           */
/* Purpose:  This function places the MG369XB in a fixed power level mode and          */
/*			 configures the power level and the level offset.						   */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureFixedPowerLevel (ViSession vi, 
					ViInt32 selectedPowerLevel, ViBoolean levelOffsetEnabled, 
					ViReal64 levelOffset)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "L%d, LO%d;", selectedPowerLevel, levelOffsetEnabled ? 1 : 0));

	if (levelOffsetEnabled)
	{
		if (gPowerLevelUnit == AUMG369XB_UNIT_LINEAR_MV)
		{
			CHECKERR (viPrintf (vi, "LOS %f VT;", levelOffset));
		}
		else
		{
			CHECKERR (viPrintf (vi, "LOS %f DM;", levelOffset));
		}
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure 1 dB Below Peak Leveled Power								   */	
/*																			           */
/* Purpose:  This function sets the instrument power level to 1 dB below peak leveled  */
/*           power.																	   */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configure1dBBelowPeakLeveledPower (ViSession vi)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "LVP;"));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;	
}


/*=====================================================================================*/
/* Function: Configure Power Level Sweep											   */	
/*																			           */
/* Purpose:  This function places the MG369XB in a power level sweep mode and		   */
/*			 configures the dwellTime, the number of steps and the trigger source.	   */	                                                   
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configurePowerLevelSweep (ViSession vi,
					ViReal64 dwellTime, ViInt32 numberOfSteps, ViInt32 triggerSource)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "LSP, PDT %f SEC, PNS %d SPS, %s;", dwellTime, numberOfSteps, 
				triggerSourceTable[triggerSource]));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;	
}


/*=====================================================================================*/
/* Function: Configure ALC Mode														   */	
/*																			           */
/* Purpose:  This function configures the leveling mode and the attenuation, enables or*/
/*           disables decoupling the step attenuator from the ALC. This function       */
/*           permits setting of the Reference Level DAC to control the ALC gain when in*/
/*           external power leveling mode.                                             */ 
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureALCMode (ViSession vi,
					ViInt32 levelingMode, ViBoolean decoupleEnabled, 
					ViInt32 attenuation, ViInt32 referenceLevelDAC)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "%s, AT%d;", levelingModeTable[levelingMode], 
				decoupleEnabled ? 1 : 0));

	if (decoupleEnabled)
	{
		CHECKERR (viPrintf (vi, "ATT%02d;", attenuation));
	}

	if (levelingMode == AUMG369XB_LEVELING_MODE_EXTERNAL_DETECTOR ||
		levelingMode == AUMG369XB_LEVELING_MODE_POWER_METER)
	{
		CHECKERR (viPrintf (vi, "EGI %d SPS;", referenceLevelDAC));
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;	
}


/*=====================================================================================*/
/* Function: Configure ALC Loop														   */	
/*																			           */
/* Purpose:  This function turns on or turns off the ALC power slope function and      */
/*           configures the the pivot point frequency and the ALC power slope.         */                                                                
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureALCLoop (ViSession vi, 
					ViReal64 pivot, ViBoolean slopeEnabled, ViInt32 slope)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "PVT %Lf HZ, SL%d;", pivot, slopeEnabled ? 1 : 0));

	if (slopeEnabled)
	{
		CHECKERR (viPrintf (vi, "SLP %d SPS;", slope));
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure Power Offset Table									           */	
/*																			           */
/* Purpose:  This function enables or disables the power offset table and loads  a     */
/*			 power offset table.													   */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configurePowerOffsetTable (ViSession vi,
					ViBoolean powerOffsetTableEnabled, ViInt32 numberOfPowerOffset, 
					ViReal64 powerOffsetList[])
{
	ViStatus	status = VI_SUCCESS;
	ViChar		command_str[10000];
	ViChar		*ptr = VI_NULL;
	ViInt32		i;
	ViUInt32	returnCount;
	ViInt16		powerLevel;

	/*- Check input parameter ranges --------------------------------------------------*/	
	CHECKPARAM (aumg369xb_invalidPointer (powerOffsetList), VI_ERROR_PARAMETER3);
	/*---------------------------------------------------------------------------------*/

	if (powerOffsetTableEnabled)
	{
		command_str[0] = 'P';
		command_str[1] = 'T';
		command_str[2] = 'L';

		ptr = (ViChar*)(&numberOfPowerOffset);
		command_str[3] = (ViChar)(*ptr);
		command_str[4] = (ViChar)(*(ptr + 1));

		for (i = 0; i < numberOfPowerOffset; i++)
		{
			powerLevel = (ViInt16)(100 * powerOffsetList[i]);
			ptr = (ViChar*)(&powerLevel);
			command_str[5 + 2 * i] = (ViChar)(*ptr);
			command_str[6 + 2 * i] = (ViChar)(*(ptr + 1));
		}

		CHECKERR (viWrite (vi, command_str, 5 + numberOfPowerOffset * 2, &returnCount));
	}

	CHECKERR (viPrintf (vi, "PT%d;", powerOffsetTableEnabled ? 1 : 0));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*- Modulation Configuration ----------------------------------------------------------*/

/*=====================================================================================*/
/* Function: Configure Internal AM													   */	
/*																			           */
/* Purpose:  This function turns on or turns off the selected internal AM function     */
/*           and configures the depth, the rate and the waveform.                      */     
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureInternalAM (ViSession vi,
					ViBoolean InternalAMEnabled, ViInt32 scaling, ViReal64 depth, 
					ViReal64 rate, ViInt32 waveform)
{
	ViStatus	status = VI_SUCCESS;

	if (InternalAMEnabled)
	{
		if (scaling == AUMG369XB_AM_SCALING_LINEAR)
		{
			CHECKERR (viPrintf (vi, "AM7, ADP1 %f PCT;", depth));
		}
		else
		{
			CHECKERR (viPrintf (vi, "AM8, ADP2 %f DB;", depth));
		}		

		CHECKERR (viPrintf (vi, "AMR %Lf HZ, AMW%d;", rate, waveform));
	}
	else
	{
		CHECKERR (viPrintf (vi, "AM0;"));
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure External AM													   */	
/*																			           */
/* Purpose:  This function turns on or turn off the selected external AM function and  */
/*           configures the sensitivity.											   */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureExternalAM (ViSession vi,
					ViBoolean ExternalAMEnabled, ViInt32 scaling, ViReal64 sensitivity)

{
	ViStatus	status = VI_SUCCESS;

	if (ExternalAMEnabled)
	{
		if (scaling == AUMG369XB_AM_SCALING_LINEAR)
		{
			CHECKERR (viPrintf (vi, "AM1, AMS %f PCV;", sensitivity));
		}
		else
		{
			CHECKERR (viPrintf (vi, "AM2, ASD %f DV;", sensitivity));
		}
	}
	else
	{
		CHECKERR (viPrintf (vi, "AM0;"));
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure Internal FM													   */	
/*																			           */
/* Purpose:  This function turns on or turns off the selected internal FM mode and     */   
/*           configures the	deviation, the rate and the waveform.                      */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureInternalFM (ViSession vi,
					ViBoolean InternalFMEnabled, ViInt32 FMMode, ViReal64 deviation, 
					ViReal64 rate, ViInt32 waveform)
{
	ViStatus	status = VI_SUCCESS;

	if (InternalFMEnabled)
	{
		CHECKERR (viPrintf (vi, "%s, FDV %Lf HZ, FMR %Lf HZ, FWV%d;", 
					internalFMModeTable[FMMode], deviation, rate, waveform));
	}
	else
	{
		CHECKERR (viPrintf (vi, "FM0;"));
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure External FM													   */	
/*																			           */
/* Purpose:  This function turns on or turns off the selected external FM mode and     */
/*           configures the sensitivity.								               */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureExternalFM (ViSession vi, 
					ViBoolean ExternalFMEnabled, ViInt32 FMMode, ViReal64 sensitivity)
{
	ViStatus	status = VI_SUCCESS;

	if (ExternalFMEnabled)
	{
		CHECKERR (viPrintf (vi, "%s, FMS %Lf KV;", externalFMModeTable[FMMode], sensitivity));
	}
	else
	{
		CHECKERR (viPrintf (vi, "FM0;"));
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure Internal PM							                           */	
/*																			           */
/* Purpose:  This function turns on or turns off the selected internal PM and          */ 
/*           configures the deviation, the rate and the waveform.                      */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureInternalPM (ViSession vi,
					ViBoolean InternalPMEnabled, ViInt32 PMMode, ViReal64 deviation, 
					ViReal64 rate, ViInt32 waveform)
{
	ViStatus	status = VI_SUCCESS;
	
	if (InternalPMEnabled)
	{
		CHECKERR (viPrintf (vi, "%s, PHD %f RD, PHR %Lf HZ, PHV%d;", 
					internalPMModeTable[PMMode], deviation, rate, waveform));
	}
	else
	{
		CHECKERR (viPrintf (vi, "PH0;"));
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure External PM													   */	
/*																			           */
/* Purpose:  This function turns on or turns off the selected external PM and          */
/*           configures the sensitivity.                                               */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureExternalPM (ViSession vi,
					ViBoolean ExternalPMEnabled, ViInt32 PMMode, ViReal64 sensitivity)
{
	ViStatus	status = VI_SUCCESS;
	if (ExternalPMEnabled)
	{
		CHECKERR (viPrintf (vi, "%s, PHS %f RV;", externalPMModeTable[PMMode], sensitivity));
	}
	else
	{
		CHECKERR (viPrintf (vi, "PH0;"));
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure Internal Pulse Modulation							           */	
/*																			           */
/* Purpose:  This function turns on or turns off the selected internal pulse modulation*/ 
/*           and configures	the period, the trigger type, the trigger edge type, the   */
/*           polarity of the signal that turns the RF on and the clock rate.           */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureInternalPulseModulation (ViSession vi,
					ViBoolean InternalPulseModulationEnabled, ViReal64 period, ViInt32 mode, 
					ViInt32 triggerType, ViInt32 triggerEdgeType, 
					ViInt32 RFOnPolarity, ViInt32 clockRate)
{
	ViStatus	status = VI_SUCCESS;

	if (InternalPulseModulationEnabled)
	{
		CHECKERR (viPrintf (vi, "IP, PER %Lf SEC, PMD%d;", period, mode));

		CHECKERR (viPrintf (vi, "PTG%d, EP%d, PC%d;", triggerType, RFOnPolarity, clockRate));

		if (triggerType == AUMG369XB_TRIGGER_TYPE_GATED || 
			triggerType == AUMG369XB_TRIGGER_TYPE_TRIGGERED ||
			triggerType == AUMG369XB_TRIGGER_TYPE_TRIGGERED_WITH_DELAY)
		{
			CHECKERR (viPrintf (vi, "%s", triggerEdgeTypeTable[triggerEdgeType]));	
		}
	}
	else
	{
		CHECKERR (viPrintf (vi, "P0;"));
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure Internal Pulse Modulation Frequency					           */	
/*																			           */
/* Purpose:  This function configures the internal pulse frequency.					   */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureInternalPulseModulationFrequency (ViSession vi,
					ViReal64 frequency)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "PR %Lf HZ;", frequency));
	
	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure Internal Pulse Width									           */	
/*																			           */
/* Purpose:  This function configures the selected internal pulse width parameter.	   */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureInternalPulseWidth (ViSession vi,
					ViInt32 selectedPulseWidth, ViReal64 pulseWidth)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "W%d %Lf SEC;", selectedPulseWidth, pulseWidth));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure Internal Pulse Delay									           */	
/*																			           */
/* Purpose:  This function configures the selected internal pulse delay parameter.     */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureInternalPulseDelay (ViSession vi,
					ViInt32 selectedPulseDelay, ViReal64 pulseDelay)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "D%d %Lf SEC;", selectedPulseDelay, pulseDelay));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;	
}


/*=====================================================================================*/
/* Function: Configure Internal Pulse Step Delay							           */	
/*																			           */
/* Purpose:  This function enables or disables the step delay and configures the 	   */
/*			 start delay, the stop delay, the step size and the step time.			   */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureInternalPulseStepDelay (ViSession vi,
					ViBoolean enabled, ViReal64 startDelay, ViReal64 stopDelay, 
					ViReal64 stepSize, ViReal64 stepTime)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "SD%d;", enabled ? 1 : 0));

	if (enabled)
	{
		CHECKERR (viPrintf (vi, "SDS %f SEC, SDE %f SEC, SDD %f SEC, SDL %f SEC;", 
					startDelay, stopDelay, stepSize, stepTime));
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;	
}


/*=====================================================================================*/
/* Function: Configure Internal Square Wave Pulse Modulation				           */	
/*																			           */
/* Purpose:  This function turns on internal square wave pulse modulation.			   */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureInternalSquareWavePulseModulation (ViSession vi,
					ViInt32 squareWaveFrequency)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "SW%d;", squareWaveFrequency));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure External Pulse Modulation							           */	
/*																			           */
/* Purpose:  This function turns on or turns off the external pulse modulation and     */
/*           configures	the polarity of the signal (Low or High) that turns the RF on. */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureExternalPulseModulation (ViSession vi,
				  ViBoolean ExternalPulseModulationEnabled, ViInt32 RFOnPolarity)
{
	ViStatus	status = VI_SUCCESS;

	if (ExternalPulseModulationEnabled)
	{
		CHECKERR (viPrintf (vi, "XP, EP%d;", RFOnPolarity));
	}
	else
	{
		CHECKERR (viPrintf (vi, "P0;"));
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure SCAN Modulation Enabled								           */	
/*																			           */
/* Purpose:  This function turns on or turns off the SCAN modulation function.         */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureSCANModulationEnabled (ViSession vi,
					ViBoolean enabled)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "SC%d;", enabled ? 0 : 1));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*- Measurement Configuration ---------------------------------------------------------*/

/*=====================================================================================*/
/* Function: Configure AM Measurement Enabled								           */	
/*																			           */
/* Purpose:  This function turns on or turns off the AM measurement mode.              */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureAMMeasurementEnabled (ViSession vi,
					ViBoolean enabled)
{
	ViStatus	status = VI_SUCCESS;

	if (enabled)
	{
		CHECKERR (viPrintf (vi,	"AMI;"));
	}
	else
	{
		CHECKERR (viPrintf (vi,	"MOM;"));
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure FM Measurement Enabled								           */	
/*																			           */
/* Purpose:  This function turns on or turns off the FM measurement mode.              */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureFMMeasurementEnabled (ViSession vi,
					ViBoolean enabled)
{
	ViStatus	status = VI_SUCCESS;

	if (enabled)
	{
		CHECKERR (viPrintf (vi,	"FMD;"));
	}
	else
	{
		CHECKERR (viPrintf (vi,	"MOM;"));
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure Power Meter Measurement Enabled						           */	
/*																			           */
/* Purpose:  This function turns on or turns off the Power Meter measurement mode.     */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configurePowerMeterMeasurementEnabled (ViSession vi,
					ViBoolean enabled)
{
	ViStatus	status = VI_SUCCESS;

	if (enabled)
	{
		CHECKERR (viPrintf (vi,	"PM1;"));
	}
	else
	{
		CHECKERR (viPrintf (vi,	"PM0;"));
	}

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*- System Configuration --------------------------------------------------------------*/

/*=====================================================================================*/
/* Function: Configure Rear Panel											           */	
/*																			           */
/* Purpose:  This function configures the rear panel parameters, including the retrace */ 
/*           and bandswitch blanking outputs polarity, the marker polarity, the        */ 
/*           internal penlift relay and the external reference loop bandwidth.         */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureRearPanel (ViSession vi, 
					ViInt32 blankingPolarity, ViInt32 markerPolarity,
					ViInt32 penliftRelay, ViInt32 externalReferenceBandwidth)
{
	ViStatus	status = VI_SUCCESS;
	
	CHECKERR (viPrintf(vi, "%s, %s, %s, EBW%d;", blankingPolarityTable[blankingPolarity],
				markerPolarityTable[markerPolarity], penliftRelayTable[penliftRelay], 
				externalReferenceBandwidth));
	
	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure Secure Mode Enabled											   */	
/*																			           */
/* Purpose:  This function turns on or turns off the secure mode.					   */															   
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureSecureModeEnabled (ViSession vi, 
					ViBoolean enabled)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "DS%d;", enabled ? 0 : 1));
	
	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Configure Group Execute Trigger										   */	
/*																			           */
/* Purpose:  This function configures MG369XB to execute selected command each time a  */
/*           GET message is received.                                                  */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureGroupExecuteTrigger (ViSession vi,
					ViInt32 executeCommand)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "%s;", executeCommandTable[executeCommand]));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*-------------------------------------------------------------------------------------*/
/*- Action/Status Functions -----------------------------------------------------------*/
/*-------------------------------------------------------------------------------------*/

/*=====================================================================================*/
/* Function: Scan Preset CW Frequency												   */
/*																			           */
/* Purpose:  This function scans to the next preset CW frequency.  			           */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_scanPresetCWFrequency (ViSession vi, 
					ViInt32 scanFrequencyType)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "%s;", scancwFrequencyTable[scanFrequencyType]));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;	
}


/*=====================================================================================*/
/* Function: Zero Phase Offset Display												   */
/*																			           */
/* Purpose:  This function zeros the phase offset display.		  			           */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_zeroPhaseOffsetDisplay (ViSession vi)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "PSZ;"));		

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;	
}


/*=====================================================================================*/
/* Function: Calculate Sweep List													   */	
/*																			           */
/* Purpose:  This function configures the MG369XB to perform calculations to set the   */
/*           frequency and power levels during the initial list sweep.                 */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_calculateSweepList (ViSession vi)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "LEA;"));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Set Internal Triggered Pulse with Delay					               */	
/*																			           */
/* Purpose:  This function sets the internal pulse mode to single, the internal pulse  */
/*           trigger to triggered with delay, turns on the internal pulse modulation   */
/*           function, and disables the external pulse modulation function, if         */
/*			 previously programmed.                                                    */  
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_setInternalTriggeredPulseWithDelay (ViSession vi)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "DPT;"));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Set Internal Gated Pulse									               */	
/*																			           */
/* Purpose:  This function sets the internal pulse mode to single, the internal pulse  */
/*           trigger to gated, turns on the internal pulse modulation function, and    */
/*           disables the external pulse modulation function, if previously programmed.*/
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_setInternalGatedPulse (ViSession vi)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "GP;"));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Set Internal 1kHz Square Wave Pulse									   */	
/*																			           */
/* Purpose:  This function sets the internal pulse width to 500ms, PRF to 1 kHz, pulse */
/*           mode to single, pulse trigger to free run, turns on the internal pulse    */
/*           modulation function, and disables the external pulse modulation function, */
/*           if previously programmed.                                                 */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_setInternal1kHzSquareWavePulse (ViSession vi)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "SQP;"));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Perform Master Reset													   */	
/*																			           */
/* Purpose:  This function performs a master reset of the instrument by setting all of */
/*           the instrument parameters to factory default.                             */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_performMasterReset (ViSession vi)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "MR;"));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Trigger Single Sweep													   */	
/*																			           */
/* Purpose:  This function triggers a single sweep. When single sweep triggering is    */
/*           selected for frequency, power level, or list sweeps, a single sweep       */
/*           starts when the TRG command is received.                                  */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_triggerSingleSweep (ViSession vi)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "TRG;"));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Reset SWeep															   */	
/*																			           */
/* Purpose:  This function resets the sweep to its start frequency (or power level),   */
/*           whenever the command is received while a single sweep is in progress.     */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_resetSweep (ViSession vi)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "RSS;"));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Step To Next Sweep														   */	
/*																			           */
/* Purpose:  This function advances sweep to the next step in dual step sweep mode.    */
/*           The dual step sweep mode provides for generating synchronized, step sweep */
/*           outputs for two MG369XBx at a frequency offset. When operating in the     */
/*           dual step sweep mode, the command, TSS, steps the sweep to the next point.*/
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_stepToNextSweep (ViSession vi)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "TSS;"));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Send GET Bus Message													   */	
/*																			           */
/* Purpose:  This function sends a Y to the MG369XB. Sending a Y is equivalent to      */
/*           sending a GET.
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_sendGETBusMessage (ViSession vi)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "Y;"));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*-------------------------------------------------------------------------------------*/
/*- Utility Functions -----------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------*/

/*=====================================================================================*/
/* Function: Reset                                                           		   */	
/*																			 		   */
/* Purpose:  This function resets the MG369XB to its default settings. Sending this    */
/*           command clears the current instrument setup.                              */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_reset (ViSession vi)
{
    ViStatus	status = VI_SUCCESS;

	/*-  Reset the instrument to a known state. ---------------------------------------*/
	
	CHECKERR (viPrintf(vi, "RST;"));
	
	CHECKERR (aumg369xb_defaultInstrSetup (vi));
	
	CHECKERR (aumg369xb_checkStatus (vi));	

Error:
    return status;
}

/*=====================================================================================*/
/* Function: Self Test                                                       		   */
/*																					   */
/* Purpose:  This function executes the instrument self-test and returns the result.   */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_selfTest (ViSession vi, ViInt32 *testResult,
					ViByte testResultBytes[6])
{
    ViStatus	status = VI_SUCCESS;
	ViChar		ch;
	ViInt32		returnCount;
	ViUInt32    oldTimeout;
	
	/*- Check input parameter ranges --------------------------------------------------*/
	CHECKPARAM (aumg369xb_invalidPointer (testResult), VI_ERROR_PARAMETER2);  
	CHECKPARAM (aumg369xb_invalidPointer (testResultBytes), VI_ERROR_PARAMETER3); 
	/*---------------------------------------------------------------------------------*/

	CHECKERR( viGetAttribute (vi, VI_ATTR_TMO_VALUE, &oldTimeout));      
	CHECKERR( viSetAttribute (vi, VI_ATTR_TMO_VALUE, 10000));

	CHECKERR (viQueryf (vi, "TST;", "%c", &ch));
	
	if (ch == 'P')
	{
		*testResult = 0;
	}
	else
	{
		*testResult = -1;
	}

	CHECKERR (viPrintf(vi, "OSR;"));
	CHECKERR (viRead (vi, testResultBytes, 6,  &returnCount));
	
	CHECKERR (aumg369xb_checkStatus (vi));	

Error:
	viSetAttribute (vi, VI_ATTR_TMO_VALUE, oldTimeout);

    return status;
}


/*=====================================================================================*/
/* Function: Error Query                                                     		   */
/*																					   */
/* Purpose:  This function queries the instrument last GPIB syntax error.			   */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_errorQuery (ViSession vi, 
					ViChar errorMessage[256])
{
    ViStatus	status = VI_SUCCESS;

	/*- Check input parameter ranges --------------------------------------------------*/	
    CHECKPARAM (aumg369xb_invalidPointer (errorMessage), VI_ERROR_PARAMETER2);
	/*---------------------------------------------------------------------------------*/

	CHECKERR (viQueryf (vi, "OSE;", "%[^\n]", errorMessage));	
	
	CHECKERR (aumg369xb_checkStatus (vi));	
	
Error:
    return status;
}


/*=====================================================================================*/
/* Function: Error Message                                                   		   */	
/*																			 		   */
/* Purpose:  This function translates the error return value from the instrument	   */
/*           driver into a user-readable string.     					               */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_errorMessage (ViSession vi, 
					ViStatus statusCode, ViChar errMessage[])
{
    ViStatus	status = VI_SUCCESS;
    ViInt32		i;
	
    static		aumg369xb_tStringValPair statusDescArray[] = {
        {VI_WARN_NSUP_ID_QUERY,					"WARNING: ID Query not supported"},
        {VI_WARN_NSUP_RESET,        			"WARNING: Reset not supported"},
        {VI_WARN_NSUP_SELF_TEST,    			"WARNING: Self-test not supported"},
        {VI_WARN_NSUP_ERROR_QUERY,  			"WARNING: Error Query not supported"},     
        {VI_WARN_NSUP_REV_QUERY,    			"WARNING: Revision Query not supported"},
        {VI_ERROR_PARAMETER1,   				"ERROR: Parameter 1 out of range"},
        {VI_ERROR_PARAMETER2,   				"ERROR: Parameter 2 out of range"},
        {VI_ERROR_PARAMETER3,   				"ERROR: Parameter 3 out of range"},
        {VI_ERROR_PARAMETER4,   				"ERROR: Parameter 4 out of range"},
        {VI_ERROR_PARAMETER5,   				"ERROR: Parameter 5 out of range"},
        {VI_ERROR_PARAMETER6,   				"ERROR: Parameter 6 out of range"},
        {VI_ERROR_PARAMETER7,   				"ERROR: Parameter 7 out of range"},
        {VI_ERROR_PARAMETER8,   				"ERROR: Parameter 8 out of range"},
        {VI_ERROR_FAIL_ID_QUERY,				"ERROR: Identification query failed"},
        {VI_ERROR_INV_RESPONSE, 				"ERROR: Interpreting instrument response"},
		
        {AUMG369XB_ERROR_SYNTAX_ERROR,			"ERROR: Syntax error"},
		{AUMG369XB_ERROR_PARAMETER_RANGE_ERROR,	"ERROR:	Parameter range error"},
		{AUMG369XB_ERROR_LOCK_ERROR,			"ERROR:	Lock error"},
		{VI_NULL, VI_NULL}
    };
	
	/*- Check input parameter ranges --------------------------------------------------*/
    CHECKPARAM (aumg369xb_invalidPointer (errMessage), VI_ERROR_PARAMETER3); 
	/*---------------------------------------------------------------------------------*/

    status = viStatusDesc (vi, statusCode, errMessage);
    if (status == VI_WARN_UNKNOWN_STATUS) 
	{
        for (i = 0; statusDescArray[i].stringName; i++) 
		{
            if (statusDescArray[i].stringVal == statusCode) 
			{
                sprintf (errMessage, "%s", statusDescArray[i].stringName); 
                return (VI_SUCCESS);
            }
        }
        sprintf (errMessage, "Unknown Error 0x%x[uw8p0]", statusCode);
        return (VI_WARN_UNKNOWN_STATUS);
    }
    
    status = VI_SUCCESS;
	
Error:	
    return status;
}


/*=====================================================================================*/
/* Function: Revision Query                                                  		   */	
/*																			 		   */
/* Purpose:  This function returns the driver and instrument revisions.      		   */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_revisionQuery (ViSession vi,
					ViChar driverRev[256], ViChar buildRev[256], 
					ViChar instrRev[256])
{
    ViStatus status = VI_SUCCESS;

	/*- Check input parameter ranges --------------------------------------------------*/
	CHECKPARAM (aumg369xb_invalidPointer (driverRev), VI_ERROR_PARAMETER2);
	CHECKPARAM (aumg369xb_invalidPointer (buildRev), VI_ERROR_PARAMETER3); 	
	CHECKPARAM (aumg369xb_invalidPointer (instrRev), VI_ERROR_PARAMETER4);
	/*---------------------------------------------------------------------------------*/

	CHECKERR (viQueryf (vi, "*IDN?;", "%*[^,],%*[^,],%*[^,],%[^\n]", instrRev));

    sprintf (driverRev, "%s", AUMG369XB_REVISION);
	sprintf (buildRev, "%s", AUMG369XB_BUILD_REVISION);
	
	CHECKERR (aumg369xb_checkStatus (vi));	

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Query Model Number                                                		   */	
/*																			 		   */
/* Purpose:  This function returns the model number of the instrument.      		   */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_queryModelNumber (ViSession vi,
					ViInt32 *modelNumber)
{
	ViStatus	status = VI_SUCCESS;
	
	*modelNumber = gModelNumber;

	return status;
}


/*=====================================================================================*/
/* Function: Configure Serial Number												   */	
/*																			           */
/* Purpose:  This function configures the instrument serial number.                    */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureSerialNumber (ViSession vi,
					ViChar serialNumber[])
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "SNR%sX;", serialNumber));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Return Local Control													   */	
/*																			           */
/* Purpose:  This function returns the instrument to local (front panel) control.      */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_returnLocalControl (ViSession vi)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "RL;"));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*- Instrument Setup Configuration ----------------------------------------------------*/

/*=====================================================================================*/
/* Function: Save Instrument Setup													   */	
/*																			           */
/* Purpose:  This function saves the current instrument setup to internal setup memory.*/															   
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_saveInstrumentSetup (ViSession vi,
					ViInt32 setupNumber)
{
	ViStatus	status = VI_SUCCESS;	

	CHECKERR (viPrintf (vi, "SSN%d;", setupNumber));
	
	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Recall Instrument Setup												   */	
/*																			           */
/* Purpose:  This function recalls the instrument setup stored in internal setup       */
/*           memory.                                                                   */															   
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_recallInstrumentSetup (ViSession vi,
					ViInt32 setupNumber)
{
	ViStatus	status = VI_SUCCESS;	

	CHECKERR (viPrintf (vi, "RSN%d;", setupNumber));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Recall Next Instrument Setup											   */	
/*																			           */
/* Purpose:  This function recalls the next stored instrument setup in sequence.	   */															   
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_recallNextInstrumentSetup (ViSession vi)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "SM;"));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Write Instrument Setup													   */	
/*																			           */
/* Purpose:  This function prepares the MG369XB to receive instrument setups.		   */															   
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_writeInstrumentSetup (ViSession vi,
					ViInt32 instrumentSetupType, ViInt32 arrayLength, 
					ViByte instrumentSetup[])
{
	ViStatus	status = VI_SUCCESS;
	ViUInt32	returnCount;

	/*- Check input parameter ranges --------------------------------------------------*/	
	CHECKPARAM (aumg369xb_invalidPointer (instrumentSetup), VI_ERROR_PARAMETER3);
	/*---------------------------------------------------------------------------------*/

	if (instrumentSetupType == AUMG369XB_INSTRUMENT_SETUP_CURRENT)
	{
		CHECKERR (viPrintf (vi, "RCF"));
	}
	else
	{
		CHECKERR (viPrintf (vi, "RCM"));
	}

	CHECKERR (viWrite(vi, instrumentSetup, arrayLength, &returnCount));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Read Instrument Setup													   */	
/*																			           */
/* Purpose:  This function outputs the instrument setups to the computer in a 4100-byte*/
/*           (approximately) binary data string.                                       */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_readInstrumentSetup (ViSession vi,
					ViInt32 instrumentSetupType, ViInt32 arrayLength, 
					ViByte instrumentSetup[], ViInt32 *returnLength)
{
	ViStatus	status = VI_SUCCESS;

	/*- Check input parameter ranges --------------------------------------------------*/	
	CHECKPARAM (aumg369xb_invalidPointer (instrumentSetup), VI_ERROR_PARAMETER3);
	CHECKPARAM (aumg369xb_invalidPointer (returnLength), VI_ERROR_PARAMETER5);
	/*---------------------------------------------------------------------------------*/
	
	if (instrumentSetupType == AUMG369XB_INSTRUMENT_SETUP_CURRENT)
	{
		CHECKERR (viPrintf (vi, "SAF;"));
	}
	else
	{
		CHECKERR (viPrintf (vi, "SAM;"));
	}

	CHECKERR (viRead(vi, instrumentSetup, arrayLength, returnLength));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*- User Level Calibration Configuration ----------------------------------------------*/

/*=====================================================================================*/
/* Function: Configure User Level Calibration										   */	
/*																			           */
/* Purpose:  This function activates the selected user level calibration table.        */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_configureUserLevelCalibration (ViSession vi,
					ViInt32 calibrationTableNumber)
{
	ViStatus	status = VI_SUCCESS;

	CHECKERR (viPrintf (vi, "LU%d;", calibrationTableNumber));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Write User Level Calibration Table										   */	
/*																			           */
/* Purpose:  This function readies the MG369XB to receive the five tables of user      */
/*           level calibration data that were stored in the controller by the LUS      */
/*			 command.                                                                  */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_writeUserLevelCalibrationTable (ViSession vi,
					ViInt32 arrayLength, ViByte calibrationData[])
{
	ViStatus	status = VI_SUCCESS;
	ViUInt32	returnCount;

	/*- Check input parameter ranges --------------------------------------------------*/	
	CHECKPARAM (aumg369xb_invalidPointer (calibrationData), VI_ERROR_PARAMETER2);
	/*---------------------------------------------------------------------------------*/

	CHECKERR (viPrintf (vi, "LUR"));

	CHECKERR (viWrite(vi, calibrationData, arrayLength, &returnCount));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Read User Level Calibration Table										   */	
/*																			           */
/* Purpose:  This function sends all five tables of of user level calibration data     */	 
/*			 to the controller where they are stored in a binary data file.            */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_readUserLevelCalibrationTable (ViSession vi,
					ViInt32 arrayLength,  ViByte calibrationData[], 
					ViInt32 *returnLength)
{
	ViStatus	status = VI_SUCCESS;

	/*- Check input parameter ranges --------------------------------------------------*/	
	CHECKPARAM (aumg369xb_invalidPointer (calibrationData), VI_ERROR_PARAMETER2);
	CHECKPARAM (aumg369xb_invalidPointer (returnLength), VI_ERROR_PARAMETER4);	
	/*---------------------------------------------------------------------------------*/

	CHECKERR (viPrintf (vi, "LUS;"));

	CHECKERR (viRead(vi, calibrationData, arrayLength, returnLength));

	CHECKERR (aumg369xb_checkStatus (vi));

Error:
	return status;
}


/*=====================================================================================*/
/* Function: Close                                                           		   */	
/*																			 		   */
/* Purpose:  This function closes the instrument.                 			           */
/*=====================================================================================*/
ViStatus _VI_FUNC aumg369xb_close (ViSession vi)
{
    ViStatus			status = VI_SUCCESS;
	ViSession			rmSession;
	
	CHECKERR (viGetAttribute (vi, VI_ATTR_RM_SESSION, &rmSession));	
	    
	CHECKERR (viClose (vi));
	CHECKERR (viClose (rmSession));
	
Error:
    return status;
}


/***************************************************************************************/
/*= UTILITY ROUTINES (Non-Exportable Functions) =======================================*/
/***************************************************************************************/

/*=====================================================================================*/
/* Function: Pointer value is VI_NULL - void *				                           */	
/*																			           */
/* Purpose:  This function checks an output parameter (pointer) to see if its value is */
/*           VI_NULL.	If the value is VI_NULL, the return value is VI_TRUE, otherwise*/
/*           the return value is VI_FALSE.				                               */
/*=====================================================================================*/
ViBoolean aumg369xb_invalidPointer (void *value)
{
	return ((value == VI_NULL) ? VI_TRUE : VI_FALSE); 
}


/***************************************************************************************/
/*= INSTRUMENT SPECIFIC UTILITY ROUTINE DECLARATIONS (Non-Exportable Functions) =======*/
/***************************************************************************************/

/*=====================================================================================*/
/* Function: Default Instrument Setup             			                           */	
/*																					   */
/* Purpose:  This function sends a default setup to the instrument. This function is   */
/*           called by the aumg369xb_reset operation and by the aumg369xb_init         */
/*			 function if the reset option has not been selected.  					   */
/*=====================================================================================*/
ViStatus aumg369xb_defaultInstrSetup (ViSession vi)
{
    ViStatus			status = VI_SUCCESS;

	/*---------------------------------------------------------------------------------*/
	/* SQ0: Disables the SRQ generation function.                                      */
	/* CSB:	Clears all GPIB status bytes.						                       */
	/*---------------------------------------------------------------------------------*/
    CHECKERR (viPrintf (vi, "SQ0, CSB;"));
	
Error:
    return status;
}


/*=====================================================================================*/
/* Function: Check Status                                   		   				   */	
/*																					   */
/* Purpose:  This function reads the Primary Status Byte to check for errors. 		   */
/*=====================================================================================*/
ViStatus aumg369xb_checkStatus (ViSession vi)
{
    ViStatus	status = VI_SUCCESS;

	ViChar		esrValue = 0;

	CHECKERR (viQueryf (vi, "OSB;", "%c", &esrValue));

	if ((esrValue & 0x20) != 0)
	{
		status = AUMG369XB_ERROR_SYNTAX_ERROR;
	}
	else if ((esrValue & 0x10) != 0)
	{
		status = AUMG369XB_ERROR_PARAMETER_RANGE_ERROR;
	}
	else if ((esrValue & 0x08) != 0)
	{
		status = AUMG369XB_ERROR_LOCK_ERROR;
	}

Error:    
	return status;
}


/***************************************************************************************/
/*=== END INSTRUMENT DRIVER SOURCE CODE ===============================================*/
/***************************************************************************************/
