/*****************************************************************************/
/* ag875x driver for Agilent Technologies 875x, $NETWORK ANALYZER$           */
/* Driver Version: A.01.01                                                   */
/* Copyright (C) 2000 - 2001 Agilent Technologies                            */
/*---------------------------------------------------------------------------*/
/* This driver is compatible with the following VXIplug&play standards:      */
/*    WIN32 System Framework revision 3.0                                    */
/*    VTL revision 3.0 & VISA revision 1.0                                   */
/*****************************************************************************/

#define ag875x_REV_CODE    "A.01.01"                 	  /* Driver Revision */

/*****************************************************************************/
/* Exactly one of the following must be defined for the program to           */
/*   compile correctly.                                                      */
/*                                                                           */
/* #define __hpux          (for compilation for HP-UX)                       */
/* #define WIN16           (for compilation for 16-bit Windows)              */
/* #define WIN32           (for compilation for 32-bit Windows)              */
/*                                                                           */
/* In addition, if WIN32 is defined, _CVI_ may also be defined to indicate   */
/*   that the program is being compiled under LabWindows CVI 4.0 or          */
/*   greater.                                                                */
/*****************************************************************************/

/*****************************************************************************/
/* The following defines are intended to provide OS compatibility among      */
/*   Windows 16-bit, Windows 32-bit, and HP-UX C compilers.                  */
/*****************************************************************************/

#ifdef __hpux
#define _fcvt    fcvt
#endif

#ifdef WIN16
#else
#define _fmemcpy memcpy
#define _huge
#endif

/*****************************************************************************/
/* The following define specifies the use of viWrite and viRead rather       */
/*   than viPrintf and viScanf.                                              */
/* NO_FORMAT_IO is preferred in the WIN16 environment.  In many WIN16        */
/*   implementations of VISA viPrintf and viScanf do not handle blocks       */
/*   of over 32K bytes well.  Formatted IO is preferred for WIN32 and        */
/*   HP-UX.                                                                  */
/*****************************************************************************/

/* #define NO_FORMAT_IO 													 */

/*****************************************************************************/

#include <stdlib.h>           			/* prototype for malloc()  			 */
#include <string.h>          		    /* prototype for strcpy()  			 */
#include <stdio.h>            			/* prototype for sprintf() 			 */
#include <math.h>             			/* prototype for pow()     			 */

#ifdef __hpux
#define _INCLUDE_HPUX_SOURCE                    USUALLY NEEDED
#define _INCLUDE_XOPEN_SOURCE_EXTENDED          USUALLY NEEDED
#include <sys/time.h>
#else
#include <windows.h>
#endif
//  #include <iostream.h>
#include <winbase.h>
#include <time.h>
#include "visa.h"
#include "ag875x.h"

#define ag875x_ERR_MSG_LENGTH 256   	/* size of error message buffer      */
#define ZERO_DELAY          0 	        /* Number of milliseconds to delay.  */
                                 	    /* See doDelay function.             */

// Added to solve the timing problem in _cmd and other functions.
#define DELAY_TIME	150		            /* in milliseconds                   */

/*****************************************************************************/
/*  The strings below are used to do the instrument identification in the    */
/*    init routine.  The string matches the second part of the instrument's  */
/*    response to it's IDN? command.                                         */
/*****************************************************************************/
/*  DEVELOPER:                                                               */
/*  This string should match the instrument's model.  For instance, suppose  */
/*  the instrument responds to IDN? with                                     */
/*          HEWLETT PACKARD,8753E,0,7.48                                     */
/*    The appropriate IDN string below is                                    */
/*          8753E                                                            */
/*****************************************************************************/

#define hp8719d_IDN  "8719D"
#define hp8719es_IDN "8719ES"
#define hp8719et_IDN "8719ET"
#define hp8720d_IDN  "8720D"
#define hp8720es_IDN "8720ES"
#define hp8720et_IDN "8720ET"
#define hp8722d_IDN  "8722D"
#define hp8722es_IDN "8722ES"
#define hp8722et_IDN "8722ET"
#define hp8753d_IDN  "8753D"
#define hp8753e_IDN  "8753E"
#define hp8753es_IDN "8753ES"
#define hp8753et_IDN "8753ET"

/*****************************************************************************/
/*  Driver Message Strings.                                                  */
/*****************************************************************************/
/*  DEVELOPER:                                                               */
/*  Add instrument or instrument driver specific message to the bottom of    */
/*  list.                                                                    */
/*****************************************************************************/

#define ag875x_MSG_VI_OPEN_ERR            "vi was zero.  Was the ag875x_init() successful?"
#define ag875x_MSG_CONDITION              "condition"
#define ag875x_MSG_EVENT                  "event"	
#define ag875x_MSG_EVENT_HDLR_INSTALLED   "event handler is already installed for event happening"
#define ag875x_MSG_EVENT_HDLR_INST2       "Only 1 handler can be installed at a time."	
#define ag875x_MSG_INVALID_HAPPENING      "is not a valid happening."
#define ag875x_MSG_NOT_QUERIABLE          "is not queriable."	
#define ag875x_MSG_INVALID_STATUS         "Parameter 2 is invalid in function ag875x_error_message()."
#define ag875x_MSG_INVALID_STATUS_VALUE   "is not a valid viStatus value."
#define ag875x_MSG_INVALID_VI             "Parameter 1 is invalid in function ag875x_error_message()."  \
                                          "  Using an inactive ViSession may cause this error."	\
                                          "  Was the instrument driver closed prematurely?"
#define ag875x_MSG_NO_ERRORS              "No Errors"
#define ag875x_MSG_SELF_TEST_FAILED       "Self test failed." 	
#define ag875x_MSG_SELF_TEST_PASSED       "Self test passed."

#define ag875x_MSG_BOOLEAN                "Expected 0 or 1; Got %d"
#define ag875x_MSG_REAL                   "Expected %lg to %lg; Got %lg"
#define ag875x_MSG_INT                    "Expected %hd to %hd; Got %hd"
#define ag875x_MSG_LONG                   "Expected %ld to %ld; Got %ld"
#define ag875x_MSG_LOOKUP                 "Error converting string response to integer"
#define ag875x_MSG_NO_MATCH               "Could not match request to valid choices!"

#define VI_ERROR_PARAMETER1_MSG           "Parameter 1 is invalid"
#define VI_ERROR_PARAMETER2_MSG           "Parameter 2 is invalid"
#define VI_ERROR_PARAMETER3_MSG           "Parameter 3 is invalid"
#define VI_ERROR_PARAMETER4_MSG           "Parameter 4 is invalid"
#define VI_ERROR_PARAMETER5_MSG           "Parameter 5 is invalid"
#define VI_ERROR_PARAMETER6_MSG           "Parameter 6 is invalid"
#define VI_ERROR_PARAMETER7_MSG           "Parameter 7 is invalid"
#define VI_ERROR_PARAMETER8_MSG           "Parameter 8 is invalid"
#define VI_ERROR_PARAMETER9_MSG           "Parameter 9 is invalid"
#define VI_ERROR_PARAMETER10_MSG          "Parameter 10 is invalid"
#define VI_ERROR_PARAMETER11_MSG          "Parameter 11 is invalid"
#define VI_ERROR_PARAMETER12_MSG          "Parameter 12 is invalid"
#define VI_ERROR_PARAMETER13_MSG          "Parameter 13 is invalid"
#define VI_ERROR_PARAMETER14_MSG          "Parameter 14 is invalid"
#define VI_ERROR_PARAMETER15_MSG          "Parameter 15 is invalid"
#define VI_ERROR_PARAMETER16_MSG          "Parameter 16 is invalid"
#define VI_ERROR_PARAMETER17_MSG          "Parameter 17 is invalid"
#define VI_ERROR_PARAMETER18_MSG          "Parameter 18 is invalid"
#define VI_ERROR_FAIL_ID_QUERY_MSG        "Instrument IDN does not match."

#define INSTR_ERROR_INV_SESSION_MSG       "ViSession (parameter 1) was not created by this driver"
#define INSTR_ERROR_NULL_PTR_MSG          "NULL pointer detected"
#define INSTR_ERROR_RESET_FAILED_MSG      "Reset failed"
#define INSTR_ERROR_UNEXPECTED_MSG        "An unexpected error occurred"
#define INSTR_ERROR_DETECTED_MSG          "Instrument Error Detected, call ag875x_error_query()."
#define INSTR_ERROR_LOOKUP_MSG            "String not found in table"

#define FILE_OPEN_ERROR_MSG               "File I/O error - could not open specified file"
#define FILE_READ_ERROR_MSG               "File I/O error - could not read from file"
#define FILE_WRITE_ERROR_MSG              "File I/O error - could not write to file"
#define HEADER_MISMATCH                   "Instrument Data Block -Header Mismatch"
#define NOT_SUPP_IN_872X				  "This function is not supported with Agilent 8719/8720/8722 analyzers"
#define INVALID_ARRAY_NUMBER_MSG          "Invalid array number for this type of data, or for this type of instrument"            
#define REQUIRES_4_PARM_FW_MSG            "Requested function/channel requires Firmware Rev. 7.40 or higher"            
#define REQUIRES_FW_612_MSG               "Requested function requires Firmware Rev. 6.12 or higher"            
#define REQUIRES_2_PORT_CAL_MSG           "Use of auxiliary channels requires a two-port calibration for this instrument"            
#define ARRAY_TOO_SMALL_MSG               "Array is not large enough to hold all the data requested"            
#define LRN_8753_TO_872X_MSG              "File has Agilent 8753 learnstring, incompatible with Agilent 8719/8720/8722"            
#define LRN_872X_TO_8753_MSG              "File has Agilent 8719/8720/8722 learnstring, incompatible with Agilent 8753"            
#define LRN_NEWER_THAN_INST_MSG           "Learnstring revision cannot be higher than that of this instrument"            
#define LIMITYPE_ERROR_MSG                "All limit types must be 0 = Sloped, 1 = Flat or 2 = Single Point"
#define INVALID_8753_RANGE_MSG            "Power range must be an integer between 0 and 7 for the Agilent 8753"
#define MSG_READ_MATCH                    "Data/Value received does not match expected choices; or parameter value is not valid"
#define NOT_SUPP_IN_8753 				  "This function is not supported with Agilent 8753 analyzers"
#define OPTION_REQUIRED_MSG				  "This function requires an option which is not present on this instrument"
#define REQUIRES_FW_710_MSG               "Requested function requires Firmware Rev. 7.10 or higher"            
#define CALKIT_NOT_SUPPORTED_MSG          "Requested calibration kit is not supported on this instrument"            
#define NO_SUCH_CAL_STANDARD_MSG          "Requested calibration standard does not exist in the specified standard class"            
#define OPTION_INVALIDATES_FUNC_MSG 	  "This instrument has an option installed which invalidates this function"
#define REQUIRES_S_PARM_ANALYZER_MSG 	  "Function made a request only supported on analyzers with S-parameter test sets"
#define REQUIRES_FW_760_MSG               "Function made a request which requires Firmware Rev. 7.60 or higher" 
#define LIMIT_TABLE_EMPTY			      "Limit table empty"
#define MISSING_FILE_NAME				  "Missing file name"
#define NO_TITLE_FOUND					  "No title found"
#define POWERMETER_CAL_INACTIVE			  "Powermeter cal is inactive"
#define LIMIT_TEST_OFF					  "Limit test is off"

/*****************************************************************************/
/*  Error table structure.                                                   */
/*    The first element of the table is the error number, the second is the  */
/*    error message.  Error numbers in the "VISA Generic" section of the     */
/*    table are defined in the VISA header files.  Error numbers in the      */
/*    "Instrument Specific" section are defined in the driver header file.   */
/*    All of the error messages are defined above.                           */
/*****************************************************************************/
/*  DEVELOPER:                                                               */
/*  Add instrument or instrument driver specific numbers to the driver       */
/*    header file.                                                           */
/*  Add instrument or instrument driver specific messages to the bottom of   */
/*    messages listed above.                                                 */
/*  Add instrument specific table entries to the "Instrument Specific"       */
/*    section of the table below.                                            */
/*****************************************************************************/

struct instrErrStruct
	{
	ViStatus errStatus;
	ViString errMessage;
	};

const static struct instrErrStruct instrErrMsgTable[] =
	{
			/**************************/
			/*  VISA Generic          */
			/**************************/

		{ VI_ERROR_PARAMETER1,                VI_ERROR_PARAMETER1_MSG 		},
		{ VI_ERROR_PARAMETER2,                VI_ERROR_PARAMETER2_MSG 		},
		{ VI_ERROR_PARAMETER3,                VI_ERROR_PARAMETER3_MSG 		},
		{ VI_ERROR_PARAMETER4,                VI_ERROR_PARAMETER4_MSG 		},
		{ VI_ERROR_PARAMETER5,                VI_ERROR_PARAMETER5_MSG 		},
		{ VI_ERROR_PARAMETER6,                VI_ERROR_PARAMETER6_MSG 		},
		{ VI_ERROR_PARAMETER7,                VI_ERROR_PARAMETER7_MSG 		},
		{ VI_ERROR_PARAMETER8,                VI_ERROR_PARAMETER8_MSG 		},
		{ ag875x_INSTR_ERROR_PARAMETER9,      VI_ERROR_PARAMETER9_MSG 		},
		{ ag875x_INSTR_ERROR_PARAMETER10,     VI_ERROR_PARAMETER10_MSG 		},
		{ ag875x_INSTR_ERROR_PARAMETER11,     VI_ERROR_PARAMETER11_MSG 		},
		{ ag875x_INSTR_ERROR_PARAMETER12,     VI_ERROR_PARAMETER12_MSG 		},
		{ ag875x_INSTR_ERROR_PARAMETER13,     VI_ERROR_PARAMETER13_MSG 		},
		{ ag875x_INSTR_ERROR_PARAMETER14,     VI_ERROR_PARAMETER14_MSG 		},
		{ ag875x_INSTR_ERROR_PARAMETER15,     VI_ERROR_PARAMETER15_MSG 		},
		{ ag875x_INSTR_ERROR_PARAMETER16,     VI_ERROR_PARAMETER16_MSG 		},
		{ ag875x_INSTR_ERROR_PARAMETER17,     VI_ERROR_PARAMETER17_MSG 		},
		{ ag875x_INSTR_ERROR_PARAMETER18,     VI_ERROR_PARAMETER18_MSG 		},
		{ VI_ERROR_FAIL_ID_QUERY,             VI_ERROR_FAIL_ID_QUERY_MSG 	},
		{ ag875x_INSTR_ERROR_INV_SESSION,     INSTR_ERROR_INV_SESSION_MSG 	},
		{ ag875x_INSTR_ERROR_NULL_PTR,        INSTR_ERROR_NULL_PTR_MSG 		},
		{ ag875x_INSTR_ERROR_RESET_FAILED,    INSTR_ERROR_RESET_FAILED_MSG 	},
		{ ag875x_INSTR_ERROR_UNEXPECTED,      INSTR_ERROR_UNEXPECTED_MSG	},
		{ ag875x_INSTR_ERROR_DETECTED,        INSTR_ERROR_DETECTED_MSG 		},
		{ ag875x_INSTR_ERROR_LOOKUP,          INSTR_ERROR_LOOKUP_MSG 		},

			/**************************/
			/*  Instrument Specific   */
			/**************************/

		{ ag875x_FILE_OPEN_ERROR,		      FILE_OPEN_ERROR_MSG 			},
		{ ag875x_FILE_READ_ERROR,		  	  FILE_READ_ERROR_MSG 			},
		{ ag875x_FILE_WRITE_ERROR,    	      FILE_WRITE_ERROR_MSG 			},
  		{ ag875x_REC_HEADER_MISMATCH,      	  HEADER_MISMATCH     			},
		{ ag875x_NOT_SUPPORTED_WITH_872X,     NOT_SUPP_IN_872X 			    },
		{ ag875x_INVALID_ARRAY_NUMBER,        INVALID_ARRAY_NUMBER_MSG		},
		{ ag875x_NO_MATCH_ERROR,              ag875x_MSG_NO_MATCH           }, 
		{ ag875x_REQUIRES_4_PARM_FW,          REQUIRES_4_PARM_FW_MSG        }, 
		{ ag875x_REQUIRES_FW_612,             REQUIRES_FW_612_MSG           }, 
		{ ag875x_REQUIRES_2_PORT_CAL,         REQUIRES_2_PORT_CAL_MSG       }, 
		{ ag875x_ARRAY_TOO_SMALL,             ARRAY_TOO_SMALL_MSG           }, 
		{ ag875x_LRN_8753_TO_872X,            LRN_8753_TO_872X_MSG          },
		{ ag875x_LRN_872X_TO_8753,            LRN_872X_TO_8753_MSG          },
		{ ag875x_LRN_NEWER_THAN_INST,         LRN_NEWER_THAN_INST_MSG       },
		{ ag875x_LIMITYPE_ERROR,              LIMITYPE_ERROR_MSG            },
		{ ag875x_INVALID_8753_RANGE,          INVALID_8753_RANGE_MSG        },
		{ ag875x_READ_MATCH_ERROR,            MSG_READ_MATCH                },
		{ ag875x_NOT_SUPPORTED_WITH_8753,     NOT_SUPP_IN_8753 		    	},
		{ ag875x_OPTION_REQUIRED,             OPTION_REQUIRED_MSG 			},
		{ ag875x_REQUIRES_FW_710,             REQUIRES_FW_710_MSG           }, 
		{ ag875x_CALKIT_NOT_SUPPORTED,        CALKIT_NOT_SUPPORTED_MSG      }, 
		{ ag875x_NO_SUCH_CAL_STANDARD,        NO_SUCH_CAL_STANDARD_MSG      }, 
		{ ag875x_OPTION_INVALIDATES_FUNC,     OPTION_INVALIDATES_FUNC_MSG   }, 
		{ ag875x_REQUIRES_S_PARM_ANALYZER,    REQUIRES_S_PARM_ANALYZER_MSG  }, 
		{ ag875x_REQUIRES_FW_760,             REQUIRES_FW_760_MSG           },
		{ ag875x_LIMIT_TABLE_EMPTY,			  LIMIT_TABLE_EMPTY				},
		{ ag875x_MISSING_FILE_NAME,			  MISSING_FILE_NAME				},
		{ ag875x_NO_TITLE_FOUND,			  NO_TITLE_FOUND				},
		{ ag875x_POWERMETER_CAL_INACTIVE,	  POWERMETER_CAL_INACTIVE       },
		{ ag875x_LIMIT_TEST_OFF,			  LIMIT_TEST_OFF                }

	};  /*  end instrErrMsgTable[] */


/*****************************************************************************/
/*  VISA Globals data structure.                                             */
/*  It is desireable to store global variables in VISA rather than the       */
/*    driver.  This avoids having any state variables in the driver.  This   */
/*    structure defines the driver's global data.  Memory is allocated to    */
/*    this structure in the init function, which passes the pointer to VISA, */
/*    and every function which follows retrieves a pointer to it.            */
/*****************************************************************************/
/*  DEVELOPER:                                                               */
/*  Add additional globals under the "Instrument Specific" section.          */
/*  In driver functions,  The globals can then be refered to via the         */
/*  thisPtr pointer to the globals which is retrieved by the GET_GLOBALS     */
/*  macro.  GET_GLOBALS must be executed in each driver function before      */
/*  any references to thisPtr are made.                                      */
/*****************************************************************************/

struct ag875x_globals
	{
    ViSession    defRMSession;

    ViStatus     errNumber;

    ViChar       errFuncName[40];
    ViChar       errMessage[160];

    ViBoolean    errQueryDetect;

		/*****************************************************/
        /* Setting this to VI_TRUE,                          */
        /* will prevent I/O from occuring in an SRQ			 */
        /* This is needed, because VTL 3.0 doesn't provide	 */
        /* atomic write/read operations.  Don't want		 */
        /* to do I/O if the driver is in the middle of		 */
        /* a read from the instrument.						 */
        /*****************************************************/
    ViBoolean    blockSrqIO;
    ViInt32      countSrqIO;

    ViInt32      myDelay;

    ViUInt16     interfaceType;
        
        /**************************/
        /*  Instrument Specific   */
        /**************************/
	ViInt32      cur_chan;
	ViChar       inst_rsrc_name[20];
	};  /* end ag875x_globals */



/*****************************************************************************/
/*****************************************************************************/
/********************                                     ********************/
/**********                                                         **********/
/****                   SUPPORTING MACROS AND FUNCTIONS                   ****/
/**                                                                         **/
/**-------------------------------------------------------------------------**/
/**     These macros and functions are not exposed to the user, but         **/
/**     are used by other functions in the driver.                          **/
/****                                                                     ****/
/**********                                                         **********/
/********************                                     ********************/
/*****************************************************************************/
/*****************************************************************************/

/*****************************************************************************/
/*  DEBUG MACRO                                                              */
/*****************************************************************************/

							  /* don't check the debug pointer all the time! */
#ifdef DEBUG
#define ag875x_DEBUG_CHK_THIS( vi, thisPtr) 								\
	/* check for NULL user data */											\
	if( 0 == thisPtr)														\
	{																		\
 		ag875x_LOG_STATUS(vi, NULL, ag875x_INSTR_ERROR_INV_SESSION );		\
	}																		\
	{																		\
		ViSession defRM;													\
																			\
		/* This should never fail */										\
		errStatus = viGetAttribute( vi, VI_ATTR_RM_SESSION, &defRM);		\
		if( VI_SUCCESS > errStatus )										\
		{																	\
 			ag875x_LOG_STATUS(	vi, NULL, ag875x_INSTR_ERROR_UNEXPECTED );	\
		}																	\
		if( defRM != thisPtr->defRMSession )								\
		{																	\
 			ag875x_LOG_STATUS(vi, NULL, ag875x_INSTR_ERROR_INV_SESSION ); 	\
		}																	\
	}
#else
#define ag875x_DEBUG_CHK_THIS( vi, thisPtr) 	
#endif

/*****************************************************************************/
/*  CONTEXT DEPENDENT ERROR MESSAGE                                          */
/*****************************************************************************/

#define ag875x_CDE_INIT( funcname ) 										\
	thisPtr->errNumber = VI_SUCCESS;										


/*****************************************************************************/
/*  MACRO GET_GLOBALS                                                        */
/*****************************************************************************/
/*  Returns a pointer to the VISA globals storage.  This storage was         */
/*    allocated in init, but it is turned over to VISA to avoid holding      */
/*    state variables in the driver.                                         */
/*  If DEBUG is defined, thisPtr is checked for validity.                    */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession            InstrDesc                                         */
/*      The instrument's VISA address.                                       */
/*    struct ag875x_globals  *thisPtr                                        */
/*      A pointer to the VISA globals storage                                */
/*****************************************************************************/

#define GET_GLOBALS(vi,thisPtr)                                               \
{                                                                             \
    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);     \
    if (errStatus < VI_SUCCESS)                                               \
        return statusUpdate(vi, NULL, errStatus);                     	      \
}


/*****************************************************************************/
/*  DELAY Functions                                                          */
/*  setDelay (located prior to init function) sets the delay time for the    */
/*    instrument.                                                            */
/*  doDelay uses delay time to slow down the driver after a read request.    */
/*---------------------------------------------------------------------------*/
/*  In some SCPI instruments, the instrument cannot change its state to      */
/*    'processing a read request' fast enough to keep another request        */
/*    from getting through to the instrument.  By setting a short delay      */
/*    (less than 100 microseconds should be sufficient) after each read,     */
/*    the driver will wait a sufficient amount of time before attempting     */
/*    to send the next string to the instrument.                             */
/*  DO NOT document these function in the fp file or the help file.  They    */
/*    are intended to address problems in specific instruments, problems     */
/*    that may only appear on faster CPU's.  They are designed so that the   */
/*    problem can be addressed quickly in the field once it is identified.   */
/*  If the problem is identified by driver developers, a 100 ms delay        */
/*    should be set in the ag875x_init function.  The template version of    */
/*    init currently sets the delay to 0.                                    */
/*****************************************************************************/

/*****************************************************************************/
void doDelay(
        ViInt32 delayTime)
/*****************************************************************************/
/*  Delays execution for a specified number of microseconds.                 */
/*  Platform   Code    Tick Resolution  Tick Increment  Call Overhead        */
/*   WIN       16-bit  1 ms             ~55 ms          0-55 ms              */
/*   WIN32     16-bit  1 ms             ~55 ms          0-55 ms              */
/*   WIN32     32-bit  CPU dependent    CPU dependent   0-20 us              */
/*   HP-UX     32-bit  1 us             CPU dependent   0-20 us              */
/*  PARAMETERS                                                               */
/*    long           delayTime (in)                                          */
/*      The number of micro-seconds to delay before proceeding.              */
/*****************************************************************************/

{
long   ticksToCount;

        /*************************************************************/
        /*  WIN32 Version                                            */
        /*  WIN32 increments depend on the hardware.  If high-       */
        /*    frequency counting isn't supported, revert to common   */
        /*    standard functions (same as WIN16).                    */
        /*  The call overhead to execute doDelay if high-frequency   */
        /*    is supported is about 25-30 micro-seconds.             */
        /*  The call overhead to execute doDelay if high-frequency   */
        /*    is -NOT- supported is ~10 milli-seconds on             */
        /*    Windows 95 & NT.                                       */
        /*************************************************************/
#ifdef WIN32

#ifdef _CVI_
#else
__int64  ticksPerSecond;
__int64  startTicks;
__int64  nowTicks;
#endif
DWORD    msTicks;

if (delayTime == 0)
    return;

#ifdef _CVI_
#else
	if (QueryPerformanceFrequency((LARGE_INTEGER *)&ticksPerSecond))
		{
		ticksToCount = (long)((ticksPerSecond/1E6)*delayTime) + 1;
		QueryPerformanceCounter((LARGE_INTEGER *)&nowTicks);
		startTicks = nowTicks;
		while (nowTicks - startTicks < (DWORD)ticksToCount)
			QueryPerformanceCounter((LARGE_INTEGER *)&nowTicks);
		}
	else
#endif
	{
    ticksToCount = (long)((1E3/1E6)*delayTime) + 1;
    msTicks = GetTickCount();
    while (GetTickCount() - msTicks < (DWORD)ticksToCount) ;
	}

#endif

        /*************************************************************/
        /*  WIN16 Version                                            */
        /*  WIN16 counts in 1 milli-second increments.               */
        /*  The call overhead to execute doDelay if high-frequency   */
        /*    is -NOT- supported is ~55 milli-seconds on Windows     */
        /*    3.1 and ~10 milli-seconds on Windows 95 & NT.          */
        /*************************************************************/
#ifdef WIN16

DWORD    msTicks;

if (delayTime == 0)
    return;

ticksToCount = (long)((1E3/1E6)*delayTime) + 1;
msTicks = GetTickCount();
while (GetTickCount() - msTicks < (DWORD)ticksToCount);

#endif

        /*************************************************************/
        /*  HP-UX Version                                            */
        /*  HP-UX counts in 1 micro-second increments.               */
        /*  The call overhead to execute doDelay on HP-UX is about   */
        /*    15-20 micro-seconds.                                   */
        /*************************************************************/
#ifdef __hpux

struct timeval t0, t1;

if (delayTime == 0)
    return;
								 /* printf("doDelay delaying %ld\n", ticks); */ 
gettimeofday(&t0, NULL);
t0.tv_usec += delayTime;
t0.tv_sec += t0.tv_usec / 1000000;
t0.tv_usec = t0.tv_usec % 1000000;

gettimeofday(&t1, NULL);
while (t1.tv_sec < t0.tv_sec)
    gettimeofday(&t1, NULL);

if (t1.tv_sec > t0.tv_sec) return; 		 /* get out quick if past delay time */

while (t1.tv_usec < t0.tv_usec)
    gettimeofday(&t1, NULL);

#endif

return;
}        /* ----- end of function ----- */


/*****************************************************************************/
ViStatus delayScanf(
        ViSession  vi,
        ViInt32    delayTime,
        ViString   readFmt,
        ... ) 
/*****************************************************************************/
/*  Same as viVscanf, but with the delay added                               */
/*                                                                           */
/*  PARAMETERS (same as viVscanf, except added delayTime)                    */
/*    ViSession   InstrDesc (in)                                             */
/*    ViInt32     delayTime (in)                                             */
/*    ViString    readFmt (in)                                               */
/*    ...                                                                    */
/*****************************************************************************/

	{
	ViStatus  errStatus = VI_SUCCESS;
	va_list   va;

	va_start(va, readFmt);
	errStatus = viVScanf(vi, readFmt, va);
	va_end(va);

	doDelay(delayTime);

	return errStatus;
	}        /* ----- end of function ----- */


/*****************************************************************************/
ViStatus delayRead(
        ViSession  vi,
        ViInt32    delayTime,
        ViPBuf     buf,
        ViUInt32   cnt,
        ViPUInt32  retCnt) 
/*****************************************************************************/
/*  Same as viRead, but with the delay added                                 */
/*                                                                           */
/*  PARAMETERS (same as viRead, except added delayTime)                      */
/*    ViSession   InstrDesc (in)                                             */
/*    ViInt32     delayTime (in)                                             */
/*    ...                                                                    */
/*****************************************************************************/

	{
	ViStatus  errStatus = VI_SUCCESS;

	errStatus = viRead(vi, buf, cnt, retCnt);

	doDelay(delayTime);

	return errStatus;
	}        /* ----- end of function ----- */

#define ag875x_LOG_STATUS( vi, thisPtr, status )		 	\
  return statusUpdate( vi, thisPtr, status )  

/*****************************************************************************/
ViStatus statusUpdate(
        ViSession  vi,
        struct     ag875x_globals *thisPtr,
        ViStatus   s ) 
/*****************************************************************************/
/*  PARAMETERS                                                               */
/*    ViSession            InstrDesc (in)                                    */
/*    struct ag875x_globals *thisPtr (in)                                    */
/*      A pointer to the VISA globals for this VISA session                  */
/*    ViStatus             s (in)                                            */
/*      A driver error return code.                                          */
/*****************************************************************************/

	{   
	ViStatus  errStatus = VI_SUCCESS;
	ViUInt16  stb = 0;
	ViInt16   eventQ = 0;

	if (thisPtr)
		thisPtr->errNumber = s;

			/*************************************************************/
			/*  If error query detect is set on and the instrument       */
			/*    has not timed out, then ...                            */
			/*  Read the status byte into eventQ and see if the "Check   */
			/*    Error Queue" bit (0x08) is set.                        */
			/*************************************************************/

	if (thisPtr && thisPtr->errQueryDetect && s != VI_ERROR_TMO)
		{   
		errStatus = viReadSTB(vi, &stb);
		if (errStatus < VI_SUCCESS)
			return VI_ERROR_SYSTEM_ERROR;

		eventQ = (ViInt16)stb;

		if( 0x08 & eventQ )
			return ag875x_INSTR_ERROR_DETECTED;
		}

	return s;
	}        /* ----- end of function ----- */


/*****************************************************************************/
/*  MACRO ag875x_CHK_BOOLEAN                                                 */
/*****************************************************************************/
/*  Ref ag875x_chk_boolean and statusUpdate for info.                    	 */
/*****************************************************************************/

#define ag875x_CHK_BOOLEAN(my_val, err)                 \
if (ag875x_chk_boolean(thisPtr, my_val))                \
    return statusUpdate(vi, thisPtr, err);


/*****************************************************************************/
static ViBoolean ag875x_chk_boolean(
		struct    	ag875x_globals *thisPtr,
        ViBoolean  	my_val)
/*****************************************************************************/
/*  PARAMETERS                                                               */
/*    struct ag875x_globals *thisPtr (in)                                    */
/*      A pointer to the VISA globals for this VISA session                  */
/*    ViBoolean my_val (in)                                             	 */
/*      The value to be checked.                                             */
/*****************************************************************************/

	{
	ViChar message[ag875x_ERR_MSG_LENGTH];

	if ( (my_val != VI_TRUE) && (my_val != VI_FALSE) )
		{
		sprintf(message, ag875x_MSG_BOOLEAN, my_val);	
		strcpy(thisPtr->errMessage, message);				
		return VI_TRUE;
		}

	return VI_FALSE;
	}        /* ----- end of function ----- */


/*****************************************************************************/
/*  MACRO ag875x_CHK_REAL_RANGE                                              */
/*****************************************************************************/
/*  Ref ag875x_chk_real_range and statusUpdate for info.                 	 */
/*****************************************************************************/

#define ag875x_CHK_REAL_RANGE(my_val, min, max, err)        \
if (ag875x_chk_real_range(thisPtr, my_val, min, max))   	\
    return statusUpdate(vi, thisPtr, err);


/*****************************************************************************/
static ViBoolean ag875x_chk_real_range( 
        struct    ag875x_globals *thisPtr,
        ViReal64  my_val,
        ViReal64  min,
        ViReal64  max)
/*****************************************************************************/
/*  Tests to see if a ViReal64 is in range.                                  */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    struct ag875x_globals *thisPtr (in)                                    */
/*      A pointer to the VISA globals for this VISA session                  */
/*    ViReal64             my_val (in)                                       */
/*      The value to be checked.                                             */
/*    ViReal64             min (in)                                          */
/*      The bottom of the range.                                             */
/*    ViReal64             max (in)                                          */
/*      The top of the range.                                                */
/*****************************************************************************/

	{
	ViChar message[ag875x_ERR_MSG_LENGTH];

	if ( (my_val < min) || (my_val > max) )  			
		{								
		sprintf(message, ag875x_MSG_REAL, min, max, my_val);	
		strcpy(thisPtr->errMessage, message);
		return VI_TRUE;
		}

	return VI_FALSE;
	}        /* ----- end of function ----- */ 
 
  
/*****************************************************************************/
/*  MACRO ag875x_CHK_INT_RANGE                                               */
/*****************************************************************************/
/*  Ref ag875x_chk_int_range and statusUpdate for info.                  	 */
/*****************************************************************************/

#define ag875x_CHK_INT_RANGE(my_val, min, max, err)       \
if(ag875x_chk_int_range(thisPtr, my_val, min, max) )      \
    return statusUpdate(vi, thisPtr, err);


/*****************************************************************************/
static ViBoolean ag875x_chk_int_range( 
        struct   ag875x_globals *thisPtr,
        ViInt16  my_val,
        ViInt16  min,
        ViInt16  max)
/*****************************************************************************/
/*  Tests to see if a ViInt16 is in range.                                   */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    struct ag875x_globals *thisPtr (in)                                    */
/*      A pointer to the VISA globals for this VISA session                  */
/*    ViInt16              my_val (in)                                       */
/*      The value to be checked.                                             */
/*    ViInt16              min (in)                                          */
/*      The bottom of the range.                                             */
/*    ViInt16              max (in)                                          */
/*      The top of the range.                                                */
/*****************************************************************************/

	{
	ViChar message[ag875x_ERR_MSG_LENGTH];

	if ( (my_val < min) || (my_val > max) )  			
		{								
		sprintf(message, ag875x_MSG_INT, min, max, my_val);	
		strcpy(thisPtr->errMessage, message);
		return VI_TRUE;
		}

	return VI_FALSE;
	}        /* ----- end of function ----- */ 
   
   
/*****************************************************************************/
/*  MACRO ag875x_CHK_LONG_RANGE                                              */
/*****************************************************************************/
/*  Ref ag875x_chk_long_range and statusUpdate for info.    	             */
/*****************************************************************************/

#define ag875x_CHK_LONG_RANGE(my_val, min, max, err)      \
if (ag875x_chk_long_range(thisPtr, my_val, min, max))     \
    return statusUpdate(vi, thisPtr, err);


/*****************************************************************************/
static ViBoolean ag875x_chk_long_range( 
        struct  ag875x_globals *thisPtr,
        ViInt32 my_val,
        ViInt32 min,
        ViInt32 max)
/*****************************************************************************/
/*  Tests to see if a ViInt32 is in range.                                   */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    struct ag875x_globals *thisPtr (in)                                    */
/*      A pointer to the VISA globals for this VISA session                  */
/*    ViInt32              my_val (in)                                       */
/*      The value to be checked.                                             */
/*    ViInt32              min (in)                                          */
/*      The bottom of the range.                                             */
/*    ViInt32              max (in)                                          */
/*      The top of the range.                                                */
/*****************************************************************************/

	{
	ViChar message[ag875x_ERR_MSG_LENGTH];

	if ( (my_val < min) || (my_val > max) )  			
		{								
		sprintf(message, ag875x_MSG_LONG, min, max, my_val);	
		strcpy(thisPtr->errMessage, message);
		return VI_TRUE;
		}

	return VI_FALSE;
	}        /* ----- end of function ----- */ 
   
   
/*****************************************************************************/
/*  MACRO ag875x_CHK_ENUM                                                    */
/*****************************************************************************/
/*  Ref ag875x_chk_enum and statusUpdate for info.                       	 */
/*****************************************************************************/

#define ag875x_CHK_ENUM( my_val, limit, err )             \
if( ag875x_chk_enum( thisPtr, my_val, limit) )            \
    return statusUpdate( vi, thisPtr, err);


/*****************************************************************************/
static ViBoolean ag875x_chk_enum (
        struct ag875x_globals *thisPtr,
        ViInt16 my_val,
        ViInt16 limit)
/*****************************************************************************/
/* Chk_enum checks whether my_val lies between zero and limit                */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    struct ag875x_globals *thisPtr (in)                                    */
/*      A pointer to the VISA globals for this VISA session                  */
/*    ViInt16              my_val (in)                                       */
/*      The value to be checked.                                             */
/*    ViInt16              limit (in)                                        */
/*      The top of the range.                                                */
/*****************************************************************************/

	{
	ViChar message[ag875x_ERR_MSG_LENGTH];

	if ( ( my_val < 0 ) || (my_val > limit) )  			
		{								
		sprintf(message, ag875x_MSG_INT, 0, limit, my_val);	
		strcpy(thisPtr->errMessage, message);
		return VI_TRUE;
		}

	return VI_FALSE;
	}        /* ----- end of function ----- */


/*****************************************************************************/
/*  MACRO ag875x_CHK_PTR                                                     */
/*****************************************************************************/  
/*                                                                           */
/* Does checks of pointers for NULL and error reporting.                     */
/* "chk_val" is a function input parameter of some pointer                   */
/* type. "err_num" is one of the                                             */
/* "ag875x_INSTR_ERROR_PARAMETER#" error numbers.                            */
/*****************************************************************************/

#define ag875x_CHK_PTR(chk_val, err_num)		\
	if (!chk_val)								\
		{										\
		strcpy(thisPtr->errMessage, INSTR_ERROR_NULL_PTR_MSG); \
		thisPtr->errNumber = err_num;			\
		return err_num;							\
		}


/*****************************************************************************/
ViStatus _VI_FUNC ag875x_setDelay (
        ViSession  vi,
        ViInt32    delayTime)
/*****************************************************************************/
/*  PARAMETERS                                                               */
/*    ViSession  vi (in)                                                     */
/*      Instrument Handle returned from ag875x_init()                        */
/*    ViInt32    delayTime (in)                                              */
/*      The delayTime in microseconds.  Must be between 0 and 1,000.  In     */
/*      practice, should never be set higher than 100.                       */
/*****************************************************************************/

	{
	ViStatus   errStatus = VI_SUCCESS;

	struct ag875x_globals *thisPtr;

	GET_GLOBALS(vi,thisPtr)

	ag875x_CHK_LONG_RANGE(delayTime, 0L, 1000L, VI_ERROR_PARAMETER2);

	thisPtr->myDelay = delayTime;

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */


/*****************************************************************************/
ViStatus _VI_FUNC initError(
        ViPSession vi,
        ViStatus   errStatus)

/*****************************************************************************/
/*  This function is only called from ag875x_init.                           */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViPSession      vi (in)                                                */
/*      Pointer to Instrument Handle returned by viOpen.                     */
/*    ViStatus       errStatus (in)                                          */
/*      A driver error return code.                                          */
/*****************************************************************************/

	{
	viClose(*vi);
	*vi=VI_NULL;
	return errStatus;
	}        /* ----- end of function ----- */


// Declaration of helper function
 static ViStatus _VI_FUNC ag875x_outputf(ViSession vi, ViInt16 opc_mode, ViString fmtStr, ...);


/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : static int strip
 *-----------------------------------------------------------------------------
 * PURPOSE : Strips any leading/trailing quotes.  
 *            
 * PARAM 1 : ViString request.  String to be stripped
 * IN		  
 * PARAM 2 : ViPString selection.  Stripped String
 * OUT		  
 *-----------------------------------------------------------------------------
 */

static int strip(ViString request,
		  ViPString selection)
	   
	{			   
	strcpy(selection,request);

	if (selection[0]=='\"')  
		strcpy(selection, selection+1);  // remove first quote if any
	if (selection[strlen(selection)-1] == '\"')  
		selection[strlen(selection)-1] = '\0'; // overwrite last quote if any

	return 0;
	}			 /* ----- end of function ----- */

/*****************************************************************************/
/*  Static Int stripLF                                                       */
/*****************************************************************************/
/* PURPOSE : Strips any trailing line feed char from the string              */
/* NOTE:      The LF char (ASCII 10) is not the same as '\n'.                */
/*                                                                           */
/* PARAM 1 : ViPString selection.  String to be stripped.                    */
/*                                                                           */
/*****************************************************************************/

static int stripLF(ViPString selection) 
	{             
    if (selection[strlen(selection) - 1] == 10)
        selection[strlen(selection) - 1] = '\0';
	return 0;
	}

/*-----------------------------------------------------------------------------
 * FUNC    : static int lrnToFile
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : This function is used only by the ag875x_readLongLrnString function.
 *           The purpose is to write the learnstring and data arrays to the disk
 *           file created by ag875x_readLongLrnString, along with the associated
 *           GPIB commands for recalling the learnstring and data.
 * 
 * PARAM 1 : FILE *fipLrn
 * IN        
 *            Pointer to the file that was opened by the ag875x_readLongLrnString
 *            function.
 * 
 * PARAM 2 : char *readWriteBuf
 * IN/OUT        
 *            Pointer to a character array which:
 *            1) If 'iDataType' corresponds to learnstring or cal kit, 
 *               'readWriteBuf' must contain the learnstring or cal kit upon 
 *               input to this function.
 *            2) If 'iDataType' corresponds to an array or trace, 'dpDatArray' 
 *               must contain the array or trace upon input to this function.  
 *               'readWriteBuf' will receive the array or trace from 
 *               'dpDatArray', so 'readWriteBuf' is used as an output.
 * 
 * PARAM 3 : int iNumBytes
 * IN        
 *            If this function is being called to write the learnstring or cal 
 *            kit definition to the file, 'iNumBytes' should indicate the total 
 *            number of bytes contained in 'readWriteBuf'.  If this function is
 *            being called to write an array or trace to the file, 'iNumBytes'
 *            should indicate the total number of bytes contained in 'dpDatArray'.
 * 
 * PARAM 4 : int iDataType
 * IN        
 *            Type of data passed to this function for writing to the file.
 *            0 = learnstring
 *            1 = calibration kit definition
 *            2 = power meter calibration array
 *            3 = calibration array
 *            4 = data trace
 *            5 = memory trace
 * 
 * PARAM 5 : double *dpDatArray
 * IN        
 *            Pointer to array or trace data (if any) passed to this function.  
 *            This parameter is unused in the case of a learnstring or cal kit.
 * 
 * PARAM 6 : int iArrayNum
 * IN        
 *            If data type is a cal array, this parameter specifies the cal
 *            array number.  If data type is a power meter cal array, this
 *            parameter specifies the channel number to write it to.  Otherwise,
 *            this parameter is unused.
 * 
 * RETURN  :   0:No error. 
 *            -1:Indicates error condition.
 *
 *-----------------------------------------------------------------------------
 */

#define kiMaxNumVals 3264
#define kiMaxBufSize 13090

static int lrnToFile(FILE *fipLrn,  
  char *readWriteBuf,
  int iNumBytes,
  int iDataType,
  double *dpDatArray,
  int iArrayNum)

	{
	short int i, iNewNumBytes, iTotBytes, iDigits;		 

	if (iDataType < 2)
		{
		iNewNumBytes = iNumBytes;
		}
	else if (iDataType == 2)
		{
		unsigned short int j, iNumVals;
		char *readWriteBuf_ptr; 
		
		iNumVals = iNumBytes / sizeof(double);

		readWriteBuf_ptr = readWriteBuf;

		iNewNumBytes = 0;
   		for (i=0; i<iNumVals; i++)
			{
			if (i == 0)
				j = sprintf(readWriteBuf_ptr, "%i", (long int)dpDatArray[0]);
			else 
				j = sprintf(readWriteBuf_ptr, ",%i", (long int)dpDatArray[i]);

			readWriteBuf_ptr += j;
			iNewNumBytes += j;
			}   
		}
	else
		{
		float fDatArray_temp[kiMaxNumVals];	
		unsigned short int j, iNumVals;
		char *char_ptr, *num_ptr, *readWriteBuf_ptr; 

		iNumVals = iNumBytes / sizeof(double);
		iNewNumBytes = iNumBytes / 2;

		for (i=0; i<iNumVals; i++)
			fDatArray_temp[i] = (float)dpDatArray[i];

		readWriteBuf_ptr = readWriteBuf;
		num_ptr = (char*)&iNewNumBytes;
        
		j = sprintf( readWriteBuf_ptr,"#A");
		readWriteBuf_ptr += j ;
        								  
		j = sprintf( readWriteBuf_ptr, "%c%c",*num_ptr,*(num_ptr+1));
		readWriteBuf_ptr += j ;
        	
		char_ptr = (char *)fDatArray_temp;
        
		for (i=0; i<iNewNumBytes; i++)
			*readWriteBuf_ptr++ = *char_ptr++;
		}

	if ((iDataType != 2)&&(iDataType != 3))
		{
		iTotBytes = iNewNumBytes + 19;
		if ((iTotBytes % 10000) != iTotBytes)
			iDigits = 5;
		else if ((iTotBytes % 1000) != iTotBytes)
			iDigits = 4;
		else if ((iTotBytes % 100) != iTotBytes)
			iDigits = 3;
		else // 10 <= iTotBytes < 100
			iDigits = 2;
		}

	switch (iDataType)
		{
		case 0: case 1:
			if (iDataType == 0)
				fprintf(fipLrn, "%c%hi%hiFORM1;INPULEAS",'#',iDigits,iTotBytes);
			else
				fprintf(fipLrn, "%c%hi%hiFORM1;INPUCALK",'#',iDigits,iTotBytes);
			break;

		case 2:
			iTotBytes = iNewNumBytes + 17;
			if ((iTotBytes % 10000) != iTotBytes)
				iDigits = 5;
			else if ((iTotBytes % 1000) != iTotBytes)
				iDigits = 4;
			else if ((iTotBytes % 100) != iTotBytes)
				iDigits = 3;
			else // 10 <= iTotBytes < 100
				iDigits = 2;
			fprintf(fipLrn, "%c%hi%hiFORM4;INPUPMCAL%hi",'#',iDigits,iTotBytes,iArrayNum);
			break;

		case 3:
			iTotBytes = iNewNumBytes + 21;
			if ((iTotBytes % 10000) != iTotBytes)
				iDigits = 5;
			else if ((iTotBytes % 1000) != iTotBytes)
				iDigits = 4;
			else if ((iTotBytes % 100) != iTotBytes)
				iDigits = 3;
			else // 10 <= iTotBytes < 100
				iDigits = 2;
			fprintf(fipLrn, "%c%hi%hiFORM5;INPUCALC%02i",'#',iDigits,iTotBytes,iArrayNum);
			break;

		case 4:
			fprintf(fipLrn, "%c%hi%hiFORM5;INPUDATA",'#',iDigits,iTotBytes);
			break;

		case 5:
			fprintf(fipLrn, "%c%hi%hiFORM5;INPUDATA",'#',iDigits,iTotBytes);
			break;
		} // end switch 

	if (iDataType == 2)
		fwrite(readWriteBuf, (size_t)iNewNumBytes, 1, fipLrn);
	else
		fwrite(readWriteBuf, (size_t)iNewNumBytes+4, 1, fipLrn);

	fprintf(fipLrn, "\n"); // linefeed character

	if (iDataType == 1)
		fprintf(fipLrn, "#210SAVEUSEK;\n");
	if (iDataType == 5)
		fprintf(fipLrn, "#16DATI;\n");

	return 0;
	}        /* ----- end of function ----- */	

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : static int toUpper
 *-----------------------------------------------------------------------------
 * PURPOSE : Converts a string to all upper characters.  
 *            
 * PARAM 1 : ViString lowerString.  String containing lowercase letters
 * IN		  
 * PARAM 2 : ViPString upperString.  String converted to uppercase letters
 * OUT		  
 *-----------------------------------------------------------------------------
 */

static int toUpper(ViString lowerString)
	   
	{
	int length = 0;
	int i = 0;
	ViChar upperString[256];

	strcpy(upperString,lowerString);
	length = strlen(lowerString);
	for (i = 0; i< length; i++)
		{
		upperString[i] = toupper(lowerString[i]);
		}

	strcpy(lowerString,upperString);
	return 0;
	}			 /* ----- end of function ----- */



/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : static int getChoice
 *-----------------------------------------------------------------------------
 * PURPOSE : Finds match for request in array: arr.  
 *            
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 * PARAM 2 : ViString request.  String to be searched for
 * IN		  
 * PARAM 3 : ViInt32 size.  Total # of possible choices (# of array elements)
 * IN        
 * PARAM 4 : ViString arr[].  String Array of all possible choices
 * IN        
 * PARAM 5 : ViPInt32 choice.  Returns choice number (position in array)
 * OUT        
 * 
 * RETURN  : 0 if request is found within array of choices;  1 if not found.
 *-----------------------------------------------------------------------------
 */

static int getChoice(
       ViSession vi, 
	   ViString request,
	   ViInt32 size,
	   ViString arr[],
	   ViPInt32 choice)

	{			   
	ViInt32 index = 0; 
	ViChar message[ag875x_ERR_MSG_LENGTH];
	ViChar req[80];
	struct ag875x_globals *thisPtr;

	ViStatus errStatus = VI_SUCCESS;
	GET_GLOBALS(vi, thisPtr)

	strcpy(req,request);
	toUpper(req);

	if (req[0]=='\"')  
		strcpy(req, req+1);  // remove first quote if any
	if (req[strlen(req)-1] == '\"')  
		req[strlen(req)-1] = '\0'; // overwrite last quote if any

	while (strcmp(req, arr[index]))
		{
		index++;
		if (index==size)
			{
			sprintf(message, "Request=`%s'", req);
			strcpy(thisPtr->errMessage, message);
			return 1;
			}
		}
	*choice= index;

	return 0;
	}			 /* ----- end of function ----- */


/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : static int compareFirmwareVersions
 *-----------------------------------------------------------------------------
 * PURPOSE : Takes two firmware versions in floating point format,
 *           type casts to ints then compares with the user supplied 
 *			 comparison operator. For example, format is: ver1 <= ver2 and 
 *			 returns VI_TRUE or VI_FALSE.
 *            
 * PARAM 1 : Firmware version #1
 * IN		  
 * PARAM 2 : Comparison operator, as a string
 * IN		  
 * PARAM 3 : Firmware version #2
 * IN		 
 *-----------------------------------------------------------------------------
 */

 static ViBoolean compFirmwareVersions(ViReal64 ver1, ViString compOperator, ViReal64 ver2)
	   
	{
	ViBoolean returnVal = VI_FALSE;

	if (strcmp(compOperator,"<") == 0)
		returnVal = (( (int)(ver1 * 100) < (int)(ver2 * 100))? VI_TRUE : VI_FALSE);
	else if (strcmp(compOperator,"<=") == 0)
		returnVal = (( (int)(ver1 * 100) <= (int)(ver2 * 100))? VI_TRUE : VI_FALSE);
	else if (strcmp(compOperator,"==") == 0)
		returnVal = (( (int)(ver1 * 100) == (int)(ver2 * 100))? VI_TRUE : VI_FALSE);
	else if (strcmp(compOperator,">") == 0)
		returnVal = (( (int)(ver1 * 100) > (int)(ver2 * 100))? VI_TRUE : VI_FALSE);
	else if (strcmp(compOperator,">=") == 0)
		returnVal = (( (int)(ver1 * 100) >= (int)(ver2 * 100))? VI_TRUE : VI_FALSE);
	else if (strcmp(compOperator,"!=") == 0)
		returnVal = (( (int)(ver1 * 100) != (int)(ver2 * 100))? VI_TRUE : VI_FALSE);
		
	return returnVal;

	}			 /* ----- end of function ----- */



/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : static int convertEnumToPoints
 *-----------------------------------------------------------------------------
 * PURPOSE : Receives an enum for points, and returns it as the
 *			 actual point value the instrument requires.
 *			
 *			 convertEnumToPoints does not check range values.  That 
 *			 should be done before calling this function.
 *            
 * PARAM 1 : ViInt16 enumValue.  Enumerated points value
 * IN		  		  
 *-----------------------------------------------------------------------------
 */

static int convertEnumToPoints(ViInt16 enumValue)
	   
	{
	ViInt16 instValue = 0;

	switch (enumValue)
	{
		case ag875x_PNTS_03:
			instValue = 3;
			break;
		case ag875x_PNTS_11:
			instValue = 11;
			break;
		case ag875x_PNTS_21:
			instValue = 21;
			break;
		case ag875x_PNTS_26:
			instValue = 26;
			break;
		case ag875x_PNTS_51:
			instValue = 51;
			break;
		case ag875x_PNTS_101:
			instValue = 101;
			break;
		case ag875x_PNTS_201:
			instValue = 201;
			break;
		case ag875x_PNTS_401:
			instValue = 401;
			break;
		case ag875x_PNTS_801:
			instValue = 801;
			break;
		case ag875x_PNTS_1601:
			instValue = 1601;
			break;
	}

	return instValue;
	}			 /* ----- end of convertEnumToPoints ----- */




/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : static int convertPointsToEnum
 *-----------------------------------------------------------------------------
 * PURPOSE : Receives the number of points, and returns it as the
 *			 enum value the driver requires.
 *			
 *			 convertPointsToEnum does not check range values.  That 
 *			 should be done before calling this function.
 *            
 * PARAM 1 : ViInt16 pointsValue.  Number of points passed in.
 * IN		  		  
 *-----------------------------------------------------------------------------
 */

static int convertPointsToEnum(ViInt16 pointsValue)
	   
	{
	ViInt16 enumValue = 0;

	switch (pointsValue)
	{
		case 3:
			enumValue = ag875x_PNTS_03;
			break;
		case 11:
			enumValue = ag875x_PNTS_11;
			break;
		case 21:
			enumValue = ag875x_PNTS_21;
			break;
		case 26:
			enumValue = ag875x_PNTS_26;
			break;
		case 51:
			enumValue = ag875x_PNTS_51;
			break;
		case 101:
			enumValue = ag875x_PNTS_101;
			break;
		case 201:
			enumValue = ag875x_PNTS_201;
			break;
		case 401:
			enumValue = ag875x_PNTS_401;
			break;
		case 801:
			enumValue = ag875x_PNTS_801;
			break;
		case 1601:
			enumValue = ag875x_PNTS_1601;
			break;
	}

	return enumValue;
	}			 /* ----- end of convertPointsToEnum ----- */




/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : static int getCurrentChannel
 *-----------------------------------------------------------------------------
 * PURPOSE : Queries the instrument for the currently active channel,
 *           and returns it as currentChannel.
 *            
 * PARAM 1 :  vi -- Current instrument sesssion.
 *  IN
 *
 * PARAM 2 : currentChannel -- Current channel number to be returned.
 * OUT		  		  
 *-----------------------------------------------------------------------------
 */

static int getCurrentChannel(ViSession vi, ViPInt32 currentChannel)
	   
	{
	ViStatus	errStatus = VI_SUCCESS;


	// Query instrument for the currently active channel.
	if ((errStatus = viQueryf(vi,"OUTPCHAN;\n","%i",currentChannel)) < VI_SUCCESS)
		return statusUpdate(vi, NULL, errStatus); 
		
	// Flush the buffer
	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		return statusUpdate(vi, NULL, errStatus);

	return errStatus;
	}			 /* ----- end of getCurrentChannel ----- */



/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : static int restoreOriginalChannel
 *-----------------------------------------------------------------------------
 * PURPOSE : Changes the instrument the channel number passed in as originalChannel.
 *           This function does no checking for valid channel conditions.  It 
 *			 assumes the user has previously queried the instrument for a channel
 *			 number and now wishes to restore it.
 *            
 *            
 * PARAM 1 :  vi -- Current instrument sesssion.
 *  IN
 *
 * PARAM 2 : ViPInt32 originalChannel
 * IN		  		  
 *-----------------------------------------------------------------------------
 */

static int restoreOrignialChannel(ViSession vi, ViInt32 originalChannel)
	   
	{
	ViStatus	errStatus = VI_SUCCESS;
	ViInt32 iReply = 0;

	// Restore instrument to original channel number.
	if ((errStatus = viQueryf(vi, "OPC?;CHAN%i;\n","%i",originalChannel, &iReply)) < VI_SUCCESS)
		return statusUpdate(vi, NULL, errStatus);
	
	// Flush the buffer
	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		return statusUpdate(vi, NULL, errStatus);


	return errStatus;
	}			 /* ----- end of restoreOriginalChannel ----- */



/*****************************************************************************/
/*****************************************************************************/
/********************                                     ********************/
/**********                                                         **********/
/****                          DRIVER FUNCTIONS                           ****/
/**                                                                         **/
/**-------------------------------------------------------------------------**/
/**     The functions which are exposed to the driver user start here.      **/
/**     They are in the following order:                                    **/
/**        VPP required functions.                                          **/
/**        Agilent required utility functions                               **/
/**        Agilent passthrough functions                                   **/
/**        Instrument specific functions                                    **/
/****                                                                     ****/
/**********                                                         **********/
/********************                                     ********************/
/*****************************************************************************/
/*****************************************************************************/




/*****************************************************************************/
ViStatus _VI_FUNC ag875x_init(
        ViRsrc 		InstrDesc,
        ViBoolean 	id_query,
        ViBoolean 	do_reset,
        ViPSession 	vi)
/*****************************************************************************/
/*  PARAMETERS                                                               */
/*    ViRsrc         InstrDesc (in)                                          */
/*      The instrument's VISA address.                                       */
/*      GPIB Example: "GPIB0::16::INSTR"                                     */
/*    ViBoolean      id_query (in)                                           */
/*      If VI_TRUE, validate instrument identity                             */
/*    ViBoolean      do_reset (in)                                           */
/*      If VI_TRUE, perform an instrument reset                              */
/*    ViPSession     vi (out)                                                */
/*      Pointer to Instrument Handle.  Returns VI_NULL if the init fails     */
/*****************************************************************************/

	{
	struct     ag875x_globals *thisPtr;
	ViStatus   errStatus = VI_SUCCESS;
	ViSession  defRM = 0;
	ViChar     idn_buf[256];

	*vi = VI_NULL;

			/*************************************************************/
			/* Find the Default Resource Manager.  Potential errors are  */
			/*   VI_ERROR_SYSTEM_ERROR, VI_ERROR_ALLOC                   */
			/*************************************************************/

	errStatus = viOpenDefaultRM(&defRM);
	if (VI_SUCCESS > errStatus)
		return errStatus;

			/*************************************************************/
			/* Open a session to the instrument.  Potential errors are   */
			/*   VI_ERROR_NSUP_OPER, VI_ERROR_INV_RSRC_NAME,             */
			/*   VI_ERROR_INV_ACC_MODE, VI_ERROR_RSRC_NFOUND,            */
			/*   VI_ERROR_ALLOC                                          */
			/*************************************************************/

	errStatus = viOpen(defRM, InstrDesc, VI_NULL, VI_NULL, vi);
	if (VI_SUCCESS > errStatus)
		{
		viClose(defRM);
		*vi=VI_NULL;
		return errStatus;
		}

			/*************************************************************/
			/* Allocate global memory for the session.  Inititalize the  */
			/*   memory.  Note that viSetAttribute should never fail,    */
			/*   as all of it's input conditions are satisfied.          */
			/*************************************************************/

	thisPtr = (struct ag875x_globals *)malloc(sizeof( struct ag875x_globals) );
	if (0 == thisPtr)
		{
		viClose(*vi);
		viClose(defRM);
		*vi=VI_NULL;
		return VI_ERROR_ALLOC ;
		}

	errStatus = viSetAttribute(*vi, VI_ATTR_USER_DATA, (ViAttrState)thisPtr); 
	if( VI_SUCCESS > errStatus)
		{
		viClose(*vi);
		viClose(defRM);
		*vi=VI_NULL;
		return errStatus;
		}

			/*************************************************************/
			/* Set initial values of variables in global memory.         */
			/*************************************************************/

	thisPtr->defRMSession = defRM;
	thisPtr->errNumber = VI_SUCCESS;
	thisPtr->errFuncName[0] = 0;
	thisPtr->errMessage[0] = 0;
	thisPtr->errQueryDetect = VI_FALSE;
	thisPtr->blockSrqIO = VI_FALSE;
	thisPtr->countSrqIO = 0;
	thisPtr->myDelay = 0;


	errStatus = viGetAttribute(*vi, VI_ATTR_INTF_TYPE, &thisPtr->interfaceType);
	if( VI_SUCCESS > errStatus)
		return initError(vi, errStatus);

			/*************************************************************/
			/* Reset the instrument if so instructed.                    */
			/*   GPIB reset includes a 488.2 device clear.               */
			/*************************************************************/

	if( VI_TRUE == do_reset ) 
		{
		if (thisPtr->interfaceType == VI_INTF_GPIB)
			if (viClear(*vi) <  VI_SUCCESS)
				return initError(vi, ag875x_INSTR_ERROR_RESET_FAILED);

		if (ag875x_reset(*vi) <  VI_SUCCESS)
			return initError(vi, ag875x_INSTR_ERROR_RESET_FAILED);
		}

			/*************************************************************/
			/* Validate the instrument's identity.                       */
			/*************************************************************/

	if( VI_TRUE == id_query ) 
		{
		if (thisPtr->interfaceType == VI_INTF_GPIB)
			if (viClear(*vi) <  VI_SUCCESS)
				return initError(vi, ag875x_INSTR_ERROR_RESET_FAILED);

		if ((ag875x_readID(*vi, idn_buf, "MODEL?", 0) < VI_SUCCESS )   
			|| ((strcmp(idn_buf, hp8719d_IDN) != 0) 
				&& (strcmp(idn_buf, hp8719es_IDN) != 0) 
				&& (strcmp(idn_buf, hp8719et_IDN) != 0) 
				&& (strcmp(idn_buf, hp8720d_IDN) != 0) 
				&& (strcmp(idn_buf, hp8720es_IDN) != 0) 
				&& (strcmp(idn_buf, hp8720et_IDN) != 0) 
				&& (strcmp(idn_buf, hp8722d_IDN) != 0) 
				&& (strcmp(idn_buf, hp8722es_IDN) != 0) 
				&& (strcmp(idn_buf, hp8722et_IDN) != 0) 
				&& (strcmp(idn_buf, hp8753d_IDN) != 0) 
				&& (strcmp(idn_buf, hp8753e_IDN) != 0) 
				&& (strcmp(idn_buf, hp8753es_IDN) != 0) 
				&& (strcmp(idn_buf, hp8753et_IDN) != 0)))
 					return initError(vi, VI_ERROR_FAIL_ID_QUERY);
		}

	strcpy(thisPtr->inst_rsrc_name,InstrDesc);

	return statusUpdate(*vi, thisPtr, errStatus);
	}        /* ----- end of function ----- */


/*****************************************************************************/
ViStatus _VI_FUNC ag875x_close( 
        ViSession vi)
/*****************************************************************************/
/*  PARAMETERS                                                               */
/*    ViSession      vi (in)                                                 */
/*      Instrument Handle returned from ag875x_init()                        */
/*****************************************************************************/

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViSession defRM = 0;

	GET_GLOBALS(vi,thisPtr);

			/*************************************************************/
			/* Retrieve Resource Management session, free global memory  */
			/*   for the session (if it exists), and close the vi and    */
			/*   RM sessions.                                            */
			/*************************************************************/

	defRM = thisPtr->defRMSession;

	if( thisPtr )		
		free(thisPtr);

	return viClose(defRM);
	}        /* ----- end of function ----- */


/*****************************************************************************/
ViStatus _VI_FUNC ag875x_reset( 
        ViSession vi)
/*****************************************************************************/
/*  PARAMETERS                                                               */
/*    ViSession      vi (in)                                                 */
/*      Instrument Handle returned from ag875x_init()                        */
/*****************************************************************************/

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;

	GET_GLOBALS(vi,thisPtr)

	if ((errStatus = ag875x_outputf(vi, 3, "RST")) < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);

	return statusUpdate(vi, thisPtr, errStatus);
	}        /* ----- end of function ----- */


/*****************************************************************************/
ViStatus _VI_FUNC ag875x_self_test(
        ViSession 		vi,
        ViPInt16  		test_result,
        ViChar _VI_FAR 	test_message[])
/*****************************************************************************/
/*  PARAMETERS                                                               */
/*    ViSession      vi (in)                                                 */
/*      Instrument Handle returned from ag875x_init()                        */
/*    ViPInt32       test_result (out)                                       */
/*      Self-test result                                                     */
/*    ViChar _VI_FAR test_message[] (out)                                    */
/*      Self-test status message string, limited to 256 characters.          */
/*****************************************************************************/
	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;

	*test_result = -1; 
	test_message[0] = 0; 

	GET_GLOBALS(vi,thisPtr)

	ag875x_CHK_PTR(test_result, VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(test_message, VI_ERROR_PARAMETER3);

	thisPtr->blockSrqIO = VI_TRUE;

	if ((errStatus = viPrintf(vi, "TST?\n")) < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);

	if ((errStatus = delayScanf(vi, thisPtr->myDelay,
								"%hd%*t", test_result)) < VI_SUCCESS)
		{
		*test_result = -1; 
		return statusUpdate(vi, thisPtr, errStatus);
		}

	switch( *test_result )
		{
		case 0:
			sprintf(test_message, ag875x_MSG_SELF_TEST_PASSED);
			break;

			/*************************************************************/
			/* DEVELOPER:  Add instrument specific self-test error       */
			/*   codes and messages here.                                */
			/*************************************************************/

		default:
			sprintf(test_message, ag875x_MSG_SELF_TEST_FAILED);
			break;
		}

	return statusUpdate(vi, thisPtr, errStatus);
	}        /* ----- end of function ----- */


/*****************************************************************************/
ViStatus _VI_FUNC ag875x_error_query(
        ViSession  		vi,
        ViPInt32   		inst_error_number,
        ViChar _VI_FAR 	inst_error_message[])
/*****************************************************************************/
/*  PARAMETERS                                                               */
/*    ViSession      vi (in)                                                 */
/*      Instrument Handle returned from ag875x_init()                        */
/*    ViPInt32       inst_error_number (out)                                 */
/*      An instrument error code                                             */
/*    ViChar _VI_FAR inst_error_message[] (out)                              */
/*      Instrument error message string, limited to 256 characters.          */
/*****************************************************************************/

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	char *quote_ptr;

	*inst_error_number = -1; 
	inst_error_message[0] = 0; 

	GET_GLOBALS(vi,thisPtr)

	/*  Parameter checks */
	ag875x_CHK_PTR(inst_error_number, VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(inst_error_message, VI_ERROR_PARAMETER3);

	thisPtr->blockSrqIO = VI_TRUE;

	if ((errStatus = viQueryf(vi, "OUTPERRO;\n", "%ld%t", inst_error_number, 
							  inst_error_message)) < VI_SUCCESS)
		{
		*inst_error_number = -1; 
		inst_error_message[0] = 0; 
		return statusUpdate(vi, thisPtr, errStatus);
		}

	/* remove space, comma and leading quote from beginning of error message string 
	   by incrementing pointer */
	quote_ptr = strchr(inst_error_message, (int)'\"');
	strcpy(inst_error_message, quote_ptr + 1);

	/* chop off trailing quote and LF from the end of the error message string */
	quote_ptr = strchr(inst_error_message, (int)'\"');
	*quote_ptr = 0;

	return statusUpdate(vi, thisPtr, errStatus);
	}        /* ----- end of function ----- */


/*****************************************************************************/
/*                                                                           */
/*  ag875x_error_message -                                                   */
/*    Returns a text message for an error number. This function does not     */
/*    communicate with the instrument.                                       */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession  instrumentHandle (in)                                       */
/*      Instrument Handle returned from ag875x_init()                        */
/*      May be VI_NULL for this function                                     */
/*                                                                           */
/*    ViStatus   statusCode (in)                                             */
/*      The error return value from an instrument driver function            */
/*                                                                           */
/*    ViPString  message[] (out)                                             */
/*      Error message string.  This is limited to 256 characters.            */
/*                                                                           */
/*  DEVELOPER:                                                               */
/*    When creating a new driver error, add a new error number and a new     */
/*    error message to the appropriate tables in this driver.                */
/*                                                                           */
/*    This function looks for an error message in the driver first and if    */
/*    it doesn't find one it queries VISA to see if it has the message.      */
/*                                                                           */
/*    Note: This function provides for the possibility that the              */
/*    instrumentHandle itself hasn't been initialized yet or, there is an    */
/*    error where it never got initialized!                                  */
/*---------------------------------------------------------------------------*/
ViStatus _VI_FUNC ag875x_error_message
                        (ViSession  instrumentHandle,
                         ViStatus   statusCode,
                         ViPString  message)
{
    struct ag875x_globals *thisPtr = NULL;
    ViStatus  errStatus = VI_SUCCESS;
    ViInt32   idx = 0;


    if (instrumentHandle != VI_NULL)
        GET_GLOBALS(instrumentHandle, thisPtr)

    /*  Parameter checks */

    if (thisPtr)
    {
        ag875x_CHK_PTR(message, VI_ERROR_PARAMETER3);
    }
    else if (message == VI_NULL)
    {
        return VI_ERROR_PARAMETER3;
    }

    message[0] = 0;

        /*-----------------------------------------------------------*
         | Since instrumentHandle is allowed to be NULL, the thisPtr |
         |   is allowed to be NULL also! So we check it before every |
         |   attempted use.                                          |
         *-----------------------------------------------------------*/

    if (statusCode == VI_SUCCESS)
    {
        strcpy(message, ag875x_MSG_NO_ERRORS);
        if (thisPtr)
        {
            /* clear the context dependent info. */
            thisPtr->errMessage[0] = '\0';
            thisPtr->errNumber = VI_SUCCESS;
        }
        return VI_SUCCESS;
    }

        /*-----------------------------------------------------------*
         | Search the driver error message table for the error       |
         |   message. If it's found, copy it to the output error     |
         |   message. If there is context dependent info available,  |
         |   append it to the output message.                        |
         *-----------------------------------------------------------*/

    for (idx = 0; idx < (sizeof instrErrMsgTable /
                         sizeof(struct instrErrStruct)); idx++)
    {
        if (instrErrMsgTable[idx].errStatus == statusCode)
        {
            strcpy(message, instrErrMsgTable[idx].errMessage);

            if (thisPtr)
            {
                if (thisPtr->errNumber == statusCode)
                {
                    /* use the context dependent info. */
                    strcat(message, ";  ");
                    strcat(message, thisPtr->errMessage);
                }
                /* clear the context dependent info. */
                thisPtr->errMessage[0] = '\0';
                thisPtr->errNumber = VI_SUCCESS;
            }
            return VI_SUCCESS;
        }
    }

        /*-----------------------------------------------------------*
         | Try VISA next.                                            |
         |   If error found, copy it to the output error message.    |
         |   If there is context dependent info available, append    |
         |   it to the output message.                               |
         |   (Yes, it is possible that we are making the below       |
         |   function call with instrumentHandle == VI_NULL. It's    |
         |   OK since, we are after the last error that occurred in  |
         |   VISA, even if the instrumentHandle is invalid.)         |
         *-----------------------------------------------------------*/

    errStatus = viStatusDesc(instrumentHandle, statusCode, message);
    if (errStatus == VI_SUCCESS)
    {
        if (thisPtr)
        {
            if (thisPtr->errNumber == statusCode)
            {
                /* use the context dependent info. */
                strcat(message, ";  ");
                strcat(message, thisPtr->errMessage);
            }
            /* clear the context dependent info. */
            thisPtr->errMessage[0] = '\0';
            thisPtr->errNumber = VI_SUCCESS;
        }
        return VI_SUCCESS;
    }

        /*-----------------------------------------------------------*
         | Error message was not found anywhere above so NOW we      |
         |   acknowledge a NULL instrumentHandle.                    |
         *-----------------------------------------------------------*/

    if (instrumentHandle == VI_NULL)
    {
        strcpy(message, ag875x_MSG_VI_OPEN_ERR);
        /* Since instrumentHandle is VI_NULL we know thisPtr is NULL */
        return VI_SUCCESS;
    }

        /*-----------------------------------------------------------*
         | Here the statusCode is invalid.                           |
         |   (There may be a program bug where there is no entry     |
         |   for this message in the message table in this driver.)  |
         |                                                           |
         |   (There is also a possibility that the instrumentHandle  |
         |   is invalid because of a program error.)                 |
         *-----------------------------------------------------------*/

    sprintf(message, ag875x_MSG_INVALID_STATUS "  %X "
                     ag875x_MSG_INVALID_STATUS_VALUE, statusCode);

    if (thisPtr)
    {
        /* clear the context dependent info. */
        thisPtr->errMessage[0] = '\0';
        thisPtr->errNumber = VI_ERROR_PARAMETER2;
    }
    return VI_ERROR_PARAMETER2;
}
/*---------------------------------------------------------------------------*/
/*                         end ag875x_error_message                          */
/*****************************************************************************/



/*****************************************************************************/
ViStatus _VI_FUNC ag875x_revision_query(
        ViSession      vi,
        ViChar _VI_FAR driver_rev[],
        ViChar _VI_FAR instr_rev[])
/*****************************************************************************/
/*  PARAMETERS                                                               */
/*    ViSession      vi (in)                                                 */
/*      Instrument Handle returned from ag875x_init()                        */
/*    ViChar _VI_FAR driver_rev[] (out)                                      */
/*      Instrument driver revision.  This is limited to 256 characters.      */
/*    ViChar _VI_FAR instr_rev[] (out)                                       */
/*      Instrument firmware revision.  This is limited to 256 characters.    */
/*****************************************************************************/

	{
	struct    ag875x_globals *thisPtr;
	ViStatus  errStatus = VI_SUCCESS;
	ViChar    temp_str1[256];         		   /* temp hold for instr rev string */
	ViChar    *last_comma;             		   /* last comma in IDN string 	 */
		
	GET_GLOBALS(vi,thisPtr)

	/*  Parameter checks */
	ag875x_CHK_PTR(driver_rev, VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(instr_rev, VI_ERROR_PARAMETER3);

	driver_rev[0] = 0; 
	instr_rev[0] = 0; 

	sprintf( driver_rev, "%s", ag875x_REV_CODE);
	thisPtr->blockSrqIO = VI_TRUE;

	 if ((errStatus = viPrintf( vi, "IDN?\n")) < VI_SUCCESS)
		{
		driver_rev[0] = 0;
		instr_rev[0]  = 0;
		return statusUpdate(vi, thisPtr, errStatus); 
		}

			/*************************************************************/
			/* In the standard c template, the format string is          */
			/*   "%t%*t".  This removes white space from the end of the  */
			/*   returned string, but doesn't work if there are          */
			/*   embedded spaces in the returned string.  I used this    */
			/*   format string and resorted to stripping off spaces and  */
			/*   spaces and trainling non-printing characters below.     */
			/*   This is changed in the template for the 86120.          */
			/*************************************************************/

	if ((errStatus = delayScanf(vi, thisPtr->myDelay, "%t", temp_str1)) < VI_SUCCESS)    
		{
		driver_rev[0] = 0; 
		instr_rev[0]  = 0; 
		statusUpdate(vi, thisPtr, errStatus); 
		}
    
			/*************************************************************/
			/* error and exit if last comma not found                    */
			/*************************************************************/

	last_comma = strrchr(temp_str1,',');

	/* Remove the Newline Character */
	if (temp_str1[strlen(temp_str1)-1] == '\n')
		{
		temp_str1[strlen(temp_str1)-1] = VI_NULL;
		} 		

	if (!last_comma) 
		{
		instr_rev[0] = 0;
		strcpy(thisPtr->errMessage, "no last comma found in IDN string");
		return (statusUpdate(vi, thisPtr, ag875x_INSTR_ERROR_UNEXPECTED)); 
		}

			/*************************************************************/
			/* The next statement assumes that the firmware revision of  */
			/*   the instrument follows the last comma/space pair.       */
			/*************************************************************/

	strcpy(instr_rev, last_comma+1);

	return statusUpdate(vi, thisPtr, VI_SUCCESS); 
	}        /* ----- end of function ----- */


/*****************************************************************************/
ViStatus _VI_FUNC ag875x_timeOut (
        ViSession  vi,
        ViInt32    time_out)
/*****************************************************************************/
/*  Sets the current setting of the driver timeout value for the instrument  */
/*  in milliseconds.                                                         */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession  vi (in)                                                     */
/*      Instrument Handle returned from ag875x_init()                        */
/*    ViInt32    time_out (in)                                               */
/*      The period of time which VISA will allow an instrument command to    */
/*      wait before stopping it's execution.                                 */
/*****************************************************************************/

	{
	ViStatus errStatus = 0;
	struct ag875x_globals *thisPtr;

	GET_GLOBALS(vi,thisPtr)

	/*  Parameter checks */
	ag875x_CHK_LONG_RANGE(time_out, ag875x_TIMEOUT_MIN,
						 ag875x_TIMEOUT_MAX, VI_ERROR_PARAMETER2);

	if ((errStatus = viSetAttribute(vi, VI_ATTR_TMO_VALUE, time_out)) < VI_SUCCESS)
		statusUpdate(vi, thisPtr, errStatus);

	return statusUpdate(vi, thisPtr, errStatus);
	}        /* ----- end of function ----- */


/*****************************************************************************/
ViStatus _VI_FUNC ag875x_timeOut_Q (
        ViSession  vi,
        ViPInt32   q_time_out)
/*****************************************************************************/
/*  Returns the current setting of the timeout value of the instrument in    */
/*  milliseconds.                                                            */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession  vi (in)                                                     */
/*      Instrument Handle returned from ag875x_init()                        */
/*    ViPInt32   q_time_out (out)                                            */
/*      The period of time which VISA will allow an instrument command to    */
/*      wait before stopping it's execution.                                 */
/*****************************************************************************/

	{
	ViStatus errStatus = VI_SUCCESS;
	struct ag875x_globals *thisPtr;

	GET_GLOBALS(vi,thisPtr)

	/*  Parameter checks */
	ag875x_CHK_PTR(q_time_out, VI_ERROR_PARAMETER2);

	errStatus = viGetAttribute(vi, VI_ATTR_TMO_VALUE, q_time_out);
	if (VI_SUCCESS > errStatus)
		statusUpdate(vi, thisPtr, errStatus);

	return statusUpdate(vi, thisPtr, errStatus);
	}        /* ----- end of function ----- */


/*****************************************************************************/
ViStatus _VI_FUNC ag875x_errorQueryDetect( 
        ViSession  vi,
        ViBoolean  eq_detect)
/*****************************************************************************/
/*  PARAMETERS                                                               */
/*    ViSession  vi (in)                                                     */
/*      Instrument Handle returned from ag875x_init()                        */
/*    ViBoolean  eq_detect  (in)                                             */
/*      Enables (VI_TRUE) or disables (VI_FALSE) automatic instrument error  */
/*      querying                                                             */
/*****************************************************************************/

	{
	ViStatus errStatus = VI_SUCCESS;

	struct ag875x_globals *thisPtr;

	GET_GLOBALS(vi,thisPtr)

	/* Parameter checks */
	ag875x_CHK_BOOLEAN(eq_detect, VI_ERROR_PARAMETER2);

	thisPtr->errQueryDetect = eq_detect;

	return statusUpdate( vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */


/*****************************************************************************/
ViStatus _VI_FUNC ag875x_errorQueryDetect_Q( 
        ViSession   vi,
        ViPBoolean  q_eq_detect)
/*****************************************************************************/
/*  PARAMETERS                                                               */
/*    ViSession   vi (in)                                                    */
/*      Instrument Handle returned from ag875x_init()                        */
/*    ViPBoolean  q_eq_detect (out)                                          */
/*      Equals VI_TRUE if automatic instrument error querying is being       */
/*      being performed.                                                     */
/*****************************************************************************/

	{
	ViStatus errStatus = VI_SUCCESS;

	struct ag875x_globals *thisPtr;

	GET_GLOBALS(vi,thisPtr)

	/*  Parameter checks */
	ag875x_CHK_PTR(q_eq_detect, VI_ERROR_PARAMETER2);

	*q_eq_detect = thisPtr->errQueryDetect;

	return statusUpdate( vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */


/*****************************************************************************/
ViStatus _VI_FUNC ag875x_dcl( 
        ViSession  vi)
/*****************************************************************************/
/*  PARAMETERS                                                               */
/*    ViSession vi (in)                                                      */
/*      Instrument Handle returned from ag875x_init()                        */
/*****************************************************************************/

	{
	ViStatus errStatus = VI_SUCCESS;
	struct ag875x_globals *thisPtr;

	GET_GLOBALS(vi,thisPtr)

	if (thisPtr->interfaceType == VI_INTF_GPIB)
		if ((errStatus = viClear(vi)) < VI_SUCCESS)
			return statusUpdate( vi, thisPtr, errStatus);	

	return statusUpdate( vi, thisPtr, errStatus);
	}        /* ----- end of function ----- */


/*****************************************************************************/
ViStatus _VI_FUNC ag875x_readStatusByte_Q(
        ViSession  vi,
        ViPInt16   status_byte)
/*****************************************************************************/
/*  PARAMETERS                                                               */
/*    ViSession vi (in)                                                      */
/*      Instrument Handle returned from ag875x_init()                        */
/*    ViPInt16  statusByte (out)                                             */
/*      Returns the contents of the status byte                              */
/*****************************************************************************/

	{
	ViStatus  errStatus = VI_SUCCESS;
	struct    ag875x_globals *thisPtr;
	ViUInt16  stb;

	GET_GLOBALS(vi,thisPtr);

	ag875x_CHK_PTR(status_byte, VI_ERROR_PARAMETER2);

	if ((errStatus = viReadSTB(vi,&stb)) < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus );

	*status_byte = (ViInt16)stb;

	return statusUpdate(vi, thisPtr, errStatus);
	}        /* ----- end of function ----- */


/*****************************************************************************/
ViStatus _VI_FUNC ag875x_cmd( 
        ViSession vi,
        ViString  cmd_string)
/*****************************************************************************/
/*  Send a scpi command, it does not look for a response                     */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession vi (in)                                                      */
/*      Instrument handle returned from ag875x_init()                        */
/*    ViString        cmd_string (in)                                        */
/*      The SCPI command string to be sent to the instrument                 */
/*****************************************************************************/

	{
	ViStatus errStatus = VI_SUCCESS;
	struct   ag875x_globals *thisPtr;

	GET_GLOBALS(vi,thisPtr)

	/*  Parameter checks */
	ag875x_CHK_PTR(cmd_string, VI_ERROR_PARAMETER2);

	#ifdef NO_FORMAT_IO
		{
		ViUInt32 rtn_size;
		errStatus = viWrite(vi, (ViBuf)cmd_string, strlen(cmd_string), &rtn_size );
		}
	#else
	// Added doDelay to solve an error reporting problem
	errStatus = viPrintf(vi,"%s\n", cmd_string);
	doDelay(DELAY_TIME);
	#endif
   
	return statusUpdate( vi, thisPtr, errStatus );
	}        /* ----- end of function ----- */


/*****************************************************************************/
ViStatus _VI_FUNC ag875x_cmdString_Q(
        ViSession       vi,
        ViString        sq_cmd_string,
        ViInt32         sq_size,
        ViChar _VI_FAR  sq_result[] )
/*****************************************************************************/
/*  PARAMETERS                                                               */
/*    ViSession       vi (in)                                                */
/*      Instrument handle returned from ag875x_init()                        */
/*    ViString        sq_cmd_string (in)                                     */
/*      The SCPI command string to be sent to the instrument                 */
/*    ViInt32         sq_size (in)                                           */
/*      The size of the char array (result) passed to the function to hold   */
/*      the string returned by the instrument                                */
/*    ViChar _VI_FAR  sq_result[] (out)                                      */
/*      The string returned by the instrument                                */
/*****************************************************************************/

	{
	ViStatus errStatus = VI_SUCCESS;
	struct   ag875x_globals *thisPtr;

	GET_GLOBALS(vi,thisPtr);

	/*  Parameter checks */
	ag875x_CHK_PTR(sq_cmd_string, VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(sq_result, VI_ERROR_PARAMETER4);

	/* Command strings must have at least one non-null character */
	if(sq_size < 2)     return statusUpdate(vi,VI_NULL,VI_ERROR_PARAMETER2);

	#ifdef WIN16
	/* Strings in 16-bit windows are limited to 32K characters. */	
	if(sq_size > 32767) return statusUpdate(vi,VI_NULL,VI_ERROR_PARAMETER2);
	#endif
		
	

	#ifdef NO_FORMAT_IO
		{
		ViInt32   mySize = 0;
		ViUInt32  rtn_size = 0;
		if ((errStatus = viWrite(vi, (ViBuf)sq_cmd_string,
								 strlen(sq_cmd_string), &rtn_size )) < VI_SUCCESS)
			return statusUpdate(vi,thisPtr,errStatus);
		mySize = sq_size-1;

		if ((errStatus = delayRead(vi, thisPtr->myDelay,
								 (ViPBuf)sq_result, mySize, &rtn_size)) < VI_SUCCESS)
			return statusUpdate(vi,thisPtr,errStatus);
		sq_result[rtn_size] = '\0';
		if (thisPtr->interfaceType == VI_INTF_GPIB)
			if (errStatus == VI_SUCCESS_MAX_CNT)
				viClear(vi);
		}
	#else
		{
		int sz;
		if ((errStatus = viPrintf(vi, "%s\n",sq_cmd_string )) < VI_SUCCESS)
			return statusUpdate(vi,thisPtr,errStatus);
		sz = (int)sq_size;

		if ((errStatus = delayScanf(vi, thisPtr->myDelay, "%#c", &sz,sq_result)) < VI_SUCCESS)
			return statusUpdate(vi,thisPtr,errStatus);
		sq_result[sz]='\0';
		}
	#endif
      
	return statusUpdate( vi, thisPtr, errStatus );
	}        /* ----- end of function ----- */ 


/*****************************************************************************/
ViStatus _VI_FUNC ag875x_cmdData_Q(
        ViSession  		vi,
        ViString   		dq_cmd_string,
        ViInt32    		dq_size,
        ViChar _VI_FAR 	dq_result[])
/*****************************************************************************/
/*  PARAMETERS                                                               */
/*    ViSession       vi (in)                                                */
/*      Instrument handle returned from ag875x_init()                        */
/*    ViString        dq_cmd_string (in)                                     */
/*      The SCPI command string to be sent to the instrument                 */
/*    ViInt32         dq_size (in)                                           */
/*      The size of the char array (result) passed to the function to hold   */
/*      the string returned by the instrument                                */
/*    ViChar _VI_FAR  dq_result[] (out)                                      */
/*      The string returned by the instrument                                */
/*****************************************************************************/

	{  
	ViStatus  errStatus = VI_SUCCESS;
	ViInt32   mySize = 0;
	struct    ag875x_globals *thisPtr;

	/* Command strings must have at least one non-null character */
	if(dq_size < 2)     return statusUpdate(vi,VI_NULL,VI_ERROR_PARAMETER2);

	#ifdef WIN16
	/* Strings in 16-bit windows are limited to 32K characters. */	
	if(dq_size > 32767) return statusUpdate(vi,VI_NULL,VI_ERROR_PARAMETER2);
	#endif
		
	GET_GLOBALS(vi,thisPtr);

	   /* If (message-based I/O) then write to instrument. */
	#ifdef NO_FORMAT_IO
		{
		ViUInt32 rtn_size;
		if ((errStatus = viWrite(vi, (ViBuf)dq_cmd_string,
								  strlen(dq_cmd_string), &rtn_size )) < VI_SUCCESS)
			return statusUpdate(vi,thisPtr,errStatus);
		mySize = dq_size-1;

		if ((errStatus = delayRead(vi, thisPtr->myDelay,
								  (ViPBuf)dq_result, mySize, &rtn_size)) < VI_SUCCESS)
			return statusUpdate(vi,thisPtr,errStatus);
		if (thisPtr->interfaceType == VI_INTF_GPIB)
			if (errStatus == VI_SUCCESS_MAX_CNT)
				viClear(vi);
		}
	#else
		{
		int sz;
		if ((errStatus = viPrintf(vi, "%s\n",dq_cmd_string )) < VI_SUCCESS)
			return statusUpdate(vi,thisPtr,errStatus);
		mySize = dq_size-1;
		sz = (int)dq_size;

		if ((errStatus = delayScanf(vi, thisPtr->myDelay, "%#t", &sz, dq_result)) < VI_SUCCESS)
			return statusUpdate(vi,thisPtr,errStatus);
		}
	#endif
      
	return statusUpdate( vi, thisPtr, errStatus );
	}        /* ----- end of function ----- */
 

/*****************************************************************************/
ViStatus _VI_FUNC ag875x_cmdInt(
        ViSession  vi,
        ViString   i_cmd_string,
        ViInt32    i_val )
/*****************************************************************************/
/*  Sends an instrument command which takes a single integer parameter.      */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession  vi (in)                                                     */
/*      Instrument handle returned from ag875x_init()                        */
/*    ViString   i_cmd_string (in)                                           */
/*      The instrument command string to be sent to the instrument.          */
/*    ViInt32    i_val (in)                                                  */
/*      The integer sent to the instrument at the end of the instrument      */
/*      command.  Can be ViInt16 or ViInt32.                                 */
/*****************************************************************************/

	{  
	ViStatus errStatus = VI_SUCCESS;
	struct ag875x_globals *thisPtr;
   
	GET_GLOBALS(vi,thisPtr);

	/*  Parameter checks */
	ag875x_CHK_PTR(i_cmd_string, VI_ERROR_PARAMETER2);
	
	#ifdef NO_FORMAT_IO
		{
		ViUInt32 rtn_size;
		ViChar   lc[300];

		sprintf(lc,"%s %ld",i_cmd_string,i_val);
		if ((errStatus = viWrite(vi, (ViBuf)lc,
										  strlen(lc), &rtn_size )) < VI_SUCCESS)
			return statusUpdate(vi,thisPtr,errStatus);
		}
	#else
		if ((errStatus = viPrintf(vi, "%s %ld\n",i_cmd_string,i_val )) < VI_SUCCESS)
			return statusUpdate(vi,thisPtr,errStatus);
	#endif

	return statusUpdate( vi, thisPtr, errStatus );
	}        /* ----- end of function ----- */ 


/*****************************************************************************/
ViStatus _VI_FUNC ag875x_cmdInt16_Q(
        ViSession  vi,
        ViString   iq_cmd_string,
        ViPInt16   iq_result)
/*****************************************************************************/
/*  Sends scpi command and waits for a response that must be representable   */
/*    as an int32.  A non numeric instrument response returns zero in        */
/*    result.                                                                */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession  vi (in)                                                     */
/*      Instrument handle returned from ag875x_init()                        */
/*    ViString   iq_cmd_string (in)                                          */
/*      The SCPI command string to be sent to the instrument.                */
/*    ViPInt16   iq_result (out)                                             */
/*      The integer returned from the instrument.                            */
/*****************************************************************************/

	{
	ViStatus errStatus = VI_SUCCESS;
	struct   ag875x_globals *thisPtr;
   
	GET_GLOBALS(vi,thisPtr)

	/*  Parameter checks */
	ag875x_CHK_PTR(iq_cmd_string, VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(iq_result, VI_ERROR_PARAMETER3);

	#ifdef NO_FORMAT_IO
		{
		ViChar lc[100];
		ViUInt32 rtn_size;

		if ((errStatus = viWrite(vi, (ViBuf)iq_cmd_string,
									 strlen(iq_cmd_string), &rtn_size )) < VI_SUCCESS)
			return statusUpdate(vi,thisPtr,errStatus);

		if ((errStatus = delayRead(vi, thisPtr->myDelay,
									 (ViPBuf)lc, 100, &rtn_size)) < VI_SUCCESS)
			return statusUpdate(vi,thisPtr,errStatus);
		*iq_result = atoi(lc);
		}
	#else
	if ((errStatus = viPrintf(vi, "%s\n",iq_cmd_string)) < VI_SUCCESS)
		return statusUpdate(vi,thisPtr,errStatus);

	if ((errStatus = delayScanf(vi, thisPtr->myDelay, "%hd%*t",iq_result)) < VI_SUCCESS)
		return statusUpdate(vi,thisPtr,errStatus);
	#endif

	return statusUpdate( vi, thisPtr, errStatus );
	}        /* ----- end of function ----- */ 


/*****************************************************************************/
ViStatus _VI_FUNC ag875x_cmdInt32_Q(
        ViSession vi,
        ViString  lq_cmd_string,
        ViPInt32  lq_result)
/*****************************************************************************/
/*  Sends scpi command and waits for a response that must be representable   */
/*    as an int32.  A non numeric instrument response returns zero in        */
/*    result.                                                                */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession  vi (in)                                                     */
/*      Instrument handle returned from ag875x_init()                        */
/*    ViString   lq_cmd_string (in)                                          */
/*      The SCPI command string to be sent to the instrument.                */
/*    ViPInt32   lq_result (out)                                             */
/*      The integer returned from the instrument.                            */
/*****************************************************************************/

	{  
	ViStatus  errStatus = VI_SUCCESS;
	struct    ag875x_globals *thisPtr;
   
	GET_GLOBALS(vi,thisPtr)

	/*  Parameter checks */
	ag875x_CHK_PTR(lq_cmd_string, VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(lq_result, VI_ERROR_PARAMETER3);

	#ifdef NO_FORMAT_IO
		{
		ViUInt32 rtn_size;
		ViChar   lc[100];

		if ((errStatus = viWrite(vi, (ViBuf)lq_cmd_string,
									 strlen(lq_cmd_string), &rtn_size )) < VI_SUCCESS)
			return statusUpdate(vi,thisPtr,errStatus);

		if ((errStatus = delayRead(vi, thisPtr->myDelay,
									 (ViPBuf)lc, 100, &rtn_size)) < VI_SUCCESS)
			return statusUpdate(vi,thisPtr,errStatus);
		*lq_result = atol(lc);
		}
	#else
	if ((errStatus = viPrintf(vi, "%s\n",lq_cmd_string)) < VI_SUCCESS)
		return statusUpdate(vi,thisPtr,errStatus);

	if ((errStatus = delayScanf(vi, thisPtr->myDelay, "%ld%*t", lq_result)) < VI_SUCCESS)
		return statusUpdate(vi,thisPtr,errStatus);
	#endif	
       
	return statusUpdate( vi, thisPtr, errStatus );
	}        /* ----- end of function ----- */ 


/*****************************************************************************/
ViStatus _VI_FUNC ag875x_cmdReal(
        ViSession  vi,
        ViString   r_cmd_string,
        ViReal64   r_val )
/*****************************************************************************/
/*  Sends an instrument command which takes a single real parameter.         */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession  vi (in)                                                     */
/*      Instrument handle returned from ag875x_init()                        */
/*    ViString   r_cmd_string (in)                                           */
/*      The instrument command string to be sent to the instrument.          */
/*    ViReal64   r_val (in)                                                  */
/*      The integer sent to the instrument at the end of the instrument      */
/*      command.  Can be ViReal32 or ViReal64.                               */
/*****************************************************************************/

	{
	ViStatus errStatus = VI_SUCCESS;
	struct   ag875x_globals *thisPtr;
   
	GET_GLOBALS(vi,thisPtr);

	/*  Parameter checks */
	ag875x_CHK_PTR(r_cmd_string, VI_ERROR_PARAMETER2);

	#ifdef NO_FORMAT_IO
		{
		ViUInt32  rtn_size;
		ViChar    lc[300];

		sprintf(lc,"%s %g",r_cmd_string,r_val);
		if ((errStatus = viWrite(vi, (ViBuf)lc,
									   strlen(lc), &rtn_size )) < VI_SUCCESS)
			return statusUpdate(vi,thisPtr,errStatus);
		}
	#else
		if ((errStatus = viPrintf(vi, "%s %g\n",r_cmd_string,r_val )) < VI_SUCCESS)
			return statusUpdate(vi,thisPtr,errStatus);
	#endif

	return statusUpdate( vi, thisPtr, errStatus );
	}        /* ----- end of function ----- */


/*****************************************************************************/
ViStatus _VI_FUNC ag875x_cmdReal32_Q(
		ViSession  vi,
        ViString   rq_cmd_string,
        ViPReal32  rq_result)
/*****************************************************************************/
/*  Sends scpi command and waits for a response that must be representable   */
/*    as an real64.  A non numeric instrument response returns zero in       */
/*    result, in case of underflow undefined.                                */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession  vi (in)                                                     */
/*      Instrument handle returned from ag875x_init()                        */
/*    ViString   rq_cmd_string (in)                                          */
/*      The SCPI command string to be sent to the instrument.                */
/*    ViPReal32  rq_result (out)                                             */
/*      The real returned from the instrument.                               */
/*****************************************************************************/

	{   
	#ifdef NO_FORMAT_IO
	ViUInt32  rtn_size;
	#endif

	ViStatus  errStatus = VI_SUCCESS;
	struct    ag875x_globals *thisPtr;
   
	GET_GLOBALS(vi,thisPtr)

	/*  Parameter checks */
	ag875x_CHK_PTR(rq_cmd_string, VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(rq_result, VI_ERROR_PARAMETER3);

	#ifdef NO_FORMAT_IO
		{
		ViChar lc[100];
		if ((errStatus = viWrite(vi, (ViBuf)rq_cmd_string,
									 strlen(rq_cmd_string), &rtn_size )) < VI_SUCCESS)
			return statusUpdate(vi,thisPtr,errStatus);

		if ((errStatus = delayRead(vi, thisPtr->myDelay,
									 (ViPBuf)lc, 100, &rtn_size)) < VI_SUCCESS)
			return statusUpdate(vi,thisPtr,errStatus);
		*rq_result = (ViReal32)atof(lc);
		}
	#else
	if ((errStatus = viPrintf(vi, "%s\n",rq_cmd_string)) < VI_SUCCESS)
		return statusUpdate(vi,thisPtr,errStatus);

	if ((errStatus = delayScanf(vi, thisPtr->myDelay,
									 "%f%*t", rq_result)) < VI_SUCCESS)
		return statusUpdate(vi,thisPtr,errStatus);
	#endif	
        
	return statusUpdate( vi, thisPtr, errStatus );
	}        /* ----- end of function ----- */ 

/*****************************************************************************/
ViStatus _VI_FUNC ag875x_cmdReal64_Q(
    ViSession  vi,
    ViString   dq_cmd_string,
    ViPReal64  dq_result)
/*****************************************************************************/
/*  Sends scpi command and waits for a response that must be representable   */
/*    as an real64.  A non numeric instrument response returns zero in       */
/*    result, in case of underflow undefined.                                */
/*                                                                           */
/*  PARAMETERS                                                               */
/*    ViSession  vi (in)                                                     */
/*      Instrument handle returned from ag875x_init()                        */
/*    ViString   dq_cmd_string (in)                                          */
/*      The SCPI command string to be sent to the instrument.                */
/*    ViPReal64  dq_result (out)                                             */
/*      The real returned from the instrument.                               */
/*****************************************************************************/

	{   
	ViStatus  errStatus = VI_SUCCESS;
	struct    ag875x_globals *thisPtr;
   
	GET_GLOBALS(vi,thisPtr)

	/*  Parameter checks */
	ag875x_CHK_PTR(dq_cmd_string, VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(dq_result, VI_ERROR_PARAMETER3);

	#ifdef NO_FORMAT_IO
		{
		ViUInt32  rtn_size;
		 ViChar   lc[100];
		if ((errStatus = viWrite(vi, (ViBuf)dq_cmd_string,
									 strlen(dq_cmd_string), &rtn_size )) < VI_SUCCESS)
			return statusUpdate(vi,thisPtr,errStatus);

		if ((errStatus = delayRead(vi, thisPtr->myDelay,
									 (ViPBuf)lc, 100, &rtn_size)) < VI_SUCCESS)
			return statusUpdate(vi,thisPtr,errStatus);
		*dq_result = atof(lc);
		}
	#else
	if ((errStatus = viPrintf(vi, "%s\n",dq_cmd_string)) < VI_SUCCESS)
		return statusUpdate(vi,thisPtr,errStatus);

	if ((errStatus = delayScanf(vi, thisPtr->myDelay,
									 "%lg%*t", dq_result)) < VI_SUCCESS)
		return statusUpdate(vi,thisPtr,errStatus);
	#endif	
        
	return statusUpdate( vi, thisPtr, errStatus );
	}        /* ----- end of function ----- */ 


/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_analogPort
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : This function enables/disables the Analog Bus for Service Use.
 *           If the Analog Bus is enabled, the data at the selected auxiliary
 *           input (analog port) is measured and displayed.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViBoolean analogState
 * IN        
 *            This parameter specifies Analog Bus On/Off.
 * 
 * PARAM 3 : ViInt16 analogPortNum
 * IN        
 *            This parameter sets the Analog Port Number.
 * 
 *      MAX = ag875x_ANALOG_MAX   31
 *      MIN = ag875x_ANALOG_MIN   1
 * 
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
static const char * const ag875x_analogPort_analogState_a[] = {"OFF","ON",0};

ViStatus _VI_FUNC ag875x_analogPort(ViSession vi,
		ViBoolean analogState,
		ViInt16 analogPortNum)
	{
	ViStatus errStatus = VI_SUCCESS;
	struct ag875x_globals *thisPtr;

	errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
	if (errStatus < VI_SUCCESS)
		{
		   ag875x_LOG_STATUS( vi, 0, errStatus );
		}

	ag875x_DEBUG_CHK_THIS( vi, thisPtr );
	ag875x_CDE_INIT( "ag875x_analogPort" );

	/* Parameter checks */
	ag875x_CHK_BOOLEAN(analogState,VI_ERROR_PARAMETER2);

	ag875x_CHK_INT_RANGE(analogPortNum
							,ag875x_ANALOG_MIN
							,ag875x_ANALOG_MAX
							,VI_ERROR_PARAMETER3);

		{
    	if (analogState == ag875x_ANLOG_PORT_ON)
    		{
    		errStatus = viPrintf(vi,"ANABON\n");
        	if (errStatus < VI_SUCCESS)
        		{
    			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
    			}
    		
        	errStatus = viPrintf(vi,"ANAI %d\n",analogPortNum);
        	if (errStatus < VI_SUCCESS)
        		{
    			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
        		}
    		}
    
     		else if (analogState==ag875x_ANLOG_PORT_OFF)
    			{
    			errStatus = viPrintf(vi,"ANABOFF\n");
        		if (errStatus < VI_SUCCESS)
        			{
    				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
        			}
    			}
		}
		ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_autoscale
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function performs an AutoScale on the selected channel.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_autoscale(ViSession vi)
	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		{
       ag875x_LOG_STATUS( vi, 0, errStatus );
		}

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_autoscale" );

		{
		errStatus = viPrintf(vi,"AUTO\n");
		if (errStatus < VI_SUCCESS)
			{
		   ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}

		}
    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */


/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_calCorrection
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function sets Calibration Error Correction ON or OFF.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViBoolean calibState
 * IN        
 *            Specify on/off which sets calibration correction.
 * 
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
static const char * const ag875x_calCorrection_calibState_a[] = {"OFF","ON",0};

ViStatus _VI_FUNC ag875x_calCorrection(ViSession vi,
	ViBoolean calibState)

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		{
		ag875x_LOG_STATUS( vi, 0, errStatus );
		}

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_calCorrection" );

	/*  Parameter checks */
    ag875x_CHK_BOOLEAN(calibState,VI_ERROR_PARAMETER2);

		{
		errStatus = viPrintf(vi,"MENUCAL\n");
		// Added needed delay
		doDelay(DELAY_TIME);
		if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}

		errStatus = viPrintf(vi,"CORR%s\n",ag875x_calCorrection_calibState_a[calibState]);
		// Added needed delay
		doDelay(DELAY_TIME);
		if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}
		}

    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_channelSelect
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : This function selects which channel (Channel 1, 2, 3 or 4) is the 
 *           active channel. Channels 3 and 4 are valid only on 8753/8719/8720/8722
 *           Firmware Revisions 7.40 and higher.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViInt16 channel
 * IN        
 *            Select the Channel Number.
 * 
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_channelSelect(ViSession vi,
	ViInt16 channel)

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;
	char functionName[256] = {0};	// Used for error reporting.

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		{
		ag875x_LOG_STATUS( vi, 0, errStatus );
		}

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_channelSelect" );

	// Used for error reporting
	sprintf(functionName, "ag875x_channelSelect");

	/*  Parameter checks */
    ag875x_CHK_INT_RANGE(channel,ag875x_CH1,ag875x_CH4,VI_ERROR_PARAMETER2);

		{
		ViInt32 primary_chan = 0;
		ViInt32 cur_chan = 0;
		ViInt32 iReply = 0;
		ViChar cur_ID[20], message[ag875x_ERR_MSG_LENGTH], calType[50];
		ViReal64 dFirmware = 0.0;

		if ((errStatus = ag875x_readID(vi, cur_ID, "REV_VALUE?", &dFirmware)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		// Channel enums start at 0, so increment by one for the instrument.
		cur_chan = channel +1;

		if (channel > ag875x_CH2) // If selected channel number is 3 or 4
		{
			// Channels 3 and 4 not supported on Firmware Revs lower than 7.40
			if (compFirmwareVersions(dFirmware, "<", 7.40))
				{
				sprintf(message, "Channel = `%i';  Function = %s", cur_chan, functionName);
				strcpy(thisPtr->errMessage, message);
				ag875x_LOG_STATUS( vi, thisPtr, ag875x_REQUIRES_4_PARM_FW );
				}

			// If < 7.60, must be 7.40 or 7.48, which require 2-port cal
			if (compFirmwareVersions(dFirmware, "<", 7.60))
				{
				// Can only use channels 1 or 2 when addressing the instrument.
				primary_chan = cur_chan - 2;
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				if ((errStatus = viQueryf(vi, "OPC?;CHAN%i;\n","%i",primary_chan,&iReply)) < VI_SUCCESS)
					ag875x_LOG_STATUS(vi, thisPtr, errStatus);
				// CalType_Q expects enums 0-3 not 1-4, so decrement primary_chan by 1
				if ((errStatus = ag875x_calType_Q(vi,(ViInt16)(primary_chan - 1),calType)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			
				if (!((!abs(strcmp(calType, ag875x_CAL_FULL_2_PORT)))||
					(!abs(strcmp(calType, ag875x_CAL_1_PATH_2_PORT)))||
					(!abs(strcmp(calType, ag875x_CAL_TRL_LRM)))))
					{
					sprintf(message, "Function = %s", functionName);
					strcpy(thisPtr->errMessage, message);
					ag875x_LOG_STATUS( vi, thisPtr, ag875x_REQUIRES_2_PORT_CAL );
					}
				}
       
		   if ((errStatus = viPrintf(vi,"AUXCON;\n")) < VI_SUCCESS)
			  ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			  
		} // end if channel  > ag875x_CH2
		
		// Change to the new channel
		if ((errStatus = viQueryf(vi, "OPC?;CHAN%i;\n","%i",cur_chan,&iReply)) < VI_SUCCESS)
			ag875x_LOG_STATUS(vi, thisPtr, errStatus);

		// Update the globals with the new channel.
		
		if ((errStatus = ag875x_readID(vi, cur_ID, "MODEL?", 0)) < VI_SUCCESS)
		   ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if (!abs(strcmp(cur_ID, "8753D")))
			{
			if (compFirmwareVersions(dFirmware, ">=", 6.12))
				{
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				if ((errStatus = viQueryf(vi,"OUTPCHAN;\n","%i",&iReply)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				thisPtr->cur_chan = iReply;
				}
			else
				{
				thisPtr->cur_chan = cur_chan;
				}
			}
		else
			{
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viQueryf(vi,"OUTPCHAN;\n","%i",&iReply)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			thisPtr->cur_chan = iReply;
			}

		// Flush the buffer before leaving.
		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		}  
    
	ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_displaySelect
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function is for selection of the various parameters
 *            related to the channel display.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViInt16 displaySelectChannel
 * IN        
 *            Selects the channel number.  Channels 3 and 4 are valid only on 
 *            8753/8719/8720/8722 Firmware Revisions 7.40 and higher.
 * 
 * PARAM 3 : ViInt16 displaySelectMode
 * IN        
 *            This commands display the indicated combination of the data and
 *            trace memory on the active Channel.
 * 
 * PARAM 4 : ViInt16 displaySelectFormat
 * IN        
 *            Set the Display Format such as Log Magnitude, Phase, Delay,
 *            Smith Chart, Polar, Linear Magnitude, Real, Imaginary, SWR for
 *            the selected channel & selected display mode.
 * 
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
static const char * const ag875x_displaySelect_dispMode_a[] = {"DISPDATA",
        "DISPMEMO","DISPDATM","DISPDDM","DISPDMM","DATI",0};

static const char * const ag875x_displaySelect_dispFormat_a[] = {"LOGM",
        "PHAS","DELA","SMIC","POLA","LINM","REAL","IMAG","SWR",0};

ViStatus _VI_FUNC ag875x_displaySelect(ViSession vi,
	ViInt16 displaySelectChannel,
	ViInt16 displaySelectMode,
	ViInt16 displaySelectFormat)

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		{
		ag875x_LOG_STATUS( vi, 0, errStatus );
		}

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_displaySelect" );

	/*  Parameter checks */
    ag875x_CHK_INT_RANGE(displaySelectChannel,ag875x_CH1,ag875x_CH4,VI_ERROR_PARAMETER2);
    ag875x_CHK_ENUM(displaySelectMode,5,VI_ERROR_PARAMETER3);
    ag875x_CHK_ENUM(displaySelectFormat,8,VI_ERROR_PARAMETER4);

		{
		ViInt32 currentChannel = 0;
		
		// Save the current active channel number, so it may be restored later.
		if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	
		// Temporarily change to requested channel.
		if ((errStatus = ag875x_channelSelect(vi, displaySelectChannel)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		// Change display.
		if ((errStatus = viPrintf(vi,"MENUDISP\n")) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
    
		if ((errStatus = viPrintf(vi,"%s\n",ag875x_displaySelect_dispMode_a[displaySelectMode])) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
    
		if ((errStatus = viPrintf(vi,"MENUFORM\n")) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
    
		if ((errStatus = viPrintf(vi,"%s\n",ag875x_displaySelect_dispFormat_a[displaySelectFormat])) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
    
		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			
		// Restore instrument to original channel number.
		if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
			ag875x_LOG_STATUS(vi, thisPtr, errStatus);
		}

    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_dualSplit
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : This function is for selection of Dual Channel Display and/or
 *           Split Channel Display modes.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViBoolean dualChannelState
 * IN        
 *            Select the Dual Channel On/Off
 * 
 * PARAM 3 : ViBoolean splitChannelMode
 * IN        
 *            Select the Split Channel On/Off
 * 
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
static const char * const ag875x_dualSplit_dualState_a[] = {"OFF","ON",0};
static const char * const ag875x_dualSplit_splitState_a[] = {"OFF","ON",0};

ViStatus _VI_FUNC ag875x_dualSplit(ViSession vi,
	ViBoolean dualChannelState,
	ViBoolean splitChannelMode)

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		{
		ag875x_LOG_STATUS( vi, 0, errStatus );
		}

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_dualSplit" );

	/*  Parameter checks */
    ag875x_CHK_BOOLEAN(dualChannelState,VI_ERROR_PARAMETER2);
    ag875x_CHK_BOOLEAN(splitChannelMode,VI_ERROR_PARAMETER3);

		{
		errStatus = viPrintf(vi,"MENUDISP\n");
		if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}

		errStatus = viPrintf(vi,"DUAC%s\n",ag875x_dualSplit_dualState_a[dualChannelState]);
		if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}

		errStatus = viPrintf(vi,"SPLD %s\n",ag875x_dualSplit_splitState_a[splitChannelMode]);
		if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}
		}

    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_esb_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : 
 *           Returns the contents of Event Status Register B.
 *           The driver returns the equivalent messages:
 *               Bit Value        Message
 *           
 *                   0           "ESB_EMPTY"
 *                   1           "ESB_SNC"
 *                   2           "ESB_SRW"
 *                   4           "ESB_DEC"
 *                   8           "ESB_LF2"
 *                  16           "ESB_LF1"
 *                  32           "ESB_SF2"
 *                  64           "ESB_SF1"
 *                 128           "ESB_CPC"
 *
 *           For 8753/8719/8720/8722 analyzers having Firmware Revision 7.40
 *           or higher, these additional bits are represented:
 *               Bit Value        Message
 *           
 *                 256           "ESB_LF4"
 *                 512           "ESB_LF3"
 *                1024           "ESB_SF4"
 *                2048           "ESB_SF3"
 *           
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViChar _VI_FAR statstr[]
 * OUT       
 *            Response from instrument
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "ag875x_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_esb_Q(ViSession vi,
	ViChar _VI_FAR statstr[])

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		{
		ag875x_LOG_STATUS( vi, 0, errStatus );
		}

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_esb_Q" );

	/*  Parameter checks */
	ag875x_CHK_PTR(statstr, VI_ERROR_PARAMETER2);

		{     
		ViInt16 esbval = 0;
		size_t len = 0;
			
		ViChar lc[100];
		ViUInt32 rtn_size;

			if ((errStatus = viWrite(vi, (ViBuf)"ESB?;", 5, &rtn_size )) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			if ((errStatus = delayRead(vi, thisPtr->myDelay,
									 (ViPBuf)lc, 100, &rtn_size)) < VI_SUCCESS)
			   ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			esbval = atoi(lc);

			/*errStatus = viPrintf(vi,"ESB?\n");
			if (errStatus < VI_SUCCESS)
				{
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				}
    
			errStatus = viScanf(vi,"%d",&esbval);
			if (errStatus < VI_SUCCESS)
				{
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				}
			*/

			strcpy(statstr, "");

			// Check if the event status register b is empty.
			if (esbval == 0)
			{
				strcat(statstr, "ESB_EMPTY");
				ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
			}

			// There may be more than one of the 8 esb bits turned on,
			// indicating more than one event.  The following will
			// concat all events into a single output string.

			if (esbval > esbval % 2048)
				{
				esbval = esbval % 2048;
				strcat(statstr, "ESB_SF3, ");
				}

			if (esbval > esbval % 1024)
				{
				esbval = esbval % 1024;
				strcat(statstr, "ESB_SF4, ");
				}
			
			if (esbval > esbval % 512)
				{
				esbval = esbval % 512;
				strcat(statstr, "ESB_LF3, ");
				}
			
			if (esbval > esbval % 256)
				{
				esbval = esbval % 256;
				strcat(statstr, "ESB_LF4, ");
				}
			
			if (esbval > esbval % 128)
				{
				esbval = esbval % 128;
				strcat(statstr, "ESB_CPC, ");
				}

			if (esbval > esbval % 64)
				{
				esbval = esbval % 64;
				strcat(statstr, "ESB_SF1, ");
				}
			
			if (esbval > esbval % 32)
				{
				esbval = esbval % 32;
				strcat(statstr, "ESB_SF2, ");
				}
			
			if (esbval > esbval % 16)
				{
				esbval = esbval % 16;
				strcat(statstr, "ESB_LF1, ");
				}
			
			if (esbval > esbval % 8)
				{
				esbval = esbval % 8;
				strcat(statstr, "ESB_LF2, ");
				}
			
			if (esbval > esbval % 4)
				{
				esbval = esbval % 4;
				strcat(statstr, "ESB_DEC, ");
				}
			
			if (esbval > esbval % 2)
				{
				esbval = esbval % 2;
				strcat(statstr, "ESB_SRW, ");
				}
			
			if (esbval == 1)
				{
				strcat(statstr, "ESB_SNC, ");
				}

			len = strlen(statstr);
			statstr[len - 2] = '\0';
			
		  }

    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_esr_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : 
 *           Returns the contents of the Event Status Register.
 *           The driver returns the equivalent messages:
 *               Bit Value        Message
 *           
 *                   0           "ESR_EMPTY"
 *                   1           "ESR_OPC"
 *                   2           "ESR_RQL"
 *                   4           "ESR_QYE"
 *                   8           "ESR_SQB"
 *                  16           "ESR_EXE"
 *                  32           "ESR_CME"
 *                  64           "ESR_URQ"
 *                 128           "ESR_PON"
 *           
 *           
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 *
 * PARAM 2 : ViChar _VI_FAR errstr[]
 * OUT       
 *            Response from instrument.
 * 
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "ag875x_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_esr_Q(ViSession vi,
	ViChar _VI_FAR errstr[])

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		{
		ag875x_LOG_STATUS( vi, 0, errStatus );
		}

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_esr_Q" );

	/*  Parameter checks */
	ag875x_CHK_PTR(errstr, VI_ERROR_PARAMETER2);

		{     
		ViInt16 esrval = 0;
		size_t len = 0;
			
		ViChar lc[100];
		ViUInt32 rtn_size = 0;

		if ((errStatus = viWrite(vi, (ViBuf)"ESR?;",
									 5, &rtn_size )) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if ((errStatus = delayRead(vi, thisPtr->myDelay,
									 (ViPBuf)lc, 100, &rtn_size)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		esrval = atoi(lc);

		/*
		errStatus = viPrintf(vi,"ESR?\n");
		if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}

		errStatus = viScanf(vi,"%d",&esrval);
		if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}
		*/

		strcpy(errstr, "");

		// Check if the event status register is empty.
		if (esrval == 0)
		{
			strcat(errstr, "ESR_EMPTY");
			ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
		}

		// There may be more than one of the 8 esr bits turned on,
		// indicating more than one event.  The following will
		// concat all events into a single output string.

		if (esrval > esrval % 128)
			{
			esrval = esrval % 128;
			strcat(errstr, "ESR_PON, ");
			}

		if (esrval > esrval % 64)
			{
			esrval = esrval % 64;
			errstr="ESR_URQ, ";
			}
			
		if (esrval > esrval % 32)
			{
			esrval = esrval % 32;
			strcat(errstr, "ESR_CME, ");
			}
			
		if (esrval > esrval % 16)
			{
			esrval = esrval % 16;
			strcat(errstr, "ESR_EXE, ");
			}
			
		if (esrval > esrval % 8)
			{
			esrval = esrval % 8;
			strcat(errstr, "ESR_SQB, ");
			}
			
		if (esrval > esrval % 4)
			{
			esrval = esrval % 4;
			strcat(errstr, "ESR_QYE, ");
			}
			
		if (esrval > esrval % 2)
			{
			esrval = esrval % 2;
			strcat(errstr, "ESR_RQL, ");
			}
			
		if (esrval == 1)
			{
			strcat(errstr, "ESR_OPC, ");
			}

		len = strlen(errstr);
		errstr[len - 2] = '\0';
			
		}

    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */


/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_frequency
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function is for setting or reading of frequency stimulus 
 *            parameters, EXCEPT in the case of a frequency list sweep.
 *            To set up or read a frequency list table, use the 
 *            ag875x_freqListSet function.
 * 
 *            Before calling this function, use the ag875x_freqSweepType 
 *            function to activate the desired type of sweep.
 *
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViString frequencyRequest
 * IN        
 *            Selects desired action; see code below for choices.  
 * 
 * PARAM 3 : ViReal64 setStartOrCenter
 * IN        
 *            Depending upon 'request', sets the Start or Center frequency value 
 *            in Hz.  If analyzer is in CW Time mode, this parameter will set
 *            the CW frequency in Hz.
 * 
 * PARAM 4 : ViReal64 setStopOrSpan
 * IN        
 *            Depending upon 'request', sets the Stop frequency or frequency Span
 *            value in Hz.  If analyzer is in CW Time mode, this parameter will 
 *            set the sweep time in Seconds (a value of 0 here will place the
 *            analyzer in auto sweep time mode).
 * 
 * PARAM 5 : ViReal64 getStartOrCenter
 * OUT        
 *            Depending upon 'request', returns the Start or Center frequency  
 *            value in Hz.  If analyzer is in CW Time mode, this parameter will 
 *            return the CW frequency in Hz.
 * 
 * PARAM 6 : ViReal64 getStopOrSpan
 * OUT        
 *            Depending upon 'request', returns the Stop frequency or frequency 
 *            Span value in Hz.  If analyzer is in CW Time mode, this parameter  
 *            will return the sweep time in Seconds.
 * 
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_frequency(ViSession vi,
	ViString frequencyRequest,
	ViReal64 setStartOrCenter,
	ViReal64 setStopOrSpan,
	ViPReal64 getStartOrCenter,
	ViPReal64 getStopOrSpan)

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;
	ViInt32 choice = 0;
	ViInt32 iCW = 0;

	const ViInt32 size = 12;
    ViString req[]= { "S/S", "START/STOP", "START_STOP", 
						  "C/S", "CENTER/SPAN", "CENTER_SPAN",
		                  "S/S?", "START/STOP?", "START_STOP?", 
	                      "C/S?", "CENTER/SPAN?", "CENTER_SPAN?"}; 

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		{
		ag875x_LOG_STATUS( vi, 0, errStatus );
		}

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_frequency" );

	/*  Parameter checks */
	ag875x_CHK_PTR(frequencyRequest, VI_ERROR_PARAMETER2);
  
    if(getChoice(vi,frequencyRequest,size,req,&choice)) 
	    return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

		{
        if ((errStatus = viPrintf(vi,"MENUSTIM;\n")) < VI_SUCCESS)
            ag875x_LOG_STATUS( vi, thisPtr, errStatus );
    
        if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
            ag875x_LOG_STATUS( vi, thisPtr, errStatus );

        if ((errStatus = viQueryf(vi,"CWTIME?;\n","%i",&iCW)) < VI_SUCCESS)
            ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if ((iCW)&&(choice < 6))
			{
            if ((errStatus = viPrintf(vi,"CWFREQ %g;\n",setStartOrCenter)) < VI_SUCCESS)
			    ag875x_LOG_STATUS( vi, thisPtr, errStatus );

            if (setStopOrSpan != 0)
				{
			    if ((errStatus = viPrintf(vi,"SWET %g;\n",setStopOrSpan)) < VI_SUCCESS)
	                ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				}
            else
				{
			    if ((errStatus = viPrintf(vi,"SWEA;\n")) < VI_SUCCESS)
	                ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				}

            ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
   			}
		else if (iCW) /* &&(choice >= 6) */
			{            
            if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
	            ag875x_LOG_STATUS( vi, thisPtr, errStatus );

            if ((errStatus = viQueryf(vi,"CWFREQ?;\n","%lf",getStartOrCenter)) < VI_SUCCESS)
			    ag875x_LOG_STATUS( vi, thisPtr, errStatus );

            if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
	            ag875x_LOG_STATUS( vi, thisPtr, errStatus );

            if ((errStatus = viQueryf(vi,"SWET?;\n","%lf",getStopOrSpan)) < VI_SUCCESS)
			    ag875x_LOG_STATUS( vi, thisPtr, errStatus );

            ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
			}

    	switch (choice)
			{ 
			case 0: case 1: case 2: // Set Start and Stop Frequencies
    			{
                if ((errStatus = viPrintf(vi,"STAR %g;\n",setStartOrCenter)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if ((errStatus = viPrintf(vi,"STOP %g;\n",setStopOrSpan)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
    			}
    			break;
         
			case 3: case 4: case 5: // Set Center Frequency and Span 
				{
				if ((errStatus = viPrintf(vi,"CENT %g;\n",setStartOrCenter)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if ((errStatus = viPrintf(vi,"SPAN %g;\n",setStopOrSpan)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
       			}
				break;       
                
			case 6: case 7: case 8: // Query Start and Stop Frequencies
				{
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if ((errStatus = viQueryf(vi,"STAR?;\n","%lf",getStartOrCenter)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if ((errStatus = viQueryf(vi,"STOP?;\n","%lf",getStopOrSpan)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
    			}
    			break;
         
			case 9: case 10: case 11: // Query Center Frequency and Span
				{
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if ((errStatus = viQueryf(vi,"CENT?;\n","%lf",getStartOrCenter)) < VI_SUCCESS)
					 ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if ((errStatus = viQueryf(vi,"SPAN?;\n","%lf",getStopOrSpan)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
    			}
    			break;
			}   /* end switch */
		
		// Flush the buffer just in case
		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		}

    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_loadOrPurgeFromDisk
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : This Function is used to load an instrument state (and any data 
 *           which may have been saved with it) into the Instrument from a file
 *           on a floppy disk in the internal drive, or to purge such a file 
 *           from the disk.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViInt16 loadOrPurgeMode
 * IN        
 *            This parameter selects the task to be performed by this function 
 *            (LOAD or PURGE).
 * 
 * PARAM 3 : ViChar _VI_FAR loadOrPurgeFileTitle[]
 * IN        
 *            Title of the instrument state file to be loaded or purged from 
 *            the disk.
 * 
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_loadOrPurgeFromDisk(ViSession vi,
	ViInt16 loadOrPurgeMode, 
	ViChar _VI_FAR loadOrPurgeFileTitle[])

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
       ag875x_LOG_STATUS( vi, 0, errStatus );

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_loadOrPurgeFromDisk" );

	/*  Parameter checks */
	ag875x_CHK_ENUM(loadOrPurgeMode, 1, VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(loadOrPurgeFileTitle, VI_ERROR_PARAMETER3);

		{
		ViChar buff[80];

        sprintf (buff, "INTD;TITF1\"%s\";", loadOrPurgeFileTitle); 
    
        errStatus = viPrintf(vi, "%s\n", buff);
    	if (errStatus < VI_SUCCESS)
    		ag875x_LOG_STATUS(vi, thisPtr, errStatus);
    	
		if (loadOrPurgeMode == ag875x_LP_LOAD_FILE)
			{
			errStatus = viPrintf(vi, "LOAD1;\n");
    		if (errStatus < VI_SUCCESS)
    			ag875x_LOG_STATUS(vi, thisPtr, errStatus);
			}
		else
			{
			errStatus = viPrintf(vi, "PURG1;\n");
    		if (errStatus < VI_SUCCESS)
    			ag875x_LOG_STATUS(vi, thisPtr, errStatus);
			}

		if ((errStatus = ag875x_outputf(vi, 3, "NOOP;")) < VI_SUCCESS)
    		ag875x_LOG_STATUS(vi, thisPtr, errStatus);
		}

    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_measType
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : This function specifies the type of measurement for the active 
 *           channel and allows selection of data conversion.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViInt16 inputSParam
 * IN        
 *            This parameter specifies whether the measurement type for the 
 *            active channel is an Input port, Input port ratio, Reflection,
 *            Transmission or S-Parameter.  Input Ports are A, B and R. Input 
 *            Port Ratios are A/R, B/R and A/B.  Reflection and Transmission apply
 *            to the non S-parameter instruments (8719ET/8720ET/8722ET/8753ET).  
 *            Reflection is essentially the same measurement as A/R, but is 
 *            handled as a separate parameter.  Transmission is essentially the 
 *            same measurement as B/R, but is handled as a separate parameter.  
 *            The 8719D/ES, 8720D/ES, 8722D/ES and 8753D/E/ES are S-Parameter
 *            capable.  S-Parameters are S11, S21, S12 and S22.  These 
 *            S-Parameter instruments will interpret Reflection as S11 and
 *            Transmission as S21.  Conversely, the 8719ET/8720ET/8722ET/8753ET
 *            analyzers will interpret S11 as Reflection and S21 as Transmission.
 *            
 * 
 * PARAM 3 : ViInt16 conversionParam
 * IN        
 *            This parameter allows conversion of measured data into equivalent 
 *            impedance or admittance values, or inverse S-parameter values.
 * 
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
static const char * const ag875x_measType_inputSparam_a[] = {"AR","BR",
        "AB","MEASA","MEASB","MEASR","S11","S21","S12","S22","TRAP","RFLP",0};

static const char * const ag875x_measType_cnvtParam_a[] = {"CONVOFF",
        "CONVZREF","CONVZTRA","CONVYREF","CONVYTRA","CONV1DS",0};

ViStatus _VI_FUNC ag875x_measType(ViSession vi,
	ViInt16 inputSParam,
	ViInt16 conversionParam)

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;
    char cur_ID[15], message[ag875x_ERR_MSG_LENGTH];
	char functionName[256] = {0};	// Used for error reporting.

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		{
		ag875x_LOG_STATUS( vi, 0, errStatus );
		}

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_measType" );
	// Used for error reporting
	sprintf(functionName, "ag875x_measType");

	/*  Parameter checks */
    ag875x_CHK_ENUM(inputSParam,11,VI_ERROR_PARAMETER2);
    ag875x_CHK_ENUM(conversionParam,5,VI_ERROR_PARAMETER3);

		{
		if ((inputSParam == ag875x_S_PAR12) || (inputSParam == ag875x_S_PAR22))
			{
			if ((errStatus = ag875x_readID(vi, cur_ID, "MODEL?", 0)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			if (strstr(cur_ID, "ET") != NULL)
				{
				sprintf(message, "Function = %s", functionName);
				strcpy(thisPtr->errMessage, message);
				ag875x_LOG_STATUS( vi, thisPtr, ag875x_REQUIRES_S_PARM_ANALYZER );
				}
			}

		errStatus = viPrintf(vi,"MENUMEAS\n");
		if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}

		// Workaround for firmware 7.64 bug for command "TRAP"
		if (inputSParam == ag875x_TRANS)
		{
			inputSParam = ag875x_S_PAR21;
		}

		errStatus = viPrintf(vi,"%s\n",ag875x_measType_inputSparam_a[inputSParam]);
		if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}
  
		errStatus = viPrintf(vi,"%s\n",ag875x_measType_cnvtParam_a[conversionParam]);
		if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}
		}

    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_opc_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  Sends "OPC?;" followed by the user-supplied OPC-compatible command  
 *            to the instrument and returns VI_TRUE when the operation initiated 
 *            by the OPC-compatible command has completed.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViString oqCmdString
 * IN        
 *            OPC-compatible command to be sent to the instrument.
 * 
 * PARAM 3 : ViPBoolean instrReady
 * OUT        
 *            Returns VI_TRUE when the OPC-compatible operation has completed.
 * 
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_opc_Q(ViSession   vi,
	ViString    oqCmdString,
    ViPBoolean  instrReady)

	{
	ViStatus errStatus = VI_SUCCESS;
	struct ag875x_globals *thisPtr;
   
    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
       ag875x_LOG_STATUS( vi, 0, errStatus );

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_opc_Q" );
	
	/*  Parameter checks */
	ag875x_CHK_PTR(oqCmdString, VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(instrReady, VI_ERROR_PARAMETER3);

		{
		if ((errStatus = viPrintf(vi,"OPC?;%s\n", oqCmdString)) < VI_SUCCESS)
			ag875x_LOG_STATUS(vi,thisPtr,errStatus);

		if ((errStatus = delayScanf(vi, thisPtr->myDelay,
                                          "%d%*t", instrReady)) < VI_SUCCESS)
			ag875x_LOG_STATUS(vi,thisPtr,errStatus);
		}

	ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS);
	}         /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_portAttenuation
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : In the case of an Agilent 8753D/8753E/8753ES Option 011 or 
 *           Agilent 8719D/ES, 8720D/ES or 8722D/ES Option 085 Network Analyzer,
 *           this function sets the required attenuation at the selected test port.
 *
 *           Attenuation   Compatibility
 *           -----------   -------------
 *
 *           ATTEN_0_DB    Both
 *           ATTEN_5_DB    Agilent 8719/20/22 Option 085
 *           ATTEN_10_DB   Both
 *           ATTEN_15_DB   Agilent 8719/20/22 Option 085
 *           ATTEN_20_DB   Both
 *           ATTEN_25_DB   Agilent 8719/20/22 Option 085
 *           ATTEN_30_DB   Both
 *           ATTEN_35_DB   Agilent 8719/20/22 Option 085
 *           ATTEN_40_DB   Both
 *           ATTEN_45_DB   Agilent 8719/20/22 Option 085
 *           ATTEN_50_DB   Both
 *           ATTEN_60_DB   Agilent 8753D/E/ES Option 011
 *           ATTEN_70_DB   Agilent 8753D/E/ES Option 011
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViInt16 portAttenuationPortNum
 * IN        
 *            Select the port number for attenuation selection.
 * 
 * PARAM 3 : ViInt16 attenuation
 * IN        
 *            Set attenuation in dB for the selected port.
 * 
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_portAttenuation(ViSession vi,
  ViInt16 portAttenuationPortNum,
  ViInt16 attenuation)

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;
    ViChar cur_ID[15];
	ViReal64 dOpt011 = 0.0;
	ViReal64 dOpt085 = 0.0;
	ViInt16 attenuationValue = 0;

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		{
		ag875x_LOG_STATUS( vi, 0, errStatus );
		}

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_portAttenuation" );

	/*  Parameter checks */
    ag875x_CHK_ENUM(portAttenuationPortNum,1,VI_ERROR_PARAMETER2);

	// Get attenuation value
	switch (attenuation)
	{
		case ag875x_ATTEN_00:
			attenuationValue = 0;
			break;
		case ag875x_ATTEN_05:
			attenuationValue = 5;
			break;
		case ag875x_ATTEN_10:
			attenuationValue = 10;
			break;
		case ag875x_ATTEN_15:
			attenuationValue = 15;
			break;
		case ag875x_ATTEN_20:
			attenuationValue = 20;
			break;
		case ag875x_ATTEN_25:
			attenuationValue = 25;
			break;
		case ag875x_ATTEN_30:
			attenuationValue = 30;
			break;
		case ag875x_ATTEN_35:
			attenuationValue = 35;
			break;
		case ag875x_ATTEN_40:
			attenuationValue = 40;
			break;
		case ag875x_ATTEN_45:
			attenuationValue = 45;
			break;
		case ag875x_ATTEN_50:
			attenuationValue = 50;
			break;
		case ag875x_ATTEN_60:
			attenuationValue = 60;
			break;
		case ag875x_ATTEN_70:
			attenuationValue = 70;
			break;
	}  // end switch(atten)


		// Check for valid instrument options.
		{
		if ((errStatus = ag875x_readID(vi, cur_ID, "011?", &dOpt011)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		if ((errStatus = ag875x_readID(vi, cur_ID, "085?", &dOpt085)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		if (!(int)(dOpt011) && !(int)(dOpt085))
			ag875x_LOG_STATUS( vi, thisPtr, ag875x_OPTION_REQUIRED );
		if ((errStatus = ag875x_readID(vi, cur_ID, "MODEL?", 0)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		if (strstr(cur_ID, "8753") != NULL) 
		
			{
    		switch (portAttenuationPortNum)
    			{ 
    			case ag875x_PORT_NUM1: 
    				errStatus = viPrintf(vi,"ATTP1 %d\n",attenuationValue);
    				if (errStatus < VI_SUCCESS)
           				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
             		break;
    		
        		case ag875x_PORT_NUM2: 
    			    errStatus = viPrintf(vi,"ATTP2 %d\n",attenuationValue);
    				if (errStatus < VI_SUCCESS)
           				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
					break; 
                      
         		default: 
					break;
    			}   
			}
		else 
			{
    		switch (portAttenuationPortNum)
    			{ 
    			case ag875x_PORT_NUM1: 
    				errStatus = viPrintf(vi,"ATTA %d\n",attenuationValue);
    				if (errStatus < VI_SUCCESS)
           				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
             		break;
    		
        		case ag875x_PORT_NUM2: 
    			    errStatus = viPrintf(vi,"ATTB %d\n",attenuationValue);
    				if (errStatus < VI_SUCCESS)
           				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
					break; 
                      
         		default: 
					break;
    			}   
			}
		}

    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_readTraceOrCalDataDos
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : This function reads (gets) trace data or calibration data from the
 *           instrument in "FORM5" PC DOS format (not compatible with UNIX 
 *           Platform). 
 *           'readTraceOrCalType' selects the type of data array to be read.
 *           'readTraceOrCalArray' is Output Real 64 bit one-dimensional array . 
 *           (Internally to the function this 'readTraceOrCalArray' is 32 bit Real 
 *           array. Hence the 'readTraceOrCalArray' values will be of 32 bit resolution.)
 *           'readTraceOrCalArrayNum' is used to select the number of the calibration 
 *           array or raw data array to be read.
 *           'readTraceOrCalPoints' is used for selecting the number of points on sweep.
 * 
 *           With this function, as with readArrayDataAscii, an error will 
 *           occur if the 64 bit real array passed to the function is not of 
 *           adequate size to accommodate all the data values output by the 
 *           instrument.
 *
 * PARAM 1 : ViSession vi
 * IN        
 *           Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViInt16 readTraceOrCalType
 * IN        
 *           'readTraceOrCalType' selects the type of data to be read from the 
 *           instrument.
 * 
 * PARAM 3 : ViReal64 _VI_FAR readTraceOrCalArray[]
 * OUT       
 *           64 bit Real one-dimensional array to receive the data values.
 * 
 * PARAM 4 : ViInt16 readTraceOrCalArrayNum
 * IN        
 *           The number of the calibration array or raw data array to be
 *           output from the instrument.  For DATA, FORMatted data and
 *           MEMOry data, 'readTraceOrCalArrayNum' is simply a placeholder (not used).
 *
 *           For ag875x_CALCnn_OUTPUT or ag875x_ICALnn_OUTPUT (cal arrays):
 * 
 *				MAX = ag875x_ARRAY_MAX_ET = 6       (8753ET/8719ET/8720ET/8722ET analyzers)
 *				MAX = ag875x_ARRAY_MAX_S_PARM = 12  (All S-parameter 8753/8719/8720/8722 analyzers)
 *				MIN = ag875x_ARRAY_MIN = 1
 * 
 *           For ag875x_RAWn_OUTPUT or ag875x_RAFn_OUTPUT (raw data arrays):
 * 
 *				MAX = ag875x_RAW_MAX_ET = 1      (8753ET/8719ET/8720ET/8722ET analyzers)
 *				MAX = ag875x_RAW_MAX_S_PARM = 4  (All S-parameter 8753/8719/8720/8722 analyzers)
 *				MIN = ag875x_ARRAY_MIN = 1
 * 
 *           For ag875x_PREn_OUTPUT ("pre-raw" data arrays):
 * 
 *				MAX = ag875x_RAW_MAX_S_PARM = 4
 *				MIN = ag875x_ARRAY_MIN = 1
 *           Pre-raw arrays are only supported on Firmware Revisions 6.12 and higher, and are
 *           not supported on 8753ET/8719ET/8720ET/8722ET analyzers.
 * 
 * RETURN  : VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *           To determine error message, pass the return value to routine
 *           "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
static const char * const ag875x_readTraceOrCalDataDos_outputType_a[] = {"","",
        "","","","","","","","","","","",0};

ViStatus _VI_FUNC ag875x_readTraceOrCalDataDos(ViSession vi,
	ViInt16 readTraceOrCalType,
	ViReal64 _VI_FAR readTraceOrCalArray[],
	ViInt16 readTraceOrCalArrayNum)

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;
	char functionName[256] = {0};	// Used for error reporting.

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, 0, errStatus );

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_readTraceOrCalDataDos" );
	// Used for error reporting
	sprintf(functionName, "ag875x_readTraceOrCalDataDos");

	/*  Parameter checks */
	ag875x_CHK_ENUM(readTraceOrCalType,ag875x_RAFn_OUTPUT,VI_ERROR_PARAMETER2);
	ag875x_CHK_ENUM(readTraceOrCalArrayNum,ag875x_ARRAY_12,VI_ERROR_PARAMETER4);
		{
		ViChar readWriteBuf[kiMaxBufSize]; 
		ViReal32 readTraceOrCalArray_temp[kiMaxNumVals];
		ViUInt32 retCount = 0;
		ViInt16 numBytes = 0;
		ViInt16 inc = 0;
		ViInt16 iSmithChart = 0;
		ViInt16 iPolar = 0;
		ViInt16 instArrayNumber = 0;	// actual array number passed to instrument
		char cur_ID[15], message[ag875x_ERR_MSG_LENGTH];

    	// Enums start with 0, so set the number used by instrument.
		instArrayNumber = readTraceOrCalArrayNum + 1;
		
		// Set instrument for Form5 type output
		errStatus =ag875x_cmd(vi,"FORM5;");
    	if( errStatus < VI_SUCCESS)
    		return statusUpdate(vi, thisPtr, errStatus);
        
		switch (readTraceOrCalType)
        	{ 
        	case ag875x_CALCnn_OUTPUT: case ag875x_ICALnn_OUTPUT:
		
				// Check instrument model
				if ((errStatus = ag875x_readID(vi, cur_ID, "MODEL?", 0)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if (strstr(cur_ID, "ET") != NULL)
					{
					ag875x_CHK_INT_RANGE(instArrayNumber, ag875x_ARRAY_MIN,
						    				 ag875x_ARRAY_MAX_ET, ag875x_INVALID_ARRAY_NUMBER);
					}
				else
					{
					ag875x_CHK_INT_RANGE(instArrayNumber, ag875x_ARRAY_MIN,
						    				 ag875x_ARRAY_MAX_S_PARM, ag875x_INVALID_ARRAY_NUMBER);
					}
				if (readTraceOrCalType == ag875x_CALCnn_OUTPUT)
					errStatus =viPrintf(vi,"OUTPCALC%02i;\n", instArrayNumber);
				else
					{
					errStatus =viPrintf(vi,"OUTPICAL%02i;\n", instArrayNumber);
					}
                break;
             
        	case ag875x_DATA_OUTPUT:
    			errStatus =viPrintf(vi,"OUTPDATA;\n");
                break;
             
        	case ag875x_DATF_OUTPUT:
    			errStatus =viPrintf(vi,"OUTPDATF;\n");
                break;
             
        	case ag875x_FORM_OUTPUT:
    			errStatus =viPrintf(vi,"OUTPFORM;\n");
                break;
             
        	case ag875x_FORF_OUTPUT:
    			errStatus =viPrintf(vi,"OUTPFORF;\n");
                break;

			case ag875x_MEMO_OUTPUT:
    			errStatus =viPrintf(vi,"OUTPMEMO;\n");
                break;
             
			case ag875x_MEMF_OUTPUT:
    			errStatus =viPrintf(vi,"OUTPMEMF;\n");
                break;
             
        	case ag875x_PREn_OUTPUT: case ag875x_RAWn_OUTPUT: case ag875x_RAFn_OUTPUT:

				if ((errStatus = ag875x_readID(vi, cur_ID, "MODEL?", 0)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if (strstr(cur_ID, "ET") != NULL)
					{
					if (readTraceOrCalType == ag875x_PREn_OUTPUT)
						{
						sprintf(message, "Function = %s", functionName);
						strcpy(thisPtr->errMessage, message);
						ag875x_LOG_STATUS( vi, thisPtr, ag875x_REQUIRES_S_PARM_ANALYZER );
						}
					else if (instArrayNumber != ag875x_ARRAY_MIN)
						ag875x_LOG_STATUS( vi, thisPtr, ag875x_INVALID_ARRAY_NUMBER );
					}
				else
					{
					ag875x_CHK_INT_RANGE(instArrayNumber, ag875x_ARRAY_MIN,
						    				 ag875x_RAW_MAX_S_PARM, ag875x_INVALID_ARRAY_NUMBER);
					}
				if (readTraceOrCalType == ag875x_PREn_OUTPUT)
	    			errStatus =viPrintf(vi,"OUTPPRE%i;\n", instArrayNumber);
				else if (readTraceOrCalType == ag875x_RAWn_OUTPUT)
					errStatus =viPrintf(vi,"OUTPRAW%i;\n", instArrayNumber);
				else
    				errStatus =viPrintf(vi,"OUTPRAF%i;\n", instArrayNumber);
         		break;
    
    		default: 
            	break;
        	}      

		if (errStatus < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		/* TURN OFF THE NEWLINE CHARACTERS IN THE TRACE*/
		if ((errStatus = viSetAttribute(vi, VI_ATTR_TERMCHAR_EN, VI_FALSE)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
                    
		/* Read the first Four Characters */
		errStatus = viRead(vi,(ViBuf)readWriteBuf, 4, &retCount);
		if( VI_SUCCESS > errStatus )
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
                    
		/* Check For the "#A" string in the Header */
                            
		if ((readWriteBuf[0] != '#')&&(readWriteBuf[1] != 'A'))
			ag875x_LOG_STATUS( vi, thisPtr, ag875x_REC_HEADER_MISMATCH );
                            		
		memcpy(&numBytes,readWriteBuf+2,sizeof(ViInt16));
                   		         
		errStatus = viRead(vi,(ViBuf)readWriteBuf,numBytes, &retCount);
			if( VI_SUCCESS > errStatus )
            	ag875x_LOG_STATUS( vi, thisPtr, errStatus );
                    				  
            {
			memcpy(readTraceOrCalArray_temp, readWriteBuf, (size_t)numBytes);
				                    
   			if (readTraceOrCalType == ag875x_FORF_OUTPUT)
				{
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

					/* Query if Smith Chart is active display format */
				errStatus = viQueryf(vi, "SMIC?;\n", "%i", &iSmithChart);
				if( VI_SUCCESS > errStatus )
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

   				/* Query if Polar is active display format */
				errStatus = viQueryf(vi, "POLA?;\n", "%i", &iPolar);
				if( VI_SUCCESS > errStatus )
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				}

			/* Check if one of the cases where OUTPFORF yields only one data value per point */
			if ((readTraceOrCalType == ag875x_FORF_OUTPUT)&&(iSmithChart != 1)&&(iPolar != 1))
				{   	                 	 
				for (inc=0;inc<numBytes/4;inc++)   /* loop through data points*/
					{
	       			readTraceOrCalArray[inc] = readTraceOrCalArray_temp[inc]; 
					}
				}
			else
				{  	                 	 
				for (inc=0;inc<numBytes/4;inc+=2)   /* loop through data points*/
					{
	       			readTraceOrCalArray[inc] = readTraceOrCalArray_temp[inc]; 
	       			readTraceOrCalArray[inc+1] = readTraceOrCalArray_temp[inc+1]; 
					}
				}
            }
     }
    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_recallOrClearReg
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : This function allows Recalling From or Clearing a selected memory 
 *           Register in the instrument, or Clearing All Registers.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViInt16 recallOrClearRegMode
 * IN        
 *            This parameter selects the task to be performed by this function 
 *            (RECALL, CLEAR or CLEAR ALL).
 * 
 * PARAM 3 : ViInt16 recallOrClearRegNum
 * IN        
 *            Specifies the Register Number.
 * 
 *      MAX = ag875x_REG_NUM_MAX   31
 *      MIN = ag875x_REG_NUM_MIN   1
 * 
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
static const char * const ag875x_recallOrClearReg_selectMode_a[] = {"1","2","3","4",0};

ViStatus _VI_FUNC ag875x_recallOrClearReg(ViSession vi,
	ViInt16 recallOrClearRegMode,
	ViInt16 recallOrClearRegNum)

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		{
		ag875x_LOG_STATUS( vi, 0, errStatus );
		}

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_recallOrClearReg" );

	/*  Parameter checks */
    ag875x_CHK_ENUM(recallOrClearRegMode,2,VI_ERROR_PARAMETER2);
    ag875x_CHK_INT_RANGE(recallOrClearRegNum
                        ,ag875x_REG_NUM_MIN
                        ,ag875x_REG_NUM_MAX
                        ,VI_ERROR_PARAMETER3);

		{
		ViInt16 opc_result = 0;

		// Make sure were in Internal Memory Mode
		errStatus = viPrintf(vi,"INTM;\n");
		if (errStatus < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		errStatus = viPrintf(vi,"MENURECA;\n");
		if (errStatus < VI_SUCCESS)
    		ag875x_LOG_STATUS( vi, thisPtr, errStatus );
    
		switch (recallOrClearRegMode)
    		{ 
    		case ag875x_RC_RECALL_REG: 
        		{
       	 		errStatus = viPrintf(vi,"OPC?;RECAREG%02d;\n",recallOrClearRegNum);
   				if (errStatus < VI_SUCCESS)
       					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
    			} 
    			break;
    
    		case ag875x_RC_CLEAR_REG: 
        		{
       	 		errStatus = viPrintf(vi,"CLEAREG%02d;OPC?;NOOP;\n",recallOrClearRegNum);
   				if (errStatus < VI_SUCCESS)
       					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
    			} 
            	break;				
           				
    		case ag875x_RC_CLEAR_ALL_REG: 
            	{
    			errStatus = viPrintf(vi,"OPC?;CLEARALL;\n");
        		if (errStatus < VI_SUCCESS)
    					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
    	 	    } 
        	    break;

            default: 
            	break;
    		}   
	    
     	errStatus = viScanf(vi, "%d",&opc_result);
		if (errStatus < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		errStatus = viPrintf(vi,"MENURECA;\n");
		if (errStatus < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		}

    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_saveToReg
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : This function saves an instrument state to one of the instrument's 
 *           internal memory registers.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViChar _VI_FAR saveRegTitle[]
 * IN        
 *            Sequence of characters for the register title is limited to a 
 *            maximum of 8 characters. Valid characters: { 0-9, '_', A-Z, a-z }.
 *            If 'saveRegTitle' is a null string (""), the default title (REGxx) will  
 *            be used.
 * 
 * PARAM 3 : ViInt16 saveRegNum
 * IN        
 *            Specifies the Register Number.
 * 
 *      MAX = ag875x_REG_NUM_MAX   31
 *      MIN = ag875x_REG_NUM_MIN   1
 * 
 * RETURN  :  VI_SUCCESS:No error. Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_saveToReg(ViSession vi,
	ViChar _VI_FAR saveRegTitle[],
	ViInt16 saveRegNum)

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		{
		ag875x_LOG_STATUS( vi, 0, errStatus );
		}

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_saveToReg" );

	/*  Parameter checks */
	ag875x_CHK_PTR(saveRegTitle, VI_ERROR_PARAMETER2);
    ag875x_CHK_INT_RANGE(saveRegNum
                        ,ag875x_REG_NUM_MIN
                        ,ag875x_REG_NUM_MAX
                        ,VI_ERROR_PARAMETER3);

		{
		ViInt16 opc_result = 0;
		ViChar buff[80];

		if (strlen(saveRegTitle) != VI_NULL)
			{
			sprintf (buff, "TITREG%02d\"%s\";", saveRegNum, saveRegTitle); 
    
			errStatus = viPrintf( vi, "%s\n",buff);
  			if (errStatus < VI_SUCCESS)
   				ag875x_LOG_STATUS( vi, thisPtr, errStatus);
			}

		// Make sure were in Internal Memory Mode
		errStatus = viPrintf(vi,"INTM;\n");
		if (errStatus < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		errStatus = viPrintf(vi,"MENURECA;OPC?;SAVEREG%02d;\n",saveRegNum);
		if (errStatus < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

   		errStatus = viScanf(vi, "%d",&opc_result);
		if (errStatus < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		errStatus = viPrintf(vi,"MENURECA;\n");
		if (errStatus < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		}

    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */


/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_powerCoupling
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  Sets or returns ON/OFF status of: 
 *			  -> Power coupling between the channels, or 
 *            -> Power coupling between the ports for a specified channel.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViString powerCouplingRequest
 * IN        
 *            See code below for other choices. 
 * 
 * PARAM 3 : ViInt16 powerCouplingChannel (OPTIONAL; default=1)
 * IN        
 *            Select the Channel Number.  Channels 3 and 4 are valid only on 
 *            8753/8719/8720/8722 Firmware Revisions 7.40 and higher.
 * 
 * PARAM 4 : ViPBoolean powerCouplingState (OPTIONAL)
 * IN        
 *            Returns the coupling ON/OFF status corresponding to 'request'.
 * 
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_powerCoupling(ViSession vi,
	ViString powerCouplingRequest,
	ViInt16 powerCouplingChannel,
	ViPBoolean powerCouplingState)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 choice = 0;
	ViInt32 currentChannel = 0;
	ViInt32 size = 6;
	ViString req[]= {"CHAN POWER CPLD", "CHAN POWER UNCPLD", "CHAN POWER CPLD?", 
                     "PORT POWER CPLD", "PORT POWER UNCPLD", "PORT POWER CPLD?"};


	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_powerCoupling" );

	/*  Parameter checks */
	ag875x_CHK_PTR(powerCouplingRequest, VI_ERROR_PARAMETER2);
	ag875x_CHK_INT_RANGE(powerCouplingChannel,ag875x_CH1,ag875x_CH4,VI_ERROR_PARAMETER3);

	if(getChoice(vi,powerCouplingRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	// Save the current active channel number, so it may be restored later.
	if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Temporarily change to requested channel.
	if ((errStatus = ag875x_channelSelect(vi, powerCouplingChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	switch (choice)
		{
		case 0: //Couple power between channels
			if ((errStatus = viPrintf(vi, "COUPON;\n")) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;

		case 1: //Do not couple power between channels
			if ((errStatus = viPrintf(vi, "COUCOFF;COUPOFF;\n")) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;

		case 2: //ON/OFF status of channel power coupling
			if ((errStatus = viQueryf(vi, "COUP?;\n","%i",powerCouplingState)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;

		case 3: //Couple power between ports
			if ((errStatus = viPrintf(vi, "PORTPCPLD;\n")) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;

		case 4: //Do not couple power between ports
			if ((errStatus = viPrintf(vi, "PORTPUNCPLD;\n")) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;

		case 5: //ON/OFF status of port power coupling
			if ((errStatus = viQueryf(vi, "PORTP?;\n","%i",powerCouplingState)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;
		}

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Restore instrument to original channel number.
	if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_storeToDisk
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : This function is for storing an instrument state and/or the various 
 *           available types of data to a file on a floppy disk in the internal 
 *           drive. The storage format (Binary or ASCII) is also selectable.
 *
 *           NOTE: The instrument state is stored along with the selected types
 *                 of data EXCEPT when 'dataOnly' is ON, in which case only the 
 *                 error-corrected complex data is stored; parameters 3, 5,
 *                 6 and 7 ('dataArray', 'formatArray', 'graphics' and 'rawArray') will then 
 *                 make no difference.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViInt16 saveUsingFormat
 * IN        
 *            Storage format ASCII or Binary.
 * 
 * PARAM 3 : ViBoolean dataArray
 * IN        
 *            Specify ON/OFF which sets error-corrected complex data to be saved.
 * 
 * PARAM 4 : ViBoolean dataOnly
 * IN        
 *            Specify ON/OFF which sets error-corrected complex data ONLY to be saved.
 *            This means if 'dataOnly' is ON, the saving of all other data types will be
 *            suppressed; the saving of the instrument state will also be suppressed.
 *            Data can only be recalled in the network analyzer as part of an 
 *            instrument state, so this parameter should be set ON only when it is 
 *            desired to save space on the floppy disk and to read the data in a 
 *            computer. 
 * 
 * PARAM 5 : ViBoolean formatArray
 * IN        
 *            Specify ON/OFF which sets Formatted Trace Data to be saved.
 * 
 * PARAM 6 : ViBoolean graphics
 * IN        
 *            Specify ON/OFF which sets User Graphics Data to be saved.
 * 
 * PARAM 7 : ViBoolean rawArray
 * IN        
 *            Specify ON/OFF which sets Raw Array Data to be saved.
 * 
 * PARAM 8 : ViChar _VI_FAR storeFileTitle[]
 * IN        
 *            Sequence of characters for the file title is limited to a maximum 
 *            of 8 characters. Valid characters: { 0-9, '_', A-Z, a-z }.
 * 
 * RETURN  :  VI_SUCCESS:No error. Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
static const char * const ag875x_storeToDisk_saveUsingFormat_a[] = {"SAVUASCI",
        "SAVUBINA",0};
static const char * const ag875x_storeToDisk_dataArray_a[] = {"OFF","ON",0};
static const char * const ag875x_storeToDisk_dataOnly_a[] = {"OFF","ON",0};
static const char * const ag875x_storeToDisk_formatArray_a[] = {"OFF","ON",0};
static const char * const ag875x_storeToDisk_graphics_a[] = {"OFF","ON",0};
static const char * const ag875x_storeToDisk_rawArray_a[] = {"OFF","ON",0};

ViStatus _VI_FUNC ag875x_storeToDisk(ViSession vi,
  ViInt16 saveUsingFormat,
  ViBoolean dataArray,
  ViBoolean dataOnly,
  ViBoolean formatArray,
  ViBoolean graphics,
  ViBoolean rawArray,
  ViChar _VI_FAR storeFileTitle[])

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		{
		ag875x_LOG_STATUS( vi, 0, errStatus );
		}

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_storeToDisk" );

    
	/*  Parameter checks */
	ag875x_CHK_ENUM(saveUsingFormat,1,VI_ERROR_PARAMETER2);
    ag875x_CHK_BOOLEAN(dataArray,VI_ERROR_PARAMETER3);
    ag875x_CHK_BOOLEAN(dataOnly,VI_ERROR_PARAMETER4);
    ag875x_CHK_BOOLEAN(formatArray,VI_ERROR_PARAMETER5);
    ag875x_CHK_BOOLEAN(graphics,VI_ERROR_PARAMETER6);
    ag875x_CHK_BOOLEAN(rawArray,VI_ERROR_PARAMETER7);
	ag875x_CHK_PTR(storeFileTitle, VI_ERROR_PARAMETER8);

		{
		ViChar buff[80];
		ViBoolean reply = VI_FALSE;

		// First change to the internal disk drive.
		errStatus = viPrintf(vi,"INTD\n");
		// Allow time (1000 ms) for error to be received if no disk in internal drive.
		Sleep(1000);
		// Wait till instrument is ready and recheck error status
		errStatus = ag875x_opc_Q(vi, "WAIT", &reply);
        if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}
    
        errStatus = viPrintf(vi,"EXTMDATA %s\n",ag875x_storeToDisk_dataArray_a[dataArray]);
        if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}
    
        errStatus = viPrintf(vi,"EXTMDATO %s\n",ag875x_storeToDisk_dataOnly_a[dataOnly]);
        if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}
    
        errStatus = viPrintf(vi,"EXTMFORM %s\n",ag875x_storeToDisk_formatArray_a[formatArray]);
        if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}

        errStatus = viPrintf(vi,"EXTMGRAP %s\n",ag875x_storeToDisk_graphics_a[graphics]);
        if (errStatus < VI_SUCCESS)
			{
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}
        
        errStatus = viPrintf(vi,"EXTMRAW %s\n",ag875x_storeToDisk_rawArray_a[rawArray]);
        if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}
        
        sprintf (buff, "TITF1\"%s\";", storeFileTitle); 
    
        errStatus = viPrintf( vi, "%s\n",buff);
    	if( VI_SUCCESS > errStatus)
    		{	
    		ag875x_LOG_STATUS( vi, thisPtr, errStatus);
    		}
    
        errStatus = viPrintf(vi,"%s\n",ag875x_storeToDisk_saveUsingFormat_a[saveUsingFormat]);
        if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}
    
    	errStatus =viPrintf(vi,"STOR1;\n");
    	if( VI_SUCCESS > errStatus)
    		{	
    		ag875x_LOG_STATUS( vi, thisPtr, errStatus);
    		}
    
		if ((errStatus = ag875x_outputf(vi, 3, "NOOP;")) < VI_SUCCESS)
    		ag875x_LOG_STATUS( vi, thisPtr, errStatus);
		}
		
    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_freqListSweepSeg
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This is for desired segment/segments selection from the
 *            pre-defined Frequency List table.
 *            Selection of segment Number '1' to '30' will narrow the sweep to
 *            only that particular segment.
 *            Selection of segment Number '31' will expand the sweep to include
 *            all segments in the list table.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViInt16 freqListSweepSegNum
 * IN        
 *            Specify the Sweep Segment Number.
 * 
 *		MAX = ag875x_SEGMENT_NUM_MAX   31
 *      MIN = ag875x_SEGMENT_NUM_MIN   1
 * 
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_freqListSweepSeg(ViSession vi,
  ViInt16 freqListSweepSegNum)

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		{
		ag875x_LOG_STATUS( vi, 0, errStatus );
		}

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_freqListSweepSeg" );

	/*  Parameter checks */
    ag875x_CHK_INT_RANGE(freqListSweepSegNum
                        ,ag875x_SEGMENT_NUM_MIN
                        ,ag875x_SEGMENT_NUM_MAX
                        ,VI_ERROR_PARAMETER2);

		{
        errStatus = viPrintf(vi,"MENUSTIM\n");
        if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}
    
    	if ((freqListSweepSegNum >= 1) && (freqListSweepSegNum  <= 30))
    		{
        	errStatus = viPrintf(vi,"SSEG %d\n",freqListSweepSegNum);
        	if (errStatus < VI_SUCCESS)
        		{
           		ag875x_LOG_STATUS( vi, thisPtr, errStatus );
        		}
    		}
    
    	if (freqListSweepSegNum == 31)
    		{
        	errStatus = viPrintf(vi,"ASEG\n");
        	if (errStatus < VI_SUCCESS)
        		{
    			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
        		}
    
    		}
		}

    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_freqSweepType
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function will activate the selected type of frequency sweep. 
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViInt16 freqSweepType
 * IN        
 *            Selects the type of sweep.  Choices are Linear Frequency, 
 *            Log Frequency, CW Time, or List Frequency.
 * 
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
static const char * const ag875x_freqSweepType_swpType_a[] = {"LINFREQ",
        "LOGFREQ","CWTIME","LISFREQ",0};

ViStatus _VI_FUNC ag875x_freqSweepType(ViSession vi,
  ViInt16 freqSweepType)

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;
	ViInt16 freqTableEmpty = 0;

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		{
		ag875x_LOG_STATUS( vi, 0, errStatus );
		}

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_freqSweepType" );

    /*  Parameter checks */
	ag875x_CHK_ENUM(freqSweepType,3,VI_ERROR_PARAMETER2);

		{
		errStatus = viPrintf(vi,"MENUSTIM\n");
		// Added delay due to error reporting timing problem
		doDelay(DELAY_TIME);
		if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}

		errStatus = viPrintf(vi,"%s\n",ag875x_freqSweepType_swpType_a[freqSweepType]);
		// Added delay due to error reporting timing problem
		doDelay(DELAY_TIME);
		if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}
		}

    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_writeTraceOrCalDataDos
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : This function writes (sends) trace data or calibration data to the
 *           instrument in "FORM5" PC DOS format (not compatible with UNIX 
 *           Platform). 
 *           'writeTraceOrCalType' selects the type of data array to be written.
 *           'writeTraceOrCalArray' is Input Real 64 bit one-dimensional array. 
 *           (Internally to the function this 'writeTraceOrCalArray' is 32 bit Real 
 *           array. Hence the 'writeTraceOrCalArray' values will be of 32 bit resolution.)
 *           'writeTraceOrCalArrayNum' is used to select the number of the calibration 
 *           array or raw data array to be written.
 *           'writeTraceOrCalPoints' is used for selecting the number of points on sweep.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *           Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViInt16 writeTraceOrCalType
 * IN        
 *           'writeTraceOrCalType' selects the type of data to be written to the instrument.
 * 
 * PARAM 3 : ViReal64 _VI_FAR writeTraceOrCalArray[]
 * IN        
 *           64 bit Real one-dimensional array to be sent to the instrument.
 * 
 * PARAM 4 : ViInt16 writeTraceOrCalArrayNum
 * IN        
 *           The number of the calibration array or raw data array to be written
 *           to the instrument.  For DATA or FORMatted data, 'writeTraceOrCalArrayNum' is 
 *           simply a placeholder (not used).
 *
 *           For ag875x_CALCnn_INPUT (cal arrays):
 * 
 *				MAX = ag875x_ARRAY_MAX_ET = 6       (8753ET/8719ET/8720ET/8722ET analyzers)
 *				MAX = ag875x_ARRAY_MAX_S_PARM = 12  (All S-parameter 8753/8719/8720/8722 analyzers)
 *				MIN = ag875x_ARRAY_MIN = 1
 * 
 *           For ag875x_RAWn_INPUT (raw data arrays):
 * 
 *				MAX = ag875x_RAW_MAX_ET = 1      (8753ET/8719ET/8720ET/8722ET analyzers)
 *				MAX = ag875x_RAW_MAX_S_PARM = 4  (All S-parameter 8753/8719/8720/8722 analyzers)
 *				MIN = ag875x_ARRAY_MIN = 1
 * 
 * PARAM 5 : ViInt16 writeTraceOrCalPoints
 * IN        
 *           Select the number of points in the sweep
 * 
 * RETURN  : VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *           To determine error message, pass the return value to routine
 *           "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_writeTraceOrCalDataDos(ViSession vi,
  ViInt16 writeTraceOrCalType,
  ViReal64 _VI_FAR writeTraceOrCalArray[],
  ViInt16 writeTraceOrCalArrayNum,
  ViInt16 writeTraceOrCalPoints)

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, 0, errStatus );

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_writeTraceOrCalDataDos" );

	/*  Parameter checks */
    ag875x_CHK_ENUM(writeTraceOrCalType, ag875x_RAWn_INPUT,VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(writeTraceOrCalArray, VI_ERROR_PARAMETER3);
	ag875x_CHK_ENUM(writeTraceOrCalArrayNum,ag875x_ARRAY_12,VI_ERROR_PARAMETER4);
	ag875x_CHK_ENUM(writeTraceOrCalPoints,ag875x_PNTS_1601,VI_ERROR_PARAMETER5);

		{
        ViChar readWriteBuf[kiMaxBufSize]; 
        ViReal32 writeTraceOrCalArray_temp[kiMaxNumVals];	
        ViUInt16 j;
        ViUInt32 retCount;
        short int i, n;		 
        char *ptr_char,*readWriteBuf_ptr,*ptr; 
        char cur_ID[15];
		ViInt16 instArrayNumber = 0;	// actual array number passed to instrument
		ViInt16 instPointsNumber = 0;	// actual points value passed to instrument

		// Enums start with 0, so set the number used by instrument.
		instArrayNumber = writeTraceOrCalArrayNum + 1;
		instPointsNumber = convertEnumToPoints(writeTraceOrCalPoints);
        
        errStatus = viPrintf(vi,"POIN%d\n",instPointsNumber);
        if (errStatus < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		n = instPointsNumber * 2;

     	for (i=0; i<n; i++)
    		writeTraceOrCalArray_temp[i] = (float)writeTraceOrCalArray[i];

        n *=4;
    
        ptr = (char*)&n;
    
        readWriteBuf_ptr = readWriteBuf;
    
        j = sprintf( readWriteBuf_ptr,"#A");
        readWriteBuf_ptr += j ;
        							  
        j = sprintf( readWriteBuf_ptr, "%c%c",*ptr,*(ptr+1));
        readWriteBuf_ptr += j ;
        
        ptr_char = (char *)writeTraceOrCalArray_temp ;
    
        for(i=0;i<n;i++)
			*readWriteBuf_ptr++ = *ptr_char++ ;
    
        errStatus =ag875x_cmd(vi,"FORM5;");
		if( errStatus < VI_SUCCESS)
    		return statusUpdate(vi, thisPtr, errStatus);

    	switch (writeTraceOrCalType)
			{ 
        	case ag875x_CALCnn_INPUT:

        		if ((errStatus = ag875x_readID(vi, cur_ID, "MODEL?", 0)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				if (strstr(cur_ID, "ET") != NULL)
					{ 
					ag875x_CHK_INT_RANGE(instArrayNumber, ag875x_ARRAY_MIN,
						    	        		 ag875x_ARRAY_MAX_ET, ag875x_INVALID_ARRAY_NUMBER);
					}
				else
					{
					ag875x_CHK_INT_RANGE(instArrayNumber, ag875x_ARRAY_MIN,
						    	        		 ag875x_ARRAY_MAX_S_PARM, ag875x_INVALID_ARRAY_NUMBER);
					}
   				errStatus =viPrintf(vi,"INPUCALC%02i;\n", instArrayNumber);
                break;
         
			case ag875x_DATA_INPUT: case ag875x_MEMO_INPUT:
    				errStatus =viPrintf(vi,"INPUDATA;\n", instArrayNumber);
                break;
     
        	case ag875x_FORM_INPUT:
    				errStatus =viPrintf(vi,"INPUFORM;\n", instArrayNumber);
                break;
     
        	case ag875x_RAWn_INPUT:

        		if ((errStatus = ag875x_readID(vi, cur_ID, "MODEL?", 0)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if ((strstr(cur_ID, "ET") != NULL) && (instArrayNumber != ag875x_ARRAY_MIN))
					{ 
					ag875x_LOG_STATUS( vi, thisPtr, ag875x_INVALID_ARRAY_NUMBER );
					}
				else
					{
					ag875x_CHK_INT_RANGE(instArrayNumber, ag875x_ARRAY_MIN,
						    	        	 ag875x_RAW_MAX_S_PARM, ag875x_INVALID_ARRAY_NUMBER);
					}
   				errStatus =viPrintf(vi,"INPURAW%i;\n", instArrayNumber);
                break;

    		default: 
                break;
			}  /* end switch */

		if (errStatus < VI_SUCCESS)
           	ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			
		if ((errStatus = viSetAttribute(vi, VI_ATTR_TERMCHAR_EN,VI_FALSE)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
      
		i=n+4;

		errStatus = viWrite(vi,(ViBuf)readWriteBuf, i, &retCount);	 
		if( errStatus < VI_SUCCESS )
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if (writeTraceOrCalType == ag875x_MEMO_INPUT)
			if ((errStatus = viPrintf(vi,"DATI;\n")) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		viClear(vi);
		}

    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_calType_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : This function returns the calibration type, if any, for the 
 *           active measurement on the specified channel.  Possible output 
 *           values are:
 *
 *           Macro Name					Value
 *           --------------------------	---------------
 *           ag875x_CAL_NONE            "NONE"
 *           ag875x_CAL_RESPONSE        "RESPONSE"
 *           ag875x_CAL_RESP_ISOL       "RESP & ISOL"
 *           ag875x_CAL_ENH_RESP        "ENHANCED RESPONSE"
 *           ag875x_CAL_REV_ENH_RESP    "REVERSE ENHANCED RESPONSE"
 *           ag875x_CAL_S11_1_PORT      "S11 1-PORT"
 *           ag875x_CAL_S22_1_PORT      "S22 1-PORT"
 *           ag875x_CAL_FULL_2_PORT     "FULL 2-PORT"
 *           ag875x_CAL_1_PATH_2_PORT   "ONE PATH 2-PORT"
 *           ag875x_CAL_TRL_LRM         "TRL/LRM 2-PORT"
 *           ag875x_CHANNEL_NOT_ENABLED "CHANNEL NOT ENABLED"
 *
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViInt16 calTypeQChannel
 * IN        
 *            Selects the channel to be tested for a calibration.
 * 
 * PARAM 3 : ViPString currentCalTypeQ
 * OUT        
 *            Returns the active calibration type, if any.
 * 
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
 ViChar stCalCmd[10]; /* Global set in calType_Q function; used by longLrnString function */
ViInt16 iNumArrays = 0;  /* Global set in calType_Q function; used by longLrnString function */

ViStatus _VI_FUNC ag875x_calType_Q(ViSession vi,
	ViInt16 calTypeQChannel,
	ViPString currentCalTypeQ)

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;
	char functionName[256] = {0};	// Used for error reporting.

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, 0, errStatus );

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_calType_Q" );
	// Used for error reporting
	sprintf(functionName, "ag875x_calType_Q");

	/*  Parameter checks */
	ag875x_CHK_INT_RANGE(calTypeQChannel,ag875x_CH1,ag875x_CH4,VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(currentCalTypeQ, VI_ERROR_PARAMETER3);
    
		{
		ViInt32 cur_chan = 0;
		ViInt32 test_chan = 0;
		ViInt32 result = 0;
		ViChar cur_ID[300], message[ag875x_ERR_MSG_LENGTH];
		ViReal64 dFirmware = 0.0;

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if ((errStatus = ag875x_readID(vi, cur_ID, "REV_VALUE?", &dFirmware)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		
		// Save the current active channel number, so it may be restored later.
		if ((errStatus = viQueryf(vi,"OUTPCHAN;\n","%i",&cur_chan)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		
		// Channel enums start at 0, so increment by one for the instrument.
		test_chan = calTypeQChannel + 1;
		
		if (calTypeQChannel > ag875x_CH2)
			{
			if (compFirmwareVersions(dFirmware, "<", 7.40))
				{
				sprintf(message, "Channel = `%i';  Function = %s", test_chan, functionName);
				strcpy(thisPtr->errMessage, message);
				ag875x_LOG_STATUS( vi, thisPtr, ag875x_REQUIRES_4_PARM_FW );
				}

		   /* Firmware Revs 7.40 and 7.48 -- */
		   
			/* A 2-port cal type must be active for Channel 3 and/or 4 to be enabled. */
			if (compFirmwareVersions(dFirmware, "<", 7.60))	
			
			/* An auxiliary channel and its primary channel share the same 2-port cal  */
			// Can only use channels 1 or 2
				test_chan = test_chan - 2; 
			}

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if ((errStatus = ag875x_readID(vi, cur_ID, "MODEL?", 0)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if (!abs(strcmp(cur_ID, "8753D")))
			{
			if (compFirmwareVersions(dFirmware, ">=", 6.12))
				{
				// Get the instrument's current channel
				if ((errStatus = viQueryf(vi,"OUTPCHAN;\n","%i",&cur_chan)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				}
			else
				{
				cur_chan = test_chan;
				}
			}
		else
			{
			if ((errStatus = viQueryf(vi,"OUTPCHAN;\n","%i",&cur_chan)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}

		// test_chan is now set properly for accessing the instrument.
			  
		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			
		// Change to the channel to be queried.
		if ((errStatus = viQueryf(vi,"OPC?;CHAN%i;\n","%i",test_chan,&result)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		// Is it a full 2-port calibration?
		strcpy(stCalCmd,"CALIFUL2");
		if ((errStatus = viQueryf(vi,"%s?;\n","%i",stCalCmd,&result)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if (result == VI_TRUE)
			{
			strcpy(currentCalTypeQ,"FULL 2-PORT");
			// Set number of arrays the calibration contains.
			iNumArrays = ag875x_NUM_CAL_ARRAYS_FULL_2_PORT;
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viQueryf(vi,"OPC?;CHAN%i;\n","%i",cur_chan,&result)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
			}

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		// Is it a one path 2-port calibration?
		strcpy(stCalCmd,"CALIONE2");
		if ((errStatus = viQueryf(vi,"%s?;\n","%i",stCalCmd,&result)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if (result == VI_TRUE)
			{
			strcpy(currentCalTypeQ,"ONE PATH 2-PORT");
			// Set number of arrays the calibration contains.
			iNumArrays = ag875x_NUM_CAL_ARRAYS_1_PATH_2_PORT;
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viQueryf(vi,"OPC?;CHAN%i;\n","%i",cur_chan,&result)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
			}

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		// Is it a TRL/LRM 2-port calibration?
		strcpy(stCalCmd,"CALITRL2");
		if ((errStatus = viQueryf(vi,"%s?;\n","%i",stCalCmd,&result)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if (result == VI_TRUE)
			{
			strcpy(currentCalTypeQ,"TRL/LRM 2-PORT");
			// Set number of arrays the calibration contains.
			iNumArrays = ag875x_NUM_CAL_ARRAYS_TRL_LRM;
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viQueryf(vi,"OPC?;CHAN%i;\n","%i",cur_chan,&result)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
			}

		/* For Firmware 7.40 and 7.48, an auxiliary channel cannot be active 
		   unless its primary channel has one of the two-port cals above */

		if ((calTypeQChannel > ag875x_CH2) && (compFirmwareVersions(dFirmware, "<", 7.60)))
			{
			strcpy(currentCalTypeQ,"CHANNEL NOT ENABLED");
			iNumArrays = ag875x_NUM_CAL_ARRAYS_NONE;
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viQueryf(vi,"OPC?;CHAN%i;\n","%i",cur_chan,&result)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
			}

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		// Is there no calibration?
		strcpy(stCalCmd,"CALN");
		// Does not always work in firmware version 7.64
		//if ((errStatus = viQueryf(vi,"%s?;\n","%i",stCalCmd,&result)) < VI_SUCCESS)
		// Query if cal correction is turned on or not.  If not, then there is 
		// no active cal if effect. Returns 0 if no correction, 1 if correction
		if ((errStatus = viQueryf(vi,"CORR?;\n","%i",&result)) < VI_SUCCESS) 
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		//if (result == VI_TRUE)  /* use this one if using CALN? */
		if (result == VI_FALSE)
			{
			strcpy(currentCalTypeQ,"NONE");
			// Set number of arrays the calibration contains.
			iNumArrays = ag875x_NUM_CAL_ARRAYS_NONE;
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viQueryf(vi,"OPC?;CHAN%i;\n","%i",cur_chan,&result)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
			}

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		// Is it a response calibration?
		strcpy(stCalCmd,"CALIRESP");
		if ((errStatus = viQueryf(vi,"%s?;\n","%i",stCalCmd,&result)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if (result == VI_TRUE)
			{
			strcpy(currentCalTypeQ,"RESPONSE");
			// Set number of arrays the calibration contains.
			iNumArrays = ag875x_NUM_CAL_ARRAYS_RESPONSE;
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viQueryf(vi,"OPC?;CHAN%i;\n","%i",cur_chan,&result)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
			}

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		// Is it a response and isolation calibration?
		strcpy(stCalCmd,"CALIRAI");
		if ((errStatus = viQueryf(vi,"%s?;\n","%i",stCalCmd,&result)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if (result == VI_TRUE)
			{
			strcpy(currentCalTypeQ,"RESP & ISOL");
			// Set number of arrays the calibration contains.
			iNumArrays = ag875x_NUM_CAL_ARRAYS_RESP_ISOL;
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viQueryf(vi,"OPC?;CHAN%i;\n","%i",cur_chan,&result)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
			}

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		// Is it a S11 1-port calibration?
		strcpy(stCalCmd,"CALIS111");
		if ((errStatus = viQueryf(vi,"%s?;\n","%i",stCalCmd,&result)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if (result == VI_TRUE)
			{
			strcpy(currentCalTypeQ,"S11 1-PORT");
			// Set number of arrays the calibration contains.
			iNumArrays = ag875x_NUM_CAL_ARRAYS_S11_1_PORT;
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viQueryf(vi,"OPC?;CHAN%i;\n","%i",cur_chan,&result)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
			}

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		// Is it a S22 1-port calibration?
		strcpy(stCalCmd,"CALIS221");
		if ((errStatus = viQueryf(vi,"%s?;\n","%i",stCalCmd,&result)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if (result == VI_TRUE)
			{
			strcpy(currentCalTypeQ,"S22 1-PORT");
			// Set number of arrays the calibration contains.
			iNumArrays = ag875x_NUM_CAL_ARRAYS_S22_1_PORT;
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viQueryf(vi,"OPC?;CHAN%i;\n","%i",cur_chan,&result)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
			}

		/* NOTE: If Firmware Revision < 7.60, one of the above cal types (including "NONE")
		         would have already been detected, so we wouldn't get to this point */
		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		// Is it an enhanced response calibration?
		strcpy(stCalCmd,"CALIERC");
		if ((errStatus = viQueryf(vi,"%s?;\n","%i",stCalCmd,&result)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if (result == VI_TRUE)
			{
			strcpy(currentCalTypeQ,"ENHANCED RESPONSE");
			// Set number of arrays the calibration contains.
			iNumArrays = ag875x_NUM_CAL_ARRAYS_ENH_RESP;			
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viQueryf(vi,"OPC?;CHAN%i;\n","%i",cur_chan,&result)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
			}

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		// Is it a reverse enhanced response calibration?
		strcpy(stCalCmd,"CALIRERC");
		if ((errStatus = viQueryf(vi,"%s?;\n","%i",stCalCmd,&result)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if (result == VI_TRUE)
			{
			strcpy(currentCalTypeQ,"REVERSE ENHANCED RESPONSE");
			// Set number of arrays the calibration contains.
			iNumArrays = ag875x_NUM_CAL_ARRAYS_REV_ENH_RESP;
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viQueryf(vi,"OPC?;CHAN%i;\n","%i",cur_chan,&result)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
			}
			
		// Just in case, return to original channel.
		if ((errStatus = viQueryf(vi,"OPC?;CHAN%i;\n","%i",cur_chan,&result)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		}
		
    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */


/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ag875x_stringParse
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Parses any string. Requires a one-character delimiter (default= comma).
 *           This is not a typical VXIplug&play function being that it performs no 
 *           instrument I/O.
 *
 * PARAM 1 : ViSession vi
 * IN        Instrument Handle returned from ag875x_init().
 *            
 * PARAM 2 : ViChar _VI_FAR stringToParse[]. String to be parsed; returns remains of string
 * IN/OUT	 after parsing	 
 *
 * PARAM 3 : ViPString parsedString.  Contains string parsed. 
 * OUT		  
 *
 * PARAM 4 : ViPInt32 parseNotDone.  Returns 0 when last of string is parsed; 1 otherwise. 
 * OUT		  
 *
 * PARAM 4 : ViString  delimiter.  OPTIONAL; Default = "," (comma)
 * IN        Specifies a ONE char delimiter. 
 * 	  
 *
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 *-----------------------------------------------------------------------------
* 
/****************************************************************************/

ViStatus _VI_FUNC ag875x_stringParse(
				   ViSession vi,
                   ViChar _VI_FAR stringToParse[],
				   ViPString parsedString,
				   ViPInt32 parseNotDone,
				   ViString delimiter)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus;
	size_t p_len;
	char *inp2;
	const size_t size = strlen(stringToParse)+1;// +1 to account for NULL
	char *input = NULL;

	if ( (input = (char *)malloc(size)) == NULL )
       return VI_ERROR_ALLOC;

	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_stringParse" );

	/*  Parameter checks */
	ag875x_CHK_PTR(stringToParse, VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(parsedString, VI_ERROR_PARAMETER3);
	ag875x_CHK_PTR(parseNotDone, VI_ERROR_PARAMETER4);
	ag875x_CHK_PTR(delimiter, VI_ERROR_PARAMETER5);
		
	strcpy(input,stringToParse);
	strcpy(parsedString,"\0");

	p_len =strcspn(stringToParse, delimiter);
	*(stringToParse+p_len)='\0';//NULL
	strcpy(parsedString,stringToParse);

	if (!(strcmp(input,parsedString))) //then done parsing
		{
		inp2=input+p_len;
 		strcpy(stringToParse,inp2);
		*parseNotDone= 0;
		}
	else 
		{
		inp2=input+p_len+1;
 		strcpy(stringToParse,inp2);
		*parseNotDone= 1;
		}
	free(input);
	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_readID
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Reads analyzer model number or other information.    
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *            
 * PARAM 2 : ViPString currentID. Contains desired parameter.
 * OUT		 
 *
 * PARAM 3 : ViString IDRequest. OPTIONAL, Default = "MODEL?"
 * IN        Other choices are "SERIAL?", "REVISION?", "REV_VALUE?" 
 *
 * PARAM 4 : ViPReal64  IDStatus. OPTIONAL, Default = 0
 * OUT       Returns real version 
 *
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_readID( 
        ViSession vi, 
		ViPString currentID,
		ViString IDRequest,
		ViPReal64 IDStatus)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	char id[256], sn[256], model[256], rev[256], brand[256], options[256];
	ViInt32 choice = 0;
	ViInt32 dummy = 0;
	char *token;
	const ViInt32 size = 21;

	ViString req[]= { "MODEL?", "REVISION?", "REV?", "REV_VALUE?", 
						  "SERIAL?", "S/N?", "MINFREQ?", "MAXFREQ?", "002?", 
						  "004?", "006?", "007?", "010?", "011?", "012?", 
						  "014?", "075?", "085?", "089?", "1D5?", "400?" }; 


	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_readID" );

	/*  Parameter checks */
	ag875x_CHK_PTR(currentID, VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(IDRequest, VI_ERROR_PARAMETER3);

	if(getChoice(vi,IDRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	if (choice<4) //only do if needed for model number or firmware revision number
		{
		if ((errStatus = viFlush(vi,VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);
		if ((errStatus = viQueryf(vi, "*IDN?;\n", "%t", id)) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);

		errStatus = ag875x_stringParse(vi, id, brand, &dummy, ",");//not needed 
		errStatus = ag875x_stringParse(vi, id, model, &dummy, ",");
		errStatus = ag875x_stringParse(vi, id, sn, &dummy, ",");
		errStatus = ag875x_stringParse(vi, id, rev, &dummy, ",");

		rev[strlen(rev)-1]='\0';  //remove last quote from rev
		}

	if (choice>7) //only do if needed for option choice
		{
		if ((errStatus = viFlush(vi,VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);
		if ((errStatus = viQueryf(vi, "OUTPOPTS;\n", "%t", options)) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);
		*IDStatus = 0; //init to No
		}
 
	switch (choice)
		{ 
		case 0: // Model
			strcpy(currentID, model);
			break;

		case 1: case 2: // Revision
			strcpy(currentID, rev);
			break;

		case 3: // rev value
			strcpy(currentID, rev);
			*IDStatus =atof(rev); // convert to real number
			break;

		case 4: case 5: // Serial #
			if ((errStatus = viFlush(vi,VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			if ((errStatus = viQueryf(vi, "OUTPSERN;\n", "%t", sn)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);

			// Strip trailing line feed at the end of the string
			stripLF(sn);
			strcpy(currentID, sn);
			break;

		case 6: // min freq
			if ((errStatus = viFlush(vi,VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			if ((errStatus = viQueryf(vi, "STAR1;STAR?;\n", "%lf", IDStatus)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;

		case 7: // max freq
			if ((errStatus = viFlush(vi,VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			if ((errStatus = viQueryf(vi, "STOP200E9;STOP?;\n", "%lf", IDStatus)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;

		case 8: // Harmonic measurement capability? (8753)
			token = strstr(options, "002");
			*IDStatus = !(token== NULL);
			break;

		case 9: // Attenuator? (8753ET/8719ET/8720ET/8722ET)
			token = strstr(options, "004");
			*IDStatus = !(token== NULL);
			break;

		case 10: // 6 GHz freq range? (8753)
			token = strstr(options, "006");
			*IDStatus = !(token== NULL);
			break;

		case 11: // Mechanical transfer switch? (8719D/ES, 8720D/ES or 8722D/ES)
			token = strstr(options, "007");
			*IDStatus = !(token== NULL);
			break;

		case 12: // Time domain capability?
			token = strstr(options, "010");
			*IDStatus = !(token== NULL);
			break;

		case 13: // No built-in test set? (8753D/E/ES)
			token = strstr(options, "011");
			*IDStatus = !(token== NULL);
			break;

		case 14: // Direct sampler access? (8719D/ES, 8720D/ES or 8722D/ES)
			token = strstr(options, "012");
			*IDStatus = (token== NULL);
			break;

		case 15: // Configurable test set? (8753ES)
			token = strstr(options, "014");
			*IDStatus = !(token== NULL);
			break;

		case 16: // Built-in test set has 75 ohm impedance? (8753D/E/ES)
			token = strstr(options, "075");
			*IDStatus = !(token== NULL);
			break;

		case 17: // High power test capability? (8719D/ES, 8720D/ES or 8722D/ES)
			token = strstr(options, "085");
			*IDStatus = !(token== NULL);
			break;

		case 18: // Frequency offset mode? (8719D/ES, 8720D/ES or 8722D/ES)
			token = strstr(options, "089");
			*IDStatus = !(token== NULL);
			break;

		case 19: // High stability frequency reference?
			token = strstr(options, "1D5");
			*IDStatus = !(token== NULL);
			break;

		case 20: // Fourth sampler and TRL? (8719D/ES, 8720D/ES or 8722D/ES)
			token = strstr(options, "400");
			*IDStatus = !(token== NULL);
			break;
		} /*	end switch */		

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}		/* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_dualSplit4Parm
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : This function is for selection of Dual Channel Display and/or
 *           Split Channel Display modes for instruments having "4 S-parameter
 *           display" firmware (Firmware Revision 7.40 or higher).
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViBoolean dualChannel4ParmState
 * IN        
 *            Select the Dual Channel mode On/Off
 * 
 * PARAM 3 : ViInt16 splitDisplay4ParmMode
 * IN        
 *            Select the Split Channel mode (1, 2 or 4 graticules)
 * 
 * PARAM 4 : ViInt16 channelPosition2XDisplay
 * IN        
 *            Select desired channel positioning for when two graticules are to
 *            be displayed.
 * 
 * PARAM 5 : ViInt16 channelPosition4XDisplay
 * IN        
 *            Select desired channel positioning for when four graticules are to
 *            be displayed.
 *
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
static const char * const ag875x_dualSplit_dualState2_a[] = {"OFF","ON",0};
static const char * const ag875x_dualSplit_splitMode_a[] = {"1","2","4",0};
static const char * const ag875x_dualSplit_chanPos2xDisp_a[] = {"3","2",0};
static const char * const ag875x_dualSplit_chanPos4xDisp_a[] = {"3","2",0};

ViStatus _VI_FUNC ag875x_dualSplit4Parm(ViSession vi,
  ViBoolean dualChannel4ParmState,
  ViInt16 splitDisplay4ParmMode,
  ViInt16 channelPosition2XDisplay,
  ViInt16 channelPosition4XDisplay)

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;
	char functionName[256] = {0};	// Used for error reporting.

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		{
		ag875x_LOG_STATUS( vi, 0, errStatus );
		}

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_dualSplit4Parm" );
	// Used for error reporting
	sprintf(functionName, "ag875x_dualSplit4Parm");

	/*  Parameter checks */
    ag875x_CHK_BOOLEAN(dualChannel4ParmState,VI_ERROR_PARAMETER2);
    ag875x_CHK_ENUM(splitDisplay4ParmMode,2,VI_ERROR_PARAMETER3);
    ag875x_CHK_ENUM(channelPosition2XDisplay,1,VI_ERROR_PARAMETER4);
    ag875x_CHK_ENUM(channelPosition4XDisplay,1,VI_ERROR_PARAMETER5);

		{
		char cur_ID[15], message[ag875x_ERR_MSG_LENGTH];
		ViReal64 dFirmware = 0.0;

		if ((errStatus = ag875x_readID(vi, cur_ID, "REV_VALUE?", &dFirmware)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if (compFirmwareVersions(dFirmware, "<", 7.40))
			{
			sprintf(message, "Function = %s", functionName);
			strcpy(thisPtr->errMessage, message);
			ag875x_LOG_STATUS( vi, thisPtr, ag875x_REQUIRES_4_PARM_FW );
			}

		/* Show DISPLAY menu on instrument's display */
		errStatus = viPrintf(vi,"MENUDISP\n");
		if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}

		/* Set dual channel mode ON/OFF status */
		errStatus = viPrintf(vi,"DUAC%s\n",ag875x_dualSplit_dualState2_a[dualChannel4ParmState]);
		if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}

		/* Specify number of graticules for display */
		errStatus = viPrintf(vi,"SPLID%s\n",ag875x_dualSplit_splitMode_a[splitDisplay4ParmMode]);
		if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}

		/* Specify channel positioning in 2x Display mode */
		errStatus = viPrintf(vi,"D2XUPCH%s\n",ag875x_dualSplit_chanPos2xDisp_a[channelPosition2XDisplay]);
		if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}

		/* Specify channel positioning in 4x Display mode */
		errStatus = viPrintf(vi,"D4XUPCH%s\n",ag875x_dualSplit_chanPos4xDisp_a[channelPosition4XDisplay]);
		if (errStatus < VI_SUCCESS)
			{
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}
		}

    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_readLongLrnString
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : This function reads an instrument state learnstring from the 
 *           network analyzer as well as the following data:
 *              -> All currently active calibrations, including power meter cals
 *              -> All currently active memory traces
 *              -> Active measurement data from all channels (if indicated by
 *                 the 'dataTraceState' parameter)
 *              -> Calibration kit definition, if the active kit is user-defined
 *           This data is all stored to a file on the computer.  This file will 
 *           also contain the appropriate GPIB commands to restore all of 
 *           these settings, when the contents of the file are fed back to the 
 *           same (or another) analyzer later using the ag875x_writeLongLrnString 
 *           function.
 *
 *    NOTE : 8719, 8720 and 8722 analyzers share learnstring compatibility.  
 *           However, learnstrings from 8753 analyzers are NOT compatible with
 *           the 8719/8720/8722, and vice versa.
 *
 *
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViString readLongLrnFile
 * IN        
 *            Indicates the file name and directory location 
 *            (for example, C:\temp\myLrn.str) where the "long" learnstring that is
 *            read from the analyzer will be stored to.  
 *
 * PARAM 3 : ViReal64 lrnStringRevisionNum (OPTIONAL; default=0)
 * IN        
 *            Indicates which revision of learnstring the analyzer will output.
 *            This allows selecting a learnstring for compatibility with other
 *            analyzers of the same model number that have older firmware, or older
 *            analyzers in the same model number family.  The learnstring revision
 *            number is obtained by multiplying a firmware revision number by 100
 *            (for example, 7.48 * 100 = 748).  For instance, it may be desired to
 *            read a learnstring from an 8719ES so that it may later be sent to an
 *            8720D having Firmware 6.14 (using the ag875x_writeLongLrnString 
 *            function).  In that case, the value of this parameter should be 614.
 *            If the value of this parameter is less than 201 (Agilent 8753B Firmware 2.01 
 *            or 8720A Firmware 2.01), this function will default to reading the
 *            learnstring in the format of the instrument's current firmware revision.
 *
 * PARAM 4 : ViBoolean dataTraceState (OPTIONAL; default=ag875x_DATA_TRACE_SAVE)
 * IN        
 *            Indicates whether or not to write the active measurement data traces to
 *            the learnstring file.
 * 
 * RETURN  :  VI_SUCCESS:No error. Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
static const char * const ag875x_longLrnString_measType_a[] = {0,"S11",
        "S21","S12","S22","ANAI","AR","BR","AB","MEASA","MEASB","MEASR",0};

static const char * const ag875x_longLrnString_memDisp_a[] = {0,"DISPMEMO",
        "DISPDATM","DISPDDM","DISPDMM",0};

ViStatus _VI_FUNC ag875x_readLongLrnString(ViSession vi,
  ViString readLongLrnFile,
  ViReal64 lrnStringRevisionNum,
  ViBoolean dataTraceState)

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		{
		ag875x_LOG_STATUS( vi, 0, errStatus );
		}

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_readLongLrnString" );

	/*  Parameter checks */
    ag875x_CHK_INT_RANGE(dataTraceState
                        ,ag875x_NO_DATA_TRACE_SAVE
                        ,ag875x_DATA_TRACE_SAVE
                        ,VI_ERROR_PARAMETER4);
		{
		ViInt32 currentChannel = 0;			/* Current active channel */
		ViInt32 iReply = 0;
		ViUInt32 retCount = 0;
		FILE *fipLrn;                      /* Pointer to the learnstring file */
		ViChar readWriteBuf[kiMaxBufSize]; /* Character buffer for unformatted instrument I/O */
		ViChar cur_ID[15];                 /* Buffer to contain instrument model number */
		ViChar ID_Type[15];				   /* Buffer to contain instrument model type */
		ViReal64 dFirmware = 0.0;          /* Firmware revision number of instrument */

		#define kiNumMeas 11
		ViInt16 iNumBytes = 0;
		ViInt16 iNumPoints[3];
		ViInt16 pointsEnumValue = 0;		/* Enum value for number of points, needed by driver */
		ViInt16 iArrayNum = 0;
		ViInt16 iIndex1 = 0;
		ViInt16 iIndex2 = 0;
		ViInt16 iDataType = 0;

		ViInt16 iChan = 0;
		ViInt16 iCaldMeas[kiNumMeas+2];
		ViInt16 iCaldMeas2[kiNumMeas+2];
		ViInt16 iCmdLen = 0;
		ViInt16 iMeasIndex = 0;

		ViInt32 iCoupChan = 0;
		ViInt32 iCurChan = 0;
		ViInt32 iOtherChan = 0;
		ViInt32 iActiveCal = 0;
		ViInt32 iNoMemTrace[5];
		ViInt32 iAux[3];
		ViInt32 iAuxChan = 0;

		ViInt32 iCurChanHigh = 0;
		ViInt32 iMemDisp[5]; 
		ViInt32 iPrimaryChan = 0;
		ViInt32 iPwrMtrCal = 0;
		ViInt32 iChanInFile = 0;
		ViInt32 iUserCalKit = 0;

		#define kiCalTypeChars 30
		ViChar other_meas[6], cur_meas[6], calType[kiCalTypeChars], stLrnRev[5];
		ViReal64 traceOrCal[kiMaxNumVals];

		if (( fipLrn = fopen( readLongLrnFile, "wb" )) == NULL) /* Open file for write */
			ag875x_LOG_STATUS(vi, thisPtr, ag875x_FILE_OPEN_ERROR);

		// Save the current active channel number, so it may be restored later.
		if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if ((errStatus = ag875x_readID(vi, cur_ID, "REV_VALUE?", &dFirmware)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if (compFirmwareVersions(lrnStringRevisionNum, ">", dFirmware))
			ag875x_LOG_STATUS( vi, thisPtr, ag875x_LRN_NEWER_THAN_INST );

		if ((errStatus = ag875x_readID(vi, cur_ID, "MODEL?", 0)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		
		// Need to find out if it's an ES or ET instrument type
		ID_Type[0] = cur_ID[4];
		ID_Type[1] = cur_ID[5];
		ID_Type[2] = 0;

		cur_ID[4] = 0;
		fprintf(fipLrn, "#14%s", cur_ID);

		if(lrnStringRevisionNum < 201.0)
			sprintf(stLrnRev, "%i", (ViInt16)(dFirmware * 100.0));
		else
			sprintf(stLrnRev, "%i", (ViInt16)lrnStringRevisionNum);

		if ((errStatus = viPrintf(vi,"SELL%s;\n",stLrnRev)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		fprintf(fipLrn, "#1%i%s", strlen(stLrnRev), stLrnRev);
		fprintf(fipLrn, "#1%iSELL%s\n", strlen(stLrnRev)+5, stLrnRev);
	   
		if (!abs(strcmp(cur_ID, "8753D")))
			{
			if (compFirmwareVersions(dFirmware, ">=", 6.12))
				{
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if ((errStatus = viQueryf(vi,"OUTPCHAN;\n","%i",&iCurChan)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				}
			else
				{
				iCurChan = 1;
				}
			}
		else
			{
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			if ((errStatus = viQueryf(vi,"OUTPCHAN;\n","%i",&iCurChan)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}
		  
		// Remember, enums begin with 0
		if ((iCurChan - 1) > ag875x_CH2)
			{
			iCurChanHigh = VI_TRUE;
			iCurChan -= 2;
			}
		else
			iCurChanHigh = VI_FALSE;

		iOtherChan = 3 - iCurChan;

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if ((errStatus = viQueryf(vi,"OPC?;CHAN%i;\n","%i",iOtherChan,&iReply)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		iReply = 0; iIndex1 = 0;
		while (!iReply)
			{
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			iIndex1++;
			if ((errStatus = viQueryf(vi,"%s?;\n","%i",ag875x_longLrnString_measType_a[iIndex1],&iReply)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}
		strcpy(other_meas,ag875x_longLrnString_measType_a[iIndex1]);

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if ((errStatus = viQueryf(vi,"OPC?;CHAN%i;\n","%i",iCurChan,&iReply)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		iReply = 0; iIndex1 = 0;
		while (!iReply)
			{
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			iIndex1++;
			if ((errStatus = viQueryf(vi,"%s?;\n","%i",ag875x_longLrnString_measType_a[iIndex1],&iReply)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}
		strcpy(cur_meas,ag875x_longLrnString_measType_a[iIndex1]);

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if ((errStatus = viQueryf(vi,"COUC?;\n","%i",&iCoupChan)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		for (iChan = 1; iChan <= 2; iChan++)
			{
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			if ((errStatus = viQueryf(vi,"OPC?;CHAN%hd;\n","%i",iChan,&iReply)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
          
			if ((iChan == 1)||(!iCoupChan))
				{
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if ((errStatus = viQueryf(vi,"POIN?;\n","%hd",&iNumPoints[iChan])) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				}
			else 
				// No. of points on Ch 2 = Ch 1 if channels are coupled
				iNumPoints[2] = iNumPoints[1];

			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			if ((errStatus = viQueryf(vi,"DISPDATA?;\n","%i",&iNoMemTrace[iChan])) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			iAuxChan = iChan + 2;
			iNoMemTrace[iAuxChan] = VI_TRUE; iAux[iChan] = VI_FALSE;

			if (!iNoMemTrace[iChan])
				{
				iReply = 0; iIndex1 = 0;
				while (!iReply)
					{
					if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );

					iIndex1++;
					if ((errStatus = viQueryf(vi,"%s?;\n","%i",ag875x_longLrnString_memDisp_a[iIndex1],&iReply)) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );
					}
				iMemDisp[iChan] = iIndex1;

				if ((errStatus = viPrintf(vi,"DISPDATA;\n")) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				}

			if (compFirmwareVersions(dFirmware, ">=", 7.40))
				{
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if ((errStatus = viQueryf(vi,"AUXC?;\n","%i",&iAux[iChan])) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if (iAux[iChan])
					{
					if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );

					if ((errStatus = viQueryf(vi,"OPC?;CHAN%i;\n","%i",iAuxChan,&iReply)) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );

					if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );

					if ((errStatus = viQueryf(vi,"DISPDATA?;\n","%i",&iNoMemTrace[iAuxChan])) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );

					if (!iNoMemTrace[iAuxChan])
						{
						iReply = 0; iIndex1 = 0;
						while (!iReply)
							{
							if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
								ag875x_LOG_STATUS( vi, thisPtr, errStatus );

							iIndex1++;
							if ((errStatus = viQueryf(vi,"%s?;\n","%i",ag875x_longLrnString_memDisp_a[iIndex1],&iReply)) < VI_SUCCESS)
								ag875x_LOG_STATUS( vi, thisPtr, errStatus );
							}
						iMemDisp[iAuxChan] = iIndex1;

						if ((errStatus = viPrintf(vi,"DISPDATA;\n")) < VI_SUCCESS)
							ag875x_LOG_STATUS( vi, thisPtr, errStatus );
						}
					}  
				}
			}

		for (iChan = 1; iChan <= 2; iChan++)
			{
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			if ((errStatus = viQueryf(vi,"OPC?;CHAN%hd;\n","%i",iChan,&iReply)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			// If it's an ET instrument, then 
			// don't do values 3 & 4 of iIndex1 in the following loop
			// but do continue with the remaining values (5, 6, etc.)

			if ((iChan == 1)||(!iCoupChan))
				{
				iIndex2 = 1;
				for (iIndex1 = 1; iIndex1 <= kiNumMeas; iIndex1++)
					{
					if (!abs(strcmp(ID_Type, "ET")) && ((iIndex1 == 3) || (iIndex1 == 4)))
							; // do nothing
					else
						{
								
						if ((errStatus = viPrintf(vi,"%s;\n",ag875x_longLrnString_measType_a[iIndex1])) < VI_SUCCESS)
							ag875x_LOG_STATUS( vi, thisPtr, errStatus );

						if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
							ag875x_LOG_STATUS( vi, thisPtr, errStatus );

						if ((errStatus = viQueryf(vi,"CORR?;\n","%i",&iActiveCal)) < VI_SUCCESS)
							ag875x_LOG_STATUS( vi, thisPtr, errStatus );

						if (iActiveCal)
							{
							if (iChan == 1)
								iCaldMeas[iIndex2] = iIndex1;
							else
								iCaldMeas2[iIndex2] = iIndex1;
							iIndex2++;

							if ((errStatus = viPrintf(vi,"CORROFF;\n")) < VI_SUCCESS)
								ag875x_LOG_STATUS( vi, thisPtr, errStatus );
							}
						}
					}

				if (iChan == 1)
					iCaldMeas[iIndex2] = 0;
				else
					iCaldMeas2[iIndex2] = 0;
				}
			}

		if ((errStatus = viPrintf(vi,"OUTPLEAS;\n")) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		/* Read newline characters as data, not terminators */
		if ((errStatus = viSetAttribute(vi, VI_ATTR_TERMCHAR_EN, VI_FALSE)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
                    
		/* Read the first Four Characters */
		if((errStatus = viRead(vi,(ViBuf)readWriteBuf, 4, &retCount)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
                    
		/* Check For the "#A" string in the Header */
		if ((readWriteBuf[0] != '#')&&(readWriteBuf[1] != 'A'))
			ag875x_LOG_STATUS( vi, thisPtr, ag875x_REC_HEADER_MISMATCH );
                            		
		iNumBytes  = (ViByte)readWriteBuf[2]*256 +(ViByte)readWriteBuf[3];
                    
		if((errStatus = viRead(vi,(ViBuf)readWriteBuf+4,iNumBytes, &retCount))< VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		iDataType = 0; iArrayNum = 0;
		lrnToFile(fipLrn,readWriteBuf,iNumBytes,iDataType,traceOrCal,iArrayNum);

		for (iChan = 1; iChan <= (2 - iCoupChan); iChan++)
			{
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			if ((errStatus = viQueryf(vi,"OPC?;CHAN%hd;\n","%i",iChan,&iReply)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			iNumBytes = iNumPoints[iChan] * sizeof(double) * 2;

			iIndex1 = 1; iDataType = 3;

			while (((iChan == 1)&&(iCaldMeas[iIndex1]))||((iChan == 2)&&(iCaldMeas2[iIndex1])))
				{
				if (iChan == 1)
					iMeasIndex = iCaldMeas[iIndex1];
				else
					iMeasIndex = iCaldMeas2[iIndex1];

				if ((errStatus = viPrintf(vi,"%s;CORRON;\n",ag875x_longLrnString_measType_a[iMeasIndex])) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				// Find out the type of calibration by querying channel 1.
				if ((errStatus = ag875x_calType_Q(vi,ag875x_CH1,calType)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if ((iIndex1 == 1)&&(!iCoupChan))
					fprintf(fipLrn, "#17CHAN%i;\n",iChan);

				iCmdLen = strlen(ag875x_longLrnString_measType_a[iMeasIndex]) + strlen(stCalCmd) + 3;

				fprintf(fipLrn, "#2%i%s;%s;\n",iCmdLen,ag875x_longLrnString_measType_a[iMeasIndex],stCalCmd);
				
				// Get all the arrays and write them to the file.
				// Remember, array enums range from 0 - 11, but when writing the 
				// instument commands, the firmware is expecting 1 - 12!
				for (iArrayNum = 1; iArrayNum <= iNumArrays; iArrayNum++)
					{
					// Remember, the function is expecting array enums ranging from 0 to 11.
					if ((errStatus = ag875x_readTraceOrCalDataDos(vi,ag875x_CALCnn_OUTPUT,traceOrCal,(ViInt16)(iArrayNum - 1))) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );

					lrnToFile(fipLrn,readWriteBuf,iNumBytes,iDataType,traceOrCal,iArrayNum);
					}
			
				// Command to save the arrays when they are eventually read back into the instrument.
				fprintf(fipLrn, "#16SAVC;\n");

				iIndex1++;
				}
			}

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if ((errStatus = viQueryf(vi,"OPC?;CHAN%i;\n","%i",iOtherChan,&iReply)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

  		if ((errStatus = viPrintf(vi,"%s;\n",other_meas)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		fprintf(fipLrn, "#17CHAN%i;\n",iOtherChan);
		fprintf(fipLrn, "#1%i%s;\n",strlen(other_meas)+2,other_meas);

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if ((errStatus = viQueryf(vi,"OPC?;CHAN%i;\n","%i",iCurChan,&iReply)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

  		if ((errStatus = viPrintf(vi,"%s;\n",cur_meas)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		fprintf(fipLrn, "#17CHAN%i;\n",iCurChan);
		fprintf(fipLrn, "#1%i%s;\n",strlen(cur_meas)+2,cur_meas);

		for (iChan = 1; iChan <= 4; iChan++)
			{
			iChanInFile = VI_FALSE;
			iPrimaryChan = 2 - (iChan % 2);

			if ((iChan < 3)||((iChan >= 3)&&(iAux[iPrimaryChan])))
				{
				if (!iNoMemTrace[iChan])
					{
					if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );

					if ((errStatus = viQueryf(vi,"OPC?;CHAN%hd;\n","%i",iChan,&iReply)) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );

					fprintf(fipLrn, "#17CHAN%i;\n",iChan);
					iChanInFile = VI_TRUE;

					if ((errStatus = viPrintf(vi,"%s;\n",ag875x_longLrnString_memDisp_a[iMemDisp[iChan]])) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );

					// Read the memory trace
					if ((errStatus = ag875x_readTraceOrCalDataDos(vi,ag875x_MEMO_OUTPUT,traceOrCal,(ViInt16)(iArrayNum - 1))) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			 
					iNumBytes = iNumPoints[iPrimaryChan] * sizeof(double) * 2;

					iDataType = 5;
					lrnToFile(fipLrn,readWriteBuf,iNumBytes,iDataType,traceOrCal,iArrayNum);

					iCmdLen = strlen(ag875x_longLrnString_memDisp_a[iMemDisp[iChan]]) + 3;

					fprintf(fipLrn, "#2%i%s ;\n",iCmdLen,ag875x_longLrnString_memDisp_a[iMemDisp[iChan]]);
					}

				if (dataTraceState)
					{
					if (!iChanInFile)
						{
						if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
							ag875x_LOG_STATUS( vi, thisPtr, errStatus );

						if ((errStatus = viQueryf(vi,"OPC?;CHAN%hd;\n","%i",iChan,&iReply)) < VI_SUCCESS)
							ag875x_LOG_STATUS( vi, thisPtr, errStatus );

						fprintf(fipLrn, "#17CHAN%i;\n",iChan);
						iChanInFile = VI_TRUE;
						}

    				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
						 ag875x_LOG_STATUS( vi, thisPtr, errStatus );

					if ((errStatus = viQueryf(vi,"OPC?;SING;\n","%i",&iReply)) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );

					// When option ag875x_DATA_OUTPUT is selected, last parameter is ignored.
					if ((errStatus = ag875x_readTraceOrCalDataDos(vi,ag875x_DATA_OUTPUT,traceOrCal,0)) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			 
					iNumBytes = iNumPoints[iPrimaryChan] * sizeof(double) * 2;

					// For this data type, the last parameter is ignored.
					iDataType = 4;
					lrnToFile(fipLrn,readWriteBuf,iNumBytes,iDataType,traceOrCal,0);
					}
				}

			if ((iChan == 1)||((iChan == 2)&&(!iCoupChan)))
				{
				if (!iChanInFile)
					{
					if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );

					if ((errStatus = viQueryf(vi,"OPC?;CHAN%hd;\n","%i",iChan,&iReply)) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );
					}

				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if ((errStatus = viQueryf(vi,"PWMCEACS?;\n","%i",&iPwrMtrCal)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if (!iPwrMtrCal)
					{
					if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );

					if ((errStatus = viQueryf(vi,"PWMCONES?;\n","%i",&iPwrMtrCal)) < VI_SUCCESS)
						 ag875x_LOG_STATUS( vi, thisPtr, errStatus );
					}
			 
				if (iPwrMtrCal)
					{
					if (!iChanInFile)
						fprintf(fipLrn, "#17CHAN%i;\n",iChan);

					// Need to convert number of points into enum value req by driver.
					pointsEnumValue = convertPointsToEnum(iNumPoints[iChan]);

					if ((errStatus = ag875x_readPowerMeterCal(vi,iChan,traceOrCal,pointsEnumValue, ag875x_READ_UNINTERP_CAL)) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );
                
					iNumBytes = iNumPoints[iChan] * sizeof(double);

					iDataType = 2; iArrayNum = iChan;
					lrnToFile(fipLrn,readWriteBuf,iNumBytes,iDataType,traceOrCal,iArrayNum);
					}
				}
			}

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if ((errStatus = viQueryf(vi,"CALKUSED?;\n","%i",&iUserCalKit)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if (iUserCalKit)
			{
			if ((errStatus = viPrintf(vi,"OUTPCALK;\n")) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			/* Read newline characters as data, not terminators */
			if ((errStatus = viSetAttribute(vi, VI_ATTR_TERMCHAR_EN, VI_FALSE)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
                    
			/* Read the first Four Characters */
			if((errStatus = viRead(vi,(ViBuf)readWriteBuf, 4, &retCount)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
                    
			/* Check For the "#A" string in the Header */
			if ((readWriteBuf[0] != '#')&&(readWriteBuf[1] != 'A'))
				ag875x_LOG_STATUS( vi, thisPtr, ag875x_REC_HEADER_MISMATCH );
                            		
			iNumBytes  = (ViByte)readWriteBuf[2]*256 +(ViByte)readWriteBuf[3];
                    
			if((errStatus = viRead(vi,(ViBuf)readWriteBuf+4,iNumBytes, &retCount))< VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			iDataType = 1;
			lrnToFile(fipLrn,readWriteBuf,iNumBytes,iDataType,traceOrCal,iArrayNum);
			}

		if (iCurChanHigh)
			iCurChan+=2;

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		fprintf(fipLrn, "#17CHAN%i;\n",iCurChan);

		fclose(fipLrn);           /* Close the file */

		// Restore instrument to original channel number.
		if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
			ag875x_LOG_STATUS(vi, thisPtr, errStatus);
		}

    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */


/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_writeLongLrnString
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : This function writes a "long" learnstring to the network analyzer,
 *           that is read from a file (stored on the computer) created 
 *           previously by the ag875x_readLongLrnString function.  This will
 *           re-create the instrument state and restore all associated data.
 *
 *           Remember that such a file contains the following data in addition 
 *           to the instrument state learnstring:
 *              -> All calibrations that were active, including power meter cals
 *              -> All memory traces that were being displayed (or used in trace math)
 *              -> Calibration kit definition, if the active kit was user-defined
 *           The file may also contain the measurement data from all channels that  
 *           were being displayed, if that option had been selected for the 
 *           ag875x_readLongLrnString function.
 *
 *    NOTE : 8719, 8720 and 8722 analyzers share learnstring compatibility.  
 *           However, learnstrings from 8753 analyzers are NOT compatible with
 *           the 8719/8720/8722, and vice versa.
 *
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViString writeLongLrnFile
 * IN        
 *            Indicates the file name and directory location 
 *            (for example, C:\temp\myLrn.str) of the file that contains the 
 *            "long" learnstring to be restored to the analyzer.  
 * 
 * 
 * RETURN  :  VI_SUCCESS:No error. Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_writeLongLrnString(ViSession vi,
  ViString writeLongLrnFile)

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		{
		ag875x_LOG_STATUS( vi, 0, errStatus );
		}

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_writeLongLrnString" );

	/*  Parameter checks */
	ag875x_CHK_PTR(writeLongLrnFile, VI_ERROR_PARAMETER2);

		{
		ViInt32 iReply = 0;
		ViUInt32 retCount = 0;
		FILE *fipLrn;                      /* Pointer to the learnstring file */
		ViChar readWriteBuf[kiMaxBufSize]; /* Character buffer for unformatted instrument I/O */
		ViChar cur_ID[15];                 /* Buffer to contain instrument model number */
		ViReal64 dFirmware = 0.0;          /* Firmware revision number of instrument */
		
		ViInt16 iIndex = 0;
		ViInt16 iDigits = 0;
		ViInt16 totBytes = 0;
		char echo, stDigits[2];

		if (( fipLrn = fopen( writeLongLrnFile, "rb" )) == NULL) /* Open file for read */
			ag875x_LOG_STATUS(vi, thisPtr, ag875x_FILE_OPEN_ERROR);

		for (iIndex = 1; iIndex <= 2; iIndex++)
			{
			fscanf(fipLrn, "%c%c", &echo, stDigits);
			stDigits[1] = 0;
			iDigits = atoi(stDigits);
			fread(readWriteBuf, sizeof(char), (size_t)iDigits, fipLrn);
			readWriteBuf[iDigits]=0;
			totBytes = atoi(readWriteBuf);
			fread(readWriteBuf, sizeof(char), (size_t)totBytes, fipLrn);
			readWriteBuf[totBytes]=0;

			if (iIndex == 1)
				{
				if ((errStatus = ag875x_readID(vi, cur_ID, "MODEL?", 0)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				cur_ID[4] = 0;
				if ((atoi(readWriteBuf) == 8753)&&(atoi(cur_ID) != 8753))
					ag875x_LOG_STATUS( vi, thisPtr, ag875x_LRN_8753_TO_872X );

				if (((atoi(readWriteBuf) >= 8719)&&(atoi(readWriteBuf) <= 8722))
					&&(!((atoi(cur_ID) >= 8719)&&(atoi(cur_ID) <= 8722))))
					ag875x_LOG_STATUS( vi, thisPtr, ag875x_LRN_872X_TO_8753 );
				}
			else
				{
				if ((errStatus = ag875x_readID(vi, cur_ID, "REV_VALUE?", &dFirmware)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if (atoi(readWriteBuf) > (int)(dFirmware * 100))
					ag875x_LOG_STATUS( vi, thisPtr, ag875x_LRN_NEWER_THAN_INST );
				}
			}

		while (fscanf(fipLrn, "%c%c", &echo, stDigits) != EOF)
			{
			stDigits[1] = 0;
			iDigits = atoi(stDigits);
			fread(readWriteBuf, sizeof(char), (size_t)iDigits, fipLrn);
			readWriteBuf[iDigits]=0;
			totBytes = atoi(readWriteBuf);
			fread(readWriteBuf, sizeof(char), (size_t)totBytes, fipLrn);

			if ((errStatus = viWrite(vi,(ViBuf)readWriteBuf, (ViUInt32)totBytes, &retCount)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
        
			if ((errStatus = viClear(vi)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			if ((errStatus = viQueryf(vi,"OPC?;NOOP\n","%i",&iReply)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			
			}  


		fclose(fipLrn);           /* Close the file */

		}

    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_writePowerMeterCal
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : This function writes an array of power meter calibration data to 
 *           the instrument.  Use the ag875x_readArrayDataAscii function to read
 *           the array of power meter calibration data from the instrument.
 *
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViInt16 writePowerMeterCalChannel
 * IN        
 *            Selects the primary channel number that the power meter 
 *            calibration data will be written to.
 * 
 * PARAM 3 : ViReal64 _VI_FAR writePowerMeterCalArray[]
 * IN       
 *            64 bit Real one-dimensional array to be sent to the instrument.
 * 
 * PARAM 4 : ViInt16 writePowerMeterCalPoints
 * IN        
 *            Select the number of points in the sweep.  The number of values
 *            contained in the 'writePowerMeterCalArray' array must equal this number.
 * 
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_writePowerMeterCal(ViSession vi,
  ViInt16 writePowerMeterCalChannel,
  ViReal64 _VI_FAR writePowerMeterCalArray[],
  ViInt16 writePowerMeterCalPoints)

	{
	ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;
	ViInt32 oneSweepActive = 0;
	ViInt32 eachSweepActive = 0;
	ViInt16 instPointsNumber = 0;	// actual points value passed to instrument
	ViInt16 instChannelNumber = 0;	// actual channel number passed to instrument
		

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, 0, errStatus );

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_writePowerMeterCal" );

	/*  Parameter checks */
    ag875x_CHK_INT_RANGE(writePowerMeterCalChannel,ag875x_CH1,ag875x_CH2,VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(writePowerMeterCalArray, VI_ERROR_PARAMETER3);
    ag875x_CHK_ENUM(writePowerMeterCalPoints,ag875x_PNTS_1601,VI_ERROR_PARAMETER4);

		{
        ViInt16 inc = 0;

		// Enums start with 0, so set the number used by instrument.
		instChannelNumber = writePowerMeterCalChannel +1;
		instPointsNumber = convertEnumToPoints(writePowerMeterCalPoints);

		// Query instrument to see if a power meter calibration is active
		if ((errStatus = viQueryf(vi,"PWMCONES?;\n","%i",&oneSweepActive)) < VI_SUCCESS)
            ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		if ((errStatus = viQueryf(vi,"PWMCEACS?;\n","%i",&eachSweepActive)) < VI_SUCCESS)
            ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		if (!oneSweepActive && !eachSweepActive)
			return ag875x_POWERMETER_CAL_INACTIVE;
	
		if ((errStatus = viPrintf(vi, "POIN%hd;\n", instPointsNumber)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if ((errStatus =viPrintf(vi,"FORM4;INPUPMCAL%hd", instChannelNumber)) < VI_SUCCESS)
       		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		// Write the array.
    	for (inc=0; inc < instPointsNumber; inc++)
			{
			if (inc == 0)
				{
			    if ((errStatus = viPrintf(vi, "%lf", writePowerMeterCalArray[0])) < VI_SUCCESS)
		   			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				}
			else 
				{
				if ((errStatus = viPrintf(vi, ",%lf", writePowerMeterCalArray[inc])) < VI_SUCCESS)
		   		    ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				}
			}

		// Terminate and flush the buffer; note a regular VIFLUSH won't work here.
        if ((errStatus =  viPrintf(vi,"\n")) < VI_SUCCESS)
		    ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		}

	ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_clock  (YEAR 2000 COMPLIANT!)
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Performs various clock (time/date) functions.  Also allows user to  
 *			 synchronize the instrument clock with computer's clock.
 *
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *
 * PARAM 2 : ViString clockRequest.  Selects desired action; see code below for choices.  
 * IN		 
 *
 * PARAM 3 : ViString clockData.  OPTIONAL Contains time to be set in 23:59:59 format,
 * IN		 or date to be set in 01 JAN 2000 format.
 *            
 * PARAM 4 : ViPString currentClock. OPTIONAL  Returned time/date in above format 
 * OUT		  
 *
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_clock(
		   ViSession vi,
		   ViString clockRequest,
		   ViString clockData,
		   ViPString currentClock)
		 
	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 choice = 0;
	char month[4];
	struct tm *newtime;
	time_t aclock;
	const ViInt32 size = 5;

	ViString req[]= { "DATE", "TIME", "DATE?", "TIME?", "SYNC" }; 


	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_clock" );

	/*  Parameter checks */
	ag875x_CHK_PTR(clockRequest, VI_ERROR_PARAMETER2);

	if(getChoice(vi,clockRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	switch (choice)
		{
		case 0: //DATE
			errStatus = viPrintf(vi, "SETDATE %s;\n", clockData);
			break;

		case 1: //TIME
			errStatus = viPrintf(vi, "SETTIME %s;\n", clockData);
			break;

		case 2: //DATE?
			if ((errStatus = viPrintf(vi, "READDATE\n")) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			errStatus = viScanf(vi, "%t", currentClock);

			// Strip trailing line feed at the end of the string
			stripLF(currentClock);
			break;

		case 3: //TIME?
			if ((errStatus = viPrintf(vi, "READTIME\n")) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			errStatus = viScanf(vi, "%t", currentClock);

			// Strip trailing line feed at the end of the string
			stripLF(currentClock);
			break;

		case 4: //sync analyzer to Computer's clock
			time( &aclock );                 // Get time in seconds 
			newtime = localtime( &aclock );  // Convert time to struct tm form 
			switch ((newtime->tm_mon)+1)
				{
				case 1:
					strcpy(month,"Jan");
					break;
				case 2:
					strcpy(month,"Feb");
					break;
				case 3:
					strcpy(month,"Mar");
					break;
				case 4:
					strcpy(month,"Apr");
					break;
				case 5:
					strcpy(month,"May");
					break;
				case 6:
					strcpy(month,"Jun");
					break;
				case 7:
					strcpy(month,"Jul");
					break;
				case 8:
					strcpy(month,"Aug");
					break;
				case 9:
					strcpy(month,"Sep");
					break;
				case 10:
					strcpy(month,"Oct");
					break;
				case 11:
					strcpy(month,"Nov");
					break;
				case 12:
					strcpy(month,"Dec");
					break;
				}
			if ((errStatus = viPrintf(vi, "SETDATE %02i %s %i;\n",newtime->tm_mday,month,(newtime->tm_year)+1900)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			errStatus = viPrintf(vi, "SETTIME %02i:%02i:%02i;\n",newtime->tm_hour,newtime->tm_min,newtime->tm_sec);
			break;
			}
	if (errStatus < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);
	else
		return statusUpdate(vi, thisPtr, VI_SUCCESS);

	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_points
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Sets or returns current # of points  
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *            
 * PARAM 2 : ViInt16 points.  
 * IN        Number of points to be set    
 *
 * PARAM 3 : ViString pointsRequest. (OPTIONAL, default="SET"). 
 * IN        Other choice is "?" or "POINTS?"
 *
 * PARAM 4 : ViInt16 pointsChannel. (OPTIONAL, default=1) 
 * IN        Indicates channel number.  
 *
 * PARAM 5 : ViPInt16 currentPoints. (OPTIONAL) 
 * OUT       Indicates current number of points. 
 *
 *           
 * RETURN  :  VI_SUCCESS:No error. Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_points( 
        ViSession vi, 
		ViInt16 points,
		ViString pointsRequest,
		ViInt16 pointsChannel,
		ViPInt16 currentPoints)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 choice = 0;
	ViInt32 currentChannel = 0;
	ViInt16 instPointsNumber = 0;	// actual points value passed to instrument

	const ViInt32 size = 3;
	ViString req[]= {"?", "POINTS?", "SET"};

	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_points" );

	/*  Parameter checks */
	ag875x_CHK_PTR(pointsRequest, VI_ERROR_PARAMETER2);
	ag875x_CHK_ENUM(pointsChannel,ag875x_CH4,VI_ERROR_PARAMETER4);

	if(getChoice(vi,pointsRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	// Save the current active channel number, so it may be restored later.
	if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Temporarily change to requested channel.
	if ((errStatus = ag875x_channelSelect(vi, pointsChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	switch (choice)
		{	
		case 0: case 1:  // Query number of points
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

   			if ((errStatus = viQueryf(vi, "POIN?;\n","%hd",currentPoints)) < VI_SUCCESS)
				ag875x_LOG_STATUS(vi, thisPtr, errStatus);
			break;

		case 2:  // Set number of Points
			instPointsNumber = convertEnumToPoints(points);
			if ((errStatus = viPrintf(vi, "POIN %i;\n",instPointsNumber)) < VI_SUCCESS)
				ag875x_LOG_STATUS(vi, thisPtr, errStatus);
			break;
		}
		
	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Restore instrument to original channel number.
	if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);

	ag875x_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_powerSweep
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Sets or returns power sweep start/stop levels, or can be used to
 *           set power sweep ON/OFF status. 
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *            
 * PARAM 2 : ViString powerSweepRequest. 
 * IN   	 Selects desired action; see code below for choices.
 *
 * PARAM 3 : ViReal64 setStartPower.  OPTIONAL, default=0
 * IN        Start Value to be set.
 *
 * PARAM 4 : ViReal64 setStopPower. OPTIONAL, default=0
 * IN        Stop Value to be set.
 *
 * PARAM 5 : ViInt16 powerSweepChannel. (OPTIONAL, default=1) 
 * IN        Indicates channel number.  
 *
 * PARAM 6 : ViPReal64 getStartPower. (OPTIONAL) 
 * OUT       When 'powerSweepRequest' is a query, returns Start power level.
 *
 * PARAM 7 : ViPReal64 getStopPower. (OPTIONAL) 
 * OUT       When 'powerSweepRequest' is a query, returns Stop power level.  
 *
 *           
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_powerSweep( 
        ViSession vi, 
		ViString powerSweepRequest,
		ViReal64 setStartPower,
		ViReal64 setStopPower,
		ViInt16 powerSweepChannel,
		ViPReal64 getStartPower,
		ViPReal64 getStopPower)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 choice = 0;
	ViInt32 currentChannel = 0;
	const ViInt32 size = 6;
	ViString req[]= {"ON", "OFF", "S/S", "START/STOP", "S/S?", "START/STOP?" };


	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_powerSweep" );

	/*  Parameter checks */
	ag875x_CHK_PTR(powerSweepRequest, VI_ERROR_PARAMETER2);
	ag875x_CHK_INT_RANGE(powerSweepChannel,ag875x_CH1,ag875x_CH4,VI_ERROR_PARAMETER5);

	if(getChoice(vi,powerSweepRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	// Save the current active channel number, so it may be restored later.
	if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	
	// Temporarily change to requested channel.
	if ((errStatus = ag875x_channelSelect(vi, powerSweepChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	switch (choice)
		{
		case 0: //Set power sweep on
			errStatus = viPrintf(vi, "POWS;\n");
			break;

		case 1:  //Set power sweep off
			errStatus = viPrintf(vi, "CWTIME;\n");
			break;

		case 2: case 3://Set Power Sweep Start/Stop
			if ((errStatus = viPrintf(vi, "POWS;STAR %lf;\n",setStartPower)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			errStatus = viPrintf(vi, "STOP %lf;\n",setStopPower);
			break;

		case 4: case 5: //Read Power Start/Stop
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viQueryf(vi, "STAR?;\n","%lf",getStartPower)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			errStatus = viQueryf(vi, "STOP?;\n","%lf",getStopPower);
			break;
		}

	if (errStatus < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Restore instrument to original channel number.
	if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_markerSet
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Activates a specific marker, or performs an instrument function
 *           related to that marker.
 *
 *			 NOTE:  Upon exiting, this function will always leave the specified
 *			 marker as the activer marker, regardless of the value of request.
 *            
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *            
 * PARAM 2 : ViString markerRequest. Choices are ON, SET, POINT, MIN, MAX, TARGET, 
 * IN        etc. (see code below). 
 *
 * PARAM 3 : ViInt16 setMarker. 
 * IN        Contains the marker number  (1-5)  
 *
 * PARAM 4 : ViReal64 setMarkerValue. (OPTIONAL) 
 * IN        Depending upon 'request', this parameter will contain one of the
 *           following values to be set for a marker or marker function:
 *           Frequency, (or power if in power sweep, time if in CW Time sweep mode), 
 *           point number, target, widths value, fixed marker stimulus,
 *           fixed marker value or fixed marker aux value.
 *
 * PARAM 5 : ViInt16 markerChannel. (OPTIONAL, default=1) 
 * IN        Indicates channel number.   
 *
 *           
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_markerSet( 
        ViSession vi, 
		ViString markerRequest,
		ViInt16 setMarker,
		ViReal64 setMarkerValue,
		ViInt16 markerChannel)
	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 choice = 0;
	const ViInt32 size = 48;
	ViInt16 instMarkerNumber = 0;		// actual marker number passed to instrument
	ViInt32 currentChannel = 0;

	ViString req[]= {"ON", "SET", "STIM", "STIMULUS", "POINT", "MIN", "MINIMUM", 
						 "MAX", "MAXIMUM", "SEARCH LEFT", "SEARCH RIGHT", "SEARCH OFF",
						 "WIDTHS", "WIDTHS OFF", "TRACK ON", "TRACK OFF", "DELTA TO MKR", 
						 "DELTA TO FIXED", "DELTA ZERO", "DELTA OFF", "STATS ON", "STATS OFF", 
						 "CONTINUOUS", "DISCRETE", "COUPLED", "UNCOUPLED", "START TO MKR", 
						 "STOP TO MKR", "CENTER TO MKR", "SPAN TO MKR", "CW TO MKR", "REF TO MKR",
						 "REFERENCE TO MKR", "DELAY TO MKR", "FIXED MKR STIM", "FIXED MKR VALUE", 
						 "FIXED MKR AUX VALUE", "ANNOTATE ALL MKRS", "ANNOTATE ONE MKR", "POLAR LIN MAG",
						 "POLAR LOG MAG", "POLAR RI", "SMITH LIN MAG", "SMITH LOG MAG", "SMITH RI",
						 "SMITH IMPEDANCE", "SMITH ADMITTANCE", "ALL OFF"
						};


	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_markerSet" );

	/*  Parameter checks */
	ag875x_CHK_PTR(markerRequest, VI_ERROR_PARAMETER2);
	ag875x_CHK_ENUM(setMarker, ag875x_MKR_5, VI_ERROR_PARAMETER3);
	ag875x_CHK_ENUM(markerChannel, ag875x_CH4, VI_ERROR_PARAMETER5);

	// Marker enums start at 0, so increment by one for the instrument.
	instMarkerNumber = setMarker +1;

	if(getChoice(vi,markerRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	// Save the current active channel number, so it may be restored later.
	if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Temporarily change to requested channel.
	if ((errStatus = ag875x_channelSelect(vi, markerChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	if ((errStatus = viPrintf(vi, "MARK%i;\n",instMarkerNumber)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	switch (choice)
		{
		case 0: // SIMPLY ACTIVATE MARKER, already done
			break;
		case 1: case 2: case 3: // ACTIVATE MARKER, SET TO A SPECIFIC STIMULUS VALUE	
	  		errStatus = viPrintf(vi, "MARK%i %lf;\n",instMarkerNumber ,setMarkerValue);
			break;
		case 4: // ACTIVATE MARKER, SET TO SPECIFIC POINT NUMBER ON TRACE
	  		errStatus = viPrintf(vi, "MARKBUCK%lf;\n",setMarkerValue-1);
			break;
		case 5: case 6: // SEARCH MIN
	  		errStatus = viPrintf(vi, "SEAMIN;\n");
			break;
		case 7: case 8: // SEARCH MAX
	   		errStatus = viPrintf(vi, "SEAMAX;\n");
			break;
		case 9: // SEARCH TARGET LEFT
			errStatus = viPrintf(vi, "SEATARG %lf;SEAL;\n",setMarkerValue);
			break;
		case 10: // SEARCH TARGET RIGHT
			errStatus = viPrintf(vi, "SEATARG %lf;SEAR;\n",setMarkerValue);
			break;
		case 11: // SEARCH OFF
			errStatus = viPrintf(vi, "SEAOFF;\n");
			break;
		case 12: // WIDTHS 
			errStatus = viPrintf(vi, "WIDV %lf;WIDTON;\n",setMarkerValue);
			break;
		case 13: // WIDTHS OFF
			errStatus = viPrintf(vi, "WIDTOFF;\n");
			break;
		case 14: // TRACK ON
			errStatus = viPrintf(vi, "TRACKON;\n");
			break;
		case 15: // TRACK OFF
			errStatus = viPrintf(vi, "TRACKOFF;\n");
			break;
		case 16: // DELTA REF = MARKER x
			errStatus = viPrintf(vi, "DELR%i;\n",instMarkerNumber);
			break;
		case 17: // DELTA REF = FIXED MARKER
			errStatus = viPrintf(vi, "DELRFIXM;\n");
			break;
		case 18: // DELTA REF = FIXED MARKER = ACTIVE MARKER (i.e., DELTA ZERO)
			errStatus = viPrintf(vi, "MARKZERO;\n");
			break;
		case 19: // DELTA OFF
			errStatus = viPrintf(vi, "DELO;\n");
			break;
		case 20: // Marker Statistics ON
			errStatus = viPrintf(vi, "MEASTATON;\n");
			break;
		case 21: // Marker Statistics OFF
			errStatus = viPrintf(vi, "MEASTATOFF;\n");
			break;
		case 22: // CONTINUOUS marker mode
			errStatus = viPrintf(vi, "MARKCONT;\n");
			break;
		case 23: // DISCRETE marker mode
			errStatus = viPrintf(vi, "MARKDISC;\n");
			break;
		case 24: // Markers stimulus-coupled between channels 1 and 2
			errStatus = viPrintf(vi, "MARKCOUP;\n");
			break;
		case 25: // Markers not stimulus-coupled between channels 1 and 2
			errStatus = viPrintf(vi, "MARKUNCO;\n");
			break;
		case 26: // Start Stimulus to Active Marker
			errStatus = viPrintf(vi, "MARKSTAR;\n");
			break;
		case 27: // Stop Stimulus to Active Marker
			errStatus = viPrintf(vi, "MARKSTOP;\n");
			break;
		case 28: // Center Stimulus to Active Marker
			errStatus = viPrintf(vi, "MARKCENT;\n");
			break;
		case 29: // Stimulus Span to Span between Active Marker and Delta Ref Marker
			errStatus = viPrintf(vi, "MARKSPAN;\n");
			break;
		case 30: // CW Frequency to Active Marker
			errStatus = viPrintf(vi, "MARKCW;\n");
			break;
		case 31: case 32: // Reference Value to Active Marker
			errStatus = viPrintf(vi, "MARKREF;\n");
			break;
		case 33: // Elec Delay to Zero Group Delay at Active Marker
			errStatus = viPrintf(vi, "MARKDELA;\n");
			break;
		case 34: // Stimulus of Fixed Marker
			errStatus = viPrintf(vi, "MARKFSTI %lf;\n",setMarkerValue);
			break;
		case 35: // Value of Fixed Marker
			errStatus = viPrintf(vi, "MARKFVAL %lf;\n",setMarkerValue);
			break;
		case 36: // Aux value of Fixed Marker
			errStatus = viPrintf(vi, "MARKFAUV %lf;\n",setMarkerValue);
			break;
		case 37: // Display data for all markers
			errStatus = viPrintf(vi, "DISMON;\n");
			break;
		case 38: // Display data for only the active marker
			errStatus = viPrintf(vi, "DISMOFF;\n");
			break;
		case 39: // LIN MAG/PHASE MARKERS WHEN POLAR DISPLAY
			errStatus = viPrintf(vi, "POLMLIN;\n");
			break;
		case 40: // LOG MAG/PHASE MARKERS WHEN POLAR DISPLAY
			errStatus = viPrintf(vi, "POLMLOG;\n");
			break;
		case 41: // REAL/IMAGINARY MARKERS WHEN POLAR DISPLAY
			errStatus = viPrintf(vi, "POLMRI;\n");
			break;
		case 42: // LIN MAG/PHASE MARKERS WHEN SMITH CHART DISPLAY
			errStatus = viPrintf(vi, "SMIMLIN;\n");
			break;
		case 43: // LOG MAG/PHASE MARKERS WHEN SMITH CHART DISPLAY
			errStatus = viPrintf(vi, "SMIMLOG;\n");
			break;
		case 44: // REAL/IMAGINARY MARKERS WHEN SMITH CHART DISPLAY
			errStatus = viPrintf(vi, "SMIMRI;\n");
			break;
		case 45: // IMPEDANCE MARKERS WHEN SMITH CHART DISPLAY
			errStatus = viPrintf(vi, "SMIMRX;\n");
			break;
		case 46: // ADMITTANCE MARKERS WHEN SMITH CHART DISPLAY
			errStatus = viPrintf(vi, "SMIMGB;\n");
			break;
		case 47: // ALL OFF
			errStatus = viPrintf(vi, "MARKOFF;\n");
			break;
		}

	if (errStatus < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Restore instrument to original channel number.
	if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_markerQuery
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  Depending upon 'markerQueryRequest', reads one of the following:
 *            -> Values associated with the selected marker
 *            -> Settings affecting all markers 
 *
 *			 NOTE:  Upon exiting, this function will always leave the specified
 *			 marker as the activer marker, regardless of the value of request.
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *
 * PARAM 2-4: ViPReal64 markerQueryResult1, markerQueryResult2, & markerQueryResult3.
 * OUT        Depending upon 'markerQueryRequest', these parameters return values 
 *            associated with the selected marker, or all markers.
 *
 * PARAM 5 : ViString markerQueryRequest.    
 * IN        See code below for choices. 
 *
 * PARAM 6 : ViInt16 getMarker. 
 * IN        Contains the marker number (1-5), used for certain values
 *           of 'request'.
 *
 * PARAM 7 : ViInt16 markerQueryChannel. (OPTIONAL; default=1)
 * IN        Indicates channel number.   
 *
 * PARAM 8 : ViPReal64 markerQueryResult4. (OPTIONAL)
 * OUT       When 'request' is a WIDTHS query, this parameter returns the loss  
 *           value.
 *
 *           
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_markerQuery( 
        ViSession vi, 
		ViPReal64 markerQueryResult1,
		ViPReal64 markerQueryResult2,
		ViPReal64 markerQueryResult3,
		ViString markerQueryRequest,
		ViInt16 getMarker,
		ViInt16 markerQueryChannel,
		ViPReal64 markerQueryResult4)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 choice = 0;
	ViInt32 iReply = 0;
	ViInt32 iMarkNum = 0;
	const ViInt32 size = 19;
	ViInt16 instMarkerNumber = 0;		// actual marker number passed to instrument
	ViInt32 currentChannel = 0;

	ViString req[]= {"VALUE STIM?", "POINT?", "SEARCH MODE?", "TARGET?", "SEARCH WIDTH?", 
						 "WIDTHS RESULTS?", "TRACK ON?", "DELTA MODE?", "STATS ON?", "STATS RESULTS?", 
						 "CONTINUOUS?", "COUPLED?", "FIXED MKR STIM?", "FIXED MKR VALUE?", 
						 "FIXED MKR AUX VALUE?", "ANNOTATE ALL MKRS?", "POLAR FORMAT?", "SMITH FORMAT?", 
						 "ALL OFF?"
						};


	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_markerQuery" );

	/*  Parameter checks */
	ag875x_CHK_PTR(markerQueryResult1, VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(markerQueryResult2, VI_ERROR_PARAMETER3);
	ag875x_CHK_PTR(markerQueryResult3, VI_ERROR_PARAMETER4);
	ag875x_CHK_PTR(markerQueryRequest, VI_ERROR_PARAMETER5);
	ag875x_CHK_INT_RANGE(getMarker,ag875x_MKR_1,ag875x_MKR_5,VI_ERROR_PARAMETER6);
	ag875x_CHK_INT_RANGE(markerQueryChannel,ag875x_CH1,ag875x_CH4,VI_ERROR_PARAMETER7);

	// Marker enums start at 0, so increment by one for the instrument.
	instMarkerNumber = getMarker +1;

	if(getChoice(vi,markerQueryRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	// Save the current active channel number, so it may be restored later.
	if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Temporarily change to requested channel.
	if ((errStatus = ag875x_channelSelect(vi, markerQueryChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// If request is to query marker on/off status, don't turn marker on!
	if (choice != 18)
		if ((errStatus = viPrintf(vi, "MARK%i;\n", instMarkerNumber)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	switch (choice)
		{
		case 0: // READ AMPLITUDE AND STIMULUS OF SELECTED MARKER
			if ((errStatus = viQueryf(vi, "OUTPMARK;\n","%lf,%lf,%lf", markerQueryResult1, markerQueryResult2, 
				markerQueryResult3)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 1: // TRACE POINT ASSOCIATED WITH SELECTED MARKER
			if ((errStatus = viQueryf(vi, "MARKBUCK?;\n","%lf",markerQueryResult1)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	  		(*markerQueryResult1)++;
			break;

		case 2: // SEARCH MODE

			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viQueryf(vi, "SEAMAX?;\n","%i",&iReply)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	  		if (iReply)
				{
				*markerQueryResult1 = ag875x_MKR_SEARCH_IS_MAX;
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				// Restore instrument to original channel number
				if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
					ag875x_LOG_STATUS(vi, thisPtr, errStatus);
				return statusUpdate(vi, thisPtr, VI_SUCCESS);
				}
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viQueryf(vi, "SEAMIN?;\n","%i",&iReply)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	  		if (iReply)
				{
				*markerQueryResult1 = ag875x_MKR_SEARCH_IS_MIN;
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				// Restore instrument to original channel number.
				if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
					ag875x_LOG_STATUS(vi, thisPtr, errStatus);
				return statusUpdate(vi, thisPtr, VI_SUCCESS);
				}
			*markerQueryResult1 = ag875x_MKR_SEARCH_IS_TARGET;
			break;

		case 3: // TARGET VALUE	
			if ((errStatus = viQueryf(vi, "SEATARG?;\n","%lf",markerQueryResult1)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 4: // WIDTH FOR SEARCH
			if ((errStatus = viQueryf(vi, "WIDV?;\n","%lf",markerQueryResult1)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 5: // GET WIDTHS SEARCH RESULTS
			if ((errStatus = viQueryf(vi, "OUTPMWIL;\n","%lf,%lf,%lf,%lf", markerQueryResult1, markerQueryResult2,
										markerQueryResult3, markerQueryResult4)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 6: // TRACKING ON/OFF STATUS
			if ((errStatus = viQueryf(vi, "TRACK?;\n","%lf",markerQueryResult1)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 7: // DELTA MODE STATUS
			if ((errStatus = viQueryf(vi, "DELO?;\n","%i",&iReply)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	  		if (iReply)
				{
				*markerQueryResult1 = ag875x_MKR_DELTA_IS_OFF;
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				// Restore instrument to original channel number.
				if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
					ag875x_LOG_STATUS(vi, thisPtr, errStatus);
				return statusUpdate(vi, thisPtr, VI_SUCCESS);
				}
			for (iMarkNum = 1; iMarkNum <= 5; iMarkNum++)
				{
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				if ((errStatus = viQueryf(vi, "DELR%i?;\n","%i",iMarkNum,&iReply)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				if (iReply)
					{
					*markerQueryResult1 = (ViReal64)iMarkNum;
					if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );
					// Restore instrument to original channel number.
					if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
						ag875x_LOG_STATUS(vi, thisPtr, errStatus);
					return statusUpdate(vi, thisPtr, VI_SUCCESS);
					}
				}
			*markerQueryResult1 = ag875x_MKR_DELTA_IS_FIXED;
			break;

		case 8: // Marker Statistics ON/OFF status
			if ((errStatus = viQueryf(vi, "MEASTAT?;\n","%lf",markerQueryResult1)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 9: // GET STATISTICS RESULTS
			if ((errStatus = viQueryf(vi, "OUTPMSTA;\n","%lf,%lf,%lf", markerQueryResult1, markerQueryResult2, 
				markerQueryResult3)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 10: // CONTINUOUS/DISCRETE marker mode status (0 = DISCRETE)
			if ((errStatus = viQueryf(vi, "MARKCONT?;\n","%lf",markerQueryResult1)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 11: // ON/OFF status of marker stimulus-coupling between channels
			if ((errStatus = viQueryf(vi, "MARKCOUP?;\n","%lf",markerQueryResult1)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 12: // Stimulus of Fixed Marker
			if ((errStatus = viQueryf(vi, "MARKFSTI?;\n","%lf",markerQueryResult1)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 13: // Value of Fixed Marker
			if ((errStatus = viQueryf(vi, "MARKFVAL?;\n","%lf",markerQueryResult1)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 14: // Aux value of Fixed Marker
			if ((errStatus = viQueryf(vi, "MARKFAUV?;\n","%lf",markerQueryResult1)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 15: // STATUS OF MARKER ANNOTATIONS
			if ((errStatus = viQueryf(vi, "DISM?;\n","%lf",markerQueryResult1)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 16: // POLAR MARKER FORMATTING
			if ((errStatus = viQueryf(vi, "POLMLIN?;\n","%i",&iReply)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	  		if (iReply)
				{
				*markerQueryResult1 = ag875x_MKR_POLAR_IS_LIN_MAG;
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				// Restore instrument to original channel number.
				if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
					ag875x_LOG_STATUS(vi, thisPtr, errStatus);
				return statusUpdate(vi, thisPtr, VI_SUCCESS);
				}
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viQueryf(vi, "POLMLOG?;\n","%i",&iReply)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	  		if (iReply)
				{
				*markerQueryResult1 = ag875x_MKR_POLAR_IS_LOG_MAG;
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				// Restore instrument to original channel number.
				if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
					ag875x_LOG_STATUS(vi, thisPtr, errStatus);
				return statusUpdate(vi, thisPtr, VI_SUCCESS);
				}
			*markerQueryResult1 = ag875x_MKR_POLAR_IS_RI;
			break;

		case 17: // SMITH CHART MARKER FORMATTING
			if ((errStatus = viQueryf(vi, "SMIMLIN?;\n","%i",&iReply)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	  		if (iReply)
				{
				*markerQueryResult1 = ag875x_MKR_SMITH_IS_LIN_MAG;
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				// Restore instrument to original channel number.
				if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
					ag875x_LOG_STATUS(vi, thisPtr, errStatus);
				return statusUpdate(vi, thisPtr, VI_SUCCESS);
				}
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viQueryf(vi, "SMIMLOG?;\n","%i",&iReply)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	  		if (iReply)
				{
				*markerQueryResult1 = ag875x_MKR_SMITH_IS_LOG_MAG;
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				// Restore instrument to original channel number.
				if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
					ag875x_LOG_STATUS(vi, thisPtr, errStatus);
				return statusUpdate(vi, thisPtr, VI_SUCCESS);
				}
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viQueryf(vi, "SMIMRI?;\n","%i",&iReply)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	  		if (iReply)
				{
				*markerQueryResult1 = ag875x_MKR_SMITH_IS_RI;
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				// Restore instrument to original channel number.
				if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
					ag875x_LOG_STATUS(vi, thisPtr, errStatus);
				return statusUpdate(vi, thisPtr, VI_SUCCESS);
				}
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viQueryf(vi, "SMIMRX?;\n","%i",&iReply)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	  		if (iReply)
				{
				*markerQueryResult1 = ag875x_MKR_SMITH_IS_IMPEDANCE;
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				// Restore instrument to original channel number.
				if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
					ag875x_LOG_STATUS(vi, thisPtr, errStatus);
				return statusUpdate(vi, thisPtr, VI_SUCCESS);
				}
			*markerQueryResult1 = ag875x_MKR_SMITH_IS_ADMITTANCE;
			break;

		case 18: // ARE ALL MARKERS TURNED OFF?
			if ((errStatus = viQueryf(vi, "MARKOFF?;\n","%lf",markerQueryResult1)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;
		} /* end switch */

	// Restore instrument to original channel number.
	if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_limitTableSet
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Sets actual limits lines or points, and displays them.
 *			 
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *
 * PARAM 2 : ViInt16 limitTableSetSegments.
 * IN        Contains the total number of segments to be set.  Range is from 1 to 18.  
 *           Make sure that the limit array has at least this number of segments specified.
 *
 * PARAM 3 : ViReal64 _VI_FAR limitTableSetLimits[].  64 bit Real one-dimensional array for limits. 
 * IN        (1 to 18 segments; 4 input values per segment) in the following format:
 *           [ 0|1|2, Stimulus, Upper Limit (or Middle), Lower Limit (or Delta) ]  
 *           where 0/1/2 is 0 for Sloping Line, 1 = Flat Line and 2 = Single Point.
 *           Repeated X number of times within the array where X equals the number of 
 *           segments.  A "sentinel" value of 9999 can be input as the stimulus value of 
 *           one or more segments to indicate that the stimulus of the active marker (if
 *           present) should be used as the stimulus for those segments.  Similarly,
 *           the same sentinel value can be input as the middle value of one or more
 *           segments to indicate that the amplitude of the active marker should be
 *           used as the middle value for those segments.
 * 
 * PARAM 4 : ViString limitTableSetRequest. (OPTIONAL, default=ag875x_LIM_TABLE_UPPER_LOWER).   
 * IN        Indicates whether the values to be set are upper and lower limits
 *           (default) or middle and delta limits (ag875x_LIM_TABLE_MIDDLE_DELTA). 
 *
 * PARAM 5 : ViInt16 limitTableSetChannel. (OPTIONAL; default=ag875x_CH1)
 * IN        Indicates channel number.   
 *
 * PARAM 6 : ViBoolean limitTableSetAppendMode. (OPTIONAL; default=ag875x_CLEAR_EXISTING_TABLE)
 * IN        Indicates whether to append these limit segments to the existing limit
 *           table if one exists (ag875x_APPEND_TO_EXISTING_TABLE) or whether to 
 *           clear the existing table first (ag875x_CLEAR_EXISTING_TABLE).
 *
 *
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *-----------------------------------------------------------------------------
 */
 ViStatus _VI_FUNC ag875x_limitTableSet(
		ViSession vi,
		ViInt16 limitTableSetSegments,
		ViReal64 _VI_FAR limitTableSetLimits[],
		ViString limitTableSetRequest,
		ViInt16 limitTableSetChannel,
		ViBoolean limitTableSetAppendMode)
 
	{
	 struct ag875x_globals *thisPtr;
	 ViStatus errStatus = VI_SUCCESS;	 
	 ViInt32 choice = 0;
	 ViInt32 seg = 0;
	 ViInt32 elem = 0;
	 ViInt32 limnum = 0;
	 ViInt32 currentChannel = 0;
	 ViString l_type[3]= {"SL", "FL", "SP"};
	 const ViInt32 size = 2;

	 ViString req[]= {"UPPER/LOWER", "MIDDLE/DELTA"};
	 
	 GET_GLOBALS(vi, thisPtr)
	 ag875x_CDE_INIT( "ag875x_limitTableSet" );

	 /*  Parameter checks */

	 ag875x_CHK_INT_RANGE(limitTableSetSegments,ag875x_LIM_TABLE_MIN_SEGS,ag875x_LIM_TABLE_MAX_SEGS,VI_ERROR_PARAMETER2);
	 ag875x_CHK_PTR(limitTableSetLimits, VI_ERROR_PARAMETER3);
	 ag875x_CHK_PTR(limitTableSetRequest, VI_ERROR_PARAMETER4);
	 ag875x_CHK_ENUM(limitTableSetChannel, ag875x_CH4, VI_ERROR_PARAMETER5);
	 ag875x_CHK_BOOLEAN(limitTableSetAppendMode, VI_ERROR_PARAMETER6);

	 if(getChoice(vi,limitTableSetRequest,size,req,&choice)) 
		 return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	// Save the current active channel number, so it may be restored later.
	if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	 	 
	 // Temporarily change to requested channel.
	 if ((errStatus = ag875x_channelSelect(vi, limitTableSetChannel)) < VI_SUCCESS)
		 ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	 
	 // Begin editing
	 if ((errStatus = viPrintf(vi, "EDITLIML;\n")) < VI_SUCCESS)
		 return statusUpdate(vi, thisPtr, errStatus);
	 
	 if (!limitTableSetAppendMode)
		{
		// Erase existing limit table if append = FALSE
		if ((errStatus = viPrintf(vi, "CLEL;\n")) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);
		}
	 
	 for (seg=1;seg<=limitTableSetSegments;seg++)  
		{
		 elem = seg-1;
		 limnum= (int) limitTableSetLimits[elem*4];
		 
		 if ((limnum < 0)||(limnum > 2))
			 return statusUpdate(vi, thisPtr, ag875x_LIMITYPE_ERROR);
		 if ((errStatus = viPrintf(vi, "SADD;LIMT%s;\n",l_type[limnum])) < VI_SUCCESS)
			 return statusUpdate(vi, thisPtr, errStatus);
		 if (limitTableSetLimits[elem*4+1] == 9999)
			{
			if ((errStatus = viPrintf(vi, "MARKSTIM;\n")) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			}
		 else
			{
			if ((errStatus = viPrintf(vi, "LIMS%lf;\n",limitTableSetLimits[elem*4+1])) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			}
		 
		 switch (choice)
			{
			case 0:
				if ((errStatus = viPrintf(vi, "LIMU%lf;\n",limitTableSetLimits[elem*4+2])) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				if ((errStatus = viPrintf(vi, "LIML%lf;\n",limitTableSetLimits[elem*4+3])) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				break;

			case 1:
				if (limitTableSetLimits[elem*4+2] == 9999)
					{
					if ((errStatus = viPrintf(vi, "MARKMIDD;\n")) < VI_SUCCESS)
						return statusUpdate(vi, thisPtr, errStatus);
					}
				else
					{
					if ((errStatus = viPrintf(vi, "LIMM%lf;\n",limitTableSetLimits[elem*4+2])) < VI_SUCCESS)
						return statusUpdate(vi, thisPtr, errStatus);
					}
				if ((errStatus = viPrintf(vi, "LIMD%lf;\n",limitTableSetLimits[elem*4+3])) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				break;
		 } /* end switch */
		 
		if ((errStatus = viPrintf(vi, "SDON;\n")) < VI_SUCCESS)
			 return statusUpdate(vi, thisPtr, errStatus);
		}
	 
	//Finished editing; turn on display of limit lines
	if ((errStatus = viPrintf(vi, "EDITDONE;LIMILINEON;\n")) < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);
	 
	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Restore instrument to original channel number.
	if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_limitStatusSet
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Sets various limit line status parameters.
 *           To set actual limits see ag875x_limitTableSet.
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *            
 * PARAM 2 : ViString limitStatusSetRequest. (OPTIONAL; default="TEST ON")
 * IN        See code below for choices.
 *
 * PARAM 3 : ViInt16 limitStatusSetChannel. (OPTIONAL; default=1)
 * IN        Indicates channel number.   
 *
 * PARAM 4 : ViReal64 limitStatusSetOffset. (OPTIONAL)
 * IN        If 'limitStatusSetRequest' is to set a limit line offset value 
 *			 (stimulus or amplitude), this parameter is meant to contain 
 *			 the offset value to be set.
 * 
 * 
 * RETURN  :  VI_SUCCESS:No error. Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_limitStatusSet( 
		ViSession vi, 
		ViString limitStatusSetRequest,
		ViInt16 limitStatusSetChannel,
		ViReal64 limitStatusSetOffset)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 choice = 0;
	ViInt32 currentChannel = 0;
	const ViInt32 size = 7;
	
	ViString req[]= {"TEST ON", "TEST OFF", "DISPLAY ON", "DISPLAY OFF", 
						 "STIM OFFSET", "AMPL OFFSET", "MKR AMPL OFFSET"
						}; 

	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_limitStatusSet" );

	/*  Parameter checks */
	ag875x_CHK_PTR(limitStatusSetRequest, VI_ERROR_PARAMETER2);
	ag875x_CHK_ENUM(limitStatusSetChannel, ag875x_CH4,VI_ERROR_PARAMETER3);

	if(getChoice(vi,limitStatusSetRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	// Save the current active channel number, so it may be restored later.
	if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Temporarily change to requested channel.
	if ((errStatus = ag875x_channelSelect(vi, limitStatusSetChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	switch (choice)
		{
		case 0: // Limit test ON
			if ((errStatus = viPrintf(vi, "LIMITESTON;\n")) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 1: // Limit test OFF
			if ((errStatus = viPrintf(vi, "LIMITESTOFF;\n")) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 2: // Limit line display ON
			if ((errStatus = viPrintf(vi, "LIMILINEON;\n")) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 3: // Limit line display OFF
			if ((errStatus = viPrintf(vi, "LIMILINEOFF;\n")) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 4: // Set stimulus offset for limit lines
			if ((errStatus = viPrintf(vi, "LIMISTIO%lf;\n",limitStatusSetOffset)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 5: // Set amplitude offset for limit lines
			if ((errStatus = viPrintf(vi, "LIMIAMPO%lf;\n",limitStatusSetOffset)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 6: // Center limit lines around the amplitude of the active marker (amplitude offset)
			if ((errStatus = viPrintf(vi, "LIMIMAOF;\n")) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;
		}

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Restore instrument to original channel number.
	if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_limitStatusQuery
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Reads various limit line status parameters, or reads
 *			 pass/fail results from limit test.  
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *            
 * PARAM 2 : ViInt16 limitStatusQueryArrSize
 * IN        This parameter is meant to contain the size of the array (in terms of
 *           64-bit values) being passed into this function as 'limitStatusQueryResult'.  
 * 
 * PARAM 3 : ViReal64 _VI_FAR limitStatusQueryResult[].  64 bit Real one-dimensional array 
 * OUT		 to receive the limit-related values.
 *
 * PARAM 4 : ViPInt16 limitStatusQueryNumValues
 * OUT       The number of individual data values that were read from the 
 *           instrument into 'limitStatusQueryResult'.
 *
 * PARAM 5 : ViString limitStatusQueryRequest. (OPTIONAL; default="CHANNEL STATUS?")
 * IN        See code below for choices.
 *
 * PARAM 6 : ViInt16 limitStatusQueryChannel. (OPTIONAL; default=1)
 * IN        Indicates channel number.   
 *
 * PARAM 7 : ViInt16 limitStatusQuerySegNum. (OPTIONAL; default=1)
 * IN        This parameter is used to indicate the limit segment number when 
 *           'request' equals "SEG STATUS?" or "SEG MIN MAX?".
 *
 * 
 * RETURN  :  VI_SUCCESS:No error. Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *
 * Note:     When testing limit condition (Pass/Fail), Returns -1 if no test,
 *			 0 for fail and 1 for pass. 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_limitStatusQuery( 
        ViSession vi, 
		ViInt16 limitStatusQueryArrSize,
		ViReal64 _VI_FAR limitStatusQueryResult[],
		ViPInt16 limitStatusQueryNumValues,
		ViString limitStatusQueryRequest,
		ViInt16 limitStatusQueryChannel,
		ViInt16 limitStatusQuerySegNum)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 choice = 0;
	ViInt32 currentChannel = 0;
	ViInt16 inc = 0;
	ViInt16 iNumSegs = 0;
	ViInt16 iNumFails = 0;
	ViInt16 iSegNum = 0;
	ViInt16 iLimStat = 0;
	ViInt16 iNumPts = 0;
	ViInt16 iNumVals = 0;
	const ViInt32 size = 14;
	ViInt16 iNumOfSegs = 0;
	ViString tempString = "";	// Need for absorbing a comma while reading arrays

	ViString req[]= {"TEST?", "DISPLAY?", "STIM OFFSET?", "AMPL OFFSET?", 
						 "CHANNEL STATUS?", "SEG STATUS?", "ALL SEGS STATUS?", 
						 "FAIL POINT RESULTS?", "ALL POINT RESULTS?", "MKR RESULTS?", 
						 "ALL SEGS MIN VAL?", "ALL SEGS MAX VAL?", "SEG MIN MAX?", 
						 "ALL MIN MAX?"
						};
 
	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_limitStatusQuery" );

	/*  Parameter checks */
	ag875x_CHK_PTR(limitStatusQueryResult, VI_ERROR_PARAMETER3);
	ag875x_CHK_PTR(limitStatusQueryNumValues, VI_ERROR_PARAMETER4);
	ag875x_CHK_PTR(limitStatusQueryRequest, VI_ERROR_PARAMETER5);
	ag875x_CHK_ENUM(limitStatusQueryChannel, ag875x_CH4,VI_ERROR_PARAMETER6);

	if(getChoice(vi,limitStatusQueryRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	// Save the current active channel number, so it may be restored later.
	if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	
	// Temporarily change to requested channel.
	if ((errStatus = ag875x_channelSelect(vi, limitStatusQueryChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	
	// First get number of segments in the Limit Table.
	// If 0 segments, then Limit Table is empty.
	if ((errStatus = viQueryf(vi,"SEDI?;\n","%i", &iNumOfSegs)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				
	// Need to flush the buffer after SEDI?, as it returns a float
	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	switch (choice)
		{
		case 0: // Query ON/OFF status of test
			if ((errStatus = viQueryf(vi, "LIMITEST?;\n","%hd",&iLimStat)) < VI_SUCCESS)
				ag875x_LOG_STATUS(vi, thisPtr, errStatus);
    		*limitStatusQueryNumValues = 1;

			limitStatusQueryResult[0] = (ViReal64)iLimStat;
			break;

		case 1: // Query ON/OFF status of limit line display
			if ((errStatus = viQueryf(vi, "LIMILINE?;\n","%hd",&iLimStat)) < VI_SUCCESS)
				ag875x_LOG_STATUS(vi, thisPtr, errStatus);
    		*limitStatusQueryNumValues = 1;

			limitStatusQueryResult[0] = (ViReal64)iLimStat;
			break;

		case 2: // Query limit line stimulus offset
			if ((errStatus = viQueryf(vi, "LIMISTIO?;\n","%lf",&limitStatusQueryResult[0])) < VI_SUCCESS)
				ag875x_LOG_STATUS(vi, thisPtr, errStatus);
    		*limitStatusQueryNumValues = 1;
			break;

		case 3: // Query limit line amplitude offset
			if ((errStatus = viQueryf(vi, "LIMIAMPO?;\n","%lf",&limitStatusQueryResult[0])) < VI_SUCCESS)
				ag875x_LOG_STATUS(vi, thisPtr, errStatus);
    		*limitStatusQueryNumValues = 1;
			break;

		case 4: // Output limit test PASS/FAIL status for specified channel
			// Channel enum starts with 0, so need to increment by one
			if ((errStatus =viQueryf(vi,"OUTPLIM%i;\n","%hd",limitStatusQueryChannel + 1,&iLimStat)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
    		*limitStatusQueryNumValues = 1;

			limitStatusQueryResult[0] = (ViReal64)iLimStat;
			break;

		case 5: // Output limit test PASS/FAIL status for specified segment
			if ((errStatus =viQueryf(vi,"SELSEG%i;OUTPSEGF;\n","%hd",limitStatusQuerySegNum,&iLimStat)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			*limitStatusQueryNumValues = 1;

			limitStatusQueryResult[0] = (ViReal64)iLimStat;
			break;

		case 6: // Output the segment number and it's PASS/FAIL status for all active segments

			// Check if Limit Test is on
			iLimStat = 0;
			if ((errStatus = viQueryf(vi, "LIMITEST?;\n","%hd",&iLimStat)) < VI_SUCCESS)
				ag875x_LOG_STATUS(vi, thisPtr, errStatus);
			if (iLimStat == 0)
				return ag875x_LIMIT_TEST_OFF;
			
			// Check if Limit Table is empty
			if (iNumOfSegs == 0)
				return ag875x_LIMIT_TABLE_EMPTY;

			if ((errStatus =viQueryf(vi,"OUTPSEGAF;\n","%hd",&iNumSegs)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			*limitStatusQueryNumValues = iNumSegs * 2;

			if (*limitStatusQueryNumValues > limitStatusQueryArrSize)
				ag875x_LOG_STATUS( vi, thisPtr, ag875x_ARRAY_TOO_SMALL );

			for (inc=0; inc<(*limitStatusQueryNumValues); inc+=2)
				{
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if ((errStatus = viScanf(vi, "%hd%s%hd", &iSegNum, &tempString, &iLimStat)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				limitStatusQueryResult[inc] = (ViReal64)iSegNum;
				limitStatusQueryResult[inc+1] = (ViReal64)iLimStat;
				}
			break;

		case 7: // Output the limit test results for each failed point
			if ((errStatus =viQueryf(vi,"OUTPFAIP;\n","%hd",&iNumFails)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			*limitStatusQueryNumValues = iNumFails * 2;

			if (*limitStatusQueryNumValues > limitStatusQueryArrSize)
				ag875x_LOG_STATUS( vi, thisPtr, ag875x_ARRAY_TOO_SMALL );

			for (inc=0; inc<(*limitStatusQueryNumValues); inc+=2)
				{
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if ((errStatus = viScanf(vi, "%,2lf", &limitStatusQueryResult[inc], 
									&limitStatusQueryResult[inc+1])) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				}
			break;

		case 8: // Output the limit test results for each point in the sweep
			
			if ((errStatus =viQueryf(vi,"POIN?;\n","%hd",&iNumPts)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			iNumVals = iNumPts * 4;
			*limitStatusQueryNumValues = iNumVals;

			if (iNumVals > limitStatusQueryArrSize)
				ag875x_LOG_STATUS( vi, thisPtr, ag875x_ARRAY_TOO_SMALL );

			if ((errStatus =ag875x_takeSweep(vi, 1)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			if ((errStatus =viPrintf(vi,"OUTPLIML;\n")) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			 // Need to set attributes so array reads in correctly.
			 if ((errStatus =viSetAttribute(vi,VI_ATTR_TERMCHAR_EN,VI_TRUE)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			for (inc=0; inc<iNumVals; inc+=4)
				{
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				
				// The format returned is float, int, float, float, per segment.  The format 
				// specifier has been changed from "%,4lf" as NI VISA could not read the array
				// correctly.  The new format below allows the float to be read, the comma
				// discarded, the int to be read and converted to a float, the comma is 
				// discarded, and the remaining two comma-seperated floats are read in as usual.
				if ((errStatus = viScanf(vi, "%lf%s%lf%s%,2lf", &limitStatusQueryResult[inc], &tempString,
						&limitStatusQueryResult[inc+1], &tempString,
						&limitStatusQueryResult[inc+2], &limitStatusQueryResult[inc+3])) < VI_SUCCESS)
	    			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				}
			// Need to restore attributes.
			if ((errStatus = viSetAttribute(vi,VI_ATTR_TERMCHAR_EN,VI_FALSE)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 9: // Output the limit test results at the active marker (if any)
			if ((errStatus =viPrintf(vi,"OUTPLIMM;\n")) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			
			*limitStatusQueryNumValues = 4;
					
			if (*limitStatusQueryNumValues > limitStatusQueryArrSize)
				ag875x_LOG_STATUS( vi, thisPtr, ag875x_ARRAY_TOO_SMALL );
				
			// The format returned is float, int, float, float.  The format 
			// specifier has been changed from "%,4lf" as NI VISA could not read the array
			// correctly.  The new format below allows the float to be read, the comma
			// discarded, the int to be read and converted to a float, the comma is 
			// discarded, and the remaining two comma-seperated floats are read in as usual.
			if ((errStatus = viScanf(vi, "%lf%s%lf%s%,2lf", &limitStatusQueryResult[0], &tempString,
								&limitStatusQueryResult[1], &tempString,
								&limitStatusQueryResult[2], &limitStatusQueryResult[3])) < VI_SUCCESS)
    			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 10: case 11: // Output min or max values for all limit line segments

			// Check if Limit Table is empty
			if (iNumOfSegs == 0)
				return ag875x_LIMIT_TABLE_EMPTY;

			if ((errStatus =viPrintf(vi,"MINMAXON;\n")) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			if (choice == 10)
				{
				if ((errStatus =viQueryf(vi,"OUTPAMIN;\n","%hd",&iNumSegs)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				}
			else
				{
				if ((errStatus =viQueryf(vi,"OUTPAMAX;\n","%hd",&iNumSegs)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				}

			// Need to set attribute so array reads in correctly
			if ((errStatus =viSetAttribute(vi,VI_ATTR_TERMCHAR_EN,VI_TRUE)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			*limitStatusQueryNumValues = iNumSegs * 3;

			if (*limitStatusQueryNumValues > limitStatusQueryArrSize)
				ag875x_LOG_STATUS( vi, thisPtr, ag875x_ARRAY_TOO_SMALL );

			for (inc=0; inc<(*limitStatusQueryNumValues); inc+=3)
				{
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				// The format returned is int, float, float, float, per segment.  The format 
				// specifier has been changed from "%,3lf" as NI VISA could not read the array
				// correctly.  The new format below allows the leading int to be read and converted
				// to a float, the comma is discarded, and the remaining two comma-seperated
				// floats are read in as usual.
				if ((errStatus = viScanf(vi, "%lf%s%,2lf", &limitStatusQueryResult[inc], &tempString, 
									&limitStatusQueryResult[inc+1], &limitStatusQueryResult[inc+2])) < VI_SUCCESS)
	    			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				}
			// Need to turn attribute off
			if ((errStatus =viSetAttribute(vi,VI_ATTR_TERMCHAR_EN,VI_FALSE)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			
			if ((errStatus =viPrintf(vi,"MINMAXOFF;\n")) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 12: // Output limit test min and max for specified segment

			// Check if Limit Table is empty
			if (iNumOfSegs == 0)
				return ag875x_LIMIT_TABLE_EMPTY;

			if ((errStatus =viPrintf(vi,"SELSEG%i;\n",limitStatusQuerySegNum)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			if ((errStatus =viPrintf(vi,"MINMAXON;OUTPSEGM;\n")) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			*limitStatusQueryNumValues = 5;
			
			if (*limitStatusQueryNumValues > limitStatusQueryArrSize)
				ag875x_LOG_STATUS( vi, thisPtr, ag875x_ARRAY_TOO_SMALL );

			if ((errStatus = viScanf(vi, "%hd%s%,4lf", &iSegNum, &tempString, &limitStatusQueryResult[1], 
								&limitStatusQueryResult[2], &limitStatusQueryResult[3], &limitStatusQueryResult[4])) < VI_SUCCESS)
    			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			limitStatusQueryResult[0] = (ViReal64)iSegNum;

			if ((errStatus =viPrintf(vi,"MINMAXOFF;\n")) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;

		case 13: // Output limit test min and max for every active segment

			// Check if Limit Table is empty
			if (iNumOfSegs == 0)
				return ag875x_LIMIT_TABLE_EMPTY;

			if ((errStatus =viPrintf(vi,"MINMAXON;\n")) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			if ((errStatus =viQueryf(vi,"OUTPSEGAM;\n","%hd",&iNumSegs)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			// Need to set attribute so array reads in correctly
			if ((errStatus =viSetAttribute(vi,VI_ATTR_TERMCHAR_EN,VI_TRUE)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			*limitStatusQueryNumValues = iNumSegs * 5;

			if (*limitStatusQueryNumValues > limitStatusQueryArrSize)
				ag875x_LOG_STATUS( vi, thisPtr, ag875x_ARRAY_TOO_SMALL );

			for (inc=0; inc<(*limitStatusQueryNumValues); inc+=5)
				{
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if ((errStatus = viScanf(vi, "%hd%s%,4lf", &iSegNum, &tempString, &limitStatusQueryResult[inc+1],
									&limitStatusQueryResult[inc+2], &limitStatusQueryResult[inc+3], 
									&limitStatusQueryResult[inc+4])) < VI_SUCCESS)
	    			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				limitStatusQueryResult[inc] = (ViReal64)iSegNum;
				}
			
			// Need to turn attributes off
			if ((errStatus =viSetAttribute(vi,VI_ATTR_TERMCHAR_EN,VI_FALSE)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			if ((errStatus =viPrintf(vi,"MINMAXOFF;\n")) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			break;
		} /* end switch */

	// Flush before exiting, just in case.
	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Restore instrument to original channel number.
	if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_freqListSet
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Sets up a frequency list table.
 *			 
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *
 * PARAM 2 : ViInt16 freqListSetSegments.  
 * IN        Contains the total number of frequency list segments
 *           to be set.  Range is from 1 to 30.  Make sure that 
 *           the 'freqListSetList' array has at least this number of segments
 *           specified.
 *
 * PARAM 3 : ViReal64 _VI_FAR freqListSetList[].  64 bit Real one-dimensional array for  
 * IN        frequency list (1-30 segments) formatted as follows:
 *
 *			'freqListSetRequest'			segment input format 
 *			---------						--------------------   
 *			"STEPPED LIST"					[ Start, Stop, Points ]
 *			"SWEPT LIST"					[ Start, Stop, Points ]
 *			"SWEPT LIST IF"					[ Start, Stop, Points, IF Bandwidth ]
 *			"SWEPT LIST CPLD PWR"			[ Start, Stop, Points, Port Power ] 
 *			"SWEPT LIST CPLD PWR IF"		[ Start, Stop, Points, Port Power, IF Bandwidth ] 
 *			"SWEPT LIST UNCPLD PWR"			[ Start, Stop, Points, Port 1 Power, Port 2 Power ] 
 *			"SWEPT LIST UNCPLD PWR IF"		[ Start, Stop, Points, Port 1 Power, Port 2 Power, IF Bandwidth ] 
 *
 *           Repeated X number of times in the array where X equals the number of segments. 
 * 
 * PARAM 4 : ViString freqListSetRequest. (OPTIONAL, default=ag875x_FREQ_LIST_SWEPT).   
 * IN        See code below for other choices. 
 *
 * PARAM 5 : ViInt16 freqListSetChannel. (OPTIONAL; default=ag875x_CH1)
 * IN        Indicates channel number.   
 *
 * PARAM 6 : ViBoolean freqListSetAppendMode. (OPTIONAL; default=ag875x_CLEAR_EXISTING_TABLE)
 * IN        Indicates whether to append these list segments to the existing frequency 
 *           list table if one exists (ag875x_APPEND_TO_EXISTING_TABLE) or whether to clear 
 *           the existing list first (ag875x_CLEAR_EXISTING_TABLE).
 *
 *
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *-----------------------------------------------------------------------------
 */
static const char * const ag875x_freqListSet_measType_a[] = {0,"S11",
        "S21","S12","S22","ANAI","AR","BR","AB","MEASA","MEASB","MEASR",0};

ViStatus _VI_FUNC ag875x_freqListSet( 
        ViSession vi, 
		ViInt16 freqListSetSegments,
		ViReal64 _VI_FAR freqListSetList[],
		ViString freqListSetRequest,
		ViInt16 freqListSetChannel,
		ViBoolean freqListSetAppendMode)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 choice = 0;
	ViInt32 seg = 0;
	ViInt32 elem = 0;
	ViInt32 iNumElems = 0;
	ViInt32 iIndex = 0;
	ViInt32 iReply = 0;
	ViChar cur_ID[15], selection[30];
	ViReal64 dFirmware = 0.0;
	const ViInt32 size = 7;
	ViInt32 currentChannel = 0;

	ViString req[]= {"STEPPED LIST", "SWEPT LIST", "SWEPT LIST IF", "SWEPT LIST CPLD PWR",
						 "SWEPT LIST CPLD PWR IF", "SWEPT LIST UNCPLD PWR", "SWEPT LIST UNCPLD PWR IF"
						}; 


	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_freqListSet" );

	/*  Parameter checks */
	ag875x_CHK_PTR(freqListSetList, VI_ERROR_PARAMETER3);
	ag875x_CHK_PTR(freqListSetRequest, VI_ERROR_PARAMETER4);
	ag875x_CHK_INT_RANGE(freqListSetSegments,ag875x_FREQ_LIST_MIN_SEGS,ag875x_FREQ_LIST_MAX_SEGS,VI_ERROR_PARAMETER2);
	ag875x_CHK_ENUM(freqListSetChannel, ag875x_CH4,VI_ERROR_PARAMETER5);
	ag875x_CHK_BOOLEAN(freqListSetAppendMode, VI_ERROR_PARAMETER6);

	if(getChoice(vi,freqListSetRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	if ((errStatus = ag875x_readID(vi, cur_ID, "REV_VALUE?", &dFirmware)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	strip(freqListSetRequest, selection);

	if ((compFirmwareVersions(dFirmware, "<", 7.10))&&(abs(strcmp(selection, ag875x_FREQ_LIST_STEPPED))))
		ag875x_LOG_STATUS( vi, thisPtr, ag875x_REQUIRES_FW_710 );
	if ((errStatus = ag875x_readID(vi, cur_ID, "MODEL?", 0)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	if ((strstr(cur_ID, "ET") != NULL)&&((!abs(strcmp(selection, ag875x_FREQ_LIST_SWEPT_UNCPLD_PWR)))
			||(!abs(strcmp(selection, ag875x_FREQ_LIST_SWEPT_UNCPLD_PWR_IF)))))
		ag875x_LOG_STATUS( vi, thisPtr, ag875x_REQUIRES_S_PARM_ANALYZER );

	// Save the current active channel number, so it may be restored later.
	if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Temporarily change to requested channel.
	if ((errStatus = ag875x_channelSelect(vi, freqListSetChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Begin editing
	if ((errStatus = viPrintf(vi, "EDITLIST;\n")) < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);

	if (!freqListSetAppendMode) // Erase existing limit table if append = FALSE
		if ((errStatus = viPrintf(vi, "CLEL;\n")) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);

	if (choice)
		{
		if ((errStatus = viPrintf(vi, "LISTTYPELSWP;\n")) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);
		}
	else if (compFirmwareVersions(dFirmware, ">=", 7.10))
		{
		if ((errStatus = viPrintf(vi, "LISTTYPELSTP;\n")) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);
		}

	if ((choice == 2)||(choice == 4)||(choice == 6))
		{
		if ((errStatus = viPrintf(vi, "LISIFBWMON;\n")) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);
		}
	else if (choice)
		{
		if ((errStatus = viPrintf(vi, "LISIFBWMOFF;\n")) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);
		}

	if ((choice == 1)||(choice == 2))
		{
		if ((errStatus = viPrintf(vi, "LISPWRMOFF;\n")) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);
		}
	else if ((choice == 3)||(choice == 4))
		{
		if ((errStatus = viPrintf(vi, "PORTPCPLD;LISPWRMON;\n")) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);
		}
	else if (choice)
		{
		iReply = 0; iIndex = 0;
		while (!iReply)
			{
		   if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			   ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		   iIndex++;
		   if ((errStatus = viQueryf(vi,"%s?;\n","%i",ag875x_freqListSet_measType_a[iIndex],&iReply)) < VI_SUCCESS)
			   ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}

		if ((errStatus = viPrintf(vi, "PORTPUNCPLD;S21;LISPWRMON;S12;LISPWRMON;\n")) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);
		}

	for (seg=1;seg<=freqListSetSegments;seg++)  
		{
		elem = seg-1;

		if ((errStatus = viPrintf(vi, "SADD;\n")) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);

		switch (choice)
			{
			case 0:
				iNumElems = 3;
				break;

			case 1:
				iNumElems = 3;
				break;

			case 2:
				iNumElems = 4;
				if ((errStatus = viPrintf(vi, "SEGIFBW%lf;\n",freqListSetList[elem*iNumElems+3])) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				break;

			case 3:
				iNumElems = 4;
				if ((errStatus = viPrintf(vi, "SEGPOWER%lf;\n",freqListSetList[elem*iNumElems+3])) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				break;

			case 4:
				iNumElems = 5;
				if ((errStatus = viPrintf(vi, "SEGPOWER%lf;\n",freqListSetList[elem*iNumElems+3])) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				if ((errStatus = viPrintf(vi, "SEGIFBW%lf;\n",freqListSetList[elem*iNumElems+4])) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				break;

			case 5:
				iNumElems = 5;
				if ((errStatus = viPrintf(vi, "S21;SEGPOWER%lf;\n",freqListSetList[elem*iNumElems+3])) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				if ((errStatus = viPrintf(vi, "S12;SEGPOWER%lf;\n",freqListSetList[elem*iNumElems+4])) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				break;

			case 6:
				iNumElems = 6;
				if ((errStatus = viPrintf(vi, "S21;SEGPOWER%lf;\n",freqListSetList[elem*iNumElems+3])) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				if ((errStatus = viPrintf(vi, "S12;SEGPOWER%lf;\n",freqListSetList[elem*iNumElems+4])) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				if ((errStatus = viPrintf(vi, "SEGIFBW%lf;\n",freqListSetList[elem*iNumElems+5])) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				break;

			} /* end switch */

		if ((errStatus = viPrintf(vi, "STAR%lf;\n",freqListSetList[elem*iNumElems])) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);
		if ((errStatus = viPrintf(vi, "STOP%lf;\n",freqListSetList[elem*iNumElems+1])) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);

		// If only one point, start and stop frequency must be the same
		if (freqListSetList[elem*iNumElems+2] == 1)
			{
			if ((errStatus = viPrintf(vi, "STOP%lf;\n",freqListSetList[elem*iNumElems])) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			}
		if ((errStatus = viPrintf(vi, "POIN%lf;\n",freqListSetList[elem*iNumElems+2])) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);
		if ((errStatus = viPrintf(vi, "SDON;\n")) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);
		}

	//Finished editing
	if ((errStatus = viPrintf(vi, "EDITDONE;\n")) < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);

	if ((choice == 5)||(choice == 6)) // measurement was toggled between S21 and S12; restore original
		{
		if ((errStatus = viPrintf(vi, "%s;\n",ag875x_freqListSet_measType_a[iIndex])) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);
		}

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Restore instrument to original channel number.
	if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_freqListQuery
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Returns the existing frequency list values (if any).
 *			 
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *
 * PARAM 2 : ViPInt16 freqListQuerySegments.  
 * OUT       Returns the number of segments in the frequency list.
 *
 * PARAM 3 : ViReal64 _VI_FAR freqListQueryList[].  64 bit Real one-dimensional array to 
 * OUT       receive the frequency list (1-30 segments) formatted as follows:
 *
 *			'listType'						segment output format 
 *			-----------						--------------------   
 *			"STEPPED LIST"					[ Start, Stop, Points ]
 *			"SWEPT LIST"					[ Start, Stop, Points ]
 *			"SWEPT LIST IF"					[ Start, Stop, Points, IF Bandwidth ]
 *			"SWEPT LIST CPLD PWR"			[ Start, Stop, Points, Port Power ] 
 *			"SWEPT LIST CPLD PWR IF"		[ Start, Stop, Points, Port Power, IF Bandwidth ] 
 *			"SWEPT LIST UNCPLD PWR"			[ Start, Stop, Points, Port 1 Power, Port 2 Power ] 
 *			"SWEPT LIST UNCPLD PWR IF"		[ Start, Stop, Points, Port 1 Power, Port 2 Power, IF Bandwidth ] 
 *
 *           Repeated X number of times in the array where X equals the number of segments. 
 * 
 * PARAM 4 : ViPString freqListQueryListType
 * OUT       Returns string describing the type of frequency list (see above). 
 *
 * PARAM 5 : ViInt16 freqListQueryChannel. (OPTIONAL; default=1)
 * IN        Indicates channel number.   
 *
 *
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *-----------------------------------------------------------------------------
 */
static const char * const ag875x_freqListQuery_measType_a[] = {0,"S11",
        "S21","S12","S22","ANAI","AR","BR","AB","MEASA","MEASB","MEASR",0};

ViStatus _VI_FUNC ag875x_freqListQuery( 
        ViSession vi, 
		ViPInt16 freqListQuerySegments,
		ViReal64 _VI_FAR freqListQueryList[],
		ViPString freqListQueryListType,
		ViInt16 freqListQueryChannel)
	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 iSegIndex = 0;
	ViInt32 iElem = 0;
	ViInt32 iNumElems = 0;
	ViInt32 iListType = 0;
	ViInt32 iReply = 0;
	ViInt32 iIFBW = 0;
	ViInt32 iPwr = 0;
	ViInt32 iPwrCpl = 0;
	ViInt32 iIndex = 0;
	ViReal32 fSeg = 0.0;
	ViInt32 currentChannel = 0;


	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_freqListQuery" );

	/*  Parameter checks */
	ag875x_CHK_PTR(freqListQuerySegments, VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(freqListQueryList, VI_ERROR_PARAMETER3);
	ag875x_CHK_PTR(freqListQueryListType, VI_ERROR_PARAMETER4);
	ag875x_CHK_ENUM(freqListQueryChannel, ag875x_CH4,VI_ERROR_PARAMETER5);

	// Save the current active channel number, so it may be restored later.
	if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Temporarily change to requested channel.
	if ((errStatus = ag875x_channelSelect(vi, freqListQueryChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	//Query list type
	if ((errStatus = viQueryf(vi,"LISTTYPE?;\n","%i",&iListType)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	if (iListType)
		{
		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		//List IF Bandwidth mode?
		if ((errStatus = viQueryf(vi,"LISIFBWM?;\n","%i",&iIFBW)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		//List Power mode?
		if ((errStatus = viQueryf(vi,"LISPWRM?;\n","%i",&iPwr)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		//Port powers coupled?
		if ((errStatus = viQueryf(vi,"PORTP?;\n","%i",&iPwrCpl)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if (!iPwr && !iIFBW)
			{
			strcpy(freqListQueryListType,ag875x_FREQ_LIST_SWEPT);
			iNumElems = 3;
			}
		else if (!iPwr && iIFBW)
			{
			strcpy(freqListQueryListType,ag875x_FREQ_LIST_SWEPT_IF);
			iNumElems = 4;
			}
		else if (iPwrCpl && !iIFBW)
			{
			strcpy(freqListQueryListType,ag875x_FREQ_LIST_SWEPT_CPLD_PWR);
			iNumElems = 4;
			}
		else if (iPwrCpl && iIFBW)
			{
			strcpy(freqListQueryListType,ag875x_FREQ_LIST_SWEPT_CPLD_PWR_IF);
			iNumElems = 5;
			}
		else if (!iPwrCpl && !iIFBW)
			{
			strcpy(freqListQueryListType,ag875x_FREQ_LIST_SWEPT_UNCPLD_PWR);
			iNumElems = 5;
			}
		else 
			{
			strcpy(freqListQueryListType,ag875x_FREQ_LIST_SWEPT_UNCPLD_PWR_IF);
			iNumElems = 6;
			}
		}
	else
		{
		strcpy(freqListQueryListType,ag875x_FREQ_LIST_STEPPED);
		iNumElems = 3;
		}

	if (iListType && iPwr && !iPwrCpl)
		{
		iReply = 0; iIndex = 0;
		while (!iReply)
			{
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			iIndex++;
			if ((errStatus = viQueryf(vi,"%s?;\n","%i",ag875x_freqListQuery_measType_a[iIndex],&iReply)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}
		}

	// Begin editing
	if ((errStatus = viPrintf(vi, "EDITLIST;\n")) < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);

	iSegIndex = 1;

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	//Is there an existing list?
	if ((errStatus = viQueryf(vi,"SEDI%i;SEDI?;\n","%f",iSegIndex,&fSeg)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	while ((int)fSeg == iSegIndex)  
		{
		iElem = iSegIndex-1;

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		//Start frequency?
		if ((errStatus = viQueryf(vi,"STAR?;\n","%lf",&freqListQueryList[iElem*iNumElems])) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		//Stop frequency?
		if ((errStatus = viQueryf(vi,"STOP?;\n","%lf",&freqListQueryList[iElem*iNumElems+1])) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		//Number of points?
		if ((errStatus = viQueryf(vi,"POIN?;\n","%lf",&freqListQueryList[iElem*iNumElems+2])) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if (iListType)
			{
			if (!iPwr && iIFBW)
				{
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				//IF bandwidth?
				if ((errStatus = viQueryf(vi,"SEGIFBW;OUTPACTI;\n","%lf",&freqListQueryList[iElem*iNumElems+3])) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				}
			else if (iPwrCpl)
				{
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				//Segment power?
				if ((errStatus = viQueryf(vi,"SEGPOWER;OUTPACTI;\n","%lf",&freqListQueryList[iElem*iNumElems+3])) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if (iIFBW)
					{
					if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );

					//IF bandwidth?
					if ((errStatus = viQueryf(vi,"SEGIFBW;OUTPACTI;\n","%lf",&freqListQueryList[iElem*iNumElems+4])) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );
					}
				}
			else 
				{
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				//Segment power for Port 1?
				if ((errStatus = viQueryf(vi,"S21;SEGPOWER;OUTPACTI;\n","%lf",&freqListQueryList[iElem*iNumElems+3])) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				// Workaround for firmware 7.64 screen update problem
				// Move to S12, then force screen update
				if ((errStatus = viPrintf(vi, "S12;\n")) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				if ((errStatus = viPrintf(vi, "EDITDONE;\n")) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				if ((errStatus = viPrintf(vi, "EDITLIST;\n")) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				// Go back to segment again
				if ((errStatus = viPrintf(vi,"SEDI%i;\n",iSegIndex)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				//Segment power for Port 2?
				if ((errStatus = viQueryf(vi,"S12;SEGPOWER;OUTPACTI;\n","%lf",&freqListQueryList[iElem*iNumElems+4])) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				if (iIFBW)
					{
					if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );

					//IF bandwidth?
					if ((errStatus = viQueryf(vi,"SEGIFBW;OUTPACTI;\n","%lf",&freqListQueryList[iElem*iNumElems+5])) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );
					}
				}
			}

		iSegIndex++;

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		//Test if recently edited segment was last segment in list
		if ((errStatus = viQueryf(vi,"SEDI%i;SEDI?;\n","%f",iSegIndex,&fSeg)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		}

	//Make sure active entry area has been turned off before we exit the function
	if ((errStatus = viPrintf(vi, "ENTO;\n")) < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);

	*freqListQuerySegments = (int)fSeg;

	//Finished editing
	if ((errStatus = viPrintf(vi, "EDITDONE;\n")) < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);

	if (iListType && iPwr && !iPwrCpl) // measurement was toggled between S21 and S12; restore original
		{
		if ((errStatus = viPrintf(vi, "%s;\n",ag875x_freqListQuery_measType_a[iIndex])) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);
		}

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Restore instrument to original channel number.
	if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_powerRange
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Sets or returns current power range.  
 *
 *           NOTE!!: This function does not apply to an 8753D/8753E/8753ES
 *                   having Option 011.  For Option 011 instruments, use the
 *                   ag875x_portAttenuation function with the ag875x_powerSet
 *                   function to achieve necessary power level at the desired
 *                   test port.  Also, an 8753ET/8719ET/8720ET/8722ET requires 
 *                   Option 004 to have a programmable attenuator, so this
 *                   function requires that those analyzers have Option 004.
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *            
 * PARAM 2 : ViInt16 range.  
 * IN        Contains the power range to be set.  The value of 
 *           ag875x_PWR_RANGE_AUTO will place the instrument in AUTO
 *           range mode.
 *
 * PARAM 3 : ViString powerRangeRequest (OPTIONAL, default="SET"). Other choice   
 * IN        is "?". 
 *			 
 * PARAM 4 : ViInt16 powerRangeChannel. (OPTIONAL, default = ag875x_CH1). 
 * IN        Indicates channel number.  Not needed if power is coupled
 *           between channels (see ag875x_powerCoupling function).
 *
 * PARAM 5 : ViPInt16 currentPowerRange. (OPTIONAL)   
 * OUT       If 'request' is a query (ag875x_Q), this parameter will return
 *           the power range currently in use.
 *           
 *           
 * RETURN  :  VI_SUCCESS:No error. Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_powerRange( 
        ViSession vi, 
		ViInt16 range,
		ViString powerRangeRequest,
		ViInt16 powerRangeChannel,
		ViPInt16 currentPowerRange)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt16 iRange = 0;
	ViInt32 choice = 0;
	ViInt32 iReply = 0; 
	ViChar cur_ID[15];
	ViReal64 statOpt = 0.0;
	const ViInt32 size = 2;
	ViInt32 currentChannel = 0;

	ViString req[]= {"SET","?"};

	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_powerRange" );

	/*  Parameter checks */
	ag875x_CHK_PTR(powerRangeRequest, VI_ERROR_PARAMETER3);
	ag875x_CHK_ENUM(range, ag875x_PWR_RANGE_AUTO, VI_ERROR_PARAMETER2);
	ag875x_CHK_ENUM(powerRangeChannel, ag875x_CH4, VI_ERROR_PARAMETER4);

	// Read instrument ID
	if ((errStatus = ag875x_readID(vi, cur_ID, "MODEL?", 0)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// If 8753, make sure it doesn't have Option 011
	if (strstr(cur_ID, "8753") != NULL)
		{
		if ((errStatus = ag875x_readID(vi, cur_ID, ag875x_OPT_011_Q, &statOpt)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		if ((int)statOpt)
			ag875x_LOG_STATUS( vi, thisPtr, ag875x_OPTION_INVALIDATES_FUNC );
		}

	// If "ET" analyzer, make sure it has Option 004
	if (strstr(cur_ID, "ET") != NULL)
		{
		if ((errStatus = ag875x_readID(vi, cur_ID, ag875x_OPT_004_Q, &statOpt)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		if (!(int)statOpt)
			ag875x_LOG_STATUS( vi, thisPtr, ag875x_OPTION_REQUIRED );
		}

	if(getChoice(vi,powerRangeRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	// If 8753 analyzer, make sure the requested range is correct.
	if ((strstr(cur_ID, "8753") != NULL))
		if ((range > ag875x_PWR_RANGE_7) && (range < ag875x_PWR_RANGE_AUTO) )
			ag875x_LOG_STATUS( vi, thisPtr, ag875x_INVALID_8753_RANGE );

	// Save the current active channel number, so it may be restored later.
	if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Temporarily change to requested channel.
	if ((errStatus = ag875x_channelSelect(vi, powerRangeChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	switch (choice)
		{
		case 0: //Set power range
			if (range == ag875x_PWR_RANGE_AUTO)
				{
				if ((errStatus = viPrintf(vi, "PWRRPAUTO;\n")) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				}
			else if (strstr(cur_ID, "8753") != NULL)
				{
				if ((errStatus = viPrintf(vi, "PWRRPMAN;PRAN%i;\n",range)) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				}
			else
				{
				if ((errStatus = viPrintf(vi, "PWRRPMAN;PRAN%02i;\n",range + 1)) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				}
			break;
		case 1: //Read power range
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viQueryf(vi, "PWRR?;\n","%i",&iReply)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);

			if (iReply)
				*currentPowerRange = ag875x_PWR_RANGE_AUTO;
			else 
				{
				iRange = ag875x_PWR_RANGE_0 - 1;
				while (!iReply)
					{
					iRange++;
					if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );
					if (strstr(cur_ID, "8753") != NULL)
						{
						if ((errStatus = viQueryf(vi, "PRAN%i?;\n","%i",iRange,&iReply)) < VI_SUCCESS)
							return statusUpdate(vi, thisPtr, errStatus);
						}
					else
						{
						if ((errStatus = viQueryf(vi, "PRAN%02i?;\n","%i",iRange + 1,&iReply)) < VI_SUCCESS)
							return statusUpdate(vi, thisPtr, errStatus);
						}
					}
				*currentPowerRange = iRange;
				}
 			break;
		}

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Restore instrument to original channel number.
	if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_powerSet
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  Sets the power level (in dBm ONLY!) for the specified channel, or 
 *            can be used to turn the source power ON or OFF.
 *
 *            For the 8753D/8753E/8753ES Option 011, the power level is specified 
 *            as the power supplied by the source; the power at the test port will 
 *            be less due to loss through the test set.
 *
 *            For the 8719D/ES/ET, 8720D/ES/ET, 8722D/ES/ET, 8753ET and standard 
 *            8753D/E/ES, the power level is specified as the power at the test
 *            port.
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *            
 * PARAM 2 : ViReal64 power.  
 * IN        Contains the power level in dBm to be set.  
 *
 * PARAM 3 : ViString powerSetRequest (OPTIONAL, default="LEVEL").    
 * IN        Other choices are shown below.  
 *			 
 * PARAM 4 : ViInt16 powerSetChannel. (OPTIONAL, default=ag875x_CH1). 
 * IN        Indicates channel number.  Not needed if power is coupled
 *           between channels (see ag875x_powerCoupling function).
 *
 * PARAM 5 : ViInt16 powerSetPortNum. (OPTIONAL, default=ag875x_PORT_NUM1).
 * IN        For the 8719D/ES, 8720D/ES, 8722D/ES and standard 8753D/E/ES, this 
 *           parameter selects the test port at which to set the specified power 
 *           level.  If port power coupling is currently enabled (see 
 *           ag875x_powerCoupling function), then 'powerSetPortNum' will not matter; the 
 *           specified power level will be set for both ports.
 *
 *           NOTE!!: For the 8719/20/22ET, 8753D/E/ES Option 011, and 8753ET, 
 *                   this parameter (powerSetPortNum) is passed to the function, but has
 *                   no effect for the following reasons:           
 *                   8753D/E/ES Option 011 
 *						-> Power level is specified at the source, not the test
 *                         ports.  To set differing power levels for the two test 
 *                         ports, use the ag875x_portAttenuation function.
 *                   8719/20/22ET and 8753ET
 *                      -> Source power is always applied at the Reflection port 
 *                         of a Transmission/Reflection Test Set.
 *           
 *           
 * RETURN  :  VI_SUCCESS:No error. Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
static const char * const ag875x_powerSet_measType_a[] = {0,"S11",
        "S21","S12","S22","ANAI","AR","BR","AB","MEASA","MEASB","MEASR",0};
ViStatus _VI_FUNC ag875x_powerSet( 
        ViSession vi, 
		ViReal64 power,
		ViString powerSetRequest,
		ViInt16 powerSetChannel,
		ViInt16 powerSetPortNum)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 choice = 0;
	ViInt32 iReply = 0;
	ViInt32 iIndex = 0;
	const ViInt32 size = 6;
	ViInt32 currentChannel = 0;

	ViString req[]= {"LVL", "LEVEL", "SOURCE ON", "ON", "SOURCE OFF", "OFF" };


	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_powerSet" );

	/*  Parameter checks */
	ag875x_CHK_PTR(powerSetRequest, VI_ERROR_PARAMETER3);
	ag875x_CHK_ENUM(powerSetChannel, ag875x_CH4,VI_ERROR_PARAMETER4);
	ag875x_CHK_ENUM(powerSetPortNum, ag875x_PORT_NUM2,VI_ERROR_PARAMETER5);

	if(getChoice(vi,powerSetRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	// Save the current active channel number, so it may be restored later.
	if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Temporarily change to requested channel.
	if ((errStatus = ag875x_channelSelect(vi, powerSetChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	switch (choice)
		{
		case 0: case 1: //Set source power level
			iReply = 0; iIndex = 0;
			while (!iReply)
				{
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				iIndex++;
				if ((errStatus = viQueryf(vi,"%s?;\n","%i",ag875x_powerSet_measType_a[iIndex],&iReply)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				}

			if ((((iIndex == 1)||(iIndex == 2))&&(powerSetPortNum == ag875x_PORT_NUM1))
				||(((iIndex == 3)||(iIndex == 4))&&(powerSetPortNum == ag875x_PORT_NUM2)))
				errStatus = viPrintf(vi, "POWE %lf;\n",power);
			else if (powerSetPortNum == ag875x_PORT_NUM1)
				{
				if ((errStatus = viPrintf(vi, "S21;POWE %lf;\n",power)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				errStatus = viPrintf(vi, "%s;\n",ag875x_powerSet_measType_a[iIndex]);
				}
			else
				{
				if ((errStatus = viPrintf(vi, "S12;POWE %lf;\n",power)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				errStatus = viPrintf(vi, "%s;\n",ag875x_powerSet_measType_a[iIndex]);
				}
 			break;

		case 2: case 3://Set source power ON
			errStatus = viPrintf(vi, "SOUPON;\n");
			break;

		case 4: case 5://Set source power OFF
			errStatus = viPrintf(vi, "SOUPOFF;\n");
			break;
		}
	if (errStatus < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Restore instrument to original channel number.
	if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_powerQuery
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  Returns the power level (dBm) for the specified channel, or can 
 *            be used to return source power ON/OFF status.  
 *
 *            For the 8753D/8753E/8753ES Option 011, the power level is specified 
 *            as the power supplied by the source; the power at the test port will 
 *            be less due to loss through the test set.
 *
 *            For the 8719D/ES/ET, 8720D/ES/ET, 8722D/ES/ET, 8753ET and standard 
 *            8753D/E/ES, the power level is specified as the power at the test
 *            port.
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *            
 * PARAM 2 : ViPReal64 currentPowerQuery. 
 * OUT       Contains queried power level or source power ON/OFF status 
 *           (1=ON, 0=OFF).    
 *           
 * PARAM 3 : ViString powerQueryRequest (OPTIONAL, default="LEVEL?").    
 * IN        Other choices are shown below.  
 *			 
 * PARAM 4 : ViInt16 powerQueryChannel. (OPTIONAL, default=ag875x_CH1).
 * IN        Indicates channel number.  Not needed if power is coupled
 *           between channels (see ag875x_powerCoupling function).
 *
 * PARAM 5 : ViInt16 powerQueryPortNum. (OPTIONAL, default=ag875x_PORT_NUM1).
 * IN        For the 8719D/ES, 8720D/ES, 8722D/ES and standard 8753D/E/ES, this 
 *           parameter selects the test port to query the power level at.
 *
 *           NOTE!!: For the 8719/20/22ET, 8753D/E/ES Option 011, and 8753ET, 
 *                   this parameter (powerQueryPortNum) is passed to the function, but has
 *                   no effect for the following reasons:           
 *                   8753D/E/ES Option 011 
 *						-> Power level is specified at the source, not the test
 *                         ports.  
 *                   8719/20/22ET and 8753ET
 *                      -> Source power is always applied at the Reflection port 
 *                         of a Transmission/Reflection Test Set.
 *
 *           
 * RETURN  :  VI_SUCCESS:No error. Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
static const char * const ag875x_powerQuery_measType_a[] = {0,"S11",
        "S21","S12","S22","ANAI","AR","BR","AB","MEASA","MEASB","MEASR",0};

ViStatus _VI_FUNC ag875x_powerQuery( 
        ViSession vi, 
		ViPReal64 currentPowerQuery,
		ViString powerQueryRequest,
		ViInt16 powerQueryChannel,
		ViInt16 powerQueryPortNum)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 choice = 0;
	ViInt32 iReply = 0;
	ViInt32 iIndex = 0;
	const ViInt32 size = 5;
	ViInt32 currentChannel = 0;

	ViString req[]= {"?", "LVL?", "LEVEL?", "SOURCE ON?", "ON?" };


	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_powerQuery" );

	/*  Parameter checks */
	ag875x_CHK_PTR(currentPowerQuery, VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(powerQueryRequest, VI_ERROR_PARAMETER3);
	ag875x_CHK_ENUM(powerQueryChannel, ag875x_CH4,VI_ERROR_PARAMETER4);
	ag875x_CHK_ENUM(powerQueryPortNum, ag875x_PORT_NUM2,VI_ERROR_PARAMETER5);

	if(getChoice(vi,powerQueryRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	// Save the current active channel number, so it may be restored later.
	if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	
	// Temporarily change to requested channel.
	if ((errStatus = ag875x_channelSelect(vi, powerQueryChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	switch (choice)
		{
		case 0: case 1: case 2://Query source power level
			iReply = 0; iIndex = 0;
			while (!iReply)
				{
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );

				iIndex++;
				if ((errStatus = viQueryf(vi,"%s?;\n","%i",ag875x_powerQuery_measType_a[iIndex],&iReply)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				}

			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			if ((((iIndex == 1)||(iIndex == 2))&&(powerQueryPortNum == ag875x_PORT_NUM1))
				||(((iIndex == 3)||(iIndex == 4))&&(powerQueryPortNum == ag875x_PORT_NUM2)))
				errStatus = viQueryf(vi, "POWE?;\n","%lf",currentPowerQuery);
			else if (powerQueryPortNum == ag875x_PORT_NUM1)
				{
				if ((errStatus = viQueryf(vi, "S21;POWE?;\n","%lf",currentPowerQuery)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				errStatus = viPrintf(vi, "%s;\n",ag875x_powerQuery_measType_a[iIndex]);
				}
			else
				{
				if ((errStatus = viQueryf(vi, "S12;POWE?;\n","%lf",currentPowerQuery)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				errStatus = viPrintf(vi, "%s;\n",ag875x_powerQuery_measType_a[iIndex]);
				}
 			break;
		case 3: case 4://Query source power ON/OFF status
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			errStatus = viQueryf(vi, "SOUP?;\n","%lf",currentPowerQuery);
			break;
		}

	if (errStatus < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Restore instrument to original channel number.
	if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_softkey
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Sets Softkeys #1 through 8; One-at-a-time or all at once  
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *            
 * PARAM 2 : ViString softkey. OPTIONAL.  Contains individual key label (10 chr max), 
 * IN		 or all keys separated by "|".  If this parameter is not passed, 
 *           the default is ag875x_ERASE_SOFTKEYS, which is for erasing all softkeys.
 *
 * PARAM 3 : ViInt16 softkeyNumber.  (OPTIONAL)  Softkey number if setting only one   
 * IN        softkey label.  The value of 0 should be used for this parameter when
 *           setting all labels at once or when erasing the labels.  The function
 *           will then proceed based upon the value of the key parameter.
 *
 *
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 *           
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_softkey( 
        ViSession vi, 
		ViString softkey,
		ViInt16 softkeyNumber)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;

	char parse[256];
	ViInt32 not_done = 0;
	ViInt32 index = 0;

	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_softkey" );

	/*  Parameter checks */
	ag875x_CHK_PTR(softkey, VI_ERROR_PARAMETER2);
	ag875x_CHK_INT_RANGE(softkeyNumber,ag875x_SOFTKEY_MIN,ag875x_SOFTKEY_MAX,VI_ERROR_PARAMETER3);

	if ((errStatus = viPrintf(vi, "MENUFORM;MENUOFF;\n")) < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);

	switch (softkeyNumber)
		{
		case 0: // Erase All or Do all Softkeys
			if (!strcmp(softkey, ag875x_ERASE_SOFTKEYS))
	  			{
				for (index = 1; index<=8; index++)
					if ((errStatus = viPrintf(vi, "WRSK%i\"\";\n",index)) < VI_SUCCESS)
						return statusUpdate(vi, thisPtr, errStatus);
				break;
				}
			else
				{
				do
					{ 
					if ((errStatus = ag875x_stringParse(vi, softkey, parse, &not_done,"|")) < VI_SUCCESS)
						return statusUpdate(vi, thisPtr, errStatus);
					index++;
					if (!(strlen(parse) ==0))
						if ((errStatus = viPrintf(vi, "WRSK%i\"%s\";\n",index,parse)) < VI_SUCCESS)
							return statusUpdate(vi, thisPtr, errStatus);
					}
				while (not_done);
				break;
				}

		case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: // Do one softkey only
			if ((errStatus = viPrintf(vi, "WRSK%i\"%s\";\n",softkeyNumber, softkey)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;

		default:
			return statusUpdate(vi, thisPtr, ag875x_READ_MATCH_ERROR);
			break;
		}

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_sweepAltOrChop
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Sets or returns alternate/chop sweep mode.  
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *            
 * PARAM 2 : ViString sweepAltOrChopRequest. Must be ALT, CHOP, or ?
 *
 * PARAM 3 : ViPInt16 sweepAltOrChopMode.  OPTIONAL  
 * OUT       If 'request' is a query, returns 1 for Alt Sweep, 0 for Chop.    
 *
 *
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_sweepAltOrChop( 
        ViSession vi, 
		ViString sweepAltOrChopRequest,
		ViPInt16 sweepAltOrChopMode)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 choice = 0;
	ViInt32 iReply = 0;
	const ViInt32 size = 4;

	ViString req[]= {"ALT", "ALTERNATE", "CHOP", "?"
						};

	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_sweepAltOrChop" );

	/*  Parameter checks */
	ag875x_CHK_PTR(sweepAltOrChopRequest, VI_ERROR_PARAMETER2);

	if(getChoice(vi,sweepAltOrChopRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	switch (choice)
		{
		case 0: case 1: //Alternate
		   errStatus = viPrintf(vi, "ALTAB;\n");
		   break;

		case 2: //Chop
			errStatus = viPrintf(vi, "CHOPAB;\n");
			break;

		case 3: //?
			if ((errStatus = viFlush(vi,VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			errStatus = viQueryf(vi, "ALTAB?;\n","%i",&iReply);
			*sweepAltOrChopMode = (ViInt16) iReply;
			break;
		}

	if (errStatus < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);
	else
		return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_sweepStep
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Sets or returns stepped sweep status (ON/OFF).  
 *           NOTE:  This function is for 8719/8720/8722 only!
 *
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *            
 * PARAM 2 : ViString sweepStepRequest. Must be ON, OFF, or ?
 *
 * PARAM 3 : ViInt16 sweepStepChannel. (OPTIONAL, default=1) 
 * IN        Indicates channel number   
 *
 * PARAM 4 : ViPInt16 sweepStepState.  OPTIONAL  
 * OUT       If 'request' is a query, returns 1 for stepped Sweep ON; 0 for OFF.    
 *
 *
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *
 *
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_sweepStep( 
        ViSession vi, 
		ViString sweepStepRequest,
		ViInt16 sweepStepChannel,
		ViPInt16 sweepStepState)
	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 choice = 0;
	ViInt32 currentChannel = 0;
	ViChar cur_ID[15];
	const ViInt32 size = 3;

	ViString req[]= {"OFF", "ON", "?"};

	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_sweepStep" );

	/*  Parameter checks */
	ag875x_CHK_PTR(sweepStepRequest, VI_ERROR_PARAMETER2);
	ag875x_CHK_ENUM(sweepStepChannel, ag875x_CH4,VI_ERROR_PARAMETER3);
	
	if(getChoice(vi,sweepStepRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	if ((errStatus = ag875x_readID(vi, cur_ID, "MODEL?",0)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	cur_ID[4] = 0; //don't care what vintage of 8753 or 872x

	if (!abs(strcmp(cur_ID, "8753")))
		ag875x_LOG_STATUS( vi, thisPtr, ag875x_NOT_SUPPORTED_WITH_8753 );

	// Save the current active channel number, so it may be restored later.
	if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Temporarily change to requested channel.
	if ((errStatus = ag875x_channelSelect(vi, sweepStepChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	switch (choice)
		{
		case 0: //Off
			errStatus = viPrintf(vi, "STEPSWPOFF;\n");
			break;

		case 1: //On
			errStatus = viPrintf(vi, "STEPSWPON;\n");
			break;

		case 2: //?
			if ((errStatus = viFlush(vi,VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			errStatus = viQueryf(vi, "STEPSWP?;\n","%hd",sweepStepState);
			break;
		}
	if (errStatus < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Restore instrument to original channel number.
	if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_sweepTime
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Sets or returns current sweep time, or can be used to set sweep 
 *           time mode (AUTO/MANUAL).  
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *            
 * PARAM 2 : ViReal64 sweepTime.
 * IN        Sweep time to set. Setting a nonzero sweep time sets analyzer to 
 *           manual mode.  Setting sweep time to 0 will activate auto mode.  
 *
 * PARAM 3 : ViString sweepTimeRequest. (OPTIONAL, default="SET"). See code below for
 * IN   	 other choices.
 *
 * PARAM 4 : ViInt16 sweepTimeChannel. (OPTIONAL, default=1) 
 * IN        Indicates channel number
 *
 * PARAM 5 : ViPReal64 currentSweepTime. (OPTIONAL) 
 * OUT       If 'request' is a query, returns current sweep time.
 *
 *           
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_sweepTime( 
        ViSession vi, 
		ViReal64 sweepTime,
		ViString sweepTimeRequest,
		ViInt16 sweepTimeChannel,
		ViPReal64 currentSweepTime)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 choice = 0;
	ViInt32 currentChannel = 0;
	const ViInt32 size = 9;
	ViReal64 currentTime = 0;

	ViString req[]= {"SET", "TIME", "SWEEPTIME", "AUTO", "MAN", "MANUAL", "?", "TIME?", 
						 "SWEEPTIME?"
				 		};

	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_sweepTime" );

	/*  Parameter checks */
	ag875x_CHK_PTR(sweepTimeRequest, VI_ERROR_PARAMETER3);
	ag875x_CHK_ENUM(sweepTimeChannel, ag875x_CH4,VI_ERROR_PARAMETER4);
	
	if(getChoice(vi,sweepTimeRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	// Save the current active channel number, so it may be restored later.
	if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Temporarily change to new channel
	if ((errStatus = ag875x_channelSelect(vi, sweepTimeChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	switch (choice)
		{
		case 0: case 1: case 2: //Set Sweep time
			if (sweepTime == 0) 
				errStatus = viPrintf(vi, "SWEA;\n");
			else
   				errStatus = viPrintf(vi, "SWET%lf;\n",sweepTime);
			break;

		case 3://Set Auto sweep mode on
			errStatus = viPrintf(vi, "SWEA;\n");
			break;

		case 4: case 5://Set Auto sweep mode off (manual mode on)
			// Query instrument for current sweep time, and 
			// return it so that we may enter manual mode
			errStatus = viQueryf(vi, "SWET?;\n", "%lf", &currentTime);
			errStatus = viPrintf(vi, "SWET%lf;\n", currentTime);
			break;

		case 6: case 7: case 8: //Read Sweep time
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	   		errStatus = viQueryf(vi, "SWET?;\n", "%lf", currentSweepTime);
			break;
		}

	if (errStatus < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Restore instrument to original channel number.
	if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_takeSweep
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Takes specified number of sweeps or determines best number of 
 *			 sweeps to take, then takes them.   
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *
 * PARAM 2 : ViInt32 sweeps. (OPTIONAL default=0) If not specified (0), takes  
 * IN        1 or more sweeps depending upon avg on/off and averaging factor.  
 *			 
 *           
 * RETURN  :  VI_SUCCESS:No error. Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_takeSweep( 
        ViSession vi, 
		ViInt32 sweeps)
	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt16 alt_swp = 0;
	ViInt16 swp = 0;
	ViInt16 chan = 0;
	ViInt16 max_swps[3];
	ViReal64 dummy=0.0;
	ViReal64 avg_factor[3];
	ViReal64 avg_on[3];


	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_takeSweep" );

	// Find if Alt Sweep  1=AltSwpOn   0=Normal

	if ((errStatus = ag875x_sweepAltOrChop(vi, "?", &alt_swp)) < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);

	if (sweeps > 1)
		{
		max_swps[1] = (ViInt16)sweeps;
		max_swps[2] = (ViInt16)sweeps;
		}
	else
		{
		max_swps[1] = 1;
		max_swps[2] = 1;
		}

	if (sweeps == 0) // sweeps not specified
		{
		for (chan = 1; chan<=2; chan++) //Find avg on each ch
			{	
			if ((errStatus = ag875x_averageSmooth(vi, dummy, "AVERAGING?", chan, &avg_on[chan])) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			if ((errStatus = ag875x_averageSmooth(vi, dummy, "FACTOR?", chan, &avg_factor[chan])) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			if (avg_on[chan] ==1)
				max_swps[chan] = (int)(avg_factor[chan]*3/2); //take 1.5*avg for best settling
			}
		}

	if (alt_swp)
		{
		for (chan = 1; chan<=2; chan++)
			{
			if (chan == 1)
				{
				if ((errStatus = ag875x_channelSelect(vi, chan)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				}
			else
				{
				if ((errStatus = ag875x_outputf(vi, 3,"CHAN%i;", chan)) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				}
			if ((errStatus = ag875x_outputf(vi, 3,"NUMG%i;", max_swps[chan])) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			}
		if ((errStatus = ag875x_outputf(vi, 3,"CHAN%i;", thisPtr->cur_chan)) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);
		}
	else
		{ 
			if (max_swps[1]> max_swps[2])
		swp = max_swps[1];
	else
		swp = max_swps[2];

		if ((errStatus = ag875x_outputf(vi, 3,"NUMG%i;", swp)) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);
		}

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}       /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_averageSmooth
 *-----------------------------------------------------------------------------
 * 
 *    NOTE : This function replaces the ag875x_average and 
 *           ag875x_averageSmoothSet functions of this driver.
 *           ag875x_average and ag875x_averageSmoothSet remain in the driver
 *           for backwards compatibility purposes.
 *
 * PURPOSE : Sets or returns current averaging factor or ON/OFF status,
 *           smoothing aperture or ON/OFF status, or restarts averaging.  
 *           Setting any nonzero avg factor turns averaging 
 *           on.  Similarly, setting any nonzero smoothing aperture will turn
 *           smoothing on.  Setting an avg factor of 0 will turn averaging off; 
 *           a smoothing aperture of 0 will turn smoothing off.  
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *            
 * PARAM 2 : ViReal64 averageSmoothSet.
 * IN        Averaging Factor or Smoothing Aperture to be set 
 *           (or OFF indicator = 0)
 *
 * PARAM 3 : ViString averageSmoothRequest. (OPTIONAL, default="FACTOR"). 
 * IN   	 Other choices are "APERTURE" or "RESTART".  Averaging "FACTOR?", 
 *           smoothing "APERTURE?", "AVERAGING?" ON/OFF status and "SMOOTHING?"
 *           ON/OFF status can be queried.
 *
 * PARAM 4 : ViInt16 averageSmoothChannel. (OPTIONAL, default=1) 
 * IN        Indicates channel number
 *
 * PARAM 5 : ViPReal64 currentAverageSmooth. (OPTIONAL) 
 * OUT       Indicates current avg factor or smoothing aperture 
 *           (or ON/OFF status 0=OFF, 1=ON), depending upon 'request'.  
 *
 *           
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_averageSmooth( 
        ViSession vi, 
		ViReal64 averageSmoothSet,
		ViString averageSmoothRequest,
		ViInt16 averageSmoothChannel,
		ViPReal64 currentAverageSmooth)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 choice = 0;
	const ViInt32 size = 7;
	ViInt32 currentChannel = 0;

	ViString req[]= {"FACTOR", "APERTURE", "RESTART", "FACTOR?", "APERTURE?", 
						 "AVERAGING?", "SMOOTHING?"
						};


	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_averageSmooth" );

	/*  Parameter checks */
	ag875x_CHK_PTR(averageSmoothRequest, VI_ERROR_PARAMETER3);
	ag875x_CHK_ENUM(averageSmoothChannel, ag875x_CH4,VI_ERROR_PARAMETER4);

	if(getChoice(vi,averageSmoothRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	
	// Save the current active channel number, so it may be restored later.
	if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		
	// Temporarily change to requested channel.
	if ((errStatus = ag875x_channelSelect(vi, averageSmoothChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	switch (choice)
		{
		case 0: //Set Averaging or turn off
			ag875x_CHK_REAL_RANGE(averageSmoothSet
		  						  ,ag875x_AVG_FACTOR_MIN
								  ,ag875x_AVG_FACTOR_MAX
								  ,VI_ERROR_PARAMETER2);

	   		if (averageSmoothSet == 0) 
				{
				if ((errStatus = viPrintf(vi, "AVEROOFF;\n")) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				}
			else
				{
   				if ((errStatus = viPrintf(vi, "AVERFACT%lf;AVEROON;\n", averageSmoothSet)) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				}
			break;

		case 1: //Set Smoothing or turn off
			ag875x_CHK_REAL_RANGE(averageSmoothSet
								 ,ag875x_SMOOTH_APERTURE_MIN
								 ,ag875x_SMOOTH_APERTURE_MAX
								 ,VI_ERROR_PARAMETER2);

			if (averageSmoothSet == 0) 
				{
				if ((errStatus = viPrintf(vi, "SMOOOOFF;\n")) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				}
			else
				{
   				if ((errStatus = viPrintf(vi, "SMOOAPER%lf;SMOOOON;\n", averageSmoothSet)) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				}
			break;

		case 2: //Restart averaging
			if ((errStatus = viPrintf(vi, "AVERREST;\n")) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;

		case 3: //Query averaging factor
			if ((errStatus = viQueryf(vi, "AVERFACT?;\n", "%lf", currentAverageSmooth)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;

		case 4: //Query smoothing aperture
			if ((errStatus = viQueryf(vi, "SMOOAPER?;\n", "%lf", currentAverageSmooth)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;

		case 5: //Averaging ON?
			if ((errStatus = viQueryf(vi, "AVERO?;\n", "%lf", currentAverageSmooth)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;

		case 6: //Smoothing ON?
			if ((errStatus = viQueryf(vi, "SMOOO?;\n", "%lf", currentAverageSmooth)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;
		}

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	
	// Restore instrument to original channel number.
	if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_outputf
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : General purpose output that allows user to select output mode, set
 *           up various implementation of OPC, and append proper termination
 * 
 * PARAM 1 : ViSession vi
 * IN        Instrument Handle returned from ag875x_init().
 *            
 * PARAM 2 : ViInt16 opc_mode. Sets OPC mode; see below for choices
 * IN		  
 *            
 * PARAM 3 : ViString fmtStr, ...   Output string; variable # of parameters 
 * IN		  
 *
 *
 * RETURN  :  VI_SUCCESS:No error. Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 *-----------------------------------------------------------------------------
 */
static ViStatus _VI_FUNC ag875x_outputf(
        ViSession  vi,
		ViInt16 opc_mode,
		ViString fmtStr,
        ...) 
	{
	ViStatus  errStatus = VI_SUCCESS;
	va_list   va;
	struct ag875x_globals *thisPtr;
	char printFmt[1024];
	ViInt32 esr = 0;
	ViInt32 opc=0;

	GET_GLOBALS(vi, thisPtr)
   
	if ((errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA,(ViAddr) &thisPtr)) < VI_SUCCESS)
		return errStatus;

	va_start(va, fmtStr);
	switch (opc_mode)
		{
		case 0: // Append "\n" only
			strcpy(printFmt, fmtStr);
			strcat(printFmt, "\n");
			errStatus = viVPrintf(vi, printFmt, va);
			if (thisPtr->myDelay > 0)
				 doDelay(thisPtr->myDelay);
			break;

		case 1: // Append ";*WAI\n"
			strcpy(printFmt, fmtStr);
			strcat(printFmt, ";*WAI\n");
			errStatus = viVPrintf(vi, printFmt, va);
			if (thisPtr->myDelay > 0)
				 doDelay(thisPtr->myDelay);
			break;

		case 2: // Append "OPC?;" to beginning, "\n" to end, and then enter opc
			strcpy(printFmt, "OPC?;");
			strcat(printFmt, fmtStr);
			strcat(printFmt, "\n");
			if ((errStatus = viFlush(vi,VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			if ((errStatus = viVPrintf(vi, printFmt, va)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			if (thisPtr->myDelay > 0)
				doDelay(thisPtr->myDelay);
			errStatus = viScanf(vi, "%i", &opc);
			break;

		case 3:// Append "OPC;" to beginning, "\n" to end, and then look for ESR bit
			if ((errStatus = viPrintf(vi, "*CLS\n")) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			strcpy(printFmt, "OPC;");
			strcat(printFmt, fmtStr);
			strcat(printFmt, "\n");
			if ((errStatus = viVPrintf(vi, printFmt, va)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			if (thisPtr->myDelay > 0)
				 doDelay(thisPtr->myDelay);
			do
				{
				Sleep(25);// wait 25 ms between checks
				
				if ((errStatus = viFlush(vi,VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				if ((errStatus = viQueryf(vi, "*ESR?\n", "%i", &esr)) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				opc = esr & 1; // Logical AND w/mask (1) to only look at bit 0
				} while (!opc);
			// Flush again just in case
			if ((errStatus = viFlush(vi,VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;

		case 4: //Append nothing
			errStatus = viVPrintf(vi, printFmt, va);
			if (thisPtr->myDelay > 0)
				 doDelay(thisPtr->myDelay);
			break;

		default:
			return statusUpdate(vi, thisPtr, ag875x_READ_MATCH_ERROR);
			break;
	} /* end switch */

	va_end(va);

	if (errStatus < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);
	else
		return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_bandwidth
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Sets or returns active IF bandwidth  
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *
 * PARAM 2 : ViReal64 bandwidth. 
 * IN        Contains the bandwidth in Hz.  
 *            
 * PARAM 3 : ViString bandwidthRequest. (OPTIONAL, default = "SET"  Other choice is "?".  
 * IN        
 *			 
 * PARAM 4 : ViInt16 bandwidthChannel. (OPTIONAL, default=1) 
 * IN        Indicates channel number
 *
 * PARAM 5 : ViReal64 currentBandwidth. (OPTIONAL) 
 * OUT       If 'request' is a query, returns the current bandwidth in Hz.   
 *           
 *           
 * RETURN  :  VI_SUCCESS:No error. Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 *           
 * LIMITATIONS: Some BW values are not valid on some revisions.  No error
 *              is reported, closest valid BW will be used by analyzer.
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_bandwidth( 
        ViSession vi, 
		ViReal64 bandwidth,
		ViString bandwidthRequest,
		ViInt16 bandwidthChannel,
		ViPReal64 currentBandwidth)
	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 choice = 0;
	ViInt32 currentChannel = 0;
	const ViInt32 size = 2;
	ViReal64 instBandwidth = 0;	// Actual bandwith value used by instrument

	ViString req[]= { "SET", "?" };

	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_bandwidth" );

	/*  Parameter checks */
	ag875x_CHK_PTR(bandwidthRequest, VI_ERROR_PARAMETER3);
	ag875x_CHK_ENUM(bandwidthChannel, ag875x_CH4,VI_ERROR_PARAMETER4);
	ag875x_CHK_ENUM((ViInt16)bandwidth, ag875x_IF_BW_6000,VI_ERROR_PARAMETER4);

	if(getChoice(vi,bandwidthRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	// Save the current active channel number, so it may be restored later.
	if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Temporarily change to requested channel.
	if ((errStatus = ag875x_channelSelect(vi, bandwidthChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Convert bandwith enum value into a value used by the instrument
	switch ((ViInt16)bandwidth)
	{
		case ag875x_IF_BW_10:
			instBandwidth = 10;
			break;
		case ag875x_IF_BW_30:
			instBandwidth = 30;
			break;
		case ag875x_IF_BW_100:
			instBandwidth = 100;
			break;
		case ag875x_IF_BW_300:
			instBandwidth = 300;
			break;
		case ag875x_IF_BW_1000:
			instBandwidth = 1000;
			break;
		case ag875x_IF_BW_3000:
			instBandwidth = 3000;
			break;
		case ag875x_IF_BW_3700:
			instBandwidth = 3700;
			break;
		case ag875x_IF_BW_6000:
			instBandwidth = 6000;
			break;

	}	// end switch (bandwidth)

	switch (choice)
		{
		case 0://Set Bandwidth
			if ((errStatus = viPrintf(vi, "IFBW %lf;\n",instBandwidth)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;

		case 1://Query
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
   			if ((errStatus = viQueryf(vi, "IFBW?;\n","%lf",currentBandwidth)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;
		}
	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Restore instrument to original channel number.
	if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);
	
	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_title
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function is used to:
 *            -> Display a user-specified title for the specified channel, or
 *            -> Query the last title that was written to the instrument, 
 *               regardless of channel.
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *
 * PARAM 2 : ViString title. 
 * IN        Contains a title to be displayed for the specified channel.  
 *            
 * PARAM 3 : ViString titleRequest. (OPTIONAL, default = "SET PADDED")  
 * IN        Other choices are "SET NO PAD" and "?".  
 *			 
 * PARAM 4 : ViInt16 titleChannel. (OPTIONAL, default=1) 
 * IN        Indicates channel number
 *
 * PARAM 5 : ViPString currentTitle. (OPTIONAL) 
 * OUT       If 'request' is a query, returns whichever title was most recently 
 *           written to the instrument.
 *           
 * RETURN  :  VI_SUCCESS:No error. Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_title( 
        ViSession vi, 
		ViString title,
		ViString titleRequest,
		ViInt16 titleChannel,
		ViPString currentTitle)
	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 choice = 0;
	ViInt32 iNotDone = 0;
	ViInt32 currentChannel = 0;
	#define MAX_TITLE_SIZE 48
	ViChar cur_ID[15];
	ViChar title_temp[MAX_TITLE_SIZE + 1], parsedString[MAX_TITLE_SIZE + 1], new_title[MAX_TITLE_SIZE + 1];
	ViBoolean bFirmwareQueried = VI_FALSE;
	ViReal64 dFirmware = 0.0;
	const ViInt32 size = 3;

	ViChar selection[256];

	ViString req[]= { "SET PADDED", "SET NO PAD", "?"
						};

	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_title" );

	/*  Parameter checks */
	ag875x_CHK_PTR(title, VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(titleRequest, VI_ERROR_PARAMETER3);
	ag875x_CHK_ENUM(titleChannel, ag875x_CH4,VI_ERROR_PARAMETER4);
	
	if(getChoice(vi,titleRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	// Save the current active channel number, so it may be restored later.
	if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Temporarily change to requested channel.
	if ((errStatus = ag875x_channelSelect(vi, titleChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	switch (choice)
		{	
		case 0://Set Title and pad with spaces if contains CR/LF pair
			strcpy(new_title, "");
			strcpy(title_temp, title);
			bFirmwareQueried = VI_FALSE;
			do
				{
				if ((errStatus = ag875x_stringParse(vi,title_temp,parsedString,&iNotDone,"\n")) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				strcat(new_title,parsedString);
				if (iNotDone)
					{
					if (!bFirmwareQueried)
						{
						if ((errStatus = ag875x_readID(vi, cur_ID, "REV_VALUE?", &dFirmware)) < VI_SUCCESS)
							ag875x_LOG_STATUS( vi, thisPtr, errStatus );
						bFirmwareQueried = VI_TRUE;
						}

					if (compFirmwareVersions(dFirmware, "<", 7.40))
						strcat(new_title,"\n     ");
					else
						strcat(new_title,"\n      ");
					}
				}
			while (iNotDone);

			if ((errStatus = viPrintf(vi, "TITL\"%s\";\n",new_title)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;

		case 1://Set Title with no padding
			if ((errStatus = viPrintf(vi, "TITL\"%s\";\n",title)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;

		case 2://Query
			// Use a temporary var for now, incase no title 
			// is present.
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
   			if ((errStatus = viQueryf(vi, "OUTPTITL;\n","%t",currentTitle)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);

			// Strip trailing line feed at the end of the string
			stripLF(currentTitle);

			// Strip beginning and ending quotes from the string
			strip(currentTitle, selection);

			// If there is no title, the instrument returns a garbage char, usually 
			// ASCII 1, instead of an empty string.  Test for this condition, and 
			// return error message as needed.
			if (strlen(selection) == 1)
				if (! isalnum(selection[0]))
					{
					currentTitle[0] = 0;
					return ag875x_NO_TITLE_FOUND;
					}

			strcpy(currentTitle, selection);
			break;
	} /* end switch */

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Restore instrument to original channel number.
	if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_print
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Can be used to set various printer related parameters, or to begin
 *           printing from the analyzer to a printer.
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *            
 * PARAM 2 : ViString printRequest. OPTIONAL, Default=PRINT. See code for choices.
 * IN
 *
 * PARAM 3 : ViString printModeA.  OPTIONAL  Contains specifier for 'printRequest'.
 * IN        See code below or the driver help file for more information.
 * 
 * PARAM 4 : ViString printModeB.  OPTIONAL  Contains specifier for 'printModeA'.
 * IN        See code below or the driver help file for more information.
 * 
 * PARAM 5 : ViPString queryPrintMode.  OPTIONAL  
 * OUT       When 'printRequest' is a query, returns the active setting of the
 *           specified printer related parameter.
 *
 *
 * RETURN  :  VI_SUCCESS:No error. Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 *           
 *----------------------------------------------------------------------------*/
 
ViStatus _VI_FUNC ag875x_print( 
        ViSession vi, 
		ViString printRequest,
		ViString printModeA,
		ViString printModeB,
		ViPString queryPrintMode)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 choice = 0;
	ViInt32 iReply = 0;
	ViInt32 idx=0;
	ViInt32 dummy=0;
	ViReal64 baudrate = 0.0;
	ViChar strp_printModeA[40], type[10], port[10], feature[10], color[10], handshake[10];
	const ViInt32 size = 20;

	ViString req[]= {"PRINT", "DESTINATION", "DEST", "PORT?", "COLOR", "COLOR?",
						 "FEATURE COLOR", "MODE", "QUADRANT", "QUADRANT?", 
						 "FORMFEED", "FF", "FORMFEED?", "FF?", "SOFTKEYS", 
						 "BAUD RATE", "BAUD RATE?", "HANDSHAKE", "HANDSHAKE?",
						 "DEFAULT" 
						/* Add when VISA implements support for GPIB Pass Control */
						// "ADDR", "ADDRESS"					
						};

	const ViInt32 clr_sz = 8;

	ViString colors[]= {"WHITE", "CYAN", "MAGENTA", "BLUE", "YELLOW", "GREEN",
							  "RED", "BLACK"
							};

	const ViInt32 baud_sz = 5;

	ViString bauds[]= {"1200", "2400", "4800", "9600", "19200"
							};

	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_print" );

	/*  Parameter checks */
	ag875x_CHK_PTR(printRequest, VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(printModeA, VI_ERROR_PARAMETER3);
	ag875x_CHK_PTR(printModeB, VI_ERROR_PARAMETER4);

	if(getChoice(vi,printRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	toUpper(printModeA);
	strip(printModeA, strp_printModeA);

	switch (choice)
		{
		case 0: //Print NOW! 
			if (!strcmp(strp_printModeA, "SINGLE PAGE"))
				errStatus = viPrintf(vi, "PRINALL;RESD;\n");
			else if (!strcmp(strp_printModeA, "MULTI PAGE"))
				errStatus = viPrintf(vi, "PRINTALL;RESD;\n");
			else
				return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);
			break;

		case 1: case 2://DESTINATION
			if (!strcmp(strp_printModeA, "DJ 540 OR 850C"))  strcpy(type,"540");	
			else if (!strcmp(strp_printModeA, "DESKJET"))  strcpy(type,"DJ");	
			else if (!strcmp(strp_printModeA, "EPSON"))  strcpy(type,"EP");	
			else if (!strcmp(strp_printModeA, "LASERJET"))  strcpy(type,"LJ");	
			else return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

			toUpper(printModeB);
			strip(printModeB, port);

			if (!strcmp(port, "PARALLEL"))  strcpy(port,"PARA");	
			else if (!strcmp(port, "SERIAL"))  strcpy(port,"SERI");	
			/* Add when VISA implements support for GPIB Pass Control */
			// else if strcmp(port, "GPIB"));
			else return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);
			errStatus = viPrintf(vi, "PRNTYP%s;PRNPRT%s;\n", type,port);
			break;

		case 3: // Read Port
			if ((errStatus = viQueryf(vi, "PRNPRTPARA?;\n","%i",&iReply)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			if (iReply) 
				strcpy(queryPrintMode,"PARALLEL");
			else
				{
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				if ((errStatus = viQueryf(vi, "PRNPRTSERI?;\n","%i",&iReply)) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				if (iReply) 
					strcpy(queryPrintMode,"SERIAL");
				else
					strcpy(queryPrintMode,"GPIB");
				}
			break;

		case 4: //Toggle color printing ON/OFF
			if (!strcmp(strp_printModeA, "OFF"))
				errStatus = viPrintf(vi, "PRIS;\n");
			else if (!strcmp(strp_printModeA, "ON"))
				errStatus = viPrintf(vi, "PRIC;\n");
			else
				return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);
			break;

		case 5: //Query color printing ON/OFF status
			if ((errStatus = viQueryf(vi, "PRIC?;\n","%i",&iReply)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			if (iReply) 
				strcpy(queryPrintMode,"ON");
			else
				strcpy(queryPrintMode,"OFF");
			break;

		case 6: //Set the color of a specific feature
			if (!strcmp(strp_printModeA, "CH 1 DATA"))  strcpy(feature,"DATA1");	
			else if (!strcmp(strp_printModeA, "CH 2 DATA"))  strcpy(feature,"DATA2");	
			else if (!strcmp(strp_printModeA, "CH 3 DATA"))  strcpy(feature,"DATA3");	
			else if (!strcmp(strp_printModeA, "CH 4 DATA"))  strcpy(feature,"DATA4");	
			else if (!strcmp(strp_printModeA, "CH 1 MEMORY"))  strcpy(feature,"MEMO1");	
			else if (!strcmp(strp_printModeA, "CH 2 MEMORY"))  strcpy(feature,"MEMO2");	
			else if (!strcmp(strp_printModeA, "CH 3 MEMORY"))  strcpy(feature,"MEMO3");	
			else if (!strcmp(strp_printModeA, "CH 4 MEMORY"))  strcpy(feature,"MEMO4");	
			else if (!strcmp(strp_printModeA, "GRATICULE"))  strcpy(feature,"GRAT");	
			else if (!strcmp(strp_printModeA, "REF LINE"))  strcpy(feature,"REFL");	
			else if (!strcmp(strp_printModeA, "WARNING"))  strcpy(feature,"WARN");	
			else if (!strcmp(strp_printModeA, "TEXT")) strcpy(feature,"TEXT");
			else 
				return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

			toUpper(printModeB);

			strip(printModeB, color);
			if (getChoice(vi,color,clr_sz,colors,&idx)) 
				return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);
			errStatus = viPrintf(vi, "PCOL%s%s;\n",feature,color);
			break;

		case 7:  //Mode
			if (!strcmp(strp_printModeA, "LIST")) 
				errStatus = viPrintf(vi, "LISV;\n");
			else if (!strcmp(strp_printModeA, "OP PARMS"))  
				errStatus = viPrintf(vi, "OPEP;\n");
			else if (!strcmp(strp_printModeA, "GRAPH"))
				errStatus = viPrintf(vi, "DISPDATA;\n");
			else
				return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);
			break;

		case 8:  //Quadrant
			if (!strcmp(strp_printModeA, "LEFT LOWER"))  
				errStatus = viPrintf(vi, "LEFL;\n");
			else if (!strcmp(strp_printModeA, "LEFT UPPER"))
				errStatus = viPrintf(vi, "LEFU;\n");
			else if (!strcmp(strp_printModeA, "RIGHT LOWER"))
				errStatus = viPrintf(vi, "RIGL;\n");
			else if (!strcmp(strp_printModeA, "RIGHT UPPER"))
				errStatus = viPrintf(vi, "RIGU;\n");
			else if (!strcmp(strp_printModeA, "FULL PAGE"))
				errStatus = viPrintf(vi, "FULP;\n");
			else 
				return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);
			break;

		case 9:  //Query quadrant
			if ((errStatus = viQueryf(vi, "FULP?;\n","%i",&iReply)) < VI_SUCCESS) 
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if (iReply)
				strcpy(queryPrintMode, "FULL PAGE");
			else 
				{
				if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				if ((errStatus = viQueryf(vi, "LEFL?;\n","%i",&iReply)) < VI_SUCCESS) 
					ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				if (iReply)
					strcpy(queryPrintMode, "LEFT LOWER");
				else 
					{
					if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );
					if ((errStatus = viQueryf(vi, "LEFU?;\n","%i",&iReply)) < VI_SUCCESS) 
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );
					if (iReply)
						strcpy(queryPrintMode, "LEFT UPPER");
					else 
						{
						if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
							ag875x_LOG_STATUS( vi, thisPtr, errStatus );
						if ((errStatus = viQueryf(vi, "RIGL?;\n","%i",&iReply)) < VI_SUCCESS) 
							ag875x_LOG_STATUS( vi, thisPtr, errStatus );
						if (iReply)
							strcpy(queryPrintMode, "RIGHT LOWER");
						else 
							strcpy(queryPrintMode, "RIGHT UPPER");
						}
					}
				}
			break;

		case 10: case 11:  //Auto FormFeed ON/OFF
			if ((strcmp(strp_printModeA, "OFF"))&&(strcmp(strp_printModeA, "ON")))
				return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);
			errStatus = viPrintf(vi, "PRNTRAUTF%s;\n",strp_printModeA);
			break;

		case 12: case 13:  //Query auto formfeed ON/OFF status
			if ((errStatus = viQueryf(vi, "PRNTRAUTF?;\n","%i",&iReply)) < VI_SUCCESS) 
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if (iReply)
				strcpy(queryPrintMode, "ON");
			else 
				strcpy(queryPrintMode, "OFF");
			break;

		case 14:  //Turn printing of softkeys ON/OFF
			if ((strcmp(strp_printModeA, "OFF"))&&(strcmp(strp_printModeA, "ON")))
				return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);
			errStatus = viPrintf(vi, "PSOFT%s;\n",strp_printModeA);
			break;

		case 15:  //Baud rate (for serial printing)
			if (getChoice(vi,strp_printModeA,baud_sz,bauds,&idx)) 
				return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);
			errStatus = viPrintf(vi, "PRNTRBAUD%s;\n",strp_printModeA);
			break;

		case 16:  //Query baud rate (for serial printing)
			if ((errStatus = viQueryf(vi, "PRNTRBAUD?;\n","%lf",&baudrate)) < VI_SUCCESS) 
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			// Baud rate is in floating point format, so let's get rid of the extra 0's
			sprintf(queryPrintMode, "%.0lf", baudrate);
			break;

		case 17:  //Type of handshake (for serial printing)
			if (!strcmp(strp_printModeA, "XON-XOFF"))  strcpy(handshake,"XON");	
			else if (!strcmp(strp_printModeA, "DTR-DSR"))  strcpy(handshake,"DTR");	
			else 
				return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);
			errStatus = viPrintf(vi, "PRNHNDSHK%s;\n",handshake);
			break;

		case 18:  //Query handshake type (for serial printing)
			if ((errStatus = viQueryf(vi, "PRNHNDSHK?;\n","%i",&iReply)) < VI_SUCCESS) 
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if (iReply)
				strcpy(queryPrintMode,"DTR-DSR");	
			else
				strcpy(queryPrintMode,"XON-XOFF");	
			break;

		case 19:  //Use default printer setup
			errStatus = viPrintf(vi, "DEFLPRINT;\n");
			break;

		/* Add when VISA implements support for GPIB Pass Control */
		//	case 20: case 21:  //GPIB address
		//	errStatus = viPrintf(vi, "ADDRPRIN %s;\n", strp_printModeA);
		//	break;
		} /* end switch */

	if (errStatus < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);
	else
		return statusUpdate(vi, thisPtr, VI_SUCCESS);

	}    /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_calkit
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Sets or returns the calibration kit that is currently selected.
 * 
 * PARAM 1 : ViSession vi
 * IN        Instrument Handle returned from ag875x_init().
 *
 * PARAM 2 : ViString calkitRequest. Cal Kit type, or query (see codes below). 
 * IN		 
 *
 * PARAM 3 : ViPString currentCalkit. 
 * OUT       When 'request' is a query, returns the identity of the active cal 
 *           kit.
 *           
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_calkit( 
        ViSession vi, 
		ViString calkitRequest,
		ViPString currentCalkit)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_TRUE;

	ViInt32 choice = 0;
	ViInt32 iReply = 0;
	ViInt32 iIndex = 0;
	ViChar selection[40], cur_ID[15], rev[15];
	ViReal64 rev_value = 0.0;
	const ViInt32 size = 11;
	ViString req[11];
	ViString meas[11][2]= {"7-16"         , "716",
							 "2.4MM"        , "24MM",
							 "2.92MM"       , "292MM",
							 "2.92MM STAR"  , "292S",
							 "3.5MM TRL"    , "TRLK",
							 "3.5MM C"      , "35MC",
							 "3.5MM D"      , "35MD",
							 "7MM"          , "7MM",
							 "N50"          , "N50",
							 "N75"          , "N75",
							 "USER DEFINED" , "USED"
							};
	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_calkit" );

	/*  Parameter checks */
	ag875x_CHK_PTR(calkitRequest, VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(currentCalkit, VI_ERROR_PARAMETER3);

	strip(calkitRequest,selection);
	if (strcmp(selection, "?")) //  NOT a query!, set cal kit type
		{ 
		for (iIndex=0;  iIndex<size; iIndex++) //Copy to single dimension array
			 req[iIndex]= meas[iIndex][0];
		if (getChoice(vi,calkitRequest,size,req,&choice)) 
			return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);
		if ((!strcmp(selection,"2.4MM"))||(!strcmp(selection,"2.92MM"))||
				(!strcmp(selection,"2.92MM STAR"))||
				(!strcmp(selection,"3.5MM C"))||
				(!strcmp(selection,"3.5MM TRL"))||
				(!strcmp(selection,"7-16")))
			{

			if ((errStatus = ag875x_readID(vi, cur_ID, "MODEL?", 0)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			if ((errStatus = ag875x_readID(vi, rev, "REV_VALUE?", &rev_value)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			if ((!strcmp(cur_ID,"8753D"))&&(rev_value < 6.12)) // 8753D	Firmware 5.48 or lower
				{
				if (!strcmp(selection,"3.5MM C"))
					{
					if ((errStatus = viPrintf(vi, "CALK35MM;\n")) < VI_SUCCESS)
						return statusUpdate(vi, thisPtr, errStatus);
					}
				else	
					return statusUpdate(vi, thisPtr, ag875x_CALKIT_NOT_SUPPORTED);
				}
			else if ((!strcmp(selection,"7-16"))&&(rev_value < 7.60)) // 7-16 is only on Firmware 7.60 or higher
				{
				return statusUpdate(vi, thisPtr, ag875x_CALKIT_NOT_SUPPORTED);
				}
			else
				{
				if ((errStatus = viPrintf(vi, "CALK%s;\n",meas[choice][1])) < VI_SUCCESS)
					return statusUpdate(vi, thisPtr, errStatus);
				}
			}
		else
			{
			if ((errStatus = viPrintf(vi, "CALK%s;\n",meas[choice][1])) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			}
		}
	else // Read Kit Type
		{
		iReply = 0; iIndex = 6;
		while (!iReply)
			{
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viQueryf(vi, "CALK%s?;\n","%i",meas[iIndex][1],&iReply)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			if (iReply)
				strcpy(currentCalkit,meas[iIndex][0]);
			iIndex++;
			if (iIndex == 11)
				break;
			}
		if (!iReply)
			{
			if ((errStatus = ag875x_readID(vi, cur_ID, "MODEL?", 0)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			if ((errStatus = ag875x_readID(vi, rev, "REV_VALUE?", &rev_value)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			if ((!strcmp(cur_ID,"8753D"))&&(rev_value < 6.12)) // 8753D	Firmware 5.48 or lower
				strcpy(currentCalkit,"3.5MM C");
			else
				{
				iReply = 0; iIndex = 1;
				while (!iReply)
					{
					if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
						ag875x_LOG_STATUS( vi, thisPtr, errStatus );
					if ((errStatus = viQueryf(vi, "CALK%s?;\n","%i",meas[iIndex][1],&iReply)) < VI_SUCCESS)
						return statusUpdate(vi, thisPtr, errStatus);
					if (iReply)
						strcpy(currentCalkit,meas[iIndex][0]);
					iIndex++;
					if (iIndex == 6)
						break;
					}
				if (!iReply) // Remaining possibility is 7-16 (Firmware 7.60 or higher)
					strcpy(currentCalkit,"7-16");
				}
			}
		}
	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */



/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_message
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Writes a message to the instrument's display using a subset of
 *           HPGL commands.  The message is written below the title area of
 *           the display, and can be used to prompt a user to do something such
 *           as to connect a specific calibration standard to the test port(s).
 *           The function can be used to clear an existing message; just pass 
 *           an empty (i.e., null, "") string as the message.  
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *            
 * PARAM 2 : ViString message.
 * IN        Message to be sent to the instrument's display, 
 *
 *           
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_message( 
        ViSession vi, 
		ViString message)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 iNotDone = 0;
	ViSession defRM = 0;
	ViSession viDisp = 0;
	ViInt16 iIndex = 0;
	ViInt16 iHpibAddrOfDisplay = 0;
	ViChar instDesc[20], parsed[20], dispDesc[20];

	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_message" );

	/*  Parameter checks */
	ag875x_CHK_PTR(message, VI_ERROR_PARAMETER2);

	strcpy(instDesc, thisPtr->inst_rsrc_name);  // VISA resource descriptor for the instrument

	for (iIndex = 1; iIndex <= 3; iIndex++) // extract the GPIB address from the descriptor
		errStatus = ag875x_stringParse(vi, instDesc, parsed, &iNotDone, ":");

	if (atoi(parsed) % 2) // determine the current GPIB address of the instrument's display
		iHpibAddrOfDisplay = atoi(parsed) - 1;
	else
		iHpibAddrOfDisplay = atoi(parsed) + 1;

	sprintf(dispDesc, "GPIB0::%i::INSTR", iHpibAddrOfDisplay); // create descriptor for the display

	viDisp = VI_NULL;

			/*************************************************************/
			/* Find the Default Resource Manager.  Potential errors are  */
			/*   VI_ERROR_SYSTEM_ERROR, VI_ERROR_ALLOC                   */
			/*************************************************************/
	if ((errStatus = viOpenDefaultRM(&defRM)) < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);

			/*************************************************************/
			/* Open a session to the instrument's display.  Potential    */
			/* errors are:												 */
			/*   VI_ERROR_NSUP_OPER, VI_ERROR_INV_RSRC_NAME,             */
			/*   VI_ERROR_INV_ACC_MODE, VI_ERROR_RSRC_NFOUND,            */
			/*   VI_ERROR_ALLOC                                          */
			/*************************************************************/
	if ((errStatus = viOpen(defRM, dispDesc, VI_NULL, VI_NULL, &viDisp)) < VI_SUCCESS)
		{
		viClose(defRM);
		viDisp=VI_NULL;
		return statusUpdate(vi, thisPtr, errStatus);
		}

	// write the message to the display
	if ((errStatus = viPrintf(viDisp, "PG;PU;PA390,3700;PD;LB%s\003\n", message)) < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);

	// close the VISA session to the display
	if ((errStatus = viClose(viDisp)) < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}		/* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_measType_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : This function queries the type of measurement on the active 
 *           channel and queries the presence of any data conversion.
 *           See ag875x_measType function.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViPInt16 currentMeasType
 * OUT        
 *            This parameter returns the measurement type on the active channel: 
 *            whether it is an Input port, Input port ratio, Reflection, 
 *            Transmission or S-Parameter.  Input Ports are A, B and R. Input 
 *            Port Ratios are A/R, B/R and A/B.  Reflection and Transmission 
 *            apply to the non S-parameter instruments 
 *            (8719ET/8720ET/8722ET/8753ET).  Reflection is essentially the 
 *            same measurement as A/R, but is handled as a separate parameter.
 *            Transmission is essentially the same measurement as B/R, but is
 *            handled as a separate parameter.  The 8719D/ES, 8720D/ES, 
 *            8722D/ES and 8753D/E/ES are S-Parameter capable.  S-Parameters 
 *            are S11, S21, S12 and S22. 
 * 
 * PARAM 3 : ViPInt16 currentConversion
 * OUT        
 *            This parameter indicates whether the instrument is currently
 *            set up to perform a conversion of measured reflection or 
 *            transmission data.  If conversion is enabled, this value will 
 *            indicate the the type of conversion.  
 * 
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
/* Uses these string arrays that are declared in the code preceding the
   ag875x_measType function --
static const char * const ag875x_measType_inputSparam_a[] = {"AR","BR",
        "AB","MEASA","MEASB","MEASR","S11","S21","S12","S22",0};
static const char * const ag875x_measType_cnvtParam_a[] = {"CONVOFF",
        "CONVZREF","CONVZTRA","CONVYREF","CONVYTRA","CONV1DS",0}; */

ViStatus _VI_FUNC ag875x_measType_Q(ViSession vi,
  ViPInt16 cur_meas,
  ViPInt16 currentConversion)

	{
	ViStatus errStatus = VI_SUCCESS;
	struct ag875x_globals *thisPtr;
	ViInt32 iReply = 0;
	ViInt16 iIndex = 0;

	errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
	if (errStatus < VI_SUCCESS)
		{
		ag875x_LOG_STATUS( vi, 0, errStatus );
		}

	ag875x_DEBUG_CHK_THIS( vi, thisPtr );
	ag875x_CDE_INIT( "ag875x_measType_Q" );

	/*  Parameter checks */
	ag875x_CHK_PTR(cur_meas, VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(currentConversion, VI_ERROR_PARAMETER3);

		{
		iReply = 0; iIndex = 0;
		while (VI_TRUE)
			{
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
    
			errStatus = viQueryf(vi,"%s?;\n","%i",ag875x_measType_inputSparam_a[iIndex],&iReply);
			if (errStatus < VI_SUCCESS)
				{
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				}
			if (iReply)
				{
				*cur_meas = iIndex;
				break;
				}
			iIndex++;
			}

		iReply = 0; iIndex = 0;
		while (VI_TRUE)
			{
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
    
			errStatus = viQueryf(vi,"%s?;\n","%i",ag875x_measType_cnvtParam_a[iIndex],&iReply);
			if (errStatus < VI_SUCCESS)
				{
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				}
			if (iReply)
				{
				*currentConversion = iIndex;
				break;
				}
			iIndex++;
			}
		}

	ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_sweepType_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function will query the type of sweep that is currently
 *            enabled -- Linear Frequency, Log Frequency, CW Time, 
 *            List Frequency or Power Sweep. 
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViPInt16 currentSweepType
 * OUT        
 *            Returns the type of sweep.
 * 
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
static const char * const ag875x_sweepType_swpType_a[] = {"LINFREQ",
        "LOGFREQ","CWTIME","LISFREQ","POWS",0}; 

ViStatus _VI_FUNC ag875x_sweepType_Q(
		ViSession vi,
		ViPInt16 currentSweepType)
	
	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;
    ViInt32 iReply = 0;
    ViInt16 iIndex = 0;

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		{
		ag875x_LOG_STATUS( vi, 0, errStatus );
		}

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_sweepType_Q" );

	/*  Parameter checks */
	ag875x_CHK_PTR(currentSweepType, VI_ERROR_PARAMETER2);

		{
		iReply = 0; iIndex = 0;
		while (VI_TRUE)
			{
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
        
			errStatus = viQueryf(vi,"%s?;\n","%i",ag875x_sweepType_swpType_a[iIndex],&iReply);
			if (errStatus < VI_SUCCESS)
				{
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
				}
			if (iReply)
				{
				*currentSweepType = iIndex;
				break;
				}
			iIndex++;
			}
		}

    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_trigger
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Sets or reads hold/continuous trigger mode.
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *
 * PARAM 2 : ViString triggerRequest. (OPTIONAL, default=ag875x_TRIG_SINGLE)
 * IN        Choices are HOLD, CONTINUOUS, or ?.  
 *
 * PARAM 3 : ViInt16 triggerChannel. (OPTIONAL, default=1) 
 * IN        Indicates channel number.
 *			 
 * PARAM 4 : ViPBoolean triggerMode. (OPTIONAL) 
 * OUT       If 'request' is a query, this parameter indicates if trigger 
 *           continuous is active (0=NO; 1=Yes)  
 *           
 * RETURN  :  VI_SUCCESS:No error. Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_trigger( 
        ViSession vi, 
		ViString triggerRequest,
		ViInt16 triggerChannel,
		ViPBoolean triggerMode)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 choice = 0;
	ViInt32 currentChannel = 0;
	const ViInt32 size = 5;
	
	ViString req[]= {"HOLD", "CONT", "CONTINUOUS", "?", "STATUS?" 
						};

	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_trigger" );

	/*  Parameter checks */
	ag875x_CHK_PTR(triggerRequest, VI_ERROR_PARAMETER2);
	ag875x_CHK_ENUM(triggerChannel, ag875x_CH4,VI_ERROR_PARAMETER3);

	if(getChoice(vi,triggerRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	// Save the current active channel number, so it may be restored later.
	if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Temporarily change to requested channel.
	if ((errStatus = ag875x_channelSelect(vi, triggerChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	switch (choice)
		{
		case 0: //Hold
			if ((errStatus = viPrintf(vi, "HOLD\n")) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;
		case 1: case 2: //CONT
			if ((errStatus = viPrintf(vi, "CONT\n")) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;
		case 3: case 4: //Status?
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
	   		if ((errStatus = viQueryf(vi, "CONT?\n", "%hd", triggerMode)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;
		}

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Restore instrument to original channel number.
	if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);

	ag875x_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_display_Q
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE :  This function queries the various parameters related to the
 *            channel display.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViInt16 displayQChannel
 * IN        
 *            Selects the channel number.  Channels 3 and 4 are valid only on 
 *            8753/8719/8720/8722 Firmware Revisions 7.40 and higher.
 * 
 * PARAM 3 : ViPInt16 displayQMode
 * OUT        
 *            This indicates what is being displayed (data and/or trace memory)
 *            on the active Channel, and if trace math is applied.
 * 
 * PARAM 4 : ViPInt16 displayQFormat
 * OUT        
 *            Returns the Display Format such as Log Magnitude, Phase, Delay,
 *            Smith Chart, Polar, Linear Magnitude, Real, Imaginary, SWR for
 *            the selected channel & selected display mode.
 * 
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
/* Uses these string arrays that are declared in the code preceding the
   ag875x_displaySelect function --
static const char * const ag875x_displaySelect_dispMode_a[] = {"DISPDATA",
        "DISPMEMO","DISPDATM","DISPDDM","DISPDMM","DATI",0};
static const char * const ag875x_displaySelect_dispFormat_a[] = {"LOGM",
        "PHAS","DELA","SMIC","POLA","LINM","REAL","IMAG","SWR",0}; */

ViStatus _VI_FUNC ag875x_display_Q(ViSession vi,
  ViInt16 displayQ,
  ViPInt16 displayQMode,
  ViPInt16 displayQFormat)

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		{
		ag875x_LOG_STATUS( vi, 0, errStatus );
		}

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_display_Q" );

	/*  Parameter checks */
	ag875x_CHK_PTR(displayQMode, VI_ERROR_PARAMETER3);
	ag875x_CHK_PTR(displayQFormat, VI_ERROR_PARAMETER4);
    ag875x_CHK_ENUM(displayQ, ag875x_CH4,VI_ERROR_PARAMETER2);
		
		{
		ViInt32 iReply = 0;
		ViInt16 iIndex = 0;
		ViInt32 currentChannel = 0;
		
		// Save the current active channel number, so it may be restored later.
		if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		// Temporarily change to requested channel.
		if ((errStatus = ag875x_channelSelect(vi, displayQ)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		iReply = 0; iIndex = 0;
		while (VI_TRUE)
			{
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
        
			if ((errStatus = viQueryf(vi,"%s?;\n","%i",ag875x_displaySelect_dispMode_a[iIndex],&iReply)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			if (iReply)
				{
				*displayQMode = iIndex;
				break;
				}
			iIndex++;
			}

		iReply = 0; iIndex = 0;
		while (VI_TRUE)
			{
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
        
			if ((errStatus = viQueryf(vi,"%s?;\n","%i",ag875x_displaySelect_dispFormat_a[iIndex],&iReply)) < VI_SUCCESS)
				 ag875x_LOG_STATUS( vi, thisPtr, errStatus );

			if (iReply)
				{
				*displayQFormat = iIndex;
				break;
				}
			iIndex++;
			}

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		// Restore instrument to original channel number.
		if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);
		}

    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_auxChannel
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Enables/disables the auxiliary channel associated with the
 *           specified primary channel, or queries the enabled/disabled
 *           status of that auxiliary channel.
 *
 *    NOTE : This function only applies to analyzers having Firmware Revision 
 *           7.40 or higher.
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *
 * PARAM 2 : ViInt16 auxChannel. 
 * IN        Indicates primary channel number (ag875x_CH1 or ag875x_CH2).
 *			 
 * PARAM 3 : ViString auxChannelRequest. (OPTIONAL, default=ag875x_ON)
 * IN        Choices are ON, OFF or ?.  
 *
 * PARAM 4 : ViPBoolean auxChannelState. (OPTIONAL) 
 * OUT       If 'request' is a query, this parameter indicates if the 
 *           auxiliary channel associated with the specified primary channel
 *           is enabled (0=No; 1=Yes).  
 *           
 * RETURN  :  VI_SUCCESS:No error. Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_auxChannel( 
        ViSession vi, 
		ViInt16 auxChannel,
		ViString auxChannelRequest,
		ViPBoolean auxChannelState)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 choice = 0;
	ViInt32 currentChannel = 0;
	char cur_ID[15], message[ag875x_ERR_MSG_LENGTH];
	ViReal64 dFirmware = 0.0;
	const ViInt32 size = 4;
	char functionName[256] = {0};	// Used for error reporting.

	ViString req[]= {"ON", "OFF", "?", "STATUS?"};

	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_auxChannel" );
	// Used for error reporting
	sprintf(functionName, "ag875x_auxChannel");

	/*  Parameter checks */
	ag875x_CHK_PTR(auxChannelRequest, VI_ERROR_PARAMETER3);
	ag875x_CHK_ENUM(auxChannel, ag875x_CH2,VI_ERROR_PARAMETER2);

	if(getChoice(vi,auxChannelRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	if ((errStatus = ag875x_readID(vi, cur_ID, "REV_VALUE?", &dFirmware)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	if (compFirmwareVersions(dFirmware, "<", 7.40))
		{
		sprintf(message, "Function = %s", functionName);
		strcpy(thisPtr->errMessage, message);
		ag875x_LOG_STATUS( vi, thisPtr, ag875x_REQUIRES_4_PARM_FW );
		}

	// Save the current active channel number, so it may be restored later.
	if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	
	// Temporarily change to requested channel.
	if ((errStatus = ag875x_channelSelect(vi, auxChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	switch (choice)
		{
		case 0: //ON
			if ((errStatus = viPrintf(vi, "AUXCON\n")) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;

		case 1: //OFF
			if ((errStatus = viPrintf(vi, "AUXCOFF\n")) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;

		case 2: case 3: //Status?
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
	   		if ((errStatus = viQueryf(vi, "AUXC?\n", "%hd", auxChannelState)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;
		}

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Restore instrument to original channel number.
	if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);

	ag875x_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_readPowerMeterCal
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : This function reads the array of power meter calibration data from 
 *           the instrument.  Use the ag875x_writePowerMeterCal function to 
 *           write an array of power meter calibration data to the instrument.
 *
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViInt16 readPowerMeterCalChannel 
 * IN        
 *            Indicates primary channel number that the power meter calibration
 *            data will be read from.
 * 
 * PARAM 3 : ViReal64 _VI_FAR readPowerMeterCalArray[]
 * OUT       
 *            64 bit Real one-dimensional array to be read from the instrument.
 * 
 * PARAM 4 : ViInt16 readPowerMeterCalPoints
 * IN        
 *            Select the number of points in the sweep.  The 'readPowerMeterCalArray' 
 *            array passed to this function must be large enough to receive 
 *            this number of values.
 *
 * PARAM 5 : ViBoolean interpolatedMode
 * IN        
 *            Chooses either interpolated (ag875x_READ_INTERP_CAL) or 
 *            uninterpolated (ag875x_READ_UNINTERP_CAL) power meter calibration
 *            data.  Interpolated data will only be present if there has been 
 *            sufficient change in the instrument state to activate interpolation.
 *.
 *
 * 
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_readPowerMeterCal(ViSession vi,
  ViInt16 readPowerMeterCalChannel,
  ViReal64 _VI_FAR readPowerMeterCalArray[],
  ViInt16 readPowerMeterCalPoints,
  ViBoolean interpolatedMode)

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;
    ViInt16 inc = 0;
	ViInt32	oneSweepActive = 0;
	ViInt32	eachSweepActive = 0;
	ViInt16 instPointsNumber = 0;	// actual points value passed to instrument
	ViInt16 instChannelNumber = 0;	// actual channel number passed to instrument

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, 0, errStatus );

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_readPowerMeterCal" );

	/*  Parameter checks */
    ag875x_CHK_ENUM(readPowerMeterCalChannel, ag875x_CH2,VI_ERROR_PARAMETER2);
    ag875x_CHK_INT_RANGE(readPowerMeterCalPoints, ag875x_PNTS_03, ag875x_PNTS_1601, VI_ERROR_PARAMETER4);
    ag875x_CHK_BOOLEAN(interpolatedMode, VI_ERROR_PARAMETER5);

	// Enums start with 0, so set the number used by instrument.
	instChannelNumber = readPowerMeterCalChannel + 1;
	instPointsNumber = convertEnumToPoints(readPowerMeterCalPoints);

	// Query instrument to verify that a power meter calibration is active
	if ((errStatus = viQueryf(vi,"PWMCONES?;\n","%i",&oneSweepActive)) < VI_SUCCESS)
            ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	if ((errStatus = viQueryf(vi,"PWMCEACS?;\n","%i",&eachSweepActive)) < VI_SUCCESS)
            ag875x_LOG_STATUS( vi, thisPtr, errStatus );
	if (!oneSweepActive && !eachSweepActive)
		return ag875x_POWERMETER_CAL_INACTIVE;

    if (interpolatedMode)
		{
		if ((errStatus = viPrintf(vi, "POIN%hd;FORM4;OUTPIPMCL%i;\n", instPointsNumber, instChannelNumber)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		}
	else  // Uninterpolated state
		{
		if ((errStatus = viPrintf(vi, "FORM4;OUTPPMCAL%i;\n", instChannelNumber)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		}

	// Flush the read buffer just in case.
	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Read the array
	for (inc=0; inc < instPointsNumber; inc++)
		{
		errStatus = viScanf(vi, "%lf", &readPowerMeterCalArray[inc]);
		if (errStatus < VI_SUCCESS)
	   		ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		}
	// Flush the read buffer
	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_srq_enable
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Enables any number of bits in the analyzer's Status Byte,
 *           Event Status Register or Event Status Register B to generate
 *           a Service Request (SRQ) when a condition occurs that sets one of  
 *           those bits.
 * 
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViChar _VI_FAR statstr[]
 * IN       
 *            To contain string specifiers for all bits which will be enabled
 *            to generate an SRQ.  The list can be delimited however the user
 *            desires (using commas, spaces, etc.), but delimiting is not required.
 * 
 *            Bit Specifiers for Event Status Register:
 *               Bit Value       Specifier
 *           
 *                   1           "ESR_OPC"
 *                   2           "ESR_RQL"
 *                   4           "ESR_QYE"
 *                   8           "ESR_SQB"
 *                  16           "ESR_EXE"
 *                  32           "ESR_CME"
 *                  64           "ESR_URQ"
 *                 128           "ESR_PON"
 *
 *            Bit Specifiers for Event Status Register B:
 *               Bit Value       Specifier
 *           
 *                   1           "ESB_SNC"
 *                   2           "ESB_SRW"
 *                   4           "ESB_DEC"
 *                   8           "ESB_LF2"
 *                  16           "ESB_LF1"
 *                  32           "ESB_SF2"
 *                  64           "ESB_SF1"
 *                 128           "ESB_CPC"
 *
 *            For 8719/8720/8722/8753 analyzers having Firmware Revision 7.40
 *            or higher, Event Status Register B contains these additional bits:
 *               Bit Value       Specifier
 *           
 *                 256           "ESB_LF4"
 *                 512           "ESB_LF3"
 *                1024           "ESB_SF4"
 *                2048           "ESB_SF3"
 *
 *            Bit Specifiers for Status Byte:
 *               Bit Value       Specifier
 *           
 *                   8           "STB_CEQ"
 *                  16           "STB_MOQ"
 *                 128           "STB_PRS"
 *
 *
 * RETURN  :  VI_SUCCESS: No error. Non VI_SUCCESS: Indicates error
 *           condition. To determine error message, pass the return value to
 *           routine "ag875x_error_message".
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_srq_enable(
		ViSession vi,
		ViChar _VI_FAR statstr[])

	{
    ViStatus errStatus = VI_SUCCESS;
    struct ag875x_globals *thisPtr;

    errStatus = viGetAttribute(vi, VI_ATTR_USER_DATA, (ViAddr) &thisPtr);
    if (errStatus < VI_SUCCESS)
		{
		ag875x_LOG_STATUS( vi, 0, errStatus );
		}

    ag875x_DEBUG_CHK_THIS( vi, thisPtr );
    ag875x_CDE_INIT( "ag875x_srq_enable" );

	/*  Parameter checks */
	ag875x_CHK_PTR(statstr, VI_ERROR_PARAMETER2);

		{     
		ViInt16 bitmask = 0;
		ViBoolean esr = VI_FALSE, esb = VI_FALSE;

	    if ((errStatus = viPrintf(vi, "CLES;\n")) < VI_SUCCESS)
		   ag875x_LOG_STATUS(vi, thisPtr, errStatus);

		if (strstr(statstr, "ESR_PON") != NULL)
			{
			bitmask += 128;
			esr = VI_TRUE;
			}

		if (strstr(statstr, "ESR_URQ") != NULL)
			{
			bitmask += 64;
			if (!esr) esr = VI_TRUE;
			}

		if (strstr(statstr, "ESR_CME") != NULL)
			{
			bitmask += 32;
			if (!esr) esr = VI_TRUE;
			}

		if (strstr(statstr, "ESR_EXE") != NULL)
			{
			bitmask += 16;
			if (!esr) esr = VI_TRUE;
			}

		if (strstr(statstr, "ESR_SQB") != NULL)
			{
			bitmask += 8;
			if (!esr) esr = VI_TRUE;
			}

		if (strstr(statstr, "ESR_QYE") != NULL)
			{
			bitmask += 4;
			if (!esr) esr = VI_TRUE;
			}

		if (strstr(statstr, "ESR_RQL") != NULL)
			{
			bitmask += 2;
			if (!esr) esr = VI_TRUE;
			}

		if (strstr(statstr, "ESR_OPC") != NULL)
			{
			bitmask += 1;
			if (!esr) esr = VI_TRUE;
			}

	    if ((errStatus = viPrintf(vi, "ESE%i;\n",bitmask)) < VI_SUCCESS)
		   ag875x_LOG_STATUS(vi, thisPtr, errStatus);

		bitmask = 0;

		if (strstr(statstr, "ESB_SF3") != NULL)
			{
			bitmask += 2048;
			esb = VI_TRUE;
			}

		if (strstr(statstr, "ESB_SF4") != NULL)
			{
			bitmask += 1024;
			if (!esb) esb = VI_TRUE;
			}

		if (strstr(statstr, "ESB_LF3") != NULL)
			{
			bitmask += 512;
			if (!esb) esb = VI_TRUE;
			}

		if (strstr(statstr, "ESB_LF4") != NULL)
			{
			bitmask += 256;
			if (!esb) esb = VI_TRUE;
			}

		if (strstr(statstr, "ESB_CPC") != NULL)
			{
			bitmask += 128;
			if (!esb) esb = VI_TRUE;
			}

		if (strstr(statstr, "ESB_SF1") != NULL)
			{
			bitmask += 64;
			if (!esb) esb = VI_TRUE;
			}

		if (strstr(statstr, "ESB_SF2") != NULL)
			{
			bitmask += 32;
			if (!esb) esb = VI_TRUE;
			}

		if (strstr(statstr, "ESB_LF1") != NULL)
			{
			bitmask += 16;
			if (!esb) esb = VI_TRUE;
			}

		if (strstr(statstr, "ESB_LF2") != NULL)
			{
			bitmask += 8;
			if (!esb) esb = VI_TRUE;
			}

		if (strstr(statstr, "ESB_DEC") != NULL)
			{
			bitmask += 4;
			if (!esb) esb = VI_TRUE;
			}

		if (strstr(statstr, "ESB_SRW") != NULL)
			{
			bitmask += 2;
			if (!esb) esb = VI_TRUE;
			}

		if (strstr(statstr, "ESB_SNC") != NULL)
			{
			bitmask += 1;
			if (!esb) esb = VI_TRUE;
			}

	    if ((errStatus = viPrintf(vi, "ESNB%i;\n",bitmask)) < VI_SUCCESS)
		   ag875x_LOG_STATUS(vi, thisPtr, errStatus);

		bitmask = 0;

		if (strstr(statstr, "STB_PRS") != NULL)
           bitmask += 128;

		if (esr)
           bitmask += 32;

		if (strstr(statstr, "STB_MOQ") != NULL)
           bitmask += 16;

		if (strstr(statstr, "STB_CEQ") != NULL)
           bitmask += 8;

		if (esb)
           bitmask += 4;

	    if ((errStatus = viPrintf(vi, "SRE%i;\n",bitmask)) < VI_SUCCESS)
		   ag875x_LOG_STATUS(vi, thisPtr, errStatus);
		}

    ag875x_LOG_STATUS( vi, thisPtr, VI_SUCCESS );
	}        /* ----- end of function ----- */

/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_plotToFile
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : This function will plot the contents of the analyzer display as an
 *           HPGL plot string to an ASCII file. A disk drive on the computer
 *           can be specified to receive the file, or alternatively the 
 *           analyzer's internal floppy drive can receive it.
 *
 * PARAM 1 : ViSession vi
 * IN        
 *            Instrument Handle returned from ag875x_init().
 * 
 * PARAM 2 : ViString plotFile
 * IN        
 *            Name and location (for example, C:\temp\myPlot) where the file
 *            will be stored on the computer.  To store to the analyzer's 
 *            internal floppy drive instead, use "INT:" as the drive specifier
 *            (for example, INT:myPlot).  NOTE:  Directories (folders) cannot
 *            be used when storing to the analyzer's drive!
 * 
 * PARAM 3 : ViBoolean includeSoftkeys 
 * IN        
 *            Specifies whether to include softkey labels in the plot (VI_TRUE)
 *            or not (VI_FALSE).
 * 
 * RETURN  :  VI_SUCCESS:No error. Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 * 
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_plotToFile( 
        ViSession vi, 
		ViString plotFile,
		ViBoolean includeSoftkeys)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;

	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_plotToFile" );

	/*  Parameter checks */
	ag875x_CHK_PTR(plotFile, VI_ERROR_PARAMETER2);
	ag875x_CHK_BOOLEAN(includeSoftkeys,VI_ERROR_PARAMETER3);
	
		{
		ViString softStat[] = { "OFF", "ON" };
		ViChar   filename[80];
		ViPChar  nameptr;
		ViChar   plotStr[10000];
		FILE*    fptr;
		int      length = 10000;
		
		/* Include softkey labels in plot? */
		if ((errStatus = viPrintf(vi, "PSOFT%s;\n", softStat[includeSoftkeys])) < VI_SUCCESS)
			return statusUpdate(vi, thisPtr, errStatus);

		strcpy(filename, plotFile);
		toUpper(filename);

		if ((nameptr = strstr(filename, "INT:")) != NULL)
			{
			/* Set up internal disk to receive plot */
			/* nameptr + 4 will point to first character after INT: */
			if ((errStatus = viPrintf(vi, "INTD;PLTPRTDISK;TITP\"%s\";PLOT;\n", nameptr + 4)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			/* Wait until analyzer has output entire plot string before continuing */
			if ((errStatus = ag875x_outputf(vi, 3, "NOOP")) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			}
		else
			{
       		/* Do not interpret linefeed as terminator for the read function */
			if ((errStatus = viSetAttribute(vi, VI_ATTR_TERMCHAR_EN,
											VI_FALSE)) < VI_SUCCESS)
           		ag875x_LOG_STATUS( vi, thisPtr, errStatus );
       		/* Make sure read function will terminate when EOI is encountered */
			if ((errStatus = viSetAttribute(vi, VI_ATTR_SUPPRESS_END_EN,
											VI_FALSE)) < VI_SUCCESS)
           		ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			if ((errStatus = viQueryf(vi, "OUTPPLOT;\n", "%t", plotStr)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);

			// Wait 100 ms to fix timeout problem caused viQueryf above.
			// I don't know what is causing the problem, could be the 
			// bus isn't ready yet.
			Sleep(100);
		

			/* Determine actual length of the plot string */
			length = strlen(plotStr);

			/* Store plot data to a disk file */
			if (( fptr = fopen( plotFile, "wb" )) == NULL) /* Open file for write */
				{ 
				return statusUpdate(vi, thisPtr, ag875x_FILE_OPEN_ERROR);
				}
			else
				{
				fwrite( plotStr, length, 1, fptr ); /* Write plot string to file */
				fclose( fptr );                     /* Close the file */
				}
			}
		}

	ag875x_LOG_STATUS(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_calInterpolation
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Sets or returns ON/OFF state of calibration
 *           interpolation.
 * 
 * PARAM 1 : ViSession vi
 * IN        Instrument Handle returned from ag875x_init().
 *           
 * PARAM 2 : ViString setCalInterpolationStateRequest (OPTIONAL; default=ag875x_SET)
 * IN        The string that essentially tells this function what
 *           to do.  If unsure of exactly what this function does
 *           in response to a specific request string, refer to
 *           this function in the Windows help file for this
 *           driver.
 *   
 * PARAM 3 : ViBoolean setCalInterpolationState (OPTIONAL; default=ag875x_CAL_INTERP_ON)
 * IN        As determined by 'request', this is for setting the
 *           ON/OFF state of cal interpolation.
 *
 * PARAM 4 : ViInt16 calInterpolationChannel (OPTIONAL; default=ag875x_CH1)
 * IN        Selects the channel number.
 *
 * PARAM 5 : ViPBoolean getCalInterpolationState (OPTIONAL; default=0)
 * OUT       When 'request' is a query, this indicates the current
 *           ON/OFF state of cal interpolation.  
 *
 *           
 * RETURN  :  VI_SUCCESS:No error. Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *           
 *           
 *-----------------------------------------------------------------------------
 */
static const char * const ag875x_calInterpolation_state_a[] = {"OFF",
        "ON",0};

ViStatus _VI_FUNC ag875x_calInterpolation( 
        ViSession vi, 
		ViString calInterpolationRequest,
		ViBoolean setCalInterpolationState,
		ViInt16 calInterpolationChannel,
		ViPBoolean getCalInterpolationState)
	
	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 choice = 0;
	ViInt32 currentChannel = 0;

	const ViInt32 size = 2;

	ViString req[]= { "SET", "?"
						};

	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_calInterpolation" );

	/*  Parameter checks */
	ag875x_CHK_PTR(calInterpolationRequest, VI_ERROR_PARAMETER2);
	ag875x_CHK_ENUM(calInterpolationChannel, ag875x_CH4,VI_ERROR_PARAMETER4);

	if(getChoice(vi,calInterpolationRequest,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	// Save the current active channel number, so it may be restored later.
	if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Temporarily change to requested channel.
	if ((errStatus = ag875x_channelSelect(vi, calInterpolationChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	switch (choice)
		{	
		case 0://Set Interpolation
			ag875x_CHK_BOOLEAN(setCalInterpolationState,VI_ERROR_PARAMETER3);
			if ((errStatus = viPrintf(vi, "CORI%s;\n",ag875x_calInterpolation_state_a[setCalInterpolationState])) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;

		case 1://Query Interpolation
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
   			if ((errStatus = viQueryf(vi, "CORI?;\n","%hd",getCalInterpolationState)) < VI_SUCCESS)
				return statusUpdate(vi, thisPtr, errStatus);
			break;
		}

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Restore instrument to original channel number.
	if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */

/*****************************************************************************/
/*-----------------------------------------------------------------------------
 * FUNC    : ViStatus _VI_FUNC ag875x_limitTableQuery
 *-----------------------------------------------------------------------------
 * 
 * PURPOSE : Returns the contents of the existing limit table (if any).
 *			 
 * 
 * PARAM 1 : ViSession vi.
 * IN        Instrument Handle returned from ag875x_init().
 *
 * PARAM 2 : ViPInt16 limitTableQuerySegments.  
 * OUT       Returns the number of segments in the limit table.
 *
 * PARAM 3 : ViReal64 _VI_FAR limitTableQueryLimits[].  64 bit Real one-dimensional array to
 * OUT       receive the limit table (1 to 18 segments; 4 input values per segment) 
 *           formatted as follows:
 *
 *           [ 0|1|2, Stimulus, Upper Limit (or Middle), Lower Limit (or Delta) ]  
 *
 *           where 0/1/2 is 0 for Sloping Line, 1 = Flat Line and 2 = Single Point.
 *           Repeated X number of times within the array where X equals the number of 
 *           segments.  
 * 
 * PARAM 4 : ViString limitTableQueryMode (OPTIONAL; default=ag875x_LIM_TABLE_UPPER_LOWER)
 * IN        String describing how to read the limits: as upper and lower, or as
 *           middle and delta. 
 *
 *           Macro name                     Value
 *           -----------------------------  --------------                 
 *           ag875x_LIM_TABLE_UPPER_LOWER   "UPPER/LOWER"
 *           ag875x_LIM_TABLE_MIDDLE_DELTA  "MIDDLE/DELTA"
 *
 * PARAM 5 : ViInt16 limitTableQueryChannel. (OPTIONAL; default=ag875x_CH1)
 * IN        Indicates channel number.   
 *
 *
 * RETURN  :  VI_SUCCESS:No error.Non VI_SUCCESS:Indicates error condition.
 *            To determine error message, pass the return value to routine
 *            "ag875x_error_message"
 *-----------------------------------------------------------------------------
 */
ViStatus _VI_FUNC ag875x_limitTableQuery( 
        ViSession vi, 
		ViPInt16 limitTableQuerySegments,
		ViReal64 _VI_FAR limitTableQueryLimits[],
		ViString limitTableQueryMode,
		ViInt16 limitTableQueryChannel)

	{
	struct ag875x_globals *thisPtr;
	ViStatus errStatus = VI_SUCCESS;
	ViInt32 choice = 0;
	ViInt32 iSegIndex = 0;
	ViInt32 iElem = 0;
	ViInt32 iReply = 0;
	ViReal32 fSeg = 0.0;
	const ViInt32 size = 2;
	ViInt32 currentChannel = 0;

	ViString req[]= { "UPPER/LOWER", "MIDDLE/DELTA" }; 

	GET_GLOBALS(vi, thisPtr)
	ag875x_CDE_INIT( "ag875x_limitTableQuery" );

	/*  Parameter checks */
	ag875x_CHK_PTR(limitTableQuerySegments, VI_ERROR_PARAMETER2);
	ag875x_CHK_PTR(limitTableQueryLimits, VI_ERROR_PARAMETER3);
	ag875x_CHK_PTR(limitTableQueryMode, VI_ERROR_PARAMETER4);
	ag875x_CHK_ENUM(limitTableQueryChannel, ag875x_CH4,VI_ERROR_PARAMETER5);

	if(getChoice(vi,limitTableQueryMode,size,req,&choice)) 
		return statusUpdate(vi,thisPtr,ag875x_NO_MATCH_ERROR);

	// Save the current active channel number, so it may be restored later.
	if ((errStatus = getCurrentChannel(vi, &currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		 
	// Temporarily change to requested channel.
	if ((errStatus = ag875x_channelSelect(vi, limitTableQueryChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Begin editing
	if ((errStatus = viPrintf(vi, "EDITLIML;\n")) < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);

	iSegIndex = 1;

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	//Is there an existing limit table?
	if ((errStatus = viQueryf(vi,"SEDI%i;SEDI?;\n","%f",iSegIndex,&fSeg)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	while ((int)fSeg == iSegIndex)  
		{
		iElem = iSegIndex-1;

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		//Limit type?
		if ((errStatus = viQueryf(vi,"LIMTSL?;\n","%i",&iReply)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		if (iReply)
			limitTableQueryLimits[iElem*4] = 0; // Sloped line
		else
			{
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viQueryf(vi,"LIMTFL?;\n","%i",&iReply)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if (iReply)
				limitTableQueryLimits[iElem*4] = 1; // Flat line
			else
				limitTableQueryLimits[iElem*4] = 2; // Single point
			}

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		//Limit stimulus?
		if ((errStatus = viQueryf(vi,"LIMS;OUTPACTI;\n","%lf",&limitTableQueryLimits[iElem*4+1])) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		if (!choice) // If upper and lower were requested
			{
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			//Upper limit?
			if ((errStatus = viQueryf(vi,"LIMU;OUTPACTI;\n","%lf",&limitTableQueryLimits[iElem*4+2])) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			//Lower limit?
			if ((errStatus = viQueryf(vi,"LIML;OUTPACTI;\n","%lf",&limitTableQueryLimits[iElem*4+3])) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}
		else // must be middle and delta
			{
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			//Middle limit?
			if ((errStatus = viQueryf(vi,"LIMM;OUTPACTI;\n","%lf",&limitTableQueryLimits[iElem*4+2])) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			//Delta limit?
			if ((errStatus = viQueryf(vi,"LIMD;OUTPACTI;\n","%lf",&limitTableQueryLimits[iElem*4+3])) < VI_SUCCESS)
				ag875x_LOG_STATUS( vi, thisPtr, errStatus );
			}

		iSegIndex++;

		if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );

		//Test if recently edited segment was last segment in list
		if ((errStatus = viQueryf(vi,"SEDI%i;SEDI?;\n","%f",iSegIndex,&fSeg)) < VI_SUCCESS)
			ag875x_LOG_STATUS( vi, thisPtr, errStatus );
		} /* end while loop */

	//Make sure active entry area has been turned off before we exit the function
	if ((errStatus = viPrintf(vi, "ENTO;\n")) < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);

	*limitTableQuerySegments = (int)fSeg;

	//Finished editing
	if ((errStatus = viPrintf(vi, "EDITDONE;\n")) < VI_SUCCESS)
		return statusUpdate(vi, thisPtr, errStatus);

	if ((errStatus = viFlush(vi, VI_READ_BUF_DISCARD)) < VI_SUCCESS)
		ag875x_LOG_STATUS( vi, thisPtr, errStatus );

	// Restore instrument to original channel number.
	if ((errStatus = restoreOrignialChannel(vi, currentChannel)) < VI_SUCCESS)
		ag875x_LOG_STATUS(vi, thisPtr, errStatus);

	return statusUpdate(vi, thisPtr, VI_SUCCESS);
	}        /* ----- end of function ----- */



