/*= Hewlett Packard 8648A Signal Generator ==================================*/

/*===========================================================================*/
/*  Please do not use global variables or arrays in the include file of      */
/*  instrument drivers that will be submitted for inclusion into the         */
/*  LabWindows Instrument Driver Library.                                    */
/*===========================================================================*/
     
#ifndef __hp8648a_HEADER
#define __hp8648a_HEADER

#include <vpptype.h>

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

/*****************************************************************************/
/*= Define Instrument Specific Error Codes Here =============================*/
/*****************************************************************************/
#define VI_INSTR_ERROR_OFFSET           (_VI_ERROR+0x3FFC0900L)
#define VI_ERROR_FILE_OPEN              (_VI_ERROR+0x3FFC0800L)
#define VI_ERROR_FILE_WRITE             (_VI_ERROR+0x3FFC0801L)
#define VI_ERROR_INTERPRETING_RESPONSE  (_VI_ERROR+0x3FFC0803L)
#define VI_ERROR_NO_DC                  (_VI_ERROR+0x3FFC0900L) 

/*****************************************************************************/
/*= GLOBAL USER-CALLABLE FUNCTION DECLARATIONS (Exportable Functions) =======*/
/*****************************************************************************/
ViStatus _VI_FUNC hp8648a_init (ViRsrc resourceName, ViBoolean IDQuery,
                                ViBoolean resetDevice, ViPSession instrumentHandle);
ViStatus _VI_FUNC hp8648a_output_signal (ViSession instrumentHandle,
                                         ViReal64 RFFrequencyMHz,
                                         ViReal64 RFAmplitudedBm,
                                         ViInt16 modulation,
                                         ViReal64 modulationLevel,
                                         ViInt16 modulatingSignal,
                                         ViBoolean internalFrequency,
                                         ViBoolean externalCoupling);
ViStatus _VI_FUNC hp8648a_rf_signal (ViSession instrumentHandle,
                                     ViReal64 RFFrequencyMHz,
                                     ViBoolean amplitudeReference,
                                     ViReal64 referenceValuedBm,
                                     ViReal64 RFAmplitudedBm,
                                     ViBoolean automaticAttenuator);
ViStatus _VI_FUNC hp8648a_modulation (ViSession instrumentHandle,
                                      ViInt16 modulation, ViReal64 modulationLevel,
                                      ViInt16 modulatingSignal,
                                      ViBoolean internalFrequency,
                                      ViBoolean externalCoupling);
ViStatus _VI_FUNC hp8648a_set_ref_att (ViSession instrumentHandle,
                                       ViBoolean amplitudeReference,
                                       ViBoolean automaticAttenuator);
ViStatus _VI_FUNC hp8648a_mod_on_off (ViSession instrumentHandle,
                                      ViInt16 selectModulation,
                                      ViBoolean modulation);
ViStatus _VI_FUNC hp8648a_gen_signl (ViSession instrumentHandle,
                                     ViBoolean modulationOFF,
                                     ViBoolean RFOutputSignal);
ViStatus _VI_FUNC hp8648a_writeInstrData (ViSession instrumentHandle,
                                          ViString writeBuffer);
ViStatus _VI_FUNC hp8648a_readInstrData (ViSession instrumentHandle,
                                         ViInt16 numberBytesToRead,
                                         ViChar _VI_FAR readBuffer[],
                                         ViPInt32 numBytesRead);
ViStatus _VI_FUNC hp8648a_reset (ViSession instrumentHandle);
ViStatus _VI_FUNC hp8648a_selfTest (ViSession instrumentHandle,
                                    ViPInt16 selfTestResult,
                                    ViChar _VI_FAR selfTestMessage[]);
ViStatus _VI_FUNC hp8648a_errorQuery (ViSession instrumentHandle,
                                      ViPInt32 errorCode,
                                      ViChar _VI_FAR errorMessage[]);
ViStatus _VI_FUNC hp8648a_errorMessage (ViSession instrumentHandle,
                                        ViStatus errorCode,
                                        ViChar _VI_FAR errorMessage[]);
ViStatus _VI_FUNC hp8648a_revisionQuery (ViSession instrumentHandle,
                                         ViChar _VI_FAR instrumentDriverRevision[],
                                         ViChar _VI_FAR firmwareRevision[]);
ViStatus _VI_FUNC hp8648a_det_mod_set (ViInt16 selectedModulation,
                                       ViReal64 desiredModulValue,
                                       ViReal64 extModSignalLevel,
                                       ViPReal64 valueToSetModul);
ViStatus _VI_FUNC hp8648a_close (ViSession instrumentHandle);


#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

/*****************************************************************************/
/*=== END INCLUDE FILE ======================================================*/
/*****************************************************************************/

#endif
