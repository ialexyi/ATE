/***************************************************************************** 
 * 2009 - 2010, National Instruments, Corporation.  All Rights Reserved.     * 
 *****************************************************************************/

/****************************************************************************
 *                       Agilent 1000 Series Oscilloscopes                           
 *                                                                          
 * Title:    ag1000ni.h                                        
 * Purpose:  Agilent 1000 Series Oscilloscopes                                       
 *           instrument driver declarations.                                
 * 
 *  Modification History:                                                    
 *  
 *       05/05/2010 - [czhou] Porting driver to 64-bits, increase minor version
 *
 *       05/13/2010 - [czhou] Fix CAR #214589: Update Driver Vendor to 
 *                            National Instruments
 ****************************************************************************/

#ifndef __AG1000NI_HEADER
#define __AG1000NI_HEADER

#include <ivi.h>
#include <iviScope.h>   

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif


/****************************************************************************
 *----------------- Instrument Driver Revision Information -----------------*
 ****************************************************************************/ 
#define AG1000NI_MAJOR_VERSION 1 /* Instrument driver major version   */
#define AG1000NI_MINOR_VERSION 2 /* Instrument driver minor version   */

#define AG1000NI_CLASS_SPEC_MAJOR_VERSION 3 /* Class specification major version */
#define AG1000NI_CLASS_SPEC_MINOR_VERSION 0 /* Class specification minor version */

#define AG1000NI_SUPPORTED_INSTRUMENT_MODELS  "DSO1002A,DSO1004A,DSO1012A,DSO1014A,DSO1022A,DSO1024A"
#define AG1000NI_DRIVER_VENDOR                "National Instruments"
    
#ifdef _IVI_mswin64_
#define AG1000NI_DRIVER_DESCRIPTION           "Agilent 1000 Series Oscilloscopes IVI Driver [Compiled for 64-bit.]"
#else
#define AG1000NI_DRIVER_DESCRIPTION           "Agilent 1000 Series Oscilloscopes IVI Driver"
#endif  


/**************************************************************************** 
 *------------------------------ Useful Macros -----------------------------* 
 ****************************************************************************/
     /* 
        Defined values for the measurementFunction parameter of the 
        ag1000ni_ReadWaveformMeasurement function.
    */
#define AG1000NI_VAL_RISE_TIME            IVISCOPE_VAL_RISE_TIME             
#define AG1000NI_VAL_FALL_TIME            IVISCOPE_VAL_FALL_TIME             
#define AG1000NI_VAL_FREQUENCY            IVISCOPE_VAL_FREQUENCY             
#define AG1000NI_VAL_PERIOD               IVISCOPE_VAL_PERIOD                
#define AG1000NI_VAL_VOLTAGE_RMS          IVISCOPE_VAL_VOLTAGE_RMS           
#define AG1000NI_VAL_VOLTAGE_PEAK_TO_PEAK IVISCOPE_VAL_VOLTAGE_PEAK_TO_PEAK  
#define AG1000NI_VAL_VOLTAGE_MAX          IVISCOPE_VAL_VOLTAGE_MAX           
#define AG1000NI_VAL_VOLTAGE_MIN          IVISCOPE_VAL_VOLTAGE_MIN           
#define AG1000NI_VAL_VOLTAGE_HIGH         IVISCOPE_VAL_VOLTAGE_HIGH          
#define AG1000NI_VAL_VOLTAGE_LOW          IVISCOPE_VAL_VOLTAGE_LOW           
#define AG1000NI_VAL_VOLTAGE_AVERAGE      IVISCOPE_VAL_VOLTAGE_AVERAGE       
#define AG1000NI_VAL_WIDTH_NEG            IVISCOPE_VAL_WIDTH_NEG             
#define AG1000NI_VAL_WIDTH_POS            IVISCOPE_VAL_WIDTH_POS             
#define AG1000NI_VAL_DUTY_CYCLE_NEG       IVISCOPE_VAL_DUTY_CYCLE_NEG        
#define AG1000NI_VAL_DUTY_CYCLE_POS       IVISCOPE_VAL_DUTY_CYCLE_POS        
#define AG1000NI_VAL_AMPLITUDE            IVISCOPE_VAL_AMPLITUDE             
#define AG1000NI_VAL_OVERSHOOT            IVISCOPE_VAL_OVERSHOOT             
#define AG1000NI_VAL_PRESHOOT             IVISCOPE_VAL_PRESHOOT              

    /* Defined emnus for ag1000ni_ReadDelayMeasurement and ag1000_ReadPhaseMeasurment. */
#define AG1000NI_VAL_RISING  1  
#define AG1000NI_VAL_FALLING 0  
    
        /* Defined values for the status parameter of the ag3000_AcquisitionStatus function */
#define AG1000NI_VAL_ACQ_COMPLETE       IVISCOPE_VAL_ACQ_COMPLETE        
#define AG1000NI_VAL_ACQ_STATUS_UNKNOWN IVISCOPE_VAL_ACQ_STATUS_UNKNOWN 
    
/**************************************************************************** 
 *---------------------------- Attribute Defines ---------------------------* 
 ****************************************************************************/

    /*- IVI Inherent Instrument Attributes ---------------------------------*/    

        /* User Options */
#define AG1000NI_ATTR_RANGE_CHECK             IVI_ATTR_RANGE_CHECK             /* ViBoolean */
#define AG1000NI_ATTR_QUERY_INSTRUMENT_STATUS IVI_ATTR_QUERY_INSTRUMENT_STATUS /* ViBoolean */
#define AG1000NI_ATTR_CACHE                   IVI_ATTR_CACHE                   /* ViBoolean */
#define AG1000NI_ATTR_SIMULATE                IVI_ATTR_SIMULATE                /* ViBoolean */
#define AG1000NI_ATTR_RECORD_COERCIONS        IVI_ATTR_RECORD_COERCIONS        /* ViBoolean */
#define AG1000NI_ATTR_INTERCHANGE_CHECK       IVI_ATTR_INTERCHANGE_CHECK       /* ViBoolean */

        /* Driver Information  */
#define AG1000NI_ATTR_SPECIFIC_DRIVER_PREFIX                   IVI_ATTR_SPECIFIC_DRIVER_PREFIX                   /* ViString, read-only  */
#define AG1000NI_ATTR_SUPPORTED_INSTRUMENT_MODELS              IVI_ATTR_SUPPORTED_INSTRUMENT_MODELS              /* ViString, read-only  */
#define AG1000NI_ATTR_GROUP_CAPABILITIES                       IVI_ATTR_GROUP_CAPABILITIES                       /* ViString, read-only  */
#define AG1000NI_ATTR_INSTRUMENT_MANUFACTURER                  IVI_ATTR_INSTRUMENT_MANUFACTURER                  /* ViString, read-only  */
#define AG1000NI_ATTR_INSTRUMENT_MODEL                         IVI_ATTR_INSTRUMENT_MODEL                         /* ViString, read-only  */
#define AG1000NI_ATTR_INSTRUMENT_FIRMWARE_REVISION             IVI_ATTR_INSTRUMENT_FIRMWARE_REVISION             /* ViString, read-only  */
#define AG1000NI_ATTR_SPECIFIC_DRIVER_REVISION                 IVI_ATTR_SPECIFIC_DRIVER_REVISION                 /* ViString, read-only  */
#define AG1000NI_ATTR_SPECIFIC_DRIVER_VENDOR                   IVI_ATTR_SPECIFIC_DRIVER_VENDOR                   /* ViString, read-only  */
#define AG1000NI_ATTR_SPECIFIC_DRIVER_DESCRIPTION              IVI_ATTR_SPECIFIC_DRIVER_DESCRIPTION              /* ViString, read-only  */
#define AG1000NI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION /* ViInt32, read-only */
#define AG1000NI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION /* ViInt32, read-only */

    /* Advanced Session Information */
#define AG1000NI_ATTR_LOGICAL_NAME           IVI_ATTR_LOGICAL_NAME           /* ViString, read-only  */
#define AG1000NI_ATTR_IO_RESOURCE_DESCRIPTOR IVI_ATTR_IO_RESOURCE_DESCRIPTOR /* ViString, read-only  */
#define AG1000NI_ATTR_DRIVER_SETUP           IVI_ATTR_DRIVER_SETUP           /* ViString, read-only  */
    
    /*- IviScope Acquisition Group Attributes ---------------*/
#define AG1000NI_ATTR_ACQUISITION_START_TIME IVISCOPE_ATTR_ACQUISITION_START_TIME /* ViReal64       */
#define AG1000NI_ATTR_ACQUISITION_TYPE       IVISCOPE_ATTR_ACQUISITION_TYPE       /* ViInt32        */
#define AG1000NI_ATTR_HORZ_MIN_NUM_PTS       IVISCOPE_ATTR_HORZ_MIN_NUM_PTS       /* ViInt32        */
#define AG1000NI_ATTR_HORZ_RECORD_LENGTH     IVISCOPE_ATTR_HORZ_RECORD_LENGTH     /* ViInt32        */
#define AG1000NI_ATTR_HORZ_SAMPLE_RATE       IVISCOPE_ATTR_HORZ_SAMPLE_RATE       /* ViReal64       */
#define AG1000NI_ATTR_HORZ_TIME_PER_RECORD   IVISCOPE_ATTR_HORZ_TIME_PER_RECORD   /* ViReal64       */
#define AG1000NI_ATTR_INITIATE_CONTINUOUS    IVISCOPE_ATTR_INITIATE_CONTINUOUS    /* ViBoolean      */
#define AG1000NI_ATTR_NUM_AVERAGES           IVISCOPE_ATTR_NUM_AVERAGES           /* ViInt32        */
#define AG1000NI_ATTR_SAMPLE_MODE            IVISCOPE_ATTR_SAMPLE_MODE            /* ViInt32        */

 /*- IviScope Channel Subsystem Group Attributes ---------------*/
#define AG1000NI_ATTR_CHANNEL_ENABLED     IVISCOPE_ATTR_CHANNEL_ENABLED     /* ViBoolean        */
#define AG1000NI_ATTR_VERTICAL_RANGE      IVISCOPE_ATTR_VERTICAL_RANGE      /* ViReal64     */
#define AG1000NI_ATTR_VERTICAL_OFFSET     IVISCOPE_ATTR_VERTICAL_OFFSET     /* ViReal64     */
#define AG1000NI_ATTR_VERTICAL_COUPLING   IVISCOPE_ATTR_VERTICAL_COUPLING   /* ViInt32      */
#define AG1000NI_ATTR_INPUT_IMPEDANCE     IVISCOPE_ATTR_INPUT_IMPEDANCE     /* ViReal64     */
#define AG1000NI_ATTR_MAX_INPUT_FREQUENCY IVISCOPE_ATTR_MAX_INPUT_FREQUENCY /* ViReal64     */
#define AG1000NI_ATTR_PROBE_ATTENUATION   IVISCOPE_ATTR_PROBE_ATTENUATION   /* ViReal64     */

 /*- IviScope Triggering Subsystem Group Attributes ---------------*/
#define AG1000NI_ATTR_TRIGGER_TYPE     IVISCOPE_ATTR_TRIGGER_TYPE     /* ViInt32        */
#define AG1000NI_ATTR_TRIGGER_SOURCE   IVISCOPE_ATTR_TRIGGER_SOURCE   /* ViString       */
#define AG1000NI_ATTR_TRIGGER_COUPLING IVISCOPE_ATTR_TRIGGER_COUPLING /* ViInt32        */
#define AG1000NI_ATTR_TRIGGER_HOLDOFF  IVISCOPE_ATTR_TRIGGER_HOLDOFF  /* ViReal64       */
#define AG1000NI_ATTR_TRIGGER_MODIFIER IVISCOPE_ATTR_TRIGGER_MODIFIER /* ViInt32        */

 /*- IviScope Triggering Subsystem -> Edge Triggering Group Attributes ---------------*/
#define AG1000NI_ATTR_TRIGGER_LEVEL IVISCOPE_ATTR_TRIGGER_LEVEL /* ViReal64     */
#define AG1000NI_ATTR_TRIGGER_SLOPE IVISCOPE_ATTR_TRIGGER_SLOPE /* ViInt32      */

 /*- IviScope Triggering Subsystem -> TV Trigger Group Attributes ---------------*/
#define AG1000NI_ATTR_TV_TRIGGER_EVENT         IVISCOPE_ATTR_TV_TRIGGER_EVENT         /* ViInt32        */
#define AG1000NI_ATTR_TV_TRIGGER_LINE_NUMBER   IVISCOPE_ATTR_TV_TRIGGER_LINE_NUMBER   /* ViInt32        */
#define AG1000NI_ATTR_TV_TRIGGER_POLARITY      IVISCOPE_ATTR_TV_TRIGGER_POLARITY      /* ViInt32        */
#define AG1000NI_ATTR_TV_TRIGGER_SIGNAL_FORMAT IVISCOPE_ATTR_TV_TRIGGER_SIGNAL_FORMAT /* ViInt32        */

 /*- IviScope Waveform Measurement Group Attributes ---------------*/
#define AG1000NI_ATTR_MEAS_HIGH_REF IVISCOPE_ATTR_MEAS_HIGH_REF /* ViReal64     */
#define AG1000NI_ATTR_MEAS_MID_REF  IVISCOPE_ATTR_MEAS_MID_REF  /* ViReal64     */
#define AG1000NI_ATTR_MEAS_LOW_REF  IVISCOPE_ATTR_MEAS_LOW_REF  /* ViReal64     */

 /*- Instrument-Specific Attributes -------------------------------------*/ 

#define AG1000NI_ATTR_ID_QUERY_RESPONSE (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 1L) /* ViString (Read Only) */
        /*- Channel Subsystem Group Attributes ---------------*/
#define AG1000NI_ATTR_INPUT_INVERSION_ENABLED (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 1002L) /* ViBoolean      */
#define AG1000NI_ATTR_DIGITAL_FILTER_ENABLED  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 1003L) /* ViBoolean      */

/*- Triggering Subsystem Group Attributes ---------------*/
#define AG1000NI_ATTR_TRIGGER_SENSITIVITY       (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 2001L) /* ViReal64     */
#define AG1000NI_ATTR_TRIGGER_HF_REJECT_ENABLED (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 2002L) /* ViBoolean    */

/*- Triggering Subsystem -> TV Trigger Group Attributes ---------------*/
#define AG1000NI_ATTR_TV_TRIGGER_LEVEL (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 3001L) /* ViReal64      */

/*- Triggering Subsystem -> Pattern Trigger Group Attributes ---------------*/
#define AG1000NI_ATTR_PATTERN_TRIGGER_LEVEL   (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 4001L) /* ViReal64       */
#define AG1000NI_ATTR_PATTERN_TRIGGER_PATTERN (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 4002L) /* ViString       */

/*- Triggering Subsystem -> Pulse Trigger Group Attributes ---------------*/
#define AG1000NI_ATTR_PULSE_TRIGGER_MODE  (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 5001L) /* ViInt32        */
#define AG1000NI_ATTR_PULSE_TRIGGER_LEVEL (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 5002L) /* ViReal64       */
#define AG1000NI_ATTR_PULSE_TRIGGER_WIDTH (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 5003L) /* ViReal64       */

/*- Triggering Subsystem -> Alternation Trigger Group Attributes ---------------*/
#define AG1000NI_ATTR_ALTERNATION_TRIGGER_COUPLING               (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 6001L) /* ViInt32     */
#define AG1000NI_ATTR_ALTERNATION_TRIGGER_HOLDOFF                (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 6002L) /* ViReal64    */
#define AG1000NI_ATTR_ALTERNATION_TRIGGER_SENSITIVITY            (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 6003L) /* ViReal64    */
#define AG1000NI_ATTR_ALTERNATION_TRIGGER_SOURCE                 (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 6004L) /* ViInt32     */
#define AG1000NI_ATTR_ALTERNATION_TRIGGER_TYPE                   (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 6006L) /* ViInt32     */
#define AG1000NI_ATTR_ALTERNATION_EDGE_TRIGGER_LEVEL             (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 6007L) /* ViReal64    */
#define AG1000NI_ATTR_ALTERNATION_EDGE_TRIGGER_SLOPE             (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 6008L) /* ViInt32     */
#define AG1000NI_ATTR_ALTERNATION_PULSE_TRIGGER_MODE             (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 6009L) /* ViInt32     */
#define AG1000NI_ATTR_ALTERNATION_PULSE_TRIGGER_WIDTH            (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 6010L) /* ViReal64    */
#define AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_LINE_NUMBER         (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 6011L) /* ViInt32     */
#define AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_EVENT               (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 6012L) /* ViInt32     */
#define AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_POLARITY            (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 6013L) /* ViInt32     */
#define AG1000NI_ATTR_ALTERNATION_TV_TRIGGER_SIGNAL_FORMAT       (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 6014L) /* ViInt32     */
#define AG1000NI_ATTR_ALTERNATION_TRIGGER_ACQUISITION_START_TIME (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 6015L) /* ViReal64    */     
#define AG1000NI_ATTR_ALTERNATION_TRIGGER_HORZ_TIME_PER_RECORD   (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 6016L) /* ViReal64    */    

/****************************************************************************
 *------------------------ Attribute Value Defines -------------------------*
****************************************************************************/

/*- Defined values for attribute AG1000NI_ATTR_ACQUISITION_TYPE -*/
#define AG1000NI_VAL_NORMAL      IVISCOPE_VAL_NORMAL       
#define AG1000NI_VAL_AVERAGE     IVISCOPE_VAL_AVERAGE      
#define AG1000NI_VAL_PEAK_DETECT IVISCOPE_VAL_PEAK_DETECT  

/*- Defined values for attribute AG1000NI_ATTR_TRIGGER_COUPLING -*/
#define AG1000NI_VAL_AC        IVISCOPE_VAL_AC         
#define AG1000NI_VAL_DC        IVISCOPE_VAL_DC         
#define AG1000NI_VAL_LF_REJECT IVISCOPE_VAL_LF_REJECT  

/*- Defined values for attribute AG1000NI_ATTR_TRIGGER_SLOPE -*/
#define AG1000NI_VAL_POSITIVE    IVISCOPE_VAL_POSITIVE  
#define AG1000NI_VAL_NEGATIVE    IVISCOPE_VAL_NEGATIVE  
#define AG1000NI_VAL_ALTERNATION (1001L)                

/*- Defined values for attribute AG1000NI_ATTR_TRIGGER_TYPE -*/
#define AG1000NI_VAL_EDGE_TRIGGER        IVISCOPE_VAL_EDGE_TRIGGER                                             
#define AG1000NI_VAL_TV_TRIGGER          IVISCOPE_VAL_TV_TRIGGER                                               
#define AG1000NI_VAL_PULSE_TRIGGER       (IVISCOPE_VAL_TRIGGER_TYPE_SPECIFIC_EXT_BASE + 1L)                    
#define AG1000NI_VAL_PATTERN_TRIGGER     (IVISCOPE_VAL_TRIGGER_TYPE_SPECIFIC_EXT_BASE + 2L)                    
#define AG1000NI_VAL_ALTERNATION_TRIGGER (IVISCOPE_VAL_TRIGGER_TYPE_SPECIFIC_EXT_BASE + 3L)                    

/*- Defined values for attribute AG1000NI_ATTR_VERTICAL_COUPLING -*/
#define AG1000NI_VAL_GND IVISCOPE_VAL_GND  
                                                                
/*- Defined values for attribute AG1000NI_ATTR_TV_TRIGGER_EVENT -*/
#define AG1000NI_VAL_TV_EVENT_LINE_NUMBER IVISCOPE_VAL_TV_EVENT_LINE_NUMBER                                                 
#define AG1000NI_VAL_TV_EVENT_ALL_LINE    (IVISCOPE_VAL_TV_TRIGGER_EVENT_SPECIFIC_EXT_BASE + 1L)                            
#define AG1000NI_VAL_TV_EVENT_ODD_FIELD   (IVISCOPE_VAL_TV_TRIGGER_EVENT_SPECIFIC_EXT_BASE + 2L)                            
#define AG1000NI_VAL_TV_EVENT_EVEN_FIELD  (IVISCOPE_VAL_TV_TRIGGER_EVENT_SPECIFIC_EXT_BASE + 3L)                            

/*- Defined values for attribute AG1000NI_ATTR_TV_TRIGGER_POLARITY -*/
#define AG1000NI_VAL_TV_POSITIVE IVISCOPE_VAL_TV_POSITIVE  
#define AG1000NI_VAL_TV_NEGATIVE IVISCOPE_VAL_TV_NEGATIVE  

/*- Defined values for attribute AG1000NI_ATTR_TV_TRIGGER_SIGNAL_FORMAT -*/
#define AG1000NI_VAL_NTSC     IVISCOPE_VAL_NTSC                                                 
#define AG1000NI_VAL_PALSECAM (IVISCOPE_VAL_TV_SIGNAL_FORMAT_SPECIFIC_EXT_BASE + 1L)            

/*- Defined values for attribute AG1000NI_ATTR_SAMPLE_MODE -*/
#define AG1000NI_VAL_REAL_TIME IVISCOPE_VAL_REAL_TIME  

/*- Defined values for attribute AG1000NI_ATTR_TRIGGER_MODIFIER -*/
/*- Defined values for attribute AG1000NI_ATTR_INSTR_EDGE_TRIGGER_MODIFIER -*/
/*- Defined values for attribute AG1000NI_ATTR_INSTR_TV_TRIGGER_MODIFIER -*/
/*- Defined values for attribute AG1000NI_ATTR_INSTR_PATTERN_TRIGGER_MODIFIER -*/
/*- Defined values for attribute AG1000NI_ATTR_INSTR_PULSE_TRIGGER_MODIFIER -*/
#define AG1000NI_VAL_NO_TRIGGER_MOD IVISCOPE_VAL_NO_TRIGGER_MOD     
#define AG1000NI_VAL_AUTO           IVISCOPE_VAL_AUTO               
#define AG1000NI_VAL_SINGLE         (1001L)                         

/* Instrument specific attribute value definitions */

/*- Defined values for attribute AG1000NI_ATTR_PULSE_TRIGGER_MODE -*/
#define AG1000NI_VAL_PULSE_TRIGGER_GT_POSITIVE (1L)  
#define AG1000NI_VAL_PULSE_TRIGGER_LT_POSITIVE (2L)  
#define AG1000NI_VAL_PULSE_TRIGGER_GT_NEGATIVE (3L)  
#define AG1000NI_VAL_PULSE_TRIGGER_LT_NEGATIVE (4L)  

/*- Defined values for attribute AG1000NI_ATTR_ALTERNATION_TRIGGER_SOURCE -*/
#define AG1000NI_VAL_CH1CH2 (1L)        
#define AG1000NI_VAL_CH1CH3 (2L)        
#define AG1000NI_VAL_CH1CH4 (3L)        
#define AG1000NI_VAL_CH2CH3 (4L)        
#define AG1000NI_VAL_CH2CH4 (5L)        
#define AG1000NI_VAL_CH3CH4 (6L)                

/*- Defined values for attribute AG1000NI_ATTR_ALTERNATION_TRIGGER_TYPE -*/
#define AG1000NI_VAL_ALTERNATION_EDGE_TRIGGER  (1L)  
#define AG1000NI_VAL_ALTERNATION_TV_TRIGGER    (2L)  
#define AG1000NI_VAL_ALTERNATION_PULSE_TRIGGER (3L)  

/*- Defined values for Instrument Models -*/   
#define AG1000NI_VAL_DSO1002A 1002  
#define AG1000NI_VAL_DSO1004A 1004  
#define AG1000NI_VAL_DSO1012A 1012  
#define AG1000NI_VAL_DSO1014A 1014  
#define AG1000NI_VAL_DSO1022A 1022  
#define AG1000NI_VAL_DSO1024A 1024  

/****************************************************************************
*---------------- Instrument Driver Function Declarations -----------------*
****************************************************************************/

/*- Init and Close Functions -------------------------------------------*/
ViStatus _VI_FUNC  ag1000ni_init (ViRsrc resourceName, ViBoolean IDQuery,
                                  ViBoolean resetDevice, ViSession *vi);
ViStatus _VI_FUNC  ag1000ni_InitWithOptions (ViRsrc resourceName, ViBoolean IDQuery,
                                             ViBoolean resetDevice, ViConstString optionString, 
                                             ViSession *newVi);
ViStatus _VI_FUNC  ag1000ni_close (ViSession vi);   

    /*- Coercion Info Functions --------------------------------------------*/
ViStatus _VI_FUNC  ag1000ni_GetNextCoercionRecord (ViSession vi,
                                                   ViInt32 bufferSize,
                                                   ViChar record[]);

    /*- Locking Functions --------------------------------------------------*/
ViStatus _VI_FUNC  ag1000ni_LockSession (ViSession vi, ViBoolean *callerHasLock);   
ViStatus _VI_FUNC  ag1000ni_UnlockSession (ViSession vi, ViBoolean *callerHasLock);
 /*- Channel Info Functions ---------------------------------------------*/
ViStatus _VI_FUNC  ag1000ni_GetChannelName (ViSession vi, ViInt32 index, 
                                   ViInt32 bufferSize, ViChar name[]);
/*- Configuration -> Acquisition Group Functions ----------------------------------------------------*/
ViStatus _VI_FUNC ag1000ni_ConfigureAcquisitionRecord(ViSession vi, ViReal64 TimePerRecord, ViInt32 MinNumPts, 
                  ViReal64 AcquisitionStartTime);
ViStatus _VI_FUNC ag1000ni_ConfigureAcquisitionType(ViSession vi, ViInt32 AcquisitionType);
ViStatus _VI_FUNC ag1000ni_ConfigureInitiateContinuous(ViSession vi, ViBoolean ContinuousAcquisition);
ViStatus _VI_FUNC ag1000ni_ConfigureNumAverages(ViSession vi, 
                                                      ViInt32 NumberOfAverages);          

/*- Configuration -> Channel Group Functions ----------------------------------------------------*/
ViStatus _VI_FUNC ag1000ni_ConfigureChannel(ViSession vi, ViConstString Channel, ViReal64 Range, 
                  ViReal64 Offset, ViInt32 Coupling, ViReal64 ProbeAttenuation, 
                  ViBoolean Enabled);
ViStatus _VI_FUNC ag1000ni_ConfigureChanCharacteristics(ViSession vi, ViConstString Channel, ViReal64 InputImpedance, 
                  ViReal64 MaxInputFrequency);
ViStatus _VI_FUNC ag1000ni_ConfigureChanAdjustment(ViSession vi, ViConstString Channel, ViBoolean InputInverted, 
                  ViBoolean DigitalFilterEnabled);

/*- Configuration -> Configuration Information Group Functions ----------------------------------------------------*/
ViStatus _VI_FUNC ag1000ni_ActualRecordLength(ViSession vi, ViInt32* ActualRecordLength);
ViStatus _VI_FUNC ag1000ni_SampleMode(ViSession vi, ViInt32* SampleMode);
ViStatus _VI_FUNC ag1000ni_SampleRate(ViSession vi, ViReal64* SampleRate);

/*- Configuration -> Trigger Group Functions ----------------------------------------------------*/
ViStatus _VI_FUNC ag1000ni_ConfigureTrigger(ViSession vi, ViInt32 TriggerType, ViReal64 Holdoff);
ViStatus _VI_FUNC ag1000ni_ConfigureTriggerCoupling(ViSession vi, ViInt32 Coupling);
ViStatus _VI_FUNC ag1000ni_ConfigureTriggerModifier(ViSession vi, ViInt32 TriggerModifier);
ViStatus _VI_FUNC ag1000ni_ConfigureTriggerCharacteristics(ViSession vi, ViReal64 Sensitivity, ViBoolean HFRejectFilterEnabled);
ViStatus _VI_FUNC ag1000ni_ConfigureEdgeTriggerSource(ViSession vi, ViConstString Source, ViReal64 Level, 
                  ViInt32 Slope);
ViStatus _VI_FUNC ag1000ni_ConfigureTVTriggerSource(ViSession vi, ViConstString Source, ViInt32 TVSignalFormat, 
                  ViInt32 TVEvent, ViInt32 TVPolarity);
ViStatus _VI_FUNC ag1000ni_ConfigureTVTriggerLineNumber(ViSession vi, ViInt32 TVLineNumber);
ViStatus _VI_FUNC ag1000ni_ConfigureTVTriggerCharacteristics(ViSession vi, ViReal64 TVTriggerLevel);
ViStatus _VI_FUNC ag1000ni_ConfigurePatternTriggerSource(ViSession vi, ViConstString Source, ViReal64 PatternTriggerLevel, 
                  ViConstString PatternTriggerPattern);
ViStatus _VI_FUNC ag1000ni_ConfigurePulseTriggerSource(ViSession vi, ViConstString Source, ViInt32 PulseTriggerMode, 
                  ViReal64 PulseTriggerLevel, ViReal64 PulseTriggerWidth);
ViStatus _VI_FUNC ag1000ni_ConfigureAlternationTrigger(ViSession vi, ViInt32 Source, ViConstString CurrentSource, 
                  ViInt32 AltTriggerType, ViInt32 TriggerCoupling, ViReal64 TriggerHoldoff, 
                  ViReal64 TriggerSensitivity);
ViStatus _VI_FUNC ag1000ni_ConfigureAlternationEdgeTrigger(ViSession vi, ViConstString CurrentSource, ViReal64 Level, ViInt32 Slope);
ViStatus _VI_FUNC ag1000ni_ConfigureAlternationPulseTrigger(ViSession vi, ViConstString CurrentSource, ViInt32 Mode, ViReal64 Width);
ViStatus _VI_FUNC ag1000ni_ConfigureAlternationTVTrigger(ViSession vi, ViConstString CurrentSource, ViInt32 SignalFormat, ViInt32 Event, 
                  ViInt32 Polarity, ViInt32 LineNumber);  
ViStatus _VI_FUNC ag1000ni_ConfigureAlternationTriggerAcquisitionRecord (ViSession vi,
                                                                         ViReal64 timePerRecord_seconds,
                                                                         ViReal64 acquisitionStartTime_second);

/*- Configuration -> Measurements Group Functions ----------------------------------------------------*/
ViStatus _VI_FUNC ag1000ni_ConfigureRefLevels(ViSession vi, ViReal64 Low, ViReal64 Mid, ViReal64 High);

/*- Configuration Group Functions ----------------------------------------------------*/
ViStatus _VI_FUNC ag1000ni_AutoSetup(ViSession vi);

/*- Waveform Acquisition Group Functions ----------------------------------------------------*/
ViStatus _VI_FUNC ag1000ni_ReadWaveform(ViSession vi, ViConstString Channel, ViInt32 WaveformSize, 
                  ViInt32 MaxTimeMilliseconds, ViReal64 WaveformArray[], ViInt32* ActualPoints, 
                  ViReal64* InitialX, ViReal64* XIncrement);
ViStatus _VI_FUNC ag1000ni_ReadMinMaxWaveform(ViSession vi, ViConstString Channel, ViInt32 WaveformSize, 
                  ViInt32 MaxTimeMilliseconds, ViReal64 MinWaveform[], ViReal64 MaxWaveform[], 
                  ViInt32* ActualPoints, ViReal64* InitialX, ViReal64* XIncrement);
ViStatus _VI_FUNC ag1000ni_ReadWaveformMeasurement(ViSession vi, ViConstString Channel, ViInt32 MeasFunction, 
                  ViInt32 MaxTimeMilliseconds, ViReal64* Measurement);
ViStatus _VI_FUNC ag1000ni_ReadDelayMeasurement(ViSession vi, ViConstString Source1, ViConstString Source2, 
                  ViInt32 Direction, ViInt32 MaxTimeMilliSeconds, ViReal64* Measurement);
ViStatus _VI_FUNC ag1000ni_ReadPhaseMeasurement(ViSession vi, ViConstString Source1, ViConstString Source2, 
                  ViInt32 MeasFunc, ViInt32 MaxTimeMilliSeconds, ViReal64* Measurement);

/*- Waveform Acquisition -> Low-level Acquisition Group Functions ----------------------------------------------------*/
ViStatus _VI_FUNC ag1000ni_InitiateAcquisition(ViSession vi);
ViStatus _VI_FUNC ag1000ni_AcquisitionStatus(ViSession vi, ViInt32* Status);
ViStatus _VI_FUNC ag1000ni_FetchWaveform(ViSession vi, ViConstString Channel, ViInt32 WaveformSize, 
                  ViReal64 WaveformArray[], ViInt32* ActualPoints, ViReal64* InitialX, 
                  ViReal64* XIncrement);
ViStatus _VI_FUNC ag1000ni_FetchMinMaxWaveform(ViSession vi, ViConstString Channel, ViInt32 WaveformSize, 
                  ViReal64 MinWaveform[], ViReal64 MaxWaveform[], ViInt32* ActualPoints, 
                  ViReal64* InitialX, ViReal64* XIncrement);
ViStatus _VI_FUNC ag1000ni_FetchWaveformMeasurement(ViSession vi, ViConstString Channel, ViInt32 MeasFunction, 
                  ViReal64* Measurement);
ViStatus _VI_FUNC ag1000ni_FetchDelayMeasurement(ViSession vi, ViConstString Source1, ViConstString Source2, 
                  ViInt32 Direction, ViReal64* Measurement);
ViStatus _VI_FUNC ag1000ni_FetchPhaseMeasurement(ViSession vi, ViConstString Source1, ViConstString Source2, 
                  ViInt32 Direction, ViReal64* Measurement);
ViStatus _VI_FUNC ag1000ni_Abort(ViSession vi);

    /*- Error Functions ----------------------------------------------------*/
ViStatus _VI_FUNC  ag1000ni_error_query (ViSession vi, ViInt32 *errorCode,
                                         ViChar errorMessage[]);

ViStatus _VI_FUNC  ag1000ni_GetError (ViSession vi, ViStatus *code, 
                                      ViInt32 bufferSize, ViChar description[]);
ViStatus _VI_FUNC  ag1000ni_ClearError (ViSession vi);

ViStatus _VI_FUNC  ag1000ni_error_message (ViSession vi, ViStatus errorCode,
                                           ViChar errorMessage[256]);

    /*- Interchangeability Checking Functions ------------------------------*/
ViStatus _VI_FUNC ag1000ni_GetNextInterchangeWarning (ViSession vi, 
                                                      ViInt32 bufferSize, 
                                                      ViChar warnString[]);
ViStatus _VI_FUNC ag1000ni_ResetInterchangeCheck (ViSession vi);
ViStatus _VI_FUNC ag1000ni_ClearInterchangeWarnings (ViSession vi);

ViStatus _VI_FUNC ag1000ni_IsInvalidWfmElement (ViSession vi,
                                       ViReal64 elementValue,
                                       ViBoolean *isInvalid);

    /*- Utility Functions --------------------------------------------------*/
ViStatus _VI_FUNC ag1000ni_InvalidateAllAttributes (ViSession vi);
ViStatus _VI_FUNC  ag1000ni_reset (ViSession vi);
ViStatus _VI_FUNC  ag1000ni_ResetWithDefaults (ViSession vi);
ViStatus _VI_FUNC  ag1000ni_self_test (ViSession vi, ViInt16 *selfTestResult,
                                       ViChar selfTestMessage[]);
ViStatus _VI_FUNC  ag1000ni_revision_query (ViSession vi, 
                                            ViChar instrumentDriverRevision[],
                                            ViChar firmwareRevision[]);
ViStatus _VI_FUNC  ag1000ni_Disable (ViSession vi);
ViStatus _VI_FUNC  ag1000ni_WriteInstrData (ViSession vi, ViConstString writeBuffer); 
ViStatus _VI_FUNC  ag1000ni_ReadInstrData  (ViSession vi, ViInt32 numBytes, 
                                            ViChar rdBuf[], ViInt32 *bytesRead);

    /*- Set, Get, and Check Attribute Functions ----------------------------*/
ViStatus _VI_FUNC  ag1000ni_GetAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 *value);
ViStatus _VI_FUNC  ag1000ni_GetAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 *value);
ViStatus _VI_FUNC  ag1000ni_GetAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 bufSize, ViChar value[]); 
ViStatus _VI_FUNC  ag1000ni_GetAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession *value);
ViStatus _VI_FUNC  ag1000ni_GetAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean *value);

ViStatus _VI_FUNC  ag1000ni_SetAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 value);
ViStatus _VI_FUNC  ag1000ni_SetAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 value);
ViStatus _VI_FUNC  ag1000ni_SetAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViConstString value); 
ViStatus _VI_FUNC  ag1000ni_SetAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession value);
ViStatus _VI_FUNC  ag1000ni_SetAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean value);

ViStatus _VI_FUNC  ag1000ni_CheckAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 value);
ViStatus _VI_FUNC  ag1000ni_CheckAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 value);
ViStatus _VI_FUNC  ag1000ni_CheckAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViConstString value); 
ViStatus _VI_FUNC  ag1000ni_CheckAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession value);
ViStatus _VI_FUNC  ag1000ni_CheckAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean value);

ViStatus _VI_FUNC  ag1000ni_WaitForOPC (ViSession vi, ViInt32 maxTime);

/****************************************************************************
 *------------------------ Error And Completion Codes ----------------------*
 ****************************************************************************/

#define AG1000NI_ERROR_CHANNEL_NOT_ENABLED             IVISCOPE_ERROR_CHANNEL_NOT_ENABLED              
#define AG1000NI_ERROR_UNABLE_TO_PERFORM_MEASUREMENT   IVISCOPE_ERROR_UNABLE_TO_PERFORM_MEASUREMENT    
#define AG1000NI_ERROR_MAX_TIME_EXCEEDED               IVISCOPE_ERROR_MAX_TIME_EXCEEDED

/*- Obsolete Inherent Instrument Attributes and functions -*/
    /*- These attributes and functions have been deprecated and may not
        be supported in future versions of this driver. 
    */ 
ViStatus _VI_FUNC ag1000ni_ConfigureNumberOfAverages(ViSession vi, 
                                                      ViInt32 NumberOfAverages);
/**************************************************************************** 
 *---------------------------- End Include File ----------------------------* 
 ****************************************************************************/
#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif
#endif /* __AG1000NI_HEADER */  
