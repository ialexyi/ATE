/*****************************************************************************/
/*  hpesa.h                                                                  */
/*  Copyright (C) 1999-2000 Agilent Technologies                             */
/*                                                                           */
/*  Driver for hpesa, Agilent Technologies ESA Series Spectrum Analyzer      */
/*  Driver Version: A.03.00                                                  */
/*                                                                           */
/*****************************************************************************/

#ifndef hpesa_INCLUDE 
#define hpesa_INCLUDE
#include "vpptype.h"

/* Used for "C" externs in C++ */
#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

/*****************************************************************************/
/*  STANDARD SECTION                                                         */
/*  Constants and function prototypes for Agilent standard functions.        */
/*****************************************************************************/


        /*-------------------------------------------------*
         |  Standard constant error conditions returned    |
         |  by driver functions.                           |
		 |                                                 |
         |  Agilent Common Error numbers start at BFFC0D00 |
         |  The parameter errors extend the number of      |
         |  errors over the eight defined in VPP 3.4       |
         *-------------------------------------------------*/

#define hpesa_INSTR_ERROR_NULL_PTR      (_VI_ERROR+0x3FFC0D02L) /* 0xBFFC0D02 */
#define hpesa_INSTR_ERROR_RESET_FAILED  (_VI_ERROR+0x3FFC0D03L) /* 0xBFFC0D03 */
#define hpesa_INSTR_ERROR_UNEXPECTED    (_VI_ERROR+0x3FFC0D04L) /* 0xBFFC0D04 */
#define hpesa_INSTR_ERROR_INV_SESSION   (_VI_ERROR+0x3FFC0D05L) /* 0xBFFC0D05 */
#define hpesa_INSTR_ERROR_LOOKUP        (_VI_ERROR+0x3FFC0D06L) /* 0xBFFC0D06 */
#define hpesa_INSTR_ERROR_DETECTED      (_VI_ERROR+0x3FFC0D07L) /* 0xBFFC0D07 */
#define hpesa_INSTR_NO_LAST_COMMA       (_VI_ERROR+0x3FFC0D08L) /* 0xBFFC0D08 */ 
#define hpesa_INSTR_INV_ASCII_NUMBER    (_VI_ERROR+0x3FFC0D09L) /* 0xBFFC0D09 */

#define hpesa_INSTR_ERROR_PARAMETER9    (_VI_ERROR+0x3FFC0D20L) /* 0xBFFC0D20 */
#define hpesa_INSTR_ERROR_PARAMETER10   (_VI_ERROR+0x3FFC0D21L) /* 0xBFFC0D21 */
#define hpesa_INSTR_ERROR_PARAMETER11   (_VI_ERROR+0x3FFC0D22L) /* 0xBFFC0D22 */
#define hpesa_INSTR_ERROR_PARAMETER12   (_VI_ERROR+0x3FFC0D23L) /* 0xBFFC0D23 */
#define hpesa_INSTR_ERROR_PARAMETER13   (_VI_ERROR+0x3FFC0D24L) /* 0xBFFC0D24 */
#define hpesa_INSTR_ERROR_PARAMETER14   (_VI_ERROR+0x3FFC0D25L) /* 0xBFFC0D25 */
#define hpesa_INSTR_ERROR_PARAMETER15   (_VI_ERROR+0x3FFC0D26L) /* 0xBFFC0D26 */
#define hpesa_INSTR_ERROR_PARAMETER16   (_VI_ERROR+0x3FFC0D27L) /* 0xBFFC0D27 */
#define hpesa_INSTR_ERROR_PARAMETER17   (_VI_ERROR+0x3FFC0D28L) /* 0xBFFC0D28 */
#define hpesa_INSTR_ERROR_PARAMETER18   (_VI_ERROR+0x3FFC0D29L) /* 0xBFFC0D29 */
#define hpesa_INSTR_ERROR_INVALID_APP   (_VI_ERROR+0x3FFC0D2AL) /* 0xBFFC0D2A */
#define hpesa_INSTR_ERROR_INVALID_RESULTS_TYPE   (_VI_ERROR+0x3FFC0D2BL) /* 0xBFFC0D2B */

        /*-------------------------------------------------*
         |  Constants used by system status functions      |
         |    These defines are bit numbers which define   |
         |    the operation and questionable registers.    |
         |    They are instrument specific.                |
         *-------------------------------------------------*/

#define hpesa_QUES_BIT0          1
#define hpesa_QUES_BIT1          2
#define hpesa_QUES_BIT2          4
#define hpesa_QUES_BIT3          8
#define hpesa_QUES_BIT4         16
#define hpesa_QUES_BIT5         32
#define hpesa_QUES_BIT6         64
#define hpesa_QUES_BIT7        128
#define hpesa_QUES_BIT8        256
#define hpesa_QUES_BIT9        512
#define hpesa_QUES_BIT10      1024
#define hpesa_QUES_BIT11      2048
#define hpesa_QUES_BIT12      4096
#define hpesa_QUES_BIT13      8192
#define hpesa_QUES_BIT14     16384
#define hpesa_QUES_BIT15     32768

#define hpesa_OPER_BIT0          1
#define hpesa_OPER_BIT1          2
#define hpesa_OPER_BIT2          4
#define hpesa_OPER_BIT3          8
#define hpesa_OPER_BIT4         16
#define hpesa_OPER_BIT5         32
#define hpesa_OPER_BIT6         64
#define hpesa_OPER_BIT7        128
#define hpesa_OPER_BIT8        256
#define hpesa_OPER_BIT9        512
#define hpesa_OPER_BIT10      1024
#define hpesa_OPER_BIT11      2048
#define hpesa_OPER_BIT12      4096
#define hpesa_OPER_BIT13      8192
#define hpesa_OPER_BIT14     16384
#define hpesa_OPER_BIT15     32768

        /*-------------------------------------------------*
         |  Constants used by function hpesa_timeOut       |
         *-------------------------------------------------*/

#define hpesa_TIMEOUT_MAX         2147483647L
#define hpesa_TIMEOUT_MIN         0L

        /*------------------------------------------------------*
         |  Constants used to determine min and max array sizes |
         *------------------------------------------------------*/

#define hpesa_CMDINT16ARR_Q_MIN   1L
#define hpesa_CMDINT16ARR_Q_MAX   2147483647L

#define hpesa_CMDINT32ARR_Q_MIN   1L
#define hpesa_CMDINT32ARR_Q_MAX   2147483647L

#define hpesa_CMDREAL64ARR_Q_MIN  1L
#define hpesa_CMDREAL64ARR_Q_MAX  2147483647L

#define hpesa_CMDSTRINGARR_Q_MIN  2L
#define hpesa_CMDSTRINGARR_Q_MAX  2147483647L


        /***************************************************/
        /*  Required VXIplug&play functions from VPP-3.1   */
        /***************************************************/

/*****************************************************************************/
/*  hpesa_init -                                                             */
/*    Initializes the instrument session and returns a valid instrument      */
/*    handle.  This will always be the first function executed to control    */
/*    an instrument.  The instrument handle is used to call all other driver */
/*    functions.                                                             */
/*                                                                           */
/*    The instrument is identified using the response to the "*IDN?" query   */
/*    and it is suggested that the response up to the Model Number be used   */
/*    for comparison.  The firmware revision should not be considered.       */
/*    A model version number may or may not be included.                     */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession      resourceName (in)                                       */
/*      The instrument's VISA address.                                       */
/*      GPIB Example: "GPIB0::18::INSTR"                                     */
/*    ViBoolean      IDQuery (in)                                            */
/*      If VI_TRUE, validate instrument identity                             */
/*    ViBoolean      resetDevice (in)                                        */
/*      If VI_TRUE, perform an instrument reset                              */
/*    ViPSession     instrumentHandle (out)                                  */
/*      Pointer to Instrument Handle.  Returns VI_NULL if the init fails.    */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_init
                        (ViRsrc        resourceName,
                         ViBoolean    IDQuery,
                         ViBoolean    resetDevice,
                         ViPSession    instrumentHandle);


/*****************************************************************************/
/*  hpesa_close -                                                            */
/*    Closes the VISA session associated with the instrument handle and      */
/*    frees all resources associated with that session.  This is typically   */
/*    the last call to the driver from the test program.                     */
/*                                                                           */  
/*  PARAMETERS                                                               */
/*    ViSession      instrumentHandle (in)                                   */
/*      Instrument Handle returned from hpesa_init()                         */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_close
                        (ViSession    instrumentHandle);


/*****************************************************************************/
/*  hpesa_reset -                                                            */
/*    Returns the instrument to a known state.  If the instrument does not   */
/*    provide a reset capability, the driver must set the instrument to a    */
/*    known state explicitly in this routine.                                */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession      instrumentHandle (in)                                   */
/*      Instrument Handle returned from hpesa_init()                         */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_reset
                        (ViSession instrumentHandle);


/*****************************************************************************/
/*  hpesa_self_test -                                                        */
/*    Performs a self-test of the instrument and returns the self-test       */
/*    and returns the self-test result and a text error message.             */
/*    Possible errors and timeout should be specified in instrument          */
/*    documentation.                                                         */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*      Instrument Handle returned from hpesa_init()                         */
/*    ViPInt16    selfTestResult (out)                                       */
/*      Self-test result                                                     */
/*    ViPString   selfTestMessage[] (out)                                    */
/*      Self-test status message string, limited to 256 characters.          */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_self_test
                        (ViSession    instrumentHandle,
                         ViPInt16    selfTestResult,
                         ViPString    selfTestMessage);


/*****************************************************************************/
/*  hpesa_error_query -                                                      */
/*    Queries the instrument for errors and reports the top error in the     */
/*    instrument's error message queue.  This function will get the error    */
/*    number and the detailed error message from the instrument.             */
/*    Documentation should specify whether the instrument returns text error */
/*    messages or only error numbers.                                        */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession    instrumentHandle (in)                                     */
/*      Instrument Handle returned from hpesa_init()                         */
/*    ViPInt32     errorCode (out)                                           */
/*      An instrument error code                                             */
/*    ViPString    errorMessage[] (out)                                      */
/*      Instrument error message string, limited to 256 characters.          */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_error_query
                        (ViSession    instrumentHandle,
                         ViPInt32    errorCode,
                         ViPString    errorMessage);



/*****************************************************************************/
/*  hpesa_error_message -                                                    */
/*    Returns a text string for an error number.  Developer is responsible   */
/*    for providing a text string in this routing for any and all errors     */
/*    that are not returned by VISA (i.e. driver-generated).  This function  */
/*    does not communicate with the instrument.                              */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*      Instrument Handle returned from hpesa_init()                         */
/*      May be VI_NULL for this function                                     */
/*    ViStatus    statusCode (in)                                            */
/*      The error return value from an instrument driver function            */
/*    ViPString   message[] (out)                                            */
/*      Error message string.  This is limited to 256 characters.            */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_error_message
                        (ViSession    instrumentHandle,
                         ViStatus    statusCode,
                         ViPString    message);



/*****************************************************************************/
/*  hpesa_revision_query -                                                   */
/*    Queries the instrument and driver for the current revision number.     */
/*    The instrument's firmware revision is obtained from the response to    */
/*    the "*IDN?" command.  The driver revision is hardcoded in driver.      */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession    instrumentHandle (in)                                     */
/*      Instrument Handle returned from hpesa_init()                         */
/*    ViPString    instrumentDriverRevision[] (out)                          */
/*      Instrument driver revision.  This is limited to 256 characters.      */
/*    ViPString    firmwareRevision[] (out)                                  */
/*      Instrument firmware revision.  This is limited to 256 characters.    */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_revision_query
                        (ViSession    instrumentHandle,
                         ViPString    instrumentDriverRevision,
                         ViPString    firmwareRevision);


        /***************************************************/
        /*  Agilent standard utility functions             */
        /***************************************************/

/*****************************************************************************/
/*  hpesa_timeOut -                                                          */
/*    Sets the amount of time a driver should wait on I/O in milliseconds,   */
/*    before returning a timeout error.  Default timeout is driver dependent */
/*    but should not be set to a value that would cause a timeout during     */
/*    normal operation.  The driver is allowed to override the default and   */
/*    and set timeout in certain long routines (e.g. self-test, long         */
/*    measurements).  It may be helpful to provide a separate timeout call   */
/*    for these functions.  A function overriding the default timeout must   */
/*    restore it before completing.                                          */
/*                                                                           */
/*    The default timeout for HP VISA is 2 seconds.  This routine is driver  */
/*    internal.                                                              */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession  instrumentHandle (in)                                       */
/*      Instrument Handle returned from hpesa_init()                         */
/*    ViInt32    setTimeOut (in)                                             */
/*      The period of time which VISA will allow an instrument command to    */
/*      wait before stopping it's execution.                                 */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_timeOut
                        (ViSession    instrumentHandle, 
                         ViInt32    setTimeOut);


/*****************************************************************************/
/*  hpesa_timeOut_Q -                                                        */
/*    Returns the current setting of the timeout value of the instrument in  */
/*    milliseconds.                                                          */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession  instrumentHandle (in)                                       */
/*      Instrument Handle returned from hpesa_init()                         */
/*    ViPInt32   timeOut (out)                                               */
/*      The period of time which VISA will allow an instrument command to    */
/*      wait before stopping it's execution.                                 */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_timeOut_Q
                        (ViSession  instrumentHandle,
                         ViPInt32    timeOut);


/*****************************************************************************/
/*  hpesa_errorQueryDetect -                                                 */
/*    This function sets a flag, detectError, which, if true, instructs a    */
/*    centralized error handling routine in the driver to a non-destructive  */
/*    query the instrument for the presence of errors.  The method of        */
/*    querying the instrument is somewhat instrument dependent.  In          */
/*    IEEE 488.2 compliant instruments, this is accomplished by querying     */
/*    the instrument's event status register and examining the four bytes    */
/*    relating to instrument errors.                                         */
/*                                                                           */
/*    This function is driver internal and only sets the flag.  No commands  */
/*    are sent to the instrument.                                            */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession  instrumentHandle (in)                                       */
/*      Instrument Handle returned from hpesa_init()                         */
/*    ViBoolean  setErrorQueryDetect (in)                                    */
/*      Enables (VI_TRUE) or disables (VI_FALSE) automatic instrument error  */
/*      querying                                                             */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_errorQueryDetect
                        (ViSession    instrumentHandle,
                         ViBoolean    setErrorQueryDetect);


/*****************************************************************************/
/*  hpesa_errorQueryDetect_Q -                                               */
/*    Queries the error detection flag.  Again, this command is driver       */
/*    and does not interact with the instrument. See hpesa_errorQueryDetect  */
/*    function for more information.                                         */
/*                                                                           */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*      Instrument Handle returned from hpesa_init()                         */
/*    ViPBoolean  errorQueryDetect (out)                                     */
/*      Equals VI_TRUE if automatic instrument error querying is being       */
/*      being performed.                                                     */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_errorQueryDetect_Q
                        (ViSession    instrumentHandle,
                         ViPBoolean errorQueryDetect);


/*****************************************************************************/
/*  hpesa_dcl -                                                              */
/*    Sends a device clear message to the instrument which, in turn, clears  */
/*    all input and output buffers, clears any pending operations, and       */
/*    the instrument parser into a state where it is ready to accept input.  */
/*    This routine does not generate any unique error conditions beyond      */
/*    those returned by VISA.                                                */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession instrumentHandle (in)                                        */
/*      Instrument Handle returned from hpesa_init()                         */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_dcl
                        (ViSession    instrumentHandle);


/*****************************************************************************/
/*  hpesa_opc_Q -                                                            */
/*    Sends an *OPC? command to the instrument and returns VI_TRUE when all  */
/*    pending operations are complete.                                       */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*      Instrument handle returned from hpesa_init()                         */
/*    ViPBoolean  instrumentReady (out)                                      */
/*      Returns VI_TRUE when pending operations are complete.                */
/*---------------------------------------------------------------------------*/
 
ViStatus _VI_FUNC hpesa_opc_Q
                        (ViSession    instrumentHandle,
                         ViPBoolean    instrumentReady);


ViStatus _VI_FUNC hpesa_wait_opc_timeout
                        (ViSession    instrumentHandle,
							int	iTimeout );
						
        /***************************************************/
        /*  Agilent standard status functions              */
        /***************************************************/

/*****************************************************************************/
/*  hpesa_readStatusByte_Q -                                                 */
/*    Returns the instrument's primary status byte.                          */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession instrumentHandle (in)                                        */
/*      Instrument Handle returned from hpesa_init()                         */
/*    ViPInt16  statusByte (out)                                             */
/*      Returns the contents of the status byte                              */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_readStatusByte_Q
                        (ViSession    instrumentHandle, 
                         ViPInt16    statusByte);


/*****************************************************************************/
/*  hpesa_operEvent_Q                                                        */
/*    Returns the contents of the instrument's operation event status        */
/*    register.  This routine is only required for IEEE 488.2 instruments.   */
/*    Note that if the user makes this call, automatic error detection may   */
/*    miss an instrument error or other event that is automatically handled  */
/*    in the driver.  Some instruments use non-standard forms of the SCPI    */
/*    command which look similar to the standard form.                       */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession vi (in)                                                      */
/*      Instrument Handle returned from hpesa_init()                         */
/*    ViPInt32  operationEventRegister (out)                                 */
/*      Returns the contents of the operation event register                 */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_operEvent_Q
                        (ViSession instrumentHandle, 
                         ViPInt32  operationEventRegister);


/*****************************************************************************/
/*  hpesa_operCond_Q                                                         */
/*    Returns the contents of the instrument's operation condition register. */
/*    This function is only required for IEEE 488.2 instruments.  Some       */
/*    instruments use non-standard forms of the SCPI command which look      */
/*    similar to the standard form.                                          */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession instrumentHandle (in)                                        */
/*      Instrument Handle returned from hpesa_init()                         */
/*    ViPInt32  operationConditionRegister (out)                             */
/*      Returns the contents of the operation condition register             */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_operCond_Q
                        (ViSession instrumentHandle, 
                         ViPInt32  operationConditionRegister);


/*****************************************************************************/
/*  hpesa_quesEvent_Q -                                                      */
/*    Returns the contents of instrument's data questionable status register.*/
/*    This function is only required for IEEE 488.2 instruments.  Some       */
/*    instruments use non-standard forms of the SCPI command which look      */
/*    similar to the standard form.                                          */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession instrumentHandle (in)                                        */
/*      Instrument Handle returned from hpesa_init()                         */
/*    ViPInt32  questionableEventRegister (out)                              */
/*      Returns the contents of the questionable event register              */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_quesEvent_Q
                        (ViSession instrumentHandle,
                         ViPInt32  questionableEventRegister);


/*****************************************************************************/
/*  hpesa_quesCond_Q -                                                       */
/*    Returns the contents of the instrument's data questionable condition   */
/*    register.  This routine is only required for IEEE 488.2 instruments.   */
/*    Some instruments use non-standard forms of the SCPI command which look */
/*    similar to the standard form.                                          */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession instrumentHandle (in)                                        */
/*      Instrument Handle returned from hpesa_init()                         */
/*    ViPInt32  questionableConditionRegister (out)                          */
/*      Returns the contents of the questionable condition register          */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_quesCond_Q
                        (ViSession instrumentHandle, 
                         ViPInt32  questionableConditionRegister);

        /***************************************************/
        /*  Agilent standard command passthrough functions */
        /***************************************************/

/*****************************************************************************/
/*  hpesa_cmd -                                                              */
/*    Send a arbitrary command string to the instrument, but does not look   */
/*    for a response.                                                        */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession     instrumentHandle (in)                                    */
/*      Instrument handle returned from hpesa_init()                         */
/*    ViString      sendStringCommand (in)                                   */
/*      The SCPI command string to be sent to the instrument                 */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_cmd
                        (ViSession instrumentHandle, 
                         ViString  sendStringCommand);


/*****************************************************************************/
/*  hpesa_cmdString_Q -                                                      */
/*    Sends an arbitrary command string to the instrument and returns a      */
/*    single string parameter.  The returned string will be null terminated  */
/*    and return only 256 characters.                                        */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession       instrumentHandle (in)                                  */
/*      Instrument handle returned from hpesa_init()                         */
/*    ViString        queryStringCommand (in)                                */
/*      The SCPI command string to be sent to the instrument                 */
/*    ViInt32         stringSize (in)                                        */
/*      The size of the char array (result) passed to the function to hold   */
/*      the string returned by the instrument                                */
/*    ViChar _VI_FAR  stringResult[] (out)                                   */
/*      The string returned by the instrument                                */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_cmdString_Q
                        (ViSession       instrumentHandle,
                         ViString        queryStringCommand,
                         ViInt32         stringSize,
                         ViChar _VI_FAR  stringResult[] );


/*****************************************************************************/
/*  hpesa_cmdInt -                                                           */
/*    Sends an arbitrary command string followed by a single integer         */
/*    parameter to the instrument.  A white space character is sent between  */
/*    the string and the integer.                                            */ 
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession  instrumentHandle (in)                                       */
/*      Instrument handle returned from hpesa_init()                         */
/*    ViString   sendIntegerCommand (in)                                     */
/*      The instrument command string to be sent to the instrument.          */
/*    ViPInt32   sendInteger (in)                                            */
/*      The integer sent to the instrument at the end of the instrument      */
/*      command.  Can be ViInt16 or ViInt32.                                 */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_cmdInt
                        (ViSession  instrumentHandle,
                         ViString   sendIntegerCommand,
                         ViInt32    sendInteger );


/*****************************************************************************/
/*  hpesa_cmdInt16_Q -                                                       */
/*    Sends SCPI command and waits for a response that must be representable */
/*    as an int16.  A non-numeric instrument response returns zero instead.  */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession  instrumentHandle (in)                                       */
/*      Instrument handle returned from hpesa_init()                         */
/*    ViString   queryI16Command (in)                                        */
/*      The SCPI command string to be sent to the instrument.                */
/*    ViPInt16   i16Result (out)                                             */
/*      The integer returned from the instrument.                            */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_cmdInt16_Q
                        (ViSession  instrumentHandle, 
                         ViString   queryI16Command,
                         ViPInt16   i16Result);


/*****************************************************************************/
/*  hpesa_cmdInt32_Q -                                                       */
/*    Sends SCPI command and waits for a response that must be representable */
/*    as an int32.  A non numeric instrument response returns zero instead.  */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession  instrumentHandle (in)                                       */
/*      Instrument handle returned from hpesa_init()                         */
/*    ViString   queryI32Command (in)                                        */
/*      The SCPI command string to be sent to the instrument.                */
/*    ViPInt32   i32Result (out)                                             */
/*      The integer returned from the instrument.                            */
/*---------------------------------------------------------------------------*/


ViStatus _VI_FUNC hpesa_cmdInt32_Q
                        (ViSession instrumentHandle,
                         ViString  queryI32Command, 
                         ViPInt32  i32Result);


/*****************************************************************************/
/*  hpesa_cmdInt16Arr_Q -                                                    */
/*    This function is part of the standard Utility package, but has not     */
/*    been verified for the basic driver. They may be restored when          */
/*    additional tests are available to verify their operation.              */
/*---------------------------------------------------------------------------*/
    
/* ViStatus _VI_FUNC hpesa_cmdInt16Arr_Q
                        (ViSession instrumentHandle,
                         ViString  queryI16ArrayCommand,
                         ViInt32   i16ArraySize,
                         ViInt16   _VI_FAR i16ArrayResult[],
                         ViPInt32  i16ArrayCount);


                                                                        

/*****************************************************************************/
/*  hpesa_cmdInt32Arr_Q -                                                    */
/*    This function is part of the standard Utility package, but has not     */
/*    been verified for the basic driver. They may be restored when          */
/*    additional tests are available to verify their operation.              */
/*---------------------------------------------------------------------------*/

/* ViStatus _VI_FUNC hpesa_cmdInt32Arr_Q
                        (ViSession instrumentHandle,
                         ViString  queryI32ArrayCommand,
                         ViInt32   i32ArraySize,
                         ViInt32   _VI_FAR i32ArrayResult[],
                         ViPInt32  i32ArrayCount);


/*****************************************************************************/
/*  hpesa_cmdReal -                                                          */
/*    Sends an instrument command followed by a single floating point        */
/*    parameter.  A white space character is sent between the string and the */
/*    parameter.                                                             */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession  instrumentHandle (in)                                       */
/*      Instrument handle returned from hpesa_init()                         */
/*    ViString   sendRealCommand (in)                                        */
/*      The instrument command string to be sent to the instrument.          */
/*    ViReal64   sendReal (in)                                               */
/*      The real number sent to the instrument at the end of the instrument  */
/*      command.  Can be ViReal32 or ViReal64.                               */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_cmdReal
                        (ViSession  instrumentHandle,
                         ViString   sendRealCommand,
                         ViReal64   sendReal );


/*****************************************************************************/
/*  hpesa_cmdReal64_Q -                                                      */
/*    Sends SCPI command and waits for a response that must be representable */
/*    as an real64.  A non-numeric instrument response returns zero instead, */
/*    in case of underflow undefined.                                        */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession  instrumentHandle (in)                                       */
/*      Instrument handle returned from hpesa_init()                         */
/*    ViString   queryRealCommand (in)                                       */
/*      The SCPI command string to be sent to the instrument.                */
/*    ViPReal64  realResult (out)                                            */
/*      The real returned from the instrument.                               */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_cmdReal64_Q
                        (ViSession  instrumentHandle,
                         ViString   queryRealCommand, 
                         ViPReal64  realResult);


/*****************************************************************************/
/*  hpesa_cmdReal64Arr_Q -                                                   */
/*    This entry point assumes that the return result is a arbitrary block.  */
/*    Do not use this entry point for ASCII values.  For that just get the   */
/*    data from _cmd_string_Q routine.                                       */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession  instrumentHandle (in)                                       */
/*      Instrument handle returned from hpesa_init()                         */
/*    ViString   realArrayCommand (in)                                       */
/*      The instrument command string to be sent to the instrument.          */
/*    ViInt32    realArraySize (in)                                          */
/*      The number of elements allocated to the results array.               */
/*    ViReal64   _VI_FAR realArrayResult[] (out)                             */
/*      Array of 64-bit reals - result from instrument.                      */
/*    ViPInt32   realArrayCount (out)                                        */
/*      The actual number of elements returned by the instrument             */
/*                                                                           */
/*  The function assumes that the data returned by the instrument has        */
/*    four components.                                                       */
/*    1 - An initial "#" taking one byte                                     */
/*    2 - A single ASCII digit indicating the size in bytes of a number      */
/*        which follows immediately.                                         */
/*    3 - A string number indicating the number of array elements to be      */
/*        returned by the instrument.                                        */
/*    4 - The arbitrary block being returned by the instrument.  In this     */
/*        case it is an array of 64-bit real numbers.                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_cmdReal64Arr_Q
                        (ViSession  instrumentHandle,
                         ViString   realArrayCommand, 
                         ViInt32    realArraySize,
                         ViReal64   _VI_FAR realArrayResult[],
                         ViPInt32   realArrayCount);


/* End of Agilent standard declarations */


/*****************************************************************************/
/*  INSTRUMENT SPECIFIC SECTION                                              */
/*  Constants and function prototypes for instrument specific functions      */
/*****************************************************************************/

        /***************************************************/
        /*  Instrument specific constants                  */
        /***************************************************/

/* Enumerated Types for Basic Spectrum Analyzer */

/*----> Frequency Types */

/* Direction Types  */
#define hpesa_STEP_UP              0
#define hpesa_STEP_DOWN            1
#define hpesa_STEP_LASTENUM        2

/* Span Types */
#define hpesa_SPANFULL            0
#define hpesa_SPANLAST            1
#define hpesa_SPANZERO            2
#define hpesa_SPAN_LASTENUM       3


/*----> Amplitude Types */

/*  Format Types */
#define hpesa_FORMAT_LOGARITHMIC     0
#define hpesa_FORMAT_LINEAR          1
#define hpesa_FORMAT_LASTENUM        2


/* Unit Types */
#define hpesa_UNITS_DBM             0
#define hpesa_UNITS_DBMV            1
#define hpesa_UNITS_DBUV            2
#define hpesa_UNITS_VOLTS           3
#define hpesa_UNITS_WATTS           4
#define hpesa_UNITS_LASTENUM        5


/*-----> Sweep/Trigger Types */

/* Sweep Trigger Mode Types */
#define hpesa_SWEEP_TRIGGER_SINGLE        0
#define hpesa_SWEEP_TRIGGER_CONTINUOUS    1
#define hpesa_SWEEP_TRIGGER_LASTENUM      2


/* Trigger Source Types */
#define hpesa_TRIGGER_IMMEDIATE           0
#define hpesa_TRIGGER_VIDEO               1
#define hpesa_TRIGGER_LINE                2
#define hpesa_TRIGGER_EXTERNAL            3
#define hpesa_TRIGGER_LASTENUM            4


/*----> Average Types */
#define hpesa_AVERAGE_LPOWER              0
#define hpesa_AVERAGE_POWER               1
#define hpesa_AVERAGE_TYPE_LASTENUM       2


/*----> Trace Types */

/* Trace Mode Types */
#define hpesa_TRACE_WRITE                 0
#define hpesa_TRACE_MAXHOLD               1
#define hpesa_TRACE_MINHOLD               2
#define hpesa_TRACE_VIEW                  3
#define hpesa_TRACE_BLANK                 4
#define hpesa_TRACE_MODELASTENUM          5

/* Trace Type */
#define hpesa_TRACE_1                    0
#define hpesa_TRACE_2                    1
#define hpesa_TRACE_3                    2
#define hpesa_TRACE_LASTENUM             3


/* -----> Marker Types */

/* Marker Number Types */
#define hpesa_MARKER_1                    0
#define hpesa_MARKER_2                    1
#define hpesa_MARKER_3                    2
#define hpesa_MARKER_4                    3
#define hpesa_MARKER_NUM_LASTENUM         4


/* Marker Mode Types */
#define hpesa_MARKER_NORMAL                0
#define hpesa_MARKER_DELTA                 1
#define hpesa_MARKER_BAND                  2
#define hpesa_MARKER_SPAN                  3
#define hpesa_MARKER_MODEOFF               4
#define hpesa_MARKER_MODE_LASTENUM         5


/*  Marker Peak Types */
#define hpesa_MARKER_PEAK                0
#define hpesa_MARKER_PEAKNEXT            1
#define hpesa_MARKER_PEAKLEFT            2
#define hpesa_MARKER_PEAKRIGHT           3
#define hpesa_MARKER_MIN                 4
#define hpesa_MARKER_MINNEXT             5
#define hpesa_MARKER_MINLEFT             6
#define hpesa_MARKER_MINRIGHT            7
#define hpesa_MARKER_PEAK_LASTENUM       8

/* Marker Move Types */
#define hpesa_MARKER_CENTERFREQ          0
#define hpesa_MARKER_CENTERFREQSTEP      1
#define hpesa_MARKER_SPANFREQ            2
#define hpesa_MARKER_STARTFREQ           3
#define hpesa_MARKER_STOPFREQ            4
#define hpesa_MARKER_REFLEVEL            5
#define hpesa_MARKER_MOVE_LASTENUM       6

/* Marker Function Types */
#define hpesa_MARKER_BPOWER               0
#define hpesa_MARKER_NOISE                1
#define hpesa_MARKER_FUNCOFF              2
#define hpesa_MARKER_FUNC_LASTENUM        3

/*-----> Trace Math Types */

#define hpesa_PEAKSORT_AMPL             0
#define hpesa_PEAKSORT_FREQ             1
#define hpesa_PEAKSORT_LASTENUM         2


/*-----> Input Coupling Types */

/* Input Couple Mode Types  */
#define hpesa_INPUT_COUPLE_AC            0
#define hpesa_INPUT_COUPLE_DC            1
#define hpesa_INPUT_COUPLE_LASTENUM      2


/* Enumerated Types for ESA Specific
   Detector Types */

/* Detector Mode Types */
#define hpesa_DETECTOR_POSITIVEPEAK        0
#define hpesa_DETECTOR_SAMPLE              1
#define hpesa_DETECTOR_NEGATIVEPEAK        2
#define hpesa_DETECTOR_LASTENUM            3


/* Input Impedance Type */

#define hpesa_INPUT_IMPEDANCE_50        0
#define hpesa_INPUT_IMPEDANCE_75        1
#define hpesa_INPUT_IMPEDANCE_LASTENUM  2


/*-----> Sweep Gate Types */

/*  Sweep Gate Mode */
#define hpesa_GATESWEEPMODE_GATEDVIDEO    0
#define hpesa_GATESWEEPMODE_TRIGDELAY     1
#define hpesa_GATESWEEPMODE_LASTENUM      2


/*  Sweep Gate Type  */
#define hpesa_GATESWEEPTYPE_LEVEL        0
#define hpesa_GATESWEEPTYPE_EDGE         1
#define hpesa_GATESWEEPTYPE_LASTENUM     2


/*  Sweep Gate Polarity Type  */
#define hpesa_GATESWEEPPOL_POSITIVE    0
#define hpesa_GATESWEEPPOL_NEGATIVE    1
#define hpesa_GATESWEEPPOL_LASTENUM    2

/* Sweep Gate Level  */
#define hpesa_GATESWEEPLEVEL_HIGH            0
#define hpesa_GATESWEEPLEVEL_LOW             1
#define hpesa_GATESWEEPLEVEL_LASTENUM        2

/*-----> Alignment Types */

/*    Align Auto Types  */
#define hpesa_ALIGN_AUTO_ALL                0
#define hpesa_ALIGN_AUTO_NRF                1
#define hpesa_ALIGN_AUTO_OFF                2
#define hpesa_ALIGN_AUTO_LASTENUM           3
 
/*  Align RF Source Types  */
#define hpesa_ALIGNRFSOURCE_INTERNAL        0
#define hpesa_ALIGNRFSOURCE_EXTERNAL        1
#define hpesa_ALIGNRFSOURCE_LASTENUM        2

/* Align Types  */
#define hpesa_ALIGN_ALL                       0
#define hpesa_ALIGN_RF                        1
#define hpesa_ALIGN_TG                        2
#define hpesa_ALIGN_FMD                       3
#define hpesa_ALIGN_LASTENUM                  4

/*-----> Data Input/Output Formatting Types */

/* Data Format Types */
#define hpesa_DATA_FORMAT_ASCII             0
#define hpesa_DATA_FORMAT_CSV               1
#define hpesa_DATA_FORMAT_INT32             2
#define hpesa_DATA_FORMAT_REAL32            3
#define hpesa_DATA_FORMAT_REAL64            4

/* Byte Order Types */
#define hpesa_BYTE_ORDER_NORMAL              0
#define hpesa_BYTE_ORDER_SWAP                1

/*-----> Measure Types */

/* Measure Mode Types  */
#define hpesa_MEASURE_MODE_OFF       0
#define hpesa_MEASURE_MODE_ADJACENT  1
#define hpesa_MEASURE_MODE_CHANNEL   2
#define hpesa_MEASURE_MODE_OCCUPIED  3
#define hpesa_MEASURE_MODE_EMISSION  4
#define hpesa_MEASURE_MODE_HARMONIC  5
#define hpesa_MEASURE_MODE_LASTENUM  6

/* Measure Method Types */
#define hpesa_MEASURE_METHOD_MEAS     0
#define hpesa_MEASURE_METHOD_READ     1
#define hpesa_MEASURE_METHOD_FETCH    2
#define hpesa_MEASURE_METHOD_LASTENUM 3

/* Alternative measure methods */
#define hpesa_MEASURE     0
#define hpesa_READ        1
#define hpesa_FETCH       2

/*  Harmonic Sweep Types */
#define hpesa_HARMONIC_SWEEP_MANUAL   0
#define hpesa_HARMONIC_SWEEP_AUTO     1
#define hpesa_HARMONIC_SWEEP_LASTENUM 2


/*------> Verification Settings */
#define hpesa_VERIFY_APPLICATION 0
#define hpesa_VERIFY_LASTENUM 1

/*------> Instrument Applications */
#define hpesa_APPLICATION_SA 0
#define hpesa_APPLICATION_CDMA 1
#define hpesa_APPLICATION_GSM 2
#define hpesa_APPLICATION_LASTENUM 3

/*------> Instrument Application Modes */
#define hpesa_APPLICATION_MODE_CHP 0
#define hpesa_APPLICATION_MODE_RHO 1
#define hpesa_APPLICATION_MODE_CDP 2
#define hpesa_APPLICATION_MODE_CSP 3
#define hpesa_APPLICATION_MODE_SHP 4
#define hpesa_APPLICATION_MODE_TXP 5
#define hpesa_APPLICATION_MODE_PVT 6
#define hpesa_APPLICATION_MODE_PFER 7
#define hpesa_APPLICATION_MODE_ORFS 8
#define hpesa_APPLICATION_MODE_TSP  9
#define hpesa_APPLICATION_MODE_LASTENUM 10

/*------> Instrument Average Modes */
#define hpesa_AVERAGE_EXPONENTIAL 0
#define hpesa_AVERAGE_REPEAT 1
#define hpesa_AVERAGE_MODE_LASTENUM 2

/*------> Trigger Slope Types*/
#define hpesa_TRIG_SLOPE_POS 0
#define hpesa_TRIG_SLOPE_NEG 1
#define hpesa_TRIG_SLOPE_LASTENUM 2

/*------> Measurement results types */
#define hpesa_RESULTS_TYPE_0 0
#define hpesa_RESULTS_TYPE_1 1
#define hpesa_RESULTS_TYPE_2 2
#define hpesa_RESULTS_TYPE_3 3
#define hpesa_RESULTS_TYPE_4 4
#define hpesa_RESULTS_TYPE_5 5
#define hpesa_RESULTS_TYPE_6 6
#define hpesa_RESULTS_TYPE_LASTENUM 7

/*------> CHP Trigger Types */
#define hpesa_CHP_TRIG_IMMEDIATE 0
#define hpesa_CHP_TRIG_EXTERNAL  1
#define hpesa_CHP_TRIG_LASTENUM  2

/*------> RHO Meas Spec Types */
#define hpesa_RHO_MEAS_SPEC_NORMAL   0
#define hpesa_RHO_MEAS_SPEC_INVERTED 1
#define hpesa_RHO_MEAS_SPEC_LASTENUM 2

/*------> CDP Meas Spec Types */
#define hpesa_CDP_MEAS_SPEC_NORMAL   0
#define hpesa_CDP_MEAS_SPEC_INVERTED 1
#define hpesa_CDP_MEAS_SPEC_LASTENUM 2

/*------> CDP Meas METHOD Types */
#define hpesa_CDP_MEAS_METHOD_POWER    0
#define hpesa_CDP_MEAS_METHOD_TPHASE   1
#define hpesa_CDP_MEAS_METHOD_LASTENUM 2

/*------> CSP Measurement Types */
#define hpesa_CSP_MEAS_TYPE_EXAMINE 0
#define hpesa_CSP_MEAS_TYPE_FULL    1
#define hpesa_CSP_MEAS_TYPE_LASTENUM 2

/*------> CSP Measure Segment Types */
#define hpesa_CSP_SEGMENT_LOWER 0
#define hpesa_CSP_SEGMENT_CENTER 1
#define hpesa_CSP_SEGMENT_UPPER 2
#define hpesa_CSP_SEGMENT_LASTENUM 3

/*------> CDMA1 Radio Standard Band Types */
#define hpesa_CDMA1_RADIO_BAND_IS95A   0
#define hpesa_CDMA1_RADIO_BAND_JSTD8   1
#define hpesa_CDMA1_RADIO_BAND_C95B    2
#define hpesa_CDMA1_RADIO_BAND_P95B    3
#define hpesa_CDMA1_RADIO_BAND_CKOR    4
#define hpesa_CDMA1_RADIO_BAND_PKOR    5
#define hpesa_CDMA1_RADIO_BAND_P95C    6
#define hpesa_CDMA1_RADIO_BAND_C95C    7
#define hpesa_CDMA1_RADIO_BAND_ARIBT53 8
#define hpesa_CDMA1_RADIO_BAND_LASTENUM 9

/*------> Radio device Types  */
#define hpesa_CDMA1_RADIO_DEVICE_BASE   0
#define hpesa_CDMA1_RADIO_DEVICE_MOBILE 1
#define hpesa_CDMA1_RADIO_DEVICE_LASTENUM 2

/*------> Demod Time Reference Types  */
#define hpesa_DEMOD_TIME_REF_ESEC 0
#define hpesa_DEMOD_TIME_REF_OFF  1
#define hpesa_DEMOD_TIME_REF_LASTENUM 2

/*------> Demod Trigger source Types  */
#define hpesa_DEMOD_TRIG_SOURCE_IMMEDIATE 0
#define hpesa_DEMOD_TRIG_SOURCE_RFBURST   1
#define hpesa_DEMOD_TRIG_SOURCE_EXTERNAL  2
#define hpesa_DEMOD_TRIG_SOURCE_FRAME     3
#define hpesa_DEMOD_TRIG_SOURCE_LASTENUM  4

/*------> GSM Radio Standard Band Types */
#define hpesa_GSM_RADIO_BAND_PGSM   0
#define hpesa_GSM_RADIO_BAND_EGSM   1
#define hpesa_GSM_RADIO_BAND_RGSM   2
#define hpesa_GSM_RADIO_BAND_DCS    3
#define hpesa_GSM_RADIO_BAND_PCS    4
#define hpesa_GSM_RADIO_BAND_LASTENUM 5


/*------> GSM Radio Standard Band Types  */
#define hpesa_GSM_RADIO_DEVICE_MS    0
#define hpesa_GSM_RADIO_DEVICE_BTS   1
#define hpesa_GSM_RADIO_DEVICE_UBTS1 2
#define hpesa_GSM_RADIO_DEVICE_UBTS2 3
#define hpesa_GSM_RADIO_DEVICE_UBTS3 4
#define hpesa_GSM_RADIO_DEVICE_LASTENUM 5

/*------> GSM Priority Band Types  */
#define hpesa_GSM_PRIORITY_BAND_DCS  0
#define hpesa_GSM_PRIORITY_BAND_PCS  1
#define hpesa_GSM_PRIORITY_BAND_LASTENUM 2

/*------> GSM Priority Band Types  */
#define hpesa_INPUT_CARRIER_SINGLE   0
#define hpesa_INPUT_CARRIER_MULTIPLE 1
#define hpesa_INPUT_CARRIER_LASTENUM 2

/*------> GSM Demod Burst Align Types  */
#define hpesa_GSM_BURST_ALIGN_GSM      0
#define hpesa_GSM_BURST_ALIGN_HALF     1
#define hpesa_GSM_BURST_ALIGN_LASTENUM 2

/*------> GSM OSC Source Types  */
#define hpesa_GSM_OSC_SOURCE_INTERNAL 0
#define hpesa_GSM_OSC_SOURCE_EXTERNAL 1
#define hpesa_GSM_OSC_SOURCE_LASTENUM 2

/*------> GSM TSC Auto Types  */
#define hpesa_TSC_MANUAL   0
#define hpesa_TSC_AUTO     1
#define hpesa_TSC_LASTENUM 2

/*------> GSM TXP Trigger Types  */
#define hpesa_TXP_TRIG_IMMEDIATE 0
#define hpesa_TXP_TRIG_RFBURST   1
#define hpesa_TXP_TRIG_EXTERNAL  2
#define hpesa_TXP_TRIG_FRAME     3
#define hpesa_TXP_TRIG_LASTENUM  4

/*------> GSM TXP Threshold Types  */
#define hpesa_TXP_THRESHOLD_ABSOLUTE 0
#define hpesa_TXP_THRESHOLD_RELATIVE 1
#define hpesa_TXP_THRESHOLD_LASTENUM 2

/*------>  Burst Sync Types  */
#define hpesa_BURST_SYNC_TSEQ     0
#define hpesa_BURST_SYNC_RFAMP    1
#define hpesa_BURST_SYNC_NONE     2 
#define hpesa_BURST_SYNC_LASTENUM 3

/*------>  PVT Trig Source Types  */
#define hpesa_PVT_TRIG_EXTERNAL 0
#define hpesa_PVT_TRIG_RFBURST  1
#define hpesa_PVT_TRIG_FRAME    2
#define hpesa_PVT_TRIG_LASTENUM 3

/*------>  PVT Mask Custom Types  */
#define hpesa_PVT_MASK_STANDARD 0
#define hpesa_PVT_MASK_CUSTOM   1
#define hpesa_PVT_MASK_LASTENUM 2

/*------>  Average Types  */
#define hpesa_AVERAGE_MEAN     0
#define hpesa_AVERAGE_MAX      1
#define hpesa_AVERAGE_LASTENUM 2

/*------> GSM PFER Trigger Types  */
#define hpesa_PFER_TRIG_IMMEDIATE 0
#define hpesa_PFER_TRIG_EXTERNAL  1
#define hpesa_PFER_TRIG_RFBURST   2
#define hpesa_PFER_TRIG_FRAME     3
#define hpesa_PFER_TRIG_LASTENUM  4

/*------> GSM ORFS Meas Mode Types  */
#define hpesa_ORFS_MEASURE_MULTIPLE 0
#define hpesa_ORFS_MEASURE_SINGLE   1
#define hpesa_ORFS_MEASURE_SWEPT    2
#define hpesa_ORFS_MEASURE_LASTENUM 3

/*------> GSM ORFS Meas Types  */
#define hpesa_ORFS_TYPE_MOD      0
#define hpesa_ORFS_TYPE_SWITCH   1
#define hpesa_ORFS_TYPE_LASTENUM 2

/*------> GSM ORFS Offset Types  */
#define hpesa_ORFS_OFFSET_STANDARD 0
#define hpesa_ORFS_OFFSET_SHORT    1
#define hpesa_ORFS_OFFSET_LASTENUM 2

/*------> GSM ORFS Trigger Types  */
#define hpesa_ORFS_TRIG_IMMEDIATE 0
#define hpesa_ORFS_TRIG_EXTERNAL  1
#define hpesa_ORFS_TRIG_RFBURST   2
#define hpesa_ORFS_TRIG_FRAME     3
#define hpesa_ORFS_TRIG_LASTENUM  4

/*------> TSP Measurement Types */
#define hpesa_TSP_MEAS_TYPE_EXAMINE  0
#define hpesa_TSP_MEAS_TYPE_FULL     1
#define hpesa_TSP_MEAS_TYPE_LASTENUM 2



    /***************************************************/
    /* Instrument specific functions                   */
    /***************************************************/

/*****************************************************************************/
/*  hpesa_cmdRealCommaArray_Q -                                              */
/*    This function retrieves an array of comma-separated values from the    */
/*    instrument.  The user may need to place the instrument in ASCII format */
/*    via the "FORM ASC" command.                                            */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession  instrumentHandle                                            */
/*      Instrument handle returned from hpesa_init()                         */
/*    ViString   realCommaCommand                                            */
/*      The command string to be sent to the instrument.                     */
/*    ViInt32    realCommaArraySize                                          */
/*      The number of elements allocated for realCommaArrayResults           */
/*    ViReal64   _VI_FAR realCommaArrayResult[]                              */
/*      Returns an array of 64-bit real numbers received from the instrument.*/
/*    ViPInt32   realCommaValuesRead                                         */
/*      The actual number of values returned in realCommaArrayResults.       */
/*                                                                           */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_cmdRealCommaArray_Q
                        (ViSession instrumentHandle,
                         ViString realCommaCommand,
                         ViInt32 realCommaArraySize,
                         ViReal64 _VI_FAR realCommaArrayResult[],
                         ViPInt32 realCommaValuesRead);

    /***************************************************/
    /*  - Basic Spectrum Analyzer Functions            */
    /***************************************************/

    /***************************************************/
    /* FREQUENCY CONTROL FUNCTIONS                     */
    /***************************************************/

/*****************************************************************************/
/*                                                                           */
/*  hpesa_frequencySetStart -                                                */
/*    This function sets the spectrum analyzers start frequency.             */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViReal64      startFrequency (in)                                      */
/*        - New start frequency in Hz                                        */
/*        - Range is checked by machine                                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_frequencySetStart
                        (ViSession instrumentHandle, 
                         ViReal64  startFrequency);

/*****************************************************************************/
/*                                                                           */
/*  hpesa_frequencyGetStart -                                                */
/*    Retrieves the spectrum analyzers start frequency.                      */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViPReal64     startFrequency_Q (out)                                   */
/*        - Returned start frequency in Hz                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_frequencyGetStart
                        (ViSession instrumentHandle, 
                         ViPReal64 startFrequency_Q);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_frequencySetStop -                                                 */
/*    This function sets the spectrum analyzers stop frequency.              */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViReal64      stopFrequency (in)                                       */
/*        - New stop frequency in Hz                                         */
/*        - Range is checked by machine                                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_frequencySetStop
                        (ViSession instrumentHandle, 
                         ViReal64 stopFrequency);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_frequencyGetStop -                                                 */
/*    Retrieves the spectrum analyzers stop frequency.                       */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViPReal64     stopFrequency_Q (out)                                    */
/*        - Returned stop frequency in Hz                                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_frequencyGetStop
                        (ViSession instrumentHandle, 
                         ViPReal64 stopFrequency_Q);

/*****************************************************************************/
/*                                                                           */
/*  hpesa_frequencySetCenter -                                               */
/*    This function sets the spectrum analyzers center frequency.            */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViReal64      centerFrequency (in)                                     */
/*        - New center frequency in Hz                                       */
/*        - Range is checked by machine                                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_frequencySetCenter
                        (ViSession instrumentHandle, 
                         ViReal64 centerFrequency);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_frequencyGetCenter -                                               */
/*    Retrieves the spectrum analyzers center frequency.                     */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViPReal64     centerFrequency_Q (out)                                  */
/*        - Returned center frequency in Hz                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_frequencyGetCenter
                        (ViSession instrumentHandle, 
                         ViPReal64 centerFrequency_Q);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_frequencySetSpan -                                                 */
/*    This function sets the spectrum analyzers span frequency.              */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViReal64      spanFrequency (in)                                       */
/*        - New span frequency in Hz                                         */
/*        - Range is checked by machine                                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_frequencySetSpan
                        (ViSession instrumentHandle, 
                         ViReal64 spanFrequency);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_frequencyGetSpan -                                                 */
/*    Retrieves the spectrum analyzers span frequency.                       */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViPReal64     spanFrequency_Q (out)                                    */
/*        - Returned span frequency in Hz                                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_frequencyGetSpan
                        (ViSession instrumentHandle, 
                         ViPReal64 spanFrequency_Q);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_frequencySetCenterStepSize-                                        */
/*    This function sets the spectrum analyzers center frequency step size.  */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViReal64      centerFrequencyStepSize (in)                             */
/*        - New center frequency step size in Hz                             */
/*        - Range is checked by machine                                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_frequencySetCenterStepSize
                        (ViSession instrumentHandle,
                         ViReal64 centerFrequencyStepSize);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_frequencyStepCenter -                                              */
/*    Steps the spectrum analyzer's center frequency.                        */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViInt16     centerFreqStepDir                                          */
/*        - Direction to step the center frequency.                          */
/*        - Range:  hpesa_STEP_UP through hpesa_STEP_LASTENUM                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_frequencyStepCenter
                        (ViSession instrumentHandle,
                         ViInt16 centerFreqStepDir);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_frequencySetSpanZoom -                                             */
/*    Steps the spectrum analyzer's center frequency.                        */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViInt16     spanZoomType                                               */
/*        - Executes one of three possible span changes                      */
/*        - Range:  hpesa_SPANFULL - sets frequency span to full             */
/*                  hpesa_SPANLAST - sets frequency span to previous span    */
/*                  hpesa_SPANZERO - sets frequency span to 0 Hz             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_frequencySetSpanZoom
                        (ViSession instrumentHandle,
                         ViInt16 spanZoomType);


    /***************************************************/
    /* AMPLITUDE CONTROL FUNCTIONS                     *
    /***************************************************/


/*****************************************************************************/
/*                                                                           */
/*  hpesa_amplitudeSetAttenuation() -                                        */
/*    This method sets the spectrum analyzer's RF Input Attenuator value     */
/*    Setting the amplitude attenuation with this function uncouples the     */
/*    attenuator from the amplitude reference level.  To couple the          */
/*    attenuation to the reference level, call the function                  */
/*    "hpesa_amplitudeSetAttenuationAuto".                                   */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViReal64    amplitudeAttenuation (in)                                  */
/*        - Instrument new amplitude attenuation in dB                       */
/*        - Range is checked by machine.                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_amplitudeSetAttenuation
                        (ViSession instrumentHandle, 
                         ViReal64 amplitudeAttenuation);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_amplitudeGetAttenuation() -                                        */
/*    This method gets the spectrum analyzer's RF Input Attenuator value.    */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViPReal64    amplitudeAttenuation_Q (out)                              */
/*        - Instrument's current amplitude attenuation in dB                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_amplitudeGetAttenuation
                        (ViSession instrumentHandle, 
                         ViPReal64 amplitudeAttenuation_Q);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_amplitudeSetAttenuationAuto() -                                    */
/*    This method sets the spectrum analyzer's RF Input Attenuator value to  */
/*    be set automatically by the instrument as determined by the amplitude  */
/*    reference level.                                                       */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_amplitudeSetAttenuationAuto
                        (ViSession instrumentHandle);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_amplitudeSetFormat() -                                             */
/*     This method sets the spectrum analyzer's display vertical scale       */
/*     format, as either logarithmic or linear.                              */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViInt16     amplitudeFormat (in)                                       */
/*        - Instrument's new display vertical scale format.                  */
/*        - Range is enumerate, either hpesa_FORMAT_LOGARITHMIC or           */
/*          hpesa_FORMAT_LINEAR                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_amplitudeSetFormat
                        (ViSession instrumentHandle, 
                         ViInt16 amplitudeFormat);

/*****************************************************************************/
/*                                                                           */
/*  hpesa_amplitudeGetFormat() -                                             */
/*     This method gets the spectrum analyzer's amplitude format.            */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViPInt16     amplitudeFormat_Q (out)                                   */
/*        - Instrument's current amplitude format, either logarithmic or     */
/*          linear.  It should only return two possible values,              */
/*          hpesa_FORMAT_LOGARITHMIC or hpesa_FORMAT_LINEAR.                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_amplitudeGetFormat
                        (ViSession instrumentHandle, 
                         ViPInt16 amplitudeFormat_Q);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_amplitudeSetReferenceLevel() -                                     */
/*     This method sets the spectrum analyzer's amplitude reference level.   */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViReal64     amplitudeRefLevel (in)                                    */
/*        - Instrument's new amplitude reference level in current amplitude  */
/*          display units.                                                   */
/*        - Range is checked by the machine.                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_amplitudeSetReferenceLevel
                        (ViSession instrumentHandle, 
                         ViReal64 amplitudeRefLevel);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_amplitudeGetReferenceLevel() -                                     */
/*     This method retrieves the spectrum analyzer's amplitude reference     */
/*     level.                                                                */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViPReal64     amplitudeRefLevel_Q (out)                                */
/*        - Instrument's amplitude reference level in current amplitude      */
/*          display units.                                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_amplitudeGetReferenceLevel
                        (ViSession instrumentHandle, 
                         ViPReal64 amplitudeRefLevel_Q);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_amplitudeSetRefLevelOffset() -                                     */
/*     This method sets the spectrum analyzer's amplitude reference level    */
/*     offset.                                                               */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViReal64     amplitudeRefLevelOffset (in)                              */
/*        - Instrument's new amplitude reference level offset in dB.         */
/*        - Range is checked by the machine.                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_amplitudeSetRefLevelOffset
                        (ViSession instrumentHandle,
                         ViReal64 amplitudeRefLevelOffset);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_amplitudeGetRefLevelOffset() -                                     */
/*     This method retrieves the spectrum analyzer's amplitude offset.       */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViPReal64     amplitudeRefLevelOffset_Q (out)                          */
/*        - Instrument's amplitude reference level in current amplitude      */
/*          display units.                                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_amplitudeGetRefLevelOffset
                        (ViSession instrumentHandle, 
                         ViPReal64 amplitudeRefLevelOffset_Q);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_amplitudeSetScale() -                                              */
/*     This method sets the spectrum analyzer's per division display scaling */
/*     for the y-axis (amplitude) logarithmic scale format (type).           */
/*     This setting doesn't affect the y-axis linear scale format, where the */
/*     per division display scaling is always the reference level divided    */
/*     10.                                                                   */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViReal64     amplitudeScale (in)                                       */
/*        - Instrument's new per division display scaling for y-axis         */
/*          logarithmic scale.  Units are in dB.                             */
/*          Log amplitude per division range is:                             */
/*          0.1 dB for steps 0.1 to 0.9                                      */
/*          1.0 dB for steps 1.0 to 20.0                                     */
/*        - Range is checked by the machine.                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_amplitudeSetScale
                        (ViSession instrumentHandle,
                         ViReal64 amplitudeScale);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_amplitudeGetScale() -                                              */
/*     This method retrieves the spectrum analyzer's per division display    */
/*     scaling for the y-axis (amplitude.  This setting is only applicable   */
/*     to y-axis (amplitude) logarithmic scale format.  This setting has no  */
/*     meaning for the linear scale format.                                  */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViPReal64     amplitudeScale_Q (out)                                   */
/*        Instruments current per division display scaling for y-axis in dB  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_amplitudeGetScale
                        (ViSession instrumentHandle,
                         ViPReal64 amplitudeScale_Q);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_amplitudeSetUnits() -                                              */
/*     This method sets the spectrum analyzer's units for the input, output  */
/*     and display for the active window.                                    */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViInt16      amplitudeUnits (in)                                       */
/*        - Instrument's new amplitude units.                                */
/*        - Enumerated:     dBm  -  hpesa_UNITS_DBM                          */
/*                          dBmV -  hpesa_UNITS_DBMV                         */
/*                          dBuV -  hpesa_UNITS_DBUV                         */
/*                          Volts - hpesa_UNITS_VOLTS                        */
/*                          Watts - hpesa_UNITS_WATTS                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_amplitudeSetUnits
                        (ViSession instrumentHandle,
                         ViInt16 amplitudeUnits);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_amplitudeGetUnits() -                                              */
/*     This method gets the spectrum analyzer's amplitude units.             */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViPInt16     amplitudeUnits_Q (out)                                    */
/*        - Instrument's current amplitude units.                            */
/*                          dBm = 0                                          */
/*                         dBmV = 1                                          */
/*                         dBuV = 2                                          */
/*                        Volts = 3                                          */
/*                        Watts = 4                                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_amplitudeGetUnits
                        (ViSession instrumentHandle, 
                         ViPInt16 amplitudeUnits_Q);



    /***************************************************/
    /* BANDWIDTH CONTROL FUNCTIONS                     *
    /***************************************************/


/*****************************************************************************/
/*                                                                           */
/*  hpesa_BWSetResolution -                                                  */
/*    This method sets the spectrum analyzer's resolution bandwidth.         */
/*    Setting the resolution bandwidth uncouples the setting from the        */
/*    frequency span.                                                        */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViReal64      BWResolution                                             */
/*        - Instrument's new resolution bandwidth in Hz                      */
/*        - Range checked by instrument                                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_BWSetResolution
                        (ViSession instrumentHandle, 
                         ViReal64 BWResolution);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_BWSetResolutionAuto() -                                            */
/*    This method sets the spectrum analyzer's resolution bandwidth mode     */
/*    to AUTO, where resolution bandwidth is coupled to the frequency span.  */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_BWSetResolutionAuto
                        (ViSession instrumentHandle);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_BWSetVideo -                                                       */
/*    This method sets the spectrum analyzer's video bandwidth.              */
/*    Setting the video bandwidth uncouples the setting from the             */
/*    resolution bandwidth.                                                  */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViReal64      BWVideo                                                  */
/*        - Instrument's new video bandwidth in Hz                           */
/*        - Range checked by instrument                                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_BWSetVideo
                        (ViSession instrumentHandle,
                         ViReal64 BWVideo);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_BWSetVideoAuto -                                                   */
/*    This method sets the spectrum analyzer's video bandwidth mode to AUTO, */
/*    where video bandwidth is coupled to the resolution bandwidth.          */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViReal64      BWVideo                                                  */
/*        - Instrument's new video bandwidth in Hz                           */
/*        - Range checked by instrument                                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_BWSetVideoAuto
                        (ViSession instrumentHandle);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_BWSetAverage -                                                     */
/*    This method sets the spectrum analyzer's average type and count.       */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViInt16     BWAverageType (in)                                         */
/*        - Specifies the type of averaging to use.                          */
/*        - Enums:                                                           */
/*              hpesa_AVERAGE_LPOWER - logarithmically averages the power    */
/*                                     of the video data.                    */
/*              hpesa_AVERAGE_POWER  - averages the linear power of the      */
/*                                     successive measurements.              */
/*    ViInt32      BWAverageCount (in)                                       */
/*        - Specifies the number of measurements that are combined.          */
/*        - Range checked by instrument.                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_BWSetAverage
                        (ViSession instrumentHandle,
                         ViInt16 BWAverageType,
                         ViInt32 BWAverageCount);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_BWSetAverageOn -                                                   */
/*    This method sets the spectrum analyzer's averaging on or off.          */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViBoolean   BWAverageOn (in)                                           */
/*        - Instrument's new average state                                   */
/*        - Values: VI_TRUE (on) or VI_FALSE (off)                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_BWSetAverageOn
                        (ViSession instrumentHandle,
                         ViBoolean BWAverageOn);



    /***************************************************/
    /* TRACE CONTROL FUNCTIONS                         *
    /***************************************************/


/*****************************************************************************/
/*                                                                           */
/*  hpesa_traceSetMode -                                                     */
/*    This method selects the spectrum analyzer's trace mode for the         */
/*    trace.                                                                 */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViInt16 traceNumber (in)                                               */
/*        - Instrument trace number to modify                                */
/*        - Range:  hpesa_TRACE_1, hpesa_TRACE_2, hpesa_TRACE_3              */
/*    ViInt16 traceMode (in)                                                 */
/*        - Instrument's new trace mode:                                     */
/*              hpesa_TRACE_WRITE - Write puts the trace in the normal mode  */
/*                upholding the data.                                        */
/*              hpesa_TRACE_MAXHOLD - Maximum hold displays the highest      */
/*                measured trace value for all the data that has been        */
/*                measured since the function was turned on.                 */
/*              hpesa_TRACE_MINHOLD - Minimum hold displays the lowest       */
/*                measured trace value for all the data that has been        */
/*                measured since the function was turned on.                 */
/*              hpesa_TRACE_VIEW - View turns on the trace data so that it   */
/*                can be viewed on the display.                              */
/*              hpesa_TRACE_BLANK - Blank turns off the trace data so that   */
/*                it is not viewed on the display.                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_traceSetMode
                        (ViSession instrumentHandle,
                         ViInt16 traceNumber,
                         ViInt16 traceMode);


    /***************************************************/
    /* TRIGGER CONTROL FUNCTIONS                       *
    /***************************************************/

/*****************************************************************************/
/*                                                                           */
/*  hpesa_triggerSetSource -                                                 */
/*    This method selects the spectrum analyzer's source (or type) for       */
/*    triggering used to start a measurement.                                */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViInt16     triggerSource                                              */
/*        - Instrument's new trigger source (or type)                        */
/*        - Values:                                                          */
/*                hpesa_TRIGGER_IMMEDIATE - free-run triggering              */
/*                hpesa_TRIGGER_VIDEO - triggers on the video signal         */
/*                hpesa_TRIGGER_EXTERNAL - triggers on the power line source */
/*                                         signal                            */
/*                hpesa_TRIGGER_LINE - connect an external trigger source    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_triggerSetSource
                        (ViSession instrumentHandle,
                         ViInt16 triggerSource);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_triggerSetVideoLevel -                                             */
/*    This method selects the spectrum analyzer's video trigger level for    */
/*    trigger source is set to video.                                        */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViReal64    videoTriggerLevel                                          */
/*        - Instrument's new trigger level in current amplitude display units*/
/*        - Video bandwidth range checked by instrument                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_triggerSetVideoLevel
                        (ViSession instrumentHandle,
                         ViReal64 videoTriggerLevel);


    /***************************************************/
    /* SWEEP CONTROL FUNCTIONS                         *
    /***************************************************/

/*****************************************************************************/
/*                                                                           */
/*  hpesa_sweepSetMode -                                                     */
/*    This method selects whether the trigger system is continuously         */
/*    initiated or not.  This corresponds to continuous measurement or single*/
/*    measurement operation.  When set to ON, at the completion of each      */
/*    trigger cycle, the trigger system immediately initiates another        */
/*    trigger cycle.  When set to OFF, the trigger system remains in "idle"  */
/*    state until continuous is set to ON or an INITiate[:IMMediate] command */
/*    is received.  On receiving the INITiate[:IMMediate] command, it will   */
/*    go through a single trigger cycle and then return to the "idle" state. */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViInt16     sweepMode                                                  */
/*        - Instrument's new sweep mode                                      */
/*        - Values:                                                          */
/*                hpesa_SWEEP_TRIGGER_SINGLE - single sweep mode             */
/*                hpesa_SWEEP_TRIGGER_CONTINUOUS - continuous sweep mode     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_sweepSetMode
                        (ViSession instrumentHandle,
                         ViInt16 sweepMode);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_sweepGetMode -                                                     */
/*    This method gets the spectrum analyzer's sweep mode.                   */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViPInt16     sweepMode_Q (out)                                         */
/*        - Instrument's current trigger source                              */
/*             0 = hpesa_SWEEP_TRIGGER_SINGLE                                */
/*             1 = hpesa_SWEEP_TRIGGER_CONTINUOUS                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_sweepGetMode
                        (ViSession instrumentHandle,
                         ViPInt16 sweepMode_Q);

/*****************************************************************************/
/*                                                                           */
/*  hpesa_sweepSetTime -                                                     */
/*    This method specifies the time in which the instrument sweeps the      */
/*    displayed frequency range.                                             */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViReal64    sweepTime                                                  */
/*        - Instrument's new sweep time in seconds                           */
/*        - Range checked by instrument                                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_sweepSetTime
                        (ViSession instrumentHandle, 
                         ViReal64 sweepTime);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_sweepGetTime -                                                     */
/*    This method gets the spectrum analyzer's sweep time.                   */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViPReal64   sweepTime_Q (out)                                          */
/*        - Instrument's current sweep time in secs.                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_sweepGetTime
                        (ViSession instrumentHandle,
                         ViPReal64 sweepTime_Q);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_sweepSetTimeAuto -                                                 */
/*    This method sets the spectrum analyzer's sweep time to AUTO, where     */
/*    sweep time is coupled to the frequency span, resolution bandwidth      */
/*    and video bandwidth.                                                   */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_sweepSetTimeAuto
                        (ViSession instrumentHandle);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_sweepDoSweep -                                                     */
/*    This method causes a sweep to start and waits for it complete.  If     */
/*    sweep mode is continuous, the sweep will be re-started.  If sweep is   */
/*    single then the sweep is started.                                      */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViReal64    sweepTimeOut (in)                                          */
/*        -Time in seconds for this method to wait for the sweep to complete */
/*        -Range - 0 to hpesa_TIMEOUT_MAX                                    */
/*    ViPBoolean  sweepComplete (out)                                        */
/*        -Status indicating whether sweep completed successfully.           */
/*             0 - VI_FALSE                                                  */
/*             1 - VI_TRUE                                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_sweepDoSweep
                        (ViSession instrumentHandle,
                         ViReal64 sweepTimeOut,
                         ViPBoolean sweepComplete);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_sweepStartSweep -                                                  */
/*    This method causes a sweep to start.  It doesn't wait for the sweep to */
/*    complete.  If the sweep mode is continuous, the sweep will be restarted*/
/*    If sweep is single, then the sweep is started.                         */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_sweepStartSweep
                        (ViSession instrumentHandle);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_sweepAbortSweep -                                                  */
/*    This method stops any measurement in progress and resets the trigger   */
/*    system.  If the sweep mode is in single measure mode, then a           */
/*    "hpesa_sweepStartSweep" will start a new measurement.  If sweep mode   */
/*    is in continuous measure mode, then a new measurement begins           */
/*    immediately.                                                           */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_sweepAbortSweep
                        (ViSession instrumentHandle);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_sweepWaitForSweepComplete -                                        */
/*    This method causes a sweep to complete.                                */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViReal64    sweepWaitTimeOut (in)                                      */
/*        -Time in seconds for this method to wait for the sweep to complete */
/*        -Range - 0 to hpesa_TIMEOUT_MAX                                    */
/*    ViPBoolean  sweepWaitComplete (out)                                    */
/*        -Status indicating whether sweep completed successfully.           */
/*             0 - VI_FALSE                                                  */
/*             1 - VI_TRUE                                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_sweepWaitForSweepComplete
                        (ViSession instrumentHandle,
                         ViReal64 sweepWaitTimeOut, 
                         ViPBoolean sweepWaitComplete);


    /***************************************************/
    /* MARKER CONTROL FUNCTIONS                        *
    /***************************************************/

/*****************************************************************************/
/*                                                                           */
/*  hpesa_markerSetMode -                                                    */
/*     This method selects the type of markers to activate.                  */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViInt16     modeMarkerNumber                                           */
/*        - Marker number to select.                                         */
/*        - Values:                                                          */
/*            hpesa_MARKER_1 (default) - Marker 1                            */
/*            hpesa_MARKER_2 - Marker 2                                      */
/*            hpesa_MARKER_3 - Marker 3                                      */
/*            hpesa_MARKER_4 - Marker 4                                      */
/*    ViInt16     markerMode                                                 */
/*        - Instrument's new marker mode                                     */
/*        - Values:                                                          */
/*             hpesa_MARKER_NORMAL - selects a normal type of marker that    */
/*               you can position on a trace and readout the exact trace     */
/*               information.                                                */
/*             hpesa_MARKER_DELTA - activates a pair of delta markers, one   */
/*               of which is fixed at the current marker location.  The      */
/*               marker can then be moved around on the trace and the marker */
/*               readout shows the difference between two markers.           */
/*             hpesa_MARKER_BAND - activates a pair of band markers, where   */
/*               each marker can be independently positioned on the trace.   */
/*               The marker readout shows the difference between the two     */
/*               markers.                                                    */
/*             hpesa_MARKER_SPAN - activates a pair of span markers, where   */
/*               the marker positioning is controlled by changing the span   */
/*               and/or center frequency between the two markers.  The       */
/*               marker readout shows the difference between the two         */
/*               markers.                                                    */
/*             hpesa_MARKER_MODEOFF - turns specified marker off             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_markerSetMode
                        (ViSession instrumentHandle,
                         ViInt16 modeMarkerNumber,
                         ViInt16 markerMode);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_markerPeakSearch -                                                 */
/*     This method uses the spectrum analyzer's marker peak search function. */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViInt16     peakSearchMarkerNumber                                     */
/*        - Marker number to select.                                         */
/*        - Values:                                                          */
/*            hpesa_MARKER_1 - Marker 1                                      */
/*            hpesa_MARKER_2 - Marker 2                                      */
/*            hpesa_MARKER_3 - Marker 3                                      */
/*            hpesa_MARKER_4 - Marker 4                                      */
/*    ViInt16     peakSearchType                                             */
/*        - Marker search type to execute.  Note:    Instrument's peak       */
/*          search algorithm for PEAKNEXT, NEXTLEFT, and NEXTRIGHT is        */
/*          dependant on marker threshold and excursion value set.           */
/*        - Values:                                                          */
/*            hpesa_MARKER_PEAK - Places the selected marker on the highest  */
/*              point on the trace that is assigned to that particular       */
/*              marker number.                                               */
/*            hpesa_MARKER_PEAKNEXT - Places the selected marker on the next */
/*              highest signal peak from the current marked peak.            */
/*            hpesa_MARKER_PEAKLEFT - Places the selected marker on the next */
/*              highest signal peak to the left of the current marked peak.  */
/*            hpesa_MARKER_PEAKRIGHT - Places the selected marker on the     */
/*              next highest signal peak to the right of the current marked  */
/*              peak.                                                        */
/*            hpesa_MARKER_MIN - Places the selected marker on the lowest    */
/*              point on the trace that is assigned to that particular       */
/*              marker number.                                               */  
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_markerPeakSearch
                        (ViSession instrumentHandle,
                         ViInt16 peakSearchMarkerNumber, 
                         ViInt16 peakSearchType);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_markerToMarker -                                                   */
/*     This method sets the spectrum analyzer's selected instrument state to */
/*     the selected marker.                                                  */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViInt16     markerNumberTo                                             */
/*        - Marker number to select.                                         */
/*        - Values:                                                          */
/*            hpesa_MARKER_1 - Marker 1                                      */
/*            hpesa_MARKER_2 - Marker 2                                      */
/*            hpesa_MARKER_3 - Marker 3                                      */
/*            hpesa_MARKER_4 - Marker 4                                      */
/*    ViInt16     instStateToMarker                                          */
/*        - Instrument's marker peak function to select.                     */
/*        - Values:                                                          */
/*            hpesa_MARKER_CENTERFREQ - Sets the center frequency equal to   */
/*              the specified marker's frequency, which moves the marker     */
/*              to the center of the screen.                                 */
/*            hpesa_MARKER_CENTERFREQSTEP - Sets the center frequency step   */
/*              equal to the specified marker's frequency.                   */
/*            hpesa_MARKER_SPANFREQ - Sets the span to the value of the      */
/*              specified marker's frequency.  The specified marker must     */
/*              in delta mode.  Select the delta marker mode with            */
/*              hpesa_markerSetMode function.                                */
/*            hpesa_MARKER_STARTFREQ - Sets the start frequency to the value */
/*              of the specified marker's frequency.                         */
/*            hpesa_MARKER_STOPFREQ - Sets the stop frequency to the value   */
/*              of the specified marker's frequency.                         */
/*            hpesa_MARKER_REFLEVEL - Sets the reference level to the        */
/*              specified marker's amplitude.                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_markerToMarker
                        (ViSession instrumentHandle,
                         ViInt16 markerNumberTo,
                         ViInt16 instStateToMarker);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_markerSetSignalTrackOn -                                           */
/*     This method turns on or off marker signal tracking.  When enabled, it */
/*     continuously puts the selected marker on the highest displayed signal */
/*     peak and moves it to the center frequency.                            */
/*     Only one marker at a time can be enable for signal tracking.          */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViInt16     signalTrackMarkerNumber                                    */
/*        - Marker number to select.                                         */
/*        - Values:                                                          */
/*            hpesa_MARKER_1 - Marker 1                                      */
/*            hpesa_MARKER_2 - Marker 2                                      */
/*            hpesa_MARKER_3 - Marker 3                                      */
/*            hpesa_MARKER_4 - Marker 4                                      */
/*    ViInt16     markerSignalTrackOn                                        */
/*        - Select marker signal tracking on (VI_TRUE) or off (VI_FALSE).    */
/*        - Values:                                                          */
/*            VI_TRUE - On                                                   */
/*            VI_FALSE - OFF                                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_markerSetSignalTrackOn
                        (ViSession instrumentHandle,
                         ViInt16 signalTrackMarkerNumber,
                         ViBoolean markerSignalTrackOn);




/*****************************************************************************/
/*                                                                           */
/*  hpesa_markerGetYValue -                                                  */
/*     This method measures the current Y value for the designated marker    */
/*     on its assigned trace.  The value is in the Y-axis units for the      */
/*     current trace (which is often dBm).                                   */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViInt16     yValueMarkerNumber                                         */
/*        - Marker number to select.                                         */
/*        - Values:                                                          */
/*            hpesa_MARKER_1 - Marker 1                                      */
/*            hpesa_MARKER_2 - Marker 2                                      */
/*            hpesa_MARKER_3 - Marker 3                                      */
/*            hpesa_MARKER_4 - Marker 4                                      */
/*    ViPReal64    markerYValue_Q                                            */
/*        - Current Y axis value in the Y-axis units for the current trace   */
/*          (which is often dBm).                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_markerGetYValue
                        (ViSession instrumentHandle,
                         ViInt16 yValueMarkerNumber,
                         ViPReal64 markerYValue_Q);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_markerGetXPosition -                                               */
/*     This method measures the current X position for the designated marker.*/
/*     A trace is composed of 401 display points (X positions); therefore,   */
/*     the range of values that can be returned is 0 to 400.                 */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViInt16     xPositionMarkerNumberQ                                     */
/*        - Marker number to measure the X position.                         */
/*        - Values:                                                          */
/*            hpesa_MARKER_1 - Marker 1                                      */
/*            hpesa_MARKER_2 - Marker 2                                      */
/*            hpesa_MARKER_3 - Marker 3                                      */
/*            hpesa_MARKER_4 - Marker 4                                      */
/*    ViPInt16    markerXPosition_Q                                          */
/*        - Current X axis position.  Range of 0 to 400 for the 401 point    */
/*          display.                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_markerGetXPosition
                        (ViSession instrumentHandle,
                         ViInt16 xPositionMarkerNumberQ,
                         ViPInt16 markerXPosition_Q);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_markerSetXPosition -                                               */
/*     This method positions the designated marker on its assigned trace at  */
/*     the specified X position.  A trace is composed of 401 display points  */
/*     (X positions).  If the marker selected is not active, it will         */
/*     activate the marker.                                                  */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViInt16     xPositionMarkerNumber                                      */
/*        - Marker number to measure the X position.                         */
/*        - Values:                                                          */
/*            hpesa_MARKER_1 - Marker 1                                      */
/*            hpesa_MARKER_2 - Marker 2                                      */
/*            hpesa_MARKER_3 - Marker 3                                      */
/*            hpesa_MARKER_4 - Marker 4                                      */
/*    ViInt32     markerXPosition                                            */
/*        - X axis position to which the specified marker will be moved.     */
/*          Range of 0 to 400 for the 401 point display.                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_markerSetXPosition
                        (ViSession instrumentHandle,
                         ViInt16 xPositionMarkerNumber,
                         ViInt32 markerXPosition);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_markerGetXValue -                                                  */
/*     This method measures the current X value of the designated marker     */
/*     on its assigned trace.  The value is in the X-axis units for the      */
/*     trace (which is often frequency (Hz) or time (secs)).                 */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViInt16     xValueMarkerNumber                                         */
/*        - Marker number to measure.                                        */
/*        - Values:                                                          */
/*            hpesa_MARKER_1 - Marker 1                                      */
/*            hpesa_MARKER_2 - Marker 2                                      */
/*            hpesa_MARKER_3 - Marker 3                                      */
/*            hpesa_MARKER_4 - Marker 4                                      */
/*    ViPReal64    markerXValue_Q                                            */
/*        - Current X axis value in the X-axis units for the current trace   */
/*          (which is often Hz or secs)                                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_markerGetXValue
                        (ViSession instrumentHandle,
                         ViInt16 xValueMarkerNumber,
                         ViPReal64 markerXValue_Q);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_markerSetFunction -                                                */
/*     This method selects the marker function for the specified marker.     */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViInt16     functionMarkerNumber                                       */
/*        - Marker number to select.                                         */
/*        - Values:                                                          */
/*            hpesa_MARKER_1 - Marker 1                                      */
/*            hpesa_MARKER_2 - Marker 2                                      */
/*            hpesa_MARKER_3 - Marker 3                                      */
/*            hpesa_MARKER_4 - Marker 4                                      */
/*    ViInt16     markerFunction                                             */
/*        - Selects the marker function.                                     */
/*        - Values:                                                          */
/*            hpesa_MARKER_BPOWER - selects power integrated within the      */
/*              bandwidth marker function.                                   */
/*            hpesa_MARKER_NOISE - selects noise measurement marker function */
/*            hpesa_MARKER_FUNCOFF - turns off marker function mode.         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_markerSetFunction
                       (ViSession instrumentHandle,
                        ViInt16 functionMarkerNumber,
                        ViInt16 markerFunction);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_markerSetPeakExcursion -                                           */
/*     This method specifies the minimum signal excursion for the analyzer's */
/*     internal peak identification routine to recognize a signal as a peak. */
/*     This applies to all traces.  (The excursion is the delta power from   */
/*     a trace minimum value before (or after) a trace maximum value.)       */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViReal64    peakExcursionLevel                                         */
/*        - Excursion delta power level in dB.                               */
/*        - Values:   Range checked by instrument.                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_markerSetPeakExcursion
                        (ViSession instrumentHandle,
                         ViReal64 peakExcursionLevel);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_markerSetPeakThreshold -                                           */
/*     This method specifies the minimum signal level for the analyzer's     */
/*     internal peak identification routine to recognize a signal as a peak. */
/*     This applies to all traces.                                           */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViReal64    peakThresholdLevel                                         */
/*        - Threshold level in current display units.                        */
/*        - Values:   Range checked by instrument.                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_markerSetPeakThreshold
                        (ViSession instrumentHandle,
                         ViReal64 peakThresholdLevel);



    /***************************************************/
    /* FREQUENCY COUNTER CONTROL FUNCTIONS             *
    /***************************************************/

/*****************************************************************************/
/*                                                                           */
/*  hpesa_freqCounterSetResolution -                                         */
/*     This method sets the spectrum analyzer's frequency counter            */
/*     resolution.  The frequency counter resolution is normally coupled to  */
/*     the span frequency to generate the fastest accurate count.  By using  */
/*     this function, the resolution will be uncoupled until an instrument   */
/*     preset (hpesa_reset) or by calling hpesa_freqCounterSetResolutionAuto.*/
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViReal64    freqCounterResolution                                      */
/*        - Sets the resolution of the frequency counter.  Instrument rounds */
/*          values to the nearest power of 10, from 1 Hz to 100 kHz.         */
/*        - Values:   Range checked by instrument.                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_freqCounterSetResolution
                        (ViSession instrumentHandle,
                         ViReal64 freqCounterResolution);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_freqCounterSetResolutionAuto -                                     */
/*     This method sets the spectrum analyzer's frequency counter resolution */
/*     to be auto coupled to the frequency span.                             */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_freqCounterSetResolutionAuto
                        (ViSession instrumentHandle);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_freqCounterSetOn -                                                 */
/*     This method enables or disables the frequency counter for the         */
/*     specified marker.  Note that only one marker can be enabled for the   */
/*     frequency.                                                            */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViInt16     frequencyCounterMarker                                     */
/*        - Marker number to enable frequency counter operation.  Please     */
/*          note that only one marker can be active for a frequency counter  */
/*          mode at a time.  Enabling another marker for frequency counter   */
/*          operation will automatically disable the previous marker as the  */
/*          active frequency counter.                                        */
/*        - Values:                                                          */
/*            hpesa_MARKER_1 - Marker 1                                      */
/*            hpesa_MARKER_2 - Marker 2                                      */
/*            hpesa_MARKER_3 - Marker 3                                      */
/*            hpesa_MARKER_4 - Marker 4                                      */
/*    ViBoolean   frequencyCounterOn                                         */
/*        - Sets the frequency counter on (VI_TRUE) or off (VI_FALSE).       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_freqCounterSetOn
                        (ViSession instrumentHandle,
                         ViInt16 frequencyCounterMarker,
                         ViBoolean frequencyCounterOn);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_freqCounterGetXValue -                                             */
/*     This method measures the current X value of the frequency counter     */
/*     marker.  The value is in the X-axis units for the current trace       */
/*     (which is often frequency (Hz) or time (secs)).                       */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViPReal64   freqCounterGetXValue_Q                                     */
/*        - Current X axis value of the frequency in the X-axis units for    */
/*          the current trace (which is often Hz or secs).                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_freqCounterGetXValue
                        (ViSession instrumentHandle,
                         ViPReal64 freqCounterGetXValue_Q);


    /***************************************************/
    /* INPUT CONTROL FUNCTIONS                         *
    /***************************************************/

/*****************************************************************************/
/*                                                                           */
/*  hpesa_inputCouplingSetMode -                                             */
/*     This method selects AC or DC coupling for the RF INPUT port.  A       */
/*     blocking capacitor is switched in for AC mode.                        */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViInt16   inputCouplingMode                                            */
/*        - Instrument's new coupling mode.                                  */
/*        - Range:   hpesa_INPUT_COUPLE_AC - AC                              */
/*                   hpesa_INPUT_COUPLE_DC - DC                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_inputCouplingSetMode
                        (ViSession instrumentHandle,
                         ViInt16 inputCouplingMode);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_inputSetImpedance -                                                */
/*     This method selects the input impedance correction method to use.     */
/*     Amplitude correction is applied to the display data to adjust for the */
/*     measurement situations where the unit under test has a different      */
/*     impedance then the instrument's input impedance.                      */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViInt16   inputImpedance                                               */
/*        - Specifies the input impedance                                    */
/*        - Range:   hpesa_INPUT_IMPEDANCE_50 - 50 Ohms                      */
/*                   hpesa_INPUT_IMPEDANCE_75 - 75 Ohms                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_inputSetImpedance
                        (ViSession instrumentHandle,
                         ViInt16 inputImpedance);



    /***************************************************/
    /* DISPLAY CONTROL FUNCTIONS                       *
    /***************************************************/


/*****************************************************************************/
/*                                                                           */
/*  hpesa_displayLineSetup -                                                 */
/*     This method turns the display line on or off and sets the display     */
/*     line level.                                                           */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*    ViBoolean   displayLineOn                                              */
/*        - Enable or disables the display line.                             */
/*        - Range:   VI_TRUE - On                                            */
/*                   VI_FALSE- Off                                           */
/*    ViReal64    displayLineLevel                                           */
/*        - Sets the display line level in current active units.             */
/*        - Range:  Determined by instrument                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_displayLineSetup
                        (ViSession instrumentHandle,
                         ViBoolean displayLineOn,
                         ViReal64 displayLineLevel);


    /***************************************************/
    /* TRACE MATH CONTROL FUNCTIONS                    *
    /***************************************************/

/*****************************************************************************/
/*                                                                           */
/*  hpesa_traceMeasureMean -                                                 */
/*     This method gets the mean of the amplitude of the trace elements in   */
/*     current amplitude measurement units.                                  */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        - Instrument handle returned from hpesa_init()                     */
/*    ViInt16     meanTraceNumber (in)                                       */
/*        - Instrument trace number to select.                               */
/*        - Range:  hpesa_TRACE_1 - Trace 1                                  */
/*                  hpesa_TRACE_2 - Trace 2                                  */
/*                  hpesa_TRACE_3 - Trace 3                                  */
/*    ViPReal64   traceMean_Q (out)                                          */
/*        - Y-Axis (amplitude) mean of the specified trace in the current    */
/*          y-axis units (which is often dBm).                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_traceMeasureMean
                        (ViSession instrumentHandle,
                         ViInt16 meanTraceNumber,
                         ViPReal64 traceMean_Q);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_traceSetSmooth -                                                   */
/*     This method smoothes the specified trace according to the number of   */
/*     specified points.                                                     */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        - Instrument handle returned from hpesa_init()                     */
/*    ViInt16     smoothTraceNumber (in)                                     */
/*        - Instrument trace number to select.                               */
/*        - Range:  hpesa_TRACE_1 - Trace 1                                  */
/*                  hpesa_TRACE_2 - Trace 2                                  */
/*                  hpesa_TRACE_3 - Trace 3                                  */
/*    ViInt32     smoothTracePoints (in)                                     */
/*        - Specifies the number of points that will be smoothed.            */
/*        - Range:  Checked by instrument                                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_traceSetSmooth
                        (ViSession instrumentHandle,
                         ViInt16 smoothTraceNumber,
                         ViInt32 smoothTracePoints);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_traceMeasurePeaks -                                                */
/*     This method gets the number of peaks and the peak amplitude values in */
/*     the active trace.                                                     */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        - Instrument handle returned from hpesa_init()                     */
/*    ViInt16     tracePeakSortMode (in)                                     */
/*        - Select how the identified peaks are sorted in the parameters     */
/*          "tracePeakFrequencyArray_Q" and "tracePeakAmplitudeArray_Q".     */
/*        - Range:  hpesa_PEAKSORT_AMPL - sorts the identified peaks by      */
/*                                        descending amplitude.              */
/*                  hpesa_PEAKSORT_FREQ - sorts the identified peaks by      */
/*                                        increasing frequency.              */
/*    ViPInt32    traceNumberPeaks_Q(out)                                    */
/*        - Returns the number of peaks detected in the active trace.        */
/*          It also indicates the actual number of values returned in the    */
/*          frequency and amplitude data arrays.                             */
/*    ViReal64   tracePeakFrequencyArray_Q[] (out)                           */
/*        - This is an array of frequencies of the identified peaks in the   */
/*          current active trace.                                            */
/*    ViReal64   tracePeakAmplitudeArray_Q[] (out)                           */
/*        - This is an array of amplitudes of the identified peaks in the    */
/*          current active trace.                                            */
/*    ViInt32     tracePeakArrayMaxSize (in)                                 */
/*        - This value specifies the maximum size of the peak data arrays    */
/*          "tracePeakFrequencyArray_Q" and "tracePeakAmplitudeArray_Q".     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_traceMeasurePeaks
                        (ViSession instrumentHandle,
                         ViInt16 tracePeakSortMode,
                         ViPInt32 traceNumberPeaks_Q,
                         ViReal64 _VI_FAR tracePeakFrequencyArray_Q[],
                         ViReal64 _VI_FAR tracePeakAmplitudeArray_Q[],
                         ViInt32 tracePeakArrayMaxSize);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_traceGetTraceData -                                                */
/*     This method transfers data from the instrument to the controller.     */
/*     There are normally 401 points in a trace.                             */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        - Instrument handle returned from hpesa_init()                     */
/*                                                                           */
/*    ViInt16     traceNumberRead (in)                                       */
/*        - Specifies the data trace to read.                                */
/*        - Range:  hpesa_TRACE_1 - Trace 1                                  */
/*                  hpesa_TRACE_2 - Trace 2                                  */
/*                  hpesa_TRACE_3 - Trace 3                                  */
/*                                                                           */
/*    ViReal64    traceDataReadArray_Q[](out)                                */
/*        - This is x-axis data values for the specified trace in the        */
/*          current x-axis units.  Normally the number of points in the      */
/*          trace will be 401.                                               */
/*                                                                           */
/*    ViInt32      traceDataReadArrayMaxSize (in)                            */
/*        - This value specifies the maximum size of the                     */
/*          "traceDataReadArray_Q" array.  The value should be large enough  */
/*          for the number of points in the trace, which is normally 401     */
/*          display points.                                                  */
/*                                                                           */
/*    ViPInt32     traceDataReadArraySize_Q                                  */
/*        - This is the actual number of data points read from the           */
/*          instrument.                                                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_traceGetTraceData
                        (ViSession instrumentHandle,
                         ViInt16 traceNumberRead,
                         ViReal64 _VI_FAR traceDataReadArray_Q[],
                         ViInt32 traceDataReadArrayMaxSize,
                         ViPInt32 traceDataReadArraySize_Q);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_traceSetTraceData -                                                */
/*     This method transfers data from the controller to the instrument.     */
/*     There are normally 401 points in a trace.                             */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        - Instrument handle returned from hpesa_init()                     */
/*                                                                           */
/*    ViInt16     traceNumberWrite (in)                                      */
/*        - Specifies the data trace to write.                               */
/*        - Range:  hpesa_TRACE_1 - Trace 1                                  */
/*                  hpesa_TRACE_2 - Trace 2                                  */
/*                  hpesa_TRACE_3 - Trace 3                                  */
/*                                                                           */
/*    ViReal64    traceDataWriteArray_Q[] (out)                              */
/*        - This is x-axis data values for the specified trace in the        */
/*          current x-axis units.                                            */
/*                                                                           */
/*    ViInt32      traceDataWriteArraySize (in)                              */
/*        - This value specifies the size of the "traceDataWriteArray" array */
/*          This value should be large enough for the number of points in    */
/*          the trace, which is normally 401 display points.                 */     
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_traceSetTraceData
                        (ViSession instrumentHandle,
                         ViInt16 traceNumberWrite,
                         ViReal64 _VI_FAR traceDataWriteArray_Q[],
                         ViInt32 traceDataWriteArraySize);



    /***************************************************/
    /* SWEEP GATE CONTROL FUNCTIONS                    *
    /***************************************************/

/*****************************************************************************/
/*                                                                           */
/*  hpesa_sweepGateSetUp -                                                   */
/*     This method sets up the spectrum analyzer's sweep gate functionality. */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        - Instrument handle returned from hpesa_init()                     */
/*                                                                           */
/*    ViBoolean   sweepGateOn (in)                                           */
/*        - Enable or disable the sweep gate operation.                      */
/*        - Range:  VI_TRUE (On) or VI_FALSE (Off)                           */
/*                                                                           */
/*    ViInt16     sweepGateType                                              */
/*        - Select sweep gate trigger type                                   */
/*        - Range:  hpesa_GATESWEEPTYPE_LEVEL - triggers the gate when       */
/*                         the signal surpasses a specific level.  Set       */
/*                         "sweepGateLevelType" to either low or high        */
/*                  hpesa_GATESWEEPTYPE_EDGE - triggers the gate when the    */
/*                         edge of a signal is encountered.  Set             */
/*                         "sweepGateEdgePol" to either a negative-going     */
/*                         or a positive-going edge.                         */
/*                                                                           */
/*    ViInt16     sweepGateLevelType                                         */
/*        - Select sweep gate trigger level type. This is for LEVEL          */
/*          triggering only.                                                 */
/*        - Range:  hpesa_GATESWEEPLEVEL_HIGH - gate triggers when level is  */
/*                        TTL high (> 2.0V),                                 */
/*                  hpesa_GATESWEEPLEVEL_LOW - gate triggers when level is   */
/*                        TTL low (<0.8V).                                   */
/*                                                                           */
/*    ViInt16     sweepGateEdgePol                                           */
/*        - Select sweep gate trigger edge polarity.  This is for EDGE       */
/*          triggering only.                                                 */
/*        - Range:  hpesa_GATESWEEPPOL_POSITIVE - gate triggers when         */
/*                        edge is positive.                                  */
/*                  hpesa_GATESWEEPPOL_NEGATIVE - gate triggers when         */
/*                        edge is negative.                                  */
/*                                                                           */
/*    ViReal64    sweepGateEdgeDelay                                         */
/*        - Sets the delay time (in seconds) from when the gate trigger      */
/*          occurs to when the gate opens.  This is for EDGE triggering only.*/
/*                                                                           */
/*    ViReal64    sweepGateEdgeLength                                        */
/*        - Sets the gate time (in seconds) length.  This is for EDGE        */
/*          triggering only.                                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_sweepGateSetUp
                        (ViSession instrumentHandle,
                         ViBoolean sweepGateOn,
                         ViInt16 sweepGateType,
                         ViInt16 sweepGateLevelType,
                         ViInt16 sweepGateEdgePol,
                         ViReal64 sweepGateEdgeDelay,
                         ViReal64 sweepGateEdgeLength);



    /***************************************************/
    /* TRACKING GENERATOR CONTROL FUNCTIONS            *
    /***************************************************/

/*****************************************************************************/
/*                                                                           */
/*  hpesa_trackGenSetOn -                                                    */
/*     This method enables or disables the tracking generator output.        */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        - Instrument handle returned from hpesa_init()                     */
/*                                                                           */
/*    ViBoolean   trackingGeneratorOn (in)                                   */
/*        - Enable or disable the tracking generator output.                 */
/*        - Range:  VI_TRUE (on) or VI_FALSE (off)                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_trackGenSetOn 
                        (ViSession instrumentHandle,
                         ViBoolean trackingGeneratorOn);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_trackGenSetAttenuation -                                           */
/*     This method sets the spectrum analyzer's RF tracking generator        */
/*     attenuation value.  Setting the amplitude attenuation with this       */
/*     function uncouples the attenuator from the output amplitude level.    */
/*     To re-couple the attenuator from the output amplitude level, call the */
/*     the function "hpesa_trackGenSetAttenationAuto".                       */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        - Instrument handle returned from hpesa_init()                     */
/*                                                                           */
/*    ViReal64    trackingGeneratorAttenuation                               */
/*        - Instrument new tracking generator amplitude attenuation in dB    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_trackGenSetAttenuation 
                        (ViSession instrumentHandle,
                         ViReal64 trackingGeneratorAttenuation);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_trackGenSetAttenuationAuto -                                       */
/*     This method couples the spectrum analyzer's RF tracking generator     */
/*     attenuation value to be set automatically by the instrument.          */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        - Instrument handle returned from hpesa_init()                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_trackGenSetAttenuationAuto 
                        (ViSession instrumentHandle);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_trackGenSetAmplitude -                                             */
/*     This method sets the spectrum analyzer's tracking generator source    */
/*     output amplitude level.                                               */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        - Instrument handle returned from hpesa_init()                     */
/*                                                                           */
/*    ViReal64    trackingGeneratorAmplitude (in)                            */
/*        - Instrument's new source output amplitude level in current        */
/*          display units.                                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_trackGenSetAmplitude
                        (ViSession instrumentHandle,
                         ViReal64 trackingGeneratorAttenuation);




/*****************************************************************************/
/*                                                                           */
/*  hpesa_trackGenSetAmplitudePeakOn -                                       */
/*     This method enables the tracking of the source output                 */
/*     with the spectrum analyzer sweep so that the power is maximized.      */
/*     This method is not applicable for the 1.5 GHz tracking generator.     */
/*     This command should be followed by a hpesa_sweepDoSweep in order      */
/*     to be effective.                                                      */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        - Instrument handle returned from hpesa_init()                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_trackGenSetAmplitudePeakOn
                        (ViSession instrumentHandle);


    /***************************************************/
    /* CALIBRATION/ALIGNMENT CONTROL FUNCTIONS         *
    /***************************************************/

/*****************************************************************************/
/*                                                                           */
/*  hpesa_alignSignalRFSetOn -                                               */
/*     This method turns of or on the internal 50 MHz oscillator.            */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        - Instrument handle returned from hpesa_init()                     */
/*                                                                           */
/*    ViBoolean   alignSignalRFOn (in)                                       */
/*        - Select 50 MHz oscillator on or off.                              */
/*        - Range:  VI_TRUE (on) or VI_FALSE (off)                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_alignSignalRFSetOn 
                        (ViSession instrumentHandle,
                         ViBoolean alignSignalRFOn);




/*****************************************************************************/
/*                                                                           */
/*  hpesa_alignAutoSetMode -                                                 */
/*     The automatic alignment routines normally run continuously in the     */
/*     background. This method allows you to choose to have all the routines */
/*     running, all the routines but not the RF alignment running, or no     */
/*     automatic alignment routines running at all.                          */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        - Instrument handle returned from hpesa_init()                     */
/*                                                                           */
/*    ViInt16     alignAutoMode (in)                                         */
/*        - Select the auto alignment mode to operate in.                    */
/*        - Range:  hpesa_ALIGN_AUTO_ALL - selects auto align all            */
/*                  hpesa_ALGIN_AUTO_NRF - selects auto align all but RF     */
/*                  hpesa_ALIGN_AUTO_OFF - turns off auto alignment          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_alignAutoSetMode 
                        (ViSession instrumentHandle,
                         ViInt16 alignAutoMode);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_alignFreqCorrSetOn -                                               */
/*     The method enables or disables the spectrum analyzer's frequency      */
/*     operation.                                                            */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        - Instrument handle returned from hpesa_init()                     */
/*                                                                           */
/*    ViBoolean   alignFreqCorrOn (in)                                       */
/*        - Select frequency correction mode to on or off.                   */
/*        - Range:  VI_TRUE (on) or VI_FALSE (off)                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_alignFreqCorrSetOn
                        (ViSession instrumentHandle,
                         ViBoolean alignFreqCorrOn);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_alignDoAlignment -                                                 */
/*     This method forces an immediate alignment of the specified assembly.  */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        - Instrument handle returned from hpesa_init()                     */
/*                                                                           */
/*    ViInt16     alignmentType(in)                                          */
/*        - Select the assembly to do an immediate alignment on.             */
/*        - Range:  hpesa_ALIGN_ALL - align all assemblies.                  */
/*                  hpesa_ALIGN_RF - align the RF assembly                   */
/*                  hpesa_ALIGN_TG - align the tracking generator assembly   */
/*                  hpesa_ALIGN_FMD - align the FM Demod assembly            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_alignDoAlignment
                        (ViSession instrumentHandle,
                         ViInt16 alignmentType);


    /***************************************************/
    /* MEASURE CONTROL FUNCTIONS                       */
    /***************************************************/


/*****************************************************************************/
/*                                                                           */
/*  hpesa_measureSetMode                                                     */
/*    This method stops the current measurement and sets up the instrument   */
/*    for the specified measurement using the factory default instrument     */
/*    settings.  This will always set the instrument in single sweep mode    */
/*    and place the measurement in the idle state.                           */
/*    If the measurement is set to "Measure Off", the instrument will be     */
/*    placed in base instrument spectrum analyzer state.                     */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle                                           */
/*        - Instrument handle returned from hpesa_init.                      */
/*    ViInt16     measureMode                                                */
/*        - The instrument's new measurement mode.                           */
/*        - Values:                                                          */
/*            hpesa_MEASURE_MODE_OFF (default) - Sets the instrument in base */
/*              instrument spectrum analyzer state.                          */
/*            hpesa_MEASURE_MODE_ADJACENT  - Sets the instrument in adjacent */
/*              channel power mode.                                          */
/*            hpesa_MEASURE_MODE_CHANNEL - Sets the instrument in channel    */
/*              power mode.                                                  */
/*            hpesa_MEASURE_MODE_OCCUPIED - Sets the instrument in occupied  */
/*              bandwidth mode.                                              */
/*            hpesa_MEASURE_MODE_EMISSION - Sets the instrument in emission  */
/*              bandwidth mode.                                              */
/*            hpesa_MEASURE_MODE_HARMONIC - Sets the instrument in harmonic  */
/*              distortion mode.                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_measureSetMode
                        (ViSession instrumentHandle,
                         ViInt16 measureMode);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_measureGetMode                                                     */
/*    This method returns the current measurement state of the instrument.   */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle                                           */
/*        - Instrument handle returned from hpesa_init.                      */
/*    ViPInt16    measureMode_Q                                              */
/*        - Instrument's current measurement mode.                           */
/*           0 = Measure Off (Spectrum Analyzer)                             */
/*           1 = Adjacent Channel Power                                      */
/*           2 = Channel Power                                               */
/*           3 = Occupied Bandwidth                                          */
/*           4 = Emission Bandwidth                                          */
/*           5 = Harmonic Distortion                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_measureGetMode
                       (ViSession instrumentHandle,
                        ViPInt16 measureMode_Q);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_ACPConfig                                                          */
/*    This method sets up the adjacent channel power measurement operation.  */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle                                           */
/*        - Instrument handle returned from hpesa_init.                      */
/*    ViBoolean   ACPAverageOn                                               */
/*        - Instrument's new ACP average state.                              */
/*        - Values:                                                          */
/*            VI_TRUE - Averaging on.                                        */
/*            VI_FALSE - Averaging off.                                      */
/*    ViInt16     ACPAverageCount                                            */
/*        - Specifies the number of ACP measurements that are combined.      */
/*    ViReal64    ACPMainChannelBW                                           */
/*        - Instrument's new main (channel) bandwidth in Hz.                 */
/*    ViReal64    ACPAdjChannelBW                                            */
/*        - Instrument's new adjacent channel bandwidth in Hz.               */
/*    ViReal64    ACPChannelSpacing                                          */
/*        - Instrument's new adjacent channel spacing in Hz.                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_ACPConfig
                        (ViSession instrumentHandle,
                         ViBoolean ACPAverageOn,
                         ViInt16 ACPAverageCount,
                         ViReal64 ACPMainChannelBW,
                         ViReal64 ACPAdjChannelBW,
                         ViReal64 ACPChannelSpacing);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_ACPConfigQuery                                                     */
/*    This method retrieves the adjacent channel power measurement setup     */
/*    values.                                                                */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle                                           */
/*        - Instrument handle returned from hpesa_init.                      */
/*    ViPBoolean  ACPAverageOn_Q                                             */
/*        - Instrument's current ACP average state.                          */
/*    ViPInt16    ACPAverageCount_Q                                          */
/*        - Returns the number of ACP measurements that are combined.        */
/*    ViPReal64   ACPMainChannelBW_Q                                         */
/*        - Instrument's current main (channel) bandwidth in Hz.             */
/*    ViPReal64   ACPAdjChannelBW_Q                                          */
/*        - Instrument's current adjacent channel bandwidth in Hz.           */
/*    ViPReal64   ACPChannelSpacing_Q                                        */
/*        - Instrument's new adjacent channel spacing in Hz.                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_ACPConfigQuery
                        (ViSession instrumentHandle,
                         ViPBoolean ACPAverageOn_Q,
                         ViPInt16 ACPAverageCount_Q,
                         ViPReal64 ACPMainChannelBW_Q,
                         ViPReal64 ACPAdjChannelBW_Q,
                         ViPReal64 ACPChannelSpacing_Q);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_ACPMeasure                                                         */
/*    This method retrieves the main, lower, and upper channel power         */
/*    measurements from the instrument, based on the method of retrieval     */
/*    (measure, read, or fetch).                                             */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle                                           */
/*        - Instrument handle returned from hpesa_init.                      */
/*    ViReal64    ACPMeasureTimeout                                          */
/*        - Temporary timeout value in seconds for measurement.              */
/*    ViInt16     ACPMeasureType                                             */
/*        - The method used to retrieve the ACP measurements.                */
/*        - Values:                                                          */
/*            hpesa_MEASURE_METHOD_MEAS - Initiates and retrieves            */
/*              measurement data from the instrument, using factory default  */
/*              settings.                                                    */
/*            hpesa_MEASURE_METHOD_READ - Initiates and retrieves            */
/*              measurement data from the instrument, using the current      */
/*              setup values.                                                */
/*            hpesa_MEASURE_METHOD_FETCH - Retrieves data from the           */
/*              instrument using the current setup values.  Does not         */
/*              initiate a measurement.                                      */
/*    ViPReal64   MainACP_Q                                                  */
/*        - The value of the main channel power in the current amplitude     */
/*          units.                                                           */
/*    ViPReal64   LowerACP_Q                                                 */
/*        - The value of the lower channel power relative to the main        */
/*          channel power, in dB                                             */
/*    ViPReal64   UpperACP_Q                                                 */
/*        - The value of the upper channel power relative to the main        */
/*          channel power, in dB.                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_ACPMeasure
                        (ViSession instrumentHandle,
                         ViReal64 ACPMeasureTimeout,
                         ViInt16 ACPMeasureType,
                         ViPReal64 MainACP_Q,
                         ViPReal64 LowerACP_Q,
                         ViPReal64 UpperACP_Q);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_CHPConfig                                                          */
/*    This method sets up the channel power measurement operation.           */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle                                           */
/*        - Instrument handle returned from hpesa_init.                      */
/*    ViBoolean   CHPAverageOn                                               */
/*        - Instrument's new CHP average state.                              */
/*        - Values:                                                          */
/*            VI_TRUE - Averaging on.                                        */
/*            VI_FALSE - Averaging off.                                      */
/*    ViInt16     CHPAverageCount                                            */
/*        - Specifies the number of CHP measurements that are combined.      */
/*    ViReal64    CHPIntegrationBW                                           */
/*        - Instrument's new frequency span (bandwidth) over which to        */
/*          integrate power.                                                 */
/*    ViReal64    CHPSpan                                                    */
/*        - Instrument's new channel power span..                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_CHPConfig
                        (ViSession instrumentHandle,
                         ViBoolean CHPAverageOn,
                         ViInt16 CHPAverageCount,
                         ViReal64 CHPIntegrationBW,
                         ViReal64 CHPSpan);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_CHPConfigQuery                                                     */
/*    This method retrieves the channel power setup information.             */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle                                           */
/*        - Instrument handle returned from hpesa_init.                      */
/*    ViPBoolean  CHPAverageOn_Q                                             */
/*        - Instrument's current CHP average state.                          */
/*    ViPInt16    CHPAverageCount_Q                                          */
/*        - Instrument's current number of averages.                         */
/*    ViPReal64   CHPIntegrationBW_Q                                         */
/*        - Instrument's current frequency span (bandwidth).                 */
/*    ViPReal64   CHPSpan_Q                                                  */
/*        - Instrument's current channel power span.                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_CHPConfigQuery
                        (ViSession instrumentHandle,
                         ViPBoolean CHPAverageOn_Q,
                         ViPInt16 CHPAverageCount_Q,
                         ViPReal64 CHPIntegrationBW_Q,
                         ViPReal64 CHPSpan_Q);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_CHPMeasure                                                         */
/*    This method retrieves the scalar results of the main channel power and */
/*    power density from the instrument, based on the method of retrieval    */
/*    (measure, read, or fetch).                                             */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle                                           */
/*        - Instrument handle returned from hpesa_init.                      */
/*    ViReal64    CHPMeasureTimeout                                          */
/*        - Temporary timeout value in seconds for measurement.              */
/*    ViInt16     CHPMeasureType                                             */
/*        - The method used to retrieve the CHP measurements.                */
/*        - Values:                                                          */
/*            hpesa_MEASURE_METHOD_MEAS - Initiates and retrieves            */
/*              measurement data from the instrument, using factory default  */
/*              settings.                                                    */
/*            hpesa_MEASURE_METHOD_READ - Initiates and retrieves            */
/*              measurement data from the instrument, using the current      */
/*              setup values.                                                */
/*            hpesa_MEASURE_METHOD_FETCH - Retrieves data from the           */
/*              instrument using the current setup values.  Does not         */
/*              initiate a measurement.                                      */
/*    ViPReal64   CHPChannelPower_Q                                          */
/*        - The value of the channel power in the current amplitude units.   */
/*    ViPReal64   CHPDensity_Q                                               */
/*        - The value of the channel power density in amplitude units/Hz     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_CHPMeasure
                        (ViSession instrumentHandle,
                         ViReal64 CHPMeasureTimeout,
                         ViInt16 CHPMeasureType,
                         ViPReal64 CHPChannelPower_Q,
                         ViPReal64 CHPDensity_Q);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_OBWConfig                                                          */
/*    This method sets up the occupied bandwidth measurement operation.      */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle                                           */
/*        - Instrument handle returned from hpesa_init.                      */
/*    ViBoolean   OBWAverageOn                                               */
/*        - Instrument's new occupied bandwidth average state.               */
/*        - Values:                                                          */
/*            VI_TRUE - Averaging on.                                        */
/*            VI_FALSE - Averaging off.                                      */
/*    ViInt16     OBWAverageCount                                            */
/*        - Specifies the number of occupied bandwidth measurements that are */
/*          combined.                                                        */
/*    ViReal64    OBWSpan                                                    */
/*        - Instrument's new occupied bandwidth span.  The analyzer span     */
/*          will retain this value throughout the measurement.               */
/*    ViReal64    OBWPercentPower                                            */
/*        - Instrument's new occupied bandwidth power in percent..           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_OBWConfig
                        (ViSession instrumentHandle,
                         ViBoolean OBWAverageOn,
                         ViInt16 OBWAverageCount, 
                         ViReal64 OBWSpan,
                         ViReal64 OBWPercentPower);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_OBWConfigQuery                                                     */
/*    This method sets up the occupied bandwidth measurement operation.      */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle                                           */
/*        - Instrument handle returned from hpesa_init.                      */
/*    ViPBoolean  OBWAverageOn_Q                                             */
/*        - Instrument's current occupied bandwidth average state.           */
/*    ViPInt16    OBWAverageCount_Q                                          */
/*        - Instrument's current number of occupied bandwidth averages.      */
/*    ViPReal64   OBWSpan_Q                                                  */
/*        - Instrument's current occupied bandwidth span.                    */
/*    ViPReal64   OBWPercentPower_Q                                          */
/*        - Instrument's current occupied bandwidth power in percent.        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_OBWConfigQuery
                        (ViSession instrumentHandle,
                         ViPBoolean OBWAverageOn_Q,
                         ViPInt16 OBWAverageCount_Q,
                         ViPReal64 OBWSpan_Q,
                         ViPReal64 OBWPercentPower_Q);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_OBWMeasure                                                         */
/*    This method retrieves the scalar results of occupied bandwidth and     */
/*    transmit frequency error from the instrument, based on the method of   */
/*    retrieval (measure, read, or fetch).                                   */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle                                           */
/*        - Instrument handle returned from hpesa_init.                      */
/*    ViReal64    OBWMeasureTimeout                                          */
/*        - Temporary timeout value in seconds for measurement.              */
/*    ViInt16     OBWMeasureType                                             */
/*        - The method used to retrieve the occupied bandwidth measurements. */
/*        - Values:                                                          */
/*            hpesa_MEASURE_METHOD_MEAS - Initiates and retrieves            */
/*              measurement data from the instrument, using factory default  */
/*              settings.                                                    */
/*            hpesa_MEASURE_METHOD_READ - Initiates and retrieves            */
/*              measurement data from the instrument, using the current      */
/*              setup values.                                                */
/*            hpesa_MEASURE_METHOD_FETCH - Retrieves data from the           */
/*              instrument using the current setup values.  Does not         */
/*              initiate a measurement.                                      */
/*    ViPReal64   OBWOccupiedBandwidth_Q                                     */
/*        - The value of the occupied bandwidth in Hz.                       */
/*    ViPReal64   OBWFreqError_Q                                             */
/*        - The value of transmit frequency error in Hz.                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_OBWMeasure
                        (ViSession instrumentHandle,
                         ViReal64 OBWMeasureTimeout,
                         ViInt16 OBWMeasureType,
                         ViPReal64 OBWOccupiedBandwidth_Q,
                         ViPReal64 OBWFreqError_Q);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_EBWConfig                                                          */
/*    This method sets up the emission bandwidth measurement operation.      */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle                                           */
/*        - Instrument handle returned from hpesa_init.                      */
/*    ViBoolean   EBWAverageOn                                               */
/*        - Instrument's new EBW average state.                              */
/*        - Values:                                                          */
/*            VI_TRUE - Averaging on.                                        */
/*            VI_FALSE - Averaging off.                                      */
/*    ViInt16     EBWAverageCount                                            */
/*        - Specifies the number of EBW measurements that are combined.      */
/*    ViBoolean   EBWMaxHold                                                 */
/*        - Turns on or off the trace max hold trace feature for emission    */
/*          bandwidth measurements.                                          */
/*        - Values:                                                          */
/*            VI_TRUE - Trace max hold on.                                   */
/*            VI_FALSE - Trace max hold off.                                 */
/*    ViReal64    EBWSpan                                                    */
/*        - Instrument's new emission bandwidth span.                        */
/*    ViReal64    EmissBWXdB                                                 */
/*        - Sets the dB value (X dB) below the maximum value on the signal   */
/*          at which to measure the emission bandwidth.                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_EBWConfig
                        (ViSession instrumentHandle,
                         ViBoolean EBWAverageOn,
                         ViInt16 EBWAverageCount,
                         ViBoolean EBWMaxHold,
                         ViReal64 EBWSpan,
                         ViReal64 EmissBWXdB);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_EBWConfigQuery                                                     */
/*    This method retrieves the current emission bandwidth settings.         */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle                                           */
/*        - Instrument handle returned from hpesa_init.                      */
/*    ViPBoolean  EBWAverageOn_Q                                             */
/*        - Instrument's current emission bandwidth average state.           */
/*    ViPInt16    EBWAverageCount_Q                                          */
/*        - Instrument's current number of emission bandwidth averages.      */
/*    ViPBoolean  EBWMaxHold_Q                                               */
/*        - Instrument's current emission bandwidth trace max hold state.    */
/*    ViPReal64   EBWSpan_Q                                                  */
/*        - Instrument's current emission bandwidth span.                    */
/*    ViPReal64   EmissBWXdB_Q                                               */
/*        - Current  dB value (X dB) below the maximum value on the signal   */
/*          at which to measure the emission bandwidth.                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_EBWConfigQuery
                        (ViSession instrumentHandle,
                         ViPBoolean EBWAverageOn_Q,
                         ViPInt16 EBWAverageCount_Q,
                         ViPBoolean EBWMaxHold_Q,
                         ViPReal64 EBWSpan_Q,
                         ViPReal64 EmissBWXdB_Q);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_EBWMeasure                                                         */
/*    This method retrieves the emission bandwidth from the instrument,      */
/*    based on the method of retrieval (measure, read, or fetch).            */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle                                           */
/*        - Instrument handle returned from hpesa_init.                      */
/*    ViReal64    EBWMeasureTimeout                                          */
/*        - Temporary timeout value in seconds for measurement.              */
/*    ViInt16     EBWMeasureType                                             */
/*        - The method used to retrieve the EBW measurements.                */
/*        - Values:                                                          */
/*            hpesa_MEASURE_METHOD_MEAS - Initiates and retrieves            */
/*              measurement data from the instrument, using factory default  */
/*              settings.                                                    */
/*            hpesa_MEASURE_METHOD_READ - Initiates and retrieves            */
/*              measurement data from the instrument, using the current      */
/*              setup values.                                                */
/*            hpesa_MEASURE_METHOD_FETCH - Retrieves data from the           */
/*              instrument using the current setup values.  Does not         */
/*              initiate a measurement.                                      */
/*    ViPReal64   emissionBandwidth_Q                                        */
/*        - The value of the emission bandwidth in Hz.                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_EBWMeasure
                        (ViSession instrumentHandle,
                         ViReal64 EBWMeasureTimeout,
                         ViInt16 EBWMeasureType,
                         ViPReal64 emissionBandwidth_Q);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_harmonicDistConfig                                                 */
/*    This method sets up the harmonic distortion measurement operation.     */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle                                           */
/*        - Instrument handle returned from hpesa_init.                      */
/*    ViBoolean   harmonicDistAverageOn                                      */
/*        - Instrument's new harmonic average state.                         */
/*        - Values:                                                          */
/*            VI_TRUE - Averaging on.                                        */
/*            VI_FALSE - Averaging off.                                      */
/*    ViInt16     harmonicDistAverageCount                                   */
/*        - Specifies the number of harmonic measurements that are combined. */
/*    ViInt16     harmonicDistCount                                          */
/*        - The number of harmonics to be measured.                          */
/*    ViInt16     harmonicDistSweepMode                                      */
/*        - Instrument's sweep mode per harmonic.                            */
/*        - Values:                                                          */
/*            hpesa_HARMONIC_SWEEP_AUTO - The sweep time for measuring each  */
/*              harmonic will be set to 200/RBW, where RBW is the resolution */
/*              bandwidth in Hz.                                             */
/*            hpesa_HARMONIC_SWEEP_MANUAL - Allows the user to set the sweep */
/*              time used for the zero span measuring of the harmonics.      */
/*    ViReal64    harmonicDistSweepTime                                      */
/*        - Instrument's new sweep time, in seconds, used to measure each    */
/*          harmonic.  This sweep time is used only for the zero span        */
/*          measuring of the harmonics, and not during the search for the    */
/*          fundamental.  In this case, the sweep time used will be the      */
/*          sweep time used before the measurement began.                    */
/*    ViBoolean   harmonicDistCenterZoomOn                                   */
/*        - Turn off or on the fundamental signal zoom before the frequency  */
/*          count occurs.                                                    */
/*        - Values:                                                          */
/*            VI_TRUE - Fundamental align zoom on.                           */
/*            VI_FALSE - Fundamental align zoom off.                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_harmonicDistConfig
                        (ViSession instrumentHandle,
                         ViBoolean harmonicDistAverageOn,
                         ViInt16 harmonicDistAverageCount,
                         ViInt16 harmonicDistCount,
                         ViBoolean harmonicDistSweepMode,
                         ViReal64 harmonicDistSweepTime,
                         ViBoolean harmonicDistCenterZoomOn);




/*****************************************************************************/
/*                                                                           */
/*  hpesa_harmonicDistConfigQuery                                            */
/*    This method retrieves the current harmonic distortion measurement      */
/*    settings.                                                              */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle                                           */
/*        - Instrument handle returned from hpesa_init.                      */
/*    ViPBoolean  harmonicDistAverageOn_Q                                    */
/*        - Instrument's current harmonic average state.                     */
/*    ViPInt16    harmonicDistAverageCount_Q                                 */
/*        - The current number of harmonic measurements that are combined.   */
/*    ViPInt16    harmonicDistCount_Q                                        */
/*        - The current number of harmonics to be measured.                  */
/*    ViPBoolean  harmonicDistSweepMode_Q                                    */
/*        - Instrument's current sweep mode per harmonic.                    */
/*    ViPReal64   harmonicDistSweepTime_Q                                    */
/*        - Instrument's current sweep time (in seconds)                     */
/*    ViPBoolean  harmonicDistCenterZoomOn_Q                                 */
/*        - Instrument's current signal zoom state.                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_harmonicDistConfigQuery
                        (ViSession instrumentHandle,
                         ViPBoolean harmonicDistAverageOn_Q,
                         ViPInt16 harmonicDistAverageCount_Q,
                         ViPInt16 harmonicDistCount_Q,
                         ViPBoolean harmonicDistSweepMode_Q,
                         ViPReal64 harmonicDistSweepTime_Q,
                         ViPBoolean harmonicDistCenterZoomOn_Q);




/*****************************************************************************/
/*                                                                           */
/*  hpesa_harmonicDistMeasure                                                */
/*    This method retrieves the total harmonic distortion as a percentage    */
/*    and the amplitude and frequency values of the harmonics, based on the  */
/*    method of retrieval (measure, read, or fetch).                         */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle                                           */
/*        - Instrument handle returned from hpesa_init.                      */
/*    ViReal64    harmonicDistMeasureTimeout                                 */
/*        - Temporary timeout value in seconds for measurement.              */
/*    ViInt16     harmonicDistMeasureType                                    */
/*        - The method used to retrieve the harmonic measurements.           */
/*        - Values:                                                          */
/*            hpesa_MEASURE_METHOD_MEAS - Initiates and retrieves            */
/*              measurement data from the instrument, using factory default  */
/*              settings.                                                    */
/*            hpesa_MEASURE_METHOD_READ - Initiates and retrieves            */
/*              measurement data from the instrument, using the current      */
/*              setup values.                                                */
/*            hpesa_MEASURE_METHOD_FETCH - Retrieves data from the           */
/*              instrument using the current setup values.  Does not         */
/*              initiate a measurement.                                      */
/*    ViPReal64   harmonicDistTotal_Q                                        */
/*        - The total harmonic distortion as a percentage.                   */
/*    ViInt16     harmonicDistArraySize                                      */
/*        - The number of frequency and amplitude values to retrieve.  This  */
/*          value should normally be the same as the number of harmonics     */
/*          measured.                                                        */
/*    ViReal64    harmonicDistAmplitude[]                                    */
/*        - An array of the amplitude values of the current harmonics.  The  */
/*          first value is measured in dBm (fundamental); all others are     */
/*          measured in dBc.                                                 */
/*    ViReal64    harmonicDistFrequency[]                                    */
/*        - An array of the frequency values of the current harmonics        */
/*          measured in Hz.                                                  */
/*    ViPInt16    harmonicDistValuesRead_Q                                   */
/*        - The actual number of harmonics measured.                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_harmonicDistMeasure
                        (ViSession instrumentHandle,
                         ViReal64 harmonicDistMeasureTimeout,
                         ViInt16 harmonicDistMeasureType,
                         ViPReal64 harmonicDistTotal_Q,
                         ViInt16 harmonicDistArraySize,
                         ViReal64 _VI_FAR harmonicDistAmplitude[],
                         ViReal64 _VI_FAR harmonicDistFrequency[],
                         ViPInt16 harmonicDistValuesRead_Q);


    /***************************************************/
    /* MISCELLANEOUS CONTROL FUNCTIONS                 *
    /***************************************************/

/*****************************************************************************/
/*                                                                           */
/*  hpesa_couplingSetAuto -                                                  */
/*     The instrument can automatically couple instrument settings together  */
/*     for accurate measurements and optimal range.  This method is used to  */
/*     override the coupling for special measuring needs.                    */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        - Instrument handle returned from hpesa_init()                     */
/*                                                                           */
/*    ViBoolean   autoCouplingOn (in)                                        */
/*        - Select auto couple mode on or off.                               */
/*        - Range:  VI_TRUE (on) or VI_FALSE (off)                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_couplingSetAuto
                        (ViSession instrumentHandle,
                         ViBoolean autoCouplingOn);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_detectorSetMode -                                                  */
/*     This method specifies the detection mode.                             */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        - Instrument handle returned from hpesa_init()                     */
/*                                                                           */
/*    ViInt16    detectorMode (in)                                           */
/*        - Select the detector mode.                                        */
/*        - Range:  hpesa_DETECTOR_POSITIVEPEAK - Positive peak detection    */
/*                        displays the highest sample taken during the       */
/*                        interval being displayed.                          */
/*                  hpesa_DETECTOR_SAMPLE - Sample detection displays the    */
/*                        first sample taken during the interval being       */
/*                        displayed.                                         */
/*                  hpesa_DETECTOR_NEGATIVEPEAK - Negative peak detection    */
/*                        displays the lowest sample taken during the        */
/*                        interval being displayed.                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_detectorSetMode
                        (ViSession instrumentHandle,
                         ViInt16 detectorMode);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_preAmpSetOn -                                                      */
/*     This method turns the internal preamp on or off.                      */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        - Instrument handle returned from hpesa_init()                     */
/*                                                                           */
/*    ViBoolean   preAmpOn (in)                                              */
/*        - Turn preamp on or off.  .                                        */
/*        - Range:  VI_TRUE (on) or VI_FALSE (off)                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_preAmpSetOn
                        (ViSession instrumentHandle,
                         ViBoolean preAmpOn);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_preselectorCenter -                                                */
/*     This method centers the preselector tracking to maximum amplitude of  */
/*     the signal at the specified marker by minimizing the loss through     */
/*     the filter.                                                           */
/*                                                                           */
/*     Use the function "hpesa_opc_q" to test instrument for completion of   */
/*     preselector centering                                                 */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession   instrumentHandle (in)                                      */
/*        Instrument handle returned from hpesa_init()                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_preselectorCenter
                        (ViSession instrumentHandle);


    /***************************************************/
    /* INSTRUMENT SETTING VERIFICATION FUNCTIONS       *
    /***************************************************/


/*****************************************************************************/
/*                                                                           */
/*  hpesa_EnableVerification                                                 */
/*                                                                           */
/*      This function enables or disables checking of various instrument     */
/*      settings.                                                            */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 verifyID                                                       */
/*                                                                           */
/*      The ID of the instrument setting that is to be affected. Possible    */
/*      values are:                                                          */
/*      0 = hpesa_VERIFY_APPLICATION.                                        */
/*                                                                           */
/*      This verification being enabled will result in the Instrument being  */
/*      queried for it's current application when all 'measurement'          */
/*      functions are called. (SPECTRUM AND WAVEFORM are exceptions.) I.e.   */
/*      CHP for CDMAOne or TXP for GSM.                                      */
/*                                                                           */
/*    ViBoolean verifyOn                                                     */
/*                                                                           */
/*      Sets the state of the verification.                                  */
/*      VI_TRUE - sets the specified verification to on.                     */
/*      VI_FALSE - sets the specified verification to off                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_EnableVerification
                       (ViSession instrumentHandle,
                        ViInt16 verifyID,
                        ViBoolean verifyOn);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GetVerification                                                    */
/*                                                                           */
/*      This function returns the state of the specified verify ID.          */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 verifyIDQ                                                      */
/*                                                                           */
/*      Determines which instrument ID verification to retreive.  Possible   */
/*      values are:                                                          */
/*      0 = hpesa_VERIFY_APPLICATION                                         */
/*                                                                           */
/*    ViPBoolean verifyOnQuery                                               */
/*                                                                           */
/*      Returns the State as specified by the verify IDQ.                    */
/*      0 = hpesa_VERIFY_STATE_OFF                                           */
/*      1 = hpesa_VERIFY_STATE_ON                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GetVerification
                       (ViSession instrumentHandle,
                        ViInt16 verifyIDQ,
                        ViPBoolean verifyOnQuery);


    /***************************************************/
    /* INSTRUMENT APPLICATION SELECTION FUNCTIONS      *
    /***************************************************/


/*****************************************************************************/
/*                                                                           */
/*  hpesa_InstrumentSetApplication                                           */
/*                                                                           */
/*      Select the measurement Application by enumerated choice. The         */
/*      available choices depend upon which modes are installed in the       */
/*      instrument. Use "hpesa_InstrumentGetAllApplications" to determine    */
/*      applications installed.                                              */
/*                                                                           */
/*      If you are using the SCPI status registers and the analyzer mode is  */
/*      changed, the status bits should be read, and any errors resolved,    */
/*      prior to switching modes. Error conditions that exist prior to       */
/*      switching modes cannot be detected using the condition registers     */
/*      after the mode change. This is true unless they recur after the mode */
/*      change, although transitions of these conditions can be detected     */
/*      using the event registers.                                           */
/*                                                                           */
/*      Changing modes resets all SCPI status registers and mask registers   */
/*      to their power-on defaults. Hence, any event or condition register   */
/*      masks must be re-established after a mode change. Also note that the */
/*      power up status bit is set by any mode change, since that is the     */
/*      default state after power up.                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 instrumentApplication                                          */
/*                                                                           */
/*      Application mode to set.                                             */
/*                                                                           */
/*      hpesa_APPLICATION_SA - Spectrum analyzer mode                        */
/*      hpesa_ APPLICATION _CDMA - CDMA One mode                             */
/*      hpesa_ APPLICATION _GSM - GSM Mode                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_InstrumentSetApplication
                       (ViSession instrumentHandle,
                        ViInt16 instrumentApplication);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_InstrumentGetApplication                                           */
/*                                                                           */
/*      This method returns the current measurement mode.                    */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 instrumentApplicationQuery                                    */
/*                                                                           */
/*      Returns the current instrument application.                          */
/*                                                                           */
/*      0 = hpesa_APPLICATION_SA                                             */
/*      1 = hpesa_APPLICATION_CDMA                                           */
/*      2 = hpesa_APPLICATION_GSM                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_InstrumentGetApplication
                       (ViSession instrumentHandle,
                        ViPInt16 instrumentApplicationQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_InstrumentGetAllApplications                                       */
/*                                                                           */
/*      This method returns a comma separated list which contains the names  */
/*      of all installed applications.                                       */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 applicationsMaxSizeQ                                           */
/*                                                                           */
/*      The maximum size of the applicationsQuery array.                     */
/*                                                                           */
/*    ViPString applicationsQuery                                            */
/*                                                                           */
/*      Returns a comma separated list of all modes installed on the         */
/*      instrument.                                                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_InstrumentGetAllApplications
                       (ViSession instrumentHandle,
                        ViInt32 applicationsMaxSizeQ,
                        ViPString applicationsQuery);


    /***************************************************/
    /* CDMA ONE MODE SETUP FUNCTIONS                   *
    /***************************************************/


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_TriggerSetExternal                                           */
/*                                                                           */
/*      This method sets the CDMA One external trigger configuration.        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 cdma1TrigExternalDelay                                        */
/*                                                                           */
/*      Sets the trigger delay, in seconds, when using the external trigger  */
/*      input. Instrument range is 0 ns to 429 s. Anything less than 300 ns  */
/*      is equal to off. Default is 0 s.                                     */
/*                                                                           */
/*    ViInt16 cdma1TrigExternalSlope                                         */
/*                                                                           */
/*      Sets the trigger slope when using the external trigger input.        */
/*      Instrument default is positive.                                      */
/*                                                                           */
/*      hpesa_TRIG_SLOPE_POS - Sets the external trigger to positive.        */
/*                                                                           */
/*      hpesa_TRIG_SLOPE_NEG - Sets the external trigger to negative.        */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_TriggerSetExternal
                       (ViSession instrumentHandle,
                        ViReal64 cdma1TrigExternalDelay,
                        ViInt16 cdma1TrigExternalSlope);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_TriggerGetExternal                                           */
/*                                                                           */
/*      This method retrieves the current CDMA One external trigger          */
/*      configuration.                                                       */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPReal64 cdma1TrigExternalDelayQuery                                  */
/*                                                                           */
/*      Returns current trigger delay, in seconds, when using the external   */
/*      trigger input. Instrument range is 0 ns to 429 s. Anything less than */
/*      300 ns is equal to off. Default is 0 s.                              */
/*                                                                           */
/*    ViInt16 cdma1TrigExternalSlopeQuery                                    */
/*                                                                           */
/*      Returns the current trigger slope when using the external trigger    */
/*      input. Instrument default is positive.                               */
/*                                                                           */
/*      0 = hpesa_TRIG_SLOPE_POS                                             */
/*      1 = hpesa_TRIG_SLOPE_NEG                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_TriggerGetExternal
                       (ViSession instrumentHandle,
                        ViPReal64 cdma1TrigExternalDelayQuery,
                        ViPInt16 cdma1TrigExternalSlopeQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_TriggerSetRFBurst                                            */
/*                                                                           */
/*      This method sets up the CDMA One RF Burst (wideband) trigger.        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 cdma1TrigRFBurstDelay                                         */
/*                                                                           */
/*      Sets the trigger delay, in seconds, when using the RF burst          */
/*      (wideband) trigger. Instrument range is 0 ns to 429 s. Anything less */
/*      than 300 ns is equal to off. Default is 0 s.                         */
/*                                                                           */
/*    ViReal64 cdma1TrigRFBurstLevel                                         */
/*                                                                           */
/*      Sets the trigger level, in db, when using the RF burst (wideband)    */
/*      trigger. The value is relative to the peak of the signal. RF Burst   */
/*      is also known as RF Envelope. Instrument range is -25 to 0 dB, and   */
/*      default is - 6dB.                                                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_TriggerSetRFBurst
                       (ViSession instrumentHandle,
                        ViReal64 cdma1TrigRFBurstDelay,
                        ViReal64 cdma1TrigRFBurstLevel);
                        


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_TriggerGetRFBurst                                            */
/*                                                                           */
/*      This method retrieves the current CDMA One RF Burst (wideband)       */
/*      trigger configuration.                                               */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPReal64 cdma1TrigRFBurstDelayQuery                                   */
/*                                                                           */
/*      Returns the current trigger delay, in seconds, when using the RF     */
/*      burst (wideband) trigger.                                            */
/*                                                                           */
/*    ViPReal64 cdma1TrigRFBurstLevelQuery                                   */
/*                                                                           */
/*      Returns the current trigger level, in dB, when using the RF burst    */
/*      (wideband) trigger. The value is relative to the peak of the signal. */
/*      RF Burst is also known as RF Envelope.                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_TriggerGetRFBurst
                       (ViSession instrumentHandle,
                        ViPReal64 cdma1TrigRFBurstDelayQuery,
                        ViPReal64 cdma1TrigRFBurstLevelQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_TriggerSetFrameTimer                                         */
/*                                                                           */
/*      This method sets the CDMA One Frame Timer trigger configuration.     */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 cdma1TrigFrameTimerPeriod                                     */
/*                                                                           */
/*      Sets the frame period, in seconds, when using the external frame     */
/*      timer trigger. If the traffic rate is changed, the value of the      */
/*      frame period is initialized to the preset value. The instrument      */
/*      range is 50 us to 13.65333 s, and the default is 26.6666667 ms.      */
/*                                                                           */
/*    ViReal64 cdma1TrigFrameTimerDelay                                      */
/*                                                                           */
/*      Sets the trigger delay, in seconds, to be used in zero span          */
/*      measurements to adjust the active burst within a mask. Use positive  */
/*      values to achieve trigger delay (that is, to measure later than the  */
/*      trigger source event) and use negative values to achieve pre-trigger */
/*      (that is, to measure earlier than the trigger source event).         */
/*      Instrument range is 0 ns to 429. Anything less than 300 ns is equal  */
/*      to off. Default is 0 s.                                              */
/*                                                                           */
/*    ViInt16 cdma1TrigFrameTimerSlope                                       */
/*                                                                           */
/*      Sets the trigger slope when using the external frame timer trigger.  */
/*      Instrument default is positive.                                      */
/*                                                                           */
/*      hpesa_TRIG_SLOPE_POS - Sets the exteral frame timer slope to         */
/*      positive.                                                            */
/*                                                                           */
/*      hpesa_TRIG_SLOPE_NEG - Sets the external frame timer slope to        */
/*      negative.                                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_TriggerSetFrameTimer
                       (ViSession instrumentHandle,
                        ViReal64 cdma1TrigFrameTimerPeriod,
                        ViReal64 cdma1TrigFrameTimerDelay,
                        ViInt16 cdma1TrigFrameTimerSlope );


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_TriggerGetFrameTimer                                         */
/*                                                                           */
/*      This method retrieves the CDMA One Frame Timer trigger configuration.*/
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPReal64 cdma1TrigFramTimerPerQuery                                   */
/*                                                                           */
/*      Returns the current frame period, in seconds, when using the         */
/*      external frame timer trigger. If the traffic rate is changed, the    */
/*      value of the frame period is initialized to the preset value. The    */
/*      instrument range is 50 us to 13.65333 s, and the default is          */
/*      26.6666667 ms.                                                       */
/*                                                                           */
/*    ViPReal64 cdma1TrigFramTimerDelQuery                                   */
/*                                                                           */
/*      Returns the current trigger delay, in seconds, to be used in zero    */
/*      span measurements to adjust the active burst within a mask. Use      */
/*      positive values to achieve trigger delay (that is, to measure later  */
/*      than the trigger source event) and use negative values to achieve    */
/*      pre-trigger (that is, to measure earlier than the trigger source     */
/*      event). Instrument range is 0 ns to 429. Anything less than 300 ns   */
/*      is equal to off. Default is 0 s.                                     */
/*                                                                           */
/*    ViPInt16 cdma1TrigFramTimerSlpQuery                                    */
/*                                                                           */
/*      Returns the current trigger slope when using the external frame      */
/*      timer trigger.                                                       */
/*      0 = hpesa_TRIG_SLOPE_POS                                             */
/*      1 = hpesa_TRIG_SLOPE_NEG                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_TriggerGetFrameTimer
                       (ViSession instrumentHandle,
                        ViPReal64 cdma1TrigFramTimerPerQuery,
                        ViPReal64 cdma1TrigFramTimerDelQuery,
                        ViPInt16 cdma1TrigFramTimerSlpQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_FrequencySetChannel                                          */
/*                                                                           */
/*      This method sets the RF channel frequency for CDMA One measurements. */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 cdma1ChannelFrequency                                         */
/*                                                                           */
/*      Sets the new channel frequency, in Hz. Instrument range is the same  */
/*      as the full frequency span, and the default is 870.03 MHz (radio     */
/*      device type base) for cdmaOne measurements.                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_FrequencySetChannel
                       (ViSession instrumentHandle,
                        ViReal64 cdma1ChannelFrequency );


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_FrequencyGetChannel                                          */
/*                                                                           */
/*      This method retrieves the RF channel frequency for CDMA One          */
/*      measurements.                                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPReal64 cdma1ChannelFrequencyQuery                                   */
/*                                                                           */
/*      Returns the current channel frequency, in Hz.                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_FrequencyGetChannel
                       (ViSession instrumentHandle,
                        ViPReal64 cdma1ChannelFrequencyQuery);


    /***************************************************/
    /* CDMA ONE CHP FUNCTIONS                          *
    /***************************************************/


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CHPConfigure                                                 */
/*                                                                           */
/*      This method sets up the instrument for CDMA One channel power        */
/*      measurements (CHP).                                                  */
/*                                                                           */
/*      This method stops the current measurement and sets up the instrument */
/*      for the Channel Power (CHP) measurement using the factory default    */
/*      instrument settings. It does not initiate the taking of measurement  */
/*      data. This method also turns the averaging function on and sets the  */
/*      number of averages to 10 for all measurements.                       */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CHPConfigure
                       (ViSession instrumentHandle);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CHPMeasure                                                   */
/*                                                                           */
/*      This method measures the total RMS power in a specified integration  */
/*      bandwidth. User must be in the cdmaOne mode to use this function.    */
/*                                                                           */
/*      The user can select one of three types of measurement modes:         */
/*      Measure, Read, or Fetch.                                             */
/*                                                                           */
/*      This method returns the CHP measurement results specified by the     */
/*      "cdma1CHPResultsType" parameter as a real array.                     */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 cdma1CHPMeasureTimeout                                        */
/*                                                                           */
/*      This value temporarily changes the VISA timeout setting for the      */
/*      duration of the instrument measurement. The user needs to set this   */
/*      value to an appropriate time that will be longer then the expected   */
/*      measurement time. Units are in seconds.                              */
/*                                                                           */
/*    ViInt16 cdma1CHPMeasureType                                            */
/*                                                                           */
/*      Selects measurement type. Note that for hpesa_FETCH type the         */
/*      instrument should already be in this measurement mode else the       */
/*      function will time out. User can select this measurement mode by     */
/*      first using the "MEASURE" or "READ" types or call the                */
/*      hpesa_cdma1_CHPConfigure function.                                   */
/*                                                                           */
/*      hpesa_MEASURE - Stops current measurement, changes to this selected  */
/*      measurement, sets up instrument using factory defaults, makes        */
/*      measurement, returns measurement results.                            */
/*                                                                           */
/*      hpesa_READ - Stops current measurement,  changes to this selected    */
/*      measurement, uses instrument settings from last measurement of this  */
/*      type, makes measurement, returns measurements results.               */
/*                                                                           */
/*      hpesa_FETCH - Returns measurements results only. No new measurement  */
/*      is initiated.                                                        */
/*                                                                           */
/*    ViPReal64 cdma1CHPChannelPowerQ                                        */
/*                                                                           */
/*      Returns the total channel power in the specified integration         */
/*      bandwidth (dBm/<integration bandwidth> Hz).                          */
/*                                                                           */
/*    ViPReal64 cdma1CHPPowSpectDensityQ                                     */
/*                                                                           */
/*      Returns the power (in dBm/Hz) in the specified integration bandwidth.*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CHPMeasure
                       (ViSession instrumentHandle,
                        ViReal64 cdma1CHPMeasureTimeout,
                        ViInt16 cdma1CHPMeasureType,
                        ViPReal64 cdma1CHPChannelPowerQ,
                        ViPReal64 cdma1CHPPowSpectDensityQ);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CHPMeasureTrace                                              */
/*                                                                           */
/*      This method measures the total RMS power in a specified integration  */
/*      bandwidth. User must be in the cdmaOne mode to use this function.    */
/*                                                                           */
/*      User can select one of three types of measurement modes: Measure,    */
/*      Read, or Fetch.                                                      */
/*                                                                           */
/*      This method returns the CHP measurement results specified by the     */
/*      "cdma1CHPResultsType" parameter as a real array.                     */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 cdma1CHPMeasureTraceTimeout                                   */
/*                                                                           */
/*      This value temporarily changes the VISA timeout setting for the      */
/*      duration of the instrument measurement. The user needs to set this   */
/*      value to an appropriate time that will be longer then the expected   */
/*      measurement time. Units are in seconds.                              */
/*                                                                           */
/*    ViInt16 cdma1CHPMeasureTraceType                                       */
/*                                                                           */
/*      Selects measurement type. Note that for hpesa_FETCH type the         */
/*      instrument should already be in this measurement mode else the       */
/*      function will time out. User can select this measurement mode by     */
/*      first using the "MEASURE" or "READ" types or call the                */
/*      hpesa_cdma1_CHPConfigure function.                                   */
/*                                                                           */
/*      hpesa_MEASURE - Stops current measurement, changes to this selected  */
/*      measurement, sets up instrument using factory defaults, makes        */
/*      measurement, returns measurement results.                            */
/*                                                                           */
/*      hpesa_READ - Stops current measurement,  changes to this selected    */
/*      measurement, uses instrument settings from last measurement of this  */
/*      type, makes measurement, returns measurements results.               */
/*                                                                           */
/*      hpesa_FETCH - Returns measurements results only. No new measurement  */
/*      is initiated.                                                        */
/*                                                                           */
/*    ViInt16 cdma1CHPResultsType                                            */
/*                                                                           */
/*      Determines the format of the data returned in cdma1CHPDataResults.   */
/*                                                                           */
/*      RESULTS_TYPE_0: Not supported by instrument.                         */
/*                                                                           */
/*      RESULTS_TYPE_1:                                                      */
/*      Returns 2 scalar results:                                            */
/*      1. Channel Power is the total channel power in the specified         */
/*      integration bandwidth (dBm/<integration bandwidth> Hz).              */
/*      2. Power Spectral Density is the power (in dBm/Hz) in the specified  */
/*      integration bandwidth.                                               */
/*                                                                           */
/*    ViAReal64 cdma1CHPDataResults                                          */
/*                                                                           */
/*      Trace data returned by measurement, formatted according to           */
/*      cdma1CHPResultsType.                                                 */
/*                                                                           */
/*    ViInt32 cdma1CHPDataResultsMaxSz                                       */
/*                                                                           */
/*      Specifies the maximum size of CHPDataResults array.                  */
/*                                                                           */
/*    ViPInt32 cdma1CHPDataResultsSz                                         */
/*                                                                           */
/*      Returns the actual size of CHPDataResults array.                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CHPMeasureTrace
                       (ViSession instrumentHandle,
                        ViReal64 cdma1CHPMeasureTraceTimeout,
                        ViInt16 cdma1CHPMeasureTraceType,
                        ViInt16 cdma1CHPResultsType ,
                        ViAReal64 cdma1CHPDataResults ,
                        ViInt32 cdma1CHPDataResultsMaxSz,
                        ViPInt32 cdma1CHPDataResultsSz);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CHPSetAveraging                                              */
/*                                                                           */
/*      This method sets up averaging for CDMA One channel power (CHP)       */
/*      measurements. User can control the number of averages, turn on and   */
/*      off averaging, and set the terminal control mode for averaging.      */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 cdma1CHPAverageCount                                           */
/*                                                                           */
/*      Sets the number of data acquisitions that will be averaged. After    */
/*      the specified number of average counts, the averaging mode (terminal */
/*      control) setting determines the averaging action. The instrument     */
/*      range is 1 to 1000.                                                  */
/*                                                                           */
/*    ViBoolean cdma1CHPAverageState                                         */
/*                                                                           */
/*      Turns averaging on or off.                                           */
/*                                                                           */
/*      VI_TRUE - Turns averaging on.                                        */
/*      VI_FALSE - Turns averaging off.                                      */
/*                                                                           */
/*    ViInt16 cdma1CHPAverageMode                                            */
/*                                                                           */
/*      Selects the type of terminal control for averaging.                  */
/*                                                                           */
/*      hpesa_AVERAGE_EXPONENTIAL  - Each successive data acquisition after  */
/*      the average count is reached, is exponentially weighted and combined */
/*      with the existing average.                                           */
/*                                                                           */
/*      hpesa_AVERAGE_REPEAT - After reaching the average count, the         */
/*      averaging is reset and a new average is started                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CHPSetAveraging
                       (ViSession instrumentHandle,
                        ViInt32 cdma1CHPAverageCount,
                        ViBoolean cdma1CHPAverageState,
                        ViInt16 cdma1CHPAverageMode);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CHPGetAveraging                                              */
/*                                                                           */
/*      This method retrieves the averaging setup for the CDMA One channel   */
/*      power (CHP) measurements.                                            */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt32 cdma1CHPAverageCountQuery                                     */
/*                                                                           */
/*      Returns the current number of data acquisitions being averaged for   */
/*      CDMA One CHP measurements.                                           */
/*                                                                           */
/*    ViPBoolean cdma1CHPAverageStateQuery                                   */
/*                                                                           */
/*      Returns the current average state for CDMA One CHP measurements.     */
/*                                                                           */
/*      0 = VI_FALSE                                                         */
/*      1 = VI_TRUE                                                          */
/*                                                                           */
/*    ViPInt16 cdma1CHPAverageModeQuery                                      */
/*                                                                           */
/*      Returns the current type of terminal control for CDMA One CHP        */
/*      averaging.                                                           */
/*      0 = hpesa_AVERAGE_EXPONENTIAL                                        */
/*      1 = hpesa_AVERAGE_REPEAT                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CHPGetAveraging
                       (ViSession instrumentHandle,
                        ViPInt32 cdma1CHPAverageCountQuery,
                        ViPBoolean cdma1CHPAverageStateQuery,
                        ViPInt16 cdma1CHPAverageModeQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CHPSetIntegrationBW                                          */
/*                                                                           */
/*      This method sets the integration bandwidth for CDMA One channel      */
/*      power (CHP) measurements.                                            */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 cdma1CHPIntegrationBW                                         */
/*                                                                           */
/*      Sets the new integration bandwidth, in Hz. The instrument's range is */
/*      1 kHz to instrument span maximum, and the default is 1.23 MHz.       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CHPSetIntegrationBW
                       (ViSession instrumentHandle,
                        ViReal64 cdma1CHPIntegrationBW);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CHPGetIntegrationBW                                          */
/*                                                                           */
/*      This method retrieves the integration bandwidth for CDMA One channel */
/*      power (CHP) measurements.                                            */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPReal64 cdma1CHPIntegrationBWQuery                                   */
/*                                                                           */
/*      Returns the current integration bandwidth for CDMA One channel power */
/*      (CHP) measurements, in Hz                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CHPGetIntegrationBW
                       (ViSession instrumentHandle,
                        ViPReal64 cdma1CHPIntegrationBWQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CHPSetFrequencySpan                                          */
/*                                                                           */
/*      This method sets the span frequency for CDMA One channel power (CHP) */
/*      measurements.  If resolution bandwidth is set to a value less than 1 */
/*      kHz, then the maximum span is limited to 5 MHz.                      */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 cdma1CHPFrequencySpan                                         */
/*                                                                           */
/*      Sets the new span frequency, in Hz. Instrument range is the          */
/*      intergration bandwidth (IBW) to IBW x 400 and the default is 2 MHz.  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CHPSetFrequencySpan
                       (ViSession instrumentHandle,
                        ViReal64 cdma1CHPFrequencySpan);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CHPGetFrequencySpan                                          */
/*                                                                           */
/*      This method retrieves the current span frequency for CDMA One        */
/*      channel power (CHP) measurements.                                    */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPReal64 cdma1CHPFrequencySpanQuery                                   */
/*                                                                           */
/*      Returns the current span frequency, in Hz.                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CHPGetFrequencySpan
                       (ViSession instrumentHandle,
                        ViPReal64 cdma1CHPFrequencySpanQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CHPSetTriggerSource                                          */
/*                                                                           */
/*      This method sets the trigger source for CDMA One channel power (CHP) */
/*      measurements.                                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 cdma1CHPTriggerSource                                          */
/*                                                                           */
/*      Sets the trigger source for CDMA One channel power measurements.     */
/*                                                                           */
/*      hpesa_CHP_TRIG_IMMEDIATE - the next data acquisition is immediately  */
/*      taken (also called Free Run).                                        */
/*                                                                           */
/*      hpesa_CHP_TRIG _EXTERNAL - rear panel external trigger input.        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CHPSetTriggerSource
                       (ViSession instrumentHandle,
                        ViInt16 cdma1CHPTriggerSource);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CHPGetTriggerSource                                          */
/*                                                                           */
/*      This method sets the trigger source for CDMA One channel power (CHP) */
/*      measurements.                                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 cdma1CHPTriggerSourceQuery                                    */
/*                                                                           */
/*      Returns the current trigger source for CDMA One channel power        */
/*      measurements.                                                        */
/*                                                                           */
/*      0 = hpesa_CHP_TRIG_IMMEDIATE                                         */
/*      1 = hpesa_CHP_TRIG_EXTERNAL                                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CHPGetTriggerSource
                       (ViSession instrumentHandle,
                        ViPInt16 cdma1CHPTriggerSourceQuery);


    /***************************************************/
    /* CDMA ONE RHO FUNCTIONS                          *
    /***************************************************/


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_RHOConfigure                                                 */
/*                                                                           */
/*      This method sets up the instrument for CDMA One modulation accuracy  */
/*      measurements (RHO).                                                  */
/*                                                                           */
/*      This function stops the current measurement and sets up the          */
/*      instrument for the Modulation Accuracy (RHO) measurement using the   */
/*      factory default instrument settings. It does not initiate the taking */
/*      of measurement data. This command also turns the averaging function  */
/*      on and sets the number of averages to 10 for all measurements.       */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_RHOConfigure
                       (ViSession instrumentHandle);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_RHOMeasure                                                   */
/*                                                                           */
/*      This method measures the CDMA One modulation accuracy of the         */
/*      transmitter by checking the magnitude and phase error and the EVM    */
/*      (error vector magnitude). User must be in the cdmaOne application    */
/*      mode to use these commands.                                          */
/*                                                                           */
/*      User can select one of three types of measurement modes: Measure,    */
/*      Read, or Fetch.                                                      */
/*                                                                           */
/*      This function returns 7 RHO measurement scalar results, as described */
/*      in the parameters below.                                             */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 cdma1RHOMeasureTimeout                                        */
/*                                                                           */
/*      This value temporarily changes the VISA timeout setting for the      */
/*      duration of the instrument measurement. The user needs to set this   */
/*      value to an appropriate time that will be longer then the expected   */
/*      measurement time. Units are in seconds.                              */
/*                                                                           */
/*    ViInt16 cdma1RHOMeasureType                                            */
/*                                                                           */
/*      Selects measurement type. Note that for hpesa_FETCH type the         */
/*      instrument should already be in this measurement mode else the       */
/*      function will time out. User can select this measurement mode by     */
/*      first using the "MEASURE" or "READ" types or call the                */
/*      hpesa_cdma1_RHOConfigure function.                                   */
/*                                                                           */
/*      hpesa_MEASURE - Stops current measurement, changes to this selected  */
/*      measurement, sets up instrument using factory defaults, makes        */
/*      measurement, returns measurement results.                            */
/*                                                                           */
/*      hpesa_READ - Stops current measurement,  changes to this selected    */
/*      measurement, uses instrument settings from last measurement of this  */
/*      type, makes measurement, returns measurements results.               */
/*                                                                           */
/*      hpesa_FETCH - Returns measurements results only. No new measurement  */
/*      is initiated.                                                        */
/*                                                                           */
/*    ViPReal64 cdma1RHOModAccuracyQuery                                     */
/*                                                                           */
/*      Returns RHO (no units) representing the correlation of the measured  */
/*      power compared to the ideal pilot channel. The calculation is        */
/*      performed after the complimentary filter, so it is IS95 compliant.   */
/*      It is performed at the decision points in the pilot waveform. If     */
/*      averaging is on, this is the average of the individual RMS           */
/*      measurements.                                                        */
/*                                                                           */
/*    ViPReal64 cdma1RHOTimeOffsetQuery                                      */
/*                                                                           */
/*      Returns the time offset (with units of seconds) which is the time    */
/*      delay of the even second clock with respect to the start of the      */
/*      short code PN sequences, at offsets from the 15 zeros in the         */
/*      characteristic phase of the sequence.                                */
/*                                                                           */
/*    ViPReal64 cdma1RHOFreqErrorQuery                                       */
/*                                                                           */
/*      Returns the frequency error of the measured signal, with units of    */
/*      Hz. This is based on the linear best fit of the uncorrected measured */
/*      phase.                                                               */
/*                                                                           */
/*    ViPReal64 cdma1RHOCarrFeedthruQuery                                    */
/*                                                                           */
/*      Returns the carrier feedthrough has units of dB and is the DC error  */
/*      offset of I and Q, from the origin.                                  */
/*                                                                           */
/*    ViPReal64 cdma1RHOErrVectMagQuery                                      */
/*                                                                           */
/*      Returns the error vector magnitude (EVM) as units of percent. The    */
/*      calculation is based on the composite of the phase error and         */
/*      magnitude error, between the measured signal and the ideal pilot     */
/*      channel. It is performed after the complimentary filter, which       */
/*      removes the intersymbol interference in the modulated data. If       */
/*      averaging is on, this is the average of the individual RMS           */
/*      measurements.                                                        */
/*                                                                           */
/*    ViPReal64 cdma1RHOMagErrorQuery                                        */
/*                                                                           */
/*      Returns the magnitude error (with units of percent), which is the    */
/*      RMS error between the measured (compensated) magnitude and the ideal */
/*      magnitude. This is performed after the complimentary filter, which   */
/*      removes the inter-symbol interference in the modulated data. If      */
/*      averaging is on, this is the average of the individual RMS           */
/*      measurements.                                                        */
/*                                                                           */
/*    ViPReal64 cdma1RHOPhaseErrorQuery                                      */
/*                                                                           */
/*      Returns the phase error (with units in percent), which is the RMS    */
/*      phase error between the measured phase and the ideal phase. The      */
/*      calculation is performed after the complimentary filter, which       */
/*      removes the intersymbol interference in the modulated data. If       */
/*      averaging is on, this is the average of the individual RMS           */
/*      measurements.                                                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_RHOMeasure
                       (ViSession instrumentHandle,
                        ViReal64 cdma1RHOMeasureTimeout,
                        ViInt16 cdma1RHOMeasureType,
                        ViPReal64 cdma1RHOModAccuracyQuery,
                        ViPReal64 cdma1RHOTimeOffsetQuery,
                        ViPReal64 cdma1RHOFreqErrorQuery,
                        ViPReal64 cdma1RHOCarrFeedthruQuery,
                        ViPReal64 cdma1RHOErrVectMagQuery,
                        ViPReal64 cdma1RHOMagErrorQuery,
                        ViPReal64 cdma1RHOPhaseErrorQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_RHOMeasureTrace                                              */
/*                                                                           */
/*      This method measures the CDMA One modulation accuracy of the         */
/*      transmitter by checking the magnitude and phase error and the EVM    */
/*      (error vector magnitude). User must be in the cdmaOne application    */
/*      mode to use these commands.                                          */
/*                                                                           */
/*      User can select one of three types of measurement modes: Measure,    */
/*      Read, or Fetch.                                                      */
/*                                                                           */
/*      Function returns the ACPR measurement results specified by the       */
/*      "cdma1RHOResultsType" parameter as a real array.                     */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 cdma1RHOMeasureTraceTimeout                                   */
/*                                                                           */
/*      This value temporarily changes the VISA timeout setting for the      */
/*      duration of the instrument measurement. The user needs to set this   */
/*      value to an appropriate time that will be longer then the expected   */
/*      measurement time. Units are in seconds.                              */
/*                                                                           */
/*    ViInt16 cdma1RHOMeasureTraceType                                       */
/*                                                                           */
/*      Selects measurement type. Note that for hpesa_FETCH type the         */
/*      instrument should already be in this measurement mode else the       */
/*      function will time out. User can select this measurement mode by     */
/*      first using the "MEASURE" or "READ" types or call the                */
/*      hpesa_cdma1_RHOConfigure function.                                   */
/*                                                                           */
/*      hpesa_MEASURE - Stops current measurement, changes to this selected  */
/*      measurement, sets up instrument using factory defaults, makes        */
/*      measurement, returns measurement results.                            */
/*                                                                           */
/*      hpesa_READ - Stops current measurement,  changes to this selected    */
/*      measurement, uses instrument settings from last measurement of this  */
/*      type, makes measurement, returns measurements results.               */
/*                                                                           */
/*      hpesa_FETCH - Returns measurements results only. No new measurement  */
/*      is initiated.                                                        */
/*                                                                           */
/*    ViInt16 cdma1RHOResultsType                                            */
/*                                                                           */
/*      Determines the format of the values returned in                      */
/*      cdma1_RHODataResults.                                                */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_0:                                                */
/*      Returns unprocessed I/Q trace in volts. The I values are listed      */
/*      first in each pair, using the 0 through even-indexed values. The Q   */
/*      values are the odd-indexed values.                                   */
/*      The standard sample rate is 4.9152 MHz and the trace length is       */
/*      determined by the current measurement interval.                      */
/*      The number of trace points depends on the current measurement        */
/*      interval and points per chip settings.                               */
/*      The numbers are sent in the following order:                         */
/*      In-phase (I) sample, of symbol 0 decision point                      */
/*      Quadrature-phase (Q) sample, of symbol 0 decision point              */
/*      ...                                                                  */
/*      In-phase (I) sample, of symbol 1 decision point                      */
/*      Quadrature-phase (Q) sample, of symbol 1 decision point              */
/*    E_NE.                                                                  */
/*      The trace can be interpolated to 1, 2, 4 points/chip selected with   */
/*      the display Points/Chip softkey or the NSPC remote command.          */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_1:                                                */
/*      Returns 7 floating point numbers, in the following order:            */
/*      1. Rho (no units) represents the correlation of the measured power   */
/*      compared to the ideal pilot channel. The calculation is performed    */
/*      after the complimentary filter, so it is IS95 compliant. It is       */
/*      performed at the decision points in the pilot waveform. If averaging */
/*      is on, this is the average of the individual RMS measurements.       */
/*      2. Time offset (with units of seconds) is the time delay of the even */
/*      second clock with respect to the start of the short code PN          */
/*      sequences, at offsets from the 15 zeros in the characteristic phase  */
/*      of the sequence.                                                     */
/*      3. Frequency error of the measured signal, with units of Hz. This is */
/*      based on the linear best fit of the uncorrected measured phase.      */
/*      4. Carrier feedthrough has units of dB and is the DC error offset of */
/*      I and Q, from the origin.                                            */
/*      5. EVM has units of percent. The calculation is based on the         */
/*      composite of the phase error and magnitude error, between the        */
/*      measured signal and the ideal pilot channel. It is performed after   */
/*      the complimentary filter, which removes the intersymbol interference */
/*      in the modulated data. If averaging is on, this is the average of    */
/*      the individual RMS measurements.                                     */
/*      6. Magnitude error (with units of percent) is the RMS error between  */
/*      the measured (compensated) magnitude and the ideal magnitude. This   */
/*      is performed after the complimentary filter, which removes the       */
/*      inter-symbol interference in the modulated data. If averaging is on, */
/*      this is the average of the individual RMS measurements.              */
/*      7. Phase error (with units in percent) is the RMS phase error        */
/*      between the measured phase and the ideal phase. The calculation is   */
/*      performed after the complimentary filter, which removes the          */
/*      intersymbol interference in the modulated data. If averaging is on,  */
/*      this is the average of the individual RMS measurements.              */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_2:                                                */
/*      Returns a trace of the error vector magnitude (EVM in percent. The   */
/*      first value is the chip 0 decision point. The trace is interpolated  */
/*      for the currently selected points/chips displayed on the front       */
/*      panel. The number of trace points depends on the current measurement */
/*      interval setting.                                                    */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_3:                                                */
/*      Returns a trace of the magnitude in percent. The first value is the  */
/*      chip 0 decision point. The trace is interpolated for the currently   */
/*      selected points/chips displayed on the front panel. The number of    */
/*      trace points depends on the current measurement interval setting.    */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_4:                                                */
/*      Returns a trace of the phase error in degrees. The first value is    */
/*      the symbol 0 decision point. The trace is interpolated for the       */
/*      currently selected chips/symbol displayed on the front panel. The    */
/*      number of trace points depends on the current measurement            */
/*      Interval setting.                                                    */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_5: Not supported by instrument.                   */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_6:                                                */
/*      Returns a series of floating point numbers that alternately          */
/*      represent I and Q pairs of the complimentary filtered measured data. */
/*      This is inverse filtered data of the intersymbol interference in     */
/*      CDMA signals due to the digital transmission filters defined in the  */
/*      standard as well as the base station phase equalization filter.      */
/*      The numbers are sent in the following order:                         */
/*        In-phase (I) sample, of symbol 0 decision point                    */
/*        Quadrature-phase (Q) sample, of symbol 0 decision point            */
/*        ...                                                                */
/*        In-phase (I) sample, of symbol 1 decision point                    */
/*        Quadrature-phase (Q) sample, of symbol 1 decision point            */
/*        ...                                                                */
/*      The trace can be interpolated to 2,4,8 points/chip selected with the */
/*      display Points/Chip softkey. This will change the number of points   */
/*      between decision points in the trace, changing the number of I/Q     */
/*      pairs sent for each decision point.  The number of trace points      */
/*      depends on the current measurement interval setting.                 */
/*                                                                           */
/*    ViAReal64 cdma1RHODataResults                                          */
/*                                                                           */
/*      Trace data returned by measurement, formatted according to           */
/*      cdma1RHOResultsType.                                                 */
/*                                                                           */
/*    ViInt32 cdma1RHODataResultsMaxSz                                       */
/*                                                                           */
/*      Maximum size of cdma1RHOTraceDataResults array.                      */
/*                                                                           */
/*    ViPInt32 cdma1RHODataResultsSz                                         */
/*                                                                           */
/*      Actual size of cdma1RHOTraceDataResults array.                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_RHOMeasureTrace
                       (ViSession instrumentHandle,
                        ViReal64 cdma1RHOMeasureTraceTimeout,
                        ViInt16 cdma1RHOMeasureTraceType,
                        ViInt16 cdma1RHOResultsType,
                        ViAReal64 cdma1RHODataResults,
                        ViInt32 cdma1RHODataResultsMaxSz,
                        ViPInt32 cdma1RHODataResultsSz);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_RHOSetAveraging                                              */
/*                                                                           */
/*      This method sets up averaging for CDMA One modulation accuracy (RHO) */
/*      measurements. User can control the number of averages, turn on and   */
/*      off averaging, and set the terminal control mode for averaging.      */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 cdma1RHOAverageCount                                           */
/*                                                                           */
/*      Sets the number of frames that will be averaged. After the specified */
/*      number of frames (average counts), the averaging mode (terminal      */
/*      control) setting determines the averaging action.                    */
/*                                                                           */
/*    ViBoolean cdma1RHOAverageState                                         */
/*                                                                           */
/*      Turns averaging on or off.                                           */
/*                                                                           */
/*      VI_TRUE - Turns averaging on.                                        */
/*      VI_FALSE - Turns averaging off.                                      */
/*                                                                           */
/*    ViInt16 cdma1RHOAverageMode                                            */
/*                                                                           */
/*      Selects the type of terminal control for averaging.                  */
/*                                                                           */
/*      hpesa_ AVERAGE_EXPONENTIAL  - Each successive data acquisition after */
/*      the average count is reached, is exponentially weighted and combined */
/*      with the existing average.                                           */
/*                                                                           */
/*      hpesa_ AVERAGE_REPEAT - After reaching the average count, the        */
/*      averaging is reset and a new average is started.                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_RHOSetAveraging
                       (ViSession instrumentHandle,
                        ViInt32 cdma1RHOAverageCount,
                        ViBoolean cdma1RHOAverageState,
                        ViInt16 cdma1RHOAverageMode);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_RHOGetAveraging                                              */
/*                                                                           */
/*      This method retrieves the averaging setup for CDMA One modulation    */
/*      accuracy (RHO) measurements.                                         */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt32 cdma1RHOAverageCountQuery                                     */
/*                                                                           */
/*      Returns the number of frames that will be averaged.                  */
/*                                                                           */
/*    ViPBoolean cdma1RHOAverageStateQuery                                   */
/*                                                                           */
/*      Returns current average state.                                       */
/*                                                                           */
/*      0 = VI_FALSE                                                         */
/*      1 = VI_TRUE                                                          */
/*                                                                           */
/*    ViPInt16 cdma1RHOAverageModeQuery                                      */
/*                                                                           */
/*      Returns the current instrument type of terminal control for          */
/*      averaging.                                                           */
/*      0 = hpesa_AVERAGE_EXPONENTIAL                                        */
/*      1 = hpesa_AVERAGE_REPEAT                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_RHOGetAveraging
                       (ViSession instrumentHandle,
                        ViPInt32 cdma1RHOAverageCountQuery,
                        ViPBoolean cdma1RHOAverageStateQuery,
                        ViPInt16 cdma1RHOAverageModeQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_RHOSetSweepTime                                              */
/*                                                                           */
/*      This method sets the length of the measurement interval that will be */
/*      used for CDMA One modulation accuracy (RHO) measurements.            */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 cdma1RHOSweepTime                                             */
/*                                                                           */
/*      New acquisition time, in seconds. Instrument range is 150 us to 26.7 */
/*      ms, and the default is 1.250 ms.                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_RHOSetSweepTime
                       (ViSession instrumentHandle,
                        ViReal64 cdma1RHOSweepTime);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_RHOGetSweepTime                                              */
/*                                                                           */
/*      This method retrieves the length of the current measurement interval */
/*      that will be used for CDMA One modulation accuracy (RHO)             */
/*      measurements.                                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPReal64 cdma1RHOSweepTimeQuery                                       */
/*                                                                           */
/*      Returns the current acquisition time in seconds.                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_RHOGetSweepTime
                       (ViSession instrumentHandle,
                        ViPReal64 cdma1RHOSweepTimeQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_RHOSetMeasurementSpectrum                                    */
/*                                                                           */
/*      This method selects the measurement spectrum type (normal or         */
/*      inverted) for the CDMA One modulation accuracy (RHO) measurement.    */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 cdma1RHOMeasSpectrum                                           */
/*                                                                           */
/*      Sets the new RHO measurement spectrum type.                          */
/*                                                                           */
/*      hpesa_RHO_MEAS_SPEC_NORMAL - normal spectrum.                        */
/*      hpesa_RHO_MEAS_SPEC_INVERT - inverted spectrum.                      */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_RHOSetMeasurementSpectrum
                       (ViSession instrumentHandle,
                        ViInt16 cdma1RHOMeasSpectrum);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_RHOGetMeasurementSpectrum                                    */
/*                                                                           */
/*      This method retrieves the current measurement spectrum type (normal  */
/*      or inverted) for the CDMA One modulation accuracy (RHO) measurement. */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 cdma1RHOMeasSpectrumQuery                                     */
/*                                                                           */
/*      Current RHO measurement spectrum type.                               */
/*      0 = hpesa_RHO_MEAS_SPEC_NORMAL                                       */
/*      1 = hpesa_RHO_MEAS_SPEC_INVERT                                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_RHOGetMeasurementSpectrum
                       (ViSession instrumentHandle,
                        ViPInt16 cdma1RHOMeasSpectrumQuery);


    /***************************************************/
    /* CDMA ONE CDP FUNCTIONS                          *
    /***************************************************/


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CDPConfigure                                                 */
/*                                                                           */
/*      This method sets up the instrument for CDMA One code domain power    */
/*      measurement (CDP).                                                   */
/*                                                                           */
/*      This method stops the current measurement and sets up the instrument */
/*      for the code domain power (CDP) measurement using the factory        */
/*      default instrument settings. It does not initiate the taking of      */
/*      measurement data. This command also turns the averaging function on  */
/*      and sets the number of averages to 10 for all measurements.          */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CDPConfigure
                       (ViSession instrumentHandle);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CDPMeasure                                                   */
/*                                                                           */
/*      This method measures the CDMA One power for each of the 64 Walsh     */
/*      codes/channels, relative to the total power in the pilot channel.    */
/*      User must be in the cdmaOne application mode to use this function.   */
/*                                                                           */
/*      User can select one of three types of measurement modes: Measure,    */
/*      Read, or Fetch.                                                      */
/*                                                                           */
/*      This method returns 10 CDP measurement scalar results, as described  */
/*      in the parameters below.                                             */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 cdma1CDPMeasureTimeout                                        */
/*                                                                           */
/*      This value temporarily changes the VISA timeout setting for the      */
/*      duration of the instrument measurement. The user needs to set this   */
/*      value to an appropriate time that will be longer then the expected   */
/*      measurement time. Units are in seconds.                              */
/*                                                                           */
/*    ViInt16 cdma1CDPMeasureType                                            */
/*                                                                           */
/*      Selects measurement type. Note that for hpesa_FETCH type the         */
/*      instrument should already be in this measurement mode else the       */
/*      function will time out. User can select this measurement mode by     */
/*      first using the "MEASURE" or "READ" types or call the                */
/*      hpesa_cdma1_CDPConfigure function.                                   */
/*                                                                           */
/*      hpesa_MEASURE - Stops current measurement, changes to this selected  */
/*      measurement, sets up instrument using factory defaults, makes        */
/*      measurement, returns measurement results.                            */
/*                                                                           */
/*      hpesa_READ - Stops current measurement,  changes to this selected    */
/*      measurement, uses instrument settings from last measurement of this  */
/*      type, makes measurement, returns measurements results.               */
/*                                                                           */
/*      hpesa_FETCH - Returns measurements results only. No new measurement  */
/*      is initiated.                                                        */
/*                                                                           */
/*    ViPReal64 cdma1CDPTimeOffsetQuery                                      */
/*                                                                           */
/*      Time offset is a floating point number with units of seconds. This   */
/*      is the time delay of the even second clock with respect to the start */
/*      of the short code PN sequences. Time offset takes into account the   */
/*      setting of the PN offset parameter. For example, with the PN offset  */
/*      to 2 the reported time offset would be the calculated time offset    */
/*      times 64 chip intervals (or 2 X 52.0833e-6).                         */
/*                                                                           */
/*    ViPReal64 cdma1CDPFreqErrorQuery                                       */
/*                                                                           */
/*      Returns the frequency error, which is a floating point number (in    */
/*      Hz) of the frequency error in the measured signal. This error is     */
/*      based on the linear best fit of the uncorrected measured phase.      */
/*                                                                           */
/*    ViPReal64 cdma1CDPCarrFeedthruQuery                                    */
/*                                                                           */
/*      Returns the carrier feedthrough, a floating point number (in dB) of  */
/*      the dc offset, of I and Q, from the origin.                          */
/*                                                                           */
/*    ViPReal64 cdma1CDPPilotPowerQuery                                      */
/*                                                                           */
/*      Returns the pilot power, a floating point number with units of dB.   */
/*      It is the relative power of the pilot channel (Walsh code 0) with    */
/*      respect to the carrier power.                                        */
/*                                                                           */
/*    ViPReal64 cdma1CDPPagingPowerQuery                                     */
/*                                                                           */
/*      Paging power is a floating point number with units of dB. It is the  */
/*      relative power of the paging channel (Walsh code 1) with respect to  */
/*      the carrier power.                                                   */
/*                                                                           */
/*    ViPReal64 cdma1CDPSyncPowerQuery                                       */
/*                                                                           */
/*      Returns the sync power, a floating point number with units of dB. It */
/*      is the relative power of the sync channel (Walsh code 32) with       */
/*      respect to the carrier power.                                        */
/*                                                                           */
/*    ViPReal64 cdma1CDPAvgTrafficPowQuery                                   */
/*                                                                           */
/*      Returns the average traffic power, a floating point number with      */
/*      units of dB. It is the average relative power of the active traffic  */
/*      channels with respect to the carrier power. Traffic channels are     */
/*      defined as all of the Walsh codes except Walsh 0,1, 32. A traffic    */
/*      channel is active if its coding power is greater than the active set */
/*      threshold parameter which you have selected.                         */
/*                                                                           */
/*    ViPReal64 cdma1CDPMaxITPowerQuery                                      */
/*                                                                           */
/*      Returns the maximum inactive traffic power, a floating point number  */
/*      with units of dB. It is the maximum relative power of an inactive    */
/*      traffic channel with respect to the carrier power. Traffic channels  */
/*      are defined as all of the Walsh codes except Walsh 0,1, 32. A        */
/*      traffic channel is inactive if its coding power is less than the     */
/*      active set threshold parameter which you have selected.              */
/*                                                                           */
/*    ViPReal64 cdma1CDPAvgITPowerQuery                                      */
/*                                                                           */
/*      Returns the average inactive traffic power, a floating point number  */
/*      with units of dB. It is the average relative power of the inactive   */
/*      traffic channels with respect to the carrier power. Traffic channels */
/*      are defined as all of the Walsh codes except Walsh 0,1, 32. A        */
/*      traffic channel is inactive if its coding power is less than the     */
/*      active set threshold parameter which you have selected.              */
/*                                                                           */
/*    ViPReal64 cdma1CDPRhoEstimateQuery                                     */
/*                                                                           */
/*      Returns the RHO (estimated), a floating point number without units.  */
/*      This is an estimate of Rho based on the input signal as a pilot only */
/*      signal. It is calculated by estimating the noise floor for the       */
/*      active code channels using the average of the noise in all of the    */
/*      inactive channels. The measured noise floor is subtracted from all   */
/*      active channels before they are summed together to get the total     */
/*      signal power. The ratio of total noise power to total power is the   */
/*      Rho (estimated).                                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CDPMeasure
                       (ViSession instrumentHandle,
                        ViReal64 cdma1CDPMeasureTimeout,
                        ViInt16 cdma1CDPMeasureType,
                        ViPReal64 cdma1CDPTimeOffsetQuery,
                        ViPReal64 cdma1CDPFreqErrorQuery,
                        ViPReal64 cdma1CDPCarrFeedthruQuery,
                        ViPReal64 cdma1CDPPilotPowerQuery,
                        ViPReal64 cdma1CDPPagingPowerQuery,
                        ViPReal64 cdma1CDPSyncPowerQuery,
                        ViPReal64 cdma1CDPAvgTrafficPowQuery,
                        ViPReal64 cdma1CDPMaxITPowerQuery,
                        ViPReal64 cdma1CDPAvgITPowerQuery,
                        ViPReal64 cdma1CDPRhoEstimateQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CDPMeasureTrace                                              */
/*                                                                           */
/*      This method measures the CDMA One power for each of the 64 Walsh     */
/*      codes/channels, relative to the total power in the pilot channel.    */
/*      User must be in the cdmaOne application mode to use this function.   */
/*                                                                           */
/*      User can select one of three types of measurement modes: Measure,    */
/*      Read, or Fetch.                                                      */
/*                                                                           */
/*      Function returns the CDP measurement results specified by the        */
/*      "cdma1CDPResultsType" parameter as a real array.                     */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 cdma1CDPMeasureTraceTimeout                                   */
/*                                                                           */
/*      This value temporarily changes the VISA timeout setting for the      */
/*      duration of the instrument measurement. The user needs to set this   */
/*      value to an appropriate time that will be longer then the expected   */
/*      measurement time. Units are in seconds.                              */
/*                                                                           */
/*    ViInt16 cdma1CDPMeasureTraceType                                       */
/*                                                                           */
/*      Selects measurement type. Note that for hpesa_FETCH type the         */
/*      instrument should already be in this measurement mode else the       */
/*      function will time out. User can select this measurement mode by     */
/*      first using the "MEASURE" or "READ" types or call the                */
/*      hpesa_cdma1_CDPConfigure function.                                   */
/*                                                                           */
/*      hpesa_MEASURE - Stops current measurement, changes to this selected  */
/*      measurement, sets up instrument using factory defaults, makes        */
/*      measurement, returns measurement results.                            */
/*                                                                           */
/*      hpesa_READ - Stops current measurement,  changes to this selected    */
/*      measurement, uses instrument settings from last measurement of this  */
/*      type, makes measurement, returns measurements results.               */
/*                                                                           */
/*      hpesa_FETCH - Returns measurements results only. No new measurement  */
/*      is initiated.                                                        */
/*                                                                           */
/*    ViInt16 cdma1CDPResultsType                                            */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_0: Not supported by instrument.                   */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_1:                                                */
/*      Returns the following 9 scalar results:                              */
/*      1. Time offset is a floating point number with units of seconds.     */
/*      This is the time delay of the even second clock with respect to the  */
/*      start of the short code PN sequences. Time offset takes into account */
/*      the setting of the PN offset parameter. For example, with the PN     */
/*      offset to 2 the reported time offset would be the calculated time    */
/*      offset times 64 chip intervals (or 2 X 52.0833e-6).                  */
/*      2. Frequency error is a floating point number (in Hz) of the         */
/*      frequency error in the measured signal. This error is based on the   */
/*      linear best fit of the uncorrected measured phase.                   */
/*      3. Carrier feedthrough is a floating point number (in dB) of the dc  */
/*      offset, of I and Q, from the origin.                                 */
/*      4. Pilot power is a floating point number with units of dB. It is    */
/*      the relative power of the pilot channel (Walsh code 0) with respect  */
/*      to the carrier power.                                                */
/*      5. Paging power is a floating point number with units of dB. It is   */
/*      the relative power of the paging channel (Walsh code 1) with respect */
/*      to the carrier power.                                                */
/*      6. Sync power is a floating point number with units of dB. It is the */
/*      relative power of the sync channel (Walsh code 32) with respect to   */
/*      the carrier power.                                                   */
/*      7. Average traffic power is a floating point number with units of    */
/*      dB. It is the average relative power of the active traffic channels  */
/*      with respect to the carrier power. Traffic channels are defined as   */
/*      all of the Walsh codes except Walsh 0,1, 32. A traffic channel is    */
/*      active if its coding power is greater than the active set threshold  */
/*      parameter which you have selected.                                   */
/*      8. Maximum inactive traffic power is a floating point number with    */
/*      units of dB. It is the maximum relative power of an inactive traffic */
/*      channel with respect to the carrier power. Traffic channels are      */
/*      defined as all of the Walsh codes except Walsh 0,1, 32. A traffic    */
/*      channel is inactive if its coding power is less than the active set  */
/*      threshold parameter which you have selected.                         */
/*      9. Average inactive traffic power is a floating point number with    */
/*      units of dB. It is the average relative power of the inactive        */
/*      traffic channels with respect to the carrier power. Traffic channels */
/*      are defined as all of the Walsh codes except Walsh 0,1, 32. A        */
/*      traffic channel is inactive if its coding power is less than the     */
/*      active set threshold parameter which you have selected.              */
/*      10. RHO (estimated) is a floating point number without units. This   */
/*      is an estimate of Rho based on the input signal as a pilot only      */
/*      signal. It is calculated by estimating the noise floor for the       */
/*      active code channels using the average of the noise in all of the    */
/*      inactive channels. The measured noise floor is subtracted from all   */
/*      active channels before they are summed together to get the total     */
/*      signal power. The ratio of total noise power to total power is the   */
/*      Rho (estimated).                                                     */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_2:                                                */
/*      Returns floating point numbers that are the trace data of the code   */
/*      domain power trace for all 64 Walsh codes. This series of 64 numbers */
/*      represent the relative power levels (in dB) of all 64 walsh codes,   */
/*      with respect to the carrier power.                                   */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_3:                                                */
/*      Returns floating point numbers that are the trace data of the code   */
/*      domain timing trace for all 64 Walsh codes. This series of 64        */
/*      numbers represent the relative timing estimations (in seconds) of    */
/*      the codes, relative to the pilot channel. Typical values are on the  */
/*      order of 1 ns.                                                       */
/*                                                                           */
/*      The instrument default method is to measure the power only. If you   */
/*      use this result type with the Method left as Power, invalid data     */
/*      will be returned. Set Method to "Timing/Phase" for proper results    */
/*      ("hpesa_SetMeasurementMethod function").                             */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_4:                                                */
/*      Returns floating point numbers that are the trace data of the code   */
/*      domain phase trace for all 64 Walsh codes. This series of 64 numbers */
/*      represent the relative phase estimations (in radians) of the codes,  */
/*      relative to the pilot channel. Typical values are on the order of 1  */
/*      mrad.                                                                */
/*                                                                           */
/*      The instrument default method is to measure the power only. If you   */
/*      use this result type with the Method left as Power, invalid data     */
/*      will be returned. Set Method to "Timing/Phase" for proper results    */
/*      ("hpesa_SetMeasurementMethod function").                             */
/*                                                                           */
/*    ViAReal64 cdma1CDPDataResults                                          */
/*                                                                           */
/*      Trace data returned by measurement, formatted according to           */
/*      cdma1CDPResultsType.                                                 */
/*                                                                           */
/*    ViInt32 cdma1CDPDataResultsMaxSz                                       */
/*                                                                           */
/*      Maximum size of cdma1CDPTraceDataResults array.                      */
/*                                                                           */
/*    ViPInt32 cdma1CDPDataResultsSz                                         */
/*                                                                           */
/*      Actual size of cdma1CDPDataResults array.                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CDPMeasureTrace
                       (ViSession instrumentHandle,
                        ViReal64 cdma1CDPMeasureTraceTimeout,
                        ViInt16 cdma1CDPMeasureTraceType,
                        ViInt16 cdma1CDPResultsType,
                        ViAReal64 cdma1CDPDataResults,
                        ViInt32 cdma1CDPDataResultsMaxSz,
                        ViPInt32 cdma1CDPDataResultsSz);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CDPSetAveraging                                              */
/*                                                                           */
/*      This method sets up averaging for CDMA One code domain power (CDP)   */
/*      measurements. User can control the number of averages, turn on and   */
/*      off averaging, and set the terminal control mode for averaging.      */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 cdma1CDPAverageCount                                           */
/*                                                                           */
/*      Sets the number of frames that will be averaged. After the specified */
/*      number of frames (average counts), the averaging mode (terminal      */
/*      control) determines the averaging action.                            */
/*                                                                           */
/*    ViBoolean cdma1CDPAverageState                                         */
/*                                                                           */
/*      Turns averaging on or off.                                           */
/*                                                                           */
/*      VI_TRUE - Turn averaging on.                                         */
/*      VI_FALSE - Turn averaging off.                                       */
/*                                                                           */
/*    ViInt16 cdma1CDPAverageMode                                            */
/*                                                                           */
/*      Selects the type of terminal control for averaging.                  */
/*                                                                           */
/*      hpesa_AVERAGE_EXPONENTIAL  - Each successive data acquisition after  */
/*      the average count is reached, is exponentially weighted and combined */
/*      with the existing average.                                           */
/*                                                                           */
/*      hpesa_AVERAGE_REPEAT - After reaching the average count, the         */
/*      averaging is reset and a new average is started                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CDPSetAveraging
                       (ViSession instrumentHandle,
                        ViInt32 cdma1CDPAverageCount,
                        ViBoolean cdma1CDPAverageState,
                        ViInt16 cdma1CDPAverageMode);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CDPGetAveraging                                              */
/*                                                                           */
/*      This method retrieves the averaging setup for the code domain power  */
/*      (CDP) measurements.                                                  */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt32 cdma1CDPAverageCountQuery                                     */
/*                                                                           */
/*      Returns the current number of sweeps that being averaged.            */
/*                                                                           */
/*    ViPBoolean cdma1CDPAverageStateQuery                                   */
/*                                                                           */
/*      Returns the current instrument current average state.                */
/*                                                                           */
/*      0 = VI_FALSE                                                         */
/*      1 = VI_TRUE                                                          */
/*                                                                           */
/*    ViPInt16 cdma1CDPAverageModeQuery                                      */
/*                                                                           */
/*      Returns the current instrument type of terminal control for          */
/*      averaging.                                                           */
/*      0 = hpesa_AVERAGE_EXPONENTIAL                                        */
/*      1 = hpesa_AVERAGE_REPEAT                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CDPGetAveraging
                       (ViSession instrumentHandle,
                        ViPInt32 cdma1CDPAverageCountQuery,
                        ViPBoolean cdma1CDPAverageStateQuery,
                        ViPInt16 cdma1CDPAverageModeQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CDPSetSweepTime                                              */
/*                                                                           */
/*      This method sets the length of the measurement interval that will be */
/*      used for CDMA One code domain power (CDP) measurements.              */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 cdma1CDPSweepTime                                             */
/*                                                                           */
/*      Sets the new measurement interval time, in seconds. Instrument range */
/*      is 500 us to 26.7 ms, and the default is 1.250 ms.                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CDPSetSweepTime
                       (ViSession instrumentHandle,
                        ViReal64 cdma1CDPSweepTime);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CDPGetSweepTime                                              */
/*                                                                           */
/*      This method retrieves the current length of the measurement interval */
/*      that will be used for CDMA One code domain power (CDP) measurements. */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPReal64 cdma1CDPSweepTimeQuery                                       */
/*                                                                           */
/*      Returns the measurement interval time in seconds.                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CDPGetSweepTime
                       (ViSession instrumentHandle,
                        ViPReal64 cdma1CDPSweepTimeQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CDPSetMeasurementSpectrum                                    */
/*                                                                           */
/*      This method selects measurement spectrum type (normal or inverted)   */
/*      for the CDMA One code domain power (CDP) measurements.               */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 cdma1CDPMeasSpectrum                                           */
/*                                                                           */
/*      Sets the new CDP measurement spectrum type.                          */
/*                                                                           */
/*      hpesa_CDP_MEAS_SPEC_NORMAL - normal spectrum.                        */
/*      hpesa_CDP_MEAS_SPEC_INVERT - inverted spectrum.                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CDPSetMeasurementSpectrum
                       (ViSession instrumentHandle,
                        ViInt16 cdma1CDPMeasSpectrum);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CDPGetMeasurementSpectrum                                    */
/*                                                                           */
/*      This method retrieves the current measurement spectrum type (normal  */
/*      or inverted) for CDMA One code domain power (CDP) measurements.      */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 cdma1CDPMeasSpectrumQuery                                     */
/*                                                                           */
/*      Current CDP measurement spectrum type.                               */
/*                                                                           */
/*      0 = hpesa_CDP_MEASURE_SPEC_NORMAL                                    */
/*      1 = hpesa_CDP_MEASURE_SPEC_INVERT                                    */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CDPGetMeasurementSpectrum
                       (ViSession instrumentHandle,
                        ViPInt16 cdma1CDPMeasSpectrumQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CDPSetMeasThreshold                                          */
/*                                                                           */
/*      This method sets the active set threshold value for CDMA One code    */
/*      domain power (CDP) measurements. Walsh Channels with powers less     */
/*      than this value will be treated as non-active (noise) channels.      */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 cdma1CDPMeasThreshold                                         */
/*                                                                           */
/*      Sets the new active threshold value in dB. Instrument range is 0 to  */
/*      - 200 dB, and the default is - 20 dB.                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CDPSetMeasThreshold
                       (ViSession instrumentHandle,
                        ViReal64 cdma1CDPMeasThreshold);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CDPGetMeasThreshold                                          */
/*                                                                           */
/*      This method retrieves the active set threshold value for CDMA One    */
/*      code domain power (CDP) measurements.                                */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPReal64 cdma1CDPMeasThresholdQuery                                   */
/*                                                                           */
/*      Returns the current active threshold value, in dB.                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CDPGetMeasThreshold
                       (ViSession instrumentHandle,
                        ViPReal64 cdma1CDPMeasThresholdQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CDPSetMeasurementMethod                                      */
/*                                                                           */
/*      This method selects the measurement method for CDMA One code domain  */
/*      power (CDP) measurements.                                            */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 cdma1CDPMeasMethod                                             */
/*                                                                           */
/*      Sets the new CDP measurement method.                                 */
/*                                                                           */
/*      hpesa_CDP_MEAS_METHOD_POWER   - Power measures the Code Domain Power */
/*      of all 64 Walsh Channels.                                            */
/*                                                                           */
/*      hpesa_CDP_MEAS_METHOD_TPHASE  - Timing & Phase measures the Code     */
/*      Domain Power, Code Domain Timing, and Code Domain Phase of all 64    */
/*      Walsh Channels.                                                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CDPSetMeasurementMethod
                       (ViSession instrumentHandle,
                        ViInt16 cdma1CDPMeasMethod);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CDPGetMeasurementMethod                                      */
/*                                                                           */
/*      This method retrieves the current measurement method for code domain */
/*      power (CDP) measurements.                                            */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 cdma1CDPMeasMethodQuery                                       */
/*                                                                           */
/*      Returns the current CDP measurement method.                          */
/*                                                                           */
/*      0 = hpesa_CDP_MEAS_METHOD_POWER                                      */
/*      1 = hpesa_CDP_MEAS_METHOD_TPHASE                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CDPGetMeasurementMethod
                       (ViSession instrumentHandle,
                        ViPInt16 cdma1CDPMeasMethodQuery);


    /***************************************************/
    /* CDMA ONE CSP FUNCTIONS                          *
    /***************************************************/


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CSPConfigure                                                 */
/*                                                                           */
/*      This method sets up the instrument for CDMA One close spurs (CSP)    */
/*      measurements.  It stops the current measurement and sets up the      */
/*      instrument for the Close Spurs Power (CSP) measurement using the     */
/*      factory default instrument settings. It does not initiate the taking */
/*      of measurement data. This command also turns the averaging function  */
/*      on and sets the number of averages to 10 for all measurements.       */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CSPConfigure
                       (ViSession instrumentHandle);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CSPMeasure                                                   */
/*                                                                           */
/*      This method measures the CDMA One spurious emissions in the transmit */
/*      band relative to the channel power in the selected channel. User     */
/*      must be in the cdmaOne application mode to use this function.        */
/*                                                                           */
/*      User can select one of three types of measurement modes: Measure,    */
/*      Read, or Fetch.                                                      */
/*                                                                           */
/*      This method returns four CSP measurement scalar results, as          */
/*      described in the parameters below. The user can select the segment   */
/*      (Lower, Center, Upper) from which to return the measurement results. */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 cdma1CSPMeasureTimeout                                        */
/*                                                                           */
/*      This value temporarily changes the VISA timeout setting for the      */
/*      duration of the instrument measurement. The user needs to set this   */
/*      value to an appropriate time that will be longer then the expected   */
/*      measurement time. Units are in seconds.                              */
/*                                                                           */
/*    ViInt16 cdma1CSPMeasureType                                            */
/*                                                                           */
/*      Selects measurement type. Note that for hpesa_FETCH type the         */
/*      instrument should already be in this measurement mode else the       */
/*      function will time out. User can select this measurement mode by     */
/*      first using the "MEASURE" or "READ" types or call the                */
/*      hpesa_cdma1_CSPConfigure function.                                   */
/*                                                                           */
/*      hpesa_MEASURE - Stops current measurement, changes to this selected  */
/*      measurement, sets up instrument using factory defaults, makes        */
/*      measurement, returns measurement results.                            */
/*                                                                           */
/*      hpesa_READ - Stops current measurement,  changes to this selected    */
/*      measurement, uses instrument settings from last measurement of this  */
/*      type, makes measurement, returns measurements results.               */
/*                                                                           */
/*      hpesa_FETCH - Returns measurements results only. No new measurement  */
/*      is initiated.                                                        */
/*                                                                           */
/*    ViInt16 cdma1CSPMeasureSegment                                         */
/*                                                                           */
/*      Selects which segment of the instrument measurement results to       */
/*      return (Lower, Center, Upper) for the scalar results below. The      */
/*      channel power scalar is the same for all three segments.             */
/*                                                                           */
/*      The user should use either "hpesa_MEAS" or "hpesa_READ" as the       */
/*      cdma1CSPMeasureType value for the first time the function is invoked */
/*      to get the first segment measurement results, then use "hpesa_FETCH" */
/*      to get the other segments measurement results.                       */
/*                                                                           */
/*      hpesa_CSP_SEGMENT_LOWER -  Lower segment measurement results         */
/*      hpesa_CSP_SEGMENT_CENTER - Center segment measurement results        */
/*      hpesa_CSP_SEGMENT_UPPER -  Upper segment measurement results         */
/*                                                                           */
/*    ViPReal64 cdma1CSPChannelPowerQuery                                    */
/*                                                                           */
/*      Returns the channel power, in dBm.                                   */
/*                                                                           */
/*    ViPReal64 cdma1CSPSpurFreqDiff                                         */
/*                                                                           */
/*      Returns the specified measurement segment spur frequency difference  */
/*      from channel center frequency (in Hz).                               */
/*                                                                           */
/*    ViPReal64 cdma1CSPSpurAmplDiffLimit                                    */
/*                                                                           */
/*      Returns the specified measurement segment spur amplitude difference  */
/*      from the limit (in dB).                                              */
/*                                                                           */
/*    ViPReal64 cdma1CSPSpurAmplDiffCHP                                      */
/*                                                                           */
/*      Returns the specified measurement segment spur amplitude difference  */
/*      from channel power (in dBc).                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CSPMeasure
                       (ViSession instrumentHandle,
                        ViReal64 cdma1CSPMeasureTimeout,
                        ViInt16 cdma1CSPMeasureType,
                        ViInt16 cdma1CSPMeasureSegment,
                        ViPReal64 cdma1CSPChannelPowerQuery,
                        ViPReal64 cdma1CSPSpurFreqDiff,
                        ViPReal64 cdma1CSPSpurAmplDiffLimit,
                        ViPReal64 cdma1CSPSpurAmplDiffCHP);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CSPMeasureTrace                                              */
/*                                                                           */
/*      This method measures the CDMA One spurious emissions in the transmit */
/*      band relative to the channel power in the selected channel. User     */
/*      must be in the cdmaOne application mode to use this function.        */
/*                                                                           */
/*      User can select one of three types of measurement modes: Measure,    */
/*      Read, or Fetch.                                                      */
/*                                                                           */
/*      Function returns the CSP measurement results specified by the        */
/*      "cdma1CSPResultsType" parameter as a real array.                     */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 cdma1CSPMeasureTraceTimeout                                   */
/*                                                                           */
/*      This value temporarily changes the VISA timeout setting for the      */
/*      duration of the instrument measurement. The user needs to set this   */
/*      value to an appropriate time that will be longer then the expected   */
/*      measurement time. Units are in seconds.                              */
/*                                                                           */
/*    ViInt16 cdma1CSPMeasureTraceType                                       */
/*                                                                           */
/*      Selects measurement type. Note that for hpesa_FETCH type the         */
/*      instrument should already be in this measurement mode else the       */
/*      function will time out. User can select this measurement mode by     */
/*      first using the "MEASURE" or "READ" types or call the                */
/*      hpesa_cdma1_CSPConfigure function.                                   */
/*                                                                           */
/*      hpesa_MEASURE - Stops current measurement, changes to this selected  */
/*      measurement, sets up instrument using factory defaults, makes        */
/*      measurement, returns measurement results.                            */
/*                                                                           */
/*      hpesa_READ - Stops current measurement,  changes to this selected    */
/*      measurement, uses instrument settings from last measurement of this  */
/*      type, makes measurement, returns measurements results.               */
/*                                                                           */
/*      hpesa_FETCH - Returns measurements results only. No new measurement  */
/*      is initiated.                                                        */
/*                                                                           */
/*    ViInt16 cdma1CSPResultsType                                            */
/*                                                                           */
/*      Determines the format of the values returned in cdma1CSPDataResults. */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_0: Not supported by instrument.                   */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_1:                                                */
/*      Returns 10 scalar results. The 10 results represent, first, the      */
/*      channel power and then 3 spur values. The 3 values are composed of   */
/*      the spurious value from each of 3 measurement segments. The          */
/*      measurement segments are Lower, Center, and Upper. For each of the 3 */
/*      spurs displayed, 3 values are returned:                              */
/*      1. The spur frequency difference from channel center frequency (in   */
/*      Hz)                                                                  */
/*      2. The spur amplitude difference from the limit (in dB)              */
/*      3. The spur amplitude difference from channel power (in dBc)         */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_2:                                                */
/*      Returns trace of the segment containing the worst spur.              */
/*                                                                           */
/*    ViAReal64 cdma1CSPDataResults                                          */
/*                                                                           */
/*      Trace data returned by measurement, formatted according to           */
/*      cdma1CSPResultsType.                                                 */
/*                                                                           */
/*    ViInt32 cdma1CSPDataResultsMaxSz                                       */
/*                                                                           */
/*      Maximum size of cdma1CSPDataResults array.                           */
/*                                                                           */
/*    ViPInt32 cdma1CSPDataResultsSz                                         */
/*                                                                           */
/*      Actual size of cdma1CSPDataResults array.                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CSPMeasureTrace
                       (ViSession instrumentHandle,
                        ViReal64 cdma1CSPMeasureTraceTimeout,
                        ViInt16 cdma1CSPMeasureTraceType,
                        ViInt16 cdma1CSPResultsType,
                        ViAReal64 cdma1CSPDataResults,
                        ViInt32 cdma1CSPDataResultsMaxSz,
                        ViPInt32 cdma1CSPDataResultsSz);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CSPSetAveraging                                              */
/*                                                                           */
/*      This method sets up averaging for CDMA One close spurs power (CSP)   */
/*      measurements. User can control the number of averages, turn on and   */
/*      off averaging, and set the terminal control mode for averaging.      */
/*                                                                           */
/*      The hpesa_cdma1CSPSetMeasurementType function will cause the         */
/*      instrument cdmaOne Close Spurs averaging mode to reset to "Repeat"   */
/*      if the measurement type is set to "Full". Set the measurement type   */
/*      to "Examine" first first (with the                                   */
/*      hpesa_cdma1_CSPSetMeasurementType" function) before invoking this    */
/*      function if  average mode "Exponential" is required.                 */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 cdma1CSPAverageCount                                           */
/*                                                                           */
/*      Sets the number of data acquisitions that will be averaged. After    */
/*      the specified number of average counts, the averaging mode (terminal */
/*      control) determines the averaging action.                            */
/*                                                                           */
/*    ViBoolean cdma1CSPAverageState                                         */
/*                                                                           */
/*      Turns averaging on or off.                                           */
/*                                                                           */
/*      VI_TRUE - Turn averaging on.                                         */
/*      VI_FALSE - Turn averaging off.                                       */
/*                                                                           */
/*    ViInt16 cdma1CSPAverageMode                                            */
/*                                                                           */
/*      Selects the type of terminal control for averaging.                  */
/*                                                                           */
/*      hpesa_AVERAGE_EXPONENTIAL  - Each successive data acquisition after  */
/*      the average count is reached, is exponentially weighted and combined */
/*      with the existing average.                                           */
/*                                                                           */
/*      hpesa_AVERAGE_REPEAT - After reaching the average count, the         */
/*      averaging is reset and a new average is started.                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CSPSetAveraging
                       (ViSession instrumentHandle,
                        ViInt32 cdma1CSPAverageCount,
                        ViBoolean cdma1CSPAverageState,
                        ViInt16 cdma1CSPAverageMode);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CSPGetAveraging                                              */
/*                                                                           */
/*      This method retrieves the averaging setup for CDMA One close spurs   */
/*      power (CSP) measurements.                                            */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt32 cdma1CSPAverageCountQuery                                     */
/*                                                                           */
/*      Current number of data acquisitions that will be averaged. After the */
/*      specified number of average counts, the averaging mode (terminal     */
/*      control) determines the averaging action.                            */
/*                                                                           */
/*    ViPBoolean cdma1CSPAverageStateQuery                                   */
/*                                                                           */
/*      Returns the current average state.                                   */
/*                                                                           */
/*      0 = VI_FALSE                                                         */
/*      1 = VI_TRUE                                                          */
/*                                                                           */
/*    ViPInt16 cdma1CSPAverageModeQ                                          */
/*                                                                           */
/*      Returns the current instrument type of terminal control for          */
/*      averaging.                                                           */
/*                                                                           */
/*      0 = hpesa_AVERAGE_EXPONENTIAL                                        */
/*      1 = hpesa_AVERAGE_REPEAT                                             */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CSPGetAveraging
                       (ViSession instrumentHandle,
                        ViPInt32 cdma1CSPAverageCountQuery,
                        ViPBoolean cdma1CSPAverageStateQuery,
                        ViPInt16 cdma1CSPAverageModeQ);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CSPSetMeasurementType                                        */
/*                                                                           */
/*      This method set the CDMA One close spurs power (CSP) measurement     */
/*      type.                                                                */
/*                                                                           */
/*      Note:  This method will cause the cdmaOne close spurs averaging mode */
/*      to reset to "Repeat" if the measurement type is set to "Full".       */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 cdma1CSPMeasType                                               */
/*                                                                           */
/*      Sets the new measurement type.                                       */
/*                                                                           */
/*      hpesa_CSP_MEAS_TYPE_EXAMINE - In single sweep mode - measures spurs  */
/*      in the upper, lower, and center segments and then displays the       */
/*      segment with the highest power spur.                                 */
/*      In the continuous mode - measures spurs in the upper, lower, and     */
/*      center segments. It then continuously measures the segment with the  */
/*      highest power spur. The initial spur values are held and the marker  */
/*      value is updated with the current spur value found on each sweep.    */
/*      Segment All is not active when the measurement type Examine is       */
/*      selected.                                                            */
/*                                                                           */
/*      hpesa_CSP_MEAS_TYPE_FULL  - In single sweep mode - measures the      */
/*      spurs in the upper, lower, and center segments and then displays the */
/*      segment with the highest power spur.                                 */
/*      In continuous mode - measures spurs in the upper, lower, and center  */
/*      segments. It then continuously cycles through segments. Unlike       */
/*      Examine, all results values are updated with the current spur values */
/*      found on each sweep.                                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CSPSetMeasurementType
                       (ViSession instrumentHandle,
                        ViInt16 cdma1CSPMeasType);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_CSPGetMeasurementType                                        */
/*                                                                           */
/*      This method retrieves the CDMA One close spurs power (CSP)           */
/*      measurement type.                                                    */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 cdma1CSPMeasTypeQuery                                         */
/*                                                                           */
/*      Current measurement type.                                            */
/*                                                                           */
/*      0 = hpesa_CSP_MEAS_TYPE_EXAMINE                                      */
/*      1 = hpesa_CSP_MEAS_TYPE_FULL                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_CSPGetMeasurementType
                       (ViSession instrumentHandle,
                        ViPInt16 cdma1CSPMeasTypeQuery);


    /***************************************************/
    /* CDMA ONE SHP FUNCTIONS                          *
    /***************************************************/


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_SHPConfigure                                                 */
/*                                                                           */
/*      This method sets up the instrument for CDMA One spurs at harmonic    */
/*      power (SHP) measurements using the factory default instrument        */
/*      settings.  It stops the current measurement and sets up the          */
/*      instrument for the Spurs at Harmonics (SHP) measurement using the    */
/*      factory default instrument settings. It does not initiate the taking */
/*      of measurement data. This command also turns the averaging function  */
/*      on and sets the number of averages to 10 for all measurements.       */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_SHPConfigure
                       (ViSession instrumentHandle);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_SHPMeasureTrace                                              */
/*                                                                           */
/*      This method measures the power of the CDMA One spurious emissions at */
/*      the selected harmonic frequencies of the current channel frequency   */
/*      using the selected resolution bandwidth filter. The number of spurs  */
/*      to be measured can be set by the "cdma1SHPSetNumberHarmonics"        */
/*      function. For each harmonic, the reference level is set 40 dB above  */
/*      the largest amplitude limit with input attenuation held constant.    */
/*      The attenuation is set to limit the carrier (fundamental) power at   */
/*      the spectrum analyzer input mixer to the value determined by Max     */
/*      Mixer Lvl. You must be in the cdmaOne mode to use these commands.    */
/*                                                                           */
/*      User can select one of three types of measurement modes: Measure,    */
/*      Read, or Fetch.                                                      */
/*                                                                           */
/*      This method returns the SHP measurement results specified by the     */
/*      "cdma1SHPResultsType" parameter as a real array.                     */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 cdma1SHPMeasureTraceTimeout                                   */
/*                                                                           */
/*      This value temporarily changes the VISA timeout setting for the      */
/*      duration of the instrument measurement. The user needs to set this   */
/*      value to an appropriate time that will be longer then the expected   */
/*      measurement time. Units are in seconds.                              */
/*                                                                           */
/*    ViInt16 cdma1SHPMeasureTraceType                                       */
/*                                                                           */
/*      Selects measurement type. Note that for hpesa_FETCH type the         */
/*      instrument should already be in this measurement mode else the       */
/*      function will time out. User can select this measurement mode by     */
/*      first using the "MEASURE" or "READ" types or call the                */
/*      hpesa_cdma1_SHPConfigure function.                                   */
/*                                                                           */
/*      hpesa_MEASURE - Stops current measurement, changes to this selected  */
/*      measurement, sets up instrument using factory defaults, makes        */
/*      measurement, returns measurement results.                            */
/*                                                                           */
/*      hpesa_READ - Stops current measurement,  changes to this selected    */
/*      measurement, uses instrument settings from last measurement of this  */
/*      type, makes measurement, returns measurements results.               */
/*                                                                           */
/*      hpesa_FETCH - Returns measurements results only. No new measurement  */
/*      is initiated.                                                        */
/*                                                                           */
/*    ViInt16 cdma1SHPResultsType                                            */
/*                                                                           */
/*      Determines the format of the values returned in cdma1SHPDataResults. */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_0:                                                */
/*      Not supported.                                                       */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_1:                                                */
/*      Returns 61 values. The channel power value and a 10 row by 6 column  */
/*      array of values. Each row contains the following values:             */
/*      1. Harmonic number                                                   */
/*      2. Frequency (MHz)                                                   */
/*      3. Absolute amplitude (dBm)                                          */
/*      4. Delta from the absolute limit                                     */
/*      5. Relative amplitude from the carrier (dBc)                         */
/*      6. Delta from the relative limit                                     */
/*                                                                           */
/*    ViAReal64 cdma1SHPDataResults                                          */
/*                                                                           */
/*      Trace data returned by measurement, formatted according to           */
/*      cdma1SHPResultsType.                                                 */
/*                                                                           */
/*    ViInt32 cdma1SHPDataResultsMaxSz                                       */
/*                                                                           */
/*      Maximum size of cdma1SHPDataResults array.                           */
/*                                                                           */
/*    ViPInt32 cdma1SHPDataResultsSz                                         */
/*                                                                           */
/*      Maximum size of cdma1SHPDataResults array.                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_SHPMeasureTrace
                       (ViSession instrumentHandle,
                        ViReal64 cdma1SHPMeasureTraceTimeout,
                        ViInt16 cdma1SHPMeasureTraceType,
                        ViInt16 cdma1SHPResultsType,
                        ViAReal64 cdma1SHPDataResults,
                        ViInt32 cdma1SHPDataResultsMaxSz,
                        ViPInt32 cdma1SHPDataResultsSz);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_SHPSetAveraging                                              */
/*                                                                           */
/*      This method sets up averaging for CDMA One spurs at harmonics (SHP)  */
/*      measurements. User can control the number of averages, turn on and   */
/*      off averaging, and set the terminal control mode for averaging.      */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 cdma1SHPAverageCount                                           */
/*                                                                           */
/*      Sets the number of data acquisitions that will be averaged. After    */
/*      the specified number of average counts, the averaging mode (terminal */
/*      control) determines the averaging action.                            */
/*                                                                           */
/*    ViBoolean cdma1SHPAverageState                                         */
/*                                                                           */
/*      Turns averaging on or off.                                           */
/*                                                                           */
/*      VI_TRUE - Turn averaging on.                                         */
/*      VI_FALSE - Turn averaging off.                                       */
/*                                                                           */
/*    ViInt16 cdma1SHPAverageMode                                            */
/*                                                                           */
/*      Selects the type of terminal control for averaging.                  */
/*                                                                           */
/*      hpesa_AVERAGE_EXPONENTIAL  - Each successive data acquisition after  */
/*      the average count is reached, is exponentially weighted and combined */
/*      with the existing average.                                           */
/*                                                                           */
/*      hpesa_AVERAGE_REPEAT - After reaching the average count, the         */
/*      averaging is reset and a new average is started.                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_SHPSetAveraging
                       (ViSession instrumentHandle,
                        ViInt32 cdma1SHPAverageCount,
                        ViBoolean cdma1SHPAverageState,
                        ViInt16 cdma1SHPAverageMode);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_SHPGetAveraging                                              */
/*                                                                           */
/*      This method retrieves the averaging setup for CDMA One spurs at      */
/*      harmonics (SHP) measurements.                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt32 cdma1SHPAverageCountQuery                                     */
/*                                                                           */
/*      Returns the current number of data acquisitions being averaged.      */
/*                                                                           */
/*    ViPBoolean cdma1SHPAverageStateQuery                                   */
/*                                                                           */
/*      Returns the current instrument average state.                        */
/*                                                                           */
/*      0 = VI_FALSE                                                         */
/*      1 = VI_TRUE                                                          */
/*                                                                           */
/*    ViPInt16 cdma1SHPAverageModeQuery                                      */
/*                                                                           */
/*      Returns the current instrument type of terminal control for          */
/*      averaging.                                                           */
/*      0 = hpesa_AVERAGE_EXPONENTIAL                                        */
/*      1 = hpesa_AVERAGE_REPEAT                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_SHPGetAveraging
                       (ViSession instrumentHandle,
                        ViPInt32 cdma1SHPAverageCountQuery,
                        ViPBoolean cdma1SHPAverageStateQuery,
                        ViPInt16 cdma1SHPAverageModeQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_SHPSetNumberHarmonics                                        */
/*                                                                           */
/*      This method sets the number of harmonics to be measured for CDMA One */
/*      spurs at harmonics (SHP) measurements.                               */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 cdma1SHPNumberHarmonics                                        */
/*                                                                           */
/*      Sets the new number of harmonics to be measured. The instrument      */
/*      range is 2 to 10, and the default is 3.                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_SHPSetNumberHarmonics
                       (ViSession instrumentHandle,
                        ViInt32 cdma1SHPNumberHarmonics);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_SHPGetNumberHarmonics                                        */
/*                                                                           */
/*      This method retrieves the current number of harmonics to be measured */
/*      for CDMA One spurs at harmonics (SHP) measurements.                  */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt32 cdma1SHPNumberHarmQuery                                       */
/*                                                                           */
/*      Returns the current number of harmonics being measured.              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_SHPGetNumberHarmonics
                       (ViSession instrumentHandle,
                        ViPInt32 cdma1SHPNumberHarmQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_RadioSetConditions                                           */
/*                                                                           */
/*      This method sets the radio conditions for CDMA One measurements.     */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 cdma1RadioStandardBand                                         */
/*                                                                           */
/*      Selects the standard variant that applies to the radio to be tested. */
/*                                                                           */
/*      hpesa_CDMA1_RADIO_BAND_IS95A - IS-95A Cellular                       */
/*      hpesa_CDMA1_RADIO_BAND_JSTD8 - J-STD-008 PCS                         */
/*      hpesa_CDMA1_RADIO_BAND_C95B - EIA/TIA-95B Cellular                   */
/*      hpesa_CDMA1_RADIO_BAND_P95B - EIA/TIA-95B (PCS)                      */
/*      hpesa_CDMA1_RADIO_BAND_CKOR - TTA.KO-06.0003 (Korea Cell)            */
/*      hpesa_CDMA1_RADIO_BAND_PKOR - TTA.KO-06.0013 (Korea PCS)             */
/*      hpesa_CDMA1_RADIO_BAND_P95C - EIA/TIA-95C (PCS)                      */
/*      hpesa_CDMA1_RADIO_BAND_C95C - EIA/TIA-95C Cellular                   */
/*      hpesa_CDMA1_RADIO_BAND_ARIBT53 - ARIB STD-T53                        */
/*                                                                           */
/*    ViInt16 cdma1RadioDevice                                               */
/*                                                                           */
/*      Selects the type of radio device to be tested.                       */
/*                                                                           */
/*      hpesa_CDMA1_RADIO_DEVICE_BASE - Base transceiver station test.       */
/*      hpesa_CDMA1_RADIO_DEVICE_MOBILE - Mobile transceiver station test.   */
/*                                                                           */
/*    ViReal64 cdma1RadioChannelBW                                           */
/*                                                                           */
/*      Sets the cdmaOne channel bandwidth.                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_RadioSetConditions
                       (ViSession instrumentHandle,
                        ViInt16 cdma1RadioStandardBand,
                        ViInt16 cdma1RadioDevice,
                        ViReal64 cdma1RadioChannelBW);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_RadioGetConditions                                           */
/*                                                                           */
/*      This method retrieves the current CDMA One radio conditions.         */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 cdma1RadioStandardBandQuery                                   */
/*                                                                           */
/*      Returns the current standard band.                                   */
/*                                                                           */
/*      0 = hpesa_CDMA1_RADIO_BAND_IS95A                                     */
/*      1 = hpesa_CDMA1_RADIO_BAND_JSTD8                                     */
/*      2 = hpesa_CDMA1_RADIO_BAND_C95B                                      */
/*      3 = hpesa_CDMA1_RADIO_BAND_P95B                                      */
/*      4 = hpesa_CDMA1_RADIO_BAND_CKOR                                      */
/*      5 = hpesa_CDMA1_RADIO_BAND_PKOR                                      */
/*      6 = hpesa_CDMA1_RADIO_BAND_P95C                                      */
/*      7 = hpesa_CDMA1_RADIO_BAND_C95C                                      */
/*      8 = hpesa_CDMA1_RADIO_BAND_ARIBT53                                   */
/*                                                                           */
/*    ViPInt16 cdma1RadioDeviceQuery                                         */
/*                                                                           */
/*      Returns the current radio device under test.                         */
/*                                                                           */
/*      0 = hpesa_CDMA1_RADIO_DEVICE_BASE                                    */
/*      1 = hpesa_CDMA1_RADIO_DEVICE_MOBILE                                  */
/*                                                                           */
/*    ViPReal64 cdma1RadioChannelBWQuery                                     */
/*                                                                           */
/*      Returns the current radio channel bandwidth in Hertz.                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_RadioGetConditions
                       (ViSession instrumentHandle,
                        ViPInt16 cdma1RadioStandardBandQuery,
                        ViPInt16 cdma1RadioDeviceQuery,
                        ViPReal64 cdma1RadioChannelBWQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_InputSetConditions                                           */
/*                                                                           */
/*      This methods sets the input conditions for CDMA One measurements.    */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViBoolean cdma1InputRFRangeState                                       */
/*                                                                           */
/*      Selects the RF port power range to be set either automatically or    */
/*      manually.                                                            */
/*                                                                           */
/*      VI_TRUE - power range is automatically set as determined by the      */
/*      actual measured power level at the start of a measurement.           */
/*                                                                           */
/*      VI_FALSE - power range is manually set                               */
/*                                                                           */
/*    ViReal64 cdma1InputMaxTotalPwr                                         */
/*                                                                           */
/*      Sets the maximum RF total power.  This value is set at its auto      */
/*      value if cdma1InputRFRangeState is set VI_TRUE.                      */
/*                                                                           */
/*    ViReal64 cdma1InputRFAtten                                             */
/*                                                                           */
/*      Sets the RF input attenuator. This value is set at its auto value if */
/*      cdma1InputRFRangeState is set VI_TRUE.                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_InputSetConditions
                       (ViSession instrumentHandle,
                        ViBoolean cdma1InputRFRangeState,
                        ViReal64 cdma1InputMaxTotalPwr,
                        ViReal64 cdma1InputRFAtten);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_InputGetConditions                                           */
/*                                                                           */
/*      This method retrieves the current input conditions for CDMA One      */
/*      measurements.                                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPBoolean cdma1InputRFRngeStateQuery                                  */
/*                                                                           */
/*      Returns whether the RF port power range is set automatically or      */
/*      manually.                                                            */
/*                                                                           */
/*      0 = VI_FALSE                                                         */
/*      1 = VI_TRUE                                                          */
/*                                                                           */
/*    ViPReal64 cdma1InputMaxTotalPwrQuery                                   */
/*                                                                           */
/*      Returns the current maximum RF total power.                          */
/*                                                                           */
/*    ViPReal64 cdma1InputRFAttenQuery                                       */
/*                                                                           */
/*      Returns the current RF input attenuation.                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_InputGetConditions
                       (ViSession instrumentHandle,
                        ViPBoolean cdma1InputRFRngeStateQuery,
                        ViPReal64 cdma1InputMaxTotalPwrQuery,
                        ViPReal64 cdma1InputRFAttenQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_InputSetGainAtten                                            */
/*                                                                           */
/*      This method sets the base station external attenuation and mobile    */
/*      station external gain for CDMA One measurements.                     */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 cdma1InputBSExtGain                                           */
/*                                                                           */
/*      Sets the correction equal to the external attenuation used when      */
/*      measuring base stations.                                             */
/*                                                                           */
/*    ViReal64 cdma1InputExtGain                                             */
/*                                                                           */
/*      Sets the gain of the external preamplifier for mobile station        */
/*      measurements.                                                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_InputSetGainAtten
                       (ViSession instrumentHandle,
                        ViReal64 cdma1InputBSExtGain,
                        ViReal64 cdma1InputExtGain);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_InputGetGainAtten                                            */
/*                                                                           */
/*      This method retrieves the current the base station external          */
/*      attenuation and mobile station external gain for CDMA One            */
/*      measurements.                                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPReal64 cdma1InputBSExtGainQuery                                     */
/*                                                                           */
/*      Returns the current external attenuation used when measuring base    */
/*      stations.                                                            */
/*                                                                           */
/*    ViPReal64 cdma1InputExtGainQuery                                       */
/*                                                                           */
/*      Returns the current gain of the external preamplifier for mobile     */
/*      station measurements.                                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_InputGetGainAtten
                       (ViSession instrumentHandle,
                        ViPReal64 cdma1InputBSExtGainQuery,
                        ViPReal64 cdma1InputExtGainQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_DemodSetConfig                                               */
/*                                                                           */
/*      This method sets the demod setting for CDMA One measurements.        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 cdma1DemodTimeRefSync                                          */
/*                                                                           */
/*      Sets the time reference to the external frame synchronization        */
/*      source.                                                              */
/*                                                                           */
/*      hpesa_DEMOD_TIME_REF_ESEC - External frame synchronization source.   */
/*      hpesa_DEMOD_TIME_REF_OFF - Turns off the synchronization source.     */
/*                                                                           */
/*    ViInt16 cdma1DemodTrigSource                                           */
/*                                                                           */
/*      Selects the trigger source used to control data acquisitions.        */
/*                                                                           */
/*      hpesa_DEMOD_TRIG_SOURCE_EXTERNAL - rear panel external trigger input */
/*                                                                           */
/*      hpesa_DEMOD_TRIG_SOURCE_FRAME - internal frame trigger from front    */
/*      panel input                                                          */
/*                                                                           */
/*      hpesa_DEMOD_TRIG_SOURCE_IMMEDIATE - the next data acquisition is     */
/*      immediately taken, capturing the signal asynchronously (also called  */
/*      free run).                                                           */
/*                                                                           */
/*      hpesa_DEMOD_TRIG_SOURCE_RFBURST - internal wideband RF burst         */
/*      envelope trigger that has automatic level control for periodic burst */
/*      signals. (Also called BCT)                                           */
/*                                                                           */
/*    ViInt32 cdma1DemodPNOffset                                             */
/*                                                                           */
/*      Sets the PN Offset in chips.                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_DemodSetConfig
                       (ViSession instrumentHandle,
                        ViInt16 cdma1DemodTimeRefSync,
                        ViInt16 cdma1DemodTrigSource,
                        ViInt32 cdma1DemodPNOffset);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_cdma1_DemodGetConfig                                               */
/*                                                                           */
/*      This method retrieves the current demod values for CDMA One          */
/*      measurements.                                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 cdma1DemodTimeRefSyncQuery                                    */
/*                                                                           */
/*      Returns the current external frame synchronization source.           */
/*                                                                           */
/*    ViPInt16 cdma1DemodTrigSourceQuery                                     */
/*                                                                           */
/*      Returns the current trigger source used to control data acquisition. */
/*                                                                           */
/*      0 = hpesa_DEMOD_TRIG_SOURCE_EXTERNAL                                 */
/*      1 = hpesa_DEMOD_TRIG_SOURCE_FRAME                                    */
/*      2 = hpesa_DEMOD_TRIG_SOURCE_IMMEDIATE                                */
/*      3 = hpesa_DEMOD_TRIG_SOURCE_RFBURST                                  */
/*                                                                           */
/*    ViPInt32 cdma1DemodPNOffsetQuery                                       */
/*                                                                           */
/*      Returns the current PN offset.                                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_cdma1_DemodGetConfig
                       (ViSession instrumentHandle,
                        ViPInt16 cdma1DemodTimeRefSyncQuery,
                        ViPInt16 cdma1DemodTrigSourceQuery,
                        ViPInt32 cdma1DemodPNOffsetQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_RadioSetConditions                                             */
/*                                                                           */
/*      This method sets the radio conditions for GSM measurements           */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 GSMRadioStandardBand                                           */
/*                                                                           */
/*      Selects the standard variant that applies to the radio to be tested. */
/*                                                                           */
/*      hpesa_GSM_RADIO_BAND_PGSM - P-GSM900                                 */
/*      hpesa_GSM_RADIO_BAND_EGSM - E-GSM900                                 */
/*      hpesa_GSM_RADIO_BAND_RGSM - R-GSM900                                 */
/*      hpesa_GSM_RADIO_BAND_DCS - DCS-1800                                  */
/*      hpesa_GSM_RADIO_BAND_PCS - PCS-1900                                  */
/*                                                                           */
/*    ViInt16 GSMRadioDevice                                                 */
/*                                                                           */
/*      Selects the type of radio device to be tested.                       */
/*                                                                           */
/*      hpesa_GSM_RADIO_DEVICE_MS - Mobile station transmitter test          */
/*      hpesa_GSM_RADIO_DEVICE_BTS - Base station transmitter test           */
/*      hpesa_GSM_RADIO_DEVICE_UBTS1 - Micro base station class M1           */
/*      transmitter test                                                     */
/*      hpesa_GSM_RADIO_DEVICE_UBTS2 - Micro base station class M2           */
/*      transmitter test                                                     */
/*      hpesa_GSM_RADIO_DEVICE_UBTS3 - Micro base station class M3           */
/*      transmitter test                                                     */
/*                                                                           */
/*    ViBoolean GSMFreqHopState                                              */
/*                                                                           */
/*      Sets the signal’s frequency hopping repetition factor on or off.     */
/*                                                                           */
/*      VI_TRUE - Turns on frequency hopping repetition factor.              */
/*      VI_FALSE - Turns off frequency hopping repetition factor.            */
/*                                                                           */
/*    ViInt32 GSMFreqHopFactor                                               */
/*                                                                           */
/*      Sets the frequency hopping repetition factor.  Range is between 1    */
/*      and 100.                                                             */
/*                                                                           */
/*    ViInt16 GSMOverlapPriority                                             */
/*                                                                           */
/*      Selects a priority band when entering an ARFCN that is common to     */
/*      more than one band.                                                  */
/*                                                                           */
/*      hpesa_GSM_PRIORITY_BAND_DCS - Selects the DCS priority band.         */
/*      hpesa_GSM_PRIORITY_BAND_PCS - Selects the PCS priority band.         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_RadioSetConditions
                       (ViSession instrumentHandle,
                        ViInt16 GSMRadioStandardBand,
                        ViInt16 GSMRadioDevice,
                        ViBoolean GSMFreqHopState,
                        ViInt32 GSMFreqHopFactor,
                        ViInt16 GSMOverlapPriority);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_RadioGetConditions                                             */
/*                                                                           */
/*      This method retrieves the current radio conditions for GSM           */
/*      measurements.                                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 GSMRadioStandardBandQuery                                     */
/*                                                                           */
/*      Returns the current standard variant that applies to the radio to be */
/*      tested.                                                              */
/*                                                                           */
/*      0 = hpesa_GSM_RADIO_BAND_PGSM                                        */
/*      1 = hpesa_GSM_RADIO_BAND_EGSM                                        */
/*      2 = hpesa_GSM_RADIO_BAND_RGSM                                        */
/*      3 = hpesa_GSM_RADIO_BAND_DCS                                         */
/*      4 = hpesa_GSM_RADIO_BAND_PCS                                         */
/*                                                                           */
/*    ViPInt16 GSMRadioDeviceQuery                                           */
/*                                                                           */
/*      Returns the current type of radio device to be tested.               */
/*                                                                           */
/*      0 = hpesa_GSM_RADIO_DEVICE_MS                                        */
/*      1 = hpesa_GSM_RADIO_DEVICE_BTS                                       */
/*      2 = hpesa_GSM_RADIO_DEVICE_UBTS1                                     */
/*      3 = hpesa_GSM_RADIO_DEVICE_UBTS2                                     */
/*      4 = hpesa_GSM_RADIO_DEVICE_UBTS3                                     */
/*                                                                           */
/*    ViPBoolean GSMFreqHopStateQuery                                        */
/*                                                                           */
/*      Returns the current frequency hopping repetition factor state.       */
/*                                                                           */
/*      0 = VI_FALSE                                                         */
/*      1 = VI_TRUE                                                          */
/*                                                                           */
/*    ViPInt32 GSMFreqHopFactorQuery                                         */
/*                                                                           */
/*      Returns the current frequency hopping repetition factor.             */
/*                                                                           */
/*    ViPInt16 GSMOverlapPriorityQuery                                       */
/*                                                                           */
/*      Returns the current priority overlap band.                           */
/*                                                                           */
/*      0 = hpesa_GSM_PRIORITY_BAND_DCS                                      */
/*      1 = hpesa_GSM_PRIORITY_BAND_PCS                                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_RadioGetConditions
                       (ViSession instrumentHandle,
                        ViPInt16 GSMRadioStandardBandQuery,
                        ViPInt16 GSMRadioDeviceQuery,
                        ViPBoolean GSMFreqHopStateQuery,
                        ViPInt32 GSMFreqHopFactorQuery,
                        ViPInt16 GSMOverlapPriorityQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_InputSetConditions                                             */
/*                                                                           */
/*      This method sets the input conditions for GSM measurements.          */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 GSM_RFCarrier                                                  */
/*                                                                           */
/*      Switch between single and multiple carrier modes.                    */
/*      																	 */
/*		hpesa_INPUT_CARRIER_SINGLE - Selects single carrier mode.	         */
/*      hpesa_INPUT_CARRIER_MULTIPLE - Selects multiple carrier mode.      . */
/*                                                                           */
/*    ViBoolean GSM_RFInputRangeState                                        */
/*                                                                           */
/*      Selects the RF port power range to be set either automatically or	 */
/*		manually.		                                                     */
/*                                                                           */
/*    ViReal64 GSMMaxTotalPower                                              */
/*                                                                           */
/*      Sets the maximum total power (in dBm) to be applied at the RF input	 */
/*      The instrument range is -100 to +80 dBm. This value is ignored       */
/*      and instead set by the instrument to the measured value if the RF    */
/*      Input Range is Man (i.e. GSMRFInputRangeState = VI_TRUE).            */
/*                                                                           */
/*    ViReal64 GSM_RFInputAttenuation                                        */
/*                                                                           */
/*      Set the RF input attenuator in dB. The instrument range is 0 to      */
/*      75 dB in 5 dB steps. This value is ignored and instead set by the 	 */
/*      instrument to it's auto value if the RF Input Range is set to Auto.	 */
/*      (i.e. GSMRFInputRangeState = VI_TRUE).                               */
/*     																		 */
/*    ViBoolean GSMInternalPreAmpState                                       */
/*		                                                                     */
/*		  Turns the internal preamp on or off for the currently selected	 */
/*		  measurement.                                                       */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_InputSetConditions
                       (ViSession instrumentHandle,
                        ViInt16 GSM_RFCarrier,
                        ViBoolean GSM_RFInputRangeState,
                        ViReal64 GSMMaxTotalPower,
						ViReal64 GSM_RFInputAttenuation,
						ViBoolean GSMInternalPreAmpState);
/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_InputGetConditions                                             */
/*                                                                           */
/*      This method retrieves the input conditions for GSM measurements.     */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 GSM_RFCarrierQuery                                             */
/*         																	 */
/*       Returns the current RF Carriers.                                    */
/*                                                                           */
/*		 0 = hpesa_INPUT_CARRIER_SINGLE                                      */
/*       1 = hpesa_INPUT_CARRIER_MULTIPLE     .                              */
/*                                                                           */
/*    ViBoolean GSM_RFInputRangeStateQuery                                   */
/*                                                                           */
/*      Returns the current RF input range state                        	 */
/*																			 */
/*		0 = VI_FALSE														 */
/*		1 = VI_TRUE															 */
/*			                                                                 */
/*          																 */                                                
/*    ViReal64 GSMMaxTotalPowerQuery                                         */
/*                                                                           */
/*      Returns the current maximum total power in dBm. 					 */
/*                                                                           */
/*    ViReal64 GSM_RFInputAttenuationQuery                                   */
/*                                                                           */
/*      Returns the RF input attenuator in dB.                               */
/*     																		 */
/*    ViBoolean GSMInternalPreAmpStateQuery                                  */
/*		 																	 */
/*		Returns the current state of the internal preamp                     */                                               
/*---------------------------------------------------------------------------*/

ViStatus _VI_FUNC hpesa_GSM_InputGetConditions
                       (ViSession instrumentHandle,
                        ViPInt16 GSM_RFCarrierQuery,
                        ViPBoolean GSM_RFInputRangeStateQuery,
                        ViPReal64 GSMMaxTotalPowerQuery,
						ViPReal64 GSM_RFInputAttenuationQuery,
						ViPBoolean GSMInternalPreAmpStateQuery);




/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_InputSetGainAtten                                              */
/*                                                                           */
/*      This method sets the gain and attenuation for base transmit and      */
/*      mobile testing stations.                                             */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 GSMMobileGain                                                 */
/*                                                                           */
/*      Sets equal to the external gain used when measuring mobile stations. */
/*                                                                           */
/*    ViReal64 GSMMobileAtten                                                */
/*                                                                           */
/*      Sets equal to the external attenuation used when measuring mobile    */
/*      stations.                                                            */
/*                                                                           */
/*    ViReal64 GSMBaseTransGain                                              */
/*                                                                           */
/*      Set equal to the external gain used when measuring base transmit     */
/*      stations.                                                            */
/*                                                                           */
/*    ViReal64 GSMBaseTransAtten                                             */
/*                                                                           */
/*      Sets equal to the external attenuation used when measuring base      */
/*      transmit stations.                                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_InputSetGainAtten
                       (ViSession instrumentHandle,
                        ViReal64 GSMMobileGain,
                        ViReal64 GSMMobileAtten,
                        ViReal64 GSMBaseTransGain,
                        ViReal64 GSMBaseTransAtten);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_InputGetGainAtten                                              */
/*                                                                           */
/*      This method retrieves the current gain and attenuation settings for  */
/*      base transmit and mobile stations.                                   */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPReal64 GSMMobileGainQuery                                           */
/*                                                                           */
/*      Returns the current external gain for mobile stations.               */
/*                                                                           */
/*    ViPReal64 GSMMobileAttenQuery                                          */
/*                                                                           */
/*      Returns the current external attenuation for mobile stations.        */
/*                                                                           */
/*    ViPReal64 GSMBaseTransGainQuery                                        */
/*                                                                           */
/*      Returns the current gain for base transmit stations.                 */
/*                                                                           */
/*    ViPReal64 GSMBaseTransAttenQuery                                       */
/*                                                                           */
/*      Returns the current attenuation for base transmit stations.          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_InputGetGainAtten
                       (ViSession instrumentHandle,
                        ViPReal64 GSMMobileGainQuery,
                        ViPReal64 GSMMobileAttenQuery,
                        ViPReal64 GSMBaseTransGainQuery,
                        ViPReal64 GSMBaseTransAttenQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TriggerSetExternal                                             */
/*                                                                           */
/*      This method sets the external trigger values for GSM measurements.   */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 GSMTrigExtDelay                                               */
/*                                                                           */
/*      Sets the trigger delay in seconds when using the rear panel external */
/*      trigger.                                                             */
/*                                                                           */
/*    ViInt16 GSMTrigExtSlope                                                */
/*                                                                           */
/*      Sets the trigger slope when using the rear panel external trigger    */
/*      input.                                                               */
/*                                                                           */
/*      hpesa_TRIG_SLOPE_POS - Sets the external trigger slope to positive.  */
/*      hpesa_TRIG_SLOPE_NEG - Sets the external trigger slope to negative.  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TriggerSetExternal
                       (ViSession instrumentHandle,
                        ViReal64 GSMTrigExtDelay,
                        ViInt16 GSMTrigExtSlope);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TriggerGetExternal                                             */
/*                                                                           */
/*      This method retrieves the current external trigger values for GSM    */
/*      measurements.                                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPReal64 GSMTrigExtDelayQuery                                         */
/*                                                                           */
/*      Retrieves the current delay in seconds for external triggering.      */
/*                                                                           */
/*    ViPInt16 GSMTrigExtSlopeQuery                                          */
/*                                                                           */
/*      Returns the current slope used with external triggering.             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TriggerGetExternal
                       (ViSession instrumentHandle,
                        ViPReal64 GSMTrigExtDelayQuery,
                        ViPInt16 GSMTrigExtSlopeQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TriggerSetFrame                                                */
/*                                                                           */
/*      This method sets the frame trigger conditions for GSM measurements.  */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 GSMTrigFrameDelay                                             */
/*                                                                           */
/*      Sets trigger delay (in seconds) to be used in zero span measurements */
/*      to adjust the active burst within a mask. Use positive values to     */
/*      achieve trigger delay (that is, to measure later than the trigger    */
/*      source event) and use negative values to achieve pre-trigger (that   */
/*      is, to measure earlier than the trigger source event).               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TriggerSetFrame
                       (ViSession instrumentHandle,
                        ViReal64 GSMTrigFrameDelay);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TriggerGetFrame                                                */
/*                                                                           */
/*      This method retrieves the current setting for GSM frame triggering.  */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPReal64 GSMTrigFrameDelayQuery                                       */
/*                                                                           */
/*      Returns the current frame delay in seconds.                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TriggerGetFrame
                       (ViSession instrumentHandle,
                        ViPReal64 GSMTrigFrameDelayQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TriggerSetRFBurst                                              */
/*                                                                           */
/*      This method sets up RF burst triggering for GSM measurements.        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 GSMTrigRFBurstDelay                                           */
/*                                                                           */
/*      Sets the trigger delay when using the RF burst (wideband) trigger.   */
/*                                                                           */
/*    ViReal64 GSMTrigRFBurstLevel                                           */
/*                                                                           */
/*      Sets the trigger level when using the RF burst (wideband) trigger.   */
/*                                                                           */
/*    ViInt16 GSMTrigRFBurstSlope                                            */
/*                                                                           */
/*      Sets the trigger slope when using the RF Burst (wideband) Trigger.   */
/*                                                                           */
/*      hpesa_TRIG_SLOPE_POS - Sets the RF Burst trigger slope to positive.  */
/*      hpesa_TRIG_SLOPE_NEG - Sets the RF Burst trigger slope to negative.  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TriggerSetRFBurst
                       (ViSession instrumentHandle,
                        ViReal64 GSMTrigRFBurstDelay,
                        ViReal64 GSMTrigRFBurstLevel,
                        ViInt16 GSMTrigRFBurstSlope);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TriggerGetRFBurst                                              */
/*                                                                           */
/*      This method retrieves the current RF Burst trigger settings for GSM  */
/*      measurements.                                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPReal64 GSMTrigRFBurstDelayQuery                                     */
/*                                                                           */
/*      Returns the current RF Burst trigger delay.                          */
/*                                                                           */
/*    ViPReal64 GSMTrigRFBurstLevelQuery                                     */
/*                                                                           */
/*      Returns the current RB Burst trigger level.                          */
/*                                                                           */
/*    ViPInt16 GSMTrigRFBurstSlopeQuery                                      */
/*                                                                           */
/*      Returns the current RF Burst trigger slope.                          */
/*                                                                           */
/*      0 = hpesa_TRIG_SLOPE_POS                                             */
/*      1 = hpesa_TRIG_SLOPE_NEG                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TriggerGetRFBurst
                       (ViSession instrumentHandle,
                        ViPReal64 GSMTrigRFBurstDelayQuery,
                        ViPReal64 GSMTrigRFBurstLevelQuery,
                        ViPInt16 GSMTrigRFBurstSlopeQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_DemodSetBurstAlign                                             */
/*                                                                           */
/*      This method sets the demod burst alignment options for GSM           */
/*      measurements.                                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 GSMDemodBurstAlign                                             */
/*                                                                           */
/*      Selects the sync alignment to be either to the GSM standard or the   */
/*      standard offset by 1/2 bit.                                          */
/*                                                                           */
/*      hpesa_GSM_BURST_ALIGN_GSM - burst alignment as defined in the GSM    */
/*      standard.                                                            */
/*      hpesa_GSM_BURST_ALIGN_HALF - burst alignment is advanced by1/2 bit,  */
/*      which corresponds to an earlier interpretation of the GSM standard.  */
/*                                                                           */
/*    ViReal64 GSMDemodRFSyncDelay                                           */
/*                                                                           */
/*      Sets the delay for the RF amplitude sync in seconds.                 */
/*                                                                           */
/*    ViReal64 GSMDemodSearchThresh                                          */
/*                                                                           */
/*      Sets the power threshold (in dB), relative to the peak power, that   */
/*      is used to determine the burst rising edge and falling edge.         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_DemodSetBurstAlign
                       (ViSession instrumentHandle,
                        ViInt16 GSMDemodBurstAlign,
                        ViReal64 GSMDemodRFSyncDelay,
                        ViReal64 GSMDemodSearchThresh);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_DemodGetBurstAlign                                             */
/*                                                                           */
/*      This method retrieves the current burst alignment setting for GSM    */
/*      measurements.                                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 GSMDemodBurstAlignQuery                                       */
/*                                                                           */
/*      Returns the current sync alignment standard.                         */
/*                                                                           */
/*      0 = hpesa_GSM_BURST_ALIGN_GSM                                        */
/*      1 = hpesa_GSM_BURST_ALIGN_HALF                                       */
/*                                                                           */
/*    ViPReal64 GSMDemodRFSyncDelayQuery                                     */
/*                                                                           */
/*      Returns the current delay for the RF amplitude sync.                 */
/*                                                                           */
/*    ViPReal64 GSMDemodSearchThreshQuery                                    */
/*                                                                           */
/*      Returns the current power threshold, relative to the peak power,     */
/*      that is used to determine the burst rising edge and falling edge.    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_DemodGetBurstAlign
                       (ViSession instrumentHandle,
                        ViPInt16 GSMDemodBurstAlignQuery,
                        ViPReal64 GSMDemodRFSyncDelayQuery,
                        ViPReal64 GSMDemodSearchThreshQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_DemodSetReferenceOsc                                           */
/*                                                                           */
/*      This method sets the demod optional reference setup for GSM          */
/*      measurements.                                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 GSMOptFreqRefSource                                            */
/*                                                                           */
/*      Selects the reference oscillator (time base) source.                 */
/*                                                                           */
/*      hpesa_GSM_DEMOD_FREQ_SOURCE_INTERNAL - uses internal 10 MHz          */
/*      reference signal.                                                    */
/*      hpesa_GSM_DEMOD_FREQ_SOURCE_EXTERNAL - uses the signal at the rear   */
/*      panel external reference input port.                                 */
/*                                                                           */
/*    ViReal64 GSMOptFreqReference                                           */
/*                                                                           */
/*      Set to the frequency of the external reference signal being supplied */
/*      to the instrument’s 10MHZ REF IN input BNC connector. If the         */
/*      external frequency reference signal is not 10MHz, the following      */
/*      procedure must be followed:                                          */
/*                                                                           */
/*      Connect the external reference signal to the DDRF board EXT REF      */
/*      IN input BNC connector.                                              */
/*                                                                           */
/*      Connect the DDRF board EXT 10MHZ OUT output BNC to the SA            */
/*      base box 10MHZ REF IN input BNC connector.                           */
/*                                                                           */
/*      Enter the external reference signal frequency value as the Opt Freq  */
/*      Ref parameter.                                                       */
/*                                                                           */
/*      Set Opt Freq Ref = External                                          */
/*                                                                           */
/*      The DDRF Option B7E reference PLL circuit will attempt to lock to    */
/*      the external reference signal and if successful will produce a phase */
/*      locked 10MHz reference on the DDRF board EXT 10MHZ OUT output BNC,   */
/*      which in turn will drive the base box 10MHZ REF IN.                  */
/*                                                                           */
/*    ViBoolean GSMOpt10MHZOut                                               */
/*                                                                           */
/*      Turn on and off the external reference signal going to the rear      */
/*      panel.                                                               */
/*                                                                           */
/*      VI_TRUE - Turns on external reference signal.                        */
/*      VI_FALSE - Turns off external reference signal.                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_DemodSetReferenceOsc
                       (ViSession instrumentHandle,
                        ViInt16 GSMOptFreqRefSource,
                        ViReal64 GSMOptFreqReference,
                        ViBoolean GSMOpt10MHZOut);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_DemodGetReferenceOsc                                           */
/*                                                                           */
/*      This method retrieves the current demod option reference setup for   */
/*      GSM measurements.                                                    */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 GSMOptFreqRefSourceQuery                                      */
/*                                                                           */
/*      Returns the current reference oscillator (time base) source.         */
/*                                                                           */
/*      0 = hpesa_GSM_DEMOD_FREQ_SOURCE_INTERNAL                             */
/*      1 = hpesa_GSM_DEMOD_FREQ_SOURCE_EXTERNAL                             */
/*                                                                           */
/*    ViPReal64 GSMOptFreqReferenceQuery                                     */
/*                                                                           */
/*      Returns the current frequency of the external reference signal being */
/*      supplied to the instrument’s 10MHZ REF IN input BNC connector.       */
/*                                                                           */
/*    ViPBoolean GSMOpt10MHZOutQuery                                         */
/*                                                                           */
/*      Returns the current state of the external reference signal going to  */
/*      the rear panel.                                                      */
/*                                                                           */
/*      0 = VI_FALSE                                                         */
/*      1 = VI_TRUE                                                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_DemodGetReferenceOsc
                       (ViSession instrumentHandle,
                        ViPInt16 GSMOptFreqRefSourceQuery,
                        ViPReal64 GSMOptFreqReferenceQuery,
                        ViPBoolean GSMOpt10MHZOutQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_SetARFCN                                                       */
/*                                                                           */
/*      This method sets the Absolute RF Channel Number for GSM measurements.*/
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 GSM_ARFCN                                                      */
/*                                                                           */
/*      Set the analyzer to a frequency that corresponds to the ARFCN        */
/*      (Absolute RF Channel Number).                                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_SetARFCN
                       (ViSession instrumentHandle,
                        ViInt32 GSM_ARFCN);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_GetARFCN                                                       */
/*                                                                           */
/*      This method retrieves the current ARFCN for GSM measurements.        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt32 GSM_ARFCNQuery                                                */
/*                                                                           */
/*      Returns the current absolute RF channel number.                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_GetARFCN
                       (ViSession instrumentHandle,
                        ViPInt32 GSM_ARFCNQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_SetTSC                                                         */
/*                                                                           */
/*      This method sets up the training sequence code for GSM measurements. */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 GSM_TSCAuto                                                    */
/*                                                                           */
/*      Select auto or manual control for slot searching. The feature is     */
/*      only supported in external and frame trigger source modes. In        */
/*      external trigger mode when timeslot is set on, the demodulation      */
/*      measurement is made on the nth timeslot specified by the external    */
/*      trigger point + n timeslots, where n is the selected timeslot value  */
/*      0 to 7. In frame trigger mode when timeslot is set on, then          */
/*      demodulation measurement is only made on the nth timeslot specified  */
/*      by bit 0 of frame reference burst + n timeslots, where n is the      */
/*      selected timeslot value 0 to 7 and where the frame reference burst   */
/*      is specified by Ref Burst and Ref TSC (Std) combination.             */
/*                                                                           */
/*      hpesa_TSC_AUTO - Automatically sets the training sequence code.      */
/*      hpesa_TSC_MANUAL - Manually sets the training sequence code.         */
/*                                                                           */
/*    ViInt32 GSM_TSC                                                        */
/*                                                                           */
/*      Set the training sequence code to search for, with normal burst      */
/*      selected and TSC auto set to off.                                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_SetTSC
                       (ViSession instrumentHandle,
                        ViInt16 GSM_TSCAuto,
                        ViInt32 GSM_TSC);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_GetTSC                                                         */
/*                                                                           */
/*      This method retrieves the current training sequence code settins for */
/*      GSM measurements.                                                    */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 GSM_TSCAutoQuery                                              */
/*                                                                           */
/*      Returns the current training sequence code state.                    */
/*                                                                           */
/*      0 = hpesa_TSC_AUTO                                                   */
/*      1 = hpesa_TSC_MANUAL                                                 */
/*                                                                           */
/*    ViPInt32 GSM_TSCQuery                                                  */
/*                                                                           */
/*      Returns the current training sequence code.                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_GetTSC
                       (ViSession instrumentHandle,
                        ViPInt16 GSM_TSCAutoQuery,
                        ViPInt32 GSM_TSCQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TXPConfigure                                                   */
/*                                                                           */
/*      This method sets up the instrument for transmit power (TXP)          */
/*      measurements using the factory default instrument settings and stops */
/*      the current measurement. It does not initiate the taking of          */
/*      measurement data.                                                    */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TXPConfigure
                       (ViSession instrumentHandle);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TXPMeasure                                                     */
/*                                                                           */
/*      This method measures the power in the channel. It compares the       */
/*      average power of the RF signal burst to a specified threshold value. */
/*                                                                           */
/*      The user can select one of three types of measurement modes:         */
/*      Measure, Read, or Fetch.                                             */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 GSM_TXPMeasureTimeout                                         */
/*                                                                           */
/*      This value temporarily changes the VISA timeout setting for the      */
/*      duration of the instrument measurement. The user needs to set this   */
/*      value to an appropriate time that will be longer then the expected   */
/*      measurement time. Units are in seconds.                              */
/*                                                                           */
/*    ViInt16 GSM_TXPMeasureType                                             */
/*                                                                           */
/*      Selects measurement type. Note that for hpesa_FETCH type the         */
/*      instrument should already be in this measurement mode else the       */
/*      function will time out. User can select this measurement mode by     */
/*      first using the "MEASURE" or "READ" types or call the                */
/*      hpesa_GSM_TXPConfigure function.                                     */
/*                                                                           */
/*      hpesa_MEASURE - Stops current measurement, changes to this selected  */
/*      measurement, sets up instrument using factory defaults, makes        */
/*      measurement, returns measurement results.                            */
/*                                                                           */
/*      hpesa_READ - Stops current measurement,  changes to this selected    */
/*      measurement, uses instrument settings from last measurement of this  */
/*      type, makes measurement, returns measurements results.               */
/*                                                                           */
/*      hpesa_FETCH - Returns measurements results only. No new measurement  */
/*      is initiated.                                                        */
/*                                                                           */
/*    ViPReal64 GSM_TXPSampleTimeQ                                           */
/*                                                                           */
/*      Returns a floating point number representing the time between        */
/*      samples when using the trace queries (n=0, 2, et cetera).            */
/*                                                                           */
/*    ViPReal64 GSM_TXPMeanPowerQ                                            */
/*                                                                           */
/*      Returns the mean power (in dBm) of the power above the threshold     */
/*      value. If averaging is on, the power is for the latest acquisition.  */
/*                                                                           */
/*    ViPReal64 GSM_TXPAvgThreshPowerQ                                       */
/*                                                                           */
/*      Returns the threshold power (in dBm) for N averages, if averaging is */
/*      on. An average consists of N acquisitions of data which represents   */
/*      the current trace. If averaging is off, the value of power averaged  */
/*      is the same as the power value.                                      */
/*                                                                           */
/*    ViPReal64 GSM_TXPSampleCountQ                                          */
/*                                                                           */
/*      Returns the number of data points in the captured signal.            */
/*      This number is useful when performing a query on the signal.         */
/*                                                                           */
/*    ViPReal64 GSM_TXPThreshValueQ                                          */
/*                                                                           */
/*      Returns the threshold (in dBm) above which the power is              */
/*      calculated.                                                          */
/*                                                                           */
/*    ViPReal64 GSM_TXPThreshPointsQ                                         */
/*                                                                           */
/*      Returns the number of points that were above the threshold and were  */
/*      used for the power calculation.                                      */
/*                                                                           */
/*    ViPReal64 GSM_TXPMaxDataValueQ                                         */
/*                                                                           */
/*      Returns the maximum of the most recently acquired data (in           */
/*      dBm).                                                                */
/*                                                                           */
/*    ViPReal64 GSM_TXPMinDataValue                                          */
/*                                                                           */
/*      Returns the minimum of the most recently acquired data (in           */
/*      dBm).                                                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TXPMeasure
                       (ViSession instrumentHandle,
                        ViReal64 GSM_TXPMeasureTimeout,
                        ViInt16 GSM_TXPMeasureType,
                        ViPReal64 GSM_TXPSampleTimeQ,
                        ViPReal64 GSM_TXPMeanPowerQ,
                        ViPReal64 GSM_TXPAvgThreshPowerQ,
                        ViPReal64 GSM_TXPSampleCountQ,
                        ViPReal64 GSM_TXPThreshValueQ,
                        ViPReal64 GSM_TXPThreshPointsQ,
                        ViPReal64 GSM_TXPMaxDataValueQ,
                        ViPReal64 GSM_TXPMinDataValue);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TXPMeasureTrace                                                */
/*                                                                           */
/*      This method measures the power in the channel. It compares the       */
/*      average power of the RF signal burst to a specified threshold value. */
/*                                                                           */
/*      User can select one of three types of measurement modes: Measure,    */
/*      Read, or Fetch.                                                      */
/*                                                                           */
/*                                                                           */
/*      This method returns the TXP measurement results specified by the     */
/*      "GSM_TXPResultsType" parameter as a real array.                      */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 GSM_TXPMeasureTraceTimeout                                    */
/*                                                                           */
/*      This value temporarily changes the VISA timeout setting for the      */
/*      duration of the instrument measurement. The user needs to set this   */
/*      value to an appropriate time that will be longer then the expected   */
/*      measurement time. Units are in seconds.                              */
/*                                                                           */
/*    ViInt16 GSM_TXPMeasureTraceType                                        */
/*                                                                           */
/*      Selects measurement type. Note that for hpesa_FETCH type the         */
/*      instrument should already be in this measurement mode else the       */
/*      function will time out. User can select this measurement mode by     */
/*      first using the "MEASURE" or "READ" types or call the                */
/*      hpesa_GSM_TXPConfigure function.                                     */
/*                                                                           */
/*      hpesa_MEASURE - Stops current measurement, changes to this selected  */
/*      measurement, sets up instrument using factory defaults, makes        */
/*      measurement, returns measurement results.                            */
/*                                                                           */
/*      hpesa_READ - Stops current measurement,  changes to this selected    */
/*      measurement, uses instrument settings from last measurement of this  */
/*      type, makes measurement, returns measurements results.               */
/*                                                                           */
/*      hpesa_FETCH - Returns measurements results only. No new measurement  */
/*      is initiated.                                                        */
/*                                                                           */
/*    ViInt16 GSM_TXPResultsType                                             */
/*                                                                           */
/*      Determines the format of the data returned in GSM_TXPDataResults.    */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_0: Not supported by instrument.                   */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_1: Returns the following scalar results:          */
/*      1. Sample time is a floating point number representing the time      */
/*      between samples when using the trace queries (n=0, 2, et cetera).    */
/*      2. Power is the mean power (in dBm) of the power above the threshold */
/*      value. If averaging is on, the power is for the latest acquisition.  */
/*      3. Power averaged is the threshold power (in dBm) for N averages, if */
/*      averaging is on. An average consists of N acquisitions of data which */
/*      represents the current trace. If averaging is off, the value of      */
/*      power averaged is the same as the power value.                       */
/*      4. Number of samples is the number of data points in the captured    */
/*      signal. This number is useful when performing a query on the signal  */
/*      (that is when n=0, 2, et cetera).                                    */
/*      5. Threshold value is the threshold (in dBm) above which the power   */
/*      is calculated.                                                       */
/*      6. Threshold points is the number of points that were above the      */
/*      threshold and were used for the power calculation.                   */
/*      7. Maximum value is the maximum of the most recently acquired data   */
/*      (in dBm).                                                            */
/*      8. Minimum value is the minimum of the most recently acquired data   */
/*      (in dBm).                                                            */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_2: Returns the RF Envelope Trace (data array).    */
/*      This array contains 401 points of data.                              */
/*                                                                           */
/*    ViAReal64 GSM_TXPDataResults                                           */
/*                                                                           */
/*      Trace data returned by measurement, formatted according to           */
/*      GSM_TXPResultsType.                                                  */
/*                                                                           */
/*    ViInt32 GSM_TXPDataResultsMaxSize                                      */
/*                                                                           */
/*      Specifies the maximum size of GSM_TXPDataResults array.              */
/*                                                                           */
/*    ViPInt32 GSM_TXPDataResultsSize                                        */
/*                                                                           */
/*      Returns the actual size of GSM_TXPDataResults array.                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TXPMeasureTrace
                       (ViSession instrumentHandle,
                        ViReal64 GSM_TXPMeasureTraceTimeout,
                        ViInt16 GSM_TXPMeasureTraceType,
                        ViInt16 GSM_TXPResultsType,
                        ViAReal64 GSM_TXPDataResults,
                        ViInt32 GSM_TXPDataResultsMaxSize,
                        ViPInt32 GSM_TXPDataResultsSize);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TXPSetAveraging                                                */
/*                                                                           */
/*      This method sets the averaging for GSM transmit power (TXP)          */
/*      measurements.  User can control the number of averages, turn off or  */
/*      on averaging, set the terminal control mode for averaging, and set   */
/*      the average type.                                                    */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 GSM_TXPAverageCount                                            */
/*                                                                           */
/*      Set the number of bursts that will be averaged. After the specified  */
/*      number of bursts (average counts), the averaging mode (terminal      */
/*      control) setting determines the averaging action.                    */
/*                                                                           */
/*    ViBoolean GSM_TXPAverageState                                          */
/*                                                                           */
/*      Sets the state of averaging, either on or off.                       */
/*                                                                           */
/*      VI_TRUE = Averaging on.                                              */
/*      VI_FALSE = Averaging off.                                            */
/*                                                                           */
/*    ViInt16 GSM_TXPAverageMode                                             */
/*                                                                           */
/*      Select the type of termination control used for the averaging        */
/*      function.  This determines the averaging action after the specified  */
/*      number of frames (average count) is reached.                         */
/*                                                                           */
/*      hpesa_AVERAGE_EXPONENTIAL - Each successive data acquisition after   */
/*      the average count is reached, is exponentially weighted and combined */
/*      with the existing average.                                           */
/*                                                                           */
/*      hpesa_AVERAGE_REPEAT - After reaching the average count, the         */
/*      averaging is reset and a new average is started.                     */
/*                                                                           */
/*    ViInt16 GSM_TXPAverageType                                             */
/*                                                                           */
/*      Select the type of averaging to be performed.                        */
/*                                                                           */
/*      hpesa_AVERAGE_LPOWER - logarithmically averages the power of the     */
/*      video data.                                                          */
/*      hpesa_AVERAGE_POWER  - averages the linear power of successive       */
/*      measurements.                                                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TXPSetAveraging
                       (ViSession instrumentHandle,
                        ViInt32 GSM_TXPAverageCount,
                        ViBoolean GSM_TXPAverageState,
                        ViInt16 GSM_TXPAverageMode,
                        ViInt16 GSM_TXPAverageType);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TXPGetAveraging                                                */
/*                                                                           */
/*      This method retrieves the average settings for GSM tranmit power     */
/*      measurements.                                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt32 GSM_TXPAverageCountQuery                                      */
/*                                                                           */
/*      Returns the current average count.                                   */
/*                                                                           */
/*    ViPBoolean GSM_TXPAverageStateQuery                                    */
/*                                                                           */
/*      Returns the current average state.                                   */
/*                                                                           */
/*      0 = VI_FALSE                                                         */
/*      1 = VI_TRUE                                                          */
/*                                                                           */
/*    ViPInt16 GSM_TXPAverageModeQuery                                       */
/*                                                                           */
/*      Returns the current average mode.                                    */
/*                                                                           */
/*      0 = hpesa_AVERAGE_EXPONENTIAL                                        */
/*      1 = hpesa_AVERAGE_REPEAT                                             */
/*                                                                           */
/*    ViPInt16 GSM_TXPAverageTypeQuery                                       */
/*                                                                           */
/*      Returns the current average type.                                    */
/*                                                                           */
/*      0 = hpesa_AVERAGE_LPOWER                                             */
/*      1 = hpesa_AVERAGE_POWER                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TXPGetAveraging
                       (ViSession instrumentHandle,
                        ViPInt32 GSM_TXPAverageCountQuery,
                        ViPBoolean GSM_TXPAverageStateQuery,
                        ViPInt16 GSM_TXPAverageModeQuery,
                        ViPInt16 GSM_TXPAverageTypeQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TXPSetTriggerSource                                            */
/*                                                                           */
/*      This method selects the trigger source for GSM transmit power        */
/*      measurements.                                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 GSM_TXPTriggerSource                                           */
/*                                                                           */
/*      Selects the trigger source used to control the data acquisitions.    */
/*                                                                           */
/*      hpesa_TXP_TRIG_IMMEDIATE - the next data acquisition is immediately  */
/*      taken (also called free run).                                        */
/*      hpesa_TXP_TRIG_EXTERNAL - rear panel external trigger input.         */
/*      hpesa_TXP_TRIG_RFBURST - wideband RF burst envelope trigger that has */
/*      automatic level control for periodic burst signals.                  */
/*      hpesa_TXP_TRIG_FRAME - uses the frame timer, which has been          */
/*      synchronized to the selected burst sync.                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TXPSetTriggerSource
                       (ViSession instrumentHandle,
                        ViInt16 GSM_TXPTriggerSource);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TXPGetTriggerSource                                            */
/*                                                                           */
/*      This method retrieves the current trigger source for GSM tranmit     */
/*      power measurements.                                                  */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 GSM_TXPTriggerSourceQuery                                     */
/*                                                                           */
/*      Returns the current trigger source.                                  */
/*                                                                           */
/*      0 = hpesa_TXP_TRIG_IMMEDIATE                                         */
/*      1 = hpesa_TXP_TRIG_RFBURST                                           */
/*      2 = hpesa_TXP_TRIG_EXTERNAL                                          */
/*      3 = hpesa_TXP_TRIG_FRAME                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TXPGetTriggerSource
                       (ViSession instrumentHandle,
                        ViPInt16 GSM_TXPTriggerSourceQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TXPSetThreshold                                                */
/*                                                                           */
/*      This method sets the threshold type and level for GSM tranmit power  */
/*      measurements.                                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 GSM_TXPThresholdType                                           */
/*                                                                           */
/*      Selects auto or manual control of the threshold level.               */
/*                                                                           */
/*      hpesa_TXP_THRESHOLD_ABSOLUTE - threshold value is set to an          */
/*      absolute power level                                                 */
/*      hpesa_TXP_THRESHOLD_RELATIVE - threshold value is set relative       */
/*      to the reference                                                     */
/*                                                                           */
/*    ViReal64 GSM_TXPThresholdLevel                                         */
/*                                                                           */
/*      Set the amplitude threshold level. Only the data above the threshold */
/*      level is kept and used to compute the average transmit carrier       */
/*      power.                                                               */
/*                                                                           */
/*      Units are in dBm for absolute mode and dB for relative mode.         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TXPSetThreshold
                       (ViSession instrumentHandle,
                        ViInt16 GSM_TXPThresholdType,
                        ViReal64 GSM_TXPThresholdLevel);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TXPGetThreshold                                                */
/*                                                                           */
/*      This method retrieves the current threshold settings for GSM         */
/*      transmit power measurements.                                         */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 GSM_TXPThresholdTypeQuery                                     */
/*                                                                           */
/*      Returns the current threshold type.                                  */
/*                                                                           */
/*      0 = hpesa_TXP_THRESHOLD_ABSOLUTE                                     */
/*      1 = hpesa_TXP_THRESHOLD_RELATIVE                                     */
/*                                                                           */
/*    ViPReal64 GSM_TXPThresholdLevelQuery                                   */
/*                                                                           */
/*      Returns the current threshold level.  Units are determined by        */
/*      threshold type:  Relative - dB; Absolute - dBm.                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TXPGetThreshold
                       (ViSession instrumentHandle,
                        ViPInt16 GSM_TXPThresholdTypeQuery,
                        ViPReal64 GSM_TXPThresholdLevelQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TXPSetTimeSlots                                                */
/*                                                                           */
/*      This method sets the number of timeslots to show on the screen for a */
/*      GSM transmit power measurement.                                      */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 GSM_TXPTimeSlots                                               */
/*                                                                           */
/*      Sets the number of timeslots to show on screen.  Range is from 1 to  */
/*      8.                                                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TXPSetTimeSlots
                       (ViSession instrumentHandle,
                        ViInt32 GSM_TXPTimeSlots);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TXPGetTimeSlots                                                */
/*                                                                           */
/*      This method retrieves the current number of timeslots being used for */
/*      GSM transmit power measurements.                                     */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt32 GSM_TXPTimeSlotsQuery                                         */
/*                                                                           */
/*      Returns the current number of timeslots.                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TXPGetTimeSlots
                       (ViSession instrumentHandle,
                        ViPInt32 GSM_TXPTimeSlotsQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TXPSetBurstSync                                                */
/*                                                                           */
/*      This method sets the method of synchronizing the measurement to the  */
/*      GSM bursts.                                                          */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 GSM_TXPBurstSyncSource                                         */
/*                                                                           */
/*      Select the method of synchronizing the measurement to the GSM        */
/*      bursts.                                                              */
/*                                                                           */
/*      hpesa_BURST_SYNC_TSEQ - the training sequence burst sync performs a  */
/*      demodulation of the burst and determines the start and stop of the   */
/*      useful part of the burst based on the midamble training sequence.    */
/*                                                                           */
/*      hpesa_BURST_SYNC_RFAMP - the RF amplitude burst synch approximates   */
/*      the start and stop of the useful part of the burst without digital   */
/*      demodulation of the burst.                                           */
/*                                                                           */
/*      hpesa_BURST_SYNC_NONE - performs no burst synchronization.           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TXPSetBurstSync
                       (ViSession instrumentHandle,
                        ViInt16 GSM_TXPBurstSyncSource);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TXPGetBurstSync                                                */
/*                                                                           */
/*      This method retrieves the current burst sync source for GSM transmit */
/*      power measurements.                                                  */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 GSM_TXPBurstSyncSourceQuery                                   */
/*                                                                           */
/*      Returns the current burst sync source.                               */
/*                                                                           */
/*      0 = hpesa_BURST_SYNC_TSEQ                                            */
/*      1 = hpesa_BURST_SYNC_RFAMP                                           */
/*      2 = hpesa_BURST_SYNC_NONE                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TXPGetBurstSync
                       (ViSession instrumentHandle,
                        ViPInt16 GSM_TXPBurstSyncSourceQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTConfigure                                                   */
/*                                                                           */
/*      This method sets up the instrument for power vs. time (PVT)          */
/*      measurements using the factory default instrument settings and stops */
/*      the current measurement. It does not initiate the taking of          */
/*      measurement data.                                                    */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTConfigure
                       (ViSession instrumentHandle);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTMeasureTrace                                                */
/*                                                                           */
/*      This method measures the average power during the “useful part” of   */
/*      the burst comparing the power ramp to required timing mask.          */
/*                                                                           */
/*      User can select one of three types of measurement modes: Measure,    */
/*      Read, or Fetch.                                                      */
/*                                                                           */
/*      This method returns the PVT measurement results specified by the     */
/*      "GSM_PVTResultsType" parameter as a real array.                      */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 GSM_PVTMeasureTraceTimeout                                    */
/*                                                                           */
/*      This value temporarily changes the VISA timeout setting for the      */
/*      duration of the instrument measurement. The user needs to set this   */
/*      value to an appropriate time that will be longer then the expected   */
/*      measurement time. Units are in seconds.                              */
/*                                                                           */
/*    ViInt16 GSM_PVTMeasureTraceType                                        */
/*                                                                           */
/*      Selects measurement type. Note that for hpesa_FETCH type the         */
/*      instrument should already be in this measurement mode else the       */
/*      function will time out. User can select this measurement mode by     */
/*      first using the "MEASURE" or "READ" types or call the                */
/*      hpesa_GSM_PVTConfigure function.                                     */
/*                                                                           */
/*      hpesa_MEASURE - Stops current measurement, changes to this selected  */
/*      measurement, sets up instrument using factory defaults, makes        */
/*      measurement, returns measurement results.                            */
/*                                                                           */
/*      hpesa_READ - Stops current measurement,  changes to this selected    */
/*      measurement, uses instrument settings from last measurement of this  */
/*      type, makes measurement, returns measurements results.               */
/*                                                                           */
/*      hpesa_FETCH - Returns measurements results only. No new measurement  */
/*      is initiated.                                                        */
/*                                                                           */
/*    ViInt16 GSM_PVTResultsType                                             */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_0: Not supported by instrument.                   */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_1: Returns the following comma-separated scalar   */
/*      results:                                                             */
/*      1. Sample time is a floating point number that represents the time   */
/*      between samples when using the trace queries (n=0, 2, et cetera).    */
/*      2. Power of single burst is the mean power (in dBm) across the       */
/*      useful part of the selected burst in the most recently acquired      */
/*      data, or in the last data acquired at the end of a set of averages.  */
/*      If averaging is on, the power is for the last burst.                 */
/*      3. Power averaged is the power (in dBm) of N averaged bursts, if     */
/*      averaging is on. The power is averaged across the useful part of the */
/*      burst. Average m is a single burst from the acquired trace. If there */
/*      are multiple bursts in the acquired trace, only one burst is used    */
/*      for average m. This means that N traces are acquired to make the     */
/*      complete average. If averaging is off, the value of power averaged   */
/*      is the same as the power single burst value.                         */
/*      4. Number of samples is the number of data points in the captured    */
/*      signal. This number is useful when performing a query on the signal  */
/*      (that is when n=0, 2, et cetera).                                    */
/*      5. Start point of the useful part of the burst is the index of the   */
/*      data point at the start of the useful part of the burst.             */
/*      6. Stop point of the useful part of the burst is the index of the    */
/*      data point at the end of the useful part of the burst.               */
/*      7. Index of the data point where T 0 occurred.                       */
/*      8. Burst width of the useful part of the burst is the width of the   */
/*      burst measured at -3dB below the mean power in the useful part of    */
/*      the burst.                                                           */
/*      9. Maximum value is the maximum value of the most recently acquired  */
/*      data (in dBm).                                                       */
/*      10.Minimum value is the minimum value of the most recently acquired  */
/*      data (in dBm).                                                       */
/*      11.Burst search threshold is the value (in dBm) of the threshold     */
/*      where a valid burst is identified, after the data has been acquired. */
/*      12.IQ point delta is the number of data points offset that are       */
/*      internally applied to the useful data in traces n=2,3,4. You must    */
/*      apply this                                                           */
/*      correction value to find the actual location of the Start, Stop, or  */
/*      T 0 values.                                                          */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_2 - Returns the entire captured RF envelope (data */
/*      array). It is represented as log-magnitude versus time. This array   */
/*      contains 401 points of data.                                         */
/*                                                                           */
/*    ViAReal64 GSM_PVTDataResults                                           */
/*                                                                           */
/*      Trace data returned by measurement, formatted according to           */
/*      GSM_PVTResultsType.                                                  */
/*                                                                           */
/*    ViInt32 GSM_PVTDataResultsMaxSize                                      */
/*                                                                           */
/*      Specifies the maximum size of GSM_PVTDataResults array.              */
/*                                                                           */
/*    ViPInt32 GSM_PVTDataResultsSize                                        */
/*                                                                           */
/*      Returns the actual size of GSM_TXPDataResults array.                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTMeasureTrace
                       (ViSession instrumentHandle,
                        ViReal64 GSM_PVTMeasureTraceTimeout,
                        ViInt16 GSM_PVTMeasureTraceType,
                        ViInt16 GSM_PVTResultsType,
                        ViAReal64 GSM_PVTDataResults,
                        ViInt32 GSM_PVTDataResultsMaxSize,
                        ViPInt32 GSM_PVTDataResultsSize);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTSetAveraging                                                */
/*                                                                           */
/*      This method sets the averaging for GSM power vs. time measurements.  */
/*      User can control the number of averages, turn off or on averaging,   */
/*      set the terminal control mode for averaging, and set the average     */
/*      type.                                                                */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 GSM_PVTAverageCount                                            */
/*                                                                           */
/*      Set the number of bursts that will be averaged. After the specified  */
/*      number of bursts (average counts), the averaging mode (terminal      */
/*      control) setting determines the averaging action.                    */
/*                                                                           */
/*    ViBoolean GSM_PVTAverageState                                          */
/*                                                                           */
/*      Sets the state of averaging, either on or off.                       */
/*                                                                           */
/*      VI_TRUE = Averaging on.                                              */
/*      VI_FALSE = Averaging off.                                            */
/*                                                                           */
/*    ViInt16 GSM_PVTAverageMode                                             */
/*                                                                           */
/*      Select the type of termination control used for the averaging        */
/*      function.  This determines the averaging action after the specified  */
/*      number of frames (average count) is reached.                         */
/*                                                                           */
/*      hpesa_AVERAGE_EXPONENTIAL - Each successive data acquisition after   */
/*      the average count is reached, is exponentially weighted and combined */
/*      with the existing average.                                           */
/*                                                                           */
/*      hpesa_AVERAGE_REPEAT - After reaching the average count, the         */
/*      averaging is reset and a new average is started.                     */
/*                                                                           */
/*    ViInt16 GSM_PVTAverageType                                             */
/*                                                                           */
/*      Select the type of averaging to be performed.                        */
/*                                                                           */
/*      hpesa_AVERAGE_LPOWER - logarithmically averages the power of the     */
/*      video data.                                                          */
/*      hpesa_AVERAGE_POWER  - averages the linear power of successive       */
/*      measurements.                                                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTSetAveraging
                       (ViSession instrumentHandle,
                        ViInt32 GSM_PVTAverageCount,
                        ViBoolean GSM_PVTAverageState,
                        ViInt16 GSM_PVTAverageMode,
                        ViInt16 GSM_PVTAverageType);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTGetAveraging                                                */
/*                                                                           */
/*      This method retrieves the average settings for GSM power vs. time    */
/*      measurements.                                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt32 GSM_PVTAverageCountQuery                                      */
/*                                                                           */
/*      Returns the current average count.                                   */
/*                                                                           */
/*    ViPBoolean GSM_PVTAverageStateQuery                                    */
/*                                                                           */
/*      Returns the current average state.                                   */
/*                                                                           */
/*      0 = VI_FALSE                                                         */
/*      1 = VI_TRUE                                                          */
/*                                                                           */
/*    ViPInt16 GSM_PVTAverageModeQuery                                       */
/*                                                                           */
/*      Returns the current average mode.                                    */
/*                                                                           */
/*      0 = hpesa_AVERAGE_EXPONENTIAL                                        */
/*      1 = hpesa_AVERAGE_REPEAT                                             */
/*                                                                           */
/*    ViPInt16 GSM_TXPAverageTypeQuery                                       */
/*                                                                           */
/*      Returns the current average type.                                    */
/*                                                                           */
/*      0 = hpesa_AVERAGE_LPOWER                                             */
/*      1 = hpesa_AVERAGE_POWER                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTGetAveraging
                       (ViSession instrumentHandle,
                        ViPInt32 GSM_PVTAverageCountQuery,
                        ViPBoolean GSM_PVTAverageStateQuery,
                        ViPInt16 GSM_PVTAverageModeQuery,
                        ViPInt16 GSM_TXPAverageTypeQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTSetTriggerSource                                            */
/*                                                                           */
/*      This method sets the trigger source for GSM power vs. time           */
/*      measurements.                                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 GSM_PVTTrigSource                                              */
/*                                                                           */
/*      Select the trigger source used to control the data acquisitions.     */
/*                                                                           */
/*      hpesa_PVT_TRIG_EXTERNAL - rear panel external trigger input          */
/*                                                                           */
/*      hpesa_PVT_TRIG_RFBURST - wideband RF burst envelope trigger that has */
/*      automatic level control for periodic burst signals.                  */
/*                                                                           */
/*      hpesa_PVT_TRIG_FRAME - uses the frame timer, which has been          */
/*      synchronized to the selected burst sync.                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTSetTriggerSource
                       (ViSession instrumentHandle,
                        ViInt16 GSM_PVTTrigSource);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTGetTriggerSource                                            */
/*                                                                           */
/*      This method returns the current trigger source for GSM power vs.     */
/*      time measurements.                                                   */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 GSM_PVTTrigSourceQuery                                        */
/*                                                                           */
/*      Returns the current trigger source.                                  */
/*                                                                           */
/*      0 = hpesa_PVT_TRIG_EXTERNAL                                          */
/*      1 = hpesa_PVT_TRIG_RFBURST                                           */
/*      2 = hpesa_PVT_TRIG_FRAME                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTGetTriggerSource
                       (ViSession instrumentHandle,
                        ViPInt16 GSM_PVTTrigSourceQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTSetBurstSync                                                */
/*                                                                           */
/*      This method sets the method of synchronizing the power vs. time      */
/*      measurement to the GSM bursts.                                       */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 GSM_PVTBurstSyncSource                                         */
/*                                                                           */
/*      Select the method of synchronizing the measurement to the GSM        */
/*      bursts.                                                              */
/*                                                                           */
/*      hpesa_BURST_SYNC_TSEQ - the training sequence burst sync performs a  */
/*      demodulation of the burst and determines the start and stop of the   */
/*      useful part of the burst based on the midamble training sequence.    */
/*                                                                           */
/*      hpesa_BURST_SYNC_RFAMP - performs burst synchronization based on the */
/*      rising and falling edges of the burst.                               */
/*                                                                           */
/*      hpesa_BURST_SYNC_NONE - performs no burst synchronization.           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTSetBurstSync
                       (ViSession instrumentHandle,
                        ViInt16 GSM_PVTBurstSyncSource);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTGetBurstSync                                                */
/*                                                                           */
/*      This method retrieves the current burst sync source for GSM power    */
/*      vs. time measurements.                                               */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 GSM_PVTBurstSyncSourceQuery                                   */
/*                                                                           */
/*      Returns the current burst sync source.                               */
/*                                                                           */
/*      0 = hpesa_BURST_SYNC_TSEQ                                            */
/*      1 = hpesa_BURST_SYNC_RFAMP                                           */
/*      2 = hpesa_BURST_SYNC_NONE                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTGetBurstSync
                       (ViSession instrumentHandle,
                        ViPInt16 GSM_PVTBurstSyncSourceQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTSetTimeSlots                                                */
/*                                                                           */
/*      This method sets the number of timeslots to show on the screen for   */
/*      GSM power vs. time measurements.                                     */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 GSM_PVTTimeSlots                                               */
/*                                                                           */
/*      Sets the number of timeslots.  Range is 1 to 8.                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTSetTimeSlots
                       (ViSession instrumentHandle,
                        ViInt32 GSM_PVTTimeSlots);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTGetTimeSlots                                                */
/*                                                                           */
/*      This method retrieves the current number of timeslots for GSM power  */
/*      vs. time measurements.                                               */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt32 GSM_PVTTimeSlotsQuery                                         */
/*                                                                           */
/*      Returns the current number of timeslots.                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTGetTimeSlots
                       (ViSession instrumentHandle,
                        ViPInt32 GSM_PVTTimeSlotsQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTLwrMskSetAbsLvl                                             */
/*                                                                           */
/*      This method allows sets the lower mask absolute amplitude levels for */
/*      GSM power vs. time measurements.  The user enters the absolute power */
/*      level for any of the mask line segments that require absolute limits */
/*      in addition to their relative limits. The defined relative mask      */
/*      values are normally used as the limits for testing. If the power of  */
/*      the reference level is decreased, all of these relative mask power   */
/*      levels will decrease by the same amount until they reach a defined   */
/*      minimum absolute power. Then that absolute power will be used as the */
/*      test limit.                                                          */
/*                                                                           */
/*      Any portion of the signal that has no limit line segment defined for */
/*      it, will default its to a very low limit (-200 dBm). Because of      */
/*      this, all data in that undefined area will pass the test.            */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 GSM_PVTLwrMskListSize                                          */
/*                                                                           */
/*      This size of the                                                     */
/*                                                                           */
/*    ViAReal64 GSM_PVTLwrMskAbsList                                         */
/*                                                                           */
/*      An array containing the  absolute power levels for the mask line     */
/*      segments.                                                            */
/*                                                                           */
/*      Note that only the amount of values equal to the number of mask      */
/*      points will be accepted.  Set the number of mask points via          */
/*      hpesa_GSM_PVTLwrMskSetTimePoints() and verify the number of mask     */
/*      points via hpesa_GSM_PVTLwrMskGetPoints.                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTLwrMskSetAbsLvl
                       (ViSession instrumentHandle,
                        ViInt32 GSM_PVTLwrMskListSize,
                        ViAReal64 GSM_PVTLwrMskAbsList);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTLwrMskGetAbsLvl                                             */
/*                                                                           */
/*      This method retrieves the current absolute power levels for the mask */
/*      line segments.                                                       */
/*                                                                           */
/*      Note that that the number of values returned is equal to the number  */
/*      of mask points will be accepted.  Set the number of mask points via  */
/*      hpesa_GSM_PVTLwrMskSetTimePoints() and verify the number of mask     */
/*      points via hpesa_GSM_PVTLwrMskGetPoints.                             */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 GSM_PVTLwrMskAbsListSzQ                                        */
/*                                                                           */
/*      The size of the array PVTLwrMskAbsListQuery.                         */
/*                                                                           */
/*    ViAReal64 GSM_PVTLwrMskAbsListQuery                                    */
/*                                                                           */
/*      Returns the current absolute power levels for the mask line segments.*/
/*                                                                           */
/*    ViPInt32 GSM_PVTLwrMskAbsListActSz                                     */
/*                                                                           */
/*      Returns the actual number of elements returned in                    */
/*      GSM_PVTLwrMskAbsListQuery.                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTLwrMskGetAbsLvl
                       (ViSession instrumentHandle,
                        ViInt32 GSM_PVTLwrMskAbsListSzQ,
                        ViAReal64 GSM_PVTLwrMskAbsListQuery,
                        ViPInt32 GSM_PVTLwrMskAbsListActSz);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTLwrMskGetPoints                                             */
/*                                                                           */
/*      This method retrieves the number of elements in the lower mask for   */
/*      GSM power vs. time measurements. This value is determined by the     */
/*      number of time points entered using hpesa_GSM_PVTLwrMskSetTimePoints.*/
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt32 GSM_PVTLwrMskListSzQuery                                      */
/*                                                                           */
/*      Returns the current number of elements in the lower mask.            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTLwrMskGetPoints
                       (ViSession instrumentHandle,
                        ViPInt32 GSM_PVTLwrMskListSzQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTLwrMskSetRelLvl                                             */
/*                                                                           */
/*      This method allows the user to enter the relative power level for    */
/*      each of the horizontal line segments in the lower limit mask for GSM */
/*      power vs. time measurements. There should be a power level for each  */
/*      time point entered using the hpesa_GSM_PVTLwrMskSetTimePoints        */
/*      method. These power levels are all relative to the defined Reference */
/*      Power Level (the average power in the useful part of the data). When */
/*      an upper and                                                         */
/*      lower limit mask have been defined, the Reference Power Level is the */
/*      mid-point between these two limits at time t0.                       */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 GSM_PVTLwrMskRelListSz                                         */
/*                                                                           */
/*      The size of the array GSM_PVTLwrMskRelList.                          */
/*                                                                           */
/*    ViAReal64 GSM_PVTLwrMskRelList                                         */
/*                                                                           */
/*      The array containing selects the relative power level for each of    */
/*      the horizontal line segments in the lower limit mask.                */
/*                                                                           */
/*      Note that only the amount of values equal to the number of mask      */
/*      points will be accepted.  Set the number of mask points via          */
/*      hpesa_GSM_PVTLwrMskSetTimePoints() and verify the number of mask     */
/*      points via hpesa_GSM_PVTLwrMskGetPoints.                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTLwrMskSetRelLvl
                       (ViSession instrumentHandle,
                        ViInt32 GSM_PVTLwrMskRelListSz,
                        ViAReal64 GSM_PVTLwrMskRelList);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTLwrMskGetRelLvl                                             */
/*                                                                           */
/*      This method retrieves the current set of relative power levels for   */
/*      the horizontal line segments in the lower limit mask for GSM power   */
/*      vs. time measurements.                                               */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 GSM_PVTLwrMskRelListSzQ                                        */
/*                                                                           */
/*      The size of the array GSM_PVTLwrMskRelListQuery.                     */
/*                                                                           */
/*    ViAReal64 GSM_PVTLwrMskRelListQuery                                    */
/*                                                                           */
/*      Returns an array containing relative power level for each of the     */
/*      horizontal line segments in the lower limit mask.                    */
/*                                                                           */
/*    ViPInt32 GSM_PVTLwrMskRelListActSz                                     */
/*                                                                           */
/*      Returns the actual number of elements returned in                    */
/*      GSM_PVTLwrMskRelListQuery.                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTLwrMskGetRelLvl
                       (ViSession instrumentHandle,
                        ViInt32 GSM_PVTLwrMskRelListSzQ,
                        ViAReal64 GSM_PVTLwrMskRelListQuery,
                        ViPInt32 GSM_PVTLwrMskRelListActSz);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTLwrMskSetTimePoints                                         */
/*                                                                           */
/*      This method allows the user to enter the ending points for the       */
/*      horizontal line segments that define the lower limit mask for GSM    */
/*      power vs. time measurements.  All the line segments begin at the     */
/*      time = t 0 reference point at the center of the useful data (usually */
/*      the center of the burst).  For example, all the mask line segments   */
/*      to the right of t 0 will have positive time values that get          */
/*      successively larger, while those to the left get successively more   */
/*      negative.                                                            */
/*                                                                           */
/*      It is recommended that you select a large time value for your first  */
/*      and last mask points (e.g. -1 and +1 second). This guarantees that   */
/*      you’ve defined a limit for all the measured data.                    */
/*                                                                           */
/*      The range is -1 second to +1 second, referenced to t0 at the center  */
/*      of the useful data (burst center).  There are 1 to 25 time points in */
/*      a mask.                                                              */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 GSM_PVTLwrMskTimeListSz                                        */
/*                                                                           */
/*      The size of the GSM_PVTLwrMskTimeList array.                         */
/*                                                                           */
/*    ViAReal64 GSM_PVTLwrMskTimeList                                        */
/*                                                                           */
/*      The array used to set the ending points for the horizontal line      */
/*      segments.                                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTLwrMskSetTimePoints
                       (ViSession instrumentHandle,
                        ViInt32 GSM_PVTLwrMskTimeListSz,
                        ViAReal64 GSM_PVTLwrMskTimeList);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTLwrMskGetTimePoints                                         */
/*                                                                           */
/*      This method retrieves the current ending points for the horizontal   */
/*      line segments that define the lower limit mask for GSM power vs.     */
/*      time measurements.                                                   */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 GSM_PVTLwrMskTimeListSzQ                                       */
/*                                                                           */
/*      The size of the GSM_PVTLwrMskTimeListQuery array.                    */
/*                                                                           */
/*    ViAReal64 GSM_PVTLwrMskTimeListQuery                                   */
/*                                                                           */
/*      Returns the current ending points for the horizontal line segments.  */
/*                                                                           */
/*    ViPInt32 GSM_PVTLwrMskTimeLstActSz                                     */
/*                                                                           */
/*      Returns the actual number of elements returned in                    */
/*      GSM_PVTLwrMskRelListQuery.                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTLwrMskGetTimePoints
                       (ViSession instrumentHandle,
                        ViInt32 GSM_PVTLwrMskTimeListSzQ,
                        ViAReal64 GSM_PVTLwrMskTimeListQuery,
                        ViPInt32 GSM_PVTLwrMskTimeLstActSz);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTMaskSetCustom                                               */
/*                                                                           */
/*      This method selects standard masks or user-defined custom masks to   */
/*      compare measured data against.                                       */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 GSM_PVTMaskCustom                                              */
/*                                                                           */
/*      Selects which mask to compare measured data against.                 */
/*                                                                           */
/*      hpesa_PVT_MASK_STANDARD - Uses standard masks.                       */
/*                                                                           */
/*      hpesa_PVT_MASK_CUSTOM - Uses user-defines custom masks.              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTMaskSetCustom
                       (ViSession instrumentHandle,
                        ViInt16 GSM_PVTMaskCustom);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTMaskGetCustom                                               */
/*                                                                           */
/*      This method retrieves the current mask type being used to compare    */
/*      measured data against.                                               */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 GSM_PVTMaskCustomQuery                                        */
/*                                                                           */
/*      Returns the current mask type.                                       */
/*                                                                           */
/*      0 = hpesa_PVT_MASK_STANDARD                                          */
/*      1 = hpesa_PVT_MASK_CUSTOM                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTMaskGetCustom
                       (ViSession instrumentHandle,
                        ViPInt16 GSM_PVTMaskCustomQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTUprMskSetAbsLvl                                             */
/*                                                                           */
/*      This method allows sets the upper mask absolute amplitude levels for */
/*      GSM power vs. time measurements.  The user enters the absolute power */
/*      level for any of the mask line segments that require absolute limits */
/*      in addition to their relative limits. The defined relative mask      */
/*      values are normally used as the limits for testing. If the power of  */
/*      the reference level is increased, all of these relative mask power   */
/*      levels will increase by the same amount until they reach a defined   */
/*      minimum absolute power. Then that absolute power will be used as the */
/*      test limit.                                                          */
/*                                                                           */
/*      Any portion of the signal that has no limit line segment defined for */
/*      it, will default its to a very low limit (-200 dBm). Because of      */
/*      this, all data in that undefined area will pass the test.            */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 GSM_PVTUprMskListSize                                          */
/*                                                                           */
/*      This size of the                                                     */
/*                                                                           */
/*    ViAReal64 GSM_PVTUprMskAbsList                                         */
/*                                                                           */
/*      An array containing the  absolute power levels for the mask line     */
/*      segments.                                                            */
/*                                                                           */
/*      Note that only the amount of values equal to the number of mask      */
/*      points will be accepted.  Set the number of mask points via          */
/*      hpesa_GSM_PVTUprMskSetTimePoints() and verify the number of mask     */
/*      points via hpesa_GSM_PVTUprMskGetPoints.                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTUprMskSetAbsLvl
                       (ViSession instrumentHandle,
                        ViInt32 GSM_PVTUprMskListSize,
                        ViAReal64 GSM_PVTUprMskAbsList);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTUprMskGetAbsLvl                                             */
/*                                                                           */
/*      This method retrieves the current absolute power levels for the mask */
/*      line segments.                                                       */
/*                                                                           */
/*      Note that that the number of values returned is equal to the number  */
/*      of mask points will be accepted.  Set the number of mask points via  */
/*      hpesa_GSM_PVTUprMskSetTimePoints() and verify the number of mask     */
/*      points via hpesa_GSM_PVTUprMskGetPoints.                             */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 GSM_PVTUprMskAbsListSzQ                                        */
/*                                                                           */
/*      The size of the array PVTUprMskAbsListQuery.                         */
/*                                                                           */
/*    ViAReal64 GSM_PVTUprMskAbsListQuery                                    */
/*                                                                           */
/*      Returns the current absolute power levels for the mask line segments.*/
/*                                                                           */
/*    ViPInt32 GSM_PVTUprMskAbsListActSz                                     */
/*                                                                           */
/*      Returns the actual number of elements returned in                    */
/*      GSM_PVTUprMskAbsListQuery.                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTUprMskGetAbsLvl
                       (ViSession instrumentHandle,
                        ViInt32 GSM_PVTUprMskAbsListSzQ,
                        ViAReal64 GSM_PVTUprMskAbsListQuery,
                        ViPInt32 GSM_PVTUprMskAbsListActSz);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTUprMskGetPoints                                             */
/*                                                                           */
/*      This method retrieves the number of elements in the upper mask for   */
/*      GSM power vs. time measurements. This value is determined by the     */
/*      number of time points entered using hpesa_GSM_PVTUprMskSetTimePoints.*/
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt32 GSM_PVTUprMskListSzQuery                                      */
/*                                                                           */
/*      Returns the current number of elements in the upper mask.            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTUprMskGetPoints
                       (ViSession instrumentHandle,
                        ViPInt32 GSM_PVTUprMskListSzQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTUprMskSetRelLvl                                             */
/*                                                                           */
/*      This method allows the user to enter the relative power level for    */
/*      each of the horizontal line segments in the upper limit mask for GSM */
/*      power vs. time measurements. There should be a power level for each  */
/*      time point entered using the hpesa_GSM_PVTUprMskSetTimePoints        */
/*      method. These power levels are all relative to the defined Reference */
/*      Power Level (the average power in the useful part of the data). When */
/*      an upper and lower limit mask have been defined, the Reference Power */
/*      Level is the mid-point between these two limits at time t0.          */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 GSM_PVTUprMskRelListSz                                         */
/*                                                                           */
/*      The size of the array GSM_PVTUprMskRelList.                          */
/*                                                                           */
/*    ViAReal64 GSM_PVTUprMskRelList                                         */
/*                                                                           */
/*      The array containing selects the relative power level for each of    */
/*      the horizontal line segments in the upper limit mask.                */
/*                                                                           */
/*      Note that only the amount of values equal to the number of mask      */
/*      points will be accepted.  Set the number of mask points via          */
/*      hpesa_GSM_PVTUprMskSetTimePoints() and verify the number of mask     */
/*      points via hpesa_GSM_PVTUprMskGetPoints.                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTUprMskSetRelLvl
                       (ViSession instrumentHandle,
                        ViInt32 GSM_PVTUprMskRelListSz,
                        ViAReal64 GSM_PVTUprMskRelList);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTUprMskGetRelLvl                                             */
/*                                                                           */
/*      This method retrieves the current set of relative power levels for   */
/*      the horizontal line segments in the upper limit mask for GSM power   */
/*      vs. time measurements.                                               */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 GSM_PVTUprMskRelListSzQ                                        */
/*                                                                           */
/*      The size of the array GSM_PVTUprMskRelListQuery.                     */
/*                                                                           */
/*    ViAReal64 GSM_PVTUprMskRelListQuery                                    */
/*                                                                           */
/*      Returns an array containing relative power level for each of the     */
/*      horizontal line segments in the upper limit mask.                    */
/*                                                                           */
/*    ViPInt32 GSM_PVTUprMskRelListActSz                                     */
/*                                                                           */
/*      Returns the actual number of elements returned in                    */
/*      GSM_PVTUprMskRelListQuery.                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTUprMskGetRelLvl
                       (ViSession instrumentHandle,
                        ViInt32 GSM_PVTUprMskRelListSzQ,
                        ViAReal64 GSM_PVTUprMskRelListQuery,
                        ViPInt32 GSM_PVTUprMskRelListActSz);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTUprMskSetTimePoints                                         */
/*                                                                           */
/*      This method allows the user to enter the ending points for the       */
/*      horizontal line segments that define the upper limit mask for GSM    */
/*      power vs. time measurements.  All the line segments begin at the     */
/*      time = t 0 reference point at the center of the useful data (usually */
/*      the center of the burst).  For example, all the mask line segments   */
/*      to the right of t 0 will have positive time values that get          */
/*      successively larger, while those to the left get successively more   */
/*      negative.                                                            */
/*                                                                           */
/*      It is recommended that you select a large time value for your first  */
/*      and last mask points (e.g. -1 and +1 second). This guarantees that   */
/*      you’ve defined a limit for all the measured data.                    */
/*                                                                           */
/*      The range is -1 second to +1 second, referenced to t0 at the center  */
/*      of the useful data (burst center).  There are 1 to 25 time points in */
/*      a mask.                                                              */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 GSM_PVTUprMskTimeListSz                                        */
/*                                                                           */
/*      The size of the GSM_PVTUprMskTimeList array.                         */
/*                                                                           */
/*    ViAReal64 GSM_PVTUprMskTimeList                                        */
/*                                                                           */
/*      The array used to set the ending points for the horizontal line      */
/*      segments.                                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTUprMskSetTimePoints
                       (ViSession instrumentHandle,
                        ViInt32 GSM_PVTUprMskTimeListSz,
                        ViAReal64 GSM_PVTUprMskTimeList);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PVTUprMskGetTimePoints                                         */
/*                                                                           */
/*      This method retrieves the current ending points for the horizontal   */
/*      line segments that define the upper limit mask for GSM power vs.     */
/*      time measurements.                                                   */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 GSM_PVTUprMskTimeListSzQ                                       */
/*                                                                           */
/*      The size of the GSM_PVTUprMskTimeListQuery array.                    */
/*                                                                           */
/*    ViAReal64 GSM_PVTUprMskTimeListQuery                                   */
/*                                                                           */
/*      Returns the current ending points for the horizontal line segments.  */
/*                                                                           */
/*    ViPInt32 GSM_PVTUprMskTimeLstActSz                                     */
/*                                                                           */
/*      Returns the actual number of elements returned in                    */
/*      GSM_PVTUprMskRelListQuery.                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PVTUprMskGetTimePoints
                       (ViSession instrumentHandle,
                        ViInt32 GSM_PVTUprMskTimeListSzQ,
                        ViAReal64 GSM_PVTUprMskTimeListQuery,
                        ViPInt32 GSM_PVTUprMskTimeLstActSz);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PFERConfigure                                                  */
/*                                                                           */
/*      This method sets up the instrument for GSM Phase and Frequency Error */
/*      (PFER) measurements using the factory default instrument settings    */
/*      and stops the current measurement. It does not initiate the taking   */
/*      of measurement data.                                                 */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PFERConfigure
                       (ViSession instrumentHandle);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PFERMeasureTrace                                               */
/*                                                                           */
/*      This method measures the modulation quality of the transmitter by    */
/*      checking phase and frequency accuracy.                               */
/*                                                                           */
/*      User can select one of three types of measurement modes: Measure,    */
/*      Read, or Fetch.                                                      */
/*                                                                           */
/*                                                                           */
/*      This method returns the TXP measurement results specified by the     */
/*      "GSM_PVTResultsType" parameter as a real array.                      */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 GSM_PFERMeasureTraceTimeout                                   */
/*                                                                           */
/*      This value temporarily changes the VISA timeout setting for the      */
/*      duration of the instrument measurement. The user needs to set this   */
/*      value to an appropriate time that will be longer then the expected   */
/*      measurement time. Units are in seconds.                              */
/*                                                                           */
/*    ViInt16 GSM_PFERMeasureTraceType                                       */
/*                                                                           */
/*      Selects measurement type. Note that for hpesa_FETCH type the         */
/*      instrument should already be in this measurement mode else the       */
/*      function will time out. User can select this measurement mode by     */
/*      first using the "MEASURE" or "READ" types or call the                */
/*      hpesa_GSM_PFERConfigure function.                                    */
/*                                                                           */
/*      hpesa_MEASURE - Stops current measurement, changes to this selected  */
/*      measurement, sets up instrument using factory defaults, makes        */
/*      measurement, returns measurement results.                            */
/*                                                                           */
/*      hpesa_READ - Stops current measurement,  changes to this selected    */
/*      measurement, uses instrument settings from last measurement of this  */
/*      type, makes measurement, returns measurements results.               */
/*                                                                           */
/*      hpesa_FETCH - Returns measurements results only. No new measurement  */
/*      is initiated.                                                        */
/*                                                                           */
/*    ViInt16 GSM_PFERResultsType                                            */
/*                                                                           */
/*      Determines the format of the data returned in GSM_PFERDataResults.   */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_0:  Returns a series of floating point numbers    */
/*      (in volts) that represent each sample of the complete current time   */
/*      record raw I/Q vector trace length scalar value (giving the total    */
/*      number of I/Q vector sample pairs = n/2).                            */
/*                                                                           */
/*      The II/Q vector sample pair data is organized as:                    */
/*      I(0), Q(0),                                                          */
/*      I(1), Q(1),                                                          */
/*      -------------                                                        */
/*      I([n/2]-1), Q([n/2]-1)                                               */
/*                                                                           */
/*      The start of bit 0 (zero) of the useful part of the measured GSM     */
/*      burst within the vector sample pairs is located at I(x/2), Q(x/2)    */
/*      where x = raw I/Q vector trace index to burst.                       */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_1:  Returns the following 15 scalar values:       */
/*      1. RMS phase error is a floating point number (in degrees) of the    */
/*      rms phase error between the measured phase and the ideal phase. The  */
/*      calculation is based on symbol decision points and points halfway    */
/*      between symbol decision points (that is 2 points/symbol). If         */
/*      averaging is on, this is the average of the individual rms phase     */
/*      error measurements.                                                  */
/*      2. Peak phase error is a floating point number (in degrees) of the   */
/*      peak phase error of all the individual symbol decision points (prior */
/*      to the rms averaging process). If averaging is on, this is the       */
/*      average of the individual peak phase error measurements.             */
/*      3. Peak phase symbol is a floating point number (in symbols)         */
/*      representing the symbol number at which the peak phase error         */
/*      occurred. Averaging does not affect this calculation.                */
/*      4. Frequency error is a floating point number (in Hz) of the         */
/*      frequency error in the measured signal. This is the difference       */
/*      between the measured phase trajectory and the reference phase        */
/*      trajectory. If averaging is on, this is the average of the           */
/*      individual frequency error measurements.                             */
/*      5. I/Q origin offset is a floating point number (in dB) of the I and */
/*      Q error (magnitude squared) offset from the origin. If averaging is  */
/*      on, this is the average of the individual IQ Offset measurements.    */
/*      6. Trace phase sample is a floating point number (in units of bits)  */
/*      representing the time between samples. It is used in querying phase  */
/*      error vector traces.                                                 */
/*      7. Trace bit 0 decision offset is an integer number in units of      */
/*      sample pairs for the sample points in an I/Q vector trace that       */
/*      represents the bit 0 (zero) decision point. The sample pairs in the  */
/*      trace are numbered 0 to N.                                           */
/*      8. Trace sync start is an integer number in units of bits for the    */
/*      bit number, within the data bits trace, that represents the start of */
/*      the sync word.                                                       */
/*      9. Trace time sample is a floating point number (in seconds) of the  */
/*      time between samples. It is used in querying time domain traces. For */
/*      the n=0 trace, of acquired I/Q pairs, this is the time between       */
/*      pairs.                                                               */
/*      10.Phase error trace length is an integer number (in units of        */
/*      samples) representing the number of samples returned by the current  */
/*      phase error trace and phase error with frequency trace.              */
/*      11.RF envelope trace length is an integer number (in units of        */
/*      samples) representing the number of samples returned by the current  */
/*      RF envelope trace.                                                   */
/*      12.RF envelope trace index to burst is an integer number (in units   */
/*      of samples) representing the trace sample which represents the start */
/*      of bit 0 (zero) decision point of the useful part of the measured    */
/*      GSM burst.n=1 (cont)                                                 */
/*      13. I/Q vector trace length is an integer number (in units of        */
/*      samples) representing the number of samples returned by the current  */
/*      RF envelope trace (ie this number divided by two represents the      */
/*      number of sample pairs in the trace.)                                */
/*      14. Raw I/Q vector trace length is an integer number (in units of    */
/*      samples) representing the number of samples returned by the current  */
/*      RF envelope trace (ie this number divided by two represents the      */
/*      number of sample pairs in the trace).                                */
/*      15. Raw I/Q vector trace index to burst is an integer number (in     */
/*      units of samples) representing the trace sample which represents the */
/*      start of bit 0 (zero) decision point of the useful part of the       */
/*      measured GSM burst.                                                  */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_1: Returns a series of floating point numbers (in */
/*      degrees) that represent each sample of the current phase error trace */
/*      data over the useful part of the measured GSM burst. It contains n   */
/*      samples, where n = phase error trace length scalar value. The first  */
/*      sample represents the start of bit 0 (zero) of the useful part of    */
/*      the demodulated burst.                                               */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_3:  Returns a series of floating point numbers    */
/*      (in degrees) that represent each sample of the current phase error   */
/*      with frequency trace data over the useful part of the measured GSM   */
/*      burst. Phase error with frequency is the error vector between the    */
/*      measured phase (that has not had frequency                           */
/*      compensation) and the ideal reference phase. It contains n samples,  */
/*      where n = phase error trace length scalar value. The first sample    */
/*      represents the start of bit 0 (zero) of the useful part of the       */
/*      demodulated burst.                                                   */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_4: Returns a series of floating point numbers (in */
/*      dB relative to peak of signal) that represent each sample of the     */
/*      complete current time record RF envelope trace data. It contains n   */
/*      samples where n =RF envelope trace length scalar value.              */
/*                                                                           */
/*      The start of bit 0 (zero) so the useful part of the measured GSM     */
/*      burst within the sample time record is located at:                   */
/*                                                                           */
/*           I(x), Q(x)                                                      */
/*                                                                           */
/*      where x = RF envelope trace index to burst.                          */
/*                                                                           */
/*    ViAReal64 GSM_PFERDataResults                                          */
/*                                                                           */
/*      Trace data returned by measurement, formatted according to           */
/*      GSM_PFERResultsType.                                                 */
/*                                                                           */
/*    ViInt32 GSM_PFERDataResultsMaxSize                                     */
/*                                                                           */
/*      Specifies the maximum size of GSM_PFERDataResults array.             */
/*                                                                           */
/*    ViPInt32 GSM_PFERDataResultsSize                                       */
/*                                                                           */
/*      Returns the actual size of GSM_PFERDataResults array.                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PFERMeasureTrace
                       (ViSession instrumentHandle,
                        ViReal64 GSM_PFERMeasureTraceTimeout,
                        ViInt16 GSM_PFERMeasureTraceType,
                        ViInt16 GSM_PFERResultsType,
                        ViAReal64 GSM_PFERDataResults,
                        ViInt32 GSM_PFERDataResultsMaxSize,
                        ViPInt32 GSM_PFERDataResultsSize);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PFERSetAveraging                                               */
/*                                                                           */
/*      This method sets the averaging for GSM phase and frequence error     */
/*      measurements.  User can control the number of averages, turn off or  */
/*      on averaging, set the terminal control mode for averaging, and set   */
/*      the average type.                                                    */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 GSM_PFERAverageCount                                           */
/*                                                                           */
/*      Set the number of bursts that will be averaged. After the specified  */
/*      number of bursts (average counts), the averaging mode (terminal      */
/*      control) setting determines the averaging action.                    */
/*                                                                           */
/*    ViBoolean GSM_PFERAverageState                                         */
/*                                                                           */
/*      Sets the state of averaging, either on or off.                       */
/*                                                                           */
/*      VI_TRUE = Averaging on.                                              */
/*      VI_FALSE = Averaging off.                                            */
/*                                                                           */
/*    ViInt16 GSM_PFERAverageMode                                            */
/*                                                                           */
/*      Select the type of termination control used for the averaging        */
/*      function.  This determines the averaging action after the specified  */
/*      number of frames (average count) is reached.                         */
/*                                                                           */
/*      hpesa_AVERAGE_EXPONENTIAL - Each successive data acquisition after   */
/*      the average count is reached, is exponentially weighted and combined */
/*      with the existing average.                                           */
/*                                                                           */
/*      hpesa_AVERAGE_REPEAT - After reaching the average count, the         */
/*      averaging is reset and a new average is started.                     */
/*                                                                           */
/*    ViInt16 GSM_PFERAverageType                                            */
/*                                                                           */
/*      Selects the type of averaging.                                       */
/*                                                                           */
/*      hpesa_AVERAGE_MEAN - the scalar results are averaged.                */
/*      hpesa_AVERAGE_MAX - the maximum scalar results are retained.         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PFERSetAveraging
                       (ViSession instrumentHandle,
                        ViInt32 GSM_PFERAverageCount,
                        ViBoolean GSM_PFERAverageState,
                        ViInt16 GSM_PFERAverageMode,
                        ViInt16 GSM_PFERAverageType);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PFERGetAveraging                                               */
/*                                                                           */
/*      This method retrieves the average settings for GSM phase and         */
/*      frequency error measurements.                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt32 GSM_PFERAverageCountQuery                                     */
/*                                                                           */
/*      Returns the current average count.                                   */
/*                                                                           */
/*    ViPBoolean GSM_PFERAverageStateQuery                                   */
/*                                                                           */
/*      Returns the current average state.                                   */
/*                                                                           */
/*      0 = VI_FALSE                                                         */
/*      1 = VI_TRUE                                                          */
/*                                                                           */
/*    ViPInt16 GSM_PFERAverageModeQuery                                      */
/*                                                                           */
/*      Returns the current average mode.                                    */
/*                                                                           */
/*      0 = hpesa_AVERAGE_EXPONENTIAL                                        */
/*      1 = hpesa_AVERAGE_REPEAT                                             */
/*                                                                           */
/*    ViPInt16 GSM_PFERAverageTypeQuery                                      */
/*                                                                           */
/*      Selects the type of averaging.                                       */
/*                                                                           */
/*      0 = hpesa_AVERAGE_MEAN                                               */
/*      1 = hpesa_AVERAGE_MAX                                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PFERGetAveraging
                       (ViSession instrumentHandle,
                        ViPInt32 GSM_PFERAverageCountQuery,
                        ViPBoolean GSM_PFERAverageStateQuery,
                        ViPInt16 GSM_PFERAverageModeQuery,
                        ViPInt16 GSM_PFERAverageTypeQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PFERSetTriggerSource                                           */
/*                                                                           */
/*      This method selects the trigger source used to control the data      */
/*      acquisitions for GSM phase and frequence error measurements.         */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 GSM_PFERTrigSource                                             */
/*                                                                           */
/*      Sets the trigger source.                                             */
/*                                                                           */
/*      hpesa_PFER_TRIG_EXTERNAL - external trigger input. Uses rear panel.  */
/*                                                                           */
/*      hpesa_PFER_TRIG_FRAME - uses the internal frame timer, which has     */
/*      been synchronized to the selected burst sync.                        */
/*                                                                           */
/*      hpesa_PFER_TRIG_IMMEDIATE - the next data acquisition is immediately */
/*      taken, capturing the signal asynchronously (also called free run).   */
/*                                                                           */
/*      hpesa_PFER_TRIG_RFBURST - internal wideband RF burst envelope        */
/*      trigger that has automatic level control for periodic burst signals. */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PFERSetTriggerSource
                       (ViSession instrumentHandle,
                        ViInt16 GSM_PFERTrigSource);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PFERGetTriggerSource                                           */
/*                                                                           */
/*      This method retrieves the current trigger source for GSM phase and   */
/*      frequency error measurements.                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 GSM_PFERTrigSourceQuery                                       */
/*                                                                           */
/*      Returns the current trigger source.                                  */
/*                                                                           */
/*      0 = hpesa_PFER_TRIG_IMMEDIATE                                        */
/*      1 = hpesa_PFER_TRIG_EXTERNAL                                         */
/*      2 = hpesa_PFER_TRIG_RFBURST                                          */
/*      3 - hpesa_PFER_TRIG_FRAME                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PFERGetTriggerSource
                       (ViSession instrumentHandle,
                        ViPInt16 GSM_PFERTrigSourceQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PFERSetBurstSync                                               */
/*                                                                           */
/*      This method selects the method of synchronizing the phase and        */
/*      frequency error measurement to the GSM bursts.                       */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 GSM_PFERBurstSync                                              */
/*                                                                           */
/*      Sets the burst synchronization.                                      */
/*                                                                           */
/*      hpesa_BURST_SYNC_TSEQ - the training sequence burst sync performs a  */
/*      demodulation of the burst and determines the start and stop of the   */
/*      useful part of the burst based on the midamble training sequence.    */
/*                                                                           */
/*      hpesa_BURST_SYNC_RFAMP - the RF amplitude burst sync approximates    */
/*      the start and stop of the useful part of the burst without           */
/*      demodulation of the burst.                                           */
/*                                                                           */
/*      hpesa_BURST_SYNC_NONE - performs no burst synchronization.           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PFERSetBurstSync
                       (ViSession instrumentHandle,
                        ViInt16 GSM_PFERBurstSync);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_PFERGetBurstSync                                               */
/*                                                                           */
/*      This method retrieves the current method of synchronizing the phase  */
/*      and frequency error measurement to the GSM bursts.                   */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 GSM_PFERBurstSyncQuery                                        */
/*                                                                           */
/*      Returns the current burst sync source.                               */
/*                                                                           */
/*      0 = hpesa_BURST_SYNC_TSEQ                                            */
/*      1 = hpesa_BURST_SYNC_RFAMP                                           */
/*      2 = hpesa_BURST_SYNC_NONE                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_PFERGetBurstSync
                       (ViSession instrumentHandle,
                        ViPInt16 GSM_PFERBurstSyncQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_ORFSConfigure                                                  */
/*                                                                           */
/*      This method sets up the instrument for Output RF Spectrum (ORFS)     */
/*      measurements using the factory default instrument settings and stops */
/*      the current measurement. It does not initiate the taking of          */
/*      measurement data.                                                    */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_ORFSConfigure
                       (ViSession instrumentHandle);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_ORFSMeasureTrace                                               */
/*                                                                           */
/*      This measures adjacent channel power for GSM measurements. From 1 to */
/*      15 offsets can be measured at one time.                              */
/*                                                                           */
/*      User can select one of three types of measurement modes: Measure,    */
/*      Read, or Fetch.                                                      */
/*                                                                           */
/*                                                                           */
/*      This method returns the ORFS measurement results specified by the    */
/*      "GSM_ORFSResultsType" parameter as a real array.                     */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 GSM_ORFSMeasureTraceTimeout                                   */
/*                                                                           */
/*      This value temporarily changes the VISA timeout setting for the      */
/*      duration of the instrument measurement. The user needs to set this   */
/*      value to an appropriate time that will be longer then the expected   */
/*      measurement time. Units are in seconds.                              */
/*                                                                           */
/*    ViInt16 GSM_ORFSMeasureTraceType                                       */
/*                                                                           */
/*      Selects measurement type. Note that for hpesa_FETCH type the         */
/*      instrument should already be in this measurement mode else the       */
/*      function will time out. User can select this measurement mode by     */
/*      first using the "MEASURE" or "READ" types or call the                */
/*      hpesa_GSM_ORFSConfigure function.                                    */
/*                                                                           */
/*      hpesa_MEASURE - Stops current measurement, changes to this selected  */
/*      measurement, sets up instrument using factory defaults, makes        */
/*      measurement, returns measurement results.                            */
/*                                                                           */
/*      hpesa_READ - Stops current measurement,  changes to this selected    */
/*      measurement, uses instrument settings from last measurement of this  */
/*      type, makes measurement, returns measurements results.               */
/*                                                                           */
/*      hpesa_FETCH - Returns measurements results only. No new measurement  */
/*      is initiated.                                                        */
/*                                                                           */
/*    ViInt16 GSM_ORFSResultsType                                            */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_0 - Not supported by instrument.                  */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_1 (Multiple Offsets) - Returns a list of          */
/*      comma-separated values for the modulation spectrum at all the        */
/*      offsets (lower and upper.) This is followed by the switching         */
/*      transients results at all the offsets (lower and upper). Note that   */
/*      the carrier is considered offset zero (0) and is the first set of    */
/*      results sent. Four values are provided for each of the offsets       */
/*      (including the carrier), in this order:                              */
/*                                                                           */
/*      1. Negative offset(a) - power relative to carrier (dB)               */
/*      2. Negative offset(a) - absolute average power (dBm)                 */
/*      3. Positive offset(a) - power relative to carrier (dB)               */
/*      4. Positive offset(a) - absolute average power (dBm)                 */
/*                                                                           */
/*      Values for all possible offsets are sent. Zeros are sent for offsets */
/*      that have not been defined. The total number of values sent (120) =  */
/*      (4 results/offset) ´ (15 offsets) ´ (2 measurement types -           */
/*      modulation & switching)                                              */
/*                                                                           */
/*      Carrier - modulation measurement values                              */
/*      Offset 1 - modulation measurement values and so on                   */
/*      Offset 14 - modulation measurement values                            */
/*      Carrier - switching transients measurement values                    */
/*      Offset 16 - switching transients measurement values                  */
/*      Offset 29 - switching transients measurement values and              */
/*      so on                                                                */
/*                                                                           */
/*      This measurement defaults to modulation measurements and not         */
/*      switching measurements. If you want to return the switching          */
/*      measurement values, you must change that default condition and use   */
/*      FETCH or READ to return values, rather than MEASURE.                 */
/*                                                                           */
/*      NOTE: When using custom modulation and switching offsets the maximum */
/*      number of measured values returned is:                               */
/*      13 modulation offsets + 0 Hz carrier                                 */
/*      4 switching offsets + 0 Hz carrier                                   */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_1 (Single offset) - Returns 4 comma-separated     */
/*      results for the specified offset:                                    */
/*                                                                           */
/*      1. Modulation spectrum power, dBc                                    */
/*      2. Modulation spectrum power, dBm                                    */
/*      3. Switching transient power, dBc                                    */
/*      4. Switching transient power, dBm                                    */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_1 (Swept) - Returns l boolean value: 1 if limits  */
/*      passed, 0 if limits failed.                                          */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_2 (Single offset) -  Returns floating point       */
/*      numbers (in dBm) of the captured trace data. It contains 401 data    */
/*      points of the "spectrum due to modulation" signal. Single offset 3   */
/*      Returns floating point numbers (in dBm) of the captured trace data.  */
/*      It contains 401 points of the "spectrum due to switching transients" */
/*      signal.                                                              */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_2 (Swept) - Returns floating point numbers (in    */
/*      dBm) of the captured trace data. It contains 401 points of the       */
/*      "spectrum due to modulation" signal.                                 */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_3 (Single offset) - Returns floating point        */
/*      numbers (in dBm) of the captured trace data. It contains 401 points  */
/*      of the "spectrum due to switching transients" signal.                */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_3 (Swept) - Returns floating point numbers (in    */
/*      dBm) of the captured trace data. It contains 401 points of the       */
/*      "spectrum due to switching transients" signal.                       */
/*                                                                           */
/*    ViAReal64 GSM_ORFSDataResults                                          */
/*                                                                           */
/*      Trace data returned by measurement, formatted according to           */
/*      GSM_ORFSResultsType.                                                 */
/*                                                                           */
/*    ViInt32 GSM_ORFSDataResultsMaxSize                                     */
/*                                                                           */
/*      Specifies the maximum size of GSM_ORFSDataResults array.             */
/*                                                                           */
/*    ViPInt32 GSM_ORFSDataResultsSize                                       */
/*                                                                           */
/*      Returns the actual size of GSM_ORFSDataResults array.                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_ORFSMeasureTrace
                       (ViSession instrumentHandle,
                        ViReal64 GSM_ORFSMeasureTraceTimeout,
                        ViInt16 GSM_ORFSMeasureTraceType,
                        ViInt16 GSM_ORFSResultsType,
                        ViAReal64 GSM_ORFSDataResults,
                        ViInt32 GSM_ORFSDataResultsMaxSize,
                        ViPInt32 GSM_ORFSDataResultsSize);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_ORFSSetAveraging                                               */
/*                                                                           */
/*      This method sets the averaging for GSM power vs. time measurements.  */
/*      User can control the number of averages, turn off or on averaging,   */
/*      and set the terminal control mode for averaging.                     */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 GSM_ORFSAverageCount                                           */
/*                                                                           */
/*      Set the number of bursts that will be averaged. For the output RF    */
/*      spectrum due to switching transients, it is more accurate to         */
/*      consider this the number of frames that are measured. After the      */
/*      specified number of bursts (average counts), the averaging mode      */
/*      (terminal control) setting determines the averaging action.          */
/*                                                                           */
/*    ViBoolean GSM_ORFSAverageState                                         */
/*                                                                           */
/*      Sets the state of averaging, either on or off.                       */
/*                                                                           */
/*      VI_TRUE = Averaging on.                                              */
/*      VI_FALSE = Averaging off.                                            */
/*                                                                           */
/*    ViInt16 GSM_ORFSAverageMode                                            */
/*                                                                           */
/*      Select the type of termination control used for the averaging        */
/*      function.  This determines the averaging action after the specified  */
/*      number of frames (average count) is reached.                         */
/*                                                                           */
/*      hpesa_AVERAGE_EXPONENTIAL - Each successive data acquisition after   */
/*      the average count is reached, is exponentially weighted and combined */
/*      with the existing average.                                           */
/*                                                                           */
/*      hpesa_AVERAGE_REPEAT - After reaching the average count, the         */
/*      averaging is reset and a new average is started.                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_ORFSSetAveraging
                       (ViSession instrumentHandle,
                        ViInt32 GSM_ORFSAverageCount,
                        ViBoolean GSM_ORFSAverageState,
                        ViInt16 GSM_ORFSAverageMode);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_ORFSGetAveraging                                               */
/*                                                                           */
/*      This method retrieves the average settings for GSM output RF         */
/*      spectrum measurements.                                               */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt32 GSM_ORFSAverageCountQuery                                     */
/*                                                                           */
/*      Returns the current average count.                                   */
/*                                                                           */
/*    ViPBoolean GSM_ORFSAverageStateQuery                                   */
/*                                                                           */
/*      Returns the current average state.                                   */
/*                                                                           */
/*      0 = VI_FALSE                                                         */
/*      1 = VI_TRUE                                                          */
/*                                                                           */
/*    ViPInt16 GSM_ORFSAverageModeQuery                                      */
/*                                                                           */
/*      Returns the current average mode.                                    */
/*                                                                           */
/*      0 = hpesa_AVERAGE_EXPONENTIAL                                        */
/*      1 = hpesa_AVERAGE_REPEAT                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_ORFSGetAveraging
                       (ViSession instrumentHandle,
                        ViPInt32 GSM_ORFSAverageCountQuery,
                        ViPBoolean GSM_ORFSAverageStateQuery,
                        ViPInt16 GSM_ORFSAverageModeQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_ORFSSetMeasurementMode                                         */
/*                                                                           */
/*      This method selects the measurement method to be used with GSM       */
/*      output RF spectrum measurements.                                     */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 GSM_ORFSMeasMode                                               */
/*                                                                           */
/*      Selects the measurement method to be used.                           */
/*                                                                           */
/*      hpesa_ORFS_MEASURE_MULTIPLE - the measurement is done at all offsets */
/*      in the offset frequency list.                                        */
/*                                                                           */
/*      hpesa_ORFS_MEASURE_SINGLE - the measurement is done at only one      */
/*      offset as determined by the offset frequency setting. This allows    */
/*      detailed examination of the time-domain waveform at the specified    */
/*      offset frequency.                                                    */
/*                                                                           */
/*      hpesa_ORFS_MEASURE_SWEPT - the measurement is done in the frequency  */
/*      domain. For output RF spectrum due to modulation it is done using    */
/*      time-gated spectrum analysis to sweep the analyzer with the gate     */
/*      turned on for the desired portion of the burst only.                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_ORFSSetMeasurementMode
                       (ViSession instrumentHandle,
                        ViInt16 GSM_ORFSMeasMode);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_ORFSGetMeasurementMode                                         */
/*                                                                           */
/*      This method retrieves the current measurement method for GSM output  */
/*      RF spectrum measurements                                             */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 GSM_ORFSMeasModeQuery                                         */
/*                                                                           */
/*      Returns the current measurement method.                              */
/*                                                                           */
/*      0 = hpesa_ORFS_MEASURE_MULTIPLE                                      */
/*      1 = hpesa_ORFS_MEASURE_SINGLE                                        */
/*      2 = hpesa_ORFS_MEASURE_SWEPT                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_ORFSGetMeasurementMode
                       (ViSession instrumentHandle,
                        ViPInt16 GSM_ORFSMeasModeQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_ORFSSetMeasurementType                                         */
/*                                                                           */
/*      This method selects the measurement type for GSM output RF spectrum  */
/*      measurements.                                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 GSM_ORFSMeasType                                               */
/*                                                                           */
/*      Selects the measurement type.                                        */
/*                                                                           */
/*      hpesa_ORFS_TYPE_MOD - only the modulation spectrum is measured.      */
/*      hpesa_ORFS_TYPE_SWITCH - only the switching transient spectrum is    */
/*      measured.                                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_ORFSSetMeasurementType
                       (ViSession instrumentHandle,
                        ViInt16 GSM_ORFSMeasType);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_ORFSGetMeasurementType                                         */
/*                                                                           */
/*      This method retrieves the current measurement type for output RF     */
/*      spectrum measurements.                                               */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 GSM_ORFSMeasTypeQuery                                         */
/*                                                                           */
/*      Returns the current measurement type.                                */
/*                                                                           */
/*      0 = hpesa_ORFS_TYPE_MOD                                              */
/*      1 = hpesa_ORFS_TYPE_SWITCH                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_ORFSGetMeasurementType
                       (ViSession instrumentHandle,
                        ViPInt16 GSM_ORFSMeasTypeQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_ORFSSetOffsetFreqList                                          */
/*                                                                           */
/*      This method select the list of frequency offsets and bandwidths to   */
/*      be used to make the ORFS measurement. Frequency offsets are used     */
/*      only in ORFS multiple measurement method for both ORFS due to        */
/*      modulation and switching transients.                                 */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 GSM_ORFSOffsetListType                                         */
/*                                                                           */
/*      Select the list of frequency offsets and bandwidths to be used to    */
/*      make the ORFS measurement. Frequency offsets are used only in ORFS   */
/*      multiple measurement method for both ORFS due to modulation and      */
/*      switching transients.                                                */
/*                                                                           */
/*      hpesa_ORFS_OFFSET_STANDARD - the complete list of the offset         */
/*      frequencies specified in the GSM Standards, except for those offsets */
/*      greater than 6 MHz. It uses two internal offset frequency lists, one */
/*      for modulation spectrum and the other for switching transient        */
/*      spectrum. These offset frequencies cannot be changed, but the        */
/*      resolution bandwidths can be changed by other commands in the        */
/*      SENSe:ORFSpectrum subsystem.                                         */
/*                                                                           */
/*      hpesa_ORFS_OFFSET_SHORT - a shortened list of the offset frequencies */
/*      specified in the GSM Standards. It uses two internal offset          */
/*      frequency lists, one for modulation spectrum and the other for       */
/*      switching transient spectrum. These offset frequencies cannot be     */
/*      changed, but the resolution bandwidths can be changed by other       */
/*      commands in the SENSe:ORFSpectrum subsystem.                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_ORFSSetOffsetFreqList
                       (ViSession instrumentHandle,
                        ViInt16 GSM_ORFSOffsetListType);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_ORFSGetOffsetFreqList                                          */
/*                                                                           */
/*      This method retrieves the current offset frequency list type for     */
/*      output RF spectrum measurements.                                     */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 GSM_ORFSOffsetListTypeQuery                                   */
/*                                                                           */
/*      Returns the current offset frequency list type.                      */
/*                                                                           */
/*      0 = hpesa_ORFS_OFFSET_STANDARD                                       */
/*      1 = hpesa_ORFS_OFFSET_SHORT                                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_ORFSGetOffsetFreqList
                       (ViSession instrumentHandle,
                        ViPInt16 GSM_ORFSOffsetListTypeQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_ORFSSetOffsetFreq                                              */
/*                                                                           */
/*      This method sets the offset frequency that is used to measure a      */
/*      single offset.  This function is only valid if the measurement       */
/*      method is set to single offset via hpesa_GSM_ORFSSetMeasurementMode. */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 GSM_ORFSOffsetFreq                                            */
/*                                                                           */
/*      Sets the offset frequency.                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_ORFSSetOffsetFreq
                       (ViSession instrumentHandle,
                        ViReal64 GSM_ORFSOffsetFreq);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_ORFSGetOffsetFreq                                              */
/*                                                                           */
/*      Returns the current offset frequency being used to measure a single  */
/*      offset for GSM output RF spectrum measurements.                      */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPReal64 GSM_ORFSOffsetFreqQuery                                      */
/*                                                                           */
/*      Returns the current offset frequency.                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_ORFSGetOffsetFreq
                       (ViSession instrumentHandle,
                        ViPReal64 GSM_ORFSOffsetFreqQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_ORFSSetTriggerSource                                           */
/*                                                                           */
/*      This method sets the trigger source for GSM output RF spectrum       */
/*      measurements.                                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 GSM_ORFSTriggerSource                                          */
/*                                                                           */
/*      Select the trigger source used to control the data acquisitions.     */
/*                                                                           */
/*      hpesa_ORFS_TRIG_IMMEDIATE - the next data acquisition is immediately */
/*      taken, capturing the signal asynchronously (also called free run)    */
/*                                                                           */
/*      hpesa_ORFS_TRIG_EXTERNAL - front panel external trigger input        */
/*                                                                           */
/*      hpesa_ORFS_TRIG_RFBURST - internal wideband RF burst envelope        */
/*      trigger that has automatic level control for periodic burst signals  */
/*                                                                           */
/*      hpesa_ORFS_TRIG_FRAME - uses the internal frame timer, which has     */
/*      been synchronized to the selected burst sync                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_ORFSSetTriggerSource
                       (ViSession instrumentHandle,
                        ViInt16 GSM_ORFSTriggerSource);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_ORFSGetTriggerSource                                           */
/*                                                                           */
/*      This method retrieves the current trigger source for output RF       */
/*      spectrum measurements.                                               */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 GSM_ORFSTriggerSourceQuery                                    */
/*                                                                           */
/*      Returns the current trigger source.                                  */
/*                                                                           */
/*      0 = hpesa_ORFS_TRIG_IMMEDIATE                                        */
/*      1 = hpesa_ORFS_TRIG_EXTERNAL                                         */
/*      2 = hpesa_ORFS_TRIG_RFBURST                                          */
/*      3 = hpesa_ORFS_TRIG_FRAME                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_ORFSGetTriggerSource
                       (ViSession instrumentHandle,
                        ViPInt16 GSM_ORFSTriggerSourceQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_ORFSSetBurstSync                                               */
/*                                                                           */
/*      This method sets the method of synchronizing the output RF spectrum  */
/*      measurement to the GSM bursts.                                       */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 GSM_ORFSBurstSyncSource                                        */
/*                                                                           */
/*      Select the method of synchronizing the measurement to the GSM        */
/*      bursts.                                                              */
/*                                                                           */
/*      hpesa_BURST_SYNC_TSEQ - the training sequence burst sync performs a  */
/*      demodulation of the burst and determines the start and stop of the   */
/*      useful part of the burst based on the midamble training sequence.    */
/*                                                                           */
/*      hpesa_BURST_SYNC_RFAMP - the RF amplitude burst synch approximates   */
/*      the start and stop of the useful part of the burst without digital   */
/*      demodulation of the burst.                                           */
/*                                                                           */
/*      hpesa_BURST_SYNC_NONE - performs no burst synchronization.           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_ORFSSetBurstSync
                       (ViSession instrumentHandle,
                        ViInt16 GSM_ORFSBurstSyncSource);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_ORFSGetBurstSync                                               */
/*                                                                           */
/*      This method retrieves the current burst sync source for GSM output   */
/*      RF spectrum measurements.                                            */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 GSM_ORFSBurstSyncSourceQuer                                   */
/*                                                                           */
/*      Returns the current burst sync source.                               */
/*                                                                           */
/*      0 = hpesa_BURST_SYNC_TSEQ                                            */
/*      1 = hpesa_BURST_SYNC_RFAMP                                           */
/*      2 = hpesa_BURST_SYNC_NONE                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_ORFSGetBurstSync
                       (ViSession instrumentHandle,
                        ViPInt16 GSM_ORFSBurstSyncSourceQuer);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_ORFSSetWidebandNoiseMode                                       */
/*                                                                           */
/*      This method sets wideband noise function to ON or OFF. When set to   */
/*      OFF, the analyzer is tuned to the carrier and -1800 kHz to +1800 kHz */
/*      either side of the center frequency is swept. When set to ON, the    */
/*      whole of the relevant band 2 MHz either side is swept.               */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViBoolean GSM_ORFS_WBNoise                                             */
/*                                                                           */
/*      Sets wideband noise state.                                           */
/*                                                                           */
/*      VI_TRUE - Sets wideband noise funciton on.                           */
/*      VI_FALSE - Sets wideband noise function off.                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_ORFSSetWidebandNoiseMode
                       (ViSession instrumentHandle,
                        ViBoolean GSM_ORFS_WBNoise);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_ORFSGetWidebandNoiseMode                                       */
/*                                                                           */
/*      This method retrieves the current state of the wideband noise        */
/*      function for GSM output RF spectrum measurements.                    */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPBoolean GSM_ORFS_WBNoiseQuery                                       */
/*                                                                           */
/*      Returns the current state of the wideband noise function.            */
/*                                                                           */
/*      0 = VI_FALSE                                                         */
/*      1 = VI_TRUE                                                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_ORFSGetWidebandNoiseMode
                       (ViSession instrumentHandle,
                        ViPBoolean GSM_ORFS_WBNoiseQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TSPConfigure                                                   */
/*                                                                           */
/*      This method sets up the instrument for GSM Transmit Band Spur (TSP)  */
/*      measurements using the factory default instrument settings and stops */
/*      the current measurement. It does not initiate the taking of          */
/*      measurement data.                                                    */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TSPConfigure
                       (ViSession instrumentHandle);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TSPMeasure                                                     */
/*                                                                           */
/*      This method measures the GSM spurious emissions in the transmit band */
/*      relative to the channel power in the selected channel.  Three values */
/*      are returned that correspond to the frequency and amplitude of the   */
/*      worst spur and the Float32NAN.                                       */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 GSM_TSPMeasureTimeout                                         */
/*                                                                           */
/*      This value temporarily changes the VISA timeout setting for the      */
/*      duration of the instrument measurement. The user needs to set this   */
/*      value to an appropriate time that will be longer then the expected   */
/*      measurement time. Units are in seconds.                              */
/*                                                                           */
/*    ViInt16 GSM_TSPMeasureType                                             */
/*                                                                           */
/*      Selects measurement type. Note that for hpesa_FETCH type the         */
/*      instrument should already be in this measurement mode else the       */
/*      function will time out. User can select this measurement mode by     */
/*      first using the "MEASURE" or "READ" types or call the                */
/*      hpesa_GSM_TSPConfigure function.                                     */
/*                                                                           */
/*      hpesa_MEASURE - Stops current measurement, changes to this selected  */
/*      measurement, sets up instrument using factory defaults, makes        */
/*      measurement, returns measurement results.                            */
/*                                                                           */
/*      hpesa_READ - Stops current measurement,  changes to this selected    */
/*      measurement, uses instrument settings from last measurement of this  */
/*      type, makes measurement, returns measurements results.               */
/*                                                                           */
/*      hpesa_FETCH - Returns measurements results only. No new measurement  */
/*      is initiated.                                                        */
/*                                                                           */
/*    ViPReal64 GSM_TSPWorstSpurFreqQuery                                    */
/*                                                                           */
/*      The frequency of the worst spur (in Hz).                             */
/*                                                                           */
/*    ViPReal64 GSM_TSPWorstSpurAmpQuery                                     */
/*                                                                           */
/*      The amplitude of the worst spur relative to limit (in dB).           */
/*                                                                           */
/*    ViPReal64 GSM_TSMFloat32NANQuery                                       */
/*                                                                           */
/*      Float32NAN is returned at all times for GSM.                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TSPMeasure
                       (ViSession instrumentHandle,
                        ViReal64 GSM_TSPMeasureTimeout,
                        ViInt16 GSM_TSPMeasureType,
                        ViPReal64 GSM_TSPWorstSpurFreqQuery,
                        ViPReal64 GSM_TSPWorstSpurAmpQuery,
                        ViPReal64 GSM_TSMFloat32NANQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TSPMeasureTrace                                                */
/*                                                                           */
/*      This method measures the GSM spurious emissions in the transmit band */
/*      relative to the channel power in the selected channel.               */
/*                                                                           */
/*      User can select one of three types of measurement modes: Measure,    */
/*      Read, or Fetch.                                                      */
/*                                                                           */
/*                                                                           */
/*      This method returns the TSP measurement results specified by the     */
/*      "GSM_TSPResultsType" parameter as a real array.                      */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViReal64 GSM_TSPMeasureTraceTimeout                                    */
/*                                                                           */
/*      This value temporarily changes the VISA timeout setting for the      */
/*      duration of the instrument measurement. The user needs to set this   */
/*      value to an appropriate time that will be longer then the expected   */
/*      measurement time. Units are in seconds.                              */
/*                                                                           */
/*    ViInt16 GSM_TSPMeasureTraceType                                        */
/*                                                                           */
/*      Selects measurement type. Note that for hpesa_FETCH type the         */
/*      instrument should already be in this measurement mode else the       */
/*      function will time out. User can select this measurement mode by     */
/*      first using the "MEASURE" or "READ" types or call the                */
/*      hpesa_GSM_TSPConfigure function.                                     */
/*                                                                           */
/*      hpesa_MEASURE - Stops current measurement, changes to this selected  */
/*      measurement, sets up instrument using factory defaults, makes        */
/*      measurement, returns measurement results.                            */
/*                                                                           */
/*      hpesa_READ - Stops current measurement,  changes to this selected    */
/*      measurement, uses instrument settings from last measurement of this  */
/*      type, makes measurement, returns measurements results.               */
/*                                                                           */
/*      hpesa_FETCH - Returns measurements results only. No new measurement  */
/*      is initiated.                                                        */
/*                                                                           */
/*    ViInt16 GSM_TSPResultsType                                             */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_0:  Not supported by instrument.                  */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_1:  Returns 3 scalar results:                     */
/*      1. The frequency of the worst spur (in Hz)                           */
/*      2. The amplitude of the worst spur relative to limit (in dB)         */
/*      3. Float32NAN is returned at all times for GSM                       */
/*                                                                           */
/*      hpesa_RESULTS_TYPE_2:  Returns the current trace data (401 point     */
/*      real number comma separated list).                                   */
/*                                                                           */
/*    ViAReal64 GSM_TSPDataResults                                           */
/*                                                                           */
/*      Trace data returned by measurement, formatted according to           */
/*      GSM_TSPResultsType.                                                  */
/*                                                                           */
/*    ViInt32 GSM_TSPDataResultsMaxSize                                      */
/*                                                                           */
/*      Specifies the maximum size of GSM_TSPDataResults array.              */
/*                                                                           */
/*    ViPInt32 GSM_TSPDataResultsSize                                        */
/*                                                                           */
/*      Returns the actual size of GSM_TSPDataResults array.                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TSPMeasureTrace
                       (ViSession instrumentHandle,
                        ViReal64 GSM_TSPMeasureTraceTimeout,
                        ViInt16 GSM_TSPMeasureTraceType,
                        ViInt16 GSM_TSPResultsType,
                        ViAReal64 GSM_TSPDataResults,
                        ViInt32 GSM_TSPDataResultsMaxSize,
                        ViPInt32 GSM_TSPDataResultsSize);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TSPSetAveraging                                                */
/*                                                                           */
/*      This method sets the averaging for GSM transmit band spur (TSP)      */
/*      measurements.  User can control the number of averages, turn off or  */
/*      on averaging, set the terminal control mode for averaging, and set   */
/*      the average type.                                                    */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt32 GSM_TSPAverageCount                                            */
/*                                                                           */
/*      Set the number of data acquisitions that will be averaged. After the */
/*      specified number of average counts, the averaging mode (terminal     */
/*      control) setting determines the averaging action.                    */
/*                                                                           */
/*    ViBoolean GSM_TSPAverageState                                          */
/*                                                                           */
/*      Sets the state of averaging, either on or off.                       */
/*                                                                           */
/*      VI_TRUE = Averaging on.                                              */
/*      VI_FALSE = Averaging off.                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TSPSetAveraging
                       (ViSession instrumentHandle,
                        ViInt32 GSM_TSPAverageCount,
                        ViBoolean GSM_TSPAverageState);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TSPGetAveraging                                                */
/*                                                                           */
/*      This method retrieves the average settings for GSM transmit band     */
/*      spur measurements.                                                   */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt32 GSM_TSPAverageCountQuery                                      */
/*                                                                           */
/*      Returns the current average count.                                   */
/*                                                                           */
/*    ViPBoolean GSM_TSPAverageStateQuery                                    */
/*                                                                           */
/*      Returns the current average atate.                                   */
/*                                                                           */
/*      0 = VI_FALSE                                                         */
/*      1 = VI_TRUE                                                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TSPGetAveraging
                       (ViSession instrumentHandle,
                        ViPInt32 GSM_TSPAverageCountQuery,
                        ViPBoolean GSM_TSPAverageStateQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TSPSetMeasurementType                                          */
/*                                                                           */
/*      This method sets the measurement type for GSM tranmit band spur      */
/*      measurements.                                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 GSM_TSPMeasType                                                */
/*                                                                           */
/*      Selects the measurement type.                                        */
/*                                                                           */
/*      hpesa_TSP_MEAS_TYPE_EXAMINE - measures spurs in all the valid        */
/*      segments and then displays the segment that has the worst spur.      */
/*                                                                           */
/*      hpesa_TSP_MEAS_TYPE_FULL - continuously measures the spurs in all    */
/*      the valid segments.                                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TSPSetMeasurementType
                       (ViSession instrumentHandle,
                        ViInt16 GSM_TSPMeasType);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TSPGetMeasurementType                                          */
/*                                                                           */
/*      This method retrieves the current measurement type for GSM transmit  */
/*      band spur measurements.                                              */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPInt16 GSM_TSPMeasTypeQuery                                          */
/*                                                                           */
/*      Returns the current measurement type.                                */
/*                                                                           */
/*      0 = hpesa_TSP_MEAS_TYPE_EXAMINE                                      */
/*      1 = hpesa_TSP_MEAS_TYPE_FULL                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TSPGetMeasurementType
                       (ViSession instrumentHandle,
                        ViPInt16 GSM_TSPMeasTypeQuery);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TSPSetLimit                                                    */
/*                                                                           */
/*      This method sets the maximum limit value for the GSM transmit band   */
/*      spur measurements.                                                   */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 GSM_TSPLimitDevice                                             */
/*                                                                           */
/*    ViInt16 GSM_TSPStandardBand                                            */
/*                                                                           */
/*      Determines the standard band that the limit will be applied to.      */
/*                                                                           */
/*      hpesa_GSM_RADIO_BAND_PGSM - P-GSM 900                                */
/*      hpesa_GSM_RADIO_BAND_EGSM - E-GSM 900                                */
/*      hpesa_GSM_RADIO_BAND_RGSM - R-GSM 900                                */
/*      hpesa_GSM_RADIO_BAND_DCS  - DCS 1800                                 */
/*      hpesa_GSM_RADIO_BAND_PCS  - PCS 1900                                 */
/*                                                                           */
/*    ViInt16 GSM_TSPLimitDevice                                             */
/*      Determines the radio device to set the limit for.                    */
/*                                                                           */
/*      hpesa_GSM_RADIO_DEVICE_MS - Mobile station transmitter               */
/*      hpesa_GSM_RADIO_DEVICE_BTS - Base station ransmitter                 */
/*      hpesa_GSM_RADIO_DEVICE_UBTS1 - Micro base station class M1           */
/*      transmitter                                                          */
/*      hpesa_GSM_RADIO_DEVICE_UBTS2 - Micro base station class M2           */
/*      transmitter                                                          */
/*      hpesa_GSM_RADIO_DEVICE_UBTS3 - Micro base station class M3           */
/*      transmitter                                                          */
/*                                                                           */
/*    ViReal64 GSM_TSPMaxLimit                                               */
/*                                                                           */
/*      Set the maximum limit value in dBm.                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TSPSetLimit
                       (ViSession instrumentHandle,
					    ViInt16 GSM_TSPStandardBand,
                        ViInt16 GSM_TSPLimitDevice,
                        ViReal64 GSM_TSPMaxLimit);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TSPGetLimit                                                    */
/*                                                                           */
/*      This method retrieves the current maximum limit value for GSM        */
/*      transmit band spur measurements.                                     */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViInt16 GSM_TSPStandardBandQ                                           */
/*                                                                           */
/*      Determines the standard band to query the limit for.                 */
/*                                                                           */
/*      hpesa_GSM_RADIO_BAND_PGSM - P-GSM 900                                */
/*      hpesa_GSM_RADIO_BAND_EGSM - E-GSM 900                                */
/*      hpesa_GSM_RADIO_BAND_RGSM - R-GSM 900                                */
/*      hpesa_GSM_RADIO_BAND_DCS  - DCS 1800                                 */
/*      hpesa_GSM_RADIO_BAND_PCS  - PCS 1900                                 */
/*                                                                           */
/*    ViInt16 GSM_TSPLimitDeviceQ                                            */
/*                                                                           */
/*      Determines the radio device to query the limit for.                  */
/*                                                                           */
/*      hpesa_GSM_RADIO_DEVICE_MS - Mobile station transmitter               */
/*      hpesa_GSM_RADIO_DEVICE_BTS - Base station transmitter                */
/*      hpesa_GSM_RADIO_DEVICE_UBTS1 - Micro base station class M1           */
/*      transmitter                                                          */
/*      hpesa_GSM_RADIO_DEVICE_UBTS2 - Micro base station class M2           */
/*      transmitter                                                          */
/*      hpesa_GSM_RADIO_DEVICE_UBTS3 - Micro base station class M3           */
/*      transmitter                                                          */
/*                                                                           */
/*    ViPReal64 GSM_TSPMaxLimitQuery                                         */
/*                                                                           */
/*      Returns the current limit value.                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TSPGetLimit
                       (ViSession instrumentHandle,
                        ViInt16 GSM_TSPStandardBandQ,
                        ViInt16 GSM_TSPLimitDeviceQ,
                        ViPReal64 GSM_TSPMaxLimitQuery);



/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TSPSetIdleMode                                                 */
/*                                                                           */
/*      This method sets the idle mode for GSM transmit band spur            */
/*      measurements.                                                        */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViBoolean GSM_TSPIdleMode                                              */
/*                                                                           */
/*      Set idle mode to VI_TRUE if the transmitter is set to idle, or       */
/*      VI_FALSE if the transmitter has an active channel.                   */
/*                                                                           */
/*      VI_TRUE - sets idle mode to On                                       */
/*      VI_FALSE - sets idle mode to Off                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TSPSetIdleMode
                       (ViSession instrumentHandle,
                        ViBoolean GSM_TSPIdleMode);


/*****************************************************************************/
/*                                                                           */
/*  hpesa_GSM_TSPGetIdleMode                                                 */
/*                                                                           */
/*      This method retrieves the current idle mode for GSM transmit band    */
/*      spur measurements.                                                   */
/*                                                                           */
/*    ViSession instrumentHandle                                             */
/*                                                                           */
/*      Instrument handle returned from hpesa_init().                        */
/*                                                                           */
/*    ViPBoolean GSM_TSPIdleModeQuery                                        */
/*                                                                           */
/*      Returns the current idle mode state.                                 */
/*                                                                           */
/*      0 = VI_TRUE                                                          */
/*      1 = VI_FALSE                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC hpesa_GSM_TSPGetIdleMode
                       (ViSession instrumentHandle,
                        ViPBoolean GSM_TSPIdleModeQuery);




#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

#endif /* hpesa_INCLUDE */
