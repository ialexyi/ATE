/*= EPM Series Power Meters =================================================*/
/* LabWindows/CVI 5.5 Instrument Driver                                      */
/* Original Release: June 2003                                               */
/* By: HS, ELCOM, a.s, Czech Republic                                        */
/*																			 */
/* Should you have any technical questions please contact 					 */
/* National Instruments driver support	     								 */
/*																			 */
/*		email:  instrument.driver@ni.com									 */
/*                                                                           */
/* Modification History: 	                                                 */
/*	10/2/2008 Rev 1.0.1 [JP]- removed the extra characters included in last  */
/*							  sprintf command in the age441x_error_message   */
/*							  function (%s<)which was causing a runtime error*/
/*							  (CAR #128631)									 */	
/*===========================================================================*/

/*= INCLUDES ================================================================*/
#include <visa.h>
#include <ansi_c.h>
#include "age441x.h"

/*= DEFINES =================================================================*/
#define age441x_REVISION    "Rev 1.0.0, 07/2003, CVI 5.5" /*  Instrument driver revision */
#define BUFFER_SIZE         512L         /* File I/O buffer size */

    
/*= MACROS ==================================================================*/
#define CHECKERR(fCal) if (age441x_status = (fCal), age441x_status < VI_SUCCESS) \
                            return age441x_status; else
#define CHECKOK(fCal) if (age441x_status >= VI_SUCCESS) age441x_status = fCal;\
                            
#define CHECKPARM(fCal, errCode) if (age441x_status = (fCal), age441x_status < VI_SUCCESS) {\
                                    if ((age441x_status>VI_ERROR_PARAMETER1) && (age441x_status<VI_ERROR_INSTR_PARAMETER12)) \
                                        return errCode; \
                                    else \
                                        return age441x_status; }\
                                 else	 
/*****************************************************************************/
/*= INSTRUMENT-DEPENDENT COMMAND ARRAYS =====================================*/
/*****************************************************************************/
static ViString	sensorArr[]		= {"A","B","D","H",VI_NULL};
static ViString switchArr[] 	= {"OFF","ON","ONCE",VI_NULL}; 
static ViString feedArr[]   	= {"AVER","PTAV","PEAK"};
static ViInt32 mratArr[]		= {20,40,200};
static ViString sensTypeArr[]	= {"ATYP","DTYP",VI_NULL};
static ViString measFuncArr[] 	= {"",":REL",":DIFF",":DIFF:REL",":RAT",":RAT:REL"};
static ViString unitArr[] 		= {"DBM","W"};
static ViInt32	channelArr[]	= {1,2,VI_NULL};
static ViString measureArr[]	= {"","CALC1","CALC2","CALC3","CALC4"};
static ViString TTLLimitArr[]	= {"LOW","HIGH"};
static ViString TTLLimit2Arr[]	= {"LOW","UPP"};
static ViString trigSourArr[]   = {"IMM","BUS","HOLD","EXT","INT1","INT2"};
static ViString slopeArr[]		= {"NEG","POS"};
static ViString bandArr[]		= {"OFF","LOW","MED","HIGH"};
static ViString sensorModeArr[]	= {"NORM","AVER"};
static ViString regArr []		= {"*SRE","*ESE","STAT:DEV","STAT:OPER","STAT:QUES","STAT:OPER:CAL","STAT:OPER:MEAS","STAT:OPER:TRIG",\
								   "STAT:OPER:SENS","STAT:OPER:LLF","STAT:OPER:ULF","STAT:QUES:POW","STAT:QUES:CAL","*ESR","*STB"}; 		
static ViString regPartArr [] 	= {":NTR",":PTR",":ENAB",":EVEN",":COND", VI_NULL}; 
//static ViInt32  csetArr[]		= {1,2};
static ViString tracResArr[]    = {"LRES","MRES","HRES"};

/*****************************************************************************/
/*= INSTRUMENT-DEPENDENT STATUS/RANGE STRUCTURE  ============================*/
/*****************************************************************************/
/* age441x_stringValPair is used in the age441x_errorMessage function          */
/* age441x_statusDataRanges is used to track session dependent status & ranges*/
/*===========================================================================*/
typedef struct  age441x_stringValPair
{
   ViStatus stringVal;
   ViString stringName;
}  age441x_tStringValPair;
 
struct age441x_statusDataRanges {
	ViBoolean   checkStatus;
    ViInt32		channelNumber;
    ViInt32		instrVersion;
    ViInt32		sensor1;
    ViInt32		sensor2;
    
    
};

typedef struct age441x_statusDataRanges *age441x_instrRange;

/*****************************************************************************/
/*= UTILITY ROUTINE DECLARATIONS (Non-Exportable Functions) =================*/
/*****************************************************************************/
static ViBoolean age441x_invalidViBooleanRange (ViBoolean val);
static ViBoolean age441x_invalidViInt16Range (ViInt16 val, ViInt16 min, ViInt16 max);
static ViBoolean age441x_invalidViInt32Range (ViInt32 val, ViInt32 min, ViInt32 max);
static ViBoolean age441x_invalidViUInt8Range (ViUInt8 val, ViUInt8 min, ViUInt8 max);
static ViBoolean age441x_invalidViUInt16Range (ViUInt16 val, ViUInt16 min, ViUInt16 max);
static ViBoolean age441x_invalidViUInt32Range (ViUInt32 val, ViUInt32 min, ViUInt32 max);
static ViBoolean age441x_invalidViReal32Range (ViReal32 val, ViReal32 min, ViReal32 max);
static ViBoolean age441x_invalidViReal64Range (ViReal64 val, ViReal64 min, ViReal64 max);
static ViBoolean age441x_invalidInstrVersion (ViSession instrumentHandle, ViInt32 measurement);
static ViStatus age441x_setViNode (ViSession instrumentHandle, ViString cmd);
static ViStatus age441x_setViNode_EX (ViSession instrumentHandle, ViString cmd);
static ViStatus age441x_getViNode (ViSession instrumentHandle, ViString cmd, ViString _VI_FAR string);        
static ViStatus age441x_setViBoolean (ViSession instrumentHandle, ViString cmd, ViBoolean value);
static ViStatus age441x_setViInt32 (ViSession instrumentHandle, ViString cmd, ViInt32, ViInt32 min, ViInt32 max);             
static ViStatus age441x_getViInt32 (ViSession instrumentHandle, ViString cmd, ViInt32 *value);
static ViStatus age441x_getViReal64 (ViSession instrumentHandle, ViString cmd, ViReal64 *value);

static ViStatus age441x_setViString (ViSession instrumentHandle, ViString cmd, ViChar value[]);
static ViStatus age441x_getViString (ViSession instrumentHandle, ViString cmd, ViString _VI_FAR string);
static ViStatus age441x_setViReal64Arr (ViSession instrumentHandle, ViString cmd, ViInt32 arraySize, ViReal64 value[]);
static ViStatus age441x_getViReal64Arr (ViSession instrumentHandle, ViString cmd, ViInt32 arraySize, ViReal64 *value);

static ViStatus age441x_setMeasString (ViSession instrumentHandle, ViInt32 meas, ViString cmd, ViChar value[]);
static ViStatus age441x_getMeasString (ViSession instrumentHandle, ViInt32 meas, ViString cmd, ViString value);
static ViStatus age441x_setMeasReal64 (ViSession instrumentHandle, ViInt32 meas, ViString cmd, ViReal64 value);
static ViStatus age441x_getMeasReal64 (ViSession instrumentHandle, ViInt32 meas, ViString cmd, ViReal64 *value);
static ViStatus age441x_setMeasInt32 (ViSession instrumentHandle, ViInt32 meas, ViString cmd, ViInt32 value);
static ViStatus age441x_getMeasInt32 (ViSession instrumentHandle, ViInt32 meas, ViString cmd, ViInt32 *value);
static ViStatus age441x_setMeasEnum (ViSession instrumentHandle, ViInt32 meas, ViString cmd, ViInt32 value, ViString _VI_FAR list[]);
static ViStatus age441x_setMeasBoolean (ViSession instrumentHandle, ViInt32 meas, ViString cmd, ViBoolean value);
static ViStatus age441x_getMeasBoolean (ViSession instrumentHandle, ViInt32 meas, ViString cmd, ViBoolean *value);
static ViStatus age441x_setMeasNode (ViSession instrumentHandle, ViInt32 meas, ViString cmd);

static ViStatus age441x_checkIndexInList (ViSession instrumentHandle, ViInt32 index, ViString list[], ViStatus statusIfError);
static ViInt32 age441x_getSensorFromUserData (ViSession instrumentHandle, ViInt32 channel);
										  	 
ViStatus age441x_waitOnVisaEvent (ViSession instrSession, ViEventType eventTypeIn, ViUInt32 tmoIn, ViPUInt16 STB);
static ViStatus age441x_sysStatus (ViSession instrSession);   
void age441x_delay (ViReal64 numberOfSeconds);  
static ViStatus age441x_ReadDataUnknownLength(ViSession instrumentHandle, ViChar **outBuffer, ViUInt32 *count);
static ViStatus age441x_buildMeasureCommand (ViSession instrumentHandle, ViInt32 measurement,
                                   					  ViInt32 measuringFunction, ViInt32 sensor1,
                                   					  ViInt32 sensor2, ViInt32 resolution, ViReal64	expectedValue,
                                   					  ViString	baseCmd, ViBoolean question, ViChar command[]);

static ViInt32	age441x_getViInt32ElementIndex (ViInt32 element, ViInt32* array);
static ViStatus age441x_waitOPC (ViSession instrSession, ViPString command, ViPString instrResp, ViInt32 timeout);
void age441x_SWAP_32(unsigned char *data, long points); 
/*****************************************************************************/
/*------ INSERT INSTRUMENT-DEPENDENT UTILITY ROUTINE DECLARATIONS HERE ------*/
/*****************************************************************************/
static ViStatus age441x_defaultInstrSetup (ViSession openInstrSession);    
static ViStatus age441x_initCleanUp (ViSession openRMSession, ViPSession openInstrSession, ViStatus currentStatus);


/*****************************************************************************/
/*====== USER-CALLABLE FUNCTIONS (Exportable Functions) =====================*/
/*****************************************************************************/

/*===========================================================================*/
/* Function: Initialize                                                      */
/* Purpose:  This function opens the instrument, queries the instrument      */
/*           for its ID, and initializes the instrument to a known state.    */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_init (ViRsrc resourceName, ViBoolean IDQuery,
                    ViBoolean resetDevice, ViPSession instrSession)
{
    ViStatus age441x_status = VI_SUCCESS;
    ViSession rmSession = 0;
    //ViUInt32 retCnt = 0;
    ViByte rdBuffer[BUFFER_SIZE];  
    ViUInt16 inter = VI_INTF_GPIB;      

    /*- Check input parameter ranges ----------------------------------------*/
    if (age441x_invalidViBooleanRange (IDQuery))
        return VI_ERROR_PARAMETER2;
    if (age441x_invalidViBooleanRange (resetDevice))
        return VI_ERROR_PARAMETER3;

    /*- Open instrument session ---------------------------------------------*/
    if ((age441x_status = viOpenDefaultRM (&rmSession)) < 0)
        return age441x_status;

    if ((age441x_status = viOpen (rmSession, resourceName, VI_LOAD_CONFIG, VI_NULL, instrSession)) < 0) {
        viClose (rmSession);
        return age441x_status;
    } 

    /*- Configure VISA Formatted I/O ----------------------------------------*/
    if ((age441x_status = viSetAttribute (*instrSession, VI_ATTR_TMO_VALUE, 15000)) < 0)
            return age441x_initCleanUp (rmSession, instrSession, age441x_status);
  
    if ((age441x_status = viSetBuf (*instrSession, VI_READ_BUF|VI_WRITE_BUF, 4000)) < 0)
            return age441x_initCleanUp (rmSession, instrSession, age441x_status);
  
    if ((age441x_status = viSetAttribute (*instrSession, VI_ATTR_WR_BUF_OPER_MODE,
                            VI_FLUSH_ON_ACCESS)) < 0)
            return age441x_initCleanUp (rmSession, instrSession, age441x_status);
    
    if ((age441x_status = viSetAttribute (*instrSession, VI_ATTR_RD_BUF_OPER_MODE,
                            VI_FLUSH_ON_ACCESS)) < 0)
            return age441x_initCleanUp (rmSession, instrSession, age441x_status);
    
	if ((age441x_status = viSetAttribute (*instrSession, VI_ATTR_TERMCHAR, '\n')) < 0)
		return age441x_initCleanUp (rmSession, instrSession, age441x_status);
	if ((age441x_status = viSetAttribute (*instrSession, VI_ATTR_TERMCHAR_EN, VI_TRUE)) < 0)
		return age441x_initCleanUp (rmSession, instrSession, age441x_status);
	/*- Disable end char asserting for write operation */		
	if ((age441x_status = viSetAttribute (*instrSession, VI_ATTR_SEND_END_EN, VI_FALSE)) < 0)
		return age441x_initCleanUp (rmSession, instrSession, age441x_status);
		
	/*- Serial Interface Support ------------------------------------------------*/
	if ((age441x_status = viGetAttribute (*instrSession, VI_ATTR_INTF_TYPE, &inter)) < 0)
        return age441x_initCleanUp (rmSession, instrSession, age441x_status);
        
    if (inter == VI_INTF_ASRL) /* Serial Port */
    {
        if ((age441x_status = viSetAttribute (*instrSession, VI_ATTR_ASRL_END_IN, VI_ASRL_END_TERMCHAR)) < 0)						
            return age441x_initCleanUp (rmSession, instrSession, age441x_status);
        if ((age441x_status = viFlush (*instrSession, VI_ASRL_IN_BUF)) < 0)
            return age441x_initCleanUp (rmSession, instrSession, age441x_status);
    }
    /* - Interface Specific Operation -*/
    switch (inter)
    {
    	case VI_INTF_ASRL:
    		if (age441x_status = viPrintf (*instrSession, "%s\n", ":SYST:COMM:SER:TRAN:ECHO OFF") < 0)
            	return age441x_initCleanUp (rmSession, instrSession, age441x_status);	
			break;
		default:
			break;
	}
    /*- Identification Query ------------------------------------------------*/
    if (IDQuery) {
        if (age441x_status = viPrintf (*instrSession, "*IDN?\n") < 0)
            return age441x_initCleanUp (rmSession, instrSession, age441x_status);
        if (age441x_status = viRead (*instrSession, rdBuffer, BUFFER_SIZE, VI_NULL) < 0)
            return age441x_status;													  
        if (strncmp(rdBuffer, "HEWLETT-PACKARD,E441", 20) && strncmp (rdBuffer, "AGILENT TECHNOLOGIES,E441", 25)&& strncmp (rdBuffer, "Agilent Technologies,E441", 25))  
            return age441x_initCleanUp (rmSession, instrSession, VI_ERROR_FAIL_ID_QUERY);
    }
/*- Reset instrument ----------------------------------------------------*/
    if (resetDevice) {
        if ((age441x_status = age441x_reset (*instrSession)) < 0)
            return age441x_initCleanUp (rmSession, instrSession, age441x_status);
    }       
    else  /*- Send Default Instrument Setup ---------------------------------*/
        if ((age441x_status = age441x_defaultInstrSetup (*instrSession)) < 0)
            return age441x_initCleanUp (rmSession, instrSession, age441x_status);
            
	CHECKERR (age441x_sysStatus(*instrSession));     
	
    return age441x_status;
}

/*===========================================================================*/

/*****************************************************************************/
/*-------- INSERT USER-CALLABLE INSTRUMENT-DEPENDENT ROUTINES HERE ----------*/
/*****************************************************************************/

/*===========================================================================*/
/* Function: Power Measurement				                                 */
/* Purpose:  This function configures the channel, inits measurement and	 */
/*			 returns measured value in specified units.					 	 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_powerMeasurement (ViSession instrumentHandle,
                                            ViInt32 channel, ViInt32 unit,
                                            ViReal64 lowerLimit,
                                            ViReal64 upperLimit,
                                            ViBoolean *limitFail,
                                            ViReal64 *value)
{
	ViStatus	age441x_status = VI_SUCCESS;
	
	CHECKERR (age441x_configureMeasurement (instrumentHandle, 1, AGE441X_MEAS_SINGLE, channel, AGE441X_CHANNEL_A, 3, unit));
	CHECKERR (age441x_configureLimits (instrumentHandle, 1, VI_TRUE, AGE441X_STATE_OFF, lowerLimit, upperLimit));
	CHECKERR (age441x_setInitiateContinuousState (instrumentHandle, channel, VI_FALSE));
	CHECKERR (age441x_configureTrigger (instrumentHandle, channel, VI_FALSE, AGE441X_TRIG_IMM));
	CHECKERR (age441x_read (instrumentHandle, 1, AGE441X_MEAS_SINGLE, value));
	age441x_delay (1);
	CHECKERR (age441x_getLimitsFailure (instrumentHandle, 1, limitFail, VI_NULL));
	
	return age441x_status;
}


/*===========================================================================*/
/* Function: Configure Input				                                 */
/* Purpose:  This function configures the input parameters.					 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_configureInput (ViSession instrumentHandle,
                                          ViInt32 channel, 
                                          ViReal64 frequency,
                                          ViInt32 measurementSpeed)
{
	ViStatus			age441x_status = VI_SUCCESS;
	age441x_instrRange 	instrPtr; 
	ViUInt16			intf;
	ViChar				buffer[BUFFER_SIZE];
	ViChar*				pbuffer=buffer;				
	ViInt32				tmp_channel;
	ViInt32				maxSpeed = 2;
	ViInt32				sensorType;
	
	CHECKERR (viGetAttribute (instrumentHandle, VI_ATTR_USER_DATA, &instrPtr));	
	CHECKERR (viGetAttribute (instrumentHandle, VI_ATTR_INTF_TYPE, &intf));
	
	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];
	if (age441x_invalidInstrVersion (instrumentHandle, tmp_channel))
		return VI_ERROR_PARAMETER2; 
	if (age441x_invalidViReal64Range (frequency, 1.000E+3, 999.999E+9))
		return VI_ERROR_PARAMETER3;
	sensorType = age441x_getSensorFromUserData (instrumentHandle, channel);
	if (sensorType < AGE441X_SENSOR_E) maxSpeed = 1;		
	if (age441x_invalidViInt32Range (measurementSpeed, 0, maxSpeed))
		return VI_ERROR_PARAMETER4;
			
	if ((intf == VI_INTF_ASRL) && (measurementSpeed == AGE441X_MEAS_FAST))
		return VI_ERROR_PARAMETER4;
		
	pbuffer += sprintf (pbuffer, ":SENS%ld:FREQ %Le", tmp_channel, frequency);
	pbuffer += sprintf (pbuffer, ";:SENS%ld:SPE %ld", tmp_channel, mratArr[measurementSpeed]);	
	
	return age441x_setViNode (instrumentHandle, buffer);		
}


/*===========================================================================*/
/* Function: Configure Input				                                 */
/* Purpose:  This function configures the input parameters.					 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_configureFrequency (ViSession instrumentHandle,
                                          ViInt32 channel, 
                                          ViReal64 frequency)
{
	ViStatus			age441x_status = VI_SUCCESS;
	age441x_instrRange 	instrPtr; 
	ViUInt16			intf;
	ViChar				buffer[BUFFER_SIZE];
	ViChar*				pbuffer=buffer;				
	ViInt32				tmp_channel;
	ViInt32				maxSpeed = 2;
	ViInt32				sensorType;
	
	CHECKERR (viGetAttribute (instrumentHandle, VI_ATTR_USER_DATA, &instrPtr));	
	CHECKERR (viGetAttribute (instrumentHandle, VI_ATTR_INTF_TYPE, &intf));
	
	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];
	if (age441x_invalidInstrVersion (instrumentHandle, tmp_channel))
		return VI_ERROR_PARAMETER2; 
	if (age441x_invalidViReal64Range (frequency, 1.000E+3, 999.999E+9))
		return VI_ERROR_PARAMETER3;
	sensorType = age441x_getSensorFromUserData (instrumentHandle, channel);
	if (sensorType < AGE441X_SENSOR_E) maxSpeed = 1;		
			
	pbuffer += sprintf (pbuffer, ":SENS%ld:FREQ %Le", tmp_channel, frequency);
	
	return age441x_setViNode_EX (instrumentHandle, buffer);		
}


/*===========================================================================*/
/* Function: Configure Channel Offset		                                 */
/* Purpose:  This function configures the input channel offset.   			 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_configureChannelOffset (ViSession instrumentHandle,
                                                  ViInt32 channel,
                                                  ViBoolean channelOffsetState,
                                                  ViReal64 channelOffset)
{
	//ViStatus			age441x_status = VI_SUCCESS;
	ViChar				buffer[BUFFER_SIZE];
	ViChar*				pbuffer=buffer;				
	ViInt32				tmp_channel;

	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];
	
	if (age441x_invalidInstrVersion (instrumentHandle, tmp_channel))
		return VI_ERROR_PARAMETER2; 
	if (age441x_invalidViBooleanRange (channelOffsetState))
		return VI_ERROR_PARAMETER3; 
	if (age441x_invalidViReal64Range (channelOffset, -100.0, 100.0))
		return VI_ERROR_PARAMETER4;
		
	pbuffer+=sprintf(pbuffer, ":SENS%ld:CORR:GAIN2:STAT %s", tmp_channel, switchArr[channelOffsetState]);	
	
	if (channelOffsetState)
		pbuffer+=sprintf(pbuffer, ";:SENS%ld:CORR:GAIN2:INP:MAGN %Le", tmp_channel, channelOffset);
	
	return age441x_setViNode (instrumentHandle, buffer);		
}


/*===========================================================================*/
/* Function: Configure Averaging Auto		                                 */
/* Purpose:  This function configures all parameters necessary for auto 	 */
/*			 averaging.														 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_configureAveragingAuto (ViSession instrumentHandle,
                                                  ViInt32 channel,
                                                  ViBoolean stepDetection)
{
	//ViStatus			age441x_status = VI_SUCCESS;
	ViChar				buffer[BUFFER_SIZE];
	ViChar*				pbuffer=buffer;
	ViInt32				tmp_channel;
	
	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];
	
	if (age441x_invalidInstrVersion (instrumentHandle, tmp_channel))
		return VI_ERROR_PARAMETER2;    
	if (age441x_invalidViBooleanRange (stepDetection))
		return VI_ERROR_PARAMETER3;

	pbuffer += sprintf (pbuffer, ":SENS%ld:AVER:SDET %s", tmp_channel, switchArr[stepDetection]);
	pbuffer += sprintf (pbuffer, ";:SENS%ld:AVER:COUN:AUTO ON", tmp_channel);	
	
	return age441x_setViNode (instrumentHandle, buffer);
}

/*===========================================================================*/
/* Function: Configure Averaging 			                                 */
/* Purpose:  This function configures the input averaging.				 	 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_configureAveraging (ViSession instrumentHandle,
                                              ViInt32 channel, 
                                              ViBoolean state,
                                              ViInt32 filterLength)
{
	ViStatus			age441x_status = VI_SUCCESS;
	ViInt32				tmp_channel; 
	
	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];

	if (age441x_invalidViBooleanRange (state))
		return VI_ERROR_PARAMETER3;
	if (age441x_invalidViInt32Range (filterLength, 1, 1024))
		return VI_ERROR_PARAMETER4;
	
	if (state)
		CHECKERR (age441x_setMeasReal64 (instrumentHandle, tmp_channel, ":SENS%ld:AVER:COUN", filterLength));
	else
		CHECKERR (age441x_setMeasBoolean (instrumentHandle, tmp_channel, ":SENS%ld:AVER:STAT", state));	

	return age441x_status;	
}

/*===========================================================================*/
/* Function: Linearity Correction 			                                 */
/* Purpose:  This function configures the type of linearity correction.	 	 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_linearityCorrection (ViSession instrumentHandle,
                                               ViInt32 channel, 
                                               ViInt32 type)
{
	//ViStatus			age441x_status = VI_SUCCESS;
	ViInt32				tmp_channel;
	
	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];
	if (age441x_invalidViInt32Range (type, 0, 1))
		return VI_ERROR_PARAMETER3;
	if ((age441x_getSensorFromUserData (instrumentHandle, channel) != AGE441X_SENSOR_8480) && (type == 1))
		return AGE441X_ERROR_BAD_SENSOR;

	return age441x_setMeasEnum (instrumentHandle, tmp_channel, ":SENS%ld:V2P", type, sensTypeArr);
}

/*===========================================================================*/
/* Function: Get Frequency Dependent Offset	                                 */
/* Purpose:  This function returns the frequency dependent offset currently  */
/*			 being applied.	 	 											 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_getFrequencyDependentOffset (ViSession instrumentHandle,
                                                       ViInt32 channel,
                                                       ViReal64 *offset)
{
	//ViStatus			age441x_status = VI_SUCCESS;
	ViInt32				tmp_channel;
	
	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];
	
	if (offset == VI_NULL)
		return VI_ERROR_PARAMETER3;
	
	return age441x_getMeasReal64 (instrumentHandle, tmp_channel, ":SENS%ld:CORR:FDOF:INP?", offset);		
}



/*===========================================================================*/
/* Function: Configure Limits                                                */
/* Purpose:  This function  configures the  parameters of limits.			 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_configureLimits (ViSession instrumentHandle,
                                           ViInt32 measurement, 
                                           ViBoolean state,
                                           ViInt32 autoClear,
                                           ViReal64 lowerLimit,
                                           ViReal64 upperLimit)
{
	//ViStatus			age441x_status = VI_SUCCESS;
	ViChar				buffer[BUFFER_SIZE];
	ViChar*     		pbuffer = buffer;
	
	
	if (age441x_invalidInstrVersion (instrumentHandle, measurement))
		return VI_ERROR_PARAMETER2;
	if (age441x_invalidViBooleanRange (state))
		return VI_ERROR_PARAMETER3;
	if (age441x_invalidViInt32Range (autoClear, 0, 2))
		return VI_ERROR_PARAMETER4;		
	if (age441x_invalidViReal64Range (lowerLimit, -180.0, 230.0))
		return VI_ERROR_PARAMETER5;
	if (age441x_invalidViReal64Range (upperLimit, -180.0, 230.0))
		return VI_ERROR_PARAMETER6;
	
	pbuffer += sprintf (pbuffer, "CALC%ld:LIM:CLE:AUTO %s", measurement, switchArr[autoClear]);
	pbuffer += sprintf (pbuffer, ";:CALC%ld:LIM:LOW %Le", measurement, lowerLimit);
	pbuffer += sprintf (pbuffer, ";:CALC%ld:LIM:UPP %Le", measurement, upperLimit);
	pbuffer += sprintf (pbuffer, ";:CALC%ld:LIM:STAT %s", measurement, switchArr[state]);
	
	return age441x_setViNode (instrumentHandle, buffer);
}

/*===========================================================================*/
/* Function: Configure Math                                                  */
/* Purpose:  This function sets the specified measurement to a single		 */
/*			 channel, difference or ratio measurement.						 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_configureMath (ViSession instrumentHandle,
                                         ViInt32 measurement, ViInt32 sensor1,
                                         ViInt32 operation, ViInt32 sensor2)
{
	//ViStatus	age441x_status = VI_SUCCESS;
	ViChar		buffer[BUFFER_SIZE];
	ViInt32		tmp_sens1,
				tmp_sens2;

	if (age441x_invalidViInt32Range (sensor1, 0, 1))
		return VI_ERROR_PARAMETER3;       
	tmp_sens1 = channelArr[sensor1];
	if (age441x_invalidViInt32Range (operation, 0, 2))
		return VI_ERROR_PARAMETER4;
	if (age441x_invalidViInt32Range (sensor2, 0, 1))
		return VI_ERROR_PARAMETER5;       
	tmp_sens2 = channelArr[sensor2];
	
	switch (operation)
	{
		//Single value
		case AGE441X_MATH_VALUE:
			sprintf (buffer, "(SENS%ld)", tmp_sens1);
			break;
		//Difference			
		case AGE441X_MATH_DIFF:	
			sprintf (buffer, "(SENS%ld-SENS%ld)", tmp_sens1, tmp_sens2);
			break;
		//Ration			
		case AGE441X_MATH_RATIO:	
			sprintf (buffer, "(SENS%ld/SENS%ld)", tmp_sens1, tmp_sens2);
			break;
	}
	
	return age441x_setMeasString (instrumentHandle, measurement, "CALC%ld:MATH", buffer);
}


/*===========================================================================*/
/* Function: Get Math Expressions                                            */
/* Purpose:  This function returns the available mathematical expressions.	 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_getMathExpressions (ViSession instrumentHandle,
                                              ViInt32 measurement,
                                              ViInt32 arraySize,
                                              ViInt32 sensor1[],
                                              ViInt32 expression[],
                                              ViInt32 sensor2[],
                                              ViInt32 *returnedValues)
{
	ViStatus	age441x_status = VI_SUCCESS;
	ViChar		listOfExpressions [BUFFER_SIZE];
	//ViChar		tmp_expr[BUFFER_SIZE];
	ViChar*		pbuff;
	ViChar		tmp_char;
	ViInt32		sens1,
				sens2,
				expr; 
	ViInt32		j;
	ViInt32		tmp;
	
	if (age441x_invalidInstrVersion (instrumentHandle, measurement))
		return VI_ERROR_PARAMETER2;   

	CHECKERR (age441x_getMeasString (instrumentHandle, measurement, "CALC%ld:MATH:CAT?", listOfExpressions));
	j=0;
	pbuff = strtok (listOfExpressions, "\"");
	while (pbuff != NULL)
	{
		if (strpbrk (pbuff, "-/"))
		{
			tmp=sscanf (pbuff, "(SENS%ld%cSENS%ld)", &sens1, &tmp_char, &sens2);
			expr =((tmp_char==(int)'-')?AGE441X_MATH_DIFF:AGE441X_MATH_RATIO);
		}
		else
		{
			sscanf (pbuff, "(SENS%ld)", &sens1);
			sens2 = 0;
			expr = AGE441X_MATH_VALUE;
		}
		if (j<arraySize)
		{
			sensor1[j]=age441x_getViInt32ElementIndex (sens1, channelArr);
			sensor2[j]=age441x_getViInt32ElementIndex (sens2, channelArr);      
			expression[j]=expr;
		}
		j++;
		pbuff = strtok (NULL, "\"");      
		pbuff = strtok (NULL, "\""); 
	};

	if (returnedValues) *returnedValues = j;
	
	return age441x_status;			
}

/*===========================================================================*/
/* Function: Configure Gain		                                             */
/* Purpose:  This function configures the display offset. 					 */
/*			 The display offset is applied the measurement signal after any  */
/*			 math calculation.	 											 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_configureGain (ViSession instrumentHandle,
                                         ViInt32 measurement, 
                                         ViBoolean state,
                                         ViReal64 value)
{
	ViStatus			age441x_status = VI_SUCCESS;

	if (age441x_invalidViBooleanRange (state))
		return VI_ERROR_PARAMETER3;
	if (age441x_invalidViReal64Range (value, -100.0, 100.0))
		return VI_ERROR_PARAMETER4;
	
	if (state)
		CHECKERR (age441x_setMeasReal64 (instrumentHandle,  measurement, "CALC%ld:GAIN:MAGN", value));
	else
		CHECKERR (age441x_setMeasBoolean (instrumentHandle, measurement, "CALC%ld:GAIN:STAT", state));
		
	return age441x_status;
}


/*===========================================================================*/
/* Function: Reset Ref. Value	                                             */
/* Purpose:  This function sets the current measured value as the reference  */
/*			 value.															 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_resetRefValue (ViSession instrumentHandle,
                                         ViInt32 measurement)
{
    return age441x_setMeasEnum (instrumentHandle, measurement, "CALC%ld:REL:AUTO", 2, switchArr); 
}

/*===========================================================================*/
/* Function: Set Ref. Value	State                                            */
/* Purpose:  This function determines whether a reference value is used.  	 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_setRefValueState (ViSession instrumentHandle,
                                            ViInt32 measurement, ViBoolean state)
{
	if (age441x_invalidViBooleanRange (state))
		return VI_ERROR_PARAMETER3;   

	return age441x_setMeasBoolean (instrumentHandle, measurement, "CALC%ld:REL:STAT", state);
}


/*===========================================================================*/
/* Function: Feed				                                             */
/* Purpose:  This function sets the input measurement mode to be fed to the  */
/*			 specified input on the CALC block. 	 						 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_feed (ViSession instrumentHandle, 
								ViInt32 measurement,
                                ViInt32 feed, 
                                ViInt32 type)
{
	ViStatus			age441x_status=VI_SUCCESS;
	age441x_instrRange 	instrPtr; 
	ViChar				buffer[BUFFER_SIZE];
	
	
	CHECKERR (viGetAttribute (instrumentHandle, VI_ATTR_USER_DATA, &instrPtr));	
	
	//Check instrument version, only E4416 or E4417 instruments supports this function
	
	if (instrPtr->instrVersion > 17)
		return AGE441X_ERROR_INVALID_INSTRUMENT;
		
	if (age441x_invalidInstrVersion (instrumentHandle, measurement))
		return VI_ERROR_PARAMETER2;   
	if (age441x_invalidViInt32Range (feed, 1, 2))
		return VI_ERROR_PARAMETER3;
	if (age441x_invalidViInt32Range (type, 0, 2))
		return VI_ERROR_PARAMETER3;
		
	sprintf (buffer, "CALC%ld:FEED%ld \"POW:%s\"", measurement, feed, feedArr[type]);
	
	return age441x_setViNode (instrumentHandle, buffer);
}

/*===========================================================================*/
/* Function: Configure Measurement                                           */
/* Purpose:  This function configures the channel measurement.			     */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_configureMeasurement (ViSession instrumentHandle,
                                                ViInt32 measurement,
                                                ViInt32 measuringFunction,
                                                ViInt32 sensor1, 
                                                ViInt32 sensor2,
                                                ViInt32 resolution, 
                                                ViInt32 unit)
{
	ViStatus			age441x_status=VI_SUCCESS;
	//age441x_instrRange 	instrPtr; 
	ViChar				buffer[BUFFER_SIZE];
	ViChar*				pbuffer=buffer;

	CHECKERR (age441x_buildMeasureCommand (instrumentHandle, measurement, measuringFunction, sensor1, sensor2, 
										   resolution, VI_NULL, ":CONF", VI_FALSE, buffer));
	
	pbuffer += strlen(buffer);
	pbuffer += sprintf (pbuffer, ";:UNIT%ld:POW %s", measurement, unitArr[unit]);
	
	return age441x_setViNode (instrumentHandle, buffer);
}

/*===========================================================================*/
/* Function: Set TTL Calibration State										 */
/* Purpose:  This function  enables and disables the rear panel TTL zero/cal */
/*			 inputs.        												 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_setTTLCalibrationState (ViSession instrumentHandle,
                                                  ViBoolean state)
{
	return age441x_setViBoolean (instrumentHandle, ":CAL1:ECON:STAT", state);		
}

/*===========================================================================*/
/* Function: Calibration Lockout											 */
/* Purpose:  This function enables/disables the zero/cal lockout facility. 	 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_calibrationLockout (ViSession instrumentHandle,
                                              ViInt32 channel, 
                                              ViBoolean state)
{
	//ViStatus			age441x_status = VI_SUCCESS;
	ViInt32				tmp_channel;
	
	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];
	if (age441x_invalidViBooleanRange (state))
		return VI_ERROR_PARAMETER3;

	return age441x_setMeasBoolean (instrumentHandle, tmp_channel, "CAL%ld:RCAL", state);
}


/*===========================================================================*/
/* Function: Configure Recorder                                              */
/* Purpose:  This function configures the recorder outputs.			         */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_configureRecorder (ViSession instrumentHandle,
                                             ViInt32 recorder,
                                             ViReal64 limitLower,
                                             ViReal64 limitUpper,
                                             ViBoolean state)
{
	ViChar		buffer[BUFFER_SIZE];
	ViChar*		pbuffer=buffer;
	
	if (age441x_invalidViInt32Range (recorder, 1, 2))
		return VI_ERROR_PARAMETER2;       
	if (age441x_invalidViBooleanRange (state))
		return VI_ERROR_PARAMETER5;
	
	pbuffer += sprintf (pbuffer, ":OUTP:REC%ld:LIM:LOW %Le", recorder, limitLower);
	pbuffer += sprintf (pbuffer, ";:OUTP:REC%ld:LIM:UPP %Le", recorder, limitUpper);
	pbuffer += sprintf (pbuffer, ";:OUTP:REC%ld:STAT %s", recorder, switchArr[state]);
	
	return age441x_setViNode (instrumentHandle, buffer);
}

/*===========================================================================*/
/* Function: Recorder Measurement                                            */
/* Purpose:  This function sets the measurement which is sent to the recorder*/
/*	         output.       													 */
/* Note:     This function is available only for EMP-P instruments.			 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_recorderMeasurement (ViSession instrumentHandle,
                                               ViInt32 recorder,
                                               ViInt32 measurement)
{

	ViStatus			age441x_status = VI_SUCCESS;     
	age441x_instrRange 	instrPtr;

	CHECKERR (viGetAttribute (instrumentHandle, VI_ATTR_USER_DATA, &instrPtr));	   
	if (age441x_invalidViInt32Range (recorder, 1, 2))
		return VI_ERROR_PARAMETER2;
	//Check instrument version, only E4416 or E4417 instruments supports this function
	if (instrPtr->instrVersion > 17)
		return AGE441X_ERROR_INVALID_INSTRUMENT;
	if (age441x_invalidInstrVersion (instrumentHandle, measurement))
		return VI_ERROR_PARAMETER3;   

	return age441x_setMeasString (instrumentHandle, recorder, ":OUTP:REC%ld:FEED", measureArr[measurement]);
}

/*===========================================================================*/
/* Function: Reference Oscillator State                                      */
/* Purpose:  This function enables/disables the POWER REF output			 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_referenceOscillatorState (ViSession instrumentHandle,
                                                    ViBoolean state)
{
	return age441x_setViBoolean (instrumentHandle, ":OUTP:ROSC:STAT", state);		
}  


/*===========================================================================*/
/* Function: Configure TTL				                                     */
/* Purpose:  This function configures the TTL outputs.						 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_configureTTL (ViSession instrumentHandle,
                                        ViInt32 TTLOutput, 
                                        ViBoolean state,
                                        ViInt32 activeMode, 
                                        ViInt32 measurement,
                                        ViInt32 limit)
{
	ViStatus			age441x_status = VI_SUCCESS;  
	ViChar 				buffer[BUFFER_SIZE];
	ViChar*				pbuffer=buffer;
	age441x_instrRange instrPtr;
	
	CHECKERR (viGetAttribute (instrumentHandle, VI_ATTR_USER_DATA, &instrPtr));	   
	
	if (age441x_invalidViInt32Range (TTLOutput, 1, 2))
		return VI_ERROR_PARAMETER2;  
	if (age441x_invalidViBooleanRange (state))
		return VI_ERROR_PARAMETER3;		
	if (age441x_invalidViInt32Range (activeMode, 0, 1))
		return VI_ERROR_PARAMETER4;  			
	if (age441x_invalidViInt32Range (measurement, 1, 4))
		return VI_ERROR_PARAMETER5;  
	if (age441x_invalidViInt32Range (limit, 0, 2))
		return VI_ERROR_PARAMETER6;  		
	
	if (age441x_invalidInstrVersion (instrumentHandle, TTLOutput))
		return VI_ERROR_PARAMETER2;  

	//For E4418B and E4419B agrees measurement with TTLOutput number (TTLOutput 1 with Upper window (CALC1)
	//and TTLOutput2 with lower window (CALC2)
	
	if (instrPtr->instrVersion > 17)
		measurement = TTLOutput;
	
	pbuffer += sprintf (pbuffer, ":OUTP:TTL%ld:ACT %s", TTLOutput, TTLLimitArr[activeMode]);
	pbuffer += sprintf (pbuffer, ";:OUTP:TTL%ld:FEED ", TTLOutput);
	switch (limit)
	{
		case AGE441X_TTL_LOW:
		case AGE441X_TTL_HIGH:
			pbuffer += sprintf (pbuffer, "\"CALC%ld:LIM:%s\"", measurement, TTLLimit2Arr[limit]);	
			break;
		case AGE441X_TTL_BOTH:
			pbuffer += sprintf (pbuffer, "\"CALC%ld:LIM:%s,CALC%ld:LIM:%s\"", measurement, TTLLimit2Arr[AGE441X_TTL_LOW], measurement, TTLLimit2Arr[AGE441X_TTL_HIGH]);	
	}
	pbuffer += sprintf (pbuffer, ";:OUTP:TTL%ld:STAT %s", TTLOutput, switchArr[state]); 
	
	return age441x_setViNode (instrumentHandle, buffer);
}



/*===========================================================================*/
/* Function: Set Initiate Continuous State                                   */
/* Purpose:  This function sets the power meter for continuous trigger cycles*/
/*===========================================================================*/
ViStatus _VI_FUNC age441x_setInitiateContinuousState (ViSession instrumentHandle,
                                                      ViInt32 channel,
                                                      ViBoolean state)
{
	ViInt32				tmp_channel;
	
	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];
	if (age441x_invalidViBooleanRange (state))
		return VI_ERROR_PARAMETER3;

	if (state)
		if(age441x_getSensorFromUserData (instrumentHandle, channel) ==  AGE441X_SENSOR_NONE)
			return AGE441X_ERROR_BAD_SENSOR;
			
	return age441x_setMeasBoolean (instrumentHandle, tmp_channel, "INIT%ld:CONT", state);	
}

/*===========================================================================*/
/* Function: Configure Trigger			                                     */
/* Purpose:  This function configures the trigger system.					 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_configureTrigger (ViSession instrumentHandle,
                                            ViInt32 channel,
                                            ViBoolean delayAutoState,
                                            ViInt32 source)
{
	ViStatus			age441x_status = VI_SUCCESS;  
	age441x_instrRange 	instrPtr;  
	ViInt32				tmp_channel;
	ViInt32				tmp_source_num;
	ViChar				buffer[BUFFER_SIZE];
	ViChar*				pbuffer=buffer;
	
	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];
	if (age441x_invalidViBooleanRange (delayAutoState))
		return VI_ERROR_PARAMETER3;  
		
	CHECKERR (viGetAttribute (instrumentHandle, VI_ATTR_USER_DATA, &instrPtr));	 	
	
	switch (instrPtr->instrVersion)
	{
		case 16:
			tmp_source_num = AGE441X_TRIG_INTA;
			break;
		case 17:
			tmp_source_num = AGE441X_TRIG_INTB;
			break;
		case 18:
		case 19:
			tmp_source_num = AGE441X_TRIG_HOLD;
			break;
	}
	if (age441x_invalidViInt32Range (source, 0, tmp_source_num))
		return VI_ERROR_PARAMETER4;  
		
	pbuffer += sprintf (pbuffer, ":TRIG%ld:DEL:AUTO %s", tmp_channel, switchArr[delayAutoState]);
	pbuffer += sprintf (pbuffer, ";:TRIG%ld:SOUR %s", tmp_channel, trigSourArr[source]);
	
	return age441x_setViNode (instrumentHandle, buffer);
}

/*===========================================================================*/
/* Function: Configure Channel A Trigger			                         */
/* Purpose:  This function configures the parameters which can be set only 	 */
/*			  for channel A.					 							 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_configureChannelATrigger (ViSession instrumentHandle,
                                                    ViReal64 delay,
                                                    ViReal64 holdoff,
                                                    ViReal64 hysteresis,
                                                    ViReal64 powerLevel,
                                                    ViInt32 levelAuto,
                                                    ViInt32 slope)
{
	ViStatus			age441x_status = VI_SUCCESS;  
	age441x_instrRange 	instrPtr;
	ViChar				buffer[BUFFER_SIZE];
	ViChar*				pbuffer=buffer;
	
	CHECKERR (viGetAttribute (instrumentHandle, VI_ATTR_USER_DATA, &instrPtr));
	if (instrPtr->instrVersion > 17)
		return AGE441X_ERROR_INVALID_INSTRUMENT;    
	
	if (age441x_invalidViReal64Range (delay, -1.0, 1.0))
		return VI_ERROR_PARAMETER2;
	if (age441x_invalidViReal64Range (holdoff, 1.0E-6, 4.0E-1))
		return VI_ERROR_PARAMETER3;	
	if (age441x_invalidViReal64Range (hysteresis, 0.0, 3.0))
		return VI_ERROR_PARAMETER4;	
	if (age441x_invalidViReal64Range (powerLevel, -40.0, 20.0))
		return VI_ERROR_PARAMETER5;	
	if (age441x_invalidViInt32Range (levelAuto, 0, 2))
		return VI_ERROR_PARAMETER6;	
	if (age441x_invalidViInt32Range (slope, 0, 1))
		return VI_ERROR_PARAMETER7;	
		
	pbuffer += sprintf (pbuffer, ":TRIG:SEQ:DEL %Le;:TRIG:SEQ:HOLD %Le;:TRIG:SEQ:HYST %Le;:TRIG:SEQ:LEV %Le",
	delay, holdoff, hysteresis, powerLevel);
	
	CHECKERR (age441x_setViNode (instrumentHandle, buffer));
	
	pbuffer=buffer;
	pbuffer += sprintf (pbuffer, ":TRIG:SEQ:LEV:AUTO %s;:TRIG:SEQ:SLOP %s", switchArr[levelAuto],slopeArr[slope]);
	
	return age441x_setViNode (instrumentHandle, buffer);
}

/*===========================================================================*/
/* Function: Trigger Event Count					                         */
/* Purpose:  This function controls the path of the trigger subsystem in the */
/*			 upward traverse of the wait for trigger state.					 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_triggerEventCount (ViSession instrumentHandle,
                                             ViInt32 channel, 
                                             ViInt32 count)
{
	ViStatus			age441x_status = VI_SUCCESS;  
	age441x_instrRange 	instrPtr;
	ViInt32				tmp_channel;
	
	CHECKERR (viGetAttribute (instrumentHandle, VI_ATTR_USER_DATA, &instrPtr));
	if (instrPtr->instrVersion > 17)
		return AGE441X_ERROR_INVALID_INSTRUMENT;   
	
	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];	
	if (age441x_invalidViInt32Range (count, 1, 50))
		return VI_ERROR_PARAMETER3;    
		
	return age441x_setMeasInt32 (instrumentHandle, tmp_channel, ":TRIG:SEQ%ld:COUN", count);		
}

/*===========================================================================*/
/* Function: E Series Offset Table			                                 */
/* Purpose:  This function  configures the offset table.					 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_eSeriesOffsetTable (ViSession instrumentHandle,
                                              ViInt32 channel,
                                              ViBoolean tableState,
                                              ViString tableName)
{
	//ViStatus			age441x_status = VI_SUCCESS;
	ViChar				buffer[BUFFER_SIZE];
	ViChar*				pbuffer=buffer;				
	ViInt32				tmp_channel;

	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];
	
	if (age441x_invalidInstrVersion (instrumentHandle, tmp_channel))
		return VI_ERROR_PARAMETER2; 
	if( age441x_getSensorFromUserData (instrumentHandle, channel) < AGE441X_SENSOR_E)
		return AGE441X_ERROR_BAD_SENSOR;
	if (age441x_invalidViBooleanRange (tableState))
		return VI_ERROR_PARAMETER3;
	if ((tableState && (!tableName) || (strlen(tableName)==0)))
		return VI_ERROR_PARAMETER4;	
	
	if (tableState)	pbuffer+=sprintf(pbuffer, ":SENS%ld:CORR:CSET2:SEL \"%s\";", tmp_channel, tableName);
	pbuffer+=sprintf(pbuffer, ":SENS%ld:CORR:CSET2:STAT %s", tmp_channel, switchArr[tableState]);
	
	return age441x_setViNode (instrumentHandle, buffer);	
}

/*===========================================================================*/
/* Function: E Series Configure Input				                         */
/* Purpose:  This function configures the special parameters for E-Series 	 */
/*			 sensors connected to input. 									 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_eSeriesConfigureInput (ViSession instrumentHandle,
                                                 ViInt32 channel,
                                                 ViBoolean autoranging,
                                                 ViInt32 powerRange)
{
	ViStatus			age441x_status = VI_SUCCESS;  
	ViInt32				tmp_channel;
	//ViChar				buffer[BUFFER_SIZE];
	//ViChar*				pbuffer=buffer;
	

	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];	
	
	if (age441x_getSensorFromUserData (instrumentHandle, channel) < AGE441X_SENSOR_E)
		return AGE441X_ERROR_BAD_SENSOR;
	if (age441x_invalidViBooleanRange (autoranging))
		return VI_ERROR_PARAMETER3;
	if (age441x_invalidViInt32Range (powerRange, 0,1))
		return VI_ERROR_PARAMETER4;	
		
	if (autoranging)
	{
		CHECKERR (age441x_setMeasBoolean (instrumentHandle, tmp_channel, ":SENS%ld:POW:AC:RANG:AUTO", autoranging));
	}	
	else
		CHECKERR (age441x_setMeasInt32 (instrumentHandle, tmp_channel, ":SENS%ld:POW:AC:RANG", powerRange));	

	return age441x_status;	
}

/*===========================================================================*/
/* Function: E Series Configure Measurement                                  */
/* Purpose:  This function configures the channel measurement for E series 	 */
/*           sensors. The expected value can be set.			     		 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_eSeriesConfigureMeasurement (ViSession instrumentHandle,
                                                       ViInt32 measurement,
                                                       ViInt32 measuringFunction,
                                                       ViInt32 sensor1,
                                                       ViInt32 sensor2,
                                                       ViInt32 resolution,
                                                       ViReal64 expectedValue,
                                                       ViInt32 unit)
{
	ViStatus	age441x_status = VI_SUCCESS;
	ViChar		buffer[BUFFER_SIZE];
	ViChar*		pbuffer=buffer;
	

	CHECKERR (age441x_buildMeasureCommand (instrumentHandle, measurement, measuringFunction, sensor1, sensor2, 
										   resolution, expectedValue, ":CONF", VI_FALSE, buffer));
	pbuffer += strlen(buffer);
	sprintf (pbuffer, ";:UNIT%ld:POW %s", measurement, unitArr[unit]);
	
	return age441x_setViNode (instrumentHandle, buffer);       
}

/*===========================================================================*/
/* Function: E9320 Video Averaging					                         */
/* Purpose:  This function configures the video averaging.					 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_e9320VideoAveraging (ViSession instrumentHandle,
                                          ViInt32 channel, 
                                          ViBoolean state,
                                          ViInt32 filterLength)
{
	//ViStatus			age441x_status = VI_SUCCESS;  
	ViInt32				tmp_channel;
	ViChar				buffer[BUFFER_SIZE];
	

	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];	
	
	if (age441x_getSensorFromUserData (instrumentHandle, channel) != AGE441X_SENSOR_E9320)
		return AGE441X_ERROR_BAD_SENSOR;	
	if (age441x_invalidViBooleanRange (state))
		return VI_ERROR_PARAMETER3;
	if (age441x_invalidViInt32Range (filterLength, 1, 255))
		return VI_ERROR_PARAMETER4; 
		
	sprintf (buffer, ":SENS%ld:AVER2:COUN %ld;:SENS%ld:AVER2:STAT %s", tmp_channel, filterLength, tmp_channel, switchArr[state]);
	
	return age441x_setViNode (instrumentHandle, buffer);
}

/*===========================================================================*/
/* Function: E9320 Sensor Parameters		                         		 */	
/* Purpose:  This function sets the E9320 sensor mode.						 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_e9320SensorParameters (ViSession instrumentHandle,
		                                         ViInt32 channel, 
		                                         ViInt32 mode,
		                                         ViInt32 bandwidth)
{
    
	ViInt32		tmp_channel;
	ViChar		buffer[BUFFER_SIZE];
	ViChar*		pbuffer;
	
	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel]; 
	if (age441x_getSensorFromUserData (instrumentHandle, channel) != AGE441X_SENSOR_E9320)
		return AGE441X_ERROR_BAD_SENSOR;
	if (age441x_invalidViInt32Range (mode, 0, 1))
		return VI_ERROR_PARAMETER3;    
	if (age441x_invalidViInt32Range (bandwidth, 0, 3))
		return VI_ERROR_PARAMETER4;
		
	pbuffer=buffer;	
	pbuffer += sprintf (pbuffer, ":SENS%ld:DET:FUNC %s", tmp_channel, sensorModeArr[mode]); 		
	if (mode == AGE441X_E9320_MODE_NORM)
	 	pbuffer += sprintf (pbuffer, ";:SENS%ld:BAND:VIDEO %s", tmp_channel, bandArr[bandwidth]);
	 	
	return age441x_setViNode (instrumentHandle, buffer); 	
}

/*===========================================================================*/
/* Function: E9320 Configure Trace					                         */
/* Purpose:  This function configures the trace for E9320  	 			  	 */
/*			 sensors in normal mode											 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_e9320ConfigureTrace (ViSession instrumentHandle,
                                            ViInt32 channel,
                                            ViBoolean traceState,
                                            ViReal64 traceOffsetTime,
                                            ViReal64 traceTime,
                                            ViReal64 traceLowLimit,
                                            ViReal64 traceUpperLimit,
                                            ViInt32 traceUnit)
{
	ViStatus			age441x_status = VI_SUCCESS;  
	ViInt32				tmp_channel;
	ViChar				buffer[BUFFER_SIZE];
	ViChar*				pbuffer=buffer;

	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];	
	
	if (age441x_getSensorFromUserData (instrumentHandle, channel) != AGE441X_SENSOR_E9320)
		return AGE441X_ERROR_BAD_SENSOR;	
	if (age441x_invalidViBooleanRange (traceState))
		return VI_ERROR_PARAMETER3;	
	if (age441x_invalidViReal64Range (traceOffsetTime, -1.0, 1.0))
		return VI_ERROR_PARAMETER4;			
	if (age441x_invalidViReal64Range (traceTime, 10.0E-6, 1.0))
		return VI_ERROR_PARAMETER5;	
	if (age441x_invalidViReal64Range (traceLowLimit, -150.0, 230.0))
		return VI_ERROR_PARAMETER6;			
	if (age441x_invalidViReal64Range (traceUpperLimit, -150.0, 230.0))
		return VI_ERROR_PARAMETER7;	
	if (age441x_invalidViInt32Range (traceUnit, 0, 1))
		return VI_ERROR_PARAMETER8;		

	if (traceState)
	{
		pbuffer += sprintf (pbuffer, ":SENS%ld:DET:FUNC NORM", tmp_channel);
		pbuffer += sprintf (pbuffer, ";:SENS%ld:TRAC:UNIT DBM", tmp_channel);
		//Send shorter strings, the serial interface has problem with long strings
		CHECKERR (age441x_setViNode (instrumentHandle, buffer));
	
		pbuffer=buffer;
		pbuffer += sprintf (pbuffer, ":SENS%ld:TRAC:LIM:LOW %Le", tmp_channel, traceLowLimit);
		pbuffer += sprintf (pbuffer, ";:SENS%ld:TRAC:LIM:UPP %Le", tmp_channel, traceUpperLimit);   
		pbuffer += sprintf (pbuffer, ";:SENS%ld:TRAC:OFFS:TIME %Le", tmp_channel, traceOffsetTime);    
		pbuffer += sprintf (pbuffer, ";:SENS%ld:TRAC:TIME %Le", tmp_channel, traceTime); 
	
		//Send shorter strings, the serial interface has problem with long strings
		CHECKERR (age441x_setViNode (instrumentHandle, buffer));
		pbuffer=buffer;
		pbuffer += sprintf (pbuffer, ":SENS%ld:TRAC:UNIT %s;", tmp_channel, unitArr[traceUnit]);
	}	
	pbuffer += sprintf (pbuffer, ":TRAC%ld:STAT %s", tmp_channel, switchArr[traceState]); 
	
	return age441x_setViNode (instrumentHandle, buffer);	
}

/*===========================================================================*/
/* Function: E9320 Configure Gate					                         */
/* Purpose:  This function configures the time-gated measurement parameters. */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_e9320ConfigureGate (ViSession instrumentHandle,
                                              ViInt32 channel, 
                                              ViInt32 gate,
                                              ViReal64 offsetTime, 
                                              ViReal64 time)
{
	//ViStatus	age441x_status=VI_SUCCESS;  
	ViInt32		tmp_channel;	    
	ViChar		buffer[BUFFER_SIZE];
	
	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];	
	
	if (age441x_getSensorFromUserData (instrumentHandle, channel) != AGE441X_SENSOR_E9320)
		return AGE441X_ERROR_BAD_SENSOR;	
	if (age441x_invalidViInt32Range (gate, 1, 4))
		return VI_ERROR_PARAMETER3; 
	if (age441x_invalidViReal64Range (offsetTime, -1.0, 1.0))
		return VI_ERROR_PARAMETER4;
	if (age441x_invalidViReal64Range (time, 0.0, 1.0))
		return VI_ERROR_PARAMETER5; 
	
	sprintf (buffer, ":SENS%ld:SWE%ld:TIME %Le;:SENS%ld:SWE%ld:OFFS:TIME %Le",
	tmp_channel, gate, time, tmp_channel, gate, offsetTime);
	
	return age441x_setViNode (instrumentHandle, buffer);
}

/*===========================================================================*/
/* Function: E9320 Output Trigger State		                                 */
/* Purpose:  This function enables/disables the output trigger state.		 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_e9320OutputTriggerState (ViSession instrumentHandle,
                                             	 ViBoolean state)
{
	ViStatus			age441x_status=VI_SUCCESS;
	age441x_instrRange 	instrPtr; 
	
	CHECKERR (viGetAttribute (instrumentHandle, VI_ATTR_USER_DATA, &instrPtr));	
	
	//Check instrument version, only E4416 or E4417 instruments supports this function
	
	if (instrPtr->instrVersion > 17)
		return AGE441X_ERROR_INVALID_INSTRUMENT;

	return age441x_setViBoolean (instrumentHandle, ":OUTP:TRIG:STAT", state);    
}

/*===========================================================================*/
/* Function: Configure Duty Cycle			                                 */
/* Purpose:  This function configures the pulse power measurement feature of */
/*			 the power meter.												 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_configureDutyCycle (ViSession instrumentHandle,
                                              ViInt32 channel,
                                              ViBoolean dutyCycleState,
                                              ViReal64 dutyCycle)
{
	ViStatus			age441x_status = VI_SUCCESS;
	ViInt32				tmp_channel;
	
	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];
	
	if (age441x_invalidViBooleanRange (dutyCycleState))
		return VI_ERROR_PARAMETER3; 
	if (age441x_invalidViReal64Range (dutyCycle, 0.001, 99.999))
		return VI_ERROR_PARAMETER4; 
	
	if (dutyCycleState)
	{
		if (age441x_getSensorFromUserData (instrumentHandle, channel) >= AGE441X_SENSOR_E)
			return AGE441X_ERROR_BAD_SENSOR;	
		CHECKERR (age441x_setMeasReal64 (instrumentHandle, tmp_channel, ":SENS%ld:CORR:GAIN3:INP:MAGN", dutyCycle));
	}	
	else
		CHECKERR (age441x_setMeasBoolean (instrumentHandle, tmp_channel, ":SENS%ld:CORR:GAIN3:STAT", dutyCycleState));	
	
	return age441x_status;			
}

/*===========================================================================*/
/* Function: Set Corrections				                                 */
/* Purpose:  This function configures configures the calibration corrections */
/*			 for selected channel.  										 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_setCorrections (ViSession instrumentHandle,
                                           ViInt32 channel,
                                           ViBoolean tableState,
                                           ViString tableName,
                                           ViReal64 calibrationFactorGain)
{
	//ViStatus			age441x_status = VI_SUCCESS;
	ViChar				buffer[BUFFER_SIZE];
	ViChar*				pbuffer=buffer;				
	ViInt32				tmp_channel;

	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];
	
	if (age441x_invalidInstrVersion (instrumentHandle, tmp_channel))
		return VI_ERROR_PARAMETER2; 
	if( age441x_getSensorFromUserData (instrumentHandle, channel) != AGE441X_SENSOR_8480)
		return AGE441X_ERROR_BAD_SENSOR;
	if (age441x_invalidViBooleanRange (tableState))
		return VI_ERROR_PARAMETER3;
	if ((tableState && (!tableName) || (strlen(tableName)==0)))
		return VI_ERROR_PARAMETER4;	
	if (age441x_invalidViReal64Range (calibrationFactorGain, 1.0, 150.0))
		return VI_ERROR_PARAMETER5; 
	
	if (tableState)	
	{
		pbuffer+=sprintf(pbuffer, ":SENS%ld:CORR:CSET1:SEL \"%s\"", tmp_channel, tableName);
		pbuffer+=sprintf(pbuffer, ";:SENS%ld:CORR:CSET1:STAT %s", tmp_channel, switchArr[tableState]);
	}
	else
	{
		pbuffer+=sprintf(pbuffer, ":SENS%ld:CORR:CSET1:STAT %s", tmp_channel, switchArr[tableState]);
		pbuffer+=sprintf(pbuffer, ";:SENS%ld:CORR:CFAC %Le", tmp_channel, calibrationFactorGain);
	}
	
	return age441x_setViNode (instrumentHandle, buffer); 
}

/*===========================================================================*/
/* Function: Set Calibration Factor                                          */
/* Purpose:  This function sets the reference calibration factor of the 	 */
/*			 specified channel.       										 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_setCalibrationFactor (ViSession instrumentHandle,
                                                ViInt32 channel, 
                                                ViReal64 factor)
{
	//ViStatus	age441x_status = VI_SUCCESS;
	ViInt32		tmp_channel;	

	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];

	if (age441x_getSensorFromUserData(instrumentHandle, channel) != AGE441X_SENSOR_8480)
		return AGE441X_ERROR_BAD_SENSOR;
	if (age441x_invalidViReal64Range (factor, 1.0, 150.0))
		return VI_ERROR_PARAMETER3;    

	return age441x_setMeasReal64 (instrumentHandle, tmp_channel, "CAL%ld:RCF", factor);	
}

/*===========================================================================*/
/* Function: Memory Contents						                         */
/* Purpose:  This function returns the name of tables (registers) stored 	 */
/*			 into memory. 													 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_memoryContents (ViSession instrumentHandle,
                                          ViInt32 type, 
                                          ViInt32 stringLength,
                                          ViChar catalog[], 
                                          ViInt32 *retLength)

{
	ViStatus	age441x_status=VI_SUCCESS;   
	//ViChar		cmd[BUFFER_SIZE];
	ViChar		name[20]; //Max 12 chars allowed
	ViChar*		pbuffer;
	ViChar*		p2buf;
	ViInt32		retLen;
	ViInt32		len;
	ViInt32		tmp_len;
	
	if (age441x_invalidViInt32Range (type, 0, 1))
		return VI_ERROR_PARAMETER2;
	if (!catalog)
		return VI_ERROR_PARAMETER4;  
		
	
	switch (type)
	{
		case AGE441X_MEM_TABLE:
			CHECKERR (viPrintf (instrumentHandle, ":MEM:CAT:TABL?\n"));     
			break;
		case AGE441X_MEM_SRREG:
			CHECKERR (viPrintf (instrumentHandle, ":MEM:CAT:STAT?\n")); 
			break;
	}
	len=0;
	if ((age441x_status = age441x_ReadDataUnknownLength (instrumentHandle, &pbuffer, &retLen)) >= VI_SUCCESS)
	{
		tmp_len = (stringLength -1);
		p2buf = strtok (pbuffer, "\"");
		p2buf = strtok (NULL, "\"");
		while ((p2buf != NULL) && (len < tmp_len))
		{
			sscanf (p2buf, "%[^,],%*[^\"]", name);
			len += strlen (name);
			if (len <= tmp_len)
			{
				strcat (catalog, name);
				if (len < tmp_len)
					strcat (catalog, ","); 
			}	
			p2buf = strtok (NULL, "\"");      
			p2buf = strtok (NULL, "\""); 
		};
		if (!p2buf)
			catalog[len]='\0';
		if (retLength)
			*retLength = retLen;
	}	
	free (pbuffer);
	CHECKERR (age441x_sysStatus (instrumentHandle));	
	
	return age441x_status;
}

/*===========================================================================*/
/* Function: Size Of Memory Block					                         */
/* Purpose:  This function returns the size of selected Table or 			 */
/*			 Save/Recall Register	 										 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_sizeOfMemoryBlock (ViSession instrumentHandle,
                                             ViChar name[], ViInt32 *size)
{
	ViStatus	age441x_status=VI_SUCCESS;   
	//ViChar		cmd[BUFFER_SIZE];
	ViChar		tmp_name[20]; //Max 12 chars allowed
	ViChar*		pbuffer;
	ViChar*		p2buf;
	ViInt32		retLen;
	ViInt32		tmp_size;
	ViBoolean	found = VI_FALSE;
	
	if (strlen(name) == 0)
		return VI_ERROR_PARAMETER2;
	if (!size)
		return VI_ERROR_PARAMETER3;  
		
	
	CHECKERR (viPrintf (instrumentHandle, ":MEM:CAT:ALL?\n"));     
	if ((age441x_status = age441x_ReadDataUnknownLength (instrumentHandle, &pbuffer, &retLen)) >= VI_SUCCESS)
	{
		p2buf = strtok (pbuffer, "\"");
		p2buf = strtok (NULL, "\"");
		while (p2buf != NULL)
		{
			sscanf (p2buf, "%[^,],%*[^,],%ld", tmp_name, &tmp_size);
			if (!strcmp(tmp_name,name))
			{
				*size = tmp_size;
				found = VI_TRUE;
				break;
			}	
			p2buf = strtok (NULL, "\"");      
			p2buf = strtok (NULL, "\""); 
		};
		if (!found)
			age441x_status = AGE441X_ERROR_NAME_NOT_FOUND;
	}	
	free (pbuffer);
	CHECKERR (age441x_sysStatus (instrumentHandle));
	return age441x_status;
}

/*===========================================================================*/
/* Function: Active Table Values				                         	 */
/* Purpose:  This function returns the contents of table stored in memory.	 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_activeTableValues (ViSession instrumentHandle,
                                             ViInt32 frequencyArraySize,
                                             ViReal64 frequencyArray[],
                                             ViInt32 *returnedFreqValues,
                                             ViInt32 factorArraySize,
                                             ViReal64 factorOffset[],
                                             ViInt32 *returnedOffsetValues)
{
	ViStatus	age441x_status = VI_SUCCESS;
    ViInt32		tmp_freq;
    ViInt32		tmp_offset;
    ViInt32		tmp_points;
    
    CHECKERR (age441x_activeTableInformation (instrumentHandle, VI_NULL, &tmp_freq, &tmp_offset));
    tmp_points = ((tmp_freq < frequencyArraySize) ? tmp_freq : frequencyArraySize);
    CHECKERR (age441x_getViReal64Arr (instrumentHandle, "MEM:TABL:FREQ?", tmp_points, frequencyArray));
    tmp_points = ((tmp_offset < factorArraySize) ? tmp_offset : factorArraySize);
    CHECKERR (age441x_getViReal64Arr (instrumentHandle, "MEM:TABL:GAIN:MAGN?", tmp_points, factorOffset));
    
    if (returnedFreqValues) *returnedFreqValues = tmp_freq;
	if (returnedOffsetValues) *returnedOffsetValues = tmp_offset;    
	
	return age441x_status;
}

/*===========================================================================*/
/* Function: Active Table Information				                         */
/* Purpose:  This function returns the name and numbers of points in 		 */
/*			 currently selected table.			 							 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_activeTableInformation (ViSession instrumentHandle,
                                                  ViChar name[],
                                                  ViInt32 *freqPoints,
                                                  ViInt32 *offsetPoints)
{
 
    ViStatus	age441x_status = VI_SUCCESS;
    ViChar		tmp_name[20];
    ViInt32		tmp_freq;
    ViInt32		tmp_offset;
    
    
    CHECKERR (age441x_getViNode (instrumentHandle, ":MEM:TABL:SEL?", tmp_name));
    CHECKERR (age441x_getViInt32 (instrumentHandle, ":MEM:TABL:FREQ:POIN?", &tmp_freq));
    CHECKERR (age441x_getViInt32 (instrumentHandle, ":MEM:TABL:GAIN:MAGN:POIN?", &tmp_offset))
 	
 	if (name) strcpy (name, tmp_name);
 	if (freqPoints) *freqPoints = tmp_freq;
	if (offsetPoints) *offsetPoints = tmp_offset; 
	
	return age441x_status;
}

/*===========================================================================*/
/* Function: Table Operation	                                             */
/* Purpose:  This function operates the sensor calibration/frequency 		 */
/*			 dependent offset table..        								 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_tableOperation (ViSession instrumentHandle,
                                          ViInt32 function, ViChar table1[],
                                          ViChar table2[])
{
	ViStatus	age441x_status = VI_SUCCESS; 
	ViChar		buffer[BUFFER_SIZE];
	
	
	if (age441x_invalidViInt32Range (function, 0, 2))
		return VI_ERROR_PARAMETER2;
	if (strlen(table1) == 0)
		return VI_ERROR_PARAMETER3;
	if ((function == AGE441X_MEM_MOVE) && (strlen(table2) == 0))
		return VI_ERROR_PARAMETER4;
	
	switch (function)
	{
		case AGE441X_MEM_SEL:
			CHECKERR (age441x_setViString (instrumentHandle, ":MEM:TABL:SEL", table1));
			break;
		case AGE441X_MEM_CLEAR:
			CHECKERR (age441x_setViString (instrumentHandle, ":MEM:CLE:NAME", table1));
			break;
		case AGE441X_MEM_MOVE:
			sprintf (buffer, ":MEM:TABL:MOVE \"%s\",\"%s\"", table1, table2);
			CHECKERR (age441x_setViNode (instrumentHandle, buffer));
			break;
	}
	return age441x_status;			
}

/*===========================================================================*/
/* Function: Define Calibration Table                                        */
/* Purpose:  This function is used to enter data into the current selected 	 */
/*			 sensor calibration table	 									 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_defineCalibrationTable (ViSession instrumentHandle,
                                                  ViInt32 arraySize,
                                                  ViReal64 frequencyArray[],
                                                  ViReal64 calibrationFactors[])
{
	ViStatus	age441x_status = VI_SUCCESS;  
	ViInt32		tmp_size;
	
	if (age441x_invalidViInt32Range (arraySize, 1, 80))
		return VI_ERROR_PARAMETER2;
	if (!frequencyArray)
		return VI_ERROR_PARAMETER3;   
	if (!calibrationFactors)
		return VI_ERROR_PARAMETER4;

	tmp_size = arraySize + 1;
	CHECKERR (age441x_setViReal64Arr (instrumentHandle, ":MEM:TABL:FREQ", arraySize, frequencyArray));
	CHECKERR (age441x_setViReal64Arr (instrumentHandle, ":MEM:TABL:GAIN:MAGN", tmp_size, calibrationFactors));	
	
	return age441x_status;		
}

/*===========================================================================*/
/* Function: Define Offset Table                                        	 */
/* Purpose:  This function is used to enter data into the current selected 	 */
/*			 frequency dependent offset table.								 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_defineOffsetTable (ViSession instrumentHandle,
                                             ViInt32 arraySize,
                                             ViReal64 frequencyArray[],
                                             ViReal64 offsets[])
{
 	ViStatus	age441x_status = VI_SUCCESS;  
	
	if (age441x_invalidViInt32Range (arraySize, 1, 80))
		return VI_ERROR_PARAMETER2;
	if (!frequencyArray)
		return VI_ERROR_PARAMETER3;   
	if (!offsets)
		return VI_ERROR_PARAMETER4;  

	CHECKERR (age441x_setViReal64Arr (instrumentHandle, ":MEM:TABL:FREQ", arraySize, frequencyArray));
	CHECKERR (age441x_setViReal64Arr (instrumentHandle, ":MEM:TABL:GAIN:MAGN", arraySize, offsets));	
	
	return age441x_status;
}

/*===========================================================================*/
/* Function: Register Map Name												 */
/* Purpose:  This function associate a name with a save/recall register 	 */
/*			 number. 	 													 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_registerMapName (ViSession instrumentHandle,
                                           ViInt32 registerNumber, 
                                           ViChar name[])
{
	ViChar		buffer[BUFFER_SIZE];
	
	if (age441x_invalidViInt32Range (registerNumber, 0, 9))
		return VI_ERROR_PARAMETER2;
	if (strlen (name) == 0)
		return VI_ERROR_PARAMETER3;  
	
	sprintf (buffer, ":MEM:STAT:DEF \"%s\",%ld", name, registerNumber);
	
	return age441x_setViNode (instrumentHandle, buffer);
}

/*===========================================================================*/
/* Function: Register Count													 */
/* Purpose:  This function returns the number of registers that are 		 */
/*			 available for save/recall.	 									 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_registerCount (ViSession instrumentHandle,
                                         ViInt32 *count)
{
	return age441x_getViInt32 (instrumentHandle, ":MEM:NST?", count);
}

/*===========================================================================*/
/* Function: Load From Register Memory										 */
/* Purpose:  This function restores the power meter state from the 			 */
/*			 save/recall register.	 	 									 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_loadFromRegister (ViSession instrumentHandle,
                                            ViInt32 registerNumber)
{
	return age441x_setViInt32 (instrumentHandle, "*RCL", registerNumber, 1, 10);		
}

/*===========================================================================*/
/* Function: Save To Register Memory										 */
/* Purpose:  This function saves the power meter state to the 			 	 */
/*			 save/recall register.	 	 									 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_saveToRegister (ViSession instrumentHandle,
                                          ViInt32 registerNumber)
{
	return age441x_setViInt32 (instrumentHandle, "*SAV", registerNumber, 1, 10);    
}

/*===========================================================================*/
/* Function: Get Limits Failure												 */
/* Purpose:  This function indicating whether there have been any limit 	 */
/*			 failures for the specified window.		 						 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_getLimitsFailure (ViSession instrumentHandle,
                                            ViInt32 measurement,
                                            ViBoolean *limitExceeded,
                                            ViInt32 *numberOfFailures)
{
	ViStatus 	age441x_status = VI_SUCCESS; 
	ViBoolean 	tmp_limit;
	ViInt32		tmp_fail;
	
	CHECKERR (age441x_getMeasBoolean (instrumentHandle, measurement, ":CALC%ld:LIM:FAIL?",&tmp_limit));
	CHECKERR (age441x_getMeasInt32 (instrumentHandle, measurement, ":CALC%ld:LIM:FCO?",&tmp_fail));
	
	if (limitExceeded) *limitExceeded = tmp_limit;
	if (numberOfFailures) *numberOfFailures = tmp_fail;	
	
	return age441x_status;
}

/*===========================================================================*/
/* Function: Clear Fail Count												 */
/* Purpose:  This function clears the FCO (fail counter) of any limit		 */
/*			 failures for the specified measurement. 						 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_clearFailCount (ViSession instrumentHandle,
                                          ViInt32 measurement)
{
	return age441x_setMeasNode (instrumentHandle, measurement, ":CALC%ld:LIM:CLE");	    
}

/*===========================================================================*/
/* Function: Calibration													 */
/* Purpose:  This function performs calibration and/or zeroing on selected   */
/*			 channel and returns the result.								 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_calibration (ViSession instrumentHandle,
                                       ViInt32 channel, 
                                       ViInt32 action,
                                       ViBoolean *fail)
{
	ViStatus 	age441x_status = VI_SUCCESS;    
	//ViBoolean 	tmp_fail = 0;
	ViInt32		tmp_channel;
	//ViUInt16	regVal;
	ViChar		buffer[BUFFER_SIZE];
	ViChar		ret_buffer[BUFFER_SIZE];
	ViInt32		ret_val;

	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];
	
	if (age441x_invalidViInt32Range (action, 0, 2))
		return VI_ERROR_PARAMETER3;

	switch (action)
	{
		case AGE441X_CAL_ALL:
			sprintf (buffer, "CAL%ld:ALL?", tmp_channel);
			break;
		case AGE441X_CAL_CAL:
			sprintf (buffer, "CAL%ld:AUTO ONCE;*OPC?", tmp_channel);
			break;
		case AGE441X_CAL_ZERO:
			sprintf (buffer, "CAL%ld:ZERO:AUTO ONCE;*OPC?", tmp_channel);
			break;
	}
	
	CHECKERR (age441x_waitOPC (instrumentHandle, buffer, ret_buffer, 100000)); 
	sscanf (ret_buffer, "%ld", &ret_val);
	if (fail)
	{
		if (action == AGE441X_CAL_ALL)
			*fail = ret_val;
		else
			*fail = 0;
	}

	return age441x_status;
}

/*===========================================================================*/
/* Function: E9320 Clear Peak Hold 											 */
/* Purpose:  This function clears the peak hold value for a specified 		 */
/*			 CALC block so that a new peak hold value can be set.			 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_e9320ClearPeakHold (ViSession instrumentHandle,
                                              ViInt32 measurement)
{

	return age441x_setMeasNode (instrumentHandle, measurement, "CALC%ld:PHOL:CLE");
}

/*===========================================================================*/
/* Function: E9320 Normal Path Zeroing										 */
/* Purpose:  This function provides a quick way of zeroing the NORMAL path 	 */
/*      	 of an E9320 series sensor.		 								 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_e9320NormalPathZeroing (ViSession instrumentHandle,
                                                  ViInt32 channel)
{
	ViStatus	age441x_status = VI_SUCCESS;
	ViInt32		tmp_channel;
	ViChar		buffer[BUFFER_SIZE];
	ViChar		ret_buffer[BUFFER_SIZE];
	
	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];
	if (age441x_getSensorFromUserData (instrumentHandle, channel) != AGE441X_SENSOR_E9320)
		return AGE441X_ERROR_BAD_SENSOR;	
	sprintf (buffer,"CAL%ld:ZERO:NORM:AUTO ONCE;*OPC?", tmp_channel);
	CHECKERR (age441x_waitOPC (instrumentHandle, buffer, ret_buffer, 60000));
	
	return age441x_status;     
}

/*===========================================================================*/
/* Function: Available Memory												 */
/* Purpose:  This function returns the amount of free and used memory	 	 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_availableMemory (ViSession instrumentHandle,
                                           ViInt32 type, 
                                           ViInt32 *free,
                                           ViInt32 *used)
{
	ViStatus 	age441x_status = VI_SUCCESS;    
	ViInt32		tmp_free;
	ViInt32		tmp_used;
	ViString	freeMemCmd[] = {"MEM:FREE:TABL?","MEM:FREE:STAT?","MEM:FREE:ALL?"};
	
	if (age441x_invalidViInt32Range (type, 0, 2))
		return VI_ERROR_PARAMETER2;
	
	CHECKERR (viPrintf (instrumentHandle, "%s\n", freeMemCmd[type]));			
	CHECKERR (viScanf (instrumentHandle, "%ld,%ld", &tmp_free, &tmp_used));
	
	if (free) *free = tmp_free;
	if (used) *used = tmp_used;	
	
	return age441x_status;
}

/*===========================================================================*/
/* Function: Abort															 */
/* Purpose:  This function aborts the waiting for trigger on selected channel*/
/*===========================================================================*/
ViStatus _VI_FUNC age441x_abort (ViSession instrumentHandle, ViInt32 channel)
{
 	ViInt32		tmp_channel;
	
	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];
	
	return age441x_setMeasNode (instrumentHandle, tmp_channel, "ABOR%ld"); 
}

/*===========================================================================*/
/* Function: Initiate														 */
/* Purpose:  This function sets the power meter for a single trigger cycle.	 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_initiate (ViSession instrumentHandle, ViInt32 channel)
{
	ViInt32		tmp_channel;
	
	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];
	
	return age441x_setMeasNode (instrumentHandle, tmp_channel, "INIT%ld");     
}

/*===========================================================================*/
/* Function: Trigger Immediate												 */
/* Purpose:  This function triggers immediate the channel.	   				 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_triggerImmediate (ViSession instrumentHandle,
                                            ViInt32 channel)
{
	ViInt32		tmp_channel;
	
	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];
	
	return age441x_setMeasNode (instrumentHandle, tmp_channel, "TRIG%ld:IMM");         
}

/*===========================================================================*/
/* Function: Set Status Registers											 */
/* Purpose:  This function sets the instrument status registers.			 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_setStatusRegisters (ViSession instrumentHandle,
                                              ViInt32 reg,
                                              ViInt32 regPart,
                                              ViUInt16 state)			
{
	//ViStatus	age441x_status = VI_SUCCESS;
	ViChar		buffer[BUFFER_SIZE]="\0";
	ViChar*		pbuffer = buffer;
	
	if (age441x_invalidViInt32Range (reg, 0, 12))
		return VI_ERROR_PARAMETER2;
	if (age441x_invalidViInt32Range (regPart, 0, 2))
		return VI_ERROR_PARAMETER3;	
		
	pbuffer += sprintf (pbuffer, "%s", regArr[reg]);	
	if (reg > 1) pbuffer += sprintf (pbuffer, "%s", regPartArr[regPart]);	
	pbuffer += sprintf (pbuffer, " %hu", state);	
	
	return age441x_setViNode (instrumentHandle, buffer);
}

/*===========================================================================*/
/* Function: Get Status Registers											 */
/* Purpose:  This function returns the instrument status registers state.	 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_getStatusRegisters (ViSession instrumentHandle,
                                              ViInt32 reg,
                                              ViInt32 regPart,
                                              ViUInt16 *state)
{
	ViStatus	age441x_status = VI_SUCCESS;
	ViChar		buffer[BUFFER_SIZE]="\0";
	ViChar*		pbuffer = buffer;
	ViInt32		tmp_reg;
	
	if (age441x_invalidViInt32Range (reg, 0, 14))
		return VI_ERROR_PARAMETER2;
	if (age441x_invalidViInt32Range (regPart, 0, 4))
		return VI_ERROR_PARAMETER3;	
	if (!state)
		return VI_ERROR_PARAMETER4;		
		
	pbuffer += sprintf (pbuffer, "%s", regArr[reg]);	
	if ((reg > 1)&&(reg <13)) pbuffer += sprintf (pbuffer, "%s", regPartArr[regPart]);	
	pbuffer += sprintf (pbuffer, "?");
	
	CHECKERR (age441x_getViInt32 (instrumentHandle, buffer, &tmp_reg));    
	*state = (ViUInt16) tmp_reg;
	
	return age441x_status;
}

/*===========================================================================*/
/* Function: Fetch															 */
/* Purpose:  This function returns the last valid result.					 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_fetch (ViSession instrumentHandle, 
								 ViInt32 measurement,
                                 ViInt32 measuringFunction,
                                 ViReal64 *measuredValue)
{
	ViChar 	buffer[BUFFER_SIZE];
	
	if (age441x_invalidViInt32Range (measuringFunction, 0, 5))
		return VI_ERROR_PARAMETER3;
	if (!measuredValue)
		return VI_ERROR_PARAMETER4;
		
	sprintf (buffer, "FETC%ld%s?", measurement, measFuncArr[measuringFunction]);     
	
	return age441x_getViReal64 (instrumentHandle, buffer, measuredValue);		    
}

/*===========================================================================*/
/* Function: Read															 */
/* Purpose:  This function initiates the measurement and returns the 		 */
/*			 measured value.			 									 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_read (ViSession instrumentHandle, 
								ViInt32 measurement,
                                ViInt32 measuringFunction,
                                ViReal64 *measuredValue)
{
	ViChar 	buffer[BUFFER_SIZE];
	
	if (age441x_invalidViInt32Range (measuringFunction, 0, 5))
		return VI_ERROR_PARAMETER3;
		
	if (!measuredValue)	
		return VI_ERROR_PARAMETER4;	
		
	sprintf (buffer, "READ%ld%s?", measurement, measFuncArr[measuringFunction]); 
	
	return age441x_getViReal64 (instrumentHandle, buffer, measuredValue);	    
}

/*===========================================================================*/
/* Function: Measure														 */
/* Purpose:  This function configures and initiates the channel measurement  */
/*  		 and returns the measured value.								 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_measure (ViSession instrumentHandle,
                                   ViInt32 measurement,
                                   ViInt32 measuringFunction, 
                                   ViInt32 sensor1,
                                   ViInt32 sensor2, 
                                   ViInt32 resolution,
                                   ViInt32 unit, 
                                   ViReal64 *measuredValue)

{
	ViStatus	age441x_status = VI_SUCCESS;
	ViChar		buffer[BUFFER_SIZE];
	ViChar*		pbuffer=buffer;
	
	if (!measuredValue)	
		return VI_ERROR_PARAMETER7;	
		
	pbuffer += sprintf (pbuffer, ":UNIT%ld:POW %s;", measurement, unitArr[unit]); 
	CHECKERR (age441x_buildMeasureCommand (instrumentHandle, measurement, measuringFunction, sensor1, sensor2, resolution,
											VI_NULL, ":MEAS", VI_TRUE, pbuffer));
	
	
	return age441x_getViReal64 (instrumentHandle, buffer, measuredValue);
}

/*===========================================================================*/
/* Function: E Series Measure												 */
/* Purpose:  This function configures and initiates the channel measurement  */
/*			 for E-Series sensors and returns the measured value.	 		 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_eSeriesMeasure (ViSession instrumentHandle,
                                          ViInt32 measurement,
                                          ViInt32 measuringFunction,
                                          ViInt32 sensor1, ViInt32 sensor2,
                                          ViInt32 resolution,
                                          ViReal64 expectedValue, 
                                          ViInt32 unit,
                                          ViReal64 *measuredValue)
{
	ViStatus			age441x_status = VI_SUCCESS;
	ViChar				buffer[BUFFER_SIZE];
	ViChar*				pbuffer=buffer; 
	
	if (!measuredValue)	
		return VI_ERROR_PARAMETER8;	
		
	pbuffer += sprintf (pbuffer, ":UNIT%ld:POW %s;", measurement, unitArr[unit]);        
	CHECKERR (age441x_buildMeasureCommand (instrumentHandle, measurement, measuringFunction, sensor1, sensor2, resolution,
											expectedValue, ":MEAS", VI_TRUE, pbuffer));
	
	
	return age441x_getViReal64 (instrumentHandle, buffer, measuredValue);    
}

/*===========================================================================*/
/* Function: Trace Data														 */
/* Purpose:  This function returns the trace data.							 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_e9320TraceData (ViSession instrumentHandle,
                                          ViInt32 channel, 
                                          ViInt32 resolution,
                                          ViInt32 arraySize, 
                                          ViReal64 data[],
                                          ViInt32 *returnedValues)

{
 	ViStatus			age441x_status = VI_SUCCESS;       
 	ViInt32				tmp_channel;
 	ViChar				tmp_buf[5];
	ViUInt16			intf; 
	//age441x_instrRange 	instrPtr; 
	ViChar				buffer[BUFFER_SIZE];
	ViInt32				tmp_cnt;
	ViChar*				pbuffer=VI_NULL;
	ViChar*				p2buff;
	ViInt32				read_now;
	ViInt32				read_num;
	ViInt32				data_num;
	ViInt32				retCnt=0;
	ViInt32				i;	
	ViReal32*			pfloat;
	
	if (returnedValues) *returnedValues = 0;
	
	CHECKERR (viGetAttribute (instrumentHandle, VI_ATTR_INTF_TYPE, &intf));
	if (intf == VI_INTF_ASRL)
		return AGE441X_ERROR_INTERFACE;
	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];
	if (age441x_invalidInstrVersion (instrumentHandle, tmp_channel))
		return VI_ERROR_PARAMETER2; 
	if (age441x_invalidViInt32Range (resolution, 0, 2))	
		return VI_ERROR_PARAMETER3;   	
	if (age441x_getSensorFromUserData (instrumentHandle, channel) != AGE441X_SENSOR_E9320)
		return AGE441X_ERROR_BAD_SENSOR;	
		
	//Disable termination character for read
	CHECKERR (viSetAttribute (instrumentHandle, VI_ATTR_TERMCHAR_EN, VI_FALSE));
//	CHECKERR (viSetAttribute (instrumentHandle, VI_ATTR_ASRL_END_IN, VI_ASRL_END_NONE));
	CHECKOK(viPrintf (instrumentHandle, ":TRAC%ld:DATA? %s\n", tmp_channel, tracResArr[resolution]));
	
	
	CHECKOK (viRead (instrumentHandle, buffer, 2, &retCnt));
	buffer[retCnt]='\0';
	sscanf (buffer, "#%ld", &tmp_cnt);
	//Read amount of data
	CHECKOK (viRead (instrumentHandle, buffer, tmp_cnt, &retCnt));
	buffer[tmp_cnt]='\0';
	if (age441x_status >= VI_SUCCESS)
	{
		sscanf (buffer, "%ld", &tmp_cnt);
		read_num = tmp_cnt;   
		read_now = (read_num<BUFFER_SIZE) ? read_num : BUFFER_SIZE;

		pbuffer = calloc (tmp_cnt, sizeof(ViChar));
		p2buff = pbuffer;
	}
	else
		read_now = 0;
		
	while (read_now > 0)
	{
		if(age441x_status=(viRead (instrumentHandle, p2buff, read_now, &retCnt) < VI_SUCCESS))
			read_now = 0;
		else
		{
			p2buff += retCnt;
			read_num -= retCnt;
			read_now = (read_num<BUFFER_SIZE) ? read_num : BUFFER_SIZE;  
		}
	};
	//Read new line character from interface
	viRead (instrumentHandle, tmp_buf, 1, VI_NULL);
	//Discard warning
	if (age441x_status == 0x3FFF0006) age441x_status = VI_SUCCESS;	
	
	if (age441x_status >= VI_SUCCESS) 
	{
		data_num = tmp_cnt/4;
		if (returnedValues) *returnedValues = data_num;
		//Swap bytes, if Intel procesor in PC
		age441x_SWAP_32 (pbuffer, data_num);
		data_num = (arraySize < data_num) ? arraySize : data_num;
		pfloat = (ViReal32*)pbuffer;
		for (i =0; i<data_num; i++)
			data[i] = pfloat[i];
	}	
	if (pbuffer) free (pbuffer);
	viSetAttribute (instrumentHandle, VI_ATTR_TERMCHAR_EN, VI_TRUE);
	
	return age441x_sysStatus (instrumentHandle);
}



/*===========================================================================*/
/* Function: Write To Instrument                                             */
/* Purpose:  This function writes a command string to the instrument.        */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_writeInstrData (ViSession instrSession, ViString writeBuffer)
{
    ViStatus age441x_status = VI_SUCCESS;
    
    CHECKERR (viPrintf (instrSession, "%s", writeBuffer));
   
    return age441x_status;
}

/*===========================================================================*/
/* Function: Read Instrument Buffer                                          */
/* Purpose:  This function reads the output buffer of the instrument.        */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_readInstrData (ViSession instrSession, ViInt32 numberBytesToRead,
                    ViChar _VI_FAR readBuffer[], ViPInt32 numBytesRead)
{
    ViStatus age441x_status = VI_SUCCESS;
    *numBytesRead = 0L;
        
    CHECKERR (viRead (instrSession, readBuffer, numberBytesToRead, numBytesRead));
      
    return age441x_status;
}

/*===========================================================================*/
/* Function: Reset                                                           */
/* Purpose:  This function resets the instrument.  If the reset function     */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_reset (ViSession instrSession)
{
    //ViUInt32	retCnt = 0;
    ViInt32	 	retVal;
    ViStatus 	age441x_status = VI_SUCCESS;

    /*  Initialize the instrument to a known state.  */
    CHECKERR (viPrintf (instrSession, "*RST;*OPC?\n"));
    CHECKERR (viScanf (instrSession, "%ld", &retVal));
    CHECKERR (age441x_defaultInstrSetup (instrSession)); 
   
    return age441x_status;
}

/*===========================================================================*/
/* Function: Self-Test                                                       */
/* Purpose:  This function executes the instrument self-test and returns     */
/*           the result. 													 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_self_test (ViSession instrSession, ViPInt16 testResult,
                    ViChar _VI_FAR testMessage[])
{
	//ViUInt32 retCnt = 0;
	ViStatus age441x_status = VI_SUCCESS;
	ViChar buffer[BUFFER_SIZE];  
	
	if (!testResult) 
		return VI_ERROR_PARAMETER2;
	if (!testMessage) 
		return VI_ERROR_PARAMETER3;
	
	CHECKERR (age441x_waitOPC (instrSession, "*TST?", buffer, 60000));     
	
	if (!sscanf (buffer, "%hd", testResult))
        return VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        
	if (*testResult == 0)
        strcpy (testMessage, "Self-Test Passed.");
    else
        strcpy (testMessage, "Self-Test Failed.");

	return age441x_status;
}

/*===========================================================================*/
/* Function: Error Query                                                     */
/* Purpose:  This function queries the instrument error queue, and returns   */
/*           the result. If the error query function is not supported by the  */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_error_query (ViSession instrSession, ViPInt32 errorCode,
                    ViChar _VI_FAR errorMessage[])
{
    ViUInt32 retCnt = 0;
    ViStatus age441x_status = VI_SUCCESS;

    CHECKERR (viPrintf (instrSession, ":SYST:ERR?\n", 10, &retCnt));
    CHECKERR (viScanf (instrSession, "%ld,\"%[^\"]", errorCode, errorMessage));

    return age441x_status;
}
/*===========================================================================*/
/* Function: Error Message                                                   */
/* Purpose:  This function translates the error return value from the        */
/*           instrument driver into a user-readable string.                  */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_error_message (ViSession instrSession, ViStatus statusCode,
                    ViChar _VI_FAR errMessage[])
{
    ViStatus age441x_status = VI_SUCCESS;
    ViInt16 i;
    static age441x_tStringValPair statusDescArray[] = {
        {VI_WARN_NSUP_ID_QUERY,     "WARNING: ID Query not supported"},
        {VI_WARN_NSUP_RESET,        "WARNING: Reset not supported"},
        {VI_WARN_NSUP_SELF_TEST,    "WARNING: Self-test not supported"},
        {VI_WARN_NSUP_ERROR_QUERY,  "WARNING: Error Query not supported"},     
        {VI_WARN_NSUP_REV_QUERY,    "WARNING: Revision Query not supported"},
        {VI_ERROR_PARAMETER1,   "ERROR: Parameter 1 out of range"},
        {VI_ERROR_PARAMETER2,   "ERROR: Parameter 2 out of range"},
        {VI_ERROR_PARAMETER3,   "ERROR: Parameter 3 out of range"},
        {VI_ERROR_PARAMETER4,   "ERROR: Parameter 4 out of range"},
        {VI_ERROR_PARAMETER5,   "ERROR: Parameter 5 out of range"},
        {VI_ERROR_PARAMETER6,   "ERROR: Parameter 6 out of range"},
        {VI_ERROR_PARAMETER7,   "ERROR: Parameter 7 out of range"},
        {VI_ERROR_PARAMETER8,   "ERROR: Parameter 8 out of range"},
        {VI_ERROR_FAIL_ID_QUERY,"ERROR: Identification query failed"},
        {VI_ERROR_INV_RESPONSE, "ERROR: Interpreting instrument response"},
        {VI_ERROR_INSTR_FILE_OPEN,    "ERROR: Opening the specified file"},
        {VI_ERROR_INSTR_FILE_WRITE,   "ERROR: Writing to the specified file"},
        {VI_ERROR_INSTR_INTERPRETING_RESPONSE, "ERROR: Interpreting the instrument's response"},
        {VI_ERROR_INSTR_PARAMETER9 ,  "ERROR: Parameter 9 out of range"},
        {VI_ERROR_INSTR_PARAMETER10,  "ERROR: Parameter 10 out of range"},
        {VI_ERROR_INSTR_PARAMETER11,  "ERROR: Parameter 11 out of range"},
        {VI_ERROR_INSTR_PARAMETER12,  "ERROR: Parameter 12 out of range"},
        {VI_ERROR_INSTR_PARAMETER13,  "ERROR: Parameter 13 out of range"},
        {VI_ERROR_INSTR_PARAMETER14,  "ERROR: Parameter 14 out of range"},
        {VI_ERROR_INSTR_PARAMETER15,  "ERROR: Parameter 15 out of range"},
           
        
        /*=CHANGE:=============================================================*/
        /* Insert instrument-specific error codes here.  Example:              */
        /*=====================================================================*/
        {AGE441X_ERROR_INSTRUMENT_ERROR,  "ERROR: Instrument status error"},
        {AGE441X_ERROR_INSTRUMENT_NUMBER,  "ERROR: Unsupported instrument number"},
        {AGE441X_ERROR_INVALID_INSTRUMENT,  "ERROR: This function or parameter is not supported by this instrument version"},
        {AGE441X_ERROR_BAD_SENSOR,  "ERROR: No sensor connected or the connected sensor doesn't support this function."},
        {AGE441X_ERROR_NAME_NOT_FOUND, "Table or Save/Recall register name not found in memory."},
        {AGE441X_ERROR_INTERFACE, "This function is not available for this interface."},
        
        {VI_NULL, VI_NULL}
    };

    age441x_status = viStatusDesc (instrSession, statusCode, errMessage);
    if (age441x_status == VI_WARN_UNKNOWN_STATUS) {
        for (i = 0; statusDescArray[i].stringName; i++) {
            if (statusDescArray[i].stringVal == statusCode) {
                sprintf (errMessage, "%s", statusDescArray[i].stringName);
                return (VI_SUCCESS);
            }
        }
        sprintf (errMessage, "Unknown Error 0x%x", statusCode);
        return (VI_WARN_UNKNOWN_STATUS);
    }
    											 
    age441x_status = VI_SUCCESS;
    return age441x_status;
}

/*===========================================================================*/
/* Function: Revision Query                                                  */
/* Purpose:  This function returns the driver and instrument revisions.      */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_revision_query (ViSession instrSession,
                    ViChar _VI_FAR instrumentDriverRevision[], 
                    ViChar _VI_FAR firmwareRevision[])
{
    ViUInt32 retCnt = 0;
    ViStatus age441x_status = VI_SUCCESS;

    CHECKERR (viPrintf (instrSession, "*IDN?\n", 5, &retCnt));
    CHECKERR (viScanf (instrSession, "%*[^,],%*[^,],%*[^,],%[^\n]", firmwareRevision));
        
    sprintf (instrumentDriverRevision, "%s", age441x_REVISION);
    
    return age441x_status;
}

/*===========================================================================*/
/* Function: Instrument Type                                                 */
/* Purpose:  This function returns the instrument type.                      */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_instrumentType (ViSession instrumentHandle,
                                          ViInt32 *type)
{
	ViStatus 			age441x_status = VI_SUCCESS;
	age441x_instrRange 	instrPtr; 	
	
	if (!type)
		return VI_ERROR_PARAMETER2;
		
	CHECKERR (viGetAttribute (instrumentHandle, VI_ATTR_USER_DATA, &instrPtr));
	*type = instrPtr->instrVersion;
	
	return age441x_status;
	
}

/*===========================================================================*/
/* Function: Get Sensor Type	                                             */
/* Purpose:  This function returns the type of sensor connected to the 		 */
/*			 selected input.       											 */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_getSensorType (ViSession instrumentHandle,
                                         ViInt32 channel, 
                                         ViBoolean *state,
                                         ViChar sensorType[])
{
	ViStatus	age441x_status = VI_SUCCESS;
	ViInt32		tmp_channel;
	ViChar		buffer[BUFFER_SIZE];
	ViUInt16	regStat;
	ViUInt16	dec_bit; //decimal value of bit
	
	if (age441x_invalidViInt32Range (channel, 0, 1))
		return VI_ERROR_PARAMETER2;       
	tmp_channel = channelArr[channel];
	
	if (!state)
		return VI_ERROR_PARAMETER3;
	
	CHECKERR (age441x_getStatusRegisters (instrumentHandle, AGE441X_STATUS_DEVICE, AGE441X_STATUS_COND, &regStat));
	dec_bit = (ViUInt16) pow (2, tmp_channel);
	*state = (regStat & dec_bit);
	if (*state)
		CHECKERR (age441x_getMeasString (instrumentHandle, tmp_channel, ":SERV:SENS%ld:TYPE?", buffer));  
	else
		strcpy(buffer,"NONE");
	
	if (sensorType) strcpy(sensorType, buffer);
	
	return age441x_status;
	
}

/*===========================================================================*/
/* Function: Set Check Status                                                */
/* Purpose:  This function sets the status checking.	                     */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_setCheckStatus (ViSession instrumentHandle,
                                          ViBoolean state)
{

    ViStatus age441x_status = VI_SUCCESS;
    age441x_instrRange instrPtr;

    CHECKERR (viGetAttribute (instrumentHandle, VI_ATTR_USER_DATA, &instrPtr));
    instrPtr -> checkStatus = state;

    return age441x_status;
}

/*===========================================================================*/
/* Function: Get Check Status                                                */
/* Purpose:  This function returns the status checking.	                     */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_getCheckStatus (ViSession instrumentHandle,
                                          ViBoolean *state)
{
    ViStatus age441x_status = VI_SUCCESS;
    age441x_instrRange instrPtr;

    CHECKERR (viGetAttribute (instrumentHandle, VI_ATTR_USER_DATA, &instrPtr));
    *state = instrPtr -> checkStatus;

    return age441x_status;    
}

/*===========================================================================*/
/* Function: Close                                                           */
/* Purpose:  This function closes the instrument.                            */
/*===========================================================================*/
ViStatus _VI_FUNC age441x_close (ViSession instrSession)
{
    age441x_instrRange instrPtr;
    ViSession rmSession;
    ViStatus age441x_status = VI_SUCCESS;

    CHECKERR (viGetAttribute (instrSession, VI_ATTR_RM_SESSION, &rmSession));
    CHECKERR (age441x_status = viGetAttribute (instrSession, VI_ATTR_USER_DATA, &instrPtr));
            
    if (instrPtr != NULL) 
        free (instrPtr);
    
    age441x_status = viClose (instrSession);
    viClose (rmSession);

    return age441x_status;
}
/*****************************************************************************/
/*= UTILITY ROUTINES (Non-Exportable Functions) =============================*/
/*****************************************************************************/

/*===========================================================================*/
/* Function: Boolean Value Out Of Range - ViBoolean                          */
/* Purpose:  This function checks a Boolean to see if it is equal to VI_TRUE */
/*           or VI_FALSE. If the value is out of range, the return value is  */
/*           VI_TRUE, otherwise the return value is VI_FALSE.                */
/*===========================================================================*/
static ViBoolean age441x_invalidViBooleanRange (ViBoolean val)
{
    return ((val != VI_FALSE && val != VI_TRUE) ? VI_TRUE : VI_FALSE);
}

/*===========================================================================*/
/* Function: Short Signed Integer Value Out Of Range - ViInt16               */
/* Purpose:  This function checks a short signed integer value to see if it  */  
/*           lies between a minimum and maximum value.  If the value is out  */
/*           of range, the return value is VI_TRUE, otherwise the return     */
/*           value is VI_FALSE.                                              */
/*===========================================================================*/
static ViBoolean age441x_invalidViInt16Range (ViInt16 val, ViInt16 min, ViInt16 max)
{
    return ((val < min || val > max) ? VI_TRUE : VI_FALSE);
}

/*===========================================================================*/
/* Function: Long Signed Integer Value Out Of Range - ViInt32                */
/* Purpose:  This function checks a long signed integer value to see if it   */  
/*           lies between a minimum and maximum value.  If the value is out  */
/*           of range, the return value is VI_TRUE, otherwise the return     */
/*           value is VI_FALSE.                                              */
/*===========================================================================*/
static ViBoolean age441x_invalidViInt32Range (ViInt32 val, ViInt32 min, ViInt32 max)
{
    return ((val < min || val > max) ? VI_TRUE : VI_FALSE);
}

/*===========================================================================*/
/* Function: Unsigned Char Value Out Of Range - ViUInt8                      */
/* Purpose:  This function checks an unsigned char value to see if it        */  
/*           lies between a minimum and maximum value.  If the value is out  */
/*           of range, the return value is VI_TRUE, otherwise the return     */
/*           value is VI_FALSE.                                              */
/*===========================================================================*/
static ViBoolean age441x_invalidViUInt8Range (ViUInt8 val, ViUInt8 min, ViUInt8 max)
{
    return ((val < min || val > max) ? VI_TRUE : VI_FALSE);
}

/*===========================================================================*/
/* Function: Short Unsigned Integer Value Out Of Range - ViUInt16            */
/* Purpose:  This function checks a short unsigned integer value to see if it*/  
/*           lies between a minimum and maximum value.  If the value is out  */
/*           of range, the return value is VI_TRUE, otherwise the return     */
/*           value is VI_FALSE.                                              */
/*===========================================================================*/
static ViBoolean age441x_invalidViUInt16Range (ViUInt16 val, ViUInt16 min, ViUInt16 max)
{
    return ((val < min || val > max) ? VI_TRUE : VI_FALSE);
}

/*===========================================================================*/
/* Function: Long Unsigned Integer Value Out Of Range - ViUInt32             */
/* Purpose:  This function checks a long unsigned integer value to see if it */  
/*           lies between a minimum and maximum value.  If the value is out  */
/*           of range, the return value is VI_TRUE, otherwise the return     */
/*           value is VI_FALSE.                                              */
/*===========================================================================*/
static ViBoolean age441x_invalidViUInt32Range (ViUInt32 val, ViUInt32 min, ViUInt32 max)
{
    return ((val < min || val > max) ? VI_TRUE : VI_FALSE);
}

/*===========================================================================*/
/* Function: Real (Float) Value Out Of Range - ViReal32                      */
/* Purpose:  This function checks a real (float) value to see if it lies     */  
/*           between a minimum and maximum value.  If the value is out of    */
/*           range, the return value is VI_TRUE, otherwise the return value  */
/*           is VI_FALSE.                                                    */
/*===========================================================================*/
static ViBoolean age441x_invalidViReal32Range (ViReal32 val, ViReal32 min, ViReal32 max)
{
    return ((val < min || val > max) ? VI_TRUE : VI_FALSE);
}

/*===========================================================================*/
/* Function: Real (Double) Value Out Of Range - ViReal64                     */
/* Purpose:  This function checks a real (double) value to see if it lies    */  
/*           between a minimum and maximum value.  If the value is out of    */
/*           range, the return value is VI_TRUE, otherwise the return value  */
/*           is VI_FALSE.                                                    */
/*===========================================================================*/
static ViBoolean age441x_invalidViReal64Range (ViReal64 val, ViReal64 min, ViReal64 max)
{
    return ((val < min || val > max) ? VI_TRUE : VI_FALSE);
}

/*===========================================================================*/
/* Function: Invalid Instrument Version										 */
/* Purpose : This function checks the measurement according instrument       */
/*			 version														 */
/* Parameters: measurement - measurement number (1 to 4) 					 */
/*							 or channel number (1 or 2)						 */
/*===========================================================================*/
static ViBoolean age441x_invalidInstrVersion (ViSession instrumentHandle, 
											  ViInt32 measurement)
{

	age441x_instrRange instrPtr;
	
	if (viGetAttribute (instrumentHandle, VI_ATTR_USER_DATA, &instrPtr)!=VI_SUCCESS)
		return VI_FALSE;	   
	
	switch (measurement)
	{
		case 2:
			if (instrPtr->instrVersion == 18)
				return VI_TRUE;
			break;
		case 3:
		case 4:
			if (instrPtr->instrVersion != 17)
				return VI_TRUE;  
			break; 
		default:
			break;
	}
	
	return VI_FALSE;
}	


/*================================================================================*/
/* Function: Set ViNode  		                                                  */
/* Purpose:  This function sends the command to the instrument and checks the     */
/*			 status 															  */
/*================================================================================*/
static ViStatus age441x_setViNode (ViSession instrumentHandle, 
									 ViString cmd)
{
	ViStatus age441x_status =VI_SUCCESS;
		
	CHECKERR (viPrintf (instrumentHandle, "%s\n", cmd));
	
	return age441x_sysStatus (instrumentHandle);    
}	

/*================================================================================*/
/* Function: Set ViNode  		                                                  */
/* Purpose:  This function sends the command to the instrument and checks the     */
/*			 status 															  */
/*================================================================================*/
static ViStatus age441x_setViNode_EX (ViSession instrumentHandle, 
									 ViString cmd)
{
	ViStatus age441x_status =VI_SUCCESS;
		
	CHECKERR (viPrintf (instrumentHandle, "%s\n", cmd));
	
	return 0;    
}	

/*================================================================================*/
/* Function: Get Node	  		                                             	  */
/* Purpose:  This function reads the string (without "") from the instrument	  */
/*================================================================================*/
static ViStatus age441x_getViNode (ViSession instrumentHandle, 
								     ViString cmd, 
								     ViString _VI_FAR string)
{
	ViStatus 	age441x_status = VI_SUCCESS;
	//ViChar*		buffer;
		
	CHECKERR (viPrintf (instrumentHandle, "%s\n", cmd));
	CHECKERR (viScanf (instrumentHandle, "%[^\n]", string));
	
	return age441x_sysStatus (instrumentHandle);    
}	

/*================================================================================*/
/* Function: Set ViBoolean 		                                                  */
/* Purpose:  This function sends the command with boolean parameter to the 		  */
/*			 instrument and checks the status 									  */
/*================================================================================*/
static ViStatus age441x_setViBoolean (ViSession instrumentHandle, 
									 ViString cmd, ViBoolean value)
{
	ViStatus			age441x_status=VI_SUCCESS;          
	
	if (age441x_invalidViBooleanRange (value))
		return VI_ERROR_PARAMETER2;
	
	CHECKERR (viPrintf (instrumentHandle, "%s %s\n", cmd, switchArr[value]));
	
	return age441x_sysStatus (instrumentHandle);    
}

/*================================================================================*/
/* Function: Set ViInt32 		                                                  */
/* Purpose:  This function sends the command with ViINt32 parameter to the 		  */
/*			 instrument and checks the status 									  */
/*================================================================================*/
static ViStatus age441x_setViInt32 (ViSession instrumentHandle, 
									ViString cmd, 
									ViInt32 value,
									ViInt32 min,
									ViInt32 max)
{
	ViStatus			age441x_status=VI_SUCCESS;          
	
	if (age441x_invalidViInt32Range (value, min, max))
		return VI_ERROR_PARAMETER2;
	
	CHECKERR (viPrintf (instrumentHandle, "%s %ld\n", cmd, value));
	
	return age441x_sysStatus (instrumentHandle);    
}

/*================================================================================*/
/* Function: Get ViInt32 		                                                  */
/* Purpose:  This function reads the ViInt32 response from the instrument	 	  */
/*			 and checks the status 									  		  */
/*================================================================================*/
static ViStatus age441x_getViInt32 (ViSession instrumentHandle, 
									 ViString cmd, ViInt32 *value)
{
	ViStatus			age441x_status=VI_SUCCESS;          
	
	if (!value)
		return VI_ERROR_PARAMETER2;
		
	CHECKERR (viPrintf (instrumentHandle, "%s\n", cmd));
	CHECKERR (viScanf (instrumentHandle, "%ld", value));
	
	return age441x_sysStatus (instrumentHandle);    
}

/*================================================================================*/
/* Function: Get ViReal64 		                                                  */
/* Purpose:  This function reads the ViReal64 response from the instrument	 	  */
/*			 and checks the status 									  		  	  */
/*================================================================================*/
static ViStatus age441x_getViReal64 (ViSession instrumentHandle, 
									 ViString cmd, ViReal64 *value)
{
	ViStatus			age441x_status=VI_SUCCESS;          
	
	if (!value)
		return VI_ERROR_PARAMETER2;
		
	CHECKERR (viPrintf (instrumentHandle, "%s\n", cmd));
	CHECKERR (viScanf (instrumentHandle, "%Le", value));
	
	return age441x_sysStatus (instrumentHandle);    
}

/*================================================================================*/
/* Function: Set ViString  		                                              	  */
/* Purpose:  This function sends the command with String parameter to the 		  */
/*			 instrument and checks the status  									  */
/*================================================================================*/
static ViStatus age441x_setViString (ViSession instrumentHandle, 
									 ViString cmd,
									 ViChar value[])
{
	ViStatus	age441x_status =VI_SUCCESS;
	
	CHECKERR (viPrintf (instrumentHandle, "%s \"%s\"\n", cmd, value));
	
	return age441x_sysStatus (instrumentHandle);    
}	

/*================================================================================*/
/* Function: Get String  		                                             	  */
/* Purpose:  This function reads the string from the instrument				      */
/*================================================================================*/
static ViStatus age441x_getViString (ViSession instrumentHandle, 
								     ViString cmd, 
								     ViString _VI_FAR string)
{
	ViStatus 	age441x_status = VI_SUCCESS;
	//ViChar*		buffer;
		
	CHECKERR (viPrintf (instrumentHandle, "%s\n", cmd));
	CHECKERR (viScanf (instrumentHandle, "\"%[^\n]\"", string));
	
	return age441x_sysStatus (instrumentHandle);    
}	

/*================================================================================*/
/* Function: Set ViReal64 Array	                                                  */
/* Purpose:  This function sets ViReal64 array into instrument and checks the 	  */
/*			 status					  											  */
/*================================================================================*/
static ViStatus age441x_setViReal64Arr (ViSession instrumentHandle, 
										ViString cmd, 
										ViInt32 arraySize,
										ViReal64 value[])
										
{
	ViStatus	age441x_status=VI_SUCCESS;
	
	CHECKERR (viPrintf (instrumentHandle, "%s %,*Lf\n", cmd, arraySize, value));
	
	return age441x_sysStatus (instrumentHandle);    
}

/*================================================================================*/
/* Function: Get ViReal64 Array	                                                  */
/* Purpose:  This function reads the list of ViReal64 values from the instrument  */
/*			 and converts them to the ViReal64 array and checks the status 		  */
/*================================================================================*/
static ViStatus age441x_getViReal64Arr (ViSession instrumentHandle, 
										ViString cmd, 
										ViInt32 arraySize,
										ViReal64 *value)
										
{
	ViStatus			age441x_status=VI_SUCCESS;          
	
	CHECKERR (viPrintf (instrumentHandle, "%s\n", cmd));
	CHECKERR (viScanf (instrumentHandle, "%,#Lf", &arraySize, value));
	
	return age441x_sysStatus (instrumentHandle);    
}

/*================================================================================*/
/* Function: Set Meas String  		                                              */
/* Purpose:  This function sends the command with measurement/window or channel   */
/*			 number and String parameter to the instrument and checks the status  */
/*================================================================================*/
static ViStatus age441x_setMeasString (ViSession instrumentHandle, 
									   ViInt32 meas,
									   ViString cmd,
									   ViChar value[])
{
	ViStatus	age441x_status =VI_SUCCESS;
	ViChar	    buffer[BUFFER_SIZE];
	
	if (age441x_invalidInstrVersion (instrumentHandle, meas))
		return VI_ERROR_PARAMETER2;   
		
	sprintf (buffer, cmd, meas);
	CHECKERR (viPrintf (instrumentHandle, "%s \"%s\"\n", buffer, value));
	
	return age441x_sysStatus (instrumentHandle);    
}	

/*================================================================================*/
/* Function: Get Meas String  		                                              */
/* Purpose:  This function reads the string from the instrument				      */
/*================================================================================*/
static ViStatus age441x_getMeasString (ViSession instrumentHandle, 
									   ViInt32 meas,
									   ViString cmd,  
									   ViString value)
{
	ViStatus 	age441x_status = VI_SUCCESS;
	ViChar	    buffer[BUFFER_SIZE];
	
	if (age441x_invalidInstrVersion (instrumentHandle, meas))
		return VI_ERROR_PARAMETER2; 
		
	sprintf (buffer, cmd, meas);
	CHECKERR (viPrintf (instrumentHandle, "%s\n", buffer));
	CHECKERR (viScanf (instrumentHandle, "%[^\r\n]", value));
	
	return age441x_sysStatus (instrumentHandle);    
}	

/*================================================================================*/
/* Function: Set Meas Real64  		                                              */
/* Purpose:  This function sends the command with measurement/window or channel   */
/*			 number and ViReal64 parameter to the instrument and checks the status*/
/*================================================================================*/
static ViStatus age441x_setMeasReal64 (ViSession instrumentHandle, 
									   ViInt32 meas,
									   ViString cmd,
									   ViReal64 value)
{
	ViStatus	age441x_status =VI_SUCCESS;
	ViChar	    buffer[BUFFER_SIZE];
	
	if (age441x_invalidInstrVersion (instrumentHandle, meas))
		return VI_ERROR_PARAMETER2;   
		
	sprintf (buffer, cmd, meas);
	CHECKERR (viPrintf (instrumentHandle, "%s %Le\n", buffer, value));
	
	return age441x_sysStatus (instrumentHandle);    
}	

/*================================================================================*/
/* Function: Get MeasReal64  		                                              */
/* Purpose:  This function reads the ViReal64 number from instrument		      */
/*================================================================================*/
static ViStatus age441x_getMeasReal64 (ViSession instrumentHandle, 
									   ViInt32 meas,
									   ViString cmd, 
									   ViReal64 *value)
{
	ViStatus	age441x_status = VI_SUCCESS;
	ViChar	    buffer[BUFFER_SIZE];	
	
	if (age441x_invalidInstrVersion (instrumentHandle, meas))
		return VI_ERROR_PARAMETER2;   
	if (!value)
		return VI_ERROR_PARAMETER3;

	sprintf (buffer, cmd, meas);
	CHECKERR (viPrintf (instrumentHandle, "%s\n", buffer));	
	CHECKERR (viScanf (instrumentHandle, "%Le", value));
	
	
	return age441x_sysStatus (instrumentHandle);    
}	


/*================================================================================*/
/* Function: Set Meas Int32  		                                              */
/* Purpose:  This function sends the command with measurement/window or channel   */
/*			 number and ViInt32 parameter to the instrument and checks the status */
/*================================================================================*/
static ViStatus age441x_setMeasInt32 (ViSession instrumentHandle, 
									   ViInt32 meas,
									   ViString cmd,
									   ViInt32 value)
{
	ViStatus	age441x_status =VI_SUCCESS;
	ViChar	    buffer[BUFFER_SIZE];
	
	if (age441x_invalidInstrVersion (instrumentHandle, meas))
		return VI_ERROR_PARAMETER2;   
		
	sprintf (buffer, cmd, meas);
	CHECKERR (viPrintf (instrumentHandle, "%s %ld\n", buffer, value));
	
	return age441x_sysStatus (instrumentHandle);    
}	

/*================================================================================*/
/* Function: Get Meas Int32  		                                              */
/* Purpose:  This function sends the command with measurement/window or channel   */
/*			 number and reads the ViInt32 from the instrument and checks  status  */
/*================================================================================*/
static ViStatus age441x_getMeasInt32 (ViSession instrumentHandle, 
									   ViInt32 meas,
									   ViString cmd,
									   ViInt32 *value)
{
	ViStatus	age441x_status =VI_SUCCESS;
	ViChar	    buffer[BUFFER_SIZE];
	
	if (age441x_invalidInstrVersion (instrumentHandle, meas))
		return VI_ERROR_PARAMETER2;   
		
	sprintf (buffer, cmd, meas);
	CHECKERR (viPrintf (instrumentHandle, "%s\n", buffer));
	CHECKERR (viScanf (instrumentHandle, "%ld", value));
	
	return age441x_sysStatus (instrumentHandle);    
}	

/*================================================================================*/
/* Function: Set Meas Enum  		                                              */
/* Purpose:  This function sends the command with measurement/window or channel   */
/*			 number and enum parameter to the instrument and checks the status   */
/*================================================================================*/
static ViStatus age441x_setMeasEnum (ViSession instrumentHandle, 
									 ViInt32 meas,
									 ViString cmd,
									 ViInt32 value, 
									 ViString _VI_FAR list[])
{
	ViStatus	age441x_status =VI_SUCCESS;
	ViChar	    buffer[BUFFER_SIZE];
	
	if (age441x_invalidInstrVersion (instrumentHandle, meas))
		return VI_ERROR_PARAMETER2; 
		
	CHECKERR (age441x_checkIndexInList (instrumentHandle, value, list, VI_ERROR_PARAMETER4)); 
	
	sprintf (buffer, cmd, meas);
	CHECKERR (viPrintf (instrumentHandle, "%s %s\n", buffer, list[value]));
	
	return age441x_sysStatus (instrumentHandle);    
}	

/*================================================================================*/
/* Function: Set Meas Boolean  		                                              */
/* Purpose:  This function sends the command with measurement/window or channel   */
/*			 number and boolean parameter to the instrument and checks the status */
/*================================================================================*/
static ViStatus age441x_setMeasBoolean (ViSession instrumentHandle, 
									 	ViInt32 meas,
									 	ViString cmd,
									 	ViBoolean value)
{
	ViStatus	age441x_status =VI_SUCCESS;
	ViChar	    buffer[BUFFER_SIZE];
	
	if (age441x_invalidInstrVersion (instrumentHandle, meas))
		return VI_ERROR_PARAMETER2; 
		
	sprintf (buffer, cmd, meas);
	CHECKERR (viPrintf (instrumentHandle, "%s %s\n", buffer, switchArr[value]));
	
	return age441x_sysStatus (instrumentHandle);    
}

/*================================================================================*/
/* Function: Get Meas Boolean  		                                              */
/* Purpose:  This function sends the command with measurement/window or channel   */
/*			 number and reads boolean value from the instrument.				  */
/*================================================================================*/
static ViStatus age441x_getMeasBoolean (ViSession instrumentHandle, 
									 	ViInt32 meas,
									 	ViString cmd,
									 	ViBoolean *value)
{
	ViStatus	age441x_status =VI_SUCCESS;
	ViChar	    buffer[BUFFER_SIZE];
	
	if (age441x_invalidInstrVersion (instrumentHandle, meas))
		return VI_ERROR_PARAMETER2; 
		
	sprintf (buffer, cmd, meas);
	CHECKERR (viPrintf (instrumentHandle, "%s\n", buffer));
	CHECKERR (viScanf (instrumentHandle, "%u", value));
	
	return age441x_sysStatus (instrumentHandle);    
}


/*================================================================================*/
/* Function: Set Meas Node  		                                              */
/* Purpose:  This function sends the command with measurement/window or channel   */
/*			 number to the instrument and checks the status 					  */
/*================================================================================*/
static ViStatus age441x_setMeasNode (ViSession instrumentHandle, 
									 	ViInt32 meas,
									 	ViString cmd)
{
	ViStatus	age441x_status =VI_SUCCESS;
	ViChar	    buffer[BUFFER_SIZE];
	
	if (age441x_invalidInstrVersion (instrumentHandle, meas))
		return VI_ERROR_PARAMETER2; 
		
	sprintf (buffer, cmd, meas);
	CHECKERR (viPrintf (instrumentHandle, "%s\n", buffer));
	
	return age441x_sysStatus (instrumentHandle);    
}

/*===========================================================================*/
/* Function: Check Index List                                                */
/* Purpose:  This function checks if the passed index is valid for given list*/
/*===========================================================================*/
static ViStatus age441x_checkIndexInList (ViSession instrumentHandle, ViInt32 index, ViString list[], 
                                   			ViStatus statusIfError)
                                      
{
    ViInt32     i=0;
    
    if (index < 0)
        return statusIfError;
    
    while (list[i++] != VI_NULL);
    if (index >= i)
        return statusIfError;
    
    return VI_SUCCESS;  
}

/*===========================================================================*/
/* Function: Get Sensor From User Data								 */
/* Purpose : This function returns the sensor identification from User Data  */
/*===========================================================================*/
static ViInt32 age441x_getSensorFromUserData (ViSession instrumentHandle, ViInt32 channel)
										  	 
{
	
	age441x_instrRange 	instrPtr; 

	viGetAttribute (instrumentHandle, VI_ATTR_USER_DATA, &instrPtr);	

	switch (channel)
	{
		case AGE441X_CHANNEL_A:
			return instrPtr->sensor1;
			
		case AGE441X_CHANNEL_B:	
			return instrPtr->sensor2;
		
		default:
			return AGE441X_SENSOR_NONE;
	}
}	

/*================================================================================*/
/* Function: Wait On VISA Event                                                   */
/* Purpose:  This function waits for the occurrence of an enabled VISA Event. The */
/*           event type must be enabled before entering this function. Any        */
/*           programmatic commands, actions, or conditions, necessary to generate */
/*           the specified event type, must be satisified before entering this    */
/*           function. The function will wait for the specified timeout and then  */
/*           return. If the specified event is received then the function will    */
/*           return VI_SUCCESS (0), otherwise the status code for the generated   */
/*           error will be returned. In either case the function will disable the */
/*           event type and deallocate the event handle passed from viWaitOnEvent */
/*           before returning. WARNING: If VI_TMO_INFINITE is passed in as the    */
/*           timeout this function WILL NOT return until the specified event is   */
/*           received, if the event is not received the function will not return  */
/*           and it will be necessary to terminate CVI in order to regain control.*/
/*================================================================================*/
ViStatus age441x_waitOnVisaEvent (ViSession instrSession, ViEventType eventTypeIn, 
            ViUInt32 timeoutIn, ViPUInt16 STB)
{            
    ViStatus age441x_status = VI_SUCCESS, tempStatus = VI_SUCCESS;
    ViEventType eventTypeOut = 0;
    ViEvent eventHandle = 0;
    
    /* For debug purposes we want to be able to see the status returned by        */
    /* viDisableEvent and viClose if one of the internal functions fails but do   */
    /* not want to return that value as that is not where the initial error       */
    /* occurs in the function, so we assign it to tempStatus.                     */
    
    if ((age441x_status = viWaitOnEvent (instrSession, eventTypeIn, timeoutIn, 
                                        &eventTypeOut, &eventHandle)) < 0) {
        tempStatus = viDisableEvent (instrSession, eventTypeIn, VI_QUEUE);
        return age441x_status;
    } 
    
    if (eventTypeIn == VI_EVENT_SERVICE_REQ)
        if ((age441x_status = viReadSTB (instrSession, STB)) < 0) {
            tempStatus = viClose (eventHandle);
            tempStatus = viDisableEvent (instrSession, eventTypeIn, VI_QUEUE);
            return age441x_status;
        }   
        
    if ((age441x_status = viClose (eventHandle)) < 0) {             
        tempStatus = viDisableEvent (instrSession, eventTypeIn, VI_QUEUE);
        return age441x_status;                 
    }
    
    if ((age441x_status = viDisableEvent (instrSession, eventTypeIn, VI_QUEUE)) < 0)
        return age441x_status;                                                   

    return age441x_status;                                    
}    

/*===========================================================================*/
/* Function: Initialize Clean Up                                             */
/* Purpose:  This function is used only by the age441x_init function.  When   */
/*           an error is detected this function is called to close the       */
/*           open resource manager and instrument object sessions and to     */
/*           set the instrSession that is returned from age441x_init to       */
/*           VI_NULL.                                                        */
/*===========================================================================*/
static ViStatus age441x_initCleanUp (ViSession openRMSession,
                    ViPSession openInstrSession, ViStatus currentStatus)
{
    age441x_instrRange instrPtr;
    
    if (viGetAttribute (*openInstrSession, VI_ATTR_USER_DATA, &instrPtr) >= 0)
        if (instrPtr != NULL) 
            free (instrPtr);

    viClose (*openInstrSession);
    viClose (openRMSession);
    *openInstrSession = VI_NULL;
    
    return currentStatus;
}



/*****************************************************************************/
/*----------- INSERT INSTRUMENT-DEPENDENT UTILITY ROUTINES HERE -------------*/
/*****************************************************************************/

/*===========================================================================*/
/* Function: Default Instrument Setup                                        */
/* Purpose:  This function sends a default setup to the instrument.  This    */
/*           function is called by the age441x_reset operation and by the     */
/*           age441x_init function if the reset option has not been           */
/*           selected.  This function is useful for configuring any          */
/*           instrument settings that are required by the rest of the        */
/*           instrument driver functions such as turning headers ON or OFF   */
/*           or using the long or short form for commands, queries, and data.*/                                    
/*===========================================================================*/
static ViStatus age441x_defaultInstrSetup (ViSession instrSession)
{
    ViStatus 			age441x_status = VI_SUCCESS;
    //ViUInt32 			retCnt = 0;
    age441x_instrRange 	instrPtr;
    ViInt32  			instrNumber;
    ViChar				type[BUFFER_SIZE];
    ViInt32 			tmp;
    ViBoolean			stat;
    

    /* Determine if the structure has been initialized for the current VISA  */
    /* Session and malloc if it has not.                                     */
    if (age441x_status = viGetAttribute (instrSession, VI_ATTR_USER_DATA, &instrPtr))
        return age441x_status;
    
    if (instrPtr == NULL) 
    {
    	instrPtr = calloc (1, sizeof (struct age441x_statusDataRanges));
        CHECKERR (viSetAttribute (instrSession, VI_ATTR_USER_DATA, (ViInt32)instrPtr));
    }
    // Enable status checking by default
    instrPtr -> checkStatus = VI_TRUE;

     // Send initialization string:  
     CHECKERR (viPrintf (instrSession, "*CLS;*ESE 60;:STAT:OPER:NTR 65535;:FORM:READ:DATA ASC\n"));

    //Determine number of channels
    CHECKERR (viPrintf (instrSession, "*IDN?\n"));
    CHECKERR (viScanf (instrSession, "%*[^,],E44%ld%*[^,],%*[^,],%*[^\n]", &instrNumber));     
    instrPtr -> instrVersion = instrNumber;
    instrPtr->sensor1 = AGE441X_SENSOR_NONE;
    instrPtr->sensor2 = AGE441X_SENSOR_NONE;  
    
    switch (instrNumber)
    {
    	case 16:
    	case 18:
    		instrPtr -> channelNumber = 1;
    		
    		break;
    	case 17:
    	case 19:
    		instrPtr -> channelNumber = 2; 
    		//Determine sensor on channel B
    		CHECKERR (age441x_getSensorType (instrSession, AGE441X_CHANNEL_B, &stat, type));
    		if (stat)
    		{	
		    	tmp = 0;
		    	if (!strncmp (type,"E",1))
		    			instrPtr->sensor2 = AGE441X_SENSOR_E;
		    	if (!strncmp (type,"E932",4))
		    		instrPtr->sensor2 = AGE441X_SENSOR_E9320; 
		    	if (instrPtr->sensor2 == AGE441X_SENSOR_NONE)
		    	{
				   	do
				   	{
				   		if (!(strcmp(type, sensorArr[tmp])))
				   		{
				   			instrPtr->sensor2 = AGE441X_SENSOR_8480;
				   			break;
				   		}	
				   		tmp ++;
				   	}while (sensorArr[tmp]!=VI_NULL);
				 }  	
			} 
    		break;
    	default:
    		return AGE441X_ERROR_INSTRUMENT_NUMBER;
    }
    //Detemine sensor on channel A
	CHECKERR (age441x_getSensorType (instrSession, AGE441X_CHANNEL_A, &stat, type));
	if (stat)
	{	
	   	tmp = 0;
	   	if (!strncmp (type,"E",1))  
	   		instrPtr->sensor1 = AGE441X_SENSOR_E;
	   		
		if (!strncmp (type,"E932",4))
			instrPtr->sensor1 = AGE441X_SENSOR_E9320; 
		if (instrPtr->sensor1 == AGE441X_SENSOR_NONE)
		{
			do
		   	{
		   		if (!(strcmp(type, sensorArr[tmp]))) 
		   		{
		   			instrPtr->sensor1 = AGE441X_SENSOR_8480;
		   			break;
		   		}	
		   		tmp ++;
		   	}while (sensorArr[tmp]!=VI_NULL);
		 }  	
	}
   
    return age441x_status;
}

/*=========================================================================*/
/* Function: Status                                                        */
/* Purpose:  This function reads the system status byte.                   */
/*=========================================================================*/
static ViStatus age441x_sysStatus (ViSession instrSession)
{
    ViStatus age441x_status = VI_SUCCESS;
    ViInt32 esrByte;
    age441x_instrRange instrPtr;

    CHECKERR (viGetAttribute (instrSession, VI_ATTR_USER_DATA, &instrPtr));

    if (instrPtr -> checkStatus) { /* Is Error Checking Enabled? */
        /*--------------- Read the Standard Event Register ---------------*/
        CHECKERR (viPrintf (instrSession, "*ESR?\n"));
        CHECKERR (viScanf (instrSession,  "%ld", &esrByte));

        /*--------------- Check if any error or message bit was asserted -*/
        if ((esrByte & 0x3C) != 0)
            age441x_status = AGE441X_ERROR_INSTRUMENT_ERROR;
        else
           age441x_status = VI_SUCCESS;
    }

    return age441x_status;
}
/*===========================================================================*/
/* Function: Delay                                                           */
/* Purpose : With a call to age441x_delay, the current program is suspended  */
/*           from execution for the number of seconds specified by the       */
/*           argument seconds. age441x_delay is accurate to a millisecond.   */
/*           This function is platform independent.                          */
/*===========================================================================*/
void age441x_delay (ViReal64 numberOfSeconds)
{
  clock_t StartTime = clock();

  while (((double)(clock() - StartTime)/CLOCKS_PER_SEC) < (numberOfSeconds));

  return;
}

/*================================================================================*/ 
/* Function: Read Data With Unknown length										  */
/* Description: This function allocate buffer and read data from the instrument   */
/* WARNING:  You must deallocate buffer, if any pointer has been returned.		  */
/*           																	  */
/*================================================================================*/ 
static ViStatus age441x_ReadDataUnknownLength(ViSession instrumentHandle, ViChar **outBuffer, ViUInt32 *count)
{
    ViStatus age441x_status = VI_SUCCESS;
    ViUInt32 actSize, retCnt = 0;
    ViChar   *buffer = VI_NULL, *p2buffer;
    
    *outBuffer = VI_NULL;
    if (count) *count = 0;
    
    /* Allocate Buffer */
    actSize = BUFFER_SIZE;
    buffer = (ViChar *)calloc(actSize, sizeof(ViChar));
    if (buffer == VI_NULL) return VI_ERROR_ALLOC;

    p2buffer = buffer;
    do
        {
        if ((age441x_status = viRead (instrumentHandle, p2buffer, BUFFER_SIZE, &retCnt)) < 0)
            {
            free(buffer);
            *outBuffer = VI_NULL;
            return age441x_status;
            }
        if (retCnt == BUFFER_SIZE) /* Buffer was small, reallocate it */
        {
            actSize += BUFFER_SIZE;
            buffer = (ViChar *)realloc(buffer, actSize);
            if (buffer == VI_NULL) return VI_ERROR_ALLOC;
            p2buffer = buffer + (actSize - BUFFER_SIZE); /* Set pointer to end of data in rallocated buffer */
        }
        else
        {
            *(p2buffer+retCnt) = '\0'; /* For sure set end of string on the end of data */
        }
       } while (age441x_status == 0x3FFF0006); /* if buffer was small, continue with next read */ 
    
    *outBuffer = buffer;
    if (count) *count = (actSize - BUFFER_SIZE) + retCnt;
    
    return age441x_status;
}

/*===========================================================================*/
/* Function: Get Index From String                                           */
/* Purpose:  This function returns index for string.                         */
/*===========================================================================*/
ViStatus age441x_getIndexFromString (ViSession instrSession,
                                   ViString string,
                                   ViString *list,
                                   ViInt32 *index)
{
    ViStatus    age441x_status = VI_SUCCESS;
    ViUInt32    i,
                len;

    for (i = 0; list[i]; i++)
        if ((len = strlen (string)) == strlen (list[i])) {
            if (strncmp(string, list[i], strlen (string)) == 0) break;
        }

    *index = i;

    return (list[i]) ? age441x_status : VI_ERROR_INSTR_INTERPRETING_RESPONSE;
}

/*===========================================================================*/
/* Function: Build Measure Command                                           */
/* Purpose:  This function returns index for string.                         */
/*===========================================================================*/
static ViStatus age441x_buildMeasureCommand (ViSession instrumentHandle,
                                   ViInt32 measurement,
                                   ViInt32 measuringFunction, 
                                   ViInt32 sensor1,
                                   ViInt32 sensor2, 
                                   ViInt32 resolution,
                                   ViReal64	expectedValue,
                                   ViString	baseCmd,
                                   ViBoolean question,
                                   ViChar command[])
{
	ViStatus			age441x_status = VI_SUCCESS;
	ViChar*				pbuffer;
	age441x_instrRange 	instrPtr;
	
	CHECKERR (viGetAttribute (instrumentHandle, VI_ATTR_USER_DATA, &instrPtr));	   
	sensor1 = channelArr[sensor1];
	sensor2 = channelArr[sensor2];     
	
	if (age441x_invalidInstrVersion (instrumentHandle, measurement))
		return VI_ERROR_PARAMETER2;  
	if (age441x_invalidViInt32Range (measuringFunction, 0, 5))
		return VI_ERROR_PARAMETER3;
	if (age441x_invalidViInt32Range (sensor1, 1, instrPtr->channelNumber))	
		return VI_ERROR_PARAMETER4;
	if (age441x_invalidViInt32Range (sensor2, 1, instrPtr->channelNumber))	
		return VI_ERROR_PARAMETER5;		
	if (age441x_invalidViInt32Range (resolution, 1, 4))	
		return VI_ERROR_PARAMETER6;		

	pbuffer = command;
	pbuffer += sprintf (pbuffer, "%s%ld%s", baseCmd, measurement, measFuncArr[measuringFunction]);
	//Add question mark, if question selected
	if (question) pbuffer += sprintf (pbuffer, "?");      
	
	if (expectedValue)
		pbuffer += sprintf (pbuffer, " %Le,%ld,(@%ld)", expectedValue, resolution, sensor1);
	else
		pbuffer += sprintf (pbuffer, " DEF,%ld,(@%ld)", resolution, sensor1);
	
	switch (measuringFunction)
	{
		case 0:
		case 1:
			break;
		default:
			pbuffer += sprintf(pbuffer, ",(@%ld)", sensor2);
	}
	
	return age441x_status;
}

/*===========================================================================*/
/* Function: Get Element Index		                                         */
/* Purpose:  This function returns index of element		                     */
/*===========================================================================*/
static ViInt32 age441x_getViInt32ElementIndex (ViInt32 element, ViInt32* array)
{
	ViInt32	i;
	
	i=0;
	while (array[i])
	{
		if (array[i] == element) return i;
		i++;
	}	
	
	return -1;	
}		
			
/*===========================================================================*/
/* Function: Wait for OPC                                                    */
/* Purpose:  This function sends a command to the instrument and waits until */
/*           the instrument response is available (queries MAV in STB).      */
/*===========================================================================*/
static ViStatus age441x_waitOPC (ViSession instrSession, ViPString command,
                		  ViPString instrResp, ViInt32 timeout)
{
    ViStatus age441x_status = VI_SUCCESS;
    ViInt16 response = 0;
    ViInt32 counter = 0;
    ViInt32 oldTimeout = 0;
    ViUInt16 interface = VI_INTF_GPIB;

    CHECKERR (viGetAttribute (instrSession, VI_ATTR_INTF_TYPE, &interface));

    /* Interface specific operations */
    switch (interface)
        {
        case VI_INTF_ASRL: /* Serial interface */
            CHECKERR (viGetAttribute (instrSession, VI_ATTR_TMO_VALUE, &oldTimeout));
            CHECKERR (viSetAttribute (instrSession, VI_ATTR_TMO_VALUE, timeout));
            CHECKERR (viPrintf (instrSession, "%s\n", command));
            if ((age441x_status = viScanf (instrSession, "%[^\r\n]", instrResp)) < 0)
            {
                viSetAttribute (instrSession, VI_ATTR_TMO_VALUE, oldTimeout);
                return age441x_status;
            }
            CHECKERR (viSetAttribute (instrSession, VI_ATTR_TMO_VALUE, oldTimeout));
        break;
        default: /* Default case */
            CHECKERR (viPrintf (instrSession, "%s\n", command));
    
            while (!(response & 0x10)) /* (queries MAV in STB) */
                {

                CHECKERR (viReadSTB (instrSession, &response));

                age441x_delay (0.005);
                counter+= 5;
                if (counter >= timeout)
                    return VI_ERROR_TMO;
                }

            CHECKERR (viScanf (instrSession, "%[^\n]", instrResp));
        break;
        }

    return age441x_status;
}

/*===========================================================================*/
/* Function: Swap 32bit Integer                                              */
/* Purpose:  Byte order for binary transfers depends on MOTOROLA or INTEL    */
/*           formats. For Trace Data only.                                   */
/*===========================================================================*/
void age441x_SWAP_32(unsigned char *data, long points)
{
    /* MOTOROLA (byte order 0123) or INTEL (byte order 3210). */
    unsigned int    word = 0x1122;
    unsigned char   *p2word = (unsigned char*)&word;

    if (*p2word == 0x22) /* INTEL format in PC (Instrument returns MOTOROLA format)*/
    {
       unsigned char    src[4];
       long             *p2data = (long*)data;
       int              idx;

       for (idx = 0; idx < points; idx++, p2data++)
          {
              *((long *)src) = *(p2data);
              ((unsigned char *)(p2data))[0] =  ((unsigned char*)(src))[3];
              ((unsigned char *)(p2data))[1] =  ((unsigned char*)(src))[2];
              ((unsigned char *)(p2data))[2] =  ((unsigned char*)(src))[1];
              ((unsigned char *)(p2data))[3] =  ((unsigned char*)(src))[0];
          }
    }
    
   return;
}


/*****************************************************************************/
/*=== END INSTRUMENT DRIVER SOURCE CODE =====================================*/
/*****************************************************************************/
