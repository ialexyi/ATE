#include <utility.h>
#include <visa.h>
#include <ansi_c.h>
#include <formatio.h>
#include "hp8563e.h"

/*= DEFINES =================================================================*/
#define hp8563e_REVISION     "Rev 1.0, 06/97, CVI 4.0.1" /*  Instrument driver revision */
#define BUFFER_SIZE         512L            /* File I/O buffer size */
#define REALLY_LARGE_BUFFER_SIZE    9400L   /* Large File I/O buffer size */
    
/*= HP8563 E-Series Spectrum Analyzer =======================================*/
/* LabWindows/CVI 4.0.1 Instrument Driver                                    */
/* Original Release:                                                         */
/* By: TKN, NI                                                               */
/*     PH. 800-433-3488   Fax 512-794-5678                                   */
/* 																			 */
/* Modification History: 11/16/97 NA.                                        */
/*                  Replaced all the hp8563e_writeAndWait() calls with a     */
/*                  simple viWrite().  There was a timing issue where        */
/*                  no SRQ would be generated.                               */
/*                  This causes the function to timeout.                     */
/*                  A similar problem occurs with the hp8563e_waitUntilDone()*/
/*                  calls in the application function.  These calls were     */
/*                  commented out.                                           */
/*                  the command array commandOptns[] required a semicolon ";"*/
/*                  between options (i.e., before SNGLS and  CONTS) 		 */
/* 																			 */
/*							11/10/04 										 */
/*					Command correction by Masha Karpov					 	 */
/*					Function hp8563e_confAmpltde1.
/*							12/10/04
/*					Command edition for the marker MKNOISE;
/*                                                                           */
/*                                                                           */
/* Important Note:  The VISA default time-out value is changed to 30         */
/*                  seconds in the init function.                            */
/*===========================================================================*/
 
/*****************************************************************************/
/*= INSTRUMENT-DEPENDENT COMMAND ARRAYS =====================================*/
/*****************************************************************************/
static ViString freqOptns[] = {"CF","FA","FB","FOFFSET"};
static ViString freqTypeOptns[] = {"","FULL","ZERO","LAST"};
static ViString amplitOptns[] = {"LN","LG","LG","LG","LG"};
static ViString amplitOptns2[] = {"","1","2","5","10"};
static ViString offOn[] = {"OFF","ON"};
static ViString sweepOptns[] = {"SR","SA"};
static ViString intExt[] = {"INT","EXT"};
static ViString extMixOptns[] = {"PRE","UNPR"};
static ViString fullOptns[] = {"A","D","E","F","G","J","K","Q","U","V","W","Y"};
static ViString sigIDOptns[] = {"OFF","AUTO","MAN"};
static ViString demodOptns[] = {"OFF","AM","FM"};
static ViString sweepOptns2[] = {"FAV","FAVA","RAMP"};
static ViString methodOptns[] = {"ANALOG","PEAK","TWOBW","BURST","GATED"};
static ViString accelOptns[] = {"NRM","FASTR","FASTS"};
static ViString measOptns[] = {"DFLT","CURR"};
static ViString freqWghtOptns[] = {"OFF","RRCOS"};
static ViString freqResOptns[] = {"1.0 HZ","10 HZ","100 HZ","1.0 KHZ","10 KHZ","100 KHZ","1.0 MHZ"};
//static ViString autoManOptns[] = {"AUTO","MAN"};
static ViString sweepModeOptns[] = {"CONTS","SNGLS"};
static ViString negPos[] = {"NEG","POS"};
static ViString cntlOptns[] = {"LEVEL","EDGE"};
static ViString trigOptns[] = {"FREE","VID","LINE","EXT"};
static ViString typeOptns[] = {"RBR","VBR"};
static ViString cntlOptns2[] = {"","RB","VB","ST","AT","SS"};
static ViString unitOptns[] = {"","HZ","HZ","MS","DB","HZ"};
static ViString detOptns[] = {"NEG","NRM","POS","SMP"};
static ViString winOptns[] = {"FlATTOP","HANNING","UNIFORM"};
static ViString calibrOptns[] = {"ADJALL","ADJIF FULL","ADJIF CURR"};
static ViString downUpOptns[] = {"DN","UP"};
static ViString traceBA[] = {"TRB","TRA"};
static ViString commandOptns[] = {"ACPCOMPUTE","ACPMEAS; SNGLS","ACPMEAS; CONTS"};
static ViString commandOptns2[] = {"APB","AXB","BML","CLRW","MXMH","MINH","VIEW","BLANK"};
static ViString markerOptns[] = {"MKN","MKT","MKD","MKCF","MKMIN","MKPK HI","MKPK NH","MKPK NR","MKPK NL", "MKNOISE ON", "MKNOISE OFF"}; // MKNOISE ON, OFF added by Masha Karpova 13/10/04
static ViString commandOptns3[] = {"MKCF","MKRL","MKSP","MKSS","IDCF","MKCHEDGE","MKDELCHBW","MKMCF"};
static ViString unitOptns2[] = {"MS","HZ"};
static ViString ACPOptns[] = {"ACPUPPER","ACPLOWER","ACPMAX","ACPPWRTX"};
static ViString carrierOptns[] = {"CARROFF","CARRON"};
static ViString powerOptns[] = {"CHANPWR","MEANPWR","DELMKBW","PWRBW"};
static ViString unitOptns3[] = {"HZ","","",""};
static ViString freqOptns2[] = {"IDFREQ","MKF","MKD","CF","FA","FB","SS","SP"};
static ViString saveRecallOptns[2][4] = {{"SAVES","SAVET TRA,","SAVET TRB,","AMPCORSAVE"},
                                         {"RCLS","RCLT TRA,","RCLT TRB,","AMPCORRCL"}};    
static ViString refOptns[] = {"STOREOPEN","STORESHORT","STORETHRU","RCLOSCAL","RCLTHRU"};                                         
//static ViString srcOptns[] = {"ANNT","GRT","TRB","TRA","ALL"};
//static ViString originOptns[] = {"DSP","GRT"};
static ViString bwOptns[] = {"MKBW","CHPWRBW","RB","VB"};
static ViString queryOptns[] = {"AT","AMPCORSIZE","PSDAC"};
static ViString amplitOptns3[] = {"MKA","RL","AT"};

/*****************************************************************************/
/*= INSTRUMENT-DEPENDENT STATUS/RANGE STRUCTURE  ============================*/
/*****************************************************************************/
/* hp8563e_stringValPair is used in the hp8563e_errorMessage function         */
/* hp8563e_statusDataRanges is used to track session dependent status & ranges*/      
/*============================================================================*/
typedef struct  hp8563e_stringValPair
{
   ViStatus stringVal;
   ViString stringName;
}  hp8563e_tStringValPair;
 
struct hp8563e_statusDataRanges {
    ViInt16 triggerMode;
    ViInt16 val2;
    ViInt16 val3;
    ViChar instrDriverRevision[256];
};

typedef struct hp8563e_statusDataRanges *hp8563e_instrRange;

/*****************************************************************************/
/*= UTILITY ROUTINE DECLARATIONS (Non-Exportable Functions) =================*/
/*****************************************************************************/
ViBoolean hp8563e_invalidViBooleanRange (ViBoolean val);
ViBoolean hp8563e_invalidViInt16Range (ViInt16 val, ViInt16 min, ViInt16 max);
ViBoolean hp8563e_invalidViInt32Range (ViInt32 val, ViInt32 min, ViInt32 max);
ViBoolean hp8563e_invalidViReal64Range (ViReal64 val, ViReal64 min, ViReal64 max);
ViStatus hp8563e_waitOnVisaEvent (ViSession instrSession, ViEventType eventTypeIn, ViUInt32 tmoIn, ViPUInt16 STB);
ViStatus hp8563e_initCleanUp (ViSession openRMSession, ViPSession openInstrSession, ViStatus currentStatus);
ViStatus hp8563e_defaultInstrSetup (ViSession openInstrSession);

/*****************************************************************************/
/*=========== INSTRUMENT-DEPENDENT UTILITY ROUTINE DECLARATIONS =============*/
/*****************************************************************************/
ViStatus hp8563e_writeAndWait (ViSession instrSession, ViString wrtBuf, 
            ViUInt32 Count, ViPUInt32 retCnt);

/*****************************************************************************/
/*====== USER-CALLABLE FUNCTIONS (Exportable Functions) =====================*/
/*****************************************************************************/
/*===========================================================================*/
/* Function: Initialize                                                      */
/* Purpose:  This function opens the instrument, queries the instrument      */
/*           for its ID, and initializes the instrument to a known state.    */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_init (ViRsrc resourceName, ViBoolean IDQuery,
                    ViBoolean resetDevice, ViPSession instrSession)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViSession rmSession = 0;
    ViUInt32 retCnt = 0;
    ViByte rdBuffer[BUFFER_SIZE];    

    /*- Check input parameter ranges ----------------------------------------*/
    if (hp8563e_invalidViBooleanRange (IDQuery))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViBooleanRange (resetDevice))
        return VI_ERROR_PARAMETER3;

    /*- Open instrument session ---------------------------------------------*/
    if ((hp8563e_status = viOpenDefaultRM (&rmSession)) < 0)
        return hp8563e_status;

    if ((hp8563e_status = viOpen (rmSession, resourceName, VI_NULL, VI_NULL, instrSession)) < 0) {
        viClose (rmSession);
        return hp8563e_status;
    }

    /*- Configure VISA Formatted I/O ----------------------------------------*/
    if ((hp8563e_status = viSetAttribute (*instrSession, VI_ATTR_TMO_VALUE, 10000)) < 0)
            return hp8563e_initCleanUp (rmSession, instrSession, hp8563e_status);
  
    if ((hp8563e_status = viSetBuf (*instrSession, VI_READ_BUF|VI_WRITE_BUF, 4000)) < 0)
            return hp8563e_initCleanUp (rmSession, instrSession, hp8563e_status);
  
    if ((hp8563e_status = viSetAttribute (*instrSession, VI_ATTR_WR_BUF_OPER_MODE,
                            VI_FLUSH_ON_ACCESS)) < 0)
            return hp8563e_initCleanUp (rmSession, instrSession, hp8563e_status);
    
    if ((hp8563e_status = viSetAttribute (*instrSession, VI_ATTR_RD_BUF_OPER_MODE,
                            VI_FLUSH_ON_ACCESS)) < 0)
            return hp8563e_initCleanUp (rmSession, instrSession, hp8563e_status);

    /*- Identification Query ------------------------------------------------*/
    if (IDQuery) {
        if ((hp8563e_status = viWrite (*instrSession, (unsigned char*)"ID?;", 4, (unsigned int*)&retCnt)) < 0)
            return hp8563e_initCleanUp (rmSession, instrSession, hp8563e_status);
        if ((hp8563e_status = viRead (*instrSession, rdBuffer, BUFFER_SIZE, (unsigned int*)&retCnt)) < 0)
            return hp8563e_status;

        Scan (rdBuffer, "HP8563E");
        if (NumFmtdBytes () != 5) //Marina (7)
            return hp8563e_initCleanUp (rmSession, instrSession, VI_ERROR_FAIL_ID_QUERY);
    }
    
    /*- Reset instrument ----------------------------------------------------*/
    if (resetDevice) {
        if ((hp8563e_status = hp8563e_reset (*instrSession)) < 0)
            return hp8563e_initCleanUp (rmSession, instrSession, hp8563e_status);
    }       
    
    /*- Changes the default time-out value to 30 seconds --------------------*/
    if ((hp8563e_status = viSetAttribute (*instrSession, VI_ATTR_TMO_VALUE, 30000)) < 0)
        return hp8563e_status;
    
    else  /*- Send Default Instrument Setup ---------------------------------*/
        if ((hp8563e_status = hp8563e_defaultInstrSetup (*instrSession)) < 0)
            return hp8563e_initCleanUp (rmSession, instrSession, hp8563e_status);
    return hp8563e_status;
}

/*===========================================================================*/
/* Function: Making a Basic Measurement                                      */
/* Purpose:  Sets the center frequency and span and then finds the highest   */
/*           peak in the signal.                                             */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_makeBasicMeas (ViSession instrSession, ViReal64 freq, 
                    ViReal64 span, ViBoolean mark, ViPReal64 refLvl, 
                    ViPReal64 mkFreq, ViPReal64 mkAmplit, ViReal64 _VI_FAR array[])
{
    ViStatus hp8563e_status = VI_SUCCESS;

    if (hp8563e_invalidViReal64Range (freq, 0.0, 40E9))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViReal64Range (span, 0.0, 40E9))
        return VI_ERROR_PARAMETER3;
    if (hp8563e_invalidViBooleanRange (mark))
        return VI_ERROR_PARAMETER4;
        
    if ((hp8563e_status = hp8563e_configFreq (instrSession, 0, freq)) < 0)
        return hp8563e_status;   
    if ((hp8563e_status = hp8563e_configSpan (instrSession, 0, span)) < 0)  
        return hp8563e_status;   
    //if ((hp8563e_status = hp8563e_waitUntilDone (instrSession, 30000)) < 0)
    //    return hp8563e_status;
        
    Delay (1.0);
        
    if ((hp8563e_status = hp8563e_placeMarker (instrSession, 5, 0)) < 0)
        return hp8563e_status;  
    //if ((hp8563e_status = hp8563e_waitUntilDone (instrSession, 30000)) < 0)
    //    return hp8563e_status; 
    if (mark == VI_ON) 
        if ((hp8563e_status = hp8563e_perfmTo (instrSession, 1)) < 0)
            return hp8563e_status;
    
    Delay (1.0);
        
    if ((hp8563e_status = hp8563e_queryAmpl (instrSession, 1, refLvl)) < 0)
        return hp8563e_status;    
    if ((hp8563e_status = hp8563e_queryFreq (instrSession, 1, mkFreq)) < 0)
        return hp8563e_status;  
    if ((hp8563e_status = hp8563e_queryAmpl (instrSession, 0, mkAmplit)) < 0)
        return hp8563e_status; 
    if ((hp8563e_status = hp8563e_queryTrace (instrSession, VI_ON, array)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config Frequency                                                */
/* Purpose:  This function sets the Center Frequency, Start Frequency,       */
/*           Stop Frequency, and the Frequency Offset.                       */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_configFreq (ViSession instrSession, ViInt16 freq, 
                    ViReal64 freqVal)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViInt16Range (freq, 0, 3))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViReal64Range (freqVal, 0.0, 40.0e9))
        return VI_ERROR_PARAMETER3;

    Fmt (wrtBuf, "%s<%s %f HZ;", freqOptns[freq], freqVal);
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*)wrtBuf, NumFmtdBytes (), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config Span                                                     */
/* Purpose:  This function sets the Frequency Span.                          */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_configSpan (ViSession instrSession, ViInt16 freqType,
                    ViReal64 freqSpan)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViInt16Range (freqType, 0, 3))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViReal64Range (freqSpan, 0.0, 40E9))
        return VI_ERROR_PARAMETER3;

    if (freqType == 0)         
        Fmt (wrtBuf, "%s<SP %f HZ;", freqSpan);
    else
        Fmt (wrtBuf, "%s<SP %s;", freqTypeOptns[freqType]);
            
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf, NumFmtdBytes (), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config Amplitude (1)                                            */
/* Purpose:  This function sets the Reference Level and Amplitude Scale.     */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confAmpltde1 (ViSession instrSession, ViInt16 amplit,
                    ViReal64 refLvl)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViInt16Range (amplit, 0, 4))
        return VI_ERROR_PARAMETER2;
    if (amplit == 0) {
        if (hp8563e_invalidViReal64Range (refLvl, 2.236e-3, 7.071e3))
            return VI_ERROR_PARAMETER3;
            
        Fmt (wrtBuf, "%s<%s;RL %f MV;", amplitOptns[amplit], refLvl);
    }
    else {
        if (hp8563e_invalidViReal64Range (refLvl, -120.0, 30.0))
            return VI_ERROR_PARAMETER3;
         
        Fmt (wrtBuf, "%s<%s %s;RL %f DBM;", amplitOptns[amplit], amplitOptns2[amplit], refLvl);
        //Fmt (wrtBuf, "%s<%s %s;RL %f;", amplitOptns[amplit], amplitOptns2[amplit], refLvl);	 // added by Masha
    }
    
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf, NumFmtdBytes (), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config Amplitude (2)                                            */
/* Purpose:  This function sets the Amplitude Reference Offset and           */
/*           Mixer Level.                                                    */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confAmpltde2 (ViSession instrSession, ViReal64 amplitOffset,
                    ViInt16 mixerLvl)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViReal64Range (amplitOffset, -100.0, 100.0))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViInt16Range (mixerLvl, -80, -10))
        return VI_ERROR_PARAMETER3;

    Fmt (wrtBuf, "%s<ROFFSET %f DB;ML %d[b2] DBM;", amplitOffset, mixerLvl);
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf, NumFmtdBytes (), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          
                      
/*===========================================================================*/
/* Function: Config Amplitude (3)                                            */
/* Purpose:  This function sets the Normalized Reference Position and        */
/*           Preselector DAC Number.                                         */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confAmpltde3 (ViSession instrSession, ViReal64 normRef,
                    ViInt16 presel)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViReal64Range (normRef, 0.0, 10.0))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViInt16Range (presel, -1, 255))
        return VI_ERROR_PARAMETER3;

    if (presel == -1)
        Fmt (wrtBuf, "%s<NRL %f DB;", normRef);
    else
        Fmt (wrtBuf, "%s<NRL %f DB;PSDAC %d[b2];", normRef, presel);
            
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf, NumFmtdBytes (), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config Calibration                                              */
/* Purpose:  This function sets the Automatic IF Adjustment, Reference Level */
/*           Calibration, and Amplitude Correction.                          */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confCalibratn (ViSession instrSession, ViBoolean autoIF,
                    ViBoolean amplit)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViBooleanRange (autoIF))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViBooleanRange (amplit))
        return VI_ERROR_PARAMETER3;

    Fmt (wrtBuf, "%s<ADJIF %s;AMPCOR %s;", offOn[autoIF], offOn[amplit]);
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf, NumFmtdBytes (), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config Reference Level                                          */
/* Purpose:  This function allows you to calibrate the reference level       */
/*           remotely.                                                       */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confRefLvl (ViSession instrSession, ViInt16 refLvl)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViInt16Range (refLvl, -528, 528))
        return VI_ERROR_PARAMETER2;

    Fmt (wrtBuf, "%s<RLCAL %d[b2];", refLvl);
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf, NumFmtdBytes (), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config Freq-Ampl Correction Pts                                 */
/* Purpose:  This function Enters an individual frequency-amplitude          */
/*           correction point. This function allows you to enter the         */
/*           frequency-amplitude correction points that are used to          */
/*           normalize the spectrum analyzer measurement.                    */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_configFreqAmplCorrectn (ViSession instrSession,
                    ViInt16 num, ViReal64 _VI_FAR freq[], ViReal64 _VI_FAR amplit[])
{
    ViStatus			hp8563e_status							=	VI_SUCCESS;
	
    ViChar				wrtBuf[REALLY_LARGE_BUFFER_SIZE]		=	{0};
	
    ViInt32 			retCnt									=	0,
						cnt										=	0;
	
    ViInt16				i										=	0;

    if (hp8563e_invalidViInt16Range (num, 1, 200))
        return VI_ERROR_PARAMETER2;
    for (i = 0; i < num; i++) {
        if (hp8563e_invalidViReal64Range (freq[i], 0.0, 325.0e9))
            return VI_ERROR_PARAMETER3;
        if (hp8563e_invalidViReal64Range (amplit[i], -109.0, 109.0))
            return VI_ERROR_PARAMETER4;
    }
    
    Fmt (wrtBuf, "%s<AMPCORDATA %f HZ, %f DB", freq[0], amplit[0]);
    cnt = NumFmtdBytes ();

    for (i = 1; i < num; i++) {
        Fmt (wrtBuf, "%s[a]<,%f HZ, %f DB", freq[i], amplit[i]);
        cnt += NumFmtdBytes ();
    }
    
    Fmt (wrtBuf, "%s[a]<;");
    cnt ++;
            
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf, cnt, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          
                                         
/*===========================================================================*/
/* Function: Config Track Generator                                          */
/* Purpose:  This function sets the Sweep Couple.                            */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confTrkGen (ViSession instrSession, ViBoolean sweep)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViBooleanRange (sweep))
        return VI_ERROR_PARAMETER2;

    Fmt (wrtBuf, "%s<SWPCPL %s;", sweepOptns[sweep]);
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf, NumFmtdBytes (), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config Normalize                                                */
/* Purpose:  This function sets the Normalize, Normalized Ref Level, and     */
/*           Normalized Ref Position.                                        */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confNorm (ViSession instrSession, ViBoolean norm,
                    ViReal64 normLvl, ViReal64 normPos)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViBooleanRange (norm))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViReal64Range (normLvl, -100.0, 100.0))
        return VI_ERROR_PARAMETER3;
    if (hp8563e_invalidViReal64Range (normPos, 0.0, 10.0))
        return VI_ERROR_PARAMETER4;

    Fmt (wrtBuf, "%s<NRL %f DB;NRPOS %f;NORMLIZE %s;", normLvl, normPos, offOn[norm]);
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf, NumFmtdBytes (), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          
                                    
/*===========================================================================*/
/* Function: Config Video Average                                            */
/* Purpose: This function sets the Video Average.                            */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confVidAvg (ViSession instrSession, ViBoolean vid,
                    ViInt16 vidAvg)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViBooleanRange (vid))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViInt16Range (vidAvg, 1, 999))
        return VI_ERROR_PARAMETER3;
        
    if (vid == VI_FALSE)
        Fmt (wrtBuf, "%s<VAVG OFF;");
    else
        Fmt (wrtBuf, "%s<VAVG %d[b2];", vidAvg);        
    
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          
                                      
/*===========================================================================*/
/* Function: Config Mixer                                                    */
/* Purpose: This function sets the Mixer Mode and External Mixer Mode.       */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confMix (ViSession instrSession, ViBoolean mix,   
                    ViBoolean extMix)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViBooleanRange (mix))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViBooleanRange (extMix))
        return VI_ERROR_PARAMETER3;

    Fmt (wrtBuf, "%s<MXRMODE %s;EXTMXR %s;", intExt[mix], extMixOptns[extMix]);
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          
                                   
/*===========================================================================*/
/* Function: Config External Mixer                                           */
/* Purpose:  This function sets the Full Band, Conversion Loss, Mixer Bias,  */
/*           and the Harmonic Number Lock parameters.                        */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confExternMix (ViSession instrSession, ViInt16 full, 
                    ViReal64 convn, ViReal64 mixBias)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViInt16Range (full, 0, 11))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViReal64Range (convn, 15.0, 60.0))
        return VI_ERROR_PARAMETER3;
    if (hp8563e_invalidViReal64Range (mixBias, -0.01, 0.01))
        return VI_ERROR_PARAMETER4;

    Fmt (wrtBuf, "%s<FULBAND %s;CNVLOSS %f DB;MBIAS %f MA;", fullOptns[full],
                 convn, mixBias);
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config Harmonic Number Lock                                     */
/* Purpose:  This function sets the Harmonic Number Lock.                    */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confHarmNumLock (ViSession instrSession, ViInt16 lock)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViInt16Range (lock, 0, 54))
        return VI_ERROR_PARAMETER2;
        
    if (lock == 0)    
        Fmt (wrtBuf, "%s<HNLOCK OFF;");
    else
        Fmt (wrtBuf, "%s<HNLOCK %d[b2];", lock);
    
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config Signal ID & Preselector                                  */
/* Purpose:  This function sets the Signal Identification and Preselector    */
/*           DAC Number                                                      */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confSignIDandPresel (ViSession instrSession, 
                    ViInt16 sigId, ViInt16 presel)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViInt16Range (sigId, 0, 2))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViInt16Range (presel, -1, 255))
        return VI_ERROR_PARAMETER3;

    if (presel == -1)
        Fmt (wrtBuf, "%s<SIGID %s;", sigIDOptns[sigId]);
    else
        Fmt (wrtBuf, "%s<SIGID %s;PSDAC %d[b2];", sigIDOptns[sigId], presel);
            
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
        
    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config AM/FM Demodulation                                       */
/* Purpose:  This function sets the Demodulation, Squelch, and Demod         */
/*           Automatic Gain Control parameters.                              */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_configDemod (ViSession instrSession, ViInt16 demod,
                    ViReal64 demodTm, ViBoolean sqlch, ViReal64 sqlchLvl, 
                    ViBoolean demodAuto)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViInt16Range (demod, 0, 2))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViReal64Range (demodTm, 100.0, 60.0e+3))
        return VI_ERROR_PARAMETER3;
    if (hp8563e_invalidViBooleanRange (sqlch))
        return VI_ERROR_PARAMETER4;
    if (hp8563e_invalidViReal64Range (sqlchLvl, -220.0, 30.0))
        return VI_ERROR_PARAMETER5;
    if (hp8563e_invalidViBooleanRange (demodAuto))
        return VI_ERROR_PARAMETER6;

    if (sqlch == VI_FALSE)
        Fmt (wrtBuf, "%s<DEMODT %f MS;DEMOD %s;DEMODAGC %s;SQUELCH OFF;", demodTm, 
                demodOptns[demod], offOn[demodAuto]);
    else
        Fmt (wrtBuf, "%s<DEMODT %f MS;DEMOD %s;DEMODAGC %s;SQUELCH %f DBM;", demodTm, 
                demodOptns[demod], offOn[demodAuto], sqlchLvl);
            
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config Rear Panel                                               */
/* Purpose:  This function sets the Frequency Reference and Sweep Output     */
/*           parameters.                                                     */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confRearPanel (ViSession instrSession, ViBoolean freq,    
                    ViInt16 sweep)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViBooleanRange (freq))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViInt16Range (sweep, 0, 2))
        return VI_ERROR_PARAMETER3;

    Fmt (wrtBuf, "%s<FREF %s;SWPOUT %s;", intExt[freq], sweepOptns2[sweep]);
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config Channel Power                                            */
/* Purpose:  This function sets the Channel Power Bandwidth and Percent      */
/*           Occupied Power Bandwidth.                                       */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confChanPow (ViSession instrSession, ViBoolean pwrType, 
                    ViReal64 pwr)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViBooleanRange (pwrType))
        return VI_ERROR_PARAMETER2;
    if (pwrType == VI_FALSE) {
        if (hp8563e_invalidViReal64Range (pwr, 1.00, 99.99))
            return VI_ERROR_PARAMETER3;
            
        Fmt (wrtBuf, "%s<OCCUP %f;", pwr);
    }
    else {
        if (hp8563e_invalidViReal64Range (pwr, 200.00, 50.00e+9))
            return VI_ERROR_PARAMETER3;
            
        Fmt (wrtBuf, "%s<CHPWRBW %f HZ;", pwr);
    }
    
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config ACP Main                                                 */
/* Purpose:  This function sets the ACP Method, Accelerator, Measurement     */
/*           State, and Graph Mode.                                          */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confACPmain (ViSession instrSession, ViInt16 method,       
                    ViInt16 accel, ViBoolean meas, ViBoolean graph)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViInt16Range (method, 0, 4))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViInt16Range (accel, 0, 2))
        return VI_ERROR_PARAMETER3;
    if (hp8563e_invalidViBooleanRange (meas))
        return VI_ERROR_PARAMETER4;
    if (hp8563e_invalidViBooleanRange (graph))
        return VI_ERROR_PARAMETER5;
    
    Fmt (wrtBuf, "%s<ACPMETHOD %s;ACPACCL %s;ACPMSTATE %s;ACPGRAPH %s;", 
              methodOptns[method], accelOptns[accel], measOptns[meas], offOn[graph]);
              
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          
                                       
/*===========================================================================*/
/* Function: Config ACP Channels                                             */
/* Purpose:  This function sets the ACP Channel Spacing, Channel Bandwidth,  */
/*           and number of Alternate Channels parameters.                    */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confACPchan (ViSession instrSession, ViReal64 chanSpc,
                    ViReal64 chanBand, ViInt16 altChan)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViReal64Range (chanSpc, 100.0, 25.0e9))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViReal64Range (chanBand, 200.0, 50.0e9))
        return VI_ERROR_PARAMETER3;
    if (hp8563e_invalidViInt16Range (altChan, 0, 2))
        return VI_ERROR_PARAMETER4;
    
    Fmt (wrtBuf, "%s<ACPSP %f HZ;ACPBW %f HZ;ACPALTCH %d[b2];", chanSpc, 
                  chanBand, altChan); 
    
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          
                                       
/*===========================================================================*/
/* Function: Config ACP Burst                                                */
/* Purpose:  This function sets the ACP Burst Period and Burst Width.        */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confACPburst (ViSession instrSession, ViReal64 period,
                    ViReal64 width)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViReal64Range (period, 50.0e-3, 10.0e3))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViReal64Range (width, 5e-3, 9.5e3))
        return VI_ERROR_PARAMETER3;
    
    Fmt (wrtBuf, "%s<ACPBRPER %f MS;ACPBRWID %f MS;", period, width); 
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          
                                       
/*===========================================================================*/
/* Function: Config ACP Weightings                                           */
/* Purpose:  This function sets the ACP Frequency Weighting, T Weighting,    */
/*           and the Alpha Weighting.                                        */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confACPweigth (ViSession instrSession, ViBoolean freqWght,
                    ViReal64 TWgh, ViReal64 alphaWgh)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViBooleanRange (freqWght))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViReal64Range (TWgh, 1.0e-3, 1.0e3))
        return VI_ERROR_PARAMETER3;
    if (hp8563e_invalidViReal64Range (alphaWgh, 0.000, 1.000))
        return VI_ERROR_PARAMETER4;
    
    Fmt (wrtBuf, "%s<ACPFRQWT %s;ACPT %f MS;ACPALPHA %f;", freqWghtOptns[freqWght],
            TWgh, alphaWgh); 
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          
                                       
/*===========================================================================*/
/* Function: Config Marker                                                   */
/* Purpose:  This function sets the Marker Noise and Signal Track.           */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confMkr (ViSession instrSession, ViBoolean mark, 
                    ViBoolean sign)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViBooleanRange (mark))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViBooleanRange (sign))
        return VI_ERROR_PARAMETER3;

    Fmt (wrtBuf, "%s<MKNOISE %s;MKTRACK %s;", offOn[mark], offOn[sign]); 
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config Frequency Counter                                        */
/* Purpose:  This function sets the Frequency Counter and Frequency Counter  */
/*           Resolution.                                                     */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confFreqCnt (ViSession instrSession, ViBoolean freqCnt,
                    ViInt16 freqRes)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViBooleanRange (freqCnt))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViInt16Range (freqRes, 0, 6))
        return VI_ERROR_PARAMETER3;
    
    Fmt (wrtBuf, "%s<MKFCR %s;MKFC %s;", freqResOptns[freqRes], offOn[freqCnt]); 
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config Peak Search                                              */
/* Purpose:  This function sets the Marker Threshold and Peak Excursion.     */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confPeakSearch (ViSession instrSession, ViReal64 mark, 
                    ViReal64 peak)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE], rdBuf[BUFFER_SIZE];
    ViInt32 retCnt;
    
    if (hp8563e_invalidViReal64Range (mark, -200.0, 30.0))
        return VI_ERROR_PARAMETER2;
        
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) "LG?;", 4, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
    if ((hp8563e_status = viRead (instrSession, (unsigned char*) rdBuf, BUFFER_SIZE, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
    
    /* Scan response and determine if instrument is in linear mode. */
    Scan (rdBuf, "0");
    if (NumFmtdBytes () == 1) {
        if (hp8563e_invalidViReal64Range (peak, 0.1, 10.0))
            return VI_ERROR_PARAMETER3;
        Fmt (wrtBuf, "%s<MKPT %f DBM; MKPX %f;", mark, peak); 
    }
    else {
        if (hp8563e_invalidViReal64Range (peak, 0.0, 30.0))
            return VI_ERROR_PARAMETER3;
        Fmt (wrtBuf, "%s<MKPT %f DBM; MKPX %f DB;", mark, peak); 
    }
    
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config Sweep                                                    */
/* Purpose:  This function sets the Sweep Time, Sweep Mode, and the Delay    */
/*           Sweep.                                                          */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confSweep (ViSession instrSession, ViBoolean sweep, 
                    ViReal64 sweepVal, ViBoolean sweepMode, ViBoolean delaySweep, 
                    ViReal64 delayVal)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt, cnt = 0;

    if (hp8563e_invalidViBooleanRange (sweep))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViReal64Range (sweepVal, 50.0e-3, 100.0e6))
        return VI_ERROR_PARAMETER3;
    if (hp8563e_invalidViBooleanRange (sweepMode))
        return VI_ERROR_PARAMETER4;
    if (hp8563e_invalidViBooleanRange (delaySweep))
        return VI_ERROR_PARAMETER5;
    if (hp8563e_invalidViReal64Range (delayVal, 2.0e-3, 65535.0))
        return VI_ERROR_PARAMETER6;
    
    if (sweep == VI_FALSE)
        Fmt (wrtBuf, "%s<ST AUTO;%s;", sweepModeOptns[sweepMode]);
    else
        Fmt (wrtBuf, "%s<ST %f MS;%s;", sweepVal, sweepModeOptns[sweepMode]);
    cnt = NumFmtdBytes ();
    
    if (delaySweep == VI_FALSE)
        Fmt (wrtBuf, "%s[a]<DLYSWP OFF;"); 
    else
        Fmt (wrtBuf, "%s[a]<DLYSWP %f MS;", delayVal); 
    cnt += NumFmtdBytes ();
    
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf, cnt, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          
                                     
/*===========================================================================*/
/* Function: Config Video-Gating                                             */
/* Purpose:  This function sets the Gate, Gate Delay, Gate Length, Edge      */
/*           Polarity, and Gate Control.                                     */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confVideoGate (ViSession instrSession, ViBoolean gate,
            ViReal64 delay, ViReal64 length, ViBoolean edge, ViBoolean cntl)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViBooleanRange (gate))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViReal64Range (delay, 3.0e-3, 65.535))
        return VI_ERROR_PARAMETER3;
    if (hp8563e_invalidViReal64Range (length, 1.0e-3, 65.535))
        return VI_ERROR_PARAMETER4;
    if (hp8563e_invalidViBooleanRange (edge))
        return VI_ERROR_PARAMETER5;
    if (hp8563e_invalidViBooleanRange (cntl))
        return VI_ERROR_PARAMETER6;
    
    Fmt (wrtBuf, "%s<GD %f MS;GL %f MS;GP %s;GATECTL %s;GATE %s;", delay, length, 
         negPos[edge], cntlOptns[cntl], offOn[gate]); 
    
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config Trigger                                                  */
/* Purpose:  This function sets the Sweep Mode, Trigger Mode, Video Trigger  */
/*           Level, and Trigger Polarity.                                    */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_configTrig (ViSession instrSession, ViBoolean sweep, 
                    ViInt16 trig, ViReal64 vidLvl, ViBoolean trigPol)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViBooleanRange (sweep))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViInt16Range (trig, 0, 3))
        return VI_ERROR_PARAMETER3;
    if (hp8563e_invalidViReal64Range (vidLvl, -220.0, 30.0))
        return VI_ERROR_PARAMETER4;
    if (hp8563e_invalidViBooleanRange (trigPol))
        return VI_ERROR_PARAMETER5;
    
    Fmt (wrtBuf, "%s<%s;TM %s;VTL %f DBM;TRIGPOL %s;", sweepModeOptns[sweep], 
         trigOptns[trig], vidLvl, negPos[trigPol]); 
    
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config Coupling Ratio                                           */
/* Purpose:  This function sets the the coupling ratio between the resolution*/
/*           bandwidth and the frequency span or the coupling ratio between  */
/*           the video bandwidth and the resolution bandwidth.               */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_configCouplRat (ViSession instrSession, ViBoolean type,
                    ViReal64 ratio)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViBooleanRange (type))
        return VI_ERROR_PARAMETER2;
    if (type == VI_FALSE) {
        if (hp8563e_invalidViReal64Range (ratio, 0.002, 0.10))
            return VI_ERROR_PARAMETER3;
    }
    else 
        if (hp8563e_invalidViReal64Range (ratio, 0.003, 3.0))
            return VI_ERROR_PARAMETER3;
    
    Fmt (wrtBuf, "%s<%s %f;", typeOptns[type], ratio); 
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config Coupling                                                 */
/* Purpose:  This function sSets the Auto-Coupling parameters.               */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_configCoupl (ViSession instrSession, ViInt16 cntl,
                    ViBoolean autoMan, ViReal64 value)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViInt16Range (cntl, 0, 5))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViBooleanRange (autoMan))
        return VI_ERROR_PARAMETER3;
    switch (cntl) {
        case 0: 
            break;
        case 1: 
            if (hp8563e_invalidViReal64Range (value, 1.0, 2.0e6))
                return VI_ERROR_PARAMETER4;
            break;
        case 2: 
            if (hp8563e_invalidViReal64Range (value, 1.0, 3.0e6))
                return VI_ERROR_PARAMETER4;
            break;
        case 3: 
            if (hp8563e_invalidViReal64Range (value, 50.0e-3, 6.0e6))
                return VI_ERROR_PARAMETER4;
            break;
        case 4: 
            if (hp8563e_invalidViReal64Range (value, 0.0, 70.0))
                return VI_ERROR_PARAMETER4;
            break;
        case 5: 
            if (hp8563e_invalidViReal64Range (value, 25.0, 40E9))
                return VI_ERROR_PARAMETER4;
            break;
    }
    
    if (cntl == 0)
        Fmt (wrtBuf, "%s<AUTOCPL;");   
    else {
        if (autoMan == VI_FALSE)
            Fmt (wrtBuf, "%s<%s AUTO;", cntlOptns2[cntl]); 
        else
            Fmt (wrtBuf, "%s<%s %f %s;", cntlOptns2[cntl], value, unitOptns[cntl]); 
    }
    
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config Trace                                                    */
/* Purpose:  This function sets the Detection Mode, A-B ->A, and A-B+DL ->B. */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confTrace (ViSession instrSession, ViInt16 detect, 
                    ViBoolean ABA, ViBoolean ABDLA)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViInt16Range (detect, 0, 3))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViBooleanRange (ABA))
        return VI_ERROR_PARAMETER3;
    if (hp8563e_invalidViBooleanRange (ABDLA))
        return VI_ERROR_PARAMETER4;
    
    Fmt (wrtBuf, "%s<DET %s;AMB %s;AMBPL %s;", detOptns[detect], offOn[ABA], offOn[ABDLA]); 
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config Window                                                   */
/* Purpose:  This function creates a window trace array for the fast Fourier */
/*           transform (FFT) function.                                       */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confWin (ViSession instrSession, ViInt16 win)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViInt16Range (win, 0, 2))
        return VI_ERROR_PARAMETER2;
    
    Fmt (wrtBuf, "%s<TWNDOW TRA,%s;", winOptns[win]); 
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config Display                                                  */
/* Purpose:  This function sets the Display-Line, Threshold, Graticule, and  */
/*           Annotation parameters.                                          */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_configDisplay (ViSession instrSession, ViBoolean dispLn,
                    ViReal64 dispLvl, ViBoolean thrsh, ViReal64 thrshLvl, 
                    ViBoolean grat, ViBoolean annot)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt, cnt = 0;

    if (hp8563e_invalidViBooleanRange (dispLn))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViReal64Range (dispLvl, -220.0, 30.0))
        return VI_ERROR_PARAMETER3;
    if (hp8563e_invalidViBooleanRange (thrsh))
        return VI_ERROR_PARAMETER4;
    if (hp8563e_invalidViReal64Range (thrshLvl, -220.0, 30.0))
        return VI_ERROR_PARAMETER5;
    if (hp8563e_invalidViBooleanRange (grat))
        return VI_ERROR_PARAMETER6;
    if (hp8563e_invalidViBooleanRange (annot))
        return VI_ERROR_PARAMETER7;
        
    if (dispLn == VI_FALSE)
        Fmt (wrtBuf, "%s<DL OFF;");
    else
        Fmt (wrtBuf, "%s<DL %f DBM;", dispLvl);
    cnt = NumFmtdBytes ();
    
    if (thrsh == VI_FALSE)
        Fmt (wrtBuf, "%s[a]<TH OFF;GRAT %s;ANNOT %s;", offOn[grat], offOn[annot]); 
    else
        Fmt (wrtBuf, "%s[a]<TH %f DBM;GRAT %s;ANNOT %s;", thrshLvl, offOn[grat], offOn[annot]);
    cnt += NumFmtdBytes ();
    
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf, cnt, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Config Title                                                    */
/* Purpose:  This function gives the current display a title.                */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_confTitle (ViSession instrSession, ViChar _VI_FAR title[])
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    Fmt (wrtBuf, "%s<TITLE@%s@;", title); 
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
                             
    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Peak Preselector                                                */
/* Purpose:  This function Peaks the preselector in the HP 8561E, HP8563E,   */
/*           HP 8564E, HP 8565E or when using the spectrum analyzer with a   */
/*           preselected external mixer.                                     */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_peakPresel (ViSession instrSession)
{
    ViStatus hp8563e_status = VI_SUCCESS;

	ViInt32 retCnt;

    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) "PP;", 3, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Perform Calibration                                             */
/* Purpose:  This function performs calibration routines.                    */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_perfmCalibr (ViSession instrSession, ViInt16 calibr)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE], rdBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViInt16Range (calibr, 0, 2))
        return VI_ERROR_PARAMETER2;
    
    Fmt (wrtBuf, "%s<%s;DONE?;", calibrOptns[calibr]); 
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf, NumFmtdBytes (), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    if ((hp8563e_status = viRead (instrSession, (unsigned char*) rdBuf, BUFFER_SIZE, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
    if ((Scan (rdBuf, "%s>%s[d]")) != 1)
        return VI_ERROR_INSTR_INTERPRETING_RESPONSE; 

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Perform Sweep                                                   */
/* Purpose:  This function commands the spectrum analyzer to take one full   */
/*           sweep across the trace display.                                 */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_perfmSwp (ViSession instrSession)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar rdBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) "TS;DONE?;", 9, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
        
    if ((hp8563e_status = viRead (instrSession, (unsigned char*) rdBuf, BUFFER_SIZE, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
    if ((Scan (rdBuf, "%s>%s[d]")) != 1)
        return VI_ERROR_INSTR_INTERPRETING_RESPONSE; 

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Perform Fourier Transform                                       */
/* Purpose:  This function performs a discreate Fourier transform (FFT) on   */
/*           the source trace array and stores the logarithms of the         */
/*           magnitude of the results in the destination array.              */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_perfmFourierT (ViSession instrSession, ViBoolean dest, 
                    ViBoolean src, ViBoolean win)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViBooleanRange (dest))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViBooleanRange (src))
        return VI_ERROR_PARAMETER3;
    if (hp8563e_invalidViBooleanRange (win))
        return VI_ERROR_PARAMETER4;
    
    Fmt (wrtBuf, "%s<FFT %s,%s,%s;", traceBA[dest], traceBA[src], traceBA[win]); 
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Perform ACP                                                     */
/* Purpose:  This function performs either a ACP Measurement or Computation  */
/*           command. The measurement can be single or continuous.           */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_perfmACP (ViSession instrSession, ViInt16 command)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViInt16Range (command, 0, 2))
        return VI_ERROR_PARAMETER2;
    
    Fmt (wrtBuf, "%s<%s;", commandOptns[command]); 
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Perform Trace                                                   */
/* Purpose:  This function performs trace related routines including:        */
/*           A+B ->A, A Exchange B, B-DL ->B, Clear-Write Trace, Max Hold    */
/*           Trace, Min Hold Trace, Blank Trace, and View Trace.             */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_perfmTrace (ViSession instrSession, ViInt16 command,
                    ViBoolean trace)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViInt16Range (command, 0, 7))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViBooleanRange (trace))
        return VI_ERROR_PARAMETER3;
        
    switch (command) {  
        case 0:
        case 1:
        case 2: 
            Fmt (wrtBuf, "%s<%s;", commandOptns2[command]); 
            break;
        default:
            Fmt (wrtBuf, "%s<%s %s;", commandOptns2[command], traceBA[trace]); 
            break;
    }
    
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
        
    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Place Marker                                                    */
/* Purpose:  This function places an active marker at the selected location  */
/*           of the trace.                                                   */ 
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_placeMarker (ViSession instrSession, ViInt16 marker,
                                       ViReal64 at)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE], rdBuf[BUFFER_SIZE];
    ViInt32 retCnt;
    ViBoolean unit = VI_FALSE; //Assuming the unit to be in microseconds.

    if (hp8563e_invalidViInt16Range (marker, 0, 10))
        return VI_ERROR_PARAMETER2;
        
    switch (marker) {   
        case 0:
        case 2:
            if ((hp8563e_status = viWrite (instrSession, (unsigned char*) "SP?;", 4, (unsigned int*)&retCnt)) < 0)
                return hp8563e_status;
            if ((hp8563e_status = viRead (instrSession, (unsigned char*) rdBuf, BUFFER_SIZE, (unsigned int*)&retCnt)) < 0)
                return hp8563e_status;

            Scan (rdBuf, "0");
            if (NumFmtdBytes () != 1) {
                unit = VI_TRUE;     //Correct the previous assumption; change the unit to hertz.
                if (hp8563e_invalidViReal64Range (at, 0.0, 40E9))
                    return VI_ERROR_PARAMETER3;  
            }
            else {
                if (hp8563e_invalidViReal64Range (at, 0.0, 2.0e+6))
                    return VI_ERROR_PARAMETER3;  
            }
                
            Fmt (wrtBuf, "%s<%s %f %s;", markerOptns[marker], at, unitOptns2[unit]); 
            break;
        case 1:
            if (hp8563e_invalidViReal64Range (at, 0.0, 100.0e+6))
                return VI_ERROR_PARAMETER3;  
            Fmt (wrtBuf, "%s<%s %f;", markerOptns[marker], at); 
            break;
        default:
            Fmt (wrtBuf, "%s<%s;", markerOptns[marker]); 
            break;
    }
    
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: Perform -> (To) Commands                                        */
/* Purpose:  This function performs routines that assign the value of one    */
/*           parameter to another parameter including: Marker -> CF,         */
/*           Marker -> Ref Lvl, Marker Delta -> Span, Marker -> CF Step,     */
/*           Sig ID -> CF, Chan Edges -> Delta Marker, Marker Delta - > Chan */
/*           Power BW, and Marker Mean -> CF.                                */ 
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_perfmTo (ViSession instrSession, ViInt16 command)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViInt16Range (command, 0, 7))
        return VI_ERROR_PARAMETER2;
    
    Fmt (wrtBuf, "%s<%s;", commandOptns3[command]); 
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}   

/*===========================================================================*/
/* Function: Shift Center Frequency                                          */
/* Purpose:  This function shifts the spectrum analyzer center frequency     */
/*           higher or lower in frequency by one channel spacing.            */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_shiftCF (ViSession instrSession, ViBoolean upDown)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViBooleanRange (upDown))
        return VI_ERROR_PARAMETER2;
    
    Fmt (wrtBuf, "%s<CHANNEL %s;", downUpOptns[upDown]); 
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}                                          

/*===========================================================================*/
/* Function: General Query                                                   */
/* Purpose:  This function queries for some general parameters.              */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_genQueries (ViSession instrSession, ViInt16 type,
                    ViPInt32 output)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE], rdBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViInt16Range (type, 0, 2))
        return VI_ERROR_PARAMETER2;
    
    Fmt (wrtBuf, "%s<%s?;", queryOptns[type]); 
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf, NumFmtdBytes (), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    if ((hp8563e_status = viRead (instrSession, (unsigned char*) rdBuf, BUFFER_SIZE, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
    if ((Scan (rdBuf, "%s>%d[b4]", output)) != 1)
        return VI_ERROR_INSTR_INTERPRETING_RESPONSE; 
        
    return hp8563e_status;
}  

/*===========================================================================*/
/* Function: Query ACP                                                       */
/* Purpose:  This function queries for the specified ACP measurement         */
/*           including upper ratio, lower ratio, maximum ACP, and total      */
/*           power.                                                          */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_queryACP (ViSession instrSession, ViInt16 queryType,
                    ViPReal64 meas)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE], rdBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViInt16Range (queryType, 0, 3))
        return VI_ERROR_PARAMETER2;
    
    Fmt (wrtBuf, "%s<%s?;", ACPOptns[queryType]); 
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf, NumFmtdBytes (), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
        
    if ((hp8563e_status = viRead (instrSession, (unsigned char*) rdBuf, BUFFER_SIZE, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
    if ((Scan (rdBuf, "%s>%f", meas)) != 1)
        return VI_ERROR_INSTR_INTERPRETING_RESPONSE; 
        
    return hp8563e_status;
}            

/*===========================================================================*/
/* Function: Query ACP (Alternate Channels)                                  */
/* Purpose:  This function queries for an array of power data resulting from */
/*           an ACP measurement of an RF signal.                             */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_queryACPchannels (ViSession instrSession,  
                    ViReal64 _VI_FAR meas[], ViPInt16 num)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar rdBuf[BUFFER_SIZE];
    ViInt32 retCnt;
    ViInt16 index = -1; 
    *num = 0;          /* Initialize the number of points to zero. */

    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) "ACPRSLTS?;", 10, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
        
    if ((hp8563e_status = viRead (instrSession, (unsigned char*) rdBuf, BUFFER_SIZE, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    /* Search for how many semicolons are in the string. This can then tell how */
    /* many measurements are in the string (# of semicolons plus 1).            */
    while ((index = FindPattern (rdBuf, index + 1, -1, ",", 1, 0)) != -1)
        (*num)++;
        
    (*num)++;       
    if ((Scan (rdBuf, "%s>%*f[x]", *num, meas)) != 1)
        return VI_ERROR_INSTR_INTERPRETING_RESPONSE; 
        
    return hp8563e_status;
}            

/*===========================================================================*/
/* Function: Query Amplitude                                                 */
/* Purpose:  This function queries for the active marker's amplitude.        */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_queryAmpl (ViSession instrSession, ViInt16 type, 
                    ViPReal64 amplit)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE], rdBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViInt16Range (type, 0, 2))
        return VI_ERROR_PARAMETER2;
        
    Fmt (wrtBuf, "%s<%s?;", amplitOptns3[type]);              
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf, NumFmtdBytes (), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
        
    if ((hp8563e_status = viRead (instrSession, (unsigned char*) rdBuf, BUFFER_SIZE, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
    if ((Scan (rdBuf, "%s>%f", amplit)) != 1)
        return VI_ERROR_INSTR_INTERPRETING_RESPONSE; 
        
    return hp8563e_status;
}            

/*===========================================================================*/
/* Function: Query Bandwidth                                                 */
/* Purpose:  This function queries for various bandwidth.                    */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_queryBW (ViSession instrSession, ViInt16 type, 
                    ViInt16 pwrLvl, ViPReal64 bw)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE], rdBuf[BUFFER_SIZE];
    ViInt32 retCnt;
    
    if (hp8563e_invalidViInt16Range (type, 0, 3))
        return VI_ERROR_PARAMETER2;
    if (type == 0) { 
        if (pwrLvl > 0)
            return VI_ERROR_PARAMETER3;
        Fmt (wrtBuf, "%s<MKBW %d[b2],?;", pwrLvl);
    }
    else
        Fmt (wrtBuf, "%s<%s?;", bwOptns[type]); 
    
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf, NumFmtdBytes (), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
        
    if ((hp8563e_status = viRead (instrSession, (unsigned char*) rdBuf, BUFFER_SIZE, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
    if ((Scan (rdBuf, "%s>%f", bw)) != 1)
        return VI_ERROR_INSTR_INTERPRETING_RESPONSE; 
                                  
    return hp8563e_status;
}            

/*===========================================================================*/
/* Function: Query Frequency                                                 */
/* Purpose:  This function queries for various frequencies                   */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_queryFreq (ViSession instrSession, ViInt16 type,
                    ViPReal64 freq)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE], rdBuf[BUFFER_SIZE];
    ViInt32 retCnt;
    
    if (hp8563e_invalidViInt16Range (type, 0, 7))
        return VI_ERROR_PARAMETER2;
        
    Fmt (wrtBuf, "%s<%s?;", freqOptns2[type]); 
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf, NumFmtdBytes (), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
        
    if ((hp8563e_status = viRead (instrSession, (unsigned char*) rdBuf, BUFFER_SIZE, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
    if ((Scan (rdBuf, "%s>%f", freq)) != 1)
        return VI_ERROR_INSTR_INTERPRETING_RESPONSE; 
                                  
    return hp8563e_status;
}            

/*===========================================================================*/
/* Function: Query Sweep Time                                                */
/* Purpose:  This function queries for the current sweep time.               */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_querySwpTime (ViSession instrSession, ViPReal64 sweepTime)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar rdBuf[BUFFER_SIZE];
    ViInt32 retCnt;
    
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) "ST?;", 4, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
        
    if ((hp8563e_status = viRead (instrSession, (unsigned char*) rdBuf, BUFFER_SIZE, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
    if ((Scan (rdBuf, "%s>%f", sweepTime)) != 1)
        return VI_ERROR_INSTR_INTERPRETING_RESPONSE; 
                                  
    return hp8563e_status;
}            


/*===========================================================================*/
/* Function: Query Trace                                                     */
/* Purpose:  This function queries an array of the trace data.               */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_queryTrace (ViSession instrSession, ViBoolean trace,
                    ViReal64 _VI_FAR array[])
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE], rdBuf[REALLY_LARGE_BUFFER_SIZE];
    ViInt32 retCnt;
    
    if (hp8563e_invalidViBooleanRange (trace))
        return VI_ERROR_PARAMETER2;
        
    Fmt (wrtBuf, "%s<%s?;", traceBA[trace]); 
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf, NumFmtdBytes (), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
        
    if ((hp8563e_status = viRead (instrSession, (unsigned char*) rdBuf, REALLY_LARGE_BUFFER_SIZE, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
    if ((Scan (rdBuf, "%s>%601f[x]", array)) != 1)
        return VI_ERROR_INSTR_INTERPRETING_RESPONSE; 
                                  
    return hp8563e_status;
}            

/*===========================================================================*/
/* Function: Query Carrier On/Off Power                                      */
/* Purpose:  This function queries for the Carrier On or Off power           */
/*           measurements.                                                   */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_queryCarrierPwr (ViSession instrSession, ViBoolean carrier,
                    ViBoolean trace, ViPReal64 meas)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE], rdBuf[BUFFER_SIZE];
    ViInt32 retCnt;
    
    if (hp8563e_invalidViBooleanRange (carrier))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViBooleanRange (trace))
        return VI_ERROR_PARAMETER3;
        
    Fmt (wrtBuf, "%s<%s %s,?;", carrierOptns[carrier], traceBA[trace]); 
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf, NumFmtdBytes (), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
        
    if ((hp8563e_status = viRead (instrSession, (unsigned char*) rdBuf, BUFFER_SIZE, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
    if ((Scan (rdBuf, "%s>%f", meas)) != 1)
        return VI_ERROR_INSTR_INTERPRETING_RESPONSE; 
                                  
    return hp8563e_status;
}            
                                                              
/*===========================================================================*/
/* Function: Query Power (Other Pwr Related)                                 */
/* Purpose:  This function queries for the power related measurements        */
/*           including: Channel Power, Mean Power, Occupied Power Bandwidth  */
/*           within Delta Marker, and Power Bandwidth.                       */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_queryPwr (ViSession instrSession, ViInt16 type, 
                    ViBoolean trace, ViReal64 param, ViPReal64 meas)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE], rdBuf[BUFFER_SIZE];
    ViInt32 retCnt;
    
    if (hp8563e_invalidViInt16Range (type, 0, 3))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViBooleanRange (trace))
        return VI_ERROR_PARAMETER3;
    switch (type) { 
        case 0: 
            if (hp8563e_invalidViReal64Range (param, 0.0, 40E9))
                return VI_ERROR_PARAMETER4;
            break;
        case 1: 
            if (hp8563e_invalidViReal64Range (param, 0.01, 100.0))
                return VI_ERROR_PARAMETER4;
            break;  
        case 2: 
            if (hp8563e_invalidViReal64Range (param, 1.00, 99.99))
                return VI_ERROR_PARAMETER4;
            break;
        case 3: 
            if (hp8563e_invalidViReal64Range (param, 0.00, 100.0))
                return VI_ERROR_PARAMETER4;
            break;
    }
        
    Fmt (wrtBuf, "%s<%s %s,%f %s,?;", powerOptns[type], traceBA[trace], param, unitOptns3[type]); 
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf, NumFmtdBytes (), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
        
    if ((hp8563e_status = viRead (instrSession, (unsigned char*) rdBuf, BUFFER_SIZE, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
    if ((Scan (rdBuf, "%s>%f", meas)) != 1)
        return VI_ERROR_INSTR_INTERPRETING_RESPONSE; 
                                  
    return hp8563e_status;
}            

/*===========================================================================*/
/* Function: Save/Recall                                                     */
/* Purpose:  This function saves or recalls instrument states, traces, or    */
/*           amplitude-frequency correction point sets.                      */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_saveRecall (ViSession instrSession, ViBoolean saveRecall,
                    ViInt16 type, ViInt16 num)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViBooleanRange (saveRecall))
        return VI_ERROR_PARAMETER2;
    if (hp8563e_invalidViInt16Range (type, 0, 3))
        return VI_ERROR_PARAMETER3;
    
    switch (type) {
        case 0:
            if (hp8563e_invalidViInt16Range (num, 0, 10))
                return VI_ERROR_PARAMETER4;
            break;
        case 1:
        case 2:
            if (hp8563e_invalidViInt16Range (num, 0, 7))
                return VI_ERROR_PARAMETER4;
            break;
        case 3:
            if (hp8563e_invalidViInt16Range (num, 0, 4))
                return VI_ERROR_PARAMETER4;
            break;
    }
        
    Fmt (wrtBuf, "%s<%s %d[b2];", saveRecallOptns[saveRecall][type], num); 
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
        
    return hp8563e_status;
}  

/*===========================================================================*/
/* Function: Store/Recall Reference Trace                                    */
/* Purpose:  This function performs trace reference related commands to be   */
/*           used in averaging trace data.                                   */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_storeRecallRef (ViSession instrSession, ViInt16 command)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar wrtBuf[BUFFER_SIZE];
    ViInt32 retCnt;

    if (hp8563e_invalidViInt16Range (command, 0, 4))
        return VI_ERROR_PARAMETER2;
    
    Fmt (wrtBuf, "%s<%s;", refOptns[command]); 
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf,NumFmtdBytes(), (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}  

/*===========================================================================*/
/* Function: Wait Until Done                                                 */
/* Purpose:  This function waits until the instrument returns a message      */
/*           indicating that all previous commands have been executed.       */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_waitUntilDone (ViSession instrSession, ViInt32 timeOut)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar rdBuf[BUFFER_SIZE];
    ViInt32 retCnt;
    ViUInt16 STB;
    
    if (hp8563e_invalidViInt32Range (timeOut, 1000, 60000))
        return VI_ERROR_PARAMETER2;

    if ((hp8563e_status = viEnableEvent (instrSession, VI_EVENT_SERVICE_REQ, VI_QUEUE, VI_NULL)) < 0)
        return hp8563e_status;
    if ((hp8563e_status = viDiscardEvents (instrSession, VI_EVENT_SERVICE_REQ, VI_QUEUE)) < 0)
        return hp8563e_status;
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) "RQS 16;", 7, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) "DONE?;", 6, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
    if ((hp8563e_status = hp8563e_waitOnVisaEvent (instrSession, VI_EVENT_SERVICE_REQ, timeOut, &STB)) < 0)
        return hp8563e_status;
    if ((hp8563e_status = viRead (instrSession, (unsigned char*) rdBuf, BUFFER_SIZE, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;

    if ((Scan (rdBuf, "%s>%s[d]")) != 1)
        return VI_ERROR_INSTR_INTERPRETING_RESPONSE; 
        
    return hp8563e_status;
}            

/*===========================================================================*/
/* Function: Write To Instrument                                             */
/* Purpose:  This function writes a command string to the instrument.        */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_writeInstrData (ViSession instrSession, ViString writeBuffer)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    
    if ((hp8563e_status = viPrintf (instrSession, "%s", writeBuffer)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}

/*===========================================================================*/
/* Function: Read Instrument Buffer                                          */
/* Purpose:  This function reads the output buffer of the instrument.        */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_readInstrData (ViSession instrSession, ViInt32 numberBytesToRead,
                    ViChar _VI_FAR readBuffer[], ViPInt32 numBytesRead)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    *numBytesRead = 0L;
        
    if ((hp8563e_status = viRead (instrSession, (unsigned char*) readBuffer, numberBytesToRead, (unsigned int*)numBytesRead)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}

/*===========================================================================*/
/* Function: Reset                                                          */
/* Purpose:  This function presets the instrument.                           */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_reset (ViSession instrSession)
{
    ViUInt32 retCnt = 0;
    ViStatus hp8563e_status = VI_SUCCESS;
    ViChar rdBuf[BUFFER_SIZE];

    /*  Initialize the instrument to a known state.  */
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) "IP;DONE?;", 9, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
    if ((hp8563e_status = viRead (instrSession, (unsigned char*) rdBuf, BUFFER_SIZE, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
    if ((Scan (rdBuf, "%s>%s[d]")) != 1)
        return VI_ERROR_INSTR_INTERPRETING_RESPONSE; 

    if ((hp8563e_status = hp8563e_defaultInstrSetup (instrSession)) < 0)  
        return hp8563e_status;

    return hp8563e_status;
}

/*===========================================================================*/
/* Function: Self-Test                                                       */
/* Purpose:  This function executes the instrument self-test and returns     */
/*           the result. If the self test function is not supported by the   */
/*           instrument, this function returns the warning                   */
/*           VI_WARN_NSUP_SELF_TEST.                                         */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_selfTest (ViSession instrSession, ViPInt16 testResult,
                    ViChar _VI_FAR testMessage[])
{
    ViStatus hp8563e_status = VI_SUCCESS;

    hp8563e_status = VI_WARN_NSUP_SELF_TEST;
    *testResult = -1;
    Fmt (testMessage, "%s<Self Test Not Supported");
    
    return hp8563e_status;
}

/*===========================================================================*/
/* Function: Error Query                                                     */
/* Purpose:  This function queries the instrument error queue, and returns   */
/*           the result. If the error query function is not supported by the  */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_errorQuery (ViSession instrSession, ViPInt32 errorCode,
                    ViChar _VI_FAR errorMessage[])
{
    ViChar rdBuf[BUFFER_SIZE];
    ViUInt32 retCnt = 0;
    ViStatus hp8563e_status = VI_SUCCESS;

    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) "ERR?;", 5, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
    if ((hp8563e_status = viRead (instrSession, (unsigned char*) rdBuf, BUFFER_SIZE, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
    if ((Scan (rdBuf, "%s>%d[b4]", errorCode)) != 1)
        return VI_ERROR_INSTR_INTERPRETING_RESPONSE; 
    Fmt (errorMessage, "%s<See Manual for Error Code Description");
    
    return hp8563e_status;
}

/*===========================================================================*/
/* Function: Error Message                                                   */
/* Purpose:  This function translates the error return value from the        */
/*           instrument driver into a user-readable string.                  */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_errorMessage (ViSession instrSession, ViStatus statusCode,
                    ViChar _VI_FAR errMessage[])
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViInt16 i;
    
    static hp8563e_tStringValPair statusDescArray[] = {
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
                
        /*=====================================================================*/
        /* Insert instrument-specific error codes here.  Example:              */
        /*=====================================================================*/
        {hp8563e_ERROR_INVALID_CONFIGURATION, "ERROR: Instrument configuration error"},
        
        {VI_NULL, VI_NULL}
    };

    hp8563e_status = viStatusDesc (instrSession, statusCode, errMessage);
    if (hp8563e_status == VI_WARN_UNKNOWN_STATUS) {
        for (i = 0; statusDescArray[i].stringName; i++) {
            if (statusDescArray[i].stringVal == statusCode) {
                Fmt (errMessage, "%s<%s", statusDescArray[i].stringName);
                return (VI_SUCCESS);
            }
        }
        Fmt (errMessage, "%s<Unknown Error 0x%x[uw8p0]", statusCode);
        return (VI_WARN_UNKNOWN_STATUS);
    }
    
    hp8563e_status = VI_SUCCESS;
    return hp8563e_status;
}

/*===========================================================================*/
/* Function: Revision Query                                                  */
/* Purpose:  This function returns the driver and instrument revisions.      */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_revisionQuery (ViSession instrSession,
                    ViChar _VI_FAR driverRev[], ViChar _VI_FAR instrRev[])
{
    ViUInt32 retCnt = 0;
    ViStatus hp8563e_status = VI_SUCCESS;

    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) "REV?", 4, (unsigned int*)&retCnt)) < 0)
        return hp8563e_status;
    if ((hp8563e_status = viScanf (instrSession, "%[^\n]", instrRev)) < 0)
        return hp8563e_status;

    Fmt (driverRev, "%s<%s", hp8563e_REVISION);
    
    return hp8563e_status;
}

/*===========================================================================*/
/* Function: Close                                                           */
/* Purpose:  This function closes the instrument.                            */
/*===========================================================================*/
ViStatus _VI_FUNC hp8563e_close (ViSession instrSession)
{
    hp8563e_instrRange instrPtr;
    ViSession rmSession;
    ViStatus hp8563e_status = VI_SUCCESS;

    if ((hp8563e_status = viGetAttribute (instrSession, VI_ATTR_RM_SESSION, &rmSession)) < 0)
        return hp8563e_status;
    if ((hp8563e_status = viGetAttribute (instrSession, VI_ATTR_USER_DATA, &instrPtr)) < 0)
        return hp8563e_status;
            
    if (instrPtr != NULL) 
        free (instrPtr);
    
    hp8563e_status = viClose (instrSession);
    viClose (rmSession);

    return hp8563e_status;
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
ViBoolean hp8563e_invalidViBooleanRange (ViBoolean val)
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
ViBoolean hp8563e_invalidViInt16Range (ViInt16 val, ViInt16 min, ViInt16 max)
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
ViBoolean hp8563e_invalidViInt32Range (ViInt32 val, ViInt32 min, ViInt32 max)
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
ViBoolean hp8563e_invalidViReal64Range (ViReal64 val, ViReal64 min, ViReal64 max)
{
    return ((val < min || val > max) ? VI_TRUE : VI_FALSE);
}

/*===========================================================================*/
/* Function: Write and Wait for Output Message                               */
/* Purpose:  This function write the wrtBuf buffer to the instrument and     */
/*           then waits for an RQS indicating that an output message is      */
/*           available in the output buffer. This function has the same      */
/*           parameters as the viWrite function in the VISA library.         */
/*===========================================================================*/
ViStatus hp8563e_writeAndWait (ViSession instrSession, ViString wrtBuf, 
                               ViUInt32 Count, ViPUInt32 retCnt)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    ViUInt16 STB;

    if ((hp8563e_status = viEnableEvent (instrSession, VI_EVENT_SERVICE_REQ, VI_QUEUE, VI_NULL)) < 0)
        return hp8563e_status;
    if ((hp8563e_status = viDiscardEvents (instrSession, VI_EVENT_SERVICE_REQ, VI_QUEUE)) < 0)
        return hp8563e_status;
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) "RQS 16;", 7, retCnt)) < 0)
        return hp8563e_status;
    if ((hp8563e_status = viWrite (instrSession, (unsigned char*) wrtBuf, Count, retCnt)) < 0)
        return hp8563e_status;
    if ((hp8563e_status = hp8563e_waitOnVisaEvent (instrSession, VI_EVENT_SERVICE_REQ, 3000, &STB)) < 0)
        return hp8563e_status;

    return hp8563e_status;
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
ViStatus hp8563e_waitOnVisaEvent (ViSession instrSession, ViEventType eventTypeIn, 
            ViUInt32 timeoutIn, ViPUInt16 STB)
{            
    ViStatus hp8563e_status = VI_SUCCESS, tempStatus = VI_SUCCESS;
    ViEventType eventTypeOut = 0;
    ViEvent eventHandle = 0;
    
    /* For debug purposes we want to be able to see the status returned by        */
    /* viDisableEvent and viClose if one of the internal functions fails but do   */
    /* not want to return that value as that is not where the initial error       */
    /* occurs in the function, so we assign it to tempStatus.                     */
    
    if ((hp8563e_status = viWaitOnEvent (instrSession, eventTypeIn, timeoutIn, 
                                        &eventTypeOut, &eventHandle)) < 0) {
        tempStatus = viDisableEvent (instrSession, eventTypeIn, VI_QUEUE);
        return hp8563e_status;
    } 
    
    if (eventTypeIn == VI_EVENT_SERVICE_REQ)
        if ((hp8563e_status = viReadSTB (instrSession, STB)) < 0) {
            tempStatus = viClose (eventHandle);
            tempStatus = viDisableEvent (instrSession, eventTypeIn, VI_QUEUE);
            return hp8563e_status;
        }   
        
    if ((hp8563e_status = viClose (eventHandle)) < 0) {             
        tempStatus = viDisableEvent (instrSession, eventTypeIn, VI_QUEUE);
        return hp8563e_status;                 
    }
    
    if ((hp8563e_status = viDisableEvent (instrSession, eventTypeIn, VI_QUEUE)) < 0)
        return hp8563e_status;                                                   

    return hp8563e_status;                                    
}    

/*===========================================================================*/
/* Function: Initialize Clean Up                                             */
/* Purpose:  This function is used only by the hp8563e_init function.  When   */
/*           an error is detected this function is called to close the       */
/*           open resource manager and instrument object sessions and to     */
/*           set the instrSession that is returned from hp8563e_init to       */
/*           VI_NULL.                                                        */
/*===========================================================================*/
ViStatus hp8563e_initCleanUp (ViSession openRMSession,
                    ViPSession openInstrSession, ViStatus currentStatus)
{
    hp8563e_instrRange instrPtr;
    
    if (viGetAttribute (*openInstrSession, VI_ATTR_USER_DATA, &instrPtr) >= 0)
        if (instrPtr != NULL) 
            free (instrPtr);

    viClose (*openInstrSession);
    viClose (openRMSession);
    *openInstrSession = VI_NULL;
    
    return currentStatus;
}

/*===========================================================================*/
/* Function: Default Instrument Setup                                        */
/* Purpose:  This function sends a default setup to the instrument.  This    */
/*           function is called by the hp8563e_reset operation and by the     */
/*           hp8563e_init function if the reset option has not been           */
/*           selected.  This function is useful for configuring any          */
/*           instrument settings that are required by the rest of the        */
/*           instrument driver functions such as turning headers ON or OFF   */
/*           or using the long or short form for commands, queries, and data.*/                                    
/*===========================================================================*/
ViStatus hp8563e_defaultInstrSetup (ViSession instrSession)
{
    ViStatus hp8563e_status = VI_SUCCESS;
    hp8563e_instrRange instrPtr;
    
    /* Determine if the structure has been initialized for the current VISA  */
    /* Session and malloc if it has not.                                     */
	
	hp8563e_status = viGetAttribute (instrSession, VI_ATTR_USER_DATA, &instrPtr);
	
    if ( hp8563e_status )
        return hp8563e_status;
    
    if (instrPtr == NULL) 
        instrPtr = malloc (sizeof (struct hp8563e_statusDataRanges));

    /*=====================================================================*/
    /* Change to reflect the global status variables that your driver needs*/
    /* to keep track of.  Not currently used left for future modifications.*/
    /*=====================================================================*/
    instrPtr -> triggerMode = 0;
    instrPtr -> val2 = 0;
    instrPtr -> val3 = 0;
    Fmt (instrPtr -> instrDriverRevision, "%s<%s", hp8563e_REVISION);
    
    if ((hp8563e_status = viSetAttribute (instrSession, VI_ATTR_USER_DATA, 
                            (ViUInt32)instrPtr)) < 0)
        return hp8563e_status;

    return hp8563e_status;
}

/*****************************************************************************/
/*=== END INSTRUMENT DRIVER SOURCE CODE =====================================*/
/*****************************************************************************/
