#include <cvidef.h>

/*======= GenPSie.h                                                           */
/*======= Lambda GENESYS Power Supply with IEEE-488. Include File ============*/

/*===========================================================================*/
/*  Please do not use global variables or arrays in the include file of      */
/*  instrument drivers that will be submitted for inclusion into the         */
/*  LabWindows Instrument Driver Library.                                    */
/*===========================================================================*/
     
#ifndef __GenPSie_HEADER
#define __GenPSie_HEADER

#include <vpptype.h>

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

#define BUFFER_SIZE     128L            /* Genesys File I/O buffer size */
#define GEN_TMO         3000            /* Genesys timeout = 3 sec      */

#define GenLOCAL        0               /* Genesys System Mode Local         */
#define GenREMOTE       1               /* Genesys System Mode Remote        */
#define GenLLO          2               /* Genesys System Mode Local-Lockout */    



/*****************************************************************************/
/*= Instrument Driver Specific Error/Warning Codes ==========================*/
/*                                                                           */
/* part of "visatype.h":   #define _VI_ERROR (-2147483647L-1)  0x80000000    */
/*****************************************************************************/


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
/*        The codes for instrument specific ERRORS must be in the range       */
/*        0xBFFC0900 to 0xBFFC0FFF and defined as follows:                    */
/*        #define GenPSie_SPECIFIC_ERROR (VI_ERROR_INSTR_OFFSET + n)          */
/*                                                                            */
/*        The codes for instrument specific WARNINGS must be in the range     */
/*        0x3FFC0900 to 0x3FFC0FFF and defined as follows:                    */
/*        #define GenPSie_SPECIFIC_WARNING (VI_WARNING_INSTR_OFFSET + n)      */
/*                                                                            */
/*        Where n represents each instrument specific error/warning number.   */
/*        The valid range for n is 0 to 0x6FF. For ease of use the lower 16   */
/*        bits of warnings and errors should not overlap in the same driver.  */
/*****************************************************************************/

#define VI_WARN_INSTR_OFFSET                              (0x3FFC0900L)
#define VI_ERROR_INSTR_OFFSET                 (_VI_ERROR + 0x3FFC0900L)

#define VI_WRN                                VI_WARN_INSTR_OFFSET
#define VI_ERR                                VI_ERROR_INSTR_OFFSET

#define GenPSie_ERROR_IEEE_WRONG_REV        (VI_ERR+0x008L) // -008=BFFC0908
#define GenPSie_ERROR_PARSING_IDENTITY      (VI_ERR+0x00AL) // -010=BFFC090A
#define GenPSie_ERROR_BUSY_STUCK_HIGH       (VI_ERR+0x020L) // -032=BFFC0920
#define GenPSie_ERROR_NO_MAV_AFTER_SYSTERR  (VI_ERR+0x022L) // -034=BFFC0922
#define GenPSie_ERROR_SYSTERR_SHORT_RESP    (VI_ERR+0x024L) // -036=BFFC0924

#define GenPSie_ERROR_SYSTERR_NO_ERR        (VI_ERR+0x026L) // -038=BFFC0926
#define GenPSie_ERROR_MAV_SET_AFTER_CMD     (VI_ERR+0x028L) // -040=BFFC0928
#define GenPSie_ERROR_QRY_NO_MAV_NO_ERR     (VI_ERR+0x02AL) // -042=BFFC092A
#define GenPSie_ERROR_COMMAND_ERROR         (VI_ERR+0x064L) // -100=BFFC0964
#define GenPSie_ERROR_INVALID_CHARACTER     (VI_ERR+0x065L) // -101=BFFC0965

#define GenPSie_ERROR_SYNTAX_ERROR          (VI_ERR+0x066L) // -102=BFFC0966
#define GenPSie_ERROR_DATA_TYPE_ERROR       (VI_ERR+0x068L) // -104=BFFC0968
#define GenPSie_ERROR_MISSING_PARAMETER     (VI_ERR+0x06DL) // -109=BFFC096D
#define GenPSie_ERROR_WORD_TOO_LONG         (VI_ERR+0x070L) // -112=BFFC0970
#define GenPSie_ERROR_DATA_OUT_OF_RANGE     (VI_ERR+0x0DEL) // -222=BFFC09DE

#define GenPSie_WARN_EXECUTION_ERROR        (VI_WRN+0x12CL) // +300=3FFC0A2C
#define GenPSie_WARN_PV_ABOVE_OVP           (VI_WRN+0x12DL) // +301=3FFC0A2D
#define GenPSie_WARN_PV_BELOW_UVL           (VI_WRN+0x12EL) // +302=3FFC0A2E
#define GenPSie_WARN_OVP_ABOVE_RATING       (VI_WRN+0x12FL) // +303=3FFC0A2F
#define GenPSie_WARN_OVP_BELOW_PV           (VI_WRN+0x130L) // +304=3FFC0A30

#define GenPSie_WARN_UVL_BELOW_ZERO         (VI_WRN+0x131L) // +305=3FFC0A31
#define GenPSie_WARN_UVL_ABOVE_PV           (VI_WRN+0x132L) // +306=3FFC0A32
#define GenPSie_WARN_ON_DURING_FAULT        (VI_WRN+0x133L) // +307=3FFC0A33
#define GenPSie_WARN_FAULT_SHUTDOWN         (VI_WRN+0x140L) // +320=3FFC0A40
#define GenPSie_WARN_AC_FAULT_SHUTDOWN      (VI_WRN+0x141L) // +321=3FFC0A41

#define GenPSie_WARN_OVER_TEMP_SHUTDOWN     (VI_WRN+0x142L) // +322=3FFC0A42
#define GenPSie_WARN_FOLD_BACK_SHUTDOWN     (VI_WRN+0x143L) // +323=3FFC0A43
#define GenPSie_WARN_OVER_VOLTAGE_SHUTDOWN  (VI_WRN+0x144L) // +324=3FFC0A44
#define GenPSie_WARN_SHUT_OFF_SHUTDOWN      (VI_WRN+0x145L) // +325=3FFC0A45
#define GenPSie_WARN_OUTPUT_OFF_SHUTDOWN    (VI_WRN+0x146L) // +326=3FFC0A46

#define GenPSie_WARN_ENABLE_OPENED_SHUTDOWN (VI_WRN+0x147L) // +327=3FFC0A47
#define GenPSie_WARN_INTERNAL_MESSAGE_FAULT (VI_WRN+0x154L) // +340=3FFC0A54
#define GenPSie_WARN_INPUT_OVERFLOW         (VI_WRN+0x155L) // +341=3FFC0A55
#define GenPSie_WARN_INTERNAL_OVERFLOW      (VI_WRN+0x156L) // +342=3FFC0A56
#define GenPSie_WARN_INTERNAL_TIMEOUT       (VI_WRN+0x157L) // +343=3FFC0A57

#define GenPSie_WARN_INTERNAL_COMM_ERROR    (VI_WRN+0x158L) // +344=3FFC0A58
#define GenPSie_WARN_INTERNAL_CHECKSUM_ERROR (VI_WRN+0x159L)// +345=3FFC0A59
#define GenPSie_WARN_QUEUE_OVERFLOW         (VI_WRN+0x15EL) // +350=3FFC0A5E
#define GenPSie_WARN_DISPLAY_COMM_ERROR     (VI_WRN+0x18FL) // +399=3FFC0A8F

#define GenPSie_ERROR_INVALID_CONFIGURATION (VI_ERR+0xF00L) // 3840=BFFC1800
#define GenPSie_WARN_NSUP_DEFLT_INSTR_SETUP (VI_ERR+0xF02L) // 3842=BFFC1802


/*****************************************************************************/
/*= GLOBAL USER-CALLABLE FUNCTION DECLARATIONS (Exportable Functions) =======*/
/*****************************************************************************/

/** INSERT USER-CALLABLE INSTRUMENT-DEPENDENT FUNCTION DECLARATIONS HERE **/

ViStatus _VI_FUNC GenPSie_init (ViRsrc resourceName, ViBoolean IDQuery,
                    ViBoolean resetDevice, ViPSession instrSession);
ViStatus _VI_FUNC GenPSie_sendCmd (ViSession instrSession, ViString cmdString);
ViStatus _VI_FUNC GenPSie_readQry (ViSession instrSession, 
                    ViString qryStr, ViString retStr, ViUInt32 *retCnt);
ViStatus _VI_FUNC GenPSie_STB (ViSession instrSession, ViUInt16 *spollByte);

ViStatus _VI_FUNC GenPSie_programVoltage (ViSession instrumentHandle,
                                          ViReal64 voltageSetting,
                                          ViInt16 settlingDelay);
ViStatus _VI_FUNC GenPSie_programCurrent (ViSession instrSession,
                                          ViReal64  currentSetting,
                                          ViInt16   settlingDelay);

ViStatus _VI_FUNC GenPSie_measureVoltage (ViSession instrumentHandle,
                                          ViReal64 *measuredVoltage);
ViStatus _VI_FUNC GenPSie_measureCurrent (ViSession instrSession,
                                          ViReal64 *measuredCurrent);


ViStatus _VI_FUNC GenPSie_readVoltageSetting (ViSession instrSession,
                                              ViReal64 *voltageSetting);

ViStatus _VI_FUNC GenPSie_readCurrentSetting (ViSession instrSession,
                                              ViReal64 *currentSetting);

ViStatus _VI_FUNC GenPSie_setOutputOn (ViSession instrSession,
                                              ViBoolean setOutputOn);
                                              
ViStatus _VI_FUNC GenPSie_readOutputOn (ViSession instrSession,
                                              ViBoolean *outputEnabled);
                                              
ViStatus _VI_FUNC GenPSie_setAutoStart (ViSession instrSession,
                                              ViBoolean setAutoStartOn);
ViStatus _VI_FUNC GenPSie_readAutoStart (ViSession instrSession,
                                              ViBoolean *autoStartOn);

ViStatus _VI_FUNC GenPSie_setFoldback (ViSession instrSession,
                                              ViBoolean setFoldbackOn);
ViStatus _VI_FUNC GenPSie_readFoldback (ViSession instrSession,
                                              ViBoolean *foldbackOn);

ViStatus _VI_FUNC GenPSie_setRemoteMode (ViSession instrSession,
                                              ViInt16 systemMode);
ViStatus _VI_FUNC GenPSie_readRemoteMode (ViSession instrSession,
                                              ViInt16 *remoteMode);
                                              
ViStatus _VI_FUNC GenPSie_setOvpLevel (ViSession instrSession, 
                                              ViReal64  ovpLevel);
ViStatus _VI_FUNC GenPSie_readOvpLevel (ViSession instrSession,
                                              ViReal64 *ovpLevel);
ViStatus _VI_FUNC GenPSie_setUvlLevel (ViSession instrSession, 
                                              ViReal64  uvlLevel);
ViStatus _VI_FUNC GenPSie_readUvlLevel (ViSession instrSession,
                                              ViReal64 *uvlLevel);
ViStatus _VI_FUNC GenPSie_setOvpMaxLevel (ViSession instrSession);

ViStatus _VI_FUNC GenPSie_readOutputMode (ViSession instrSession,
                                             ViChar _VI_FAR outputMode[]);

ViStatus _VI_FUNC GenPSie_saveSettings (ViSession instrSession);
ViStatus _VI_FUNC GenPSie_recallSettings (ViSession instrSession);


ViStatus _VI_FUNC GenPSie_writeInstrData (ViSession instrSession, 
                                            ViString writeBuffer);
ViStatus _VI_FUNC GenPSie_readInstrData (ViSession instrSession,
                    ViInt32 numberBytesToRead, ViChar _VI_FAR readBuffer[], 
                                            ViPInt32 numBytesRead);
ViStatus _VI_FUNC GenPSie_reset (ViSession instrSession);
ViStatus _VI_FUNC GenPSie_selfTest (ViSession instrSession, 
                                            ViPInt16 selfTestResult,
                                            ViChar _VI_FAR selfTestMessage[]);

ViStatus _VI_FUNC GenPSie_readFaultShutdowns (ViSession instrSession,
                                 ViChar *faultBits, ViChar *faultName);


ViStatus _VI_FUNC GenPSie_errorQuery (ViSession instrSession, 
                                            ViPInt32 scpiCode,
                                            ViChar _VI_FAR errorMessage[], 
                                            ViPInt32 visaCode);

ViStatus _VI_FUNC GenPSie_errorMessage (ViSession instrSession, 
                                            ViStatus statusCode,
                                            ViChar _VI_FAR message[]);
ViStatus _VI_FUNC GenPSie_revisionQuery (ViSession instrSession,
                                            ViChar _VI_FAR driverRev[], 
                                            ViChar _VI_FAR instrRev[]);

ViStatus _VI_FUNC GenPSie_identityQuery (ViSession instrSession,
                                         ViReal64 *maximumVoltage,
                                         ViReal64 *maximumCurrent,
                                         ViChar serialNumber[],
                                         ViChar mainRevision[],
                                         ViChar IEEERevision[]);

ViStatus _VI_FUNC GenPSie_close (ViSession instrSession);

#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

/*****************************************************************************/
/*=== END INCLUDE FILE ======================================================*/
/*****************************************************************************/

#endif
