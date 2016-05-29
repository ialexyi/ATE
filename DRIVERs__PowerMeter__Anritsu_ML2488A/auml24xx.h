/****************************************************************************
 *                       Anritsu ML248x / ML249x IVI Driver                           
 *                                                                          
 * Title:    auml24xx.h                                        
 * Purpose:  Anritsu ML248x / ML249x IVI Driver                                       
 *           instrument driver declarations.                                
 *                                                                          
 ****************************************************************************/

#ifndef __AUML24XX_HEADER
#define __AUML24XX_HEADER

#include <ivi.h>
#include <ivipwrmeter.h>

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

/****************************************************************************
 *----------------- Instrument Driver Revision Information -----------------*
 ****************************************************************************/
#define AUML24XX_MAJOR_VERSION                1     /* Instrument driver major version   */
#define AUML24XX_MINOR_VERSION                1     /* Instrument driver minor version   */
                                                                
#define AUML24XX_CLASS_SPEC_MAJOR_VERSION     1     /* Class specification major version */
#define AUML24XX_CLASS_SPEC_MINOR_VERSION     0     /* Class specification minor version */


#define AUML24XX_SUPPORTED_INSTRUMENT_MODELS  "ML2487A,ML2488A,ML2495A,ML2496A,ML2487B,ML2488B,"
#define AUML24XX_DRIVER_VENDOR                "National Instruments"
#define AUML24XX_DRIVER_DESCRIPTION           "Anritsu ML248x and ML249x Power Meter"
                    



/**************************************************************************** 
 *------------------------------ Useful Macros -----------------------------* 
 ****************************************************************************/

/**************************************************************************** 
 *---------------------------- Attribute Defines ---------------------------* 
 ****************************************************************************/

    /*- IVI Inherent Instrument Attributes ---------------------------------*/    

        /* User Options */
#define AUML24XX_ATTR_RANGE_CHECK                  					 IVI_ATTR_RANGE_CHECK                    /* ViBoolean */
#define AUML24XX_ATTR_QUERY_INSTRUMENT_STATUS      					 IVI_ATTR_QUERY_INSTRUMENT_STATUS        /* ViBoolean */
#define AUML24XX_ATTR_CACHE                        					 IVI_ATTR_CACHE                          /* ViBoolean */
#define AUML24XX_ATTR_SIMULATE                     					 IVI_ATTR_SIMULATE                       /* ViBoolean */
#define AUML24XX_ATTR_RECORD_COERCIONS             					 IVI_ATTR_RECORD_COERCIONS               /* ViBoolean */
#define AUML24XX_ATTR_INTERCHANGE_CHECK            					 IVI_ATTR_INTERCHANGE_CHECK              /* ViBoolean */
        
        /* Driver Information  */
#define AUML24XX_ATTR_SPECIFIC_DRIVER_PREFIX       					 IVI_ATTR_SPECIFIC_DRIVER_PREFIX         /* ViString, read-only  */
#define AUML24XX_ATTR_SUPPORTED_INSTRUMENT_MODELS  					 IVI_ATTR_SUPPORTED_INSTRUMENT_MODELS    /* ViString, read-only  */
#define AUML24XX_ATTR_GROUP_CAPABILITIES           					 IVI_ATTR_GROUP_CAPABILITIES             /* ViString, read-only  */
#define AUML24XX_ATTR_INSTRUMENT_MANUFACTURER      					 IVI_ATTR_INSTRUMENT_MANUFACTURER        /* ViString, read-only  */
#define AUML24XX_ATTR_INSTRUMENT_MODEL             					 IVI_ATTR_INSTRUMENT_MODEL               /* ViString, read-only  */
#define AUML24XX_ATTR_INSTRUMENT_FIRMWARE_REVISION 					 IVI_ATTR_INSTRUMENT_FIRMWARE_REVISION   /* ViString, read-only  */
#define AUML24XX_ATTR_SPECIFIC_DRIVER_REVISION     					 IVI_ATTR_SPECIFIC_DRIVER_REVISION       /* ViString, read-only  */
#define AUML24XX_ATTR_SPECIFIC_DRIVER_VENDOR       					 IVI_ATTR_SPECIFIC_DRIVER_VENDOR         /* ViString, read-only  */
#define AUML24XX_ATTR_SPECIFIC_DRIVER_DESCRIPTION  					 IVI_ATTR_SPECIFIC_DRIVER_DESCRIPTION    /* ViString, read-only  */
#define AUML24XX_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION		 IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION /* ViInt32, read-only */
#define AUML24XX_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION		 IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION /* ViInt32, read-only */

        /* Advanced Session Information */
#define AUML24XX_ATTR_LOGICAL_NAME                 					 IVI_ATTR_LOGICAL_NAME                   /* ViString, read-only  */
#define AUML24XX_ATTR_IO_RESOURCE_DESCRIPTOR       					 IVI_ATTR_IO_RESOURCE_DESCRIPTOR         /* ViString, read-only  */
#define AUML24XX_ATTR_DRIVER_SETUP                 					 IVI_ATTR_DRIVER_SETUP                   /* ViString, read-only  */        
    /*- Instrument-Specific Attributes -------------------------------------*/
    /*- IviPwrMeter Fundamental Attributes -*/
#define AUML24XX_ATTR_CHANNEL_COUNT                   				 IVI_ATTR_CHANNEL_COUNT                          /* ViInt32,  read-only  */
#define AUML24XX_ATTR_UNITS                           				 IVIPWRMETER_ATTR_UNITS                          /* ViInt32                         */
#define AUML24XX_ATTR_RANGE_AUTO_ENABLED              				 IVIPWRMETER_ATTR_RANGE_AUTO_ENABLED             /* ViBoolean, multi-channel        */
#define AUML24XX_ATTR_AVERAGING_AUTO_ENABLED          				 IVIPWRMETER_ATTR_AVERAGING_AUTO_ENABLED         /* ViBoolean, multi-channel        */
#define AUML24XX_ATTR_CORRECTION_FREQUENCY            				 IVIPWRMETER_ATTR_CORRECTION_FREQUENCY           /* ViReal64, multi-channel, hertz  */
#define AUML24XX_ATTR_OFFSET                          				 IVIPWRMETER_ATTR_OFFSET                         /* ViReal64, multi-channel, dB     */


    /*- IviPwrMeter TriggerSource Attribute -*/
#define AUML24XX_ATTR_TRIGGER_SOURCE                  				 IVIPWRMETER_ATTR_TRIGGER_SOURCE                 /* ViInt32                         */

    /*- IviPwrMeter InternalTrigger Attributes -*/
#define AUML24XX_ATTR_INTERNAL_TRIGGER_EVENT_SOURCE   				 IVIPWRMETER_ATTR_INTERNAL_TRIGGER_EVENT_SOURCE  /* ViString                        */
#define AUML24XX_ATTR_INTERNAL_TRIGGER_LEVEL          				 IVIPWRMETER_ATTR_INTERNAL_TRIGGER_LEVEL         /* ViReal64                        */
#define AUML24XX_ATTR_INTERNAL_TRIGGER_SLOPE          				 IVIPWRMETER_ATTR_INTERNAL_TRIGGER_SLOPE         /* ViInt32                         */

    /*- IviPwrMeter AveragingCount Attribute -*/
#define AUML24XX_ATTR_AVERAGING_COUNT                 				 IVIPWRMETER_ATTR_AVERAGING_COUNT                /* ViInt32, multi-channel          */

    /*- IviPwrMeter DutyCycleCorrection Attributes -*/
#define AUML24XX_ATTR_DUTY_CYCLE_CORRECTION_ENABLED   				 IVIPWRMETER_ATTR_DUTY_CYCLE_CORRECTION_ENABLED  /* ViBoolean, multi-channel        */
#define AUML24XX_ATTR_DUTY_CYCLE_CORRECTION           				 IVIPWRMETER_ATTR_DUTY_CYCLE_CORRECTION          /* ViReal64, multi-channel         */

    /*- IviPwrMeter ReferenceOscillator Attributes -*/
#define AUML24XX_ATTR_REF_OSCILLATOR_ENABLED          				 IVIPWRMETER_ATTR_REF_OSCILLATOR_ENABLED         /* ViBoolean                       */
#define AUML24XX_ATTR_REF_OSCILLATOR_FREQUENCY        				 IVIPWRMETER_ATTR_REF_OSCILLATOR_FREQUENCY       /* ViReal64, hertz                 */
#define AUML24XX_ATTR_REF_OSCILLATOR_LEVEL            				 IVIPWRMETER_ATTR_REF_OSCILLATOR_LEVEL           /* ViReal64, dBm                   */

/****************************************************************************
 *---------- IviPwrMeter Class Function Parameter Value Defines ------------*
 ****************************************************************************/

    /*- Defined values for function: Read, Fetch   -*/
    /*-                   parameter: maxTime       -*/
#define AUML24XX_VAL_MAX_TIME_INFINITE                				 IVI_VAL_MAX_TIME_INFINITE                       /* Immediate Timeout               */
#define AUML24XX_VAL_MAX_TIME_IMMEDIATE               				 IVI_VAL_MAX_TIME_IMMEDIATE                      /* Infinite Timeout                */

    /*- Defined values for function: ConfigureMeasurement  -*/
    /*-                   parameter: Operator              -*/
#define AUML24XX_VAL_NONE                             				 IVIPWRMETER_VAL_NONE                            /* Read Channel                    */
#define AUML24XX_VAL_DIFFERENCE                       				 IVIPWRMETER_VAL_DIFFERENCE                      /* Difference                      */
#define AUML24XX_VAL_QUOTIENT                         				 IVIPWRMETER_VAL_QUOTIENT                        /* Quotient                        */

    /*- Defined values for function: IsMeasurementComplete  -*/
    /*-                   parameter: status                 -*/
#define AUML24XX_VAL_MEAS_COMPLETE                    				 IVIPWRMETER_VAL_MEAS_COMPLETE                   /* Measurement Complete            */
#define AUML24XX_VAL_MEAS_IN_PROGRESS                 				 IVIPWRMETER_VAL_MEAS_IN_PROGRESS                /* Measurement In Progress         */
#define AUML24XX_VAL_MEAS_STATUS_UNKNOWN              				 IVIPWRMETER_VAL_MEAS_STATUS_UNKNOWN             /* Measurement Status Unknow       */

 /*- Defined values for function: IsCalibrationComplete     -*/
    /*-                   parameter: status                 -*/
#define AUML24XX_VAL_CALIBRATION_COMPLETE            				 IVIPWRMETER_VAL_CALIBRATION_COMPLETE            /* Measurement Complete            */
#define AUML24XX_VAL_CALIBRATION_IN_PROGRESS         				 IVIPWRMETER_VAL_CALIBRATION_IN_PROGRESS         /* Measurement In Progress         */
#define AUML24XX_VAL_CALIBRATION_STATUS_UNKNOWN      				 IVIPWRMETER_VAL_CALIBRATION_STATUS_UNKNOWN      /* Measurement Status Unknow       */

    /*- Defined values for function: QueryResultRangeType  -*/
    /*-                   parameter: rangeType             -*/
#define AUML24XX_VAL_IN_RANGE                         				 IVIPWRMETER_VAL_IN_RANGE                        /* Within the current range limits */
#define AUML24XX_VAL_UNDER_RANGE                      				 IVIPWRMETER_VAL_UNDER_RANGE                     /* Below the current range limits  */
#define AUML24XX_VAL_OVER_RANGE                       				 IVIPWRMETER_VAL_OVER_RANGE                      /* Above the current range limits  */

    /*- Defined values for function: IsZeroComplete         -*/
    /*-                   parameter: status                 -*/
#define AUML24XX_VAL_ZERO_COMPLETE                    				 IVIPWRMETER_VAL_ZERO_COMPLETE                   /* Zero Correction Complete        */
#define AUML24XX_VAL_ZERO_IN_PROGRESS                 				 IVIPWRMETER_VAL_ZERO_IN_PROGRESS                /* Zero Correction In Progress     */
#define AUML24XX_VAL_ZERO_STATUS_UNKNOWN              				 IVIPWRMETER_VAL_ZERO_STATUS_UNKNOWN             /* Zero Correction Status Unknow   */

	/*- auml24xx-Specific Attributes -------------------------------------*/  
#define AUML24XX_ATTR_ID_QUERY_RESPONSE                                        (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 1L)     /* ViString (Read Only) */
	
	/*- Basic Operation Attributes -----------------------*/ 
#define AUML24XX_ATTR_CHANNEL_COUNTS                                           (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 101L)	/* ViInt32   */ 
#define AUML24XX_ATTR_AVERAGING_ENABLED                                        (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 102L)	/* ViBoolean  */ 
#define AUML24XX_ATTR_ACTIVE_CHANNEL                                           (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 103L)	/* ViInt32  */ 
#define AUML24XX_ATTR_DUAL_CHANNEL_MODE_ENABLED                                (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 104L)	/* ViBoolean  */ 
#define AUML24XX_ATTR_SAMPLE_RATE                                              (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 105L)	/* ViInt32  */ 
#define AUML24XX_ATTR_MIN_MAX_TRACKING_ENABLED                                 (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 106L)	/* ViInt32  */ 
#define AUML24XX_ATTR_CW_AVERAGING_MODE                                        (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 107L)	    /* ViInt32  */ 
#define AUML24XX_ATTR_SETTLE_PERCENT                                           (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 108L)		/* ViReal64  */ 
#define AUML24XX_ATTR_PMOD_MEASUREMENT_DISPLAY                                 (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 109L) 	/* ViInt32  */ 
#define AUML24XX_ATTR_PMOD_MEASUREMENT_TYPE                                    (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 110L)		/* ViInt32  */ 
#define AUML24XX_ATTR_CHANNEL_MEASUREMENT_MODE                                 (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 111L)		/* ViInt32  */ 
#define AUML24XX_ATTR_CHANNEL_INPUT_CONFIG                                     (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 112L)		/* ViInt32  */ 
#define AUML24XX_ATTR_RELATIVE_MODE                                            (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 113L)		/* ViInt32  */ 

	/*- Trigger Attributes -----------------------*/ 
#define AUML24XX_ATTR_RRS_TRACE_STATE                                          (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 201L)	/* ViBoolean */ 
#define AUML24XX_ATTR_TRIGGER_EXTERNAL_EDGE                                    (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 202L)	/* ViInt32  */ 
#define AUML24XX_ATTR_TRIGGER_LINKING_ENABLED                                  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 203L)	/* ViBoolean */ 
#define AUML24XX_ATTR_TRIGGER_HOLDOFF_TIME                                     (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 204L)	/* ViReal64  */ 
#define AUML24XX_ATTR_TRIGGER_HOLDOFF_ENABLED                                  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 205L)	/* ViBoolean */ 
#define AUML24XX_ATTR_TRIGGER_FRAME_ARMING_TIME_DURATION                       (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 206L)	/* ViReal64  */ 
#define AUML24XX_ATTR_TRIGGER_FRAME_ARMING_LEVEL                               (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 207L)	/* ViReal64  */ 
#define AUML24XX_ATTR_TRIGGER_DELAY_TIME                                       (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 208L)	/* ViReal64  */ 
#define AUML24XX_ATTR_CAPTURE_TIME                                             (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 209L)	/* ViInt32  */ 
#define AUML24XX_ATTR_TRIGGER_BANDWIDTH                                        (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 210L)	/* ViBoolean */ 
#define AUML24XX_ATTR_TRIGGER_AUTO_ENABLED                                     (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 211L)	/* ViInt32  */ 
#define AUML24XX_ATTR_TRIGGER_ARMING_MODE                                      (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 212L)    /* ViInt32  */ 

	/*- Gate Attributes -----------------------*/ 
#define AUML24XX_ATTR_GATE_PATTERN_START_TIME                                  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 301L) 	/* ViReal64  */ 
#define AUML24XX_ATTR_GATE_PATTERN_STOP_TIME                                   (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 302L) 	/* ViReal64  */ 
#define AUML24XX_ATTR_GATE_PATTERN_ENABLED                                     (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 303L) 	/* ViBoolean  */ 
#define AUML24XX_ATTR_FENCE_START_TIME                                         (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 304L)	/* ViReal64  */ 
#define AUML24XX_ATTR_FENCE_STOP_TIME                                          (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 305L) 	/* ViReal64  */ 
#define AUML24XX_ATTR_FENCE_ENABLED                                            (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 306L) 	/* ViBoolean */ 
#define AUML24XX_ATTR_GATE_PATTERN_CONFIGURATION_NUMBER                        (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 307L) 	/* ViInt32  */ 
#define AUML24XX_ATTR_GATE_PATTERN_1_REPEAT_OFFSET                             (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 308L)	/* ViReal64  */ 
#define AUML24XX_ATTR_GATE_PATTERN_1_REPEAT_ENABLED                            (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 309L)	/* ViBoolean */ 
#define AUML24XX_ATTR_GATE_PATTERN_1_REPEAT_NUMBER                             (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 400L)	/* ViInt32  */ 

	/*- Marker Attributes -----------------------*/ 
#define AUML24XX_ATTR_MARKER_ENABLED                                           (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 401L) 	/* ViBoolean  */ 
#define AUML24XX_ATTR_MARKER_CONFIGURATION_NUMBER                              (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 402L) 	/* ViInt32  */ 
#define AUML24XX_ATTR_MARKER_DELTA_ENABLED                                     (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 403L) 	/* ViBoolean */ 
#define AUML24XX_ATTR_MARKER_DELTA_LINK_ENABLED                                (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 404L) 	/* ViBoolean  */ 
#define AUML24XX_ATTR_MARKER_DELTA_MEASUREMENT_TYPE                            (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 405L) 	/* ViInt32  */ 
#define AUML24XX_ATTR_MARKER_DELTA_POSITION                                    (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 406L) 	/* ViReal64  */ 
#define AUML24XX_ATTR_MARKER_POSITION                                          (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 407L) 	/* ViReal64  */ 
#define AUML24XX_ATTR_MARKER_SEARCH_LOWER_TARGET                               (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 408L) 	/* ViReal64  */ 
#define AUML24XX_ATTR_MARKER_SEARCH_START_VALUE_SOURCE                         (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 409L) 	/* ViInt32  */ 
#define AUML24XX_ATTR_MARKER_SEARCH_UPPER_TARGET                               (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 410L) 	/* ViReal64  */ 

   /*- Limit Attributes -----------------------*/ 
#define AUML24XX_ATTR_LIMIT_COMPLEX_TIME_OFFSET                                (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 501L)   	/* ViReal64  */ 
#define AUML24XX_ATTR_LIMIT_COMPLEX_SPECIFICATION_ACTIVE_NUMBER                (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 502L) 	/* ViInt32  */ 
#define AUML24XX_ATTR_LIMIT_COMPLEX_SPECIFICATION_TYPE                         (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 503L)   	/* ViInt32  */ 
#define AUML24XX_ATTR_LIMIT_COMPLEX_TIME_REPLICATION_OFFSET                    (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 504L)   	/* ViReal64  */ 
#define AUML24XX_ATTR_LIMIT_COMPLEX_POWER_REPLICATION_OFFSET                   (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 505L)   	/* ViReal64  */ 
#define AUML24XX_ATTR_LIMIT_COMPLEX_RPEAT_ENABLED                              (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 506L)  	/* ViBoolean  */ 
#define AUML24XX_ATTR_LIMIT_COMPLEX_RPEAT_COUNT                                (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 507L)  	/* ViInt32  */ 
#define AUML24XX_ATTR_LIMIT_COMPLEX_POWER_OFFSET                               (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 508L)  	/* ViReal64  */ 
#define AUML24XX_ATTR_LIMIT_CHECKING_TYPE                                      (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 509L)  	/* ViInt32  */ 
#define AUML24XX_ATTR_LIMIT_SIMPLE_UPPER_LINE                                  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 510L) 	/* ViReal64  */ 
#define AUML24XX_ATTR_LIMIT_CHECKING_ENABLED                                   (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 511L)  	/* ViBoolean  */ 
#define AUML24XX_ATTR_LIMIT_LINE_TEST_TYPE                                     (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 512L)  	/* ViBoolean  */ 
#define AUML24XX_ATTR_LIMIT_SIMPLE_LOWER_LINE                                  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 513L) 	/* ViReal64  */ 
#define AUML24XX_ATTR_LIMIT_FAIL_INDICATOR_HOLD_ENABLED                        (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 514L) 	/* ViBoolean  */ 


   /*- Processing Attributes -----------------------*/ 
#define AUML24XX_ATTR_POST_PROCESSING_ACQUISITION_ENABLED                      (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 601L)  	/* ViBoolean  */ 
#define AUML24XX_ATTR_POST_PROCESSING_FUNCTION_MODULE                          (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 602L)  	/* ViInt32  */ 
#define AUML24XX_ATTR_STAT_POST_PROCESSING_SOURCE_SELECTION                    (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 603L)  	/* ViInt32  */ 
#define AUML24XX_ATTR_STAT_POST_PROCESSING_MARKER_ENABLED                      (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 604L)  	/* ViBoolean  */ 
#define AUML24XX_ATTR_STAT_POST_PROCESSING_MARKER_POSITION                     (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 605L)  	/* ViReal64  */ 
#define AUML24XX_ATTR_STAT_POST_PROCESSING_FUNCTION_TYPE                       (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 606L)  	/* ViInt32  */ 
#define AUML24XX_ATTR_PAE_SOURCE_SELECTION                                     (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 607L)  	/* ViInt32  */ 
#define AUML24XX_ATTR_PAE_INPUT_CONFIGURATION                                  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 608L)  	/* ViInt32  */ 
#define AUML24XX_ATTR_PAE_BIAS_CURRENT_CONVERSION_FACTOR                       (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 609L)   	/* ViReal64  */ 
#define AUML24XX_ATTR_PAE_BIAS_VOLTAGE                                         (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 610L)  	/* ViReal64  */ 
#define AUML24XX_ATTR_PAE_BIAS_CURRENT_SOURCE                                  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 611L)  	/* ViInt32  */ 
#define AUML24XX_ATTR_PAE_BIAS_CURRENT                                         (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 612L) 	/* ViReal64  */ 

   /*- Sensor Attributes -----------------------*/ 
#define AUML24XX_ATTR_ACTIVE_CALIBRATION_FACTOR_NUMBER                         (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 701L)	/* ViInt32   */ 
#define AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_ADJUST                         (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 702L)	/* ViReal64  */ 
#define AUML24XX_ATTR_SENSOR_CALIBRATION_MANUAL_FACTOR                         (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 703L)	/* ViReal64  */ 
#define AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_SOURCE                         (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 704L)	/* ViInt32  */ 
#define AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_UNITS                          (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 705L)	/* ViInt32  */ 
#define AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_STOP_FREQUENCY                 (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 706L)    /* ViReal64  */ 
#define AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_STOP_VOLTAGE                   (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 707L)	/* ViReal64  */ 
#define AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_START_FREQUENCY                (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 708L)	/* ViReal64  */ 
#define AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_START_VOLTAGE                  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 709L)	/* ViReal64  */ 
#define AUML24XX_ATTR_SENSOR_OFFSET_TABLE_ACTIVE_NUMBER                        (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 710L)  	/* ViInt32  */ 
#define AUML24XX_ATTR_SENSOR_OFFSET_TYPE                                       (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 711L)  	/* ViInt32  */ 
#define AUML24XX_ATTR_SENSOR_OFFSET                                            (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 712L)   /* ViReal64  */ 
#define AUML24XX_ATTR_SENSOR_CORRECTION_FREQUENCY                              (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 713L)   /* ViReal64  */ 
#define AUML24XX_ATTR_SENSOR_RANGE_HOLD                                        (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 714L)  	/* ViInt32  */ 
#define AUML24XX_ATTR_SENSOR_UNIVERSAL_OPERATION_MODE                          (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 715L)  	/* ViInt32  */ 
#define AUML24XX_ATTR_SENSOR_FILTER_ENABLED                                    (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 716L)  	/* ViBoolean  */ 

   /*- System Attributes -----------------------*/ 
#define AUML24XX_ATTR_GPIB_RESPONSE_BUFFERING_ENABLED                          (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 801L)  	/* ViBoolean  */ 
#define AUML24XX_ATTR_DISPlAY_MEASUREMENT_POINTS                               (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 802L)    /* ViBoolean  */ 
#define AUML24XX_ATTR_FAST_MODE_ENABLED                                        (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 803L)   	/* ViBoolean  */ 
#define AUML24XX_ATTR_BNC_PASS_VOLTAGE_LEVEL                                   (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 804L)  	/* ViInt32  */ 
#define AUML24XX_ATTR_BNC_ANALOG_OUTPUT_START_VOLTAGE_SCALE                    (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 805L)  	/* ViReal64  */ 
#define AUML24XX_ATTR_BNC_ANALOG_OUTPUT_STOP_VOLTAGE_SCALE                     (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 806L) 	/* ViReal64  */ 
#define AUML24XX_ATTR_BNC_OUTPUT_SOURCE_CHANNEL                                (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 807L)  	/* ViInt32  */ 
#define AUML24XX_ATTR_BNC_DB_ANALOG_OUTPUT_START                               (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 808L)   	/* ViReal64  */ 
#define AUML24XX_ATTR_BNC_DBM_ANALOG_OUTPUT_START                              (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 809L)	/* ViReal64  */ 
#define AUML24XX_ATTR_BNC_DBW_ANALOG_OUTPUT_START                              (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 810L) 	/* ViReal64  */ 
#define AUML24XX_ATTR_BNC_DBUV_ANALOG_OUTPUT_START                             (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 811L)  	/* ViReal64  */ 
#define AUML24XX_ATTR_BNC_DBMV_ANALOG_OUTPUT_START                             (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 812L) 	/* ViReal64  */ 
#define AUML24XX_ATTR_BNC_WATTS_ANALOG_OUTPUT_STOP                             (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 813L)	/* ViReal64  */ 
#define AUML24XX_ATTR_BNC_DB_ANALOG_OUTPUT_STOP                                (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 814L)	/* ViReal64  */ 
#define AUML24XX_ATTR_BNC_DBM_ANALOG_OUTPUT_STOP                               (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 815L)  	/* ViReal64  */ 
#define AUML24XX_ATTR_BNC_DBUV_ANALOG_OUTPUT_STOP                              (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 816L)  	/* ViReal64  */ 
#define AUML24XX_ATTR_BNC_DBMV_ANALOG_OUTPUT_STOP                              (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 817L) 	/* ViReal64  */ 
#define AUML24XX_ATTR_BNC_WATTS_ANALOG_OUTPUT_START                            (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 818L) 	/* ViReal64  */ 
#define AUML24XX_ATTR_BNC_DBW_ANALOG_OUTPUT_STOP                               (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 819L)   	/* ViReal64  */ 
#define AUML24XX_ATTR_BNC2_OUTPUT_MODE                                         (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 820L)  	/* ViInt32  */ 
#define AUML24XX_ATTR_BNC1_OUTPUT_MODE                                         (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 821L)  	/* ViInt32  */ 

   /*- Range Calibrator Attributes -----------------------*/ 
#define AUML24XX_ATTR_RANGE_CALIBRATOR_DIAGNOSTICS_TEST                        (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 901L) 	/* ViInt32  */ 



/**************************************************************************** 
 *------------------------ Attribute Value Defines -------------------------* 
 ****************************************************************************/

   /*- Defined values for attribute AUML24XX_ATTR_MEASUREMENT_UNITS -*/

#define AUML24XX_VAL_DBM                               IVIPWRMETER_VAL_DBM
#define AUML24XX_VAL_DBMV                              IVIPWRMETER_VAL_DBMV
#define AUML24XX_VAL_DBUV                              IVIPWRMETER_VAL_DBUV
#define AUML24XX_VAL_WATTS                             IVIPWRMETER_VAL_WATTS
#define AUML24XX_VAL_DBW                               IVIPWRMETER_VAL_UNITS_SPECIFIC_EXT_BASE+1L
#define AUML24XX_VAL_VOLTS                             IVIPWRMETER_VAL_UNITS_SPECIFIC_EXT_BASE+2L

    /*- Defined values for attribute AUML24XX_ATTR_TRIGGER_SOURCE -*/

#define AUML24XX_VAL_IMMEDIATE                         IVIPWRMETER_VAL_IMMEDIATE      
#define AUML24XX_VAL_EXTERNAL                          IVIPWRMETER_VAL_EXTERNAL
#define AUML24XX_VAL_INTERNAL                          IVIPWRMETER_VAL_INTERNAL
#define AUML24XX_VAL_SOFTWARE_TRIG                     IVIPWRMETER_VAL_SOFTWARE_TRIG	
#define AUML24XX_VAL_INTA                              IVIPWRMETER_VAL_TRIGGER_SOURCE_SPECIFIC_EXT_BASE+1
#define AUML24XX_VAL_INTB                              IVIPWRMETER_VAL_TRIGGER_SOURCE_SPECIFIC_EXT_BASE+2
#define AUML24XX_VAL_CONT                              IVIPWRMETER_VAL_TRIGGER_SOURCE_SPECIFIC_EXT_BASE+3

/*- Defined values for attribute AUML24XX_ATTR_INTERNAL_TRIGGER_SLOPE -*/

#define AUML24XX_VAL_NEGATIVE                          IVIPWRMETER_VAL_NEGATIVE
#define AUML24XX_VAL_POSITIVE                          IVIPWRMETER_VAL_POSITIVE

/*- Defined values for attribute AUML24XX_ATTR_SAMPLE_RATE -*/
#define AUML24XX_VAL_AUTO                                           1
#define AUML24XX_VAL_31K25                                          2
#define AUML24XX_VAL_62K5                                           3
#define AUML24XX_VAL_125K                                           4
#define AUML24XX_VAL_250K                                           5
#define AUML24XX_VAL_500K                                           6
#define AUML24XX_VAL_1M                                             7
#define AUML24XX_VAL_2M                                             8
#define AUML24XX_VAL_4M                                             9
#define AUML24XX_VAL_8M                                             10
#define AUML24XX_VAL_16M                                            11
#define AUML24XX_VAL_32M                                            12
#define AUML24XX_VAL_64M                                            13
#define AUML24XX_VAL_31M25                                          14
#define AUML24XX_VAL_62M5                                           15

/*- Defined values for attribute AUML24XX_ATTR_INSTR_MODEL_TYPE -*/
#define AUML24XX_VAL_MODEL_ML2487A                                  1
#define AUML24XX_VAL_MODEL_ML2488A                                  2
#define AUML24XX_VAL_MODEL_ML2495A                                  3
#define AUML24XX_VAL_MODEL_ML2496A                                  4
#define AUML24XX_VAL_MODEL_ML2487B                                  5
#define AUML24XX_VAL_MODEL_ML2488B                                  6

/*- Defined values for attribute AUML24XX_ATTR_DISPlAY_MEASUREMENT_POINTS -*/	
#define AUML24XX_VAL_P200                                           1
#define AUML24XX_VAL_P400                                           2


/*- Defined values for attribute AUML24XX_ATTR_PMOD_MEASUREMENT_TYPE -*/	
#define AUML24XX_VAL_PMOD_MEAS_TYPE_1                               1
#define AUML24XX_VAL_PMOD_MEAS_TYPE_2                               2
#define AUML24XX_VAL_PMOD_MEAS_TYPE_3                               3
#define AUML24XX_VAL_PMOD_MEAS_TYPE_4                               4
#define AUML24XX_VAL_PMOD_MEAS_TYPE_5                               5
#define AUML24XX_VAL_MEAS_MODE_CW                                   1
#define AUML24XX_VAL_MEAS_MODE_PMOD                                 2

/*- Defined values for attribute AUML24XX_ATTR_CHANNEL_INPUT_CONFIG -*/
#define AUML24XX_VAL_CHAN_CONFIG_A                                  1
#define AUML24XX_VAL_CHAN_CONFIG_B                                  2
#define AUML24XX_VAL_CHAN_CONFIG_A_MINUS_B                          3
#define AUML24XX_VAL_CHAN_CONFIG_B_MINUS_A                          4
#define AUML24XX_VAL_CHAN_CONFIG_A_DIVIDED_B                        5
#define AUML24XX_VAL_CHAN_CONFIG_B_DIVIDED_A                        6
#define AUML24XX_VAL_CHAN_CONFIG_V                                  7

/*- Defined values for attribute AUML24XX_ATTR_RELATIVE_MODE -*/  
#define AUML24XX_VAL_RELATIVE_MODE_OFF                              1
#define AUML24XX_VAL_RELATIVE_MODE_REF                              2
#define AUML24XX_VAL_RELATIVE_MODE_OLD_REF                          3

/*- Defined values for attribute AUML24XX_ATTR_TRIGGER_EXTERNAL_EDGE -*/  	
#define AUML24XX_VAL_EXT_EDGE_RISE                                  1
#define AUML24XX_VAL_EXT_EDGE_FALL                                  2

/*- Defined values for attribute AUML24XX_ATTR_TRIGGER_BANDWIDTH -*/  
#define AUML24XX_VAL_TRIGGER_BANDWIDTH_20MHZ                        1
#define AUML24XX_VAL_TRIGGER_BANDWIDTH_2MHZ                         2
#define AUML24XX_VAL_TRIGGER_BANDWIDTH_200KHZ                       3
#define AUML24XX_VAL_TRIGGER_BANDWIDTH_20KHZ                        4


/*- Defined values for attribute AUML24XX_ATTR_REF_OSCILLATOR_FREQUENCY -*/ 
#define AUML24XX_VAL_REF_CALIBRATOR_FREQ_50MHZ                      1
#define AUML24XX_VAL_REF_CALIBRATOR_FREQ_1GHZ                       2

/*- Defined values for attribute AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_UNITS -*/ 
#define AUML24XX_VAL_CAL_FACTOR_UNIT_PCT                            1
#define AUML24XX_VAL_CAL_FACTOR_UNIT_DB                             2

/*- Defined values for attribute AUML24XX_ATTR_SENSOR_CALIBRATION_FACTOR_SOURCE -*/  
#define AUML24XX_VAL_CAL_FACTOR_SOURCE_FREQ                         1
#define AUML24XX_VAL_CAL_FACTOR_SOURCE_MAN                          2
#define AUML24XX_VAL_CAL_FACTOR_SOURCE_VGHZ                         3

/*- Defined values for attribute AUML24XX_ATTR_LIMIT_CHECKING_TYPE -*/  
#define AUML24XX_VAL_LIMIT_CHECKING_COMPLEX                         1
#define AUML24XX_VAL_LIMIT_CHECKING_SIMPLE                          2
	
/*- Defined values for attribute AUML24XX_ATTR_LIMIT_LINE_TEST_TYPE -*/ 
#define AUML24XX_VAL_LIMIT_LINE_TEST_BOTH                           1
#define AUML24XX_VAL_LIMIT_LINE_TEST_LOWER                          3
#define AUML24XX_VAL_LIMIT_LINE_TEST_UPPER                          2
 
/*- Defined values for attribute AUML24XX_ATTR_STAT_POST_PROCESSING_SOURCE_SELECTION -*/   	 
#define AUML24XX_VAL_STAT_POST_PROCESSING_SOURCE_CHANNEL            1
#define AUML24XX_VAL_STAT_POST_PROCESSING_SOURCE_GATE               2
#define AUML24XX_VAL_STAT_POST_PROCESSING_SOURCE_MARKER             3

/*- Defined values for attribute AUML24XX_ATTR_STAT_POST_PROCESSING_FUNCTION_TYPE -*/ 
#define AUML24XX_VAL_PROCESSING_TYPE_PDF                            1
#define AUML24XX_VAL_PROCESSING_TYPE_CDF                            2
#define AUML24XX_VAL_PROCESSING_TYPE_CCDF                           3
	

/*- Defined values for attribute AUML24XX_ATTR_POST_PROCESSING_FUNCTION_MODULE -*/
#define AUML24XX_VAL_POST_PROCESSING_STATS                          1
#define AUML24XX_VAL_POST_PROCESSING_PAE                            2

/*- Defined values for attribute AUML24XX_ATTR_PAE_SOURCE_SELECTION -*/	
#define AUML24XX_VAL_PAE_SOURCE_CHANNEL                             1
#define AUML24XX_VAL_PAE_SOURCE_GATE                                2
#define AUML24XX_VAL_PAE_SOURCE_MARKER                              3

/*- Defined values for attribute AUML24XX_ATTR_PAE_INPUT_CONFIGURATION -*/	
#define AUML24XX_VAL_PAE_INPUT_A_MINUS_B                            1
#define AUML24XX_VAL_PAE_INPUT_B_MINUS_A                            2

/*- Defined values for attribute AUML24XX_ATTR_PAE_BIAS_CURRENT_SOURCE -*/
#define AUML24XX_VAL_PAE_BIAS_CURRENT_SOURCE_FIXED                  1
#define AUML24XX_VAL_PAE_BIAS_CURRENT_SOURCE_PROBE                  2

/*- Defined values for attribute AUML24XX_ATTR_SENSOR_OFFSET_TYPE -*/	
#define AUML24XX_VAL_SENSOR_OFFSET_TYPE_OFF                         1
#define AUML24XX_VAL_SENSOR_OFFSET_TYPE_FIXED                       2
#define AUML24XX_VAL_SENSOR_OFFSET_TYPE_TABLE                       3

/*- Defined values for attribute AUML24XX_ATTR_BNC1_OUTPUT_MODE and AUML24XX_ATTR_BNC2_OUTPUT_MODE -*/	
#define AUML24XX_VAL_OUTPUT_MODE_OFF                                1
#define AUML24XX_VAL_OUTPUT_MODE_AOUT                               2
#define AUML24XX_VAL_OUTPUT_MODE_LVLA1                              4
#define AUML24XX_VAL_OUTPUT_MODE_LVLA2                              5
#define AUML24XX_VAL_OUTPUT_MODE_PSFL                               3
#define AUML24XX_VAL_OUTPUT_MODE_LVLB1                              6
#define AUML24XX_VAL_OUTPUT_MODE_LVLB2                              7
#define AUML24XX_VAL_OUTPUT_MODE_TRIG                               8

/*- Defined values for attribute AUML24XX_ATTR_RANGE_CALIBRATOR_DIAGNOSTICS_TEST -*/	
#define AUML24XX_VAL_RANGE_CALIBRATOR_ZERO                          1
#define AUML24XX_VAL_RANGE_CALIBRATOR_RANGE1_HIGH                   2
#define AUML24XX_VAL_RANGE_CALIBRATOR_RANGE1_LOW                    3
#define AUML24XX_VAL_RANGE_CALIBRATOR_RANGE2_HIGH                   4
#define AUML24XX_VAL_RANGE_CALIBRATOR_RANGE2_LOW                    5
#define AUML24XX_VAL_RANGE_CALIBRATOR_RANGE3_HIGH                   6
#define AUML24XX_VAL_RANGE_CALIBRATOR_RANGE3_LOW                    7
#define AUML24XX_VAL_RANGE_CALIBRATOR_RANGE4_HIGH                   8
#define AUML24XX_VAL_RANGE_CALIBRATOR_RANGE4_LOW                    9
#define AUML24XX_VAL_RANGE_CALIBRATOR_RANGE5_HIGH                   10
#define AUML24XX_VAL_RANGE_CALIBRATOR_RANGE5_LOW                    11

/*- Defined values for attribute AUML24XX_ATTR_SENSOR_UNIVERSAL_OPERATION_MODE -*/	

#define AUML24XX_VAL_UNIVERSAL_SENSOR_TRMS                          1
#define AUML24XX_VAL_UNIVERSAL_SENSOR_FCW                           2

/*- Defined values for attribute AUML24XX_ATTR_BNC_PASS_VOLTAGE_LEVEL -*/	
#define AUML24XX_VAL_BNC_PASS_VOLTAGE_LEVEL_HIGH                    1
#define AUML24XX_VAL_BNC_PASS_VOLTAGE_LEVEL_LOW                     2
	 
/*- Defined values for attribute AUML24XX_ATTR_PAE_SOURCE_SELECTION -*/	
#define AUML24XX_VAL_BNC_OUTPUT_SOURCE_CHANNEL_1                    1
#define AUML24XX_VAL_BNC_OUTPUT_SOURCE_CHANNEL_2                    2

/*- Defined values for attribute AUML24XX_ATTR_TRIGGER_ARMING_MODE -*/	
#define AUML24XX_VAL_TRIGGER_ARMING_MODE_AUTO                       1
#define AUML24XX_VAL_TRIGGER_ARMING_MODE_SINGLE                     2
#define AUML24XX_VAL_TRIGGER_ARMING_MODE_FRAME                      3

/*- Defined values for attribute AUML24XX_ATTR_MARKER_SEARCH_START_VALUE_SOURCE -*/	
#define AUML24XX_VAL_MARKER_SEARCH_SOURCE_MARKER                    1
#define AUML24XX_VAL_MARKER_SEARCH_SOURCE_GATE                      2

/*- Defined values for attribute AUML24XX_ATTR_MARKER_DELTA_MEASUREMENT_TYPE -*/	
#define AUML24XX_VAL_DELTA_MARKER_PDIFF                             1
#define AUML24XX_VAL_DELTA_MARKER_PAVG                              2

/*- Defined values for attribute AUML24XX_ATTR_CW_AVERAGING_MODE -*/	
#define AUML24XX_VAL_AVERAGING_MOV                                  1
#define AUML24XX_VAL_AVERAGING_RPT                                  2
#define AUML24XX_VAL_AVERAGING_AUTO                                 3
#define AUML24XX_VAL_AVERAGING_OFF                                  4

/*- Defined values for attribute AUML24XX_ATTR_LIMIT_COMPLEX_SPECIFICATION_TYPE -*/	
#define AUML24XX_VAL_LIMIT_SPEC_USER                                1
#define AUML24XX_VAL_LIMIT_SPEC_PDEF                                2


/*- Defined values for attribute AUML24XX_ATTR_PAE_SOURCE_SELECTION -*/	
#define AUML24XX_VAL_RANGE_HOLD_AUTO                                10
#define AUML24XX_VAL_RANGE_HOLD_1                                   1
#define AUML24XX_VAL_RANGE_HOLD_2                                   2
#define AUML24XX_VAL_RANGE_HOLD_3                                   3
#define AUML24XX_VAL_RANGE_HOLD_4                                   4
#define AUML24XX_VAL_RANGE_HOLD_5                                   5
#define AUML24XX_VAL_RANGE_HOLD_6                                   6
#define AUML24XX_VAL_RANGE_HOLD_7                                   7
#define AUML24XX_VAL_RANGE_HOLD_8                                   8
#define AUML24XX_VAL_RANGE_HOLD_9                                   9


/*- Defined values for attribute AUML24XX_ATTR_RRS_TRACE_STATE -*/	
#define AUML24XX_VAL_RRS_FALSE                                      1
#define AUML24XX_VAL_RRS_FULL                                       2
#define AUML24XX_VAL_RRS_PARTIAL                                    3

/*- Defined values for attribute AUML24XX_ATTR_PMOD_MEASUREMENT_DISPLAY -*/	
#define AUML24XX_VAL_PMOD_PRF                                       1
#define AUML24XX_VAL_PMOD_RDO                                       2

/*- Defined values for attribute AUML24XX_ATTR_ACTIVE_CHANNEL -*/	
#define AUML24XX_VAL_CHAN1                                          1
#define AUML24XX_VAL_CHAN2                                          2

/*- Defined values for auml24xx_ConfigureBNCOutputCharacteristics -*/	
#define AUML24XX_VAL_BNC_OUTPUT_DB                                  1
#define AUML24XX_VAL_BNC_OUTPUT_DBM                                 2
#define AUML24XX_VAL_BNC_OUTPUT_DBUV                                3
#define AUML24XX_VAL_BNC_OUTPUT_DBW                                 4
#define AUML24XX_VAL_BNC_OUTPUT_DBMV                                5
#define AUML24XX_VAL_BNC_OUTPUT_WATTS                               6


/*- Defined values for auml24xx_ConfigureGET -*/
#define AUML24XX_VAL_GET_0                                          0
#define AUML24XX_VAL_GET_1                                          1
#define AUML24XX_VAL_GET_2                                          2




/**************************************************************************** 
 *---------------- Instrument Driver Function Declarations -----------------* 
 ****************************************************************************/

    /*- Init and Close Functions -------------------------------------------*/
ViStatus _VI_FUNC  auml24xx_init (ViRsrc resourceName, ViBoolean IDQuery,
                                  ViBoolean resetDevice, ViSession *vi);
ViStatus _VI_FUNC  auml24xx_InitWithOptions (ViRsrc resourceName, ViBoolean IDQuery,
                                             ViBoolean resetDevice, ViConstString optionString, 
                                             ViSession *newVi);
ViStatus _VI_FUNC  auml24xx_close (ViSession vi);   

    /*- Coercion Info Functions --------------------------------------------*/
ViStatus _VI_FUNC  auml24xx_GetNextCoercionRecord (ViSession vi,
                                                   ViInt32 bufferSize,
                                                   ViChar record[]);
    
    /*- Locking Functions --------------------------------------------------*/
ViStatus _VI_FUNC  auml24xx_LockSession (ViSession vi, ViBoolean *callerHasLock);   
ViStatus _VI_FUNC  auml24xx_UnlockSession (ViSession vi, ViBoolean *callerHasLock);

    /*- Channel Info Functions ---------------------------------------------*/
ViStatus _VI_FUNC auml24xx_GetChannelName (ViSession vi, ViInt32 index, 
                                   ViInt32 bufferSize, ViChar name[]);
    
    /*- IviPwrMeterBase Configuration Functions -*/
ViStatus _VI_FUNC auml24xx_ConfigureMeasurement(
    ViSession       vi,
    ViInt32         Operator,
    ViConstString   channelNameOne,
    ViConstString   channelNameTwo
);
ViStatus _VI_FUNC auml24xx_ConfigureUnits(
    ViSession       vi,
    ViInt32         inputUnits
);
ViStatus _VI_FUNC auml24xx_ConfigureRangeAutoEnabled(
    ViSession       vi,
    ViConstString   channelName,
    ViBoolean       enabled
);
ViStatus _VI_FUNC auml24xx_ConfigureAveragingEnabled(
	ViSession  vi,
	ViConstString Channel,
	ViBoolean AveragingEnabled
);

ViStatus _VI_FUNC auml24xx_ConfigureAveragingAutoEnabled(
    ViSession       vi,
    ViConstString   channelName,
    ViBoolean       autoAveragingEnabled
);
ViStatus _VI_FUNC auml24xx_ConfigureCorrectionFrequency(
    ViSession       vi,
    ViConstString   channelName,
    ViReal64        frequency
);


ViStatus _VI_FUNC auml24xx_ConfigureMeasurementMode(
	ViSession  vi,ViConstString Channel,
	ViInt32 MeasurementMode
);

ViStatus _VI_FUNC auml24xx_ConfigureSettlePercentage(
	ViSession  vi,
	ViConstString Channel,
	ViReal64 SettlePercentage
);

ViStatus _VI_FUNC auml24xx_ConfigureOffset(
    ViSession       vi,
    ViConstString   channelName,
    ViReal64        offset
);


ViStatus _VI_FUNC auml24xx_ConfigurePulsedModulatedMeasurement(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 MeasurementType,
	ViInt32 DisplayType,
	ViInt32 SampleRate
);


ViStatus _VI_FUNC auml24xx_ConfigureRelativeMeasurement(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 RelativeModeControl
);

        /*- IviPwrMeterTriggerSource Function -*/
ViStatus _VI_FUNC auml24xx_ConfigureTriggerSource(
    ViSession   vi,
    ViInt32     triggerSource
);

ViStatus _VI_FUNC auml24xx_ConfigureGET(
	ViSession  vi,
	ViInt32 GETType
);

ViStatus _VI_FUNC auml24xx_ConfigureTrigger(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 ArmingMode,
	ViReal64 CaptureTime,
	ViReal64 DelayTime
);

ViStatus _VI_FUNC auml24xx_ConfigureTriggerFrameArming(
	ViSession  vi,
	ViConstString Channel,  
	ViReal64 ArmingLevel,
	ViReal64 ArmingTimeDuration
);


ViStatus _VI_FUNC auml24xx_ConfigureTriggerLinkingEnabled(
	ViSession  vi,
	ViBoolean TriggerLinkingEnabled
);

ViStatus _VI_FUNC auml24xx_ConfigurePMODTriggerHoldoff(
	ViSession  vi,
	ViConstString Channel,
    ViBoolean TriggerHoldoffEnabled,
	ViReal64 TriggerHoldoffTime
);

ViStatus _VI_FUNC auml24xx_ConfigurePMODTrigger(
	ViSession  vi,
	ViConstString Channel,
	ViBoolean AutoTriggeringEnabled,
	ViInt32 TriggerBandwidth
);

ViStatus _VI_FUNC auml24xx_ConfigureExternalTriggerEdge(
	ViSession  vi,
	ViInt32 ExternaTriggerEdge
);

ViStatus _VI_FUNC auml24xx_EnterTriggerHoldMode(
    ViSession   vi
);

ViStatus _VI_FUNC auml24xx_QueryRRSTraceState(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 *RRSTraceState
);


        /*- IviPwrMeterInternalTrigger Functions -*/
ViStatus _VI_FUNC auml24xx_ConfigureInternalTrigger(
    ViSession       vi,
    ViConstString   eventSource,
    ViInt32         slope
);
ViStatus _VI_FUNC auml24xx_ConfigureInternalTriggerLevel(
    ViSession   vi,
    ViReal64    triggerLevel
);




        /*- IviPwrMeterDutyCycleCorrection Function -*/
ViStatus _VI_FUNC auml24xx_ConfigureDutyCycleCorrection(
    ViSession       vi,
    ViConstString   channelName,
    ViBoolean       enabled,
    ViReal64        correctionValue
);

/* /*- IviPwrMeterChannel Function -*/
ViStatus _VI_FUNC auml24xx_ConfigureActiveChannel(
	ViSession  vi,
	ViConstString Channel
);

ViStatus _VI_FUNC auml24xx_ResetMinMaxTracking(
	ViSession  vi,
    ViConstString Channel
);

ViStatus _VI_FUNC auml24xx_ConfigureMinMaxTrackingEnabled(
	ViSession  vi,
	ViConstString Channel,
	ViBoolean TrackingEnabled
);


ViStatus _VI_FUNC auml24xx_ConfigureDualChannelEnabled(
	ViSession       vi,
	ViBoolean DualChannelEnabled
);

       /*- IviPwrMeterAveragingCount Function -*/
ViStatus _VI_FUNC auml24xx_ConfigureAveragingCount(
    ViSession       vi,
    ViConstString   channelName,
    ViInt32         averagingCount
);

ViStatus _VI_FUNC auml24xx_ConfigureCWAveragingMode(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 AveragingMode
);

ViStatus _VI_FUNC auml24xx_ResetPMODProfileAveraging(
	ViSession  vi,
	ViConstString Channel
);
				 


/*-auml24xx Gate Functions -*/
ViStatus _VI_FUNC  auml24xx_ConfigureRepeateGates(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 GatePattern1RepeatNumber,
	ViBoolean GatePattern1RepeatEnabled,
	ViReal64 GatePattern1RepeatOffset
);

ViStatus _VI_FUNC auml24xx_ConfigureActiveGatingPattern(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 ActiveGatingPattern
);

ViStatus _VI_FUNC auml24xx_ConfigureFence(
	ViSession  vi,
	ViConstString Channel,
	ViBoolean FenceEnabled,
	ViReal64 FenceStopTime,
	ViReal64 FenceStartTime
);

ViStatus _VI_FUNC auml24xx_ConfigureGating(
	ViSession  vi,
	ViConstString Channel,
	ViBoolean GateEnabled,
	ViReal64 GateStopTime,
	ViReal64 GateStartTime
);

ViStatus _VI_FUNC auml24xx_SwitchOffGatingPatterns(
	ViSession  vi,
	ViConstString Channel
);

/*-auml24xx Limit Functions -*/  

ViStatus _VI_FUNC auml24xx_ConfigureLimit(
	ViSession  vi,
	ViConstString Channel,
	ViBoolean LimitCheckingEnabled ,
	ViBoolean FailIndicatorHoldEnabled,
	ViInt32 LimitLineTestType,
	ViInt32 LimitCheckingType
);

ViStatus _VI_FUNC  auml24xx_ConfigureSimpleLimit(
	ViSession  vi,
	ViConstString Channel,
	ViReal64 LowerLimitLineValue,
	ViReal64 UpperLimitLineValue
);

ViStatus _VI_FUNC auml24xx_ConfigureComplexLimit(
	ViSession  vi,
	ViConstString Channel,
	ViReal64 ComplexLimitsPowerOffset,
	ViInt32 ComplexLimitsRepeatCount,
	ViBoolean ComplexLimitsRepeatEnabled,
	ViReal64 ComplexLimitsPowerReplicationOffset,
	ViReal64 ComplexLimitsTimeReplicationOffset,
	ViReal64 ComplexLimitsTimeOffset
);

ViStatus _VI_FUNC auml24xx_ConfigureComplexLimitSpecType(
	ViSession  vi,  
	ViConstString Channel, 
	ViInt32 ComplexLimitsSpecificationType
);
	
	
ViStatus _VI_FUNC auml24xx_ConfigureComplexLimitSpecActiveNumber(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 ComplexLimitsSpecificationActiveNumber
);

ViStatus _VI_FUNC auml24xx_QueryComplexLimitMemoryStore(
	ViSession  vi,
	ViInt32 StoreNumber,
	ViBoolean * StatusUsed
);

ViStatus _VI_FUNC auml24xx_ConfigureComplexLimitSpecificationIDName(
	ViSession  vi,
	ViInt32 StoreNumber,
	ViConstString Name 
);

ViStatus _VI_FUNC auml24xx_ConfigureComplexLimitSegment(
	ViSession  vi,
	ViReal64 StartTime,
	ViReal64 StopTime,
	ViReal64 UpLimitStart,
	ViReal64 UpLimitStop,
	ViReal64 LowLimitSart,
	ViReal64 LowLimitStop
);

ViStatus _VI_FUNC auml24xx_SaveComplexLimitSpecification(
	ViSession  vi, 
	ViInt32 MaxTime
);

/*-auml24xx Marker Functions -*/  
ViStatus _VI_FUNC auml24xx_DisableAllMarkers(
	ViSession  vi,
	ViConstString Channel
);

ViStatus _VI_FUNC auml24xx_ConfigureMarker(
	ViSession  vi,
	ViConstString Channel,
	ViBoolean MarkerEnabled,
	ViReal64 MarkerPosition
);

ViStatus _VI_FUNC auml24xx_ConfigureMarkerActive(
	ViSession  vi,
	ViConstString Channel
);

ViStatus _VI_FUNC auml24xx_PositionActiveMarkerToMax(
	ViSession  vi,
	ViConstString Channel
);

ViStatus _VI_FUNC auml24xx_PositionActiveMarkerToMin(
	ViSession  vi,
	ViConstString Channel
);

ViStatus _VI_FUNC  auml24xx_ConfigureAdvancedMarkerSearch(
	ViSession  vi,
	ViConstString Channel,
	ViReal64 LowerTarget,
	ViReal64 UpperTarget,
	ViInt32 StartValueSource
);

ViStatus _VI_FUNC auml24xx_ConfigureDeltaMarker(
	ViSession  vi,
	ViConstString Channel,
	ViBoolean DeltaMarkerEnabled,
	ViBoolean DeltaMarkerLinkingEnabled,
	ViInt32 DeltaMarkerMeasurementType,
	ViReal64 DeltaMarkerPosition
);

ViStatus _VI_FUNC auml24xx_ConfigureMarkerNumber(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 MarkerNumber
);


/*-auml24xx Processing Functions -*/ 
ViStatus _VI_FUNC auml24xx_ConfigurePostProcessingAcquisition(
	ViSession  vi,
	ViConstString Channel,
	ViBoolean PostAcqusitionEnabled,
	ViInt32 FunctionModule
);

ViStatus _VI_FUNC auml24xx_ConfigureStatisticalPostProcessing(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 FunctionType,
	ViReal64 StatisticalPostProcessingMarkerPosition,
	ViBoolean StatisticalPostProcessingMarkerEnabled,ViInt32 SourceSelection
);

ViStatus _VI_FUNC auml24xx_RestartPostProcessingAcquisition(
	ViSession  vi,
	ViConstString Channel
);

ViStatus _VI_FUNC auml24xx_ConfigurePowerAddedEfficiencyProcessing(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 PAEInputConfiguration,
	ViReal64 PAEBiasCurrentValue,
	ViReal64 PAEBiasCurrentConversionFactor,
	ViInt32 PAEBiasCurrentSource,
	ViReal64 PAEBiasVoltageValue,
	ViInt32 PAESourceSelection
);



/*-auml24xx Sensor Functions -*/ 
ViStatus _VI_FUNC auml24xx_ConfigureSensor(
	ViSession  vi,
	ViConstString Sensor,
	ViBoolean SensorFilterState,
	ViInt32 RangeHoldType
);

ViStatus _VI_FUNC auml24xx_ConfigureSensorOperatingMode(
	ViSession  vi,
	ViConstString Sensor,
	ViInt32 UniversalSensorOperatingMode
);


ViStatus _VI_FUNC auml24xx_ConfigureSensorOffset(
    ViSession       vi,
    ViConstString   channelName,
    ViReal64        sensorOffset
);

ViStatus _VI_FUNC auml24xx_ConfigureActiveOffsetTableNumber(
	ViSession  vi,
	ViConstString Sensor,
	ViInt32 ActiveTableNumber
);

ViStatus _VI_FUNC auml24xx_ConfigureSensorOffsetType(
	ViSession  vi,
	ViConstString Sensor,
	ViInt32 OffsetType
);

ViStatus _VI_FUNC auml24xx_QuerySensorOffsetValue(
	ViSession  vi,
	ViConstString Sensor,
	ViReal64 * OffsetValue
);

ViStatus _VI_FUNC auml24xx_ClearOffsetTable(
	ViSession  vi,
	ViInt32 TableNumber,
	ViInt32 MaxTime
);

ViStatus _VI_FUNC auml24xx_ConfigureOffsetTableEntry(
	ViSession  vi,
	ViInt32 TableNumber,
	ViReal64 Frequency,
	ViReal64 Offset
);

ViStatus _VI_FUNC auml24xx_ConfigureOffsetTableIDName(
	ViSession  vi,
	ViInt32 TableNumber,
	ViConstString IDName
);

ViStatus _VI_FUNC auml24xx_QueryValidOffsetTable(
	ViSession  vi,
	ViInt32 TableNumber,
	ViBoolean * Valid
);


ViStatus _VI_FUNC auml24xx_ConfigureSensorCorrectionFrequency(
    ViSession       vi,
    ViConstString   channelName,
    ViReal64        sensorCorrectionFrequency
);

ViStatus _VI_FUNC auml24xx_ConfigureCalibrationFactor(
	ViSession  vi,
	ViConstString Sensor,
	ViReal64 SensorCalibrationFactorAdjust,
	ViReal64 CalibrationFactorManualValue,
	ViInt32 SensorCalFactorSource,
	ViInt32 SensorCalFactorUnits
);

ViStatus _VI_FUNC auml24xx_ConfigureVGHZCalibrationFactor(
	ViSession  vi,
	ViConstString Sensor,
	ViReal64 CalibrationFactorStopFrequency,
	ViReal64 CalibrationFactorStartFrequency,
	ViReal64 CalibrationFactorStopVoltage,
	ViReal64 CalibrationFactorStartVoltage
);

ViStatus _VI_FUNC auml24xx_QueryCurrentCalibrationFactor(
	ViSession  vi,
	ViConstString Sensor,
	ViReal64 * CalibrationFactorValue
);

ViStatus _VI_FUNC auml24xx_ConfigureCalibrationFactorActiveTableNumber(
	ViSession  vi,
	ViConstString Sensor,
	ViInt32 TableNumber
);

ViStatus _VI_FUNC auml24xx_QueryValidCalibrationFactorTable(
	ViSession  vi,
	ViConstString Sensor,
	ViInt32 TableNumber,
	ViBoolean * Valid
);

ViStatus _VI_FUNC auml24xx_SaveCalibrationFactorTable(
	ViSession  vi,
	ViInt32 MaxTime
);														   

ViStatus _VI_FUNC auml24xx_PresetCalibrationFactorTable(
	ViSession  vi, 
	ViConstString Sensor, 
	ViInt32 TableNumber,
	ViInt32 MaxTime); 

ViStatus _VI_FUNC auml24xx_QueryCalibrationFactorActiveTableNumber(
	ViSession  vi,
	ViConstString Sensor,
	ViInt32 * NumberOfTable
);

ViStatus _VI_FUNC auml24xx_ConfigureCalibrationFactorTableIDName(
	ViSession  vi,
	ViConstString Sensor,
	ViInt32 TableNumber,
	ViConstString IDName
);

ViStatus _VI_FUNC auml24xx_ClearCalibrationFactorTable(
	ViSession  vi,
	ViConstString Sensor,
	ViInt32 TableNumber,
	ViInt32 MaxTime
);

ViStatus _VI_FUNC auml24xx_ConfigureCalibrationFactorTableEntry(
	ViSession  vi,
	ViConstString Sensor,
	ViInt32 TableNumber,
	ViReal64 Frequency,
	ViReal64 CalibrationFactor,
	ViInt32  UnitType
);

 
/*-auml24xx System Functions -*/    
ViStatus _VI_FUNC auml24xx_ConfigureSystem(
	ViSession  vi,
	ViBoolean GPIBBufferingEnabled,
    ViBoolean FastModeEnabled,
 	ViInt32 NumberOfPoints
);


ViStatus _VI_FUNC auml24xx_ConfigureBNCOutput(
	ViSession  vi,
	ViConstString BNCNumber,
	ViInt32 OutputMode,
	ViInt32 PassVoltageLevel,
	ViInt32 ChannelNumber  
);

ViStatus _VI_FUNC auml24xx_ConfigureBNCOutputCharacteristics(
	ViSession vi,
	ViConstString BNCNumber,
	ViReal64 AnalogOutputDisplayPowerStop,
	ViReal64 AnalogOutputDisplayPowerStart,
	ViReal64 AnalogOutputStopVoltageScale,
	ViReal64 AnalogOutputStartVoltageScale,
	ViInt32 UnitType
);

/*- IviPwrMeterZeroCorrection Functions -*/
ViStatus _VI_FUNC auml24xx_IsZeroComplete(
    ViSession   vi,
    ViInt32    *atatus
);

ViStatus _VI_FUNC auml24xx_Zero(
    ViSession       vi,
    ViConstString   channelName
);

ViStatus _VI_FUNC auml24xx_ZeroAllChannels(
    ViSession   vi
);

ViStatus _VI_FUNC  auml24xx_ZeroBNCInputConnector(
	ViSession  vi, 
	ViInt32 MaxTime
);


/*-auml24xx Range Calibrator Functions -*/    
ViStatus _VI_FUNC auml24xx_AbortRangeCalibratorTest(
	ViSession  vi,
	ViInt32 MaxTime
);

ViStatus _VI_FUNC auml24xx_ZeroRangeCalibratorSensorInput(
	ViSession  vi,
	ViInt32 MaxTime
);

ViStatus _VI_FUNC auml24xx_InitiateRangeCalibratorFullTest(
	ViSession  vi,
	ViConstString Sensor,
	ViInt32 MaxTime
);

ViStatus _VI_FUNC auml24xx_ConfigureRangeCalibratorDiagnosticTest(
	ViSession  vi,
	ViConstString Sensor,
	ViInt32 TestType
);

ViStatus _VI_FUNC auml24xx_OutputRangeCalibratorDiagnosticTestData(
	ViSession  vi,
	ViConstString Sensor,
	ViBoolean *Valid,
	ViInt32 *ActualPoints, 
	ViReal64 Data[],
	ViInt32 Length
);

	/*- IviPwrMeterCalibration Function -*/
ViStatus _VI_FUNC auml24xx_IsCalibrationComplete(
    ViSession   vi,
    ViInt32    *status
);
  
ViStatus _VI_FUNC auml24xx_Calibrate(
    ViSession       vi,
    ViConstString   channelName
);

        /*- IviPwrMeterReferenceOscillator Functions -*/
ViStatus _VI_FUNC auml24xx_ConfigureRefOscillatorEnabled(
    ViSession       vi,
    ViBoolean       enabled
);

ViStatus _VI_FUNC auml24xx_ConfigureRefOscillator(
    ViSession       vi,
    ViReal64        frequency,
    ViReal64        power
);

   /*- IviPwrMeter Measurement Functions -*/
ViStatus _VI_FUNC auml24xx_Read(
    ViSession       vi,
    ViInt32         maxTime,
    ViReal64       *result
);

ViStatus _VI_FUNC auml24xx_ReadPMODTrace(
	ViSession       vi,
	ViConstString   ChannelName,
	ViInt32 *ActualPoints, 
	ViReal64 TraceArray[],
	ViInt32  Length,
	ViReal64 *InitialX,
	ViReal64 *xIncrement,
	ViInt32   MaxTime
);

ViStatus _VI_FUNC auml24xx_ReadPMODReadOut(
	ViSession       vi,
	ViConstString   ChannelName,
	ViInt32 *ActualPoints, 
	ViReal64 MeasurementData[],
	ViInt32         MaxTime
);

ViStatus _VI_FUNC auml24xx_ReadPMODMaxTrace(
	ViSession       vi,
	ViConstString   ChannelName,
	ViInt32 *ActualPoints, 
	ViReal64 TraceArray[],
	ViInt32 Length,
	ViReal64 *InitialX,
	ViReal64 *xIncrement,
	ViInt32         MaxTime
);

ViStatus _VI_FUNC auml24xx_ReadPMODMinTrace(
	ViSession       vi,
	ViConstString   ChannelName,
	ViInt32 *ActualPoints, 
	ViReal64 TraceArray[],
	ViInt32 Length,
	ViReal64 *InitialX,
	ViReal64 *xIncrement,
	ViInt32         MaxTime
);

ViStatus _VI_FUNC auml24xx_ReadCWData(
	ViSession       vi,
	ViConstString   ChannelName,
	ViReal64 *CWdata,
	ViInt32         MaxTime    	
);



ViStatus _VI_FUNC auml24xx_ReadMarker(
	ViSession       vi,
	ViConstString   ChannelName,
	ViInt32  MarkerNumber,
	ViReal64 *MeasurementData,
	ViReal64 *Position,
	ViInt32         MaxTime
);

ViStatus _VI_FUNC auml24xx_ReadDeltaMarker(
	ViSession       vi,
	ViConstString   ChannelName,
	ViReal64 *Power,
	ViReal64 *Position,
	ViInt32         MaxTime
);

ViStatus _VI_FUNC auml24xx_ReadPulseMeasurement(
	ViSession  vi,
	ViConstString Channel,
	ViReal64 * PulseFallTime,
	ViReal64 * PulseRiseTime,
	ViReal64 * PulseOffTime,
	ViReal64 * PulseWidth,
	ViReal64 * PulseRepetitionInterval,
	ViInt32         MaxTime
);

ViStatus _VI_FUNC auml24xx_ReadStatisticalCursor(
	ViSession       vi,
	ViConstString   ChannelName,
	ViReal64 *Percentage,
	ViReal64 *Power,
	ViInt32         MaxTime
);

ViStatus _VI_FUNC auml24xx_ReadPAE(
	ViSession       vi,
	ViConstString   ChannelName,
	ViReal64 *PAEData,
	ViInt32         MaxTime
);

ViStatus _VI_FUNC auml24xx_ReadStatPostProcessingTrace(
	ViSession       vi,
	ViConstString   ChannelName,
	ViInt32 *ActualPoints, 
	ViReal64 TraceArray[],
	ViInt32 Length,
	ViReal64 *InitialX,
	ViReal64 *xIncrement,
	ViInt32         MaxTime
);

ViStatus _VI_FUNC auml24xx_ReadGating(
	ViSession  vi,
    ViConstString Channel,
	ViInt32 GateNumber,
	ViInt32 *ActualPoints, 
	ViReal64 MeasurementData[],
	ViInt32         MaxTime
);

        /*- Low-Level Measurement Functions -*/
ViStatus _VI_FUNC auml24xx_Fetch(
    ViSession   vi,
    ViReal64   *reading
);

ViStatus _VI_FUNC auml24xx_Initiate(
    ViSession   vi
);
ViStatus _VI_FUNC auml24xx_IsMeasurementComplete(
    ViSession   vi,
    ViInt32    *measurementStatus
);

ViStatus _VI_FUNC auml24xx_Abort(
    ViSession   vi
);

  
        /*- IviPwrMeterSoftwareTrigger Function -*/
ViStatus _VI_FUNC auml24xx_SendSoftwareTrigger(
    ViSession   vi
);


ViStatus _VI_FUNC auml24xx_QueryResultRangeType(
    ViSession   vi,
    ViReal64 measurementValue,
    ViInt32 *rangeType
);

ViStatus _VI_FUNC auml24xx_FetchHeader(
	ViSession       vi,
	ViConstString   ChannelName,
	ViReal64        *HeaderData,
	ViInt32         MaxTime
);

ViStatus _VI_FUNC auml24xx_FetchPMODTrace(
	ViSession       vi,
	ViConstString   ChannelName,
	ViInt32 *ActualPoints, 
	ViReal64 TraceData[],
	ViInt32  Length,
	ViReal64 *InitialX,
	ViReal64 *xIncrement
);

ViStatus _VI_FUNC auml24xx_FetchPMODReadOut(
	ViSession       vi,
	ViConstString   ChannelName,
	ViInt32 *ActualPoints, 
	ViReal64 MeasurementData[]
);

ViStatus _VI_FUNC auml24xx_FetchPMODMaxTrace(
	ViSession       vi,
	ViConstString   ChannelName,
	ViInt32 *ActualPoints, 
	ViReal64 Trace[],
	ViInt32 Length,
	ViReal64 *InitialX,
	ViReal64 *xIncrement
);

ViStatus _VI_FUNC auml24xx_FetchPMODMinTrace(
	ViSession       vi,
	ViConstString   ChannelName,
	ViInt32 *ActualPoints, 
	ViReal64 TraceData[],
	ViInt32 Length,
	ViReal64 *InitialX,
	ViReal64 *xIncrement
);

ViStatus _VI_FUNC auml24xx_FetchCWData(
	ViSession       vi,
	ViConstString   ChannelName,
	ViReal64 *CWdata
);

ViStatus _VI_FUNC auml24xx_FetchMarker(
	ViSession       vi,
	ViConstString   ChannelName,
	ViInt32  MarkerNumber,
	ViReal64 *Power,
	ViReal64 *Position
);

ViStatus _VI_FUNC auml24xx_FetchDeltaMarker(
	ViSession       vi,
	ViConstString   ChannelName,
	ViReal64 *Power,
	ViReal64 *Position
);

ViStatus _VI_FUNC auml24xx_FetchPulseMeasurement(
	ViSession  vi,
	ViConstString Channel,
	ViReal64 * PulseFallTime,
	ViReal64 * PulseRiseTime,
	ViReal64 * PulseOffTime,
	ViReal64 * PulseWidth,
	ViReal64 * PulseRepetitionInterval
);

ViStatus _VI_FUNC auml24xx_FetchStatisticalCursor(
	ViSession       vi,
	ViConstString   ChannelName,
	ViReal64 *Percentage,
	ViReal64 *Power
);

ViStatus _VI_FUNC auml24xx_FetchPAE(
	ViSession       vi,
	ViConstString   ChannelName,
	ViReal64 *PAEData
);

ViStatus _VI_FUNC auml24xx_FetchStatisticalPostProcessingTrace(
	ViSession       vi,
	ViConstString   ChannelName,
	ViInt32 *ActualPoints, 
	ViReal64 TraceData[],
	ViInt32 Length,
	ViReal64 *InitialX,
	ViReal64 *xIncrement
);

ViStatus _VI_FUNC auml24xx_FetchGating(
	ViSession  vi,
	ViConstString Channel,
	ViInt32 GateNumber,
	ViInt32 *ActualPoints, 
	ViReal64 MeasurementData[]
);


/*- Error Functions ----------------------------------------------------*/
ViStatus _VI_FUNC auml24xx_error_query (ViSession vi, ViInt32 *errCode,
	                                    ViChar errorMessage[]);

ViStatus _VI_FUNC auml24xx_GetError (ViSession vi, ViStatus *code, 
                                      ViInt32 bufferSize, ViChar description[]);
ViStatus _VI_FUNC auml24xx_ClearError (ViSession vi);

ViStatus _VI_FUNC auml24xx_error_message (ViSession vi, ViStatus errorCode,
                                           ViChar errorMessage[256]);


    /*- Interchangeability Checking Functions ------------------------------*/
ViStatus _VI_FUNC auml24xx_GetNextInterchangeWarning (ViSession vi, 
                                                      ViInt32 bufferSize, 
                                                      ViChar warnString[]);
ViStatus _VI_FUNC auml24xx_ResetInterchangeCheck (ViSession vi);
ViStatus _VI_FUNC auml24xx_ClearInterchangeWarnings (ViSession vi);

    /*- Utility Functions --------------------------------------------------*/
ViStatus _VI_FUNC auml24xx_InvalidateAllAttributes (ViSession vi);
ViStatus _VI_FUNC auml24xx_reset (ViSession vi);
ViStatus _VI_FUNC auml24xx_ResetWithDefaults (ViSession vi);
ViStatus _VI_FUNC auml24xx_self_test (ViSession vi, ViInt16 *selfTestResult,
                                       ViChar selfTestMessage[]);
ViStatus _VI_FUNC auml24xx_revision_query (ViSession vi, 
                                            ViChar instrumentDriverRevision[],
                                            ViChar firmwareRevision[]);
ViStatus _VI_FUNC auml24xx_Disable (ViSession vi);
ViStatus _VI_FUNC auml24xx_WriteInstrData (ViSession vi, ViConstString writeBuffer); 
ViStatus _VI_FUNC auml24xx_ReadInstrData  (ViSession vi, ViInt32 numBytes, 
                                            ViChar rdBuf[], ViInt32 *bytesRead);

    /*- Set, Get, and Check Attribute Functions ----------------------------*/
ViStatus _VI_FUNC auml24xx_GetAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 *value);
ViStatus _VI_FUNC auml24xx_GetAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 *value);
ViStatus _VI_FUNC auml24xx_GetAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 bufSize, ViChar value[]); 
ViStatus _VI_FUNC auml24xx_GetAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession *value);
ViStatus _VI_FUNC auml24xx_GetAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean *value);

ViStatus _VI_FUNC auml24xx_SetAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 value);
ViStatus _VI_FUNC auml24xx_SetAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 value);
ViStatus _VI_FUNC auml24xx_SetAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViConstString value); 
ViStatus _VI_FUNC auml24xx_SetAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession value);
ViStatus _VI_FUNC auml24xx_SetAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean value);

ViStatus _VI_FUNC auml24xx_CheckAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 value);
ViStatus _VI_FUNC auml24xx_CheckAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 value);
ViStatus _VI_FUNC auml24xx_CheckAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViConstString value); 
ViStatus _VI_FUNC auml24xx_CheckAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession value);
ViStatus _VI_FUNC auml24xx_CheckAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean value);



/****************************************************************************
 *------------------------ Error And Completion Codes ----------------------*
 ****************************************************************************/

#define AUML24XX_WARN_UNDER_RANGE                      IVIPWRMETER_WARN_UNDER_RANGE
#define AUML24XX_WARN_OVER_RANGE                       IVIPWRMETER_WARN_OVER_RANGE

#define AUML24XX_ERROR_CHANNEL_NOT_ENABLED             IVIPWRMETER_ERROR_CHANNEL_NOT_ENABLED
#define AUML24XX_ERROR_MAX_TIME_EXCEEDED               IVIPWRMETER_ERROR_MAX_TIME_EXCEEDED
#define AUML24XX_ERROR_TRIGGER_NOT_SOFTWARE            IVIPWRMETER_ERROR_TRIGGER_NOT_SOFTWARE

#define AUML24XX_WRNMSG_UNDER_RANGE                    IVIPWRMETER_WRNMSG_UNDER_RANGE
#define AUML24XX_WRNMSG_OVER_RANGE                     IVIPWRMETER_WRNMSG_OVER_RANGE

#define AUML24XX_ERRMSG_CHANNEL_NOT_ENABLED            IVIPWRMETER_ERRMSG_CHANNEL_NOT_ENABLED
#define AUML24XX_ERRMSG_MAX_TIME_EXCEEDED              IVIPWRMETER_ERRMSG_MAX_TIME_EXCEEDED


#define AUML24XX_ERROR_CODES_AND_MSGS\
         {AUML24XX_WARN_UNDER_RANGE                    AUML24XX_WRNMSG_UNDER_RANGE}, \
         {AUML24XX_WARN_OVER_RANGE                     AUML24XX_WRNMSG_OVER_RANGE}, \
         {AUML24XX_ERROR_CHANNEL_NOT_ENABLED           AUML24XX_ERRMSG_CHANNEL_NOT_ENABLED}, \
         {AUML24XX_ERROR_MAX_TIME_EXCEEDED             AUML24XX_ERRMSG_MAX_TIME_EXCEEDED}


#define AUML24XX_ERROR_ATTR_NO_INTERNAL_TRIIGER_SOURCE	(IVI_SPECIFIC_ERROR_BASE + 1) 
#define AUML24XX_ERROR_ATTR_AVERAGE_NUMBER_UNAVAILABLE  (IVI_SPECIFIC_ERROR_BASE + 2) 

/**************************************************************************** 
 *---------------------------- End Include File ----------------------------* 
 ****************************************************************************/
#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif
#endif /* __AUML24XX_HEADER */




