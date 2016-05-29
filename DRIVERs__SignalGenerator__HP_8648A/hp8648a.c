/***************************************************************************/
/*  Copyright 1996 National Instruments Corporation.  All Rights Reserved. */
/***************************************************************************/

#include <formatio.h>
#include <utility.h>
#include <visa.h>
#include <ansi_c.h>
#include "hp8648a.h"

/*= DEFINES =================================================================*/
#define hp8648a_REVISION     "Rev 1.0, 1/96, CVI 3.1" /*  Instrument driver revision */
#define BUFFER_SIZE         512L         /* File I/O buffer size */
 
/*===========================================================================*/
/*= Hewlett Packard 8648A Signal Generator ==================================*/    
/* LabWindows/CVI 3.1 Instrument Driver                                      */
/* Original Release: January 1996                                            */
/* By: ELH, LWILDP, National Instruments, Austin, TX                         */
/*     PH. (800)433-3488   Fax (512)794-5678                                 */
/*  Originally written in C                                                  */
/*  Modification History:                                                    */
/* Modifications:        Added _VI_FAR  for MDLL Compatibility               */
/*                                                                           */
/* Modification Date:    June 26, 1996                                       */
/*                                                                           */
/* Modified by:          DS, National Instruments, Austin, Texas             */
/*                       Phone (800) 433-3488  Fax (512) 794-5678            */
/*                                                                           */                                                                          
/*****************************************************************************/ 
 
/*****************************************************************************/
/*= INSTRUMENT-DEPENDENT COMMAND ARRAYS =====================================*/
/*****************************************************************************/

static ViInt16 uppr[3] = {100.0, 100.0, 10.0};
static ViString modul[3] = {":AM:DEPT",":FM:DEV",":PM:DEV"};
static ViString mod_unt[3] = {"PCT","KHZ","RAD"};
static ViString source[3] = {"INT","EXT","INT,EXT"};
static ViString intrnl[2] = {"400 HZ","1 KHZ"};
static ViString func[3] = {":AM:",":FM:",":PM:"};
static ViString extrn_coup[2] = {"DC","AC"};

/*****************************************************************************/
/*= INSTRUMENT-DEPENDENT STATUS/RANGE STRUCTURE  ============================*/
/*****************************************************************************/
/* hp8648a_stringValPair is used in the hp8648a_errorMessage function        */
/* hp8648a_statusDataRanges is used to track session dependent status & ranges*/
/*===========================================================================*/
typedef struct  hp8648a_stringValPair
{
   ViStatus stringVal;
   ViString stringName;
}  hp8648a_tStringValPair;

/*****************************************************************************/
/*= UTILITY ROUTINE DECLARATIONS (Non-Exportable Functions) =================*/
/*****************************************************************************/
ViBoolean hp8648a_invalidViBooleanRange (ViBoolean val);
ViBoolean hp8648a_invalidViInt16Range (ViInt16 val, ViInt16 min, ViInt16 max);
ViBoolean hp8648a_invalidViReal64Range (ViReal64 val, ViReal64 min, ViReal64 max);
ViStatus hp8648a_initCleanUp (ViSession openRMSession, ViPSession openInstrSession, ViStatus currentStatus);

/*****************************************************************************/
/*====== USER-CALLABLE FUNCTIONS (Exportable Functions) =====================*/
/*****************************************************************************/

/*===========================================================================*/
/* Function: Initialize                                                      */
/* Purpose:  This function opens the instrument, queries the instrument      */
/*           for its ID, and initializes the instrument to a known state.    */
/*===========================================================================*/
ViStatus _VI_FUNC hp8648a_init (ViRsrc resourceName, ViBoolean IDQuery,
                        ViBoolean reset, ViPSession instrSession)
{
    ViStatus hp8648a_status = VI_SUCCESS;
    ViSession rmSession = 0;
    ViUInt32 retCnt = 0;
    ViByte rdBuffer[BUFFER_SIZE];    

    /*- Check input parameter ranges ----------------------------------------*/
    if (hp8648a_invalidViBooleanRange (IDQuery))
        return VI_ERROR_PARAMETER2;
    if (hp8648a_invalidViBooleanRange (reset))
        return VI_ERROR_PARAMETER3;

    /*- Open instrument session ---------------------------------------------*/
    if ((hp8648a_status = viOpenDefaultRM (&rmSession)) < 0)
        return hp8648a_status;

    if ((hp8648a_status = viOpen (rmSession, resourceName, VI_NULL, VI_NULL, instrSession)) < 0) {
        viClose (rmSession);
        return hp8648a_status;
    }

    /*- Configure VISA Formatted I/O ----------------------------------------*/
    if ((hp8648a_status = viSetAttribute (*instrSession, VI_ATTR_TMO_VALUE, 10000)) < 0)
            return hp8648a_initCleanUp (rmSession, instrSession, hp8648a_status);
    if ((hp8648a_status = viSetBuf (*instrSession, VI_READ_BUF|VI_WRITE_BUF, 4000)) < 0)
            return hp8648a_initCleanUp (rmSession, instrSession, hp8648a_status);
    if ((hp8648a_status = viSetAttribute (*instrSession, VI_ATTR_WR_BUF_OPER_MODE,
                            VI_FLUSH_ON_ACCESS)) < 0)
            return hp8648a_initCleanUp (rmSession, instrSession, hp8648a_status);
    if ((hp8648a_status = viSetAttribute (*instrSession, VI_ATTR_RD_BUF_OPER_MODE,
                            VI_FLUSH_ON_ACCESS)) < 0)
            return hp8648a_initCleanUp (rmSession, instrSession, hp8648a_status);

    /*- Identification Query ------------------------------------------------*/
    if (IDQuery) {
        if ((hp8648a_status = viWrite (*instrSession, "*IDN?", 5, &retCnt)) < 0)
            return hp8648a_initCleanUp (rmSession, instrSession, hp8648a_status);
        if ((hp8648a_status = viRead (*instrSession, rdBuffer, BUFFER_SIZE, &retCnt)) < 0)
            return hp8648a_status;

        if ( strstr( rdBuffer, "Hewlett-Packard, 8648") == NULL )
            return hp8648a_initCleanUp (rmSession, instrSession, VI_ERROR_FAIL_ID_QUERY);
    }

    /*- Reset instrument ----------------------------------------------------*/
    if (reset) {
        if ((hp8648a_status = hp8648a_reset (*instrSession)) < 0)
            return hp8648a_initCleanUp (rmSession, instrSession, hp8648a_status);
    }       
        
    return hp8648a_status;
}

/*=========================================================================*/
/* Function:    Output RF Signal                                           */
/* Purpose:     Configures the instrument for Modulation. It selects the   */
/*              modulation type (AM, FM or PM) and the modulating signal.  */
/*=========================================================================*/
ViStatus _VI_FUNC hp8648a_output_signal (ViSession instrSession, ViReal64 freq, 
                        ViReal64 ampl, ViInt16 mod, ViReal64 mod_vl, 
                        ViInt16 mod_sgnl, ViBoolean int_freq, ViBoolean ext_coup)
{
    ViStatus hp8648a_status = VI_SUCCESS;
    
    if (hp8648a_invalidViReal64Range (freq, 0.100, 50000.0))
        return VI_ERROR_PARAMETER2;
    if (hp8648a_invalidViReal64Range (ampl, -136.0, 30.0))
        return VI_ERROR_PARAMETER3;
    if (hp8648a_invalidViInt16Range (mod, 0, 2))
        return VI_ERROR_PARAMETER4;
    if (hp8648a_invalidViReal64Range (mod_vl, 0.0, uppr[mod]))
        return VI_ERROR_PARAMETER5;
    if (hp8648a_invalidViInt16Range (mod_sgnl, 0, 2))
        return VI_ERROR_PARAMETER6;
    if (hp8648a_invalidViBooleanRange (int_freq))
        return VI_ERROR_PARAMETER7;
    if (hp8648a_invalidViBooleanRange (ext_coup))
        return VI_ERROR_PARAMETER8;

    if ((hp8648a_status = hp8648a_rf_signal (instrSession, freq, VI_OFF, 0.0, ampl, VI_ON)) != 0)
        return hp8648a_status;        

    if ((hp8648a_status = hp8648a_modulation (instrSession, mod, mod_vl, mod_sgnl, int_freq, ext_coup)) != 0)
        return hp8648a_status;

    if ((hp8648a_status = hp8648a_gen_signl (instrSession, VI_OFF, VI_ON)) != 0)
        return hp8648a_status;
        
    return hp8648a_status;                
}

/*=========================================================================*/
/* Function:    RF Signal                                                  */
/* Purpose:     Configures the RF Signal. It sets the frequency and        */
/*              amplitude of the signal.                                   */
/*=========================================================================*/
ViStatus _VI_FUNC hp8648a_rf_signal (ViSession instrSession, ViReal64 freq,
                        ViBoolean ref, ViReal64 ref_vl, ViReal64 ampl, 
                        ViBoolean attn)
{
    ViUInt32 retCnt = 0L;
    ViStatus hp8648a_status = VI_SUCCESS;
    ViByte wrtBuf[BUFFER_SIZE];
    ViInt16 total = 0;

    if (hp8648a_invalidViReal64Range (freq, 0.100, 50000.0))
        return VI_ERROR_PARAMETER2;
    if (hp8648a_invalidViBooleanRange (ref))
        return VI_ERROR_PARAMETER3;
    if (hp8648a_invalidViReal64Range (ref_vl, -136.0, 30.0))
        return VI_ERROR_PARAMETER4;
    if (hp8648a_invalidViBooleanRange (attn))
        return VI_ERROR_PARAMETER6;
            
    if (attn == VI_ON)
        Fmt (wrtBuf, "%s<:POW:ATT:AUTO ON;:FREQ:CW %f MHZ;", freq);
    else
        Fmt (wrtBuf, "%s<:POW:ATT:AUTO OFF;:FREQ:CW %f MHZ;", freq);
    total = NumFmtdBytes();    
    
    if (ref == VI_ON) {
        if (hp8648a_invalidViReal64Range (ampl, -136.0 - ref_vl,13.0 - ref_vl))
            return VI_ERROR_PARAMETER5;
        Fmt (wrtBuf, "%s[a]<:POW:REF:STAT ON;");                
    }
    else {
        if (hp8648a_invalidViReal64Range (ampl, -136.0, 13.0))
            return VI_ERROR_PARAMETER5;
            Fmt (wrtBuf, "%s[a]<:POW:REF:STAT OFF;");
    }
    total += NumFmtdBytes();
    
    Fmt (wrtBuf, "%s[a]<:POW:REF %f DBM;:POW:AMPL %f DBM;", ref_vl, ampl);
    total += NumFmtdBytes();

    if ((hp8648a_status = viWrite (instrSession, wrtBuf, total, &retCnt)) < 0)
        return hp8648a_status;

    return hp8648a_status;
}

/*=========================================================================*/
/* Function:    Modulation                                                 */
/* Purpose:     Configures the instrument for Modulation. It selects the   */
/*              modulation type (AM, FM or PM) and the modulating signal.  */
/*=========================================================================*/
ViStatus _VI_FUNC hp8648a_modulation (ViSession instrSession, ViInt16 mod, 
                        ViReal64 mod_vl, ViInt16 mod_sgnl, ViBoolean int_freq, 
                        ViBoolean ext_coup)
{
    ViUInt32 retCnt = 0L;
    ViStatus hp8648a_status = VI_SUCCESS;
    ViByte wrtBuf[BUFFER_SIZE];
    ViInt16 total = 0;

    if (hp8648a_invalidViInt16Range (mod, 0, 2))
        return VI_ERROR_PARAMETER2;
    if (hp8648a_invalidViReal64Range (mod_vl, 0.0, uppr[mod]))
        return VI_ERROR_PARAMETER3;
    if (hp8648a_invalidViInt16Range (mod_sgnl, 0, 2))
        return VI_ERROR_PARAMETER4;
    if (hp8648a_invalidViBooleanRange (int_freq))
        return VI_ERROR_PARAMETER5;
    if (hp8648a_invalidViBooleanRange (ext_coup))
        return VI_ERROR_PARAMETER6;

    Fmt (wrtBuf, "%s<%s %f %s;%sSOUR %s;",modul[mod], mod_vl, mod_unt[mod], func[mod], source[mod_sgnl]);

    total = NumFmtdBytes();
    if (mod_sgnl != 1) {
        Fmt (wrtBuf, "%s[a]<%sINT:FREQ %s;", func[mod], intrnl[int_freq]);
        total += NumFmtdBytes();
    }
    if (mod_sgnl != 0) {
        if ((mod != 1) & (ext_coup == VI_OFF))
            return VI_ERROR_NO_DC;
        Fmt (wrtBuf, "%s[a]<%sEXT:COUP %s;", func[mod], extrn_coup[ext_coup]);
        total += NumFmtdBytes();
    }
    if (mod == 0) {
        Fmt (wrtBuf, "%s[a]<:FM:STAT OFF;:PM:STAT OFF;:AM:STAT ON;");
        total += NumFmtdBytes();
    }
    if (mod == 1) {
        Fmt (wrtBuf, "%s[a]<:AM:STAT OFF;:PM:STAT OFF;:FM:STAT ON;");
        total += NumFmtdBytes();
    }
    if (mod == 2) {
        Fmt (wrtBuf, "%s[a]<:FM:STAT OFF;:AM:STAT OFF;:PM:STAT ON;");
        total += NumFmtdBytes();
    }
    if ((hp8648a_status = viWrite (instrSession, wrtBuf, total, &retCnt)) < 0)
        return hp8648a_status;

    return hp8648a_status;
}

/*=========================================================================*/
/* Function:    Set Attenuator & Reference                                 */
/* Purpose:     Disables/Enables the Amplitude Reference and the Automatic */
/*              Attenuation modes.                                         */
/*=========================================================================*/
ViStatus _VI_FUNC hp8648a_set_ref_att (ViSession instrSession, ViBoolean ref, 
                        ViBoolean att)
{
    ViUInt32 retCnt = 0L;
    ViStatus hp8648a_status = VI_SUCCESS;
    ViByte wrtBuf[BUFFER_SIZE];
    ViInt16 total = 0;
    
    if (hp8648a_invalidViBooleanRange (att))
        return VI_ERROR_PARAMETER2;
    if (hp8648a_invalidViBooleanRange (ref))
        return VI_ERROR_PARAMETER3;
      
    if (att == VI_ON)
        Fmt (wrtBuf, "%s<:POW:ATT:AUTO ON;");
    else
        Fmt (wrtBuf, "%s<:POW:ATT:AUTO OFF;");
    
    total = NumFmtdBytes();
    if (ref == VI_ON)
        Fmt (wrtBuf, "%s[a]<:POW:REF:STAT ON;");
    else
        Fmt (wrtBuf, "%s[a]<:POW:REF:STAT OFF;");    
    
    total += NumFmtdBytes();
    if ((hp8648a_status = viWrite (instrSession, wrtBuf, total, &retCnt)) < 0)
        return hp8648a_status;
        
    return hp8648a_status;        
}

/*=========================================================================*/
/* Function:    Modulation On/Off                                          */
/* Purpose:     Turns the selected modulation On or Off.                   */
/*=========================================================================*/
ViStatus _VI_FUNC hp8648a_mod_on_off (ViSession instrSession, ViInt16 set_mod,
                        ViBoolean mod)
{
    ViUInt32 retCnt = 0L;
    ViStatus hp8648a_status = VI_SUCCESS;
    ViByte wrtBuf[BUFFER_SIZE];
    ViInt16 total = 0;
    
    if (hp8648a_invalidViInt16Range (set_mod, 0, 2))
        return VI_ERROR_PARAMETER2;
    if (hp8648a_invalidViBooleanRange (mod))
        return VI_ERROR_PARAMETER3;

    if (mod == VI_ON) {
        if (set_mod == 0)
            Fmt (wrtBuf, "%s<:FM:STAT OFF;:PM:STAT OFF;:AM:STAT ON;");
        if (set_mod == 1)
            Fmt (wrtBuf, "%s<:AM:STAT OFF;:PM:STAT OFF;:FM:STAT ON;");
        if (set_mod == 2)
            Fmt (wrtBuf, "%s<:FM:STAT OFF;:AM:STAT OFF;:PM:STAT ON;");
    }
    else {
        if (set_mod == 0)
            Fmt (wrtBuf, "%s<:FM:STAT OFF;:PM:STAT OFF;:AM:STAT ON;:AM:STAT OFF;");
        if (set_mod == 1)
            Fmt (wrtBuf, "%s<:AM:STAT OFF;:PM:STAT OFF;:FM:STAT ON;:FM:STAT OFF;");
        if (set_mod == 2)
            Fmt (wrtBuf, "%s<:FM:STAT OFF;:AM:STAT OFF;:PM:STAT ON;:PM:STAT OFF;");
    }
    total = NumFmtdBytes();
    if ((hp8648a_status = viWrite (instrSession, wrtBuf, total, &retCnt)) < 0)
        return hp8648a_status;
        
    return hp8648a_status;        
}

/*=========================================================================*/
/* Function:    Generate Signal                                            */
/* Purpose:     Sets the instrument to generate the configured RF signal   */
/*              with or without the configured modulation.                 */
/*=========================================================================*/
ViStatus _VI_FUNC hp8648a_gen_signl (ViSession instrSession, ViBoolean mod_all_off,
                        ViBoolean signl_out)
{
    ViUInt32 retCnt = 0L;
    ViStatus hp8648a_status = VI_SUCCESS;
    ViByte wrtBuf[BUFFER_SIZE];
    ViInt16 total = 0;

    if (hp8648a_invalidViBooleanRange (mod_all_off))
        return VI_ERROR_PARAMETER2;
    if (hp8648a_invalidViBooleanRange (signl_out))
        return VI_ERROR_PARAMETER3;

    if (mod_all_off == VI_ON) {
        Fmt (wrtBuf, "%s<:AM:STAT OFF;:PM:STAT OFF;:AM:STAT OFF;");
        total = NumFmtdBytes();
        if (signl_out == VI_ON)
            Fmt (wrtBuf, "%s[a]<:OUTP:STAT ON");
        else
            Fmt (wrtBuf, "%s[a]<:OUTP:STAT OFF");            
        total += NumFmtdBytes();
    }
    else {
        if (signl_out == VI_ON)
            Fmt (wrtBuf, "%s<:OUTP:STAT ON");
        else
            Fmt (wrtBuf, "%s<:OUTP:STAT OFF");            
        total = NumFmtdBytes();
    }
    if ((hp8648a_status = viWrite (instrSession, wrtBuf, total, &retCnt)) < 0)
        return hp8648a_status;
        
    return hp8648a_status;        
}

/*===========================================================================*/
/* Function: Write To Instrument                                             */
/* Purpose:  This function writes a command string to the instrument.        */
/*===========================================================================*/
ViStatus _VI_FUNC hp8648a_writeInstrData (ViSession instrSession, ViString writeBuffer)
{
    ViStatus hp8648a_status = VI_SUCCESS;
    
    if ((hp8648a_status = viPrintf (instrSession, "%s", writeBuffer)) < 0)
        return hp8648a_status;

    return hp8648a_status;
}

/*===========================================================================*/
/* Function: Read Instrument Buffer                                          */
/* Purpose:  This function reads the output buffer of the instrument.        */
/*===========================================================================*/
ViStatus _VI_FUNC hp8648a_readInstrData (ViSession instrSession, ViInt16 numBytes,
                        ViChar _VI_FAR rdBuf[], ViPInt32 bytesRead)
{
    ViStatus hp8648a_status = VI_SUCCESS;
    *bytesRead = 0L;
        
    if ((hp8648a_status = viRead (instrSession, rdBuf, numBytes, bytesRead)) < 0)
        return hp8648a_status;

    return hp8648a_status;
}

/*===========================================================================*/
/* Function: Reset                                                           */
/* Purpose:  This function resets the instrument.                            */
/*===========================================================================*/
ViStatus _VI_FUNC hp8648a_reset (ViSession instrSession)
{
    ViUInt32 retCnt = 0;
    ViStatus hp8648a_status = VI_SUCCESS;

    /*  Initialize the instrument to a known state.  */
    if ((hp8648a_status = viWrite (instrSession, "*RST", 4, &retCnt)) < 0)
        return hp8648a_status;

    return hp8648a_status;
}

/*===========================================================================*/
/* Function: Self-Test                                                       */
/* Purpose:  This function executes the instrument self-test and returns     */
/*           the result.                                                     */
/*===========================================================================*/
ViStatus _VI_FUNC hp8648a_selfTest (ViSession instrSession, ViPInt16 testResult,
                        ViChar _VI_FAR testMessage[])
{
    ViUInt32 retCnt = 0;
    ViStatus hp8648a_status = VI_SUCCESS;

    if ((hp8648a_status = viWrite (instrSession, "*TST?", 5, &retCnt)) < 0)
        return hp8648a_status;

    Delay (6.0);

    if ((hp8648a_status = viScanf (instrSession, "%hd", testResult)) < 0)
        return hp8648a_status;
    strcpy (testMessage, "See Manual for Self-Test Code Description");

    return hp8648a_status;
}

/*===========================================================================*/
/* Function: Error Query                                                     */
/* Purpose:  This function queries the instrument error queue, and returns   */
/*           the result.                                                     */
/*===========================================================================*/
ViStatus _VI_FUNC hp8648a_errorQuery (ViSession instrSession, ViPInt32 errCode,
                        ViChar _VI_FAR errMessage[])
{
    ViUInt32 retCnt = 0L;
    ViStatus hp8648a_status = VI_SUCCESS;

    if ((hp8648a_status = viWrite (instrSession, ":SYST:ERR?", 10, &retCnt)) < 0)
        return hp8648a_status;

    if ((hp8648a_status = viScanf (instrSession, "%ld,\"%[^\"]", errCode, errMessage)) < 0)
        return hp8648a_status;
      
    return hp8648a_status;
}

/*===========================================================================*/
/* Function: Error Message                                                   */
/* Purpose:  This function translates the error return value from the        */
/*           instrument driver into a user-readable string.                  */
/*===========================================================================*/
ViStatus _VI_FUNC hp8648a_errorMessage (ViSession instrSession, ViStatus errorCode,
                        ViChar _VI_FAR errMessage[])
{
    ViStatus hp8648a_status = VI_SUCCESS;
    ViInt16 i = 0;
    static hp8648a_tStringValPair statusDescArray[] = {
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
        {VI_ERROR_FILE_OPEN,    "ERROR: Opening the specified file"},
        {VI_ERROR_FILE_WRITE,   "ERROR: Writing to the specified file"},
        {VI_ERROR_INTERPRETING_RESPONSE, "ERROR: Interpreting the instrument's response"},
        {VI_ERROR_NO_DC,        "ERROR: No external DC coupling for AM or PM"},        
      
/*===========================================================================*/
/*        The define statements for instrument specific WARNINGS and ERRORS  */
/*        need to be inserted in the include file "hp8648a.h".  The codes     */
/*        must be in the range of 0xBFFC0900 to 0xBFFC0FFF and defined as    */
/*        follows:                                                           */
/*        #define INSTRUMENT_SPECIFIC_ERROR (VI_INSTR_ERROR_OFFSET+n)        */
/*        Where n represents each instrument specific error number.  Valid   */
/*        ranges for n are 0 to 0x6FF.                                       */
/*                                                                           */
/*        The INSTRUMENT_SPECIFIC_ERROR constants and string descriptor then */
/*        need to be added here.  Example:                                   */                
/*        {INSTRUMENT_SPECIFIC_ERROR, "ERROR: Instrument specific error"},   */
/*===========================================================================*/

        {VI_NULL, VI_NULL}
    };

    hp8648a_status = viStatusDesc (instrSession, errorCode, errMessage);
    if (hp8648a_status == VI_WARN_UNKNOWN_STATUS) {
        for (i=0; statusDescArray[i].stringName; i++) {
            if (statusDescArray[i].stringVal == errorCode) {
                strcpy (errMessage, statusDescArray[i].stringName);
                return (VI_SUCCESS);
            }
        }
        sprintf (errMessage, "Unknown Error 0x%08lX", errorCode);
        return (VI_WARN_UNKNOWN_STATUS);
    }
    
    hp8648a_status = VI_SUCCESS;
    return hp8648a_status;
}
 
/*===========================================================================*/
/* Function: Revision Query                                                  */
/* Purpose:  This function returns the driver and instrument revisions.      */
/*===========================================================================*/
ViStatus _VI_FUNC hp8648a_revisionQuery (ViSession instrSession,
                        ViChar _VI_FAR driverRev[], ViChar _VI_FAR instrRev[])
{
    ViUInt32 retCnt = 0L;
    ViStatus hp8648a_status = VI_SUCCESS;

    if ((hp8648a_status = viWrite (instrSession, "*IDN?", 5, &retCnt)) < 0)
        return hp8648a_status;

    if ((hp8648a_status = viScanf (instrSession, "%*[^,],%*[^,],%*[^,], %[^\n]", instrRev)) < 0)
        return hp8648a_status;

    strcpy (driverRev, hp8648a_REVISION);
    
    return hp8648a_status;
}
 
/*=========================================================================*/
/* Function:    Determine Modulation Level                                 */
/* Purpose:     Determines the Modulation level to be set for Depth(AM),   */
/*              Deviation(FM), or Deviation(PM) when the internal and      */
/*              external modulating sources are used with the selected     */
/*              modulation.                                                */
/*=========================================================================*/
ViStatus _VI_FUNC hp8648a_det_mod_set (ViInt16 mod, ViReal64 des_mod, 
                        ViReal64 ext_levl, ViPReal64 mod_set)
{
    ViStatus hp8648a_status = VI_SUCCESS;
    
    if (hp8648a_invalidViInt16Range (mod, 0, 2))
        return VI_ERROR_PARAMETER1;
    if (hp8648a_invalidViReal64Range (des_mod, 0.0, uppr[mod]))
        return VI_ERROR_PARAMETER2;
    if (hp8648a_invalidViReal64Range (ext_levl, 0.0, 0.5))
        return VI_ERROR_PARAMETER3;

    if (ext_levl < 0.5)
        *mod_set = des_mod/(1 + ext_levl);
    else
        *mod_set = (2*des_mod)/3;

    return hp8648a_status;        
}

/*===========================================================================*/
/* Function: Close                                                           */
/* Purpose:  This function closes the instrument.                            */
/*===========================================================================*/
ViStatus _VI_FUNC hp8648a_close (ViSession instrSession)
{
    ViSession rmSession;
    ViStatus hp8648a_status = VI_SUCCESS;

    if ((hp8648a_status = viGetAttribute (instrSession, VI_ATTR_RM_SESSION, &rmSession)) < 0)
        return hp8648a_status;
    
    hp8648a_status = viClose (instrSession);
    viClose (rmSession);

    return hp8648a_status;
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
ViBoolean hp8648a_invalidViBooleanRange (ViBoolean val)
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
ViBoolean hp8648a_invalidViInt16Range (ViInt16 val, ViInt16 min, ViInt16 max)
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
ViBoolean hp8648a_invalidViReal64Range  (ViReal64 val, ViReal64 min, ViReal64 max)
{
    return ((val < min || val > max) ? VI_TRUE : VI_FALSE);
}

/*===========================================================================*/
/* Function: Initialize Clean Up                                             */
/* Purpose:  This function is used only by the hp8648a_init function.  When  */
/*           an error is detected this function is called to close the       */
/*           open resource manager and instrument object sessions and to     */
/*           set the instrSession that is returned from hp8648a_init to      */
/*           VI_NULL.                                                        */
/*===========================================================================*/
ViStatus hp8648a_initCleanUp (ViSession openRMSession,
                    ViPSession openInstrSession, ViStatus currentStatus)
{
    viClose (*openInstrSession);
    viClose (openRMSession);
    *openInstrSession = VI_NULL;
    
    return currentStatus;
}

/*****************************************************************************/
/*=== END INSTRUMENT DRIVER SOURCE CODE =====================================*/
/*****************************************************************************/
