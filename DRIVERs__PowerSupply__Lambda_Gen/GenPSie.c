
/*= <Lambda Genesys Power Supply> ===========================================*/
/*                                                                           */
/* LabWindows/CVI 6.0 Instrument Driver           P/N:  87-507-004-01 Rev A  */
/* for IEEE-488                                                              */
/*                                                                           */
/* By: Walter Heyck                                                          */
/*     Lambda EMI                                                            */
/*     405 Essex Road                                                        */
/*     Neptune,  NJ  007753  USA                                             */
/*     PH. (732)922-9300       Fax (732)922-9334                             */
/*     walter.heyck@lambda.com                                               */
/*                                                                           */
/* Modification History:                                                     */
/*                                                                           */
/*     Rev 0.2  Aug 1, 2003                                                  */
/*           1. Inital Beta release                                          */
/*                                                                           */
/*     Rev A    Aug 14, 2003                                                 */
/*           1. Initial release                                              */
/*           2. Not supported: desktops set to Unversal (European) setting   */
/*               that use comma for decimal seperator                        */
/*           3. Not supported: GEN IEEE rev 14 (first released revision)     */
/*===========================================================================*/


#include <userint.h>
#include <utility.h>
#include <visa.h>
#include <ansi_c.h>
#include <formatio.h>
#include "GenPSie.h"


/****************************************************** 80 CHAR ***************/
/*= DEFINES =================================================================*/
/* The format for GenPSie_REVISION is a string with three fields separated by*/ 
/* commas, the first field is the revision of the instrument driver, the     */
/* second field is the month and year of the version's release, and the third*/
/* field is the version of CVI used to write the current version.  Please    */
/* modify here and in the function panel help for the GenPSie_revisionQuery  */
/* function panel.                                                           */
/*===========================================================================*/
#define GenPSie_REVISION    "Rev A, 08/14/2003, CVI 6.0" /* Instr driver rev  */

#define BUSY    0x01        // Busy bit in STB (spoll)
#define MAV     0x10        // Message Available bit in STB (spoll)
#define SYST    0x04        // SYSTEM:ERROR bit in STB (spoll)

 
/*****************************************************************************/
/*= INSTRUMENT-DEPENDENT COMMAND ARRAYS =====================================*/
/*****************************************************************************/
/* None used by Genesys drivers */
  
/*****************************************************************************/
/*= INSTRUMENT-DEPENDENT STATUS/RANGE STRUCTURE  ============================*/
/*****************************************************************************/

    /* GenPSie_stringValPair is used in the GenPSie_errorMessage function    */
typedef struct  GenPSie_stringValPair
{
   ViStatus stringVal;
   ViString stringName;
}  GenPSie_tStringValPair;

    /* Global status variables that Genesys driver needs to keep track of. */
    /* Not used by Genesys drivers                                         */
struct GenPSie_statusDataRanges {
    ViInt16 triggerMode;
    ViInt16 val2;
    ViInt16 val3;
    ViChar instrDriverRevision[256];
};

typedef struct GenPSie_statusDataRanges *GenPSie_instrRange;


/*****************************************************************************/
/*= UTILITY ROUTINE DECLARATIONS (Non-Exportable Functions) =================*/
/*****************************************************************************/

void     GenPSie_waitMS (ViUInt16 mSecWait);
ViStatus GenPSie_convertErrNumb (ViInt32 systErr);
//ViStatus GenPSie_scanSystErr (ViChar errMesg[], ViPInt32 errNumb, 
//                                                      ViChar errString[] );

ViBoolean GenPSie_invalidViBooleanRange (ViBoolean val);
ViBoolean GenPSie_invalidViInt16Range (ViInt16 val, ViInt16 min, ViInt16 max);
ViBoolean GenPSie_invalidViInt32Range (ViInt32 val, ViInt32 min, ViInt32 max);
ViBoolean GenPSie_invalidViUInt8Range (ViUInt8 val, ViUInt8 min, ViUInt8 max);
ViBoolean GenPSie_invalidViUInt16Range (ViUInt16 val, ViUInt16 min, ViUInt16 max);
ViBoolean GenPSie_invalidViUInt32Range (ViUInt32 val, ViUInt32 min, ViUInt32 max);
ViBoolean GenPSie_invalidViReal32Range (ViReal32 val, ViReal32 min, ViReal32 max);
ViBoolean GenPSie_invalidViReal64Range (ViReal64 val, ViReal64 min, ViReal64 max);
ViStatus GenPSie_waitOnVisaEvent (ViSession instrSession, ViEventType eventTypeIn, ViUInt32 tmoIn, ViPUInt16 STB);
ViStatus GenPSie_initCleanUp (ViSession openRMSession, 
                        ViPSession openInstrSession, ViStatus currentStatus);
ViStatus GenPSie_readToFile (ViSession instrSession, ViString filename, 
                        ViUInt32 readBytes, ViPUInt32 retCount);
ViStatus GenPSie_writeFromFile (ViSession instrSession, ViString filename,
                        ViUInt32 writeBytes, ViPUInt32 retCount);
ViStatus GenPSie_defaultInstrSetup (ViSession openInstrSession);

/*****************************************************************************/
/*------ INSERT INSTRUMENT-DEPENDENT UTILITY ROUTINE DECLARATIONS HERE ------*/
/*****************************************************************************/


/*****************************************************************************/
/*====== USER-CALLABLE FUNCTIONS (Exportable Functions) =====================*/
/*****************************************************************************/

/*===========================================================================*/
/* Function: Initialize                                                      */
/* Purpose:  This function opens the instrument,                             */
/*           sends "SYST:ERR:ENAB" for error feedback from drivers,          */
/*           optionally queries the instrument for its ID,                   */
/*           optioanlly resets the instrument to a safe state.               */
/*===========================================================================*/
ViStatus _VI_FUNC GenPSie_init (ViRsrc resourceName, ViBoolean IDQuery,
                    ViBoolean resetDevice, ViPSession instrSession)
{
    ViStatus  GenPSie_status = VI_SUCCESS;
    ViSession rmSession = 0;
    ViUInt32  retCnt = 0;
    ViByte    rdBuffer[BUFFER_SIZE];   /* Input buffer, max = *IDN? = 72 char */
    ViUInt16  StatByte;                /* Status byte read during serial poll */

    /*- Check input parameter ranges ----------------------------------------*/
    if (GenPSie_invalidViBooleanRange (IDQuery))
        return VI_ERROR_PARAMETER2;
    if (GenPSie_invalidViBooleanRange (resetDevice))
        return VI_ERROR_PARAMETER3;

    /*- Open instrument session ---------------------------------------------*/
    if ((GenPSie_status = viOpenDefaultRM (&rmSession)) < 0)
        return GenPSie_status;

    GenPSie_status = viOpen (rmSession, resourceName, 
                                            VI_NULL, VI_NULL, instrSession);  
    if (GenPSie_status < 0) {
        viClose (rmSession);
        return GenPSie_status;
    }

    /*- Configure VISA Formatted I/O ----------------------------------------*/
    /* Set response timeout to 3 seconds */
    if ((GenPSie_status = viSetAttribute (*instrSession, 
                                            VI_ATTR_TMO_VALUE, GEN_TMO)) < 0)
            return GenPSie_initCleanUp (rmSession, instrSession,
                                            GenPSie_status);

    /* Set size of formatted I/O read & write buffers to 128 bytes */
    if ((GenPSie_status = viSetBuf (*instrSession, 
                                    VI_READ_BUF|VI_WRITE_BUF, BUFFER_SIZE)) < 0)
            return GenPSie_initCleanUp (rmSession, instrSession,
                                            GenPSie_status);
  
    if ((GenPSie_status = viSetAttribute (*instrSession,
                            VI_ATTR_WR_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS)) < 0)
        return GenPSie_initCleanUp (rmSession, instrSession,
                                            GenPSie_status);
    
    if ((GenPSie_status = viSetAttribute (*instrSession,
                            VI_ATTR_RD_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS)) < 0)
        return GenPSie_initCleanUp (rmSession, instrSession,
                                           GenPSie_status);

     /*- See if Genesys is listening, send serial poll -----------------------*/
     GenPSie_status = viReadSTB (*instrSession, &StatByte);
     if (GenPSie_status < 0) {
            return GenPSie_initCleanUp (rmSession, instrSession,
                                           GenPSie_status);
     }

     /*- Enable System Error reporting, for driver status feedback -----------*/
     if ((GenPSie_status = viWrite (*instrSession, "SYST:ERR:ENAB",
                                                      13, &retCnt)) < 0)
            return GenPSie_initCleanUp (rmSession, instrSession,
                                           GenPSie_status);

    /*- Identification Query ------------------------------------------------*/
    if (IDQuery)
    {
        if ((GenPSie_status = viWrite (*instrSession, "*IDN?", 5, &retCnt)) < 0)
            return GenPSie_initCleanUp (rmSession, instrSession,GenPSie_status);
        if ((GenPSie_status = viRead (*instrSession, rdBuffer, 
                                                    BUFFER_SIZE, &retCnt)) < 0)
            return GenPSie_status;

        if (retCnt <= 30)             // Expect at least 30 char *IDN? response
           return GenPSie_initCleanUp (rmSession, instrSession, 
                                                        VI_ERROR_FAIL_ID_QUERY);
        if ((strncmp (rdBuffer, "LAMBDA,GEN", 10)) != 0)
           return GenPSie_initCleanUp (rmSession, instrSession, 
                                                        VI_ERROR_FAIL_ID_QUERY);
    }
        
    /*- Reset instrument ----------------------------------------------------*/
    if (resetDevice) 
    {
        if ((GenPSie_status = GenPSie_reset (*instrSession)) < 0)
            return GenPSie_initCleanUp (rmSession, instrSession,
                                           GenPSie_status);
    }       

    return GenPSie_status;
}

/*****************************************************************************/
/*-------- INSERT USER-CALLABLE INSTRUMENT-DEPENDENT ROUTINES HERE ----------*/
/*****************************************************************************/

           
           

/*===========================================================================*/
/* Function: Program the Output Voltage                                      */
/* Purpose:  This function inputs a floating-point value and sends it in a   */
/*           ":VOLT nn.nnnn" command to program the power supply's output.   */
/*           Voltage values outside of the instrument rating or              */
/*           outside of the OVP and UVL range will return with an error      */
/*===========================================================================*/
ViStatus _VI_FUNC GenPSie_programVoltage (ViSession instrSession,
                                          ViReal64  voltageSetting,
                                          ViInt16   settlingDelay)
{
    ViStatus GenPSie_status = VI_SUCCESS;
    ViChar   cmdString [20];
    
    Fmt (cmdString, "%s<%s%f", ":VOLT ", voltageSetting);

    GenPSie_status = GenPSie_sendCmd (instrSession, cmdString);
    
    Delay (settlingDelay / 1000);

    return GenPSie_status;
}


/*===========================================================================*/
/* Function: Program the Output Current                                      */
/* Purpose:  This function inputs a floating-point value and sends it in a   */
/*           ":CURR nn.nnnn" command to program the power supply's output.   */
/*           Current values outside of the instrument rating will return     */
/*           with an error                                                   */
/*===========================================================================*/
ViStatus _VI_FUNC GenPSie_programCurrent (ViSession instrSession,
                                          ViReal64  currentSetting,
                                          ViInt16   settlingDelay)
{
    ViStatus GenPSie_status = VI_SUCCESS;
    ViChar   cmdString [20];
    
    Fmt (cmdString, "%s<%s%f", ":CURR ", currentSetting);

    GenPSie_status = GenPSie_sendCmd (instrSession, cmdString);
    
    Delay (settlingDelay / 1000);

    return GenPSie_status;
    
}

/*===========================================================================*/
/* Function: Measure the Output Voltage                                      */
/* Purpose:  This function sends the "MEAS:VOLT?" query and                  */
/*           returns the response as a floating point value                  */
/*===========================================================================*/
ViStatus _VI_FUNC GenPSie_measureVoltage (ViSession instrSession,
                                          ViReal64 *measuredVoltage)
{
    ViStatus GenPSie_status = VI_SUCCESS;
    ViChar   returnStr [BUFFER_SIZE];   // Response message
    ViUInt32 returnCnt;                 // Number characters in response


    GenPSie_status = GenPSie_readQry (instrSession, 
                    "MEAS:VOLT?", returnStr, &returnCnt);
    
    if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
    
    returnStr[ returnCnt] = VI_NULL;        // add string terminator

    
    Fmt ( measuredVoltage, "%f<%s", returnStr );
    
    return GenPSie_status;

}

/*===========================================================================*/
/* Function: Measure the Output Current                                      */
/* Purpose:  This function sends the "MEAS:CURR?" query and                  */
/*           returns the response as a floating point value                  */
/*===========================================================================*/
ViStatus _VI_FUNC GenPSie_measureCurrent (ViSession instrSession,
                                          ViReal64 *measuredCurrent)
{
    ViStatus GenPSie_status = VI_SUCCESS;
    ViChar   returnStr [BUFFER_SIZE];   // Response message
    ViUInt32 returnCnt;

    GenPSie_status = GenPSie_readQry (instrSession, 
                    "MEAS:CURR?", returnStr, &returnCnt);
    if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
    
    returnStr[ returnCnt] = VI_NULL;        // add string terminator

    
    Fmt ( measuredCurrent, "%f<%s", returnStr );
    
    return GenPSie_status;

}

/*===========================================================================*/
/* Function: Write Any Message To Instrument                                 */
/* Purpose:  This function writes a command string to the instrument.        */
/*           No error or status returned from instrument                     */
/*===========================================================================*/
ViStatus _VI_FUNC GenPSie_writeInstrData (ViSession instrSession, 
                                                      ViString writeBuffer)
{
    ViStatus GenPSie_status = VI_SUCCESS;
    
    GenPSie_status = viPrintf (instrSession, "%s", writeBuffer);

    return GenPSie_status;
}

/*===========================================================================*/
/* Function: Read Data from Instrument                                       */
/* Purpose:  This function writes a query string to the instrument, then     */
/*           Reads and returns response message.                             */
/*           Clears any Messages or System Errors before sending query.      */
/*           Clears the Request Service (RQS) bit of the Status Byte         */
/*===========================================================================*/

ViStatus _VI_FUNC GenPSie_readQry (ViSession instrSession, 
                    ViString qryStr, ViString retStr, ViPUInt32 retCnt)

{
    ViStatus    GenPSie_status = VI_SUCCESS;
    ViUInt16    qryStrLeng;     // Length of query string to be sent
    ViUInt16    spollByte;      // Return from serial poll
    ViChar      retStrTmp[BUFFER_SIZE]; // Response mesge for cleanup cmd/qry
    ViUInt32    retCntTmp;      // Read/Write count for cleanup cmd/qry
    ViInt32     systErr;        // If System Error, is SCPI error number 

    // Initialze: no char in Response string, zero char read
    strcpy (retStr, "");
    *retCnt = 0;
    // Read serial poll, verify no pending SYST:ERR or messages
    GenPSie_status = GenPSie_STB (instrSession, &spollByte);
    if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
            
    // If Message Available (MAV) set high, read and discard message
    if ( spollByte & MAV) {
        GenPSie_status = viRead (instrSession, retStrTmp, BUFFER_SIZE, 
                                                                &retCntTmp);
        if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
    }
        
    // If SYST:ERR set high from prior error, reset error queue
    if ( spollByte & SYST) {
        GenPSie_status = viWrite (instrSession, "SYST:ERR:ENAB", 13, 
                                                                &retCntTmp);
        if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
    }
    
    // Send query to Genesys --------------  SEND QRY  -----------------------

    qryStrLeng = StringLength (qryStr);
    GenPSie_status = viWrite (instrSession, qryStr, qryStrLeng, retCnt);
    if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
    
    // Read serial poll, see if query accepted with no errors
    GenPSie_status = GenPSie_STB (instrSession, &spollByte);
    if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);

    if ( spollByte & MAV) {         // If MAV bit set, --- READ RESPONSE ----
    
        GenPSie_status = viRead(instrSession, retStr, BUFFER_SIZE, retCnt);
        retStr[ *retCnt] = VI_NULL;     // add string terminator
        // After reading message, must wait 2 mSec for power supply to
        // reset the IEEE interface chip before another serial poll is allowed
        GenPSie_waitMS (2);
        
        if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
    }
    else if ( spollByte & SYST) {   // If SYST:ERR bit set, read error
        GenPSie_status = viWrite (instrSession, "SYST:ERR?", 9, retCnt);
        if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
        GenPSie_status = GenPSie_STB (instrSession, &spollByte);
        if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
        if (spollByte & MAV) {
                                    // Read system error message
            GenPSie_status = viRead (instrSession, retStrTmp, BUFFER_SIZE, 
                                                                &retCntTmp);
        // After reading message, must wait 2 mSec for power supply to
        // reset the IEEE interface chip before another serial poll is allowed
            GenPSie_waitMS (2);
            
            if (GenPSie_status != VI_SUCCESS) 
                return (GenPSie_status);
            else if (retCntTmp < 11)
                return (GenPSie_ERROR_SYSTERR_SHORT_RESP);
            // Extract error number from system error response
            //    Example, extract "301" from response: +301,"PV above OVP"
            systErr = atoi (retStrTmp);
            if (systErr == 0)   // SYST:ERR bit set but no error reported
                return (GenPSie_ERROR_SYSTERR_NO_ERR);
            else
                // end of "If SYST:ERR bit set,,,,
                // Convert SCPI error number to VISA long number
                return (GenPSie_convertErrNumb (systErr));
        }
        else {              // MAV not set after SYST:ERR? sent
            return (GenPSie_ERROR_NO_MAV_AFTER_SYSTERR);
        }
    }                               
    else {                  // after query send, neither MAV or SYST set
        return (GenPSie_ERROR_QRY_NO_MAV_NO_ERR);
    }
    return (GenPSie_status); 
}


/*===========================================================================*/
/* Function: Send Command To Instrument, Verify Acceptance                   */
/* Purpose:  This function writes a command string to the instrument.        */
/*           Polls instrument and returns status:                            */
/*               error:   if the command can never be processed              */
/*               warning: if command cannot be processed at this time        */
/*           Clears any Message or System Errors before sending command.     */
/*===========================================================================*/
ViStatus _VI_FUNC GenPSie_sendCmd (ViSession instrSession, ViString cmdString)
{
    ViStatus GenPSie_status = VI_SUCCESS;
    ViUInt16 cmdStrLeng;
    ViUInt16 spollByte;      // Return from serial poll
    ViChar   respStr[BUFFER_SIZE];   // Read queue if leftover MAV message
    ViInt16  systErr = 0;    // If System Error, is error number 
    ViUInt32 numBytesRead;   // Read count if leftover MAV message
    ViUInt32 retCnt = 0;     // Number of bytes written to instrument

    // Read serial poll, verify no pending SYST:ERR or messages
    GenPSie_status = GenPSie_STB (instrSession, &spollByte);
    if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);

    // If Message Available (MAV) set high, read and discard message
    if ( spollByte & MAV) {
        GenPSie_status = GenPSie_readInstrData (
                      instrSession, BUFFER_SIZE, respStr,&numBytesRead);
        if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
    }
        
    // If SYST:ERR set high from prior error, empty and reset error queue
    if ( spollByte & SYST) {
        GenPSie_status = viWrite (instrSession, "SYST:ERR:ENAB", 13, &retCnt);
        if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
    }

    // Send command to Genesys --------------  SEND CMD  --------------
    cmdStrLeng = StringLength (cmdString);
    GenPSie_status = viWrite (instrSession, cmdString, cmdStrLeng, &retCnt);
    if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);

    // Read serial poll, see if command accepted with no errors
    GenPSie_status = GenPSie_STB (instrSession, &spollByte);
        if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);

    if (spollByte & SYST) {             // If SYST:ERR bit set, read error
        GenPSie_status = viWrite (instrSession, "SYST:ERR?", 9, &retCnt);
        if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);

        GenPSie_status = GenPSie_STB (instrSession, &spollByte);  // Read spoll
        if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
        else if (!(spollByte & MAV)) return (GenPSie_ERROR_NO_MAV_AFTER_SYSTERR);
            
        // Read system error message
        GenPSie_status = viRead (instrSession, respStr, BUFFER_SIZE, &retCnt);
        if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
        else if ( retCnt < 11) return (GenPSie_ERROR_SYSTERR_SHORT_RESP);
        // Extract error number from system error response
        //    Example, extract "301" from response: +301,"PV above OVP"
        systErr = atoi (respStr);
        if (systErr == 0)   // SYST:ERR bit set but no error reported
            return (GenPSie_ERROR_SYSTERR_NO_ERR);
        else
            // end of "If SYST:ERR bit set,,,,
            // Convert SCPI error number to VISA long number
            return (GenPSie_convertErrNumb (systErr));
    }                               // end of "If SYST:ERR bit set,,,,
    
    else if (spollByte & MAV)           // If Mesg Available bit set, 
    {
        return (GenPSie_ERROR_MAV_SET_AFTER_CMD);       // Query sent, not cmd
    }
    return (GenPSie_status);        // End GenPSie_sendCmd 
}



/*===========================================================================*/
/* Function: Read Instrument Buffer                                          */
/* Purpose:  This function reads the output buffer of the instrument.        */
/*           It assumes that message is available,                           */
/*              otherwise returns with timeout (TMO) error                   */
/*===========================================================================*/
ViStatus _VI_FUNC GenPSie_readInstrData 
                (ViSession instrSession, ViInt32 numberBytesToRead,
                    ViChar _VI_FAR readBuffer[], ViPInt32  numBytesRead)
{
    ViStatus GenPSie_status = VI_SUCCESS;
    *numBytesRead = 0L;
        
    GenPSie_status = viRead (instrSession, readBuffer, numberBytesToRead, 
                                                                numBytesRead);
    readBuffer[ *numBytesRead] = VI_NULL;   // add string terminator
                                                                
    return GenPSie_status;
}

/*===========================================================================*/
/* Function: Self-Test                                                       */
/* Purpose:  This function executes the instrument self-test and returns     */
/*           the result.                                                     */
/*===========================================================================*/
ViStatus _VI_FUNC GenPSie_selfTest (ViSession instrSession, ViPInt16 testResult,
                    ViChar _VI_FAR testMessage[])
{
    ViStatus GenPSie_status = VI_SUCCESS;
    ViChar   retStr [BUFFER_SIZE];              // Response mesg 
    ViUInt32 retCnt;                        // Number character in response

    /* Genesys *TST? query:                                     */
    /* Verifies communication from IEEE micro to Main micro.    */
    /* Returns:                                                 */
    /*     "0" in message queue and                             */
    /*     SYST:ERR? set to "0,"No error"" or error message     */

    GenPSie_status = GenPSie_readQry (instrSession, "*TST?", retStr, &retCnt);
    if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);

    if ( 0 ==  strcmp ("0", retStr)) {      // "0" = no error
        strcpy (testMessage, "Self-test passed");
        *testResult = 0;
    }
    else {
        strcpy (testMessage, "Self-test failed");
        *testResult = -1;
    }

    return (GenPSie_status);
}

/*===========================================================================*/
/* Function: Error Query                                                     */
/* Purpose:  This function queries the instrument system error queue         */
/*           and returns the result as a number and descriptive message      */
/*           Return:                                                         */
/*              scpiCode = error number +100 to -399 from SYST:ERR? response */
/*              visaCode = error number 0x3FFC0900L to 0xBFFC0900L           */
/*===========================================================================*/
ViStatus _VI_FUNC GenPSie_errorQuery (ViSession instrSession, 
                            ViPInt32 scpiCode, ViChar _VI_FAR errorMessage[],
                            ViPInt32 visaCode)
{
    ViStatus GenPSie_status = VI_SUCCESS;
    ViChar   retStr[BUFFER_SIZE];   // Response message from query
    ViUInt16 spollByte;             // Return from serial poll
    ViUInt32 retCnt;                // Qty chars read or write to IEEE

    // Initialze: no char in errorMessage string, zero char read
    strcpy (retStr, "");
    retCnt = 0;
            
    // Send read system errors query
    GenPSie_status = viWrite (instrSession, "SYST:ERR?", 9, &retCnt);
    if (GenPSie_status != VI_SUCCESS) return GenPSie_status;

    // Read serial poll
    GenPSie_status = GenPSie_STB (instrSession, &spollByte);
    if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);

    if ( !(spollByte & MAV))   
            /* If Message Available (MAV) bit not set,           */
            /*   there is no response and some error occurred    */
        return (GenPSie_ERROR_NO_MAV_AFTER_SYSTERR);    
    
    
    /* MAV bit set, --- READ SYSTEM:ERROR? RESPONSE ---- */
    
        /* Parse the error message and return Error Code and Error String. */
        /* See Genesys IEEE Manual section SYSTEM:ERROR MESSAGES           */
        /*    for list of possible errors                                  */
        /* Examples:  0,"No Error" or -410,"Query Interrupted".            */

        /* Extract error number from system error response               */
        /* Example, extract "301" from response: +301,"PV above OVP"     */

    GenPSie_status = viScanf (instrSession, "%ld,\"%[^\"]", scpiCode, 
                                                                errorMessage);
    if ( GenPSie_status < VI_SUCCESS)  return GenPSie_status;

        /* Convert SCPI error number to VISA long number     */
    *visaCode = GenPSie_convertErrNumb (*scpiCode);

    return (GenPSie_status); 
}

/*===========================================================================*/
/* Function: Error Message                                                   */
/* Purpose:  This function translates the error return value from the        */
/*           instrument driver into a user-readable string.                  */
/*===========================================================================*/
ViStatus _VI_FUNC GenPSie_errorMessage (ViSession instrSession, 
                            ViStatus statusCode,ViChar _VI_FAR errMessage[])
{
    ViStatus GenPSie_status = VI_SUCCESS;
    ViInt16 i;
    static GenPSie_tStringValPair statusDescArray[] = {
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
        {VI_ERROR_INSTR_INTERPRETING_RESPONSE, 
                        "ERROR: Interpreting the instrument's response"},
        {VI_ERROR_INSTR_PARAMETER9 ,  "ERROR: Parameter 9 out of range"},
        {VI_ERROR_INSTR_PARAMETER10,  "ERROR: Parameter 10 out of range"},
        {VI_ERROR_INSTR_PARAMETER11,  "ERROR: Parameter 11 out of range"},
        {VI_ERROR_INSTR_PARAMETER12,  "ERROR: Parameter 12 out of range"},
        {VI_ERROR_INSTR_PARAMETER13,  "ERROR: Parameter 13 out of range"},
        {VI_ERROR_INSTR_PARAMETER14,  "ERROR: Parameter 14 out of range"},
        {VI_ERROR_INSTR_PARAMETER15,  "ERROR: Parameter 15 out of range"},
        
/* Note                                                                  */
/* MAV = Message Available bit in Status Byte                            */
/* SYST:ERR? = query to read system error if SYS bit set in Status Byte  */
        {GenPSie_ERROR_IEEE_WRONG_REV,
                    "ERROR: Bad Genesys IEEE revision"},    // -008=BFFC0908
        {GenPSie_ERROR_PARSING_IDENTITY,
                    "ERROR: Cannot parse identity string"}, // -010=BFFC090A
        {GenPSie_ERROR_BUSY_STUCK_HIGH,
                    "ERROR: BUSY bit stuck high"},          // -032=BFFC0920
        {GenPSie_ERROR_NO_MAV_AFTER_SYSTERR,
                    "ERROR: No MAV after SYST:ERR? query"}, // -034=BFFC0922
        {GenPSie_ERROR_SYSTERR_SHORT_RESP,
                    "ERROR: SYST:ERR? returned < 11 char"}, // -036=BFFC0924
        {GenPSie_ERROR_SYSTERR_NO_ERR,
                    "ERROR: SYST:ERR? returned no error"},  // -038=BFFC0926
        {GenPSie_ERROR_MAV_SET_AFTER_CMD,
                    "ERROR: After command, MAV was set"},   // -040=BFFC0928
        {GenPSie_ERROR_QRY_NO_MAV_NO_ERR,
                    "ERROR: Query, no MAV no SYST:ERR set"},// -042=BFFC092A
        {GenPSie_ERROR_COMMAND_ERROR,
                    "ERROR: Command error"},                // -100=BFFC0964
        {GenPSie_ERROR_INVALID_CHARACTER,
                    "ERROR: Invalid character"},            // -101=BFFC0965
        {GenPSie_ERROR_SYNTAX_ERROR,
                    "ERROR: Syntax error"},                 // -102=BFFC0966
        {GenPSie_ERROR_DATA_TYPE_ERROR,
                    "ERROR: Data type error"},              // -104=BFFC0968
        {GenPSie_ERROR_MISSING_PARAMETER,
                    "ERROR: Missing parameter"},            // -109=BFFC096D
        {GenPSie_ERROR_WORD_TOO_LONG,
                    "ERROR: Program word too long"},        // -112=BFFC0970
        {GenPSie_ERROR_DATA_OUT_OF_RANGE,
                    "ERROR: Data out of range"},            // -222=BFFC09DE
        {GenPSie_WARN_EXECUTION_ERROR,
                    "WARNING: Execution error"},            // +300=3FFC0A2C
        {GenPSie_WARN_PV_ABOVE_OVP,
                    "WARNING: PV above OVP"},               // +301=3FFC0A2D
        {GenPSie_WARN_PV_BELOW_UVL,
                    "WARNING: PV below UVL"},               // +302=3FFC0A2E
        {GenPSie_WARN_OVP_ABOVE_RATING, 
                    "WARNING: OVP above rating"},           // +303=3FFC0A2F
        {GenPSie_WARN_OVP_BELOW_PV,
                    "WARNING: OVP below PV"},               // +304=3FFC0A30
        {GenPSie_WARN_UVL_BELOW_ZERO,
                    "WARNING: UVL below zero"},             // +305=3FFC0A31
        {GenPSie_WARN_UVL_ABOVE_PV,
                    "WARNING: UVL above PV"},               // +306=3FFC0A32
        {GenPSie_WARN_ON_DURING_FAULT,
                    "WARNING: No ON during fault"},         // +307=3FFC0A33
        {GenPSie_WARN_FAULT_SHUTDOWN,
                    "WARNING: Fault shutdown"},             // +320=3FFC0A40
        {GenPSie_WARN_AC_FAULT_SHUTDOWN,
                    "WARNING: AC fault shutdown"},          // +321=3FFC0A41
        {GenPSie_WARN_OVER_TEMP_SHUTDOWN,               
                    "WARNING: Over temperature shutdown"},  // +322=3FFC0A42
        {GenPSie_WARN_FOLD_BACK_SHUTDOWN,
                    "WARNING: Foldback shutdown"},          // +323=3FFC0A43
        {GenPSie_WARN_OVER_VOLTAGE_SHUTDOWN,
                    "WARNING: Over-Voltage shutdown"},      // +324=3FFC0A44
        {GenPSie_WARN_SHUT_OFF_SHUTDOWN,
                    "WARNING: Shut-Off signal shutdown"},   // +325=3FFC0A45
        {GenPSie_WARN_OUTPUT_OFF_SHUTDOWN,
                    "WARNING: Output-Off button shutdown"}, // +326=3FFC0A46
        {GenPSie_WARN_ENABLE_OPENED_SHUTDOWN,
                    "WARNING: Enable opened shutdown"},     // +327=3FFC0A47
        {GenPSie_WARN_INTERNAL_MESSAGE_FAULT,
                    "WARNING: Internal message fault"},     // +340=3FFC0A54
        {GenPSie_WARN_INPUT_OVERFLOW,
                    "WARNING: Input overflow"},             // +341=3FFC0A55
        {GenPSie_WARN_INTERNAL_OVERFLOW,
                    "WARNING: Internal overflow"},          // +342=3FFC0A56
        {GenPSie_WARN_INTERNAL_TIMEOUT,
                    "WARNING: Internal timeout"},           // +343=3FFC0A57
        {GenPSie_WARN_INTERNAL_COMM_ERROR,
                    "WARNING: Internal comm error"},        // +344=3FFC0A58
        {GenPSie_WARN_INTERNAL_CHECKSUM_ERROR,
                    "WARNING: Internal checksum error"},    // +345=3FFC0A59
        {GenPSie_WARN_QUEUE_OVERFLOW,
                    "WARNING: Internal queue overflow"},    // +350=3FFC0A5E
        {GenPSie_WARN_INTERNAL_COMM_ERROR,
                    "WARNING: Internal comm error"},        // +399=3FFC0A8F

        {GenPSie_ERROR_INVALID_CONFIGURATION, 
                    "ERROR: Instrument configuration error"}, //3840=BFFC1800
        
        {VI_NULL, VI_NULL}
    };

    GenPSie_status = viStatusDesc (instrSession, statusCode, errMessage);
    if (GenPSie_status == VI_WARN_UNKNOWN_STATUS) {
        for (i = 0; statusDescArray[i].stringName; i++) {
            if (statusDescArray[i].stringVal == statusCode) {
                Fmt (errMessage, "%s<%s", statusDescArray[i].stringName);
                return (VI_SUCCESS);
            }
        }
        Fmt (errMessage, "%s<Unknown Error 0x%x[uw8p0]", statusCode);
        return (VI_WARN_UNKNOWN_STATUS);
    }
    
    GenPSie_status = VI_SUCCESS;
    return GenPSie_status;
}

/*===========================================================================*/
/* Function: Revision Query                                                  */
/* Purpose:  This function returns the driver and instrument revisions.      */
/*===========================================================================*/
ViStatus _VI_FUNC GenPSie_revisionQuery (ViSession instrSession,
                    ViChar _VI_FAR driverRev[], ViChar _VI_FAR instrRev[])
{
    ViStatus GenPSie_status = VI_SUCCESS;
    ViUInt32 retCnt = 0;

    // Send ID Query query and read response
    GenPSie_status = viWrite (instrSession, "*IDN?", 5, &retCnt);
    if (GenPSie_status  < 0) return GenPSie_status;

    // Read response and scan to extract revision string
    GenPSie_status = viScanf (instrSession, "%*[^,],%*[^,],%*[^,],%[^\n]", 
                                                                    instrRev);
    if (GenPSie_status < 0) return GenPSie_status;

    // Return string version of this driver file
    Fmt (driverRev, "%s<%s", GenPSie_REVISION);
    
    return GenPSie_status;
}

/*===========================================================================*/
/* Function: Reads Identity string and parse its data                        */
/* Purpose:  To return the following information about the power supply:     */
/*           Maximum Voltage, Maximum Current, Serial Number,                */
/*           Main Firmware Revision, IEEE Firmware Revision                  */
/*                                                                           */
/* Example identity string from power supply:                                */
/*          "LAMBDA,GEN12.5-60,S/N:98K-3741,REV:GEN1U:1.6-B"                 */
/*===========================================================================*/

ViStatus _VI_FUNC GenPSie_identityQuery (ViSession instrSession,
                                         ViReal64 *maximumVoltage,
                                         ViReal64 *maximumCurrent,
                                         ViChar serialNumber[],
                                         ViChar mainRevision[],
                                         ViChar IEEERevision[])

{
    
#define COMMA 44  // ASCII comma
#define COLON 58  // ASCII colon
#define DASH  45  // ASCII dash
#define NULL  0
    
    ViStatus GenPSie_status = VI_SUCCESS;
    ViChar   returnStr [BUFFER_SIZE];   // Response message
    ViUInt32 returnCnt = 0;     // Number of characters read from instrument
    ViInt32  scanFields = 0;    // Number of fields, comma seperators
    ViReal64 rateVolt;
    ViReal64 rateCurr;
    ViInt32  i;                 // Temporary pointer into source string
    ViInt32  j;                 // Temporary pointer into destination string
    
    ViChar   lambdaFeild   [20];    // Hold "LAMBDA"
    ViChar   voltCurrField [20];    // Hold "GEN12.5-60"
    ViChar   serNumbField  [20];    // Hold "S/N:98K-3741"
    ViChar   twoRevs       [20];    // Hold "REV:GEN1U:1.6-B"
    ViChar   mainRev       [20];    // Hold "1.6"
    ViChar   ieeeRev       [20];    // Hold "B"
    
    char *skipModel;
    char *skipSN;
    char *skipDash;

    GenPSie_status = GenPSie_readQry (instrSession, 
                    "*IDN?", returnStr, &returnCnt);
    if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);

                    // Make sure at least 10 char in *IDN string
    if (returnCnt < 10) return (GenPSie_ERROR_PARSING_IDENTITY);
    
                    // Verify three commas in *IDN, keep count in scanFields
    for (i = 0; i < returnCnt; i++) {
        if (returnStr[ i] == COMMA) ++scanFields;
    }
    if (scanFields != 3) {
        if ( maximumVoltage ) *maximumVoltage = 0;
        if ( maximumCurrent ) *maximumCurrent = 0;
        return (GenPSie_ERROR_PARSING_IDENTITY);
    }

    // Parse four fields between commas
    // Parse field one before first comma "LAMBDA"

    i = 0; j = 0;
    while ( returnStr[i] != COMMA ) 
        lambdaFeild [j++] = returnStr[i++];  // lambdaField not used

    lambdaFeild [j] = 0;    // Add null terminator
    if ( strlen (lambdaFeild) < 2 ) return (GenPSie_ERROR_PARSING_IDENTITY);

    // Parse field two, after first comma "GEN12.5-60"
    i++; j = 0;
    
    while ( returnStr[i] != COMMA ) 
        voltCurrField [j++] = returnStr[i++];
    voltCurrField [j] = 0;  // Add null terminator
                // Find first number past model string (GEN), skipModel="6-200"         
    skipModel = strpbrk (voltCurrField, "0123456789");
                // Extract two floating point numbers with dash inbetween
    scanFields = sscanf (skipModel, "%lf-%lf", 
                                    &rateVolt, &rateCurr);
    if (scanFields != 2) return (GenPSie_ERROR_PARSING_IDENTITY);
	
    if ( maximumVoltage ) *maximumVoltage = rateVolt;
    if ( maximumCurrent ) *maximumCurrent = rateCurr;

    // Parse field three, after second comma "S/N:98K-3741"
    i++; j = 0;
    while ( returnStr[i] != COMMA ) 
        serNumbField [j++] = returnStr[i++];
    serNumbField [j] = 0;   // Add null terminator
                // Verify field has clolon and skip over it
    skipSN = strpbrk (serNumbField, ":");
    if ( skipSN == NULL) return (GenPSie_ERROR_PARSING_IDENTITY);
                // Increment skipSN to go past colon    
    if ( serialNumber ) strcpy (serialNumber, ++skipSN);
    
    // Parse field four, after third comma "REV:GEN1U:1.6-B"
                // Skip i past two colons
    i++; j = 0;
    while ( returnStr[++i] != COLON && returnStr[i] != NULL ) ;
    i++;
    while ( returnStr[++i] != COLON && returnStr[i] != NULL ) ;
    i++;
    while ( returnStr[i] != NULL )
        twoRevs[j++] = returnStr[i++];
    twoRevs [j] = 0;    // Add null terminator
                // Make sure there are enough chars to hold revions
    if ( strlen (twoRevs) < 3) return (GenPSie_ERROR_PARSING_IDENTITY);

                // twoRevs now has "1.6-B", get dash and chars after
    skipDash = strstr (twoRevs, "-");
                // Make sure there is a dash in revision
    if ( skipDash == NULL) return (GenPSie_ERROR_PARSING_IDENTITY);

                // Copy chars before dash to mainRev
    i = -1; 
    while ( twoRevs[ ++i] != DASH)
        mainRev[ i] = twoRevs[ i];
    mainRev[ i] = 0;    // Add null terminator

                // Copy chars after dash to ieeeRev
    i = 1; j = 0;
    while ( skipDash[ i] != NULL)
        ieeeRev[ j++] = skipDash[ i++];
    ieeeRev[ j] = 0;    // Add null terminator

    if ( mainRevision ) strcpy (mainRevision, mainRev);
	if ( IEEERevision ) strcpy (IEEERevision, ieeeRev);

    return GenPSie_status;
}


/*===========================================================================*/
/* Function: Close                                                           */
/* Purpose:  This function closes the instrument.                            */
/*===========================================================================*/
ViStatus _VI_FUNC GenPSie_close (ViSession instrSession)
{
    GenPSie_instrRange instrPtr;
    ViSession rmSession;
    ViStatus GenPSie_status = VI_SUCCESS;

    GenPSie_status = viGetAttribute (instrSession, VI_ATTR_RM_SESSION, 
                                                                &rmSession);
    if (GenPSie_status < 0) return GenPSie_status;
    
    GenPSie_status = viGetAttribute (instrSession, VI_ATTR_USER_DATA, 
                                                                &instrPtr);
    if (GenPSie_status < 0) return GenPSie_status;
            
    if (instrPtr != NULL) 
        free (instrPtr);
    
    GenPSie_status = viClose (instrSession);
    viClose (rmSession);

    return GenPSie_status;
}

/*===========================================================================*/
/* Function: Read the Output Voltage Setting                                 */
/* Purpose:  This function sends the ":VOLT?" query and                      */
/*           returns the response as a floating point value                  */
/*===========================================================================*/

ViStatus _VI_FUNC GenPSie_readVoltageSetting (ViSession instrSession,
                                              ViReal64 *voltageSetting)
{
    ViStatus GenPSie_status = VI_SUCCESS;
    ViChar   returnStr [BUFFER_SIZE];   // Response message
    ViUInt32 returnCnt;

    GenPSie_status = GenPSie_readQry (instrSession, 
                    ":VOLT?", returnStr, &returnCnt);
    if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
    
    returnStr[returnCnt] = VI_NULL;     // add string terminator

    
    Fmt (voltageSetting, "%f<%s", returnStr);
    
    return GenPSie_status;

}

/*===========================================================================*/
/* Function: Read the Output Current Setting                                 */
/* Purpose:  This function sends the ":CURR?" query and                      */
/*           returns the response as a floating point value                  */
/*===========================================================================*/

ViStatus _VI_FUNC GenPSie_readCurrentSetting (ViSession instrSession,
                                              ViReal64 *currentSetting)
{
    ViStatus GenPSie_status = VI_SUCCESS;
    ViChar   returnStr [BUFFER_SIZE];   // Response message
    ViUInt32 returnCnt;

    GenPSie_status = GenPSie_readQry (instrSession, 
                    ":CURR?", returnStr, &returnCnt);
    if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
    
    returnStr[returnCnt] = VI_NULL;     // add string terminator

    
    Fmt (currentSetting, "%f<%s", returnStr);
    
    return GenPSie_status;

}

/*===========================================================================*/
/* Function: Set the output On or Off                                        */
/* Purpose:  This function sends the "OUTPUT:STATUS" command with            */
/*           the argument "ON" or "OFF"                                      */
/*===========================================================================*/


ViStatus _VI_FUNC GenPSie_setOutputOn (ViSession instrSession,
                                              ViBoolean setOutputOn)
{
    ViStatus GenPSie_status = VI_SUCCESS;
    ViChar   cmdOn [20] = "OUTP:STAT ON";
    ViChar   cmdOff[20] = "OUTP:STAT OFF";
    
    if (setOutputOn) {
        GenPSie_status = GenPSie_sendCmd (instrSession, cmdOn);
        }
    else {
        GenPSie_status = GenPSie_sendCmd (instrSession, cmdOff);
        }
    
    return GenPSie_status;
    
}

/*===========================================================================*/
/* Function: Query if the output is On or Off                                */
/* Purpose:  This function sends the "OUTPUT:STATUS?" query and returns      */
/*                      True  = Output Enabled (ON)                          */
/*                      False = Output Disabled (OFF)                        */
/*===========================================================================*/


ViStatus _VI_FUNC GenPSie_readOutputOn (ViSession instrSession,
                                              ViBoolean *outputEnabled)

{
    ViStatus   GenPSie_status = VI_SUCCESS;
    ViChar     returnStr [BUFFER_SIZE]; // Response message
    ViUInt32   returnCnt;
    

    GenPSie_status = GenPSie_readQry (instrSession, 
                    "OUTP:STAT?", returnStr, &returnCnt);
    if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
    
    if (0 == strncmp (returnStr, "ON", 2))
        *outputEnabled = VI_TRUE;
    else
        *outputEnabled = VI_FALSE;
    
    return GenPSie_status;
}

/*===========================================================================*/
/* Function: Set Auto-Start On or Off (Safe-Start)                           */
/* Purpose:  This function sends the "OUTPUT:PON" command with               */
/*           the argument "ON" or "OFF"                                      */
/*===========================================================================*/

ViStatus _VI_FUNC GenPSie_setAutoStart (ViSession instrSession,
                                              ViBoolean setAutoStartOn)
{
    ViStatus GenPSie_status = VI_SUCCESS;
    ViChar   cmdOn [20] = "OUTPUT:PON ON";
    ViChar   cmdOff[20] = "OUTPUT:PON OFF";
    
    if (setAutoStartOn) {
        GenPSie_status = GenPSie_sendCmd (instrSession, cmdOn);
        }
    else {
        GenPSie_status = GenPSie_sendCmd (instrSession, cmdOff);
        }
    
    return GenPSie_status;
}

/*===========================================================================*/
/* Function: Query if the Auto-Start is On or Off                            */
/* Purpose:  This function sends the "OUTPUT:PON?" query and returns         */
/*                      True  = Auto-Start On                                */
/*                      False = Auto-Start Off (Safe-Start)                  */
/*===========================================================================*/

ViStatus _VI_FUNC GenPSie_readAutoStart (ViSession instrSession,
                                              ViBoolean *autoStartOn)

{
    ViStatus   GenPSie_status = VI_SUCCESS;
    ViChar     returnStr [BUFFER_SIZE]; // Response message
    ViUInt32   returnCnt;
    

    GenPSie_status = GenPSie_readQry (instrSession, 
                    "OUTP:PON?", returnStr, &returnCnt);
    if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
    
    if (0 == strncmp (returnStr, "ON", 2))
        *autoStartOn = VI_TRUE;
    else
        *autoStartOn = VI_FALSE;
    
    return GenPSie_status;
}

/*===========================================================================*/
/* Function: Set Foldback protection On or Off                               */
/* Purpose:  This function sends the ":CURR:PROT:STAT" command with          */
/*           the argument "ON" or "OFF"                                      */
/*===========================================================================*/

ViStatus _VI_FUNC GenPSie_setFoldback (ViSession instrSession,
                                              ViBoolean setFoldbackOn)
{
    ViStatus GenPSie_status = VI_SUCCESS;
    ViChar   cmdOn [20] = ":CURR:PROT:STAT ON";
    ViChar   cmdOff[20] = ":CURR:PROT:STAT OFF";
    
    if (setFoldbackOn) {
        GenPSie_status = GenPSie_sendCmd (instrSession, cmdOn);
        }
    else {
        GenPSie_status = GenPSie_sendCmd (instrSession, cmdOff);
        }
    
    return GenPSie_status;
}

/*===========================================================================*/
/* Function: Query if the Foldback protection is On or Off                   */
/* Purpose:  This function sends the ":CURR:PROT:STAT?" query and returns    */
/*                      True  = Foldback On                                  */
/*                      False = Foldback Off                                 */
/*===========================================================================*/

ViStatus _VI_FUNC GenPSie_readFoldback (ViSession instrSession,
                                              ViBoolean *foldbackOn)

{
    ViStatus   GenPSie_status = VI_SUCCESS;
    ViChar     returnStr [BUFFER_SIZE]; // Response message
    ViUInt32   returnCnt;
    

    GenPSie_status = GenPSie_readQry (instrSession, 
                    ":CURR:PROT:STAT?", returnStr, &returnCnt);
    if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
    
    if (0 == strncmp (returnStr, "ON", 2))
        *foldbackOn = VI_TRUE;
    else
        *foldbackOn = VI_FALSE;
    
    return GenPSie_status;
}

/*===========================================================================*/
/* Function: Set Remote Mode to  Local, Remote, or Local-Lockout             */
/* Purpose:  This function sends the "SYST:SET " command with                */
/*           the argument "LOC", "RMT" or "LLO"                              */
/*===========================================================================*/

ViStatus _VI_FUNC GenPSie_setRemoteMode (ViSession instrSession,
                                              ViInt16 remoteMode)

{
    ViStatus GenPSie_status = VI_SUCCESS;
    ViChar   cmdLOC [20] = "SYST:SET LOC";
    ViChar   cmdREM [20] = "SYST:SET REM";
    ViChar   cmdLLO [20] = "SYST:SET LLO";

    switch (remoteMode) {
        case GenLOCAL:
            GenPSie_status = GenPSie_sendCmd (instrSession, cmdLOC);
            break;
        case GenREMOTE:
            GenPSie_status = GenPSie_sendCmd (instrSession, cmdREM);
            break;
        case GenLLO:
            GenPSie_status = GenPSie_sendCmd (instrSession, cmdLLO);
            break;
        default:
            GenPSie_status = GenPSie_ERROR_COMMAND_ERROR;
        }
    
    return GenPSie_status;
}

/*===========================================================================*/
/* Function: Query if Remote Mode is Local, Remote or Local-Lockout          */
/* Purpose:  This function sends the "SYSTEM:SET?" query and returns         */
/*                      0  = GenLOCAL  = Local mode                          */
/*                      1  = GenREMOTE = Remote mode                         */
/*                      2  = GenLLO    = Local-Lockout mode                  */
/*===========================================================================*/

ViStatus _VI_FUNC GenPSie_readRemoteMode (ViSession instrSession,
                                              ViInt16 *remoteMode)

{
    ViStatus   GenPSie_status = VI_SUCCESS;
    ViChar     returnStr [BUFFER_SIZE]; // Response message
    ViUInt32   returnCnt;
    

    GenPSie_status = GenPSie_readQry (instrSession, 
                    "SYSTEM:SET?", returnStr, &returnCnt);
    if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
    
    if (0 == strncmp (returnStr, "LOC", 3))
        *remoteMode = GenLOCAL;
    else if (0 == strncmp (returnStr, "REM", 3))
        *remoteMode = GenREMOTE;
    else if (0 == strncmp (returnStr, "LLO", 3))
        *remoteMode = GenLLO;
    else 
        GenPSie_status = GenPSie_ERROR_COMMAND_ERROR;
    
    return GenPSie_status;
}

/*===========================================================================*/
/* Function: Set the Over-Voltage Protection (OVP) level                     */
/* Purpose:  This function inputs a floating-point value and sends it in a   */
/*              ":VOLT:PROT:LEV nn.nnnn" command                             */
/*           OVP values above 95% of the  output voltage setting or          */
/*           below zero will return with an error                            */
/*===========================================================================*/

ViStatus _VI_FUNC GenPSie_setOvpLevel (ViSession instrSession, 
                                                    ViReal64  ovpLevel)
{
    ViStatus GenPSie_status = VI_SUCCESS;
    ViChar   cmdString [20];
    
    Fmt (cmdString, "%s<%s%f", ":VOLT:PROT:LEV ", ovpLevel);

    GenPSie_status = GenPSie_sendCmd (instrSession, cmdString);
    
    return GenPSie_status;
}

/*===========================================================================*/
/* Function: Set the Over-Voltage Protection (OVP) to maximum                */
/* Purpose:  This function sends a ":VOLT:PROT:LEV MAX" command              */
/*===========================================================================*/

ViStatus _VI_FUNC GenPSie_setOvpMaxLevel (ViSession instrSession) 
{
    ViStatus GenPSie_status = VI_SUCCESS;
    
    GenPSie_status = GenPSie_sendCmd (instrSession, ":VOLT:PROT:LEV MAX");
    
    return GenPSie_status;
}

/*===========================================================================*/
/* Function: Read the Over-Voltage Protection (OVP) level                    */
/* Purpose:  This function sends the ":VOLT:PROT:LEV?" query and             */
/*           returns the response as a floating point value                  */
/*===========================================================================*/

ViStatus _VI_FUNC GenPSie_readOvpLevel (ViSession instrSession,
                                              ViReal64 *ovpLevel)
{
    ViStatus GenPSie_status = VI_SUCCESS;
    ViChar   returnStr [BUFFER_SIZE];   // Response message
    ViUInt32 returnCnt;

    GenPSie_status = GenPSie_readQry (instrSession, 
                    ":VOLT:PROT:LEV?", returnStr, &returnCnt);
    if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
    
    returnStr[returnCnt] = VI_NULL;     // add string terminator

    Fmt (ovpLevel, "%f<%s", returnStr);
    
    return GenPSie_status;
}


/*===========================================================================*/
/* Function: Set the Under-Voltage Limit (UVL) level                         */
/* Purpose:  This function inputs a floating-point value and sends it in a   */
/*           ":VOLT:LIM:LOW nn.nnnn" command                                 */
/*           UVL values below zero or above 95% of the  output voltage       */
/*           setting will return with an error                               */
/*===========================================================================*/

ViStatus _VI_FUNC GenPSie_setUvlLevel (ViSession instrSession, 
                                                    ViReal64  uvlLevel)
{
    ViStatus GenPSie_status = VI_SUCCESS;
    ViChar   cmdString [20];
    
    Fmt (cmdString, "%s<%s%f", ":VOLT:LIM:LOW ", uvlLevel);

    GenPSie_status = GenPSie_sendCmd (instrSession, cmdString);
    
    return GenPSie_status;
}

/*===========================================================================*/
/* Function: Read the Under-Voltage Limit (UVL) level                        */
/* Purpose:  This function sends the ":VOLT:LIM:LOW?" query and              */
/*           returns the response as a floating point value                  */
/*===========================================================================*/

ViStatus _VI_FUNC GenPSie_readUvlLevel (ViSession instrSession,
                                              ViReal64 *uvlLevel)
{
    ViStatus GenPSie_status = VI_SUCCESS;
    ViChar   returnStr [BUFFER_SIZE];   // Response message
    ViUInt32 returnCnt;

    GenPSie_status = GenPSie_readQry (instrSession, 
                    ":VOLT:LIM:LOW?", returnStr, &returnCnt);
    if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
    
    returnStr[returnCnt] = VI_NULL;     // add string terminator

    Fmt (uvlLevel, "%f<%s", returnStr);
    
    return GenPSie_status;
}

/*===========================================================================*/
/* Function: Read the output mode.  Returns null terminated string:          */
/*                "CV"  = Constant Voltage                                   */
/*                "CC"  = Constant Current                                   */
/*                "OFF" = Output off                                         */
/* Purpose:  This function sends the "SOUR:MOD?" query                       */
/*===========================================================================*/

ViStatus _VI_FUNC GenPSie_readOutputMode (ViSession instrSession,
                                             ViChar _VI_FAR outputMode[])
{
    ViStatus GenPSie_status = VI_SUCCESS;
    ViChar   returnStr [BUFFER_SIZE];       // Response message
    ViUInt32 returnCnt;

    GenPSie_status = GenPSie_readQry (instrSession, "SOUR:MOD?",  
                                                    returnStr, &returnCnt);
    if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
    
    returnStr[returnCnt] = VI_NULL;     // add string terminator

    strcpy (outputMode, returnStr);
    
    return GenPSie_status;
}

/*===========================================================================*/
/* Function: Reset power supply to known safe state                          */
/* Purpose:  This function performs a "*RST" reset command                   */
/* Settings changed to:                                                      */
/*      Output = off                                                         */
/*      Program voltage = zero volts                                         */
/*      Program current = zero amps                                          */
/*      Over-Voltage protection (OVP) = maximum                              */
/*      Under-Voltage limit (UVL) = zero                                     */
/*      Safe-Start = on                                                      */
/*      Foldback = off                                                       */
/*      Remote mode = remote (not lockout)                                   */
/*                                                                           */
/* The event registers are set to zeroes as if "*CLS" sent.                  */
/* The enable registers are not changed                                      */
/*===========================================================================*/

ViStatus _VI_FUNC GenPSie_reset (ViSession instrSession)
{
    ViStatus GenPSie_status = VI_SUCCESS;

    GenPSie_status = GenPSie_sendCmd (instrSession, "*RST");
    
    return GenPSie_status;
}

/*===========================================================================*/
/* Function: Save the power supply settings                                  */
/* Purpose:  This function performs a "*SAV 0" SAVE command                  */
/* The new settings replace those saved during last power-down or            */
/*   last SAVE command.                                                      */
/* The new settings may be replaced when the instrument is powered-down.     */
/*                                                                           */
/* Settings saved:                                                           */
/*      Output on or off                                                     */
/*      Programmed voltage setting                                           */
/*      Programmed current setting                                           */
/*      Over-Voltage protection (OVP) setting                                */
/*      Under-Voltage limit (UVL) setting                                    */
/*      Safe-Start or auto-start                                             */
/*      Foldback on or off                                                   */
/*      Remote mode, local, remote or lockout                                */
/*      IEEE Address                                                         */
/* The event registers are set to zeroes as if "*CLS" sent.                  */
/* The enable registers are not changed                                      */
/*===========================================================================*/

ViStatus _VI_FUNC GenPSie_saveSettings (ViSession instrSession)
{
    ViStatus GenPSie_status = VI_SUCCESS;

    GenPSie_status = GenPSie_sendCmd (instrSession, "*SAV 0");
    
    return GenPSie_status;
}

/*===========================================================================*/
/* Function: Recall the power supply settings                                */
/* Purpose:  This function performs a "*RCL 0" RECALL command                */
/* The recalled settings may be replaced when the instrument is powered-down */
/*                                                                           */
/* Settings recalled:                                                        */
/*      Output on or off                                                     */
/*      Programmed voltage setting                                           */
/*      Programmed current setting                                           */
/*      Over-Voltage protection (OVP) setting                                */
/*      Under-Voltage limit (UVL) setting                                    */
/*      Safe-Start or auto-start                                             */
/*      Foldback on or off                                                   */
/*      Remote mode, local, remote or lockout                                */
/*      IEEE Address                                                         */
/* The event registers are set to zeroes as if "*CLS" sent.                  */
/* The enable registers are not changed                                      */
/*===========================================================================*/

ViStatus _VI_FUNC GenPSie_recallSettings (ViSession instrSession)
{
    ViStatus GenPSie_status = VI_SUCCESS;

    GenPSie_status = GenPSie_sendCmd (instrSession, "*RCL 0");
    
    return GenPSie_status;
}

/*===========================================================================*/
/* Function: Read the faults that have caused a power supply shut-down 
/* Purpose:  This function sends the "STATUS:QUESTIONABLE:CONDITION?" query
/* Returned values:
/*   faultBits = eight bits set to indicate fault conditions.
/*               MSB                                LSB
/*               ENA  OFF  SO   OVP  FLD  OTP  AC   "0"
/*
/*   faultName = Null-terminated character string
/*               No Fault = "" (null string)
/*               Fault = any one of "ENA" "OFF" "SO" "OVP"  FLD" "OTP" "AC"
/*            If multiple faults exist, only one fault name is returned.
/*===========================================================================*/

ViStatus _VI_FUNC GenPSie_readFaultShutdowns (ViSession instrSession,
                                 ViChar *faultBits, ViChar *faultName)

{
    ViStatus   GenPSie_status = VI_SUCCESS;
    ViInt32    faultDecimal = 0;        // Integer representation of faults
    ViChar     returnStr [BUFFER_SIZE]; // Response message
    ViUInt32   returnCnt;
    

    GenPSie_status = GenPSie_readQry (instrSession, 
                    "STAT:QUES:COND?", returnStr, &returnCnt);
    if (GenPSie_status != VI_SUCCESS) return (GenPSie_status);
    
    // Save decimal fault register value as eight bits
    Fmt ( &faultDecimal, "%i<%s", returnStr );
    *faultBits = (char) faultDecimal;

    strcpy (faultName, "");

    // One or more faults exist. 
    // Search for first fault and copy its name into faultName

    if (faultDecimal & 2)
        strcpy (faultName, "AC");
    else if (faultDecimal & 4)
        strcpy (faultName, "OTP");
    else if (faultDecimal & 8)
        strcpy (faultName, "FLD");
    else if (faultDecimal & 16)
        strcpy (faultName, "OVP");
    else if (faultDecimal & 32)
        strcpy (faultName, "SO");
    else if (faultDecimal & 64)
        strcpy (faultName, "OFF");
    else if (faultDecimal & 128)
        strcpy (faultName, "ENA");
    
    return GenPSie_status;
}



/*****************************************************************************/
/*= UTILITY ROUTINES (Non-Exportable Functions) =============================*/
/*****************************************************************************/

/*===========================================================================*/
/* Function: Convert SCPI error code to VISA error code                      */
/* Purpose:  This function accepts a SCPI error number which is returned     */
/*           from a SYST:ERR? query and has a range from +100 to -300.       */
/*           It returns a VISA error number using these conversion rules:    */
/*              Input "+0", return 0 Long (no error)                         */
/*              Input "+300 to +399" return 0x3FFC0900L + number (warnings)  */
/*              Input "-100 to -222" return 0xBFFC0900L + number (errors)    */
/*===========================================================================*/
ViStatus GenPSie_convertErrNumb (ViInt32 systErr)
{

    ViInt32     visaErr;        // SCPI error number converted to VISA

    if (systErr > 0L)                   // Positive error = warning
        visaErr = systErr + VI_WARN_INSTR_OFFSET;

    else if (systErr < 0L)              // Negative error = error
        visaErr = (-1L * systErr) + VI_ERROR_INSTR_OFFSET;

    else                                // Zero error = no error
        visaErr = 0L;

    return (visaErr);
}

/*===========================================================================*/
/* Function: Boolean Value Out Of Range - ViBoolean                          */
/* Purpose:  This function checks a Boolean to see if it is equal to VI_TRUE */
/*           or VI_FALSE. If the value is out of range, the return value is  */
/*           VI_TRUE, otherwise the return value is VI_FALSE.                */
/*===========================================================================*/
ViBoolean GenPSie_invalidViBooleanRange (ViBoolean val)
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
ViBoolean GenPSie_invalidViInt16Range (ViInt16 val, ViInt16 min, ViInt16 max)
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
ViBoolean GenPSie_invalidViInt32Range (ViInt32 val, ViInt32 min, ViInt32 max)
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
ViBoolean GenPSie_invalidViUInt8Range (ViUInt8 val, ViUInt8 min, ViUInt8 max)
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
ViBoolean GenPSie_invalidViUInt16Range (ViUInt16 val, ViUInt16 min, ViUInt16 max)
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
ViBoolean GenPSie_invalidViUInt32Range (ViUInt32 val, ViUInt32 min, ViUInt32 max)
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
ViBoolean GenPSie_invalidViReal32Range (ViReal32 val, ViReal32 min, ViReal32 max)
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
ViBoolean GenPSie_invalidViReal64Range (ViReal64 val, ViReal64 min, ViReal64 max)
{
    return ((val < min || val > max) ? VI_TRUE : VI_FALSE);
}

/*============================================================================*/
/* Function: Wait On VISA Event                                               */
/* Purpose:  This function waits for the occurrence of an enabled VISA Event. */
/*       The event type must be enabled before entering this function. Any    */
/*       programmatic commands, actions, or conditions, necessary to generate */
/*       the specified event type, must be satisified before entering this    */
/*       function. The function will wait for the specified timeout and then  */
/*       return. If the specified event is received then the function will    */
/*       return VI_SUCCESS (0), otherwise the status code for the generated   */
/*       error will be returned. In either case the function will disable the */
/*       event type and deallocate the event handle passed from viWaitOnEvent */
/*       before returning. WARNING: If VI_TMO_INFINITE is passed in as the    */
/*       timeout this function WILL NOT return until the specified event is   */
/*       received, if the event is not received the function will not return  */
/*       and it will be necessary to terminate CVI in order to regain control.*/
/*============================================================================*/
ViStatus GenPSie_waitOnVisaEvent (ViSession instrSession, 
                    ViEventType eventTypeIn, ViUInt32 timeoutIn, ViPUInt16 STB)
{            
    ViStatus GenPSie_status = VI_SUCCESS, tempStatus = VI_SUCCESS;
    ViEventType eventTypeOut = 0;
    ViEvent eventHandle = 0;
    
    /* For debug purposes we want to be able to see the status returned by        */
    /* viDisableEvent and viClose if one of the internal functions fails but do   */
    /* not want to return that value as that is not where the initial error       */
    /* occurs in the function, so we assign it to tempStatus.                     */
    
    if ((GenPSie_status = viWaitOnEvent (instrSession, eventTypeIn, timeoutIn, 
                                        &eventTypeOut, &eventHandle)) < 0) {
        tempStatus = viDisableEvent (instrSession, eventTypeIn, VI_QUEUE);
        return GenPSie_status;
    } 
    
    if (eventTypeIn == VI_EVENT_SERVICE_REQ)
        if ((GenPSie_status = viReadSTB (instrSession, STB)) < 0) {
            tempStatus = viClose (eventHandle);
            tempStatus = viDisableEvent (instrSession, eventTypeIn, VI_QUEUE);
            return GenPSie_status;
        }   
        
    if ((GenPSie_status = viClose (eventHandle)) < 0) {             
        tempStatus = viDisableEvent (instrSession, eventTypeIn, VI_QUEUE);
        return GenPSie_status;                 
    }
    
    if ((GenPSie_status = viDisableEvent (instrSession, eventTypeIn, 
                                                                VI_QUEUE)) < 0)
        return GenPSie_status;                                                   

    return GenPSie_status;                                    
}    

/*=========================================================================== */
/* Function: Initialize Clean Up                                              */
/* Purpose:  This function is used only by the GenPSie_init function.  When   */
/*           an error is detected this function is called to close the        */
/*           open resource manager and instrument object sessions and to      */
/*           set the instrSession that is returned from GenPSie_init to       */
/*           VI_NULL.                                                         */
/*=========================================================================== */
ViStatus GenPSie_initCleanUp (ViSession openRMSession,
                    ViPSession openInstrSession, ViStatus currentStatus)
{
    GenPSie_instrRange instrPtr;
    
    if (viGetAttribute (*openInstrSession, VI_ATTR_USER_DATA, &instrPtr) >= 0)
        if (instrPtr != NULL) 
            free (instrPtr);

    viClose (*openInstrSession);
    viClose (openRMSession);
    *openInstrSession = VI_NULL;
    
    return currentStatus;
}

/*===========================================================================*/
/* Function: Read To File From Instrument                                    */
/* Purpose:  This function is used to read data from the instrument and      */
/*           write it to a user specified file.                              */
/*===========================================================================*/
ViStatus GenPSie_readToFile (ViSession instrSession, ViString filename,
                    ViUInt32 readBytes, ViPUInt32 retCount)
{
    ViStatus  GenPSie_status = VI_SUCCESS;
    ViByte    buffer[BUFFER_SIZE];
    ViUInt32  bytesReadInstr = 0, bytesWrittenFile = 0;
    FILE     *targetFile;

    *retCount = 0L;
    if ((targetFile = fopen (filename, "wb")) == VI_NULL)
        return VI_ERROR_INSTR_FILE_OPEN; /* not defined by VTL */

    for (;;) {
        if (readBytes > BUFFER_SIZE)
            GenPSie_status = viRead (instrSession, buffer, BUFFER_SIZE, 
                                                            &bytesReadInstr);
        else
            GenPSie_status = viRead (instrSession, buffer, readBytes, 
                                                            &bytesReadInstr);

        bytesWrittenFile = fwrite (buffer, sizeof (ViByte), 
                                        (size_t)bytesReadInstr, targetFile);
        *retCount += bytesWrittenFile;
        if (bytesWrittenFile < bytesReadInstr)
            GenPSie_status = VI_ERROR_INSTR_FILE_WRITE; /* not defined by VTL */

        if ((readBytes <= BUFFER_SIZE) || (GenPSie_status <= 0) || 
                                    (GenPSie_status == VI_SUCCESS_TERM_CHAR))
            break;

        readBytes -= BUFFER_SIZE;
    }

    fclose (targetFile);
    return GenPSie_status;
}

/*===========================================================================*/
/* Function: Write From File To Instrument                                   */
/* Purpose:  This function is used to read data from a user specified file   */
/*           and write it to the instrument.                                 */
/*===========================================================================*/
ViStatus GenPSie_writeFromFile (ViSession instrSession, ViString filename,
                    ViUInt32 writeBytes, ViPUInt32 retCount)
{
    ViStatus  GenPSie_status = VI_SUCCESS;
    ViByte    buffer[BUFFER_SIZE];
    ViUInt32  bytesRead = 0, bytesWritten = 0;
    FILE     *sourceFile;
    ViBoolean sendEnd = VI_FALSE;

    *retCount = 0L;
    if ((sourceFile = fopen (filename, "rb")) == VI_NULL)
        return VI_ERROR_INSTR_FILE_OPEN; /* not defined by VTL */

    while (!feof (sourceFile)) {
        bytesRead = (ViUInt32)fread (buffer, sizeof (ViByte), BUFFER_SIZE, 
                                                                    sourceFile);
        if ((writeBytes > BUFFER_SIZE) && (bytesRead == BUFFER_SIZE)) {
            viGetAttribute (instrSession, VI_ATTR_SEND_END_EN, &sendEnd);
            viSetAttribute (instrSession, VI_ATTR_SEND_END_EN, VI_FALSE);
            GenPSie_status = viWrite (instrSession, buffer, BUFFER_SIZE, 
                                                                &bytesWritten);
            viSetAttribute (instrSession, VI_ATTR_SEND_END_EN, sendEnd);
            writeBytes -= BUFFER_SIZE;
            *retCount += bytesWritten;
            if (GenPSie_status < 0)
                break;
        }
        else {
            GenPSie_status = viWrite (instrSession, buffer, 
                        ((bytesRead < writeBytes) ? bytesRead : writeBytes), 
                        &bytesWritten);
            *retCount += bytesWritten;
            break;
        }
    }

    fclose (sourceFile);
    return GenPSie_status;
}


/*===========================================================================*/
/* Function: Read serial poll Status Byte                                    */
/* Purpose:  This function will red the IEEE-488 Status Byte register.       */
/*           The STB register is typically used to read 3 non-maskable bits: */
/*             1. MAV  bit: Message Available (read from Output Queue)       */
/*             2. SYST bit: System Error message exists                      */
/*             3. BUSY bit: Set when other status bits are not valid         */
/*           The STB register can also be enabled to cause Service Requests  */
/*             as a result of any other status bit being set                 */
/*           This function will return with timeout error (TMO) if           */
/*             the BUSY bit is not cleared within 3 seconds                  */
/*===========================================================================*/

ViStatus _VI_FUNC GenPSie_STB (ViSession instrSession, ViUInt16* spollByte)
{
    ViStatus    GenPSie_status = VI_SUCCESS;
    ViUInt16    statByte;       // Status byte read during serial poll
    ViUInt16    retries = 0;    // Number of serial poll retries until NOT BUSY
    

    GenPSie_waitMS (5);
    GenPSie_status = viReadSTB (instrSession, &statByte);
    if (GenPSie_status < VI_SUCCESS)        // If VISA comm error occured,
        return (GenPSie_status);            // return with VISA error

    // Loop until BUSY bit is low, otherwise status byte read is not valid
    // Loop for up to 3 seconds (30 reads * 10 mSec delay)
    
    while (statByte & BUSY) {
        GenPSie_waitMS (10);
        ++retries;
        if (retries >= 300) 
            // Return if status BUSY bit not cleared in 3 sec 
            return (GenPSie_ERROR_BUSY_STUCK_HIGH); 
        GenPSie_status = viReadSTB (instrSession, &statByte);
         if (GenPSie_status < VI_SUCCESS)
            return (GenPSie_status);
    }   

    *spollByte = statByte;
    return (VI_SUCCESS);
}

/*===========================================================================*/
/* Function: Wait MilliSeconds                                               */
/* Purpose:  This function will return                                       */
/*           after the specified duration in milliseconds                    */
/*===========================================================================*/
void GenPSie_waitMS (ViUInt16 mSecWait)
{
    ViReal64    secWait;

    secWait = (double) mSecWait / 1000;     // Convert mSec to seconds

    Delay (secWait);
    
    return;
}


/*****************************************************************************/
/*----------- INSERT INSTRUMENT-DEPENDENT UTILITY ROUTINES HERE -------------*/
/*****************************************************************************/

/*===========================================================================*/
/* Function: Default Instrument Setup                                        */
/* Purpose:  There are no default setups except to enable error reporting    */
/*           The reset (*RST) command does not change the need for this cmd  */
/*===========================================================================*/
ViStatus GenPSie_defaultInstrSetup (ViSession instrSession)
{
    ViStatus GenPSie_status;
    
    GenPSie_status = GenPSie_WARN_NSUP_DEFLT_INSTR_SETUP;

    return (GenPSie_status);
}

/*****************************************************************************/
/*=== END INSTRUMENT DRIVER SOURCE CODE =====================================*/
/*****************************************************************************/

