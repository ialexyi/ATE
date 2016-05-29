/****************************************************************************
 *                       Kikusui PIA4800 Series Power Supply Controller
 *---------------------------------------------------------------------------
 *   (c)2005 Kikusui Electronics Corp.  All Rights Reserved.         
 *---------------------------------------------------------------------------
 *                                                                          
 * Title:    ki4800.h                                        
 * Purpose:  Kikusui PIA4800 Series Power Supply Controller
 *           instrument driver declarations.                                
 *                                                                          
 ****************************************************************************/

#ifndef __KI4800_HEADER
#define __KI4800_HEADER

#include <ivi.h>
#include <ividcpwr.h>

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

/****************************************************************************
 *----------------- Instrument Driver Revision Information -----------------*
 ****************************************************************************/
#define KI4800_DRIVER_PREFIX					"ki4800"

#define KI4800_MAJOR_VERSION					2     /* Instrument driver major version   */
#define KI4800_MINOR_VERSION					0     /* Instrument driver minor version   */
                                                                
#define KI4800_CLASS_SPEC_MAJOR_VERSION			2     /* Class specification major version */
#define KI4800_CLASS_SPEC_MINOR_VERSION			0     /* Class specification minor version */

#define KI4800_SUPPORTED_INSTRUMENT_MODELS		"PIA4810,PIA4830"
#define KI4800_DRIVER_VENDOR					"Kikusui"
#define KI4800_DRIVER_DESCRIPTION				"Kikusui PIA4800 Series Power Supply Controller"


/**************************************************************************** 
 *---------------------------- Attribute Defines ---------------------------* 
 ****************************************************************************/

/*- IVI Inherent Instrument Attributes ---------------------------------*/    

/* User Options */
#define KI4800_ATTR_RANGE_CHECK                   IVI_ATTR_RANGE_CHECK                    /* ViBoolean */
#define KI4800_ATTR_QUERY_INSTRUMENT_STATUS       IVI_ATTR_QUERY_INSTRUMENT_STATUS        /* ViBoolean */
#define KI4800_ATTR_CACHE                         IVI_ATTR_CACHE                          /* ViBoolean */
#define KI4800_ATTR_SIMULATE                      IVI_ATTR_SIMULATE                       /* ViBoolean */
#define KI4800_ATTR_RECORD_COERCIONS              IVI_ATTR_RECORD_COERCIONS               /* ViBoolean */

#define KI4800_ATTR_DRIVER_SETUP                  IVI_ATTR_DRIVER_SETUP                   /* ViString */
#define KI4800_ATTR_INTERCHANGE_CHECK             IVI_ATTR_INTERCHANGE_CHECK              /* ViBoolean */

/* Instrument Capabilities */
#define KI4800_ATTR_CHANNEL_COUNT                 IVI_ATTR_CHANNEL_COUNT                  /* ViInt32,  read-only  */

/* Driver Information  */
#define KI4800_ATTR_SPECIFIC_DRIVER_PREFIX        IVI_ATTR_SPECIFIC_DRIVER_PREFIX         /* ViString, read-only  */
#define KI4800_ATTR_SUPPORTED_INSTRUMENT_MODELS   IVI_ATTR_SUPPORTED_INSTRUMENT_MODELS    /* ViString, read-only  */
#define KI4800_ATTR_GROUP_CAPABILITIES            IVI_ATTR_GROUP_CAPABILITIES             /* ViString, read-only  */
#define KI4800_ATTR_INSTRUMENT_MANUFACTURER       IVI_ATTR_INSTRUMENT_MANUFACTURER        /* ViString, read-only  */
#define KI4800_ATTR_INSTRUMENT_MODEL              IVI_ATTR_INSTRUMENT_MODEL               /* ViString, read-only  */
#define KI4800_ATTR_INSTRUMENT_FIRMWARE_REVISION  IVI_ATTR_INSTRUMENT_FIRMWARE_REVISION   /* ViString, read-only  */
#define KI4800_ATTR_SPECIFIC_DRIVER_REVISION      IVI_ATTR_SPECIFIC_DRIVER_REVISION       /* ViString, read-only  */
#define KI4800_ATTR_SPECIFIC_DRIVER_VENDOR        IVI_ATTR_SPECIFIC_DRIVER_VENDOR         /* ViString, read-only  */
#define KI4800_ATTR_SPECIFIC_DRIVER_DESCRIPTION   IVI_ATTR_SPECIFIC_DRIVER_DESCRIPTION    /* ViString, read-only  */
#define KI4800_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION /* ViInt32, read-only */
#define KI4800_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION /* ViInt32, read-only */

/* Advanced Session Information */
#define KI4800_ATTR_LOGICAL_NAME                  IVI_ATTR_LOGICAL_NAME                   /* ViString, read-only  */
#define KI4800_ATTR_IO_RESOURCE_DESCRIPTOR        IVI_ATTR_IO_RESOURCE_DESCRIPTOR         /* ViString, read-only  */
  

/*- Instrument-Specific Attributes -------------------------------------*/

/*- Basic Instrument Operations ----------------------------------------*/

/*  viReal64    */
#define KI4800_ATTR_VOLTAGE_LEVEL				IVIDCPWR_ATTR_VOLTAGE_LEVEL				/* ViReal64,  channel-based, volts */
#define KI4800_ATTR_CURRENT_LIMIT				IVIDCPWR_ATTR_CURRENT_LIMIT				/* ViReal64,  channel-based, amps  */
#define KI4800_ATTR_OVP_LIMIT					IVIDCPWR_ATTR_OVP_LIMIT					/* ViReal64,  channel-based, volts */
	
#define KI4800_ATTR_POWER_LIMIT					(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 1L)	/* ViReal64,  channel-based, volts */
#define KI4800_ATTR_OCP_LIMIT					(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 2L)	/* ViReal64,  channel-based, volts */


/*  ViInt32	    */
#define KI4800_ATTR_CURRENT_LIMIT_BEHAVIOR		IVIDCPWR_ATTR_CURRENT_LIMIT_BEHAVIOR	/* ViInt32,   channel-based        */
//#define KI4800_ATTR_OP_TYPE						(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 101L)	/* ViInt32,   channel-based        */
#define KI4800_ATTR_DIGITAL_PORT				(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 102L)	/* ViInt32,   channel-based        */

/*  ViBoolean   */
#define KI4800_ATTR_OUTPUT_ENABLED				IVIDCPWR_ATTR_OUTPUT_ENABLED			/* ViBoolean, channel-based        */

/*  ViString   */
//#define KI4800_ATTR_TRACK_NAME					(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 103L)	/* ViString, channel-based         */


/**************************************************************************** 
 *------------------------ Enumerated Values		-------------------------* 
 ****************************************************************************/

#define KI4800_SPECIFIC_EXT_BASE	(1000L)

#define KI4800_VAL_CURRENT_REGULATE				IVIDCPWR_VAL_CURRENT_REGULATE
#define KI4800_VAL_CURRENT_TRIP					IVIDCPWR_VAL_CURRENT_TRIP

#define KI4800_VAL_MEASURE_CURRENT				IVIDCPWR_VAL_MEASURE_CURRENT
#define KI4800_VAL_MEASURE_VOLTAGE				IVIDCPWR_VAL_MEASURE_VOLTAGE

#define KI4800_VAL_OUTPUT_CONSTANT_VOLTAGE		IVIDCPWR_VAL_OUTPUT_CONSTANT_VOLTAGE
#define KI4800_VAL_OUTPUT_CONSTANT_CURRENT		IVIDCPWR_VAL_OUTPUT_CONSTANT_CURRENT
#define KI4800_VAL_OUTPUT_OVER_VOLTAGE			IVIDCPWR_VAL_OUTPUT_OVER_VOLTAGE
#define KI4800_VAL_OUTPUT_OVER_CURRENT			IVIDCPWR_VAL_OUTPUT_OVER_CURRENT
#define KI4800_VAL_OUTPUT_UNREGULATED			IVIDCPWR_VAL_OUTPUT_UNREGULATED
#define KI4800_VAL_OUTPUT_CONSTANT_POWER		(KI4800_SPECIFIC_EXT_BASE + 1001)


    /* Instrument specific attribute value definitions */

/**************************************************************************** 
 IVI-C inherent functions
 ****************************************************************************/

/*- Init and Close Functions -------------------------------------------*/
ViStatus _VI_FUNC  ki4800_init(
	ViRsrc		resourceName,
	ViBoolean	IDQuery,
	ViBoolean	resetDevice,
	ViSession*	vi
);
ViStatus _VI_FUNC  ki4800_InitWithOptions(
	ViRsrc			resourceName,
	ViBoolean		IDQuery,
	ViBoolean		resetDevice,
	ViConstString	optionString,
	ViSession*		newVi
);
ViStatus _VI_FUNC  ki4800_close(
	ViSession	vi
);   

ViStatus _VI_FUNC  ki4800_Disable(
	ViSession	vi
);

//ViStatus _VI_FUNC  ki4800_GetNativeIUnknownPtr(ViSession vi, IUnknown** pNativeIUnknownPtr);
//ViStatus _VI_FUNC  ki4800_AttachToExistingCOMSession(IUnknown* pExistingIUnknownPtr, ViSession* pVi);

/*- Coercion Info Functions --------------------------------------------*/
ViStatus _VI_FUNC  ki4800_GetNextCoercionRecord(
	ViSession	vi,
	ViInt32		bufferSize,
	ViChar		record[]
);

/*- Interchangeability Checking Functions ------------------------------*/
ViStatus _VI_FUNC ki4800_GetNextInterchangeWarning(
	ViSession	vi, 
	ViInt32		bufferSize,
	ViChar		warnString[]
);
ViStatus _VI_FUNC ki4800_ResetInterchangeCheck(
	ViSession	vi
);
ViStatus _VI_FUNC ki4800_ClearInterchangeWarnings(
	ViSession	vi
);


/*- Locking Functions --------------------------------------------------*/
ViStatus _VI_FUNC  ki4800_LockSession(
	ViSession	vi,
	ViBoolean*	callerHasLock
);
ViStatus _VI_FUNC  ki4800_UnlockSession(
	ViSession 	vi,
	ViBoolean*	callerHasLock
);

    /*- Error Functions ----------------------------------------------------*/
ViStatus _VI_FUNC  ki4800_error_query(
	ViSession	vi,
	ViInt32*	errorCode,
	ViChar		errorMessage[]
);
ViStatus _VI_FUNC  ki4800_error_message(
	ViSession	vi,
	ViStatus	errorCode,
	ViChar		errorMessage[256]
);
ViStatus _VI_FUNC  ki4800_GetError(
	ViSession	vi,
	ViStatus*	errorCode,
	ViInt32		bufferSize,
	ViChar		description[]
);
ViStatus _VI_FUNC  ki4800_ClearError(
	ViSession	vi
);

/*- Deprecated Error Information functions (still required by LabVIEW 6.0 when converted) -*/
ViStatus _VI_FUNC  ki4800_GetErrorInfo (ViSession vi, ViStatus *primaryError, 
                                          ViStatus *secondaryError, 
                                          ViChar errorElaboration[256]);
ViStatus _VI_FUNC  ki4800_ClearErrorInfo (ViSession vi);


/*- Utility Functions --------------------------------------------------*/
ViStatus _VI_FUNC  ki4800_reset(
	ViSession	vi
);
ViStatus _VI_FUNC  ki4800_ResetWithDefaults(
	ViSession	vi
);
ViStatus _VI_FUNC  ki4800_InvalidateAllAttributes(
	ViSession	vi
);
ViStatus _VI_FUNC  ki4800_self_test(
	ViSession	vi,
	ViInt16*	TestResult,
	ViChar		TestMessage[]
);
ViStatus _VI_FUNC  ki4800_revision_query(
	ViSession	vi,
	ViChar		DriverRev[],
	ViChar		InstrRev[]
);

/*- Set, Get, Attribute Functions ----------------------------*/
ViStatus _VI_FUNC  ki4800_GetAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 *value);
ViStatus _VI_FUNC  ki4800_GetAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 *value);
ViStatus _VI_FUNC  ki4800_GetAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 bufSize, ViChar value[]); 
ViStatus _VI_FUNC  ki4800_GetAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession *value);
ViStatus _VI_FUNC  ki4800_GetAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean *value);

ViStatus _VI_FUNC  ki4800_SetAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 value);
ViStatus _VI_FUNC  ki4800_SetAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 value);
ViStatus _VI_FUNC  ki4800_SetAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViConstString value); 
ViStatus _VI_FUNC  ki4800_SetAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession value);
ViStatus _VI_FUNC  ki4800_SetAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean value);

/**************************************************************************** 
 IviDCPwr functions
 ****************************************************************************/
ViStatus _VI_FUNC ki4800_GetChannelName(
	ViSession	vi,
	ViInt32		index,
	ViInt32		bufferSize,
	ViChar		name[]
);

ViStatus _VI_FUNC  ki4800_ConfigureOutputEnabled(
	ViSession vi,
	ViConstString channelName,
	ViBoolean enabled
);

ViStatus _VI_FUNC  ki4800_ConfigureVoltageLevel(
	ViSession		vi,
	ViConstString	channelName,
	ViReal64		value
);

ViStatus _VI_FUNC  ki4800_ConfigureCurrentLimit(
	ViSession		vi,
	ViConstString	channelName,
	ViInt32			behaviour,
	ViReal64		value
);

ViStatus _VI_FUNC  ki4800_ConfigureOVP(
	ViSession		vi,
	ViConstString	channelName,
	ViBoolean		enabled,
	ViReal64		value
);


ViStatus _VI_FUNC  ki4800_ConfigureTriggeredVoltageLevel(
	ViSession		vi,
	ViConstString	channelName,
	ViReal64		value
);

ViStatus _VI_FUNC  ki4800_ConfigureTriggeredCurrentLimit(
	ViSession		vi,
	ViConstString	channelName,
	ViReal64		value
);

ViStatus _VI_FUNC  ki4800_ConfigureTriggerSource(
	ViSession		vi,
	ViConstString	channelName,
	ViInt32			source
);

ViStatus _VI_FUNC  ki4800_QueryOutputState(
	ViSession		vi,
	ViConstString	channelName,
	ViInt32			outputState,
	ViBoolean*		inState
);

ViStatus _VI_FUNC  ki4800_ResetOutputProtection(
	ViSession		vi,
	ViConstString	channelName
);

    /*- Triggering ---------------------------------------------------------*/


ViStatus _VI_FUNC  ki4800_Abort(
	ViSession	vi
);
ViStatus _VI_FUNC  ki4800_Initiate(
	ViSession	vi
);

ViStatus _VI_FUNC  ki4800_SendSoftwareTrigger(
	ViSession	vi
);

ViStatus _VI_FUNC  ki4800_Measure(
	ViSession		vi,
	ViConstString	channelName,
	ViInt32			measurementType,
	ViReal64*		measurement
);


ViStatus _VI_FUNC ki4800_QueryMaxCurrentLimit (ViSession vi,
                                                 ViConstString channelName,
                                                 ViReal64 voltageLevel,
                                                 ViReal64* maxCurrentLimit);

ViStatus _VI_FUNC ki4800_QueryMaxVoltageLevel (ViSession vi,
                                                 ViConstString channelName,
                                                 ViReal64 currentLimit,
                                                 ViReal64* maxVoltageLevel);

/**************************************************************************** 
 instrument specific functions
 ****************************************************************************/

    /*- Channel Info Functions ---------------------------------------------*/


    /*- Basic Functions ------------------------------*/


    /*- Action/Status ---------------------------------------------------------*/

ViStatus _VI_FUNC  ki4800_ConfigurePowerLimit(
	ViSession		vi,
	ViConstString	channelName,
	ViReal64		value
);

ViStatus _VI_FUNC  ki4800_ConfigureOVPLimit(
	ViSession		vi,
	ViConstString	channelName,
	ViReal64		value
);

ViStatus _VI_FUNC  ki4800_ConfigureOCPLimit(
	ViSession		vi,
	ViConstString	channelName,
	ViReal64		value
);

ViStatus _VI_FUNC  ki4800_QueryVoltageLevelMinMax(
	ViSession		vi,
	ViConstString	channelName,
	ViReal64*		min_Value,
	ViReal64*		max_Value
);

ViStatus _VI_FUNC  ki4800_QueryCurrentLimitMinMax(
	ViSession		vi,
	ViConstString	channelName,
	ViReal64*		min_Value,
	ViReal64*		max_Value
);

ViStatus _VI_FUNC  ki4800_QueryPowerLimitMinMax(
	ViSession		vi,
	ViConstString	channelName,
	ViReal64*		min_Value,
	ViReal64*		max_Value
);

ViStatus _VI_FUNC  ki4800_QueryOPType(
	ViSession		vi,
	ViConstString	channelName,
	ViInt32*		opType
);


ViStatus _VI_FUNC  ki4800_QueryTrackName(
	ViSession		vi,
	ViConstString	channelName,
	ViInt32			bufferSize,
	ViChar			name[]
);


ViStatus _VI_FUNC  ki4800_OutputDigitalPort(
	ViSession		vi,
	ViConstString	channelName,
	ViInt32			valueOutput
);


ViStatus _VI_FUNC  ki4800_InputDigitalPort(
	ViSession		vi,
	ViConstString	channelName,
	ViInt32*		valueInput
);


    /*- Measure ---------------------------------------------------------*/


ViStatus _VI_FUNC  ki4800_WriteInstrData(
	ViSession		vi,
	ViConstString	writeBuffer
); 
ViStatus _VI_FUNC  ki4800_ReadInstrData(
	ViSession		vi,
	ViInt32			numBytes,
	ViChar			rdBuf[],
	ViInt32*		bytesRead
);



    /*********************************************************
        Functions reserved for class driver use only.
        End-users should not call these functions.  
     *********************************************************/
/*
ViStatus _VI_FUNC  ki4800_IviInit (ViRsrc resourceName, ViBoolean IDQuery, 
                                     ViBoolean reset, ViSession vi);
ViStatus _VI_FUNC  ki4800_IviClose (ViSession vi);   
*/



/****************************************************************************
 *------------------------ Error And Completion Codes ----------------------*
 ****************************************************************************/
#define KI4800_ERROR_TRIGGER_NOT_SOFTWARE			(IVI_CROSS_CLASS_ERROR_BASE + 1L)

#define KI4800_ERROR_NODECHECK_LOG_NOT_FOUND		(IVI_SPECIFIC_ERROR_BASE + 1)	//Node Check LOG data not found
#define KI4800_ERROR_CHANNEL_NO_RESPONSE			(IVI_SPECIFIC_ERROR_BASE + 2)	//The channel has no response

/****************************************************************************
 *--------- Obsolete Inherent Instrument Attributes and functions ----------*
 ****************************************************************************/
    /*- These attributes and functions have been deprecated and may not
        be supported in future versions of this driver. -*/
 
         /* Driver Information  */
#define KI4800_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION  /* ViInt32,  read-only  */
#define KI4800_ATTR_SPECIFIC_DRIVER_MINOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_MINOR_VERSION  /* ViInt32,  read-only  */

        /* Error Info */
#define KI4800_ATTR_PRIMARY_ERROR                 IVI_ATTR_PRIMARY_ERROR                  /* ViInt32   */
#define KI4800_ATTR_SECONDARY_ERROR               IVI_ATTR_SECONDARY_ERROR                /* ViInt32   */
#define KI4800_ATTR_ERROR_ELABORATION             IVI_ATTR_ERROR_ELABORATION              /* ViString  */

        /* Advanced Session Information */
#define KI4800_ATTR_IO_SESSION_TYPE               IVI_ATTR_IO_SESSION_TYPE                /* ViString, read-only  */
#define KI4800_ATTR_IO_SESSION                    IVI_ATTR_IO_SESSION                     /* ViSession, read-only */

        /*- Deprecated Attribute Identifiers for Renamed Attributes -*/
#define KI4800_ATTR_NUM_CHANNELS                  KI4800_ATTR_CHANNEL_COUNT             /* ViInt32,  read-only  */
#define KI4800_ATTR_QUERY_INSTR_STATUS            KI4800_ATTR_QUERY_INSTRUMENT_STATUS   /* ViBoolean */
#define KI4800_ATTR_RESOURCE_DESCRIPTOR           KI4800_ATTR_IO_RESOURCE_DESCRIPTOR    /* ViString, read-only  */


/**************************************************************************** 
 *---------------------------- End Include File ----------------------------* 
 ****************************************************************************/
#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

#endif //__KI4800_HEADER
